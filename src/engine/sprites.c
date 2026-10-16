/**
 *    Name:        engine/sprites.c
 *    Description: animation and the sprite list: animation steps (frames, flips, moves, offsets, multi-part sprites), part palette cycles, the depth buckets, build_sprite_list and the buffer swap at VBLANK
 *    Version:     0.1.0
 *    Created:     12-10-2026
 *    Modified:    15-10-2026
 *    Author:      Sebastian Toyos
 *
 *    SegaSonic The Hedgehog (arcade, Sega System 32): the game's V60 program decompiled into C by
 *    tools/decompile.py, with the names of symbols.txt.
 */
#include "game.h"

void cycle_part_palettes(void)
{
	uint8_t last_index = 0;
	uint16_t palette = 0;
	uint32_t next_index = 0, palettes = 0, part = 0, parts_left = 0;
	uint8_t palette_timer = 0;

	palette_timer = OBJECT(current_object)->cycle_part_palettes.palette_timer;
	OBJECT(current_object)->cycle_part_palettes.palette_timer = palette_timer - 1u;
	if ((int8_t)palette_timer > 1)
		return;
	OBJECT(current_object)->cycle_part_palettes.palette_timer = OBJECT(current_object)->cycle_part_palettes.palette_delay;
	palettes = OBJECT(current_object)->cycle_part_palettes.palette_list;
	last_index = (uint8_t)mem_read16(palettes);
	next_index = OBJECT(current_object)->cycle_part_palettes.palette_index;
	next_index = (uint8_t)(next_index + 1u);
	palettes += 2u;
	if ((int8_t)last_index <= (int8_t)next_index && (int8_t)last_index < (int8_t)next_index)
		next_index = 0u;
	OBJECT(current_object)->cycle_part_palettes.palette_index = next_index;
	palette = mem_read16(palettes + next_index * 2u);
	part = OBJECT(current_object)->part_list;
	parts_left = mem_read16(part);
	part += 2u;
	do
	{
		mem_write16(part + 6u, palette);
		part += 8u;
		parts_left--;
	} while (parts_left != 0u);
}

void animate_with_flags(uint32_t *p_frame, uint32_t *p_at_last_step)
{
	uint32_t frame = *p_frame;
	uint32_t at_last_step = 0;
	uint8_t animation_timer = 0;

	at_last_step = 0u;
	animation_timer = OBJECT(current_object)->animation_timer;
	OBJECT(current_object)->animation_timer = animation_timer - 1u;
	if ((int8_t)animation_timer > 1)
	{
		*p_at_last_step = (*p_at_last_step & 0xFFFFFF00u) | (at_last_step & 0xFFu);
		return;
	}
	OBJECT(current_object)->animation_timer = OBJECT(current_object)->animation_speed;
	animate_step_with_flags(&frame, &at_last_step);
	*p_frame = frame;
	*p_at_last_step = (*p_at_last_step & 0xFFFFFF00u) | (at_last_step & 0xFFu);
}

void animate_step_with_flags(uint32_t *p_frame, uint32_t *p_at_last_step)
{
	uint8_t at_last_step = (uint8_t)*p_at_last_step;
	uint8_t last_frame = 0;
	uint32_t frame = 0, step_record = 0;

	step_record = OBJECT(current_object)->animation;
	last_frame = (uint8_t)mem_read16(step_record);
	frame = OBJECT(current_object)->animation_frame;
	frame = (uint8_t)(frame + 1u);
	step_record += 2u;
	if ((int8_t)last_frame <= (int8_t)frame)
	{
		if ((int8_t)last_frame < (int8_t)frame)
			frame = 0u;
		else
			at_last_step = 0xFFu;
	}
	OBJECT(current_object)->animation_frame = frame;
	frame = frame * 2u;
	step_record += frame * 2u;
	OBJECT(current_object)->sprite_flags = (OBJECT(current_object)->sprite_flags & ~0xC0u) | mem_read16(step_record);
	OBJECT(current_object)->sprite_frame = mem_read16(step_record + 2u);
	*p_frame = frame;
	*p_at_last_step = (*p_at_last_step & 0xFFFFFF00u) | at_last_step;
}

void animate(uint32_t *p_frame, uint32_t *p_wrapped, uint32_t *p_at_last_step)
{
	uint32_t frame = *p_frame;
	uint32_t at_last_step = 0, wrapped = 0;
	uint8_t animation_timer = 0;

	at_last_step = 0u;
	wrapped = 0u;
	animation_timer = OBJECT(current_object)->animation_timer;
	OBJECT(current_object)->animation_timer = animation_timer - 1u;
	if ((int8_t)animation_timer > 1)
	{
		*p_wrapped = (*p_wrapped & 0xFFFFFF00u) | (wrapped & 0xFFu);
		*p_at_last_step = (*p_at_last_step & 0xFFFFFF00u) | (at_last_step & 0xFFu);
		return;
	}
	OBJECT(current_object)->animation_timer = OBJECT(current_object)->animation_speed;
	animate_step(&frame, &wrapped, &at_last_step);
	*p_frame = frame;
	*p_wrapped = (*p_wrapped & 0xFFFFFF00u) | (wrapped & 0xFFu);
	*p_at_last_step = (*p_at_last_step & 0xFFFFFF00u) | (at_last_step & 0xFFu);
}

