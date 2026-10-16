/**
 *    Name:        stage/events.c
 *    Description: the scripted game modes run_game_mode jumps into and what they spawn: platforms, rocks, lightning, message boxes, the escape alarm and countdown
 *    Version:     0.1.0
 *    Created:     12-10-2026
 *    Modified:    16-10-2026
 *    Author:      Sebastian Toyos
 *
 *    SegaSonic The Hedgehog (arcade, Sega System 32): the game's V60 program decompiled into C by
 *    tools/decompile.py, with the names of symbols.txt.
 */
#include "game.h"

void escape_alarm_flash(void)
{
	uint8_t sound_id = 0;
	uint32_t ignored = 0;

	if (escape_seconds == 0u)
	{
		delete_current_object();
		return;
	}
	if (OBJECT(current_object)->escape_alarm_flash.started == 0u)
	{
		mixer_nbg0_blend |= 0x4000u;
		mixer_nbg1_blend |= 0x4000u;
		mixer_nbg2_blend |= 0x4000u;
		mixer_nbg3_blend |= 0x4000u;
		mixer_bitmap_blend |= 0x4000u;
		escape_alarm_stray_flags |= 0x4000u;
		color_offset_red = 0u;
		color_offset_blue = 0x40u;
		color_offset_green = 0x40u;
		OBJECT(current_object)->escape_alarm_flash.counting_up = 0u;
		OBJECT(current_object)->escape_alarm_flash.step_timer = 0;
		OBJECT(current_object)->escape_alarm_flash.started++;
	}
	OBJECT(current_object)->escape_alarm_flash.step_timer++;
	if (OBJECT(current_object)->escape_alarm_flash.step_timer < 2)
		return;
	OBJECT(current_object)->escape_alarm_flash.step_timer = 0;
	if (OBJECT(current_object)->escape_alarm_flash.counting_up != 0u)
	{
		color_offset_blue++;
		color_offset_green++;
		if (color_offset_blue == 0x3Fu)
		{
			OBJECT(current_object)->escape_alarm_flash.counting_up = 0u;
			sound_id = 0x19u;
			play_sound(sound_id, &ignored, &ignored);
		}
	}
	else
	{
		color_offset_blue--;
		color_offset_green--;
		if (color_offset_blue == 0x28u)
		{
			OBJECT(current_object)->escape_alarm_flash.counting_up = 1u;
			sound_id = 0x19u;
			play_sound(sound_id, &ignored, &ignored);
		}
	}
}

void event_0f_start(void)
{
	scene_timer = 0;
	scroll_mode = 0u;
	scroll_speed_low = 3;
	scene_step++;
}

void event_0f_stop_scroll(void)
{
	scene_timer++;
	if (scene_timer >= 0x3C)
		scroll_mode = 1u;
	if (scene_timer >= 0x78)
	{
		scene_timer = 0;
		event_0f_message_cursor = event_0f_messages_ADDRESS;
		scene_step++;
	}
}

void event_0f_show_messages(uint32_t *p_frames, uint32_t *p_box)
{
	uint16_t frames = (uint16_t)*p_frames;
	uint32_t box = *p_box;
	uint16_t kind = 0, screen_x = 0, screen_y = 0;
	uint32_t cursor = 0;
	uint16_t show_time = 0;

	scene_timer++;
	cursor = event_0f_message_cursor;
	show_time = ROM16(cursor);
	cursor += 2u;
	if ((int16_t)show_time <= scene_timer)
	{
		screen_x = ROM16(cursor);
		cursor += 2u;
		screen_y = ROM16(cursor);
		cursor += 2u;
		kind = ROM16(cursor);
		cursor += 2u;
		frames = ROM16(cursor);
		event_0f_message_cursor = cursor + 2u;
		box = spawn_message_box(screen_x, screen_y, kind, frames, box);
	}
	if (scene_timer < 0x258)
	{
		*p_frames = (*p_frames & 0xFFFF0000u) | frames;
		*p_box = box;
		return;
	}
	scroll_mode = 0u;
	scroll_speed_low = 5;
	game_mode = 3u;
	*p_frames = (*p_frames & 0xFFFF0000u) | frames;
	*p_box = box;
}

void spawn_falling_rocks(uint32_t object)
{
	uint32_t free_count = 0, rock_record = 0, rocks_left = 0;

	rocks_left = falling_rock_count;
	rock_record = falling_rocks_ADDRESS;
	do
	{
		allocate_object_outside_loop(&free_count, &object);
		if (free_count == 0u)
			break;
		OBJECT(object)->x_int = ROM16(rock_record);
		OBJECT(object)->x_fraction = 0u;
		rock_record += 2u;
		OBJECT(object)->y_int = ROM16(rock_record);
		OBJECT(object)->y_fraction = 0u;
		rock_record += 2u;
		OBJECT(object)->z_int = ROM16(rock_record);
		OBJECT(object)->z_fraction = 0u;
		OBJECT(object)->sprite_flags = 0x1000u;
		OBJECT(object)->sprite_palette = 0x8240u;
		OBJECT(object)->sprite_offset_x = 0;
		OBJECT(object)->sprite_offset_y = 0;
		OBJECT(object)->flags = 0u;
		OBJECT(object)->handler = falling_rock_bounce_ADDRESS;
		rock_record += 2u;
		OBJECT(object)->speed_x = (uint32_t)(int16_t)ROM16(rock_record) << 12;
		rock_record += 2u;
		OBJECT(object)->speed_y = (uint32_t)(int16_t)ROM16(rock_record) << 12;
		OBJECT(object)->speed_z = 0;
		OBJECT(object)->animation_frame = 0u;
		OBJECT(object)->animation_speed = 4u;
		OBJECT(object)->animation_timer = 0;
		rock_record += 2u;
		OBJECT(object)->animation = ROM32(rock_record);
		rock_record += 4u;
		rocks_left--;
	} while (rocks_left != 0u);
}

