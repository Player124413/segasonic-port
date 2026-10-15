/**
 *    Name:        engine/animation.c
 *    Description: animation steps: frames, flips, moves and screen offsets per step, around an anchor, multi-part sprites
 *    Version:     0.1.0
 *    Created:     12-10-2026
 *    Modified:    15-10-2026
 *    Author:      Sebastian Toyos
 *
 *    SegaSonic The Hedgehog (arcade, Sega System 32): the game's V60 program decompiled into C by
 *    tools/decompile.py, with the names of symbols.txt.
 */
#include "game.h"

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
