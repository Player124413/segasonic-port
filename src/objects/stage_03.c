/**
 *    Name:        objects/stage_03.c
 *    Description: stage 3's objects: the spiked panels and the electric poles, the crate pushed to a post, the breakable panel, the spiked roller that chases the players (also in stage E) and the floor that crumbles at the finale, the machine at 8300 with its fire and fireballs, the fire vents, the end pillar
 *    Version:     0.1.0
 *    Created:     12-10-2026
 *    Modified:    15-10-2026
 *    Author:      Sebastian Toyos
 *
 *    SegaSonic The Hedgehog (arcade, Sega System 32): the game's V60 program decompiled into C by
 *    tools/decompile.py, with the names of symbols.txt.
 */
#include "game.h"

void spike_panel_init(void)
{
	uint32_t block = 0, first = 0, object = 0, parts_left = 0, spawned = 0;

	spawn_count = 4u;
	spawn_objects(0x00F092u, &spawned, &first, &block);
	if ((uint8_t)spawned == 0u)
		return;
	OBJECT(current_object)->spike_panel.step = OBJECT(current_object)->spike_panel.start_step;
	OBJECT(current_object)->spike_panel.step_timer = OBJECT(current_object)->spawn_arg_2;
	OBJECT(current_object)->spike_panel.trigger_delay = 0x1Eu;
	OBJECT(current_object)->child = first;
	OBJECT(current_object)->sprite_palette = 0x9020u;
	OBJECT(current_object)->sprite_flags = 6u;
	OBJECT(current_object)->flags |= 2u;
	OBJECT(current_object)->hitbox_offset_y = -0x16;
	OBJECT(current_object)->hitbox_height = 0x3Cu;
	OBJECT(current_object)->kind = 0xCu;
	OBJECT(current_object)->handler = spike_panel_hold_ADDRESS;
	parts_left = 2u;
	do
	{
		current_object = LINKED(current_object, OBJECT(current_object)->child);
		OBJECT(current_object)->z = OBJECT(object_cursor)->z;
		OBJECT(current_object)->floor_z = OBJECT(object_cursor)->floor_z;
		OBJECT(current_object)->sprite_palette = 0x9020u;
		OBJECT(current_object)->flags |= 2u;
		OBJECT(current_object)->hitbox_offset_y = -0x1A;
		OBJECT(current_object)->hitbox_height = 0x3Cu;
		OBJECT(current_object)->kind = 0xCu;
		OBJECT(current_object)->handler = spike_panel_part_idle_ADDRESS;
		parts_left--;
	} while (parts_left != 0u);
	OBJECT(current_object)->sprite_flags = 6u;
	OBJECT(current_object)->projected_x = OBJECT(object_cursor)->projected_x;
	OBJECT(current_object)->projected_y = OBJECT(object_cursor)->projected_y;
	OBJECT(current_object)->projected_x -= 0x7Bu;
	OBJECT(current_object)->projected_y_int -= 0x3Cu;
	current_object = LINKED(current_object, OBJECT(current_object)->child);
	OBJECT(current_object)->direction = 0x300u;
	OBJECT(current_object)->flags |= 2u;
	OBJECT(current_object)->hitbox_size_x = 0x8Du;
	OBJECT(current_object)->hitbox_size_y = 0xBCu;
	OBJECT(current_object)->hitbox_offset_x = 0;
	OBJECT(current_object)->hitbox_offset_y = 0;
	OBJECT(current_object)->hitbox_height = 0x3Cu;
	OBJECT(current_object)->kind = 0x12u;
	OBJECT(current_object)->handler = spike_panel_hitbox_update_ADDRESS;
	object = LINKED(first, OBJECT(current_object)->child);
	OBJECT(current_object)->child = 0xFFFFu;
	copy_position_to(object);
	OBJECT(object)->projected_x -= 0x7Cu;
	OBJECT(object)->projected_y_int += 0x38u;
	OBJECT(object)->sprite_frame = 0x6B58u;
	OBJECT(object)->sprite_palette = 0x11E0u;
	OBJECT(object)->handler = spike_panel_base_draw_ADDRESS;
}

void spike_panel_hold(void)
{
	uint16_t trigger_delay = 0;

	if (OBJECT(current_object)->spike_panel.start_step != 0u)
	{
		find_nearest_player_x();
		if (nearest_player_distance > 0x40)
		{
			OBJECT(current_object)->spike_panel.step_timer = 0;
			OBJECT(current_object)->spike_panel.step = OBJECT(current_object)->spike_panel.start_step;
			spike_panel_update();
			return;
		}
	}
	trigger_delay = OBJECT(current_object)->spike_panel.trigger_delay;
	OBJECT(current_object)->spike_panel.trigger_delay = trigger_delay - 1u;
	if (trigger_delay == 0u)
	{
		OBJECT(current_object)->spike_panel.start_step = 0u;
		OBJECT(current_object)->handler = spike_panel_update_ADDRESS;
		spike_panel_update();
		return;
	}
	OBJECT(current_object)->spike_panel.step_timer = 0;
	OBJECT(current_object)->spike_panel.step = OBJECT(current_object)->spike_panel.start_step;
	spike_panel_update();
}

void spike_panel_update(void)
{
	uint8_t bucket = 0, sound_id = 0;
	uint16_t count = 0;
	uint32_t r0 = 0, r2 = 0, r11 = 0, block_offset = 0, deleted = 0, depth = 0, frame_index = 0, ignored = 0, offset_record = 0, part_2_frame = 0, slope_height = 0;

	unproject_object(&ignored, &block_offset, &slope_height, &r11);
	delete_if_off_screen(&deleted, &block_offset, &slope_height);
	if ((uint8_t)deleted != 0u)
		return;
	r14 = LINKED(WORK_RAM, chaser_object);
	r0 = face_object();
	if ((int16_t)OBJECT(current_object)->direction <= 0x100 || (int16_t)OBJECT(current_object)->direction >= 0x378)
	{
		OBJECT(current_object)->spike_panel.step_timer--;
		if (OBJECT(current_object)->spike_panel.step_timer < 0 && OBJECT(current_object)->spike_panel.start_step == 0u)
		{
			OBJECT(current_object)->spike_panel.step++;
			OBJECT(current_object)->spike_panel.step_timer = spike_panel_step_durations[(uint8_t)OBJECT(current_object)->spike_panel.step];
			if (OBJECT(current_object)->spike_panel.step > 8)
			{
				OBJECT(current_object)->spike_panel.step = 0;
				OBJECT(current_object)->spike_panel.step_timer = OBJECT(current_object)->spawn_arg_2;
			}
		}
		frame_index = spike_panel_frame_sequence[(uint8_t)OBJECT(current_object)->spike_panel.step];
		OBJECT(current_object)->spike_panel.frame_index = frame_index;
		offset_record = spike_panel_head_offsets_ADDRESS + frame_index * 4u;
		OBJECT(current_object)->screen_x += ROM16(offset_record);
		OBJECT(current_object)->screen_y += ROM16(offset_record + 2u);
		OBJECT(current_object)->sprite_frame = spike_panel_head_sprites[frame_index];
		if ((int8_t)frame_index >= 5)
		{
			OBJECT(current_object)->hitbox_size_x = 0x65u;
			OBJECT(current_object)->hitbox_size_y = 0x30u;
			OBJECT(current_object)->hitbox_offset_x = -0x11;
		}
		else
		{
			OBJECT(current_object)->hitbox_size_x = 0x54u;
			OBJECT(current_object)->hitbox_size_y = 0x30u;
			OBJECT(current_object)->hitbox_offset_x = -1;
		}
		depth = (uint16_t)(OBJECT(current_object)->screen_y - 0xFu);
		queue_sprite_at_depth(&ignored, &depth);
		OBJECT(current_object)->health = 0x7F;
		add_to_touchable_list(ignored);
		current_object = LINKED(current_object, OBJECT(current_object)->child);
		OBJECT(current_object)->direction = 0x300u;
		OBJECT(current_object)->projected_x = OBJECT(object_cursor)->projected_x;
		OBJECT(current_object)->projected_y = OBJECT(object_cursor)->projected_y;
		OBJECT(current_object)->z = OBJECT(object_cursor)->z;
		frame_index = (uint8_t)OBJECT(object_cursor)->spike_panel.frame_index;
		offset_record = spike_panel_part_1_offsets_ADDRESS + (frame_index * 2u * 2u + frame_index + frame_index);
		OBJECT(current_object)->projected_x += mem_read16(offset_record);
		offset_record += 2u;
		OBJECT(current_object)->projected_y_int += mem_read16(offset_record);
		OBJECT(current_object)->z_int += mem_read16(offset_record + 2u);
		OBJECT(current_object)->sprite_flags = spike_panel_part_1_sprite_flags[frame_index];
		OBJECT(current_object)->sprite_frame = spike_panel_part_1_sprites[frame_index];
		unproject_object(&ignored, &r2, &ignored, &r11);
		if (OBJECT(object_cursor)->spike_panel.frame_index >= 3)
			queue_sprite(offset_record);
		else if ((uint8_t)OBJECT(object_cursor)->spike_panel.frame_index != 5u)
			queue_sprite_at_previous_floor_2();
		else
		{
			sound_id = 0x25u;
			play_sound(sound_id, &offset_record, &r2);
			bucket = 0u;
			queue_sprite_in_bucket(bucket);
		}
		current_object = LINKED(current_object, OBJECT(current_object)->child);
		unproject_object(&ignored, &ignored, &ignored, &r11);
		part_2_frame = (uint8_t)OBJECT(object_cursor)->spike_panel.frame_index;
		offset_record = spike_panel_part_2_offsets_ADDRESS + part_2_frame * 4u;
		OBJECT(current_object)->sprite_frame = spike_panel_part_2_sprites[part_2_frame];
		if (part_2_frame != 3u)
		{
			OBJECT(current_object)->screen_x += ROM16(offset_record);
			OBJECT(current_object)->screen_y += ROM16(offset_record + 2u);
			depth = (uint16_t)(OBJECT(current_object)->screen_y - 0xFu);
			queue_sprite_at_depth(&part_2_frame, &depth);
		}
		if ((int8_t)part_2_frame >= 5)
		{
			OBJECT(current_object)->hitbox_size_x = 0x59u;
			OBJECT(current_object)->hitbox_size_y = 0x2Au;
			OBJECT(current_object)->hitbox_offset_x = -0x14;
		}
		else
		{
			OBJECT(current_object)->hitbox_size_x = 0x4Bu;
			OBJECT(current_object)->hitbox_size_y = 0x2Au;
			OBJECT(current_object)->hitbox_offset_x = -5;
		}
		OBJECT(current_object)->health = 0x7F;
		add_to_touchable_list(part_2_frame);
		return;
	}
	count = 0xFu;
	OBJECT(current_object)->projected_x -= 0x90u;
	OBJECT(current_object)->projected_y_int -= 0x40u;
	spawn_debris(r0, count, r11);
	delete_object_and_children(&ignored, &block_offset, &slope_height);
}

void spike_panel_base_draw(void)
{
	uint8_t bucket = 0;
	uint32_t block_offset = 0, cursor = 0, deleted = 0, ignored = 0, slope_height = 0;

	unproject_object(&ignored, &block_offset, &slope_height, &cursor);
	delete_if_off_screen(&deleted, &block_offset, &slope_height);
	if ((uint8_t)deleted != 1u)
	{
		bucket = 0xFFu;
		queue_sprite_in_bucket(bucket);
		return;
	}
}

void spike_panel_hitbox_update(void)
{
	uint32_t cursor = 0, ignored = 0;

	current_object = LINKED(current_object, OBJECT(object_cursor)->parent);
	if ((int8_t)OBJECT(current_object)->var_80 < 4)
		return;
	OBJECT(object_cursor)->kind = 0x12u;
	if ((int8_t)OBJECT(current_object)->var_81 >= 5)
		OBJECT(object_cursor)->kind = 0xCu;
	current_object = LINKED(current_object, OBJECT(current_object)->child);
	OBJECT(object_cursor)->projected_x = OBJECT(current_object)->projected_x;
	OBJECT(object_cursor)->projected_y = OBJECT(current_object)->projected_y;
	OBJECT(object_cursor)->z = OBJECT(current_object)->z;
	OBJECT(object_cursor)->floor_z = OBJECT(current_object)->floor_z;
	OBJECT(object_cursor)->projected_x -= 0xD8u;
	current_object = (current_object & ~0xFFFFu) | (uint16_t)object_cursor;
	unproject_object(&ignored, &ignored, &ignored, &cursor);
	OBJECT(current_object)->health = 0x7F;
	add_to_touchable_list(ignored);
}

void spike_panel_part_idle(void)
{
}

void electric_pole_init(void)
{
	OBJECT(current_object)->sprite_frame = 0x67D8u;
	OBJECT(current_object)->sprite_palette = 0x87F0u;
	OBJECT(current_object)->hitbox_size_x = 0x19u;
	OBJECT(current_object)->hitbox_height = 0x53u;
	OBJECT(current_object)->health = 4;
	OBJECT(current_object)->kind = 5u;
	OBJECT(current_object)->handler = electric_pole_wait_ADDRESS;
	current_object = LINKED(current_object, OBJECT(current_object)->child);
	OBJECT(current_object)->sprite_palette = 0x83D0u;
	current_object = LINKED(current_object, OBJECT(current_object)->child);
	OBJECT(current_object)->sprite_frame = 0x6850u;
	OBJECT(current_object)->sprite_palette = 0x83D0u;
	current_object = LINKED(current_object, OBJECT(current_object)->child);
	OBJECT(current_object)->direction = 0x401u;
	OBJECT(current_object)->sprite_frame = 0x6850u;
	OBJECT(current_object)->sprite_palette = 0x83D0u;
}

void electric_pole_wait(void)
{
	face_nearest_player(0u, 0u);
	if (nearest_player_distance > 0x70)
	{
		electric_pole_update();
		return;
	}
	OBJECT(current_object)->handler = electric_pole_rise_ADDRESS;
	electric_pole_rise();
}

void electric_pole_rise(void)
{
	uint16_t odd_frame = 0;

	OBJECT(current_object)->electric_pole.frame_toggle++;
	odd_frame = OBJECT(current_object)->electric_pole.frame_toggle & 1u;
	OBJECT(current_object)->electric_pole.frame_toggle = odd_frame;
	if (odd_frame != 0u)
	{
		electric_pole_update();
		return;
	}
	OBJECT(current_object)->electric_pole.frame_index++;
	OBJECT(current_object)->sprite_frame = electric_pole_sprites[electric_pole_rise_frames[(uint8_t)OBJECT(current_object)->electric_pole.frame_index]];
	if (OBJECT(current_object)->electric_pole.frame_index < 0xA)
	{
		electric_pole_update();
		return;
	}
	OBJECT(current_object)->projected_y_int -= 0xBu;
	OBJECT(current_object)->electric_pole.draw_offset_y = 0xBu;
	OBJECT(current_object)->handler = electric_pole_update_ADDRESS;
	electric_pole_update();
}

