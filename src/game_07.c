/**
 *    Name:        game_07.c
 *    Description: the game's functions, part 07 (in the order of their V60 addresses)
 *    Version:     0.1.0
 *    Created:     12-10-2026
 *    Modified:    13-10-2026
 *    Author:      Sebastian Toyos
 *
 *    SegaSonic The Hedgehog (arcade, Sega System 32): the game's V60 program decompiled into C by
 *    tools/decompile.py, with the names of symbols.txt.
 */
#include "game.h"

void lava_flow_advance(void)
{
	uint16_t part_list = 0;
	uint32_t r0 = 0, r2 = 0, r3 = 0, r8 = 0, r11 = 0, depth = 0, flow_speed = 0, ignored = 0, path_cursor = 0;
	uint8_t hold_timer = 0, sound_timer = 0;
	uint16_t record_scroll = 0, wait_timer = 0;
	uint32_t flags = 0;

	flags = OBJECT(current_object)->flags;
	OBJECT(current_object)->flags = flags | 0x80000000u;
	if ((flags & 0x80000000u) == 0u)
	{
	lava_flow_read_script:
		path_cursor = OBJECT(current_object)->lava_flow.path;
		record_scroll = mem_read16(path_cursor);
		path_cursor += 2u;
		if (record_scroll <= scroll_position || (mem_read16(path_cursor) != 0u && (int16_t)mem_read16(path_cursor) <= OBJECT(current_object)->x_int))
		{
			path_cursor += 2u;
			if ((int16_t)mem_read16(path_cursor + 2u) < 0)
			{
				part_list = (uint16_t)OBJECT(current_object)->part_list;
				free_object_2(part_list);
				delete_current_object();
				return;
			}
			OBJECT(current_object)->lava_flow.screen_threshold = mem_read16(path_cursor);
			path_cursor += 2u;
			OBJECT(current_object)->lava_flow.catch_up_factor = mem_read16(path_cursor);
			path_cursor += 2u;
			OBJECT(current_object)->lava_flow.base_speed = mem_read16(path_cursor);
			path_cursor += 2u;
			OBJECT(current_object)->lava_flow.wait_timer = mem_read16(path_cursor);
			OBJECT(current_object)->lava_flow.path = path_cursor + 2u;
		}
	}
	else if (OBJECT(current_object)->lava_flow.holding_back == 0u)
	{
		if (no_player_in_play == 0u && player_mishap == 0u && player_dropping_in == 0u)
			goto lava_flow_read_script;
		OBJECT(current_object)->lava_flow.holding_back = 0xFFu;
		OBJECT(current_object)->lava_flow.saved_base_speed = OBJECT(current_object)->lava_flow.base_speed;
		OBJECT(current_object)->lava_flow.base_speed = 0;
		OBJECT(current_object)->lava_flow.hold_start_position = scroll_position;
		OBJECT(current_object)->lava_flow.hold_timer = 0xFFu;
		OBJECT(current_object)->lava_flow.saved_threshold = OBJECT(current_object)->lava_flow.screen_threshold;
		OBJECT(current_object)->lava_flow.screen_threshold = -0x50;
	}
	project_object(&r0, &ignored);
	if (OBJECT(current_object)->lava_flow.holding_back != 0u)
	{
		if ((int16_t)(scroll_position - OBJECT(current_object)->lava_flow.hold_start_position) >= 0x300)
			goto lava_flow_stop_holding_back;
		if (no_player_in_play != 0u)
			OBJECT(current_object)->lava_flow.hold_timer = 0xFFu;
		hold_timer = OBJECT(current_object)->lava_flow.hold_timer;
		OBJECT(current_object)->lava_flow.hold_timer = hold_timer - 1u;
		if (hold_timer == 0u)
			goto lava_flow_stop_holding_back;
	}
	if (OBJECT(current_object)->lava_flow.screen_threshold <= OBJECT(current_object)->screen_x)
	{
	lava_flow_use_base_speed:
		flow_speed = (uint16_t)OBJECT(current_object)->lava_flow.base_speed;
	}
	else
	{
		if (OBJECT(current_object)->lava_flow.holding_back != 0u)
		{
		lava_flow_stop_holding_back:
			OBJECT(current_object)->lava_flow.holding_back = 0u;
			OBJECT(current_object)->lava_flow.screen_threshold = OBJECT(current_object)->lava_flow.saved_threshold;
			OBJECT(current_object)->lava_flow.base_speed = OBJECT(current_object)->lava_flow.saved_base_speed;
		}
		r3 = compute_camera_ground_speed(0u);
		flow_speed = (uint16_t)((uint32_t)camera_ground_speed_int * OBJECT(current_object)->lava_flow.catch_up_factor) >> 8;
		if ((int16_t)flow_speed <= OBJECT(current_object)->lava_flow.base_speed)
			goto lava_flow_use_base_speed;
	}
	chaser_speed = flow_speed;
	wait_timer = OBJECT(current_object)->lava_flow.wait_timer;
	OBJECT(current_object)->lava_flow.wait_timer = wait_timer - 1u;
	if ((int16_t)wait_timer <= 1)
	{
		OBJECT(current_object)->lava_flow.wait_timer = 0;
		r0 = (uint32_t)chaser_speed << 12;
		OBJECT(current_object)->x += r0;
		OBJECT(current_object)->speed_x = r0;
		OBJECT(current_object)->speed_y = 0;
	}
	animate_all_parts(&r8, &r11);
	if (OBJECT(current_object)->screen_x < -0x40)
		OBJECT(current_object)->lava_flow.sound_timer = 0xFFu;
	else
	{
		if (OBJECT(current_object)->lava_flow.sound_timer != 0u)
		{
			OBJECT(current_object)->lava_flow.sound_timer = 0u;
			OBJECT(current_object)->lava_flow.sound_timer = 0u;
		}
		if (current_game_mode != 7u)
		{
			sound_timer = OBJECT(current_object)->lava_flow.sound_timer;
			OBJECT(current_object)->lava_flow.sound_timer = sound_timer - 1u;
			if (sound_timer == 0u)
			{
				OBJECT(current_object)->lava_flow.sound_timer = 0x40u;
				r0 = 0x10u;
				play_sound((uint8_t)r0, &flow_speed, &r2);
			}
		}
	}
	r11 = spawn_lava_flame(r3, r11);
	extend_lava_flow(r11);
	add_to_touchable_list(r0);
	depth = (uint16_t)(OBJECT(current_object)->screen_y + 0x50u);
	queue_sprite_at_depth(&ignored, &depth);
}

void extend_lava_flow(uint32_t block)
{
	uint32_t ignored = 0, object = 0;

	if ((uint16_t)free_object_stack != 0u && (uint16_t)free_object_stack_2[0] != 0u && (int16_t)OBJECT(current_object)->child < 0 && OBJECT(current_object)->screen_x >= -0x50 && scroll_position >= 0x6180u)
	{
		allocate_object(&ignored, &block);
		OBJECT(block)->sprite_flags = 0xD008u;
		OBJECT(block)->flags = 0u;
		OBJECT(block)->sprite_offset_x = 0;
		OBJECT(block)->sprite_offset_y = 0;
		OBJECT(block)->child = 0xFFFFu;
		OBJECT(block)->spawn_arg_2 = 0u;
		OBJECT(block)->parent = current_object;
		OBJECT(current_object)->child = block;
		OBJECT(block)->handler = lava_flow_segment_follow_ADDRESS;
		OBJECT(block)->z = OBJECT(current_object)->z;
		object = block;
		allocate_object_2(&ignored, &ignored, &block);
		move_halfwords_backward(block, lava_flow_parts_ADDRESS, 9u);
		OBJECT(object)->part_list = block;
	}
}

void lava_flow_segment_follow(void)
{
	uint16_t part_list = 0;
	uint32_t r1 = 0, r11 = 0, depth = 0, ignored = 0, object = 0, part_list_2 = 0, part_list_3 = 0;

	if (scroll_position >= 0x7E00u)
	{
		part_list = (uint16_t)OBJECT(current_object)->part_list;
		free_object_2(part_list);
		delete_current_object();
		return;
	}
	object = LINKED(current_object, OBJECT(current_object)->parent);
	OBJECT(current_object)->projected_x = OBJECT(object)->projected_x;
	OBJECT(current_object)->projected_x -= 0x40u;
	OBJECT(current_object)->projected_y = OBJECT(object)->projected_y;
	OBJECT(current_object)->projected_y_int += 0x20u;
	update_screen_position(&ignored, &r1);
	if (OBJECT(current_object)->screen_x > -0xA0)
	{
		object = LINKED(current_object, OBJECT(current_object)->parent);
		OBJECT(current_object)->x = OBJECT(object)->x;
		OBJECT(current_object)->x_int -= 0x50u;
		OBJECT(current_object)->y = OBJECT(object)->y;
		part_list_2 = OBJECT(LINKED(current_object, OBJECT(current_object)->parent))->part_list;
		part_list_3 = OBJECT(current_object)->part_list;
		mem_write16(part_list_3 + 2u, mem_read16(part_list_2 + 2u));
		mem_write16(part_list_3 + 0xAu, mem_read16(part_list_2 + 2u));
		r11 = spawn_lava_flame(0u, 0u);
		extend_lava_flow(r11);
		depth = (uint16_t)(OBJECT(LINKED(current_object, OBJECT(current_object)->parent))->screen_y + 0x48u);
		queue_sprite_at_depth(&ignored, &depth);
		return;
	}
	OBJECT(LINKED(current_object, OBJECT(current_object)->parent))->child = 0xFFFFu;
	part_list = (uint16_t)OBJECT(current_object)->part_list;
	free_object_2(part_list);
	delete_current_object();
}

uint32_t spawn_lava_flame(uint32_t random_value, uint32_t flame)
{
	uint32_t flame_random = 0, free_count = 0, ignored = 0;
	uint32_t saved_current_object = 0;
	uint16_t spawn_arg_2 = 0;

	spawn_arg_2 = OBJECT(current_object)->spawn_arg_2;
	OBJECT(current_object)->spawn_arg_2 = spawn_arg_2 - 1u;
	if ((int16_t)spawn_arg_2 <= 1)
	{
		next_random(&random_value, &ignored, &ignored);
		lava_flame_random = random_value;
		OBJECT(current_object)->spawn_arg_2 = random_value & 0x1Fu;
		allocate_object(&free_count, &flame);
		if (free_count == 0u)
			return flame;
		OBJECT(flame)->sprite_flags = 0x1004u;
		OBJECT(flame)->sprite_palette = 0x50C0u;
		OBJECT(flame)->flags = 0u;
		OBJECT(flame)->sprite_offset_x = 0;
		OBJECT(flame)->sprite_offset_y = 0;
		OBJECT(flame)->handler = lava_flame_burn_ADDRESS;
		OBJECT(flame)->z = OBJECT(current_object)->z;
		OBJECT(flame)->parent = current_object;
		OBJECT(flame)->animation = lava_flame_animation_ADDRESS;
		flame_random = lava_flame_random;
		if ((flame_random & 1u) != 0u)
			OBJECT(flame)->animation = lava_flame_animation_2_ADDRESS;
		OBJECT(flame)->animation_frame = 0u;
		OBJECT(flame)->animation_speed = 4u;
		OBJECT(flame)->animation_timer = 0;
		OBJECT(flame)->anchor_y = (((uint16_t)flame_random & 0xFFu) * 0xC0C1u + 0x100000u) >> 16;
		OBJECT(flame)->anchor_x = 0u - (((flame_random >> 16) & 0xFFu) + 0x30u);
		saved_current_object = current_object;
		current_object = flame;
		project_object(&ignored, &ignored);
		current_object = saved_current_object;
	}
	return flame;
}

void lava_flame_burn(void)
{
	uint32_t r1 = 0, r8 = 0, depth = 0, ignored = 0, object = 0, wrapped = 0;

	object = LINKED(current_object, OBJECT(current_object)->parent);
	OBJECT(current_object)->x = OBJECT(object)->x;
	OBJECT(current_object)->x_int += OBJECT(current_object)->anchor_x;
	OBJECT(current_object)->y = OBJECT(object)->y;
	OBJECT(current_object)->y_int += OBJECT(current_object)->anchor_y;
	project_object(&ignored, &r1);
	animate(&r1, &wrapped, &r8);
	if ((uint8_t)wrapped == 0u)
	{
		depth = (uint16_t)(OBJECT(current_object)->screen_y + 0x50u);
		queue_sprite_at_depth(&ignored, &depth);
		return;
	}
	delete_current_object();
}

void spawn_lava_steam(void)
{
	uint32_t free_count = 0, object = 0, offset_cursor = 0, puffs_left = 0;

	puffs_left = lava_steam_puff_count;
	offset_cursor = lava_steam_puff_offsets_ADDRESS;
	do
	{
		allocate_object(&free_count, &object);
		if (free_count == 0u)
			break;
		OBJECT(object)->sprite_flags = 0x1005u;
		OBJECT(object)->sprite_palette = 0x50D0u;
		OBJECT(object)->flags = 0u;
		OBJECT(object)->sprite_offset_x = 0;
		OBJECT(object)->sprite_offset_y = 0;
		OBJECT(object)->handler = lava_steam_puff_update_ADDRESS;
		OBJECT(object)->spawn_arg_2 = 2u;
		OBJECT(object)->animation = lava_steam_animation_ADDRESS;
		OBJECT(object)->animation_frame = 0xFFu;
		OBJECT(object)->animation_timer = 0;
		OBJECT(object)->animation_speed = 4u;
		OBJECT(object)->anchor_x = ROM16(offset_cursor);
		offset_cursor += 2u;
		OBJECT(object)->anchor_y = ROM16(offset_cursor);
		OBJECT(object)->parent = current_object;
		offset_cursor += 2u;
		puffs_left--;
	} while (puffs_left != 0u);
}

void lava_steam_puff_update(void)
{
	uint32_t r1 = 0, r8 = 0, depth = 0, ignored = 0, object = 0, random_bits = 0, wrapped = 0;
	uint16_t spawn_arg_2 = 0;

	if (scroll_position >= 0x7E00u)
	{
		delete_current_object();
		return;
	}
	if ((int8_t)OBJECT(current_object)->animation_frame >= 0)
		update_screen_position(&ignored, &r1);
	else
	{
		spawn_arg_2 = OBJECT(current_object)->spawn_arg_2;
		OBJECT(current_object)->spawn_arg_2 = spawn_arg_2 - 1u;
		if ((int16_t)spawn_arg_2 > 1)
			return;
		OBJECT(current_object)->animation_frame = 0u;
		object = LINKED(current_object, OBJECT(current_object)->parent);
		OBJECT(current_object)->x = OBJECT(object)->x;
		OBJECT(current_object)->x_int += OBJECT(current_object)->anchor_x;
		OBJECT(current_object)->y = OBJECT(object)->y;
		OBJECT(current_object)->y_int += OBJECT(current_object)->anchor_y;
		OBJECT(current_object)->z = OBJECT(object)->z;
		project_object(&ignored, &r1);
	}
	animate(&r1, &wrapped, &r8);
	if ((uint8_t)wrapped != 0u)
	{
		next_random(&random_bits, &ignored, &ignored);
		OBJECT(current_object)->spawn_arg_2 = random_bits & 0x3Fu;
		OBJECT(current_object)->animation_frame = 0xFFu;
	}
	depth = (uint16_t)(OBJECT(current_object)->screen_y + 0x50u);
	queue_sprite_at_depth(&ignored, &depth);
}

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
	uint32_t r1 = 0, r2 = 0, ignored = 0;

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
		play_sound(sound_id, &r1, &r2);
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
	uint32_t r1 = 0, r2 = 0, depth = 0, ignored = 0, object = 0, off_screen = 0;
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
				play_sound(sound_id, &r1, &r2);
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
			play_sound(sound_id, &r1, &r2);
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

void icicle_dropper_start(void)
{
	uint32_t dropper_record = 0;

	if (icicle_dropper_ready == 0u)
		return;
	icicle_dropper_ready = 0u;
	dropper_record = icicle_dropper_records_ADDRESS + icicle_dropper_records[OBJECT(current_object)->sprite_frame];
	OBJECT(current_object)->x_int = ROM16(dropper_record);
	OBJECT(current_object)->x_fraction = 0u;
	dropper_record += 2u;
	OBJECT(current_object)->y_int = ROM16(dropper_record);
	OBJECT(current_object)->y_fraction = 0u;
	dropper_record += 2u;
	OBJECT(current_object)->z_int = ROM16(dropper_record);
	OBJECT(current_object)->z_fraction = 0u;
	OBJECT(current_object)->floor_z = OBJECT(current_object)->z;
	dropper_record += 2u;
	OBJECT(current_object)->direction = ROM16(dropper_record);
	dropper_record += 2u;
	OBJECT(current_object)->var_96 = ROM16(dropper_record);
	OBJECT(current_object)->icicle_dropper.path = icicle_dropper_scripts_ADDRESS + ROM16(dropper_record + 2u);
	OBJECT(current_object)->handler = icicle_dropper_move_ADDRESS;
	OBJECT(current_object)->sprite_offset_x = 0;
	OBJECT(current_object)->sprite_offset_y = 0;
	OBJECT(current_object)->icicle_dropper.holding_back = 0u;
	OBJECT(current_object)->icicle_dropper.platform_offset = 0;
	OBJECT(current_object)->icicle_dropper.travel = 0x180;
	OBJECT(current_object)->icicle_dropper.wait_timer = 1;
	OBJECT(current_object)->flags = 0u;
	current_icicle_dropper = current_object;
}

void icicle_dropper_move(void)
{
	uint16_t move_speed = 0, row_spacing = 0;
	uint32_t r3 = 0, r11 = 0, block_offset = 0, direction = 0, floor_slope = 0, hold_threshold = 0, ignored = 0, path_cursor = 0, slope_factor = 0, step = 0;
	uint8_t hold_timer = 0;
	uint16_t wait_timer = 0, record_scroll = 0;
	uint32_t flags = 0;

	flags = OBJECT(current_object)->flags;
	OBJECT(current_object)->flags = flags | 0x80000000u;
	if ((flags & 0x80000000u) != 0u)
	{
		if (OBJECT(current_object)->icicle_dropper.holding_back != 0u)
			goto icicle_dropper_move_step;
		if (no_player_in_play != 0u || player_mishap != 0u || player_dropping_in != 0u)
		{
			OBJECT(current_object)->icicle_dropper.holding_back = 0xFFu;
			OBJECT(current_object)->icicle_dropper.saved_base_speed = OBJECT(current_object)->icicle_dropper.base_speed;
			OBJECT(current_object)->icicle_dropper.base_speed = 0;
			OBJECT(current_object)->icicle_dropper.hold_start_position = scroll_position;
			OBJECT(current_object)->icicle_dropper.hold_timer = 0xFFu;
			OBJECT(current_object)->icicle_dropper.saved_threshold = OBJECT(current_object)->icicle_dropper.screen_threshold;
			hold_threshold = 0xFFE0u;
			if (OBJECT(current_object)->direction < 0x100 || OBJECT(current_object)->direction > 0x200)
				hold_threshold = 0x160u;
			OBJECT(current_object)->icicle_dropper.screen_threshold = hold_threshold;
			goto icicle_dropper_move_step;
		}
	}
	wait_timer = OBJECT(current_object)->icicle_dropper.wait_timer;
	OBJECT(current_object)->icicle_dropper.wait_timer = wait_timer - 1u;
	if ((int16_t)wait_timer > 1)
	{
	icicle_dropper_move_step:
		if (OBJECT(current_object)->icicle_dropper.rides_platform == 0u)
		{
			find_floor(&direction, &hold_threshold, &block_offset, &r3, &r11);
			OBJECT(current_object)->z = OBJECT(current_object)->floor_z;
		}
		if (OBJECT(current_object)->icicle_dropper.rides_platform != 0u)
		{
			OBJECT(current_object)->x = OBJECT(LINKED(current_object, moving_floor_object))->x;
			OBJECT(current_object)->x -= OBJECT(current_object)->icicle_dropper.platform_offset;
			OBJECT(current_object)->x_int += 0x2Du;
		}
		project_object(&ignored, &ignored);
		if (OBJECT(current_object)->icicle_dropper.holding_back != 0u)
		{
			if ((int16_t)(scroll_position - OBJECT(current_object)->icicle_dropper.hold_start_position) >= 0x300 || (OBJECT(current_object)->icicle_dropper.last_dropper != 0u && AS32(players_finished[0]) == 0xFFFFFFu))
				goto icicle_dropper_stop_holding_back;
			if (no_player_in_play != 0u)
				OBJECT(current_object)->icicle_dropper.hold_timer = 0xFFu;
			hold_timer = OBJECT(current_object)->icicle_dropper.hold_timer;
			OBJECT(current_object)->icicle_dropper.hold_timer = hold_timer - 1u;
			if (hold_timer == 0u)
				goto icicle_dropper_stop_holding_back;
		}
		direction = OBJECT(current_object)->direction;
		switch (direction >> 8)
		{
		case 0:
		case 3:
			if (OBJECT(current_object)->screen_x <= OBJECT(current_object)->icicle_dropper.screen_threshold)
			{
			icicle_dropper_use_base_speed:
				move_speed = (uint16_t)OBJECT(current_object)->icicle_dropper.base_speed;
			}
			else
			{
			icicle_dropper_catch_up:
				OBJECT(current_object)->icicle_dropper.wait_timer = 0;
				if (OBJECT(current_object)->icicle_dropper.holding_back != 0u)
				{
				icicle_dropper_stop_holding_back:
					OBJECT(current_object)->icicle_dropper.holding_back = 0u;
					OBJECT(current_object)->icicle_dropper.screen_threshold = OBJECT(current_object)->icicle_dropper.saved_threshold;
					OBJECT(current_object)->icicle_dropper.base_speed = OBJECT(current_object)->icicle_dropper.saved_base_speed;
				}
				slope_factor = 0u;
				if ((OBJECT(current_object)->floor_slope | OBJECT(current_object)->floor_type) != 0u)
				{
					floor_slope = OBJECT(current_object)->floor_slope;
					slope_factor = floor_slope_factors[(uint16_t)(floor_slope * 2u)];
				}
				r3 = compute_camera_slope_speed(r3, slope_factor);
				move_speed = (uint16_t)((uint32_t)camera_slope_speed_int * OBJECT(current_object)->icicle_dropper.catch_up_factor) >> 8;
				if ((int16_t)move_speed <= OBJECT(current_object)->icicle_dropper.base_speed)
					goto icicle_dropper_use_base_speed;
			}
			break;
		case 1:
		case 2:
		case 4:
			if (OBJECT(current_object)->icicle_dropper.screen_threshold <= OBJECT(current_object)->screen_x)
				goto icicle_dropper_use_base_speed;
			goto icicle_dropper_catch_up;
		default:
			unknown_code(0x884C0u + (direction >> 8) * 2u);
		}
		chaser_speed = move_speed;
		if (OBJECT(current_object)->icicle_dropper.wait_timer <= 0)
		{
			if (OBJECT(current_object)->icicle_dropper.last_dropper != 0u && AS32(players_finished[0]) == 0xFFFFFFu)
				chaser_speed = 0x400;
			direction = OBJECT(current_object)->direction;
			switch (direction >> 8)
			{
			case 0:
				step = chaser_speed;
				OBJECT(current_object)->icicle_dropper.travel += step;
				step = step << 12;
				OBJECT(current_object)->icicle_dropper.platform_offset += step;
				if (OBJECT(current_object)->icicle_dropper.rides_platform == 0u)
					OBJECT(current_object)->y -= step;
				break;
			case 1:
				step = chaser_speed;
				OBJECT(current_object)->icicle_dropper.travel += step;
				step = step << 12;
				OBJECT(current_object)->icicle_dropper.platform_offset += step;
				if (OBJECT(current_object)->icicle_dropper.rides_platform == 0u)
					OBJECT(current_object)->x += step;
				break;
			case 2:
			case 4:
				step = chaser_speed;
				OBJECT(current_object)->icicle_dropper.travel += step;
				step = step << 12;
				OBJECT(current_object)->icicle_dropper.platform_offset += step;
				if (OBJECT(current_object)->icicle_dropper.rides_platform == 0u)
					OBJECT(current_object)->y += step;
				break;
			case 3:
				step = chaser_speed;
				OBJECT(current_object)->icicle_dropper.travel += step;
				step = step << 12;
				OBJECT(current_object)->icicle_dropper.platform_offset += step;
				if (OBJECT(current_object)->icicle_dropper.rides_platform == 0u)
					OBJECT(current_object)->x -= step;
				break;
			default:
				unknown_code(0x88572u + (direction >> 8) * 2u);
			}
		}
		if ((OBJECT(current_object)->flags & 1u) != 0u)
			return;
		row_spacing = 0x400u;
		if (OBJECT(current_object)->icicle_dropper.rides_platform != 0u)
			row_spacing = 0x300u;
		if ((int16_t)row_spacing <= OBJECT(current_object)->icicle_dropper.travel)
		{
			OBJECT(current_object)->icicle_dropper.travel -= row_spacing;
			drop_icicle_row(r3, r11);
		}
		return;
	}
	OBJECT(current_object)->icicle_dropper.wait_timer = 0;
	path_cursor = OBJECT(current_object)->icicle_dropper.path;
	if (OBJECT(current_object)->icicle_dropper.rides_platform != 0u)
	{
		if ((int16_t)mem_read16(path_cursor - 2u) <= OBJECT(current_object)->icicle_dropper.platform_offset_int)
		{
			icicle_dropper_ready = 0xFFu;
			if (OBJECT(current_object)->icicle_dropper.last_dropper != 0u)
			{
				OBJECT(current_object)->icicle_dropper.finish_timer = 3u;
				OBJECT(current_object)->handler = icicle_dropper_finish_ADDRESS;
				icicle_dropper_finish();
				return;
			}
			delete_current_object();
			return;
		}
		goto icicle_dropper_check_scroll_position;
	}
	direction = OBJECT(current_object)->direction;
	switch (direction >> 8)
	{
	case 0:
		if (OBJECT(current_object)->y_int <= (int16_t)mem_read16(path_cursor - 2u))
		{
		icicle_dropper_coordinate_reached:
			path_cursor += 2u;
		icicle_dropper_load_record:
			if ((int16_t)mem_read16(path_cursor + 2u) >= 0)
			{
				OBJECT(current_object)->icicle_dropper.screen_threshold = mem_read16(path_cursor);
				path_cursor += 2u;
				OBJECT(current_object)->icicle_dropper.catch_up_factor = mem_read16(path_cursor);
				path_cursor += 2u;
				OBJECT(current_object)->icicle_dropper.base_speed = mem_read16(path_cursor);
				path_cursor += 2u;
				OBJECT(current_object)->icicle_dropper.wait_timer = mem_read16(path_cursor);
				OBJECT(current_object)->icicle_dropper.path = path_cursor + 4u;
				goto icicle_dropper_move_step;
			}
			icicle_dropper_ready = 0xFFu;
			if (OBJECT(current_object)->icicle_dropper.last_dropper != 0u)
			{
				OBJECT(current_object)->icicle_dropper.finish_timer = 3u;
				OBJECT(current_object)->handler = icicle_dropper_finish_ADDRESS;
				icicle_dropper_finish();
				return;
			}
			delete_current_object();
			return;
		}
	icicle_dropper_check_scroll_position:
		record_scroll = mem_read16(path_cursor);
		path_cursor += 2u;
		if (scroll_position < record_scroll)
			goto icicle_dropper_move_step;
		goto icicle_dropper_load_record;
	case 1:
		if ((int16_t)mem_read16(path_cursor - 2u) <= OBJECT(current_object)->x_int)
			goto icicle_dropper_coordinate_reached;
		goto icicle_dropper_check_scroll_position;
	case 2:
	case 4:
		if ((int16_t)mem_read16(path_cursor - 2u) <= OBJECT(current_object)->y_int)
			goto icicle_dropper_coordinate_reached;
		goto icicle_dropper_check_scroll_position;
	case 3:
		if ((int16_t)mem_read16(path_cursor - 2u) < OBJECT(current_object)->x_int)
			goto icicle_dropper_check_scroll_position;
		goto icicle_dropper_coordinate_reached;
	default:
		unknown_code(0x883C6u + (direction >> 8) * 2u);
	}
}

void icicle_dropper_finish(void)
{
	uint16_t finish_timer = 0;

	if ((int8_t)goal_unlocked < 3)
		return;
	finish_timer = OBJECT(current_object)->icicle_dropper.finish_timer;
	OBJECT(current_object)->icicle_dropper.finish_timer = finish_timer - 1u;
	if ((int16_t)finish_timer > 1)
		return;
	goal_unlocked = 0xFFu;
	delete_current_object();
}

void drop_icicle_row(uint32_t random_value, uint32_t block)
{
	uint16_t end_coordinate = 0, line_coordinate = 0;
	uint32_t direction = 0, free_count = 0, icicles_left = 0, ignored = 0, object = 0;

	icicles_left = 3u;
	icicle_row_offset = 0x20u;
	do
	{
		allocate_object(&free_count, &block);
		if (free_count == 0u)
			break;
		OBJECT(block)->sprite_flags = 0x1004u;
		OBJECT(block)->sprite_frame = 0x74A0u;
		OBJECT(block)->sprite_palette = 0x9140u;
		OBJECT(block)->handler = icicle_update_ADDRESS;
		OBJECT(block)->hitbox_size_x = 0x28u;
		AS8(OBJECT(block)->hitbox_size_y) = 0u;
		AT8(OBJECT(block)->hitbox_size_y, 1) = 0xF0u;
		OBJECT(block)->sprite_offset_x = 0;
		OBJECT(block)->sprite_offset_y = 0;
		OBJECT(block)->hitbox_height = 0xA0u;
		OBJECT(block)->flags = 0x40000000u;
		OBJECT(block)->kind = 0x28u;
		OBJECT(block)->speed_z = 1;
		OBJECT(block)->direction = OBJECT(current_object)->direction;
		OBJECT(block)->speed = 0;
		OBJECT(block)->var_96 = OBJECT(current_object)->var_96;
		AS8(OBJECT(block)->var_82) = 0u;
		if (AS8(OBJECT(block)->var_96) != 0u)
			AS8(OBJECT(block)->var_82) = 4u;
		OBJECT(block)->x = OBJECT(current_object)->x;
		OBJECT(block)->y = OBJECT(current_object)->y;
		OBJECT(block)->z = OBJECT(current_object)->z;
		OBJECT(block)->z_int -= 0xE0u;
		OBJECT(block)->floor_z = OBJECT(current_object)->z;
		direction = OBJECT(current_object)->direction;
		switch (direction >> 8)
		{
		case 0:
		case 2:
		case 4:
		case 5:
			OBJECT(block)->x_int += icicle_row_offset;
			break;
		case 1:
		case 3:
			OBJECT(block)->y_int += icicle_row_offset;
			break;
		default:
			unknown_code(0x886F7u + (direction >> 8) * 2u);
		}
		icicle_row_offset += 0x48u;
		if (AS8(OBJECT(block)->var_96) == 0u)
		{
			end_coordinate = mem_read16(OBJECT(current_object)->drop_icicle_row.path - 2u);
			line_coordinate = (uint16_t)end_coordinate;
			direction = OBJECT(current_object)->direction;
			switch (direction >> 8)
			{
			case 0:
				if ((int16_t)(end_coordinate - OBJECT(current_object)->y_int) >= -0x40)
				{
					OBJECT(block)->y_int = line_coordinate + 0x20u;
				drop_icicle_row_line_up:
					OBJECT(block)->flags |= 1u;
					OBJECT(current_object)->flags |= 1u;
					OBJECT(block)->var_84 = mem_read16(OBJECT(current_object)->drop_icicle_row.path - 2u);
				}
				break;
			case 1:
				if ((int16_t)(end_coordinate - OBJECT(current_object)->x_int) <= 0x40)
				{
					OBJECT(block)->x_int = line_coordinate - 0x20u;
					goto drop_icicle_row_line_up;
				}
				break;
			case 2:
			case 4:
				if ((int16_t)(end_coordinate - OBJECT(current_object)->y_int) <= 0x40)
				{
					OBJECT(block)->y_int = line_coordinate - 0x20u;
					goto drop_icicle_row_line_up;
				}
				break;
			case 3:
				if ((int16_t)(end_coordinate - OBJECT(current_object)->x_int) >= -0x40)
				{
					OBJECT(block)->x_int = line_coordinate + 0x20u;
					goto drop_icicle_row_line_up;
				}
				break;
			default:
				unknown_code(0x8873Cu + (direction >> 8) * 2u);
			}
		}
		next_random(&random_value, &ignored, &ignored);
		random_value = (random_value & ~0xFFFFu) | (random_value & 0x1Fu);
		random_value = (random_value & ~0xFFFFu) | (uint16_t)(random_value + 0x3Cu);
		AS16(OBJECT(block)->var_80) = random_value;
		object = block;
		allocate_object(&free_count, &block);
		if (free_count == 0u)
			break;
		OBJECT(block)->sprite_flags = 0u;
		OBJECT(block)->sprite_palette = 0x1FFEu;
		OBJECT(block)->sprite_frame = 0x5D90u;
		OBJECT(block)->sprite_scale = 1u;
		OBJECT(block)->handler = icicle_shadow_follow_ADDRESS;
		OBJECT(block)->sprite_offset_x = 0;
		OBJECT(block)->sprite_offset_y = 0;
		OBJECT(block)->flags = 0u;
		OBJECT(block)->speed_z = 0;
		OBJECT(block)->var_86 = 0u;
		OBJECT(block)->parent = object;
		OBJECT(block)->x = OBJECT(object)->x;
		OBJECT(block)->y = OBJECT(object)->y;
		OBJECT(block)->z = OBJECT(current_object)->z;
		OBJECT(block)->floor_z = OBJECT(current_object)->z;
		if (OBJECT(current_object)->drop_icicle_row.rides_platform != 0u)
			OBJECT(current_object)->sprite_palette = 0x87A0u;
		icicles_left--;
	} while (icicles_left != 0u);
}

