/**
 *    Name:        objects/centipede.c
 *    Description: object type 20 (stage A): the centipede, its segments and sand puffs
 *    Version:     0.1.0
 *    Created:     12-10-2026
 *    Modified:    15-10-2026
 *    Author:      Sebastian Toyos
 *
 *    SegaSonic The Hedgehog (arcade, Sega System 32): the game's V60 program decompiled into C by
 *    tools/decompile.py, with the names of symbols.txt.
 */
#include "game.h"

void centipede_init(void)
{
	uint16_t follow_delay = 0;
	uint32_t block = 0, free_count = 0, object = 0, piece_count = 0, previous_segment = 0, spawned = 0;

	OBJECT(current_object)->hitbox_size_x = 0x10u;
	OBJECT(current_object)->hitbox_height = 0x32u;
	OBJECT(current_object)->kind = 0x24u;
	OBJECT(current_object)->sprite_frame = 0x7940u;
	OBJECT(current_object)->sprite_palette = 0x9270u;
	current_object = LINKED(current_object, OBJECT(current_object)->child);
	OBJECT(current_object)->sprite_frame = 0x5D80u;
	OBJECT(current_object)->sprite_palette = 0x87A0u;
	r14 = 0xAu;
	follow_delay = 0u;
	do
	{
		previous_segment = current_object;
		spawn_count = 2u;
		spawn_object_with_pieces(0x012351u, &spawned, &piece_count, &free_count, &object, &block);
		copy_position_to(object);
		OBJECT(object)->sprite_frame = 0x7980u;
		OBJECT(object)->sprite_palette = 0x9270u;
		OBJECT(object)->handler = centipede_segment_wait_ADDRESS;
		follow_delay += 5u;
		OBJECT(object)->centipede_segment.path_index = follow_delay;
		current_object = object;
		object = LINKED(object, OBJECT(object)->child);
		OBJECT(object)->sprite_frame = 0x5D80u;
		OBJECT(object)->sprite_palette = 0x87A0u;
		OBJECT(object)->sprite_scale = 0xA0u;
		r14 = r14 - 1u;
	} while (r14 != 0u);
	AS32(OBJECT(current_object)->spawn_arg_1) = previous_segment;
	OBJECT(current_object)->sprite_scale = 0x100u;
	OBJECT(current_object)->sprite_frame = 0x7A08u;
	OBJECT(current_object)->sprite_palette = 0x9270u;
	OBJECT(current_object)->handler = centipede_tail_wait_ADDRESS;
	current_object = object_cursor;
	OBJECT(current_object)->handler = centipede_chase_ADDRESS;
	centipede_command = 0u;
	add_to_rider_list();
}

