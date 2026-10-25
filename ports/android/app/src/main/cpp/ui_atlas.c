/*
 * SegaSonic The Hedgehog — Android Port
 * Procedural 512x512 RGBA8888 UI, Touch Control & Bitmap Font Atlas
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "ui_atlas.h"

#include <math.h>
#include <stdbool.h>
#include <stddef.h>
#include <string.h>

/* ---- Compact 8x8 ASCII font bitmaps (chars 32..126, MSB = left pixel) ---- */

static const uint8_t k_font8x8[95][8] = {
	{ 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 }, /* 32 ' ' */
	{ 0x18, 0x3c, 0x3c, 0x18, 0x18, 0x00, 0x18, 0x00 }, /* 33 '!' */
	{ 0x66, 0x66, 0x24, 0x00, 0x00, 0x00, 0x00, 0x00 }, /* 34 '"' */
	{ 0x6c, 0x6c, 0xfe, 0x6c, 0xfe, 0x6c, 0x6c, 0x00 }, /* 35 '#' */
	{ 0x18, 0x7e, 0xc0, 0x7c, 0x06, 0xfc, 0x18, 0x00 }, /* 36 '$' */
	{ 0x00, 0xc6, 0xcc, 0x18, 0x30, 0x66, 0xc6, 0x00 }, /* 37 '%' */
	{ 0x38, 0x6c, 0x38, 0x76, 0xdc, 0xcc, 0x76, 0x00 }, /* 38 '&' */
	{ 0x18, 0x18, 0x30, 0x00, 0x00, 0x00, 0x00, 0x00 }, /* 39 ''' */
	{ 0x0c, 0x18, 0x30, 0x30, 0x30, 0x18, 0x0c, 0x00 }, /* 40 '(' */
	{ 0x30, 0x18, 0x0c, 0x0c, 0x0c, 0x18, 0x30, 0x00 }, /* 41 ')' */
	{ 0x00, 0x66, 0x3c, 0xff, 0x3c, 0x66, 0x00, 0x00 }, /* 42 '*' */
	{ 0x00, 0x18, 0x18, 0x7e, 0x18, 0x18, 0x00, 0x00 }, /* 43 '+' */
	{ 0x00, 0x00, 0x00, 0x00, 0x00, 0x18, 0x18, 0x30 }, /* 44 ',' */
	{ 0x00, 0x00, 0x00, 0x7e, 0x00, 0x00, 0x00, 0x00 }, /* 45 '-' */
	{ 0x00, 0x00, 0x00, 0x00, 0x00, 0x18, 0x18, 0x00 }, /* 46 '.' */
	{ 0x06, 0x0c, 0x18, 0x30, 0x60, 0xc0, 0x80, 0x00 }, /* 47 '/' */
	{ 0x7c, 0xc6, 0xce, 0xd6, 0xe6, 0xc6, 0x7c, 0x00 }, /* 48 '0' */
	{ 0x18, 0x38, 0x18, 0x18, 0x18, 0x18, 0x7e, 0x00 }, /* 49 '1' */
	{ 0x7c, 0xc6, 0x06, 0x1c, 0x70, 0xc6, 0xfe, 0x00 }, /* 50 '2' */
	{ 0x7c, 0xc6, 0x06, 0x3c, 0x06, 0xc6, 0x7c, 0x00 }, /* 51 '3' */
	{ 0x1c, 0x3c, 0x6c, 0xcc, 0xfe, 0x0c, 0x1e, 0x00 }, /* 52 '4' */
	{ 0xfe, 0xc0, 0xfc, 0x06, 0x06, 0xc6, 0x7c, 0x00 }, /* 53 '5' */
	{ 0x38, 0x60, 0xc0, 0xfc, 0xc6, 0xc6, 0x7c, 0x00 }, /* 54 '6' */
	{ 0xfe, 0xc6, 0x0c, 0x18, 0x30, 0x30, 0x30, 0x00 }, /* 55 '7' */
	{ 0x7c, 0xc6, 0xc6, 0x7c, 0xc6, 0xc6, 0x7c, 0x00 }, /* 56 '8' */
	{ 0x7c, 0xc6, 0xc6, 0x7e, 0x06, 0x0c, 0x78, 0x00 }, /* 57 '9' */
	{ 0x00, 0x18, 0x18, 0x00, 0x00, 0x18, 0x18, 0x00 }, /* 58 ':' */
	{ 0x00, 0x18, 0x18, 0x00, 0x00, 0x18, 0x18, 0x30 }, /* 59 ';' */
	{ 0x0c, 0x18, 0x30, 0x60, 0x30, 0x18, 0x0c, 0x00 }, /* 60 '<' */
	{ 0x00, 0x00, 0x7e, 0x00, 0x7e, 0x00, 0x00, 0x00 }, /* 61 '=' */
	{ 0x30, 0x18, 0x0c, 0x06, 0x0c, 0x18, 0x30, 0x00 }, /* 62 '>' */
	{ 0x7c, 0xc6, 0x0c, 0x18, 0x18, 0x00, 0x18, 0x00 }, /* 63 '?' */
	{ 0x7c, 0xc6, 0xde, 0xde, 0xde, 0xc0, 0x7c, 0x00 }, /* 64 '@' */
	{ 0x38, 0x6c, 0xc6, 0xc6, 0xfe, 0xc6, 0xc6, 0x00 }, /* 65 'A' */
	{ 0xfc, 0x66, 0x66, 0x7c, 0x66, 0x66, 0xfc, 0x00 }, /* 66 'B' */
	{ 0x3c, 0x66, 0xc0, 0xc0, 0xc0, 0x66, 0x3c, 0x00 }, /* 67 'C' */
	{ 0xf8, 0x6c, 0x66, 0x66, 0x66, 0x6c, 0xf8, 0x00 }, /* 68 'D' */
	{ 0xfe, 0x62, 0x68, 0x78, 0x68, 0x62, 0xfe, 0x00 }, /* 69 'E' */
	{ 0xfe, 0x62, 0x68, 0x78, 0x68, 0x60, 0xf0, 0x00 }, /* 70 'F' */
	{ 0x3c, 0x66, 0xc0, 0xc0, 0xce, 0x66, 0x3e, 0x00 }, /* 71 'G' */
	{ 0xc6, 0xc6, 0xc6, 0xfe, 0xc6, 0xc6, 0xc6, 0x00 }, /* 72 'H' */
	{ 0x3c, 0x18, 0x18, 0x18, 0x18, 0x18, 0x3c, 0x00 }, /* 73 'I' */
	{ 0x1e, 0x0c, 0x0c, 0x0c, 0xcc, 0xcc, 0x78, 0x00 }, /* 74 'J' */
	{ 0xe6, 0x66, 0x6c, 0x78, 0x6c, 0x66, 0xe6, 0x00 }, /* 75 'K' */
	{ 0xf0, 0x60, 0x60, 0x60, 0x62, 0x66, 0xfe, 0x00 }, /* 76 'L' */
	{ 0xc6, 0xee, 0xfe, 0xfe, 0xd6, 0xc6, 0xc6, 0x00 }, /* 77 'M' */
	{ 0xc6, 0xe6, 0xf6, 0xde, 0xce, 0xc6, 0xc6, 0x00 }, /* 78 'N' */
	{ 0x7c, 0xc6, 0xc6, 0xc6, 0xc6, 0xc6, 0x7c, 0x00 }, /* 79 'O' */
	{ 0xfc, 0x66, 0x66, 0x7c, 0x60, 0x60, 0xf0, 0x00 }, /* 80 'P' */
	{ 0x7c, 0xc6, 0xc6, 0xc6, 0xd6, 0xde, 0x7c, 0x0e }, /* 81 'Q' */
	{ 0xfc, 0x66, 0x66, 0x7c, 0x6c, 0x66, 0xe6, 0x00 }, /* 82 'R' */
	{ 0x7c, 0xc6, 0xe0, 0x7c, 0x0e, 0xc6, 0x7c, 0x00 }, /* 83 'S' */
	{ 0x7e, 0x7e, 0x5a, 0x18, 0x18, 0x18, 0x3c, 0x00 }, /* 84 'T' */
	{ 0xc6, 0xc6, 0xc6, 0xc6, 0xc6, 0xc6, 0x7c, 0x00 }, /* 85 'U' */
	{ 0xc6, 0xc6, 0xc6, 0xc6, 0x6c, 0x38, 0x10, 0x00 }, /* 86 'V' */
	{ 0xc6, 0xc6, 0xd6, 0xfe, 0xfe, 0xee, 0xc6, 0x00 }, /* 87 'W' */
	{ 0xc6, 0xc6, 0x6c, 0x38, 0x6c, 0xc6, 0xc6, 0x00 }, /* 88 'X' */
	{ 0x66, 0x66, 0x66, 0x3c, 0x18, 0x18, 0x3c, 0x00 }, /* 89 'Y' */
	{ 0xfe, 0xc6, 0x8c, 0x18, 0x32, 0x66, 0xfe, 0x00 }, /* 90 'Z' */
	{ 0x3c, 0x30, 0x30, 0x30, 0x30, 0x30, 0x3c, 0x00 }, /* 91 '[' */
	{ 0xc0, 0x60, 0x30, 0x18, 0x0c, 0x06, 0x02, 0x00 }, /* 92 '\' */
	{ 0x3c, 0x0c, 0x0c, 0x0c, 0x0c, 0x0c, 0x3c, 0x00 }, /* 93 ']' */
	{ 0x10, 0x38, 0x6c, 0xc6, 0x00, 0x00, 0x00, 0x00 }, /* 94 '^' */
	{ 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xff }, /* 95 '_' */
	{ 0x30, 0x18, 0x0c, 0x00, 0x00, 0x00, 0x00, 0x00 }, /* 96 '`' */
	{ 0x00, 0x00, 0x78, 0x0c, 0x7c, 0xcc, 0x76, 0x00 }, /* 97 'a' */
	{ 0xe0, 0x60, 0x7c, 0x66, 0x66, 0x66, 0xdc, 0x00 }, /* 98 'b' */
	{ 0x00, 0x00, 0x7c, 0xc6, 0xc0, 0xc6, 0x7c, 0x00 }, /* 99 'c' */
	{ 0x1c, 0x0c, 0x7c, 0xcc, 0xcc, 0xcc, 0x76, 0x00 }, /* 100 'd' */
	{ 0x00, 0x00, 0x7c, 0xc6, 0xfe, 0xc0, 0x7c, 0x00 }, /* 101 'e' */
	{ 0x3c, 0x66, 0x60, 0xf8, 0x60, 0x60, 0xf0, 0x00 }, /* 102 'f' */
	{ 0x00, 0x00, 0x76, 0xcc, 0xcc, 0x7c, 0x0c, 0xf8 }, /* 103 'g' */
	{ 0xe0, 0x60, 0x6c, 0x76, 0x66, 0x66, 0xe6, 0x00 }, /* 104 'h' */
	{ 0x18, 0x00, 0x38, 0x18, 0x18, 0x18, 0x3c, 0x00 }, /* 105 'i' */
	{ 0x06, 0x00, 0x06, 0x06, 0x06, 0x66, 0x66, 0x3c }, /* 106 'j' */
	{ 0xe0, 0x60, 0x66, 0x6c, 0x78, 0x6c, 0xe6, 0x00 }, /* 107 'k' */
	{ 0x38, 0x18, 0x18, 0x18, 0x18, 0x18, 0x3c, 0x00 }, /* 108 'l' */
	{ 0x00, 0x00, 0xec, 0xfe, 0xd6, 0xc6, 0xc6, 0x00 }, /* 109 'm' */
	{ 0x00, 0x00, 0xdc, 0x66, 0x66, 0x66, 0x66, 0x00 }, /* 110 'n' */
	{ 0x00, 0x00, 0x7c, 0xc6, 0xc6, 0xc6, 0x7c, 0x00 }, /* 111 'o' */
	{ 0x00, 0x00, 0xdc, 0x66, 0x66, 0x7c, 0x60, 0xf0 }, /* 112 'p' */
	{ 0x00, 0x00, 0x76, 0xcc, 0xcc, 0x7c, 0x0c, 0x1e }, /* 113 'q' */
	{ 0x00, 0x00, 0xdc, 0x76, 0x60, 0x60, 0xf0, 0x00 }, /* 114 'r' */
	{ 0x00, 0x00, 0x7e, 0xc0, 0x7c, 0x06, 0xfc, 0x00 }, /* 115 's' */
	{ 0x30, 0x30, 0xfc, 0x30, 0x30, 0x36, 0x1c, 0x00 }, /* 116 't' */
	{ 0x00, 0x00, 0xcc, 0xcc, 0xcc, 0xcc, 0x76, 0x00 }, /* 117 'u' */
	{ 0x00, 0x00, 0xc6, 0xc6, 0xc6, 0x6c, 0x38, 0x00 }, /* 118 'v' */
	{ 0x00, 0x00, 0xc6, 0xc6, 0xd6, 0xfe, 0x6c, 0x00 }, /* 119 'w' */
	{ 0x00, 0x00, 0xc6, 0x6c, 0x38, 0x6c, 0xc6, 0x00 }, /* 120 'x' */
	{ 0x00, 0x00, 0xc6, 0xc6, 0xce, 0x76, 0x06, 0x7c }, /* 121 'y' */
	{ 0x00, 0x00, 0xfe, 0x8c, 0x18, 0x32, 0xfe, 0x00 }, /* 122 'z' */
	{ 0x0e, 0x18, 0x18, 0x70, 0x18, 0x18, 0x0e, 0x00 }, /* 123 '{' */
	{ 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x00 }, /* 124 '|' */
	{ 0x70, 0x18, 0x18, 0x0e, 0x18, 0x18, 0x70, 0x00 }, /* 125 '}' */
	{ 0x76, 0xdc, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 }  /* 126 '~' */
};

