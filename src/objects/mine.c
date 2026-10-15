/**
 *    Name:        objects/mine.c
 *    Description: object types 29 and 30 (stage 10): the bubble column (never spawned) and the mines
 *    Version:     0.1.0
 *    Created:     12-10-2026
 *    Modified:    15-10-2026
 *    Author:      Sebastian Toyos
 *
 *    SegaSonic The Hedgehog (arcade, Sega System 32): the game's V60 program decompiled into C by
 *    tools/decompile.py, with the names of symbols.txt.
 */
#include "game.h"

void spawn_bubble_column(uint32_t spawned, uint32_t free_count, uint32_t column, uint32_t block)
{
	uint8_t sound_id = 0;
	uint32_t r1 = 0;

	spawn_count = 1u;
	spawn_object_with_pieces(0x013AB8u, &spawned, &r1, &free_count, &column, &block);
	if ((uint8_t)spawned != 0u)
	{
		OBJECT(column)->sprite_palette = 0x1060u;
		OBJECT(column)->spawn_arg_1 = r14;
		OBJECT(column)->handler = bubble_column_animate_ADDRESS;
		sound_id = 0x24u;
		play_sound(sound_id, &r1, &free_count);
	}
}

void bubble_column_init(void)
{
	uint8_t sound_id = 0;
	uint32_t write_index = 0, write_index_address = 0;

	OBJECT(current_object)->sprite_palette = 0x1060u;
	OBJECT(current_object)->handler = bubble_column_animate_ADDRESS;
	sound_id = 0x24u;
	play_sound(sound_id, &write_index, &write_index_address);
}

void bubble_column_animate(void)
{
	uint32_t cursor = 0, frame_base = 0, frame_index = 0, ignored = 0;
	uint16_t frame_timer = 0;

	unproject_object(&ignored, &ignored, &ignored, &cursor);
	frame_timer = OBJECT(current_object)->bubble_column.frame_timer;
	OBJECT(current_object)->bubble_column.frame_timer = frame_timer - 1u;
	if (frame_timer == 0u)
	{
		OBJECT(current_object)->bubble_column.frame_timer = 2u;
		frame_index = (uint8_t)OBJECT(current_object)->bubble_column.frame_index;
		frame_base = OBJECT(current_object)->spawn_arg_1;
		frame_base = (uint16_t)(frame_base * 0x13u);
		OBJECT(current_object)->sprite_frame = mine_bubble_frames[(uint16_t)(frame_index + frame_base)];
		OBJECT(current_object)->bubble_column.frame_index++;
		if (OBJECT(current_object)->bubble_column.frame_index > 0x13)
			OBJECT(current_object)->bubble_column.frame_index = 0;
	}
	queue_sprite(frame_base);
}

void mine_init(void)
{
	if (OBJECT(current_object)->spawn_arg_2 != 0u)
		OBJECT(current_object)->sprite_scale = 0x80u;
	OBJECT(current_object)->sprite_frame = 0x8610u;
	OBJECT(current_object)->hitbox_size_x = 8u;
	OBJECT(current_object)->hitbox_height = 0x1Eu;
	OBJECT(current_object)->kind = 0x24u;
	OBJECT(current_object)->handler = mine_float_ADDRESS;
	current_object = LINKED(current_object, OBJECT(current_object)->child);
	OBJECT(current_object)->sprite_scale += 0x24u;
	OBJECT(current_object)->sprite_scale = OBJECT(object_cursor)->sprite_scale;
	OBJECT(current_object)->sprite_frame = 0x5D90u;
	OBJECT(current_object)->sprite_palette = 0x7A0u;
	OBJECT(current_object)->sprite_palette |= 0x4000u;
	current_object = LINKED(current_object, OBJECT(current_object)->child);
	OBJECT(current_object)->sprite_palette = 0x1060u;
	OBJECT(current_object)->sprite_palette |= 0x4000u;
	current_object = LINKED(current_object, OBJECT(current_object)->child);
	AT8(OBJECT(current_object)->projected_y, 1) = 1u;
	OBJECT(current_object)->sprite_palette = 0x1060u;
	OBJECT(current_object)->sprite_palette |= 0x4000u;
	current_object = LINKED(current_object, OBJECT(current_object)->child);
	AT8(OBJECT(current_object)->projected_y, 1) = 2u;
	OBJECT(current_object)->sprite_palette = 0x1060u;
	OBJECT(current_object)->sprite_palette |= 0x4000u;
	OBJECT(current_object)->sprite_flags |= 0x40u;
}

