/**
 *    Name:        engine/spawning.c
 *    Description: spawn_objects_by_type (the stage lists of object types) and making objects: with pieces, init_object
 *    Version:     0.1.0
 *    Created:     12-10-2026
 *    Modified:    15-10-2026
 *    Author:      Sebastian Toyos
 *
 *    SegaSonic The Hedgehog (arcade, Sega System 32): the game's V60 program decompiled into C by
 *    tools/decompile.py, with the names of symbols.txt.
 */
#include "game.h"

void reset_linked_objects_flag(void)
{
	electric_fence_state = 0u;
}

void start_spawns_by_type(void)
{
	uint32_t stage_number = 0;

	stage_number = stage;
	type_spawn_stage = stage_number;
	type_spawn_cursor[0] = stage_spawns_by_type[stage_number];
	unused_type_spawn_byte = 0u;
	string_count = 0;
	stage_trigger_bits = 0u;
}

void spawn_objects_by_type(uint32_t *p_player_object, uint32_t *p_player_index, uint32_t *p_first, uint32_t *p_projected_x, uint32_t *p_projected_y, uint32_t *p_object_type, uint32_t *p_object)
{
	uint32_t player_object = *p_player_object;
	uint32_t player_index = *p_player_index;
	uint32_t first = *p_first;
	uint32_t projected_x = *p_projected_x;
	uint32_t projected_y = *p_projected_y;
	uint32_t object_type = *p_object_type;
	uint32_t object = *p_object;
	uint16_t spawn_z = 0;
	uint32_t r0 = 0, r3 = 0, ignored = 0, record = 0, trigger_scroll = 0;

	if (type_spawn_stage != stage)
	{
		start_spawns_by_type();
		*p_player_object = player_object;
		*p_player_index = player_index;
		*p_first = first;
		*p_projected_x = projected_x;
		*p_projected_y = projected_y;
		*p_object_type = object_type;
		*p_object = object;
		return;
	}
	record = type_spawn_cursor[0];
	r0 = scroll_position;
	trigger_scroll = ROM16(record);
	record += 2u;
	if (trigger_scroll == 0xFFFFu || r0 <= trigger_scroll)
	{
		count_active_players(&player_object, &player_index);
		*p_player_object = player_object;
		*p_player_index = player_index;
		return;
	}
	projected_x = ROM16(record);
	record += 2u;
	projected_y = (int16_t)ROM16(record);
	record += 2u;
	spawn_z = (uint16_t)(int16_t)ROM16(record);
	record += 2u;
	if ((int32_t)trigger_scroll <= (int32_t)r0)
	{
		object_type = 0x100000u + ROM16(record) * 8u;
		spawn_count = ROM32(object_type);
		record += 2u;
		object_type += 4u;
		spawn_object_with_pieces(0x08DCB1u, &r0, &trigger_scroll, &player_index, &first, &object);
		if ((uint8_t)r0 == 0u)
		{
			type_spawn_cursor[0] += 0xEu;
			count_active_players(&player_object, &player_index);
			*p_player_object = player_object;
			*p_player_index = player_index;
			*p_first = first;
			*p_projected_x = projected_x;
			*p_projected_y = projected_y;
			*p_object_type = object_type;
			*p_object = object;
			return;
		}
		current_object = first;
		projected_y = projected_y << 16;
		OBJECT(current_object)->projected_x = projected_x;
		OBJECT(current_object)->projected_y = projected_y;
		OBJECT(current_object)->z = (uint32_t)spawn_z << 16;
		OBJECT(current_object)->flags = 0u;
		OBJECT(current_object)->handler = ROM32(object_type);
		OBJECT(current_object)->spawn_arg_1 = ROM16(record);
		record += 2u;
		OBJECT(current_object)->spawn_arg_2 = ROM16(record);
		type_spawn_cursor[0] = record + 2u;
		unproject_object(&ignored, &ignored, &r3, &object);
		count_active_players(&player_object, &player_index);
		*p_player_object = player_object;
		*p_player_index = player_index;
		*p_first = r3;
		*p_projected_x = projected_x;
		*p_projected_y = projected_y;
		*p_object_type = object_type;
		*p_object = object;
		return;
	}
	type_spawn_cursor[0] += 0xEu;
	count_active_players(&player_object, &player_index);
	*p_player_object = player_object;
	*p_player_index = player_index;
	*p_first = first;
	*p_projected_x = projected_x;
	*p_projected_y = projected_y;
	*p_object_type = object_type;
	*p_object = object;
}