void electric_pole_update(void)
{
	uint8_t sound_id = 0;
	uint32_t r1 = 0, block_offset = 0, charging_spark_y = 0, cursor = 0, deleted = 0, ignored = 0, slope_height = 0, spark_y = 0, write_index_address = 0;
	uint8_t front_odd_frame = 0, side_odd_frame = 0;

	r14 = LINKED(WORK_RAM, chaser_object);
	distance_xy_to_object(&ignored, &r1);
	if (distance_to_player_1 <= 0xF)
	{
		OBJECT(current_object)->sprite_scale = 0x200u;
		OBJECT(current_object)->projected_y_int += 0xBu;
		OBJECT(current_object)->electric_pole.frame_index = 0;
		OBJECT(current_object)->electric_pole.last_frame = 0xF;
		OBJECT(current_object)->sprite_palette = 0x9000u;
		OBJECT(current_object)->handler = explosion_animate_ADDRESS;
		OBJECT(current_object)->electric_pole.frame_toggle = 0u;
		sound_id = 7u;
		play_sound(sound_id, &r1, &write_index_address);
		return;
	}
	if ((OBJECT(current_object)->flags & 4u) == 0u)
	{
	electric_pole_draw:
		unproject_object(&ignored, &block_offset, &slope_height, &cursor);
		delete_if_off_screen(&deleted, &block_offset, &slope_height);
		if ((uint8_t)deleted == 1u)
			return;
		OBJECT(current_object)->screen_y += OBJECT(current_object)->electric_pole.draw_offset_y;
		queue_sprite(r1);
		if ((int16_t)AS16(OBJECT(object_cursor)->electric_pole.frame_index) >= 0xA)
		{
			current_object = LINKED(current_object, OBJECT(current_object)->child);
			AT8(OBJECT(current_object)->direction, 1)++;
			front_odd_frame = AT8(OBJECT(current_object)->direction, 1) & 1u;
			AT8(OBJECT(current_object)->direction, 1) = front_odd_frame;
			if (front_odd_frame == 0u)
			{
				AS8(OBJECT(current_object)->direction)++;
				if ((int8_t)AS8(OBJECT(current_object)->direction) > 6)
					OBJECT(current_object)->direction = 0u;
				OBJECT(current_object)->sprite_frame = electric_pole_front_spark_sprites[AS8(OBJECT(current_object)->direction)];
			}
			OBJECT(current_object)->screen_x = OBJECT(object_cursor)->screen_x;
			spark_y = (uint16_t)(OBJECT(object_cursor)->screen_y - 0xAu);
			OBJECT(current_object)->screen_y = spark_y;
			spark_y += 0xBu;
			queue_sprite_at_depth(&ignored, &spark_y);
			r14 = 2u;
			do
			{
				current_object = LINKED(current_object, OBJECT(current_object)->child);
				AT8(OBJECT(current_object)->direction, 1)++;
				side_odd_frame = AT8(OBJECT(current_object)->direction, 1) & 1u;
				AT8(OBJECT(current_object)->direction, 1) = side_odd_frame;
				if (side_odd_frame == 0u)
				{
					AS8(OBJECT(current_object)->direction)++;
					if ((int8_t)AS8(OBJECT(current_object)->direction) > 6)
						OBJECT(current_object)->direction = 0u;
					OBJECT(current_object)->sprite_frame = electric_pole_side_spark_sprites[AS8(OBJECT(current_object)->direction)];
				}
				OBJECT(current_object)->screen_x = OBJECT(object_cursor)->screen_x;
				OBJECT(current_object)->screen_x += electric_pole_side_spark_offsets_x[r14 - 1u];
				spark_y = (uint16_t)(OBJECT(object_cursor)->screen_y - 0x10u);
				OBJECT(current_object)->screen_y = spark_y;
				spark_y += 0x11u;
				queue_sprite_at_depth(&ignored, &spark_y);
				r14 = r14 - 1u;
			} while (r14 != 0u);
		}
		else
		{
			current_object = LINKED(current_object, OBJECT(current_object)->child);
			if (OBJECT(object_cursor)->electric_pole.frame_index < 2)
				return;
			OBJECT(current_object)->sprite_frame = electric_pole_side_spark_sprites[(uint8_t)OBJECT(object_cursor)->electric_pole.frame_index - 2u];
			OBJECT(current_object)->screen_x = OBJECT(object_cursor)->screen_x;
			charging_spark_y = (uint16_t)OBJECT(object_cursor)->screen_y;
			OBJECT(current_object)->screen_y = charging_spark_y;
			charging_spark_y++;
			queue_sprite_at_depth(&ignored, &charging_spark_y);
		}
		current_object = object_cursor;
		add_to_touchable_list(ignored);
		return;
	}
	if (OBJECT(current_object)->health < 4)
	{
		OBJECT(current_object)->sprite_scale = 0x200u;
		OBJECT(current_object)->projected_y_int += 0xBu;
		OBJECT(current_object)->electric_pole.frame_index = 0;
		OBJECT(current_object)->electric_pole.last_frame = 0xF;
		OBJECT(current_object)->sprite_palette = 0x9000u;
		OBJECT(current_object)->handler = explosion_animate_ADDRESS;
		OBJECT(current_object)->electric_pole.frame_toggle = 0u;
		sound_id = 7u;
		play_sound(sound_id, &r1, &write_index_address);
		return;
	}
	OBJECT(current_object)->kind = 0x15u;
	goto electric_pole_draw;
}

void push_crate_init(void)
{
	uint16_t post_distance = 0;
	uint32_t block = 0, free_count = 0, object = 0, piece_count = 0, spawned = 0;

	OBJECT(current_object)->sprite_frame = 0x6968u;
	OBJECT(current_object)->sprite_palette = 0x9D0u;
	OBJECT(current_object)->flags |= 2u;
	OBJECT(current_object)->hitbox_size_x = 0x40u;
	OBJECT(current_object)->hitbox_size_y = 0x4Bu;
	OBJECT(current_object)->hitbox_offset_x = -9;
	OBJECT(current_object)->hitbox_offset_y = -0x42;
	OBJECT(current_object)->hitbox_height = 0x20u;
	OBJECT(current_object)->kind = 0x20u;
	OBJECT(current_object)->push_crate.frame_count = 0x12Cu;
	OBJECT(current_object)->handler = push_crate_update_ADDRESS;
	spawn_count = 2u;
	spawn_object_with_pieces(0x011203u, &spawned, &piece_count, &free_count, &object, &block);
	OBJECT(object_cursor)->child = object;
	copy_position_to(object);
	post_distance = OBJECT(object_cursor)->push_crate.post_distance;
	OBJECT(object)->projected_x -= post_distance * 0x22u + 0x11u;
	OBJECT(object)->projected_y_int -= post_distance * 0x11u + 0x1Du;
	OBJECT(object)->sprite_frame = 0x73B0u;
	OBJECT(object)->sprite_palette = 0x89E0u;
	OBJECT(object)->handler = push_crate_stop_post_update_ADDRESS;
	object = LINKED(object, OBJECT(object)->child);
	OBJECT(object)->sprite_frame = 0x73B0u;
	OBJECT(object)->sprite_palette = 0x89E0u;
}

void push_crate_update(void)
{
	uint32_t direction = 0, distance_y = 0, ignored = 0, object = 0, speed = 0, speed_x = 0, speed_y = 0;

	r14 = LINKED(WORK_RAM, chaser_object);
	distance_xy_to_object(&ignored, &distance_y);
	if ((int16_t)distance_y <= 3)
	{
		OBJECT(current_object)->speed_x = 0;
		OBJECT(current_object)->speed_y = 0;
		OBJECT(current_object)->handler = push_crate_break_ADDRESS;
		push_crate_break();
		return;
	}
	if ((OBJECT(current_object)->flags & 4u) == 0u)
	{
		push_crate_move(0u, 0u, 0u);
		return;
	}
	object = LINKED(WORK_RAM, OBJECT(current_object)->touched_by);
	if ((OBJECT(object)->flags & 0x40u) != 0u)
	{
		OBJECT(current_object)->speed_x = 0;
		OBJECT(current_object)->speed_y = 0;
		OBJECT(current_object)->handler = push_crate_break_ADDRESS;
		push_crate_break();
		return;
	}
	if (OBJECT(current_object)->push_crate.stopped != 0u || OBJECT(current_object)->push_crate.push_quadrant != (uint16_t)((int16_t)(OBJECT(object)->direction + 0x40u) / 0x40 - 1u))
	{
		push_crate_move(0u, 0u, 0u);
		return;
	}
	direction = push_crate_push_angles[OBJECT(current_object)->push_crate.push_quadrant];
	speed = (uint32_t)OBJECT(object)->speed >> 16;
	direction_to_speed(speed, &direction, &speed_x, &speed_y);
	OBJECT(current_object)->speed_x = speed_x;
	OBJECT(current_object)->speed_y = speed_y;
	push_crate_move(0u, 0u, 0u);
}

void push_crate_break(void)
{
	uint32_t r11 = 0, r16 = 0, r17 = 0, break_frame = 0;
	uint16_t odd_frame = 0;

	OBJECT(current_object)->push_crate.frame_count++;
	odd_frame = OBJECT(current_object)->push_crate.frame_count & 1u;
	OBJECT(current_object)->push_crate.frame_count = odd_frame;
	if (odd_frame != 0u)
	{
		push_crate_move(0u, 0u, 0u);
		return;
	}
	OBJECT(current_object)->push_crate.break_frame++;
	break_frame = OBJECT(current_object)->push_crate.break_frame;
	OBJECT(current_object)->sprite_frame = push_crate_break_sprites[break_frame];
	if (break_frame != 2u)
	{
		push_crate_move(0u, 0u, 0u);
		return;
	}
	OBJECT(current_object)->handler = push_crate_move_ADDRESS;
	if (OBJECT(current_object)->push_crate.ring_count <= 0)
	{
		push_crate_move(0u, 0u, 0u);
		return;
	}
	push_crate_release_all_rings(&r11, &r16, &r17);
	push_crate_move(r11, r16, r17);
}

void push_crate_move(uint32_t cursor, uint32_t random_value, uint32_t ring)
{
	uint32_t r3 = 0, block_offset = 0, deleted = 0, depth = 0, ignored = 0;
	uint16_t ring_delay = 0;

	move_object_projected(&ignored, &block_offset, &r3, &cursor);
	delete_if_off_screen(&deleted, &block_offset, &r3);
	if ((uint8_t)deleted == 1u)
		return;
	if (OBJECT(current_object)->push_crate.break_frame != 2u)
	{
		OBJECT(current_object)->health = 0x7F;
		add_to_touchable_list(deleted);
		depth = (uint16_t)(OBJECT(current_object)->screen_y - 0xDu);
		queue_sprite_at_depth(&ignored, &depth);
		if ((uint32_t)OBJECT(current_object)->speed_x != 0u)
		{
			ring_delay = OBJECT(current_object)->push_crate.ring_timer - 1u;
			OBJECT(current_object)->push_crate.ring_timer = ring_delay;
			if ((int16_t)ring_delay < 0 && OBJECT(current_object)->push_crate.ring_count > 0)
			{
				OBJECT(current_object)->push_crate.ring_count--;
				OBJECT(current_object)->push_crate.ring_timer = 0x10;
				rings_to_release = 1u;
				push_crate_release_rings(r3, &cursor, &random_value, &ring);
			}
		}
	}
	else
	{
		deleted = (uint16_t)(deleted + 0xFFu);
		queue_sprite_in_bucket(deleted);
	}
}

void push_crate_stop_post_update(void)
{
	uint16_t offset_x = 0, offset_y = 0;
	uint32_t r1 = 0, cursor = 0, depth = 0, ignored = 0;

	unproject_object(&ignored, &ignored, &ignored, &cursor);
	r14 = LINKED(WORK_RAM, OBJECT(current_object)->parent);
	distance_xy_to_object(&ignored, &r1);
	if (distance_to_player_2 <= 0x33)
	{
		OBJECT(r14)->var_81 = 1u;
		OBJECT(r14)->speed_x = 0;
		OBJECT(r14)->speed_y = 0;
	}
	queue_sprite(r1);
	current_object = LINKED(current_object, OBJECT(current_object)->child);
	offset_x = 0u;
	offset_y = 0xFFF1u;
	place_sprite_near_cursor(offset_x, offset_y);
	depth = (uint16_t)(OBJECT(current_object)->screen_y + 0x11u);
	queue_sprite_at_depth(&ignored, &depth);
}

void push_crate_release_all_rings(uint32_t *p_allocated, uint32_t *p_random_value, uint32_t *p_ring)
{
	uint32_t allocated = *p_allocated;
	uint32_t random_value = *p_random_value;
	uint32_t ring = *p_ring;

	rings_to_release = OBJECT(current_object)->push_crate.ring_count;
	push_crate_release_rings(0u, &allocated, &random_value, &ring);
	*p_allocated = allocated;
	*p_random_value = random_value;
	*p_ring = ring;
}

void push_crate_release_rings(uint32_t seed, uint32_t *p_allocated, uint32_t *p_random_value, uint32_t *p_ring)
{
	uint32_t allocated = *p_allocated;
	uint32_t random_value = *p_random_value;
	uint32_t ring = *p_ring;
	uint32_t opposite_quadrant = 0;

	opposite_quadrant = OBJECT(current_object)->push_crate.push_quadrant;
	opposite_quadrant = (uint8_t)(opposite_quadrant + 2u);
	released_ring_angle = push_crate_push_angles[opposite_quadrant & 3u];
	released_ring_flip = 0u;
	released_rings_random = 0u;
	drop_rings(seed, &allocated, &random_value, &ring);
	*p_allocated = allocated;
	*p_random_value = random_value;
	*p_ring = ring;
}

void breakable_panel_init(void)
{
	OBJECT(current_object)->sprite_frame = 0x9278u;
	OBJECT(current_object)->sprite_palette = 0x91F0u;
	OBJECT(current_object)->flags |= 2u;
	OBJECT(current_object)->hitbox_size_x = 7u;
	OBJECT(current_object)->hitbox_size_y = 0x40u;
	OBJECT(current_object)->hitbox_offset_x = -4;
	OBJECT(current_object)->hitbox_offset_y = -0x28;
	OBJECT(current_object)->hitbox_height = 0x64u;
	OBJECT(current_object)->kind = 5u;
	OBJECT(current_object)->handler = breakable_panel_wait_ADDRESS;
	current_object = LINKED(current_object, OBJECT(current_object)->child);
	OBJECT(current_object)->sprite_frame = 0x9308u;
	OBJECT(current_object)->sprite_palette = 0x9200u;
}

void breakable_panel_wait(void)
{
	uint32_t allocated = 0, random_value = 0, ring = 0;

	if ((OBJECT(current_object)->flags & 4u) == 0u || (OBJECT(LINKED(WORK_RAM, OBJECT(current_object)->touched_by))->flags & 0x40u) == 0u)
	{
		draw_breakable_panel();
		return;
	}
	released_ring_flip = 0u;
	released_rings_random = 1u;
	released_ring_angle = OBJECT(current_object)->spawn_arg_1;
	rings_to_release = OBJECT(current_object)->spawn_arg_2;
	drop_rings(0u, &allocated, &random_value, &ring);
	OBJECT(current_object)->handler = breakable_panel_break_ADDRESS;
	breakable_panel_break();
}

void breakable_panel_break(void)
{
	uint16_t frame_timer = 0;

	frame_timer = OBJECT(current_object)->breakable_panel.frame_timer;
	OBJECT(current_object)->breakable_panel.frame_timer = frame_timer - 1u;
	if (frame_timer != 0u)
	{
		draw_breakable_panel();
		return;
	}
	OBJECT(current_object)->sprite_frame = breakable_panel_break_frames[(uint8_t)OBJECT(current_object)->breakable_panel.frame_index];
	OBJECT(current_object)->breakable_panel.frame_timer = 2u;
	OBJECT(current_object)->breakable_panel.frame_index++;
	if (OBJECT(current_object)->breakable_panel.frame_index <= 0x10)
	{
		draw_breakable_panel();
		return;
	}
	OBJECT(current_object)->handler = draw_breakable_panel_ADDRESS;
	draw_breakable_panel();
}