void falling_rock_bounce(void)
{
	uint32_t bounce_speed = 0, ignored = 0, off_screen = 0;

	if ((OBJECT(current_object)->flags & 0x400u) != 0u)
	{
		project_object(&ignored, &ignored);
		off_screen = check_left_screen();
		if (off_screen == 0u)
		{
			ignored = queue_sprite(ignored);
			return;
		}
		delete_current_object();
		return;
	}
	OBJECT(current_object)->x += OBJECT(current_object)->speed_x;
	OBJECT(current_object)->y += OBJECT(current_object)->speed_y;
	OBJECT(current_object)->speed_z += 0x6000u;
	OBJECT(current_object)->z += OBJECT(current_object)->speed_z;
	find_floor(&ignored, &ignored, &ignored, &ignored, &ignored);
	if (OBJECT(current_object)->z_int < OBJECT(current_object)->floor_z_int)
	{
		animate(&ignored, &ignored, &ignored);
		project_object(&ignored, &ignored);
		off_screen = check_left_screen();
		if (off_screen == 0u)
		{
			ignored = queue_sprite(ignored);
			return;
		}
		delete_current_object();
		return;
	}
	OBJECT(current_object)->z = OBJECT(current_object)->floor_z;
	bounce_speed = (int32_t)OBJECT(current_object)->speed_z >> 1;
	if ((int32_t)bounce_speed > 0x20000)
	{
		OBJECT(current_object)->speed_z = 0u - bounce_speed;
		animate(&ignored, &ignored, &ignored);
		project_object(&ignored, &ignored);
		off_screen = check_left_screen();
		if (off_screen == 0u)
		{
			ignored = queue_sprite(ignored);
			return;
		}
		delete_current_object();
		return;
	}
	OBJECT(current_object)->flags |= 0x400u;
	animate(&ignored, &ignored, &ignored);
	project_object(&ignored, &ignored);
	off_screen = check_left_screen();
	if (off_screen == 0u)
	{
		ignored = queue_sprite(ignored);
		return;
	}
	delete_current_object();
}

void event_0c_start(uint32_t *p_block_offset, uint32_t *p_slope_height, uint32_t *p_object)
{
	uint32_t block_offset = *p_block_offset;
	uint32_t slope_height = *p_slope_height;
	uint32_t object = *p_object;

	scene_timer = 0x3C;
	spawn_door_rollers(&block_offset, &slope_height, &object);
	scene_step++;
	scroll_mode = 0u;
	scroll_speed = 2u;
	*p_block_offset = block_offset;
	*p_slope_height = slope_height;
	*p_object = object;
}

uint32_t event_0c_first_quake(uint32_t write_index_address)
{
	uint8_t sound_id = 0;
	uint32_t ignored = 0;
	uint16_t time_left = 0;

	if (scene_timer <= 0x1E)
		scroll_mode = 1u;
	if ((uint16_t)scene_timer == 0x3Cu)
	{
		camera_shake_active = 1u;
		sound_id = 0xFu;
		play_sound(sound_id, &ignored, &write_index_address);
	}
	time_left = scene_timer;
	scene_timer = time_left - 1u;
	if ((int16_t)time_left <= 1)
	{
		scene_step++;
		scene_timer = 0x3C;
	}
	return write_index_address;
}

void event_0c_second_quake(uint32_t *p_part_size, uint32_t *p_point_y, uint32_t *p_z, uint32_t *p_wall)
{
	uint32_t part_size = *p_part_size;
	uint32_t point_y = *p_point_y;
	uint32_t z = *p_z;
	uint32_t wall = *p_wall;
	uint8_t sound_id = 0;
	uint32_t ignored = 0;
	uint16_t time_left = 0;

	if ((uint16_t)scene_timer == 0x3Cu)
	{
		camera_shake_active = 1u;
		sound_id = 0xFu;
		play_sound(sound_id, &ignored, &part_size);
	}
	spawn_walls(&ignored, &part_size, &ignored, &point_y, &z, &wall);
	time_left = scene_timer;
	scene_timer = time_left - 1u;
	if ((int16_t)time_left > 1)
	{
		*p_part_size = part_size;
		*p_point_y = point_y;
		*p_z = z;
		*p_wall = wall;
		return;
	}
	scene_step++;
	scene_timer = 0xA;
	sound_id = 0xAu;
	play_sound(sound_id, &ignored, &part_size);
	*p_part_size = part_size;
	*p_point_y = point_y;
	*p_z = z;
	*p_wall = wall;
}

void event_0c_finish(void)
{
	uint16_t time_left = 0;

	time_left = scene_timer;
	scene_timer = time_left - 1u;
	if ((int16_t)time_left <= 1)
	{
		game_mode = 3u;
		scroll_mode = 7u;
	}
}

