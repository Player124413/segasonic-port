/**
 *    Name:        stage/stage_frame.c
 *    Description: what the stage frames of run_game_mode call: layer blending, zones, the stage clear check, the player summary, stage slots and events, restart points, the player objects
 *    Version:     0.1.0
 *    Created:     12-10-2026
 *    Modified:    15-10-2026
 *    Author:      Sebastian Toyos
 *
 *    SegaSonic The Hedgehog (arcade, Sega System 32): the game's V60 program decompiled into C by
 *    tools/decompile.py, with the names of symbols.txt.
 */
#include "game.h"

void apply_stage_layer_blending(uint32_t *p_layer_bit, uint32_t *p_count)
{
	uint32_t layer_bit = *p_layer_bit;
	uint16_t blend_mask = 0;
	uint32_t count = 0, cursor = 0, layer_shadow = 0;

	cursor = mixer_text_blend_ADDRESS;
	count = 7u;
	do
	{
		mem_write32(cursor, mem_read32(cursor) & ~0x1000u);
		cursor += 2u;
		count--;
	} while (count != 0u);
	cursor = stage_layer_blending_ADDRESS + stage_layer_blending[stage];
	if (ROM8(cursor) == 0u)
	{
		*p_count = count;
		return;
	}
	count = ROM8(cursor);
	cursor++;
	do
	{
		blend_mask = ROM8(cursor);
		cursor++;
		layer_bit = ROM8(cursor);
		cursor++;
		layer_shadow = GAME_VARIABLES + ROM16(cursor);
		mem_write16(layer_shadow, mem_read16(layer_shadow) & ~0x3Fu);
		mem_write16(layer_shadow, mem_read16(layer_shadow) | blend_mask);
		mem_write32(layer_shadow, mem_read32(layer_shadow) | shl32(1u, (int)(layer_bit)));
		mixer_blend &= ~0xF00u;
		cursor += 2u;
		mixer_blend |= ROM16(cursor);
		mixer_blend |= 0x800u;
		count--;
		cursor += 2u;
	} while (count != 0u);
	*p_layer_bit = layer_bit;
	*p_count = count;
}

uint32_t check_zones(uint32_t box_edge)
{
	uint16_t box_start = 0;
	uint32_t hitter = 0, object = 0;

	hitter = unk_20F016_ADDRESS;
	object = unk_20F016_ADDRESS;
	if ((int16_t)zone_list < 0)
	{
		zone_list = 0xFFFFu;
		return box_edge;
	}
	while ((int16_t)OBJECT(hitter)->hit_list_next >= 0)
	{
		hitter = LINKED(hitter, OBJECT(hitter)->hit_list_next);
		object = LINKED(object, zone_list);
		for (;;)
		{
			box_edge = (box_edge & ~0xFFFFu) | (uint16_t)OBJECT(object)->hitbox_offset_x;
			box_start = (uint16_t)(OBJECT(object)->x_int + box_edge);
			if (OBJECT(hitter)->x_int < (int16_t)box_start || (int16_t)(box_start + OBJECT(object)->hitbox_size_x) < OBJECT(hitter)->x_int)
			{
				if ((int16_t)OBJECT(object)->hit_list_next < 0)
					break;
				object = LINKED(object, OBJECT(object)->hit_list_next);
				continue;
			}
			box_edge = (box_edge & ~0xFFFFu) | (uint16_t)OBJECT(object)->hitbox_offset_y;
			box_start = (uint16_t)(OBJECT(object)->y_int + box_edge);
			if (OBJECT(hitter)->y_int < (int16_t)box_start || (int16_t)(box_start + OBJECT(object)->hitbox_size_y) < OBJECT(hitter)->y_int)
			{
				if ((int16_t)OBJECT(object)->hit_list_next < 0)
					break;
				object = LINKED(object, OBJECT(object)->hit_list_next);
				continue;
			}
			box_edge = (box_edge & ~0xFFFFu) | (uint16_t)OBJECT(object)->z_int;
			box_edge = (box_edge & ~0xFFFFu) | (uint16_t)(box_edge - OBJECT(object)->hitbox_height);
			if (OBJECT(hitter)->z_int < (int16_t)box_edge)
			{
				if ((int16_t)OBJECT(object)->hit_list_next < 0)
					break;
				object = LINKED(object, OBJECT(object)->hit_list_next);
				continue;
			}
			box_edge = (box_edge & ~0xFFFFu) | (uint16_t)OBJECT(hitter)->z_int;
			box_edge = (box_edge & ~0xFFFFu) | (uint16_t)(box_edge - OBJECT(hitter)->hitbox_height);
			if (OBJECT(object)->z_int < (int16_t)box_edge)
			{
				if ((int16_t)OBJECT(object)->hit_list_next < 0)
					break;
				object = LINKED(object, OBJECT(object)->hit_list_next);
				continue;
			}
			OBJECT(object)->flags |= 0x8000000u;
			OBJECT(hitter)->flags |= 0x8000000u;
			OBJECT(hitter)->anchor_x = object;
			OBJECT(object)->anchor_x = hitter;
			break;
		}
	}
	zone_list = 0xFFFFu;
	return box_edge;
}

