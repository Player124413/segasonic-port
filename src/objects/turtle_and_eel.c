/**
 *    Name:        objects/turtle_and_eel.c
 *    Description: object types 33-35 (stage 10): the lid trap, the turtle, the eel
 *    Version:     0.1.0
 *    Created:     12-10-2026
 *    Modified:    15-10-2026
 *    Author:      Sebastian Toyos
 *
 *    SegaSonic The Hedgehog (arcade, Sega System 32): the game's V60 program decompiled into C by
 *    tools/decompile.py, with the names of symbols.txt.
 */
#include "game.h"

void lid_trap_init(void)
{
	OBJECT(current_object)->sprite_frame = 0x8840u;
	OBJECT(current_object)->sprite_palette = 0x10F0u;
	OBJECT(current_object)->sprite_palette |= 0x4000u;
	OBJECT(current_object)->flags |= 2u;
	OBJECT(current_object)->hitbox_size_x = 0x26u;
	OBJECT(current_object)->hitbox_size_y = 0x26u;
	OBJECT(current_object)->hitbox_offset_x = 0x1A;
	OBJECT(current_object)->hitbox_offset_y = -0x44;
	OBJECT(current_object)->hitbox_height = 0u;
	OBJECT(current_object)->kind = 0x23u;
	OBJECT(current_object)->handler = lid_trap_update_ADDRESS;
	current_object = LINKED(current_object, OBJECT(current_object)->child);
	OBJECT(current_object)->sprite_frame = 0x8850u;
	OBJECT(current_object)->sprite_palette = 0x1100u;
	OBJECT(current_object)->sprite_palette |= 0x4000u;
}

void lid_trap_update(void)
{
	uint8_t sound_id = 0;
	uint16_t offset_x = 0, offset_y = 0;
	uint32_t r2 = 0, cursor = 0, deleted = 0, depth = 0, ignored = 0, slope_height = 0, write_index = 0;
	uint16_t direction = 0;

	unproject_object(&ignored, &r2, &slope_height, &cursor);
	delete_if_off_screen(&deleted, &r2, &slope_height);
	if ((uint8_t)deleted != 0u)
		return;
	if ((OBJECT(current_object)->flags & 4u) != 0u)
	{
		OBJECT(current_object)->kind = 5u;
		OBJECT(current_object)->sprite_frame = 0x8848u;
		OBJECT(current_object)->lid_trap.sprung = 1u;
		current_object = LINKED(current_object, OBJECT(current_object)->child);
		OBJECT(current_object)->sprite_palette = 0x1110u;
		sound_id = 0x11u;
		play_sound(sound_id, &write_index, &r2);
		current_object = object_cursor;
	}
	depth = (uint16_t)(OBJECT(current_object)->screen_y - 0x58u);
	queue_sprite_at_depth(&ignored, &depth);
	add_to_touchable_list(ignored);
	current_object = LINKED(current_object, OBJECT(current_object)->child);
	if (OBJECT(object_cursor)->lid_trap.sprung != 0u && (int16_t)AT16(OBJECT(current_object)->projected_y, 1) < 4)
	{
		direction = OBJECT(current_object)->direction;
		OBJECT(current_object)->direction = direction - 1u;
		if (direction == 0u)
		{
			OBJECT(current_object)->direction = 3u;
			OBJECT(current_object)->sprite_frame = lid_trap_lid_frames[lid_trap_close_steps[AT8(OBJECT(current_object)->projected_y, 1)]];
			AT8(OBJECT(current_object)->projected_y, 1)++;
		}
	}
	offset_x = 0u;
	offset_y = 0xFFF0u;
	place_sprite_near_cursor(offset_x, offset_y);
	depth = (uint16_t)(OBJECT(object_cursor)->screen_y - 0x20u);
	queue_sprite_at_depth(&ignored, &depth);
}

