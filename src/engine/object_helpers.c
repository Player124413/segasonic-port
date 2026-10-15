/**
 *    Name:        engine/object_helpers.c
 *    Description: next_random, child shadows, the off-screen tests, allocating and deleting objects from outside the object loop
 *    Version:     0.1.0
 *    Created:     12-10-2026
 *    Modified:    15-10-2026
 *    Author:      Sebastian Toyos
 *
 *    SegaSonic The Hedgehog (arcade, Sega System 32): the game's V60 program decompiled into C by
 *    tools/decompile.py, with the names of symbols.txt.
 */
#include "game.h"

void next_random(uint32_t *p_random_value, uint32_t *p_words_left, uint32_t *p_random_offset)
{
	uint32_t random_value = *p_random_value;
	uint32_t random_offset = 0, words_left = 0;

	random_offset = random_position;
	words_left = 2u;
	do
	{
		random_offset = (random_offset + mem_read16(random_offset + 0xD80000u)) & 0x7FFFFu;
		random_value = (random_value & ~0xFFFFu) | mem_read16(random_offset + 0xD80000u);
		random_value = (random_value << 16) | (random_value >> 16);
		words_left--;
	} while (words_left != 0u);
	random_position = random_offset;
	*p_random_value = random_value;
	*p_words_left = words_left;
	*p_random_offset = random_offset;
}

void create_child_shadow_unscaled(uint32_t shadow)
{
	uint32_t free_count = 0, ignored = 0, owner = 0;

	OBJECT(current_object)->child = 0xFFFFu;
	owner = current_object;
	allocate_object_2(&free_count, &ignored, &shadow);
	if (free_count != 0u)
	{
		OBJECT(shadow)->sprite_flags = 0x1000u;
		OBJECT(shadow)->sprite_palette = 0x9FFEu;
		OBJECT(shadow)->sprite_frame = 0x5D90u;
		AS8(OBJECT(shadow)->hitbox_size_x) = 0u;
		OBJECT(owner)->child = shadow;
		OBJECT(shadow)->parent = current_object;
	}
}

void create_child_shadow_9ffe(uint16_t scale, uint32_t shadow)
{
	uint32_t free_count = 0, ignored = 0;

	OBJECT(current_object)->child = 0xFFFFu;
	current_player = current_object;
	allocate_object_2(&free_count, &ignored, &shadow);
	if (free_count != 0u)
	{
		OBJECT(shadow)->sprite_flags = 0u;
		OBJECT(shadow)->sprite_palette = 0x9FFEu;
		OBJECT(shadow)->sprite_frame = 0x5D90u;
		OBJECT(shadow)->sprite_scale = scale;
		AS8(OBJECT(shadow)->hitbox_size_x) = 0u;
		OBJECT(current_player)->child = shadow;
		OBJECT(shadow)->parent = current_object;
	}
}

void create_child_shadow(uint16_t scale, uint32_t shadow)
{
	uint32_t free_count = 0, ignored = 0;

	OBJECT(current_object)->child = 0xFFFFu;
	current_player = current_object;
	allocate_object_2(&free_count, &ignored, &shadow);
	if (free_count != 0u)
	{
		OBJECT(shadow)->sprite_flags = 0u;
		OBJECT(shadow)->sprite_palette = 0x87A0u;
		OBJECT(shadow)->sprite_frame = 0x5D90u;
		OBJECT(shadow)->sprite_scale = scale;
		AS8(OBJECT(shadow)->hitbox_size_x) = 0u;
		OBJECT(current_player)->child = shadow;
		OBJECT(shadow)->parent = current_object;
	}
}

void draw_child_shadow(void)
{
	uint32_t depth = 0, ignored = 0;
	uint8_t shadow_blink = 0;

	if (OBJECT(current_object)->floor_block < 0 || (int16_t)OBJECT(current_object)->child < 0)
		return;
	current_object = LINKED(current_object, OBJECT(object_cursor)->child);
	OBJECT(current_object)->screen_x = OBJECT(object_cursor)->projected_x - view_x + OBJECT(object_cursor)->sprite_offset_x;
	OBJECT(current_object)->screen_y = ((int32_t)((uint32_t)OBJECT(object_cursor)->projected_y + OBJECT(object_cursor)->floor_z) >> 16) - view_y + OBJECT(object_cursor)->sprite_offset_y;
	OBJECT(current_object)->sprite_palette = 0x9FFEu;
	if ((OBJECT(object_cursor)->flags & 0x10u) == 0u)
	{
		depth = (uint16_t)(OBJECT(current_object)->screen_y - 0x10u);
		queue_sprite_at_depth(&ignored, &depth);
		current_object = (current_object & ~0xFFFFu) | (uint16_t)object_cursor;
	}
	else
	{
		OBJECT(current_object)->sprite_palette = 0x87A0u;
		shadow_blink = ~AS8(OBJECT(current_object)->hitbox_size_x);
		AS8(OBJECT(current_object)->hitbox_size_x) = shadow_blink;
		if (shadow_blink == 0u)
		{
			depth = (uint16_t)(OBJECT(current_object)->screen_y - 0x10u);
			queue_sprite_at_depth(&ignored, &depth);
			current_object = (current_object & ~0xFFFFu) | (uint16_t)object_cursor;
			return;
		}
	}
}