typedef struct sprite_box
{
	int x, y, w, h;
} sprite_box;

static const sprite_box k_sprite_boxes[UI_SPRITE_COUNT] = {
	[UI_SPRITE_WHITE]          = { 260, 260,   8,   8 }, /* Inside 16x16 white block at (256,256) */
	[UI_SPRITE_TRACKBALL_BASE] = {   0,   0, 128, 128 },
	[UI_SPRITE_TRACKBALL_BALL] = { 128,   0, 128, 128 },
	[UI_SPRITE_TRACKBALL_DOTS] = { 256,   0, 128, 128 },
	[UI_SPRITE_STICK_RING]     = { 384,   0, 128, 128 },
	[UI_SPRITE_STICK_NUB]      = { 320, 128,  96,  96 },
	[UI_SPRITE_DPAD]           = {   0, 128, 128, 128 },
	[UI_SPRITE_BTN_FACE]       = { 128, 128,  96,  96 },
	[UI_SPRITE_BTN_RING]       = { 224, 128,  96,  96 },
	[UI_SPRITE_PILL]           = {   0, 256, 128,  64 },
	[UI_SPRITE_SELECT_BOX]     = { 416, 128,  96,  96 },
	[UI_SPRITE_ICON_GEAR]      = { 128, 256,  64,  64 },
	[UI_SPRITE_ICON_BOLT]      = { 192, 256,  64,  64 }
};

