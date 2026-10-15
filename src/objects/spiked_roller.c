/**
 *    Name:        objects/spiked_roller.c
 *    Description: the spiked roller (the chaser of stages 3 and E) and the stone floor blocks that crumble at the stage 3 finale
 *    Version:     0.1.0
 *    Created:     12-10-2026
 *    Modified:    15-10-2026
 *    Author:      Sebastian Toyos
 *
 *    SegaSonic The Hedgehog (arcade, Sega System 32): the game's V60 program decompiled into C by
 *    tools/decompile.py, with the names of symbols.txt.
 */
#include "game.h"

void stage_03_floor_block_start(void)
{
	uint32_t block_kind = 0, ignored = 0, kind_entry = 0, kind_record = 0;

	block_kind = OBJECT(current_object)->sprite_frame;
	OBJECT(current_object)->speed_x_int = block_kind;
	kind_entry = stage_03_floor_block_kinds_ADDRESS + block_kind * 4u;
	kind_record = ROM16(kind_entry) + stage_03_floor_block_kinds_ADDRESS;
	OBJECT(current_object)->sprite_offset_x = ROM8(kind_record);
	kind_record++;
	OBJECT(current_object)->sprite_offset_y = ROM8(kind_record);
	kind_record++;
	OBJECT(current_object)->speed_y_fraction = ROM16(kind_record);
	OBJECT(current_object)->animation = kind_record + 2u;
	OBJECT(current_object)->speed_x_fraction = ROM16(kind_entry + 2u);
	OBJECT(current_object)->sprite_flags = 0x1004u;
	OBJECT(current_object)->sprite_palette = 0x9040u;
	OBJECT(current_object)->handler = stage_03_floor_block_update_ADDRESS;
	OBJECT(current_object)->hitbox_size_x = 0x10u;
	OBJECT(current_object)->hitbox_size_y = 0x10u;
	OBJECT(current_object)->speed_z = 0;
	OBJECT(current_object)->flags = 2u;
	stage_03_floor_block_falls[0] = 0u;
	stage_03_floor_block_falls_1 = 0u;
	OBJECT(current_object)->sprite_frame = mem_read16(OBJECT(current_object)->animation + 2u);
	OBJECT(current_object)->animation_frame = 0u;
	OBJECT(current_object)->animation_speed = 8u;
	OBJECT(current_object)->animation_timer = 0;
	OBJECT(current_object)->z = 0;
	project_object(&ignored, &ignored);
}

void stage_03_floor_block_update(void)
{
	uint32_t r1 = 0, r7 = 0, at_last_step = 0, depth = 0, ignored = 0;

	if (stage_03_floor_block_falls[OBJECT(current_object)->speed_x_fraction] != 0u)
	{
		OBJECT(current_object)->speed_z += 0x1000u;
		OBJECT(current_object)->z += OBJECT(current_object)->speed_z;
		if (OBJECT(current_object)->speed_x_int < 4)
		{
			OBJECT(current_object)->sprite_offset_x = 0;
			OBJECT(current_object)->sprite_offset_y = 0;
		}
		if ((int8_t)OBJECT(current_object)->animation_speed >= 0)
		{
			animate(&r1, &r7, &at_last_step);
			if ((uint8_t)at_last_step != 0u)
				OBJECT(current_object)->animation_speed = 0xFFu;
		}
	}
	update_screen_position(&ignored, &r1);
	if (OBJECT(current_object)->screen_y < 0x158)
	{
		depth = (uint16_t)(OBJECT(current_object)->screen_y - OBJECT(current_object)->speed_y_fraction - OBJECT(current_object)->sprite_offset_y);
		queue_sprite_at_depth(&ignored, &depth);
		return;
	}
	delete_current_object();
}

