/**
 *    Name:        objects/breakable_panel_and_props.c
 *    Description: object types 36-40: the breakable panel (stage 3), the white puffs, the checkered pad, the static triangle, the railing
 *    Version:     0.1.0
 *    Created:     12-10-2026
 *    Modified:    15-10-2026
 *    Author:      Sebastian Toyos
 *
 *    SegaSonic The Hedgehog (arcade, Sega System 32): the game's V60 program decompiled into C by
 *    tools/decompile.py, with the names of symbols.txt.
 */
#include "game.h"

void breakable_panel_init(void)
{
	OBJECT(current_object)->sprite_frame = 0x9278u;
	OBJECT(current_object)->sprite_palette = 0x91F0u;
	OBJECT(current_object)->flags |= 2u;
	OBJECT(current_object)->hitbox_size_x = 7u;
	OBJECT(current_object)->hitbox_size_y = 0x40u;
	OBJECT(current_object)->hitbox_offset_x = -4;
	OBJECT(current_object)->hitbox_offset_y = -0x28;
	OBJECT(current_object)->hitbox_height = 0x64u;
	OBJECT(current_object)->kind = 5u;
	OBJECT(current_object)->handler = breakable_panel_wait_ADDRESS;
	current_object = LINKED(current_object, OBJECT(current_object)->child);
	OBJECT(current_object)->sprite_frame = 0x9308u;
	OBJECT(current_object)->sprite_palette = 0x9200u;
}

void breakable_panel_wait(void)
{
	uint32_t allocated = 0, random_value = 0, ring = 0;

	if ((OBJECT(current_object)->flags & 4u) == 0u || (OBJECT(LINKED(WORK_RAM, OBJECT(current_object)->touched_by))->flags & 0x40u) == 0u)
	{
		draw_breakable_panel();
		return;
	}
	released_ring_flip = 0u;
	released_rings_random = 1u;
	released_ring_angle = OBJECT(current_object)->spawn_arg_1;
	rings_to_release = OBJECT(current_object)->spawn_arg_2;
	drop_rings(0u, &allocated, &random_value, &ring);
	OBJECT(current_object)->handler = breakable_panel_break_ADDRESS;
	breakable_panel_break();
}

void breakable_panel_break(void)
{
	uint16_t frame_timer = 0;

	frame_timer = OBJECT(current_object)->breakable_panel.frame_timer;
	OBJECT(current_object)->breakable_panel.frame_timer = frame_timer - 1u;
	if (frame_timer != 0u)
	{
		draw_breakable_panel();
		return;
	}
	OBJECT(current_object)->sprite_frame = breakable_panel_break_frames[(uint8_t)OBJECT(current_object)->breakable_panel.frame_index];
	OBJECT(current_object)->breakable_panel.frame_timer = 2u;
	OBJECT(current_object)->breakable_panel.frame_index++;
	if (OBJECT(current_object)->breakable_panel.frame_index <= 0x10)
	{
		draw_breakable_panel();
		return;
	}
	OBJECT(current_object)->handler = draw_breakable_panel_ADDRESS;
	draw_breakable_panel();
}

