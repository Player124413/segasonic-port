/*
 * SegaSonic The Hedgehog — Android Port
 * Khronos OpenGL ES 3.0 ETC2 / EAC Texture Compression & Caching Module
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "etc2.h"

#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#if defined(__ARM_NEON) || defined(__ARM_NEON__)
#include <arm_neon.h>
#define ETC2_HAS_NEON 1
#else
#define ETC2_HAS_NEON 0
#endif

/* ---- Lookup tables from Khronos OpenGL ES 3.0 Specification Appendix C ---- */

/*
 * Standard ETC1/ETC2 subblock luminance modifier tables (Table C.8).
 * Index mapping for 2-bit pixel code `(msb << 1) | lsb`:
 *   0 (msb=0, lsb=0): +small
 *   1 (msb=0, lsb=1): +large
 *   2 (msb=1, lsb=0): -small
 *   3 (msb=1, lsb=1): -large
 */
static const int16_t k_etc_mod_tables[8][4] = {
	{   2,   8,  -2,   -8 },
	{   5,  17,  -5,  -17 },
	{   9,  29,  -9,  -29 },
	{  13,  42, -13,  -42 },
	{  18,  60, -18,  -60 },
	{  24,  80, -24,  -80 },
	{  33, 106, -33, -106 },
	{  47, 183, -47, -183 }
};

/* ETC2 T-mode and H-mode distance table (Table C.9) */
static const uint8_t k_etc2_th_dist[8] = {
	3, 6, 11, 16, 23, 32, 41, 64
};

/* EAC 8-bit alpha signed 3-bit modifier tables (Table C.10) */
static const int8_t k_eac_alpha_tables[16][8] = {
	{ -3, -6,  -9, -15,  2, 5, 8, 14 },
	{ -3, -7, -10, -13,  2, 6, 9, 12 },
	{ -2, -5,  -8, -13,  1, 4, 7, 12 },
	{ -2, -4,  -6, -13,  1, 3, 5, 12 },
	{ -3, -6,  -8, -12,  2, 5, 7, 11 },
	{ -3, -7,  -9, -11,  2, 6, 8, 10 },
	{ -4, -7,  -8, -11,  3, 6, 7, 10 },
	{ -3, -5,  -8, -11,  2, 4, 7, 10 },
	{ -2, -6,  -8, -10,  1, 5, 7,  9 },
	{ -2, -5,  -8, -10,  1, 4, 7,  9 },
	{ -2, -4,  -8, -10,  1, 3, 7,  9 },
	{ -2, -5,  -7, -10,  1, 4, 6,  9 },
	{ -3, -4,  -7, -10,  2, 3, 6,  9 },
	{ -1, -2,  -3, -10,  0, 1, 2,  9 },
	{ -4, -6,  -8,  -9,  3, 5, 7,  8 },
	{ -3, -5,  -7,  -9,  2, 4, 6,  8 }
};

/* Flip=0 subblock pixel indices in column-major p = x*4 + y */
static const uint8_t k_subblock_pixels[2][2][8] = {
	/* flip = 0: two 2x4 vertical halves */
	{
		{ 0, 1, 2, 3, 4, 5, 6, 7 },
		{ 8, 9, 10, 11, 12, 13, 14, 15 }
	},
	/* flip = 1: two 4x2 horizontal halves */
	{
		{ 0, 1, 4, 5, 8, 9, 12, 13 },
		{ 2, 3, 6, 7, 10, 11, 14, 15 }
	}
};

/* ---- Basic integer & bit utilities ---- */

static inline uint8_t clamp_u8(int v)
{
	if (v < 0)
		return 0;
	if (v > 255)
		return 255;
	return (uint8_t)v;
}

static inline int clamp_int(int v, int lo, int hi)
{
	if (v < lo)
		return lo;
	if (v > hi)
		return hi;
	return v;
}

static inline uint8_t expand4to8(int v)
{
	return (uint8_t)((v << 4) | v);
}

static inline uint8_t expand5to8(int v)
{
	return (uint8_t)((v << 3) | (v >> 2));
}

static inline uint8_t expand6to8(int v)
{
	return (uint8_t)((v << 2) | (v >> 4));
}

static inline uint8_t expand7to8(int v)
{
	return (uint8_t)((v << 1) | (v >> 6));
}

static inline int quant8to4(int v)
{
	return clamp_int((v * 15 + 135) >> 8, 0, 15);
}

static inline int quant8to5(int v)
{
	return clamp_int((v * 31 + 135) >> 8, 0, 31);
}

static inline int quant8to6(int v)
{
	return clamp_int((v * 63 + 135) >> 8, 0, 63);
}

static inline int quant8to7(int v)
{
	return clamp_int((v * 127 + 135) >> 8, 0, 127);
}

static inline void write_be64(uint8_t out[8], uint64_t code)
{
	out[0] = (uint8_t)(code >> 56);
	out[1] = (uint8_t)(code >> 48);
	out[2] = (uint8_t)(code >> 40);
	out[3] = (uint8_t)(code >> 32);
	out[4] = (uint8_t)(code >> 24);
	out[5] = (uint8_t)(code >> 16);
	out[6] = (uint8_t)(code >> 8);
	out[7] = (uint8_t)(code);
}

static inline uint64_t read_be64(const uint8_t in[8])
{
	return ((uint64_t)in[0] << 56) |
	       ((uint64_t)in[1] << 48) |
	       ((uint64_t)in[2] << 40) |
	       ((uint64_t)in[3] << 32) |
	       ((uint64_t)in[4] << 24) |
	       ((uint64_t)in[5] << 16) |
	       ((uint64_t)in[6] << 8)  |
	       ((uint64_t)in[7]);
}

static inline uint32_t color_sq_err(int r0, int g0, int b0, int r1, int g1, int b1)
{
	const int dr = r0 - r1;
	const int dg = g0 - g1;
	const int db = b0 - b1;
	return (uint32_t)(dr * dr + dg * dg + db * db);
}

size_t etc2_rgb8_data_size(int width, int height)
{
	return (size_t)etc2_blocks_dim(width) * (size_t)etc2_blocks_dim(height) * 8u;
}

size_t etc2_rgba8_data_size(int width, int height)
{
	return (size_t)etc2_blocks_dim(width) * (size_t)etc2_blocks_dim(height) * 16u;
}

/* ---- IEEE 802.3 CRC32 ---- */

uint32_t etc2_crc32(const void *data, size_t size)
{
	static uint32_t table[256];
	static bool initialized = false;
	const uint8_t *bytes = (const uint8_t *)data;
	uint32_t crc = 0xffffffffu;

	if (!initialized)
	{
		for (uint32_t i = 0; i < 256; i++)
		{
			uint32_t c = i;
			for (int b = 0; b < 8; b++)
				c = (c & 1u) ? (0xedb88320u ^ (c >> 1)) : (c >> 1);
			table[i] = c;
		}
		initialized = true;
	}

	for (size_t i = 0; i < size; i++)
		crc = table[(crc ^ bytes[i]) & 0xffu] ^ (crc >> 8);
	return crc ^ 0xffffffffu;
}

/* ---- Subblock & Multi-Mode ETC2 RGB8 Block Encoders ---- */

/*
 * Evaluate an 8-pixel subblock against a candidate base color (br, bg, bb),
 * finding the best modifier table (0..7) and 2-bit pixel codes.
 * Writes pixel index bits directly into `*out_idx_bits` (bits 31..16 = MSB, bits 15..0 = LSB).
 */
