/**
 *    Name:        objects/eel_den.c
 *    Description: object type 43 (stage 10): the eel den and its unused spawner
 *    Version:     0.1.0
 *    Created:     12-10-2026
 *    Modified:    15-10-2026
 *    Author:      Sebastian Toyos
 *
 *    SegaSonic The Hedgehog (arcade, Sega System 32): the game's V60 program decompiled into C by
 *    tools/decompile.py, with the names of symbols.txt.
 */
#include "game.h"

void spawn_eel_den(uint32_t spawned, uint32_t free_count, uint32_t den, uint32_t block)
{
	uint32_t piece_count = 0;

	spawn_count = 1u;
	spawn_object_with_pieces(0x0156FFu, &spawned, &piece_count, &free_count, &den, &block);
	if ((uint8_t)spawned == 0u)
	{
		current_object = object_cursor;
		return;
	}
	current_object = den;
	eel_den_init();
}

void eel_den_init(void)
{
	OBJECT(current_object)->flags |= 2u;
	OBJECT(current_object)->hitbox_size_x = 0x47u;
	OBJECT(current_object)->hitbox_size_y = 0x42u;
	OBJECT(current_object)->hitbox_offset_x = -3;
	OBJECT(current_object)->hitbox_offset_y = -0x43;
	OBJECT(current_object)->hitbox_height = 0x28u;
	OBJECT(current_object)->kind = 0xCu;
	OBJECT(current_object)->sprite_frame = 0xAC70u;
	OBJECT(current_object)->sprite_palette = 0x1300u;
	OBJECT(current_object)->sprite_palette |= 0x4000u;
	OBJECT(current_object)->health = 1;
	OBJECT(current_object)->handler = eel_den_wait_ADDRESS;
	current_object = object_cursor;
}

void eel_den_wait(void)
{
	uint32_t allocated = 0, distance_y = 0, ignored = 0, random_value = 0, ring = 0;

	if ((stage_trigger_bits & shl32(1u, (int)(OBJECT(current_object)->spawn_arg_1))) == 0u)
	{
		r14 = LINKED(WORK_RAM, chaser_object);
		distance_xy_to_object(&ignored, &distance_y);
		if ((int16_t)distance_y > 0x35 && ((OBJECT(current_object)->flags & 4u) == 0u || OBJECT(current_object)->health > 0))
		{
			eel_den_draw();
			return;
		}
	}
	stage_trigger_bits |= shl32(1u, (int)(OBJECT(current_object)->spawn_arg_1));
	OBJECT(current_object)->handler = eel_den_collapse_ADDRESS;
	OBJECT(current_object)->projected_y_int -= 0x18u;
	released_ring_angle = 0x144u;
	rings_to_release = 3u;
	released_ring_flip = 2u;
	released_rings_random = 1u;
	drop_rings(0u, &allocated, &random_value, &ring);
	OBJECT(current_object)->projected_y_int += 0x18u;
	eel_den_collapse();
}

void eel_den_collapse(void)
{
	uint16_t frame_timer = 0;

	frame_timer = OBJECT(current_object)->eel_den.frame_timer;
	OBJECT(current_object)->eel_den.frame_timer = frame_timer - 1u;
	if (frame_timer != 0u)
	{
		eel_den_draw();
		return;
	}
	OBJECT(current_object)->eel_den.frame_timer = 2u;
	OBJECT(current_object)->sprite_frame = eel_den_collapse_frames[(uint8_t)OBJECT(current_object)->eel_den.frame_index];
	OBJECT(current_object)->eel_den.frame_index++;
	if (OBJECT(current_object)->eel_den.frame_index <= 0x2C)
	{
		eel_den_draw();
		return;
	}
	OBJECT(current_object)->handler = eel_den_draw_ADDRESS;
	eel_den_draw();
}

void eel_den_draw(void)
{
	uint32_t block_offset = 0, cursor = 0, deleted = 0, depth = 0, ignored = 0, slope_height = 0;

	unproject_object(&ignored, &block_offset, &slope_height, &cursor);
	delete_if_off_screen(&deleted, &block_offset, &slope_height);
	if ((uint8_t)deleted != 1u)
	{
		depth = (uint16_t)(OBJECT(current_object)->screen_y - 0x18u);
		queue_sprite_at_depth(&ignored, &depth);
		add_to_touchable_list(ignored);
	}
}