void draw_breakable_panel(void)
{
	uint16_t offset_x = 0, offset_y = 0;
	uint32_t block_offset = 0, cursor = 0, deleted = 0, depth = 0, ignored = 0, slope_height = 0;
	uint16_t direction = 0;

	unproject_object(&ignored, &block_offset, &slope_height, &cursor);
	delete_if_off_screen(&deleted, &block_offset, &slope_height);
	if ((uint8_t)deleted != 0u)
		return;
	depth = (uint16_t)(OBJECT(current_object)->screen_y - 0x20u);
	queue_sprite_at_depth(&ignored, &depth);
	add_to_touchable_list(ignored);
	if ((uint8_t)OBJECT(current_object)->draw_breakable_panel.frame_index != 0u)
		return;
	current_object = LINKED(current_object, OBJECT(current_object)->child);
	direction = OBJECT(current_object)->direction;
	OBJECT(current_object)->direction = direction - 1u;
	if (direction == 0u)
	{
		OBJECT(current_object)->sprite_palette = breakable_panel_glow_palettes[breakable_panel_glow_steps[AT8(OBJECT(current_object)->projected_y, 1)]];
		OBJECT(current_object)->direction = 2u;
		AT8(OBJECT(current_object)->projected_y, 1)++;
		if (AT8(OBJECT(current_object)->projected_y, 1) == 0xFu)
			OBJECT(current_object)->direction = 0xFu;
		if ((int8_t)AT8(OBJECT(current_object)->projected_y, 1) > 0x1C)
		{
			OBJECT(current_object)->direction = 0x1Eu;
			AT8(OBJECT(current_object)->projected_y, 1) = 0u;
		}
	}
	offset_x = 0u;
	offset_y = 0u;
	place_sprite_near_cursor(offset_x, offset_y);
	depth = (uint16_t)(OBJECT(object_cursor)->screen_y - 0x19u);
	queue_sprite_at_depth(&ignored, &depth);
}

void white_puff_init(void)
{
	OBJECT(current_object)->sprite_palette = 0x90A0u;
	OBJECT(current_object)->sprite_palette = 0x2000u;
	OBJECT(current_object)->handler = white_puff_animate_ADDRESS;
}

void white_puff_animate(void)
{
	uint32_t block_offset = 0, cursor = 0, deleted = 0, ignored = 0, slope_height = 0;
	uint8_t wrapped_frame_index = 0;
	uint16_t frame_timer = 0;

	unproject_object(&ignored, &block_offset, &slope_height, &cursor);
	delete_if_off_screen(&deleted, &block_offset, &slope_height);
	if ((uint16_t)deleted != 0u)
		return;
	frame_timer = OBJECT(current_object)->white_puff.frame_timer;
	OBJECT(current_object)->white_puff.frame_timer = frame_timer - 1u;
	if (frame_timer == 0u)
	{
		OBJECT(current_object)->sprite_frame = white_puff_frames[OBJECT(current_object)->white_puff.frame_index];
		OBJECT(current_object)->white_puff.frame_timer = 3u;
		OBJECT(current_object)->white_puff.frame_index++;
		wrapped_frame_index = OBJECT(current_object)->white_puff.frame_index % 0xAu;
		OBJECT(current_object)->white_puff.frame_index = wrapped_frame_index;
		if (wrapped_frame_index == 0u)
			OBJECT(current_object)->white_puff.frame_timer = 0x5Au;
	}
	queue_sprite(0u);
}

void checker_pad_init(void)
{
	OBJECT(current_object)->sprite_palette = 0x9040u;
	OBJECT(current_object)->handler = checker_pad_animate_ADDRESS;
}

void checker_pad_animate(void)
{
	uint32_t block_offset = 0, deleted = 0, slope_height = 0;
	uint16_t frame_timer = 0;

	update_screen_position_no_offset();
	delete_if_off_screen(&deleted, &block_offset, &slope_height);
	if ((uint8_t)deleted != 0u)
		return;
	frame_timer = OBJECT(current_object)->checker_pad.frame_timer;
	OBJECT(current_object)->checker_pad.frame_timer = frame_timer - 1u;
	if (frame_timer == 0u)
	{
		OBJECT(current_object)->sprite_frame = checker_pad_frames[AS16(OBJECT(current_object)->checker_pad.frame_index)];
		OBJECT(current_object)->checker_pad.frame_timer = 8u;
		OBJECT(current_object)->checker_pad.frame_index++;
		if (OBJECT(current_object)->checker_pad.frame_index > 4)
			OBJECT(current_object)->checker_pad.frame_index = 0;
	}
	queue_sprite(0u);
}