void draw_breakable_panel(void)
{
	uint16_t offset_x = 0, offset_y = 0;
	uint32_t block_offset = 0, cursor = 0, deleted = 0, depth = 0, ignored = 0, slope_height = 0;
	uint16_t direction = 0;

	unproject_object(&ignored, &block_offset, &slope_height, &cursor);
	delete_if_off_screen(&deleted, &block_offset, &slope_height);
	if ((uint8_t)deleted != 0u)
		return;
	depth = (uint16_t)(OBJECT(current_object)->screen_y - 0x20u);
	queue_sprite_at_depth(&ignored, &depth);
	add_to_touchable_list(ignored);
	if ((uint8_t)OBJECT(current_object)->draw_breakable_panel.frame_index != 0u)
		return;
	current_object = LINKED(current_object, OBJECT(current_object)->child);
	direction = OBJECT(current_object)->direction;
	OBJECT(current_object)->direction = direction - 1u;
	if (direction == 0u)
	{
		OBJECT(current_object)->sprite_palette = breakable_panel_glow_palettes[breakable_panel_glow_steps[AT8(OBJECT(current_object)->projected_y, 1)]];
		OBJECT(current_object)->direction = 2u;
		AT8(OBJECT(current_object)->projected_y, 1)++;
		if (AT8(OBJECT(current_object)->projected_y, 1) == 0xFu)
			OBJECT(current_object)->direction = 0xFu;
		if ((int8_t)AT8(OBJECT(current_object)->projected_y, 1) > 0x1C)
		{
			OBJECT(current_object)->direction = 0x1Eu;
			AT8(OBJECT(current_object)->projected_y, 1) = 0u;
		}
	}
	offset_x = 0u;
	offset_y = 0u;
	place_sprite_near_cursor(offset_x, offset_y);
	depth = (uint16_t)(OBJECT(object_cursor)->screen_y - 0x19u);
	queue_sprite_at_depth(&ignored, &depth);
}

void stage_03_floor_block_start(void)
{
	uint32_t block_kind = 0, ignored = 0, kind_entry = 0, kind_record = 0;

	block_kind = OBJECT(current_object)->sprite_frame;
	OBJECT(current_object)->speed_x_int = block_kind;
	kind_entry = stage_03_floor_block_kinds_ADDRESS + block_kind * 4u;
	kind_record = ROM16(kind_entry) + stage_03_floor_block_kinds_ADDRESS;
	OBJECT(current_object)->sprite_offset_x = ROM8(kind_record);
	kind_record++;
	OBJECT(current_object)->sprite_offset_y = ROM8(kind_record);
	kind_record++;
	OBJECT(current_object)->speed_y_fraction = ROM16(kind_record);
	OBJECT(current_object)->animation = kind_record + 2u;
	OBJECT(current_object)->speed_x_fraction = ROM16(kind_entry + 2u);
	OBJECT(current_object)->sprite_flags = 0x1004u;
	OBJECT(current_object)->sprite_palette = 0x9040u;
	OBJECT(current_object)->handler = stage_03_floor_block_update_ADDRESS;
	OBJECT(current_object)->hitbox_size_x = 0x10u;
	OBJECT(current_object)->hitbox_size_y = 0x10u;
	OBJECT(current_object)->speed_z = 0;
	OBJECT(current_object)->flags = 2u;
	stage_03_floor_block_falls[0] = 0u;
	stage_03_floor_block_falls_1 = 0u;
	OBJECT(current_object)->sprite_frame = mem_read16(OBJECT(current_object)->animation + 2u);
	OBJECT(current_object)->animation_frame = 0u;
	OBJECT(current_object)->animation_speed = 8u;
	OBJECT(current_object)->animation_timer = 0;
	OBJECT(current_object)->z = 0;
	project_object(&ignored, &ignored);
}

void stage_03_floor_block_update(void)
{
	uint32_t r1 = 0, r7 = 0, at_last_step = 0, depth = 0, ignored = 0;

	if (stage_03_floor_block_falls[OBJECT(current_object)->speed_x_fraction] != 0u)
	{
		OBJECT(current_object)->speed_z += 0x1000u;
		OBJECT(current_object)->z += OBJECT(current_object)->speed_z;
		if (OBJECT(current_object)->speed_x_int < 4)
		{
			OBJECT(current_object)->sprite_offset_x = 0;
			OBJECT(current_object)->sprite_offset_y = 0;
		}
		if ((int8_t)OBJECT(current_object)->animation_speed >= 0)
		{
			animate(&r1, &r7, &at_last_step);
			if ((uint8_t)at_last_step != 0u)
				OBJECT(current_object)->animation_speed = 0xFFu;
		}
	}
	update_screen_position(&ignored, &r1);
	if (OBJECT(current_object)->screen_y < 0x158)
	{
		depth = (uint16_t)(OBJECT(current_object)->screen_y - OBJECT(current_object)->speed_y_fraction - OBJECT(current_object)->sprite_offset_y);
		queue_sprite_at_depth(&ignored, &depth);
		return;
	}
	delete_current_object();
}

void spiked_roller_start(void)
{
	uint32_t r11 = 0, block_offset = 0, free_count = 0, ignored = 0, object = 0, record_cursor = 0, roller_kind = 0, y = 0, zone_index = 0;
	uint8_t parts_left = 0;

	roller_kind = OBJECT(current_object)->sprite_frame;
	OBJECT(current_object)->platform = roller_kind;
	OBJECT(current_object)->spiked_roller.event_offset = spiked_roller_events[roller_kind];
	record_cursor = spiked_roller_kinds_ADDRESS + spiked_roller_kinds[roller_kind];
	roller_parts_left = ROM8(record_cursor);
	record_cursor++;
	r11 = ROM16(record_cursor);
	record_cursor += 2u;
	roller_first_state = ROM16(record_cursor);
	record_cursor += 2u;
	roller_back_zone_index = ROM16(record_cursor);
	OBJECT(current_object)->spiked_roller.path = record_cursor + 2u;
	r11 += 0x8219Cu;
	roller_step_x = (uint32_t)(int8_t)ROM8(r11) << 16;
	r11++;
	roller_step_y = (uint32_t)(int8_t)ROM8(r11) << 16;
	r11++;
	roller_part_record = r11;
	OBJECT(current_object)->flags = 0u;
	find_floor(&roller_kind, &y, &block_offset, &ignored, &r11);
	OBJECT(current_object)->z = OBJECT(current_object)->floor_z;
	OBJECT(current_object)->spiked_roller.screen_threshold = 0;
	OBJECT(current_object)->spiked_roller.catch_up_factor = 0u;
	OBJECT(current_object)->spiked_roller.base_speed = 0;
	OBJECT(current_object)->spiked_roller.wait_timer = 0;
	OBJECT(current_object)->spiked_roller.rumble_timer = 0u;
	OBJECT(current_object)->spiked_roller.substate = 0u;
	OBJECT(current_object)->player = 0u;
	OBJECT(current_object)->spiked_roller.holding_back = 0u;
	roller_ram_count = 0u;
	roller_ram_hit = 0u;
	machine_fire_done = 0u;
	OBJECT(current_object)->spiked_roller.rumble_off_screen = 0xFFu;
	if (chaser_object == 0u)
		chaser_object = current_object;
	roller_part_x = OBJECT(current_object)->x;
	roller_part_y = OBJECT(current_object)->y;
	roller_part_frame_cursor = spiked_roller_part_frames_ADDRESS;
	object = current_object;
	for (;;)
	{
		OBJECT(current_object)->sprite_palette = 0x82F0u;
		OBJECT(current_object)->hitbox_height = 0xF0u;
		OBJECT(current_object)->kind = 0xCu;
		OBJECT(current_object)->handler = spiked_roller_update_ADDRESS;
		OBJECT(current_object)->sprite_offset_x = 0;
		OBJECT(current_object)->sprite_offset_y = 5;
		OBJECT(current_object)->speed_z = 0;
		OBJECT(current_object)->var_80 = 2u;
		OBJECT(current_object)->player = 0u;
		OBJECT(current_object)->speed_x = 0;
		OBJECT(current_object)->speed_y = 0;
		record_cursor = roller_part_record;
		OBJECT(current_object)->sprite_flags = mem_read16(record_cursor);
		record_cursor += 2u;
		OBJECT(current_object)->hitbox_size_x = mem_read16(record_cursor);
		record_cursor += 2u;
		OBJECT(current_object)->hitbox_size_y = mem_read16(record_cursor);
		record_cursor += 2u;
		OBJECT(current_object)->spawn_arg_2 = mem_read16(record_cursor);
		record_cursor += 2u;
		OBJECT(current_object)->flags = mem_read32(record_cursor);
		record_cursor += 4u;
		OBJECT(current_object)->hitbox_offset_x = mem_read8(record_cursor);
		OBJECT(current_object)->hitbox_offset_y = mem_read8(record_cursor + 1u);
		OBJECT(current_object)->direction = (uint32_t)OBJECT(current_object)->spawn_arg_2 << 8;
		OBJECT(current_object)->var_86 = object_cursor;
		OBJECT(current_object)->parent = object;
		OBJECT(object)->child = current_object;
		object = (object & ~0xFFFFu) | (uint16_t)current_object;
		OBJECT(current_object)->animation_timer = 0;
		OBJECT(current_object)->animation = spiked_roller_animation_ADDRESS;
		OBJECT(current_object)->animation_frame = mem_read8(roller_part_frame_cursor);
		roller_part_frame_cursor++;
		OBJECT(current_object)->sprite_frame = spiked_roller_frames[OBJECT(current_object)->animation_frame];
		parts_left = roller_parts_left;
		roller_parts_left = parts_left - 1u;
		if ((int8_t)parts_left <= 1)
			break;
		allocate_object(&free_count, &r11);
		if (free_count == 0u)
			return;
		current_object = r11;
		roller_part_x += roller_step_x;
		OBJECT(current_object)->x = roller_part_x;
		roller_part_y += roller_step_y;
		OBJECT(current_object)->y = roller_part_y;
		OBJECT(current_object)->z = OBJECT(object_cursor)->z;
		OBJECT(current_object)->spawn_arg_2 = OBJECT(object_cursor)->spiked_roller.path_direction;
	}
	OBJECT(current_object)->child = 0xFFFFu;
	AS16(OBJECT(object_cursor)->spiked_roller.state) = roller_first_state;
	zone_index = roller_back_zone_index;
	spawn_roller_back_zone(zone_index, r11);
	if (OBJECT(object_cursor)->spiked_roller.state == 0u)
		players_frozen = 0xFFu;
	if (OBJECT(object_cursor)->spiked_roller.state == 0xEu)
	{
		OBJECT(object_cursor)->speed_z = -0x100000;
		OBJECT(object_cursor)->z = 0x500000;
		OBJECT(object_cursor)->speed_x = 0x8000;
		OBJECT(object_cursor)->speed_y = 0;
		OBJECT(object_cursor)->flags |= 1u;
	}
}

void spiked_roller_update(void)
{
	uint32_t r1 = 0, ignored = 0;

	switch (OBJECT(current_object)->spiked_roller.state)
	{
	case 0:
		spiked_roller_chase();
		break;
	case 1:
		spiked_roller_fall_off_ledge();
		break;
	case 2:
		spiked_roller_follow_leader();
		break;
	case 3:
		spiked_roller_drop_in();
		break;
	case 4:
		spiked_roller_ram();
		break;
	case 5:
		spiked_roller_remove();
		break;
	case 6:
		spiked_roller_crash_into_machine();
		break;
	case 7:
		spiked_roller_sink_and_jump();
		break;
	case 8:
		spiked_roller_fall();
		break;
	case 9:
		spiked_roller_burst_out();
		break;
	case 10:
		spiked_roller_smash_wall();
		break;
	case 11:
		spiked_roller_wait_for_turn();
		break;
	case 12:
		spiked_roller_wait_to_burst_out();
		break;
	case 13:
		spiked_roller_shift_box();
		break;
	case 14:
		spiked_roller_wait_hidden();
		break;
	case 15:
		spiked_roller_spawn_fixed_zone();
		break;
	case 16:
		spiked_roller_chase();
		break;
	default:
		unknown_code(0x803BBu + OBJECT(current_object)->spiked_roller.state * 3u);
	}
	if (OBJECT(current_object)->player == 0u)
	{
		if ((OBJECT(current_object)->flags & 1u) != 0u)
			return;
		add_to_zone_list(0u);
		if (OBJECT(current_object)->spiked_roller.state == 1u)
		{
			ignored = queue_sprite_at_previous_floor();
			return;
		}
		r1 = queue_sprite_at_base(0u);
		return;
	}
	if (chaser_object == (uint16_t)current_object)
		chaser_object = 0u;
	delete_current_object();
}