static uint32_t fit_subblock_tables(const uint8_t block_rgb[16][3], const uint8_t indices[8],
	int br, int bg, int bb, uint32_t *out_cw, uint32_t *out_idx_bits, uint32_t err_limit)
{
	uint32_t best_err = UINT32_MAX;
	uint32_t best_cw = 0;
	uint32_t best_bits = 0;

	for (uint32_t cw = 0; cw < 8; cw++)
	{
		const int16_t *mods = k_etc_mod_tables[cw];
		uint8_t cand[4][3];
		uint32_t table_err = 0;
		uint32_t bits = 0;

		for (int m = 0; m < 4; m++)
		{
			cand[m][0] = clamp_u8(br + mods[m]);
			cand[m][1] = clamp_u8(bg + mods[m]);
			cand[m][2] = clamp_u8(bb + mods[m]);
		}

		for (int i = 0; i < 8; i++)
		{
			const uint8_t p = indices[i];
			const int pr = block_rgb[p][0];
			const int pg = block_rgb[p][1];
			const int pb = block_rgb[p][2];

			uint32_t px_best = color_sq_err(pr, pg, pb, cand[0][0], cand[0][1], cand[0][2]);
			uint32_t px_code = 0;

			for (uint32_t m = 1; m < 4; m++)
			{
				const uint32_t e = color_sq_err(pr, pg, pb, cand[m][0], cand[m][1], cand[m][2]);
				if (e < px_best)
				{
					px_best = e;
					px_code = m;
				}
			}

			table_err += px_best;
			if (table_err >= best_err || table_err >= err_limit)
				break;

			if (px_code & 1u)
				bits |= (1u << p);
			if (px_code & 2u)
				bits |= (1u << (16u + p));
		}

		if (table_err < best_err)
		{
			best_err = table_err;
			best_cw = cw;
			best_bits = bits;
			if (best_err == 0)
				break;
		}
	}

	*out_cw = best_cw;
	*out_idx_bits = best_bits;
	return best_err;
}

/* Encode Differential (RGB555 + dRGB333) or Individual (RGB444 + RGB444) for a given flip bit */
static uint32_t try_encode_subblocks(const uint8_t block_rgb[16][3], int flip,
	uint64_t *best_code, uint32_t best_err, bool allow_individual)
{
	int avg[2][3] = { { 0, 0, 0 }, { 0, 0, 0 } };

	for (int s = 0; s < 2; s++)
	{
		const uint8_t *idxs = k_subblock_pixels[flip][s];
		for (int i = 0; i < 8; i++)
		{
			const uint8_t p = idxs[i];
			avg[s][0] += block_rgb[p][0];
			avg[s][1] += block_rgb[p][1];
			avg[s][2] += block_rgb[p][2];
		}
		avg[s][0] = (avg[s][0] + 4) >> 3;
		avg[s][1] = (avg[s][1] + 4) >> 3;
		avg[s][2] = (avg[s][2] + 4) >> 3;
	}

	/* 1. Try Differential mode (555 + 3-bit signed delta -4..+3) */
	{
		const int r0 = quant8to5(avg[0][0]);
		const int g0 = quant8to5(avg[0][1]);
		const int b0 = quant8to5(avg[0][2]);
		const int r1_target = quant8to5(avg[1][0]);
		const int g1_target = quant8to5(avg[1][1]);
		const int b1_target = quant8to5(avg[1][2]);
		const int dr = r1_target - r0;
		const int dg = g1_target - g0;
		const int db = b1_target - b0;

		if (dr >= -4 && dr <= 3 && dg >= -4 && dg <= 3 && db >= -4 && db <= 3)
		{
			uint32_t cw0 = 0, cw1 = 0, bits0 = 0, bits1 = 0;
			const int r1 = r0 + dr;
			const int g1 = g0 + dg;
			const int b1 = b0 + db;
			const uint32_t err0 = fit_subblock_tables(block_rgb, k_subblock_pixels[flip][0],
				expand5to8(r0), expand5to8(g0), expand5to8(b0), &cw0, &bits0, best_err);

			if (err0 < best_err)
			{
				const uint32_t err1 = fit_subblock_tables(block_rgb, k_subblock_pixels[flip][1],
					expand5to8(r1), expand5to8(g1), expand5to8(b1), &cw1, &bits1, best_err - err0);
				if (err0 + err1 < best_err)
				{
					best_err = err0 + err1;
					*best_code =
						((uint64_t)(r0 & 0x1f) << 59) |
						((uint64_t)(dr & 0x07) << 56) |
						((uint64_t)(g0 & 0x1f) << 51) |
						((uint64_t)(dg & 0x07) << 48) |
						((uint64_t)(b0 & 0x1f) << 43) |
						((uint64_t)(db & 0x07) << 40) |
						((uint64_t)(cw0 & 0x07) << 37) |
						((uint64_t)(cw1 & 0x07) << 34) |
						(1ULL << 33) |
						((uint64_t)(flip & 1) << 32) |
						(uint64_t)(bits0 | bits1);
				}
			}
		}
	}

	/* 2. Try Individual mode (444 + 444) when requested or when differential didn't fit */
	if (allow_individual && best_err > 0)
	{
		const int r0 = quant8to4(avg[0][0]);
		const int g0 = quant8to4(avg[0][1]);
		const int b0 = quant8to4(avg[0][2]);
		const int r1 = quant8to4(avg[1][0]);
		const int g1 = quant8to4(avg[1][1]);
		const int b1 = quant8to4(avg[1][2]);
		uint32_t cw0 = 0, cw1 = 0, bits0 = 0, bits1 = 0;
		const uint32_t err0 = fit_subblock_tables(block_rgb, k_subblock_pixels[flip][0],
			expand4to8(r0), expand4to8(g0), expand4to8(b0), &cw0, &bits0, best_err);

		if (err0 < best_err)
		{
			const uint32_t err1 = fit_subblock_tables(block_rgb, k_subblock_pixels[flip][1],
				expand4to8(r1), expand4to8(g1), expand4to8(b1), &cw1, &bits1, best_err - err0);
			if (err0 + err1 < best_err)
			{
				best_err = err0 + err1;
				*best_code =
					((uint64_t)(r0 & 0x0f) << 60) |
					((uint64_t)(r1 & 0x0f) << 56) |
					((uint64_t)(g0 & 0x0f) << 52) |
					((uint64_t)(g1 & 0x0f) << 48) |
					((uint64_t)(b0 & 0x0f) << 44) |
					((uint64_t)(b1 & 0x0f) << 40) |
					((uint64_t)(cw0 & 0x07) << 37) |
					((uint64_t)(cw1 & 0x07) << 34) |
					/* diffbit = 0 at bit 33 */
					((uint64_t)(flip & 1) << 32) |
					(uint64_t)(bits0 | bits1);
			}
		}
	}

	return best_err;
}

/*
 * Pack Khronos ETC2 Planar mode codeword (RO/RH/RV 6-bit, GO/GH/GV 7-bit, BO/BH/BV 6-bit).
 * Guarantees R and G differential fields do NOT overflow [0..31], while B DOES overflow [0..31],
 * cleanly triggering the hardware ETC2 Planar decoder on every OpenGL ES 3.0 GPU.
 */
static uint64_t pack_planar_code(int ro, int go, int bo, int rh, int gh, int bh, int rv, int gv, int bv)
{
	uint64_t code = 0;

	/* Pack data bits first */
	code |= ((uint64_t)(ro & 0x3f) << 57);
	code |= ((uint64_t)((go >> 6) & 0x01) << 56);
	code |= ((uint64_t)(go & 0x3f) << 49);
	code |= ((uint64_t)((bo >> 5) & 0x01) << 48);
	code |= ((uint64_t)((bo >> 3) & 0x03) << 43);
	code |= ((uint64_t)(bo & 0x07) << 39);

	code |= ((uint64_t)((rh >> 1) & 0x1f) << 34);
	code |= (1ULL << 33); /* diffbit = 1 */
	code |= ((uint64_t)(rh & 0x01) << 32);

	code |= ((uint64_t)(gh & 0x7f) << 25);
	code |= ((uint64_t)(bh & 0x3f) << 19);
	code |= ((uint64_t)(rv & 0x3f) << 13);
	code |= ((uint64_t)(gv & 0x7f) << 6);
	code |= ((uint64_t)(bv & 0x3f));

	/* Prevent R overflow: bit 63 mirrors dR sign bit (bit 58) */
	if (code & (1ULL << 58))
		code |= (1ULL << 63);

	/* Prevent G overflow: bit 55 mirrors dG sign bit (bit 50) */
	if (code & (1ULL << 50))
		code |= (1ULL << 55);

	/* Force B overflow using unused mode bits 47..45 and 42 */
	{
		const int b43 = (bo >> 3) & 0x03;
		const int b21 = (bo >> 1) & 0x03;

		if (b43 + b21 < 4)
		{
			/* B_base = b43 (0..3), dB = b21 - 4 (-4..-1) => B_base + dB < 0 */
			code |= (1ULL << 42);
		}
		else
		{
			/* B_base = 28 + b43, dB = b21 (1..3) => B_base + dB >= 32 */
			code |= (7ULL << 45);
		}
	}

	return code;
}

