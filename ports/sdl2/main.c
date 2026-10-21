/**
 *    Name:        main.c
 *    Description: the game in a window: SDL2 shows its frames, plays its sound, reads the controls and keeps
 *                 the pace, around the same game and board the headless runs check
 *    Version:     0.1.0
 *    Created:     13-10-2026
 *    Modified:    20-10-2026
 *    Author:      Sebastian Toyos
 *
 *    license:BSD-3-Clause
 *    Usage: sonic
 *
 *    The executable carries nothing of the game's: it loads the game's assets from segasonic.assets beside
 *    it (game/data.h; the pack of the assets taken out of the player's ROM set), with SDL2 beside it too.
 *    What it keeps is beside it as well.
 *
 *    The game (src/) and the board (runtime/) run as they are: this file only wraps them. Where the board
 *    has its hooks, the port does its part: when VBLANK starts the video draws the frame and the port puts
 *    it on the screen; when the board reads the controls the port hands it the keyboard, the mouse and the
 *    game controllers; when the game finishes a frame the port takes the frame's samples, mixes them to
 *    48 kHz and queues them, and waits for the frame's time (60 a second) to pass. The EEPROM (the game's
 *    settings, bookkeeping) is kept in sonic.eeprom; the window's own settings (mouse, keys, picture,
 *    volume) in sonic.ini (settings.c), which the game reads again whenever it is saved.
 *
 *    Controls (player 1; MAME's keys where it has them):
 *      mouse, or the arrow keys       trackball (the mouse's ball rolls on when the mouse stops;
 *                                     the right button holds it)
 *      left button, Z or space        the button
 *      5, 6, 7                        coins 1-3        1, 2, 3   starts 1-3
 *      9                              service          F2        test (the operator's menu: 9 moves
 *                                                                  its cursor, F2 picks)
 *      Escape                         quit             F11 or Alt+Enter   full screen
 *      F3                             smooth or sharp pixels
 *      F4                             the mouse's ball rolls on, or stops with the mouse
 *      F5, F6                         the mouse slower, faster (the window's title says how fast)
 *    Game controllers, one per player: the left stick is the trackball, A the button, Start the start,
 *    Back a coin.
 */
#include "s32/machine.h"
#include "s32/program.h"
#include "s32/video.h"
#include "game/data.h"
#include "sound/chips.h"

#include "settings.h"

#include <SDL.h>

#include <math.h>
#include <setjmp.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define PLAYERS            3
#define OUTPUT_RATE        48000
#define QUEUE_FRAMES_MAX   6          /* audio queued beyond this many frames is dropped */

static const double FM_RATE = S32_SOUND_CLOCK / 144.0;
static const double PCM_RATE = S32_PCM_CLOCK / 384.0;

static jmp_buf finished;
static SDL_Window *window;
static SDL_Renderer *renderer;
static SDL_Texture *texture;
static SDL_AudioDeviceID audio;
static SDL_GameController *controllers[PLAYERS];
static double mouse_rest[2];          /* the fractions of a count the mouse has moved and not handed on */
static double hand[2];                /* counts the mouse has moved this frame */
static bool hand_moved;
static double ball[2];                /* counts a frame the ball turns */
static uint64_t next_frame;           /* performance counter value the next frame waits for */
static double shown_scale;            /* the window's scale as it was last set */
static char eeprom_path[1024];

/* What the controls say, gathered between frames */
static struct
{
	int roll_x[PLAYERS];
	int roll_y[PLAYERS];
	bool button[PLAYERS];
	bool coin[PLAYERS];
	bool start[PLAYERS];
	bool service;
	bool test;
} controls;

/* A chip's samples on their way to the output rate: what is left of the last frame and where the next
   output sample falls between two of them */
typedef struct resampler
{
	double step;            /* source samples an output sample */
	double position;        /* of the next output sample, from the first sample kept */
	int32_t *samples;       /* left and right */
	size_t count;           /* samples (pairs) kept */
	size_t capacity;
} resampler;

static resampler chips[3];

/* ---- files ---- */

static uint8_t *load_file(const char *path, size_t *size)
{
	FILE *input = fopen(path, "rb");
	uint8_t *data;
	long length;

	if (!input)
		return NULL;
	fseek(input, 0, SEEK_END);
	length = ftell(input);
	fseek(input, 0, SEEK_SET);
	data = length > 0 ? (uint8_t *)malloc((size_t)length) : NULL;
	if (data && fread(data, 1, (size_t)length, input) != (size_t)length)
	{
		free(data);
		data = NULL;
	}
	fclose(input);
	if (size)
		*size = data ? (size_t)length : 0;
	return data;
}

