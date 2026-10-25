/*
 * SegaSonic The Hedgehog — Android Port
 * OpenGL ES 3.0 (ETC2/EAC + Async PBO) & OpenGL ES 2.0 Fallback Renderer
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "gles_renderer.h"
#include "etc2.h"
#include "ui_atlas.h"

#include <SDL.h>
#include <SDL_opengles2.h>

#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#if defined(__ARM_NEON) || defined(__ARM_NEON__)
#include <arm_neon.h>
#define GLES_HAS_NEON 1
#else
#define GLES_HAS_NEON 0
#endif

/* OpenGL ES 3.0 constants & PBO function pointers (loaded dynamically for ES 2.0 binary safety) */
#ifndef GL_PIXEL_UNPACK_BUFFER
#define GL_PIXEL_UNPACK_BUFFER 0x88EC
#endif
#ifndef GL_MAP_WRITE_BIT
#define GL_MAP_WRITE_BIT 0x0002
#endif
#ifndef GL_MAP_INVALIDATE_BUFFER_BIT
#define GL_MAP_INVALIDATE_BUFFER_BIT 0x0008
#endif

typedef void *(GL_APIENTRY *pfn_glMapBufferRange)(GLenum target, GLintptr offset, GLsizeiptr length, GLbitfield access);
typedef GLboolean (GL_APIENTRY *pfn_glUnmapBuffer)(GLenum target);

#define MAX_GAME_W 416
#define MAX_GAME_H 224

struct gles_renderer
{
	bool is_gles3;
	bool has_etc2_hw;
	bool has_pbo_dma;
	bool ui_atlas_compressed;
	bool frame_tex_is_etc2;

	pfn_glMapBufferRange fn_map_buffer_range;
	pfn_glUnmapBuffer fn_unmap_buffer;

	gles_filter_mode filter;
	gles_aspect_mode aspect;
	gles_etc2_mode etc2_mode;
	char cache_dir[512];

	/* Shader programs: [0] = nearest/bilinear, [1] = analytical sharp-bilinear, [2] = CRT */
	GLuint prog_basic;
	GLuint prog_sharp;
	GLuint prog_crt;
	GLuint prog_ui;

	/* Cached uniform & attribute locations */
	GLint u_sharp_inv_tex;
	GLint u_sharp_scale;
	GLint u_crt_inv_tex;
	GLint u_crt_scale;
	GLint u_ui_vp;

	/* Textures & buffers */
	GLuint frame_tex;
	int frame_tex_w;
	int frame_tex_h;
	GLuint ui_tex;

	GLuint quad_vbo;
	GLuint ui_vbo;
	GLuint ui_ibo;
	GLuint pbos[2];
	int pbo_idx;

	/* Streaming buffers & dirty-tracking state */
	uint16_t *rgb565_staging;
	uint64_t row_hashes[MAX_GAME_H];

	uint8_t *etc2_frame_blocks;
	uint64_t *etc2_block_hashes;
};

/* ---- GLSL ES 1.00 Shaders (100% compatible across OpenGL ES 2.0 and ES 3.x) ---- */

static const char *const k_game_vs =
	"attribute vec2 a_pos;\n"
	"attribute vec2 a_tex;\n"
	"varying vec2 v_tex;\n"
	"void main() {\n"
	"    v_tex = a_tex;\n"
	"    gl_Position = vec4(a_pos, 0.0, 1.0);\n"
	"}\n";

/* Basic Nearest / Hardware Bilinear (`v_tex` normalized [0..1]) */
static const char *const k_basic_fs =
	"precision mediump float;\n"
	"varying vec2 v_tex;\n"
	"uniform sampler2D u_tex;\n"
	"void main() {\n"
	"    gl_FragColor = vec4(texture2D(u_tex, v_tex).rgb, 1.0);\n"
	"}\n";

/* Analytical Subpixel-AA Sharp-Bilinear (`v_tex` in texel coordinates [0..W, 0..H]) */
static const char *const k_sharp_fs =
	"precision highp float;\n"
	"varying vec2 v_tex;\n"
	"uniform sampler2D u_tex;\n"
	"uniform vec2 u_inv_tex;\n"
	"uniform vec2 u_scale;\n"
	"void main() {\n"
	"    vec2 fl = floor(v_tex);\n"
	"    vec2 fr = fract(v_tex);\n"
	"    vec2 sc = max(u_scale, vec2(1.0));\n"
	"    vec2 reg = 0.5 - 0.5 / sc;\n"
	"    vec2 cd = fr - 0.5;\n"
	"    vec2 f = (cd - clamp(cd, -reg, reg)) * sc + 0.5;\n"
	"    vec2 uv = (fl + f) * u_inv_tex;\n"
	"    gl_FragColor = vec4(texture2D(u_tex, uv).rgb, 1.0);\n"
	"}\n";

