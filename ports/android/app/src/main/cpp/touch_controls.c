/*
 * SegaSonic The Hedgehog — Android Port
 * Multi-Touch Arcade Trackball / Stick / D-Pad Input, Quick Overlay & Live HUD Editor
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "touch_controls.h"
#include "ui_atlas.h"

#include <ctype.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define TRACKBALL_SAFE_MAX 120
#define DPAD_RAMP_RATE     12.0f
#define DPAD_MAX_SPEED     92.0f
#define TURBO_SPEED        108.0f

static const char *const k_widget_keys[TOUCH_WIDGET_COUNT] = {
	"move", "jump", "turbo", "start", "coin_back", "service", "menu"
};

static const char *const k_widget_titles[TOUCH_WIDGET_COUNT] = {
	"MOVEMENT", "JUMP / ACT", "TURBO ROLL", "START / PAUSE", "COIN / BACK", "SERVICE", "QUICK MENU"
};

/* ---- Math & Geometry Helpers ---- */

static inline float clampf(float v, float lo, float hi)
{
	if (v < lo) return lo;
	if (v > hi) return hi;
	return v;
}

static inline float base_unit_px(const touch_controls *tc)
{
	const float w = (float)(tc->screen_w > 0 ? tc->screen_w : 1280);
	const float h = (float)(tc->screen_h > 0 ? tc->screen_h : 720);
	return (w < h) ? w : h;
}

typedef struct rect_f
{
	float x0, y0, x1, y1;
} rect_f;

static inline bool rect_contains(rect_f r, float x, float y)
{
	return x >= r.x0 && x <= r.x1 && y >= r.y0 && y <= r.y1;
}

static bool widget_is_pill(touch_widget_id id)
{
	return id == TOUCH_WIDGET_START || id == TOUCH_WIDGET_COIN_BACK || id == TOUCH_WIDGET_SERVICE;
}

static void widget_half_extents(const touch_controls *tc, touch_widget_id id, float *out_hw, float *out_hh)
{
	const float u = base_unit_px(tc);
	const float sc = clampf(tc->cfg.widgets[id].scale, 0.50f, 2.00f);

	switch (id)
	{
		case TOUCH_WIDGET_MOVE:
			*out_hw = *out_hh = 0.195f * u * sc;
			break;
		case TOUCH_WIDGET_JUMP:
			*out_hw = *out_hh = 0.115f * u * sc;
			break;
		case TOUCH_WIDGET_TURBO:
			*out_hw = *out_hh = 0.082f * u * sc;
			break;
		case TOUCH_WIDGET_START:
		case TOUCH_WIDGET_COIN_BACK:
		case TOUCH_WIDGET_SERVICE:
			*out_hw = 0.084f * u * sc;
			*out_hh = 0.040f * u * sc;
			break;
		case TOUCH_WIDGET_MENU:
		default:
			*out_hw = *out_hh = 0.044f * u * sc;
			break;
	}
}

static void widget_center_px(const touch_controls *tc, touch_widget_id id, float *out_cx, float *out_cy)
{
	const float w = (float)(tc->screen_w > 0 ? tc->screen_w : 1280);
	const float h = (float)(tc->screen_h > 0 ? tc->screen_h : 720);
	*out_cx = tc->cfg.widgets[id].nx * w;
	*out_cy = tc->cfg.widgets[id].ny * h;
}

/* ---- Defaults & Layout Presets ---- */

void touch_config_apply_preset(touch_config *cfg, touch_preset_id preset, bool console_mode)
{
	if (!cfg)
		return;

	switch (preset)
	{
		case TOUCH_PRESET_COMPACT_PHONE:
			cfg->widgets[TOUCH_WIDGET_MOVE]      = (touch_widget_cfg){ 0.165f, 0.720f, 0.88f, 0.85f, true };
			cfg->widgets[TOUCH_WIDGET_JUMP]      = (touch_widget_cfg){ 0.855f, 0.740f, 0.92f, 0.90f, true };
			cfg->widgets[TOUCH_WIDGET_TURBO]     = (touch_widget_cfg){ 0.745f, 0.560f, 0.88f, 0.85f, true };
			cfg->widgets[TOUCH_WIDGET_START]     = (touch_widget_cfg){ 0.900f, 0.100f, 0.90f, 0.80f, true };
			cfg->widgets[TOUCH_WIDGET_COIN_BACK] = (touch_widget_cfg){ 0.760f, 0.100f, 0.90f, 0.80f, true };
			cfg->widgets[TOUCH_WIDGET_SERVICE]   = (touch_widget_cfg){ 0.620f, 0.100f, 0.85f, 0.75f, false };
			cfg->widgets[TOUCH_WIDGET_MENU]      = (touch_widget_cfg){ 0.500f, 0.085f, 0.90f, 0.65f, true };
			break;

		case TOUCH_PRESET_WIDE_TABLET:
			cfg->widgets[TOUCH_WIDGET_MOVE]      = (touch_widget_cfg){ 0.145f, 0.640f, 1.05f, 0.85f, true };
			cfg->widgets[TOUCH_WIDGET_JUMP]      = (touch_widget_cfg){ 0.875f, 0.660f, 1.08f, 0.90f, true };
			cfg->widgets[TOUCH_WIDGET_TURBO]     = (touch_widget_cfg){ 0.775f, 0.490f, 1.00f, 0.85f, true };
			cfg->widgets[TOUCH_WIDGET_START]     = (touch_widget_cfg){ 0.910f, 0.085f, 1.00f, 0.82f, true };
			cfg->widgets[TOUCH_WIDGET_COIN_BACK] = (touch_widget_cfg){ 0.790f, 0.085f, 1.00f, 0.82f, true };
			cfg->widgets[TOUCH_WIDGET_SERVICE]   = (touch_widget_cfg){ 0.670f, 0.085f, 0.95f, 0.75f, !console_mode };
			cfg->widgets[TOUCH_WIDGET_MENU]      = (touch_widget_cfg){ 0.500f, 0.075f, 1.00f, 0.65f, true };
			break;

		case TOUCH_PRESET_ARCADE_DEFAULT:
		default:
			cfg->widgets[TOUCH_WIDGET_MOVE]      = (touch_widget_cfg){ 0.185f, 0.695f, 1.00f, 0.88f, true };
			cfg->widgets[TOUCH_WIDGET_JUMP]      = (touch_widget_cfg){ 0.845f, 0.720f, 1.05f, 0.92f, true };
			cfg->widgets[TOUCH_WIDGET_TURBO]     = (touch_widget_cfg){ 0.725f, 0.545f, 0.95f, 0.86f, true };
			cfg->widgets[TOUCH_WIDGET_START]     = (touch_widget_cfg){ 0.895f, 0.095f, 0.96f, 0.82f, true };
			cfg->widgets[TOUCH_WIDGET_COIN_BACK] = (touch_widget_cfg){ 0.755f, 0.095f, 0.96f, 0.82f, true };
			cfg->widgets[TOUCH_WIDGET_SERVICE]   = (touch_widget_cfg){ 0.105f, 0.095f, 0.90f, 0.75f, false };
			cfg->widgets[TOUCH_WIDGET_MENU]      = (touch_widget_cfg){ 0.500f, 0.085f, 0.95f, 0.68f, true };
			break;
	}
}

void touch_config_set_defaults(touch_config *cfg, bool console_mode)
{
	if (!cfg)
		return;

	memset(cfg, 0, sizeof(*cfg));
	cfg->move_mode = TOUCH_MOVE_TRACKBALL;
	cfg->floating_origin = false;
	cfg->haptics_enabled = true;
	cfg->auto_hide_gamepad = true;
	cfg->grid_snap = false;
	cfg->trackball_sensitivity = 1.15f;
	cfg->trackball_friction = 0.91f;
	cfg->stick_deadzone = 0.10f;
	cfg->master_opacity = 0.88f;
	cfg->player_color = 0;

	touch_config_apply_preset(cfg, TOUCH_PRESET_ARCADE_DEFAULT, console_mode);
}

void touch_controls_init(touch_controls *tc, bool console_mode)
{
	if (!tc)
		return;

	memset(tc, 0, sizeof(*tc));
	tc->console_mode = console_mode;
	tc->screen_w = 1280;
	tc->screen_h = 720;
	tc->hud_visibility = 1.0f;
	tc->selected_widget = TOUCH_WIDGET_MOVE;
	tc->active_preset = TOUCH_PRESET_ARCADE_DEFAULT;
	snprintf(tc->status_filter, sizeof(tc->status_filter), "SHARP");
	snprintf(tc->status_aspect, sizeof(tc->status_aspect), console_mode ? "16:9 WIDE" : "4:3 ARCADE");
	snprintf(tc->status_gpu, sizeof(tc->status_gpu), "GLES3 ETC2");

	touch_config_set_defaults(&tc->cfg, console_mode);
	touch_controls_reset_fingers(tc);
}

