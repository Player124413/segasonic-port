/**
 *    Name:        stage/scroll.c
 *    Description: update_scroll: the scroll modes, the screen edges the players push, the camera velocity, the background scroll; an unused debug sprite mover
 *    Version:     0.1.0
 *    Created:     12-10-2026
 *    Modified:    15-10-2026
 *    Author:      Sebastian Toyos
 *
 *    SegaSonic The Hedgehog (arcade, Sega System 32): the game's V60 program decompiled into C by
 *    tools/decompile.py, with the names of symbols.txt.
 */
#include "game.h"

void clamp_camera_pixels(void)
{
	if ((int16_t)camera_x_pixels <= 0)
		camera_x_pixels = 0u;
	if ((int16_t)camera_x_pixels >= 0x2000)
		camera_x_pixels = 0x1FFFu;
	if ((int16_t)camera_y_pixels <= 0)
		camera_y_pixels = 0u;
	if ((int16_t)camera_y_pixels >= 0x1000)
		camera_y_pixels = 0xFFFu;
}

void update_scroll(uint32_t *p_position, uint32_t *p_speed, uint32_t *p_speed_value, uint32_t *p_next_scroll_position, uint32_t *p_second_player_object, uint32_t *p_third_player_object, uint32_t *p_end_script, uint32_t *p_dest)
{
	uint32_t position = *p_position;
	uint32_t speed = *p_speed;
	uint32_t speed_value = *p_speed_value;
	uint32_t next_scroll_position = *p_next_scroll_position;
	uint32_t second_player_object = *p_second_player_object;
	uint32_t third_player_object = *p_third_player_object;
	uint32_t end_script = *p_end_script;
	uint32_t dest = *p_dest;
	uint16_t layer_scroll_x = 0, target_heading = 0;
	uint32_t r2 = 0, r3 = 0, r4 = 0, r5 = 0, check_result = 0, count = 0, count_2 = 0, mode_script_cursor = 0, object = 0, object_2 = 0, object_3 = 0, player = 0, player_2 = 0, player_speed = 0, push_speed = 0, script = 0, sine_entry = 0, velocity_y = 0;

	if (scroll_ended < 0)
	{
	update_scroll_at_end:
		scroll_ended = -1;
		switch (stage)
		{
		case 0:
		case 1:
		case 2:
		case 4:
		case 5:
		case 6:
		case 7:
		case 8:
		case 9:
		case 10:
		case 11:
		case 12:
		case 13:
		case 14:
		case 15:
		case 16:
		case 17:
		case 18:
			*p_position = position;
			return;
		case 3:
			break;
		case 19:
			*p_position = position;
			return;
		default:
			unknown_code(0x16BE2u + stage * 3u);
		}
		if ((int8_t)stage_03_end_started <= 0)
		{
			stage_03_end_started++;
			script = stage_03_end_script_1_ADDRESS;
			run_vram_script(&count, &script, &dest);
			end_script = stage_03_end_script_2_ADDRESS;
			run_vram_script(&count, &end_script, &dest);
			nbg0_pages = 0x2020202u;
			scroll_mode_timer = 0x3Cu;
			scroll_mode_duration = scroll_mode_timer;
			scroll_mode = 4u;
			scroll_speed = 0x19u;
			stage_03_end_step = 0u;
			stage_03_end_timer = 0x78;
			stage_03_end_frames = 0u;
		}
		switch (stage_03_end_step)
		{
		case 0:
		case 4:
			stage_03_end_frames++;
			scroll_speeding_up(&position, &speed, &speed_value, &next_scroll_position, &end_script, &dest);
			if (AS8(scroll_mode) == 0u)
			{
				speed = (speed & ~0xFFFFu) | 0x25u;
				scroll_speed = 0x25u;
				stage_03_end_step++;
			}
			break;
		case 1:
			stage_03_end_frames++;
			speed = (speed & ~0xFFFFu) | scroll_speed;
			stage_03_end_timer--;
			if (stage_03_end_timer <= 0)
			{
				camera_y = 0x10000;
				end_script = stage_03_end_script_3_ADDRESS;
				run_vram_script(&count, &end_script, &dest);
				nbg0_pages = 0x2020200u;
				stage_03_end_step++;
				scroll_mode_timer = 5u;
				scroll_mode_duration = scroll_mode_timer;
				scroll_mode = 4u;
				scroll_speed = 0x19u;
			}
			break;
		case 2:
			stage_03_end_frames++;
			scroll_slowing_down(&position, &speed, &speed_value, &next_scroll_position, &end_script, &dest);
			if ((int16_t)scroll_speed <= 1)
			{
				scroll_speed = 0u;
				stage_03_end_step++;
				all_players_path_request = 0x23u;
				dest = spawn_stage_03_end_pillar(dest);
				*p_position = position;
				*p_speed = speed;
				*p_speed_value = speed_value;
				*p_next_scroll_position = next_scroll_position;
				*p_end_script = end_script;
				*p_dest = dest;
				return;
			}
			break;
		case 3:
			camera_velocity_x = 0;
			camera_velocity_y = 0;
			*p_position = position;
			*p_end_script = end_script;
			*p_dest = dest;
			return;
		default:
			unknown_code(0x174D2u + stage_03_end_step * 3u);
		}
		section_limit_y = -1;
		section_direction_y = -0x4000;
		section_direction_x = 0;
		advance_camera_y(speed);
		update_camera_view();
		layer_scroll_x = camera_x_pixels;
		nbg0_scroll_y_int = camera_y_pixels;
		nbg0_scroll_x_int = layer_scroll_x;
		nbg0_scroll_x_int &= 0x3FFu;
		nbg0_scroll_y_int &= 0x1FFu;
		speed_value = (uint16_t)speed;
		velocity_y = section_direction_y * speed_value;
		camera_velocity_x = section_direction_x * speed_value;
		camera_velocity_y = velocity_y;
		*p_position = velocity_y;
		*p_speed = speed;
		*p_speed_value = speed_value;
		*p_next_scroll_position = next_scroll_position;
		*p_end_script = end_script;
		*p_dest = dest;
		return;
	}
	position = scroll_position;
	if ((int32_t)scroll_end <= (int32_t)position)
		goto update_scroll_at_end;
	switch (scroll_mode)
	{
	case 0:
		scroll_at_scroll_speed(&position, &speed, &speed_value, &next_scroll_position, &end_script, &dest);
		*p_position = position;
		*p_speed = speed;
		*p_speed_value = speed_value;
		*p_next_scroll_position = next_scroll_position;
		*p_second_player_object = second_player_object;
		*p_third_player_object = third_player_object;
		*p_end_script = end_script;
		*p_dest = dest;
		return;
	case 1:
		scroll_speed = 0u;
		speed &= ~0xFFFFu;
		set_camera_velocity(&r2, &speed, &r4, &next_scroll_position, &end_script, &dest);
		*p_position = r2;
		*p_speed = speed;
		*p_speed_value = r4;
		*p_next_scroll_position = next_scroll_position;
		*p_second_player_object = second_player_object;
		*p_third_player_object = third_player_object;
		*p_end_script = end_script;
		*p_dest = dest;
		return;
	case 2:
		scroll_mode_timer--;
		if (scroll_mode_timer != 0u)
		{
			update_camera_view();
			speed = 0u;
			update_camera_view();
			set_camera_velocity(&r2, &speed, &r4, &next_scroll_position, &end_script, &dest);
			*p_position = r2;
			*p_speed = speed;
			*p_speed_value = r4;
			*p_next_scroll_position = next_scroll_position;
			*p_second_player_object = second_player_object;
			*p_third_player_object = third_player_object;
			*p_end_script = end_script;
			*p_dest = dest;
			return;
		}
		if (scroll_mode != 2u)
		{
			scroll_mode = 0u;
			scroll_ramp_speed = 1u;
			update_camera_view();
			set_camera_velocity(&r2, &speed, &r4, &next_scroll_position, &end_script, &dest);
			*p_position = r2;
			*p_speed = speed;
			*p_speed_value = r4;
			*p_next_scroll_position = next_scroll_position;
			*p_second_player_object = second_player_object;
			*p_third_player_object = third_player_object;
			*p_end_script = end_script;
			*p_dest = dest;
			return;
		}
		mode_script_cursor = scroll_mode_script;
		scroll_mode = ROM16(mode_script_cursor);
		mode_script_cursor += 2u;
		scroll_mode_timer = ROM16(mode_script_cursor);
		scroll_mode_duration = scroll_mode_timer;
		scroll_mode_script = mode_script_cursor + 2u;
		scroll_ramp_speed = 1u;
		set_camera_velocity(&r2, &speed, &r4, &next_scroll_position, &end_script, &dest);
		*p_position = r2;
		*p_speed = speed;
		*p_speed_value = r4;
		*p_next_scroll_position = next_scroll_position;
		*p_second_player_object = second_player_object;
		*p_third_player_object = third_player_object;
		*p_end_script = end_script;
		*p_dest = dest;
		return;
	case 3:
		scroll_slowing_down(&position, &r3, &speed_value, &next_scroll_position, &end_script, &dest);
		*p_position = position;
		*p_speed = r3;
		*p_speed_value = speed_value;
		*p_next_scroll_position = next_scroll_position;
		*p_second_player_object = second_player_object;
		*p_third_player_object = third_player_object;
		*p_end_script = end_script;
		*p_dest = dest;
		return;
	case 4:
		scroll_speeding_up(&position, &speed, &speed_value, &next_scroll_position, &end_script, &dest);
		*p_position = position;
		*p_speed = speed;
		*p_speed_value = speed_value;
		*p_next_scroll_position = next_scroll_position;
		*p_second_player_object = second_player_object;
		*p_third_player_object = third_player_object;
		*p_end_script = end_script;
		*p_dest = dest;
		return;
	case 5:
		nearby_floor_blocks_locked = 1u;
		mode_script_cursor = scroll_mode_script;
		scroll_mode = ROM16(mode_script_cursor);
		mode_script_cursor += 2u;
		scroll_mode_timer = ROM16(mode_script_cursor);
		scroll_mode_duration = scroll_mode_timer;
		scroll_mode_script = mode_script_cursor + 2u;
		scroll_ramp_speed = 1u;
		set_camera_velocity(&r2, &speed, &r4, &next_scroll_position, &end_script, &dest);
		*p_position = r2;
		*p_speed = speed;
		*p_speed_value = r4;
		*p_next_scroll_position = next_scroll_position;
		*p_second_player_object = second_player_object;
		*p_third_player_object = third_player_object;
		*p_end_script = end_script;
		*p_dest = dest;
		return;
	case 6:
		nearby_floor_blocks_locked = 0u;
		mode_script_cursor = scroll_mode_script;
		scroll_mode = ROM16(mode_script_cursor);
		mode_script_cursor += 2u;
		scroll_mode_timer = ROM16(mode_script_cursor);
		scroll_mode_duration = scroll_mode_timer;
		scroll_mode_script = mode_script_cursor + 2u;
		scroll_ramp_speed = 1u;
		set_camera_velocity(&r2, &speed, &r4, &next_scroll_position, &end_script, &dest);
		*p_position = r2;
		*p_speed = speed;
		*p_speed_value = r4;
		*p_next_scroll_position = next_scroll_position;
		*p_second_player_object = second_player_object;
		*p_third_player_object = third_player_object;
		*p_end_script = end_script;
		*p_dest = dest;
		return;
	case 7:
	case 8:
		player = PLAYER_BLOCK(1);
		player_2 = PLAYER_BLOCK(2);
		r14 = PLAYER_BLOCK(3);
		object = LINKED(WORK_RAM, player_1_object);
		object_2 = LINKED(WORK_RAM, player_2_object);
		object_3 = LINKED(WORK_RAM, player_3_object);
		players_pushing_edge = 0u;
		players_facing_scroll = 0u;
		players_in_play_bits = 0u;
		player_past_leading_edge = 0u;
		if ((OBJECT(object)->flags & 0x10000400u) == 0u)
			players_in_play_bits |= 1u;
		if ((OBJECT(object_2)->flags & 0x10000400u) == 0u)
			players_in_play_bits |= 2u;
		if ((OBJECT(object_3)->flags & 0x10000400u) == 0u)
			players_in_play_bits |= 4u;
		check_result = 0u;
		set_screen_push_edges(check_result, &count_2, &sine_entry);
		is_direction_in_quadrant(count_2, &check_result, &sine_entry);
		if ((uint8_t)check_result == 0u)
		{
			if (player_at_trailing_edge != 0u && (int16_t)((uint32_t)ROM16(section_end_list) - scroll_position) <= 0x180)
			{
				if ((int8_t)back_edge_frames >= 0x40)
					follow_base_speed = 6u;
				else
					back_edge_frames++;
			}
			push_speed = 0u;
			push_speed &= 0xFFu;
			push_speed = (uint16_t)(push_speed + follow_base_speed);
			advance_scroll(&r2, &push_speed, &r4, &r5, &player, &sine_entry);
			*p_position = r2;
			*p_speed = push_speed;
			*p_speed_value = r4;
			*p_next_scroll_position = r5;
			*p_second_player_object = object_2;
			*p_third_player_object = object_3;
			*p_end_script = player;
			*p_dest = sine_entry;
			return;
		}
		if ((players_in_play_bits & 1u) != 0u)
		{
			target_heading = PLAYER(player)->target_heading;
			check_result = is_player_facing_scroll(check_result, target_heading, (uint16_t)sine_entry);
			if ((uint8_t)check_result != 0u)
				players_facing_scroll |= 1u;
		}
		if ((players_in_play_bits & 2u) != 0u)
		{
			target_heading = PLAYER(player_2)->target_heading;
			check_result = is_player_facing_scroll(check_result, target_heading, (uint16_t)sine_entry);
			if ((uint8_t)check_result != 0u)
				players_facing_scroll |= 2u;
		}
		if ((players_in_play_bits & 4u) != 0u)
		{
			target_heading = PLAYER(r14)->target_heading;
			check_result = is_player_facing_scroll(check_result, target_heading, (uint16_t)sine_entry);
			if ((uint8_t)check_result != 0u)
				players_facing_scroll |= 4u;
		}
		player_at_trailing_edge = 0u;
		if ((OBJECT(object)->flags & 0x400u) == 0u)
		{
			check_result = is_player_pushing_edge(check_result, object);
			if ((uint8_t)check_result != 0u)
				players_pushing_edge |= 1u;
		}
		if ((OBJECT(object_2)->flags & 0x400u) == 0u)
		{
			object = object_2;
			check_result = is_player_pushing_edge(check_result, object);
			if ((uint8_t)check_result != 0u)
				players_pushing_edge |= 2u;
		}
		if ((OBJECT(object_3)->flags & 0x400u) == 0u)
		{
			object = object_3;
			check_result = is_player_pushing_edge(check_result, object);
			if ((uint8_t)check_result != 0u)
				players_pushing_edge |= 4u;
		}
		if (players_pushing_edge == 0u || (int8_t)players_pushing_edge > 7 || player_at_trailing_edge != 0u)
		{
			if (player_at_trailing_edge != 0u && (int16_t)((uint32_t)ROM16(section_end_list) - scroll_position) <= 0x180)
			{
				if ((int8_t)back_edge_frames >= 0x40)
					follow_base_speed = 6u;
				else
					back_edge_frames++;
			}
			push_speed = 0u;
			push_speed &= 0xFFu;
			push_speed = (uint16_t)(push_speed + follow_base_speed);
			advance_scroll(&r2, &push_speed, &r4, &r5, &player, &sine_entry);
			*p_position = r2;
			*p_speed = push_speed;
			*p_speed_value = r4;
			*p_next_scroll_position = r5;
			*p_second_player_object = object_2;
			*p_third_player_object = object_3;
			*p_end_script = player;
			*p_dest = sine_entry;
			return;
		}
		players_pushing_edge &= players_facing_scroll;
		players_pushing_edge &= players_in_play_bits;
		object = LINKED(object, PLAYER(player)->object);
		switch (players_pushing_edge)
		{
		case 0:
		case 8:
			if (players_facing_scroll != 0u || player_past_leading_edge == 0u)
			{
				speed = 5u;
				speed = (uint16_t)(speed + follow_base_speed);
				advance_scroll(&r2, &speed, &r4, &r5, &player, &sine_entry);
				*p_position = r2;
				*p_speed = speed;
				*p_speed_value = r4;
				*p_next_scroll_position = r5;
				*p_second_player_object = object_2;
				*p_third_player_object = object_3;
				*p_end_script = player;
				*p_dest = sine_entry;
				return;
			}
			speed = 5u;
			speed = (uint16_t)(speed + follow_base_speed);
			advance_scroll(&r2, &speed, &r4, &r5, &player, &sine_entry);
			*p_position = r2;
			*p_speed = speed;
			*p_speed_value = r4;
			*p_next_scroll_position = r5;
			*p_second_player_object = object_2;
			*p_third_player_object = object_3;
			*p_end_script = player;
			*p_dest = sine_entry;
			return;
		case 1:
		case 9:
			player_speed = (int32_t)OBJECT(object)->speed >> 16;
			if ((int32_t)player_speed > 0x500 || (int32_t)player_speed < 0)
			{
				speed = 5u;
				speed = (uint16_t)(speed + follow_base_speed);
				advance_scroll(&r2, &speed, &r4, &r5, &player, &sine_entry);
				*p_position = r2;
				*p_speed = speed;
				*p_speed_value = r4;
				*p_next_scroll_position = r5;
				*p_second_player_object = object_2;
				*p_third_player_object = object_3;
				*p_end_script = player;
				*p_dest = sine_entry;
				return;
			}
			push_speed = ((uint16_t)player_speed * 0x400u) >> 16;
			if (player_past_leading_edge == 0u)
			{
				push_speed &= 0xFFu;
				push_speed = (uint16_t)(push_speed + follow_base_speed);
				advance_scroll(&r2, &push_speed, &r4, &r5, &player, &sine_entry);
				*p_position = r2;
				*p_speed = push_speed;
				*p_speed_value = r4;
				*p_next_scroll_position = r5;
				*p_second_player_object = object_2;
				*p_third_player_object = object_3;
				*p_end_script = player;
				*p_dest = sine_entry;
				return;
			}
			push_speed = (push_speed & 0xFFu) + 5u;
			push_speed &= 0xFFu;
			push_speed = (uint16_t)(push_speed + follow_base_speed);
			advance_scroll(&r2, &push_speed, &r4, &r5, &player, &sine_entry);
			*p_position = r2;
			*p_speed = push_speed;
			*p_speed_value = r4;
			*p_next_scroll_position = r5;
			*p_second_player_object = object_2;
			*p_third_player_object = object_3;
			*p_end_script = player;
			*p_dest = sine_entry;
			return;
		case 2:
		case 10:
			object = LINKED(object, PLAYER(player_2)->object);
			player_speed = (int32_t)OBJECT(object)->speed >> 16;
			if ((int32_t)player_speed > 0x500 || (int32_t)player_speed < 0)
			{
				speed = 5u;
				speed = (uint16_t)(speed + follow_base_speed);
				advance_scroll(&r2, &speed, &r4, &r5, &player, &sine_entry);
				*p_position = r2;
				*p_speed = speed;
				*p_speed_value = r4;
				*p_next_scroll_position = r5;
				*p_second_player_object = object_2;
				*p_third_player_object = object_3;
				*p_end_script = player;
				*p_dest = sine_entry;
				return;
			}
			push_speed = ((uint16_t)player_speed * 0x400u) >> 16;
			if (player_past_leading_edge != 0u)
				push_speed = (push_speed & 0xFFu) + 5u;
			push_speed &= 0xFFu;
			push_speed = (uint16_t)(push_speed + follow_base_speed);
			advance_scroll(&r2, &push_speed, &r4, &r5, &player, &sine_entry);
			*p_position = r2;
			*p_speed = push_speed;
			*p_speed_value = r4;
			*p_next_scroll_position = r5;
			*p_second_player_object = object_2;
			*p_third_player_object = object_3;
			*p_end_script = player;
			*p_dest = sine_entry;
			return;
		case 3:
			if (OBJECT(object_2)->speed < OBJECT(object)->speed)
			{
				player_speed = (int32_t)OBJECT(object)->speed >> 16;
				if ((int32_t)player_speed > 0x500 || (int32_t)player_speed < 0)
				{
					speed = 5u;
					speed = (uint16_t)(speed + follow_base_speed);
					advance_scroll(&r2, &speed, &r4, &r5, &player, &sine_entry);
					*p_position = r2;
					*p_speed = speed;
					*p_speed_value = r4;
					*p_next_scroll_position = r5;
					*p_second_player_object = object_2;
					*p_third_player_object = object_3;
					*p_end_script = player;
					*p_dest = sine_entry;
					return;
				}
				push_speed = ((uint16_t)player_speed * 0x400u) >> 16;
				if (player_past_leading_edge != 0u)
					push_speed = (push_speed & 0xFFu) + 5u;
				push_speed &= 0xFFu;
				push_speed = (uint16_t)(push_speed + follow_base_speed);
				advance_scroll(&r2, &push_speed, &r4, &r5, &player, &sine_entry);
				*p_position = r2;
				*p_speed = push_speed;
				*p_speed_value = r4;
				*p_next_scroll_position = r5;
				*p_second_player_object = object_2;
				*p_third_player_object = object_3;
				*p_end_script = player;
				*p_dest = sine_entry;
				return;
			}
			object = object_2;
			player_speed = (int32_t)OBJECT(object)->speed >> 16;
			if ((int32_t)player_speed > 0x500 || (int32_t)player_speed < 0)
			{
				speed = 5u;
				speed = (uint16_t)(speed + follow_base_speed);
				advance_scroll(&r2, &speed, &r4, &r5, &player, &sine_entry);
				*p_position = r2;
				*p_speed = speed;
				*p_speed_value = r4;
				*p_next_scroll_position = r5;
				*p_second_player_object = object_2;
				*p_third_player_object = object_3;
				*p_end_script = player;
				*p_dest = sine_entry;
				return;
			}
			push_speed = ((uint16_t)player_speed * 0x400u) >> 16;
			if (player_past_leading_edge != 0u)
				push_speed = (push_speed & 0xFFu) + 5u;
			push_speed &= 0xFFu;
			push_speed = (uint16_t)(push_speed + follow_base_speed);
			advance_scroll(&r2, &push_speed, &r4, &r5, &player, &sine_entry);
			*p_position = r2;
			*p_speed = push_speed;
			*p_speed_value = r4;
			*p_next_scroll_position = r5;
			*p_second_player_object = object_2;
			*p_third_player_object = object_3;
			*p_end_script = player;
			*p_dest = sine_entry;
			return;
		case 4:
			object = LINKED(object, PLAYER(r14)->object);
			player_speed = (int32_t)OBJECT(object)->speed >> 16;
			if ((int32_t)player_speed > 0x500 || (int32_t)player_speed < 0)
			{
				speed = 5u;
				speed = (uint16_t)(speed + follow_base_speed);
				advance_scroll(&r2, &speed, &r4, &r5, &player, &sine_entry);
				*p_position = r2;
				*p_speed = speed;
				*p_speed_value = r4;
				*p_next_scroll_position = r5;
				*p_second_player_object = object_2;
				*p_third_player_object = object_3;
				*p_end_script = player;
				*p_dest = sine_entry;
				return;
			}
			push_speed = ((uint16_t)player_speed * 0x400u) >> 16;
			if (player_past_leading_edge != 0u)
				push_speed = (push_speed & 0xFFu) + 5u;
			push_speed &= 0xFFu;
			push_speed = (uint16_t)(push_speed + follow_base_speed);
			advance_scroll(&r2, &push_speed, &r4, &r5, &player, &sine_entry);
			*p_position = r2;
			*p_speed = push_speed;
			*p_speed_value = r4;
			*p_next_scroll_position = r5;
			*p_second_player_object = object_2;
			*p_third_player_object = object_3;
			*p_end_script = player;
			*p_dest = sine_entry;
			return;
		case 5:
			if (OBJECT(object_3)->speed < OBJECT(object)->speed)
			{
				player_speed = (int32_t)OBJECT(object)->speed >> 16;
				if ((int32_t)player_speed > 0x500 || (int32_t)player_speed < 0)
				{
					speed = 5u;
					speed = (uint16_t)(speed + follow_base_speed);
					advance_scroll(&r2, &speed, &r4, &r5, &player, &sine_entry);
					*p_position = r2;
					*p_speed = speed;
					*p_speed_value = r4;
					*p_next_scroll_position = r5;
					*p_second_player_object = object_2;
					*p_third_player_object = object_3;
					*p_end_script = player;
					*p_dest = sine_entry;
					return;
				}
				push_speed = ((uint16_t)player_speed * 0x400u) >> 16;
				if (player_past_leading_edge != 0u)
					push_speed = (push_speed & 0xFFu) + 5u;
				push_speed &= 0xFFu;
				push_speed = (uint16_t)(push_speed + follow_base_speed);
				advance_scroll(&r2, &push_speed, &r4, &r5, &player, &sine_entry);
				*p_position = r2;
				*p_speed = push_speed;
				*p_speed_value = r4;
				*p_next_scroll_position = r5;
				*p_second_player_object = object_2;
				*p_third_player_object = object_3;
				*p_end_script = player;
				*p_dest = sine_entry;
				return;
			}
			object = object_3;
			player_speed = (int32_t)OBJECT(object)->speed >> 16;
			if ((int32_t)player_speed > 0x500 || (int32_t)player_speed < 0)
			{
				speed = 5u;
				speed = (uint16_t)(speed + follow_base_speed);
				advance_scroll(&r2, &speed, &r4, &r5, &player, &sine_entry);
				*p_position = r2;
				*p_speed = speed;
				*p_speed_value = r4;
				*p_next_scroll_position = r5;
				*p_second_player_object = object_2;
				*p_third_player_object = object_3;
				*p_end_script = player;
				*p_dest = sine_entry;
				return;
			}
			push_speed = ((uint16_t)player_speed * 0x400u) >> 16;
			if (player_past_leading_edge != 0u)
				push_speed = (push_speed & 0xFFu) + 5u;
			push_speed &= 0xFFu;
			push_speed = (uint16_t)(push_speed + follow_base_speed);
			advance_scroll(&r2, &push_speed, &r4, &r5, &player, &sine_entry);
			*p_position = r2;
			*p_speed = push_speed;
			*p_speed_value = r4;
			*p_next_scroll_position = r5;
			*p_second_player_object = object_2;
			*p_third_player_object = object_3;
			*p_end_script = player;
			*p_dest = sine_entry;
			return;
		case 6:
			object = object_3;
			if (OBJECT(object_2)->speed < OBJECT(object)->speed)
			{
				player_speed = (int32_t)OBJECT(object)->speed >> 16;
				if ((int32_t)player_speed > 0x500 || (int32_t)player_speed < 0)
				{
					speed = 5u;
					speed = (uint16_t)(speed + follow_base_speed);
					advance_scroll(&r2, &speed, &r4, &r5, &player, &sine_entry);
					*p_position = r2;
					*p_speed = speed;
					*p_speed_value = r4;
					*p_next_scroll_position = r5;
					*p_second_player_object = object_2;
					*p_third_player_object = object_3;
					*p_end_script = player;
					*p_dest = sine_entry;
					return;
				}
				push_speed = ((uint16_t)player_speed * 0x400u) >> 16;
				if (player_past_leading_edge != 0u)
					push_speed = (push_speed & 0xFFu) + 5u;
				push_speed &= 0xFFu;
				push_speed = (uint16_t)(push_speed + follow_base_speed);
				advance_scroll(&r2, &push_speed, &r4, &r5, &player, &sine_entry);
				*p_position = r2;
				*p_speed = push_speed;
				*p_speed_value = r4;
				*p_next_scroll_position = r5;
				*p_second_player_object = object_2;
				*p_third_player_object = object_3;
				*p_end_script = player;
				*p_dest = sine_entry;
				return;
			}
			object = object_2;
			player_speed = (int32_t)OBJECT(object)->speed >> 16;
			if ((int32_t)player_speed > 0x500 || (int32_t)player_speed < 0)
			{
				speed = 5u;
				speed = (uint16_t)(speed + follow_base_speed);
				advance_scroll(&r2, &speed, &r4, &r5, &player, &sine_entry);
				*p_position = r2;
				*p_speed = speed;
				*p_speed_value = r4;
				*p_next_scroll_position = r5;
				*p_second_player_object = object_2;
				*p_third_player_object = object_3;
				*p_end_script = player;
				*p_dest = sine_entry;
				return;
			}
			push_speed = ((uint16_t)player_speed * 0x400u) >> 16;
			if (player_past_leading_edge != 0u)
				push_speed = (push_speed & 0xFFu) + 5u;
			push_speed &= 0xFFu;
			push_speed = (uint16_t)(push_speed + follow_base_speed);
			advance_scroll(&r2, &push_speed, &r4, &r5, &player, &sine_entry);
			*p_position = r2;
			*p_speed = push_speed;
			*p_speed_value = r4;
			*p_next_scroll_position = r5;
			*p_second_player_object = object_2;
			*p_third_player_object = object_3;
			*p_end_script = player;
			*p_dest = sine_entry;
			return;
		case 7:
			if (OBJECT(object)->speed <= OBJECT(object_2)->speed)
				object = object_2;
			if (OBJECT(object_3)->speed < OBJECT(object)->speed)
			{
				player_speed = (int32_t)OBJECT(object)->speed >> 16;
				if ((int32_t)player_speed > 0x500 || (int32_t)player_speed < 0)
				{
					speed = 5u;
					speed = (uint16_t)(speed + follow_base_speed);
					advance_scroll(&r2, &speed, &r4, &r5, &player, &sine_entry);
					*p_position = r2;
					*p_speed = speed;
					*p_speed_value = r4;
					*p_next_scroll_position = r5;
					*p_second_player_object = object_2;
					*p_third_player_object = object_3;
					*p_end_script = player;
					*p_dest = sine_entry;
					return;
				}
				push_speed = ((uint16_t)player_speed * 0x400u) >> 16;
				if (player_past_leading_edge != 0u)
					push_speed = (push_speed & 0xFFu) + 5u;
				push_speed &= 0xFFu;
				push_speed = (uint16_t)(push_speed + follow_base_speed);
				advance_scroll(&r2, &push_speed, &r4, &r5, &player, &sine_entry);
				*p_position = r2;
				*p_speed = push_speed;
				*p_speed_value = r4;
				*p_next_scroll_position = r5;
				*p_second_player_object = object_2;
				*p_third_player_object = object_3;
				*p_end_script = player;
				*p_dest = sine_entry;
				return;
			}
			object = object_3;
			player_speed = (int32_t)OBJECT(object)->speed >> 16;
			if ((int32_t)player_speed > 0x500 || (int32_t)player_speed < 0)
			{
				speed = 5u;
				speed = (uint16_t)(speed + follow_base_speed);
				advance_scroll(&r2, &speed, &r4, &r5, &player, &sine_entry);
				*p_position = r2;
				*p_speed = speed;
				*p_speed_value = r4;
				*p_next_scroll_position = r5;
				*p_second_player_object = object_2;
				*p_third_player_object = object_3;
				*p_end_script = player;
				*p_dest = sine_entry;
				return;
			}
			push_speed = ((uint16_t)player_speed * 0x400u) >> 16;
			if (player_past_leading_edge != 0u)
				push_speed = (push_speed & 0xFFu) + 5u;
			push_speed &= 0xFFu;
			push_speed = (uint16_t)(push_speed + follow_base_speed);
			advance_scroll(&r2, &push_speed, &r4, &r5, &player, &sine_entry);
			*p_position = r2;
			*p_speed = push_speed;
			*p_speed_value = r4;
			*p_next_scroll_position = r5;
			*p_second_player_object = object_2;
			*p_third_player_object = object_3;
			*p_end_script = player;
			*p_dest = sine_entry;
			return;
		default:
			unknown_code(0x16151u + players_pushing_edge * 3u);
		}
		break;
	default:
		unknown_code(0x15FA9u + scroll_mode * 3u);
	}
}

