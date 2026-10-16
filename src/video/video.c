/**
 *    Name:        video/video.c
 *    Description: the board's video: the video registers and the screen mode, block copies and fills, palette RAM and its copies at VBLANK, the VRAM scripts that build the tilemaps
 *    Version:     0.1.0
 *    Created:     12-10-2026
 *    Modified:    15-10-2026
 *    Author:      Sebastian Toyos
 *
 *    SegaSonic The Hedgehog (arcade, Sega System 32): the game's V60 program decompiled into C by
 *    tools/decompile.py, with the names of symbols.txt.
 */
#include "game.h"

void init_video_registers(void)
{
	uint8_t mode = 0;
	uint16_t value = 0;
	uint32_t count = 0, defaults_cursor = 0, ignored = 0, rects_left = 0, register_cursor = 0, right_bottom = 0;

	vblank_start_hook = apply_video_registers_ADDRESS;
	defaults_cursor = video_register_defaults_ADDRESS;
	register_cursor = video_control;
	count = 1u;
	copy_blocks(count, &defaults_cursor, &register_cursor);
	count = 4u;
	clear_blocks(&ignored, &count, &register_cursor);
	value = 0x200u;
	count = 6u;
	register_cursor = fill_halfwords(value, count, register_cursor);
	mem_write32(register_cursor, mem_read32(defaults_cursor));
	register_cursor += 4u;
	right_bottom = 0xDF013Fu;
	rects_left = 5u;
	defaults_cursor += 4u;
	do
	{
		mem_write32(register_cursor, 0u);
		register_cursor += 4u;
		mem_write32(register_cursor, right_bottom);
		rects_left--;
		register_cursor += 4u;
	} while (rects_left != 0u);
	mem_write32(register_cursor, mem_read32(defaults_cursor));
	mem_write32(register_cursor + 4u, mem_read32(defaults_cursor + 4u));
	mode = 0u;
	set_screen_mode(mode);
	ignored = clear_page_0_and_scroll();
}

uint32_t clear_page_0_and_scroll(void)
{
	uint32_t count = 0, dest = 0, ignored = 0;

	dest = 0x300000u;
	count = 0x40u;
	clear_blocks(&ignored, &count, &dest);
	dest = nbg0_scroll_x_ADDRESS;
	count = 8u;
	clear_blocks(&ignored, &count, &dest);
	return dest;
}

void init_video_registers_layers_off(void)
{
	init_video_registers();
	mem_write16(video_layer_disable, mem_read16(video_layer_disable) | 0xFu);
	vblank_start_hook = empty_vblank_hook_ADDRESS;
	empty_vblank_hook();
}

void empty_vblank_hook(void)
{
}

void apply_video_registers(void)
{
	move_halfwords(VIDEO_RAM + 0x1FF10u, video_registers_latched_ADDRESS, 0x20u);
	mem_write8(video_layer_opaque_flags, video_layer_opaque_latched);
	move_halfwords(video_registers_latched_ADDRESS, nbg0_scroll_x_ADDRESS, 0x21u);
}

uint32_t set_screen_mode(uint16_t mode)
{
	uint16_t control_bits = 0;
	uint32_t control_register = 0;

	mode = (uint8_t)(mode ^ (setting_monitor_flip & 2u));
	screen_mode = (uint8_t)mode;
	control_register = video_control;
	control_bits = (uint16_t)(mode & 1u);
	mem_write8(sprite_wide_screen, (uint8_t)control_bits);
	mem_write16(video_control, mem_read16(video_control) & 0x7DFFu);
	mode &= 2u;
	mode = (uint16_t)(((uint32_t)mode << 8) | (mode >> 8));
	control_bits = (uint16_t)(((uint32_t)control_bits << 15) | (control_bits >> 1) | mode);
	mode = (uint16_t)(mode * 3u);
	mem_write8(sprite_flip, ((uint32_t)mode << 7) | (mode >> 9));
	mem_write16(video_control, mem_read16(video_control) | control_bits);
	return control_register;
}

