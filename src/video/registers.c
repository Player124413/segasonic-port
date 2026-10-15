/**
 *    Name:        video/registers.c
 *    Description: the video registers: init_video_registers, applied at VBLANK, the screen mode; block copies and fills
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