/* Single-Pass Sega System 32 15kHz Nanao CRT Scanline + Aperture Grille Shader */
static const char *const k_crt_fs =
	"precision highp float;\n"
	"varying vec2 v_tex;\n"
	"uniform sampler2D u_tex;\n"
	"uniform vec2 u_inv_tex;\n"
	"uniform vec2 u_scale;\n"
	"void main() {\n"
	"    vec2 fl = floor(v_tex);\n"
	"    vec2 fr = fract(v_tex);\n"
	"    vec2 sc = max(u_scale, vec2(1.0));\n"
	"    vec2 reg = 0.5 - 0.5 / sc;\n"
	"    vec2 cd = fr - 0.5;\n"
	"    vec2 f = (cd - clamp(cd, -reg, reg)) * sc + 0.5;\n"
	"    vec3 col = texture2D(u_tex, (fl + f) * u_inv_tex).rgb;\n"
	"    float beam = sin(fr.y * 3.14159265);\n"
	"    float lum = dot(col, vec3(0.299, 0.587, 0.114));\n"
	"    float scan = mix(0.66 + 0.22 * lum, 1.06, beam * beam);\n"
	"    float tr = mod(floor(gl_FragCoord.x), 3.0);\n"
	"    vec3 mask = (tr < 0.5) ? vec3(1.06, 0.93, 0.93)\n"
	"              : ((tr < 1.5) ? vec3(0.93, 1.06, 0.93) : vec3(0.93, 0.93, 1.06));\n"
	"    gl_FragColor = vec4(clamp(col * scan * mask * 1.07, 0.0, 1.0), 1.0);\n"
	"}\n";

/* Batched 2D UI / Touch Controls / Font Overlay Shader */
static const char *const k_ui_vs =
	"attribute vec2 a_pos;\n"
	"attribute vec2 a_uv;\n"
	"attribute vec4 a_col;\n"
	"uniform vec2 u_vp;\n"
	"varying vec2 v_uv;\n"
	"varying vec4 v_col;\n"
	"void main() {\n"
	"    vec2 ndc = vec2((a_pos.x / u_vp.x) * 2.0 - 1.0, 1.0 - (a_pos.y / u_vp.y) * 2.0);\n"
	"    v_uv = a_uv;\n"
	"    v_col = a_col;\n"
	"    gl_Position = vec4(ndc, 0.0, 1.0);\n"
	"}\n";

static const char *const k_ui_fs =
	"precision mediump float;\n"
	"varying vec2 v_uv;\n"
	"varying vec4 v_col;\n"
	"uniform sampler2D u_tex;\n"
	"void main() {\n"
	"    gl_FragColor = texture2D(u_tex, v_uv) * v_col;\n"
	"}\n";

/* ---- Shader Compilation Helpers ---- */

static GLuint compile_shader(GLenum type, const char *src)
{
	GLuint sh = glCreateShader(type);
	GLint ok = 0;

	if (!sh)
		return 0;
	glShaderSource(sh, 1, &src, NULL);
	glCompileShader(sh);
	glGetShaderiv(sh, GL_COMPILE_STATUS, &ok);
	if (!ok)
	{
		char log[512];
		glGetShaderInfoLog(sh, sizeof(log), NULL, log);
		SDL_Log("GLES shader compile error: %s", log);
		glDeleteShader(sh);
		return 0;
	}
	return sh;
}

static GLuint link_program(const char *vs_src, const char *fs_src, bool is_ui)
{
	GLuint vs = compile_shader(GL_VERTEX_SHADER, vs_src);
	GLuint fs = compile_shader(GL_FRAGMENT_SHADER, fs_src);
	GLuint prog;
	GLint ok = 0;

	if (!vs || !fs)
	{
		if (vs) glDeleteShader(vs);
		if (fs) glDeleteShader(fs);
		return 0;
	}

	prog = glCreateProgram();
	glAttachShader(prog, vs);
	glAttachShader(prog, fs);
	if (is_ui)
	{
		glBindAttribLocation(prog, 0, "a_pos");
		glBindAttribLocation(prog, 1, "a_uv");
		glBindAttribLocation(prog, 2, "a_col");
	}
	else
	{
		glBindAttribLocation(prog, 0, "a_pos");
		glBindAttribLocation(prog, 1, "a_tex");
	}
	glLinkProgram(prog);
	glDeleteShader(vs);
	glDeleteShader(fs);

	glGetProgramiv(prog, GL_LINK_STATUS, &ok);
	if (!ok)
	{
		char log[512];
		glGetProgramInfoLog(prog, sizeof(log), NULL, log);
		SDL_Log("GLES program link error: %s", log);
		glDeleteProgram(prog);
		return 0;
	}

	glUseProgram(prog);
	{
		GLint tex_loc = glGetUniformLocation(prog, "u_tex");
		if (tex_loc >= 0)
			glUniform1i(tex_loc, 0);
	}
	glUseProgram(0);
	return prog;
}

