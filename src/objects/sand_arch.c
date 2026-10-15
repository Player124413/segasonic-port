/**
 *    Name:        objects/sand_arch.c
 *    Description: object type 41 (stage A): the arch of sand balls
 *    Version:     0.1.0
 *    Created:     12-10-2026
 *    Modified:    15-10-2026
 *    Author:      Sebastian Toyos
 *
 *    SegaSonic The Hedgehog (arcade, Sega System 32): the game's V60 program decompiled into C by
 *    tools/decompile.py, with the names of symbols.txt.
 */
#include "game.h"

void sand_arch_init(void)
{
	uint16_t ball_phase = 0, phase_spacing = 0;
	uint32_t balls_left = 0, block = 0, free_count = 0, object = 0, object_2 = 0, piece_count = 0, spawned = 0;

	OBJECT(current_object)->direction = OBJECT(current_object)->sand_arch.row_direction * 0x40u;
	OBJECT(current_object)->sprite_flags = 0u;
	OBJECT(current_object)->sprite_frame = 0xA1C0u;
	OBJECT(current_object)->sprite_palette = 0x9200u;
	OBJECT(current_object)->handler = sand_arch_update_ADDRESS;
	current_object = LINKED(current_object, OBJECT(current_object)->child);
	OBJECT(current_object)->sprite_flags = 0u;
	OBJECT(current_object)->sprite_frame = 0xA1C0u;
	OBJECT(current_object)->sprite_palette = 0x87A0u;
	object = current_object;
	balls_left = (uint8_t)OBJECT(object_cursor)->sand_arch.ball_count;
	current_player = (current_player & ~0xFFFFu) | 6u;
	ball_phase = 0u;
	phase_spacing = divide_s16(OBJECT(object_cursor)->spawn_arg_2, (uint16_t)(balls_left + 1u));
	do
	{
		spawn_count = 2u;
		spawn_object_with_pieces(0x01369Cu, &spawned, &piece_count, &free_count, &object_2, &block);
		OBJECT(object)->child = object_2;
		copy_position_to(object_2);
		OBJECT(object_2)->sprite_flags = 0u;
		OBJECT(object_2)->sprite_frame = 0xA1C0u;
		OBJECT(object_2)->sprite_palette = 0x9200u;
		OBJECT(object_2)->direction = OBJECT(object_cursor)->direction;
		OBJECT(object_2)->hitbox_size_x = 0x10u;
		OBJECT(object_2)->hitbox_height = 0x10u;
		OBJECT(object_2)->kind = 0x24u;
		OBJECT(object_2)->handler = sand_arch_ball_init_ADDRESS;
		ball_phase = (ball_phase - phase_spacing) & 0x3FFu;
		OBJECT(object_2)->sand_arch.bob_phase = ball_phase;
		OBJECT(object_2)->sand_arch.row_position = current_player;
		current_player = (current_player & ~0xFFFFu) | (uint16_t)(current_player + 6u);
		object_2 = LINKED(object_2, OBJECT(object_2)->child);
		OBJECT(object_2)->sprite_flags = 0u;
		OBJECT(object_2)->sprite_frame = 0xA1C0u;
		OBJECT(object_2)->sprite_palette = 0x87A0u;
		object = object_2;
		balls_left--;
	} while (balls_left != 0u);
}

void sand_arch_update(void)
{
	uint32_t block_offset = 0, bucket = 0, cursor = 0, ignored = 0, on_screen = 0, slope_height = 0;

	unproject_object(&ignored, &block_offset, &slope_height, &cursor);
	on_screen = is_on_screen();
	OBJECT(current_object)->sand_arch.row_position += on_screen;
	OBJECT(current_object)->sand_arch.row_position &= 1u;
	if (OBJECT(current_object)->sand_arch.balls_gone < OBJECT(current_object)->sand_arch.ball_count)
	{
		current_object = LINKED(current_object, OBJECT(current_object)->child);
		bucket = place_sprite_at_cursor_floor();
		queue_sprite_in_bucket(bucket);
		current_object = object_cursor;
		return;
	}
	delete_object_and_children(&ignored, &block_offset, &slope_height);
}

