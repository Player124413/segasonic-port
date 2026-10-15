/**
 *    Name:        objects/stage_10_props.c
 *    Description: stage 10: the propeller machine (the chaser), the barred gate, the scene anchor, the railing
 *    Version:     0.1.0
 *    Created:     12-10-2026
 *    Modified:    15-10-2026
 *    Author:      Sebastian Toyos
 *
 *    SegaSonic The Hedgehog (arcade, Sega System 32): the game's V60 program decompiled into C by
 *    tools/decompile.py, with the names of symbols.txt.
 */
#include "game.h"

void propeller_machine_init(void)
{
	uint32_t free_count = 0, parts = 0, zone_index = 0;

	OBJECT(current_object)->propeller_machine.path = propeller_machine_path_ADDRESS;
	OBJECT(current_object)->sprite_flags = 0xD004u;
	OBJECT(current_object)->kind = 0x18u;
	OBJECT(current_object)->flags = 0x40000002u;
	OBJECT(current_object)->hitbox_size_x = 0xF2u;
	OBJECT(current_object)->hitbox_size_y = 0x80u;
	OBJECT(current_object)->direction = 0x200u;
	OBJECT(current_object)->hitbox_height = 0xF0u;
	OBJECT(current_object)->sprite_offset_x = 0;
	OBJECT(current_object)->sprite_offset_y = -8;
	OBJECT(current_object)->hitbox_offset_x = -8;
	OBJECT(current_object)->hitbox_offset_y = -0x70;
	chaser_object = current_object;
	OBJECT(current_object)->propeller_machine.sound_timer = 0u;
	OBJECT(current_object)->animation = propeller_machine_animation_ADDRESS;
	OBJECT(current_object)->animation_frame = 0u;
	OBJECT(current_object)->animation_speed = 5u;
	OBJECT(current_object)->animation_timer = 0;
	OBJECT(current_object)->propeller_machine.holding_back = 0u;
	OBJECT(current_object)->propeller_machine.wait_timer = 0;
	OBJECT(current_object)->handler = propeller_machine_init_ADDRESS;
	take_free_object(&free_count, &parts);
	if (free_count != 0u)
	{
		move_halfwords(parts, propeller_machine_parts_ADDRESS, 0x2Du);
		OBJECT(current_object)->part_list = parts;
		zone_index = 0u;
		spawn_chaser_back_zone(zone_index, parts);
		OBJECT(current_object)->handler = propeller_machine_chase_ADDRESS;
	}
}