void spiked_roller_chase(void)
{
	uint8_t sound_id = 0;
	uint16_t height_above_floor = 0;
	uint32_t r2 = 0, r3 = 0, r7 = 0, chase_speed = 0, cursor = 0, event_kind = 0, ignored = 0, move_step = 0, off_screen = 0, record_cursor = 0, speed_level = 0, x = 0, y = 0;
	uint8_t hold_timer = 0, rumble_timer = 0;
	uint16_t record_position = 0, z_int = 0, floor_z_int = 0, event_position = 0, max_y = 0, min_x = 0, min_y = 0, max_x = 0;
	uint32_t flags = 0;

	flags = OBJECT(current_object)->flags;
	OBJECT(current_object)->flags = flags | 0x80000000u;
	if ((flags & 0x80000000u) == 0u)
	{
	spiked_roller_chase_read_script:
		if ((uint16_t)OBJECT(current_object)->spiked_roller.wait_timer == 0u)
		{
			record_cursor = OBJECT(current_object)->spiked_roller.path;
			record_position = mem_read16(record_cursor);
			record_cursor += 2u;
			if (record_position <= scroll_position)
			{
				if ((int16_t)mem_read16(record_cursor + 2u) >= 0)
				{
					OBJECT(current_object)->spiked_roller.screen_threshold = mem_read16(record_cursor);
					record_cursor += 2u;
					OBJECT(current_object)->spiked_roller.catch_up_factor = mem_read16(record_cursor);
					record_cursor += 2u;
					OBJECT(current_object)->spiked_roller.base_speed = mem_read16(record_cursor);
					record_cursor += 2u;
					OBJECT(current_object)->spiked_roller.wait_timer = mem_read16(record_cursor);
					OBJECT(current_object)->spiked_roller.path = record_cursor + 2u;
					if ((uint16_t)OBJECT(current_object)->spiked_roller.wait_timer == 0u)
					{
						OBJECT(current_object)->kind = 0x18u;
						players_frozen = 0u;
					}
				}
				else
				{
					OBJECT(current_object)->spiked_roller.catch_up_factor = 0u;
					OBJECT(current_object)->spiked_roller.base_speed = 0;
					off_screen = is_off_screen();
					if (off_screen != 0u)
					{
						OBJECT(current_object)->spiked_roller.state = 5u;
						return;
					}
				}
			}
		}
	}
	else if (OBJECT(current_object)->spiked_roller.holding_back == 0u)
	{
		if (no_player_in_play == 0u && player_mishap == 0u && player_dropping_in == 0u)
			goto spiked_roller_chase_read_script;
		OBJECT(current_object)->spiked_roller.holding_back = 0xFFu;
		OBJECT(current_object)->spiked_roller.saved_base_speed = OBJECT(current_object)->spiked_roller.base_speed;
		OBJECT(current_object)->spiked_roller.base_speed = 0;
		OBJECT(current_object)->spiked_roller.hold_start_position = scroll_position;
		OBJECT(current_object)->spiked_roller.hold_timer = 0xFFu;
		OBJECT(current_object)->spiked_roller.saved_threshold = OBJECT(current_object)->spiked_roller.screen_threshold;
		switch (OBJECT(current_object)->spiked_roller.path_direction)
		{
		case 0:
		case 3:
		case 4:
			OBJECT(current_object)->spiked_roller.screen_threshold = 0x160;
			break;
		case 1:
		case 2:
			OBJECT(current_object)->spiked_roller.screen_threshold = -0x20;
			break;
		default:
			unknown_code(0x80448u + OBJECT(current_object)->spiked_roller.path_direction * 2u);
		}
	}
	if (OBJECT(current_object)->platform != 5u && OBJECT(current_object)->platform != 2u)
	{
		find_floor(&x, &y, &r2, &r3, &cursor);
		z_int = OBJECT(current_object)->z_int;
		floor_z_int = OBJECT(current_object)->floor_z_int;
		height_above_floor = (uint16_t)(floor_z_int - z_int);
		if (floor_z_int == z_int || (int16_t)height_above_floor <= 8)
			OBJECT(current_object)->z = OBJECT(current_object)->floor_z;
		else
		{
			OBJECT(current_object)->spiked_roller.state = 1u;
			chaser_object = 0u;
		}
	}
	project_object(&ignored, &ignored);
	if (OBJECT(current_object)->spiked_roller.holding_back != 0u)
	{
		if ((int16_t)(scroll_position - OBJECT(current_object)->spiked_roller.hold_start_position) >= 0x300)
			goto spiked_roller_chase_stop_holding_back;
		if (no_player_in_play != 0u)
			OBJECT(current_object)->spiked_roller.hold_timer = 0xFFu;
		hold_timer = OBJECT(current_object)->spiked_roller.hold_timer;
		OBJECT(current_object)->spiked_roller.hold_timer = hold_timer - 1u;
		if (hold_timer == 0u || AS32(players_finished[0]) == 0xFFFFFFu)
			goto spiked_roller_chase_stop_holding_back;
	}
	switch (OBJECT(current_object)->spiked_roller.path_direction)
	{
	case 0:
	case 3:
		if (OBJECT(current_object)->screen_x <= OBJECT(current_object)->spiked_roller.screen_threshold)
		{
		spiked_roller_chase_use_base_speed:
			chase_speed = (uint16_t)OBJECT(current_object)->spiked_roller.base_speed;
		}
		else
		{
		spiked_roller_chase_catch_up:
			OBJECT(current_object)->spiked_roller.wait_timer = 0;
			if (OBJECT(current_object)->spiked_roller.holding_back != 0u)
			{
			spiked_roller_chase_stop_holding_back:
				OBJECT(current_object)->spiked_roller.holding_back = 0u;
				OBJECT(current_object)->spiked_roller.screen_threshold = OBJECT(current_object)->spiked_roller.saved_threshold;
				OBJECT(current_object)->spiked_roller.base_speed = OBJECT(current_object)->spiked_roller.saved_base_speed;
			}
			compute_camera_ground_speed(r3);
			chase_speed = (uint16_t)((uint32_t)camera_ground_speed_int * OBJECT(current_object)->spiked_roller.catch_up_factor) >> 8;
			if ((int16_t)chase_speed <= OBJECT(current_object)->spiked_roller.base_speed)
				goto spiked_roller_chase_use_base_speed;
		}
		break;
	case 1:
	case 2:
	case 4:
		if (OBJECT(current_object)->spiked_roller.screen_threshold <= OBJECT(current_object)->screen_x)
			goto spiked_roller_chase_use_base_speed;
		goto spiked_roller_chase_catch_up;
	default:
		unknown_code(0x80548u + OBJECT(current_object)->spiked_roller.path_direction * 2u);
	}
	chaser_speed = chase_speed;
	if (current_game_mode != 0x11u)
		OBJECT(current_object)->spiked_roller.wait_timer--;
	if (OBJECT(current_object)->spiked_roller.wait_timer <= 0)
	{
		OBJECT(current_object)->spiked_roller.wait_timer = 0;
		if (grey_screen_active == 0u)
		{
			switch (OBJECT(current_object)->spiked_roller.path_direction)
			{
			case 0:
				move_step = (uint32_t)chaser_speed << 12;
				OBJECT(current_object)->y -= move_step;
				OBJECT(current_object)->speed_y = move_step;
				OBJECT(current_object)->speed_x = 0;
				break;
			case 1:
				move_step = (uint32_t)chaser_speed << 12;
				OBJECT(current_object)->x += move_step;
				OBJECT(current_object)->speed_x = move_step;
				OBJECT(current_object)->speed_y = 0;
				break;
			case 2:
			case 4:
				move_step = (uint32_t)chaser_speed << 12;
				OBJECT(current_object)->y += move_step;
				OBJECT(current_object)->speed_y = move_step;
				OBJECT(current_object)->speed_x = 0;
				break;
			case 3:
				move_step = (uint32_t)chaser_speed << 12;
				OBJECT(current_object)->x -= move_step;
				OBJECT(current_object)->speed_x = move_step;
				OBJECT(current_object)->speed_y = 0;
				break;
			default:
				unknown_code(0x805E4u + OBJECT(current_object)->spiked_roller.path_direction * 2u);
			}
		}
		else
			chaser_speed = 0;
	}
	if ((uint16_t)chaser_speed != 0u)
	{
		speed_level = (uint16_t)chaser_speed >> 4;
		if ((int16_t)speed_level > 5)
			speed_level = 5u;
		OBJECT(current_object)->animation_speed = spiked_roller_animation_speeds[speed_level];
		animate(&chase_speed, &r7, &off_screen);
	}
	switch (OBJECT(current_object)->spiked_roller.path_direction)
	{
	case 0:
	case 4:
		if (OBJECT(current_object)->screen_x > 0x140)
		{
		spiked_roller_chase_no_rumble:
			OBJECT(current_object)->spiked_roller.rumble_off_screen = 0xFFu;
		}
		else
		{
		spiked_roller_chase_rumble:
			if (OBJECT(current_object)->spiked_roller.rumble_off_screen != 0u)
			{
				OBJECT(current_object)->spiked_roller.rumble_off_screen = 0u;
				OBJECT(current_object)->spiked_roller.rumble_timer = 0u;
			}
			if ((uint16_t)chaser_speed == 0u)
				goto spiked_roller_chase_no_rumble;
			rumble_timer = OBJECT(current_object)->spiked_roller.rumble_timer;
			OBJECT(current_object)->spiked_roller.rumble_timer = rumble_timer - 1u;
			if (rumble_timer == 0u)
			{
				sound_id = 0xBu;
				play_sound(sound_id, &chase_speed, &r2);
				speed_level = (uint16_t)chaser_speed;
				speed_level = speed_level >> 4;
				if ((int16_t)speed_level < 0)
					speed_level = 0u;
				if ((int16_t)speed_level > 4)
					speed_level = 4u;
				OBJECT(current_object)->spiked_roller.rumble_timer = spiked_roller_rumble_delays[speed_level];
			}
		}
		break;
	case 1:
		if (OBJECT(current_object)->screen_x < -0x50)
			goto spiked_roller_chase_no_rumble;
		goto spiked_roller_chase_rumble;
	case 2:
		if (OBJECT(current_object)->screen_x < 0)
			goto spiked_roller_chase_no_rumble;
		goto spiked_roller_chase_rumble;
	case 3:
		if (OBJECT(current_object)->screen_x > 0x140)
			goto spiked_roller_chase_no_rumble;
		goto spiked_roller_chase_rumble;
	default:
		unknown_code(0x80678u + OBJECT(current_object)->spiked_roller.path_direction * 2u);
	}
	record_cursor = OBJECT(current_object)->spiked_roller.event_offset + spiked_roller_events_ADDRESS;
	event_position = ROM16(record_cursor);
	record_cursor += 2u;
	if (scroll_position < event_position || OBJECT(current_object)->spiked_roller.substate != 0u)
	{
		roller_ram_hit = 0u;
		return;
	}
	event_kind = ROM16(record_cursor) * 2u;
	record_cursor += 2u;
	switch (event_kind)
	{
	case 0x0u:
	case 0x8u:
		max_y = ROM16(record_cursor);
		record_cursor += 2u;
		if (OBJECT(current_object)->y_int <= (int16_t)max_y)
		{
			OBJECT(current_object)->spiked_roller.state = (uint8_t)ROM16(record_cursor);
			AS32(OBJECT(current_object)->spiked_roller.event_offset) += 8u;
		}
		break;
	case 0x2u:
		min_x = ROM16(record_cursor);
		record_cursor += 2u;
		if ((int16_t)min_x <= OBJECT(current_object)->x_int)
		{
			OBJECT(current_object)->spiked_roller.state = (uint8_t)ROM16(record_cursor);
			AS32(OBJECT(current_object)->spiked_roller.event_offset) += 8u;
			roller_ram_hit = 0u;
			return;
		}
		break;
	case 0x4u:
		min_y = ROM16(record_cursor);
		record_cursor += 2u;
		if ((int16_t)min_y <= OBJECT(current_object)->y_int)
		{
			OBJECT(current_object)->spiked_roller.state = (uint8_t)ROM16(record_cursor);
			AS32(OBJECT(current_object)->spiked_roller.event_offset) += 8u;
			roller_ram_hit = 0u;
			return;
		}
		break;
	case 0x6u:
		max_x = ROM16(record_cursor);
		record_cursor += 2u;
		if (OBJECT(current_object)->x_int <= (int16_t)max_x)
		{
			OBJECT(current_object)->spiked_roller.state = (uint8_t)ROM16(record_cursor);
			AS32(OBJECT(current_object)->spiked_roller.event_offset) += 8u;
			roller_ram_hit = 0u;
			return;
		}
		break;
	default:
		unknown_code(0x8071Bu + event_kind);
	}
	roller_ram_hit = 0u;
}

void spiked_roller_fall_off_ledge(void)
{
	OBJECT(current_object)->sprite_palette |= 0x4000u;
	spiked_roller_fall();
}

void spiked_roller_fall(void)
{
	uint32_t r7 = 0, r8 = 0, frame = 0, ignored = 0;

	OBJECT(current_object)->speed_z += 0x1000u;
	OBJECT(current_object)->z += OBJECT(current_object)->speed_z;
	OBJECT(current_object)->animation_speed = 1u;
	animate(&frame, &r7, &r8);
	if (OBJECT(current_object)->screen_y >= 0x130)
		OBJECT(current_object)->player = 0xFFu;
	project_object(&ignored, &ignored);
}

void spiked_roller_follow_leader(void)
{
	uint32_t r1 = 0, r7 = 0, r8 = 0, ignored = 0, leader_hidden_bit = 0, leader_reverse_bit = 0, object = 0, speed_level = 0;

	if (OBJECT((current_object & ~0xFFFFu) | OBJECT(current_object)->spiked_roller.leader)->var_80 == 5u)
	{
		OBJECT(current_object)->player = 0xFFu;
		return;
	}
	object = (current_object & ~0xFFFFu) | OBJECT(current_object)->spiked_roller.leader;
	switch (OBJECT(current_object)->spiked_roller.path_direction)
	{
	case 0:
	case 2:
	case 4:
		OBJECT(current_object)->y = OBJECT(object)->y;
		OBJECT(current_object)->speed_y = OBJECT(object)->speed_y;
		break;
	case 1:
	case 3:
		OBJECT(current_object)->x = OBJECT(object)->x;
		OBJECT(current_object)->speed_x = OBJECT(object)->speed_x;
		break;
	default:
		unknown_code(0x807BBu + OBJECT(current_object)->spiked_roller.path_direction * 2u);
	}
	OBJECT(current_object)->kind = OBJECT(object)->kind;
	OBJECT(current_object)->hitbox_height = OBJECT(object)->hitbox_height;
	OBJECT(current_object)->hitbox_offset_x = OBJECT(object)->hitbox_offset_x;
	OBJECT(current_object)->hitbox_offset_y = OBJECT(object)->hitbox_offset_y;
	OBJECT(current_object)->z = OBJECT(object)->z;
	OBJECT(current_object)->floor_z = OBJECT(object)->floor_z;
	OBJECT(current_object)->previous_floor_z = OBJECT(object)->previous_floor_z;
	if (OBJECT(object)->var_80 == 1u)
		OBJECT(current_object)->spiked_roller.state = 1u;
	if (OBJECT(object)->var_80 == 8u)
		OBJECT(current_object)->spiked_roller.state = 8u;
	project_object(&ignored, &r1);
	if ((uint16_t)chaser_speed == 0u)
	{
		leader_hidden_bit = OBJECT((current_object & ~0xFFFFu) | OBJECT(current_object)->spiked_roller.leader)->flags & 1u;
		OBJECT(current_object)->flags &= ~1u;
		OBJECT(current_object)->flags |= leader_hidden_bit;
		return;
	}
	speed_level = (uint16_t)chaser_speed >> 4;
	if ((int16_t)speed_level > 5)
		speed_level = 5u;
	OBJECT(current_object)->animation_speed = spiked_roller_animation_speeds[speed_level];
	leader_reverse_bit = OBJECT((current_object & ~0xFFFFu) | OBJECT(current_object)->spiked_roller.leader)->flags & 0x80u;
	OBJECT(current_object)->flags &= ~0x80u;
	OBJECT(current_object)->flags |= leader_reverse_bit;
	animate(&r1, &r7, &r8);
	leader_hidden_bit = OBJECT((current_object & ~0xFFFFu) | OBJECT(current_object)->spiked_roller.leader)->flags & 1u;
	OBJECT(current_object)->flags &= ~1u;
	OBJECT(current_object)->flags |= leader_hidden_bit;
}

void spiked_roller_wait_hidden(void)
{
	OBJECT(current_object)->flags |= 1u;
	if (scroll_position >= 0x9B00u || machine_fire_done != 0u)
	{
		OBJECT(current_object)->spiked_roller.state = 3u;
		OBJECT(current_object)->flags &= ~1u;
	}
}

void spiked_roller_drop_in(void)
{
	uint8_t sound_number = 0;
	uint32_t r1 = 0, r7 = 0, r8 = 0, ignored = 0, write_index_address = 0;

	OBJECT(current_object)->speed_z += 0x8000u;
	OBJECT(current_object)->z += OBJECT(current_object)->speed_z;
	OBJECT(current_object)->x += OBJECT(current_object)->speed_x;
	OBJECT(current_object)->y += OBJECT(current_object)->speed_y;
	OBJECT(current_object)->animation_speed = 1u;
	animate(&r1, &r7, &r8);
	OBJECT(current_object)->floor_z = 0;
	if (OBJECT(current_object)->speed_z_int >= 0 && OBJECT(current_object)->floor_z_int <= OBJECT(current_object)->z_int)
	{
		OBJECT(current_object)->z = OBJECT(current_object)->floor_z;
		OBJECT(current_object)->spiked_roller.state = 0u;
		camera_shake_active = 1u;
		sound_number = 0x20u;
		play_sound(sound_number, &r1, &write_index_address);
	}
	project_object(&ignored, &ignored);
}

void spiked_roller_ram(void)
{
	uint8_t sound_id = 0;
	uint32_t ignored = 0, speed_level = 0, write_index = 0, write_index_address = 0;
	uint8_t rumble_timer = 0;

	switch (OBJECT(current_object)->spiked_roller.substate)
	{
	case 0:
		spiked_roller_ram_charge();
		break;
	case 1:
		spiked_roller_ram_pause();
		break;
	case 2:
		spiked_roller_ram_back_off();
		break;
	case 3:
		spiked_roller_ram_wait();
		break;
	case 4:
		spiked_roller_ram_charge();
		break;
	default:
		unknown_code(0x80944u + OBJECT(current_object)->spiked_roller.substate * 3u);
	}
	rumble_timer = OBJECT(current_object)->spiked_roller.rumble_timer;
	OBJECT(current_object)->spiked_roller.rumble_timer = rumble_timer - 1u;
	if ((int8_t)rumble_timer > 1)
	{
		project_object(&ignored, &ignored);
		return;
	}
	sound_id = 0xBu;
	play_sound(sound_id, &write_index, &write_index_address);
	speed_level = (uint16_t)chaser_speed;
	speed_level = speed_level >> 4;
	if ((int16_t)speed_level < 0)
		speed_level = 0u;
	if ((int16_t)speed_level > 4)
		speed_level = 4u;
	OBJECT(current_object)->spiked_roller.rumble_timer = spiked_roller_rumble_delays[speed_level];
	project_object(&ignored, &ignored);
}

