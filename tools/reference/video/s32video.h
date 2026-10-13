/**
 *    Name:        s32video.h
 *    Description: reference System 32 video: MAME's segas32_v.cpp drawing the board model's frames
 *    Version:     0.1.0
 *    Created:     13-10-2026
 *    Modified:    13-10-2026
 *    Author:      Sebastian Toyos
 *
 *    license:BSD-3-Clause
 *    A development tool, never part of the game: the frames the game's own video library has to match.
 *    s32v_attach() hooks it to the board (s32_board.frame_shown and sprites_due): the sprite list is
 *    drawn when the board sends it, and each frame when VBLANK starts, from the memories as they are.
 */
#ifndef S32_REFERENCE_VIDEO_H
#define S32_REFERENCE_VIDEO_H

#include <stddef.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

#define S32V_MAX_WIDTH  416
#define S32V_HEIGHT     224

/* The tile ROM (2 MB) and the sprite ROMs (16 MB, every 32-bit word big endian) as tools/build_rom.py
   writes them; the reference keeps pointers to them */
void s32v_attach(const uint8_t *tiles, size_t tiles_size, const uint8_t *sprites, size_t sprites_size);

/* The last frame drawn: 0x00RRGGBB pixels, S32V_MAX_WIDTH a row; its width is 320 or 416 */
const uint32_t *s32v_frame(int *width);

/* FNV-1a hash of the last frame's visible pixels */
uint64_t s32v_frame_hash(void);

#ifdef __cplusplus
}
#endif

#endif