void spiked_roller_start(void)
{
	uint32_t r11 = 0, block_offset = 0, free_count = 0, ignored = 0, object = 0, record_cursor = 0, roller_kind = 0, y = 0, zone_index = 0;
	uint8_t parts_left = 0;

	roller_kind = OBJECT(current_object)->sprite_frame;
	OBJECT(current_object)->platform = roller_kind;
	OBJECT(current_object)->spiked_roller.event_offset = spiked_roller_events[roller_kind];
	record_cursor = spiked_roller_kinds_ADDRESS + spiked_roller_kinds[roller_kind];
	roller_parts_left = ROM8(record_cursor);
	record_cursor++;
	r11 = ROM16(record_cursor);
	record_cursor += 2u;
	roller_first_state = ROM16(record_cursor);
	record_cursor += 2u;
	roller_back_zone_index = ROM16(record_cursor);
	OBJECT(current_object)->spiked_roller.path = record_cursor + 2u;
	r11 += 0x8219Cu;
	roller_step_x = (uint32_t)(int8_t)ROM8(r11) << 16;
	r11++;
	roller_step_y = (uint32_t)(int8_t)ROM8(r11) << 16;
	r11++;
	roller_part_record = r11;
	OBJECT(current_object)->flags = 0u;
	find_floor(&roller_kind, &y, &block_offset, &ignored, &r11);
	OBJECT(current_object)->z = OBJECT(current_object)->floor_z;
	OBJECT(current_object)->spiked_roller.screen_threshold = 0;
	OBJECT(current_object)->spiked_roller.catch_up_factor = 0u;
	OBJECT(current_object)->spiked_roller.base_speed = 0;
	OBJECT(current_object)->spiked_roller.wait_timer = 0;
	OBJECT(current_object)->spiked_roller.rumble_timer = 0u;
	OBJECT(current_object)->spiked_roller.substate = 0u;
	OBJECT(current_object)->player = 0u;
	OBJECT(current_object)->spiked_roller.holding_back = 0u;
	roller_ram_count = 0u;
	roller_ram_hit = 0u;
	machine_fire_done = 0u;
	OBJECT(current_object)->spiked_roller.rumble_off_screen = 0xFFu;
	if (chaser_object == 0u)
		chaser_object = current_object;
	roller_part_x = OBJECT(current_object)->x;
	roller_part_y = OBJECT(current_object)->y;
	roller_part_frame_cursor = spiked_roller_part_frames_ADDRESS;
	object = current_object;
	for (;;)
	{
		OBJECT(current_object)->sprite_palette = 0x82F0u;
		OBJECT(current_object)->hitbox_height = 0xF0u;
		OBJECT(current_object)->kind = 0xCu;
		OBJECT(current_object)->handler = spiked_roller_update_ADDRESS;
		OBJECT(current_object)->sprite_offset_x = 0;
		OBJECT(current_object)->sprite_offset_y = 5;
		OBJECT(current_object)->speed_z = 0;
		OBJECT(current_object)->var_80 = 2u;
		OBJECT(current_object)->player = 0u;
		OBJECT(current_object)->speed_x = 0;
		OBJECT(current_object)->speed_y = 0;
		record_cursor = roller_part_record;
		OBJECT(current_object)->sprite_flags = mem_read16(record_cursor);
		record_cursor += 2u;
		OBJECT(current_object)->hitbox_size_x = mem_read16(record_cursor);
		record_cursor += 2u;
		OBJECT(current_object)->hitbox_size_y = mem_read16(record_cursor);
		record_cursor += 2u;
		OBJECT(current_object)->spawn_arg_2 = mem_read16(record_cursor);
		record_cursor += 2u;
		OBJECT(current_object)->flags = mem_read32(record_cursor);
		record_cursor += 4u;
		OBJECT(current_object)->hitbox_offset_x = mem_read8(record_cursor);
		OBJECT(current_object)->hitbox_offset_y = mem_read8(record_cursor + 1u);
		OBJECT(current_object)->direction = (uint32_t)OBJECT(current_object)->spawn_arg_2 << 8;
		OBJECT(current_object)->var_86 = object_cursor;
		OBJECT(current_object)->parent = object;
		OBJECT(object)->child = current_object;
		object = (object & ~0xFFFFu) | (uint16_t)current_object;
		OBJECT(current_object)->animation_timer = 0;
		OBJECT(current_object)->animation = spiked_roller_animation_ADDRESS;
		OBJECT(current_object)->animation_frame = mem_read8(roller_part_frame_cursor);
		roller_part_frame_cursor++;
		OBJECT(current_object)->sprite_frame = spiked_roller_frames[OBJECT(current_object)->animation_frame];
		parts_left = roller_parts_left;
		roller_parts_left = parts_left - 1u;
		if ((int8_t)parts_left <= 1)
			break;
		allocate_object(&free_count, &r11);
		if (free_count == 0u)
			return;
		current_object = r11;
		roller_part_x += roller_step_x;
		OBJECT(current_object)->x = roller_part_x;
		roller_part_y += roller_step_y;
		OBJECT(current_object)->y = roller_part_y;
		OBJECT(current_object)->z = OBJECT(object_cursor)->z;
		OBJECT(current_object)->spawn_arg_2 = OBJECT(object_cursor)->spiked_roller.path_direction;
	}
	OBJECT(current_object)->child = 0xFFFFu;
	AS16(OBJECT(object_cursor)->spiked_roller.state) = roller_first_state;
	zone_index = roller_back_zone_index;
	spawn_roller_back_zone(zone_index, r11);
	if (OBJECT(object_cursor)->spiked_roller.state == 0u)
		players_frozen = 0xFFu;
	if (OBJECT(object_cursor)->spiked_roller.state == 0xEu)
	{
		OBJECT(object_cursor)->speed_z = -0x100000;
		OBJECT(object_cursor)->z = 0x500000;
		OBJECT(object_cursor)->speed_x = 0x8000;
		OBJECT(object_cursor)->speed_y = 0;
		OBJECT(object_cursor)->flags |= 1u;
	}
}

void spiked_roller_update(void)
{
	uint32_t r1 = 0, ignored = 0;

	switch (OBJECT(current_object)->spiked_roller.state)
	{
	case 0:
		spiked_roller_chase();
		break;
	case 1:
		spiked_roller_fall_off_ledge();
		break;
	case 2:
		spiked_roller_follow_leader();
		break;
	case 3:
		spiked_roller_drop_in();
		break;
	case 4:
		spiked_roller_ram();
		break;
	case 5:
		spiked_roller_remove();
		break;
	case 6:
		spiked_roller_crash_into_machine();
		break;
	case 7:
		spiked_roller_sink_and_jump();
		break;
	case 8:
		spiked_roller_fall();
		break;
	case 9:
		spiked_roller_burst_out();
		break;
	case 10:
		spiked_roller_smash_wall();
		break;
	case 11:
		spiked_roller_wait_for_turn();
		break;
	case 12:
		spiked_roller_wait_to_burst_out();
		break;
	case 13:
		spiked_roller_shift_box();
		break;
	case 14:
		spiked_roller_wait_hidden();
		break;
	case 15:
		spiked_roller_spawn_fixed_zone();
		break;
	case 16:
		spiked_roller_chase();
		break;
	default:
		unknown_code(0x803BBu + OBJECT(current_object)->spiked_roller.state * 3u);
	}
	if (OBJECT(current_object)->player == 0u)
	{
		if ((OBJECT(current_object)->flags & 1u) != 0u)
			return;
		add_to_zone_list(0u);
		if (OBJECT(current_object)->spiked_roller.state == 1u)
		{
			ignored = queue_sprite_at_previous_floor();
			return;
		}
		r1 = queue_sprite_at_base(0u);
		return;
	}
	if (chaser_object == (uint16_t)current_object)
		chaser_object = 0u;
	delete_current_object();
}