void count_active_players(uint32_t *p_player_object, uint32_t *p_player_index)
{
	uint32_t object = 0, player = 0, player_index = 0, players_left = 0;

	players_left = 3u;
	player_index = 2u;
	active_player_count = 0u;
	do
	{
		player = GAME_VARIABLES + player_block_offsets[player_index];
		object = LINKED(player, PLAYER(player)->object);
		if ((OBJECT(object)->flags & 0x400u) == 0u)
			active_player_count++;
		players_left--;
		player_index--;
	} while (players_left != 0u);
	*p_player_object = object;
	*p_player_index = player_index;
}

void spawn_object_with_pieces(uint32_t return_address, uint32_t *p_spawned, uint32_t *p_piece_count, uint32_t *p_free_count, uint32_t *p_first, uint32_t *p_block)
{
	uint32_t spawned = *p_spawned;
	uint16_t piece_count = (uint16_t)*p_piece_count;
	uint32_t free_count = *p_free_count;
	uint32_t first = *p_first;
	uint32_t block = *p_block;
	uint32_t r2 = 0, ignored = 0, pieces_to_add = 0, pool_2_free = 0;
	uint16_t free_blocks = 0, spawn_total = 0;

	if ((int16_t)spawn_count <= 0)
	{
		current_object = object_cursor;
		return;
	}
	free_objects = free_object_stack_ADDRESS;
	first_object = first_object_address;
	last_object = last_object_address;
	spawned &= ~0xFFu;
	free_count = LINKED(free_count, free_object_stack);
	if (free_count == 0u)
	{
		current_object = object_cursor;
		*p_spawned = spawned;
		*p_free_count = free_count;
		return;
	}
	piece_count = (uint16_t)(spawn_count - 1u);
	if (piece_count != 0u)
	{
		pool_2_free = LINKED(WORK_RAM, free_object_stack_2[0]);
		free_blocks = pool_2_free;
		free_count = (pool_2_free & ~0xFFFFu) | (uint16_t)(free_blocks - piece_count);
		if (free_blocks < piece_count)
		{
			current_object = object_cursor;
			*p_spawned = spawned;
			*p_piece_count = (*p_piece_count & 0xFFFF0000u) | piece_count;
			*p_free_count = free_count;
			return;
		}
	}
	allocate_object(&ignored, &block);
	init_object(block);
	OBJECT(block)->next_handler = return_address;
	OBJECT(block)->parent = object_cursor;
	OBJECT(block)->child = 0xFFFFu;
	first = block;
	spawned = 1u;
	spawn_total = spawn_count;
	pieces_to_add = (uint16_t)(spawn_total - 1u);
	if (spawn_total == 1u)
	{
		current_object = object_cursor;
		*p_spawned = spawned;
		*p_piece_count = (*p_piece_count & 0xFFFF0000u) | (pieces_to_add & 0xFFFFu);
		*p_free_count = free_count;
		*p_first = first;
		*p_block = block;
		return;
	}
	add_object_pieces(&spawned, &pieces_to_add, &r2, &first, &block);
	*p_spawned = spawned;
	*p_piece_count = (*p_piece_count & 0xFFFF0000u) | (pieces_to_add & 0xFFFFu);
	*p_free_count = r2;
	*p_first = first;
	*p_block = block;
}

void add_pieces_to_current_object(uint32_t *p_spawned, uint32_t *p_free_count, uint32_t *p_first, uint32_t *p_block)
{
	uint32_t spawned = *p_spawned;
	uint32_t first = *p_first;
	uint32_t r2 = 0, block = 0, piece_count = 0;

	block = object_cursor;
	piece_count = spawn_count;
	add_object_pieces(&spawned, &piece_count, &r2, &first, &block);
	*p_spawned = spawned;
	*p_free_count = r2;
	*p_first = first;
	*p_block = block;
}

