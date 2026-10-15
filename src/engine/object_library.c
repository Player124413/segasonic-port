/**
 *    Name:        engine/object_library.c
 *    Description: what the stage objects share: moving and unprojecting, deleting off screen, positions, the nearest player, falling, explosions, dropped rings, debris
 *    Version:     0.1.0
 *    Created:     12-10-2026
 *    Modified:    15-10-2026
 *    Author:      Sebastian Toyos
 *
 *    SegaSonic The Hedgehog (arcade, Sega System 32): the game's V60 program decompiled into C by
 *    tools/decompile.py, with the names of symbols.txt.
 */
#include "game.h"

void move_object_projected(uint32_t *p_screen_y, uint32_t *p_block_offset, uint32_t *p_slope_height, uint32_t *p_cursor)
{
	uint32_t cursor = *p_cursor;
	uint32_t r2 = 0, screen_y = 0, slope_height = 0;

	OBJECT(current_object)->move_object_projected.projected_x_carry += OBJECT(current_object)->speed_x;
	OBJECT(current_object)->projected_x += OBJECT(current_object)->move_object_projected.projected_x_carry_int;
	OBJECT(current_object)->projected_y += OBJECT(current_object)->speed_y;
	OBJECT(current_object)->z += OBJECT(current_object)->speed_z;
	OBJECT(current_object)->move_object_projected.projected_x_carry_int = 0;
	unproject_object(&screen_y, &r2, &slope_height, &cursor);
	*p_screen_y = screen_y;
	*p_block_offset = r2;
	*p_slope_height = slope_height;
	*p_cursor = cursor;
}

void unproject_object(uint32_t *p_screen_y, uint32_t *p_block_offset, uint32_t *p_slope_height, uint32_t *p_cursor)
{
	uint32_t cursor = *p_cursor;
	uint32_t r2 = 0, scaled_x = 0, scaled_y = 0, screen_y = 0, slope_height = 0;

	scaled_x = (uint32_t)OBJECT(current_object)->projected_x << 8;
	scaled_y = (uint32_t)((int32_t)OBJECT(current_object)->projected_y >> 16) << 9;
	r2 = scaled_x;
	OBJECT(current_object)->x = (scaled_x - scaled_y) * 0xB5u;
	OBJECT(current_object)->y = (scaled_y + r2) * 0xB5u;
	find_floor(&scaled_x, &scaled_y, &r2, &slope_height, &cursor);
	screen_y = update_screen_position_no_offset();
	*p_screen_y = screen_y;
	*p_block_offset = r2;
	*p_slope_height = slope_height;
	*p_cursor = cursor;
}

uint32_t update_screen_position_no_offset(void)
{
	uint32_t unscrolled_y = 0;

	OBJECT(current_object)->screen_x = OBJECT(current_object)->projected_x - view_x;
	unscrolled_y = (int32_t)((uint32_t)OBJECT(current_object)->projected_y + OBJECT(current_object)->z) >> 16;
	unscrolled_y = (unscrolled_y & ~0xFFFFu) | (uint16_t)(unscrolled_y - view_y);
	OBJECT(current_object)->screen_y = unscrolled_y;
	return unscrolled_y;
}

void move_object_projected_no_floor(void)
{
	OBJECT(current_object)->move_object_projected.projected_x_carry += OBJECT(current_object)->speed_x;
	OBJECT(current_object)->projected_x += OBJECT(current_object)->move_object_projected.projected_x_carry_int;
	OBJECT(current_object)->projected_y += OBJECT(current_object)->speed_y;
	OBJECT(current_object)->z += OBJECT(current_object)->speed_z;
	OBJECT(current_object)->move_object_projected.projected_x_carry_int = 0;
	OBJECT(current_object)->screen_x = OBJECT(current_object)->projected_x - view_x;
	OBJECT(current_object)->screen_y = ((int32_t)((uint32_t)OBJECT(current_object)->projected_y + OBJECT(current_object)->z) >> 16) - view_y;
}