static void save_eeprom(void)
{
	FILE *output;

	if (!s32_board.eeprom.dirty)
		return;
	output = fopen(eeprom_path, "wb");
	if (!output)
		return;
	fwrite(s32_board.eeprom.data, 1, sizeof(s32_board.eeprom.data), output);
	fclose(output);
	s32_board.eeprom.dirty = false;
}

static void load_eeprom(void)
{
	size_t size;
	uint8_t *data = load_file(eeprom_path, &size);

	if (data && size == sizeof(s32_board.eeprom.data))
		memcpy(s32_board.eeprom.data, data, size);
	free(data);
}

/* ---- the picture ---- */

static void on_frame_shown(void)
{
	int width;
	const uint32_t *pixels;
	SDL_Rect source;

	s32_video_draw_frame();
	pixels = s32_video_frame(&width);
	SDL_UpdateTexture(texture, NULL, pixels, S32_VIDEO_MAX_WIDTH * (int)sizeof(uint32_t));
	source.x = 0;
	source.y = 0;
	source.w = width;
	source.h = S32_VIDEO_HEIGHT;
	SDL_RenderSetLogicalSize(renderer, width, S32_VIDEO_HEIGHT);
	SDL_RenderClear(renderer);
	SDL_RenderCopy(renderer, texture, &source, NULL);
	SDL_RenderPresent(renderer);
}

static void toggle_full_screen(void)
{
	settings.full_screen = (SDL_GetWindowFlags(window) & SDL_WINDOW_FULLSCREEN_DESKTOP) == 0;
	SDL_SetWindowFullscreen(window, settings.full_screen ? SDL_WINDOW_FULLSCREEN_DESKTOP : 0);
	settings_save();
}

static void toggle_smooth(void)
{
	settings.smooth = !settings.smooth;
	SDL_SetTextureScaleMode(texture, settings.smooth ? SDL_ScaleModeLinear : SDL_ScaleModeNearest);
	settings_save();
}

/* ---- the controls ---- */

static void show_mouse_speed(void)
{
	char title[96];

	snprintf(title, sizeof(title), "SegaSonic The Hedgehog - mouse %d%% (F5/F6), ball %s (F4)",
		(int)lround(settings.mouse_speed * 100), settings.ball_rolls_on ? "rolls on" : "stops with the mouse");
	SDL_SetWindowTitle(window, title);
}

static void toggle_ball_rolls_on(void)
{
	settings.ball_rolls_on = !settings.ball_rolls_on;
	show_mouse_speed();
	settings_save();
}

/* The mouse's frame on the trackball. While the hand moves, the ball turns with it; when the hand stops
   (or is lifted to start again), the ball rolls on and slows down, as the arcade's does. The right button
   is the hand resting on the ball: the ball only turns as the mouse moves. */
static void roll_ball(bool hand_on_ball)
{
	if (hand_moved || hand_on_ball || !settings.ball_rolls_on)
	{
		ball[0] = hand[0];
		ball[1] = hand[1];
	}
	else
	{
		const double speed = hypot(ball[0], ball[1]);
		const double slower = speed * settings.ball_drag - settings.ball_friction;
		const double keep = slower > 0.0 ? slower / speed : 0.0;

		ball[0] *= keep;
		ball[1] *= keep;
	}
	for (int axis = 0; axis < 2; axis++)
	{
		int whole;

		mouse_rest[axis] += ball[axis];
		whole = (int)mouse_rest[axis];
		*(axis ? &controls.roll_y[0] : &controls.roll_x[0]) += whole;
		mouse_rest[axis] -= whole;
		hand[axis] = 0.0;
	}
	hand_moved = false;
}

static void change_mouse_speed(bool faster)
{
	settings.mouse_speed *= faster ? 1.25 : 0.8;
	if (settings.mouse_speed < 0.05)
		settings.mouse_speed = 0.05;
	if (settings.mouse_speed > 4.0)
		settings.mouse_speed = 4.0;
	show_mouse_speed();
	settings_save();
}

/* the settings the picture shows, as the file has them after it was read again */
static void apply_settings(void)
{
	const bool full = (SDL_GetWindowFlags(window) & SDL_WINDOW_FULLSCREEN_DESKTOP) != 0;

	SDL_SetTextureScaleMode(texture, settings.smooth ? SDL_ScaleModeLinear : SDL_ScaleModeNearest);
	if (full != settings.full_screen)
		SDL_SetWindowFullscreen(window, settings.full_screen ? SDL_WINDOW_FULLSCREEN_DESKTOP : 0);
	if (settings.window_scale != shown_scale)
	{
		shown_scale = settings.window_scale;
		SDL_SetWindowSize(window, (int)(320 * shown_scale), (int)(S32_VIDEO_HEIGHT * shown_scale));
	}
	show_mouse_speed();
}