void add_object_pieces(uint32_t *p_spawned, uint32_t *p_piece_count, uint32_t *p_free_count, uint32_t *p_first, uint32_t *p_block)
{
	uint32_t spawned = *p_spawned;
	uint16_t piece_count = (uint16_t)*p_piece_count;
	uint32_t first = *p_first;
	uint32_t block = *p_block;
	uint32_t r2 = 0, free_count = 0, ignored = 0, object = 0, pool_2_free = 0;
	uint16_t free_blocks = 0;

	spawn_first = block;
	spawn_last = block;
	spawned &= ~0xFFu;
	piece_count = (uint16_t)piece_count;
	pool_2_free = LINKED(WORK_RAM, free_object_stack_2[0]);
	free_blocks = pool_2_free;
	free_count = (pool_2_free & ~0xFFFFu) | (uint16_t)(free_blocks - piece_count);
	if (free_blocks < piece_count)
	{
		current_object = object_cursor;
		*p_spawned = spawned;
		*p_piece_count = (*p_piece_count & 0xFFFF0000u) | piece_count;
		*p_free_count = free_count;
		return;
	}
	spawn_count = piece_count;
	do
	{
		allocate_object_2(&ignored, &r2, &block);
		init_object_2(block);
		object = spawn_last;
		AS16(spawn_last) = block;
		OBJECT(object)->child = block;
		OBJECT(block)->parent = AS16(spawn_first);
		OBJECT(block)->child = 0xFFFFu;
		spawn_count--;
	} while (spawn_count != 0u);
	first = spawn_first;
	spawned = 1u;
	current_object = object_cursor;
	*p_spawned = spawned;
	*p_piece_count = (*p_piece_count & 0xFFFF0000u) | piece_count;
	*p_free_count = r2;
	*p_first = first;
	*p_block = block;
}

void spawn_objects(uint32_t return_address, uint32_t *p_spawned, uint32_t *p_first, uint32_t *p_block)
{
	uint32_t spawned = *p_spawned;
	uint32_t block = *p_block;
	uint32_t first = 0, ignored = 0;

	if ((int16_t)spawn_count <= 0)
	{
		first = spawn_first;
		current_object = object_cursor;
		*p_first = first;
		return;
	}
	free_objects = free_object_stack_ADDRESS;
	first_object = first_object_address;
	last_object = last_object_address;
	spawned = 0u;
	if ((uint16_t)free_object_stack < spawn_count)
	{
		first = spawn_first;
		current_object = object_cursor;
		*p_spawned = spawned;
		*p_first = first;
		return;
	}
	allocate_object(&ignored, &block);
	spawn_first = block;
	spawn_last = block;
	OBJECT(block)->parent = object_cursor;
	OBJECT(block)->child = 0xFFFFu;
	init_object(block);
	OBJECT(block)->next_handler = return_address;
	for (;;)
	{
		spawn_last = block;
		spawn_count--;
		spawned = 1u;
		if (spawn_count == 0u)
			break;
		allocate_object(&ignored, &block);
		init_object(block);
		OBJECT(block)->next_handler = return_address;
		mem_write16(spawn_last + 0x28u, block);
		OBJECT(block)->parent = object_cursor;
		OBJECT(block)->child = 0xFFFFu;
	}
	first = spawn_first;
	current_object = object_cursor;
	*p_spawned = spawned;
	*p_first = first;
	*p_block = block;
}

void unlink_child_chain(uint32_t object)
{
	uint16_t next_child = 0;

	for (;;)
	{
		next_child = mem_read16(object + 0x28u);
		if (mem_read16(object + 0x28u) == 0xFFFFu)
			break;
		mem_write16(object + 0x28u, 0xFFFFu);
		object = (object & ~0xFFFFu) | next_child;
	}
}

void init_object(uint32_t object)
{
	uint8_t mode = 0;

	mode = 0u;
	init_object_fields(object, mode);
}