/*
 * Exact closed-form Least-Squares linear plane fit over the 4x4 block:
 *   f(x, y) = c0 + cx * x + cy * y   for x, y in {0, 1, 2, 3}.
 *
 * Because x and y are orthogonal over the uniform 4x4 grid:
 *   sum(1)=16, sum(x)=sum(y)=24, sum(x^2)=sum(y^2)=56, sum((x-1.5)^2)=20:
 *   cx = (2 * sum(x * P) - 3 * sum(P)) / 40
 *   cy = (2 * sum(y * P) - 3 * sum(P)) / 40
 *   c0 = (sum(P) - 24 * cx - 24 * cy) / 16
 * And the Khronos Planar endpoints are:
 *   CO = f(0, 0) = c0
 *   CH = f(4, 0) = c0 + 4 * cx
 *   CV = f(0, 4) = c0 + 4 * cy
 */
static uint32_t try_encode_planar(const uint8_t block_rgb[16][3], uint64_t *best_code, uint32_t best_err)
{
	int ro6[3]; /* R=0, G=1, B=2 */
	int rh6[3];
	int rv6[3];

	for (int c = 0; c < 3; c++)
	{
		int sum_p = 0;
		int sum_xp = 0;
		int sum_yp = 0;

		for (int x = 0; x < 4; x++)
		{
			for (int y = 0; y < 4; y++)
			{
				const int v = block_rgb[x * 4 + y][c];
				sum_p += v;
				sum_xp += x * v;
				sum_yp += y * v;
			}
		}

		/* Scale by 40 to keep exact integer precision before quantization */
		const int cx40 = 2 * sum_xp - 3 * sum_p;
		const int cy40 = 2 * sum_yp - 3 * sum_p;
		/* c0 * 40 = (sum_p * 40 - 24 * cx40 - 24 * cy40) / 16 = (5 * sum_p - 3 * cx40 - 3 * cy40) / 2 */
		const int c0_80 = 5 * sum_p - 3 * cx40 - 3 * cy40;
		const int co = (c0_80 + 40) / 80;
		const int ch = (c0_80 + 8 * cx40 + 40) / 80;
		const int cv = (c0_80 + 8 * cy40 + 40) / 80;

		if (c == 1)
		{
			ro6[c] = quant8to7(co);
			rh6[c] = quant8to7(ch);
			rv6[c] = quant8to7(cv);
		}
		else
		{
			ro6[c] = quant8to6(co);
			rh6[c] = quant8to6(ch);
			rv6[c] = quant8to6(cv);
		}
	}

	{
		const int ro8 = expand6to8(ro6[0]);
		const int go8 = expand7to8(ro6[1]);
		const int bo8 = expand6to8(ro6[2]);
		const int drh = expand6to8(rh6[0]) - ro8;
		const int dgh = expand7to8(rh6[1]) - go8;
		const int dbh = expand6to8(rh6[2]) - bo8;
		const int drv = expand6to8(rv6[0]) - ro8;
		const int dgv = expand7to8(rv6[1]) - go8;
		const int dbv = expand6to8(rv6[2]) - bo8;
		uint32_t err = 0;

		for (int x = 0; x < 4; x++)
		{
			for (int y = 0; y < 4; y++)
			{
				const int p = x * 4 + y;
				const int r = clamp_u8((x * drh + y * drv + 4 * ro8 + 2) >> 2);
				const int g = clamp_u8((x * dgh + y * dgv + 4 * go8 + 2) >> 2);
				const int b = clamp_u8((x * dbh + y * dbv + 4 * bo8 + 2) >> 2);

				err += color_sq_err(block_rgb[p][0], block_rgb[p][1], block_rgb[p][2], r, g, b);
				if (err >= best_err)
					return best_err;
			}
		}

		*best_code = pack_planar_code(
			ro6[0], ro6[1], ro6[2],
			rh6[0], rh6[1], rh6[2],
			rv6[0], rv6[1], rv6[2]);
		return err;
	}
}

/* Pack Khronos ETC2 T-mode codeword (forces R differential overflow) */
static uint64_t pack_t_mode_code(int r0, int g0, int b0, int r2, int g2, int b2, int dist_idx, uint32_t idx_bits)
{
	uint64_t code = 0;
	const int r32 = (r0 >> 2) & 0x03;
	const int r10 = r0 & 0x03;

	code |= ((uint64_t)r32 << 59);
	code |= ((uint64_t)r10 << 56);
	if (r32 + r10 < 4)
		code |= (1ULL << 58); /* R_base = r32, dR = r10 - 4 < 0 */
	else
		code |= (7ULL << 61); /* R_base = 28 + r32, dR = r10 => >= 32 */

	code |= ((uint64_t)(g0 & 0x0f) << 52);
	code |= ((uint64_t)(b0 & 0x0f) << 48);
	code |= ((uint64_t)(r2 & 0x0f) << 44);
	code |= ((uint64_t)(g2 & 0x0f) << 40);
	code |= ((uint64_t)(b2 & 0x0f) << 36);
	code |= ((uint64_t)((dist_idx >> 1) & 0x03) << 34);
	code |= (1ULL << 33); /* diffbit = 1 */
	code |= ((uint64_t)(dist_idx & 0x01) << 32);
	code |= (uint64_t)idx_bits;
	return code;
}

/*
 * ETC2 T-mode encoder: splits the 16 pixels into two luminance clusters (ideal for
 * sharp glyph strokes, UI icons, and high-contrast pixel edges across arbitrary shapes).
 */