void animate_step(uint32_t *p_frame, uint32_t *p_wrapped, uint32_t *p_at_last_step)
{
	uint8_t wrapped = (uint8_t)*p_wrapped;
	uint8_t at_last_step = (uint8_t)*p_at_last_step;
	uint8_t last_frame = 0;
	uint32_t frame = 0, frame_list = 0;

	frame_list = OBJECT(current_object)->animation;
	last_frame = (uint8_t)mem_read16(frame_list);
	frame = OBJECT(current_object)->animation_frame;
	frame_list += 2u;
	if ((OBJECT(current_object)->flags & 0x80u) == 0u)
	{
		frame = (uint8_t)(frame + 1u);
		if ((int8_t)frame < (int8_t)last_frame)
		{
			OBJECT(current_object)->animation_frame = frame;
			OBJECT(current_object)->sprite_frame = mem_read16(frame_list + frame * 2u);
			*p_frame = frame;
			return;
		}
		if ((int8_t)last_frame < (int8_t)frame)
		{
			frame = 0u;
			wrapped = 0xFFu;
			OBJECT(current_object)->animation_frame = frame;
			OBJECT(current_object)->sprite_frame = mem_read16(frame_list + frame * 2u);
			*p_frame = frame;
			*p_wrapped = (*p_wrapped & 0xFFFFFF00u) | wrapped;
			return;
		}
		at_last_step = 0xFFu;
		OBJECT(current_object)->animation_frame = frame;
		OBJECT(current_object)->sprite_frame = mem_read16(frame_list + frame * 2u);
		*p_frame = frame;
		*p_at_last_step = (*p_at_last_step & 0xFFFFFF00u) | at_last_step;
		return;
	}
	frame = (uint8_t)(frame - 1u);
	if ((int8_t)frame > 0)
	{
		OBJECT(current_object)->animation_frame = frame;
		OBJECT(current_object)->sprite_frame = mem_read16(frame_list + frame * 2u);
		*p_frame = frame;
		return;
	}
	if ((int8_t)frame < 0)
	{
		frame = (uint8_t)last_frame;
		wrapped = 0xFFu;
		OBJECT(current_object)->animation_frame = frame;
		OBJECT(current_object)->sprite_frame = mem_read16(frame_list + frame * 2u);
		*p_frame = frame;
		*p_wrapped = (*p_wrapped & 0xFFFFFF00u) | wrapped;
		return;
	}
	at_last_step = 0xFFu;
	OBJECT(current_object)->animation_frame = frame;
	OBJECT(current_object)->sprite_frame = mem_read16(frame_list + frame * 2u);
	*p_frame = frame;
	*p_at_last_step = (*p_at_last_step & 0xFFFFFF00u) | at_last_step;
}

uint32_t animate_and_move(uint32_t at_last_step)
{
	uint8_t last_frame = 0;
	uint16_t delta = 0;
	uint32_t frame = 0, step_record = 0;
	uint8_t animation_timer = 0;

	at_last_step &= ~0xFFu;
	animation_timer = OBJECT(current_object)->animation_timer;
	OBJECT(current_object)->animation_timer = animation_timer - 1u;
	if ((int8_t)animation_timer > 1)
		return at_last_step;
	OBJECT(current_object)->animation_timer = OBJECT(current_object)->animation_speed;
	step_record = OBJECT(current_object)->animation;
	last_frame = (uint8_t)mem_read16(step_record);
	frame = OBJECT(current_object)->animation_frame;
	step_record += 2u;
	if ((OBJECT(current_object)->flags & 0x80u) == 0u)
	{
		frame = (uint8_t)(frame + 1u);
		if ((int8_t)last_frame <= (int8_t)frame)
		{
			if ((int8_t)last_frame < (int8_t)frame)
				frame = 0u;
			else
				at_last_step |= 0xFFu;
		}
	}
	else
	{
		frame = (uint8_t)(frame - 1u);
		if ((int8_t)frame <= 0)
		{
			if ((int8_t)frame < 0)
				frame = (uint8_t)last_frame;
			else
				at_last_step |= 0xFFu;
		}
	}
	OBJECT(current_object)->animation_frame = frame;
	step_record += frame * 4u;
	OBJECT(current_object)->sprite_frame = mem_read16(step_record);
	step_record += 2u;
	delta = (uint16_t)(int8_t)mem_read8(step_record);
	step_record++;
	if ((OBJECT(current_object)->sprite_flags & 0x40u) != 0u)
		delta = (uint16_t)(0u - delta);
	OBJECT(current_object)->projected_x += delta;
	delta = (uint16_t)(int8_t)mem_read8(step_record);
	if ((OBJECT(current_object)->sprite_flags & 0x80u) != 0u)
		delta = (uint16_t)(0u - delta);
	OBJECT(current_object)->projected_y_int += delta;
	return at_last_step;
}

uint32_t animate_around_anchor(void)
{
	uint8_t last_frame = 0;
	uint32_t at_last_step = 0, frame = 0, steps = 0;
	uint8_t animation_timer = 0;

	at_last_step = 0u;
	animation_timer = OBJECT(current_object)->animation_timer;
	OBJECT(current_object)->animation_timer = animation_timer - 1u;
	if ((int8_t)animation_timer > 1)
		return at_last_step;
	OBJECT(current_object)->animation_timer = OBJECT(current_object)->animation_speed;
	steps = OBJECT(current_object)->animation;
	last_frame = (uint8_t)mem_read16(steps);
	frame = OBJECT(current_object)->animation_frame;
	steps += 2u;
	if ((OBJECT(current_object)->flags & 0x80u) == 0u)
	{
		frame = (uint8_t)(frame + 1u);
		if ((int8_t)frame < (int8_t)last_frame)
		{
			OBJECT(current_object)->animation_frame = frame;
			set_step_around_anchor(frame, steps);
			return at_last_step;
		}
		if ((int8_t)last_frame < (int8_t)frame)
			frame = 0u;
		else
			at_last_step = 0xFFFFFFFFu;
	}
	else
	{
		frame = (uint8_t)(frame - 1u);
		if ((int8_t)frame > 0)
		{
			OBJECT(current_object)->animation_frame = frame;
			set_step_around_anchor(frame, steps);
			return at_last_step;
		}
		if ((int8_t)frame < 0)
			frame = (uint8_t)last_frame;
		else
			at_last_step = 0xFFFFFFFFu;
	}
	OBJECT(current_object)->animation_frame = frame;
	set_step_around_anchor(frame, steps);
	return at_last_step;
}

