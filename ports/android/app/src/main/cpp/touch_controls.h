/*
 * SegaSonic The Hedgehog — Android Port
 * Multi-Touch Arcade Trackball / Stick / D-Pad Input, Quick Overlay & Live HUD Editor
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef SEGASONIC_ANDROID_TOUCH_CONTROLS_H
#define SEGASONIC_ANDROID_TOUCH_CONTROLS_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

#define TOUCH_MAX_FINGERS    10
#define TOUCH_CONFIG_VERSION 3

typedef enum touch_move_mode
{
	TOUCH_MOVE_TRACKBALL = 0, /* Authentic arcade trackball with momentum & spin brake */
	TOUCH_MOVE_STICK     = 1, /* Responsive virtual analog joystick (default) */
	TOUCH_MOVE_DPAD      = 2  /* Classic 8-way directional pad */
} touch_move_mode;

typedef enum touch_widget_id
{
	TOUCH_WIDGET_MOVE      = 0, /* Left Joystick / D-Pad / Trackball */
	TOUCH_WIDGET_JUMP      = 1, /* Primary Jump / Action / Confirm */
	TOUCH_WIDGET_TURBO     = 2, /* Optional Rapid-Jump / Trap-Escape button */
	TOUCH_WIDGET_START     = 3, /* Start (Arcade) / Pause (Console) */
	TOUCH_WIDGET_COIN_BACK = 4, /* Coin (Arcade) / Back (Console) */
	TOUCH_WIDGET_SERVICE   = 5, /* Arcade Test/Service switch */
	TOUCH_WIDGET_MENU      = 6, /* Top HUD Quick-Menu / Editor Gear button */
	TOUCH_WIDGET_COUNT     = 7
} touch_widget_id;

typedef enum touch_preset_id
{
	TOUCH_PRESET_ARCADE_DEFAULT = 0,
	TOUCH_PRESET_COMPACT_PHONE  = 1,
	TOUCH_PRESET_WIDE_TABLET    = 2,
	TOUCH_PRESET_LEFT_HANDED    = 3,
	TOUCH_PRESET_COUNT          = 4
} touch_preset_id;

/* Haptic impulse bitmask consumed each frame by android_main */
enum
{
	TOUCH_HAPTIC_NONE    = 0,
	TOUCH_HAPTIC_PRESS   = 1 << 0, /* Crisp button press (12 ms) */
	TOUCH_HAPTIC_RELEASE = 1 << 1, /* Subtle release tick (5 ms) */
	TOUCH_HAPTIC_TICK    = 1 << 2, /* Trackball rolling bearing tick (4 ms) */
	TOUCH_HAPTIC_UI      = 1 << 3  /* Editor / menu tap (8 ms) */
};

/* Commands emitted by the Quick Overlay Menu back to android_main / gles_renderer */
enum
{
	TOUCH_CMD_NONE               = 0,
	TOUCH_CMD_CYCLE_FILTER       = 1 << 0,
	TOUCH_CMD_CYCLE_ASPECT       = 1 << 1,
	TOUCH_CMD_TOGGLE_ETC2_STREAM = 1 << 2,
	TOUCH_CMD_TOGGLE_FPS         = 1 << 3,
	TOUCH_CMD_EXIT_TO_LAUNCHER   = 1 << 4,
	TOUCH_CMD_SAVE_CONFIG        = 1 << 5,
	TOUCH_CMD_EDITOR_CLOSED      = 1 << 6
};

/* Menu / console action flags driven by touch controls alongside hardware switches */
enum
{
	TOUCH_ACT_UP      = 1 << 0,
	TOUCH_ACT_DOWN    = 1 << 1,
	TOUCH_ACT_LEFT    = 1 << 2,
	TOUCH_ACT_RIGHT   = 1 << 3,
	TOUCH_ACT_CONFIRM = 1 << 4,
	TOUCH_ACT_BACK    = 1 << 5,
	TOUCH_ACT_PAUSE   = 1 << 6
};

typedef struct touch_widget_cfg
{
	float nx;      /* Normalized center X in [0.04 .. 0.96] */
	float ny;      /* Normalized center Y in [0.06 .. 0.94] */
	float scale;   /* Size multiplier in [0.50 .. 2.00] */
	float opacity; /* Alpha multiplier in [0.15 .. 1.00] */
	bool visible;
} touch_widget_cfg;

typedef struct touch_config
{
	touch_move_mode move_mode;
	bool modern_controls;          /* Console-style instant turn, crisp stop & true isometric angles */
	bool floating_origin;          /* Dynamic follow-thumb joystick / trackball origin */
	bool haptics_enabled;
	bool auto_hide_gamepad;        /* Smoothly fade HUD when physical controller active */
	bool grid_snap;                /* Snap to 2.5% grid in Layout Editor */
	float trackball_sensitivity;   /* [0.25 .. 3.00], default 1.00 */
	float trackball_friction;      /* [0.80 .. 0.97], default 0.91 */
	float stick_deadzone;          /* [0.02 .. 0.35], default 0.08 */
	float master_opacity;          /* [0.20 .. 1.00], default 0.88 */
	int player_color;              /* 0 = Sonic Blue, 1 = Mighty Red, 2 = Ray Gold */
	touch_widget_cfg widgets[TOUCH_WIDGET_COUNT];
} touch_config;