void touch_controls_set_viewport(touch_controls *tc, int width, int height)
{
	if (!tc || width <= 0 || height <= 0)
		return;
	tc->screen_w = width;
	tc->screen_h = height;
}

void touch_controls_reset_fingers(touch_controls *tc)
{
	if (!tc)
		return;
	for (int i = 0; i < TOUCH_MAX_FINGERS; i++)
	{
		tc->fingers[i].active = false;
		tc->fingers[i].owner_widget = -1;
	}
	tc->move_active = false;
	tc->nub_offset_x = 0.0f;
	tc->nub_offset_y = 0.0f;
	tc->pinch_active = false;
}

void touch_controls_set_edit_mode(touch_controls *tc, bool enabled, bool standalone)
{
	if (!tc)
		return;
	touch_controls_reset_fingers(tc);
	tc->edit_mode = enabled;
	tc->standalone_editor = enabled && standalone;
	if (enabled)
	{
		tc->quick_menu_open = false;
		tc->hud_visibility = 1.0f;
		tc->gamepad_recently_used = false;
	}
}

bool touch_controls_on_back_pressed(touch_controls *tc)
{
	if (!tc)
		return false;

	if (tc->edit_mode)
	{
		tc->edit_mode = false;
		tc->pending_commands |= TOUCH_CMD_SAVE_CONFIG | TOUCH_CMD_EDITOR_CLOSED;
		if (tc->standalone_editor)
			tc->pending_commands |= TOUCH_CMD_EXIT_TO_LAUNCHER;
		tc->standalone_editor = false;
		touch_controls_reset_fingers(tc);
		tc->pending_haptics |= TOUCH_HAPTIC_UI;
		return true;
	}

	tc->quick_menu_open = !tc->quick_menu_open;
	touch_controls_reset_fingers(tc);
	tc->pending_haptics |= TOUCH_HAPTIC_UI;
	return true;
}

void touch_controls_notify_gamepad_input(touch_controls *tc)
{
	if (!tc)
		return;
	tc->gamepad_recently_used = true;
}

void touch_controls_set_status_info(touch_controls *tc, const char *filter_name,
	const char *aspect_name, const char *gpu_desc, bool show_fps, float fps)
{
	if (!tc)
		return;
	if (filter_name)
		snprintf(tc->status_filter, sizeof(tc->status_filter), "%s", filter_name);
	if (aspect_name)
		snprintf(tc->status_aspect, sizeof(tc->status_aspect), "%s", aspect_name);
	if (gpu_desc)
		snprintf(tc->status_gpu, sizeof(tc->status_gpu), "%s", gpu_desc);
	tc->show_fps = show_fps;
	tc->current_fps = fps;
}

/* ---- INI Persistence (`touch_controls.ini`) ---- */

static char *trim_ws(char *s)
{
	char *end;
	while (*s && isspace((unsigned char)*s))
		s++;
	end = s + strlen(s);
	while (end > s && isspace((unsigned char)end[-1]))
		*--end = '\0';
	return s;
}

bool touch_controls_load_ini(touch_controls *tc, const char *path)
{
	FILE *fp;
	char line[256];

	if (!tc || !path)
		return false;
	fp = fopen(path, "r");
	if (!fp)
		return false;

	while (fgets(line, sizeof(line), fp))
	{
		char *eq;
		char *key;
		char *val;

		char *hash = strchr(line, '#');
		if (hash)
			*hash = '\0';
		eq = strchr(line, '=');
		if (!eq)
			continue;
		*eq = '\0';
		key = trim_ws(line);
		val = trim_ws(eq + 1);
		if (!*key || !*val)
			continue;

		if (strcmp(key, "move_mode") == 0)
			tc->cfg.move_mode = (touch_move_mode)clampf((float)atoi(val), 0.0f, 2.0f);
		else if (strcmp(key, "floating_origin") == 0)
			tc->cfg.floating_origin = atoi(val) != 0;
		else if (strcmp(key, "haptics_enabled") == 0)
			tc->cfg.haptics_enabled = atoi(val) != 0;
		else if (strcmp(key, "auto_hide_gamepad") == 0)
			tc->cfg.auto_hide_gamepad = atoi(val) != 0;
		else if (strcmp(key, "grid_snap") == 0)
			tc->cfg.grid_snap = atoi(val) != 0;
		else if (strcmp(key, "trackball_sensitivity") == 0)
			tc->cfg.trackball_sensitivity = clampf((float)atof(val), 0.25f, 3.00f);
		else if (strcmp(key, "trackball_friction") == 0)
			tc->cfg.trackball_friction = clampf((float)atof(val), 0.80f, 0.97f);
		else if (strcmp(key, "stick_deadzone") == 0)
			tc->cfg.stick_deadzone = clampf((float)atof(val), 0.02f, 0.35f);
		else if (strcmp(key, "master_opacity") == 0)
			tc->cfg.master_opacity = clampf((float)atof(val), 0.20f, 1.00f);
		else if (strcmp(key, "player_color") == 0)
			tc->cfg.player_color = (int)clampf((float)atoi(val), 0.0f, 2.0f);
		else
		{
			for (int w = 0; w < TOUCH_WIDGET_COUNT; w++)
			{
				size_t klen = strlen(k_widget_keys[w]);
				if (strncmp(key, k_widget_keys[w], klen) == 0 && key[klen] == '.')
				{
					const char *sub = key + klen + 1;
					if (strcmp(sub, "nx") == 0)
						tc->cfg.widgets[w].nx = clampf((float)atof(val), 0.04f, 0.96f);
					else if (strcmp(sub, "ny") == 0)
						tc->cfg.widgets[w].ny = clampf((float)atof(val), 0.05f, 0.95f);
					else if (strcmp(sub, "scale") == 0)
						tc->cfg.widgets[w].scale = clampf((float)atof(val), 0.50f, 2.00f);
					else if (strcmp(sub, "opacity") == 0)
						tc->cfg.widgets[w].opacity = clampf((float)atof(val), 0.15f, 1.00f);
					else if (strcmp(sub, "visible") == 0)
						tc->cfg.widgets[w].visible = atoi(val) != 0;
					break;
				}
			}
		}
	}

	/* Menu button must always remain accessible */
	tc->cfg.widgets[TOUCH_WIDGET_MENU].visible = true;

	fclose(fp);
	return true;
}

bool touch_controls_save_ini(const touch_controls *tc, const char *path)
{
	FILE *fp;

	if (!tc || !path)
		return false;
	fp = fopen(path, "w");
	if (!fp)
		return false;

	fprintf(fp, "# SegaSonic The Hedgehog Android Touch Controls\n");
	fprintf(fp, "move_mode = %d\n", (int)tc->cfg.move_mode);
	fprintf(fp, "floating_origin = %d\n", tc->cfg.floating_origin ? 1 : 0);
	fprintf(fp, "haptics_enabled = %d\n", tc->cfg.haptics_enabled ? 1 : 0);
	fprintf(fp, "auto_hide_gamepad = %d\n", tc->cfg.auto_hide_gamepad ? 1 : 0);
	fprintf(fp, "grid_snap = %d\n", tc->cfg.grid_snap ? 1 : 0);
	fprintf(fp, "trackball_sensitivity = %.3f\n", tc->cfg.trackball_sensitivity);
	fprintf(fp, "trackball_friction = %.3f\n", tc->cfg.trackball_friction);
	fprintf(fp, "stick_deadzone = %.3f\n", tc->cfg.stick_deadzone);
	fprintf(fp, "master_opacity = %.3f\n", tc->cfg.master_opacity);
	fprintf(fp, "player_color = %d\n\n", tc->cfg.player_color);

	for (int w = 0; w < TOUCH_WIDGET_COUNT; w++)
	{
		const touch_widget_cfg *wc = &tc->cfg.widgets[w];
		fprintf(fp, "%s.nx = %.4f\n", k_widget_keys[w], wc->nx);
		fprintf(fp, "%s.ny = %.4f\n", k_widget_keys[w], wc->ny);
		fprintf(fp, "%s.scale = %.3f\n", k_widget_keys[w], wc->scale);
		fprintf(fp, "%s.opacity = %.3f\n", k_widget_keys[w], wc->opacity);
		fprintf(fp, "%s.visible = %d\n\n", k_widget_keys[w], wc->visible ? 1 : 0);
	}

	return fclose(fp) == 0;
}

/* ---- Deterministic Modal Geometries (Quick Menu & Editor Dock) ---- */