void set_step_around_anchor(uint32_t frame, uint32_t steps)
{
	uint16_t delta = 0;

	steps += frame * 4u;
	OBJECT(current_object)->sprite_frame = mem_read16(steps);
	steps += 2u;
	delta = (uint16_t)(int8_t)mem_read8(steps);
	steps++;
	if ((OBJECT(current_object)->sprite_flags & 0x40u) != 0u)
		delta = (uint16_t)(0u - delta);
	OBJECT(current_object)->projected_x = delta + OBJECT(current_object)->anchor_x;
	delta = (uint16_t)(int8_t)mem_read8(steps);
	if ((OBJECT(current_object)->sprite_flags & 0x80u) != 0u)
		delta = (uint16_t)(0u - delta);
	OBJECT(current_object)->projected_y_int = delta + OBJECT(current_object)->anchor_y;
}

uint32_t animate_with_screen_offset(uint32_t at_last_step)
{
	uint8_t last_frame = 0;
	uint32_t frame = 0, step_record = 0;
	uint8_t animation_timer = 0;

	at_last_step &= ~0xFFu;
	animation_timer = OBJECT(current_object)->animation_timer;
	OBJECT(current_object)->animation_timer = animation_timer - 1u;
	if ((int8_t)animation_timer > 1)
	{
		apply_step_screen_offset();
		return at_last_step;
	}
	OBJECT(current_object)->animation_timer = OBJECT(current_object)->animation_speed;
	step_record = OBJECT(current_object)->animation;
	last_frame = (uint8_t)mem_read16(step_record);
	frame = OBJECT(current_object)->animation_frame;
	step_record += 2u;
	if ((OBJECT(current_object)->flags & 0x80u) == 0u)
	{
		frame = (uint8_t)(frame + 1u);
		if ((int8_t)frame < (int8_t)last_frame)
		{
			OBJECT(current_object)->animation_frame = frame;
			step_record += frame * 4u;
			OBJECT(current_object)->sprite_frame = mem_read16(step_record);
			step_record += 2u;
			add_step_screen_offset(step_record);
			return at_last_step;
		}
		if ((int8_t)last_frame < (int8_t)frame)
			frame = 0u;
		else
			at_last_step |= 0xFFu;
	}
	else
	{
		frame = (uint8_t)(frame - 1u);
		if ((int8_t)frame > 0)
		{
			OBJECT(current_object)->animation_frame = frame;
			step_record += frame * 4u;
			OBJECT(current_object)->sprite_frame = mem_read16(step_record);
			step_record += 2u;
			add_step_screen_offset(step_record);
			return at_last_step;
		}
		if ((int8_t)frame < 0)
			frame = (uint8_t)last_frame;
		else
			at_last_step |= 0xFFu;
	}
	OBJECT(current_object)->animation_frame = frame;
	step_record += frame * 4u;
	OBJECT(current_object)->sprite_frame = mem_read16(step_record);
	step_record += 2u;
	add_step_screen_offset(step_record);
	return at_last_step;
}

void apply_step_screen_offset(void)
{
	uint32_t step_offset = 0;

	step_offset = OBJECT(current_object)->animation + 4u + OBJECT(current_object)->animation_frame * 4u;
	add_step_screen_offset(step_offset);
}

void add_step_screen_offset(uint32_t step_offset)
{
	uint16_t delta = 0;

	delta = (uint16_t)(int8_t)mem_read8(step_offset);
	step_offset++;
	if ((OBJECT(current_object)->sprite_flags & 0x40u) != 0u)
		delta = (uint16_t)(0u - delta);
	OBJECT(current_object)->screen_x += delta;
	delta = (uint16_t)(int8_t)mem_read8(step_offset);
	if ((OBJECT(current_object)->sprite_flags & 0x80u) != 0u)
		delta = (uint16_t)(0u - delta);
	OBJECT(current_object)->screen_y += delta;
}

void animate_part_list(void)
{
	uint8_t animation_timer = 0;

	animation_timer = OBJECT(current_object)->animation_timer;
	OBJECT(current_object)->animation_timer = animation_timer - 1u;
	if ((int8_t)animation_timer > 1)
		return;
	OBJECT(current_object)->animation_timer = OBJECT(current_object)->animation_speed;
	animate_part_list_step();
}

void animate_part_list_step(void)
{
	uint8_t max_frame = 0;
	uint32_t frame_offsets = 0, next_frame = 0;

	frame_offsets = OBJECT(current_object)->animation;
	max_frame = (uint8_t)mem_read16(frame_offsets);
	next_frame = OBJECT(current_object)->animation_frame;
	frame_offsets += 2u;
	if ((OBJECT(current_object)->flags & 0x80u) == 0u)
	{
		next_frame = (uint8_t)(next_frame + 1u);
		if ((int8_t)max_frame <= (int8_t)next_frame && (int8_t)max_frame < (int8_t)next_frame)
			next_frame = 0u;
	}
	else
	{
		next_frame = (uint8_t)(next_frame - 1u);
		if ((int8_t)next_frame <= 0 && (int8_t)next_frame < 0)
			next_frame = (uint8_t)max_frame;
	}
	OBJECT(current_object)->animation_frame = next_frame;
	OBJECT(current_object)->part_list = mem_read16(frame_offsets + next_frame * 2u) + frame_offsets - 2u;
}

