/*
 * SegaSonic The Hedgehog — Android Port
 * Native SDL2 + OpenGL ES 3.0/2.0 (ETC2/EAC + PBO) + Multi-Touch Game Host
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 * Builds into two isolated native libraries loaded in `:game` process:
 *   - libsegasonic_console.so  (compiled with -DSEGASONIC_CONSOLE)
 *   - libsegasonic_arcade.so   (compiled without SEGASONIC_CONSOLE)
 */

#include "s32/machine.h"
#include "s32/program.h"
#include "s32/video.h"
#include "game/data.h"
#include "sound/chips.h"
#include "settings.h"

#ifdef SEGASONIC_CONSOLE
#include "console.h"
#include "actions.h"
#define FILE_NAME    "segasonic"
#define WINDOW_TITLE "SegaSonic The Hedgehog"
#else
#define FILE_NAME    "sonic"
#define WINDOW_TITLE "SegaSonic Arcade"
#endif

#include "etc2.h"
#include "gles_renderer.h"
#include "touch_controls.h"

#include <SDL.h>

#ifdef __ANDROID__
#include <jni.h>
#include <sys/stat.h>
#endif

#include <ctype.h>
#include <math.h>
#include <setjmp.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define PLAYERS            3
#define OUTPUT_RATE        48000
#define QUEUE_FRAMES_MAX   5
#define ASSETS_FILE        "segasonic.assets"
#define WIDE_WIDTH         398
#define TRACKBALL_CAP      120

static const double FM_RATE  = S32_SOUND_CLOCK / 144.0;
static const double PCM_RATE = S32_PCM_CLOCK / 384.0;

/* ---- Runtime & Rendering State ---- */

static SDL_Window *g_window;
static SDL_GLContext g_gl_ctx;
static gles_renderer *g_renderer;
static SDL_AudioDeviceID g_audio;
static SDL_GameController *g_controllers[PLAYERS];

static touch_controls g_touch;
static ui_quad_batch g_ui_batch;
static touch_frame_output g_touch_out;

static jmp_buf g_finished;
static uint64_t g_next_frame;
static bool g_app_in_background;

/* FPS measurement */
static uint64_t g_fps_last_counter;
static int g_fps_frames;
static float g_fps_value = 60.0f;

/* Persistent file paths inside Android filesDir */
static char g_saves_dir[1024];
static char g_cache_dir[1088];
static char g_settings_path[1088];
static char g_eeprom_path[1088];
static char g_touch_ini_path[1088];
static char g_port_ini_path[1088];

typedef struct android_port_cfg
{
	gles_filter_mode filter;
	gles_aspect_mode aspect;
	gles_etc2_mode etc2_mode;
	bool show_fps;
	bool low_latency_audio;
	bool vsync;
} android_port_cfg;

static android_port_cfg g_port_cfg = {
	GLES_FILTER_SHARP_BILINEAR,
	GLES_ASPECT_AUTO,
	GLES_ETC2_AUTO_ATLAS,
	false,
	true,
	true
};

/* Per-frame aggregated hardware controls */
static struct
{
	int roll_x[PLAYERS];
	int roll_y[PLAYERS];
	bool button[PLAYERS];
	bool coin[PLAYERS];
	bool start[PLAYERS];
	bool service;
	bool test;
} g_controls;

/* Fixed-point Q16.16 stereo linear resampler for the 3 System 32 sound chips */
typedef struct fast_resampler
{
	uint32_t step_q16;
	uint32_t pos_q16;
	int32_t *samples;
	size_t count;
	size_t capacity;
} fast_resampler;

static fast_resampler g_chips[3];

#ifdef SEGASONIC_CONSOLE
static bool g_pause_requested;
static bool g_bound_now;
static char g_times_path[1088];
static SDL_Surface *g_prompt_sheet;
static uint32_t g_controls_picture[S32_VIDEO_MAX_WIDTH * S32_VIDEO_HEIGHT];
static uint32_t g_pause_picture[S32_VIDEO_MAX_WIDTH * S32_VIDEO_HEIGHT];
#endif

static const uint32_t *g_last_frame_pixels;
static int g_last_frame_width = 320;

/* ---- Android JNI Bridge (Haptics & Launch Flags) ---- */

#ifdef __ANDROID__
static void android_trigger_haptics(uint32_t mask)
{
	JNIEnv *env;
	jclass cls;
	jmethodID mid;
	int kind = 0;

	if (mask == 0)
		return;
	if (mask & TOUCH_HAPTIC_PRESS)        kind = 1;
	else if (mask & TOUCH_HAPTIC_UI)      kind = 2;
	else if (mask & TOUCH_HAPTIC_RELEASE) kind = 3;
	else if (mask & TOUCH_HAPTIC_TICK)    kind = 4;
	if (kind == 0)
		return;

	env = (JNIEnv *)SDL_AndroidGetJNIEnv();
	if (!env)
		return;
	cls = (*env)->FindClass(env, "io/github/segasonic/port/GameActivity");
	if (!cls)
	{
		(*env)->ExceptionClear(env);
		return;
	}
	mid = (*env)->GetStaticMethodID(env, cls, "vibratePulse", "(I)V");
	if (mid)
		(*env)->CallStaticVoidMethod(env, cls, mid, (jint)kind);
	else
		(*env)->ExceptionClear(env);
	(*env)->DeleteLocalRef(env, cls);
}

static bool android_query_editor_only_launch(void)
{
	JNIEnv *env = (JNIEnv *)SDL_AndroidGetJNIEnv();
	jclass cls;
	jmethodID mid;
	jboolean res = JNI_FALSE;

	if (!env)
		return false;
	cls = (*env)->FindClass(env, "io/github/segasonic/port/GameActivity");
	if (!cls)
	{
		(*env)->ExceptionClear(env);
		return false;
	}
	mid = (*env)->GetStaticMethodID(env, cls, "isLaunchTouchEditorOnly", "()Z");
	if (mid)
		res = (*env)->CallStaticBooleanMethod(env, cls, mid);
	else
		(*env)->ExceptionClear(env);
	(*env)->DeleteLocalRef(env, cls);
	return res == JNI_TRUE;
}
#else
static void android_trigger_haptics(uint32_t mask) { (void)mask; }
static bool android_query_editor_only_launch(void) { return false; }
#endif