void copy_blocks(uint32_t count, uint32_t *p_source, uint32_t *p_dest)
{
	uint32_t source = *p_source;
	uint32_t dest = *p_dest;

	do
	{
		mem_write32(dest, mem_read32(source));
		dest += 4u;
		source += 4u;
		mem_write32(dest, mem_read32(source));
		dest += 4u;
		source += 4u;
		mem_write32(dest, mem_read32(source));
		dest += 4u;
		source += 4u;
		mem_write32(dest, mem_read32(source));
		dest += 4u;
		count--;
		source += 4u;
	} while (count != 0u);
	*p_source = source;
	*p_dest = dest;
}

void clear_blocks(uint32_t *p_value, uint32_t *p_count, uint32_t *p_dest)
{
	uint32_t count = *p_count;
	uint32_t dest = *p_dest;
	uint32_t value = 0;

	value = 0u;
	do
	{
		mem_write32(dest, value);
		dest += 4u;
		mem_write32(dest, value);
		dest += 4u;
		mem_write32(dest, value);
		dest += 4u;
		mem_write32(dest, value);
		count--;
		dest += 4u;
	} while (count != 0u);
	*p_value = value;
	*p_count = count;
	*p_dest = dest;
}

uint32_t fill_halfwords(uint16_t value, uint32_t count, uint32_t dest)
{
	do
	{
		mem_write16(dest, value);
		count--;
		dest += 2u;
	} while (count != 0u);
	return dest;
}

void run_vram_script(uint32_t *p_count, uint32_t *p_script, uint32_t *p_dest)
{
	uint32_t count = *p_count;
	uint32_t script = *p_script;
	uint32_t dest = *p_dest;
	uint32_t dictionary_cursor = 0, script_code = 0;

	dictionary_cursor = vram_script_dictionary_ADDRESS;
	for (;;)
	{
		script_code = mem_read8(script);
		script++;
		if ((int8_t)script_code >= 0)
		{
			mem_write16(dest, vram_script_dictionary[script_code]);
			dest += 2u;
			continue;
		}
		script_code = script_code & 0x7Fu;
		switch (script_code)
		{
		case 0x0u:
			vram_script_set_address(script_code, &script, &dest);
			continue;
		case 0x3u:
			vram_script_skip(&script, &dest);
			continue;
		case 0x6u:
			vram_script_fill(&count, &script, &dest);
			continue;
		case 0x9u:
			vram_script_count_up(&count, &script, &dest);
			continue;
		case 0xCu:
			vram_script_count_down(&count, &script, &dest);
			continue;
		case 0xFu:
			vram_script_bytes(&count, &script, &dest);
			continue;
		case 0x12u:
			vram_script_words(&count, &script, &dest);
			continue;
		case 0x15u:
			vram_script_words_to_dictionary(&count, &script, &dest, &dictionary_cursor);
			continue;
		case 0x18u:
			vram_script_clear_pages(&script, &dest);
			continue;
		case 0x1Bu:
			vram_script_fill_down(&count, &script, &dest);
			continue;
		case 0x1Eu:
			vram_script_fill_down_wide(&count, &script, &dest);
			continue;
		case 0x21u:
			vram_script_fill_up(&count, &script, &dest);
			continue;
		case 0x24u:
			vram_script_repeat(dest, &count, &script);
			continue;
		case 0x27u:
			vram_script_skip_long(&script, &dest);
			continue;
		case 0x2Au:
			*p_count = count;
			*p_script = script;
			*p_dest = dest;
			return;
		default:
			unknown_code(0x9988Eu + script_code);
		}
	}
}

void vram_script_end(void)
{
}

void vram_script_set_address(uint32_t script_code, uint32_t *p_script, uint32_t *p_dest)
{
	uint32_t script = *p_script;
	uint32_t dest = 0;

	dest = 0x300000u + ((script_code & ~0xFFFFu) | mem_read16(script)) * 2u;
	script += 2u;
	*p_script = script;
	*p_dest = dest;
}

