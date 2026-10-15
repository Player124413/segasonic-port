/**
 *    Name:        scenes/ending_credits.c
 *    Description: the ending's credits (portraits, name plates, scrolling blocks) and after them: Eggman flying in, turning and away, the three running across
 *    Version:     0.1.0
 *    Created:     12-10-2026
 *    Modified:    15-10-2026
 *    Author:      Sebastian Toyos
 *
 *    SegaSonic The Hedgehog (arcade, Sega System 32): the game's V60 program decompiled into C by
 *    tools/decompile.py, with the names of symbols.txt.
 */
#include "game.h"

void ending_credits_fade_in(void)
{
	uint32_t ignored = 0, object = 0;

	if (OBJECT(current_object)->ending.started == 0u)
	{
		object = load_credits_palettes();
		color_offset_2_red = 0x1Fu;
		color_offset_2_blue = 0x1Fu;
		color_offset_2_green = 0x1Fu;
		allocate_object_outside_loop(&ignored, &object);
		OBJECT(object)->handler = ending_credits_spawner_run_ADDRESS;
		OBJECT(object)->ending.started = 0u;
		mem_write16(video_layer_disable, mem_read16(video_layer_disable) & 0xFF0u);
		mem_write16(video_layer_disable, mem_read16(video_layer_disable) | 0x10A8u);
		mem_write16(video_backdrop_control, mem_read16(video_backdrop_control) | 0x200u);
		mem_write16(backdrop_color, 0x5FD2u);
		nbg2_scroll_x = 0u;
		nbg2_scroll_y = 0u;
		nbg1_scroll_x_int = 0u;
		nbg1_scroll_y_int = 0u;
		mixer_blend &= 0x1F00u;
		OBJECT(current_object)->ending.cycle_count = 0;
		OBJECT(current_object)->ending.timer = 0;
		OBJECT(current_object)->ending.started = 1u;
		OBJECT(current_object)->ending.step = 0u;
	}
	OBJECT(current_object)->ending.timer++;
	if (OBJECT(current_object)->ending.timer < 0x78)
		return;
	OBJECT(current_object)->ending.timer = 0x79;
	if (OBJECT(current_object)->ending.step == 0u)
	{
		allocate_object_outside_loop(&ignored, &object);
		OBJECT(object)->handler = ending_music_play_ADDRESS;
		OBJECT(object)->ending.started = 0u;
		OBJECT(current_object)->ending.step = 1u;
	}
	if (color_offset_2_red == 0u)
	{
		delete_current_object();
		return;
	}
	OBJECT(current_object)->ending.cycle_count++;
	if (OBJECT(current_object)->ending.cycle_count >= 3)
	{
		color_offset_2_red--;
		color_offset_2_blue--;
		color_offset_2_green--;
		OBJECT(current_object)->ending.cycle_count = 0;
	}
}

void ending_credits_spawner_run(void)
{
	if (OBJECT(current_object)->ending.started == 0u)
	{
		OBJECT(current_object)->ending.started = 1u;
		OBJECT(current_object)->ending.timer = 0;
		OBJECT(current_object)->ending.list_index = 0u;
	}
	OBJECT(current_object)->ending.timer++;
	run_timed_spawns_3();
}

uint32_t load_credits_palettes(void)
{
	uint32_t dest = 0, ignored = 0, palette_list = 0;

	mem_write32(video_control, mem_read32(video_control) & ~0x400u);
	mem_write16(video_control, mem_read16(video_control) | 0x400u);
	vblank_copies_pending = 0u;
	vblank_copy_count = 0u;
	palette_list = tile_bank_1_palette_list_ADDRESS;
	load_palette_list(palette_list, &ignored, &dest);
	return dest;
}

void ending_music_play(void)
{
	uint8_t sound_id = 0;
	uint32_t cue_record = 0, ignored = 0;

	if (OBJECT(current_object)->ending.started == 0u)
	{
		OBJECT(current_object)->ending.started = 1u;
		OBJECT(current_object)->ending.timer = 0;
		sound_id = 0x94u;
		queue_sound(sound_id, &ignored, &ignored);
		sound_id = 0x82u;
		queue_sound(sound_id, &ignored, &ignored);
		OBJECT(current_object)->sprite_offset_x = 0;
		OBJECT(current_object)->sprite_offset_y = 0;
		OBJECT(current_object)->ending.list_index = 0u;
	}
	cue_record = ending_music_cues_ADDRESS + OBJECT(current_object)->ending.list_index * 4u;
	if (ROM16(cue_record) == 0xFFFFu)
	{
		delete_current_object();
		return;
	}
	if ((int16_t)ROM16(cue_record) <= OBJECT(current_object)->ending.timer)
	{
		sound_id = (uint8_t)ROM16(cue_record + 2u);
		queue_sound(sound_id, &ignored, &ignored);
		OBJECT(current_object)->ending.list_index++;
	}
	OBJECT(current_object)->ending.timer++;
}

void ending_portrait_show(void)
{
	uint16_t speed_y = 0;
	uint32_t backdrop_object = 0, ignored = 0, palette_record = 0, timed_record = 0;

	if (OBJECT(current_object)->ending.started == 0u)
	{
		OBJECT(current_object)->ending.started = 1u;
		OBJECT(current_object)->ending.timer = 0;
		allocate_object_2(&ignored, &ignored, &backdrop_object);
		OBJECT(current_object)->speed_y = backdrop_object;
	}
	OBJECT(current_object)->ending.timer++;
	timed_record = ending_portrait_frames_ADDRESS;
	palette_record = ending_portrait_palettes_ADDRESS;
	while ((int16_t)ROM16(timed_record) < OBJECT(current_object)->ending.timer)
	{
		timed_record += 4u;
		palette_record += 4u;
		if (ROM16(timed_record) == 0xFFFFu)
		{
			speed_y = (uint16_t)OBJECT(current_object)->speed_y;
			free_object_2(speed_y);
			delete_current_object();
			return;
		}
	}
	OBJECT(current_object)->sprite_frame = ROM16(timed_record + 2u);
	OBJECT(current_object)->sprite_palette = ROM16(palette_record + 2u);
	OBJECT(current_object)->screen_x = 0xE8;
	OBJECT(current_object)->screen_y = 0x4C;
	OBJECT(current_object)->sprite_layer = 0u;
	OBJECT(current_object)->sprite_scale = 0x100u;
	OBJECT(current_object)->sprite_flags = 0u;
	queue_sprite_in_layer();
	timed_record = ending_portrait_backdrop_palettes_ADDRESS;
	while ((int16_t)ROM16(timed_record) < OBJECT(current_object)->ending.timer)
	{
		timed_record += 4u;
		if (ROM16(timed_record) == 0xFFFFu)
			return;
	}
	current_object = OBJECT(current_object)->speed_y;
	OBJECT(current_object)->sprite_frame = 0x9880u;
	OBJECT(current_object)->sprite_palette = ROM16(timed_record + 2u);
	OBJECT(current_object)->screen_x = 0xE8;
	OBJECT(current_object)->screen_y = 0x4C;
	OBJECT(current_object)->sprite_layer = 0u;
	OBJECT(current_object)->sprite_scale = 0x100u;
	OBJECT(current_object)->sprite_flags = 0u;
	queue_sprite_in_layer();
}

