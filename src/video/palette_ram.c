/**
 *    Name:        video/palette_ram.c
 *    Description: palette RAM: load_palette_list and its copies, queue_vblank_copy
 *    Version:     0.1.0
 *    Created:     12-10-2026
 *    Modified:    15-10-2026
 *    Author:      Sebastian Toyos
 *
 *    SegaSonic The Hedgehog (arcade, Sega System 32): the game's V60 program decompiled into C by
 *    tools/decompile.py, with the names of symbols.txt.
 */
#include "game.h"

uint32_t load_default_palettes(void)
{
	uint32_t r11 = 0, ignored = 0, palette_list = 0;

	vblank_copies_pending = 0u;
	vblank_copy_count = 0u;
	palette_list = default_palette_list_ADDRESS;
	load_palette_list(palette_list, &ignored, &r11);
	return r11;
}

void load_palette_list(uint32_t palette_list, uint32_t *p_source, uint32_t *p_dest)
{
	uint32_t copy_mode = 0, dest = 0, entries_left = 0, size = 0, source = 0;

	entries_left = mem_read16(palette_list);
	dest = 0x600000u;
	palette_list += 2u;
	do
	{
		source = mem_read32(palette_list);
		palette_list += 4u;
		dest = (dest & ~0xFFFFu) | mem_read16(palette_list);
		palette_list += 2u;
		copy_mode = mem_read16(palette_list);
		palette_list += 2u;
		size = mem_read32(palette_list) - source;
		switch (copy_mode)
		{
		case 0x0u:
			copy_palettes_with_pen_0((uint16_t)copy_mode, size, &source, &dest);
			break;
		case 0x2u:
			copy_palette_block(size, &source, &dest);
			break;
		case 0x4u:
			repeat_palette_block(size, &source, &dest, &palette_list);
			break;
		case 0x6u:
			copy_palettes_with_pen_0((uint16_t)copy_mode, size, &source, &dest);
			break;
		default:
			unknown_code(0x9A4DFu + copy_mode);
		}
		entries_left--;
	} while (entries_left != 0u);
	*p_source = source;
	*p_dest = dest;
}

void copy_palettes_with_pen_0(uint16_t pen_0_color, uint32_t size, uint32_t *p_source, uint32_t *p_dest)
{
	uint32_t source = *p_source;
	uint32_t dest = *p_dest;
	uint16_t pen_15 = 0;
	uint32_t pens_1_2 = 0, pens_3_4 = 0, pens_5_6 = 0, pens_7_8 = 0, pens_9_10 = 0, pens_11_12 = 0, pens_13_14 = 0, halfwords_left = 0;

	size = size >> 1;
	do
	{
		mem_write16(dest, pen_0_color);
		pens_1_2 = mem_read32(source);
		source += 4u;
		dest += 2u;
		mem_write32(dest, pens_1_2);
		pens_3_4 = mem_read32(source);
		source += 4u;
		dest += 4u;
		mem_write32(dest, pens_3_4);
		pens_5_6 = mem_read32(source);
		source += 4u;
		dest += 4u;
		mem_write32(dest, pens_5_6);
		pens_7_8 = mem_read32(source);
		source += 4u;
		dest += 4u;
		mem_write32(dest, pens_7_8);
		pens_9_10 = mem_read32(source);
		source += 4u;
		dest += 4u;
		mem_write32(dest, pens_9_10);
		pens_11_12 = mem_read32(source);
		source += 4u;
		dest += 4u;
		mem_write32(dest, pens_11_12);
		pens_13_14 = mem_read32(source);
		source += 4u;
		dest += 4u;
		mem_write32(dest, pens_13_14);
		pen_15 = mem_read16(source);
		source += 2u;
		dest += 4u;
		mem_write16(dest, pen_15);
		halfwords_left = size;
		size = halfwords_left - 0xFu;
		dest += 2u;
	} while ((int32_t)halfwords_left > 0xF);
	*p_source = source;
	*p_dest = dest;
}

void copy_palette_block(uint32_t size, uint32_t *p_source, uint32_t *p_dest)
{
	uint32_t source = *p_source;
	uint32_t dest = *p_dest;
	uint32_t dest_end = 0, source_end = 0;

	size = size >> 1;
	move_string(source, size, dest, size, 2u, 0u, 0u, fill_value, &dest_end, &source_end);
	source = source_end;
	dest = dest_end;
	*p_source = source;
	*p_dest = dest;
}

void repeat_palette_block(uint32_t size, uint32_t *p_source, uint32_t *p_dest, uint32_t *p_palette_list)
{
	uint32_t source = *p_source;
	uint32_t dest = *p_dest;
	uint32_t palette_list = *p_palette_list;
	uint32_t dest_end = 0, repeat_counts = 0, repeats_left = 0, source_end = 0;

	repeat_counts = source;
	source = mem_read32(palette_list);
	size = size >> 1;
	palette_list += 4u;
	do
	{
		repeats_left = mem_read16(repeat_counts);
		repeat_counts += 2u;
		do
		{
			move_string(source, 0x10u, dest, 0x10u, 2u, 0u, 0u, fill_value, &dest_end, &source_end);
			dest = dest_end;
			repeats_left--;
		} while (repeats_left != 0u);
		source = source_end;
		size--;
	} while (size != 0u);
	*p_source = source;
	*p_dest = dest;
	*p_palette_list = palette_list;
}

void queue_vblank_copy(uint32_t source, uint32_t dest, uint16_t halfwords, uint32_t *p_copy_offset, uint32_t *p_copy_entry)
{
	uint32_t copy_entry = 0, copy_offset = 0;

	vblank_copies_pending = 0u;
	copy_offset = vblank_copy_count;
	vblank_copy_count++;
	copy_offset = copy_offset * 5u;
	copy_entry = vblank_copies_ADDRESS + copy_offset * 2u;
	mem_write32(copy_entry, source);
	copy_entry += 4u;
	mem_write32(copy_entry, dest);
	copy_entry += 4u;
	mem_write16(copy_entry, halfwords);
	vblank_copies_pending = 1u;
	copy_entry += 2u;
	*p_copy_offset = copy_offset;
	*p_copy_entry = copy_entry;
}
