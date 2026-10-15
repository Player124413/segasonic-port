/**
 *    Name:        video/packed_picture.c
 *    Description: unused: a packed picture unpacked into the bitmap layer, an LZ unpacker
 *    Version:     0.1.0
 *    Created:     12-10-2026
 *    Modified:    15-10-2026
 *    Author:      Sebastian Toyos
 *
 *    SegaSonic The Hedgehog (arcade, Sega System 32): the game's V60 program decompiled into C by
 *    tools/decompile.py, with the names of symbols.txt.
 */
#include "game.h"

void draw_packed_picture(uint32_t picture)
{
	uint8_t is_8bpp = 0;
	uint32_t colour_cache = 0, done_bits = 0, newest_slot = 0, stream = 0, width = 0;

	stream = picture;
	is_8bpp = 1u;
	init_bitmap_layer(is_8bpp);
	mem_write16(video_layer_disable, mem_read16(video_layer_disable) | 0xFu);
	init_picture_unpacker(&colour_cache, &newest_slot, &done_bits);
	init_picture_colour_cache(colour_cache);
	width = read_picture_size(stream);
	unpack_picture_pixels(stream, width, colour_cache, newest_slot, done_bits);
	rotate_bitmap_for_monitor_flip();
}

void init_picture_unpacker(uint32_t *p_colour_cache, uint32_t *p_newest_slot, uint32_t *p_done_bits)
{
	uint32_t colour_cache = 0, done_bits = 0, newest_slot = 0;

	colour_cache = bitmap_draw_buffer_ADDRESS;
	newest_slot = 0u;
	done_bits = SPRITE_RAM + 0x100u;
	current_player = 0u;
	fill_value = 0u;
	set_halfwords(SPRITE_RAM + 0x100u, 0x1C00u, fill_value);
	*p_colour_cache = colour_cache;
	*p_newest_slot = newest_slot;
	*p_done_bits = done_bits;
}

void init_picture_colour_cache(uint32_t colour_cache)
{
	uint8_t next_slot = 0, previous_slot = 0;
	uint32_t entry_cursor = 0, slots_left = 0;

	entry_cursor = colour_cache;
	slots_left = 0x10u;
	next_slot = 1u;
	previous_slot = 0xFFu;
	do
	{
		mem_write8(entry_cursor, next_slot);
		entry_cursor++;
		mem_write8(entry_cursor, previous_slot);
		entry_cursor++;
		mem_write8(entry_cursor, 0u);
		next_slot++;
		previous_slot++;
		slots_left--;
		entry_cursor++;
	} while (slots_left != 0u);
	mem_write8(colour_cache + 0x2Du, 0u);
	mem_write8(colour_cache + 1u, 0xFu);
}

uint32_t read_picture_size(uint32_t stream)
{
	uint32_t width = 0;

	width = shr32(mem_read32(stream + (current_player >> 3)), (int)(current_player & 7u)) & 0xFFFFu;
	current_player = current_player + 0x10u;
	r14 = shr32(mem_read32(stream + (current_player >> 3)), (int)(current_player & 7u)) & 0xFFFFu;
	current_player = current_player + 0x10u;
	return width;
}