void animate_all_parts(uint32_t *p_at_last_step, uint32_t *p_part)
{
	uint32_t part = *p_part;
	uint8_t at_last_step = 0, max_frame = 0;
	uint16_t frame_sprite = 0;
	uint32_t frame_sprites = 0, next_frame = 0, parts_left = 0;
	uint8_t animation_timer = 0;

	at_last_step = 0u;
	animation_timer = OBJECT(current_object)->animation_timer;
	OBJECT(current_object)->animation_timer = animation_timer - 1u;
	if ((int8_t)animation_timer > 1)
	{
		*p_at_last_step = (*p_at_last_step & 0xFFFFFF00u) | at_last_step;
		return;
	}
	OBJECT(current_object)->animation_timer = OBJECT(current_object)->animation_speed;
	frame_sprites = OBJECT(current_object)->animation;
	max_frame = (uint8_t)mem_read16(frame_sprites);
	next_frame = OBJECT(current_object)->animation_frame;
	frame_sprites += 2u;
	if ((OBJECT(current_object)->flags & 0x80u) == 0u)
	{
		next_frame = (uint8_t)(next_frame + 1u);
		if ((int8_t)max_frame <= (int8_t)next_frame)
		{
			if ((int8_t)max_frame < (int8_t)next_frame)
				next_frame = 0u;
			else
				at_last_step = 0xFFu;
		}
	}
	else
	{
		next_frame = (uint8_t)(next_frame - 1u);
		if ((int8_t)next_frame <= 0)
		{
			if ((int8_t)next_frame < 0)
				next_frame = (uint8_t)max_frame;
			else
				at_last_step = 0xFFu;
		}
	}
	OBJECT(current_object)->animation_frame = next_frame;
	frame_sprite = mem_read16(frame_sprites + next_frame * 2u);
	part = OBJECT(current_object)->part_list;
	parts_left = mem_read16(part);
	part += 2u;
	do
	{
		mem_write16(part, frame_sprite);
		part += 8u;
		parts_left--;
	} while (parts_left != 0u);
	*p_at_last_step = (*p_at_last_step & 0xFFFFFF00u) | at_last_step;
	*p_part = part;
}

void init_sprites_without_swaps(void)
{
	uint32_t end_mark = 0, ignored = 0;

	vblank_hook = cancel_sprite_buffer_swap_ADDRESS;
	init_sprites_from_frame_wait(&end_mark, &ignored);
}

void init_sprites_swapping_every_frame(void)
{
	uint32_t end_mark = 0, ignored = 0;

	vblank_hook = swap_sprite_buffers_now_ADDRESS;
	frames_to_wait = 1;
	frames_per_wait = 1u;
	init_sprite_hardware(&end_mark, &ignored);
}

void init_sprites(uint32_t *p_end_mark, uint32_t *p_source)
{
	uint32_t control_init = 0, end_mark = 0;

	vblank_hook = swap_sprite_buffers_ADDRESS;
	init_sprites_from_frame_wait(&end_mark, &control_init);
	*p_end_mark = (*p_end_mark & 0xFFFF0000u) | (end_mark & 0xFFFFu);
	*p_source = control_init;
}

void init_sprites_from_frame_wait(uint32_t *p_end_mark, uint32_t *p_source)
{
	uint32_t control_init = 0, end_mark = 0;

	frames_to_wait = 0;
	frames_per_wait = 0u;
	init_sprite_hardware(&end_mark, &control_init);
	*p_end_mark = (*p_end_mark & 0xFFFF0000u) | (end_mark & 0xFFFFu);
	*p_source = control_init;
}

void init_sprite_hardware(uint32_t *p_end_mark, uint32_t *p_source)
{
	uint16_t end_mark = 0;
	uint32_t control_cursor = 0, control_init = 0, dest_end = 0, ignored = 0, list_start = 0, registers_left = 0;

	control_init = sprite_control_init_ADDRESS;
	mem_write16(sprite_flip, setting_monitor_flip);
	registers_left = 4u;
	control_cursor = SPRITE_CONTROL + 0x6u;
	do
	{
		mem_write16(control_cursor, ROM8(control_init));
		registers_left--;
		control_init++;
		control_cursor += 2u;
	} while (registers_left != 0u);
	move_string(sprite_list_header_ADDRESS, 0x10u, 0x400000u, 0x10u, 2u, 0u, 0u, fill_value, &dest_end, &ignored);
	list_start = dest_end;
	sprite_jump_init = 0xA400u;
	sprite_buffer_flip = 0u;
	sprite_back_buffer = list_start;
	sprite_list_start = list_start;
	end_mark = 0xFFFFu;
	mem_write16(list_start, 0xFFFFu);
	mem_write16(list_start + 0x1FF0u, 0xFFFFu);
	mem_write16(list_start + 0x3FE0u, 0xFFFFu);
	mem_write16(list_start + 0x5FD0u, 0xFFFFu);
	sprite_list_end_command = 0xFFFFu;
	sprite_frames = 0x130000u;
	ignored = clear_sprite_buckets();
	*p_end_mark = (*p_end_mark & 0xFFFF0000u) | end_mark;
	*p_source = control_init;
}