void icicle_update(void)
{
	uint8_t sound_id = 0;
	uint16_t child = 0, margin_y = 0, projected_x = 0, swing_limit = 0;
	uint32_t r1 = 0, r2 = 0, r7 = 0, r8 = 0, depth = 0, direction = 0, floor_z = 0, ignored = 0, landing_offset = 0, margin_x = 0, object = 0, object_2 = 0, object_3 = 0, off_screen = 0, parts_left = 0, platform_shift = 0, projected_y = 0;
	uint16_t timer = 0, timer_2 = 0;

	switch (OBJECT(current_object)->icicle_update.state)
	{
	case 0:
	case 8:
		OBJECT(current_object)->icicle_update.state++;
		project_object(&ignored, &ignored);
		return;
	case 1:
		margin_x = 0x50u;
		margin_y = 0x140u;
		off_screen = is_off_screen_margin((uint16_t)margin_x, margin_y);
		if (off_screen != 0u)
		{
			delete_current_object();
			return;
		}
		timer = OBJECT(current_object)->icicle_update.timer;
		OBJECT(current_object)->icicle_update.timer = timer - 1u;
		if ((int16_t)timer <= 1)
		{
			OBJECT(current_object)->icicle_update.timer = 0;
			OBJECT(current_object)->speed_z += 0x6000u;
			OBJECT(current_object)->z += OBJECT(current_object)->speed_z;
			if (OBJECT(current_object)->floor_z_int <= OBJECT(current_object)->z_int)
				OBJECT(current_object)->icicle_update.state++;
		}
		update_screen_position(&ignored, &margin_x);
		margin_x = queue_sprite(margin_x);
		return;
	case 2:
		if (free_object_stack >= 2 && free_object_stack_2[0] >= 2)
		{
			if ((OBJECT(current_object)->flags & 1u) == 0u)
			{
				allocate_object(&ignored, &object_2);
				OBJECT(object_2)->sprite_flags = OBJECT(current_object)->sprite_flags;
				OBJECT(object_2)->sprite_frame = OBJECT(current_object)->sprite_frame;
				OBJECT(object_2)->sprite_palette = OBJECT(current_object)->sprite_palette;
				OBJECT(object_2)->sprite_palette &= 0x1FFFu;
				OBJECT(object_2)->sprite_palette |= 0x2000u;
				OBJECT(object_2)->handler = icicle_buried_part_draw_ADDRESS;
				OBJECT(object_2)->x = OBJECT(current_object)->x;
				OBJECT(object_2)->y = OBJECT(current_object)->y;
				OBJECT(object_2)->floor_z = OBJECT(current_object)->floor_z;
				OBJECT(object_2)->parent = current_object;
				OBJECT(current_object)->child = object_2;
			}
			else
				object_2 = current_object;
			parts_left = 2u;
			do
			{
				object_3 = object_2;
				allocate_object_2(&ignored, &r2, &object_2);
				object = object_2 + 2u;
				OBJECT(object - 0x2u)->sprite_flags = 0x5000u;
				object += 2u;
				OBJECT(object - 0x4u)->sprite_frame = 0xE0u;
				object += 2u;
				OBJECT(object - 0x6u)->sprite_palette = 0u;
				object += 2u;
				OBJECT(object - 0x8u)->sprite_scale = 0x140u;
				object += 2u;
				OBJECT(object - 0xAu)->sprite_layer = 0u;
				object += 2u;
				OBJECT(object - 0xCu)->screen_y = 0;
				object += 2u;
				OBJECT(object - 0xEu)->screen_x = 0;
				OBJECT(object - 0xEu)->hitbox_size_x = 0u;
				OBJECT(object_2)->parent = object_3;
				OBJECT(object_3)->child = object_2;
				parts_left--;
			} while (parts_left != 0u);
			allocate_object(&ignored, &object_2);
			OBJECT(object_2)->sprite_flags = 0x1004u;
			OBJECT(object_2)->sprite_palette = 0x9150u;
			OBJECT(object_2)->handler = icicle_cracks_spread_ADDRESS;
			OBJECT(object_2)->x = OBJECT(current_object)->x;
			OBJECT(object_2)->y = OBJECT(current_object)->y;
			OBJECT(object_2)->floor_z = OBJECT(current_object)->floor_z;
			OBJECT(object_2)->z = OBJECT(object_2)->floor_z;
			OBJECT(object_2)->flags = 0u;
			OBJECT(object_2)->sprite_offset_x = 0;
			OBJECT(object_2)->sprite_offset_y = 0;
			OBJECT(object_2)->parent = current_object;
			OBJECT(object_2)->animation = icicle_crack_animation_ADDRESS;
			OBJECT(object_2)->animation_frame = 0xFFu;
			OBJECT(object_2)->animation_speed = 0u;
			OBJECT(object_2)->animation_timer = 0;
			OBJECT(current_object)->icicle_update.state++;
			if ((OBJECT(current_object)->flags & 1u) != 0u)
				OBJECT(current_object)->icicle_update.state = 7u;
		}
		if ((uint16_t)(OBJECT(current_object)->screen_x + 8u) < 0x150u)
		{
			sound_id = 0x29u;
			play_sound(sound_id, &r1, &r2);
		}
		update_screen_position(&ignored, &r1);
		add_to_touchable_list(ignored);
		depth = (uint16_t)(((int32_t)((uint32_t)OBJECT(current_object)->projected_y + OBJECT(current_object)->floor_z) >> 16) - view_y);
		OBJECT(current_object)->icicle_update.draw_depth = depth;
		queue_sprite_at_depth(&ignored, &depth);
		return;
	case 3:
		if ((uint32_t)OBJECT(current_object)->speed_z != 0u)
		{
			OBJECT(current_object)->z += OBJECT(current_object)->speed_z;
			if ((int16_t)(OBJECT(current_object)->z_int - OBJECT(current_object)->floor_z_int) >= 0x70)
			{
				OBJECT(current_object)->speed_z = 0;
				set_icicle_wall_box();
			}
		}
		if (map_screen_active != 0u)
		{
			delete_current_object();
			return;
		}
		margin_x = 0x30u;
		margin_y = 0xA0u;
		off_screen = is_off_screen_margin((uint16_t)margin_x, margin_y);
		if (off_screen != 0u)
		{
			delete_current_object();
			return;
		}
		update_screen_position(&ignored, &margin_x);
		add_to_touchable_list(ignored);
		depth = (uint16_t)(((int32_t)((uint32_t)OBJECT(current_object)->projected_y + OBJECT(current_object)->floor_z) >> 16) - view_y - 0x10u);
		if ((int16_t)depth < -0x10)
			depth = 0xFFF0u;
		if ((int16_t)depth > 0xEE)
			depth = 0xEEu;
		OBJECT(current_object)->icicle_update.draw_depth = depth;
		queue_sprite_at_depth(&ignored, &depth);
		return;
	case 4:
		OBJECT(current_object)->icicle_update.state++;
		object = LINKED(current_object, moving_floor_object);
		landing_offset = OBJECT(object)->speed_x * ((uint16_t)OBJECT(current_object)->icicle_update.timer + 0x19u);
		platform_shift = landing_offset;
		landing_offset = (uint16_t)(((int32_t)landing_offset >> 16) + OBJECT(object)->var_8C);
		swing_limit = 0x87u;
		if ((int16_t)landing_offset > 0x87)
			OBJECT(current_object)->x += platform_shift - ((uint32_t)(uint16_t)((landing_offset - swing_limit) * 2u) << 16);
		else
		{
			swing_limit = (uint16_t)(0u - swing_limit);
			if ((int16_t)landing_offset < (int16_t)swing_limit)
			{
				OBJECT(current_object)->x += platform_shift - ((uint32_t)(uint16_t)((landing_offset - swing_limit) * 2u) << 16);
				project_object(&ignored, &ignored);
				return;
			}
			OBJECT(current_object)->x += platform_shift;
		}
		project_object(&ignored, &ignored);
		return;
	case 5:
		if (OBJECT(current_object)->z_int >= 0x320)
		{
			delete_current_object();
			return;
		}
		timer_2 = OBJECT(current_object)->icicle_update.timer;
		OBJECT(current_object)->icicle_update.timer = timer_2 - 1u;
		if ((int16_t)timer_2 <= 1)
		{
			OBJECT(current_object)->icicle_update.timer = 0;
			OBJECT(current_object)->speed_z += 0x6000u;
			OBJECT(current_object)->z += OBJECT(current_object)->speed_z;
			if ((OBJECT(current_object)->flags & 8u) != 0u)
			{
				object = LINKED(current_object, OBJECT(current_object)->platform);
				if (OBJECT(object)->z_int <= OBJECT(current_object)->z_int)
				{
					OBJECT(current_object)->icicle_update.state++;
					OBJECT(current_object)->speed_z = 0;
					OBJECT(current_object)->sprite_palette |= 0x2000u;
					AS8(OBJECT(object)->spawn_arg_1) = 0xFFu;
				}
			}
		}
		update_screen_position(&ignored, &r1);
		add_to_rider_list();
		ignored = queue_sprite_at_floor();
		return;
	case 6:
		off_screen = is_off_screen();
		if (off_screen != 0u)
		{
			delete_current_object();
			return;
		}
		OBJECT(current_object)->speed_z += 0x6000u;
		OBJECT(current_object)->z += OBJECT(current_object)->speed_z;
		update_screen_position(&ignored, &r1);
		depth = (uint16_t)(OBJECT(LINKED(current_object, OBJECT(current_object)->platform))->var_88 + 1u);
		OBJECT(current_object)->icicle_update.draw_depth = depth;
		queue_sprite_at_depth(&ignored, &depth);
		return;
	case 7:
		if ((uint32_t)OBJECT(current_object)->speed_z != 0u)
		{
			OBJECT(current_object)->z += OBJECT(current_object)->speed_z;
			if ((int16_t)(OBJECT(current_object)->z_int - OBJECT(current_object)->floor_z_int) >= 0x70)
			{
				OBJECT(current_object)->speed_z = 0;
				if (OBJECT(current_object)->icicle_update.counts_goal != 0u)
					goal_unlocked++;
				set_icicle_wall_box();
				direction = OBJECT(current_object)->direction;
				switch (direction >> 8)
				{
				case 0:
				case 4:
					OBJECT(current_object)->y_int = OBJECT(current_object)->icicle_update.line_coordinate;
					OBJECT(current_object)->hitbox_offset_y = 0;
					break;
				case 1:
					OBJECT(current_object)->x_int = OBJECT(current_object)->icicle_update.line_coordinate;
					break;
				case 2:
					OBJECT(current_object)->y_int = OBJECT(current_object)->icicle_update.line_coordinate;
					break;
				case 3:
					OBJECT(current_object)->x_int = OBJECT(current_object)->icicle_update.line_coordinate;
					OBJECT(current_object)->hitbox_offset_x = 0;
					break;
				default:
					unknown_code(0x88BCBu + (direction >> 8) * 2u);
				}
			}
		}
		if (map_screen_active != 0u)
		{
			child = OBJECT(current_object)->child;
			free_object_2(child);
			child = OBJECT(LINKED(current_object, OBJECT(current_object)->child))->child;
			free_object_2(child);
			delete_current_object();
			return;
		}
		margin_x = 0x28u;
		margin_y = 0xA0u;
		r8 = is_off_screen_margin((uint16_t)margin_x, margin_y);
		if (r8 != 0u)
		{
			child = OBJECT(current_object)->child;
			free_object_2(child);
			child = OBJECT(LINKED(current_object, OBJECT(current_object)->child))->child;
			free_object_2(child);
			delete_current_object();
			return;
		}
		update_screen_position(&ignored, &margin_x);
		add_to_touchable_list(ignored);
		OBJECT(current_object)->icicle_update.draw_depth = ((int32_t)((uint32_t)OBJECT(current_object)->projected_y + OBJECT(current_object)->floor_z) >> 16) - view_y;
		depth = OBJECT(current_object)->icicle_update.draw_depth;
		current_object = LINKED(current_object, OBJECT(current_object)->child);
		current_object = LINKED(current_object, OBJECT(current_object)->child);
		queue_sprite_at_depth(&ignored, &depth);
		current_object = object_cursor;
		depth = OBJECT(current_object)->icicle_update.draw_depth;
		queue_sprite_at_depth(&ignored, &depth);
		projected_x = (uint16_t)OBJECT(current_object)->projected_x;
		projected_y = OBJECT(current_object)->projected_y;
		floor_z = OBJECT(current_object)->floor_z;
		point_to_screen(projected_x, floor_z, &projected_y, &r7, &r8);
		current_object = LINKED(current_object, OBJECT(current_object)->child);
		OBJECT(current_object)->sprite_frame = r8;
		depth = OBJECT(object_cursor)->icicle_update.draw_depth;
		queue_sprite_at_depth(&ignored, &depth);
		return;
	default:
		unknown_code(0x88840u + OBJECT(current_object)->icicle_update.state * 3u);
	}
}

void icicle_buried_part_draw(void)
{
	uint16_t child = 0, margin_x = 0, margin_y = 0;
	uint32_t depth = 0, floor_z = 0, ignored = 0, object = 0, off_screen = 0, part_depth = 0, point_x = 0, point_y = 0;

	object = LINKED(current_object, OBJECT(current_object)->parent);
	OBJECT(current_object)->screen_x = OBJECT(object)->screen_x;
	OBJECT(current_object)->screen_y = OBJECT(object)->screen_y;
	if (map_screen_active != 0u)
	{
		child = OBJECT(current_object)->child;
		free_object_2(child);
		child = OBJECT(LINKED(current_object, OBJECT(current_object)->child))->child;
		free_object_2(child);
		delete_current_object();
		return;
	}
	margin_x = 0x28u;
	margin_y = 0xA0u;
	off_screen = is_off_screen_margin(margin_x, margin_y);
	if (off_screen != 0u)
	{
		child = OBJECT(current_object)->child;
		free_object_2(child);
		child = OBJECT(LINKED(current_object, OBJECT(current_object)->child))->child;
		free_object_2(child);
		delete_current_object();
		return;
	}
	part_depth = (uint16_t)(OBJECT(LINKED(current_object, OBJECT(current_object)->parent))->var_88 + 1u);
	buried_part_depth = part_depth;
	current_object = LINKED(current_object, OBJECT(current_object)->child);
	current_object = LINKED(current_object, OBJECT(current_object)->child);
	queue_sprite_at_depth(&ignored, &part_depth);
	current_object = object_cursor;
	part_depth = buried_part_depth;
	queue_sprite_at_depth(&ignored, &part_depth);
	point_x = OBJECT(current_object)->x;
	point_y = OBJECT(current_object)->y;
	floor_z = OBJECT(current_object)->floor_z;
	project_point(floor_z, &ignored, &point_x, &point_y);
	current_object = LINKED(current_object, OBJECT(current_object)->child);
	OBJECT(current_object)->sprite_flags = (OBJECT(current_object)->sprite_flags & ~0xFFFu) | (point_y & 0xFFFu);
	depth = buried_part_depth;
	queue_sprite_at_depth(&ignored, &depth);
}

void icicle_cracks_spread(void)
{
	uint32_t r1 = 0, depth = 0, ignored = 0, object = 0, off_screen = 0;
	uint32_t flags = 0;

	flags = OBJECT(current_object)->flags;
	OBJECT(current_object)->flags = flags | 1u;
	if ((flags & 1u) == 0u)
	{
		OBJECT(current_object)->icicle_cracks.next_depth = 0;
		project_object(&ignored, &ignored);
		OBJECT(current_object)->anchor_x = OBJECT(current_object)->projected_x;
		OBJECT(current_object)->anchor_y = OBJECT(current_object)->projected_y_int;
		return;
	}
	if (map_screen_active != 0u)
	{
		delete_current_object();
		return;
	}
	off_screen = is_off_screen();
	if (off_screen != 0u)
	{
		delete_current_object();
		return;
	}
	object = LINKED(current_object, OBJECT(current_object)->parent);
	if (OBJECT(current_object)->icicle_cracks.next_depth <= (int16_t)(OBJECT(object)->z_int - OBJECT(object)->floor_z_int))
	{
		animate_around_anchor();
		OBJECT(current_object)->icicle_cracks.next_depth += 0x1Cu;
	}
	update_screen_position(&ignored, &r1);
	depth = (uint16_t)(OBJECT(LINKED(current_object, OBJECT(current_object)->parent))->var_88 + 2u);
	queue_sprite_at_depth(&ignored, &depth);
}

void icicle_shadow_follow(void)
{
	uint32_t depth = 0, ignored = 0, object = 0, off_screen = 0;
	uint8_t blink_timer = 0;

	project_object(&ignored, &ignored);
	off_screen = is_off_screen();
	if (off_screen != 0u)
	{
		delete_current_object();
		return;
	}
	object = LINKED(current_object, OBJECT(current_object)->parent);
	if ((AS8(OBJECT(object)->var_82) == 3u || AS8(OBJECT(object)->var_82) == 7u) && (uint32_t)OBJECT(object)->speed_z == 0u)
	{
		delete_current_object();
		return;
	}
	if (AS8(OBJECT(object)->var_96) != 0u)
	{
		OBJECT(current_object)->x += OBJECT(LINKED(current_object, moving_floor_object))->speed_x;
		if (AS8(OBJECT(object)->var_82) == 6u)
		{
			OBJECT(current_object)->speed_z += 0x6000u;
			OBJECT(current_object)->z += OBJECT(current_object)->speed_z;
			OBJECT(current_object)->sprite_palette |= 0x2000u;
		}
		OBJECT(current_object)->flags &= ~0x80000u;
		blink_timer = OBJECT(current_object)->icicle_shadow.blink_timer;
		OBJECT(current_object)->icicle_shadow.blink_timer = blink_timer - 1u;
		if ((int8_t)blink_timer <= 1)
		{
			OBJECT(current_object)->var_86 = 2u;
			OBJECT(current_object)->flags |= 0x80000u;
		}
	}
	if ((int16_t)AS16(OBJECT(object)->var_80) <= 0x32 && (int16_t)OBJECT(current_object)->sprite_scale < 0x21C)
		OBJECT(current_object)->sprite_scale += 9u;
	if ((OBJECT(current_object)->flags & 0x80000u) != 0u)
		return;
	object = LINKED(current_object, OBJECT(current_object)->parent);
	if (AS8(OBJECT(object)->var_82) == 6u)
		depth = OBJECT(object)->var_88;
	else
		depth = (uint16_t)(((int32_t)((uint32_t)OBJECT(current_object)->projected_y + OBJECT(current_object)->floor_z) >> 16) - view_y - 0x20u);
	queue_sprite_at_depth(&ignored, &depth);
}

void set_icicle_wall_box(void)
{
	uint32_t direction = 0;

	OBJECT(current_object)->flags |= 2u;
	direction = OBJECT(current_object)->direction;
	switch (direction >> 8)
	{
	case 0:
		OBJECT(current_object)->hitbox_size_x = 0x70u;
		OBJECT(current_object)->hitbox_size_y = 0x80u;
		OBJECT(current_object)->hitbox_offset_x = -0x20;
		OBJECT(current_object)->hitbox_offset_y = -0x40;
		break;
	case 1:
		OBJECT(current_object)->hitbox_size_x = 0x80u;
		OBJECT(current_object)->hitbox_size_y = 0x70u;
		OBJECT(current_object)->hitbox_offset_x = -0x40;
		OBJECT(current_object)->hitbox_offset_y = -0x40;
		break;
	case 2:
	case 4:
		OBJECT(current_object)->hitbox_size_x = 0x70u;
		OBJECT(current_object)->hitbox_size_y = 0x80u;
		OBJECT(current_object)->hitbox_offset_x = -0x20;
		OBJECT(current_object)->hitbox_offset_y = -0x7F;
		break;
	case 3:
		OBJECT(current_object)->hitbox_size_x = 0x80u;
		OBJECT(current_object)->hitbox_size_y = 0x70u;
		OBJECT(current_object)->hitbox_offset_x = 0;
		OBJECT(current_object)->hitbox_offset_y = -0x20;
		break;
	default:
		unknown_code(0x88EBFu + (direction >> 8) * 2u);
	}
}

void tornado_start(void)
{
	uint16_t angle_offset = 0, radius = 0;
	uint32_t column_slot = 0, columns_left = 0, free_count = 0, ignored = 0, object = 0, object_2 = 0, tornado_record = 0, zone_index = 0;

	if (sand_block_leader_active != 0u && (OBJECT(current_object)->sprite_frame != 1u || scroll_position < 0x9C00u))
		return;
	tornado_record = tornado_records_ADDRESS + tornado_records[OBJECT(current_object)->sprite_frame];
	OBJECT(current_object)->x_int = ROM16(tornado_record);
	OBJECT(current_object)->x_fraction = 0u;
	tornado_record += 2u;
	OBJECT(current_object)->y_int = ROM16(tornado_record);
	OBJECT(current_object)->y_fraction = 0u;
	tornado_record += 2u;
	OBJECT(current_object)->animation = ROM16(tornado_record) + tornado_hazard_lists_ADDRESS;
	tornado_kind = OBJECT(current_object)->sprite_frame;
	tornado_record += 2u;
	object = ROM16(tornado_record) + tornado_whirlwind_lists_ADDRESS;
	OBJECT(current_object)->part_list = object;
	OBJECT(current_object)->tornado.path = tornado_record + 2u;
	OBJECT(current_object)->flags = 2u;
	OBJECT(current_object)->tornado.holding_back = 0u;
	OBJECT(current_object)->kind = 0x1Eu;
	OBJECT(current_object)->tornado.wait_timer = 0;
	OBJECT(current_object)->tornado.rotation_angle = 0u;
	OBJECT(current_object)->hitbox_height = 0x70u;
	OBJECT(current_object)->tornado.off_screen = 0xFFu;
	OBJECT(current_object)->handler = tornado_move_ADDRESS;
	chaser_object = current_object;
	tornado_object = current_object;
	group_leader_0_held = 0xFFu;
	if (tornado_kind == 0u)
		players_frozen = 0xFFu;
	if (tornado_kind == 0u)
	{
		zone_index = 1u;
		object = spawn_chaser_back_zone(zone_index, object);
	}
	else
		object = spawn_tornado_rear_zone(object);
	if (free_object_stack < 5)
		return;
	columns_left = 5u;
	column_slot = current_object + 0x96u;
	angle_offset = 0u;
	radius = 0x4000u;
	do
	{
		allocate_object(&ignored, &object);
		OBJECT(object)->sprite_flags = 0x1004u;
		OBJECT(object)->sprite_palette = 0x1100u;
		OBJECT(object)->handler = tornado_column_circle_ADDRESS;
		OBJECT(object)->flags = 0u;
		OBJECT(object)->sprite_offset_x = 0;
		OBJECT(object)->sprite_offset_y = 0;
		OBJECT(object)->hitbox_size_y = 0x200u;
		OBJECT(object)->speed_z_fraction = radius;
		radius += 0x2000u;
		OBJECT(object)->speed_z_int = angle_offset;
		angle_offset += 0xCCu;
		OBJECT(object)->animation = tornado_column_animation_ADDRESS;
		OBJECT(object)->animation_frame = 0u;
		OBJECT(object)->animation_speed = 1u;
		OBJECT(object)->animation_timer = 0;
		mem_write16(column_slot, object);
		OBJECT(object)->parent = current_object;
		object_2 = object;
		OBJECT(object_2)->child = 0xFFFFu;
		column_slot += 2u;
		if (AS8(tornado_kind) != 0u)
		{
			allocate_object_2(&free_count, &ignored, &object);
			if (free_count != 0u)
			{
				OBJECT(object)->sprite_flags = 0x1004u;
				OBJECT(object)->sprite_palette = 0x1100u;
				OBJECT(object_2)->child = object;
			}
		}
		columns_left--;
	} while (columns_left != 0u);
}

void tornado_move(void)
{
	uint8_t sound_id = 0;
	uint16_t box_across = 0, box_along = 0, hazard_coordinate = 0;
	uint32_t r0 = 0, r1 = 0, r2 = 0, r3 = 0, r11 = 0, columns_left = 0, direction = 0, floor_slope = 0, hazard_record = 0, ignored = 0, move_speed = 0, object = 0, path_cursor = 0, slope_factor = 0, slot_cursor = 0, x = 0, y = 0, z = 0;
	uint8_t hold_timer = 0, sound_timer = 0;
	uint16_t hazard_scroll = 0, record_scroll = 0;
	uint32_t flags = 0;

	if (map_screen_active != 0u)
	{
		object = current_object;
		slot_cursor = current_object + 0x96u;
		columns_left = 5u;
		do
		{
			object = (object & ~0xFFFFu) | mem_read16(slot_cursor);
			OBJECT(object)->flags |= 0x400u;
			columns_left--;
			slot_cursor += 2u;
		} while (columns_left != 0u);
		delete_current_object();
		return;
	}
	flags = OBJECT(current_object)->flags;
	OBJECT(current_object)->flags = flags | 0x80000000u;
	if ((flags & 0x80000000u) != 0u)
	{
		if (OBJECT(current_object)->tornado.holding_back != 0u)
			goto tornado_move_step;
		if (no_player_in_play != 0u || player_mishap != 0u || player_dropping_in != 0u)
		{
			OBJECT(current_object)->tornado.holding_back = 0xFFu;
			OBJECT(current_object)->tornado.saved_base_speed = OBJECT(current_object)->tornado.base_speed;
			OBJECT(current_object)->tornado.base_speed = 0;
			OBJECT(current_object)->tornado.hold_start_position = scroll_position;
			OBJECT(current_object)->tornado.hold_timer = 0xFFu;
			OBJECT(current_object)->tornado.saved_threshold = OBJECT(current_object)->tornado.screen_threshold;
			r1 = 0xFFB0u;
			if (OBJECT(current_object)->direction < 0x100 || OBJECT(current_object)->direction > 0x200)
				r1 = 0x190u;
			OBJECT(current_object)->tornado.screen_threshold = r1;
			goto tornado_move_step;
		}
	}
	if (current_game_mode != 0x11u)
		OBJECT(current_object)->tornado.wait_timer--;
	if (OBJECT(current_object)->tornado.wait_timer > 0)
	{
	tornado_move_step:
		find_floor(&direction, &r1, &r2, &r3, &r11);
		OBJECT(current_object)->z = OBJECT(current_object)->floor_z;
		OBJECT(current_object)->z_int += 8u;
		OBJECT(current_object)->tornado.rotation_angle -= 4u;
		OBJECT(current_object)->tornado.rotation_angle &= 0x3FFu;
		project_object(&r0, &ignored);
		r0 = add_to_zone_list(r0);
		r11 = tornado_throw_whirlwinds((uint8_t)r0, r2, r3, r11);
		hazard_record = OBJECT(current_object)->animation;
		hazard_scroll = mem_read16(hazard_record);
		hazard_record += 2u;
		if (hazard_scroll <= scroll_position)
		{
			hazard_coordinate = mem_read16(hazard_record);
			direction = OBJECT(current_object)->direction;
			switch (direction >> 8)
			{
			case 0:
			case 4:
				if (OBJECT(current_object)->y_int <= (int16_t)hazard_coordinate)
				{
					y = OBJECT(current_object)->y;
				tornado_hazard_x_at_block_edge:
					x = mem_read32(floor_blocks + 2u + OBJECT(current_object)->floor_block) - 0x80000u;
				tornado_place_hazard:
					z = OBJECT(current_object)->z;
					spawn_tornado_hazard(x, y, z, r11);
					OBJECT(current_object)->animation += 4u;
				}
				break;
			case 1:
				if ((int16_t)hazard_coordinate <= OBJECT(current_object)->x_int)
				{
					x = OBJECT(current_object)->x - 0x800000u;
				tornado_hazard_y_at_block_edge:
					y = mem_read32(floor_blocks + 2u + OBJECT(current_object)->floor_block + 8u) - 0x80000u;
					goto tornado_place_hazard;
				}
				break;
			case 2:
				if ((int16_t)hazard_coordinate <= OBJECT(current_object)->y_int)
				{
					y = OBJECT(current_object)->y - 0x800000u;
					goto tornado_hazard_x_at_block_edge;
				}
				break;
			case 3:
				if (OBJECT(current_object)->x_int <= (int16_t)hazard_coordinate)
				{
					x = OBJECT(current_object)->x;
					goto tornado_hazard_y_at_block_edge;
				}
				break;
			default:
				unknown_code(0x89477u + (direction >> 8) * 2u);
			}
		}
		if (OBJECT(current_object)->tornado.holding_back != 0u)
		{
			if ((int16_t)(scroll_position - OBJECT(current_object)->tornado.hold_start_position) >= 0x300)
				goto tornado_stop_holding_back;
			if (player_mishap != 0u || player_dropping_in != 0u || no_player_in_play != 0u)
				OBJECT(current_object)->tornado.hold_timer = 0xFFu;
			hold_timer = OBJECT(current_object)->tornado.hold_timer;
			OBJECT(current_object)->tornado.hold_timer = hold_timer - 1u;
			if (hold_timer == 0u)
				goto tornado_stop_holding_back;
		}
		if (OBJECT(current_object)->screen_y < 0)
		{
		tornado_catch_up:
			if (OBJECT(current_object)->tornado.holding_back != 0u)
			{
			tornado_stop_holding_back:
				OBJECT(current_object)->tornado.holding_back = 0u;
				OBJECT(current_object)->tornado.screen_threshold = OBJECT(current_object)->tornado.saved_threshold;
				OBJECT(current_object)->tornado.base_speed = OBJECT(current_object)->tornado.saved_base_speed;
			}
			slope_factor = 0u;
			if ((OBJECT(current_object)->floor_slope | OBJECT(current_object)->floor_type) != 0u)
			{
				floor_slope = OBJECT(current_object)->floor_slope;
				slope_factor = floor_slope_factors[(uint16_t)(floor_slope * 2u)];
			}
			compute_camera_slope_speed(r3, slope_factor);
			move_speed = (uint16_t)((uint32_t)camera_slope_speed_int * OBJECT(current_object)->tornado.catch_up_factor) >> 8;
			if ((int16_t)move_speed <= OBJECT(current_object)->tornado.base_speed)
			{
			tornado_use_base_speed:
				move_speed = (uint16_t)OBJECT(current_object)->tornado.base_speed;
			}
		}
		else
		{
			direction = OBJECT(current_object)->direction;
			switch (direction >> 8)
			{
			case 0:
			case 3:
				if (OBJECT(current_object)->screen_x <= OBJECT(current_object)->tornado.screen_threshold)
					goto tornado_use_base_speed;
				goto tornado_catch_up;
			case 1:
			case 2:
			case 4:
				if (OBJECT(current_object)->tornado.screen_threshold <= OBJECT(current_object)->screen_x)
					goto tornado_use_base_speed;
				goto tornado_catch_up;
			default:
				unknown_code(0x89545u + (direction >> 8) * 2u);
			}
		}
		if (AS32(players_finished[0]) == 0xFFFFFFu)
			move_speed = (uint16_t)(move_speed << 2);
		chaser_speed = move_speed;
		if (OBJECT(current_object)->tornado.wait_timer <= 0)
		{
			direction = OBJECT(current_object)->direction;
			switch (direction >> 8)
			{
			case 0:
				OBJECT(current_object)->y -= (uint32_t)chaser_speed << 12;
				break;
			case 1:
				OBJECT(current_object)->x += (uint32_t)chaser_speed << 12;
				break;
			case 2:
			case 4:
				OBJECT(current_object)->y += (uint32_t)chaser_speed << 12;
				break;
			case 3:
				OBJECT(current_object)->x -= (uint32_t)chaser_speed << 12;
				break;
			default:
				unknown_code(0x895E6u + (direction >> 8) * 2u);
			}
		}
		direction = OBJECT(current_object)->direction;
		switch (direction >> 8)
		{
		case 0:
		case 3:
			if (OBJECT(current_object)->screen_x > 0x180)
				OBJECT(current_object)->tornado.off_screen = 0xFFu;
			else
			{
				if (OBJECT(current_object)->tornado.off_screen != 0u)
				{
					OBJECT(current_object)->tornado.off_screen = 0u;
					OBJECT(current_object)->tornado.sound_timer = 0u;
				}
				if (current_game_mode != 7u)
				{
					sound_timer = OBJECT(current_object)->tornado.sound_timer;
					OBJECT(current_object)->tornado.sound_timer = sound_timer - 1u;
					if (sound_timer == 0u)
					{
						sound_id = 0x12u;
						play_sound(sound_id, &move_speed, &r2);
						OBJECT(current_object)->tornado.sound_timer = 0x28u;
					}
				}
			}
			break;
		case 1:
		case 2:
		case 4:
			if (OBJECT(current_object)->screen_x < -0x40)
			{
				OBJECT(current_object)->tornado.off_screen = 0xFFu;
				return;
			}
			if (OBJECT(current_object)->tornado.off_screen != 0u)
			{
				OBJECT(current_object)->tornado.off_screen = 0u;
				OBJECT(current_object)->tornado.sound_timer = 0u;
			}
			if (current_game_mode != 7u)
			{
				sound_timer = OBJECT(current_object)->tornado.sound_timer;
				OBJECT(current_object)->tornado.sound_timer = sound_timer - 1u;
				if (sound_timer == 0u)
				{
					sound_id = 0x12u;
					play_sound(sound_id, &move_speed, &r2);
					OBJECT(current_object)->tornado.sound_timer = 0x28u;
				}
			}
			return;
		default:
			unknown_code(0x89634u + (direction >> 8) * 2u);
		}
		return;
	}
	OBJECT(current_object)->tornado.wait_timer = 0;
	path_cursor = OBJECT(current_object)->tornado.path;
	record_scroll = mem_read16(path_cursor);
	path_cursor += 2u;
	if (record_scroll <= scroll_position)
	{
		path_cursor += 2u;
		goto tornado_load_record;
	}
	r1 = mem_read16(path_cursor);
	direction = OBJECT(current_object)->direction;
	path_cursor += 2u;
	switch (direction >> 8)
	{
	case 0:
	case 4:
		if ((int16_t)r1 < OBJECT(current_object)->y_int)
			goto tornado_move_step;
	tornado_load_record:
		if ((int16_t)mem_read16(path_cursor + 2u) >= 0)
		{
			OBJECT(current_object)->tornado.screen_threshold = mem_read16(path_cursor);
			path_cursor += 2u;
			OBJECT(current_object)->tornado.catch_up_factor = mem_read16(path_cursor);
			path_cursor += 2u;
			OBJECT(current_object)->tornado.base_speed = mem_read16(path_cursor);
			path_cursor += 2u;
			OBJECT(current_object)->tornado.wait_timer = mem_read16(path_cursor);
			path_cursor += 2u;
			OBJECT(current_object)->direction = mem_read16(path_cursor);
			OBJECT(current_object)->tornado.path = path_cursor + 2u;
			box_across = 0xF0u;
			box_along = 0x80u;
			direction = OBJECT(current_object)->direction;
			switch (direction >> 8)
			{
			case 0:
			case 4:
				OBJECT(current_object)->hitbox_size_x = box_across;
				OBJECT(current_object)->hitbox_size_y = box_along;
				OBJECT(current_object)->hitbox_offset_x = 0u - (box_across >> 1);
				OBJECT(current_object)->hitbox_offset_y = 0u - (box_along >> 1);
				break;
			case 1:
				OBJECT(current_object)->hitbox_size_x = box_along;
				OBJECT(current_object)->hitbox_size_y = box_across;
				OBJECT(current_object)->hitbox_offset_x = 0u - (box_along >> 1);
				OBJECT(current_object)->hitbox_offset_y = 0u - (box_across >> 1);
				break;
			case 2:
				OBJECT(current_object)->hitbox_size_x = box_across;
				OBJECT(current_object)->hitbox_size_y = box_along;
				OBJECT(current_object)->hitbox_offset_x = 0u - (box_across >> 1);
				OBJECT(current_object)->hitbox_offset_y = 0u - (box_along >> 1);
				break;
			case 3:
				OBJECT(current_object)->hitbox_size_x = box_along;
				OBJECT(current_object)->hitbox_size_y = box_across;
				OBJECT(current_object)->hitbox_offset_x = 0u - (box_along >> 1);
				OBJECT(current_object)->hitbox_offset_y = 0u - (box_across >> 1);
				if ((uint16_t)OBJECT(current_object)->tornado.wait_timer == 0u)
					players_frozen = 0u;
				break;
			default:
				unknown_code(0x8939Du + (direction >> 8) * 2u);
			}
			if (OBJECT(current_object)->tornado.path != tornado_kind_0_end_record_ADDRESS)
				goto tornado_move_step;
			group_leader_0_held = 0u;
			goto tornado_move_step;
		}
		if ((int16_t)mem_read16(path_cursor + 4u) >= 0)
		{
			object = current_object;
			slot_cursor = current_object + 0x96u;
			columns_left = 5u;
			do
			{
				object = (object & ~0xFFFFu) | mem_read16(slot_cursor);
				OBJECT(object)->flags |= 0x400u;
				columns_left--;
				slot_cursor += 2u;
			} while (columns_left != 0u);
			delete_current_object();
			return;
		}
		goal_unlocked = 0xFFu;
		object = current_object;
		slot_cursor = current_object + 0x96u;
		columns_left = 5u;
		do
		{
			object = (object & ~0xFFFFu) | mem_read16(slot_cursor);
			OBJECT(object)->flags |= 0x400u;
			columns_left--;
			slot_cursor += 2u;
		} while (columns_left != 0u);
		delete_current_object();
		return;
	case 1:
		if (OBJECT(current_object)->x_int < (int16_t)r1)
			goto tornado_move_step;
		goto tornado_load_record;
	case 2:
		if (OBJECT(current_object)->y_int < (int16_t)r1)
			goto tornado_move_step;
		goto tornado_load_record;
	case 3:
		if ((int16_t)r1 < OBJECT(current_object)->x_int)
			goto tornado_move_step;
		goto tornado_load_record;
	default:
		unknown_code(0x89306u + (direction >> 8) * 2u);
	}
}