/* ---- Android Port INI (`android_port.ini`) ---- */

static char *trim_str(char *s)
{
	char *e;
	while (*s && isspace((unsigned char)*s)) s++;
	e = s + strlen(s);
	while (e > s && isspace((unsigned char)e[-1])) *--e = '\0';
	return s;
}

static void load_port_ini(const char *path)
{
	FILE *fp = fopen(path, "r");
	char line[256];

	if (!fp)
		return;
	while (fgets(line, sizeof(line), fp))
	{
		char *eq = strchr(line, '=');
		char *k, *v;
		if (!eq) continue;
		*eq = '\0';
		k = trim_str(line);
		v = trim_str(eq + 1);
		if (strcmp(k, "filter") == 0)
			g_port_cfg.filter = (gles_filter_mode)(atoi(v) & 3);
		else if (strcmp(k, "aspect") == 0)
			g_port_cfg.aspect = (gles_aspect_mode)(atoi(v) % GLES_ASPECT_COUNT);
		else if (strcmp(k, "etc2_mode") == 0)
			g_port_cfg.etc2_mode = (gles_etc2_mode)(atoi(v) % GLES_ETC2_MODE_COUNT);
		else if (strcmp(k, "show_fps") == 0)
			g_port_cfg.show_fps = atoi(v) != 0;
		else if (strcmp(k, "low_latency_audio") == 0)
			g_port_cfg.low_latency_audio = atoi(v) != 0;
		else if (strcmp(k, "vsync") == 0)
			g_port_cfg.vsync = atoi(v) != 0;
	}
	fclose(fp);
}

static void save_port_ini(const char *path)
{
	FILE *fp = fopen(path, "w");
	if (!fp)
		return;
	fprintf(fp, "filter = %d\n", (int)g_port_cfg.filter);
	fprintf(fp, "aspect = %d\n", (int)g_port_cfg.aspect);
	fprintf(fp, "etc2_mode = %d\n", (int)g_port_cfg.etc2_mode);
	fprintf(fp, "show_fps = %d\n", g_port_cfg.show_fps ? 1 : 0);
	fprintf(fp, "low_latency_audio = %d\n", g_port_cfg.low_latency_audio ? 1 : 0);
	fprintf(fp, "vsync = %d\n", g_port_cfg.vsync ? 1 : 0);
	fclose(fp);
}

/* ---- EEPROM & Console Best Times Persistence ---- */

static void save_eeprom(void)
{
	FILE *fp;
	if (!s32_board.eeprom.dirty || !g_eeprom_path[0])
		return;
	fp = fopen(g_eeprom_path, "wb");
	if (!fp)
		return;
	fwrite(s32_board.eeprom.data, 1, sizeof(s32_board.eeprom.data), fp);
	fclose(fp);
	s32_board.eeprom.dirty = false;
}

static void load_eeprom(void)
{
	FILE *fp = fopen(g_eeprom_path, "rb");
	if (!fp)
		return;
	if (fread(s32_board.eeprom.data, 1, sizeof(s32_board.eeprom.data), fp) != sizeof(s32_board.eeprom.data))
		memset(s32_board.eeprom.data, 0xff, sizeof(s32_board.eeprom.data));
	fclose(fp);
}

#ifdef SEGASONIC_CONSOLE
static void load_times(void)
{
	FILE *fp = fopen(g_times_path, "r");
	int level = 0;
	unsigned long frames = 0;
	char line[128];

	if (!fp)
		return;
	while (fgets(line, sizeof(line), fp))
	{
		if (sscanf(line, " level %d = %lu", &level, &frames) == 2 && level >= 0 && level < CONSOLE_LEVELS)
			console_best_times.levels[level] = (uint32_t)frames;
		else if (sscanf(line, " run %d = %lu", &level, &frames) == 2 && level >= 0 && level < CONSOLE_LEVELS)
			console_best_times.run[level] = (uint32_t)frames;
	}
	fclose(fp);
}

static void save_times(void)
{
	FILE *fp = fopen(g_times_path, "w");
	console_best_times.changed = false;
	if (!fp)
		return;
	fputs("; TIME ATTACK's best times, in the board's frames (60 a second)\n", fp);
	for (int i = 0; i < CONSOLE_LEVELS; i++)
		fprintf(fp, "level %d = %lu\n", i, (unsigned long)console_best_times.levels[i]);
	for (int i = 0; i < CONSOLE_LEVELS; i++)
		fprintf(fp, "run %d = %lu\n", i, (unsigned long)console_best_times.run[i]);
	fclose(fp);
}

static void tell_console_settings(void)
{
	console_window_settings.window_scale = 3;
	console_window_settings.full_screen = true;
	console_window_settings.smooth = (g_port_cfg.filter == GLES_FILTER_SHARP_BILINEAR);
	console_window_settings.bilinear = (g_port_cfg.filter == GLES_FILTER_SMOOTH);
	console_window_settings.crt = (g_port_cfg.filter == GLES_FILTER_CRT);
	console_window_settings.wide = settings.wide;
	console_window_settings.volume = (int)lround(settings.volume * 5.0);
	console_window_settings.title_music = (int)settings.title_music;
	console_window_settings.continues = (int)settings.continues;
	console_window_settings.changed = false;
}
#endif

static void flush_all_persistent_state(void)
{
	save_eeprom();
#ifdef SEGASONIC_CONSOLE
	if (console_best_times.changed)
		save_times();
#endif
	settings_save();
	save_port_ini(g_port_ini_path);
	touch_controls_save_ini(&g_touch, g_touch_ini_path);
}

/* ---- Console Input Prompts Sheet (OPTIONS > CONTROLS) ---- */

#ifdef SEGASONIC_CONSOLE
typedef struct
{
	int what;
	uint8_t row, column, tiles;
} prompt;