uint32_t add_to_zone_list(uint32_t old_head)
{
	old_head = LINKED(old_head, zone_list);
	zone_list = current_object;
	OBJECT(current_object)->hit_list_next = old_head;
	OBJECT(current_object)->flags &= ~0x8000000u;
	return old_head;
}

void check_stage_clear(uint32_t *p_count, uint32_t *p_finished_flag, uint32_t *p_status)
{
	uint32_t count = *p_count;
	uint32_t finished_flag = *p_finished_flag;
	uint32_t status = *p_status;
	uint8_t next_game_mode = 0;

	if (scroll_position < goal_scroll_position || player_status_spare != 0u || stage_exit_started == 0u)
		return;
	finished_flag = players_finished_ADDRESS;
	status = player_status_ADDRESS;
	count = (count & ~0xFFu) | 3u;
	do
	{
		if (mem_read8(status) == 0u || mem_read8(status) == 0x82u || mem_read8(status) == 3u || mem_read8(status) == 4u || mem_read8(status) == 2u)
			mem_write8(finished_flag, 0xFFu);
		finished_flag++;
		status++;
		count--;
	} while (count != 0u);
	if (AS32(players_finished[0]) != 0xFFFFFFFFu)
	{
		*p_count = count;
		*p_finished_flag = finished_flag;
		*p_status = status;
		return;
	}
	game_mode_timer = 0;
	next_game_mode = 0x17u;
	if (stage == 0xBu || stage == 0xCu || stage == 0xDu || stage == 0xEu || stage == 0xFu)
		next_game_mode = 0x1Eu;
	game_mode = next_game_mode;
	*p_count = count;
	*p_finished_flag = finished_flag;
	*p_status = status;
}

uint32_t clear_player_blocks(void)
{
	uint32_t count = 0, dest = 0, fill_word = 0;

	dest = PLAYER_BLOCK(1);
	count = 0x30u;
	fill_word = 0u;
	do
	{
		mem_write32(dest, fill_word);
		dest += 4u;
		mem_write32(dest, fill_word);
		dest += 4u;
		mem_write32(dest, fill_word);
		dest += 4u;
		mem_write32(dest, fill_word);
		dest += 4u;
		mem_write32(dest, fill_word);
		dest += 4u;
		mem_write32(dest, fill_word);
		dest += 4u;
		mem_write32(dest, fill_word);
		dest += 4u;
		mem_write32(dest, fill_word);
		count--;
		dest += 4u;
	} while (count != 0u);
	return count;
}