void spiked_roller_ram_charge(void)
{
	uint8_t sound_id = 0;
	uint16_t margin_x = 0, margin_y = 0;
	uint32_t r1 = 0, r7 = 0, r8 = 0, off_screen = 0, write_index_address = 0;

	OBJECT(current_object)->speed_x = 0;
	OBJECT(current_object)->speed_y = -0x40000;
	chaser_speed = 0x40;
	OBJECT(current_object)->y += OBJECT(current_object)->speed_y;
	OBJECT(current_object)->animation_speed = 1u;
	animate(&r1, &r7, &r8);
	if ((int16_t)(ROM16(spiked_roller_event_coords_ADDRESS + OBJECT(current_object)->spiked_roller.event_offset) - 0x50u) < OBJECT(current_object)->y_int)
		return;
	OBJECT(current_object)->spiked_roller.substate++;
	camera_shake_active = 1u;
	roller_ram_hit = 0xFFu;
	OBJECT(current_object)->spiked_roller.pause_timer = 0u;
	sound_id = 7u;
	play_sound(sound_id, &r1, &write_index_address);
	margin_x = 0x50u;
	margin_y = 0x50u;
	off_screen = is_off_screen_margin(margin_x, margin_y);
	if (off_screen != 0u)
	{
		OBJECT(current_object)->spiked_roller.substate = 0u;
		roller_ram_count = 0u;
		OBJECT(current_object)->spiked_roller.state = 0u;
		OBJECT(current_object)->flags &= ~0x800u;
		OBJECT(current_object)->flags |= 0x80u;
	}
	roller_ram_count++;
	if ((int8_t)roller_ram_count >= 4)
	{
		OBJECT(current_object)->spiked_roller.substate = 0u;
		roller_ram_count = 0u;
		OBJECT(current_object)->spiked_roller.state = 0u;
	}
}

void spiked_roller_ram_pause(void)
{
	OBJECT(current_object)->speed_x = 0;
	OBJECT(current_object)->speed_y = 0;
	chaser_speed = 0;
	roller_ram_hit = 0u;
	OBJECT(current_object)->spiked_roller.pause_timer++;
	if ((int8_t)OBJECT(current_object)->spiked_roller.pause_timer >= 0x3C)
	{
		OBJECT(current_object)->spiked_roller.substate++;
		OBJECT(current_object)->flags &= ~0x80u;
	}
}

void spiked_roller_ram_back_off(void)
{
	uint32_t r7 = 0, r8 = 0, frame = 0;

	OBJECT(current_object)->speed_x = 0;
	OBJECT(current_object)->speed_y = 0x10000;
	chaser_speed = 0x10;
	OBJECT(current_object)->y += OBJECT(current_object)->speed_y;
	OBJECT(current_object)->animation_speed = 4u;
	animate(&frame, &r7, &r8);
	if ((int16_t)ROM16(spiked_roller_event_coords_ADDRESS + OBJECT(current_object)->spiked_roller.event_offset) <= OBJECT(current_object)->y_int)
	{
		OBJECT(current_object)->spiked_roller.substate++;
		OBJECT(current_object)->spiked_roller.pause_timer = 0u;
		OBJECT(current_object)->flags |= 0x80u;
	}
}

void spiked_roller_ram_wait(void)
{
	uint32_t r7 = 0, r8 = 0, frame = 0;

	OBJECT(current_object)->speed_x = 0;
	OBJECT(current_object)->speed_y = 0;
	chaser_speed = 0x40;
	OBJECT(current_object)->animation_speed = 1u;
	animate(&frame, &r7, &r8);
	if (no_player_in_play == 0u)
	{
		OBJECT(current_object)->spiked_roller.pause_timer++;
		if ((int8_t)OBJECT(current_object)->spiked_roller.pause_timer >= 0x3C)
			OBJECT(current_object)->spiked_roller.substate = 0u;
	}
}

void spiked_roller_remove(void)
{
	OBJECT(current_object)->player = 0xFFu;
}

void spiked_roller_crash_into_machine(void)
{
	uint8_t sound_id = 0;
	uint32_t r0 = 0, r1 = 0, r7 = 0, r8 = 0, ignored = 0, speed_level = 0, write_index_address = 0;
	uint8_t rumble_timer = 0;

	r0 = OBJECT(current_object)->spiked_roller.substate * 3u;
	switch (OBJECT(current_object)->spiked_roller.substate)
	{
	case 0:
	case 4:
		OBJECT(current_object)->spiked_roller.substate++;
		chaser_speed = 0x10;
		OBJECT(current_object)->speed_y = -0x10000;
		speed_level = (uint16_t)chaser_speed;
		speed_level = speed_level >> 4;
		if ((int16_t)speed_level > 5)
			speed_level = 5u;
		OBJECT(current_object)->animation_speed = spiked_roller_animation_speeds[speed_level];
	case 1:
		if (scroll_position >= 0x8600u)
		{
			chaser_speed = 0x50;
			OBJECT(current_object)->speed_y = -0x50000;
		}
		OBJECT(current_object)->y += OBJECT(current_object)->speed_y;
		animate(&r1, &r7, &r8);
		rumble_timer = OBJECT(current_object)->spiked_roller.rumble_timer;
		OBJECT(current_object)->spiked_roller.rumble_timer = rumble_timer - 1u;
		if ((int8_t)rumble_timer <= 1)
		{
			sound_id = 0xBu;
			play_sound(sound_id, &r1, &write_index_address);
			speed_level = (uint16_t)chaser_speed;
			speed_level = speed_level >> 4;
			if ((int16_t)speed_level < 0)
				speed_level = 0u;
			if ((int16_t)speed_level > 4)
				speed_level = 4u;
			OBJECT(current_object)->spiked_roller.rumble_timer = spiked_roller_rumble_delays[speed_level];
		}
		if (OBJECT(current_object)->y_int <= 0xAE0)
			OBJECT(current_object)->spiked_roller.substate++;
		project_object(&ignored, &ignored);
		return;
	case 2:
		OBJECT(current_object)->spiked_roller.substate++;
		chaser_speed = 0;
		OBJECT(current_object)->speed_y = 0;
		camera_shake_active = 1u;
		sound_id = 6u;
		play_sound(sound_id, &r1, &write_index_address);
		machine_wrecked = 0xFFu;
		scroll_resume_request = 0xFFu;
		update_screen_position(&ignored, &r1);
		return;
	case 3:
		if (OBJECT(current_object)->screen_y >= 0x158)
		{
			OBJECT(current_object)->spiked_roller.state = 5u;
			chaser_object = 0u;
		}
		update_screen_position(&ignored, &r1);
		return;
	default:
		unknown_code(0x80ACBu + OBJECT(current_object)->spiked_roller.substate * 3u);
	}
}

void spiked_roller_sink_and_jump(void)
{
	uint8_t sound_number = 0;
	uint32_t r1 = 0, r7 = 0, r8 = 0, ignored = 0, write_index_address = 0;

	switch (OBJECT(current_object)->spiked_roller.substate)
	{
	case 0:
	case 2:
		if (stage_03_floor_block_falls[0] == 0u)
		{
			sound_number = 0xDu;
			play_sound(sound_number, &r1, &write_index_address);
		}
		stage_03_floor_block_falls[0] = 0xFFu;
		OBJECT(current_object)->speed_z += 0x1000u;
		OBJECT(current_object)->z += OBJECT(current_object)->speed_z;
		chaser_speed = 0x40;
		OBJECT(current_object)->speed_x = 0;
		OBJECT(current_object)->animation_speed = 1u;
		animate(&r1, &r7, &r8);
		if ((int16_t)(OBJECT(current_object)->z_int - OBJECT(current_object)->floor_z_int) >= 0x40)
		{
			OBJECT(current_object)->spiked_roller.substate++;
			OBJECT(current_object)->speed_z = -0x60000;
			OBJECT(current_object)->speed_x = 0x30000;
		}
		project_object(&ignored, &ignored);
		return;
	case 1:
		OBJECT(current_object)->speed_z += 0x3000u;
		OBJECT(current_object)->z += OBJECT(current_object)->speed_z;
		OBJECT(current_object)->x += OBJECT(current_object)->speed_x;
		OBJECT(current_object)->animation_speed = 1u;
		animate(&r1, &r7, &r8);
		if (OBJECT(current_object)->speed_z_int >= 0 && OBJECT(current_object)->floor_z_int <= OBJECT(current_object)->z_int)
		{
			OBJECT(current_object)->spiked_roller.state = 8u;
			OBJECT(current_object)->speed_z = 0;
			stage_03_floor_block_falls_1 = 0xFFu;
			camera_shake_active = 1u;
			goal_unlocked = 0xFFu;
			sound_number = 0xDu;
			play_sound(sound_number, &r1, &write_index_address);
		}
		project_object(&ignored, &ignored);
		return;
	default:
		unknown_code(0x80BC9u + OBJECT(current_object)->spiked_roller.substate * 2u);
	}
}

void spiked_roller_burst_out(void)
{
	uint8_t sound_id = 0;
	uint16_t screen_x = 0;
	uint32_t r1 = 0, fragment_cursor = 0, fragments_left = 0, free_count = 0, ignored = 0, object = 0, write_index_address = 0;
	uint16_t wait_timer = 0;

	OBJECT(current_object)->flags |= 1u;
	wait_timer = OBJECT(current_object)->spiked_roller.wait_timer;
	OBJECT(current_object)->spiked_roller.wait_timer = wait_timer - 1u;
	if ((int16_t)wait_timer > 1)
		return;
	OBJECT(current_object)->spiked_roller.state = 0u;
	chaser_object = current_object;
	OBJECT(current_object)->flags &= ~1u;
	scroll_resume_request = 0xFFu;
	project_object(&ignored, &r1);
	sound_id = 7u;
	play_sound(sound_id, &r1, &write_index_address);
	screen_x = (uint16_t)OBJECT(current_object)->screen_x;
	r14 = (uint16_t)OBJECT(current_object)->screen_y;
	fragments_left = roller_wall_fragment_count;
	fragment_cursor = roller_wall_fragments_ADDRESS;
	do
	{
		allocate_object(&free_count, &object);
		if (free_count == 0u)
			break;
		OBJECT(object)->sprite_flags = ROM16(fragment_cursor);
		OBJECT(object)->screen_x = screen_x;
		fragment_cursor += 2u;
		OBJECT(object)->screen_x += ROM16(fragment_cursor);
		OBJECT(object)->screen_y = r14;
		fragment_cursor += 2u;
		OBJECT(object)->screen_y += ROM16(fragment_cursor);
		fragment_cursor += 2u;
		OBJECT(object)->speed_x_fraction = ROM16(fragment_cursor);
		fragment_cursor += 2u;
		OBJECT(object)->speed_y_fraction = ROM16(fragment_cursor);
		fragment_cursor += 2u;
		OBJECT(object)->animation = rock_debris_animations_ADDRESS + ROM16(fragment_cursor);
		OBJECT(object)->sprite_palette = 0x8240u;
		OBJECT(object)->projected_x = (uint32_t)OBJECT(object)->screen_x << 4;
		OBJECT(object)->projected_y_fraction = (uint32_t)OBJECT(object)->screen_y << 4;
		OBJECT(object)->animation_frame = 0u;
		OBJECT(object)->animation_speed = 4u;
		OBJECT(object)->animation_timer = 0;
		OBJECT(object)->flags = 0u;
		OBJECT(object)->sprite_scale = 0x100u;
		OBJECT(object)->handler = rock_debris_fly_ADDRESS;
		fragment_cursor += 2u;
		fragments_left--;
	} while (fragments_left != 0u);
}

void spiked_roller_smash_wall(void)
{
	uint16_t margin_y = 0;
	uint32_t ignored = 0, off_screen = 0, x_margin = 0;

	OBJECT(current_object)->kind = 0xCu;
	chaser_speed = 0;
	if (stage == 0xEu)
		chaser_stopped = 0xFFu;
	x_margin = 0x68u;
	margin_y = 0x70u;
	off_screen = is_off_screen_margin((uint16_t)x_margin, margin_y);
	if (off_screen != 0u)
		OBJECT(current_object)->spiked_roller.state = 5u;
	update_screen_position(&ignored, &x_margin);
}

void spiked_roller_wait_for_turn(void)
{
	uint32_t ignored = 0;

	OBJECT(current_object)->kind = 0xCu;
	chaser_speed = 0;
	if (chaser_object == 0u)
	{
		OBJECT(current_object)->spiked_roller.state = 0u;
		chaser_object = current_object;
		OBJECT(current_object)->hitbox_height = 0x40u;
	}
	project_object(&ignored, &ignored);
}

void spiked_roller_wait_to_burst_out(void)
{
	uint32_t ignored = 0;

	OBJECT(current_object)->flags |= 1u;
	if (chaser_object == 0u)
	{
		OBJECT(current_object)->spiked_roller.state = 9u;
		OBJECT(current_object)->spiked_roller.wait_timer = 0x3C;
	}
	project_object(&ignored, &ignored);
}

void spiked_roller_shift_box(void)
{
	OBJECT(current_object)->spiked_roller.state = 0u;
	OBJECT(current_object)->hitbox_offset_y = -0x26;
	spiked_roller_chase();
}

void spiked_roller_spawn_fixed_zone(void)
{
	uint32_t free_count = 0, object = 0;

	allocate_object(&free_count, &object);
	if (free_count != 0u)
	{
		OBJECT(object)->sprite_flags = 0x1004u;
		OBJECT(object)->kind = 0xCu;
		OBJECT(object)->flags = 2u;
		OBJECT(object)->hitbox_size_x = 0x100u;
		OBJECT(object)->hitbox_size_y = 0xF3u;
		OBJECT(object)->direction = 0x300u;
		OBJECT(object)->hitbox_height = 0xA0u;
		OBJECT(object)->sprite_offset_x = 0;
		OBJECT(object)->sprite_offset_y = 0;
		OBJECT(object)->hitbox_offset_x = 0;
		OBJECT(object)->hitbox_offset_y = 0;
		OBJECT(object)->x = -0x8AD0000;
		OBJECT(object)->y = 0x114A0000;
		OBJECT(object)->z = 0x4700000;
		OBJECT(object)->handler = roller_fixed_zone_update_ADDRESS;
	}
	OBJECT(current_object)->spiked_roller.state = 0u;
	spiked_roller_chase();
}

void roller_fixed_zone_update(void)
{
	uint32_t ignored = 0;

	if (scroll_position < 0x7600u)
	{
		project_object(&ignored, &ignored);
		ignored = add_to_touchable_list(ignored);
		return;
	}
	delete_current_object();
}

