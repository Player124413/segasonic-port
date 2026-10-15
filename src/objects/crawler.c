/**
 *    Name:        objects/crawler.c
 *    Description: object type 21, never spawned: the crawler
 *    Version:     0.1.0
 *    Created:     12-10-2026
 *    Modified:    15-10-2026
 *    Author:      Sebastian Toyos
 *
 *    SegaSonic The Hedgehog (arcade, Sega System 32): the game's V60 program decompiled into C by
 *    tools/decompile.py, with the names of symbols.txt.
 */
#include "game.h"

void crawler_init(void)
{
	uint8_t segment_walk_step = 0;
	uint16_t segment_x = 0, step_stagger = 0;
	uint32_t block = 0, free_count = 0, object = 0, offset_record = 0, piece_count = 0, rom_record = 0, variant = 0;

	variant = OBJECT(current_object)->crawler.variant;
	r14 = 8u;
	step_stagger = 0u;
	segment_walk_step = 0u;
	segment_x = 0u;
	current_player = 0u;
	do
	{
		spawn_count = 2u;
		spawn_object_with_pieces(0x01276Bu, &offset_record, &piece_count, &free_count, &object, &block);
		copy_position_to(object);
		offset_record = crawler_segment_offsets_ADDRESS + variant * 4u;
		segment_x += ROM16(offset_record);
		current_player = (current_player & ~0xFFFFu) | (uint16_t)(current_player + ROM16(offset_record + 2u));
		OBJECT(object)->projected_x += segment_x;
		OBJECT(object)->projected_y_int += current_player;
		segment_walk_step++;
		if ((int8_t)segment_walk_step > 5)
			segment_walk_step = 0u;
		OBJECT(object)->direction = 0x280u;
		step_stagger += 4u;
		OBJECT(object)->crawler.step_timer = step_stagger;
		OBJECT(object)->crawler.walk_step = segment_walk_step;
		OBJECT(object)->sprite_palette = 0x90A0u;
		OBJECT(object)->flags |= 2u;
		OBJECT(object)->hitbox_size_x = 0x29u;
		OBJECT(object)->hitbox_size_y = 0x2Fu;
		OBJECT(object)->hitbox_offset_x = 0xA;
		OBJECT(object)->hitbox_offset_y = -0x3D;
		OBJECT(object)->hitbox_height = 0x20u;
		OBJECT(object)->kind = 4u;
		OBJECT(object)->handler = crawler_segment_animate_ADDRESS;
		OBJECT(object)->spawn_arg_1 = OBJECT(object_cursor)->spawn_arg_1;
		OBJECT(object)->spawn_arg_2 = OBJECT(object_cursor)->spawn_arg_2;
		current_object = object;
		object = LINKED(object, OBJECT(object)->child);
		OBJECT(object)->sprite_frame = 0x5D80u;
		OBJECT(object)->sprite_palette = 0x9FFEu;
		OBJECT(object)->sprite_scale = 0x100u;
		r14 = r14 - 1u;
	} while (r14 != 0u);
	OBJECT(current_object)->direction = 0x280u;
	OBJECT(current_object)->var_80 = 0xFFu;
	rom_record = crawler_tail_offsets_ADDRESS + variant * 4u;
	OBJECT(current_object)->projected_x += ROM16(rom_record);
	OBJECT(current_object)->projected_y_int += ROM16(rom_record + 2u);
	OBJECT(current_object)->sprite_frame = crawler_tail_frames[variant];
	OBJECT(current_object)->sprite_palette = 0x90C0u;
	rom_record = crawler_collision_boxes_ADDRESS + variant * 6u;
	OBJECT(current_object)->hitbox_size_x = ROM16(rom_record);
	rom_record += 2u;
	OBJECT(current_object)->hitbox_size_y = ROM16(rom_record);
	rom_record += 2u;
	OBJECT(current_object)->hitbox_offset_x = ROM8(rom_record);
	OBJECT(current_object)->hitbox_offset_y = ROM8(rom_record + 1u);
	OBJECT(current_object)->flags |= 2u;
	OBJECT(current_object)->hitbox_height = 0x20u;
	OBJECT(current_object)->kind = 0xDu;
	OBJECT(current_object)->handler = crawler_segment_update_ADDRESS;
	OBJECT(object_cursor)->parent = object_cursor;
	rom_record = crawler_head_offsets_ADDRESS + variant * 4u;
	OBJECT(object_cursor)->projected_x += ROM16(rom_record);
	OBJECT(object_cursor)->projected_y_int += ROM16(rom_record + 2u);
	OBJECT(object_cursor)->sprite_palette = 0x90B0u;
	variant = OBJECT(object_cursor)->crawler.variant;
	OBJECT(object_cursor)->sprite_frame = crawler_head_frames[variant];
	rom_record = crawler_collision_boxes_ADDRESS + variant * 6u;
	OBJECT(object_cursor)->hitbox_size_x = ROM16(rom_record);
	rom_record += 2u;
	OBJECT(object_cursor)->hitbox_size_y = ROM16(rom_record);
	rom_record += 2u;
	OBJECT(object_cursor)->hitbox_offset_x = ROM8(rom_record);
	OBJECT(object_cursor)->hitbox_offset_y = ROM8(rom_record + 1u);
	OBJECT(object_cursor)->hitbox_height = 0x20u;
	OBJECT(object_cursor)->kind = 0xDu;
	OBJECT(object_cursor)->flags |= 2u;
	OBJECT(object_cursor)->handler = crawler_head_update_ADDRESS;
	current_object = LINKED(current_object, OBJECT(object_cursor)->child);
	OBJECT(current_object)->sprite_frame = 0x5D80u;
	OBJECT(current_object)->sprite_palette = 0x9FFEu;
	OBJECT(current_object)->sprite_scale = 0x100u;
}

