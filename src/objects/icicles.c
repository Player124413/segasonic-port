/**
 *    Name:        objects/icicles.c
 *    Description: stage 9: the icicle droppers, the icicles, their cracks and shadows
 *    Version:     0.1.0
 *    Created:     12-10-2026
 *    Modified:    15-10-2026
 *    Author:      Sebastian Toyos
 *
 *    SegaSonic The Hedgehog (arcade, Sega System 32): the game's V60 program decompiled into C by
 *    tools/decompile.py, with the names of symbols.txt.
 */
#include "game.h"

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