#define FONT_ORIGIN_X 0
#define FONT_ORIGIN_Y 352
#define FONT_CELL_W   24
#define FONT_CELL_H   24
#define FONT_COLS     16

static inline uint8_t clamp_byte(float v)
{
	if (v <= 0.0f) return 0;
	if (v >= 255.0f) return 255;
	return (uint8_t)(v + 0.5f);
}

static inline float smoothstepf(float edge0, float edge1, float x)
{
	float t = (x - edge0) / (edge1 - edge0);
	if (t < 0.0f) t = 0.0f;
	if (t > 1.0f) t = 1.0f;
	return t * t * (3.0f - 2.0f * t);
}

static inline void put_px(uint8_t *rgba, int x, int y, uint8_t r, uint8_t g, uint8_t b, uint8_t a)
{
	if ((unsigned)x >= UI_ATLAS_WIDTH || (unsigned)y >= UI_ATLAS_HEIGHT)
		return;
	{
		uint8_t *dst = rgba + ((size_t)y * UI_ATLAS_WIDTH + (size_t)x) * 4u;
		dst[0] = r;
		dst[1] = g;
		dst[2] = b;
		dst[3] = a;
	}
}

/* ---- Procedural Sprite Generators ---- */

static void draw_trackball_base(uint8_t *rgba, sprite_box b)
{
	const float cx = b.w * 0.5f - 0.5f;
	const float cy = b.h * 0.5f - 0.5f;
	const float r_outer = b.w * 0.485f;
	const float r_bezel = b.w * 0.405f;
	const float r_well  = b.w * 0.355f;

	for (int y = 0; y < b.h; y++)
	{
		for (int x = 0; x < b.w; x++)
		{
			const float dx = x - cx;
			const float dy = y - cy;
			const float dist = sqrtf(dx * dx + dy * dy);
			const float alpha = 1.0f - smoothstepf(r_outer - 1.4f, r_outer + 0.4f, dist);

			if (alpha <= 0.001f)
				continue;

			float lum;
			if (dist < r_well)
			{
				/* Deep recessed arcade socket well */
				const float t = dist / r_well;
				lum = 22.0f + 24.0f * t * t;
			}
			else if (dist < r_bezel)
			{
				/* Inner chrome chamfer */
				const float t = (dist - r_well) / (r_bezel - r_well);
				const float light = (-dx * 0.6f - dy * 0.8f) / (dist + 0.001f);
				lum = 120.0f + 65.0f * light * sinf(t * 3.14159265f);
			}
			else
			{
				/* Outer brushed housing ring with 4 subtle mounting screw indents */
				const float light = (-dx * 0.45f - dy * 0.75f) / (dist + 0.001f);
				const float rim = 1.0f - fabsf((dist - (r_bezel + r_outer) * 0.5f) / ((r_outer - r_bezel) * 0.5f));
				lum = 68.0f + 38.0f * light + 26.0f * rim;
			}

			put_px(rgba, b.x + x, b.y + y,
				clamp_byte(lum * 0.92f),
				clamp_byte(lum * 0.96f),
				clamp_byte(lum * 1.08f),
				clamp_byte(alpha * 255.0f));
		}
	}
}

