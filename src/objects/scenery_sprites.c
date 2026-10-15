/**
 *    Name:        objects/scenery_sprites.c
 *    Description: the scenery sprites of stages 3, 8 and B-10
 *    Version:     0.1.0
 *    Created:     12-10-2026
 *    Modified:    15-10-2026
 *    Author:      Sebastian Toyos
 *
 *    SegaSonic The Hedgehog (arcade, Sega System 32): the game's V60 program decompiled into C by
 *    tools/decompile.py, with the names of symbols.txt.
 */
#include "game.h"

void scenery_sprite_start(void)
{
	uint32_t block_offset = 0, cursor = 0, ignored = 0, kind_record = 0, sprite_kind = 0, y = 0;

	sprite_kind = OBJECT(current_object)->sprite_frame;
	scenery_sprite_kind = sprite_kind;
	kind_record = scenery_sprite_kinds[sprite_kind] + scenery_sprite_kinds_ADDRESS;
	OBJECT(current_object)->sprite_flags = ROM16(kind_record);
	kind_record += 2u;
	OBJECT(current_object)->sprite_frame = ROM16(kind_record);
	kind_record += 2u;
	OBJECT(current_object)->sprite_palette = ROM16(kind_record);
	kind_record += 2u;
	OBJECT(current_object)->scenery_sprite.depth_offset = ROM16(kind_record);
	OBJECT(current_object)->handler = ROM32(kind_record + 2u);
	OBJECT(current_object)->sprite_offset_x = 0;
	OBJECT(current_object)->sprite_offset_y = 0;
	OBJECT(current_object)->flags = 0u;
	OBJECT(current_object)->scenery_sprite.unused_stage = stage;
	if (scenery_sprite_kind != 0xDu)
	{
		find_floor(&sprite_kind, &y, &block_offset, &ignored, &cursor);
		OBJECT(current_object)->z = OBJECT(current_object)->floor_z;
	}
	else
		OBJECT(current_object)->z = 0x8D0000;
	project_object(&ignored, &ignored);
}

void scenery_sprite_draw_until_you_failed(void)
{
	if (current_game_mode != 0x20u || game_mode_timer < 0x3C)
	{
		scenery_sprite_draw();
		return;
	}
	delete_current_object();
}

void scenery_sprite_draw(void)
{
	uint32_t r1 = 0, depth = 0, ignored = 0;

	if (map_screen_active == 0u)
	{
		update_screen_position(&ignored, &r1);
		depth = (uint16_t)(OBJECT(current_object)->screen_y + OBJECT(current_object)->scenery_sprite.depth_offset);
		queue_sprite_at_depth(&ignored, &depth);
		return;
	}
	delete_current_object();
}

void scenery_sprite_draw_until_offscreen(void)
{
	uint32_t r1 = 0, depth = 0, ignored = 0, off_screen = 0;

	off_screen = check_left_screen();
	if (off_screen == 0u)
	{
		update_screen_position(&ignored, &r1);
		depth = (uint16_t)(OBJECT(current_object)->screen_y + OBJECT(current_object)->scenery_sprite.depth_offset);
		queue_sprite_at_depth(&ignored, &depth);
		return;
	}
	delete_current_object();
}

void scenery_sprite_draw_wide_margin(void)
{
	uint32_t depth = 0, ignored = 0, margin = 0, off_screen = 0;

	margin = 0x50u;
	off_screen = check_left_screen_margin((uint16_t)margin);
	if (off_screen == 0u)
	{
		update_screen_position(&ignored, &margin);
		depth = (uint16_t)(OBJECT(current_object)->screen_y + OBJECT(current_object)->scenery_sprite.depth_offset);
		queue_sprite_at_depth(&ignored, &depth);
		return;
	}
	delete_current_object();
}

void scenery_sprite_cycle_palette_start(void)
{
	OBJECT(current_object)->scenery_sprite.palette_index = 0u;
	OBJECT(current_object)->scenery_sprite.palette_delay = 6u;
	OBJECT(current_object)->scenery_sprite.palette_timer = 0u;
	OBJECT(current_object)->scenery_sprite.palette_list = scenery_sprite_palettes_ADDRESS;
	OBJECT(current_object)->sprite_offset_x = 0;
	OBJECT(current_object)->sprite_offset_y = -0x10;
	OBJECT(current_object)->handler = scenery_sprite_cycle_palette_ADDRESS;
}

void scenery_sprite_cycle_palette(void)
{
	uint32_t r1 = 0, depth = 0, ignored = 0;

	if (map_screen_active == 0u)
	{
		animate_sign();
		update_screen_position(&ignored, &r1);
		depth = (uint16_t)(OBJECT(current_object)->screen_y + OBJECT(current_object)->scenery_sprite.depth_offset);
		queue_sprite_at_depth(&ignored, &depth);
		return;
	}
	delete_current_object();
}