uint32_t is_on_screen(void)
{
	uint16_t screen_x = 0, screen_y = 0;
	uint32_t on_screen = 0;

	screen_x = (uint16_t)OBJECT(current_object)->screen_x;
	screen_y = (uint16_t)OBJECT(current_object)->screen_y;
	on_screen = 0u;
	if ((int16_t)screen_x >= -0xC0 && (int16_t)screen_x <= 0x200 && (int16_t)screen_y >= -0xC0 && (int16_t)screen_y <= 0x1A0)
		on_screen = 1u;
	return on_screen;
}

void delete_if_off_screen(uint32_t *p_deleted, uint32_t *p_block_offset, uint32_t *p_slope_height)
{
	uint32_t block_offset = *p_block_offset;
	uint32_t slope_height = *p_slope_height;
	uint16_t screen_x = 0, screen_y = 0;
	uint32_t r0 = 0, deleted = 0;

	screen_x = (uint16_t)OBJECT(current_object)->screen_x;
	screen_y = (uint16_t)OBJECT(current_object)->screen_y;
	if ((int16_t)screen_x < -0xC0 || (int16_t)screen_x > 0x200 || (int16_t)screen_y < -0xC0 || (int16_t)screen_y > 0x1A0)
	{
		delete_object_and_children(&r0, &block_offset, &slope_height);
		*p_deleted = r0;
		*p_block_offset = block_offset;
		*p_slope_height = slope_height;
		return;
	}
	deleted = 0u;
	*p_deleted = deleted;
}

void delete_object_and_children(uint32_t *p_deleted, uint32_t *p_block_offset, uint32_t *p_slope_height)
{
	uint32_t block_offset = *p_block_offset;
	uint32_t slope_height = *p_slope_height;
	uint32_t r2 = 0, r3 = 0, child = 0, deleted = 0, free_blocks = 0, ignored = 0, object = 0;

	if (OBJECT(object_cursor)->child == 0xFFFFu)
	{
		delete_current_object();
		deleted = 1u;
		*p_deleted = deleted;
		return;
	}
	child = LINKED(WORK_RAM, OBJECT(object_cursor)->child);
	deleted_child = child;
	for (;;)
	{
		if ((int16_t)child < 0x5000)
		{
			delete_object(child, &ignored, &r2);
		delete_object_and_children_next_child:
			object = LINKED(WORK_RAM, deleted_child);
			child = LINKED(object, OBJECT(object)->child);
			if ((uint16_t)child == 0xFFFFu)
			{
				delete_current_object();
				deleted = 1u;
				*p_deleted = deleted;
				*p_block_offset = r2;
				return;
			}
			deleted_child = child;
			continue;
		}
		r2 = free_object_2((uint16_t)child);
		free_blocks = LINKED(WORK_RAM, free_object_stack_2[0]);
		if ((int16_t)free_blocks <= 0x100)
			goto delete_object_and_children_next_child;
		break;
	}
	int_default(&free_blocks, &r2, &r3, &ignored);
	*p_deleted = free_blocks;
	*p_block_offset = r2;
	*p_slope_height = r3;
}

void copy_position_to(uint32_t object)
{
	OBJECT(object)->x = OBJECT(object_cursor)->x;
	OBJECT(object)->y = OBJECT(object_cursor)->y;
	OBJECT(object)->projected_x = OBJECT(object_cursor)->projected_x;
	OBJECT(object)->projected_y = OBJECT(object_cursor)->projected_y;
	OBJECT(object)->z = OBJECT(object_cursor)->z;
	OBJECT(object)->floor_z = OBJECT(object_cursor)->floor_z;
}

void place_sprite_near_cursor(uint16_t offset_x, uint16_t offset_y)
{
	OBJECT(current_object)->screen_x = OBJECT(object_cursor)->screen_x;
	OBJECT(current_object)->screen_y = OBJECT(object_cursor)->screen_y;
	OBJECT(current_object)->screen_x += offset_x;
	OBJECT(current_object)->screen_y += offset_y;
}

void copy_position_to_cursor(void)
{
	OBJECT(object_cursor)->projected_x = OBJECT(current_object)->projected_x;
	OBJECT(object_cursor)->projected_y = OBJECT(current_object)->projected_y;
	OBJECT(object_cursor)->z = OBJECT(current_object)->z;
	OBJECT(object_cursor)->floor_z = OBJECT(current_object)->floor_z;
}