void machine_start(void)
{
	uint16_t kind = 0, x = 0, y_2 = 0, z = 0;
	uint32_t block_offset = 0, depth = 0, free_count = 0, ignored = 0, object = 0, y = 0;

	OBJECT(current_object)->sprite_flags = 0x1006u;
	OBJECT(current_object)->sprite_palette = 0x9050u;
	OBJECT(current_object)->sprite_offset_x = 0;
	OBJECT(current_object)->sprite_offset_y = 0;
	OBJECT(current_object)->flags = 2u;
	OBJECT(current_object)->hitbox_offset_x = 0;
	OBJECT(current_object)->hitbox_offset_y = -0x10;
	OBJECT(current_object)->hitbox_size_x = 0xE8u;
	OBJECT(current_object)->hitbox_size_y = 0x20u;
	OBJECT(current_object)->hitbox_height = 0x50u;
	OBJECT(current_object)->kind = 0xCu;
	machine_wrecked = 0u;
	OBJECT(current_object)->handler = machine_run_ADDRESS;
	OBJECT(current_object)->animation = machine_animation_ADDRESS;
	OBJECT(current_object)->animation_frame = 0u;
	OBJECT(current_object)->animation_speed = 8u;
	OBJECT(current_object)->animation_timer = 0;
	find_floor(&free_count, &y, &block_offset, &ignored, &object);
	OBJECT(current_object)->z = OBJECT(current_object)->floor_z;
	project_object(&ignored, &depth);
	allocate_object(&free_count, &object);
	if (free_count != 0u)
	{
		OBJECT(object)->sprite_flags = 0x1004u;
		OBJECT(object)->sprite_palette = 0x9060u;
		OBJECT(object)->flags = 0u;
		OBJECT(object)->sprite_offset_x = 0;
		OBJECT(object)->sprite_offset_y = 0;
		OBJECT(object)->handler = machine_fire_update_ADDRESS;
		OBJECT(object)->machine_fire.state = 0u;
		OBJECT(object)->parent = current_object;
		OBJECT(object)->animation = machine_flame_animation_ADDRESS;
		OBJECT(object)->animation_frame = 0u;
		OBJECT(object)->animation_speed = 2u;
		OBJECT(object)->animation_timer = 0;
		OBJECT(object)->projected_x = OBJECT(current_object)->projected_x;
		OBJECT(object)->projected_x += 0x2Cu;
		OBJECT(object)->projected_y = OBJECT(current_object)->projected_y;
		OBJECT(object)->projected_y_int -= 0x59u;
		OBJECT(object)->z = OBJECT(current_object)->z;
		x = 0xF74Du;
		y_2 = 0xAEDu;
		kind = 5u;
		z = 0x43Eu;
		spawn_sign(x, y_2, kind, z, &depth, &object);
	}
}

void machine_run(void)
{
	uint32_t r1 = 0, r7 = 0, depth = 0, ignored = 0, off_screen = 0;

	if (machine_wrecked != 0u)
	{
		OBJECT(current_object)->animation_frame = 0xFFu;
		OBJECT(current_object)->sprite_frame = 0x68E8u;
	}
	off_screen = check_left_screen();
	if (off_screen != 0u)
	{
		delete_current_object();
		return;
	}
	update_screen_position(&ignored, &r1);
	if ((int8_t)OBJECT(current_object)->animation_frame >= 0)
		animate(&r1, &r7, &off_screen);
	add_to_touchable_list(ignored);
	depth = (uint16_t)(OBJECT(current_object)->screen_y - 0x68u);
	queue_sprite_at_depth(&ignored, &depth);
}

void machine_fire_update(void)
{
	uint8_t sound_id_2 = 0;
	uint32_t r1 = 0, r2 = 0, r7 = 0, r8 = 0, climb_speed = 0, free_count = 0, ignored = 0, object = 0, path_cursor = 0, point_x = 0, point_y = 0, projected_dx = 0, random_bits = 0, sound_id = 0, spawn_record = 0, x = 0;
	uint8_t hold_timer = 0, sound_timer = 0, sound_timer_2 = 0, lane_timer_before = 0, lane_before = 0, sound_timer_3 = 0;
	uint16_t spawns_left_before = 0, record_height = 0, wait_timer = 0;
	uint32_t flags = 0, lane_timer_address = 0;

	switch (OBJECT(current_object)->machine_fire.state)
	{
	case 0:
	case 8:
		if ((int8_t)OBJECT(LINKED(current_object, OBJECT(current_object)->parent))->animation_frame < 0)
			OBJECT(current_object)->machine_fire.state++;
		update_screen_position(&ignored, &r1);
		animate(&r1, &r7, &r8);
		r1 = queue_sprite(r1);
		return;
	case 1:
		OBJECT(current_object)->machine_fire.state++;
		OBJECT(current_object)->x = -0x8B40000;
		OBJECT(current_object)->y = 0xA900000;
		OBJECT(current_object)->flags = 2u;
		OBJECT(current_object)->hitbox_size_x = 0x90u;
		OBJECT(current_object)->hitbox_size_y = 0xD0u;
		OBJECT(current_object)->hitbox_height = 0x70u;
		OBJECT(current_object)->hitbox_offset_x = -0x48;
		OBJECT(current_object)->hitbox_offset_y = 0;
		OBJECT(current_object)->kind = 0x13u;
		OBJECT(current_object)->machine_fire.off_screen = 0xFFu;
		find_floor(&x, &r1, &r2, &ignored, &object);
		OBJECT(current_object)->z = OBJECT(current_object)->floor_z;
		OBJECT(current_object)->z += (uint32_t)OBJECT(current_object)->hitbox_height << 16;
		rising_fire_object = current_object;
		OBJECT(current_object)->machine_fire.path = machine_fire_script_ADDRESS;
		OBJECT(current_object)->machine_fire.holding_back = 0u;
		project_object(&ignored, &ignored);
		OBJECT(current_object)->machine_fire.projected_x_whole = OBJECT(current_object)->projected_x;
		OBJECT(current_object)->spawn_arg_1 = 0u;
		allocate_object(&free_count, &object);
		if (free_count != 0u)
		{
			OBJECT(object)->handler = flame_start_ADDRESS;
			OBJECT(object)->x = -0x8B80000;
			OBJECT(object)->y = 0xA500000;
			spawn_record = spawns_with_next_handler_ADDRESS;
			if (free_object_stack < (int16_t)spawns_with_next_handler)
				return;
			temp_0 = ROM16(spawn_record);
			spawn_record += 2u;
			do
			{
				allocate_object(&ignored, &object);
				OBJECT(object)->handler = fireball_start_ADDRESS;
				OBJECT(object)->var_7F = 0u;
				OBJECT(object)->x_int = ROM16(spawn_record);
				OBJECT(object)->x_fraction = 0u;
				spawn_record += 2u;
				OBJECT(object)->y_int = ROM16(spawn_record);
				OBJECT(object)->y_fraction = 0u;
				spawn_record += 2u;
				OBJECT(object)->var_96 = ROM16(spawn_record);
				spawn_record += 2u;
				AS16(OBJECT(object)->player) = ROM16(spawn_record);
				spawn_record += 2u;
				OBJECT(object)->z_int = ROM16(spawn_record);
				OBJECT(object)->z_fraction = 0u;
				spawn_record += 2u;
				OBJECT(object)->next_handler = ROM32(spawn_record);
				spawns_left_before = temp_0;
				temp_0 = spawns_left_before - 1u;
				spawn_record += 4u;
			} while ((int16_t)spawns_left_before > 1);
		}
		return;
	case 2:
		flags = OBJECT(current_object)->flags;
		OBJECT(current_object)->flags = flags | 0x80000000u;
		if ((flags & 0x80000000u) == 0u)
		{
		machine_fire_read_script:
			path_cursor = OBJECT(current_object)->machine_fire.path;
			record_height = mem_read16(path_cursor);
			path_cursor += 2u;
			if ((int16_t)record_height < (int16_t)(OBJECT(current_object)->z_int - OBJECT(current_object)->hitbox_height))
			{
			machine_fire_climb_step:
				if (OBJECT(current_object)->machine_fire.holding_back != 0u)
				{
					if ((int16_t)(scroll_position - OBJECT(current_object)->machine_fire.hold_start_position) >= 0x300)
						goto machine_fire_stop_holding_back;
					if (no_player_in_play != 0u)
						OBJECT(current_object)->machine_fire.hold_timer = 0xFFu;
					hold_timer = OBJECT(current_object)->machine_fire.hold_timer;
					OBJECT(current_object)->machine_fire.hold_timer = hold_timer - 1u;
					if (hold_timer == 0u)
						goto machine_fire_stop_holding_back;
				}
				if ((int16_t)(OBJECT(current_object)->screen_y - OBJECT(current_object)->hitbox_height) <= OBJECT(current_object)->machine_fire.screen_threshold)
				{
				machine_fire_use_base_speed:
					climb_speed = (uint16_t)OBJECT(current_object)->machine_fire.base_speed;
				}
				else
				{
					if (OBJECT(current_object)->machine_fire.holding_back != 0u)
					{
					machine_fire_stop_holding_back:
						OBJECT(current_object)->machine_fire.holding_back = 0u;
						OBJECT(current_object)->machine_fire.screen_threshold = OBJECT(current_object)->machine_fire.saved_threshold;
						OBJECT(current_object)->machine_fire.base_speed = OBJECT(current_object)->machine_fire.saved_base_speed;
					}
					compute_camera_ground_speed(0u);
					climb_speed = (uint16_t)((uint32_t)camera_ground_speed_int * OBJECT(current_object)->machine_fire.catch_up_factor) >> 8;
					if ((int16_t)climb_speed <= OBJECT(current_object)->machine_fire.base_speed)
						goto machine_fire_use_base_speed;
				}
				OBJECT(current_object)->speed_fraction = climb_speed;
				wait_timer = OBJECT(current_object)->machine_fire.wait_timer;
				OBJECT(current_object)->machine_fire.wait_timer = wait_timer - 1u;
				if ((int16_t)wait_timer <= 1)
				{
					OBJECT(current_object)->machine_fire.wait_timer = 0;
					OBJECT(current_object)->z -= (uint32_t)(int16_t)OBJECT(current_object)->speed_fraction << 12;
				}
				projected_dx = (int32_t)((uint32_t)(int16_t)ROM16(floor_attribute_slopes_ADDRESS + OBJECT(current_object)->floor_attribute) * OBJECT(current_object)->speed_fraction) >> 4;
				OBJECT(current_object)->machine_fire.fixed_projected_x += projected_dx;
				OBJECT(current_object)->projected_x = OBJECT(current_object)->machine_fire.projected_x_whole;
				OBJECT(current_object)->projected_y -= (int32_t)projected_dx >> 1;
				update_screen_position(&sound_id, &climb_speed);
				if (OBJECT(current_object)->screen_y > 0x160)
					OBJECT(current_object)->machine_fire.off_screen = 0xFFu;
				else
				{
					if (OBJECT(current_object)->machine_fire.off_screen != 0u)
					{
						OBJECT(current_object)->machine_fire.off_screen = 0u;
						OBJECT(current_object)->machine_fire.sound_timer = 0;
					}
					if (current_game_mode != 7u)
					{
						sound_timer = OBJECT(current_object)->machine_fire.sound_timer;
						OBJECT(current_object)->machine_fire.sound_timer = sound_timer - 1u;
						if ((int8_t)sound_timer <= 1)
						{
							OBJECT(current_object)->machine_fire.sound_timer = 0x3C;
							sound_id = 9u;
							play_sound((uint8_t)sound_id, &climb_speed, &r2);
						}
					}
				}
				point_x = OBJECT(current_object)->machine_fire.fixed_projected_x;
				point_y = OBJECT(current_object)->projected_y * 2u;
				unproject_point(&point_x, &point_y);
				OBJECT(current_object)->x = point_x;
				OBJECT(current_object)->y = point_y;
				sound_id = add_to_touchable_list(sound_id);
				return;
			}
			if ((int16_t)mem_read16(path_cursor + 2u) >= 0)
			{
				OBJECT(current_object)->machine_fire.screen_threshold = mem_read16(path_cursor);
				path_cursor += 2u;
				OBJECT(current_object)->machine_fire.catch_up_factor = mem_read16(path_cursor);
				path_cursor += 2u;
				OBJECT(current_object)->machine_fire.base_speed = mem_read16(path_cursor);
				path_cursor += 2u;
				OBJECT(current_object)->machine_fire.wait_timer = mem_read16(path_cursor);
				OBJECT(current_object)->machine_fire.path = path_cursor + 2u;
				goto machine_fire_climb_step;
			}
			OBJECT(current_object)->machine_fire.state++;
			sound_timer_2 = OBJECT(current_object)->machine_fire.sound_timer;
			OBJECT(current_object)->machine_fire.sound_timer = sound_timer_2 - 1u;
			if ((int8_t)sound_timer_2 <= 1)
			{
				OBJECT(current_object)->machine_fire.sound_timer = 0x3C;
				sound_id_2 = 9u;
				play_sound(sound_id_2, &r1, &r2);
			}
			update_screen_position(&ignored, &r1);
			ignored = add_to_touchable_list(ignored);
			return;
		}
		if (OBJECT(current_object)->machine_fire.holding_back != 0u)
			goto machine_fire_climb_step;
		if (no_player_in_play != 0u || player_mishap != 0u || player_dropping_in != 0u)
		{
			OBJECT(current_object)->machine_fire.holding_back = 0xFFu;
			OBJECT(current_object)->machine_fire.saved_base_speed = OBJECT(current_object)->machine_fire.base_speed;
			OBJECT(current_object)->machine_fire.base_speed = 0;
			OBJECT(current_object)->machine_fire.hold_start_position = scroll_position;
			OBJECT(current_object)->machine_fire.hold_timer = 0xFFu;
			OBJECT(current_object)->machine_fire.saved_threshold = OBJECT(current_object)->machine_fire.screen_threshold;
			OBJECT(current_object)->machine_fire.screen_threshold = 0xE0;
			goto machine_fire_climb_step;
		}
		goto machine_fire_read_script;
	case 3:
		OBJECT(current_object)->machine_fire.state++;
		OBJECT(current_object)->platform_list_next = 0u;
		OBJECT(current_object)->machine_fire.move_frames = 0;
		OBJECT(current_object)->z = 0x100000;
		OBJECT(current_object)->x = -0x8520000;
		OBJECT(current_object)->y = 0xA790000;
		OBJECT(current_object)->hitbox_size_x = 0x58u;
		OBJECT(current_object)->hitbox_size_y = 0x1A2u;
		OBJECT(current_object)->hitbox_offset_x = -0x68;
		OBJECT(current_object)->hitbox_offset_y = -0x60;
		OBJECT(current_object)->kind = 0x14u;
		OBJECT(current_object)->direction = 0x100u;
	case 4:
		OBJECT(current_object)->x += 0x10000u;
		OBJECT(current_object)->machine_fire.move_frames++;
		if (OBJECT(current_object)->machine_fire.move_frames >= 0x80)
			OBJECT(current_object)->machine_fire.state++;
		find_floor(&x, &r1, &r2, &random_bits, &object);
		fireball_lane_timer_cursor = current_object + 0x72u;
		fireball_lane = 3;
		for (;;)
		{
			lane_timer_address = fireball_lane_timer_cursor;
			lane_timer_before = mem_read8(lane_timer_address);
			mem_write8(lane_timer_address, lane_timer_before - 1u);
			if ((int8_t)lane_timer_before > 1)
			{
				fireball_lane_timer_cursor++;
				lane_before = fireball_lane;
				fireball_lane = lane_before - 1u;
				if ((int8_t)lane_before <= 1)
					break;
				continue;
			}
			next_random(&random_bits, &ignored, &ignored);
			random_bits = (random_bits & ~0xFFu) | (random_bits & 0x1Fu);
			mem_write8(fireball_lane_timer_cursor, random_bits);
			allocate_object(&free_count, &object);
			if (free_count == 0u)
				break;
			OBJECT(object)->handler = fireball_start_ADDRESS;
			OBJECT(object)->var_7F = 0u;
			OBJECT(object)->x = OBJECT(current_object)->x;
			OBJECT(object)->y = OBJECT(current_object)->y;
			OBJECT(object)->y_int += (uint8_t)fireball_lane * 0x38u;
			OBJECT(object)->var_96 = OBJECT(object)->x_int;
			OBJECT(object)->var_96 += mem_read8(fireball_lane_timer_cursor);
			AS16(OBJECT(object)->player) = OBJECT(object)->y_int;
			OBJECT(object)->z = OBJECT(current_object)->z;
			OBJECT(object)->next_handler = flame_start_ADDRESS;
			fireball_lane_timer_cursor++;
			lane_before = fireball_lane;
			fireball_lane = lane_before - 1u;
			if ((int8_t)lane_before <= 1)
				break;
		}
		sound_timer_3 = OBJECT(current_object)->machine_fire.sound_timer;
		OBJECT(current_object)->machine_fire.sound_timer = sound_timer_3 - 1u;
		if ((int8_t)sound_timer_3 <= 1)
		{
			OBJECT(current_object)->machine_fire.sound_timer = 0x3C;
			sound_id_2 = 9u;
			play_sound(sound_id_2, &r1, &r2);
		}
		project_object(&ignored, &ignored);
		ignored = add_to_touchable_list(ignored);
		return;
	case 5:
		machine_fire_done = 0xFFu;
		OBJECT(current_object)->machine_fire.state++;
		if ((int16_t)(OBJECT(current_object)->screen_y - OBJECT(current_object)->hitbox_height) > 0x110 || OBJECT(current_object)->screen_x <= -0xA0)
		{
			OBJECT(current_object)->machine_fire.state++;
			sound_timer_2 = OBJECT(current_object)->machine_fire.sound_timer;
			OBJECT(current_object)->machine_fire.sound_timer = sound_timer_2 - 1u;
			if ((int8_t)sound_timer_2 <= 1)
			{
				OBJECT(current_object)->machine_fire.sound_timer = 0x3C;
				sound_id_2 = 9u;
				play_sound(sound_id_2, &r1, &r2);
			}
			update_screen_position(&ignored, &r1);
			ignored = add_to_touchable_list(ignored);
			return;
		}
		sound_timer_2 = OBJECT(current_object)->machine_fire.sound_timer;
		OBJECT(current_object)->machine_fire.sound_timer = sound_timer_2 - 1u;
		if ((int8_t)sound_timer_2 <= 1)
		{
			OBJECT(current_object)->machine_fire.sound_timer = 0x3C;
			sound_id_2 = 9u;
			play_sound(sound_id_2, &r1, &r2);
		}
		update_screen_position(&ignored, &r1);
		ignored = add_to_touchable_list(ignored);
		return;
	case 6:
		if ((int16_t)(OBJECT(current_object)->screen_y - OBJECT(current_object)->hitbox_height) > 0x110 || OBJECT(current_object)->screen_x <= -0xA0)
			OBJECT(current_object)->machine_fire.state++;
		sound_timer_2 = OBJECT(current_object)->machine_fire.sound_timer;
		OBJECT(current_object)->machine_fire.sound_timer = sound_timer_2 - 1u;
		if ((int8_t)sound_timer_2 <= 1)
		{
			OBJECT(current_object)->machine_fire.sound_timer = 0x3C;
			sound_id_2 = 9u;
			play_sound(sound_id_2, &r1, &r2);
		}
		update_screen_position(&ignored, &r1);
		ignored = add_to_touchable_list(ignored);
		return;
	case 7:
		delete_current_object();
		return;
	default:
		unknown_code(0x8358Cu + OBJECT(current_object)->machine_fire.state * 3u);
	}
}