void turtle_init(void)
{
	OBJECT(current_object)->sprite_frame = 0x7F28u;
	OBJECT(current_object)->sprite_palette = 0x1040u;
	OBJECT(current_object)->sprite_palette |= 0x4000u;
	OBJECT(current_object)->flags |= 2u;
	OBJECT(current_object)->hitbox_size_x = 0x26u;
	OBJECT(current_object)->hitbox_size_y = 0x26u;
	OBJECT(current_object)->hitbox_offset_x = 0x1A;
	OBJECT(current_object)->hitbox_offset_y = -0x44;
	OBJECT(current_object)->hitbox_height = 0u;
	OBJECT(current_object)->kind = 0xCu;
	OBJECT(current_object)->handler = turtle_wait_ADDRESS;
	current_object = LINKED(current_object, OBJECT(current_object)->child);
	OBJECT(current_object)->sprite_frame = 0x7F70u;
	OBJECT(current_object)->sprite_palette = 0x1050u;
	OBJECT(current_object)->sprite_palette |= 0x4000u;
	current_object = LINKED(current_object, OBJECT(current_object)->child);
	OBJECT(current_object)->sprite_frame = 0x5D88u;
	OBJECT(current_object)->sprite_palette = 0x7A0u;
	OBJECT(current_object)->sprite_palette |= 0x4000u;
}

void turtle_wait(void)
{
	uint8_t bucket = 0;
	uint16_t offset_x = 0, offset_y = 0;
	uint32_t cursor = 0, depth = 0, ignored = 0;
	uint8_t head_step = 0;
	uint16_t direction = 0;

	if ((OBJECT(current_object)->flags & 4u) != 0u)
	{
		OBJECT(current_object)->turtle.stroke_timer = 0u;
		OBJECT(current_object)->turtle.stroke_step = 1u;
		OBJECT(current_object)->handler = turtle_surface_ADDRESS;
		turtle_surface();
		return;
	}
	unproject_object(&ignored, &ignored, &ignored, &cursor);
	depth = (uint16_t)(OBJECT(current_object)->screen_y - 0x20u);
	queue_sprite_at_depth(&ignored, &depth);
	add_to_touchable_list(ignored);
	current_object = LINKED(current_object, OBJECT(current_object)->child);
	if (OBJECT(object_cursor)->turtle.stroke_step != 4u)
		return;
	direction = OBJECT(current_object)->direction;
	OBJECT(current_object)->direction = direction - 1u;
	if (direction != 0u)
	{
		offset_x = 0xBu;
		offset_y = 0xFFEDu;
		place_sprite_near_cursor(offset_x, offset_y);
		depth = (uint16_t)(OBJECT(object_cursor)->screen_y - 0x1Fu);
		queue_sprite_at_depth(&ignored, &depth);
	}
	else
	{
		OBJECT(current_object)->direction = 4u;
		OBJECT(current_object)->sprite_frame = turtle_head_frames[turtle_head_steps[AT16(OBJECT(current_object)->projected_y, 1)]];
		AT8(OBJECT(current_object)->projected_y, 1)++;
		head_step = AT8(OBJECT(current_object)->projected_y, 1) & 0xFu;
		AT8(OBJECT(current_object)->projected_y, 1) = head_step;
		if (head_step != 0u)
		{
			offset_x = 0xBu;
			offset_y = 0xFFEDu;
			place_sprite_near_cursor(offset_x, offset_y);
			depth = (uint16_t)(OBJECT(object_cursor)->screen_y - 0x1Fu);
			queue_sprite_at_depth(&ignored, &depth);
			current_object = LINKED(current_object, OBJECT(current_object)->child);
			offset_x = 0u;
			offset_y = (uint16_t)(OBJECT(object_cursor)->floor_z_int - OBJECT(object_cursor)->z_int);
			place_sprite_near_cursor(offset_x, offset_y);
			bucket = 0xFFu;
			queue_sprite_in_bucket(bucket);
			return;
		}
		OBJECT(current_object)->direction = 0x1E0u;
	}
	current_object = LINKED(current_object, OBJECT(current_object)->child);
	offset_x = 0u;
	offset_y = (uint16_t)(OBJECT(object_cursor)->floor_z_int - OBJECT(object_cursor)->z_int);
	place_sprite_near_cursor(offset_x, offset_y);
	bucket = 0xFFu;
	queue_sprite_in_bucket(bucket);
}

void turtle_surface(void)
{
	OBJECT(current_object)->speed_z = 0u - ((OBJECT(current_object)->speed_z + 0x8000u) & 0x3FFFFu);
	if ((int16_t)(OBJECT(current_object)->floor_z_int - 0x20u) < OBJECT(current_object)->z_int)
	{
		turtle_swim();
		return;
	}
	OBJECT(current_object)->speed_z = 0;
	OBJECT(current_object)->handler = turtle_drop_rings_ADDRESS;
	turtle_drop_rings();
}