void copy_position_from_cursor(void)
{
	OBJECT(current_object)->x = OBJECT(object_cursor)->x;
	OBJECT(current_object)->y = OBJECT(object_cursor)->y;
	OBJECT(current_object)->projected_x = OBJECT(object_cursor)->projected_x;
	OBJECT(current_object)->projected_y = OBJECT(object_cursor)->projected_y;
	OBJECT(current_object)->z = OBJECT(object_cursor)->z;
	OBJECT(current_object)->floor_z = OBJECT(object_cursor)->floor_z;
}

void copy_screen_position_from_cursor(void)
{
	OBJECT(current_object)->screen_x = OBJECT(object_cursor)->screen_x;
	OBJECT(current_object)->screen_y = OBJECT(object_cursor)->screen_y;
}

uint32_t face_nearest_player(uint32_t direction, uint32_t octant)
{
	uint16_t dy = 0, player_dx = 0, player_dy = 0;
	uint32_t abs_dy = 0, distance = 0, ignored = 0, player_index = 0, player_object = 0;
	uint16_t diff_x = 0, diff_y = 0;
	uint32_t players_left = 0;

	nearest_player_distance = 0x7FFF;
	player_index = 3u;
	for (;;)
	{
		players_left = player_index;
		player_index = players_left - 1u;
		if (players_left == 0u)
			break;
		player_object = GAME_VARIABLES + player_block_offsets[player_index];
		player_object = LINKED(player_object, PLAYER(player_object)->object);
		if ((OBJECT(player_object)->flags & 0x400u) == 0u)
		{
			diff_x = OBJECT(player_object)->projected_x - OBJECT(current_object)->projected_x;
			distance = diff_x;
			player_dx = (uint16_t)distance;
			if ((int16_t)diff_x < 0)
				distance = (uint16_t)(0u - distance);
			diff_y = OBJECT(player_object)->projected_y_int - OBJECT(current_object)->projected_y_int;
			abs_dy = diff_y;
			player_dy = (uint16_t)abs_dy;
			if ((int16_t)diff_y < 0)
				abs_dy = (uint16_t)(0u - abs_dy);
			distance = (int16_t)distance;
			abs_dy = (int16_t)abs_dy;
			distance = distance * distance + abs_dy * abs_dy;
			square_root(&distance, &ignored);
			if ((int16_t)distance <= nearest_player_distance)
			{
				nearest_player = player_index;
				nearest_player_distance = distance;
				nearest_player_dx = player_dx;
				nearest_player_dy = player_dy;
			}
			RAM16(GAME_VARIABLES + distance_to_player_offsets[player_index]) = distance;
			direction = player_dx;
			dy = (uint16_t)player_dy;
			angle_of_vector(dy, &direction, &octant);
			RAM16(GAME_VARIABLES + direction_to_player_offsets[player_index]) = direction;
			continue;
		}
		RAM16(GAME_VARIABLES + distance_to_player_offsets[player_index]) = 0x7FFFu;
		RAM16(GAME_VARIABLES + direction_to_player_offsets[player_index]) = 0x7FFFu;
	}
	player_index = nearest_player;
	player_object = GAME_VARIABLES + player_block_offsets[player_index];
	player_object = LINKED(player_object, PLAYER(player_object)->object);
	nearest_player_z = OBJECT(player_object)->z_int;
	nearest_player_floor_z = OBJECT(player_object)->floor_z_int;
	direction = (uint16_t)(direction + direction_to_player_offsets[player_index]);
	OBJECT(current_object)->direction = direction;
	direction_to_nearest_player = direction;
	return octant;
}

