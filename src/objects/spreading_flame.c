/**
 *    Name:        objects/spreading_flame.c
 *    Description: stages B and C: the flames that spread ahead of the fire front
 *    Version:     0.1.0
 *    Created:     12-10-2026
 *    Modified:    15-10-2026
 *    Author:      Sebastian Toyos
 *
 *    SegaSonic The Hedgehog (arcade, Sega System 32): the game's V60 program decompiled into C by
 *    tools/decompile.py, with the names of symbols.txt.
 */
#include "game.h"

void spreading_flame_start(void)
{
	uint32_t block_offset = 0, cursor = 0, ignored = 0, x = 0, y = 0;

	OBJECT(current_object)->sprite_flags = 0x1044u;
	OBJECT(current_object)->sprite_palette = 0x10C0u;
	OBJECT(current_object)->sprite_offset_x = 0;
	OBJECT(current_object)->sprite_offset_y = 0;
	OBJECT(current_object)->flags = 0u;
	OBJECT(current_object)->animation = flame_animation_ADDRESS;
	OBJECT(current_object)->animation_frame = 0u;
	OBJECT(current_object)->animation_speed = 2u;
	OBJECT(current_object)->animation_timer = 0;
	OBJECT(current_object)->handler = spreading_flame_burn_ADDRESS;
	find_floor(&x, &y, &block_offset, &ignored, &cursor);
	OBJECT(current_object)->z = OBJECT(current_object)->floor_z;
	project_object(&ignored, &ignored);
}

void spreading_flame_burn(void)
{
	uint16_t chaser_gap = 0;
	uint32_t r1 = 0, r7 = 0, r8 = 0, depth = 0, fireball = 0, free_count = 0, ignored = 0, object = 0, off_screen = 0, target_dy = 0;

	if (chaser_object == 0u)
		OBJECT(current_object)->flags |= 0x400u;
	if ((OBJECT(current_object)->flags & 0x400u) == 0u)
	{
		if ((OBJECT(current_object)->flags & 0x400u) == 0u)
		{
			object = LINKED(current_object, chaser_object);
			if (OBJECT(current_object)->y_int < OBJECT(object)->y_int)
			{
				update_screen_position(&ignored, &r1);
				animate(&r1, &r7, &r8);
				depth = (uint16_t)(OBJECT(current_object)->screen_y + 0x30u);
				queue_sprite_at_depth(&ignored, &depth);
				return;
			}
			chaser_gap = (uint16_t)(OBJECT(current_object)->y_int - OBJECT(object)->y_int);
			if (stage == 0xCu && chaser_speed > 8)
				chaser_gap += 0x40u;
			flame_chaser_gap = chaser_gap;
			allocate_object(&free_count, &fireball);
			if (free_count == 0u)
			{
				update_screen_position(&ignored, &r1);
				animate(&r1, &r7, &r8);
				depth = (uint16_t)(OBJECT(current_object)->screen_y + 0x30u);
				queue_sprite_at_depth(&ignored, &depth);
				return;
			}
			current_object = fireball;
			OBJECT(current_object)->x = OBJECT(object_cursor)->x;
			OBJECT(current_object)->y = OBJECT(object_cursor)->y;
			OBJECT(current_object)->var_96 = OBJECT(current_object)->x_int;
			AS16(OBJECT(current_object)->player) = OBJECT(current_object)->y_int;
			next_random(&target_dy, &ignored, &ignored);
			target_dy = target_dy & 0x3Fu;
			if ((int16_t)target_dy < flame_chaser_gap)
				target_dy = (uint16_t)flame_chaser_gap;
			AS16(OBJECT(current_object)->player) -= target_dy;
			OBJECT(current_object)->z = OBJECT(object_cursor)->z;
			OBJECT(current_object)->var_7F = 0u;
			OBJECT(current_object)->next_handler = spreading_flame_start_ADDRESS;
			start_fireball_flipped();
			current_object = object_cursor;
			OBJECT(current_object)->flags |= 0x400u;
		}
		update_screen_position(&ignored, &r1);
		animate(&r1, &r7, &r8);
		depth = (uint16_t)(OBJECT(current_object)->screen_y + 0x30u);
		queue_sprite_at_depth(&ignored, &depth);
		return;
	}
	off_screen = is_off_screen();
	if (off_screen == 0u)
	{
		update_screen_position(&ignored, &r1);
		animate(&r1, &r7, &off_screen);
		depth = (uint16_t)(OBJECT(current_object)->screen_y + 0x30u);
		queue_sprite_at_depth(&ignored, &depth);
		return;
	}
	delete_current_object();
}
