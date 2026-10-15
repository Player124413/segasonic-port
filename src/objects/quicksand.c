/**
 *    Name:        objects/quicksand.c
 *    Description: stage A: quicksand, sinkholes, sand patches
 *    Version:     0.1.0
 *    Created:     12-10-2026
 *    Modified:    15-10-2026
 *    Author:      Sebastian Toyos
 *
 *    SegaSonic The Hedgehog (arcade, Sega System 32): the game's V60 program decompiled into C by
 *    tools/decompile.py, with the names of symbols.txt.
 */
#include "game.h"

void quicksand_start(void)
{
	uint32_t block_offset = 0, ignored = 0, object = 0, object_2 = 0, x = 0, y = 0;

	OBJECT(current_object)->flags = 0u;
	if (OBJECT(current_object)->sprite_frame != 0u)
		OBJECT(current_object)->flags |= 1u;
	OBJECT(current_object)->sprite_flags = 0x1000u;
	OBJECT(current_object)->sprite_frame = 0x74E0u;
	OBJECT(current_object)->sprite_offset_x = 0;
	OBJECT(current_object)->sprite_offset_y = -0x34;
	OBJECT(current_object)->hitbox_size_x = 0x60u;
	OBJECT(current_object)->hitbox_height = 0x50u;
	OBJECT(current_object)->hitbox_size_y = 0u;
	OBJECT(current_object)->handler = quicksand_swirl_ADDRESS;
	OBJECT(current_object)->kind = 0x1Cu;
	OBJECT(current_object)->quicksand.palette_index = 0u;
	OBJECT(current_object)->quicksand.palette_delay = 4u;
	OBJECT(current_object)->quicksand.palette_timer = 0u;
	OBJECT(current_object)->quicksand.palette_list = quicksand_palettes_ADDRESS;
	OBJECT(current_object)->animation_frame = 0u;
	OBJECT(current_object)->animation_speed = 4u;
	OBJECT(current_object)->animation_timer = 0;
	OBJECT(current_object)->animation = quicksand_animation_ADDRESS;
	find_floor(&x, &y, &block_offset, &ignored, &object);
	OBJECT(current_object)->z = OBJECT(current_object)->floor_z;
	OBJECT(current_object)->z_int += 0x30u;
	project_object(&ignored, &ignored);
	OBJECT(current_object)->child = 0xFFFFu;
	if (free_object_stack_2[0] >= 2)
	{
		allocate_object_2(&ignored, &ignored, &object);
		OBJECT(object)->sprite_flags = 0xD000u;
		OBJECT(current_object)->child = object;
		object_2 = object;
		allocate_object_2(&ignored, &ignored, &object);
		move_halfwords(object, quicksand_parts_ADDRESS, 0x11u);
		OBJECT(object_2)->part_list = object;
	}
}

void quicksand_swirl(void)
{
	uint16_t object = 0, sprite_frame = 0, step_frame = 0;
	uint32_t depth = 0, ignored = 0, margin = 0, next_step = 0, object_2 = 0, off_screen = 0, part_cursor = 0, parts_left = 0, step_list = 0;
	uint8_t animation_timer = 0;
	uint16_t last_step = 0;

	margin = 0x70u;
	off_screen = check_left_screen_margin((uint16_t)margin);
	if (off_screen == 0u)
	{
		add_to_touchable_list(0u);
		update_screen_position(&ignored, &margin);
		if ((OBJECT(current_object)->flags & 1u) != 0u)
			return;
		animate_sign();
		if ((int16_t)OBJECT(current_object)->child < 0)
		{
			current_object = object_cursor;
			depth = (uint16_t)(OBJECT(current_object)->screen_y - 0x58u);
			queue_sprite_at_depth(&ignored, &depth);
			return;
		}
		animation_timer = OBJECT(current_object)->animation_timer;
		OBJECT(current_object)->animation_timer = animation_timer - 1u;
		if ((int8_t)animation_timer <= 1)
		{
			OBJECT(current_object)->animation_timer = OBJECT(current_object)->animation_speed;
			step_list = OBJECT(current_object)->animation;
			next_step = OBJECT(current_object)->animation_frame;
			next_step = (uint8_t)(next_step + 1u);
			last_step = mem_read16(step_list);
			step_list += 2u;
			if ((int16_t)last_step < (int16_t)next_step)
				next_step = 0u;
			OBJECT(current_object)->animation_frame = next_step;
			step_frame = mem_read16(step_list + next_step * 2u);
			part_cursor = OBJECT(LINKED(current_object, OBJECT(current_object)->child))->part_list;
			parts_left = mem_read16(part_cursor);
			part_cursor += 2u;
			do
			{
				mem_write16(part_cursor, step_frame);
				part_cursor += 8u;
				parts_left--;
			} while (parts_left != 0u);
		}
		object_2 = LINKED(current_object, OBJECT(current_object)->child);
		OBJECT(object_2)->screen_x = OBJECT(current_object)->screen_x;
		OBJECT(object_2)->screen_y = OBJECT(current_object)->screen_y;
		current_object = LINKED(current_object, OBJECT(current_object)->child);
		depth = (uint16_t)(OBJECT(current_object)->screen_y - 0x58u);
		queue_sprite_at_depth(&ignored, &depth);
		current_object = object_cursor;
		depth = (uint16_t)(OBJECT(current_object)->screen_y - 0x58u);
		queue_sprite_at_depth(&ignored, &depth);
		return;
	}
	object_2 = LINKED(current_object, OBJECT(current_object)->child);
	sprite_frame = OBJECT(object_2)->sprite_frame;
	free_object_2(sprite_frame);
	object = (uint16_t)object_2;
	free_object_2(object);
	delete_current_object();
}