void vram_script_skip(uint32_t *p_script, uint32_t *p_dest)
{
	uint32_t script = *p_script;
	uint32_t dest = *p_dest;

	dest += (int8_t)mem_read8(script) * 2u;
	script++;
	*p_script = script;
	*p_dest = dest;
}

void vram_script_skip_long(uint32_t *p_script, uint32_t *p_dest)
{
	uint32_t script = *p_script;
	uint32_t dest = *p_dest;

	dest += (int16_t)mem_read16(script) * 2u;
	script += 2u;
	*p_script = script;
	*p_dest = dest;
}

void vram_script_fill(uint32_t *p_count, uint32_t *p_script, uint32_t *p_dest)
{
	uint32_t script = *p_script;
	uint32_t dest = *p_dest;
	uint16_t tile = 0;
	uint32_t count = 0;

	count = mem_read8(script);
	script++;
	tile = mem_read16(script);
	script += 2u;
	do
	{
		mem_write16(dest, tile);
		count--;
		dest += 2u;
	} while (count != 0u);
	*p_count = count;
	*p_script = script;
	*p_dest = dest;
}

void vram_script_count_up(uint32_t *p_count, uint32_t *p_script, uint32_t *p_dest)
{
	uint32_t script = *p_script;
	uint32_t dest = *p_dest;
	uint16_t tile = 0;
	uint32_t count = 0;

	count = mem_read8(script);
	script++;
	tile = mem_read16(script);
	script += 2u;
	do
	{
		mem_write16(dest, tile);
		tile++;
		count--;
		dest += 2u;
	} while (count != 0u);
	*p_count = count;
	*p_script = script;
	*p_dest = dest;
}

void vram_script_count_down(uint32_t *p_count, uint32_t *p_script, uint32_t *p_dest)
{
	uint32_t script = *p_script;
	uint32_t dest = *p_dest;
	uint16_t tile = 0;
	uint32_t count = 0;

	count = mem_read8(script);
	script++;
	tile = mem_read16(script);
	script += 2u;
	do
	{
		mem_write16(dest, tile);
		tile--;
		count--;
		dest += 2u;
	} while (count != 0u);
	*p_count = count;
	*p_script = script;
	*p_dest = dest;
}

void vram_script_bytes(uint32_t *p_count, uint32_t *p_script, uint32_t *p_dest)
{
	uint32_t script = *p_script;
	uint32_t dest = *p_dest;
	uint16_t tile = 0;
	uint32_t count = 0;

	count = mem_read8(script);
	script++;
	tile = (uint16_t)((uint32_t)mem_read8(script) << 8);
	script++;
	do
	{
		tile = (uint16_t)((tile & ~0xFFu) | mem_read8(script));
		mem_write16(dest, tile);
		dest += 2u;
		count--;
		script++;
	} while (count != 0u);
	*p_count = count;
	*p_script = script;
	*p_dest = dest;
}

void vram_script_words(uint32_t *p_count, uint32_t *p_script, uint32_t *p_dest)
{
	uint32_t script = *p_script;
	uint32_t dest = *p_dest;
	uint32_t count = 0;

	count = mem_read8(script);
	script++;
	do
	{
		mem_write16(dest, mem_read16(script));
		dest += 2u;
		count--;
		script += 2u;
	} while (count != 0u);
	*p_count = count;
	*p_script = script;
	*p_dest = dest;
}

void vram_script_words_to_dictionary(uint32_t *p_count, uint32_t *p_script, uint32_t *p_dest, uint32_t *p_dictionary_cursor)
{
	uint32_t script = *p_script;
	uint32_t dest = *p_dest;
	uint32_t dictionary_cursor = *p_dictionary_cursor;
	uint16_t tile = 0;
	uint32_t count = 0;

	count = mem_read8(script);
	script++;
	do
	{
		tile = mem_read16(script);
		mem_write16(dest, tile);
		dest += 2u;
		mem_write16(dictionary_cursor, tile);
		dictionary_cursor = (dictionary_cursor & ~0xFFu) | (uint8_t)(dictionary_cursor + 2u);
		count--;
		script += 2u;
	} while (count != 0u);
	*p_count = count;
	*p_script = script;
	*p_dest = dest;
	*p_dictionary_cursor = (*p_dictionary_cursor & 0xFFFFFF00u) | (dictionary_cursor & 0xFFu);
}

