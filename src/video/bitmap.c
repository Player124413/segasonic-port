/**
 *    Name:        video/bitmap.c
 *    Description: the bitmap layer: clearing it, its address, scroll and window, pixels, lines, spans and rectangles; an unused packed picture decoder and LZ unpacker
 *    Version:     0.1.0
 *    Created:     12-10-2026
 *    Modified:    16-10-2026
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
		PICTURE_COLOUR_SLOT(entry_cursor)->next = next_slot;
		entry_cursor++;
		PICTURE_COLOUR_SLOT(entry_cursor - 0x1u)->previous = previous_slot;
		entry_cursor++;
		PICTURE_COLOUR_SLOT(entry_cursor - 0x2u)->colour = 0u;
		next_slot++;
		previous_slot++;
		slots_left--;
		entry_cursor++;
	} while (slots_left != 0u);
	mem_write8(colour_cache + 0x2Du, 0u);
	PICTURE_COLOUR_SLOT(colour_cache)->previous = 0xFu;
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
					invalid_jump(picture_chain_step_jumps[chain_step]);
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

void clear_bitmap_layer(uint8_t is_8bpp, uint32_t *p_top, uint32_t *p_color, uint32_t *p_inner_count, uint32_t *p_row_pairs_left, uint32_t *p_blocks_per_row, uint32_t *p_lower_row_cursor)
{
	uint32_t color = *p_color;
	uint32_t inner_count = *p_inner_count;
	uint32_t row_pairs_left = *p_row_pairs_left;
	uint32_t blocks_per_row = *p_blocks_per_row;
	uint32_t lower_row_cursor = *p_lower_row_cursor;
	uint8_t is_8bpp_2 = 0;
	uint16_t bottom = 0, right = 0, scroll_x = 0, scroll_y = 0, window_top = 0, x = 0, y = 0;
	uint32_t clip_right_bottom = 0, top = 0, video_base = 0;

	is_8bpp_2 = (uint8_t)is_8bpp != 0u;
	scroll_x = 0u;
	scroll_y = 0u;
	set_bitmap_scroll(scroll_x, scroll_y, is_8bpp_2);
	x = 0u;
	y = 0u;
	get_bitmap_address(x, y);
	video_base = video_control;
	mem_write16(video_layer_disable, mem_read16(video_layer_disable) & 0x7BDFu);
	mem_write32(video_bitmap_clip_left, 0u);
	clip_right_bottom = 0xDF013Fu;
	if ((int16_t)mem_read16(video_control) < 0)
		clip_right_bottom = (clip_right_bottom & ~0xFFFFu) | 0x19Fu;
	mem_write32(video_base + 0x84u, clip_right_bottom);
	clip_right_bottom &= ~0xFFFFu;
	window_top = 0u;
	right = 0x1FFu;
	bottom = 0xDFu;
	set_bitmap_window((uint16_t)clip_right_bottom, window_top, right, bottom);
	clip_right_bottom |= (uint16_t)bitmap_window_left;
	top = (uint16_t)bitmap_window_top;
	right = (uint16_t)bitmap_window_right;
	bottom = (uint16_t)bitmap_window_bottom;
	color &= ~0xFFu;
	fill_rect_unclipped(right, bottom, &clip_right_bottom, &top, &color, &inner_count, &row_pairs_left, &blocks_per_row, &lower_row_cursor);
	*p_top = top;
	*p_color = color;
	*p_inner_count = inner_count;
	*p_row_pairs_left = row_pairs_left;
	*p_blocks_per_row = blocks_per_row;
	*p_lower_row_cursor = lower_row_cursor;
}

void get_bitmap_address(uint16_t x, uint16_t y)
{
	uint8_t x_shift = 0;

	x_shift = (uint8_t)(0xFFu + bitmap_8bpp);
	bitmap_address = 0x300000u + (shift_signed_by((uint32_t)(int16_t)x, x_shift, 32) + shift_by((uint32_t)(int16_t)y, (uint8_t)(x_shift + 9u), 32));
}

void set_bitmap_scroll(uint16_t scroll_x, uint16_t scroll_y, uint8_t is_8bpp)
{
	mem_write16(video_bitmap_scroll_x, scroll_x);
	mem_write16(video_bitmap_scroll_y, scroll_y);
	is_8bpp &= 1u;
	bitmap_8bpp = is_8bpp;
	mem_write16(video_control, mem_read16(video_control) & ~0x800u);
	mem_write16(video_control, mem_read16(video_control) | ((uint32_t)is_8bpp << 11));
}

void set_bitmap_window(uint16_t left, uint16_t top, uint16_t right, uint16_t bottom)
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

void set_bitmap_clip(uint16_t left, uint16_t top, uint16_t right, uint16_t bottom)
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

void draw_pixel(uint32_t x, uint32_t y, uint8_t color)
{
	uint8_t keep_mask = 0, saved_x = 0;
	uint32_t outcode = 0;

	if (bitmap_8bpp != 0u)
	{
		get_point_outcode(&x, &y, &outcode);
		if ((uint8_t)outcode != 0u)
			return;
		mem_write8((y << 9) + x + bitmap_address, color);
		return;
	}
	get_point_outcode(&x, &y, &outcode);
	if ((uint8_t)outcode != 0u)
		return;
	saved_x = (uint8_t)x;
	y = (y << 8) + ((int32_t)x >> 1) + bitmap_address;
	keep_mask = 0xF0u;
	color &= 0xFu;
	if ((saved_x & 1u) != 0u)
	{
		keep_mask = 0xFu;
		color = (uint8_t)((uint32_t)color << 4);
	}
	mem_write8(y, (keep_mask & mem_read8(y)) | color);
}

uint32_t read_pixel(uint32_t x, uint32_t y)
{
	uint8_t saved_x = 0;
	uint32_t outcode = 0, pixel = 0;

	if (bitmap_8bpp != 0u)
	{
		get_point_outcode(&x, &y, &outcode);
		if ((uint8_t)outcode != 0u)
		{
			pixel = 0xFFFFFFFFu;
			return pixel;
		}
		pixel = mem_read8((y << 9) + x + bitmap_address);
		return pixel;
	}
	get_point_outcode(&x, &y, &outcode);
	if ((uint8_t)outcode != 0u)
	{
		pixel = 0xFFFFFFFFu;
		return pixel;
	}
	saved_x = (uint8_t)x;
	pixel = mem_read8((y << 8) + ((int32_t)x >> 1) + bitmap_address);
	if ((saved_x & 1u) != 0u)
		pixel = (uint8_t)((pixel << 4) | (pixel >> 4));
	pixel &= 0xFu;
	return pixel;
}

void draw_line(uint32_t end_x, uint32_t end_y, uint32_t *p_start_x, uint32_t *p_start_y, uint32_t *p_color, uint32_t *p_unclipped_y)
{
	uint32_t start_x = *p_start_x;
	uint32_t start_y = *p_start_y;
	uint32_t color = *p_color;
	uint32_t unclipped_y = *p_unclipped_y;
	uint32_t zero = 0, negative = 0, overflow = 0, carry = 0;
	uint8_t high_nibble_mask = 0, low_nibble_color = 0;
	uint16_t fraction_4bpp = 0, fraction_8bpp = 0;
	uint32_t direction_index = 0, end_outcode = 0, end_x_copy = 0, end_y_copy = 0, end_y_copy_8bpp = 0, ignored = 0, major_step = 0, minor_step = 0, outcode = 0, pixel = 0, row_step_4bpp = 0, slope = 0, span_end_copy = 0, span_length = 0, steep_slope = 0, step_record = 0, steps_left = 0, top_row = 0;
	uint16_t end_y_4bpp = 0, dy_4bpp = 0, end_x_4bpp = 0, dx_4bpp = 0, end_y_8bpp = 0, dy_8bpp = 0, span_end_x = 0, end_x_8bpp = 0, dx_8bpp = 0;
	uint32_t pixels_left = 0, saved_step = 0, saved_dy = 0;

	for (;;)
	{
		get_window_outcode(&start_x, &start_y, &end_x, &end_y, &outcode);
		end_outcode = outcode;
		get_window_outcode(&start_x, &start_y, &end_x, &end_y, &outcode);
		if ((uint16_t)(outcode & end_outcode) != 0u)
		{
			pixel = 0xFFFFFFFFu;
			*p_start_x = pixel;
			*p_start_y = start_y;
			*p_unclipped_y = unclipped_y;
			return;
		}
		if ((uint16_t)(outcode | end_outcode) == 0u)
			break;
		clip_line_to_window(outcode, end_outcode, &start_x, &start_y, &end_x, &end_y, &unclipped_y);
	}
	if (bitmap_8bpp == 0u)
	{
		row_step_4bpp = 0x100u;
		end_y_copy = (uint16_t)end_y;
		end_y_4bpp = end_y_copy;
		dy_4bpp = end_y_4bpp - start_y;
		overflow = (uint32_t)(int16_t)end_y_4bpp - (int16_t)start_y != (uint32_t)(int16_t)(end_y_4bpp - start_y);
		zero = dy_4bpp == 0u;
		negative = dy_4bpp >> 15;
		slope = dy_4bpp;
		if (end_y_4bpp == (uint16_t)start_y)
		{
			fill_span_4bpp((uint16_t)end_x, &start_x, &start_y, &color, &ignored);
			*p_start_x = start_x;
			*p_start_y = start_y;
			*p_color = color;
			*p_unclipped_y = unclipped_y;
			return;
		}
		if ((negative ^ overflow) | zero)
		{
			row_step_4bpp = 0u - row_step_4bpp;
			slope = (uint16_t)(0u - slope);
		}
		end_x_copy = (uint16_t)end_x;
		end_x_4bpp = end_x_copy;
		dx_4bpp = end_x_4bpp - start_x;
		overflow = (uint32_t)(int16_t)end_x_4bpp - (int16_t)start_x != (uint32_t)(int16_t)(end_x_4bpp - start_x);
		zero = dx_4bpp == 0u;
		negative = dx_4bpp >> 15;
		steep_slope = dx_4bpp;
		if (end_x_4bpp == (uint16_t)start_x)
		{
			draw_vertical_span_4bpp((uint16_t)start_x, row_step_4bpp, &start_y, &color, &slope);
			*p_start_x = start_x;
			*p_start_y = start_y;
			*p_color = color;
			*p_unclipped_y = unclipped_y;
			return;
		}
		if ((negative ^ overflow) | zero)
		{
			steep_slope = (uint16_t)(0u - steep_slope);
			start_x = (start_x & ~0xFFFFu) | (uint16_t)end_x;
			start_y = (uint16_t)end_y;
			row_step_4bpp = 0u - row_step_4bpp;
		}
		start_y = ((uint32_t)(uint16_t)start_y << 8) + ((uint16_t)start_x >> 1) + bitmap_address;
		color = (color & ~0xFFu) | (color & 0xFu);
		low_nibble_color = (uint8_t)color;
		color = (color & ~0xFFu) | (uint8_t)(color << 4);
		high_nibble_mask = 0xF0u;
		slope = (uint16_t)(slope + 1u);
		steep_slope = (uint16_t)(steep_slope + 1u);
		if (steep_slope < slope)
		{
			steep_slope = divide_u32(steep_slope << 16, slope);
			fraction_4bpp = 0u;
			if ((start_x & 1u) != 0u)
			{
				do
				{
				draw_line_steep_odd_pixel:
					start_x = (start_x & ~0xFFu) | mem_read8(start_y);
					start_x = (start_x & ~0xFFu) | (start_x & 0xFu);
					start_x = (start_x & ~0xFFu) | (uint8_t)(start_x | color);
					mem_write8(start_y, start_x);
					start_y += row_step_4bpp;
					carry = (uint16_t)(fraction_4bpp + steep_slope) < fraction_4bpp;
					fraction_4bpp += steep_slope;
					slope--;
				} while ((slope != 0u) & (carry == 0u));
				start_y++;
				if (slope == 0u)
				{
					*p_start_x = start_x;
					*p_start_y = start_y;
					*p_color = color;
					*p_unclipped_y = unclipped_y;
					return;
				}
			}
			do
			{
				start_x = (start_x & ~0xFFu) | mem_read8(start_y);
				start_x = (start_x & ~0xFFu) | (start_x & high_nibble_mask);
				start_x = (start_x & ~0xFFu) | (uint8_t)(start_x | low_nibble_color);
				mem_write8(start_y, start_x);
				start_y += row_step_4bpp;
				carry = (uint16_t)(fraction_4bpp + steep_slope) < fraction_4bpp;
				fraction_4bpp += steep_slope;
				slope--;
			} while ((slope != 0u) & (carry == 0u));
			if (slope == 0u)
			{
				*p_start_x = start_x;
				*p_start_y = start_y;
				*p_color = color;
				*p_unclipped_y = unclipped_y;
				return;
			}
			goto draw_line_steep_odd_pixel;
		}
		if (steep_slope == slope)
		{
			if ((start_x & 1u) != 0u)
			{
			draw_line_diagonal_odd_pixel:
				start_x = (start_x & ~0xFFu) | mem_read8(start_y);
				start_x = (start_x & ~0xFFu) | (start_x & 0xFu);
				start_x = (start_x & ~0xFFu) | (uint8_t)(start_x | color);
				mem_write8(start_y, start_x);
				start_y = start_y + 1u + row_step_4bpp;
				steep_slope--;
				if (steep_slope == 0u)
				{
					*p_start_x = start_x;
					*p_start_y = start_y;
					*p_color = color;
					*p_unclipped_y = unclipped_y;
					return;
				}
			}
			start_x = (start_x & ~0xFFu) | mem_read8(start_y);
			start_x = (start_x & ~0xFFu) | (start_x & high_nibble_mask);
			start_x = (start_x & ~0xFFu) | (uint8_t)(start_x | low_nibble_color);
			mem_write8(start_y, start_x);
			start_y += row_step_4bpp;
			pixels_left = steep_slope;
			steep_slope = pixels_left - 1u;
			if (pixels_left == 1u)
			{
				*p_start_x = start_x;
				*p_start_y = start_y;
				*p_color = color;
				*p_unclipped_y = unclipped_y;
				return;
			}
			goto draw_line_diagonal_odd_pixel;
		}
		slope = divide_u32(slope << 16, steep_slope);
		fraction_4bpp = 0u;
		if ((start_x & 1u) != 0u)
		{
		draw_line_shallow_odd_pixel:
			start_x = (start_x & ~0xFFu) | mem_read8(start_y);
		draw_line_shallow_set_high_nibble:
			start_x = (start_x & ~0xFFu) | (start_x & 0xFu);
			start_x = (start_x & ~0xFFu) | (uint8_t)(start_x | color);
			mem_write8(start_y, start_x);
			carry = (uint16_t)(fraction_4bpp + slope) < fraction_4bpp;
			fraction_4bpp += slope;
			steep_slope--;
			start_y++;
			if ((steep_slope != 0u) & (carry == 0u))
			{
			draw_line_shallow_even_pixel:
				start_x = (start_x & ~0xFFu) | mem_read8(start_y);
				start_x = (start_x & ~0xFFu) | (start_x & high_nibble_mask);
				start_x = (start_x & ~0xFFu) | (uint8_t)(start_x | low_nibble_color);
				carry = (uint16_t)(fraction_4bpp + slope) < fraction_4bpp;
				fraction_4bpp += slope;
				steep_slope--;
				if ((steep_slope != 0u) & (carry == 0u))
					goto draw_line_shallow_set_high_nibble;
				mem_write8(start_y, start_x);
				if (steep_slope == 0u)
				{
					*p_start_x = start_x;
					*p_start_y = start_y;
					*p_color = color;
					*p_unclipped_y = unclipped_y;
					return;
				}
				start_y += row_step_4bpp;
				goto draw_line_shallow_odd_pixel;
			}
			start_y += row_step_4bpp;
			if (steep_slope == 0u)
			{
				*p_start_x = start_x;
				*p_start_y = start_y;
				*p_color = color;
				*p_unclipped_y = unclipped_y;
				return;
			}
			goto draw_line_shallow_even_pixel;
		}
		goto draw_line_shallow_even_pixel;
	}
	direction_index = 0u;
	end_y_copy_8bpp = end_y;
	end_y_8bpp = end_y_copy_8bpp;
	dy_8bpp = end_y_8bpp - start_y;
	overflow = (uint32_t)(int16_t)end_y_8bpp - (int16_t)start_y != (uint32_t)(int16_t)(end_y_8bpp - start_y);
	zero = dy_8bpp == 0u;
	negative = dy_8bpp >> 15;
	slope = (end_y_copy_8bpp & ~0xFFFFu) | dy_8bpp;
	if (end_y_8bpp == (uint16_t)start_y)
	{
		start_y = ((uint32_t)(uint16_t)start_y << 9) + start_x + bitmap_address;
		span_end_copy = (uint16_t)end_x;
		span_end_x = span_end_copy;
		span_length = (uint16_t)(span_end_x - start_x);
		if ((int16_t)start_x <= (int16_t)span_end_x)
		{
			fill_span_8bpp(span_length, &start_x, &start_y, &color);
			*p_start_x = start_x;
			*p_start_y = start_y;
			*p_color = color;
			*p_unclipped_y = unclipped_y;
			return;
		}
		span_length = (uint16_t)(0u - span_length);
		start_y -= span_length;
		fill_span_8bpp(span_length, &start_x, &start_y, &color);
		*p_start_x = start_x;
		*p_start_y = start_y;
		*p_color = color;
		*p_unclipped_y = unclipped_y;
		return;
	}
	if ((negative ^ overflow) | zero)
	{
		slope = (slope & ~0xFFFFu) | (uint16_t)(0u - slope);
		direction_index = (uint8_t)(direction_index + 1u);
	}
	steps_left = end_x;
	end_x_8bpp = steps_left;
	dx_8bpp = end_x_8bpp - start_x;
	overflow = (uint32_t)(int16_t)end_x_8bpp - (int16_t)start_x != (uint32_t)(int16_t)(end_x_8bpp - start_x);
	zero = dx_8bpp == 0u;
	negative = dx_8bpp >> 15;
	steps_left = (steps_left & ~0xFFFFu) | dx_8bpp;
	if (end_x_8bpp == (uint16_t)start_x)
	{
		if (direction_index == 0u)
		{
			start_y = (uint32_t)(uint16_t)start_y << 9;
			start_y = ((start_y & ~0xFFFFu) | (uint16_t)(start_y + start_x)) + bitmap_address;
			start_y = draw_vertical_line_8bpp(start_y, (uint8_t)color, slope);
			*p_start_x = start_x;
			*p_start_y = start_y;
			*p_color = color;
			*p_unclipped_y = unclipped_y;
			return;
		}
		top_row = end_y;
		top_row = (uint32_t)(uint16_t)top_row << 9;
		top_row = ((top_row & ~0xFFFFu) | (uint16_t)(top_row + start_x)) + bitmap_address;
		top_row = draw_vertical_line_8bpp(top_row, (uint8_t)color, slope);
		*p_start_x = start_x;
		*p_start_y = top_row;
		*p_color = color;
		*p_unclipped_y = unclipped_y;
		return;
	}
	if ((negative ^ overflow) | zero)
	{
		steps_left = (steps_left & ~0xFFFFu) | (uint16_t)(0u - steps_left);
		direction_index = (uint8_t)(direction_index + 2u);
	}
	start_y = ((uint32_t)(int16_t)start_y << 9) + start_x + bitmap_address;
	step_record = line_direction_steps_ADDRESS + direction_index * 4u;
	minor_step = (int16_t)ROM16(step_record);
	major_step = (int16_t)ROM16(step_record + 2u);
	if ((uint16_t)steps_left == (uint16_t)slope)
	{
		minor_step += major_step;
		slope = (slope & ~0xFFFFu) | (uint16_t)(slope + 1u);
		carry = slope & 1u;
		slope = slope >> 1;
		if (carry)
			slope = (slope & ~0xFFFFu) | (uint16_t)(slope + 1u);
		do
		{
			mem_write8(start_y, color);
			start_y += minor_step;
			mem_write8(start_y, color);
			start_y += minor_step;
			slope--;
		} while (slope != 0u);
		start_x = (uint16_t)slope;
		*p_start_x = start_x;
		*p_start_y = start_y;
		*p_unclipped_y = unclipped_y;
		return;
	}
	if ((uint16_t)steps_left < (uint16_t)slope)
	{
		saved_step = major_step;
		major_step = minor_step;
		minor_step = saved_step;
		saved_dy = slope;
		slope = steps_left;
		steps_left = saved_dy;
	}
	steps_left = (steps_left & ~0xFFFFu) | (uint16_t)(steps_left + 1u);
	slope = divide_s32((uint32_t)(uint16_t)(slope + 1u) << 16, steps_left);
	fraction_8bpp = 0u;
	for (;;)
	{
		mem_write8(start_y, color);
		start_y += major_step;
		carry = (uint16_t)(fraction_8bpp + slope) < fraction_8bpp;
		fraction_8bpp += slope;
		steps_left--;
		if ((steps_left != 0u) & (carry == 0u))
			continue;
		start_y += minor_step;
		if (steps_left == 0u)
			break;
	}
	start_x = (uint16_t)steps_left;
	*p_start_x = start_x;
	*p_start_y = start_y;
	*p_unclipped_y = unclipped_y;
}

void fill_span_8bpp(uint32_t length, uint32_t *p_blocks, uint32_t *p_pixel_address, uint32_t *p_color)
{
	uint32_t blocks = *p_blocks;
	uint32_t pixel_address = *p_pixel_address;
	uint32_t color = *p_color;
	uint32_t zero = 0;
	uint8_t tail_count = 0;
	uint32_t length_left = 0, block_count = 0, tail_pixels = 0;

	if ((pixel_address & 1u) != 0u)
	{
		mem_write8(pixel_address, color);
		length_left = length - 1u;
		length = length_left;
		pixel_address++;
		if ((int32_t)length_left < 0)
		{
			pixel_address = (pixel_address & ~0xFFFFu) | (uint16_t)(pixel_address - 1u);
			*p_pixel_address = pixel_address;
			return;
		}
	}
	if (length != 0u)
	{
		color = (uint8_t)color * 0x1010101u;
		length++;
		block_count = length >> 4;
		blocks = block_count;
		if (block_count != 0u)
		{
			do
			{
				mem_write32(pixel_address, color);
				pixel_address += 4u;
				mem_write32(pixel_address, color);
				pixel_address += 4u;
				mem_write32(pixel_address, color);
				pixel_address += 4u;
				mem_write32(pixel_address, color);
				blocks--;
				pixel_address += 4u;
			} while (blocks != 0u);
		}
		length &= 0xFu;
		if (length >= 8u)
		{
			mem_write32(pixel_address, color);
			pixel_address += 4u;
			mem_write32(pixel_address, color);
			length -= 8u;
			pixel_address += 4u;
		}
		if (length >= 4u)
		{
			mem_write32(pixel_address, color);
			pixel_address += 4u;
		}
		tail_pixels = length & 3u;
		tail_count = (uint8_t)tail_pixels;
		if (tail_pixels == 0u)
		{
			pixel_address = (pixel_address & ~0xFFFFu) | (uint16_t)(pixel_address - 1u);
			*p_blocks = blocks;
			*p_pixel_address = pixel_address;
			*p_color = color;
			return;
		}
		zero = (uint8_t)(tail_count - 2u) == 0u;
		if (tail_count >= 2u)
		{
			mem_write16(pixel_address, color);
			pixel_address += 2u;
			if (zero)
			{
				pixel_address = (pixel_address & ~0xFFFFu) | (uint16_t)(pixel_address - 1u);
				*p_blocks = blocks;
				*p_pixel_address = pixel_address;
				*p_color = color;
				return;
			}
		}
	}
	mem_write8(pixel_address, color);
	pixel_address++;
	pixel_address = (pixel_address & ~0xFFFFu) | (uint16_t)(pixel_address - 1u);
	*p_blocks = blocks;
	*p_pixel_address = pixel_address;
	*p_color = color;
}

uint32_t draw_vertical_line_8bpp(uint32_t pixel_address, uint8_t color, uint32_t length)
{
	uint32_t row_step = 0;

	row_step = 0x200u;
	draw_vertical_span_8bpp(row_step, color, &pixel_address, &length);
	return pixel_address;
}

void draw_vertical_span_8bpp(uint32_t row_step, uint8_t color, uint32_t *p_pixel_address, uint32_t *p_length)
{
	uint32_t pixel_address = *p_pixel_address;
	uint32_t length = *p_length;
	uint32_t carry = 0;

	length = (length & ~0xFFFFu) | (uint16_t)(length + 1u);
	carry = length & 1u;
	length = (length & ~0xFFFFu) | ((uint16_t)length >> 1);
	if (carry == 0u)
	{
		mem_write8(pixel_address, color);
		pixel_address += row_step;
	}
	else
		length = (length & ~0xFFFFu) | (uint16_t)(length + 1u);
	for (;;)
	{
		mem_write8(pixel_address, color);
		pixel_address += row_step;
		length--;
		if (length == 0u)
			break;
		mem_write8(pixel_address, color);
		pixel_address += row_step;
	}
	pixel_address -= row_step;
	*p_pixel_address = pixel_address;
	*p_length = length;
}

void fill_span_4bpp(uint16_t end_x, uint32_t *p_start_x, uint32_t *p_start_y, uint32_t *p_color, uint32_t *p_length)
{
	uint32_t start_x = *p_start_x;
	uint32_t start_y = *p_start_y;
	uint32_t color = *p_color;
	uint32_t carry = 0;
	uint32_t end_x_copy = 0, keep_mask = 0, length = 0, longs_left = 0;
	uint16_t end_column = 0;
	uint32_t length_left = 0, tail_pixels = 0, pixels_before_byte = 0, pixels_before_second = 0;

	end_x_copy = (uint16_t)end_x;
	end_column = end_x_copy;
	length = (uint16_t)(end_column - start_x);
	if ((int16_t)end_column < (int16_t)start_x)
	{
		start_x = (start_x & ~0xFFFFu) | (uint16_t)end_x;
		length = (uint16_t)(0u - length);
	}
	color = (color & ~0xFFu) | (color & 0xFu);
	start_y = ((uint32_t)(uint16_t)start_y << 8) + bitmap_address;
	carry = start_x & 1u;
	start_x = (start_x & ~0xFFFFu) | ((uint16_t)start_x >> 1);
	if (carry)
	{
		start_y = (start_y & ~0xFFFFu) | (uint16_t)(start_y + start_x);
		start_x = (start_x & ~0xFFu) | mem_read8(start_y);
		start_x = (start_x & ~0xFFu) | (start_x & 0xFu);
		start_x = (start_x & ~0xFFu) | (uint8_t)(start_x | (color << 4));
		mem_write8(start_y, start_x);
		length_left = length - 1u;
		length = length_left;
		start_y++;
		if ((int32_t)length_left < 0)
		{
			*p_start_x = start_x;
			*p_start_y = start_y;
			*p_color = color;
			*p_length = length;
			return;
		}
		start_x = 0u;
	}
	start_y = (start_y & ~0xFFFFu) | (uint16_t)(start_y + start_x);
	length++;
	if ((start_y & 1u) == 0u)
	{
		color = color * 0x11111111u;
		if ((uint16_t)length >= 8u)
		{
			longs_left = length >> 3;
			do
			{
				mem_write32(start_y, color);
				start_y += 4u;
				longs_left--;
			} while (longs_left != 0u);
			tail_pixels = length & 7u;
			length = tail_pixels;
			if (tail_pixels == 0u)
			{
				*p_start_x = start_x;
				*p_start_y = start_y;
				*p_color = color;
				*p_length = length;
				return;
			}
		}
		length = (length & ~0xFFu) | (uint8_t)(length * 2u);
		length = (length & ~0xFFu) | (uint8_t)(length * 2u);
		keep_mask = shift_by(0xFFFFFFFFu, (uint8_t)length, 32);
		start_x = ~keep_mask;
		color = color & start_x & start_x;
		mem_write32(start_y, (keep_mask & mem_read32(start_y)) | color);
		*p_start_x = start_x;
		*p_start_y = start_y;
		*p_color = color;
		*p_length = length;
		return;
	}
	pixels_before_byte = length;
	length = pixels_before_byte - 1u;
	if (pixels_before_byte != 1u)
	{
		start_x = (start_x & ~0xFFu) | (uint8_t)color;
		start_x = (start_x & ~0xFFu) | (uint8_t)(start_x << 4);
		start_x = (start_x & ~0xFFu) | (uint8_t)(start_x | color);
		mem_write8(start_y, start_x);
		pixels_before_second = length;
		length = pixels_before_second - 1u;
		start_y++;
		if (pixels_before_second == 1u)
		{
			*p_start_x = start_x;
			*p_start_y = start_y;
			*p_color = color;
			*p_length = length;
			return;
		}
		color = color * 0x11111111u;
		if ((uint16_t)length >= 8u)
		{
			longs_left = length >> 3;
			do
			{
				mem_write32(start_y, color);
				start_y += 4u;
				longs_left--;
			} while (longs_left != 0u);
			tail_pixels = length & 7u;
			length = tail_pixels;
			if (tail_pixels == 0u)
			{
				*p_start_x = start_x;
				*p_start_y = start_y;
				*p_color = color;
				*p_length = length;
				return;
			}
		}
		length = (length & ~0xFFu) | (uint8_t)(length * 2u);
		length = (length & ~0xFFu) | (uint8_t)(length * 2u);
		keep_mask = shift_by(0xFFFFFFFFu, (uint8_t)length, 32);
		start_x = ~keep_mask;
		color = color & start_x & start_x;
		mem_write32(start_y, (keep_mask & mem_read32(start_y)) | color);
		*p_start_x = start_x;
		*p_start_y = start_y;
		*p_color = color;
		*p_length = length;
		return;
	}
	start_x = (start_x & ~0xFFu) | 0xF0u;
	start_x = (start_x & ~0xFFu) | (start_x & mem_read8(start_y));
	start_x = (start_x & ~0xFFu) | (uint8_t)(start_x | color);
	mem_write8(start_y, start_x);
	*p_start_x = start_x;
	*p_start_y = start_y;
	*p_color = color;
	*p_length = length;
}

void draw_vertical_span_4bpp(uint16_t start_x, uint32_t row_step, uint32_t *p_start_y, uint32_t *p_color, uint32_t *p_length)
{
	uint32_t start_y = *p_start_y;
	uint32_t color = *p_color;
	uint32_t length = *p_length;

	start_y = (uint32_t)(uint16_t)start_y << 8;
	start_y = ((start_y & ~0xFFFFu) | (uint16_t)(start_y + ((uint16_t)start_x >> 1))) + bitmap_address;
	draw_vertical_span_4bpp_here((uint8_t)start_x, row_step, &start_y, &color, &length);
	*p_start_y = start_y;
	*p_color = (*p_color & 0xFFFFFF00u) | (color & 0xFFu);
	*p_length = length;
}

void draw_vertical_span_4bpp_here(uint8_t start_x, uint32_t row_step, uint32_t *p_pixel_address, uint32_t *p_color, uint32_t *p_length)
{
	uint32_t pixel_address = *p_pixel_address;
	uint8_t color = (uint8_t)*p_color;
	uint32_t length = *p_length;
	uint32_t carry = 0;
	uint8_t keep_mask = 0;

	length = (length & ~0xFFFFu) | (uint16_t)(length + 1u);
	keep_mask = 0xF0u;
	color &= 0xFu;
	if ((start_x & 1u) != 0u)
	{
		color = (uint8_t)((uint32_t)color << 4);
		keep_mask = 0xFu;
	}
	carry = length & 1u;
	length = length >> 1;
	if (carry == 0u)
	{
		mem_write8(pixel_address, (mem_read8(pixel_address) & keep_mask) | color);
		pixel_address += row_step;
	}
	else
		length++;
	for (;;)
	{
		mem_write8(pixel_address, (mem_read8(pixel_address) & keep_mask) | color);
		pixel_address += row_step;
		length--;
		if (length == 0u)
			break;
		mem_write8(pixel_address, (mem_read8(pixel_address) & keep_mask) | color);
		pixel_address += row_step;
	}
	*p_pixel_address = pixel_address;
	*p_color = (*p_color & 0xFFFFFF00u) | color;
	*p_length = length;
}

void draw_dotted_line_8bpp(uint32_t start_x, uint32_t start_y, uint32_t end_x, uint32_t end_y, uint8_t color)
{
	uint32_t zero = 0, negative = 0, overflow = 0, carry = 0;
	uint16_t fraction = 0;
	uint32_t direction_index = 0, dot_count = 0, dots_left = 0, dy = 0, end_outcode = 0, end_x_copy = 0, end_y_copy = 0, ignored = 0, major_step = 0, minor_step = 0, outcode = 0, row_step = 0, span_end_copy = 0, span_length = 0, step_record = 0, steps_left = 0;
	uint16_t end_y_16 = 0, dy_16 = 0, span_end_16 = 0, span_dots = 0, end_x_16 = 0, dx_16 = 0, column_dots = 0, diagonal_dots = 0;
	uint32_t saved_step = 0, saved_dy = 0;

	get_window_outcode(&start_x, &start_y, &end_x, &end_y, &outcode);
	end_outcode = outcode;
	get_window_outcode(&start_x, &start_y, &end_x, &end_y, &outcode);
	if ((uint16_t)(outcode & end_outcode) != 0u)
		return;
	if ((uint16_t)(outcode | end_outcode) != 0u)
		clip_line_to_window(outcode, end_outcode, &start_x, &start_y, &end_x, &end_y, &ignored);
	direction_index = 0u;
	end_y_copy = end_y;
	end_y_16 = end_y_copy;
	dy_16 = end_y_16 - start_y;
	overflow = (uint32_t)(int16_t)end_y_16 - (int16_t)start_y != (uint32_t)(int16_t)(end_y_16 - start_y);
	zero = dy_16 == 0u;
	negative = dy_16 >> 15;
	dy = (end_y_copy & ~0xFFFFu) | dy_16;
	if (end_y_16 == (uint16_t)start_y)
	{
		start_y = ((uint32_t)(uint16_t)start_y << 9) + start_x + bitmap_address;
		span_end_copy = (uint16_t)end_x;
		span_end_16 = span_end_copy;
		span_length = (uint16_t)(span_end_16 - start_x);
		if ((int16_t)span_end_16 < (int16_t)start_x)
		{
			span_length = (uint16_t)(0u - span_length);
			start_y -= span_length;
		}
		span_length = (uint16_t)(span_length + 1u);
		span_dots = span_length >> 1;
		dots_left = span_dots;
		if (span_dots == 0u)
		{
			dots_left = (uint16_t)(dots_left + 1u);
		}
		do
		{
			mem_write8(start_y, color);
			start_y = ((start_y + 1u) & ~0xFFFFu) | (uint16_t)(start_y + 1u + 1u);
			dots_left--;
		} while (dots_left != 0u);
		return;
	}
	if ((negative ^ overflow) | zero)
	{
		dy = (dy & ~0xFFFFu) | (uint16_t)(0u - dy);
		direction_index = (uint8_t)(direction_index + 1u);
	}
	end_x_copy = end_x;
	end_x_16 = end_x_copy;
	dx_16 = end_x_16 - start_x;
	overflow = (uint32_t)(int16_t)end_x_16 - (int16_t)start_x != (uint32_t)(int16_t)(end_x_16 - start_x);
	zero = dx_16 == 0u;
	negative = dx_16 >> 15;
	steps_left = (end_x_copy & ~0xFFFFu) | dx_16;
	if (end_x_16 == (uint16_t)start_x)
	{
		if (direction_index != 0u)
			start_y = end_y;
		start_y = (uint32_t)(uint16_t)start_y << 9;
		start_y = ((start_y & ~0xFFFFu) | (uint16_t)(start_y + start_x)) + bitmap_address;
		row_step = 0x400u;
		dy = (dy & ~0xFFFFu) | (uint16_t)(dy + 1u);
		column_dots = (uint16_t)dy >> 1;
		dot_count = (dy & ~0xFFFFu) | column_dots;
		if (column_dots == 0u)
		{
			dot_count = (dot_count & ~0xFFFFu) | (uint16_t)(dot_count + 1u);
		}
		do
		{
			mem_write8(start_y, color);
			start_y += row_step;
			dot_count--;
		} while (dot_count != 0u);
		return;
	}
	if ((negative ^ overflow) | zero)
	{
		steps_left = (steps_left & ~0xFFFFu) | (uint16_t)(0u - steps_left);
		direction_index = (uint8_t)(direction_index + 2u);
	}
	start_y = ((uint32_t)(uint16_t)start_y << 9) + start_x + bitmap_address;
	step_record = line_direction_steps_ADDRESS + direction_index * 4u;
	minor_step = (int16_t)ROM16(step_record);
	major_step = (int16_t)ROM16(step_record + 2u);
	if ((uint16_t)steps_left == (uint16_t)dy)
	{
		minor_step = (minor_step + major_step) * 2u;
		dy = (dy & ~0xFFFFu) | (uint16_t)(dy + 1u);
		diagonal_dots = (uint16_t)dy >> 1;
		dot_count = (dy & ~0xFFFFu) | diagonal_dots;
		if (diagonal_dots == 0u)
		{
			dot_count = (dot_count & ~0xFFFFu) | (uint16_t)(dot_count + 1u);
		}
		do
		{
			mem_write8(start_y, color);
			start_y += minor_step;
			dot_count--;
		} while (dot_count != 0u);
		return;
	}
	if ((uint16_t)steps_left < (uint16_t)dy)
	{
		saved_step = major_step;
		major_step = minor_step;
		minor_step = saved_step;
		saved_dy = dy;
		dy = steps_left;
		steps_left = saved_dy;
	}
	steps_left = (steps_left & ~0xFFFFu) | (uint16_t)(steps_left + 1u);
	dy = divide_s32((uint32_t)(uint16_t)(dy + 1u) << 16, steps_left);
	fraction = 0u;
	for (;;)
	{
		mem_write8(start_y, color);
		start_y += major_step;
		carry = (uint16_t)(fraction + dy) < fraction;
		fraction += dy;
		steps_left--;
		if (((steps_left != 0u) & (carry == 0u)) == 0u)
		{
			start_y += minor_step;
			if (steps_left == 0u)
				break;
		}
		start_y += major_step;
		carry = (uint16_t)(fraction + dy) < fraction;
		fraction += dy;
		steps_left--;
		if ((steps_left != 0u) & (carry == 0u))
			continue;
		start_y += minor_step;
		if ((int32_t)steps_left <= 0)
			break;
	}
}

void draw_rect_outline(uint32_t left, uint32_t top, uint32_t width, uint32_t height, uint32_t color)
{
	uint32_t column_address = 0, column_length = 0, ignored = 0, left_byte_offset = 0, length = 0, pixel_address = 0, right_side_length = 0, row_step = 0, saved_top_left = 0, top_left_address = 0, unclipped_edges = 0;

	clip_rect_to_window(&left, &top, &width, &height, &left_byte_offset, &unclipped_edges);
	if (unclipped_edges == 0u)
		return;
	left = (int16_t)left;
	top = (int16_t)top;
	width = (int16_t)width - left;
	height = (int16_t)height - top;
	unclipped_edges = unclipped_edges * 3u;
	if (bitmap_8bpp == 0u)
	{
		color = (color & 0xFu) * 0x11111111u;
		left_byte_offset = left >> 1;
		top = (top << 8) + left_byte_offset + bitmap_address;
		switch (unclipped_edges)
		{
		case 0x0u:
		case 0x9u:
		case 0xCu:
		case 0xFu:
		case 0x12u:
		case 0x15u:
		case 0x18u:
		case 0x1Bu:
		case 0x1Eu:
		case 0x21u:
		case 0x24u:
		case 0x27u:
		case 0x2Au:
		case 0x2Du:
			return;
		case 0x3u:
			break;
		case 0x6u:
		case 0x30u:
			left = (uint16_t)(left + width);
			top = (top & ~0xFFFFu) | (uint16_t)(top + ((left >> 1) - left_byte_offset));
			break;
		default:
			invalid_jump(0x9ADA3u + unclipped_edges);
		}
		column_length = (uint16_t)height;
		draw_vertical_span_4bpp_here((uint8_t)left, unclipped_edges, &top, &color, &column_length);
		return;
	}
	color = (uint8_t)color * 0x1010101u;
	top = (top << 9) + left + bitmap_address;
	switch (unclipped_edges)
	{
	case 0x0u:
	case 0x30u:
	case 0x39u:
	case 0x3Cu:
	case 0x3Fu:
	case 0x42u:
	case 0x45u:
	case 0x48u:
	case 0x4Bu:
	case 0x4Eu:
	case 0x51u:
	case 0x54u:
	case 0x57u:
	case 0x5Au:
	case 0x5Du:
		return;
	case 0x3u:
		column_length = height;
		top = draw_vertical_line_8bpp(top, (uint8_t)color, column_length);
		return;
	case 0x6u:
		top += width;
		column_length = height;
		top = draw_vertical_line_8bpp(top, (uint8_t)color, column_length);
		return;
	case 0x9u:
		if (height == 0u)
		{
			column_length = height;
			top = draw_vertical_line_8bpp(top, (uint8_t)color, column_length);
			return;
		}
		top = draw_rect_sides_8bpp(top, width, height, (uint8_t)color);
		return;
	case 0xCu:
		length = width;
		fill_span_8bpp(length, &ignored, &top, &color);
		return;
	case 0xFu:
		saved_top_left = top;
		column_length = height;
		length = width;
		fill_span_8bpp(length, &ignored, &top, &color);
		column_address = saved_top_left;
		column_address = draw_vertical_line_8bpp(column_address, (uint8_t)color, column_length);
		return;
	case 0x12u:
		column_length = height;
		length = width;
		fill_span_8bpp(length, &ignored, &top, &color);
		top = draw_vertical_line_8bpp(top, (uint8_t)color, column_length);
		return;
	case 0x15u:
		if (height != 0u)
		{
			top_left_address = top;
			draw_rect_sides_8bpp(top, width, height, (uint8_t)color);
			pixel_address = top_left_address;
			length = width;
			fill_span_8bpp(length, &ignored, &pixel_address, &color);
			return;
		}
		saved_top_left = top;
		column_length = height;
		length = width;
		fill_span_8bpp(length, &ignored, &top, &color);
		column_address = saved_top_left;
		column_address = draw_vertical_line_8bpp(column_address, (uint8_t)color, column_length);
		return;
	case 0x18u:
		top += height << 9;
		length = width;
		fill_span_8bpp(length, &ignored, &top, &color);
		return;
	case 0x1Bu:
		column_length = height;
		length = width;
		top = draw_vertical_line_8bpp(top, (uint8_t)color, column_length);
		fill_span_8bpp(length, &ignored, &top, &color);
		return;
	case 0x1Eu:
		right_side_length = height;
		top += height << 9;
		length = width;
		fill_span_8bpp(length, &ignored, &top, &color);
		row_step = (int16_t)0xFE00u;
		draw_vertical_span_8bpp(row_step, (uint8_t)color, &top, &right_side_length);
		return;
	case 0x21u:
		if (height == 0u)
		{
			column_length = height;
			length = width;
			top = draw_vertical_line_8bpp(top, (uint8_t)color, column_length);
			fill_span_8bpp(length, &ignored, &top, &color);
			return;
		}
		top = draw_rect_sides_8bpp(top, width, height, (uint8_t)color);
		length = width;
		fill_span_8bpp(length, &ignored, &top, &color);
		return;
	case 0x24u:
		top = draw_rect_top_bottom_8bpp(top, width, height, color);
		return;
	case 0x27u:
		if (width != 0u)
		{
			top_left_address = top;
			draw_rect_top_bottom_8bpp(top, width, height, color);
			column_address = top_left_address;
			column_length = height;
			column_address = draw_vertical_line_8bpp(column_address, (uint8_t)color, column_length);
			return;
		}
		saved_top_left = top;
		column_length = height;
		length = width;
		fill_span_8bpp(length, &ignored, &top, &color);
		column_address = saved_top_left;
		column_address = draw_vertical_line_8bpp(column_address, (uint8_t)color, column_length);
		return;
	case 0x2Au:
		if (width == 0u)
		{
			column_length = height;
			length = width;
			fill_span_8bpp(length, &ignored, &top, &color);
			top = draw_vertical_line_8bpp(top, (uint8_t)color, column_length);
			return;
		}
		top = draw_rect_top_bottom_8bpp(top, width, height, color);
		column_length = height;
		top = draw_vertical_line_8bpp(top, (uint8_t)color, column_length);
		return;
	case 0x2Du:
		if (width == 0u)
		{
			if (height != 0u)
			{
				top_left_address = top;
				draw_rect_sides_8bpp(top, width, height, (uint8_t)color);
				pixel_address = top_left_address;
				length = width;
				fill_span_8bpp(length, &ignored, &pixel_address, &color);
				return;
			}
			saved_top_left = top;
			column_length = height;
			length = width;
			fill_span_8bpp(length, &ignored, &top, &color);
			column_address = saved_top_left;
			column_address = draw_vertical_line_8bpp(column_address, (uint8_t)color, column_length);
			return;
		}
		top_left_address = top;
		draw_rect_top_bottom_8bpp(top, width, height, color);
		top = top_left_address;
		if (height == 0u)
		{
			column_length = height;
			top = draw_vertical_line_8bpp(top, (uint8_t)color, column_length);
			return;
		}
		top = draw_rect_sides_8bpp(top, width, height, (uint8_t)color);
		return;
	case 0x33u:
		column_length = (uint16_t)height;
		draw_vertical_span_4bpp_here((uint8_t)left, unclipped_edges, &top, &color, &column_length);
		return;
	case 0x36u:
	case 0x60u:
		left = (uint16_t)(left + width);
		top = (top & ~0xFFFFu) | (uint16_t)(top + ((left >> 1) - left_byte_offset));
		column_length = (uint16_t)height;
		draw_vertical_span_4bpp_here((uint8_t)left, unclipped_edges, &top, &color, &column_length);
		return;
	default:
		invalid_jump(0x9AD73u + unclipped_edges);
	}
}

uint32_t draw_rect_sides_8bpp(uint32_t pixel_address, uint32_t width, uint32_t height, uint8_t color)
{
	uint32_t right_address = 0, row_step = 0;

	right_address = pixel_address + width;
	row_step = 0x200u;
	height = (height & ~0xFFFFu) | (uint16_t)(height + 1u);
	do
	{
		mem_write8(pixel_address, color);
		mem_write8(right_address, color);
		pixel_address += row_step;
		right_address += row_step;
		height--;
	} while (height != 0u);
	pixel_address -= row_step;
	return pixel_address;
}

uint32_t draw_rect_top_bottom_8bpp(uint32_t pixel_address, uint32_t width, uint32_t height, uint32_t color)
{
	uint8_t byte_count = 0, tail_bytes = 0;
	uint32_t bottom_address = 0, bytes_left = 0, longs_left = 0;
	uint32_t bytes_after_odd = 0, long_count = 0, tail_count = 0;

	bottom_address = (height << 9) + pixel_address;
	bytes_left = width;
	if ((pixel_address & 1u) != 0u)
	{
		mem_write8(pixel_address, color);
		mem_write8(bottom_address, color);
		bottom_address++;
		bytes_after_odd = bytes_left - 1u;
		bytes_left = bytes_after_odd;
		pixel_address++;
		if ((int32_t)bytes_after_odd < 0)
		{
			pixel_address = (pixel_address & ~0xFFFFu) | (uint16_t)(pixel_address - 1u);
			return pixel_address;
		}
	}
	if (bytes_left == 0u)
	{
		mem_write8(pixel_address, color);
		mem_write8(bottom_address, color);
		pixel_address++;
	}
	else
	{
		bytes_left++;
		byte_count = (uint8_t)bytes_left;
		long_count = bytes_left >> 2;
		longs_left = long_count;
		if (long_count != 0u)
		{
			do
			{
				mem_write32(pixel_address, color);
				mem_write32(bottom_address, color);
				bottom_address += 4u;
				longs_left--;
				pixel_address += 4u;
			} while (longs_left != 0u);
		}
		tail_count = byte_count & 3u;
		tail_bytes = (uint8_t)tail_count;
		if (tail_count == 0u)
		{
			pixel_address = (pixel_address & ~0xFFFFu) | (uint16_t)(pixel_address - 1u);
			return pixel_address;
		}
		if (tail_bytes < 2u)
		{
			mem_write8(pixel_address, color);
			mem_write8(bottom_address, color);
			pixel_address++;
			pixel_address = (pixel_address & ~0xFFFFu) | (uint16_t)(pixel_address - 1u);
			return pixel_address;
		}
		mem_write16(pixel_address, color);
		mem_write16(bottom_address, color);
		bottom_address += 2u;
		pixel_address += 2u;
		if (tail_bytes != 2u)
		{
			mem_write8(pixel_address, color);
			mem_write8(bottom_address, color);
			pixel_address++;
			pixel_address = (pixel_address & ~0xFFFFu) | (uint16_t)(pixel_address - 1u);
			return pixel_address;
		}
	}
	pixel_address = (pixel_address & ~0xFFFFu) | (uint16_t)(pixel_address - 1u);
	return pixel_address;
}

void fill_rect(uint32_t top, uint32_t right, uint32_t bottom, uint32_t *p_left, uint32_t *p_color, uint32_t *p_blocks_per_row)
{
	uint32_t left = *p_left;
	uint32_t color = *p_color;
	uint32_t blocks_per_row = *p_blocks_per_row;
	uint32_t ignored = 0, pixel = 0, unclipped_edges = 0;

	clip_rect_to_window(&left, &top, &right, &bottom, &ignored, &unclipped_edges);
	if (unclipped_edges == 0u)
	{
		pixel = 0xFFFFFFFFu;
		*p_left = pixel;
		return;
	}
	fill_rect_unclipped(right, bottom, &left, &top, &color, &ignored, &ignored, &blocks_per_row, &ignored);
	*p_left = left;
	*p_color = color;
	*p_blocks_per_row = blocks_per_row;
}

void fill_rect_unclipped(uint32_t right, uint32_t bottom, uint32_t *p_left, uint32_t *p_top, uint32_t *p_color, uint32_t *p_inner_count, uint32_t *p_row_pairs_left, uint32_t *p_blocks_per_row, uint32_t *p_lower_row_cursor)
{
	uint32_t left = *p_left;
	uint32_t top = *p_top;
	uint32_t color = *p_color;
	uint32_t inner_count = *p_inner_count;
	uint32_t row_pairs_left = *p_row_pairs_left;
	uint32_t blocks_per_row = *p_blocks_per_row;
	uint32_t lower_row_cursor = *p_lower_row_cursor;
	uint32_t carry = 0;
	uint8_t byte_color = 0, color_low = 0, high_nibble_color = 0, tail_width = 0;
	uint16_t straddle_keep_mask = 0, tail_keep_mask = 0;
	uint32_t block_cursor = 0, bottom_copy = 0, column_cursor = 0, first_row = 0, last_column = 0, length = 0, lower_cursor = 0, pair_skip = 0, pixel_columns = 0, right_copy = 0, row_bytes = 0, row_offset_halfwords = 0, row_offset_longs = 0, row_pitch_4bpp = 0, row_pitch_8bpp = 0, row_step = 0, row_step_2 = 0, rows_left = 0, rows_to_fill = 0, span_length = 0, upper_cursor = 0;
	uint16_t block_count = 0, bottom_row = 0, right_column = 0, rect_dx = 0;
	uint32_t columns_left = 0, tail_columns = 0, tail_pixels = 0;

	if (bitmap_8bpp != 0u)
	{
		bottom = (uint16_t)bottom;
		top = (uint16_t)top;
		bottom = (uint16_t)(bottom - top);
		top = top << 9;
		top = ((top & ~0xFFFFu) | (uint16_t)(top + left)) + bitmap_address;
		color = (uint8_t)color * 0x1010101u;
		right = (uint16_t)right;
		right = (uint16_t)(right - left);
		bottom = (uint16_t)(bottom + 1u);
		row_pitch_8bpp = 0x200u;
		carry = bottom & 1u;
		bottom = bottom >> 1;
		if (carry)
		{
			first_row = top;
			row_pairs_left = bottom;
			length = right;
			fill_span_8bpp(length, &left, &top, &color);
			if (row_pairs_left == 0u)
			{
				*p_left = left;
				*p_top = top;
				*p_color = color;
				*p_row_pairs_left = row_pairs_left;
				return;
			}
			top = first_row + row_pitch_8bpp;
			bottom = row_pairs_left;
		}
		if ((top & 1u) != 0u)
		{
			left = top;
			inner_count = bottom;
			top++;
			do
			{
				mem_write8(left, color);
				mem_write8(left + row_pitch_8bpp, color);
				left += row_pitch_8bpp * 2u;
				inner_count--;
			} while (inner_count != 0u);
			columns_left = right - 1u;
			right = columns_left;
			if ((int32_t)columns_left < 0)
			{
				*p_left = left;
				*p_top = top;
				*p_color = color;
				*p_inner_count = inner_count;
				*p_row_pairs_left = row_pairs_left;
				return;
			}
		}
		if (right != 0u)
		{
			right = (right & ~0xFFFFu) | (uint16_t)(right + 1u);
			row_bytes = right;
			block_count = (uint16_t)row_bytes >> 4;
			blocks_per_row = (row_bytes & ~0xFFFFu) | block_count;
			if (block_count != 0u)
			{
				left = top;
				row_pairs_left = bottom;
				top += 0xFFFFFFF0u & right;
				do
				{
					inner_count = blocks_per_row;
					upper_cursor = left;
					lower_row_cursor = left + row_pitch_8bpp;
					do
					{
						mem_write32(upper_cursor, color);
						upper_cursor += 4u;
						mem_write32(upper_cursor, color);
						upper_cursor += 4u;
						mem_write32(upper_cursor, color);
						upper_cursor += 4u;
						mem_write32(upper_cursor, color);
						mem_write32(lower_row_cursor, color);
						lower_row_cursor += 4u;
						mem_write32(lower_row_cursor, color);
						lower_row_cursor += 4u;
						mem_write32(lower_row_cursor, color);
						lower_row_cursor += 4u;
						mem_write32(lower_row_cursor, color);
						inner_count--;
						upper_cursor += 4u;
						lower_row_cursor += 4u;
					} while (inner_count != 0u);
					left += row_pitch_8bpp * 2u;
					row_pairs_left--;
				} while (row_pairs_left != 0u);
			}
			right &= 0xFu;
			if (right >= 8u)
			{
				left = top;
				inner_count = bottom;
				lower_cursor = left + row_pitch_8bpp;
				pair_skip = 0x3F8u;
				top += 8u;
				do
				{
					mem_write32(left, color);
					left += 4u;
					mem_write32(left, color);
					mem_write32(lower_cursor, color);
					lower_cursor += 4u;
					mem_write32(lower_cursor, color);
					left = left + 4u + pair_skip;
					lower_cursor = lower_cursor + 4u + pair_skip;
					inner_count--;
				} while (inner_count != 0u);
				row_pitch_8bpp = 0x200u;
				right -= 8u;
			}
			if (right >= 4u)
			{
				left = top;
				inner_count = bottom;
				row_offset_longs = 0x80u;
				top += 4u;
				do
				{
					mem_write32(left, color);
					mem_write32(left + row_offset_longs * 4u, color);
					left += row_pitch_8bpp * 2u;
					inner_count--;
				} while (inner_count != 0u);
			}
			tail_columns = right & 3u;
			tail_width = (uint8_t)tail_columns;
			if (tail_columns == 0u)
			{
				*p_left = left;
				*p_top = top;
				*p_color = color;
				*p_inner_count = inner_count;
				*p_row_pairs_left = row_pairs_left;
				*p_blocks_per_row = blocks_per_row;
				*p_lower_row_cursor = lower_row_cursor;
				return;
			}
			if (tail_width >= 2u)
			{
				inner_count = bottom;
				left = top;
				row_offset_halfwords = 0x100u;
				top += 2u;
				do
				{
					mem_write16(left, color);
					mem_write16(left + row_offset_halfwords * 2u, color);
					left += row_pitch_8bpp * 2u;
					inner_count--;
				} while (inner_count != 0u);
				if ((tail_width & 1u) == 0u)
				{
					*p_left = left;
					*p_top = top;
					*p_color = color;
					*p_inner_count = inner_count;
					*p_row_pairs_left = row_pairs_left;
					*p_blocks_per_row = blocks_per_row;
					*p_lower_row_cursor = lower_row_cursor;
					return;
				}
			}
		}
		do
		{
			mem_write8(top, color);
			mem_write8(top + row_pitch_8bpp, color);
			top += row_pitch_8bpp * 2u;
			bottom--;
		} while (bottom != 0u);
		*p_left = left;
		*p_top = top;
		*p_color = color;
		*p_inner_count = inner_count;
		*p_row_pairs_left = row_pairs_left;
		*p_blocks_per_row = blocks_per_row;
		*p_lower_row_cursor = lower_row_cursor;
		return;
	}
	color = (color & ~0xFFu) | (color & 0xFu);
	bottom_copy = (uint16_t)bottom;
	top = (uint16_t)top;
	bottom_row = bottom_copy;
	rows_left = (uint16_t)(bottom_row - top);
	if (bottom_row == top)
	{
		fill_span_4bpp((uint16_t)right, &left, &top, &color, &span_length);
		*p_left = left;
		*p_top = top;
		*p_color = color;
		*p_inner_count = span_length;
		*p_row_pairs_left = row_pairs_left;
		*p_blocks_per_row = blocks_per_row;
		*p_lower_row_cursor = lower_row_cursor;
		return;
	}
	right_copy = (uint16_t)right;
	right_column = right_copy;
	last_column = (uint16_t)(right_column - left);
	if (right_column == (uint16_t)left)
	{
		row_step_2 = 0x100u;
		draw_vertical_span_4bpp((uint16_t)left, row_step_2, &top, &color, &rows_left);
		*p_left = left;
		*p_top = top;
		*p_color = color;
		*p_inner_count = rows_left;
		*p_row_pairs_left = row_pairs_left;
		*p_blocks_per_row = blocks_per_row;
		*p_lower_row_cursor = row_step_2;
		return;
	}
	top = top << 8;
	top = ((top & ~0xFFFFu) | (uint16_t)(top + ((uint16_t)left >> 1))) + bitmap_address;
	row_pitch_4bpp = 0x100u;
	rows_left = (uint16_t)(rows_left + 1u);
	rect_dx = last_column;
	pixel_columns = (uint16_t)(rect_dx - 1u);
	if (rect_dx == 1u)
	{
		if ((left & 1u) == 0u)
		{
			color_low = (uint8_t)color;
			color = (color & ~0xFFu) | (uint8_t)(color << 4);
			color = (color & ~0xFFu) | (uint8_t)(color | color_low);
			row_step = row_pitch_4bpp;
			draw_vertical_span_8bpp(row_step, (uint8_t)color, &top, &rows_left);
			*p_left = left;
			*p_top = top;
			*p_color = color;
			*p_inner_count = rows_left;
			*p_row_pairs_left = row_pitch_4bpp;
			*p_blocks_per_row = blocks_per_row;
			*p_lower_row_cursor = lower_row_cursor;
			return;
		}
		color = (color & ~0xFFFFu) | (uint16_t)(color * 0x110u);
		straddle_keep_mask = 0xF00Fu;
		do
		{
			left = (left & ~0xFFFFu) | mem_read16(top);
			left = (left & ~0xFFFFu) | (left & straddle_keep_mask);
			left = (left & ~0xFFFFu) | (uint16_t)(left | color);
			mem_write16(top, left);
			top += row_pitch_4bpp;
			rows_left--;
		} while (rows_left != 0u);
		*p_left = left;
		*p_top = top;
		*p_color = color;
		*p_inner_count = rows_left;
		*p_row_pairs_left = row_pitch_4bpp;
		return;
	}
	pixel_columns = (uint16_t)(pixel_columns + 2u);
	if ((left & 1u) != 0u)
	{
		high_nibble_color = (uint8_t)(color << 4);
		rows_to_fill = rows_left;
		column_cursor = top;
		top++;
		do
		{
			left = (left & ~0xFFu) | mem_read8(column_cursor);
			left = (left & ~0xFFu) | (left & 0xFu);
			left = (left & ~0xFFu) | (uint8_t)(left | high_nibble_color);
			mem_write8(column_cursor, left);
			column_cursor += row_pitch_4bpp;
			rows_to_fill--;
		} while (rows_to_fill != 0u);
		pixel_columns--;
	}
	if ((top & 1u) != 0u)
	{
		byte_color = (uint8_t)color;
		color = (color & ~0xFFu) | (uint8_t)(color << 4);
		byte_color |= color;
		column_cursor = top;
		rows_to_fill = rows_left;
		top++;
		do
		{
			mem_write8(column_cursor, byte_color);
			column_cursor += row_pitch_4bpp;
			rows_to_fill--;
		} while (rows_to_fill != 0u);
		pixel_columns -= 2u;
	}
	if (pixel_columns >= 4u)
	{
		color = (color & ~0xFFFFu) | (uint16_t)(color * 0x1111u);
		rows_to_fill = rows_left;
		block_cursor = (uint16_t)(rows_left << 8) + top;
		blocks_per_row = pixel_columns >> 2;
		do
		{
			lower_row_cursor = blocks_per_row;
			block_cursor -= row_pitch_4bpp;
			do
			{
				mem_write16(block_cursor, color);
				block_cursor += 2u;
				lower_row_cursor--;
			} while (lower_row_cursor != 0u);
			rows_to_fill--;
		} while (rows_to_fill != 0u);
		top = block_cursor;
		tail_pixels = pixel_columns & 3u;
		pixel_columns = tail_pixels;
		if (tail_pixels == 0u)
		{
			*p_left = left;
			*p_top = top;
			*p_color = color;
			*p_inner_count = rows_left;
			*p_row_pairs_left = row_pitch_4bpp;
			*p_blocks_per_row = blocks_per_row;
			*p_lower_row_cursor = lower_row_cursor;
			return;
		}
	}
	color = (color & ~0xFFFFu) | (color & 0xFu);
	color = (color & ~0xFFFFu) | (uint16_t)(color * 0x1111u);
	tail_keep_mask = shift_by(0xFFFFu, (uint8_t)(pixel_columns * 2u * 2u), 16);
	left = (left & ~0xFFFFu) | (uint16_t)~tail_keep_mask;
	color = (color & ~0xFFFFu) | (uint16_t)(color & left);
	do
	{
		left = (left & ~0xFFFFu) | mem_read16(top);
		left = (left & ~0xFFFFu) | (left & tail_keep_mask);
		left = (left & ~0xFFFFu) | (uint16_t)(left | color);
		mem_write16(top, left);
		top += row_pitch_4bpp;
		rows_left--;
	} while (rows_left != 0u);
	*p_left = left;
	*p_top = top;
	*p_color = color;
	*p_inner_count = rows_left;
	*p_row_pairs_left = row_pitch_4bpp;
	*p_blocks_per_row = blocks_per_row;
	*p_lower_row_cursor = lower_row_cursor;
}
