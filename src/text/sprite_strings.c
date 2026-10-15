/**
 *    Name:        text/sprite_strings.c
 *    Description: strings drawn with sprites: the string queue and draw_queued_strings
 *    Version:     0.1.0
 *    Created:     12-10-2026
 *    Modified:    15-10-2026
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
		x = mem_read16(text);
		text += 2u;
		y = mem_read16(text);
		text += 2u;
		palette = mem_read16(text);
		text += 2u;
		scale = mem_read16(text);
		text += 2u;
	}
	entry = (uint16_t)string_count * 0xCu + string_queue_ADDRESS;
	mem_write16(entry, x);
	entry += 2u;
	mem_write16(entry, y);
	entry += 2u;
	mem_write16(entry, palette);
	entry += 2u;
	mem_write16(entry, scale);
	mem_write32(entry + 2u, text);
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
		char_x = (uint16_t)(int16_t)mem_read16(cursor);
		cursor += 2u;
		string_y = (uint16_t)(int16_t)mem_read16(cursor);
		cursor += 2u;
		palette = mem_read16(cursor);
		cursor += 2u;
		scale = mem_read16(cursor);
		text = mem_read32(cursor + 2u);
		char_code = mem_read8(text);
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
					size = mem_read16(frame_record);
					frame_record += 2u;
					height = mem_read16(frame_record);
					width = (uint16_t)shift_by(size * scale + rounding, shift, 32);
					advance = (uint16_t)shift_by((uint8_t)height * scale + rounding, shift, 32);
					size = (uint16_t)(((uint16_t)((uint32_t)size << 6) & ~0xFFu) | (uint8_t)height);
					height = (uint16_t)((height & ~0xFFu) | advance);
					size = rotate_by(size, shift, 16);
					frame_record += 2u;
					width |= mem_read16(frame_record);
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
				mem_write16(sprite_entry, mem_read16(frame_record));
				sprite_entry += 2u;
				mem_write16(sprite_entry, palette);
				sprite_entry += 2u;
			}
			else
				leading_blanks++;
			char_x += advance;
			char_code = mem_read8(text);
			text++;
		} while (char_code != 0u);
	}
	string_count = 0;
	mem_write16(sprite_entry, sprite_list_end_command);
	*p_scale = scale;
	*p_cursor = cursor;
	*p_height = (*p_height & 0xFFFF0000u) | height;
}