void propeller_machine_chase(void)
{
	uint32_t r1 = 0, animation = 0, cursor = 0, depth = 0, frame_entry = 0, ignored = 0, part_frame = 0, slope_factor = 0, sound_id = 0;
	uint8_t hold_timer = 0, animation_timer = 0;
	uint16_t wait_timer = 0, trigger_scroll = 0, trigger_y = 0, sound_timer = 0;
	uint32_t flags = 0;

	flags = OBJECT(current_object)->flags;
	OBJECT(current_object)->flags = flags | 0x80000000u;
	if ((flags & 0x80000000u) == 0u)
	{
	propeller_machine_read_script:
		wait_timer = OBJECT(current_object)->propeller_machine.wait_timer;
		OBJECT(current_object)->propeller_machine.wait_timer = wait_timer - 1u;
		if ((int16_t)wait_timer <= 1)
		{
			OBJECT(current_object)->propeller_machine.wait_timer = 0;
			cursor = OBJECT(current_object)->propeller_machine.path;
			trigger_scroll = mem_read16(cursor);
			cursor += 2u;
			if (trigger_scroll <= scroll_position)
			{
				cursor += 2u;
				goto propeller_machine_load_record;
			}
			trigger_y = mem_read16(cursor);
			cursor += 2u;
			if ((int16_t)trigger_y <= OBJECT(current_object)->y_int)
			{
			propeller_machine_load_record:
				if ((int16_t)mem_read16(cursor + 2u) < 0)
				{
					current_object = (current_object & ~0xFFFFu) | OBJECT(current_object)->sprite_frame;
					push_free_object();
					delete_current_object();
					return;
				}
				OBJECT(current_object)->propeller_machine.screen_threshold = mem_read16(cursor);
				cursor += 2u;
				OBJECT(current_object)->propeller_machine.catch_up_factor = mem_read16(cursor);
				cursor += 2u;
				OBJECT(current_object)->propeller_machine.base_speed = mem_read16(cursor);
				cursor += 2u;
				OBJECT(current_object)->propeller_machine.wait_timer = mem_read16(cursor);
				OBJECT(current_object)->propeller_machine.path = cursor + 2u;
			}
		}
	}
	else if (OBJECT(current_object)->propeller_machine.holding_back == 0u)
	{
		if (no_player_in_play == 0u && player_mishap == 0u && player_dropping_in == 0u)
			goto propeller_machine_read_script;
		OBJECT(current_object)->propeller_machine.holding_back = 0xFFu;
		OBJECT(current_object)->propeller_machine.saved_base_speed = OBJECT(current_object)->propeller_machine.base_speed;
		OBJECT(current_object)->propeller_machine.base_speed = 0;
		OBJECT(current_object)->propeller_machine.hold_start_position = scroll_position;
		OBJECT(current_object)->propeller_machine.hold_timer = 0xFFu;
		OBJECT(current_object)->propeller_machine.saved_threshold = OBJECT(current_object)->propeller_machine.screen_threshold;
		OBJECT(current_object)->propeller_machine.screen_threshold = -0x50;
	}
	if (OBJECT(current_object)->propeller_machine.holding_back != 0u)
	{
		if ((int16_t)(scroll_position - OBJECT(current_object)->propeller_machine.hold_start_position) >= 0x300)
		{
		propeller_machine_stop_holding_back:
			OBJECT(current_object)->propeller_machine.holding_back = 0u;
			OBJECT(current_object)->propeller_machine.screen_threshold = OBJECT(current_object)->propeller_machine.saved_threshold;
			OBJECT(current_object)->propeller_machine.base_speed = OBJECT(current_object)->propeller_machine.saved_base_speed;
		}
		else
		{
			if (no_player_in_play != 0u)
				OBJECT(current_object)->propeller_machine.hold_timer = 0xFFu;
			hold_timer = OBJECT(current_object)->propeller_machine.hold_timer;
			OBJECT(current_object)->propeller_machine.hold_timer = hold_timer - 1u;
			if (hold_timer == 0u)
				goto propeller_machine_stop_holding_back;
		}
	}
	if (OBJECT(current_object)->propeller_machine.wait_timer <= 0)
	{
		if (OBJECT(current_object)->propeller_machine.screen_threshold <= OBJECT(current_object)->screen_x)
		{
		propeller_machine_use_base_speed:
			r1 = (uint16_t)OBJECT(current_object)->propeller_machine.base_speed;
		}
		else
		{
			slope_factor = 0u;
			compute_camera_slope_speed(0u, slope_factor);
			r1 = (uint16_t)((uint32_t)camera_slope_speed_int * OBJECT(current_object)->propeller_machine.catch_up_factor) >> 8;
			if ((int16_t)r1 <= OBJECT(current_object)->propeller_machine.base_speed)
				goto propeller_machine_use_base_speed;
		}
		chaser_speed = r1;
		OBJECT(current_object)->y += (uint32_t)chaser_speed << 12;
	}
	animation_timer = OBJECT(current_object)->animation_timer;
	OBJECT(current_object)->animation_timer = animation_timer - 1u;
	if ((int8_t)animation_timer <= 1)
	{
		OBJECT(current_object)->animation_timer = OBJECT(current_object)->animation_speed;
		animation = OBJECT(current_object)->animation;
		r1 = OBJECT(current_object)->animation_frame;
		r1 = (uint16_t)(r1 + 1u);
		if ((int16_t)mem_read16(animation) < (int16_t)r1)
			r1 = 0u;
		OBJECT(current_object)->animation_frame = r1;
		frame_entry = animation + 2u + r1 * 4u;
		part_frame = mem_read16(frame_entry);
		mem_write16(OBJECT(current_object)->part_list + 0x22u, mem_read16(frame_entry + 2u));
		mem_write16(OBJECT(current_object)->part_list + 0x32u, part_frame);
		mem_write16(OBJECT(current_object)->part_list + 0x4Au, part_frame);
	}
	if (OBJECT(current_object)->y_int >= 0x1BB0)
		geyser_front_start = 0xFFu;
	if (OBJECT(current_object)->y_int >= 0x1B00)
		OBJECT(current_object)->hitbox_height = 0x68u;
	if (OBJECT(current_object)->screen_y < 0x130)
	{
		if (OBJECT(current_object)->screen_x < -0x80)
			OBJECT(current_object)->propeller_machine.sound_timer = 0u;
		else
		{
			sound_timer = OBJECT(current_object)->propeller_machine.sound_timer;
			OBJECT(current_object)->propeller_machine.sound_timer = sound_timer - 1u;
			if ((int16_t)sound_timer <= 1)
			{
				OBJECT(current_object)->propeller_machine.sound_timer = 0x3Cu;
				sound_id = 0xBu;
				play_sound((uint8_t)sound_id, &r1, &part_frame);
			}
		}
	}
	find_floor(&sound_id, &r1, &part_frame, &ignored, &frame_entry);
	OBJECT(current_object)->z = OBJECT(current_object)->floor_z;
	project_object(&ignored, &ignored);
	add_to_zone_list(ignored);
	depth = (uint16_t)(OBJECT(current_object)->screen_y - 0x40u);
	queue_sprite_at_depth(&ignored, &depth);
}