uint32_t clear_sprite_buckets(void)
{
	uint32_t bucket = 0, buckets_left = 0;

	sprite_buckets_end = 0xFFFFu;
	buckets_left = 0x100u;
	bucket = sprite_buckets_ADDRESS;
	do
	{
		mem_write16(bucket, bucket);
		bucket += 2u;
		buckets_left--;
	} while (buckets_left != 0u);
	return bucket;
}

uint32_t queue_sprite(uint32_t depth)
{
	uint32_t ignored = 0;

	depth = (depth & ~0xFFFFu) | (uint16_t)OBJECT(current_object)->screen_y;
	queue_sprite_at_depth(&ignored, &depth);
	return depth;
}

void queue_sprite_at_depth(uint32_t *p_bucket_head, uint32_t *p_depth)
{
	uint32_t depth = *p_depth;
	uint32_t bucket_head = 0;

	depth = (depth & ~0xFFFFu) | (uint16_t)(depth + 0x10u);
	if ((int16_t)depth < 0xFE)
	{
		if ((int16_t)depth >= 0)
		{
			bucket_head = sprite_buckets_ADDRESS + ((uint16_t)(0xFFu - depth) & 0xFFu) * 2u;
			OBJECT(current_object)->draw_next = mem_read16(bucket_head);
			mem_write16(bucket_head, current_object);
			*p_bucket_head = bucket_head;
			*p_depth = depth;
			return;
		}
		depth = 0u;
		bucket_head = sprite_buckets_ADDRESS + ((uint16_t)(0xFFu - depth) & 0xFFu) * 2u;
		OBJECT(current_object)->draw_next = mem_read16(bucket_head);
		mem_write16(bucket_head, current_object);
		*p_bucket_head = bucket_head;
		*p_depth = depth;
		return;
	}
	depth = 0xFEu;
	bucket_head = sprite_buckets_ADDRESS + ((uint16_t)(0xFFu - depth) & 0xFFu) * 2u;
	OBJECT(current_object)->draw_next = mem_read16(bucket_head);
	mem_write16(bucket_head, current_object);
	*p_bucket_head = bucket_head;
	*p_depth = depth;
}

void queue_sprite_in_layer(void)
{
	uint32_t bucket_head = 0;

	bucket_head = sprite_buckets_ADDRESS + (OBJECT(current_object)->sprite_layer & 0xFFu) * 2u;
	OBJECT(current_object)->draw_next = mem_read16(bucket_head);
	mem_write16(bucket_head, current_object);
}

void queue_sprite_on_top(void)
{
	uint8_t bucket = 0;

	bucket = 0u;
	queue_sprite_in_bucket(bucket);
}

void queue_sprite_in_bucket(uint32_t bucket)
{
	bucket = sprite_buckets_ADDRESS + (bucket & 0xFFu) * 2u;
	OBJECT(current_object)->draw_next = mem_read16(bucket);
	mem_write16(bucket, current_object);
}

uint32_t queue_sprite_at_floor(void)
{
	uint32_t bucket_head = 0, floor_y = 0;

	floor_y = (int32_t)((uint32_t)OBJECT(current_object)->projected_y + OBJECT(current_object)->floor_z) >> 16;
	floor_y = (floor_y & ~0xFFFFu) | (uint16_t)(floor_y - view_y);
	floor_y = (floor_y & ~0xFFFFu) | (uint16_t)(floor_y + 0x10u);
	if ((int16_t)floor_y < 0xFE)
	{
		if ((int16_t)floor_y < 0)
			floor_y = 0u;
	}
	else
		floor_y = 0xFEu;
	bucket_head = sprite_buckets_ADDRESS + ((uint16_t)(0xFFu - floor_y) & 0xFFu) * 2u;
	OBJECT(current_object)->draw_next = mem_read16(bucket_head);
	mem_write16(bucket_head, current_object);
	return floor_y;
}

uint32_t queue_sprite_at_previous_floor(void)
{
	uint32_t bucket_head = 0, floor_y = 0;

	floor_y = (int32_t)((uint32_t)OBJECT(current_object)->projected_y + OBJECT(current_object)->previous_floor_z) >> 16;
	floor_y = (floor_y & ~0xFFFFu) | (uint16_t)(floor_y - view_y);
	floor_y = (floor_y & ~0xFFFFu) | (uint16_t)(floor_y + 0x10u);
	if ((int16_t)floor_y < 0xFE)
	{
		if ((int16_t)floor_y < 0)
			floor_y = 0u;
	}
	else
		floor_y = 0xFEu;
	bucket_head = sprite_buckets_ADDRESS + ((uint16_t)(0xFFu - floor_y) & 0xFFu) * 2u;
	OBJECT(current_object)->draw_next = mem_read16(bucket_head);
	mem_write16(bucket_head, current_object);
	return floor_y;
}

uint32_t queue_sprite_at_base(uint32_t depth)
{
	uint32_t bucket_head = 0;

	depth = (depth & ~0xFFFFu) | (uint16_t)OBJECT(current_object)->screen_y;
	depth = (depth & ~0xFFFFu) | (uint16_t)(depth - OBJECT(current_object)->sprite_offset_y);
	depth = (depth & ~0xFFFFu) | (uint16_t)(depth + 0x10u);
	if ((int16_t)depth < 0xFE)
	{
		if ((int16_t)depth < 0)
			depth = 0u;
	}
	else
		depth = 0xFEu;
	bucket_head = sprite_buckets_ADDRESS + ((uint16_t)(0xFFu - depth) & 0xFFu) * 2u;
	OBJECT(current_object)->draw_next = mem_read16(bucket_head);
	mem_write16(bucket_head, current_object);
	return depth;
}