static void draw_trackball_ball(uint8_t *rgba, sprite_box b)
{
	const float cx = b.w * 0.5f - 0.5f;
	const float cy = b.h * 0.5f - 0.5f;
	const float rad = b.w * 0.46f;

	for (int y = 0; y < b.h; y++)
	{
		for (int x = 0; x < b.w; x++)
		{
			const float nx = (x - cx) / rad;
			const float ny = (y - cy) / rad;
			const float r2 = nx * nx + ny * ny;
			const float dist = sqrtf(r2) * rad;
			const float alpha = 1.0f - smoothstepf(rad - 1.4f, rad + 0.4f, dist);

			if (alpha <= 0.001f)
				continue;

			const float nz = sqrtf(r2 < 1.0f ? (1.0f - r2) : 0.0f);
			/* Directional key light from top-left + subtle bottom bounce */
			const float lx = -0.42f, ly = -0.52f, lz = 0.743f;
			float ndotl = nx * lx + ny * ly + nz * lz;
			if (ndotl < 0.0f) ndotl = 0.0f;

			/* Half-vector specular highlight for glossy phenolic resin ball */
			const float hx = -0.22f, hy = -0.28f, hz = 0.934f;
			float ndoth = nx * hx + ny * hy + nz * hz;
			if (ndoth < 0.0f) ndoth = 0.0f;
			const float spec = powf(ndoth, 28.0f);
			const float fresnel = powf(1.0f - nz, 3.0f) * 0.22f;

			/* Neutral white-grey base so vertex color tints saturation cleanly */
			const float base = 0.34f + 0.64f * ndotl + fresnel;
			const float r = (base + spec * 0.85f) * 255.0f;
			const float g = (base + spec * 0.85f) * 255.0f;
			const float bl = (base + spec * 0.90f) * 255.0f;

			put_px(rgba, b.x + x, b.y + y,
				clamp_byte(r), clamp_byte(g), clamp_byte(bl),
				clamp_byte(alpha * 255.0f));
		}
	}
}