static uint32_t try_encode_t_mode(const uint8_t block_rgb[16][3], uint64_t *best_code, uint32_t best_err)
{
	int min_lum = INT_MAX, max_lum = INT_MIN;
	int min_p = 0, max_p = 0;

	for (int p = 0; p < 16; p++)
	{
		const int lum = block_rgb[p][0] * 2 + block_rgb[p][1] * 5 + block_rgb[p][2];
		if (lum < min_lum)
		{
			min_lum = lum;
			min_p = p;
		}
		if (lum > max_lum)
		{
			max_lum = lum;
			max_p = p;
		}
	}

	if (max_lum - min_lum < 24)
		return best_err;

	{
		const int mid_lum = (min_lum + max_lum) >> 1;
		int c_lo[3] = { 0, 0, 0 }, c_hi[3] = { 0, 0, 0 };
		int n_lo = 0, n_hi = 0;

		for (int p = 0; p < 16; p++)
		{
			const int lum = block_rgb[p][0] * 2 + block_rgb[p][1] * 5 + block_rgb[p][2];
			if (lum <= mid_lum)
			{
				c_lo[0] += block_rgb[p][0];
				c_lo[1] += block_rgb[p][1];
				c_lo[2] += block_rgb[p][2];
				n_lo++;
			}
			else
			{
				c_hi[0] += block_rgb[p][0];
				c_hi[1] += block_rgb[p][1];
				c_hi[2] += block_rgb[p][2];
				n_hi++;
			}
		}

		if (n_lo == 0 || n_hi == 0)
		{
			c_lo[0] = block_rgb[min_p][0];
			c_lo[1] = block_rgb[min_p][1];
			c_lo[2] = block_rgb[min_p][2];
			c_hi[0] = block_rgb[max_p][0];
			c_hi[1] = block_rgb[max_p][1];
			c_hi[2] = block_rgb[max_p][2];
		}
		else
		{
			c_lo[0] = (c_lo[0] + (n_lo >> 1)) / n_lo;
			c_lo[1] = (c_lo[1] + (n_lo >> 1)) / n_lo;
			c_lo[2] = (c_lo[2] + (n_lo >> 1)) / n_lo;
			c_hi[0] = (c_hi[0] + (n_hi >> 1)) / n_hi;
			c_hi[1] = (c_hi[1] + (n_hi >> 1)) / n_hi;
			c_hi[2] = (c_hi[2] + (n_hi >> 1)) / n_hi;
		}

		/* Try both assignments: (c0 = lo, c2 = hi) and (c0 = hi, c2 = lo) */
		for (int swap = 0; swap < 2; swap++)
		{
			const int *a = swap ? c_hi : c_lo;
			const int *b = swap ? c_lo : c_hi;
			const int r0 = quant8to4(a[0]);
			const int g0 = quant8to4(a[1]);
			const int b0 = quant8to4(a[2]);
			const int r2 = quant8to4(b[0]);
			const int g2 = quant8to4(b[1]);
			const int b2 = quant8to4(b[2]);
			const uint8_t pal0[3] = { expand4to8(r0), expand4to8(g0), expand4to8(b0) };
			const uint8_t pal2[3] = { expand4to8(r2), expand4to8(g2), expand4to8(b2) };

			for (int d = 0; d < 8; d++)
			{
				const int delta = k_etc2_th_dist[d];
				uint8_t pal[4][3];
				uint32_t err = 0;
				uint32_t bits = 0;

				pal[0][0] = pal0[0];
				pal[0][1] = pal0[1];
				pal[0][2] = pal0[2];
				pal[1][0] = clamp_u8(pal2[0] + delta);
				pal[1][1] = clamp_u8(pal2[1] + delta);
				pal[1][2] = clamp_u8(pal2[2] + delta);
				pal[2][0] = pal2[0];
				pal[2][1] = pal2[1];
				pal[2][2] = pal2[2];
				pal[3][0] = clamp_u8(pal2[0] - delta);
				pal[3][1] = clamp_u8(pal2[1] - delta);
				pal[3][2] = clamp_u8(pal2[2] - delta);

				for (int p = 0; p < 16; p++)
				{
					uint32_t px_best = color_sq_err(block_rgb[p][0], block_rgb[p][1], block_rgb[p][2],
						pal[0][0], pal[0][1], pal[0][2]);
					uint32_t px_code = 0;

					for (uint32_t m = 1; m < 4; m++)
					{
						const uint32_t e = color_sq_err(block_rgb[p][0], block_rgb[p][1], block_rgb[p][2],
							pal[m][0], pal[m][1], pal[m][2]);
						if (e < px_best)
						{
							px_best = e;
							px_code = m;
						}
					}

					err += px_best;
					if (err >= best_err)
						break;

					if (px_code & 1u)
						bits |= (1u << p);
					if (px_code & 2u)
						bits |= (1u << (16u + p));
				}

				if (err < best_err)
				{
					best_err = err;
					*best_code = pack_t_mode_code(r0, g0, b0, r2, g2, b2, d, bits);
					if (best_err == 0)
						return 0;
				}
			}
		}
	}

	return best_err;
}

static uint64_t encode_etc2_rgb_block(const uint8_t block_rgb[16][3], etc2_quality quality)
{
	uint64_t best_code = 0;
	uint32_t best_err = UINT32_MAX;

	/* Fast uniform-block check (exact 0-error in 555/planar if flat) */
	{
		const uint8_t r = block_rgb[0][0];
		const uint8_t g = block_rgb[0][1];
		const uint8_t b = block_rgb[0][2];
		bool uniform = true;

		for (int p = 1; p < 16; p++)
		{
			if (block_rgb[p][0] != r || block_rgb[p][1] != g || block_rgb[p][2] != b)
			{
				uniform = false;
				break;
			}
		}
		if (uniform)
		{
			best_err = try_encode_planar(block_rgb, &best_code, best_err);
			best_err = try_encode_subblocks(block_rgb, 0, &best_code, best_err, true);
			return best_code;
		}
	}

	if (quality == ETC2_QUALITY_FAST)
	{
		best_err = try_encode_subblocks(block_rgb, 0, &best_code, best_err, true);
		if (best_err > 192)
			best_err = try_encode_subblocks(block_rgb, 1, &best_code, best_err, true);
		return best_code;
	}

	/* Planar first: frequently wins with tiny error on smooth UI gradients */
	best_err = try_encode_planar(block_rgb, &best_code, best_err);
	if (best_err == 0)
		return best_code;

	/* Differential & Individual subblocks (flip = 0 and flip = 1) */
	best_err = try_encode_subblocks(block_rgb, 0, &best_code, best_err, true);
	if (best_err == 0)
		return best_code;
	best_err = try_encode_subblocks(block_rgb, 1, &best_code, best_err, true);
	if (best_err == 0)
		return best_code;

	/* High quality: also evaluate ETC2 T-mode for crisp 2-tone UI/font edges */
	if (quality >= ETC2_QUALITY_HIGH)
		best_err = try_encode_t_mode(block_rgb, &best_code, best_err);

	return best_code;
}

/* ---- EAC 8-bit Alpha Block Encoder (Khronos ES 3.0 Table C.10) ---- */

static uint64_t encode_eac_alpha_block(const uint8_t block_a[16], etc2_quality quality)
{
	int min_a = 255;
	int max_a = 0;
	int sum_a = 0;

	for (int p = 0; p < 16; p++)
	{
		const int a = block_a[p];
		if (a < min_a)
			min_a = a;
		if (a > max_a)
			max_a = a;
		sum_a += a;
	}

	/* Constant alpha block: multiplier = 0 reproduces `min_a` with zero error */
	if (min_a == max_a)
		return ((uint64_t)min_a << 56);

	{
		uint64_t best_code = ((uint64_t)((sum_a + 8) >> 4) << 56);
		uint32_t best_err = UINT32_MAX;
		const int span = max_a - min_a;
		const int approx_mul = clamp_int((span + 13) / 27, 1, 15);
		const int mul_lo = (quality >= ETC2_QUALITY_HIGH) ? clamp_int(approx_mul - 2, 1, 15) : approx_mul;
		const int mul_hi = (quality >= ETC2_QUALITY_HIGH) ? clamp_int(approx_mul + 3, 1, 15) : clamp_int(approx_mul + 1, 1, 15);
		const int base_step = (quality >= ETC2_QUALITY_HIGH) ? 1 : 2;

		for (int tbl = 0; tbl < 16; tbl++)
		{
			const int8_t *mods = k_eac_alpha_tables[tbl];

			for (int mul = mul_lo; mul <= mul_hi; mul++)
			{
				/* Center base codeword so [base + mods[3]*mul, base + mods[7]*mul] brackets [min_a, max_a] */
				const int center = ((min_a - mods[3] * mul) + (max_a - mods[7] * mul)) >> 1;
				const int b_lo = clamp_int(center - 3, 0, 255);
				const int b_hi = clamp_int(center + 3, 0, 255);

				for (int base = b_lo; base <= b_hi; base += base_step)
				{
					uint8_t recon[8];
					uint32_t err = 0;
					uint64_t idx_bits = 0;

					for (int m = 0; m < 8; m++)
						recon[m] = clamp_u8(base + mods[m] * mul);

					for (int p = 0; p < 16; p++)
					{
						const int target = block_a[p];
						uint32_t px_best = (uint32_t)((target - recon[0]) * (target - recon[0]));
						uint64_t px_idx = 0;

						for (uint64_t m = 1; m < 8; m++)
						{
							const int d = target - recon[m];
							const uint32_t e = (uint32_t)(d * d);
							if (e < px_best)
							{
								px_best = e;
								px_idx = m;
							}
						}

						err += px_best;
						if (err >= best_err)
							break;

						idx_bits |= (px_idx << ((15 - p) * 3));
					}

					if (err < best_err)
					{
						best_err = err;
						best_code = ((uint64_t)base << 56) |
						            ((uint64_t)mul << 52) |
						            ((uint64_t)tbl << 48) |
						            idx_bits;
						if (best_err == 0)
							return best_code;
					}
				}
			}
		}

		return best_code;
	}
}