void start_fireball_flipped(void)
{
	OBJECT(current_object)->sprite_flags = 0x1044u;
	OBJECT(current_object)->sprite_palette = 0x10C0u;
	init_fireball_flight();
}

void fireball_start(void)
{
	OBJECT(current_object)->sprite_flags = 0x1004u;
	OBJECT(current_object)->sprite_palette = 0x9010u;
	init_fireball_flight();
}

void init_fireball_flight(void)
{
	uint8_t sound_id = 0;
	uint32_t flight_frames = 0, write_index_address = 0;

	OBJECT(current_object)->sprite_offset_x = 0;
	OBJECT(current_object)->sprite_offset_y = 0;
	OBJECT(current_object)->flags = 0u;
	OBJECT(current_object)->animation = fireball_animation_ADDRESS;
	OBJECT(current_object)->animation_frame = 0u;
	OBJECT(current_object)->animation_timer = 0;
	OBJECT(current_object)->floor_z = OBJECT(current_object)->z;
	OBJECT(current_object)->init_fireball_flight.state = 0u;
	OBJECT(current_object)->animation_speed = 3u;
	OBJECT(current_object)->init_fireball_flight.flight_timer = 0x36;
	if (stage == 0xCu)
	{
		OBJECT(current_object)->animation_speed = 2u;
		OBJECT(current_object)->init_fireball_flight.flight_timer = 0x24;
	}
	flight_frames = (uint16_t)OBJECT(current_object)->init_fireball_flight.flight_timer;
	OBJECT(current_object)->speed_x = divide_s32(((uint32_t)(uint16_t)OBJECT(current_object)->init_fireball_flight.target_x << 16) - OBJECT(current_object)->x, flight_frames);
	OBJECT(current_object)->speed_y = divide_s32(((uint32_t)(uint16_t)OBJECT(current_object)->init_fireball_flight.target_y << 16) - OBJECT(current_object)->y, flight_frames);
	OBJECT(current_object)->speed_z = 0;
	OBJECT(current_object)->direction = 0u;
	sound_id = 0xFu;
	play_sound(sound_id, &flight_frames, &write_index_address);
	OBJECT(current_object)->handler = fireball_fly_ADDRESS;
}

void fireball_fly(void)
{
	uint32_t r1 = 0, arc_step = 0, at_last_step = 0, ignored = 0;
	uint16_t flight_timer = 0;

	switch (OBJECT(current_object)->fireball_fly.state)
	{
	case 0:
	case 2:
		OBJECT(current_object)->x += OBJECT(current_object)->speed_x;
		OBJECT(current_object)->y += OBJECT(current_object)->speed_y;
		arc_step = 9u;
		if (stage == 0xCu)
			arc_step = 0xEu;
		OBJECT(current_object)->fireball_fly.arc_angle += arc_step;
		OBJECT(current_object)->z = sine_table[OBJECT(current_object)->direction & 0x3FFu] * 0xFFFFFEC0u + OBJECT(current_object)->floor_z;
		animate_and_move(0u);
		flight_timer = OBJECT(current_object)->fireball_fly.flight_timer;
		OBJECT(current_object)->fireball_fly.flight_timer = flight_timer - 1u;
		if ((int16_t)flight_timer <= 1)
		{
			OBJECT(current_object)->fireball_fly.state++;
			OBJECT(current_object)->z = OBJECT(current_object)->floor_z;
		}
		project_object(&ignored, &ignored);
		ignored = queue_sprite_at_floor();
		return;
	case 1:
		at_last_step = animate_and_move(0u);
		if ((uint8_t)at_last_step != 0u)
		{
			OBJECT(current_object)->handler = OBJECT(current_object)->next_handler;
			OBJECT(current_object)->fireball_fly.state = 0u;
			OBJECT(current_object)->z -= 0x280000u;
		}
		update_screen_position(&ignored, &r1);
		r1 = queue_sprite(r1);
		return;
	default:
		unknown_code(0x83AA5u + OBJECT(current_object)->fireball_fly.state * 2u);
	}
}

void flame_start(void)
{
	uint32_t block_offset = 0, cursor = 0, ignored = 0, x = 0, y = 0;

	OBJECT(current_object)->sprite_flags = 0x1000u;
	OBJECT(current_object)->sprite_palette = 0x9010u;
	OBJECT(current_object)->sprite_offset_x = 0;
	OBJECT(current_object)->sprite_offset_y = 0;
	OBJECT(current_object)->flags = 0u;
	OBJECT(current_object)->animation = flame_animation_ADDRESS;
	OBJECT(current_object)->animation_frame = 0u;
	OBJECT(current_object)->animation_speed = 2u;
	OBJECT(current_object)->animation_timer = 0;
	OBJECT(current_object)->handler = flame_burn_ADDRESS;
	find_floor(&x, &y, &block_offset, &ignored, &cursor);
	OBJECT(current_object)->z = OBJECT(current_object)->floor_z;
	project_object(&ignored, &ignored);
}

void flame_burn(void)
{
	uint32_t r1 = 0, r7 = 0, depth = 0, ignored = 0, off_screen = 0;

	off_screen = check_left_screen();
	if (off_screen == 0u)
	{
		update_screen_position(&ignored, &r1);
		animate(&r1, &r7, &off_screen);
		depth = (uint16_t)(OBJECT(current_object)->screen_y + 0x30u);
		queue_sprite_at_depth(&ignored, &depth);
		return;
	}
	delete_current_object();
}

void rising_flame_update(void)
{
	uint32_t r1 = 0, r7 = 0, r8 = 0, depth = 0, ignored = 0;

	switch (OBJECT(current_object)->rising_flame.state)
	{
	case 0:
		rising_flame_init();
		break;
	case 1:
		rising_flame_rise();
		break;
	case 2:
		rising_flame_spawn_next();
		break;
	case 3:
		rising_flame_wait_offscreen();
		break;
	case 4:
		rising_flame_init();
		break;
	default:
		unknown_code(0x83BF7u + OBJECT(current_object)->rising_flame.state * 3u);
	}
	if (OBJECT(LINKED(current_object, rising_fire_object))->var_7F == 7u || OBJECT(current_object)->rising_flame.off_screen != 0u)
	{
		delete_current_object();
		return;
	}
	animate(&r1, &r7, &r8);
	update_screen_position(&ignored, &r1);
	if (OBJECT(current_object)->screen_y > 0x108)
		return;
	depth = (uint16_t)(OBJECT(current_object)->screen_y + 0x20u);
	queue_sprite_at_depth(&ignored, &depth);
}

void rising_flame_init(void)
{
	uint32_t ignored = 0;

	OBJECT(current_object)->rising_flame.state++;
	OBJECT(current_object)->sprite_flags = 0x1000u;
	OBJECT(current_object)->sprite_palette = 0x9010u;
	if (stage == 8u)
		OBJECT(current_object)->sprite_palette = 0x9260u;
	OBJECT(current_object)->sprite_offset_x = 0;
	OBJECT(current_object)->sprite_offset_y = 0;
	OBJECT(current_object)->flags = 0u;
	OBJECT(current_object)->animation = flame_animation_ADDRESS;
	OBJECT(current_object)->animation_frame = 0u;
	OBJECT(current_object)->animation_speed = 2u;
	OBJECT(current_object)->animation_timer = 0;
	OBJECT(current_object)->rising_flame.off_screen = 0u;
	OBJECT(current_object)->floor_z = OBJECT(current_object)->z;
	project_object(&ignored, &ignored);
}

void rising_flame_rise(void)
{
	OBJECT(current_object)->z -= 0x10000u;
	if ((int16_t)(OBJECT(current_object)->floor_z_int - OBJECT(current_object)->z_int) >= 0x28)
		OBJECT(current_object)->rising_flame.state++;
}

void rising_flame_spawn_next(void)
{
	uint32_t free_count = 0, object = 0, object_2 = 0, object_3 = 0;

	object = LINKED(current_object, rising_fire_object);
	if (OBJECT(object)->var_7F != 2u)
		OBJECT(current_object)->rising_flame.state++;
	else if ((int16_t)(OBJECT(object)->z_int - OBJECT(object)->hitbox_height) <= OBJECT(current_object)->z_int)
	{
		allocate_object(&free_count, &object_2);
		if (free_count == 0u)
			return;
		OBJECT(object_2)->handler = rising_flame_update_ADDRESS;
		OBJECT(object_2)->rising_flame.state = 0u;
		object_3 = LINKED(current_object, rising_fire_object);
		OBJECT(object_2)->x = OBJECT(object_3)->x;
		OBJECT(object_2)->y = OBJECT(current_object)->y;
		OBJECT(object_2)->z = OBJECT(object_3)->z;
		OBJECT(object_2)->z_int -= OBJECT(object_3)->hitbox_height;
		OBJECT(current_object)->rising_flame.state++;
		return;
	}
}

void rising_flame_wait_offscreen(void)
{
	if (OBJECT(current_object)->screen_y >= 0x110 || OBJECT(current_object)->screen_x <= -0x40)
		OBJECT(current_object)->rising_flame.off_screen = 0xFFu;
}

uint32_t spawn_stage_03_end_pillar(uint32_t pillar)
{
	uint32_t free_count = 0;

	allocate_object_outside_loop(&free_count, &pillar);
	if (free_count != 0u)
	{
		OBJECT(pillar)->sprite_flags = 0x1004u;
		OBJECT(pillar)->sprite_frame = 0x6600u;
		OBJECT(pillar)->sprite_palette = 0x8740u;
		OBJECT(pillar)->sprite_offset_x = 0;
		OBJECT(pillar)->sprite_offset_y = 0;
		OBJECT(pillar)->flags = 0u;
		OBJECT(pillar)->handler = stage_03_end_pillar_show_ADDRESS;
		OBJECT(pillar)->screen_x = 0x146;
		OBJECT(pillar)->screen_y = 0x75;
	}
	return pillar;
}

void stage_03_end_pillar_show(void)
{
	uint32_t depth = 0, ignored = 0;

	if (map_screen_active == 0u)
	{
		depth = (uint16_t)(OBJECT(current_object)->screen_y + 0x20u);
		queue_sprite_at_depth(&ignored, &depth);
		return;
	}
	delete_current_object();
}

void exit_gate_start(uint32_t sign)
{
	uint16_t kind = 0, x_2 = 0, y_2 = 0, z = 0;
	uint32_t block_offset = 0, ignored = 0, x = 0, y = 0;

	OBJECT(current_object)->sprite_flags = 0x1005u;
	OBJECT(current_object)->sprite_frame = 0x6520u;
	OBJECT(current_object)->sprite_palette = 0x86A0u;
	OBJECT(current_object)->flags = 0u;
	OBJECT(current_object)->sprite_offset_x = 0;
	OBJECT(current_object)->sprite_offset_y = 0;
	OBJECT(current_object)->z = 0;
	OBJECT(current_object)->speed_z = 0;
	OBJECT(current_object)->handler = exit_gate_update_ADDRESS;
	sign_stop_flag = 0u;
	find_floor(&x, &y, &block_offset, &ignored, &sign);
	OBJECT(current_object)->z = OBJECT(current_object)->floor_z;
	OBJECT(current_object)->z_int -= 0xE0u;
	project_object(&ignored, &ignored);
	x_2 = 0x942u;
	y_2 = 0x10E0u;
	kind = 8u;
	z = (uint16_t)(OBJECT(current_object)->floor_z_int - 0x50u);
	spawn_sign_2(x_2, y_2, kind, z, sign);
}

void exit_gate_update(void)
{
	uint8_t sound_id = 0;
	uint32_t r1 = 0, ignored = 0, write_index_address = 0;

	if (map_screen_active != 0u)
	{
		delete_current_object();
		return;
	}
	if ((uint32_t)OBJECT(current_object)->z != (uint32_t)OBJECT(current_object)->floor_z && AS32(players_finished[0]) == 0xFFFFFFu)
	{
		OBJECT(current_object)->speed_z += 0x6000u;
		OBJECT(current_object)->z += OBJECT(current_object)->speed_z;
		if (OBJECT(current_object)->floor_z_int <= OBJECT(current_object)->z_int)
		{
			OBJECT(current_object)->z = OBJECT(current_object)->floor_z;
			sign_stop_flag = 0xFFu;
			sound_id = 0x20u;
			play_sound(sound_id, &r1, &write_index_address);
		}
	}
	update_screen_position(&ignored, &r1);
	r1 = queue_sprite(r1);
}