static const prompt k_button_prompts[] = {
	{ SDL_CONTROLLER_BUTTON_A,             0, 4, 1 },
	{ SDL_CONTROLLER_BUTTON_B,             0, 5, 1 },
	{ SDL_CONTROLLER_BUTTON_X,             0, 6, 1 },
	{ SDL_CONTROLLER_BUTTON_Y,             0, 7, 1 },
	{ SDL_CONTROLLER_BUTTON_DPAD_UP,       1, 1, 1 },
	{ SDL_CONTROLLER_BUTTON_DPAD_RIGHT,    1, 2, 1 },
	{ SDL_CONTROLLER_BUTTON_DPAD_DOWN,     1, 3, 1 },
	{ SDL_CONTROLLER_BUTTON_DPAD_LEFT,     1, 4, 1 },
	{ SDL_CONTROLLER_BUTTON_BACK,         18, 4, 1 },
	{ SDL_CONTROLLER_BUTTON_START,        18, 5, 1 },
	{ SDL_CONTROLLER_BUTTON_LEFTSHOULDER, 18, 7, 1 },
	{ SDL_CONTROLLER_BUTTON_RIGHTSHOULDER,18, 8, 1 }
};

#define PROMPT_SIZE  16
#define PROMPT_CLEAR 0xffff00ffu

static void load_prompts_asset(void)
{
	SDL_Surface *loaded = SDL_LoadBMP("input_prompts.bmp");
	if (!loaded)
		loaded = SDL_LoadBMP("prompts/input_prompts.bmp");
	if (!loaded)
		return;
	g_prompt_sheet = SDL_ConvertSurfaceFormat(loaded, SDL_PIXELFORMAT_ARGB8888, 0);
	SDL_FreeSurface(loaded);
}

static const prompt *find_prompt(const prompt *list, size_t count, int what)
{
	for (size_t i = 0; i < count; i++)
		if (list[i].what == what)
			return &list[i];
	return NULL;
}

static void draw_prompt(const prompt *icon, int x, int y)
{
	const uint32_t *sheet = (const uint32_t *)g_prompt_sheet->pixels;
	const int pitch = g_prompt_sheet->pitch / 4;

	for (int row = 0; row < PROMPT_SIZE; row++)
	{
		for (int col = 0; col < PROMPT_SIZE * icon->tiles; col++)
		{
			const uint32_t color = sheet[(icon->row * PROMPT_SIZE + row) * pitch + icon->column * PROMPT_SIZE + col];
			if ((color | 0xff000000u) == PROMPT_CLEAR || y + row < 0 || y + row >= S32_VIDEO_HEIGHT
				|| x + col < 0 || x + col >= S32_VIDEO_MAX_WIDTH)
				continue;
			g_controls_picture[(y + row) * S32_VIDEO_MAX_WIDTH + x + col] = color | 0xff000000u;
		}
	}
}

static const uint32_t *with_controls(const uint32_t *pixels)
{
	const action_binding *bindings = action_bindings[console_controls_player()];

	memcpy(g_controls_picture, pixels, sizeof(g_controls_picture));
	for (int action = 0; action < ACTIONS; action++)
	{
		const int y = console_controls_row(action) * 8 - 4;
		const prompt *icon = find_prompt(k_button_prompts, SDL_arraysize(k_button_prompts), bindings[action].buttons[0]);
		if (icon)
			draw_prompt(icon, s32_video_side() + CONSOLE_CONTROLS_BUTTON_X, y);
	}
	return g_controls_picture;
}
#endif

/* ---- Video Presentation & Touch Overlay Sync ---- */

static int picture_width(void)
{
#ifdef SEGASONIC_CONSOLE
	if (settings.wide)
		return WIDE_WIDTH;
#endif
	return 320;
}

static void sync_hud_status_labels(void)
{
	gles_renderer_info info;
	bool wide_now = false;

#ifdef SEGASONIC_CONSOLE
	wide_now = settings.wide;
#endif
	gles_renderer_get_info(g_renderer, &info);
	touch_controls_set_status_info(&g_touch,
		gles_renderer_filter_name(g_port_cfg.filter),
		gles_renderer_aspect_name(g_port_cfg.aspect, wide_now),
		info.summary,
		g_port_cfg.show_fps,
		g_fps_value);
}

static void present_frame_and_hud(const uint32_t *pixels, int width)
{
	int vp_w = 0, vp_h = 0;
	int vis_w = width;

	if (!g_window || !g_renderer || g_app_in_background)
		return;

	SDL_GL_GetDrawableSize(g_window, &vp_w, &vp_h);
	if (vp_w <= 0 || vp_h <= 0)
		SDL_GetWindowSize(g_window, &vp_w, &vp_h);
	touch_controls_set_viewport(&g_touch, vp_w, vp_h);

	if (vis_w > picture_width())
		vis_w = picture_width();

	sync_hud_status_labels();
	touch_controls_build_vertices(&g_touch, &g_ui_batch);

	gles_renderer_draw_frame(g_renderer, pixels, width, vis_w, S32_VIDEO_HEIGHT,
		vp_w, vp_h, &g_ui_batch);
	SDL_GL_SwapWindow(g_window);

	/* Update smoothed FPS counter once every 30 frames */
	g_fps_frames++;
	if (g_fps_frames >= 30)
	{
		const uint64_t now = SDL_GetPerformanceCounter();
		const uint64_t freq = SDL_GetPerformanceFrequency();
		if (g_fps_last_counter != 0 && now > g_fps_last_counter && freq > 0)
			g_fps_value = (float)((double)g_fps_frames * (double)freq / (double)(now - g_fps_last_counter));
		g_fps_last_counter = now;
		g_fps_frames = 0;
	}
}

static void draw_board_frame(void)
{
#ifdef SEGASONIC_CONSOLE
	console_draw_frame();
#else
	s32_video_draw_frame();
#endif
}

static void on_frame_shown(void)
{
	int width = 320;
	const uint32_t *pixels;

	draw_board_frame();
	pixels = s32_video_frame(&width);
#ifdef SEGASONIC_CONSOLE
	if (g_prompt_sheet && console_controls_row(0) >= 0)
		pixels = with_controls(pixels);
#endif
	g_last_frame_pixels = pixels;
	g_last_frame_width = width;
	present_frame_and_hud(pixels, width);
}

/* ---- Event Pumping, Quick Menu Commands & Modal Loops ---- */

