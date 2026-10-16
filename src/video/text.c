/**
 *    Name:        video/text.c
 *    Description: text: the text layer's font, printing and erasing text, lines, fractions and digits, and the strings drawn with sprites
 *    Version:     0.1.0
 *    Created:     12-10-2026
 *    Modified:    16-10-2026
 *    Author:      Sebastian Toyos
 *
 *    SegaSonic The Hedgehog (arcade, Sega System 32): the game's V60 program decompiled into C by
 *    tools/decompile.py, with the names of symbols.txt.
 */
#include "game.h"

void queue_string_record(uint32_t record, uint16_t y, uint32_t scale, uint32_t *p_palette, uint32_t *p_text)
{
	uint32_t palette = *p_palette;
	uint32_t text = *p_text;
	uint8_t mode = 0;

	mode = 0u;
	queue_string(record, y, mode, &palette, &scale, &text);
	*p_palette = (*p_palette & 0xFFFF0000u) | (palette & 0xFFFFu);
	*p_text = text;
}

void queue_string_at(uint32_t x, uint16_t y, uint32_t *p_palette, uint32_t *p_scale, uint32_t *p_text)
{
	uint32_t palette = *p_palette;
	uint32_t scale = *p_scale;
	uint32_t text = *p_text;
	uint8_t mode = 0;

	mode = 1u;
	queue_string(x, y, mode, &palette, &scale, &text);
	*p_palette = (*p_palette & 0xFFFF0000u) | (palette & 0xFFFFu);
	*p_scale = (*p_scale & 0xFFFF0000u) | (scale & 0xFFFFu);
	*p_text = text;
}

void queue_string(uint32_t x, uint16_t y, uint8_t mode, uint32_t *p_palette, uint32_t *p_scale, uint32_t *p_text)
{
	uint16_t palette = (uint16_t)*p_palette;
	uint16_t scale = (uint16_t)*p_scale;
	uint32_t text = *p_text;
	uint32_t entry = 0;

	if (string_count > 0x2A)
		return;
	if ((uint8_t)mode == 0u)
	{
		text = x;
		x = STRING_RECORD(text)->x;
		text += 2u;
		y = STRING_RECORD(text - 0x2u)->y;
		text += 2u;
		palette = STRING_RECORD(text - 0x4u)->palette;
		text += 2u;
		scale = STRING_RECORD(text - 0x6u)->scale;
		text += 2u;
	}
	entry = (uint16_t)string_count * 0xCu + string_queue_ADDRESS;
	QUEUED_STRING(entry)->x = x;
	entry += 2u;
	QUEUED_STRING(entry - 0x2u)->y = y;
	entry += 2u;
	QUEUED_STRING(entry - 0x4u)->palette = palette;
	entry += 2u;
	QUEUED_STRING(entry - 0x6u)->scale = scale;
	QUEUED_STRING(entry - 0x6u)->text = text;
	string_count++;
	*p_palette = (*p_palette & 0xFFFF0000u) | palette;
	*p_scale = (*p_scale & 0xFFFF0000u) | scale;
	*p_text = text;
}