/* ---- SIMD / Fast Row Conversion (0x00RRGGBB -> Lossless RGB565) ---- */

static inline uint64_t hash_row_xrgb(const uint32_t *row, int width)
{
	uint64_t h = 0xcbf29ce484222325ULL;
	int x = 0;

	for (; x + 1 < width; x += 2)
	{
		uint64_t pair = ((uint64_t)(row[x] & 0x00ffffffu) << 32) | (row[x + 1] & 0x00ffffffu);
		h = (h ^ pair) * 0x100000001b3ULL;
	}
	if (x < width)
		h = (h ^ (row[x] & 0x00ffffffu)) * 0x100000001b3ULL;
	return (h == ~0ULL) ? ~1ULL : h;
}

static void convert_xrgb32_rows_to_rgb565(const uint32_t *src, int src_stride_px,
	uint16_t *dst, int width, int y0, int y1)
{
	for (int y = y0; y < y1; y++)
	{
		const uint32_t *s = src + (size_t)y * (size_t)src_stride_px;
		uint16_t *d = dst + (size_t)(y - y0) * (size_t)width;
		int x = 0;

#if GLES_HAS_NEON && (__BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__)
		/* Deinterleave 8 little-endian 0x00RRGGBB pixels (B, G, R, X bytes) per iteration */
		for (; x + 8 <= width; x += 8)
		{
			const uint8x8x4_t bgrx = vld4_u8((const uint8_t *)(s + x));
			const uint16x8_t r5 = vshll_n_u8(vshr_n_u8(bgrx.val[2], 3), 11);
			const uint16x8_t g6 = vshll_n_u8(vshr_n_u8(bgrx.val[1], 2), 5);
			const uint16x8_t b5 = vmovl_u8(vshr_n_u8(bgrx.val[0], 3));
			vst1q_u16(d + x, vorrq_u16(vorrq_u16(r5, g6), b5));
		}
#endif
		for (; x < width; x++)
		{
			const uint32_t c = s[x];
			d[x] = (uint16_t)(((c >> 8) & 0xf800u) | ((c >> 5) & 0x07e0u) | ((c >> 3) & 0x001fu));
		}
	}
}

/* ---- Texture Creation & ETC2 Upload with Automatic Driver Validation ---- */

static void upload_ui_atlas(gles_renderer *r)
{
	const size_t raw_size = (size_t)UI_ATLAS_WIDTH * (size_t)UI_ATLAS_HEIGHT * 4u;
	uint8_t *rgba = (uint8_t *)malloc(raw_size);

	if (!rgba)
		return;
	ui_atlas_generate_rgba8(rgba);

	glGenTextures(1, &r->ui_tex);
	glBindTexture(GL_TEXTURE_2D, r->ui_tex);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	r->ui_atlas_compressed = false;

	if (r->has_etc2_hw && r->etc2_mode != GLES_ETC2_UNCOMPRESSED)
	{
		const uint32_t src_crc = etc2_crc32(rgba, raw_size) ^ UI_ATLAS_VERSION;
		const size_t etc_size = etc2_rgba8_data_size(UI_ATLAS_WIDTH, UI_ATLAS_HEIGHT);
		char cache_file[640];
		uint8_t *etc_data = NULL;
		size_t loaded_sz = 0;

		if (r->cache_dir[0])
		{
			snprintf(cache_file, sizeof(cache_file), "%s/ui_atlas_v%u.etc2", r->cache_dir, UI_ATLAS_VERSION);
			etc_data = etc2_cache_load(cache_file, ETC2_GL_COMPRESSED_RGBA8_ETC2_EAC,
				UI_ATLAS_WIDTH, UI_ATLAS_HEIGHT, src_crc, &loaded_sz);
		}

		if (!etc_data)
		{
			etc_data = (uint8_t *)malloc(etc_size);
			if (etc_data)
			{
				etc2_compress_rgba8(rgba, UI_ATLAS_WIDTH, UI_ATLAS_HEIGHT,
					UI_ATLAS_WIDTH * 4, etc_data, ETC2_QUALITY_BALANCED);
				if (r->cache_dir[0])
				{
					etc2_cache_save(cache_file, ETC2_GL_COMPRESSED_RGBA8_ETC2_EAC,
						UI_ATLAS_WIDTH, UI_ATLAS_HEIGHT, src_crc, etc_data, etc_size);
				}
			}
		}

		if (etc_data)
		{
			while (glGetError() != GL_NO_ERROR) {}
			glCompressedTexImage2D(GL_TEXTURE_2D, 0, ETC2_GL_COMPRESSED_RGBA8_ETC2_EAC,
				UI_ATLAS_WIDTH, UI_ATLAS_HEIGHT, 0, (GLsizei)etc_size, etc_data);
			if (glGetError() == GL_NO_ERROR)
				r->ui_atlas_compressed = true;
			else
				SDL_Log("GLES driver rejected RGBA8_ETC2_EAC; falling back to uncompressed RGBA8");
			free(etc_data);
		}
	}

	if (!r->ui_atlas_compressed)
	{
		glPixelStorei(GL_UNPACK_ALIGNMENT, 4);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, UI_ATLAS_WIDTH, UI_ATLAS_HEIGHT,
			0, GL_RGBA, GL_UNSIGNED_BYTE, rgba);
	}

	free(rgba);
}

