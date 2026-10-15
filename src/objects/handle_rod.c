/**
 *    Name:        objects/handle_rod.c
 *    Description: object type 14 (stage 9): the handle rod between two posts
 *    Version:     0.1.0
 *    Created:     12-10-2026
 *    Modified:    15-10-2026
 *    Author:      Sebastian Toyos
 *
 *    SegaSonic The Hedgehog (arcade, Sega System 32): the game's V60 program decompiled into C by
 *    tools/decompile.py, with the names of symbols.txt.
 */
#include "game.h"

void handle_rod_init(void)
{
	uint16_t angle_offset = 0, angle_step = 0;
	uint32_t r0 = 0, block = 0, handle_index = 0, object = 0, object_2 = 0, object_3 = 0, rod_dx = 0, rod_dz = 0, rod_length = 0;

	OBJECT(current_object)->sprite_scale = 0x100u;
	OBJECT(current_object)->sprite_frame = 0x73B8u;
	OBJECT(current_object)->sprite_palette = 0x9170u;
	OBJECT(current_object)->handle_rod.angle = 0x322u;
	OBJECT(current_object)->handle_rod.swing_accel = 0x60;
	r0 = OBJECT(current_object)->handle_rod.handle_count;
	spawn_count = r0;
	r0 = (uint16_t)(r0 * 0x12u);
	OBJECT(current_object)->z_int -= r0;
	spawn_objects(0x011474u, &r0, &object_2, &block);
	if ((uint8_t)r0 == 0u)
		return;
	OBJECT(current_object)->child = object_2;
	OBJECT(current_object)->parent = object_2;
	object_3 = LINKED(WORK_RAM, OBJECT(object_2)->child);
	OBJECT(current_object)->handler = handle_rod_wait_ADDRESS;
	handle_index = (r0 & ~0xFFFFu) | OBJECT(current_object)->handle_rod.handle_count;
	angle_step = divide_s16(0x400u, (uint16_t)handle_index);
	do
	{
		current_object = LINKED(current_object, OBJECT(current_object)->child);
		r14 = (uint16_t)current_object;
		copy_position_from_cursor();
		OBJECT(current_object)->sprite_frame = OBJECT(object_cursor)->sprite_frame;
		OBJECT(current_object)->sprite_palette = OBJECT(object_cursor)->sprite_palette;
		OBJECT(current_object)->handler = handle_rod_handle_init_ADDRESS;
		OBJECT(current_object)->var_82 = OBJECT(object_cursor)->handle_rod.angle;
		OBJECT(current_object)->var_84 = OBJECT(object_cursor)->handle_rod.angle_speed;
		OBJECT(current_object)->sprite_scale = OBJECT(object_cursor)->sprite_scale;
		AS16(OBJECT(current_object)->var_80) = handle_index;
		angle_offset = (uint16_t)(angle_step * handle_index);
		OBJECT(current_object)->spawn_arg_1 = 0x400u;
		OBJECT(current_object)->spawn_arg_1 -= angle_offset;
		OBJECT(current_object)->spawn_arg_2 += angle_offset;
		OBJECT(current_object)->z_int += handle_index * 0x12u;
		handle_index--;
	} while (handle_index != 0u);
	OBJECT(object_3)->var_81 = 1u;
	OBJECT(object_3)->var_86 = object_cursor;
	OBJECT(object_3)->spawn_arg_2 = OBJECT(object_cursor)->x_int;
	OBJECT(object_3)->hitbox_size_x = 0x10u;
	OBJECT(object_3)->kind = 0x1Au;
	current_object = LINKED(current_object, OBJECT(object_cursor)->child);
	OBJECT(object_cursor)->child = 0xFFFFu;
	OBJECT(object_cursor)->handle_rod.angle = 0x300u;
	object = LINKED(WORK_RAM, OBJECT(current_object)->child);
	OBJECT(current_object)->child = 0xFFFFu;
	OBJECT(current_object)->sprite_frame = 0x74C8u;
	OBJECT(current_object)->sprite_palette = 0x1060u;
	OBJECT(current_object)->projected_x = OBJECT(object)->projected_x;
	OBJECT(current_object)->projected_y = OBJECT(object)->projected_y;
	OBJECT(current_object)->projected_x -= 0x11u;
	OBJECT(current_object)->projected_y_int += 3u;
	OBJECT(current_object)->z = OBJECT(object_cursor)->z;
	OBJECT(current_object)->handler = handle_rod_post_init_ADDRESS;
	OBJECT(current_object)->sprite_scale = 0x100u;
	rod_length = OBJECT(current_object)->var_80 * 0x12u;
	rod_dx = sine_of_300 * 2u * 2u * rod_length;
	rod_dz = cosine_of_300 * 2u * 2u * rod_length;
	OBJECT(current_object)->projected_x += (int32_t)rod_dx >> 16;
	OBJECT(current_object)->projected_y += 0u - ((int32_t)rod_dx >> 1);
	OBJECT(current_object)->z += rod_dz;
	spawn_count = 1u;
	spawn_objects(0x0115D3u, &object, &object_2, &block);
	copy_position_to(object_2);
	OBJECT(object_2)->projected_x -= 0x12u;
	OBJECT(object_2)->projected_y_int -= 6u;
	OBJECT(object_2)->handle_rod_post.fallen = 1u;
	OBJECT(object_2)->handler = handle_rod_post_init_ADDRESS;
	OBJECT(object_cursor)->z_int -= 0x18u;
}

