/**
 *    Name:        objects/catapult.c
 *    Description: stage 4's catapults; the goal unlocker of stages B-F
 *    Version:     0.1.0
 *    Created:     12-10-2026
 *    Modified:    15-10-2026
 *    Author:      Sebastian Toyos
 *
 *    SegaSonic The Hedgehog (arcade, Sega System 32): the game's V60 program decompiled into C by
 *    tools/decompile.py, with the names of symbols.txt.
 */
#include "game.h"

void catapult_x_start(void)
{
	OBJECT(current_object)->catapult.state = 0u;
	OBJECT(current_object)->flags = 0u;
	OBJECT(current_object)->handler = catapult_wait_for_platform_ADDRESS;
	add_to_rider_list();
}

void catapult_y_start(void)
{
	OBJECT(current_object)->catapult.state = 0x1Au;
	OBJECT(current_object)->flags = 0u;
	OBJECT(current_object)->handler = catapult_wait_for_platform_ADDRESS;
	add_to_rider_list();
}

void catapult_wait_for_platform(void)
{
	uint32_t state = 0;

	if ((OBJECT(current_object)->flags & 8u) != 0u)
	{
		state = OBJECT(current_object)->catapult.state;
		OBJECT(current_object)->catapult.rides_platform = 0xFFu;
		catapult_wait_for_platform_setup(state, 0u);
		return;
	}
	add_to_rider_list();
}

void catapult_y_start_on_floor(uint32_t object)
{
	uint32_t template_offset = 0;

	template_offset = 0x1Au;
	OBJECT(current_object)->catapult.rides_platform = 0u;
	OBJECT(current_object)->flags = 0u;
	catapult_wait_for_platform_setup(template_offset, object);
}

void catapult_x_start_on_floor(uint32_t object)
{
	uint32_t template_offset = 0;

	OBJECT(current_object)->catapult.rides_platform = 0u;
	OBJECT(current_object)->flags = 0u;
	template_offset = 0u;
	catapult_wait_for_platform_setup(template_offset, object);
}