void ram_target_start(void)
{
	uint32_t block_offset = 0, cursor = 0, ignored = 0, x = 0, y = 0;

	OBJECT(current_object)->sprite_flags = 0x1045u;
	OBJECT(current_object)->sprite_frame = 0x6A58u;
	OBJECT(current_object)->sprite_palette = 0x91A0u;
	OBJECT(current_object)->flags = 2u;
	OBJECT(current_object)->handler = ram_target_update_ADDRESS;
	OBJECT(current_object)->sprite_offset_x = 6;
	OBJECT(current_object)->sprite_offset_y = 0x16;
	OBJECT(current_object)->hitbox_offset_x = -0x30;
	OBJECT(current_object)->hitbox_offset_y = -0x20;
	OBJECT(current_object)->hitbox_size_x = 0x30u;
	OBJECT(current_object)->hitbox_size_y = 0x10u;
	OBJECT(current_object)->hitbox_height = 0x28u;
	OBJECT(current_object)->kind = 0x21u;
	OBJECT(current_object)->direction = 0x300u;
	OBJECT(current_object)->ram_target.appear_state = 0;
	OBJECT(current_object)->ram_target.hit_count = 0;
	OBJECT(current_object)->animation = ram_target_animation_ADDRESS;
	OBJECT(current_object)->animation_frame = 0u;
	OBJECT(current_object)->animation_speed = 4u;
	OBJECT(current_object)->animation_timer = 0;
	OBJECT(current_object)->ram_target.damage_script = ram_target_damage_script_ADDRESS;
	find_floor(&x, &y, &block_offset, &ignored, &cursor);
	OBJECT(current_object)->z = OBJECT(current_object)->floor_z;
	project_object(&ignored, &ignored);
}

void ram_target_update(void)
{
	uint16_t margin = 0;
	uint32_t off_screen = 0;

	OBJECT(current_object)->health = 0x7F;
	margin = 0x40u;
	off_screen = check_left_screen_margin(margin);
	if (off_screen != 0u)
	{
		delete_current_object();
		return;
	}
	if ((uint8_t)OBJECT(current_object)->ram_target.appear_state == 0u && OBJECT(current_object)->screen_x >= 0x50)
		OBJECT(current_object)->ram_target.appear_state = -1;
	ram_target_draw();
}

void ram_target_check_hits(void)
{
	uint32_t animation = 0, script_record = 0;

	if (roller_ram_hit == 0u)
	{
		ram_target_draw();
		return;
	}
	OBJECT(current_object)->animation_frame = 0u;
	OBJECT(current_object)->ram_target.hit_count++;
	if (OBJECT(current_object)->ram_target.hit_count < (int8_t)mem_read8(OBJECT(current_object)->ram_target.damage_script))
	{
		ram_target_draw();
		return;
	}
	script_record = OBJECT(current_object)->ram_target.damage_script + 2u;
	animation = mem_read16(script_record) + ram_target_damage_script_ADDRESS;
	OBJECT(current_object)->animation = animation;
	OBJECT(current_object)->ram_target.damage_script = script_record + 2u;
	if (OBJECT(current_object)->ram_target.hit_count < 4)
	{
		ram_target_draw();
		return;
	}
	ram_target_burst(animation);
	ram_target_draw();
}

void ram_target_draw(void)
{
	uint32_t r1 = 0, r7 = 0, at_last_step = 0, depth = 0, ignored = 0;

	if ((uint8_t)OBJECT(current_object)->ram_target.appear_state != 0u && (int8_t)OBJECT(current_object)->animation_frame >= 0)
	{
		animate(&r1, &r7, &at_last_step);
		if ((uint8_t)at_last_step != 0u)
		{
			OBJECT(current_object)->ram_target.appear_state = 1;
			OBJECT(current_object)->animation_frame = 0xFFu;
		}
	}
	if ((uint8_t)OBJECT(current_object)->ram_target.appear_state != 0u && OBJECT(current_object)->ram_target.appear_state >= 0)
		add_to_zone_list(0u);
	update_screen_position(&ignored, &r1);
	depth = (uint16_t)(OBJECT(current_object)->screen_y - OBJECT(current_object)->sprite_offset_y + 0x10u);
	if ((uint8_t)OBJECT(current_object)->ram_target.appear_state == 0u)
		depth = (uint16_t)(depth - 0x30u);
	queue_sprite_at_depth(&ignored, &depth);
}

void ram_target_burst(uint32_t piece)
{
	uint16_t screen_x = 0;
	uint32_t free_count = 0, piece_record = 0, pieces_left = 0;

	screen_x = (uint16_t)OBJECT(current_object)->screen_x;
	r14 = (uint16_t)OBJECT(current_object)->screen_y;
	pieces_left = ram_target_debris_count;
	piece_record = ram_target_debris_ADDRESS;
	do
	{
		allocate_object_outside_loop(&free_count, &piece);
		if (free_count == 0u)
			break;
		OBJECT(piece)->sprite_flags = ROM16(piece_record);
		OBJECT(piece)->screen_x = screen_x;
		piece_record += 2u;
		OBJECT(piece)->screen_x += ROM16(piece_record);
		OBJECT(piece)->screen_y = r14;
		piece_record += 2u;
		OBJECT(piece)->screen_y += ROM16(piece_record);
		piece_record += 2u;
		OBJECT(piece)->speed_x_fraction = ROM16(piece_record);
		piece_record += 2u;
		OBJECT(piece)->speed_y_fraction = ROM16(piece_record);
		piece_record += 2u;
		OBJECT(piece)->animation = ram_target_debris_animations_ADDRESS + ROM16(piece_record);
		OBJECT(piece)->sprite_palette = 0x8240u;
		OBJECT(piece)->projected_x = (uint32_t)OBJECT(piece)->screen_x << 4;
		OBJECT(piece)->projected_y_fraction = (uint32_t)OBJECT(piece)->screen_y << 4;
		OBJECT(piece)->animation_frame = 0u;
		OBJECT(piece)->animation_speed = 4u;
		OBJECT(piece)->animation_timer = 0;
		OBJECT(piece)->flags = 0u;
		OBJECT(piece)->sprite_scale = 0x100u;
		OBJECT(piece)->handler = rock_debris_fly_ADDRESS;
		piece_record += 2u;
		pieces_left--;
	} while (pieces_left != 0u);
}

void ram_target_part_start(void)
{
	uint32_t block_offset = 0, cursor = 0, ignored = 0, part_list = 0, x = 0, y = 0;

	OBJECT(current_object)->ram_target.variant = OBJECT(current_object)->sprite_frame;
	part_list = ram_target_part_list_2_ADDRESS;
	if (OBJECT(current_object)->sprite_frame != 0u)
		part_list = ram_target_part_list_3_ADDRESS;
	OBJECT(current_object)->part_list = part_list;
	OBJECT(current_object)->sprite_flags = 0xD006u;
	OBJECT(current_object)->flags = 0u;
	OBJECT(current_object)->sprite_offset_x = 2;
	OBJECT(current_object)->sprite_offset_y = 1;
	OBJECT(current_object)->ram_target.hit_count = 0;
	OBJECT(current_object)->handler = ram_target_part_update_ADDRESS;
	find_floor(&x, &y, &block_offset, &ignored, &cursor);
	OBJECT(current_object)->z = OBJECT(current_object)->floor_z;
	project_object(&ignored, &ignored);
}

void ram_target_part_update(void)
{
	uint32_t depth = 0, ignored = 0, margin = 0, off_screen = 0;

	if (OBJECT(LINKED(current_object, chaser_object))->y_int <= (int16_t)(OBJECT(current_object)->y_int + 0x60u) && roller_ram_hit != 0u)
	{
		OBJECT(current_object)->ram_target.hit_count++;
		if (OBJECT(current_object)->ram_target.hit_count >= 4)
		{
			if (OBJECT(current_object)->ram_target.variant != 0u)
			{
				delete_current_object();
				return;
			}
			OBJECT(current_object)->sprite_flags = 0x1049u;
			OBJECT(current_object)->sprite_frame = 0x6A88u;
			OBJECT(current_object)->sprite_palette = 0x11B0u;
			OBJECT(current_object)->sprite_offset_x = 2;
			OBJECT(current_object)->sprite_offset_y = -0x19;
		}
	}
	margin = 0x88u;
	off_screen = check_left_screen_margin((uint16_t)margin);
	if (off_screen == 0u)
	{
		update_screen_position(&ignored, &margin);
		depth = (uint16_t)(OBJECT(current_object)->screen_y - 0x28u);
		queue_sprite_at_depth(&ignored, &depth);
		return;
	}
	delete_current_object();
}

void fire_vent_spawner_start(void)
{
	fire_vent_cursor = fire_vent_list_ADDRESS;
	OBJECT(current_object)->handler = fire_vent_spawner_update_ADDRESS;
	fire_vent_spawner_update();
}

void fire_vent_spawner_update(void)
{
	uint32_t free_count = 0, object = 0, vent_record = 0;
	uint16_t spawn_position = 0;

	vent_record = fire_vent_cursor;
	spawn_position = ROM16(vent_record);
	vent_record += 2u;
	if (scroll_position < spawn_position)
		return;
	if ((int16_t)ROM16(vent_record + 6u) < 0)
	{
		delete_current_object();
		return;
	}
	allocate_object(&free_count, &object);
	if (free_count != 0u)
	{
		OBJECT(object)->x_int = ROM16(vent_record);
		OBJECT(object)->x_fraction = 0u;
		vent_record += 2u;
		OBJECT(object)->y_int = ROM16(vent_record);
		OBJECT(object)->y_fraction = 0u;
		vent_record += 2u;
		OBJECT(object)->z_int = ROM16(vent_record);
		OBJECT(object)->z_fraction = 0u;
		OBJECT(object)->fire_vent.blast_delay = ROM16(vent_record + 2u);
		OBJECT(object)->sprite_flags = 0x1006u;
		OBJECT(object)->sprite_frame = 0x6A48u;
		OBJECT(object)->sprite_palette = 0x9110u;
		OBJECT(object)->flags = 2u;
		OBJECT(object)->handler = fire_vent_start_ADDRESS;
		OBJECT(object)->fire_vent.burst_state = 0u;
		OBJECT(object)->fire_vent.palette_list = fire_vent_glow_palettes_ADDRESS;
		OBJECT(object)->fire_vent.palette_index = 0u;
		OBJECT(object)->fire_vent.palette_delay = 8u;
		OBJECT(object)->fire_vent.palette_timer = 0u;
		OBJECT(object)->sprite_offset_x = 0;
		OBJECT(object)->sprite_offset_y = 0x10;
		OBJECT(object)->kind = 0x22u;
		OBJECT(object)->hitbox_size_x = 0x90u;
		OBJECT(object)->hitbox_size_y = 0x48u;
		OBJECT(object)->hitbox_height = 0x28u;
		OBJECT(object)->hitbox_offset_x = -0x48;
		OBJECT(object)->hitbox_offset_y = -8;
		fire_vent_cursor += 0xAu;
	}
}

void fire_vent_start(void)
{
	uint32_t ignored = 0;

	project_object(&ignored, &ignored);
	OBJECT(current_object)->handler = fire_vent_update_ADDRESS;
	fire_vent_update();
}

void fire_vent_update(void)
{
	uint32_t zero = 0, negative = 0, overflow = 0;
	uint8_t sound_id = 0;
	uint32_t depth = 0, free_count = 0, ignored = 0, margin = 0, object = 0, off_screen = 0, write_index_address = 0;
	uint16_t blast_delay = 0, next_delay = 0;

	margin = 0x40u;
	off_screen = check_left_screen_margin((uint16_t)margin);
	if (off_screen != 0u)
	{
		delete_current_object();
		return;
	}
	if ((int8_t)OBJECT(current_object)->fire_vent.palette_index >= 0)
	{
		if (OBJECT(current_object)->fire_vent.blast_delay <= 0x1E)
			OBJECT(current_object)->fire_vent.palette_delay = 2u;
		animate_sign();
	}
	if ((uint16_t)OBJECT(current_object)->fire_vent.blast_delay == 0u)
	{
	fire_vent_blast:
		OBJECT(current_object)->fire_vent.blast_delay = -1;
		OBJECT(current_object)->fire_vent.burst_state = 1u;
		OBJECT(current_object)->sprite_frame = 0x6A50u;
		OBJECT(current_object)->sprite_palette = 0x9120u;
		OBJECT(current_object)->fire_vent.palette_index = 0xFFu;
		allocate_object(&free_count, &object);
		if (free_count != 0u)
		{
			OBJECT(object)->sprite_flags = 0x1009u;
			OBJECT(object)->sprite_palette = 0x90B0u;
			OBJECT(object)->sprite_offset_x = 0;
			OBJECT(object)->sprite_offset_y = 0;
			OBJECT(object)->flags = 0u;
			OBJECT(object)->handler = fire_vent_burst_animate_ADDRESS;
			OBJECT(object)->parent = current_object;
			OBJECT(object)->animation = fire_vent_blast_animation_ADDRESS;
			OBJECT(object)->animation_frame = 0u;
			OBJECT(object)->animation_speed = 3u;
			OBJECT(object)->animation_timer = 0;
			OBJECT(object)->projected_x = OBJECT(current_object)->projected_x;
			OBJECT(object)->projected_x += 0x3Au;
			OBJECT(object)->anchor_x = OBJECT(object)->projected_x;
			OBJECT(object)->projected_y = OBJECT(current_object)->projected_y;
			OBJECT(object)->projected_y_int -= 0x31u;
			OBJECT(object)->anchor_y = OBJECT(object)->projected_y_int;
			OBJECT(object)->z = OBJECT(current_object)->z;
			sound_id = 0xAu;
			play_sound(sound_id, &margin, &write_index_address);
		}
	}
	else if (OBJECT(current_object)->screen_y >= 0x18)
	{
		blast_delay = OBJECT(current_object)->fire_vent.blast_delay;
		next_delay = blast_delay - 1u;
		overflow = blast_delay == 0x8000u;
		zero = next_delay == 0u;
		negative = next_delay >> 15;
		OBJECT(current_object)->fire_vent.blast_delay = next_delay;
		if (blast_delay == 1u)
			goto fire_vent_blast;
		if ((negative ^ overflow) | zero)
			OBJECT(current_object)->fire_vent.blast_delay = -1;
	}
	update_screen_position(&ignored, &margin);
	if (OBJECT(current_object)->fire_vent.burst_state == 0u)
	{
		depth = (uint16_t)(OBJECT(current_object)->screen_y - 0x48u);
		queue_sprite_at_depth(&ignored, &depth);
		return;
	}
	if ((int8_t)OBJECT(current_object)->fire_vent.burst_state < 0)
	{
		OBJECT(current_object)->fire_vent.burst_state = 1u;
		OBJECT(current_object)->kind = 0xCu;
	}
	add_to_touchable_list(ignored);
	depth = (uint16_t)(OBJECT(current_object)->screen_y - 0x48u);
	queue_sprite_at_depth(&ignored, &depth);
}

void fire_vent_burst_animate(void)
{
	uint32_t r1 = 0, r7 = 0, at_last_step = 0, depth = 0, ignored = 0;

	animate(&r1, &r7, &at_last_step);
	if ((uint8_t)OBJECT(current_object)->animation_timer == OBJECT(current_object)->animation_speed)
		OBJECT(current_object)->sprite_palette = fire_vent_blast_palettes[OBJECT(current_object)->animation_frame];
	if ((uint8_t)at_last_step == 0u)
	{
		update_screen_position(&ignored, &r1);
		depth = (uint16_t)(OBJECT(current_object)->screen_y + 0x68u);
		queue_sprite_at_depth(&ignored, &depth);
		return;
	}
	AS8(OBJECT(LINKED(current_object, OBJECT(current_object)->parent))->var_88) = 0xFFu;
	delete_current_object();
}
