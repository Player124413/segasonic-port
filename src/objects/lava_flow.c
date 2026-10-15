/**
 *    Name:        objects/lava_flow.c
 *    Description: stage 8's end: what the players ride and the fire under the hanging bridge (game mode 0x1C), the lava flow
 *    Version:     0.1.0
 *    Created:     12-10-2026
 *    Modified:    15-10-2026
 *    Author:      Sebastian Toyos
 *
 *    SegaSonic The Hedgehog (arcade, Sega System 32): the game's V60 program decompiled into C by
 *    tools/decompile.py, with the names of symbols.txt.
 */
#include "game.h"

void ridden_object_start(void)
{
	OBJECT(current_object)->flags = 2u;
	OBJECT(current_object)->hitbox_size_x = 0x10u;
	OBJECT(current_object)->hitbox_size_y = 0x10u;
	OBJECT(current_object)->handler = ridden_object_fall_ADDRESS;
	scene_object = current_object;
	ridden_object_fall();
}

void ridden_object_fall(void)
{
	if ((OBJECT(current_object)->flags & 8u) != 0u)
		OBJECT(current_object)->handler = ridden_object_follow_ADDRESS;
	add_to_rider_list();
}

void ridden_object_follow(void)
{
	uint32_t object = 0;

	if (scroll_position < 0x8D00u)
	{
		object = LINKED(current_object, OBJECT(current_object)->platform);
		OBJECT(current_object)->x = OBJECT(object)->x;
		OBJECT(current_object)->z = OBJECT(object)->z;
		return;
	}
	delete_current_object();
}

void bridge_fire_start(void)
{
	uint32_t block_offset = 0, ignored = 0, object = 0, x = 0, y = 0;
	uint16_t flames_left_before = 0;

	OBJECT(current_object)->flags = 2u;
	OBJECT(current_object)->hitbox_size_x = 0x40u;
	OBJECT(current_object)->hitbox_size_y = 0x80u;
	OBJECT(current_object)->hitbox_height = 0x70u;
	OBJECT(current_object)->hitbox_offset_x = -0x20;
	OBJECT(current_object)->hitbox_offset_y = -0x10;
	OBJECT(current_object)->kind = 0x13u;
	OBJECT(current_object)->bridge_fire.state = 2u;
	OBJECT(current_object)->bridge_fire.off_screen = 0u;
	OBJECT(current_object)->bridge_fire.sound_timer = 0u;
	OBJECT(current_object)->z = 0x8680000;
	OBJECT(current_object)->floor_z = OBJECT(current_object)->z;
	OBJECT(current_object)->player = 0u;
	rising_fire_object = current_object;
	OBJECT(current_object)->bridge_fire.path = bridge_fire_script_ADDRESS;
	OBJECT(current_object)->bridge_fire.holding_back = 0u;
	OBJECT(current_object)->bridge_fire.wait_timer = 0;
	find_floor(&x, &y, &block_offset, &ignored, &object);
	project_object(&ignored, &ignored);
	OBJECT(current_object)->bridge_fire.projected_x_whole = OBJECT(current_object)->projected_x;
	OBJECT(current_object)->spawn_arg_2 = 0u;
	bridge_flames_left = 3;
	if (free_object_stack < bridge_flames_left)
		return;
	bridge_flame_offset_y = 0u;
	do
	{
		allocate_object(&ignored, &object);
		OBJECT(object)->rising_flame.state = 0u;
		OBJECT(object)->handler = rising_flame_update_ADDRESS;
		OBJECT(object)->x = OBJECT(current_object)->x;
		OBJECT(object)->y = OBJECT(current_object)->y;
		bridge_flame_offset_y += 0x200000u;
		OBJECT(object)->y += bridge_flame_offset_y;
		OBJECT(object)->z = OBJECT(current_object)->z;
		OBJECT(object)->z_int -= OBJECT(current_object)->hitbox_height;
		flames_left_before = bridge_flames_left;
		bridge_flames_left = flames_left_before - 1u;
	} while ((int16_t)flames_left_before > 1);
	OBJECT(current_object)->handler = bridge_fire_rise_ADDRESS;
}