void mine_float(void)
{
	face_nearest_player(0u, 0u);
	if (nearest_player_distance < 0x80)
		OBJECT(current_object)->handler = mine_fall_ADDRESS;
	OBJECT(current_object)->speed_z = 0;
	OBJECT(current_object)->mine.timer += 0x10u;
	OBJECT(current_object)->mine.timer &= 0x3FFu;
	OBJECT(current_object)->speed_z += sine_table[OBJECT(current_object)->mine.timer];
	mine_fall();
}

void mine_fall(void)
{
	uint8_t sinking = 0, sound_id = 0;
	uint32_t r0 = 0, r1 = 0, r2 = 0, r3 = 0, r11 = 0, bucket = 0, deleted = 0, ignored = 0;
	uint16_t odd_frame = 0;

	move_object_projected(&ignored, &r2, &r3, &r11);
	delete_if_off_screen(&deleted, &r2, &r3);
	if ((uint8_t)deleted == 1u)
		return;
	fall_to_floor(&r0, &r1, &r2);
	if (OBJECT(current_object)->z_int < (int16_t)OBJECT(current_object)->spawn_arg_1)
	{
		sinking = 0u;
		flash_mine_palette(sinking);
		queue_sprite_at_floor();
		add_to_touchable_list(r0);
		current_object = LINKED(current_object, OBJECT(current_object)->child);
		OBJECT(current_object)->direction++;
		odd_frame = OBJECT(current_object)->direction & 1u;
		OBJECT(current_object)->direction = odd_frame;
		if (odd_frame == 0u)
		{
			bucket = place_sprite_at_cursor_floor();
			queue_sprite_in_bucket(bucket);
		}
		return;
	}
	spawn_mine_splash(r0, r2, r3, r11);
	OBJECT(current_object)->mine.timer = 0u;
	OBJECT(current_object)->speed_z = 0;
	OBJECT(current_object)->z_int += 4u;
	OBJECT(current_object)->handler = mine_sink_ADDRESS;
	sound_id = 0x16u;
	play_sound(sound_id, &r1, &r2);
	animate_sinking_mine();
}

void mine_sink(void)
{
	uint8_t sound_id = 0;
	uint32_t r1 = 0, r2 = 0, cursor = 0, height = 0, ignored = 0;

	move_object_projected(&ignored, &r2, &ignored, &cursor);
	OBJECT(current_object)->speed_z -= 0x5800u;
	fall_to_floor(&height, &r1, &r2);
	if (height != 0u)
	{
		animate_sinking_mine();
		return;
	}
	sound_id = 0x20u;
	play_sound(sound_id, &r1, &r2);
	OBJECT(current_object)->mine.timer = 0u;
	OBJECT(current_object)->mine.frame_index = 0;
	OBJECT(current_object)->sprite_palette = 0x1070u;
	OBJECT(current_object)->handler = mine_explode_ADDRESS;
	current_object = LINKED(current_object, OBJECT(current_object)->child);
	AS8(OBJECT(current_object)->projected_y_int) = 1u;
	current_object = object_cursor;
	mine_explode();
}

void mine_explode(void)
{
	uint8_t sound = 0;
	uint32_t r2 = 0, cursor = 0, frame_index = 0, ignored = 0, slope_height = 0, write_index = 0;
	uint8_t next_frame = 0;
	uint16_t timer = 0;

	unproject_object(&ignored, &r2, &slope_height, &cursor);
	timer = OBJECT(current_object)->mine.timer;
	OBJECT(current_object)->mine.timer = timer - 1u;
	if (timer != 0u)
	{
		draw_mine_with_bubbles();
		return;
	}
	frame_index = (uint8_t)OBJECT(current_object)->mine.frame_index;
	OBJECT(current_object)->sprite_frame = underwater_explosion_frames[frame_index];
	OBJECT(current_object)->mine.timer = underwater_explosion_durations[frame_index];
	OBJECT(current_object)->mine.frame_index++;
	next_frame = (uint8_t)OBJECT(current_object)->mine.frame_index % 0x17u;
	OBJECT(current_object)->mine.frame_index = next_frame;
	if (next_frame == 0u)
	{
		sound = 0x24u;
		play_sound(sound, &write_index, &r2);
		delete_object_and_children(&ignored, &r2, &slope_height);
		return;
	}
	if (OBJECT(current_object)->mine.frame_index <= 0xF)
	{
		draw_mine_with_bubbles();
		return;
	}
	draw_mine_with_bubbles();
}