void barred_gate_init(void)
{
	uint32_t ignored = 0;

	OBJECT(current_object)->sprite_flags = 0x1008u;
	OBJECT(current_object)->sprite_frame = 0x9F00u;
	OBJECT(current_object)->sprite_palette = 0x92F0u;
	OBJECT(current_object)->sprite_offset_x = 0;
	OBJECT(current_object)->sprite_offset_y = -0x49;
	OBJECT(current_object)->flags = 0u;
	OBJECT(current_object)->animation = barred_gate_animation_ADDRESS;
	OBJECT(current_object)->animation_frame = 0u;
	OBJECT(current_object)->animation_speed = 2u;
	OBJECT(current_object)->animation_timer = 0;
	OBJECT(current_object)->z = 0x1580000;
	project_object(&ignored, &ignored);
	OBJECT(current_object)->anchor_x = OBJECT(current_object)->projected_x;
	OBJECT(current_object)->anchor_y = OBJECT(current_object)->projected_y_int;
	OBJECT(current_object)->handler = barred_gate_update_ADDRESS;
}

void barred_gate_update(void)
{
	uint8_t sound_id = 0;
	uint32_t r7 = 0, r8 = 0, depth = 0, ignored = 0, margin = 0, write_index_address = 0;

	margin = 0x50u;
	r8 = check_left_screen_margin((uint16_t)margin);
	if (r8 != 0u)
	{
		delete_current_object();
		return;
	}
	if ((int8_t)OBJECT(current_object)->animation_frame >= 0 && barred_gate_closing != 0u)
	{
		animate(&margin, &r7, &r8);
		if ((uint8_t)r8 != 0u)
		{
			OBJECT(current_object)->animation_frame = 0xFFu;
			sound_id = 0x29u;
			play_sound(sound_id, &margin, &write_index_address);
		}
	}
	update_screen_position(&ignored, &margin);
	depth = (uint16_t)(OBJECT(current_object)->screen_y - 0x40u);
	queue_sprite_at_depth(&ignored, &depth);
}