void handle_rod_handle_init(void)
{
	uint32_t first = 0, ignored = 0, object = 0, spawned = 0;

	spawn_count = 1u;
	add_pieces_to_current_object(&spawned, &ignored, &first, &object);
	if ((uint8_t)spawned != 0u)
	{
		OBJECT(current_object)->handler = handle_rod_handle_update_ADDRESS;
		OBJECT(object)->sprite_frame = 0x5D80u;
		OBJECT(object)->sprite_palette = 0x87A0u;
		OBJECT(object)->sprite_scale = 0x80u;
	}
}

void handle_rod_wait(void)
{
	uint32_t cursor = 0, depth = 0, ignored = 0;

	unproject_object(&ignored, &ignored, &ignored, &cursor);
	if (OBJECT(LINKED(WORK_RAM, OBJECT(current_object)->parent))->var_81 == 0u)
	{
		depth = (uint16_t)OBJECT(current_object)->screen_y;
		OBJECT(current_object)->hitbox_size_x = depth;
		queue_sprite_at_depth(&ignored, &depth);
		add_to_rider_list();
		return;
	}
	OBJECT(current_object)->handler = handle_rod_start_swing_ADDRESS;
	if (OBJECT(current_object)->handle_rod.drops_players == 0u)
	{
		players_pulled = 1u;
		event_12_fast_scroll = 1u;
	}
	else
		handle_rod_drop_players = 1u;
	depth = (uint16_t)OBJECT(current_object)->screen_y;
	OBJECT(current_object)->hitbox_size_x = depth;
	queue_sprite_at_depth(&ignored, &depth);
	add_to_rider_list();
}

void handle_rod_start_swing(void)
{
	handle_rod_drop_players = 0u;
	players_pulled = 0u;
	OBJECT(current_object)->handler = handle_rod_swing_ADDRESS;
	handle_rod_swing();
}