/* ---- Image Extraction & Full-Surface Compression ---- */

static void extract_rgba_block(const uint8_t *rgba, int width, int height, int stride_bytes,
	int bx, int by, uint8_t out_rgb[16][3], uint8_t out_a[16])
{
	const int x0 = bx * 4;
	const int y0 = by * 4;

	for (int x = 0; x < 4; x++)
	{
		const int sx = (x0 + x < width) ? (x0 + x) : (width - 1);

		for (int y = 0; y < 4; y++)
		{
			const int sy = (y0 + y < height) ? (y0 + y) : (height - 1);
			const uint8_t *px = rgba + (size_t)sy * (size_t)stride_bytes + (size_t)sx * 4u;
			const int p = x * 4 + y;

			out_rgb[p][0] = px[0];
			out_rgb[p][1] = px[1];
			out_rgb[p][2] = px[2];
			if (out_a)
				out_a[p] = px[3];
		}
	}
}

/*
 * Weight transparent RGB texels toward the mean opaque RGB of the 4x4 block so
 * bilinear filtering across alpha edges never bleeds black halos.
 */
static void bleed_transparent_rgb(uint8_t block_rgb[16][3], const uint8_t block_a[16])
{
	int sum_r = 0, sum_g = 0, sum_b = 0, sum_w = 0;

	for (int p = 0; p < 16; p++)
	{
		const int w = block_a[p];
		if (w > 0)
		{
			sum_r += block_rgb[p][0] * w;
			sum_g += block_rgb[p][1] * w;
			sum_b += block_rgb[p][2] * w;
			sum_w += w;
		}
	}

	if (sum_w == 0)
		return;

	{
		const uint8_t avg_r = (uint8_t)((sum_r + (sum_w >> 1)) / sum_w);
		const uint8_t avg_g = (uint8_t)((sum_g + (sum_w >> 1)) / sum_w);
		const uint8_t avg_b = (uint8_t)((sum_b + (sum_w >> 1)) / sum_w);

		for (int p = 0; p < 16; p++)
		{
			if (block_a[p] == 0)
			{
				block_rgb[p][0] = avg_r;
				block_rgb[p][1] = avg_g;
				block_rgb[p][2] = avg_b;
			}
		}
	}
}

void etc2_compress_rgb8(const uint8_t *rgba, int width, int height, int stride_bytes,
	uint8_t *out_blocks, etc2_quality quality)
{
	const int bw = etc2_blocks_dim(width);
	const int bh = etc2_blocks_dim(height);

	if (!rgba || !out_blocks || bw <= 0 || bh <= 0)
		return;
	if (stride_bytes <= 0)
		stride_bytes = width * 4;

	for (int by = 0; by < bh; by++)
	{
		for (int bx = 0; bx < bw; bx++)
		{
			uint8_t block_rgb[16][3];
			uint8_t *dst = out_blocks + ((size_t)by * (size_t)bw + (size_t)bx) * 8u;

			extract_rgba_block(rgba, width, height, stride_bytes, bx, by, block_rgb, NULL);
			write_be64(dst, encode_etc2_rgb_block(block_rgb, quality));
		}
	}
}

void etc2_compress_rgba8(const uint8_t *rgba, int width, int height, int stride_bytes,
	uint8_t *out_blocks, etc2_quality quality)
{
	const int bw = etc2_blocks_dim(width);
	const int bh = etc2_blocks_dim(height);

	if (!rgba || !out_blocks || bw <= 0 || bh <= 0)
		return;
	if (stride_bytes <= 0)
		stride_bytes = width * 4;

	for (int by = 0; by < bh; by++)
	{
		for (int bx = 0; bx < bw; bx++)
		{
			uint8_t block_rgb[16][3];
			uint8_t block_a[16];
			uint8_t *dst = out_blocks + ((size_t)by * (size_t)bw + (size_t)bx) * 16u;

			extract_rgba_block(rgba, width, height, stride_bytes, bx, by, block_rgb, block_a);
			bleed_transparent_rgb(block_rgb, block_a);
			write_be64(dst, encode_eac_alpha_block(block_a, quality));
			write_be64(dst + 8, encode_etc2_rgb_block(block_rgb, quality));
		}
	}
}

/* ---- Real-Time Dirty-Block Streaming XRGB32 -> RGB8_ETC2 Compressor ---- */

static inline uint64_t hash_xrgb_block_4x4(const uint32_t *r0, const uint32_t *r1,
	const uint32_t *r2, const uint32_t *r3)
{
#if ETC2_HAS_NEON
	/* Mix 16 pixels (4x uint32x4_t) in 4 vector operations */
	const uint32x4_t m = vdupq_n_u32(0x00ffffffu);
	const uint32x4_t v0 = vandq_u32(vld1q_u32(r0), m);
	const uint32x4_t v1 = vandq_u32(vld1q_u32(r1), m);
	const uint32x4_t v2 = vandq_u32(vld1q_u32(r2), m);
	const uint32x4_t v3 = vandq_u32(vld1q_u32(r3), m);
	uint64x2_t h0 = vpaddlq_u32(veorq_u32(v0, vshlq_n_u32(v1, 7)));
	uint64x2_t h1 = vpaddlq_u32(veorq_u32(v2, vshlq_n_u32(v3, 13)));
	uint64x2_t sum = vaddq_u64(veorq_u64(h0, vshlq_n_u64(h1, 19)), h1);
	uint64_t lo = vgetq_lane_u64(sum, 0);
	uint64_t hi = vgetq_lane_u64(sum, 1);
	uint64_t h = (lo ^ (hi * 0x9e3779b97f4a7c15ULL)) * 0xbf58476d1ce4e5b9ULL;
	return h ^ (h >> 31);
#else
	uint64_t h = 0x9e3779b97f4a7c15ULL;
	const uint32_t *rows[4] = { r0, r1, r2, r3 };

	for (int y = 0; y < 4; y++)
	{
		const uint64_t pair0 = ((uint64_t)(rows[y][0] & 0x00ffffffu) << 32) | (rows[y][1] & 0x00ffffffu);
		const uint64_t pair1 = ((uint64_t)(rows[y][2] & 0x00ffffffu) << 32) | (rows[y][3] & 0x00ffffffu);
		h = (h ^ pair0) * 0xbf58476d1ce4e5b9ULL;
		h = (h ^ pair1) * 0x94d049bb133111ebULL;
	}
	return h ^ (h >> 32);
#endif
}

/*
 * Ultra-fast bounding-box luminance encoder for 60 FPS streaming.
 * Uses direct analytical table selection instead of exhaustive 8-table search.
 */