/* Per-frame output snapshot consumed by the game loop */
typedef struct touch_frame_output
{
	int16_t track_dx;        /* Clamped to [-120, +120] (safe against V60 int8 wrap!) */
	int16_t track_dy;        /* Clamped to [-120, +120] (+Y = screen down; negate if needed) */
	bool jump_held;          /* S32_P1_BUTTON */
	bool start_held;         /* S32_P1_START */
	bool coin_held;          /* S32_COIN1 (Arcade edition) */
	bool service_held;       /* S32_TEST / S32_SERVICE (Arcade edition) */
	uint32_t actions_held;   /* TOUCH_ACT_* bitmask */
	uint32_t actions_pressed;/* Rising-edge TOUCH_ACT_* bitmask for menu navigation */
	uint32_t haptic_events;  /* TOUCH_HAPTIC_* bitmask */
	uint32_t commands;       /* TOUCH_CMD_* bitmask */
	bool modal_paused;       /* True while Quick Menu or Layout Editor is active */
} touch_frame_output;

/* Interleaved 2D UI vertex (20 bytes) rendered from the 512x512 UI Atlas */
typedef struct ui_vertex
{
	float x, y;
	float u, v;
	uint8_t r, g, b, a;
} ui_vertex;

#define UI_MAX_QUADS    1536
#define UI_MAX_VERTICES (UI_MAX_QUADS * 4)

typedef struct ui_quad_batch
{
	ui_vertex verts[UI_MAX_VERTICES];
	int quad_count;
} ui_quad_batch;

/* Opaque state struct (stack/static allocatable) */
typedef struct touch_finger
{
	bool active;
	int64_t id;
	float x, y;           /* Current pixel coordinates */
	float start_x, start_y;
	float prev_x, prev_y;
	int owner_widget;     /* -1 = none, 0..TOUCH_WIDGET_COUNT-1, 100 = editor/modal */
	uint32_t down_frames;
} touch_finger;

typedef struct touch_controls
{
	touch_config cfg;
	bool console_mode;
	bool edit_mode;
	bool standalone_editor; /* Launched directly from Launcher solely to edit layout */
	bool quick_menu_open;
	int selected_widget;
	int active_preset;

	int screen_w;
	int screen_h;

	touch_finger fingers[TOUCH_MAX_FINGERS];

	/* Dynamic movement / trackball physics state */
	bool move_active;
	float move_origin_x;
	float move_origin_y;
	float nub_offset_x;
	float nub_offset_y;
	float vel_x;            /* Sub-pixel / inertia velocity reservoir (counts/frame) */
	float vel_y;
	float accum_x;          /* Fractional trackball pulse accumulator */
	float accum_y;
	float ball_roll_u;      /* Rolling dot-grid UV phase */
	float ball_roll_v;
	float tick_dist_accum;
	float last_angle_rad;   /* Angular hysteresis against 16-way octant border flicker */
	bool has_last_angle;
	uint32_t move_held_frames;
	uint32_t turbo_phase;

	/* Pinch-to-scale state in Layout Editor */
	bool pinch_active;
	float pinch_start_dist;
	float pinch_start_scale;
	float drag_offset_x;
	float drag_offset_y;

	/* Gamepad auto-hide visibility envelope [0..1] */
	float hud_visibility;
	uint32_t frames_since_touch;
	bool gamepad_recently_used;

	/* Rising-edge tracking & queued one-frame commands */
	uint32_t prev_actions;
	uint32_t pending_haptics;
	uint32_t pending_commands;
	uint32_t pulse_start_frames;
	uint32_t pulse_coin_frames;

	/* Live status strings displayed in Quick Menu / HUD */
	char status_filter[24];
	char status_aspect[24];
	char status_gpu[48];
	bool show_fps;
	float current_fps;
} touch_controls;

/* Initialize state and apply default configuration */
void touch_controls_init(touch_controls *tc, bool console_mode);

/* Populate `cfg` with clean defaults or a specific preset */
void touch_config_set_defaults(touch_config *cfg, bool console_mode);
void touch_config_apply_preset(touch_config *cfg, touch_preset_id preset, bool console_mode);

/* Load / save persistent INI file (`touch_controls.ini`) */
bool touch_controls_load_ini(touch_controls *tc, const char *path);
bool touch_controls_save_ini(const touch_controls *tc, const char *path);

/* Update viewport dimensions in pixels */
void touch_controls_set_viewport(touch_controls *tc, int width, int height);

/* Enter/exit live HUD Layout Editor or Quick Menu (e.g. on Android Back button) */
void touch_controls_set_edit_mode(touch_controls *tc, bool enabled, bool standalone);
bool touch_controls_on_back_pressed(touch_controls *tc);

/* Notify when a physical controller sends input (drives auto-hide fade) */
void touch_controls_notify_gamepad_input(touch_controls *tc);

/* Update status labels shown in the Quick Menu & FPS overlay */
void touch_controls_set_status_info(touch_controls *tc, const char *filter_name,
	const char *aspect_name, const char *gpu_desc, bool show_fps, float fps);

/* Feed normalized [0..1] SDL_FINGERDOWN / UP / MOTION events */
void touch_controls_finger_down(touch_controls *tc, int64_t finger_id, float norm_x, float norm_y);
void touch_controls_finger_motion(touch_controls *tc, int64_t finger_id, float norm_x, float norm_y);
void touch_controls_finger_up(touch_controls *tc, int64_t finger_id, float norm_x, float norm_y);
void touch_controls_reset_fingers(touch_controls *tc);

/* Advance 1 frame (60 Hz) of trackball inertia, buttons, and modal state */
void touch_controls_step_frame(touch_controls *tc, touch_frame_output *out);

/* Emit all HUD, Quick Menu, FPS overlay, and Editor quads into `batch` */
void touch_controls_build_vertices(const touch_controls *tc, ui_quad_batch *batch);

#ifdef __cplusplus
}
#endif

#endif /* SEGASONIC_ANDROID_TOUCH_CONTROLS_H */