void turtle_drop_rings(void)
{
	uint32_t allocated = 0, random_value = 0, ring = 0;
	uint16_t spawn_arg_1 = 0;

	spawn_arg_1 = OBJECT(current_object)->spawn_arg_1;
	OBJECT(current_object)->spawn_arg_1 = spawn_arg_1 - 1u;
	if (spawn_arg_1 != 0u)
	{
		turtle_swim();
		return;
	}
	OBJECT(current_object)->spawn_arg_1 = 0x30u;
	OBJECT(current_object)->spawn_arg_2++;
	if ((int16_t)OBJECT(current_object)->spawn_arg_2 > 4)
		OBJECT(current_object)->handler = turtle_swim_ADDRESS;
	released_ring_flip = 2u;
	released_rings_random = 0u;
	released_ring_angle = 0x154u;
	rings_to_release = 1u;
	drop_rings(0u, &allocated, &random_value, &ring);
	turtle_swim();
}

void turtle_swim(void)
{
	uint16_t offset_x = 0, offset_y = 0, shadow_offset_y = 0;
	uint32_t block_offset = 0, bucket = 0, cursor = 0, deleted = 0, direction = 0, height_above_floor = 0, ignored = 0, slope_height = 0, speed = 0, speed_x = 0, speed_y = 0, stroke_step = 0;
	uint8_t wrapped_stroke_step = 0, wrapped_head_step = 0;
	uint16_t stroke_timer = 0, direction_2 = 0;

	stroke_timer = OBJECT(current_object)->turtle.stroke_timer;
	OBJECT(current_object)->turtle.stroke_timer = stroke_timer - 1u;
	if (stroke_timer == 0u)
	{
		OBJECT(current_object)->turtle.stroke_timer = 2u;
		stroke_step = OBJECT(current_object)->turtle.stroke_step;
		OBJECT(current_object)->sprite_frame = turtle_swim_frames[stroke_step];
		OBJECT(current_object)->speed = turtle_swim_speeds[stroke_step];
		OBJECT(current_object)->turtle.stroke_step++;
		wrapped_stroke_step = OBJECT(current_object)->turtle.stroke_step & 0xFu;
		OBJECT(current_object)->turtle.stroke_step = wrapped_stroke_step;
		if (wrapped_stroke_step == 0u)
			OBJECT(current_object)->turtle.stroke_timer = 0x3Cu;
	}
	OBJECT(current_object)->speed -= 3u;
	direction = 0x14Cu;
	speed = OBJECT(current_object)->speed + 0xE0u;
	direction_to_speed(speed, &direction, &speed_x, &speed_y);
	OBJECT(current_object)->speed_x = speed_x;
	OBJECT(current_object)->speed_y = speed_y;
	move_object_projected(&ignored, &block_offset, &slope_height, &cursor);
	delete_if_off_screen(&deleted, &block_offset, &slope_height);
	if ((uint8_t)deleted != 0u)
		return;
	current_object = LINKED(current_object, OBJECT(current_object)->child);
	direction_2 = OBJECT(current_object)->direction;
	OBJECT(current_object)->direction = direction_2 - 1u;
	if (direction_2 == 0u)
	{
		OBJECT(current_object)->direction = 1u;
		OBJECT(current_object)->sprite_frame = turtle_head_frames[turtle_head_steps[AT16(OBJECT(current_object)->projected_y, 1)]];
		AT8(OBJECT(current_object)->projected_y, 1)++;
		wrapped_head_step = AT8(OBJECT(current_object)->projected_y, 1) & 0xFu;
		AT8(OBJECT(current_object)->projected_y, 1) = wrapped_head_step;
		if (wrapped_head_step == 0u)
			OBJECT(current_object)->direction = 0x78u;
	}
	offset_x = 0xBu;
	offset_y = 0xFFEDu;
	place_sprite_near_cursor(offset_x, offset_y);
	height_above_floor = (uint16_t)(OBJECT(object_cursor)->floor_z_int - OBJECT(object_cursor)->z_int);
	r14 = height_above_floor;
	height_above_floor = (uint16_t)(height_above_floor + OBJECT(current_object)->screen_y + 0x13u + 1u);
	queue_sprite_at_depth(&bucket, &height_above_floor);
	if (OBJECT(object_cursor)->floor_z_int < 0x800)
	{
		current_object = LINKED(current_object, OBJECT(current_object)->child);
		offset_x = 0u;
		shadow_offset_y = (uint16_t)r14;
		place_sprite_near_cursor(offset_x, shadow_offset_y);
		OBJECT(current_object)->sprite_frame = OBJECT(object_cursor)->sprite_frame;
		OBJECT(current_object)->sprite_scale = 0x100u;
		OBJECT(current_object)->sprite_scale -= shadow_offset_y;
		bucket = 0xFFu;
		queue_sprite_in_bucket(bucket);
	}
	current_object = object_cursor;
	queue_sprite_at_floor();
	add_to_touchable_list(bucket);
}