void catapult_wait_for_platform_setup(uint32_t template_offset, uint32_t object)
{
	uint16_t launch_delay = 0;
	uint32_t block_offset = 0, frame = 0, free_count = 0, ignored = 0, steps = 0, template_cursor = 0, x = 0, y = 0;

	template_cursor = object_pair_templates_ADDRESS + template_offset;
	catapult_pair_template = template_cursor;
	OBJECT(current_object)->handler = mem_read32(template_cursor);
	template_cursor += 4u;
	OBJECT(current_object)->animation = mem_read32(template_cursor);
	template_cursor += 4u;
	OBJECT(current_object)->sprite_offset_x = mem_read8(template_cursor);
	template_cursor++;
	OBJECT(current_object)->sprite_offset_y = mem_read8(template_cursor);
	OBJECT(current_object)->catapult.launch_kind = mem_read8(template_cursor + 1u);
	OBJECT(current_object)->sprite_flags = 0x1000u;
	OBJECT(current_object)->hitbox_height = 0x20u;
	OBJECT(current_object)->hitbox_size_x = 0x10u;
	OBJECT(current_object)->hitbox_size_y = 0u;
	OBJECT(current_object)->sprite_palette = 0x9000u;
	OBJECT(current_object)->animation_frame = 0u;
	find_floor(&x, &y, &block_offset, &ignored, &object);
	OBJECT(current_object)->z = OBJECT(current_object)->floor_z;
	project_object(&ignored, &ignored);
	OBJECT(current_object)->anchor_x = OBJECT(current_object)->projected_x;
	OBJECT(current_object)->anchor_y = OBJECT(current_object)->projected_y_int;
	OBJECT(current_object)->catapult.launch_position = scroll_position;
	launch_delay = catapult_launch_delays[OBJECT(current_object)->sprite_frame];
	OBJECT(current_object)->catapult.launch_position += launch_delay;
	OBJECT(current_object)->kind = 5u;
	OBJECT(current_object)->catapult.state = 4u;
	if (launch_delay == 0u)
	{
		OBJECT(current_object)->kind = OBJECT(current_object)->catapult.launch_kind;
		OBJECT(current_object)->catapult.state = 0u;
	}
	frame = 0u;
	steps = OBJECT(current_object)->animation + 2u;
	set_step_around_anchor(frame, steps);
	if (free_object_stack < 1)
	{
		delete_current_object();
		return;
	}
	allocate_object(&ignored, &object);
	OBJECT(object)->sprite_flags = 0x1000u;
	OBJECT(object)->sprite_frame = OBJECT(current_object)->sprite_frame;
	OBJECT(object)->sprite_palette = 0x9000u;
	OBJECT(object)->anchor_x = OBJECT(current_object)->anchor_x;
	OBJECT(object)->anchor_y = OBJECT(current_object)->anchor_y;
	OBJECT(object)->x = OBJECT(current_object)->x;
	OBJECT(object)->y = OBJECT(current_object)->y;
	OBJECT(object)->z = OBJECT(current_object)->z;
	OBJECT(object)->hitbox_height = OBJECT(current_object)->hitbox_height;
	template_cursor = catapult_pair_template + 0xCu;
	OBJECT(object)->hitbox_size_x = mem_read16(template_cursor);
	template_cursor += 2u;
	OBJECT(object)->hitbox_size_y = mem_read16(template_cursor);
	template_cursor += 2u;
	OBJECT(object)->hitbox_offset_x = mem_read8(template_cursor);
	template_cursor++;
	OBJECT(object)->hitbox_offset_y = mem_read8(template_cursor);
	template_cursor++;
	OBJECT(object)->handler = mem_read32(template_cursor);
	OBJECT(object)->animation = mem_read32(template_cursor + 4u);
	OBJECT(object)->flags = 2u;
	OBJECT(object)->kind = 5u;
	OBJECT(object)->sprite_offset_x = 0;
	OBJECT(object)->sprite_offset_y = 0;
	OBJECT(object)->parent = current_object;
	OBJECT(current_object)->child = object;
	if (OBJECT(current_object)->catapult.state != 0u)
	{
		allocate_object(&free_count, &object);
		if (free_count != 0u)
		{
			OBJECT(object)->handler = catapult_dummy_update_ADDRESS;
			OBJECT(object)->parent = current_object;
			OBJECT(object)->x = OBJECT(current_object)->x;
			OBJECT(object)->y = OBJECT(current_object)->y;
			OBJECT(object)->flags = 0u;
			if (OBJECT(current_object)->catapult.rides_platform != 0u)
			{
				OBJECT(object)->flags |= 8u;
				OBJECT(object)->platform = OBJECT(current_object)->platform;
			}
		}
	}
}

void catapult_update(void)
{
	uint16_t depth_offset = 0;
	uint32_t r1 = 0, depth = 0, ignored = 0, object = 0, off_screen = 0, platform_z = 0;

	off_screen = check_left_screen();
	if ((uint8_t)off_screen != 0u)
	{
		object = LINKED(current_object, OBJECT(current_object)->child);
		delete_object(object, &ignored, &ignored);
		delete_current_object();
		return;
	}
	current_player = current_object;
	current_player = LINKED(current_player, OBJECT(current_object)->child);
	switch (OBJECT(current_object)->catapult.state)
	{
	case 0:
		catapult_wait_for_rider();
		break;
	case 1:
		catapult_swing_up();
		break;
	case 2:
		catapult_hold_up();
		break;
	case 3:
		catapult_swing_down();
		break;
	case 4:
		catapult_wait_for_scroll();
		break;
	case 5:
		catapult_wait_for_rider();
		break;
	default:
		unknown_code(0x810C5u + OBJECT(current_object)->catapult.state * 3u);
	}
	if (OBJECT(current_object)->catapult.rides_platform != 0u)
	{
		platform_z = OBJECT(LINKED(current_object, OBJECT(current_object)->platform))->z;
		OBJECT(current_object)->floor_z = platform_z;
		OBJECT(current_object)->z = platform_z;
	}
	update_screen_position(&ignored, &r1);
	add_to_touchable_list(ignored);
	depth = (uint16_t)OBJECT(current_object)->screen_y;
	depth_offset = 0xFFECu;
	if (OBJECT(current_object)->catapult.state >= 1 && OBJECT(current_object)->catapult.state <= 3)
		depth_offset = 0x14u;
	depth = (uint16_t)(depth + depth_offset);
	queue_sprite_at_depth(&ignored, &depth);
}