void centipede_chase(void)
{
	uint32_t r0 = 0, r2 = 0, r3 = 0, r11 = 0, path_cursor = 0;
	uint32_t path_write_index = 0;

	move_object_projected(&r0, &r2, &r3, &r11);
	if (scroll_position <= OBJECT(current_object)->spawn_arg_1)
	{
		if ((int8_t)active_player_count > 0)
		{
			nearest_player_distance = 0;
			centipede_command = 0u;
			if (OBJECT(current_object)->speed_z > 0)
			{
				centipede_move(r11);
				return;
			}
			r0 = add_to_touchable_list(r0);
			face_nearest_player(r0, r3);
			if (nearest_player_floor_z < 0x800)
			{
				centipede_move(r11);
				return;
			}
			OBJECT(current_object)->anchor_y = 0;
			centipede_move(r11);
			return;
		}
		path_write_index = (uint8_t)OBJECT(current_object)->centipede.path_write_index;
		path_cursor = path_write_index - 1u;
		if (path_write_index == 0u)
			path_cursor = 0x32u;
		path_cursor = path_cursor * 4u + centipede_path_ADDRESS;
		OBJECT(current_object)->projected_x = mem_read16(path_cursor);
		OBJECT(current_object)->projected_y_int = mem_read16(path_cursor + 2u);
		path_cursor = (uint8_t)OBJECT(current_object)->centipede.path_write_index * 4u + centipede_path_ADDRESS;
		mem_write16(path_cursor, OBJECT(current_object)->projected_x);
		path_cursor += 2u;
		mem_write16(path_cursor, OBJECT(current_object)->projected_y_int);
		OBJECT(current_object)->speed_x = 0;
		OBJECT(current_object)->speed_y = 0;
		OBJECT(current_object)->anchor_y = 0x280;
		centipede_command = 1u;
		path_cursor += 2u;
		centipede_hop(&path_cursor, &r2);
		if (OBJECT(current_object)->centipede.hop_timer == 0x5Au)
			spawn_centipede_sand_puff(path_cursor, r2, r3, r11);
		draw_centipede_piece();
		return;
	}
	OBJECT(current_object)->handler = centipede_leave_ADDRESS;
	nearest_player_distance = 0;
	centipede_command = 0u;
	if (OBJECT(current_object)->speed_z > 0)
	{
		centipede_move(r11);
		return;
	}
	r0 = add_to_touchable_list(r0);
	face_nearest_player(r0, r3);
	if (nearest_player_floor_z < 0x800)
	{
		centipede_move(r11);
		return;
	}
	OBJECT(current_object)->anchor_y = 0;
	centipede_move(r11);
}

void centipede_leave(void)
{
	uint32_t r11 = 0, block_offset = 0, deleted = 0, ignored = 0, slope_height = 0;

	move_object_projected(&ignored, &block_offset, &slope_height, &r11);
	delete_if_off_screen(&deleted, &block_offset, &slope_height);
	if ((uint8_t)deleted == 1u)
	{
		centipede_command = 9u;
		return;
	}
	centipede_command = 0u;
	OBJECT(current_object)->direction = OBJECT(current_object)->spawn_arg_2;
	nearest_player_distance = 0x1F;
	nearest_player_dx = 0;
	centipede_move(r11);
}

void centipede_move(uint32_t block)
{
	uint16_t saved_direction = 0;
	uint32_t r0 = 0, chase_speed = 0, direction = 0, head_frame = 0, speed = 0, speed_x = 0, speed_y = 0, wobble_speed_x = 0, wobble_speed_y = 0;

	head_frame = OBJECT(current_object)->direction;
	saved_direction = (uint16_t)head_frame;
	head_frame = (uint16_t)((int16_t)head_frame / 0x92);
	head_frame = (uint16_t)(head_frame + 1u);
	OBJECT(current_object)->sprite_frame = centipede_head_frames[head_frame & 7u];
	direction = saved_direction;
	chase_speed = (uint16_t)nearest_player_distance * 2u;
	chase_speed = (chase_speed & ~0xFFFFu) | (chase_speed & 0x3FFu);
	chase_speed = (chase_speed & ~0xFFFFu) | (uint16_t)(chase_speed + 0x100u);
	r14 = chase_speed;
	chase_speed = chase_speed * 2u;
	direction_to_speed(chase_speed, &direction, &speed_x, &speed_y);
	OBJECT(current_object)->speed_x = speed_x;
	OBJECT(current_object)->speed_y = speed_y;
	OBJECT(current_object)->centipede.wobble_angle -= 0x10u;
	OBJECT(current_object)->centipede.wobble_angle &= 0x3FFu;
	r0 = OBJECT(current_object)->centipede.wobble_angle;
	speed = r14;
	direction_to_speed(speed, &r0, &wobble_speed_x, &wobble_speed_y);
	OBJECT(current_object)->speed_x += wobble_speed_x;
	OBJECT(current_object)->speed_y += wobble_speed_y;
	if (centipede_command != 1u)
	{
		r0 = (uint8_t)OBJECT(current_object)->centipede.path_write_index * 4u + centipede_path_ADDRESS;
		mem_write16(r0, OBJECT(current_object)->projected_x);
		r0 += 2u;
		mem_write16(r0, OBJECT(current_object)->projected_y_int);
		OBJECT(current_object)->centipede.path_write_index++;
		r0 += 2u;
		if (OBJECT(current_object)->centipede.path_write_index > 0x32)
			OBJECT(current_object)->centipede.path_write_index = 0;
	}
	centipede_hop(&r0, &wobble_speed_x);
	if (OBJECT(current_object)->centipede.hop_timer == 0x5Au)
		spawn_centipede_sand_puff(r0, wobble_speed_x, wobble_speed_y, block);
	draw_centipede_piece();
}