void tornado_column_circle(void)
{
	uint32_t r0 = 0, r1 = 0, r7 = 0, r8 = 0, depth = 0, ignored = 0, object = 0, object_2 = 0, orbit_dx = 0, orbit_dy = 0, radius_limit = 0, radius_rate = 0;

	if ((OBJECT(current_object)->flags & 0x400u) != 0u)
	{
		delete_current_object();
		return;
	}
	OBJECT(current_object)->speed_z_fraction += OBJECT(current_object)->hitbox_size_y;
	if ((int16_t)OBJECT(current_object)->speed_z_fraction < 0x1000)
	{
		next_random(&radius_rate, &ignored, &ignored);
		radius_rate = (radius_rate + 0x100u) & 0x3FFu;
		radius_limit = 0x1000u;
	tornado_column_bounce_radius:
		OBJECT(current_object)->hitbox_size_y = radius_rate;
		OBJECT(current_object)->speed_z_fraction = radius_limit;
	}
	else if ((int16_t)OBJECT(current_object)->speed_z_fraction > 0x6800)
	{
		next_random(&radius_rate, &ignored, &ignored);
		radius_rate = (uint16_t)(0u - ((radius_rate + 0x100u) & 0x3FFu));
		radius_limit = 0x6800u;
		goto tornado_column_bounce_radius;
	}
	object_2 = LINKED(current_object, OBJECT(current_object)->parent);
	OBJECT(current_object)->x = OBJECT(object_2)->x;
	OBJECT(current_object)->y = OBJECT(object_2)->y;
	orbit_dx = (OBJECT(object_2)->spawn_arg_2 + OBJECT(current_object)->speed_z_int) & 0x3FFu;
	orbit_dy = OBJECT(current_object)->speed_z_fraction;
	polar_to_xy(&r1, &orbit_dx, &orbit_dy);
	OBJECT(current_object)->x += orbit_dx;
	OBJECT(current_object)->y += orbit_dy;
	animate(&r1, &r7, &r8);
	find_floor(&r0, &r1, &radius_limit, &ignored, &object_2);
	OBJECT(current_object)->z = OBJECT(current_object)->floor_z;
	project_object(&ignored, &r1);
	queue_sprite(r1);
	if ((int16_t)OBJECT(current_object)->child < 0)
		return;
	object = LINKED(current_object, OBJECT(current_object)->child);
	OBJECT(object)->screen_x = OBJECT(current_object)->screen_x;
	OBJECT(object)->screen_y = OBJECT(current_object)->screen_y;
	OBJECT(object)->screen_y -= 0xE0u;
	OBJECT(object)->sprite_frame = tornado_column_top_frames[OBJECT(current_object)->animation_frame];
	depth = (uint16_t)OBJECT(current_object)->screen_y;
	current_object = object;
	queue_sprite_at_depth(&ignored, &depth);
}

void spawn_tornado_hazard(uint32_t x, uint32_t y, uint32_t z, uint32_t hazard)
{
	uint32_t direction = 0, free_count = 0, ignored = 0;
	uint32_t saved_current_object = 0;

	allocate_object(&free_count, &hazard);
	if (free_count == 0u)
		return;
	OBJECT(hazard)->x = x;
	OBJECT(hazard)->y = y;
	OBJECT(hazard)->z = z;
	OBJECT(hazard)->sprite_flags = 0x1004u;
	OBJECT(hazard)->handler = tornado_hazard_update_ADDRESS;
	OBJECT(hazard)->flags = 2u;
	OBJECT(hazard)->hitbox_offset_x = 0;
	OBJECT(hazard)->hitbox_offset_y = 0;
	OBJECT(hazard)->direction = OBJECT(current_object)->direction;
	OBJECT(hazard)->kind = 0xCu;
	OBJECT(hazard)->hitbox_height = 0x50u;
	direction = OBJECT(hazard)->direction;
	switch (direction >> 8)
	{
	case 0:
	case 2:
	case 4:
		OBJECT(hazard)->hitbox_size_x = 0xF0u;
		OBJECT(hazard)->hitbox_size_y = 0x80u;
		break;
	case 1:
	case 3:
		OBJECT(hazard)->hitbox_size_x = 0x80u;
		OBJECT(hazard)->hitbox_size_y = 0xF0u;
		break;
	default:
		unknown_code(0x897BBu + (direction >> 8) * 2u);
	}
	saved_current_object = current_object;
	current_object = hazard;
	project_object(&ignored, &ignored);
	current_object = saved_current_object;
}

void tornado_hazard_update(void)
{
	uint16_t margin_y = 0;
	uint32_t ignored = 0, margin_x = 0, off_screen = 0;

	margin_x = 0xF0u;
	margin_y = 0xF0u;
	off_screen = is_off_screen_margin((uint16_t)margin_x, margin_y);
	if (off_screen == 0u)
	{
		add_to_touchable_list(0u);
		update_screen_position(&ignored, &margin_x);
		return;
	}
	delete_current_object();
}

uint32_t tornado_throw_whirlwinds(uint8_t spawned, uint32_t r2, uint32_t whirlwind, uint32_t object)
{
	uint32_t saved_current_object = 0;

	if (mem_read16(OBJECT(current_object)->part_list) <= scroll_position)
	{
		saved_current_object = current_object;
		object = spawn_whirlwind(spawned, r2, whirlwind, object);
		current_object = saved_current_object;
		OBJECT(current_object)->part_list += 2u;
	}
	return object;
}

uint32_t spawn_tornado_rear_zone(uint32_t zone)
{
	uint32_t free_count = 0;

	allocate_object(&free_count, &zone);
	if (free_count != 0u)
	{
		OBJECT(zone)->sprite_flags = 0x1004u;
		OBJECT(zone)->kind = 9u;
		OBJECT(zone)->flags = 0x40000002u;
		OBJECT(zone)->hitbox_height = 0x160u;
		OBJECT(zone)->hitbox_offset_x = 0;
		OBJECT(zone)->hitbox_offset_y = 0;
		OBJECT(zone)->handler = tornado_rear_zone_follow_ADDRESS;
	}
	return zone;
}

void tornado_rear_zone_follow(void)
{
	uint32_t direction = 0, object = 0;

	if (scroll_position >= 0xE500u)
	{
		delete_current_object();
		return;
	}
	object = LINKED(current_object, chaser_object);
	OBJECT(current_object)->hitbox_size_x = OBJECT(object)->hitbox_size_x;
	OBJECT(current_object)->hitbox_size_y = OBJECT(object)->hitbox_size_y;
	OBJECT(current_object)->x = OBJECT(object)->x;
	OBJECT(current_object)->y = OBJECT(object)->y;
	OBJECT(current_object)->hitbox_offset_x = OBJECT(object)->hitbox_offset_x;
	OBJECT(current_object)->hitbox_offset_y = OBJECT(object)->hitbox_offset_y;
	OBJECT(current_object)->z = OBJECT(object)->z;
	OBJECT(current_object)->z_int += 0x60u;
	OBJECT(current_object)->direction = OBJECT(object)->direction;
	direction = OBJECT(object)->direction;
	switch (direction >> 8)
	{
	case 0:
	case 4:
		OBJECT(current_object)->hitbox_size_y = 0x200u;
		OBJECT(current_object)->y_int += 0x40u;
		OBJECT(current_object)->hitbox_offset_y = 0;
		break;
	case 1:
		OBJECT(current_object)->hitbox_size_x = 0x200u;
		OBJECT(current_object)->x_int -= 0x240u;
		OBJECT(current_object)->hitbox_offset_x = 0;
		break;
	case 2:
		OBJECT(current_object)->hitbox_size_y = 0x200u;
		OBJECT(current_object)->y_int -= 0x240u;
		OBJECT(current_object)->hitbox_offset_y = 0;
		break;
	case 3:
		OBJECT(current_object)->hitbox_size_x = 0x200u;
		OBJECT(current_object)->x_int += 0x40u;
		OBJECT(current_object)->hitbox_offset_x = 0;
		break;
	default:
		unknown_code(0x898C7u + (direction >> 8) * 2u);
	}
	direction = add_to_zone_list(direction);
}

void quicksand_start(void)
{
	uint32_t r0 = 0, block_offset = 0, ignored = 0, object = 0, object_2 = 0, y = 0;

	OBJECT(current_object)->flags = 0u;
	if (OBJECT(current_object)->sprite_frame != 0u)
		OBJECT(current_object)->flags |= 1u;
	OBJECT(current_object)->sprite_flags = 0x1000u;
	OBJECT(current_object)->sprite_frame = 0x74E0u;
	OBJECT(current_object)->sprite_offset_x = 0;
	OBJECT(current_object)->sprite_offset_y = -0x34;
	OBJECT(current_object)->hitbox_size_x = 0x60u;
	OBJECT(current_object)->hitbox_height = 0x50u;
	OBJECT(current_object)->hitbox_size_y = 0u;
	OBJECT(current_object)->handler = quicksand_swirl_ADDRESS;
	OBJECT(current_object)->kind = 0x1Cu;
	OBJECT(current_object)->quicksand.palette_index = 0u;
	OBJECT(current_object)->quicksand.palette_delay = 4u;
	OBJECT(current_object)->quicksand.palette_timer = 0u;
	OBJECT(current_object)->quicksand.palette_list = quicksand_palettes_ADDRESS;
	OBJECT(current_object)->animation_frame = 0u;
	OBJECT(current_object)->animation_speed = 4u;
	OBJECT(current_object)->animation_timer = 0;
	OBJECT(current_object)->animation = quicksand_animation_ADDRESS;
	find_floor(&r0, &y, &block_offset, &ignored, &object);
	OBJECT(current_object)->z = OBJECT(current_object)->floor_z;
	OBJECT(current_object)->z_int += 0x30u;
	project_object(&ignored, &ignored);
	OBJECT(current_object)->child = 0xFFFFu;
	if (free_object_stack_2[0] >= 2)
	{
		allocate_object_2(&ignored, &ignored, &object);
		OBJECT(object)->sprite_flags = 0xD000u;
		OBJECT(current_object)->child = object;
		object_2 = object;
		allocate_object_2(&ignored, &ignored, &object);
		move_halfwords(object, quicksand_parts_ADDRESS, 0x11u);
		OBJECT(object_2)->part_list = object;
	}
}

void quicksand_swirl(void)
{
	uint16_t object = 0, sprite_frame = 0, step_frame = 0;
	uint32_t depth = 0, ignored = 0, margin = 0, next_step = 0, object_2 = 0, off_screen = 0, part_cursor = 0, parts_left = 0, step_list = 0;
	uint8_t animation_timer = 0;
	uint16_t last_step = 0;

	margin = 0x70u;
	off_screen = check_left_screen_margin((uint16_t)margin);
	if (off_screen == 0u)
	{
		add_to_touchable_list(0u);
		update_screen_position(&ignored, &margin);
		if ((OBJECT(current_object)->flags & 1u) != 0u)
			return;
		animate_sign();
		if ((int16_t)OBJECT(current_object)->child < 0)
		{
			current_object = object_cursor;
			depth = (uint16_t)(OBJECT(current_object)->screen_y - 0x58u);
			queue_sprite_at_depth(&ignored, &depth);
			return;
		}
		animation_timer = OBJECT(current_object)->animation_timer;
		OBJECT(current_object)->animation_timer = animation_timer - 1u;
		if ((int8_t)animation_timer <= 1)
		{
			OBJECT(current_object)->animation_timer = OBJECT(current_object)->animation_speed;
			step_list = OBJECT(current_object)->animation;
			next_step = OBJECT(current_object)->animation_frame;
			next_step = (uint8_t)(next_step + 1u);
			last_step = mem_read16(step_list);
			step_list += 2u;
			if ((int16_t)last_step < (int16_t)next_step)
				next_step = 0u;
			OBJECT(current_object)->animation_frame = next_step;
			step_frame = mem_read16(step_list + next_step * 2u);
			part_cursor = OBJECT(LINKED(current_object, OBJECT(current_object)->child))->part_list;
			parts_left = mem_read16(part_cursor);
			part_cursor += 2u;
			do
			{
				mem_write16(part_cursor, step_frame);
				part_cursor += 8u;
				parts_left--;
			} while (parts_left != 0u);
		}
		object_2 = LINKED(current_object, OBJECT(current_object)->child);
		OBJECT(object_2)->screen_x = OBJECT(current_object)->screen_x;
		OBJECT(object_2)->screen_y = OBJECT(current_object)->screen_y;
		current_object = LINKED(current_object, OBJECT(current_object)->child);
		depth = (uint16_t)(OBJECT(current_object)->screen_y - 0x58u);
		queue_sprite_at_depth(&ignored, &depth);
		current_object = object_cursor;
		depth = (uint16_t)(OBJECT(current_object)->screen_y - 0x58u);
		queue_sprite_at_depth(&ignored, &depth);
		return;
	}
	object_2 = LINKED(current_object, OBJECT(current_object)->child);
	sprite_frame = OBJECT(object_2)->sprite_frame;
	free_object_2(sprite_frame);
	object = (uint16_t)object_2;
	free_object_2(object);
	delete_current_object();
}

void sinkhole_start(void)
{
	uint32_t r0 = 0, block_offset = 0, cursor = 0, growth_record = 0, ignored = 0, y = 0;
	uint32_t sprite_frame = 0;

	OBJECT(current_object)->sprite_flags = 8u;
	sprite_frame = OBJECT(current_object)->sprite_frame;
	growth_record = sprite_frame & ~0x8000u;
	if ((sprite_frame & 0x8000u) != 0u)
		OBJECT(current_object)->sprite_flags |= 0x40u;
	growth_record = sinkhole_growth_records_ADDRESS + growth_record * 4u;
	OBJECT(current_object)->sinkhole.zoom_step = mem_read16(growth_record);
	OBJECT(current_object)->sinkhole.open_position = mem_read16(growth_record + 2u);
	OBJECT(current_object)->sinkhole.open_position += scroll_position;
	OBJECT(current_object)->sprite_palette = 0x1020u;
	OBJECT(current_object)->sprite_scale = 1u;
	OBJECT(current_object)->flags = 0u;
	OBJECT(current_object)->sinkhole.unscaled_offset_x = -8;
	if ((OBJECT(current_object)->sprite_flags & 0x40u) != 0u)
		OBJECT(current_object)->sinkhole.unscaled_offset_x = 0u - OBJECT(current_object)->sinkhole.unscaled_offset_x;
	OBJECT(current_object)->sinkhole.unscaled_offset_y = -0x30;
	OBJECT(current_object)->hitbox_size_x = 1u;
	OBJECT(current_object)->sinkhole.unscaled_hitbox_size_x = 0x4Cu;
	OBJECT(current_object)->hitbox_height = 0x40u;
	OBJECT(current_object)->hitbox_size_y = 0u;
	OBJECT(current_object)->handler = sinkhole_open_ADDRESS;
	OBJECT(current_object)->kind = 0x1Bu;
	OBJECT(current_object)->direction = 0x200u;
	if ((OBJECT(current_object)->sprite_flags & 0x40u) != 0u)
		OBJECT(current_object)->direction = 0x300u;
	OBJECT(current_object)->animation_frame = 0u;
	OBJECT(current_object)->animation_speed = 4u;
	OBJECT(current_object)->animation_timer = 0;
	OBJECT(current_object)->animation = sinkhole_animation_ADDRESS;
	find_floor(&r0, &y, &block_offset, &ignored, &cursor);
	OBJECT(current_object)->z = OBJECT(current_object)->floor_z;
	project_object(&ignored, &ignored);
}

void sinkhole_open(void)
{
	uint32_t r1 = 0, r2 = 0, r7 = 0, ignored = 0, off_screen = 0, scale = 0, sound_id = 0;

	r1 = 0x68u;
	off_screen = check_left_screen_margin((uint16_t)r1);
	if (off_screen != 0u)
	{
		delete_current_object();
		return;
	}
	if (scroll_position < OBJECT(current_object)->sinkhole.open_position)
		return;
	if ((int16_t)OBJECT(current_object)->sprite_scale >= 0x180)
	{
		add_to_touchable_list(sound_id);
		update_screen_position(&ignored, &r1);
		OBJECT(current_object)->sinkhole.draw_depth = OBJECT(current_object)->screen_y;
		r1 = queue_sprite(r1);
		return;
	}
	if (OBJECT(current_object)->sprite_scale == 1u)
	{
		sound_id = 0xFu;
		play_sound((uint8_t)sound_id, &r1, &r2);
	}
	OBJECT(current_object)->sprite_scale += OBJECT(current_object)->sinkhole.zoom_step;
	scale = OBJECT(current_object)->sprite_scale;
	OBJECT(current_object)->sprite_offset_x = (int32_t)(OBJECT(current_object)->sinkhole.unscaled_offset_x * scale) >> 8;
	OBJECT(current_object)->sprite_offset_y = (int32_t)(OBJECT(current_object)->sinkhole.unscaled_offset_y * scale) >> 8;
	animate(&scale, &r7, &off_screen);
	r1 = OBJECT(current_object)->sprite_scale;
	OBJECT(current_object)->hitbox_size_x = (int32_t)(OBJECT(current_object)->sinkhole.unscaled_hitbox_size_x * r1) >> 8;
	add_to_touchable_list(sound_id);
	update_screen_position(&ignored, &r1);
	OBJECT(current_object)->sinkhole.draw_depth = OBJECT(current_object)->screen_y;
	r1 = queue_sprite(r1);
}

void sand_patch_start(void)
{
	uint32_t r0 = 0, block_offset = 0, cursor = 0, ignored = 0, y = 0;

	OBJECT(current_object)->sprite_flags = 0x1000u;
	if ((OBJECT(current_object)->sprite_frame & 0x8000u) != 0u)
		OBJECT(current_object)->sprite_flags |= 0x40u;
	OBJECT(current_object)->sprite_frame = 0x7530u;
	OBJECT(current_object)->sprite_palette = 0x9020u;
	OBJECT(current_object)->handler = sand_patch_draw_ADDRESS;
	OBJECT(current_object)->flags = 0u;
	OBJECT(current_object)->sprite_offset_x = 0;
	OBJECT(current_object)->sprite_offset_y = 0;
	find_floor(&r0, &y, &block_offset, &ignored, &cursor);
	OBJECT(current_object)->z = OBJECT(current_object)->floor_z;
	project_object(&ignored, &ignored);
}

void sand_patch_draw(void)
{
	uint16_t unseen_margin = 0;
	uint32_t depth = 0, ignored = 0, margin = 0, off_screen = 0;

	margin = 0x38u;
	unseen_margin = 0x10u;
	off_screen = check_left_screen_margins((uint16_t)margin, unseen_margin);
	if (off_screen == 0u)
	{
		update_screen_position(&ignored, &margin);
		depth = (uint16_t)(OBJECT(current_object)->screen_y - 0x38u);
		queue_sprite_at_depth(&ignored, &depth);
		return;
	}
	delete_current_object();
}

void rimmed_sinkhole_start(void)
{
	uint32_t block_offset = 0, free_count = 0, growth_record = 0, ignored = 0, object = 0, y = 0;
	uint32_t sprite_frame = 0;

	OBJECT(current_object)->sprite_flags = 0x1000u;
	OBJECT(current_object)->hitbox_size_x = 0x70u;
	OBJECT(current_object)->hitbox_height = 0x40u;
	OBJECT(current_object)->sprite_offset_x = 0;
	OBJECT(current_object)->sprite_offset_y = 0;
	OBJECT(current_object)->hitbox_size_y = 0u;
	AS16(OBJECT(current_object)->flags) = 0u;
	OBJECT(current_object)->kind = 0x1Du;
	allocate_object(&free_count, &object);
	if (free_count == 0u)
		return;
	OBJECT(object_cursor)->handler = rimmed_sinkhole_edge_zone_update_ADDRESS;
	current_object = object;
	OBJECT(object_cursor)->parent = current_object;
	OBJECT(current_object)->var_86 = object_cursor;
	OBJECT(current_object)->x = OBJECT(object_cursor)->x;
	OBJECT(current_object)->y = OBJECT(object_cursor)->y;
	OBJECT(current_object)->sprite_flags = 0u;
	sprite_frame = OBJECT(object_cursor)->sprite_frame;
	growth_record = sprite_frame & ~0x8000u;
	if ((sprite_frame & 0x8000u) != 0u)
		OBJECT(current_object)->sprite_flags |= 0x40u;
	growth_record = sinkhole_growth_records_ADDRESS + growth_record * 4u;
	AS16(OBJECT(current_object)->var_80) = mem_read16(growth_record);
	OBJECT(current_object)->var_84 = mem_read16(growth_record + 2u);
	OBJECT(current_object)->var_84 += scroll_position;
	OBJECT(current_object)->sprite_frame = 0x7518u;
	OBJECT(current_object)->sprite_palette = 0x9020u;
	OBJECT(current_object)->sprite_scale = 1u;
	OBJECT(current_object)->flags = 0u;
	OBJECT(current_object)->sprite_offset_x = 0;
	OBJECT(current_object)->sprite_offset_y = 0;
	OBJECT(current_object)->handler = rimmed_sinkhole_open_ADDRESS;
	OBJECT(current_object)->hitbox_size_x = 1u;
	OBJECT(current_object)->var_8C = 0x4Cu;
	OBJECT(current_object)->hitbox_height = 0x40u;
	OBJECT(current_object)->hitbox_size_y = 0u;
	OBJECT(current_object)->kind = 0x1Bu;
	OBJECT(current_object)->direction = 0u;
	if ((OBJECT(current_object)->sprite_flags & 0x40u) != 0u)
		OBJECT(current_object)->direction = 0x100u;
	OBJECT(current_object)->animation = rimmed_sinkhole_animation_ADDRESS;
	OBJECT(current_object)->animation_frame = 0u;
	OBJECT(current_object)->animation_speed = 4u;
	OBJECT(current_object)->animation_timer = 0;
	find_floor(&free_count, &y, &block_offset, &ignored, &object);
	OBJECT(current_object)->z = OBJECT(current_object)->floor_z;
	OBJECT(object_cursor)->z = OBJECT(current_object)->floor_z;
	project_object(&ignored, &ignored);
	allocate_object_2(&free_count, &ignored, &object);
	if (free_count == 0u)
		return;
	OBJECT(object)->sprite_flags = 0u;
	if ((OBJECT(current_object)->sprite_flags & 0x40u) != 0u)
		OBJECT(object)->sprite_flags |= 0x40u;
	OBJECT(object)->sprite_frame = 0x7538u;
	OBJECT(object)->sprite_palette = 0xF020u;
	OBJECT(object)->sprite_scale = 1u;
	OBJECT(current_object)->child = object;
}

void rimmed_sinkhole_open(void)
{
	uint16_t child = 0;
	uint32_t r7 = 0, depth = 0, ignored = 0, margin = 0, off_screen = 0, sound_id = 0, unseen_margin = 0;

	margin = 0x58u;
	unseen_margin = 0x20u;
	off_screen = check_left_screen_margins((uint16_t)margin, (uint16_t)unseen_margin);
	if (off_screen != 0u)
	{
		child = OBJECT(current_object)->child;
		free_object_2(child);
		current_object = (current_object & ~0xFFFFu) | OBJECT(current_object)->rimmed_sinkhole.edge_zone;
		OBJECT(current_object)->flags |= 0x400u;
		delete_current_object();
		return;
	}
	if (scroll_position < OBJECT(current_object)->rimmed_sinkhole.open_position)
		return;
	if ((int16_t)OBJECT(current_object)->sprite_scale < 0x100)
	{
		if (OBJECT(current_object)->sprite_scale == 1u)
		{
			sound_id = 0xFu;
			play_sound((uint8_t)sound_id, &margin, &unseen_margin);
		}
		OBJECT(current_object)->sprite_scale += OBJECT(current_object)->rimmed_sinkhole.zoom_step;
		OBJECT(LINKED(current_object, OBJECT(current_object)->child))->sprite_scale = OBJECT(current_object)->sprite_scale;
		animate(&margin, &r7, &off_screen);
		OBJECT(current_object)->hitbox_size_x = (int32_t)((uint32_t)OBJECT(current_object)->rimmed_sinkhole.unscaled_hitbox_size_x * OBJECT(current_object)->sprite_scale) >> 8;
	}
	add_to_touchable_list(sound_id);
	update_screen_position(&ignored, &margin);
	depth = (uint16_t)(OBJECT(current_object)->screen_y - 0x30u);
	if ((int16_t)depth <= -0xF)
		depth = 0xFFF1u;
	OBJECT(current_object)->rimmed_sinkhole.draw_depth = depth;
	queue_sprite_at_depth(&ignored, &depth);
	current_object = LINKED(current_object, OBJECT(current_object)->child);
	OBJECT(current_object)->screen_x = OBJECT(object_cursor)->screen_x;
	OBJECT(current_object)->screen_y = OBJECT(object_cursor)->screen_y;
	depth = (uint16_t)(OBJECT(object_cursor)->rimmed_sinkhole.draw_depth - 8u);
	queue_sprite_at_depth(&ignored, &depth);
}

void rimmed_sinkhole_edge_zone_update(void)
{
	uint32_t r0 = 0, object = 0;

	if ((OBJECT(current_object)->flags & 0x400u) == 0u)
	{
		object = LINKED(current_object, OBJECT(current_object)->parent);
		OBJECT(current_object)->sprite_scale = OBJECT(object)->sprite_scale;
		OBJECT(current_object)->rimmed_sinkhole.draw_depth = OBJECT(object)->var_88;
		r0 = add_to_touchable_list(0u);
		return;
	}
	delete_current_object();
}

void spawn_sand_block_leader(uint32_t record, uint32_t *p_spawned, uint32_t *p_leader)
{
	uint32_t leader = *p_leader;
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
	OBJECT(leader)->x_int = mem_read16(record);
	OBJECT(leader)->x_fraction = 0u;
	record += 2u;
	OBJECT(leader)->y_int = mem_read16(record);
	OBJECT(leader)->y_fraction = 0u;
	record += 2u;
	OBJECT(leader)->z_int = mem_read16(record);
	OBJECT(leader)->z_fraction = 0u;
	OBJECT(leader)->floor_z = OBJECT(leader)->z;
	record += 2u;
	OBJECT(leader)->direction = mem_read16(record);
	OBJECT(leader)->sand_block_leader.path = mem_read16(record + 2u) + group_leader_data_ADDRESS;
	OBJECT(leader)->handler = sand_block_leader_move_ADDRESS;
	OBJECT(leader)->flags = 0u;
	OBJECT(leader)->sprite_offset_x = 0;
	OBJECT(leader)->sprite_offset_y = 0;
	OBJECT(leader)->sand_block_leader.holding_back = 0u;
	OBJECT(leader)->sand_block_leader.wait_timer = 1;
	sand_block_leader_active = 0xFFu;
	if (group_leader_0_held != 0u)
		OBJECT(leader)->handler = sand_block_leader_wait_ADDRESS;
	spawned = 0xFFFFFFFFu;
	*p_spawned = spawned;
	*p_leader = leader;
}

void sand_block_leader_wait(void)
{
	if (scroll_position >= 0x5100u || group_leader_0_held == 0u)
		OBJECT(current_object)->handler = sand_block_leader_move_ADDRESS;
}

void sand_block_leader_move(void)
{
	uint16_t move_speed = 0;
	uint32_t r0 = 0, r3 = 0, block_offset = 0, block_record = 0, hold_threshold = 0, ignored = 0, path_cursor = 0;
	uint8_t hold_timer = 0;
	uint16_t wait_timer = 0, record_scroll = 0;
	uint32_t flags = 0;

	flags = OBJECT(current_object)->flags;
	OBJECT(current_object)->flags = flags | 0x80000000u;
	if ((flags & 0x80000000u) == 0u)
	{
	sand_block_leader_read_script:
		wait_timer = OBJECT(current_object)->sand_block_leader.wait_timer;
		OBJECT(current_object)->sand_block_leader.wait_timer = wait_timer - 1u;
		if ((int16_t)wait_timer <= 1)
		{
			OBJECT(current_object)->sand_block_leader.wait_timer = 0;
			path_cursor = OBJECT(current_object)->sand_block_leader.path;
			record_scroll = mem_read16(path_cursor);
			path_cursor += 2u;
			if (record_scroll <= scroll_position)
			{
				if ((int16_t)mem_read16(path_cursor + 2u) < 0)
				{
					delete_current_object();
					return;
				}
				OBJECT(current_object)->sand_block_leader.screen_threshold = mem_read16(path_cursor);
				path_cursor += 2u;
				OBJECT(current_object)->sand_block_leader.catch_up_factor = mem_read16(path_cursor);
				path_cursor += 2u;
				OBJECT(current_object)->sand_block_leader.base_speed = mem_read16(path_cursor);
				path_cursor += 2u;
				OBJECT(current_object)->sand_block_leader.wait_timer = mem_read16(path_cursor);
				OBJECT(current_object)->sand_block_leader.path = path_cursor + 2u;
			}
		}
	}
	else if (OBJECT(current_object)->sand_block_leader.holding_back == 0u)
	{
		if (no_player_in_play == 0u && player_mishap == 0u && player_dropping_in == 0u)
			goto sand_block_leader_read_script;
		OBJECT(current_object)->sand_block_leader.holding_back = 0xFFu;
		OBJECT(current_object)->sand_block_leader.saved_base_speed = OBJECT(current_object)->sand_block_leader.base_speed;
		OBJECT(current_object)->sand_block_leader.base_speed = 0;
		OBJECT(current_object)->sand_block_leader.hold_start_position = scroll_position;
		OBJECT(current_object)->sand_block_leader.hold_timer = 0xFFu;
		OBJECT(current_object)->sand_block_leader.saved_threshold = OBJECT(current_object)->sand_block_leader.screen_threshold;
		hold_threshold = 0xFFE0u;
		if ((OBJECT(current_object)->direction & 1u) != 0u)
			hold_threshold = 0x160u;
		OBJECT(current_object)->sand_block_leader.screen_threshold = hold_threshold;
	}
	find_floor(&r0, &hold_threshold, &block_offset, &r3, &block_record);
	project_object(&ignored, &ignored);
	add_to_rider_list();
	if (OBJECT(current_object)->sand_block_leader.holding_back != 0u)
	{
		if ((int16_t)(scroll_position - OBJECT(current_object)->sand_block_leader.hold_start_position) >= 0x300)
			goto sand_block_leader_stop_holding_back;
		if (no_player_in_play != 0u)
			OBJECT(current_object)->sand_block_leader.hold_timer = 0xFFu;
		hold_timer = OBJECT(current_object)->sand_block_leader.hold_timer;
		OBJECT(current_object)->sand_block_leader.hold_timer = hold_timer - 1u;
		if (hold_timer == 0u)
			goto sand_block_leader_stop_holding_back;
	}
	switch (OBJECT(current_object)->direction)
	{
	case 0:
	case 2:
	case 4:
		if (OBJECT(current_object)->sand_block_leader.screen_threshold <= OBJECT(current_object)->screen_x)
		{
		sand_block_leader_use_base_speed:
			move_speed = (uint16_t)OBJECT(current_object)->sand_block_leader.base_speed;
		}
		else
		{
		sand_block_leader_catch_up:
			OBJECT(current_object)->sand_block_leader.wait_timer = 0;
			if (OBJECT(current_object)->sand_block_leader.holding_back != 0u)
			{
			sand_block_leader_stop_holding_back:
				OBJECT(current_object)->sand_block_leader.holding_back = 0u;
				OBJECT(current_object)->sand_block_leader.screen_threshold = OBJECT(current_object)->sand_block_leader.saved_threshold;
				OBJECT(current_object)->sand_block_leader.base_speed = OBJECT(current_object)->sand_block_leader.saved_base_speed;
			}
			compute_camera_ground_speed(r3);
			move_speed = (uint16_t)((uint32_t)camera_ground_speed_int * OBJECT(current_object)->sand_block_leader.catch_up_factor) >> 8;
			if ((int16_t)move_speed <= OBJECT(current_object)->sand_block_leader.base_speed)
				goto sand_block_leader_use_base_speed;
		}
		break;
	case 1:
	case 3:
		if (OBJECT(current_object)->sand_block_leader.screen_threshold < OBJECT(current_object)->screen_x || OBJECT(current_object)->screen_y > 0xE0)
			goto sand_block_leader_catch_up;
		goto sand_block_leader_use_base_speed;
	default:
		unknown_code(0x8A046u + OBJECT(current_object)->direction * 2u);
	}
	chaser_speed = move_speed;
	if (OBJECT(current_object)->sand_block_leader.wait_timer <= 0)
	{
		block_record = floor_blocks + 2u + OBJECT(current_object)->floor_block;
		switch (OBJECT(current_object)->direction)
		{
		case 0:
		case 4:
			OBJECT(current_object)->y += (uint32_t)chaser_speed << 12;
			OBJECT(current_object)->x = ((int32_t)(mem_read32(block_record + 4u) - mem_read32(block_record)) >> 1) + mem_read32(block_record);
			break;
		case 1:
			OBJECT(current_object)->y -= (uint32_t)chaser_speed << 12;
			OBJECT(current_object)->x = ((int32_t)(mem_read32(block_record + 4u) - mem_read32(block_record)) >> 1) + mem_read32(block_record);
			break;
		case 2:
			OBJECT(current_object)->x += (uint32_t)chaser_speed << 12;
			OBJECT(current_object)->y = ((int32_t)(mem_read32(block_record + 0xCu) - mem_read32(block_record + 8u)) >> 1) + mem_read32(block_record + 8u);
			break;
		case 3:
			OBJECT(current_object)->x -= (uint32_t)chaser_speed << 12;
			OBJECT(current_object)->y = ((int32_t)(mem_read32(block_record + 0xCu) - mem_read32(block_record + 8u)) >> 1) + mem_read32(block_record + 8u);
			break;
		default:
			unknown_code(0x8A0D4u + OBJECT(current_object)->direction * 2u);
		}
	}
	if (OBJECT(current_object)->y_int <= 0xA2B)
		sand_block_leader_active = 0u;
}