void ending_name_plate_show(void)
{
	uint32_t frame_record = 0, ignored = 0, object = 0;

	if (OBJECT(current_object)->ending.started == 0u)
	{
		OBJECT(current_object)->ending.started = 1u;
		OBJECT(current_object)->ending.timer = 0;
		mem_write16(video_clip_1_left, 0xB1u);
		mem_write16(video_clip_1_right, 0x120u);
		mem_write16(video_clip_1_top, 0x21u);
		mem_write16(video_clip_1_bottom, 0x78u);
		allocate_object_2(&ignored, &ignored, &object);
		OBJECT(current_object)->z = object;
	}
	OBJECT(current_object)->ending.timer++;
	frame_record = ending_name_plate_frames_ADDRESS;
	while ((int16_t)ROM16(frame_record) < OBJECT(current_object)->ending.timer)
	{
		frame_record += 4u;
		if (ROM16(frame_record) == 0xFFFFu)
		{
			allocate_object_outside_loop(&ignored, &object);
			OBJECT(object)->handler = ending_window_open_ADDRESS;
			OBJECT(object)->ending.started = 0u;
			delete_current_object();
			return;
		}
	}
	nbg0_scroll_x_int++;
	OBJECT(current_object)->sprite_frame = ROM16(frame_record + 2u);
	OBJECT(current_object)->screen_x = 0xE8;
	OBJECT(current_object)->screen_y = 0xA3;
	OBJECT(current_object)->sprite_layer = 0u;
	OBJECT(current_object)->sprite_scale = 0x100u;
	OBJECT(current_object)->sprite_flags = 0u;
	OBJECT(current_object)->sprite_palette = 0x1020u;
	queue_sprite_in_layer();
	current_object = OBJECT(current_object)->z;
	OBJECT(current_object)->sprite_frame = 0x9800u;
	OBJECT(current_object)->sprite_palette = 0x1030u;
	OBJECT(current_object)->screen_x = 0xE9;
	OBJECT(current_object)->screen_y = 0x18;
	OBJECT(current_object)->sprite_layer = 0u;
	OBJECT(current_object)->sprite_scale = 0x100u;
	OBJECT(current_object)->sprite_flags = 0u;
	queue_sprite_in_layer();
}

void ending_credits_team_logo_scroll(void)
{
	uint16_t speed_y = 0;
	uint32_t field_cursor = 0, ignored = 0, object = 0;

	if (OBJECT(current_object)->ending.started == 0u)
	{
		OBJECT(current_object)->ending.started = 1u;
		OBJECT(current_object)->ending.timer = 0;
		OBJECT(current_object)->ending.step = 0u;
		OBJECT(current_object)->screen_y = 0xF4;
		OBJECT(current_object)->sprite_frame = 0x9850u;
		OBJECT(current_object)->sprite_palette = 0x1010u;
		init_credits_sprite();
		OBJECT(current_object)->speed_x = 0x1CC3D;
		for (;;)
		{
			field_cursor = OBJECT(object_cursor)->speed_x;
			if (mem_read16(field_cursor) == 0xFFFFu)
				break;
			allocate_object_2(&ignored, &ignored, &object);
			OBJECT(object)->sprite_frame = mem_read16(field_cursor);
			field_cursor += 2u;
			OBJECT(object)->screen_y = mem_read16(field_cursor);
			field_cursor += 2u;
			OBJECT(object)->sprite_palette = mem_read16(field_cursor);
			field_cursor += 2u;
			current_object = (current_object & ~0xFFFFu) | (uint16_t)(current_object + mem_read16(field_cursor));
			OBJECT(object_cursor)->speed_x = field_cursor + 2u;
			AS32(OBJECT(current_object)->draw_next) = object;
			current_object = object;
			init_credits_sprite();
		}
	}
	if (OBJECT(current_object)->ending.step != 0u)
	{
		speed_y = (uint16_t)OBJECT(object_cursor)->speed_y;
		free_object_2(speed_y);
		delete_current_object();
		return;
	}
	OBJECT(object_cursor)->ending.timer++;
	scroll_credits_sprite();
	current_object = OBJECT(current_object)->speed_y;
	scroll_credits_sprite();
	wrap_credits_scroll_timer();
	current_object = OBJECT(current_object)->speed_y;
	if (OBJECT(current_object)->screen_y <= -0x14)
	{
		OBJECT(object_cursor)->ending.step = 1u;
		return;
	}
}

void ending_credits_director_scroll(void)
{
	uint32_t field_cursor = 0, ignored = 0, object = 0;

	if (OBJECT(current_object)->ending.started != 0u)
	{
		scroll_credits_block();
		return;
	}
	OBJECT(current_object)->ending.started = 1u;
	OBJECT(current_object)->ending.timer = 0;
	OBJECT(current_object)->ending.step = 0u;
	OBJECT(current_object)->screen_y = 0xF4;
	OBJECT(current_object)->sprite_frame = 0x97C8u;
	OBJECT(current_object)->sprite_palette = 0x1010u;
	init_credits_sprite();
	OBJECT(current_object)->speed_x = 0x1CD01;
	for (;;)
	{
		field_cursor = OBJECT(object_cursor)->speed_x;
		if (mem_read16(field_cursor) == 0xFFFFu)
			break;
		allocate_object_2(&ignored, &ignored, &object);
		OBJECT(object)->sprite_frame = mem_read16(field_cursor);
		field_cursor += 2u;
		OBJECT(object)->screen_y = mem_read16(field_cursor);
		field_cursor += 2u;
		OBJECT(object)->sprite_palette = mem_read16(field_cursor);
		field_cursor += 2u;
		current_object = (current_object & ~0xFFFFu) | (uint16_t)(current_object + mem_read16(field_cursor));
		OBJECT(object_cursor)->speed_x = field_cursor + 2u;
		AS32(OBJECT(current_object)->draw_next) = object;
		current_object = object;
		init_credits_sprite();
	}
	scroll_credits_block();
}

void scroll_credits_block(void)
{
	uint16_t speed_y = 0, z = 0;

	if (OBJECT(current_object)->scroll_credits.scrolled_out != 0u)
	{
		speed_y = (uint16_t)OBJECT(object_cursor)->speed_y;
		free_object_2(speed_y);
		z = (uint16_t)OBJECT(object_cursor)->z;
		free_object_2(z);
		delete_current_object();
		return;
	}
	OBJECT(object_cursor)->scroll_credits.timer++;
	scroll_credits_sprite();
	current_object = OBJECT(current_object)->speed_y;
	scroll_credits_sprite();
	current_object = OBJECT(current_object)->z;
	scroll_credits_sprite();
	wrap_credits_scroll_timer();
	current_object = OBJECT(current_object)->z;
	if (OBJECT(current_object)->screen_y <= -0x14)
	{
		OBJECT(object_cursor)->scroll_credits.scrolled_out = 1u;
		return;
	}
}

void ending_credits_program_scroll(void)
{
	uint16_t animation = 0, part_3 = 0, speed_y = 0, z = 0;
	uint32_t field_cursor = 0, ignored = 0, object = 0;

	if (OBJECT(current_object)->ending.started != 0u)
	{
		if (OBJECT(current_object)->ending.step == 0u)
		{
			OBJECT(object_cursor)->ending.timer++;
			scroll_credits_sprite();
			current_object = OBJECT(current_object)->speed_y;
			scroll_credits_sprite();
			current_object = OBJECT(current_object)->z;
			scroll_credits_sprite();
			current_object = OBJECT(current_object)->ending.part_3;
			scroll_credits_sprite();
			current_object = OBJECT(current_object)->animation;
			scroll_credits_sprite();
			wrap_credits_scroll_timer();
			current_object = OBJECT(current_object)->animation;
			if (OBJECT(current_object)->screen_y <= -0x14)
			{
				OBJECT(object_cursor)->ending.step = 1u;
				return;
			}
			return;
		}
		speed_y = (uint16_t)OBJECT(object_cursor)->speed_y;
		free_object_2(speed_y);
		z = (uint16_t)OBJECT(object_cursor)->z;
		free_object_2(z);
		part_3 = (uint16_t)OBJECT(object_cursor)->ending.part_3;
		free_object_2(part_3);
		animation = (uint16_t)OBJECT(object_cursor)->animation;
		free_object_2(animation);
		delete_current_object();
		return;
	}
	OBJECT(current_object)->ending.started = 1u;
	OBJECT(current_object)->ending.step = 0u;
	OBJECT(current_object)->ending.timer = 0;
	OBJECT(current_object)->screen_y = 0xF4;
	OBJECT(current_object)->sprite_frame = 0x97C0u;
	OBJECT(current_object)->sprite_palette = 0x1010u;
	init_credits_sprite();
	OBJECT(current_object)->speed_x = 0x1CDEF;
	for (;;)
	{
		field_cursor = OBJECT(object_cursor)->speed_x;
		if (mem_read16(field_cursor) == 0xFFFFu)
			break;
		allocate_object_2(&ignored, &ignored, &object);
		OBJECT(object)->sprite_frame = mem_read16(field_cursor);
		field_cursor += 2u;
		OBJECT(object)->screen_y = mem_read16(field_cursor);
		field_cursor += 2u;
		OBJECT(object)->sprite_palette = mem_read16(field_cursor);
		field_cursor += 2u;
		current_object = (current_object & ~0xFFFFu) | (uint16_t)(current_object + mem_read16(field_cursor));
		OBJECT(object_cursor)->speed_x = field_cursor + 2u;
		AS32(OBJECT(current_object)->draw_next) = object;
		current_object = object;
		init_credits_sprite();
	}
	scroll_credits_block();
}