static void recreate_frame_texture(gles_renderer *r, int tex_w, int tex_h, bool want_etc2)
{
	if (r->frame_tex)
		glDeleteTextures(1, &r->frame_tex);
	glGenTextures(1, &r->frame_tex);
	glBindTexture(GL_TEXTURE_2D, r->frame_tex);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	r->frame_tex_w = tex_w;
	r->frame_tex_h = tex_h;
	r->frame_tex_is_etc2 = false;

	if (want_etc2 && r->has_etc2_hw)
	{
		const size_t sz = etc2_rgb8_data_size(tex_w, tex_h);
		memset(r->etc2_frame_blocks, 0, sz);
		memset(r->etc2_block_hashes, 0xff,
			(size_t)etc2_blocks_dim(tex_w) * (size_t)etc2_blocks_dim(tex_h) * sizeof(uint64_t));

		while (glGetError() != GL_NO_ERROR) {}
		glCompressedTexImage2D(GL_TEXTURE_2D, 0, ETC2_GL_COMPRESSED_RGB8_ETC2,
			tex_w, tex_h, 0, (GLsizei)sz, r->etc2_frame_blocks);
		if (glGetError() == GL_NO_ERROR)
			r->frame_tex_is_etc2 = true;
	}

	if (!r->frame_tex_is_etc2)
	{
		memset(r->rgb565_staging, 0, (size_t)tex_w * (size_t)tex_h * sizeof(uint16_t));
		memset(r->row_hashes, 0xff, sizeof(r->row_hashes));
		glPixelStorei(GL_UNPACK_ALIGNMENT, 2);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, tex_w, tex_h, 0,
			GL_RGB, GL_UNSIGNED_SHORT_5_6_5, r->rgb565_staging);
	}
}

/* ---- Public Lifecycle & Settings API ---- */

