/**
 *    Name:        objects/electric_fence.c
 *    Description: object type 1 (stage 4): the electric fence posts around the flame machine
 *    Version:     0.1.0
 *    Created:     12-10-2026
 *    Modified:    15-10-2026
 *    Author:      Sebastian Toyos
 *
 *    SegaSonic The Hedgehog (arcade, Sega System 32): the game's V60 program decompiled into C by
 *    tools/decompile.py, with the names of symbols.txt.
 */
#include "game.h"

void fence_post_init(void)
{
	uint16_t arc_phase = 0;
	uint32_t block = 0, free_count = 0, object = 0, object_2 = 0, piece_count = 0, rom_record = 0, row_direction = 0;
	uint32_t posts_left = 0;

	electric_fence_state = 1u;
	r14 = OBJECT(current_object)->fence_post.extra_posts;
	row_direction = OBJECT(current_object)->fence_post.row_direction;
	arc_phase = OBJECT(current_object)->fence_post.arc_phase;
	for (;;)
	{
		OBJECT(current_object)->sprite_frame = 0x6CA8u;
		OBJECT(current_object)->sprite_palette = 0x9060u;
		OBJECT(current_object)->flags |= 2u;
		OBJECT(current_object)->hitbox_height = 0x64u;
		OBJECT(current_object)->kind = 0xEu;
		rom_record = fence_post_boxes_ADDRESS + row_direction * 4u;
		OBJECT(current_object)->hitbox_size_x = ROM16(rom_record);
		OBJECT(current_object)->hitbox_size_y = ROM16(rom_record + 2u);
		rom_record = fence_post_box_offsets_ADDRESS + row_direction * 2u;
		OBJECT(current_object)->hitbox_offset_x = ROM8(rom_record);
		OBJECT(current_object)->hitbox_offset_y = ROM8(rom_record + 1u);
		OBJECT(current_object)->handler = fence_post_start_ADDRESS;
		object = LINKED(WORK_RAM, OBJECT(current_object)->child);
		OBJECT(object)->direction = arc_phase;
		OBJECT(object)->sprite_frame = 0x6C88u;
		OBJECT(object)->sprite_palette = 0x9030u;
		OBJECT(object)->sprite_flags = fence_arc_flags[row_direction];
		posts_left = r14;
		r14 = posts_left - 1u;
		if (posts_left == 0u)
			break;
		current_player = current_object;
		spawn_count = 2u;
		spawn_object_with_pieces(0x013956u, &object, &piece_count, &free_count, &object_2, &block);
		if ((uint8_t)object == 0u)
			break;
		OBJECT(object_2)->screen_x = OBJECT(current_player)->screen_x;
		OBJECT(object_2)->screen_y = OBJECT(current_player)->screen_y;
		OBJECT(object_2)->projected_x = OBJECT(current_player)->projected_x;
		OBJECT(object_2)->projected_y = OBJECT(current_player)->projected_y;
		OBJECT(object_2)->z = OBJECT(current_player)->z;
		OBJECT(object_2)->floor_z = OBJECT(current_player)->floor_z;
		OBJECT(object_2)->spawn_arg_2 = OBJECT(current_player)->spawn_arg_2;
		rom_record = fence_post_offsets_ADDRESS + row_direction * 4u;
		OBJECT(object_2)->projected_x += ROM16(rom_record);
		OBJECT(object_2)->projected_y_int += ROM16(rom_record + 2u);
		current_object = object_2;
	}
}

void fence_post_start(void)
{
	uint32_t cursor = 0, ignored = 0;

	add_to_rider_list();
	unproject_object(&ignored, &ignored, &ignored, &cursor);
	OBJECT(current_object)->handler = fence_post_update_ADDRESS;
}

void fence_post_update(void)
{
	uint16_t offset_x = 0, offset_y = 0;
	uint32_t arc_offset_record = 0, block_offset = 0, depth = 0, ignored = 0, slope_height = 0;
	uint16_t odd_frame = 0;

	if (electric_fence_state != 1u)
	{
		if (electric_fence_state == 0xFFu)
		{
			delete_object_and_children(&ignored, &block_offset, &slope_height);
			return;
		}
		OBJECT(current_object)->kind = 5u;
		OBJECT(current_object)->hitbox_size_x = 0x1Bu;
		OBJECT(current_object)->hitbox_size_y = 0x19u;
		OBJECT(current_object)->hitbox_offset_x = -9;
		OBJECT(current_object)->hitbox_offset_y = -0x13;
		OBJECT(current_object)->handler = fence_post_draw_ADDRESS;
	}
	current_object = LINKED(current_object, OBJECT(current_object)->child);
	arc_offset_record = fence_arc_offsets_ADDRESS + OBJECT(object_cursor)->fence_post.row_direction * 4u;
	offset_x = ROM16(arc_offset_record);
	offset_y = ROM16(arc_offset_record + 2u);
	place_sprite_near_cursor(offset_x, offset_y);
	AS16(OBJECT(current_object)->handler)++;
	odd_frame = AS16(OBJECT(current_object)->handler) & 1u;
	AS16(OBJECT(current_object)->handler) = odd_frame;
	if (odd_frame == 0u)
	{
		OBJECT(current_object)->direction++;
		OBJECT(current_object)->direction &= 3u;
		OBJECT(current_object)->sprite_frame = fence_arc_frames[OBJECT(current_object)->direction];
	}
	depth = (uint16_t)(OBJECT(current_object)->screen_y + 8u);
	queue_sprite_at_depth(&ignored, &depth);
	current_object = object_cursor;
	fence_post_draw();
}

void fence_post_draw(void)
{
	uint32_t r1 = 0, r2 = 0, cursor = 0, deleted = 0, ignored = 0, slope_height = 0;

	unproject_object(&ignored, &r2, &slope_height, &cursor);
	fall_to_floor(&ignored, &r1, &r2);
	delete_if_off_screen(&deleted, &r2, &slope_height);
	if ((uint8_t)deleted != 1u)
	{
		queue_sprite(r1);
		OBJECT(current_object)->health = 0x7F;
		add_to_touchable_list(deleted);
		add_to_rider_list();
	}
}