uint32_t is_player_pushing_edge(uint32_t pushing, uint32_t object)
{
	uint16_t edge_offset = 0;
	uint32_t current_position = 0;

	switch (push_edge_case)
	{
	case 0:
	case 9:
		pushing &= ~0xFFu;
		return pushing;
	case 1:
		pushing = is_player_at_left_edge(pushing, object);
		if ((uint8_t)pushing != 0u)
		{
			player_at_trailing_edge = 1u;
			pushing &= ~0xFFu;
			return pushing;
		}
		pushing = is_player_at_right_edge(pushing, object);
		if ((uint8_t)pushing != 0u)
		{
			pushing = (pushing & ~0xFFu) | 1u;
			return pushing;
		}
		pushing &= ~0xFFu;
		return pushing;
	case 2:
		pushing = is_player_at_left_edge(pushing, object);
		if ((uint8_t)pushing == 0u)
		{
			pushing = is_player_at_top_edge(pushing, object);
			if ((uint8_t)pushing != 0u)
			{
				player_at_trailing_edge = 1u;
				pushing &= ~0xFFu;
				return pushing;
			}
			edge_offset = 0xF0u;
			pushing = is_player_at_diagonal_edge_f0(pushing, object, edge_offset);
			if ((uint8_t)pushing != 0u)
			{
				pushing = (pushing & ~0xFFu) | 1u;
				return pushing;
			}
			pushing = is_player_at_right_edge(pushing, object);
			if ((uint8_t)pushing != 0u)
			{
				pushing = (pushing & ~0xFFu) | 1u;
				return pushing;
			}
			pushing = is_player_at_bottom_edge(pushing, object);
			if ((uint8_t)pushing != 0u)
			{
				pushing = (pushing & ~0xFFu) | 1u;
				return pushing;
			}
			pushing &= ~0xFFu;
			return pushing;
		}
		player_at_trailing_edge = 1u;
		pushing &= ~0xFFu;
		return pushing;
	case 3:
		pushing = is_player_at_top_edge(pushing, object);
		if ((uint8_t)pushing != 0u)
		{
			player_at_trailing_edge = 1u;
			pushing &= ~0xFFu;
			return pushing;
		}
		pushing = is_player_at_bottom_edge(pushing, object);
		if ((uint8_t)pushing != 0u)
		{
			pushing = (pushing & ~0xFFu) | 1u;
			return pushing;
		}
		pushing &= ~0xFFu;
		return pushing;
	case 4:
		pushing = is_player_at_right_edge(pushing, object);
		if ((uint8_t)pushing == 0u)
		{
			pushing = is_player_at_top_edge(pushing, object);
			if ((uint8_t)pushing != 0u)
			{
				player_at_trailing_edge = 1u;
				pushing &= ~0xFFu;
				return pushing;
			}
			edge_offset = 0x60u;
			pushing = is_player_at_diagonal_edge_60(pushing, object, edge_offset);
			if ((uint8_t)pushing != 0u)
			{
				pushing = (pushing & ~0xFFu) | 1u;
				return pushing;
			}
			pushing = is_player_at_left_edge(pushing, object);
			if ((uint8_t)pushing != 0u)
			{
				pushing = (pushing & ~0xFFu) | 1u;
				return pushing;
			}
			pushing = is_player_at_bottom_edge(pushing, object);
			if ((uint8_t)pushing != 0u)
			{
				pushing = (pushing & ~0xFFu) | 1u;
				return pushing;
			}
			pushing &= ~0xFFu;
			return pushing;
		}
		player_at_trailing_edge = 1u;
		pushing &= ~0xFFu;
		return pushing;
	case 5:
		pushing = is_player_at_right_edge(pushing, object);
		if ((uint8_t)pushing != 0u)
		{
			player_at_trailing_edge = 1u;
			pushing &= ~0xFFu;
			return pushing;
		}
		pushing = is_player_at_left_edge(pushing, object);
		if ((uint8_t)pushing != 0u)
		{
			pushing = (pushing & ~0xFFu) | 1u;
			return pushing;
		}
		pushing &= ~0xFFu;
		return pushing;
	case 6:
		pushing = is_player_at_bottom_edge(pushing, object);
		if ((uint8_t)pushing == 0u)
		{
			pushing = is_player_at_right_edge(pushing, object);
			if ((uint8_t)pushing != 0u)
			{
				player_at_trailing_edge = 1u;
				pushing &= ~0xFFu;
				return pushing;
			}
			edge_offset = 0xC0u;
			pushing = is_player_at_diagonal_edge_c0(pushing, object, edge_offset);
			if ((uint8_t)pushing != 0u)
			{
				pushing = (pushing & ~0xFFu) | 1u;
				return pushing;
			}
			pushing = is_player_at_left_edge(pushing, object);
			if ((uint8_t)pushing != 0u)
			{
				pushing = (pushing & ~0xFFu) | 1u;
				return pushing;
			}
			pushing = is_player_at_top_edge(pushing, object);
			if ((uint8_t)pushing != 0u)
			{
				pushing = (pushing & ~0xFFu) | 1u;
				return pushing;
			}
			pushing &= ~0xFFu;
			return pushing;
		}
		player_at_trailing_edge = 1u;
		pushing &= ~0xFFu;
		return pushing;
	case 7:
		pushing = is_player_at_bottom_edge(pushing, object);
		if ((uint8_t)pushing != 0u)
		{
			player_at_trailing_edge = 1u;
			pushing &= ~0xFFu;
			return pushing;
		}
		pushing = is_player_at_top_edge(pushing, object);
		if ((uint8_t)pushing != 0u)
		{
			pushing = (pushing & ~0xFFu) | 1u;
			return pushing;
		}
		pushing &= ~0xFFu;
		return pushing;
	case 8:
		pushing = is_player_at_bottom_edge(pushing, object);
		if ((uint8_t)pushing == 0u)
		{
			pushing = is_player_at_left_edge(pushing, object);
			if ((uint8_t)pushing != 0u)
			{
				player_at_trailing_edge = 1u;
				pushing &= ~0xFFu;
				return pushing;
			}
			if (stage != 3u)
			{
				edge_offset = 0x30u;
				pushing = is_player_at_diagonal_edge_30(pushing, object, edge_offset);
				if ((uint8_t)pushing != 0u)
				{
					pushing = (pushing & ~0xFFu) | 1u;
					return pushing;
				}
				pushing = is_player_at_right_edge(pushing, object);
				if ((uint8_t)pushing != 0u)
				{
					pushing = (pushing & ~0xFFu) | 1u;
					return pushing;
				}
				pushing = is_player_at_top_edge(pushing, object);
				if ((uint8_t)pushing != 0u)
				{
					pushing = (pushing & ~0xFFu) | 1u;
					return pushing;
				}
				pushing &= ~0xFFu;
				return pushing;
			}
			current_position = scroll_position;
			if ((int32_t)current_position < 0x87A9 || (int32_t)current_position > 0x96F1)
			{
				edge_offset = 0x30u;
				pushing = is_player_at_diagonal_edge_30(pushing, object, edge_offset);
				if ((uint8_t)pushing != 0u)
				{
					pushing = (pushing & ~0xFFu) | 1u;
					return pushing;
				}
				pushing = is_player_at_right_edge(pushing, object);
				if ((uint8_t)pushing != 0u)
				{
					pushing = (pushing & ~0xFFu) | 1u;
					return pushing;
				}
				pushing = is_player_at_top_edge(pushing, object);
				if ((uint8_t)pushing != 0u)
				{
					pushing = (pushing & ~0xFFu) | 1u;
					return pushing;
				}
				pushing &= ~0xFFu;
				return pushing;
			}
			pushing = is_player_at_top_edge(pushing, object);
			if ((uint8_t)pushing != 0u)
			{
				pushing = (pushing & ~0xFFu) | 1u;
				return pushing;
			}
			pushing &= ~0xFFu;
			return pushing;
		}
		player_at_trailing_edge = 1u;
		pushing &= ~0xFFu;
		return pushing;
	default:
		unknown_code(0x16274u + push_edge_case * 3u);
	}
}

