/**
 *    Name:        game_06.c
 *    Description: the game's functions, part 06 (in the order of their V60 addresses)
 *    Version:     0.1.0
 *    Created:     12-10-2026
 *    Modified:    13-10-2026
 *    Author:      Sebastian Toyos
 *
 *    SegaSonic The Hedgehog (arcade, Sega System 32): the game's V60 program decompiled into C by
 *    tools/decompile.py, with the names of symbols.txt.
 */
#include "game.h"

void fireball_emitter_start(void)
{
	uint32_t r0 = 0, block_offset = 0, ignored = 0, list_cursor = 0, y = 0;

	list_cursor = fireball_emitter_lists[OBJECT(current_object)->sprite_frame] + fireball_emitter_lists_ADDRESS;
	OBJECT(current_object)->x_int = ROM16(list_cursor);
	OBJECT(current_object)->x_fraction = 0u;
	list_cursor += 2u;
	OBJECT(current_object)->y_int = ROM16(list_cursor);
	OBJECT(current_object)->y_fraction = 0u;
	list_cursor += 2u;
	OBJECT(current_object)->floor_z_int = ROM16(list_cursor);
	OBJECT(current_object)->floor_z_fraction = 0u;
	OBJECT(current_object)->animation = list_cursor + 2u;
	OBJECT(current_object)->flags = 0u;
	find_floor(&r0, &y, &block_offset, &ignored, &list_cursor);
	OBJECT(current_object)->handler = fireball_emitter_update_ADDRESS;
}

void fireball_emitter_update(void)
{
	uint16_t scale = 0;
	uint32_t floor_slope = 0, ignored = 0, list_record = 0, object = 0, slope_direction = 0, slope_factor = 0, slope_speed = 0, type_record = 0;

	project_object(&slope_direction, &floor_slope);
	if (OBJECT(current_object)->screen_y >= -0x40)
	{
		slope_factor = 0u;
		if ((OBJECT(current_object)->floor_slope | OBJECT(current_object)->floor_type) != 0u)
		{
			floor_slope = OBJECT(current_object)->floor_slope;
			slope_factor = floor_slope_factors[(uint16_t)(floor_slope * 2u)];
		}
		compute_camera_slope_speed(0u, slope_factor);
		slope_direction = camera_slope_direction;
		slope_speed = camera_slope_speed >> 14;
		OBJECT(current_object)->x += (int32_t)(sine_table[slope_direction] * slope_speed) >> 7;
		OBJECT(current_object)->y -= (int32_t)(cosine_table[slope_direction] * slope_speed) >> 7;
	}
	find_floor(&slope_direction, &floor_slope, &slope_speed, &ignored, &object);
	OBJECT(current_object)->z = OBJECT(current_object)->floor_z;
	if (mem_read16(OBJECT(current_object)->animation) <= scroll_position)
	{
		if (free_object_stack <= 0 || free_object_stack_2[0] <= 0)
			return;
		allocate_object(&ignored, &object);
		OBJECT(object)->handler = bouncing_fireball_update_ADDRESS;
		OBJECT(object)->flags = 0u;
		OBJECT(object)->hitbox_size_y = 0u;
		OBJECT(object)->kind = 0x17u;
		OBJECT(object)->z = OBJECT(current_object)->floor_z;
		OBJECT(object)->y = OBJECT(current_object)->y;
		list_record = OBJECT(current_object)->animation + 2u;
		OBJECT(object)->x_int = mem_read16(list_record);
		OBJECT(object)->x_fraction = 0u;
		list_record += 2u;
		OBJECT(object)->y_int += mem_read16(list_record);
		list_record += 2u;
		OBJECT(object)->speed_y = (uint32_t)(int16_t)mem_read16(list_record) << 8;
		OBJECT(object)->speed_x = 0;
		list_record += 2u;
		OBJECT(object)->speed_z = (uint32_t)(int16_t)mem_read16(list_record) << 8;
		type_record = mem_read16(list_record + 2u) + fireball_types_ADDRESS;
		OBJECT(object)->sprite_flags = ROM16(type_record);
		type_record += 2u;
		OBJECT(object)->sprite_palette = ROM16(type_record);
		type_record += 2u;
		OBJECT(object)->sprite_scale = ROM16(type_record);
		type_record += 2u;
		OBJECT(object)->hitbox_height = ROM16(type_record);
		type_record += 2u;
		OBJECT(object)->hitbox_size_x = ROM16(type_record);
		type_record += 2u;
		OBJECT(object)->sprite_offset_x = ROM8(type_record);
		type_record++;
		OBJECT(object)->sprite_offset_y = ROM8(type_record);
		type_record++;
		AS8(OBJECT(object)->hitbox_size_y) = ROM8(type_record);
		type_record++;
		AT8(OBJECT(object)->hitbox_size_y, 1) = ROM8(type_record);
		OBJECT(object)->animation = ROM16(type_record + 1u) + 0x83329u;
		OBJECT(object)->animation_frame = 0u;
		OBJECT(object)->animation_timer = 0;
		OBJECT(object)->animation_speed = 4u;
		current_object = object;
		scale = 0x180u;
		if (OBJECT(current_object)->hitbox_size_x == 0u)
			scale = 0x30u;
		create_child_shadow(scale, object);
		current_object = object_cursor;
		OBJECT(current_object)->animation += 0xCu;
		if ((int16_t)mem_read16(OBJECT(current_object)->animation + 0xAu) < 0)
		{
			delete_current_object();
			return;
		}
	}
	add_to_rider_list();
}

void bouncing_fireball_update(void)
{
	uint8_t sound_id = 0;
	uint16_t child = 0, shadow_dx = 0, shadow_dy = 0;
	uint32_t r0 = 0, r1 = 0, r2 = 0, r7 = 0, cursor = 0, depth = 0, ignored = 0, off_screen = 0;
	uint8_t blink_phase = 0;

	OBJECT(current_object)->x += OBJECT(current_object)->speed_x;
	OBJECT(current_object)->y += OBJECT(current_object)->speed_y;
	find_floor(&r0, &r1, &r2, &ignored, &cursor);
	OBJECT(current_object)->speed_z += 0x6000u;
	OBJECT(current_object)->z += OBJECT(current_object)->speed_z;
	if (OBJECT(current_object)->floor_z <= OBJECT(current_object)->z)
	{
		OBJECT(current_object)->speed_z = 0u - OBJECT(current_object)->speed_z;
		OBJECT(current_object)->speed_z = (int32_t)OBJECT(current_object)->speed_z >> 1;
		OBJECT(current_object)->z = OBJECT(current_object)->floor_z;
		if (OBJECT(current_object)->speed_z_int <= -2)
		{
			sound_id = 0x21u;
			play_sound(sound_id, &r1, &r2);
		}
	}
	project_object(&ignored, &r1);
	off_screen = check_left_screen();
	if (off_screen == 0u)
	{
		animate(&r1, &r7, &off_screen);
		if (OBJECT(current_object)->hitbox_size_x != 0u)
			add_to_touchable_list(ignored);
		OBJECT(current_object)->flags &= ~0x10u;
		if ((OBJECT(current_object)->flags & 8u) != 0u)
			OBJECT(current_object)->flags |= 0x10u;
		add_to_rider_list();
		queue_sprite(r1);
		shadow_dx = 8u;
		shadow_dy = 0xFFFDu;
		if (OBJECT(current_object)->hitbox_size_x == 0u)
		{
			shadow_dx = 0u;
			shadow_dy = 0xFFFDu;
		}
		if (OBJECT(current_object)->floor_block < 0 || (int16_t)OBJECT(current_object)->child < 0)
			return;
		current_object = LINKED(current_object, OBJECT(object_cursor)->child);
		OBJECT(current_object)->screen_x = OBJECT(object_cursor)->projected_x - view_x + shadow_dx;
		OBJECT(current_object)->screen_y = ((int32_t)((uint32_t)OBJECT(object_cursor)->projected_y + OBJECT(object_cursor)->floor_z) >> 16) - view_y + shadow_dy;
		OBJECT(current_object)->sprite_palette = 0x9FFEu;
		if ((OBJECT(object_cursor)->flags & 0x10u) == 0u)
		{
			depth = (uint16_t)(OBJECT(current_object)->screen_y - 0x10u);
			queue_sprite_at_depth(&ignored, &depth);
			current_object = (current_object & ~0xFFFFu) | (uint16_t)object_cursor;
		}
		else
		{
			OBJECT(current_object)->sprite_palette = 0x87A0u;
			blink_phase = ~AS8(OBJECT(current_object)->hitbox_size_x);
			AS8(OBJECT(current_object)->hitbox_size_x) = blink_phase;
			if (blink_phase == 0u)
			{
				depth = (uint16_t)(OBJECT(current_object)->screen_y - 0x10u);
				queue_sprite_at_depth(&ignored, &depth);
				current_object = (current_object & ~0xFFFFu) | (uint16_t)object_cursor;
				return;
			}
		}
		return;
	}
	child = OBJECT(current_object)->child;
	if ((int16_t)child >= 0)
		free_object_2(child);
	delete_current_object();
}

uint32_t spawn_chaser_back_zone(uint32_t zone_index, uint32_t zone)
{
	uint32_t free_count = 0, step_record = 0;

	chaser_zone_index = zone_index;
	allocate_object(&free_count, &zone);
	if (free_count != 0u)
	{
		OBJECT(zone)->kind = 9u;
		OBJECT(zone)->flags = 0x40000002u;
		step_record = object_steps_ADDRESS + chaser_zone_index * 3u * 2u;
		AS16(OBJECT(zone)->var_80) = mem_read16(step_record);
		OBJECT(zone)->handler = mem_read32(step_record + 2u);
		OBJECT(zone)->parent = current_object;
	}
	return zone;
}

void chaser_back_zone_check_parent(void)
{
	OBJECT(current_object)->kind = 0x14u;
	if (AS8(OBJECT(LINKED(current_object, OBJECT(current_object)->parent))->var_96) == 3u)
	{
		delete_current_object();
		return;
	}
	chaser_back_zone_update();
}

void chaser_back_zone_update(void)
{
	uint32_t direction = 0, object = 0;

	if (OBJECT(current_object)->chaser_back_zone.end_position <= scroll_position)
	{
		delete_current_object();
		return;
	}
	object = LINKED(current_object, OBJECT(current_object)->parent);
	OBJECT(current_object)->hitbox_size_x = OBJECT(object)->hitbox_size_x;
	OBJECT(current_object)->hitbox_size_y = OBJECT(object)->hitbox_size_y;
	OBJECT(current_object)->x = OBJECT(object)->x;
	OBJECT(current_object)->y = OBJECT(object)->y;
	OBJECT(current_object)->hitbox_offset_x = OBJECT(object)->hitbox_offset_x;
	OBJECT(current_object)->hitbox_offset_y = OBJECT(object)->hitbox_offset_y;
	OBJECT(current_object)->z = OBJECT(object)->z;
	OBJECT(current_object)->direction = OBJECT(object)->direction;
	OBJECT(current_object)->hitbox_height = OBJECT(object)->hitbox_height;
	direction = OBJECT(object)->direction;
	switch (direction >> 8)
	{
	case 0:
	case 4:
		OBJECT(current_object)->y_int += OBJECT(current_object)->hitbox_offset_y;
		OBJECT(current_object)->y_int += OBJECT(current_object)->hitbox_size_y;
		OBJECT(current_object)->hitbox_size_y = 0x200u;
		OBJECT(current_object)->hitbox_offset_y = 0;
		break;
	case 1:
		OBJECT(current_object)->x_int += OBJECT(current_object)->hitbox_offset_x;
		OBJECT(current_object)->hitbox_size_x = 0x200u;
		OBJECT(current_object)->x_int -= OBJECT(current_object)->hitbox_size_x;
		OBJECT(current_object)->hitbox_offset_x = 0;
		break;
	case 2:
		OBJECT(current_object)->y_int += OBJECT(current_object)->hitbox_offset_y;
		OBJECT(current_object)->hitbox_size_y = 0x200u;
		OBJECT(current_object)->y_int -= OBJECT(current_object)->hitbox_size_y;
		OBJECT(current_object)->hitbox_offset_y = 0;
		break;
	case 3:
		OBJECT(current_object)->x_int += OBJECT(current_object)->hitbox_offset_x;
		OBJECT(current_object)->x_int += OBJECT(current_object)->hitbox_size_x;
		OBJECT(current_object)->hitbox_size_x = 0x200u;
		OBJECT(current_object)->hitbox_offset_x = 0;
		break;
	default:
		unknown_code(0x82F30u + (direction >> 8) * 2u);
	}
	direction = add_to_zone_list(direction);
}

void spawn_roller_back_zone(uint32_t zone_index, uint32_t zone)
{
	uint32_t free_count = 0;

	roller_zone_index = zone_index;
	allocate_object(&free_count, &zone);
	if (free_count != 0u)
	{
		OBJECT(zone)->kind = 0x18u;
		OBJECT(zone)->flags = 0x40000002u;
		OBJECT(zone)->roller_back_zone.limit = object_steps[roller_zone_index * 3u];
		OBJECT(zone)->handler = roller_back_zone_update_ADDRESS;
		OBJECT(zone)->parent = object_cursor;
	}
}

void roller_back_zone_update(void)
{
	uint32_t direction = 0, object = 0;

	object = LINKED(current_object, OBJECT(current_object)->parent);
	OBJECT(current_object)->hitbox_size_x = OBJECT(object)->hitbox_size_x;
	OBJECT(current_object)->hitbox_size_y = OBJECT(object)->hitbox_size_y;
	OBJECT(current_object)->x = OBJECT(object)->x;
	OBJECT(current_object)->y = OBJECT(object)->y;
	OBJECT(current_object)->hitbox_offset_x = OBJECT(object)->hitbox_offset_x;
	OBJECT(current_object)->hitbox_offset_y = OBJECT(object)->hitbox_offset_y;
	OBJECT(current_object)->z = OBJECT(object)->z;
	OBJECT(current_object)->direction = OBJECT(object)->direction;
	OBJECT(current_object)->hitbox_height = OBJECT(object)->hitbox_height;
	direction = OBJECT(object)->direction;
	switch (direction >> 8)
	{
	case 0:
	case 4:
		if (OBJECT(current_object)->y_int <= OBJECT(current_object)->roller_back_zone.limit)
		{
			delete_current_object();
			return;
		}
		OBJECT(current_object)->y_int += OBJECT(current_object)->hitbox_size_y;
		OBJECT(current_object)->hitbox_size_y = 0x200u;
		OBJECT(current_object)->hitbox_size_x = 0x1A2u;
		break;
	case 1:
		if (OBJECT(current_object)->roller_back_zone.limit <= OBJECT(current_object)->x_int)
		{
			delete_current_object();
			return;
		}
		OBJECT(current_object)->x_int += OBJECT(current_object)->hitbox_offset_x;
		OBJECT(current_object)->hitbox_size_x = 0x200u;
		OBJECT(current_object)->x_int -= OBJECT(current_object)->hitbox_size_x;
		OBJECT(current_object)->y_int -= 0xB0u;
		OBJECT(current_object)->hitbox_size_y = 0x1A2u;
		OBJECT(current_object)->hitbox_offset_x = 0;
		break;
	case 2:
		if (OBJECT(current_object)->roller_back_zone.limit <= OBJECT(current_object)->y_int)
		{
			delete_current_object();
			return;
		}
		OBJECT(current_object)->y_int += OBJECT(current_object)->hitbox_offset_y;
		OBJECT(current_object)->hitbox_size_y = 0x200u;
		OBJECT(current_object)->y_int -= OBJECT(current_object)->hitbox_size_y;
		OBJECT(current_object)->x_int -= 0xB0u;
		OBJECT(current_object)->hitbox_size_x = 0x1A2u;
		OBJECT(current_object)->hitbox_offset_y = 0;
		break;
	case 3:
		if (OBJECT(current_object)->x_int <= OBJECT(current_object)->roller_back_zone.limit)
		{
			delete_current_object();
			return;
		}
		OBJECT(current_object)->x_int += OBJECT(current_object)->hitbox_size_x;
		OBJECT(current_object)->hitbox_size_x = 0x200u;
		OBJECT(current_object)->hitbox_size_y = 0x1A2u;
		break;
	default:
		unknown_code(0x8303Bu + (direction >> 8) * 3u);
	}
	direction = add_to_zone_list(direction);
}

void machine_start(void)
{
	uint16_t kind = 0, x = 0, y_2 = 0, z = 0;
	uint32_t r1 = 0, block_offset = 0, free_count = 0, ignored = 0, object = 0, y = 0;

	OBJECT(current_object)->sprite_flags = 0x1006u;
	OBJECT(current_object)->sprite_palette = 0x9050u;
	OBJECT(current_object)->sprite_offset_x = 0;
	OBJECT(current_object)->sprite_offset_y = 0;
	OBJECT(current_object)->flags = 2u;
	OBJECT(current_object)->hitbox_offset_x = 0;
	OBJECT(current_object)->hitbox_offset_y = -0x10;
	OBJECT(current_object)->hitbox_size_x = 0xE8u;
	OBJECT(current_object)->hitbox_size_y = 0x20u;
	OBJECT(current_object)->hitbox_height = 0x50u;
	OBJECT(current_object)->kind = 0xCu;
	machine_wrecked = 0u;
	OBJECT(current_object)->handler = machine_run_ADDRESS;
	OBJECT(current_object)->animation = machine_animation_ADDRESS;
	OBJECT(current_object)->animation_frame = 0u;
	OBJECT(current_object)->animation_speed = 8u;
	OBJECT(current_object)->animation_timer = 0;
	find_floor(&free_count, &y, &block_offset, &ignored, &object);
	OBJECT(current_object)->z = OBJECT(current_object)->floor_z;
	project_object(&ignored, &r1);
	allocate_object(&free_count, &object);
	if (free_count != 0u)
	{
		OBJECT(object)->sprite_flags = 0x1004u;
		OBJECT(object)->sprite_palette = 0x9060u;
		OBJECT(object)->flags = 0u;
		OBJECT(object)->sprite_offset_x = 0;
		OBJECT(object)->sprite_offset_y = 0;
		OBJECT(object)->handler = machine_fire_update_ADDRESS;
		OBJECT(object)->machine_fire.state = 0u;
		OBJECT(object)->parent = current_object;
		OBJECT(object)->animation = machine_flame_animation_ADDRESS;
		OBJECT(object)->animation_frame = 0u;
		OBJECT(object)->animation_speed = 2u;
		OBJECT(object)->animation_timer = 0;
		OBJECT(object)->projected_x = OBJECT(current_object)->projected_x;
		OBJECT(object)->projected_x += 0x2Cu;
		OBJECT(object)->projected_y = OBJECT(current_object)->projected_y;
		OBJECT(object)->projected_y_int -= 0x59u;
		OBJECT(object)->z = OBJECT(current_object)->z;
		x = 0xF74Du;
		y_2 = 0xAEDu;
		kind = 5u;
		z = 0x43Eu;
		spawn_sign(x, y_2, kind, z, &r1, &object);
	}
}

void machine_run(void)
{
	uint32_t r1 = 0, r7 = 0, depth = 0, ignored = 0, off_screen = 0;

	if (machine_wrecked != 0u)
	{
		OBJECT(current_object)->animation_frame = 0xFFu;
		OBJECT(current_object)->sprite_frame = 0x68E8u;
	}
	off_screen = check_left_screen();
	if (off_screen != 0u)
	{
		delete_current_object();
		return;
	}
	update_screen_position(&ignored, &r1);
	if ((int8_t)OBJECT(current_object)->animation_frame >= 0)
		animate(&r1, &r7, &off_screen);
	add_to_touchable_list(ignored);
	depth = (uint16_t)(OBJECT(current_object)->screen_y - 0x68u);
	queue_sprite_at_depth(&ignored, &depth);
}

