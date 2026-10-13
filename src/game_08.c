/**
 *    Name:        game_08.c
 *    Description: the game's functions, part 08 (in the order of their V60 addresses)
 *    Version:     0.1.0
 *    Created:     12-10-2026
 *    Modified:    13-10-2026
 *    Author:      Sebastian Toyos
 *
 *    SegaSonic The Hedgehog (arcade, Sega System 32): the game's V60 program decompiled into C by
 *    tools/decompile.py, with the names of symbols.txt.
 */
#include "game.h"

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

uint32_t face_nearest_player(uint32_t r0, uint32_t octant)
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
			r0 = player_dx;
			dy = (uint16_t)player_dy;
			angle_of_vector(dy, &r0, &octant);
			RAM16(GAME_VARIABLES + direction_to_player_offsets[player_index]) = r0;
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
	r0 = (uint16_t)(r0 + direction_to_player_offsets[player_index]);
	OBJECT(current_object)->direction = r0;
	direction_to_nearest_player = r0;
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

void explode_object(void)
{
	uint8_t sound_id = 0;
	uint32_t r1 = 0, r2 = 0;

	OBJECT(current_object)->explode_object.frame_index = 0x1F;
	OBJECT(current_object)->explode_object.last_frame = 0x2B;
	OBJECT(current_object)->sprite_palette = 0x86D0u;
	OBJECT(current_object)->handler = explosion_animate_ADDRESS;
	OBJECT(current_object)->explode_object.frame_toggle = 0u;
	sound_id = 7u;
	play_sound(sound_id, &r1, &r2);
}

void explosion_animate(void)
{
	uint32_t r2 = 0, r3 = 0, r11 = 0, ignored = 0;
	uint16_t toggle = 0;

	unproject_object(&ignored, &r2, &r3, &r11);
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
	delete_object_and_children(&ignored, &r2, &r3);
}

void drop_rings(uint32_t r3, uint32_t *p_r11, uint32_t *p_random_value, uint32_t *p_ring)
{
	uint32_t r11 = *p_r11;
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
			next_random(&r3, &ignored, &ignored);
			random_value = r3;
		}
		count = 1u;
		scatter_rings_random(r3, count, &r11, &ring);
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
		r3 = (new_ring & ~0xFFFFu) | mem_read16(new_ring + 0x28u);
		mem_write16(r3 + 0x28u, 0xFFFFu);
		if (released_ring_flip == 1u)
		{
			mem_write16(ring + 6u, mem_read16(ring + 6u) | 1u);
			mem_write16(r3 + 6u, mem_read16(r3 + 6u) | 0x2000u);
			mem_write16(ring + 6u, mem_read16(ring + 6u) ^ 0x8000u);
			mem_write16(r3 + 6u, mem_read16(r3 + 6u) ^ 0x8000u);
		}
		if (released_ring_flip == 2u)
		{
			mem_write16(ring + 6u, mem_read16(ring + 6u) | 0x4000u);
			mem_write16(r3 + 6u, mem_read16(r3 + 6u) | 0x4000u);
			mem_write16(ring + 6u, mem_read16(ring + 6u) ^ 0x8000u);
			mem_write16(r3 + 6u, mem_read16(r3 + 6u) ^ 0x8000u);
		}
		rings_to_release--;
	} while ((int16_t)rings_to_release >= 1);
	*p_r11 = r11;
	*p_random_value = random_value;
	*p_ring = ring;
}

void released_ring_wait(void)
{
	uint8_t sound_id = 0;
	uint32_t r1 = 0, r2 = 0;
	uint16_t release_delay = 0;

	release_delay = OBJECT(current_object)->released_ring.release_delay;
	OBJECT(current_object)->released_ring.release_delay = release_delay - 1u;
	if (release_delay != 0u)
		return;
	sound_id = 0x13u;
	play_sound(sound_id, &r1, &r2);
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
	uint32_t r1 = 0, r2 = 0, r3 = 0, ignored = 0, object = 0, player = 0;
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
			play_sound(sound_id, &r1, &r2);
			delete_object_and_children(&ignored, &r2, &r3);
			return;
		}
		player = PLAYER_BLOCK(1) + ((uint32_t)OBJECT(object)->player << 9);
		if ((PLAYER(player)->state == 0xDu || PLAYER(player)->state == 0x20u) && (uint8_t)PLAYER(player)->hurt_contact_delay != 0u)
		{
			released_ring_move(player);
			return;
		}
		sound_id = 0x13u;
		play_sound(sound_id, &r1, &r2);
		delete_object_and_children(&ignored, &r2, &r3);
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

void released_ring_move(uint32_t r11)
{
	uint32_t r1 = 0, r2 = 0, r7 = 0, r8 = 0, bounce_speed = 0, direction = 0, height = 0, ignored = 0, speed = 0, speed_x = 0, speed_y = 0;

	move_object_projected(&ignored, &r2, &ignored, &r11);
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
		spawn_ring_glint(r11);
	queue_sprite_at_floor();
	add_to_touchable_list(direction);
	add_to_rider_list();
	draw_child_shadow();
}

