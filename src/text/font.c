/**
 *    Name:        text/font.c
 *    Description: the text layer's font: load_text_font, clear_text_layer, the 1-bit (and unused 2-bit) font expansion
 *    Version:     0.1.0
 *    Created:     12-10-2026
 *    Modified:    15-10-2026
 *    Author:      Sebastian Toyos
 *
 *    SegaSonic The Hedgehog (arcade, Sega System 32): the game's V60 program decompiled into C by
 *    tools/decompile.py, with the names of symbols.txt.
 */
#include "game.h"

void load_text_font(uint32_t *p_pixels, uint32_t *p_color_index)
{
	uint8_t color = 0;
	uint16_t count = 0;
	uint32_t r4 = 0, r5 = 0, dest = 0, dest_end = 0, ignored = 0, source = 0, source_end = 0;

	mem_write16(video_text_control, 0x1F7u);
	move_string(text_font_ADDRESS, 0x9A0u, VIDEO_RAM + 0x1C000u, 0x9A0u, 2u, 0u, 0u, fill_value, &dest_end, &source_end);
	source = source_end;
	dest = dest_end;
	count = 0x5Bu;
	color = 1u;
	expand_font(color, count, source, dest, &r4, &r5);
	clear_text_layer(&ignored, &ignored, &ignored);
	*p_pixels = r4;
	*p_color_index = r5;
}

void clear_text_layer(uint32_t *p_value, uint32_t *p_count, uint32_t *p_dest)
{
	uint32_t r0 = 0, count = 0, dest = 0;

	dest = TEXT_CELL(0, 0);
	count = 0xE0u;
	clear_blocks(&r0, &count, &dest);
	*p_value = r0;
	*p_count = count;
	*p_dest = dest;
}

void expand_font(uint8_t color, uint32_t count, uint32_t source, uint32_t dest, uint32_t *p_pixels, uint32_t *p_color_index)
{
	uint32_t carry = 0;
	uint8_t font_row = 0;
	uint32_t char_rows_left = 0, color_index = 0, pairs_left = 0, pixel_mask = 0, pixels = 0, set_pixels = 0, shadow_pixels = 0;

	color_index = (uint8_t)color & 0xFu;
	count = (uint16_t)count;
	do
	{
		char_rows_left = 8u;
		shadow_pixels = 0u;
		do
		{
			font_row = mem_read8(source);
			pairs_left = 4u;
			pixels = 0u;
			source++;
			do
			{
				carry = (uint8_t)(font_row * 2u) < font_row;
				font_row = (uint8_t)(font_row * 2u);
				pixels = rotate_with_carry_by(pixels, 4u, carry, 32);
				carry = (uint8_t)(font_row * 2u) < font_row;
				font_row = (uint8_t)(font_row * 2u);
				pixels = rotate_with_carry_by(pixels, 4u, carry, 32);
				pairs_left--;
			} while (pairs_left != 0u);
			pixels = (pixels << 29) | (pixels >> 3);
			set_pixels = pixels;
			pixel_mask = pixels * 0xFu;
			pixels = pixels * color_index;
			mem_write32(dest, byte_swap32((shadow_pixels & ~pixel_mask) | pixels));
			shadow_pixels = (pixels + set_pixels) >> 4;
			char_rows_left--;
			dest += 4u;
		} while (char_rows_left != 0u);
		count--;
	} while (count != 0u);
	*p_pixels = pixels;
	*p_color_index = color_index;
}

void expand_font_2bpp(uint8_t color, uint32_t count, uint32_t source, uint32_t dest)
{
	uint32_t carry = 0;
	uint16_t font_row = 0;
	uint32_t color_nibbles = 0, pairs_left = 0, pixels = 0;

	color_nibbles = ((uint8_t)color & 0xFu) * 0x11111111u;
	count = (uint32_t)(uint16_t)count << 3;
	do
	{
		font_row = mem_read16(source);
		pairs_left = 4u;
		pixels = 0u;
		source += 2u;
		do
		{
			carry = (uint16_t)(font_row * 2u) < font_row;
			font_row = (uint16_t)(font_row * 2u);
			pixels = pixels * 2u + carry;
			carry = (uint16_t)(font_row * 2u) < font_row;
			font_row = (uint16_t)(font_row * 2u);
			pixels = rotate_with_carry_by(pixels, 3u, carry, 32);
			carry = (uint16_t)(font_row * 2u) < font_row;
			font_row = (uint16_t)(font_row * 2u);
			pixels = pixels * 2u + carry;
			carry = (uint16_t)(font_row * 2u) < font_row;
			font_row = (uint16_t)(font_row * 2u);
			pixels = rotate_with_carry_by(pixels, 3u, carry, 32);
			pairs_left--;
		} while (pairs_left != 0u);
		mem_write32(dest, byte_swap32(((pixels << 30) | (pixels >> 2)) + color_nibbles));
		count--;
		dest += 4u;
	} while (count != 0u);
}

void expand_font_2bpp_transparent(uint8_t color, uint32_t count, uint32_t pixels, uint32_t opaque_mask, uint32_t source, uint32_t dest)
{
	uint8_t pixel = 0;
	uint16_t font_row = 0;
	uint32_t color_nibbles = 0, pairs_left = 0;

	color_nibbles = ((uint8_t)color & 0xFu) * 0x11111111u;
	count = (uint32_t)(uint16_t)count << 3;
	do
	{
		font_row = mem_read16(source);
		pairs_left = 4u;
		source += 2u;
		do
		{
			font_row = (uint16_t)(((uint32_t)font_row << 2) | (font_row >> 14));
			pixel = font_row & 3u;
			pixels = pixels << 4;
			pixels = (pixels & ~0xFFu) | (uint8_t)(pixels | pixel);
			opaque_mask = opaque_mask << 4;
			opaque_mask = (opaque_mask & ~0xFFu) | (uint8_t)(opaque_mask | (pixel != 0u));
			font_row = (uint16_t)(((uint32_t)font_row << 2) | (font_row >> 14));
			pixel = font_row & 3u;
			pixels = pixels << 4;
			pixels = (pixels & ~0xFFu) | (uint8_t)(pixels | pixel);
			opaque_mask = opaque_mask << 4;
			opaque_mask = (opaque_mask & ~0xFFu) | (uint8_t)(opaque_mask | (pixel != 0u));
			pairs_left--;
		} while (pairs_left != 0u);
		opaque_mask = opaque_mask * 0xFu;
		pixels = (pixels + color_nibbles) & opaque_mask;
		mem_write32(dest, byte_swap32(pixels));
		count--;
		dest += 4u;
	} while (count != 0u);
}