void ending_credits_design_scroll(void)
{
	uint16_t animation = 0, part_3 = 0, speed_y = 0, x = 0, z = 0;
	uint32_t field_cursor = 0, ignored = 0, object = 0;

	if (OBJECT(current_object)->ending.started != 0u)
	{
		if (OBJECT(current_object)->ending.step == 0u)
		{
			OBJECT(object_cursor)->ending.timer++;
			scroll_credits_sprite();
			current_object = OBJECT(current_object)->speed_y;
			scroll_credits_sprite();
			current_object = OBJECT(current_object)->z;
			scroll_credits_sprite();
			current_object = OBJECT(current_object)->ending.part_3;
			scroll_credits_sprite();
			current_object = OBJECT(current_object)->animation;
			scroll_credits_sprite();
			current_object = OBJECT(current_object)->x;
			scroll_credits_sprite();
			wrap_credits_scroll_timer();
			current_object = OBJECT(current_object)->x;
			if (OBJECT(current_object)->screen_y <= -0x14)
			{
				OBJECT(object_cursor)->ending.step = 1u;
				return;
			}
			return;
		}
		speed_y = (uint16_t)OBJECT(object_cursor)->speed_y;
		free_object_2(speed_y);
		z = (uint16_t)OBJECT(object_cursor)->z;
		free_object_2(z);
		part_3 = (uint16_t)OBJECT(object_cursor)->ending.part_3;
		free_object_2(part_3);
		animation = (uint16_t)OBJECT(object_cursor)->animation;
		free_object_2(animation);
		x = (uint16_t)OBJECT(object_cursor)->x;
		free_object_2(x);
		delete_current_object();
		return;
	}
	OBJECT(current_object)->ending.started = 1u;
	OBJECT(current_object)->ending.step = 0u;
	OBJECT(current_object)->ending.timer = 0;
	OBJECT(current_object)->screen_y = 0xF4;
	OBJECT(current_object)->sprite_frame = 0x97E8u;
	OBJECT(current_object)->sprite_palette = 0x1010u;
	init_credits_sprite();
	OBJECT(current_object)->speed_x = 0x1CEFE;
	for (;;)
	{
		field_cursor = OBJECT(object_cursor)->speed_x;
		if (mem_read16(field_cursor) == 0xFFFFu)
			break;
		allocate_object_2(&ignored, &ignored, &object);
		OBJECT(object)->sprite_frame = mem_read16(field_cursor);
		field_cursor += 2u;
		OBJECT(object)->screen_y = mem_read16(field_cursor);
		field_cursor += 2u;
		OBJECT(object)->sprite_palette = mem_read16(field_cursor);
		field_cursor += 2u;
		current_object = (current_object & ~0xFFFFu) | (uint16_t)(current_object + mem_read16(field_cursor));
		OBJECT(object_cursor)->speed_x = field_cursor + 2u;
		AS32(OBJECT(current_object)->draw_next) = object;
		current_object = object;
		init_credits_sprite();
	}
	scroll_credits_block();
}

void ending_credits_sound_scroll(void)
{
	uint16_t animation = 0, part_3 = 0, speed_y = 0, z = 0;
	uint32_t field_cursor = 0, ignored = 0, object = 0;

	if (OBJECT(current_object)->ending.started != 0u)
	{
		if (OBJECT(current_object)->ending.step == 0u)
		{
			OBJECT(object_cursor)->ending.timer++;
			scroll_credits_sprite();
			current_object = OBJECT(current_object)->speed_y;
			scroll_credits_sprite();
			current_object = OBJECT(current_object)->z;
			scroll_credits_sprite();
			current_object = OBJECT(current_object)->ending.part_3;
			scroll_credits_sprite();
			current_object = OBJECT(current_object)->animation;
			scroll_credits_sprite();
			wrap_credits_scroll_timer();
			current_object = OBJECT(current_object)->animation;
			if (OBJECT(current_object)->screen_y <= -0x14)
			{
				OBJECT(object_cursor)->ending.step = 1u;
				return;
			}
			return;
		}
		speed_y = (uint16_t)OBJECT(object_cursor)->speed_y;
		free_object_2(speed_y);
		z = (uint16_t)OBJECT(object_cursor)->z;
		free_object_2(z);
		part_3 = (uint16_t)OBJECT(object_cursor)->ending.part_3;
		free_object_2(part_3);
		animation = (uint16_t)OBJECT(object_cursor)->animation;
		free_object_2(animation);
		delete_current_object();
		return;
	}
	OBJECT(current_object)->ending.started = 1u;
	OBJECT(current_object)->ending.step = 0u;
	OBJECT(current_object)->ending.timer = 0;
	OBJECT(current_object)->screen_y = 0xF4;
	OBJECT(current_object)->sprite_frame = 0x97E0u;
	OBJECT(current_object)->sprite_palette = 0x1010u;
	init_credits_sprite();
	OBJECT(current_object)->speed_x = 0x1D004;
	for (;;)
	{
		field_cursor = OBJECT(object_cursor)->speed_x;
		if (mem_read16(field_cursor) == 0xFFFFu)
			break;
		allocate_object_2(&ignored, &ignored, &object);
		OBJECT(object)->sprite_frame = mem_read16(field_cursor);
		field_cursor += 2u;
		OBJECT(object)->screen_y = mem_read16(field_cursor);
		field_cursor += 2u;
		OBJECT(object)->sprite_palette = mem_read16(field_cursor);
		field_cursor += 2u;
		current_object = (current_object & ~0xFFFFu) | (uint16_t)(current_object + mem_read16(field_cursor));
		OBJECT(object_cursor)->speed_x = field_cursor + 2u;
		AS32(OBJECT(current_object)->draw_next) = object;
		current_object = object;
		init_credits_sprite();
	}
	scroll_credits_block();
}