static uint64_t encode_fast_xrgb_block(const uint8_t block_rgb[16][3])
{
	int avg[2][3] = { { 0, 0, 0 }, { 0, 0, 0 } };
	int r0, g0, b0, r1, g1, b1;
	int base[2][3];
	uint32_t cw[2] = { 0, 0 };
	uint32_t bits = 0;
	bool use_diff = false;
	int dr = 0, dg = 0, db = 0;

	for (int s = 0; s < 2; s++)
	{
		const int base_p = s * 8;
		for (int i = 0; i < 8; i++)
		{
			avg[s][0] += block_rgb[base_p + i][0];
			avg[s][1] += block_rgb[base_p + i][1];
			avg[s][2] += block_rgb[base_p + i][2];
		}
		avg[s][0] = (avg[s][0] + 4) >> 3;
		avg[s][1] = (avg[s][1] + 4) >> 3;
		avg[s][2] = (avg[s][2] + 4) >> 3;
	}

	r0 = quant8to5(avg[0][0]);
	g0 = quant8to5(avg[0][1]);
	b0 = quant8to5(avg[0][2]);
	r1 = quant8to5(avg[1][0]);
	g1 = quant8to5(avg[1][1]);
	b1 = quant8to5(avg[1][2]);
	dr = r1 - r0;
	dg = g1 - g0;
	db = b1 - b0;

	if (dr >= -4 && dr <= 3 && dg >= -4 && dg <= 3 && db >= -4 && db <= 3)
	{
		use_diff = true;
		base[0][0] = expand5to8(r0);
		base[0][1] = expand5to8(g0);
		base[0][2] = expand5to8(b0);
		base[1][0] = expand5to8(r1);
		base[1][1] = expand5to8(g1);
		base[1][2] = expand5to8(b1);
	}
	else
	{
		r0 = quant8to4(avg[0][0]);
		g0 = quant8to4(avg[0][1]);
		b0 = quant8to4(avg[0][2]);
		r1 = quant8to4(avg[1][0]);
		g1 = quant8to4(avg[1][1]);
		b1 = quant8to4(avg[1][2]);
		base[0][0] = expand4to8(r0);
		base[0][1] = expand4to8(g0);
		base[0][2] = expand4to8(b0);
		base[1][0] = expand4to8(r1);
		base[1][1] = expand4to8(g1);
		base[1][2] = expand4to8(b1);
	}

	for (int s = 0; s < 2; s++)
	{
		const int base_lum = base[s][0] * 2 + base[s][1] * 5 + base[s][2];
		int max_dev = 0;
		int lum_delta[8];

		for (int i = 0; i < 8; i++)
		{
			const int p = s * 8 + i;
			const int lum = block_rgb[p][0] * 2 + block_rgb[p][1] * 5 + block_rgb[p][2];
			const int d = (lum - base_lum) >> 3;
			const int ad = d < 0 ? -d : d;

			lum_delta[i] = d;
			if (ad > max_dev)
				max_dev = ad;
		}

		/* Pick best-matching table directly from peak luminance excursion */
		uint32_t table = 0;
		if (max_dev > 130)      table = 7;
		else if (max_dev > 90)  table = 6;
		else if (max_dev > 68)  table = 5;
		else if (max_dev > 49)  table = 4;
		else if (max_dev > 34)  table = 3;
		else if (max_dev > 22)  table = 2;
		else if (max_dev > 11)  table = 1;
		cw[s] = table;

		{
			const int mid_thresh = (k_etc_mod_tables[table][0] + k_etc_mod_tables[table][1]) >> 1;

			for (int i = 0; i < 8; i++)
			{
				const uint32_t p = (uint32_t)(s * 8 + i);
				const int d = lum_delta[i];

				if (d < 0)
				{
					bits |= (1u << (16u + p));
					if (-d >= mid_thresh)
						bits |= (1u << p);
				}
				else if (d >= mid_thresh)
				{
					bits |= (1u << p);
				}
			}
		}
	}

	if (use_diff)
	{
		return ((uint64_t)(r0 & 0x1f) << 59) |
		       ((uint64_t)(dr & 0x07) << 56) |
		       ((uint64_t)(g0 & 0x1f) << 51) |
		       ((uint64_t)(dg & 0x07) << 48) |
		       ((uint64_t)(b0 & 0x1f) << 43) |
		       ((uint64_t)(db & 0x07) << 40) |
		       ((uint64_t)cw[0] << 37) |
		       ((uint64_t)cw[1] << 34) |
		       (1ULL << 33) |
		       (uint64_t)bits;
	}

	return ((uint64_t)(r0 & 0x0f) << 60) |
	       ((uint64_t)(r1 & 0x0f) << 56) |
	       ((uint64_t)(g0 & 0x0f) << 52) |
	       ((uint64_t)(g1 & 0x0f) << 48) |
	       ((uint64_t)(b0 & 0x0f) << 44) |
	       ((uint64_t)(b1 & 0x0f) << 40) |
	       ((uint64_t)cw[0] << 37) |
	       ((uint64_t)cw[1] << 34) |
	       (uint64_t)bits;
}

int etc2_compress_xrgb32_stream_dirty(const uint32_t *xrgb_pixels, int width, int height,
	int stride_pixels, uint8_t *out_blocks, uint64_t *block_hashes,
	int *out_dirty_y0, int *out_dirty_y1)
{
	const int bw = etc2_blocks_dim(width);
	const int bh = etc2_blocks_dim(height);
	int dirty_by0 = bh;
	int dirty_by1 = -1;
	int encoded_count = 0;

	if (!xrgb_pixels || !out_blocks || bw <= 0 || bh <= 0)
	{
		if (out_dirty_y0) *out_dirty_y0 = 0;
		if (out_dirty_y1) *out_dirty_y1 = 0;
		return 0;
	}
	if (stride_pixels <= 0)
		stride_pixels = width;

	for (int by = 0; by < bh; by++)
	{
		const int y0 = by * 4;
		const bool full_y = (y0 + 4 <= height);
		const uint32_t *r0 = xrgb_pixels + (size_t)y0 * (size_t)stride_pixels;
		const uint32_t *r1 = xrgb_pixels + (size_t)(y0 + 1 < height ? y0 + 1 : height - 1) * (size_t)stride_pixels;
		const uint32_t *r2 = xrgb_pixels + (size_t)(y0 + 2 < height ? y0 + 2 : height - 1) * (size_t)stride_pixels;
		const uint32_t *r3 = xrgb_pixels + (size_t)(y0 + 3 < height ? y0 + 3 : height - 1) * (size_t)stride_pixels;

		for (int bx = 0; bx < bw; bx++)
		{
			const int x0 = bx * 4;
			const size_t bidx = (size_t)by * (size_t)bw + (size_t)bx;
			uint8_t block_rgb[16][3];

			if (full_y && (x0 + 4 <= width) && block_hashes)
			{
				uint64_t h = hash_xrgb_block_4x4(r0 + x0, r1 + x0, r2 + x0, r3 + x0);
				if (h == ~0ULL)
					h = ~1ULL;
				if (block_hashes[bidx] == h)
					continue;
				block_hashes[bidx] = h;
			}

			for (int x = 0; x < 4; x++)
			{
				const int sx = (x0 + x < width) ? (x0 + x) : (width - 1);
				const uint32_t c0 = r0[sx];
				const uint32_t c1 = r1[sx];
				const uint32_t c2 = r2[sx];
				const uint32_t c3 = r3[sx];
				const int p = x * 4;

				block_rgb[p + 0][0] = (uint8_t)(c0 >> 16);
				block_rgb[p + 0][1] = (uint8_t)(c0 >> 8);
				block_rgb[p + 0][2] = (uint8_t)(c0);

				block_rgb[p + 1][0] = (uint8_t)(c1 >> 16);
				block_rgb[p + 1][1] = (uint8_t)(c1 >> 8);
				block_rgb[p + 1][2] = (uint8_t)(c1);

				block_rgb[p + 2][0] = (uint8_t)(c2 >> 16);
				block_rgb[p + 2][1] = (uint8_t)(c2 >> 8);
				block_rgb[p + 2][2] = (uint8_t)(c2);

				block_rgb[p + 3][0] = (uint8_t)(c3 >> 16);
				block_rgb[p + 3][1] = (uint8_t)(c3 >> 8);
				block_rgb[p + 3][2] = (uint8_t)(c3);
			}

			write_be64(out_blocks + bidx * 8u, encode_fast_xrgb_block(block_rgb));
			encoded_count++;

			if (by < dirty_by0)
				dirty_by0 = by;
			if (by > dirty_by1)
				dirty_by1 = by;
		}
	}

	if (dirty_by1 < dirty_by0)
	{
		if (out_dirty_y0) *out_dirty_y0 = 0;
		if (out_dirty_y1) *out_dirty_y1 = 0;
	}
	else
	{
		if (out_dirty_y0) *out_dirty_y0 = dirty_by0 * 4;
		if (out_dirty_y1) *out_dirty_y1 = (dirty_by1 + 1) * 4;
	}
	return encoded_count;
}

