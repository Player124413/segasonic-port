/**
 *    Name:        objects/burning_boulders.c
 *    Description: stage 8: the collapsing log bridge, the burning boulders, the falling embers
 *    Version:     0.1.0
 *    Created:     12-10-2026
 *    Modified:    15-10-2026
 *    Author:      Sebastian Toyos
 *
 *    SegaSonic The Hedgehog (arcade, Sega System 32): the game's V60 program decompiled into C by
 *    tools/decompile.py, with the names of symbols.txt.
 */
#include "game.h"

void collapsing_bridge_plank_update(void)
{
	uint8_t sound_id = 0, start_frame = 0;
	uint16_t x = 0, y = 0;
	uint32_t angle_whole = 0, depth = 0, ignored = 0, kind = 0, object = 0, plank_index = 0, sag = 0, sag_table = 0, sign = 0, swing_dx = 0, swing_dz = 0, write_index_address = 0, z = 0;

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
				play_sound(sound_id, &sag, &write_index_address);
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
					play_sound(sound_id, &swing_dz, &write_index_address);
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
				project_object(&ignored, &depth);
				if (OBJECT(current_object)->screen_y > 0xE0)
					return;
				if ((int8_t)OBJECT(current_object)->collapsing_bridge_plank.swing_count < 1)
				{
					depth = queue_sprite(depth);
					return;
				}
				depth = 0u;
				queue_sprite_at_depth(&ignored, &depth);
				return;
			}
			OBJECT(current_object)->sprite_frame = mem_read16(OBJECT(current_object)->animation + 2u);
		}
		project_object(&ignored, &depth);
		if (OBJECT(current_object)->screen_y > 0xE0)
			return;
		if ((int8_t)OBJECT(current_object)->collapsing_bridge_plank.swing_count < 1)
		{
			depth = queue_sprite(depth);
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
	uint32_t block_offset = 0, boulder_record = 0, ignored = 0, object = 0, object_2 = 0, x = 0, zone_index = 0;
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
	find_floor(&x, &zone_index, &block_offset, &ignored, &object);
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
	uint32_t r2 = 0, r7 = 0, cursor = 0, depth = 0, drop_step = 0, flames_left = 0, floor_slope = 0, ignored = 0, object = 0, path_cursor = 0, slope_factor = 0, speed_level = 0, step_y = 0, x = 0;
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
		find_floor(&x, &step_y, &r2, &ignored, &cursor);
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
		project_object(&ignored, &depth);
		depth = queue_sprite(depth);
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
	uint32_t block_offset = 0, cursor = 0, ignored = 0, x = 0, y = 0;

	OBJECT(current_object)->ember_emitter.spawn_timer = 0;
	OBJECT(current_object)->floor_slope = 0u;
	OBJECT(current_object)->floor_type = 0u;
	OBJECT(current_object)->flags = 0u;
	OBJECT(current_object)->ember_emitter.end_position = ember_emitter_end_positions[OBJECT(current_object)->sprite_frame];
	find_floor(&x, &y, &block_offset, &ignored, &cursor);
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
	uint32_t r1 = 0, block_offset = 0, burst_animation = 0, cursor = 0, depth = 0, ignored = 0, off_screen = 0, wrapped = 0, x = 0;

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
		find_floor(&x, &r1, &block_offset, &ignored, &cursor);
		if (OBJECT(current_object)->falling_ember.floor_block_high < 0)
			OBJECT(current_object)->floor_z = 0x5300000;
		project_object(&ignored, &depth);
		if (OBJECT(current_object)->z_int < OBJECT(current_object)->floor_z_int)
		{
			depth = queue_sprite(depth);
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
		depth = queue_sprite(depth);
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