void vram_script_clear_pages(uint32_t *p_script, uint32_t *p_dest)
{
	uint32_t script = *p_script;
	uint32_t dest = 0, halfword_count = 0;

	dest = 0x300000u + mem_read16(script) * 2u;
	script += 2u;
	halfword_count = (uint32_t)mem_read8(script) << 9;
	fill_value = 0u;
	script++;
	set_halfwords(dest, halfword_count, fill_value);
	*p_script = script;
	*p_dest = dest;
}

void vram_script_fill_down(uint32_t *p_count, uint32_t *p_script, uint32_t *p_dest)
{
	uint32_t script = *p_script;
	uint32_t dest = *p_dest;
	uint16_t tile = 0;
	uint32_t count = 0;

	count = mem_read8(script);
	script++;
	tile = mem_read16(script);
	script += 2u;
	do
	{
		mem_write16(dest, tile);
		dest += 0x40u;
		count--;
	} while (count != 0u);
	*p_count = count;
	*p_script = script;
	*p_dest = dest;
}

void vram_script_fill_up(uint32_t *p_count, uint32_t *p_script, uint32_t *p_dest)
{
	uint32_t script = *p_script;
	uint32_t dest = *p_dest;
	uint16_t tile = 0;
	uint32_t count = 0;

	count = mem_read8(script);
	script++;
	tile = mem_read16(script);
	script += 2u;
	do
	{
		mem_write16(dest, tile);
		dest -= 0x40u;
		count--;
	} while (count != 0u);
	*p_count = count;
	*p_script = script;
	*p_dest = dest;
}

void vram_script_fill_down_wide(uint32_t *p_count, uint32_t *p_script, uint32_t *p_dest)
{
	uint32_t script = *p_script;
	uint32_t dest = *p_dest;
	uint16_t tile = 0;
	uint32_t count = 0;

	count = mem_read8(script);
	script++;
	tile = mem_read16(script);
	script += 2u;
	do
	{
		mem_write16(dest, tile);
		dest += 0x80u;
		count--;
	} while (count != 0u);
	*p_count = count;
	*p_script = script;
	*p_dest = dest;
}

void vram_script_repeat(uint32_t dest, uint32_t *p_count, uint32_t *p_script)
{
	uint32_t script = *p_script;
	uint16_t tile = 0;
	uint32_t count = 0;

	count = mem_read8(script);
	script++;
	tile = mem_read16(script);
	script += 2u;
	do
	{
		mem_write16(dest, tile);
		count--;
	} while (count != 0u);
	*p_count = count;
	*p_script = script;
}

void erase_vram_script(uint32_t script, uint32_t dest)
{
	uint16_t cell = 0;
	uint32_t script_code = 0;

	cell = 0u;
	for (;;)
	{
		script_code = mem_read8(script);
		script++;
		if ((int8_t)script_code >= 0)
		{
			mem_write16(dest, cell);
			dest += 2u;
			continue;
		}
		script_code = script_code & 0x7Fu;
		switch (script_code)
		{
		case 0x0u:
			vram_script_set_address(script_code, &script, &dest);
			continue;
		case 0x3u:
			vram_script_skip(&script, &dest);
			continue;
		case 0x6u:
			erase_vram_script_fill(cell, &script, &dest);
			continue;
		case 0x9u:
			erase_vram_script_fill(cell, &script, &dest);
			continue;
		case 0xCu:
			erase_vram_script_fill(cell, &script, &dest);
			continue;
		case 0xFu:
			erase_vram_script_bytes(cell, &script, &dest);
			continue;
		case 0x12u:
			erase_vram_script_words(cell, &script, &dest);
			continue;
		case 0x15u:
			erase_vram_script_words(cell, &script, &dest);
			continue;
		case 0x18u:
			vram_script_clear_pages(&script, &dest);
			continue;
		case 0x1Bu:
			erase_vram_script_fill_down(cell, &script, &dest);
			continue;
		case 0x1Eu:
			erase_vram_script_fill_down_wide(cell, &script, &dest);
			continue;
		case 0x21u:
			erase_vram_script_fill_up(cell, &script, &dest);
			continue;
		case 0x24u:
			script = erase_vram_script_repeat(cell, script, dest);
			continue;
		case 0x27u:
			vram_script_skip_long(&script, &dest);
			continue;
		case 0x2Au:
			return;
		default:
			unknown_code(0x999B7u + script_code);
		}
	}
}