uint32_t is_player_at_diagonal_edge_30(uint32_t at_edge, uint32_t object, uint16_t edge_offset)
{
	uint16_t screen_y = 0;
	uint32_t passes_left = 0;

	passes_left = 2u;
	at_edge &= ~0xFFFFu;
	player_past_leading_edge = 0u;
	do
	{
		screen_y = (uint16_t)OBJECT(object)->screen_y;
		if ((int16_t)(((int16_t)OBJECT(object)->screen_x >> 1) + edge_offset) < (int16_t)screen_y)
		{
			if ((int16_t)((uint32_t)(screen_y - edge_offset) << 1) <= OBJECT(object)->screen_x)
			{
				at_edge = (at_edge & ~0xFFu) | 1u;
				if ((uint16_t)passes_left == 1u)
					player_past_leading_edge = 1u;
			}
		}
		else
		{
			at_edge = (at_edge & ~0xFFu) | 1u;
			if ((uint16_t)passes_left == 1u)
				player_past_leading_edge = 1u;
		}
		edge_offset -= 0xAu;
		passes_left--;
	} while (passes_left != 0u);
	return at_edge;
}

uint32_t is_player_at_diagonal_edge_f0(uint32_t at_edge, uint32_t object, uint16_t edge_offset)
{
	uint16_t screen_y = 0;
	uint32_t passes_left = 0;

	passes_left = 2u;
	at_edge &= ~0xFFFFu;
	player_past_leading_edge = 0u;
	do
	{
		screen_y = (uint16_t)OBJECT(object)->screen_y;
		if ((int16_t)screen_y < (int16_t)(0u - ((int16_t)OBJECT(object)->screen_x >> 1) + edge_offset))
		{
			if ((int16_t)((0u - (screen_y - edge_offset)) << 1) <= OBJECT(object)->screen_x)
			{
				at_edge = (at_edge & ~0xFFu) | 1u;
				if ((uint16_t)passes_left == 1u)
					player_past_leading_edge = 1u;
			}
		}
		else
		{
			at_edge = (at_edge & ~0xFFu) | 1u;
			if ((uint16_t)passes_left == 1u)
				player_past_leading_edge = 1u;
		}
		edge_offset -= 0xAu;
		passes_left--;
	} while (passes_left != 0u);
	return at_edge;
}