void bridge_fire_rise(void)
{
	uint32_t climb_speed = 0, path_cursor = 0, point_x = 0, point_y = 0, projected_dx = 0, sound_id = 0, write_index_address = 0;
	uint8_t hold_timer = 0, sound_timer = 0;
	uint16_t record_height = 0, wait_timer = 0;
	uint32_t flags = 0;

	flags = OBJECT(current_object)->flags;
	OBJECT(current_object)->flags = flags | 0x80000000u;
	if ((flags & 0x80000000u) == 0u)
	{
	bridge_fire_read_script:
		path_cursor = OBJECT(current_object)->bridge_fire.path;
		record_height = mem_read16(path_cursor);
		path_cursor += 2u;
		if ((int16_t)(OBJECT(current_object)->z_int - OBJECT(current_object)->hitbox_height) <= (int16_t)record_height || (mem_read16(path_cursor) != 0u && (uint16_t)(mem_read16(path_cursor) - OBJECT(current_object)->hitbox_height) <= scroll_position))
		{
			path_cursor += 2u;
			if ((int16_t)mem_read16(path_cursor + 2u) < 0)
			{
				delete_current_object();
				return;
			}
			if ((int16_t)mem_read16(path_cursor) < 0)
				OBJECT(current_object)->bridge_fire.state = 7u;
			OBJECT(current_object)->bridge_fire.screen_threshold = mem_read16(path_cursor);
			path_cursor += 2u;
			OBJECT(current_object)->bridge_fire.catch_up_factor = mem_read16(path_cursor);
			path_cursor += 2u;
			OBJECT(current_object)->bridge_fire.base_speed = mem_read16(path_cursor);
			path_cursor += 2u;
			OBJECT(current_object)->bridge_fire.wait_timer = mem_read16(path_cursor);
			OBJECT(current_object)->bridge_fire.path = path_cursor + 2u;
		}
	}
	else if (OBJECT(current_object)->bridge_fire.holding_back == 0u)
	{
		if (no_player_in_play == 0u && player_mishap == 0u && player_dropping_in == 0u)
			goto bridge_fire_read_script;
		OBJECT(current_object)->bridge_fire.holding_back = 0xFFu;
		OBJECT(current_object)->bridge_fire.saved_base_speed = OBJECT(current_object)->bridge_fire.base_speed;
		OBJECT(current_object)->bridge_fire.base_speed = 0;
		OBJECT(current_object)->bridge_fire.hold_start_position = scroll_position;
		OBJECT(current_object)->bridge_fire.hold_timer = 0xFFu;
		OBJECT(current_object)->bridge_fire.saved_threshold = OBJECT(current_object)->bridge_fire.screen_threshold;
		OBJECT(current_object)->bridge_fire.screen_threshold = 0xE0;
	}
	if (OBJECT(current_object)->bridge_fire.holding_back != 0u)
	{
		if ((int16_t)(scroll_position - OBJECT(current_object)->bridge_fire.hold_start_position) >= 0x300)
			goto bridge_fire_stop_holding_back;
		if (no_player_in_play != 0u)
			OBJECT(current_object)->bridge_fire.hold_timer = 0xFFu;
		hold_timer = OBJECT(current_object)->bridge_fire.hold_timer;
		OBJECT(current_object)->bridge_fire.hold_timer = hold_timer - 1u;
		if (hold_timer == 0u)
			goto bridge_fire_stop_holding_back;
	}
	if ((int16_t)(OBJECT(current_object)->screen_y - OBJECT(current_object)->hitbox_height) <= OBJECT(current_object)->bridge_fire.screen_threshold)
	{
	bridge_fire_use_base_speed:
		climb_speed = (uint16_t)OBJECT(current_object)->bridge_fire.base_speed;
	}
	else
	{
		if (OBJECT(current_object)->bridge_fire.holding_back != 0u)
		{
		bridge_fire_stop_holding_back:
			OBJECT(current_object)->bridge_fire.holding_back = 0u;
			OBJECT(current_object)->bridge_fire.screen_threshold = OBJECT(current_object)->bridge_fire.saved_threshold;
			OBJECT(current_object)->bridge_fire.base_speed = OBJECT(current_object)->bridge_fire.saved_base_speed;
		}
		compute_camera_ground_speed(0u);
		climb_speed = (uint16_t)((uint32_t)camera_ground_speed_int * OBJECT(current_object)->bridge_fire.catch_up_factor) >> 8;
		if ((int16_t)climb_speed <= OBJECT(current_object)->bridge_fire.base_speed)
			goto bridge_fire_use_base_speed;
	}
	OBJECT(current_object)->speed_fraction = climb_speed;
	wait_timer = OBJECT(current_object)->bridge_fire.wait_timer;
	OBJECT(current_object)->bridge_fire.wait_timer = wait_timer - 1u;
	if ((int16_t)wait_timer <= 1)
	{
		OBJECT(current_object)->bridge_fire.wait_timer = 0;
		OBJECT(current_object)->z -= (uint32_t)(int16_t)OBJECT(current_object)->speed_fraction << 12;
	}
	projected_dx = (int32_t)((uint32_t)(int16_t)ROM16(floor_attribute_slopes_ADDRESS + OBJECT(current_object)->floor_attribute) * OBJECT(current_object)->speed_fraction) >> 4;
	OBJECT(current_object)->bridge_fire.fixed_projected_x += projected_dx;
	OBJECT(current_object)->projected_x = OBJECT(current_object)->bridge_fire.projected_x_whole;
	OBJECT(current_object)->projected_y -= (int32_t)projected_dx >> 1;
	update_screen_position(&sound_id, &climb_speed);
	if (OBJECT(current_object)->screen_y > 0x160)
		OBJECT(current_object)->bridge_fire.off_screen = 0xFFu;
	else
	{
		if (OBJECT(current_object)->bridge_fire.off_screen != 0u)
		{
			OBJECT(current_object)->bridge_fire.off_screen = 0u;
			OBJECT(current_object)->bridge_fire.sound_timer = 0u;
		}
		if (current_game_mode != 7u)
		{
			sound_timer = OBJECT(current_object)->bridge_fire.sound_timer;
			OBJECT(current_object)->bridge_fire.sound_timer = sound_timer - 1u;
			if ((int8_t)sound_timer <= 1)
			{
				OBJECT(current_object)->bridge_fire.sound_timer = 0x3Cu;
				sound_id = 9u;
				play_sound((uint8_t)sound_id, &climb_speed, &write_index_address);
			}
		}
	}
	point_x = OBJECT(current_object)->bridge_fire.fixed_projected_x;
	point_y = OBJECT(current_object)->projected_y * 2u;
	unproject_point(&point_x, &point_y);
	OBJECT(current_object)->x = point_x;
	OBJECT(current_object)->y = point_y;
	if (OBJECT(current_object)->player == 0u && (int16_t)(OBJECT(current_object)->z_int - OBJECT(current_object)->hitbox_height) <= 0x534)
	{
		stage_trigger_bits = 0xFFFFFFFFu;
		OBJECT(current_object)->player = 0xFFu;
	}
	sound_id = add_to_touchable_list(sound_id);
}