void sand_arch_ball_init(void)
{
	uint32_t ball_direction = 0, cursor = 0, ignored = 0, speed = 0, speed_x = 0, speed_y = 0;

	ball_direction = OBJECT(current_object)->direction;
	speed = 0x200u * OBJECT(current_object)->sand_arch.row_position;
	direction_to_speed(speed, &ball_direction, &speed_x, &speed_y);
	OBJECT(current_object)->speed_x = speed_x;
	OBJECT(current_object)->speed_y = speed_y;
	move_object_projected(&ignored, &ignored, &ignored, &cursor);
	OBJECT(current_object)->speed_x = 0;
	OBJECT(current_object)->speed_y = 0;
	OBJECT(current_object)->handler = sand_arch_ball_delay_ADDRESS;
	sand_arch_ball_delay();
}

void sand_arch_ball_delay(void)
{
	add_to_rider_list();
	OBJECT(current_object)->sand_arch.bob_phase += 2u;
	if (OBJECT(current_object)->sand_arch.bob_phase <= 0x3FF)
	{
		sand_arch_ball_gone();
		return;
	}
	OBJECT(current_object)->sand_arch.bob_phase &= 0x3FFu;
	OBJECT(current_object)->handler = sand_arch_ball_move_ADDRESS;
}

void sand_arch_ball_move(void)
{
	uint16_t below_floor = 0, offset_x = 0, offset_y = 0;
	uint32_t r1 = 0, r2 = 0, cursor = 0, depth = 0, ignored = 0;
	uint16_t direction = 0;

	move_object_projected(&ignored, &r2, &ignored, &cursor);
	add_to_touchable_list(ignored);
	add_to_rider_list();
	if (OBJECT(current_object)->floor_z_int >= 0x800)
	{
		if (OBJECT(current_object)->z_int >= 0x800)
		{
			current_object = LINKED(current_object, OBJECT(current_object)->parent);
			OBJECT(current_object)->var_80++;
			current_object = object_cursor;
			OBJECT(current_object)->handler = sand_arch_ball_gone_ADDRESS;
		}
		fall_to_floor(&ignored, &r1, &r2);
		goto sand_arch_ball_draw;
	}
	sand_arch_ball_bob();
	below_floor = (uint16_t)(OBJECT(current_object)->z_int - OBJECT(current_object)->floor_z_int);
	if ((int16_t)below_floor >= 0x10)
	{
		sand_arch_ball_gone();
		return;
	}
	if ((int16_t)below_floor < -8)
	{
	sand_arch_ball_draw:
		r1 = queue_sprite_at_floor();
	}
	if ((int16_t)(OBJECT(current_object)->floor_z_int - OBJECT(current_object)->z_int) <= 0x10)
	{
		current_object = LINKED(current_object, OBJECT(current_object)->child);
		direction = OBJECT(current_object)->direction;
		OBJECT(current_object)->direction = direction - 1u;
		if (direction == 0u)
		{
			OBJECT(current_object)->direction = 2u;
			OBJECT(current_object)->sprite_frame = sand_arch_spray_frames[AT8(OBJECT(current_object)->projected_y, 1)];
			OBJECT(current_object)->sprite_palette = 0x9020u;
			AT8(OBJECT(current_object)->projected_y, 1)++;
			AT8(OBJECT(current_object)->projected_y, 1) = AT8(OBJECT(current_object)->projected_y, 1) % 4u;
		}
		offset_x = 0u;
		offset_y = 8u;
		place_sprite_near_cursor(offset_x, offset_y);
		OBJECT(current_object)->screen_y -= OBJECT(object_cursor)->z_int - OBJECT(object_cursor)->floor_z_int;
		queue_sprite(r1);
		sand_arch_ball_gone();
		return;
	}
	current_object = LINKED(current_object, OBJECT(current_object)->child);
	OBJECT(current_object)->sprite_frame = 0xA1C0u;
	OBJECT(current_object)->sprite_palette = 0x87A0u;
	offset_x = 0u;
	offset_y = (uint16_t)(OBJECT(object_cursor)->floor_z_int - OBJECT(object_cursor)->z_int);
	place_sprite_near_cursor(offset_x, offset_y);
	depth = (uint16_t)(OBJECT(current_object)->screen_y - 0x10u);
	queue_sprite_at_depth(&ignored, &depth);
	AT8(OBJECT(current_object)->projected_y, 1) = 0u;
	sand_arch_ball_gone();
}

void sand_arch_ball_gone(void)
{
}

void sand_arch_ball_bob(void)
{
	OBJECT(current_object)->sand_arch.bob_phase += 0xCu;
	OBJECT(current_object)->sand_arch.bob_phase &= 0x3FFu;
	OBJECT(current_object)->speed_z = 0u - sine_table[(uint16_t)OBJECT(current_object)->sand_arch.bob_phase] * 0xCu;
}