uint32_t is_player_at_diagonal_edge_c0(uint32_t at_edge, uint32_t object, uint16_t edge_offset)
{
	uint16_t screen_y = 0;
	uint32_t passes_left = 0;

	passes_left = 2u;
	at_edge &= ~0xFFFFu;
	player_past_leading_edge = 0u;
	do
	{
		screen_y = (uint16_t)OBJECT(object)->screen_y;
		if ((int16_t)(0u - ((int16_t)OBJECT(object)->screen_x >> 1) + edge_offset) < (int16_t)screen_y)
		{
			if (OBJECT(object)->screen_x <= (int16_t)((0u - (screen_y - edge_offset)) << 1))
			{
				at_edge = (at_edge & ~0xFFu) | 1u;
				if ((uint16_t)passes_left == 1u)
					player_past_leading_edge = 1u;
			}
		}
		else
		{
			at_edge = (at_edge & ~0xFFu) | 1u;
			if ((uint16_t)passes_left == 1u)
				player_past_leading_edge = 1u;
		}
		edge_offset -= 0xAu;
		passes_left--;
	} while (passes_left != 0u);
	return at_edge;
}

uint32_t is_player_at_diagonal_edge_60(uint32_t at_edge, uint32_t object, uint16_t edge_offset)
{
	uint16_t screen_y = 0;
	uint32_t passes_left = 0;

	passes_left = 2u;
	at_edge &= ~0xFFFFu;
	player_past_leading_edge = 0u;
	do
	{
		screen_y = (uint16_t)OBJECT(object)->screen_y;
		if ((int16_t)screen_y < (int16_t)(((int16_t)OBJECT(object)->screen_x >> 1) + edge_offset))
		{
			if (OBJECT(object)->screen_x <= (int16_t)((uint32_t)(screen_y - edge_offset) << 1))
			{
				at_edge = (at_edge & ~0xFFu) | 1u;
				if ((uint16_t)passes_left == 1u)
					player_past_leading_edge = 1u;
			}
		}
		else
		{
			at_edge = (at_edge & ~0xFFu) | 1u;
			if ((uint16_t)passes_left == 1u)
				player_past_leading_edge = 1u;
		}
		edge_offset -= 0xAu;
		passes_left--;
	} while (passes_left != 0u);
	return at_edge;
}