void eel_init(void)
{
	uint16_t explode_delay_step = 0;
	uint32_t block = 0, free_count = 0, object = 0, object_2 = 0, piece_count = 0, segment_amplitude = 0, segments_left = 0, spawned = 0;

	OBJECT(current_object)->handler = eel_anchor_update_ADDRESS;
	OBJECT(current_object)->sprite_flags = 0u;
	OBJECT(current_object)->sprite_frame = 0xA898u;
	OBJECT(current_object)->sprite_palette = 0x1210u;
	OBJECT(current_object)->sprite_palette |= 0x4000u;
	OBJECT(current_object)->hitbox_size_x = 8u;
	OBJECT(current_object)->hitbox_height = 0xAu;
	OBJECT(current_object)->kind = 0x24u;
	OBJECT(current_object)->eel_init.stretch_step = 0x10;
	OBJECT(current_object)->speed = 0x480;
	OBJECT(current_object)->direction = 0x34Au;
	if (OBJECT(current_object)->eel_init.facing != 0u)
		OBJECT(current_object)->direction = 0x14Au;
	current_object = LINKED(current_object, OBJECT(current_object)->child);
	OBJECT(current_object)->sprite_flags = 0u;
	OBJECT(current_object)->sprite_palette = 0x1200u;
	OBJECT(current_object)->sprite_palette |= 0x4000u;
	current_object = LINKED(current_object, OBJECT(current_object)->child);
	OBJECT(current_object)->sprite_flags = 0u;
	OBJECT(current_object)->sprite_palette = 0x12E0u;
	OBJECT(current_object)->sprite_palette |= 0x4000u;
	current_object = LINKED(current_object, OBJECT(current_object)->child);
	OBJECT(current_object)->sprite_flags = 0u;
	OBJECT(current_object)->sprite_frame = 0xA898u;
	OBJECT(current_object)->sprite_palette = 0x7A0u;
	OBJECT(current_object)->sprite_palette |= 0x4000u;
	r14 = 0x30u;
	explode_delay_step = 0x30u;
	segments_left = OBJECT(object_cursor)->eel_init.piece_count;
	segment_amplitude = OBJECT(object_cursor)->eel_init.wave_step;
	current_player = current_object;
	spawn_count = 4u;
	spawn_object_with_pieces(0x014A32u, &spawned, &piece_count, &free_count, &object, &block);
	for (;;)
	{
		OBJECT(current_player)->child = object;
		copy_position_to(object);
		OBJECT(object)->sprite_flags = 0u;
		OBJECT(object)->sprite_frame = 0xA898u;
		OBJECT(object)->sprite_palette = 0x1210u;
		OBJECT(object)->sprite_palette |= 0x4000u;
		OBJECT(object)->eel_segment.explode_delay = r14;
		OBJECT(object)->hitbox_size_x = 0x10u;
		OBJECT(object)->hitbox_height = 0x10u;
		OBJECT(object)->kind = 0x24u;
		OBJECT(object)->spawn_arg_1 = segment_amplitude;
		OBJECT(object)->spawn_arg_2 = OBJECT(object_cursor)->spawn_arg_1;
		OBJECT(object)->eel_segment.height_offset = 0x4000u * segment_amplitude;
		OBJECT(object)->handler = eel_segment_move_ADDRESS;
		object_2 = object;
		object = LINKED(object, OBJECT(object)->child);
		OBJECT(object)->sprite_flags = 0u;
		OBJECT(object)->sprite_palette = 0x1200u;
		OBJECT(object)->sprite_palette |= 0x4000u;
		object = LINKED(object, OBJECT(object)->child);
		OBJECT(object)->sprite_flags = 0u;
		OBJECT(object)->sprite_palette = 0x12E0u;
		OBJECT(object)->sprite_palette |= 0x4000u;
		AT8(OBJECT(object)->projected_y, 1) = (uint8_t)segment_amplitude % 0x1Fu;
		object = LINKED(object, OBJECT(object)->child);
		OBJECT(object)->sprite_flags = 0u;
		OBJECT(object)->sprite_frame = 0xA898u;
		OBJECT(object)->sprite_palette = 0x7A0u;
		OBJECT(object)->sprite_palette |= 0x4000u;
		current_player = object;
		r14 = (uint16_t)(r14 + explode_delay_step);
		segment_amplitude = (uint16_t)(segment_amplitude + OBJECT(current_object)->eel_init.wave_step);
		segments_left--;
		if (segments_left == 0u)
			break;
		spawn_count = 4u;
		spawn_object_with_pieces(0x014A3Fu, &spawned, &piece_count, &free_count, &object, &block);
	}
	OBJECT(object_2)->sprite_frame = 0xAA38u;
	OBJECT(object_2)->sprite_palette = 0x1210u;
	OBJECT(object_2)->sprite_palette |= 0x4000u;
	OBJECT(object_2)->handler = eel_head_move_ADDRESS;
	object_2 = LINKED(object_2, OBJECT(object_2)->child);
	OBJECT(object_2)->sprite_flags = 0u;
	OBJECT(object_2)->sprite_frame = 0xAA38u;
	OBJECT(object_2)->sprite_palette = 0x7A0u;
	OBJECT(object_2)->sprite_palette |= 0x4000u;
}