void ending_credits_voice_actor_scroll(void)
{
	uint16_t animation = 0, part_3 = 0, speed_y = 0, x = 0, z = 0;
	uint32_t field_cursor = 0, ignored = 0, object = 0;

	if (OBJECT(current_object)->ending.started != 0u)
	{
		if (OBJECT(current_object)->ending.step == 0u)
		{
			OBJECT(object_cursor)->ending.timer++;
			scroll_credits_sprite();
			current_object = OBJECT(current_object)->speed_y;
			scroll_credits_sprite();
			current_object = OBJECT(current_object)->z;
			scroll_credits_sprite();
			current_object = OBJECT(current_object)->ending.part_3;
			scroll_credits_sprite();
			current_object = OBJECT(current_object)->animation;
			scroll_credits_sprite();
			current_object = OBJECT(current_object)->x;
			scroll_credits_sprite();
			wrap_credits_scroll_timer();
			current_object = OBJECT(current_object)->x;
			if (OBJECT(current_object)->screen_y <= -0x14)
			{
				OBJECT(object_cursor)->ending.step = 1u;
				return;
			}
			return;
		}
		speed_y = (uint16_t)OBJECT(object_cursor)->speed_y;
		free_object_2(speed_y);
		z = (uint16_t)OBJECT(object_cursor)->z;
		free_object_2(z);
		part_3 = (uint16_t)OBJECT(object_cursor)->ending.part_3;
		free_object_2(part_3);
		animation = (uint16_t)OBJECT(object_cursor)->animation;
		free_object_2(animation);
		x = (uint16_t)OBJECT(object_cursor)->x;
		free_object_2(x);
		delete_current_object();
		return;
	}
	OBJECT(current_object)->ending.started = 1u;
	OBJECT(current_object)->ending.step = 0u;
	OBJECT(current_object)->ending.timer = 0;
	OBJECT(current_object)->screen_y = 0xF4;
	OBJECT(current_object)->sprite_frame = 0x97F0u;
	OBJECT(current_object)->sprite_palette = 0x1010u;
	init_credits_sprite();
	OBJECT(current_object)->speed_x = 0x1D113;
	for (;;)
	{
		field_cursor = OBJECT(object_cursor)->speed_x;
		if (mem_read16(field_cursor) == 0xFFFFu)
			break;
		allocate_object_2(&ignored, &ignored, &object);
		OBJECT(object)->sprite_frame = mem_read16(field_cursor);
		field_cursor += 2u;
		OBJECT(object)->screen_y = mem_read16(field_cursor);
		field_cursor += 2u;
		OBJECT(object)->sprite_palette = mem_read16(field_cursor);
		field_cursor += 2u;
		current_object = (current_object & ~0xFFFFu) | (uint16_t)(current_object + mem_read16(field_cursor));
		OBJECT(object_cursor)->speed_x = field_cursor + 2u;
		AS32(OBJECT(current_object)->draw_next) = object;
		current_object = object;
		init_credits_sprite();
	}
	scroll_credits_block();
}

void ending_credits_special_thanks_scroll(void)
{
	uint16_t animation = 0, floor_z = 0, part_3 = 0, previous_floor_z = 0, speed_y = 0, x = 0, y = 0, z = 0;
	uint32_t field_cursor = 0, ignored = 0, object = 0;

	if (OBJECT(current_object)->ending.started != 0u)
	{
		if (OBJECT(current_object)->ending.step == 0u)
		{
			OBJECT(object_cursor)->ending.timer++;
			scroll_credits_sprite();
			current_object = OBJECT(current_object)->speed_y;
			scroll_credits_sprite();
			current_object = OBJECT(current_object)->z;
			scroll_credits_sprite();
			current_object = OBJECT(current_object)->ending.part_3;
			scroll_credits_sprite();
			current_object = OBJECT(current_object)->animation;
			scroll_credits_sprite();
			current_object = OBJECT(current_object)->x;
			scroll_credits_sprite();
			current_object = OBJECT(current_object)->y;
			scroll_credits_sprite();
			current_object = OBJECT(current_object)->floor_z;
			scroll_credits_sprite();
			current_object = OBJECT(current_object)->previous_floor_z;
			scroll_credits_sprite();
			wrap_credits_scroll_timer();
			current_object = OBJECT(current_object)->previous_floor_z;
			if (OBJECT(current_object)->screen_y <= -0x14)
			{
				OBJECT(object_cursor)->ending.step = 1u;
				return;
			}
			return;
		}
		speed_y = (uint16_t)OBJECT(object_cursor)->speed_y;
		free_object_2(speed_y);
		z = (uint16_t)OBJECT(object_cursor)->z;
		free_object_2(z);
		part_3 = (uint16_t)OBJECT(object_cursor)->ending.part_3;
		free_object_2(part_3);
		animation = (uint16_t)OBJECT(object_cursor)->animation;
		free_object_2(animation);
		x = (uint16_t)OBJECT(object_cursor)->x;
		free_object_2(x);
		y = (uint16_t)OBJECT(object_cursor)->y;
		free_object_2(y);
		floor_z = (uint16_t)OBJECT(object_cursor)->floor_z;
		free_object_2(floor_z);
		previous_floor_z = (uint16_t)OBJECT(object_cursor)->previous_floor_z;
		free_object_2(previous_floor_z);
		delete_current_object();
		return;
	}
	OBJECT(current_object)->ending.started = 1u;
	OBJECT(current_object)->ending.step = 0u;
	OBJECT(current_object)->ending.timer = 0;
	OBJECT(current_object)->screen_y = 0xF4;
	OBJECT(current_object)->sprite_frame = 0x97D0u;
	OBJECT(current_object)->sprite_palette = 0x1010u;
	init_credits_sprite();
	OBJECT(current_object)->speed_x = 0x1D25D;
	for (;;)
	{
		field_cursor = OBJECT(object_cursor)->speed_x;
		if (mem_read16(field_cursor) == 0xFFFFu)
			break;
		allocate_object_2(&ignored, &ignored, &object);
		OBJECT(object)->sprite_frame = mem_read16(field_cursor);
		field_cursor += 2u;
		OBJECT(object)->screen_y = mem_read16(field_cursor);
		field_cursor += 2u;
		OBJECT(object)->sprite_palette = mem_read16(field_cursor);
		field_cursor += 2u;
		current_object = (current_object & ~0xFFFFu) | (uint16_t)(current_object + mem_read16(field_cursor));
		OBJECT(object_cursor)->speed_x = field_cursor + 2u;
		AS32(OBJECT(current_object)->draw_next) = object;
		current_object = object;
		init_credits_sprite();
	}
	scroll_credits_block();
}

void scroll_credits_sprite(void)
{
	if (OBJECT(object_cursor)->scroll_credits.timer >= 2)
		OBJECT(current_object)->screen_y--;
	queue_sprite_in_layer();
	current_object = object_cursor;
}

void wrap_credits_scroll_timer(void)
{
	if ((int16_t)AT16(OBJECT(object_cursor)->flags, 2) >= 2)
		AT16(OBJECT(object_cursor)->flags, 2) = 0u;
}

void init_credits_sprite(void)
{
	OBJECT(current_object)->screen_x = 0x58;
	OBJECT(current_object)->sprite_layer = 0u;
	OBJECT(current_object)->sprite_flags = 0u;
	OBJECT(current_object)->sprite_scale = 0x100u;
	current_object = object_cursor;
}

void run_timed_spawns_3(void)
{
	uint32_t ignored = 0, object = 0, spawn_record = 0;

	spawn_record = timed_spawns_3_ADDRESS + OBJECT(current_object)->run_timed_spawns.list_index * 4u;
	OBJECT(current_object)->z = ROM16(spawn_record + 2u);
	if (OBJECT(current_object)->z_fraction == 0xFFFFu)
	{
		delete_current_object();
		return;
	}
	if ((int16_t)ROM16(spawn_record) <= OBJECT(current_object)->run_timed_spawns.timer)
	{
		allocate_object_outside_loop(&ignored, &object);
		OBJECT(current_object)->z += 0x1C82Fu;
		OBJECT(object)->handler = OBJECT(current_object)->z;
		AS8(OBJECT(object)->flags) = 0u;
		OBJECT(current_object)->run_timed_spawns.list_index++;
	}
}

void animate_ending_runner(uint32_t frame_entry)
{
	if (ending_first_scene_done == 1u)
	{
		delete_current_object();
		return;
	}
	OBJECT(current_object)->sprite_frame = mem_read16(frame_entry);
	OBJECT(current_object)->sprite_flags = 0x100u;
	queue_sprite_in_layer();
	OBJECT(current_object)->animate_ending.timer++;
	if ((OBJECT(current_object)->animate_ending.timer & 3u) == 2u)
		OBJECT(current_object)->sprite_scale++;
	if ((OBJECT(current_object)->animate_ending.timer & 1u) != 0u)
	{
		OBJECT(current_object)->animate_ending.frame_index++;
		if (OBJECT(current_object)->animate_ending.frame_index > 0xB)
			OBJECT(current_object)->animate_ending.frame_index = 0;
	}
}