void unpack_picture_pixels(uint32_t stream, uint32_t width, uint32_t colour_cache, uint32_t newest_slot, uint32_t done_bits)
{
	uint16_t x = 0, y = 0;
	uint32_t bit = 0, bitmap = 0, chain_offset = 0, code_bits = 0, code_value = 0, colour = 0, ignored = 0, offset = 0, pixels_left = 0, row_bytes = 0, row_skip = 0;
	uint32_t run_length = 0, done_address = 0, chain_step = 0, chain_done_address = 0;

	x = 0xFFFFu;
	y = 0u;
	colour = 0u;
	bitmap = bitmap_address;
	row_bytes = 0x200u;
	row_skip = 0x200u - width;
	offset = 0xFFFFFFFFu;
	for (;;)
	{
		code_bits = search_bits_up(stream, current_player, 0xFu, 0u, &ignored);
		code_bits++;
		current_player = current_player + code_bits;
		code_value = shr32(mem_read32(stream + (current_player >> 3)), (int)(current_player & 7u)) & (shl32(1u, (int)(code_bits)) - 1u);
		current_player = current_player + code_bits;
		run_length = code_value - 1u + shift_by(1u, (uint8_t)code_bits, 32);
		pixels_left = run_length - 1u;
		if (run_length == 1u)
		{
		unpack_picture_new_pixel:
			x++;
			offset++;
			if ((uint16_t)width <= (uint16_t)x)
			{
				y++;
				if ((uint16_t)r14 <= (uint16_t)y)
					break;
				x = 0u;
				offset += row_skip;
			}
			read_picture_colour(stream, colour_cache, &colour, &newest_slot);
			mem_write8(bitmap + offset, colour);
			done_address = done_bits + (offset >> 3);
			mem_write32(done_address, (mem_read32(done_address) & ~shl32(1u, (int)(offset & 7u))) | shl32(1u, (int)(offset & 7u)));
			bit = shr32(mem_read32(stream + (current_player >> 3)), (int)(current_player & 7u)) & 1u;
			current_player = current_player + 1u;
			if (bit == 0u)
				continue;
			chain_offset = offset;
			for (;;)
			{
				chain_step = shr32(mem_read32(stream + (current_player >> 3)), (int)(current_player & 7u)) & 3u;
				current_player = current_player + 2u;
				switch (chain_step)
				{
				case 0:
					break;
				case 1:
					chain_offset--;
				case 2:
				unpack_picture_chain_down:
					chain_offset += row_bytes;
					mem_write8(bitmap + chain_offset, colour);
					chain_done_address = done_bits + (chain_offset >> 3);
					mem_write32(chain_done_address, (mem_read32(chain_done_address) & ~shl32(1u, (int)(chain_offset & 7u))) | shl32(1u, (int)(chain_offset & 7u)));
					continue;
				case 3:
					chain_offset++;
					goto unpack_picture_chain_down;
				default:
					unknown_code(picture_chain_step_jumps[chain_step]);
				}
				bit = shr32(mem_read32(stream + (current_player >> 3)), (int)(current_player & 7u)) & 1u;
				current_player = current_player + 1u;
				if (bit == 0u)
					break;
				bit = shr32(mem_read32(stream + (current_player >> 3)), (int)(current_player & 7u)) & 1u;
				current_player = current_player + 1u;
				if (bit == 0u)
				{
					chain_offset -= 2u;
					chain_offset += row_bytes;
					mem_write8(bitmap + chain_offset, colour);
					chain_done_address = done_bits + (chain_offset >> 3);
					mem_write32(chain_done_address, (mem_read32(chain_done_address) & ~shl32(1u, (int)(chain_offset & 7u))) | shl32(1u, (int)(chain_offset & 7u)));
					continue;
				}
				chain_offset += 2u;
				chain_offset += row_bytes;
				mem_write8(bitmap + chain_offset, colour);
				chain_done_address = done_bits + (chain_offset >> 3);
				mem_write32(chain_done_address, (mem_read32(chain_done_address) & ~shl32(1u, (int)(chain_offset & 7u))) | shl32(1u, (int)(chain_offset & 7u)));
			}
			continue;
		}
		do
		{
			x++;
			offset++;
			if ((uint16_t)width <= (uint16_t)x)
			{
				y++;
				if ((uint16_t)r14 <= (uint16_t)y)
					return;
				x = 0u;
				offset += row_skip;
			}
			if ((shr32(mem_read32(done_bits + (offset >> 3)), (int)(offset & 7u)) & 1u) == 0u)
				mem_write8(bitmap + offset, colour);
			else
				colour = mem_read8(bitmap + offset);
			pixels_left--;
		} while (pixels_left != 0u);
		goto unpack_picture_new_pixel;
	}
}

void read_picture_colour(uint32_t stream, uint32_t colour_cache, uint32_t *p_colour, uint32_t *p_newest_slot)
{
	uint32_t colour = *p_colour;
	uint32_t newest_slot = *p_newest_slot;
	uint32_t is_cached = 0, newest_offset = 0, next_slot = 0, oldest_slot = 0, picked_colour = 0, picked_slot = 0, previous_slot = 0, slot = 0, slot_offset = 0;

	is_cached = shr32(mem_read32(stream + (current_player >> 3)), (int)(current_player & 7u)) & 1u;
	current_player = current_player + 1u;
	if (is_cached == 0u)
	{
		picked_colour = shr32(mem_read32(stream + (current_player >> 3)), (int)(current_player & 7u)) & 0xFFu;
		current_player = current_player + 8u;
		picked_slot = mem_read8(colour_cache + newest_slot * 3u);
		mem_write8(colour_cache + 2u + picked_slot * 3u, picked_colour);
		*p_colour = picked_colour;
		*p_newest_slot = picked_slot;
		return;
	}
	slot = shr32(mem_read32(stream + (current_player >> 3)), (int)(current_player & 7u)) & 0xFu;
	current_player = current_player + 4u;
	if (newest_slot == slot)
	{
		picked_colour = (colour & ~0xFFu) | mem_read8(colour_cache + 2u + slot * 3u);
		*p_colour = picked_colour;
		return;
	}
	slot_offset = slot * 3u;
	previous_slot = mem_read8(colour_cache + 1u + slot_offset);
	next_slot = mem_read8(colour_cache + slot_offset);
	mem_write8(colour_cache + previous_slot * 3u, next_slot);
	mem_write8(colour_cache + 1u + next_slot * 3u, previous_slot);
	newest_offset = newest_slot * 3u;
	oldest_slot = mem_read8(colour_cache + newest_offset);
	mem_write8(colour_cache + 1u + oldest_slot * 3u, slot);
	mem_write8(colour_cache + slot_offset, oldest_slot);
	mem_write8(colour_cache + newest_offset, slot);
	mem_write8(colour_cache + 1u + slot_offset, newest_slot);
	picked_slot = slot;
	picked_colour = (colour & ~0xFFu) | mem_read8(colour_cache + 2u + slot * 3u);
	*p_colour = picked_colour;
	*p_newest_slot = picked_slot;
}