void find_nearest_player_x(void)
{
	uint16_t distance_x = 0;
	uint32_t player_index = 0, player_object = 0;
	uint16_t diff_x = 0;
	uint32_t checked_index = 0;

	nearest_player_distance = 0x7FFF;
	player_index = 2u;
	do
	{
		player_object = GAME_VARIABLES + player_block_offsets[player_index];
		player_object = LINKED(player_object, PLAYER(player_object)->object);
		diff_x = OBJECT(player_object)->x_int - OBJECT(current_object)->x_int;
		distance_x = diff_x;
		if ((int16_t)diff_x < 0)
			distance_x = (uint16_t)(0u - distance_x);
		if ((int16_t)distance_x <= nearest_player_distance)
			nearest_player_distance = distance_x;
		checked_index = player_index;
		player_index = checked_index - 1u;
	} while (checked_index != 0u);
}

uint32_t face_object(void)
{
	uint16_t dy = 0;
	uint32_t dx = 0, ignored = 0;

	dx = (uint16_t)(OBJECT(r14)->projected_x - OBJECT(current_object)->projected_x);
	dy = (uint16_t)(OBJECT(r14)->projected_y_int - OBJECT(current_object)->projected_y_int);
	angle_of_vector(dy, &dx, &ignored);
	OBJECT(current_object)->direction = dx;
	return distance_to_object();
}

uint32_t distance_to_object(void)
{
	uint32_t distance = 0, dy = 0, ignored = 0;

	distance = (int16_t)(OBJECT(r14)->projected_x - OBJECT(current_object)->projected_x);
	dy = (int16_t)(OBJECT(r14)->projected_y_int - OBJECT(current_object)->projected_y_int);
	if ((int32_t)distance < 0)
		distance = 0u - distance;
	if ((int32_t)dy < 0)
		dy = 0u - dy;
	distance = distance * distance + dy * dy;
	square_root(&distance, &ignored);
	nearest_player_distance = distance;
	return distance;
}

uint32_t direction_to_object(void)
{
	uint16_t dy = 0;
	uint32_t dx = 0, octant = 0;

	dx = (uint16_t)(mem_read16(r14 + 0x2Au) - OBJECT(current_object)->projected_x);
	dy = (uint16_t)(mem_read16(r14 + 0x2Eu) - OBJECT(current_object)->projected_y_int);
	angle_of_vector(dy, &dx, &octant);
	OBJECT(current_object)->direction = dx;
	return octant;
}

void distance_xy_to_object(uint32_t *p_distance_x, uint32_t *p_distance_y)
{
	uint32_t distance_x = 0, distance_y = 0, x_int = 0, y_int = 0;
	uint16_t dx = 0, dy = 0;

	x_int = (uint16_t)OBJECT(r14)->x_int;
	dx = x_int - OBJECT(current_object)->x_int;
	distance_x = dx;
	if ((int16_t)dx < 0)
		distance_x = (uint16_t)(0u - distance_x);
	distance_to_player_1 = distance_x;
	y_int = (uint16_t)OBJECT(r14)->y_int;
	dy = y_int - OBJECT(current_object)->y_int;
	distance_y = dy;
	if ((int16_t)dy >= 0)
	{
		distance_to_player_2 = distance_y;
		*p_distance_x = distance_x;
		*p_distance_y = distance_y;
		return;
	}
	distance_y = (uint16_t)(0u - distance_y);
	distance_to_player_2 = distance_y;
	*p_distance_x = distance_x;
	*p_distance_y = distance_y;
}

void fall_to_floor(uint32_t *p_height, uint32_t *p_over_pit, uint32_t *p_z)
{
	uint32_t z = *p_z;
	uint8_t over_pit = 0;
	uint32_t height = 0;

	over_pit = 1u;
	if (OBJECT(current_object)->floor_z_int < 0x800)
		over_pit = 0u;
	if ((uint32_t)OBJECT(current_object)->z != (uint32_t)OBJECT(current_object)->floor_z)
	{
		OBJECT(current_object)->speed_z += 0x6000u;
		if ((int32_t)((uint32_t)OBJECT(current_object)->z + OBJECT(current_object)->speed_z) < OBJECT(current_object)->floor_z)
		{
			z = OBJECT(current_object)->z;
			height = OBJECT(current_object)->floor_z - z;
			*p_height = height;
			*p_over_pit = (*p_over_pit & 0xFFFFFF00u) | over_pit;
			*p_z = z;
			return;
		}
		OBJECT(current_object)->z = OBJECT(current_object)->floor_z;
		OBJECT(current_object)->speed_z = 0;
		height = 0u;
		*p_height = height;
		*p_over_pit = (*p_over_pit & 0xFFFFFF00u) | over_pit;
		return;
	}
	OBJECT(current_object)->speed_z = 0;
	height = 0u;
	*p_height = height;
	*p_over_pit = (*p_over_pit & 0xFFFFFF00u) | over_pit;
}