void eel_anchor_update(void)
{
	uint32_t block_offset = 0, bucket = 0, cursor = 0, deleted = 0, ignored = 0, segment_speed = 0, slope_height = 0, speed_step = 0, stretch_step = 0;
	uint16_t pause_timer = 0;

	if ((stage_trigger_bits & shl32(1u, (int)(OBJECT(current_object)->eel_anchor.den_number))) != 0u)
	{
		OBJECT(current_object)->handler = eel_anchor_wait_explode_ADDRESS;
		eel_anchor_wait_explode();
		return;
	}
	unproject_object(&ignored, &block_offset, &slope_height, &cursor);
	delete_if_off_screen(&deleted, &block_offset, &slope_height);
	if ((uint8_t)deleted != 0u)
		return;
	queue_sprite_at_floor();
	add_to_touchable_list(deleted);
	if (OBJECT(current_object)->eel_anchor.pausing == 0u)
	{
	eel_anchor_stretch:
		stretch_step = OBJECT(current_object)->eel_anchor.stretch_step;
		OBJECT(current_object)->speed += stretch_step;
		if ((int32_t)stretch_step >= 0)
		{
			if (OBJECT(current_object)->speed > 0x1111)
			{
				OBJECT(current_object)->eel_anchor.pause_timer = 0x3Cu;
				OBJECT(current_object)->eel_anchor.pausing = 1u;
				OBJECT(current_object)->eel_anchor.stretch_step = -0x18;
			}
		}
		else if (OBJECT(current_object)->speed < 0x480)
		{
			OBJECT(current_object)->eel_anchor.pause_timer = 0x3Cu;
			OBJECT(current_object)->eel_anchor.pausing = 1u;
			OBJECT(current_object)->eel_anchor.stretch_step = 0x28;
		}
		segment_speed = OBJECT(current_object)->speed;
		speed_step = segment_speed;
		current_object = LINKED(current_object, OBJECT(object_cursor)->child);
		current_object = LINKED(current_object, OBJECT(current_object)->child);
		current_object = LINKED(current_object, OBJECT(current_object)->child);
		for (;;)
		{
			current_object = LINKED(current_object, OBJECT(current_object)->child);
			OBJECT(current_object)->direction = OBJECT(object_cursor)->direction;
			OBJECT(current_object)->speed = segment_speed;
			OBJECT(current_object)->var_81 = OBJECT(object_cursor)->eel_anchor.stretch_step;
			current_object = LINKED(current_object, OBJECT(current_object)->child);
			current_object = LINKED(current_object, OBJECT(current_object)->child);
			current_object = LINKED(current_object, OBJECT(current_object)->child);
			if (OBJECT(current_object)->child == 0xFFFFu)
				break;
			segment_speed += speed_step;
		}
	}
	else
	{
		pause_timer = OBJECT(current_object)->eel_anchor.pause_timer;
		OBJECT(current_object)->eel_anchor.pause_timer = pause_timer - 1u;
		if (pause_timer == 0u)
		{
			OBJECT(current_object)->eel_anchor.pausing = 0u;
			goto eel_anchor_stretch;
		}
	}
	current_object = object_cursor;
	current_object = LINKED(current_object, OBJECT(current_object)->child);
	current_object = LINKED(current_object, OBJECT(current_object)->child);
	current_object = LINKED(current_object, OBJECT(current_object)->child);
	bucket = place_sprite_at_cursor_floor();
	queue_sprite_in_bucket(bucket);
}

