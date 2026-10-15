/**
 *    Name:        scenes/game_over_banner.c
 *    Description: the GAME OVER banner (game mode 7) and the prompt that blinks over the attract demos
 *    Version:     0.1.0
 *    Created:     12-10-2026
 *    Modified:    15-10-2026
 *    Author:      Sebastian Toyos
 *
 *    SegaSonic The Hedgehog (arcade, Sega System 32): the game's V60 program decompiled into C by
 *    tools/decompile.py, with the names of symbols.txt.
 */
#include "game.h"

void draw_game_over_banner(void)
{
	current_object = 0x209FC0u;
	banner_sprite_flags = 0x1000u;
	banner_sprite_x = 0xA0u;
	banner_sprite_y = 0x6Cu;
	banner_sprite_frame = 0x89A0u;
	banner_sprite_palette = 0x8970u;
	queue_sprite_on_top();
}

uint32_t blink_demo_prompt(uint32_t prompt_frame, uint32_t credit_block)
{
	uint8_t shown = 0, port_d = 0;

	if ((frame_counter & 0x1Fu) == 0u)
	{
		shown = ~demo_prompt_shown;
		demo_prompt_shown = shown;
		if (shown == 0u)
		{
			port_d = mem_read8(io_port_d);
			mem_write8(io_port_d, port_d & ~0xCu);
			return prompt_frame;
		}
		current_object = 0x209FC0u;
		banner_sprite_flags = 0x1000u;
		banner_sprite_x = 0xA0u;
		banner_sprite_y = 0x64u;
		banner_sprite_palette = 0x8960u;
		prompt_frame = (prompt_frame & ~0xFFFFu) | 0x89B8u;
		if (free_play == 0u && p1_credits < credits_per_start)
			prompt_frame = (prompt_frame & ~0xFFFFu) | 0x89A8u;
		OBJECT(current_object)->sprite_frame = prompt_frame;
	}
	if ((current_screen == 4u || current_screen == 5u) && demo_prompt_shown != 0u)
	{
		current_object = 0x209FC0u;
		queue_sprite_on_top();
		return prompt_frame;
	}
	port_d = mem_read8(io_port_d);
	mem_write8(io_port_d, port_d & ~0xCu);
	return prompt_frame;
}