void rotate_bitmap_for_monitor_flip(void)
{
	uint32_t blocks_left = 0, blocks_per_row = 0, bottom_cursor = 0, bottom_word = 0, row_pairs_left = 0, top_cursor = 0;

	if (setting_monitor_flip == 0u)
		return;
	top_cursor = 0x300000u;
	bottom_cursor = VIDEO_RAM + 0x1BF40u;
	row_pairs_left = 0x70u;
	blocks_per_row = 0x14u;
	do
	{
		blocks_left = blocks_per_row;
		do
		{
			bottom_cursor -= 4u;
			bottom_word = mem_read32(bottom_cursor);
			mem_write32(bottom_cursor, byte_swap32(mem_read32(top_cursor)));
			mem_write32(top_cursor, byte_swap32(bottom_word));
			top_cursor += 4u;
			bottom_cursor -= 4u;
			bottom_word = mem_read32(bottom_cursor);
			mem_write32(bottom_cursor, byte_swap32(mem_read32(top_cursor)));
			mem_write32(top_cursor, byte_swap32(bottom_word));
			top_cursor += 4u;
			bottom_cursor -= 4u;
			bottom_word = mem_read32(bottom_cursor);
			mem_write32(bottom_cursor, byte_swap32(mem_read32(top_cursor)));
			mem_write32(top_cursor, byte_swap32(bottom_word));
			top_cursor += 4u;
			bottom_cursor -= 4u;
			bottom_word = mem_read32(bottom_cursor);
			mem_write32(bottom_cursor, byte_swap32(mem_read32(top_cursor)));
			mem_write32(top_cursor, byte_swap32(bottom_word));
			top_cursor += 4u;
			blocks_left--;
		} while (blocks_left != 0u);
		bottom_cursor -= 0xC0u;
		row_pairs_left--;
		top_cursor += 0xC0u;
	} while (row_pairs_left != 0u);
}

void init_bitmap_layer(uint8_t is_8bpp)
{
	uint8_t is_8bpp_2 = 0;
	uint16_t bottom = 0, left = 0, right = 0, scroll_x = 0, scroll_y = 0, window_top = 0, x = 0, y = 0;
	uint32_t clip_right_bottom = 0, video_base = 0;

	is_8bpp_2 = (uint8_t)is_8bpp != 0u;
	scroll_x = 0u;
	scroll_y = 0u;
	set_bitmap_scroll_2(scroll_x, scroll_y, is_8bpp_2);
	x = 0u;
	y = 0u;
	get_bitmap_address_2(x, y);
	video_base = video_control;
	mem_write16(video_layer_disable, mem_read16(video_layer_disable) & 0x7BDFu);
	mem_write32(video_bitmap_clip_left, 0u);
	clip_right_bottom = 0xDF013Fu;
	if ((int16_t)mem_read16(video_control) < 0)
		clip_right_bottom = (clip_right_bottom & ~0xFFFFu) | 0x19Fu;
	mem_write32(video_base + 0x84u, clip_right_bottom);
	left = 0u;
	window_top = 0u;
	right = 0x1FFu;
	bottom = 0xDFu;
	set_bitmap_window_2(left, window_top, right, bottom);
}

void get_bitmap_address_2(uint16_t x, uint16_t y)
{
	uint8_t x_shift = 0;

	x_shift = (uint8_t)(0xFFu + bitmap_8bpp);
	bitmap_address = shift_by((uint32_t)(int16_t)y, (uint8_t)(x_shift + 9u), 32) + 0x300000u + shift_signed_by((uint32_t)(int16_t)x, x_shift, 32);
}

void set_bitmap_scroll_2(uint16_t scroll_x, uint16_t scroll_y, uint8_t is_8bpp)
{
	mem_write16(video_bitmap_scroll_x, scroll_x);
	mem_write16(video_bitmap_scroll_y, scroll_y);
	is_8bpp &= 1u;
	bitmap_8bpp = is_8bpp;
	mem_write16(video_control, mem_read16(video_control) & ~0x800u);
	mem_write16(video_control, mem_read16(video_control) | ((uint32_t)is_8bpp << 11));
}

