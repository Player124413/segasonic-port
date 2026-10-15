/**
 *    Name:        objects/bouncing_fireball.c
 *    Description: stage 4's end: the fireball emitter and the bouncing fireballs
 *    Version:     0.1.0
 *    Created:     12-10-2026
 *    Modified:    15-10-2026
 *    Author:      Sebastian Toyos
 *
 *    SegaSonic The Hedgehog (arcade, Sega System 32): the game's V60 program decompiled into C by
 *    tools/decompile.py, with the names of symbols.txt.
 */
#include "game.h"

void fireball_emitter_start(void)
{
	uint32_t block_offset = 0, ignored = 0, list_cursor = 0, x = 0, y = 0;

	list_cursor = fireball_emitter_lists[OBJECT(current_object)->sprite_frame] + fireball_emitter_lists_ADDRESS;
	OBJECT(current_object)->x_int = ROM16(list_cursor);
	OBJECT(current_object)->x_fraction = 0u;
	list_cursor += 2u;
	OBJECT(current_object)->y_int = ROM16(list_cursor);
	OBJECT(current_object)->y_fraction = 0u;
	list_cursor += 2u;
	OBJECT(current_object)->floor_z_int = ROM16(list_cursor);
	OBJECT(current_object)->floor_z_fraction = 0u;
	OBJECT(current_object)->animation = list_cursor + 2u;
	OBJECT(current_object)->flags = 0u;
	find_floor(&x, &y, &block_offset, &ignored, &list_cursor);
	OBJECT(current_object)->handler = fireball_emitter_update_ADDRESS;
}

void fireball_emitter_update(void)
{
	uint16_t scale = 0;
	uint32_t floor_slope = 0, ignored = 0, list_record = 0, object = 0, slope_direction = 0, slope_factor = 0, slope_speed = 0, type_record = 0;

	project_object(&slope_direction, &floor_slope);
	if (OBJECT(current_object)->screen_y >= -0x40)
	{
		slope_factor = 0u;
		if ((OBJECT(current_object)->floor_slope | OBJECT(current_object)->floor_type) != 0u)
		{
			floor_slope = OBJECT(current_object)->floor_slope;
			slope_factor = floor_slope_factors[(uint16_t)(floor_slope * 2u)];
		}
		compute_camera_slope_speed(0u, slope_factor);
		slope_direction = camera_slope_direction;
		slope_speed = camera_slope_speed >> 14;
		OBJECT(current_object)->x += (int32_t)(sine_table[slope_direction] * slope_speed) >> 7;
		OBJECT(current_object)->y -= (int32_t)(cosine_table[slope_direction] * slope_speed) >> 7;
	}
	find_floor(&slope_direction, &floor_slope, &slope_speed, &ignored, &object);
	OBJECT(current_object)->z = OBJECT(current_object)->floor_z;
	if (mem_read16(OBJECT(current_object)->animation) <= scroll_position)
	{
		if (free_object_stack <= 0 || free_object_stack_2[0] <= 0)
			return;
		allocate_object(&ignored, &object);
		OBJECT(object)->handler = bouncing_fireball_update_ADDRESS;
		OBJECT(object)->flags = 0u;
		OBJECT(object)->hitbox_size_y = 0u;
		OBJECT(object)->kind = 0x17u;
		OBJECT(object)->z = OBJECT(current_object)->floor_z;
		OBJECT(object)->y = OBJECT(current_object)->y;
		list_record = OBJECT(current_object)->animation + 2u;
		OBJECT(object)->x_int = mem_read16(list_record);
		OBJECT(object)->x_fraction = 0u;
		list_record += 2u;
		OBJECT(object)->y_int += mem_read16(list_record);
		list_record += 2u;
		OBJECT(object)->speed_y = (uint32_t)(int16_t)mem_read16(list_record) << 8;
		OBJECT(object)->speed_x = 0;
		list_record += 2u;
		OBJECT(object)->speed_z = (uint32_t)(int16_t)mem_read16(list_record) << 8;
		type_record = mem_read16(list_record + 2u) + fireball_types_ADDRESS;
		OBJECT(object)->sprite_flags = ROM16(type_record);
		type_record += 2u;
		OBJECT(object)->sprite_palette = ROM16(type_record);
		type_record += 2u;
		OBJECT(object)->sprite_scale = ROM16(type_record);
		type_record += 2u;
		OBJECT(object)->hitbox_height = ROM16(type_record);
		type_record += 2u;
		OBJECT(object)->hitbox_size_x = ROM16(type_record);
		type_record += 2u;
		OBJECT(object)->sprite_offset_x = ROM8(type_record);
		type_record++;
		OBJECT(object)->sprite_offset_y = ROM8(type_record);
		type_record++;
		AS8(OBJECT(object)->hitbox_size_y) = ROM8(type_record);
		type_record++;
		AT8(OBJECT(object)->hitbox_size_y, 1) = ROM8(type_record);
		OBJECT(object)->animation = ROM16(type_record + 1u) + blank_fireball_animation_ADDRESS;
		OBJECT(object)->animation_frame = 0u;
		OBJECT(object)->animation_timer = 0;
		OBJECT(object)->animation_speed = 4u;
		current_object = object;
		scale = 0x180u;
		if (OBJECT(current_object)->hitbox_size_x == 0u)
			scale = 0x30u;
		create_child_shadow(scale, object);
		current_object = object_cursor;
		OBJECT(current_object)->animation += 0xCu;
		if ((int16_t)mem_read16(OBJECT(current_object)->animation + 0xAu) < 0)
		{
			delete_current_object();
			return;
		}
	}
	add_to_rider_list();
}