void sand_block_update(void)
{
	uint8_t sound_id = 0;
	uint16_t child = 0, part_scale = 0, span_y = 0;
	uint32_t r1 = 0, r2 = 0, blocks_left = 0, depth = 0, floor_z = 0, ignored = 0, object = 0, object_2 = 0, object_3 = 0, part_cursor = 0, part_list = 0, point_x = 0, point_y = 0, stream_parts_left = 0;

	switch (OBJECT(current_object)->sand_block.state)
	{
	case 0:
	case 4:
		project_object(&ignored, &r1);
		OBJECT(current_object)->sand_block.state++;
		current_player = current_object;
		current_player = LINKED(current_player, OBJECT(current_object)->parent);
		switch (OBJECT(current_object)->direction)
		{
		case 0:
		case 1:
		case 2:
		case 3:
		case 4:
			break;
		default:
			unknown_code(0x8A1A5u + OBJECT(current_object)->direction * 2u);
		}
		if (OBJECT(current_player)->y_int <= (int16_t)(OBJECT(current_object)->y_int + OBJECT(current_object)->hitbox_size_y - 0x10u))
			OBJECT(current_object)->sand_block.state++;
		add_to_platform_list();
		update_screen_position(&ignored, &r1);
		if ((uint16_t)(OBJECT(current_object)->screen_x + 0x140u) >= 0x281u)
			return;
		depth = (uint16_t)(OBJECT(current_object)->screen_y + OBJECT(current_object)->sand_block.depth_offset - OBJECT(current_object)->sprite_offset_y);
		OBJECT(current_object)->sand_block.draw_depth = depth;
		queue_sprite_at_depth(&ignored, &depth);
		return;
	case 1:
		current_player = current_object;
		current_player = LINKED(current_player, OBJECT(current_object)->parent);
		switch (OBJECT(current_object)->direction)
		{
		case 0:
		case 1:
		case 2:
		case 3:
		case 4:
			break;
		default:
			unknown_code(0x8A1A5u + OBJECT(current_object)->direction * 2u);
		}
		if (OBJECT(current_player)->y_int <= (int16_t)(OBJECT(current_object)->y_int + OBJECT(current_object)->hitbox_size_y - 0x10u))
			OBJECT(current_object)->sand_block.state++;
		add_to_platform_list();
		update_screen_position(&ignored, &r1);
		if ((uint16_t)(OBJECT(current_object)->screen_x + 0x140u) >= 0x281u)
			return;
		depth = (uint16_t)(OBJECT(current_object)->screen_y + OBJECT(current_object)->sand_block.depth_offset - OBJECT(current_object)->sprite_offset_y);
		OBJECT(current_object)->sand_block.draw_depth = depth;
		queue_sprite_at_depth(&ignored, &depth);
		return;
	case 2:
		if (free_object_stack < 2 || free_object_stack_2[0] < 3)
		{
			add_to_platform_list();
			update_screen_position(&ignored, &r1);
			if ((uint16_t)(OBJECT(current_object)->screen_x + 0x140u) >= 0x281u)
				return;
			depth = (uint16_t)(OBJECT(current_object)->screen_y + OBJECT(current_object)->sand_block.depth_offset - OBJECT(current_object)->sprite_offset_y);
			OBJECT(current_object)->sand_block.draw_depth = depth;
			queue_sprite_at_depth(&ignored, &depth);
			return;
		}
		OBJECT(current_object)->sand_block.state++;
		allocate_object(&ignored, &object_2);
		OBJECT(object_2)->sprite_flags = 0xD002u;
		OBJECT(object_2)->handler = sand_face_update_ADDRESS;
		OBJECT(object_2)->flags = 0u;
		OBJECT(object_2)->speed_z = 0;
		OBJECT(object_2)->var_96 = 0x70u;
		OBJECT(object_2)->animation_frame = 0u;
		OBJECT(object_2)->animation_speed = 2u;
		OBJECT(object_2)->animation_timer = 0;
		OBJECT(object_2)->animation = sand_face_animation_ADDRESS;
		OBJECT(object_2)->x = OBJECT(current_object)->x;
		OBJECT(object_2)->y = OBJECT(current_object)->y;
		OBJECT(object_2)->sprite_offset_x = -6;
		OBJECT(object_2)->sprite_offset_y = 0x10;
		OBJECT(object_2)->z = OBJECT(current_object)->z;
		OBJECT(object_2)->parent = current_object;
		OBJECT(current_object)->sand_block.face_object = object_2;
		object_3 = object_2;
		allocate_object_2(&ignored, &ignored, &object_2);
		move_halfwords_backward(object_2, sand_face_parts_ADDRESS, 9u);
		part_list = OBJECT(current_object)->part_list;
		OBJECT(object_2)->sprite_scale = mem_read16(part_list + 8u);
		OBJECT(object_2)->hitbox_size_x = mem_read16(part_list + 8u);
		OBJECT(object_3)->part_list = object_2;
		allocate_object(&ignored, &object_2);
		OBJECT(object_2)->sprite_flags = 0xD002u;
		OBJECT(object_2)->handler = sand_streams_update_ADDRESS;
		OBJECT(object_2)->flags = 0u;
		move_halfwords_backward(object_2 + 0x40u, sand_stream_parts_ADDRESS, 0x19u);
		OBJECT(object_2)->part_list = object_2 + 0x40u;
		part_scale = (uint16_t)mem_read32(OBJECT(object_3)->part_list + 8u);
		part_cursor = OBJECT(object_2)->part_list;
		stream_parts_left = mem_read16(part_cursor);
		part_cursor += 2u;
		do
		{
			mem_write16(part_cursor + 6u, part_scale);
			part_cursor += 8u;
			stream_parts_left--;
		} while (stream_parts_left != 0u);
		OBJECT(object_2)->parent = object_3;
		OBJECT(object_3)->child = object_2;
		blocks_left = 2u;
		object_2 = current_object;
		do
		{
			object_3 = object_2;
			allocate_object_2(&ignored, &r2, &object_2);
			object = object_2 + 2u;
			OBJECT(object - 0x2u)->sprite_flags = 0x5000u;
			object += 2u;
			OBJECT(object - 0x4u)->sprite_frame = 0xE0u;
			object += 2u;
			OBJECT(object - 0x6u)->sprite_palette = 0u;
			object += 2u;
			OBJECT(object - 0x8u)->sprite_scale = 0x140u;
			object += 2u;
			OBJECT(object - 0xAu)->sprite_layer = 0u;
			object += 2u;
			OBJECT(object - 0xCu)->screen_y = 0;
			object += 2u;
			OBJECT(object - 0xEu)->screen_x = 0;
			OBJECT(object - 0xEu)->hitbox_size_x = 0u;
			OBJECT(object_2)->parent = object_3;
			OBJECT(object_3)->child = object_2;
			blocks_left--;
		} while (blocks_left != 0u);
		if (OBJECT(current_object)->screen_x <= 0x148 && OBJECT(current_object)->screen_y <= 0x130)
		{
			sound_id = 0x11u;
			play_sound(sound_id, &r1, &r2);
		}
	case 3:
		update_screen_position(&ignored, &r1);
		object = LINKED(current_object, OBJECT(current_object)->parent);
		span_y = (uint16_t)(OBJECT(object)->y_int - OBJECT(current_object)->y_int);
		OBJECT(current_object)->hitbox_size_y = span_y;
		if ((int16_t)span_y > 0)
		{
			point_x = OBJECT(current_object)->x + ((uint32_t)OBJECT(current_object)->hitbox_size_x << 16);
			point_y = OBJECT(object)->y;
			floor_z = OBJECT(current_object)->floor_z;
			project_point(floor_z, &ignored, &point_x, &point_y);
			OBJECT(LINKED(current_object, OBJECT(current_object)->child))->sprite_scale = point_x;
			add_to_platform_list();
			if ((uint16_t)(OBJECT(current_object)->screen_x + 0x140u) < 0x281u)
			{
				OBJECT(current_object)->sand_block.draw_depth = OBJECT(current_object)->screen_y + OBJECT(current_object)->sand_block.depth_offset - OBJECT(current_object)->sprite_offset_y;
				current_object = LINKED(current_object, OBJECT(current_object)->child);
				current_object = LINKED(current_object, OBJECT(current_object)->child);
				depth = OBJECT(object_cursor)->sand_block.draw_depth;
				queue_sprite_at_depth(&ignored, &depth);
				current_object = object_cursor;
				depth = OBJECT(object_cursor)->sand_block.draw_depth;
				queue_sprite_at_depth(&ignored, &depth);
				current_object = LINKED(current_object, OBJECT(current_object)->child);
				depth = OBJECT(object_cursor)->sand_block.draw_depth;
				queue_sprite_at_depth(&ignored, &depth);
			}
			return;
		}
		object = (current_object & ~0xFFFFu) | OBJECT(current_object)->sand_block.face_object;
		OBJECT(object)->flags |= 0x400u;
		child = OBJECT(current_object)->child;
		free_object_2(child);
		current_object = LINKED(current_object, OBJECT(current_object)->child);
		child = OBJECT(current_object)->child;
		free_object_2(child);
		delete_current_object();
		return;
	default:
		unknown_code(0x8A17Cu + OBJECT(current_object)->sand_block.state * 3u);
	}
}

void sand_face_update(void)
{
	uint16_t part_list = 0;
	uint32_t r8 = 0, depth = 0, ignored = 0, object = 0, part = 0;

	if ((OBJECT(current_object)->flags & 0x400u) == 0u)
	{
		object = LINKED(current_object, OBJECT(current_object)->parent);
		object = LINKED(object, OBJECT(object)->parent);
		OBJECT(current_object)->y = OBJECT(object)->y;
		OBJECT(current_object)->z = OBJECT(object)->z;
	}
	else
	{
		if (OBJECT(current_object)->screen_y >= 0x150)
		{
			part_list = (uint16_t)OBJECT(current_object)->part_list;
			free_object_2(part_list);
			object = LINKED(current_object, OBJECT(current_object)->child);
			OBJECT(object)->flags |= 0x400u;
			delete_current_object();
			return;
		}
		OBJECT(current_object)->sand_face.depth_offset = 0x38;
		OBJECT(current_object)->animation = sand_face_fall_animation_ADDRESS;
		OBJECT(current_object)->speed_z += 0x6000u;
		OBJECT(current_object)->z += OBJECT(current_object)->speed_z;
	}
	animate_all_parts(&r8, &part);
	project_object(&ignored, &ignored);
	depth = (uint16_t)(OBJECT(current_object)->screen_y - OBJECT(current_object)->sand_face.depth_offset);
	queue_sprite_at_depth(&ignored, &depth);
}

void sand_streams_update(void)
{
	uint16_t stream_frame = 0;
	uint32_t depth = 0, ignored = 0, object = 0, part_cursor = 0, parts_left = 0;

	if ((OBJECT(current_object)->flags & 0x400u) != 0u)
	{
		delete_current_object();
		return;
	}
	object = LINKED(current_object, OBJECT(current_object)->parent);
	OBJECT(current_object)->screen_x = OBJECT(object)->screen_x;
	OBJECT(current_object)->screen_x -= OBJECT(object)->sprite_offset_x;
	OBJECT(current_object)->screen_y = OBJECT(object)->screen_y;
	OBJECT(current_object)->screen_y -= OBJECT(object)->sprite_offset_y;
	stream_frame = sand_stream_frames[OBJECT(object)->animation_frame];
	part_cursor = OBJECT(current_object)->part_list;
	parts_left = mem_read16(part_cursor);
	part_cursor += 2u;
	do
	{
		mem_write16(part_cursor, stream_frame);
		part_cursor += 8u;
		parts_left--;
	} while (parts_left != 0u);
	depth = (uint16_t)(OBJECT(object)->screen_y - OBJECT(object)->var_96);
	queue_sprite_at_depth(&ignored, &depth);
}

uint32_t spawn_sink_swirl(uint32_t allocated)
{
	uint32_t effect_record = 0, ignored = 0, object = 0, swirls_left = 0;

	if (free_object_stack < 4)
		return allocated;
	effect_record = sink_swirl_effects_ADDRESS;
	swirls_left = 2u;
	do
	{
		allocate_object(&ignored, &allocated);
		OBJECT(allocated)->sprite_flags = 0x1004u;
		OBJECT(allocated)->sprite_palette = 0x1210u;
		AS16(OBJECT(allocated)->flags) = 0u;
		OBJECT(allocated)->sprite_offset_x = 0;
		OBJECT(allocated)->sprite_offset_y = 0;
		OBJECT(allocated)->sink_swirl.depth_offset = ROM16(effect_record);
		OBJECT(allocated)->handler = sink_swirl_start_ADDRESS;
		effect_record += 2u;
		OBJECT(allocated)->animation = ROM32(effect_record);
		OBJECT(allocated)->animation_frame = 0xFFu;
		OBJECT(allocated)->animation_speed = 0xAu;
		OBJECT(allocated)->animation_timer = 0;
		OBJECT(allocated)->x = OBJECT(current_object)->x;
		OBJECT(allocated)->y = OBJECT(current_object)->y;
		OBJECT(allocated)->z = OBJECT(current_object)->z;
		OBJECT(allocated)->z_int += 8u;
		object = allocated;
		effect_record += 4u;
		take_free_object(&ignored, &allocated);
		mem_write16(allocated + 2u, 0x1004u);
		mem_write16(allocated + 6u, 0x1230u);
		mem_write16(allocated + 0x30u, 0u);
		mem_write32(allocated + 0x48u, ROM32(effect_record));
		mem_write8(allocated + 0x46u, 0xFFu);
		mem_write8(allocated + 0x44u, 4u);
		mem_write8(allocated + 0x45u, 0u);
		OBJECT(object)->child = allocated;
		swirls_left--;
		effect_record += 4u;
	} while (swirls_left != 0u);
	return allocated;
}

void sink_swirl_start(void)
{
	uint32_t ignored = 0;

	project_object(&ignored, &ignored);
	OBJECT(current_object)->handler = sink_swirl_animate_ADDRESS;
	sink_swirl_animate();
}

void sink_swirl_animate(void)
{
	uint32_t r1 = 0, r7 = 0, at_last_step = 0, depth = 0, ignored = 0;

	animate(&r1, &r7, &at_last_step);
	if ((uint8_t)at_last_step != 0u)
	{
		current_object = LINKED(current_object, OBJECT(current_object)->child);
		push_free_object();
		delete_current_object();
		return;
	}
	update_screen_position(&ignored, &r1);
	current_object = LINKED(current_object, OBJECT(current_object)->child);
	animate(&r1, &r7, &at_last_step);
	OBJECT(current_object)->screen_x = OBJECT(object_cursor)->screen_x;
	OBJECT(current_object)->screen_y = OBJECT(object_cursor)->screen_y;
	depth = (uint16_t)(OBJECT(current_object)->screen_y + OBJECT(object_cursor)->sink_swirl.depth_offset);
	queue_sprite_at_depth(&ignored, &depth);
	current_object = object_cursor;
	depth = (uint16_t)(OBJECT(current_object)->screen_y + OBJECT(current_object)->sink_swirl.depth_offset);
	queue_sprite_at_depth(&ignored, &depth);
}

void brick_wall_init(void)
{
	uint32_t ignored = 0;

	OBJECT(current_object)->sprite_flags = 0x1002u;
	OBJECT(current_object)->sprite_frame = 0x7D90u;
	OBJECT(current_object)->sprite_palette = 0x10D0u;
	OBJECT(current_object)->sprite_offset_x = 0;
	OBJECT(current_object)->sprite_offset_y = 8;
	OBJECT(current_object)->flags = 0u;
	OBJECT(current_object)->floor_z = 0x1D80000;
	OBJECT(current_object)->z = OBJECT(current_object)->floor_z;
	OBJECT(current_object)->handler = brick_wall_wait_ADDRESS;
	project_object(&ignored, &ignored);
}

void brick_wall_wait(void)
{
	uint16_t screen_x = 0;
	uint32_t r1 = 0, brick_animation = 0, depth = 0, free_count = 0, ignored = 0, object = 0, piece_record = 0, pieces_left = 0;

	if (chaser_stopped == 0u)
	{
		update_screen_position(&ignored, &r1);
		depth = (uint16_t)(OBJECT(current_object)->screen_y - 0x10u);
		queue_sprite_at_depth(&ignored, &depth);
		return;
	}
	screen_x = (uint16_t)OBJECT(current_object)->screen_x;
	r14 = (uint16_t)OBJECT(current_object)->screen_y;
	pieces_left = brick_wall_bricks;
	piece_record = brick_wall_brick_records_ADDRESS;
	do
	{
		allocate_object(&free_count, &object);
		if (free_count == 0u)
		{
			delete_current_object();
			return;
		}
		OBJECT(object)->sprite_flags = ROM16(piece_record);
		OBJECT(object)->screen_x = screen_x;
		piece_record += 2u;
		OBJECT(object)->screen_x += ROM16(piece_record);
		OBJECT(object)->screen_y = r14;
		piece_record += 2u;
		OBJECT(object)->screen_y += ROM16(piece_record);
		piece_record += 2u;
		OBJECT(object)->speed_x_fraction = ROM16(piece_record);
		piece_record += 2u;
		OBJECT(object)->speed_y_fraction = ROM16(piece_record);
		piece_record += 2u;
		brick_animation = brick_animations_ADDRESS + ROM16(piece_record);
		OBJECT(object)->sprite_palette = ROM16(brick_animation);
		OBJECT(object)->animation = brick_animation + 2u;
		OBJECT(object)->projected_x = (uint32_t)OBJECT(object)->screen_x << 4;
		OBJECT(object)->projected_y_fraction = (uint32_t)OBJECT(object)->screen_y << 4;
		OBJECT(object)->animation_frame = 0u;
		OBJECT(object)->animation_speed = 4u;
		OBJECT(object)->animation_timer = 0;
		OBJECT(object)->flags = 0u;
		OBJECT(object)->sprite_scale = 0x100u;
		OBJECT(object)->handler = rock_debris_fly_ADDRESS;
		piece_record += 2u;
		pieces_left--;
	} while (pieces_left != 0u);
	screen_x = (uint16_t)OBJECT(current_object)->screen_x;
	r14 = (uint16_t)OBJECT(current_object)->screen_y;
	pieces_left = brick_wall_dust;
	piece_record = brick_wall_dust_records_ADDRESS;
	do
	{
		allocate_object(&free_count, &object);
		if (free_count == 0u)
			break;
		OBJECT(object)->sprite_flags = ROM16(piece_record);
		OBJECT(object)->screen_x = screen_x;
		piece_record += 2u;
		OBJECT(object)->screen_x += ROM16(piece_record);
		OBJECT(object)->screen_y = r14;
		piece_record += 2u;
		OBJECT(object)->screen_y += ROM16(piece_record);
		piece_record += 2u;
		AS16(OBJECT(object)->var_80) = ROM16(piece_record);
		OBJECT(object)->animation = brick_dust_animation_ADDRESS;
		OBJECT(object)->sprite_palette = 0x10E0u;
		OBJECT(object)->animation_frame = 0u;
		OBJECT(object)->animation_speed = 4u;
		OBJECT(object)->animation_timer = 0;
		OBJECT(object)->flags = 0u;
		OBJECT(object)->sprite_scale = 0x100u;
		OBJECT(object)->handler = brick_dust_animate_ADDRESS;
		piece_record += 2u;
		pieces_left--;
	} while (pieces_left != 0u);
	delete_current_object();
}

void brick_dust_animate(void)
{
	uint32_t r1 = 0, r7 = 0, at_last_step = 0;
	uint16_t wait_timer = 0;

	wait_timer = OBJECT(current_object)->brick_dust.wait_timer;
	OBJECT(current_object)->brick_dust.wait_timer = wait_timer - 1u;
	if ((int16_t)wait_timer > 1)
		return;
	OBJECT(current_object)->brick_dust.wait_timer = 0;
	animate(&r1, &r7, &at_last_step);
	if ((int8_t)at_last_step >= 0)
	{
		r1 = queue_sprite(r1);
		return;
	}
	delete_current_object();
}

void biting_head_init(void)
{
	uint32_t free_count = 0, head_record = 0, taken_object = 0;

	head_record = biting_head_paths_ADDRESS + biting_head_paths[OBJECT(current_object)->sprite_frame];
	OBJECT(current_object)->x_int = ROM16(head_record);
	OBJECT(current_object)->x_fraction = 0u;
	head_record += 2u;
	OBJECT(current_object)->y_int = ROM16(head_record);
	OBJECT(current_object)->y_fraction = 0u;
	OBJECT(current_object)->biting_head_view.path = head_record + 2u;
	OBJECT(current_object)->sprite_flags = 0xD004u;
	OBJECT(current_object)->flags = 0x40000000u;
	OBJECT(current_object)->biting_head_view.holding_back = 0u;
	OBJECT(current_object)->kind = 9u;
	OBJECT(current_object)->direction = 0x300u;
	OBJECT(current_object)->biting_head_view.wait_timer = 1;
	OBJECT(current_object)->hitbox_height = 0xF0u;
	OBJECT(current_object)->hitbox_size_x = 0x80u;
	OBJECT(current_object)->hitbox_size_y = 0x40u;
	OBJECT(current_object)->hitbox_offset_x = 0x20;
	OBJECT(current_object)->hitbox_offset_y = -0x20;
	OBJECT(current_object)->biting_head_view.bounce_index = -1;
	OBJECT(current_object)->floor_z = 0x1D80000;
	OBJECT(current_object)->z = 0x1D80000;
	AS8(OBJECT(current_object)->spawn_arg_2) = 0u;
	biting_head = current_object;
	biting_head_done = 0u;
	OBJECT(current_object)->animation_frame = 0u;
	OBJECT(current_object)->animation_timer = 0;
	OBJECT(current_object)->biting_head_view.bite_step = 0u;
	OBJECT(current_object)->biting_head_view.bite_timer = 0;
	take_free_object(&free_count, &taken_object);
	if (free_count != 0u)
	{
		move_halfwords_backward(taken_object, biting_head_parts_ADDRESS, 0x19u);
		OBJECT(current_object)->part_list = taken_object;
		move_halfwords_backward(taken_object + 0x40u, biting_head_clip_entry_ADDRESS, 0xAu);
		move_halfwords_backward(taken_object + 0x54u, biting_head_clip_entry_ADDRESS, 0xAu);
		take_free_object(&free_count, &taken_object);
		if (free_count != 0u)
		{
			mem_write16(taken_object + 0x10u, 0x200u);
			mem_write16(taken_object + 0x12u, 0x130u);
			mem_write16(taken_object + 0x24u, 0x300u);
			mem_write16(taken_object + 0x66u, 0xF0u);
			mem_write8(taken_object + 0x7Cu, 0u);
			mem_write8(taken_object + 0x7Du, 0x80u);
			mem_write32(taken_object + 0x30u, 0x40000000u);
			mem_write8(taken_object + 0x47u, 9u);
			OBJECT(current_object)->child = taken_object;
			OBJECT(current_object)->handler = biting_head_wait_ADDRESS;
		}
	}
}

void biting_head_wait(void)
{
	if (chaser_stopped == 0u)
	{
		if (scroll_position < 0x1200u)
			return;
		chaser_stopped = 0xFFu;
	}
	OBJECT(current_object)->handler = biting_head_chase_ADDRESS;
}

void biting_head_chase(void)
{
	uint16_t bounce_dy = 0, hold_threshold = 0, move_frame = 0, move_speed = 0, record_coordinate = 0;
	uint32_t bite_record = 0, depth = 0, direction = 0, floor_slope = 0, ignored = 0, move_frames = 0, next_bite_step = 0, next_frame = 0, object = 0, path_cursor = 0, point_x = 0, point_y = 0, slope_factor = 0, z = 0;
	uint8_t hold_timer = 0, animation_timer = 0, bob_phase = 0, bite_timer = 0;
	uint16_t wait_timer = 0, record_scroll = 0;
	uint32_t flags = 0;

	flags = OBJECT(current_object)->flags;
	OBJECT(current_object)->flags = flags | 0x80000000u;
	if ((flags & 0x80000000u) == 0u)
	{
	biting_head_read_script:
		wait_timer = OBJECT(current_object)->biting_head_view.wait_timer;
		OBJECT(current_object)->biting_head_view.wait_timer = wait_timer - 1u;
		if ((int16_t)wait_timer <= 1)
		{
			OBJECT(current_object)->biting_head_view.wait_timer = 0;
			path_cursor = OBJECT(current_object)->biting_head_view.path;
			record_scroll = mem_read16(path_cursor);
			path_cursor += 2u;
			if (record_scroll <= scroll_position)
			{
				path_cursor += 2u;
				goto biting_head_load_record;
			}
			record_coordinate = mem_read16(path_cursor);
			direction = OBJECT(current_object)->direction;
			path_cursor += 2u;
			switch (direction >> 8)
			{
			case 0:
			case 4:
				if (OBJECT(current_object)->y_int <= (int16_t)record_coordinate)
				{
				biting_head_load_record:
					if ((int16_t)mem_read16(path_cursor + 2u) < 0)
					{
						current_object = (current_object & ~0xFFFFu) | OBJECT(current_object)->sprite_frame;
						push_free_object();
						current_object = LINKED(current_object, OBJECT(object_cursor)->child);
						push_free_object();
						delete_current_object();
						return;
					}
					OBJECT(current_object)->biting_head_view.screen_threshold = mem_read16(path_cursor);
					path_cursor += 2u;
					OBJECT(current_object)->biting_head_view.catch_up_factor = mem_read16(path_cursor);
					path_cursor += 2u;
					OBJECT(current_object)->biting_head_view.base_speed = mem_read16(path_cursor);
					path_cursor += 2u;
					OBJECT(current_object)->biting_head_view.wait_timer = mem_read16(path_cursor);
					path_cursor += 2u;
					OBJECT(current_object)->direction = mem_read16(path_cursor);
					OBJECT(current_object)->biting_head_view.path = path_cursor + 2u;
				}
				break;
			case 1:
				if ((int16_t)record_coordinate <= OBJECT(current_object)->x_int)
					goto biting_head_load_record;
				break;
			case 2:
				if ((int16_t)record_coordinate <= OBJECT(current_object)->y_int)
					goto biting_head_load_record;
				break;
			case 3:
				if (OBJECT(current_object)->x_int <= (int16_t)record_coordinate)
					goto biting_head_load_record;
				break;
			default:
				unknown_code(0x8ABC3u + (direction >> 8) * 2u);
			}
		}
	}
	else if (OBJECT(current_object)->biting_head_view.holding_back == 0u)
	{
		if (no_player_in_play == 0u && player_mishap == 0u && player_dropping_in == 0u)
			goto biting_head_read_script;
		OBJECT(current_object)->biting_head_view.holding_back = 0xFFu;
		OBJECT(current_object)->biting_head_view.saved_base_speed = OBJECT(current_object)->biting_head_view.base_speed;
		OBJECT(current_object)->biting_head_view.base_speed = 0;
		OBJECT(current_object)->biting_head_view.hold_start_position = scroll_position;
		OBJECT(current_object)->biting_head_view.hold_timer = 0xFFu;
		OBJECT(current_object)->biting_head_view.saved_threshold = OBJECT(current_object)->biting_head_view.screen_threshold;
		hold_threshold = 0xFFE0u;
		if (OBJECT(current_object)->direction < 0x100 || OBJECT(current_object)->direction > 0x200)
			hold_threshold = 0x160u;
		OBJECT(current_object)->biting_head_view.screen_threshold = hold_threshold;
	}
	project_object(&ignored, &ignored);
	object = LINKED(current_object, OBJECT(current_object)->child);
	OBJECT(object)->x = OBJECT(current_object)->x;
	OBJECT(object)->x_int += 0x40u;
	OBJECT(object)->y = OBJECT(current_object)->y;
	OBJECT(object)->z = OBJECT(current_object)->z;
	current_object = LINKED(current_object, OBJECT(current_object)->child);
	add_to_zone_list(ignored);
	current_object = object_cursor;
	add_to_zone_list(ignored);
	if (OBJECT(current_object)->biting_head_view.holding_back != 0u)
	{
		if ((int16_t)(scroll_position - OBJECT(current_object)->biting_head_view.hold_start_position) >= 0x300)
			goto biting_head_stop_holding_back;
		if (game_mode != 3u)
			OBJECT(current_object)->biting_head_view.hold_timer = 0xFFu;
		hold_timer = OBJECT(current_object)->biting_head_view.hold_timer;
		OBJECT(current_object)->biting_head_view.hold_timer = hold_timer - 1u;
		if (hold_timer == 0u)
			goto biting_head_stop_holding_back;
	}
	if (OBJECT(current_object)->screen_y < 0)
	{
	biting_head_catch_up:
		OBJECT(current_object)->biting_head_view.wait_timer = 0;
		if (OBJECT(current_object)->biting_head_view.holding_back != 0u)
		{
		biting_head_stop_holding_back:
			OBJECT(current_object)->biting_head_view.holding_back = 0u;
			OBJECT(current_object)->biting_head_view.screen_threshold = OBJECT(current_object)->biting_head_view.saved_threshold;
			OBJECT(current_object)->biting_head_view.base_speed = OBJECT(current_object)->biting_head_view.saved_base_speed;
		}
		slope_factor = 0u;
		if ((OBJECT(current_object)->floor_slope | OBJECT(current_object)->floor_type) != 0u)
		{
			floor_slope = OBJECT(current_object)->floor_slope;
			slope_factor = floor_slope_factors[(uint16_t)(floor_slope * 2u)];
		}
		compute_camera_slope_speed(0u, slope_factor);
		move_speed = (uint16_t)((uint32_t)camera_slope_speed_int * OBJECT(current_object)->biting_head_view.catch_up_factor) >> 8;
		if ((int16_t)move_speed <= OBJECT(current_object)->biting_head_view.base_speed)
		{
		biting_head_use_base_speed:
			move_speed = (uint16_t)OBJECT(current_object)->biting_head_view.base_speed;
		}
	}
	else
	{
		direction = OBJECT(current_object)->direction;
		switch (direction >> 8)
		{
		case 0:
		case 3:
			if (OBJECT(current_object)->screen_x <= OBJECT(current_object)->biting_head_view.screen_threshold)
				goto biting_head_use_base_speed;
			goto biting_head_catch_up;
		case 1:
		case 2:
		case 4:
			if (OBJECT(current_object)->biting_head_view.screen_threshold <= OBJECT(current_object)->screen_x)
				goto biting_head_use_base_speed;
			goto biting_head_catch_up;
		default:
			unknown_code(0x8ACAAu + (direction >> 8) * 2u);
		}
	}
	chaser_speed = move_speed;
	if (OBJECT(current_object)->biting_head_view.wait_timer <= 0)
	{
		direction = OBJECT(current_object)->direction;
		switch (direction >> 8)
		{
		case 0:
			OBJECT(current_object)->y -= (uint32_t)chaser_speed << 12;
			break;
		case 1:
			OBJECT(current_object)->x += (uint32_t)chaser_speed << 12;
			break;
		case 2:
		case 4:
			OBJECT(current_object)->y += (uint32_t)chaser_speed << 12;
			break;
		case 3:
			OBJECT(current_object)->x -= (uint32_t)chaser_speed << 12;
			break;
		default:
			unknown_code(0x8AD42u + (direction >> 8) * 2u);
		}
	}
	if (OBJECT(current_object)->biting_head_view.holding_back == 0u && (uint16_t)chaser_speed != 0u)
	{
		animation_timer = OBJECT(current_object)->animation_timer;
		OBJECT(current_object)->animation_timer = animation_timer - 1u;
		if ((int8_t)animation_timer <= 1)
		{
			OBJECT(current_object)->animation_timer = 2;
			next_frame = OBJECT(current_object)->animation_frame;
			next_frame = (uint8_t)(next_frame + 1u);
			move_frames = biting_head_move_frames_ADDRESS;
			if ((int8_t)biting_head_frames < (int8_t)next_frame)
				next_frame = 0u;
			OBJECT(current_object)->animation_frame = next_frame;
			move_frame = ROM16(move_frames + next_frame * 2u);
			mem_write16(OBJECT(current_object)->part_list + 2u, move_frame);
			mem_write16(OBJECT(current_object)->part_list + 0x22u, move_frame);
		}
		OBJECT(current_object)->biting_head_view.bob_counter++;
		bob_phase = OBJECT(current_object)->biting_head_view.bob_counter & 3u;
		OBJECT(current_object)->biting_head_view.bob_counter = bob_phase;
		if (bob_phase == 0u)
			OBJECT(current_object)->screen_y--;
	}
	bite_timer = OBJECT(current_object)->biting_head_view.bite_timer;
	OBJECT(current_object)->biting_head_view.bite_timer = bite_timer - 1u;
	if ((int8_t)bite_timer <= 1)
	{
		next_bite_step = OBJECT(current_object)->biting_head_view.bite_step;
		next_bite_step = (uint8_t)(next_bite_step + 1u);
		bite_record = biting_head_bite_step_records_ADDRESS;
		if ((int8_t)biting_head_bite_steps < (int8_t)next_bite_step)
			next_bite_step = 0u;
		OBJECT(current_object)->biting_head_view.bite_step = next_bite_step;
		bite_record += next_bite_step * 4u;
		mem_write16(OBJECT(current_object)->part_list + 0x1Au, ROM16(bite_record));
		OBJECT(current_object)->biting_head_view.bite_timer = ROM8(bite_record + 2u);
		if (OBJECT(current_object)->biting_head_view.bite_step == 5u)
			OBJECT(current_object)->biting_head_view.bounce_index = 0;
	}
	if (OBJECT(current_object)->biting_head_view.bounce_index >= 0)
	{
		OBJECT(current_object)->biting_head_view.bounce_index++;
		bounce_dy = (uint16_t)biting_head_bounce[(uint8_t)OBJECT(current_object)->biting_head_view.bounce_index];
		if ((uint8_t)bounce_dy != 0x7Fu)
		{
		biting_head_apply_bounce:
			OBJECT(current_object)->screen_y += bounce_dy;
		}
		else
		{
			OBJECT(current_object)->biting_head_view.bounce_index = -1;
			if ((uint8_t)bounce_dy != 0x7Fu)
				goto biting_head_apply_bounce;
		}
	}
	if (OBJECT(current_object)->x_int <= -0x355)
	{
		OBJECT(LINKED(current_object, OBJECT(current_object)->child))->hitbox_height = 0x50u;
		OBJECT(current_object)->hitbox_height = 0x50u;
	}
	if (OBJECT(current_object)->x_int <= -0x431)
		biting_head_done = 0xFFu;
	if (scroll_position >= 0x1500u)
	{
		depth = (uint16_t)(OBJECT(current_object)->screen_y - 0x50u);
		queue_sprite_at_depth(&ignored, &depth);
		return;
	}
	point_x = 0x8210000u;
	point_y = 0x5EC0000u;
	z = 0x1D80000u;
	project_point(z, &ignored, &point_x, &point_y);
	mem_write16(OBJECT(current_object)->part_list + 0x48u, point_x);
	depth = (uint16_t)(OBJECT(current_object)->screen_y - 0x50u);
	biting_head_depth = depth;
	current_object = OBJECT(current_object)->part_list;
	current_object = current_object + 0x54u;
	queue_sprite_at_depth(&ignored, &depth);
	current_object = object_cursor;
	depth = biting_head_depth;
	queue_sprite_at_depth(&ignored, &depth);
	current_object = OBJECT(current_object)->part_list;
	depth = biting_head_depth;
	current_object = current_object + 0x40u;
	queue_sprite_at_depth(&ignored, &depth);
}