gles_renderer *gles_renderer_create(const char *cache_dir, gles_etc2_mode etc2_mode)
{
	gles_renderer *r = (gles_renderer *)calloc(1, sizeof(gles_renderer));
	const char *ver_str;

	if (!r)
		return NULL;

	if (cache_dir)
		snprintf(r->cache_dir, sizeof(r->cache_dir), "%s", cache_dir);

	r->filter = GLES_FILTER_SHARP_BILINEAR;
	r->aspect = GLES_ASPECT_AUTO;
	r->etc2_mode = etc2_mode;

	ver_str = (const char *)glGetString(GL_VERSION);
	if (ver_str && (strstr(ver_str, "OpenGL ES 3.") || strstr(ver_str, "OpenGL ES-CM 3.")))
	{
		r->is_gles3 = true;
		r->has_etc2_hw = true;
		r->fn_map_buffer_range = (pfn_glMapBufferRange)SDL_GL_GetProcAddress("glMapBufferRange");
		r->fn_unmap_buffer = (pfn_glUnmapBuffer)SDL_GL_GetProcAddress("glUnmapBuffer");
		r->has_pbo_dma = (r->fn_map_buffer_range != NULL && r->fn_unmap_buffer != NULL);
	}
	SDL_Log("SegaSonic GLES Renderer: version='%s', gles3=%d, etc2=%d, pbo=%d",
		ver_str ? ver_str : "unknown", r->is_gles3, r->has_etc2_hw, r->has_pbo_dma);

	r->prog_basic = link_program(k_game_vs, k_basic_fs, false);
	r->prog_sharp = link_program(k_game_vs, k_sharp_fs, false);
	r->prog_crt   = link_program(k_game_vs, k_crt_fs, false);
	r->prog_ui    = link_program(k_ui_vs,   k_ui_fs,    true);

	if (!r->prog_basic || !r->prog_ui)
	{
		gles_renderer_destroy(r);
		return NULL;
	}

	if (r->prog_sharp)
	{
		r->u_sharp_inv_tex = glGetUniformLocation(r->prog_sharp, "u_inv_tex");
		r->u_sharp_scale   = glGetUniformLocation(r->prog_sharp, "u_scale");
	}
	if (r->prog_crt)
	{
		r->u_crt_inv_tex = glGetUniformLocation(r->prog_crt, "u_inv_tex");
		r->u_crt_scale   = glGetUniformLocation(r->prog_crt, "u_scale");
	}
	r->u_ui_vp = glGetUniformLocation(r->prog_ui, "u_vp");

	/* Allocate staging & ETC2 streaming buffers for max 416x224 System 32 frame */
	r->rgb565_staging = (uint16_t *)calloc(MAX_GAME_W * MAX_GAME_H, sizeof(uint16_t));
	r->etc2_frame_blocks = (uint8_t *)calloc(etc2_rgb8_data_size(MAX_GAME_W, MAX_GAME_H), 1);
	r->etc2_block_hashes = (uint64_t *)malloc(
		(size_t)etc2_blocks_dim(MAX_GAME_W) * (size_t)etc2_blocks_dim(MAX_GAME_H) * sizeof(uint64_t));

	if (!r->rgb565_staging || !r->etc2_frame_blocks || !r->etc2_block_hashes)
	{
		gles_renderer_destroy(r);
		return NULL;
	}

	glGenBuffers(1, &r->quad_vbo);
	glGenBuffers(1, &r->ui_vbo);
	glGenBuffers(1, &r->ui_ibo);

	/* Pre-populate static UI quad index buffer (0,1,2, 0,2,3 ...) */
	{
		uint16_t *indices = (uint16_t *)malloc(UI_MAX_QUADS * 6 * sizeof(uint16_t));
		if (indices)
		{
			for (int q = 0; q < UI_MAX_QUADS; q++)
			{
				const uint16_t v = (uint16_t)(q * 4);
				indices[q * 6 + 0] = v + 0;
				indices[q * 6 + 1] = v + 1;
				indices[q * 6 + 2] = v + 2;
				indices[q * 6 + 3] = v + 0;
				indices[q * 6 + 4] = v + 2;
				indices[q * 6 + 5] = v + 3;
			}
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, r->ui_ibo);
			glBufferData(GL_ELEMENT_ARRAY_BUFFER,
				(GLsizeiptr)(UI_MAX_QUADS * 6 * sizeof(uint16_t)), indices, GL_STATIC_DRAW);
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
			free(indices);
		}
	}

	if (r->has_pbo_dma)
	{
		const GLsizeiptr pbo_sz = (GLsizeiptr)(MAX_GAME_W * MAX_GAME_H * sizeof(uint16_t));
		glGenBuffers(2, r->pbos);
		for (int i = 0; i < 2; i++)
		{
			glBindBuffer(GL_PIXEL_UNPACK_BUFFER, r->pbos[i]);
			glBufferData(GL_PIXEL_UNPACK_BUFFER, pbo_sz, NULL, GL_STREAM_DRAW);
		}
		glBindBuffer(GL_PIXEL_UNPACK_BUFFER, 0);
	}

	upload_ui_atlas(r);
	recreate_frame_texture(r, MAX_GAME_W, MAX_GAME_H, r->etc2_mode == GLES_ETC2_FULL_STREAM);

	glDisable(GL_DEPTH_TEST);
	glDisable(GL_CULL_FACE);
	return r;
}