uint32_t update_grey_screen(void)
{
	uint32_t count = 0, player_block = 0;

	grey_screen_ended = 0u;
	player_block = 0x208200u;
	count = 3u;
	do
	{
		player_block += 0x200u;
		count--;
	} while ((count != 0u) & (mem_read8(player_block + 0x9Fu) == 0u));
	if (mem_read8(player_block + 0x9Fu) != 0u)
	{
		grey_screen_active = 0xFFu;
		mixer_offset_select |= 0x1Eu;
		return count;
	}
	if (grey_screen_active != 0u)
	{
		grey_screen_ended = 0xFFu;
		grey_screen_active = 0u;
		mixer_offset_select &= ~0x1Eu;
	}
	return count;
}

void update_player_summary(uint32_t *p_in_play_count, uint32_t *p_count, uint32_t *p_cursor, uint32_t *p_player_object)
{
	uint8_t in_play_count = (uint8_t)*p_in_play_count;
	uint32_t count = *p_count;
	uint32_t cursor = *p_cursor;
	uint32_t player_object = *p_player_object;
	uint32_t zero = 0, negative = 0;
	uint32_t player_block = 0, players_left = 0, status_cursor = 0;
	uint8_t drop_in_status = 0, in_play_status = 0, status = 0;
	uint32_t init_flags = 0;

	init_flags = stage_init_flags;
	stage_init_flags = init_flags | 0x800u;
	if ((init_flags & 0x800u) == 0u)
		unused_summary_byte = 0u;
	player_dropping_in = 0u;
	no_player_in_play = 0u;
	player_mishap = 0u;
	if (skip_player_summary != 0u)
		return;
	status_cursor = player_status_ADDRESS;
	players_left = 3u;
	do
	{
		drop_in_status = mem_read8(status_cursor);
		zero = (uint8_t)(drop_in_status + 0x7Fu) == 0u;
		status_cursor++;
		players_left--;
	} while ((players_left != 0u) & (drop_in_status != 0x81u));
	if (zero == 0u)
	{
		if (player_dropping_in_before != 0u)
			scroll_mode = saved_scroll_mode;
	}
	else
	{
		player_dropping_in = 0xFFu;
		if (player_dropping_in_before == 0u || scroll_mode != 0u)
			saved_scroll_mode = scroll_mode;
		scroll_mode = 0u;
		if (mem_read16(scroll_mode_script - 4u) != 0u)
			AS8(scroll_speed) = 5u;
	}
	player_dropping_in_before = player_dropping_in;
	status_cursor = player_status_ADDRESS;
	players_left = 3u;
	do
	{
		in_play_status = mem_read8(status_cursor);
		negative = in_play_status >> 7;
		status_cursor++;
		players_left--;
	} while ((players_left != 0u) & ((int8_t)in_play_status >= 0));
	if (negative == 0u)
		no_player_in_play = 0xFFu;
	player_block = PLAYER_BLOCK(1);
	player_object = PLAYER_BLOCK(1);
	players_left = 3u;
	for (;;)
	{
		player_object = (player_object & ~0xFFFFu) | mem_read16(player_block + 0x14u);
		if ((mem_read32(player_object + 0x30u) & 0x400u) != 0u || mem_read8(player_block + 0xB4u) == 0u)
		{
			players_left--;
			player_block += 0x200u;
			if (players_left != 0u)
				continue;
			count = 3u;
			in_play_count = 0u;
			cursor = player_status_ADDRESS;
			do
			{
				status = mem_read8(cursor);
				cursor++;
				if ((int8_t)status < 0)
					in_play_count++;
				count--;
			} while (count != 0u);
			players_in_play = in_play_count;
			*p_in_play_count = (*p_in_play_count & 0xFFFFFF00u) | in_play_count;
			*p_count = count;
			*p_cursor = cursor;
			*p_player_object = player_object;
			return;
		}
		break;
	}
	player_mishap = 0xFFu;
	player_1_mishap_flag = 0u;
	player_2_mishap_flag = 0u;
	player_3_mishap_flag = 0u;
	count = 3u;
	in_play_count = 0u;
	cursor = player_status_ADDRESS;
	do
	{
		status = mem_read8(cursor);
		cursor++;
		if ((int8_t)status < 0)
			in_play_count++;
		count--;
	} while (count != 0u);
	players_in_play = in_play_count;
	*p_in_play_count = (*p_in_play_count & 0xFFFFFF00u) | in_play_count;
	*p_count = count;
	*p_cursor = cursor;
	*p_player_object = player_object;
}

