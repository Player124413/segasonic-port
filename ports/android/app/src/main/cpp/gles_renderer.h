/*
 * SegaSonic The Hedgehog — Android Port
 * OpenGL ES 3.0 (ETC2/EAC + Async PBO) & OpenGL ES 2.0 Fallback Renderer
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef SEGASONIC_ANDROID_GLES_RENDERER_H
#define SEGASONIC_ANDROID_GLES_RENDERER_H

#include <stdbool.h>
#include <stdint.h>

#include "touch_controls.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef enum gles_filter_mode
{
	GLES_FILTER_SHARP_BILINEAR = 0, /* Subpixel-AA sharp pixels (zero shimmer!) */
	GLES_FILTER_NEAREST        = 1, /* Raw integer/nearest pixels */
	GLES_FILTER_SMOOTH         = 2, /* Soft bilinear arcade monitor */
	GLES_FILTER_CRT            = 3, /* Single-pass 15kHz Nanao CRT scanlines + grille */
	GLES_FILTER_COUNT          = 4
} gles_filter_mode;

typedef enum gles_aspect_mode
{
	GLES_ASPECT_AUTO = 0, /* 16:9 (398:224) in Console widescreen, 4:3 in Arcade */
	GLES_ASPECT_4_3  = 1, /* Classic 4:3 CRT pillarboxed */
	GLES_ASPECT_FILL = 2, /* Stretch to fill full phone display */
	GLES_ASPECT_COUNT = 3
} gles_aspect_mode;

typedef enum gles_etc2_mode
{
	GLES_ETC2_AUTO_ATLAS     = 0, /* GLES3: ETC2+EAC UI Atlas + PBO RGB565 lossless game stream */
	GLES_ETC2_FULL_STREAM    = 1, /* GLES3: ETC2+EAC UI Atlas + real-time dirty-strip RGB8_ETC2 stream */
	GLES_ETC2_UNCOMPRESSED   = 2, /* Force uncompressed textures (diagnostic / GLES2 mode) */
	GLES_ETC2_MODE_COUNT     = 3
} gles_etc2_mode;

typedef struct gles_renderer_info
{
	bool is_gles3;
	bool has_etc2_hw;
	bool has_pbo_dma;
	bool ui_atlas_compressed;
	bool frame_etc2_active;
	char summary[48]; /* e.g. "GLES3 ETC2+PBO" or "GLES3 ETC2 STREAM" or "GLES2 RGB565" */
} gles_renderer_info;

typedef struct gles_renderer gles_renderer;

/*
 * Create and initialize the OpenGL ES renderer on the current EGL/SDL_GL context.
 * `cache_dir` is used to load/save `.etc2` compressed texture caches (`NULL` disables disk cache).
 * Automatically detects OpenGL ES 3.0+ vs OpenGL ES 2.0 and configures hardware ETC2/EAC & PBOs.
 */
gles_renderer *gles_renderer_create(const char *cache_dir, gles_etc2_mode etc2_mode);

void gles_renderer_destroy(gles_renderer *r);

void gles_renderer_set_filter(gles_renderer *r, gles_filter_mode filter);
gles_filter_mode gles_renderer_get_filter(const gles_renderer *r);
const char *gles_renderer_filter_name(gles_filter_mode filter);

void gles_renderer_set_aspect(gles_renderer *r, gles_aspect_mode aspect);
gles_aspect_mode gles_renderer_get_aspect(const gles_renderer *r);
const char *gles_renderer_aspect_name(gles_aspect_mode aspect, bool wide_enabled);

void gles_renderer_set_etc2_mode(gles_renderer *r, gles_etc2_mode mode);
gles_etc2_mode gles_renderer_get_etc2_mode(const gles_renderer *r);

void gles_renderer_get_info(const gles_renderer *r, gles_renderer_info *out_info);

/*
 * Render one complete frame (System 32 416x224 XRGB32 framebuffer cropped to `visible_width`
 * + single-draw-call batched Touch HUD / Quick Menu / Editor overlay).
 */
void gles_renderer_draw_frame(gles_renderer *r,
	const uint32_t *xrgb_frame, int frame_width, int visible_width, int frame_height,
	int viewport_w, int viewport_h,
	const ui_quad_batch *ui_batch);

#ifdef __cplusplus
}
#endif

#endif /* SEGASONIC_ANDROID_GLES_RENDERER_H */
