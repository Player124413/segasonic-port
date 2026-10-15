/**
 *    Name:        objects/eggman_booth.c
 *    Description: Eggman's booth that slides in during his speeches; object type 4 (a lone rsr)
 *    Version:     0.1.0
 *    Created:     12-10-2026
 *    Modified:    15-10-2026
 *    Author:      Sebastian Toyos
 *
 *    SegaSonic The Hedgehog (arcade, Sega System 32): the game's V60 program decompiled into C by
 *    tools/decompile.py, with the names of symbols.txt.
 */
#include "game.h"

void spawn_eggman_booth(uint32_t spawned, uint32_t piece_count, uint32_t booth, uint32_t *p_free_count, uint32_t *p_block)
{
	uint32_t free_count = *p_free_count;
	uint32_t block = *p_block;
	uint32_t object = 0;

	spawn_count = 2u;
	spawn_object_with_pieces(0x012B2Fu, &spawned, &piece_count, &free_count, &booth, &block);
	if ((uint8_t)spawned == 0u)
	{
		*p_free_count = free_count;
		*p_block = block;
		return;
	}
	OBJECT(booth)->sprite_flags |= 0x100u;
	OBJECT(booth)->eggman_booth.step_timer = 0x2F;
	OBJECT(booth)->sprite_palette = 0x8200u;
	OBJECT(booth)->screen_x = 0x9F;
	OBJECT(booth)->handler = eggman_booth_slide_in_ADDRESS;
	object = LINKED(booth, OBJECT(booth)->child);
	OBJECT(object)->sprite_frame = 0x5718u;
	OBJECT(object)->sprite_palette = 0x8210u;
	OBJECT(object)->screen_x = 0x9F;
	OBJECT(object)->sprite_flags |= 0x100u;
	*p_free_count = free_count;
	*p_block = block;
}

void eggman_booth_slide_in(void)
{
	OBJECT(current_object)->eggman_booth.slide_y += 4u;
	if (OBJECT(current_object)->eggman_booth.slide_y < 0x2C)
	{
		draw_eggman_booth();
		return;
	}
	OBJECT(current_object)->eggman_booth.slide_y = 0x2F;
	OBJECT(current_object)->handler = eggman_booth_animate_ADDRESS;
	OBJECT(current_object)->eggman_booth.step = 0u;
	eggman_booth_animate();
}

void eggman_booth_animate(void)
{
	uint32_t block_offset = 0, ignored = 0, slope_height = 0;
	uint16_t slide_y = 0;

	if (OBJECT(current_object)->eggman_booth.step != 0x1Du)
	{
		OBJECT(current_object)->eggman_booth.step_timer--;
		if (OBJECT(current_object)->eggman_booth.step_timer >= 0)
		{
			draw_eggman_booth();
			return;
		}
		OBJECT(current_object)->eggman_booth.step++;
		OBJECT(current_object)->eggman_booth.step_timer = eggman_booth_frame_durations[eggman_booth_steps[OBJECT(current_object)->eggman_booth.step]];
		draw_eggman_booth();
		return;
	}
	slide_y = OBJECT(current_object)->eggman_booth.slide_y;
	OBJECT(current_object)->eggman_booth.slide_y = slide_y - 4u;
	if (slide_y >= 4u)
	{
		draw_eggman_booth();
		return;
	}
	delete_object_and_children(&ignored, &block_offset, &slope_height);
}

void draw_eggman_booth(void)
{
	uint8_t bucket = 0, sound_id = 0;
	uint32_t booth_frame = 0, write_index_address = 0;

	OBJECT(current_object)->screen_y = OBJECT(object_cursor)->draw_eggman_booth.slide_y;
	booth_frame = eggman_booth_steps[OBJECT(current_object)->draw_eggman_booth.step];
	OBJECT(current_object)->sprite_frame = eggman_booth_frames[booth_frame];
	if (stage != 0u && stage != 5u && booth_frame == 0xBu)
	{
		sound_id = 0x41u;
		play_sound(sound_id, &booth_frame, &write_index_address);
	}
	bucket = 0u;
	queue_sprite_in_bucket(bucket);
	current_object = LINKED(current_object, OBJECT(current_object)->child);
	OBJECT(current_object)->screen_y = OBJECT(object_cursor)->draw_eggman_booth.slide_y;
	bucket = 0u;
	queue_sprite_in_bucket(bucket);
}

void object_type_4_idle(void)
{
}
