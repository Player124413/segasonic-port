/**
 *    Name:        objects/push_crate.c
 *    Description: object types 12 and 13: the crate pushed to a post (stages 3 and 7), a sign from the type list
 *    Version:     0.1.0
 *    Created:     12-10-2026
 *    Modified:    15-10-2026
 *    Author:      Sebastian Toyos
 *
 *    SegaSonic The Hedgehog (arcade, Sega System 32): the game's V60 program decompiled into C by
 *    tools/decompile.py, with the names of symbols.txt.
 */
#include "game.h"

void push_crate_init(void)
{
	uint16_t post_distance = 0;
	uint32_t block = 0, free_count = 0, object = 0, piece_count = 0, spawned = 0;

	OBJECT(current_object)->sprite_frame = 0x6968u;
	OBJECT(current_object)->sprite_palette = 0x9D0u;
	OBJECT(current_object)->flags |= 2u;
	OBJECT(current_object)->hitbox_size_x = 0x40u;
	OBJECT(current_object)->hitbox_size_y = 0x4Bu;
	OBJECT(current_object)->hitbox_offset_x = -9;
	OBJECT(current_object)->hitbox_offset_y = -0x42;
	OBJECT(current_object)->hitbox_height = 0x20u;
	OBJECT(current_object)->kind = 0x20u;
	OBJECT(current_object)->push_crate.frame_count = 0x12Cu;
	OBJECT(current_object)->handler = push_crate_update_ADDRESS;
	spawn_count = 2u;
	spawn_object_with_pieces(0x011203u, &spawned, &piece_count, &free_count, &object, &block);
	OBJECT(object_cursor)->child = object;
	copy_position_to(object);
	post_distance = OBJECT(object_cursor)->push_crate.post_distance;
	OBJECT(object)->projected_x -= post_distance * 0x22u + 0x11u;
	OBJECT(object)->projected_y_int -= post_distance * 0x11u + 0x1Du;
	OBJECT(object)->sprite_frame = 0x73B0u;
	OBJECT(object)->sprite_palette = 0x89E0u;
	OBJECT(object)->handler = push_crate_stop_post_update_ADDRESS;
	object = LINKED(object, OBJECT(object)->child);
	OBJECT(object)->sprite_frame = 0x73B0u;
	OBJECT(object)->sprite_palette = 0x89E0u;
}

void push_crate_update(void)
{
	uint32_t direction = 0, distance_y = 0, ignored = 0, object = 0, speed = 0, speed_x = 0, speed_y = 0;

	r14 = LINKED(WORK_RAM, chaser_object);
	distance_xy_to_object(&ignored, &distance_y);
	if ((int16_t)distance_y <= 3)
	{
		OBJECT(current_object)->speed_x = 0;
		OBJECT(current_object)->speed_y = 0;
		OBJECT(current_object)->handler = push_crate_break_ADDRESS;
		push_crate_break();
		return;
	}
	if ((OBJECT(current_object)->flags & 4u) == 0u)
	{
		push_crate_move(0u, 0u, 0u);
		return;
	}
	object = LINKED(WORK_RAM, OBJECT(current_object)->touched_by);
	if ((OBJECT(object)->flags & 0x40u) != 0u)
	{
		OBJECT(current_object)->speed_x = 0;
		OBJECT(current_object)->speed_y = 0;
		OBJECT(current_object)->handler = push_crate_break_ADDRESS;
		push_crate_break();
		return;
	}
	if (OBJECT(current_object)->push_crate.stopped != 0u || OBJECT(current_object)->push_crate.push_quadrant != (uint16_t)((int16_t)(OBJECT(object)->direction + 0x40u) / 0x40 - 1u))
	{
		push_crate_move(0u, 0u, 0u);
		return;
	}
	direction = push_crate_push_angles[OBJECT(current_object)->push_crate.push_quadrant];
	speed = (uint32_t)OBJECT(object)->speed >> 16;
	direction_to_speed(speed, &direction, &speed_x, &speed_y);
	OBJECT(current_object)->speed_x = speed_x;
	OBJECT(current_object)->speed_y = speed_y;
	push_crate_move(0u, 0u, 0u);
}

void push_crate_break(void)
{
	uint32_t r11 = 0, r16 = 0, r17 = 0, break_frame = 0;
	uint16_t odd_frame = 0;

	OBJECT(current_object)->push_crate.frame_count++;
	odd_frame = OBJECT(current_object)->push_crate.frame_count & 1u;
	OBJECT(current_object)->push_crate.frame_count = odd_frame;
	if (odd_frame != 0u)
	{
		push_crate_move(0u, 0u, 0u);
		return;
	}
	OBJECT(current_object)->push_crate.break_frame++;
	break_frame = OBJECT(current_object)->push_crate.break_frame;
	OBJECT(current_object)->sprite_frame = push_crate_break_sprites[break_frame];
	if (break_frame != 2u)
	{
		push_crate_move(0u, 0u, 0u);
		return;
	}
	OBJECT(current_object)->handler = push_crate_move_ADDRESS;
	if (OBJECT(current_object)->push_crate.ring_count <= 0)
	{
		push_crate_move(0u, 0u, 0u);
		return;
	}
	push_crate_release_all_rings(&r11, &r16, &r17);
	push_crate_move(r11, r16, r17);
}