#define EDITOR_BTN_COUNT 11
enum
{
	ED_BTN_SIZE_DEC = 0,
	ED_BTN_SIZE_INC,
	ED_BTN_ALPHA_DEC,
	ED_BTN_ALPHA_INC,
	ED_BTN_VISIBILITY,
	ED_BTN_MODE,
	ED_BTN_FLOAT,
	ED_BTN_GRID,
	ED_BTN_PRESET,
	ED_BTN_RESET,
	ED_BTN_DONE
};

typedef struct editor_layout
{
	rect_f panel;
	rect_f btns[EDITOR_BTN_COUNT];
} editor_layout;

static void compute_editor_layout(const touch_controls *tc, editor_layout *out)
{
	const float sw = (float)(tc->screen_w > 0 ? tc->screen_w : 1280);
	const float sh = (float)(tc->screen_h > 0 ? tc->screen_h : 720);
	const float pw = fminf(sw * 0.90f, sh * 1.48f);
	const float ph = sh * 0.235f;
	/* Auto-flip dock to bottom if selected widget is in the top 36% of screen */
	const bool dock_bottom = (tc->cfg.widgets[tc->selected_widget].ny < 0.36f);
	const float px0 = (sw - pw) * 0.5f;
	const float py0 = dock_bottom ? (sh - ph - sh * 0.03f) : (sh * 0.03f);

	out->panel = (rect_f){ px0, py0, px0 + pw, py0 + ph };

	/* Row 1: 6 buttons; Row 2: 5 buttons */
	{
		const float pad_x = pw * 0.02f;
		const float gap_x = pw * 0.012f;
		const float row1_y0 = py0 + ph * 0.34f;
		const float row1_y1 = py0 + ph * 0.64f;
		const float row2_y0 = py0 + ph * 0.69f;
		const float row2_y1 = py0 + ph * 0.95f;
		const float w1 = (pw - 2.0f * pad_x - 5.0f * gap_x) / 6.0f;
		const float w2 = (pw - 2.0f * pad_x - 4.0f * gap_x) / 5.0f;

		for (int i = 0; i < 6; i++)
		{
			const float bx = px0 + pad_x + i * (w1 + gap_x);
			out->btns[i] = (rect_f){ bx, row1_y0, bx + w1, row1_y1 };
		}
		for (int i = 0; i < 5; i++)
		{
			const float bx = px0 + pad_x + i * (w2 + gap_x);
			out->btns[6 + i] = (rect_f){ bx, row2_y0, bx + w2, row2_y1 };
		}
	}
}

#define QMENU_BTN_COUNT 8
enum
{
	QM_BTN_RESUME = 0,
	QM_BTN_EDIT_HUD,
	QM_BTN_MOVE_MODE,
	QM_BTN_FILTER,
	QM_BTN_ASPECT,
	QM_BTN_ETC2_STREAM,
	QM_BTN_FPS,
	QM_BTN_EXIT
};

typedef struct qmenu_layout
{
	rect_f panel;
	rect_f btns[QMENU_BTN_COUNT];
} qmenu_layout;

static void compute_qmenu_layout(const touch_controls *tc, qmenu_layout *out)
{
	const float sw = (float)(tc->screen_w > 0 ? tc->screen_w : 1280);
	const float sh = (float)(tc->screen_h > 0 ? tc->screen_h : 720);
	const float pw = fminf(sw * 0.78f, sh * 1.24f);
	const float ph = sh * 0.76f;
	const float px0 = (sw - pw) * 0.5f;
	const float py0 = (sh - ph) * 0.5f;

	out->panel = (rect_f){ px0, py0, px0 + pw, py0 + ph };

	/* 2 columns x 4 rows of crisp action cards */
	{
		const float pad_x = pw * 0.04f;
		const float gap_x = pw * 0.03f;
		const float top_y = py0 + ph * 0.20f;
		const float bot_y = py0 + ph * 0.94f;
		const float gap_y = ph * 0.032f;
		const float bw = (pw - 2.0f * pad_x - gap_x) * 0.5f;
		const float bh = (bot_y - top_y - 3.0f * gap_y) * 0.25f;

		for (int i = 0; i < QMENU_BTN_COUNT; i++)
		{
			const int col = i & 1;
			const int row = i >> 1;
			const float bx = px0 + pad_x + col * (bw + gap_x);
			const float by = top_y + row * (bh + gap_y);
			out->btns[i] = (rect_f){ bx, by, bx + bw, by + bh };
		}
	}
}

/* ---- Hit-Testing & Multi-Touch Event Handling ---- */

static int hit_test_widget(const touch_controls *tc, float px, float py, bool include_hidden)
{
	int best_w = -1;
	float best_score = 1e9f;

	/* Test smaller buttons before large movement zone so overlapping edges prefer buttons */
	static const touch_widget_id order[TOUCH_WIDGET_COUNT] = {
		TOUCH_WIDGET_MENU,
		TOUCH_WIDGET_START,
		TOUCH_WIDGET_COIN_BACK,
		TOUCH_WIDGET_SERVICE,
		TOUCH_WIDGET_TURBO,
		TOUCH_WIDGET_JUMP,
		TOUCH_WIDGET_MOVE
	};

	for (int i = 0; i < TOUCH_WIDGET_COUNT; i++)
	{
		const touch_widget_id id = order[i];
		float cx, cy, hw, hh;

		if (!include_hidden && !tc->cfg.widgets[id].visible)
			continue;

		widget_center_px(tc, id, &cx, &cy);
		widget_half_extents(tc, id, &hw, &hh);

		/* Slightly generous 18% touch halo for reliable thumb hits */
		const float hit_hw = hw * 1.18f;
		const float hit_hh = hh * 1.18f;
		const float dx = (px - cx) / hit_hw;
		const float dy = (py - cy) / hit_hh;

		if (widget_is_pill(id))
		{
			if (fabsf(dx) <= 1.0f && fabsf(dy) <= 1.0f)
			{
				const float score = dx * dx + dy * dy;
				if (score < best_score)
				{
					best_score = score;
					best_w = (int)id;
				}
			}
		}
		else
		{
			const float d2 = dx * dx + dy * dy;
			if (d2 <= 1.0f && d2 < best_score)
			{
				best_score = d2;
				best_w = (int)id;
			}
		}
	}

	/* Floating origin: unclaimed touch in left 42% below top bar activates movement widget */
	if (best_w < 0 && !include_hidden && tc->cfg.floating_origin && tc->cfg.widgets[TOUCH_WIDGET_MOVE].visible)
	{
		if (px < (float)tc->screen_w * 0.42f && py > (float)tc->screen_h * 0.18f)
			best_w = TOUCH_WIDGET_MOVE;
	}

	return best_w;
}

static touch_finger *alloc_finger_slot(touch_controls *tc, int64_t id)
{
	touch_finger *free_slot = NULL;
	for (int i = 0; i < TOUCH_MAX_FINGERS; i++)
	{
		if (tc->fingers[i].active && tc->fingers[i].id == id)
			return &tc->fingers[i];
		if (!tc->fingers[i].active && !free_slot)
			free_slot = &tc->fingers[i];
	}
	return free_slot;
}

static touch_finger *find_finger_slot(touch_controls *tc, int64_t id)
{
	for (int i = 0; i < TOUCH_MAX_FINGERS; i++)
	{
		if (tc->fingers[i].active && tc->fingers[i].id == id)
			return &tc->fingers[i];
	}
	return NULL;
}

static void handle_editor_button(touch_controls *tc, int btn)
{
	touch_widget_cfg *wc = &tc->cfg.widgets[tc->selected_widget];
	tc->pending_haptics |= TOUCH_HAPTIC_UI;

	switch (btn)
	{
		case ED_BTN_SIZE_DEC:
			wc->scale = clampf(wc->scale - 0.08f, 0.50f, 2.00f);
			break;
		case ED_BTN_SIZE_INC:
			wc->scale = clampf(wc->scale + 0.08f, 0.50f, 2.00f);
			break;
		case ED_BTN_ALPHA_DEC:
			wc->opacity = clampf(wc->opacity - 0.08f, 0.15f, 1.00f);
			break;
		case ED_BTN_ALPHA_INC:
			wc->opacity = clampf(wc->opacity + 0.08f, 0.15f, 1.00f);
			break;
		case ED_BTN_VISIBILITY:
			if (tc->selected_widget != TOUCH_WIDGET_MENU)
				wc->visible = !wc->visible;
			break;
		case ED_BTN_MODE:
			tc->cfg.move_mode = (touch_move_mode)(((int)tc->cfg.move_mode + 1) % 3);
			break;
		case ED_BTN_FLOAT:
			tc->cfg.floating_origin = !tc->cfg.floating_origin;
			break;
		case ED_BTN_GRID:
			tc->cfg.grid_snap = !tc->cfg.grid_snap;
			break;
		case ED_BTN_PRESET:
			tc->active_preset = (tc->active_preset + 1) % TOUCH_PRESET_COUNT;
			touch_config_apply_preset(&tc->cfg, (touch_preset_id)tc->active_preset, tc->console_mode);
			break;
		case ED_BTN_RESET:
			touch_config_set_defaults(&tc->cfg, tc->console_mode);
			tc->active_preset = TOUCH_PRESET_ARCADE_DEFAULT;
			break;
		case ED_BTN_DONE:
			tc->edit_mode = false;
			tc->pending_commands |= TOUCH_CMD_SAVE_CONFIG | TOUCH_CMD_EDITOR_CLOSED;
			if (tc->standalone_editor)
				tc->pending_commands |= TOUCH_CMD_EXIT_TO_LAUNCHER;
			tc->standalone_editor = false;
			touch_controls_reset_fingers(tc);
			break;
	}
}

