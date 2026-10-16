/**
 *    Name:        engine/objects.c
 *    Description: the object system: the object list and its two pools, allocating, linking and deleting objects, spawning them from the stage lists (by type, by handler, in groups led by a leader), and what the stage objects share
 *    Version:     0.1.0
 *    Created:     12-10-2026
 *    Modified:    15-10-2026
 *    Author:      Sebastian Toyos
 *
 *    SegaSonic The Hedgehog (arcade, Sega System 32): the game's V60 program decompiled into C by
 *    tools/decompile.py, with the names of symbols.txt.
 */
#include "game.h"

void move_object_to_next_stage_slot(void)
{
	if (next_stage_slot_request != 0u)
	{
		OBJECT(current_object)->x = (uint32_t)OBJECT(current_object)->x - stage_link_from_x + stage_link_to_x;
		OBJECT(current_object)->y = (uint32_t)OBJECT(current_object)->y - stage_link_from_y + stage_link_to_y;
		OBJECT(current_object)->previous_floor_z_int = OBJECT(current_object)->previous_floor_z_int - stage_link_from_height + stage_link_to_height;
		OBJECT(current_object)->z_int = OBJECT(current_object)->z_int - stage_link_from_height + stage_link_to_height;
		OBJECT(current_object)->previous_floor_block = (uint16_t)(OBJECT(current_object)->previous_floor_block - stage_link_from_block + stage_link_to_block);
	}
}

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

void spawn_object_groups(uint32_t *p_r1, uint32_t *p_free_stack, uint32_t *p_slope_z, uint32_t *p_count, uint32_t *p_object)
{
	uint32_t r1 = *p_r1;
	uint32_t free_stack = *p_free_stack;
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
			*p_free_stack = free_stack;
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
				*p_free_stack = free_stack;
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
				*p_free_stack = free_stack;
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
				*p_free_stack = free_stack;
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
				*p_free_stack = free_stack;
				*p_count = count;
				*p_object = object;
				return;
			}
			group_spawn_cursor += 0xEu;
			continue;
		case 0xCu:
			goto spawn_object_groups_mark_silent;
		case 0xEu:
			spawn_waterfall(record, &spawned, &free_stack, &object);
			if (spawned == 0u)
			{
				*p_r1 = r1;
				*p_free_stack = free_stack;
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
			*p_free_stack = free_stack;
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
				*p_free_stack = free_stack;
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
	*p_free_stack = free_stack;
	*p_slope_z = slope_z;
	*p_count = count;
	*p_object = object;
	return;
spawn_object_groups_mark_silent:
	AS8(group_previous_member) = 0xFFu;
	goto spawn_object_groups_no_depth_step;
}

