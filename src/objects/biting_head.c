/**
 *    Name:        objects/biting_head.c
 *    Description: the chasers of stages B, C and E: the brick wall that bursts when the chaser stops, the biting head, the fire front
 *    Version:     0.1.0
 *    Created:     12-10-2026
 *    Modified:    15-10-2026
 *    Author:      Sebastian Toyos
 *
 *    SegaSonic The Hedgehog (arcade, Sega System 32): the game's V60 program decompiled into C by
 *    tools/decompile.py, with the names of symbols.txt.
 */
#include "game.h"

void brick_wall_init(void)
{
	uint32_t ignored = 0;

	OBJECT(current_object)->sprite_flags = 0x1002u;
	OBJECT(current_object)->sprite_frame = 0x7D90u;
	OBJECT(current_object)->sprite_palette = 0x10D0u;
	OBJECT(current_object)->sprite_offset_x = 0;
	OBJECT(current_object)->sprite_offset_y = 8;
	OBJECT(current_object)->flags = 0u;
	OBJECT(current_object)->floor_z = 0x1D80000;
	OBJECT(current_object)->z = OBJECT(current_object)->floor_z;
	OBJECT(current_object)->handler = brick_wall_wait_ADDRESS;
	project_object(&ignored, &ignored);
}

void brick_wall_wait(void)
{
	uint16_t screen_x = 0;
	uint32_t r1 = 0, brick_animation = 0, depth = 0, free_count = 0, ignored = 0, object = 0, piece_record = 0, pieces_left = 0;

	if (chaser_stopped == 0u)
	{
		update_screen_position(&ignored, &r1);
		depth = (uint16_t)(OBJECT(current_object)->screen_y - 0x10u);
		queue_sprite_at_depth(&ignored, &depth);
		return;
	}
	screen_x = (uint16_t)OBJECT(current_object)->screen_x;
	r14 = (uint16_t)OBJECT(current_object)->screen_y;
	pieces_left = brick_wall_bricks;
	piece_record = brick_wall_brick_records_ADDRESS;
	do
	{
		allocate_object(&free_count, &object);
		if (free_count == 0u)
		{
			delete_current_object();
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
		brick_animation = brick_animations_ADDRESS + ROM16(piece_record);
		OBJECT(object)->sprite_palette = ROM16(brick_animation);
		OBJECT(object)->animation = brick_animation + 2u;
		OBJECT(object)->projected_x = (uint32_t)OBJECT(object)->screen_x << 4;
		OBJECT(object)->projected_y_fraction = (uint32_t)OBJECT(object)->screen_y << 4;
		OBJECT(object)->animation_frame = 0u;
		OBJECT(object)->animation_speed = 4u;
		OBJECT(object)->animation_timer = 0;
		OBJECT(object)->flags = 0u;
		OBJECT(object)->sprite_scale = 0x100u;
		OBJECT(object)->handler = rock_debris_fly_ADDRESS;
		piece_record += 2u;
		pieces_left--;
	} while (pieces_left != 0u);
	screen_x = (uint16_t)OBJECT(current_object)->screen_x;
	r14 = (uint16_t)OBJECT(current_object)->screen_y;
	pieces_left = brick_wall_dust;
	piece_record = brick_wall_dust_records_ADDRESS;
	do
	{
		allocate_object(&free_count, &object);
		if (free_count == 0u)
			break;
		OBJECT(object)->sprite_flags = ROM16(piece_record);
		OBJECT(object)->screen_x = screen_x;
		piece_record += 2u;
		OBJECT(object)->screen_x += ROM16(piece_record);
		OBJECT(object)->screen_y = r14;
		piece_record += 2u;
		OBJECT(object)->screen_y += ROM16(piece_record);
		piece_record += 2u;
		AS16(OBJECT(object)->var_80) = ROM16(piece_record);
		OBJECT(object)->animation = brick_dust_animation_ADDRESS;
		OBJECT(object)->sprite_palette = 0x10E0u;
		OBJECT(object)->animation_frame = 0u;
		OBJECT(object)->animation_speed = 4u;
		OBJECT(object)->animation_timer = 0;
		OBJECT(object)->flags = 0u;
		OBJECT(object)->sprite_scale = 0x100u;
		OBJECT(object)->handler = brick_dust_animate_ADDRESS;
		piece_record += 2u;
		pieces_left--;
	} while (pieces_left != 0u);
	delete_current_object();
}

void brick_dust_animate(void)
{
	uint32_t r1 = 0, r7 = 0, at_last_step = 0;
	uint16_t wait_timer = 0;

	wait_timer = OBJECT(current_object)->brick_dust.wait_timer;
	OBJECT(current_object)->brick_dust.wait_timer = wait_timer - 1u;
	if ((int16_t)wait_timer > 1)
		return;
	OBJECT(current_object)->brick_dust.wait_timer = 0;
	animate(&r1, &r7, &at_last_step);
	if ((int8_t)at_last_step >= 0)
	{
		r1 = queue_sprite(r1);
		return;
	}
	delete_current_object();
}

void biting_head_init(void)
{
	uint32_t free_count = 0, head_record = 0, taken_object = 0;

	head_record = biting_head_paths_ADDRESS + biting_head_paths[OBJECT(current_object)->sprite_frame];
	OBJECT(current_object)->x_int = ROM16(head_record);
	OBJECT(current_object)->x_fraction = 0u;
	head_record += 2u;
	OBJECT(current_object)->y_int = ROM16(head_record);
	OBJECT(current_object)->y_fraction = 0u;
	OBJECT(current_object)->biting_head_view.path = head_record + 2u;
	OBJECT(current_object)->sprite_flags = 0xD004u;
	OBJECT(current_object)->flags = 0x40000000u;
	OBJECT(current_object)->biting_head_view.holding_back = 0u;
	OBJECT(current_object)->kind = 9u;
	OBJECT(current_object)->direction = 0x300u;
	OBJECT(current_object)->biting_head_view.wait_timer = 1;
	OBJECT(current_object)->hitbox_height = 0xF0u;
	OBJECT(current_object)->hitbox_size_x = 0x80u;
	OBJECT(current_object)->hitbox_size_y = 0x40u;
	OBJECT(current_object)->hitbox_offset_x = 0x20;
	OBJECT(current_object)->hitbox_offset_y = -0x20;
	OBJECT(current_object)->biting_head_view.bounce_index = -1;
	OBJECT(current_object)->floor_z = 0x1D80000;
	OBJECT(current_object)->z = 0x1D80000;
	AS8(OBJECT(current_object)->spawn_arg_2) = 0u;
	biting_head = current_object;
	biting_head_done = 0u;
	OBJECT(current_object)->animation_frame = 0u;
	OBJECT(current_object)->animation_timer = 0;
	OBJECT(current_object)->biting_head_view.bite_step = 0u;
	OBJECT(current_object)->biting_head_view.bite_timer = 0;
	take_free_object(&free_count, &taken_object);
	if (free_count != 0u)
	{
		move_halfwords_backward(taken_object, biting_head_parts_ADDRESS, 0x19u);
		OBJECT(current_object)->part_list = taken_object;
		move_halfwords_backward(taken_object + 0x40u, biting_head_clip_entry_ADDRESS, 0xAu);
		move_halfwords_backward(taken_object + 0x54u, biting_head_clip_entry_ADDRESS, 0xAu);
		take_free_object(&free_count, &taken_object);
		if (free_count != 0u)
		{
			mem_write16(taken_object + 0x10u, 0x200u);
			mem_write16(taken_object + 0x12u, 0x130u);
			mem_write16(taken_object + 0x24u, 0x300u);
			mem_write16(taken_object + 0x66u, 0xF0u);
			mem_write8(taken_object + 0x7Cu, 0u);
			mem_write8(taken_object + 0x7Du, 0x80u);
			mem_write32(taken_object + 0x30u, 0x40000000u);
			mem_write8(taken_object + 0x47u, 9u);
			OBJECT(current_object)->child = taken_object;
			OBJECT(current_object)->handler = biting_head_wait_ADDRESS;
		}
	}
}

void biting_head_wait(void)
{
	if (chaser_stopped == 0u)
	{
		if (scroll_position < 0x1200u)
			return;
		chaser_stopped = 0xFFu;
	}
	OBJECT(current_object)->handler = biting_head_chase_ADDRESS;
}

void biting_head_chase(void)
{
	uint16_t bounce_dy = 0, hold_threshold = 0, move_frame = 0, move_speed = 0, record_coordinate = 0;
	uint32_t bite_record = 0, depth = 0, direction = 0, floor_slope = 0, ignored = 0, move_frames = 0, next_bite_step = 0, next_frame = 0, object = 0, path_cursor = 0, point_x = 0, point_y = 0, slope_factor = 0, z = 0;
	uint8_t hold_timer = 0, animation_timer = 0, bob_phase = 0, bite_timer = 0;
	uint16_t wait_timer = 0, record_scroll = 0;
	uint32_t flags = 0;

	flags = OBJECT(current_object)->flags;
	OBJECT(current_object)->flags = flags | 0x80000000u;
	if ((flags & 0x80000000u) == 0u)
	{
	biting_head_read_script:
		wait_timer = OBJECT(current_object)->biting_head_view.wait_timer;
		OBJECT(current_object)->biting_head_view.wait_timer = wait_timer - 1u;
		if ((int16_t)wait_timer <= 1)
		{
			OBJECT(current_object)->biting_head_view.wait_timer = 0;
			path_cursor = OBJECT(current_object)->biting_head_view.path;
			record_scroll = mem_read16(path_cursor);
			path_cursor += 2u;
			if (record_scroll <= scroll_position)
			{
				path_cursor += 2u;
				goto biting_head_load_record;
			}
			record_coordinate = mem_read16(path_cursor);
			direction = OBJECT(current_object)->direction;
			path_cursor += 2u;
			switch (direction >> 8)
			{
			case 0:
			case 4:
				if (OBJECT(current_object)->y_int <= (int16_t)record_coordinate)
				{
				biting_head_load_record:
					if ((int16_t)mem_read16(path_cursor + 2u) < 0)
					{
						current_object = (current_object & ~0xFFFFu) | OBJECT(current_object)->sprite_frame;
						push_free_object();
						current_object = LINKED(current_object, OBJECT(object_cursor)->child);
						push_free_object();
						delete_current_object();
						return;
					}
					OBJECT(current_object)->biting_head_view.screen_threshold = mem_read16(path_cursor);
					path_cursor += 2u;
					OBJECT(current_object)->biting_head_view.catch_up_factor = mem_read16(path_cursor);
					path_cursor += 2u;
					OBJECT(current_object)->biting_head_view.base_speed = mem_read16(path_cursor);
					path_cursor += 2u;
					OBJECT(current_object)->biting_head_view.wait_timer = mem_read16(path_cursor);
					path_cursor += 2u;
					OBJECT(current_object)->direction = mem_read16(path_cursor);
					OBJECT(current_object)->biting_head_view.path = path_cursor + 2u;
				}
				break;
			case 1:
				if ((int16_t)record_coordinate <= OBJECT(current_object)->x_int)
					goto biting_head_load_record;
				break;
			case 2:
				if ((int16_t)record_coordinate <= OBJECT(current_object)->y_int)
					goto biting_head_load_record;
				break;
			case 3:
				if (OBJECT(current_object)->x_int <= (int16_t)record_coordinate)
					goto biting_head_load_record;
				break;
			default:
				unknown_code(0x8ABC3u + (direction >> 8) * 2u);
			}
		}
	}
	else if (OBJECT(current_object)->biting_head_view.holding_back == 0u)
	{
		if (no_player_in_play == 0u && player_mishap == 0u && player_dropping_in == 0u)
			goto biting_head_read_script;
		OBJECT(current_object)->biting_head_view.holding_back = 0xFFu;
		OBJECT(current_object)->biting_head_view.saved_base_speed = OBJECT(current_object)->biting_head_view.base_speed;
		OBJECT(current_object)->biting_head_view.base_speed = 0;
		OBJECT(current_object)->biting_head_view.hold_start_position = scroll_position;
		OBJECT(current_object)->biting_head_view.hold_timer = 0xFFu;
		OBJECT(current_object)->biting_head_view.saved_threshold = OBJECT(current_object)->biting_head_view.screen_threshold;
		hold_threshold = 0xFFE0u;
		if (OBJECT(current_object)->direction < 0x100 || OBJECT(current_object)->direction > 0x200)
			hold_threshold = 0x160u;
		OBJECT(current_object)->biting_head_view.screen_threshold = hold_threshold;
	}
	project_object(&ignored, &ignored);
	object = LINKED(current_object, OBJECT(current_object)->child);
	OBJECT(object)->x = OBJECT(current_object)->x;
	OBJECT(object)->x_int += 0x40u;
	OBJECT(object)->y = OBJECT(current_object)->y;
	OBJECT(object)->z = OBJECT(current_object)->z;
	current_object = LINKED(current_object, OBJECT(current_object)->child);
	add_to_zone_list(ignored);
	current_object = object_cursor;
	add_to_zone_list(ignored);
	if (OBJECT(current_object)->biting_head_view.holding_back != 0u)
	{
		if ((int16_t)(scroll_position - OBJECT(current_object)->biting_head_view.hold_start_position) >= 0x300)
			goto biting_head_stop_holding_back;
		if (game_mode != 3u)
			OBJECT(current_object)->biting_head_view.hold_timer = 0xFFu;
		hold_timer = OBJECT(current_object)->biting_head_view.hold_timer;
		OBJECT(current_object)->biting_head_view.hold_timer = hold_timer - 1u;
		if (hold_timer == 0u)
			goto biting_head_stop_holding_back;
	}
	if (OBJECT(current_object)->screen_y < 0)
	{
	biting_head_catch_up:
		OBJECT(current_object)->biting_head_view.wait_timer = 0;
		if (OBJECT(current_object)->biting_head_view.holding_back != 0u)
		{
		biting_head_stop_holding_back:
			OBJECT(current_object)->biting_head_view.holding_back = 0u;
			OBJECT(current_object)->biting_head_view.screen_threshold = OBJECT(current_object)->biting_head_view.saved_threshold;
			OBJECT(current_object)->biting_head_view.base_speed = OBJECT(current_object)->biting_head_view.saved_base_speed;
		}
		slope_factor = 0u;
		if ((OBJECT(current_object)->floor_slope | OBJECT(current_object)->floor_type) != 0u)
		{
			floor_slope = OBJECT(current_object)->floor_slope;
			slope_factor = floor_slope_factors[(uint16_t)(floor_slope * 2u)];
		}
		compute_camera_slope_speed(0u, slope_factor);
		move_speed = (uint16_t)((uint32_t)camera_slope_speed_int * OBJECT(current_object)->biting_head_view.catch_up_factor) >> 8;
		if ((int16_t)move_speed <= OBJECT(current_object)->biting_head_view.base_speed)
		{
		biting_head_use_base_speed:
			move_speed = (uint16_t)OBJECT(current_object)->biting_head_view.base_speed;
		}
	}
	else
	{
		direction = OBJECT(current_object)->direction;
		switch (direction >> 8)
		{
		case 0:
		case 3:
			if (OBJECT(current_object)->screen_x <= OBJECT(current_object)->biting_head_view.screen_threshold)
				goto biting_head_use_base_speed;
			goto biting_head_catch_up;
		case 1:
		case 2:
		case 4:
			if (OBJECT(current_object)->biting_head_view.screen_threshold <= OBJECT(current_object)->screen_x)
				goto biting_head_use_base_speed;
			goto biting_head_catch_up;
		default:
			unknown_code(0x8ACAAu + (direction >> 8) * 2u);
		}
	}
	chaser_speed = move_speed;
	if (OBJECT(current_object)->biting_head_view.wait_timer <= 0)
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
			unknown_code(0x8AD42u + (direction >> 8) * 2u);
		}
	}
	if (OBJECT(current_object)->biting_head_view.holding_back == 0u && (uint16_t)chaser_speed != 0u)
	{
		animation_timer = OBJECT(current_object)->animation_timer;
		OBJECT(current_object)->animation_timer = animation_timer - 1u;
		if ((int8_t)animation_timer <= 1)
		{
			OBJECT(current_object)->animation_timer = 2;
			next_frame = OBJECT(current_object)->animation_frame;
			next_frame = (uint8_t)(next_frame + 1u);
			move_frames = biting_head_move_frames_ADDRESS;
			if ((int8_t)biting_head_frames < (int8_t)next_frame)
				next_frame = 0u;
			OBJECT(current_object)->animation_frame = next_frame;
			move_frame = ROM16(move_frames + next_frame * 2u);
			mem_write16(OBJECT(current_object)->part_list + 2u, move_frame);
			mem_write16(OBJECT(current_object)->part_list + 0x22u, move_frame);
		}
		OBJECT(current_object)->biting_head_view.bob_counter++;
		bob_phase = OBJECT(current_object)->biting_head_view.bob_counter & 3u;
		OBJECT(current_object)->biting_head_view.bob_counter = bob_phase;
		if (bob_phase == 0u)
			OBJECT(current_object)->screen_y--;
	}
	bite_timer = OBJECT(current_object)->biting_head_view.bite_timer;
	OBJECT(current_object)->biting_head_view.bite_timer = bite_timer - 1u;
	if ((int8_t)bite_timer <= 1)
	{
		next_bite_step = OBJECT(current_object)->biting_head_view.bite_step;
		next_bite_step = (uint8_t)(next_bite_step + 1u);
		bite_record = biting_head_bite_step_records_ADDRESS;
		if ((int8_t)biting_head_bite_steps < (int8_t)next_bite_step)
			next_bite_step = 0u;
		OBJECT(current_object)->biting_head_view.bite_step = next_bite_step;
		bite_record += next_bite_step * 4u;
		mem_write16(OBJECT(current_object)->part_list + 0x1Au, ROM16(bite_record));
		OBJECT(current_object)->biting_head_view.bite_timer = ROM8(bite_record + 2u);
		if (OBJECT(current_object)->biting_head_view.bite_step == 5u)
			OBJECT(current_object)->biting_head_view.bounce_index = 0;
	}
	if (OBJECT(current_object)->biting_head_view.bounce_index >= 0)
	{
		OBJECT(current_object)->biting_head_view.bounce_index++;
		bounce_dy = (uint16_t)biting_head_bounce[(uint8_t)OBJECT(current_object)->biting_head_view.bounce_index];
		if ((uint8_t)bounce_dy != 0x7Fu)
		{
		biting_head_apply_bounce:
			OBJECT(current_object)->screen_y += bounce_dy;
		}
		else
		{
			OBJECT(current_object)->biting_head_view.bounce_index = -1;
			if ((uint8_t)bounce_dy != 0x7Fu)
				goto biting_head_apply_bounce;
		}
	}
	if (OBJECT(current_object)->x_int <= -0x355)
	{
		OBJECT(LINKED(current_object, OBJECT(current_object)->child))->hitbox_height = 0x50u;
		OBJECT(current_object)->hitbox_height = 0x50u;
	}
	if (OBJECT(current_object)->x_int <= -0x431)
		biting_head_done = 0xFFu;
	if (scroll_position >= 0x1500u)
	{
		depth = (uint16_t)(OBJECT(current_object)->screen_y - 0x50u);
		queue_sprite_at_depth(&ignored, &depth);
		return;
	}
	point_x = 0x8210000u;
	point_y = 0x5EC0000u;
	z = 0x1D80000u;
	project_point(z, &ignored, &point_x, &point_y);
	mem_write16(OBJECT(current_object)->part_list + 0x48u, point_x);
	depth = (uint16_t)(OBJECT(current_object)->screen_y - 0x50u);
	biting_head_depth = depth;
	current_object = OBJECT(current_object)->part_list;
	current_object = current_object + 0x54u;
	queue_sprite_at_depth(&ignored, &depth);
	current_object = object_cursor;
	depth = biting_head_depth;
	queue_sprite_at_depth(&ignored, &depth);
	current_object = OBJECT(current_object)->part_list;
	depth = biting_head_depth;
	current_object = current_object + 0x40u;
	queue_sprite_at_depth(&ignored, &depth);
}