void eel_anchor_wait_explode(void)
{
	uint32_t block_offset = 0, distance_y = 0, ignored = 0, slope_height = 0;

	if (OBJECT(current_object)->eel_anchor.piece_count <= 0)
		delete_object_and_children(&ignored, &block_offset, &slope_height);
	if (OBJECT(current_object)->eel_anchor.facing == 0u)
	{
		r14 = LINKED(WORK_RAM, chaser_object);
		distance_xy_to_object(&ignored, &distance_y);
		if ((int32_t)distance_y <= 0x10)
			stage_trigger_bits |= shl32(1u, (int)(OBJECT(current_object)->eel_anchor.den_number));
	}
}

void eel_anchor_wait_explode_spare_return(void)
{
}

void eel_head_move(void)
{
	uint16_t explosion_delay = 0, head_offset_x = 0, head_offset_y = 0;
	uint32_t bucket = 0, cursor = 0, ignored = 0;
	uint16_t frame_timer = 0, frame_timer_2 = 0;

	if ((stage_trigger_bits & shl32(1u, (int)(OBJECT(current_object)->eel_head.den_number))) == 0u)
	{
		place_eel_piece();
		if (OBJECT(current_object)->eel_head.facing != 0u)
		{
			head_offset_x = 0xBu;
			head_offset_y = 0u;
			frame_timer = OBJECT(current_object)->eel_head.frame_timer;
			OBJECT(current_object)->eel_head.frame_timer = frame_timer - 1u;
			if (frame_timer == 0u)
			{
				OBJECT(current_object)->eel_head.frame_timer = 3u;
				if (OBJECT(current_object)->eel_head.stretch_step < 0)
				{
					OBJECT(current_object)->sprite_frame = eel_head_retract_frames[(uint8_t)OBJECT(current_object)->eel_head.frame_index];
					OBJECT(current_object)->eel_head.frame_index++;
					if (OBJECT(current_object)->eel_head.frame_index > 3)
					{
						OBJECT(current_object)->eel_head.frame_index = 0;
						OBJECT(current_object)->eel_head.frame_timer = 0x3Cu;
					}
				}
				else
				{
					OBJECT(current_object)->eel_head.frame_index = (uint8_t)OBJECT(current_object)->eel_head.frame_index % 3u;
					OBJECT(current_object)->sprite_frame = eel_head_extend_frames[(uint8_t)OBJECT(current_object)->eel_head.frame_index];
					OBJECT(current_object)->eel_head.frame_index++;
					if (OBJECT(current_object)->eel_head.frame_index > 2)
					{
						OBJECT(current_object)->eel_head.frame_index = 0;
						OBJECT(current_object)->eel_head.frame_timer = 0x3Cu;
					}
				}
			}
		}
		else
		{
			head_offset_x = 0xFFF4u;
			head_offset_y = 3u;
			frame_timer_2 = OBJECT(current_object)->eel_head.frame_timer;
			OBJECT(current_object)->eel_head.frame_timer = frame_timer_2 - 1u;
			if (frame_timer_2 == 0u)
			{
				OBJECT(current_object)->eel_head.frame_timer = 3u;
				OBJECT(current_object)->sprite_frame = eel_head_frames[(uint8_t)OBJECT(current_object)->eel_head.frame_index];
				OBJECT(current_object)->eel_head.frame_index++;
				if (OBJECT(current_object)->eel_head.frame_index > 2)
				{
					OBJECT(current_object)->eel_head.frame_index = 0;
					OBJECT(current_object)->eel_head.frame_timer = 0x3Cu;
				}
			}
		}
		OBJECT(current_object)->projected_x += head_offset_x;
		OBJECT(current_object)->projected_y_int += head_offset_y;
		move_object_projected(&ignored, &ignored, &ignored, &cursor);
		queue_sprite_at_floor();
		current_object = LINKED(current_object, OBJECT(current_object)->child);
		bucket = place_sprite_at_cursor_floor();
		queue_sprite_in_bucket(bucket);
		return;
	}
	current_object = LINKED(current_object, OBJECT(current_object)->parent);
	explosion_delay = (uint16_t)(divide_s32((uint32_t)OBJECT(object_cursor)->speed, (uint32_t)OBJECT(current_object)->speed) * 0xBu);
	current_object = object_cursor;
	OBJECT(current_object)->eel_head.explode_delay = explosion_delay;
	OBJECT(current_object)->handler = eel_head_explode_delay_ADDRESS;
	eel_head_explode_delay();
}