uint32_t place_sprite_at_cursor_floor(void)
{
	uint32_t bucket = 0;

	OBJECT(current_object)->screen_x = OBJECT(object_cursor)->screen_x;
	OBJECT(current_object)->screen_y = OBJECT(object_cursor)->screen_y;
	OBJECT(current_object)->screen_y += OBJECT(object_cursor)->floor_z_int - OBJECT(object_cursor)->z_int;
	bucket = 0xFFu;
	return bucket;
}

void direction_to_speed(uint32_t speed, uint32_t *p_direction, uint32_t *p_speed_x, uint32_t *p_speed_y)
{
	uint32_t direction = *p_direction;
	uint32_t speed_x = 0, speed_y = 0;

	direction &= 0x3FFu;
	speed_x = (int32_t)(sine_table[direction] * speed) >> 6;
	speed_y = 0u - ((int32_t)(cosine_table[direction] * speed) >> 6);
	*p_direction = direction;
	*p_speed_x = speed_x;
	*p_speed_y = speed_y;
}

uint32_t cursor_floor_screen_y(uint32_t floor_screen_y)
{
	floor_screen_y = (floor_screen_y & ~0xFFFFu) | (uint16_t)OBJECT(object_cursor)->floor_z_int;
	floor_screen_y = (floor_screen_y & ~0xFFFFu) | (uint16_t)(floor_screen_y - OBJECT(object_cursor)->z_int);
	floor_screen_y = (floor_screen_y & ~0xFFFFu) | (uint16_t)(floor_screen_y + OBJECT(current_object)->screen_y);
	return floor_screen_y;
}

void explode_object_medium(void)
{
	OBJECT(current_object)->explode_object.frame_index = 0xF;
	OBJECT(current_object)->explode_object.last_frame = 0x1F;
	OBJECT(current_object)->sprite_palette = 0x82A0u;
	OBJECT(current_object)->handler = explosion_animate_ADDRESS;
	explode_object();
}

void explode_object(void)
{
	uint8_t sound_id = 0;
	uint32_t write_index = 0, write_index_address = 0;

	OBJECT(current_object)->explode_object.frame_index = 0x1F;
	OBJECT(current_object)->explode_object.last_frame = 0x2B;
	OBJECT(current_object)->sprite_palette = 0x86D0u;
	OBJECT(current_object)->handler = explosion_animate_ADDRESS;
	OBJECT(current_object)->explode_object.frame_toggle = 0u;
	sound_id = 7u;
	play_sound(sound_id, &write_index, &write_index_address);
}

void explosion_animate(void)
{
	uint32_t block_offset = 0, cursor = 0, ignored = 0, slope_height = 0;
	uint16_t toggle = 0;

	unproject_object(&ignored, &block_offset, &slope_height, &cursor);
	OBJECT(current_object)->explosion.frame_toggle++;
	toggle = OBJECT(current_object)->explosion.frame_toggle & 1u;
	OBJECT(current_object)->explosion.frame_toggle = toggle;
	if (toggle != 0u)
	{
		OBJECT(current_object)->sprite_frame = explosion_frames[(uint8_t)OBJECT(current_object)->explosion.frame_index];
		queue_sprite(0u);
		return;
	}
	OBJECT(current_object)->explosion.frame_index++;
	if (OBJECT(current_object)->explosion.frame_index <= OBJECT(current_object)->explosion.last_frame)
	{
		OBJECT(current_object)->sprite_frame = explosion_frames[(uint8_t)OBJECT(current_object)->explosion.frame_index];
		queue_sprite(0u);
		return;
	}
	delete_object_and_children(&ignored, &block_offset, &slope_height);
}