void scene_anchor_init(void)
{
	uint32_t record = 0;

	record = OBJECT(current_object)->sprite_frame * 8u + scene_anchor_positions_ADDRESS;
	OBJECT(current_object)->x_int = ROM16(record);
	OBJECT(current_object)->x_fraction = 0u;
	record += 2u;
	OBJECT(current_object)->y_int = ROM16(record);
	OBJECT(current_object)->y_fraction = 0u;
	record += 2u;
	OBJECT(current_object)->floor_z_int = ROM16(record);
	OBJECT(current_object)->floor_z_fraction = 0u;
	OBJECT(current_object)->scene_anchor.end_position = ROM16(record + 2u);
	OBJECT(current_object)->handler = scene_anchor_update_ADDRESS;
	scene_object = current_object;
}

void scene_anchor_update(void)
{
	uint32_t floor_z = 0, ignored = 0, point_x = 0, point_y = 0;

	if (OBJECT(current_object)->scene_anchor.end_position <= scroll_position)
	{
		delete_current_object();
		return;
	}
	point_x = OBJECT(current_object)->x;
	point_y = OBJECT(current_object)->y;
	floor_z = OBJECT(current_object)->floor_z;
	project_point(floor_z, &ignored, &point_x, &point_y);
	OBJECT(current_object)->z_int = 0x70u + view_y - (point_y + view_y - OBJECT(current_object)->floor_z_int) + 0x30u;
	OBJECT(current_object)->z_fraction = 0u;
}

void railing_spawner_init(void)
{
	OBJECT(current_object)->railing_spawner.last_spawn_position = 0u;
	OBJECT(current_object)->z = 0x5200000;
	OBJECT(current_object)->handler = railing_spawner_update_ADDRESS;
}

void railing_spawner_update(void)
{
	uint32_t ignored = 0, object = 0;
	uint16_t segments_left = 0;

	if (scroll_position >= 0x5000u)
	{
		delete_current_object();
		return;
	}
	if ((int16_t)(scroll_position - OBJECT(current_object)->railing_spawner.last_spawn_position) < 0x800 || free_object_stack < 0xA)
		return;
	OBJECT(current_object)->railing_spawner.last_spawn_position = scroll_position;
	railing_segments_left = 0xA;
	OBJECT(current_object)->y_int += railing_segments_left * 0xB8u;
	railing_segment_y = OBJECT(current_object)->y;
	do
	{
		allocate_object(&ignored, &object);
		OBJECT(object)->sprite_flags = 0xD002u;
		OBJECT(object)->part_list = railing_parts_ADDRESS;
		OBJECT(object)->flags = 0u;
		OBJECT(object)->sprite_offset_x = 0;
		OBJECT(object)->sprite_offset_y = 4;
		OBJECT(object)->handler = railing_segment_start_ADDRESS;
		OBJECT(object)->z = 0x5280000;
		railing_segment_y_int -= 0xB8u;
		OBJECT(object)->x = OBJECT(current_object)->x;
		OBJECT(object)->y = railing_segment_y;
		OBJECT(object)->z = OBJECT(current_object)->z;
		segments_left = railing_segments_left;
		railing_segments_left = segments_left - 1u;
	} while ((int16_t)segments_left > 1);
}

void railing_segment_start(void)
{
	uint32_t ignored = 0;

	project_object(&ignored, &ignored);
	OBJECT(current_object)->handler = railing_segment_draw_ADDRESS;
}

void railing_segment_draw(void)
{
	uint32_t depth = 0, ignored = 0, margin = 0, off_screen = 0;

	if (scroll_position >= 0x6700u)
	{
		delete_current_object();
		return;
	}
	margin = 0x88u;
	off_screen = check_left_screen_margin((uint16_t)margin);
	if (off_screen != 0u)
	{
		delete_current_object();
		return;
	}
	update_screen_position(&ignored, &margin);
	if ((uint16_t)(OBJECT(current_object)->screen_x + 0x88u) >= 0x1C8u)
		return;
	depth = (uint16_t)(OBJECT(current_object)->screen_y + 0x40u);
	queue_sprite_at_depth(&ignored, &depth);
}