void centipede_segment_wait(void)
{
	uint32_t r0 = 0, r2 = 0;

	OBJECT(current_object)->centipede_segment.path_index--;
	if (OBJECT(current_object)->centipede_segment.path_index <= 0)
	{
		OBJECT(current_object)->handler = centipede_segment_follow_ADDRESS;
		centipede_segment_follow();
		return;
	}
	centipede_hop(&r0, &r2);
	if (OBJECT(current_object)->centipede_segment.hop_timer == 0x5Au)
		spawn_centipede_sand_puff(r0, r2, 0u, 0u);
	draw_centipede_piece();
}

void centipede_segment_follow(void)
{
	uint32_t r0 = 0, r2 = 0, r3 = 0, r11 = 0, ignored = 0;

	move_object_projected(&r0, &r2, &r3, &r11);
	if (centipede_command == 9u)
	{
		delete_object_and_children(&ignored, &r2, &r3);
		return;
	}
	if (centipede_command != 1u)
		r0 = centipede_follow_path();
	centipede_hop(&r0, &r2);
	if (OBJECT(current_object)->centipede_segment.hop_timer == 0x5Au)
		spawn_centipede_sand_puff(r0, r2, r3, r11);
	draw_centipede_piece();
}

void centipede_tail_wait(void)
{
	uint32_t r0 = 0, r2 = 0;

	OBJECT(current_object)->centipede_tail.path_index--;
	if (OBJECT(current_object)->centipede_tail.path_index <= 0)
	{
		OBJECT(current_object)->handler = centipede_tail_follow_ADDRESS;
		centipede_tail_follow();
		return;
	}
	centipede_hop(&r0, &r2);
	if (OBJECT(current_object)->centipede_tail.hop_timer == 0x5Au)
		spawn_centipede_sand_puff(r0, r2, 0u, 0u);
	draw_centipede_piece();
}

void centipede_tail_follow(void)
{
	uint32_t r0 = 0, r2 = 0, r3 = 0, r11 = 0, ignored = 0, tail_frame = 0;

	move_object_projected(&r0, &r2, &r3, &r11);
	if (centipede_command == 9u)
	{
		delete_object_and_children(&ignored, &r2, &r3);
		return;
	}
	if (centipede_command != 1u)
	{
		r14 = OBJECT(current_object)->centipede_tail.segment_ahead;
		r3 = direction_to_object();
		tail_frame = OBJECT(current_object)->direction;
		tail_frame = (uint16_t)((int16_t)tail_frame / 0x92);
		tail_frame = (uint16_t)(tail_frame + 1u);
		tail_frame = (uint16_t)(tail_frame + 4u);
		OBJECT(current_object)->sprite_frame = centipede_tail_frames[tail_frame & 7u];
		r0 = centipede_follow_path();
	}
	centipede_hop(&r0, &r2);
	if (OBJECT(current_object)->centipede_tail.hop_timer == 0x5Au)
		spawn_centipede_sand_puff(r0, r2, r3, r11);
	draw_centipede_piece();
}