void push_crate_move(uint32_t cursor, uint32_t random_value, uint32_t ring)
{
	uint32_t r3 = 0, block_offset = 0, deleted = 0, depth = 0, ignored = 0;
	uint16_t ring_delay = 0;

	move_object_projected(&ignored, &block_offset, &r3, &cursor);
	delete_if_off_screen(&deleted, &block_offset, &r3);
	if ((uint8_t)deleted == 1u)
		return;
	if (OBJECT(current_object)->push_crate.break_frame != 2u)
	{
		OBJECT(current_object)->health = 0x7F;
		add_to_touchable_list(deleted);
		depth = (uint16_t)(OBJECT(current_object)->screen_y - 0xDu);
		queue_sprite_at_depth(&ignored, &depth);
		if ((uint32_t)OBJECT(current_object)->speed_x != 0u)
		{
			ring_delay = OBJECT(current_object)->push_crate.ring_timer - 1u;
			OBJECT(current_object)->push_crate.ring_timer = ring_delay;
			if ((int16_t)ring_delay < 0 && OBJECT(current_object)->push_crate.ring_count > 0)
			{
				OBJECT(current_object)->push_crate.ring_count--;
				OBJECT(current_object)->push_crate.ring_timer = 0x10;
				rings_to_release = 1u;
				push_crate_release_rings(r3, &cursor, &random_value, &ring);
			}
		}
	}
	else
	{
		deleted = (uint16_t)(deleted + 0xFFu);
		queue_sprite_in_bucket(deleted);
	}
}

void push_crate_stop_post_update(void)
{
	uint16_t offset_x = 0, offset_y = 0;
	uint32_t r1 = 0, cursor = 0, depth = 0, ignored = 0;

	unproject_object(&ignored, &ignored, &ignored, &cursor);
	r14 = LINKED(WORK_RAM, OBJECT(current_object)->parent);
	distance_xy_to_object(&ignored, &r1);
	if (distance_to_player_2 <= 0x33)
	{
		OBJECT(r14)->var_81 = 1u;
		OBJECT(r14)->speed_x = 0;
		OBJECT(r14)->speed_y = 0;
	}
	queue_sprite(r1);
	current_object = LINKED(current_object, OBJECT(current_object)->child);
	offset_x = 0u;
	offset_y = 0xFFF1u;
	place_sprite_near_cursor(offset_x, offset_y);
	depth = (uint16_t)(OBJECT(current_object)->screen_y + 0x11u);
	queue_sprite_at_depth(&ignored, &depth);
}

void push_crate_release_all_rings(uint32_t *p_allocated, uint32_t *p_random_value, uint32_t *p_ring)
{
	uint32_t allocated = *p_allocated;
	uint32_t random_value = *p_random_value;
	uint32_t ring = *p_ring;

	rings_to_release = OBJECT(current_object)->push_crate.ring_count;
	push_crate_release_rings(0u, &allocated, &random_value, &ring);
	*p_allocated = allocated;
	*p_random_value = random_value;
	*p_ring = ring;
}

void push_crate_release_rings(uint32_t seed, uint32_t *p_allocated, uint32_t *p_random_value, uint32_t *p_ring)
{
	uint32_t allocated = *p_allocated;
	uint32_t random_value = *p_random_value;
	uint32_t ring = *p_ring;
	uint32_t opposite_quadrant = 0;

	opposite_quadrant = OBJECT(current_object)->push_crate.push_quadrant;
	opposite_quadrant = (uint8_t)(opposite_quadrant + 2u);
	released_ring_angle = push_crate_push_angles[opposite_quadrant & 3u];
	released_ring_flip = 0u;
	released_rings_random = 0u;
	drop_rings(seed, &allocated, &random_value, &ring);
	*p_allocated = allocated;
	*p_random_value = random_value;
	*p_ring = ring;
}

void sign_start_from_type(void)
{
	uint16_t spawn_arg_1 = 0, x_int = 0, y_int = 0, z_int = 0;
	uint32_t cursor = 0, ignored = 0, object = 0;

	unproject_object(&ignored, &ignored, &ignored, &cursor);
	x_int = (uint16_t)OBJECT(current_object)->x_int;
	y_int = (uint16_t)OBJECT(current_object)->y_int;
	spawn_arg_1 = OBJECT(current_object)->spawn_arg_1;
	z_int = (uint16_t)OBJECT(current_object)->z_int;
	object = current_object;
	place_sign(x_int, y_int, spawn_arg_1, z_int, &ignored, &object);
	OBJECT(object)->speed_x_fraction = OBJECT(current_object)->spawn_arg_2;
}