void eel_segment_move(void)
{
	uint16_t explosion_delay = 0;
	uint32_t bucket = 0, cursor = 0, depth = 0, ignored = 0;

	if ((stage_trigger_bits & shl32(1u, (int)(OBJECT(current_object)->eel_segment.den_number))) == 0u)
	{
		place_eel_piece();
		move_object_projected(&ignored, &ignored, &ignored, &cursor);
		depth = queue_sprite_at_floor();
		r14 = depth;
		add_to_touchable_list(ignored);
		current_object = LINKED(current_object, OBJECT(current_object)->child);
		current_object = LINKED(current_object, OBJECT(current_object)->child);
		current_object = LINKED(current_object, OBJECT(current_object)->child);
		bucket = place_sprite_at_cursor_floor();
		queue_sprite_in_bucket(bucket);
		return;
	}
	current_object = LINKED(current_object, OBJECT(current_object)->parent);
	explosion_delay = (uint16_t)(divide_s32((uint32_t)OBJECT(object_cursor)->speed, (uint32_t)OBJECT(current_object)->speed) * 0xBu);
	current_object = object_cursor;
	OBJECT(current_object)->eel_segment.explode_delay = explosion_delay;
	OBJECT(current_object)->handler = eel_segment_explode_delay_ADDRESS;
	eel_segment_explode_delay();
}

void draw_eel_piece_parts(void)
{
	uint32_t bucket = 0, depth = 0, floor_depth = 0, ignored = 0;
	uint16_t part_2_timer = 0, part_3_timer = 0;

	current_object = LINKED(current_object, OBJECT(current_object)->child);
	part_2_timer = OBJECT(current_object)->direction;
	OBJECT(current_object)->direction = part_2_timer - 1u;
	if (part_2_timer == 0u)
	{
		OBJECT(current_object)->direction = 2u;
		OBJECT(current_object)->sprite_frame = eel_part_frames_2[eel_part_steps[AT8(OBJECT(current_object)->projected_y, 1)]];
		AT8(OBJECT(current_object)->projected_y, 1)++;
		AT8(OBJECT(current_object)->projected_y, 1) = AT8(OBJECT(current_object)->projected_y, 1) % 0x18u;
	}
	OBJECT(current_object)->screen_x = OBJECT(object_cursor)->screen_x;
	OBJECT(current_object)->screen_y = OBJECT(object_cursor)->screen_y;
	floor_depth = (uint16_t)(AS32(OBJECT(object_cursor)->projected_y_int) + AS32(OBJECT(object_cursor)->floor_z_int) - view_y + 1u);
	r14 = floor_depth;
	queue_sprite_at_depth(&ignored, &floor_depth);
	current_object = LINKED(current_object, OBJECT(current_object)->child);
	part_3_timer = OBJECT(current_object)->direction;
	OBJECT(current_object)->direction = part_3_timer - 1u;
	if (part_3_timer == 0u)
	{
		OBJECT(current_object)->direction = 2u;
		OBJECT(current_object)->sprite_frame = eel_part_frames[AT8(OBJECT(current_object)->projected_y, 1)];
		AT8(OBJECT(current_object)->projected_y, 1)++;
		AT8(OBJECT(current_object)->projected_y, 1) = AT8(OBJECT(current_object)->projected_y, 1) % 0x1Fu;
	}
	copy_screen_position_from_cursor();
	depth = (uint16_t)r14;
	queue_sprite_at_depth(&ignored, &depth);
	current_object = LINKED(current_object, OBJECT(current_object)->child);
	bucket = place_sprite_at_cursor_floor();
	queue_sprite_in_bucket(bucket);
}

void place_eel_piece(void)
{
	uint32_t direction = 0, speed = 0, wave_offset = 0;

	current_object = LINKED(current_object, OBJECT(object_cursor)->parent);
	OBJECT(object_cursor)->projected_x = OBJECT(current_object)->projected_x;
	OBJECT(object_cursor)->projected_y = OBJECT(current_object)->projected_y;
	OBJECT(object_cursor)->z = OBJECT(current_object)->z;
	OBJECT(object_cursor)->floor_z = OBJECT(current_object)->floor_z;
	OBJECT(object_cursor)->z -= OBJECT(object_cursor)->place_eel_piece.height_offset;
	current_object = object_cursor;
	direction = OBJECT(current_object)->direction;
	speed = OBJECT(current_object)->speed;
	OBJECT(current_object)->speed_x = (int32_t)(sine_table[direction] * speed) >> 6;
	OBJECT(current_object)->speed_y = 0u - ((int32_t)(cosine_table[direction] * speed) >> 6);
	OBJECT(current_object)->place_eel_piece.wave_angle += 8u;
	OBJECT(current_object)->place_eel_piece.wave_angle &= 0x3FFu;
	wave_offset = (uint32_t)sine_table[OBJECT(current_object)->place_eel_piece.wave_angle] * OBJECT(current_object)->place_eel_piece.wave_amplitude;
	OBJECT(current_object)->z += wave_offset;
	OBJECT(current_object)->speed_y += wave_offset * 2u;
}