void spiked_roller_chase(void)
{
	uint8_t sound_id = 0;
	uint16_t height_above_floor = 0;
	uint32_t r2 = 0, r3 = 0, r7 = 0, chase_speed = 0, cursor = 0, event_kind = 0, ignored = 0, move_step = 0, off_screen = 0, record_cursor = 0, speed_level = 0, x = 0, y = 0;
	uint8_t hold_timer = 0, rumble_timer = 0;
	uint16_t record_position = 0, z_int = 0, floor_z_int = 0, event_position = 0, max_y = 0, min_x = 0, min_y = 0, max_x = 0;
	uint32_t flags = 0;

	flags = OBJECT(current_object)->flags;
	OBJECT(current_object)->flags = flags | 0x80000000u;
	if ((flags & 0x80000000u) == 0u)
	{
	spiked_roller_chase_read_script:
		if ((uint16_t)OBJECT(current_object)->spiked_roller.wait_timer == 0u)
		{
			record_cursor = OBJECT(current_object)->spiked_roller.path;
			record_position = mem_read16(record_cursor);
			record_cursor += 2u;
			if (record_position <= scroll_position)
			{
				if ((int16_t)mem_read16(record_cursor + 2u) >= 0)
				{
					OBJECT(current_object)->spiked_roller.screen_threshold = mem_read16(record_cursor);
					record_cursor += 2u;
					OBJECT(current_object)->spiked_roller.catch_up_factor = mem_read16(record_cursor);
					record_cursor += 2u;
					OBJECT(current_object)->spiked_roller.base_speed = mem_read16(record_cursor);
					record_cursor += 2u;
					OBJECT(current_object)->spiked_roller.wait_timer = mem_read16(record_cursor);
					OBJECT(current_object)->spiked_roller.path = record_cursor + 2u;
					if ((uint16_t)OBJECT(current_object)->spiked_roller.wait_timer == 0u)
					{
						OBJECT(current_object)->kind = 0x18u;
						players_frozen = 0u;
					}
				}
				else
				{
					OBJECT(current_object)->spiked_roller.catch_up_factor = 0u;
					OBJECT(current_object)->spiked_roller.base_speed = 0;
					off_screen = is_off_screen();
					if (off_screen != 0u)
					{
						OBJECT(current_object)->spiked_roller.state = 5u;
						return;
					}
				}
			}
		}
	}
	else if (OBJECT(current_object)->spiked_roller.holding_back == 0u)
	{
		if (no_player_in_play == 0u && player_mishap == 0u && player_dropping_in == 0u)
			goto spiked_roller_chase_read_script;
		OBJECT(current_object)->spiked_roller.holding_back = 0xFFu;
		OBJECT(current_object)->spiked_roller.saved_base_speed = OBJECT(current_object)->spiked_roller.base_speed;
		OBJECT(current_object)->spiked_roller.base_speed = 0;
		OBJECT(current_object)->spiked_roller.hold_start_position = scroll_position;
		OBJECT(current_object)->spiked_roller.hold_timer = 0xFFu;
		OBJECT(current_object)->spiked_roller.saved_threshold = OBJECT(current_object)->spiked_roller.screen_threshold;
		switch (OBJECT(current_object)->spiked_roller.path_direction)
		{
		case 0:
		case 3:
		case 4:
			OBJECT(current_object)->spiked_roller.screen_threshold = 0x160;
			break;
		case 1:
		case 2:
			OBJECT(current_object)->spiked_roller.screen_threshold = -0x20;
			break;
		default:
			unknown_code(0x80448u + OBJECT(current_object)->spiked_roller.path_direction * 2u);
		}
	}
	if (OBJECT(current_object)->platform != 5u && OBJECT(current_object)->platform != 2u)
	{
		find_floor(&x, &y, &r2, &r3, &cursor);
		z_int = OBJECT(current_object)->z_int;
		floor_z_int = OBJECT(current_object)->floor_z_int;
		height_above_floor = (uint16_t)(floor_z_int - z_int);
		if (floor_z_int == z_int || (int16_t)height_above_floor <= 8)
			OBJECT(current_object)->z = OBJECT(current_object)->floor_z;
		else
		{
			OBJECT(current_object)->spiked_roller.state = 1u;
			chaser_object = 0u;
		}
	}
	project_object(&ignored, &ignored);
	if (OBJECT(current_object)->spiked_roller.holding_back != 0u)
	{
		if ((int16_t)(scroll_position - OBJECT(current_object)->spiked_roller.hold_start_position) >= 0x300)
			goto spiked_roller_chase_stop_holding_back;
		if (no_player_in_play != 0u)
			OBJECT(current_object)->spiked_roller.hold_timer = 0xFFu;
		hold_timer = OBJECT(current_object)->spiked_roller.hold_timer;
		OBJECT(current_object)->spiked_roller.hold_timer = hold_timer - 1u;
		if (hold_timer == 0u || AS32(players_finished[0]) == 0xFFFFFFu)
			goto spiked_roller_chase_stop_holding_back;
	}
	switch (OBJECT(current_object)->spiked_roller.path_direction)
	{
	case 0:
	case 3:
		if (OBJECT(current_object)->screen_x <= OBJECT(current_object)->spiked_roller.screen_threshold)
		{
		spiked_roller_chase_use_base_speed:
			chase_speed = (uint16_t)OBJECT(current_object)->spiked_roller.base_speed;
		}
		else
		{
		spiked_roller_chase_catch_up:
			OBJECT(current_object)->spiked_roller.wait_timer = 0;
			if (OBJECT(current_object)->spiked_roller.holding_back != 0u)
			{
			spiked_roller_chase_stop_holding_back:
				OBJECT(current_object)->spiked_roller.holding_back = 0u;
				OBJECT(current_object)->spiked_roller.screen_threshold = OBJECT(current_object)->spiked_roller.saved_threshold;
				OBJECT(current_object)->spiked_roller.base_speed = OBJECT(current_object)->spiked_roller.saved_base_speed;
			}
			compute_camera_ground_speed(r3);
			chase_speed = (uint16_t)((uint32_t)camera_ground_speed_int * OBJECT(current_object)->spiked_roller.catch_up_factor) >> 8;
			if ((int16_t)chase_speed <= OBJECT(current_object)->spiked_roller.base_speed)
				goto spiked_roller_chase_use_base_speed;
		}
		break;
	case 1:
	case 2:
	case 4:
		if (OBJECT(current_object)->spiked_roller.screen_threshold <= OBJECT(current_object)->screen_x)
			goto spiked_roller_chase_use_base_speed;
		goto spiked_roller_chase_catch_up;
	default:
		unknown_code(0x80548u + OBJECT(current_object)->spiked_roller.path_direction * 2u);
	}
	chaser_speed = chase_speed;
	if (current_game_mode != 0x11u)
		OBJECT(current_object)->spiked_roller.wait_timer--;
	if (OBJECT(current_object)->spiked_roller.wait_timer <= 0)
	{
		OBJECT(current_object)->spiked_roller.wait_timer = 0;
		if (grey_screen_active == 0u)
		{
			switch (OBJECT(current_object)->spiked_roller.path_direction)
			{
			case 0:
				move_step = (uint32_t)chaser_speed << 12;
				OBJECT(current_object)->y -= move_step;
				OBJECT(current_object)->speed_y = move_step;
				OBJECT(current_object)->speed_x = 0;
				break;
			case 1:
				move_step = (uint32_t)chaser_speed << 12;
				OBJECT(current_object)->x += move_step;
				OBJECT(current_object)->speed_x = move_step;
				OBJECT(current_object)->speed_y = 0;
				break;
			case 2:
			case 4:
				move_step = (uint32_t)chaser_speed << 12;
				OBJECT(current_object)->y += move_step;
				OBJECT(current_object)->speed_y = move_step;
				OBJECT(current_object)->speed_x = 0;
				break;
			case 3:
				move_step = (uint32_t)chaser_speed << 12;
				OBJECT(current_object)->x -= move_step;
				OBJECT(current_object)->speed_x = move_step;
				OBJECT(current_object)->speed_y = 0;
				break;
			default:
				unknown_code(0x805E4u + OBJECT(current_object)->spiked_roller.path_direction * 2u);
			}
		}
		else
			chaser_speed = 0;
	}
	if ((uint16_t)chaser_speed != 0u)
	{
		speed_level = (uint16_t)chaser_speed >> 4;
		if ((int16_t)speed_level > 5)
			speed_level = 5u;
		OBJECT(current_object)->animation_speed = spiked_roller_animation_speeds[speed_level];
		animate(&chase_speed, &r7, &off_screen);
	}
	switch (OBJECT(current_object)->spiked_roller.path_direction)
	{
	case 0:
	case 4:
		if (OBJECT(current_object)->screen_x > 0x140)
		{
		spiked_roller_chase_no_rumble:
			OBJECT(current_object)->spiked_roller.rumble_off_screen = 0xFFu;
		}
		else
		{
		spiked_roller_chase_rumble:
			if (OBJECT(current_object)->spiked_roller.rumble_off_screen != 0u)
			{
				OBJECT(current_object)->spiked_roller.rumble_off_screen = 0u;
				OBJECT(current_object)->spiked_roller.rumble_timer = 0u;
			}
			if ((uint16_t)chaser_speed == 0u)
				goto spiked_roller_chase_no_rumble;
			rumble_timer = OBJECT(current_object)->spiked_roller.rumble_timer;
			OBJECT(current_object)->spiked_roller.rumble_timer = rumble_timer - 1u;
			if (rumble_timer == 0u)
			{
				sound_id = 0xBu;
				play_sound(sound_id, &chase_speed, &r2);
				speed_level = (uint16_t)chaser_speed;
				speed_level = speed_level >> 4;
				if ((int16_t)speed_level < 0)
					speed_level = 0u;
				if ((int16_t)speed_level > 4)
					speed_level = 4u;
				OBJECT(current_object)->spiked_roller.rumble_timer = spiked_roller_rumble_delays[speed_level];
			}
		}
		break;
	case 1:
		if (OBJECT(current_object)->screen_x < -0x50)
			goto spiked_roller_chase_no_rumble;
		goto spiked_roller_chase_rumble;
	case 2:
		if (OBJECT(current_object)->screen_x < 0)
			goto spiked_roller_chase_no_rumble;
		goto spiked_roller_chase_rumble;
	case 3:
		if (OBJECT(current_object)->screen_x > 0x140)
			goto spiked_roller_chase_no_rumble;
		goto spiked_roller_chase_rumble;
	default:
		unknown_code(0x80678u + OBJECT(current_object)->spiked_roller.path_direction * 2u);
	}
	record_cursor = OBJECT(current_object)->spiked_roller.event_offset + spiked_roller_events_ADDRESS;
	event_position = ROM16(record_cursor);
	record_cursor += 2u;
	if (scroll_position < event_position || OBJECT(current_object)->spiked_roller.substate != 0u)
	{
		roller_ram_hit = 0u;
		return;
	}
	event_kind = ROM16(record_cursor) * 2u;
	record_cursor += 2u;
	switch (event_kind)
	{
	case 0x0u:
	case 0x8u:
		max_y = ROM16(record_cursor);
		record_cursor += 2u;
		if (OBJECT(current_object)->y_int <= (int16_t)max_y)
		{
			OBJECT(current_object)->spiked_roller.state = (uint8_t)ROM16(record_cursor);
			AS32(OBJECT(current_object)->spiked_roller.event_offset) += 8u;
		}
		break;
	case 0x2u:
		min_x = ROM16(record_cursor);
		record_cursor += 2u;
		if ((int16_t)min_x <= OBJECT(current_object)->x_int)
		{
			OBJECT(current_object)->spiked_roller.state = (uint8_t)ROM16(record_cursor);
			AS32(OBJECT(current_object)->spiked_roller.event_offset) += 8u;
			roller_ram_hit = 0u;
			return;
		}
		break;
	case 0x4u:
		min_y = ROM16(record_cursor);
		record_cursor += 2u;
		if ((int16_t)min_y <= OBJECT(current_object)->y_int)
		{
			OBJECT(current_object)->spiked_roller.state = (uint8_t)ROM16(record_cursor);
			AS32(OBJECT(current_object)->spiked_roller.event_offset) += 8u;
			roller_ram_hit = 0u;
			return;
		}
		break;
	case 0x6u:
		max_x = ROM16(record_cursor);
		record_cursor += 2u;
		if (OBJECT(current_object)->x_int <= (int16_t)max_x)
		{
			OBJECT(current_object)->spiked_roller.state = (uint8_t)ROM16(record_cursor);
			AS32(OBJECT(current_object)->spiked_roller.event_offset) += 8u;
			roller_ram_hit = 0u;
			return;
		}
		break;
	default:
		unknown_code(0x8071Bu + event_kind);
	}
	roller_ram_hit = 0u;
}

