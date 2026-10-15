/**
 *    Name:        objects/stage_03_machine.c
 *    Description: stage 3: the machine at 8300, its fire, the fireballs and the rising flames
 *    Version:     0.1.0
 *    Created:     12-10-2026
 *    Modified:    15-10-2026
 *    Author:      Sebastian Toyos
 *
 *    SegaSonic The Hedgehog (arcade, Sega System 32): the game's V60 program decompiled into C by
 *    tools/decompile.py, with the names of symbols.txt.
 */
#include "game.h"

void machine_start(void)
{
	uint16_t kind = 0, x = 0, y_2 = 0, z = 0;
	uint32_t block_offset = 0, depth = 0, free_count = 0, ignored = 0, object = 0, y = 0;

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
	project_object(&ignored, &depth);
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
		spawn_sign(x, y_2, kind, z, &depth, &object);
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
	uint32_t r1 = 0, r2 = 0, r7 = 0, r8 = 0, climb_speed = 0, free_count = 0, ignored = 0, object = 0, path_cursor = 0, point_x = 0, point_y = 0, projected_dx = 0, random_bits = 0, sound_id = 0, spawn_record = 0, x = 0;
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
		find_floor(&x, &r1, &r2, &ignored, &object);
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
		find_floor(&x, &r1, &r2, &random_bits, &object);
		fireball_lane_timer_cursor = current_object + 0x72u;
		fireball_lane = 3;
		for (;;)
		{
			lane_timer_address = fireball_lane_timer_cursor;
			lane_timer_before = mem_read8(lane_timer_address);
			mem_write8(lane_timer_address, lane_timer_before - 1u);
			if ((int8_t)lane_timer_before > 1)
			{
				fireball_lane_timer_cursor++;
				lane_before = fireball_lane;
				fireball_lane = lane_before - 1u;
				if ((int8_t)lane_before <= 1)
					break;
				continue;
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
			fireball_lane_timer_cursor++;
			lane_before = fireball_lane;
			fireball_lane = lane_before - 1u;
			if ((int8_t)lane_before <= 1)
				break;
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
	uint32_t flight_frames = 0, write_index_address = 0;

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
	play_sound(sound_id, &flight_frames, &write_index_address);
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
	uint32_t block_offset = 0, cursor = 0, ignored = 0, x = 0, y = 0;

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
	find_floor(&x, &y, &block_offset, &ignored, &cursor);
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