static void apply_touch_commands(uint32_t cmds)
{
	if (!cmds)
		return;

	if (cmds & TOUCH_CMD_CYCLE_FILTER)
	{
		g_port_cfg.filter = (gles_filter_mode)(((int)g_port_cfg.filter + 1) % GLES_FILTER_COUNT);
		gles_renderer_set_filter(g_renderer, g_port_cfg.filter);
		settings.smooth   = (g_port_cfg.filter == GLES_FILTER_SHARP_BILINEAR);
		settings.bilinear = (g_port_cfg.filter == GLES_FILTER_SMOOTH);
		settings.crt      = (g_port_cfg.filter == GLES_FILTER_CRT);
#ifdef SEGASONIC_CONSOLE
		tell_console_settings();
#endif
		save_port_ini(g_port_ini_path);
		settings_save();
	}
	if (cmds & TOUCH_CMD_CYCLE_ASPECT)
	{
		g_port_cfg.aspect = (gles_aspect_mode)(((int)g_port_cfg.aspect + 1) % GLES_ASPECT_COUNT);
		gles_renderer_set_aspect(g_renderer, g_port_cfg.aspect);
		save_port_ini(g_port_ini_path);
	}
	if (cmds & TOUCH_CMD_TOGGLE_ETC2_STREAM)
	{
		g_port_cfg.etc2_mode = (gles_etc2_mode)(((int)g_port_cfg.etc2_mode + 1) % GLES_ETC2_MODE_COUNT);
		gles_renderer_set_etc2_mode(g_renderer, g_port_cfg.etc2_mode);
		save_port_ini(g_port_ini_path);
	}
	if (cmds & TOUCH_CMD_TOGGLE_FPS)
	{
		g_port_cfg.show_fps = !g_port_cfg.show_fps;
		save_port_ini(g_port_ini_path);
	}
	if (cmds & TOUCH_CMD_SAVE_CONFIG)
	{
		touch_controls_save_ini(&g_touch, g_touch_ini_path);
		save_port_ini(g_port_ini_path);
	}
	if (cmds & TOUCH_CMD_EXIT_TO_LAUNCHER)
	{
		flush_all_persistent_state();
		longjmp(g_finished, 1);
	}
}

static void pump_sdl_events(void)
{
	SDL_Event ev;

	while (SDL_PollEvent(&ev))
	{
		switch (ev.type)
		{
			case SDL_QUIT:
			case SDL_APP_TERMINATING:
				flush_all_persistent_state();
				longjmp(g_finished, 1);

			case SDL_APP_WILLENTERBACKGROUND:
				g_app_in_background = true;
				touch_controls_reset_fingers(&g_touch);
				if (g_audio)
					SDL_PauseAudioDevice(g_audio, 1);
				flush_all_persistent_state();
				break;

			case SDL_APP_DIDENTERFOREGROUND:
				g_app_in_background = false;
				if (g_audio && !g_touch.quick_menu_open && !g_touch.edit_mode)
				{
					SDL_ClearQueuedAudio(g_audio);
					SDL_PauseAudioDevice(g_audio, 0);
				}
				g_next_frame = SDL_GetPerformanceCounter();
				break;

			case SDL_FINGERDOWN:
				touch_controls_finger_down(&g_touch, (int64_t)ev.tfinger.fingerId, ev.tfinger.x, ev.tfinger.y);
				break;

			case SDL_FINGERMOTION:
				touch_controls_finger_motion(&g_touch, (int64_t)ev.tfinger.fingerId, ev.tfinger.x, ev.tfinger.y);
				break;

			case SDL_FINGERUP:
				touch_controls_finger_up(&g_touch, (int64_t)ev.tfinger.fingerId, ev.tfinger.x, ev.tfinger.y);
				break;

			case SDL_KEYDOWN:
				if (!ev.key.repeat && (ev.key.keysym.scancode == SDL_SCANCODE_AC_BACK ||
				                       ev.key.keysym.scancode == SDL_SCANCODE_ESCAPE))
				{
					touch_controls_on_back_pressed(&g_touch);
					break;
				}
#ifdef SEGASONIC_CONSOLE
				if (!ev.key.repeat && console_controls_waiting() >= 0)
				{
					actions_bind_key(console_controls_player(), console_controls_waiting(), ev.key.keysym.scancode);
					console_controls_bound();
					g_bound_now = true;
				}
#endif
				break;

			case SDL_CONTROLLERBUTTONDOWN:
				touch_controls_notify_gamepad_input(&g_touch);
#ifdef SEGASONIC_CONSOLE
				if (console_controls_waiting() >= 0)
				{
					actions_bind_button(console_controls_player(), console_controls_waiting(),
						(SDL_GameControllerButton)ev.cbutton.button);
					console_controls_bound();
					g_bound_now = true;
				}
#endif
				break;

			case SDL_CONTROLLERAXISMOTION:
				if (abs(ev.caxis.value) > 6000)
					touch_controls_notify_gamepad_input(&g_touch);
				break;

			case SDL_CONTROLLERDEVICEADDED:
				for (int p = 0; p < PLAYERS; p++)
				{
					if (!g_controllers[p])
					{
						g_controllers[p] = SDL_GameControllerOpen(ev.cdevice.which);
						break;
					}
				}
				break;

			case SDL_CONTROLLERDEVICEREMOVED:
				for (int p = 0; p < PLAYERS; p++)
				{
					if (g_controllers[p] &&
					    SDL_JoystickInstanceID(SDL_GameControllerGetJoystick(g_controllers[p])) == ev.cdevice.which)
					{
						SDL_GameControllerClose(g_controllers[p]);
						g_controllers[p] = NULL;
					}
				}
				break;

			default:
				break;
		}
	}
}

/* Modal loop while Quick Menu, Interactive Touch HUD Editor, or App Background is active */
static void run_modal_if_needed(void)
{
	if (!g_touch.quick_menu_open && !g_touch.edit_mode && !g_app_in_background)
		return;

	if (g_audio)
		SDL_PauseAudioDevice(g_audio, 1);

	while (g_touch.quick_menu_open || g_touch.edit_mode || g_app_in_background)
	{
		pump_sdl_events();
		touch_controls_step_frame(&g_touch, &g_touch_out);
		android_trigger_haptics(g_touch_out.haptic_events);
		apply_touch_commands(g_touch_out.commands);

		if (!g_app_in_background && g_last_frame_pixels)
			present_frame_and_hud(g_last_frame_pixels, g_last_frame_width);
		else
			SDL_Delay(16);
	}

	if (g_audio)
	{
		SDL_ClearQueuedAudio(g_audio);
		SDL_PauseAudioDevice(g_audio, 0);
	}
	g_next_frame = SDL_GetPerformanceCounter();
}