void fire_front_init(void)
{
	uint16_t flame_offset_x = 0;
	uint32_t flames_left = 0, ignored = 0, object = 0, path = 0;

	path = fire_front_paths_ADDRESS + fire_front_paths[OBJECT(current_object)->sprite_frame];
	OBJECT(current_object)->x_int = ROM16(path);
	OBJECT(current_object)->x_fraction = 0u;
	path += 2u;
	OBJECT(current_object)->y_int = ROM16(path);
	OBJECT(current_object)->y_fraction = 0u;
	path += 2u;
	OBJECT(current_object)->floor_z_int = ROM16(path);
	OBJECT(current_object)->floor_z_fraction = 0u;
	OBJECT(current_object)->z = OBJECT(current_object)->floor_z;
	path += 2u;
	OBJECT(current_object)->direction = ROM16(path);
	OBJECT(current_object)->fire_front.path = path + 2u;
	OBJECT(current_object)->flags = 0x40000000u;
	OBJECT(current_object)->fire_front.holding_back = 0u;
	OBJECT(current_object)->sprite_offset_x = 0;
	OBJECT(current_object)->sprite_offset_y = 0;
	OBJECT(current_object)->kind = 0x14u;
	OBJECT(current_object)->fire_front.wait_timer = 1;
	OBJECT(current_object)->hitbox_height = 0xF0u;
	OBJECT(current_object)->handler = fire_front_chase_ADDRESS;
	OBJECT(current_object)->hitbox_size_x = 0x132u;
	OBJECT(current_object)->hitbox_size_y = 0x200u;
	OBJECT(current_object)->hitbox_offset_x = -0x40;
	OBJECT(current_object)->hitbox_offset_y = 0;
	OBJECT(current_object)->speed_x = 0;
	OBJECT(current_object)->speed_y = 0;
	chaser_object = current_object;
	if (free_object_stack < 3)
		return;
	flames_left = 4u;
	flame_offset_x = 0x28u;
	do
	{
		allocate_object(&ignored, &object);
		OBJECT(object)->handler = spreading_flame_start_ADDRESS;
		OBJECT(object)->x = OBJECT(current_object)->x;
		OBJECT(object)->y = OBJECT(current_object)->y;
		OBJECT(object)->x_int += flame_offset_x;
		flame_offset_x += 0x38u;
		flames_left--;
	} while (flames_left != 0u);
}

void fire_front_chase(void)
{
	uint16_t direction = 0, speed = 0, threshold = 0, trigger_coordinate = 0;
	uint32_t cursor = 0, direction_2 = 0, ignored = 0, step = 0;
	uint8_t hold_timer = 0;
	uint16_t wait_timer = 0, trigger_scroll = 0, hold_threshold = 0;
	uint32_t flags = 0;

	flags = OBJECT(current_object)->flags;
	OBJECT(current_object)->flags = flags | 0x80000000u;
	if ((flags & 0x80000000u) == 0u)
	{
	fire_front_read_script:
		wait_timer = OBJECT(current_object)->fire_front.wait_timer;
		OBJECT(current_object)->fire_front.wait_timer = wait_timer - 1u;
		if ((int16_t)wait_timer <= 1)
		{
			OBJECT(current_object)->fire_front.wait_timer = 0;
			cursor = OBJECT(current_object)->fire_front.path;
			trigger_scroll = mem_read16(cursor);
			cursor += 2u;
			if (trigger_scroll <= scroll_position)
			{
				cursor += 2u;
				goto fire_front_load_record;
			}
			trigger_coordinate = mem_read16(cursor);
			cursor += 2u;
			if (trigger_coordinate != 0u)
			{
				direction_2 = OBJECT(current_object)->direction;
				switch (direction_2 >> 8)
				{
				case 0:
				case 4:
					if ((int16_t)(OBJECT(current_object)->y_int + OBJECT(current_object)->hitbox_offset_y) <= (int16_t)trigger_coordinate)
					{
					fire_front_load_record:
						if ((int16_t)mem_read16(cursor + 2u) < 0)
						{
							chaser_object = 0u;
							delete_current_object();
							return;
						}
						OBJECT(current_object)->fire_front.screen_threshold = mem_read16(cursor);
						cursor += 2u;
						OBJECT(current_object)->fire_front.catch_up_factor = mem_read16(cursor);
						cursor += 2u;
						OBJECT(current_object)->fire_front.base_speed = mem_read16(cursor);
						cursor += 2u;
						OBJECT(current_object)->fire_front.wait_timer = mem_read16(cursor);
						cursor += 2u;
						if ((uint16_t)OBJECT(current_object)->fire_front.wait_timer != 0u)
						{
							OBJECT(current_object)->speed_x = 0;
							OBJECT(current_object)->speed_y = 0;
						}
						OBJECT(current_object)->fire_front.path = cursor;
					}
					break;
				case 1:
					if ((int16_t)trigger_coordinate <= OBJECT(current_object)->x_int)
						goto fire_front_load_record;
					break;
				case 2:
					if ((int16_t)trigger_coordinate <= OBJECT(current_object)->y_int)
						goto fire_front_load_record;
					break;
				case 3:
					if (OBJECT(current_object)->x_int <= (int16_t)trigger_coordinate)
						goto fire_front_load_record;
					break;
				default:
					unknown_code(0x8B05Du + (direction_2 >> 8) * 2u);
				}
			}
		}
	}
	else if (OBJECT(current_object)->fire_front.holding_back == 0u)
	{
		if (no_player_in_play == 0u && player_mishap == 0u && player_dropping_in == 0u)
			goto fire_front_read_script;
		OBJECT(current_object)->fire_front.holding_back = 0xFFu;
		OBJECT(current_object)->fire_front.saved_base_speed = OBJECT(current_object)->fire_front.base_speed;
		OBJECT(current_object)->fire_front.base_speed = 0;
		OBJECT(current_object)->speed_x = 0;
		OBJECT(current_object)->speed_y = 0;
		OBJECT(current_object)->fire_front.hold_start_position = scroll_position;
		OBJECT(current_object)->fire_front.hold_timer = 0xFFu;
		OBJECT(current_object)->fire_front.saved_threshold = OBJECT(current_object)->fire_front.screen_threshold;
		direction = OBJECT(current_object)->direction;
		hold_threshold = direction ^ (direction >> 1);
		threshold = hold_threshold;
		if (hold_threshold == 0u)
			threshold = 0x160u;
		OBJECT(current_object)->fire_front.screen_threshold = threshold;
	}
	project_object(&ignored, &ignored);
	add_to_zone_list(ignored);
	if (OBJECT(current_object)->fire_front.holding_back != 0u)
	{
		if ((int16_t)(scroll_position - OBJECT(current_object)->fire_front.hold_start_position) >= 0x300)
			goto fire_front_stop_holding_back;
		if (game_mode != 3u)
			OBJECT(current_object)->fire_front.hold_timer = 0xFFu;
		hold_timer = OBJECT(current_object)->fire_front.hold_timer;
		OBJECT(current_object)->fire_front.hold_timer = hold_timer - 1u;
		if (hold_timer == 0u)
			goto fire_front_stop_holding_back;
	}
	switch (OBJECT(current_object)->direction >> 8)
	{
	case 0:
	case 3:
		if (OBJECT(current_object)->screen_x <= OBJECT(current_object)->fire_front.screen_threshold)
		{
		fire_front_use_base_speed:
			speed = (uint16_t)OBJECT(current_object)->fire_front.base_speed;
		}
		else
		{
		fire_front_catch_up:
			OBJECT(current_object)->fire_front.wait_timer = 0;
			if (OBJECT(current_object)->fire_front.holding_back != 0u)
			{
			fire_front_stop_holding_back:
				OBJECT(current_object)->fire_front.holding_back = 0u;
				OBJECT(current_object)->fire_front.screen_threshold = OBJECT(current_object)->fire_front.saved_threshold;
				OBJECT(current_object)->fire_front.base_speed = OBJECT(current_object)->fire_front.saved_base_speed;
			}
			compute_camera_ground_speed(0u);
			speed = (uint16_t)((uint32_t)camera_ground_speed_int * OBJECT(current_object)->fire_front.catch_up_factor) >> 8;
			if ((int16_t)speed <= OBJECT(current_object)->fire_front.base_speed)
				goto fire_front_use_base_speed;
		}
		break;
	case 1:
	case 2:
	case 4:
		if (OBJECT(current_object)->fire_front.screen_threshold <= OBJECT(current_object)->screen_x)
			goto fire_front_use_base_speed;
		goto fire_front_catch_up;
	default:
		unknown_code(0x8B117u + (OBJECT(current_object)->direction >> 8) * 2u);
	}
	chaser_speed = speed;
	if (OBJECT(current_object)->fire_front.wait_timer <= 0)
	{
		switch (OBJECT(current_object)->direction >> 8)
		{
		case 0:
			step = (uint32_t)chaser_speed << 12;
			OBJECT(current_object)->y -= step;
			OBJECT(current_object)->speed_y = 0u - step;
			OBJECT(current_object)->speed_x = 0;
			break;
		case 1:
			step = (uint32_t)chaser_speed << 12;
			OBJECT(current_object)->x += step;
			OBJECT(current_object)->speed_x = step;
			OBJECT(current_object)->speed_y = 0;
			break;
		case 2:
		case 4:
			step = (uint32_t)chaser_speed << 12;
			OBJECT(current_object)->y += step;
			OBJECT(current_object)->speed_y = step;
			OBJECT(current_object)->speed_x = 0;
			break;
		case 3:
			step = (uint32_t)chaser_speed << 12;
			OBJECT(current_object)->x -= step;
			OBJECT(current_object)->speed_x = 0u - step;
			OBJECT(current_object)->speed_y = 0;
			break;
		default:
			unknown_code(0x8B193u + (OBJECT(current_object)->direction >> 8) * 2u);
		}
	}
	if (mem_read16(OBJECT(current_object)->fire_front.path - 6u) == 0u)
	{
		OBJECT(current_object)->hitbox_size_x = 0xF2u;
		OBJECT(current_object)->hitbox_offset_x = 0;
		chaser_stopped = 0xFFu;
	}
}

void falling_grate_update(void)
{
	uint8_t sound_id = 0;
	uint32_t r1 = 0, r2 = 0, r7 = 0, at_last_step = 0, depth = 0, ignored = 0;
	uint8_t animation_timer = 0;

	switch (OBJECT(current_object)->falling_grate.state)
	{
	case 0:
	case 3:
		OBJECT(current_object)->falling_grate.state++;
		OBJECT(current_object)->falling_grate.rattle_step = 1;
		project_object(&ignored, &r1);
	case 1:
		update_screen_position(&ignored, &r1);
		current_player = current_object;
		current_player = LINKED(current_player, OBJECT(current_object)->parent);
		switch (OBJECT(current_object)->direction)
		{
		case 0:
		case 4:
			if (OBJECT(current_object)->y_int <= OBJECT(current_player)->y_int)
			{
				OBJECT(current_object)->falling_grate.state++;
				OBJECT(current_object)->speed_z = 0;
				OBJECT(current_object)->animation_speed = 6u;
				OBJECT(current_object)->animation_timer = 0;
				OBJECT(current_object)->animation_frame = 0xFFu;
				OBJECT(current_object)->animation = falling_grate_animation_ADDRESS;
				sound_id = 0xDu;
				play_sound(sound_id, &r1, &r2);
			}
			else if (OBJECT(current_object)->y_int <= (int16_t)(OBJECT(current_player)->y_int + 0x60u))
			{
				animation_timer = OBJECT(current_object)->animation_timer;
				OBJECT(current_object)->animation_timer = animation_timer - 1u;
				if ((int8_t)animation_timer <= 1)
				{
					OBJECT(current_object)->animation_timer = OBJECT(current_object)->animation_speed;
					OBJECT(current_object)->floor_z_int += OBJECT(current_object)->falling_grate.rattle_step;
					OBJECT(current_object)->z_int += OBJECT(current_object)->falling_grate.rattle_step;
					OBJECT(current_object)->falling_grate.rattle_step = 0u - OBJECT(current_object)->falling_grate.rattle_step;
				}
			}
			break;
		case 1:
			if (OBJECT(current_player)->y_int <= OBJECT(current_object)->y_int)
			{
				OBJECT(current_object)->falling_grate.state++;
				OBJECT(current_object)->speed_z = 0;
				OBJECT(current_object)->animation_speed = 6u;
				OBJECT(current_object)->animation_timer = 0;
				OBJECT(current_object)->animation_frame = 0xFFu;
				OBJECT(current_object)->animation = falling_grate_animation_ADDRESS;
				sound_id = 0xDu;
				play_sound(sound_id, &r1, &r2);
				add_to_platform_list();
				if ((uint16_t)(OBJECT(current_object)->screen_x + 0xA0u) >= 0x281u)
					return;
				depth = (uint16_t)(OBJECT(current_object)->screen_y + OBJECT(current_object)->falling_grate.depth_offset - OBJECT(current_object)->sprite_offset_y);
				OBJECT(current_object)->falling_grate.draw_depth = depth;
				queue_sprite_at_depth(&ignored, &depth);
				return;
			}
			if ((int16_t)(OBJECT(current_player)->y_int - 0x60u) <= OBJECT(current_object)->y_int)
			{
				animation_timer = OBJECT(current_object)->animation_timer;
				OBJECT(current_object)->animation_timer = animation_timer - 1u;
				if ((int8_t)animation_timer <= 1)
				{
					OBJECT(current_object)->animation_timer = OBJECT(current_object)->animation_speed;
					OBJECT(current_object)->floor_z_int += OBJECT(current_object)->falling_grate.rattle_step;
					OBJECT(current_object)->z_int += OBJECT(current_object)->falling_grate.rattle_step;
					OBJECT(current_object)->falling_grate.rattle_step = 0u - OBJECT(current_object)->falling_grate.rattle_step;
				}
				add_to_platform_list();
				if ((uint16_t)(OBJECT(current_object)->screen_x + 0xA0u) >= 0x281u)
					return;
				depth = (uint16_t)(OBJECT(current_object)->screen_y + OBJECT(current_object)->falling_grate.depth_offset - OBJECT(current_object)->sprite_offset_y);
				OBJECT(current_object)->falling_grate.draw_depth = depth;
				queue_sprite_at_depth(&ignored, &depth);
				return;
			}
			break;
		case 2:
			if (OBJECT(current_object)->x_int <= OBJECT(current_player)->x_int)
			{
				OBJECT(current_object)->falling_grate.state++;
				OBJECT(current_object)->speed_z = 0;
				OBJECT(current_object)->animation_speed = 6u;
				OBJECT(current_object)->animation_timer = 0;
				OBJECT(current_object)->animation_frame = 0xFFu;
				OBJECT(current_object)->animation = falling_grate_animation_ADDRESS;
				sound_id = 0xDu;
				play_sound(sound_id, &r1, &r2);
				add_to_platform_list();
				if ((uint16_t)(OBJECT(current_object)->screen_x + 0xA0u) >= 0x281u)
					return;
				depth = (uint16_t)(OBJECT(current_object)->screen_y + OBJECT(current_object)->falling_grate.depth_offset - OBJECT(current_object)->sprite_offset_y);
				OBJECT(current_object)->falling_grate.draw_depth = depth;
				queue_sprite_at_depth(&ignored, &depth);
				return;
			}
			if (OBJECT(current_object)->x_int <= (int16_t)(OBJECT(current_player)->x_int + 0x60u))
			{
				animation_timer = OBJECT(current_object)->animation_timer;
				OBJECT(current_object)->animation_timer = animation_timer - 1u;
				if ((int8_t)animation_timer <= 1)
				{
					OBJECT(current_object)->animation_timer = OBJECT(current_object)->animation_speed;
					OBJECT(current_object)->floor_z_int += OBJECT(current_object)->falling_grate.rattle_step;
					OBJECT(current_object)->z_int += OBJECT(current_object)->falling_grate.rattle_step;
					OBJECT(current_object)->falling_grate.rattle_step = 0u - OBJECT(current_object)->falling_grate.rattle_step;
				}
				add_to_platform_list();
				if ((uint16_t)(OBJECT(current_object)->screen_x + 0xA0u) >= 0x281u)
					return;
				depth = (uint16_t)(OBJECT(current_object)->screen_y + OBJECT(current_object)->falling_grate.depth_offset - OBJECT(current_object)->sprite_offset_y);
				OBJECT(current_object)->falling_grate.draw_depth = depth;
				queue_sprite_at_depth(&ignored, &depth);
				return;
			}
			break;
		case 3:
			if (OBJECT(current_player)->x_int <= OBJECT(current_object)->x_int)
			{
				OBJECT(current_object)->falling_grate.state++;
				OBJECT(current_object)->speed_z = 0;
				OBJECT(current_object)->animation_speed = 6u;
				OBJECT(current_object)->animation_timer = 0;
				OBJECT(current_object)->animation_frame = 0xFFu;
				OBJECT(current_object)->animation = falling_grate_animation_ADDRESS;
				sound_id = 0xDu;
				play_sound(sound_id, &r1, &r2);
				add_to_platform_list();
				if ((uint16_t)(OBJECT(current_object)->screen_x + 0xA0u) >= 0x281u)
					return;
				depth = (uint16_t)(OBJECT(current_object)->screen_y + OBJECT(current_object)->falling_grate.depth_offset - OBJECT(current_object)->sprite_offset_y);
				OBJECT(current_object)->falling_grate.draw_depth = depth;
				queue_sprite_at_depth(&ignored, &depth);
				return;
			}
			if ((int16_t)(OBJECT(current_player)->x_int - 0x60u) <= OBJECT(current_object)->x_int)
			{
				animation_timer = OBJECT(current_object)->animation_timer;
				OBJECT(current_object)->animation_timer = animation_timer - 1u;
				if ((int8_t)animation_timer <= 1)
				{
					OBJECT(current_object)->animation_timer = OBJECT(current_object)->animation_speed;
					OBJECT(current_object)->floor_z_int += OBJECT(current_object)->falling_grate.rattle_step;
					OBJECT(current_object)->z_int += OBJECT(current_object)->falling_grate.rattle_step;
					OBJECT(current_object)->falling_grate.rattle_step = 0u - OBJECT(current_object)->falling_grate.rattle_step;
				}
				add_to_platform_list();
				if ((uint16_t)(OBJECT(current_object)->screen_x + 0xA0u) >= 0x281u)
					return;
				depth = (uint16_t)(OBJECT(current_object)->screen_y + OBJECT(current_object)->falling_grate.depth_offset - OBJECT(current_object)->sprite_offset_y);
				OBJECT(current_object)->falling_grate.draw_depth = depth;
				queue_sprite_at_depth(&ignored, &depth);
				return;
			}
			break;
		default:
			unknown_code(0x8B256u + OBJECT(current_object)->direction * 2u);
		}
		add_to_platform_list();
		if ((uint16_t)(OBJECT(current_object)->screen_x + 0xA0u) >= 0x281u)
			return;
		depth = (uint16_t)(OBJECT(current_object)->screen_y + OBJECT(current_object)->falling_grate.depth_offset - OBJECT(current_object)->sprite_offset_y);
		OBJECT(current_object)->falling_grate.draw_depth = depth;
		queue_sprite_at_depth(&ignored, &depth);
		return;
	case 2:
		OBJECT(current_object)->speed_z += 0x2000u;
		OBJECT(current_object)->z += OBJECT(current_object)->speed_z;
		project_object(&ignored, &r1);
		if (OBJECT(current_object)->animation_timer >= 0)
		{
			animate(&r1, &r7, &at_last_step);
			if ((int8_t)at_last_step < 0)
				OBJECT(current_object)->animation_timer = -1;
		}
		if (OBJECT(current_object)->screen_y > 0x1C0)
		{
			delete_current_object();
			return;
		}
		if ((int8_t)OBJECT(current_object)->animation_frame <= 2)
			add_to_platform_list();
		depth = (uint16_t)(((int32_t)((uint32_t)OBJECT(current_object)->projected_y + OBJECT(current_object)->floor_z) >> 16) - view_y + OBJECT(current_object)->falling_grate.depth_offset);
		queue_sprite_at_depth(&ignored, &depth);
		return;
	default:
		unknown_code(0x8B224u + OBJECT(current_object)->falling_grate.state * 3u);
	}
}

void rising_saw_init(void)
{
	uint32_t r0 = 0, block_offset = 0, ignored = 0, object = 0, object_2 = 0, path = 0, y = 0;
	uint8_t blades_left = 0;

	OBJECT(current_object)->handler = rising_saw_init_ADDRESS;
	if (scroll_position < 0x3780u && biting_head_done == 0u)
		return;
	path = rising_saw_paths[OBJECT(current_object)->sprite_frame] + rising_saw_paths_ADDRESS;
	OBJECT(current_object)->x_int = ROM16(path);
	OBJECT(current_object)->x_fraction = 0u;
	path += 2u;
	OBJECT(current_object)->y_int = ROM16(path);
	OBJECT(current_object)->y_fraction = 0u;
	path += 2u;
	OBJECT(current_object)->z_int = ROM16(path);
	OBJECT(current_object)->z_fraction = 0u;
	OBJECT(current_object)->rising_saw.path = path + 2u;
	rising_saws[OBJECT(current_object)->sprite_frame] = current_object;
	find_floor(&r0, &y, &block_offset, &ignored, &object_2);
	OBJECT(current_object)->previous_floor_z_int = OBJECT(current_object)->z_int;
	project_object(&ignored, &ignored);
	OBJECT(current_object)->rising_saw.projected_x_whole = OBJECT(current_object)->projected_x;
	OBJECT(current_object)->spawn_arg_2 = 0u;
	OBJECT(current_object)->flags = 2u;
	OBJECT(current_object)->hitbox_offset_x = -0x10;
	OBJECT(current_object)->hitbox_offset_y = -0x20;
	OBJECT(current_object)->hitbox_size_x = 0x50u;
	OBJECT(current_object)->hitbox_size_y = 0x40u;
	OBJECT(current_object)->hitbox_height = 0x20u;
	OBJECT(current_object)->kind = 0x26u;
	OBJECT(current_object)->direction = 0x200u;
	OBJECT(current_object)->rising_saw.holding_back = 0u;
	OBJECT(current_object)->rising_saw.wait_timer = 0;
	if (free_object_stack < 4)
		return;
	saw_blade_offset_x = 8u;
	saw_blades_left = 1;
	do
	{
		allocate_object(&ignored, &object_2);
		OBJECT(object_2)->sprite_flags = 0x1002u;
		OBJECT(object_2)->sprite_frame = 0x7F98u;
		OBJECT(object_2)->sprite_palette = 0x1190u;
		OBJECT(object_2)->sprite_offset_x = saw_blade_offset_x;
		saw_blade_offset_x += 0x10u;
		OBJECT(object_2)->sprite_offset_y = 0;
		OBJECT(object_2)->flags = 0u;
		AS16(OBJECT(object_2)->var_80) = OBJECT(current_object)->z_int;
		AS16(OBJECT(object_2)->var_80) += 0x1Du;
		AT8(OBJECT(object_2)->var_82, 1) = 0u;
		OBJECT(object_2)->var_84 = 0u;
		OBJECT(object_2)->handler = saw_blade_update_ADDRESS;
		OBJECT(object_2)->animation = saw_blade_animation_ADDRESS;
		OBJECT(object_2)->animation_frame = 0u;
		OBJECT(object_2)->animation_speed = 1u;
		OBJECT(object_2)->animation_timer = 0;
		OBJECT(object_2)->handler = saw_blade_update_ADDRESS;
		OBJECT(object_2)->parent = current_object;
		object = object_2;
		allocate_object(&ignored, &object_2);
		OBJECT(object_2)->sprite_flags = 0x100Au;
		OBJECT(object_2)->sprite_frame = 0x8378u;
		OBJECT(object_2)->sprite_palette = 0x1200u;
		OBJECT(object_2)->sprite_offset_x = -4;
		OBJECT(object_2)->sprite_offset_y = -0x30;
		OBJECT(object_2)->flags = 0u;
		OBJECT(object_2)->handler = saw_sparks_update_ADDRESS;
		OBJECT(object_2)->animation = saw_sparks_animation_ADDRESS;
		OBJECT(object_2)->animation_frame = 0u;
		OBJECT(object_2)->animation_speed = 2u;
		OBJECT(object_2)->animation_timer = 0;
		OBJECT(object_2)->handler = saw_sparks_update_ADDRESS;
		OBJECT(object_2)->parent = object;
		OBJECT(object)->child = object_2;
		blades_left = saw_blades_left;
		saw_blades_left = blades_left - 1u;
	} while ((int8_t)blades_left > 1);
	OBJECT(current_object)->handler = rising_saw_rise_ADDRESS;
}

void rising_saw_rise(void)
{
	uint16_t speed = 0;
	uint32_t attribute_record = 0, cursor = 0, point_x = 0, point_y = 0, projected_dx = 0, slope_step = 0;
	uint8_t hold_timer = 0;
	uint16_t wait_timer = 0;
	uint32_t flags = 0;

	flags = OBJECT(current_object)->flags;
	OBJECT(current_object)->flags = flags | 0x80000000u;
	if ((flags & 0x80000000u) == 0u)
	{
	rising_saw_read_script:
		if ((uint16_t)OBJECT(current_object)->rising_saw.wait_timer == 0u)
		{
			cursor = OBJECT(current_object)->rising_saw.path;
			if (mem_read16(cursor) <= scroll_position || OBJECT(current_object)->z_int <= (int16_t)mem_read16(cursor + 2u))
			{
				cursor += 4u;
				if ((int16_t)mem_read16(cursor + 2u) < 0)
				{
					delete_current_object();
					return;
				}
				OBJECT(current_object)->rising_saw.screen_threshold = mem_read16(cursor);
				cursor += 2u;
				OBJECT(current_object)->rising_saw.catch_up_factor = mem_read16(cursor);
				cursor += 2u;
				OBJECT(current_object)->rising_saw.base_speed = mem_read16(cursor);
				cursor += 2u;
				OBJECT(current_object)->rising_saw.wait_timer = mem_read16(cursor);
				OBJECT(current_object)->rising_saw.path = cursor + 2u;
			}
		}
	}
	else if (OBJECT(current_object)->rising_saw.holding_back == 0u)
	{
		if (no_player_in_play == 0u && player_mishap == 0u && player_dropping_in == 0u)
			goto rising_saw_read_script;
		OBJECT(current_object)->rising_saw.holding_back = 0xFFu;
		OBJECT(current_object)->rising_saw.saved_base_speed = OBJECT(current_object)->rising_saw.base_speed;
		OBJECT(current_object)->rising_saw.base_speed = 0;
		OBJECT(current_object)->rising_saw.hold_start_position = scroll_position;
		OBJECT(current_object)->rising_saw.hold_timer = 0xFFu;
		OBJECT(current_object)->rising_saw.saved_threshold = OBJECT(current_object)->rising_saw.screen_threshold;
		OBJECT(current_object)->rising_saw.screen_threshold = 0x130;
	}
	if (OBJECT(current_object)->rising_saw.holding_back != 0u)
	{
		if ((int16_t)(scroll_position - OBJECT(current_object)->rising_saw.hold_start_position) >= 0x300)
		{
		rising_saw_stop_holding_back:
			OBJECT(current_object)->rising_saw.holding_back = 0u;
			OBJECT(current_object)->rising_saw.screen_threshold = OBJECT(current_object)->rising_saw.saved_threshold;
			OBJECT(current_object)->rising_saw.base_speed = OBJECT(current_object)->rising_saw.saved_base_speed;
		}
		else
		{
			if (no_player_in_play != 0u)
				OBJECT(current_object)->rising_saw.hold_timer = 0xFFu;
			hold_timer = OBJECT(current_object)->rising_saw.hold_timer;
			OBJECT(current_object)->rising_saw.hold_timer = hold_timer - 1u;
			if (hold_timer == 0u)
				goto rising_saw_stop_holding_back;
		}
	}
	update_screen_position(&slope_step, &projected_dx);
	if (OBJECT(current_object)->screen_y <= OBJECT(current_object)->rising_saw.screen_threshold)
	{
	rising_saw_use_base_speed:
		speed = (uint16_t)OBJECT(current_object)->rising_saw.base_speed;
	}
	else
	{
		OBJECT(current_object)->rising_saw.wait_timer = 0;
		compute_camera_ground_speed(0u);
		speed = (uint16_t)((uint32_t)camera_ground_speed_int * OBJECT(current_object)->rising_saw.catch_up_factor) >> 8;
		if ((int16_t)speed <= OBJECT(current_object)->rising_saw.base_speed)
			goto rising_saw_use_base_speed;
	}
	OBJECT(current_object)->speed_fraction = speed;
	wait_timer = OBJECT(current_object)->rising_saw.wait_timer;
	OBJECT(current_object)->rising_saw.wait_timer = wait_timer - 1u;
	if ((int16_t)wait_timer > 1)
	{
		add_to_touchable_list(slope_step);
		return;
	}
	OBJECT(current_object)->rising_saw.wait_timer = 0;
	OBJECT(current_object)->z -= (uint32_t)(int16_t)OBJECT(current_object)->speed_fraction << 12;
	attribute_record = OBJECT(current_object)->floor_attribute + floor_types_ADDRESS;
	slope_step = ((uint32_t)(int16_t)ROM16(attribute_record + 4u) * (int16_t)OBJECT(current_object)->speed_fraction) >> 4;
	projected_dx = slope_step;
	if (ROM8(attribute_record) != 0u)
		projected_dx = 0u - projected_dx;
	OBJECT(current_object)->rising_saw.fixed_projected_x += projected_dx;
	OBJECT(current_object)->projected_x = OBJECT(current_object)->rising_saw.projected_x_whole;
	OBJECT(current_object)->projected_y -= slope_step >> 1;
	point_x = OBJECT(current_object)->rising_saw.fixed_projected_x;
	point_y = OBJECT(current_object)->projected_y * 2u;
	unproject_point(&point_x, &point_y);
	OBJECT(current_object)->x = point_x;
	OBJECT(current_object)->y = point_y;
	add_to_touchable_list(slope_step);
}

void saw_blade_update(void)
{
	uint8_t sound_id = 0;
	uint16_t child = 0, object_4 = 0;
	uint32_t r1 = 0, r2 = 0, r7 = 0, r8 = 0, clip_x = 0, clips_left = 0, depth = 0, free_count = 0, ignored = 0, object = 0, object_2 = 0, object_3 = 0, object_5 = 0, point_y = 0, z = 0;
	uint16_t sound_timer = 0;

	switch (OBJECT(current_object)->saw_blade.state)
	{
	case 0:
	case 4:
		OBJECT(current_object)->saw_blade.state++;
		object = LINKED(current_object, OBJECT(current_object)->parent);
		OBJECT(current_object)->x = OBJECT(object)->x;
		OBJECT(current_object)->y = OBJECT(object)->y;
		OBJECT(current_object)->y_int -= 0x30u;
		OBJECT(current_object)->z = OBJECT(object)->z;
		clips_left = 2u;
		object_2 = current_object;
		do
		{
			object_3 = object_2;
			allocate_object_2(&ignored, &r2, &object_2);
			object_5 = object_2 + 2u;
			OBJECT(object_5 - 0x2u)->sprite_flags = 0x5000u;
			object_5 += 2u;
			OBJECT(object_5 - 0x4u)->sprite_frame = 0xE0u;
			object_5 += 2u;
			OBJECT(object_5 - 0x6u)->sprite_palette = 0u;
			object_5 += 2u;
			OBJECT(object_5 - 0x8u)->sprite_scale = 0x140u;
			object_5 += 2u;
			OBJECT(object_5 - 0xAu)->sprite_layer = 0u;
			object_5 += 2u;
			OBJECT(object_5 - 0xCu)->screen_y = 0;
			object_5 += 2u;
			OBJECT(object_5 - 0xEu)->screen_x = 0;
			OBJECT(object_5 - 0xEu)->hitbox_size_x = 0u;
			OBJECT(object_2)->parent = object_3;
			OBJECT(object_3)->child = object_2;
			clips_left--;
		} while (clips_left != 0u);
		sound_id = 0xAu;
		play_sound(sound_id, &r1, &r2);
		return;
	case 1:
		OBJECT(current_object)->y += 0x20000u;
		object = LINKED(current_object, OBJECT(current_object)->parent);
		if (OBJECT(object)->y_int <= OBJECT(current_object)->y_int)
		{
			OBJECT(current_object)->saw_blade.state++;
			OBJECT(current_object)->y = OBJECT(object)->y;
		}
		animate(&r1, &r7, &r8);
		project_object(&ignored, &ignored);
		OBJECT(current_object)->saw_blade.draw_depth = OBJECT(current_object)->screen_y;
		depth = OBJECT(current_object)->saw_blade.draw_depth;
		current_object = LINKED(current_object, OBJECT(current_object)->child);
		current_object = LINKED(current_object, OBJECT(current_object)->child);
		queue_sprite_at_depth(&ignored, &depth);
		current_object = object_cursor;
		depth = OBJECT(current_object)->saw_blade.draw_depth;
		queue_sprite_at_depth(&ignored, &depth);
		clip_x = OBJECT(current_object)->x;
		point_y = OBJECT(LINKED(current_object, OBJECT(current_object)->parent))->y;
		z = OBJECT(current_object)->z;
		project_point(z, &ignored, &clip_x, &point_y);
		clip_x = (uint16_t)(clip_x + OBJECT(current_object)->sprite_offset_x);
		current_object = LINKED(current_object, OBJECT(current_object)->child);
		OBJECT(current_object)->sprite_palette = clip_x;
		depth = OBJECT(object_cursor)->saw_blade.draw_depth;
		queue_sprite_at_depth(&ignored, &depth);
		return;
	case 2:
		OBJECT(current_object)->saw_blade.state++;
		current_object = LINKED(current_object, OBJECT(current_object)->child);
		child = OBJECT(current_object)->child;
		free_object_2(child);
		object_4 = (uint16_t)current_object;
		free_object_2(object_4);
		current_object = object_cursor;
	case 3:
		object_3 = LINKED(current_object, OBJECT(current_object)->parent);
		OBJECT(current_object)->x = OBJECT(object_3)->x;
		OBJECT(current_object)->y = OBJECT(object_3)->y;
		OBJECT(current_object)->z = OBJECT(object_3)->z;
		project_object(&ignored, &r1);
		if ((int16_t)(OBJECT(current_object)->saw_blade.next_shaft_z - OBJECT(current_object)->z_int) >= 0x1D)
		{
			OBJECT(current_object)->saw_blade.next_shaft_z -= 0x1Du;
			allocate_object(&free_count, &object_2);
			if (free_count != 0u)
			{
				OBJECT(object_2)->sprite_flags = 0x1004u;
				OBJECT(object_2)->sprite_frame = 0x7FC0u;
				OBJECT(object_2)->sprite_palette = 0x11A0u;
				OBJECT(object_2)->sprite_offset_x = 8;
				OBJECT(object_2)->sprite_offset_y = -0x1C;
				OBJECT(object_2)->flags = 0u;
				OBJECT(object_2)->saw_shaft_update.state = 0u;
				OBJECT(object_2)->animation = saw_shaft_animation_ADDRESS;
				OBJECT(object_2)->animation_frame = 1u;
				OBJECT(object_2)->projected_x = OBJECT(current_object)->projected_x;
				OBJECT(object_2)->projected_y = OBJECT(current_object)->projected_y;
				OBJECT(object_2)->z = OBJECT(current_object)->z;
				OBJECT(object_2)->saw_shaft_update.next_step_z = OBJECT(object_2)->z_int;
				OBJECT(object_2)->handler = saw_shaft_update_ADDRESS;
				OBJECT(object_2)->parent = current_object;
			}
		}
		animate(&r1, &r7, &r8);
		sound_timer = OBJECT(current_object)->saw_blade.sound_timer;
		OBJECT(current_object)->saw_blade.sound_timer = sound_timer - 1u;
		if ((int16_t)sound_timer <= 1)
		{
			OBJECT(current_object)->saw_blade.sound_timer = 0x3C;
			sound_id = 8u;
			play_sound(sound_id, &r1, &r2);
		}
		if (OBJECT(current_object)->screen_y >= 0x140)
			return;
		OBJECT(current_object)->saw_blade.draw_depth = OBJECT(current_object)->screen_y;
		r1 = queue_sprite(r1);
		return;
	default:
		unknown_code(0x8B6FBu + OBJECT(current_object)->saw_blade.state * 3u);
	}
}