void ending_window_open(void)
{
	uint32_t ignored = 0, object = 0;

	if (OBJECT(current_object)->ending.started == 0u)
	{
		OBJECT(current_object)->ending.started = 1u;
		OBJECT(current_object)->ending.step = 0u;
	}
	nbg0_scroll_x_int++;
	mem_write16(video_clip_1_left, mem_read16(video_clip_1_left) - 1u);
	mem_write16(video_clip_1_right, mem_read16(video_clip_1_right) + 1u);
	mem_write16(video_clip_1_top, mem_read16(video_clip_1_top) - 1u);
	mem_write16(video_clip_1_bottom, mem_read16(video_clip_1_bottom) + 1u);
	if ((int16_t)mem_read16(video_clip_1_left) <= 0)
	{
		mem_write16(video_clip_1_left, 0u);
		OBJECT(current_object)->ending.step |= 1u;
	}
	if ((int16_t)mem_read16(video_clip_1_top) <= 0)
	{
		mem_write16(video_clip_1_top, 0u);
		OBJECT(current_object)->ending.step |= 2u;
	}
	if ((int16_t)mem_read16(video_clip_1_right) >= 0x140)
	{
		mem_write16(video_clip_1_right, 0x140u);
		OBJECT(current_object)->ending.step |= 4u;
	}
	if ((int16_t)mem_read16(video_clip_1_bottom) >= 0xE0)
	{
		mem_write16(video_clip_1_bottom, 0xE0u);
		OBJECT(current_object)->ending.step |= 8u;
	}
	if (OBJECT(current_object)->ending.step != 0xFu)
		return;
	allocate_object_outside_loop(&ignored, &object);
	OBJECT(object)->ending.started = 0u;
	OBJECT(object)->handler = ending_credits_fade_out_ADDRESS;
	delete_current_object();
}

void ending_credits_fade_out(void)
{
	uint32_t ignored = 0, object = 0;

	if (OBJECT(current_object)->ending.started == 0u)
	{
		fade_step = 0u;
		fade_kind = 0;
		OBJECT(current_object)->ending.started = 1u;
	}
	update_fade(0u);
	color_offset_2_red = color_offset_red;
	color_offset_2_blue = color_offset_blue;
	color_offset_2_green = color_offset_green;
	if (fade_kind >= 0)
		return;
	allocate_object_outside_loop(&ignored, &object);
	OBJECT(object)->ending.started = 0u;
	OBJECT(object)->handler = ending_eggman_scene_fade_in_ADDRESS;
	delete_current_object();
}

void ending_eggman_scene_fade_in(void)
{
	uint32_t r1 = 0, ignored = 0, object = 0, script = 0;

	if (OBJECT(current_object)->ending.started != 0u)
	{
		update_fade(0u);
		color_offset_2_red = color_offset_red;
		color_offset_2_blue = color_offset_blue;
		color_offset_2_green = color_offset_green;
		if (fade_kind >= 0)
			return;
		allocate_object_outside_loop(&ignored, &object);
		OBJECT(object)->ending.started = 0u;
		OBJECT(object)->handler = ending_eggman_fly_in_ADDRESS;
		delete_current_object();
		return;
	}
	mem_write16(video_layer_disable, mem_read16(video_layer_disable) | 0xBu);
	script = ending_stripes_script_ADDRESS;
	run_vram_script(&r1, &script, &object);
	nbg0_pages = 0x3030303u;
	nbg1_pages = 0x2020202u;
	nbg2_pages = 0x8080707u;
	nbg3_pages = 0xF0F0F0Fu;
	mem_write16(video_layer_disable, mem_read16(video_layer_disable) & ~0xFu);
	OBJECT(current_object)->ending.started = 1u;
	fade_step = 0u;
	fade_kind = 2;
	nbg0_scroll_x_int = 0u;
}

