/**
 *    Name:        objects/vines.c
 *    Description: object types 2 and 3 (stage 4): the vines with a bar and the scroll stop that holds the players at them
 *    Version:     0.1.0
 *    Created:     12-10-2026
 *    Modified:    15-10-2026
 *    Author:      Sebastian Toyos
 *
 *    SegaSonic The Hedgehog (arcade, Sega System 32): the game's V60 program decompiled into C by
 *    tools/decompile.py, with the names of symbols.txt.
 */
#include "game.h"

void vine_init(void)
{
	uint32_t block = 0, free_count = 0, ignored = 0, object = 0, object_2 = 0, object_3 = 0, piece_count = 0, spawn_result = 0;
	uint16_t speed_x_fraction = 0;

	OBJECT(current_object)->flags = 0u;
	OBJECT(current_object)->sprite_offset_x = 0;
	OBJECT(current_object)->sprite_offset_y = 0;
	OBJECT(current_object)->speed_z = 0x3FF;
	OBJECT(current_object)->vine_init.base_x = OBJECT(current_object)->x;
	OBJECT(current_object)->speed = (uint32_t)OBJECT(current_object)->vine_init.phase_step << 16;
	OBJECT(current_object)->speed_x_int = OBJECT(current_object)->vine_init.segment_count;
	OBJECT(current_object)->speed_x_fraction = OBJECT(current_object)->speed_x_int;
	OBJECT(current_object)->vine_init.depth_offset = OBJECT(current_object)->speed_x_int * 8u;
	spawn_count = OBJECT(current_object)->speed_x_int;
	spawn_count--;
	add_pieces_to_current_object(&spawn_result, &free_count, &object_2, &block);
	if ((uint8_t)spawn_result == 0u)
		return;
	object_3 = block;
	spawn_count = 2u;
	spawn_object_with_pieces(0x00ECFFu, &spawn_result, &piece_count, &free_count, &object_2, &block);
	if ((uint8_t)spawn_result == 0u)
		return;
	OBJECT(object_3)->child = object_2;
	OBJECT(object_2)->parent = object_3;
	OBJECT(object_2)->direction = object_cursor;
	OBJECT(object_2)->hitbox_size_x = 0x14u;
	OBJECT(object_2)->hitbox_height = 8u;
	OBJECT(object_2)->kind = 0xAu;
	OBJECT(object_2)->vine_bar.anchor_z = OBJECT(object_cursor)->z_int;
	OBJECT(object_2)->handler = vine_bar_update_ADDRESS;
	object_2 = LINKED(object_2, OBJECT(object_2)->child);
	OBJECT(object_2)->sprite_scale = 0x100u;
	OBJECT(object_2)->sprite_frame = 0x5D90u;
	OBJECT(object_2)->sprite_palette = 0x7A0u;
	OBJECT(object_2)->sprite_flags = 8u;
	OBJECT(current_object)->handler = vine_swing_ADDRESS;
	OBJECT(current_object)->speed_x_fraction = OBJECT(current_object)->speed_x_int;
	current_player = current_object;
	object = LINKED(block, OBJECT(current_object)->child);
	do
	{
		OBJECT(object)->projected_y = OBJECT(object_cursor)->projected_y;
		OBJECT(object)->sprite_flags = 0u;
		OBJECT(object)->sprite_palette = 0x8350u;
		OBJECT(object)->sprite_frame = 0x6E68u;
		OBJECT(current_player)->child = object;
		OBJECT(object)->parent = current_player;
		current_player = (current_player & ~0xFFFFu) | (uint16_t)object;
		object = LINKED(object, OBJECT(object)->child);
		speed_x_fraction = OBJECT(current_object)->speed_x_fraction;
		OBJECT(current_object)->speed_x_fraction = speed_x_fraction - 1u;
	} while ((int16_t)speed_x_fraction > 1);
	OBJECT(current_player)->sprite_frame = 0x6E70u;
	OBJECT(current_player)->sprite_palette = 0x10E0u;
	OBJECT(current_object)->floor_z = OBJECT(current_object)->z;
	project_object(&ignored, &ignored);
}

