/**
 *    Name:        objects/walls.c
 *    Description: the walls of spawn_walls (players and objects bounce off them); an unused wall editor
 *    Version:     0.1.0
 *    Created:     12-10-2026
 *    Modified:    15-10-2026
 *    Author:      Sebastian Toyos
 *
 *    SegaSonic The Hedgehog (arcade, Sega System 32): the game's V60 program decompiled into C by
 *    tools/decompile.py, with the names of symbols.txt.
 */
#include "game.h"

void spawn_walls(uint32_t *p_spawns_offset, uint32_t *p_part_size, uint32_t *p_second_sprite, uint32_t *p_point_y, uint32_t *p_z, uint32_t *p_wall)
{
	uint32_t spawns_offset = *p_spawns_offset;
	uint32_t part_size = *p_part_size;
	uint32_t second_sprite = *p_second_sprite;
	uint32_t point_y = *p_point_y;
	uint32_t z = *p_z;
	uint32_t wall = *p_wall;
	uint16_t blocks_needed = 0, past_spawn = 0;
	uint32_t r1 = 0, r2 = 0, corner_x = 0, free_count = 0, ignored = 0, kind_record = 0, object = 0, record = 0;
	uint16_t spawn_position = 0, current_scroll = 0;
	uint32_t init_flags = 0;

	init_flags = stage_init_flags;
	stage_init_flags = init_flags | 0x20u;
	if ((init_flags & 0x20u) == 0u)
	{
		spawns_offset = wall_spawns[stage];
		wall_spawn_cursor = wall_spawns_ADDRESS + spawns_offset;
	}
	spawn_position = mem_read16(wall_spawn_cursor);
	current_scroll = scroll_position;
	past_spawn = (uint16_t)(current_scroll - spawn_position);
	if (current_scroll < spawn_position)
	{
		*p_spawns_offset = spawns_offset;
		return;
	}
	if ((int16_t)past_spawn >= 0x100)
	{
		wall_spawn_cursor += 0xEu;
		*p_spawns_offset = spawns_offset;
		*p_part_size = part_size;
		*p_second_sprite = (*p_second_sprite & 0xFFFF0000u) | (second_sprite & 0xFFFFu);
		*p_point_y = point_y;
		*p_z = z;
		*p_wall = wall;
		return;
	}
	allocate_object_outside_loop(&free_count, &wall);
	if (free_count == 0u)
	{
		*p_spawns_offset = spawns_offset;
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
		build_wall_parts(&part_size, &second_sprite, &wall);
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
		project_point(z, &spawns_offset, &corner_x, &point_y);
		OBJECT(current_object)->speed_z_fraction = corner_x - OBJECT(current_object)->screen_x;
		point_y = (point_y & ~0xFFFFu) | (uint16_t)(point_y - OBJECT(current_object)->screen_y);
		OBJECT(current_object)->speed_z_int = point_y;
		wall_spawn_cursor += 0xEu;
		*p_spawns_offset = spawns_offset;
		*p_part_size = part_size;
		*p_second_sprite = (*p_second_sprite & 0xFFFF0000u) | (second_sprite & 0xFFFFu);
		*p_point_y = point_y;
		*p_z = z;
		*p_wall = wall;
		return;
	}
	object = current_object;
	delete_object_outside_loop(object, &r1, &r2);
	*p_spawns_offset = r1;
	*p_part_size = r2;
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

void wall_update_spare_return(void)
{
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

void wall_editor_set_side_label(void)
{
	uint32_t label_text = 0;

	label_text = wall_editor_far_label_ADDRESS;
	if (OBJECT(current_object)->wall_editor.depth_from_next == 0u)
		label_text = wall_editor_near_label_ADDRESS;
	move_bytes(wall_editor_side_text_ADDRESS, label_text, 0xCu);
}

void wall_editor_draw_arrow(uint32_t marker_x, uint32_t marker_y)
{
	uint8_t bucket = 0;
	uint32_t block_offset = 0, ignored = 0, marker = 0, marker_number = 0, y = 0;
	uint32_t saved_current_object = 0;

	marker = wall_editor_arrow_count_ADDRESS;
	marker_number = (uint16_t)wall_editor_arrow_count;
	marker_number = (uint16_t)(marker_number + 1u);
	if ((int16_t)marker_number >= 8)
		return;
	wall_editor_arrow_count = marker_number;
	marker_number = (uint16_t)(marker_number - 1u);
	marker = marker + 2u + (uint16_t)(marker_number * 0x80u);
	mem_write16(marker + 2u, 0x1004u);
	mem_write16(marker + 4u, 0x6388u);
	mem_write16(marker + 6u, 0x8230u);
	mem_write16(marker + 0x64u, 0u);
	mem_write32(marker + 0x30u, 0u);
	if ((OBJECT(object_cursor)->flags & 0x10u) != 0u || (OBJECT(object_cursor)->flags & 8u) != 0u)
	{
		mem_write32(marker + 0x30u, mem_read32(marker + 0x30u) | 8u);
		mem_write16(marker + 0x74u, OBJECT(object_cursor)->platform);
	}
	mem_write32(marker + 0x4Cu, marker_x);
	mem_write32(marker + 0x50u, marker_y);
	saved_current_object = current_object;
	current_object = marker;
	find_floor(&marker_number, &y, &block_offset, &ignored, &marker);
	OBJECT(current_object)->z = OBJECT(current_object)->floor_z;
	project_object(&ignored, &ignored);
	bucket = 0u;
	queue_sprite_in_bucket(bucket);
	current_object = saved_current_object;
}

void wall_editor_draw_arrow_at_z(uint32_t marker_x, uint32_t marker_y, uint32_t marker_z)
{
	uint8_t bucket = 0;
	uint32_t ignored = 0, marker = 0, marker_number = 0;
	uint32_t saved_current_object = 0;

	marker = wall_editor_arrow_count_ADDRESS;
	marker_number = (uint16_t)wall_editor_arrow_count;
	marker_number = (uint16_t)(marker_number + 1u);
	if ((int16_t)marker_number >= 8)
		return;
	wall_editor_arrow_count = marker_number;
	marker_number = (uint16_t)(marker_number - 1u);
	marker = marker + 2u + (uint16_t)(marker_number * 0x80u);
	mem_write16(marker + 2u, 0x1004u);
	mem_write16(marker + 4u, 0x6388u);
	mem_write16(marker + 6u, 0x8230u);
	mem_write16(marker + 0x64u, 0u);
	mem_write32(marker + 0x30u, 0u);
	if ((OBJECT(object_cursor)->flags & 0x10u) != 0u || (OBJECT(object_cursor)->flags & 8u) != 0u)
	{
		mem_write32(marker + 0x30u, mem_read32(marker + 0x30u) | 8u);
		mem_write16(marker + 0x74u, OBJECT(object_cursor)->platform);
	}
	mem_write32(marker + 0x4Cu, marker_x);
	mem_write32(marker + 0x50u, marker_y);
	mem_write32(marker + 0x40u, marker_z);
	saved_current_object = current_object;
	current_object = marker;
	project_object(&ignored, &ignored);
	bucket = 0u;
	queue_sprite_in_bucket(bucket);
	current_object = saved_current_object;
}

void wall_editor_draw_pattern_number(void)
{
	uint16_t attribute = 0;
	uint32_t text_position = 0, value = 0;

	value = (uint16_t)((int16_t)OBJECT(current_object)->sprite_frame / 0x14);
	attribute = 0u;
	text_position = TEXT_CELL(12, 18);
	value = draw_hex_2(value, attribute, text_position);
}

void wall_editor_rebuild_parts(uint32_t part)
{
	uint16_t child = 0;
	uint32_t r2 = 0, second_sprite = 0;

	current_player = current_object;
	while ((int16_t)OBJECT(current_player)->child >= 0)
	{
		child = OBJECT(current_player)->child;
		free_object_2(child);
		current_player = LINKED(current_player, OBJECT(current_player)->child);
	}
	build_wall_parts(&r2, &second_sprite, &part);
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