static void handle_qmenu_button(touch_controls *tc, int btn)
{
	tc->pending_haptics |= TOUCH_HAPTIC_UI;

	switch (btn)
	{
		case QM_BTN_RESUME:
			tc->quick_menu_open = false;
			break;
		case QM_BTN_EDIT_HUD:
			tc->quick_menu_open = false;
			tc->edit_mode = true;
			tc->standalone_editor = false;
			break;
		case QM_BTN_MOVE_MODE:
			tc->cfg.move_mode = (touch_move_mode)(((int)tc->cfg.move_mode + 1) % 3);
			tc->pending_commands |= TOUCH_CMD_SAVE_CONFIG;
			break;
		case QM_BTN_FILTER:
			tc->pending_commands |= TOUCH_CMD_CYCLE_FILTER;
			break;
		case QM_BTN_ASPECT:
			tc->pending_commands |= TOUCH_CMD_CYCLE_ASPECT;
			break;
		case QM_BTN_ETC2_STREAM:
			tc->pending_commands |= TOUCH_CMD_TOGGLE_ETC2_STREAM;
			break;
		case QM_BTN_FPS:
			tc->pending_commands |= TOUCH_CMD_TOGGLE_FPS;
			break;
		case QM_BTN_EXIT:
			tc->quick_menu_open = false;
			tc->pending_commands |= TOUCH_CMD_SAVE_CONFIG | TOUCH_CMD_EXIT_TO_LAUNCHER;
			break;
	}
}

static void update_move_deflection(touch_controls *tc, float px, float py)
{
	float hw, hh;
	widget_half_extents(tc, TOUCH_WIDGET_MOVE, &hw, &hh);

	{
		const float max_r = hw * 0.62f;
		float dx = px - tc->move_origin_x;
		float dy = py - tc->move_origin_y;
		const float dist = sqrtf(dx * dx + dy * dy);

		if (dist > max_r && dist > 0.0001f)
		{
			dx = dx * (max_r / dist);
			dy = dy * (max_r / dist);
		}
		tc->nub_offset_x = dx;
		tc->nub_offset_y = dy;
	}
}

void touch_controls_finger_down(touch_controls *tc, int64_t finger_id, float norm_x, float norm_y)
{
	if (!tc)
		return;

	const float sw = (float)(tc->screen_w > 0 ? tc->screen_w : 1280);
	const float sh = (float)(tc->screen_h > 0 ? tc->screen_h : 720);
	const float px = clampf(norm_x, 0.0f, 1.0f) * sw;
	const float py = clampf(norm_y, 0.0f, 1.0f) * sh;

	tc->frames_since_touch = 0;
	tc->gamepad_recently_used = false;
	tc->hud_visibility = 1.0f;

	/* 1. Quick Menu modal intercepts all taps */
	if (tc->quick_menu_open)
	{
		qmenu_layout qm;
		compute_qmenu_layout(tc, &qm);

		for (int b = 0; b < QMENU_BTN_COUNT; b++)
		{
			if (rect_contains(qm.btns[b], px, py))
			{
				handle_qmenu_button(tc, b);
				return;
			}
		}
		if (!rect_contains(qm.panel, px, py))
		{
			tc->quick_menu_open = false;
			tc->pending_haptics |= TOUCH_HAPTIC_UI;
		}
		return;
	}

	/* 2. Interactive HUD Layout Editor mode */
	if (tc->edit_mode)
	{
		editor_layout ed;
		touch_finger *slot;

		compute_editor_layout(tc, &ed);
		for (int b = 0; b < EDITOR_BTN_COUNT; b++)
		{
			if (rect_contains(ed.btns[b], px, py))
			{
				handle_editor_button(tc, b);
				return;
			}
		}
		if (rect_contains(ed.panel, px, py))
			return;

		slot = alloc_finger_slot(tc, finger_id);
		if (!slot)
			return;

		slot->active = true;
		slot->id = finger_id;
		slot->x = slot->start_x = slot->prev_x = px;
		slot->y = slot->start_y = slot->prev_y = py;
		slot->down_frames = 0;

		/* Check if another finger is already dragging a widget -> start 2-finger pinch scale! */
		{
			touch_finger *other = NULL;
			for (int i = 0; i < TOUCH_MAX_FINGERS; i++)
			{
				if (&tc->fingers[i] != slot && tc->fingers[i].active)
				{
					other = &tc->fingers[i];
					break;
				}
			}

			if (other)
			{
				const float d = hypotf(px - other->x, py - other->y);
				if (d > 16.0f)
				{
					tc->pinch_active = true;
					tc->pinch_start_dist = d;
					tc->pinch_start_scale = tc->cfg.widgets[tc->selected_widget].scale;
					slot->owner_widget = 100;
					return;
				}
			}
		}

		{
			const int hit = hit_test_widget(tc, px, py, true);
			if (hit >= 0)
			{
				float cx, cy;
				tc->selected_widget = hit;
				widget_center_px(tc, (touch_widget_id)hit, &cx, &cy);
				tc->drag_offset_x = cx - px;
				tc->drag_offset_y = cy - py;
				slot->owner_widget = hit;
				tc->pending_haptics |= TOUCH_HAPTIC_UI;
			}
			else
			{
				slot->owner_widget = -1;
			}
		}
		return;
	}

	/* 3. Standard Gameplay Multi-Touch */
	{
		touch_finger *slot = alloc_finger_slot(tc, finger_id);
		if (!slot)
			return;

		const int hit = hit_test_widget(tc, px, py, false);
		slot->active = true;
		slot->id = finger_id;
		slot->x = slot->start_x = slot->prev_x = px;
		slot->y = slot->start_y = slot->prev_y = py;
		slot->owner_widget = hit;
		slot->down_frames = 0;

		if (hit == TOUCH_WIDGET_MENU)
		{
			slot->active = false;
			tc->quick_menu_open = true;
			tc->pending_haptics |= TOUCH_HAPTIC_UI;
			return;
		}

		if (hit == TOUCH_WIDGET_MOVE)
		{
			float cx, cy;
			widget_center_px(tc, TOUCH_WIDGET_MOVE, &cx, &cy);
			tc->move_active = true;
			if (tc->cfg.floating_origin)
			{
				tc->move_origin_x = px;
				tc->move_origin_y = py;
			}
			else
			{
				tc->move_origin_x = cx;
				tc->move_origin_y = cy;
			}

			if (tc->cfg.move_mode == TOUCH_MOVE_TRACKBALL)
			{
				/* Palm-brake an existing spin when thumb catches the ball */
				tc->vel_x *= 0.25f;
				tc->vel_y *= 0.25f;
				tc->nub_offset_x = 0.0f;
				tc->nub_offset_y = 0.0f;
			}
			else
			{
				update_move_deflection(tc, px, py);
			}
			tc->pending_haptics |= TOUCH_HAPTIC_PRESS;
		}
		else if (hit >= 0)
		{
			tc->pending_haptics |= TOUCH_HAPTIC_PRESS;
		}
	}
}