static void read_events(void)
{
	SDL_Event event;

	while (SDL_PollEvent(&event))
	{
		switch (event.type)
		{
			case SDL_QUIT:
				longjmp(finished, 1);
			case SDL_MOUSEMOTION:
				/* the board's X counts the other way (MAME reverses it) */
				hand[0] -= event.motion.xrel * settings.mouse_speed;
				hand[1] += event.motion.yrel * settings.mouse_speed;
				hand_moved = true;
				break;
			case SDL_KEYDOWN:
				if (event.key.repeat)
					break;
				if (event.key.keysym.sym == SDLK_ESCAPE)
					longjmp(finished, 1);
				if (event.key.keysym.sym == SDLK_F11
					|| (event.key.keysym.sym == SDLK_RETURN && (event.key.keysym.mod & KMOD_ALT)))
					toggle_full_screen();
				if (event.key.keysym.sym == SDLK_F3)
					toggle_smooth();
				if (event.key.keysym.sym == SDLK_F4)
					toggle_ball_rolls_on();
				if (event.key.keysym.sym == SDLK_F5 || event.key.keysym.sym == SDLK_F6)
					change_mouse_speed(event.key.keysym.sym == SDLK_F6);
				break;
			case SDL_CONTROLLERDEVICEADDED:
				for (int player = 0; player < PLAYERS; player++)
					if (!controllers[player])
					{
						controllers[player] = SDL_GameControllerOpen(event.cdevice.which);
						break;
					}
				break;
			case SDL_CONTROLLERDEVICEREMOVED:
				for (int player = 0; player < PLAYERS; player++)
					if (controllers[player]
						&& SDL_JoystickInstanceID(SDL_GameControllerGetJoystick(controllers[player])) == event.cdevice.which)
					{
						SDL_GameControllerClose(controllers[player]);
						controllers[player] = NULL;
					}
				break;
			default:
				break;
		}
	}
}

/* the state of every control once a frame, where the board reads them */
static void on_controls_update(void)
{
	const Uint8 *keys = SDL_GetKeyboardState(NULL);
	const Uint32 mouse = SDL_GetMouseState(NULL, NULL);
	s32_inputs *input = &s32_board.input;

	read_events();
	roll_ball((mouse & SDL_BUTTON_RMASK) != 0);
	controls.roll_x[0] += (keys[SDL_SCANCODE_LEFT] - keys[SDL_SCANCODE_RIGHT]) * (int)lround(settings.key_roll);
	controls.roll_y[0] += (keys[SDL_SCANCODE_DOWN] - keys[SDL_SCANCODE_UP]) * (int)lround(settings.key_roll);
	controls.button[0] = keys[SDL_SCANCODE_Z] || keys[SDL_SCANCODE_SPACE] || (mouse & SDL_BUTTON_LMASK);
	controls.button[1] = controls.button[2] = false;
	for (int player = 0; player < PLAYERS; player++)
	{
		controls.coin[player] = keys[SDL_SCANCODE_5 + player] != 0;
		controls.start[player] = keys[SDL_SCANCODE_1 + player] != 0;
	}
	controls.service = keys[SDL_SCANCODE_9] != 0;
	controls.test = keys[SDL_SCANCODE_F2] != 0;
	for (int player = 0; player < PLAYERS; player++)
	{
		SDL_GameController *pad = controllers[player];

		if (!pad)
			continue;
		controls.roll_x[player] -= (int)lround(SDL_GameControllerGetAxis(pad, SDL_CONTROLLER_AXIS_LEFTX) / 32767.0 * settings.stick_roll);
		controls.roll_y[player] += (int)lround(SDL_GameControllerGetAxis(pad, SDL_CONTROLLER_AXIS_LEFTY) / 32767.0 * settings.stick_roll);
		controls.button[player] |= SDL_GameControllerGetButton(pad, SDL_CONTROLLER_BUTTON_A) != 0;
		controls.start[player] |= SDL_GameControllerGetButton(pad, SDL_CONTROLLER_BUTTON_START) != 0;
		controls.coin[player] |= SDL_GameControllerGetButton(pad, SDL_CONTROLLER_BUTTON_BACK) != 0;
	}

	/* the board's ports are active low: a cleared bit is a pressed button */
	input->port_a = (uint8_t)(0xff & ~(controls.button[0] ? 0x01 : 0) & ~(controls.button[2] ? 0x04 : 0));
	input->port_b = (uint8_t)(0xff & ~(controls.button[1] ? 0x01 : 0));
	input->port_e = (uint8_t)(0xff
		& ~(controls.service ? 0x01 : 0) & ~(controls.test ? 0x02 : 0)
		& ~(controls.coin[0] ? 0x04 : 0) & ~(controls.coin[1] ? 0x08 : 0) & ~(controls.coin[2] ? 0x40 : 0)
		& ~(controls.start[0] ? 0x10 : 0) & ~(controls.start[1] ? 0x20 : 0) & ~(controls.start[2] ? 0x80 : 0));
	for (int player = 0; player < PLAYERS; player++)
	{
		input->trackball_x[player] = (uint16_t)(input->trackball_x[player] + controls.roll_x[player]);
		input->trackball_y[player] = (uint16_t)(input->trackball_y[player] + controls.roll_y[player]);
		controls.roll_x[player] = controls.roll_y[player] = 0;
	}
}