void set_bitmap_window_2(uint16_t left, uint16_t top, uint16_t right, uint16_t bottom)
{
	uint16_t old_left = 0, old_top = 0;

	if ((int16_t)right < (int16_t)left)
	{
		old_left = left;
		left = (uint16_t)right;
		right = old_left;
	}
	if ((int16_t)bottom < (int16_t)top)
	{
		old_top = top;
		top = (uint16_t)bottom;
		bottom = old_top;
	}
	bitmap_window_left = left;
	bitmap_window_top = top;
	bitmap_window_right = right;
	bitmap_window_bottom = bottom;
}

void set_bitmap_clip_2(uint16_t left, uint16_t top, uint16_t right, uint16_t bottom)
{
	uint16_t old_left = 0, old_top = 0;

	if ((int16_t)right < (int16_t)left)
	{
		old_left = left;
		left = (uint16_t)right;
		right = old_left;
	}
	if ((int16_t)bottom < (int16_t)top)
	{
		old_top = top;
		top = (uint16_t)bottom;
		bottom = old_top;
	}
	mem_write16(video_layer_disable, mem_read16(video_layer_disable) | 0x8000u);
	mem_write16(video_bitmap_clip_left, left);
	mem_write16(video_bitmap_clip_top, top);
	mem_write16(video_bitmap_clip_right, right);
	mem_write16(video_bitmap_clip_bottom, bottom);
}

void unpack_lz_bytes(uint32_t packed, uint32_t dest)
{
	uint8_t literal = 0;
	uint32_t bit = 0, bytes_left = 0, code_bits = 0, code_value = 0, copy_left = 0, copy_source = 0, ignored = 0, literal_slot = 0, stream = 0;
	uint32_t bytes_before_copy = 0;

	stream = packed;
	current_player = 0u;
	literal_slot = 0u;
	code_bits = search_bits_up(stream, 0u, 0xFu, 0u, &ignored);
	code_bits++;
	current_player = current_player + code_bits;
	code_value = shr32(mem_read32(stream + (current_player >> 3)), (int)(current_player & 7u)) & (shl32(1u, (int)(code_bits)) - 1u);
	current_player = current_player + code_bits;
	bytes_left = code_value - 1u + shift_by(1u, (uint8_t)code_bits, 32);
	for (;;)
	{
		bit = shr32(mem_read32(stream + (current_player >> 3)), (int)(current_player & 7u)) & 1u;
		current_player = current_player + 1u;
		if (bit == 0u)
		{
			literal = (uint8_t)(shr32(mem_read32(stream + (current_player >> 3)), (int)(current_player & 7u)) & 0xFFu);
			mem_write8(dest, literal);
			literal_slot &= 0xFu;
			recent_literals[literal_slot] = literal;
			literal_slot = (uint8_t)(literal_slot + 1u);
			bytes_left--;
			dest++;
			current_player = current_player + 8u;
			if (bytes_left == 0u)
				return;
			continue;
		}
		bit = shr32(mem_read32(stream + (current_player >> 3)), (int)(current_player & 7u)) & 1u;
		current_player = current_player + 1u;
		if (bit == 0u)
		{
			mem_write8(dest, recent_literals[shr32(mem_read32(stream + (current_player >> 3)), (int)(current_player & 7u)) & 0xFu]);
			bytes_left--;
			dest++;
			current_player = current_player + 4u;
			if (bytes_left == 0u)
				return;
			continue;
		}
		code_bits = search_bits_up(stream, current_player, 0xFu, 0u, &ignored);
		code_bits++;
		current_player = current_player + code_bits;
		code_value = shr32(mem_read32(stream + (current_player >> 3)), (int)(current_player & 7u)) & (shl32(1u, (int)(code_bits)) - 1u);
		current_player = current_player + code_bits;
		copy_source = dest - (code_value - 1u + shift_by(1u, (uint8_t)code_bits, 32));
		code_bits = search_bits_up(stream, current_player, 0xFu, 0u, &ignored);
		code_bits++;
		current_player = current_player + code_bits;
		code_value = shr32(mem_read32(stream + (current_player >> 3)), (int)(current_player & 7u)) & (shl32(1u, (int)(code_bits)) - 1u);
		current_player = current_player + code_bits;
		code_bits = code_value - 1u + shift_by(1u, (uint8_t)code_bits, 32);
		copy_left = code_bits;
		do
		{
			mem_write8(dest, mem_read8(copy_source));
			copy_left--;
			copy_source++;
			dest++;
		} while (copy_left != 0u);
		bytes_before_copy = bytes_left;
		bytes_left = bytes_before_copy - code_bits;
		if ((int32_t)bytes_before_copy <= (int32_t)code_bits)
			break;
	}
}