void machine_fire_update(void)
{
	uint8_t sound_id_2 = 0;
	uint32_t r0 = 0, r1 = 0, r2 = 0, r7 = 0, r8 = 0, climb_speed = 0, free_count = 0, ignored = 0, object = 0, path_cursor = 0, point_x = 0, point_y = 0, projected_dx = 0, random_bits = 0, sound_id = 0, spawn_record = 0;
	uint8_t hold_timer = 0, sound_timer = 0, sound_timer_2 = 0, lane_timer_before = 0, lane_before = 0, sound_timer_3 = 0;
	uint16_t spawns_left_before = 0, record_height = 0, wait_timer = 0;
	uint32_t flags = 0, lane_timer_address = 0;

	switch (OBJECT(current_object)->machine_fire.state)
	{
	case 0:
	case 8:
		if ((int8_t)OBJECT(LINKED(current_object, OBJECT(current_object)->parent))->animation_frame < 0)
			OBJECT(current_object)->machine_fire.state++;
		update_screen_position(&ignored, &r1);
		animate(&r1, &r7, &r8);
		r1 = queue_sprite(r1);
		return;
	case 1:
		OBJECT(current_object)->machine_fire.state++;
		OBJECT(current_object)->x = -0x8B40000;
		OBJECT(current_object)->y = 0xA900000;
		OBJECT(current_object)->flags = 2u;
		OBJECT(current_object)->hitbox_size_x = 0x90u;
		OBJECT(current_object)->hitbox_size_y = 0xD0u;
		OBJECT(current_object)->hitbox_height = 0x70u;
		OBJECT(current_object)->hitbox_offset_x = -0x48;
		OBJECT(current_object)->hitbox_offset_y = 0;
		OBJECT(current_object)->kind = 0x13u;
		OBJECT(current_object)->machine_fire.off_screen = 0xFFu;
		find_floor(&r0, &r1, &r2, &ignored, &object);
		OBJECT(current_object)->z = OBJECT(current_object)->floor_z;
		OBJECT(current_object)->z += (uint32_t)OBJECT(current_object)->hitbox_height << 16;
		rising_fire_object = current_object;
		OBJECT(current_object)->machine_fire.path = machine_fire_script_ADDRESS;
		OBJECT(current_object)->machine_fire.holding_back = 0u;
		project_object(&ignored, &ignored);
		OBJECT(current_object)->machine_fire.projected_x_whole = OBJECT(current_object)->projected_x;
		OBJECT(current_object)->spawn_arg_1 = 0u;
		allocate_object(&free_count, &object);
		if (free_count != 0u)
		{
			OBJECT(object)->handler = flame_start_ADDRESS;
			OBJECT(object)->x = -0x8B80000;
			OBJECT(object)->y = 0xA500000;
			spawn_record = spawns_with_next_handler_ADDRESS;
			if (free_object_stack < (int16_t)spawns_with_next_handler)
				return;
			temp_0 = ROM16(spawn_record);
			spawn_record += 2u;
			do
			{
				allocate_object(&ignored, &object);
				OBJECT(object)->handler = fireball_start_ADDRESS;
				OBJECT(object)->var_7F = 0u;
				OBJECT(object)->x_int = ROM16(spawn_record);
				OBJECT(object)->x_fraction = 0u;
				spawn_record += 2u;
				OBJECT(object)->y_int = ROM16(spawn_record);
				OBJECT(object)->y_fraction = 0u;
				spawn_record += 2u;
				OBJECT(object)->var_96 = ROM16(spawn_record);
				spawn_record += 2u;
				AS16(OBJECT(object)->player) = ROM16(spawn_record);
				spawn_record += 2u;
				OBJECT(object)->z_int = ROM16(spawn_record);
				OBJECT(object)->z_fraction = 0u;
				spawn_record += 2u;
				OBJECT(object)->next_handler = ROM32(spawn_record);
				spawns_left_before = temp_0;
				temp_0 = spawns_left_before - 1u;
				spawn_record += 4u;
			} while ((int16_t)spawns_left_before > 1);
		}
		return;
	case 2:
		flags = OBJECT(current_object)->flags;
		OBJECT(current_object)->flags = flags | 0x80000000u;
		if ((flags & 0x80000000u) == 0u)
		{
		machine_fire_read_script:
			path_cursor = OBJECT(current_object)->machine_fire.path;
			record_height = mem_read16(path_cursor);
			path_cursor += 2u;
			if ((int16_t)record_height < (int16_t)(OBJECT(current_object)->z_int - OBJECT(current_object)->hitbox_height))
			{
			machine_fire_climb_step:
				if (OBJECT(current_object)->machine_fire.holding_back != 0u)
				{
					if ((int16_t)(scroll_position - OBJECT(current_object)->machine_fire.hold_start_position) >= 0x300)
						goto machine_fire_stop_holding_back;
					if (no_player_in_play != 0u)
						OBJECT(current_object)->machine_fire.hold_timer = 0xFFu;
					hold_timer = OBJECT(current_object)->machine_fire.hold_timer;
					OBJECT(current_object)->machine_fire.hold_timer = hold_timer - 1u;
					if (hold_timer == 0u)
						goto machine_fire_stop_holding_back;
				}
				if ((int16_t)(OBJECT(current_object)->screen_y - OBJECT(current_object)->hitbox_height) <= OBJECT(current_object)->machine_fire.screen_threshold)
				{
				machine_fire_use_base_speed:
					climb_speed = (uint16_t)OBJECT(current_object)->machine_fire.base_speed;
				}
				else
				{
					if (OBJECT(current_object)->machine_fire.holding_back != 0u)
					{
					machine_fire_stop_holding_back:
						OBJECT(current_object)->machine_fire.holding_back = 0u;
						OBJECT(current_object)->machine_fire.screen_threshold = OBJECT(current_object)->machine_fire.saved_threshold;
						OBJECT(current_object)->machine_fire.base_speed = OBJECT(current_object)->machine_fire.saved_base_speed;
					}
					compute_camera_ground_speed(0u);
					climb_speed = (uint16_t)((uint32_t)camera_ground_speed_int * OBJECT(current_object)->machine_fire.catch_up_factor) >> 8;
					if ((int16_t)climb_speed <= OBJECT(current_object)->machine_fire.base_speed)
						goto machine_fire_use_base_speed;
				}
				OBJECT(current_object)->speed_fraction = climb_speed;
				wait_timer = OBJECT(current_object)->machine_fire.wait_timer;
				OBJECT(current_object)->machine_fire.wait_timer = wait_timer - 1u;
				if ((int16_t)wait_timer <= 1)
				{
					OBJECT(current_object)->machine_fire.wait_timer = 0;
					OBJECT(current_object)->z -= (uint32_t)(int16_t)OBJECT(current_object)->speed_fraction << 12;
				}
				projected_dx = (int32_t)((uint32_t)(int16_t)ROM16(floor_attribute_slopes_ADDRESS + OBJECT(current_object)->floor_attribute) * OBJECT(current_object)->speed_fraction) >> 4;
				OBJECT(current_object)->machine_fire.fixed_projected_x += projected_dx;
				OBJECT(current_object)->projected_x = OBJECT(current_object)->machine_fire.projected_x_whole;
				OBJECT(current_object)->projected_y -= (int32_t)projected_dx >> 1;
				update_screen_position(&sound_id, &climb_speed);
				if (OBJECT(current_object)->screen_y > 0x160)
					OBJECT(current_object)->machine_fire.off_screen = 0xFFu;
				else
				{
					if (OBJECT(current_object)->machine_fire.off_screen != 0u)
					{
						OBJECT(current_object)->machine_fire.off_screen = 0u;
						OBJECT(current_object)->machine_fire.sound_timer = 0;
					}
					if (current_game_mode != 7u)
					{
						sound_timer = OBJECT(current_object)->machine_fire.sound_timer;
						OBJECT(current_object)->machine_fire.sound_timer = sound_timer - 1u;
						if ((int8_t)sound_timer <= 1)
						{
							OBJECT(current_object)->machine_fire.sound_timer = 0x3C;
							sound_id = 9u;
							play_sound((uint8_t)sound_id, &climb_speed, &r2);
						}
					}
				}
				point_x = OBJECT(current_object)->machine_fire.fixed_projected_x;
				point_y = OBJECT(current_object)->projected_y * 2u;
				unproject_point(&point_x, &point_y);
				OBJECT(current_object)->x = point_x;
				OBJECT(current_object)->y = point_y;
				sound_id = add_to_touchable_list(sound_id);
				return;
			}
			if ((int16_t)mem_read16(path_cursor + 2u) >= 0)
			{
				OBJECT(current_object)->machine_fire.screen_threshold = mem_read16(path_cursor);
				path_cursor += 2u;
				OBJECT(current_object)->machine_fire.catch_up_factor = mem_read16(path_cursor);
				path_cursor += 2u;
				OBJECT(current_object)->machine_fire.base_speed = mem_read16(path_cursor);
				path_cursor += 2u;
				OBJECT(current_object)->machine_fire.wait_timer = mem_read16(path_cursor);
				OBJECT(current_object)->machine_fire.path = path_cursor + 2u;
				goto machine_fire_climb_step;
			}
			OBJECT(current_object)->machine_fire.state++;
			sound_timer_2 = OBJECT(current_object)->machine_fire.sound_timer;
			OBJECT(current_object)->machine_fire.sound_timer = sound_timer_2 - 1u;
			if ((int8_t)sound_timer_2 <= 1)
			{
				OBJECT(current_object)->machine_fire.sound_timer = 0x3C;
				sound_id_2 = 9u;
				play_sound(sound_id_2, &r1, &r2);
			}
			update_screen_position(&ignored, &r1);
			ignored = add_to_touchable_list(ignored);
			return;
		}
		if (OBJECT(current_object)->machine_fire.holding_back != 0u)
			goto machine_fire_climb_step;
		if (no_player_in_play != 0u || player_mishap != 0u || player_dropping_in != 0u)
		{
			OBJECT(current_object)->machine_fire.holding_back = 0xFFu;
			OBJECT(current_object)->machine_fire.saved_base_speed = OBJECT(current_object)->machine_fire.base_speed;
			OBJECT(current_object)->machine_fire.base_speed = 0;
			OBJECT(current_object)->machine_fire.hold_start_position = scroll_position;
			OBJECT(current_object)->machine_fire.hold_timer = 0xFFu;
			OBJECT(current_object)->machine_fire.saved_threshold = OBJECT(current_object)->machine_fire.screen_threshold;
			OBJECT(current_object)->machine_fire.screen_threshold = 0xE0;
			goto machine_fire_climb_step;
		}
		goto machine_fire_read_script;
	case 3:
		OBJECT(current_object)->machine_fire.state++;
		OBJECT(current_object)->platform_list_next = 0u;
		OBJECT(current_object)->machine_fire.move_frames = 0;
		OBJECT(current_object)->z = 0x100000;
		OBJECT(current_object)->x = -0x8520000;
		OBJECT(current_object)->y = 0xA790000;
		OBJECT(current_object)->hitbox_size_x = 0x58u;
		OBJECT(current_object)->hitbox_size_y = 0x1A2u;
		OBJECT(current_object)->hitbox_offset_x = -0x68;
		OBJECT(current_object)->hitbox_offset_y = -0x60;
		OBJECT(current_object)->kind = 0x14u;
		OBJECT(current_object)->direction = 0x100u;
	case 4:
		OBJECT(current_object)->x += 0x10000u;
		OBJECT(current_object)->machine_fire.move_frames++;
		if (OBJECT(current_object)->machine_fire.move_frames >= 0x80)
			OBJECT(current_object)->machine_fire.state++;
		find_floor(&r0, &r1, &r2, &random_bits, &object);
		fireball_lane_timer_cursor = current_object + 0x72u;
		fireball_lane = 3;
		for (;;)
		{
			lane_timer_address = fireball_lane_timer_cursor;
			lane_timer_before = mem_read8(lane_timer_address);
			mem_write8(lane_timer_address, lane_timer_before - 1u);
			if ((int8_t)lane_timer_before > 1)
			{
			machine_fire_next_lane:
				fireball_lane_timer_cursor++;
				lane_before = fireball_lane;
				fireball_lane = lane_before - 1u;
				if ((int8_t)lane_before > 1)
					continue;
				break;
			}
			next_random(&random_bits, &ignored, &ignored);
			random_bits = (random_bits & ~0xFFu) | (random_bits & 0x1Fu);
			mem_write8(fireball_lane_timer_cursor, random_bits);
			allocate_object(&free_count, &object);
			if (free_count == 0u)
				break;
			OBJECT(object)->handler = fireball_start_ADDRESS;
			OBJECT(object)->var_7F = 0u;
			OBJECT(object)->x = OBJECT(current_object)->x;
			OBJECT(object)->y = OBJECT(current_object)->y;
			OBJECT(object)->y_int += (uint8_t)fireball_lane * 0x38u;
			OBJECT(object)->var_96 = OBJECT(object)->x_int;
			OBJECT(object)->var_96 += mem_read8(fireball_lane_timer_cursor);
			AS16(OBJECT(object)->player) = OBJECT(object)->y_int;
			OBJECT(object)->z = OBJECT(current_object)->z;
			OBJECT(object)->next_handler = flame_start_ADDRESS;
			goto machine_fire_next_lane;
		}
		sound_timer_3 = OBJECT(current_object)->machine_fire.sound_timer;
		OBJECT(current_object)->machine_fire.sound_timer = sound_timer_3 - 1u;
		if ((int8_t)sound_timer_3 <= 1)
		{
			OBJECT(current_object)->machine_fire.sound_timer = 0x3C;
			sound_id_2 = 9u;
			play_sound(sound_id_2, &r1, &r2);
		}
		project_object(&ignored, &ignored);
		ignored = add_to_touchable_list(ignored);
		return;
	case 5:
		machine_fire_done = 0xFFu;
		OBJECT(current_object)->machine_fire.state++;
		if ((int16_t)(OBJECT(current_object)->screen_y - OBJECT(current_object)->hitbox_height) > 0x110 || OBJECT(current_object)->screen_x <= -0xA0)
		{
			OBJECT(current_object)->machine_fire.state++;
			sound_timer_2 = OBJECT(current_object)->machine_fire.sound_timer;
			OBJECT(current_object)->machine_fire.sound_timer = sound_timer_2 - 1u;
			if ((int8_t)sound_timer_2 <= 1)
			{
				OBJECT(current_object)->machine_fire.sound_timer = 0x3C;
				sound_id_2 = 9u;
				play_sound(sound_id_2, &r1, &r2);
			}
			update_screen_position(&ignored, &r1);
			ignored = add_to_touchable_list(ignored);
			return;
		}
		sound_timer_2 = OBJECT(current_object)->machine_fire.sound_timer;
		OBJECT(current_object)->machine_fire.sound_timer = sound_timer_2 - 1u;
		if ((int8_t)sound_timer_2 <= 1)
		{
			OBJECT(current_object)->machine_fire.sound_timer = 0x3C;
			sound_id_2 = 9u;
			play_sound(sound_id_2, &r1, &r2);
		}
		update_screen_position(&ignored, &r1);
		ignored = add_to_touchable_list(ignored);
		return;
	case 6:
		if ((int16_t)(OBJECT(current_object)->screen_y - OBJECT(current_object)->hitbox_height) > 0x110 || OBJECT(current_object)->screen_x <= -0xA0)
			OBJECT(current_object)->machine_fire.state++;
		sound_timer_2 = OBJECT(current_object)->machine_fire.sound_timer;
		OBJECT(current_object)->machine_fire.sound_timer = sound_timer_2 - 1u;
		if ((int8_t)sound_timer_2 <= 1)
		{
			OBJECT(current_object)->machine_fire.sound_timer = 0x3C;
			sound_id_2 = 9u;
			play_sound(sound_id_2, &r1, &r2);
		}
		update_screen_position(&ignored, &r1);
		ignored = add_to_touchable_list(ignored);
		return;
	case 7:
		delete_current_object();
		return;
	default:
		unknown_code(0x8358Cu + OBJECT(current_object)->machine_fire.state * 3u);
	}
}

void start_fireball_flipped(void)
{
	OBJECT(current_object)->sprite_flags = 0x1044u;
	OBJECT(current_object)->sprite_palette = 0x10C0u;
	init_fireball_flight();
}

void fireball_start(void)
{
	OBJECT(current_object)->sprite_flags = 0x1004u;
	OBJECT(current_object)->sprite_palette = 0x9010u;
	init_fireball_flight();
}

void init_fireball_flight(void)
{
	uint8_t sound_id = 0;
	uint32_t r2 = 0, flight_frames = 0;

	OBJECT(current_object)->sprite_offset_x = 0;
	OBJECT(current_object)->sprite_offset_y = 0;
	OBJECT(current_object)->flags = 0u;
	OBJECT(current_object)->animation = fireball_animation_ADDRESS;
	OBJECT(current_object)->animation_frame = 0u;
	OBJECT(current_object)->animation_timer = 0;
	OBJECT(current_object)->floor_z = OBJECT(current_object)->z;
	OBJECT(current_object)->init_fireball_flight.state = 0u;
	OBJECT(current_object)->animation_speed = 3u;
	OBJECT(current_object)->init_fireball_flight.flight_timer = 0x36;
	if (stage == 0xCu)
	{
		OBJECT(current_object)->animation_speed = 2u;
		OBJECT(current_object)->init_fireball_flight.flight_timer = 0x24;
	}
	flight_frames = (uint16_t)OBJECT(current_object)->init_fireball_flight.flight_timer;
	OBJECT(current_object)->speed_x = divide_s32(((uint32_t)(uint16_t)OBJECT(current_object)->init_fireball_flight.target_x << 16) - OBJECT(current_object)->x, flight_frames);
	OBJECT(current_object)->speed_y = divide_s32(((uint32_t)(uint16_t)OBJECT(current_object)->init_fireball_flight.target_y << 16) - OBJECT(current_object)->y, flight_frames);
	OBJECT(current_object)->speed_z = 0;
	OBJECT(current_object)->direction = 0u;
	sound_id = 0xFu;
	play_sound(sound_id, &flight_frames, &r2);
	OBJECT(current_object)->handler = fireball_fly_ADDRESS;
}

void fireball_fly(void)
{
	uint32_t r1 = 0, arc_step = 0, at_last_step = 0, ignored = 0;
	uint16_t flight_timer = 0;

	switch (OBJECT(current_object)->fireball_fly.state)
	{
	case 0:
	case 2:
		OBJECT(current_object)->x += OBJECT(current_object)->speed_x;
		OBJECT(current_object)->y += OBJECT(current_object)->speed_y;
		arc_step = 9u;
		if (stage == 0xCu)
			arc_step = 0xEu;
		OBJECT(current_object)->fireball_fly.arc_angle += arc_step;
		OBJECT(current_object)->z = sine_table[OBJECT(current_object)->direction & 0x3FFu] * 0xFFFFFEC0u + OBJECT(current_object)->floor_z;
		animate_and_move(0u);
		flight_timer = OBJECT(current_object)->fireball_fly.flight_timer;
		OBJECT(current_object)->fireball_fly.flight_timer = flight_timer - 1u;
		if ((int16_t)flight_timer <= 1)
		{
			OBJECT(current_object)->fireball_fly.state++;
			OBJECT(current_object)->z = OBJECT(current_object)->floor_z;
		}
		project_object(&ignored, &ignored);
		ignored = queue_sprite_at_floor();
		return;
	case 1:
		at_last_step = animate_and_move(0u);
		if ((uint8_t)at_last_step != 0u)
		{
			OBJECT(current_object)->handler = OBJECT(current_object)->next_handler;
			OBJECT(current_object)->fireball_fly.state = 0u;
			OBJECT(current_object)->z -= 0x280000u;
		}
		update_screen_position(&ignored, &r1);
		r1 = queue_sprite(r1);
		return;
	default:
		unknown_code(0x83AA5u + OBJECT(current_object)->fireball_fly.state * 2u);
	}
}

void flame_start(void)
{
	uint32_t r0 = 0, block_offset = 0, cursor = 0, ignored = 0, y = 0;

	OBJECT(current_object)->sprite_flags = 0x1000u;
	OBJECT(current_object)->sprite_palette = 0x9010u;
	OBJECT(current_object)->sprite_offset_x = 0;
	OBJECT(current_object)->sprite_offset_y = 0;
	OBJECT(current_object)->flags = 0u;
	OBJECT(current_object)->animation = flame_animation_ADDRESS;
	OBJECT(current_object)->animation_frame = 0u;
	OBJECT(current_object)->animation_speed = 2u;
	OBJECT(current_object)->animation_timer = 0;
	OBJECT(current_object)->handler = flame_burn_ADDRESS;
	find_floor(&r0, &y, &block_offset, &ignored, &cursor);
	OBJECT(current_object)->z = OBJECT(current_object)->floor_z;
	project_object(&ignored, &ignored);
}