void touch_controls_finger_motion(touch_controls *tc, int64_t finger_id, float norm_x, float norm_y)
{
	touch_finger *slot;

	if (!tc)
		return;
	slot = find_finger_slot(tc, finger_id);
	if (!slot)
		return;

	const float sw = (float)(tc->screen_w > 0 ? tc->screen_w : 1280);
	const float sh = (float)(tc->screen_h > 0 ? tc->screen_h : 720);
	const float px = clampf(norm_x, 0.0f, 1.0f) * sw;
	const float py = clampf(norm_y, 0.0f, 1.0f) * sh;
	const float dx = px - slot->x;
	const float dy = py - slot->y;

	slot->prev_x = slot->x;
	slot->prev_y = slot->y;
	slot->x = px;
	slot->y = py;

	/* Editor drag & 2-finger pinch scale */
	if (tc->edit_mode)
	{
		if (tc->pinch_active)
		{
			touch_finger *f0 = NULL, *f1 = NULL;
			for (int i = 0; i < TOUCH_MAX_FINGERS; i++)
			{
				if (tc->fingers[i].active)
				{
					if (!f0) f0 = &tc->fingers[i];
					else { f1 = &tc->fingers[i]; break; }
				}
			}
			if (f0 && f1 && tc->pinch_start_dist > 16.0f)
			{
				const float d = hypotf(f1->x - f0->x, f1->y - f0->y);
				const float ratio = d / tc->pinch_start_dist;
				tc->cfg.widgets[tc->selected_widget].scale =
					clampf(tc->pinch_start_scale * ratio, 0.50f, 2.00f);
			}
			return;
		}

		if (slot->owner_widget >= 0 && slot->owner_widget < TOUCH_WIDGET_COUNT)
		{
			float nx = (px + tc->drag_offset_x) / sw;
			float ny = (py + tc->drag_offset_y) / sh;

			if (tc->cfg.grid_snap)
			{
				nx = roundf(nx * 40.0f) / 40.0f;
				ny = roundf(ny * 40.0f) / 40.0f;
			}
			tc->cfg.widgets[slot->owner_widget].nx = clampf(nx, 0.05f, 0.95f);
			tc->cfg.widgets[slot->owner_widget].ny = clampf(ny, 0.06f, 0.94f);
		}
		return;
	}

	/* Gameplay movement update */
	if (slot->owner_widget == TOUCH_WIDGET_MOVE)
	{
		if (tc->cfg.move_mode == TOUCH_MOVE_TRACKBALL)
		{
			const float scale_720 = 720.0f / base_unit_px(tc);
			const float sens = 1.45f * tc->cfg.trackball_sensitivity * scale_720;
			const float vx = dx * sens;
			const float vy = dy * sens;

			tc->accum_x += vx;
			tc->accum_y += vy;
			tc->vel_x = 0.35f * tc->vel_x + 0.65f * vx;
			tc->vel_y = 0.35f * tc->vel_y + 0.65f * vy;

			/* Subtle visual ball shift inside recessed well */
			tc->nub_offset_x = clampf(tc->nub_offset_x * 0.6f + dx * 0.8f, -14.0f, 14.0f);
			tc->nub_offset_y = clampf(tc->nub_offset_y * 0.6f + dy * 0.8f, -14.0f, 14.0f);
		}
		else
		{
			update_move_deflection(tc, px, py);
		}
	}
	else if (slot->owner_widget == TOUCH_WIDGET_JUMP || slot->owner_widget == TOUCH_WIDGET_TURBO)
	{
		/* Allow smooth thumb slide-over between JUMP and TURBO! */
		const int over = hit_test_widget(tc, px, py, false);
		if ((over == TOUCH_WIDGET_JUMP || over == TOUCH_WIDGET_TURBO) && over != slot->owner_widget)
		{
			slot->owner_widget = over;
			tc->pending_haptics |= TOUCH_HAPTIC_PRESS;
		}
	}
}

void touch_controls_finger_up(touch_controls *tc, int64_t finger_id, float norm_x, float norm_y)
{
	touch_finger *slot;
	(void)norm_x;
	(void)norm_y;

	if (!tc)
		return;
	slot = find_finger_slot(tc, finger_id);
	if (!slot)
		return;

	const int owner = slot->owner_widget;
	slot->active = false;
	slot->owner_widget = -1;

	if (tc->edit_mode)
	{
		int active_count = 0;
		for (int i = 0; i < TOUCH_MAX_FINGERS; i++)
			active_count += tc->fingers[i].active ? 1 : 0;
		if (active_count < 2)
			tc->pinch_active = false;
		return;
	}

	if (owner == TOUCH_WIDGET_MOVE)
	{
		/* Check if any other finger still owns TOUCH_WIDGET_MOVE */
		bool still_held = false;
		for (int i = 0; i < TOUCH_MAX_FINGERS; i++)
		{
			if (tc->fingers[i].active && tc->fingers[i].owner_widget == TOUCH_WIDGET_MOVE)
			{
				still_held = true;
				break;
			}
		}
		if (!still_held)
		{
			tc->move_active = false;
			tc->nub_offset_x = 0.0f;
			tc->nub_offset_y = 0.0f;
		}
	}
	else if (owner >= 0)
	{
		tc->pending_haptics |= TOUCH_HAPTIC_RELEASE;
	}
}

/* ---- 60 Hz Physics, Action Generation & Safe Delta Clamping ---- */

