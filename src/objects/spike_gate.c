/**
 *    Name:        objects/spike_gate.c
 *    Description: object type 42 (stages C and E): the spike gate
 *    Version:     0.1.0
 *    Created:     12-10-2026
 *    Modified:    15-10-2026
 *    Author:      Sebastian Toyos
 *
 *    SegaSonic The Hedgehog (arcade, Sega System 32): the game's V60 program decompiled into C by
 *    tools/decompile.py, with the names of symbols.txt.
 */
#include "game.h"

void spike_gate_init(void)
{
	uint32_t block = 0, free_count = 0, object = 0, piece_count = 0, spawned = 0;

	OBJECT(current_object)->sprite_frame = 0xA200u;
	OBJECT(current_object)->sprite_palette = 0x91F0u;
	OBJECT(current_object)->flags |= 2u;
	OBJECT(current_object)->hitbox_size_x = 0x2Du;
	OBJECT(current_object)->hitbox_size_y = 0x21u;
	OBJECT(current_object)->hitbox_offset_x = -0xE;
	OBJECT(current_object)->hitbox_offset_y = -0x1A;
	OBJECT(current_object)->hitbox_height = 0x100u;
	OBJECT(current_object)->kind = 0xCu;
	OBJECT(current_object)->handler = spike_gate_pillar_update_ADDRESS;
	spawn_count = 1u;
	spawn_object_with_pieces(0x01543Du, &spawned, &piece_count, &free_count, &object, &block);
	copy_position_to(object);
	OBJECT(object)->projected_x += 0x94u;
	OBJECT(object)->projected_y_int += 0x4Au;
	OBJECT(object)->sprite_frame = 0xA208u;
	OBJECT(object)->sprite_palette = 0x91F0u;
	OBJECT(object)->flags |= 2u;
	OBJECT(object)->hitbox_size_x = 0x2Cu;
	OBJECT(object)->hitbox_size_y = 0x29u;
	OBJECT(object)->hitbox_offset_x = -9;
	OBJECT(object)->hitbox_offset_y = -0x20;
	OBJECT(object)->hitbox_height = 0x100u;
	OBJECT(object)->kind = 0xCu;
	OBJECT(object)->spawn_arg_2 = OBJECT(object_cursor)->spawn_arg_2;
	OBJECT(object)->handler = spike_gate_pillar_update_ADDRESS;
	spawn_count = 4u;
	spawn_object_with_pieces(0x0154A4u, &spawned, &piece_count, &free_count, &object, &block);
	copy_position_to(object);
	OBJECT(object)->direction = 0x300u;
	OBJECT(object)->projected_x += 0x1Au;
	OBJECT(object)->projected_y_int += 0x14u;
	OBJECT(object)->sprite_frame = 0xA1E8u;
	OBJECT(object)->sprite_palette = 0x91E0u;
	OBJECT(object)->flags |= 2u;
	OBJECT(object)->hitbox_size_x = 0x2Du;
	OBJECT(object)->hitbox_size_y = 0xDAu;
	OBJECT(object)->hitbox_offset_x = 3;
	OBJECT(object)->hitbox_offset_y = -0x38;
	OBJECT(object)->hitbox_height = 0x28u;
	OBJECT(object)->kind = 0xCu;
	OBJECT(object)->spawn_arg_2 = OBJECT(object_cursor)->spawn_arg_2;
	OBJECT(object)->handler = spike_gate_beam_wait_ADDRESS;
	object = LINKED(object, OBJECT(object)->child);
	OBJECT(object)->sprite_frame = 0xA1F0u;
	OBJECT(object)->sprite_palette = 0x91E0u;
	object = LINKED(object, OBJECT(object)->child);
	OBJECT(object)->sprite_frame = 0xA1F8u;
	OBJECT(object)->sprite_palette = 0x91E0u;
	object = LINKED(object, OBJECT(object)->child);
	OBJECT(object)->sprite_scale = 0x180u;
	OBJECT(object)->sprite_frame = 0xA788u;
	OBJECT(object)->sprite_palette = 0x87A0u;
}

void spike_gate_pillar_update(void)
{
	uint16_t count = 0;
	uint32_t r11 = 0, block_offset = 0, depth = 0, head_distance_x = 0, ignored = 0, slope_height = 0;

	unproject_object(&ignored, &block_offset, &slope_height, &r11);
	r14 = LINKED(WORK_RAM, biting_head);
	distance_xy_to_object(&head_distance_x, &ignored);
	if ((int32_t)head_distance_x <= 0x20 || (int16_t)OBJECT(current_object)->spawn_arg_2 < (int16_t)scroll_position)
	{
		count = 5u;
		OBJECT(current_object)->projected_x -= 0x90u;
		OBJECT(current_object)->projected_y_int -= 0x40u;
		spawn_debris(head_distance_x, count, r11);
		delete_object_and_children(&ignored, &block_offset, &slope_height);
		return;
	}
	depth = (uint16_t)(OBJECT(current_object)->screen_y - 8u);
	queue_sprite_at_depth(&ignored, &depth);
	add_to_touchable_list(ignored);
}