void gles_renderer_destroy(gles_renderer *r)
{
	if (!r)
		return;

	if (r->pbos[0])    glDeleteBuffers(2, r->pbos);
	if (r->quad_vbo)   glDeleteBuffers(1, &r->quad_vbo);
	if (r->ui_vbo)     glDeleteBuffers(1, &r->ui_vbo);
	if (r->ui_ibo)     glDeleteBuffers(1, &r->ui_ibo);
	if (r->frame_tex)  glDeleteTextures(1, &r->frame_tex);
	if (r->ui_tex)     glDeleteTextures(1, &r->ui_tex);
	if (r->prog_basic) glDeleteProgram(r->prog_basic);
	if (r->prog_sharp) glDeleteProgram(r->prog_sharp);
	if (r->prog_crt)   glDeleteProgram(r->prog_crt);
	if (r->prog_ui)    glDeleteProgram(r->prog_ui);

	free(r->rgb565_staging);
	free(r->etc2_frame_blocks);
	free(r->etc2_block_hashes);
	free(r);
}

void gles_renderer_set_filter(gles_renderer *r, gles_filter_mode filter)
{
	if (r)
		r->filter = (gles_filter_mode)((unsigned)filter % GLES_FILTER_COUNT);
}

gles_filter_mode gles_renderer_get_filter(const gles_renderer *r)
{
	return r ? r->filter : GLES_FILTER_SHARP_BILINEAR;
}

const char *gles_renderer_filter_name(gles_filter_mode filter)
{
	switch (filter)
	{
		case GLES_FILTER_SHARP_BILINEAR: return "SHARP AA";
		case GLES_FILTER_NEAREST:        return "PIXELS";
		case GLES_FILTER_SMOOTH:         return "SMOOTH";
		case GLES_FILTER_CRT:            return "CRT 15KHZ";
		default:                         return "SHARP AA";
	}
}

void gles_renderer_set_aspect(gles_renderer *r, gles_aspect_mode aspect)
{
	if (r)
		r->aspect = (gles_aspect_mode)((unsigned)aspect % GLES_ASPECT_COUNT);
}

gles_aspect_mode gles_renderer_get_aspect(const gles_renderer *r)
{
	return r ? r->aspect : GLES_ASPECT_AUTO;
}

const char *gles_renderer_aspect_name(gles_aspect_mode aspect, bool wide_enabled)
{
	switch (aspect)
	{
		case GLES_ASPECT_4_3:  return "4:3 CRT";
		case GLES_ASPECT_FILL: return "FULL FILL";
		case GLES_ASPECT_AUTO:
		default:               return wide_enabled ? "16:9 WIDE" : "4:3 AUTO";
	}
}

void gles_renderer_set_etc2_mode(gles_renderer *r, gles_etc2_mode mode)
{
	if (!r)
		return;
	mode = (gles_etc2_mode)((unsigned)mode % GLES_ETC2_MODE_COUNT);
	if (r->etc2_mode == mode)
		return;
	r->etc2_mode = mode;

	if (r->ui_tex)
	{
		glDeleteTextures(1, &r->ui_tex);
		r->ui_tex = 0;
	}
	upload_ui_atlas(r);
	recreate_frame_texture(r, r->frame_tex_w, r->frame_tex_h, mode == GLES_ETC2_FULL_STREAM);
}

gles_etc2_mode gles_renderer_get_etc2_mode(const gles_renderer *r)
{
	return r ? r->etc2_mode : GLES_ETC2_AUTO_ATLAS;
}

void gles_renderer_get_info(const gles_renderer *r, gles_renderer_info *out_info)
{
	if (!out_info)
		return;
	memset(out_info, 0, sizeof(*out_info));
	if (!r)
		return;

	out_info->is_gles3 = r->is_gles3;
	out_info->has_etc2_hw = r->has_etc2_hw;
	out_info->has_pbo_dma = r->has_pbo_dma;
	out_info->ui_atlas_compressed = r->ui_atlas_compressed;
	out_info->frame_etc2_active = r->frame_tex_is_etc2;

	if (r->frame_tex_is_etc2)
		snprintf(out_info->summary, sizeof(out_info->summary), "GLES3 ETC2 STREAM");
	else if (r->ui_atlas_compressed && r->has_pbo_dma)
		snprintf(out_info->summary, sizeof(out_info->summary), "GLES3 ETC2+PBO");
	else if (r->ui_atlas_compressed)
		snprintf(out_info->summary, sizeof(out_info->summary), "GLES3 ETC2");
	else if (r->is_gles3)
		snprintf(out_info->summary, sizeof(out_info->summary), "GLES3 RGB565");
	else
		snprintf(out_info->summary, sizeof(out_info->summary), "GLES2 RGB565");
}

/* ---- Per-Frame Upload & Batched Rendering ---- */