void touch_controls_step_frame(touch_controls *tc, touch_frame_output *out)
{
	if (!tc || !out)
		return;

	memset(out, 0, sizeof(*out));
	out->modal_paused = tc->quick_menu_open || tc->edit_mode;

	/* Consume queued commands & haptics */
	out->commands = tc->pending_commands;
	tc->pending_commands = 0;

	/* Update auto-hide fade envelope */
	{
		bool any_finger = false;
		for (int i = 0; i < TOUCH_MAX_FINGERS; i++)
		{
			if (tc->fingers[i].active)
			{
				any_finger = true;
				tc->fingers[i].down_frames++;
			}
		}

		if (any_finger || out->modal_paused)
		{
			tc->frames_since_touch = 0;
			tc->hud_visibility = 1.0f;
		}
		else
		{
			tc->frames_since_touch++;
			if (tc->cfg.auto_hide_gamepad && tc->gamepad_recently_used)
				tc->hud_visibility = clampf(tc->hud_visibility - 0.05f, 0.0f, 1.0f);
			else
				tc->hud_visibility = clampf(tc->hud_visibility + 0.08f, 0.0f, 1.0f);
		}
	}

	if (out->modal_paused)
	{
		if (tc->cfg.haptics_enabled)
			out->haptic_events = tc->pending_haptics;
		tc->pending_haptics = 0;
		tc->prev_actions = 0;
		return;
	}

	/* Scan held touch buttons */
	bool move_held = false;
	bool turbo_held = false;

	for (int i = 0; i < TOUCH_MAX_FINGERS; i++)
	{
		if (!tc->fingers[i].active)
			continue;
		switch (tc->fingers[i].owner_widget)
		{
			case TOUCH_WIDGET_MOVE:      move_held = true; break;
			case TOUCH_WIDGET_JUMP:      out->jump_held = true; break;
			case TOUCH_WIDGET_TURBO:     turbo_held = true; break;
			case TOUCH_WIDGET_START:     out->start_held = true; break;
			case TOUCH_WIDGET_COIN_BACK: out->coin_held = true; break;
			case TOUCH_WIDGET_SERVICE:   out->service_held = true; break;
			default: break;
		}
	}

	/* Movement mode physics -> fractional trackball pulse reservoir */
	if (tc->cfg.move_mode == TOUCH_MOVE_TRACKBALL)
	{
		if (move_held)
		{
			/* Gentle damping when finger rests stationary on the ball */
			tc->vel_x *= 0.62f;
			tc->vel_y *= 0.62f;
			tc->nub_offset_x *= 0.75f;
			tc->nub_offset_y *= 0.75f;
		}
		else
		{
			/* Free-spinning heavy arcade ball inertia */
			tc->accum_x += tc->vel_x;
			tc->accum_y += tc->vel_y;
			tc->vel_x *= tc->cfg.trackball_friction;
			tc->vel_y *= tc->cfg.trackball_friction;
			if (hypotf(tc->vel_x, tc->vel_y) < 0.12f)
			{
				tc->vel_x = 0.0f;
				tc->vel_y = 0.0f;
			}
		}
	}
	else if (tc->cfg.move_mode == TOUCH_MOVE_STICK)
	{
		float hw, hh;
		widget_half_extents(tc, TOUCH_WIDGET_MOVE, &hw, &hh);
		const float max_r = fmaxf(hw * 0.62f, 1.0f);
		const float nx = tc->nub_offset_x / max_r;
		const float ny = tc->nub_offset_y / max_r;
		const float mag = hypotf(nx, ny);
		const float dz = tc->cfg.stick_deadzone;

		if (move_held && mag > dz)
		{
			const float norm_mag = clampf((mag - dz) / (1.0f - dz), 0.0f, 1.0f);
			/* Subtle 1.35 exponent gives surgical low-speed control + snappy top speed */
			const float curved = powf(norm_mag, 1.35f) * 96.0f * tc->cfg.trackball_sensitivity;
			tc->vel_x = (nx / mag) * curved;
			tc->vel_y = (ny / mag) * curved;
			tc->accum_x += tc->vel_x;
			tc->accum_y += tc->vel_y;

			if (nx < -0.45f) out->actions_held |= TOUCH_ACT_LEFT;
			if (nx >  0.45f) out->actions_held |= TOUCH_ACT_RIGHT;
			if (ny < -0.45f) out->actions_held |= TOUCH_ACT_UP;
			if (ny >  0.45f) out->actions_held |= TOUCH_ACT_DOWN;
		}
		else
		{
			tc->vel_x = 0.0f;
			tc->vel_y = 0.0f;
		}
	}
	else /* TOUCH_MOVE_DPAD */
	{
		float hw, hh;
		widget_half_extents(tc, TOUCH_WIDGET_MOVE, &hw, &hh);
		const float nx = tc->nub_offset_x / fmaxf(hw * 0.62f, 1.0f);
		const float ny = tc->nub_offset_y / fmaxf(hw * 0.62f, 1.0f);
		int dir_x = 0, dir_y = 0;

		if (move_held && hypotf(nx, ny) > 0.18f)
		{
			if (nx < -0.32f) { dir_x = -1; out->actions_held |= TOUCH_ACT_LEFT; }
			if (nx >  0.32f) { dir_x =  1; out->actions_held |= TOUCH_ACT_RIGHT; }
			if (ny < -0.32f) { dir_y = -1; out->actions_held |= TOUCH_ACT_UP; }
			if (ny >  0.32f) { dir_y =  1; out->actions_held |= TOUCH_ACT_DOWN; }
		}

		if (dir_x == 0)
			tc->vel_x = 0.0f;
		else
			tc->vel_x = clampf(tc->vel_x + dir_x * DPAD_RAMP_RATE, -DPAD_MAX_SPEED, DPAD_MAX_SPEED);

		if (dir_y == 0)
			tc->vel_y = 0.0f;
		else
			tc->vel_y = clampf(tc->vel_y + dir_y * DPAD_RAMP_RATE, -DPAD_MAX_SPEED, DPAD_MAX_SPEED);

		if (dir_x != 0 && dir_y != 0)
		{
			tc->accum_x += tc->vel_x * 0.70710678f;
			tc->accum_y += tc->vel_y * 0.70710678f;
		}
		else
		{
			tc->accum_x += tc->vel_x;
			tc->accum_y += tc->vel_y;
		}
	}

	/* Turbo Roll button: boost along current velocity or isometric up-right run */
	if (turbo_held)
	{
		float dir_x = tc->accum_x + tc->vel_x;
		float dir_y = tc->accum_y + tc->vel_y;
		const float len = hypotf(dir_x, dir_y);

		if (len > 2.5f)
		{
			dir_x /= len;
			dir_y /= len;
		}
		else
		{
			/* SegaSonic standard isometric stage progression angle (up-right) */
			dir_x =  0.70710678f;
			dir_y = -0.70710678f;
		}
		tc->accum_x = dir_x * TURBO_SPEED;
		tc->accum_y = dir_y * TURBO_SPEED;
	}

	/* Bound reservoir and emit strictly int8-safe [-120, +120] trackball counts */
	tc->accum_x = clampf(tc->accum_x, -240.0f, 240.0f);
	tc->accum_y = clampf(tc->accum_y, -240.0f, 240.0f);

	{
		int emit_x = (int)tc->accum_x;
		int emit_y = (int)tc->accum_y;

		if (emit_x >  TRACKBALL_SAFE_MAX) emit_x =  TRACKBALL_SAFE_MAX;
		if (emit_x < -TRACKBALL_SAFE_MAX) emit_x = -TRACKBALL_SAFE_MAX;
		if (emit_y >  TRACKBALL_SAFE_MAX) emit_y =  TRACKBALL_SAFE_MAX;
		if (emit_y < -TRACKBALL_SAFE_MAX) emit_y = -TRACKBALL_SAFE_MAX;

		tc->accum_x -= (float)emit_x;
		tc->accum_y -= (float)emit_y;

		out->track_dx = (int16_t)emit_x;
		out->track_dy = (int16_t)emit_y;

		/* Advance visual rolling dot-grid phase & optional bearing tick haptics */
		tc->ball_roll_u = fmodf(tc->ball_roll_u - (float)emit_x * 0.0028f + 10.0f, 1.0f);
		tc->ball_roll_v = fmodf(tc->ball_roll_v - (float)emit_y * 0.0028f + 10.0f, 1.0f);
		tc->tick_dist_accum += hypotf((float)emit_x, (float)emit_y);
		if (tc->tick_dist_accum >= 52.0f)
		{
			tc->tick_dist_accum = fmodf(tc->tick_dist_accum, 52.0f);
			if (tc->cfg.move_mode == TOUCH_MOVE_TRACKBALL)
				tc->pending_haptics |= TOUCH_HAPTIC_TICK;
		}

		/* Trackball swipe menu navigation for Console Edition menus */
		if (tc->cfg.move_mode == TOUCH_MOVE_TRACKBALL)
		{
			if (emit_x <= -22) out->actions_held |= TOUCH_ACT_LEFT;
			if (emit_x >=  22) out->actions_held |= TOUCH_ACT_RIGHT;
			if (emit_y <= -22) out->actions_held |= TOUCH_ACT_UP;
			if (emit_y >=  22) out->actions_held |= TOUCH_ACT_DOWN;
		}
	}

	if (out->jump_held)
		out->actions_held |= TOUCH_ACT_CONFIRM;
	if (out->coin_held && tc->console_mode)
		out->actions_held |= TOUCH_ACT_BACK;
	if (out->start_held)
		out->actions_held |= TOUCH_ACT_PAUSE | (tc->console_mode ? 0u : TOUCH_ACT_CONFIRM);

	out->actions_pressed = out->actions_held & ~tc->prev_actions;
	tc->prev_actions = out->actions_held;

	if (tc->cfg.haptics_enabled)
		out->haptic_events = tc->pending_haptics;
	tc->pending_haptics = 0;
}

/* ---- Batched UI Vertex Generation (HUD + Quick Menu + Layout Editor) ---- */

static void push_uv_quad(ui_quad_batch *b, float x0, float y0, float x1, float y1,
	ui_uv_rect uv, uint8_t r, uint8_t g, uint8_t bl, uint8_t a)
{
	if (!b || b->quad_count >= UI_MAX_QUADS || a == 0)
		return;

	{
		ui_vertex *v = &b->verts[b->quad_count * 4];
		v[0] = (ui_vertex){ x0, y0, uv.u0, uv.v0, r, g, bl, a };
		v[1] = (ui_vertex){ x1, y0, uv.u1, uv.v0, r, g, bl, a };
		v[2] = (ui_vertex){ x1, y1, uv.u1, uv.v1, r, g, bl, a };
		v[3] = (ui_vertex){ x0, y1, uv.u0, uv.v1, r, g, bl, a };
		b->quad_count++;
	}
}

static void push_sprite(ui_quad_batch *b, ui_sprite_id id, rect_f rc,
	uint8_t r, uint8_t g, uint8_t bl, uint8_t a)
{
	push_uv_quad(b, rc.x0, rc.y0, rc.x1, rc.y1, ui_atlas_get_sprite_uv(id), r, g, bl, a);
}

static void push_solid_rect(ui_quad_batch *b, rect_f rc, uint8_t r, uint8_t g, uint8_t bl, uint8_t a)
{
	push_sprite(b, UI_SPRITE_WHITE, rc, r, g, bl, a);
}

static void push_outline_rect(ui_quad_batch *b, rect_f rc, float thick,
	uint8_t r, uint8_t g, uint8_t bl, uint8_t a)
{
	push_solid_rect(b, (rect_f){ rc.x0, rc.y0, rc.x1, rc.y0 + thick }, r, g, bl, a);
	push_solid_rect(b, (rect_f){ rc.x0, rc.y1 - thick, rc.x1, rc.y1 }, r, g, bl, a);
	push_solid_rect(b, (rect_f){ rc.x0, rc.y0 + thick, rc.x0 + thick, rc.y1 - thick }, r, g, bl, a);
	push_solid_rect(b, (rect_f){ rc.x1 - thick, rc.y0 + thick, rc.x1, rc.y1 - thick }, r, g, bl, a);
}

static void push_text_centered(ui_quad_batch *b, const char *text, float cx, float cy,
	float glyph_h, uint8_t r, uint8_t g, uint8_t bl, uint8_t a)
{
	if (!text || !*text || a == 0)
		return;

	const size_t len = strlen(text);
	const float advance = glyph_h * 0.78f;
	const float total_w = (float)len * advance;
	float x = cx - total_w * 0.5f;
	const float y0 = cy - glyph_h * 0.5f;
	const float y1 = cy + glyph_h * 0.5f;

	for (size_t i = 0; i < len; i++, x += advance)
	{
		if (text[i] == ' ')
			continue;
		push_uv_quad(b, x, y0, x + glyph_h, y1, ui_atlas_get_glyph_uv(text[i]), r, g, bl, a);
	}
}

static bool is_widget_pressed(const touch_controls *tc, touch_widget_id id)
{
	if (tc->edit_mode || tc->quick_menu_open)
		return false;
	for (int i = 0; i < TOUCH_MAX_FINGERS; i++)
	{
		if (tc->fingers[i].active && tc->fingers[i].owner_widget == (int)id)
			return true;
	}
	return false;
}