/* ---- System 32 Board Controls Callback ---- */

static inline int clamp_trackball_delta(int v)
{
	if (v >  TRACKBALL_CAP) return  TRACKBALL_CAP;
	if (v < -TRACKBALL_CAP) return -TRACKBALL_CAP;
	return v;
}

static void on_controls_update(void)
{
	s32_inputs *input = &s32_board.input;

	pump_sdl_events();
	touch_controls_step_frame(&g_touch, &g_touch_out);
	android_trigger_haptics(g_touch_out.haptic_events);
	apply_touch_commands(g_touch_out.commands);

	if (g_touch_out.modal_paused || g_app_in_background)
	{
		run_modal_if_needed();
		touch_controls_step_frame(&g_touch, &g_touch_out);
		android_trigger_haptics(g_touch_out.haptic_events);
		apply_touch_commands(g_touch_out.commands);
	}

	for (int p = 0; p < PLAYERS; p++)
	{
		g_controls.button[p] = false;
		g_controls.coin[p] = false;
		g_controls.start[p] = false;
	}
	g_controls.service = false;
	g_controls.test = false;

	/*
	 * Apply Player 1 Touch Controls:
	 * Note System 32 uPD4701 quadrature polarity (verified against ports/sdl2/main.c):
	 *   moving RIGHT (track_dx > 0) decrements trackball_x;
	 *   moving DOWN  (track_dy > 0) increments trackball_y.
	 */
	g_controls.roll_x[0] -= g_touch_out.track_dx;
	g_controls.roll_y[0] += g_touch_out.track_dy;
	g_controls.button[0] |= g_touch_out.jump_held;
	g_controls.service   |= g_touch_out.service_held;
	g_controls.test      |= g_touch_out.service_held;

#ifdef SEGASONIC_CONSOLE
	{
		uint32_t t_held = 0;
		uint32_t t_pressed = 0;

		if (g_touch_out.actions_held & TOUCH_ACT_UP)      t_held |= ACTION_BIT(ACTION_UP);
		if (g_touch_out.actions_held & TOUCH_ACT_DOWN)    t_held |= ACTION_BIT(ACTION_DOWN);
		if (g_touch_out.actions_held & TOUCH_ACT_LEFT)    t_held |= ACTION_BIT(ACTION_LEFT);
		if (g_touch_out.actions_held & TOUCH_ACT_RIGHT)   t_held |= ACTION_BIT(ACTION_RIGHT);
		if (g_touch_out.actions_held & TOUCH_ACT_CONFIRM) t_held |= ACTION_BIT(ACTION_ACCEPT);
		if (g_touch_out.actions_held & TOUCH_ACT_BACK)    t_held |= ACTION_BIT(ACTION_BACK);
		if (g_touch_out.actions_held & TOUCH_ACT_PAUSE)   t_held |= ACTION_BIT(ACTION_START);

		if (g_touch_out.actions_pressed & TOUCH_ACT_UP)      t_pressed |= ACTION_BIT(ACTION_UP);
		if (g_touch_out.actions_pressed & TOUCH_ACT_DOWN)    t_pressed |= ACTION_BIT(ACTION_DOWN);
		if (g_touch_out.actions_pressed & TOUCH_ACT_LEFT)    t_pressed |= ACTION_BIT(ACTION_LEFT);
		if (g_touch_out.actions_pressed & TOUCH_ACT_RIGHT)   t_pressed |= ACTION_BIT(ACTION_RIGHT);
		if (g_touch_out.actions_pressed & TOUCH_ACT_CONFIRM) t_pressed |= ACTION_BIT(ACTION_ACCEPT);
		if (g_touch_out.actions_pressed & TOUCH_ACT_BACK)    t_pressed |= ACTION_BIT(ACTION_BACK);
		if (g_touch_out.actions_pressed & TOUCH_ACT_PAUSE)   t_pressed |= ACTION_BIT(ACTION_START);

		actions_read(g_controllers);
		if (console_controls_defaults())
			actions_defaults(console_controls_player());
		if (g_bound_now)
		{
			g_bound_now = false;
			actions_forget_pressed();
		}

		for (int p = 0; p < PLAYERS; p++)
		{
			SDL_GameController *pad = g_controllers[p];
			const uint32_t p_held = actions_player_held(p) | (p == 0 ? t_held : 0u);
			const uint32_t p_pressed = actions_player_pressed(p) | (p == 0 ? t_pressed : 0u);

			g_controls.roll_x[p] += (actions_holds(p, pad, ACTION_LEFT) - actions_holds(p, pad, ACTION_RIGHT))
				* (int)lround(settings.key_roll);
			g_controls.roll_y[p] += (actions_holds(p, pad, ACTION_DOWN) - actions_holds(p, pad, ACTION_UP))
				* (int)lround(settings.key_roll);
			g_controls.button[p] |= actions_holds(p, pad, ACTION_JUMP);

			if (!console_in_game() || console_start_prompted(p))
				g_controls.start[p] |= (p_held & ACTION_BIT(ACTION_START)) != 0;
			else if (p_pressed & (ACTION_BIT(ACTION_START) | ACTION_BIT(ACTION_BACK)))
				g_pause_requested = true;
		}

		if (!console_in_game() && ((actions_pressed() | t_pressed) & ACTION_BIT(ACTION_BACK)))
			console_back();

		console_actions(actions_pressed() | t_pressed, actions_held() | t_held);
		s32_video_set_wide(console_wide_now());
	}
#else
	g_controls.start[0] |= g_touch_out.start_held;
	g_controls.coin[0]  |= g_touch_out.coin_held;
#endif

	/* Physical Bluetooth / USB Gamepad Analog Sticks */
	for (int p = 0; p < PLAYERS; p++)
	{
		SDL_GameController *pad = g_controllers[p];
		if (!pad)
			continue;
		g_controls.roll_x[p] -= (int)lround(SDL_GameControllerGetAxis(pad, SDL_CONTROLLER_AXIS_LEFTX) / 32767.0 * settings.stick_roll);
		g_controls.roll_y[p] += (int)lround(SDL_GameControllerGetAxis(pad, SDL_CONTROLLER_AXIS_LEFTY) / 32767.0 * settings.stick_roll);
#ifndef SEGASONIC_CONSOLE
		g_controls.button[p] |= SDL_GameControllerGetButton(pad, SDL_CONTROLLER_BUTTON_A) != 0;
		g_controls.start[p]  |= SDL_GameControllerGetButton(pad, SDL_CONTROLLER_BUTTON_START) != 0;
		g_controls.coin[p]   |= SDL_GameControllerGetButton(pad, SDL_CONTROLLER_BUTTON_BACK) != 0;
#endif
	}

	/* Active-low System 32 I/O ports + int8-safe trackball counters */
	input->port_a = (uint8_t)(0xff
		& ~(g_controls.button[0] ? 0x01 : 0)
		& ~(g_controls.button[2] ? 0x04 : 0));
	input->port_b = (uint8_t)(0xff
		& ~(g_controls.button[1] ? 0x01 : 0));
	input->port_e = (uint8_t)(0xff
		& ~(g_controls.service  ? 0x01 : 0)
		& ~(g_controls.test     ? 0x02 : 0)
		& ~(g_controls.coin[0]  ? 0x04 : 0)
		& ~(g_controls.coin[1]  ? 0x08 : 0)
		& ~(g_controls.coin[2]  ? 0x40 : 0)
		& ~(g_controls.start[0] ? 0x10 : 0)
		& ~(g_controls.start[1] ? 0x20 : 0)
		& ~(g_controls.start[2] ? 0x80 : 0));

	for (int p = 0; p < PLAYERS; p++)
	{
		const int dx = clamp_trackball_delta(g_controls.roll_x[p]);
		const int dy = clamp_trackball_delta(g_controls.roll_y[p]);
		input->trackball_x[p] = (uint16_t)(input->trackball_x[p] + dx);
		input->trackball_y[p] = (uint16_t)(input->trackball_y[p] + dy);
		g_controls.roll_x[p] = 0;
		g_controls.roll_y[p] = 0;
	}
}