void sinkhole_start(void)
{
	uint32_t block_offset = 0, cursor = 0, growth_record = 0, ignored = 0, x = 0, y = 0;
	uint32_t sprite_frame = 0;

	OBJECT(current_object)->sprite_flags = 8u;
	sprite_frame = OBJECT(current_object)->sprite_frame;
	growth_record = sprite_frame & ~0x8000u;
	if ((sprite_frame & 0x8000u) != 0u)
		OBJECT(current_object)->sprite_flags |= 0x40u;
	growth_record = sinkhole_growth_records_ADDRESS + growth_record * 4u;
	OBJECT(current_object)->sinkhole.zoom_step = mem_read16(growth_record);
	OBJECT(current_object)->sinkhole.open_position = mem_read16(growth_record + 2u);
	OBJECT(current_object)->sinkhole.open_position += scroll_position;
	OBJECT(current_object)->sprite_palette = 0x1020u;
	OBJECT(current_object)->sprite_scale = 1u;
	OBJECT(current_object)->flags = 0u;
	OBJECT(current_object)->sinkhole.unscaled_offset_x = -8;
	if ((OBJECT(current_object)->sprite_flags & 0x40u) != 0u)
		OBJECT(current_object)->sinkhole.unscaled_offset_x = 0u - OBJECT(current_object)->sinkhole.unscaled_offset_x;
	OBJECT(current_object)->sinkhole.unscaled_offset_y = -0x30;
	OBJECT(current_object)->hitbox_size_x = 1u;
	OBJECT(current_object)->sinkhole.unscaled_hitbox_size_x = 0x4Cu;
	OBJECT(current_object)->hitbox_height = 0x40u;
	OBJECT(current_object)->hitbox_size_y = 0u;
	OBJECT(current_object)->handler = sinkhole_open_ADDRESS;
	OBJECT(current_object)->kind = 0x1Bu;
	OBJECT(current_object)->direction = 0x200u;
	if ((OBJECT(current_object)->sprite_flags & 0x40u) != 0u)
		OBJECT(current_object)->direction = 0x300u;
	OBJECT(current_object)->animation_frame = 0u;
	OBJECT(current_object)->animation_speed = 4u;
	OBJECT(current_object)->animation_timer = 0;
	OBJECT(current_object)->animation = sinkhole_animation_ADDRESS;
	find_floor(&x, &y, &block_offset, &ignored, &cursor);
	OBJECT(current_object)->z = OBJECT(current_object)->floor_z;
	project_object(&ignored, &ignored);
}

void sinkhole_open(void)
{
	uint32_t r1 = 0, r7 = 0, ignored = 0, off_screen = 0, scale = 0, sound_id = 0, write_index_address = 0;

	r1 = 0x68u;
	off_screen = check_left_screen_margin((uint16_t)r1);
	if (off_screen != 0u)
	{
		delete_current_object();
		return;
	}
	if (scroll_position < OBJECT(current_object)->sinkhole.open_position)
		return;
	if ((int16_t)OBJECT(current_object)->sprite_scale >= 0x180)
	{
		add_to_touchable_list(sound_id);
		update_screen_position(&ignored, &r1);
		OBJECT(current_object)->sinkhole.draw_depth = OBJECT(current_object)->screen_y;
		r1 = queue_sprite(r1);
		return;
	}
	if (OBJECT(current_object)->sprite_scale == 1u)
	{
		sound_id = 0xFu;
		play_sound((uint8_t)sound_id, &r1, &write_index_address);
	}
	OBJECT(current_object)->sprite_scale += OBJECT(current_object)->sinkhole.zoom_step;
	scale = OBJECT(current_object)->sprite_scale;
	OBJECT(current_object)->sprite_offset_x = (int32_t)(OBJECT(current_object)->sinkhole.unscaled_offset_x * scale) >> 8;
	OBJECT(current_object)->sprite_offset_y = (int32_t)(OBJECT(current_object)->sinkhole.unscaled_offset_y * scale) >> 8;
	animate(&scale, &r7, &off_screen);
	r1 = OBJECT(current_object)->sprite_scale;
	OBJECT(current_object)->hitbox_size_x = (int32_t)(OBJECT(current_object)->sinkhole.unscaled_hitbox_size_x * r1) >> 8;
	add_to_touchable_list(sound_id);
	update_screen_position(&ignored, &r1);
	OBJECT(current_object)->sinkhole.draw_depth = OBJECT(current_object)->screen_y;
	r1 = queue_sprite(r1);
}