void animate_sinking_mine(void)
{
	uint8_t sinking = 0;

	sinking = 1u;
	flash_mine_palette(sinking);
	OBJECT(current_object)->sprite_frame = mine_frames[mine_sink_steps[OBJECT(current_object)->animate_sinking_mine.sink_step]];
	OBJECT(current_object)->animate_sinking_mine.sink_step++;
	OBJECT(current_object)->animate_sinking_mine.sink_step = OBJECT(current_object)->animate_sinking_mine.sink_step % 8u;
	draw_mine_with_bubbles();
}

void draw_mine_with_bubbles(void)
{
	uint16_t offset_x = 0, offset_y = 0;
	uint32_t bucket = 0, depth = 0, ignored = 0;
	uint16_t shadow_odd_frame = 0, bubble_1_odd_frame = 0, bubble_2_odd_frame = 0, bubble_3_odd_frame = 0;

	add_to_touchable_list(0u);
	queue_sprite_at_floor();
	current_object = LINKED(current_object, OBJECT(current_object)->child);
	if (AS8(OBJECT(current_object)->projected_y_int) == 0u)
	{
		OBJECT(current_object)->direction++;
		shadow_odd_frame = OBJECT(current_object)->direction & 1u;
		OBJECT(current_object)->direction = shadow_odd_frame;
		if (shadow_odd_frame == 0u)
		{
			bucket = place_sprite_at_cursor_floor();
			r14 = (uint16_t)OBJECT(current_object)->screen_y;
			queue_sprite_in_bucket(bucket);
		}
	}
	current_object = LINKED(current_object, OBJECT(current_object)->child);
	offset_x = 0u;
	offset_y = 0xFFF6u;
	place_sprite_near_cursor(offset_x, offset_y);
	OBJECT(current_object)->direction++;
	bubble_1_odd_frame = OBJECT(current_object)->direction & 1u;
	OBJECT(current_object)->direction = bubble_1_odd_frame;
	if (bubble_1_odd_frame != 0u)
	{
		OBJECT(current_object)->sprite_frame = mine_bubble_frames[AT8(OBJECT(current_object)->projected_y, 1)];
		AT8(OBJECT(current_object)->projected_y, 1)++;
		if ((int8_t)AT8(OBJECT(current_object)->projected_y, 1) > 0x13)
			AT8(OBJECT(current_object)->projected_y, 1) = 0u;
	}
	r14 = (uint16_t)(r14 + 1u);
	depth = r14;
	queue_sprite_at_depth(&ignored, &depth);
	current_object = LINKED(current_object, OBJECT(current_object)->child);
	offset_x = 5u;
	offset_y = 0xFFECu;
	place_sprite_near_cursor(offset_x, offset_y);
	OBJECT(current_object)->direction++;
	bubble_2_odd_frame = OBJECT(current_object)->direction & 1u;
	OBJECT(current_object)->direction = bubble_2_odd_frame;
	if (bubble_2_odd_frame != 0u)
	{
		OBJECT(current_object)->sprite_frame = mine_bubble_frames_2[AT8(OBJECT(current_object)->projected_y, 1)];
		AT8(OBJECT(current_object)->projected_y, 1)++;
		if ((int8_t)AT8(OBJECT(current_object)->projected_y, 1) > 0x13)
			AT8(OBJECT(current_object)->projected_y, 1) = 0u;
	}
	r14 = (uint16_t)(r14 - 2u);
	depth = r14;
	queue_sprite_at_depth(&ignored, &depth);
	current_object = LINKED(current_object, OBJECT(current_object)->child);
	offset_x = 0xAu;
	offset_y = 0xFFECu;
	place_sprite_near_cursor(offset_x, offset_y);
	OBJECT(current_object)->direction++;
	bubble_3_odd_frame = OBJECT(current_object)->direction & 1u;
	OBJECT(current_object)->direction = bubble_3_odd_frame;
	if (bubble_3_odd_frame != 0u)
	{
		OBJECT(current_object)->sprite_frame = mine_bubble_frames[AT8(OBJECT(current_object)->projected_y, 1)];
		AT8(OBJECT(current_object)->projected_y, 1)++;
		if ((int8_t)AT8(OBJECT(current_object)->projected_y, 1) > 0x13)
			AT8(OBJECT(current_object)->projected_y, 1) = 0u;
	}
	depth = (uint16_t)r14;
	queue_sprite_at_depth(&ignored, &depth);
}