static void upload_game_frame(gles_renderer *r, const uint32_t *xrgb_frame, int stride_px, int w, int h)
{
	const bool want_etc2 = (r->etc2_mode == GLES_ETC2_FULL_STREAM) && r->has_etc2_hw;

	if (w != r->frame_tex_w || h != r->frame_tex_h || (want_etc2 != r->frame_tex_is_etc2 && r->has_etc2_hw))
		recreate_frame_texture(r, w, h, want_etc2);

	glBindTexture(GL_TEXTURE_2D, r->frame_tex);

	if (r->frame_tex_is_etc2)
	{
		int dy0 = 0, dy1 = 0;
		const int changed = etc2_compress_xrgb32_stream_dirty(
			xrgb_frame, w, h, stride_px, r->etc2_frame_blocks, r->etc2_block_hashes, &dy0, &dy1);

		if (changed > 0 && dy1 > dy0)
		{
			const int bw = etc2_blocks_dim(w);
			const size_t offset = (size_t)(dy0 >> 2) * (size_t)bw * 8u;
			const size_t slice_bytes = (size_t)((dy1 - dy0) >> 2) * (size_t)bw * 8u;

			glCompressedTexSubImage2D(GL_TEXTURE_2D, 0, 0, dy0, w, dy1 - dy0,
				ETC2_GL_COMPRESSED_RGB8_ETC2, (GLsizei)slice_bytes, r->etc2_frame_blocks + offset);
		}
		return;
	}

	/* Lossless RGB565 path (with row dirty-tracking + GLES3 Async Double-PBO DMA!) */
	{
		int y0 = h;
		int y1 = -1;

		for (int y = 0; y < h; y++)
		{
			const uint64_t rh = hash_row_xrgb(xrgb_frame + (size_t)y * (size_t)stride_px, w);
			if (r->row_hashes[y] != rh)
			{
				r->row_hashes[y] = rh;
				if (y < y0) y0 = y;
				if (y > y1) y1 = y;
			}
		}

		if (y1 < y0)
			return;
		y1 += 1;

		glPixelStorei(GL_UNPACK_ALIGNMENT, 2);
		{
			const int dirty_h = y1 - y0;
			const size_t dirty_bytes = (size_t)w * (size_t)dirty_h * sizeof(uint16_t);

			if (r->has_pbo_dma && r->pbos[0])
			{
				r->pbo_idx ^= 1;
				glBindBuffer(GL_PIXEL_UNPACK_BUFFER, r->pbos[r->pbo_idx]);
				uint16_t *mapped = (uint16_t *)r->fn_map_buffer_range(
					GL_PIXEL_UNPACK_BUFFER, 0, (GLsizeiptr)dirty_bytes,
					GL_MAP_WRITE_BIT | GL_MAP_INVALIDATE_BUFFER_BIT);

				if (mapped)
				{
					convert_xrgb32_rows_to_rgb565(xrgb_frame, stride_px, mapped, w, y0, y1);
					r->fn_unmap_buffer(GL_PIXEL_UNPACK_BUFFER);
					glTexSubImage2D(GL_TEXTURE_2D, 0, 0, y0, w, dirty_h,
						GL_RGB, GL_UNSIGNED_SHORT_5_6_5, (const void *)0);
					glBindBuffer(GL_PIXEL_UNPACK_BUFFER, 0);
					return;
				}
				glBindBuffer(GL_PIXEL_UNPACK_BUFFER, 0);
			}

			convert_xrgb32_rows_to_rgb565(xrgb_frame, stride_px, r->rgb565_staging, w, y0, y1);
			glTexSubImage2D(GL_TEXTURE_2D, 0, 0, y0, w, dirty_h,
				GL_RGB, GL_UNSIGNED_SHORT_5_6_5, r->rgb565_staging);
		}
	}
}