/* ---- Spec-Compliant Reference Software Decoder (ETC2 RGB8 + EAC Alpha) ---- */

static inline int sign_extend_3bit(uint32_t v)
{
	return (v & 4u) ? ((int)(v & 7u) - 8) : (int)(v & 7u);
}

static void decode_etc2_rgb_block(uint64_t code, uint8_t out_rgb[16][3])
{
	const bool flip = ((code >> 32) & 1u) != 0;
	const bool diff = ((code >> 33) & 1u) != 0;

	if (diff)
	{
		const int r0_5 = (int)((code >> 59) & 0x1fu);
		const int dr_3 = sign_extend_3bit((uint32_t)(code >> 56));
		const int r1_5 = r0_5 + dr_3;

		if (r1_5 < 0 || r1_5 > 31)
		{
			/* T-mode */
			const int r0 = (int)(((code >> 57) & 0x0cu) | ((code >> 56) & 0x03u));
			const int g0 = (int)((code >> 52) & 0x0fu);
			const int b0 = (int)((code >> 48) & 0x0fu);
			const int r2 = (int)((code >> 44) & 0x0fu);
			const int g2 = (int)((code >> 40) & 0x0fu);
			const int b2 = (int)((code >> 36) & 0x0fu);
			const int da = (int)(((code >> 33) & 0x06u) | ((code >> 32) & 0x01u));
			const int delta = k_etc2_th_dist[da];
			uint8_t pal[4][3];

			pal[0][0] = expand4to8(r0);
			pal[0][1] = expand4to8(g0);
			pal[0][2] = expand4to8(b0);
			pal[2][0] = expand4to8(r2);
			pal[2][1] = expand4to8(g2);
			pal[2][2] = expand4to8(b2);
			pal[1][0] = clamp_u8(pal[2][0] + delta);
			pal[1][1] = clamp_u8(pal[2][1] + delta);
			pal[1][2] = clamp_u8(pal[2][2] + delta);
			pal[3][0] = clamp_u8(pal[2][0] - delta);
			pal[3][1] = clamp_u8(pal[2][1] - delta);
			pal[3][2] = clamp_u8(pal[2][2] - delta);

			for (int p = 0; p < 16; p++)
			{
				const uint32_t idx = (uint32_t)(((code >> p) & 1u) | (((code >> (16 + p)) & 1u) << 1));
				out_rgb[p][0] = pal[idx][0];
				out_rgb[p][1] = pal[idx][1];
				out_rgb[p][2] = pal[idx][2];
			}
			return;
		}

		{
			const int g0_5 = (int)((code >> 51) & 0x1fu);
			const int dg_3 = sign_extend_3bit((uint32_t)(code >> 48));
			const int g1_5 = g0_5 + dg_3;

			if (g1_5 < 0 || g1_5 > 31)
			{
				/* H-mode */
				const int r0 = (int)((code >> 59) & 0x0fu);
				const int g0 = (int)(((code >> 55) & 0x0eu) | ((code >> 52) & 0x01u));
				const int b0 = (int)(((code >> 48) & 0x08u) | ((code >> 47) & 0x07u));
				const int r2 = (int)((code >> 43) & 0x0fu);
				const int g2 = (int)((code >> 39) & 0x0fu);
				const int b2 = (int)((code >> 35) & 0x0fu);
				const uint8_t c0[3] = { expand4to8(r0), expand4to8(g0), expand4to8(b0) };
				const uint8_t c2[3] = { expand4to8(r2), expand4to8(g2), expand4to8(b2) };
				const uint32_t v0 = ((uint32_t)c0[0] << 16) | ((uint32_t)c0[1] << 8) | c0[2];
				const uint32_t v2 = ((uint32_t)c2[0] << 16) | ((uint32_t)c2[1] << 8) | c2[2];
				const int da = (int)(((code >> 32) & 0x04u) | ((code >> 31) & 0x02u) | (v0 >= v2 ? 1u : 0u));
				const int delta = k_etc2_th_dist[da];
				uint8_t pal[4][3];

				pal[0][0] = clamp_u8(c0[0] + delta);
				pal[0][1] = clamp_u8(c0[1] + delta);
				pal[0][2] = clamp_u8(c0[2] + delta);
				pal[1][0] = clamp_u8(c0[0] - delta);
				pal[1][1] = clamp_u8(c0[1] - delta);
				pal[1][2] = clamp_u8(c0[2] - delta);
				pal[2][0] = clamp_u8(c2[0] + delta);
				pal[2][1] = clamp_u8(c2[1] + delta);
				pal[2][2] = clamp_u8(c2[2] + delta);
				pal[3][0] = clamp_u8(c2[0] - delta);
				pal[3][1] = clamp_u8(c2[1] - delta);
				pal[3][2] = clamp_u8(c2[2] - delta);

				for (int p = 0; p < 16; p++)
				{
					const uint32_t idx = (uint32_t)(((code >> p) & 1u) | (((code >> (16 + p)) & 1u) << 1));
					out_rgb[p][0] = pal[idx][0];
					out_rgb[p][1] = pal[idx][1];
					out_rgb[p][2] = pal[idx][2];
				}
				return;
			}

			{
				const int b0_5 = (int)((code >> 43) & 0x1fu);
				const int db_3 = sign_extend_3bit((uint32_t)(code >> 40));
				const int b1_5 = b0_5 + db_3;

				if (b1_5 < 0 || b1_5 > 31)
				{
					/* Planar mode */
					const int ro = expand6to8((int)((code >> 57) & 0x3fu));
					const int go = expand7to8((int)(((code >> 50) & 0x40u) | ((code >> 49) & 0x3fu)));
					const int bo = expand6to8((int)(((code >> 43) & 0x20u) | ((code >> 40) & 0x18u) | ((code >> 39) & 0x07u)));
					const int rh = expand6to8((int)(((code >> 33) & 0x3eu) | ((code >> 32) & 0x01u)));
					const int gh = expand7to8((int)((code >> 25) & 0x7fu));
					const int bh = expand6to8((int)((code >> 19) & 0x3fu));
					const int rv = expand6to8((int)((code >> 13) & 0x3fu));
					const int gv = expand7to8((int)((code >> 6) & 0x7fu));
					const int bv = expand6to8((int)(code & 0x3fu));

					for (int x = 0; x < 4; x++)
					{
						for (int y = 0; y < 4; y++)
						{
							const int p = x * 4 + y;
							out_rgb[p][0] = clamp_u8((x * (rh - ro) + y * (rv - ro) + 4 * ro + 2) >> 2);
							out_rgb[p][1] = clamp_u8((x * (gh - go) + y * (gv - go) + 4 * go + 2) >> 2);
							out_rgb[p][2] = clamp_u8((x * (bh - bo) + y * (bv - bo) + 4 * bo + 2) >> 2);
						}
					}
					return;
				}

				/* Standard Differential mode */
				{
					const int base[2][3] = {
						{ expand5to8(r0_5), expand5to8(g0_5), expand5to8(b0_5) },
						{ expand5to8(r1_5), expand5to8(g1_5), expand5to8(b1_5) }
					};
					const uint32_t cw[2] = {
						(uint32_t)((code >> 37) & 7u),
						(uint32_t)((code >> 34) & 7u)
					};

					for (int s = 0; s < 2; s++)
					{
						const uint8_t *idxs = k_subblock_pixels[flip ? 1 : 0][s];
						const int16_t *mods = k_etc_mod_tables[cw[s]];

						for (int i = 0; i < 8; i++)
						{
							const uint8_t p = idxs[i];
							const uint32_t m = (uint32_t)(((code >> p) & 1u) | (((code >> (16 + p)) & 1u) << 1));
							out_rgb[p][0] = clamp_u8(base[s][0] + mods[m]);
							out_rgb[p][1] = clamp_u8(base[s][1] + mods[m]);
							out_rgb[p][2] = clamp_u8(base[s][2] + mods[m]);
						}
					}
					return;
				}
			}
		}
	}

	/* Individual mode (444 + 444) */
	{
		const int base[2][3] = {
			{
				expand4to8((int)((code >> 60) & 0x0fu)),
				expand4to8((int)((code >> 52) & 0x0fu)),
				expand4to8((int)((code >> 44) & 0x0fu))
			},
			{
				expand4to8((int)((code >> 56) & 0x0fu)),
				expand4to8((int)((code >> 48) & 0x0fu)),
				expand4to8((int)((code >> 40) & 0x0fu))
			}
		};
		const uint32_t cw[2] = {
			(uint32_t)((code >> 37) & 7u),
			(uint32_t)((code >> 34) & 7u)
		};

		for (int s = 0; s < 2; s++)
		{
			const uint8_t *idxs = k_subblock_pixels[flip ? 1 : 0][s];
			const int16_t *mods = k_etc_mod_tables[cw[s]];

			for (int i = 0; i < 8; i++)
			{
				const uint8_t p = idxs[i];
				const uint32_t m = (uint32_t)(((code >> p) & 1u) | (((code >> (16 + p)) & 1u) << 1));
				out_rgb[p][0] = clamp_u8(base[s][0] + mods[m]);
				out_rgb[p][1] = clamp_u8(base[s][1] + mods[m]);
				out_rgb[p][2] = clamp_u8(base[s][2] + mods[m]);
			}
		}
	}
}