void lava_flow_start(void)
{
	uint32_t free_count = 0, ignored = 0, part_list = 0;

	allocate_object_2(&free_count, &ignored, &part_list);
	if (free_count != 0u)
	{
		move_halfwords_backward(part_list, lava_flow_parts_ADDRESS, 9u);
		OBJECT(current_object)->part_list = part_list;
		OBJECT(current_object)->sprite_flags = 0xD008u;
		OBJECT(current_object)->flags = 2u;
		OBJECT(current_object)->parent = current_object;
		OBJECT(current_object)->child = 0xFFFFu;
		OBJECT(current_object)->lava_flow.path = lava_flow_script_ADDRESS;
		OBJECT(current_object)->lava_flow.holding_back = 0u;
		OBJECT(current_object)->lava_flow.wait_timer = 0x78;
		OBJECT(current_object)->spawn_arg_2 = 2u;
		OBJECT(current_object)->lava_flow.unused_word = 0u;
		OBJECT(current_object)->lava_flow.sound_timer = 0xFFu;
		OBJECT(current_object)->handler = lava_flow_wait_ADDRESS;
		metal_platform_lifted = 0u;
		OBJECT(current_object)->sprite_offset_x = 0;
		OBJECT(current_object)->sprite_offset_y = 0;
		OBJECT(current_object)->hitbox_offset_x = -0x6A;
		OBJECT(current_object)->hitbox_offset_y = 0;
		OBJECT(current_object)->hitbox_size_x = 0x60u;
		OBJECT(current_object)->hitbox_size_y = 0xE7u;
		OBJECT(current_object)->hitbox_height = 0x50u;
		OBJECT(current_object)->kind = 0x14u;
		OBJECT(current_object)->direction = 0x100u;
		OBJECT(current_object)->animation = lava_flow_animation_ADDRESS;
		OBJECT(current_object)->animation_frame = 0u;
		OBJECT(current_object)->animation_speed = 6u;
		OBJECT(current_object)->animation_timer = 0;
		OBJECT(current_object)->z = 0x5300000;
	}
}

void lava_flow_wait(void)
{
	uint16_t wait_timer = 0;

	if (metal_platform_lifted != 0u)
	{
		wait_timer = OBJECT(current_object)->lava_flow.wait_timer;
		OBJECT(current_object)->lava_flow.wait_timer = wait_timer - 1u;
		if ((int16_t)wait_timer <= 1)
		{
			OBJECT(current_object)->handler = lava_flow_advance_ADDRESS;
			spawn_lava_steam();
		}
	}
}

void lava_flow_advance(void)
{
	uint16_t part_list = 0;
	uint32_t r0 = 0, r3 = 0, r8 = 0, r11 = 0, depth = 0, flow_speed = 0, ignored = 0, path_cursor = 0, write_index_address = 0;
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
				play_sound((uint8_t)r0, &flow_speed, &write_index_address);
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