static void draw_trackball_dots(uint8_t *rgba, sprite_box b)
{
	/* Tileable high-contrast dot/stripe texture for rolling illusion */
	const float cx = b.w * 0.5f - 0.5f;
	const float cy = b.h * 0.5f - 0.5f;
	const float rad = b.w * 0.44f;

	for (int y = 0; y < b.h; y++)
	{
		for (int x = 0; x < b.w; x++)
		{
			const float dx = x - cx;
			const float dy = y - cy;
			const float dist = sqrtf(dx * dx + dy * dy);
			const float mask = 1.0f - smoothstepf(rad * 0.75f, rad, dist);
			if (mask <= 0.001f)
				continue;

			/* Repeating 16px grid of soft circular dimples */
			const float gx = fmodf((float)x + 8.0f, 16.0f) - 8.0f;
			const float gy = fmodf((float)y + 8.0f, 16.0f) - 8.0f;
			const float gd = sqrtf(gx * gx + gy * gy);
			const float dot = 1.0f - smoothstepf(2.2f, 4.2f, gd);
			const float a = dot * mask * 0.65f;

			put_px(rgba, b.x + x, b.y + y, 255, 255, 255, clamp_byte(a * 255.0f));
		}
	}
}

static void draw_stick_ring(uint8_t *rgba, sprite_box b)
{
	const float cx = b.w * 0.5f - 0.5f;
	const float cy = b.h * 0.5f - 0.5f;
	const float r_out = b.w * 0.47f;
	const float r_in  = b.w * 0.41f;

	for (int y = 0; y < b.h; y++)
	{
		for (int x = 0; x < b.w; x++)
		{
			const float dx = x - cx;
			const float dy = y - cy;
			const float dist = sqrtf(dx * dx + dy * dy);
			if (dist > r_out + 1.5f)
				continue;

			const float outer_a = 1.0f - smoothstepf(r_out - 1.3f, r_out + 0.5f, dist);
			const float ring_band = smoothstepf(r_in - 1.2f, r_in + 0.5f, dist) * outer_a;
			const float fill_a = (dist < r_out) ? (0.24f * outer_a) : 0.0f;

			/* 4 cardinal direction ticks */
			float tick = 0.0f;
			if (dist > r_out * 0.68f && dist < r_in - 1.5f)
			{
				if (fabsf(dx) < 2.2f || fabsf(dy) < 2.2f)
					tick = 0.55f;
			}

			const float a = fmaxf(ring_band * 0.90f, fmaxf(fill_a, tick));
			const uint8_t lum = (ring_band > 0.35f || tick > 0.1f) ? 245 : 42;
			put_px(rgba, b.x + x, b.y + y, lum, lum, 255, clamp_byte(a * 255.0f));
		}
	}
}

static void draw_stick_nub(uint8_t *rgba, sprite_box b)
{
	const float cx = b.w * 0.5f - 0.5f;
	const float cy = b.h * 0.5f - 0.5f;
	const float rad = b.w * 0.45f;

	for (int y = 0; y < b.h; y++)
	{
		for (int x = 0; x < b.w; x++)
		{
			const float dx = x - cx;
			const float dy = y - cy;
			const float dist = sqrtf(dx * dx + dy * dy);
			const float alpha = 1.0f - smoothstepf(rad - 1.4f, rad + 0.4f, dist);
			if (alpha <= 0.001f)
				continue;

			const float t = dist / rad;
			/* Concave thumb-dish rim */
			const float rim = expf(-powf((t - 0.80f) / 0.12f, 2.0f));
			const float light = (-dx * 0.5f - dy * 0.7f) / (rad + 1.0f);
			const float v = 155.0f + 55.0f * rim + 42.0f * light;
			put_px(rgba, b.x + x, b.y + y,
				clamp_byte(v), clamp_byte(v), clamp_byte(v + 8.0f),
				clamp_byte(alpha * 255.0f));
		}
	}
}