void spike_gate_beam_rise(void)
{
	uint16_t height_above_floor = 0;

	OBJECT(current_object)->z_int -= 3u;
	height_above_floor = (uint16_t)(OBJECT(current_object)->floor_z_int - OBJECT(current_object)->z_int);
	if ((int16_t)height_above_floor > 0x20)
		OBJECT(current_object)->kind = 0x24u;
	if ((int16_t)height_above_floor <= 0x50)
	{
		spike_gate_beam_update();
		return;
	}
	OBJECT(current_object)->spike_gate_beam.pause_timer = 8u;
	OBJECT(current_object)->handler = spike_gate_beam_hold_ADDRESS;
	spike_gate_beam_hold();
}

void spike_gate_beam_hold(void)
{
	uint16_t pause_timer = 0;

	pause_timer = OBJECT(current_object)->spike_gate_beam.pause_timer;
	OBJECT(current_object)->spike_gate_beam.pause_timer = pause_timer - 1u;
	if (pause_timer != 0u)
	{
		spike_gate_beam_update();
		return;
	}
	OBJECT(current_object)->handler = spike_gate_beam_drop_ADDRESS;
	spike_gate_beam_drop();
}

void spike_gate_beam_drop(void)
{
	uint8_t sound_id = 0;
	uint32_t r1 = 0, r2 = 0, height = 0;

	OBJECT(current_object)->speed_z += 0x2000u;
	fall_to_floor(&height, &r1, &r2);
	if ((int32_t)height >= 1)
	{
		spike_gate_beam_update();
		return;
	}
	OBJECT(current_object)->kind = 0xCu;
	OBJECT(current_object)->z_int -= 6u;
	OBJECT(current_object)->spike_gate_beam.bounce_count++;
	if (OBJECT(current_object)->spike_gate_beam.bounce_count <= 2)
	{
		spike_gate_beam_update();
		return;
	}
	OBJECT(current_object)->spike_gate_beam.bounce_count = 0;
	OBJECT(current_object)->z_int += 3u;
	OBJECT(current_object)->spike_gate_beam.pause_timer = 0xAu;
	sound_id = 7u;
	play_sound(sound_id, &r1, &r2);
	OBJECT(current_object)->handler = spike_gate_beam_wait_ADDRESS;
	spike_gate_beam_wait();
}

void spike_gate_beam_wait(void)
{
	uint16_t pause_timer = 0;

	pause_timer = OBJECT(current_object)->spike_gate_beam.pause_timer;
	OBJECT(current_object)->spike_gate_beam.pause_timer = pause_timer - 1u;
	if (pause_timer != 0u)
	{
		spike_gate_beam_update();
		return;
	}
	OBJECT(current_object)->handler = spike_gate_beam_rise_ADDRESS;
	spike_gate_beam_update();
}

void spike_gate_beam_update(void)
{
	uint16_t count = 0, offset_x = 0, offset_y = 0;
	uint32_t r11 = 0, block_offset = 0, bucket = 0, depth = 0, head_distance_x = 0, ignored = 0, slope_height = 0;

	move_object_projected(&ignored, &block_offset, &slope_height, &r11);
	r14 = LINKED(WORK_RAM, biting_head);
	distance_xy_to_object(&head_distance_x, &depth);
	if ((int32_t)head_distance_x <= 0x20 || (int16_t)OBJECT(current_object)->spawn_arg_2 < (int16_t)scroll_position)
	{
		count = 5u;
		OBJECT(current_object)->projected_x -= 0x90u;
		OBJECT(current_object)->projected_y_int -= 0x40u;
		spawn_debris(head_distance_x, count, r11);
		delete_object_and_children(&ignored, &block_offset, &slope_height);
		return;
	}
	depth = cursor_floor_screen_y(depth);
	depth = (uint16_t)(depth - 0x10u);
	queue_sprite_at_depth(&ignored, &depth);
	add_to_touchable_list(ignored);
	current_object = LINKED(current_object, OBJECT(current_object)->child);
	offset_x = 0x30u;
	offset_y = 0x19u;
	place_sprite_near_cursor(offset_x, offset_y);
	depth = cursor_floor_screen_y(depth);
	depth = (uint16_t)(depth - 0x14u);
	queue_sprite_at_depth(&ignored, &depth);
	current_object = LINKED(current_object, OBJECT(current_object)->child);
	offset_x = 0x60u;
	offset_y = 0x2Du;
	place_sprite_near_cursor(offset_x, offset_y);
	depth = cursor_floor_screen_y(depth);
	depth = (uint16_t)(depth - 0x10u);
	queue_sprite_at_depth(&ignored, &depth);
	current_object = LINKED(current_object, OBJECT(current_object)->child);
	bucket = place_sprite_at_cursor_floor();
	OBJECT(current_object)->screen_x += 0x30u;
	OBJECT(current_object)->screen_y += 0x48u;
	queue_sprite_in_bucket(bucket);
}