void saw_sparks_update(void)
{
	uint32_t r7 = 0, r8 = 0, depth = 0, frame = 0, ignored = 0, object = 0;

	object = LINKED(current_object, OBJECT(current_object)->parent);
	OBJECT(current_object)->screen_x = OBJECT(object)->screen_x;
	OBJECT(current_object)->screen_y = OBJECT(object)->screen_y;
	OBJECT(current_object)->screen_x += OBJECT(current_object)->sprite_offset_x;
	OBJECT(current_object)->screen_y += OBJECT(current_object)->sprite_offset_y;
	animate(&frame, &r7, &r8);
	if (OBJECT(current_object)->screen_y >= 0x168)
		return;
	object = LINKED(current_object, OBJECT(current_object)->parent);
	if (OBJECT(LINKED(object, OBJECT(object)->parent))->var_88 == 0u)
		return;
	depth = (uint16_t)(OBJECT(object)->var_88 + 1u);
	queue_sprite_at_depth(&ignored, &depth);
}

void saw_shaft_update(void)
{
	uint32_t r1 = 0, at_last_step = 0, depth = 0, ignored = 0, off_screen = 0, wrapped = 0;

	switch (OBJECT(current_object)->saw_shaft_update.state)
	{
	case 0:
	case 2:
		if ((int16_t)(OBJECT(current_object)->saw_shaft_update.next_step_z - OBJECT(LINKED(current_object, OBJECT(current_object)->parent))->z_int) >= 5)
		{
			OBJECT(current_object)->saw_shaft_update.next_step_z -= 5u;
			wrapped = 0u;
			animate_step(&r1, &wrapped, &at_last_step);
			if ((uint8_t)wrapped != 0u)
				OBJECT(current_object)->saw_shaft_update.state++;
		}
		update_screen_position(&ignored, &r1);
		depth = 0xFFE9u;
		queue_sprite_at_depth(&ignored, &depth);
		return;
	case 1:
		off_screen = is_off_screen();
		if (off_screen == 0u)
		{
			update_screen_position(&ignored, &r1);
			depth = 0xFFF7u;
			queue_sprite_at_depth(&ignored, &depth);
			return;
		}
		delete_current_object();
		return;
	default:
		unknown_code(0x8B957u + OBJECT(current_object)->saw_shaft_update.state * 2u);
	}
}

void grate_steps_init(void)
{
	uint32_t r0 = 0, r1 = 0, block_offset = 0, cursor = 0, ignored = 0;

	OBJECT(current_object)->x = -0x3BD0000;
	OBJECT(current_object)->y = 0x4F40000;
	OBJECT(current_object)->z = 0x80000;
	project_object(&r0, &r1);
	OBJECT(current_object)->grate_steps.row_projected_x = OBJECT(current_object)->projected_x;
	OBJECT(current_object)->spawn_arg_2 = 0u;
	OBJECT(current_object)->grate_steps.row_projected_y = OBJECT(current_object)->projected_y;
	find_floor(&r0, &r1, &block_offset, &ignored, &cursor);
	OBJECT(current_object)->grate_steps.rows_left = 0xF;
	rising_saws[0] = 0xFFFFu;
	rising_saw_1 = 0xFFFFu;
	rising_saw_2 = 0xFFFFu;
	OBJECT(current_object)->handler = build_grate_steps_ADDRESS;
}

void build_grate_steps(void)
{
	uint32_t ignored = 0, object = 0, slope_shift = 0;
	uint8_t rows_to_build = 0;
	uint16_t rows_left = 0;

	if (free_object_stack < 6)
		return;
	grate_rows_to_build = 1;
	do
	{
		OBJECT(current_object)->projected_y = OBJECT(current_object)->build_grate_steps.row_projected_y;
		OBJECT(current_object)->projected_x = OBJECT(current_object)->build_grate_steps.row_projected_x_whole;
		grate_step_number = 0;
		do
		{
			allocate_object(&ignored, &object);
			OBJECT(object)->sprite_flags = 0x1009u;
			OBJECT(object)->sprite_frame = 0x7FF0u;
			OBJECT(object)->sprite_palette = 0x11B0u;
			OBJECT(object)->sprite_offset_x = 0;
			OBJECT(object)->sprite_offset_y = 0;
			OBJECT(object)->flags = 0u;
			OBJECT(object)->screen_y = 0;
			OBJECT(object)->kind = grate_step_number;
			OBJECT(object)->handler = grate_step_update_ADDRESS;
			OBJECT(object)->animation = grate_step_cut_animation_ADDRESS;
			OBJECT(object)->animation_frame = 0u;
			OBJECT(object)->projected_x = OBJECT(current_object)->projected_x;
			OBJECT(object)->projected_y = OBJECT(current_object)->projected_y;
			OBJECT(object)->z = OBJECT(current_object)->z;
			AS32(OBJECT(object)->grate_step_update.cut_z) = AS32(OBJECT(object)->z_int);
			OBJECT(object)->grate_step_update.cut_z += 0x1Eu;
			OBJECT(current_object)->projected_x += 0x35u;
			OBJECT(current_object)->projected_y -= 0x1A8000u;
			grate_step_number++;
		} while (grate_step_number < 3);
		slope_shift = (int16_t)ROM16(floor_attribute_slopes_ADDRESS + OBJECT(current_object)->floor_attribute) * 0x1Eu;
		OBJECT(current_object)->build_grate_steps.row_projected_x += slope_shift;
		OBJECT(current_object)->build_grate_steps.row_projected_y += slope_shift >> 1;
		OBJECT(current_object)->z_int += 0x1Eu;
		rows_to_build = grate_rows_to_build;
		grate_rows_to_build = rows_to_build - 1u;
	} while ((int8_t)rows_to_build > 1);
	rows_left = OBJECT(current_object)->build_grate_steps.rows_left;
	OBJECT(current_object)->build_grate_steps.rows_left = rows_left - 1u;
	if ((int16_t)rows_left > 1)
		return;
	delete_current_object();
}

void grate_step_update(void)
{
	uint8_t sound_id = 0;
	uint16_t cut_gap = 0;
	uint32_t r1 = 0, r2 = 0, at_last_step = 0, depth = 0, ignored = 0, object = 0, wrapped = 0;
	uint16_t cut_z = 0, saw_top_z = 0;

	if (OBJECT(current_object)->screen_y >= 0x118)
	{
		delete_current_object();
		return;
	}
	if ((int8_t)OBJECT(current_object)->animation_frame >= 0)
	{
		object = (current_object & ~0xFFFFu) | rising_saws[OBJECT(current_object)->kind];
		if ((int16_t)object >= 0)
		{
			cut_z = OBJECT(current_object)->grate_step_update.cut_z;
			saw_top_z = OBJECT(object)->z_int - 0x20u;
			cut_gap = (uint16_t)(saw_top_z - cut_z);
			if ((int16_t)saw_top_z <= (int16_t)cut_z)
			{
				if ((int16_t)cut_gap >= -0x20)
				{
					spawn_grate_step_debris();
					sound_id = 0x28u;
					play_sound(sound_id, &r1, &r2);
				}
				OBJECT(current_object)->grate_step_update.cut_z -= 7u;
				at_last_step = 0u;
				animate_step(&r1, &wrapped, &at_last_step);
				if ((uint8_t)at_last_step != 0u)
					OBJECT(current_object)->animation_frame = 0xFFu;
			}
		}
	}
	update_screen_position(&ignored, &r1);
	if (OBJECT(current_object)->screen_y < -0x38)
		return;
	depth = 0xFFF8u;
	queue_sprite_at_depth(&ignored, &depth);
}

void spawn_grate_step_debris(void)
{
	uint32_t ignored = 0, object = 0, random_value = 0, record = 0;
	uint32_t stack_4 = 0;
	uint16_t debris_left = 0;

	record = grate_step_debris_ADDRESS;
	if (free_object_stack < (int16_t)grate_step_debris)
		return;
	grate_debris_left = ROM16(record);
	grate_debris_record = record + 2u;
	do
	{
		allocate_object(&ignored, &object);
		OBJECT(object)->sprite_flags = 0x1000u;
		OBJECT(object)->sprite_palette = 0x11C0u;
		OBJECT(object)->flags = 0u;
		OBJECT(object)->sprite_offset_x = 0;
		OBJECT(object)->sprite_offset_y = 0;
		OBJECT(object)->handler = grate_step_debris_fly_ADDRESS;
		OBJECT(object)->grate_step_debris_view.projected_x_whole = OBJECT(current_object)->projected_x;
		OBJECT(object)->spawn_arg_2 = 0u;
		OBJECT(object)->projected_y = OBJECT(current_object)->projected_y;
		OBJECT(object)->z_int = OBJECT(current_object)->spawn_grate_step_debris.cut_z;
		OBJECT(object)->z_fraction = 0u;
		record = grate_debris_record;
		OBJECT(object)->grate_step_debris_view.projected_x_whole += mem_read16(record);
		record += 2u;
		OBJECT(object)->projected_y_int += mem_read16(record);
		record += 2u;
		OBJECT(object)->speed_x = (uint32_t)(int16_t)mem_read16(record) << 8;
		record += 2u;
		OBJECT(object)->speed_z = (uint32_t)(int16_t)mem_read16(record) << 8;
		stack_4 = record + 2u;
		next_random(&random_value, &ignored, &ignored);
		record = stack_4;
		OBJECT(object)->speed_x += (int16_t)random_value;
		random_value = (int32_t)random_value >> 16;
		OBJECT(object)->speed_z += random_value;
		OBJECT(object)->animation = mem_read16(record) + grate_step_debris_animations_ADDRESS;
		OBJECT(object)->animation_frame = 0xFFu;
		OBJECT(object)->animation_speed = 3u;
		OBJECT(object)->animation_timer = 0;
		grate_debris_record = record + 2u;
		debris_left = grate_debris_left;
		grate_debris_left = debris_left - 1u;
	} while ((int16_t)debris_left > 1);
}

void grate_step_debris_fly(void)
{
	uint32_t r1 = 0, r7 = 0, depth = 0, ignored = 0, off_screen = 0;

	OBJECT(current_object)->speed_z += 0x6000u;
	OBJECT(current_object)->z += OBJECT(current_object)->speed_z;
	OBJECT(current_object)->grate_step_debris_view.fixed_projected_x += OBJECT(current_object)->speed_x;
	OBJECT(current_object)->projected_x = OBJECT(current_object)->grate_step_debris_view.projected_x_whole;
	update_screen_position(&ignored, &r1);
	off_screen = is_off_screen();
	if (off_screen == 0u)
	{
		animate(&r1, &r7, &off_screen);
		depth = 0xE0u;
		queue_sprite_at_depth(&ignored, &depth);
		return;
	}
	delete_current_object();
}

void build_saw_shaft_base(void)
{
	uint16_t piece_frame = 0;
	uint32_t r0 = 0, block_offset = 0, ignored = 0, object = 0, slope_shift = 0, y = 0;

	OBJECT(current_object)->handler = build_saw_shaft_base_ADDRESS;
	if (scroll_position < 0x3780u && biting_head_done == 0u)
		return;
	OBJECT(current_object)->flags = 0u;
	find_floor(&r0, &y, &block_offset, &ignored, &object);
	OBJECT(current_object)->z = OBJECT(current_object)->floor_z;
	OBJECT(current_object)->z_int -= 0x3Au;
	project_object(&ignored, &ignored);
	OBJECT(current_object)->build_saw_shaft_base.projected_x_whole = OBJECT(current_object)->projected_x;
	OBJECT(current_object)->spawn_arg_2 = 0u;
	saw_shaft_part = 0;
	do
	{
		allocate_object(&ignored, &object);
		OBJECT(object)->sprite_flags = 0x1004u;
		OBJECT(object)->sprite_palette = 0x11A0u;
		OBJECT(object)->sprite_offset_x = 0;
		OBJECT(object)->sprite_offset_y = 0;
		OBJECT(object)->flags = 0u;
		OBJECT(object)->saw_shaft_base_draw.state = 1u;
		OBJECT(object)->handler = saw_shaft_base_draw_ADDRESS;
		piece_frame = 0x7FB8u;
		if ((uint16_t)saw_shaft_part == 0u)
		{
		build_saw_shaft_base_end_piece:
			piece_frame = 0x7FE8u;
		}
		else if ((uint16_t)saw_shaft_part == 2u)
		{
			OBJECT(object)->sprite_flags |= 0x80u;
			goto build_saw_shaft_base_end_piece;
		}
		OBJECT(object)->sprite_frame = piece_frame;
		OBJECT(object)->projected_x = OBJECT(current_object)->build_saw_shaft_base.projected_x_whole;
		OBJECT(object)->projected_y = OBJECT(current_object)->projected_y;
		OBJECT(object)->z = OBJECT(current_object)->z;
		OBJECT(current_object)->z_int += 0x1Du;
		slope_shift = (int16_t)ROM16(floor_attribute_slopes_ADDRESS + OBJECT(current_object)->floor_attribute) * 0x1Du;
		OBJECT(current_object)->build_saw_shaft_base.fixed_projected_x += slope_shift;
		OBJECT(current_object)->projected_y += slope_shift >> 1;
		saw_shaft_part++;
	} while (saw_shaft_part < 3);
	delete_current_object();
}

void saw_shaft_base_draw(void)
{
	uint32_t r1 = 0, depth = 0, ignored = 0, off_screen = 0;

	update_screen_position(&ignored, &r1);
	off_screen = check_left_screen();
	if (off_screen == 0u)
	{
		depth = 0xFFF7u;
		queue_sprite_at_depth(&ignored, &depth);
		return;
	}
	delete_current_object();
}

void river_raft_spawner_init(void)
{
	uint32_t record = 0;

	record = river_raft_spawners[OBJECT(current_object)->sprite_frame] + river_raft_spawners_ADDRESS;
	OBJECT(current_object)->x_int = ROM16(record);
	OBJECT(current_object)->x_fraction = 0u;
	record += 2u;
	OBJECT(current_object)->y_int = ROM16(record);
	OBJECT(current_object)->y_fraction = 0u;
	record += 2u;
	OBJECT(current_object)->floor_z_int = ROM16(record);
	OBJECT(current_object)->floor_z_fraction = 0u;
	record += 2u;
	OBJECT(current_object)->river_raft_spawner.end_position = ROM16(record);
	record += 2u;
	OBJECT(current_object)->river_raft_spawner.entry = record;
	OBJECT(current_object)->river_raft_spawner.list_start = record;
	OBJECT(current_object)->river_raft_spawner.last_raft_y = OBJECT(current_object)->y;
	OBJECT(current_object)->river_raft_spawner.last_raft_speed = 0u;
	OBJECT(current_object)->z = OBJECT(current_object)->floor_z;
	OBJECT(current_object)->handler = river_raft_spawner_update_ADDRESS;
}

void river_raft_spawner_update(void)
{
	uint32_t floor_block_record = 0, ignored = 0, object = 0, object_2 = 0, raft_entry = 0;

	if (OBJECT(current_object)->river_raft_spawner.end_position <= scroll_position)
	{
		delete_current_object();
		return;
	}
	if (scroll_position >= 0x9C00u)
		goal_unlocked = 0xFFu;
	OBJECT(current_object)->river_raft_spawner.last_raft_y += OBJECT(current_object)->river_raft_spawner.last_raft_speed >> 8;
	if (OBJECT(current_object)->y_int <= OBJECT(current_object)->river_raft_spawner.last_raft_y_int && OBJECT(current_object)->y_int <= OBJECT(LINKED(current_object, group_leaders[0]))->y_int)
	{
		while ((int16_t)mem_read16(OBJECT(current_object)->river_raft_spawner.entry) >= 0)
		{
			if (free_object_stack < 2)
			{
				project_object(&ignored, &ignored);
				if (OBJECT(current_object)->screen_x >= -0x10)
				{
					compute_camera_ground_speed(0u);
					OBJECT(current_object)->y -= camera_ground_speed >> 7;
				}
				return;
			}
			allocate_object(&ignored, &object);
			OBJECT(object)->sprite_flags = 0x100Au;
			OBJECT(object)->sprite_frame = 0x8DF0u;
			OBJECT(object)->sprite_palette = 0xB1A0u;
			OBJECT(object)->hitbox_size_x = 0x30u;
			OBJECT(object)->hitbox_size_y = 0x88u;
			OBJECT(object)->direction = 0u;
			AS8(OBJECT(object)->var_84) = 0u;
			OBJECT(object)->sprite_offset_x = -8;
			OBJECT(object)->sprite_offset_y = -0x10;
			OBJECT(object)->flags = 0u;
			OBJECT(object)->handler = river_raft_update_ADDRESS;
			raft_entry = OBJECT(current_object)->river_raft_spawner.entry;
			OBJECT(object)->y = OBJECT(current_object)->y;
			OBJECT(object)->y -= (uint32_t)mem_read16(raft_entry) << 16;
			raft_entry += 2u;
			OBJECT(object)->speed_int = mem_read16(raft_entry);
			OBJECT(object)->speed_fraction = 0u;
			raft_entry += 2u;
			OBJECT(object)->floor_block = mem_read32(raft_entry);
			OBJECT(current_object)->river_raft_spawner.entry = raft_entry + 4u;
			floor_block_record = floor_blocks + 2u + OBJECT(object)->floor_block;
			OBJECT(object)->floor_z_int = mem_read16(floor_block_record + 0x10u);
			OBJECT(object)->floor_z_fraction = 0u;
			OBJECT(object)->z = OBJECT(object)->floor_z;
			OBJECT(object)->x = mem_read32(floor_block_record);
			OBJECT(object)->x -= 0x10000u;
			OBJECT(current_object)->river_raft_spawner.last_raft_y = OBJECT(object)->y;
			OBJECT(current_object)->river_raft_spawner.last_raft_speed = OBJECT(object)->speed;
			object_2 = object;
			allocate_object(&ignored, &object);
			OBJECT(object)->sprite_flags = OBJECT(object_2)->sprite_flags;
			OBJECT(object)->sprite_offset_x = OBJECT(object_2)->sprite_offset_x;
			OBJECT(object)->sprite_offset_y = OBJECT(object_2)->sprite_offset_y;
			OBJECT(object)->flags = 0u;
			AS8(OBJECT(object)->var_84) = 0u;
			OBJECT(object)->sprite_palette = 0xB190u;
			OBJECT(object)->handler = river_raft_foam_update_ADDRESS;
			OBJECT(object)->animation = river_raft_foam_animation_ADDRESS;
			OBJECT(object)->animation_frame = 0xFFu;
			OBJECT(object)->animation_timer = 0;
			OBJECT(object)->animation_speed = 4u;
			OBJECT(object)->parent = object_2;
			OBJECT(object_2)->child = object;
		}
		if (mem_read16(OBJECT(current_object)->river_raft_spawner.entry) == 0x8000u)
			OBJECT(current_object)->river_raft_spawner.entry = OBJECT(current_object)->river_raft_spawner.list_start;
	}
	project_object(&ignored, &ignored);
	if (OBJECT(current_object)->screen_x >= -0x10)
	{
		compute_camera_ground_speed(0u);
		OBJECT(current_object)->y -= camera_ground_speed >> 7;
	}
}

void river_raft_update(void)
{
	uint16_t draw_depth = 0;
	uint32_t r7 = 0, r8 = 0, depth = 0, frame = 0, ignored = 0, object = 0, off_screen = 0;

	switch (OBJECT(current_object)->river_raft_update.state)
	{
	case 0:
	case 4:
		OBJECT(current_object)->river_raft_update.state++;
		OBJECT(current_object)->speed_y = (uint32_t)OBJECT(current_object)->speed >> 8;
		OBJECT(current_object)->speed_x = 0;
		project_object(&ignored, &ignored);
		return;
	case 1:
		off_screen = check_left_screen();
		if (off_screen != 0u)
		{
			current_object = LINKED(current_object, OBJECT(current_object)->child);
			OBJECT(current_object)->flags |= 0x400u;
			delete_current_object();
			return;
		}
		OBJECT(current_object)->y += OBJECT(current_object)->speed_y;
		OBJECT(current_object)->z_int = OBJECT(current_object)->floor_z_int;
		if ((OBJECT(current_object)->flags & 8u) != 0u)
			OBJECT(current_object)->z_int += 2u;
		project_object(&ignored, &ignored);
		add_to_platform_list();
		if (OBJECT(LINKED(current_object, group_leaders[0]))->y_int <= (int16_t)(OBJECT(current_object)->y_int + OBJECT(current_object)->hitbox_size_y))
			OBJECT(current_object)->river_raft_update.state++;
		draw_depth = (uint16_t)(OBJECT(current_object)->screen_y - OBJECT(current_object)->sprite_offset_y - 0x10u);
		if ((int16_t)draw_depth <= -0x10)
			draw_depth = 0xFFF1u;
		OBJECT(current_object)->river_raft_update.draw_depth = draw_depth;
		current_object = LINKED(current_object, OBJECT(current_object)->child);
		depth = OBJECT(object_cursor)->river_raft_update.draw_depth;
		queue_sprite_at_depth(&ignored, &depth);
		current_object = object_cursor;
		depth = OBJECT(current_object)->river_raft_update.draw_depth;
		queue_sprite_at_depth(&ignored, &depth);
		return;
	case 2:
		off_screen = check_left_screen();
		if (off_screen != 0u)
		{
			current_object = LINKED(current_object, OBJECT(current_object)->child);
			OBJECT(current_object)->flags |= 0x400u;
			delete_current_object();
			return;
		}
		OBJECT(current_object)->y += OBJECT(current_object)->speed_y;
		OBJECT(current_object)->z_int = OBJECT(current_object)->floor_z_int;
		if ((OBJECT(current_object)->flags & 8u) != 0u)
			OBJECT(current_object)->z_int += 2u;
		add_to_platform_list();
		if (OBJECT(LINKED(current_object, group_leaders[0]))->y_int <= (int16_t)(OBJECT(current_object)->y_int + (OBJECT(current_object)->hitbox_size_y >> 1)))
		{
			OBJECT(current_object)->river_raft_update.state++;
			OBJECT(current_object)->animation = river_raft_fall_animation_ADDRESS;
			OBJECT(current_object)->animation_frame = 0xFFu;
			OBJECT(current_object)->animation_speed = 4u;
			OBJECT(current_object)->animation_timer = 0;
			OBJECT(current_object)->sprite_frame = mem_read16(OBJECT(current_object)->animation + 2u);
			OBJECT(current_object)->sprite_flags = 0x1000u;
			OBJECT(current_object)->sprite_offset_x = 0;
			OBJECT(current_object)->sprite_offset_y = 0;
			OBJECT(current_object)->x_int += 0x16u;
			OBJECT(current_object)->y_int += 0x44u;
			OBJECT(current_object)->speed_z = 0;
			object = LINKED(current_object, OBJECT(current_object)->child);
			OBJECT(object)->flags |= 0x400u;
		}
		project_object(&ignored, &ignored);
		OBJECT(current_object)->river_raft_update.draw_depth = OBJECT(current_object)->screen_y - OBJECT(current_object)->sprite_offset_y - 0x10u;
		current_object = LINKED(current_object, OBJECT(current_object)->child);
		if ((OBJECT(current_object)->flags & 0x400u) == 0u)
		{
			depth = OBJECT(object_cursor)->river_raft_update.draw_depth;
			queue_sprite_at_depth(&ignored, &depth);
		}
		current_object = object_cursor;
		depth = OBJECT(current_object)->river_raft_update.draw_depth;
		queue_sprite_at_depth(&ignored, &depth);
		return;
	case 3:
		if (OBJECT(current_object)->screen_y >= 0xE8)
		{
			delete_current_object();
			return;
		}
		r8 = check_left_screen();
		if (r8 != 0u)
		{
			delete_current_object();
			return;
		}
		OBJECT(current_object)->y += OBJECT(current_object)->speed_y;
		OBJECT(current_object)->speed_z += 0x2000u;
		OBJECT(current_object)->z += OBJECT(current_object)->speed_z;
		if ((int8_t)OBJECT(current_object)->animation_speed >= 0)
		{
			animate(&frame, &r7, &r8);
			if ((uint8_t)r8 != 0u)
				OBJECT(current_object)->animation_speed = 0xFFu;
		}
		OBJECT(current_object)->sprite_palette = river_raft_fall_palettes[OBJECT(current_object)->animation_frame];
		project_object(&ignored, &ignored);
		depth = (uint16_t)(OBJECT(current_object)->screen_y - OBJECT(current_object)->sprite_offset_y - 0x10u);
		queue_sprite_at_depth(&ignored, &depth);
		return;
	default:
		unknown_code(0x8C340u + OBJECT(current_object)->river_raft_update.state * 3u);
	}
}

void river_raft_foam_update(void)
{
	uint32_t r7 = 0, r8 = 0, frame = 0, object = 0;

	if ((OBJECT(current_object)->flags & 0x400u) == 0u)
	{
		object = LINKED(current_object, OBJECT(current_object)->parent);
		OBJECT(current_object)->screen_x = OBJECT(object)->screen_x;
		OBJECT(current_object)->screen_y = OBJECT(object)->screen_y;
		animate(&frame, &r7, &r8);
		return;
	}
	delete_current_object();
}

void geyser_front_init(void)
{
	uint32_t block_offset = 0, free_count = 0, hitbox_object = 0, ignored = 0, path = 0, y = 0;

	path = geyser_fronts[OBJECT(current_object)->sprite_frame] + geyser_fronts_ADDRESS;
	OBJECT(current_object)->z_int = ROM16(path);
	OBJECT(current_object)->z_fraction = 0u;
	path += 2u;
	OBJECT(current_object)->geyser_front.top_height = ROM16(path);
	path += 2u;
	OBJECT(current_object)->geyser_front.next_listed_geyser = ROM16(path) + geyser_lists_ADDRESS;
	OBJECT(current_object)->geyser_front.path = path + 2u;
	find_floor(&free_count, &y, &block_offset, &ignored, &hitbox_object);
	OBJECT(current_object)->previous_floor_z_int = OBJECT(current_object)->z_int;
	OBJECT(current_object)->previous_floor_z_int += 0x30u;
	project_object(&ignored, &ignored);
	OBJECT(current_object)->geyser_front.projected_x_whole = OBJECT(current_object)->projected_x;
	AS16(OBJECT(current_object)->geyser_front.fixed_projected_x) = 0u;
	OBJECT(current_object)->geyser_front.row_pattern = geyser_row_patterns[OBJECT(current_object)->sprite_frame] + geyser_row_patterns_ADDRESS;
	OBJECT(current_object)->geyser_front.holding_back = 0u;
	OBJECT(current_object)->geyser_front.wait_timer = 0;
	geyser_front_start = 0u;
	OBJECT(current_object)->geyser_front.sound_timer = 0;
	take_free_object(&free_count, &hitbox_object);
	if (free_count != 0u)
	{
		mem_write16(hitbox_object + 0x66u, 0x40u);
		mem_write16(hitbox_object + 0x10u, 0x30u);
		mem_write16(hitbox_object + 0x12u, 0xC0u);
		mem_write32(hitbox_object + 0x30u, 2u);
		mem_write16(hitbox_object + 0x7Cu, 0u);
		mem_write16(hitbox_object + 0x24u, 0x300u);
		mem_write8(hitbox_object + 0x47u, 0x29u);
		OBJECT(current_object)->child = hitbox_object;
		OBJECT(current_object)->handler = geyser_front_wait_ADDRESS;
	}
}

void geyser_front_wait(void)
{
	uint16_t start_position = 0;

	spawn_listed_geyser(0u);
	if (geyser_front_start != 0u)
		OBJECT(current_object)->handler = geyser_front_rise_ADDRESS;
	else
	{
		start_position = 0x6500u;
		if (OBJECT(current_object)->sprite_frame != 0u)
			start_position = 0x8400u;
		if (start_position <= scroll_position)
		{
			OBJECT(current_object)->handler = geyser_front_rise_ADDRESS;
			return;
		}
	}
}

void geyser_front_rise(void)
{
	uint8_t sound_id = 0;
	uint16_t wait = 0;
	uint32_t r1 = 0, r2 = 0, r11 = 0, attribute_record = 0, cursor = 0, ignored = 0, point_x = 0, point_y = 0, row_delays = 0, row_pattern = 0, slope_step = 0, x = 0, y = 0;
	uint8_t hold_timer = 0;
	uint16_t wait_timer = 0, geysers_remaining = 0, sound_timer = 0;
	uint32_t flags = 0;

	flags = OBJECT(current_object)->flags;
	OBJECT(current_object)->flags = flags | 0x200000u;
	if ((flags & 0x200000u) == 0u)
	{
	geyser_front_read_script:
		if ((uint16_t)OBJECT(current_object)->geyser_front.wait_timer == 0u)
		{
			cursor = OBJECT(current_object)->geyser_front.path;
			if (mem_read16(cursor) <= scroll_position || OBJECT(current_object)->z_int <= (int16_t)mem_read16(cursor + 2u))
			{
				cursor += 4u;
				if ((int16_t)mem_read16(cursor + 2u) < 0)
				{
					current_object = LINKED(current_object, OBJECT(current_object)->child);
					push_free_object();
					delete_current_object();
					return;
				}
				OBJECT(current_object)->geyser_front.screen_threshold = mem_read16(cursor);
				cursor += 2u;
				OBJECT(current_object)->geyser_front.catch_up_factor = mem_read16(cursor);
				cursor += 2u;
				OBJECT(current_object)->geyser_front.base_speed = mem_read16(cursor);
				cursor += 2u;
				OBJECT(current_object)->geyser_front.wait_timer = mem_read16(cursor);
				OBJECT(current_object)->geyser_front.path = cursor + 2u;
			}
		}
	}
	else if (OBJECT(current_object)->geyser_front.holding_back == 0u)
	{
		if (no_player_in_play == 0u && player_mishap == 0u && player_dropping_in == 0u)
			goto geyser_front_read_script;
		OBJECT(current_object)->geyser_front.holding_back = 0xFFu;
		OBJECT(current_object)->geyser_front.saved_base_speed = OBJECT(current_object)->geyser_front.base_speed;
		OBJECT(current_object)->geyser_front.base_speed = 0;
		OBJECT(current_object)->geyser_front.hold_start_position = scroll_position;
		OBJECT(current_object)->geyser_front.hold_timer = 0xFFu;
		OBJECT(current_object)->geyser_front.saved_threshold = OBJECT(current_object)->geyser_front.screen_threshold;
		OBJECT(current_object)->geyser_front.screen_threshold = 0x130;
	}
	if (OBJECT(current_object)->geyser_front.holding_back != 0u)
	{
		if ((int16_t)(scroll_position - OBJECT(current_object)->geyser_front.hold_start_position) >= 0x300)
			goto geyser_front_stop_holding_back;
		if (no_player_in_play != 0u)
			OBJECT(current_object)->geyser_front.hold_timer = 0xFFu;
		hold_timer = OBJECT(current_object)->geyser_front.hold_timer;
		OBJECT(current_object)->geyser_front.hold_timer = hold_timer - 1u;
		if (hold_timer == 0u)
			goto geyser_front_stop_holding_back;
	}
	update_screen_position(&slope_step, &r1);
	if (OBJECT(current_object)->screen_y <= OBJECT(current_object)->geyser_front.screen_threshold)
	{
	geyser_front_use_base_speed:
		r1 = (uint16_t)OBJECT(current_object)->geyser_front.base_speed;
	}
	else
	{
		OBJECT(current_object)->geyser_front.wait_timer = 0;
		if (OBJECT(current_object)->geyser_front.holding_back != 0u)
		{
		geyser_front_stop_holding_back:
			OBJECT(current_object)->geyser_front.holding_back = 0u;
			OBJECT(current_object)->geyser_front.screen_threshold = OBJECT(current_object)->geyser_front.saved_threshold;
			OBJECT(current_object)->geyser_front.base_speed = OBJECT(current_object)->geyser_front.saved_base_speed;
		}
		compute_camera_ground_speed(0u);
		r1 = (uint16_t)((uint32_t)camera_ground_speed_int * OBJECT(current_object)->geyser_front.catch_up_factor) >> 8;
		if ((int16_t)r1 <= OBJECT(current_object)->geyser_front.base_speed)
			goto geyser_front_use_base_speed;
	}
	chaser_speed = r1;
	wait_timer = OBJECT(current_object)->geyser_front.wait_timer;
	OBJECT(current_object)->geyser_front.wait_timer = wait_timer - 1u;
	if ((int16_t)wait_timer <= 1)
	{
		OBJECT(current_object)->geyser_front.wait_timer = 0;
		OBJECT(current_object)->z -= (uint32_t)chaser_speed << 12;
		attribute_record = OBJECT(current_object)->floor_attribute + floor_types_ADDRESS;
		slope_step = ((uint32_t)(int16_t)ROM16(attribute_record + 4u) * chaser_speed) >> 4;
		r1 = slope_step;
		if (ROM8(attribute_record) != 0u)
			r1 = 0u - r1;
		OBJECT(current_object)->geyser_front.fixed_projected_x += r1;
		OBJECT(current_object)->projected_x = OBJECT(current_object)->geyser_front.projected_x_whole;
		OBJECT(current_object)->projected_y -= slope_step >> 1;
		point_x = OBJECT(current_object)->geyser_front.fixed_projected_x;
		point_y = OBJECT(current_object)->projected_y * 2u;
		unproject_point(&point_x, &point_y);
		OBJECT(current_object)->x = point_x;
		OBJECT(current_object)->y = point_y;
	}
	current_object = LINKED(current_object, OBJECT(current_object)->child);
	OBJECT(current_object)->x = OBJECT(object_cursor)->x;
	OBJECT(current_object)->x_int -= 0x10u;
	OBJECT(current_object)->y = OBJECT(object_cursor)->y;
	OBJECT(current_object)->z = OBJECT(object_cursor)->z;
	OBJECT(current_object)->z_int += OBJECT(current_object)->hitbox_height;
	OBJECT(current_object)->z_int += 0x60u;
	add_to_touchable_list(slope_step);
	current_object = object_cursor;
	r11 = spawn_listed_geyser(0u);
	OBJECT(current_object)->flags &= ~1u;
	if (OBJECT(current_object)->z_int <= OBJECT(current_object)->geyser_front.top_height)
	{
		OBJECT(current_object)->geyser_front.top_height = 0;
		spawn_geyser_line(r11);
		goto geyser_front_spawn_geyser_row;
	}
	if ((int16_t)(OBJECT(current_object)->previous_floor_z_int - OBJECT(current_object)->z_int) >= 0x30)
	{
	geyser_front_spawn_geyser_row:
		OBJECT(current_object)->flags |= 1u;
		OBJECT(current_object)->previous_floor_z_int = OBJECT(current_object)->z_int;
		geysers_left = 5;
		if (free_object_stack < geysers_left)
			return;
		geyser_y = OBJECT(current_object)->y;
		geyser_y_int += 0x10u;
		row_pattern = OBJECT(current_object)->geyser_front.row_pattern;
		row_delays = mem_read8(row_pattern);
		OBJECT(current_object)->geyser_front.row_pattern = row_pattern + 1u;
		row_delays = row_delays * 0xAu + geyser_row_delays_ADDRESS;
		geyser_delay_cursor = row_delays;
		do
		{
			x = OBJECT(current_object)->x;
			y = geyser_y;
			geyser_y_int += 0x2Eu;
			r14 = OBJECT(current_object)->z;
			wait = mem_read16(geyser_delay_cursor);
			geyser_delay_cursor += 2u;
			spawn_geyser(x, y, wait, &ignored, &row_delays);
			geysers_remaining = geysers_left;
			geysers_left = geysers_remaining - 1u;
		} while ((int16_t)geysers_remaining > 1);
	}
	if (OBJECT(current_object)->screen_y <= 0x130)
	{
		sound_timer = OBJECT(current_object)->geyser_front.sound_timer;
		OBJECT(current_object)->geyser_front.sound_timer = sound_timer - 1u;
		if ((int16_t)sound_timer <= 1)
		{
			OBJECT(current_object)->geyser_front.sound_timer = 0x32;
			sound_id = 0x12u;
			play_sound(sound_id, &r1, &r2);
		}
	}
}