void drop_rings(uint32_t shadow, uint32_t *p_allocated, uint32_t *p_random_value, uint32_t *p_ring)
{
	uint32_t allocated = *p_allocated;
	uint32_t random_value = *p_random_value;
	uint32_t ring = *p_ring;
	uint32_t count = 0, ignored = 0, new_ring = 0, random_bits = 0;

	if ((int16_t)rings_to_release <= 0)
		return;
	r14 = 0u;
	do
	{
		if (released_rings_random != 0u)
		{
			next_random(&shadow, &ignored, &ignored);
			random_value = shadow;
		}
		count = 1u;
		scatter_rings_random(shadow, count, &allocated, &ring);
		new_ring = ring;
		copy_position_to(new_ring);
		mem_write16(new_ring + 0x26u, object_cursor);
		mem_write8(new_ring + 0x44u, 0u);
		mem_write8(new_ring + 0x45u, 0u);
		mem_write16(new_ring + 0x3Eu, mem_read16(new_ring + 0x3Eu) + 1u);
		mem_write32(new_ring + 0x92u, mem_read32(new_ring + 0x3Cu));
		mem_write16(new_ring + 0x24u, released_ring_angle);
		mem_write32(new_ring + 0x6Eu, 0x60u);
		mem_write32(new_ring + 0x6Eu, mem_read32(new_ring + 0x6Eu) & 0x3FFu);
		mem_write16(new_ring + 0x84u, r14);
		mem_write32(new_ring + 0x1Cu, released_ring_wait_ADDRESS);
		r14 = (uint16_t)(r14 + 0x10u);
		if (released_rings_random != 0u)
		{
			random_bits = random_value & 0x7Fu;
			mem_write32(new_ring + 0x6Eu, mem_read32(new_ring + 0x6Eu) + random_bits);
			mem_write16(new_ring + 0x24u, mem_read16(new_ring + 0x24u) + (random_bits & 0x3Fu) * 2u);
			mem_write16(new_ring + 0x24u, mem_read16(new_ring + 0x24u) - 0x40u);
		}
		shadow = (new_ring & ~0xFFFFu) | mem_read16(new_ring + 0x28u);
		mem_write16(shadow + 0x28u, 0xFFFFu);
		if (released_ring_flip == 1u)
		{
			mem_write16(ring + 6u, mem_read16(ring + 6u) | 1u);
			mem_write16(shadow + 6u, mem_read16(shadow + 6u) | 0x2000u);
			mem_write16(ring + 6u, mem_read16(ring + 6u) ^ 0x8000u);
			mem_write16(shadow + 6u, mem_read16(shadow + 6u) ^ 0x8000u);
		}
		if (released_ring_flip == 2u)
		{
			mem_write16(ring + 6u, mem_read16(ring + 6u) | 0x4000u);
			mem_write16(shadow + 6u, mem_read16(shadow + 6u) | 0x4000u);
			mem_write16(ring + 6u, mem_read16(ring + 6u) ^ 0x8000u);
			mem_write16(shadow + 6u, mem_read16(shadow + 6u) ^ 0x8000u);
		}
		rings_to_release--;
	} while ((int16_t)rings_to_release >= 1);
	*p_allocated = allocated;
	*p_random_value = random_value;
	*p_ring = ring;
}

void released_ring_wait(void)
{
	uint8_t sound_id = 0;
	uint32_t write_index = 0, write_index_address = 0;
	uint16_t release_delay = 0;

	release_delay = OBJECT(current_object)->released_ring.release_delay;
	OBJECT(current_object)->released_ring.release_delay = release_delay - 1u;
	if (release_delay != 0u)
		return;
	sound_id = 0x13u;
	play_sound(sound_id, &write_index, &write_index_address);
	OBJECT(current_object)->released_ring.release_delay = 0u;
	OBJECT(current_object)->handler = released_ring_start_ADDRESS;
	released_ring_start();
}