void fire_front_init(void)
{
	uint16_t flame_offset_x = 0;
	uint32_t flames_left = 0, ignored = 0, object = 0, path = 0;

	path = fire_front_paths_ADDRESS + fire_front_paths[OBJECT(current_object)->sprite_frame];
	OBJECT(current_object)->x_int = ROM16(path);
	OBJECT(current_object)->x_fraction = 0u;
	path += 2u;
	OBJECT(current_object)->y_int = ROM16(path);
	OBJECT(current_object)->y_fraction = 0u;
	path += 2u;
	OBJECT(current_object)->floor_z_int = ROM16(path);
	OBJECT(current_object)->floor_z_fraction = 0u;
	OBJECT(current_object)->z = OBJECT(current_object)->floor_z;
	path += 2u;
	OBJECT(current_object)->direction = ROM16(path);
	OBJECT(current_object)->fire_front.path = path + 2u;
	OBJECT(current_object)->flags = 0x40000000u;
	OBJECT(current_object)->fire_front.holding_back = 0u;
	OBJECT(current_object)->sprite_offset_x = 0;
	OBJECT(current_object)->sprite_offset_y = 0;
	OBJECT(current_object)->kind = 0x14u;
	OBJECT(current_object)->fire_front.wait_timer = 1;
	OBJECT(current_object)->hitbox_height = 0xF0u;
	OBJECT(current_object)->handler = fire_front_chase_ADDRESS;
	OBJECT(current_object)->hitbox_size_x = 0x132u;
	OBJECT(current_object)->hitbox_size_y = 0x200u;
	OBJECT(current_object)->hitbox_offset_x = -0x40;
	OBJECT(current_object)->hitbox_offset_y = 0;
	OBJECT(current_object)->speed_x = 0;
	OBJECT(current_object)->speed_y = 0;
	chaser_object = current_object;
	if (free_object_stack < 3)
		return;
	flames_left = 4u;
	flame_offset_x = 0x28u;
	do
	{
		allocate_object(&ignored, &object);
		OBJECT(object)->handler = spreading_flame_start_ADDRESS;
		OBJECT(object)->x = OBJECT(current_object)->x;
		OBJECT(object)->y = OBJECT(current_object)->y;
		OBJECT(object)->x_int += flame_offset_x;
		flame_offset_x += 0x38u;
		flames_left--;
	} while (flames_left != 0u);
}

