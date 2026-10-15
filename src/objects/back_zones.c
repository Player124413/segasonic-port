/**
 *    Name:        objects/back_zones.c
 *    Description: the zones behind the chasers: spawn_chaser_back_zone and the spiked roller's back zone
 *    Version:     0.1.0
 *    Created:     12-10-2026
 *    Modified:    15-10-2026
 *    Author:      Sebastian Toyos
 *
 *    SegaSonic The Hedgehog (arcade, Sega System 32): the game's V60 program decompiled into C by
 *    tools/decompile.py, with the names of symbols.txt.
 */
#include "game.h"

uint32_t spawn_chaser_back_zone(uint32_t zone_index, uint32_t zone)
{
	uint32_t free_count = 0, step_record = 0;

	chaser_zone_index = zone_index;
	allocate_object(&free_count, &zone);
	if (free_count != 0u)
	{
		OBJECT(zone)->kind = 9u;
		OBJECT(zone)->flags = 0x40000002u;
		step_record = object_steps_ADDRESS + chaser_zone_index * 3u * 2u;
		AS16(OBJECT(zone)->var_80) = mem_read16(step_record);
		OBJECT(zone)->handler = mem_read32(step_record + 2u);
		OBJECT(zone)->parent = current_object;
	}
	return zone;
}

void chaser_back_zone_check_parent(void)
{
	OBJECT(current_object)->kind = 0x14u;
	if (AS8(OBJECT(LINKED(current_object, OBJECT(current_object)->parent))->var_96) == 3u)
	{
		delete_current_object();
		return;
	}
	chaser_back_zone_update();
}

void chaser_back_zone_update(void)
{
	uint32_t direction = 0, object = 0;

	if (OBJECT(current_object)->chaser_back_zone.end_position <= scroll_position)
	{
		delete_current_object();
		return;
	}
	object = LINKED(current_object, OBJECT(current_object)->parent);
	OBJECT(current_object)->hitbox_size_x = OBJECT(object)->hitbox_size_x;
	OBJECT(current_object)->hitbox_size_y = OBJECT(object)->hitbox_size_y;
	OBJECT(current_object)->x = OBJECT(object)->x;
	OBJECT(current_object)->y = OBJECT(object)->y;
	OBJECT(current_object)->hitbox_offset_x = OBJECT(object)->hitbox_offset_x;
	OBJECT(current_object)->hitbox_offset_y = OBJECT(object)->hitbox_offset_y;
	OBJECT(current_object)->z = OBJECT(object)->z;
	OBJECT(current_object)->direction = OBJECT(object)->direction;
	OBJECT(current_object)->hitbox_height = OBJECT(object)->hitbox_height;
	direction = OBJECT(object)->direction;
	switch (direction >> 8)
	{
	case 0:
	case 4:
		OBJECT(current_object)->y_int += OBJECT(current_object)->hitbox_offset_y;
		OBJECT(current_object)->y_int += OBJECT(current_object)->hitbox_size_y;
		OBJECT(current_object)->hitbox_size_y = 0x200u;
		OBJECT(current_object)->hitbox_offset_y = 0;
		break;
	case 1:
		OBJECT(current_object)->x_int += OBJECT(current_object)->hitbox_offset_x;
		OBJECT(current_object)->hitbox_size_x = 0x200u;
		OBJECT(current_object)->x_int -= OBJECT(current_object)->hitbox_size_x;
		OBJECT(current_object)->hitbox_offset_x = 0;
		break;
	case 2:
		OBJECT(current_object)->y_int += OBJECT(current_object)->hitbox_offset_y;
		OBJECT(current_object)->hitbox_size_y = 0x200u;
		OBJECT(current_object)->y_int -= OBJECT(current_object)->hitbox_size_y;
		OBJECT(current_object)->hitbox_offset_y = 0;
		break;
	case 3:
		OBJECT(current_object)->x_int += OBJECT(current_object)->hitbox_offset_x;
		OBJECT(current_object)->x_int += OBJECT(current_object)->hitbox_size_x;
		OBJECT(current_object)->hitbox_size_x = 0x200u;
		OBJECT(current_object)->hitbox_offset_x = 0;
		break;
	default:
		unknown_code(0x82F30u + (direction >> 8) * 2u);
	}
	direction = add_to_zone_list(direction);
}

