/**
 *    Name:        objects/ice.c
 *    Description: stage 9: the ice blocks along the edge, the moving ice platform, the crumbling ice floor
 *    Version:     0.1.0
 *    Created:     12-10-2026
 *    Modified:    15-10-2026
 *    Author:      Sebastian Toyos
 *
 *    SegaSonic The Hedgehog (arcade, Sega System 32): the game's V60 program decompiled into C by
 *    tools/decompile.py, with the names of symbols.txt.
 */
#include "game.h"

void ice_block_row_start(void)
{
	uint32_t ignored = 0, object = 0;
	uint16_t blocks_left = 0;

	OBJECT(current_object)->ice_block_row.blocks_left = 0xE;
	OBJECT(current_object)->handler = ice_block_row_start_ADDRESS;
	if (free_object_stack < OBJECT(current_object)->ice_block_row.blocks_left)
		return;
	do
	{
		allocate_object(&ignored, &object);
		OBJECT(object)->sprite_flags = 0x1002u;
		OBJECT(object)->sprite_frame = 0x7498u;
		OBJECT(object)->sprite_palette = 0x9130u;
		OBJECT(object)->handler = ice_block_update_ADDRESS;
		OBJECT(object)->flags = 2u;
		OBJECT(object)->hitbox_size_x = 0x20u;
		OBJECT(object)->hitbox_size_y = 0x5Cu;
		OBJECT(object)->hitbox_height = 0x50u;
		OBJECT(object)->sprite_offset_x = 0;
		OBJECT(object)->sprite_offset_y = 0;
		OBJECT(object)->hitbox_offset_x = 0;
		OBJECT(object)->hitbox_offset_y = 0;
		OBJECT(object)->kind = 0xCu;
		OBJECT(object)->speed_z = 0;
		OBJECT(object)->ice_block.state = 0u;
		OBJECT(object)->x = -0x2B20000;
		OBJECT(object)->y = OBJECT(current_object)->y;
		OBJECT(object)->z = 0x780000;
		OBJECT(current_object)->y_int -= 0x5Cu;
		blocks_left = OBJECT(current_object)->ice_block_row.blocks_left;
		OBJECT(current_object)->ice_block_row.blocks_left = blocks_left - 1u;
	} while ((int16_t)blocks_left > 1);
	delete_current_object();
}

void ice_block_update(void)
{
	uint32_t depth = 0, ignored = 0, margin = 0, object = 0, off_screen = 0, push_speed = 0;

	switch (OBJECT(current_object)->ice_block.state)
	{
	case 0:
	case 3:
		OBJECT(current_object)->ice_block.state++;
		project_object(&ignored, &ignored);
		return;
	case 1:
		margin = 0x50u;
		off_screen = check_left_screen_margin((uint16_t)margin);
		if (off_screen != 0u)
		{
			delete_current_object();
			return;
		}
		if ((OBJECT(current_object)->flags & 4u) == 0u)
		{
			add_to_touchable_list(push_speed);
			update_screen_position(&ignored, &margin);
			depth = (uint16_t)(OBJECT(current_object)->screen_y + 0x20u);
			queue_sprite_at_depth(&ignored, &depth);
			return;
		}
		OBJECT(current_object)->ice_block.state++;
		object = LINKED(current_object, OBJECT(current_object)->touched_by);
		push_speed = 0x10000u;
		if ((int8_t)OBJECT(object)->kind > 2)
			push_speed = (uint32_t)OBJECT(object)->speed_fraction << 8;
		OBJECT(current_object)->speed_x = push_speed;
		add_to_touchable_list(push_speed);
		update_screen_position(&ignored, &margin);
		depth = (uint16_t)(OBJECT(current_object)->screen_y + 0x20u);
		queue_sprite_at_depth(&ignored, &depth);
		return;
	case 2:
		OBJECT(current_object)->x -= OBJECT(current_object)->speed_x;
		if (OBJECT(current_object)->x_int <= -0x2BA)
		{
			OBJECT(current_object)->speed_z += 0x6000u;
			OBJECT(current_object)->z += OBJECT(current_object)->speed_z;
		}
		project_object(&ignored, &ignored);
		if (OBJECT(current_object)->screen_y < 0x120)
		{
			depth = (uint16_t)(OBJECT(current_object)->screen_y + 0x20u);
			queue_sprite_at_depth(&ignored, &depth);
			return;
		}
		delete_current_object();
		return;
	default:
		unknown_code(0x87E2Fu + OBJECT(current_object)->ice_block.state * 2u);
	}
}

