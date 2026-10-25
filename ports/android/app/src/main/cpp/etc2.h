/*
 * SegaSonic The Hedgehog — Android Port
 * Khronos OpenGL ES 3.0 ETC2 / EAC Texture Compression & Caching Module
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 * Implements standard OpenGL ES 3.0 / Khronos Data Format 1.3 compressed
 * texture formats:
 *   - GL_COMPRESSED_RGB8_ETC2        (0x9274, 8 bytes per 4x4 block, 4 bpp)
 *   - GL_COMPRESSED_RGBA8_ETC2_EAC   (0x9278, 16 bytes per 4x4 block, 8 bpp)
 *
 * Includes:
 *   1. Multi-mode high-quality offline/startup encoder (Planar, T-mode,
 *      Differential, Individual + 8-bit EAC Alpha) with CRC32-validated
 *      on-disk `.etc2` cache for zero-cost warm boots.
 *   2. Real-time dirty-block RGB8_ETC2 streaming frame encoder with ARM NEON
 *      acceleration for optional compressed 60 FPS framebuffer upload.
 *   3. Spec-compliant software reference decoder for verification & fallback.
 */

#ifndef SEGASONIC_ANDROID_ETC2_H
#define SEGASONIC_ANDROID_ETC2_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/* OpenGL ES 3.0 internalFormat constants for glCompressedTexImage2D */
#define ETC2_GL_COMPRESSED_RGB8_ETC2                     0x9274
#define ETC2_GL_COMPRESSED_SRGB8_ETC2                    0x9275
#define ETC2_GL_COMPRESSED_RGB8_PUNCHTHROUGH_ALPHA1_ETC2 0x9276
#define ETC2_GL_COMPRESSED_RGBA8_ETC2_EAC                0x9278
#define ETC2_GL_COMPRESSED_SRGB8_ALPHA8_ETC2_EAC         0x9279

typedef enum etc2_quality
{
	ETC2_QUALITY_FAST = 0,     /* Real-time / low-latency mode */
	ETC2_QUALITY_BALANCED = 1, /* Differential + Individual + Planar */
	ETC2_QUALITY_HIGH = 2      /* Full Planar + T-mode + Diff + Indiv + EAC search */
} etc2_quality;

/* Number of 4x4 blocks covering width or height */
static inline int etc2_blocks_dim(int dim)
{
	return (dim > 0) ? ((dim + 3) >> 2) : 0;
}

/* Total compressed payload size in bytes for GL_COMPRESSED_RGB8_ETC2 */
size_t etc2_rgb8_data_size(int width, int height);

/* Total compressed payload size in bytes for GL_COMPRESSED_RGBA8_ETC2_EAC */
size_t etc2_rgba8_data_size(int width, int height);

/* Standard IEEE 802.3 CRC32 helper used for .etc2 cache invalidation */
uint32_t etc2_crc32(const void *data, size_t size);

/*
 * Compress an RGBA8888 image (byte order R, G, B, A per pixel, stride in bytes;
 * if stride_bytes == 0, width * 4 is used) into GL_COMPRESSED_RGB8_ETC2 (8B/block).
 */
void etc2_compress_rgb8(const uint8_t *rgba, int width, int height, int stride_bytes,
	uint8_t *out_blocks, etc2_quality quality);

/*
 * Compress an RGBA8888 image (byte order R, G, B, A per pixel) into
 * GL_COMPRESSED_RGBA8_ETC2_EAC (16B/block: 8B EAC alpha + 8B ETC2 RGB).
 */
void etc2_compress_rgba8(const uint8_t *rgba, int width, int height, int stride_bytes,
	uint8_t *out_blocks, etc2_quality quality);

/*
 * Real-time dirty-block streaming compressor for System 32 ARGB8888 (0x00RRGGBB)
 * game frames into GL_COMPRESSED_RGB8_ETC2.
 *
 * `block_hashes` holds `etc2_blocks_dim(width) * etc2_blocks_dim(height)` uint64_t
 * entries initialized to ~0ULL before the first frame (or on texture recreate).
 * Unchanged 4x4 blocks are skipped in O(1); only modified blocks are encoded.
 *
 * Writes dirty scanline range `[out_dirty_y0, out_dirty_y1)` (multiples of 4,
 * clamped to padded height) and returns the number of re-encoded 4x4 blocks.
 */
int etc2_compress_xrgb32_stream_dirty(const uint32_t *xrgb_pixels, int width, int height,
	int stride_pixels, uint8_t *out_blocks, uint64_t *block_hashes,
	int *out_dirty_y0, int *out_dirty_y1);

/*
 * Spec-compliant software decoders into RGBA8888 (byte order R, G, B, A).
 * Primarily used for validation, diagnostics, and GLES2 tooling.
 */
void etc2_decompress_rgb8(const uint8_t *blocks, int width, int height,
	uint8_t *out_rgba, int stride_bytes);

void etc2_decompress_rgba8(const uint8_t *blocks, int width, int height,
	uint8_t *out_rgba, int stride_bytes);

/*
 * Disk cache helpers: store/load pre-compressed ETC2/EAC payloads on Android
 * internal cache/files storage so static atlases compress only once.
 * Caller frees the returned buffer from `etc2_cache_load` with `free()`.
 */
uint8_t *etc2_cache_load(const char *cache_path, uint32_t expected_gl_format,
	int expected_width, int expected_height, uint32_t expected_source_crc32,
	size_t *out_size);

bool etc2_cache_save(const char *cache_path, uint32_t gl_format,
	int width, int height, uint32_t source_crc32,
	const uint8_t *data, size_t data_size);

#ifdef __cplusplus
}
#endif

#endif /* SEGASONIC_ANDROID_ETC2_H */