static void player_accent_rgb(int player_color, uint8_t *r, uint8_t *g, uint8_t *b)
{
	switch (player_color)
	{
		case 1: /* Mighty Crimson */
			*r = 238; *g = 62; *b = 76;
			break;
		case 2: /* Ray Gold */
			*r = 250; *g = 198; *b = 46;
			break;
		default: /* Sonic Cobalt Blue */
			*r = 46; *g = 138; *b = 255;
			break;
	}
}

static void draw_single_widget(const touch_controls *tc, ui_quad_batch *batch, touch_widget_id id, float vis_alpha)
{
	const touch_widget_cfg *wc = &tc->cfg.widgets[id];
	float cx, cy, hw, hh;
	const bool pressed = is_widget_pressed(tc, id);
	uint8_t acc_r, acc_g, acc_b;

	if (!wc->visible && !tc->edit_mode)
		return;

	widget_center_px(tc, id, &cx, &cy);
	widget_half_extents(tc, id, &hw, &hh);
	player_accent_rgb(tc->cfg.player_color, &acc_r, &acc_g, &acc_b);

	if (id == TOUCH_WIDGET_MOVE && tc->move_active && tc->cfg.floating_origin && !tc->edit_mode)
	{
		cx = tc->move_origin_x;
		cy = tc->move_origin_y;
	}

	float alpha_f = vis_alpha * tc->cfg.master_opacity * wc->opacity;
	if (!wc->visible && tc->edit_mode)
		alpha_f = 0.26f; /* Dim ghost preview when hidden in Editor */
	else if (pressed)
		alpha_f = clampf(alpha_f * 1.22f, 0.0f, 1.0f);

	const uint8_t alpha = (uint8_t)(clampf(alpha_f, 0.0f, 1.0f) * 255.0f + 0.5f);
	if (alpha == 0)
		return;

	const float press_scale = pressed ? 0.94f : 1.0f;
	const rect_f box = {
		cx - hw * press_scale, cy - hh * press_scale,
		cx + hw * press_scale, cy + hh * press_scale
	};

	switch (id)
	{
		case TOUCH_WIDGET_MOVE:
		{
			if (tc->cfg.move_mode == TOUCH_MOVE_TRACKBALL)
			{
				const float ball_r = hw * 0.72f;
				const float bx = cx + tc->nub_offset_x * 0.45f;
				const float by = cy + tc->nub_offset_y * 0.45f;
				const rect_f ball_rc = { bx - ball_r, by - ball_r, bx + ball_r, by + ball_r };

				push_sprite(batch, UI_SPRITE_TRACKBALL_BASE, box, 225, 232, 248, alpha);
				push_sprite(batch, UI_SPRITE_TRACKBALL_BALL, ball_rc, acc_r, acc_g, acc_b, alpha);

				/* Shifted dot-grid UV inside the trackball for vivid rolling sensation */
				{
					ui_uv_rect duv = ui_atlas_get_sprite_uv(UI_SPRITE_TRACKBALL_DOTS);
					const float uw = (duv.u1 - duv.u0) * 0.72f;
					const float vh = (duv.v1 - duv.v0) * 0.72f;
					const float u_off = duv.u0 + (duv.u1 - duv.u0 - uw) * tc->ball_roll_u;
					const float v_off = duv.v0 + (duv.v1 - duv.v0 - vh) * tc->ball_roll_v;
					const ui_uv_rect ruv = { u_off, v_off, u_off + uw, v_off + vh };
					push_uv_quad(batch,
						bx - ball_r * 0.86f, by - ball_r * 0.86f,
						bx + ball_r * 0.86f, by + ball_r * 0.86f,
						ruv, 255, 255, 255, (uint8_t)(alpha * 0.65f));
				}
			}
			else if (tc->cfg.move_mode == TOUCH_MOVE_STICK)
			{
				const float nub_r = hw * 0.42f;
				const float nx = cx + tc->nub_offset_x;
				const float ny = cy + tc->nub_offset_y;
				push_sprite(batch, UI_SPRITE_STICK_RING, box, 210, 228, 255, alpha);
				push_sprite(batch, UI_SPRITE_STICK_NUB,
					(rect_f){ nx - nub_r, ny - nub_r, nx + nub_r, ny + nub_r },
					acc_r, acc_g, acc_b, alpha);
			}
			else
			{
				push_sprite(batch, UI_SPRITE_DPAD, box, 220, 230, 248, alpha);
			}
			break;
		}

		case TOUCH_WIDGET_JUMP:
			push_sprite(batch, UI_SPRITE_BTN_FACE, box, acc_r, acc_g, acc_b, alpha);
			push_sprite(batch, UI_SPRITE_BTN_RING, box, 255, 255, 255, alpha);
			push_text_centered(batch, "JUMP", cx, cy, hh * 0.34f, 255, 255, 255, alpha);
			break;

		case TOUCH_WIDGET_TURBO:
		{
			const float ir = hh * 0.48f;
			push_sprite(batch, UI_SPRITE_BTN_FACE, box, 245, 175, 32, alpha);
			push_sprite(batch, UI_SPRITE_BTN_RING, box, 255, 240, 185, alpha);
			push_sprite(batch, UI_SPRITE_ICON_BOLT,
				(rect_f){ cx - ir, cy - ir * 1.15f, cx + ir, cy + ir * 0.85f },
				255, 255, 255, alpha);
			push_text_centered(batch, "DASH", cx, cy + hh * 0.48f, hh * 0.28f, 255, 255, 255, alpha);
			break;
		}

		case TOUCH_WIDGET_START:
			push_sprite(batch, UI_SPRITE_PILL, box, 42, 186, 118, alpha);
			push_text_centered(batch, tc->console_mode ? "PAUSE" : "START",
				cx, cy, hh * 0.62f, 255, 255, 255, alpha);
			break;

		case TOUCH_WIDGET_COIN_BACK:
			push_sprite(batch, UI_SPRITE_PILL, box,
				tc->console_mode ? 215 : 228,
				tc->console_mode ? 76  : 162,
				tc->console_mode ? 88  : 38, alpha);
			push_text_centered(batch, tc->console_mode ? "BACK" : "COIN",
				cx, cy, hh * 0.62f, 255, 255, 255, alpha);
			break;

		case TOUCH_WIDGET_SERVICE:
			push_sprite(batch, UI_SPRITE_PILL, box, 110, 124, 152, alpha);
			push_text_centered(batch, "TEST", cx, cy, hh * 0.62f, 255, 255, 255, alpha);
			break;

		case TOUCH_WIDGET_MENU:
		default:
		{
			const float ir = hh * 0.62f;
			push_sprite(batch, UI_SPRITE_BTN_FACE, box, 48, 58, 82, alpha);
			push_sprite(batch, UI_SPRITE_BTN_RING, box, 175, 195, 235, alpha);
			push_sprite(batch, UI_SPRITE_ICON_GEAR,
				(rect_f){ cx - ir, cy - ir, cx + ir, cy + ir },
				240, 246, 255, alpha);
			break;
		}
	}

	/* Selection box in Layout Editor */
	if (tc->edit_mode)
	{
		const bool sel = ((int)id == tc->selected_widget);
		const rect_f sel_rc = { cx - hw - 6.0f, cy - hh - 6.0f, cx + hw + 6.0f, cy + hh + 6.0f };
		if (sel)
			push_sprite(batch, UI_SPRITE_SELECT_BOX, sel_rc, 72, 224, 255, 255);
		else
			push_outline_rect(batch, sel_rc, 1.5f, 255, 255, 255, wc->visible ? 90 : 45);
	}
}

static void draw_ui_card_btn(ui_quad_batch *batch, rect_f rc, const char *label,
	uint8_t r, uint8_t g, uint8_t b, bool highlight)
{
	push_solid_rect(batch, rc, r, g, b, 232);
	push_outline_rect(batch, rc, highlight ? 2.5f : 1.5f,
		highlight ? 90 : 170, highlight ? 235 : 195, 255, 235);
	push_text_centered(batch, label,
		(rc.x0 + rc.x1) * 0.5f, (rc.y0 + rc.y1) * 0.5f,
		fminf((rc.y1 - rc.y0) * 0.44f, (rc.x1 - rc.x0) / fmaxf((float)strlen(label) * 0.82f, 1.0f)),
		255, 255, 255, 255);
}

