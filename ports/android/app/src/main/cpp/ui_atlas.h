/*
 * SegaSonic The Hedgehog — Android Port
 * Procedural 512x512 RGBA8888 UI, Touch Control & Bitmap Font Atlas
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 * Generates all touch-control vector/shaded sprites, editor gizmos, and an
 * embedded 8x8 ASCII font into a single 512x512 RGBA8888 texture sheet so the
 * entire HUD, Quick Menu, FPS overlay, and Touch Layout Editor render in a
 * single batched draw call (compressed to GL_COMPRESSED_RGBA8_ETC2_EAC on GLES3).
 */

#ifndef SEGASONIC_ANDROID_UI_ATLAS_H
#define SEGASONIC_ANDROID_UI_ATLAS_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

#define UI_ATLAS_WIDTH   512
#define UI_ATLAS_HEIGHT  512
#define UI_ATLAS_VERSION 2u

typedef enum ui_sprite_id
{
	UI_SPRITE_WHITE = 0,       /* Solid white 4x4 interior texel for tinted quads/lines */
	UI_SPRITE_TRACKBALL_BASE,  /* Arcade trackball metallic housing & recessed well */
	UI_SPRITE_TRACKBALL_BALL,  /* Glossy 3D sphere (tintable) */
	UI_SPRITE_TRACKBALL_DOTS,  /* Rolling specular dot pattern shifted by ball UV */
	UI_SPRITE_STICK_RING,      /* Virtual analog outer ring */
	UI_SPRITE_STICK_NUB,       /* Virtual analog thumb cap */
	UI_SPRITE_DPAD,            /* 8-way cross D-Pad with directional chevrons */
	UI_SPRITE_BTN_FACE,        /* Glossy circular button face (tinted per button) */
	UI_SPRITE_BTN_RING,        /* Crisp anti-aliased button rim highlight */
	UI_SPRITE_PILL,            /* Rounded pill background for START / COIN / MENU */
	UI_SPRITE_SELECT_BOX,      /* Editor selection ring with corner handles */
	UI_SPRITE_ICON_GEAR,       /* Quick menu / settings gear icon */
	UI_SPRITE_ICON_BOLT,       /* Turbo / spin-dash lightning icon */
	UI_SPRITE_COUNT
} ui_sprite_id;

typedef struct ui_uv_rect
{
	float u0, v0, u1, v1;
} ui_uv_rect;

/* Generate the 512x512 RGBA8888 (R,G,B,A byte order) atlas into `out_rgba` (1,048,576 bytes). */
void ui_atlas_generate_rgba8(uint8_t *out_rgba);

/* Query normalized [0..1] UV coordinates for a named UI sprite. */
ui_uv_rect ui_atlas_get_sprite_uv(ui_sprite_id id);

/* Query normalized [0..1] UV coordinates for an ASCII glyph (32..126, 8x8 cell). */
ui_uv_rect ui_atlas_get_glyph_uv(char ch);

#ifdef __cplusplus
}
#endif

#endif /* SEGASONIC_ANDROID_UI_ATLAS_H */