void crawler_head_update(void)
{
	uint32_t r2 = 0, cursor = 0, deleted = 0, depth = 0, ignored = 0, is_over_pit = 0, slope_height = 0;

	move_object_projected(&ignored, &r2, &slope_height, &cursor);
	fall_to_floor(&ignored, &is_over_pit, &r2);
	OBJECT(current_object)->crawler_head.over_pit = is_over_pit;
	delete_if_off_screen(&deleted, &r2, &slope_height);
	if ((uint8_t)deleted == 1u)
		return;
	find_nearest_player_x();
	if (OBJECT(current_object)->crawler_head.variant != 0u)
	{
		if (nearest_player_distance <= 0x35)
			OBJECT(current_object)->crawler_head.triggered = 1u;
	}
	else if (nearest_player_distance <= 0x28)
	{
		OBJECT(current_object)->crawler_head.triggered = 1u;
		if (nearest_player_distance <= 0x35)
			OBJECT(current_object)->crawler_head.triggered = 1u;
		OBJECT(current_object)->direction = 0x280u;
		add_to_touchable_list(deleted);
		add_to_rider_list();
		if (OBJECT(current_object)->crawler_head.over_pit != 0u && OBJECT(current_object)->crawler_head.on_platform != 0u)
		{
			depth = (uint16_t)(OBJECT(LINKED(WORK_RAM, OBJECT(current_object)->platform))->var_88 - 0xFu);
			queue_sprite_at_depth(&ignored, &depth);
			crawler_step();
			return;
		}
		queue_sprite(is_over_pit);
		current_object = LINKED(current_object, OBJECT(current_object)->child);
		place_sprite_at_cursor_floor();
		depth = (uint16_t)(OBJECT(current_object)->screen_y - 0xAu);
		queue_sprite_at_depth(&ignored, &depth);
		current_object = object_cursor;
		crawler_step();
		return;
	}
	OBJECT(current_object)->direction = 0x280u;
	add_to_touchable_list(deleted);
	add_to_rider_list();
	if (OBJECT(current_object)->crawler_head.over_pit != 0u && OBJECT(current_object)->crawler_head.on_platform != 0u)
	{
		depth = (uint16_t)(OBJECT(LINKED(WORK_RAM, OBJECT(current_object)->platform))->var_88 - 0xFu);
		queue_sprite_at_depth(&ignored, &depth);
		crawler_step();
		return;
	}
	queue_sprite(is_over_pit);
	current_object = LINKED(current_object, OBJECT(current_object)->child);
	place_sprite_at_cursor_floor();
	depth = (uint16_t)(OBJECT(current_object)->screen_y - 0xAu);
	queue_sprite_at_depth(&ignored, &depth);
	current_object = object_cursor;
	crawler_step();
}