void flash_mine_palette(uint8_t sinking)
{
	uint16_t flash_timer = 0;

	flash_timer = OBJECT(current_object)->flash_mine_palette.flash_timer;
	OBJECT(current_object)->flash_mine_palette.flash_timer = flash_timer - 1u;
	if (flash_timer != 0u)
		return;
	OBJECT(current_object)->flash_mine_palette.flash_timer = 4u;
	OBJECT(current_object)->sprite_palette = mine_flash_palettes[mine_flash_steps[OBJECT(current_object)->flash_mine_palette.flash_step]];
	if ((uint8_t)sinking != 0u)
	{
		OBJECT(current_object)->sprite_palette ^= 0x8000u;
		OBJECT(current_object)->sprite_palette |= 0x4000u;
	}
	OBJECT(current_object)->flash_mine_palette.flash_step++;
	OBJECT(current_object)->flash_mine_palette.flash_step = OBJECT(current_object)->flash_mine_palette.flash_step % 0xCu;
}

void spawn_mine_splash(uint32_t spawned, uint32_t free_count, uint32_t splash, uint32_t block)
{
	uint32_t object = 0, piece_count = 0;

	OBJECT(current_object)->sprite_palette = 0x1070u;
	spawn_count = 2u;
	spawn_object_with_pieces(0x013F45u, &spawned, &piece_count, &free_count, &splash, &block);
	if ((uint8_t)spawned != 0u)
	{
		copy_position_to(splash);
		OBJECT(splash)->projected_y_int++;
		OBJECT(splash)->sprite_palette = 0x90A0u;
		OBJECT(splash)->handler = mine_splash_animate_ADDRESS;
		object = LINKED(splash, OBJECT(splash)->child);
		OBJECT(object)->sprite_frame = 0x8570u;
		OBJECT(object)->sprite_palette = 0x1080u;
		OBJECT(object)->sprite_palette |= 0x4000u;
	}
}

void mine_splash_animate(void)
{
	uint32_t block_offset = 0, cursor = 0, depth = 0, ignored = 0, slope_height = 0;
	uint16_t frame_timer = 0, direction = 0;

	unproject_object(&ignored, &block_offset, &slope_height, &cursor);
	if (OBJECT(current_object)->mine_splash.frame_index > 0x19)
	{
		delete_object_and_children(&ignored, &block_offset, &slope_height);
		return;
	}
	frame_timer = OBJECT(current_object)->mine_splash.frame_timer;
	OBJECT(current_object)->mine_splash.frame_timer = frame_timer - 1u;
	if (frame_timer == 0u)
	{
		OBJECT(current_object)->mine_splash.frame_timer = 2u;
		OBJECT(current_object)->sprite_frame = mine_splash_frames[(uint8_t)OBJECT(current_object)->mine_splash.frame_index];
		OBJECT(current_object)->mine_splash.frame_index++;
	}
	queue_sprite_at_floor();
	current_object = LINKED(current_object, OBJECT(object_cursor)->child);
	if ((int8_t)AT8(OBJECT(current_object)->projected_y, 1) >= 0x14)
		return;
	direction = OBJECT(current_object)->direction;
	OBJECT(current_object)->direction = direction - 1u;
	if (direction == 0u)
	{
		OBJECT(current_object)->direction = 2u;
		OBJECT(current_object)->sprite_frame = mine_splash_spray_frames[AT8(OBJECT(current_object)->projected_y, 1)];
		AT8(OBJECT(current_object)->projected_y, 1)++;
	}
	OBJECT(current_object)->screen_x = OBJECT(object_cursor)->screen_x;
	OBJECT(current_object)->screen_y = OBJECT(object_cursor)->screen_y;
	OBJECT(current_object)->screen_y += 7u;
	depth = (uint16_t)(r14 + 2u);
	queue_sprite_at_depth(&ignored, &depth);
}
