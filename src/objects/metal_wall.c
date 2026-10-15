/**
 *    Name:        objects/metal_wall.c
 *    Description: object type 0: the metal wall that rises and breaks (stage 7, the how to play demo)
 *    Version:     0.1.0
 *    Created:     12-10-2026
 *    Modified:    15-10-2026
 *    Author:      Sebastian Toyos
 *
 *    SegaSonic The Hedgehog (arcade, Sega System 32): the game's V60 program decompiled into C by
 *    tools/decompile.py, with the names of symbols.txt.
 */
#include "game.h"

void metal_wall_init(void)
{
	OBJECT(current_object)->handler = metal_wall_wait_ADDRESS;
	OBJECT(current_object)->metal_wall.rise_limit = 3;
	OBJECT(current_object)->sprite_palette = 0x8310u;
	OBJECT(current_object)->hitbox_size_x = 0x6Cu;
	OBJECT(current_object)->hitbox_size_y = 0x38u;
	OBJECT(current_object)->hitbox_offset_x = -0x10;
	OBJECT(current_object)->hitbox_offset_y = -0x30;
	OBJECT(current_object)->hitbox_height = 0x4Cu;
	OBJECT(current_object)->kind = 5u;
	OBJECT(current_object)->health = 4;
	OBJECT(current_object)->metal_wall.last_health = OBJECT(current_object)->health;
	OBJECT(current_object)->flags |= 2u;
	if (OBJECT(current_object)->metal_wall.flipped != 0u)
	{
		OBJECT(current_object)->sprite_flags |= 0x40u;
		OBJECT(current_object)->hitbox_size_x = 0x35u;
		OBJECT(current_object)->hitbox_size_y = 0x6Fu;
		OBJECT(current_object)->hitbox_offset_x = -0xD;
		OBJECT(current_object)->hitbox_offset_y = -0x60;
		OBJECT(current_object)->hitbox_height = 0x4Cu;
	}
	current_object = LINKED(current_object, OBJECT(current_object)->child);
	OBJECT(current_object)->sprite_palette = 0x8310u;
	OBJECT(current_object)->sprite_flags = OBJECT(object_cursor)->sprite_flags;
	current_object = LINKED(current_object, OBJECT(current_object)->child);
	OBJECT(current_object)->sprite_palette = 0x89B0u;
	OBJECT(current_object)->sprite_flags = OBJECT(object_cursor)->sprite_flags;
	current_object = LINKED(current_object, OBJECT(current_object)->child);
	OBJECT(current_object)->sprite_palette = 0x89C0u;
	OBJECT(current_object)->sprite_flags = OBJECT(object_cursor)->sprite_flags;
	current_object = LINKED(current_object, OBJECT(current_object)->child);
	OBJECT(current_object)->sprite_palette = 0x8260u;
	OBJECT(current_object)->sprite_flags = OBJECT(object_cursor)->sprite_flags;
	current_object = LINKED(current_object, OBJECT(current_object)->child);
	OBJECT(current_object)->sprite_palette = 0x8260u;
	OBJECT(current_object)->sprite_flags = OBJECT(object_cursor)->sprite_flags;
	current_object = LINKED(current_object, OBJECT(current_object)->child);
	OBJECT(current_object)->sprite_palette = 0x8260u;
	OBJECT(current_object)->direction = 3u;
	OBJECT(current_object)->sprite_flags = OBJECT(object_cursor)->sprite_flags;
	current_object = LINKED(current_object, OBJECT(current_object)->child);
	OBJECT(current_object)->sprite_palette = 0x8260u;
	OBJECT(current_object)->direction = 3u;
	OBJECT(current_object)->sprite_flags = OBJECT(object_cursor)->sprite_flags;
}

void metal_wall_wait(void)
{
	uint32_t octant = 0;

	OBJECT(current_object)->projected_x += 0x10u;
	OBJECT(current_object)->projected_y_int += 0x10u;
	octant = face_nearest_player(0u, 0u);
	OBJECT(current_object)->projected_x -= 0x10u;
	OBJECT(current_object)->projected_y_int -= 0x10u;
	if (nearest_player_distance > 0x80)
	{
		metal_wall_update(octant);
		return;
	}
	OBJECT(current_object)->metal_wall.dust_timer = 0x14u;
	OBJECT(current_object)->handler = metal_wall_rise_ADDRESS;
	metal_wall_rise(octant);
}