uint32_t depth_to_bucket(uint32_t bucket, uint16_t depth)
{
	depth += 0x10u;
	if ((int16_t)depth < 0xFE)
	{
		if ((int16_t)depth < 0)
			depth = 0u;
	}
	else
		depth = 0xFEu;
	bucket = (bucket & ~0xFFFFu) | 0xFFu;
	bucket = (bucket & ~0xFFFFu) | (uint16_t)(bucket - depth);
	return bucket;
}

void queue_sprite_at_depth_offset(uint16_t depth, uint8_t bucket_offset)
{
	uint32_t bucket_head = 0;

	depth += 0x10u;
	if ((int16_t)depth < 0xFE)
	{
		if ((int16_t)depth < 0)
			depth = 0u;
	}
	else
		depth = 0xFEu;
	bucket_head = sprite_buckets_ADDRESS + ((uint16_t)(0xFFu - depth + bucket_offset) & 0xFFu) * 2u;
	OBJECT(current_object)->draw_next = mem_read16(bucket_head);
	mem_write16(bucket_head, current_object);
}

void queue_sprite_at_previous_floor_2(void)
{
	uint16_t floor_depth = 0;
	uint32_t bucket_head = 0;

	floor_depth = (uint16_t)(((int32_t)((uint32_t)OBJECT(current_object)->projected_y + OBJECT(current_object)->previous_floor_z) >> 16) - view_y + 0x10u);
	if ((int16_t)floor_depth < 0xFE)
	{
		if ((int16_t)floor_depth < 0)
			floor_depth = 0u;
	}
	else
		floor_depth = 0xFEu;
	bucket_head = sprite_buckets_ADDRESS + ((uint16_t)(0xFFu - floor_depth) & 0xFFu) * 2u;
	OBJECT(current_object)->draw_next = mem_read16(bucket_head);
	mem_write16(bucket_head, current_object);
}

void get_previous_floor_bucket(uint32_t top_bucket, uint32_t *p_floor_depth, uint32_t *p_bucket)
{
	uint32_t bucket = 0, floor_depth = 0;

	floor_depth = (int32_t)((uint32_t)OBJECT(current_object)->projected_y + OBJECT(current_object)->previous_floor_z) >> 16;
	floor_depth = (floor_depth & ~0xFFFFu) | (uint16_t)(floor_depth - view_y);
	floor_depth = (floor_depth & ~0xFFFFu) | (uint16_t)(floor_depth + 0x10u);
	if ((int16_t)floor_depth < 0xFE)
	{
		if ((int16_t)floor_depth >= 0)
		{
			top_bucket = (top_bucket & ~0xFFFFu) | 0xFFu;
			bucket = (top_bucket & ~0xFFFFu) | (uint16_t)(top_bucket - floor_depth);
			*p_floor_depth = floor_depth;
			*p_bucket = bucket;
			return;
		}
		floor_depth = 0u;
		top_bucket = (top_bucket & ~0xFFFFu) | 0xFFu;
		bucket = (top_bucket & ~0xFFFFu) | (uint16_t)(top_bucket - floor_depth);
		*p_floor_depth = floor_depth;
		*p_bucket = bucket;
		return;
	}
	floor_depth = 0xFEu;
	top_bucket = (top_bucket & ~0xFFFFu) | 0xFFu;
	bucket = (top_bucket & ~0xFFFFu) | (uint16_t)(top_bucket - floor_depth);
	*p_floor_depth = floor_depth;
	*p_bucket = bucket;
}

void swap_sprite_buffers(void)
{
	if (sprite_list_ready == 0u)
		return;
	sprite_list_ready = 0u;
	sprite_buffer_flip += 2u;
	swap_sprite_buffers_now();
}

void swap_sprite_buffers_now(void)
{
	uint32_t swap_entry = 0;
	uint8_t status = 0;

	sprite_buffer_flip += 2u;
	swap_entry = sprite_buffer_swaps_ADDRESS + (6u & sprite_buffer_flip) * 2u;
	AS16(sprite_back_buffer) = ROM16(swap_entry);
	mem_write16(sprite_list_jump, ROM16(swap_entry + 2u));
	status = mem_read8(sprite_status);
}

void cancel_sprite_buffer_swap(void)
{
	sprite_list_ready = 0u;
}

void load_sized_frame_address_at_row(uint16_t row)
{
	uint16_t row_bytes = 0;
	uint32_t frame_record = 0;

	frame_record = (sprite_frames & ~0xFFFFu) | OBJECT(current_object)->sprite_frame;
	row_bytes = mem_read16(frame_record);
	frame_record += 2u;
	if ((OBJECT(current_object)->sprite_flags & 0x200u) == 0u)
		row_bytes = row_bytes >> 1;
	OBJECT(current_object)->sprite_scale = (uint32_t)(row_bytes >> 2) * row + mem_read16(frame_record + 4u);
}

void load_sized_frame_address(void)
{
	OBJECT(current_object)->sprite_scale = mem_read16(((sprite_frames & ~0xFFFFu) | OBJECT(current_object)->sprite_frame) + 6u);
}

void load_frame_size(void)
{
	uint32_t frame_record = 0;

	frame_record = (sprite_frames & ~0xFFFFu) | OBJECT(current_object)->sprite_frame;
	OBJECT(current_object)->sprite_width = mem_read16(frame_record);
	OBJECT(current_object)->sprite_height = mem_read8(frame_record + 2u);
}