void vine_swing(void)
{
	uint8_t bucket = 0;
	uint16_t screen_x = 0;
	uint32_t block_offset = 0, ignored = 0, in_front = 0, point_x = 0, point_y = 0, segment_angle_int = 0, segment_depth = 0, slope_height = 0, z = 0;
	uint16_t speed_x_fraction = 0;

	OBJECT(current_object)->vine_swing.holding = 1u;
	if (vines_hold_players == 0u && OBJECT(current_object)->speed_z_int >= 0x9F && OBJECT(current_object)->speed_z_int <= 0x110)
		OBJECT(current_object)->vine_swing.holding = 0u;
	screen_x = (uint16_t)OBJECT(current_object)->screen_x;
	if ((int16_t)screen_x >= 0x200 || (int16_t)screen_x <= -0x200)
	{
		delete_object_and_children(&ignored, &block_offset, &slope_height);
		vines_hold_players = 0xFFu;
		return;
	}
	OBJECT(current_object)->speed_z += OBJECT(current_object)->speed;
	OBJECT(current_object)->speed_z_int &= 0x3FFu;
	OBJECT(current_object)->speed_y = (uint32_t)sine_table[(uint16_t)OBJECT(current_object)->speed_z_int] * OBJECT(current_object)->vine_swing.curl_scale;
	OBJECT(current_object)->z = OBJECT(current_object)->floor_z;
	update_screen_position(&ignored, &segment_depth);
	in_front = 0u;
	if (OBJECT(object_cursor)->vine_swing.segment_angle_int >= 0x100 && OBJECT(object_cursor)->vine_swing.segment_angle_int < 0x300)
		in_front = 1u;
	OBJECT(current_object)->vine_swing.segment_angle = 0u;
	OBJECT(current_object)->x = OBJECT(current_object)->vine_swing.base_x;
	OBJECT(current_object)->speed_x_fraction = OBJECT(current_object)->speed_x_int;
	OBJECT(object_cursor)->vine_swing.segment_angle += OBJECT(object_cursor)->speed_y;
	do
	{
		current_object = LINKED(current_object, OBJECT(current_object)->child);
		OBJECT(object_cursor)->vine_swing.segment_angle += OBJECT(object_cursor)->speed_y;
		OBJECT(object_cursor)->vine_swing.segment_angle_int &= 0x3FFu;
		segment_angle_int = (uint16_t)OBJECT(object_cursor)->vine_swing.segment_angle_int;
		OBJECT(object_cursor)->x += sine_table[segment_angle_int] * 0x18u;
		point_x = OBJECT(object_cursor)->x;
		OBJECT(object_cursor)->z += cosine_table[segment_angle_int] * 0x18u;
		z = OBJECT(object_cursor)->z;
		point_y = OBJECT(object_cursor)->y;
		project_point(z, &ignored, &point_x, &point_y);
		OBJECT(current_object)->screen_x = point_x;
		OBJECT(current_object)->screen_y = point_y;
		if (in_front == 0u)
		{
			segment_depth = (uint16_t)(OBJECT(object_cursor)->screen_y - (OBJECT(object_cursor)->x_int - OBJECT(object_cursor)->vine_swing.base_x_int) + OBJECT(object_cursor)->vine_swing.depth_offset);
			queue_sprite_at_depth(&ignored, &segment_depth);
		}
		else
		{
			bucket = 0u;
			queue_sprite_in_bucket(bucket);
		}
		speed_x_fraction = OBJECT(object_cursor)->speed_x_fraction;
		OBJECT(object_cursor)->speed_x_fraction = speed_x_fraction - 1u;
	} while ((int16_t)speed_x_fraction > 1);
}

