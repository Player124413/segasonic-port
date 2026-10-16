/**
 *    Name:        objects/stage_09.c
 *    Description: stage 9's objects, the ice: the handle rod between two posts, the spear traps, the ice cube launcher, the padded wall, the snow mound, the ice blocks and the moving ice platform, the crumbling ice floor, the icicles
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

void spear_trap_init_100(void)
{
	OBJECT(current_object)->direction = 0x100u;
	spear_trap_init();
}

void spear_trap_init_300(void)
{
	OBJECT(current_object)->direction = 0x300u;
	spear_trap_init();
}

void spear_trap_init(void)
{
	OBJECT(current_object)->sprite_flags |= 0x4000u;
	OBJECT(current_object)->sprite_flags = 0x5000u;
	OBJECT(current_object)->sprite_frame = 0x7FFu;
	OBJECT(current_object)->sprite_palette = 0u;
	OBJECT(current_object)->sprite_scale = 0x7FFu;
	OBJECT(current_object)->flags |= 2u;
	OBJECT(current_object)->hitbox_size_x = 0x18u;
	OBJECT(current_object)->hitbox_size_y = 0x18u;
	OBJECT(current_object)->hitbox_offset_x = -4;
	OBJECT(current_object)->hitbox_offset_y = -0x13;
	OBJECT(current_object)->hitbox_height = 0x100u;
	OBJECT(current_object)->spear_trap.phase_timer = OBJECT(current_object)->spawn_arg_2;
	OBJECT(current_object)->spear_trap.spear_z = OBJECT(current_object)->z;
	OBJECT(current_object)->handler = spear_trap_wait_ADDRESS;
	current_object = LINKED(current_object, OBJECT(current_object)->child);
	OBJECT(current_object)->sprite_frame = 0x73A0u;
	OBJECT(current_object)->sprite_palette = 0x9050u;
	OBJECT(current_object)->sprite_flags = 0u;
	current_object = LINKED(current_object, OBJECT(current_object)->child);
	OBJECT(current_object)->sprite_flags |= 0x4000u;
	OBJECT(current_object)->sprite_flags = 0x5000u;
	OBJECT(current_object)->sprite_palette = 0u;
	OBJECT(current_object)->sprite_scale = 0x7FFu;
	current_object = LINKED(current_object, OBJECT(current_object)->child);
	OBJECT(current_object)->sprite_flags = 0x5000u;
	OBJECT(current_object)->sprite_frame = 0x7FFu;
	OBJECT(current_object)->sprite_palette = 0u;
	OBJECT(current_object)->sprite_scale = 0x7FFu;
	current_object = LINKED(current_object, OBJECT(current_object)->child);
	OBJECT(current_object)->sprite_frame = 0x73A0u;
	OBJECT(current_object)->sprite_palette = 0x1050u;
	OBJECT(current_object)->sprite_palette |= 0x2000u;
	OBJECT(current_object)->sprite_flags = 0u;
	current_object = LINKED(current_object, OBJECT(current_object)->child);
	OBJECT(current_object)->sprite_flags = 0x5000u;
	OBJECT(current_object)->sprite_palette = 0u;
	OBJECT(current_object)->sprite_scale = 0x7FFu;
	current_object = LINKED(current_object, OBJECT(current_object)->child);
	OBJECT(current_object)->sprite_frame = 0x7390u;
	OBJECT(current_object)->sprite_palette = 0x1030u;
	OBJECT(current_object)->sprite_palette |= 0x2000u;
	current_object = LINKED(current_object, OBJECT(current_object)->child);
	OBJECT(current_object)->sprite_frame = 0x7388u;
	OBJECT(current_object)->sprite_palette = 0x1030u;
	OBJECT(current_object)->sprite_palette |= 0x2000u;
	current_object = LINKED(current_object, OBJECT(current_object)->child);
	OBJECT(current_object)->sprite_frame = 0x7398u;
	OBJECT(current_object)->sprite_palette = 0x9040u;
	current_object = LINKED(current_object, OBJECT(current_object)->child);
	OBJECT(current_object)->sprite_frame = 0x73A8u;
	OBJECT(current_object)->sprite_palette = 0x9040u;
}

void spear_trap_wait(void)
{
	uint32_t block_offset = 0, deleted = 0, slope_height = 0;
	uint16_t phase_timer = 0;

	OBJECT(current_object)->health = 0x7F;
	delete_if_off_screen(&deleted, &block_offset, &slope_height);
	if ((uint8_t)deleted == 1u)
		return;
	phase_timer = OBJECT(current_object)->spear_trap.phase_timer;
	OBJECT(current_object)->spear_trap.phase_timer = phase_timer - 1u;
	if (phase_timer != 0u)
	{
		spear_trap_draw();
		return;
	}
	OBJECT(current_object)->spear_trap.phase_timer = 0xCu;
	OBJECT(current_object)->kind = 4u;
	OBJECT(current_object)->handler = spear_trap_rise_ADDRESS;
	spear_trap_rise();
}

void spear_trap_rise(void)
{
	uint8_t sound_id = 0;
	uint32_t write_index = 0, write_index_address = 0;
	uint16_t phase_timer = 0;

	OBJECT(current_object)->speed_z -= 0x10F00u;
	phase_timer = OBJECT(current_object)->spear_trap.phase_timer;
	OBJECT(current_object)->spear_trap.phase_timer = phase_timer - 1u;
	if (phase_timer != 0u)
	{
		spear_trap_draw_touchable();
		return;
	}
	sound_id = 0x1Bu;
	play_sound(sound_id, &write_index, &write_index_address);
	OBJECT(current_object)->speed_z = 0;
	OBJECT(current_object)->spear_trap.phase_timer = 0x3Cu;
	OBJECT(current_object)->kind = 0xCu;
	OBJECT(current_object)->handler = spear_trap_stay_up_ADDRESS;
	spear_trap_stay_up();
}

void spear_trap_stay_up(void)
{
	uint16_t phase_timer = 0;

	phase_timer = OBJECT(current_object)->spear_trap.phase_timer;
	OBJECT(current_object)->spear_trap.phase_timer = phase_timer - 1u;
	if (phase_timer != 0u)
	{
		spear_trap_draw_touchable();
		return;
	}
	OBJECT(current_object)->handler = spear_trap_retract_ADDRESS;
	spear_trap_retract();
}

void spear_trap_retract(void)
{
	uint32_t r1 = 0, height = 0, z = 0;

	OBJECT(current_object)->z = OBJECT(current_object)->spear_trap.spear_z;
	fall_to_floor(&height, &r1, &z);
	if (height != 0u)
	{
		spear_trap_draw_touchable();
		return;
	}
	OBJECT(current_object)->spear_trap.phase_timer = OBJECT(current_object)->spawn_arg_1;
	OBJECT(current_object)->handler = spear_trap_wait_ADDRESS;
	spear_trap_draw_touchable();
}

void spear_trap_draw_touchable(void)
{
	add_to_touchable_list(0u);
	spear_trap_draw();
}

void spear_trap_draw(void)
{
	uint8_t bucket = 0;
	uint32_t cursor = 0, depth = 0, ignored = 0;

	OBJECT(current_object)->z = OBJECT(current_object)->spear_trap.spear_z;
	move_object_projected(&ignored, &ignored, &ignored, &cursor);
	r14 = (uint16_t)(OBJECT(current_object)->floor_z_int - OBJECT(current_object)->z_int + OBJECT(current_object)->screen_y);
	depth = r14;
	queue_sprite_at_depth(&ignored, &depth);
	current_object = LINKED(current_object, OBJECT(current_object)->child);
	OBJECT(current_object)->screen_x = OBJECT(object_cursor)->screen_x;
	OBJECT(current_object)->screen_y = OBJECT(object_cursor)->screen_y;
	OBJECT(current_object)->screen_y += 0x39u;
	depth = (uint16_t)r14;
	queue_sprite_at_depth(&ignored, &depth);
	current_object = LINKED(current_object, OBJECT(current_object)->child);
	OBJECT(current_object)->sprite_frame = 0xFFFBu + r14;
	depth = (uint16_t)r14;
	queue_sprite_at_depth(&ignored, &depth);
	current_object = LINKED(current_object, OBJECT(current_object)->child);
	bucket = 0xFFu;
	queue_sprite_in_bucket(bucket);
	current_object = LINKED(current_object, OBJECT(current_object)->child);
	OBJECT(current_object)->screen_x = OBJECT(object_cursor)->screen_x;
	OBJECT(current_object)->screen_y = OBJECT(object_cursor)->screen_y;
	OBJECT(current_object)->screen_y += 0x39u;
	bucket = 0xFFu;
	queue_sprite_in_bucket(bucket);
	current_object = LINKED(current_object, OBJECT(current_object)->child);
	OBJECT(current_object)->sprite_frame = 0x20u + r14;
	bucket = 0xFFu;
	queue_sprite_in_bucket(bucket);
	current_object = LINKED(current_object, OBJECT(current_object)->child);
	OBJECT(current_object)->screen_x = OBJECT(object_cursor)->screen_x;
	OBJECT(current_object)->screen_y = r14;
	OBJECT(current_object)->screen_y += 0x20u;
	bucket = 0xFEu;
	queue_sprite_in_bucket(bucket);
	current_object = LINKED(current_object, OBJECT(current_object)->child);
	OBJECT(current_object)->screen_x = OBJECT(object_cursor)->screen_x;
	OBJECT(current_object)->screen_y = r14;
	OBJECT(current_object)->screen_y += 0x20u;
	bucket = 0xFFu;
	queue_sprite_in_bucket(bucket);
	current_object = LINKED(current_object, OBJECT(current_object)->child);
	OBJECT(current_object)->screen_x = OBJECT(object_cursor)->screen_x;
	OBJECT(current_object)->screen_y = r14;
	depth = (uint16_t)(OBJECT(current_object)->screen_y - 0xFu);
	queue_sprite_at_depth(&ignored, &depth);
	current_object = LINKED(current_object, OBJECT(current_object)->child);
	OBJECT(current_object)->screen_x = OBJECT(object_cursor)->screen_x;
	OBJECT(current_object)->screen_y = r14;
	depth = (uint16_t)(OBJECT(current_object)->screen_y - 0xFu);
	queue_sprite_at_depth(&ignored, &depth);
	current_object = object_cursor;
	OBJECT(current_object)->spear_trap.spear_z = OBJECT(current_object)->z;
	OBJECT(current_object)->z = OBJECT(current_object)->floor_z;
}

void ice_launcher_init(void)
{
	OBJECT(current_object)->sprite_frame = 0x7420u;
	OBJECT(current_object)->sprite_palette = 0x9110u;
	OBJECT(current_object)->ice_launcher.last_step = 0xF;
	OBJECT(current_object)->sprite_flags = 9u;
	OBJECT(current_object)->flags |= 2u;
	OBJECT(current_object)->hitbox_height = 0x28u;
	OBJECT(current_object)->kind = 5u;
	OBJECT(current_object)->handler = ice_launcher_update_ADDRESS;
}

void ice_launcher_update(void)
{
	uint32_t r0 = 0, r11 = 0, block_offset = 0, depth = 0, frame_index = 0, ignored = 0, object = 0, offset_record = 0, size_record = 0;
	uint16_t step_timer = 0;

	unproject_object(&r0, &block_offset, &object, &r11);
	if (OBJECT(current_object)->spawn_arg_2 <= scroll_position)
	{
		delete_object_and_children(&ignored, &block_offset, &object);
		return;
	}
	step_timer = OBJECT(current_object)->ice_launcher.step_timer;
	OBJECT(current_object)->ice_launcher.step_timer = step_timer - 1u;
	if (step_timer != 0u)
	{
		add_to_touchable_list(r0);
		depth = (uint16_t)(OBJECT(current_object)->screen_y + 0x30u);
		queue_sprite_at_depth(&ignored, &depth);
		return;
	}
	OBJECT(current_object)->ice_launcher.step++;
	if (OBJECT(current_object)->ice_launcher.step <= OBJECT(current_object)->ice_launcher.last_step)
	{
		frame_index = ice_launcher_frames[(uint8_t)OBJECT(current_object)->ice_launcher.step];
		OBJECT(current_object)->sprite_frame = ice_launcher_sprites[frame_index];
		size_record = ice_launcher_hitbox_sizes_ADDRESS + frame_index * 4u;
		OBJECT(current_object)->hitbox_size_x = ROM16(size_record);
		OBJECT(current_object)->hitbox_size_y = ROM16(size_record + 2u);
		offset_record = ice_launcher_hitbox_offsets_ADDRESS + frame_index * 2u;
		OBJECT(current_object)->hitbox_offset_x = ROM8(offset_record);
		offset_record++;
		OBJECT(current_object)->hitbox_offset_y = ROM8(offset_record);
		r0 = OBJECT(current_object)->ice_launcher.step_duration_index;
		OBJECT(current_object)->ice_launcher.step_timer = ice_launcher_frame_durations[r0];
		if ((uint8_t)OBJECT(current_object)->ice_launcher.step == 0xCu)
		{
			spawn_count = 2u;
			spawn_object_with_pieces(0x011D7Du, &r0, &size_record, &offset_record, &object, &r11);
			if ((uint8_t)r0 != 0u)
			{
				OBJECT(object_cursor)->ice_launcher.step_timer += 3u;
				copy_position_to(object);
				OBJECT(object)->projected_x -= 0x2Bu;
				OBJECT(object)->projected_y_int += 0x4Fu;
				OBJECT(object)->sprite_frame = 0x7418u;
				OBJECT(object)->sprite_palette = OBJECT(object_cursor)->sprite_palette;
				OBJECT(object)->flags |= 2u;
				OBJECT(object)->hitbox_size_x = 0x22u;
				OBJECT(object)->hitbox_size_y = 0x42u;
				OBJECT(object)->hitbox_offset_x = -0xB;
				OBJECT(object)->hitbox_offset_y = -0x3C;
				OBJECT(object)->hitbox_height = 0x28u;
				OBJECT(object)->kind = 0x19u;
				OBJECT(object)->handler = ice_cube_slide_ADDRESS;
				OBJECT(object)->speed = ice_cube_speeds[OBJECT(current_object)->ice_launcher.cube_speed_index];
				object = LINKED(object, OBJECT(object)->child);
				OBJECT(object)->sprite_frame = 0x7418u;
				OBJECT(object)->sprite_palette = 0x11D0u;
				OBJECT(object)->sprite_palette |= 0x2000u;
			}
		}
	}
	else
	{
		OBJECT(current_object)->ice_launcher.step = 0;
		OBJECT(current_object)->sprite_frame = 0x7420u;
		OBJECT(current_object)->ice_launcher.step_timer = 0u;
	}
	add_to_touchable_list(r0);
	depth = (uint16_t)(OBJECT(current_object)->screen_y + 0x30u);
	queue_sprite_at_depth(&ignored, &depth);
}

void ice_cube_slide(void)
{
	uint8_t bucket = 0;
	uint32_t r1 = 0, r2 = 0, cursor = 0, deleted = 0, depth = 0, direction = 0, ignored = 0, slope_height = 0, speed = 0, speed_x = 0, speed_y = 0;

	OBJECT(current_object)->health = 0x7F;
	move_object_projected(&ignored, &r2, &slope_height, &cursor);
	fall_to_floor(&ignored, &r1, &r2);
	delete_if_off_screen(&deleted, &r2, &slope_height);
	if ((uint8_t)deleted != 1u)
	{
		direction = 0x2B6u;
		speed = OBJECT(current_object)->speed;
		direction_to_speed(speed, &direction, &speed_x, &speed_y);
		OBJECT(current_object)->speed_x = speed_x;
		OBJECT(current_object)->speed_y = speed_y;
		depth = (uint16_t)(OBJECT(current_object)->screen_y - 0x10u);
		queue_sprite_at_depth(&ignored, &depth);
		current_object = LINKED(current_object, OBJECT(current_object)->child);
		OBJECT(current_object)->screen_x = OBJECT(object_cursor)->screen_x;
		OBJECT(current_object)->screen_y = OBJECT(object_cursor)->screen_y;
		OBJECT(current_object)->screen_y += 0x1Eu;
		bucket = 0xFFu;
		queue_sprite_in_bucket(bucket);
		current_object = object_cursor;
		add_to_hitter_list();
	}
}

void object_type_17_idle(void)
{
}

void padded_wall_init(void)
{
	uint16_t pad_offset_x = 0, pad_offset_y = 0;
	uint32_t block = 0, object = 0, pads_left = 0, spawned = 0;

	spawn_count = 4u;
	spawn_objects(0x011F2Bu, &spawned, &object, &block);
	if ((uint8_t)spawned == 0u)
		return;
	OBJECT(current_object)->sprite_frame = 0x77D0u;
	OBJECT(current_object)->sprite_palette = 0x91A0u;
	OBJECT(current_object)->handler = padded_wall_update_ADDRESS;
	OBJECT(current_object)->sprite_flags = 0xAu;
	pads_left = 4u;
	pad_offset_x = 0u;
	pad_offset_y = 0u;
	do
	{
		copy_position_to(object);
		OBJECT(object)->direction = 0x300u;
		OBJECT(object)->projected_x += 7u;
		OBJECT(object)->projected_y_int -= 0xAu;
		pad_offset_x += 0x26u;
		pad_offset_y += 0x13u;
		OBJECT(object)->projected_x += pad_offset_x;
		OBJECT(object)->projected_y_int += pad_offset_y;
		OBJECT(object)->sprite_frame = 0x77A8u;
		OBJECT(object)->sprite_palette = 0x9180u;
		OBJECT(object)->spawn_arg_1 = OBJECT(object_cursor)->spawn_arg_1;
		OBJECT(object)->sprite_flags = 9u;
		OBJECT(object)->flags |= 2u;
		OBJECT(object)->hitbox_size_x = 0x17u;
		OBJECT(object)->hitbox_size_y = 0x35u;
		OBJECT(object)->hitbox_offset_x = -0x6E;
		OBJECT(object)->hitbox_offset_y = 0x15;
		OBJECT(object)->hitbox_height = 0x20u;
		OBJECT(object)->kind = 0xBu;
		OBJECT(object)->handler = wall_pad_update_ADDRESS;
		current_object = LINKED(current_object, OBJECT(object)->child);
		OBJECT(object)->child = 0xFFFFu;
		object = current_object;
		pads_left--;
	} while (pads_left != 0u);
}

void padded_wall_update(void)
{
	uint32_t block_offset = 0, cursor = 0, ignored = 0, slope_height = 0;

	unproject_object(&ignored, &block_offset, &slope_height, &cursor);
	if (scroll_position <= OBJECT(current_object)->spawn_arg_1)
	{
		queue_sprite(0u);
		return;
	}
	delete_object_and_children(&ignored, &block_offset, &slope_height);
}

void wall_pad_update(void)
{
	uint32_t block_offset = 0, cursor = 0, frame_index = 0, ignored = 0, pad_step = 0, slope_height = 0;
	uint16_t frame_timer = 0;

	unproject_object(&pad_step, &block_offset, &slope_height, &cursor);
	if (OBJECT(current_object)->spawn_arg_1 < scroll_position)
	{
		delete_object_and_children(&ignored, &block_offset, &slope_height);
		return;
	}
	if (OBJECT(current_object)->wall_pad.frame_step != 0u || (OBJECT(current_object)->flags & 4u) != 0u)
	{
		frame_timer = OBJECT(current_object)->wall_pad.frame_timer;
		OBJECT(current_object)->wall_pad.frame_timer = frame_timer - 1u;
		if (frame_timer == 0u)
		{
			OBJECT(current_object)->wall_pad.frame_step++;
			OBJECT(current_object)->wall_pad.frame_step = OBJECT(current_object)->wall_pad.frame_step % 0xBu;
			pad_step = OBJECT(current_object)->wall_pad.frame_step;
			frame_index = wall_pad_frames[pad_step];
			OBJECT(current_object)->sprite_frame = wall_pad_sprites[frame_index];
			OBJECT(current_object)->wall_pad.frame_timer = wall_pad_frame_durations[pad_step];
		}
	}
	queue_sprite(frame_index);
	add_to_touchable_list(pad_step);
}

void snow_mound_init(void)
{
	OBJECT(current_object)->projected_y_int += 0x20u;
	add_to_rider_list();
	OBJECT(current_object)->sprite_flags = 0u;
	OBJECT(current_object)->sprite_frame = 0x7B30u;
	OBJECT(current_object)->sprite_palette = 0x91B0u;
	OBJECT(current_object)->flags |= 2u;
	OBJECT(current_object)->hitbox_size_x = 0x22u;
	OBJECT(current_object)->hitbox_size_y = 0x21u;
	OBJECT(current_object)->hitbox_offset_x = -0x21;
	OBJECT(current_object)->hitbox_offset_y = 3;
	OBJECT(current_object)->hitbox_height = 0x20u;
	OBJECT(current_object)->kind = 5u;
	OBJECT(current_object)->snow_mound.part_3_frame = 4u;
	OBJECT(current_object)->handler = snow_mound_update_ADDRESS;
	current_object = LINKED(current_object, OBJECT(current_object)->child);
	OBJECT(current_object)->sprite_flags = 0u;
	OBJECT(current_object)->sprite_palette = 0x91B0u;
	current_object = LINKED(current_object, OBJECT(current_object)->child);
	OBJECT(current_object)->sprite_flags = 0u;
	OBJECT(current_object)->sprite_flags |= 0x40u;
	OBJECT(current_object)->sprite_palette = 0x91B0u;
	current_object = LINKED(current_object, OBJECT(current_object)->child);
	OBJECT(current_object)->sprite_flags = 0u;
	OBJECT(current_object)->sprite_flags |= 0x80u;
	OBJECT(current_object)->sprite_palette = 0x11C0u;
	OBJECT(current_object)->sprite_palette |= 0x2000u;
}

void snow_mound_update(void)
{
	uint8_t bucket = 0;
	uint16_t offset_x = 0, offset_y = 0;
	uint32_t r2 = 0, r3 = 0, r11 = 0, burst_frame = 0, deleted = 0, depth = 0, ignored = 0, object = 0, object_2 = 0, random_value = 0, ring = 0;

	move_object_projected(&ignored, &r2, &r3, &r11);
	fall_to_floor(&ignored, &depth, &r2);
	delete_if_off_screen(&deleted, &r2, &r3);
	if ((uint8_t)deleted == 1u)
		return;
	if (OBJECT(current_object)->snow_mound.bursting != 0u)
	{
	snow_mound_burst:
		OBJECT(current_object)->snow_mound.frame_count++;
		OBJECT(current_object)->snow_mound.part_3_count++;
		if (OBJECT(current_object)->snow_mound.frame_count <= 1)
		{
		snow_mound_draw:
			depth = (depth & ~0xFFFFu) | (uint16_t)OBJECT(current_object)->screen_y;
			depth = (depth & ~0xFFFFu) | (uint16_t)(depth + 0x12u);
			queue_sprite_at_depth(&ignored, &depth);
			if ((uint8_t)OBJECT(current_object)->snow_mound.burst_frame != 0u)
			{
				current_object = LINKED(current_object, OBJECT(object_cursor)->child);
				OBJECT(current_object)->screen_x = OBJECT(object_cursor)->screen_x;
				OBJECT(current_object)->screen_y = OBJECT(object_cursor)->screen_y;
				depth = (depth & ~0xFFFFu) | (uint16_t)OBJECT(current_object)->screen_y;
				depth = (depth & ~0xFFFFu) | (uint16_t)(depth + 0x11u);
				queue_sprite_at_depth(&ignored, &depth);
				current_object = LINKED(current_object, OBJECT(object_cursor)->child);
				current_object = LINKED(current_object, OBJECT(current_object)->child);
				OBJECT(current_object)->screen_x = OBJECT(object_cursor)->screen_x;
				OBJECT(current_object)->screen_y = OBJECT(object_cursor)->screen_y;
				OBJECT(current_object)->screen_y -= 0x16u;
				queue_sprite(depth);
			}
			else
			{
				add_to_touchable_list(ignored);
				add_to_rider_list();
			}
			current_object = LINKED(current_object, OBJECT(object_cursor)->child);
			object = current_object;
			current_object = LINKED(current_object, OBJECT(current_object)->child);
			current_object = LINKED(current_object, OBJECT(current_object)->child);
			OBJECT(current_object)->sprite_frame = OBJECT(object_cursor)->sprite_frame;
			if ((uint8_t)OBJECT(object_cursor)->snow_mound.burst_frame != 0u)
				OBJECT(current_object)->sprite_frame = OBJECT(object)->sprite_frame;
			offset_x = 0u;
			offset_y = 0x15u;
			place_sprite_near_cursor(offset_x, offset_y);
			bucket = 0xFFu;
			queue_sprite_in_bucket(bucket);
			return;
		}
		if (OBJECT(current_object)->snow_mound.burst_frame <= 0xE)
		{
			burst_frame = (uint8_t)OBJECT(current_object)->snow_mound.burst_frame;
			OBJECT(current_object)->sprite_frame = snow_mound_burst_frames[burst_frame];
			OBJECT(current_object)->snow_mound.frame_count = 0;
			OBJECT(current_object)->snow_mound.burst_frame++;
			burst_frame += 2u;
			if ((int8_t)burst_frame > 0xE)
				goto snow_mound_draw;
			object_2 = LINKED(WORK_RAM, OBJECT(current_object)->child);
			OBJECT(object_2)->sprite_frame = snow_mound_burst_frames[burst_frame];
			if (OBJECT(current_object)->snow_mound.part_3_count <= 2)
				goto snow_mound_draw;
			object_2 = LINKED(object_2, OBJECT(current_object)->child);
			object_2 = LINKED(object_2, OBJECT(object_2)->child);
			OBJECT(current_object)->snow_mound.part_3_count = 0;
			OBJECT(current_object)->snow_mound.part_3_frame++;
			OBJECT(object_2)->sprite_frame = snow_mound_burst_frames[OBJECT(current_object)->snow_mound.part_3_frame];
			goto snow_mound_draw;
		}
		OBJECT(current_object)->snow_mound.frame_count = 0;
		OBJECT(current_object)->snow_mound.part_3_count = 0;
		OBJECT(current_object)->snow_mound.burst_frame = 0;
		OBJECT(current_object)->snow_mound.bursting = 0u;
		OBJECT(current_object)->snow_mound.part_3_frame = 4u;
		OBJECT(current_object)->sprite_frame = snow_mound_burst_frames[(uint8_t)OBJECT(current_object)->snow_mound.burst_frame];
		delete_object_and_children(&ignored, &r2, &r3);
		return;
	}
	if ((OBJECT(current_object)->flags & 4u) == 0u)
		goto snow_mound_draw;
	OBJECT(current_object)->snow_mound.bursting = 1u;
	if (OBJECT(current_object)->snow_mound.ring_count == 0u)
		goto snow_mound_burst;
	released_ring_flip = 0u;
	rings_to_release = OBJECT(current_object)->snow_mound.ring_count;
	released_ring_angle = OBJECT(current_object)->spawn_arg_2;
	released_rings_random = OBJECT(current_object)->snow_mound.random_scatter;
	drop_rings(r3, &r11, &random_value, &ring);
	goto snow_mound_burst;
}

void ice_block_row_start(void)
{
	uint32_t ignored = 0, object = 0;
	uint16_t blocks_left = 0;

	OBJECT(current_object)->ice_block_row.blocks_left = 0xE;
	OBJECT(current_object)->handler = ice_block_row_start_ADDRESS;
	if (free_object_stack < OBJECT(current_object)->ice_block_row.blocks_left)
		return;
	do
	{
		allocate_object(&ignored, &object);
		OBJECT(object)->sprite_flags = 0x1002u;
		OBJECT(object)->sprite_frame = 0x7498u;
		OBJECT(object)->sprite_palette = 0x9130u;
		OBJECT(object)->handler = ice_block_update_ADDRESS;
		OBJECT(object)->flags = 2u;
		OBJECT(object)->hitbox_size_x = 0x20u;
		OBJECT(object)->hitbox_size_y = 0x5Cu;
		OBJECT(object)->hitbox_height = 0x50u;
		OBJECT(object)->sprite_offset_x = 0;
		OBJECT(object)->sprite_offset_y = 0;
		OBJECT(object)->hitbox_offset_x = 0;
		OBJECT(object)->hitbox_offset_y = 0;
		OBJECT(object)->kind = 0xCu;
		OBJECT(object)->speed_z = 0;
		OBJECT(object)->ice_block.state = 0u;
		OBJECT(object)->x = -0x2B20000;
		OBJECT(object)->y = OBJECT(current_object)->y;
		OBJECT(object)->z = 0x780000;
		OBJECT(current_object)->y_int -= 0x5Cu;
		blocks_left = OBJECT(current_object)->ice_block_row.blocks_left;
		OBJECT(current_object)->ice_block_row.blocks_left = blocks_left - 1u;
	} while ((int16_t)blocks_left > 1);
	delete_current_object();
}

void ice_block_update(void)
{
	uint32_t depth = 0, ignored = 0, margin = 0, object = 0, off_screen = 0, push_speed = 0;

	switch (OBJECT(current_object)->ice_block.state)
	{
	case 0:
	case 3:
		OBJECT(current_object)->ice_block.state++;
		project_object(&ignored, &ignored);
		return;
	case 1:
		margin = 0x50u;
		off_screen = check_left_screen_margin((uint16_t)margin);
		if (off_screen != 0u)
		{
			delete_current_object();
			return;
		}
		if ((OBJECT(current_object)->flags & 4u) == 0u)
		{
			add_to_touchable_list(push_speed);
			update_screen_position(&ignored, &margin);
			depth = (uint16_t)(OBJECT(current_object)->screen_y + 0x20u);
			queue_sprite_at_depth(&ignored, &depth);
			return;
		}
		OBJECT(current_object)->ice_block.state++;
		object = LINKED(current_object, OBJECT(current_object)->touched_by);
		push_speed = 0x10000u;
		if ((int8_t)OBJECT(object)->kind > 2)
			push_speed = (uint32_t)OBJECT(object)->speed_fraction << 8;
		OBJECT(current_object)->speed_x = push_speed;
		add_to_touchable_list(push_speed);
		update_screen_position(&ignored, &margin);
		depth = (uint16_t)(OBJECT(current_object)->screen_y + 0x20u);
		queue_sprite_at_depth(&ignored, &depth);
		return;
	case 2:
		OBJECT(current_object)->x -= OBJECT(current_object)->speed_x;
		if (OBJECT(current_object)->x_int <= -0x2BA)
		{
			OBJECT(current_object)->speed_z += 0x6000u;
			OBJECT(current_object)->z += OBJECT(current_object)->speed_z;
		}
		project_object(&ignored, &ignored);
		if (OBJECT(current_object)->screen_y < 0x120)
		{
			depth = (uint16_t)(OBJECT(current_object)->screen_y + 0x20u);
			queue_sprite_at_depth(&ignored, &depth);
			return;
		}
		delete_current_object();
		return;
	default:
		unknown_code(0x87E2Fu + OBJECT(current_object)->ice_block.state * 2u);
	}
}

void spawn_ice_platform_leader(uint32_t record, uint32_t *p_spawned, uint32_t *p_travel, uint32_t *p_leader)
{
	uint16_t travel = (uint16_t)*p_travel;
	uint32_t leader = *p_leader;
	uint16_t reference_x = 0;
	uint32_t free_count = 0, spawned = 0;

	allocate_object_outside_loop(&free_count, &leader);
	if (free_count == 0u)
	{
		*p_spawned = free_count;
		*p_leader = leader;
		return;
	}
	group_leaders[mem_read8(record) & 3u] = leader;
	record++;
	OBJECT(leader)->ice_platform_leader.end_position = mem_read16(record);
	record += 2u;
	reference_x = mem_read16(record);
	record += 2u;
	travel = (uint16_t)(mem_read16(record) - reference_x);
	OBJECT(leader)->ice_platform_leader.travel = travel;
	OBJECT(leader)->x_int = mem_read16(record);
	OBJECT(leader)->x_fraction = 0u;
	record += 2u;
	OBJECT(leader)->y_int = mem_read16(record);
	OBJECT(leader)->y_fraction = 0u;
	OBJECT(leader)->z_int = mem_read16(record + 2u);
	OBJECT(leader)->z_fraction = 0u;
	OBJECT(leader)->floor_z = OBJECT(leader)->z;
	OBJECT(leader)->handler = ice_platform_leader_move_ADDRESS;
	OBJECT(leader)->speed_x = 0x10000;
	OBJECT(leader)->speed_y = 0;
	OBJECT(leader)->flags = 0u;
	OBJECT(leader)->sprite_offset_x = 0;
	OBJECT(leader)->sprite_offset_y = 0;
	moving_floor_object = leader;
	spawned = 0xFFFFFFFFu;
	*p_spawned = spawned;
	*p_travel = (*p_travel & 0xFFFF0000u) | travel;
	*p_leader = leader;
}

void ice_platform_leader_move(void)
{
	uint8_t sound_id = 0;
	uint32_t r1 = 0, ignored = 0, write_index_address = 0;

	project_object(&ignored, &r1);
	if (OBJECT(current_object)->ice_platform_leader.end_position <= scroll_position)
	{
		delete_current_object();
		return;
	}
	OBJECT(current_object)->x += OBJECT(current_object)->speed_x;
	OBJECT(current_object)->ice_platform_leader.travel += OBJECT(current_object)->speed_x_int;
	if ((uint16_t)(OBJECT(current_object)->ice_platform_leader.travel + 0x87u) >= 0x10Fu)
	{
		OBJECT(current_object)->ice_platform_leader.travel = 0;
		OBJECT(current_object)->speed_x = 0u - OBJECT(current_object)->speed_x;
		sound_id = 0xFu;
		play_sound(sound_id, &r1, &write_index_address);
	}
}

void ice_platform_piece_update(void)
{
	uint16_t margin = 0;
	uint32_t r1 = 0, depth = 0, ignored = 0, object = 0, off_screen = 0;

	switch (OBJECT(current_object)->ice_platform_piece.state)
	{
	case 0:
	case 3:
		OBJECT(current_object)->ice_platform_piece.state++;
		OBJECT(current_object)->speed_z = 0;
		project_object(&ignored, &ignored);
		if (OBJECT(current_object)->ice_platform_piece.member == 3u)
			OBJECT(current_object)->part_list = ice_platform_piece_parts_2_ADDRESS;
		return;
	case 1:
		margin = 0x60u;
		off_screen = check_left_screen_margin(margin);
		if (off_screen != 0u)
		{
			delete_current_object();
			return;
		}
		if (OBJECT(current_object)->ice_platform_piece.icicle_landed != 0u)
		{
			OBJECT(current_object)->ice_platform_piece.state++;
			OBJECT(current_object)->part_list = ice_platform_piece_parts_2_ADDRESS;
		}
		object = LINKED(current_object, OBJECT(current_object)->parent);
		OBJECT(current_object)->x = OBJECT(object)->x;
		OBJECT(current_object)->y = OBJECT(object)->y;
		OBJECT(current_object)->x_int -= OBJECT(current_object)->ice_platform_piece.member * 0x2Du;
		add_to_platform_list();
		project_object(&ignored, &ignored);
		depth = (uint16_t)(OBJECT(current_object)->screen_y + OBJECT(current_object)->ice_platform_piece.depth_offset - OBJECT(current_object)->sprite_offset_y);
		OBJECT(current_object)->ice_platform_piece.draw_depth = depth;
		queue_sprite_at_depth(&ignored, &depth);
		return;
	case 2:
		off_screen = is_off_screen();
		if (off_screen == 0u)
		{
			OBJECT(current_object)->speed_z += 0x6000u;
			OBJECT(current_object)->z += OBJECT(current_object)->speed_z;
			update_screen_position(&ignored, &r1);
			depth = OBJECT(current_object)->ice_platform_piece.draw_depth;
			queue_sprite_at_depth(&ignored, &depth);
			return;
		}
		delete_current_object();
		return;
	default:
		unknown_code(0x87FB3u + OBJECT(current_object)->ice_platform_piece.state * 3u);
	}
}

void ice_floor_start(void)
{
	uint32_t cubes_left = 0, ignored = 0, object = 0, rows_left = 0;

	if (free_object_stack < 9)
		return;
	ice_row_start_x = OBJECT(current_object)->x;
	ice_cube_floor_block = ice_floor_floors[OBJECT(current_object)->sprite_frame];
	rows_left = 5u;
	do
	{
		cubes_left = 7u;
		do
		{
			allocate_object(&ignored, &object);
			OBJECT(object)->sprite_flags = 0x100Au;
			OBJECT(object)->sprite_frame = 0x7490u;
			OBJECT(object)->sprite_palette = 0x9120u;
			OBJECT(object)->handler = ice_floor_block_update_ADDRESS;
			OBJECT(object)->flags = 0u;
			OBJECT(object)->hitbox_size_x = 0x30u;
			OBJECT(object)->hitbox_size_y = 0x30u;
			OBJECT(object)->hitbox_height = 0x30u;
			OBJECT(object)->sprite_offset_x = 2;
			OBJECT(object)->sprite_offset_y = -0x10;
			OBJECT(object)->hitbox_offset_x = 0;
			OBJECT(object)->hitbox_offset_y = 0;
			OBJECT(object)->speed_z = 0;
			OBJECT(object)->ice_floor_block.state = 0u;
			OBJECT(object)->floor_block = ice_cube_floor_block;
			if ((uint16_t)cubes_left == 8u || (uint16_t)cubes_left == 7u)
				OBJECT(object)->sprite_palette |= 0x2000u;
			OBJECT(object)->x = OBJECT(current_object)->x;
			OBJECT(object)->y = OBJECT(current_object)->y;
			OBJECT(current_object)->x_int += 0x2Eu;
			OBJECT(object)->floor_z_int = mem_read16(floor_blocks + 2u + OBJECT(object)->floor_block + 0x10u);
			OBJECT(object)->floor_z_fraction = 0u;
			OBJECT(object)->z = OBJECT(object)->floor_z;
			cubes_left--;
		} while (cubes_left != 0u);
		OBJECT(current_object)->y_int += 0x2Eu;
		OBJECT(current_object)->x = ice_row_start_x;
		rows_left--;
	} while (rows_left != 0u);
	delete_current_object();
}

void ice_floor_block_update(void)
{
	uint8_t sound_id = 0;
	uint32_t r1 = 0, depth = 0, ignored = 0, object = 0, off_screen = 0, write_index_address = 0;
	uint8_t shake_timer = 0;
	uint16_t shake_duration = 0;

	switch (OBJECT(current_object)->ice_floor_block.state)
	{
	case 0:
	case 4:
		OBJECT(current_object)->ice_floor_block.state++;
		project_object(&ignored, &ignored);
		return;
	case 1:
		off_screen = check_left_screen();
		if (off_screen != 0u)
		{
			delete_current_object();
			return;
		}
		if ((OBJECT(current_object)->flags & 8u) != 0u)
		{
			object = LINKED(current_object, OBJECT(current_object)->platform);
			if ((int8_t)OBJECT(object)->kind <= 2)
			{
				OBJECT(current_object)->ice_floor_block.state++;
				OBJECT(current_object)->ice_floor_block.shake_offset = 1;
				OBJECT(current_object)->ice_floor_block.shake_timer = 0u;
				OBJECT(current_object)->ice_floor_block.shake_duration = 0x1E;
				sound_id = 0x14u;
				play_sound(sound_id, &r1, &write_index_address);
				if (PLAYER(PLAYER_BLOCK(1) + ((uint32_t)OBJECT(object)->player << 8) * 2u)->invulnerable_timer > 0)
					OBJECT(current_object)->ice_floor_block.shake_duration = 0x12C;
			}
		}
		add_to_platform_list();
		update_screen_position(&ignored, &r1);
		depth = (uint16_t)OBJECT(current_object)->screen_y;
		OBJECT(current_object)->ice_floor_block.draw_depth = depth;
		queue_sprite_at_depth(&ignored, &depth);
		return;
	case 2:
		shake_duration = OBJECT(current_object)->ice_floor_block.shake_duration;
		OBJECT(current_object)->ice_floor_block.shake_duration = shake_duration - 1u;
		if ((int16_t)shake_duration <= 1)
		{
			OBJECT(current_object)->ice_floor_block.state++;
			OBJECT(current_object)->sprite_palette |= 0x2000u;
			sound_id = 0x2Au;
			play_sound(sound_id, &r1, &write_index_address);
		}
		shake_timer = OBJECT(current_object)->ice_floor_block.shake_timer;
		OBJECT(current_object)->ice_floor_block.shake_timer = shake_timer - 1u;
		if ((int8_t)shake_timer <= 1)
		{
			OBJECT(current_object)->ice_floor_block.shake_timer = 2u;
			OBJECT(current_object)->z_int += OBJECT(current_object)->ice_floor_block.shake_offset;
			OBJECT(current_object)->ice_floor_block.shake_offset = 0u - OBJECT(current_object)->ice_floor_block.shake_offset;
		}
		add_to_platform_list();
		update_screen_position(&ignored, &r1);
		depth = (uint16_t)OBJECT(current_object)->screen_y;
		OBJECT(current_object)->ice_floor_block.draw_depth = depth;
		queue_sprite_at_depth(&ignored, &depth);
		return;
	case 3:
		off_screen = is_off_screen();
		if (off_screen != 0u)
		{
			delete_current_object();
			return;
		}
		OBJECT(current_object)->speed_z += 0x6000u;
		OBJECT(current_object)->z += OBJECT(current_object)->speed_z;
		update_screen_position(&ignored, &r1);
		depth = (uint16_t)(((int32_t)((uint32_t)OBJECT(current_object)->projected_y + OBJECT(current_object)->floor_z) >> 16) - view_y + OBJECT(current_object)->sprite_offset_y);
		OBJECT(current_object)->ice_floor_block.draw_depth = depth;
		queue_sprite_at_depth(&ignored, &depth);
		return;
	default:
		unknown_code(0x8816Cu + OBJECT(current_object)->ice_floor_block.state * 3u);
	}
}

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