/* ---- Q16.16 Fixed-Point 3-Chip Audio Resampler & Mixer ---- */

static void keep_samples(fast_resampler *chip, const int32_t *samples, size_t count)
{
	if (chip->count + count > chip->capacity)
	{
		chip->capacity = (chip->count + count) * 2 + 256;
		chip->samples = (int32_t *)realloc(chip->samples, chip->capacity * 2 * sizeof(int32_t));
	}
	if (chip->samples && count > 0)
	{
		memcpy(chip->samples + chip->count * 2, samples, count * 2 * sizeof(int32_t));
		chip->count += count;
	}
}

static inline void sample_at_q16(const fast_resampler *chip, int32_t out[2])
{
	const size_t idx = (size_t)(chip->pos_q16 >> 16);
	const int64_t frac = (int64_t)(chip->pos_q16 & 0xffffu);
	const int32_t *s = chip->samples + idx * 2;

	out[0] = (int32_t)(s[0] + (((int64_t)(s[2] - s[0]) * frac) >> 16));
	out[1] = (int32_t)(s[1] + (((int64_t)(s[3] - s[1]) * frac) >> 16));
}

static void drop_used_q16(fast_resampler *chip)
{
	const size_t used = (size_t)(chip->pos_q16 >> 16);
	if (!used)
		return;
	memmove(chip->samples, chip->samples + used * 2, (chip->count - used) * 2 * sizeof(int32_t));
	chip->count -= used;
	chip->pos_q16 &= 0xffffu;
}

static void play_frame_audio(void)
{
	static int16_t mix[OUTPUT_RATE / 20 * 2];
	size_t made = 0;
	/* MAME route weights (0.30 FM0 + 0.30 FM1 + 0.40 PCM) scaled by volume in Q12 */
	const int32_t vol_fm_q12  = (int32_t)(settings.volume * 0.30 * 4096.0 + 0.5);
	const int32_t vol_pcm_q12 = (int32_t)(settings.volume * 0.40 * 4096.0 + 0.5);

	sound_chips_end_frame(s32_board.cycles);
	for (int c = 0; c < 3; c++)
	{
		size_t count = 0;
		const int32_t *samples = sound_chips_frame_samples(c, &count);
		keep_samples(&g_chips[c], samples, count);
	}

	while (made < OUTPUT_RATE / 20)
	{
		int32_t fm0[2], fm1[2], pcm[2];

		if ((size_t)(g_chips[0].pos_q16 >> 16) + 1 >= g_chips[0].count ||
		    (size_t)(g_chips[1].pos_q16 >> 16) + 1 >= g_chips[1].count ||
		    (size_t)(g_chips[2].pos_q16 >> 16) + 1 >= g_chips[2].count)
		{
			break;
		}

		sample_at_q16(&g_chips[0], fm0);
		sample_at_q16(&g_chips[1], fm1);
		sample_at_q16(&g_chips[2], pcm);

		for (int side = 0; side < 2; side++)
		{
			const int64_t acc = (int64_t)(fm0[side] + fm1[side]) * vol_fm_q12
			                  + (int64_t)pcm[side] * vol_pcm_q12;
			int32_t v = (int32_t)(acc >> 12);
			if (v > 32767)       v = 32767;
			else if (v < -32768) v = -32768;
			mix[made * 2 + side] = (int16_t)v;
		}

		g_chips[0].pos_q16 += g_chips[0].step_q16;
		g_chips[1].pos_q16 += g_chips[1].step_q16;
		g_chips[2].pos_q16 += g_chips[2].step_q16;
		made++;
	}

	for (int c = 0; c < 3; c++)
		drop_used_q16(&g_chips[c]);

	if (g_audio && !g_app_in_background &&
	    SDL_GetQueuedAudioSize(g_audio) < (Uint32)(OUTPUT_RATE / 60 * 4 * QUEUE_FRAMES_MAX))
	{
		SDL_QueueAudio(g_audio, mix, (Uint32)(made * 2 * sizeof(int16_t)));
	}
}

/* ---- Console Stage Pause Screen ---- */