void erase_vram_script_end(void)
{
}

void erase_vram_script_fill(uint16_t cell, uint32_t *p_script, uint32_t *p_dest)
{
	uint32_t script = *p_script;
	uint32_t dest = *p_dest;
	uint32_t count = 0;

	count = mem_read8(script);
	script += 3u;
	do
	{
		mem_write16(dest, cell);
		count--;
		dest += 2u;
	} while (count != 0u);
	*p_script = script;
	*p_dest = dest;
}

void erase_vram_script_bytes(uint16_t cell, uint32_t *p_script, uint32_t *p_dest)
{
	uint32_t script = *p_script;
	uint32_t dest = *p_dest;
	uint32_t count = 0;

	count = mem_read8(script);
	script = script + 2u + count;
	do
	{
		mem_write16(dest, cell);
		count--;
		dest += 2u;
	} while (count != 0u);
	*p_script = script;
	*p_dest = dest;
}

void erase_vram_script_words(uint16_t cell, uint32_t *p_script, uint32_t *p_dest)
{
	uint32_t script = *p_script;
	uint32_t dest = *p_dest;
	uint32_t count = 0;

	count = mem_read8(script);
	script = script + 1u + count * 2u;
	do
	{
		mem_write16(dest, cell);
		count--;
		dest += 2u;
	} while (count != 0u);
	*p_script = script;
	*p_dest = dest;
}

void erase_vram_script_fill_down(uint16_t cell, uint32_t *p_script, uint32_t *p_dest)
{
	uint32_t script = *p_script;
	uint32_t dest = *p_dest;
	uint32_t count = 0;

	count = mem_read8(script);
	script += 3u;
	do
	{
		mem_write16(dest, cell);
		dest += 0x40u;
		count--;
	} while (count != 0u);
	*p_script = script;
	*p_dest = dest;
}

void erase_vram_script_fill_up(uint16_t cell, uint32_t *p_script, uint32_t *p_dest)
{
	uint32_t script = *p_script;
	uint32_t dest = *p_dest;
	uint32_t count = 0;

	count = mem_read8(script);
	script += 3u;
	do
	{
		mem_write16(dest, cell);
		dest -= 0x40u;
		count--;
	} while (count != 0u);
	*p_script = script;
	*p_dest = dest;
}

void erase_vram_script_fill_down_wide(uint16_t cell, uint32_t *p_script, uint32_t *p_dest)
{
	uint32_t script = *p_script;
	uint32_t dest = *p_dest;
	uint32_t count = 0;

	count = mem_read8(script);
	script += 3u;
	do
	{
		mem_write16(dest, cell);
		dest += 0x80u;
		count--;
	} while (count != 0u);
	*p_script = script;
	*p_dest = dest;
}

uint32_t erase_vram_script_repeat(uint16_t cell, uint32_t script, uint32_t dest)
{
	uint32_t count = 0;

	count = mem_read8(script);
	script += 3u;
	do
	{
		mem_write16(dest, cell);
		count--;
	} while (count != 0u);
	return script;
}

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