void handle_rod_swing(void)
{
	uint32_t cursor = 0, depth = 0, ignored = 0;

	if (OBJECT(current_object)->handle_rod.end_position < scroll_position)
	{
		OBJECT(current_object)->handle_rod.end_position = 0xFFFFu;
		OBJECT(current_object)->handler = handle_rod_remove_ADDRESS;
		return;
	}
	unproject_object(&ignored, &ignored, &ignored, &cursor);
	if (OBJECT(current_object)->handle_rod.swing_side != 0u)
	{
		if (OBJECT(current_object)->handle_rod.drops_players == 0u)
		{
			if ((int16_t)OBJECT(current_object)->handle_rod.angle >= 0x200)
			{
				if (OBJECT(current_object)->handle_rod.angle_speed < -0x100)
					OBJECT(current_object)->handle_rod.angle_speed += 0x100u;
				OBJECT(current_object)->handle_rod.angle_speed++;
				OBJECT(current_object)->handle_rod.swing_accel = 0u - OBJECT(current_object)->handle_rod.swing_accel;
				OBJECT(current_object)->handle_rod.swing_side = 0u;
			}
		}
		else
		{
			OBJECT(current_object)->handle_rod.angle = 0u;
			OBJECT(current_object)->handle_rod.angle_speed = (int16_t)OBJECT(current_object)->handle_rod.angle_speed >> 1;
			OBJECT(current_object)->handle_rod.angle_speed = 0u - OBJECT(current_object)->handle_rod.angle_speed;
			OBJECT(current_object)->handle_rod.swing_accel = 0u - OBJECT(current_object)->handle_rod.swing_accel;
			OBJECT(current_object)->handle_rod.swing_side = 0u;
		}
		OBJECT(current_object)->handle_rod.angle_speed += OBJECT(current_object)->handle_rod.swing_accel;
		OBJECT(current_object)->handle_rod.angle += OBJECT(current_object)->handle_rod.angle_speed_int;
		OBJECT(current_object)->handle_rod.angle &= 0x3FFu;
	}
	else
	{
		if (OBJECT(current_object)->handle_rod.angle != 0u || OBJECT(current_object)->handle_rod.angle_speed > 0x100)
		{
			if ((int16_t)OBJECT(current_object)->handle_rod.angle > 0x200)
			{
				OBJECT(current_object)->handle_rod.angle_speed += OBJECT(current_object)->handle_rod.swing_accel;
				OBJECT(current_object)->handle_rod.angle += OBJECT(current_object)->handle_rod.angle_speed_int;
				OBJECT(current_object)->handle_rod.angle &= 0x3FFu;
				depth = (uint16_t)OBJECT(current_object)->screen_y;
				OBJECT(current_object)->hitbox_size_x = depth;
				queue_sprite_at_depth(&ignored, &depth);
				add_to_rider_list();
				return;
			}
			if (OBJECT(current_object)->handle_rod.angle_speed > 0x100)
				OBJECT(current_object)->handle_rod.angle_speed -= 0x100u;
			OBJECT(current_object)->handle_rod.angle_speed--;
			OBJECT(current_object)->handle_rod.swing_accel = 0u - OBJECT(current_object)->handle_rod.swing_accel;
			OBJECT(current_object)->handle_rod.swing_side = 1u;
			OBJECT(current_object)->handle_rod.angle_speed += OBJECT(current_object)->handle_rod.swing_accel;
			OBJECT(current_object)->handle_rod.angle += OBJECT(current_object)->handle_rod.angle_speed_int;
			OBJECT(current_object)->handle_rod.angle &= 0x3FFu;
			depth = (uint16_t)OBJECT(current_object)->screen_y;
			OBJECT(current_object)->hitbox_size_x = depth;
			queue_sprite_at_depth(&ignored, &depth);
			add_to_rider_list();
			return;
		}
		OBJECT(current_object)->handle_rod.angle = 0u;
		OBJECT(current_object)->handler = handle_rod_hang_ADDRESS;
	}
	depth = (uint16_t)OBJECT(current_object)->screen_y;
	OBJECT(current_object)->hitbox_size_x = depth;
	queue_sprite_at_depth(&ignored, &depth);
	add_to_rider_list();
}

void handle_rod_hang(void)
{
	uint32_t r1 = 0, cursor = 0, depth = 0, ignored = 0, z = 0;

	if ((OBJECT(current_object)->flags & 8u) == 0u && OBJECT(current_object)->floor_z_int >= 0x800)
		fall_to_floor(&ignored, &r1, &z);
	move_object_projected(&ignored, &ignored, &ignored, &cursor);
	if (OBJECT(current_object)->handle_rod.end_position < scroll_position)
	{
		OBJECT(current_object)->handle_rod.end_position = 0xFFFFu;
		OBJECT(current_object)->handler = handle_rod_remove_ADDRESS;
		return;
	}
	depth = (uint16_t)OBJECT(current_object)->screen_y;
	OBJECT(current_object)->hitbox_size_x = depth;
	queue_sprite_at_depth(&ignored, &depth);
	add_to_rider_list();
}

void handle_rod_remove(void)
{
	uint32_t block_offset = 0, ignored = 0, slope_height = 0;

	delete_object_and_children(&ignored, &block_offset, &slope_height);
}