uint32_t is_floor_block_off_screen(void)
{
	uint32_t zero = 0;
	uint16_t floor_block = 0;
	uint32_t blocks_left = 0, cursor = 0, off_screen = 0;
	uint16_t listed_block = 0;

	off_screen = 0u;
	blocks_left = nearby_floor_block_count;
	cursor = nearby_floor_blocks_ADDRESS;
	if (blocks_left == 0u)
		off_screen = 0xFFFFFFFFu;
	else
	{
		floor_block = AS16(OBJECT(current_object)->floor_block);
		do
		{
			listed_block = mem_read16(cursor);
			zero = (uint16_t)(floor_block - listed_block) == 0u;
			cursor += 2u;
			blocks_left--;
		} while ((blocks_left != 0u) & (floor_block != listed_block));
		if (zero == 0u)
		{
			off_screen = 0xFFFFFFFFu;
			return off_screen;
		}
	}
	return off_screen;
}

uint32_t is_off_screen(void)
{
	uint32_t off_screen = 0;

	off_screen = 0u;
	if ((uint16_t)(OBJECT(current_object)->screen_x + 0x20u) >= 0x180u || (uint16_t)(OBJECT(current_object)->screen_y + 0x20u) >= 0x120u)
		off_screen = 0xFFFFFFFFu;
	return off_screen;
}

uint32_t is_off_screen_margin(uint16_t margin_x, uint16_t margin_y)
{
	uint32_t off_screen = 0;

	off_screen = 0u;
	if ((uint16_t)(0x140u + margin_x * 2u) <= (uint16_t)(OBJECT(current_object)->screen_x + margin_x) || (uint16_t)(0xE0u + margin_y * 2u) <= (uint16_t)(OBJECT(current_object)->screen_y + margin_y))
		off_screen = 0xFFFFFFFFu;
	return off_screen;
}

uint32_t check_left_screen(void)
{
	uint16_t edge_margin = 0;
	uint32_t off_screen = 0;

	off_screen = 0u;
	edge_margin = 0x20u;
	if ((OBJECT(current_object)->flags & 0x800u) == 0u)
		edge_margin = 0u;
	if (0x140u + edge_margin * 2u <= (uint16_t)(OBJECT(current_object)->screen_x + edge_margin) || 0xE0u + edge_margin * 2u <= (uint16_t)(OBJECT(current_object)->screen_y + edge_margin))
	{
		if ((OBJECT(current_object)->flags & 0x800u) != 0u)
			off_screen = 0xFFFFFFFFu;
	}
	else
		OBJECT(current_object)->flags |= 0x800u;
	return off_screen;
}

uint32_t check_left_screen_margin(uint16_t margin)
{
	uint32_t off_screen = 0;

	off_screen = 0u;
	if ((OBJECT(current_object)->flags & 0x800u) == 0u)
		margin = 0u;
	if ((uint16_t)(0x140u + margin * 2u) <= (uint16_t)(OBJECT(current_object)->screen_x + margin) || (uint16_t)(0xE0u + margin * 2u) <= (uint16_t)(OBJECT(current_object)->screen_y + margin))
	{
		if ((OBJECT(current_object)->flags & 0x800u) != 0u)
			off_screen = 0xFFFFFFFFu;
	}
	else
		OBJECT(current_object)->flags |= 0x800u;
	return off_screen;
}

uint32_t check_left_screen_margins(uint16_t margin, uint16_t unseen_margin)
{
	uint32_t off_screen = 0;

	off_screen = 0u;
	if ((OBJECT(current_object)->flags & 0x800u) == 0u)
		margin = (uint16_t)unseen_margin;
	if ((uint16_t)(0x140u + margin * 2u) <= (uint16_t)(OBJECT(current_object)->screen_x + margin) || (uint16_t)(0xE0u + margin * 2u) <= (uint16_t)(OBJECT(current_object)->screen_y + margin))
	{
		if ((OBJECT(current_object)->flags & 0x800u) != 0u)
			off_screen = 0xFFFFFFFFu;
	}
	else
		OBJECT(current_object)->flags |= 0x800u;
	return off_screen;
}

void allocate_object_outside_loop(uint32_t *p_free_count, uint32_t *p_object)
{
	uint32_t object = *p_object;
	uint32_t free_count = 0;

	free_objects = free_object_stack_ADDRESS;
	first_object = first_object_address;
	last_object = last_object_address;
	allocate_object(&free_count, &object);
	*p_free_count = free_count;
	*p_object = object;
}

void delete_object_outside_loop(uint32_t object, uint32_t *p_free_count, uint32_t *p_previous)
{
	uint32_t free_count = 0, previous = 0;

	free_objects = free_object_stack_ADDRESS;
	first_object = first_object_address;
	last_object = last_object_address;
	delete_object(object, &free_count, &previous);
	first_object_address = first_object;
	last_object_address = last_object;
	*p_free_count = free_count;
	*p_previous = previous;
}