void flame_burn(void)
{
	uint32_t r1 = 0, r7 = 0, depth = 0, ignored = 0, off_screen = 0;

	off_screen = check_left_screen();
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

void rising_flame_update(void)
{
	uint32_t r1 = 0, r7 = 0, r8 = 0, depth = 0, ignored = 0;

	switch (OBJECT(current_object)->rising_flame.state)
	{
	case 0:
		rising_flame_init();
		break;
	case 1:
		rising_flame_rise();
		break;
	case 2:
		rising_flame_spawn_next();
		break;
	case 3:
		rising_flame_wait_offscreen();
		break;
	case 4:
		rising_flame_init();
		break;
	default:
		unknown_code(0x83BF7u + OBJECT(current_object)->rising_flame.state * 3u);
	}
	if (OBJECT(LINKED(current_object, rising_fire_object))->var_7F == 7u || OBJECT(current_object)->rising_flame.off_screen != 0u)
	{
		delete_current_object();
		return;
	}
	animate(&r1, &r7, &r8);
	update_screen_position(&ignored, &r1);
	if (OBJECT(current_object)->screen_y > 0x108)
		return;
	depth = (uint16_t)(OBJECT(current_object)->screen_y + 0x20u);
	queue_sprite_at_depth(&ignored, &depth);
}

void rising_flame_init(void)
{
	uint32_t ignored = 0;

	OBJECT(current_object)->rising_flame.state++;
	OBJECT(current_object)->sprite_flags = 0x1000u;
	OBJECT(current_object)->sprite_palette = 0x9010u;
	if (stage == 8u)
		OBJECT(current_object)->sprite_palette = 0x9260u;
	OBJECT(current_object)->sprite_offset_x = 0;
	OBJECT(current_object)->sprite_offset_y = 0;
	OBJECT(current_object)->flags = 0u;
	OBJECT(current_object)->animation = flame_animation_ADDRESS;
	OBJECT(current_object)->animation_frame = 0u;
	OBJECT(current_object)->animation_speed = 2u;
	OBJECT(current_object)->animation_timer = 0;
	OBJECT(current_object)->rising_flame.off_screen = 0u;
	OBJECT(current_object)->floor_z = OBJECT(current_object)->z;
	project_object(&ignored, &ignored);
}

void rising_flame_rise(void)
{
	OBJECT(current_object)->z -= 0x10000u;
	if ((int16_t)(OBJECT(current_object)->floor_z_int - OBJECT(current_object)->z_int) >= 0x28)
		OBJECT(current_object)->rising_flame.state++;
}

void rising_flame_spawn_next(void)
{
	uint32_t free_count = 0, object = 0, object_2 = 0, object_3 = 0;

	object = LINKED(current_object, rising_fire_object);
	if (OBJECT(object)->var_7F != 2u)
		OBJECT(current_object)->rising_flame.state++;
	else if ((int16_t)(OBJECT(object)->z_int - OBJECT(object)->hitbox_height) <= OBJECT(current_object)->z_int)
	{
		allocate_object(&free_count, &object_2);
		if (free_count == 0u)
			return;
		OBJECT(object_2)->handler = rising_flame_update_ADDRESS;
		OBJECT(object_2)->rising_flame.state = 0u;
		object_3 = LINKED(current_object, rising_fire_object);
		OBJECT(object_2)->x = OBJECT(object_3)->x;
		OBJECT(object_2)->y = OBJECT(current_object)->y;
		OBJECT(object_2)->z = OBJECT(object_3)->z;
		OBJECT(object_2)->z_int -= OBJECT(object_3)->hitbox_height;
		OBJECT(current_object)->rising_flame.state++;
		return;
	}
}

void rising_flame_wait_offscreen(void)
{
	if (OBJECT(current_object)->screen_y >= 0x110 || OBJECT(current_object)->screen_x <= -0x40)
		OBJECT(current_object)->rising_flame.off_screen = 0xFFu;
}

void how_to_play_button_hand_start(void)
{
	OBJECT(current_object)->sprite_flags = 0xD000u;
	OBJECT(current_object)->sprite_offset_x = 0;
	OBJECT(current_object)->sprite_offset_y = 0;
	OBJECT(current_object)->flags = 0u;
	OBJECT(current_object)->handler = how_to_play_button_hand_update_ADDRESS;
	OBJECT(current_object)->part_list = how_to_play_button_hand_parts_ADDRESS;
	OBJECT(current_object)->animation = how_to_play_button_hand_animation_ADDRESS;
	OBJECT(current_object)->animation_frame = 0u;
	OBJECT(current_object)->how_to_play_button_hand.next_press = how_to_play_button_presses_ADDRESS;
	OBJECT(current_object)->screen_x = 0x42;
	OBJECT(current_object)->screen_y = 0xB5;
	OBJECT(current_object)->handler = how_to_play_button_hand_update_ADDRESS;
}

void how_to_play_button_hand_update(void)
{
	uint16_t frames = 0, kind = 0, screen_x = 0, screen_y = 0;
	uint32_t depth = 0, ignored = 0;

	if (stage != 7u)
	{
		delete_current_object();
		return;
	}
	if ((uint16_t)game_mode_timer == 0xC8u)
	{
		screen_x = 0xA0u;
		screen_y = 0x7Cu;
		kind = 0xCu;
		frames = 0x12Cu;
		spawn_message_box(screen_x, screen_y, kind, frames, 0u);
	}
	if ((uint16_t)game_mode_timer == mem_read16(OBJECT(current_object)->how_to_play_button_hand.next_press))
	{
		OBJECT(current_object)->animation_frame = (uint8_t)mem_read16(OBJECT(current_object)->how_to_play_button_hand.next_press + 2u);
		OBJECT(current_object)->how_to_play_button_hand.next_press += 4u;
		animate_part_list_step();
	}
	depth = 0xEEu;
	queue_sprite_at_depth(&ignored, &depth);
}

void how_to_play_trackball_hand_start(void)
{
	uint32_t free_count = 0, ignored = 0, object = 0;

	OBJECT(current_object)->sprite_flags = 0x1000u;
	OBJECT(current_object)->sprite_frame = 0x71C8u;
	OBJECT(current_object)->sprite_palette = 0x8530u;
	OBJECT(current_object)->sprite_offset_x = 0;
	OBJECT(current_object)->sprite_offset_y = 0;
	OBJECT(current_object)->flags = 0u;
	OBJECT(current_object)->handler = how_to_play_trackball_hand_update_ADDRESS;
	OBJECT(current_object)->how_to_play_trackball_hand.step = 0u;
	OBJECT(current_object)->screen_x = 0xFB;
	OBJECT(current_object)->screen_y = 0xB3;
	OBJECT(current_object)->projected_x = OBJECT(current_object)->screen_x;
	OBJECT(current_object)->handler = how_to_play_trackball_hand_update_ADDRESS;
	OBJECT(current_object)->animation = how_to_play_trackball_animation_ADDRESS;
	OBJECT(current_object)->animation_frame = 0u;
	OBJECT(current_object)->animation_speed = 4u;
	OBJECT(current_object)->animation_timer = 0;
	OBJECT(current_object)->child = 0u;
	allocate_object_2(&free_count, &ignored, &object);
	if (free_count != 0u)
	{
		OBJECT(object)->sprite_flags = 0x1000u;
		OBJECT(object)->sprite_frame = 0x71C0u;
		OBJECT(object)->sprite_palette = 0x8550u;
		OBJECT(object)->screen_x = OBJECT(current_object)->screen_x;
		OBJECT(object)->screen_x -= 0x38u;
		OBJECT(object)->screen_y = OBJECT(current_object)->screen_y;
		OBJECT(object)->screen_y -= 8u;
		OBJECT(current_object)->child = object;
	}
}

void how_to_play_trackball_hand_update(void)
{
	uint8_t last_step = 0;
	uint16_t child = 0, child_palette = 0, frames = 0, kind = 0, screen_x = 0, screen_y = 0;
	uint32_t box = 0, depth = 0, ignored = 0, next_step = 0, step_list = 0;
	uint8_t pause_timer = 0, animation_timer = 0;

	if (stage == 7u)
	{
		if (game_mode_timer >= 0x32)
		{
			switch (OBJECT(current_object)->how_to_play_trackball_hand.step)
			{
			case 0:
			case 3:
				OBJECT(current_object)->screen_x -= 0xAu;
				OBJECT(current_object)->screen_y += 0xAu;
				if ((int16_t)(OBJECT(current_object)->screen_x - OBJECT(current_object)->projected_x) <= -0x20)
				{
					OBJECT(current_object)->how_to_play_trackball_hand.step++;
					OBJECT(current_object)->how_to_play_trackball_hand.pause_timer = 8;
				}
				break;
			case 1:
				pause_timer = OBJECT(current_object)->how_to_play_trackball_hand.pause_timer;
				OBJECT(current_object)->how_to_play_trackball_hand.pause_timer = pause_timer - 1u;
				if ((int8_t)pause_timer <= 1)
					OBJECT(current_object)->how_to_play_trackball_hand.step++;
				break;
			case 2:
				OBJECT(current_object)->screen_x += 4u;
				OBJECT(current_object)->screen_y -= 4u;
				if (OBJECT(current_object)->projected_x <= OBJECT(current_object)->screen_x)
					OBJECT(current_object)->how_to_play_trackball_hand.step = 0u;
				break;
			default:
				unknown_code(0x83E6Du + OBJECT(current_object)->how_to_play_trackball_hand.step * 2u);
			}
		}
		if ((uint16_t)game_mode_timer == 0xAu)
			OBJECT(current_object)->sprite_palette = 0x8760u;
		else if ((uint16_t)game_mode_timer == 0xBEu)
			OBJECT(current_object)->sprite_palette = 0x8530u;
		if ((uint16_t)game_mode_timer == 0xAu)
		{
			screen_x = 0x40u;
			screen_y = 0x30u;
			kind = 0xDu;
			frames = 0x1E0u;
			box = spawn_message_box(screen_x, screen_y, kind, frames, 0u);
			screen_x = 0xA0u;
			screen_y = 0x7Cu;
			kind = 0xBu;
			frames = 0xB4u;
			spawn_message_box(screen_x, screen_y, kind, frames, box);
		}
		if (game_mode_timer >= 0x20)
		{
			scroll_mode = 0u;
			AS8(scroll_speed) = 0x10u;
		}
		depth = 0xEEu;
		queue_sprite_at_depth(&ignored, &depth);
		current_object = LINKED(current_object, OBJECT(current_object)->child);
		if ((uint16_t)current_object == 0u)
			return;
		if (game_mode_timer >= 0x32)
		{
			animation_timer = OBJECT(object_cursor)->animation_timer;
			OBJECT(object_cursor)->animation_timer = animation_timer - 1u;
			if ((int8_t)animation_timer <= 1)
			{
				OBJECT(object_cursor)->animation_timer = OBJECT(object_cursor)->animation_speed;
				step_list = OBJECT(object_cursor)->animation;
				last_step = (uint8_t)mem_read16(step_list);
				next_step = OBJECT(object_cursor)->animation_frame;
				next_step = (uint8_t)(next_step + 1u);
				step_list += 2u;
				if ((int8_t)last_step < (int8_t)next_step)
					next_step = 0u;
				OBJECT(object_cursor)->animation_frame = next_step;
				OBJECT(current_object)->sprite_frame = mem_read16(step_list + next_step * 2u);
			}
		}
		child_palette = 0x8750u;
		if (OBJECT(object_cursor)->sprite_palette != 0x8760u)
			child_palette = 0x8550u;
		OBJECT(current_object)->sprite_palette = child_palette;
		depth = 0xEDu;
		queue_sprite_at_depth(&ignored, &depth);
		return;
	}
	if (OBJECT(current_object)->child != 0u)
	{
		child = OBJECT(current_object)->child;
		free_object_2(child);
	}
	delete_current_object();
}

void how_to_play_panel_start(void)
{
	OBJECT(current_object)->sprite_flags = 0x100Au;
	OBJECT(current_object)->sprite_frame = 0x71F0u;
	OBJECT(current_object)->sprite_palette = 0x8780u;
	OBJECT(current_object)->screen_x = 0;
	OBJECT(current_object)->screen_y = 0x88;
	OBJECT(current_object)->handler = how_to_play_panel_show_ADDRESS;
	how_to_play_panel_show();
}

void how_to_play_panel_show(void)
{
	uint32_t depth = 0, ignored = 0;

	if (stage == 7u)
	{
		depth = 0xECu;
		queue_sprite_at_depth(&ignored, &depth);
		return;
	}
	delete_current_object();
}

void opening_ground_start(void)
{
	OBJECT(current_object)->sprite_flags = 0x1000u;
	OBJECT(current_object)->sprite_palette = 0x9120u;
	OBJECT(current_object)->flags = 0u;
	OBJECT(current_object)->speed_y = 0;
	OBJECT(current_object)->handler = opening_ground_run_ADDRESS;
	OBJECT(current_object)->x = 0xA00000;
	OBJECT(current_object)->y = 0xB80000;
	OBJECT(current_object)->animation_frame = 0u;
	OBJECT(current_object)->animation = opening_ground_animation_ADDRESS;
	OBJECT(current_object)->animation_timer = 0;
	OBJECT(current_object)->animation_speed = 1u;
}

void opening_ground_run(void)
{
	uint16_t shake_dy = 0;
	uint32_t r7 = 0, r8 = 0, depth = 0, frame = 0, ignored = 0;

	if ((int8_t)scene_step >= 2)
	{
		delete_current_object();
		return;
	}
	OBJECT(current_object)->y += OBJECT(current_object)->speed_y;
	OBJECT(current_object)->screen_x = OBJECT(current_object)->x_int;
	OBJECT(current_object)->screen_y = OBJECT(current_object)->y_int;
	shake_dy = AT8(stage_06_shake, 1);
	if (stage_06_shake_sign < 0)
		shake_dy = (uint16_t)(0u - shake_dy);
	OBJECT(current_object)->screen_y += shake_dy;
	animate(&frame, &r7, &r8);
	depth = 0x20u;
	queue_sprite_at_depth(&ignored, &depth);
}

void opening_machine_start(void)
{
	OBJECT(current_object)->opening_machine.scene_step_seen = 0xFFu;
	OBJECT(current_object)->handler = opening_machine_update_ADDRESS;
	opening_machine_update();
}

void opening_machine_update(void)
{
	uint16_t shake_dy = 0;
	uint32_t depth = 0, free_count = 0, ignored = 0, object = 0, scale = 0;

	switch (scene_step)
	{
	case 0:
	case 1:
	case 4:
		if (OBJECT(current_object)->opening_machine.scene_step_seen != scene_step)
		{
			OBJECT(current_object)->opening_machine.scene_step_seen = scene_step;
			OBJECT(current_object)->sprite_flags = 0xC000u;
			OBJECT(current_object)->opening_machine.zoom = 0xC00000;
			OBJECT(current_object)->sprite_scale = OBJECT(current_object)->opening_machine.zoom_int;
			OBJECT(current_object)->opening_machine.zoom_speed = 0x8000;
			OBJECT(current_object)->flags = 0u;
			OBJECT(current_object)->opening_machine.phase = 0;
			OBJECT(current_object)->speed_y = -0xC0000;
			OBJECT(current_object)->x = 0xA00000;
			OBJECT(current_object)->y = 0xF80000;
			OBJECT(current_object)->animation_frame = 0u;
			OBJECT(current_object)->animation = opening_machine_leap_animation_ADDRESS;
			OBJECT(current_object)->animation_timer = 0;
			OBJECT(current_object)->animation_speed = 2u;
			opening_machine_object = current_object;
		}
		OBJECT(current_object)->opening_machine.draw_depth = OBJECT(current_object)->screen_y;
		switch ((uint8_t)OBJECT(current_object)->opening_machine.phase)
		{
		case 0:
			opening_machine_leap_in();
			break;
		case 1:
			opening_machine_approach();
			break;
		case 2:
			opening_machine_lunge();
			break;
		case 3:
			opening_machine_show_mouth();
			break;
		case 4:
			opening_machine_engulf();
			break;
		case 5:
			opening_machine_hold();
			break;
		case 6:
			opening_machine_leap_in();
			break;
		default:
			unknown_code(0x84157u + (uint8_t)OBJECT(current_object)->opening_machine.phase * 3u);
		}
		OBJECT(current_object)->screen_x = OBJECT(current_object)->x_int;
		OBJECT(current_object)->screen_y = OBJECT(current_object)->y_int;
		shake_dy = AT8(stage_06_shake, 1);
		if (stage_06_shake_sign < 0)
			shake_dy = (uint16_t)(0u - shake_dy);
		OBJECT(current_object)->screen_y += shake_dy;
		if (OBJECT(current_object)->opening_machine.phase < 3)
			animate_part_list();
		depth = OBJECT(current_object)->opening_machine.draw_depth;
		queue_sprite_at_depth(&ignored, &depth);
		return;
	case 2:
	case 3:
		if (OBJECT(current_object)->opening_machine.scene_step_seen != scene_step)
		{
			OBJECT(current_object)->opening_machine.scene_step_seen = scene_step;
			if (OBJECT(current_object)->opening_machine.scene_step_seen == 2u)
			{
				OBJECT(current_object)->sprite_flags = 0xC000u;
				OBJECT(current_object)->opening_machine.zoom = 0;
				OBJECT(current_object)->flags = 0u;
				OBJECT(current_object)->y = -0x600000;
				OBJECT(current_object)->animation_frame = 0u;
				OBJECT(current_object)->animation = opening_machine_fly_away_animation_ADDRESS;
				OBJECT(current_object)->animation_timer = 0;
				OBJECT(current_object)->animation_speed = 2u;
				OBJECT(current_object)->child = 0xFFFFu;
				allocate_object_2(&free_count, &ignored, &object);
				if (free_count != 0u)
				{
					OBJECT(object)->sprite_flags = 0u;
					OBJECT(object)->sprite_frame = 0x7078u;
					OBJECT(object)->sprite_palette = 0x87A0u;
					OBJECT(object)->sprite_width = 0;
					OBJECT(object)->sprite_height = 0x18;
					OBJECT(current_object)->child = object;
				}
			}
		}
		OBJECT(current_object)->opening_machine.zoom += 2u;
		OBJECT(current_object)->sprite_scale = divide_s32(0x4000u, OBJECT(current_object)->opening_machine.zoom + 0x10u);
		OBJECT(current_object)->y += 0x10000u;
		scale = OBJECT(current_object)->sprite_scale;
		OBJECT(current_object)->screen_y = ((int32_t)(OBJECT(current_object)->y_int * scale) >> 10) + 0xB8u;
		animate_part_list();
		queue_sprite(scale);
		draw_opening_machine_shadow();
		return;
	default:
		unknown_code(0x84089u + scene_step * 3u);
	}
}

void opening_machine_leap_in(void)
{
	uint8_t sound_id = 0;
	uint32_t r1 = 0, r2 = 0;

	if (OBJECT(current_object)->speed_y_int < 0)
		OBJECT(current_object)->opening_machine.draw_depth = 0x10u;
	if (scene_timer >= 0x3C)
	{
		OBJECT(current_object)->speed_y += 0x6000u;
		OBJECT(current_object)->y += OBJECT(current_object)->speed_y;
		if (OBJECT(current_object)->speed_y_int >= 0 && OBJECT(current_object)->y_int >= 0x63)
		{
			OBJECT(current_object)->y = 0x630000;
			stage_06_shake = 0x800u;
			stage_06_shake_sign = 0;
			OBJECT(current_object)->animation_frame = 0u;
			OBJECT(current_object)->animation = opening_machine_landed_animation_ADDRESS;
			OBJECT(current_object)->animation_timer = 0;
			OBJECT(current_object)->animation_speed = 2u;
			sound_id = 0xAu;
			play_sound(sound_id, &r1, &r2);
			OBJECT(current_object)->opening_machine.phase++;
		}
	}
}

void opening_machine_approach(void)
{
	uint16_t shake_before = 0;

	if (stage_06_shake != 0u)
	{
		shake_before = stage_06_shake;
		stage_06_shake = shake_before - 0x80u;
		if ((int16_t)shake_before < 0x80)
			stage_06_shake = 0u;
		stage_06_shake_sign = ~stage_06_shake_sign;
	}
	OBJECT(current_object)->opening_machine.zoom += OBJECT(current_object)->opening_machine.zoom_speed;
	OBJECT(current_object)->sprite_scale = OBJECT(current_object)->opening_machine.zoom_int;
	if (scene_timer > 0xC0)
		OBJECT(current_object)->opening_machine.phase++;
}

void opening_machine_lunge(void)
{
	OBJECT(current_object)->opening_machine.zoom_speed += 0x1000u;
	OBJECT(current_object)->opening_machine.zoom += OBJECT(current_object)->opening_machine.zoom_speed;
	OBJECT(current_object)->sprite_scale = OBJECT(current_object)->opening_machine.zoom_int;
	if ((int16_t)OBJECT(current_object)->sprite_scale >= 0x1B3)
		OBJECT(current_object)->y -= 0xE000u;
	if (OBJECT(current_object)->opening_machine.zoom_int >= 0x32B)
		OBJECT(current_object)->opening_machine.phase++;
}

void opening_machine_show_mouth(void)
{
	OBJECT(current_object)->opening_machine.phase++;
	OBJECT(current_object)->sprite_flags = 0u;
	OBJECT(current_object)->sprite_frame = 0x7198u;
	OBJECT(current_object)->sprite_palette = 0x8510u;
	OBJECT(current_object)->y += (OBJECT(current_object)->sprite_scale * 0x1Eu) << 8;
	opening_machine_engulf();
}

void opening_machine_engulf(void)
{
	OBJECT(current_object)->opening_machine.zoom_speed += 0x10000u;
	OBJECT(current_object)->opening_machine.zoom += OBJECT(current_object)->opening_machine.zoom_speed;
	OBJECT(current_object)->sprite_scale = OBJECT(current_object)->opening_machine.zoom_int;
	if (OBJECT(current_object)->opening_machine.zoom_int < 0x5B0)
	{
		opening_machine_hold();
		return;
	}
	OBJECT(current_object)->opening_machine.phase++;
	OBJECT(current_object)->sprite_palette = 0x87A0u;
	opening_machine_hold();
}

void opening_machine_hold(void)
{
}

void draw_opening_machine_shadow(void)
{
	uint16_t scaled_dy = 0;
	uint32_t depth = 0, ignored = 0, sprite_scale = 0;

	current_object = LINKED(current_object, OBJECT(current_object)->child);
	sprite_scale = OBJECT(object_cursor)->sprite_scale;
	OBJECT(current_object)->screen_x = ((int32_t)(OBJECT(current_object)->sprite_width * sprite_scale) >> 8) + OBJECT(object_cursor)->screen_x;
	scaled_dy = (uint16_t)((int32_t)(OBJECT(current_object)->sprite_height * sprite_scale) >> 8);
	OBJECT(current_object)->screen_y = OBJECT(object_cursor)->screen_y;
	OBJECT(current_object)->screen_y += scaled_dy;
	OBJECT(current_object)->sprite_scale = OBJECT(object_cursor)->sprite_scale;
	depth = (uint16_t)(OBJECT(object_cursor)->screen_y - 0x10u);
	queue_sprite_at_depth(&ignored, &depth);
}

void player_pacer_start(void)
{
	OBJECT(current_object)->speed_x_fraction = pacer_end_positions[OBJECT(current_object)->sprite_frame];
	OBJECT(current_object)->handler = player_pacer_update_ADDRESS;
	player_pacer_update();
}

void player_pacer_update(void)
{
	uint32_t axis_field = 0, decreasing = 0, leader_object = 0, middle_player = 0, other_object = 0, player = 0, player_1 = 0, player_2 = 0, player_3 = 0, player_4 = 0, players_left = 0, playing_count = 0, status_cursor = 0;
	uint8_t status = 0;
	uint32_t swap = 0, swap_2 = 0, swap_3 = 0, swap_4 = 0;

	if (OBJECT(current_object)->speed_x_fraction <= scroll_position)
	{
		player_1_speed_scale = 0u;
		player_2_speed_scale = 0u;
		player_3_speed_scale = 0u;
		delete_current_object();
		return;
	}
	player_1_speed_scale = 0u;
	player_2_speed_scale = 0u;
	player_3_speed_scale = 0u;
	status_cursor = player_status_ADDRESS;
	players_left = 3u;
	playing_count = 0u;
	do
	{
		status = mem_read8(status_cursor);
		status_cursor++;
		if (status == 0xFFu)
			playing_count = (uint8_t)(playing_count + 1u);
		players_left--;
	} while (players_left != 0u);
	switch (playing_count)
	{
	case 0:
	case 1:
		return;
	case 2:
	case 4:
		player = PLAYER_BLOCK(1);
		player_2 = PLAYER_BLOCK(2);
		player_3 = PLAYER_BLOCK(3);
		if (player_status[0] == 0xFFu)
		{
			if (player_2_status != 0xFFu)
				player_2 = player_3;
		}
		else
		{
			player = player_2;
			player_2 = player_3;
		}
		get_pacing_axis(player, &axis_field, &decreasing);
		if ((int16_t)mem_read16(LINKED(current_object, PLAYER(player)->object) + axis_field * 2u) < (int16_t)mem_read16(LINKED(current_object, PLAYER(player_2)->object) + axis_field * 2u))
		{
			swap = player;
			player = player_2;
			player_2 = swap;
		}
		if ((uint16_t)decreasing != 0u)
		{
			mem_write16(player_2 + 0xC4u, 0xDu);
			return;
		}
		PLAYER(player)->speed_scale = 0xDu;
		return;
	case 3:
		player_1 = PLAYER_BLOCK(1);
		get_pacing_axis(player_1, &axis_field, &decreasing);
		player = PLAYER_BLOCK(1);
		middle_player = PLAYER_BLOCK(2);
		player_4 = PLAYER_BLOCK(3);
		leader_object = LINKED(current_object, player_1_object);
		other_object = LINKED(current_object, player_2_object);
		if ((int16_t)mem_read16(leader_object + axis_field * 2u) < (int16_t)mem_read16(other_object + axis_field * 2u))
		{
			swap_2 = player;
			player = middle_player;
			middle_player = swap_2;
			leader_object = LINKED(leader_object, PLAYER(player)->object);
		}
		other_object = LINKED(other_object, PLAYER(player_4)->object);
		if ((int16_t)mem_read16(leader_object + axis_field * 2u) < (int16_t)mem_read16(other_object + axis_field * 2u))
		{
			swap_3 = player;
			player = player_4;
			player_4 = swap_3;
		}
		if ((int16_t)mem_read16(((leader_object & ~0xFFFFu) | mem_read16(middle_player + 0x14u)) + axis_field * 2u) < (int16_t)mem_read16(((other_object & ~0xFFFFu) | mem_read16(player_4 + 0x14u)) + axis_field * 2u))
		{
			swap_4 = middle_player;
			middle_player = player_4;
			player_4 = swap_4;
		}
		if ((uint16_t)decreasing != 0u)
		{
			mem_write16(player_4 + 0xC4u, 0xDu);
			mem_write16(middle_player + 0xC4u, 0xEu);
			return;
		}
		PLAYER(player)->speed_scale = 0xDu;
		mem_write16(middle_player + 0xC4u, 0xEu);
		return;
	default:
		unknown_code(0x84428u + playing_count * 2u);
	}
}

void get_pacing_axis(uint32_t player, uint32_t *p_axis_field, uint32_t *p_decreasing)
{
	uint16_t decreasing = 0;
	uint32_t axis_field = 0;

	switch (mem_read8(floor_blocks + 2u + OBJECT(LINKED(current_object, PLAYER(player)->object))->floor_block + 0x15u))
	{
	case 0:
	case 4:
		axis_field = 0x29u;
		decreasing = 0xFFFFu;
		*p_axis_field = axis_field;
		*p_decreasing = (*p_decreasing & 0xFFFF0000u) | decreasing;
		return;
	case 1:
		axis_field = 0x27u;
		decreasing = 0u;
		*p_axis_field = axis_field;
		*p_decreasing = (*p_decreasing & 0xFFFF0000u) | decreasing;
		return;
	case 2:
		axis_field = 0x29u;
		decreasing = 0u;
		*p_axis_field = axis_field;
		*p_decreasing = (*p_decreasing & 0xFFFF0000u) | decreasing;
		return;
	case 3:
		axis_field = 0x27u;
		decreasing = 0xFFFFu;
		*p_axis_field = axis_field;
		*p_decreasing = (*p_decreasing & 0xFFFF0000u) | decreasing;
		return;
	default:
		unknown_code(0x8451Du + mem_read8(floor_blocks + 2u + OBJECT(LINKED(current_object, PLAYER(player)->object))->floor_block + 0x15u) * 2u);
	}
}

void exit_door_start(void)
{
	uint32_t ignored = 0;

	OBJECT(current_object)->sprite_frame = 0x7348u;
	OBJECT(current_object)->sprite_palette = 0x91A0u;
	OBJECT(current_object)->sprite_flags = 0x1004u;
	OBJECT(current_object)->sprite_offset_x = 0;
	OBJECT(current_object)->sprite_offset_y = 0;
	OBJECT(current_object)->flags = 0u;
	OBJECT(current_object)->handler = exit_door_wait_for_switch_ADDRESS;
	OBJECT(current_object)->z = 0;
	project_object(&ignored, &ignored);
}

void exit_door_wait_for_switch(void)
{
	uint32_t ignored = 0;

	if ((uint8_t)stage_05_switch_step != 2u)
	{
		exit_door_show();
		return;
	}
	OBJECT(current_object)->sprite_frame = 0x71A0u;
	OBJECT(current_object)->sprite_palette = 0x9140u;
	OBJECT(current_object)->x = 0x1E20000;
	OBJECT(current_object)->y = 0x154C0000;
	OBJECT(current_object)->handler = exit_door_show_ADDRESS;
	project_object(&ignored, &ignored);
}

void exit_door_show(void)
{
	uint32_t r1 = 0, ignored = 0;

	update_screen_position(&ignored, &r1);
	if (OBJECT(current_object)->screen_y < 0x130)
	{
		r1 = queue_sprite(r1);
		return;
	}
	delete_current_object();
}

void stage_05_floor_patch_start(void)
{
	uint32_t ignored = 0;

	OBJECT(current_object)->sprite_frame = 0x7050u;
	OBJECT(current_object)->sprite_palette = 0xB0E0u;
	OBJECT(current_object)->sprite_flags = 0x1008u;
	OBJECT(current_object)->sprite_offset_x = 0;
	OBJECT(current_object)->sprite_offset_y = 0;
	OBJECT(current_object)->flags = 0u;
	OBJECT(current_object)->handler = stage_05_floor_patch_show_ADDRESS;
	OBJECT(current_object)->z = 0;
	project_object(&ignored, &ignored);
}

void stage_05_floor_patch_show(void)
{
	uint32_t r1 = 0, ignored = 0;

	update_screen_position(&ignored, &r1);
	if (stage == 5u)
	{
		r1 = queue_sprite(r1);
		return;
	}
	delete_current_object();
}

void bubble_start(void)
{
	uint32_t ignored = 0;

	OBJECT(current_object)->z_int = bubble_heights[OBJECT(current_object)->sprite_frame];
	OBJECT(current_object)->z_fraction = 0u;
	OBJECT(current_object)->bubble_start.variant = OBJECT(current_object)->sprite_frame;
	OBJECT(current_object)->bubble_start.base_x = OBJECT(current_object)->x_int;
	OBJECT(current_object)->bubble_start.base_y = OBJECT(current_object)->y_int;
	OBJECT(current_object)->sprite_flags = 0x1004u;
	OBJECT(current_object)->sprite_palette = 0x9000u;
	if (stage != 5u && stage != 8u)
		OBJECT(current_object)->sprite_palette = 0x10B0u;
	if (stage == 8u)
		OBJECT(current_object)->sprite_palette |= 0x4000u;
	OBJECT(current_object)->sprite_offset_x = 0;
	OBJECT(current_object)->sprite_offset_y = 0;
	OBJECT(current_object)->flags = 0u;
	OBJECT(current_object)->handler = bubble_update_ADDRESS;
	OBJECT(current_object)->animation_frame = 0u;
	OBJECT(current_object)->animation = bubble_animation_ADDRESS;
	OBJECT(current_object)->animation_timer = 0;
	OBJECT(current_object)->animation_speed = 4u;
	project_object(&ignored, &ignored);
}

void bubble_update(void)
{
	if ((stage == 0xCu || stage == 8u || stage == 0xBu || stage == 5u) && current_game_mode != 0x20u)
	{
		OBJECT(current_object)->bubble_update.draw_depth = OBJECT(current_object)->screen_y;
		bubble_pop();
		return;
	}
	delete_current_object();
}

void bubble_near_parent_update(void)
{
	if ((int16_t)(OBJECT(LINKED(current_object, OBJECT(current_object)->parent))->y_int + 0xB0u) < OBJECT(current_object)->y_int)
	{
		OBJECT(current_object)->bubble_near_parent_update.draw_depth = 0xFFF0u;
		bubble_pop();
		return;
	}
	delete_current_object();
}

void bubble_pop(void)
{
	uint16_t jitter_y = 0;
	uint32_t r1 = 0, depth = 0, ignored = 0, jitter = 0, off_screen = 0, wrapped = 0;

	if (map_screen_active != 0u)
	{
		delete_current_object();
		return;
	}
	off_screen = check_left_screen();
	if (off_screen != 0u)
	{
		delete_current_object();
		return;
	}
	update_screen_position(&ignored, &r1);
	animate(&r1, &wrapped, &off_screen);
	if ((uint8_t)wrapped == 0u)
	{
		depth = OBJECT(current_object)->bubble_pop.draw_depth;
		queue_sprite_at_depth(&ignored, &depth);
		return;
	}
	next_random(&jitter, &ignored, &ignored);
	if (OBJECT(current_object)->bubble_pop.variant != 6u)
	{
		jitter = (jitter & ~0xFFFFu) | (jitter & 0xFu);
		if ((int32_t)jitter < 0)
			jitter = (uint16_t)(0u - jitter);
		jitter_y = (uint16_t)jitter;
		jitter = (uint16_t)(jitter + OBJECT(current_object)->bubble_pop.base_x);
		OBJECT(current_object)->x_int = jitter;
		OBJECT(current_object)->y_int = jitter_y + OBJECT(current_object)->bubble_pop.base_y;
		project_object(&ignored, &ignored);
	}
	OBJECT(current_object)->animation_speed = (jitter & 7u) + 1u;
	depth = OBJECT(current_object)->bubble_pop.draw_depth;
	queue_sprite_at_depth(&ignored, &depth);
}

void spiked_press_start(void)
{
	uint32_t r0 = 0, r1 = 0, r2 = 0, r3 = 0, r11 = 0;

	OBJECT(current_object)->sprite_flags = 0x1104u;
	OBJECT(current_object)->sprite_frame = 0x7070u;
	OBJECT(current_object)->sprite_palette = 0x9100u;
	OBJECT(current_object)->sprite_offset_x = 0x38;
	OBJECT(current_object)->sprite_offset_y = 0;
	OBJECT(current_object)->kind = 9u;
	OBJECT(current_object)->flags = 2u;
	OBJECT(current_object)->hitbox_size_x = 0xE0u;
	OBJECT(current_object)->hitbox_size_y = 0x20u;
	OBJECT(current_object)->hitbox_offset_x = 0;
	OBJECT(current_object)->hitbox_offset_y = -0x10;
	OBJECT(current_object)->hitbox_height = 0xE0u;
	OBJECT(current_object)->spiked_press.sound_timer = 0;
	OBJECT(current_object)->spiked_press.frame_count = 0;
	OBJECT(current_object)->spiked_press.stopped_frames = 0;
	OBJECT(current_object)->spiked_press.holding_back = 0u;
	OBJECT(current_object)->handler = spiked_press_update_ADDRESS;
	OBJECT(current_object)->speed_x = 0;
	OBJECT(current_object)->speed_y = 0x2000;
	OBJECT(current_object)->direction = 0x200u;
	find_floor(&r0, &r1, &r2, &r3, &r11);
	OBJECT(current_object)->z = OBJECT(current_object)->floor_z;
	spiked_press_object = current_object;
	scroll_mode = 1u;
	players_frozen = 0xFFu;
	spawn_eggman_booth(r0, r1, r3, &r2, &r11);
}

void spiked_press_update(void)
{
	uint8_t sound = 0;
	uint16_t frames = 0, kind = 0, screen_x = 0, screen_y = 0, x = 0, y = 0, z = 0;
	uint32_t r1 = 0, r2 = 0, r11 = 0, depth = 0, ignored = 0, player = 0, player_index = 0, players_left = 0, status_cursor = 0;
	uint8_t hold_timer = 0, status = 0;
	uint16_t sound_timer = 0;

	if (current_game_mode == 3u || current_game_mode == 0xAu)
		OBJECT(current_object)->spiked_press.frame_count++;
	if (OBJECT(current_object)->spiked_press.frame_count >= 0xC8 && OBJECT(current_object)->spiked_press.holding_back == 0u)
	{
		if (no_player_in_play != 0u || player_dropping_in != 0u)
		{
			OBJECT(current_object)->spiked_press.holding_back = 0xFFu;
			OBJECT(current_object)->spiked_press.hold_timer = 0xFFu;
		}
		else
			OBJECT(current_object)->y += OBJECT(current_object)->speed_y;
	}
	if (OBJECT(current_object)->spiked_press.holding_back != 0u)
	{
		if (current_game_mode != 3u && current_game_mode != 0xAu)
			OBJECT(current_object)->spiked_press.hold_timer = 0xFFu;
		hold_timer = OBJECT(current_object)->spiked_press.hold_timer;
		OBJECT(current_object)->spiked_press.hold_timer = hold_timer - 1u;
		if (hold_timer == 0u)
			OBJECT(current_object)->spiked_press.holding_back = 0u;
	}
	if ((uint16_t)OBJECT(current_object)->spiked_press.frame_count == 0xC8u)
	{
		x = 0xFE0Du;
		y = 0x3E0u;
		kind = 1u;
		z = 0xFFB0u;
		r11 = spawn_sign_2(x, y, kind, z, 0u);
		x = 0xFE0Du;
		y = 0x3E0u;
		kind = 2u;
		z = 0xFFC2u;
		r11 = spawn_sign_2(x, y, kind, z, r11);
		players_frozen = 0u;
	}
	if ((uint16_t)OBJECT(current_object)->spiked_press.frame_count == 0x168u)
	{
		x = 0xFE70u;
		y = 0x480u;
		kind = 3u;
		z = 0xFFB0u;
		r11 = spawn_sign_2(x, y, kind, z, r11);
	}
	if ((uint16_t)OBJECT(current_object)->spiked_press.frame_count == 0x14u)
	{
		players_left = 3u;
		player_index = 0u;
		status_cursor = player_status_ADDRESS;
		do
		{
			status = mem_read8(status_cursor);
			status_cursor++;
			if (status == 0xFFu)
				break;
			player_index = (uint16_t)(player_index + 1u);
			players_left--;
		} while (players_left != 0u);
		player = PLAYER_BLOCK(1) + (player_index << 9);
		caged_character = OBJECT(LINKED(player, PLAYER(player)->object))->kind;
		kind = 0xFu;
		if (caged_character != 0u)
		{
			kind = 0x1Au;
			if (caged_character != 1u)
				kind = 0x1Bu;
		}
		frames = 0x78u;
		screen_x = 0xA0u;
		screen_y = 0x40u;
		spawn_message_box(screen_x, screen_y, kind, frames, r11);
	}
	if ((uint16_t)OBJECT(current_object)->spiked_press.frame_count == 0x14u)
	{
		sound = 0x34u;
		play_sound(sound, &r1, &r2);
	}
	if ((uint16_t)OBJECT(current_object)->spiked_press.frame_count == 0x50u)
	{
		sound = 0x3Fu;
		if (caged_character != 0u)
		{
			sound = 0x5Cu;
			if (caged_character != 1u)
				sound = 0x40u;
		}
		play_sound(sound, &r1, &r2);
	}
	if (OBJECT(current_object)->y_int >= 0x3D0)
	{
		OBJECT(current_object)->speed_y = 0;
		OBJECT(current_object)->spiked_press.stopped_frames++;
		scroll_mode = 0u;
		scroll_speed = 5u;
	}
	if (stage != 5u)
	{
		delete_current_object();
		return;
	}
	if (OBJECT(current_object)->speed_y_fraction != 0u && OBJECT(current_object)->spiked_press.frame_count >= 0xC8 && OBJECT(current_object)->spiked_press.holding_back == 0u)
	{
		sound_timer = OBJECT(current_object)->spiked_press.sound_timer;
		OBJECT(current_object)->spiked_press.sound_timer = sound_timer - 1u;
		if ((int16_t)sound_timer <= 1)
		{
			OBJECT(current_object)->spiked_press.sound_timer = 0x3C;
			sound = 0xDu;
			play_sound(sound, &r1, &r2);
		}
	}
	OBJECT(current_object)->health = 0x7F;
	project_object(&ignored, &ignored);
	add_to_touchable_list(ignored);
	depth = (uint16_t)(OBJECT(current_object)->screen_y - 0x50u);
	queue_sprite_at_depth(&ignored, &depth);
}

void cage_start(void)
{
	uint32_t ignored = 0, object = 0;
	uint16_t bars_left_before = 0;

	cage_bars_left = 0xA;
	if (free_object_stack < cage_bars_left)
		return;
	cage_bar_x = OBJECT(current_object)->x_int;
	cage_bar_y = OBJECT(current_object)->y_int;
	cage_bar_hit = 0u;
	do
	{
		allocate_object(&ignored, &object);
		OBJECT(object)->sprite_flags = 0x1006u;
		OBJECT(object)->sprite_frame = 0x7058u;
		OBJECT(object)->sprite_palette = 0x90F0u;
		OBJECT(object)->sprite_offset_x = -8;
		OBJECT(object)->sprite_offset_y = 8;
		OBJECT(object)->kind = 0xCu;
		OBJECT(object)->flags = 2u;
		OBJECT(object)->hitbox_size_x = 0x20u;
		OBJECT(object)->hitbox_size_y = 0x10u;
		OBJECT(object)->hitbox_offset_x = -0x10;
		OBJECT(object)->hitbox_offset_y = -0xC;
		OBJECT(object)->hitbox_height = 0xE0u;
		OBJECT(object)->animation = cage_bar_animation_ADDRESS;
		OBJECT(object)->animation_frame = 0u;
		OBJECT(object)->animation_timer = -1;
		OBJECT(object)->health = 5;
		OBJECT(object)->handler = cage_bar_start_ADDRESS;
		OBJECT(object)->x_int = cage_bar_x;
		OBJECT(object)->x_fraction = 0u;
		OBJECT(object)->y_int = cage_bar_y;
		OBJECT(object)->y_fraction = 0u;
		OBJECT(object)->z = 0;
		cage_bar_x += 0x18u;
		bars_left_before = cage_bars_left;
		cage_bars_left = bars_left_before - 1u;
	} while ((int16_t)bars_left_before > 1);
	delete_current_object();
}

void cage_bar_start(void)
{
	uint32_t ignored = 0;

	project_object(&ignored, &ignored);
	OBJECT(current_object)->handler = cage_bar_update_ADDRESS;
	cage_bar_update();
}

void cage_bar_update(void)
{
	uint8_t sound_id = 0;
	uint32_t r1 = 0, r2 = 0, animation_frame = 0, depth = 0, ignored = 0, off_screen = 0, step_list = 0;
	uint8_t animation_timer = 0;
	uint16_t last_step = 0;

	OBJECT(current_object)->health = 5;
	if ((int16_t)OBJECT((current_object & ~0xFFFFu) | spiked_press_object)->var_82 >= 0xC8 && (int8_t)OBJECT(current_object)->animation_frame >= 0 && (OBJECT(current_object)->flags & 4u) != 0u && (OBJECT(current_object)->flags & 0x40u) != 0u)
	{
		OBJECT(current_object)->animation_frame++;
		animation_frame = OBJECT(current_object)->animation_frame;
		step_list = OBJECT(current_object)->animation;
		last_step = mem_read16(step_list);
		step_list += 2u;
		if ((int16_t)animation_frame < (int16_t)last_step)
		{
			OBJECT(current_object)->sprite_frame = mem_read16(step_list + animation_frame * 2u);
			OBJECT(current_object)->health = 1;
			cage_bar_hit = 0xFFu;
		}
		else
		{
			OBJECT(current_object)->animation_timer = 8;
			OBJECT(current_object)->animation_frame = 0xFFu;
		}
		sound_id = 0x21u;
		play_sound(sound_id, &r1, &r2);
	}
	if ((int8_t)OBJECT(current_object)->animation_frame < 0 && OBJECT(current_object)->animation_timer >= 0)
	{
		animation_timer = OBJECT(current_object)->animation_timer;
		OBJECT(current_object)->animation_timer = animation_timer - 1u;
		if ((int8_t)animation_timer < 1)
		{
			OBJECT(current_object)->sprite_frame = mem_read16(OBJECT(current_object)->animation + 6u);
			OBJECT(current_object)->projected_x++;
		}
	}
	update_screen_position(&ignored, &r1);
	if (stage != 5u)
	{
		delete_current_object();
		return;
	}
	off_screen = check_left_screen();
	if (off_screen != 0u)
	{
		delete_current_object();
		return;
	}
	if ((int8_t)OBJECT(current_object)->animation_frame >= 0)
		add_to_touchable_list(ignored);
	depth = (uint16_t)(OBJECT(current_object)->screen_y - 8u);
	queue_sprite_at_depth(&ignored, &depth);
}

void sign_start(void)
{
	uint32_t ignored = 0, kind_record = 0, sign = 0;

	kind_record = OBJECT(current_object)->sprite_frame * 0xCu + object_kinds_ADDRESS;
	OBJECT(current_object)->z_int = mem_read16(kind_record);
	OBJECT(current_object)->z_fraction = 0u;
	sign = current_object;
	kind_record += 2u;
	init_sign_from_kind(kind_record, &ignored, &sign);
}

void spawn_sign(uint16_t x, uint16_t y, uint16_t kind, uint16_t z, uint32_t *p_r1, uint32_t *p_sign)
{
	uint32_t r1 = *p_r1;
	uint32_t sign = *p_sign;
	uint32_t r1_2 = 0, free_count = 0;

	allocate_object_outside_loop(&free_count, &sign);
	if (free_count == 0u)
	{
		*p_sign = sign;
		return;
	}
	place_sign(x, y, kind, z, &r1_2, &sign);
	*p_r1 = r1_2;
	*p_sign = sign;
}

uint32_t spawn_sign_2(uint16_t x, uint16_t y, uint16_t kind, uint16_t z, uint32_t sign)
{
	uint32_t free_count = 0, ignored = 0;

	allocate_object(&free_count, &sign);
	if (free_count == 0u)
		return sign;
	place_sign(x, y, kind, z, &ignored, &sign);
	return sign;
}

void place_sign(uint16_t x, uint16_t y, uint32_t kind, uint16_t z, uint32_t *p_r1, uint32_t *p_sign)
{
	uint32_t sign = *p_sign;
	uint32_t r1 = 0;

	OBJECT(sign)->x_int = x;
	OBJECT(sign)->x_fraction = 0u;
	OBJECT(sign)->y_int = y;
	OBJECT(sign)->y_fraction = 0u;
	OBJECT(sign)->z_int = z;
	OBJECT(sign)->z_fraction = 0u;
	kind = (uint16_t)kind * 0xCu + 0x84FB7u;
	init_sign_from_kind(kind, &r1, &sign);
	*p_r1 = r1;
	*p_sign = sign;
}

void init_sign_from_kind(uint32_t record, uint32_t *p_r1, uint32_t *p_sign)
{
	uint32_t sign = *p_sign;
	uint32_t r1 = 0, ignored = 0;
	uint32_t saved_current_object = 0;

	OBJECT(sign)->sprite_frame = mem_read16(record);
	record += 2u;
	OBJECT(sign)->speed_x_fraction = mem_read16(record);
	record += 2u;
	AS32(OBJECT(sign)->var_80) = mem_read16(record);
	AS32(OBJECT(sign)->var_80) += 0x8514Du;
	OBJECT(sign)->handler = mem_read32(record + 2u);
	OBJECT(sign)->sprite_flags = 0x1000u;
	OBJECT(sign)->flags = 0u;
	OBJECT(sign)->sprite_offset_x = 0;
	OBJECT(sign)->sprite_offset_y = 0;
	AS8(OBJECT(sign)->var_84) = 0u;
	AT8(OBJECT(sign)->var_84, 1) = 4u;
	AS8(OBJECT(sign)->var_86) = 0u;
	OBJECT(sign)->parent = current_object;
	saved_current_object = current_object;
	current_object = sign;
	project_object(&ignored, &r1);
	sign = current_object;
	current_object = saved_current_object;
	*p_r1 = r1;
	*p_sign = sign;
}

void sign_show_unless_mode_32(void)
{
	if (current_game_mode != 0x20u)
	{
		sign_show();
		return;
	}
	delete_current_object();
}

void sign_show_while_player_busy(void)
{
	uint32_t player = 0, player_2 = 0;

	player = OBJECT(LINKED(current_object, OBJECT(current_object)->parent))->player;
	player_2 = PLAYER_BLOCK(1) + (uint16_t)(player << 9);
	if (PLAYER(player_2)->state == 0x1Du || PLAYER(player_2)->state == 0x2Cu || PLAYER(player_2)->state == 0x32u)
	{
		sign_show();
		return;
	}
	delete_current_object();
}

void sign_show_until_goal_unlocked(void)
{
	if (goal_unlocked == 0u)
	{
		sign_show();
		return;
	}
	delete_current_object();
}

void sign_show_while_parent_still(void)
{
	if ((uint16_t)OBJECT(LINKED(current_object, OBJECT(current_object)->parent))->speed_z_int == 0u)
	{
		sign_show();
		return;
	}
	delete_current_object();
}

void sign_show_until_20e5f4(void)
{
	if (sign_stop_flag == 0u)
	{
		sign_show();
		return;
	}
	delete_current_object();
}

void sign_show_until_cage_bar_hit(void)
{
	if (cage_bar_hit == 0u)
	{
		sign_show_unless_stage_0();
		return;
	}
	delete_current_object();
}

void sign_show_unless_stage_0(void)
{
	if (stage != 0u)
	{
		sign_show();
		return;
	}
	delete_current_object();
}

void sign_show(void)
{
	uint8_t bucket = 0;
	uint32_t r1 = 0, ignored = 0;
	uint16_t speed_x_fraction = 0;

	update_screen_position(&ignored, &r1);
	animate_sign();
	speed_x_fraction = OBJECT(current_object)->speed_x_fraction;
	OBJECT(current_object)->speed_x_fraction = speed_x_fraction - 1u;
	if ((int16_t)speed_x_fraction > 1)
	{
		bucket = 0u;
		queue_sprite_in_bucket(bucket);
		return;
	}
	delete_current_object();
}

void sign_follow_parent(void)
{
	uint8_t bucket = 0;
	uint32_t object = 0;

	animate_sign();
	object = LINKED(current_object, OBJECT(current_object)->parent);
	OBJECT(current_object)->screen_x = OBJECT(object)->screen_x;
	OBJECT(current_object)->screen_y = ((int32_t)((uint32_t)OBJECT(object)->projected_y + OBJECT(object)->floor_z) >> 16) - view_y - 0x38u;
	if ((OBJECT(object)->flags & 0x40u) != 0u)
	{
		bucket = 0u;
		queue_sprite_in_bucket(bucket);
		return;
	}
	delete_current_object();
}

void sign_follow_player(void)
{
	uint8_t bucket = 0;
	uint32_t object = 0, player = 0;

	animate_sign();
	object = LINKED(current_object, OBJECT(current_object)->parent);
	OBJECT(current_object)->screen_x = OBJECT(object)->screen_x;
	OBJECT(current_object)->screen_y = ((int32_t)((uint32_t)OBJECT(object)->projected_y + OBJECT(object)->floor_z) >> 16) - view_y - 0x28u;
	player = PLAYER_BLOCK(1);
	if (OBJECT(object)->player != 0u)
	{
		player = PLAYER_BLOCK(2);
		if (OBJECT(object)->player != 1u)
			player = PLAYER_BLOCK(3);
	}
	if (PLAYER(player)->state == 0x22u)
	{
		bucket = 0u;
		queue_sprite_in_bucket(bucket);
		return;
	}
	delete_current_object();
}

void spreading_flame_start(void)
{
	uint32_t r0 = 0, block_offset = 0, cursor = 0, ignored = 0, y = 0;

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
	find_floor(&r0, &y, &block_offset, &ignored, &cursor);
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

uint32_t spawn_stage_03_end_pillar(uint32_t pillar)
{
	uint32_t free_count = 0;

	allocate_object_outside_loop(&free_count, &pillar);
	if (free_count != 0u)
	{
		OBJECT(pillar)->sprite_flags = 0x1004u;
		OBJECT(pillar)->sprite_frame = 0x6600u;
		OBJECT(pillar)->sprite_palette = 0x8740u;
		OBJECT(pillar)->sprite_offset_x = 0;
		OBJECT(pillar)->sprite_offset_y = 0;
		OBJECT(pillar)->flags = 0u;
		OBJECT(pillar)->handler = stage_03_end_pillar_show_ADDRESS;
		OBJECT(pillar)->screen_x = 0x146;
		OBJECT(pillar)->screen_y = 0x75;
	}
	return pillar;
}

void stage_03_end_pillar_show(void)
{
	uint32_t depth = 0, ignored = 0;

	if (map_screen_active == 0u)
	{
		depth = (uint16_t)(OBJECT(current_object)->screen_y + 0x20u);
		queue_sprite_at_depth(&ignored, &depth);
		return;
	}
	delete_current_object();
}

void eggman_speech_start(void)
{
	OBJECT(current_object)->eggman_speech.frame_count = 0;
	OBJECT(current_object)->eggman_speech.script = eggman_speech_scripts[OBJECT(current_object)->sprite_frame] + eggman_speech_scripts_ADDRESS;
	OBJECT(current_object)->handler = eggman_speech_run_ADDRESS;
	eggman_speech_run();
}

void eggman_speech_run(void)
{
	uint8_t sound_id = 0;
	uint16_t frames = 0, kind = 0, screen_x = 0, screen_y = 0;
	uint32_t r1 = 0, r2 = 0, r11 = 0, command = 0, script_cursor = 0;
	uint16_t delay = 0;

	if (current_game_mode == 0x11u)
		return;
	OBJECT(current_object)->eggman_speech.frame_count++;
	script_cursor = OBJECT(current_object)->eggman_speech.script;
	delay = mem_read16(script_cursor);
	script_cursor += 2u;
	if (OBJECT(current_object)->eggman_speech.frame_count < (int16_t)delay)
		return;
	command = mem_read16(script_cursor) * 2u;
	script_cursor += 2u;
	switch (command)
	{
	case 0x0u:
	case 0x8u:
		delete_current_object();
		return;
	case 0x2u:
		OBJECT(current_object)->eggman_speech.script = script_cursor;
		OBJECT(current_object)->eggman_speech.frame_count = 0;
		spawn_eggman_booth(command, 0u, 0u, &r2, &r11);
		return;
	case 0x4u:
		break;
	case 0x6u:
		sound_id = (uint8_t)mem_read16(script_cursor);
		OBJECT(current_object)->eggman_speech.script = script_cursor + 2u;
		OBJECT(current_object)->eggman_speech.frame_count = 0;
		play_sound(sound_id, &r1, &r2);
		return;
	case 0xEu:
		return;
	default:
		unknown_code(0x85794u + command);
	}
	screen_x = mem_read16(script_cursor);
	script_cursor += 2u;
	screen_y = mem_read16(script_cursor);
	script_cursor += 2u;
	kind = mem_read16(script_cursor);
	script_cursor += 2u;
	frames = mem_read16(script_cursor);
	OBJECT(current_object)->eggman_speech.script = script_cursor + 2u;
	OBJECT(current_object)->eggman_speech.frame_count = 0;
	r11 = spawn_message_box(screen_x, screen_y, kind, frames, 0u);
}

void scenery_sprite_start(void)
{
	uint32_t block_offset = 0, cursor = 0, ignored = 0, kind_record = 0, sprite_kind = 0, y = 0;

	sprite_kind = OBJECT(current_object)->sprite_frame;
	scenery_sprite_kind = sprite_kind;
	kind_record = scenery_sprite_kinds[sprite_kind] + scenery_sprite_kinds_ADDRESS;
	OBJECT(current_object)->sprite_flags = ROM16(kind_record);
	kind_record += 2u;
	OBJECT(current_object)->sprite_frame = ROM16(kind_record);
	kind_record += 2u;
	OBJECT(current_object)->sprite_palette = ROM16(kind_record);
	kind_record += 2u;
	OBJECT(current_object)->scenery_sprite.depth_offset = ROM16(kind_record);
	OBJECT(current_object)->handler = ROM32(kind_record + 2u);
	OBJECT(current_object)->sprite_offset_x = 0;
	OBJECT(current_object)->sprite_offset_y = 0;
	OBJECT(current_object)->flags = 0u;
	OBJECT(current_object)->scenery_sprite.unused_stage = stage;
	if (scenery_sprite_kind != 0xDu)
	{
		find_floor(&sprite_kind, &y, &block_offset, &ignored, &cursor);
		OBJECT(current_object)->z = OBJECT(current_object)->floor_z;
	}
	else
		OBJECT(current_object)->z = 0x8D0000;
	project_object(&ignored, &ignored);
}

void scenery_sprite_draw_until_you_failed(void)
{
	if (current_game_mode != 0x20u || game_mode_timer < 0x3C)
	{
		scenery_sprite_draw();
		return;
	}
	delete_current_object();
}

void scenery_sprite_draw(void)
{
	uint32_t r1 = 0, depth = 0, ignored = 0;

	if (map_screen_active == 0u)
	{
		update_screen_position(&ignored, &r1);
		depth = (uint16_t)(OBJECT(current_object)->screen_y + OBJECT(current_object)->scenery_sprite.depth_offset);
		queue_sprite_at_depth(&ignored, &depth);
		return;
	}
	delete_current_object();
}

void scenery_sprite_draw_until_offscreen(void)
{
	uint32_t r1 = 0, depth = 0, ignored = 0, off_screen = 0;

	off_screen = check_left_screen();
	if (off_screen == 0u)
	{
		update_screen_position(&ignored, &r1);
		depth = (uint16_t)(OBJECT(current_object)->screen_y + OBJECT(current_object)->scenery_sprite.depth_offset);
		queue_sprite_at_depth(&ignored, &depth);
		return;
	}
	delete_current_object();
}

void scenery_sprite_draw_wide_margin(void)
{
	uint32_t depth = 0, ignored = 0, margin = 0, off_screen = 0;

	margin = 0x50u;
	off_screen = check_left_screen_margin((uint16_t)margin);
	if (off_screen == 0u)
	{
		update_screen_position(&ignored, &margin);
		depth = (uint16_t)(OBJECT(current_object)->screen_y + OBJECT(current_object)->scenery_sprite.depth_offset);
		queue_sprite_at_depth(&ignored, &depth);
		return;
	}
	delete_current_object();
}

void scenery_sprite_cycle_palette_start(void)
{
	OBJECT(current_object)->scenery_sprite.palette_index = 0u;
	OBJECT(current_object)->scenery_sprite.palette_delay = 6u;
	OBJECT(current_object)->scenery_sprite.palette_timer = 0u;
	OBJECT(current_object)->scenery_sprite.palette_list = scenery_sprite_palettes_ADDRESS;
	OBJECT(current_object)->sprite_offset_x = 0;
	OBJECT(current_object)->sprite_offset_y = -0x10;
	OBJECT(current_object)->handler = scenery_sprite_cycle_palette_ADDRESS;
}

void scenery_sprite_cycle_palette(void)
{
	uint32_t r1 = 0, depth = 0, ignored = 0;

	if (map_screen_active == 0u)
	{
		animate_sign();
		update_screen_position(&ignored, &r1);
		depth = (uint16_t)(OBJECT(current_object)->screen_y + OBJECT(current_object)->scenery_sprite.depth_offset);
		queue_sprite_at_depth(&ignored, &depth);
		return;
	}
	delete_current_object();
}

void fire_vent_spawner_start(void)
{
	fire_vent_cursor = fire_vent_list_ADDRESS;
	OBJECT(current_object)->handler = fire_vent_spawner_update_ADDRESS;
	fire_vent_spawner_update();
}

void fire_vent_spawner_update(void)
{
	uint32_t free_count = 0, object = 0, vent_record = 0;
	uint16_t spawn_position = 0;

	vent_record = fire_vent_cursor;
	spawn_position = ROM16(vent_record);
	vent_record += 2u;
	if (scroll_position < spawn_position)
		return;
	if ((int16_t)ROM16(vent_record + 6u) < 0)
	{
		delete_current_object();
		return;
	}
	allocate_object(&free_count, &object);
	if (free_count != 0u)
	{
		OBJECT(object)->x_int = ROM16(vent_record);
		OBJECT(object)->x_fraction = 0u;
		vent_record += 2u;
		OBJECT(object)->y_int = ROM16(vent_record);
		OBJECT(object)->y_fraction = 0u;
		vent_record += 2u;
		OBJECT(object)->z_int = ROM16(vent_record);
		OBJECT(object)->z_fraction = 0u;
		OBJECT(object)->fire_vent.blast_delay = ROM16(vent_record + 2u);
		OBJECT(object)->sprite_flags = 0x1006u;
		OBJECT(object)->sprite_frame = 0x6A48u;
		OBJECT(object)->sprite_palette = 0x9110u;
		OBJECT(object)->flags = 2u;
		OBJECT(object)->handler = fire_vent_start_ADDRESS;
		OBJECT(object)->fire_vent.burst_state = 0u;
		OBJECT(object)->fire_vent.palette_list = fire_vent_glow_palettes_ADDRESS;
		OBJECT(object)->fire_vent.palette_index = 0u;
		OBJECT(object)->fire_vent.palette_delay = 8u;
		OBJECT(object)->fire_vent.palette_timer = 0u;
		OBJECT(object)->sprite_offset_x = 0;
		OBJECT(object)->sprite_offset_y = 0x10;
		OBJECT(object)->kind = 0x22u;
		OBJECT(object)->hitbox_size_x = 0x90u;
		OBJECT(object)->hitbox_size_y = 0x48u;
		OBJECT(object)->hitbox_height = 0x28u;
		OBJECT(object)->hitbox_offset_x = -0x48;
		OBJECT(object)->hitbox_offset_y = -8;
		fire_vent_cursor += 0xAu;
	}
}

void fire_vent_start(void)
{
	uint32_t ignored = 0;

	project_object(&ignored, &ignored);
	OBJECT(current_object)->handler = fire_vent_update_ADDRESS;
	fire_vent_update();
}

void fire_vent_update(void)
{
	uint32_t zero = 0, negative = 0, overflow = 0;
	uint8_t sound_id = 0;
	uint32_t r2 = 0, depth = 0, free_count = 0, ignored = 0, margin = 0, object = 0, off_screen = 0;
	uint16_t blast_delay = 0, next_delay = 0;

	margin = 0x40u;
	off_screen = check_left_screen_margin((uint16_t)margin);
	if (off_screen != 0u)
	{
		delete_current_object();
		return;
	}
	if ((int8_t)OBJECT(current_object)->fire_vent.palette_index >= 0)
	{
		if (OBJECT(current_object)->fire_vent.blast_delay <= 0x1E)
			OBJECT(current_object)->fire_vent.palette_delay = 2u;
		animate_sign();
	}
	if ((uint16_t)OBJECT(current_object)->fire_vent.blast_delay == 0u)
	{
	fire_vent_blast:
		OBJECT(current_object)->fire_vent.blast_delay = -1;
		OBJECT(current_object)->fire_vent.burst_state = 1u;
		OBJECT(current_object)->sprite_frame = 0x6A50u;
		OBJECT(current_object)->sprite_palette = 0x9120u;
		OBJECT(current_object)->fire_vent.palette_index = 0xFFu;
		allocate_object(&free_count, &object);
		if (free_count != 0u)
		{
			OBJECT(object)->sprite_flags = 0x1009u;
			OBJECT(object)->sprite_palette = 0x90B0u;
			OBJECT(object)->sprite_offset_x = 0;
			OBJECT(object)->sprite_offset_y = 0;
			OBJECT(object)->flags = 0u;
			OBJECT(object)->handler = fire_vent_burst_animate_ADDRESS;
			OBJECT(object)->parent = current_object;
			OBJECT(object)->animation = fire_vent_blast_animation_ADDRESS;
			OBJECT(object)->animation_frame = 0u;
			OBJECT(object)->animation_speed = 3u;
			OBJECT(object)->animation_timer = 0;
			OBJECT(object)->projected_x = OBJECT(current_object)->projected_x;
			OBJECT(object)->projected_x += 0x3Au;
			OBJECT(object)->anchor_x = OBJECT(object)->projected_x;
			OBJECT(object)->projected_y = OBJECT(current_object)->projected_y;
			OBJECT(object)->projected_y_int -= 0x31u;
			OBJECT(object)->anchor_y = OBJECT(object)->projected_y_int;
			OBJECT(object)->z = OBJECT(current_object)->z;
			sound_id = 0xAu;
			play_sound(sound_id, &margin, &r2);
		}
	}
	else if (OBJECT(current_object)->screen_y >= 0x18)
	{
		blast_delay = OBJECT(current_object)->fire_vent.blast_delay;
		next_delay = blast_delay - 1u;
		overflow = blast_delay == 0x8000u;
		zero = next_delay == 0u;
		negative = next_delay >> 15;
		OBJECT(current_object)->fire_vent.blast_delay = next_delay;
		if (blast_delay == 1u)
			goto fire_vent_blast;
		if ((negative ^ overflow) | zero)
			OBJECT(current_object)->fire_vent.blast_delay = -1;
	}
	update_screen_position(&ignored, &margin);
	if (OBJECT(current_object)->fire_vent.burst_state == 0u)
	{
		depth = (uint16_t)(OBJECT(current_object)->screen_y - 0x48u);
		queue_sprite_at_depth(&ignored, &depth);
		return;
	}
	if ((int8_t)OBJECT(current_object)->fire_vent.burst_state < 0)
	{
		OBJECT(current_object)->fire_vent.burst_state = 1u;
		OBJECT(current_object)->kind = 0xCu;
	}
	add_to_touchable_list(ignored);
	depth = (uint16_t)(OBJECT(current_object)->screen_y - 0x48u);
	queue_sprite_at_depth(&ignored, &depth);
}

void fire_vent_burst_animate(void)
{
	uint32_t r1 = 0, r7 = 0, at_last_step = 0, depth = 0, ignored = 0;

	animate(&r1, &r7, &at_last_step);
	if ((uint8_t)OBJECT(current_object)->animation_timer == OBJECT(current_object)->animation_speed)
		OBJECT(current_object)->sprite_palette = fire_vent_blast_palettes[OBJECT(current_object)->animation_frame];
	if ((uint8_t)at_last_step == 0u)
	{
		update_screen_position(&ignored, &r1);
		depth = (uint16_t)(OBJECT(current_object)->screen_y + 0x68u);
		queue_sprite_at_depth(&ignored, &depth);
		return;
	}
	AS8(OBJECT(LINKED(current_object, OBJECT(current_object)->parent))->var_88) = 0xFFu;
	delete_current_object();
}

void group_leader_hold_start(void)
{
	uint32_t held_flag = 0, hold_record = 0;

	hold_record = group_leader_hold_records[OBJECT(current_object)->sprite_frame] + group_leader_hold_records_ADDRESS;
	OBJECT(current_object)->group_leader_hold.leader = ROM8(hold_record);
	hold_record++;
	OBJECT(current_object)->direction = ROM8(hold_record);
	hold_record++;
	OBJECT(current_object)->group_leader_hold.coordinate = ROM16(hold_record);
	hold_record += 2u;
	OBJECT(current_object)->group_leader_hold.end_position = ROM16(hold_record);
	OBJECT(current_object)->group_leader_hold.which_flag = ROM16(hold_record + 2u);
	held_flag = group_leader_0_held_ADDRESS;
	if (AS8(OBJECT(current_object)->group_leader_hold.which_flag) != 0u)
		held_flag = group_leaders_1_3_held_ADDRESS;
	mem_write8(held_flag, 0xFFu);
	OBJECT(current_object)->handler = group_leader_hold_wait_ADDRESS;
}

void group_leader_hold_wait(void)
{
	uint32_t held_flag = 0, object = 0;

	if (scroll_position < OBJECT(current_object)->group_leader_hold.end_position)
	{
		object = (current_object & ~0xFFFFu) | group_leaders[OBJECT(current_object)->group_leader_hold.leader];
		switch (OBJECT(current_object)->direction)
		{
		case 0:
		case 4:
			if (OBJECT(current_object)->group_leader_hold.coordinate < OBJECT(object)->y_int)
				return;
			break;
		case 1:
			if (OBJECT(object)->x_int < OBJECT(current_object)->group_leader_hold.coordinate)
				return;
			break;
		case 2:
			if (OBJECT(object)->y_int < OBJECT(current_object)->group_leader_hold.coordinate)
				return;
			break;
		case 3:
			if (OBJECT(current_object)->group_leader_hold.coordinate < OBJECT(object)->x_int)
				return;
			break;
		default:
			unknown_code(0x85BE6u + OBJECT(current_object)->direction * 2u);
		}
	}
	held_flag = group_leader_0_held_ADDRESS;
	if (OBJECT(current_object)->group_leader_hold.which_flag != 0u)
		held_flag = group_leaders_1_3_held_ADDRESS;
	mem_write8(held_flag, 0u);
	delete_current_object();
}

void collapsing_bridge_plank_update(void)
{
	uint8_t sound_id = 0, start_frame = 0;
	uint16_t x = 0, y = 0;
	uint32_t r1 = 0, r2 = 0, angle_whole = 0, depth = 0, ignored = 0, kind = 0, object = 0, plank_index = 0, sag = 0, sag_table = 0, sign = 0, swing_dx = 0, swing_dz = 0, z = 0;

	switch (OBJECT(current_object)->collapsing_bridge_plank.state)
	{
	case 0:
	case 3:
		OBJECT(current_object)->collapsing_bridge_plank.state++;
		OBJECT(current_object)->collapsing_bridge_plank.angle = 0x3000000u;
		OBJECT(current_object)->sprite_offset_x = -4;
		OBJECT(current_object)->sprite_offset_y = -4;
		OBJECT(current_object)->collapsing_bridge_plank.angle_speed = 0;
		OBJECT(current_object)->collapsing_bridge_plank.swing_count = 0u;
		OBJECT(current_object)->collapsing_bridge_plank.plank_index--;
		OBJECT(current_object)->collapsing_bridge_plank.roll_timer = 0u;
		OBJECT(current_object)->collapsing_bridge_plank.roll_frames = OBJECT(current_object)->collapsing_bridge_plank.plank_index * 2u;
		OBJECT(current_object)->animation = collapsing_bridge_plank_animation_ADDRESS;
		start_frame = OBJECT(current_object)->collapsing_bridge_plank.plank_index & 7u;
		OBJECT(current_object)->animation_frame = start_frame;
		OBJECT(current_object)->animation_speed = 3u;
		OBJECT(current_object)->animation_timer = start_frame & 3u;
		if (OBJECT(current_object)->collapsing_bridge_plank.plank_index == 0u)
		{
			bridge_swing_angle = 0;
			bridge_swing_speed = 0;
		}
		project_object(&ignored, &ignored);
		return;
	case 1:
		OBJECT(current_object)->previous_floor_z = OBJECT(current_object)->floor_z;
		plank_index = OBJECT(current_object)->collapsing_bridge_plank.plank_index;
		sag_table = bridge_0_sag_ADDRESS;
		if (OBJECT(current_object)->collapsing_bridge_plank.bridge != 0u)
			sag_table = bridge_1_sag_ADDRESS;
		sag = mem_read16(sag_table + plank_index * 2u);
		if ((sag & ~0xFFu) == 0u)
			sag = 0u;
		OBJECT(current_object)->previous_floor_z += sag << 8;
		OBJECT(current_object)->speed_x += (int32_t)((uint32_t)OBJECT(current_object)->previous_floor_z - OBJECT(current_object)->speed_x) >> 2;
		OBJECT(current_object)->z_int = OBJECT(current_object)->speed_x_int;
		update_screen_position(&ignored, &sag);
		if (event_1c_riding != 0u)
		{
			OBJECT(current_object)->collapsing_bridge_plank.state++;
			if (OBJECT(current_object)->collapsing_bridge_plank.plank_index == 0u)
			{
				sound_id = 9u;
				play_sound(sound_id, &sag, &r2);
			}
		}
		if ((uint16_t)(OBJECT(current_object)->screen_x + 0x50u) >= 0x1B0u)
			return;
		add_to_platform_list();
		depth = (uint16_t)(((int32_t)((uint32_t)OBJECT(current_object)->projected_y + OBJECT(current_object)->floor_z) >> 16) - view_y + OBJECT(current_object)->collapsing_bridge_plank.depth_offset);
		OBJECT(current_object)->collapsing_bridge_plank.draw_depth = depth;
		queue_sprite_at_depth(&ignored, &depth);
		return;
	case 2:
		if (scroll_position >= 0x9000u)
		{
			delete_current_object();
			return;
		}
		if (OBJECT(current_object)->collapsing_bridge_plank.plank_index != 0u)
		{
			object = (current_object & ~0xFFFFu) | OBJECT(current_object)->collapsing_bridge_plank.previous_plank;
			OBJECT(current_object)->collapsing_bridge_plank.angle = AS32(OBJECT(object)->var_80);
			OBJECT(current_object)->collapsing_bridge_plank.angle += bridge_swing_angle;
			OBJECT(current_object)->collapsing_bridge_plank.angle_whole &= 0x3FFu;
			if (OBJECT(current_object)->collapsing_bridge_plank.angle_whole < 0x200)
				OBJECT(current_object)->collapsing_bridge_plank.angle = 0x2000000u;
			angle_whole = (uint16_t)OBJECT(current_object)->collapsing_bridge_plank.angle_whole;
			swing_dx = sine_table[angle_whole] * 0x40u;
			OBJECT(current_object)->x = OBJECT(object)->x;
			OBJECT(current_object)->x += swing_dx;
			swing_dz = cosine_table[angle_whole] * 0x40u;
			OBJECT(current_object)->z = OBJECT(object)->z;
			OBJECT(current_object)->z -= swing_dz;
			OBJECT(current_object)->flags &= 0xFFFFFFFEu;
			AS16(OBJECT(current_object)->flags) |= AS16(OBJECT(object)->flags) & 1u;
			OBJECT(current_object)->collapsing_bridge_plank.swing_count = AS8(OBJECT(object)->var_8A);
		}
		else if ((OBJECT(current_object)->flags & 1u) == 0u)
		{
			if (OBJECT(current_object)->collapsing_bridge_plank.swing_count != 0u)
			{
				bridge_swing_speed -= 0x200u;
				bridge_swing_angle += bridge_swing_speed;
				if ((int16_t)AT16(bridge_swing_speed, 2) < 0 && bridge_swing_angle <= 0)
				{
					OBJECT(current_object)->collapsing_bridge_plank.swing_count++;
					if ((int8_t)OBJECT(current_object)->collapsing_bridge_plank.swing_count >= 6)
					{
						OBJECT(current_object)->flags |= 1u;
						event_1c_ride_phase = 0u;
						x = 0xBA0u;
						y = 0xEA4u;
						kind = 0x16u;
						z = 0x660u;
						spawn_sign(x, y, (uint16_t)kind, (uint16_t)z, &swing_dz, &sign);
					}
					bridge_swing_speed = 0u - bridge_swing_speed;
					bridge_swing_speed = (int32_t)(bridge_swing_speed * 0x5Eu) >> 8;
					sound_id = 0x20u;
					play_sound(sound_id, &swing_dz, &r2);
				}
			}
			else
			{
				OBJECT(current_object)->collapsing_bridge_plank.angle_speed -= 0x3000u;
				OBJECT(current_object)->collapsing_bridge_plank.angle += OBJECT(current_object)->collapsing_bridge_plank.angle_speed;
				bridge_head_angle = OBJECT(current_object)->collapsing_bridge_plank.angle_whole;
				if (OBJECT(current_object)->collapsing_bridge_plank.angle_whole <= 0x200)
				{
					OBJECT(current_object)->collapsing_bridge_plank.angle = 0x2000000u;
					OBJECT(current_object)->collapsing_bridge_plank.angle_speed = 0;
					OBJECT(current_object)->collapsing_bridge_plank.swing_count++;
					bridge_swing_speed = -0x10000;
				}
				bridge_swing_angle += 0x2000u;
			}
		}
		if ((int8_t)OBJECT(current_object)->collapsing_bridge_plank.swing_count < 3)
			animate(&swing_dz, &kind, &z);
		else
		{
			if ((int16_t)OBJECT(current_object)->collapsing_bridge_plank.roll_timer < (int16_t)OBJECT(current_object)->collapsing_bridge_plank.roll_frames)
			{
				OBJECT(current_object)->collapsing_bridge_plank.roll_timer++;
				animate(&swing_dz, &kind, &z);
				project_object(&ignored, &r1);
				if (OBJECT(current_object)->screen_y > 0xE0)
					return;
				if ((int8_t)OBJECT(current_object)->collapsing_bridge_plank.swing_count < 1)
				{
					r1 = queue_sprite(r1);
					return;
				}
				depth = 0u;
				queue_sprite_at_depth(&ignored, &depth);
				return;
			}
			OBJECT(current_object)->sprite_frame = mem_read16(OBJECT(current_object)->animation + 2u);
		}
		project_object(&ignored, &r1);
		if (OBJECT(current_object)->screen_y > 0xE0)
			return;
		if ((int8_t)OBJECT(current_object)->collapsing_bridge_plank.swing_count < 1)
		{
			r1 = queue_sprite(r1);
			return;
		}
		depth = 0u;
		queue_sprite_at_depth(&ignored, &depth);
		return;
	default:
		unknown_code(0x85F8Eu + OBJECT(current_object)->collapsing_bridge_plank.state * 3u);
	}
}

void burning_boulder_start(void)
{
	uint32_t r0 = 0, block_offset = 0, boulder_record = 0, ignored = 0, object = 0, object_2 = 0, zone_index = 0;
	uint8_t flames_left_before = 0;

	boulder_record = burning_boulder_records_ADDRESS + OBJECT(current_object)->sprite_frame * 2u * 4u;
	OBJECT(current_object)->x_int = ROM16(boulder_record);
	OBJECT(current_object)->x_fraction = 0u;
	boulder_record += 2u;
	OBJECT(current_object)->burning_boulder.start_delay = ROM16(boulder_record);
	OBJECT(current_object)->burning_boulder.path = ROM32(boulder_record + 2u);
	OBJECT(current_object)->y = 0x3E00000;
	OBJECT(current_object)->y_int -= OBJECT(current_object)->sprite_frame * 0x60u;
	OBJECT(current_object)->sprite_flags = 0x1004u;
	OBJECT(current_object)->sprite_frame = 0xAA58u;
	OBJECT(current_object)->sprite_palette = 0x51B0u;
	OBJECT(current_object)->kind = 0x14u;
	OBJECT(current_object)->flags = 2u;
	OBJECT(current_object)->hitbox_size_x = 0x90u;
	OBJECT(current_object)->hitbox_size_y = 0x90u;
	OBJECT(current_object)->burning_boulder.state = 0u;
	OBJECT(current_object)->burning_boulder.depth_offset = 0x20u;
	OBJECT(current_object)->speed_x = 0;
	OBJECT(current_object)->direction = 0x200u;
	OBJECT(current_object)->hitbox_height = 0xA0u;
	OBJECT(current_object)->sprite_offset_x = 0;
	OBJECT(current_object)->sprite_offset_y = 0;
	OBJECT(current_object)->hitbox_offset_x = -0x20;
	OBJECT(current_object)->hitbox_offset_y = -0x70;
	OBJECT(current_object)->speed_z = 0;
	OBJECT(current_object)->burning_boulder.rumble_timer = 0x3Cu;
	burning_boulder_released = 0u;
	burning_boulder_release_timer = 0;
	OBJECT(current_object)->animation = burning_boulder_roll_animation_ADDRESS;
	OBJECT(current_object)->animation_frame = 0u;
	OBJECT(current_object)->animation_speed = 2u;
	OBJECT(current_object)->animation_timer = 0;
	if (AS16(OBJECT(current_object)->burning_boulder.path) == 0xFFFFu)
	{
		OBJECT(current_object)->hitbox_size_x = 0x120u;
		zone_index = 2u;
		object = spawn_chaser_back_zone(zone_index, 0u);
	}
	if (AS16(OBJECT(current_object)->burning_boulder.path) != 0xFFFFu)
		lead_burning_boulder = current_object;
	find_floor(&r0, &zone_index, &block_offset, &ignored, &object);
	OBJECT(current_object)->z = OBJECT(current_object)->floor_z;
	OBJECT(current_object)->z_int -= 0x70u;
	project_object(&ignored, &ignored);
	OBJECT(current_object)->burning_boulder.holding_back = 0u;
	OBJECT(current_object)->burning_boulder.wait_timer = 0;
	if (free_object_stack < 4)
		return;
	boulder_flames_left = 4;
	boulder_flame_number = 0u;
	object_2 = current_object;
	do
	{
		allocate_object(&ignored, &object);
		OBJECT(object)->sprite_flags = 0x100Au;
		OBJECT(object)->sprite_palette = 0x51C0u;
		OBJECT(object)->flags = 0u;
		OBJECT(object)->sprite_offset_x = 0;
		OBJECT(object)->sprite_offset_y = 0;
		OBJECT(object)->kind = boulder_flame_number;
		boulder_flame_number++;
		OBJECT(object)->handler = burning_boulder_flame_start_ADDRESS;
		OBJECT(object)->parent = current_object;
		OBJECT(object_2)->child = object;
		object_2 = object;
		flames_left_before = boulder_flames_left;
		boulder_flames_left = flames_left_before - 1u;
	} while ((int8_t)flames_left_before > 1);
	OBJECT(current_object)->handler = burning_boulder_update_ADDRESS;
}

void burning_boulder_update(void)
{
	uint32_t zero = 0;
	uint8_t sound_id = 0;
	uint16_t animation_speed = 0, roll_speed = 0;
	uint32_t r0 = 0, r1 = 0, r2 = 0, r7 = 0, cursor = 0, depth = 0, drop_step = 0, flames_left = 0, floor_slope = 0, ignored = 0, object = 0, path_cursor = 0, slope_factor = 0, speed_level = 0, step_y = 0;
	uint8_t hold_timer = 0;
	uint16_t start_delay = 0, wait_timer = 0, record_scroll = 0, record_y = 0, next_slow_timer = 0, rumble_before = 0, rumble_after = 0, start_delay_2 = 0;

	switch (OBJECT(current_object)->burning_boulder.state)
	{
	case 0:
	case 6:
		start_delay = OBJECT(current_object)->burning_boulder.start_delay;
		OBJECT(current_object)->burning_boulder.start_delay = start_delay - 1u;
		if ((int16_t)start_delay > 1)
			return;
		OBJECT(current_object)->burning_boulder.start_delay = 0;
		OBJECT(current_object)->speed_z += 0x6000u;
		OBJECT(current_object)->z += OBJECT(current_object)->speed_z;
		find_floor(&r0, &step_y, &r2, &ignored, &cursor);
		if (OBJECT(current_object)->floor_z_int <= OBJECT(current_object)->z_int)
		{
			OBJECT(current_object)->z = OBJECT(current_object)->floor_z;
			OBJECT(current_object)->burning_boulder.state++;
			if (OBJECT(current_object)->burning_boulder.path_high == 0xFFFFu)
				OBJECT(current_object)->burning_boulder.state = 2u;
			camera_shake_active = 1u;
			sound_id = 7u;
			play_sound(sound_id, &step_y, &r2);
		}
		project_object(&ignored, &ignored);
		add_to_zone_list(ignored);
		depth = (uint16_t)(OBJECT(current_object)->screen_y - OBJECT(current_object)->burning_boulder.depth_offset);
		if (OBJECT(current_object)->burning_boulder.path_high == 0xFFFFu)
			depth = (uint16_t)(depth - 0x20u);
		queue_sprite_at_depth(&ignored, &depth);
		return;
	case 1:
		if (OBJECT(current_object)->burning_boulder.holding_back == 0u)
		{
			if (no_player_in_play != 0u || player_mishap != 0u || player_dropping_in != 0u)
			{
				OBJECT(current_object)->burning_boulder.holding_back = 0xFFu;
				OBJECT(current_object)->burning_boulder.saved_base_speed = OBJECT(current_object)->burning_boulder.base_speed;
				OBJECT(current_object)->burning_boulder.base_speed = 0;
				OBJECT(current_object)->burning_boulder.hold_start_position = scroll_position;
				OBJECT(current_object)->burning_boulder.hold_timer = 0xFFu;
				OBJECT(current_object)->burning_boulder.saved_threshold = OBJECT(current_object)->burning_boulder.screen_threshold;
				OBJECT(current_object)->burning_boulder.screen_threshold = -0x20;
			}
			else
			{
				wait_timer = OBJECT(current_object)->burning_boulder.wait_timer;
				OBJECT(current_object)->burning_boulder.wait_timer = wait_timer - 1u;
				if ((int16_t)wait_timer <= 1)
				{
					OBJECT(current_object)->burning_boulder.wait_timer = 0;
					path_cursor = OBJECT(current_object)->burning_boulder.path;
					record_scroll = mem_read16(path_cursor);
					path_cursor += 2u;
					if (record_scroll <= scroll_position)
					{
						path_cursor += 2u;
						goto burning_boulder_load_record;
					}
					record_y = mem_read16(path_cursor);
					path_cursor += 2u;
					if ((int16_t)record_y <= OBJECT(current_object)->y_int)
					{
					burning_boulder_load_record:
						OBJECT(current_object)->burning_boulder.screen_threshold = mem_read16(path_cursor);
						path_cursor += 2u;
						OBJECT(current_object)->burning_boulder.catch_up_factor = mem_read16(path_cursor);
						path_cursor += 2u;
						OBJECT(current_object)->burning_boulder.base_speed = mem_read16(path_cursor);
						path_cursor += 2u;
						OBJECT(current_object)->burning_boulder.wait_timer = mem_read16(path_cursor);
						OBJECT(current_object)->burning_boulder.path = path_cursor + 2u;
					}
				}
			}
		}
		if (OBJECT(current_object)->burning_boulder.holding_back != 0u)
		{
			OBJECT(current_object)->burning_boulder.rumble_timer = 0x3Cu;
			if ((int16_t)(scroll_position - OBJECT(current_object)->burning_boulder.hold_start_position) >= 0x300)
			{
			burning_boulder_stop_holding_back:
				OBJECT(current_object)->burning_boulder.holding_back = 0u;
				OBJECT(current_object)->burning_boulder.screen_threshold = OBJECT(current_object)->burning_boulder.saved_threshold;
				OBJECT(current_object)->burning_boulder.base_speed = OBJECT(current_object)->burning_boulder.saved_base_speed;
				OBJECT(current_object)->burning_boulder.slow_timer = 0x78;
				OBJECT(current_object)->burning_boulder.rumble_timer = 0u;
			}
			else
			{
				if (no_player_in_play != 0u)
					OBJECT(current_object)->burning_boulder.hold_timer = 0xFFu;
				hold_timer = OBJECT(current_object)->burning_boulder.hold_timer;
				OBJECT(current_object)->burning_boulder.hold_timer = hold_timer - 1u;
				if (hold_timer == 0u)
					goto burning_boulder_stop_holding_back;
			}
		}
		next_slow_timer = OBJECT(current_object)->burning_boulder.slow_timer - 1u;
		OBJECT(current_object)->burning_boulder.slow_timer = next_slow_timer;
		if ((int16_t)next_slow_timer < 0)
			OBJECT(current_object)->burning_boulder.slow_timer = 0;
		if (OBJECT(current_object)->burning_boulder.wait_timer <= 0)
		{
			if (OBJECT(current_object)->burning_boulder.screen_threshold <= OBJECT(current_object)->screen_y)
			{
			burning_boulder_use_base_speed:
				roll_speed = (uint16_t)OBJECT(current_object)->burning_boulder.base_speed;
				if ((uint16_t)OBJECT(current_object)->burning_boulder.slow_timer != 0u)
					roll_speed = 0x10u;
			}
			else
			{
				slope_factor = 0u;
				if (OBJECT(current_object)->floor_slope != 0u)
				{
					floor_slope = OBJECT(current_object)->floor_slope;
					slope_factor = floor_slope_factors[(uint16_t)(floor_slope * 2u)];
				}
				compute_camera_slope_speed(0u, slope_factor);
				roll_speed = (uint16_t)((uint32_t)camera_slope_speed_int * OBJECT(current_object)->burning_boulder.catch_up_factor) >> 8;
				if ((int16_t)roll_speed <= OBJECT(current_object)->burning_boulder.base_speed)
					goto burning_boulder_use_base_speed;
			}
			OBJECT(current_object)->speed_fraction = roll_speed;
			step_y = (uint32_t)(int16_t)OBJECT(current_object)->speed_fraction << 12;
			OBJECT(current_object)->y += step_y;
			OBJECT(current_object)->speed_y = step_y;
		}
		speed_level = OBJECT(current_object)->speed_fraction;
		if (speed_level != 0u)
		{
			speed_level = speed_level >> 4;
			if ((int16_t)speed_level > 5)
				speed_level = 5u;
			OBJECT(current_object)->animation_speed = burning_boulder_roll_rates[speed_level];
			animate(&step_y, &r7, &slope_factor);
		}
		find_floor(&speed_level, &step_y, &r2, &ignored, &cursor);
		rumble_before = AS16(OBJECT(current_object)->burning_boulder.rumble_timer);
		rumble_after = rumble_before - 1u;
		zero = rumble_after == 0u;
		AS16(OBJECT(current_object)->burning_boulder.rumble_timer) = rumble_after;
		if ((int16_t)rumble_before <= 1)
		{
			animation_speed = OBJECT(current_object)->animation_speed;
			if (zero == 0u)
			{
				AS16(OBJECT(current_object)->burning_boulder.rumble_timer) = animation_speed * 0xAu;
				sound_id = 0xDu;
				play_sound(sound_id, &step_y, &r2);
			}
		}
		if (scroll_position < 0x3300u || (int16_t)AT16(OBJECT(current_object)->floor_block, 2) >= 0)
			OBJECT(current_object)->z = OBJECT(current_object)->floor_z;
		else
			OBJECT(current_object)->burning_boulder.state = 3u;
		project_object(&ignored, &ignored);
		add_to_zone_list(ignored);
		depth = (uint16_t)(OBJECT(current_object)->screen_y - OBJECT(current_object)->burning_boulder.depth_offset);
		OBJECT(current_object)->burning_boulder.draw_depth = depth;
		queue_sprite_at_depth(&ignored, &depth);
		return;
	case 2:
		object = (current_object & ~0xFFFFu) | lead_burning_boulder;
		step_y = (int16_t)OBJECT(object)->speed_fraction;
		if ((int8_t)AS8(OBJECT(object)->var_96) >= 3)
			step_y = (int16_t)OBJECT(current_object)->speed_fraction;
		step_y = step_y << 12;
		OBJECT(current_object)->y += step_y;
		OBJECT(current_object)->speed_y = step_y;
		speed_level = OBJECT(object)->speed_fraction;
		OBJECT(current_object)->speed_fraction = speed_level;
		if (speed_level != 0u)
		{
			speed_level = speed_level >> 4;
			if ((int16_t)speed_level > 5)
				speed_level = 5u;
			OBJECT(current_object)->animation_speed = burning_boulder_roll_rates[speed_level];
			animate(&step_y, &r7, &slope_factor);
		}
		find_floor(&speed_level, &step_y, &r2, &ignored, &cursor);
		if ((int16_t)AT16(OBJECT(current_object)->floor_block, 2) >= 0)
			OBJECT(current_object)->z = OBJECT(current_object)->floor_z;
		if (AS8(OBJECT((current_object & ~0xFFFFu) | lead_burning_boulder)->var_96) == 3u)
			OBJECT(current_object)->burning_boulder.state = 3u;
		project_object(&ignored, &ignored);
		add_to_zone_list(ignored);
		depth = (uint16_t)(OBJECT(current_object)->screen_y - OBJECT(current_object)->burning_boulder.depth_offset);
		OBJECT(current_object)->burning_boulder.draw_depth = depth;
		queue_sprite_at_depth(&ignored, &depth);
		return;
	case 3:
		drop_step = 0x60000u;
		if (scroll_position >= 0x4800u)
			drop_step = 0u;
		OBJECT(current_object)->z += drop_step;
		OBJECT(current_object)->x += 0x167D2u;
		OBJECT(current_object)->y += 0x1638Eu;
		OBJECT(current_object)->animation_speed = 2u;
		animate(&step_y, &r7, &slope_factor);
		project_object(&ignored, &ignored);
		if (scroll_position < 0x4C50u && OBJECT(current_object)->screen_y > -0x10)
		{
			depth = 0u;
			queue_sprite_at_depth(&ignored, &depth);
			return;
		}
		OBJECT(current_object)->burning_boulder.state++;
		OBJECT(current_object)->burning_boulder.start_delay = 0x1E;
		flames_left = 4u;
		do
		{
			current_object = LINKED(current_object, OBJECT(current_object)->child);
			OBJECT(current_object)->flags |= 0x400u;
			flames_left--;
		} while (flames_left != 0u);
		current_object = object_cursor;
		if (AS16(OBJECT(current_object)->burning_boulder.path) != 0xFFFFu)
		{
			depth = 0u;
			queue_sprite_at_depth(&ignored, &depth);
			return;
		}
		delete_current_object();
		return;
	case 4:
		burning_boulder_release_timer++;
		if (burning_boulder_release_timer >= 0x258)
			burning_boulder_released = 0xFFu;
		if (burning_boulder_released != 0u)
		{
			start_delay_2 = OBJECT(current_object)->burning_boulder.start_delay;
			OBJECT(current_object)->burning_boulder.start_delay = start_delay_2 - 1u;
			if ((int16_t)start_delay_2 <= 1)
			{
				OBJECT(current_object)->burning_boulder.state++;
				OBJECT(current_object)->flags &= ~1u;
				OBJECT(current_object)->x = 0x1800000;
				OBJECT(current_object)->y = 0xEF00000;
				OBJECT(current_object)->z = 0x9700000;
				OBJECT(current_object)->speed_z = 0xC0000;
				project_object(&ignored, &ignored);
				OBJECT(current_object)->sprite_palette = 0x1240u;
			}
		}
		return;
	case 5:
		if (OBJECT(current_object)->screen_y >= 0x150)
		{
			delete_current_object();
			return;
		}
		if ((OBJECT(current_object)->flags & 1u) == 0u)
		{
			if (OBJECT(current_object)->z_int >= 0xA28)
			{
				OBJECT(current_object)->flags |= 1u;
				camera_shake_active = 1u;
				sound_id = 0xAu;
				play_sound(sound_id, &step_y, &r2);
			}
			OBJECT(current_object)->speed_z += 0x6000u;
			OBJECT(current_object)->z += OBJECT(current_object)->speed_z;
		}
		if (OBJECT(current_object)->z_int >= 0x9E0)
			seesaw_launched = 0xFFu;
		project_object(&ignored, &r1);
		r1 = queue_sprite(r1);
		return;
	default:
		unknown_code(0x863F8u + OBJECT(current_object)->burning_boulder.state * 3u);
	}
}

void burning_boulder_flame_start(void)
{
	uint32_t layout_record = 0;

	layout_record = OBJECT(current_object)->kind * 6u + burning_boulder_flame_layout_ADDRESS;
	OBJECT(current_object)->burning_boulder_flame.offset_x = ROM16(layout_record);
	layout_record += 2u;
	OBJECT(current_object)->burning_boulder_flame.offset_y = ROM16(layout_record);
	OBJECT(current_object)->animation = ROM16(layout_record + 2u) + burning_boulder_flame_animations_ADDRESS;
	OBJECT(current_object)->animation_frame = 0xFFu;
	OBJECT(current_object)->animation_speed = 1u;
	OBJECT(current_object)->animation_timer = 0;
	OBJECT(current_object)->burning_boulder_flame.pause_timer = 0;
	OBJECT(current_object)->handler = burning_boulder_flame_flicker_ADDRESS;
}

void burning_boulder_flame_flicker(void)
{
	uint32_t r8 = 0, depth = 0, frame = 0, ignored = 0, object = 0, random_bits = 0, wrapped = 0;
	uint16_t pause_timer = 0;

	if ((OBJECT(current_object)->flags & 0x400u) != 0u)
	{
		delete_current_object();
		return;
	}
	pause_timer = OBJECT(current_object)->burning_boulder_flame.pause_timer;
	OBJECT(current_object)->burning_boulder_flame.pause_timer = pause_timer - 1u;
	if ((int16_t)pause_timer <= 1)
	{
		OBJECT(current_object)->burning_boulder_flame.pause_timer = 0;
		if ((int16_t)OBJECT(LINKED(current_object, OBJECT(current_object)->parent))->speed_fraction <= 0)
			return;
		object = LINKED(current_object, OBJECT(current_object)->parent);
		OBJECT(current_object)->screen_x = OBJECT(object)->screen_x;
		OBJECT(current_object)->screen_y = OBJECT(object)->screen_y;
		OBJECT(current_object)->screen_x += OBJECT(current_object)->burning_boulder_flame.offset_x;
		OBJECT(current_object)->screen_y += OBJECT(current_object)->burning_boulder_flame.offset_y;
		animate(&frame, &wrapped, &r8);
		if ((uint8_t)wrapped == 0u)
		{
			depth = (uint16_t)(AS16(OBJECT(LINKED(current_object, OBJECT(current_object)->parent))->player) + 1u);
			queue_sprite_at_depth(&ignored, &depth);
			return;
		}
		next_random(&random_bits, &ignored, &ignored);
		OBJECT(current_object)->burning_boulder_flame.pause_timer = random_bits & 0x3Fu;
	}
}

void ember_emitter_start(void)
{
	uint32_t r0 = 0, block_offset = 0, cursor = 0, ignored = 0, y = 0;

	OBJECT(current_object)->ember_emitter.spawn_timer = 0;
	OBJECT(current_object)->floor_slope = 0u;
	OBJECT(current_object)->floor_type = 0u;
	OBJECT(current_object)->flags = 0u;
	OBJECT(current_object)->ember_emitter.end_position = ember_emitter_end_positions[OBJECT(current_object)->sprite_frame];
	find_floor(&r0, &y, &block_offset, &ignored, &cursor);
	OBJECT(current_object)->z = OBJECT(current_object)->floor_z;
	OBJECT(current_object)->handler = ember_emitter_move_ADDRESS;
}

void ember_emitter_move(void)
{
	uint8_t random_low = 0;
	uint32_t floor_slope = 0, free_count = 0, ignored = 0, move_step = 0, object = 0, random_bits = 0, slope_factor = 0, speed_entry = 0, travel_direction = 0;
	uint16_t spawn_timer = 0;

	if (OBJECT(current_object)->ember_emitter.end_position <= scroll_position)
	{
		delete_current_object();
		return;
	}
	travel_direction = mem_read8(floor_blocks + 2u + OBJECT(current_object)->floor_block + 0x15u);
	OBJECT(current_object)->ember_emitter.travel_direction = travel_direction;
	switch (travel_direction)
	{
	case 0:
	case 3:
		if (OBJECT(current_object)->screen_x >= 0x40)
		{
		ember_emitter_keep_up:
			slope_factor = 0u;
			if (OBJECT(current_object)->floor_slope != 0u)
			{
				floor_slope = OBJECT(current_object)->floor_slope;
				slope_factor = floor_slope_factors[(uint16_t)(floor_slope * 2u)];
			}
			random_bits = compute_camera_slope_speed(0u, slope_factor);
			move_step = camera_slope_speed >> 7;
			switch (OBJECT(current_object)->ember_emitter.travel_direction)
			{
			case 0:
			case 4:
				OBJECT(current_object)->y -= move_step;
				break;
			case 1:
				OBJECT(current_object)->x += move_step;
				break;
			case 2:
				OBJECT(current_object)->y += move_step;
				break;
			case 3:
				OBJECT(current_object)->x -= move_step;
				break;
			default:
				unknown_code(0x869ABu + OBJECT(current_object)->ember_emitter.travel_direction * 2u);
			}
		}
		break;
	case 1:
	case 2:
	case 4:
		if (OBJECT(current_object)->screen_x <= 0x40)
			goto ember_emitter_keep_up;
		break;
	default:
		unknown_code(0x86958u + travel_direction * 2u);
	}
	if (OBJECT(current_object)->sprite_frame != 3u)
	{
		find_floor(&travel_direction, &floor_slope, &move_step, &random_bits, &object);
		OBJECT(current_object)->z = OBJECT(current_object)->floor_z;
	}
	project_object(&ignored, &ignored);
	spawn_timer = OBJECT(current_object)->ember_emitter.spawn_timer;
	OBJECT(current_object)->ember_emitter.spawn_timer = spawn_timer - 1u;
	if ((int16_t)spawn_timer > 1)
		return;
	allocate_object(&free_count, &object);
	if (free_count != 0u)
	{
		OBJECT(object)->sprite_flags = 0x1000u;
		OBJECT(object)->sprite_palette = 0x51D0u;
		OBJECT(object)->flags = 0u;
		OBJECT(object)->sprite_offset_x = 0;
		OBJECT(object)->sprite_offset_y = 0;
		OBJECT(object)->falling_ember.state = 0u;
		OBJECT(object)->falling_ember.travel_direction = OBJECT(current_object)->ember_emitter.travel_direction;
		OBJECT(object)->handler = falling_ember_update_ADDRESS;
		OBJECT(object)->x = OBJECT(current_object)->x;
		OBJECT(object)->y = OBJECT(current_object)->y;
		OBJECT(object)->z = OBJECT(current_object)->z;
		next_random(&random_bits, &ignored, &ignored);
		random_low = (uint8_t)random_bits;
		random_bits = (random_bits & ~0xFFFFu) | (random_bits & 0x7Fu);
		OBJECT(object)->x_int += random_bits;
		random_bits = random_bits >> 16;
		random_bits = random_bits & 0x7Fu;
		OBJECT(object)->y_int += random_bits;
		speed_entry = falling_ember_speeds_ADDRESS + (random_low & 3u) * 4u;
		OBJECT(object)->speed_z = (uint32_t)(int16_t)ROM16(speed_entry) << 8;
		OBJECT(object)->speed_y = (uint32_t)(int16_t)ROM16(speed_entry + 2u) << 8;
		OBJECT(object)->animation = falling_ember_animation_ADDRESS;
		OBJECT(object)->animation_frame = 0xFFu;
		OBJECT(object)->animation_speed = 2u;
		OBJECT(object)->animation_timer = 0;
	}
	next_random(&random_bits, &ignored, &ignored);
	OBJECT(current_object)->ember_emitter.spawn_timer = random_bits & 0x3Fu;
}

void falling_ember_update(void)
{
	uint32_t r0 = 0, r1 = 0, block_offset = 0, burst_animation = 0, cursor = 0, ignored = 0, off_screen = 0, wrapped = 0;

	switch (OBJECT(current_object)->falling_ember.state)
	{
	case 0:
	case 3:
		OBJECT(current_object)->falling_ember.state++;
		do
		{
			OBJECT(current_object)->z_int -= 0x70u;
			project_object(&ignored, &ignored);
		} while (OBJECT(current_object)->screen_y > 0);
		return;
	case 1:
		off_screen = check_left_screen();
		if (off_screen != 0u)
		{
			delete_current_object();
			return;
		}
		OBJECT(current_object)->y += OBJECT(current_object)->speed_y;
		OBJECT(current_object)->z += OBJECT(current_object)->speed_z;
		animate(&r1, &wrapped, &off_screen);
		find_floor(&r0, &r1, &block_offset, &ignored, &cursor);
		if (OBJECT(current_object)->falling_ember.floor_block_high < 0)
			OBJECT(current_object)->floor_z = 0x5300000;
		project_object(&ignored, &r1);
		if (OBJECT(current_object)->z_int < OBJECT(current_object)->floor_z_int)
		{
			r1 = queue_sprite(r1);
			return;
		}
		OBJECT(current_object)->falling_ember.state++;
		burst_animation = falling_ember_burst_animation_ADDRESS;
		if (OBJECT(current_object)->falling_ember.floor_block_high >= 0 && (OBJECT(current_object)->floor_slope | OBJECT(current_object)->floor_type) != 0u)
			burst_animation = falling_ember_slope_burst_animation_ADDRESS;
		OBJECT(current_object)->animation = burst_animation;
		OBJECT(current_object)->animation_frame = 0xFFu;
		OBJECT(current_object)->animation_speed = 2u;
		OBJECT(current_object)->animation_timer = 0;
		OBJECT(current_object)->z = OBJECT(current_object)->floor_z;
		r1 = queue_sprite(r1);
		return;
	case 2:
		off_screen = check_left_screen();
		if ((uint8_t)off_screen != 0u)
		{
			delete_current_object();
			return;
		}
		animate(&r1, &wrapped, &off_screen);
		if ((uint8_t)wrapped == 0u)
		{
			update_screen_position(&ignored, &r1);
			r1 = queue_sprite(r1);
			return;
		}
		delete_current_object();
		return;
	default:
		unknown_code(0x86AA8u + OBJECT(current_object)->falling_ember.state * 3u);
	}
}

void scene_carrier_start(void)
{
	uint32_t carrier_record = 0, sprite_frame = 0;

	carrier_record = scene_carrier_records[OBJECT(current_object)->sprite_frame] + scene_carrier_records_ADDRESS;
	OBJECT(current_object)->x_int = ROM16(carrier_record);
	OBJECT(current_object)->x_fraction = 0u;
	carrier_record += 2u;
	OBJECT(current_object)->y_int = ROM16(carrier_record);
	OBJECT(current_object)->y_fraction = 0u;
	carrier_record += 2u;
	OBJECT(current_object)->z_int = ROM16(carrier_record);
	OBJECT(current_object)->z_fraction = 0u;
	carrier_record += 2u;
	OBJECT(current_object)->speed_x = ROM32(carrier_record);
	carrier_record += 4u;
	OBJECT(current_object)->speed_y = ROM32(carrier_record);
	carrier_record += 4u;
	OBJECT(current_object)->speed_z = ROM32(carrier_record);
	OBJECT(current_object)->scene_carrier.end_position = ROM16(carrier_record + 4u);
	sprite_frame = OBJECT(current_object)->sprite_frame;
	if ((uint8_t)sprite_frame != 0u)
		player_carriers[(sprite_frame & ~0xFFu) | (uint8_t)(sprite_frame - 1u)] = current_object;
	else
		scene_object = current_object;
	OBJECT(current_object)->handler = scene_carrier_move_ADDRESS;
}

void scene_carrier_move(void)
{
	uint32_t ignored = 0;

	if (OBJECT(current_object)->scene_carrier.end_position <= scroll_position)
	{
		delete_current_object();
		return;
	}
	if (OBJECT(current_object)->sprite_frame == 0u || seesaw_launched != 0u)
	{
		OBJECT(current_object)->z += OBJECT(current_object)->speed_z;
		OBJECT(current_object)->y += OBJECT(current_object)->speed_y;
		OBJECT(current_object)->x += OBJECT(current_object)->speed_x;
	}
	project_object(&ignored, &ignored);
}

void seesaw_start(void)
{
	uint32_t free_count = 0, ignored = 0, part_list = 0;

	OBJECT(current_object)->sprite_flags = 0xD006u;
	OBJECT(current_object)->flags = 0u;
	OBJECT(current_object)->sprite_offset_x = 0;
	OBJECT(current_object)->sprite_offset_y = 0;
	allocate_object_2(&free_count, &ignored, &part_list);
	if (free_count != 0u)
	{
		move_halfwords_backward(part_list, seesaw_parts_ADDRESS, 0xDu);
		OBJECT(current_object)->part_list = part_list;
		OBJECT(current_object)->x = 0x700000;
		OBJECT(current_object)->y = 0xF680000;
		OBJECT(current_object)->z = 0x9E00000;
		project_object(&ignored, &ignored);
		OBJECT(current_object)->animation = seesaw_animation_ADDRESS;
		OBJECT(current_object)->animation_frame = 0u;
		OBJECT(current_object)->animation_speed = 2u;
		OBJECT(current_object)->animation_timer = 0;
		OBJECT(current_object)->handler = seesaw_tilt_ADDRESS;
	}
}

void seesaw_tilt(void)
{
	uint16_t part_list = 0;
	uint32_t r1 = 0, depth = 0, ignored = 0, next_step = 0, step_list = 0;
	uint8_t animation_timer = 0;
	uint16_t last_step = 0;

	if (scroll_position >= 0x4D00u)
	{
		part_list = (uint16_t)OBJECT(current_object)->part_list;
		free_object_2(part_list);
		delete_current_object();
		return;
	}
	if ((int8_t)OBJECT(current_object)->animation_speed >= 0 && seesaw_launched != 0u)
	{
		animation_timer = OBJECT(current_object)->animation_timer;
		OBJECT(current_object)->animation_timer = animation_timer - 1u;
		if ((int8_t)animation_timer > 1)
		{
			update_screen_position(&ignored, &r1);
			depth = 0u;
			queue_sprite_at_depth(&ignored, &depth);
			return;
		}
		OBJECT(current_object)->animation_timer = OBJECT(current_object)->animation_speed;
		next_step = OBJECT(current_object)->animation_frame;
		next_step = (uint8_t)(next_step + 1u);
		step_list = OBJECT(current_object)->animation;
		last_step = mem_read16(step_list);
		step_list += 2u;
		if (next_step == last_step)
			OBJECT(current_object)->animation_speed = 0xFFu;
		OBJECT(current_object)->animation_frame = next_step;
		mem_write16(OBJECT(current_object)->part_list + 0xAu, mem_read16(step_list + next_step * 2u));
	}
	update_screen_position(&ignored, &r1);
	depth = 0u;
	queue_sprite_at_depth(&ignored, &depth);
}

void metal_platform_base_start(void)
{
	uint32_t ignored = 0;

	OBJECT(current_object)->sprite_flags = 0x1002u;
	OBJECT(current_object)->sprite_frame = 0xAFB8u;
	OBJECT(current_object)->sprite_palette = 0x1230u;
	OBJECT(current_object)->flags = 0u;
	OBJECT(current_object)->sprite_offset_x = -6;
	OBJECT(current_object)->sprite_offset_y = 0x1B;
	OBJECT(current_object)->z = 0x5300000;
	project_object(&ignored, &ignored);
	OBJECT(current_object)->handler = metal_platform_base_draw_ADDRESS;
	metal_platform_base_draw();
}

void metal_platform_base_draw(void)
{
	uint16_t unseen_margin = 0;
	uint32_t depth = 0, ignored = 0, margin = 0, off_screen = 0;

	unseen_margin = 0x10u;
	margin = 0x40u;
	off_screen = check_left_screen_margins((uint16_t)margin, unseen_margin);
	if (off_screen == 0u)
	{
		update_screen_position(&ignored, &margin);
		depth = (uint16_t)(OBJECT(current_object)->screen_y - OBJECT(current_object)->sprite_offset_y - 0x28u);
		queue_sprite_at_depth(&ignored, &depth);
		return;
	}
	delete_current_object();
}

void metal_platform_start(void)
{
	uint32_t free_count = 0, ignored = 0, object = 0;

	OBJECT(current_object)->sprite_flags = 0x1002u;
	OBJECT(current_object)->sprite_frame = 0xAFC0u;
	OBJECT(current_object)->sprite_palette = 0x1230u;
	OBJECT(current_object)->flags = 0u;
	OBJECT(current_object)->sprite_offset_x = -6;
	OBJECT(current_object)->sprite_offset_y = 0x1B;
	OBJECT(current_object)->metal_platform.state = 0u;
	OBJECT(current_object)->metal_platform.timer = 0x12C;
	OBJECT(current_object)->z = 0x5300000;
	project_object(&ignored, &ignored);
	allocate_object(&free_count, &object);
	if (free_count != 0u)
	{
		OBJECT(object)->sprite_flags = 0x1001u;
		OBJECT(object)->sprite_frame = 0xAFC8u;
		OBJECT(object)->sprite_palette = 0x1230u;
		OBJECT(object)->flags = 0u;
		OBJECT(object)->sprite_offset_x = 0;
		OBJECT(object)->sprite_offset_y = 0;
		OBJECT(object)->x = 0x1310000;
		OBJECT(object)->y = 0xF240000;
		OBJECT(object)->z = 0x5300000;
		OBJECT(object)->handler = metal_platform_update_ADDRESS;
		OBJECT(object)->metal_platform.state = 3u;
		OBJECT(object)->parent = current_object;
		OBJECT(current_object)->handler = metal_platform_update_ADDRESS;
	}
}

void metal_platform_update(void)
{
	uint32_t r1 = 0, depth = 0, free_count = 0, ignored = 0, object = 0, object_2 = 0;
	uint16_t timer = 0, timer_2 = 0;

	switch (OBJECT(current_object)->metal_platform.state)
	{
	case 0:
	case 4:
		timer = OBJECT(current_object)->metal_platform.timer;
		OBJECT(current_object)->metal_platform.timer = timer - 1u;
		if ((int16_t)timer <= 1)
			OBJECT(current_object)->metal_platform.state++;
		update_screen_position(&ignored, &r1);
		depth = (uint16_t)(OBJECT(current_object)->screen_y - OBJECT(current_object)->sprite_offset_y - 0x28u);
		queue_sprite_at_depth(&ignored, &depth);
		return;
	case 1:
		OBJECT(current_object)->metal_platform.state++;
		metal_platform_lifted = 0xFFu;
		allocate_object(&free_count, &object_2);
		if (free_count != 0u)
			OBJECT(object_2)->handler = fire_wall_start_ADDRESS;
		OBJECT(current_object)->speed_z = 0x100000;
		OBJECT(current_object)->sprite_frame = 0xAFD0u;
		OBJECT(current_object)->metal_platform.frame = 0xAFD0u;
		OBJECT(current_object)->metal_platform.timer = 0xA;
		OBJECT(current_object)->floor_z = OBJECT(current_object)->z;
		OBJECT(current_object)->sprite_offset_y = 0xC;
		timer_2 = OBJECT(current_object)->metal_platform.timer;
		OBJECT(current_object)->metal_platform.timer = timer_2 - 1u;
		if ((int16_t)timer_2 <= 1)
		{
			OBJECT(current_object)->metal_platform.timer = 0;
			OBJECT(current_object)->speed_z += 0x6000u;
			OBJECT(current_object)->z -= OBJECT(current_object)->speed_z;
		}
		if (OBJECT(current_object)->screen_y <= -0x58)
		{
			delete_current_object();
			return;
		}
		if (OBJECT(current_object)->screen_y <= 0x28)
			OBJECT(current_object)->sprite_frame = OBJECT(current_object)->metal_platform.frame;
		project_object(&ignored, &ignored);
		depth = (uint16_t)(((int32_t)((uint32_t)OBJECT(current_object)->projected_y + OBJECT(current_object)->floor_z) >> 16) - view_y);
		queue_sprite_at_depth(&ignored, &depth);
		return;
	case 2:
		timer_2 = OBJECT(current_object)->metal_platform.timer;
		OBJECT(current_object)->metal_platform.timer = timer_2 - 1u;
		if ((int16_t)timer_2 <= 1)
		{
			OBJECT(current_object)->metal_platform.timer = 0;
			OBJECT(current_object)->speed_z += 0x6000u;
			OBJECT(current_object)->z -= OBJECT(current_object)->speed_z;
		}
		if (OBJECT(current_object)->screen_y <= -0x58)
		{
			delete_current_object();
			return;
		}
		if (OBJECT(current_object)->screen_y <= 0x28)
			OBJECT(current_object)->sprite_frame = OBJECT(current_object)->metal_platform.frame;
		project_object(&ignored, &ignored);
		depth = (uint16_t)(((int32_t)((uint32_t)OBJECT(current_object)->projected_y + OBJECT(current_object)->floor_z) >> 16) - view_y);
		queue_sprite_at_depth(&ignored, &depth);
		return;
	case 3:
		object = LINKED(current_object, OBJECT(current_object)->parent);
		if ((int8_t)OBJECT(object)->var_80 >= 1)
		{
			OBJECT(current_object)->sprite_flags = 0x1002u;
			OBJECT(current_object)->x = OBJECT(object)->x;
			OBJECT(current_object)->y = OBJECT(object)->y;
			OBJECT(current_object)->z = OBJECT(object)->z;
			OBJECT(current_object)->x_int += 0x2Du;
			OBJECT(current_object)->metal_platform.timer = 0xA;
			OBJECT(current_object)->sprite_offset_x = -0x24;
			OBJECT(current_object)->sprite_offset_y = 0x1C;
			OBJECT(current_object)->speed_z = 0x80000;
			OBJECT(current_object)->floor_z = OBJECT(current_object)->z;
			OBJECT(current_object)->sprite_frame = 0xAFD8u;
			OBJECT(current_object)->metal_platform.frame = 0xAFE8u;
			OBJECT(current_object)->metal_platform.state = 2u;
		}
		project_object(&ignored, &r1);
		r1 = queue_sprite(r1);
		return;
	default:
		unknown_code(0x86DF9u + OBJECT(current_object)->metal_platform.state * 3u);
	}
}

void fire_wall_start(void)
{
	uint8_t sound_id = 0;
	uint32_t r1 = 0, r2 = 0, free_count = 0, ignored = 0, part_list = 0;

	OBJECT(current_object)->sprite_flags = 0xD006u;
	OBJECT(current_object)->sprite_offset_x = 0;
	OBJECT(current_object)->sprite_offset_y = 0;
	OBJECT(current_object)->flags = 2u;
	OBJECT(current_object)->hitbox_size_x = 0x5Bu;
	OBJECT(current_object)->hitbox_size_y = 0xF0u;
	OBJECT(current_object)->hitbox_height = 0x100u;
	OBJECT(current_object)->hitbox_offset_x = -0x2A;
	OBJECT(current_object)->hitbox_offset_y = -1;
	OBJECT(current_object)->kind = 0x14u;
	OBJECT(current_object)->direction = 0x100u;
	OBJECT(current_object)->speed_x = 0;
	OBJECT(current_object)->speed_y = 0;
	OBJECT(current_object)->x = 0x1000000;
	OBJECT(current_object)->y = 0xE470000;
	OBJECT(current_object)->z = 0x6100000;
	OBJECT(current_object)->animation = fire_wall_animation_ADDRESS;
	OBJECT(current_object)->animation_speed = 2u;
	OBJECT(current_object)->animation_timer = 0;
	OBJECT(current_object)->animation_frame = 0u;
	OBJECT(current_object)->speed_z = -0x100000;
	project_object(&ignored, &r1);
	OBJECT(current_object)->child = 0xFFFFu;
	allocate_object_2(&free_count, &r2, &part_list);
	if (free_count != 0u)
	{
		move_halfwords_backward(part_list, fire_wall_parts_ADDRESS, 9u);
		OBJECT(current_object)->part_list = part_list;
		sound_id = 7u;
		play_sound(sound_id, &r1, &r2);
		OBJECT(current_object)->handler = fire_wall_rise_ADDRESS;
	}
}

void fire_wall_rise(void)
{
	uint16_t part_list = 0;
	uint32_t r1 = 0, r8 = 0, depth = 0, ignored = 0, part = 0;

	if (scroll_position >= 0x6800u)
	{
		part_list = (uint16_t)OBJECT(current_object)->part_list;
		free_object_2(part_list);
		delete_current_object();
		return;
	}
	if (OBJECT(current_object)->z_int > 0x550)
	{
		OBJECT(current_object)->speed_z += 0x6000u;
		OBJECT(current_object)->z += OBJECT(current_object)->speed_z;
	}
	update_screen_position(&ignored, &r1);
	animate_all_parts(&r8, &part);
	add_to_touchable_list(ignored);
	depth = (uint16_t)(OBJECT(current_object)->screen_y + 0x70u);
	queue_sprite_at_depth(&ignored, &depth);
}

void fire_trench_start(void)
{
	uint32_t ignored = 0;

	OBJECT(current_object)->sprite_flags = 0x100Au;
	OBJECT(current_object)->sprite_frame = 0xB200u;
	OBJECT(current_object)->sprite_palette = 0x51F0u;
	OBJECT(current_object)->flags = 0u;
	OBJECT(current_object)->sprite_offset_x = 0;
	OBJECT(current_object)->sprite_offset_y = -0x16;
	OBJECT(current_object)->fire_trench_start.flame_timer = 0;
	OBJECT(current_object)->handler = fire_trench_update_ADDRESS;
	OBJECT(current_object)->z = 0x5300000;
	project_object(&ignored, &ignored);
}

void fire_trench_update(void)
{
	uint32_t depth = 0, ignored = 0, margin = 0, off_screen = 0;
	uint16_t flame_timer = 0;

	margin = 0x70u;
	off_screen = check_left_screen_margin((uint16_t)margin);
	if (off_screen != 0u)
	{
		delete_current_object();
		return;
	}
	flame_timer = OBJECT(current_object)->fire_trench_update.flame_timer;
	OBJECT(current_object)->fire_trench_update.flame_timer = flame_timer - 1u;
	if ((int16_t)flame_timer <= 1)
	{
		OBJECT(current_object)->fire_trench_update.flame_timer = 0x12C;
		spawn_fire_trench_flames();
	}
	update_screen_position(&ignored, &margin);
	depth = (uint16_t)(OBJECT(current_object)->screen_y - OBJECT(current_object)->sprite_offset_y - 0x20u);
	queue_sprite_at_depth(&ignored, &depth);
}

void spawn_fire_trench_flames(void)
{
	uint8_t sound_id = 0;
	uint32_t r1 = 0, r2 = 0, ignored = 0, object = 0;
	uint16_t flames_left_before = 0;

	if (free_object_stack < 3)
		return;
	trench_flame_y = 0xE47u;
	trench_flames_left = 3;
	do
	{
		allocate_object(&ignored, &object);
		OBJECT(object)->sprite_flags = 0x1006u;
		OBJECT(object)->sprite_palette = 0x5200u;
		OBJECT(object)->flags = 2u;
		OBJECT(object)->sprite_offset_x = -8;
		OBJECT(object)->sprite_offset_y = 0x3E;
		OBJECT(object)->hitbox_offset_x = 0;
		OBJECT(object)->hitbox_offset_y = 0;
		OBJECT(object)->kind = 0x14u;
		OBJECT(object)->direction = 0x100u;
		OBJECT(object)->hitbox_size_x = 0x40u;
		OBJECT(object)->hitbox_size_y = 0x80u;
		OBJECT(object)->handler = fire_trench_flame_burn_ADDRESS;
		OBJECT(object)->x = OBJECT(current_object)->x;
		OBJECT(object)->y_int = trench_flame_y;
		OBJECT(object)->y_fraction = 0u;
		OBJECT(object)->z = 0x5300000;
		trench_flame_y += 0x50u;
		OBJECT(object)->animation = fire_trench_flame_animation_ADDRESS;
		OBJECT(object)->animation_frame = 0xFFu;
		OBJECT(object)->animation_timer = 0;
		OBJECT(object)->animation_speed = 2u;
		flames_left_before = trench_flames_left;
		trench_flames_left = flames_left_before - 1u;
	} while ((int16_t)flames_left_before > 1);
	if (OBJECT(current_object)->screen_x <= 0x140)
	{
		sound_id = 7u;
		play_sound(sound_id, &r1, &r2);
	}
}

void fire_trench_flame_burn(void)
{
	uint32_t r7 = 0, at_last_step = 0, depth = 0, frame = 0, ignored = 0;

	animate(&frame, &r7, &at_last_step);
	if ((uint8_t)at_last_step == 0u)
	{
		OBJECT(current_object)->hitbox_height = fire_trench_flame_heights[OBJECT(current_object)->animation_frame];
		project_object(&ignored, &ignored);
		add_to_touchable_list(ignored);
		depth = (uint16_t)(OBJECT(current_object)->screen_y - OBJECT(current_object)->sprite_offset_y);
		queue_sprite_at_depth(&ignored, &depth);
		return;
	}
	delete_current_object();
}

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
	uint32_t r0 = 0, block_offset = 0, ignored = 0, object = 0, y = 0;
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
	find_floor(&r0, &y, &block_offset, &ignored, &object);
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
	uint32_t r2 = 0, climb_speed = 0, path_cursor = 0, point_x = 0, point_y = 0, projected_dx = 0, sound_id = 0;
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
				play_sound((uint8_t)sound_id, &climb_speed, &r2);
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