uint32_t spawn_listed_geyser(uint32_t geyser)
{
	uint16_t wait = 0;
	uint32_t record = 0, spawned = 0, x = 0, y = 0;

	if (mem_read16(OBJECT(current_object)->spawn_listed_geyser.next_listed_geyser) <= scroll_position)
	{
		record = OBJECT(current_object)->spawn_listed_geyser.next_listed_geyser + 2u;
		x = (uint32_t)mem_read16(record) << 16;
		record += 2u;
		y = (uint32_t)mem_read16(record) << 16;
		record += 2u;
		r14 = (uint32_t)mem_read16(record) << 16;
		record += 2u;
		wait = mem_read16(record);
		OBJECT(current_object)->spawn_listed_geyser.next_listed_geyser = record + 2u;
		spawn_geyser(x, y, wait, &spawned, &geyser);
		if (spawned != 0u)
		{
			OBJECT(geyser)->flags |= 0x80000000u;
			OBJECT(geyser)->animation_speed = 0x20u;
		}
	}
	return geyser;
}

void spawn_geyser(uint32_t x, uint32_t y, uint16_t wait, uint32_t *p_spawned, uint32_t *p_geyser)
{
	uint32_t geyser = *p_geyser;
	uint32_t free_count = 0, spawned = 0;

	allocate_object(&free_count, &geyser);
	if (free_count == 0u)
	{
		*p_spawned = free_count;
		*p_geyser = geyser;
		return;
	}
	OBJECT(geyser)->x = x;
	OBJECT(geyser)->y = y;
	OBJECT(geyser)->z = r14;
	OBJECT(geyser)->geyser_wait.erupt_delay = wait;
	OBJECT(geyser)->sprite_flags = 0x1000u;
	OBJECT(geyser)->sprite_frame = 0x5DA0u;
	OBJECT(geyser)->sprite_palette = 0x8280u;
	OBJECT(geyser)->hitbox_size_x = 0x20u;
	OBJECT(geyser)->hitbox_size_y = 0x30u;
	OBJECT(geyser)->hitbox_height = 0x20u;
	OBJECT(geyser)->kind = 0x27u;
	OBJECT(geyser)->geyser_wait.draw_depth = 0xEFu;
	OBJECT(geyser)->handler = geyser_wait_ADDRESS;
	OBJECT(geyser)->direction = 0x300u;
	OBJECT(geyser)->sprite_offset_x = 0;
	OBJECT(geyser)->sprite_offset_y = -0x10;
	OBJECT(geyser)->flags = 2u;
	OBJECT(geyser)->hitbox_offset_x = -5;
	OBJECT(geyser)->hitbox_offset_y = -0x18;
	OBJECT(geyser)->parent = current_object;
	OBJECT(geyser)->animation_speed = 8u;
	OBJECT(geyser)->animation_timer = 0;
	OBJECT(geyser)->animation_frame = 0xFFu;
	OBJECT(geyser)->animation = geyser_animation_ADDRESS;
	spawned = 0xFFFFFFFFu;
	*p_spawned = spawned;
	*p_geyser = geyser;
}

void geyser_wait(void)
{
	uint32_t r1 = 0, ignored = 0;
	uint16_t erupt_delay = 0;

	if ((OBJECT(current_object)->flags & 0x80000000u) != 0u)
	{
		project_object(&ignored, &r1);
		OBJECT(current_object)->anchor_x = OBJECT(current_object)->projected_x;
		OBJECT(current_object)->anchor_y = OBJECT(current_object)->projected_y_int;
		if (OBJECT(LINKED(current_object, OBJECT(current_object)->parent))->var_8E == 0u && OBJECT(current_object)->geyser_wait.erupt_delay <= scroll_position)
			OBJECT(current_object)->handler = geyser_erupt_ADDRESS;
		r1 = queue_sprite(r1);
		return;
	}
	project_object(&ignored, &ignored);
	OBJECT(current_object)->anchor_x = OBJECT(current_object)->projected_x;
	OBJECT(current_object)->anchor_y = OBJECT(current_object)->projected_y_int;
	if ((OBJECT(LINKED(current_object, OBJECT(current_object)->parent))->flags & 1u) != 0u)
		OBJECT(current_object)->geyser_wait.draw_depth--;
	if (OBJECT(LINKED(current_object, OBJECT(current_object)->parent))->var_8E == 0u)
	{
		erupt_delay = OBJECT(current_object)->geyser_wait.erupt_delay;
		OBJECT(current_object)->geyser_wait.erupt_delay = erupt_delay - 1u;
		if ((int16_t)erupt_delay <= 1)
			OBJECT(current_object)->handler = geyser_erupt_ADDRESS;
	}
}

void geyser_erupt(void)
{
	uint8_t sound_id = 0;
	uint32_t r1 = 0, r2 = 0, at_last_step = 0, depth = 0, ignored = 0;

	if ((int8_t)OBJECT(current_object)->animation_frame >= 3)
		OBJECT(current_object)->animation_speed = 2u;
	at_last_step = animate_around_anchor();
	if (at_last_step != 0u)
		OBJECT(current_object)->animation = geyser_jet_animation_ADDRESS;
	if (OBJECT(current_object)->animation_frame == 3u && (uint8_t)OBJECT(current_object)->animation_timer == OBJECT(current_object)->animation_speed)
	{
		sound_id = 0xAu;
		play_sound(sound_id, &r1, &r2);
	}
	update_screen_position(&ignored, &r1);
	if (OBJECT(current_object)->screen_y > 0x180)
	{
		delete_current_object();
		return;
	}
	if (OBJECT(current_object)->animation == geyser_jet_animation_ADDRESS || (int8_t)OBJECT(current_object)->animation_frame > 3)
		add_to_touchable_list(ignored);
	if ((OBJECT(LINKED(current_object, OBJECT(current_object)->parent))->flags & 1u) != 0u)
		OBJECT(current_object)->geyser_erupt.draw_depth--;
	if ((OBJECT(current_object)->flags & 0x80000000u) != 0u || (OBJECT(current_object)->animation != geyser_jet_animation_ADDRESS && (int8_t)OBJECT(current_object)->animation_frame < 4))
	{
		r1 = queue_sprite(r1);
		return;
	}
	depth = OBJECT(current_object)->geyser_erupt.draw_depth;
	queue_sprite_at_depth(&ignored, &depth);
}

void spawn_waterfall(uint32_t record, uint32_t *p_spawned, uint32_t *p_free_stack, uint32_t *p_block)
{
	uint32_t free_stack = *p_free_stack;
	uint32_t block = *p_block;
	uint32_t free_count = 0, ignored = 0, object = 0, spawned = 0;

	allocate_object_outside_loop(&free_count, &block);
	if (free_count == 0u || (uint16_t)free_object_stack_2[0] == 0u)
	{
		*p_spawned = free_count;
		*p_block = block;
		return;
	}
	group_leaders[mem_read8(record) & 3u] = block;
	record++;
	OBJECT(block)->x_int = mem_read16(record);
	OBJECT(block)->x_fraction = 0u;
	record += 2u;
	OBJECT(block)->y_int = mem_read16(record);
	OBJECT(block)->y_fraction = 0u;
	record += 2u;
	OBJECT(block)->z_int = mem_read16(record);
	OBJECT(block)->z_fraction = 0u;
	OBJECT(block)->floor_z = OBJECT(block)->z;
	record += 2u;
	OBJECT(block)->direction = mem_read16(record);
	OBJECT(block)->waterfall.path = mem_read16(record + 2u) + group_leader_data_ADDRESS;
	OBJECT(block)->handler = waterfall_move_ADDRESS;
	OBJECT(block)->sprite_flags = 0xD00Au;
	OBJECT(block)->flags = 0u;
	OBJECT(block)->sprite_offset_x = 0;
	OBJECT(block)->sprite_offset_y = 0;
	OBJECT(block)->waterfall.holding_back = 0u;
	OBJECT(block)->waterfall.sound_timer = 0u;
	OBJECT(block)->waterfall.wait_timer = 1;
	OBJECT(block)->animation = waterfall_animation_ADDRESS;
	OBJECT(block)->animation_frame = 0u;
	OBJECT(block)->animation_speed = 3u;
	OBJECT(block)->animation_timer = 0;
	object = block;
	allocate_object_2(&ignored, &free_stack, &block);
	move_halfwords_backward(block, waterfall_parts_ADDRESS, 0xDu);
	OBJECT(object)->part_list = block;
	spawned = 0xFFFFFFFFu;
	*p_spawned = spawned;
	*p_free_stack = free_stack;
	*p_block = block;
}

void waterfall_move(void)
{
	uint8_t sound_id = 0;
	uint16_t part_list = 0;
	uint32_t r0 = 0, r2 = 0, r3 = 0, r8 = 0, r11 = 0, cursor = 0, depth = 0, ignored = 0, speed = 0;
	uint8_t hold_timer = 0;
	uint16_t wait_timer = 0, trigger_scroll = 0, sound_timer = 0;
	uint32_t flags = 0;

	flags = OBJECT(current_object)->flags;
	OBJECT(current_object)->flags = flags | 0x80000000u;
	if ((flags & 0x80000000u) == 0u)
	{
	waterfall_read_script:
		wait_timer = OBJECT(current_object)->waterfall.wait_timer;
		OBJECT(current_object)->waterfall.wait_timer = wait_timer - 1u;
		if ((int16_t)wait_timer <= 1)
		{
			OBJECT(current_object)->waterfall.wait_timer = 0;
			cursor = OBJECT(current_object)->waterfall.path;
			trigger_scroll = mem_read16(cursor);
			cursor += 2u;
			if (trigger_scroll <= scroll_position)
			{
				if ((int16_t)mem_read16(cursor + 2u) < 0)
				{
					part_list = (uint16_t)OBJECT(current_object)->part_list;
					free_object_2(part_list);
					delete_current_object();
					return;
				}
				OBJECT(current_object)->waterfall.screen_threshold = mem_read16(cursor);
				cursor += 2u;
				OBJECT(current_object)->waterfall.catch_up_factor = mem_read16(cursor);
				cursor += 2u;
				OBJECT(current_object)->waterfall.base_speed = mem_read16(cursor);
				cursor += 2u;
				OBJECT(current_object)->waterfall.wait_timer = mem_read16(cursor);
				OBJECT(current_object)->waterfall.path = cursor + 2u;
			}
		}
	}
	else if (OBJECT(current_object)->waterfall.holding_back == 0u)
	{
		if (no_player_in_play == 0u && player_mishap == 0u && player_dropping_in == 0u)
			goto waterfall_read_script;
		OBJECT(current_object)->waterfall.holding_back = 0xFFu;
		OBJECT(current_object)->waterfall.saved_base_speed = OBJECT(current_object)->waterfall.base_speed;
		OBJECT(current_object)->waterfall.base_speed = 0;
		OBJECT(current_object)->waterfall.hold_start_position = scroll_position;
		OBJECT(current_object)->waterfall.hold_timer = 0xFFu;
		OBJECT(current_object)->waterfall.saved_threshold = OBJECT(current_object)->waterfall.screen_threshold;
		OBJECT(current_object)->waterfall.screen_threshold = 0x160;
	}
	find_floor(&r0, &speed, &r2, &r3, &r11);
	project_object(&ignored, &ignored);
	if (OBJECT(current_object)->waterfall.holding_back != 0u)
	{
		if ((int16_t)(scroll_position - OBJECT(current_object)->waterfall.hold_start_position) >= 0x300)
			goto waterfall_stop_holding_back;
		if (no_player_in_play != 0u)
			OBJECT(current_object)->waterfall.hold_timer = 0xFFu;
		hold_timer = OBJECT(current_object)->waterfall.hold_timer;
		OBJECT(current_object)->waterfall.hold_timer = hold_timer - 1u;
		if (hold_timer == 0u)
			goto waterfall_stop_holding_back;
	}
	if (OBJECT(current_object)->screen_x <= OBJECT(current_object)->waterfall.screen_threshold && OBJECT(current_object)->screen_y <= 0xE0)
	{
		speed = (uint16_t)OBJECT(current_object)->waterfall.base_speed;
		chaser_speed = speed;
		if (OBJECT(current_object)->waterfall.wait_timer <= 0)
			OBJECT(current_object)->y -= (uint32_t)chaser_speed << 12;
		if (OBJECT(current_object)->y_int <= 0x1480)
			geyser_front_start = 0xFFu;
		if (map_screen_active == 0u)
		{
			sound_timer = OBJECT(current_object)->waterfall.sound_timer;
			OBJECT(current_object)->waterfall.sound_timer = sound_timer - 1u;
			if ((int16_t)sound_timer <= 1)
			{
				OBJECT(current_object)->waterfall.sound_timer = 0x3Cu;
				sound_id = 0x10u;
				play_sound(sound_id, &speed, &r2);
			}
		}
		animate_all_parts(&r8, &r11);
		depth = (uint16_t)(OBJECT(current_object)->screen_y - 0x60u);
		queue_sprite_at_depth(&ignored, &depth);
		return;
	}
	OBJECT(current_object)->waterfall.wait_timer = 0;
	if (OBJECT(current_object)->waterfall.holding_back != 0u)
	{
	waterfall_stop_holding_back:
		OBJECT(current_object)->waterfall.holding_back = 0u;
		OBJECT(current_object)->waterfall.screen_threshold = OBJECT(current_object)->waterfall.saved_threshold;
		OBJECT(current_object)->waterfall.base_speed = OBJECT(current_object)->waterfall.saved_base_speed;
	}
	compute_camera_ground_speed(r3);
	speed = (uint16_t)((uint32_t)camera_ground_speed_int * OBJECT(current_object)->waterfall.catch_up_factor) >> 8;
	if ((int16_t)speed <= OBJECT(current_object)->waterfall.base_speed)
		speed = (uint16_t)OBJECT(current_object)->waterfall.base_speed;
	chaser_speed = speed;
	if (OBJECT(current_object)->waterfall.wait_timer <= 0)
		OBJECT(current_object)->y -= (uint32_t)chaser_speed << 12;
	if (OBJECT(current_object)->y_int <= 0x1480)
		geyser_front_start = 0xFFu;
	if (map_screen_active == 0u)
	{
		sound_timer = OBJECT(current_object)->waterfall.sound_timer;
		OBJECT(current_object)->waterfall.sound_timer = sound_timer - 1u;
		if ((int16_t)sound_timer <= 1)
		{
			OBJECT(current_object)->waterfall.sound_timer = 0x3Cu;
			sound_id = 0x10u;
			play_sound(sound_id, &speed, &r2);
		}
	}
	animate_all_parts(&r8, &r11);
	depth = (uint16_t)(OBJECT(current_object)->screen_y - 0x60u);
	queue_sprite_at_depth(&ignored, &depth);
}

void river_water_update(void)
{
	uint16_t child = 0;
	uint32_t r1 = 0, clip_x = 0, clips_left = 0, depth = 0, floor_z = 0, ignored = 0, object = 0, object_2 = 0, object_3 = 0, point_y = 0;

	switch (OBJECT(current_object)->river_water.state)
	{
	case 0:
	case 4:
		project_object(&ignored, &r1);
		OBJECT(current_object)->river_water.state++;
		current_player = current_object;
		current_player = LINKED(current_player, OBJECT(current_object)->parent);
		if (OBJECT(current_player)->y_int <= OBJECT(current_object)->y_int)
			OBJECT(current_object)->river_water.state++;
		if (OBJECT(current_object)->screen_y > 0xE8)
		{
			delete_current_object();
			return;
		}
		OBJECT(current_object)->sprite_frame = mem_read16(OBJECT(current_object)->animation + 2u + OBJECT(LINKED(current_object, OBJECT(current_object)->parent))->animation_frame * 2u);
		update_screen_position(&ignored, &r1);
		if ((uint16_t)(OBJECT(current_object)->screen_x + 0x10u) >= 0x1A1u || OBJECT(current_object)->screen_y <= -0xB0)
			return;
		depth = (uint16_t)(OBJECT(current_object)->screen_y + OBJECT(current_object)->river_water.depth_offset);
		queue_sprite_at_depth(&ignored, &depth);
		return;
	case 1:
		current_player = current_object;
		current_player = LINKED(current_player, OBJECT(current_object)->parent);
		if (OBJECT(current_player)->y_int <= OBJECT(current_object)->y_int)
			OBJECT(current_object)->river_water.state++;
		if (OBJECT(current_object)->screen_y > 0xE8)
		{
			delete_current_object();
			return;
		}
		OBJECT(current_object)->sprite_frame = mem_read16(OBJECT(current_object)->animation + 2u + OBJECT(LINKED(current_object, OBJECT(current_object)->parent))->animation_frame * 2u);
		update_screen_position(&ignored, &r1);
		if ((uint16_t)(OBJECT(current_object)->screen_x + 0x10u) >= 0x1A1u || OBJECT(current_object)->screen_y <= -0xB0)
			return;
		depth = (uint16_t)(OBJECT(current_object)->screen_y + OBJECT(current_object)->river_water.depth_offset);
		queue_sprite_at_depth(&ignored, &depth);
		return;
	case 2:
		if (free_object_stack_2[0] < 2)
		{
			if (OBJECT(current_object)->screen_y > 0xE8)
			{
				delete_current_object();
				return;
			}
			OBJECT(current_object)->sprite_frame = mem_read16(OBJECT(current_object)->animation + 2u + OBJECT(LINKED(current_object, OBJECT(current_object)->parent))->animation_frame * 2u);
			update_screen_position(&ignored, &r1);
			if ((uint16_t)(OBJECT(current_object)->screen_x + 0x10u) >= 0x1A1u || OBJECT(current_object)->screen_y <= -0xB0)
				return;
			depth = (uint16_t)(OBJECT(current_object)->screen_y + OBJECT(current_object)->river_water.depth_offset);
			queue_sprite_at_depth(&ignored, &depth);
			return;
		}
		OBJECT(current_object)->river_water.state++;
		clips_left = 2u;
		object_2 = current_object;
		do
		{
			object_3 = object_2;
			allocate_object_2(&ignored, &ignored, &object_2);
			object = object_2 + 2u;
			OBJECT(object - 0x2u)->sprite_flags = 0x5000u;
			object += 2u;
			OBJECT(object - 0x4u)->sprite_frame = 0xE0u;
			object += 2u;
			OBJECT(object - 0x6u)->sprite_palette = 0u;
			object += 2u;
			OBJECT(object - 0x8u)->sprite_scale = 0x140u;
			object += 2u;
			OBJECT(object - 0xAu)->sprite_layer = 0u;
			object += 2u;
			OBJECT(object - 0xCu)->screen_y = 0;
			object += 2u;
			OBJECT(object - 0xEu)->screen_x = 0;
			OBJECT(object - 0xEu)->hitbox_size_x = 0u;
			OBJECT(object_2)->parent = object_3;
			OBJECT(object_3)->child = object_2;
			clips_left--;
		} while (clips_left != 0u);
	case 3:
		OBJECT(current_object)->sprite_frame = mem_read16(OBJECT(current_object)->animation + 2u + OBJECT(LINKED(current_object, OBJECT(current_object)->parent))->animation_frame * 2u);
		update_screen_position(&ignored, &r1);
		object = LINKED(current_object, OBJECT(current_object)->parent);
		if ((int16_t)(OBJECT(current_object)->y_int - 0x5Cu) < OBJECT(object)->y_int)
		{
			clip_x = OBJECT(current_object)->x;
			point_y = OBJECT(object)->y;
			floor_z = OBJECT(current_object)->floor_z;
			project_point(floor_z, &ignored, &clip_x, &point_y);
			OBJECT(LINKED(current_object, OBJECT(current_object)->child))->sprite_scale = clip_x;
			if ((uint16_t)(OBJECT(current_object)->screen_x + 0x10u) < 0x1A1u)
			{
				OBJECT(current_object)->river_water.draw_depth = OBJECT(current_object)->screen_y + OBJECT(current_object)->river_water.depth_offset;
				current_object = LINKED(current_object, OBJECT(current_object)->child);
				current_object = LINKED(current_object, OBJECT(current_object)->child);
				depth = OBJECT(object_cursor)->river_water.draw_depth;
				queue_sprite_at_depth(&ignored, &depth);
				current_object = object_cursor;
				depth = OBJECT(object_cursor)->river_water.draw_depth;
				queue_sprite_at_depth(&ignored, &depth);
				current_object = LINKED(current_object, OBJECT(current_object)->child);
				depth = OBJECT(object_cursor)->river_water.draw_depth;
				queue_sprite_at_depth(&ignored, &depth);
			}
			return;
		}
		child = OBJECT(current_object)->child;
		free_object_2(child);
		current_object = LINKED(current_object, OBJECT(current_object)->child);
		child = OBJECT(current_object)->child;
		free_object_2(child);
		delete_current_object();
		return;
	default:
		unknown_code(0x8CEC1u + OBJECT(current_object)->river_water.state * 3u);
	}
}

void stone_block_init(void)
{
	uint32_t ignored = 0, record = 0;

	record = stone_block_positions_ADDRESS + OBJECT(current_object)->sprite_frame * 2u * 4u;
	OBJECT(current_object)->x_int = ROM16(record);
	OBJECT(current_object)->x_fraction = 0u;
	record += 2u;
	OBJECT(current_object)->y_int = ROM16(record);
	OBJECT(current_object)->y_fraction = 0u;
	record += 2u;
	OBJECT(current_object)->z_int = ROM16(record);
	OBJECT(current_object)->z_fraction = 0u;
	OBJECT(current_object)->stone_block.depth_offset = ROM16(record + 2u);
	OBJECT(current_object)->sprite_flags = 0x1000u;
	OBJECT(current_object)->sprite_frame = 0x8A00u;
	OBJECT(current_object)->sprite_palette = 0x9120u;
	OBJECT(current_object)->flags = 0u;
	OBJECT(current_object)->sprite_offset_x = 0;
	OBJECT(current_object)->sprite_offset_y = 0;
	project_object(&ignored, &ignored);
	OBJECT(current_object)->handler = stone_block_draw_ADDRESS;
}

void stone_block_draw(void)
{
	uint32_t r1 = 0, depth = 0, ignored = 0, off_screen = 0;

	off_screen = check_left_screen();
	if (off_screen == 0u)
	{
		update_screen_position(&ignored, &r1);
		depth = (uint16_t)(OBJECT(current_object)->screen_y + OBJECT(current_object)->stone_block.depth_offset);
		queue_sprite_at_depth(&ignored, &depth);
		return;
	}
	delete_current_object();
}

void spawn_geyser_line(uint32_t line)
{
	uint32_t free_count = 0, record = 0;

	allocate_object(&free_count, &line);
	if (free_count != 0u)
	{
		record = ((uint32_t)OBJECT(current_object)->sprite_frame << 3) + geyser_line_starts_ADDRESS;
		OBJECT(line)->x_int = ROM16(record);
		OBJECT(line)->x_fraction = 0u;
		record += 2u;
		OBJECT(line)->y_int = ROM16(record);
		OBJECT(line)->y_fraction = 0u;
		record += 2u;
		OBJECT(line)->z_int = ROM16(record);
		OBJECT(line)->z_fraction = 0u;
		OBJECT(line)->floor_z = OBJECT(line)->z;
		OBJECT(line)->geyser_line.path = ROM16(record + 2u) + geyser_line_scripts_ADDRESS;
		OBJECT(line)->geyser_line.holding_back = 0u;
		OBJECT(line)->geyser_line.wait_timer = 0;
		OBJECT(line)->flags = 0u;
		OBJECT(line)->handler = geyser_line_move_ADDRESS;
		OBJECT(line)->geyser_line.travel = 0;
	}
}

void geyser_line_move(void)
{
	uint32_t cursor = 0, hold_distance = 0, ignored = 0, speed = 0;
	uint8_t hold_timer = 0;
	uint16_t wait_timer = 0, trigger_scroll = 0, trigger_y = 0;

	if (OBJECT(current_object)->geyser_line.holding_back == 0u)
	{
		if (no_player_in_play != 0u || player_mishap != 0u || player_dropping_in != 0u)
		{
			OBJECT(current_object)->geyser_line.holding_back = 0xFFu;
			OBJECT(current_object)->geyser_line.hold_start_position = scroll_position;
			OBJECT(current_object)->geyser_line.hold_timer = 0xFFu;
		}
		else
		{
			wait_timer = OBJECT(current_object)->geyser_line.wait_timer;
			OBJECT(current_object)->geyser_line.wait_timer = wait_timer - 1u;
			if ((int16_t)wait_timer <= 1)
			{
				OBJECT(current_object)->geyser_line.wait_timer = 0;
				cursor = OBJECT(current_object)->geyser_line.path;
				trigger_scroll = mem_read16(cursor);
				cursor += 2u;
				if (trigger_scroll <= scroll_position)
				{
					cursor += 2u;
					goto geyser_line_load_record;
				}
				trigger_y = mem_read16(cursor);
				cursor += 2u;
				if (OBJECT(current_object)->y_int <= (int16_t)trigger_y)
				{
				geyser_line_load_record:
					if ((int16_t)mem_read16(cursor + 2u) < 0)
					{
						spawn_geyser_burst();
						delete_current_object();
						return;
					}
					OBJECT(current_object)->geyser_line.speed = mem_read16(cursor);
					cursor += 2u;
					OBJECT(current_object)->geyser_line.wait_timer = mem_read16(cursor);
					OBJECT(current_object)->geyser_line.path = cursor + 2u;
				}
			}
		}
	}
	project_object(&hold_distance, &ignored);
	if (OBJECT(current_object)->geyser_line.holding_back != 0u)
	{
		hold_distance = (hold_distance & ~0xFFFFu) | scroll_position;
		hold_distance = (hold_distance & ~0xFFFFu) | (uint16_t)(hold_distance - OBJECT(current_object)->geyser_line.hold_start_position);
		if ((int16_t)hold_distance >= 0x300)
			OBJECT(current_object)->geyser_line.holding_back = 0u;
		else
		{
			if (no_player_in_play != 0u)
				OBJECT(current_object)->geyser_line.hold_timer = 0xFFu;
			hold_timer = OBJECT(current_object)->geyser_line.hold_timer;
			OBJECT(current_object)->geyser_line.hold_timer = hold_timer - 1u;
			if (hold_timer == 0u)
			{
				OBJECT(current_object)->geyser_line.holding_back = 0u;
				if (OBJECT(current_object)->geyser_line.holding_back == 0u)
				{
					speed = (hold_distance & ~0xFFFFu) | (uint16_t)OBJECT(current_object)->geyser_line.speed;
					OBJECT(current_object)->geyser_line.travel += speed;
					OBJECT(current_object)->y -= speed << 12;
					if (OBJECT(current_object)->geyser_line.travel >= 0x300)
					{
						spawn_geyser_burst();
						OBJECT(current_object)->geyser_line.travel = 0;
					}
				}
				return;
			}
		}
	}
	if (OBJECT(current_object)->geyser_line.holding_back == 0u)
	{
		speed = (hold_distance & ~0xFFFFu) | (uint16_t)OBJECT(current_object)->geyser_line.speed;
		OBJECT(current_object)->geyser_line.travel += speed;
		OBJECT(current_object)->y -= speed << 12;
		if (OBJECT(current_object)->geyser_line.travel >= 0x300)
		{
			spawn_geyser_burst();
			OBJECT(current_object)->geyser_line.travel = 0;
		}
	}
}

void spawn_geyser_burst(void)
{
	uint32_t free_count = 0, object = 0;

	allocate_object(&free_count, &object);
	if (free_count != 0u)
	{
		OBJECT(object)->sprite_flags = 0x1004u;
		OBJECT(object)->sprite_palette = 0x9130u;
		OBJECT(object)->handler = geyser_burst_start_ADDRESS;
		OBJECT(object)->hitbox_size_x = 0x30u;
		OBJECT(object)->hitbox_size_y = 0x30u;
		OBJECT(object)->hitbox_offset_x = 0;
		OBJECT(object)->hitbox_offset_y = 0;
		OBJECT(object)->sprite_offset_x = 0x14;
		OBJECT(object)->sprite_offset_y = 8;
		OBJECT(object)->hitbox_height = 0xA0u;
		OBJECT(object)->flags = 2u;
		OBJECT(object)->kind = 0x29u;
		OBJECT(object)->direction = 0x100u;
		OBJECT(object)->animation = geyser_burst_animation_ADDRESS;
		OBJECT(object)->animation_frame = 0xFFu;
		OBJECT(object)->animation_speed = 6u;
		OBJECT(object)->animation_timer = 0;
		OBJECT(object)->x = OBJECT(current_object)->x;
		OBJECT(object)->y = OBJECT(current_object)->y;
		OBJECT(object)->z = OBJECT(current_object)->z;
	}
}

void geyser_burst_start(void)
{
	uint32_t ignored = 0;

	project_object(&ignored, &ignored);
	OBJECT(current_object)->handler = geyser_burst_erupt_ADDRESS;
}

void geyser_burst_erupt(void)
{
	uint8_t sound_id = 0;
	uint32_t r1 = 0, r2 = 0, r7 = 0, r8 = 0, depth = 0, ignored = 0;

	r8 = is_off_screen();
	if (r8 != 0u)
	{
		delete_current_object();
		return;
	}
	animate(&r1, &r7, &r8);
	if (OBJECT(current_object)->animation != geyser_burst_spray_animation_ADDRESS)
	{
		if (OBJECT(current_object)->animation_frame != 4u)
		{
			if ((uint8_t)r8 != 0u)
				OBJECT(current_object)->animation = geyser_burst_spray_animation_ADDRESS;
		}
		else
			OBJECT(current_object)->animation_speed = 2u;
	}
	if (OBJECT(current_object)->animation != geyser_burst_spray_animation_ADDRESS && OBJECT(current_object)->animation_frame == 4u && OBJECT(current_object)->animation_speed == (uint8_t)OBJECT(current_object)->animation_timer)
	{
		sound_id = 0xAu;
		play_sound(sound_id, &r1, &r2);
	}
	update_screen_position(&ignored, &r1);
	if (OBJECT(current_object)->animation == geyser_burst_spray_animation_ADDRESS)
		add_to_touchable_list(ignored);
	depth = (uint16_t)(OBJECT(current_object)->screen_y - 0x10u);
	queue_sprite_at_depth(&ignored, &depth);
}

void propeller_machine_init(void)
{
	uint32_t free_count = 0, parts = 0, zone_index = 0;

	OBJECT(current_object)->propeller_machine.path = propeller_machine_path_ADDRESS;
	OBJECT(current_object)->sprite_flags = 0xD004u;
	OBJECT(current_object)->kind = 0x18u;
	OBJECT(current_object)->flags = 0x40000002u;
	OBJECT(current_object)->hitbox_size_x = 0xF2u;
	OBJECT(current_object)->hitbox_size_y = 0x80u;
	OBJECT(current_object)->direction = 0x200u;
	OBJECT(current_object)->hitbox_height = 0xF0u;
	OBJECT(current_object)->sprite_offset_x = 0;
	OBJECT(current_object)->sprite_offset_y = -8;
	OBJECT(current_object)->hitbox_offset_x = -8;
	OBJECT(current_object)->hitbox_offset_y = -0x70;
	chaser_object = current_object;
	OBJECT(current_object)->propeller_machine.sound_timer = 0u;
	OBJECT(current_object)->animation = propeller_machine_animation_ADDRESS;
	OBJECT(current_object)->animation_frame = 0u;
	OBJECT(current_object)->animation_speed = 5u;
	OBJECT(current_object)->animation_timer = 0;
	OBJECT(current_object)->propeller_machine.holding_back = 0u;
	OBJECT(current_object)->propeller_machine.wait_timer = 0;
	OBJECT(current_object)->handler = propeller_machine_init_ADDRESS;
	take_free_object(&free_count, &parts);
	if (free_count != 0u)
	{
		move_halfwords(parts, propeller_machine_parts_ADDRESS, 0x2Du);
		OBJECT(current_object)->part_list = parts;
		zone_index = 0u;
		spawn_chaser_back_zone(zone_index, parts);
		OBJECT(current_object)->handler = propeller_machine_chase_ADDRESS;
	}
}