/* ---- the sound ---- */

static void keep(resampler *chip, const int32_t *samples, size_t count)
{
	if (chip->count + count > chip->capacity)
	{
		chip->capacity = (chip->count + count) * 2;
		chip->samples = (int32_t *)realloc(chip->samples, chip->capacity * 2 * sizeof(int32_t));
	}
	memcpy(chip->samples + chip->count * 2, samples, count * 2 * sizeof(int32_t));
	chip->count += count;
}

/* the chip's output at the next output sample, between the two samples around it */
static void sample_at(const resampler *chip, double position, double out[2])
{
	const size_t index = (size_t)position;
	const double fraction = position - (double)index;

	for (int side = 0; side < 2; side++)
	{
		const double here = chip->samples[index * 2 + side];
		const double next = chip->samples[(index + 1) * 2 + side];
		out[side] = here + (next - here) * fraction;
	}
}

static void drop_used(resampler *chip)
{
	const size_t used = (size_t)chip->position;

	if (!used)
		return;
	memmove(chip->samples, chip->samples + used * 2, (chip->count - used) * 2 * sizeof(int32_t));
	chip->count -= used;
	chip->position -= (double)used;
}

/* the frame's samples of the three chips, mixed as MAME's routes do (each YM3438 at 0.30, the RF5C68
   at 0.40) at 48 kHz, at the settings' volume */
static void play_frame(void)
{
	static int16_t mix[OUTPUT_RATE / 20 * 2];
	size_t made = 0;

	sound_chips_end_frame(s32_board.cycles);
	for (int chip = 0; chip < 3; chip++)
	{
		size_t count;
		const int32_t *samples = sound_chips_frame_samples(chip, &count);
		keep(&chips[chip], samples, count);
	}
	while (made < OUTPUT_RATE / 20)
	{
		double fm0[2], fm1[2], pcm[2];

		if (chips[0].position + 1 >= (double)chips[0].count || chips[1].position + 1 >= (double)chips[1].count
			|| chips[2].position + 1 >= (double)chips[2].count)
			break;
		sample_at(&chips[0], chips[0].position, fm0);
		sample_at(&chips[1], chips[1].position, fm1);
		sample_at(&chips[2], chips[2].position, pcm);
		for (int side = 0; side < 2; side++)
		{
			double value = settings.volume * (0.30 * (fm0[side] + fm1[side]) + 0.40 * pcm[side]);
			mix[made * 2 + side] = (int16_t)(value > 32767 ? 32767 : (value < -32768 ? -32768 : value));
		}
		for (int chip = 0; chip < 3; chip++)
			chips[chip].position += chips[chip].step;
		made++;
	}
	for (int chip = 0; chip < 3; chip++)
		drop_used(&chips[chip]);
	if (audio && SDL_GetQueuedAudioSize(audio) < (Uint32)(OUTPUT_RATE / 60 * 4 * QUEUE_FRAMES_MAX))
		SDL_QueueAudio(audio, mix, (Uint32)(made * 2 * sizeof(int16_t)));
}

/* ---- the frame ---- */