void spiked_roller_fall_off_ledge(void)
{
	OBJECT(current_object)->sprite_palette |= 0x4000u;
	spiked_roller_fall();
}

void spiked_roller_fall(void)
{
	uint32_t r7 = 0, r8 = 0, frame = 0, ignored = 0;

	OBJECT(current_object)->speed_z += 0x1000u;
	OBJECT(current_object)->z += OBJECT(current_object)->speed_z;
	OBJECT(current_object)->animation_speed = 1u;
	animate(&frame, &r7, &r8);
	if (OBJECT(current_object)->screen_y >= 0x130)
		OBJECT(current_object)->player = 0xFFu;
	project_object(&ignored, &ignored);
}

void spiked_roller_follow_leader(void)
{
	uint32_t r1 = 0, r7 = 0, r8 = 0, ignored = 0, leader_hidden_bit = 0, leader_reverse_bit = 0, object = 0, speed_level = 0;

	if (OBJECT((current_object & ~0xFFFFu) | OBJECT(current_object)->spiked_roller.leader)->var_80 == 5u)
	{
		OBJECT(current_object)->player = 0xFFu;
		return;
	}
	object = (current_object & ~0xFFFFu) | OBJECT(current_object)->spiked_roller.leader;
	switch (OBJECT(current_object)->spiked_roller.path_direction)
	{
	case 0:
	case 2:
	case 4:
		OBJECT(current_object)->y = OBJECT(object)->y;
		OBJECT(current_object)->speed_y = OBJECT(object)->speed_y;
		break;
	case 1:
	case 3:
		OBJECT(current_object)->x = OBJECT(object)->x;
		OBJECT(current_object)->speed_x = OBJECT(object)->speed_x;
		break;
	default:
		unknown_code(0x807BBu + OBJECT(current_object)->spiked_roller.path_direction * 2u);
	}
	OBJECT(current_object)->kind = OBJECT(object)->kind;
	OBJECT(current_object)->hitbox_height = OBJECT(object)->hitbox_height;
	OBJECT(current_object)->hitbox_offset_x = OBJECT(object)->hitbox_offset_x;
	OBJECT(current_object)->hitbox_offset_y = OBJECT(object)->hitbox_offset_y;
	OBJECT(current_object)->z = OBJECT(object)->z;
	OBJECT(current_object)->floor_z = OBJECT(object)->floor_z;
	OBJECT(current_object)->previous_floor_z = OBJECT(object)->previous_floor_z;
	if (OBJECT(object)->var_80 == 1u)
		OBJECT(current_object)->spiked_roller.state = 1u;
	if (OBJECT(object)->var_80 == 8u)
		OBJECT(current_object)->spiked_roller.state = 8u;
	project_object(&ignored, &r1);
	if ((uint16_t)chaser_speed == 0u)
	{
		leader_hidden_bit = OBJECT((current_object & ~0xFFFFu) | OBJECT(current_object)->spiked_roller.leader)->flags & 1u;
		OBJECT(current_object)->flags &= ~1u;
		OBJECT(current_object)->flags |= leader_hidden_bit;
		return;
	}
	speed_level = (uint16_t)chaser_speed >> 4;
	if ((int16_t)speed_level > 5)
		speed_level = 5u;
	OBJECT(current_object)->animation_speed = spiked_roller_animation_speeds[speed_level];
	leader_reverse_bit = OBJECT((current_object & ~0xFFFFu) | OBJECT(current_object)->spiked_roller.leader)->flags & 0x80u;
	OBJECT(current_object)->flags &= ~0x80u;
	OBJECT(current_object)->flags |= leader_reverse_bit;
	animate(&r1, &r7, &r8);
	leader_hidden_bit = OBJECT((current_object & ~0xFFFFu) | OBJECT(current_object)->spiked_roller.leader)->flags & 1u;
	OBJECT(current_object)->flags &= ~1u;
	OBJECT(current_object)->flags |= leader_hidden_bit;
}