void handle_rod_handle_update(void)
{
	uint32_t cursor = 0, depth = 0, handle_distance = 0, ignored = 0, rod_angle = 0, rod_dx = 0, rod_dz = 0;

	r14 = LINKED(WORK_RAM, OBJECT(current_object)->parent);
	if (OBJECT(r14)->spawn_arg_2 == 0xFFFFu)
	{
		delete_object_and_children(&ignored, &rod_dz, &handle_distance);
		return;
	}
	if (OBJECT(current_object)->handle_rod_handle.grip == 0u)
		move_object_projected_no_floor();
	else
	{
		move_object_projected(&ignored, &ignored, &ignored, &cursor);
		add_to_touchable_list(ignored);
		add_to_rider_list();
	}
	OBJECT(current_object)->speed_x = 0;
	OBJECT(current_object)->speed_y = 0;
	OBJECT(current_object)->speed_z = 0;
	if (OBJECT(current_object)->handle_rod_handle.start_angle == 0x300u || OBJECT(r14)->var_82 != 0u)
	{
		rod_angle = OBJECT(r14)->var_82;
		handle_distance = OBJECT(current_object)->handle_rod_handle.index * 0x12u;
		rod_dx = sine_table[rod_angle] * 2u * 2u * handle_distance;
		rod_dz = cosine_table[rod_angle] * 2u * 2u * handle_distance;
		OBJECT(current_object)->speed_x = rod_dx;
		OBJECT(current_object)->speed_y = 0u - ((int32_t)rod_dx >> 1);
		OBJECT(current_object)->speed_z = rod_dz;
	}
	else
	{
		OBJECT(current_object)->speed_x = 0;
		OBJECT(current_object)->speed_y = 0;
		OBJECT(current_object)->speed_z = 0x10000u * (OBJECT(current_object)->handle_rod_handle.index * 0x12u);
	}
	OBJECT(current_object)->projected_x = OBJECT(r14)->projected_x;
	OBJECT(current_object)->projected_y = OBJECT(r14)->projected_y;
	OBJECT(current_object)->z = OBJECT(r14)->z;
	OBJECT(current_object)->floor_z = OBJECT(r14)->floor_z;
	if (OBJECT(current_object)->screen_x <= 0x145 && OBJECT(current_object)->screen_y >= 0)
	{
		depth = OBJECT(current_object)->handle_rod_handle.index;
		depth = (uint16_t)(depth * 2u);
		depth = (uint16_t)(depth * 2u);
		depth = (uint16_t)(depth + OBJECT(r14)->hitbox_size_x);
		queue_sprite_at_depth(&ignored, &depth);
		if (AT8(OBJECT(r14)->spawn_arg_1, 1) == 0u)
		{
			current_object = LINKED(current_object, OBJECT(current_object)->child);
			place_sprite_at_cursor_floor();
			queue_sprite(depth);
		}
	}
}

void handle_rod_post_init(void)
{
	uint32_t r3 = 0, cursor = 0, ignored = 0, object = 0;

	unproject_object(&ignored, &ignored, &r3, &cursor);
	add_to_rider_list();
	OBJECT(current_object)->sprite_frame = 0x74C8u;
	if (OBJECT(current_object)->handle_rod_post.fallen != 0u)
		OBJECT(current_object)->sprite_frame = 0x74D8u;
	OBJECT(current_object)->sprite_palette = 0x9060u;
	OBJECT(current_object)->handler = handle_rod_post_update_ADDRESS;
	if (OBJECT(current_object)->handle_rod_post.fallen == 0u)
	{
		spawn_count = 1u;
		add_pieces_to_current_object(&ignored, &ignored, &r3, &object);
		OBJECT(object)->sprite_frame = 0x74D0u;
		OBJECT(object)->sprite_palette = OBJECT(object_cursor)->sprite_palette;
	}
}

void handle_rod_post_update(void)
{
	uint8_t bucket = 0;
	uint16_t offset_x = 0, offset_y = 0;
	uint32_t cursor = 0, depth = 0, ignored = 0;

	unproject_object(&ignored, &ignored, &ignored, &cursor);
	if ((OBJECT(current_object)->flags & 8u) == 0u && OBJECT(current_object)->floor_z_int >= 0x800)
	{
		OBJECT(current_object)->handle_rod_post.fallen = 1u;
		OBJECT(current_object)->handler = handle_rod_post_fall_ADDRESS;
	}
	add_to_rider_list();
	if (OBJECT(current_object)->handle_rod_post.fallen == 0u)
		queue_sprite(0u);
	else
	{
		depth = OBJECT(LINKED(WORK_RAM, OBJECT(current_object)->parent))->hitbox_size_x;
		queue_sprite_at_depth(&ignored, &depth);
	}
	if (OBJECT(current_object)->handle_rod_post.fallen == 0u)
	{
		current_object = LINKED(current_object, OBJECT(current_object)->child);
		offset_x = 0x13u;
		offset_y = 0xFFD6u;
		place_sprite_near_cursor(offset_x, offset_y);
		bucket = 0u;
		queue_sprite_in_bucket(bucket);
	}
}

void handle_rod_post_fall(void)
{
	uint32_t r1 = 0, r2 = 0, cursor = 0, deleted = 0, ignored = 0, slope_height = 0;

	move_object_projected(&ignored, &r2, &slope_height, &cursor);
	delete_if_off_screen(&deleted, &r2, &slope_height);
	if ((uint8_t)deleted != 1u)
	{
		fall_to_floor(&ignored, &r1, &r2);
		queue_sprite_at_floor();
	}
}