void eel_head_explode_delay(void)
{
	uint32_t bucket = 0, cursor = 0, ignored = 0;
	uint16_t explode_delay = 0;

	explode_delay = OBJECT(current_object)->eel_head.explode_delay;
	OBJECT(current_object)->eel_head.explode_delay = explode_delay - 1u;
	if (explode_delay == 0u)
	{
		eel_piece_start_explosion();
		return;
	}
	unproject_object(&ignored, &ignored, &ignored, &cursor);
	queue_sprite_at_floor();
	current_object = LINKED(current_object, OBJECT(current_object)->child);
	bucket = place_sprite_at_cursor_floor();
	queue_sprite_in_bucket(bucket);
}

void eel_segment_explode_delay(void)
{
	uint32_t bucket = 0, cursor = 0, ignored = 0;
	uint16_t explode_delay = 0;

	explode_delay = OBJECT(current_object)->eel_segment.explode_delay;
	OBJECT(current_object)->eel_segment.explode_delay = explode_delay - 1u;
	if (explode_delay == 0u)
	{
		eel_piece_start_explosion();
		return;
	}
	unproject_object(&ignored, &ignored, &ignored, &cursor);
	queue_sprite_at_floor();
	current_object = LINKED(current_object, OBJECT(current_object)->child);
	current_object = LINKED(current_object, OBJECT(current_object)->child);
	current_object = LINKED(current_object, OBJECT(current_object)->child);
	bucket = place_sprite_at_cursor_floor();
	queue_sprite_in_bucket(bucket);
}

void eel_piece_start_explosion(void)
{
	uint8_t sound_id = 0;
	uint32_t write_index = 0, write_index_address = 0;

	OBJECT(current_object)->projected_y_int += 0x18u;
	OBJECT(current_object)->eel_piece.frame_timer = 0u;
	OBJECT(current_object)->eel_piece.frame_index = 2;
	OBJECT(current_object)->sprite_palette = 0x1070u;
	OBJECT(current_object)->sprite_palette |= 0x4000u;
	OBJECT(current_object)->sprite_flags = 4u;
	OBJECT(current_object)->handler = eel_piece_explode_ADDRESS;
	sound_id = 0x24u;
	play_sound(sound_id, &write_index, &write_index_address);
	eel_piece_explode();
}

void eel_piece_explode(void)
{
	uint8_t sound_id = 0;
	uint32_t cursor = 0, depth = 0, ignored = 0, write_index = 0, write_index_address = 0;
	uint16_t frame_timer = 0;

	frame_timer = OBJECT(current_object)->eel_piece.frame_timer;
	OBJECT(current_object)->eel_piece.frame_timer = frame_timer - 1u;
	if (frame_timer == 0u)
	{
		OBJECT(current_object)->eel_piece.frame_timer = 2u;
		OBJECT(current_object)->sprite_frame = underwater_explosion_frames[(uint8_t)OBJECT(current_object)->eel_piece.frame_index];
		OBJECT(current_object)->eel_piece.frame_index++;
		if (OBJECT(current_object)->eel_piece.frame_index > 0x15)
		{
			current_object = LINKED(current_object, OBJECT(current_object)->parent);
			AS8(OBJECT(current_object)->spawn_arg_2)--;
			current_object = object_cursor;
			OBJECT(current_object)->handler = eel_piece_gone_ADDRESS;
			sound_id = 0x24u;
			play_sound(sound_id, &write_index, &write_index_address);
		}
	}
	unproject_object(&ignored, &ignored, &ignored, &cursor);
	depth = (uint16_t)(OBJECT(current_object)->screen_y - 0x10u);
	queue_sprite_at_depth(&ignored, &depth);
	eel_piece_gone();
}

void eel_piece_gone(void)
{
}