void spawn_ice_platform_leader(uint32_t record, uint32_t *p_spawned, uint32_t *p_travel, uint32_t *p_leader)
{
	uint16_t travel = (uint16_t)*p_travel;
	uint32_t leader = *p_leader;
	uint16_t reference_x = 0;
	uint32_t free_count = 0, spawned = 0;

	allocate_object_outside_loop(&free_count, &leader);
	if (free_count == 0u)
	{
		*p_spawned = free_count;
		*p_leader = leader;
		return;
	}
	group_leaders[mem_read8(record) & 3u] = leader;
	record++;
	OBJECT(leader)->ice_platform_leader.end_position = mem_read16(record);
	record += 2u;
	reference_x = mem_read16(record);
	record += 2u;
	travel = (uint16_t)(mem_read16(record) - reference_x);
	OBJECT(leader)->ice_platform_leader.travel = travel;
	OBJECT(leader)->x_int = mem_read16(record);
	OBJECT(leader)->x_fraction = 0u;
	record += 2u;
	OBJECT(leader)->y_int = mem_read16(record);
	OBJECT(leader)->y_fraction = 0u;
	OBJECT(leader)->z_int = mem_read16(record + 2u);
	OBJECT(leader)->z_fraction = 0u;
	OBJECT(leader)->floor_z = OBJECT(leader)->z;
	OBJECT(leader)->handler = ice_platform_leader_move_ADDRESS;
	OBJECT(leader)->speed_x = 0x10000;
	OBJECT(leader)->speed_y = 0;
	OBJECT(leader)->flags = 0u;
	OBJECT(leader)->sprite_offset_x = 0;
	OBJECT(leader)->sprite_offset_y = 0;
	moving_floor_object = leader;
	spawned = 0xFFFFFFFFu;
	*p_spawned = spawned;
	*p_travel = (*p_travel & 0xFFFF0000u) | travel;
	*p_leader = leader;
}

void ice_platform_leader_move(void)
{
	uint8_t sound_id = 0;
	uint32_t r1 = 0, ignored = 0, write_index_address = 0;

	project_object(&ignored, &r1);
	if (OBJECT(current_object)->ice_platform_leader.end_position <= scroll_position)
	{
		delete_current_object();
		return;
	}
	OBJECT(current_object)->x += OBJECT(current_object)->speed_x;
	OBJECT(current_object)->ice_platform_leader.travel += OBJECT(current_object)->speed_x_int;
	if ((uint16_t)(OBJECT(current_object)->ice_platform_leader.travel + 0x87u) >= 0x10Fu)
	{
		OBJECT(current_object)->ice_platform_leader.travel = 0;
		OBJECT(current_object)->speed_x = 0u - OBJECT(current_object)->speed_x;
		sound_id = 0xFu;
		play_sound(sound_id, &r1, &write_index_address);
	}
}

