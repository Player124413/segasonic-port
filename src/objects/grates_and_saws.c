/**
 *    Name:        objects/grates_and_saws.c
 *    Description: stages D and F: the falling grates; stages C and E: the rising saws and the grate steps they cut
 *    Version:     0.1.0
 *    Created:     12-10-2026
 *    Modified:    15-10-2026
 *    Author:      Sebastian Toyos
 *
 *    SegaSonic The Hedgehog (arcade, Sega System 32): the game's V60 program decompiled into C by
 *    tools/decompile.py, with the names of symbols.txt.
 */
#include "game.h"

void falling_grate_update(void)
{
	uint8_t sound_id = 0;
	uint32_t r1 = 0, r7 = 0, at_last_step = 0, depth = 0, ignored = 0, write_index_address = 0;
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
				play_sound(sound_id, &r1, &write_index_address);
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
				play_sound(sound_id, &r1, &write_index_address);
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
				play_sound(sound_id, &r1, &write_index_address);
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
				play_sound(sound_id, &r1, &write_index_address);
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
	uint32_t block_offset = 0, ignored = 0, object = 0, object_2 = 0, path = 0, x = 0, y = 0;
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
	find_floor(&x, &y, &block_offset, &ignored, &object_2);
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
	uint32_t r1 = 0, at_last_step = 0, depth = 0, ignored = 0, object = 0, wrapped = 0, write_index_address = 0;
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
					play_sound(sound_id, &r1, &write_index_address);
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
	uint32_t block_offset = 0, ignored = 0, object = 0, slope_shift = 0, x = 0, y = 0;

	OBJECT(current_object)->handler = build_saw_shaft_base_ADDRESS;
	if (scroll_position < 0x3780u && biting_head_done == 0u)
		return;
	OBJECT(current_object)->flags = 0u;
	find_floor(&x, &y, &block_offset, &ignored, &object);
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
