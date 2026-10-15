/**
 *    Name:        objects/spike_panels_and_poles.c
 *    Description: object types 5 and 6 (stage 3): the spiked panels that flip up and the electric poles
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
