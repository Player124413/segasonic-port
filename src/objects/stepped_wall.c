/**
 *    Name:        objects/stepped_wall.c
 *    Description: object type 44 (stage A): the stepped wall
 *    Version:     0.1.0
 *    Created:     12-10-2026
 *    Modified:    15-10-2026
 *    Author:      Sebastian Toyos
 *
 *    SegaSonic The Hedgehog (arcade, Sega System 32): the game's V60 program decompiled into C by
 *    tools/decompile.py, with the names of symbols.txt.
 */
#include "game.h"

void stepped_wall_init(void)
{
	OBJECT(current_object)->sprite_flags = 6u;
	OBJECT(current_object)->sprite_frame = 0xB670u;
	OBJECT(current_object)->sprite_palette = 0x8D60u;
	OBJECT(current_object)->sprite_palette |= 0x2000u;
	OBJECT(current_object)->flags |= 2u;
	OBJECT(current_object)->hitbox_size_x = 0x1Du;
	OBJECT(current_object)->hitbox_size_y = 0xDCu;
	OBJECT(current_object)->hitbox_offset_x = 0x73;
	OBJECT(current_object)->hitbox_offset_y = -0x7E;
	OBJECT(current_object)->hitbox_height = 0x80u;
	OBJECT(current_object)->kind = 0xCu;
	OBJECT(current_object)->handler = stepped_wall_draw_ADDRESS;
}

void stepped_wall_draw(void)
{
	uint32_t block_offset = 0, bucket = 0, cursor = 0, deleted = 0, ignored = 0, slope_height = 0;

	unproject_object(&ignored, &block_offset, &slope_height, &cursor);
	delete_if_off_screen(&deleted, &block_offset, &slope_height);
	if ((uint8_t)deleted == 0u)
	{
		bucket = 0xFFu;
		queue_sprite_in_bucket(bucket);
		add_to_touchable_list(bucket);
		return;
	}
}