void continue_to_next_stage_slot(uint32_t *p_table_offset, uint32_t *p_link_record, uint32_t *p_block)
{
	uint32_t table_offset = *p_table_offset;
	uint32_t link_record = *p_link_record;
	uint32_t block = *p_block;
	uint32_t r10 = 0, current_stage = 0, from_block = 0, from_record = 0, link_point = 0, to_block = 0;

	if (next_stage_slot_request == 0u)
		return;
	link_point = stage_link_points_ADDRESS + stage_link_points[stage];
	from_block = ROM16(link_point);
	from_record = floor_blocks + 2u + from_block;
	stage_link_from_block = from_block;
	stage_link_from_x = ROM32(from_record);
	stage_link_from_y = ROM32(from_record + 8u);
	stage_link_from_height = ROM16(from_record + 0x10u);
	current_stage = stage;
	block = stage_floor_blocks_ADDRESS + stage_floor_blocks[(uint16_t)(current_stage + 1u)];
	to_block = ROM16(link_point + 2u);
	block = block + 2u + to_block;
	stage_link_to_block = to_block;
	stage_link_to_x = ROM32(block);
	stage_link_to_y = ROM32(block + 8u);
	stage_link_to_height = ROM16(block + 0x10u);
	stage++;
	load_stage_map_pointers(&table_offset, &r10);
	stage_init_flags &= 0xFFFFFFF0u;
	*p_table_offset = table_offset;
	*p_link_record = r10;
	*p_block = block;
}

void load_stage_map_pointers(uint32_t *p_table_offset, uint32_t *p_bounds_list)
{
	uint32_t bounds_list = 0, stage_index = 0, table_offset = 0;

	current_restart_point = stage_restart_points_ADDRESS + stage_restart_points[stage];
	stage_index = stage;
	floor_blocks = stage_floor_blocks_ADDRESS + stage_floor_blocks[stage_index];
	table_offset = stage_floor_block_bounds[stage_index];
	bounds_list = stage_floor_block_bounds_ADDRESS + table_offset;
	floor_block_bounds = bounds_list;
	*p_table_offset = table_offset;
	*p_bounds_list = bounds_list;
}

void run_stage_events(uint32_t *p_table_offset, uint32_t *p_event)
{
	uint32_t table_offset = *p_table_offset;
	uint16_t distance_past = 0;
	uint32_t event = 0;
	uint32_t init_flags = 0;

	init_flags = stage_init_flags;
	stage_init_flags = init_flags | 0x100u;
	if ((init_flags & 0x100u) == 0u)
	{
		table_offset = stage_events[stage];
		stage_event_cursor = stage_events_ADDRESS + table_offset;
	}
	event = stage_event_cursor;
	if (scroll_position < ROM16(event))
	{
		*p_table_offset = table_offset;
		*p_event = event;
		return;
	}
	distance_past = (uint16_t)(scroll_position - ROM16(event));
	event += 2u;
	if (stage == 0x11u || distance_past < 0x100u)
	{
		game_mode = ROM8(event);
		scene_step = 0u;
		scene_timer = 0;
	}
	stage_event_cursor += 4u;
	*p_table_offset = table_offset;
	*p_event = event;
}

void reset_trackball_step(uint32_t player_number)
{
	uint32_t ignored = 0;

	trackball_step_sums[player_number] = 0u;
	read_trackball_step(&player_number, &ignored);
}

