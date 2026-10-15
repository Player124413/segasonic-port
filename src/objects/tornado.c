/**
 *    Name:        objects/tornado.c
 *    Description: stage A: the tornado, its twisters, its hazard and rear zones, the whirlwinds it throws
 *    Version:     0.1.0
 *    Created:     12-10-2026
 *    Modified:    15-10-2026
 *    Author:      Sebastian Toyos
 *
 *    SegaSonic The Hedgehog (arcade, Sega System 32): the game's V60 program decompiled into C by
 *    tools/decompile.py, with the names of symbols.txt.
 */
#include "game.h"

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
	uint32_t r1 = 0, r7 = 0, r8 = 0, depth = 0, ignored = 0, object = 0, object_2 = 0, orbit_dx = 0, orbit_dy = 0, radius_limit = 0, radius_rate = 0, x = 0;

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
	find_floor(&x, &r1, &radius_limit, &ignored, &object_2);
	OBJECT(current_object)->z = OBJECT(current_object)->floor_z;
	project_object(&ignored, &depth);
	queue_sprite(depth);
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

uint32_t tornado_throw_whirlwinds(uint8_t spawned, uint32_t free_count, uint32_t whirlwind, uint32_t object)
{
	uint32_t saved_current_object = 0;

	if (mem_read16(OBJECT(current_object)->part_list) <= scroll_position)
	{
		saved_current_object = current_object;
		object = spawn_whirlwind(spawned, free_count, whirlwind, object);
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