void sand_patch_start(void)
{
	uint32_t block_offset = 0, cursor = 0, ignored = 0, x = 0, y = 0;

	OBJECT(current_object)->sprite_flags = 0x1000u;
	if ((OBJECT(current_object)->sprite_frame & 0x8000u) != 0u)
		OBJECT(current_object)->sprite_flags |= 0x40u;
	OBJECT(current_object)->sprite_frame = 0x7530u;
	OBJECT(current_object)->sprite_palette = 0x9020u;
	OBJECT(current_object)->handler = sand_patch_draw_ADDRESS;
	OBJECT(current_object)->flags = 0u;
	OBJECT(current_object)->sprite_offset_x = 0;
	OBJECT(current_object)->sprite_offset_y = 0;
	find_floor(&x, &y, &block_offset, &ignored, &cursor);
	OBJECT(current_object)->z = OBJECT(current_object)->floor_z;
	project_object(&ignored, &ignored);
}

void sand_patch_draw(void)
{
	uint16_t unseen_margin = 0;
	uint32_t depth = 0, ignored = 0, margin = 0, off_screen = 0;

	margin = 0x38u;
	unseen_margin = 0x10u;
	off_screen = check_left_screen_margins((uint16_t)margin, unseen_margin);
	if (off_screen == 0u)
	{
		update_screen_position(&ignored, &margin);
		depth = (uint16_t)(OBJECT(current_object)->screen_y - 0x38u);
		queue_sprite_at_depth(&ignored, &depth);
		return;
	}
	delete_current_object();
}

void rimmed_sinkhole_start(void)
{
	uint32_t block_offset = 0, free_count = 0, growth_record = 0, ignored = 0, object = 0, y = 0;
	uint32_t sprite_frame = 0;

	OBJECT(current_object)->sprite_flags = 0x1000u;
	OBJECT(current_object)->hitbox_size_x = 0x70u;
	OBJECT(current_object)->hitbox_height = 0x40u;
	OBJECT(current_object)->sprite_offset_x = 0;
	OBJECT(current_object)->sprite_offset_y = 0;
	OBJECT(current_object)->hitbox_size_y = 0u;
	AS16(OBJECT(current_object)->flags) = 0u;
	OBJECT(current_object)->kind = 0x1Du;
	allocate_object(&free_count, &object);
	if (free_count == 0u)
		return;
	OBJECT(object_cursor)->handler = rimmed_sinkhole_edge_zone_update_ADDRESS;
	current_object = object;
	OBJECT(object_cursor)->parent = current_object;
	OBJECT(current_object)->var_86 = object_cursor;
	OBJECT(current_object)->x = OBJECT(object_cursor)->x;
	OBJECT(current_object)->y = OBJECT(object_cursor)->y;
	OBJECT(current_object)->sprite_flags = 0u;
	sprite_frame = OBJECT(object_cursor)->sprite_frame;
	growth_record = sprite_frame & ~0x8000u;
	if ((sprite_frame & 0x8000u) != 0u)
		OBJECT(current_object)->sprite_flags |= 0x40u;
	growth_record = sinkhole_growth_records_ADDRESS + growth_record * 4u;
	AS16(OBJECT(current_object)->var_80) = mem_read16(growth_record);
	OBJECT(current_object)->var_84 = mem_read16(growth_record + 2u);
	OBJECT(current_object)->var_84 += scroll_position;
	OBJECT(current_object)->sprite_frame = 0x7518u;
	OBJECT(current_object)->sprite_palette = 0x9020u;
	OBJECT(current_object)->sprite_scale = 1u;
	OBJECT(current_object)->flags = 0u;
	OBJECT(current_object)->sprite_offset_x = 0;
	OBJECT(current_object)->sprite_offset_y = 0;
	OBJECT(current_object)->handler = rimmed_sinkhole_open_ADDRESS;
	OBJECT(current_object)->hitbox_size_x = 1u;
	OBJECT(current_object)->var_8C = 0x4Cu;
	OBJECT(current_object)->hitbox_height = 0x40u;
	OBJECT(current_object)->hitbox_size_y = 0u;
	OBJECT(current_object)->kind = 0x1Bu;
	OBJECT(current_object)->direction = 0u;
	if ((OBJECT(current_object)->sprite_flags & 0x40u) != 0u)
		OBJECT(current_object)->direction = 0x100u;
	OBJECT(current_object)->animation = rimmed_sinkhole_animation_ADDRESS;
	OBJECT(current_object)->animation_frame = 0u;
	OBJECT(current_object)->animation_speed = 4u;
	OBJECT(current_object)->animation_timer = 0;
	find_floor(&free_count, &y, &block_offset, &ignored, &object);
	OBJECT(current_object)->z = OBJECT(current_object)->floor_z;
	OBJECT(object_cursor)->z = OBJECT(current_object)->floor_z;
	project_object(&ignored, &ignored);
	allocate_object_2(&free_count, &ignored, &object);
	if (free_count == 0u)
		return;
	OBJECT(object)->sprite_flags = 0u;
	if ((OBJECT(current_object)->sprite_flags & 0x40u) != 0u)
		OBJECT(object)->sprite_flags |= 0x40u;
	OBJECT(object)->sprite_frame = 0x7538u;
	OBJECT(object)->sprite_palette = 0xF020u;
	OBJECT(object)->sprite_scale = 1u;
	OBJECT(current_object)->child = object;
}