void metal_wall_rise(uint32_t octant)
{
	uint16_t frame_count = 0;

	frame_count = (uint16_t)OBJECT(current_object)->metal_wall.frame_count;
	OBJECT(current_object)->metal_wall.frame_count++;
	if ((int16_t)frame_count <= 2)
	{
		metal_wall_update(octant);
		return;
	}
	OBJECT(current_object)->metal_wall.frame_count = 0;
	OBJECT(current_object)->metal_wall.frame_index++;
	if (OBJECT(current_object)->metal_wall.frame_index <= OBJECT(current_object)->metal_wall.rise_limit)
	{
		metal_wall_update(octant);
		return;
	}
	OBJECT(current_object)->metal_wall.rise_limit = 7;
	OBJECT(current_object)->handler = metal_wall_update_ADDRESS;
	metal_wall_update(octant);
}

void metal_wall_update(uint32_t octant)
{
	uint32_t r1 = 0, r2 = 0, r11 = 0, block_offset = 0, deleted = 0, health = 0, ignored = 0, ring = 0, ring_count = 0, spawn_arg_1 = 0;

	if (OBJECT(current_object)->health <= 0)
	{
		OBJECT(current_object)->health = 0;
		OBJECT(current_object)->handler = metal_wall_broken_ADDRESS;
		if (OBJECT(current_object)->metal_wall.ring_count != 0u)
		{
			ring_count = OBJECT(current_object)->metal_wall.ring_count;
			scatter_rings_random(octant, ring_count, &r11, &ring);
		}
	}
	health = (uint8_t)OBJECT(current_object)->health;
	r1 = OBJECT(current_object)->metal_wall.last_health;
	if (health != r1)
	{
		OBJECT(current_object)->metal_wall.last_health = OBJECT(current_object)->health;
		OBJECT(current_object)->metal_wall.frame_index = metal_wall_damage_frames[health];
		if ((int8_t)(r1 - health) > 1)
			OBJECT(current_object)->z -= 0x16000u;
		OBJECT(current_object)->screen_x += 2u;
		spawn_arg_1 = OBJECT(current_object)->spawn_arg_1;
		r1 = (4u - spawn_arg_1) * 2u;
		if ((int32_t)r1 > 0)
			spawn_debris(spawn_arg_1, (uint16_t)r1, r11);
	}
	delete_if_off_screen(&deleted, &block_offset, &octant);
	if ((uint8_t)deleted == 1u)
		return;
	move_object_projected(&ignored, &r2, &ignored, &r11);
	fall_to_floor(&ignored, &r1, &r2);
	if ((uint8_t)OBJECT(current_object)->health == 0u)
	{
		metal_wall_draw();
		return;
	}
	add_to_touchable_list(ignored);
	metal_wall_draw();
}

void metal_wall_broken(void)
{
	uint32_t block_offset = 0, deleted = 0, slope_height = 0;

	delete_if_off_screen(&deleted, &block_offset, &slope_height);
	if ((uint8_t)deleted == 1u)
		return;
	update_screen_position_no_offset();
	metal_wall_draw();
}