void gles_renderer_draw_frame(gles_renderer *r,
	const uint32_t *xrgb_frame, int frame_width, int visible_width, int frame_height,
	int viewport_w, int viewport_h,
	const ui_quad_batch *ui_batch)
{
	if (!r || viewport_w <= 0 || viewport_h <= 0)
		return;

	glViewport(0, 0, viewport_w, viewport_h);
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);

	/* 1. Upload & Draw System 32 Game Frame */
	if (xrgb_frame && frame_width > 0 && frame_height > 0)
	{
		const int src_w = (visible_width > 0 && visible_width <= frame_width) ? visible_width : frame_width;
		const int src_x = (frame_width - src_w) / 2;

		upload_game_frame(r, xrgb_frame, MAX_GAME_W, MAX_GAME_W, frame_height);

		/* Compute aspect-preserving target rectangle in pixels -> NDC [-1..1] */
		float target_aspect;
		if (r->aspect == GLES_ASPECT_FILL)
			target_aspect = (float)viewport_w / (float)viewport_h;
		else if (r->aspect == GLES_ASPECT_4_3 || src_w <= 320)
			target_aspect = 4.0f / 3.0f;
		else
			target_aspect = (float)src_w / (float)frame_height;

		float shown_w = (float)viewport_w;
		float shown_h = shown_w / target_aspect;
		if (shown_h > (float)viewport_h)
		{
			shown_h = (float)viewport_h;
			shown_w = shown_h * target_aspect;
		}

		const float nx = shown_w / (float)viewport_w;
		const float ny = shown_h / (float)viewport_h;

		const bool use_texel_coords =
			(r->filter == GLES_FILTER_SHARP_BILINEAR && r->prog_sharp != 0) ||
			(r->filter == GLES_FILTER_CRT && r->prog_crt != 0);

		const float tx0 = use_texel_coords ? (float)src_x : ((float)src_x / (float)r->frame_tex_w);
		const float tx1 = use_texel_coords ? (float)(src_x + src_w) : ((float)(src_x + src_w) / (float)r->frame_tex_w);
		const float ty0 = 0.0f;
		const float ty1 = use_texel_coords ? (float)frame_height : ((float)frame_height / (float)r->frame_tex_h);

		const float quad_verts[16] = {
			-nx,  ny, tx0, ty0,
			 nx,  ny, tx1, ty0,
			-nx, -ny, tx0, ty1,
			 nx, -ny, tx1, ty1
		};

		glDisable(GL_BLEND);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, r->frame_tex);

		if (r->filter == GLES_FILTER_NEAREST)
		{
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
			glUseProgram(r->prog_basic);
		}
		else if (r->filter == GLES_FILTER_CRT && r->prog_crt)
		{
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			glUseProgram(r->prog_crt);
			glUniform2f(r->u_crt_inv_tex, 1.0f / (float)r->frame_tex_w, 1.0f / (float)r->frame_tex_h);
			glUniform2f(r->u_crt_scale, shown_w / (float)src_w, shown_h / (float)frame_height);
		}
		else if (r->filter == GLES_FILTER_SHARP_BILINEAR && r->prog_sharp)
		{
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			glUseProgram(r->prog_sharp);
			glUniform2f(r->u_sharp_inv_tex, 1.0f / (float)r->frame_tex_w, 1.0f / (float)r->frame_tex_h);
			glUniform2f(r->u_sharp_scale, shown_w / (float)src_w, shown_h / (float)frame_height);
		}
		else
		{
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			glUseProgram(r->prog_basic);
		}

		glBindBuffer(GL_ARRAY_BUFFER, r->quad_vbo);
		glBufferData(GL_ARRAY_BUFFER, sizeof(quad_verts), quad_verts, GL_STREAM_DRAW);
		glEnableVertexAttribArray(0);
		glEnableVertexAttribArray(1);
		glDisableVertexAttribArray(2);
		glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (const void *)0);
		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (const void *)(2 * sizeof(float)));
		glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
	}

	/* 2. Draw Single-Call Batched Touch Controls, Font & Modal UI */
	if (ui_batch && ui_batch->quad_count > 0 && r->ui_tex)
	{
		const int qc = (ui_batch->quad_count <= UI_MAX_QUADS) ? ui_batch->quad_count : UI_MAX_QUADS;

		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		glUseProgram(r->prog_ui);
		glUniform2f(r->u_ui_vp, (float)viewport_w, (float)viewport_h);

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, r->ui_tex);

		glBindBuffer(GL_ARRAY_BUFFER, r->ui_vbo);
		glBufferData(GL_ARRAY_BUFFER, (GLsizeiptr)(qc * 4 * sizeof(ui_vertex)),
			ui_batch->verts, GL_STREAM_DRAW);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, r->ui_ibo);

		glEnableVertexAttribArray(0);
		glEnableVertexAttribArray(1);
		glEnableVertexAttribArray(2);
		glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(ui_vertex), (const void *)offsetof(ui_vertex, x));
		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(ui_vertex), (const void *)offsetof(ui_vertex, u));
		glVertexAttribPointer(2, 4, GL_UNSIGNED_BYTE, GL_TRUE, sizeof(ui_vertex), (const void *)offsetof(ui_vertex, r));

		glDrawElements(GL_TRIANGLES, qc * 6, GL_UNSIGNED_SHORT, (const void *)0);

		glDisableVertexAttribArray(2);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	}

	glDisableVertexAttribArray(0);
	glDisableVertexAttribArray(1);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glUseProgram(0);
}