uint32_t is_player_at_right_edge(uint32_t at_edge, uint32_t object)
{
	uint32_t leading_limit = 0;

	if (OBJECT(object)->screen_x < (int16_t)push_edge_right)
	{
		at_edge &= ~0xFFu;
		return at_edge;
	}
	leading_limit = (at_edge & ~0xFFFFu) | push_edge_right;
	leading_limit = (leading_limit & ~0xFFFFu) | (uint16_t)(leading_limit + 0xAu);
	if ((int16_t)leading_limit <= OBJECT(object)->screen_x)
		player_past_leading_edge = 1u;
	at_edge = (leading_limit & ~0xFFu) | 1u;
	return at_edge;
}

uint32_t is_player_at_left_edge(uint32_t at_edge, uint32_t object)
{
	uint32_t leading_limit = 0;

	if ((int16_t)push_edge_left < OBJECT(object)->screen_x)
	{
		at_edge &= ~0xFFu;
		return at_edge;
	}
	leading_limit = (at_edge & ~0xFFFFu) | push_edge_left;
	leading_limit = (leading_limit & ~0xFFFFu) | (uint16_t)(leading_limit - 0xAu);
	if (OBJECT(object)->screen_x <= (int16_t)leading_limit)
		player_past_leading_edge = 1u;
	at_edge = (leading_limit & ~0xFFu) | 1u;
	return at_edge;
}