void catapult_wait_for_rider(void)
{
	uint32_t frame = 0, steps = 0;

	frame = 0u;
	if ((OBJECT(current_object)->flags & 4u) != 0u)
		frame = 1u;
	OBJECT(current_object)->animation_frame = frame;
	steps = OBJECT(current_object)->animation + 2u;
	set_step_around_anchor(frame, steps);
	if ((OBJECT(current_object)->flags & 4u) == 0u || OBJECT(current_object)->touch_distance > 0x211)
		return;
	catapult_launch();
}

void catapult_launch(void)
{
	uint8_t sound_id = 0;
	uint32_t write_index = 0, write_index_address = 0;

	OBJECT(current_object)->catapult.state = 1u;
	OBJECT(current_object)->animation_timer = 0;
	OBJECT(current_object)->animation_speed = 3u;
	OBJECT(current_object)->animation_frame = 1u;
	OBJECT(current_object)->flags &= ~0x80u;
	sound_id = 0x1Du;
	play_sound(sound_id, &write_index, &write_index_address);
}

void catapult_swing_up(void)
{
	uint32_t at_last_step = 0;

	at_last_step = animate_around_anchor();
	if ((int8_t)at_last_step < 0)
	{
		OBJECT(current_object)->catapult.state++;
		OBJECT(current_object)->animation_timer = 3;
		OBJECT(current_object)->animation_speed = 0x32u;
	}
}

void catapult_hold_up(void)
{
	uint32_t frame = 0, steps = 0;
	uint8_t animation_speed = 0, animation_timer = 0;

	animation_speed = OBJECT(current_object)->animation_speed;
	OBJECT(current_object)->animation_speed = animation_speed - 1u;
	if ((int8_t)animation_speed <= 1)
	{
		OBJECT(current_object)->catapult.state++;
		OBJECT(current_object)->flags |= 0x80u;
		OBJECT(current_object)->animation_timer = 0;
		OBJECT(current_object)->animation_speed = 4u;
		OBJECT(current_object)->animation_frame = 4u;
	}
	if ((int8_t)OBJECT(current_object)->animation_speed <= 0x1E)
	{
		frame = 4u;
		OBJECT(current_player)->animation_frame = 4u;
		steps = OBJECT(current_object)->animation + 2u;
		set_step_around_anchor(frame, steps);
	}
	else
	{
		animation_timer = OBJECT(current_object)->animation_timer;
		OBJECT(current_object)->animation_timer = animation_timer - 1u;
		if ((int8_t)animation_timer > 1)
			return;
		OBJECT(current_object)->animation_timer = 3;
		frame = 5u;
		if (OBJECT(current_object)->animation_frame != 4u)
			frame = 4u;
		OBJECT(current_object)->animation_frame = frame;
		steps = OBJECT(current_object)->animation + 2u;
		set_step_around_anchor(frame, steps);
	}
}

void catapult_swing_down(void)
{
	uint32_t frame = 0, steps = 0;

	animate_around_anchor();
	if ((int8_t)OBJECT(current_object)->animation_frame <= 1)
	{
		frame = 0u;
		steps = OBJECT(current_object)->animation + 2u;
		set_step_around_anchor(frame, steps);
		OBJECT(current_object)->animation_frame = 0u;
		OBJECT(current_object)->catapult.state = 0u;
		OBJECT(current_object)->kind = OBJECT(current_object)->catapult.launch_kind;
	}
}

void catapult_wait_for_scroll(void)
{
	if (scroll_position < OBJECT(current_object)->catapult.launch_position)
		return;
	catapult_launch();
}

void catapult_base_update(void)
{
	uint32_t r1 = 0, ignored = 0, object = 0, parent_z = 0;

	object = LINKED(current_object, OBJECT(current_object)->parent);
	parent_z = OBJECT(object)->z;
	OBJECT(current_object)->floor_z = parent_z;
	OBJECT(current_object)->z = parent_z;
	OBJECT(current_object)->animation_frame = OBJECT(object)->animation_frame;
	OBJECT(current_object)->animation_frame--;
	OBJECT(current_object)->animation_timer = 0;
	animate_around_anchor();
	update_screen_position(&ignored, &r1);
	if (OBJECT(current_object)->sprite_frame == 0u)
		return;
	r1 = queue_sprite(r1);
}

void catapult_dummy_update(void)
{
}

void goal_unlocker_start(void)
{
	goal_unlocked = 0xFFu;
	delete_current_object();
}