void read_trackball_step(uint32_t *p_player_number, uint32_t *p_trackball)
{
	uint32_t player_number = *p_player_number;
	uint16_t step = 0;
	uint32_t step_sum = 0, trackball = 0;

	trackball = trackball_1_x_ADDRESS + player_number * 4u;
	step_sum = trackball_step_sums_ADDRESS + player_number * 2u;
	if ((int16_t)(mem_read16(step_sum) ^ mem_read16(trackball)) < 0)
		mem_write16(step_sum, 0u);
	mem_write16(step_sum, mem_read16(step_sum) + mem_read16(trackball));
	step = 0u;
	if ((int16_t)mem_read16(step_sum) < 0x40)
	{
		if ((int16_t)mem_read16(step_sum) > -0x40)
		{
			*p_player_number = (*p_player_number & 0xFFFF0000u) | step;
			*p_trackball = trackball;
			return;
		}
		step = 0xFFFFu;
	}
	else
		step = 1u;
	mem_write16(step_sum, 0u);
	*p_player_number = (*p_player_number & 0xFFFF0000u) | step;
	*p_trackball = trackball;
}

uint32_t update_restart_point(uint32_t position)
{
	uint32_t restart_point = 0;

	restart_point = current_restart_point - 8u;
	position = (position & ~0xFFFFu) | scroll_position;
	do
	{
		restart_point += 8u;
	} while (ROM16(restart_point) <= (uint16_t)position);
	current_restart_point = restart_point;
	return position;
}

uint32_t start_player_objects_alt(uint32_t object)
{
	uint32_t r3 = 0, ignored = 0;

	player_handler_list = player_objects_alt_ADDRESS;
	reset_objects_from_list(object, &r3, &ignored);
	return r3;
}

uint32_t start_player_objects(uint32_t object)
{
	uint32_t r10 = 0, ignored = 0;

	player_handler_list = player_objects_ADDRESS;
	reset_objects_from_list(object, &ignored, &r10);
	return r10;
}

void reset_objects_from_list(uint32_t object, uint32_t *p_player_number, uint32_t *p_handler_cursor)
{
	uint8_t status = 0;
	uint32_t free_count = 0, handler_cursor = 0, handlers_left = 0, ignored = 0, player_number = 0;
	uint32_t handler_count = 0;

	player_block_cursor = PLAYER_BLOCK(1);
	reset_objects(&ignored, &player_number);
	handler_cursor = object_handler_list;
	handlers_left = mem_read16(handler_cursor);
	handler_cursor += 2u;
	object = allocate_object_or_start_list(object);
	OBJECT(object)->handler = mem_read32(handler_cursor);
	handler_cursor += 4u;
	if (OBJECT(object)->handler == player_start_stage_ADDRESS)
	{
		if (mem_read8(player_block_cursor + 9u) == 0u)
		{
			status = player_status[0];
			if (status == 2u || status == 1u || status == 3u)
				OBJECT(object)->handler = player_resume_ADDRESS;
		}
		else
			OBJECT(object)->handler = player_join_ADDRESS;
	}
	player_block_cursor += 0x200u;
	handler_count = handlers_left;
	handlers_left = handler_count - 1u;
	if ((int32_t)handler_count <= 1)
	{
		*p_player_number = player_number;
		*p_handler_cursor = handler_cursor;
		return;
	}
	do
	{
		allocate_object(&free_count, &object);
		if (free_count == 0u)
		{
			*p_player_number = player_number;
			*p_handler_cursor = handler_cursor;
			return;
		}
		OBJECT(object)->handler = mem_read32(handler_cursor);
		handler_cursor += 4u;
		if (OBJECT(object)->handler == player_start_stage_ADDRESS)
		{
			if (mem_read8(player_block_cursor + 9u) == 0u)
			{
				player_number = 1u;
				if (player_block_cursor != PLAYER_BLOCK(2))
					player_number = 2u;
				status = player_status[player_number];
				if (status == 2u || status == 1u || status == 3u)
					OBJECT(object)->handler = player_resume_ADDRESS;
			}
			else
				OBJECT(object)->handler = player_join_ADDRESS;
		}
		player_block_cursor += 0x200u;
		handlers_left--;
	} while (handlers_left != 0u);
	*p_player_number = player_number;
	*p_handler_cursor = handler_cursor;
}