static void draw_dpad(uint8_t *rgba, sprite_box b)
{
	const float cx = b.w * 0.5f - 0.5f;
	const float cy = b.h * 0.5f - 0.5f;
	const float arm_half = b.w * 0.165f;
	const float arm_len  = b.w * 0.465f;
	const float rad_c    = 5.0f;

	for (int y = 0; y < b.h; y++)
	{
		for (int x = 0; x < b.w; x++)
		{
			const float ax = fabsf(x - cx);
			const float ay = fabsf(y - cy);

			/* Signed distance to union of two rounded rectangles (horizontal + vertical arms) */
			const float qhx = fmaxf(ax - (arm_len - rad_c), 0.0f);
			const float qhy = fmaxf(ay - (arm_half - rad_c), 0.0f);
			const float dh = sqrtf(qhx * qhx + qhy * qhy) + fminf(fmaxf(ax - (arm_len - rad_c), ay - (arm_half - rad_c)), 0.0f) - rad_c;

			const float qvx = fmaxf(ax - (arm_half - rad_c), 0.0f);
			const float qvy = fmaxf(ay - (arm_len - rad_c), 0.0f);
			const float dv = sqrtf(qvx * qvx + qvy * qvy) + fminf(fmaxf(ax - (arm_half - rad_c), ay - (arm_len - rad_c)), 0.0f) - rad_c;

			const float sd = fminf(dh, dv);
			const float alpha = 1.0f - smoothstepf(-0.8f, 0.9f, sd);
			if (alpha <= 0.001f)
				continue;

			const float bevel = smoothstepf(-5.5f, -1.0f, sd);
			const float top_light = (cy - y) / b.h;
			float lum = 76.0f + 58.0f * top_light + 68.0f * bevel;

			/* Crisp directional chevron triangles on each arm */
			bool arrow = false;
			if (ay > b.w * 0.24f && ay < b.w * 0.40f && ax < (b.w * 0.41f - ay) * 0.85f)
				arrow = true;
			if (ax > b.w * 0.24f && ax < b.w * 0.40f && ay < (b.w * 0.41f - ax) * 0.85f)
				arrow = true;
			/* Center pivot dimple */
			const float cdist = sqrtf(ax * ax + ay * ay);
			if (cdist < b.w * 0.09f)
				lum -= 18.0f * (1.0f - cdist / (b.w * 0.09f));

			if (arrow)
				lum = 238.0f;

			put_px(rgba, b.x + x, b.y + y,
				clamp_byte(lum * 0.95f), clamp_byte(lum * 0.98f), clamp_byte(lum * 1.05f),
				clamp_byte(alpha * 255.0f));
		}
	}
}

static void draw_btn_face(uint8_t *rgba, sprite_box b)
{
	const float cx = b.w * 0.5f - 0.5f;
	const float cy = b.h * 0.5f - 0.5f;
	const float rad = b.w * 0.46f;

	for (int y = 0; y < b.h; y++)
	{
		for (int x = 0; x < b.w; x++)
		{
			const float dx = x - cx;
			const float dy = y - cy;
			const float dist = sqrtf(dx * dx + dy * dy);
			const float alpha = 1.0f - smoothstepf(rad - 1.4f, rad + 0.4f, dist);
			if (alpha <= 0.001f)
				continue;

			const float nx = dx / rad;
			const float ny = dy / rad;
			const float nz = sqrtf(fmaxf(0.0f, 1.0f - nx * nx - ny * ny));
			/* Arcade Sanwa convex plunger shading */
			const float dome = 0.58f + 0.36f * nz + 0.18f * (-0.35f * nx - 0.65f * ny);
			const float spec = powf(fmaxf(0.0f, -0.25f * nx - 0.45f * ny + 0.85f * nz), 18.0f) * 0.38f;
			const uint8_t v = clamp_byte((dome + spec) * 255.0f);

			put_px(rgba, b.x + x, b.y + y, v, v, v, clamp_byte(alpha * 255.0f));
		}
	}
}

static void draw_btn_ring(uint8_t *rgba, sprite_box b)
{
	const float cx = b.w * 0.5f - 0.5f;
	const float cy = b.h * 0.5f - 0.5f;
	const float r_out = b.w * 0.48f;
	const float r_in  = b.w * 0.415f;

	for (int y = 0; y < b.h; y++)
	{
		for (int x = 0; x < b.w; x++)
		{
			const float dx = x - cx;
			const float dy = y - cy;
			const float dist = sqrtf(dx * dx + dy * dy);
			const float a = (1.0f - smoothstepf(r_out - 1.3f, r_out + 0.5f, dist)) *
			                smoothstepf(r_in - 1.3f, r_in + 0.4f, dist);
			if (a <= 0.001f)
				continue;
			put_px(rgba, b.x + x, b.y + y, 255, 255, 255, clamp_byte(a * 255.0f));
		}
	}
}