void draw_queued_strings(uint16_t size, uint16_t width, uint32_t sprite_entry, uint32_t *p_scale, uint32_t *p_cursor, uint32_t *p_height)
{
	uint32_t scale = *p_scale;
	uint16_t height = (uint16_t)*p_height;
	uint8_t shift = 0;
	uint16_t advance = 0, char_x = 0, palette = 0, string_y = 0;
	uint32_t char_code = 0, cursor = 0, frame_record = 0, leading_blanks = 0, rounding = 0, text = 0;

	for (;;)
	{
		string_count--;
		cursor = (uint16_t)string_count;
		if ((int16_t)cursor < 0)
			break;
		advance = 0u;
		leading_blanks = 0u;
		rounding = 0x80u;
		shift = 0xF8u;
		frame_record = sprite_frames;
		cursor = cursor * 0xCu + string_queue_ADDRESS;
		char_x = (uint16_t)QUEUED_STRING(cursor)->x;
		cursor += 2u;
		string_y = (uint16_t)QUEUED_STRING(cursor - 0x2u)->y;
		cursor += 2u;
		palette = QUEUED_STRING(cursor - 0x4u)->palette;
		cursor += 2u;
		scale = QUEUED_STRING(cursor - 0x6u)->scale;
		text = QUEUED_STRING(cursor - 0x6u)->text;
		char_code = ROM8(text);
		r14 = 0u;
		text++;
		do
		{
			frame_record = (frame_record & ~0xFFFFu) | text_frames[char_code];
			if ((uint16_t)frame_record != 0u)
			{
				if (r14 == 0u)
				{
					r14 = 1u;
					size = SPRITE_FRAME_RECORD(frame_record)->width;
					frame_record += 2u;
					height = SPRITE_FRAME_RECORD(frame_record - 0x2u)->height;
					width = (uint16_t)shift_by(size * scale + rounding, shift, 32);
					advance = (uint16_t)shift_by((uint8_t)height * scale + rounding, shift, 32);
					size = (uint16_t)(((uint16_t)((uint32_t)size << 6) & ~0xFFu) | (uint8_t)height);
					height = (uint16_t)((height & ~0xFFu) | advance);
					size = rotate_by(size, shift, 16);
					frame_record += 2u;
					width |= SPRITE_FRAME_RECORD(frame_record - 0x4u)->bank_bits;
					frame_record += 2u;
					if (leading_blanks != 0u)
						char_x += leading_blanks * advance;
				}
				else
					frame_record += 6u;
				leading_blanks = 0u;
				mem_write16(sprite_entry, 0xAu);
				sprite_entry += 2u;
				mem_write16(sprite_entry, size);
				sprite_entry += 2u;
				mem_write16(sprite_entry, height);
				sprite_entry += 2u;
				mem_write16(sprite_entry, width);
				sprite_entry += 2u;
				mem_write16(sprite_entry, string_y);
				sprite_entry += 2u;
				mem_write16(sprite_entry, char_x);
				sprite_entry += 2u;
				mem_write16(sprite_entry, SPRITE_FRAME_RECORD(frame_record - 0x6u)->address);
				sprite_entry += 2u;
				mem_write16(sprite_entry, palette);
				sprite_entry += 2u;
			}
			else
				leading_blanks++;
			char_x += advance;
			char_code = ROM8(text);
			text++;
		} while (char_code != 0u);
	}
	string_count = 0;
	mem_write16(sprite_entry, sprite_list_end_command);
	*p_scale = scale;
	*p_cursor = cursor;
	*p_height = (*p_height & 0xFFFF0000u) | height;
}

void load_text_font(uint32_t *p_pixels, uint32_t *p_color_index)
{
	uint8_t color = 0;
	uint16_t count = 0;
	uint32_t color_index = 0, dest = 0, dest_end = 0, ignored = 0, pixels = 0, source = 0, source_end = 0;

	mem_write16(video_text_control, 0x1F7u);
	move_string(text_font_ADDRESS, 0x9A0u, VIDEO_RAM + 0x1C000u, 0x9A0u, 2u, 0u, 0u, fill_value, &dest_end, &source_end);
	source = source_end;
	dest = dest_end;
	count = 0x5Bu;
	color = 1u;
	expand_font(color, count, source, dest, &pixels, &color_index);
	clear_text_layer(&ignored, &ignored, &ignored);
	*p_pixels = pixels;
	*p_color_index = color_index;
}