void fire_front_chase(void)
{
	uint16_t direction = 0, speed = 0, threshold = 0, trigger_coordinate = 0;
	uint32_t cursor = 0, direction_2 = 0, ignored = 0, step = 0;
	uint8_t hold_timer = 0;
	uint16_t wait_timer = 0, trigger_scroll = 0, hold_threshold = 0;
	uint32_t flags = 0;

	flags = OBJECT(current_object)->flags;
	OBJECT(current_object)->flags = flags | 0x80000000u;
	if ((flags & 0x80000000u) == 0u)
	{
	fire_front_read_script:
		wait_timer = OBJECT(current_object)->fire_front.wait_timer;
		OBJECT(current_object)->fire_front.wait_timer = wait_timer - 1u;
		if ((int16_t)wait_timer <= 1)
		{
			OBJECT(current_object)->fire_front.wait_timer = 0;
			cursor = OBJECT(current_object)->fire_front.path;
			trigger_scroll = mem_read16(cursor);
			cursor += 2u;
			if (trigger_scroll <= scroll_position)
			{
				cursor += 2u;
				goto fire_front_load_record;
			}
			trigger_coordinate = mem_read16(cursor);
			cursor += 2u;
			if (trigger_coordinate != 0u)
			{
				direction_2 = OBJECT(current_object)->direction;
				switch (direction_2 >> 8)
				{
				case 0:
				case 4:
					if ((int16_t)(OBJECT(current_object)->y_int + OBJECT(current_object)->hitbox_offset_y) <= (int16_t)trigger_coordinate)
					{
					fire_front_load_record:
						if ((int16_t)mem_read16(cursor + 2u) < 0)
						{
							chaser_object = 0u;
							delete_current_object();
							return;
						}
						OBJECT(current_object)->fire_front.screen_threshold = mem_read16(cursor);
						cursor += 2u;
						OBJECT(current_object)->fire_front.catch_up_factor = mem_read16(cursor);
						cursor += 2u;
						OBJECT(current_object)->fire_front.base_speed = mem_read16(cursor);
						cursor += 2u;
						OBJECT(current_object)->fire_front.wait_timer = mem_read16(cursor);
						cursor += 2u;
						if ((uint16_t)OBJECT(current_object)->fire_front.wait_timer != 0u)
						{
							OBJECT(current_object)->speed_x = 0;
							OBJECT(current_object)->speed_y = 0;
						}
						OBJECT(current_object)->fire_front.path = cursor;
					}
					break;
				case 1:
					if ((int16_t)trigger_coordinate <= OBJECT(current_object)->x_int)
						goto fire_front_load_record;
					break;
				case 2:
					if ((int16_t)trigger_coordinate <= OBJECT(current_object)->y_int)
						goto fire_front_load_record;
					break;
				case 3:
					if (OBJECT(current_object)->x_int <= (int16_t)trigger_coordinate)
						goto fire_front_load_record;
					break;
				default:
					unknown_code(0x8B05Du + (direction_2 >> 8) * 2u);
				}
			}
		}
	}
	else if (OBJECT(current_object)->fire_front.holding_back == 0u)
	{
		if (no_player_in_play == 0u && player_mishap == 0u && player_dropping_in == 0u)
			goto fire_front_read_script;
		OBJECT(current_object)->fire_front.holding_back = 0xFFu;
		OBJECT(current_object)->fire_front.saved_base_speed = OBJECT(current_object)->fire_front.base_speed;
		OBJECT(current_object)->fire_front.base_speed = 0;
		OBJECT(current_object)->speed_x = 0;
		OBJECT(current_object)->speed_y = 0;
		OBJECT(current_object)->fire_front.hold_start_position = scroll_position;
		OBJECT(current_object)->fire_front.hold_timer = 0xFFu;
		OBJECT(current_object)->fire_front.saved_threshold = OBJECT(current_object)->fire_front.screen_threshold;
		direction = OBJECT(current_object)->direction;
		hold_threshold = direction ^ (direction >> 1);
		threshold = hold_threshold;
		if (hold_threshold == 0u)
			threshold = 0x160u;
		OBJECT(current_object)->fire_front.screen_threshold = threshold;
	}
	project_object(&ignored, &ignored);
	add_to_zone_list(ignored);
	if (OBJECT(current_object)->fire_front.holding_back != 0u)
	{
		if ((int16_t)(scroll_position - OBJECT(current_object)->fire_front.hold_start_position) >= 0x300)
			goto fire_front_stop_holding_back;
		if (game_mode != 3u)
			OBJECT(current_object)->fire_front.hold_timer = 0xFFu;
		hold_timer = OBJECT(current_object)->fire_front.hold_timer;
		OBJECT(current_object)->fire_front.hold_timer = hold_timer - 1u;
		if (hold_timer == 0u)
			goto fire_front_stop_holding_back;
	}
	switch (OBJECT(current_object)->direction >> 8)
	{
	case 0:
	case 3:
		if (OBJECT(current_object)->screen_x <= OBJECT(current_object)->fire_front.screen_threshold)
		{
		fire_front_use_base_speed:
			speed = (uint16_t)OBJECT(current_object)->fire_front.base_speed;
		}
		else
		{
		fire_front_catch_up:
			OBJECT(current_object)->fire_front.wait_timer = 0;
			if (OBJECT(current_object)->fire_front.holding_back != 0u)
			{
			fire_front_stop_holding_back:
				OBJECT(current_object)->fire_front.holding_back = 0u;
				OBJECT(current_object)->fire_front.screen_threshold = OBJECT(current_object)->fire_front.saved_threshold;
				OBJECT(current_object)->fire_front.base_speed = OBJECT(current_object)->fire_front.saved_base_speed;
			}
			compute_camera_ground_speed(0u);
			speed = (uint16_t)((uint32_t)camera_ground_speed_int * OBJECT(current_object)->fire_front.catch_up_factor) >> 8;
			if ((int16_t)speed <= OBJECT(current_object)->fire_front.base_speed)
				goto fire_front_use_base_speed;
		}
		break;
	case 1:
	case 2:
	case 4:
		if (OBJECT(current_object)->fire_front.screen_threshold <= OBJECT(current_object)->screen_x)
			goto fire_front_use_base_speed;
		goto fire_front_catch_up;
	default:
		unknown_code(0x8B117u + (OBJECT(current_object)->direction >> 8) * 2u);
	}
	chaser_speed = speed;
	if (OBJECT(current_object)->fire_front.wait_timer <= 0)
	{
		switch (OBJECT(current_object)->direction >> 8)
		{
		case 0:
			step = (uint32_t)chaser_speed << 12;
			OBJECT(current_object)->y -= step;
			OBJECT(current_object)->speed_y = 0u - step;
			OBJECT(current_object)->speed_x = 0;
			break;
		case 1:
			step = (uint32_t)chaser_speed << 12;
			OBJECT(current_object)->x += step;
			OBJECT(current_object)->speed_x = step;
			OBJECT(current_object)->speed_y = 0;
			break;
		case 2:
		case 4:
			step = (uint32_t)chaser_speed << 12;
			OBJECT(current_object)->y += step;
			OBJECT(current_object)->speed_y = step;
			OBJECT(current_object)->speed_x = 0;
			break;
		case 3:
			step = (uint32_t)chaser_speed << 12;
			OBJECT(current_object)->x -= step;
			OBJECT(current_object)->speed_x = 0u - step;
			OBJECT(current_object)->speed_y = 0;
			break;
		default:
			unknown_code(0x8B193u + (OBJECT(current_object)->direction >> 8) * 2u);
		}
	}
	if (mem_read16(OBJECT(current_object)->fire_front.path - 6u) == 0u)
	{
		OBJECT(current_object)->hitbox_size_x = 0xF2u;
		OBJECT(current_object)->hitbox_offset_x = 0;
		chaser_stopped = 0xFFu;
	}
}