void spawn_debris(uint32_t r0, uint16_t count, uint32_t r11)
{
	uint32_t r3 = 0, ignored = 0, object = 0, random_byte = 0;

	spawn_count = count;
	spawn_objects(0x08E73Au, &r0, &r3, &r11);
	if (r0 == 0u)
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
	uint32_t r1 = 0, r2 = 0, r3 = 0, r11 = 0, deleted = 0, direction = 0, height = 0, ignored = 0, speed = 0, speed_x = 0, speed_y = 0;
	uint16_t phase = 0, lifetime = 0;

	move_object_projected(&ignored, &r2, &r3, &r11);
	delete_if_off_screen(&deleted, &r2, &r3);
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

void queue_string_record(uint32_t record, uint16_t y, uint32_t scale, uint32_t *p_palette, uint32_t *p_text)
{
	uint32_t palette = *p_palette;
	uint32_t text = *p_text;
	uint8_t mode = 0;

	mode = 0u;
	queue_string(record, y, mode, &palette, &scale, &text);
	*p_palette = (*p_palette & 0xFFFF0000u) | (palette & 0xFFFFu);
	*p_text = text;
}

void queue_string_at(uint32_t x, uint16_t y, uint32_t *p_palette, uint32_t *p_scale, uint32_t *p_text)
{
	uint32_t palette = *p_palette;
	uint32_t scale = *p_scale;
	uint32_t text = *p_text;
	uint8_t mode = 0;

	mode = 1u;
	queue_string(x, y, mode, &palette, &scale, &text);
	*p_palette = (*p_palette & 0xFFFF0000u) | (palette & 0xFFFFu);
	*p_scale = (*p_scale & 0xFFFF0000u) | (scale & 0xFFFFu);
	*p_text = text;
}

void queue_string(uint32_t x, uint16_t y, uint8_t mode, uint32_t *p_palette, uint32_t *p_scale, uint32_t *p_text)
{
	uint16_t palette = (uint16_t)*p_palette;
	uint16_t scale = (uint16_t)*p_scale;
	uint32_t text = *p_text;
	uint32_t entry = 0;

	if (string_count > 0x2A)
		return;
	if ((uint8_t)mode == 0u)
	{
		text = x;
		x = mem_read16(text);
		text += 2u;
		y = mem_read16(text);
		text += 2u;
		palette = mem_read16(text);
		text += 2u;
		scale = mem_read16(text);
		text += 2u;
	}
	entry = (uint16_t)string_count * 0xCu + string_queue_ADDRESS;
	mem_write16(entry, x);
	entry += 2u;
	mem_write16(entry, y);
	entry += 2u;
	mem_write16(entry, palette);
	entry += 2u;
	mem_write16(entry, scale);
	mem_write32(entry + 2u, text);
	string_count++;
	*p_palette = (*p_palette & 0xFFFF0000u) | palette;
	*p_scale = (*p_scale & 0xFFFF0000u) | scale;
	*p_text = text;
}

void draw_queued_strings(uint16_t size, uint16_t width, uint32_t sprite_entry, uint32_t *p_scale, uint32_t *p_cursor, uint32_t *p_height)
{
	uint32_t scale = *p_scale;
	uint16_t height = (uint16_t)*p_height;
	uint8_t shift = 0;
	uint16_t advance = 0, char_x = 0, palette = 0, string_y = 0;
	uint32_t char_code = 0, cursor = 0, frame_record = 0, leading_blanks = 0, rounding = 0, text = 0;

	for (;;)
	{
		string_count--;
		cursor = (uint16_t)string_count;
		if ((int16_t)cursor < 0)
			break;
		advance = 0u;
		leading_blanks = 0u;
		rounding = 0x80u;
		shift = 0xF8u;
		frame_record = sprite_frames;
		cursor = cursor * 0xCu + string_queue_ADDRESS;
		char_x = (uint16_t)(int16_t)mem_read16(cursor);
		cursor += 2u;
		string_y = (uint16_t)(int16_t)mem_read16(cursor);
		cursor += 2u;
		palette = mem_read16(cursor);
		cursor += 2u;
		scale = mem_read16(cursor);
		text = mem_read32(cursor + 2u);
		char_code = mem_read8(text);
		r14 = 0u;
		text++;
		do
		{
			frame_record = (frame_record & ~0xFFFFu) | text_frames[char_code];
			if ((uint16_t)frame_record != 0u)
			{
				if (r14 == 0u)
				{
					r14 = 1u;
					size = mem_read16(frame_record);
					frame_record += 2u;
					height = mem_read16(frame_record);
					width = (uint16_t)shift_by(size * scale + rounding, shift, 32);
					advance = (uint16_t)shift_by((uint8_t)height * scale + rounding, shift, 32);
					size = (uint16_t)(((uint16_t)((uint32_t)size << 6) & ~0xFFu) | (uint8_t)height);
					height = (uint16_t)((height & ~0xFFu) | advance);
					size = rotate_by(size, shift, 16);
					frame_record += 2u;
					width |= mem_read16(frame_record);
					frame_record += 2u;
					if (leading_blanks != 0u)
						char_x += leading_blanks * advance;
				}
				else
					frame_record += 6u;
				leading_blanks = 0u;
				mem_write16(sprite_entry, 0xAu);
				sprite_entry += 2u;
				mem_write16(sprite_entry, size);
				sprite_entry += 2u;
				mem_write16(sprite_entry, height);
				sprite_entry += 2u;
				mem_write16(sprite_entry, width);
				sprite_entry += 2u;
				mem_write16(sprite_entry, string_y);
				sprite_entry += 2u;
				mem_write16(sprite_entry, char_x);
				sprite_entry += 2u;
				mem_write16(sprite_entry, mem_read16(frame_record));
				sprite_entry += 2u;
				mem_write16(sprite_entry, palette);
				sprite_entry += 2u;
			}
			else
				leading_blanks++;
			char_x += advance;
			char_code = mem_read8(text);
			text++;
		} while (char_code != 0u);
	}
	string_count = 0;
	mem_write16(sprite_entry, sprite_list_end_command);
	*p_scale = scale;
	*p_cursor = cursor;
	*p_height = (*p_height & 0xFFFF0000u) | height;
}

void start_ranking_entry(void)
{
	uint8_t ranking_size = 0;
	uint16_t rings = 0;
	uint32_t block = 0, entry_x_index = 0, next_block = 0, object = 0, piece = 0, piece_copy = 0, pieces_left = 0, player = 0, previous_block = 0, record = 0, x_index = 0;

	PLAYER(current_player)->game_over_done = 0u;
	rings = PLAYER(current_player)->rings;
	r14 = 0xFu;
	ranking_size = 0xFu;
	record = ranking_ADDRESS;
	do
	{
		if ((int16_t)rings < (int16_t)mem_read16(record))
			break;
		r14 = r14 - 1u;
		record += 8u;
	} while (r14 != 0u);
	if ((int8_t)r14 < (int8_t)ranking_size)
	{
		player = OBJECT(object_cursor)->player;
		ranking_entry_players |= shl32(1u, (int)(player));
		block = ranking_entry_blocks[player];
		next_block = (block & ~0xFFFFu) | (uint16_t)(block + 0xA0u);
		mem_write16(block + 0x28u, next_block);
		object = block;
		init_object(object);
		pieces_left = 4u;
		do
		{
			piece = next_block;
			next_block = (next_block & ~0xFFFFu) | (uint16_t)(next_block + 0x30u);
			mem_write16(piece + 0x28u, next_block);
			piece_copy = piece;
			init_object_2(piece_copy);
			pieces_left--;
		} while (pieces_left != 0u);
		mem_write16(piece + 0x28u, 0xFFFFu);
		mem_write32(block + 0x10u, current_player);
		mem_write8(block + 0x47u, player);
		entry_x_index = player;
		if ((cabinet_config & 1u) == 0u)
			entry_x_index += 3u;
		mem_write16(block + 0xEu, ranking_entry_x[entry_x_index]);
		mem_write16(block + 0xCu, 0xD0u);
		mem_write32(block + 0x3Cu, ranking_entry_trackballs[player]);
		mem_write8(block + 0x80u, ranking_entry_buttons[player]);
		mem_write16(block + 0x84u, 0x4B0u);
		mem_write16(block + 0x24u, r14);
		mem_write8(block + 0x36u, OBJECT(current_object)->kind);
		mem_write16(block + 0x34u, PLAYER(current_player)->rings);
		mem_write8(block + 0x37u, PLAYER(current_player)->stages_reached);
		mem_write32(block + 0x1Cu, 0u);
		previous_block = (piece_copy & ~0xFFFFu) | (uint16_t)block;
		block = (block & ~0xFFFFu) | mem_read16(block + 0x28u);
		mem_write16(block + 4u, 0xA238u);
		mem_write16(block + 0xEu, mem_read16(previous_block + 0xEu));
		mem_write16(block + 0xCu, mem_read16(previous_block + 0xCu));
		mem_write16(block + 0xEu, mem_read16(block + 0xEu) + 0xDu);
		mem_write16(block + 6u, 0x8AD0u);
		mem_write8(block + 0x2Au, 0x80u);
		previous_block = (previous_block & ~0xFFFFu) | (uint16_t)block;
		block = (block & ~0xFFFFu) | mem_read16(block + 0x28u);
		mem_write16(block + 4u, 0xA238u);
		mem_write16(block + 0xEu, mem_read16(previous_block + 0xEu));
		mem_write16(block + 0xCu, mem_read16(previous_block + 0xCu));
		mem_write16(block + 0xEu, mem_read16(block + 0xEu) + 0xDu);
		mem_write16(block + 6u, 0x8AD0u);
		mem_write8(block + 0x2Au, 0x80u);
		previous_block = (previous_block & ~0xFFFFu) | (uint16_t)block;
		block = (block & ~0xFFFFu) | mem_read16(block + 0x28u);
		mem_write16(block + 4u, 0xA238u);
		mem_write16(block + 0xEu, mem_read16(previous_block + 0xEu));
		mem_write16(block + 0xCu, mem_read16(previous_block + 0xCu));
		mem_write16(block + 0xEu, mem_read16(block + 0xEu) + 0xDu);
		mem_write16(block + 6u, 0x8AD0u);
		mem_write8(block + 0x2Au, 0x80u);
		previous_block = (previous_block & ~0xFFFFu) | (uint16_t)block;
		block = (block & ~0xFFFFu) | mem_read16(block + 0x28u);
		mem_write16(block + 0xEu, mem_read16(previous_block + 0xEu));
		mem_write16(block + 0xCu, mem_read16(previous_block + 0xCu));
		mem_write16(block + 0xEu, mem_read16(block + 0xEu) + 0xDu);
		mem_write8(block + 0x2Du, 0x22u);
		mem_write16(block + 4u, 0xA230u);
		mem_write16(block + 6u, 0x8AD0u);
		return;
	}
	player = OBJECT(object_cursor)->player;
	ranking_entry_players |= shl32(1u, (int)(player));
	block = ranking_entry_blocks[player];
	x_index = OBJECT(object_cursor)->player;
	if ((cabinet_config & 1u) == 0u)
		x_index += 3u;
	mem_write16(block + 0xEu, ranking_entry_x[x_index]);
	mem_write16(block + 0x82u, 0x78u);
	mem_write32(block + 0x10u, current_player);
	mem_write32(block + 0x1Cu, 1u);
}

void enter_ranking_initials(uint32_t r0)
{
	uint16_t abs_dx = 0, abs_dy = 0, dx = 0, dy = 0, speed_x_fraction = 0, y = 0;
	uint32_t blink_timer = 0, button_bit = 0, copy_dest = 0, copy_source = 0, count = 0, crc = 0, data = 0, dest = 0, ignored = 0, letters_left = 0, links_left = 0, object = 0, palette = 0, player = 0, read_cursor = 0, record_initials = 0, record_rings = 0, records_left = 0, scale = 0, screen_x = 0, slot = 0, speed_y = 0, text = 0, text_x = 0, trackball = 0, x = 0;
	uint16_t out_of_rank_timer = 0, time_left = 0, slot_position = 0, letter_position = 0, direction = 0;

	player = OBJECT(object_cursor)->player;
	if ((ranking_entry_players & shl32(1u, (int)(player))) == 0u)
		return;
	current_object = ranking_entry_blocks[player];
	if (OBJECT(current_object)->handler != 0u)
	{
		out_of_rank_timer = OBJECT(current_object)->var_82;
		OBJECT(current_object)->var_82 = out_of_rank_timer - 1u;
		if (out_of_rank_timer == 0u)
		{
			mem_write8(AS32(OBJECT(current_object)->hitbox_size_x) + 0x135u, 1u);
			ranking_entry_players &= ~shl32(1u, (int)(OBJECT(object_cursor)->player));
			return;
		}
		blink_timer = (r0 & ~0xFFFFu) | OBJECT(current_object)->var_82;
		if ((blink_timer & 3u) != 0u)
		{
			x = (blink_timer & ~0xFFFFu) | (uint16_t)OBJECT(current_object)->screen_x;
			y = 0xC8u;
			palette = 0x8F70u;
			scale = 0x100u;
			text = out_of_rank_text_ADDRESS;
			queue_string_at(x, y, &palette, &scale, &text);
		}
		return;
	}
	r14 = current_object;
	button_bit = OBJECT(current_object)->var_80;
	if ((inputs_pressed & shl32(1u, (int)(button_bit))) == 0u)
	{
	enter_ranking_initials_count_down:
		time_left = OBJECT(current_object)->var_84;
		OBJECT(current_object)->var_84 = time_left - 1u;
		if (time_left != 0u)
		{
			screen_x = (uint16_t)OBJECT(current_object)->screen_x;
			y = (uint16_t)(OBJECT(current_object)->screen_y - 0x20u);
			palette = 0x8AD0u;
			scale = 0x100u;
			text = 0x8F08Eu + (int32_t)OBJECT(current_object)->var_84 / 0x3C * 3u;
			queue_string_at(screen_x, y, &palette, &scale, &text);
			text_x = (uint16_t)OBJECT(current_object)->screen_x;
			text_x = (uint16_t)(text_x + 0x20u);
			y = (uint16_t)(OBJECT(current_object)->screen_y - 0x20u);
			palette = 0x8AD0u;
			scale = 0x100u;
			text = 0x8EFF8u + OBJECT(current_object)->direction * 0xAu;
			queue_string_at(text_x, y, &palette, &scale, &text);
			trackball = OBJECT(current_object)->speed_z;
			abs_dx = mem_read16(trackball);
			dx = (uint16_t)abs_dx;
			trackball += 2u;
			if ((int16_t)abs_dx < 0)
				abs_dx = (uint16_t)(0u - abs_dx);
			abs_dy = mem_read16(trackball);
			dy = (uint16_t)abs_dy;
			if ((int16_t)abs_dy < 0)
				abs_dy = (uint16_t)(0u - abs_dy);
			if ((int16_t)abs_dy < (int16_t)abs_dx)
			{
				slot_position = OBJECT(current_object)->spawn_arg_1 + dx;
				OBJECT(current_object)->spawn_arg_1 = slot_position;
				if ((int16_t)slot_position < 0)
					OBJECT(current_object)->spawn_arg_1 = 0u;
				if (AT8(OBJECT(current_object)->spawn_arg_2, 1) != AT8(OBJECT(current_object)->spawn_arg_1, 1))
				{
					AS8(OBJECT(current_object)->spawn_arg_1) = 0x80u;
					OBJECT(current_object)->spawn_arg_2 = OBJECT(current_object)->spawn_arg_1;
				}
			}
			links_left = AT8(OBJECT(current_object)->spawn_arg_1, 1);
			slot = links_left;
			links_left = (uint8_t)(links_left + 1u);
			if ((int8_t)links_left > 4)
			{
				AT8(OBJECT(current_object)->spawn_arg_1, 1) = 3u;
				links_left = 4u;
				slot = 3u;
			}
			do
			{
				current_object = LINKED(current_object, OBJECT(current_object)->child);
				links_left--;
			} while (links_left != 0u);
			AS8(OBJECT(current_object)->projected_y_int) = 1u;
			if (slot != 3u && dy != 0u)
			{
				letter_position = OBJECT(current_object)->projected_x + dy;
				OBJECT(current_object)->projected_x = letter_position;
				if ((int16_t)letter_position < 0)
					AT16(OBJECT(current_object)->projected_x, 1) = 0x2Bu;
				if ((int8_t)AT8(OBJECT(current_object)->projected_x, 1) > 0x2B)
					AT8(OBJECT(current_object)->projected_x, 1) = 0u;
				if (AT8(OBJECT(current_object)->projected_y, 1) != AT8(OBJECT(current_object)->projected_x, 1))
					AS8(OBJECT(current_object)->projected_x) = 0x80u;
				OBJECT(current_object)->projected_y_fraction = OBJECT(current_object)->projected_x;
				mem_write8(ranking_initial_fields[slot] + r14, AT8(OBJECT(current_object)->projected_x, 1));
			}
			current_object = ranking_entry_blocks[OBJECT(object_cursor)->player];
			letters_left = 3u;
			do
			{
				current_object = LINKED(current_object, OBJECT(current_object)->child);
				if (AS8(OBJECT(current_object)->projected_y_int) == 0u)
				{
				enter_ranking_initials_draw_letter:
					queue_sprite_on_top();
				}
				else
				{
					AS8(OBJECT(current_object)->projected_y_int) = 0u;
					OBJECT(current_object)->sprite_frame = ranking_letter_sprites[AT8(OBJECT(current_object)->projected_x, 1)];
					OBJECT(current_object)->direction++;
					if ((OBJECT(current_object)->direction & 3u) != 0u)
						goto enter_ranking_initials_draw_letter;
				}
				object = current_object;
				letters_left--;
			} while (letters_left != 0u);
			current_object = LINKED(current_object, OBJECT(current_object)->child);
			if (AS8(OBJECT(current_object)->projected_y_int) == 0u)
			{
				OBJECT(current_object)->screen_x = OBJECT(object)->screen_x;
				OBJECT(current_object)->screen_y = OBJECT(object)->screen_y;
				OBJECT(current_object)->screen_x += 0x18u;
				queue_sprite_on_top();
			}
			else
			{
				AS8(OBJECT(current_object)->projected_y_int) = 0u;
				direction = OBJECT(current_object)->direction;
				OBJECT(current_object)->direction = direction - 1u;
				if (direction != 0u)
				{
					OBJECT(current_object)->screen_x = OBJECT(object)->screen_x;
					OBJECT(current_object)->screen_y = OBJECT(object)->screen_y;
					OBJECT(current_object)->screen_x += 0x18u;
					queue_sprite_on_top();
					current_object = object_cursor;
					return;
				}
				OBJECT(current_object)->direction = 3u;
			}
			current_object = object_cursor;
			return;
		}
	enter_ranking_initials_save_record:
		mem_write8(AS32(OBJECT(current_object)->hitbox_size_x) + 0x135u, 1u);
		speed_x_fraction = OBJECT(current_object)->speed_x_fraction;
		read_cursor = ranking_ADDRESS;
		if ((int16_t)speed_x_fraction < (int16_t)AS16(ranking))
		{
			current_object = object_cursor;
			ranking_entry_players &= ~shl32(1u, (int)(OBJECT(object_cursor)->player));
			return;
		}
		dest = read_cursor;
		r14 = 0xEu;
		read_cursor += 8u;
		do
		{
			record_rings = mem_read32(read_cursor);
			read_cursor += 4u;
			if ((int16_t)speed_x_fraction < (int16_t)record_rings)
				break;
			record_initials = mem_read32(read_cursor);
			mem_write32(dest, record_rings);
			dest += 4u;
			mem_write32(dest, record_initials);
			r14 = r14 - 1u;
			read_cursor += 4u;
			dest += 4u;
		} while (r14 != 0u);
		speed_y = OBJECT(current_object)->speed_y;
		if (speed_y == 0x170412u || speed_y == 0x171717u || speed_y == 0x172B17u || speed_y == 0x2B1717u)
			OBJECT(current_object)->speed_y = 0xA0013;
		mem_write16(dest, OBJECT(current_object)->speed_x_fraction);
		dest += 2u;
		mem_write8(dest, AS8(OBJECT(current_object)->speed_x_int));
		dest++;
		mem_write8(dest, AT8(OBJECT(current_object)->speed_x_int, 1));
		dest++;
		mem_write8(dest, AS8(OBJECT(current_object)->speed_y));
		dest++;
		mem_write8(dest, AT8(OBJECT(current_object)->speed_y, 1));
		dest++;
		mem_write8(dest, AS8(OBJECT(current_object)->speed_y_int));
		mem_write8(dest + 1u, AT8(OBJECT(current_object)->speed_y_int, 1));
		copy_source = ranking_ADDRESS;
		copy_dest = saved_ranking;
		records_left = 0xFu;
		do
		{
			mem_write32(copy_dest, mem_read32(copy_source));
			copy_source += 4u;
			copy_dest += 4u;
			mem_write32(copy_dest, mem_read32(copy_source));
			records_left--;
			copy_source += 4u;
			copy_dest += 4u;
		} while (records_left != 0u);
		data = ranking_ADDRESS;
		count = 0x78u;
		compute_crc16(count, &crc, &data, &ignored);
		mem_write16(ranking_checksum, crc);
		current_object = object_cursor;
		ranking_entry_players &= ~shl32(1u, (int)(OBJECT(object_cursor)->player));
		return;
	}
	if (AT8(OBJECT(current_object)->spawn_arg_1, 1) == 3u)
		goto enter_ranking_initials_save_record;
	AT8(OBJECT(current_object)->spawn_arg_1, 1)++;
	if ((int8_t)AT8(OBJECT(current_object)->spawn_arg_1, 1) <= 3)
		goto enter_ranking_initials_count_down;
	AT8(OBJECT(current_object)->spawn_arg_1, 1) = 3u;
	goto enter_ranking_initials_count_down;
}

uint32_t load_ranking(void)
{
	uint32_t count = 0, count_2 = 0, crc = 0, data = 0, dest = 0, ignored = 0, source = 0;

	data = saved_ranking;
	count_2 = 0x78u;
	compute_crc16(count_2, &crc, &data, &ignored);
	if (mem_read16(ranking_checksum) != (uint16_t)crc)
		return reset_ranking();
	source = saved_ranking;
	dest = ranking_ADDRESS;
	count = 0xFu;
	return copy_ranking_records(source, dest, count);
}

uint32_t reset_ranking(void)
{
	uint32_t count = 0, dest = 0, letter = 0, read_cursor = 0, records_left = 0, source = 0, write_cursor = 0;
	uint8_t second_letter = 0, third_letter = 0, last_byte = 0;

	read_cursor = default_ranking_ADDRESS;
	write_cursor = ranking_ADDRESS;
	records_left = 0xFu;
	do
	{
		mem_write16(write_cursor, ROM16(read_cursor));
		write_cursor += 2u;
		read_cursor += 2u;
		mem_write16(write_cursor, ROM16(read_cursor));
		write_cursor += 2u;
		read_cursor += 2u;
		letter = ROM8(read_cursor);
		read_cursor++;
		letter = ascii_to_ranking_letter(letter);
		mem_write8(write_cursor, letter);
		second_letter = ROM8(read_cursor);
		read_cursor++;
		letter = second_letter;
		write_cursor++;
		letter = ascii_to_ranking_letter(letter);
		mem_write8(write_cursor, letter);
		third_letter = ROM8(read_cursor);
		read_cursor++;
		letter = third_letter;
		write_cursor++;
		letter = ascii_to_ranking_letter(letter);
		mem_write8(write_cursor, letter);
		last_byte = ROM8(read_cursor);
		read_cursor++;
		write_cursor++;
		mem_write8(write_cursor, last_byte);
		records_left--;
		write_cursor++;
	} while (records_left != 0u);
	source = ranking_ADDRESS;
	dest = saved_ranking;
	count = 0xFu;
	return copy_ranking_records(source, dest, count);
}

uint32_t copy_ranking_records(uint32_t source, uint32_t dest, uint32_t count)
{
	uint32_t count_2 = 0, crc = 0, data = 0, polynomial = 0;

	do
	{
		mem_write32(dest, mem_read32(source));
		source += 4u;
		dest += 4u;
		mem_write32(dest, mem_read32(source));
		count--;
		source += 4u;
		dest += 4u;
	} while (count != 0u);
	data = ranking_ADDRESS;
	count_2 = 0x78u;
	compute_crc16(count_2, &crc, &data, &polynomial);
	mem_write16(ranking_checksum, crc);
	return polynomial;
}

uint32_t ascii_to_ranking_letter(uint32_t letter)
{
	if ((int8_t)letter >= 0x61)
	{
		letter = (letter & ~0xFFu) | (uint8_t)(letter - 0x61u);
		return letter;
	}
	if ((uint8_t)letter == 0x20u)
	{
		letter = (letter & ~0xFFu) | 0x2Bu;
		return letter;
	}
	do
	{
	} while ((uint8_t)letter != 0x33u);
	letter = (letter & ~0xFFu) | 0x23u;
	return letter;
}

void start_ranking_display(uint32_t r11, uint32_t *p_rows, uint32_t *p_source)
{
	uint32_t ignored = 0, object = 0, rows = 0, source = 0;

	r11 = allocate_object_or_start_list(r11);
	OBJECT(r11)->parent = object_cursor;
	OBJECT(r11)->child = 0xFFFFu;
	init_object(r11);
	object = r11;
	show_nbg0_picture(&ignored, &rows, &source, &ignored);
	OBJECT(object)->screen_x = 0xA0;
	OBJECT(object)->screen_y = -0x1A4;
	OBJECT(object)->handler = build_ranking_display_ADDRESS;
	*p_rows = rows;
	*p_source = source;
}

void build_ranking_display(void)
{
	uint32_t block = 0, character = 0, first_initial = 0, free_count = 0, hundreds_digit = 0, label_second_char = 0, object = 0, rank_label = 0, record = 0, remainder = 0, rows_left = 0, second_initial = 0, stages = 0, tens_digit = 0, third_initial = 0, thousands_digit = 0;

	record = ranking_ADDRESS;
	current_player = 0u;
	rows_left = 0xFu;
	OBJECT(current_object)->speed_z = 0x180;
	OBJECT(current_object)->handler = ranking_display_scroll_in_ADDRESS;
	do
	{
		spawn_count = 0xCu;
		spawn_object_with_pieces(0x08F28Bu, &third_initial, &tens_digit, &free_count, &object, &block);
		OBJECT(object)->screen_x = 0xC;
		rank_label = 0x8F858u + ((current_player & ~0xFFu) | (uint8_t)(current_player * 2u));
		OBJECT(object)->sprite_frame = ranking_letter_sprites[mem_read8(rank_label)];
		OBJECT(object)->sprite_palette = 0x8AD0u;
		OBJECT(object)->spawn_arg_1 = rows_left;
		OBJECT(object)->handler = ranking_row_update_ADDRESS;
		object = LINKED(object, OBJECT(object)->child);
		OBJECT(object)->screen_x = 0x18;
		label_second_char = mem_read8(rank_label + 1u);
		OBJECT(object)->sprite_frame = ranking_letter_sprites[label_second_char];
		OBJECT(object)->sprite_palette = 0x8AD0u;
		remainder = mem_read16(record);
		remainder = remainder & 0x7FFFu;
		object = LINKED(object, OBJECT(object)->child);
		OBJECT(object)->screen_x = 0xC0;
		thousands_digit = (int32_t)remainder / 0x3E8;
		thousands_digit = (thousands_digit & ~0xFFu) | (uint8_t)(thousands_digit + 0x20u);
		OBJECT(object)->sprite_frame = ranking_letter_sprites[thousands_digit];
		OBJECT(object)->sprite_palette = 0x8AC0u;
		record += 2u;
		if ((uint8_t)thousands_digit != 0x20u)
			remainder -= ((thousands_digit & ~0xFFu) | (uint8_t)(thousands_digit - 0x20u)) * 0x3E8u;
		object = LINKED(object, OBJECT(object)->child);
		OBJECT(object)->screen_x = 0xCC;
		hundreds_digit = (int32_t)(uint16_t)remainder / 0x64;
		hundreds_digit = (hundreds_digit & ~0xFFu) | (uint8_t)(hundreds_digit + 0x20u);
		OBJECT(object)->sprite_frame = ranking_letter_sprites[hundreds_digit];
		OBJECT(object)->sprite_palette = 0x8AC0u;
		if ((uint8_t)hundreds_digit != 0x20u)
			remainder -= ((hundreds_digit & ~0xFFu) | (uint8_t)(hundreds_digit - 0x20u)) * 0x64u;
		object = LINKED(object, OBJECT(object)->child);
		OBJECT(object)->screen_x = 0xD8;
		tens_digit = (int32_t)(uint16_t)remainder / 0xA;
		tens_digit = (tens_digit & ~0xFFu) | (uint8_t)(tens_digit + 0x20u);
		OBJECT(object)->sprite_frame = ranking_letter_sprites[tens_digit];
		OBJECT(object)->sprite_palette = 0x8AC0u;
		if ((uint8_t)tens_digit != 0x20u)
			remainder -= ((tens_digit & ~0xFFu) | (uint8_t)(tens_digit - 0x20u)) * 0xAu;
		object = LINKED(object, OBJECT(object)->child);
		OBJECT(object)->screen_x = 0xE4;
		OBJECT(object)->sprite_frame = ranking_letter_sprites[(remainder & ~0xFFu) | (uint8_t)(remainder + 0x20u)];
		OBJECT(object)->sprite_palette = 0x8AC0u;
		character = mem_read8(record);
		character = character % 3u;
		object = LINKED(object, OBJECT(object)->child);
		OBJECT(object)->sprite_flags |= 0x100u;
		OBJECT(object)->screen_x = 0x68;
		OBJECT(object)->sprite_palette = ranking_portrait_palettes[character];
		AS8(OBJECT(object)->projected_y_int) = character;
		object = LINKED(object, OBJECT(object)->child);
		OBJECT(object)->screen_x = 0x96;
		OBJECT(object)->sprite_frame = ranking_name_sprites[character];
		OBJECT(object)->sprite_palette = 0x8B10u;
		record++;
		stages = mem_read8(record);
		stages = stages % 9u;
		object = LINKED(object, OBJECT(object)->child);
		OBJECT(object)->screen_x = 0x114;
		OBJECT(object)->sprite_frame = ranking_stage_sprites[(uint8_t)(stages - 1u)];
		OBJECT(object)->sprite_palette = 0x8AD0u;
		object = LINKED(object, OBJECT(object)->child);
		OBJECT(object)->screen_x = 0x30;
		record++;
		first_initial = mem_read8(record);
		OBJECT(object)->sprite_frame = ranking_letter_sprites[first_initial % 0x2Cu];
		OBJECT(object)->sprite_palette = 0x8AD0u;
		object = LINKED(object, OBJECT(object)->child);
		OBJECT(object)->screen_x = 0x3C;
		record++;
		second_initial = mem_read8(record);
		OBJECT(object)->sprite_frame = ranking_letter_sprites[second_initial % 0x2Cu];
		OBJECT(object)->sprite_palette = 0x8AD0u;
		object = LINKED(object, OBJECT(object)->child);
		OBJECT(object)->screen_x = 0x48;
		record++;
		third_initial = mem_read8(record);
		OBJECT(object)->sprite_frame = ranking_letter_sprites[third_initial % 0x2Cu];
		OBJECT(object)->sprite_palette = 0x8AD0u;
		current_player = (current_player & ~0xFFu) | (uint8_t)(current_player + 1u);
		rows_left--;
		record += 2u;
	} while (rows_left != 0u);
	spawn_count = 5u;
	spawn_object_with_pieces(0x08F482u, &third_initial, &tens_digit, &free_count, &object, &block);
	ranking_title_layout++;
	ranking_title_layout &= 1u;
	OBJECT(object)->screen_x = 0xA0;
	OBJECT(object)->screen_y = 0x70;
	OBJECT(object)->sprite_scale = 0x10u;
	OBJECT(object)->sprite_frame = 0xA2B0u;
	OBJECT(object)->sprite_palette = 0x8B20u;
	OBJECT(object)->handler = ranking_title_spin_in_ADDRESS;
	object = LINKED(object, OBJECT(object)->child);
	OBJECT(object)->sprite_frame = 0xA2A8u;
	OBJECT(object)->sprite_palette = 0x8B20u;
	object = LINKED(object, OBJECT(object)->child);
	OBJECT(object)->sprite_frame = 0xA340u;
	OBJECT(object)->sprite_palette = 0x8B20u;
	object = LINKED(object, OBJECT(object)->child);
	OBJECT(object)->sprite_frame = 0xA2D0u;
	OBJECT(object)->sprite_palette = 0x8B20u;
	object = LINKED(object, OBJECT(object)->child);
	OBJECT(object)->sprite_frame = 0xA360u;
	OBJECT(object)->sprite_palette = 0x8B20u;
}

void ranking_title_spin_in(void)
{
	uint32_t direction = 0, speed = 0, speed_x = 0, speed_y = 0;

	if ((int16_t)OBJECT(current_object)->sprite_scale < 0x200)
		OBJECT(current_object)->sprite_scale += 2u;
	OBJECT(current_object)->direction += 8u;
	OBJECT(current_object)->direction &= 0x3FFu;
	direction = OBJECT(current_object)->direction;
	speed = 0x2000u;
	direction_to_speed(speed, &direction, &speed_x, &speed_y);
	OBJECT(current_object)->speed_x = speed_x;
	OBJECT(current_object)->speed_y = speed_y;
	if ((int16_t)OBJECT(current_object)->sprite_scale < 0x200 || OBJECT(current_object)->direction != 0x300u)
	{
		draw_ranking_title();
		return;
	}
	OBJECT(current_object)->ranking_title.pause_timer = 0x1C;
	OBJECT(current_object)->handler = ranking_title_pause_ADDRESS;
	ranking_title_pause();
}

void ranking_title_pause(void)
{
	uint16_t pause_timer = 0;

	pause_timer = OBJECT(current_object)->ranking_title.pause_timer;
	OBJECT(current_object)->ranking_title.pause_timer = pause_timer - 1u;
	if (pause_timer != 0u)
	{
		draw_ranking_title();
		return;
	}
	OBJECT(current_object)->handler = ranking_title_rise_ADDRESS;
	ranking_title_rise();
}

void ranking_title_rise(void)
{
	OBJECT(current_object)->screen_y--;
	if (OBJECT(current_object)->screen_y > 0x23)
	{
		draw_ranking_title();
		return;
	}
	OBJECT(current_object)->screen_y = 0x23;
	OBJECT(current_object)->handler = draw_ranking_title_ADDRESS;
	draw_ranking_title();
}

void draw_ranking_title(void)
{
	uint32_t r1 = 0, layout_offset = 0;

	r1 = queue_sprite(0u);
	current_object = LINKED(current_object, OBJECT(current_object)->child);
	OBJECT(current_object)->sprite_scale = OBJECT(object_cursor)->sprite_scale;
	OBJECT(current_object)->screen_x = OBJECT(object_cursor)->screen_x;
	OBJECT(current_object)->screen_y = OBJECT(object_cursor)->screen_y;
	OBJECT(current_object)->screen_x += OBJECT(object_cursor)->speed_x_int;
	OBJECT(current_object)->screen_y += OBJECT(object_cursor)->speed_y_int;
	r1 = queue_sprite(r1);
	current_object = LINKED(current_object, OBJECT(current_object)->child);
	OBJECT(current_object)->sprite_scale = OBJECT(object_cursor)->sprite_scale;
	OBJECT(current_object)->screen_x = OBJECT(object_cursor)->screen_x;
	OBJECT(current_object)->screen_y = OBJECT(object_cursor)->screen_y;
	OBJECT(current_object)->screen_x -= OBJECT(object_cursor)->speed_x_int;
	OBJECT(current_object)->screen_y -= OBJECT(object_cursor)->speed_y_int;
	r1 = queue_sprite(r1);
	current_object = LINKED(current_object, OBJECT(current_object)->child);
	OBJECT(current_object)->sprite_scale = OBJECT(object_cursor)->sprite_scale;
	OBJECT(current_object)->screen_x = OBJECT(object_cursor)->screen_x;
	OBJECT(current_object)->screen_y = OBJECT(object_cursor)->screen_y;
	OBJECT(current_object)->screen_x += OBJECT(object_cursor)->speed_x_int * 2u;
	layout_offset = (r1 & ~0xFFFFu) | (uint16_t)OBJECT(object_cursor)->speed_y_int;
	layout_offset = (layout_offset & ~0xFFFFu) | (uint16_t)(layout_offset * 2u);
	if (ranking_title_layout == 0u)
		OBJECT(current_object)->screen_x += layout_offset;
	else
		OBJECT(current_object)->screen_y += layout_offset;
	layout_offset = queue_sprite(layout_offset);
	current_object = LINKED(current_object, OBJECT(current_object)->child);
	OBJECT(current_object)->sprite_scale = OBJECT(object_cursor)->sprite_scale;
	OBJECT(current_object)->screen_x = OBJECT(object_cursor)->screen_x;
	OBJECT(current_object)->screen_y = OBJECT(object_cursor)->screen_y;
	OBJECT(current_object)->screen_x -= OBJECT(object_cursor)->speed_x_int * 2u;
	layout_offset = (layout_offset & ~0xFFFFu) | (uint16_t)OBJECT(object_cursor)->speed_y_int;
	layout_offset = (layout_offset & ~0xFFFFu) | (uint16_t)(layout_offset * 2u);
	if (ranking_title_layout == 0u)
		OBJECT(current_object)->screen_x -= layout_offset;
	else
		OBJECT(current_object)->screen_y -= layout_offset;
	queue_sprite(layout_offset);
}

void ranking_display_scroll_in(void)
{
	OBJECT(current_object)->screen_y++;
	if (OBJECT(current_object)->screen_y < 0x24)
	{
		drift_ranking_background();
		return;
	}
	OBJECT(current_object)->ranking_display.rows_animate = 1u;
	OBJECT(current_object)->handler = ranking_display_hold_ADDRESS;
	ranking_display_hold();
}

void ranking_display_hold(void)
{
	OBJECT(current_object)->ranking_display.hold_timer++;
	if (OBJECT(current_object)->ranking_display.hold_timer <= 0x12C)
	{
		drift_ranking_background();
		return;
	}
	AS16(OBJECT(current_object)->ranking_display.rows_remove) = 1u;
	OBJECT(current_object)->handler = ranking_display_end_ADDRESS;
	drift_ranking_background();
}

void drift_ranking_background(void)
{
	uint32_t direction = 0, speed = 0, speed_x = 0, speed_y = 0;
	uint16_t speed_fraction = 0;

	if ((uint16_t)OBJECT(current_object)->speed_z_int != 0u)
	{
	drift_ranking_background_swing_down:
		OBJECT(current_object)->speed_z_fraction -= 2u;
		if ((int16_t)OBJECT(current_object)->speed_z_fraction < 0x180)
			OBJECT(current_object)->speed_z_int = 0;
	}
	else
	{
		OBJECT(current_object)->speed_z_fraction += 2u;
		if ((int16_t)OBJECT(current_object)->speed_z_fraction > 0x280)
		{
			OBJECT(current_object)->speed_z_int = 1;
			goto drift_ranking_background_swing_down;
		}
	}
	if ((uint16_t)OBJECT(current_object)->speed_int != 0u)
	{
		speed_fraction = OBJECT(current_object)->speed_fraction;
		OBJECT(current_object)->speed_fraction = speed_fraction - 8u;
		if (speed_fraction < 8u)
			OBJECT(current_object)->speed = 0;
	}
	else
	{
		OBJECT(current_object)->speed_fraction += 8u;
		if ((int16_t)OBJECT(current_object)->speed_fraction > 0x300)
		{
			OBJECT(current_object)->speed_int = 1;
			speed_fraction = OBJECT(current_object)->speed_fraction;
			OBJECT(current_object)->speed_fraction = speed_fraction - 8u;
			if (speed_fraction < 8u)
				OBJECT(current_object)->speed = 0;
			direction = OBJECT(current_object)->speed_z_fraction;
			speed = 0x100u;
			direction_to_speed(speed, &direction, &speed_x, &speed_y);
			OBJECT(current_object)->speed_x += speed_x;
			OBJECT(current_object)->speed_y += speed_y;
			nbg0_scroll_x_int = OBJECT(current_object)->speed_x_int;
			nbg0_scroll_y_int = OBJECT(current_object)->speed_y_int;
			return;
		}
	}
	direction = OBJECT(current_object)->speed_z_fraction;
	speed = 0x100u;
	direction_to_speed(speed, &direction, &speed_x, &speed_y);
	OBJECT(current_object)->speed_x += speed_x;
	OBJECT(current_object)->speed_y += speed_y;
	nbg0_scroll_x_int = OBJECT(current_object)->speed_x_int;
	nbg0_scroll_y_int = OBJECT(current_object)->speed_y_int;
}

void ranking_display_end(void)
{
	uint32_t r2 = 0, r3 = 0, ignored = 0;

	ranking_done = 1u;
	delete_object_and_children(&ignored, &r2, &r3);
}

void ranking_row_update(void)
{
	uint8_t bucket = 0;
	uint32_t r3 = 0, frame_index = 0, ignored = 0, portrait_frames = 0;
	uint16_t direction = 0;

	current_object = LINKED(current_object, OBJECT(current_object)->parent);
	OBJECT(object_cursor)->ranking_row.animate = OBJECT(current_object)->var_80;
	if (OBJECT(current_object)->var_81 != 0u)
	{
		delete_object_and_children(&ignored, &frame_index, &r3);
		return;
	}
	OBJECT(object_cursor)->screen_y = OBJECT(current_object)->screen_y;
	current_object = object_cursor;
	OBJECT(current_object)->screen_y += OBJECT(current_object)->ranking_row.row * 0x17u;
	bucket = 0u;
	queue_sprite_in_bucket(bucket);
	current_object = LINKED(current_object, OBJECT(current_object)->child);
	OBJECT(current_object)->screen_y = OBJECT(object_cursor)->screen_y;
	bucket = 0u;
	queue_sprite_in_bucket(bucket);
	current_object = LINKED(current_object, OBJECT(current_object)->child);
	OBJECT(current_object)->screen_y = OBJECT(object_cursor)->screen_y;
	bucket = 0u;
	queue_sprite_in_bucket(bucket);
	current_object = LINKED(current_object, OBJECT(current_object)->child);
	OBJECT(current_object)->screen_y = OBJECT(object_cursor)->screen_y;
	bucket = 0u;
	queue_sprite_in_bucket(bucket);
	current_object = LINKED(current_object, OBJECT(current_object)->child);
	OBJECT(current_object)->screen_y = OBJECT(object_cursor)->screen_y;
	bucket = 0u;
	queue_sprite_in_bucket(bucket);
	current_object = LINKED(current_object, OBJECT(current_object)->child);
	OBJECT(current_object)->screen_y = OBJECT(object_cursor)->screen_y;
	bucket = 0u;
	queue_sprite_in_bucket(bucket);
	current_object = LINKED(current_object, OBJECT(current_object)->child);
	OBJECT(current_object)->screen_y = OBJECT(object_cursor)->screen_y;
	if (OBJECT(object_cursor)->ranking_row.animate == 0u)
	{
	ranking_row_set_portrait_frame:
		portrait_frames = ranking_portrait_frames[AS8(OBJECT(current_object)->projected_y_int)];
		frame_index = AT8(OBJECT(current_object)->projected_y, 1);
		if (OBJECT(object_cursor)->ranking_row.row == 1u)
			frame_index += 0x14u;
		OBJECT(current_object)->sprite_frame = mem_read16(portrait_frames + frame_index * 2u);
		if (OBJECT(object_cursor)->ranking_row.animate != 0u && (int8_t)AT8(OBJECT(current_object)->projected_y, 1) < 0x13)
			AT8(OBJECT(current_object)->projected_y, 1)++;
	}
	else
	{
		direction = OBJECT(current_object)->direction;
		OBJECT(current_object)->direction = direction - 1u;
		if (direction == 0u)
		{
			OBJECT(current_object)->direction = 1u;
			goto ranking_row_set_portrait_frame;
		}
	}
	bucket = 0u;
	queue_sprite_in_bucket(bucket);
	current_object = LINKED(current_object, OBJECT(current_object)->child);
	OBJECT(current_object)->screen_y = OBJECT(object_cursor)->screen_y;
	bucket = 0u;
	queue_sprite_in_bucket(bucket);
	current_object = LINKED(current_object, OBJECT(current_object)->child);
	OBJECT(current_object)->screen_y = OBJECT(object_cursor)->screen_y;
	bucket = 0u;
	queue_sprite_in_bucket(bucket);
	current_object = LINKED(current_object, OBJECT(current_object)->child);
	OBJECT(current_object)->screen_y = OBJECT(object_cursor)->screen_y;
	bucket = 0u;
	queue_sprite_in_bucket(bucket);
	current_object = LINKED(current_object, OBJECT(current_object)->child);
	OBJECT(current_object)->screen_y = OBJECT(object_cursor)->screen_y;
	bucket = 0u;
	queue_sprite_in_bucket(bucket);
	current_object = LINKED(current_object, OBJECT(current_object)->child);
	OBJECT(current_object)->screen_y = OBJECT(object_cursor)->screen_y;
	bucket = 0u;
	queue_sprite_in_bucket(bucket);
}

void start_character_chooser(uint32_t r11)
{
	uint16_t chooser_x = 0;
	uint32_t r0 = 0, r2 = 0, character = 0, frame_index = 0, ignored = 0, object = 0, piece_count = 0, player = 0, x_index = 0;

	count_free_characters(&r0, &r2, &object);
	players_choosing_character++;
	if (character_select_timer <= 0)
	{
		mem_write16(video_layer_disable, 0x2Fu);
		mem_write16(video_backdrop_control, 0u);
		show_nbg0_picture(&r2, &ignored, &ignored, &r11);
		spawn_count = 2u;
		spawn_count = 0xAu;
		spawn_object_with_pieces(0x08F9E2u, &r0, &piece_count, &r2, &object, &r11);
		OBJECT(object)->screen_x = 0x118;
		OBJECT(object)->screen_y = 0x1F;
		OBJECT(object)->sprite_palette = 0x8AD0u;
		OBJECT(object)->handler = character_select_countdown_update_ADDRESS;
		object = LINKED(object, OBJECT(object)->child);
		OBJECT(object)->screen_x = 0x90;
		OBJECT(object)->screen_y = 0x20;
		OBJECT(object)->sprite_frame = 0xB708u;
		OBJECT(object)->sprite_palette = 0x8D90u;
		AS8(OBJECT(object)->projected_y_int) = 1u;
	}
	character_select_timer = 0x258;
	spawn_count = 8u;
	spawn_object_with_pieces(0x08FA36u, &r0, &piece_count, &r2, &object, &r11);
	AS32(OBJECT(object)->hitbox_size_x) = current_player;
	player = OBJECT(object_cursor)->player;
	OBJECT(object)->kind = player;
	character = player;
	frame_index = character * 4u;
	while ((characters_taken & shl32(1u, (int)(character_taken_bits[character]))) != 0u)
	{
		character = (uint8_t)(character + 1u);
		frame_index = character * 4u;
		if ((int8_t)character <= 2)
			continue;
		character = 0u;
		frame_index &= ~0xFFu;
	}
	OBJECT(object)->sprite_frame = chooser_portrait_frames[frame_index];
	OBJECT(object)->sprite_palette = chooser_portrait_palettes[character];
	x_index = player;
	if ((cabinet_config & 1u) == 0u)
		x_index += 3u;
	chooser_x = chooser_x_by_player[x_index];
	OBJECT(object)->screen_x = chooser_x;
	OBJECT(object)->screen_y = 0xA0;
	OBJECT(object)->sprite_flags |= 0x100u;
	OBJECT(object)->var_81 = player;
	AT8(OBJECT(object)->hitbox_size_y, 1) = 1u;
	OBJECT(object)->handler = character_chooser_wait_ADDRESS;
	OBJECT(object)->spawn_arg_1 = OBJECT(object)->screen_x;
	OBJECT(object)->spawn_arg_2 = OBJECT(object)->screen_y;
	frame_index = (frame_index & ~0xFFu) | (uint8_t)(frame_index + 4u);
	if ((int8_t)frame_index > 8)
		frame_index &= ~0xFFu;
	character = (uint8_t)(character + 1u);
	character = character % 3u;
	object = LINKED(object, OBJECT(object)->child);
	OBJECT(object)->sprite_flags |= 0x100u;
	OBJECT(object)->sprite_frame = chooser_portrait_frames[frame_index];
	OBJECT(object)->sprite_palette = chooser_portrait_palettes[character];
	OBJECT(object)->screen_x = chooser_x;
	OBJECT(object)->screen_x -= 0x18u;
	OBJECT(object)->screen_y = 0x88;
	AS8(OBJECT(object)->projected_y_int) = character;
	OBJECT(object)->sprite_scale = 0xC0u;
	frame_index = (frame_index & ~0xFFu) | (uint8_t)(frame_index + 4u);
	if ((int8_t)frame_index > 8)
		frame_index &= ~0xFFu;
	character = (uint8_t)(character + 1u);
	character = character % 3u;
	object = LINKED(object, OBJECT(object)->child);
	OBJECT(object)->sprite_flags |= 0x100u;
	OBJECT(object)->sprite_frame = chooser_portrait_frames[frame_index];
	OBJECT(object)->sprite_palette = chooser_portrait_palettes[character];
	OBJECT(object)->screen_x = chooser_x;
	OBJECT(object)->screen_x += 0x18u;
	OBJECT(object)->screen_y = 0x88;
	AS8(OBJECT(object)->projected_y_int) = character;
	OBJECT(object)->sprite_scale = 0xC0u;
	object = LINKED(object, OBJECT(object)->child);
	OBJECT(object)->sprite_palette = 0x8DB0u;
	OBJECT(object)->sprite_frame = character_name_sprites[player];
	OBJECT(object)->screen_x = chooser_x;
	OBJECT(object)->screen_y = 0xB8;
	object = LINKED(object, OBJECT(object)->child);
	OBJECT(object)->sprite_frame = chooser_player_labels[player];
	OBJECT(object)->sprite_palette = 0x8DA0u;
	OBJECT(object)->screen_x = chooser_x;
	OBJECT(object)->screen_y = 0xD0;
	object = LINKED(object, OBJECT(object)->child);
	OBJECT(object)->sprite_flags = 8u;
	OBJECT(object)->sprite_frame = 0xB740u;
	OBJECT(object)->sprite_palette = 0x8DC0u;
	OBJECT(object)->screen_x = chooser_x;
	OBJECT(object)->screen_y = 0x30;
}

void character_chooser_wait(void)
{
	uint8_t left_turns = 0, right_turns = 0, turn = 0;
	uint16_t trackball_x = 0;
	uint32_t r1 = 0, idle_frame = 0, kind = 0, projected_y_int = 0, taken_bit = 0;
	uint8_t right_steps = 0;
	uint16_t step_timer = 0;

	if (free_character_count == 1u || character_select_timer < 0x1E)
	{
		character_chooser_confirm();
		return;
	}
	kind = OBJECT(current_object)->kind;
	if ((inputs_pressed & shl32(1u, (int)(chooser_button_bits[kind]))) != 0u)
	{
		character_chooser_confirm();
		return;
	}
	trackball_x = RAM16(GAME_VARIABLES + chooser_trackball_offsets[kind]);
	if (trackball_x == 0u)
	{
	character_chooser_idle:
		r1 = character_taken_bits[OBJECT(current_object)->character_chooser.character];
		if ((characters_taken & shl32(1u, (int)(r1))) != 0u)
		{
			OBJECT(current_object)->character_chooser.step_timer = 0;
			OBJECT(current_object)->character_chooser.turn_timer = 0x18u;
			OBJECT(current_object)->handler = character_chooser_turn_ADDRESS;
			character_chooser_turn();
			return;
		}
		step_timer = OBJECT(current_object)->character_chooser.step_timer;
		OBJECT(current_object)->character_chooser.step_timer = step_timer - 1u;
		if (step_timer == 0u)
		{
			OBJECT(current_object)->character_chooser.step_timer = 3;
			r1 = OBJECT(current_object)->character_chooser.idle_step;
			idle_frame = chooser_idle_steps[r1];
			OBJECT(current_object)->sprite_frame = chooser_portrait_frames[(uint8_t)(idle_frame + OBJECT(current_object)->character_chooser.character * 4u)];
			OBJECT(current_object)->character_chooser.idle_step++;
			OBJECT(current_object)->character_chooser.idle_step &= 3u;
		}
		queue_sprite(r1);
		r14 = 2u;
		do
		{
			current_object = LINKED(current_object, OBJECT(current_object)->child);
			projected_y_int = AS8(OBJECT(current_object)->projected_y_int);
			taken_bit = character_taken_bits[projected_y_int];
			if ((characters_taken & shl32(1u, (int)(taken_bit))) != 0u)
			{
				OBJECT(current_object)->sprite_palette = chooser_portrait_palettes[projected_y_int];
				OBJECT(current_object)->sprite_palette = 0x87A0u;
			}
			queue_sprite(taken_bit);
			r14 = r14 - 1u;
		} while (r14 != 0u);
		draw_character_chooser_labels();
		return;
	}
	if ((int16_t)trackball_x < 0)
	{
		OBJECT(current_object)->character_chooser.right_count = 0u;
		OBJECT(current_object)->character_chooser.left_count += 8u;
	}
	else
	{
		OBJECT(current_object)->character_chooser.right_count += 8u;
		OBJECT(current_object)->character_chooser.left_count = 0u;
	}
	right_turns = AT8(OBJECT(current_object)->character_chooser.right_count, 1);
	left_turns = AT8(OBJECT(current_object)->character_chooser.left_count, 1);
	AT8(OBJECT(current_object)->character_chooser.left_count, 1) = 0u;
	AT8(OBJECT(current_object)->character_chooser.right_count, 1) = 0u;
	right_steps = right_turns;
	turn = (uint8_t)(right_steps - left_turns);
	if (right_steps == left_turns)
		goto character_chooser_idle;
	OBJECT(current_object)->character_chooser.turn_direction = turn;
	OBJECT(current_object)->character_chooser.step_timer = 0;
	OBJECT(current_object)->character_chooser.turn_timer = 0x18u;
	OBJECT(current_object)->handler = character_chooser_turn_ADDRESS;
	character_chooser_turn();
}

void character_chooser_confirm(void)
{
	uint8_t sound_id = 0;
	uint32_t r2 = 0, character = 0, chosen_character = 0, frame_index = 0, ignored = 0, random_value = 0, x_index = 0;

	while ((characters_taken & shl32(1u, (int)(character_taken_bits[OBJECT(current_object)->character_chooser.character]))) != 0u)
	{
		OBJECT(current_object)->character_chooser.character++;
		OBJECT(current_object)->character_chooser.character = OBJECT(current_object)->character_chooser.character % 3u;
	}
	character = OBJECT(current_object)->character_chooser.character;
	OBJECT(LINKED(current_object, OBJECT(current_object)->parent))->kind = character;
	characters_taken |= shl32(1u, (int)(character_taken_bits[character]));
	free_character_count--;
	players_choosing_character--;
	if ((int8_t)players_choosing_character < 1)
		character_select_timer = 0x3C;
	OBJECT(current_object)->sprite_scale = 0x100u;
	x_index = OBJECT(current_object)->kind;
	if ((cabinet_config & 1u) == 0u)
		x_index += 3u;
	OBJECT(current_object)->screen_x = chooser_x_by_player[x_index];
	OBJECT(current_object)->screen_y = 0xA0;
	frame_index = OBJECT(current_object)->character_chooser.character;
	chosen_character = frame_index;
	frame_index = (uint8_t)(frame_index * 4u);
	OBJECT(current_object)->sprite_frame = chooser_portrait_frames[(uint8_t)(frame_index + 3u)];
	OBJECT(current_object)->sprite_palette = chooser_portrait_palettes[chosen_character];
	OBJECT(current_object)->handler = character_chooser_chosen_ADDRESS;
	next_random(&random_value, &ignored, &ignored);
	sound_id = mem_read8(chooser_voice_sounds_ADDRESS + (OBJECT(current_object)->character_chooser.character * 4u + (random_value & 3u)));
	play_sound(sound_id, &chosen_character, &r2);
	sound_id = 0x20u;
	play_sound(sound_id, &chosen_character, &r2);
	character_chooser_chosen();
}

void draw_character_chooser_labels(void)
{
	uint32_t depth = 0;

	current_object = LINKED(current_object, OBJECT(current_object)->child);
	if (OBJECT(object_cursor)->draw_character_chooser_labels.name_changed != 0u)
		OBJECT(current_object)->sprite_frame = character_name_sprites[OBJECT(object_cursor)->draw_character_chooser_labels.character];
	depth = queue_sprite(0u);
	current_object = LINKED(current_object, OBJECT(current_object)->child);
	depth = queue_sprite(depth);
	current_object = LINKED(current_object, OBJECT(current_object)->child);
	queue_sprite(depth);
	OBJECT(object_cursor)->draw_character_chooser_labels.name_changed = 0u;
}

void character_chooser_turn(void)
{
	uint16_t turn_step = 0;
	uint32_t r1 = 0, character = 0, idle_frame = 0, projected_y_int = 0, taken_bit = 0, x_index = 0;
	uint8_t next_character = 0;
	uint16_t turn_timer = 0, step_timer = 0, scale_step = 0;

	for (;;)
	{
		if (free_character_count == 1u)
		{
			character_chooser_confirm();
			return;
		}
		if (character_select_timer < 0x1E)
			character_select_timer = 0x1D;
		turn_timer = OBJECT(current_object)->character_chooser.turn_timer;
		OBJECT(current_object)->character_chooser.turn_timer = turn_timer - 1u;
		if (turn_timer != 0u)
			break;
		x_index = OBJECT(current_object)->kind;
		if ((cabinet_config & 1u) == 0u)
			x_index += 3u;
		OBJECT(current_object)->screen_x = chooser_x_by_player[x_index];
		OBJECT(current_object)->screen_y = 0xA0;
		OBJECT(current_object)->sprite_scale = 0x100u;
		next_character = OBJECT(current_object)->character_chooser.character + OBJECT(current_object)->character_chooser.turn_direction;
		OBJECT(current_object)->character_chooser.character = next_character;
		if ((int8_t)next_character < 0)
			OBJECT(current_object)->character_chooser.character = 2u;
		OBJECT(current_object)->character_chooser.character = OBJECT(current_object)->character_chooser.character % 3u;
		current_object = LINKED(current_object, OBJECT(current_object)->child);
		OBJECT(current_object)->screen_x = OBJECT(object_cursor)->screen_x;
		OBJECT(current_object)->screen_x -= 0x18u;
		OBJECT(current_object)->screen_y = 0x88;
		OBJECT(current_object)->sprite_scale = 0xC0u;
		AS8(OBJECT(current_object)->projected_y_int) = OBJECT(object_cursor)->character_chooser.character;
		AS8(OBJECT(current_object)->projected_y_int)++;
		AS8(OBJECT(current_object)->projected_y_int) = AS8(OBJECT(current_object)->projected_y_int) % 3u;
		projected_y_int = AS8(OBJECT(current_object)->projected_y_int);
		OBJECT(current_object)->sprite_frame = chooser_portrait_frames[projected_y_int * 4u];
		OBJECT(current_object)->sprite_palette = chooser_portrait_palettes[projected_y_int];
		if ((characters_taken & shl32(1u, (int)(character_taken_bits[projected_y_int]))) != 0u)
		{
			OBJECT(current_object)->sprite_palette = chooser_portrait_palettes[projected_y_int];
			OBJECT(current_object)->sprite_palette = 0x87A0u;
		}
		current_object = LINKED(current_object, OBJECT(current_object)->child);
		OBJECT(current_object)->screen_x = OBJECT(object_cursor)->screen_x;
		OBJECT(current_object)->screen_x += 0x18u;
		OBJECT(current_object)->screen_y = 0x88;
		OBJECT(current_object)->sprite_scale = 0xC0u;
		AS8(OBJECT(current_object)->projected_y_int) = OBJECT(object_cursor)->character_chooser.character;
		AS8(OBJECT(current_object)->projected_y_int) += 2u;
		AS8(OBJECT(current_object)->projected_y_int) = AS8(OBJECT(current_object)->projected_y_int) % 3u;
		projected_y_int = AS8(OBJECT(current_object)->projected_y_int);
		OBJECT(current_object)->sprite_frame = chooser_portrait_frames[projected_y_int * 4u];
		OBJECT(current_object)->sprite_palette = chooser_portrait_palettes[projected_y_int];
		if ((characters_taken & shl32(1u, (int)(character_taken_bits[projected_y_int]))) != 0u)
		{
			OBJECT(current_object)->sprite_palette = chooser_portrait_palettes[projected_y_int];
			OBJECT(current_object)->sprite_palette = 0x87A0u;
		}
		current_object = object_cursor;
		character = OBJECT(current_object)->character_chooser.character;
		OBJECT(current_object)->sprite_frame = chooser_portrait_frames[character * 4u];
		OBJECT(current_object)->sprite_palette = chooser_portrait_palettes[character];
		r1 = character_taken_bits[character];
		if ((characters_taken & shl32(1u, (int)(r1))) == 0u)
		{
			OBJECT(current_object)->character_chooser.name_changed = 1u;
			OBJECT(current_object)->handler = character_chooser_wait_ADDRESS;
			r1 = character_taken_bits[OBJECT(current_object)->character_chooser.character];
			if ((characters_taken & shl32(1u, (int)(r1))) != 0u)
			{
			character_chooser_start_turn:
				OBJECT(current_object)->character_chooser.step_timer = 0;
				OBJECT(current_object)->character_chooser.turn_timer = 0x18u;
				OBJECT(current_object)->handler = character_chooser_turn_ADDRESS;
				continue;
			}
			step_timer = OBJECT(current_object)->character_chooser.step_timer;
			OBJECT(current_object)->character_chooser.step_timer = step_timer - 1u;
			if (step_timer == 0u)
			{
				OBJECT(current_object)->character_chooser.step_timer = 3;
				r1 = OBJECT(current_object)->character_chooser.idle_step;
				idle_frame = chooser_idle_steps[r1];
				OBJECT(current_object)->sprite_frame = chooser_portrait_frames[(uint8_t)(idle_frame + OBJECT(current_object)->character_chooser.character * 4u)];
				OBJECT(current_object)->character_chooser.idle_step++;
				OBJECT(current_object)->character_chooser.idle_step &= 3u;
			}
			queue_sprite(r1);
			r14 = 2u;
			do
			{
				current_object = LINKED(current_object, OBJECT(current_object)->child);
				projected_y_int = AS8(OBJECT(current_object)->projected_y_int);
				taken_bit = character_taken_bits[projected_y_int];
				if ((characters_taken & shl32(1u, (int)(taken_bit))) != 0u)
				{
					OBJECT(current_object)->sprite_palette = chooser_portrait_palettes[projected_y_int];
					OBJECT(current_object)->sprite_palette = 0x87A0u;
				}
				queue_sprite(taken_bit);
				r14 = r14 - 1u;
			} while (r14 != 0u);
			draw_character_chooser_labels();
			return;
		}
		OBJECT(current_object)->sprite_palette = chooser_portrait_palettes[character];
		OBJECT(current_object)->sprite_palette = 0x87A0u;
		goto character_chooser_start_turn;
	}
	r14 = OBJECT(current_object)->character_chooser.turn_direction;
	turn_step = (uint16_t)r14;
	OBJECT(current_object)->screen_x += turn_step;
	OBJECT(current_object)->screen_y--;
	scale_step = r14 * 2u;
	r14 = scale_step;
	if ((int16_t)scale_step >= 0)
		OBJECT(current_object)->sprite_scale -= r14;
	else
		OBJECT(current_object)->sprite_scale += r14;
	r1 = queue_sprite(r1);
	current_object = LINKED(current_object, OBJECT(current_object)->child);
	if ((uint8_t)turn_step == 1u)
	{
		OBJECT(current_object)->screen_x += turn_step;
		OBJECT(current_object)->screen_y += turn_step;
		OBJECT(current_object)->sprite_scale += r14;
	}
	else
		OBJECT(current_object)->screen_x -= r14;
	r1 = queue_sprite(r1);
	current_object = LINKED(current_object, OBJECT(current_object)->child);
	if ((uint8_t)turn_step == 0xFFu)
	{
		OBJECT(current_object)->screen_x += turn_step;
		OBJECT(current_object)->screen_y -= turn_step;
		OBJECT(current_object)->sprite_scale -= r14;
	}
	else
		OBJECT(current_object)->screen_x -= r14;
	queue_sprite(r1);
	draw_character_chooser_labels();
}

void character_chooser_chosen(void)
{
	uint32_t r2 = 0, r3 = 0, depth = 0, ignored = 0;

	if (character_select_timer <= 0)
	{
		delete_object_and_children(&ignored, &r2, &r3);
		return;
	}
	depth = queue_sprite(0u);
	current_object = LINKED(current_object, OBJECT(current_object)->child);
	current_object = LINKED(current_object, OBJECT(current_object)->child);
	current_object = LINKED(current_object, OBJECT(current_object)->child);
	OBJECT(current_object)->sprite_frame = character_name_sprites[OBJECT(object_cursor)->character_chooser.character];
	depth = queue_sprite(depth);
	current_object = LINKED(current_object, OBJECT(current_object)->child);
	depth = queue_sprite(depth);
	current_object = LINKED(current_object, OBJECT(current_object)->child);
	queue_sprite(depth);
}

void character_select_countdown_update(void)
{
	uint32_t r2 = 0, r3 = 0, depth = 0, ignored = 0, seconds = 0;
	uint16_t time_left = 0, direction = 0;

	if (players_choosing_character == 0u && character_select_timer <= 0)
	{
		delete_object_and_children(&ignored, &r2, &r3);
		return;
	}
	time_left = character_select_timer;
	character_select_timer = time_left - 1u;
	if (time_left == 0u)
		character_select_timer = 1;
	seconds = (uint16_t)character_select_timer;
	seconds = (uint16_t)((int16_t)seconds / 0x3C);
	if (OBJECT(current_object)->character_select_countdown.shown_seconds != (uint8_t)seconds)
	{
		OBJECT(current_object)->character_select_countdown.shown_seconds = seconds;
		OBJECT(current_object)->sprite_frame = ranking_letter_sprites[(uint16_t)(seconds + 0x20u)];
		OBJECT(current_object)->sprite_scale = 0x10u;
	}
	if ((int16_t)OBJECT(current_object)->sprite_scale < 0x140)
		OBJECT(current_object)->sprite_scale += 8u;
	depth = queue_sprite(0u);
	current_object = LINKED(current_object, OBJECT(current_object)->child);
	direction = OBJECT(current_object)->direction;
	OBJECT(current_object)->direction = direction - 1u;
	if (direction != 0u)
	{
		queue_sprite(depth);
		return;
	}
	OBJECT(current_object)->direction = 2u;
	OBJECT(current_object)->sprite_palette = player_select_title_palettes[AT8(OBJECT(current_object)->projected_y, 1)];
	AT8(OBJECT(current_object)->projected_y, 1) += AS8(OBJECT(current_object)->projected_y_int);
	if ((int8_t)AT8(OBJECT(current_object)->projected_y, 1) > 0xC)
	{
		AS8(OBJECT(current_object)->direction) = 8u;
		AT8(OBJECT(current_object)->projected_y, 1) = 0xCu;
		AS8(OBJECT(current_object)->projected_y_int) = 0xFFu;
	}
	if ((int8_t)AT8(OBJECT(current_object)->projected_y, 1) < 0)
	{
		AS8(OBJECT(current_object)->direction) = 8u;
		AT8(OBJECT(current_object)->projected_y, 1) = 0u;
		AS8(OBJECT(current_object)->projected_y_int) = 1u;
	}
	queue_sprite(depth);
}

uint32_t start_join_character_chooser(void)
{
	uint16_t chooser_x = 0;
	uint32_t block = 0, character = 0, ignored = 0, object = 0, player = 0, x_index = 0;

	count_free_characters(&x_index, &ignored, &ignored);
	if (free_character_count == 0u)
		return x_index;
	character = OBJECT(object_cursor)->kind;
	player = OBJECT(object_cursor)->player;
	block = join_chooser_blocks[player];
	object = block;
	init_object(object);
	while ((characters_taken & shl32(1u, (int)(mem_read8(character_taken_bits_ADDRESS + character)))) != 0u)
	{
		character = ((character & ~0xFFu) | (uint8_t)(character + 1u)) % 3u;
	}
	mem_write8(block + 0x47u, player);
	AS32(players_in_join_chooser) |= shl32(1u, (int)(player));
	mem_write32(block + 0x10u, current_player);
	x_index = player;
	if ((cabinet_config & 1u) == 0u)
		x_index += 3u;
	chooser_x = chooser_x_by_player[x_index];
	mem_write16(block + 0xEu, chooser_x);
	mem_write16(block + 0xCu, 0xCCu);
	mem_write16(block + 0x82u, 0x21Cu);
	mem_write16(block + 2u, mem_read16(block + 2u) | 0x100u);
	mem_write16(block + 4u, join_chooser_portraits[character]);
	mem_write16(block + 6u, join_chooser_palettes[character]);
	mem_write8(block + 0x81u, character);
	mem_write32(block + 0x1Cu, join_character_chooser_update_ADDRESS);
	mem_write8(block + 0x13u, 1u);
	mem_write16(block + 0x28u, block);
	mem_write16(block + 0x28u, mem_read16(block + 0x28u) + 0xA0u);
	block = (block & ~0xFFFFu) | mem_read16(block + 0x28u);
	object = block;
	init_object_2(object);
	mem_write16(block + 0xEu, chooser_x);
	mem_write16(block + 0xCu, 0xDDu);
	mem_write16(block + 4u, character_name_sprites[character]);
	mem_write16(block + 6u, 0x8DB0u);
	mem_write8(block + 0x2Eu, character);
	return x_index;
}

void join_character_chooser_update(uint32_t r0)
{
	uint8_t left_steps = 0, right_turns = 0, turn = 0;
	uint16_t trackball_x = 0, y = 0;
	uint32_t character = 0, kind = 0, palette = 0, player = 0, scale = 0, taken_bit = 0, text = 0, text_x = 0;
	uint8_t right_steps = 0, next_character = 0;
	uint16_t time_left = 0;

	player = OBJECT(object_cursor)->player;
	if ((AS32(players_in_join_chooser) & shl32(1u, (int)(player))) == 0u)
		return;
	current_object = join_chooser_blocks[player];
	time_left = OBJECT(current_object)->var_82;
	OBJECT(current_object)->var_82 = time_left - 1u;
	if (time_left == 0u)
	{
		for (;;)
		{
		join_character_chooser_take_character:
			character = OBJECT(current_object)->var_81;
			OBJECT(object_cursor)->kind = character;
			taken_bit = character_taken_bits[character];
			if ((characters_taken & shl32(1u, (int)(taken_bit))) == 0u)
				break;
			OBJECT(current_object)->var_81++;
			OBJECT(current_object)->var_81 = OBJECT(current_object)->var_81 % 3u;
		}
		characters_taken |= shl32(1u, (int)(taken_bit));
		AS32(players_in_join_chooser) &= ~shl32(1u, (int)(OBJECT(current_object)->kind));
	}
	else
	{
		text_x = (r0 & ~0xFFFFu) | (uint16_t)OBJECT(current_object)->screen_x;
		text_x = (text_x & ~0xFFFFu) | (uint16_t)(text_x - 0x25u);
		y = (uint16_t)(OBJECT(current_object)->screen_y - 0x13u);
		palette = 0x8AD0u;
		scale = 0x140u;
		text = 0x8F08Eu + (int32_t)OBJECT(current_object)->var_82 / 0x3C * 3u;
		queue_string_at(text_x, y, &palette, &scale, &text);
		text_x = (text_x & ~0xFFFFu) | (uint16_t)OBJECT(current_object)->screen_x;
		text_x = (text_x & ~0xFFFFu) | (uint16_t)(text_x + 9u);
		y = (uint16_t)(OBJECT(current_object)->screen_y - 0x13u);
		queue_string_at(text_x, y, &palette, &scale, &text);
		if (joining_closed != 0u)
			goto join_character_chooser_take_character;
		kind = OBJECT(current_object)->kind;
		if ((inputs_pressed & shl32(1u, (int)(chooser_button_bits[kind]))) != 0u)
			goto join_character_chooser_take_character;
		trackball_x = RAM16(GAME_VARIABLES + chooser_trackball_offsets[kind]);
		if (trackball_x != 0u)
		{
			if ((int16_t)trackball_x < 0)
			{
				OBJECT(current_object)->spawn_arg_1 = 0u;
				OBJECT(current_object)->spawn_arg_2 += 8u;
			}
			else
			{
				OBJECT(current_object)->spawn_arg_1 += 8u;
				OBJECT(current_object)->spawn_arg_2 = 0u;
			}
			right_turns = AT8(OBJECT(current_object)->spawn_arg_1, 1);
			left_steps = AT8(OBJECT(current_object)->spawn_arg_2, 1);
			AT8(OBJECT(current_object)->spawn_arg_2, 1) = 0u;
			AT8(OBJECT(current_object)->spawn_arg_1, 1) = 0u;
			right_steps = right_turns;
			turn = (uint8_t)(right_steps - left_steps);
			if (right_steps != left_steps)
			{
				AT8(OBJECT(current_object)->hitbox_size_y, 1) = turn;
				OBJECT(current_object)->var_80 = 1u;
				next_character = OBJECT(current_object)->var_81 + turn;
				OBJECT(current_object)->var_81 = next_character;
				if ((int8_t)next_character >= 0)
					OBJECT(current_object)->var_81 = OBJECT(current_object)->var_81 % 3u;
				else
					OBJECT(current_object)->var_81 = 2u;
			}
		}
		for (;;)
		{
			character = OBJECT(current_object)->var_81;
			if ((characters_taken & shl32(1u, (int)(character_taken_bits[character]))) == 0u)
				break;
			OBJECT(current_object)->var_80 = 1u;
			OBJECT(current_object)->var_81 += AT8(OBJECT(current_object)->hitbox_size_y, 1);
			if ((int8_t)OBJECT(current_object)->var_81 < 0)
				OBJECT(current_object)->var_81 = 2u;
			OBJECT(current_object)->var_81 = OBJECT(current_object)->var_81 % 3u;
		}
		if (OBJECT(current_object)->var_80 != 0u)
		{
			OBJECT(current_object)->sprite_frame = join_chooser_portraits[character];
			OBJECT(current_object)->sprite_palette = join_chooser_palettes[character];
		}
		queue_sprite_on_top();
		OBJECT(current_object)->var_80 = 0u;
		r14 = current_object;
		current_object = LINKED(current_object, OBJECT(current_object)->child);
		if (AS8(OBJECT(current_object)->projected_y_int) != OBJECT(r14)->var_81)
		{
			AS8(OBJECT(current_object)->projected_y_int) = OBJECT(r14)->var_81;
			OBJECT(current_object)->sprite_frame = character_name_sprites[AS8(OBJECT(current_object)->projected_y_int)];
		}
		queue_sprite_on_top();
	}
	current_object = object_cursor;
}

void count_free_characters(uint32_t *p_r0, uint32_t *p_r2, uint32_t *p_r3)
{
	uint8_t free_characters = 0;
	uint32_t character = 0, characters_left = 0, taken_bit = 0;

	characters_left = 3u;
	free_characters = 0u;
	do
	{
		character = characters_left - 1u;
		taken_bit = mem_read8(character_taken_bits_ADDRESS + character);
		if ((characters_taken & shl32(1u, (int)(taken_bit))) == 0u)
			free_characters++;
		characters_left--;
	} while (characters_left != 0u);
	free_character_count = free_characters;
	*p_r0 = characters_left;
	*p_r2 = character;
	*p_r3 = taken_bit;
}

void spawn_walls(uint32_t *p_r1, uint32_t *p_r2, uint32_t *p_second_sprite, uint32_t *p_point_y, uint32_t *p_z, uint32_t *p_wall)
{
	uint32_t r1 = *p_r1;
	uint32_t r2 = *p_r2;
	uint32_t second_sprite = *p_second_sprite;
	uint32_t point_y = *p_point_y;
	uint32_t z = *p_z;
	uint32_t wall = *p_wall;
	uint16_t blocks_needed = 0, past_spawn = 0;
	uint32_t r1_2 = 0, r2_2 = 0, corner_x = 0, free_count = 0, ignored = 0, kind_record = 0, object = 0, record = 0;
	uint16_t spawn_position = 0, current_scroll = 0;
	uint32_t init_flags = 0;

	init_flags = stage_init_flags;
	stage_init_flags = init_flags | 0x20u;
	if ((init_flags & 0x20u) == 0u)
	{
		r1 = wall_spawns[stage];
		wall_spawn_cursor = wall_spawns_ADDRESS + r1;
	}
	spawn_position = mem_read16(wall_spawn_cursor);
	current_scroll = scroll_position;
	past_spawn = (uint16_t)(current_scroll - spawn_position);
	if (current_scroll < spawn_position)
	{
		*p_r1 = r1;
		return;
	}
	if ((int16_t)past_spawn >= 0x100)
	{
		wall_spawn_cursor += 0xEu;
		*p_r1 = r1;
		*p_r2 = r2;
		*p_second_sprite = (*p_second_sprite & 0xFFFF0000u) | (second_sprite & 0xFFFFu);
		*p_point_y = point_y;
		*p_z = z;
		*p_wall = wall;
		return;
	}
	allocate_object_outside_loop(&free_count, &wall);
	if (free_count == 0u)
	{
		*p_r1 = r1;
		*p_wall = wall;
		return;
	}
	record = wall_spawn_cursor + 2u;
	OBJECT(wall)->wall_update.end_position = mem_read16(record);
	record += 2u;
	OBJECT(wall)->x_int = mem_read16(record);
	OBJECT(wall)->x_fraction = 0u;
	record += 2u;
	OBJECT(wall)->y_int = mem_read16(record);
	OBJECT(wall)->y_fraction = 0u;
	record += 2u;
	OBJECT(wall)->z_int = mem_read16(record);
	OBJECT(wall)->z_fraction = 0u;
	OBJECT(wall)->floor_z = OBJECT(wall)->z;
	record += 2u;
	OBJECT(wall)->sprite_frame = mem_read16(record);
	record += 2u;
	OBJECT(wall)->speed_x_fraction = mem_read8(record);
	OBJECT(wall)->wall_update.depth_from_next = mem_read8(record + 1u);
	OBJECT(wall)->handler = wall_update_ADDRESS;
	OBJECT(wall)->sprite_flags = 0x1004u;
	OBJECT(wall)->hitbox_height = 0x100u;
	OBJECT(wall)->flags = 0u;
	AS8(OBJECT(wall)->speed_y) = 0u;
	current_object = wall;
	blocks_needed = OBJECT(current_object)->speed_x_fraction;
	if ((int16_t)ROM16(wall_kind_second_sprites_ADDRESS + OBJECT(current_object)->sprite_frame) >= 0)
		blocks_needed = (uint16_t)(blocks_needed * 2u);
	if ((int16_t)blocks_needed < free_object_stack_2[0])
	{
		build_wall_parts(&r2, &second_sprite, &wall);
		kind_record = OBJECT(current_object)->sprite_frame + wall_kinds_ADDRESS;
		OBJECT(current_object)->hitbox_offset_x = ROM8(kind_record);
		kind_record++;
		OBJECT(current_object)->hitbox_offset_y = ROM8(kind_record);
		kind_record++;
		OBJECT(current_object)->sprite_offset_x = ROM8(kind_record);
		kind_record++;
		OBJECT(current_object)->sprite_offset_y = ROM8(kind_record);
		OBJECT(current_object)->direction = ROM8(kind_record + 1u);
		project_object(&ignored, &ignored);
		wall_corner_x = OBJECT(current_object)->x + ((uint32_t)OBJECT(current_object)->hitbox_size_x << 16);
		wall_corner_y = OBJECT(current_object)->y + ((uint32_t)OBJECT(current_object)->hitbox_size_y << 16);
		z = OBJECT(current_object)->z;
		corner_x = wall_corner_x;
		point_y = wall_corner_y;
		project_point(z, &r1, &corner_x, &point_y);
		OBJECT(current_object)->speed_z_fraction = corner_x - OBJECT(current_object)->screen_x;
		point_y = (point_y & ~0xFFFFu) | (uint16_t)(point_y - OBJECT(current_object)->screen_y);
		OBJECT(current_object)->speed_z_int = point_y;
		wall_spawn_cursor += 0xEu;
		*p_r1 = r1;
		*p_r2 = r2;
		*p_second_sprite = (*p_second_sprite & 0xFFFF0000u) | (second_sprite & 0xFFFFu);
		*p_point_y = point_y;
		*p_z = z;
		*p_wall = wall;
		return;
	}
	object = current_object;
	delete_object_outside_loop(object, &r1_2, &r2_2);
	*p_r1 = r1_2;
	*p_r2 = r2_2;
	*p_wall = wall;
}

void wall_update(void)
{
	uint8_t bucket = 0;
	uint16_t child = 0, depth_bias = 0, part_y = 0;
	uint32_t r1 = 0, depth_y = 0, ignored = 0, off_screen = 0;

	update_screen_position(&ignored, &r1);
	if (map_screen_active != 0u)
	{
		for (;;)
		{
		wall_update_remove:
			if ((int16_t)OBJECT(current_object)->child < 0)
				break;
			child = OBJECT(current_object)->child;
			free_object_2(child);
			current_object = LINKED(current_object, OBJECT(current_object)->child);
		}
		delete_current_object();
		return;
	}
	if (OBJECT(current_object)->wall_update.end_position == 0u)
	{
		off_screen = check_wall_left_screen();
		if (off_screen == 0u)
		{
		wall_update_list_and_draw:
			add_to_wall_list();
			if (OBJECT(current_object)->wall_update.invisible != 0u)
				return;
			r14 = (uint16_t)OBJECT(current_object)->screen_x;
			part_y = (uint16_t)OBJECT(current_object)->screen_y;
			wall_sprite_offset_x = (uint8_t)OBJECT(current_object)->sprite_offset_x;
			depth_bias = 0x10u;
			if (OBJECT(current_object)->wall_update.depth_from_next == 0u)
				depth_bias = (uint16_t)(0u - depth_bias);
			current_player = current_object;
			for (;;)
			{
				current_object = LINKED(current_object, OBJECT(current_object)->child);
				if ((int16_t)current_object < 0)
					break;
				OBJECT(current_object)->screen_x = r14;
				r14 = (uint16_t)(r14 + (int8_t)AS8(OBJECT(current_object)->hitbox_size_y));
				OBJECT(current_object)->screen_y = part_y;
				depth_y = part_y;
				part_y += (int8_t)AT8(OBJECT(current_object)->hitbox_size_y, 1);
				if ((int16_t)(r14 - wall_sprite_offset_x) <= -0x20 || (int16_t)(OBJECT(current_object)->screen_x - wall_sprite_offset_x) > 0x160)
				{
					if (OBJECT(object_cursor)->wall_update.no_second_sprite != 0u)
						continue;
					current_object = LINKED(current_object, OBJECT(current_object)->child);
					continue;
				}
				if ((int16_t)depth_bias >= 0)
					depth_y = part_y;
				depth_y = (uint16_t)(depth_y - depth_bias);
				queue_sprite_at_depth(&ignored, &depth_y);
				if (OBJECT(object_cursor)->wall_update.no_second_sprite != 0u)
					continue;
				current_player = LINKED(current_player, OBJECT(current_object)->child);
				OBJECT(current_player)->screen_x = (int8_t)AS8(OBJECT(current_player)->hitbox_size_y) + OBJECT(current_object)->screen_x;
				OBJECT(current_player)->screen_y = (int8_t)AT8(OBJECT(current_player)->hitbox_size_y, 1) + OBJECT(current_object)->screen_y;
				current_object = (current_object & ~0xFFFFu) | (uint16_t)current_player;
				bucket = 0xFFu;
				queue_sprite_in_bucket(bucket);
			}
			return;
		}
		goto wall_update_remove;
	}
	if (OBJECT(current_object)->wall_update.end_position <= scroll_position)
		goto wall_update_remove;
	goto wall_update_list_and_draw;
}

uint32_t check_wall_left_screen(void)
{
	uint32_t off_screen = 0;

	off_screen = 0u;
	if ((uint16_t)(OBJECT(current_object)->screen_x - OBJECT(current_object)->sprite_offset_x + 0x10u) >= 0x160u || (uint16_t)(OBJECT(current_object)->screen_y - OBJECT(current_object)->sprite_offset_y + 0x20u) >= 0x120u)
	{
		if ((OBJECT(current_object)->speed_y & 1u) != 0u)
			OBJECT(current_object)->speed_y |= 2u;
	}
	else
		OBJECT(current_object)->speed_y |= 1u;
	if ((uint16_t)(OBJECT(current_object)->screen_x + OBJECT(current_object)->speed_z_fraction + 0x10u) >= 0x160u || (uint16_t)(OBJECT(current_object)->screen_y + OBJECT(current_object)->speed_z_int + 0x20u) >= 0x120u)
	{
		if ((OBJECT(current_object)->speed_y & 4u) != 0u)
			OBJECT(current_object)->speed_y |= 8u;
	}
	else
		OBJECT(current_object)->speed_y |= 4u;
	if ((OBJECT(current_object)->speed_y & 2u) != 0u && (OBJECT(current_object)->speed_y & 8u) != 0u)
		off_screen = 0xFFFFFFFFu;
	return off_screen;
}

void build_wall_parts(uint32_t *p_part_size, uint32_t *p_second_sprite, uint32_t *p_part)
{
	uint32_t part_size = *p_part_size;
	uint16_t second_sprite = (uint16_t)*p_second_sprite;
	uint32_t part = *p_part;
	uint8_t part_step_x = 0, part_step_y = 0, second_offset_x = 0;
	uint16_t part_sprite = 0;
	uint32_t r2 = 0, free_count = 0, object = 0, object_2 = 0, parts_left = 0, record = 0;

	record = OBJECT(current_object)->sprite_frame + 0x90A78u;
	OBJECT(current_object)->hitbox_size_x = ROM16(record);
	record += 2u;
	OBJECT(current_object)->hitbox_size_y = ROM16(record);
	record += 2u;
	if ((int16_t)OBJECT(current_object)->hitbox_size_x >= 0)
	{
		if ((int16_t)OBJECT(current_object)->hitbox_size_y < 0)
		{
			part_size = (part_size & ~0xFFFFu) | OBJECT(current_object)->hitbox_size_y;
			part_size = (part_size & ~0xFFFFu) | (part_size & 0x7FFFu);
			OBJECT(current_object)->hitbox_size_y = OBJECT(current_object)->speed_x_fraction * part_size;
		}
	}
	else
	{
		part_size = (part_size & ~0xFFFFu) | OBJECT(current_object)->hitbox_size_x;
		part_size = (part_size & ~0xFFFFu) | (part_size & 0x7FFFu);
		OBJECT(current_object)->hitbox_size_x = OBJECT(current_object)->speed_x_fraction * part_size;
	}
	parts_left = OBJECT(current_object)->speed_x_fraction;
	OBJECT(current_object)->sprite_palette = ROM16(record);
	record += 2u;
	part_step_x = ROM8(record);
	record++;
	part_step_y = ROM8(record);
	record++;
	part_sprite = ROM16(record);
	OBJECT(current_object)->build_wall_parts.invisible = 0u;
	record += 2u;
	if ((int16_t)part_sprite >= 0)
	{
		second_offset_x = ROM8(record);
		record++;
		current_player = (current_player & ~0xFFu) | ROM8(record);
		second_sprite = ROM16(record + 1u);
		OBJECT(current_object)->build_wall_parts.no_second_sprite = 0xFFu;
		if ((int16_t)second_sprite >= 0)
			OBJECT(current_object)->build_wall_parts.no_second_sprite = 0u;
		object_2 = current_object;
		for (;;)
		{
			allocate_object_2(&free_count, &r2, &part);
			if (free_count == 0u)
			{
				OBJECT(part)->child = 0xFFFFu;
				OBJECT(current_object)->speed_y_int = part;
				*p_part_size = r2;
				*p_second_sprite = (*p_second_sprite & 0xFFFF0000u) | second_sprite;
				*p_part = part;
				return;
			}
			OBJECT(part)->sprite_frame = part_sprite;
			AS8(OBJECT(part)->hitbox_size_y) = part_step_x;
			AT8(OBJECT(part)->hitbox_size_y, 1) = part_step_y;
			OBJECT(part)->sprite_flags = OBJECT(object_2)->sprite_flags;
			OBJECT(part)->sprite_palette = OBJECT(current_object)->sprite_palette;
			OBJECT(part)->parent = object_2;
			OBJECT(object_2)->child = part;
			object_2 = part;
			if ((int16_t)second_sprite >= 0)
			{
				allocate_object_2(&free_count, &r2, &part);
				if (free_count == 0u)
					break;
				OBJECT(part)->sprite_frame = second_sprite;
				AS8(OBJECT(part)->hitbox_size_y) = second_offset_x;
				AT8(OBJECT(part)->hitbox_size_y, 1) = current_player;
				OBJECT(part)->sprite_flags = OBJECT(object_2)->sprite_flags;
				OBJECT(part)->sprite_palette = 0x9FFEu;
				OBJECT(part)->parent = object_2;
				OBJECT(object_2)->child = part;
				object_2 = part;
			}
			parts_left--;
			if (parts_left == 0u)
			{
				OBJECT(part)->child = 0xFFFFu;
				OBJECT(current_object)->speed_y_int = part;
				*p_part_size = r2;
				*p_second_sprite = (*p_second_sprite & 0xFFFF0000u) | second_sprite;
				*p_part = part;
				return;
			}
		}
		OBJECT(part)->child = 0xFFFFu;
		OBJECT(current_object)->speed_y_int = part;
		*p_part_size = r2;
		*p_second_sprite = (*p_second_sprite & 0xFFFF0000u) | second_sprite;
		*p_part = part;
		return;
	}
	OBJECT(current_object)->build_wall_parts.invisible = 0xFFu;
	object = current_object;
	OBJECT(object)->child = 0xFFFFu;
	OBJECT(current_object)->speed_y_int = object;
	*p_part_size = part_size;
	*p_part = object;
}

void spawn_object_groups(uint32_t *p_r1, uint32_t *p_r2, uint32_t *p_slope_z, uint32_t *p_count, uint32_t *p_object)
{
	uint32_t r1 = *p_r1;
	uint32_t r2 = *p_r2;
	uint32_t slope_z = *p_slope_z;
	uint32_t count = *p_count;
	uint32_t object = *p_object;
	uint32_t floor_block_record = 0, free_count = 0, kind_index = 0, member_record = 0, record = 0, slope_number = 0, slope_record = 0, slot_cursor = 0, spawned = 0;
	uint16_t spawn_position = 0, slot_taken = 0, members_left = 0;
	uint32_t init_flags = 0;

	init_flags = stage_init_flags;
	stage_init_flags = init_flags | 0x200u;
	if ((init_flags & 0x200u) == 0u)
	{
		r1 = object_group_spawns[stage];
		group_spawn_cursor = object_group_spawns_ADDRESS + r1;
	}
	for (;;)
	{
		record = group_spawn_cursor;
		spawn_position = ROM16(record);
		record += 2u;
		if (scroll_position < spawn_position)
		{
			*p_r1 = r1;
			*p_r2 = r2;
			*p_count = count;
			*p_object = object;
			return;
		}
		AS8(group_previous_member) = 0u;
		kind_index = ROM8(record) * 2u;
		record++;
		switch (kind_index)
		{
		case 0x0u:
			goto spawn_object_groups_no_depth_step;
		case 0x2u:
		case 0x10u:
			spawn_group_leader(record, &spawned, &object);
			if (spawned == 0u)
			{
				*p_r1 = r1;
				*p_r2 = r2;
				*p_count = count;
				*p_object = object;
				return;
			}
			group_spawn_cursor += 0xEu;
			continue;
		case 0x4u:
			group_depth_step = -8;
			goto spawn_object_groups_build_group;
		case 0x6u:
			spawn_group_slot_trigger(record, &spawned, &count, &object);
			if (spawned == 0u)
			{
				*p_r1 = r1;
				*p_r2 = r2;
				*p_count = count;
				*p_object = object;
				return;
			}
			group_spawn_cursor += 0xEu;
			continue;
		case 0x8u:
			spawn_ice_platform_leader(record, &spawned, &r1, &object);
			if (spawned == 0u)
			{
				*p_r1 = r1;
				*p_r2 = r2;
				*p_count = count;
				*p_object = object;
				return;
			}
			group_spawn_cursor += 0xEu;
			continue;
		case 0xAu:
			spawn_sand_block_leader(record, &spawned, &object);
			if (spawned == 0u)
			{
				*p_r1 = r1;
				*p_r2 = r2;
				*p_count = count;
				*p_object = object;
				return;
			}
			group_spawn_cursor += 0xEu;
			continue;
		case 0xCu:
			goto spawn_object_groups_mark_silent;
		case 0xEu:
			spawn_waterfall(record, &spawned, &r2, &object);
			if (spawned == 0u)
			{
				*p_r1 = r1;
				*p_r2 = r2;
				*p_count = count;
				*p_object = object;
				return;
			}
			group_spawn_cursor += 0xEu;
			continue;
		default:
			unknown_code(0x9125Eu + kind_index);
		}
	}
spawn_object_groups_mark_silent:
	AS8(group_previous_member) = 0xFFu;
spawn_object_groups_no_depth_step:
	group_depth_step = 0;
spawn_object_groups_build_group:
	if ((uint16_t)(scroll_position - mem_read16(record - 3u)) < 0x100u)
	{
		group_leader = group_leaders[ROM8(record)];
		record++;
		group_takes_slot = ROM8(record);
		record += 2u;
		group_floor_block = ROM32(record);
		record += 4u;
		group_member_count = ROM16(record);
		record += 2u;
		if (free_object_stack < group_member_count)
		{
			*p_r1 = r1;
			*p_r2 = r2;
			*p_count = count;
			*p_object = object;
			return;
		}
		group_step_x = ROM16(record);
		record += 2u;
		group_step_y = ROM16(record);
		record += 2u;
		group_direction = ROM16(record);
		record += 2u;
		group_member_x = ROM16(record);
		record += 2u;
		group_member_y = ROM16(record);
		object = ROM16(record + 2u) + stage_object_records_ADDRESS;
		group_record = object;
		group_member_number = 0u;
		if (group_takes_slot != 0u)
		{
			slot_cursor = group_slots_ADDRESS;
			count = 0x40u;
			do
			{
				slot_taken = mem_read16(slot_cursor);
				slot_cursor += 2u;
				count--;
			} while ((count != 0u) & (slot_taken != 0u));
			mem_write16(slot_cursor - 2u, 1u);
			count = (count & ~0xFFFFu) | (uint16_t)(count + 1u);
			r1 = (r1 & ~0xFFFFu) | 0x40u;
			r1 = (r1 & ~0xFFFFu) | (uint16_t)(r1 - count);
			group_slot_index = r1;
		}
		do
		{
			allocate_object_outside_loop(&free_count, &object);
			if (free_count == 0u)
			{
				*p_r1 = r1;
				*p_r2 = r2;
				*p_slope_z = slope_z;
				*p_count = count;
				*p_object = object;
				return;
			}
			OBJECT(object)->x_int = group_member_x;
			OBJECT(object)->x_fraction = 0u;
			group_member_x += group_step_x;
			OBJECT(object)->y_int = group_member_y;
			OBJECT(object)->y_fraction = 0u;
			group_member_y += group_step_y;
			member_record = group_record;
			OBJECT(object)->sprite_flags = ROM16(member_record);
			member_record += 2u;
			OBJECT(object)->sprite_frame = ROM16(member_record);
			member_record += 2u;
			OBJECT(object)->sprite_palette = ROM16(member_record);
			member_record += 2u;
			OBJECT(object)->handler = ROM32(member_record);
			member_record += 4u;
			OBJECT(object)->animation = ROM32(member_record);
			member_record += 4u;
			OBJECT(object)->hitbox_size_x = ROM16(member_record);
			member_record += 2u;
			OBJECT(object)->hitbox_size_y = ROM16(member_record);
			member_record += 2u;
			OBJECT(object)->sprite_offset_x = ROM8(member_record);
			member_record++;
			OBJECT(object)->sprite_offset_y = ROM8(member_record);
			OBJECT(object)->var_96 = ROM16(member_record + 1u);
			OBJECT(object)->floor_block = group_floor_block;
			OBJECT(object)->direction = group_direction;
			OBJECT(object)->parent = group_leader;
			AS16(OBJECT(object)->var_98) = OBJECT(object)->sprite_palette;
			AT8(OBJECT(object)->var_90, 1) = AS8(group_previous_member);
			AT8(OBJECT(object)->speed, 1) = group_member_number;
			group_member_number++;
			OBJECT(object)->var_8A = group_slot_index;
			AS8(OBJECT(object)->spawn_arg_1) = group_takes_slot;
			OBJECT(object)->animation_frame = 0u;
			OBJECT(object)->animation_timer = 0;
			OBJECT(object)->animation_speed = 6u;
			OBJECT(object)->floor_slope = mem_read8(floor_blocks + 2u + OBJECT(object)->floor_block + 0x13u);
			OBJECT(object)->flags = 0u;
			AS8(OBJECT(object)->speed) = 0u;
			if ((uint8_t)group_depth_step != 0u)
			{
				r1 = (r1 & ~0xFFFFu) | (uint16_t)group_depth_step;
				OBJECT(object)->var_96 += group_member_count * r1;
			}
			floor_block_record = floor_blocks + 2u + OBJECT(object)->floor_block;
			OBJECT(object)->floor_z_int = mem_read16(floor_block_record + 0x10u);
			OBJECT(object)->floor_z_fraction = 0u;
			if (mem_read8(floor_block_record + 0x13u) != 0u)
			{
				slope_number = mem_read8(floor_block_record + 0x13u);
				slope_record = floor_slopes_ADDRESS + (uint16_t)(slope_number * 2u) * 4u;
				if ((ROM32(slope_record) & 2u) == 0u)
					slope_z = OBJECT(object)->y - mem_read32(floor_block_record + 8u);
				else
					slope_z = OBJECT(object)->x - mem_read32(floor_block_record);
				slope_z = ((int32_t)slope_z >> 16) * ROM32(slope_record + 2u);
				OBJECT(object)->floor_z += slope_z;
			}
			OBJECT(object)->z = OBJECT(object)->floor_z;
			members_left = group_member_count;
			group_member_count = members_left - 1u;
		} while ((int16_t)members_left > 1);
	}
	group_spawn_cursor += 0x18u;
	*p_r1 = r1;
	*p_r2 = r2;
	*p_slope_z = slope_z;
	*p_count = count;
	*p_object = object;
}

void crumbling_floor_update(void)
{
	uint8_t sound_id = 0;
	uint32_t r1 = 0, r2 = 0, at_last_step = 0, depth = 0, ignored = 0, sprite_depth = 0;

	switch (OBJECT(current_object)->crumbling_floor.state)
	{
	case 0:
	case 3:
		project_object(&ignored, &r1);
		OBJECT(current_object)->crumbling_floor.state++;
	case 1:
		update_screen_position(&ignored, &r1);
		if (grey_screen_active == 0u)
		{
			if (grey_screen_ended != 0u)
				OBJECT(current_object)->sprite_palette = OBJECT(current_object)->crumbling_floor.normal_palette;
			if ((OBJECT(current_object)->flags & 8u) != 0u && (OBJECT(LINKED(current_object, OBJECT(current_object)->platform))->flags & 0x400000u) != 0u)
				OBJECT(current_object)->flags |= 0x400000u;
			if ((OBJECT(current_object)->flags & 0x400000u) != 0u)
			{
			crumbling_floor_play_crack_steps:
				animate_with_screen_offset(0u);
				if ((int8_t)OBJECT(current_object)->animation_frame >= 4)
				{
					OBJECT(current_object)->animation_timer = -1;
					if ((OBJECT(current_object)->flags & 0x400000u) != 0u)
					{
						OBJECT(current_object)->crumbling_floor.state++;
						OBJECT(current_object)->speed_z = 0;
						OBJECT(current_object)->animation_speed = 6u;
						OBJECT(current_object)->animation_timer = 0;
						OBJECT(current_object)->animation_frame = 4u;
						if (OBJECT(current_object)->crumbling_floor.silent_row == 0u)
						{
							switch (OBJECT(current_object)->direction)
							{
							case 0:
							case 4:
								if (OBJECT(current_object)->screen_x >= 0)
								{
									sound_id = 0xDu;
									play_sound(sound_id, &r1, &r2);
								}
								break;
							case 1:
								if (OBJECT(current_object)->screen_x <= 0x180)
								{
									sound_id = 0xDu;
									play_sound(sound_id, &r1, &r2);
									add_to_platform_list();
									if ((uint16_t)(OBJECT(current_object)->screen_x + 0xA0u) >= 0x281u)
										return;
									sprite_depth = (uint16_t)(OBJECT(current_object)->screen_y + OBJECT(current_object)->crumbling_floor.depth_offset - OBJECT(current_object)->sprite_offset_y);
									OBJECT(current_object)->crumbling_floor.draw_depth = sprite_depth;
									queue_sprite_at_depth(&ignored, &sprite_depth);
									return;
								}
								break;
							case 2:
								if (OBJECT(current_object)->screen_x >= -8)
								{
									sound_id = 0xDu;
									play_sound(sound_id, &r1, &r2);
									add_to_platform_list();
									if ((uint16_t)(OBJECT(current_object)->screen_x + 0xA0u) >= 0x281u)
										return;
									sprite_depth = (uint16_t)(OBJECT(current_object)->screen_y + OBJECT(current_object)->crumbling_floor.depth_offset - OBJECT(current_object)->sprite_offset_y);
									OBJECT(current_object)->crumbling_floor.draw_depth = sprite_depth;
									queue_sprite_at_depth(&ignored, &sprite_depth);
									return;
								}
								break;
							case 3:
								if (OBJECT(current_object)->screen_x <= 0x1A0)
								{
									sound_id = 0xDu;
									play_sound(sound_id, &r1, &r2);
									add_to_platform_list();
									if ((uint16_t)(OBJECT(current_object)->screen_x + 0xA0u) >= 0x281u)
										return;
									sprite_depth = (uint16_t)(OBJECT(current_object)->screen_y + OBJECT(current_object)->crumbling_floor.depth_offset - OBJECT(current_object)->sprite_offset_y);
									OBJECT(current_object)->crumbling_floor.draw_depth = sprite_depth;
									queue_sprite_at_depth(&ignored, &sprite_depth);
									return;
								}
								break;
							default:
								unknown_code(0x9161Cu + OBJECT(current_object)->direction * 2u);
							}
						}
					}
				}
			}
			else
			{
				current_player = current_object;
				current_player = LINKED(current_player, OBJECT(current_object)->parent);
				switch (OBJECT(current_object)->direction)
				{
				case 0:
				case 4:
					if (OBJECT(current_object)->y_int <= OBJECT(current_player)->y_int)
					{
						OBJECT(current_object)->crumbling_floor.state++;
						OBJECT(current_object)->speed_z = 0;
						OBJECT(current_object)->animation_speed = 6u;
						OBJECT(current_object)->animation_timer = 0;
						OBJECT(current_object)->animation_frame = 4u;
						if (OBJECT(current_object)->crumbling_floor.silent_row == 0u)
						{
							switch (OBJECT(current_object)->direction)
							{
							case 0:
							case 4:
								if (OBJECT(current_object)->screen_x >= 0)
								{
									sound_id = 0xDu;
									play_sound(sound_id, &r1, &r2);
								}
								break;
							case 1:
								if (OBJECT(current_object)->screen_x <= 0x180)
								{
									sound_id = 0xDu;
									play_sound(sound_id, &r1, &r2);
									add_to_platform_list();
									if ((uint16_t)(OBJECT(current_object)->screen_x + 0xA0u) >= 0x281u)
										return;
									sprite_depth = (uint16_t)(OBJECT(current_object)->screen_y + OBJECT(current_object)->crumbling_floor.depth_offset - OBJECT(current_object)->sprite_offset_y);
									OBJECT(current_object)->crumbling_floor.draw_depth = sprite_depth;
									queue_sprite_at_depth(&ignored, &sprite_depth);
									return;
								}
								break;
							case 2:
								if (OBJECT(current_object)->screen_x >= -8)
								{
									sound_id = 0xDu;
									play_sound(sound_id, &r1, &r2);
									add_to_platform_list();
									if ((uint16_t)(OBJECT(current_object)->screen_x + 0xA0u) >= 0x281u)
										return;
									sprite_depth = (uint16_t)(OBJECT(current_object)->screen_y + OBJECT(current_object)->crumbling_floor.depth_offset - OBJECT(current_object)->sprite_offset_y);
									OBJECT(current_object)->crumbling_floor.draw_depth = sprite_depth;
									queue_sprite_at_depth(&ignored, &sprite_depth);
									return;
								}
								break;
							case 3:
								if (OBJECT(current_object)->screen_x <= 0x1A0)
								{
									sound_id = 0xDu;
									play_sound(sound_id, &r1, &r2);
									add_to_platform_list();
									if ((uint16_t)(OBJECT(current_object)->screen_x + 0xA0u) >= 0x281u)
										return;
									sprite_depth = (uint16_t)(OBJECT(current_object)->screen_y + OBJECT(current_object)->crumbling_floor.depth_offset - OBJECT(current_object)->sprite_offset_y);
									OBJECT(current_object)->crumbling_floor.draw_depth = sprite_depth;
									queue_sprite_at_depth(&ignored, &sprite_depth);
									return;
								}
								break;
							default:
								unknown_code(0x9161Cu + OBJECT(current_object)->direction * 2u);
							}
						}
						add_to_platform_list();
						if ((uint16_t)(OBJECT(current_object)->screen_x + 0xA0u) >= 0x281u)
							return;
						sprite_depth = (uint16_t)(OBJECT(current_object)->screen_y + OBJECT(current_object)->crumbling_floor.depth_offset - OBJECT(current_object)->sprite_offset_y);
						OBJECT(current_object)->crumbling_floor.draw_depth = sprite_depth;
						queue_sprite_at_depth(&ignored, &sprite_depth);
						return;
					}
					if (OBJECT(current_object)->y_int <= (int16_t)(OBJECT(current_player)->y_int + 0x60u))
					{
					crumbling_floor_crack:
						if (OBJECT(current_object)->animation_timer >= 0)
							goto crumbling_floor_play_crack_steps;
						apply_step_screen_offset();
					}
					break;
				case 1:
					if (OBJECT(current_player)->y_int <= OBJECT(current_object)->y_int)
					{
						OBJECT(current_object)->crumbling_floor.state++;
						OBJECT(current_object)->speed_z = 0;
						OBJECT(current_object)->animation_speed = 6u;
						OBJECT(current_object)->animation_timer = 0;
						OBJECT(current_object)->animation_frame = 4u;
						if (OBJECT(current_object)->crumbling_floor.silent_row == 0u)
						{
							switch (OBJECT(current_object)->direction)
							{
							case 0:
							case 4:
								if (OBJECT(current_object)->screen_x >= 0)
								{
									sound_id = 0xDu;
									play_sound(sound_id, &r1, &r2);
								}
								break;
							case 1:
								if (OBJECT(current_object)->screen_x <= 0x180)
								{
									sound_id = 0xDu;
									play_sound(sound_id, &r1, &r2);
									add_to_platform_list();
									if ((uint16_t)(OBJECT(current_object)->screen_x + 0xA0u) >= 0x281u)
										return;
									sprite_depth = (uint16_t)(OBJECT(current_object)->screen_y + OBJECT(current_object)->crumbling_floor.depth_offset - OBJECT(current_object)->sprite_offset_y);
									OBJECT(current_object)->crumbling_floor.draw_depth = sprite_depth;
									queue_sprite_at_depth(&ignored, &sprite_depth);
									return;
								}
								break;
							case 2:
								if (OBJECT(current_object)->screen_x >= -8)
								{
									sound_id = 0xDu;
									play_sound(sound_id, &r1, &r2);
									add_to_platform_list();
									if ((uint16_t)(OBJECT(current_object)->screen_x + 0xA0u) >= 0x281u)
										return;
									sprite_depth = (uint16_t)(OBJECT(current_object)->screen_y + OBJECT(current_object)->crumbling_floor.depth_offset - OBJECT(current_object)->sprite_offset_y);
									OBJECT(current_object)->crumbling_floor.draw_depth = sprite_depth;
									queue_sprite_at_depth(&ignored, &sprite_depth);
									return;
								}
								break;
							case 3:
								if (OBJECT(current_object)->screen_x <= 0x1A0)
								{
									sound_id = 0xDu;
									play_sound(sound_id, &r1, &r2);
									add_to_platform_list();
									if ((uint16_t)(OBJECT(current_object)->screen_x + 0xA0u) >= 0x281u)
										return;
									sprite_depth = (uint16_t)(OBJECT(current_object)->screen_y + OBJECT(current_object)->crumbling_floor.depth_offset - OBJECT(current_object)->sprite_offset_y);
									OBJECT(current_object)->crumbling_floor.draw_depth = sprite_depth;
									queue_sprite_at_depth(&ignored, &sprite_depth);
									return;
								}
								break;
							default:
								unknown_code(0x9161Cu + OBJECT(current_object)->direction * 2u);
							}
						}
						add_to_platform_list();
						if ((uint16_t)(OBJECT(current_object)->screen_x + 0xA0u) >= 0x281u)
							return;
						sprite_depth = (uint16_t)(OBJECT(current_object)->screen_y + OBJECT(current_object)->crumbling_floor.depth_offset - OBJECT(current_object)->sprite_offset_y);
						OBJECT(current_object)->crumbling_floor.draw_depth = sprite_depth;
						queue_sprite_at_depth(&ignored, &sprite_depth);
						return;
					}
					if ((int16_t)(OBJECT(current_player)->y_int - 0x60u) <= OBJECT(current_object)->y_int)
						goto crumbling_floor_crack;
					break;
				case 2:
					if (OBJECT(current_object)->x_int <= OBJECT(current_player)->x_int)
					{
						OBJECT(current_object)->crumbling_floor.state++;
						OBJECT(current_object)->speed_z = 0;
						OBJECT(current_object)->animation_speed = 6u;
						OBJECT(current_object)->animation_timer = 0;
						OBJECT(current_object)->animation_frame = 4u;
						if (OBJECT(current_object)->crumbling_floor.silent_row == 0u)
						{
							switch (OBJECT(current_object)->direction)
							{
							case 0:
							case 4:
								if (OBJECT(current_object)->screen_x >= 0)
								{
									sound_id = 0xDu;
									play_sound(sound_id, &r1, &r2);
								}
								break;
							case 1:
								if (OBJECT(current_object)->screen_x <= 0x180)
								{
									sound_id = 0xDu;
									play_sound(sound_id, &r1, &r2);
									add_to_platform_list();
									if ((uint16_t)(OBJECT(current_object)->screen_x + 0xA0u) >= 0x281u)
										return;
									sprite_depth = (uint16_t)(OBJECT(current_object)->screen_y + OBJECT(current_object)->crumbling_floor.depth_offset - OBJECT(current_object)->sprite_offset_y);
									OBJECT(current_object)->crumbling_floor.draw_depth = sprite_depth;
									queue_sprite_at_depth(&ignored, &sprite_depth);
									return;
								}
								break;
							case 2:
								if (OBJECT(current_object)->screen_x >= -8)
								{
									sound_id = 0xDu;
									play_sound(sound_id, &r1, &r2);
									add_to_platform_list();
									if ((uint16_t)(OBJECT(current_object)->screen_x + 0xA0u) >= 0x281u)
										return;
									sprite_depth = (uint16_t)(OBJECT(current_object)->screen_y + OBJECT(current_object)->crumbling_floor.depth_offset - OBJECT(current_object)->sprite_offset_y);
									OBJECT(current_object)->crumbling_floor.draw_depth = sprite_depth;
									queue_sprite_at_depth(&ignored, &sprite_depth);
									return;
								}
								break;
							case 3:
								if (OBJECT(current_object)->screen_x <= 0x1A0)
								{
									sound_id = 0xDu;
									play_sound(sound_id, &r1, &r2);
									add_to_platform_list();
									if ((uint16_t)(OBJECT(current_object)->screen_x + 0xA0u) >= 0x281u)
										return;
									sprite_depth = (uint16_t)(OBJECT(current_object)->screen_y + OBJECT(current_object)->crumbling_floor.depth_offset - OBJECT(current_object)->sprite_offset_y);
									OBJECT(current_object)->crumbling_floor.draw_depth = sprite_depth;
									queue_sprite_at_depth(&ignored, &sprite_depth);
									return;
								}
								break;
							default:
								unknown_code(0x9161Cu + OBJECT(current_object)->direction * 2u);
							}
						}
						add_to_platform_list();
						if ((uint16_t)(OBJECT(current_object)->screen_x + 0xA0u) >= 0x281u)
							return;
						sprite_depth = (uint16_t)(OBJECT(current_object)->screen_y + OBJECT(current_object)->crumbling_floor.depth_offset - OBJECT(current_object)->sprite_offset_y);
						OBJECT(current_object)->crumbling_floor.draw_depth = sprite_depth;
						queue_sprite_at_depth(&ignored, &sprite_depth);
						return;
					}
					if (OBJECT(current_object)->x_int <= (int16_t)(OBJECT(current_player)->x_int + 0x60u))
						goto crumbling_floor_crack;
					break;
				case 3:
					if (OBJECT(current_player)->x_int <= OBJECT(current_object)->x_int)
					{
						OBJECT(current_object)->crumbling_floor.state++;
						OBJECT(current_object)->speed_z = 0;
						OBJECT(current_object)->animation_speed = 6u;
						OBJECT(current_object)->animation_timer = 0;
						OBJECT(current_object)->animation_frame = 4u;
						if (OBJECT(current_object)->crumbling_floor.silent_row == 0u)
						{
							switch (OBJECT(current_object)->direction)
							{
							case 0:
							case 4:
								if (OBJECT(current_object)->screen_x >= 0)
								{
									sound_id = 0xDu;
									play_sound(sound_id, &r1, &r2);
								}
								break;
							case 1:
								if (OBJECT(current_object)->screen_x <= 0x180)
								{
									sound_id = 0xDu;
									play_sound(sound_id, &r1, &r2);
									add_to_platform_list();
									if ((uint16_t)(OBJECT(current_object)->screen_x + 0xA0u) >= 0x281u)
										return;
									sprite_depth = (uint16_t)(OBJECT(current_object)->screen_y + OBJECT(current_object)->crumbling_floor.depth_offset - OBJECT(current_object)->sprite_offset_y);
									OBJECT(current_object)->crumbling_floor.draw_depth = sprite_depth;
									queue_sprite_at_depth(&ignored, &sprite_depth);
									return;
								}
								break;
							case 2:
								if (OBJECT(current_object)->screen_x >= -8)
								{
									sound_id = 0xDu;
									play_sound(sound_id, &r1, &r2);
									add_to_platform_list();
									if ((uint16_t)(OBJECT(current_object)->screen_x + 0xA0u) >= 0x281u)
										return;
									sprite_depth = (uint16_t)(OBJECT(current_object)->screen_y + OBJECT(current_object)->crumbling_floor.depth_offset - OBJECT(current_object)->sprite_offset_y);
									OBJECT(current_object)->crumbling_floor.draw_depth = sprite_depth;
									queue_sprite_at_depth(&ignored, &sprite_depth);
									return;
								}
								break;
							case 3:
								if (OBJECT(current_object)->screen_x <= 0x1A0)
								{
									sound_id = 0xDu;
									play_sound(sound_id, &r1, &r2);
									add_to_platform_list();
									if ((uint16_t)(OBJECT(current_object)->screen_x + 0xA0u) >= 0x281u)
										return;
									sprite_depth = (uint16_t)(OBJECT(current_object)->screen_y + OBJECT(current_object)->crumbling_floor.depth_offset - OBJECT(current_object)->sprite_offset_y);
									OBJECT(current_object)->crumbling_floor.draw_depth = sprite_depth;
									queue_sprite_at_depth(&ignored, &sprite_depth);
									return;
								}
								break;
							default:
								unknown_code(0x9161Cu + OBJECT(current_object)->direction * 2u);
							}
						}
						add_to_platform_list();
						if ((uint16_t)(OBJECT(current_object)->screen_x + 0xA0u) >= 0x281u)
							return;
						sprite_depth = (uint16_t)(OBJECT(current_object)->screen_y + OBJECT(current_object)->crumbling_floor.depth_offset - OBJECT(current_object)->sprite_offset_y);
						OBJECT(current_object)->crumbling_floor.draw_depth = sprite_depth;
						queue_sprite_at_depth(&ignored, &sprite_depth);
						return;
					}
					if ((int16_t)(OBJECT(current_player)->x_int - 0x60u) <= OBJECT(current_object)->x_int)
						goto crumbling_floor_crack;
					break;
				default:
					unknown_code(0x91555u + OBJECT(current_object)->direction * 2u);
				}
			}
		}
		else
			OBJECT(current_object)->sprite_palette = 0x83A0u;
		add_to_platform_list();
		if ((uint16_t)(OBJECT(current_object)->screen_x + 0xA0u) >= 0x281u)
			return;
		sprite_depth = (uint16_t)(OBJECT(current_object)->screen_y + OBJECT(current_object)->crumbling_floor.depth_offset - OBJECT(current_object)->sprite_offset_y);
		OBJECT(current_object)->crumbling_floor.draw_depth = sprite_depth;
		queue_sprite_at_depth(&ignored, &sprite_depth);
		return;
	case 2:
		OBJECT(current_object)->speed_z += 0x2000u;
		OBJECT(current_object)->z += OBJECT(current_object)->speed_z;
		project_object(&ignored, &ignored);
		if (OBJECT(current_object)->animation_timer < 0)
			apply_step_screen_offset();
		else
		{
			at_last_step = animate_with_screen_offset(0u);
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
		depth = (uint16_t)(((int32_t)((uint32_t)OBJECT(current_object)->projected_y + OBJECT(current_object)->floor_z) >> 16) - view_y + OBJECT(current_object)->crumbling_floor.depth_offset);
		queue_sprite_at_depth(&ignored, &depth);
		return;
	default:
		unknown_code(0x914DFu + OBJECT(current_object)->crumbling_floor.state * 3u);
	}
}

void spawn_group_leader(uint32_t record, uint32_t *p_spawned, uint32_t *p_leader)
{
	uint32_t leader = *p_leader;
	uint32_t free_count = 0, leader_index = 0, spawned = 0;

	allocate_object_outside_loop(&free_count, &leader);
	if (free_count == 0u)
	{
		*p_spawned = free_count;
		*p_leader = leader;
		return;
	}
	leader_index = mem_read8(record) & 3u;
	OBJECT(leader)->sprite_frame = leader_index;
	group_leaders[leader_index] = leader;
	record++;
	OBJECT(leader)->x_int = mem_read16(record);
	OBJECT(leader)->x_fraction = 0u;
	record += 2u;
	OBJECT(leader)->y_int = mem_read16(record);
	OBJECT(leader)->y_fraction = 0u;
	record += 2u;
	OBJECT(leader)->z_int = mem_read16(record);
	OBJECT(leader)->z_fraction = 0u;
	OBJECT(leader)->floor_z = OBJECT(leader)->z;
	record += 2u;
	OBJECT(leader)->direction = mem_read16(record);
	OBJECT(leader)->animation = mem_read16(record + 2u) + group_leader_data_ADDRESS;
	OBJECT(leader)->handler = group_leader_move_ADDRESS;
	OBJECT(leader)->flags = 0u;
	OBJECT(leader)->sprite_offset_x = 0;
	OBJECT(leader)->sprite_offset_y = 0;
	AS8(OBJECT(leader)->speed_z_int) = 0u;
	OBJECT(leader)->var_80 = 1u;
	OBJECT(leader)->var_81 = 0u;
	if (group_leaders_1_3_held != 0u || group_leader_0_held != 0u)
		OBJECT(leader)->handler = group_leader_wait_ADDRESS;
	spawned = 0xFFFFFFFFu;
	*p_spawned = spawned;
	*p_leader = leader;
}

void group_leader_wait(void)
{
	if (OBJECT(current_object)->sprite_frame == 0u)
	{
		if (group_leader_0_held == 0u)
			OBJECT(current_object)->handler = group_leader_move_ADDRESS;
	}
	else if (group_leaders_1_3_held == 0u)
	{
		OBJECT(current_object)->handler = group_leader_move_ADDRESS;
		return;
	}
}

void group_leader_move(void)
{
	uint16_t hold_threshold = 0, speed = 0;
	uint32_t cursor = 0, ignored = 0;
	uint8_t hold_timer = 0;
	uint16_t wait_timer = 0, trigger_scroll = 0;
	uint32_t flags = 0;

	flags = OBJECT(current_object)->flags;
	OBJECT(current_object)->flags = flags | 1u;
	if ((flags & 1u) == 0u)
	{
	group_leader_read_script:
		wait_timer = OBJECT(current_object)->group_leader_move.wait_timer;
		OBJECT(current_object)->group_leader_move.wait_timer = wait_timer - 1u;
		if ((int16_t)wait_timer <= 1)
		{
			OBJECT(current_object)->group_leader_move.wait_timer = 0;
			cursor = OBJECT(current_object)->group_leader_move.path;
			trigger_scroll = mem_read16(cursor);
			cursor += 2u;
			if (trigger_scroll <= scroll_position)
			{
				if ((int16_t)mem_read16(cursor + 2u) < 0)
				{
					delete_current_object();
					return;
				}
				OBJECT(current_object)->group_leader_move.screen_threshold = mem_read16(cursor);
				cursor += 2u;
				OBJECT(current_object)->group_leader_move.catch_up_factor = mem_read16(cursor);
				cursor += 2u;
				OBJECT(current_object)->group_leader_move.base_speed = mem_read16(cursor);
				cursor += 2u;
				OBJECT(current_object)->group_leader_move.wait_timer = mem_read16(cursor);
				OBJECT(current_object)->group_leader_move.path = cursor + 2u;
			}
		}
	}
	else if (OBJECT(current_object)->group_leader_move.holding_back == 0u)
	{
		if ((stage != 8u || (current_game_mode != 0x17u && current_game_mode != 0x1Du)) && no_player_in_play == 0u && player_mishap == 0u && player_dropping_in == 0u)
			goto group_leader_read_script;
		OBJECT(current_object)->group_leader_move.holding_back = 0xFFu;
		OBJECT(current_object)->group_leader_move.saved_base_speed = OBJECT(current_object)->group_leader_move.base_speed;
		OBJECT(current_object)->group_leader_move.base_speed = 0;
		OBJECT(current_object)->group_leader_move.hold_start_position = scroll_position;
		OBJECT(current_object)->group_leader_move.hold_timer = 0xFFu;
		OBJECT(current_object)->group_leader_move.saved_threshold = OBJECT(current_object)->group_leader_move.screen_threshold;
		hold_threshold = 0xFF90u;
		if ((OBJECT(current_object)->direction & 1u) != 0u)
			hold_threshold = 0x1B0u;
		OBJECT(current_object)->group_leader_move.screen_threshold = hold_threshold;
	}
	project_object(&ignored, &ignored);
	if (OBJECT(current_object)->group_leader_move.holding_back != 0u)
	{
		if ((int16_t)(scroll_position - OBJECT(current_object)->group_leader_move.hold_start_position) >= 0x300)
			goto group_leader_stop_holding_back;
		if (no_player_in_play != 0u)
			OBJECT(current_object)->group_leader_move.hold_timer = 0xFFu;
		hold_timer = OBJECT(current_object)->group_leader_move.hold_timer;
		OBJECT(current_object)->group_leader_move.hold_timer = hold_timer - 1u;
		if (hold_timer == 0u)
			goto group_leader_stop_holding_back;
	}
	switch (OBJECT(current_object)->direction)
	{
	case 0:
	case 2:
	case 4:
		if (OBJECT(current_object)->group_leader_move.screen_threshold <= OBJECT(current_object)->screen_x)
			speed = (uint16_t)OBJECT(current_object)->group_leader_move.base_speed;
		else
		{
		group_leader_catch_up:
			OBJECT(current_object)->group_leader_move.wait_timer = 0;
			if (OBJECT(current_object)->group_leader_move.holding_back != 0u)
			{
			group_leader_stop_holding_back:
				OBJECT(current_object)->group_leader_move.holding_back = 0u;
				OBJECT(current_object)->group_leader_move.screen_threshold = OBJECT(current_object)->group_leader_move.saved_threshold;
				OBJECT(current_object)->group_leader_move.base_speed = OBJECT(current_object)->group_leader_move.saved_base_speed;
			}
			compute_camera_ground_speed(0u);
			speed = (uint16_t)((uint32_t)camera_ground_speed_int * OBJECT(current_object)->group_leader_move.catch_up_factor) >> 8;
			if ((int16_t)speed <= OBJECT(current_object)->group_leader_move.base_speed)
			{
				speed = (uint16_t)OBJECT(current_object)->group_leader_move.base_speed;
				chaser_speed = speed;
				if (OBJECT(current_object)->group_leader_move.wait_timer > 0 || grey_screen_active != 0u)
					return;
				switch (OBJECT(current_object)->direction)
				{
				case 0:
				case 4:
					OBJECT(current_object)->y += (uint32_t)chaser_speed << 12;
					break;
				case 1:
					OBJECT(current_object)->y -= (uint32_t)chaser_speed << 12;
					break;
				case 2:
					OBJECT(current_object)->x += (uint32_t)chaser_speed << 12;
					break;
				case 3:
					OBJECT(current_object)->x -= (uint32_t)chaser_speed << 12;
					break;
				default:
					unknown_code(0x918FAu + OBJECT(current_object)->direction * 2u);
				}
				return;
			}
		}
		break;
	case 1:
	case 3:
		if (OBJECT(current_object)->screen_x <= OBJECT(current_object)->group_leader_move.screen_threshold)
		{
			speed = (uint16_t)OBJECT(current_object)->group_leader_move.base_speed;
			chaser_speed = speed;
			if (OBJECT(current_object)->group_leader_move.wait_timer > 0 || grey_screen_active != 0u)
				return;
			switch (OBJECT(current_object)->direction)
			{
			case 0:
			case 4:
				OBJECT(current_object)->y += (uint32_t)chaser_speed << 12;
				break;
			case 1:
				OBJECT(current_object)->y -= (uint32_t)chaser_speed << 12;
				break;
			case 2:
				OBJECT(current_object)->x += (uint32_t)chaser_speed << 12;
				break;
			case 3:
				OBJECT(current_object)->x -= (uint32_t)chaser_speed << 12;
				break;
			default:
				unknown_code(0x918FAu + OBJECT(current_object)->direction * 2u);
			}
			return;
		}
		goto group_leader_catch_up;
	default:
		unknown_code(0x91887u + OBJECT(current_object)->direction * 2u);
	}
	chaser_speed = speed;
	if (OBJECT(current_object)->group_leader_move.wait_timer > 0 || grey_screen_active != 0u)
		return;
	switch (OBJECT(current_object)->direction)
	{
	case 0:
	case 4:
		OBJECT(current_object)->y += (uint32_t)chaser_speed << 12;
		break;
	case 1:
		OBJECT(current_object)->y -= (uint32_t)chaser_speed << 12;
		break;
	case 2:
		OBJECT(current_object)->x += (uint32_t)chaser_speed << 12;
		break;
	case 3:
		OBJECT(current_object)->x -= (uint32_t)chaser_speed << 12;
		break;
	default:
		unknown_code(0x918FAu + OBJECT(current_object)->direction * 2u);
	}
}

void lava_rock_update(void)
{
	uint8_t sound_id = 0;
	uint16_t part_list = 0;
	uint32_t r1 = 0, r2 = 0, r7 = 0, r11 = 0, at_last_step = 0, depth = 0, free_count = 0, height = 0, ignored = 0, random_value = 0, slot_entry = 0, sprite_depth = 0;
	uint16_t erupt_delay = 0;

	if (map_screen_active != 0u)
	{
		if ((OBJECT(current_object)->sprite_flags & 0xD000u) == 0xD000u)
		{
			part_list = (uint16_t)OBJECT(current_object)->part_list;
			free_object_2(part_list);
		}
		delete_current_object();
		return;
	}
	switch (OBJECT(current_object)->lava_rock.state)
	{
	case 0:
	case 4:
		OBJECT(current_object)->lava_rock.state++;
		OBJECT(current_object)->lava_rock.palette_list = lava_rock_glow_palettes_ADDRESS;
		OBJECT(current_object)->lava_rock.palette_index = 0u;
		OBJECT(current_object)->lava_rock.palette_timer = 0u;
		next_random(&random_value, &ignored, &ignored);
		OBJECT(current_object)->lava_rock.palette_delay = (random_value & 0xFu) + 4u;
		if ((OBJECT(current_object)->sprite_flags & 0xD000u) == 0xD000u)
		{
			allocate_object_2(&free_count, &ignored, &r11);
			if ((uint8_t)free_count == 0u)
			{
				delete_current_object();
				return;
			}
			move_halfwords_backward(r11, lava_rock_parts_ADDRESS, 0x10u);
			OBJECT(current_object)->part_list = r11;
		}
		project_object(&ignored, &r1);
		OBJECT(current_object)->previous_floor_z = OBJECT(current_object)->floor_z;
	case 1:
		update_screen_position(&ignored, &r1);
		current_player = current_object;
		current_player = LINKED(current_player, OBJECT(current_object)->parent);
		if ((OBJECT(current_player)->var_80 | OBJECT(current_player)->var_81) != 0u)
		{
			if (OBJECT(current_object)->lava_rock.takes_slot != 0u)
			{
				slot_entry = group_slots_ADDRESS + OBJECT(current_object)->lava_rock.group_slot * 2u;
				if ((int16_t)mem_read16(slot_entry) < 0)
				{
					OBJECT(current_object)->lava_rock.erupt_delay = mem_read16(slot_entry) & 0x7FFFu;
					OBJECT(current_object)->lava_rock.rise_limit = 0u;
					OBJECT(current_object)->lava_rock.state++;
					OBJECT(current_object)->speed_z = -0x10000;
				}
			}
		}
		else
		{
			switch (OBJECT(current_object)->direction)
			{
			case 0:
			case 4:
				if (OBJECT(current_object)->y_int <= (int16_t)(OBJECT(current_player)->y_int + 0x5Au))
				{
					OBJECT(current_object)->lava_rock.rise_limit = 0x10u;
					OBJECT(current_object)->lava_rock.state++;
					OBJECT(current_object)->speed_z = -0x10000;
					if ((uint16_t)(OBJECT(current_object)->screen_x + 0xA8u) >= 0x1E8u)
						return;
					if ((OBJECT(current_object)->sprite_flags & 0xD000u) != 0xD000u)
						animate_sign();
					else
						cycle_part_palettes();
					add_to_platform_list();
					sprite_depth = (uint16_t)(OBJECT(current_object)->screen_y + OBJECT(current_object)->lava_rock.depth_offset - OBJECT(current_object)->sprite_offset_y);
					OBJECT(current_object)->lava_rock.draw_depth = sprite_depth;
					queue_sprite_at_depth(&ignored, &sprite_depth);
					return;
				}
				if (OBJECT(current_object)->lava_rock.takes_slot != 0u)
				{
					slot_entry = group_slots_ADDRESS + OBJECT(current_object)->lava_rock.group_slot * 2u;
					if ((int16_t)mem_read16(slot_entry) < 0)
					{
						OBJECT(current_object)->lava_rock.erupt_delay = mem_read16(slot_entry) & 0x7FFFu;
						OBJECT(current_object)->lava_rock.rise_limit = 0u;
						OBJECT(current_object)->lava_rock.state++;
						OBJECT(current_object)->speed_z = -0x10000;
					}
				}
				if ((uint16_t)(OBJECT(current_object)->screen_x + 0xA8u) >= 0x1E8u)
					return;
				if ((OBJECT(current_object)->sprite_flags & 0xD000u) != 0xD000u)
					animate_sign();
				else
					cycle_part_palettes();
				add_to_platform_list();
				sprite_depth = (uint16_t)(OBJECT(current_object)->screen_y + OBJECT(current_object)->lava_rock.depth_offset - OBJECT(current_object)->sprite_offset_y);
				OBJECT(current_object)->lava_rock.draw_depth = sprite_depth;
				queue_sprite_at_depth(&ignored, &sprite_depth);
				return;
			case 1:
				if ((int16_t)(OBJECT(current_player)->y_int - 0x5Au) <= OBJECT(current_object)->y_int)
				{
					OBJECT(current_object)->lava_rock.rise_limit = 0x10u;
					OBJECT(current_object)->lava_rock.state++;
					OBJECT(current_object)->speed_z = -0x10000;
					if ((uint16_t)(OBJECT(current_object)->screen_x + 0xA8u) >= 0x1E8u)
						return;
					if ((OBJECT(current_object)->sprite_flags & 0xD000u) != 0xD000u)
						animate_sign();
					else
						cycle_part_palettes();
					add_to_platform_list();
					sprite_depth = (uint16_t)(OBJECT(current_object)->screen_y + OBJECT(current_object)->lava_rock.depth_offset - OBJECT(current_object)->sprite_offset_y);
					OBJECT(current_object)->lava_rock.draw_depth = sprite_depth;
					queue_sprite_at_depth(&ignored, &sprite_depth);
					return;
				}
				if (OBJECT(current_object)->lava_rock.takes_slot != 0u)
				{
					slot_entry = group_slots_ADDRESS + OBJECT(current_object)->lava_rock.group_slot * 2u;
					if ((int16_t)mem_read16(slot_entry) < 0)
					{
						OBJECT(current_object)->lava_rock.erupt_delay = mem_read16(slot_entry) & 0x7FFFu;
						OBJECT(current_object)->lava_rock.rise_limit = 0u;
						OBJECT(current_object)->lava_rock.state++;
						OBJECT(current_object)->speed_z = -0x10000;
					}
				}
				if ((uint16_t)(OBJECT(current_object)->screen_x + 0xA8u) >= 0x1E8u)
					return;
				if ((OBJECT(current_object)->sprite_flags & 0xD000u) != 0xD000u)
					animate_sign();
				else
					cycle_part_palettes();
				add_to_platform_list();
				sprite_depth = (uint16_t)(OBJECT(current_object)->screen_y + OBJECT(current_object)->lava_rock.depth_offset - OBJECT(current_object)->sprite_offset_y);
				OBJECT(current_object)->lava_rock.draw_depth = sprite_depth;
				queue_sprite_at_depth(&ignored, &sprite_depth);
				return;
			case 2:
				if (OBJECT(current_object)->x_int <= (int16_t)(OBJECT(current_player)->x_int + 0x5Au))
				{
					OBJECT(current_object)->lava_rock.rise_limit = 0x10u;
					OBJECT(current_object)->lava_rock.state++;
					OBJECT(current_object)->speed_z = -0x10000;
					if ((uint16_t)(OBJECT(current_object)->screen_x + 0xA8u) >= 0x1E8u)
						return;
					if ((OBJECT(current_object)->sprite_flags & 0xD000u) != 0xD000u)
						animate_sign();
					else
						cycle_part_palettes();
					add_to_platform_list();
					sprite_depth = (uint16_t)(OBJECT(current_object)->screen_y + OBJECT(current_object)->lava_rock.depth_offset - OBJECT(current_object)->sprite_offset_y);
					OBJECT(current_object)->lava_rock.draw_depth = sprite_depth;
					queue_sprite_at_depth(&ignored, &sprite_depth);
					return;
				}
				if (OBJECT(current_object)->lava_rock.takes_slot != 0u)
				{
					slot_entry = group_slots_ADDRESS + OBJECT(current_object)->lava_rock.group_slot * 2u;
					if ((int16_t)mem_read16(slot_entry) < 0)
					{
						OBJECT(current_object)->lava_rock.erupt_delay = mem_read16(slot_entry) & 0x7FFFu;
						OBJECT(current_object)->lava_rock.rise_limit = 0u;
						OBJECT(current_object)->lava_rock.state++;
						OBJECT(current_object)->speed_z = -0x10000;
					}
				}
				if ((uint16_t)(OBJECT(current_object)->screen_x + 0xA8u) >= 0x1E8u)
					return;
				if ((OBJECT(current_object)->sprite_flags & 0xD000u) != 0xD000u)
					animate_sign();
				else
					cycle_part_palettes();
				add_to_platform_list();
				sprite_depth = (uint16_t)(OBJECT(current_object)->screen_y + OBJECT(current_object)->lava_rock.depth_offset - OBJECT(current_object)->sprite_offset_y);
				OBJECT(current_object)->lava_rock.draw_depth = sprite_depth;
				queue_sprite_at_depth(&ignored, &sprite_depth);
				return;
			case 3:
				if ((int16_t)(OBJECT(current_player)->x_int - 0x5Au) <= OBJECT(current_object)->x_int)
				{
					OBJECT(current_object)->lava_rock.rise_limit = 0x10u;
					OBJECT(current_object)->lava_rock.state++;
					OBJECT(current_object)->speed_z = -0x10000;
					if ((uint16_t)(OBJECT(current_object)->screen_x + 0xA8u) >= 0x1E8u)
						return;
					if ((OBJECT(current_object)->sprite_flags & 0xD000u) != 0xD000u)
						animate_sign();
					else
						cycle_part_palettes();
					add_to_platform_list();
					sprite_depth = (uint16_t)(OBJECT(current_object)->screen_y + OBJECT(current_object)->lava_rock.depth_offset - OBJECT(current_object)->sprite_offset_y);
					OBJECT(current_object)->lava_rock.draw_depth = sprite_depth;
					queue_sprite_at_depth(&ignored, &sprite_depth);
					return;
				}
				if (OBJECT(current_object)->lava_rock.takes_slot != 0u)
				{
					slot_entry = group_slots_ADDRESS + OBJECT(current_object)->lava_rock.group_slot * 2u;
					if ((int16_t)mem_read16(slot_entry) < 0)
					{
						OBJECT(current_object)->lava_rock.erupt_delay = mem_read16(slot_entry) & 0x7FFFu;
						OBJECT(current_object)->lava_rock.rise_limit = 0u;
						OBJECT(current_object)->lava_rock.state++;
						OBJECT(current_object)->speed_z = -0x10000;
					}
				}
				if ((uint16_t)(OBJECT(current_object)->screen_x + 0xA8u) >= 0x1E8u)
					return;
				if ((OBJECT(current_object)->sprite_flags & 0xD000u) != 0xD000u)
					animate_sign();
				else
					cycle_part_palettes();
				add_to_platform_list();
				sprite_depth = (uint16_t)(OBJECT(current_object)->screen_y + OBJECT(current_object)->lava_rock.depth_offset - OBJECT(current_object)->sprite_offset_y);
				OBJECT(current_object)->lava_rock.draw_depth = sprite_depth;
				queue_sprite_at_depth(&ignored, &sprite_depth);
				return;
			default:
				unknown_code(0x919DDu + OBJECT(current_object)->direction * 2u);
			}
		}
		if ((uint16_t)(OBJECT(current_object)->screen_x + 0xA8u) >= 0x1E8u)
			return;
		if ((OBJECT(current_object)->sprite_flags & 0xD000u) != 0xD000u)
			animate_sign();
		else
			cycle_part_palettes();
		add_to_platform_list();
		sprite_depth = (uint16_t)(OBJECT(current_object)->screen_y + OBJECT(current_object)->lava_rock.depth_offset - OBJECT(current_object)->sprite_offset_y);
		OBJECT(current_object)->lava_rock.draw_depth = sprite_depth;
		queue_sprite_at_depth(&ignored, &sprite_depth);
		return;
	case 2:
		OBJECT(current_object)->z += OBJECT(current_object)->speed_z;
		height = (uint32_t)OBJECT(current_object)->floor_z - OBJECT(current_object)->z;
		if (height >= 0x20000u)
		{
			OBJECT(current_object)->speed_z = 0u - OBJECT(current_object)->speed_z;
			if ((int32_t)height >= 0 && (int16_t)(OBJECT(current_object)->previous_floor_z_int - OBJECT(current_object)->floor_z_int) <= (int16_t)OBJECT(current_object)->lava_rock.rise_limit)
				OBJECT(current_object)->floor_z_int--;
		}
		update_screen_position(&ignored, &r1);
		current_player = current_object;
		current_player = LINKED(current_player, OBJECT(current_object)->parent);
		if (OBJECT(current_object)->y_int <= OBJECT(current_player)->y_int)
		{
		lava_rock_erupt:
			OBJECT(current_object)->lava_rock.state++;
			OBJECT(current_object)->speed_z = 0;
			OBJECT(current_object)->animation_speed = 4u;
			OBJECT(current_object)->animation_timer = 0;
			OBJECT(current_object)->animation_frame = 0u;
			if (OBJECT(current_object)->lava_rock.takes_slot != 1u || OBJECT(current_object)->lava_rock.member == 0u)
			{
				if (OBJECT(current_object)->screen_x >= -0x90)
				{
					sound_id = 7u;
					play_sound(sound_id, &r1, &r2);
				}
				r11 = spawn_lava_column(r11);
				if ((OBJECT(current_object)->sprite_flags & 0xD000u) == 0xD000u)
				{
					OBJECT(current_object)->x_int += 0x71u;
					spawn_lava_column(r11);
					OBJECT(current_object)->x_int -= 0x71u;
				}
			}
			else if (OBJECT(current_object)->lava_rock.member == 2u)
			{
				r11 = spawn_lava_column(r11);
				if ((OBJECT(current_object)->sprite_flags & 0xD000u) == 0xD000u)
				{
					OBJECT(current_object)->x_int += 0x71u;
					spawn_lava_column(r11);
					OBJECT(current_object)->x_int -= 0x71u;
				}
				if ((uint16_t)(OBJECT(current_object)->screen_x + 0xA0u) >= 0x281u)
					return;
				add_to_platform_list();
				sprite_depth = (uint16_t)(((int32_t)((uint32_t)OBJECT(current_object)->projected_y + OBJECT(current_object)->previous_floor_z) >> 16) - view_y + OBJECT(current_object)->lava_rock.depth_offset);
				OBJECT(current_object)->lava_rock.draw_depth = sprite_depth;
				queue_sprite_at_depth(&ignored, &sprite_depth);
				return;
			}
		}
		else
		{
			if (OBJECT(current_object)->y_int <= (int16_t)(OBJECT(current_player)->y_int + 0x30u))
				OBJECT(current_object)->lava_rock.rise_limit = 0x20u;
			if (OBJECT(current_object)->lava_rock.takes_slot != 0u)
			{
				r11 = group_slots_ADDRESS + OBJECT(current_object)->lava_rock.group_slot * 2u;
				if ((int16_t)mem_read16(r11) < 0)
				{
					erupt_delay = OBJECT(current_object)->lava_rock.erupt_delay;
					OBJECT(current_object)->lava_rock.erupt_delay = erupt_delay - 1u;
					if ((int16_t)erupt_delay <= 1)
					{
						OBJECT(current_object)->lava_rock.takes_slot = 1u;
						goto lava_rock_erupt;
					}
				}
			}
		}
		if ((uint16_t)(OBJECT(current_object)->screen_x + 0xA0u) >= 0x281u)
			return;
		add_to_platform_list();
		sprite_depth = (uint16_t)(((int32_t)((uint32_t)OBJECT(current_object)->projected_y + OBJECT(current_object)->previous_floor_z) >> 16) - view_y + OBJECT(current_object)->lava_rock.depth_offset);
		OBJECT(current_object)->lava_rock.draw_depth = sprite_depth;
		queue_sprite_at_depth(&ignored, &sprite_depth);
		return;
	case 3:
		OBJECT(current_object)->speed_z -= 0x6000u;
		OBJECT(current_object)->z += OBJECT(current_object)->speed_z;
		update_screen_position(&ignored, &r1);
		if (OBJECT(current_object)->screen_y > -0x28)
		{
			if ((int8_t)OBJECT(current_object)->animation_frame >= 0)
			{
				if ((OBJECT(current_object)->sprite_flags & 0xD000u) != 0xD000u)
				{
					animate(&r1, &r7, &at_last_step);
					if ((uint8_t)at_last_step != 0u)
						OBJECT(current_object)->animation_frame = 0xFFu;
				}
				else
				{
					animate_all_parts(&at_last_step, &r11);
					if ((uint8_t)at_last_step != 0u)
					{
						OBJECT(current_object)->animation_frame = 0xFFu;
						if ((int8_t)OBJECT(current_object)->animation_frame <= 2)
							add_to_platform_list();
						depth = (uint16_t)(((int32_t)((uint32_t)OBJECT(current_object)->projected_y + OBJECT(current_object)->floor_z) >> 16) - view_y + OBJECT(current_object)->lava_rock.depth_offset);
						queue_sprite_at_depth(&ignored, &depth);
						return;
					}
				}
			}
			if ((int8_t)OBJECT(current_object)->animation_frame <= 2)
				add_to_platform_list();
			depth = (uint16_t)(((int32_t)((uint32_t)OBJECT(current_object)->projected_y + OBJECT(current_object)->floor_z) >> 16) - view_y + OBJECT(current_object)->lava_rock.depth_offset);
			queue_sprite_at_depth(&ignored, &depth);
			return;
		}
		if ((OBJECT(current_object)->sprite_flags & 0xD000u) == 0xD000u)
		{
			part_list = (uint16_t)OBJECT(current_object)->part_list;
			free_object_2(part_list);
		}
		delete_current_object();
		return;
	default:
		unknown_code(0x9194Du + OBJECT(current_object)->lava_rock.state * 3u);
	}
}

uint32_t spawn_lava_column(uint32_t block)
{
	uint16_t side = 0;
	uint32_t free_count = 0, ignored = 0, object = 0;

	if ((uint16_t)free_object_stack_2[0] != 0u)
	{
		allocate_object(&free_count, &block);
		if (free_count == 0u)
			return block;
		OBJECT(block)->sprite_flags = 0xD006u;
		OBJECT(block)->flags = 2u;
		OBJECT(block)->sprite_offset_x = 0;
		OBJECT(block)->sprite_offset_y = 0;
		OBJECT(block)->handler = lava_column_start_ADDRESS;
		OBJECT(block)->direction = 0x200u;
		OBJECT(block)->speed_x = 0;
		OBJECT(block)->speed_y = 0;
		OBJECT(block)->speed_z = 0;
		OBJECT(block)->lava_column.standing = 0;
		OBJECT(block)->parent = OBJECT(current_object)->parent;
		OBJECT(block)->x = OBJECT(current_object)->x;
		OBJECT(block)->y = OBJECT(current_object)->y;
		OBJECT(block)->z = OBJECT(current_object)->z;
		OBJECT(block)->z += 0xE00000u;
		OBJECT(block)->kind = 0x14u;
		OBJECT(block)->hitbox_size_x = 0x74u;
		OBJECT(block)->hitbox_size_y = 0x30u;
		OBJECT(block)->hitbox_height = 0xE0u;
		OBJECT(block)->hitbox_offset_x = 0;
		OBJECT(block)->hitbox_offset_y = 0;
		OBJECT(block)->animation = lava_column_animation_ADDRESS;
		OBJECT(block)->animation_speed = 2u;
		OBJECT(block)->animation_timer = 0;
		OBJECT(block)->animation_frame = 0u;
		side = 0u;
		if (OBJECT(block)->x_int >= 0xC97)
			side = 1u;
		OBJECT(block)->lava_column.side = side;
		if (OBJECT(current_object)->spawn_lava_column.takes_slot == 1u)
		{
			OBJECT(block)->handler = lava_column_toss_start_ADDRESS;
			OBJECT(block)->speed_z = -0xE0000;
			OBJECT(block)->lava_column.depth_offset = OBJECT(current_object)->spawn_lava_column.depth_offset;
			OBJECT(block)->floor_z = OBJECT(current_object)->previous_floor_z;
			OBJECT(block)->hitbox_size_x = 0x70u;
			OBJECT(block)->hitbox_size_y = 0x60u;
		}
		object = block;
		allocate_object_2(&ignored, &ignored, &block);
		move_halfwords_backward(block, lava_column_parts_ADDRESS, 9u);
		OBJECT(object)->part_list = block;
	}
	return block;
}

void lava_column_start(void)
{
	uint32_t ignored = 0;

	project_object(&ignored, &ignored);
	OBJECT(current_object)->handler = lava_column_rise_ADDRESS;
	lava_column_rise();
}

void lava_column_rise(void)
{
	uint16_t part_list = 0;
	uint32_t r1 = 0, r8 = 0, depth = 0, ignored = 0, part = 0, sprite_depth = 0;

	if (OBJECT(current_object)->screen_x <= -0x88)
	{
		part_list = (uint16_t)OBJECT(current_object)->part_list;
		free_object_2(part_list);
		delete_current_object();
		return;
	}
	if (map_screen_active != 0u || (OBJECT(current_object)->lava_column.standing < 0 && lava_columns[OBJECT(current_object)->lava_column.side] != (uint16_t)current_object))
	{
		part_list = (uint16_t)OBJECT(current_object)->part_list;
		free_object_2(part_list);
		delete_current_object();
		return;
	}
	if (OBJECT(current_object)->lava_column.standing >= 0)
	{
		OBJECT(current_object)->speed_z -= 0x6000u;
		OBJECT(current_object)->z += OBJECT(current_object)->speed_z;
	}
	update_screen_position(&ignored, &r1);
	if (OBJECT(current_object)->z_int <= 0x570)
	{
		OBJECT(current_object)->lava_column.standing = -1;
		lava_columns[OBJECT(current_object)->lava_column.side] = current_object;
	}
	animate_all_parts(&r8, &part);
	add_to_touchable_list(ignored);
	if (OBJECT(current_object)->lava_column.standing < 0)
	{
		depth = 0xFFF1u;
		queue_sprite_at_depth(&ignored, &depth);
		return;
	}
	sprite_depth = (uint16_t)(OBJECT(current_object)->screen_y - 0x128u);
	if ((int16_t)sprite_depth <= -0xF)
		sprite_depth = 0xFFF2u;
	queue_sprite_at_depth(&ignored, &sprite_depth);
}

void lava_column_toss_start(void)
{
	uint32_t ignored = 0;

	project_object(&ignored, &ignored);
	OBJECT(current_object)->handler = lava_column_toss_ADDRESS;
	lava_column_toss();
}

void lava_column_toss(void)
{
	uint16_t part_list = 0;
	uint32_t r1 = 0, r8 = 0, depth = 0, ignored = 0, part = 0;

	if (OBJECT(current_object)->screen_x <= -0x88)
	{
		part_list = (uint16_t)OBJECT(current_object)->part_list;
		free_object_2(part_list);
		delete_current_object();
		return;
	}
	OBJECT(current_object)->speed_z += 0x6000u;
	OBJECT(current_object)->z += OBJECT(current_object)->speed_z;
	if (OBJECT(current_object)->z_int < 0x660)
	{
		update_screen_position(&ignored, &r1);
		animate_all_parts(&r8, &part);
		add_to_touchable_list(ignored);
		depth = (uint16_t)(((int32_t)((uint32_t)OBJECT(current_object)->projected_y + OBJECT(current_object)->floor_z) >> 16) - view_y + OBJECT(current_object)->lava_column.depth_offset);
		queue_sprite_at_depth(&ignored, &depth);
		return;
	}
	OBJECT(current_object)->handler = lava_column_land_ADDRESS;
	part_list = (uint16_t)OBJECT(current_object)->part_list;
	free_object_2(part_list);
	lava_column_land();
}

void lava_column_land(void)
{
	uint32_t ignored = 0;

	OBJECT(current_object)->lava_column.base_x = 0x40;
	OBJECT(current_object)->lava_column.base_x += OBJECT(current_object)->x_int;
	OBJECT(current_object)->lava_column.base_y = OBJECT(current_object)->y_int;
	OBJECT(current_object)->z = 0x5400000;
	OBJECT(current_object)->sprite_flags = 0x1004u;
	OBJECT(current_object)->sprite_palette = 0xD000u;
	OBJECT(current_object)->sprite_offset_x = 0;
	OBJECT(current_object)->sprite_offset_y = 0;
	OBJECT(current_object)->flags = 0u;
	OBJECT(current_object)->handler = bubble_near_parent_update_ADDRESS;
	OBJECT(current_object)->animation_frame = 0u;
	OBJECT(current_object)->animation = bubble_animation_ADDRESS;
	OBJECT(current_object)->animation_timer = 0;
	OBJECT(current_object)->animation_speed = 4u;
	project_object(&ignored, &ignored);
}

void spawn_group_slot_trigger(uint32_t record, uint32_t *p_spawned, uint32_t *p_count, uint32_t *p_trigger)
{
	uint32_t count = *p_count;
	uint32_t trigger = *p_trigger;
	uint32_t fill_word = 0, free_count = 0, slot_cursor = 0, spawned = 0;

	allocate_object_outside_loop(&free_count, &trigger);
	if (free_count == 0u)
	{
		*p_spawned = free_count;
		*p_trigger = trigger;
		return;
	}
	OBJECT(trigger)->group_slot_trigger.next_trigger = mem_read16(record + 9u) + group_slot_triggers_ADDRESS;
	OBJECT(trigger)->handler = group_slot_trigger_update_ADDRESS;
	OBJECT(trigger)->flags = 0u;
	slot_cursor = group_slots_ADDRESS;
	count = 0x20u;
	fill_word = 0u;
	do
	{
		mem_write32(slot_cursor, fill_word);
		slot_cursor += 4u;
		count--;
	} while (count != 0u);
	spawned = 0xFFFFFFFFu;
	*p_spawned = spawned;
	*p_count = count;
	*p_trigger = trigger;
}

void group_slot_trigger_update(void)
{
	uint32_t cursor = 0, slot = 0;
	uint16_t trigger_scroll = 0;
	uint32_t slot_address = 0;

	cursor = OBJECT(current_object)->group_slot_trigger.next_trigger;
	trigger_scroll = mem_read16(cursor);
	cursor += 2u;
	if (scroll_position < trigger_scroll)
		return;
	if ((int16_t)mem_read16(cursor) >= 0)
	{
		slot = mem_read16(cursor);
		cursor += 2u;
		if (group_slots[slot] != 0u)
		{
			group_slots[slot] = mem_read16(cursor);
			slot_address = group_slots_ADDRESS + slot * 2u;
			mem_write16(slot_address, mem_read16(slot_address) | 0x8000u);
			AS16(OBJECT(current_object)->group_slot_trigger.next_trigger) = cursor + 2u;
		}
		return;
	}
	if (mem_read16(cursor) == 0x8000u)
		goal_unlocked = 0xFFu;
	delete_current_object();
}

void spawn_object_groups_2(uint32_t *p_r1, uint32_t *p_count, uint32_t *p_object)
{
	uint32_t r1 = *p_r1;
	uint32_t count = *p_count;
	uint32_t object = *p_object;
	uint8_t bridge_number = 0;
	uint32_t fill_word = 0, floor_block_record = 0, floor_type_record = 0, ignored = 0, kind_index = 0, last_plank_var = 0, member_record = 0, plank_count_var = 0, record = 0, slot_cursor = 0, spawned = 0;
	uint8_t slot_taken = 0;
	uint16_t spawn_position = 0, members_left = 0;
	uint32_t init_flags = 0;

	init_flags = stage_init_flags;
	stage_init_flags = init_flags | 0x10u;
	if ((init_flags & 0x10u) == 0u)
	{
		unused_group_spawn_word_2 = 0u;
		unused_group_spawn_word = 0u;
		unused_group_spawn_word_3 = 1u;
		r1 = stage_record_spawns[stage];
		group_spawn_cursor_2 = stage_record_spawns_ADDRESS + r1;
		bridge_0_plank_count = 0u;
		bridge_1_plank_count = 0u;
		slot_cursor = group_slots_ADDRESS;
		count = 0x10u;
		fill_word = 0u;
		do
		{
			mem_write32(slot_cursor, fill_word);
			slot_cursor += 4u;
			count--;
		} while (count != 0u);
	}
	for (;;)
	{
		record = group_spawn_cursor_2;
		spawn_position = ROM16(record);
		record += 2u;
		if (scroll_position < spawn_position)
		{
			*p_r1 = r1;
			*p_count = count;
			*p_object = object;
			return;
		}
		kind_index = ROM8(record) * 2u;
		record++;
		switch (kind_index)
		{
		case 0x0u:
			goto spawn_object_groups_2_pick_bridge;
		case 0x2u:
		case 0x8u:
			spawn_group_leader(record, &spawned, &object);
			if (spawned == 0u)
			{
				*p_r1 = r1;
				*p_count = count;
				*p_object = object;
				return;
			}
			group_spawn_cursor_2 += 0xEu;
			continue;
		case 0x4u:
			group_bridge_number = 0u;
			plank_count_var = bridge_0_plank_count_ADDRESS;
			goto spawn_object_groups_2_build_group;
		case 0x6u:
			group_bridge_number = 1u;
			plank_count_var = bridge_1_plank_count_ADDRESS;
			goto spawn_object_groups_2_build_group;
		default:
			unknown_code(0x938A6u + kind_index);
		}
	}
spawn_object_groups_2_pick_bridge:
	if (free_object_stack < (int16_t)ROM16(record + 5u))
	{
		*p_r1 = r1;
		*p_count = count;
		*p_object = object;
		return;
	}
	bridge_number = 0u;
	plank_count_var = bridge_0_plank_count_ADDRESS;
	if (bridge_0_plank_count != 0u)
	{
		bridge_number = 1u;
		plank_count_var = bridge_1_plank_count_ADDRESS;
		if (bridge_1_plank_count != 0u)
		{
			*p_r1 = r1;
			*p_count = count;
			*p_object = object;
			return;
		}
	}
	group_bridge_number = bridge_number;
	mem_write16(plank_count_var, mem_read16(plank_count_var) + 1u);
	group_member_number = 0u;
spawn_object_groups_2_build_group:
	if ((uint16_t)(scroll_position - mem_read16(record - 3u)) < 0x100u)
	{
		group_leader = group_leaders[ROM8(record)];
		record++;
		group_takes_slot = ROM8(record);
		record += 2u;
		group_floor_block = ROM32(record);
		record += 4u;
		group_member_count = ROM16(record);
		group_previous_member = 0xFFFFu;
		mem_write16(plank_count_var, mem_read16(plank_count_var) + group_member_count);
		record += 2u;
		group_step_x = ROM16(record);
		record += 2u;
		group_step_y = ROM16(record);
		record += 2u;
		group_direction = ROM16(record);
		record += 2u;
		group_member_x = ROM16(record);
		record += 2u;
		group_member_y = ROM16(record);
		group_record = ROM16(record + 2u);
		if (group_takes_slot != 0u)
		{
			slot_cursor = group_slots_ADDRESS;
			count = 0x40u;
			do
			{
				slot_taken = mem_read8(slot_cursor);
				slot_cursor++;
				count--;
			} while ((count != 0u) & (slot_taken != 0u));
			mem_write8(slot_cursor - 1u, 1u);
			count = (count & ~0xFFFFu) | (uint16_t)(count + 1u);
			group_slot_index = 0x40u - count;
		}
		do
		{
			allocate_object_outside_loop(&ignored, &object);
			OBJECT(object)->x_int = group_member_x;
			OBJECT(object)->x_fraction = 0u;
			group_member_x += group_step_x;
			OBJECT(object)->y_int = group_member_y;
			OBJECT(object)->y_fraction = 0u;
			group_member_y += group_step_y;
			member_record = group_record;
			OBJECT(object)->kind = member_record;
			member_record += 0x93F59u;
			OBJECT(object)->sprite_flags = mem_read16(member_record);
			member_record += 2u;
			OBJECT(object)->sprite_frame = mem_read16(member_record);
			member_record += 2u;
			OBJECT(object)->sprite_palette = mem_read16(member_record);
			member_record += 2u;
			OBJECT(object)->handler = mem_read32(member_record);
			member_record += 4u;
			OBJECT(object)->animation = mem_read32(member_record);
			member_record += 4u;
			r1 = mem_read32(member_record);
			member_record += 4u;
			if ((group_member_number & 1u) != 0u)
				OBJECT(object)->animation = r1;
			OBJECT(object)->hitbox_size_x = mem_read16(member_record);
			member_record += 2u;
			OBJECT(object)->hitbox_size_y = mem_read16(member_record);
			OBJECT(object)->var_96 = mem_read16(member_record + 2u);
			OBJECT(object)->sprite_offset_x = 0x2E;
			OBJECT(object)->sprite_offset_y = 0x11;
			AS8(OBJECT(object)->speed) = 0u;
			group_member_number++;
			AT8(OBJECT(object)->speed, 1) = group_member_number;
			OBJECT(object)->floor_block = group_floor_block;
			AT8(OBJECT(object)->spawn_arg_2, 1) = group_bridge_number;
			OBJECT(object)->direction = group_direction;
			OBJECT(object)->parent = group_leader;
			AS16(OBJECT(object)->player) = group_previous_member;
			group_previous_member = object;
			floor_block_record = floor_blocks + 2u + OBJECT(object)->floor_block;
			OBJECT(object)->z_int = mem_read16(floor_block_record + 0x10u);
			OBJECT(object)->z_fraction = 0u;
			OBJECT(object)->floor_z = OBJECT(object)->z;
			OBJECT(object)->speed_x = OBJECT(object)->z;
			OBJECT(object)->floor_slope = mem_read8(floor_block_record + 0x13u);
			floor_type_record = floor_types_ADDRESS + mem_read8(floor_block_record + 0x12u) * 4u;
			OBJECT(object)->floor_type = ROM8(floor_type_record);
			OBJECT(object)->floor_attribute = ROM16(floor_type_record + 2u);
			OBJECT(object)->flags = 0u;
			AS8(OBJECT(object)->spawn_arg_2) = 0u;
			OBJECT(object)->var_8A = group_slot_index;
			AS8(OBJECT(object)->spawn_arg_1) = group_takes_slot;
			members_left = group_member_count;
			group_member_count = members_left - 1u;
		} while ((int16_t)members_left > 1);
		AS8(OBJECT(object)->spawn_arg_2) = 0xFFu;
		last_plank_var = bridge_0_last_plank_ADDRESS;
		if (group_bridge_number != 0u)
			last_plank_var = bridge_1_last_plank_ADDRESS;
		mem_write16(last_plank_var, object);
	}
	group_spawn_cursor_2 += 0x18u;
	*p_r1 = r1;
	*p_count = count;
	*p_object = object;
}

void rope_bridge_plank_update(void)
{
	uint8_t slot_mark = 0, sound_id = 0;
	uint16_t child = 0, last_plank = 0;
	uint32_t r1 = 0, r2 = 0, r7 = 0, at_last_step = 0, hitbox_record = 0, ignored = 0, object = 0, object_2 = 0, object_3 = 0, odd_plank_animation = 0, plank_count_address = 0, plank_index = 0, rope_offsets = 0, ropes_left = 0, sag = 0, sag_table = 0, slot_address = 0, sprite_depth = 0;

	switch (OBJECT(current_object)->rope_bridge_plank.state)
	{
	case 0:
	case 3:
		project_object(&ignored, &ignored);
		OBJECT(current_object)->child = 0xFFFFu;
		if ((OBJECT(current_object)->rope_bridge_plank.plank_index & 3u) == 0u && free_object_stack_2[0] >= 2)
		{
			ropes_left = 2u;
			object = current_object;
			rope_offsets = rope_bridge_rope_offsets_ADDRESS;
			if (OBJECT(current_object)->kind == 0x30u)
				rope_offsets = wide_rope_bridge_rope_offsets_ADDRESS;
			do
			{
				allocate_object_2(&ignored, &r2, &object_2);
				OBJECT(object_2)->sprite_flags = 0x1004u;
				OBJECT(object_2)->sprite_palette = 0x8290u;
				OBJECT(object_2)->sprite_frame = 0x9D18u;
				OBJECT(object_2)->projected_x = ROM16(rope_offsets);
				rope_offsets += 2u;
				OBJECT(object_2)->projected_y_fraction = ROM16(rope_offsets);
				OBJECT(object)->child = object_2;
				OBJECT(object_2)->parent = object;
				object = (object & ~0xFFFFu) | (uint16_t)object_2;
				rope_offsets += 2u;
				ropes_left--;
			} while (ropes_left != 0u);
		}
		OBJECT(current_object)->rope_bridge_plank.state++;
	case 1:
		OBJECT(current_object)->previous_floor_z = OBJECT(current_object)->floor_z;
		plank_index = OBJECT(current_object)->rope_bridge_plank.plank_index;
		sag_table = bridge_0_sag_ADDRESS;
		if (OBJECT(current_object)->rope_bridge_plank.bridge != 0u)
			sag_table = bridge_1_sag_ADDRESS;
		sag = mem_read16(sag_table + plank_index * 2u);
		if ((sag & ~0xFFu) == 0u)
			sag = 0u;
		OBJECT(current_object)->previous_floor_z += sag << 8;
		OBJECT(current_object)->speed_x += (int32_t)((uint32_t)OBJECT(current_object)->previous_floor_z - OBJECT(current_object)->speed_x) >> 2;
		OBJECT(current_object)->z_int = OBJECT(current_object)->speed_x_int;
		update_screen_position(&ignored, &sag);
		if (rope_bridge_collapsing != 0u)
		{
		rope_bridge_plank_start_falling:
			OBJECT(current_object)->rope_bridge_plank.state++;
			last_plank = bridge_0_last_plank;
			plank_count_address = bridge_0_plank_count_ADDRESS;
			if (OBJECT(current_object)->rope_bridge_plank.bridge != 0u)
			{
				last_plank = bridge_1_last_plank;
				plank_count_address = bridge_1_plank_count_ADDRESS;
			}
			if (last_plank == (uint16_t)current_object)
				mem_write16(plank_count_address, 0u);
			OBJECT(current_object)->speed_z = 0;
			OBJECT(current_object)->animation_speed = 2u;
			OBJECT(current_object)->animation_timer = 0;
			OBJECT(current_object)->animation_frame = 0u;
			if ((uint16_t)OBJECT(current_object)->screen_x < 0x200u)
			{
				sound_id = 0x28u;
				play_sound(sound_id, &sag, &r2);
			}
		}
		else
		{
			current_player = current_object;
			current_player = LINKED(current_player, OBJECT(current_object)->parent);
			switch (OBJECT(current_object)->direction)
			{
			case 0:
			case 4:
				if (OBJECT(current_object)->y_int <= OBJECT(current_player)->y_int)
					goto rope_bridge_plank_start_falling;
				break;
			case 1:
				if (OBJECT(current_player)->y_int <= OBJECT(current_object)->y_int)
					goto rope_bridge_plank_start_falling;
				break;
			case 2:
				if (OBJECT(current_object)->x_int <= OBJECT(current_player)->x_int)
					goto rope_bridge_plank_start_falling;
				break;
			case 3:
				if (OBJECT(current_player)->x_int <= OBJECT(current_object)->x_int)
					goto rope_bridge_plank_start_falling;
				break;
			default:
				unknown_code(0x93BC7u + OBJECT(current_object)->direction * 2u);
			}
		}
		if (OBJECT(current_object)->rope_bridge_plank.takes_slot < 0)
		{
			if ((OBJECT(current_object)->flags & 8u) != 0u)
			{
				object_3 = LINKED(current_object, OBJECT(current_object)->platform);
				if (OBJECT(object_3)->kind == 0xFu)
				{
					switch (OBJECT(current_object)->direction)
					{
					case 0:
					case 1:
					case 4:
						slot_mark = 0xFFu;
						if ((int16_t)((OBJECT(current_object)->hitbox_size_x >> 1) + OBJECT(current_object)->x_int) <= OBJECT(object_3)->x_int)
							slot_mark = 0x80u;
						break;
					case 2:
					case 3:
						slot_mark = 0xFFu;
						if ((int16_t)((OBJECT(current_object)->hitbox_size_y >> 1) + OBJECT(current_object)->y_int) <= OBJECT(object_3)->y_int)
							slot_mark = 0x80u;
						break;
					default:
						unknown_code(0x93C6Cu + OBJECT(current_object)->direction * 2u);
					}
					RAM8(group_slots_ADDRESS + OBJECT(current_object)->rope_bridge_plank.group_slot) = slot_mark;
				}
			}
			slot_address = group_slots_ADDRESS + OBJECT(current_object)->rope_bridge_plank.group_slot;
			if ((int8_t)mem_read8(slot_address) < 0)
			{
				switch (OBJECT(current_object)->direction)
				{
				case 0:
				case 1:
				case 4:
					OBJECT(current_object)->x_int += OBJECT(current_object)->hitbox_size_x >> 1;
					OBJECT(current_object)->floor_block += 0x16u;
					if (mem_read8(slot_address) != 0xFFu)
					{
						OBJECT(current_object)->rope_bridge_plank.depth_offset = (uint32_t)OBJECT(current_object)->rope_bridge_plank.depth_offset << 1;
						OBJECT(current_object)->floor_block += 0x16u;
					}
					break;
				case 2:
				case 3:
					OBJECT(current_object)->floor_block += 0x16u;
					if (mem_read8(slot_address) == 0xFFu)
					{
						OBJECT(current_object)->y_int += OBJECT(current_object)->hitbox_size_y >> 1;
						OBJECT(current_object)->floor_block += 0x16u;
						OBJECT(current_object)->rope_bridge_plank.depth_offset -= 0x28u;
					}
					break;
				default:
					unknown_code(0x93CD1u + OBJECT(current_object)->direction * 2u);
				}
				OBJECT(current_object)->sprite_flags = stage_object_records_2;
				OBJECT(current_object)->sprite_frame = rope_plank_frame;
				OBJECT(current_object)->sprite_palette = rope_plank_palette;
				OBJECT(current_object)->animation = rope_plank_animation;
				odd_plank_animation = rope_plank_animation_2;
				hitbox_record = rope_plank_hitbox_size_ADDRESS;
				if ((OBJECT(current_object)->rope_bridge_plank.plank_index & 1u) != 0u)
					OBJECT(current_object)->animation = odd_plank_animation;
				OBJECT(current_object)->hitbox_size_x = ROM16(hitbox_record);
				OBJECT(current_object)->hitbox_size_y = ROM16(hitbox_record + 2u);
				OBJECT(current_object)->rope_bridge_plank.takes_slot = 1;
				if ((int16_t)OBJECT(current_object)->child >= 0)
				{
					object_2 = LINKED(current_object, OBJECT(current_object)->child);
					OBJECT(object_2)->projected_x = rope_bridge_rope_offsets;
					OBJECT(object_2)->projected_y_fraction = rope_bridge_rope_1_dy;
					object_2 = LINKED(object_2, OBJECT(object_2)->child);
					OBJECT(object_2)->projected_x = rope_bridge_rope_2_dx;
					OBJECT(object_2)->projected_y_fraction = rope_bridge_rope_2_dy;
				}
				project_object(&ignored, &ignored);
			}
		}
		if ((uint16_t)(OBJECT(current_object)->screen_x + 0x50u) >= 0x1B0u)
			return;
		add_to_platform_list();
		draw_rope_bridge_ropes();
		sprite_depth = (uint16_t)(((int32_t)((uint32_t)OBJECT(current_object)->projected_y + OBJECT(current_object)->floor_z) >> 16) - view_y + OBJECT(current_object)->rope_bridge_plank.depth_offset);
		OBJECT(current_object)->rope_bridge_plank.draw_depth = sprite_depth;
		queue_sprite_at_depth(&ignored, &sprite_depth);
		return;
	case 2:
		OBJECT(current_object)->speed_z += 0x6000u;
		OBJECT(current_object)->z += OBJECT(current_object)->speed_z;
		if (OBJECT(current_object)->animation_timer >= 0)
		{
			if ((OBJECT(current_object)->sprite_flags & ~0xFFFu) != 0xD000u)
			{
				animate(&r1, &r7, &at_last_step);
				if ((int8_t)at_last_step < 0)
					OBJECT(current_object)->animation_timer = -1;
			}
			else if ((int32_t)OBJECT(current_object)->part_list >= 0x205000)
			{
				animate_all_parts(&at_last_step, &object_2);
				if ((int8_t)at_last_step < 0)
					OBJECT(current_object)->animation_timer = -1;
			}
		}
		update_screen_position(&ignored, &r1);
		if (OBJECT(current_object)->screen_y <= 0x1C0)
		{
			draw_rope_bridge_ropes();
			ignored = queue_sprite_at_floor();
			return;
		}
		if ((int16_t)OBJECT(current_object)->child >= 0)
		{
			child = OBJECT(current_object)->child;
			free_object_2(child);
			current_object = LINKED(current_object, OBJECT(current_object)->child);
			child = OBJECT(current_object)->child;
			free_object_2(child);
		}
		delete_current_object();
		return;
	default:
		unknown_code(0x93AEAu + OBJECT(current_object)->rope_bridge_plank.state * 3u);
	}
}

void draw_rope_bridge_ropes(void)
{
	uint32_t depth = 0, ropes_left = 0, slot_address = 0;

	if ((int16_t)OBJECT(current_object)->child < 0)
		return;
	ropes_left = 2u;
	do
	{
		current_object = LINKED(current_object, OBJECT(current_object)->child);
		if ((uint8_t)OBJECT(object_cursor)->draw_rope_bridge_ropes.takes_slot == 0u)
		{
		draw_rope_bridge_ropes_show_rope:
			if (OBJECT(object_cursor)->draw_rope_bridge_ropes.state == 2u)
				OBJECT(current_object)->sprite_frame = 0x9D20u;
			OBJECT(current_object)->screen_x = OBJECT(object_cursor)->screen_x + OBJECT(current_object)->projected_x;
			OBJECT(current_object)->screen_y = OBJECT(object_cursor)->screen_y + OBJECT(current_object)->projected_y_fraction;
			depth = queue_sprite(depth);
		}
		else
		{
			slot_address = group_slots_ADDRESS + OBJECT(object_cursor)->draw_rope_bridge_ropes.group_slot;
			if ((uint16_t)ropes_left == 1u)
			{
				if (mem_read8(slot_address) != 0x80u)
					goto draw_rope_bridge_ropes_show_rope;
			}
			else if (mem_read8(slot_address) != 0xFFu)
				goto draw_rope_bridge_ropes_show_rope;
		}
		ropes_left--;
	} while (ropes_left != 0u);
	current_object = object_cursor;
}

void event_0f_start(void)
{
	scene_timer = 0;
	scroll_mode = 0u;
	AS8(scroll_speed) = 3u;
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

void event_0f_show_messages(uint32_t r5, uint32_t *p_frames, uint32_t *p_box)
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
	AS8(scroll_speed) = 5u;
	game_mode = 3u;
	*p_frames = (*p_frames & 0xFFFF0000u) | frames;
	*p_box = box;
}

void event_0c_start(uint32_t *p_r2, uint32_t *p_r3, uint32_t *p_object)
{
	uint32_t r2 = *p_r2;
	uint32_t r3 = *p_r3;
	uint32_t object = *p_object;

	scene_timer = 0x3C;
	spawn_door_rollers(&r2, &r3, &object);
	scene_step++;
	scroll_mode = 0u;
	scroll_speed = 2u;
	*p_r2 = r2;
	*p_r3 = r3;
	*p_object = object;
}

uint32_t event_0c_first_quake(uint32_t r2)
{
	uint8_t sound_id = 0;
	uint32_t r1 = 0;
	uint16_t time_left = 0;

	if (scene_timer <= 0x1E)
		scroll_mode = 1u;
	if ((uint16_t)scene_timer == 0x3Cu)
	{
		camera_shake_active = 1u;
		sound_id = 0xFu;
		play_sound(sound_id, &r1, &r2);
	}
	time_left = scene_timer;
	scene_timer = time_left - 1u;
	if ((int16_t)time_left <= 1)
	{
		scene_step++;
		scene_timer = 0x3C;
	}
	return r2;
}

void event_0c_second_quake(uint32_t r5, uint32_t *p_r2, uint32_t *p_point_y, uint32_t *p_z, uint32_t *p_wall)
{
	uint32_t r2 = *p_r2;
	uint32_t point_y = *p_point_y;
	uint32_t z = *p_z;
	uint32_t wall = *p_wall;
	uint8_t sound_id = 0;
	uint32_t r1 = 0;
	uint16_t time_left = 0;

	if ((uint16_t)scene_timer == 0x3Cu)
	{
		camera_shake_active = 1u;
		sound_id = 0xFu;
		play_sound(sound_id, &r1, &r2);
	}
	spawn_walls(&r1, &r2, &r5, &point_y, &z, &wall);
	time_left = scene_timer;
	scene_timer = time_left - 1u;
	if ((int16_t)time_left > 1)
	{
		*p_r2 = r2;
		*p_point_y = point_y;
		*p_z = z;
		*p_wall = wall;
		return;
	}
	scene_step++;
	scene_timer = 0xA;
	sound_id = 0xAu;
	play_sound(sound_id, &r1, &r2);
	*p_r2 = r2;
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

void update_stage_05_event(uint32_t r2, uint32_t r3, uint32_t r11, uint32_t *p_r4, uint32_t *p_r8, uint32_t *p_r9, uint32_t *p_r10)
{
	uint32_t r4 = *p_r4;
	uint32_t r8 = *p_r8;
	uint32_t r9 = *p_r9;
	uint32_t r10 = *p_r10;
	uint8_t sound_id = 0, spawned = 0;
	uint16_t kind = 0, screen_x = 0, screen_y = 0;
	uint32_t r1 = 0, r2_2 = 0, r5 = 0, r10_2 = 0, free_count = 0, object = 0, piece_record = 0, rocks_left = 0;

	spawned = (uint8_t)(scene_step * 3u);
	switch (scene_step)
	{
	case 0:
	case 8:
		scene_timer = 0;
		unused_stage_05_event_long = 0u;
		scene_step++;
		update_player_summary(&r3, &r8, &r10, &r11);
		update_scroll(&r2, &r3, &r4, &r5, &r8, &r9, &r10, &r11);
		update_stage_tile_layers(&r1, &r2, &r3, &r4, &r5, &r8, &r9, &r10, &r11);
		check_hits(&r1, &r2, &r3, &r4, &r5, &r8, &r11);
		check_walls(r1);
		check_platforms(&r1, &r2, &r3, &r4, &r5, &r8, &r9, &r10_2, &r11);
		goal_unlocked = 0xFFu;
		check_stage_clear(&r8, &r10_2, &r11);
		if (game_mode != 0x17u)
		{
			*p_r4 = r4;
			*p_r8 = r8;
			*p_r9 = r9;
			*p_r10 = r10_2;
			return;
		}
		game_mode = 0xAu;
		scene_step++;
		scene_timer = 0;
		goal_scroll_position = 0xFFFFu;
		*p_r4 = r4;
		*p_r8 = r8;
		*p_r9 = r9;
		*p_r10 = r10_2;
		return;
	case 1:
		update_player_summary(&r3, &r8, &r10, &r11);
		update_scroll(&r2, &r3, &r4, &r5, &r8, &r9, &r10, &r11);
		update_stage_tile_layers(&r1, &r2, &r3, &r4, &r5, &r8, &r9, &r10, &r11);
		check_hits(&r1, &r2, &r3, &r4, &r5, &r8, &r11);
		check_walls(r1);
		check_platforms(&r1, &r2, &r3, &r4, &r5, &r8, &r9, &r10_2, &r11);
		goal_unlocked = 0xFFu;
		check_stage_clear(&r8, &r10_2, &r11);
		if (game_mode != 0x17u)
		{
			*p_r4 = r4;
			*p_r8 = r8;
			*p_r9 = r9;
			*p_r10 = r10_2;
			return;
		}
		game_mode = 0xAu;
		scene_step++;
		scene_timer = 0;
		goal_scroll_position = 0xFFFFu;
		*p_r4 = r4;
		*p_r8 = r8;
		*p_r9 = r9;
		*p_r10 = r10_2;
		return;
	case 2:
		if (stage_05_switch_step >= 0)
		{
			run_stage_05_switch(r3, &r2, &r4, &r11);
			if ((uint8_t)stage_05_switch_step == 2u)
			{
				allocate_object_outside_loop(&free_count, &r11);
				if (free_count != 0u)
					OBJECT(r11)->handler = stage_00_platform_start_ADDRESS;
			}
			if (stage_05_switch_step < 0)
				load_stage_map_pointers(&r1, &r10);
		}
		find_nearby_floor_blocks(&r1, &r3, &r4, &r5, &r8, &r10, &r11);
		check_hits(&r1, &r2, &r3, &r4, &r5, &r8, &r11);
		check_walls(r1);
		check_platforms(&r1, &r2, &r3, &r4, &r5, &r8, &r9, &r10_2, &r11);
		scene_timer++;
		if (scene_timer < 0xB4)
		{
			*p_r4 = r4;
			*p_r8 = r8;
			*p_r9 = r9;
			*p_r10 = r10_2;
			return;
		}
		scene_step++;
		scene_timer = 0;
		stage_init_flags &= ~0x20u;
		*p_r4 = r4;
		*p_r8 = r8;
		*p_r9 = r9;
		*p_r10 = r10_2;
		return;
	case 3:
		if ((uint16_t)scene_timer == 0u)
			spawn_eggman_booth(spawned, 0u, r3, &r2, &r11);
		if ((uint16_t)scene_timer == 0xAu)
		{
			kind = 0x10u;
			r8 = (r8 & ~0xFFFFu) | 0xB4u;
			screen_y = 0x50u;
			r5 = 0xA0u;
			r11 = spawn_message_box((uint16_t)r5, screen_y, kind, (uint16_t)r8, r11);
			sound_id = 0x70u;
			play_sound(sound_id, &r1, &r2);
		}
		spawn_walls(&r1, &r2, &r5, &r8, &r9, &r11);
		check_hits(&r1, &r2, &r3, &r4, &r5, &r8, &r11);
		check_walls(r1);
		check_platforms(&r1, &r2, &r3, &r4, &r5, &r8, &r9, &r10_2, &r11);
		scene_timer++;
		if (scene_timer < 0xA0)
		{
			*p_r4 = r4;
			*p_r8 = r8;
			*p_r9 = r9;
			*p_r10 = r10_2;
			return;
		}
		scene_step++;
		scene_timer = 0;
		*p_r4 = r4;
		*p_r8 = r8;
		*p_r9 = r9;
		*p_r10 = r10_2;
		return;
	case 4:
		camera_shake_active = 1u;
		camera_shake_length = 0x200u;
		camera_shake_wave = 0x200u;
		r2_2 = update_camera_with_shake();
		update_scroll(&r2_2, &r3, &r4, &r5, &r8, &r9, &r10, &r11);
		update_stage_tile_layers(&r1, &r2_2, &r3, &r4, &r5, &r8, &r9, &r10, &r11);
		check_hits(&r1, &r2_2, &r3, &r4, &r5, &r8, &r11);
		check_walls(r1);
		check_platforms(&r1, &r2_2, &r3, &r4, &r5, &r8, &r9, &r10_2, &r11);
		if ((uint16_t)scene_timer == 1u || (uint16_t)scene_timer == 0x3Bu)
		{
			sound_id = 0xFu;
			play_sound(sound_id, &r1, &r2_2);
		}
		scene_timer++;
		if (scene_timer < 0x3C)
		{
			*p_r4 = r4;
			*p_r8 = r8;
			*p_r9 = r9;
			*p_r10 = r10_2;
			return;
		}
		scene_step++;
		*p_r4 = r4;
		*p_r8 = r8;
		*p_r9 = r9;
		*p_r10 = r10_2;
		return;
	case 5:
		allocate_object_outside_loop(&free_count, &r11);
		if (free_count != 0u)
		{
			OBJECT(r11)->flags = 0u;
			OBJECT(r11)->handler = stage_00_geyser_rise_ADDRESS;
			stage_00_geyser = r11;
		}
		object = LINKED(WORK_RAM, stage_00_platform);
		screen_x = (uint16_t)OBJECT(object)->screen_x;
		r14 = (r14 & ~0xFFFFu) | (uint16_t)OBJECT(object)->screen_y;
		rocks_left = stage_00_rock_count;
		piece_record = stage_00_rock_pieces_ADDRESS;
		do
		{
			allocate_object_outside_loop(&free_count, &r11);
			if (free_count == 0u)
			{
				scene_timer = 0;
				sound_id = 6u;
				play_sound(sound_id, &r1, &r2);
				scene_step++;
				*p_r8 = rocks_left;
				*p_r10 = object;
				return;
			}
			OBJECT(r11)->sprite_flags = ROM16(piece_record);
			OBJECT(r11)->screen_x = screen_x;
			piece_record += 2u;
			OBJECT(r11)->screen_x += ROM16(piece_record);
			OBJECT(r11)->screen_y = r14;
			piece_record += 2u;
			OBJECT(r11)->screen_y += ROM16(piece_record);
			piece_record += 2u;
			OBJECT(r11)->speed_x_fraction = ROM16(piece_record);
			piece_record += 2u;
			OBJECT(r11)->speed_y_fraction = ROM16(piece_record);
			piece_record += 2u;
			OBJECT(r11)->animation = rock_debris_animations_ADDRESS + ROM16(piece_record);
			OBJECT(r11)->sprite_palette = 0x8240u;
			OBJECT(r11)->projected_x = (uint32_t)OBJECT(r11)->screen_x << 4;
			OBJECT(r11)->projected_y_fraction = (uint32_t)OBJECT(r11)->screen_y << 4;
			OBJECT(r11)->animation_frame = 0u;
			OBJECT(r11)->animation_speed = 4u;
			OBJECT(r11)->animation_timer = 0;
			OBJECT(r11)->flags = 0u;
			OBJECT(r11)->sprite_scale = 0x100u;
			OBJECT(r11)->handler = rock_debris_fly_ADDRESS;
			rocks_left--;
			piece_record += 2u;
		} while (rocks_left != 0u);
		scene_timer = 0;
		sound_id = 6u;
		play_sound(sound_id, &r1, &r2);
		scene_step++;
		*p_r8 = rocks_left;
		*p_r10 = object;
		return;
	case 6:
		scene_timer++;
		if (scene_timer >= 0x1E)
			update_stage_00_exit_scene(r11, &r4, &r10);
		if ((uint16_t)scene_timer == 5u)
		{
			sound_id = 6u;
			play_sound(sound_id, &r1, &r2);
		}
		else if ((uint16_t)scene_timer == 0x30u)
		{
			sound_id = 6u;
			play_sound(sound_id, &r1, &r2);
			if (scene_timer < 0x186)
			{
				*p_r4 = r4;
				*p_r10 = r10;
				return;
			}
			scene_step++;
			scene_timer = 0;
			*p_r4 = r4;
			*p_r10 = r10;
			return;
		}
		if (scene_timer < 0x186)
		{
			*p_r4 = r4;
			*p_r10 = r10;
			return;
		}
		scene_step++;
		scene_timer = 0;
		*p_r4 = r4;
		*p_r10 = r10;
		return;
	case 7:
		update_stage_00_exit_scene(r11, &r4, &r10);
		scene_timer++;
		if (scene_timer < 0x12C)
		{
			*p_r4 = r4;
			*p_r10 = r10;
			return;
		}
		game_mode = 2u;
		*p_r4 = r4;
		*p_r10 = r10;
		return;
	default:
		unknown_code(0x9493Cu + scene_step * 3u);
	}
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
	uint32_t r1 = 0, bucket = 0;

	if ((int8_t)(game_mode - 0xAu) < 0 || (int8_t)scene_step < 5)
	{
		update_screen_position(&bucket, &r1);
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
	uint32_t r1 = 0, r7 = 0, r8 = 0, bucket = 0;

	animate(&r1, &r7, &r8);
	update_screen_position(&bucket, &r1);
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
	uint32_t r1 = 0, r7 = 0, r8 = 0, cursor = 0, free_count = 0, ignored = 0, object = 0, object_2 = 0;
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
		OBJECT(current_object)->speed_y_fraction = mem_read16(cursor);
		cursor += 2u;
		OBJECT(current_object)->speed_z = (uint32_t)mem_read16(cursor) << 12;
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
	update_screen_position(&ignored, &r1);
	if (OBJECT(current_object)->screen_y < 0x130)
	{
		animate(&r1, &r7, &r8);
		r1 = queue_sprite(r1);
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
	uint32_t r1 = 0, ignored = 0, object = 0;

	object = LINKED(current_object, OBJECT(current_object)->parent);
	OBJECT(current_object)->z -= OBJECT(object)->speed_z;
	OBJECT(current_object)->sprite_frame = stage_00_geyser_column_sprites[OBJECT(object)->animation_frame];
	update_screen_position(&ignored, &r1);
	if (OBJECT(current_object)->screen_y < 0x130)
	{
		r1 = queue_sprite(r1);
		return;
	}
	delete_current_object();
}

void rock_debris_fly(void)
{
	uint8_t bucket = 0;
	uint32_t r7 = 0, r8 = 0, frame = 0;

	OBJECT(current_object)->projected_x += OBJECT(current_object)->speed_x_fraction;
	OBJECT(current_object)->screen_x = (int16_t)OBJECT(current_object)->projected_x >> 4;
	OBJECT(current_object)->projected_y_fraction += OBJECT(current_object)->speed_y_fraction;
	OBJECT(current_object)->screen_y = (int16_t)OBJECT(current_object)->projected_y_fraction >> 4;
	if ((uint16_t)(OBJECT(current_object)->screen_x + 0x20u) >= 0x180u || (uint16_t)(OBJECT(current_object)->screen_y + 0x20u) >= 0x120u)
	{
		delete_current_object();
		return;
	}
	if ((OBJECT(current_object)->sprite_flags & ~0xFFFu) != 0u)
	{
		animate(&frame, &r7, &r8);
		bucket = 1u;
		queue_sprite_in_bucket(bucket);
		return;
	}
	OBJECT(current_object)->sprite_scale += 0x30u;
	if ((int16_t)OBJECT(current_object)->sprite_scale < 0x600)
	{
		animate(&frame, &r7, &r8);
		bucket = 1u;
		queue_sprite_in_bucket(bucket);
		return;
	}
	delete_current_object();
}

void event_1c_follow(void)
{
	follow_base_speed = 5u;
	AS8(scroll_mode) = 7u;
	if (no_player_in_play != 0u)
		AS8(scroll_mode) = 1u;
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
	AS8(scroll_mode) = 1u;
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
	AS8(scroll_mode) = 0u;
	AS8(scroll_speed) = 0xFu;
}

void update_ending(uint32_t r2, uint32_t r3, uint32_t r11, uint32_t *p_r4, uint32_t *p_r8, uint32_t *p_r9, uint32_t *p_r10)
{
	uint32_t r4 = *p_r4;
	uint32_t r8 = *p_r8;
	uint32_t r9 = *p_r9;
	uint32_t r1 = 0, r5 = 0, r10 = 0, r10_2 = 0, ignored = 0;

	if (scene_step == 0u)
		scene_step = 0xFFu;
	next_stage_slot_request = 0u;
	joining_closed = 0xFFu;
	build_ending_scene(&r2, &r3, &r4, &r8, &r9, &r11);
	find_nearby_floor_blocks(&r1, &r3, &r4, &r5, &r8, &r10, &r11);
	r4 = update_restart_point(r4);
	spawn_objects_by_type(&r1, &r2, &r3, &r4, &r5, &r8, &r11);
	spawn_ring_rows(&r1, &r2, &r3, &r11);
	spawn_walls(&r1, &r2, &r5, &r8, &r9, &r11);
	spawn_object_groups_2(&r1, &r8, &r11);
	spawn_object_groups(&r1, &r2, &r3, &r8, &r11);
	spawn_objects_by_handler(&r1, &ignored, &r11);
	r1 = check_zones(r1);
	check_hits(&r1, &r2, &r3, &r4, &r5, &r8, &r11);
	check_walls(r1);
	check_platforms(&r1, &r2, &r3, &r4, &r5, &r8, &r9, &r10_2, &r11);
	if ((player_status[0] | player_2_status | player_3_status | player_status_spare) != 0u)
	{
		*p_r4 = r4;
		*p_r8 = r8;
		*p_r9 = r9;
		*p_r10 = r10_2;
		return;
	}
	saved_game_mode = game_mode;
	game_mode = 7u;
	game_mode_timer = 0;
	*p_r4 = r4;
	*p_r8 = r8;
	*p_r9 = r9;
	*p_r10 = r10_2;
}

void spawn_stage_06_lightning(uint32_t *p_record, uint32_t *p_bolt)
{
	uint32_t bolt = *p_bolt;
	uint8_t sound_id = 0;
	uint32_t r1 = 0, r2 = 0, free_count = 0, record = 0;
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
	play_sound(sound_id, &r1, &r2);
	*p_record = record;
	*p_bolt = bolt;
}

void stage_06_lightning_animate(void)
{
	uint32_t r1 = 0, r7 = 0, at_last_step = 0;

	if (current_game_mode != 0x10u)
	{
		delete_current_object();
		return;
	}
	animate(&r1, &r7, &at_last_step);
	if ((uint8_t)at_last_step == 0u)
	{
		r1 = queue_sprite(r1);
		return;
	}
	delete_current_object();
}

uint32_t spawn_message_box(uint16_t screen_x, uint16_t screen_y, uint32_t kind, uint16_t frames, uint32_t box)
{
	uint32_t free_count = 0;

	allocate_object_outside_loop(&free_count, &box);
	if (free_count != 0u)
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
	return box;
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

uint32_t run_map_screen(uint32_t r4, uint32_t object)
{
	uint32_t r4_2 = 0, free_count = 0, record = 0;

	switch (map_screen_step)
	{
	case 0:
	case 6:
		mixer_text_blend |= 0x4000u;
		mixer_nbg0_blend |= 0x4000u;
		mixer_nbg1_blend |= 0x4000u;
		mixer_nbg2_blend |= 0x4000u;
		mixer_nbg3_blend |= 0x4000u;
		mixer_bitmap_blend |= 0x4000u;
		mixer_sprite_control |= 0x8000u;
		fade_kind = 0;
		fade_speed = 1u;
		window_wipe_closing = 0u;
		record = map_wipe_in_windows_ADDRESS + stage * 6u;
		r4_2 = start_window_wipe(record);
		map_screen_step++;
		return r4_2;
	case 1:
		update_fade(0u);
		if (fade_kind < 0)
			map_screen_step++;
		return r4;
	case 2:
		allocate_object_outside_loop(&free_count, &object);
		if ((uint8_t)free_count != 0u)
		{
			OBJECT(object)->handler = map_intro_update_ADDRESS;
			OBJECT(object)->map_intro.timer = 0;
			OBJECT(object)->animation_frame = 0u;
			OBJECT(object)->map_intro.base_x = window_wipe_left;
			OBJECT(object)->map_intro.base_y = window_wipe_top;
			OBJECT(object)->map_intro.step_x = 0;
			OBJECT(object)->map_intro.step_y = 0;
			OBJECT(object)->map_intro.offset_x = 0;
			OBJECT(object)->map_intro.offset_y = 0;
			map_screen_step++;
		}
		return r4;
	case 3:
	case 4:
	case 5:
		return r4;
	default:
		unknown_code(0x95983u + map_screen_step * 3u);
	}
}

void map_intro_update(void)
{
	uint16_t icon_y = 0;
	uint32_t r1 = 0, dest = 0, free_count = 0, ignored = 0, object = 0, palette_list = 0, script = 0, stage_number = 0, step_y = 0;
	uint8_t io_cnt_value = 0;

	switch (OBJECT(current_object)->animation_frame)
	{
	case 0:
	case 8:
		OBJECT(current_object)->animation_frame++;
		OBJECT(current_object)->animation_frame++;
		OBJECT(current_object)->animation_frame++;
		OBJECT(current_object)->animation_frame++;
		OBJECT(current_object)->map_intro.timer++;
		if (OBJECT(current_object)->map_intro.timer >= 0x1E)
			map_screen_active = 0xFFu;
		return;
	case 1:
		OBJECT(current_object)->animation_frame++;
		return;
	case 2:
		step_y = (int32_t)((0x10u - (uint16_t)OBJECT(current_object)->map_intro.base_y) << 16) / 0x3C;
		OBJECT(current_object)->map_intro.step_x = (int32_t)((0x20u - (uint16_t)OBJECT(current_object)->map_intro.base_x) << 16) / 0x3C;
		OBJECT(current_object)->map_intro.step_y = step_y;
		OBJECT(current_object)->map_intro.timer = 0x3C;
		mem_write16(video_layer_disable, mem_read16(video_layer_disable) | 0x2Fu);
		mem_write16(backdrop_color, 0u);
		OBJECT(current_object)->animation_frame++;
		stage_number = stage;
		map_stage_icon = ROM16(stage_map_icons_ADDRESS + (uint16_t)(stage_number << 1));
		return;
	case 3:
		move_map_stage_icon();
		return;
	case 4:
		io_cnt_value = mem_read8(io_cnt);
		mem_write8(io_cnt, io_cnt_value & ~2u);
		display_off_frames = 2u;
		nbg0_scroll_x_int = 0u;
		nbg0_scroll_y_int = 0u;
		mem_write16(video_layer_disable, mem_read16(video_layer_disable) | 0x2Fu);
		mem_write16(backdrop_color, 0u);
		mem_write32(video_control, mem_read32(video_control) & ~0x400u);
		vblank_copies_pending = 0u;
		vblank_copy_count = 0u;
		palette_list = tile_bank_0_palette_list_ADDRESS;
		load_palette_list(palette_list, &ignored, &dest);
		script = map_monitor_script_ADDRESS;
		run_vram_script(&r1, &script, &dest);
		script = map_eggman_head_script_ADDRESS;
		nbg2_scroll_x = 0u;
		nbg2_scroll_y = 0u;
		run_vram_script(&r1, &script, &dest);
		nbg0_pages = 0x1010101u;
		nbg2_pages = 0u;
		fade_kind = 2;
		fade_speed = 1u;
		fade_step = 0u;
		OBJECT(current_object)->animation_frame++;
		mixer_nbg0_blend |= 0x4000u;
		mixer_nbg3_blend |= 0x4000u;
		update_fade(r1);
		return;
	case 5:
		mem_write16(video_layer_disable, mem_read16(video_layer_disable) | 0xEu);
		mem_write16(video_layer_disable, mem_read16(video_layer_disable) & ~5u);
		OBJECT(current_object)->animation_frame++;
		return;
	case 6:
		update_fade(0u);
		if (color_offset_red != 0x3Fu)
		{
			draw_map_markers_and_trail();
			return;
		}
		allocate_object_outside_loop(&free_count, &object);
		if ((uint8_t)free_count != 0u)
		{
			mixer_nbg0_blend &= 0xBFFFu;
			mixer_nbg1_blend &= 0xBFFFu;
			OBJECT(object)->handler = map_screen_update_ADDRESS;
			icon_y = (uint16_t)(((int32_t)OBJECT(current_object)->map_intro.offset_y >> 16) + OBJECT(current_object)->map_intro.base_y);
			OBJECT(object)->map_screen.base_x = ((int32_t)OBJECT(current_object)->map_intro.offset_x >> 16) + OBJECT(current_object)->map_intro.base_x;
			OBJECT(object)->map_screen.base_y = icon_y;
			OBJECT(object)->sprite_frame = map_stage_icon;
			OBJECT(object)->sprite_palette = 0x6B0u;
			OBJECT(object)->sprite_scale = 0x100u;
			OBJECT(object)->sprite_layer = 1u;
			OBJECT(object)->screen_x = 0x20;
			OBJECT(object)->screen_y = 0x10;
			OBJECT(object)->sprite_flags = 0x10Au;
			OBJECT(object)->animation_frame = 0u;
			OBJECT(object)->map_screen.timer = 0;
			OBJECT(current_object)->animation_frame++;
			object = draw_map_stage_markers(object);
			draw_map_trail_dot(object);
		}
		return;
	case 7:
		delete_current_object();
		return;
	default:
		unknown_code(0x95A5Fu + OBJECT(current_object)->animation_frame * 3u);
	}
}

void move_map_stage_icon(void)
{
	uint16_t icon_x = 0, icon_y = 0;

	icon_y = (uint16_t)(((int32_t)OBJECT(current_object)->move_map_stage_icon.offset_y >> 16) + OBJECT(current_object)->move_map_stage_icon.base_y);
	if ((uint16_t)(((int32_t)OBJECT(current_object)->move_map_stage_icon.offset_x >> 16) + OBJECT(current_object)->move_map_stage_icon.base_x) != 0x30u || icon_y != 0x10u)
	{
		OBJECT(current_object)->move_map_stage_icon.timer--;
		if ((uint16_t)OBJECT(current_object)->move_map_stage_icon.timer == 0u)
			OBJECT(current_object)->animation_frame++;
		OBJECT(current_object)->move_map_stage_icon.offset_x += OBJECT(current_object)->move_map_stage_icon.step_x;
		OBJECT(current_object)->move_map_stage_icon.offset_y += OBJECT(current_object)->move_map_stage_icon.step_y;
	}
	icon_x = (uint16_t)(((int32_t)OBJECT(current_object)->move_map_stage_icon.offset_x >> 16) + OBJECT(current_object)->move_map_stage_icon.base_x);
	icon_y = (uint16_t)(((int32_t)OBJECT(current_object)->move_map_stage_icon.offset_y >> 16) + OBJECT(current_object)->move_map_stage_icon.base_y);
	OBJECT(current_object)->sprite_frame = map_stage_icon;
	OBJECT(current_object)->move_map_stage_icon.blink_phase++;
	OBJECT(current_object)->move_map_stage_icon.blink_phase &= 1u;
	if (OBJECT(current_object)->move_map_stage_icon.blink_phase == 0u)
		OBJECT(current_object)->sprite_frame = 0x6538u;
	OBJECT(current_object)->sprite_palette = 0x6B0u;
	OBJECT(current_object)->sprite_scale = 0x100u;
	OBJECT(current_object)->sprite_layer = 1u;
	OBJECT(current_object)->screen_x = icon_x;
	OBJECT(current_object)->screen_y = icon_y;
	OBJECT(current_object)->sprite_flags = 0x10Au;
	queue_sprite_in_layer();
}

void draw_map_markers_and_trail(void)
{
	uint32_t r11 = 0;

	r11 = draw_map_stage_markers(0u);
	start_map_trail();
	draw_map_trail_dot(r11);
}

void map_screen_update(void)
{
	uint16_t next_x = 0;
	uint32_t edges_done = 0, free_count = 0, ignored = 0, marker_position = 0, next_y = 0, object = 0, record = 0, stage_number = 0;

	switch (OBJECT(current_object)->animation_frame)
	{
	case 0:
	case 12:
		allocate_object_outside_loop(&free_count, &object);
		if ((uint8_t)free_count == 0u)
			return;
		OBJECT(object)->handler = map_eggman_update_ADDRESS;
		OBJECT(object)->animation_frame = 0u;
		OBJECT(object)->map_eggman.sound_step = 0u;
		OBJECT(current_object)->animation_frame++;
		OBJECT(current_object)->map_screen.timer = 1;
		return;
	case 1:
		OBJECT(current_object)->map_screen.timer--;
		if (AS8(OBJECT(current_object)->map_screen.timer) == 0u)
		{
			OBJECT(current_object)->map_screen.timer = 0x64;
			OBJECT(current_object)->animation_frame++;
		}
		OBJECT(current_object)->map_screen.blink_phase++;
		OBJECT(current_object)->map_screen.blink_phase &= 1u;
		object = draw_map_stage_markers(0u);
		start_map_trail();
		draw_map_trail_dot(object);
		return;
	case 2:
		object = draw_map_stage_markers(0u);
		start_map_trail();
		draw_map_trail_dot(object);
		OBJECT(object_cursor)->map_screen.timer--;
		if (OBJECT(object_cursor)->map_screen.timer <= 0)
		{
			OBJECT(object_cursor)->map_screen.timer = 0x12C;
			OBJECT(object_cursor)->map_screen.blink_frames = 0x154;
			OBJECT(object_cursor)->animation_frame++;
			start_map_trail();
		}
		return;
	case 3:
		object = draw_map_stage_markers(0u);
		draw_map_trail_dot(object);
		if ((uint16_t)OBJECT(current_object)->map_screen.blink_frames != 0u && OBJECT(current_object)->map_screen.trail_done < 0)
		{
			blink_current_map_marker();
			OBJECT(current_object)->map_screen.blink_frames--;
			if (OBJECT(current_object)->map_screen.blink_frames <= 0)
			{
				OBJECT(current_object)->map_screen.timer = 0x28;
				OBJECT(current_object)->animation_frame++;
			}
		}
		return;
	case 4:
		OBJECT(current_object)->map_screen.timer--;
		if ((uint16_t)OBJECT(current_object)->map_screen.timer == 0u)
		{
			OBJECT(current_object)->map_screen.timer = 0x12C;
			OBJECT(current_object)->map_screen.blink_frames = 0x154;
			OBJECT(current_object)->animation_frame++;
			start_map_trail();
		}
		OBJECT(current_object)->sprite_frame = 0x65C8u;
		OBJECT(current_object)->sprite_palette = 0x6C0u;
		OBJECT(current_object)->screen_x = 0x20;
		OBJECT(current_object)->screen_y = 0x10;
		OBJECT(current_object)->sprite_layer = 0u;
		OBJECT(current_object)->sprite_flags = 0u;
		OBJECT(current_object)->sprite_flags &= ~0xFFu;
		OBJECT(current_object)->sprite_flags |= 0x10Au;
		OBJECT(current_object)->map_screen.blink_phase++;
		if (OBJECT(current_object)->map_screen.blink_phase >= 3)
			OBJECT(current_object)->map_screen.blink_phase = 0;
		if ((OBJECT(current_object)->flags & 2u) == 0u)
		{
			if ((OBJECT(current_object)->map_screen.blink_phase & 2u) != 0u)
				OBJECT(current_object)->sprite_flags |= 0x80u;
		}
		else
			OBJECT(current_object)->sprite_flags |= 0x40u;
		queue_sprite_in_layer();
		return;
	case 5:
		stage_number = stage;
		map_stage_icon = ROM16(next_stage_map_icons_ADDRESS + (uint16_t)(stage_number << 1));
		window_wipe_closing = 0u;
		record = map_wipe_out_windows_ADDRESS + stage * 6u;
		start_window_wipe(record);
		OBJECT(current_object)->map_screen.base_x = 0x20;
		OBJECT(current_object)->map_screen.base_y = 0x10;
		OBJECT(current_object)->sprite_frame = map_stage_icon;
		OBJECT(current_object)->map_screen.blink_phase++;
		OBJECT(current_object)->map_screen.blink_phase &= 1u;
		if ((uint8_t)OBJECT(current_object)->map_screen.blink_phase == 0u)
			OBJECT(current_object)->sprite_frame = 0x6538u;
		OBJECT(current_object)->sprite_palette = 0x1130u;
		OBJECT(current_object)->sprite_scale = 0x100u;
		OBJECT(current_object)->sprite_layer = 1u;
		OBJECT(current_object)->screen_x = OBJECT(current_object)->map_screen.base_x;
		OBJECT(current_object)->screen_y = OBJECT(current_object)->map_screen.base_y;
		OBJECT(current_object)->sprite_flags = 0x10Au;
		queue_sprite_in_layer();
		OBJECT(current_object)->animation_frame++;
		OBJECT(current_object)->map_screen.timer = 0x3C;
		return;
	case 6:
		if ((uint16_t)OBJECT(current_object)->map_screen.timer != 0u)
			OBJECT(current_object)->map_screen.timer--;
		else
		{
			update_screen_wipe(&ignored, &ignored, &ignored, &edges_done);
			if ((edges_done & 0xFu) == 0xFu)
			{
				OBJECT(current_object)->animation_frame++;
				map_screen_step++;
				OBJECT(current_object)->map_screen.timer--;
				OBJECT(current_object)->sprite_frame = map_stage_icon;
				OBJECT(current_object)->map_screen.blink_phase++;
				OBJECT(current_object)->map_screen.blink_phase &= 1u;
				if ((uint8_t)OBJECT(current_object)->map_screen.blink_phase == 0u)
					OBJECT(current_object)->sprite_frame = 0x6538u;
				OBJECT(current_object)->sprite_palette = 0x6B0u;
				OBJECT(current_object)->sprite_scale = 0x100u;
				OBJECT(current_object)->sprite_layer = 1u;
				OBJECT(current_object)->screen_x = OBJECT(current_object)->map_screen.base_x;
				OBJECT(current_object)->screen_y = OBJECT(current_object)->map_screen.base_y;
				OBJECT(current_object)->sprite_flags = 0x10Au;
				queue_sprite_in_layer();
				return;
			}
		}
		OBJECT(current_object)->sprite_frame = map_stage_icon;
		OBJECT(current_object)->map_screen.blink_phase++;
		OBJECT(current_object)->map_screen.blink_phase &= 1u;
		if ((uint8_t)OBJECT(current_object)->map_screen.blink_phase == 0u)
			OBJECT(current_object)->sprite_frame = 0x6538u;
		OBJECT(current_object)->sprite_palette = 0x6B0u;
		OBJECT(current_object)->sprite_scale = 0x100u;
		OBJECT(current_object)->sprite_layer = 1u;
		OBJECT(current_object)->screen_x = OBJECT(current_object)->map_screen.base_x;
		OBJECT(current_object)->screen_y = OBJECT(current_object)->map_screen.base_y;
		OBJECT(current_object)->sprite_flags = 0x10Au;
		queue_sprite_in_layer();
		return;
	case 7:
		mem_write16(video_layer_disable, mem_read16(video_layer_disable) | 0x2Fu);
		OBJECT(current_object)->sprite_frame = map_stage_icon;
		OBJECT(current_object)->map_screen.blink_phase++;
		OBJECT(current_object)->map_screen.blink_phase &= 1u;
		if ((uint8_t)OBJECT(current_object)->map_screen.blink_phase == 0u)
			OBJECT(current_object)->sprite_frame = 0x6538u;
		OBJECT(current_object)->sprite_palette = 0x6B0u;
		OBJECT(current_object)->sprite_scale = 0x100u;
		OBJECT(current_object)->sprite_layer = 1u;
		OBJECT(current_object)->screen_x = OBJECT(current_object)->map_screen.base_x;
		OBJECT(current_object)->screen_y = OBJECT(current_object)->map_screen.base_y;
		OBJECT(current_object)->sprite_flags = 0x10Au;
		queue_sprite_in_layer();
		marker_position = ((uint32_t)stage_map_markers[stage] << 2) + map_stage_positions_ADDRESS;
		next_x = ROM16(marker_position);
		next_y = ROM16(marker_position + 2u);
		OBJECT(current_object)->map_screen.target_x = next_x;
		OBJECT(current_object)->map_screen.target_y = next_y;
		next_y = (int32_t)((next_y - (uint16_t)OBJECT(current_object)->map_screen.base_y) << 16) / 0x3C;
		OBJECT(current_object)->map_screen.step_x = (int32_t)(((uint32_t)next_x - (uint16_t)OBJECT(current_object)->map_screen.base_x) << 16) / 0x3C;
		OBJECT(current_object)->map_screen.step_y = next_y;
		OBJECT(current_object)->map_screen.timer = 0x3C;
		OBJECT(current_object)->animation_frame++;
		return;
	case 8:
		move_map_stage_icon();
		return;
	case 9:
		window_wipe_closing = 1u;
		record = map_wipe_next_windows_ADDRESS + stage * 6u;
		start_window_wipe(record);
		mem_write16(video_layer_disable, mem_read16(video_layer_disable) & ~0x21u);
		OBJECT(current_object)->sprite_frame = map_stage_icon;
		OBJECT(current_object)->map_screen.blink_phase++;
		OBJECT(current_object)->map_screen.blink_phase &= 1u;
		if (AS8(OBJECT(current_object)->map_screen.marker_blink_count) == 0u)
			OBJECT(current_object)->sprite_frame = 0x6538u;
		OBJECT(current_object)->sprite_palette = 0x6B0u;
		OBJECT(current_object)->sprite_scale = 0x100u;
		OBJECT(current_object)->sprite_layer = 1u;
		OBJECT(current_object)->screen_x = OBJECT(current_object)->map_screen.target_x;
		OBJECT(current_object)->screen_y = OBJECT(current_object)->map_screen.target_y;
		OBJECT(current_object)->sprite_flags = 0x10Au;
		queue_sprite_in_layer();
		OBJECT(current_object)->animation_frame++;
		map_screen_step = 5u;
		return;
	case 10:
		return;
	case 11:
		update_screen_wipe(&ignored, &ignored, &ignored, &edges_done);
		if ((edges_done & 0xFu) == 0xFu)
			delete_current_object();
		return;
	default:
		unknown_code(0x95DBEu + OBJECT(current_object)->animation_frame * 3u);
	}
}

void map_eggman_update(void)
{
	uint8_t sound_id = 0;
	uint16_t gesture = 0;
	uint32_t free_count = 0, ignored = 0, object = 0;
	uint16_t timer = 0;

	switch (OBJECT(current_object)->animation_frame)
	{
	case 0:
	case 4:
		OBJECT(current_object)->sprite_frame = 0x5760u;
		OBJECT(current_object)->sprite_palette = 0x680u;
		OBJECT(current_object)->sprite_scale = 0x100u;
		OBJECT(current_object)->sprite_layer = 1u;
		OBJECT(current_object)->screen_x = 0xB7;
		OBJECT(current_object)->screen_y = 0xDF;
		OBJECT(current_object)->sprite_flags = 0u;
		OBJECT(current_object)->sprite_flags &= ~0x10Fu;
		OBJECT(current_object)->sprite_flags |= 0x107u;
		queue_sprite_in_layer();
		OBJECT(current_object)->map_eggman.timer = 0x3Cu;
		OBJECT(current_object)->animation_frame++;
		map_stage_09_sound_count = 0u;
		sound_id = stage_map_sounds_1[stage];
		queue_sound(sound_id, &ignored, &ignored);
		return;
	case 1:
		timer = OBJECT(current_object)->map_eggman.timer;
		OBJECT(current_object)->map_eggman.timer = timer - 1u;
		if (timer == 1u)
		{
			allocate_object_outside_loop(&free_count, &object);
			if ((uint8_t)free_count != 0u)
			{
				OBJECT(object)->handler = map_eggman_hands_update_ADDRESS;
				OBJECT(object)->map_eggman_hands.timer = 0xF0u;
				AS16(OBJECT(object)->map_eggman_hands.frame_step) = 0u;
				OBJECT(current_object)->animation_frame++;
				mem_write16(video_layer_disable, mem_read16(video_layer_disable) | 0xEu);
				OBJECT(current_object)->map_eggman.timer = 0x186u;
				OBJECT(current_object)->kind = 0u;
				OBJECT(current_object)->map_eggman.gesture_list = map_eggman_gesture_frames_ADDRESS;
				OBJECT(current_object)->map_eggman.unused_step = 0u;
			}
		}
		OBJECT(current_object)->sprite_frame = 0x5760u;
		OBJECT(current_object)->sprite_palette = 0x680u;
		OBJECT(current_object)->sprite_scale = 0x100u;
		OBJECT(current_object)->sprite_layer = 1u;
		OBJECT(current_object)->screen_x = 0xB7;
		OBJECT(current_object)->screen_y = 0xDF;
		OBJECT(current_object)->sprite_flags = 0u;
		OBJECT(current_object)->sprite_flags &= ~0x10Fu;
		OBJECT(current_object)->sprite_flags |= 0x107u;
		queue_sprite_in_layer();
		return;
	case 2:
		OBJECT(current_object)->map_eggman.timer--;
		if (OBJECT(current_object)->map_eggman.timer == 0u)
		{
			OBJECT(current_object)->animation_frame++;
			OBJECT(current_object)->kind = 0u;
			OBJECT(current_object)->map_eggman.timer = 3u;
			OBJECT(current_object)->map_eggman.hold_frames = 0x50u;
		}
		for (;;)
		{
			if ((uint8_t)(int8_t)mem_read8(OBJECT(current_object)->map_eggman.gesture_list + OBJECT(current_object)->kind) == 7u)
			{
				sound_id = 0x21u;
				queue_sound(sound_id, &ignored, &ignored);
			}
			gesture = (uint16_t)(int8_t)mem_read8(OBJECT(current_object)->map_eggman.gesture_list + OBJECT(current_object)->kind);
			if ((int16_t)gesture >= 0)
				break;
			OBJECT(current_object)->kind--;
		}
		OBJECT(current_object)->sprite_frame = ((gesture - 1u) << 3) + 0x5760u;
		OBJECT(current_object)->sprite_palette = 0x680u;
		OBJECT(current_object)->sprite_scale = 0x100u;
		OBJECT(current_object)->sprite_layer = 1u;
		OBJECT(current_object)->screen_x = 0xB7;
		OBJECT(current_object)->screen_y = 0xDF;
		OBJECT(current_object)->sprite_flags = 0u;
		OBJECT(current_object)->sprite_flags &= ~0x10Fu;
		OBJECT(current_object)->sprite_flags |= 0x107u;
		queue_sprite_in_layer();
		if ((OBJECT(current_object)->map_eggman.timer & 3u) == 3u)
		{
			OBJECT(current_object)->kind++;
			OBJECT(current_object)->map_eggman.unused_step++;
			if (OBJECT(current_object)->map_eggman.unused_step == 7u)
				OBJECT(current_object)->map_eggman.unused_step = 0u;
		}
		return;
	case 3:
		if (map_screen_step == 4u)
		{
			delete_current_object();
			return;
		}
		OBJECT(current_object)->map_eggman.timer--;
		OBJECT(current_object)->sprite_frame = 0x57C8u;
		OBJECT(current_object)->sprite_frame += (uint32_t)OBJECT(current_object)->kind << 3;
		if (OBJECT(current_object)->map_eggman.timer != 0u)
		{
		map_eggman_hold_and_loop:
			if (OBJECT(current_object)->map_eggman.hold_frames == 0u)
			{
				if (OBJECT(current_object)->map_eggman.sound_step == 1u)
				{
					OBJECT(current_object)->map_eggman.sound_step = 2u;
					sound_id = stage_map_sounds_3[stage];
					queue_sound(sound_id, &ignored, &ignored);
					if (stage == 9u)
					{
						map_stage_09_sound_count++;
						if ((int8_t)map_stage_09_sound_count >= 0x5A)
						{
							map_stage_09_sound_count = 0x5Au;
							sound_id = 0x95u;
							queue_sound(sound_id, &ignored, &ignored);
						}
					}
				}
				if ((int16_t)AS16(OBJECT(current_object)->kind) > 5)
				{
					OBJECT(current_object)->kind = 4u;
					OBJECT(current_object)->map_eggman.timer = 5u;
				}
			}
			else
			{
				if (OBJECT(current_object)->map_eggman.sound_step == 0u)
				{
					OBJECT(current_object)->map_eggman.sound_step = 1u;
					sound_id = stage_map_sounds_2[stage];
					queue_sound(sound_id, &ignored, &ignored);
				}
				OBJECT(current_object)->kind = 3u;
				OBJECT(current_object)->map_eggman.hold_frames--;
			}
		}
		else
		{
			OBJECT(current_object)->kind++;
			OBJECT(current_object)->map_eggman.timer = 3u;
			if ((int8_t)OBJECT(current_object)->kind >= 4)
				goto map_eggman_hold_and_loop;
		}
		OBJECT(current_object)->sprite_palette = 0x680u;
		OBJECT(current_object)->sprite_scale = 0x100u;
		OBJECT(current_object)->sprite_layer = 1u;
		OBJECT(current_object)->screen_x = 0xB7;
		OBJECT(current_object)->screen_y = 0xDF;
		OBJECT(current_object)->sprite_flags = 0u;
		OBJECT(current_object)->sprite_flags &= ~0x10Fu;
		OBJECT(current_object)->sprite_flags |= 0x107u;
		queue_sprite_in_layer();
		return;
	default:
		unknown_code(0x96188u + OBJECT(current_object)->animation_frame * 3u);
	}
}

void map_eggman_hands_update(void)
{
	if (OBJECT(current_object)->map_eggman_hands.timer == 0u)
		return;
	OBJECT(current_object)->map_eggman_hands.timer--;
	OBJECT(current_object)->sprite_frame = ((uint32_t)OBJECT(current_object)->map_eggman_hands.frame_step << 3) + 0x5798u;
	OBJECT(current_object)->sprite_palette = 0x690u;
	OBJECT(current_object)->sprite_scale = 0x100u;
	OBJECT(current_object)->sprite_layer = 0u;
	OBJECT(current_object)->screen_x = 0xB7;
	OBJECT(current_object)->screen_y = 0x9B;
	OBJECT(current_object)->sprite_flags = 0u;
	OBJECT(current_object)->sprite_flags &= ~0xFu;
	OBJECT(current_object)->sprite_flags |= 7u;
	queue_sprite_in_layer();
	if ((OBJECT(current_object)->map_eggman_hands.timer & 3u) != 3u)
		return;
	OBJECT(current_object)->kind++;
	OBJECT(current_object)->map_eggman_hands.frame_step++;
	if (OBJECT(current_object)->map_eggman_hands.frame_step != 6u)
		return;
	OBJECT(current_object)->map_eggman_hands.frame_step = 0u;
}

void start_map_trail(void)
{
	uint32_t trail = 0;

	OBJECT(current_object)->start_map_trail.trail_done = 0;
	trail = stage_map_trails[stage];
	if ((int32_t)trail <= 0)
		OBJECT(current_object)->start_map_trail.trail_done = -1;
	OBJECT(current_object)->start_map_trail.segment_frames = mem_read16(trail);
	OBJECT(current_object)->start_map_trail.segment_timer = mem_read16(trail);
	trail += 2u;
	next_map_trail_segment(trail);
}

void next_map_trail_segment(uint32_t trail_point)
{
	uint16_t end_x = 0, end_y = 0, start_x = 0, start_y = 0;
	uint32_t segment_frames = 0, step_y = 0;

	start_x = mem_read16(trail_point);
	trail_point += 2u;
	start_y = mem_read16(trail_point);
	trail_point += 2u;
	end_x = mem_read16(trail_point);
	end_y = mem_read16(trail_point + 2u);
	OBJECT(current_object)->next_map_trail_segment.next_trail_point = trail_point;
	if ((int16_t)end_x >= 0)
	{
		OBJECT(current_object)->next_map_trail_segment.base_x = start_x;
		OBJECT(current_object)->next_map_trail_segment.base_y = start_y;
		OBJECT(current_object)->next_map_trail_segment.target_x = end_x;
		OBJECT(current_object)->next_map_trail_segment.target_y = end_y;
		segment_frames = OBJECT(current_object)->next_map_trail_segment.segment_frames;
		step_y = divide_s32(((uint32_t)(uint16_t)OBJECT(current_object)->next_map_trail_segment.base_y - (uint16_t)OBJECT(current_object)->next_map_trail_segment.target_y) << 16, segment_frames);
		OBJECT(current_object)->next_map_trail_segment.step_x = divide_s32(((uint32_t)(uint16_t)OBJECT(current_object)->next_map_trail_segment.base_x - (uint16_t)OBJECT(current_object)->next_map_trail_segment.target_x) << 16, segment_frames);
		OBJECT(current_object)->next_map_trail_segment.step_y = step_y;
		OBJECT(current_object)->next_map_trail_segment.offset_x = 0;
		OBJECT(current_object)->z_int = 0;
		OBJECT(current_object)->animation_speed = 0u;
		OBJECT(current_object)->animation_timer = 0;
	}
	else
		OBJECT(current_object)->next_map_trail_segment.trail_done = -1;
}

void draw_map_trail_dot(uint32_t object)
{
	uint16_t dot_y = 0, object_2 = 0;
	uint32_t ignored = 0, trail_point = 0;

	allocate_object_2(&ignored, &ignored, &object);
	current_object = object;
	AS32(screen_sprite_block) = object;
	if ((int8_t)AT8(OBJECT(object_cursor)->animation, 2) >= 0)
	{
		AS32(OBJECT(object_cursor)->speed_z_int) -= AS32(OBJECT(object_cursor)->speed_x_int);
		AS32(OBJECT(object_cursor)->z_int) -= AS32(OBJECT(object_cursor)->speed_y_int);
	}
	dot_y = (uint16_t)(((int32_t)AS32(OBJECT(object_cursor)->z_int) >> 16) + OBJECT(object_cursor)->speed_x_fraction);
	OBJECT(current_object)->screen_x = ((int32_t)AS32(OBJECT(object_cursor)->speed_z_int) >> 16) + AT16(OBJECT(object_cursor)->flags, 2);
	OBJECT(current_object)->screen_y = dot_y;
	OBJECT(current_object)->sprite_layer = 1u;
	OBJECT(current_object)->sprite_scale = 0x100u;
	OBJECT(current_object)->sprite_flags = 0u;
	OBJECT(current_object)->sprite_frame = 0x64E0u;
	OBJECT(current_object)->sprite_palette = 0x660u;
	queue_sprite_in_layer();
	current_object = object_cursor;
	object_2 = (uint16_t)AS32(screen_sprite_block);
	free_object_2(object_2);
	if ((int8_t)AT8(OBJECT(object_cursor)->animation, 2) >= 0)
	{
		AT16(OBJECT(object_cursor)->floor_z, 1)--;
		if (AT8(OBJECT(object_cursor)->floor_z, 1) == 0u)
		{
			AT16(OBJECT(object_cursor)->floor_z, 1) = AT16(OBJECT(object_cursor)->animation, 3);
			trail_point = AT32(OBJECT(object_cursor)->y, 1);
			next_map_trail_segment(trail_point);
		}
	}
}

uint32_t draw_map_stage_markers(uint32_t r11)
{
	uint8_t current_from_end = 0, current_marker = 0;
	uint32_t groups_left = 0, layout = 0, markers_left = 0;

	current_object = 0x209102u;
	current_marker = stage_map_markers[stage];
	if ((int8_t)current_marker < 0)
		return r11;
	layout = map_marker_layouts_ADDRESS;
	groups_left = 2u;
	do
	{
		markers_left = 7u;
		current_from_end = (uint8_t)(uint16_t)(7u - current_marker);
		do
		{
			mem_write16(current_object + 2u, 0xAu);
			mem_write16(current_object + 8u, 0x100u);
			mem_write16(current_object + 0xAu, mem_read16(layout));
			layout += 2u;
			mem_write16(current_object + 0xEu, mem_read16(layout));
			layout += 2u;
			mem_write16(current_object + 0xCu, mem_read16(layout));
			layout += 2u;
			mem_write16(current_object + 4u, mem_read16(layout));
			layout += 2u;
			mem_write16(current_object + 6u, mem_read16(layout));
			layout += 2u;
			if ((int8_t)current_from_end < (int8_t)markers_left && ((uint8_t)(current_from_end + 1u) != (uint8_t)markers_left || (int8_t)AT8(OBJECT(object_cursor)->animation, 2) < 0))
				mem_write16(current_object + 6u, mem_read16(layout));
			layout = (layout & ~0xFFFFu) | (uint16_t)(layout + 2u);
			if ((int8_t)AT8(OBJECT(object_cursor)->animation, 2) >= 0)
			{
			draw_map_stage_markers_show_marker:
				queue_sprite_in_layer();
				current_object = current_object + 0x30u;
			}
			else
			{
				r11 = stage_map_markers[stage];
				if ((int8_t)r11 < 0)
					goto draw_map_stage_markers_show_marker;
				r11 = r11 * 0xCu;
				if (mem_read16(current_object + 4u) != ROM16(map_number_badge_sprites_ADDRESS + r11))
					goto draw_map_stage_markers_show_marker;
				AT16(OBJECT(object_cursor)->floor_z_int, 1)++;
				if ((OBJECT(object_cursor)->floor_z_int & 0x800u) != 0u)
					goto draw_map_stage_markers_show_marker;
			}
			markers_left--;
		} while (markers_left != 0u);
		groups_left--;
	} while (groups_left != 0u);
	current_object = object_cursor;
	return r11;
}

void blink_current_map_marker(void)
{
	uint16_t current_marker = 0;
	uint32_t record = 0;

	if (OBJECT(current_object)->blink_current_map_marker.step_timer != 0u)
	{
		OBJECT(current_object)->blink_current_map_marker.step_timer--;
		if ((OBJECT(current_object)->blink_current_map_marker.step_timer & 8u) != 0u)
		{
			current_marker = stage_map_markers[stage];
			if ((int8_t)current_marker >= 0)
			{
				record = 0x90000u | (uint16_t)(0x6C6Fu + ((uint32_t)current_marker << 3));
				record = (record & ~0xFFFFu) | (uint16_t)(record + current_marker * 2u);
				OBJECT(current_object)->sprite_scale = 0x100u;
				OBJECT(current_object)->sprite_flags = 0xAu;
				OBJECT(current_object)->sprite_layer = mem_read16(record);
				record += 2u;
				OBJECT(current_object)->screen_x = mem_read16(record);
				record += 2u;
				OBJECT(current_object)->screen_y = mem_read16(record);
				record += 2u;
				OBJECT(current_object)->sprite_frame = mem_read16(record);
				OBJECT(current_object)->sprite_palette = mem_read16(record + 2u);
				queue_sprite_in_layer();
			}
		}
	}
}

uint32_t start_window_wipe(uint32_t record)
{
	uint16_t left = 0, right = 0, top = 0;
	uint32_t bottom = 0, frames = 0, ignored = 0;

	fill_value = (fill_value & ~0xFFFFu) | 0x1111u;
	set_halfwords(VIDEO_RAM + 0xE000u, 0x7000u, fill_value);
	mem_write32(video_control, mem_read32(video_control) & ~0x800u);
	mem_write32(video_layer_disable, mem_read32(video_layer_disable) | 0x20u);
	mem_write32(video_layer_disable, mem_read32(video_layer_disable) | 0x8000u);
	mem_write32(video_layer_disable, mem_read32(video_layer_disable) | 0x400u);
	mem_write16(video_bitmap_scroll_x, 0u);
	mem_write16(video_bitmap_scroll_y, 0xE0u);
	mem_write16(mixer_bitmap, mem_read16(mixer_bitmap) & ~0xFu);
	mem_write16(mixer_bitmap, 0x2Fu);
	left = mem_read16(record);
	record += 2u;
	top = mem_read16(record);
	frames = mem_read16(record + 2u);
	bottom = top + 0x80u;
	right = (uint16_t)(left + 0x100u);
	if ((int16_t)right >= 0x140)
		right = 0x140u;
	if ((int16_t)bottom >= 0xE0)
		bottom = 0xE0u;
	if ((int16_t)left <= 0)
		left = window_wipe_default_left;
	if ((int16_t)top <= 0)
		top = window_wipe_default_top;
	window_wipe_left = left;
	window_wipe_top = top;
	window_wipe_right = right;
	window_wipe_bottom = bottom;
	window_wipe_frames = frames;
	bottom = divide_s32((0u - bottom + 0xE0u) << 16, frames);
	window_wipe_left_speed = divide_s32((uint32_t)left << 16, frames);
	window_wipe_right_speed = divide_s32((0u - right + 0x140u) << 16, frames);
	window_wipe_top_speed = divide_s32((uint32_t)top << 16, frames);
	window_wipe_bottom_speed = bottom;
	window_wipe_left_edge = 0;
	window_wipe_right_edge = 0x1400000;
	window_wipe_top_edge = 0;
	window_wipe_bottom_edge = 0xE00000;
	if (window_wipe_closing != 0u)
	{
		AS16(window_wipe_left_edge) = window_wipe_left;
		AS16(window_wipe_right_edge) = window_wipe_right;
		AS16(window_wipe_top_edge) = window_wipe_top;
		AS16(window_wipe_bottom_edge) = window_wipe_bottom;
		window_wipe_left_edge = (uint32_t)window_wipe_left_edge << 16;
		window_wipe_right_edge = (uint32_t)window_wipe_right_edge << 16;
		window_wipe_top_edge = (uint32_t)window_wipe_top_edge << 16;
		window_wipe_bottom_edge = (uint32_t)window_wipe_bottom_edge << 16;
		window_wipe_left_speed = 0u - window_wipe_left_speed;
		window_wipe_right_speed = 0u - window_wipe_right_speed;
		window_wipe_top_speed = 0u - window_wipe_top_speed;
		window_wipe_bottom_speed = 0u - window_wipe_bottom_speed;
	}
	update_screen_wipe(&ignored, &ignored, &ignored, &frames);
	mem_write32(video_layer_disable, mem_read32(video_layer_disable) & ~0x20u);
	return bottom;
}

void update_screen_wipe(uint32_t *p_r1, uint32_t *p_r2, uint32_t *p_r3, uint32_t *p_edges_done)
{
	uint32_t edges_done = *p_edges_done;
	uint16_t clip_left = 0;
	uint32_t clip_bottom = 0, clip_right = 0, clip_top = 0;

	mem_write16(video_layer_disable, mem_read16(video_layer_disable) & ~0xF0u);
	window_wipe_left_edge += window_wipe_left_speed;
	window_wipe_right_edge -= window_wipe_right_speed;
	window_wipe_top_edge += window_wipe_top_speed;
	window_wipe_bottom_edge -= window_wipe_bottom_speed;
	clip_left = (uint16_t)((int32_t)window_wipe_left_edge >> 16);
	clip_right = (int32_t)window_wipe_right_edge >> 16;
	clip_top = (int32_t)window_wipe_top_edge >> 16;
	clip_bottom = (int32_t)window_wipe_bottom_edge >> 16;
	edges_done &= ~0xFFu;
	if ((window_wipe_closing & 1u) != 0u)
	{
		if ((int16_t)clip_left <= 0)
		{
			edges_done |= 1u;
			clip_left = 0u;
			window_wipe_left_edge = 0;
		}
		if ((int16_t)clip_right >= 0x13F)
		{
			edges_done |= 2u;
			clip_right = (clip_right & ~0xFFFFu) | 0x13Fu;
			window_wipe_right_edge = 0x13F0000;
		}
		if ((int16_t)clip_top <= 0)
		{
			edges_done |= 8u;
			clip_top = 0u;
			window_wipe_top_edge = 0;
		}
		if ((int16_t)clip_bottom >= 0xE0)
		{
			edges_done |= 4u;
			clip_bottom = (clip_bottom & ~0xFFFFu) | 0xDFu;
			window_wipe_bottom_edge = 0xDF0000;
		}
	}
	else
	{
		if ((int16_t)window_wipe_left <= (int16_t)clip_left)
		{
			edges_done |= 1u;
			window_wipe_left_edge = (uint32_t)window_wipe_left << 16;
			clip_left = window_wipe_left;
		}
		if ((int16_t)clip_right <= (int16_t)window_wipe_right)
		{
			edges_done |= 2u;
			clip_right = (uint32_t)window_wipe_right << 16;
			window_wipe_right_edge = clip_right;
			clip_right |= window_wipe_right;
		}
		if ((int16_t)window_wipe_top <= (int16_t)clip_top)
		{
			edges_done |= 4u;
			clip_top = (uint32_t)window_wipe_top << 16;
			window_wipe_top_edge = clip_top;
			clip_top |= window_wipe_top;
		}
		if ((int16_t)clip_bottom <= (int16_t)window_wipe_bottom)
		{
			edges_done |= 8u;
			clip_bottom = (uint32_t)window_wipe_bottom << 16;
			window_wipe_bottom_edge = clip_bottom;
			clip_bottom |= window_wipe_bottom;
		}
	}
	mem_write16(video_bitmap_clip_left, clip_left);
	mem_write16(video_bitmap_clip_top, clip_top);
	mem_write16(video_bitmap_clip_right, clip_right);
	mem_write16(video_bitmap_clip_bottom, clip_bottom);
	if ((uint16_t)edges_done != 0xFFFFu)
	{
		*p_r1 = clip_right;
		*p_r2 = clip_top;
		*p_r3 = clip_bottom;
		*p_edges_done = edges_done;
		return;
	}
	mem_write8(video_layer_disable, mem_read8(video_layer_disable) | 0x20u);
	*p_r1 = clip_right;
	*p_r2 = clip_top;
	*p_r3 = clip_bottom;
	*p_edges_done = edges_done;
}

void run_title_screen(uint32_t r11, uint32_t *p_r4, uint32_t *p_r8, uint32_t *p_r9, uint32_t *p_r10)
{
	uint32_t r4 = *p_r4;
	uint32_t r8 = *p_r8;
	uint32_t r9 = *p_r9;
	uint32_t r10 = *p_r10;
	uint32_t background_script = 0, count = 0, ignored = 0, object = 0, palette_list = 0, script = 0;
	uint8_t cnt_bits = 0;

	run_sega_logo(r11, &r4, &r8, &r9, &r10);
	if (sega_logo_step != 7u)
	{
		*p_r4 = r4;
		*p_r8 = r8;
		*p_r9 = r9;
		*p_r10 = r10;
		return;
	}
	if (title_scene_built != 0u)
	{
		*p_r4 = r4;
		*p_r8 = r8;
		*p_r9 = r9;
		*p_r10 = r10;
		return;
	}
	cnt_bits = mem_read8(io_cnt);
	mem_write8(io_cnt, cnt_bits & ~2u);
	display_off_frames = 3u;
	color_offset_red = 0u;
	color_offset_blue = 0u;
	color_offset_green = 0u;
	title_sonic_step = 0;
	title_step_timer = 0;
	title_timer = 0;
	title_done = 0u;
	title_ray_mighty_started = 0u;
	title_ray_mighty_frame = 0u;
	title_ray_mighty_delay = 3u;
	title_ray_mighty_wait = 0x28u;
	unused_title_word = 0x58u;
	clear_sprite_buckets();
	mem_write8(video_layer_disable, mem_read8(video_layer_disable) | 0x2Fu);
	mixer_blend = 0u;
	mem_write32(video_control, mem_read32(video_control) & ~0x400u);
	mem_write16(video_control, mem_read16(video_control) | 0x400u);
	vblank_copies_pending = 0u;
	vblank_copy_count = 0u;
	palette_list = tile_bank_1_palette_list_ADDRESS;
	load_palette_list(palette_list, &ignored, &object);
	script = title_sky_script_ADDRESS;
	run_vram_script(&count, &script, &object);
	background_script = title_waving_background_script_ADDRESS;
	run_vram_script(&count, &background_script, &object);
	nbg2_scroll_x = 0u;
	nbg2_scroll_y = 0xFFE8u;
	nbg3_scroll_x = 0u;
	nbg3_scroll_y = 0xFFE0u;
	nbg2_pages = 0u;
	nbg3_pages = 0x5050505u;
	free_objects = free_object_stack_ADDRESS;
	first_object = first_object_address;
	last_object = last_object_address;
	object = allocate_object_or_start_list(object);
	OBJECT(object)->handler = title_land_slide_ADDRESS;
	AS8(OBJECT(object)->flags) = 0u;
	allocate_object(&ignored, &object);
	OBJECT(object)->handler = title_emblem_animate_ADDRESS;
	AS8(OBJECT(object)->flags) = 0u;
	allocate_object(&ignored, &object);
	OBJECT(object)->handler = timed_spawner_run_ADDRESS;
	AS8(OBJECT(object)->flags) = 0u;
	allocate_object(&ignored, &object);
	OBJECT(object)->handler = title_sky_wave_ADDRESS;
	AS8(OBJECT(object)->flags) = 0u;
	title_scene_built = 1u;
	mem_write16(video_backdrop_control, 0u);
	mem_write16(video_layer_disable, mem_read16(video_layer_disable) & 0xD0u);
	mem_write16(video_layer_disable, mem_read16(video_layer_disable) | 0x23u);
	*p_r4 = r4;
	*p_r8 = r8;
	*p_r9 = r9;
	*p_r10 = background_script;
}

void title_emblem_animate(void)
{
	uint16_t animation = 0, emblem_bottom_left = 0, floor_z = 0, sonic_frame = 0, sonic_y = 0, speed_y = 0, x = 0, y = 0, z = 0;
	uint32_t emblem_part = 0, ignored = 0, loop_entry = 0, sonic_step = 0;

	if (OBJECT(current_object)->title.started == 0u)
	{
		allocate_object_2(&ignored, &ignored, &emblem_part);
		OBJECT(current_object)->speed_y = emblem_part;
		allocate_object_2(&ignored, &ignored, &emblem_part);
		OBJECT(current_object)->z = emblem_part;
		allocate_object_2(&ignored, &ignored, &emblem_part);
		OBJECT(current_object)->title.emblem_bottom_left = emblem_part;
		allocate_object_2(&ignored, &ignored, &emblem_part);
		OBJECT(current_object)->animation = emblem_part;
		allocate_object_2(&ignored, &ignored, &emblem_part);
		OBJECT(current_object)->x = emblem_part;
		allocate_object_2(&ignored, &ignored, &emblem_part);
		OBJECT(current_object)->y = emblem_part;
		allocate_object_2(&ignored, &ignored, &emblem_part);
		OBJECT(current_object)->floor_z = emblem_part;
		OBJECT(current_object)->title.started = 1u;
	}
	current_object = OBJECT(object_cursor)->speed_y;
	OBJECT(current_object)->sprite_layer = 5u;
	OBJECT(current_object)->screen_x = 0x5A;
	OBJECT(current_object)->sprite_frame = 0x52E0u;
	OBJECT(current_object)->screen_y = 0x72;
	draw_title_emblem_part();
	current_object = OBJECT(object_cursor)->z;
	OBJECT(current_object)->sprite_layer = 5u;
	OBJECT(current_object)->screen_x = 0xE6;
	OBJECT(current_object)->sprite_frame = 0x52E8u;
	OBJECT(current_object)->screen_y = 0x72;
	draw_title_emblem_part();
	current_object = OBJECT(object_cursor)->title.emblem_bottom_left;
	OBJECT(current_object)->sprite_layer = 0u;
	OBJECT(current_object)->screen_x = 0x63;
	OBJECT(current_object)->screen_y = 0x98;
	OBJECT(current_object)->sprite_frame = 0x52F0u;
	draw_title_emblem_part();
	current_object = OBJECT(object_cursor)->animation;
	OBJECT(current_object)->sprite_layer = 0u;
	OBJECT(current_object)->screen_x = 0xDB;
	OBJECT(current_object)->screen_y = 0x98;
	OBJECT(current_object)->sprite_frame = 0x52F8u;
	OBJECT(current_object)->sprite_flags = 0u;
	OBJECT(current_object)->sprite_scale = 0x100u;
	draw_title_emblem_part();
	current_object = OBJECT(object_cursor)->x;
	if (title_sonic_step < 0x14)
	{
		sonic_y = (uint16_t)(0x70u - title_sonic_rise_heights[(uint8_t)title_sonic_step]);
		sonic_frame = (uint8_t)title_sonic_step;
	}
	else
	{
		for (;;)
		{
			sonic_step = (uint8_t)title_sonic_step;
			loop_entry = 0x96E02u + (uint8_t)(sonic_step - 0x14u);
			if (ROM8(loop_entry) != 0xFFu)
				break;
			title_sonic_step = 0x14;
		}
		sonic_y = 0x46u;
		sonic_frame = (uint8_t)(ROM8(loop_entry) - 1u);
	}
	title_timer++;
	if (title_timer < 0x258)
	{
		OBJECT(current_object)->screen_y = sonic_y;
		OBJECT(current_object)->sprite_palette = 0x100u;
		OBJECT(current_object)->sprite_frame = sonic_frame * 8u + 0x5300u;
		OBJECT(current_object)->sprite_layer = 0u;
		OBJECT(current_object)->screen_x = 0xA1;
		OBJECT(current_object)->sprite_flags = 0u;
		OBJECT(current_object)->sprite_scale = 0x100u;
		OBJECT(current_object)->sprite_flags |= 0x100u;
		queue_sprite_in_layer();
		title_step_timer++;
		if (title_ray_mighty_started != 0u)
		{
			if (title_ray_mighty_wait == 0u)
			{
				title_ray_mighty_delay--;
				if (title_ray_mighty_delay == 0u)
				{
					title_ray_mighty_delay = 2u;
					title_ray_mighty_frame++;
					if (title_mighty_frames[title_ray_mighty_frame] == 0xFFFFu)
						title_ray_mighty_frame--;
				}
				current_object = OBJECT(object_cursor)->y;
				OBJECT(current_object)->sprite_frame = title_mighty_frames[title_ray_mighty_frame];
				OBJECT(current_object)->sprite_palette = 0x840u;
				OBJECT(current_object)->sprite_layer = 2u;
				OBJECT(current_object)->screen_y = 0x81;
				OBJECT(current_object)->screen_x = 0xDA;
				OBJECT(current_object)->sprite_flags = 0u;
				OBJECT(current_object)->sprite_flags &= ~0xFu;
				OBJECT(current_object)->sprite_flags |= 4u;
				OBJECT(current_object)->sprite_scale = 0x100u;
				OBJECT(current_object)->sprite_flags |= 0x100u;
				queue_sprite_in_layer();
				current_object = OBJECT(object_cursor)->floor_z;
				OBJECT(current_object)->sprite_frame = title_ray_frames[title_ray_mighty_frame];
				OBJECT(current_object)->sprite_palette = 0x830u;
				OBJECT(current_object)->sprite_layer = 2u;
				OBJECT(current_object)->screen_y = 0x81;
				OBJECT(current_object)->screen_x = 0x66;
				OBJECT(current_object)->sprite_flags = 0u;
				OBJECT(current_object)->sprite_flags &= ~0xFu;
				OBJECT(current_object)->sprite_flags |= 4u;
				OBJECT(current_object)->sprite_scale = 0x100u;
				OBJECT(current_object)->sprite_flags |= 0x100u;
				queue_sprite_in_layer();
			}
			else
				title_ray_mighty_wait--;
		}
		if (title_step_timer < 0)
			return;
		title_sonic_step++;
		if (title_sonic_step > 4)
		{
			if (title_sonic_step >= 0x13)
			{
				if (title_sonic_step >= 0x14)
					title_step_timer = -3;
				else
				{
					title_step_timer = -0x3C;
					title_ray_mighty_started = 1u;
				}
			}
			else
				title_step_timer = -4;
		}
		else
			title_step_timer = -2;
		return;
	}
	title_done = 1u;
	speed_y = (uint16_t)OBJECT(object_cursor)->speed_y;
	free_object_2(speed_y);
	z = (uint16_t)OBJECT(object_cursor)->z;
	free_object_2(z);
	emblem_bottom_left = (uint16_t)OBJECT(object_cursor)->title.emblem_bottom_left;
	free_object_2(emblem_bottom_left);
	animation = (uint16_t)OBJECT(object_cursor)->animation;
	free_object_2(animation);
	x = (uint16_t)OBJECT(object_cursor)->x;
	free_object_2(x);
	y = (uint16_t)OBJECT(object_cursor)->y;
	free_object_2(y);
	floor_z = (uint16_t)OBJECT(object_cursor)->floor_z;
	free_object_2(floor_z);
	delete_current_object();
}

void draw_title_emblem_part(void)
{
	OBJECT(current_object)->sprite_palette = 0u;
	OBJECT(current_object)->sprite_flags = 0u;
	OBJECT(current_object)->sprite_flags |= 0x200u;
	OBJECT(current_object)->sprite_scale = 0x100u;
	queue_sprite_in_layer();
}

void title_land_slide(void)
{
	if (OBJECT(current_object)->title.started == 0u)
	{
		OBJECT(current_object)->sprite_frame = 0xB6D0u;
		OBJECT(current_object)->sprite_palette = 0xD70u;
		OBJECT(current_object)->sprite_scale = 0x100u;
		OBJECT(current_object)->sprite_flags = 0u;
		OBJECT(current_object)->sprite_layer = 7u;
		OBJECT(current_object)->screen_y = 0x7A;
		OBJECT(current_object)->screen_x = 0xA0;
		OBJECT(current_object)->speed_y = 0x1900000;
		AS16(OBJECT(current_object)->title.started) = 1u;
	}
	if (title_done == 0u)
	{
		OBJECT(current_object)->speed_y -= 0x8000u;
		OBJECT(current_object)->screen_x = (uint32_t)OBJECT(current_object)->speed_y >> 16;
		queue_sprite_in_layer();
		return;
	}
	delete_current_object();
}

void timed_spawner_run(void)
{
	uint32_t ignored = 0, object = 0, spawn_record = 0, spawned_handler = 0;
	uint16_t spawn_frame = 0;

	if (OBJECT(current_object)->timed_spawner.started == 0u)
	{
		OBJECT(current_object)->timed_spawner.started = 1u;
		OBJECT(current_object)->timed_spawner.list_index = 0u;
		OBJECT(current_object)->timed_spawner.timer = 0;
	}
	for (;;)
	{
		spawn_record = timed_spawns_ADDRESS + OBJECT(current_object)->timed_spawner.list_index * 4u;
		if (ROM16(spawn_record) == 0xFFFFu || title_done != 0u)
		{
			delete_current_object();
			return;
		}
		spawn_frame = ROM16(spawn_record);
		spawn_record += 2u;
		if (OBJECT(current_object)->timed_spawner.timer <= (int16_t)spawn_frame)
			break;
		OBJECT(current_object)->timed_spawner.list_index++;
		spawned_handler = ROM16(spawn_record) + 0x973B4u;
		allocate_object(&ignored, &object);
		OBJECT(object)->handler = spawned_handler;
		AS8(OBJECT(object)->flags) = 0u;
	}
	OBJECT(current_object)->timed_spawner.timer++;
}

void title_flying_craft_move(void)
{
	uint32_t ignored = 0, object = 0;

	if (OBJECT(current_object)->title.started == 0u)
	{
		OBJECT(current_object)->title.started = 1u;
		OBJECT(current_object)->sprite_frame = 0xBB58u;
		OBJECT(current_object)->screen_x = 0x154;
		OBJECT(current_object)->screen_y = 0x70;
		OBJECT(current_object)->sprite_layer = 7u;
		OBJECT(current_object)->sprite_scale = 0x100u;
		OBJECT(current_object)->sprite_flags = 0u;
		OBJECT(current_object)->sprite_palette = 0xEB0u;
		OBJECT(current_object)->title.timer = 0;
	}
	if (title_done != 0u)
	{
		delete_current_object();
		return;
	}
	OBJECT(current_object)->screen_x--;
	queue_sprite_in_layer();
	OBJECT(current_object)->title.timer++;
	if (OBJECT(current_object)->title.timer > 0xA)
	{
		OBJECT(current_object)->title.timer = 0;
		allocate_object(&ignored, &object);
		OBJECT(object)->handler = title_smoke_puff_animate_ADDRESS;
		OBJECT(object)->screen_x = OBJECT(current_object)->screen_x;
		OBJECT(object)->screen_x += 4u;
		OBJECT(object)->title.started = 0u;
	}
}