void propeller_machine_chase(void)
{
	uint32_t r1 = 0, animation = 0, cursor = 0, depth = 0, frame_entry = 0, ignored = 0, part_frame = 0, slope_factor = 0, sound_id = 0;
	uint8_t hold_timer = 0, animation_timer = 0;
	uint16_t wait_timer = 0, trigger_scroll = 0, trigger_y = 0, sound_timer = 0;
	uint32_t flags = 0;

	flags = OBJECT(current_object)->flags;
	OBJECT(current_object)->flags = flags | 0x80000000u;
	if ((flags & 0x80000000u) == 0u)
	{
	propeller_machine_read_script:
		wait_timer = OBJECT(current_object)->propeller_machine.wait_timer;
		OBJECT(current_object)->propeller_machine.wait_timer = wait_timer - 1u;
		if ((int16_t)wait_timer <= 1)
		{
			OBJECT(current_object)->propeller_machine.wait_timer = 0;
			cursor = OBJECT(current_object)->propeller_machine.path;
			trigger_scroll = mem_read16(cursor);
			cursor += 2u;
			if (trigger_scroll <= scroll_position)
			{
				cursor += 2u;
				goto propeller_machine_load_record;
			}
			trigger_y = mem_read16(cursor);
			cursor += 2u;
			if ((int16_t)trigger_y <= OBJECT(current_object)->y_int)
			{
			propeller_machine_load_record:
				if ((int16_t)mem_read16(cursor + 2u) < 0)
				{
					current_object = (current_object & ~0xFFFFu) | OBJECT(current_object)->sprite_frame;
					push_free_object();
					delete_current_object();
					return;
				}
				OBJECT(current_object)->propeller_machine.screen_threshold = mem_read16(cursor);
				cursor += 2u;
				OBJECT(current_object)->propeller_machine.catch_up_factor = mem_read16(cursor);
				cursor += 2u;
				OBJECT(current_object)->propeller_machine.base_speed = mem_read16(cursor);
				cursor += 2u;
				OBJECT(current_object)->propeller_machine.wait_timer = mem_read16(cursor);
				OBJECT(current_object)->propeller_machine.path = cursor + 2u;
			}
		}
	}
	else if (OBJECT(current_object)->propeller_machine.holding_back == 0u)
	{
		if (no_player_in_play == 0u && player_mishap == 0u && player_dropping_in == 0u)
			goto propeller_machine_read_script;
		OBJECT(current_object)->propeller_machine.holding_back = 0xFFu;
		OBJECT(current_object)->propeller_machine.saved_base_speed = OBJECT(current_object)->propeller_machine.base_speed;
		OBJECT(current_object)->propeller_machine.base_speed = 0;
		OBJECT(current_object)->propeller_machine.hold_start_position = scroll_position;
		OBJECT(current_object)->propeller_machine.hold_timer = 0xFFu;
		OBJECT(current_object)->propeller_machine.saved_threshold = OBJECT(current_object)->propeller_machine.screen_threshold;
		OBJECT(current_object)->propeller_machine.screen_threshold = -0x50;
	}
	if (OBJECT(current_object)->propeller_machine.holding_back != 0u)
	{
		if ((int16_t)(scroll_position - OBJECT(current_object)->propeller_machine.hold_start_position) >= 0x300)
		{
		propeller_machine_stop_holding_back:
			OBJECT(current_object)->propeller_machine.holding_back = 0u;
			OBJECT(current_object)->propeller_machine.screen_threshold = OBJECT(current_object)->propeller_machine.saved_threshold;
			OBJECT(current_object)->propeller_machine.base_speed = OBJECT(current_object)->propeller_machine.saved_base_speed;
		}
		else
		{
			if (no_player_in_play != 0u)
				OBJECT(current_object)->propeller_machine.hold_timer = 0xFFu;
			hold_timer = OBJECT(current_object)->propeller_machine.hold_timer;
			OBJECT(current_object)->propeller_machine.hold_timer = hold_timer - 1u;
			if (hold_timer == 0u)
				goto propeller_machine_stop_holding_back;
		}
	}
	if (OBJECT(current_object)->propeller_machine.wait_timer <= 0)
	{
		if (OBJECT(current_object)->propeller_machine.screen_threshold <= OBJECT(current_object)->screen_x)
		{
		propeller_machine_use_base_speed:
			r1 = (uint16_t)OBJECT(current_object)->propeller_machine.base_speed;
		}
		else
		{
			slope_factor = 0u;
			compute_camera_slope_speed(0u, slope_factor);
			r1 = (uint16_t)((uint32_t)camera_slope_speed_int * OBJECT(current_object)->propeller_machine.catch_up_factor) >> 8;
			if ((int16_t)r1 <= OBJECT(current_object)->propeller_machine.base_speed)
				goto propeller_machine_use_base_speed;
		}
		chaser_speed = r1;
		OBJECT(current_object)->y += (uint32_t)chaser_speed << 12;
	}
	animation_timer = OBJECT(current_object)->animation_timer;
	OBJECT(current_object)->animation_timer = animation_timer - 1u;
	if ((int8_t)animation_timer <= 1)
	{
		OBJECT(current_object)->animation_timer = OBJECT(current_object)->animation_speed;
		animation = OBJECT(current_object)->animation;
		r1 = OBJECT(current_object)->animation_frame;
		r1 = (uint16_t)(r1 + 1u);
		if ((int16_t)mem_read16(animation) < (int16_t)r1)
			r1 = 0u;
		OBJECT(current_object)->animation_frame = r1;
		frame_entry = animation + 2u + r1 * 4u;
		part_frame = mem_read16(frame_entry);
		mem_write16(OBJECT(current_object)->part_list + 0x22u, mem_read16(frame_entry + 2u));
		mem_write16(OBJECT(current_object)->part_list + 0x32u, part_frame);
		mem_write16(OBJECT(current_object)->part_list + 0x4Au, part_frame);
	}
	if (OBJECT(current_object)->y_int >= 0x1BB0)
		geyser_front_start = 0xFFu;
	if (OBJECT(current_object)->y_int >= 0x1B00)
		OBJECT(current_object)->hitbox_height = 0x68u;
	if (OBJECT(current_object)->screen_y < 0x130)
	{
		if (OBJECT(current_object)->screen_x < -0x80)
			OBJECT(current_object)->propeller_machine.sound_timer = 0u;
		else
		{
			sound_timer = OBJECT(current_object)->propeller_machine.sound_timer;
			OBJECT(current_object)->propeller_machine.sound_timer = sound_timer - 1u;
			if ((int16_t)sound_timer <= 1)
			{
				OBJECT(current_object)->propeller_machine.sound_timer = 0x3Cu;
				sound_id = 0xBu;
				play_sound((uint8_t)sound_id, &r1, &part_frame);
			}
		}
	}
	find_floor(&sound_id, &r1, &part_frame, &ignored, &frame_entry);
	OBJECT(current_object)->z = OBJECT(current_object)->floor_z;
	project_object(&ignored, &ignored);
	add_to_zone_list(ignored);
	depth = (uint16_t)(OBJECT(current_object)->screen_y - 0x40u);
	queue_sprite_at_depth(&ignored, &depth);
}

void barred_gate_init(void)
{
	uint32_t ignored = 0;

	OBJECT(current_object)->sprite_flags = 0x1008u;
	OBJECT(current_object)->sprite_frame = 0x9F00u;
	OBJECT(current_object)->sprite_palette = 0x92F0u;
	OBJECT(current_object)->sprite_offset_x = 0;
	OBJECT(current_object)->sprite_offset_y = -0x49;
	OBJECT(current_object)->flags = 0u;
	OBJECT(current_object)->animation = barred_gate_animation_ADDRESS;
	OBJECT(current_object)->animation_frame = 0u;
	OBJECT(current_object)->animation_speed = 2u;
	OBJECT(current_object)->animation_timer = 0;
	OBJECT(current_object)->z = 0x1580000;
	project_object(&ignored, &ignored);
	OBJECT(current_object)->anchor_x = OBJECT(current_object)->projected_x;
	OBJECT(current_object)->anchor_y = OBJECT(current_object)->projected_y_int;
	OBJECT(current_object)->handler = barred_gate_update_ADDRESS;
}

void barred_gate_update(void)
{
	uint8_t sound_id = 0;
	uint32_t r2 = 0, r7 = 0, r8 = 0, depth = 0, ignored = 0, margin = 0;

	margin = 0x50u;
	r8 = check_left_screen_margin((uint16_t)margin);
	if (r8 != 0u)
	{
		delete_current_object();
		return;
	}
	if ((int8_t)OBJECT(current_object)->animation_frame >= 0 && barred_gate_closing != 0u)
	{
		animate(&margin, &r7, &r8);
		if ((uint8_t)r8 != 0u)
		{
			OBJECT(current_object)->animation_frame = 0xFFu;
			sound_id = 0x29u;
			play_sound(sound_id, &margin, &r2);
		}
	}
	update_screen_position(&ignored, &margin);
	depth = (uint16_t)(OBJECT(current_object)->screen_y - 0x40u);
	queue_sprite_at_depth(&ignored, &depth);
}

void scene_anchor_init(void)
{
	uint32_t record = 0;

	record = OBJECT(current_object)->sprite_frame * 8u + scene_anchor_positions_ADDRESS;
	OBJECT(current_object)->x_int = ROM16(record);
	OBJECT(current_object)->x_fraction = 0u;
	record += 2u;
	OBJECT(current_object)->y_int = ROM16(record);
	OBJECT(current_object)->y_fraction = 0u;
	record += 2u;
	OBJECT(current_object)->floor_z_int = ROM16(record);
	OBJECT(current_object)->floor_z_fraction = 0u;
	OBJECT(current_object)->scene_anchor.end_position = ROM16(record + 2u);
	OBJECT(current_object)->handler = scene_anchor_update_ADDRESS;
	scene_object = current_object;
}

void scene_anchor_update(void)
{
	uint32_t floor_z = 0, ignored = 0, point_x = 0, point_y = 0;

	if (OBJECT(current_object)->scene_anchor.end_position <= scroll_position)
	{
		delete_current_object();
		return;
	}
	point_x = OBJECT(current_object)->x;
	point_y = OBJECT(current_object)->y;
	floor_z = OBJECT(current_object)->floor_z;
	project_point(floor_z, &ignored, &point_x, &point_y);
	OBJECT(current_object)->z_int = 0x70u + view_y - (point_y + view_y - OBJECT(current_object)->floor_z_int) + 0x30u;
	OBJECT(current_object)->z_fraction = 0u;
}

void railing_spawner_init(void)
{
	OBJECT(current_object)->railing_spawner.last_spawn_position = 0u;
	OBJECT(current_object)->z = 0x5200000;
	OBJECT(current_object)->handler = railing_spawner_update_ADDRESS;
}

void railing_spawner_update(void)
{
	uint32_t ignored = 0, object = 0;
	uint16_t segments_left = 0;

	if (scroll_position >= 0x5000u)
	{
		delete_current_object();
		return;
	}
	if ((int16_t)(scroll_position - OBJECT(current_object)->railing_spawner.last_spawn_position) < 0x800 || free_object_stack < 0xA)
		return;
	OBJECT(current_object)->railing_spawner.last_spawn_position = scroll_position;
	railing_segments_left = 0xA;
	OBJECT(current_object)->y_int += railing_segments_left * 0xB8u;
	railing_segment_y = OBJECT(current_object)->y;
	do
	{
		allocate_object(&ignored, &object);
		OBJECT(object)->sprite_flags = 0xD002u;
		OBJECT(object)->part_list = railing_parts_ADDRESS;
		OBJECT(object)->flags = 0u;
		OBJECT(object)->sprite_offset_x = 0;
		OBJECT(object)->sprite_offset_y = 4;
		OBJECT(object)->handler = railing_segment_start_ADDRESS;
		OBJECT(object)->z = 0x5280000;
		railing_segment_y_int -= 0xB8u;
		OBJECT(object)->x = OBJECT(current_object)->x;
		OBJECT(object)->y = railing_segment_y;
		OBJECT(object)->z = OBJECT(current_object)->z;
		segments_left = railing_segments_left;
		railing_segments_left = segments_left - 1u;
	} while ((int16_t)segments_left > 1);
}

void railing_segment_start(void)
{
	uint32_t ignored = 0;

	project_object(&ignored, &ignored);
	OBJECT(current_object)->handler = railing_segment_draw_ADDRESS;
}

void railing_segment_draw(void)
{
	uint32_t depth = 0, ignored = 0, margin = 0, off_screen = 0;

	if (scroll_position >= 0x6700u)
	{
		delete_current_object();
		return;
	}
	margin = 0x88u;
	off_screen = check_left_screen_margin((uint16_t)margin);
	if (off_screen != 0u)
	{
		delete_current_object();
		return;
	}
	update_screen_position(&ignored, &margin);
	if ((uint16_t)(OBJECT(current_object)->screen_x + 0x88u) >= 0x1C8u)
		return;
	depth = (uint16_t)(OBJECT(current_object)->screen_y + 0x40u);
	queue_sprite_at_depth(&ignored, &depth);
}

void reset_linked_objects_flag(void)
{
	electric_fence_state = 0u;
}

void start_spawns_by_type(void)
{
	uint32_t stage_number = 0;

	stage_number = stage;
	type_spawn_stage = stage_number;
	type_spawn_cursor[0] = stage_spawns_by_type[stage_number];
	unused_type_spawn_byte = 0u;
	string_count = 0;
	stage_trigger_bits = 0u;
}

void spawn_objects_by_type(uint32_t *p_r1, uint32_t *p_r2, uint32_t *p_first, uint32_t *p_projected_x, uint32_t *p_projected_y, uint32_t *p_object_type, uint32_t *p_r11)
{
	uint32_t r1 = *p_r1;
	uint32_t r2 = *p_r2;
	uint32_t first = *p_first;
	uint32_t projected_x = *p_projected_x;
	uint32_t projected_y = *p_projected_y;
	uint32_t object_type = *p_object_type;
	uint32_t r11 = *p_r11;
	uint16_t spawn_z = 0;
	uint32_t r0 = 0, r1_2 = 0, r2_2 = 0, r3 = 0, ignored = 0, record = 0, trigger_scroll = 0;

	if (type_spawn_stage != stage)
	{
		start_spawns_by_type();
		*p_r1 = r1;
		*p_r2 = r2;
		*p_first = first;
		*p_projected_x = projected_x;
		*p_projected_y = projected_y;
		*p_object_type = object_type;
		*p_r11 = r11;
		return;
	}
	record = type_spawn_cursor[0];
	r0 = scroll_position;
	trigger_scroll = ROM16(record);
	record += 2u;
	if (trigger_scroll == 0xFFFFu || r0 <= trigger_scroll)
	{
		count_active_players(&r1_2, &r2_2);
		*p_r1 = r1_2;
		*p_r2 = r2_2;
		return;
	}
	projected_x = ROM16(record);
	record += 2u;
	projected_y = (int16_t)ROM16(record);
	record += 2u;
	spawn_z = (uint16_t)(int16_t)ROM16(record);
	record += 2u;
	if ((int32_t)trigger_scroll <= (int32_t)r0)
	{
		object_type = 0x100000u + ROM16(record) * 8u;
		spawn_count = ROM32(object_type);
		record += 2u;
		object_type += 4u;
		spawn_object_with_pieces(0x08DCB1u, &r0, &trigger_scroll, &r2, &first, &r11);
		if ((uint8_t)r0 == 0u)
		{
			type_spawn_cursor[0] += 0xEu;
			count_active_players(&r1_2, &r2_2);
			*p_r1 = r1_2;
			*p_r2 = r2_2;
			*p_first = first;
			*p_projected_x = projected_x;
			*p_projected_y = projected_y;
			*p_object_type = object_type;
			*p_r11 = r11;
			return;
		}
		current_object = first;
		projected_y = projected_y << 16;
		OBJECT(current_object)->projected_x = projected_x;
		OBJECT(current_object)->projected_y = projected_y;
		OBJECT(current_object)->z = (uint32_t)spawn_z << 16;
		OBJECT(current_object)->flags = 0u;
		OBJECT(current_object)->handler = ROM32(object_type);
		OBJECT(current_object)->spawn_arg_1 = ROM16(record);
		record += 2u;
		OBJECT(current_object)->spawn_arg_2 = ROM16(record);
		type_spawn_cursor[0] = record + 2u;
		unproject_object(&ignored, &ignored, &r3, &r11);
		count_active_players(&r1_2, &r2_2);
		*p_r1 = r1_2;
		*p_r2 = r2_2;
		*p_first = r3;
		*p_projected_x = projected_x;
		*p_projected_y = projected_y;
		*p_object_type = object_type;
		*p_r11 = r11;
		return;
	}
	type_spawn_cursor[0] += 0xEu;
	count_active_players(&r1_2, &r2_2);
	*p_r1 = r1_2;
	*p_r2 = r2_2;
	*p_first = first;
	*p_projected_x = projected_x;
	*p_projected_y = projected_y;
	*p_object_type = object_type;
	*p_r11 = r11;
}

void count_active_players(uint32_t *p_player_object, uint32_t *p_player_index)
{
	uint32_t object = 0, player = 0, player_index = 0, players_left = 0;

	players_left = 3u;
	player_index = 2u;
	active_player_count = 0u;
	do
	{
		player = GAME_VARIABLES + player_block_offsets[player_index];
		object = LINKED(player, PLAYER(player)->object);
		if ((OBJECT(object)->flags & 0x400u) == 0u)
			active_player_count++;
		players_left--;
		player_index--;
	} while (players_left != 0u);
	*p_player_object = object;
	*p_player_index = player_index;
}

void spawn_object_with_pieces(uint32_t return_address, uint32_t *p_spawned, uint32_t *p_piece_count, uint32_t *p_free_count, uint32_t *p_first, uint32_t *p_block)
{
	uint32_t spawned = *p_spawned;
	uint16_t piece_count = (uint16_t)*p_piece_count;
	uint32_t free_count = *p_free_count;
	uint32_t first = *p_first;
	uint32_t block = *p_block;
	uint32_t r2 = 0, ignored = 0, pieces_to_add = 0, pool_2_free = 0;
	uint16_t free_blocks = 0, spawn_total = 0;

	if ((int16_t)spawn_count <= 0)
	{
		current_object = object_cursor;
		return;
	}
	free_objects = free_object_stack_ADDRESS;
	first_object = first_object_address;
	last_object = last_object_address;
	spawned &= ~0xFFu;
	free_count = LINKED(free_count, free_object_stack);
	if (free_count == 0u)
	{
		current_object = object_cursor;
		*p_spawned = spawned;
		*p_free_count = free_count;
		return;
	}
	piece_count = (uint16_t)(spawn_count - 1u);
	if (piece_count != 0u)
	{
		pool_2_free = LINKED(WORK_RAM, free_object_stack_2[0]);
		free_blocks = pool_2_free;
		free_count = (pool_2_free & ~0xFFFFu) | (uint16_t)(free_blocks - piece_count);
		if (free_blocks < piece_count)
		{
			current_object = object_cursor;
			*p_spawned = spawned;
			*p_piece_count = (*p_piece_count & 0xFFFF0000u) | piece_count;
			*p_free_count = free_count;
			return;
		}
	}
	allocate_object(&ignored, &block);
	init_object(block);
	OBJECT(block)->next_handler = return_address;
	OBJECT(block)->parent = object_cursor;
	OBJECT(block)->child = 0xFFFFu;
	first = block;
	spawned = 1u;
	spawn_total = spawn_count;
	pieces_to_add = (uint16_t)(spawn_total - 1u);
	if (spawn_total == 1u)
	{
		current_object = object_cursor;
		*p_spawned = spawned;
		*p_piece_count = (*p_piece_count & 0xFFFF0000u) | (pieces_to_add & 0xFFFFu);
		*p_free_count = free_count;
		*p_first = first;
		*p_block = block;
		return;
	}
	add_object_pieces(&spawned, &pieces_to_add, &r2, &first, &block);
	*p_spawned = spawned;
	*p_piece_count = (*p_piece_count & 0xFFFF0000u) | (pieces_to_add & 0xFFFFu);
	*p_free_count = r2;
	*p_first = first;
	*p_block = block;
}

void add_pieces_to_current_object(uint32_t *p_spawned, uint32_t *p_free_count, uint32_t *p_first, uint32_t *p_block)
{
	uint32_t spawned = *p_spawned;
	uint32_t first = *p_first;
	uint32_t r2 = 0, block = 0, piece_count = 0;

	block = object_cursor;
	piece_count = spawn_count;
	add_object_pieces(&spawned, &piece_count, &r2, &first, &block);
	*p_spawned = spawned;
	*p_free_count = r2;
	*p_first = first;
	*p_block = block;
}

void add_object_pieces(uint32_t *p_spawned, uint32_t *p_piece_count, uint32_t *p_free_count, uint32_t *p_first, uint32_t *p_block)
{
	uint32_t spawned = *p_spawned;
	uint16_t piece_count = (uint16_t)*p_piece_count;
	uint32_t first = *p_first;
	uint32_t block = *p_block;
	uint32_t r2 = 0, free_count = 0, ignored = 0, object = 0, pool_2_free = 0;
	uint16_t free_blocks = 0;

	spawn_first = block;
	spawn_last = block;
	spawned &= ~0xFFu;
	piece_count = (uint16_t)piece_count;
	pool_2_free = LINKED(WORK_RAM, free_object_stack_2[0]);
	free_blocks = pool_2_free;
	free_count = (pool_2_free & ~0xFFFFu) | (uint16_t)(free_blocks - piece_count);
	if (free_blocks < piece_count)
	{
		current_object = object_cursor;
		*p_spawned = spawned;
		*p_piece_count = (*p_piece_count & 0xFFFF0000u) | piece_count;
		*p_free_count = free_count;
		return;
	}
	spawn_count = piece_count;
	do
	{
		allocate_object_2(&ignored, &r2, &block);
		init_object_2(block);
		object = spawn_last;
		AS16(spawn_last) = block;
		OBJECT(object)->child = block;
		OBJECT(block)->parent = AS16(spawn_first);
		OBJECT(block)->child = 0xFFFFu;
		spawn_count--;
	} while (spawn_count != 0u);
	first = spawn_first;
	spawned = 1u;
	current_object = object_cursor;
	*p_spawned = spawned;
	*p_piece_count = (*p_piece_count & 0xFFFF0000u) | piece_count;
	*p_free_count = r2;
	*p_first = first;
	*p_block = block;
}

void spawn_objects(uint32_t return_address, uint32_t *p_spawned, uint32_t *p_first, uint32_t *p_block)
{
	uint32_t spawned = *p_spawned;
	uint32_t block = *p_block;
	uint32_t first = 0, ignored = 0;

	if ((int16_t)spawn_count <= 0)
	{
		first = spawn_first;
		current_object = object_cursor;
		*p_first = first;
		return;
	}
	free_objects = free_object_stack_ADDRESS;
	first_object = first_object_address;
	last_object = last_object_address;
	spawned = 0u;
	if ((uint16_t)free_object_stack < spawn_count)
	{
		first = spawn_first;
		current_object = object_cursor;
		*p_spawned = spawned;
		*p_first = first;
		return;
	}
	allocate_object(&ignored, &block);
	spawn_first = block;
	spawn_last = block;
	OBJECT(block)->parent = object_cursor;
	OBJECT(block)->child = 0xFFFFu;
	init_object(block);
	OBJECT(block)->next_handler = return_address;
	for (;;)
	{
		spawn_last = block;
		spawn_count--;
		spawned = 1u;
		if (spawn_count == 0u)
			break;
		allocate_object(&ignored, &block);
		init_object(block);
		OBJECT(block)->next_handler = return_address;
		mem_write16(spawn_last + 0x28u, block);
		OBJECT(block)->parent = object_cursor;
		OBJECT(block)->child = 0xFFFFu;
	}
	first = spawn_first;
	current_object = object_cursor;
	*p_spawned = spawned;
	*p_first = first;
	*p_block = block;
}

void init_object(uint32_t object)
{
	uint8_t mode = 0;

	mode = 0u;
	init_object_fields(object, mode);
}

void init_object_2(uint32_t object)
{
	uint8_t mode = 0;

	mode = 1u;
	init_object_fields(object, mode);
}

void init_object_fields(uint32_t object, uint8_t mode)
{
	OBJECT(object)->draw_next = 0u;
	OBJECT(object)->sprite_flags = 4u;
	OBJECT(object)->sprite_frame = 0u;
	OBJECT(object)->sprite_palette = 0u;
	OBJECT(object)->sprite_scale = 0x100u;
	OBJECT(object)->sprite_layer = 0u;
	OBJECT(object)->screen_y = 0;
	OBJECT(object)->screen_x = 0;
	OBJECT(object)->hitbox_size_x = 0u;
	OBJECT(object)->hitbox_size_y = 0u;
	OBJECT(object)->anchor_x = 0;
	OBJECT(object)->anchor_y = 0;
	OBJECT(object)->sprite_height = 0;
	OBJECT(object)->sprite_width = 0;
	OBJECT(object)->handler = delete_object_and_children_ADDRESS;
	OBJECT(object)->direction = 0u;
	OBJECT(object)->projected_x = 0;
	OBJECT(object)->projected_y = 0;
	OBJECT(object)->projected_y_int = 0;
	if ((uint8_t)mode != 0u)
		return;
	OBJECT(object)->flags = 0u;
	OBJECT(object)->speed_x = 0;
	OBJECT(object)->speed_y = 0;
	OBJECT(object)->speed_z = 0;
	if ((uint8_t)mode != 2u)
	{
		OBJECT(object)->z = 0;
		OBJECT(object)->animation_speed = 0u;
		OBJECT(object)->animation_timer = 0;
		OBJECT(object)->animation_frame = 0u;
		OBJECT(object)->kind = 0u;
		OBJECT(object)->animation = 0u;
		OBJECT(object)->x = 0;
		OBJECT(object)->y = 0;
		OBJECT(object)->floor_z = 0;
		OBJECT(object)->previous_floor_z = 0;
		OBJECT(object)->floor_block = 0;
		OBJECT(object)->previous_floor_block = 0;
		OBJECT(object)->sprite_offset_x = 0;
		OBJECT(object)->sprite_offset_y = 0;
		OBJECT(object)->hitbox_height = 0u;
		OBJECT(object)->hit_list_next = 0u;
		OBJECT(object)->touched_by = 0u;
		OBJECT(object)->touch_distance = 0;
		OBJECT(object)->speed = 0;
		OBJECT(object)->platform_list_next = 0u;
		OBJECT(object)->platform = 0u;
		OBJECT(object)->floor_slope = 0u;
		OBJECT(object)->floor_type = 0u;
		OBJECT(object)->wall = 0u;
		OBJECT(object)->floor_attribute = 0u;
		OBJECT(object)->hitbox_offset_x = 0;
		OBJECT(object)->hitbox_offset_y = 0;
		OBJECT(object)->health = 0;
		OBJECT(object)->var_7F = 0u;
		OBJECT(object)->var_80 = 0u;
		OBJECT(object)->var_81 = 0u;
		OBJECT(object)->var_82 = 0u;
		OBJECT(object)->var_84 = 0u;
		OBJECT(object)->var_86 = 0u;
		OBJECT(object)->var_88 = 0u;
		OBJECT(object)->var_8A = 0u;
		OBJECT(object)->var_8C = 0u;
		OBJECT(object)->var_8E = 0u;
		OBJECT(object)->var_8F = 0u;
		OBJECT(object)->var_90 = 0u;
		OBJECT(object)->spawn_arg_1 = 0u;
		OBJECT(object)->spawn_arg_2 = 0u;
		OBJECT(object)->var_96 = 0u;
		AS32(OBJECT(object)->var_98) = 0u;
		OBJECT(object)->next_handler = 0u;
	}
}

void move_object_projected(uint32_t *p_screen_y, uint32_t *p_r2, uint32_t *p_slope_height, uint32_t *p_r11)
{
	uint32_t r11 = *p_r11;
	uint32_t r2 = 0, screen_y = 0, slope_height = 0;

	OBJECT(current_object)->move_object_projected.projected_x_carry += OBJECT(current_object)->speed_x;
	OBJECT(current_object)->projected_x += OBJECT(current_object)->move_object_projected.projected_x_carry_int;
	OBJECT(current_object)->projected_y += OBJECT(current_object)->speed_y;
	OBJECT(current_object)->z += OBJECT(current_object)->speed_z;
	OBJECT(current_object)->move_object_projected.projected_x_carry_int = 0;
	unproject_object(&screen_y, &r2, &slope_height, &r11);
	*p_screen_y = screen_y;
	*p_r2 = r2;
	*p_slope_height = slope_height;
	*p_r11 = r11;
}

void unproject_object(uint32_t *p_r0, uint32_t *p_r2, uint32_t *p_slope_height, uint32_t *p_r11)
{
	uint32_t r11 = *p_r11;
	uint32_t r2 = 0, scaled_x = 0, scaled_y = 0, screen_y = 0, slope_height = 0;

	scaled_x = (uint32_t)OBJECT(current_object)->projected_x << 8;
	scaled_y = (uint32_t)((int32_t)OBJECT(current_object)->projected_y >> 16) << 9;
	r2 = scaled_x;
	OBJECT(current_object)->x = (scaled_x - scaled_y) * 0xB5u;
	OBJECT(current_object)->y = (scaled_y + r2) * 0xB5u;
	find_floor(&scaled_x, &scaled_y, &r2, &slope_height, &r11);
	screen_y = update_screen_position_no_offset();
	*p_r0 = screen_y;
	*p_r2 = r2;
	*p_slope_height = slope_height;
	*p_r11 = r11;
}

uint32_t update_screen_position_no_offset(void)
{
	uint32_t unscrolled_y = 0;

	OBJECT(current_object)->screen_x = OBJECT(current_object)->projected_x - view_x;
	unscrolled_y = (int32_t)((uint32_t)OBJECT(current_object)->projected_y + OBJECT(current_object)->z) >> 16;
	unscrolled_y = (unscrolled_y & ~0xFFFFu) | (uint16_t)(unscrolled_y - view_y);
	OBJECT(current_object)->screen_y = unscrolled_y;
	return unscrolled_y;
}

void move_object_projected_no_floor(void)
{
	OBJECT(current_object)->move_object_projected.projected_x_carry += OBJECT(current_object)->speed_x;
	OBJECT(current_object)->projected_x += OBJECT(current_object)->move_object_projected.projected_x_carry_int;
	OBJECT(current_object)->projected_y += OBJECT(current_object)->speed_y;
	OBJECT(current_object)->z += OBJECT(current_object)->speed_z;
	OBJECT(current_object)->move_object_projected.projected_x_carry_int = 0;
	OBJECT(current_object)->screen_x = OBJECT(current_object)->projected_x - view_x;
	OBJECT(current_object)->screen_y = ((int32_t)((uint32_t)OBJECT(current_object)->projected_y + OBJECT(current_object)->z) >> 16) - view_y;
}

uint32_t is_on_screen(void)
{
	uint16_t screen_x = 0, screen_y = 0;
	uint32_t on_screen = 0;

	screen_x = (uint16_t)OBJECT(current_object)->screen_x;
	screen_y = (uint16_t)OBJECT(current_object)->screen_y;
	on_screen = 0u;
	if ((int16_t)screen_x >= -0xC0 && (int16_t)screen_x <= 0x200 && (int16_t)screen_y >= -0xC0 && (int16_t)screen_y <= 0x1A0)
		on_screen = 1u;
	return on_screen;
}

void delete_if_off_screen(uint32_t *p_deleted, uint32_t *p_r2, uint32_t *p_r3)
{
	uint32_t r2 = *p_r2;
	uint32_t r3 = *p_r3;
	uint16_t screen_x = 0, screen_y = 0;
	uint32_t r0 = 0, deleted = 0;

	screen_x = (uint16_t)OBJECT(current_object)->screen_x;
	screen_y = (uint16_t)OBJECT(current_object)->screen_y;
	if ((int16_t)screen_x < -0xC0 || (int16_t)screen_x > 0x200 || (int16_t)screen_y < -0xC0 || (int16_t)screen_y > 0x1A0)
	{
		delete_object_and_children(&r0, &r2, &r3);
		*p_deleted = r0;
		*p_r2 = r2;
		*p_r3 = r3;
		return;
	}
	deleted = 0u;
	*p_deleted = deleted;
}

void delete_object_and_children(uint32_t *p_deleted, uint32_t *p_r2, uint32_t *p_r3)
{
	uint32_t r2 = *p_r2;
	uint32_t r3 = *p_r3;
	uint32_t r2_2 = 0, r3_2 = 0, child = 0, deleted = 0, free_blocks = 0, ignored = 0, object = 0;

	if (OBJECT(object_cursor)->child == 0xFFFFu)
	{
		delete_current_object();
		deleted = 1u;
		*p_deleted = deleted;
		return;
	}
	child = LINKED(WORK_RAM, OBJECT(object_cursor)->child);
	deleted_child = child;
	for (;;)
	{
		if ((int16_t)child < 0x5000)
		{
			delete_object(child, &ignored, &r2_2);
		delete_object_and_children_next_child:
			object = LINKED(WORK_RAM, deleted_child);
			child = LINKED(object, OBJECT(object)->child);
			if ((uint16_t)child == 0xFFFFu)
			{
				delete_current_object();
				deleted = 1u;
				*p_deleted = deleted;
				*p_r2 = r2_2;
				return;
			}
			deleted_child = child;
			continue;
		}
		r2_2 = free_object_2((uint16_t)child);
		free_blocks = LINKED(WORK_RAM, free_object_stack_2[0]);
		if ((int16_t)free_blocks <= 0x100)
			goto delete_object_and_children_next_child;
		break;
	}
	int_default(&free_blocks, &r2_2, &r3_2, &ignored);
	*p_deleted = free_blocks;
	*p_r2 = r2_2;
	*p_r3 = r3_2;
}

void copy_position_to(uint32_t object)
{
	OBJECT(object)->x = OBJECT(object_cursor)->x;
	OBJECT(object)->y = OBJECT(object_cursor)->y;
	OBJECT(object)->projected_x = OBJECT(object_cursor)->projected_x;
	OBJECT(object)->projected_y = OBJECT(object_cursor)->projected_y;
	OBJECT(object)->z = OBJECT(object_cursor)->z;
	OBJECT(object)->floor_z = OBJECT(object_cursor)->floor_z;
}