void spawn_roller_back_zone(uint32_t zone_index, uint32_t zone)
{
	uint32_t free_count = 0;

	roller_zone_index = zone_index;
	allocate_object(&free_count, &zone);
	if (free_count != 0u)
	{
		OBJECT(zone)->kind = 0x18u;
		OBJECT(zone)->flags = 0x40000002u;
		OBJECT(zone)->roller_back_zone.limit = object_steps[roller_zone_index * 3u];
		OBJECT(zone)->handler = roller_back_zone_update_ADDRESS;
		OBJECT(zone)->parent = object_cursor;
	}
}

void roller_back_zone_update(void)
{
	uint32_t direction = 0, object = 0;

	object = LINKED(current_object, OBJECT(current_object)->parent);
	OBJECT(current_object)->hitbox_size_x = OBJECT(object)->hitbox_size_x;
	OBJECT(current_object)->hitbox_size_y = OBJECT(object)->hitbox_size_y;
	OBJECT(current_object)->x = OBJECT(object)->x;
	OBJECT(current_object)->y = OBJECT(object)->y;
	OBJECT(current_object)->hitbox_offset_x = OBJECT(object)->hitbox_offset_x;
	OBJECT(current_object)->hitbox_offset_y = OBJECT(object)->hitbox_offset_y;
	OBJECT(current_object)->z = OBJECT(object)->z;
	OBJECT(current_object)->direction = OBJECT(object)->direction;
	OBJECT(current_object)->hitbox_height = OBJECT(object)->hitbox_height;
	direction = OBJECT(object)->direction;
	switch (direction >> 8)
	{
	case 0:
	case 4:
		if (OBJECT(current_object)->y_int <= OBJECT(current_object)->roller_back_zone.limit)
		{
			delete_current_object();
			return;
		}
		OBJECT(current_object)->y_int += OBJECT(current_object)->hitbox_size_y;
		OBJECT(current_object)->hitbox_size_y = 0x200u;
		OBJECT(current_object)->hitbox_size_x = 0x1A2u;
		break;
	case 1:
		if (OBJECT(current_object)->roller_back_zone.limit <= OBJECT(current_object)->x_int)
		{
			delete_current_object();
			return;
		}
		OBJECT(current_object)->x_int += OBJECT(current_object)->hitbox_offset_x;
		OBJECT(current_object)->hitbox_size_x = 0x200u;
		OBJECT(current_object)->x_int -= OBJECT(current_object)->hitbox_size_x;
		OBJECT(current_object)->y_int -= 0xB0u;
		OBJECT(current_object)->hitbox_size_y = 0x1A2u;
		OBJECT(current_object)->hitbox_offset_x = 0;
		break;
	case 2:
		if (OBJECT(current_object)->roller_back_zone.limit <= OBJECT(current_object)->y_int)
		{
			delete_current_object();
			return;
		}
		OBJECT(current_object)->y_int += OBJECT(current_object)->hitbox_offset_y;
		OBJECT(current_object)->hitbox_size_y = 0x200u;
		OBJECT(current_object)->y_int -= OBJECT(current_object)->hitbox_size_y;
		OBJECT(current_object)->x_int -= 0xB0u;
		OBJECT(current_object)->hitbox_size_x = 0x1A2u;
		OBJECT(current_object)->hitbox_offset_y = 0;
		break;
	case 3:
		if (OBJECT(current_object)->x_int <= OBJECT(current_object)->roller_back_zone.limit)
		{
			delete_current_object();
			return;
		}
		OBJECT(current_object)->x_int += OBJECT(current_object)->hitbox_size_x;
		OBJECT(current_object)->hitbox_size_x = 0x200u;
		OBJECT(current_object)->hitbox_size_y = 0x1A2u;
		break;
	default:
		unknown_code(0x8303Bu + (direction >> 8) * 3u);
	}
	direction = add_to_zone_list(direction);
}