static void decode_eac_alpha_block(uint64_t code, uint8_t out_a[16])
{
	const int base = (int)((code >> 56) & 0xffu);
	const int mul = (int)((code >> 52) & 0x0fu);
	const int tbl = (int)((code >> 48) & 0x0fu);
	const int8_t *mods = k_eac_alpha_tables[tbl];

	for (int p = 0; p < 16; p++)
	{
		const uint32_t idx = (uint32_t)((code >> ((15 - p) * 3)) & 7u);
		out_a[p] = clamp_u8(base + mul * mods[idx]);
	}
}

void etc2_decompress_rgb8(const uint8_t *blocks, int width, int height,
	uint8_t *out_rgba, int stride_bytes)
{
	const int bw = etc2_blocks_dim(width);
	const int bh = etc2_blocks_dim(height);

	if (!blocks || !out_rgba || bw <= 0 || bh <= 0)
		return;
	if (stride_bytes <= 0)
		stride_bytes = width * 4;

	for (int by = 0; by < bh; by++)
	{
		for (int bx = 0; bx < bw; bx++)
		{
			uint8_t block_rgb[16][3];
			const uint8_t *src = blocks + ((size_t)by * (size_t)bw + (size_t)bx) * 8u;

			decode_etc2_rgb_block(read_be64(src), block_rgb);

			for (int x = 0; x < 4 && bx * 4 + x < width; x++)
			{
				for (int y = 0; y < 4 && by * 4 + y < height; y++)
				{
					uint8_t *dst = out_rgba + (size_t)(by * 4 + y) * (size_t)stride_bytes + (size_t)(bx * 4 + x) * 4u;
					const int p = x * 4 + y;

					dst[0] = block_rgb[p][0];
					dst[1] = block_rgb[p][1];
					dst[2] = block_rgb[p][2];
					dst[3] = 255;
				}
			}
		}
	}
}

void etc2_decompress_rgba8(const uint8_t *blocks, int width, int height,
	uint8_t *out_rgba, int stride_bytes)
{
	const int bw = etc2_blocks_dim(width);
	const int bh = etc2_blocks_dim(height);

	if (!blocks || !out_rgba || bw <= 0 || bh <= 0)
		return;
	if (stride_bytes <= 0)
		stride_bytes = width * 4;

	for (int by = 0; by < bh; by++)
	{
		for (int bx = 0; bx < bw; bx++)
		{
			uint8_t block_rgb[16][3];
			uint8_t block_a[16];
			const uint8_t *src = blocks + ((size_t)by * (size_t)bw + (size_t)bx) * 16u;

			decode_eac_alpha_block(read_be64(src), block_a);
			decode_etc2_rgb_block(read_be64(src + 8), block_rgb);

			for (int x = 0; x < 4 && bx * 4 + x < width; x++)
			{
				for (int y = 0; y < 4 && by * 4 + y < height; y++)
				{
					uint8_t *dst = out_rgba + (size_t)(by * 4 + y) * (size_t)stride_bytes + (size_t)(bx * 4 + x) * 4u;
					const int p = x * 4 + y;

					dst[0] = block_rgb[p][0];
					dst[1] = block_rgb[p][1];
					dst[2] = block_rgb[p][2];
					dst[3] = block_a[p];
				}
			}
		}
	}
}

/* ---- Disk Cache (.etc2) Serialization ---- */

#define ETC2_CACHE_MAGIC   0x32435445u /* "ETC2" little-endian */
#define ETC2_CACHE_VERSION 1u

typedef struct etc2_cache_header
{
	uint32_t magic;
	uint32_t version;
	uint32_t gl_format;
	uint32_t width;
	uint32_t height;
	uint32_t data_size;
	uint32_t source_crc32;
	uint32_t payload_crc32;
} etc2_cache_header;

uint8_t *etc2_cache_load(const char *cache_path, uint32_t expected_gl_format,
	int expected_width, int expected_height, uint32_t expected_source_crc32,
	size_t *out_size)
{
	FILE *fp;
	etc2_cache_header hdr;
	uint8_t *buf;

	if (out_size)
		*out_size = 0;
	if (!cache_path || expected_width <= 0 || expected_height <= 0)
		return NULL;

	fp = fopen(cache_path, "rb");
	if (!fp)
		return NULL;

	if (fread(&hdr, sizeof(hdr), 1, fp) != 1 ||
		hdr.magic != ETC2_CACHE_MAGIC ||
		hdr.version != ETC2_CACHE_VERSION ||
		hdr.gl_format != expected_gl_format ||
		hdr.width != (uint32_t)expected_width ||
		hdr.height != (uint32_t)expected_height ||
		hdr.source_crc32 != expected_source_crc32 ||
		hdr.data_size == 0 || hdr.data_size > (64u * 1024u * 1024u))
	{
		fclose(fp);
		return NULL;
	}

	buf = (uint8_t *)malloc(hdr.data_size);
	if (!buf)
	{
		fclose(fp);
		return NULL;
	}

	if (fread(buf, 1, hdr.data_size, fp) != hdr.data_size ||
		etc2_crc32(buf, hdr.data_size) != hdr.payload_crc32)
	{
		free(buf);
		fclose(fp);
		return NULL;
	}

	fclose(fp);
	if (out_size)
		*out_size = hdr.data_size;
	return buf;
}

bool etc2_cache_save(const char *cache_path, uint32_t gl_format,
	int width, int height, uint32_t source_crc32,
	const uint8_t *data, size_t data_size)
{
	FILE *fp;
	etc2_cache_header hdr;
	bool ok;

	if (!cache_path || !data || data_size == 0 || data_size > 0xffffffffu || width <= 0 || height <= 0)
		return false;

	hdr.magic = ETC2_CACHE_MAGIC;
	hdr.version = ETC2_CACHE_VERSION;
	hdr.gl_format = gl_format;
	hdr.width = (uint32_t)width;
	hdr.height = (uint32_t)height;
	hdr.data_size = (uint32_t)data_size;
	hdr.source_crc32 = source_crc32;
	hdr.payload_crc32 = etc2_crc32(data, data_size);

	fp = fopen(cache_path, "wb");
	if (!fp)
		return false;

	ok = (fwrite(&hdr, sizeof(hdr), 1, fp) == 1) &&
	     (fwrite(data, 1, data_size, fp) == data_size);
	if (fclose(fp) != 0)
		ok = false;
	return ok;
}