void static_triangle_init(void)
{
	OBJECT(current_object)->sprite_palette = 0x9050u;
	OBJECT(current_object)->handler = static_triangle_animate_ADDRESS;
	current_object = LINKED(current_object, OBJECT(current_object)->child);
	OBJECT(current_object)->sprite_frame = 0x7828u;
	OBJECT(current_object)->sprite_palette = 0x9060u;
}

void static_triangle_animate(void)
{
	uint16_t offset_x = 0, offset_y = 0;
	uint32_t block_offset = 0, deleted = 0, depth = 0, slope_height = 0;
	uint16_t frame_timer = 0, direction = 0;

	update_screen_position_no_offset();
	delete_if_off_screen(&deleted, &block_offset, &slope_height);
	if ((uint8_t)deleted != 0u)
		return;
	frame_timer = OBJECT(current_object)->static_triangle.frame_timer;
	OBJECT(current_object)->static_triangle.frame_timer = frame_timer - 1u;
	if (frame_timer == 0u)
	{
		OBJECT(current_object)->sprite_frame = static_triangle_frames[(uint8_t)OBJECT(current_object)->static_triangle.frame_index];
		OBJECT(current_object)->static_triangle.frame_timer = 2u;
		OBJECT(current_object)->static_triangle.frame_index++;
		if (OBJECT(current_object)->static_triangle.frame_index > 2)
		{
			OBJECT(current_object)->static_triangle.frame_index = 0;
			OBJECT(current_object)->var_81 = 0u;
		}
	}
	depth = queue_sprite(0u);
	current_object = LINKED(current_object, OBJECT(current_object)->child);
	direction = OBJECT(current_object)->direction;
	OBJECT(current_object)->direction = direction - 1u;
	if (direction == 0u)
	{
		OBJECT(current_object)->direction = 1u;
		offset_x = 0x68u;
		offset_y = 0u;
		place_sprite_near_cursor(offset_x, offset_y);
		queue_sprite(depth);
	}
}

void railing_init(void)
{
	uint32_t block = 0, object = 0, spawned = 0;

	OBJECT(current_object)->sprite_frame = 0x7838u;
	OBJECT(current_object)->sprite_palette = 0x9080u;
	OBJECT(current_object)->flags |= 2u;
	OBJECT(current_object)->hitbox_size_x = 0xFu;
	OBJECT(current_object)->hitbox_size_y = 0x57u;
	OBJECT(current_object)->hitbox_offset_x = 3;
	OBJECT(current_object)->hitbox_offset_y = -0x67;
	OBJECT(current_object)->hitbox_height = 0x100u;
	OBJECT(current_object)->kind = 0xCu;
	OBJECT(current_object)->handler = railing_draw_ADDRESS;
	spawn_count = 1u;
	spawn_objects(0x015389u, &spawned, &object, &block);
	OBJECT(object_cursor)->child = object;
	copy_position_to(object);
	OBJECT(object)->flags |= 2u;
	OBJECT(object)->hitbox_size_x = 0x56u;
	OBJECT(object)->hitbox_size_y = 0x11u;
	OBJECT(object)->hitbox_offset_x = 0x12;
	OBJECT(object)->hitbox_offset_y = -0x11;
	OBJECT(object)->hitbox_height = 0x100u;
	OBJECT(object)->kind = 0xCu;
	OBJECT(object)->handler = railing_side_update_ADDRESS;
}

void railing_draw(void)
{
	uint32_t block_offset = 0, cursor = 0, deleted = 0, ignored = 0, slope_height = 0;

	unproject_object(&ignored, &block_offset, &slope_height, &cursor);
	delete_if_off_screen(&deleted, &block_offset, &slope_height);
	if ((uint8_t)deleted == 0u)
	{
		queue_sprite(0u);
		add_to_touchable_list(deleted);
		return;
	}
}

void railing_side_update(void)
{
	uint32_t cursor = 0, ignored = 0;

	unproject_object(&ignored, &ignored, &ignored, &cursor);
	add_to_touchable_list(ignored);
}