void ending_eggman_fly_in(void)
{
	uint8_t sound_id = 0;
	uint16_t animation = 0, floor_block = 0, floor_z = 0, part_3 = 0, previous_floor_z = 0, speed_x = 0, speed_y = 0, x = 0, y = 0, z = 0;
	uint32_t animation_2 = 0, animation_3 = 0, floor_block_2 = 0, floor_z_2 = 0, floor_z_3 = 0, ignored = 0, object = 0, part_3_2 = 0, part_3_3 = 0, previous_floor_z_2 = 0, previous_floor_z_3 = 0, x_2 = 0, x_3 = 0, y_2 = 0, y_3 = 0, y_entry = 0, z_2 = 0, z_3 = 0;

	if (OBJECT(current_object)->ending.started == 0u)
	{
		OBJECT(current_object)->ending.started = 1u;
		OBJECT(current_object)->ending.step = 0u;
		OBJECT(current_object)->screen_y = 0x70;
		OBJECT(current_object)->screen_x = -0x20;
		OBJECT(current_object)->speed_y = 0x103B76;
		OBJECT(current_object)->ending.timer = 0x1E0;
		allocate_object_2(&ignored, &ignored, &object);
		OBJECT(current_object)->speed_x = object;
		allocate_object_2(&ignored, &ignored, &object);
		OBJECT(current_object)->z = object;
		init_eggman_smoke_puff(object);
		allocate_object_2(&ignored, &ignored, &object);
		OBJECT(current_object)->ending.part_3 = object;
		init_eggman_smoke_puff(object);
		allocate_object_2(&ignored, &ignored, &object);
		OBJECT(current_object)->animation = object;
		init_eggman_smoke_puff(object);
		allocate_object_2(&ignored, &ignored, &object);
		OBJECT(current_object)->x = object;
		init_eggman_smoke_puff(object);
		allocate_object_2(&ignored, &ignored, &object);
		OBJECT(current_object)->y = object;
		init_eggman_smoke_puff(object);
		OBJECT(object)->sprite_frame = 0x9B80u;
		allocate_object_2(&ignored, &ignored, &object);
		OBJECT(current_object)->floor_z = object;
		init_eggman_smoke_puff(object);
		OBJECT(object)->sprite_frame = 0x9B78u;
		allocate_object_2(&ignored, &ignored, &object);
		OBJECT(current_object)->previous_floor_z = object;
		init_eggman_smoke_puff(object);
		OBJECT(object)->sprite_frame = 0x9B70u;
		allocate_object_2(&ignored, &ignored, &object);
		OBJECT(current_object)->floor_block = object;
		init_eggman_smoke_puff(object);
		OBJECT(object)->sprite_frame = 0x9B68u;
	}
	OBJECT(current_object)->ending.timer--;
	if ((uint16_t)OBJECT(current_object)->ending.timer == 0x104u)
	{
		sound_id = 0x75u;
		queue_sound(sound_id, &ignored, &ignored);
	}
	if ((uint16_t)OBJECT(current_object)->ending.timer != 0u)
	{
		y_entry = OBJECT(current_object)->speed_y;
		OBJECT(current_object)->screen_x = mem_read16(y_entry);
		y_entry += 2u;
		OBJECT(current_object)->screen_y = mem_read16(y_entry);
		OBJECT(current_object)->sprite_layer = 0u;
		OBJECT(current_object)->sprite_frame = 0x9AE0u;
		OBJECT(current_object)->sprite_palette = 0x1250u;
		OBJECT(current_object)->sprite_scale = 0x100u;
		OBJECT(current_object)->sprite_flags = 0u;
		OBJECT(current_object)->speed_y = y_entry + 2u;
		queue_sprite_in_layer();
		if ((uint16_t)OBJECT(current_object)->screen_x != 0xA6u)
		{
			current_object = OBJECT(current_object)->speed_x;
			OBJECT(current_object)->sprite_frame = 0x9AB0u;
			if ((OBJECT(object_cursor)->ending.timer & 2u) != 0u)
				OBJECT(current_object)->sprite_frame = 0x9AB8u;
			OBJECT(current_object)->screen_x = OBJECT(object_cursor)->screen_x;
			OBJECT(current_object)->screen_y = OBJECT(object_cursor)->screen_y;
			OBJECT(current_object)->sprite_layer = 0u;
			OBJECT(current_object)->sprite_scale = 0x100u;
			OBJECT(current_object)->screen_x -= 0x20u;
			OBJECT(current_object)->sprite_flags = 0u;
			OBJECT(current_object)->sprite_palette = 0x10E0u;
			queue_sprite_in_layer();
		}
		else
			OBJECT(current_object)->speed_y += 4u;
		current_object = object_cursor;
		if ((uint16_t)((int16_t)OBJECT(current_object)->ending.timer % 0xA) == 0u)
		{
			previous_floor_z_3 = OBJECT(object_cursor)->previous_floor_z;
			floor_block_2 = OBJECT(object_cursor)->floor_block;
			mem_write16(floor_block_2 + 0xEu, mem_read16(previous_floor_z_3 + 0xEu));
			mem_write16(floor_block_2 + 0xCu, mem_read16(previous_floor_z_3 + 0xCu));
			floor_z_3 = OBJECT(object_cursor)->floor_z;
			previous_floor_z_2 = OBJECT(object_cursor)->previous_floor_z;
			mem_write16(previous_floor_z_2 + 0xEu, mem_read16(floor_z_3 + 0xEu));
			mem_write16(previous_floor_z_2 + 0xCu, mem_read16(floor_z_3 + 0xCu));
			y_3 = OBJECT(object_cursor)->y;
			floor_z_2 = OBJECT(object_cursor)->floor_z;
			mem_write16(floor_z_2 + 0xEu, mem_read16(y_3 + 0xEu));
			mem_write16(floor_z_2 + 0xCu, mem_read16(y_3 + 0xCu));
			x_3 = OBJECT(object_cursor)->x;
			y_2 = OBJECT(object_cursor)->y;
			mem_write16(y_2 + 0xEu, mem_read16(x_3 + 0xEu));
			mem_write16(y_2 + 0xCu, mem_read16(x_3 + 0xCu));
			animation_3 = OBJECT(object_cursor)->animation;
			x_2 = OBJECT(object_cursor)->x;
			mem_write16(x_2 + 0xEu, mem_read16(animation_3 + 0xEu));
			mem_write16(x_2 + 0xCu, mem_read16(animation_3 + 0xCu));
			part_3_3 = OBJECT(object_cursor)->ending.part_3;
			animation_2 = OBJECT(object_cursor)->animation;
			mem_write16(animation_2 + 0xEu, mem_read16(part_3_3 + 0xEu));
			mem_write16(animation_2 + 0xCu, mem_read16(part_3_3 + 0xCu));
			z_3 = OBJECT(object_cursor)->z;
			part_3_2 = OBJECT(object_cursor)->ending.part_3;
			mem_write16(part_3_2 + 0xEu, mem_read16(z_3 + 0xEu));
			mem_write16(part_3_2 + 0xCu, mem_read16(z_3 + 0xCu));
			z_2 = OBJECT(object_cursor)->z;
			mem_write16(z_2 + 0xEu, OBJECT(object_cursor)->screen_x);
			mem_write16(z_2 + 0xCu, OBJECT(object_cursor)->screen_y);
			mem_write16(z_2 + 0xEu, mem_read16(z_2 + 0xEu) - 0x34u);
		}
		if (OBJECT(object_cursor)->ending.timer > 0x32)
		{
			current_object = OBJECT(object_cursor)->z;
			queue_sprite_in_layer();
			current_object = OBJECT(object_cursor)->ending.part_3;
			queue_sprite_in_layer();
			current_object = OBJECT(object_cursor)->animation;
			queue_sprite_in_layer();
			current_object = OBJECT(object_cursor)->x;
			queue_sprite_in_layer();
			current_object = OBJECT(object_cursor)->y;
			queue_sprite_in_layer();
			current_object = OBJECT(object_cursor)->floor_z;
			queue_sprite_in_layer();
			current_object = OBJECT(object_cursor)->previous_floor_z;
			queue_sprite_in_layer();
			current_object = OBJECT(object_cursor)->floor_block;
			queue_sprite_in_layer();
		}
		return;
	}
	allocate_object_outside_loop(&ignored, &object);
	OBJECT(object)->ending.started = 0u;
	OBJECT(object)->handler = ending_eggman_turn_ADDRESS;
	OBJECT(object)->sprite_frame = OBJECT(current_object)->sprite_frame;
	OBJECT(object)->screen_x = OBJECT(current_object)->screen_x;
	OBJECT(object)->screen_y = OBJECT(current_object)->screen_y;
	OBJECT(object)->sprite_layer = OBJECT(current_object)->sprite_layer;
	OBJECT(object)->sprite_flags = OBJECT(current_object)->sprite_flags;
	OBJECT(object)->sprite_palette = OBJECT(current_object)->sprite_palette;
	OBJECT(object)->sprite_scale = OBJECT(current_object)->sprite_scale;
	speed_x = (uint16_t)OBJECT(current_object)->speed_x;
	free_object_2(speed_x);
	speed_y = (uint16_t)OBJECT(current_object)->speed_y;
	free_object_2(speed_y);
	z = (uint16_t)OBJECT(current_object)->z;
	free_object_2(z);
	part_3 = (uint16_t)OBJECT(current_object)->ending.part_3;
	free_object_2(part_3);
	animation = (uint16_t)OBJECT(current_object)->animation;
	free_object_2(animation);
	x = (uint16_t)OBJECT(current_object)->x;
	free_object_2(x);
	y = (uint16_t)OBJECT(current_object)->y;
	free_object_2(y);
	floor_z = (uint16_t)OBJECT(current_object)->floor_z;
	free_object_2(floor_z);
	previous_floor_z = (uint16_t)OBJECT(current_object)->previous_floor_z;
	free_object_2(previous_floor_z);
	floor_block = (uint16_t)OBJECT(current_object)->floor_block;
	free_object_2(floor_block);
	delete_current_object();
}

void init_eggman_smoke_puff(uint32_t piece)
{
	OBJECT(piece)->sprite_frame = 0x9B88u;
	OBJECT(piece)->sprite_layer = 0u;
	OBJECT(piece)->screen_x = -0x10;
	OBJECT(piece)->screen_y = 0;
	OBJECT(piece)->sprite_scale = 0x100u;
	OBJECT(piece)->sprite_flags = 0u;
	OBJECT(piece)->sprite_palette = 0x1280u;
}

void ending_eggman_turn(void)
{
	uint32_t frame_record = 0, ignored = 0, object = 0;

	if (OBJECT(current_object)->ending.started == 0u)
	{
		OBJECT(current_object)->ending.started = 1u;
		OBJECT(current_object)->ending.timer = 0;
		OBJECT(current_object)->ending.step = 0u;
	}
	OBJECT(current_object)->ending.timer++;
	frame_record = ending_eggman_turn_frames_ADDRESS + OBJECT(current_object)->ending.step * 4u;
	if (ROM16(frame_record) == 0xFFFFu)
	{
		allocate_object_outside_loop(&ignored, &object);
		OBJECT(object)->ending.started = 0u;
		OBJECT(object)->handler = ending_eggman_scene_fade_out_ADDRESS;
		delete_current_object();
		return;
	}
	if ((int16_t)ROM16(frame_record) < OBJECT(current_object)->ending.timer)
		OBJECT(current_object)->ending.step++;
	OBJECT(current_object)->sprite_frame = ROM16(frame_record + 2u);
	queue_sprite_in_layer();
}