void init_object_2(uint32_t object)
{
	uint8_t mode = 0;

	mode = 1u;
	init_object_fields(object, mode);
}

void init_object_fields(uint32_t object, uint8_t mode)
{
	OBJECT(object)->draw_next = 0u;
	OBJECT(object)->sprite_flags = 4u;
	OBJECT(object)->sprite_frame = 0u;
	OBJECT(object)->sprite_palette = 0u;
	OBJECT(object)->sprite_scale = 0x100u;
	OBJECT(object)->sprite_layer = 0u;
	OBJECT(object)->screen_y = 0;
	OBJECT(object)->screen_x = 0;
	OBJECT(object)->hitbox_size_x = 0u;
	OBJECT(object)->hitbox_size_y = 0u;
	OBJECT(object)->anchor_x = 0;
	OBJECT(object)->anchor_y = 0;
	OBJECT(object)->sprite_height = 0;
	OBJECT(object)->sprite_width = 0;
	OBJECT(object)->handler = delete_object_and_children_ADDRESS;
	OBJECT(object)->direction = 0u;
	OBJECT(object)->projected_x = 0;
	OBJECT(object)->projected_y = 0;
	OBJECT(object)->projected_y_int = 0;
	if ((uint8_t)mode != 0u)
		return;
	OBJECT(object)->flags = 0u;
	init_object_fields_tail(object, mode);
}

void clear_object_speeds(void)
{
	uint8_t mode = 0;
	uint32_t object = 0;

	mode = 2u;
	object = current_object;
	init_object_fields_tail(object, mode);
}

void init_object_fields_tail(uint32_t object, uint8_t mode)
{
	OBJECT(object)->speed_x = 0;
	OBJECT(object)->speed_y = 0;
	OBJECT(object)->speed_z = 0;
	if ((uint8_t)mode != 2u)
	{
		OBJECT(object)->z = 0;
		OBJECT(object)->animation_speed = 0u;
		OBJECT(object)->animation_timer = 0;
		OBJECT(object)->animation_frame = 0u;
		OBJECT(object)->kind = 0u;
		OBJECT(object)->animation = 0u;
		OBJECT(object)->x = 0;
		OBJECT(object)->y = 0;
		OBJECT(object)->floor_z = 0;
		OBJECT(object)->previous_floor_z = 0;
		OBJECT(object)->floor_block = 0;
		OBJECT(object)->previous_floor_block = 0;
		OBJECT(object)->sprite_offset_x = 0;
		OBJECT(object)->sprite_offset_y = 0;
		OBJECT(object)->hitbox_height = 0u;
		OBJECT(object)->hit_list_next = 0u;
		OBJECT(object)->touched_by = 0u;
		OBJECT(object)->touch_distance = 0;
		OBJECT(object)->speed = 0;
		OBJECT(object)->platform_list_next = 0u;
		OBJECT(object)->platform = 0u;
		OBJECT(object)->floor_slope = 0u;
		OBJECT(object)->floor_type = 0u;
		OBJECT(object)->wall = 0u;
		OBJECT(object)->floor_attribute = 0u;
		OBJECT(object)->hitbox_offset_x = 0;
		OBJECT(object)->hitbox_offset_y = 0;
		OBJECT(object)->health = 0;
		OBJECT(object)->var_7F = 0u;
		OBJECT(object)->var_80 = 0u;
		OBJECT(object)->var_81 = 0u;
		OBJECT(object)->var_82 = 0u;
		OBJECT(object)->var_84 = 0u;
		OBJECT(object)->var_86 = 0u;
		OBJECT(object)->var_88 = 0u;
		OBJECT(object)->var_8A = 0u;
		OBJECT(object)->var_8C = 0u;
		OBJECT(object)->var_8E = 0u;
		OBJECT(object)->var_8F = 0u;
		OBJECT(object)->var_90 = 0u;
		OBJECT(object)->spawn_arg_1 = 0u;
		OBJECT(object)->spawn_arg_2 = 0u;
		OBJECT(object)->var_96 = 0u;
		AS32(OBJECT(object)->var_98) = 0u;
		OBJECT(object)->next_handler = 0u;
	}
}