void update_stage_05_event(uint32_t position, uint32_t speed, uint32_t object, uint32_t *p_speed_value, uint32_t *p_count, uint32_t *p_cycle, uint32_t *p_list_cursor)
{
	uint32_t speed_value = *p_speed_value;
	uint32_t count = *p_count;
	uint32_t cycle = *p_cycle;
	uint32_t list_cursor = *p_list_cursor;
	uint8_t sound_id = 0, spawned = 0;
	uint16_t kind = 0, screen_x = 0, screen_y = 0;
	uint32_t frames = 0, free_count = 0, ignored = 0, object_2 = 0, piece_record = 0, platform = 0, rocks_left = 0, scroll_y = 0;

	spawned = (uint8_t)(scene_step * 3u);
	switch (scene_step)
	{
	case 0:
	case 8:
		scene_timer = 0;
		unused_stage_05_event_long = 0u;
		scene_step++;
		update_player_summary(&speed, &count, &ignored, &object);
		update_scroll(&position, &speed, &speed_value, &ignored, &count, &cycle, &ignored, &object);
		update_stage_tile_layers(&ignored, &position, &ignored, &speed_value, &ignored, &count, &cycle, &ignored, &object);
		check_hits(&ignored, &position, &ignored, &speed_value, &ignored, &count, &ignored);
		check_walls(ignored);
		check_platforms(&ignored, &ignored, &ignored, &speed_value, &ignored, &count, &cycle, &platform, &ignored);
		goal_unlocked = 0xFFu;
		check_stage_clear(&count, &platform, &ignored);
		if (game_mode != 0x17u)
		{
			*p_speed_value = speed_value;
			*p_count = count;
			*p_cycle = cycle;
			*p_list_cursor = platform;
			return;
		}
		game_mode = 0xAu;
		scene_step++;
		scene_timer = 0;
		goal_scroll_position = 0xFFFFu;
		*p_speed_value = speed_value;
		*p_count = count;
		*p_cycle = cycle;
		*p_list_cursor = platform;
		return;
	case 1:
		update_player_summary(&speed, &count, &ignored, &object);
		update_scroll(&position, &speed, &speed_value, &ignored, &count, &cycle, &ignored, &object);
		update_stage_tile_layers(&ignored, &position, &ignored, &speed_value, &ignored, &count, &cycle, &ignored, &object);
		check_hits(&ignored, &position, &ignored, &speed_value, &ignored, &count, &ignored);
		check_walls(ignored);
		check_platforms(&ignored, &ignored, &ignored, &speed_value, &ignored, &count, &cycle, &platform, &ignored);
		goal_unlocked = 0xFFu;
		check_stage_clear(&count, &platform, &ignored);
		if (game_mode != 0x17u)
		{
			*p_speed_value = speed_value;
			*p_count = count;
			*p_cycle = cycle;
			*p_list_cursor = platform;
			return;
		}
		game_mode = 0xAu;
		scene_step++;
		scene_timer = 0;
		goal_scroll_position = 0xFFFFu;
		*p_speed_value = speed_value;
		*p_count = count;
		*p_cycle = cycle;
		*p_list_cursor = platform;
		return;
	case 2:
		if (stage_05_switch_step < 0)
		{
			find_nearby_floor_blocks(&ignored, &ignored, &speed_value, &ignored, &count, &ignored, &ignored);
			check_hits(&ignored, &position, &ignored, &speed_value, &ignored, &count, &ignored);
			check_walls(ignored);
			check_platforms(&ignored, &ignored, &ignored, &speed_value, &ignored, &count, &cycle, &platform, &ignored);
			scene_timer++;
			if (scene_timer < 0xB4)
			{
				*p_speed_value = speed_value;
				*p_count = count;
				*p_cycle = cycle;
				*p_list_cursor = platform;
				return;
			}
			scene_step++;
			scene_timer = 0;
			stage_init_flags &= ~0x20u;
			*p_speed_value = speed_value;
			*p_count = count;
			*p_cycle = cycle;
			*p_list_cursor = platform;
			return;
		}
		run_stage_05_switch(speed, &position, &speed_value, &object);
		if ((uint8_t)stage_05_switch_step == 2u)
		{
			allocate_object_outside_loop(&free_count, &object);
			if (free_count != 0u)
				OBJECT(object)->handler = stage_00_platform_start_ADDRESS;
		}
		if (stage_05_switch_step >= 0)
		{
			find_nearby_floor_blocks(&ignored, &ignored, &speed_value, &ignored, &count, &ignored, &ignored);
			check_hits(&ignored, &position, &ignored, &speed_value, &ignored, &count, &ignored);
			check_walls(ignored);
			check_platforms(&ignored, &ignored, &ignored, &speed_value, &ignored, &count, &cycle, &platform, &ignored);
			scene_timer++;
			if (scene_timer < 0xB4)
			{
				*p_speed_value = speed_value;
				*p_count = count;
				*p_cycle = cycle;
				*p_list_cursor = platform;
				return;
			}
			scene_step++;
			scene_timer = 0;
			stage_init_flags &= ~0x20u;
			*p_speed_value = speed_value;
			*p_count = count;
			*p_cycle = cycle;
			*p_list_cursor = platform;
			return;
		}
		load_stage_map_pointers(&ignored, &ignored);
		find_nearby_floor_blocks(&ignored, &ignored, &speed_value, &ignored, &count, &ignored, &ignored);
		check_hits(&ignored, &position, &ignored, &speed_value, &ignored, &count, &ignored);
		check_walls(ignored);
		check_platforms(&ignored, &ignored, &ignored, &speed_value, &ignored, &count, &cycle, &platform, &ignored);
		scene_timer++;
		if (scene_timer < 0xB4)
		{
			*p_speed_value = speed_value;
			*p_count = count;
			*p_cycle = cycle;
			*p_list_cursor = platform;
			return;
		}
		scene_step++;
		scene_timer = 0;
		stage_init_flags &= ~0x20u;
		*p_speed_value = speed_value;
		*p_count = count;
		*p_cycle = cycle;
		*p_list_cursor = platform;
		return;
	case 3:
		if ((uint16_t)scene_timer == 0u)
			spawn_eggman_booth(spawned, 0u, speed, &position, &object);
		if ((uint16_t)scene_timer == 0xAu)
		{
			kind = 0x10u;
			frames = (count & ~0xFFFFu) | 0xB4u;
			screen_y = 0x50u;
			stage_05_event_show_message(position, screen_y, kind, object, &speed_value, &frames, &cycle, &platform);
			*p_speed_value = speed_value;
			*p_count = frames;
			*p_cycle = cycle;
			*p_list_cursor = platform;
			return;
		}
		stage_05_event_step_3_frame(position, object, &speed_value, &count, &cycle, &platform);
		*p_speed_value = speed_value;
		*p_count = count;
		*p_cycle = cycle;
		*p_list_cursor = platform;
		return;
	case 4:
		camera_shake_active = 1u;
		camera_shake_length = 0x200u;
		camera_shake_wave = 0x200u;
		scroll_y = update_camera_with_shake();
		update_scroll(&scroll_y, &speed, &speed_value, &ignored, &count, &cycle, &ignored, &object);
		update_stage_tile_layers(&ignored, &scroll_y, &ignored, &speed_value, &ignored, &count, &cycle, &ignored, &object);
		check_hits(&ignored, &scroll_y, &ignored, &speed_value, &ignored, &count, &ignored);
		check_walls(ignored);
		check_platforms(&ignored, &ignored, &ignored, &speed_value, &ignored, &count, &cycle, &platform, &ignored);
		if ((uint16_t)scene_timer == 1u || (uint16_t)scene_timer == 0x3Bu)
		{
			sound_id = 0xFu;
			play_sound(sound_id, &ignored, &ignored);
		}
		scene_timer++;
		if (scene_timer < 0x3C)
		{
			*p_speed_value = speed_value;
			*p_count = count;
			*p_cycle = cycle;
			*p_list_cursor = platform;
			return;
		}
		scene_step++;
		*p_speed_value = speed_value;
		*p_count = count;
		*p_cycle = cycle;
		*p_list_cursor = platform;
		return;
	case 5:
		allocate_object_outside_loop(&free_count, &object);
		if (free_count != 0u)
		{
			OBJECT(object)->flags = 0u;
			OBJECT(object)->handler = stage_00_geyser_rise_ADDRESS;
			stage_00_geyser = object;
		}
		object_2 = LINKED(WORK_RAM, stage_00_platform);
		screen_x = (uint16_t)OBJECT(object_2)->screen_x;
		r14 = (r14 & ~0xFFFFu) | (uint16_t)OBJECT(object_2)->screen_y;
		rocks_left = stage_00_rock_count;
		piece_record = stage_00_rock_pieces_ADDRESS;
		do
		{
			allocate_object_outside_loop(&free_count, &object);
			if (free_count == 0u)
			{
				scene_timer = 0;
				sound_id = 6u;
				play_sound(sound_id, &ignored, &ignored);
				scene_step++;
				*p_count = rocks_left;
				*p_list_cursor = object_2;
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
			OBJECT(object)->animation = rock_debris_animations_ADDRESS + ROM16(piece_record);
			OBJECT(object)->sprite_palette = 0x8240u;
			OBJECT(object)->projected_x = (uint32_t)OBJECT(object)->screen_x << 4;
			OBJECT(object)->projected_y_fraction = (uint32_t)OBJECT(object)->screen_y << 4;
			OBJECT(object)->animation_frame = 0u;
			OBJECT(object)->animation_speed = 4u;
			OBJECT(object)->animation_timer = 0;
			OBJECT(object)->flags = 0u;
			OBJECT(object)->sprite_scale = 0x100u;
			OBJECT(object)->handler = rock_debris_fly_ADDRESS;
			rocks_left--;
			piece_record += 2u;
		} while (rocks_left != 0u);
		scene_timer = 0;
		sound_id = 6u;
		play_sound(sound_id, &ignored, &ignored);
		scene_step++;
		*p_count = rocks_left;
		*p_list_cursor = object_2;
		return;
	case 6:
		scene_timer++;
		if (scene_timer >= 0x1E)
			update_stage_00_exit_scene(object, &speed_value, &list_cursor);
		if ((uint16_t)scene_timer == 5u)
			sound_id = 6u;
		else
		{
			if ((uint16_t)scene_timer != 0x30u)
			{
				if (scene_timer < 0x186)
				{
					*p_speed_value = speed_value;
					*p_list_cursor = list_cursor;
					return;
				}
				scene_step++;
				scene_timer = 0;
				*p_speed_value = speed_value;
				*p_list_cursor = list_cursor;
				return;
			}
			sound_id = 6u;
		}
		play_sound(sound_id, &ignored, &ignored);
		if (scene_timer < 0x186)
		{
			*p_speed_value = speed_value;
			*p_list_cursor = list_cursor;
			return;
		}
		scene_step++;
		scene_timer = 0;
		*p_speed_value = speed_value;
		*p_list_cursor = list_cursor;
		return;
	case 7:
		update_stage_00_exit_scene(object, &speed_value, &list_cursor);
		scene_timer++;
		if (scene_timer < 0x12C)
		{
			*p_speed_value = speed_value;
			*p_list_cursor = list_cursor;
			return;
		}
		game_mode = 2u;
		*p_speed_value = speed_value;
		*p_list_cursor = list_cursor;
		return;
	default:
		invalid_jump(0x9493Cu + scene_step * 3u);
	}
}

void stage_05_event_show_message_11(uint32_t part_size, uint32_t box)
{
	uint16_t kind = 0, screen_y = 0;
	uint32_t frames = 0, ignored = 0;

	kind = 0x11u;
	frames = 0x78u;
	screen_y = 0x40u;
	stage_05_event_show_message(part_size, screen_y, kind, box, &ignored, &frames, &ignored, &ignored);
}

void stage_05_event_show_message_12(uint32_t part_size, uint32_t box)
{
	uint16_t kind = 0, screen_y = 0;
	uint32_t frames = 0, ignored = 0;

	kind = 0x12u;
	frames = 0x1Eu;
	screen_y = 0x40u;
	stage_05_event_show_message(part_size, screen_y, kind, box, &ignored, &frames, &ignored, &ignored);
}

void stage_05_event_show_message(uint32_t part_size, uint16_t screen_y, uint16_t kind, uint32_t box, uint32_t *p_sag, uint32_t *p_frames, uint32_t *p_z, uint32_t *p_platform)
{
	uint32_t sag = *p_sag;
	uint32_t frames = *p_frames;
	uint32_t z = *p_z;
	uint8_t sound_id = 0;
	uint16_t screen_x = 0;
	uint32_t ignored = 0, platform = 0;

	screen_x = 0xA0u;
	box = spawn_message_box(screen_x, screen_y, kind, (uint16_t)frames, box);
	sound_id = 0x70u;
	play_sound(sound_id, &ignored, &part_size);
	stage_05_event_step_3_frame(part_size, box, &sag, &frames, &z, &platform);
	*p_sag = (*p_sag & 0xFFFF0000u) | (sag & 0xFFFFu);
	*p_frames = frames;
	*p_z = z;
	*p_platform = platform;
}

void stage_05_event_step_3_frame(uint32_t part_size, uint32_t wall, uint32_t *p_sag, uint32_t *p_point_y, uint32_t *p_z, uint32_t *p_platform)
{
	uint32_t sag = *p_sag;
	uint32_t point_y = *p_point_y;
	uint32_t z = *p_z;
	uint32_t ignored = 0, platform = 0;

	spawn_walls(&ignored, &part_size, &ignored, &point_y, &z, &wall);
	check_hits(&ignored, &part_size, &ignored, &sag, &ignored, &point_y, &ignored);
	check_walls(ignored);
	check_platforms(&ignored, &ignored, &ignored, &sag, &ignored, &point_y, &z, &platform, &ignored);
	scene_timer++;
	if (scene_timer < 0xA0)
	{
		*p_sag = (*p_sag & 0xFFFF0000u) | (sag & 0xFFFFu);
		*p_point_y = point_y;
		*p_z = z;
		*p_platform = platform;
		return;
	}
	scene_step++;
	scene_timer = 0;
	*p_sag = (*p_sag & 0xFFFF0000u) | (sag & 0xFFFFu);
	*p_point_y = point_y;
	*p_z = z;
	*p_platform = platform;
}

void stage_00_platform_start(void)
{
	uint32_t ignored = 0;

	if ((uint8_t)stage_05_switch_step == 2u)
		return;
	OBJECT(current_object)->sprite_flags = 0x1000u;
	OBJECT(current_object)->sprite_frame = 0x6FF8u;
	OBJECT(current_object)->sprite_palette = 0x9040u;
	OBJECT(current_object)->sprite_offset_x = 0;
	OBJECT(current_object)->sprite_offset_y = 0;
	OBJECT(current_object)->x_int = 0x185;
	OBJECT(current_object)->x_fraction = 0u;
	OBJECT(current_object)->y_int = 0x1545;
	OBJECT(current_object)->y_fraction = 0u;
	OBJECT(current_object)->z = 0;
	project_object(&ignored, &ignored);
	OBJECT(current_object)->handler = stage_00_platform_stand_ADDRESS;
	stage_00_platform = current_object;
	stage_00_platform_stand();
}

void stage_00_platform_stand(void)
{
	uint16_t screen_y = 0;
	uint32_t bucket = 0, ignored = 0;

	if ((int8_t)(game_mode - 0xAu) < 0 || (int8_t)scene_step < 5)
	{
		update_screen_position(&bucket, &ignored);
		screen_y = (uint16_t)OBJECT(current_object)->screen_y;
		bucket = depth_to_bucket(bucket, screen_y);
		bucket = (uint16_t)(bucket + 0x3Cu);
		if ((int16_t)bucket > 0xFF)
			bucket = 0xFFu;
		queue_sprite_in_bucket(bucket);
		return;
	}
	if (scene_step != 5u)
	{
		stage_00_platform_break();
		return;
	}
	OBJECT(current_object)->sprite_palette = 0x9030u;
	OBJECT(current_object)->animation_frame = 0u;
	OBJECT(current_object)->animation_speed = 2u;
	OBJECT(current_object)->animation_timer = 0;
	OBJECT(current_object)->animation = stage_00_platform_break_animation_ADDRESS;
	OBJECT(current_object)->projected_x += 4u;
	OBJECT(current_object)->projected_y_int -= 0x13u;
	OBJECT(current_object)->handler = stage_00_platform_break_ADDRESS;
	stage_00_platform_break();
}

void stage_00_platform_break(void)
{
	uint16_t screen_y = 0;
	uint32_t bucket = 0, ignored = 0;

	animate(&ignored, &ignored, &ignored);
	update_screen_position(&bucket, &ignored);
	if (OBJECT(current_object)->screen_y < 0x130)
	{
		screen_y = (uint16_t)OBJECT(current_object)->screen_y;
		bucket = depth_to_bucket(bucket, screen_y);
		bucket = (uint16_t)(bucket - 0x3Cu);
		queue_sprite_in_bucket(bucket);
		return;
	}
	delete_current_object();
}

void stage_00_geyser_rise(void)
{
	uint32_t cursor = 0, free_count = 0, ignored = 0, object = 0, object_2 = 0;
	uint16_t speed_y_fraction = 0, speed_z_int = 0, speed_x_int = 0;
	uint32_t flags = 0;

	flags = OBJECT(current_object)->flags;
	OBJECT(current_object)->flags = flags | 4u;
	if ((flags & 4u) == 0u)
	{
		OBJECT(current_object)->sprite_flags = 0x1000u;
		OBJECT(current_object)->sprite_frame = 0x6F38u;
		OBJECT(current_object)->sprite_palette = 0x1010u;
		OBJECT(current_object)->sprite_offset_x = 0;
		OBJECT(current_object)->sprite_offset_y = 0;
		object = LINKED(current_object, stage_00_platform);
		OBJECT(current_object)->projected_x = OBJECT(object)->projected_x;
		OBJECT(current_object)->projected_y = OBJECT(object)->projected_y;
		OBJECT(current_object)->z = OBJECT(object)->z;
		OBJECT(current_object)->z_int -= 8u;
		OBJECT(current_object)->animation_frame = 0u;
		OBJECT(current_object)->animation_speed = 2u;
		OBJECT(current_object)->animation_timer = 2;
		OBJECT(current_object)->animation = stage_00_geyser_animation_ADDRESS;
		OBJECT(current_object)->speed_x_int = 0x2B;
		OBJECT(current_object)->speed_x_fraction = 8u;
		OBJECT(current_object)->floor_z = 0x9569C;
		OBJECT(current_object)->speed_y_fraction = 0u;
		OBJECT(current_object)->speed_z = 0;
	}
	speed_y_fraction = OBJECT(current_object)->speed_y_fraction;
	OBJECT(current_object)->speed_y_fraction = speed_y_fraction - 1u;
	if ((int16_t)speed_y_fraction <= 1)
	{
		cursor = OBJECT(current_object)->floor_z;
		OBJECT(current_object)->speed_y_fraction = GEYSER_RISE_STEP(cursor)->frames;
		cursor += 2u;
		OBJECT(current_object)->speed_z = (uint32_t)GEYSER_RISE_STEP(cursor - 0x2u)->rise_speed << 12;
		OBJECT(current_object)->floor_z = cursor + 2u;
	}
	OBJECT(current_object)->z -= OBJECT(current_object)->speed_z;
	if (OBJECT(current_object)->speed_x_fraction != 0u)
	{
		speed_z_int = OBJECT(current_object)->speed_z_int;
		speed_x_int = OBJECT(current_object)->speed_x_int;
		OBJECT(current_object)->speed_x_int = speed_x_int - speed_z_int;
		if ((int16_t)speed_x_int <= (int16_t)speed_z_int)
		{
			OBJECT(current_object)->speed_x_fraction--;
			OBJECT(current_object)->speed_x_int = 0x1C;
			allocate_object(&free_count, &object_2);
			if (free_count != 0u)
			{
				OBJECT(object_2)->handler = stage_00_geyser_column_start_ADDRESS;
				OBJECT(object_2)->parent = current_object;
			}
		}
	}
	update_screen_position(&ignored, &ignored);
	if (OBJECT(current_object)->screen_y < 0x130)
	{
		animate(&ignored, &ignored, &ignored);
		ignored = queue_sprite(ignored);
		return;
	}
	delete_current_object();
}

void stage_00_geyser_column_start(void)
{
	uint32_t object = 0;

	OBJECT(current_object)->sprite_flags = 0x1000u;
	OBJECT(current_object)->sprite_palette = 0x1010u;
	OBJECT(current_object)->sprite_offset_x = 0;
	OBJECT(current_object)->sprite_offset_y = 0;
	object = LINKED(WORK_RAM, stage_00_platform);
	OBJECT(current_object)->projected_x = OBJECT(object)->projected_x;
	OBJECT(current_object)->projected_y = OBJECT(object)->projected_y;
	OBJECT(current_object)->z = OBJECT(object)->z;
	OBJECT(current_object)->z_int += 8u;
	OBJECT(current_object)->handler = stage_00_geyser_column_rise_ADDRESS;
	stage_00_geyser_column_rise();
}

void stage_00_geyser_column_rise(void)
{
	uint32_t ignored = 0, object = 0;

	object = LINKED(current_object, OBJECT(current_object)->parent);
	OBJECT(current_object)->z -= OBJECT(object)->speed_z;
	OBJECT(current_object)->sprite_frame = stage_00_geyser_column_sprites[OBJECT(object)->animation_frame];
	update_screen_position(&ignored, &ignored);
	if (OBJECT(current_object)->screen_y < 0x130)
	{
		ignored = queue_sprite(ignored);
		return;
	}
	delete_current_object();
}

void rock_debris_fly(void)
{
	uint8_t bucket = 0;
	uint32_t ignored = 0;

	OBJECT(current_object)->projected_x += OBJECT(current_object)->speed_x_fraction;
	OBJECT(current_object)->screen_x = (int16_t)OBJECT(current_object)->projected_x >> 4;
	OBJECT(current_object)->projected_y_fraction += OBJECT(current_object)->speed_y_fraction;
	OBJECT(current_object)->screen_y = (int16_t)OBJECT(current_object)->projected_y_fraction >> 4;
	if ((uint16_t)(OBJECT(current_object)->screen_x + 0x20u) >= 0x180u || (uint16_t)(OBJECT(current_object)->screen_y + 0x20u) >= 0x120u)
	{
		delete_current_object();
		return;
	}
	if ((OBJECT(current_object)->sprite_flags & ~0xFFFu) == 0u)
	{
		OBJECT(current_object)->sprite_scale += 0x30u;
		if ((int16_t)OBJECT(current_object)->sprite_scale >= 0x600)
		{
			delete_current_object();
			return;
		}
	}
	animate(&ignored, &ignored, &ignored);
	bucket = 1u;
	queue_sprite_in_bucket(bucket);
}

void event_1c_follow(void)
{
	follow_base_speed = 5u;
	scroll_mode_low = 7u;
	if (no_player_in_play != 0u)
		scroll_mode_low = 1u;
	if (scroll_position >= 0x7E9Fu)
	{
		scene_step++;
		scene_timer = 0x1E;
		event_1c_ride_phase = 0xFFu;
	}
}

void event_1c_stop(void)
{
	uint16_t time_left = 0;

	follow_base_speed = 0u;
	scroll_mode_low = 1u;
	time_left = scene_timer;
	scene_timer = time_left - 1u;
	if ((int16_t)time_left <= 1)
	{
		scene_step++;
		event_1c_riding = 0xFFu;
	}
}

void event_1c_scroll(void)
{
	scroll_mode_low = 0u;
	scroll_speed_low = 0xF;
}

void update_ending(uint32_t position, uint32_t speed, uint32_t object, uint32_t *p_speed_value, uint32_t *p_count, uint32_t *p_cycle, uint32_t *p_platform)
{
	uint32_t speed_value = *p_speed_value;
	uint32_t count = *p_count;
	uint32_t cycle = *p_cycle;
	uint32_t ignored = 0, platform = 0;

	if (scene_step == 0u)
		scene_step = 0xFFu;
	next_stage_slot_request = 0u;
	joining_closed = 0xFFu;
	build_ending_scene(&position, &speed, &speed_value, &count, &cycle, &object);
	find_nearby_floor_blocks(&ignored, &speed, &speed_value, &ignored, &count, &ignored, &object);
	speed_value = update_restart_point(speed_value);
	spawn_objects_by_type(&ignored, &position, &speed, &speed_value, &ignored, &count, &object);
	spawn_ring_rows(&ignored, &position, &ignored, &object);
	spawn_walls(&ignored, &position, &ignored, &count, &cycle, &object);
	spawn_object_groups_2(&ignored, &count, &object);
	spawn_object_groups(&ignored, &position, &ignored, &count, &object);
	spawn_objects_by_handler(&ignored, &ignored, &object);
	ignored = check_zones(ignored);
	check_hits(&ignored, &position, &ignored, &speed_value, &ignored, &count, &ignored);
	check_walls(ignored);
	check_platforms(&ignored, &ignored, &ignored, &speed_value, &ignored, &count, &cycle, &platform, &ignored);
	if ((player_status[0] | player_2_status | player_3_status | player_status_spare) != 0u)
	{
		*p_speed_value = speed_value;
		*p_count = count;
		*p_cycle = cycle;
		*p_platform = platform;
		return;
	}
	saved_game_mode = game_mode;
	game_mode = 7u;
	game_mode_timer = 0;
	*p_speed_value = speed_value;
	*p_count = count;
	*p_cycle = cycle;
	*p_platform = platform;
}

void spawn_stage_06_lightning(uint32_t *p_record, uint32_t *p_bolt)
{
	uint32_t bolt = *p_bolt;
	uint8_t sound_id = 0;
	uint32_t free_count = 0, ignored = 0, record = 0;
	uint16_t strike_time = 0;
	uint32_t init_flags = 0;

	init_flags = stage_init_flags;
	stage_init_flags = init_flags | 0x1000u;
	if ((init_flags & 0x1000u) == 0u)
		stage_06_lightning_cursor = stage_06_lightning_strikes_ADDRESS;
	record = stage_06_lightning_cursor;
	strike_time = ROM16(record);
	record += 2u;
	if (scene_timer < (int16_t)strike_time)
	{
		*p_record = record;
		return;
	}
	allocate_object_outside_loop(&free_count, &bolt);
	if (free_count == 0u)
	{
		*p_record = record;
		*p_bolt = bolt;
		return;
	}
	OBJECT(bolt)->sprite_flags = 0x1008u;
	OBJECT(bolt)->sprite_palette = 0x9110u;
	OBJECT(bolt)->flags = 0u;
	OBJECT(bolt)->handler = stage_06_lightning_animate_ADDRESS;
	OBJECT(bolt)->screen_x = ROM16(record);
	record += 2u;
	OBJECT(bolt)->screen_y = ROM16(record);
	record += 2u;
	OBJECT(bolt)->animation = ROM32(record);
	OBJECT(bolt)->animation_frame = 0u;
	OBJECT(bolt)->animation_timer = 0;
	OBJECT(bolt)->animation_speed = 4u;
	record += 4u;
	stage_06_lightning_cursor = record;
	sound_id = 0xBu;
	play_sound(sound_id, &ignored, &ignored);
	*p_record = record;
	*p_bolt = bolt;
}

void stage_06_lightning_animate(void)
{
	uint32_t at_last_step = 0, ignored = 0;

	if (current_game_mode == 0x10u)
	{
		animate(&ignored, &ignored, &at_last_step);
		if ((uint8_t)at_last_step == 0u)
		{
			ignored = queue_sprite(ignored);
			return;
		}
	}
	delete_current_object();
}

uint32_t spawn_message_box(uint16_t screen_x, uint16_t screen_y, uint16_t kind, uint16_t frames, uint32_t box)
{
	uint32_t free_count = 0;

	allocate_object_outside_loop(&free_count, &box);
	if (free_count == 0u)
		return box;
	place_message_box(screen_x, screen_y, kind, frames, box);
	return box;
}

void spawn_message_box_2(uint16_t screen_x, uint16_t screen_y, uint16_t kind, uint16_t frames, uint32_t box)
{
	uint32_t free_count = 0;

	allocate_object(&free_count, &box);
	if (free_count == 0u)
		return;
	place_message_box(screen_x, screen_y, kind, frames, box);
}

void place_message_box(uint16_t screen_x, uint16_t screen_y, uint32_t kind, uint16_t frames, uint32_t box)
{
	OBJECT(box)->screen_x = screen_x;
	OBJECT(box)->screen_y = screen_y;
	OBJECT(box)->speed_x_fraction = frames;
	kind = (uint16_t)kind * 8u + message_box_kinds_ADDRESS;
	OBJECT(box)->sprite_frame = ROM16(kind);
	kind += 2u;
	OBJECT(box)->sprite_palette = ROM16(kind);
	kind += 2u;
	OBJECT(box)->speed_x_int = ROM16(kind);
	OBJECT(box)->sprite_height = ROM16(kind + 2u);
	OBJECT(box)->sprite_flags = 0x9000u;
	OBJECT(box)->flags = 0u;
	OBJECT(box)->sprite_width = 0;
	OBJECT(box)->handler = message_box_show_ADDRESS;
	OBJECT(box)->sprite_scale = mem_read16(((sprite_frames & ~0xFFFFu) | OBJECT(box)->sprite_frame) + 6u);
}

void message_box_show(void)
{
	uint8_t bucket = 0;
	uint16_t speed_x_fraction = 0, sprite_width = 0;

	speed_x_fraction = OBJECT(current_object)->speed_x_fraction;
	OBJECT(current_object)->speed_x_fraction = speed_x_fraction - 1u;
	if ((int16_t)speed_x_fraction > 1)
	{
		OBJECT(current_object)->sprite_width += 0x18u;
		if (OBJECT(current_object)->speed_x_int < OBJECT(current_object)->sprite_width)
			OBJECT(current_object)->sprite_width = OBJECT(current_object)->speed_x_int;
		bucket = 0u;
		queue_sprite_in_bucket(bucket);
		return;
	}
	OBJECT(current_object)->speed_x_fraction = 0u;
	sprite_width = OBJECT(current_object)->sprite_width;
	OBJECT(current_object)->sprite_width = sprite_width - 0x18u;
	if ((int16_t)sprite_width > 0x18)
	{
		bucket = 0u;
		queue_sprite_in_bucket(bucket);
		return;
	}
	delete_current_object();
}

void start_escape_countdown(uint32_t *p_free_stack, uint32_t *p_count, uint32_t *p_digit)
{
	uint32_t digit = *p_digit;
	uint32_t count = 0, digit_slot = 0, free_stack = 0, ignored = 0;

	digit_slot = escape_tens_digit_ADDRESS;
	count = 2u;
	do
	{
		allocate_object_2(&ignored, &free_stack, &digit);
		OBJECT(digit)->sprite_flags = 0u;
		OBJECT(digit)->sprite_palette = 0xF60u;
		OBJECT(digit)->projected_x = 0xA0;
		OBJECT(digit)->screen_y = 0x80;
		OBJECT(digit)->sprite_scale = 1u;
		mem_write16(digit_slot, digit);
		count--;
		digit_slot += 2u;
	} while (count != 0u);
	escape_countdown_frames = 0;
	escape_seconds = 0x14u;
	escape_started = 0u;
	escape_countdown_running = 0u;
	*p_free_stack = free_stack;
	*p_count = count;
	*p_digit = digit;
}

void update_escape_countdown(void)
{
	uint32_t ignored = 0, seconds = 0, seconds_bcd = 0, tens_digit = 0;
	uint16_t tens = 0;

	if (escape_started == 0u)
		return;
	if (escape_countdown_running != 0u || escape_countdown_frames < 0x3B)
	{
		escape_countdown_frames++;
		if (escape_countdown_frames >= 0x3C)
		{
			escape_countdown_frames = 0;
			escape_seconds--;
		}
	}
	seconds = escape_seconds;
	convert_to_bcd(&seconds, &ignored);
	escape_seconds_bcd = seconds;
	escape_digit_scale = (uint8_t)escape_countdown_frames * 9u;
	current_object = LINKED(WORK_RAM, escape_tens_digit);
	OBJECT(current_object)->sprite_scale = escape_digit_scale;
	OBJECT(current_object)->screen_x = OBJECT(current_object)->projected_x - ((uint16_t)(OBJECT(current_object)->sprite_scale * 0xCu) >> 8);
	seconds_bcd = escape_seconds_bcd;
	tens = seconds_bcd >> 4;
	tens_digit = tens;
	if (tens != 0u)
	{
		OBJECT(current_object)->sprite_frame = escape_countdown_digits[tens_digit];
		queue_sprite_on_top();
	}
	current_object = LINKED(WORK_RAM, escape_units_digit);
	OBJECT(current_object)->sprite_scale = escape_digit_scale;
	if ((escape_seconds_bcd & 0xF0u) == 0u)
		OBJECT(current_object)->screen_x = OBJECT(current_object)->projected_x;
	else
		OBJECT(current_object)->screen_x = ((uint16_t)(OBJECT(current_object)->sprite_scale * 0xCu) >> 8) + OBJECT(current_object)->projected_x;
	seconds_bcd = escape_seconds_bcd;
	OBJECT(current_object)->sprite_frame = escape_countdown_digits[seconds_bcd & 0xFu];
	queue_sprite_on_top();
}

void start_escape_alarm_flash(uint32_t flash)
{
	uint32_t free_count = 0;

	allocate_object_outside_loop(&free_count, &flash);
	if (free_count != 0u)
	{
		OBJECT(flash)->escape_alarm_flash.started = 0u;
		OBJECT(flash)->handler = escape_alarm_flash_ADDRESS;
	}
}
