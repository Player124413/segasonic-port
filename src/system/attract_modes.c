/**
 *    Name:        system/attract_modes.c
 *    Description: system modes 0, 1 and 7: the warning screen, the idle mode, the attract timer wait
 *    Version:     0.1.0
 *    Created:     12-10-2026
 *    Modified:    15-10-2026
 *    Author:      Sebastian Toyos
 *
 *    SegaSonic The Hedgehog (arcade, Sega System 32): the game's V60 program decompiled into C by
 *    tools/decompile.py, with the names of symbols.txt.
 */
#include "game.h"

void attract_warning(uint32_t *p_pixels, uint32_t *p_text)
{
	uint32_t pixels = *p_pixels;
	uint32_t text = *p_text;
	uint8_t new_mode = 0;
	uint32_t r0 = 0, r11 = 0, ignored = 0;
	uint8_t cnt_bits = 0;

	switch (system_mode_step)
	{
	case 0:
	case 4:
		break;
	case 1:
		wait_attract_timer();
		*p_pixels = pixels;
		*p_text = text;
		return;
	case 2:
		advance_attract_screen();
		*p_pixels = pixels;
		*p_text = text;
		return;
	default:
		unknown_code(0x9C4EFu + system_mode_step * 3u);
	}
	attract_timer = -0x258;
	system_mode_step++;
	display_off_frames = 3u;
	cnt_bits = mem_read8(io_cnt);
	mem_write8(io_cnt, cnt_bits & ~2u);
	load_text_font(&pixels, &ignored);
	init_video_registers();
	init_sprites(&r0, &ignored);
	new_mode = 1u;
	set_screen_mode(new_mode);
	init_sound();
	text = warning_screen_text_ADDRESS;
	print_text_list(&text, &r11);
	unused_coin_byte = 0u;
	current_screen = 0u;
	*p_pixels = pixels;
	*p_text = text;
}

void system_mode_idle(void)
{
	system_mode_step = 0u;
}

void wait_attract_timer(void)
{
	if ((uint16_t)attract_timer == 0u)
	{
		system_mode_step++;
		frame_counter = 0u;
	}
}