void crumbling_floor_update(void)
{
	uint8_t sound_id = 0;
	uint32_t r1 = 0, at_last_step = 0, depth = 0, ignored = 0, sprite_depth = 0, write_index_address = 0;

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
									play_sound(sound_id, &r1, &write_index_address);
								}
								break;
							case 1:
								if (OBJECT(current_object)->screen_x <= 0x180)
								{
									sound_id = 0xDu;
									play_sound(sound_id, &r1, &write_index_address);
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
									play_sound(sound_id, &r1, &write_index_address);
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
									play_sound(sound_id, &r1, &write_index_address);
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
									play_sound(sound_id, &r1, &write_index_address);
								}
								break;
							case 1:
								if (OBJECT(current_object)->screen_x <= 0x180)
								{
									sound_id = 0xDu;
									play_sound(sound_id, &r1, &write_index_address);
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
									play_sound(sound_id, &r1, &write_index_address);
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
									play_sound(sound_id, &r1, &write_index_address);
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
									play_sound(sound_id, &r1, &write_index_address);
								}
								break;
							case 1:
								if (OBJECT(current_object)->screen_x <= 0x180)
								{
									sound_id = 0xDu;
									play_sound(sound_id, &r1, &write_index_address);
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
									play_sound(sound_id, &r1, &write_index_address);
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
									play_sound(sound_id, &r1, &write_index_address);
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
									play_sound(sound_id, &r1, &write_index_address);
								}
								break;
							case 1:
								if (OBJECT(current_object)->screen_x <= 0x180)
								{
									sound_id = 0xDu;
									play_sound(sound_id, &r1, &write_index_address);
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
									play_sound(sound_id, &r1, &write_index_address);
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
									play_sound(sound_id, &r1, &write_index_address);
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
									play_sound(sound_id, &r1, &write_index_address);
								}
								break;
							case 1:
								if (OBJECT(current_object)->screen_x <= 0x180)
								{
									sound_id = 0xDu;
									play_sound(sound_id, &r1, &write_index_address);
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
									play_sound(sound_id, &r1, &write_index_address);
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
									play_sound(sound_id, &r1, &write_index_address);
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

void spawn_object_groups_2(uint32_t *p_list_offset, uint32_t *p_count, uint32_t *p_object)
{
	uint32_t list_offset = *p_list_offset;
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
		list_offset = stage_record_spawns[stage];
		group_spawn_cursor_2 = stage_record_spawns_ADDRESS + list_offset;
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
			*p_list_offset = list_offset;
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
				*p_list_offset = list_offset;
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
		*p_list_offset = list_offset;
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
			*p_list_offset = list_offset;
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
			list_offset = mem_read32(member_record);
			member_record += 4u;
			if ((group_member_number & 1u) != 0u)
				OBJECT(object)->animation = list_offset;
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
	*p_list_offset = list_offset;
	*p_count = count;
	*p_object = object;
}

void reset_objects(uint32_t *p_block, uint32_t *p_stack_slot)
{
	uint32_t r0 = 0, r3 = 0;

	reset_object_list();
	reset_object_pool_2(&r0, &r3);
	run_objects();
	*p_block = r0;
	*p_stack_slot = r3;
}

void reset_objects_alt(void)
{
	reset_object_list();
	reset_object_pool_2_alt();
	run_objects();
}

void run_objects(void)
{
	free_objects = free_object_stack_ADDRESS;
	if (object_count == 0u)
		return;
	first_object = first_object_address;
	last_object = last_object_address;
	object_cursor = last_object;
	do
	{
		object_cursor = LINKED(object_cursor, OBJECT(object_cursor)->next);
		current_object = object_cursor;
		call_code(OBJECT(current_object)->handler);
	} while ((uint16_t)object_cursor != (uint16_t)last_object);
	AS16(first_object_address) = first_object;
	AS16(last_object_address) = last_object;
}

void reset_object_list(void)
{
	uint16_t object_offset = 0;
	uint32_t objects_left = 0, stack_slot = 0;

	object_offset = 0u;
	first_object_address = 0x200000u;
	last_object_address = 0x200000u;
	objects_left = 0x80u;
	free_object_stack = 0x80;
	stack_slot = 0x20E902u;
	do
	{
		mem_write16(stack_slot, object_offset);
		objects_left--;
		stack_slot += 2u;
		object_offset += 0xA0u;
	} while (objects_left != 0u);
	object_count = objects_left;
}

void take_free_object(uint32_t *p_free_count, uint32_t *p_object)
{
	uint32_t object = *p_object;
	uint32_t free_count = 0;

	free_count = mem_read16(free_objects);
	if (free_count == 0u)
	{
		*p_free_count = free_count;
		return;
	}
	object = 0x200000u | mem_read16(free_objects + free_count * 2u);
	mem_write16(free_objects, mem_read16(free_objects) - 1u);
	*p_free_count = free_count;
	*p_object = object;
}

uint32_t allocate_object_or_start_list(uint32_t object)
{
	uint32_t ignored = 0, new_object = 0;

	if (object_count != 0u)
	{
		allocate_object(&ignored, &object);
		return object;
	}
	new_object = 0x200000u | mem_read16(free_objects + mem_read16(free_objects) * 2u);
	mem_write16(free_objects, mem_read16(free_objects) - 1u);
	AS16(first_object_address) = new_object;
	first_object = new_object;
	last_object = new_object;
	link_object(new_object);
	return new_object;
}

void allocate_object(uint32_t *p_free_count, uint32_t *p_object)
{
	uint32_t object = *p_object;
	uint32_t free_count = 0;

	free_count = mem_read16(free_objects);
	if (free_count == 0u)
	{
		*p_free_count = free_count;
		return;
	}
	object = 0x200000u | mem_read16(free_objects + free_count * 2u);
	mem_write16(free_objects, mem_read16(free_objects) - 1u);
	link_object(object);
	*p_free_count = free_count;
	*p_object = object;
}

void link_object(uint32_t object)
{
	mem_write16(object + 0x20u, last_object);
	OBJECT(last_object)->next = object;
	OBJECT(first_object)->previous = object;
	mem_write16(object + 0x22u, first_object);
	AS16(last_object_address) = object;
	last_object = object;
	object_count++;
}

void allocate_object_after_current(void)
{
	uint32_t free_count = 0, new_object = 0, object = 0;

	free_count = mem_read16(free_objects);
	if (free_count == 0u)
		return;
	new_object = 0x200000u | mem_read16(free_objects + free_count * 2u);
	mem_write16(free_objects, mem_read16(free_objects) - 1u);
	object = LINKED(current_object, OBJECT(current_object)->next);
	OBJECT(current_object)->next = new_object;
	OBJECT(object)->previous = new_object;
	mem_write16(new_object + 0x20u, current_object);
	mem_write16(new_object + 0x22u, object);
	object_count++;
	if ((uint16_t)last_object == (uint16_t)current_object)
		last_object = (last_object & ~0xFFFFu) | (uint16_t)new_object;
}

void delete_current_object(void)
{
	uint32_t object = 0, object_2 = 0;

	object_count--;
	object = LINKED(object_cursor, OBJECT(object_cursor)->previous);
	object_2 = LINKED(object_cursor, OBJECT(object_cursor)->next);
	OBJECT(object)->next = object_2;
	OBJECT(object_2)->previous = object;
	if ((uint16_t)object_cursor == (uint16_t)first_object)
		first_object = (first_object & ~0xFFFFu) | (uint16_t)object_2;
	if ((uint16_t)object_cursor == (uint16_t)last_object)
		last_object = (last_object & ~0xFFFFu) | (uint16_t)object;
	current_object = object_cursor;
	object_cursor = (object_cursor & ~0xFFFFu) | (uint16_t)object;
	push_free_object();
}

void push_free_object(void)
{
	mem_write16(free_objects, mem_read16(free_objects) + 1u);
	mem_write16(free_objects + mem_read16(free_objects) * 2u, current_object);
}

void delete_next_object(void)
{
	uint32_t ignored = 0, object = 0;

	object = LINKED(object_cursor, OBJECT(current_object)->next);
	delete_object(object, &ignored, &ignored);
}

void delete_object(uint32_t object, uint32_t *p_free_count, uint32_t *p_previous)
{
	uint32_t free_count = 0, object_2 = 0, object_3 = 0;

	object_count--;
	object_2 = LINKED(object, OBJECT(object)->previous);
	object_3 = LINKED(object, OBJECT(object)->next);
	OBJECT(object_3)->previous = object_2;
	OBJECT(object_2)->next = object_3;
	if ((uint16_t)object == (uint16_t)first_object)
		first_object = (first_object & ~0xFFFFu) | (uint16_t)object_3;
	if ((uint16_t)object != (uint16_t)last_object)
	{
		mem_write16(free_objects, mem_read16(free_objects) + 1u);
		free_count = mem_read16(free_objects);
		mem_write16(free_objects + free_count * 2u, object);
		*p_free_count = free_count;
		*p_previous = object_2;
		return;
	}
	last_object = (last_object & ~0xFFFFu) | (uint16_t)object_2;
	mem_write16(free_objects, mem_read16(free_objects) + 1u);
	free_count = mem_read16(free_objects);
	mem_write16(free_objects + free_count * 2u, object);
	*p_free_count = free_count;
	*p_previous = object_2;
}

void reset_object_pool_2(uint32_t *p_block, uint32_t *p_stack_slot)
{
	uint32_t block = 0, pool_blocks_left = 0, stack_slot = 0;

	block = 0x205000u;
	pool_blocks_left = 0xFFu;
	free_object_stack_2[0] = 0xFF;
	stack_slot = 0x20EA12u;
	do
	{
		mem_write16(stack_slot, block);
		pool_blocks_left--;
		stack_slot += 2u;
		block += 0x30u;
	} while (pool_blocks_left != 0u);
	*p_block = block;
	*p_stack_slot = stack_slot;
}

void reset_object_pool_2_alt(void)
{
	uint16_t block = 0;
	uint32_t pool_blocks_left = 0, stack_slot = 0;

	block = 0x5000u;
	pool_blocks_left = 0x1D8u;
	free_object_stack_2[0] = 0x1D8;
	stack_slot = 0x20EA12u;
	do
	{
		mem_write16(stack_slot, block);
		pool_blocks_left--;
		stack_slot += 2u;
		block += 0x34u;
	} while (pool_blocks_left != 0u);
}

void allocate_object_2(uint32_t *p_free_count, uint32_t *p_free_stack, uint32_t *p_object)
{
	uint32_t object = *p_object;
	uint32_t free_count = 0, free_stack = 0;

	free_stack = free_object_stack_2_ADDRESS;
	free_count = (uint16_t)free_object_stack_2[0];
	if (free_count == 0u)
	{
		*p_free_count = free_count;
		*p_free_stack = free_stack;
		return;
	}
	object = 0x200000u | (uint16_t)free_object_stack_2[free_count];
	free_object_stack_2[0]--;
	*p_free_count = free_count;
	*p_free_stack = free_stack;
	*p_object = object;
}

uint32_t free_object_2(uint16_t object)
{
	uint32_t free_stack = 0;

	free_stack = free_object_stack_2_ADDRESS;
	free_object_stack_2[0]++;
	free_object_stack_2[(uint16_t)free_object_stack_2[0]] = object;
	return free_stack;
}