#ifdef SEGASONIC_CONSOLE
static void build_pause_picture(int *width)
{
	const uint32_t *pixels;

	draw_board_frame();
	pixels = s32_video_frame(width);
	memcpy(g_pause_picture, pixels, sizeof(g_pause_picture));
	console_pause_text(true);
	draw_board_frame();
	pixels = s32_video_frame(width);

	for (size_t i = 0; i < sizeof(g_pause_picture) / sizeof(g_pause_picture[0]); i++)
	{
		const uint32_t color = g_pause_picture[i];
		if (pixels[i] != color)
		{
			g_pause_picture[i] = pixels[i];
			continue;
		}
		{
			const uint32_t grey = ((((color >> 16) & 0xffu) * 77u +
			                        ((color >> 8)  & 0xffu) * 150u +
			                        ( color        & 0xffu) * 29u) >> 8) * 3u / 4u;
			g_pause_picture[i] = 0xff000000u | (grey << 16) | (grey << 8) | grey;
		}
	}
	console_pause_text(false);
	draw_board_frame();
}

static void run_console_pause(void)
{
	int width = 320;
	bool paused = true;

	g_pause_requested = false;
	if (g_audio)
		SDL_PauseAudioDevice(g_audio, 1);

	build_pause_picture(&width);
	g_last_frame_pixels = g_pause_picture;
	g_last_frame_width = width;
	present_frame_and_hud(g_pause_picture, width);

	while (paused)
	{
		pump_sdl_events();
		touch_controls_step_frame(&g_touch, &g_touch_out);
		android_trigger_haptics(g_touch_out.haptic_events);
		apply_touch_commands(g_touch_out.commands);

		if (g_touch_out.modal_paused || g_app_in_background)
		{
			run_modal_if_needed();
			g_last_frame_pixels = g_pause_picture;
			g_last_frame_width = width;
			continue;
		}

		actions_read(g_controllers);
		if ((actions_pressed() & (ACTION_BIT(ACTION_START) | ACTION_BIT(ACTION_BACK))) ||
		    (g_touch_out.actions_pressed & (TOUCH_ACT_PAUSE | TOUCH_ACT_BACK | TOUCH_ACT_CONFIRM)))
		{
			paused = false;
		}

		present_frame_and_hud(g_pause_picture, width);
	}

	if (g_audio)
	{
		SDL_ClearQueuedAudio(g_audio);
		SDL_PauseAudioDevice(g_audio, 0);
	}
	g_next_frame = SDL_GetPerformanceCounter();
}
#endif

/* ---- Frame Timing & Board Callbacks ---- */

static void wait_for_frame_60hz(void)
{
	const uint64_t freq = SDL_GetPerformanceFrequency();
	uint64_t now = SDL_GetPerformanceCounter();

	g_next_frame += freq / S32_FRAME_RATE;
	if (now > g_next_frame + freq / 4)
		g_next_frame = now;

	while ((now = SDL_GetPerformanceCounter()) < g_next_frame)
	{
		const uint64_t left_ms = (g_next_frame - now) * 1000 / freq;
		if (left_ms > 2)
			SDL_Delay((Uint32)(left_ms - 1));
	}
}

static void on_frame_done(void)
{
	play_frame_audio();

#ifdef SEGASONIC_CONSOLE
	if (console_quit_requested())
	{
		flush_all_persistent_state();
		longjmp(g_finished, 1);
	}
	if (console_window_settings.changed)
	{
		settings.wide = console_window_settings.wide;
		settings.volume = console_window_settings.volume / 5.0;
		settings.title_music = console_window_settings.title_music;
		settings.continues = console_window_settings.continues;
		if (console_window_settings.crt)
			g_port_cfg.filter = GLES_FILTER_CRT;
		else if (console_window_settings.bilinear)
			g_port_cfg.filter = GLES_FILTER_SMOOTH;
		else if (console_window_settings.smooth)
			g_port_cfg.filter = GLES_FILTER_SHARP_BILINEAR;
		else
			g_port_cfg.filter = GLES_FILTER_NEAREST;

		gles_renderer_set_filter(g_renderer, g_port_cfg.filter);
		tell_console_settings();
		settings_save();
		save_port_ini(g_port_ini_path);
	}
	if (console_best_times.changed)
		save_times();
	if (g_pause_requested)
		run_console_pause();
#endif

	if (program_frames() % 600 == 0)
		save_eeprom();

	run_modal_if_needed();
	wait_for_frame_60hz();
}

static void on_irq_changed(bool asserted)
{
	program_irq_line(asserted);
}

static void on_log(const char *message)
{
	(void)message;
}

/* ---- GLES 3.0 -> GLES 2.0 Window/Context Creation ---- */

static bool create_gles_window_and_context(void)
{
	SDL_SetHint(SDL_HINT_ORIENTATIONS, "LandscapeLeft LandscapeRight");

	/* Try OpenGL ES 3.0 first (unlocks hardware ETC2/EAC + PBO async DMA) */
	for (int attempt = 0; attempt < 2; attempt++)
	{
		const int major = (attempt == 0) ? 3 : 2;

		SDL_GL_ResetAttributes();
		SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_ES);
		SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, major);
		SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 0);
		SDL_GL_SetAttribute(SDL_GL_RED_SIZE, 8);
		SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE, 8);
		SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE, 8);
		SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 0);
		SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 0);
		SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);

		g_window = SDL_CreateWindow(WINDOW_TITLE,
			SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
			1280, 720,
			SDL_WINDOW_OPENGL | SDL_WINDOW_FULLSCREEN | SDL_WINDOW_ALLOW_HIGHDPI | SDL_WINDOW_RESIZABLE);

		if (g_window)
		{
			g_gl_ctx = SDL_GL_CreateContext(g_window);
			if (g_gl_ctx)
				break;
			SDL_DestroyWindow(g_window);
			g_window = NULL;
		}
	}

	if (!g_window || !g_gl_ctx)
		return false;

	SDL_GL_MakeCurrent(g_window, g_gl_ctx);
	SDL_GL_SetSwapInterval(g_port_cfg.vsync ? 1 : 0);
	return true;
}

/* ---- Entry Point (`SDL_main` on Android) ---- */