void ice_platform_piece_update(void)
{
	uint16_t margin = 0;
	uint32_t r1 = 0, depth = 0, ignored = 0, object = 0, off_screen = 0;

	switch (OBJECT(current_object)->ice_platform_piece.state)
	{
	case 0:
	case 3:
		OBJECT(current_object)->ice_platform_piece.state++;
		OBJECT(current_object)->speed_z = 0;
		project_object(&ignored, &ignored);
		if (OBJECT(current_object)->ice_platform_piece.member == 3u)
			OBJECT(current_object)->part_list = ice_platform_piece_parts_2_ADDRESS;
		return;
	case 1:
		margin = 0x60u;
		off_screen = check_left_screen_margin(margin);
		if (off_screen != 0u)
		{
			delete_current_object();
			return;
		}
		if (OBJECT(current_object)->ice_platform_piece.icicle_landed != 0u)
		{
			OBJECT(current_object)->ice_platform_piece.state++;
			OBJECT(current_object)->part_list = ice_platform_piece_parts_2_ADDRESS;
		}
		object = LINKED(current_object, OBJECT(current_object)->parent);
		OBJECT(current_object)->x = OBJECT(object)->x;
		OBJECT(current_object)->y = OBJECT(object)->y;
		OBJECT(current_object)->x_int -= OBJECT(current_object)->ice_platform_piece.member * 0x2Du;
		add_to_platform_list();
		project_object(&ignored, &ignored);
		depth = (uint16_t)(OBJECT(current_object)->screen_y + OBJECT(current_object)->ice_platform_piece.depth_offset - OBJECT(current_object)->sprite_offset_y);
		OBJECT(current_object)->ice_platform_piece.draw_depth = depth;
		queue_sprite_at_depth(&ignored, &depth);
		return;
	case 2:
		off_screen = is_off_screen();
		if (off_screen == 0u)
		{
			OBJECT(current_object)->speed_z += 0x6000u;
			OBJECT(current_object)->z += OBJECT(current_object)->speed_z;
			update_screen_position(&ignored, &r1);
			depth = OBJECT(current_object)->ice_platform_piece.draw_depth;
			queue_sprite_at_depth(&ignored, &depth);
			return;
		}
		delete_current_object();
		return;
	default:
		unknown_code(0x87FB3u + OBJECT(current_object)->ice_platform_piece.state * 3u);
	}
}

void ice_floor_start(void)
{
	uint32_t cubes_left = 0, ignored = 0, object = 0, rows_left = 0;

	if (free_object_stack < 9)
		return;
	ice_row_start_x = OBJECT(current_object)->x;
	ice_cube_floor_block = ice_floor_floors[OBJECT(current_object)->sprite_frame];
	rows_left = 5u;
	do
	{
		cubes_left = 7u;
		do
		{
			allocate_object(&ignored, &object);
			OBJECT(object)->sprite_flags = 0x100Au;
			OBJECT(object)->sprite_frame = 0x7490u;
			OBJECT(object)->sprite_palette = 0x9120u;
			OBJECT(object)->handler = ice_floor_block_update_ADDRESS;
			OBJECT(object)->flags = 0u;
			OBJECT(object)->hitbox_size_x = 0x30u;
			OBJECT(object)->hitbox_size_y = 0x30u;
			OBJECT(object)->hitbox_height = 0x30u;
			OBJECT(object)->sprite_offset_x = 2;
			OBJECT(object)->sprite_offset_y = -0x10;
			OBJECT(object)->hitbox_offset_x = 0;
			OBJECT(object)->hitbox_offset_y = 0;
			OBJECT(object)->speed_z = 0;
			OBJECT(object)->ice_floor_block.state = 0u;
			OBJECT(object)->floor_block = ice_cube_floor_block;
			if ((uint16_t)cubes_left == 8u || (uint16_t)cubes_left == 7u)
				OBJECT(object)->sprite_palette |= 0x2000u;
			OBJECT(object)->x = OBJECT(current_object)->x;
			OBJECT(object)->y = OBJECT(current_object)->y;
			OBJECT(current_object)->x_int += 0x2Eu;
			OBJECT(object)->floor_z_int = mem_read16(floor_blocks + 2u + OBJECT(object)->floor_block + 0x10u);
			OBJECT(object)->floor_z_fraction = 0u;
			OBJECT(object)->z = OBJECT(object)->floor_z;
			cubes_left--;
		} while (cubes_left != 0u);
		OBJECT(current_object)->y_int += 0x2Eu;
		OBJECT(current_object)->x = ice_row_start_x;
		rows_left--;
	} while (rows_left != 0u);
	delete_current_object();
}