uint32_t is_player_at_top_edge(uint32_t at_edge, uint32_t object)
{
	uint32_t leading_limit = 0;

	if ((int16_t)push_edge_top < OBJECT(object)->screen_y)
	{
		at_edge &= ~0xFFu;
		return at_edge;
	}
	leading_limit = (at_edge & ~0xFFFFu) | push_edge_top;
	leading_limit = (leading_limit & ~0xFFFFu) | (uint16_t)(leading_limit - 0xAu);
	if (OBJECT(object)->screen_y <= (int16_t)leading_limit)
		player_past_leading_edge = 1u;
	at_edge = (leading_limit & ~0xFFu) | 1u;
	return at_edge;
}

uint32_t is_player_at_bottom_edge(uint32_t at_edge, uint32_t object)
{
	uint32_t leading_limit = 0;

	if (OBJECT(object)->screen_y < (int16_t)push_edge_bottom)
	{
		at_edge &= ~0xFFu;
		return at_edge;
	}
	leading_limit = (at_edge & ~0xFFFFu) | push_edge_bottom;
	leading_limit = (leading_limit & ~0xFFFFu) | (uint16_t)(leading_limit + 0xAu);
	if ((int16_t)leading_limit <= OBJECT(object)->screen_y)
		player_past_leading_edge = 1u;
	at_edge = (leading_limit & ~0xFFu) | 1u;
	return at_edge;
}

void set_screen_push_edges(uint32_t quadrant_bits, uint32_t *p_count, uint32_t *p_sine_entry)
{
	uint32_t count = 0, sine_entry = 0;

	sine_entry = sine_table_ADDRESS;
	if ((int16_t)AS16(section_direction_y) < 0)
	{
		if ((int16_t)AS16(section_direction_x) >= 0)
		{
			sine_entry += 0x200u;
			quadrant_bits |= 8u;
		}
		sine_entry += 0x200u;
		quadrant_bits |= 4u;
		goto set_screen_push_edges_past_quadrant_0;
	}
	if ((int16_t)AS16(section_direction_x) < 0)
	{
	set_screen_push_edges_past_quadrant_0:
		sine_entry += 0x200u;
		quadrant_bits |= 2u;
	}
	count = 0x100u;
	push_edge_top = 0xFFFFu;
	push_edge_bottom = 0xFFFFu;
	push_edge_right = 0xFFFFu;
	push_edge_left = 0xFFFFu;
	switch (quadrant_bits | 1u)
	{
	case 0:
	case 2:
	case 4:
	case 5:
	case 6:
	case 8:
	case 9:
	case 10:
	case 11:
	case 12:
	case 13:
	case 14:
	case 16:
		*p_count = count;
		*p_sine_entry = sine_entry;
		return;
	case 1:
		push_edge_case = 0u;
		if (AS16(section_direction_y) != 0u)
		{
			push_edge_bottom = 0x88u;
			push_edge_top = 0x20u;
			push_edge_case++;
		}
		push_edge_case++;
		push_edge_right = 0xB8u;
		push_edge_left = 0x40u;
		*p_count = count;
		*p_sine_entry = sine_entry;
		return;
	case 3:
		push_edge_case = 2u;
		if (AS16(section_direction_x) != 0u)
		{
			push_edge_left = 0x88u;
			push_edge_right = 0x100u;
			push_edge_case++;
		}
		push_edge_case++;
		push_edge_bottom = 0x88u;
		push_edge_top = 0x20u;
		*p_count = count;
		*p_sine_entry = sine_entry;
		return;
	case 7:
		push_edge_case = 4u;
		if (AS16(section_direction_y) != 0u)
		{
			push_edge_top = 0x58u;
			push_edge_bottom = 0xC0u;
			push_edge_case++;
		}
		push_edge_case++;
		push_edge_left = 0x88u;
		push_edge_right = 0x100u;
		*p_count = count;
		*p_sine_entry = sine_entry;
		return;
	case 15:
		push_edge_case = 6u;
		if (AS16(section_direction_x) != 0u)
		{
			push_edge_right = 0xB8u;
			push_edge_left = 0x40u;
			push_edge_case++;
		}
		push_edge_case++;
		push_edge_top = 0x58u;
		push_edge_bottom = 0xC0u;
		*p_count = count;
		*p_sine_entry = sine_entry;
		return;
	default:
		unknown_code(0x16726u + (quadrant_bits | 1u) * 3u);
	}
}