void released_ring_start(void)
{
	uint16_t pickup_delay = 0;

	OBJECT(current_object)->released_ring.projected_x_carry_int = 0;
	pickup_delay = OBJECT(current_object)->released_ring.pickup_delay;
	OBJECT(current_object)->released_ring.pickup_delay = pickup_delay - 1u;
	if ((int16_t)pickup_delay > 1)
	{
		released_ring_move(0u);
		return;
	}
	OBJECT(current_object)->flags &= ~0x200u;
	OBJECT(current_object)->handler = released_ring_bounce_ADDRESS;
	released_ring_bounce();
}

void released_ring_bounce(void)
{
	uint8_t sound_id = 0;
	uint32_t r2 = 0, ignored = 0, object = 0, player = 0, slope_height = 0, write_index = 0;
	uint16_t lifetime = 0;

	lifetime = OBJECT(current_object)->released_ring.lifetime;
	OBJECT(current_object)->released_ring.lifetime = lifetime - 1u;
	if ((int16_t)lifetime > 1)
	{
		if ((OBJECT(current_object)->flags & 0x20u) != 0u)
		{
			OBJECT(current_object)->direction += 0x300u;
			OBJECT(current_object)->direction &= 0x3FFu;
			OBJECT(current_object)->speed_x = 0;
			OBJECT(current_object)->speed_y = 0;
		}
		if ((OBJECT(current_object)->flags & 4u) == 0u)
		{
			released_ring_move(player);
			return;
		}
		object = LINKED(current_object, OBJECT(current_object)->touched_by);
		if ((int8_t)OBJECT(object)->kind > 2)
		{
			sound_id = 0x13u;
			play_sound(sound_id, &write_index, &r2);
			delete_object_and_children(&ignored, &r2, &slope_height);
			return;
		}
		player = PLAYER_BLOCK(1) + ((uint32_t)OBJECT(object)->player << 9);
		if ((PLAYER(player)->state == 0xDu || PLAYER(player)->state == 0x20u) && (uint8_t)PLAYER(player)->hurt_contact_delay != 0u)
		{
			released_ring_move(player);
			return;
		}
		sound_id = 0x13u;
		play_sound(sound_id, &write_index, &r2);
		delete_object_and_children(&ignored, &r2, &slope_height);
		return;
	}
	OBJECT(current_object)->sprite_flags = 4u;
	OBJECT(current_object)->sprite_scale = 0x100u;
	object = LINKED(current_object, OBJECT(current_object)->child);
	OBJECT(object)->sprite_flags = 0u;
	OBJECT(object)->sprite_scale = 0x100u;
	OBJECT(current_object)->handler = scattered_ring_vanish_ADDRESS;
	scattered_ring_vanish();
}

void released_ring_move(uint32_t cursor)
{
	uint32_t r1 = 0, r2 = 0, r7 = 0, r8 = 0, bounce_speed = 0, direction = 0, height = 0, ignored = 0, speed = 0, speed_x = 0, speed_y = 0;

	move_object_projected(&ignored, &r2, &ignored, &cursor);
	fall_to_floor(&height, &r1, &r2);
	OBJECT(current_object)->animation_frame += 3u;
	if (height == 0u)
	{
		OBJECT(current_object)->animation_frame -= 3u;
		bounce_speed = OBJECT(current_object)->released_ring.bounce_speed;
		bounce_speed = bounce_speed - ((int32_t)bounce_speed >> 2) + 0x10000u;
		OBJECT(current_object)->released_ring.bounce_speed = bounce_speed;
		OBJECT(current_object)->speed_z = bounce_speed;
	}
	direction = OBJECT(current_object)->direction;
	speed = OBJECT(current_object)->speed;
	direction_to_speed(speed, &direction, &speed_x, &speed_y);
	OBJECT(current_object)->speed_x = speed_x;
	OBJECT(current_object)->speed_y = speed_y;
	animate(&speed, &r7, &r8);
	if (OBJECT(current_object)->animation_frame == 4u)
		spawn_ring_glint(cursor);
	queue_sprite_at_floor();
	add_to_touchable_list(direction);
	add_to_rider_list();
	draw_child_shadow();
}