static void wait_for_frame(void)
{
	const uint64_t frequency = SDL_GetPerformanceFrequency();
	uint64_t now = SDL_GetPerformanceCounter();

	next_frame += frequency / S32_FRAME_RATE;
	if (now > next_frame + frequency / 4)
		next_frame = now;    /* far behind (the window was moved, the machine was busy): start over */
	while ((now = SDL_GetPerformanceCounter()) < next_frame)
	{
		const uint64_t left = (next_frame - now) * 1000 / frequency;
		if (left > 2)
			SDL_Delay((Uint32)(left - 1));
	}
}

static void on_frame_done(void)
{
	play_frame();
	if (program_frames() % 600 == 0)
		save_eeprom();
	if (program_frames() % 30 == 0 && settings_reread())
		apply_settings();
	wait_for_frame();
}

static void on_irq_changed(bool asserted)
{
	program_irq_line(asserted);
}

static void on_log(const char *message)
{
	(void)message;
}

/* A window with what went wrong: the port has no console */
static int fail(const char *what, const char *detail)
{
	char message[1200];

	snprintf(message, sizeof(message), "%s\n%s", what, detail);
	fprintf(stderr, "%s\n", message);
	SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "SegaSonic The Hedgehog", message, NULL);
	return 1;
}

int main(int argc, char **argv)
{
	SDL_AudioSpec wanted;
	char settings_path[1024], assets_path[1024];
	char *base;

	(void)argc;
	(void)argv;
	if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO | SDL_INIT_GAMECONTROLLER) != 0)
		return fail("SDL could not start.", SDL_GetError());
	/* what the game keeps goes beside the executable */
	base = SDL_GetBasePath();
	snprintf(settings_path, sizeof(settings_path), "%ssonic.ini", base ? base : "");
	snprintf(eeprom_path, sizeof(eeprom_path), "%ssonic.eeprom", base ? base : "");
	snprintf(assets_path, sizeof(assets_path), "%ssegasonic.assets", base ? base : "");
	SDL_free(base);
	switch (game_data_load(assets_path))
	{
	case GAME_DATA_LOADED:
		break;
	case GAME_DATA_MISSING:
		return fail("The game's assets are missing.", assets_path);
	default:
		return fail("The game's assets cannot be read.", assets_path);
	}
	settings_open(settings_path);
	shown_scale = settings.window_scale;
	window = SDL_CreateWindow("SegaSonic The Hedgehog", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
		(int)(320 * shown_scale), (int)(S32_VIDEO_HEIGHT * shown_scale),
		SDL_WINDOW_RESIZABLE | (settings.full_screen ? SDL_WINDOW_FULLSCREEN_DESKTOP : 0));
	renderer = window ? SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED) : NULL;
	if (window && !renderer)
		renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_SOFTWARE);
	texture = renderer ? SDL_CreateTexture(renderer, SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_STREAMING,
		S32_VIDEO_MAX_WIDTH, S32_VIDEO_HEIGHT) : NULL;
	if (!texture)
		return fail("The window could not be opened.", SDL_GetError());
	SDL_RenderSetIntegerScale(renderer, SDL_FALSE);
	SDL_SetTextureScaleMode(texture, settings.smooth ? SDL_ScaleModeLinear : SDL_ScaleModeNearest);
	SDL_SetRelativeMouseMode(SDL_TRUE);
	show_mouse_speed();

	SDL_zero(wanted);
	wanted.freq = OUTPUT_RATE;
	wanted.format = AUDIO_S16SYS;
	wanted.channels = 2;
	wanted.samples = 1024;
	audio = SDL_OpenAudioDevice(NULL, 0, &wanted, NULL, 0);
	if (audio)
		SDL_PauseAudioDevice(audio, 0);
	chips[0].step = chips[1].step = FM_RATE / OUTPUT_RATE;
	chips[2].step = PCM_RATE / OUTPUT_RATE;

	/* the board, the video, the sound board and the game, as the headless runs set them up */
	s32_board.irq_changed = on_irq_changed;
	s32_board.controls_update = on_controls_update;
	s32_board.log = on_log;
	s32_board.frame_timing = true;
	s32_board.sound_rom = game_sound;
	s32_video_init(game_tiles, GAME_TILES_SIZE, game_sprites, GAME_SPRITES_SIZE);
	s32_board.frame_shown = on_frame_shown;
	sound_chips_attach();
	sound_cpu_attach();
	s32_init(game_program);
	load_eeprom();
	program_reset();
	program_frame_done = on_frame_done;
	next_frame = SDL_GetPerformanceCounter();

	if (!setjmp(finished))
		program_run();

	save_eeprom();
	if (audio)
		SDL_CloseAudioDevice(audio);
	SDL_Quit();
	return 0;
}