uint32_t is_player_facing_scroll(uint32_t facing, uint16_t heading, uint16_t sine_entry)
{
	uint16_t sine_offset = 0;

	heading = (uint16_t)((uint32_t)heading << 1);
	sine_offset = (uint16_t)(sine_entry - sine_table_ADDRESS);
	if ((int16_t)sine_offset < 0x200)
	{
		if ((int16_t)heading < 0x100 || (int16_t)heading > 0x500)
		{
			facing &= ~0xFFu;
			return facing;
		}
		facing = (facing & ~0xFFu) | 1u;
		return facing;
	}
	if ((int16_t)sine_offset < 0x400)
	{
		if ((int16_t)heading < 0x300 || (int16_t)heading > 0x700)
		{
			facing &= ~0xFFu;
			return facing;
		}
		facing = (facing & ~0xFFu) | 1u;
		return facing;
	}
	if ((int16_t)sine_offset < 0x600)
	{
		if ((int16_t)heading < 0x500)
			heading += 0x800u;
		if ((int16_t)heading > 0x900)
		{
			facing &= ~0xFFu;
			return facing;
		}
		facing = (facing & ~0xFFu) | 1u;
		return facing;
	}
	if ((int16_t)heading < 0x700)
		heading += 0x800u;
	if ((int16_t)heading > 0xB00)
	{
		facing &= ~0xFFu;
		return facing;
	}
	facing = (facing & ~0xFFu) | 1u;
	return facing;
}

void is_direction_in_quadrant(uint32_t count, uint32_t *p_found, uint32_t *p_sine_entry)
{
	uint32_t sine_entry = *p_sine_entry;
	uint8_t entry_at_least = 0, found = 0, next_at_least = 0;
	uint16_t direction_y = 0, next_entry = 0;

	for (;;)
	{
		if (AS16(section_direction_y) != 0u)
		{
			if (AS16(section_direction_x) == 0u && (AS16(section_direction_y) == 0x4000u || AS16(section_direction_y) == 0xC000u))
			{
				found = 1u;
				*p_found = (*p_found & 0xFFFFFF00u) | found;
				*p_sine_entry = sine_entry;
				return;
			}
		}
		else if (AS16(section_direction_x) == 0x4000u || AS16(section_direction_x) == 0xC000u)
			break;
		direction_y = AS16(section_direction_y);
		next_entry = mem_read16(sine_entry + 2u);
		if ((int16_t)mem_read16(sine_entry) < (int16_t)direction_y)
			entry_at_least = 0u;
		else
			entry_at_least = 1u;
		if ((int16_t)next_entry < (int16_t)direction_y)
			next_at_least = 0u;
		else
			next_at_least = 1u;
		if (next_at_least != entry_at_least)
		{
			found = 1u;
			*p_found = (*p_found & 0xFFFFFF00u) | found;
			*p_sine_entry = sine_entry;
			return;
		}
		sine_entry += 2u;
		count--;
		if (count == 0u)
		{
			found = 0u;
			*p_found = (*p_found & 0xFFFFFF00u) | found;
			*p_sine_entry = sine_entry;
			return;
		}
	}
	found = 1u;
	*p_found = (*p_found & 0xFFFFFF00u) | found;
	*p_sine_entry = sine_entry;
}

void scroll_slowing_down(uint32_t *p_other_list, uint32_t *p_speed, uint32_t *p_speed_value, uint32_t *p_next_scroll_position, uint32_t *p_range, uint32_t *p_quadrant_cursor)
{
	uint32_t other_list = *p_other_list;
	uint32_t speed_value = *p_speed_value;
	uint32_t next_scroll_position = *p_next_scroll_position;
	uint32_t range = *p_range;
	uint32_t quadrant_cursor = *p_quadrant_cursor;
	uint32_t speed = 0;

	scroll_mode_timer--;
	if (scroll_mode_timer != 0u || (int16_t)scroll_speed < 2)
	{
		scroll_at_scroll_speed(&other_list, &speed, &speed_value, &next_scroll_position, &range, &quadrant_cursor);
		*p_other_list = other_list;
		*p_speed = speed;
		*p_speed_value = speed_value;
		*p_next_scroll_position = next_scroll_position;
		*p_range = range;
		*p_quadrant_cursor = quadrant_cursor;
		return;
	}
	scroll_speed--;
	scroll_mode_timer = scroll_mode_duration;
	scroll_at_scroll_speed(&other_list, &speed, &speed_value, &next_scroll_position, &range, &quadrant_cursor);
	*p_other_list = other_list;
	*p_speed = speed;
	*p_speed_value = speed_value;
	*p_next_scroll_position = next_scroll_position;
	*p_range = range;
	*p_quadrant_cursor = quadrant_cursor;
}

void scroll_speeding_up(uint32_t *p_other_list, uint32_t *p_speed, uint32_t *p_speed_value, uint32_t *p_next_scroll_position, uint32_t *p_range, uint32_t *p_quadrant_cursor)
{
	uint32_t other_list = *p_other_list;
	uint32_t speed = *p_speed;
	uint32_t speed_value = *p_speed_value;
	uint32_t next_scroll_position = *p_next_scroll_position;
	uint32_t range = *p_range;
	uint32_t quadrant_cursor = *p_quadrant_cursor;
	uint32_t r2 = 0, r4 = 0;

	scroll_mode_timer--;
	if (scroll_mode_timer != 0u)
	{
		speed = scroll_ramp_speed;
		advance_scroll_unless_ended(&other_list, &speed, &speed_value, &next_scroll_position, &range, &quadrant_cursor);
		*p_other_list = other_list;
		*p_speed = speed;
		*p_speed_value = speed_value;
		*p_next_scroll_position = next_scroll_position;
		*p_range = range;
		*p_quadrant_cursor = quadrant_cursor;
		return;
	}
	if ((int8_t)AS8(scroll_speed) <= (int8_t)scroll_ramp_speed)
	{
		scroll_mode = 0u;
		scroll_ramp_speed = 1u;
		set_camera_velocity(&r2, &speed, &r4, &next_scroll_position, &range, &quadrant_cursor);
		*p_other_list = r2;
		*p_speed = speed;
		*p_speed_value = r4;
		*p_next_scroll_position = next_scroll_position;
		*p_range = range;
		*p_quadrant_cursor = quadrant_cursor;
		return;
	}
	scroll_ramp_speed++;
	scroll_mode_timer = scroll_mode_duration;
	speed = scroll_ramp_speed;
	advance_scroll_unless_ended(&other_list, &speed, &speed_value, &next_scroll_position, &range, &quadrant_cursor);
	*p_other_list = other_list;
	*p_speed = speed;
	*p_speed_value = speed_value;
	*p_next_scroll_position = next_scroll_position;
	*p_range = range;
	*p_quadrant_cursor = quadrant_cursor;
}

void scroll_at_scroll_speed(uint32_t *p_other_list, uint32_t *p_speed, uint32_t *p_speed_value, uint32_t *p_next_scroll_position, uint32_t *p_range, uint32_t *p_quadrant_cursor)
{
	uint32_t other_list = *p_other_list;
	uint32_t speed_value = *p_speed_value;
	uint32_t next_scroll_position = *p_next_scroll_position;
	uint32_t range = *p_range;
	uint32_t quadrant_cursor = *p_quadrant_cursor;
	uint32_t speed = 0;

	speed = scroll_speed;
	advance_scroll_unless_ended(&other_list, &speed, &speed_value, &next_scroll_position, &range, &quadrant_cursor);
	*p_other_list = other_list;
	*p_speed = speed;
	*p_speed_value = speed_value;
	*p_next_scroll_position = next_scroll_position;
	*p_range = range;
	*p_quadrant_cursor = quadrant_cursor;
}

void advance_scroll_unless_ended(uint32_t *p_other_list, uint32_t *p_speed, uint32_t *p_speed_value, uint32_t *p_next_scroll_position, uint32_t *p_range, uint32_t *p_quadrant_cursor)
{
	uint32_t other_list = *p_other_list;
	uint32_t speed = *p_speed;
	uint32_t speed_value = *p_speed_value;
	uint32_t next_scroll_position = *p_next_scroll_position;
	uint32_t range = *p_range;
	uint32_t quadrant_cursor = *p_quadrant_cursor;
	uint32_t r2 = 0, r4 = 0, r5 = 0;

	if (scroll_ended < 0)
		return;
	advance_scroll(&r2, &speed, &r4, &r5, &range, &quadrant_cursor);
	*p_other_list = r2;
	*p_speed = speed;
	*p_speed_value = r4;
	*p_next_scroll_position = r5;
	*p_range = range;
	*p_quadrant_cursor = quadrant_cursor;
}