void bouncing_fireball_update(void)
{
	uint8_t sound_id = 0;
	uint16_t child = 0, shadow_dx = 0, shadow_dy = 0;
	uint32_t r1 = 0, r2 = 0, r7 = 0, cursor = 0, depth = 0, ignored = 0, off_screen = 0, x = 0;
	uint8_t blink_phase = 0;

	OBJECT(current_object)->x += OBJECT(current_object)->speed_x;
	OBJECT(current_object)->y += OBJECT(current_object)->speed_y;
	find_floor(&x, &r1, &r2, &ignored, &cursor);
	OBJECT(current_object)->speed_z += 0x6000u;
	OBJECT(current_object)->z += OBJECT(current_object)->speed_z;
	if (OBJECT(current_object)->floor_z <= OBJECT(current_object)->z)
	{
		OBJECT(current_object)->speed_z = 0u - OBJECT(current_object)->speed_z;
		OBJECT(current_object)->speed_z = (int32_t)OBJECT(current_object)->speed_z >> 1;
		OBJECT(current_object)->z = OBJECT(current_object)->floor_z;
		if (OBJECT(current_object)->speed_z_int <= -2)
		{
			sound_id = 0x21u;
			play_sound(sound_id, &r1, &r2);
		}
	}
	project_object(&ignored, &r1);
	off_screen = check_left_screen();
	if (off_screen == 0u)
	{
		animate(&r1, &r7, &off_screen);
		if (OBJECT(current_object)->hitbox_size_x != 0u)
			add_to_touchable_list(ignored);
		OBJECT(current_object)->flags &= ~0x10u;
		if ((OBJECT(current_object)->flags & 8u) != 0u)
			OBJECT(current_object)->flags |= 0x10u;
		add_to_rider_list();
		queue_sprite(r1);
		shadow_dx = 8u;
		shadow_dy = 0xFFFDu;
		if (OBJECT(current_object)->hitbox_size_x == 0u)
		{
			shadow_dx = 0u;
			shadow_dy = 0xFFFDu;
		}
		if (OBJECT(current_object)->floor_block < 0 || (int16_t)OBJECT(current_object)->child < 0)
			return;
		current_object = LINKED(current_object, OBJECT(object_cursor)->child);
		OBJECT(current_object)->screen_x = OBJECT(object_cursor)->projected_x - view_x + shadow_dx;
		OBJECT(current_object)->screen_y = ((int32_t)((uint32_t)OBJECT(object_cursor)->projected_y + OBJECT(object_cursor)->floor_z) >> 16) - view_y + shadow_dy;
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
			blink_phase = ~AS8(OBJECT(current_object)->hitbox_size_x);
			AS8(OBJECT(current_object)->hitbox_size_x) = blink_phase;
			if (blink_phase == 0u)
			{
				depth = (uint16_t)(OBJECT(current_object)->screen_y - 0x10u);
				queue_sprite_at_depth(&ignored, &depth);
				current_object = (current_object & ~0xFFFFu) | (uint16_t)object_cursor;
				return;
			}
		}
		return;
	}
	child = OBJECT(current_object)->child;
	if ((int16_t)child >= 0)
		free_object_2(child);
	delete_current_object();
}

void bouncing_fireball_update_spare_return(void)
{
}

void bouncing_fireball_knocked_away(void)
{
	uint16_t turn = 0;
	uint32_t toucher = 0;

	OBJECT(current_object)->speed_z = -0x80000;
	toucher = LINKED(current_object, OBJECT(current_object)->touched_by);
	OBJECT(current_object)->direction = OBJECT(toucher)->direction;
	OBJECT(current_object)->speed = OBJECT(toucher)->speed;
	turn = 0x80u;
	if ((OBJECT(current_object)->speed_int & 1u) != 0u)
		turn = (uint16_t)(0u - turn);
	OBJECT(current_object)->direction += turn;
	OBJECT(current_object)->direction &= 0x3FFu;
	set_speed_from_direction();
	OBJECT(current_object)->handler = knocked_object_fly_ADDRESS;
	knocked_object_fly();
}

void knocked_object_fly(void)
{
	uint16_t child = 0;
	uint32_t r1 = 0, r7 = 0, ignored = 0, off_screen = 0;

	OBJECT(current_object)->speed_z += 0x6000u;
	OBJECT(current_object)->z += OBJECT(current_object)->speed_z;
	OBJECT(current_object)->x += OBJECT(current_object)->speed_x;
	OBJECT(current_object)->y += OBJECT(current_object)->speed_y;
	project_object(&ignored, &r1);
	off_screen = is_off_screen();
	if (off_screen == 0u)
	{
		animate(&r1, &r7, &off_screen);
		r1 = queue_sprite(r1);
		return;
	}
	child = OBJECT(current_object)->child;
	if ((int16_t)child >= 0)
		free_object_2(child);
	delete_current_object();
}