void crawler_segment_animate(void)
{
	OBJECT(current_object)->crawler.frame_timer++;
	if (OBJECT(current_object)->crawler.frame_timer <= 3)
	{
		crawler_segment_update();
		return;
	}
	OBJECT(current_object)->crawler.frame_timer = 0;
	OBJECT(current_object)->sprite_frame = crawler_segment_frames[mem_read8(crawler_walk_steps_ADDRESS + (OBJECT(current_object)->crawler.variant * 6u + (uint8_t)OBJECT(current_object)->crawler.walk_step))];
	OBJECT(current_object)->crawler.walk_step++;
	if (OBJECT(current_object)->crawler.walk_step <= 5)
	{
		crawler_segment_update();
		return;
	}
	OBJECT(current_object)->crawler.walk_step = 0;
	crawler_segment_update();
}

void crawler_segment_update(void)
{
	uint32_t r2 = 0, cursor = 0, deleted = 0, depth = 0, ignored = 0, is_over_pit = 0, slope_height = 0;

	move_object_projected(&deleted, &r2, &slope_height, &cursor);
	if (OBJECT(LINKED(current_object, OBJECT(current_object)->parent))->var_80 != 0u)
	{
		delete_if_off_screen(&deleted, &r2, &slope_height);
		if ((uint8_t)deleted == 1u)
			return;
	}
	add_to_touchable_list(deleted);
	add_to_rider_list();
	fall_to_floor(&ignored, &is_over_pit, &r2);
	if ((uint8_t)is_over_pit != 0u)
	{
		if (OBJECT(LINKED(WORK_RAM, OBJECT(current_object)->parent))->var_80 == 0u)
		{
			OBJECT(current_object)->speed_z = 0;
			crawler_step();
			return;
		}
		if (OBJECT(current_object)->crawler.on_platform != 0u)
		{
			depth = (uint16_t)(OBJECT(LINKED(WORK_RAM, OBJECT(current_object)->platform))->var_88 - 0xFu);
			queue_sprite_at_depth(&ignored, &depth);
			crawler_step();
			return;
		}
	}
	if (OBJECT(current_object)->crawler.variant == 0u || (uint8_t)OBJECT(current_object)->crawler.walk_step != 0xFFu)
		queue_sprite(is_over_pit);
	else
	{
		depth = (uint16_t)(OBJECT(current_object)->screen_y + 8u);
		queue_sprite_at_depth(&ignored, &depth);
	}
	current_object = LINKED(current_object, OBJECT(current_object)->child);
	place_sprite_at_cursor_floor();
	depth = (uint16_t)(OBJECT(current_object)->screen_y - 0xAu);
	queue_sprite_at_depth(&ignored, &depth);
	current_object = object_cursor;
	crawler_step();
}

void crawler_step(void)
{
	uint32_t direction = 0, speed = 0, speed_x = 0, speed_y = 0;
	uint16_t step_timer = 0;

	if (OBJECT(LINKED(WORK_RAM, OBJECT(current_object)->parent))->var_8C == 0u)
		return;
	step_timer = OBJECT(current_object)->crawler.step_timer;
	OBJECT(current_object)->crawler.step_timer = step_timer - 1u;
	if (step_timer == 0u)
	{
		OBJECT(current_object)->speed_x = 0;
		OBJECT(current_object)->speed_y = 0;
		if (OBJECT(current_object)->crawler.moving == 0u)
		{
			OBJECT(current_object)->crawler.moving = 1u;
			OBJECT(current_object)->crawler.step_timer = 4u;
		}
		else
		{
			OBJECT(current_object)->crawler.moving = 0u;
			OBJECT(current_object)->crawler.step_timer = 4u;
		}
	}
	if (OBJECT(current_object)->crawler.moving != 0u)
	{
		direction = OBJECT(current_object)->spawn_arg_2;
		speed = 0x200u;
		direction_to_speed(speed, &direction, &speed_x, &speed_y);
		OBJECT(current_object)->speed_x = speed_x;
		OBJECT(current_object)->speed_y = speed_y;
	}
}