void build_sprite_list(uint32_t *p_height, uint32_t *p_scale_x, uint32_t *p_scale_y, uint32_t *p_sprite_flags, uint32_t *p_sprite_entry)
{
	uint32_t height = *p_height;
	uint32_t scale_x = *p_scale_x;
	uint32_t scale_y = *p_scale_y;
	uint16_t sprite_flags = (uint16_t)*p_sprite_flags;
	uint8_t scale_shift = 0;
	uint16_t dest_height = 0, dest_width = 0, part_dest_width = 0, screen_x = 0, screen_y = 0, sized_width = 0, source_width_2 = 0;
	uint32_t r4 = 0, flags_word = 0, frame_data = 0, ignored = 0, object = 0, part_scale = 0, parts_left = 0, rounding = 0, source_width = 0, sprite_entry = 0, sprite_scale = 0;
	uint16_t part_width = 0, part_height = 0, width_bits = 0;

	sprite_entry = sprite_list_start;
	last_object = 0x20FC02u;
	frame_data = sprite_frames;
	rounding = 0x80u;
	scale_shift = 0xF8u;
	for (;;)
	{
		last_object = last_object - 2u;
		current_object = LINKED(current_object, OBJECT(last_object)->draw_next);
		if ((uint16_t)last_object == (uint16_t)current_object)
			continue;
		if ((uint16_t)last_object < (uint16_t)current_object)
		{
			*p_height = height;
			*p_scale_x = scale_x;
			*p_scale_y = scale_y;
			*p_sprite_flags = (*p_sprite_flags & 0xFFFF0000u) | sprite_flags;
			*p_sprite_entry = sprite_entry;
			return;
		}
		do
		{
			sprite_flags = OBJECT(current_object)->sprite_flags;
			flags_word = sprite_flags;
			flags_word = (uint16_t)((flags_word << 6) | (flags_word >> 10));
			switch (flags_word & 0x34u)
			{
			case 0x0u:
			case 0x37u:
				frame_data = (frame_data & ~0xFFFFu) | OBJECT(current_object)->sprite_frame;
				sprite_scale = OBJECT(current_object)->sprite_scale;
				source_width = mem_read16(frame_data);
				frame_data += 2u;
				height = (height & ~0xFFFFu) | mem_read16(frame_data);
				dest_width = (uint16_t)shift_by(source_width * sprite_scale + rounding, scale_shift, 32);
				dest_height = (uint16_t)shift_by((uint8_t)height * sprite_scale + rounding, scale_shift, 32);
				source_width = ((uint16_t)(source_width << 6) & ~0xFFu) | (uint8_t)height;
				height &= ~0xFFu;
				height = (height & ~0xFFFFu) | (uint16_t)(height | dest_height);
				frame_data += 2u;
			build_sprite_list_write_entry:
				dest_width |= mem_read16(frame_data);
				mem_write16(sprite_entry, sprite_flags);
				sprite_entry += 2u;
				mem_write16(sprite_entry, rotate_by(source_width, scale_shift, 16));
				sprite_entry += 2u;
				mem_write16(sprite_entry, height);
				sprite_entry += 2u;
				mem_write16(sprite_entry, dest_width);
				sprite_entry += 2u;
				mem_write32(sprite_entry, AS32(OBJECT(current_object)->screen_y));
				sprite_entry += 4u;
				frame_data += 2u;
				mem_write16(sprite_entry, mem_read16(frame_data));
				sprite_entry += 2u;
				mem_write16(sprite_entry, OBJECT(current_object)->sprite_palette);
				sprite_entry += 2u;
				break;
			case 0x3u:
			case 0x4u:
				frame_data = (frame_data & ~0xFFFFu) | OBJECT(current_object)->sprite_frame;
				source_width = mem_read16(frame_data);
				dest_width = (uint16_t)source_width;
				frame_data += 2u;
				height = (height & ~0xFFFFu) | mem_read16(frame_data);
				source_width = ((uint16_t)(source_width << 6) & ~0xFFu) | (uint8_t)height;
				sprite_flags &= 0x2FFFu;
				frame_data += 2u;
				goto build_sprite_list_write_entry;
			case 0x7u:
			case 0x8u:
			case 0x9u:
			case 0xAu:
			case 0xBu:
			case 0xCu:
			case 0xDu:
			case 0xEu:
			case 0xFu:
			case 0x10u:
			case 0x13u:
			case 0x14u:
				object = current_object + 2u;
				mem_write32(sprite_entry, AS32(OBJECT(object - 0x2u)->sprite_flags));
				sprite_entry += 4u;
				object += 4u;
				mem_write32(sprite_entry, AS32(OBJECT(object - 0x6u)->sprite_palette));
				sprite_entry += 4u;
				object += 4u;
				mem_write32(sprite_entry, AS32(OBJECT(object - 0xAu)->sprite_layer));
				sprite_entry += 4u;
				mem_write32(sprite_entry, AS32(OBJECT(object - 0xAu)->screen_x));
				sprite_entry += 4u;
				break;
			case 0x17u:
			case 0x18u:
			case 0x19u:
			case 0x1Au:
			case 0x1Bu:
			case 0x1Cu:
			case 0x1Du:
			case 0x1Eu:
			case 0x1Fu:
			case 0x20u:
				mem_write16(sprite_entry, (sprite_entry >> 4) - 0x7FFFu);
				sprite_entry += 2u;
				mem_write32(sprite_entry, AS32(OBJECT(current_object)->screen_y));
				sprite_entry += 0xEu;
				break;
			case 0x23u:
			case 0x24u:
				sprite_flags &= 0x2FFFu;
				frame_data = (frame_data & ~0xFFFFu) | OBJECT(current_object)->sprite_frame;
				source_width = mem_read16(frame_data);
				frame_data += 2u;
				height = (height & ~0xFFFFu) | mem_read16(frame_data);
				source_width = rotate_by(((uint16_t)(source_width << 6) & ~0xFFu) | (uint8_t)height, scale_shift, 16);
				height &= ~0xFFu;
				height = (height & ~0xFFFFu) | (uint16_t)(height | OBJECT(current_object)->sprite_height);
				frame_data += 2u;
				sized_width = (uint16_t)(OBJECT(current_object)->sprite_width | mem_read16(frame_data));
				mem_write16(sprite_entry, sprite_flags);
				sprite_entry += 2u;
				mem_write16(sprite_entry, source_width);
				sprite_entry += 2u;
				mem_write16(sprite_entry, height);
				sprite_entry += 2u;
				mem_write16(sprite_entry, sized_width);
				sprite_entry += 2u;
				mem_write32(sprite_entry, AS32(OBJECT(current_object)->screen_y));
				sprite_entry += 4u;
				mem_write16(sprite_entry, OBJECT(current_object)->sprite_scale);
				sprite_entry += 2u;
				mem_write16(sprite_entry, OBJECT(current_object)->sprite_palette);
				sprite_entry += 2u;
				frame_data += 2u;
				break;
			case 0x27u:
			case 0x28u:
			case 0x29u:
			case 0x2Au:
			case 0x2Bu:
			case 0x2Cu:
			case 0x2Du:
			case 0x2Eu:
			case 0x2Fu:
			case 0x30u:
				part_scale = OBJECT(current_object)->sprite_scale;
			build_sprite_list_write_parts:
				switch ((sprite_flags >> 5) & 6u)
				{
				case 0x0u:
					scale_x = part_scale;
				build_sprite_list_y_not_flipped:
					scale_y = part_scale;
					break;
				case 0x2u:
					scale_x = 0u - part_scale;
					goto build_sprite_list_y_not_flipped;
				case 0x4u:
					scale_x = part_scale;
					scale_y = 0u - part_scale;
					break;
				case 0x6u:
					scale_x = 0u - part_scale;
					scale_y = 0u - part_scale;
					break;
				default:
					unknown_code(0x9A3C4u + ((sprite_flags >> 5) & 6u));
				}
				sprite_flags &= 0xF0u;
				screen_x = (uint16_t)OBJECT(current_object)->screen_x;
				screen_y = (uint16_t)OBJECT(current_object)->screen_y;
				r14 = OBJECT(current_object)->part_list;
				parts_left = mem_read16(r14);
				r14 = r14 + 2u;
				if (parts_left >= 0x11u)
				{
					int_default(&flags_word, &source_width, &ignored, &r4);
					*p_height = r4;
					*p_scale_x = scale_x;
					*p_scale_y = scale_y;
					*p_sprite_flags = (*p_sprite_flags & 0xFFFF0000u) | sprite_flags;
					*p_sprite_entry = sprite_entry;
					return;
				}
				do
				{
					frame_data = (frame_data & ~0xFFFFu) | mem_read16(r14);
					part_width = mem_read16(frame_data);
					frame_data += 2u;
					source_width_2 = part_width;
					part_height = mem_read16(frame_data);
					frame_data += 2u;
					height = (height & ~0xFFFFu) | part_height;
					part_dest_width = (uint16_t)shift_by(source_width_2 * part_scale + rounding, scale_shift, 32);
					dest_height = (uint16_t)shift_by((uint8_t)height * part_scale + rounding, scale_shift, 32);
					source_width_2 = (uint16_t)(((uint16_t)((uint32_t)source_width_2 << 6) & ~0xFFu) | (uint8_t)height);
					height &= ~0xFFu;
					height = (height & ~0xFFFFu) | (uint16_t)(height | dest_height);
					width_bits = mem_read16(frame_data);
					frame_data += 2u;
					r14 = r14 + 2u;
					mem_write16(sprite_entry, mem_read16(r14) ^ sprite_flags);
					sprite_entry += 2u;
					mem_write16(sprite_entry, rotate_by(source_width_2, scale_shift, 16));
					sprite_entry += 2u;
					mem_write16(sprite_entry, height);
					sprite_entry += 2u;
					mem_write16(sprite_entry, part_dest_width | width_bits);
					sprite_entry += 2u;
					r14 = r14 + 2u;
					source_width = (int8_t)mem_read8(r14) * scale_x;
					r14 = r14 + 1u;
					mem_write16(sprite_entry, shift_signed_by((int8_t)mem_read8(r14) * scale_y, scale_shift, 32) + screen_y);
					sprite_entry += 2u;
					mem_write16(sprite_entry, shift_signed_by(source_width, scale_shift, 32) + screen_x);
					sprite_entry += 2u;
					mem_write16(sprite_entry, mem_read16(frame_data));
					sprite_entry += 2u;
					r14 = r14 + 1u;
					mem_write16(sprite_entry, mem_read16(r14));
					sprite_entry += 2u;
					parts_left--;
					r14 = r14 + 2u;
				} while (parts_left != 0u);
				break;
			case 0x33u:
			case 0x34u:
				part_scale = 0x100u;
				goto build_sprite_list_write_parts;
			default:
				unknown_code(0x9A2E0u + (flags_word & 0x34u));
			}
			current_object = LINKED(current_object, OBJECT(current_object)->draw_next);
		} while ((uint16_t)current_object != (uint16_t)last_object);
		OBJECT(last_object)->draw_next = last_object;
	}
}