void ending_eggman_scene_fade_out(void)
{
	uint32_t ignored = 0, object = 0;

	if (OBJECT(current_object)->ending.started == 0u)
	{
		fade_step = 0u;
		fade_kind = 0;
		OBJECT(current_object)->ending.started = 1u;
	}
	update_fade(0u);
	color_offset_2_red = color_offset_red;
	color_offset_2_blue = color_offset_blue;
	color_offset_2_green = color_offset_green;
	if (fade_kind >= 0)
		return;
	allocate_object_outside_loop(&ignored, &object);
	OBJECT(object)->ending.started = 0u;
	OBJECT(object)->handler = ending_close_up_fade_in_ADDRESS;
	allocate_object_outside_loop(&ignored, &object);
	OBJECT(object)->ending.started = 0u;
	OBJECT(object)->handler = ending_eggman_fly_away_ADDRESS;
	delete_current_object();
}

void ending_close_up_fade_in(void)
{
	if (OBJECT(current_object)->ending.started == 0u)
	{
		OBJECT(current_object)->ending.started = 1u;
		fade_step = 0u;
		fade_kind = 2;
	}
	update_fade(0u);
	color_offset_2_red = color_offset_red;
	color_offset_2_blue = color_offset_blue;
	color_offset_2_green = color_offset_green;
	if (fade_kind >= 0)
		return;
	delete_current_object();
}

void ending_eggman_fly_away(void)
{
	uint8_t sound_id = 0;
	uint16_t halfwords = 0, line_dy = 0, scale_step = 0, speed_y = 0, z = 0;
	uint32_t r2 = 0, cycle = 0, half_width_squared = 0, ignored = 0, left_entry = 0, lines_left = 0, object = 0, palette_count = 0, right_entry = 0, source = 0;

	if (OBJECT(current_object)->ending.started == 0u)
	{
		OBJECT(current_object)->ending.started = 1u;
		OBJECT(current_object)->ending.step = 0u;
		mem_write16(video_layer_disable, 0x20u);
		mem_write16(video_layer_disable, mem_read16(video_layer_disable) | 4u);
		mixer_nbg1_blend = 0x580u;
		mixer_blend = 0u;
		OBJECT(current_object)->screen_x = 0xA0;
		OBJECT(current_object)->screen_y = 0x70;
		OBJECT(current_object)->sprite_layer = 1u;
		OBJECT(current_object)->sprite_scale = 0x200u;
		OBJECT(current_object)->sprite_flags = 0u;
		OBJECT(current_object)->sprite_frame = 0x9AD8u;
		OBJECT(current_object)->sprite_palette = 0x1230u;
		allocate_object_2(&ignored, &ignored, &object);
		OBJECT(current_object)->speed_y = object;
		OBJECT(object)->screen_x = 0xA0;
		OBJECT(object)->screen_y = 0x70;
		OBJECT(object)->sprite_layer = 0u;
		OBJECT(object)->sprite_scale = 0x200u;
		OBJECT(object)->sprite_flags = 0u;
		OBJECT(object)->sprite_flags += 8u;
		OBJECT(object)->sprite_frame = 0x9AD0u;
		OBJECT(object)->sprite_palette = 0x1240u;
		allocate_object_2(&ignored, &ignored, &object);
		OBJECT(current_object)->z = object;
		OBJECT(object)->screen_x = 0xA0;
		OBJECT(object)->screen_y = 0x70;
		OBJECT(object)->sprite_layer = 0u;
		OBJECT(object)->sprite_scale = 0u;
		OBJECT(object)->sprite_flags = 0u;
		OBJECT(object)->sprite_frame = 0x9B50u;
		OBJECT(object)->sprite_palette = 0x1270u;
		sound_id = 0x72u;
		queue_sound(sound_id, &ignored, &r2);
	}
	object = PALETTE_RAM + 0x8BC0u;
	palette_count = 1u;
	source = ending_eggman_cycle_palettes_ADDRESS;
	halfwords = 0x10u;
	cycle = palette_cycle_0_ADDRESS;
	palette_cycle_0 = 6u;
	palette_cycle_0_frames = 9u;
	cycle_palette(cycle, halfwords, &ignored, &r2, &palette_count, &source, &object);
	if ((int16_t)OBJECT(current_object)->sprite_scale >= 0x20)
	{
		current_object = object_cursor;
		scale_step = 3u;
		if ((int16_t)OBJECT(current_object)->sprite_scale < 0x100)
			scale_step = 2u;
		OBJECT(current_object)->sprite_scale -= scale_step;
		queue_sprite_in_layer();
		current_object = OBJECT(current_object)->speed_y;
		OBJECT(current_object)->sprite_scale -= scale_step;
		queue_sprite_in_layer();
		return;
	}
	current_object = OBJECT(current_object)->z;
	switch (OBJECT(object_cursor)->ending.step)
	{
	case 0:
	case 7:
		sound_id = 9u;
		queue_sound(sound_id, &ignored, &ignored);
		nbg2_pages = 0x2020202u;
		OBJECT(object_cursor)->ending.step = 1u;
		return;
	case 1:
		OBJECT(current_object)->sprite_scale += 5u;
		queue_sprite_in_layer();
		if ((int16_t)OBJECT(current_object)->sprite_scale >= 0x110)
			OBJECT(object_cursor)->ending.step = 2u;
		return;
	case 2:
		OBJECT(current_object)->sprite_scale -= 5u;
		if ((int16_t)OBJECT(current_object)->sprite_scale >= 0)
		{
			queue_sprite_in_layer();
			return;
		}
		OBJECT(object_cursor)->ending.step = 3u;
		return;
	case 3:
		speed_y = (uint16_t)OBJECT(object_cursor)->speed_y;
		free_object_2(speed_y);
		z = (uint16_t)OBJECT(object_cursor)->z;
		free_object_2(z);
		OBJECT(object_cursor)->ending.step = 4u;
		return;
	case 4:
		mem_write16(video_rowscroll_control, 0x5015u);
		mem_write16(video_clip_select, 0x400u);
		mem_write32(nbg2_page_select, 0x2020202u);
		mixer_nbg2_blend = 0x580u;
		mixer_blend = 0u;
		mem_write16(video_layer_disable, 0x2122u);
		nbg2_scroll_x_fraction = 0u;
		nbg2_scroll_y_fraction = 0u;
		OBJECT(object_cursor)->ending.step = 5u;
		AS8(circle_wipe_radius) = 0u;
		AT16(OBJECT(current_object)->previous_floor_block, 2) = 0u;
		fill_value = (fill_value & ~0xFFFFu) | 1u;
		set_halfwords(VIDEO_RAM + 0xA000u, 0x200u, fill_value);
		fill_value = fill_value & ~0xFFFFu;
		set_halfwords(VIDEO_RAM + 0xA400u, 0x200u, fill_value);
		return;
	case 5:
		AT16(OBJECT(current_object)->previous_floor_block, 2)++;
		if ((OBJECT(current_object)->previous_floor_block & 0x10000u) != 0u)
		{
			left_entry = VIDEO_RAM + 0xA0E0u;
			right_entry = VIDEO_RAM + 0xA4E0u;
			circle_wipe_line = 0x70u;
		}
		else
		{
			circle_wipe_radius += 3u;
			left_entry = VIDEO_RAM + 0xA000u;
			right_entry = VIDEO_RAM + 0xA400u;
			circle_wipe_line = 0u;
		}
		lines_left = 0x70u;
		do
		{
			half_width_squared = (uint16_t)circle_wipe_radius;
			line_dy = (uint16_t)(circle_wipe_line - 0x70u);
			half_width_squared = (uint16_t)(half_width_squared * half_width_squared - (uint32_t)line_dy * line_dy);
			if ((int16_t)half_width_squared >= 0)
			{
				square_root(&half_width_squared, &ignored);
				circle_wipe_left = 0xA0u;
				circle_wipe_right = 0xA0u;
				circle_wipe_right += half_width_squared;
				circle_wipe_left -= half_width_squared;
			}
			else
			{
				if ((OBJECT(current_object)->previous_floor_block & 0x10000u) == 0u)
				{
					if ((int16_t)(0x70u - circle_wipe_radius) <= (int16_t)((int32_t)(left_entry - (VIDEO_RAM + 0xA000u)) >> 1))
						goto ending_eggman_fly_away_whole_line;
					goto ending_eggman_fly_away_empty_line;
				}
				if ((int16_t)((int32_t)(left_entry - (VIDEO_RAM + 0xA000u)) >> 1) <= (int16_t)(circle_wipe_radius + 0x70u))
				{
				ending_eggman_fly_away_whole_line:
					circle_wipe_left = 0u;
					circle_wipe_right = 0x13Fu;
				}
				else
				{
				ending_eggman_fly_away_empty_line:
					circle_wipe_left = 1u;
					circle_wipe_right = 0u;
				}
			}
			if ((int16_t)circle_wipe_left <= 0)
				circle_wipe_left = 0u;
			if ((int16_t)circle_wipe_right >= 0x13F)
				circle_wipe_right = 0x13Fu;
			mem_write16(left_entry, circle_wipe_left);
			mem_write16(right_entry, circle_wipe_right);
			circle_wipe_line++;
			lines_left--;
			left_entry += 2u;
			right_entry += 2u;
		} while (lines_left != 0u);
		if (circle_wipe_radius > 0xE0)
			OBJECT(object_cursor)->ending.step = 6u;
		return;
	case 6:
		allocate_object_outside_loop(&ignored, &object);
		OBJECT(object)->handler = ending_sonic_run_across_ADDRESS;
		OBJECT(object)->ending.started = 0u;
		allocate_object_outside_loop(&ignored, &object);
		OBJECT(object)->handler = ending_ray_run_across_ADDRESS;
		OBJECT(object)->ending.started = 0u;
		allocate_object_outside_loop(&ignored, &object);
		OBJECT(object)->handler = ending_mighty_run_across_ADDRESS;
		OBJECT(object)->ending.started = 0u;
		delete_current_object();
		return;
	default:
		unknown_code(0x1DB45u + OBJECT(object_cursor)->ending.step * 3u);
	}
}