void advance_scroll(uint32_t *p_other_list, uint32_t *p_speed, uint32_t *p_speed_value, uint32_t *p_next_scroll_position, uint32_t *p_range, uint32_t *p_quadrant_cursor)
{
	uint32_t speed = *p_speed;
	uint32_t range = *p_range;
	uint32_t quadrant_cursor = *p_quadrant_cursor;
	uint32_t r2 = 0, r4 = 0, direction_y_list = 0, limit_x_cursor = 0, limit_y_cursor = 0, list = 0, mode_script_cursor = 0, next_scroll_position = 0;
	uint16_t direction_y = 0;

	next_scroll_position = move_camera_along_path(speed);
	update_camera_view();
	if ((int32_t)scroll_position < (int32_t)ROM16(section_end_list))
	{
		set_camera_velocity(&r2, &speed, &r4, &next_scroll_position, &range, &quadrant_cursor);
		*p_other_list = r2;
		*p_speed = speed;
		*p_speed_value = r4;
		*p_next_scroll_position = next_scroll_position;
		*p_range = range;
		*p_quadrant_cursor = quadrant_cursor;
		return;
	}
	back_edge_frames = 0u;
	list = section_speed_list;
	scroll_speed = ROM8(list);
	section_speed_list = list + 1u;
	list = follow_base_speed_list;
	follow_base_speed = mem_read8(list);
	follow_base_speed_list = list + 1u;
	limit_y_cursor = section_limit_y_list;
	section_limit_y = ROM16(limit_y_cursor);
	section_limit_y_list = limit_y_cursor + 2u;
	limit_x_cursor = section_limit_x_list;
	section_limit_x = ROM16(limit_x_cursor);
	section_limit_x_list = limit_x_cursor + 2u;
	mode_script_cursor = scroll_mode_script;
	scroll_mode = ROM16(mode_script_cursor);
	mode_script_cursor += 2u;
	scroll_mode_timer = ROM16(mode_script_cursor);
	scroll_mode_duration = scroll_mode_timer;
	scroll_mode_script = mode_script_cursor + 2u;
	list = section_direction_x_list;
	direction_y_list = section_direction_y_list;
	direction_y = ROM16(direction_y_list);
	section_direction_x = (int16_t)ROM16(list);
	section_direction_y = (int16_t)direction_y;
	section_direction_x_list = list + 2u;
	section_direction_y_list = direction_y_list + 2u;
	section_end_list += 2u;
	section_unused_cursor += 2u;
	set_camera_velocity(&r2, &speed, &r4, &next_scroll_position, &range, &quadrant_cursor);
	*p_other_list = r2;
	*p_speed = speed;
	*p_speed_value = r4;
	*p_next_scroll_position = next_scroll_position;
	*p_range = range;
	*p_quadrant_cursor = quadrant_cursor;
}

void set_camera_velocity(uint32_t *p_scroll_y, uint32_t *p_speed, uint32_t *p_speed_value, uint32_t *p_next_scroll_position, uint32_t *p_range, uint32_t *p_quadrant_cursor)
{
	uint32_t speed = *p_speed;
	uint32_t next_scroll_position = *p_next_scroll_position;
	uint32_t range = *p_range;
	uint32_t quadrant_cursor = *p_quadrant_cursor;
	uint32_t r2 = 0, r10 = 0, ignored = 0, speed_value = 0, velocity_y = 0;

	speed_value = (uint16_t)speed;
	velocity_y = section_direction_y * speed_value;
	camera_velocity_x = section_direction_x * speed_value;
	camera_velocity_y = velocity_y;
	camera_shake_length = 0x200u;
	camera_shake_wave = 0x200u;
	r2 = update_camera_with_shake();
	if (scroll_ended < 0)
	{
		*p_scroll_y = r2;
		*p_speed_value = speed_value;
		return;
	}
	update_background_scroll(&ignored, &r2, &speed, &speed_value, &next_scroll_position, &r10, &quadrant_cursor);
	*p_scroll_y = r2;
	*p_speed = speed;
	*p_speed_value = speed_value;
	*p_next_scroll_position = next_scroll_position;
	*p_range = r10;
	*p_quadrant_cursor = quadrant_cursor;
}

void update_background_scroll(uint32_t *p_fast_scroll_start, uint32_t *p_scroll_y, uint32_t *p_speed, uint32_t *p_speed_value, uint32_t *p_next_scroll_position, uint32_t *p_range, uint32_t *p_quadrant_cursor)
{
	uint32_t scroll_y = *p_scroll_y;
	uint32_t speed = *p_speed;
	uint32_t speed_value = *p_speed_value;
	uint32_t next_scroll_position = *p_next_scroll_position;
	uint32_t quadrant_cursor = *p_quadrant_cursor;
	uint32_t current_position = 0, fast_scroll_start = 0, range = 0, section_start = 0;

	update_nbg1_scroll();
	update_nbg3_scroll(&scroll_y, &speed, &speed_value, &next_scroll_position, &quadrant_cursor);
	speed = update_nbg2_scroll(speed);
	fast_scroll_start = stage;
	range = stage_fast_scroll_ranges[fast_scroll_start] + stage_fast_scroll_ranges_ADDRESS;
	section_start = mem_read16(section_end_list - 2u);
	do
	{
		if (ROM16(range) == 0xFFFFu)
		{
			*p_fast_scroll_start = fast_scroll_start;
			*p_scroll_y = scroll_y;
			*p_speed = speed;
			*p_speed_value = speed_value;
			*p_next_scroll_position = next_scroll_position;
			*p_range = range;
			*p_quadrant_cursor = quadrant_cursor;
			return;
		}
		fast_scroll_start = ROM16(range);
		range += 2u;
	} while ((int32_t)fast_scroll_start <= (int32_t)section_start);
	current_position = scroll_position;
	if ((int32_t)current_position < (int32_t)fast_scroll_start)
	{
		*p_fast_scroll_start = fast_scroll_start;
		*p_scroll_y = current_position;
		*p_speed = speed;
		*p_speed_value = speed_value;
		*p_next_scroll_position = next_scroll_position;
		*p_range = range;
		*p_quadrant_cursor = quadrant_cursor;
		return;
	}
	follow_base_speed = 0x10u;
	*p_fast_scroll_start = fast_scroll_start;
	*p_scroll_y = current_position;
	*p_speed = speed;
	*p_speed_value = speed_value;
	*p_next_scroll_position = next_scroll_position;
	*p_range = range;
	*p_quadrant_cursor = quadrant_cursor;
}

void spawn_debug_sprite_mover(uint32_t object)
{
	uint32_t free_count = 0;

	if (debug_sprite_spawned == 0u)
	{
		allocate_object_outside_loop(&free_count, &object);
		if ((uint8_t)free_count != 0u)
		{
			OBJECT(object)->handler = debug_sprite_mover_update_ADDRESS;
			debug_sprite_spawned++;
		}
	}
}

void debug_sprite_mover_update(void)
{
	uint32_t attribute = 0, text_position = 0, value = 0;

	OBJECT(current_object)->screen_x = debug_sprite_x;
	OBJECT(current_object)->screen_y = debug_sprite_y;
	OBJECT(current_object)->sprite_layer = 0x32u;
	OBJECT(current_object)->sprite_frame = 0x8DB8u;
	OBJECT(current_object)->sprite_palette = 0x130u;
	OBJECT(current_object)->sprite_flags = 0u;
	OBJECT(current_object)->sprite_scale = 0x100u;
	queue_sprite_in_layer();
	if ((input_ports_active & 0x10000u) != 0u)
		debug_sprite_x++;
	if ((input_ports_active & 0x10u) != 0u)
		debug_sprite_x--;
	if ((input_ports_active & 0x1000000u) != 0u)
		debug_sprite_y++;
	if ((input_ports_active & 0x2000000u) != 0u)
		debug_sprite_y--;
	value = debug_sprite_x;
	text_position = TEXT_CELL(3, 22);
	attribute = 0x200u;
	draw_hex_4(&value, &attribute, &text_position);
	value = debug_sprite_y;
	text_position = TEXT_CELL(8, 22);
	attribute = 0x200u;
	draw_hex_4(&value, &attribute, &text_position);
}