static void draw_pill(uint8_t *rgba, sprite_box b)
{
	const float cx = b.w * 0.5f - 0.5f;
	const float cy = b.h * 0.5f - 0.5f;
	const float half_h = b.h * 0.42f;
	const float half_w = b.w * 0.45f - half_h;

	for (int y = 0; y < b.h; y++)
	{
		for (int x = 0; x < b.w; x++)
		{
			const float qx = fmaxf(fabsf(x - cx) - half_w, 0.0f);
			const float qy = fabsf(y - cy);
			const float dist = sqrtf(qx * qx + qy * qy);
			const float alpha = 1.0f - smoothstepf(half_h - 1.4f, half_h + 0.4f, dist);
			if (alpha <= 0.001f)
				continue;

			const float border = smoothstepf(half_h - 4.2f, half_h - 2.0f, dist);
			const float grad = 0.72f + 0.22f * ((cy - y) / half_h);
			const uint8_t v = clamp_byte((grad * (1.0f - border * 0.25f) + border * 0.35f) * 255.0f);
			put_px(rgba, b.x + x, b.y + y, v, v, v, clamp_byte(alpha * 255.0f));
		}
	}
}

static void draw_select_box(uint8_t *rgba, sprite_box b)
{
	const int m = 4;
	const int corner = 18;

	for (int y = m; y < b.h - m; y++)
	{
		for (int x = m; x < b.w - m; x++)
		{
			const int dx = (x - m < b.w - 1 - m - x) ? (x - m) : (b.w - 1 - m - x);
			const int dy = (y - m < b.h - 1 - m - y) ? (y - m) : (b.h - 1 - m - y);
			const bool on_edge = (dx < 3 || dy < 3);
			const bool in_corner = (dx < corner && dy < corner && (dx < 5 || dy < 5));
			const bool dashed = (((x + y) / 6) & 1) == 0;

			if (in_corner)
				put_px(rgba, b.x + x, b.y + y, 255, 255, 255, 255);
			else if (on_edge && dashed)
				put_px(rgba, b.x + x, b.y + y, 255, 255, 255, 200);
		}
	}
}

static void draw_icon_gear(uint8_t *rgba, sprite_box b)
{
	const float cx = b.w * 0.5f - 0.5f;
	const float cy = b.h * 0.5f - 0.5f;

	for (int y = 0; y < b.h; y++)
	{
		for (int x = 0; x < b.w; x++)
		{
			const float dx = x - cx;
			const float dy = y - cy;
			const float dist = sqrtf(dx * dx + dy * dy);
			const float ang = atan2f(dy, dx);
			const float tooth = cosf(ang * 8.0f);
			const float outer_r = b.w * 0.34f + (tooth > 0.2f ? b.w * 0.085f * smoothstepf(0.2f, 0.55f, tooth) : 0.0f);
			const float inner_r = b.w * 0.145f;

			const float a = (1.0f - smoothstepf(outer_r - 1.2f, outer_r + 0.5f, dist)) *
			                smoothstepf(inner_r - 1.0f, inner_r + 0.5f, dist);
			if (a > 0.001f)
				put_px(rgba, b.x + x, b.y + y, 255, 255, 255, clamp_byte(a * 255.0f));
		}
	}
}

static void draw_icon_bolt(uint8_t *rgba, sprite_box b)
{
	/* Crisp lightning bolt polygon inside 64x64 cell */
	for (int y = 0; y < b.h; y++)
	{
		const float ny = (float)y / (float)b.h;
		for (int x = 0; x < b.w; x++)
		{
			const float nx = (float)x / (float)b.w;
			bool inside = false;

			/* Upper slash: top-right (0.58,0.10) down to mid-left (0.26,0.54) */
			if (ny >= 0.10f && ny <= 0.54f)
			{
				const float t = (ny - 0.10f) / 0.44f;
				const float left  = 0.50f - 0.24f * t;
				const float right = 0.66f - 0.14f * t;
				if (nx >= left && nx <= right)
					inside = true;
			}
			/* Lower slash: mid-right (0.74,0.44) down to tip (0.38,0.90) */
			if (ny >= 0.44f && ny <= 0.90f)
			{
				const float t = (ny - 0.44f) / 0.46f;
				const float left  = 0.46f - 0.08f * t;
				const float right = 0.72f - 0.34f * t;
				if (nx >= left && nx <= right)
					inside = true;
			}

			if (inside)
				put_px(rgba, b.x + x, b.y + y, 255, 255, 255, 255);
		}
	}
}