int main(int argc, char **argv)
{
	char assets_path[1088];
	char *pref_dir;
	SDL_AudioSpec wanted;
	game_data_result assets_res;

	(void)argc;
	(void)argv;

	if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO | SDL_INIT_GAMECONTROLLER | SDL_INIT_EVENTS) != 0)
	{
		SDL_Log("SDL_Init failed: %s", SDL_GetError());
		return 1;
	}

	pref_dir = SDL_GetPrefPath("", "SegaSonic");
	snprintf(g_saves_dir, sizeof(g_saves_dir), "%s", pref_dir ? pref_dir : "");
	if (pref_dir)
		SDL_free(pref_dir);

	snprintf(g_cache_dir, sizeof(g_cache_dir), "%setc2_cache", g_saves_dir);
#ifdef __ANDROID__
	mkdir(g_cache_dir, 0770);
#endif

	snprintf(assets_path,      sizeof(assets_path),      "%s" ASSETS_FILE,         g_saves_dir);
	snprintf(g_settings_path,  sizeof(g_settings_path),  "%s" FILE_NAME ".ini",    g_saves_dir);
	snprintf(g_eeprom_path,    sizeof(g_eeprom_path),    "%s" FILE_NAME ".eeprom", g_saves_dir);
	snprintf(g_touch_ini_path, sizeof(g_touch_ini_path), "%stouch_controls.ini",   g_saves_dir);
	snprintf(g_port_ini_path,  sizeof(g_port_ini_path),  "%sandroid_port.ini",     g_saves_dir);

	load_port_ini(g_port_ini_path);

#ifdef SEGASONIC_CONSOLE
	touch_controls_init(&g_touch, true);
#else
	touch_controls_init(&g_touch, false);
#endif
	touch_controls_load_ini(&g_touch, g_touch_ini_path);

#ifdef SEGASONIC_CONSOLE
	{
		char controls_path[1088];
		snprintf(controls_path, sizeof(controls_path), "%scontrols.ini", g_saves_dir);
		actions_open(controls_path);
		load_prompts_asset();
		snprintf(g_times_path, sizeof(g_times_path), "%s" FILE_NAME ".times", g_saves_dir);
		load_times();
	}
#endif

	assets_res = game_data_load(assets_path);
	if (assets_res != GAME_DATA_LOADED)
	{
		SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, WINDOW_TITLE,
			"segasonic.assets could not be loaded. Please import MAME sonic.zip in the Launcher first.", NULL);
		SDL_Quit();
		return 1;
	}

	settings_open(g_settings_path);
#ifdef SEGASONIC_CONSOLE
	console_jingle_program((int)settings.sega_jingle_program, (int)settings.sega_jingle_layer);
	tell_console_settings();
#endif

	if (!create_gles_window_and_context())
	{
		char err_msg[512];
		snprintf(err_msg, sizeof(err_msg), "Failed to create OpenGL ES 3.0 or 2.0 context:\n%s", SDL_GetError());
		SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, WINDOW_TITLE, err_msg, NULL);
		SDL_Quit();
		return 1;
	}

	g_renderer = gles_renderer_create(g_cache_dir, g_port_cfg.etc2_mode);
	if (!g_renderer)
	{
		SDL_GL_DeleteContext(g_gl_ctx);
		SDL_DestroyWindow(g_window);
		SDL_Quit();
		return 1;
	}
	gles_renderer_set_filter(g_renderer, g_port_cfg.filter);
	gles_renderer_set_aspect(g_renderer, g_port_cfg.aspect);

	/* Open low-latency 48 kHz stereo audio stream */
	SDL_zero(wanted);
	wanted.freq = OUTPUT_RATE;
	wanted.format = AUDIO_S16SYS;
	wanted.channels = 2;
	wanted.samples = g_port_cfg.low_latency_audio ? 512 : 1024;
	g_audio = SDL_OpenAudioDevice(NULL, 0, &wanted, NULL, 0);
	if (g_audio)
		SDL_PauseAudioDevice(g_audio, 0);

	g_chips[0].step_q16 = (uint32_t)((FM_RATE  / OUTPUT_RATE) * 65536.0 + 0.5);
	g_chips[1].step_q16 = (uint32_t)((FM_RATE  / OUTPUT_RATE) * 65536.0 + 0.5);
	g_chips[2].step_q16 = (uint32_t)((PCM_RATE / OUTPUT_RATE) * 65536.0 + 0.5);

	/* Attach existing already-connected Bluetooth / USB controllers */
	for (int i = 0, p = 0; i < SDL_NumJoysticks() && p < PLAYERS; i++)
	{
		if (SDL_IsGameController(i))
			g_controllers[p++] = SDL_GameControllerOpen(i);
	}

	/* Initialize System 32 hardware board & V60 / Z80 programs */
	s32_board.irq_changed = on_irq_changed;
	s32_board.controls_update = on_controls_update;
	s32_board.log = on_log;
	s32_board.frame_timing = true;
	s32_board.sound_rom = game_sound;
#ifdef SEGASONIC_CONSOLE
	console_prepare_sprites(game_sprites, GAME_SPRITES_SIZE, game_program);
#endif
	s32_video_init(game_tiles, GAME_TILES_SIZE, game_sprites, GAME_SPRITES_SIZE);
	s32_board.frame_shown = on_frame_shown;
	sound_chips_attach();
	sound_cpu_attach();
	s32_init(game_program);
	load_eeprom();
	program_reset();
	program_frame_done = on_frame_done;
	g_next_frame = SDL_GetPerformanceCounter();
	g_fps_last_counter = g_next_frame;

	/* If launched via "Customize Touch Controls" in Launcher, open interactive editor immediately */
	if (android_query_editor_only_launch())
		touch_controls_set_edit_mode(&g_touch, true, true);

	if (!setjmp(g_finished))
		program_run();

	flush_all_persistent_state();

	for (int p = 0; p < PLAYERS; p++)
	{
		if (g_controllers[p])
			SDL_GameControllerClose(g_controllers[p]);
	}
	for (int c = 0; c < 3; c++)
		free(g_chips[c].samples);
#ifdef SEGASONIC_CONSOLE
	if (g_prompt_sheet)
		SDL_FreeSurface(g_prompt_sheet);
#endif
	if (g_audio)
		SDL_CloseAudioDevice(g_audio);
	if (g_renderer)
		gles_renderer_destroy(g_renderer);
	if (g_gl_ctx)
		SDL_GL_DeleteContext(g_gl_ctx);
	if (g_window)
		SDL_DestroyWindow(g_window);
	SDL_Quit();
	return 0;
}