void spiked_roller_wait_hidden(void)
{
	OBJECT(current_object)->flags |= 1u;
	if (scroll_position >= 0x9B00u || machine_fire_done != 0u)
	{
		OBJECT(current_object)->spiked_roller.state = 3u;
		OBJECT(current_object)->flags &= ~1u;
	}
}

void spiked_roller_drop_in(void)
{
	uint8_t sound_number = 0;
	uint32_t r1 = 0, r7 = 0, r8 = 0, ignored = 0, write_index_address = 0;

	OBJECT(current_object)->speed_z += 0x8000u;
	OBJECT(current_object)->z += OBJECT(current_object)->speed_z;
	OBJECT(current_object)->x += OBJECT(current_object)->speed_x;
	OBJECT(current_object)->y += OBJECT(current_object)->speed_y;
	OBJECT(current_object)->animation_speed = 1u;
	animate(&r1, &r7, &r8);
	OBJECT(current_object)->floor_z = 0;
	if (OBJECT(current_object)->speed_z_int >= 0 && OBJECT(current_object)->floor_z_int <= OBJECT(current_object)->z_int)
	{
		OBJECT(current_object)->z = OBJECT(current_object)->floor_z;
		OBJECT(current_object)->spiked_roller.state = 0u;
		camera_shake_active = 1u;
		sound_number = 0x20u;
		play_sound(sound_number, &r1, &write_index_address);
	}
	project_object(&ignored, &ignored);
}