void ice_floor_block_update(void)
{
	uint8_t sound_id = 0;
	uint32_t r1 = 0, depth = 0, ignored = 0, object = 0, off_screen = 0, write_index_address = 0;
	uint8_t shake_timer = 0;
	uint16_t shake_duration = 0;

	switch (OBJECT(current_object)->ice_floor_block.state)
	{
	case 0:
	case 4:
		OBJECT(current_object)->ice_floor_block.state++;
		project_object(&ignored, &ignored);
		return;
	case 1:
		off_screen = check_left_screen();
		if (off_screen != 0u)
		{
			delete_current_object();
			return;
		}
		if ((OBJECT(current_object)->flags & 8u) != 0u)
		{
			object = LINKED(current_object, OBJECT(current_object)->platform);
			if ((int8_t)OBJECT(object)->kind <= 2)
			{
				OBJECT(current_object)->ice_floor_block.state++;
				OBJECT(current_object)->ice_floor_block.shake_offset = 1;
				OBJECT(current_object)->ice_floor_block.shake_timer = 0u;
				OBJECT(current_object)->ice_floor_block.shake_duration = 0x1E;
				sound_id = 0x14u;
				play_sound(sound_id, &r1, &write_index_address);
				if (PLAYER(PLAYER_BLOCK(1) + ((uint32_t)OBJECT(object)->player << 8) * 2u)->invulnerable_timer > 0)
					OBJECT(current_object)->ice_floor_block.shake_duration = 0x12C;
			}
		}
		add_to_platform_list();
		update_screen_position(&ignored, &r1);
		depth = (uint16_t)OBJECT(current_object)->screen_y;
		OBJECT(current_object)->ice_floor_block.draw_depth = depth;
		queue_sprite_at_depth(&ignored, &depth);
		return;
	case 2:
		shake_duration = OBJECT(current_object)->ice_floor_block.shake_duration;
		OBJECT(current_object)->ice_floor_block.shake_duration = shake_duration - 1u;
		if ((int16_t)shake_duration <= 1)
		{
			OBJECT(current_object)->ice_floor_block.state++;
			OBJECT(current_object)->sprite_palette |= 0x2000u;
			sound_id = 0x2Au;
			play_sound(sound_id, &r1, &write_index_address);
		}
		shake_timer = OBJECT(current_object)->ice_floor_block.shake_timer;
		OBJECT(current_object)->ice_floor_block.shake_timer = shake_timer - 1u;
		if ((int8_t)shake_timer <= 1)
		{
			OBJECT(current_object)->ice_floor_block.shake_timer = 2u;
			OBJECT(current_object)->z_int += OBJECT(current_object)->ice_floor_block.shake_offset;
			OBJECT(current_object)->ice_floor_block.shake_offset = 0u - OBJECT(current_object)->ice_floor_block.shake_offset;
		}
		add_to_platform_list();
		update_screen_position(&ignored, &r1);
		depth = (uint16_t)OBJECT(current_object)->screen_y;
		OBJECT(current_object)->ice_floor_block.draw_depth = depth;
		queue_sprite_at_depth(&ignored, &depth);
		return;
	case 3:
		off_screen = is_off_screen();
		if (off_screen != 0u)
		{
			delete_current_object();
			return;
		}
		OBJECT(current_object)->speed_z += 0x6000u;
		OBJECT(current_object)->z += OBJECT(current_object)->speed_z;
		update_screen_position(&ignored, &r1);
		depth = (uint16_t)(((int32_t)((uint32_t)OBJECT(current_object)->projected_y + OBJECT(current_object)->floor_z) >> 16) - view_y + OBJECT(current_object)->sprite_offset_y);
		OBJECT(current_object)->ice_floor_block.draw_depth = depth;
		queue_sprite_at_depth(&ignored, &depth);
		return;
	default:
		unknown_code(0x8816Cu + OBJECT(current_object)->ice_floor_block.state * 3u);
	}
}