void ending_eggman_fly_away_spare_return(void)
{
}

void ending_sonic_run_across(void)
{
	uint32_t ignored = 0, second_object = 0;

	if (OBJECT(current_object)->ending.started != 0u)
	{
		OBJECT(object_cursor)->hit_list_next++;
		if ((int16_t)OBJECT(object_cursor)->hit_list_next >= 0x78)
			ending_finished = 1u;
		animate_ending_runner_across();
		OBJECT(object_cursor)->screen_x += 5u;
		OBJECT(current_object)->screen_x += 5u;
		return;
	}
	OBJECT(current_object)->sprite_layer = 1u;
	OBJECT(current_object)->sprite_palette = 0x1220u;
	OBJECT(current_object)->sprite_frame = 0xA810u;
	OBJECT(current_object)->sprite_scale = 0x100u;
	init_ending_runner_across();
	allocate_object_2(&ignored, &ignored, &second_object);
	OBJECT(current_object)->speed_y = second_object;
	current_object = second_object;
	OBJECT(current_object)->sprite_frame = 0xA820u;
	OBJECT(current_object)->sprite_scale = 0x100u;
	OBJECT(current_object)->sprite_palette = 0x1220u;
	OBJECT(current_object)->sprite_layer = 0u;
	init_ending_runner_across();
	OBJECT(current_object)->screen_y += 0x1Au;
	OBJECT(current_object)->screen_x -= 4u;
	OBJECT(object_cursor)->x = 0x1E054;
	OBJECT(object_cursor)->y = 0x1E058;
	OBJECT(object_cursor)->hit_list_next = 0u;
}

void ending_ray_run_across(void)
{
	uint32_t ignored = 0, second_object = 0;

	if (OBJECT(current_object)->ending.started != 0u)
	{
		animate_ending_runner_across();
		OBJECT(object_cursor)->screen_x += 4u;
		OBJECT(current_object)->screen_x += 4u;
		return;
	}
	OBJECT(current_object)->sprite_layer = 3u;
	OBJECT(current_object)->sprite_palette = 0x1210u;
	OBJECT(current_object)->sprite_frame = 0xA7E0u;
	OBJECT(current_object)->sprite_scale = 0xF0u;
	init_ending_runner_across();
	allocate_object_2(&ignored, &ignored, &second_object);
	OBJECT(current_object)->speed_y = second_object;
	current_object = second_object;
	OBJECT(current_object)->sprite_frame = 0xA7F0u;
	OBJECT(current_object)->sprite_scale = 0xF0u;
	OBJECT(current_object)->sprite_palette = 0x1210u;
	OBJECT(current_object)->sprite_layer = 2u;
	init_ending_runner_across();
	OBJECT(current_object)->screen_y += 0x1Au;
	OBJECT(current_object)->screen_x -= 4u;
	OBJECT(object_cursor)->x = 0x1E060;
	OBJECT(object_cursor)->y = 0x1E064;
}

void ending_mighty_run_across(void)
{
	uint32_t ignored = 0, second_object = 0;

	if (OBJECT(current_object)->ending.started != 0u)
	{
		animate_ending_runner_across();
		OBJECT(object_cursor)->screen_x += 3u;
		OBJECT(current_object)->screen_x += 3u;
		return;
	}
	OBJECT(current_object)->sprite_layer = 4u;
	OBJECT(current_object)->sprite_palette = 0x1200u;
	OBJECT(current_object)->sprite_frame = 0xA7B0u;
	OBJECT(current_object)->sprite_scale = 0xE0u;
	init_ending_runner_across();
	allocate_object_2(&ignored, &ignored, &second_object);
	OBJECT(current_object)->speed_y = second_object;
	current_object = second_object;
	OBJECT(current_object)->sprite_frame = 0xA7C0u;
	OBJECT(current_object)->sprite_scale = 0xE0u;
	OBJECT(current_object)->sprite_palette = 0x1200u;
	OBJECT(current_object)->sprite_layer = 3u;
	init_ending_runner_across();
	OBJECT(current_object)->screen_y += 0x1Au;
	OBJECT(current_object)->screen_x -= 4u;
	OBJECT(object_cursor)->x = 0x1E06C;
	OBJECT(object_cursor)->y = 0x1E070;
}

void init_ending_runner_across(void)
{
	AS8(OBJECT(object_cursor)->flags) = 1u;
	AT16(OBJECT(object_cursor)->flags, 2) = 0u;
	AT16(OBJECT(object_cursor)->previous_floor_block, 2) = 0u;
	OBJECT(current_object)->screen_y = 0x68;
	OBJECT(current_object)->screen_x = 0;
	OBJECT(current_object)->sprite_palette |= 0x2000u;
	OBJECT(current_object)->sprite_flags = 0u;
	OBJECT(current_object)->sprite_flags |= 0x140u;
}

void animate_ending_runner_across(void)
{
	uint32_t second_frame_record = 0;

	OBJECT(object_cursor)->animate_ending.timer++;
	OBJECT(object_cursor)->animate_ending.cycle_count++;
	OBJECT(object_cursor)->animate_ending.timer &= 7u;
	OBJECT(object_cursor)->animate_ending.cycle_count &= 3u;
	second_frame_record = OBJECT(object_cursor)->y + OBJECT(object_cursor)->animate_ending.cycle_count * 2u;
	OBJECT(current_object)->sprite_frame = mem_read16(OBJECT(object_cursor)->x + ((uint16_t)OBJECT(object_cursor)->animate_ending.timer >> 2) * 2u);
	queue_sprite_in_layer();
	current_object = OBJECT(object_cursor)->speed_y;
	OBJECT(current_object)->sprite_frame = mem_read16(second_frame_record);
	queue_sprite_in_layer();
	if (OBJECT(object_cursor)->screen_x >= 0x300)
	{
		OBJECT(current_object)->screen_x = 0;
		OBJECT(object_cursor)->screen_x = 0;
	}
}