void spiked_roller_ram(void)
{
	uint8_t sound_id = 0;
	uint32_t ignored = 0, speed_level = 0, write_index = 0, write_index_address = 0;
	uint8_t rumble_timer = 0;

	switch (OBJECT(current_object)->spiked_roller.substate)
	{
	case 0:
		spiked_roller_ram_charge();
		break;
	case 1:
		spiked_roller_ram_pause();
		break;
	case 2:
		spiked_roller_ram_back_off();
		break;
	case 3:
		spiked_roller_ram_wait();
		break;
	case 4:
		spiked_roller_ram_charge();
		break;
	default:
		unknown_code(0x80944u + OBJECT(current_object)->spiked_roller.substate * 3u);
	}
	rumble_timer = OBJECT(current_object)->spiked_roller.rumble_timer;
	OBJECT(current_object)->spiked_roller.rumble_timer = rumble_timer - 1u;
	if ((int8_t)rumble_timer > 1)
	{
		project_object(&ignored, &ignored);
		return;
	}
	sound_id = 0xBu;
	play_sound(sound_id, &write_index, &write_index_address);
	speed_level = (uint16_t)chaser_speed;
	speed_level = speed_level >> 4;
	if ((int16_t)speed_level < 0)
		speed_level = 0u;
	if ((int16_t)speed_level > 4)
		speed_level = 4u;
	OBJECT(current_object)->spiked_roller.rumble_timer = spiked_roller_rumble_delays[speed_level];
	project_object(&ignored, &ignored);
}

void spiked_roller_ram_charge(void)
{
	uint8_t sound_id = 0;
	uint16_t margin_x = 0, margin_y = 0;
	uint32_t r1 = 0, r7 = 0, r8 = 0, off_screen = 0, write_index_address = 0;

	OBJECT(current_object)->speed_x = 0;
	OBJECT(current_object)->speed_y = -0x40000;
	chaser_speed = 0x40;
	OBJECT(current_object)->y += OBJECT(current_object)->speed_y;
	OBJECT(current_object)->animation_speed = 1u;
	animate(&r1, &r7, &r8);
	if ((int16_t)(ROM16(spiked_roller_event_coords_ADDRESS + OBJECT(current_object)->spiked_roller.event_offset) - 0x50u) < OBJECT(current_object)->y_int)
		return;
	OBJECT(current_object)->spiked_roller.substate++;
	camera_shake_active = 1u;
	roller_ram_hit = 0xFFu;
	OBJECT(current_object)->spiked_roller.pause_timer = 0u;
	sound_id = 7u;
	play_sound(sound_id, &r1, &write_index_address);
	margin_x = 0x50u;
	margin_y = 0x50u;
	off_screen = is_off_screen_margin(margin_x, margin_y);
	if (off_screen != 0u)
	{
		OBJECT(current_object)->spiked_roller.substate = 0u;
		roller_ram_count = 0u;
		OBJECT(current_object)->spiked_roller.state = 0u;
		OBJECT(current_object)->flags &= ~0x800u;
		OBJECT(current_object)->flags |= 0x80u;
	}
	roller_ram_count++;
	if ((int8_t)roller_ram_count >= 4)
	{
		OBJECT(current_object)->spiked_roller.substate = 0u;
		roller_ram_count = 0u;
		OBJECT(current_object)->spiked_roller.state = 0u;
	}
}

void spiked_roller_ram_pause(void)
{
	OBJECT(current_object)->speed_x = 0;
	OBJECT(current_object)->speed_y = 0;
	chaser_speed = 0;
	roller_ram_hit = 0u;
	OBJECT(current_object)->spiked_roller.pause_timer++;
	if ((int8_t)OBJECT(current_object)->spiked_roller.pause_timer >= 0x3C)
	{
		OBJECT(current_object)->spiked_roller.substate++;
		OBJECT(current_object)->flags &= ~0x80u;
	}
}

void spiked_roller_ram_back_off(void)
{
	uint32_t r7 = 0, r8 = 0, frame = 0;

	OBJECT(current_object)->speed_x = 0;
	OBJECT(current_object)->speed_y = 0x10000;
	chaser_speed = 0x10;
	OBJECT(current_object)->y += OBJECT(current_object)->speed_y;
	OBJECT(current_object)->animation_speed = 4u;
	animate(&frame, &r7, &r8);
	if ((int16_t)ROM16(spiked_roller_event_coords_ADDRESS + OBJECT(current_object)->spiked_roller.event_offset) <= OBJECT(current_object)->y_int)
	{
		OBJECT(current_object)->spiked_roller.substate++;
		OBJECT(current_object)->spiked_roller.pause_timer = 0u;
		OBJECT(current_object)->flags |= 0x80u;
	}
}

