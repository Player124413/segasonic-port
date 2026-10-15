/**
 *    Name:        engine/spawn_by_handler.c
 *    Description: spawn_objects_by_handler: starts the objects of the stage lists of handlers (stage_spawns_by_handler)
 *    Version:     0.1.0
 *    Created:     12-10-2026
 *    Modified:    15-10-2026
 *    Author:      Sebastian Toyos
 *
 *    SegaSonic The Hedgehog (arcade, Sega System 32): the game's V60 program decompiled into C by
 *    tools/decompile.py, with the names of symbols.txt.
 */
#include "game.h"

void spawn_objects_by_handler(uint32_t *p_list_offset, uint32_t *p_record, uint32_t *p_object)
{
	uint32_t list_offset = *p_list_offset;
	uint32_t object = *p_object;
	uint32_t free_count = 0, record = 0;
	uint32_t old_init_flags = 0;

	old_init_flags = stage_init_flags;
	stage_init_flags = old_init_flags | 1u;
	if ((old_init_flags & 1u) == 0u)
	{
		list_offset = stage_spawns_by_handler[stage];
		handler_spawn_cursor = stage_spawns_by_handler_ADDRESS + list_offset;
	}
	record = handler_spawn_cursor;
	if (scroll_position < ROM16(record))
	{
		*p_list_offset = list_offset;
		*p_record = record;
		return;
	}
	if ((uint16_t)(scroll_position - ROM16(record)) < 0x100u)
	{
		allocate_object_outside_loop(&free_count, &object);
		if (free_count == 0u)
		{
			*p_list_offset = list_offset;
			*p_record = record;
			*p_object = object;
			return;
		}
		record = handler_spawn_cursor + 2u;
		OBJECT(object)->x_int = ROM16(record);
		OBJECT(object)->x_fraction = 0u;
		record += 2u;
		OBJECT(object)->y_int = ROM16(record);
		OBJECT(object)->y_fraction = 0u;
		record += 2u;
		OBJECT(object)->sprite_frame = ROM16(record);
		record += 2u;
		OBJECT(object)->handler = ROM32(record);
		record += 4u;
	}
	handler_spawn_cursor += 0xCu;
	*p_list_offset = list_offset;
	*p_record = record;
	*p_object = object;
}