void rimmed_sinkhole_open(void)
{
	uint16_t child = 0;
	uint32_t r7 = 0, depth = 0, ignored = 0, margin = 0, off_screen = 0, sound_id = 0, unseen_margin = 0;

	margin = 0x58u;
	unseen_margin = 0x20u;
	off_screen = check_left_screen_margins((uint16_t)margin, (uint16_t)unseen_margin);
	if (off_screen != 0u)
	{
		child = OBJECT(current_object)->child;
		free_object_2(child);
		current_object = (current_object & ~0xFFFFu) | OBJECT(current_object)->rimmed_sinkhole.edge_zone;
		OBJECT(current_object)->flags |= 0x400u;
		delete_current_object();
		return;
	}
	if (scroll_position < OBJECT(current_object)->rimmed_sinkhole.open_position)
		return;
	if ((int16_t)OBJECT(current_object)->sprite_scale < 0x100)
	{
		if (OBJECT(current_object)->sprite_scale == 1u)
		{
			sound_id = 0xFu;
			play_sound((uint8_t)sound_id, &margin, &unseen_margin);
		}
		OBJECT(current_object)->sprite_scale += OBJECT(current_object)->rimmed_sinkhole.zoom_step;
		OBJECT(LINKED(current_object, OBJECT(current_object)->child))->sprite_scale = OBJECT(current_object)->sprite_scale;
		animate(&margin, &r7, &off_screen);
		OBJECT(current_object)->hitbox_size_x = (int32_t)((uint32_t)OBJECT(current_object)->rimmed_sinkhole.unscaled_hitbox_size_x * OBJECT(current_object)->sprite_scale) >> 8;
	}
	add_to_touchable_list(sound_id);
	update_screen_position(&ignored, &margin);
	depth = (uint16_t)(OBJECT(current_object)->screen_y - 0x30u);
	if ((int16_t)depth <= -0xF)
		depth = 0xFFF1u;
	OBJECT(current_object)->rimmed_sinkhole.draw_depth = depth;
	queue_sprite_at_depth(&ignored, &depth);
	current_object = LINKED(current_object, OBJECT(current_object)->child);
	OBJECT(current_object)->screen_x = OBJECT(object_cursor)->screen_x;
	OBJECT(current_object)->screen_y = OBJECT(object_cursor)->screen_y;
	depth = (uint16_t)(OBJECT(object_cursor)->rimmed_sinkhole.draw_depth - 8u);
	queue_sprite_at_depth(&ignored, &depth);
}

void rimmed_sinkhole_edge_zone_update(void)
{
	uint32_t r0 = 0, object = 0;

	if ((OBJECT(current_object)->flags & 0x400u) == 0u)
	{
		object = LINKED(current_object, OBJECT(current_object)->parent);
		OBJECT(current_object)->sprite_scale = OBJECT(object)->sprite_scale;
		OBJECT(current_object)->rimmed_sinkhole.draw_depth = OBJECT(object)->var_88;
		r0 = add_to_touchable_list(0u);
		return;
	}
	delete_current_object();
}