void spawn_debris(uint32_t spawned, uint16_t count, uint32_t block)
{
	uint32_t r3 = 0, ignored = 0, object = 0, random_byte = 0;

	spawn_count = count;
	spawn_objects(0x08E73Au, &spawned, &r3, &block);
	if (spawned == 0u)
		return;
	r14 = r3;
	do
	{
		OBJECT(r14)->x = OBJECT(current_object)->x;
		OBJECT(r14)->y = OBJECT(current_object)->y;
		OBJECT(r14)->projected_x = OBJECT(current_object)->projected_x;
		OBJECT(r14)->projected_y = OBJECT(current_object)->projected_y;
		OBJECT(r14)->z = OBJECT(current_object)->z;
		OBJECT(r14)->z_int--;
		OBJECT(r14)->floor_z = OBJECT(current_object)->floor_z;
		OBJECT(r14)->sprite_palette = 0x8300u;
		OBJECT(r14)->var_81 = 9u;
		OBJECT(r14)->var_82 = 3u;
		OBJECT(r14)->var_84 = 0xFu;
		next_random(&r3, &ignored, &ignored);
		OBJECT(r14)->sprite_scale += r3 & 0xFFu;
		OBJECT(r14)->speed_z = -0x40000;
		random_byte = r3 & 0x3FFFFu;
		OBJECT(r14)->speed_z -= random_byte;
		OBJECT(r14)->direction = OBJECT(current_object)->direction;
		random_byte = random_byte & 0xFFu;
		OBJECT(r14)->direction += random_byte;
		OBJECT(r14)->speed = OBJECT(current_object)->speed;
		random_byte &= 0x3Fu;
		OBJECT(r14)->speed += random_byte;
		if ((random_byte & 1u) != 0u)
		{
			OBJECT(r14)->var_80 = 0xAu;
			OBJECT(r14)->var_81 = 0x13u;
		}
		OBJECT(r14)->handler = debris_fly_ADDRESS;
		object = r14;
		r14 = LINKED(r14, OBJECT(r14)->child);
		OBJECT(object)->child = 0xFFFFu;
	} while ((uint16_t)r14 != 0xFFFFu);
}

void debris_fly(void)
{
	uint32_t r1 = 0, r2 = 0, cursor = 0, deleted = 0, direction = 0, height = 0, ignored = 0, slope_height = 0, speed = 0, speed_x = 0, speed_y = 0;
	uint16_t phase = 0, lifetime = 0;

	move_object_projected(&ignored, &r2, &slope_height, &cursor);
	delete_if_off_screen(&deleted, &r2, &slope_height);
	if ((uint8_t)deleted != 0u)
		return;
	fall_to_floor(&height, &r1, &r2);
	if (height != 0u)
	{
		direction = OBJECT(current_object)->direction;
		speed = OBJECT(current_object)->speed;
		direction_to_speed(speed, &direction, &speed_x, &speed_y);
		OBJECT(current_object)->speed_x += speed_x;
		OBJECT(current_object)->speed_y += speed_y;
		OBJECT(current_object)->debris.frame_timer++;
		phase = OBJECT(current_object)->debris.frame_timer & 3u;
		OBJECT(current_object)->debris.frame_timer = phase;
		if (phase == 0u)
		{
			OBJECT(current_object)->debris.frame_index++;
			if (OBJECT(current_object)->debris.last_frame < OBJECT(current_object)->debris.frame_index)
				OBJECT(current_object)->debris.frame_index -= 9u;
			OBJECT(current_object)->sprite_frame = debris_frames[(uint8_t)OBJECT(current_object)->debris.frame_index];
		}
	}
	else
	{
		OBJECT(current_object)->speed_x = 0;
		OBJECT(current_object)->speed_y = 0;
		lifetime = OBJECT(current_object)->debris.lifetime;
		OBJECT(current_object)->debris.lifetime = lifetime - 1u;
		if (lifetime == 0u)
		{
			delete_current_object();
			return;
		}
		if ((OBJECT(current_object)->debris.lifetime & 1u) != 0u)
			return;
	}
	queue_sprite_at_floor();
}