void spiked_roller_ram_wait(void)
{
	uint32_t r7 = 0, r8 = 0, frame = 0;

	OBJECT(current_object)->speed_x = 0;
	OBJECT(current_object)->speed_y = 0;
	chaser_speed = 0x40;
	OBJECT(current_object)->animation_speed = 1u;
	animate(&frame, &r7, &r8);
	if (no_player_in_play == 0u)
	{
		OBJECT(current_object)->spiked_roller.pause_timer++;
		if ((int8_t)OBJECT(current_object)->spiked_roller.pause_timer >= 0x3C)
			OBJECT(current_object)->spiked_roller.substate = 0u;
	}
}

void spiked_roller_remove(void)
{
	OBJECT(current_object)->player = 0xFFu;
}

void spiked_roller_crash_into_machine(void)
{
	uint8_t sound_id = 0;
	uint32_t r0 = 0, r1 = 0, r7 = 0, r8 = 0, ignored = 0, speed_level = 0, write_index_address = 0;
	uint8_t rumble_timer = 0;

	r0 = OBJECT(current_object)->spiked_roller.substate * 3u;
	switch (OBJECT(current_object)->spiked_roller.substate)
	{
	case 0:
	case 4:
		OBJECT(current_object)->spiked_roller.substate++;
		chaser_speed = 0x10;
		OBJECT(current_object)->speed_y = -0x10000;
		speed_level = (uint16_t)chaser_speed;
		speed_level = speed_level >> 4;
		if ((int16_t)speed_level > 5)
			speed_level = 5u;
		OBJECT(current_object)->animation_speed = spiked_roller_animation_speeds[speed_level];
	case 1:
		if (scroll_position >= 0x8600u)
		{
			chaser_speed = 0x50;
			OBJECT(current_object)->speed_y = -0x50000;
		}
		OBJECT(current_object)->y += OBJECT(current_object)->speed_y;
		animate(&r1, &r7, &r8);
		rumble_timer = OBJECT(current_object)->spiked_roller.rumble_timer;
		OBJECT(current_object)->spiked_roller.rumble_timer = rumble_timer - 1u;
		if ((int8_t)rumble_timer <= 1)
		{
			sound_id = 0xBu;
			play_sound(sound_id, &r1, &write_index_address);
			speed_level = (uint16_t)chaser_speed;
			speed_level = speed_level >> 4;
			if ((int16_t)speed_level < 0)
				speed_level = 0u;
			if ((int16_t)speed_level > 4)
				speed_level = 4u;
			OBJECT(current_object)->spiked_roller.rumble_timer = spiked_roller_rumble_delays[speed_level];
		}
		if (OBJECT(current_object)->y_int <= 0xAE0)
			OBJECT(current_object)->spiked_roller.substate++;
		project_object(&ignored, &ignored);
		return;
	case 2:
		OBJECT(current_object)->spiked_roller.substate++;
		chaser_speed = 0;
		OBJECT(current_object)->speed_y = 0;
		camera_shake_active = 1u;
		sound_id = 6u;
		play_sound(sound_id, &r1, &write_index_address);
		machine_wrecked = 0xFFu;
		scroll_resume_request = 0xFFu;
		update_screen_position(&ignored, &r1);
		return;
	case 3:
		if (OBJECT(current_object)->screen_y >= 0x158)
		{
			OBJECT(current_object)->spiked_roller.state = 5u;
			chaser_object = 0u;
		}
		update_screen_position(&ignored, &r1);
		return;
	default:
		unknown_code(0x80ACBu + OBJECT(current_object)->spiked_roller.substate * 3u);
	}
}

void spiked_roller_sink_and_jump(void)
{
	uint8_t sound_number = 0;
	uint32_t r1 = 0, r7 = 0, r8 = 0, ignored = 0, write_index_address = 0;

	switch (OBJECT(current_object)->spiked_roller.substate)
	{
	case 0:
	case 2:
		if (stage_03_floor_block_falls[0] == 0u)
		{
			sound_number = 0xDu;
			play_sound(sound_number, &r1, &write_index_address);
		}
		stage_03_floor_block_falls[0] = 0xFFu;
		OBJECT(current_object)->speed_z += 0x1000u;
		OBJECT(current_object)->z += OBJECT(current_object)->speed_z;
		chaser_speed = 0x40;
		OBJECT(current_object)->speed_x = 0;
		OBJECT(current_object)->animation_speed = 1u;
		animate(&r1, &r7, &r8);
		if ((int16_t)(OBJECT(current_object)->z_int - OBJECT(current_object)->floor_z_int) >= 0x40)
		{
			OBJECT(current_object)->spiked_roller.substate++;
			OBJECT(current_object)->speed_z = -0x60000;
			OBJECT(current_object)->speed_x = 0x30000;
		}
		project_object(&ignored, &ignored);
		return;
	case 1:
		OBJECT(current_object)->speed_z += 0x3000u;
		OBJECT(current_object)->z += OBJECT(current_object)->speed_z;
		OBJECT(current_object)->x += OBJECT(current_object)->speed_x;
		OBJECT(current_object)->animation_speed = 1u;
		animate(&r1, &r7, &r8);
		if (OBJECT(current_object)->speed_z_int >= 0 && OBJECT(current_object)->floor_z_int <= OBJECT(current_object)->z_int)
		{
			OBJECT(current_object)->spiked_roller.state = 8u;
			OBJECT(current_object)->speed_z = 0;
			stage_03_floor_block_falls_1 = 0xFFu;
			camera_shake_active = 1u;
			goal_unlocked = 0xFFu;
			sound_number = 0xDu;
			play_sound(sound_number, &r1, &write_index_address);
		}
		project_object(&ignored, &ignored);
		return;
	default:
		unknown_code(0x80BC9u + OBJECT(current_object)->spiked_roller.substate * 2u);
	}
}