void vine_bar_update(void)
{
	uint16_t kind = 0, screen_dy = 0, screen_x = 0, x_int = 0, y_int = 0, z = 0;
	uint32_t r11 = 0, depth = 0, height = 0, ignored = 0, vine = 0;

	OBJECT(current_object)->vine_bar.holding = mem_read8((0x200000u | OBJECT(current_object)->direction) + 0x80u);
	if ((OBJECT(current_object)->flags & 8u) != 0u && OBJECT(current_object)->floor_z_int < 0x800)
	{
		current_object = LINKED(current_object, OBJECT(object_cursor)->child);
		OBJECT(current_object)->screen_x = OBJECT(object_cursor)->screen_x;
		OBJECT(current_object)->screen_y = OBJECT(object_cursor)->screen_y;
		OBJECT(current_object)->screen_y += OBJECT(object_cursor)->floor_z_int - OBJECT(object_cursor)->z_int;
		queue_sprite(0u);
		current_object = object_cursor;
	}
	vine = 0x200000u | OBJECT(current_object)->direction;
	OBJECT(current_object)->x = mem_read32(vine + 0x4Cu);
	OBJECT(current_object)->y = mem_read32(vine + 0x50u);
	OBJECT(current_object)->z = mem_read32(vine + 0x40u);
	screen_x = (uint16_t)OBJECT(current_object)->screen_x;
	screen_dy = (uint16_t)OBJECT(current_object)->screen_y;
	project_object(&ignored, &depth);
	screen_dy -= OBJECT(current_object)->screen_y;
	OBJECT(current_object)->projected_x += screen_x - OBJECT(current_object)->screen_x;
	OBJECT(current_object)->projected_y_int += screen_dy;
	unproject_object(&ignored, &ignored, &ignored, &r11);
	height = (OBJECT(object_cursor)->floor_z_int - OBJECT(object_cursor)->z_int) & 0x7Fu;
	if ((int16_t)height > 0x50)
	{
		AT8(OBJECT(object_cursor)->hitbox_size_y, 1) = 0u;
		OBJECT(object_cursor)->hitbox_height = 8u;
	}
	else
	{
		AT8(OBJECT(object_cursor)->hitbox_size_y, 1) = height;
		OBJECT(object_cursor)->hitbox_height = height;
	}
	OBJECT(current_object)->health = 0x7F;
	add_to_touchable_list(height);
	add_to_rider_list();
	current_object = object_cursor;
	if (OBJECT(current_object)->floor_z_int < 0x800 && mem_read8((0x200000u | OBJECT(current_object)->direction) + 0x80u) == 1u)
	{
		x_int = (uint16_t)OBJECT(current_object)->x_int;
		y_int = (uint16_t)OBJECT(current_object)->y_int;
		kind = 7u;
		z = (uint16_t)(OBJECT(current_object)->z_int + 0x14u);
		spawn_sign(x_int, y_int, kind, z, &depth, &r11);
	}
}

void vine_scroll_stop_init(void)
{
	vines_hold_players = 1u;
	OBJECT(current_object)->handler = vine_scroll_stop_wait_ADDRESS;
	vine_scroll_stop_wait();
}

void vine_scroll_stop_wait(void)
{
	if ((int16_t)scroll_position < (int16_t)OBJECT(current_object)->spawn_arg_1)
	{
		vine_scroll_stop_hold();
		return;
	}
	scroll_mode = 1u;
	saved_scroll_mode = 1u;
	OBJECT(current_object)->handler = vine_scroll_stop_hold_ADDRESS;
	vine_scroll_stop_hold();
}

void vine_scroll_stop_hold(void)
{
	uint32_t all_clear = 0, player_index = 0, player_object = 0;

	player_index = 3u;
	all_clear = 1u;
	do
	{
		player_index--;
		if (player_index == 0xFFFFFFFFu)
		{
			if (all_clear == 0u)
			{
				vine_scroll_stop_end();
				return;
			}
			scroll_mode = 0u;
			saved_scroll_mode = 0u;
			scroll_speed = 8u;
			OBJECT(current_object)->handler = vine_scroll_stop_end_ADDRESS;
			vine_scroll_stop_end();
			return;
		}
		player_object = GAME_VARIABLES + vine_scroll_stop_player_blocks[player_index];
		player_object = (player_object & ~0xFFFFu) | mem_read16(player_object + 0x14u);
	} while ((mem_read32(player_object + 0x30u) & 0x400u) != 0u || (mem_read32(player_object + 0x30u) & 0x10000u) != 0u || (int16_t)mem_read16(player_object + 0x4Eu) >= -0x120);
	all_clear = 0u;
	if (all_clear == 0u)
	{
		vine_scroll_stop_end();
		return;
	}
	scroll_mode = 0u;
	saved_scroll_mode = 0u;
	scroll_speed = 8u;
	OBJECT(current_object)->handler = vine_scroll_stop_end_ADDRESS;
	vine_scroll_stop_end();
}

void vine_scroll_stop_end(void)
{
	uint32_t block_offset = 0, ignored = 0, slope_height = 0;

	if ((int16_t)OBJECT(current_object)->spawn_arg_2 < (int16_t)scroll_position)
	{
		scroll_mode = 7u;
		saved_scroll_mode = 7u;
		vines_hold_players = 0u;
		delete_object_and_children(&ignored, &block_offset, &slope_height);
	}
}