void metal_wall_draw(void)
{
	uint8_t bucket = 0, health = 0;
	uint16_t dust_frame = 0, dust_x = 0, dust_y = 0, part_dy = 0, screen_offset = 0;
	uint32_t depth = 0, direction = 0, dust_sprite_base = 0, dust_y_2 = 0, flip_offset = 0, frame_index = 0, ignored = 0, offset_pair = 0, part_index = 0, parts_left = 0;

	OBJECT(current_object)->sprite_frame = metal_wall_sprites[(uint8_t)OBJECT(current_object)->metal_wall.frame_index];
	if ((uint8_t)OBJECT(current_object)->health == 0u)
	{
		bucket = 0xFEu;
		queue_sprite_in_bucket(bucket);
	}
	else
	{
		depth = (uint16_t)(OBJECT(current_object)->screen_y - 0x14u);
		queue_sprite_at_depth(&ignored, &depth);
	}
	parts_left = 3u;
	flip_offset = (uint32_t)OBJECT(object_cursor)->metal_wall.flipped << 4;
	do
	{
		current_object = LINKED(current_object, OBJECT(current_object)->child);
		part_index = (parts_left & ~0xFFu) | (uint8_t)(parts_left - 1u);
		frame_index = (uint8_t)OBJECT(object_cursor)->metal_wall.frame_index;
		offset_pair = metal_wall_part_tables[(part_index & ~0xFFu) | (uint8_t)(part_index + 3u)] + (uint8_t)(frame_index * 2u) + flip_offset;
		screen_offset = (uint16_t)(int8_t)mem_read8(offset_pair);
		part_dy = (uint16_t)(int8_t)mem_read8(offset_pair + 1u);
		OBJECT(current_object)->screen_x = OBJECT(object_cursor)->screen_x;
		OBJECT(current_object)->screen_x += screen_offset;
		OBJECT(current_object)->screen_y = OBJECT(object_cursor)->screen_y;
		OBJECT(current_object)->screen_y += part_dy;
		OBJECT(current_object)->sprite_frame = mem_read16(metal_wall_part_tables[part_index] + (uint8_t)OBJECT(object_cursor)->metal_wall.frame_index * 2u);
		health = (uint8_t)OBJECT(object_cursor)->health;
		if ((uint8_t)part_index != 2u)
		{
			bucket = 0xFEu;
			if (parts_left != 2u)
				bucket = 0xFFu;
		metal_wall_draw_part_in_bucket:
			queue_sprite_in_bucket(bucket);
		}
		else
		{
			bucket = 0xFEu;
			if (health == 0u)
				goto metal_wall_draw_part_in_bucket;
			depth = (uint16_t)(OBJECT(current_object)->screen_y - 0x20u);
			queue_sprite_at_depth(&ignored, &depth);
		}
		parts_left--;
	} while (parts_left != 0u);
	if (OBJECT(object_cursor)->metal_wall.dust_timer != 0u)
	{
		OBJECT(object_cursor)->metal_wall.dust_timer--;
		if ((OBJECT(object_cursor)->metal_wall.dust_timer & 1u) == 0u)
			return;
		dust_x = 0xFFF8u;
		dust_y = 6u;
		dust_sprite_base = 0u;
		parts_left = 4u;
		do
		{
			current_object = LINKED(current_object, OBJECT(current_object)->child);
			dust_frame = (uint16_t)(OBJECT(current_object)->direction + 1u);
			if ((int16_t)dust_frame > 7)
				dust_frame = 0u;
			if (parts_left == 2u)
			{
				dust_x = 0xFFF8u;
				dust_sprite_base = 0u;
			}
			OBJECT(current_object)->direction = dust_frame;
			direction = OBJECT(current_object)->direction;
			OBJECT(current_object)->sprite_frame = metal_wall_dust_sprites[dust_sprite_base + direction];
			if ((int32_t)parts_left <= 2)
			{
				screen_offset = (uint16_t)(0x20u - (direction << 2));
				dust_y -= screen_offset;
			}
			OBJECT(current_object)->screen_x = OBJECT(object_cursor)->screen_x;
			OBJECT(current_object)->screen_x += dust_x;
			OBJECT(current_object)->screen_y = OBJECT(object_cursor)->screen_y;
			OBJECT(current_object)->screen_y += dust_y;
			dust_y_2 = (uint16_t)OBJECT(current_object)->screen_y;
			if ((int32_t)parts_left <= 2)
				dust_y_2 = (uint16_t)(dust_y_2 + screen_offset);
			dust_y_2 = (uint16_t)(dust_y_2 - 0x14u);
			queue_sprite_at_depth(&ignored, &dust_y_2);
			dust_x = 0x2Cu;
			dust_y = 6u;
			dust_sprite_base = 8u;
			parts_left--;
		} while (parts_left != 0u);
	}
}