void touch_controls_build_vertices(const touch_controls *tc, ui_quad_batch *batch)
{
	if (!tc || !batch)
		return;

	batch->quad_count = 0;

	const float sw = (float)(tc->screen_w > 0 ? tc->screen_w : 1280);
	const float sh = (float)(tc->screen_h > 0 ? tc->screen_h : 720);

	/* 1. Optional Grid Alignment Overlay in Layout Editor */
	if (tc->edit_mode)
	{
		push_solid_rect(batch, (rect_f){ 0.0f, 0.0f, sw, sh }, 8, 12, 22, 96);
		if (tc->cfg.grid_snap)
		{
			for (int g = 1; g < 20; g++)
			{
				const float gx = sw * ((float)g / 20.0f);
				const float gy = sh * ((float)g / 20.0f);
				const uint8_t a = (g == 10) ? 105 : 36;
				push_solid_rect(batch, (rect_f){ gx - 0.75f, 0.0f, gx + 0.75f, sh }, 120, 200, 255, a);
				push_solid_rect(batch, (rect_f){ 0.0f, gy - 0.75f, sw, gy + 0.75f }, 120, 200, 255, a);
			}
		}
		else
		{
			/* Always show subtle center crosshairs */
			push_solid_rect(batch, (rect_f){ sw * 0.5f - 0.8f, 0.0f, sw * 0.5f + 0.8f, sh }, 120, 200, 255, 64);
			push_solid_rect(batch, (rect_f){ 0.0f, sh * 0.5f - 0.8f, sw, sh * 0.5f + 0.8f }, 120, 200, 255, 64);
		}
	}

	/* 2. Touch Control Widgets */
	if (tc->hud_visibility > 0.01f || tc->edit_mode)
	{
		for (int w = 0; w < TOUCH_WIDGET_COUNT; w++)
			draw_single_widget(tc, batch, (touch_widget_id)w, tc->edit_mode ? 1.0f : tc->hud_visibility);
	}

	/* 3. Optional Live FPS & GPU Compression Badge */
	if (tc->show_fps && !tc->edit_mode && !tc->quick_menu_open)
	{
		char fps_buf[96];
		const float bh = sh * 0.042f;
		const float bw = bh * 9.5f;
		const rect_f badge = { sh * 0.02f, sh * 0.02f, sh * 0.02f + bw, sh * 0.02f + bh };

		snprintf(fps_buf, sizeof(fps_buf), "%.1f FPS | %s", tc->current_fps, tc->status_gpu);
		push_solid_rect(batch, badge, 10, 16, 28, 185);
		push_outline_rect(batch, badge, 1.0f, 70, 175, 255, 160);
		push_text_centered(batch, fps_buf, (badge.x0 + badge.x1) * 0.5f, (badge.y0 + badge.y1) * 0.5f,
			bh * 0.52f, 140, 245, 190, 245);
	}

	/* 4. Interactive HUD Layout Editor Inspector Dock */
	if (tc->edit_mode)
	{
		editor_layout ed;
		char info[128];
		char mode_lbl[24];
		char float_lbl[20];
		char grid_lbl[20];
		char vis_lbl[20];
		const touch_widget_cfg *sel = &tc->cfg.widgets[tc->selected_widget];
		static const char *const mode_names[3] = { "BALL", "STICK", "DPAD" };

		compute_editor_layout(tc, &ed);
		push_solid_rect(batch, ed.panel, 14, 20, 34, 236);
		push_outline_rect(batch, ed.panel, 2.0f, 72, 205, 255, 235);

		snprintf(info, sizeof(info), "%s | SIZE %d%% | OPACITY %d%% (DRAG OR PINCH)",
			k_widget_titles[tc->selected_widget],
			(int)(sel->scale * 100.0f + 0.5f),
			(int)(sel->opacity * 100.0f + 0.5f));
		push_text_centered(batch, info,
			(ed.panel.x0 + ed.panel.x1) * 0.5f,
			ed.panel.y0 + (ed.panel.y1 - ed.panel.y0) * 0.17f,
			(ed.panel.y1 - ed.panel.y0) * 0.145f,
			235, 245, 255, 255);

		snprintf(vis_lbl, sizeof(vis_lbl), sel->visible ? "SHOWN" : "HIDDEN");
		snprintf(mode_lbl, sizeof(mode_lbl), "PAD:%s", mode_names[tc->cfg.move_mode]);
		snprintf(float_lbl, sizeof(float_lbl), "FLOAT:%s", tc->cfg.floating_origin ? "ON" : "OFF");
		snprintf(grid_lbl, sizeof(grid_lbl), "GRID:%s", tc->cfg.grid_snap ? "ON" : "OFF");

		draw_ui_card_btn(batch, ed.btns[ED_BTN_SIZE_DEC],   "SIZE -",   34, 48, 76, false);
		draw_ui_card_btn(batch, ed.btns[ED_BTN_SIZE_INC],   "SIZE +",   34, 48, 76, false);
		draw_ui_card_btn(batch, ed.btns[ED_BTN_ALPHA_DEC],  "ALPHA -",  34, 48, 76, false);
		draw_ui_card_btn(batch, ed.btns[ED_BTN_ALPHA_INC],  "ALPHA +",  34, 48, 76, false);
		draw_ui_card_btn(batch, ed.btns[ED_BTN_VISIBILITY], vis_lbl,
			sel->visible ? 32 : 96, sel->visible ? 112 : 44, sel->visible ? 86 : 56, sel->visible);
		draw_ui_card_btn(batch, ed.btns[ED_BTN_MODE],       mode_lbl,   42, 82, 148, true);

		draw_ui_card_btn(batch, ed.btns[ED_BTN_FLOAT],      float_lbl,  36, 56, 88, tc->cfg.floating_origin);
		draw_ui_card_btn(batch, ed.btns[ED_BTN_GRID],       grid_lbl,   36, 56, 88, tc->cfg.grid_snap);
		draw_ui_card_btn(batch, ed.btns[ED_BTN_PRESET],     "PRESET",   72, 58, 128, false);
		draw_ui_card_btn(batch, ed.btns[ED_BTN_RESET],      "RESET",    142, 52, 58, false);
		draw_ui_card_btn(batch, ed.btns[ED_BTN_DONE],       "SAVE DONE", 32, 152, 96, true);
	}

	/* 5. Quick In-Game Menu Modal */
	if (tc->quick_menu_open)
	{
		qmenu_layout qm;
		char move_lbl[32], filt_lbl[40], asp_lbl[40], gpu_lbl[64], fps_lbl[28];
		static const char *const mode_names[3] = { "TRACKBALL", "STICK", "D-PAD" };

		compute_qmenu_layout(tc, &qm);
		push_solid_rect(batch, (rect_f){ 0.0f, 0.0f, sw, sh }, 4, 8, 16, 168);
		push_solid_rect(batch, qm.panel, 16, 24, 42, 244);
		push_outline_rect(batch, qm.panel, 2.5f, 64, 178, 255, 245);

		push_text_centered(batch, "SEGASONIC QUICK MENU",
			(qm.panel.x0 + qm.panel.x1) * 0.5f,
			qm.panel.y0 + (qm.panel.y1 - qm.panel.y0) * 0.10f,
			(qm.panel.y1 - qm.panel.y0) * 0.068f,
			255, 224, 72, 255);

		snprintf(move_lbl, sizeof(move_lbl), "PAD: %s", mode_names[tc->cfg.move_mode]);
		snprintf(filt_lbl, sizeof(filt_lbl), "FILTER: %s", tc->status_filter);
		snprintf(asp_lbl,  sizeof(asp_lbl),  "ASPECT: %s", tc->status_aspect);
		snprintf(gpu_lbl,  sizeof(gpu_lbl),  "GPU: %s", tc->status_gpu);
		snprintf(fps_lbl,  sizeof(fps_lbl),  "FPS HUD: %s", tc->show_fps ? "ON" : "OFF");

		draw_ui_card_btn(batch, qm.btns[QM_BTN_RESUME],      "RESUME GAME",       32, 148, 92, true);
		draw_ui_card_btn(batch, qm.btns[QM_BTN_EDIT_HUD],    "EDIT TOUCH HUD",    38, 108, 196, true);
		draw_ui_card_btn(batch, qm.btns[QM_BTN_MOVE_MODE],   move_lbl,            36, 54, 88, false);
		draw_ui_card_btn(batch, qm.btns[QM_BTN_FILTER],      filt_lbl,            36, 54, 88, false);
		draw_ui_card_btn(batch, qm.btns[QM_BTN_ASPECT],      asp_lbl,             36, 54, 88, false);
		draw_ui_card_btn(batch, qm.btns[QM_BTN_ETC2_STREAM], gpu_lbl,             46, 68, 108, false);
		draw_ui_card_btn(batch, qm.btns[QM_BTN_FPS],         fps_lbl,             36, 54, 88, tc->show_fps);
		draw_ui_card_btn(batch, qm.btns[QM_BTN_EXIT],        "EXIT TO LAUNCHER",  156, 48, 58, false);
	}
}