void clear_text_layer(uint32_t *p_value, uint32_t *p_count, uint32_t *p_dest)
{
	uint32_t count = 0, dest = 0, value = 0;

	dest = TEXT_CELL(0, 0);
	count = 0xE0u;
	clear_blocks(&value, &count, &dest);
	*p_value = value;
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
			font_row = ROM8(source);
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
		font_row = ROM16(source);
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
		font_row = ROM16(source);
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

void draw_fraction(uint16_t attribute, uint32_t *p_numerator, uint32_t *p_denominator, uint32_t *p_text_position)
{
	uint16_t numerator = (uint16_t)*p_numerator;
	uint16_t denominator = (uint16_t)*p_denominator;
	uint32_t text_position = *p_text_position;

	attribute = (uint16_t)((uint32_t)attribute << 8);
	numerator &= 0xFu;
	denominator &= 0xFu;
	numerator = (uint8_t)(numerator - 0x80u) | attribute;
	denominator = (uint8_t)(denominator - 0x70u) | attribute;
	mem_write16(text_position, numerator);
	text_position += 2u;
	mem_write16(text_position, denominator);
	*p_numerator = (*p_numerator & 0xFFFF0000u) | numerator;
	*p_denominator = (*p_denominator & 0xFFFF0000u) | denominator;
	*p_text_position = text_position;
}

void print_text_list(uint32_t *p_text, uint32_t *p_text_position)
{
	uint32_t text = *p_text;
	uint32_t text_position = *p_text_position;

	while (ROM16(text) != 0u)
	{
		print_text(&text, &text_position);
	}
	*p_text = text;
	*p_text_position = text_position;
}

void print_text(uint32_t *p_text, uint32_t *p_text_position)
{
	uint32_t text = *p_text;
	uint32_t ignored = 0, text_position = 0;

	text_position = 0x310000u | ROM16(text);
	text += 2u;
	print_text_here(&ignored, &text, &text_position);
	*p_text = text;
	*p_text_position = text_position;
}

void print_text_here(uint32_t *p_cell, uint32_t *p_text, uint32_t *p_text_position)
{
	uint32_t text = *p_text;
	uint32_t text_position = *p_text_position;
	uint16_t cell = 0;
	uint8_t next_char = 0;

	cell = (uint16_t)((uint32_t)ROM8(text) << 8);
	text++;
	cell |= ROM8(text);
	text++;
	if ((uint8_t)cell == 0u)
	{
		*p_cell = (*p_cell & 0xFFFF0000u) | cell;
		*p_text = text;
		return;
	}
	do
	{
		mem_write16(text_position, cell);
		next_char = ROM8(text);
		text++;
		cell = (uint16_t)((cell & ~0xFFu) | next_char);
		text_position += 2u;
	} while ((uint8_t)cell != 0u);
	*p_cell = (*p_cell & 0xFFFF0000u) | cell;
	*p_text = text;
	*p_text_position = text_position;
}

void erase_text_list(uint32_t text)
{
	while (ROM16(text) != 0u)
	{
		text = erase_text(text);
	}
}

uint32_t erase_text(uint32_t text)
{
	uint16_t cell = 0;
	uint32_t text_position = 0;

	cell = 0u;
	text_position = 0x310000u | ROM16(text);
	text += 2u;
	erase_text_here(cell, &text, &text_position);
	return text;
}

void erase_text_here(uint16_t cell, uint32_t *p_text, uint32_t *p_text_position)
{
	uint32_t text = *p_text;
	uint32_t text_position = *p_text_position;
	uint8_t first_char = 0, next_char = 0;

	text++;
	first_char = ROM8(text);
	text++;
	if (first_char == 0u)
	{
		*p_text = text;
		return;
	}
	do
	{
		mem_write16(text_position, cell);
		next_char = ROM8(text);
		text++;
		text_position += 2u;
	} while (next_char != 0u);
	*p_text = text;
	*p_text_position = text_position;
}

void print_text_lines(uint32_t *p_text, uint32_t *p_text_position)
{
	uint32_t text = *p_text;
	uint32_t text_position = *p_text_position;
	uint16_t text_cell = 0;
	uint32_t line_start = 0;

	line_start = text_position;
	text_cell = (uint16_t)((uint32_t)ROM8(text) << 8);
	text++;
	for (;;)
	{
		text_cell = (uint16_t)((text_cell & ~0xFFu) | ROM8(text));
		text++;
		if ((uint8_t)text_cell == 0u)
			break;
		if ((int8_t)text_cell >= 0)
		{
			mem_write16(text_position, text_cell);
			text_position += 2u;
			continue;
		}
		line_start += 0x80u;
		text_position = line_start;
	}
	*p_text = text;
	*p_text_position = text_position;
}

void erase_text_lines(uint32_t *p_text, uint32_t *p_text_position)
{
	uint32_t text = *p_text;
	uint32_t text_position = *p_text_position;
	uint32_t negative = 0;
	uint16_t blank_cell = 0;
	uint32_t line_start = 0;
	uint8_t text_char = 0;

	line_start = text_position;
	blank_cell = 0u;
	text++;
	for (;;)
	{
		text_char = ROM8(text);
		negative = text_char >> 7;
		text++;
		if (text_char == 0u)
			break;
		if (negative == 0u)
		{
			mem_write16(text_position, blank_cell);
			text_position += 2u;
			continue;
		}
		line_start += 0x80u;
		text_position = line_start;
	}
	*p_text = text;
	*p_text_position = text_position;
}

void print_formatted_text(uint32_t text)
{
	uint32_t text_position = 0;

	text_position = TEXT_CELL(0, 0);
	formatted_text_position_single(text, text_position);
}

void formatted_text_position_single(uint32_t text, uint32_t text_position)
{
	text_position = (text_position & ~0xFFFFu) | ROM16(text);
	text += 2u;
	formatted_text_attribute_single(text, text_position);
}

void formatted_text_attribute_single(uint32_t text, uint32_t text_position)
{
	uint16_t cell = 0;

	cell = (uint16_t)((uint32_t)ROM8(text) << 8);
	text++;
	formatted_text_single(cell, text, text_position);
}

void formatted_text_single(uint16_t cell, uint32_t text, uint32_t text_position)
{
	uint32_t zero = 0;
	uint32_t code = 0;
	uint8_t next_char = 0;

	for (;;)
	{
		cell = (uint16_t)((cell & ~0xFFu) | ROM8(text));
		zero = (uint8_t)cell == 0u;
		text++;
		if ((uint8_t)cell != 0u)
		{
			do
			{
				mem_write16(text_position, cell);
				next_char = ROM8(text);
				text++;
				cell = (uint16_t)((cell & ~0xFFu) | next_char);
				zero = (uint8_t)cell == 0u;
				text_position += 2u;
			} while ((uint8_t)cell != 0u);
		}
		code = ROM8(text);
		text++;
		switch (code)
		{
		case 0x0u:
			break;
		case 0x3u:
			continue;
		case 0x6u:
			formatted_text_double(cell, text, text_position);
			return;
		case 0x9u:
			formatted_text_triple(cell, text, text_position);
			return;
		case 0xCu:
		case 0x12u:
			formatted_text_attribute_single(text, text_position);
			return;
		case 0xFu:
			formatted_text_position_single(text, text_position);
			return;
		case 0x15u:
			formatted_text_position_double(text, text_position);
			return;
		case 0x18u:
			formatted_text_attribute_double(text, text_position);
			return;
		case 0x1Bu:
			formatted_text_position_triple(text, text_position);
			return;
		case 0x1Eu:
			formatted_text_attribute_triple(text, text_position);
			return;
		case 0x21u:
			formatted_text_skip_single(cell, text, text_position);
			return;
		case 0x24u:
			formatted_text_skip_long_single(cell, text, text_position);
			return;
		default:
			invalid_jump(0x996A7u + code);
		}
		if (zero)
			return;
	}
}

void formatted_text_skip_single(uint16_t cell, uint32_t text, uint32_t text_position)
{
	text_position += (int8_t)ROM8(text) * 2u;
	text++;
	formatted_text_single(cell, text, text_position);
}

void formatted_text_skip_long_single(uint16_t cell, uint32_t text, uint32_t text_position)
{
	text_position += (int16_t)ROM16(text) * 2u;
	text += 2u;
	formatted_text_single(cell, text, text_position);
}

void formatted_text_position_double(uint32_t text, uint32_t text_position)
{
	text_position = (text_position & ~0xFFFFu) | ROM16(text);
	text += 2u;
	formatted_text_attribute_double(text, text_position);
}

void formatted_text_attribute_double(uint32_t text, uint32_t text_position)
{
	uint16_t cell = 0;

	cell = (uint16_t)((uint32_t)ROM8(text) << 8);
	text++;
	formatted_text_double(cell, text, text_position);
}

void formatted_text_double(uint16_t cell, uint32_t text, uint32_t text_position)
{
	uint32_t zero = 0;
	uint32_t code = 0;
	uint8_t next_char = 0;

	for (;;)
	{
		cell = (uint16_t)((cell & ~0xFFu) | ROM8(text));
		zero = (uint8_t)cell == 0u;
		text++;
		if ((uint8_t)cell != 0u)
		{
			do
			{
				mem_write16(text_position, cell);
				cell++;
				text_position += 2u;
				mem_write16(text_position, cell);
				next_char = ROM8(text);
				text++;
				cell = (uint16_t)((cell & ~0xFFu) | next_char);
				zero = (uint8_t)cell == 0u;
				text_position += 2u;
			} while ((uint8_t)cell != 0u);
		}
		code = ROM8(text);
		text++;
		switch (code)
		{
		case 0x0u:
			if (zero)
				return;
			formatted_text_single(cell, text, text_position);
			return;
		case 0x3u:
			formatted_text_single(cell, text, text_position);
			return;
		case 0x6u:
			continue;
		case 0x9u:
			formatted_text_triple(cell, text, text_position);
			return;
		case 0xCu:
		case 0x18u:
			formatted_text_attribute_double(text, text_position);
			return;
		case 0xFu:
			formatted_text_position_single(text, text_position);
			return;
		case 0x12u:
			formatted_text_attribute_single(text, text_position);
			return;
		case 0x15u:
			formatted_text_position_double(text, text_position);
			return;
		case 0x1Bu:
			formatted_text_position_triple(text, text_position);
			return;
		case 0x1Eu:
			formatted_text_attribute_triple(text, text_position);
			return;
		case 0x21u:
			formatted_text_skip_double(cell, text, text_position);
			return;
		case 0x24u:
			formatted_text_skip_long_double(cell, text, text_position);
			return;
		default:
			invalid_jump(0x99703u + code);
		}
	}
}

void formatted_text_skip_double(uint16_t cell, uint32_t text, uint32_t text_position)
{
	text_position += (int8_t)ROM8(text) * 2u;
	text++;
	formatted_text_double(cell, text, text_position);
}

void formatted_text_skip_long_double(uint16_t cell, uint32_t text, uint32_t text_position)
{
	text_position += (int16_t)ROM16(text) * 2u;
	text += 2u;
	formatted_text_double(cell, text, text_position);
}

void formatted_text_position_triple(uint32_t text, uint32_t text_position)
{
	text_position = (text_position & ~0xFFFFu) | ROM16(text);
	text += 2u;
	formatted_text_attribute_triple(text, text_position);
}

void formatted_text_attribute_triple(uint32_t text, uint32_t text_position)
{
	uint16_t cell = 0;

	cell = (uint16_t)((uint32_t)ROM8(text) << 8);
	text++;
	formatted_text_triple(cell, text, text_position);
}

void formatted_text_triple(uint16_t cell, uint32_t text, uint32_t text_position)
{
	uint32_t zero = 0;
	uint32_t code = 0;
	uint8_t next_char = 0;

	for (;;)
	{
		cell = (uint16_t)((cell & ~0xFFu) | ROM8(text));
		zero = (uint8_t)cell == 0u;
		text++;
		if ((uint8_t)cell != 0u)
		{
			do
			{
				mem_write16(text_position, cell);
				cell++;
				text_position += 2u;
				mem_write16(text_position, cell);
				cell++;
				text_position += 2u;
				mem_write16(text_position, cell);
				next_char = ROM8(text);
				text++;
				cell = (uint16_t)((cell & ~0xFFu) | next_char);
				zero = (uint8_t)cell == 0u;
				text_position += 2u;
			} while ((uint8_t)cell != 0u);
		}
		code = ROM8(text);
		text++;
		switch (code)
		{
		case 0x0u:
			if (zero)
				return;
			formatted_text_single(cell, text, text_position);
			return;
		case 0x3u:
			formatted_text_single(cell, text, text_position);
			return;
		case 0x6u:
			formatted_text_double(cell, text, text_position);
			return;
		case 0x9u:
			continue;
		case 0xCu:
		case 0x1Eu:
			formatted_text_attribute_triple(text, text_position);
			return;
		case 0xFu:
			formatted_text_position_single(text, text_position);
			return;
		case 0x12u:
			formatted_text_attribute_single(text, text_position);
			return;
		case 0x15u:
			formatted_text_position_double(text, text_position);
			return;
		case 0x18u:
			formatted_text_attribute_double(text, text_position);
			return;
		case 0x1Bu:
			formatted_text_position_triple(text, text_position);
			return;
		case 0x21u:
			formatted_text_skip_triple(cell, text, text_position);
			return;
		case 0x24u:
			formatted_text_skip_long_triple(cell, text, text_position);
			return;
		default:
			invalid_jump(0x99764u + code);
		}
	}
}

void formatted_text_skip_triple(uint16_t cell, uint32_t text, uint32_t text_position)
{
	text_position += (int8_t)ROM8(text) * 2u;
	text++;
	formatted_text_triple(cell, text, text_position);
}

void formatted_text_skip_long_triple(uint16_t cell, uint32_t text, uint32_t text_position)
{
	text_position += (int16_t)ROM16(text) * 2u;
	text += 2u;
	formatted_text_triple(cell, text, text_position);
}

void erase_formatted_text(uint32_t cell, uint32_t text)
{
	uint32_t text_position = 0;

	text_position = TEXT_CELL(0, 0);
	cell &= ~0xFFFFu;
	erase_formatted_text_position_single(cell, text, text_position);
}

void erase_formatted_text_position_single(uint32_t cell, uint32_t text, uint32_t text_position)
{
	text_position = (text_position & ~0xFFFFu) | ROM16(text);
	text += 2u;
	erase_formatted_text_attribute_single(cell, text, text_position);
}

void erase_formatted_text_attribute_single(uint32_t cell, uint32_t text, uint32_t text_position)
{
	text++;
	erase_formatted_text_single(cell, text, text_position);
}

void erase_formatted_text_single(uint32_t cell, uint32_t text, uint32_t text_position)
{
	uint32_t zero = 0;
	uint32_t code = 0;
	uint8_t first_char = 0, next_char = 0;

	for (;;)
	{
		first_char = ROM8(text);
		zero = first_char == 0u;
		text++;
		if (first_char != 0u)
		{
			do
			{
				mem_write16(text_position, cell);
				next_char = ROM8(text);
				text++;
				zero = next_char == 0u;
				text_position += 2u;
			} while (next_char != 0u);
		}
		code = ROM8(text);
		text++;
		switch (code)
		{
		case 0x0u:
			break;
		case 0x3u:
			continue;
		case 0x6u:
			erase_formatted_text_double(cell, text, text_position);
			return;
		case 0x9u:
			erase_formatted_text_triple(cell, text, text_position);
			return;
		case 0xCu:
		case 0x12u:
			erase_formatted_text_attribute_single(cell, text, text_position);
			return;
		case 0xFu:
			erase_formatted_text_position_single(cell, text, text_position);
			return;
		case 0x15u:
			erase_formatted_text_position_double(cell, text, text_position);
			return;
		case 0x18u:
			erase_formatted_text_attribute_double(cell, text, text_position);
			return;
		case 0x1Bu:
			erase_formatted_text_position_triple(cell, text, text_position);
			return;
		case 0x1Eu:
			erase_formatted_text_attribute_triple(cell, text, text_position);
			return;
		case 0x21u:
			erase_formatted_text_skip_single(cell, text, text_position);
			return;
		default:
			invalid_jump(0x997BCu + code);
		}
		if (zero)
			return;
	}
}

void erase_formatted_text_skip_single(uint32_t cell, uint32_t text, uint32_t text_position)
{
	text_position += (int8_t)ROM8(text) * 2u;
	text++;
	erase_formatted_text_single(cell, text, text_position);
}

void erase_formatted_text_position_double(uint32_t cell, uint32_t text, uint32_t text_position)
{
	text_position = (text_position & ~0xFFFFu) | ROM16(text);
	text += 2u;
	erase_formatted_text_attribute_double(cell, text, text_position);
}

void erase_formatted_text_attribute_double(uint32_t cell, uint32_t text, uint32_t text_position)
{
	text++;
	erase_formatted_text_double(cell, text, text_position);
}

void erase_formatted_text_double(uint32_t cell, uint32_t text, uint32_t text_position)
{
	uint32_t zero = 0;
	uint32_t code = 0;
	uint8_t first_char = 0, next_char = 0;

	for (;;)
	{
		first_char = ROM8(text);
		zero = first_char == 0u;
		text++;
		if (first_char != 0u)
		{
			do
			{
				mem_write32(text_position, cell);
				next_char = ROM8(text);
				text++;
				zero = next_char == 0u;
				text_position += 4u;
			} while (next_char != 0u);
		}
		code = ROM8(text);
		text++;
		switch (code)
		{
		case 0x0u:
			if (zero)
				return;
			erase_formatted_text_single(cell, text, text_position);
			return;
		case 0x3u:
			erase_formatted_text_single(cell, text, text_position);
			return;
		case 0x6u:
			continue;
		case 0x9u:
			erase_formatted_text_triple(cell, text, text_position);
			return;
		case 0xCu:
		case 0x18u:
			erase_formatted_text_attribute_double(cell, text, text_position);
			return;
		case 0xFu:
			erase_formatted_text_position_single(cell, text, text_position);
			return;
		case 0x12u:
			erase_formatted_text_attribute_single(cell, text, text_position);
			return;
		case 0x15u:
			erase_formatted_text_position_double(cell, text, text_position);
			return;
		case 0x1Bu:
			erase_formatted_text_position_triple(cell, text, text_position);
			return;
		case 0x1Eu:
			erase_formatted_text_attribute_triple(cell, text, text_position);
			return;
		case 0x21u:
			erase_formatted_text_skip_double(cell, text, text_position);
			return;
		default:
			invalid_jump(0x997FDu + code);
		}
	}
}

void erase_formatted_text_skip_double(uint32_t cell, uint32_t text, uint32_t text_position)
{
	text_position += (int8_t)ROM8(text) * 2u;
	text++;
	erase_formatted_text_double(cell, text, text_position);
}

void erase_formatted_text_position_triple(uint32_t cell, uint32_t text, uint32_t text_position)
{
	text_position = (text_position & ~0xFFFFu) | ROM16(text);
	text += 2u;
	erase_formatted_text_attribute_triple(cell, text, text_position);
}

void erase_formatted_text_attribute_triple(uint32_t cell, uint32_t text, uint32_t text_position)
{
	text++;
	erase_formatted_text_triple(cell, text, text_position);
}

void erase_formatted_text_triple(uint32_t cell, uint32_t text, uint32_t text_position)
{
	uint32_t zero = 0;
	uint32_t code = 0;
	uint8_t first_char = 0, next_char = 0;

	for (;;)
	{
		first_char = ROM8(text);
		zero = first_char == 0u;
		text++;
		if (first_char != 0u)
		{
			do
			{
				mem_write32(text_position, cell);
				text_position += 4u;
				mem_write16(text_position, cell);
				next_char = ROM8(text);
				text++;
				zero = next_char == 0u;
				text_position += 2u;
			} while (next_char != 0u);
		}
		code = ROM8(text);
		text++;
		switch (code)
		{
		case 0x0u:
			if (zero)
				return;
			erase_formatted_text_single(cell, text, text_position);
			return;
		case 0x3u:
			erase_formatted_text_single(cell, text, text_position);
			return;
		case 0x6u:
			erase_formatted_text_double(cell, text, text_position);
			return;
		case 0x9u:
			continue;
		case 0xCu:
		case 0x1Eu:
			erase_formatted_text_attribute_triple(cell, text, text_position);
			return;
		case 0xFu:
			erase_formatted_text_position_single(cell, text, text_position);
			return;
		case 0x12u:
			erase_formatted_text_attribute_single(cell, text, text_position);
			return;
		case 0x15u:
			erase_formatted_text_position_double(cell, text, text_position);
			return;
		case 0x18u:
			erase_formatted_text_attribute_double(cell, text, text_position);
			return;
		case 0x1Bu:
			erase_formatted_text_position_triple(cell, text, text_position);
			return;
		case 0x21u:
			erase_formatted_text_skip_triple(cell, text, text_position);
			return;
		default:
			invalid_jump(0x99841u + code);
		}
	}
}

void erase_formatted_text_skip_triple(uint32_t cell, uint32_t text, uint32_t text_position)
{
	text_position += (int8_t)ROM8(text) * 2u;
	text++;
	erase_formatted_text_triple(cell, text, text_position);
}

uint32_t draw_hex_2(uint32_t value, uint32_t attribute, uint32_t text_position)
{
	uint32_t digit_count = 0;

	value = (uint8_t)value;
	digit_count = 2u;
	draw_hex_digits(digit_count, &value, &attribute, &text_position);
	return value;
}

void draw_hex_4(uint32_t *p_value, uint32_t *p_attribute, uint32_t *p_text_position)
{
	uint32_t value = *p_value;
	uint32_t attribute = *p_attribute;
	uint32_t text_position = *p_text_position;
	uint32_t digit_count = 0;

	value = (uint16_t)value;
	digit_count = 4u;
	draw_hex_digits(digit_count, &value, &attribute, &text_position);
	*p_value = value;
	*p_attribute = (*p_attribute & 0xFFFFFF00u) | (attribute & 0xFFu);
	*p_text_position = text_position;
}

void draw_hex_6(uint32_t value, uint32_t attribute, uint32_t text_position)
{
	uint32_t digit_count = 0;

	value &= 0xFFFFFFu;
	digit_count = 6u;
	draw_hex_digits(digit_count, &value, &attribute, &text_position);
}

void draw_hex_8(uint32_t value, uint32_t attribute, uint32_t text_position)
{
	uint32_t digit_count = 0;

	digit_count = 8u;
	draw_hex_digits(digit_count, &value, &attribute, &text_position);
}

void draw_hex_digits(uint32_t digit_count, uint32_t *p_value, uint32_t *p_attribute, uint32_t *p_text_position)
{
	uint32_t value = *p_value;
	uint16_t attribute = (uint16_t)*p_attribute;
	uint32_t text_position = *p_text_position;
	uint32_t remaining = 0;

	text_position += digit_count * 2u;
	do
	{
		attribute = (uint16_t)((attribute & ~0xFFu) | (uint8_t)value);
		attribute = (uint16_t)((attribute & ~0xFFu) | (attribute & 0xFu));
		if ((uint8_t)attribute >= 0xAu)
			attribute = (uint16_t)((attribute & ~0xFFu) | (uint8_t)(attribute + 7u));
		attribute = (uint16_t)((attribute & ~0xFFu) | (uint8_t)(attribute + 0x30u));
		text_position -= 2u;
		mem_write16(text_position, attribute);
		remaining = value >> 4;
		value = remaining;
		digit_count--;
	} while ((digit_count != 0u) & (remaining != 0u));
	if (digit_count == 0u)
	{
		*p_value = value;
		*p_attribute = (*p_attribute & 0xFFFFFF00u) | (attribute & 0xFFu);
		*p_text_position = text_position;
		return;
	}
	do
	{
		text_position -= 2u;
		mem_write16(text_position, value);
		digit_count--;
	} while (digit_count != 0u);
	*p_value = value;
	*p_attribute = (*p_attribute & 0xFFFFFF00u) | (attribute & 0xFFu);
	*p_text_position = text_position;
}

void draw_credit_digits(uint32_t value, uint32_t *p_attribute, uint32_t *p_text_position)
{
	uint32_t attribute = *p_attribute;
	uint32_t text_position = *p_text_position;
	uint32_t digits_left = 0;

	value = (uint8_t)value;
	digits_left = 2u;
	draw_bcd_digits(value, digits_left, &attribute, &text_position);
	*p_attribute = (*p_attribute & 0xFFFFFF00u) | (attribute & 0xFFu);
	*p_text_position = text_position;
}

void draw_bcd_4(uint32_t value, uint32_t attribute, uint32_t text_position)
{
	uint32_t digit_count = 0;

	value = (uint16_t)value;
	digit_count = 4u;
	draw_bcd_digits(value, digit_count, &attribute, &text_position);
}

void draw_bcd_6(uint32_t value, uint32_t attribute, uint32_t text_position)
{
	uint32_t digit_count = 0;

	value &= 0xFFFFFFu;
	digit_count = 6u;
	draw_bcd_digits(value, digit_count, &attribute, &text_position);
}

void draw_bcd_8(uint32_t value, uint32_t attribute, uint32_t text_position)
{
	uint32_t digit_count = 0;

	digit_count = 8u;
	draw_bcd_digits(value, digit_count, &attribute, &text_position);
}

void draw_bcd_digits(uint32_t value, uint32_t digit_count, uint32_t *p_attribute, uint32_t *p_text_position)
{
	uint16_t attribute = (uint16_t)*p_attribute;
	uint32_t text_position = *p_text_position;
	uint8_t shift = 0;
	uint32_t remaining = 0;

	text_position += digit_count * 2u;
	shift = 0xFCu;
	do
	{
		attribute = (uint16_t)((attribute & ~0xFFu) | (uint8_t)value);
		attribute = (uint16_t)((attribute & ~0xFFu) | (attribute & 0xFu));
		attribute = (uint16_t)((attribute & ~0xFFu) | (uint8_t)(attribute + 1u));
		text_position -= 2u;
		mem_write16(text_position, attribute);
		remaining = shift_by(value, shift, 32);
		value = remaining;
		digit_count--;
	} while ((digit_count != 0u) & (remaining != 0u));
	if (digit_count == 0u)
	{
		*p_attribute = (*p_attribute & 0xFFFFFF00u) | (attribute & 0xFFu);
		*p_text_position = text_position;
		return;
	}
	do
	{
		text_position -= 2u;
		mem_write16(text_position, value);
		digit_count--;
	} while (digit_count != 0u);
	*p_attribute = (*p_attribute & 0xFFFFFF00u) | (attribute & 0xFFu);
	*p_text_position = text_position;
}