uint32_t centipede_follow_path(void)
{
	uint32_t path_cursor = 0;

	path_cursor = (uint16_t)OBJECT(current_object)->centipede_follow_path.path_index * 4u + centipede_path_ADDRESS;
	OBJECT(current_object)->projected_x = mem_read16(path_cursor);
	path_cursor += 2u;
	OBJECT(current_object)->projected_y_int = mem_read16(path_cursor);
	OBJECT(current_object)->centipede_follow_path.path_index++;
	path_cursor += 2u;
	if (OBJECT(current_object)->centipede_follow_path.path_index > 0x32)
		OBJECT(current_object)->centipede_follow_path.path_index = 0;
	return path_cursor;
}

void centipede_hop(uint32_t *p_height, uint32_t *p_z)
{
	uint32_t height = *p_height;
	uint32_t z = *p_z;
	uint32_t r1 = 0, fall_height = 0;

	if (OBJECT(current_object)->centipede.hop_timer == 0u)
	{
		fall_to_floor(&fall_height, &r1, &z);
		OBJECT(current_object)->speed_z -= 0x4000u;
		if (OBJECT(current_object)->z < 8)
		{
			*p_height = fall_height;
			*p_z = z;
			return;
		}
		OBJECT(current_object)->speed_z = 0;
		OBJECT(current_object)->centipede.hop_timer = 0x5Au;
		*p_height = fall_height;
		*p_z = z;
		return;
	}
	OBJECT(current_object)->centipede.hop_timer--;
	OBJECT(current_object)->speed_z -= 0x600u;
}

void draw_centipede_piece(void)
{
	uint32_t depth = 0, ignored = 0;

	queue_sprite_at_floor();
	add_to_rider_list();
	current_object = LINKED(current_object, OBJECT(current_object)->child);
	place_sprite_at_cursor_floor();
	depth = (uint16_t)(OBJECT(current_object)->screen_y - 0xAu);
	queue_sprite_at_depth(&ignored, &depth);
}

void spawn_centipede_sand_puff(uint32_t spawned, uint32_t free_count, uint32_t puff, uint32_t block)
{
	uint32_t piece_count = 0;

	spawn_count = 1u;
	spawn_object_with_pieces(0x012698u, &spawned, &piece_count, &free_count, &puff, &block);
	if ((uint8_t)spawned == 0u)
		return;
	copy_position_to(puff);
	OBJECT(puff)->projected_y_int += 6u;
	OBJECT(puff)->sand_puff.lifetime = 0x2Du;
	OBJECT(puff)->sprite_frame = 0x75A8u;
	OBJECT(puff)->sprite_palette = 0x1020u;
	OBJECT(puff)->sprite_scale = 0x180u;
	OBJECT(puff)->handler = sand_puff_start_ADDRESS;
}

void sand_puff_start(void)
{
	uint32_t cursor = 0, ignored = 0;

	unproject_object(&ignored, &ignored, &ignored, &cursor);
	add_to_rider_list();
	OBJECT(current_object)->handler = sand_puff_animate_ADDRESS;
}

void sand_puff_animate(void)
{
	uint32_t block_offset = 0, cursor = 0, ignored = 0, next_frame = 0, slope_height = 0;
	uint16_t lifetime = 0;

	unproject_object(&ignored, &block_offset, &slope_height, &cursor);
	if (OBJECT(current_object)->floor_z_int >= 0x800)
	{
		delete_object_and_children(&ignored, &block_offset, &slope_height);
		return;
	}
	lifetime = OBJECT(current_object)->sand_puff.lifetime;
	OBJECT(current_object)->sand_puff.lifetime = lifetime - 1u;
	if (lifetime == 0u)
	{
		delete_object_and_children(&ignored, &block_offset, &slope_height);
		return;
	}
	next_frame = (uint16_t)OBJECT(current_object)->sand_puff.frame_index;
	next_frame = (uint16_t)(next_frame + 1u);
	if ((int16_t)next_frame > 2)
		next_frame = 0u;
	OBJECT(current_object)->sand_puff.frame_index = next_frame;
	OBJECT(current_object)->sprite_frame = sand_puff_frames[next_frame];
	queue_sprite(0u);
	add_to_rider_list();
}