void spiked_roller_burst_out(void)
{
	uint8_t sound_id = 0;
	uint16_t screen_x = 0;
	uint32_t r1 = 0, fragment_cursor = 0, fragments_left = 0, free_count = 0, ignored = 0, object = 0, write_index_address = 0;
	uint16_t wait_timer = 0;

	OBJECT(current_object)->flags |= 1u;
	wait_timer = OBJECT(current_object)->spiked_roller.wait_timer;
	OBJECT(current_object)->spiked_roller.wait_timer = wait_timer - 1u;
	if ((int16_t)wait_timer > 1)
		return;
	OBJECT(current_object)->spiked_roller.state = 0u;
	chaser_object = current_object;
	OBJECT(current_object)->flags &= ~1u;
	scroll_resume_request = 0xFFu;
	project_object(&ignored, &r1);
	sound_id = 7u;
	play_sound(sound_id, &r1, &write_index_address);
	screen_x = (uint16_t)OBJECT(current_object)->screen_x;
	r14 = (uint16_t)OBJECT(current_object)->screen_y;
	fragments_left = roller_wall_fragment_count;
	fragment_cursor = roller_wall_fragments_ADDRESS;
	do
	{
		allocate_object(&free_count, &object);
		if (free_count == 0u)
			break;
		OBJECT(object)->sprite_flags = ROM16(fragment_cursor);
		OBJECT(object)->screen_x = screen_x;
		fragment_cursor += 2u;
		OBJECT(object)->screen_x += ROM16(fragment_cursor);
		OBJECT(object)->screen_y = r14;
		fragment_cursor += 2u;
		OBJECT(object)->screen_y += ROM16(fragment_cursor);
		fragment_cursor += 2u;
		OBJECT(object)->speed_x_fraction = ROM16(fragment_cursor);
		fragment_cursor += 2u;
		OBJECT(object)->speed_y_fraction = ROM16(fragment_cursor);
		fragment_cursor += 2u;
		OBJECT(object)->animation = rock_debris_animations_ADDRESS + ROM16(fragment_cursor);
		OBJECT(object)->sprite_palette = 0x8240u;
		OBJECT(object)->projected_x = (uint32_t)OBJECT(object)->screen_x << 4;
		OBJECT(object)->projected_y_fraction = (uint32_t)OBJECT(object)->screen_y << 4;
		OBJECT(object)->animation_frame = 0u;
		OBJECT(object)->animation_speed = 4u;
		OBJECT(object)->animation_timer = 0;
		OBJECT(object)->flags = 0u;
		OBJECT(object)->sprite_scale = 0x100u;
		OBJECT(object)->handler = rock_debris_fly_ADDRESS;
		fragment_cursor += 2u;
		fragments_left--;
	} while (fragments_left != 0u);
}

void spiked_roller_smash_wall(void)
{
	uint16_t margin_y = 0;
	uint32_t ignored = 0, off_screen = 0, x_margin = 0;

	OBJECT(current_object)->kind = 0xCu;
	chaser_speed = 0;
	if (stage == 0xEu)
		chaser_stopped = 0xFFu;
	x_margin = 0x68u;
	margin_y = 0x70u;
	off_screen = is_off_screen_margin((uint16_t)x_margin, margin_y);
	if (off_screen != 0u)
		OBJECT(current_object)->spiked_roller.state = 5u;
	update_screen_position(&ignored, &x_margin);
}

void spiked_roller_wait_for_turn(void)
{
	uint32_t ignored = 0;

	OBJECT(current_object)->kind = 0xCu;
	chaser_speed = 0;
	if (chaser_object == 0u)
	{
		OBJECT(current_object)->spiked_roller.state = 0u;
		chaser_object = current_object;
		OBJECT(current_object)->hitbox_height = 0x40u;
	}
	project_object(&ignored, &ignored);
}

void spiked_roller_wait_to_burst_out(void)
{
	uint32_t ignored = 0;

	OBJECT(current_object)->flags |= 1u;
	if (chaser_object == 0u)
	{
		OBJECT(current_object)->spiked_roller.state = 9u;
		OBJECT(current_object)->spiked_roller.wait_timer = 0x3C;
	}
	project_object(&ignored, &ignored);
}

void spiked_roller_shift_box(void)
{
	OBJECT(current_object)->spiked_roller.state = 0u;
	OBJECT(current_object)->hitbox_offset_y = -0x26;
	spiked_roller_chase();
}

void spiked_roller_spawn_fixed_zone(void)
{
	uint32_t free_count = 0, object = 0;

	allocate_object(&free_count, &object);
	if (free_count != 0u)
	{
		OBJECT(object)->sprite_flags = 0x1004u;
		OBJECT(object)->kind = 0xCu;
		OBJECT(object)->flags = 2u;
		OBJECT(object)->hitbox_size_x = 0x100u;
		OBJECT(object)->hitbox_size_y = 0xF3u;
		OBJECT(object)->direction = 0x300u;
		OBJECT(object)->hitbox_height = 0xA0u;
		OBJECT(object)->sprite_offset_x = 0;
		OBJECT(object)->sprite_offset_y = 0;
		OBJECT(object)->hitbox_offset_x = 0;
		OBJECT(object)->hitbox_offset_y = 0;
		OBJECT(object)->x = -0x8AD0000;
		OBJECT(object)->y = 0x114A0000;
		OBJECT(object)->z = 0x4700000;
		OBJECT(object)->handler = roller_fixed_zone_update_ADDRESS;
	}
	OBJECT(current_object)->spiked_roller.state = 0u;
	spiked_roller_chase();
}

void roller_fixed_zone_update(void)
{
	uint32_t ignored = 0;

	if (scroll_position < 0x7600u)
	{
		project_object(&ignored, &ignored);
		ignored = add_to_touchable_list(ignored);
		return;
	}
	delete_current_object();
}