static void draw_font_sheet(uint8_t *rgba)
{
	for (int idx = 0; idx < 95; idx++)
	{
		const int col = idx % FONT_COLS;
		const int row = idx / FONT_COLS;
		const int ox = FONT_ORIGIN_X + col * FONT_CELL_W + 4; /* 16x16 centered in 24x24 */
		const int oy = FONT_ORIGIN_Y + row * FONT_CELL_H + 4;
		const uint8_t *glyph = k_font8x8[idx];
		uint8_t mask[20][20];

		memset(mask, 0, sizeof(mask));

		/* Upscale 8x8 -> 16x16 foreground mask (at +2,+2 inside 20x20 buffer) */
		for (int gy = 0; gy < 8; gy++)
		{
			const uint8_t bits = glyph[gy];
			for (int gx = 0; gx < 8; gx++)
			{
				if (bits & (0x80u >> gx))
				{
					mask[2 + gy * 2 + 0][2 + gx * 2 + 0] = 2;
					mask[2 + gy * 2 + 0][2 + gx * 2 + 1] = 2;
					mask[2 + gy * 2 + 1][2 + gx * 2 + 0] = 2;
					mask[2 + gy * 2 + 1][2 + gx * 2 + 1] = 2;
				}
			}
		}

		/* Dilate by 1px for crisp dark outline / drop shadow */
		for (int py = 1; py < 19; py++)
		{
			for (int px = 1; px < 19; px++)
			{
				if (mask[py][px] == 2)
					continue;
				for (int dy = -1; dy <= 1; dy++)
				{
					for (int dx = -1; dx <= 1; dx++)
					{
						if (mask[py + dy][px + dx] == 2)
							mask[py][px] = 1;
					}
				}
			}
		}

		for (int py = 0; py < 20; py++)
		{
			for (int px = 0; px < 20; px++)
			{
				const int ax = ox - 2 + px;
				const int ay = oy - 2 + py;
				if (mask[py][px] == 2)
					put_px(rgba, ax, ay, 255, 255, 255, 255);
				else if (mask[py][px] == 1)
					put_px(rgba, ax, ay, 14, 18, 28, 220);
			}
		}
	}
}

void ui_atlas_generate_rgba8(uint8_t *out_rgba)
{
	if (!out_rgba)
		return;

	memset(out_rgba, 0, (size_t)UI_ATLAS_WIDTH * (size_t)UI_ATLAS_HEIGHT * 4u);

	/* Solid 16x16 pure white block at (256, 256) */
	for (int y = 256; y < 272; y++)
		for (int x = 256; x < 272; x++)
			put_px(out_rgba, x, y, 255, 255, 255, 255);

	draw_trackball_base(out_rgba, k_sprite_boxes[UI_SPRITE_TRACKBALL_BASE]);
	draw_trackball_ball(out_rgba, k_sprite_boxes[UI_SPRITE_TRACKBALL_BALL]);
	draw_trackball_dots(out_rgba, k_sprite_boxes[UI_SPRITE_TRACKBALL_DOTS]);
	draw_stick_ring(out_rgba, k_sprite_boxes[UI_SPRITE_STICK_RING]);
	draw_stick_nub(out_rgba, k_sprite_boxes[UI_SPRITE_STICK_NUB]);
	draw_dpad(out_rgba, k_sprite_boxes[UI_SPRITE_DPAD]);
	draw_btn_face(out_rgba, k_sprite_boxes[UI_SPRITE_BTN_FACE]);
	draw_btn_ring(out_rgba, k_sprite_boxes[UI_SPRITE_BTN_RING]);
	draw_pill(out_rgba, k_sprite_boxes[UI_SPRITE_PILL]);
	draw_select_box(out_rgba, k_sprite_boxes[UI_SPRITE_SELECT_BOX]);
	draw_icon_gear(out_rgba, k_sprite_boxes[UI_SPRITE_ICON_GEAR]);
	draw_icon_bolt(out_rgba, k_sprite_boxes[UI_SPRITE_ICON_BOLT]);
	draw_font_sheet(out_rgba);
}

ui_uv_rect ui_atlas_get_sprite_uv(ui_sprite_id id)
{
	ui_uv_rect uv = { 0.0f, 0.0f, 0.0f, 0.0f };
	if ((unsigned)id >= UI_SPRITE_COUNT)
		id = UI_SPRITE_WHITE;

	{
		const sprite_box b = k_sprite_boxes[id];
		uv.u0 = (float)b.x / (float)UI_ATLAS_WIDTH;
		uv.v0 = (float)b.y / (float)UI_ATLAS_HEIGHT;
		uv.u1 = (float)(b.x + b.w) / (float)UI_ATLAS_WIDTH;
		uv.v1 = (float)(b.y + b.h) / (float)UI_ATLAS_HEIGHT;
	}
	return uv;
}

ui_uv_rect ui_atlas_get_glyph_uv(char ch)
{
	int idx = (unsigned char)ch - 32;
	ui_uv_rect uv;

	if (idx < 0 || idx >= 95)
		idx = '?' - 32;

	{
		const int col = idx % FONT_COLS;
		const int row = idx / FONT_COLS;
		/* Include 1px outline around 16x16 glyph -> 18x18 inside 24x24 cell */
		const float x0 = (float)(FONT_ORIGIN_X + col * FONT_CELL_W + 3);
		const float y0 = (float)(FONT_ORIGIN_Y + row * FONT_CELL_H + 3);

		uv.u0 = x0 / (float)UI_ATLAS_WIDTH;
		uv.v0 = y0 / (float)UI_ATLAS_HEIGHT;
		uv.u1 = (x0 + 18.0f) / (float)UI_ATLAS_WIDTH;
		uv.v1 = (y0 + 18.0f) / (float)UI_ATLAS_HEIGHT;
	}
	return uv;
}
