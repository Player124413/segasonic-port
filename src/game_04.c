/**
 *    Name:        game_04.c
 *    Description: the game's functions, part 04 (in the order of their V60 addresses)
 *    Version:     0.1.0
 *    Created:     12-10-2026
 *    Modified:    14-10-2026
 *    Author:      Sebastian Toyos
 *
 *    SegaSonic The Hedgehog (arcade, Sega System 32): the game's V60 program decompiled into C by
 *    tools/decompile.py, with the names of symbols.txt.
 */
#include "game.h"

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

void white_puff_init(void)
{
	OBJECT(current_object)->sprite_palette = 0x90A0u;
	OBJECT(current_object)->sprite_palette = 0x2000u;
	OBJECT(current_object)->handler = white_puff_animate_ADDRESS;
}

void white_puff_animate(void)
{
	uint32_t block_offset = 0, cursor = 0, deleted = 0, ignored = 0, slope_height = 0;
	uint8_t wrapped_frame_index = 0;
	uint16_t frame_timer = 0;

	unproject_object(&ignored, &block_offset, &slope_height, &cursor);
	delete_if_off_screen(&deleted, &block_offset, &slope_height);
	if ((uint16_t)deleted != 0u)
		return;
	frame_timer = OBJECT(current_object)->white_puff.frame_timer;
	OBJECT(current_object)->white_puff.frame_timer = frame_timer - 1u;
	if (frame_timer == 0u)
	{
		OBJECT(current_object)->sprite_frame = white_puff_frames[OBJECT(current_object)->white_puff.frame_index];
		OBJECT(current_object)->white_puff.frame_timer = 3u;
		OBJECT(current_object)->white_puff.frame_index++;
		wrapped_frame_index = OBJECT(current_object)->white_puff.frame_index % 0xAu;
		OBJECT(current_object)->white_puff.frame_index = wrapped_frame_index;
		if (wrapped_frame_index == 0u)
			OBJECT(current_object)->white_puff.frame_timer = 0x5Au;
	}
	queue_sprite(0u);
}

void checker_pad_init(void)
{
	OBJECT(current_object)->sprite_palette = 0x9040u;
	OBJECT(current_object)->handler = checker_pad_animate_ADDRESS;
}

void checker_pad_animate(void)
{
	uint32_t block_offset = 0, deleted = 0, slope_height = 0;
	uint16_t frame_timer = 0;

	update_screen_position_no_offset();
	delete_if_off_screen(&deleted, &block_offset, &slope_height);
	if ((uint8_t)deleted != 0u)
		return;
	frame_timer = OBJECT(current_object)->checker_pad.frame_timer;
	OBJECT(current_object)->checker_pad.frame_timer = frame_timer - 1u;
	if (frame_timer == 0u)
	{
		OBJECT(current_object)->sprite_frame = checker_pad_frames[AS16(OBJECT(current_object)->checker_pad.frame_index)];
		OBJECT(current_object)->checker_pad.frame_timer = 8u;
		OBJECT(current_object)->checker_pad.frame_index++;
		if (OBJECT(current_object)->checker_pad.frame_index > 4)
			OBJECT(current_object)->checker_pad.frame_index = 0;
	}
	queue_sprite(0u);
}

void static_triangle_init(void)
{
	OBJECT(current_object)->sprite_palette = 0x9050u;
	OBJECT(current_object)->handler = static_triangle_animate_ADDRESS;
	current_object = LINKED(current_object, OBJECT(current_object)->child);
	OBJECT(current_object)->sprite_frame = 0x7828u;
	OBJECT(current_object)->sprite_palette = 0x9060u;
}

void static_triangle_animate(void)
{
	uint16_t offset_x = 0, offset_y = 0;
	uint32_t block_offset = 0, deleted = 0, depth = 0, slope_height = 0;
	uint16_t frame_timer = 0, direction = 0;

	update_screen_position_no_offset();
	delete_if_off_screen(&deleted, &block_offset, &slope_height);
	if ((uint8_t)deleted != 0u)
		return;
	frame_timer = OBJECT(current_object)->static_triangle.frame_timer;
	OBJECT(current_object)->static_triangle.frame_timer = frame_timer - 1u;
	if (frame_timer == 0u)
	{
		OBJECT(current_object)->sprite_frame = static_triangle_frames[(uint8_t)OBJECT(current_object)->static_triangle.frame_index];
		OBJECT(current_object)->static_triangle.frame_timer = 2u;
		OBJECT(current_object)->static_triangle.frame_index++;
		if (OBJECT(current_object)->static_triangle.frame_index > 2)
		{
			OBJECT(current_object)->static_triangle.frame_index = 0;
			OBJECT(current_object)->var_81 = 0u;
		}
	}
	depth = queue_sprite(0u);
	current_object = LINKED(current_object, OBJECT(current_object)->child);
	direction = OBJECT(current_object)->direction;
	OBJECT(current_object)->direction = direction - 1u;
	if (direction == 0u)
	{
		OBJECT(current_object)->direction = 1u;
		offset_x = 0x68u;
		offset_y = 0u;
		place_sprite_near_cursor(offset_x, offset_y);
		queue_sprite(depth);
	}
}

void railing_init(void)
{
	uint32_t block = 0, object = 0, spawned = 0;

	OBJECT(current_object)->sprite_frame = 0x7838u;
	OBJECT(current_object)->sprite_palette = 0x9080u;
	OBJECT(current_object)->flags |= 2u;
	OBJECT(current_object)->hitbox_size_x = 0xFu;
	OBJECT(current_object)->hitbox_size_y = 0x57u;
	OBJECT(current_object)->hitbox_offset_x = 3;
	OBJECT(current_object)->hitbox_offset_y = -0x67;
	OBJECT(current_object)->hitbox_height = 0x100u;
	OBJECT(current_object)->kind = 0xCu;
	OBJECT(current_object)->handler = railing_draw_ADDRESS;
	spawn_count = 1u;
	spawn_objects(0x015389u, &spawned, &object, &block);
	OBJECT(object_cursor)->child = object;
	copy_position_to(object);
	OBJECT(object)->flags |= 2u;
	OBJECT(object)->hitbox_size_x = 0x56u;
	OBJECT(object)->hitbox_size_y = 0x11u;
	OBJECT(object)->hitbox_offset_x = 0x12;
	OBJECT(object)->hitbox_offset_y = -0x11;
	OBJECT(object)->hitbox_height = 0x100u;
	OBJECT(object)->kind = 0xCu;
	OBJECT(object)->handler = railing_side_update_ADDRESS;
}

void railing_draw(void)
{
	uint32_t block_offset = 0, cursor = 0, deleted = 0, ignored = 0, slope_height = 0;

	unproject_object(&ignored, &block_offset, &slope_height, &cursor);
	delete_if_off_screen(&deleted, &block_offset, &slope_height);
	if ((uint8_t)deleted == 0u)
	{
		queue_sprite(0u);
		add_to_touchable_list(deleted);
		return;
	}
}

void railing_side_update(void)
{
	uint32_t cursor = 0, ignored = 0;

	unproject_object(&ignored, &ignored, &ignored, &cursor);
	add_to_touchable_list(ignored);
}

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

void spawn_eel_den(uint32_t spawned, uint32_t free_count, uint32_t den, uint32_t block)
{
	uint32_t piece_count = 0;

	spawn_count = 1u;
	spawn_object_with_pieces(0x0156FFu, &spawned, &piece_count, &free_count, &den, &block);
	if ((uint8_t)spawned == 0u)
	{
		current_object = object_cursor;
		return;
	}
	current_object = den;
	eel_den_init();
}

void eel_den_init(void)
{
	OBJECT(current_object)->flags |= 2u;
	OBJECT(current_object)->hitbox_size_x = 0x47u;
	OBJECT(current_object)->hitbox_size_y = 0x42u;
	OBJECT(current_object)->hitbox_offset_x = -3;
	OBJECT(current_object)->hitbox_offset_y = -0x43;
	OBJECT(current_object)->hitbox_height = 0x28u;
	OBJECT(current_object)->kind = 0xCu;
	OBJECT(current_object)->sprite_frame = 0xAC70u;
	OBJECT(current_object)->sprite_palette = 0x1300u;
	OBJECT(current_object)->sprite_palette |= 0x4000u;
	OBJECT(current_object)->health = 1;
	OBJECT(current_object)->handler = eel_den_wait_ADDRESS;
	current_object = object_cursor;
}

void eel_den_wait(void)
{
	uint32_t allocated = 0, distance_y = 0, ignored = 0, random_value = 0, ring = 0;

	if ((stage_trigger_bits & shl32(1u, (int)(OBJECT(current_object)->spawn_arg_1))) == 0u)
	{
		r14 = LINKED(WORK_RAM, chaser_object);
		distance_xy_to_object(&ignored, &distance_y);
		if ((int16_t)distance_y > 0x35 && ((OBJECT(current_object)->flags & 4u) == 0u || OBJECT(current_object)->health > 0))
		{
			eel_den_draw();
			return;
		}
	}
	stage_trigger_bits |= shl32(1u, (int)(OBJECT(current_object)->spawn_arg_1));
	OBJECT(current_object)->handler = eel_den_collapse_ADDRESS;
	OBJECT(current_object)->projected_y_int -= 0x18u;
	released_ring_angle = 0x144u;
	rings_to_release = 3u;
	released_ring_flip = 2u;
	released_rings_random = 1u;
	drop_rings(0u, &allocated, &random_value, &ring);
	OBJECT(current_object)->projected_y_int += 0x18u;
	eel_den_collapse();
}

void eel_den_collapse(void)
{
	uint16_t frame_timer = 0;

	frame_timer = OBJECT(current_object)->eel_den.frame_timer;
	OBJECT(current_object)->eel_den.frame_timer = frame_timer - 1u;
	if (frame_timer != 0u)
	{
		eel_den_draw();
		return;
	}
	OBJECT(current_object)->eel_den.frame_timer = 2u;
	OBJECT(current_object)->sprite_frame = eel_den_collapse_frames[(uint8_t)OBJECT(current_object)->eel_den.frame_index];
	OBJECT(current_object)->eel_den.frame_index++;
	if (OBJECT(current_object)->eel_den.frame_index <= 0x2C)
	{
		eel_den_draw();
		return;
	}
	OBJECT(current_object)->handler = eel_den_draw_ADDRESS;
	eel_den_draw();
}

void eel_den_draw(void)
{
	uint32_t block_offset = 0, cursor = 0, deleted = 0, depth = 0, ignored = 0, slope_height = 0;

	unproject_object(&ignored, &block_offset, &slope_height, &cursor);
	delete_if_off_screen(&deleted, &block_offset, &slope_height);
	if ((uint8_t)deleted != 1u)
	{
		depth = (uint16_t)(OBJECT(current_object)->screen_y - 0x18u);
		queue_sprite_at_depth(&ignored, &depth);
		add_to_touchable_list(ignored);
	}
}

void stepped_wall_init(void)
{
	OBJECT(current_object)->sprite_flags = 6u;
	OBJECT(current_object)->sprite_frame = 0xB670u;
	OBJECT(current_object)->sprite_palette = 0x8D60u;
	OBJECT(current_object)->sprite_palette |= 0x2000u;
	OBJECT(current_object)->flags |= 2u;
	OBJECT(current_object)->hitbox_size_x = 0x1Du;
	OBJECT(current_object)->hitbox_size_y = 0xDCu;
	OBJECT(current_object)->hitbox_offset_x = 0x73;
	OBJECT(current_object)->hitbox_offset_y = -0x7E;
	OBJECT(current_object)->hitbox_height = 0x80u;
	OBJECT(current_object)->kind = 0xCu;
	OBJECT(current_object)->handler = stepped_wall_draw_ADDRESS;
}

void stepped_wall_draw(void)
{
	uint32_t block_offset = 0, bucket = 0, cursor = 0, deleted = 0, ignored = 0, slope_height = 0;

	unproject_object(&ignored, &block_offset, &slope_height, &cursor);
	delete_if_off_screen(&deleted, &block_offset, &slope_height);
	if ((uint8_t)deleted == 0u)
	{
		bucket = 0xFFu;
		queue_sprite_in_bucket(bucket);
		add_to_touchable_list(bucket);
		return;
	}
}

void init_stage_view(uint32_t *p_direction_y, uint32_t *p_direction_x, uint32_t *p_count, uint32_t *p_stage_offset)
{
	uint16_t camera_origin = 0;
	uint32_t count = 0, dest = 0, direction_x = 0, direction_y = 0, direction_y_list = 0, ignored = 0, list = 0, sets_left = 0, stage_number = 0, table_offset = 0, tile_set = 0, tile_set_cursor = 0;
	uint8_t io_cnt_bits = 0;
	uint32_t unrotated_sets = 0;

	io_cnt_bits = mem_read8(io_cnt);
	mem_write8(io_cnt, io_cnt_bits & ~2u);
	display_off_frames = 8u;
	dest = load_default_palettes();
	debug_sprite_x = 0x30u;
	debug_sprite_y = 0x30u;
	debug_sprite_spawned = 0u;
	circle_wipe_left = 0x82u;
	circle_wipe_right = 0xBEu;
	circle_wipe_line = 0x32u;
	unused_circle_wipe_byte = 0u;
	AS8(circle_wipe_radius) = 0xAu;
	loaded_tile_sets = 0u;
	nbg0_pages = 0x3020100u;
	nbg2_pages = 0xD0C0B0Au;
	mem_write32(nbg0_page_select, 0x3020100u);
	tile_set_cursor = stage_initial_tile_sets[stage];
	sets_left = 4u;
	mem_write8(video_layer_disable, mem_read8(video_layer_disable) | 0xFu);
	fade_speed = 3u;
	mixer_text_blend &= 0xBFFFu;
	mixer_nbg0_blend &= 0xBFFFu;
	mixer_nbg1_blend &= 0xBFFFu;
	mixer_nbg2_blend &= 0xBFFFu;
	mixer_nbg3_blend &= 0xBFFFu;
	mixer_bitmap_blend &= 0xBFFFu;
	mixer_sprite_control &= ~0x8000u;
	mem_write32(video_control, mem_read32(video_control) & ~0x1000u);
	mem_write32(video_control, mem_read32(video_control) & ~0x2000u);
	mem_write32(video_control, mem_read32(video_control) & ~0x4000u);
	mem_write16(nbg0_zoom_x, 0x200u);
	mem_write16(nbg0_zoom_y, 0x200u);
	mem_write16(nbg1_zoom_x, 0x200u);
	mem_write16(nbg1_zoom_y, 0x200u);
	nbg0_center_x = 0u;
	nbg0_center_y = 0u;
	nbg1_center_x = 0u;
	nbg1_center_y = 0u;
	tile_set_cursor += 0x19712u;
	do
	{
		tile_set = ROM32(tile_set_cursor);
		AS8(loaded_tile_sets) += tile_set;
		unrotated_sets = loaded_tile_sets;
		loaded_tile_sets = (unrotated_sets << 8) | (unrotated_sets >> 24);
		tile_set_cursor += 4u;
		load_tile_set(tile_set, &list, &ignored, &ignored, &dest);
		sets_left--;
	} while (sets_left != 0u);
	wanted_tile_sets = loaded_tile_sets;
	mixer_blend &= 0x1F00u;
	switch (stage)
	{
	case 0:
	case 9:
	case 16:
		break;
	case 1:
	case 2:
	case 3:
	case 4:
	case 5:
	case 6:
	case 7:
	case 8:
	case 10:
	case 11:
	case 12:
	case 13:
	case 14:
	case 15:
	case 17:
	case 18:
	case 19:
		mixer_blend = 0u;
		break;
	default:
		unknown_code(0x15A33u + stage * 3u);
	}
	stage_06_background_step = 0u;
	scroll_ended = 0;
	fade_step = 0u;
	stage_03_end_started = 0u;
	stage_03_end_step = 0u;
	stage_03_end_frames = 0u;
	unused_view_word = 0u;
	stage_00_exit_step = 0u;
	stage_00_exit_delay = 0u;
	stage_00_exit_timer = 0;
	stage_00_exit_zoom = 0u;
	color_offset_red = 0u;
	color_offset_blue = 0u;
	color_offset_green = 0u;
	AS16(stage_05_switch_step) = 0u;
	map_screen_step = 0u;
	dest = init_nbg2_layer(dest);
	dest = init_nbg1_layer(dest);
	init_nbg3_layer(dest);
	count = set_stage_layer_mixing();
	load_stage_palette_set(&ignored, &ignored);
	view_y = 0;
	view_x = 0;
	camera_x_pixels = 0u;
	camera_y_pixels = 0u;
	tile_set_slot = 0u;
	camera_x = 0;
	camera_y = 0;
	view_x = 0;
	view_y = 0;
	scroll_position = 0u;
	table_offset = stage;
	stage_block_map_width = stage_block_map_widths[table_offset];
	table_offset = (uint16_t)(table_offset * 2u);
	camera_origin = ROM16(stage_camera_origins_x_ADDRESS + table_offset);
	camera_origin_x = camera_origin;
	camera_x += (uint32_t)camera_origin << 16;
	camera_origin = ROM16(stage_camera_origins_y_ADDRESS + table_offset);
	camera_origin_y = camera_origin;
	camera_y += (uint32_t)camera_origin << 16;
	view_offset_y = ROM16(stage_view_offsets_y_ADDRESS + table_offset);
	view_offset_x = ROM16(stage_view_offsets_x_ADDRESS + table_offset);
	scroll_end = ROM16(stage_scroll_ends_ADDRESS + table_offset);
	stage_number = stage;
	list = stage_section_speeds[stage_number] + stage_section_speeds_ADDRESS;
	scroll_speed = ROM8(list);
	section_speed_list = list + 1u;
	list = stage_section_follow_speeds[stage_number] + stage_section_follow_speeds_ADDRESS;
	follow_base_speed = ROM8(list);
	follow_base_speed_list = list + 1u;
	section_end_list = stage_section_ends[stage_number] + stage_section_ends_ADDRESS;
	section_end_list += 2u;
	section_unused_cursor = stage_section_unused[stage_number];
	list = stage_section_limits_x[stage_number] + stage_section_limits_x_ADDRESS;
	list = (list & ~0xFFFFu) | (uint16_t)(list + 2u);
	section_limit_x = mem_read16(list);
	section_limit_x_list = list + 2u;
	list = stage_section_limits_y[stage_number] + stage_section_limits_y_ADDRESS;
	list = (list & ~0xFFFFu) | (uint16_t)(list + 2u);
	section_limit_y = mem_read16(list);
	section_limit_y_list = list + 2u;
	list = stage_scroll_modes[stage_number] + stage_scroll_modes_ADDRESS;
	scroll_mode = ROM16(list);
	list += 2u;
	scroll_mode_timer = ROM16(list);
	scroll_mode_duration = scroll_mode_timer;
	scroll_ramp_speed = 1u;
	scroll_mode_script = list + 2u;
	direction_y_list = stage_section_directions_y[stage_number] + stage_section_directions_y_ADDRESS;
	list = stage_section_directions_x[stage_number] + stage_section_directions_x_ADDRESS;
	direction_x = (int16_t)ROM16(list);
	direction_y = (int16_t)ROM16(direction_y_list);
	section_direction_x = direction_x;
	section_direction_y = direction_y;
	section_direction_x_list = list + 2u;
	section_direction_y_list = direction_y_list + 2u;
	unused_scroll_flag = 0u;
	unused_scroll_offset = -0xA;
	nbg0_scroll_x_int = (int32_t)camera_x >> 16;
	nbg0_scroll_y_int = (int32_t)camera_y >> 16;
	nbg2_scroll_x = nbg0_scroll_x_int;
	nbg1_scroll_x_int = nbg0_scroll_x_int;
	nbg2_scroll_y = nbg0_scroll_y_int;
	nbg1_scroll_y_int = nbg0_scroll_y_int;
	camera_shake_phase = 0u;
	camera_shake_step = 0u;
	camera_shake_length = 0u;
	camera_shake_active = 0u;
	AS16(camera_shake_frame) = 0u;
	back_edge_frames = 0u;
	nearby_floor_blocks_locked = 0u;
	scroll_skip_open = 0u;
	*p_direction_y = direction_y;
	*p_direction_x = direction_x;
	*p_count = count;
	*p_stage_offset = table_offset;
}

void update_stage_tile_layers(uint32_t *p_shifted_tile_set, uint32_t *p_copy_entry, uint32_t *p_columns_left, uint32_t *p_palette_count, uint32_t *p_stage_number, uint32_t *p_row_skip, uint32_t *p_cycle, uint32_t *p_block_cursor, uint32_t *p_dest)
{
	uint32_t copy_entry = *p_copy_entry;
	uint32_t columns_left = *p_columns_left;
	uint32_t palette_count = *p_palette_count;
	uint32_t stage_number = *p_stage_number;
	uint32_t row_skip = *p_row_skip;
	uint32_t cycle = *p_cycle;
	uint32_t block_cursor = *p_block_cursor;
	uint32_t dest = *p_dest;
	uint32_t r1 = 0, r2 = 0, r4 = 0, r5 = 0, r8 = 0, rows_left = 0, set_shift = 0, shifted_tile_set = 0;

	animate_stage_palette(&r1, &copy_entry, &palette_count, &cycle, &block_cursor, &dest);
	if (scroll_ended < 0)
	{
		*p_shifted_tile_set = r1;
		*p_copy_entry = copy_entry;
		*p_palette_count = palette_count;
		*p_cycle = cycle;
		*p_block_cursor = block_cursor;
		*p_dest = dest;
		return;
	}
	clamp_camera_pixels();
	set_nbg0_scroll_from_camera(copy_entry);
	stage_number = stage;
	block_cursor = stage_block_maps[stage_number] + stage_block_maps_ADDRESS + tile_block_row + tile_block_column;
	columns_left = 2u;
	rows_left = 2u;
	set_shift = 0u;
	wanted_tile_sets = 0u;
	for (;;)
	{
		shifted_tile_set = rotate_by(ROM8(block_cursor), (uint8_t)set_shift, 32);
		wanted_tile_sets += shifted_tile_set;
		set_shift = (uint16_t)(set_shift + 8u);
		columns_left--;
		block_cursor++;
		if (columns_left != 0u)
			continue;
		row_skip = stage_block_map_width;
		row_skip = (uint16_t)(row_skip - 2u);
		block_cursor += row_skip;
		columns_left = 2u;
		rows_left--;
		if (rows_left != 0u)
			continue;
		break;
	}
	if (wanted_tile_sets != loaded_tile_sets)
	{
		if (tile_sets_in_progress != wanted_tile_sets && (int8_t)AS8(tile_set_slot) > 1)
			wanted_tile_sets = tile_sets_in_progress;
		reload_changed_tile_set(&shifted_tile_set, &r2, &r4, &r5, &r8, &block_cursor, &dest);
		clamp_camera_pixels();
		*p_shifted_tile_set = shifted_tile_set;
		*p_copy_entry = r2;
		*p_columns_left = columns_left;
		*p_palette_count = r4;
		*p_stage_number = r5;
		*p_row_skip = r8;
		*p_cycle = cycle;
		*p_block_cursor = block_cursor;
		*p_dest = dest;
		return;
	}
	tile_set_slot = 0u;
	clamp_camera_pixels();
	*p_shifted_tile_set = shifted_tile_set;
	*p_copy_entry = rows_left;
	*p_columns_left = columns_left;
	*p_palette_count = set_shift;
	*p_stage_number = stage_number;
	*p_row_skip = row_skip;
	*p_cycle = cycle;
	*p_block_cursor = block_cursor;
	*p_dest = dest;
}

void reload_changed_tile_set(uint32_t *p_shifted_tile_set, uint32_t *p_slots_left, uint32_t *p_r4, uint32_t *p_slot_tile_set, uint32_t *p_loaded_shift, uint32_t *p_script, uint32_t *p_dest)
{
	uint32_t shifted_tile_set = *p_shifted_tile_set;
	uint32_t script = *p_script;
	uint32_t dest = *p_dest;
	uint8_t shift_per_slot = 0;
	uint32_t r4 = 0, r5 = 0, r10 = 0, loaded_shift = 0, slot_tile_set = 0, slots_left = 0, wanted_tile_set = 0;

	slots_left = 4u;
	r4 = 0u;
	shift_per_slot = 0xF8u;
	loaded_shift = 0u;
	if (tile_sets_in_progress != wanted_tile_sets)
		tile_set_slot = 0u;
	wanted_tile_set = shift_by(wanted_tile_sets, (uint8_t)((uint32_t)shift_per_slot * tile_set_slot), 32) & 0xFFu;
	tile_set_slot++;
	for (;;)
	{
		slot_tile_set = shift_by(loaded_tile_sets, (uint8_t)loaded_shift, 32) & 0xFFu;
		if (wanted_tile_set == slot_tile_set)
			break;
		loaded_shift = (uint16_t)(loaded_shift - 8u);
		slots_left--;
		if (slots_left == 0u)
		{
			load_tile_set(wanted_tile_set, &shifted_tile_set, &r5, &r10, &dest);
			*p_shifted_tile_set = shifted_tile_set;
			*p_slots_left = slots_left;
			*p_r4 = r4;
			*p_slot_tile_set = r5;
			*p_loaded_shift = loaded_shift;
			*p_script = r10;
			*p_dest = dest;
			return;
		}
	}
	if ((int8_t)AS8(tile_set_slot) < 4)
	{
		tile_sets_in_progress = wanted_tile_sets;
		*p_slots_left = slots_left;
		*p_r4 = r4;
		*p_slot_tile_set = slot_tile_set;
		*p_loaded_shift = loaded_shift;
		return;
	}
	loaded_tile_sets = wanted_tile_sets;
	tile_set_slot = 0u;
	tile_sets_in_progress = wanted_tile_sets;
	*p_slots_left = slots_left;
	*p_r4 = r4;
	*p_slot_tile_set = slot_tile_set;
	*p_loaded_shift = loaded_shift;
}

void load_tile_set(uint32_t tile_set, uint32_t *p_count, uint32_t *p_stage_offset, uint32_t *p_script, uint32_t *p_dest)
{
	uint32_t count = *p_count;
	uint32_t dest = *p_dest;
	uint32_t script = 0, script_list = 0, stage_offset = 0;

	script_list = stage_tile_sets[stage];
	get_tile_set_list(tile_set, &stage_offset, &script_list);
	script_list = get_tile_set_list_bank(script_list);
	for (;;)
	{
		script = mem_read32(script_list);
		script_list += 4u;
		if ((int32_t)script < 0)
			break;
		script = get_tile_set_script_bank(script);
		run_vram_script(&count, &script, &dest);
	}
	if ((int8_t)AS8(tile_set_slot) < 4)
	{
		tile_sets_in_progress = wanted_tile_sets;
		*p_count = count;
		*p_stage_offset = stage_offset;
		*p_script = script;
		*p_dest = dest;
		return;
	}
	loaded_tile_sets = wanted_tile_sets;
	tile_set_slot = 0u;
	tile_sets_in_progress = wanted_tile_sets;
	*p_count = count;
	*p_stage_offset = stage_offset;
	*p_script = script;
	*p_dest = dest;
}

uint32_t get_tile_set_script_bank(uint32_t script)
{
	uint32_t stage_number = 0;

	stage_number = AS8(stage);
	switch ((uint16_t)(stage_number << 1))
	{
	case 0x0u:
	case 0x2u:
	case 0x4u:
	case 0x22u:
	case 0x24u:
		return script;
	case 0x6u:
	case 0x8u:
	case 0xAu:
	case 0xCu:
	case 0xEu:
	case 0x10u:
	case 0x12u:
	case 0x14u:
	case 0x16u:
	case 0x18u:
	case 0x1Au:
	case 0x1Cu:
	case 0x1Eu:
	case 0x20u:
	case 0x26u:
		script += 0x100000u;
		return script;
	default:
		unknown_code(0x15E85u + (uint16_t)(stage_number << 1));
	}
}

uint32_t get_tile_set_list_bank(uint32_t script_list)
{
	uint32_t stage_number = 0;

	stage_number = AS8(stage);
	switch ((uint16_t)(stage_number << 1))
	{
	case 0x0u:
	case 0x2u:
	case 0x4u:
	case 0x22u:
	case 0x24u:
		return script_list;
	case 0x6u:
	case 0x8u:
	case 0xAu:
	case 0xCu:
	case 0xEu:
	case 0x10u:
	case 0x12u:
	case 0x14u:
	case 0x16u:
	case 0x18u:
	case 0x1Au:
	case 0x1Cu:
	case 0x1Eu:
	case 0x20u:
	case 0x26u:
		script_list += 0x100000u;
		return script_list;
	default:
		unknown_code(0x15EC0u + (uint16_t)(stage_number << 1));
	}
}

void get_tile_set_list(uint32_t tile_set, uint32_t *p_stage_offset, uint32_t *p_script_list)
{
	uint32_t script_list = *p_script_list;
	uint32_t stage_offset = 0;

	stage_offset = AS8(stage);
	stage_offset = (uint16_t)(stage_offset << 1);
	switch (stage_offset)
	{
	case 0x0u:
	case 0x2u:
	case 0x4u:
	case 0x22u:
	case 0x24u:
	case 0x26u:
		script_list = mem_read16((script_list & ~0xFFFFu) | (uint16_t)(script_list + (tile_set << 1))) + blank_tile_set_lists_ADDRESS;
		*p_stage_offset = stage_offset;
		*p_script_list = script_list;
		return;
	case 0x6u:
	case 0x8u:
	case 0xAu:
	case 0xCu:
	case 0xEu:
	case 0x10u:
	case 0x12u:
	case 0x14u:
	case 0x16u:
	case 0x18u:
	case 0x1Au:
	case 0x1Cu:
	case 0x1Eu:
	case 0x20u:
		script_list = mem_read32(script_list + (tile_set << 2));
		*p_stage_offset = stage_offset;
		*p_script_list = script_list;
		return;
	default:
		unknown_code(0x15EFAu + stage_offset);
	}
}

void clamp_camera_pixels(void)
{
	if ((int16_t)camera_x_pixels <= 0)
		camera_x_pixels = 0u;
	if ((int16_t)camera_x_pixels >= 0x2000)
		camera_x_pixels = 0x1FFFu;
	if ((int16_t)camera_y_pixels <= 0)
		camera_y_pixels = 0u;
	if ((int16_t)camera_y_pixels >= 0x1000)
		camera_y_pixels = 0xFFFu;
}

void update_scroll(uint32_t *p_position, uint32_t *p_speed, uint32_t *p_speed_value, uint32_t *p_next_scroll_position, uint32_t *p_second_player_object, uint32_t *p_third_player_object, uint32_t *p_end_script, uint32_t *p_dest)
{
	uint32_t position = *p_position;
	uint32_t speed = *p_speed;
	uint32_t speed_value = *p_speed_value;
	uint32_t next_scroll_position = *p_next_scroll_position;
	uint32_t second_player_object = *p_second_player_object;
	uint32_t third_player_object = *p_third_player_object;
	uint32_t end_script = *p_end_script;
	uint32_t dest = *p_dest;
	uint16_t layer_scroll_x = 0, target_heading = 0;
	uint32_t r2 = 0, r3 = 0, r4 = 0, r5 = 0, check_result = 0, count = 0, count_2 = 0, mode_script_cursor = 0, object = 0, object_2 = 0, object_3 = 0, player = 0, player_2 = 0, player_speed = 0, push_speed = 0, script = 0, sine_entry = 0, velocity_y = 0;

	if (scroll_ended < 0)
	{
	update_scroll_at_end:
		scroll_ended = -1;
		switch (stage)
		{
		case 0:
		case 1:
		case 2:
		case 4:
		case 5:
		case 6:
		case 7:
		case 8:
		case 9:
		case 10:
		case 11:
		case 12:
		case 13:
		case 14:
		case 15:
		case 16:
		case 17:
		case 18:
			*p_position = position;
			return;
		case 3:
			break;
		case 19:
			*p_position = position;
			return;
		default:
			unknown_code(0x16BE2u + stage * 3u);
		}
		if ((int8_t)stage_03_end_started <= 0)
		{
			stage_03_end_started++;
			script = stage_03_end_script_1_ADDRESS;
			run_vram_script(&count, &script, &dest);
			end_script = stage_03_end_script_2_ADDRESS;
			run_vram_script(&count, &end_script, &dest);
			nbg0_pages = 0x2020202u;
			scroll_mode_timer = 0x3Cu;
			scroll_mode_duration = scroll_mode_timer;
			scroll_mode = 4u;
			scroll_speed = 0x19u;
			stage_03_end_step = 0u;
			stage_03_end_timer = 0x78;
			stage_03_end_frames = 0u;
		}
		switch (stage_03_end_step)
		{
		case 0:
		case 4:
			stage_03_end_frames++;
			scroll_speeding_up(&position, &speed, &speed_value, &next_scroll_position, &end_script, &dest);
			if (AS8(scroll_mode) == 0u)
			{
				speed = (speed & ~0xFFFFu) | 0x25u;
				scroll_speed = 0x25u;
				stage_03_end_step++;
			}
			break;
		case 1:
			stage_03_end_frames++;
			speed = (speed & ~0xFFFFu) | scroll_speed;
			stage_03_end_timer--;
			if (stage_03_end_timer <= 0)
			{
				camera_y = 0x10000;
				end_script = stage_03_end_script_3_ADDRESS;
				run_vram_script(&count, &end_script, &dest);
				nbg0_pages = 0x2020200u;
				stage_03_end_step++;
				scroll_mode_timer = 5u;
				scroll_mode_duration = scroll_mode_timer;
				scroll_mode = 4u;
				scroll_speed = 0x19u;
			}
			break;
		case 2:
			stage_03_end_frames++;
			scroll_slowing_down(&position, &speed, &speed_value, &next_scroll_position, &end_script, &dest);
			if ((int16_t)scroll_speed <= 1)
			{
				scroll_speed = 0u;
				stage_03_end_step++;
				all_players_path_request = 0x23u;
				dest = spawn_stage_03_end_pillar(dest);
				*p_position = position;
				*p_speed = speed;
				*p_speed_value = speed_value;
				*p_next_scroll_position = next_scroll_position;
				*p_end_script = end_script;
				*p_dest = dest;
				return;
			}
			break;
		case 3:
			camera_velocity_x = 0;
			camera_velocity_y = 0;
			*p_position = position;
			*p_end_script = end_script;
			*p_dest = dest;
			return;
		default:
			unknown_code(0x174D2u + stage_03_end_step * 3u);
		}
		section_limit_y = -1;
		section_direction_y = -0x4000;
		section_direction_x = 0;
		advance_camera_y(speed);
		update_camera_view();
		layer_scroll_x = camera_x_pixels;
		nbg0_scroll_y_int = camera_y_pixels;
		nbg0_scroll_x_int = layer_scroll_x;
		nbg0_scroll_x_int &= 0x3FFu;
		nbg0_scroll_y_int &= 0x1FFu;
		speed_value = (uint16_t)speed;
		velocity_y = section_direction_y * speed_value;
		camera_velocity_x = section_direction_x * speed_value;
		camera_velocity_y = velocity_y;
		*p_position = velocity_y;
		*p_speed = speed;
		*p_speed_value = speed_value;
		*p_next_scroll_position = next_scroll_position;
		*p_end_script = end_script;
		*p_dest = dest;
		return;
	}
	position = scroll_position;
	if ((int32_t)scroll_end <= (int32_t)position)
		goto update_scroll_at_end;
	switch (scroll_mode)
	{
	case 0:
		scroll_at_scroll_speed(&position, &speed, &speed_value, &next_scroll_position, &end_script, &dest);
		*p_position = position;
		*p_speed = speed;
		*p_speed_value = speed_value;
		*p_next_scroll_position = next_scroll_position;
		*p_second_player_object = second_player_object;
		*p_third_player_object = third_player_object;
		*p_end_script = end_script;
		*p_dest = dest;
		return;
	case 1:
		scroll_speed = 0u;
		speed &= ~0xFFFFu;
		set_camera_velocity(&r2, &speed, &r4, &next_scroll_position, &end_script, &dest);
		*p_position = r2;
		*p_speed = speed;
		*p_speed_value = r4;
		*p_next_scroll_position = next_scroll_position;
		*p_second_player_object = second_player_object;
		*p_third_player_object = third_player_object;
		*p_end_script = end_script;
		*p_dest = dest;
		return;
	case 2:
		scroll_mode_timer--;
		if (scroll_mode_timer != 0u)
		{
			update_camera_view();
			speed = 0u;
			update_camera_view();
			set_camera_velocity(&r2, &speed, &r4, &next_scroll_position, &end_script, &dest);
			*p_position = r2;
			*p_speed = speed;
			*p_speed_value = r4;
			*p_next_scroll_position = next_scroll_position;
			*p_second_player_object = second_player_object;
			*p_third_player_object = third_player_object;
			*p_end_script = end_script;
			*p_dest = dest;
			return;
		}
		if (scroll_mode != 2u)
		{
			scroll_mode = 0u;
			scroll_ramp_speed = 1u;
			update_camera_view();
			set_camera_velocity(&r2, &speed, &r4, &next_scroll_position, &end_script, &dest);
			*p_position = r2;
			*p_speed = speed;
			*p_speed_value = r4;
			*p_next_scroll_position = next_scroll_position;
			*p_second_player_object = second_player_object;
			*p_third_player_object = third_player_object;
			*p_end_script = end_script;
			*p_dest = dest;
			return;
		}
		mode_script_cursor = scroll_mode_script;
		scroll_mode = ROM16(mode_script_cursor);
		mode_script_cursor += 2u;
		scroll_mode_timer = ROM16(mode_script_cursor);
		scroll_mode_duration = scroll_mode_timer;
		scroll_mode_script = mode_script_cursor + 2u;
		scroll_ramp_speed = 1u;
		set_camera_velocity(&r2, &speed, &r4, &next_scroll_position, &end_script, &dest);
		*p_position = r2;
		*p_speed = speed;
		*p_speed_value = r4;
		*p_next_scroll_position = next_scroll_position;
		*p_second_player_object = second_player_object;
		*p_third_player_object = third_player_object;
		*p_end_script = end_script;
		*p_dest = dest;
		return;
	case 3:
		scroll_slowing_down(&position, &r3, &speed_value, &next_scroll_position, &end_script, &dest);
		*p_position = position;
		*p_speed = r3;
		*p_speed_value = speed_value;
		*p_next_scroll_position = next_scroll_position;
		*p_second_player_object = second_player_object;
		*p_third_player_object = third_player_object;
		*p_end_script = end_script;
		*p_dest = dest;
		return;
	case 4:
		scroll_speeding_up(&position, &speed, &speed_value, &next_scroll_position, &end_script, &dest);
		*p_position = position;
		*p_speed = speed;
		*p_speed_value = speed_value;
		*p_next_scroll_position = next_scroll_position;
		*p_second_player_object = second_player_object;
		*p_third_player_object = third_player_object;
		*p_end_script = end_script;
		*p_dest = dest;
		return;
	case 5:
		nearby_floor_blocks_locked = 1u;
		mode_script_cursor = scroll_mode_script;
		scroll_mode = ROM16(mode_script_cursor);
		mode_script_cursor += 2u;
		scroll_mode_timer = ROM16(mode_script_cursor);
		scroll_mode_duration = scroll_mode_timer;
		scroll_mode_script = mode_script_cursor + 2u;
		scroll_ramp_speed = 1u;
		set_camera_velocity(&r2, &speed, &r4, &next_scroll_position, &end_script, &dest);
		*p_position = r2;
		*p_speed = speed;
		*p_speed_value = r4;
		*p_next_scroll_position = next_scroll_position;
		*p_second_player_object = second_player_object;
		*p_third_player_object = third_player_object;
		*p_end_script = end_script;
		*p_dest = dest;
		return;
	case 6:
		nearby_floor_blocks_locked = 0u;
		mode_script_cursor = scroll_mode_script;
		scroll_mode = ROM16(mode_script_cursor);
		mode_script_cursor += 2u;
		scroll_mode_timer = ROM16(mode_script_cursor);
		scroll_mode_duration = scroll_mode_timer;
		scroll_mode_script = mode_script_cursor + 2u;
		scroll_ramp_speed = 1u;
		set_camera_velocity(&r2, &speed, &r4, &next_scroll_position, &end_script, &dest);
		*p_position = r2;
		*p_speed = speed;
		*p_speed_value = r4;
		*p_next_scroll_position = next_scroll_position;
		*p_second_player_object = second_player_object;
		*p_third_player_object = third_player_object;
		*p_end_script = end_script;
		*p_dest = dest;
		return;
	case 7:
	case 8:
		player = PLAYER_BLOCK(1);
		player_2 = PLAYER_BLOCK(2);
		r14 = PLAYER_BLOCK(3);
		object = LINKED(WORK_RAM, player_1_object);
		object_2 = LINKED(WORK_RAM, player_2_object);
		object_3 = LINKED(WORK_RAM, player_3_object);
		players_pushing_edge = 0u;
		players_facing_scroll = 0u;
		players_in_play_bits = 0u;
		player_past_leading_edge = 0u;
		if ((OBJECT(object)->flags & 0x10000400u) == 0u)
			players_in_play_bits |= 1u;
		if ((OBJECT(object_2)->flags & 0x10000400u) == 0u)
			players_in_play_bits |= 2u;
		if ((OBJECT(object_3)->flags & 0x10000400u) == 0u)
			players_in_play_bits |= 4u;
		check_result = 0u;
		set_screen_push_edges(check_result, &count_2, &sine_entry);
		is_direction_in_quadrant(count_2, &check_result, &sine_entry);
		if ((uint8_t)check_result == 0u)
		{
			if (player_at_trailing_edge != 0u && (int16_t)((uint32_t)ROM16(section_end_list) - scroll_position) <= 0x180)
			{
				if ((int8_t)back_edge_frames >= 0x40)
					follow_base_speed = 6u;
				else
					back_edge_frames++;
			}
			push_speed = 0u;
			push_speed &= 0xFFu;
			push_speed = (uint16_t)(push_speed + follow_base_speed);
			advance_scroll(&r2, &push_speed, &r4, &r5, &player, &sine_entry);
			*p_position = r2;
			*p_speed = push_speed;
			*p_speed_value = r4;
			*p_next_scroll_position = r5;
			*p_second_player_object = object_2;
			*p_third_player_object = object_3;
			*p_end_script = player;
			*p_dest = sine_entry;
			return;
		}
		if ((players_in_play_bits & 1u) != 0u)
		{
			target_heading = PLAYER(player)->target_heading;
			check_result = is_player_facing_scroll(check_result, target_heading, (uint16_t)sine_entry);
			if ((uint8_t)check_result != 0u)
				players_facing_scroll |= 1u;
		}
		if ((players_in_play_bits & 2u) != 0u)
		{
			target_heading = PLAYER(player_2)->target_heading;
			check_result = is_player_facing_scroll(check_result, target_heading, (uint16_t)sine_entry);
			if ((uint8_t)check_result != 0u)
				players_facing_scroll |= 2u;
		}
		if ((players_in_play_bits & 4u) != 0u)
		{
			target_heading = PLAYER(r14)->target_heading;
			check_result = is_player_facing_scroll(check_result, target_heading, (uint16_t)sine_entry);
			if ((uint8_t)check_result != 0u)
				players_facing_scroll |= 4u;
		}
		player_at_trailing_edge = 0u;
		if ((OBJECT(object)->flags & 0x400u) == 0u)
		{
			check_result = is_player_pushing_edge(check_result, object);
			if ((uint8_t)check_result != 0u)
				players_pushing_edge |= 1u;
		}
		if ((OBJECT(object_2)->flags & 0x400u) == 0u)
		{
			object = object_2;
			check_result = is_player_pushing_edge(check_result, object);
			if ((uint8_t)check_result != 0u)
				players_pushing_edge |= 2u;
		}
		if ((OBJECT(object_3)->flags & 0x400u) == 0u)
		{
			object = object_3;
			check_result = is_player_pushing_edge(check_result, object);
			if ((uint8_t)check_result != 0u)
				players_pushing_edge |= 4u;
		}
		if (players_pushing_edge == 0u || (int8_t)players_pushing_edge > 7 || player_at_trailing_edge != 0u)
		{
			if (player_at_trailing_edge != 0u && (int16_t)((uint32_t)ROM16(section_end_list) - scroll_position) <= 0x180)
			{
				if ((int8_t)back_edge_frames >= 0x40)
					follow_base_speed = 6u;
				else
					back_edge_frames++;
			}
			push_speed = 0u;
			push_speed &= 0xFFu;
			push_speed = (uint16_t)(push_speed + follow_base_speed);
			advance_scroll(&r2, &push_speed, &r4, &r5, &player, &sine_entry);
			*p_position = r2;
			*p_speed = push_speed;
			*p_speed_value = r4;
			*p_next_scroll_position = r5;
			*p_second_player_object = object_2;
			*p_third_player_object = object_3;
			*p_end_script = player;
			*p_dest = sine_entry;
			return;
		}
		players_pushing_edge &= players_facing_scroll;
		players_pushing_edge &= players_in_play_bits;
		object = LINKED(object, PLAYER(player)->object);
		switch (players_pushing_edge)
		{
		case 0:
		case 8:
			if (players_facing_scroll != 0u || player_past_leading_edge == 0u)
			{
				speed = 5u;
				speed = (uint16_t)(speed + follow_base_speed);
				advance_scroll(&r2, &speed, &r4, &r5, &player, &sine_entry);
				*p_position = r2;
				*p_speed = speed;
				*p_speed_value = r4;
				*p_next_scroll_position = r5;
				*p_second_player_object = object_2;
				*p_third_player_object = object_3;
				*p_end_script = player;
				*p_dest = sine_entry;
				return;
			}
			speed = 5u;
			speed = (uint16_t)(speed + follow_base_speed);
			advance_scroll(&r2, &speed, &r4, &r5, &player, &sine_entry);
			*p_position = r2;
			*p_speed = speed;
			*p_speed_value = r4;
			*p_next_scroll_position = r5;
			*p_second_player_object = object_2;
			*p_third_player_object = object_3;
			*p_end_script = player;
			*p_dest = sine_entry;
			return;
		case 1:
		case 9:
			player_speed = (int32_t)OBJECT(object)->speed >> 16;
			if ((int32_t)player_speed > 0x500 || (int32_t)player_speed < 0)
			{
				speed = 5u;
				speed = (uint16_t)(speed + follow_base_speed);
				advance_scroll(&r2, &speed, &r4, &r5, &player, &sine_entry);
				*p_position = r2;
				*p_speed = speed;
				*p_speed_value = r4;
				*p_next_scroll_position = r5;
				*p_second_player_object = object_2;
				*p_third_player_object = object_3;
				*p_end_script = player;
				*p_dest = sine_entry;
				return;
			}
			push_speed = ((uint16_t)player_speed * 0x400u) >> 16;
			if (player_past_leading_edge == 0u)
			{
				push_speed &= 0xFFu;
				push_speed = (uint16_t)(push_speed + follow_base_speed);
				advance_scroll(&r2, &push_speed, &r4, &r5, &player, &sine_entry);
				*p_position = r2;
				*p_speed = push_speed;
				*p_speed_value = r4;
				*p_next_scroll_position = r5;
				*p_second_player_object = object_2;
				*p_third_player_object = object_3;
				*p_end_script = player;
				*p_dest = sine_entry;
				return;
			}
			push_speed = (push_speed & 0xFFu) + 5u;
			push_speed &= 0xFFu;
			push_speed = (uint16_t)(push_speed + follow_base_speed);
			advance_scroll(&r2, &push_speed, &r4, &r5, &player, &sine_entry);
			*p_position = r2;
			*p_speed = push_speed;
			*p_speed_value = r4;
			*p_next_scroll_position = r5;
			*p_second_player_object = object_2;
			*p_third_player_object = object_3;
			*p_end_script = player;
			*p_dest = sine_entry;
			return;
		case 2:
		case 10:
			object = LINKED(object, PLAYER(player_2)->object);
			player_speed = (int32_t)OBJECT(object)->speed >> 16;
			if ((int32_t)player_speed > 0x500 || (int32_t)player_speed < 0)
			{
				speed = 5u;
				speed = (uint16_t)(speed + follow_base_speed);
				advance_scroll(&r2, &speed, &r4, &r5, &player, &sine_entry);
				*p_position = r2;
				*p_speed = speed;
				*p_speed_value = r4;
				*p_next_scroll_position = r5;
				*p_second_player_object = object_2;
				*p_third_player_object = object_3;
				*p_end_script = player;
				*p_dest = sine_entry;
				return;
			}
			push_speed = ((uint16_t)player_speed * 0x400u) >> 16;
			if (player_past_leading_edge != 0u)
				push_speed = (push_speed & 0xFFu) + 5u;
			push_speed &= 0xFFu;
			push_speed = (uint16_t)(push_speed + follow_base_speed);
			advance_scroll(&r2, &push_speed, &r4, &r5, &player, &sine_entry);
			*p_position = r2;
			*p_speed = push_speed;
			*p_speed_value = r4;
			*p_next_scroll_position = r5;
			*p_second_player_object = object_2;
			*p_third_player_object = object_3;
			*p_end_script = player;
			*p_dest = sine_entry;
			return;
		case 3:
			if (OBJECT(object_2)->speed < OBJECT(object)->speed)
			{
				player_speed = (int32_t)OBJECT(object)->speed >> 16;
				if ((int32_t)player_speed > 0x500 || (int32_t)player_speed < 0)
				{
					speed = 5u;
					speed = (uint16_t)(speed + follow_base_speed);
					advance_scroll(&r2, &speed, &r4, &r5, &player, &sine_entry);
					*p_position = r2;
					*p_speed = speed;
					*p_speed_value = r4;
					*p_next_scroll_position = r5;
					*p_second_player_object = object_2;
					*p_third_player_object = object_3;
					*p_end_script = player;
					*p_dest = sine_entry;
					return;
				}
				push_speed = ((uint16_t)player_speed * 0x400u) >> 16;
				if (player_past_leading_edge != 0u)
					push_speed = (push_speed & 0xFFu) + 5u;
				push_speed &= 0xFFu;
				push_speed = (uint16_t)(push_speed + follow_base_speed);
				advance_scroll(&r2, &push_speed, &r4, &r5, &player, &sine_entry);
				*p_position = r2;
				*p_speed = push_speed;
				*p_speed_value = r4;
				*p_next_scroll_position = r5;
				*p_second_player_object = object_2;
				*p_third_player_object = object_3;
				*p_end_script = player;
				*p_dest = sine_entry;
				return;
			}
			object = object_2;
			player_speed = (int32_t)OBJECT(object)->speed >> 16;
			if ((int32_t)player_speed > 0x500 || (int32_t)player_speed < 0)
			{
				speed = 5u;
				speed = (uint16_t)(speed + follow_base_speed);
				advance_scroll(&r2, &speed, &r4, &r5, &player, &sine_entry);
				*p_position = r2;
				*p_speed = speed;
				*p_speed_value = r4;
				*p_next_scroll_position = r5;
				*p_second_player_object = object_2;
				*p_third_player_object = object_3;
				*p_end_script = player;
				*p_dest = sine_entry;
				return;
			}
			push_speed = ((uint16_t)player_speed * 0x400u) >> 16;
			if (player_past_leading_edge != 0u)
				push_speed = (push_speed & 0xFFu) + 5u;
			push_speed &= 0xFFu;
			push_speed = (uint16_t)(push_speed + follow_base_speed);
			advance_scroll(&r2, &push_speed, &r4, &r5, &player, &sine_entry);
			*p_position = r2;
			*p_speed = push_speed;
			*p_speed_value = r4;
			*p_next_scroll_position = r5;
			*p_second_player_object = object_2;
			*p_third_player_object = object_3;
			*p_end_script = player;
			*p_dest = sine_entry;
			return;
		case 4:
			object = LINKED(object, PLAYER(r14)->object);
			player_speed = (int32_t)OBJECT(object)->speed >> 16;
			if ((int32_t)player_speed > 0x500 || (int32_t)player_speed < 0)
			{
				speed = 5u;
				speed = (uint16_t)(speed + follow_base_speed);
				advance_scroll(&r2, &speed, &r4, &r5, &player, &sine_entry);
				*p_position = r2;
				*p_speed = speed;
				*p_speed_value = r4;
				*p_next_scroll_position = r5;
				*p_second_player_object = object_2;
				*p_third_player_object = object_3;
				*p_end_script = player;
				*p_dest = sine_entry;
				return;
			}
			push_speed = ((uint16_t)player_speed * 0x400u) >> 16;
			if (player_past_leading_edge != 0u)
				push_speed = (push_speed & 0xFFu) + 5u;
			push_speed &= 0xFFu;
			push_speed = (uint16_t)(push_speed + follow_base_speed);
			advance_scroll(&r2, &push_speed, &r4, &r5, &player, &sine_entry);
			*p_position = r2;
			*p_speed = push_speed;
			*p_speed_value = r4;
			*p_next_scroll_position = r5;
			*p_second_player_object = object_2;
			*p_third_player_object = object_3;
			*p_end_script = player;
			*p_dest = sine_entry;
			return;
		case 5:
			if (OBJECT(object_3)->speed < OBJECT(object)->speed)
			{
				player_speed = (int32_t)OBJECT(object)->speed >> 16;
				if ((int32_t)player_speed > 0x500 || (int32_t)player_speed < 0)
				{
					speed = 5u;
					speed = (uint16_t)(speed + follow_base_speed);
					advance_scroll(&r2, &speed, &r4, &r5, &player, &sine_entry);
					*p_position = r2;
					*p_speed = speed;
					*p_speed_value = r4;
					*p_next_scroll_position = r5;
					*p_second_player_object = object_2;
					*p_third_player_object = object_3;
					*p_end_script = player;
					*p_dest = sine_entry;
					return;
				}
				push_speed = ((uint16_t)player_speed * 0x400u) >> 16;
				if (player_past_leading_edge != 0u)
					push_speed = (push_speed & 0xFFu) + 5u;
				push_speed &= 0xFFu;
				push_speed = (uint16_t)(push_speed + follow_base_speed);
				advance_scroll(&r2, &push_speed, &r4, &r5, &player, &sine_entry);
				*p_position = r2;
				*p_speed = push_speed;
				*p_speed_value = r4;
				*p_next_scroll_position = r5;
				*p_second_player_object = object_2;
				*p_third_player_object = object_3;
				*p_end_script = player;
				*p_dest = sine_entry;
				return;
			}
			object = object_3;
			player_speed = (int32_t)OBJECT(object)->speed >> 16;
			if ((int32_t)player_speed > 0x500 || (int32_t)player_speed < 0)
			{
				speed = 5u;
				speed = (uint16_t)(speed + follow_base_speed);
				advance_scroll(&r2, &speed, &r4, &r5, &player, &sine_entry);
				*p_position = r2;
				*p_speed = speed;
				*p_speed_value = r4;
				*p_next_scroll_position = r5;
				*p_second_player_object = object_2;
				*p_third_player_object = object_3;
				*p_end_script = player;
				*p_dest = sine_entry;
				return;
			}
			push_speed = ((uint16_t)player_speed * 0x400u) >> 16;
			if (player_past_leading_edge != 0u)
				push_speed = (push_speed & 0xFFu) + 5u;
			push_speed &= 0xFFu;
			push_speed = (uint16_t)(push_speed + follow_base_speed);
			advance_scroll(&r2, &push_speed, &r4, &r5, &player, &sine_entry);
			*p_position = r2;
			*p_speed = push_speed;
			*p_speed_value = r4;
			*p_next_scroll_position = r5;
			*p_second_player_object = object_2;
			*p_third_player_object = object_3;
			*p_end_script = player;
			*p_dest = sine_entry;
			return;
		case 6:
			object = object_3;
			if (OBJECT(object_2)->speed < OBJECT(object)->speed)
			{
				player_speed = (int32_t)OBJECT(object)->speed >> 16;
				if ((int32_t)player_speed > 0x500 || (int32_t)player_speed < 0)
				{
					speed = 5u;
					speed = (uint16_t)(speed + follow_base_speed);
					advance_scroll(&r2, &speed, &r4, &r5, &player, &sine_entry);
					*p_position = r2;
					*p_speed = speed;
					*p_speed_value = r4;
					*p_next_scroll_position = r5;
					*p_second_player_object = object_2;
					*p_third_player_object = object_3;
					*p_end_script = player;
					*p_dest = sine_entry;
					return;
				}
				push_speed = ((uint16_t)player_speed * 0x400u) >> 16;
				if (player_past_leading_edge != 0u)
					push_speed = (push_speed & 0xFFu) + 5u;
				push_speed &= 0xFFu;
				push_speed = (uint16_t)(push_speed + follow_base_speed);
				advance_scroll(&r2, &push_speed, &r4, &r5, &player, &sine_entry);
				*p_position = r2;
				*p_speed = push_speed;
				*p_speed_value = r4;
				*p_next_scroll_position = r5;
				*p_second_player_object = object_2;
				*p_third_player_object = object_3;
				*p_end_script = player;
				*p_dest = sine_entry;
				return;
			}
			object = object_2;
			player_speed = (int32_t)OBJECT(object)->speed >> 16;
			if ((int32_t)player_speed > 0x500 || (int32_t)player_speed < 0)
			{
				speed = 5u;
				speed = (uint16_t)(speed + follow_base_speed);
				advance_scroll(&r2, &speed, &r4, &r5, &player, &sine_entry);
				*p_position = r2;
				*p_speed = speed;
				*p_speed_value = r4;
				*p_next_scroll_position = r5;
				*p_second_player_object = object_2;
				*p_third_player_object = object_3;
				*p_end_script = player;
				*p_dest = sine_entry;
				return;
			}
			push_speed = ((uint16_t)player_speed * 0x400u) >> 16;
			if (player_past_leading_edge != 0u)
				push_speed = (push_speed & 0xFFu) + 5u;
			push_speed &= 0xFFu;
			push_speed = (uint16_t)(push_speed + follow_base_speed);
			advance_scroll(&r2, &push_speed, &r4, &r5, &player, &sine_entry);
			*p_position = r2;
			*p_speed = push_speed;
			*p_speed_value = r4;
			*p_next_scroll_position = r5;
			*p_second_player_object = object_2;
			*p_third_player_object = object_3;
			*p_end_script = player;
			*p_dest = sine_entry;
			return;
		case 7:
			if (OBJECT(object)->speed <= OBJECT(object_2)->speed)
				object = object_2;
			if (OBJECT(object_3)->speed < OBJECT(object)->speed)
			{
				player_speed = (int32_t)OBJECT(object)->speed >> 16;
				if ((int32_t)player_speed > 0x500 || (int32_t)player_speed < 0)
				{
					speed = 5u;
					speed = (uint16_t)(speed + follow_base_speed);
					advance_scroll(&r2, &speed, &r4, &r5, &player, &sine_entry);
					*p_position = r2;
					*p_speed = speed;
					*p_speed_value = r4;
					*p_next_scroll_position = r5;
					*p_second_player_object = object_2;
					*p_third_player_object = object_3;
					*p_end_script = player;
					*p_dest = sine_entry;
					return;
				}
				push_speed = ((uint16_t)player_speed * 0x400u) >> 16;
				if (player_past_leading_edge != 0u)
					push_speed = (push_speed & 0xFFu) + 5u;
				push_speed &= 0xFFu;
				push_speed = (uint16_t)(push_speed + follow_base_speed);
				advance_scroll(&r2, &push_speed, &r4, &r5, &player, &sine_entry);
				*p_position = r2;
				*p_speed = push_speed;
				*p_speed_value = r4;
				*p_next_scroll_position = r5;
				*p_second_player_object = object_2;
				*p_third_player_object = object_3;
				*p_end_script = player;
				*p_dest = sine_entry;
				return;
			}
			object = object_3;
			player_speed = (int32_t)OBJECT(object)->speed >> 16;
			if ((int32_t)player_speed > 0x500 || (int32_t)player_speed < 0)
			{
				speed = 5u;
				speed = (uint16_t)(speed + follow_base_speed);
				advance_scroll(&r2, &speed, &r4, &r5, &player, &sine_entry);
				*p_position = r2;
				*p_speed = speed;
				*p_speed_value = r4;
				*p_next_scroll_position = r5;
				*p_second_player_object = object_2;
				*p_third_player_object = object_3;
				*p_end_script = player;
				*p_dest = sine_entry;
				return;
			}
			push_speed = ((uint16_t)player_speed * 0x400u) >> 16;
			if (player_past_leading_edge != 0u)
				push_speed = (push_speed & 0xFFu) + 5u;
			push_speed &= 0xFFu;
			push_speed = (uint16_t)(push_speed + follow_base_speed);
			advance_scroll(&r2, &push_speed, &r4, &r5, &player, &sine_entry);
			*p_position = r2;
			*p_speed = push_speed;
			*p_speed_value = r4;
			*p_next_scroll_position = r5;
			*p_second_player_object = object_2;
			*p_third_player_object = object_3;
			*p_end_script = player;
			*p_dest = sine_entry;
			return;
		default:
			unknown_code(0x16151u + players_pushing_edge * 3u);
		}
		break;
	default:
		unknown_code(0x15FA9u + scroll_mode * 3u);
	}
}

uint32_t is_player_pushing_edge(uint32_t pushing, uint32_t object)
{
	uint16_t edge_offset = 0;
	uint32_t current_position = 0;

	switch (push_edge_case)
	{
	case 0:
	case 9:
		pushing &= ~0xFFu;
		return pushing;
	case 1:
		pushing = is_player_at_left_edge(pushing, object);
		if ((uint8_t)pushing != 0u)
		{
			player_at_trailing_edge = 1u;
			pushing &= ~0xFFu;
			return pushing;
		}
		pushing = is_player_at_right_edge(pushing, object);
		if ((uint8_t)pushing != 0u)
		{
			pushing = (pushing & ~0xFFu) | 1u;
			return pushing;
		}
		pushing &= ~0xFFu;
		return pushing;
	case 2:
		pushing = is_player_at_left_edge(pushing, object);
		if ((uint8_t)pushing == 0u)
		{
			pushing = is_player_at_top_edge(pushing, object);
			if ((uint8_t)pushing != 0u)
			{
				player_at_trailing_edge = 1u;
				pushing &= ~0xFFu;
				return pushing;
			}
			edge_offset = 0xF0u;
			pushing = is_player_at_diagonal_edge_f0(pushing, object, edge_offset);
			if ((uint8_t)pushing != 0u)
			{
				pushing = (pushing & ~0xFFu) | 1u;
				return pushing;
			}
			pushing = is_player_at_right_edge(pushing, object);
			if ((uint8_t)pushing != 0u)
			{
				pushing = (pushing & ~0xFFu) | 1u;
				return pushing;
			}
			pushing = is_player_at_bottom_edge(pushing, object);
			if ((uint8_t)pushing != 0u)
			{
				pushing = (pushing & ~0xFFu) | 1u;
				return pushing;
			}
			pushing &= ~0xFFu;
			return pushing;
		}
		player_at_trailing_edge = 1u;
		pushing &= ~0xFFu;
		return pushing;
	case 3:
		pushing = is_player_at_top_edge(pushing, object);
		if ((uint8_t)pushing != 0u)
		{
			player_at_trailing_edge = 1u;
			pushing &= ~0xFFu;
			return pushing;
		}
		pushing = is_player_at_bottom_edge(pushing, object);
		if ((uint8_t)pushing != 0u)
		{
			pushing = (pushing & ~0xFFu) | 1u;
			return pushing;
		}
		pushing &= ~0xFFu;
		return pushing;
	case 4:
		pushing = is_player_at_right_edge(pushing, object);
		if ((uint8_t)pushing == 0u)
		{
			pushing = is_player_at_top_edge(pushing, object);
			if ((uint8_t)pushing != 0u)
			{
				player_at_trailing_edge = 1u;
				pushing &= ~0xFFu;
				return pushing;
			}
			edge_offset = 0x60u;
			pushing = is_player_at_diagonal_edge_60(pushing, object, edge_offset);
			if ((uint8_t)pushing != 0u)
			{
				pushing = (pushing & ~0xFFu) | 1u;
				return pushing;
			}
			pushing = is_player_at_left_edge(pushing, object);
			if ((uint8_t)pushing != 0u)
			{
				pushing = (pushing & ~0xFFu) | 1u;
				return pushing;
			}
			pushing = is_player_at_bottom_edge(pushing, object);
			if ((uint8_t)pushing != 0u)
			{
				pushing = (pushing & ~0xFFu) | 1u;
				return pushing;
			}
			pushing &= ~0xFFu;
			return pushing;
		}
		player_at_trailing_edge = 1u;
		pushing &= ~0xFFu;
		return pushing;
	case 5:
		pushing = is_player_at_right_edge(pushing, object);
		if ((uint8_t)pushing != 0u)
		{
			player_at_trailing_edge = 1u;
			pushing &= ~0xFFu;
			return pushing;
		}
		pushing = is_player_at_left_edge(pushing, object);
		if ((uint8_t)pushing != 0u)
		{
			pushing = (pushing & ~0xFFu) | 1u;
			return pushing;
		}
		pushing &= ~0xFFu;
		return pushing;
	case 6:
		pushing = is_player_at_bottom_edge(pushing, object);
		if ((uint8_t)pushing == 0u)
		{
			pushing = is_player_at_right_edge(pushing, object);
			if ((uint8_t)pushing != 0u)
			{
				player_at_trailing_edge = 1u;
				pushing &= ~0xFFu;
				return pushing;
			}
			edge_offset = 0xC0u;
			pushing = is_player_at_diagonal_edge_c0(pushing, object, edge_offset);
			if ((uint8_t)pushing != 0u)
			{
				pushing = (pushing & ~0xFFu) | 1u;
				return pushing;
			}
			pushing = is_player_at_left_edge(pushing, object);
			if ((uint8_t)pushing != 0u)
			{
				pushing = (pushing & ~0xFFu) | 1u;
				return pushing;
			}
			pushing = is_player_at_top_edge(pushing, object);
			if ((uint8_t)pushing != 0u)
			{
				pushing = (pushing & ~0xFFu) | 1u;
				return pushing;
			}
			pushing &= ~0xFFu;
			return pushing;
		}
		player_at_trailing_edge = 1u;
		pushing &= ~0xFFu;
		return pushing;
	case 7:
		pushing = is_player_at_bottom_edge(pushing, object);
		if ((uint8_t)pushing != 0u)
		{
			player_at_trailing_edge = 1u;
			pushing &= ~0xFFu;
			return pushing;
		}
		pushing = is_player_at_top_edge(pushing, object);
		if ((uint8_t)pushing != 0u)
		{
			pushing = (pushing & ~0xFFu) | 1u;
			return pushing;
		}
		pushing &= ~0xFFu;
		return pushing;
	case 8:
		pushing = is_player_at_bottom_edge(pushing, object);
		if ((uint8_t)pushing == 0u)
		{
			pushing = is_player_at_left_edge(pushing, object);
			if ((uint8_t)pushing != 0u)
			{
				player_at_trailing_edge = 1u;
				pushing &= ~0xFFu;
				return pushing;
			}
			if (stage != 3u)
			{
				edge_offset = 0x30u;
				pushing = is_player_at_diagonal_edge_30(pushing, object, edge_offset);
				if ((uint8_t)pushing != 0u)
				{
					pushing = (pushing & ~0xFFu) | 1u;
					return pushing;
				}
				pushing = is_player_at_right_edge(pushing, object);
				if ((uint8_t)pushing != 0u)
				{
					pushing = (pushing & ~0xFFu) | 1u;
					return pushing;
				}
				pushing = is_player_at_top_edge(pushing, object);
				if ((uint8_t)pushing != 0u)
				{
					pushing = (pushing & ~0xFFu) | 1u;
					return pushing;
				}
				pushing &= ~0xFFu;
				return pushing;
			}
			current_position = scroll_position;
			if ((int32_t)current_position < 0x87A9 || (int32_t)current_position > 0x96F1)
			{
				edge_offset = 0x30u;
				pushing = is_player_at_diagonal_edge_30(pushing, object, edge_offset);
				if ((uint8_t)pushing != 0u)
				{
					pushing = (pushing & ~0xFFu) | 1u;
					return pushing;
				}
				pushing = is_player_at_right_edge(pushing, object);
				if ((uint8_t)pushing != 0u)
				{
					pushing = (pushing & ~0xFFu) | 1u;
					return pushing;
				}
				pushing = is_player_at_top_edge(pushing, object);
				if ((uint8_t)pushing != 0u)
				{
					pushing = (pushing & ~0xFFu) | 1u;
					return pushing;
				}
				pushing &= ~0xFFu;
				return pushing;
			}
			pushing = is_player_at_top_edge(pushing, object);
			if ((uint8_t)pushing != 0u)
			{
				pushing = (pushing & ~0xFFu) | 1u;
				return pushing;
			}
			pushing &= ~0xFFu;
			return pushing;
		}
		player_at_trailing_edge = 1u;
		pushing &= ~0xFFu;
		return pushing;
	default:
		unknown_code(0x16274u + push_edge_case * 3u);
	}
}

uint32_t is_player_at_diagonal_edge_30(uint32_t at_edge, uint32_t object, uint16_t edge_offset)
{
	uint16_t screen_y = 0;
	uint32_t passes_left = 0;

	passes_left = 2u;
	at_edge &= ~0xFFFFu;
	player_past_leading_edge = 0u;
	do
	{
		screen_y = (uint16_t)OBJECT(object)->screen_y;
		if ((int16_t)(((int16_t)OBJECT(object)->screen_x >> 1) + edge_offset) < (int16_t)screen_y)
		{
			if ((int16_t)((uint32_t)(screen_y - edge_offset) << 1) <= OBJECT(object)->screen_x)
			{
				at_edge = (at_edge & ~0xFFu) | 1u;
				if ((uint16_t)passes_left == 1u)
					player_past_leading_edge = 1u;
			}
		}
		else
		{
			at_edge = (at_edge & ~0xFFu) | 1u;
			if ((uint16_t)passes_left == 1u)
				player_past_leading_edge = 1u;
		}
		edge_offset -= 0xAu;
		passes_left--;
	} while (passes_left != 0u);
	return at_edge;
}

uint32_t is_player_at_diagonal_edge_f0(uint32_t at_edge, uint32_t object, uint16_t edge_offset)
{
	uint16_t screen_y = 0;
	uint32_t passes_left = 0;

	passes_left = 2u;
	at_edge &= ~0xFFFFu;
	player_past_leading_edge = 0u;
	do
	{
		screen_y = (uint16_t)OBJECT(object)->screen_y;
		if ((int16_t)screen_y < (int16_t)(0u - ((int16_t)OBJECT(object)->screen_x >> 1) + edge_offset))
		{
			if ((int16_t)((0u - (screen_y - edge_offset)) << 1) <= OBJECT(object)->screen_x)
			{
				at_edge = (at_edge & ~0xFFu) | 1u;
				if ((uint16_t)passes_left == 1u)
					player_past_leading_edge = 1u;
			}
		}
		else
		{
			at_edge = (at_edge & ~0xFFu) | 1u;
			if ((uint16_t)passes_left == 1u)
				player_past_leading_edge = 1u;
		}
		edge_offset -= 0xAu;
		passes_left--;
	} while (passes_left != 0u);
	return at_edge;
}

uint32_t is_player_at_diagonal_edge_c0(uint32_t at_edge, uint32_t object, uint16_t edge_offset)
{
	uint16_t screen_y = 0;
	uint32_t passes_left = 0;

	passes_left = 2u;
	at_edge &= ~0xFFFFu;
	player_past_leading_edge = 0u;
	do
	{
		screen_y = (uint16_t)OBJECT(object)->screen_y;
		if ((int16_t)(0u - ((int16_t)OBJECT(object)->screen_x >> 1) + edge_offset) < (int16_t)screen_y)
		{
			if (OBJECT(object)->screen_x <= (int16_t)((0u - (screen_y - edge_offset)) << 1))
			{
				at_edge = (at_edge & ~0xFFu) | 1u;
				if ((uint16_t)passes_left == 1u)
					player_past_leading_edge = 1u;
			}
		}
		else
		{
			at_edge = (at_edge & ~0xFFu) | 1u;
			if ((uint16_t)passes_left == 1u)
				player_past_leading_edge = 1u;
		}
		edge_offset -= 0xAu;
		passes_left--;
	} while (passes_left != 0u);
	return at_edge;
}

uint32_t is_player_at_diagonal_edge_60(uint32_t at_edge, uint32_t object, uint16_t edge_offset)
{
	uint16_t screen_y = 0;
	uint32_t passes_left = 0;

	passes_left = 2u;
	at_edge &= ~0xFFFFu;
	player_past_leading_edge = 0u;
	do
	{
		screen_y = (uint16_t)OBJECT(object)->screen_y;
		if ((int16_t)screen_y < (int16_t)(((int16_t)OBJECT(object)->screen_x >> 1) + edge_offset))
		{
			if (OBJECT(object)->screen_x <= (int16_t)((uint32_t)(screen_y - edge_offset) << 1))
			{
				at_edge = (at_edge & ~0xFFu) | 1u;
				if ((uint16_t)passes_left == 1u)
					player_past_leading_edge = 1u;
			}
		}
		else
		{
			at_edge = (at_edge & ~0xFFu) | 1u;
			if ((uint16_t)passes_left == 1u)
				player_past_leading_edge = 1u;
		}
		edge_offset -= 0xAu;
		passes_left--;
	} while (passes_left != 0u);
	return at_edge;
}

uint32_t is_player_at_right_edge(uint32_t at_edge, uint32_t object)
{
	uint32_t leading_limit = 0;

	if (OBJECT(object)->screen_x < (int16_t)push_edge_right)
	{
		at_edge &= ~0xFFu;
		return at_edge;
	}
	leading_limit = (at_edge & ~0xFFFFu) | push_edge_right;
	leading_limit = (leading_limit & ~0xFFFFu) | (uint16_t)(leading_limit + 0xAu);
	if ((int16_t)leading_limit <= OBJECT(object)->screen_x)
		player_past_leading_edge = 1u;
	at_edge = (leading_limit & ~0xFFu) | 1u;
	return at_edge;
}

uint32_t is_player_at_left_edge(uint32_t at_edge, uint32_t object)
{
	uint32_t leading_limit = 0;

	if ((int16_t)push_edge_left < OBJECT(object)->screen_x)
	{
		at_edge &= ~0xFFu;
		return at_edge;
	}
	leading_limit = (at_edge & ~0xFFFFu) | push_edge_left;
	leading_limit = (leading_limit & ~0xFFFFu) | (uint16_t)(leading_limit - 0xAu);
	if (OBJECT(object)->screen_x <= (int16_t)leading_limit)
		player_past_leading_edge = 1u;
	at_edge = (leading_limit & ~0xFFu) | 1u;
	return at_edge;
}

uint32_t is_player_at_top_edge(uint32_t at_edge, uint32_t object)
{
	uint32_t leading_limit = 0;

	if ((int16_t)push_edge_top < OBJECT(object)->screen_y)
	{
		at_edge &= ~0xFFu;
		return at_edge;
	}
	leading_limit = (at_edge & ~0xFFFFu) | push_edge_top;
	leading_limit = (leading_limit & ~0xFFFFu) | (uint16_t)(leading_limit - 0xAu);
	if (OBJECT(object)->screen_y <= (int16_t)leading_limit)
		player_past_leading_edge = 1u;
	at_edge = (leading_limit & ~0xFFu) | 1u;
	return at_edge;
}

uint32_t is_player_at_bottom_edge(uint32_t at_edge, uint32_t object)
{
	uint32_t leading_limit = 0;

	if (OBJECT(object)->screen_y < (int16_t)push_edge_bottom)
	{
		at_edge &= ~0xFFu;
		return at_edge;
	}
	leading_limit = (at_edge & ~0xFFFFu) | push_edge_bottom;
	leading_limit = (leading_limit & ~0xFFFFu) | (uint16_t)(leading_limit + 0xAu);
	if ((int16_t)leading_limit <= OBJECT(object)->screen_y)
		player_past_leading_edge = 1u;
	at_edge = (leading_limit & ~0xFFu) | 1u;
	return at_edge;
}

void set_screen_push_edges(uint32_t quadrant_bits, uint32_t *p_count, uint32_t *p_sine_entry)
{
	uint32_t count = 0, sine_entry = 0;

	sine_entry = sine_table_ADDRESS;
	if ((int16_t)AS16(section_direction_y) < 0)
	{
		if ((int16_t)AS16(section_direction_x) >= 0)
		{
			sine_entry += 0x200u;
			quadrant_bits |= 8u;
		}
		sine_entry += 0x200u;
		quadrant_bits |= 4u;
		goto set_screen_push_edges_past_quadrant_0;
	}
	if ((int16_t)AS16(section_direction_x) < 0)
	{
	set_screen_push_edges_past_quadrant_0:
		sine_entry += 0x200u;
		quadrant_bits |= 2u;
	}
	count = 0x100u;
	push_edge_top = 0xFFFFu;
	push_edge_bottom = 0xFFFFu;
	push_edge_right = 0xFFFFu;
	push_edge_left = 0xFFFFu;
	switch (quadrant_bits | 1u)
	{
	case 0:
	case 2:
	case 4:
	case 5:
	case 6:
	case 8:
	case 9:
	case 10:
	case 11:
	case 12:
	case 13:
	case 14:
	case 16:
		*p_count = count;
		*p_sine_entry = sine_entry;
		return;
	case 1:
		push_edge_case = 0u;
		if (AS16(section_direction_y) != 0u)
		{
			push_edge_bottom = 0x88u;
			push_edge_top = 0x20u;
			push_edge_case++;
		}
		push_edge_case++;
		push_edge_right = 0xB8u;
		push_edge_left = 0x40u;
		*p_count = count;
		*p_sine_entry = sine_entry;
		return;
	case 3:
		push_edge_case = 2u;
		if (AS16(section_direction_x) != 0u)
		{
			push_edge_left = 0x88u;
			push_edge_right = 0x100u;
			push_edge_case++;
		}
		push_edge_case++;
		push_edge_bottom = 0x88u;
		push_edge_top = 0x20u;
		*p_count = count;
		*p_sine_entry = sine_entry;
		return;
	case 7:
		push_edge_case = 4u;
		if (AS16(section_direction_y) != 0u)
		{
			push_edge_top = 0x58u;
			push_edge_bottom = 0xC0u;
			push_edge_case++;
		}
		push_edge_case++;
		push_edge_left = 0x88u;
		push_edge_right = 0x100u;
		*p_count = count;
		*p_sine_entry = sine_entry;
		return;
	case 15:
		push_edge_case = 6u;
		if (AS16(section_direction_x) != 0u)
		{
			push_edge_right = 0xB8u;
			push_edge_left = 0x40u;
			push_edge_case++;
		}
		push_edge_case++;
		push_edge_top = 0x58u;
		push_edge_bottom = 0xC0u;
		*p_count = count;
		*p_sine_entry = sine_entry;
		return;
	default:
		unknown_code(0x16726u + (quadrant_bits | 1u) * 3u);
	}
}

uint32_t is_player_facing_scroll(uint32_t facing, uint16_t heading, uint16_t sine_entry)
{
	uint16_t sine_offset = 0;

	heading = (uint16_t)((uint32_t)heading << 1);
	sine_offset = (uint16_t)(sine_entry - sine_table_ADDRESS);
	if ((int16_t)sine_offset < 0x200)
	{
		if ((int16_t)heading < 0x100 || (int16_t)heading > 0x500)
		{
			facing &= ~0xFFu;
			return facing;
		}
		facing = (facing & ~0xFFu) | 1u;
		return facing;
	}
	if ((int16_t)sine_offset < 0x400)
	{
		if ((int16_t)heading < 0x300 || (int16_t)heading > 0x700)
		{
			facing &= ~0xFFu;
			return facing;
		}
		facing = (facing & ~0xFFu) | 1u;
		return facing;
	}
	if ((int16_t)sine_offset < 0x600)
	{
		if ((int16_t)heading < 0x500)
			heading += 0x800u;
		if ((int16_t)heading > 0x900)
		{
			facing &= ~0xFFu;
			return facing;
		}
		facing = (facing & ~0xFFu) | 1u;
		return facing;
	}
	if ((int16_t)heading < 0x700)
		heading += 0x800u;
	if ((int16_t)heading > 0xB00)
	{
		facing &= ~0xFFu;
		return facing;
	}
	facing = (facing & ~0xFFu) | 1u;
	return facing;
}

void is_direction_in_quadrant(uint32_t count, uint32_t *p_found, uint32_t *p_sine_entry)
{
	uint32_t sine_entry = *p_sine_entry;
	uint8_t entry_at_least = 0, found = 0, next_at_least = 0;
	uint16_t direction_y = 0, next_entry = 0;

	for (;;)
	{
		if (AS16(section_direction_y) != 0u)
		{
			if (AS16(section_direction_x) == 0u && (AS16(section_direction_y) == 0x4000u || AS16(section_direction_y) == 0xC000u))
			{
				found = 1u;
				*p_found = (*p_found & 0xFFFFFF00u) | found;
				*p_sine_entry = sine_entry;
				return;
			}
		}
		else if (AS16(section_direction_x) == 0x4000u || AS16(section_direction_x) == 0xC000u)
			break;
		direction_y = AS16(section_direction_y);
		next_entry = mem_read16(sine_entry + 2u);
		if ((int16_t)mem_read16(sine_entry) < (int16_t)direction_y)
			entry_at_least = 0u;
		else
			entry_at_least = 1u;
		if ((int16_t)next_entry < (int16_t)direction_y)
			next_at_least = 0u;
		else
			next_at_least = 1u;
		if (next_at_least != entry_at_least)
		{
			found = 1u;
			*p_found = (*p_found & 0xFFFFFF00u) | found;
			*p_sine_entry = sine_entry;
			return;
		}
		sine_entry += 2u;
		count--;
		if (count == 0u)
		{
			found = 0u;
			*p_found = (*p_found & 0xFFFFFF00u) | found;
			*p_sine_entry = sine_entry;
			return;
		}
	}
	found = 1u;
	*p_found = (*p_found & 0xFFFFFF00u) | found;
	*p_sine_entry = sine_entry;
}

void scroll_slowing_down(uint32_t *p_other_list, uint32_t *p_speed, uint32_t *p_speed_value, uint32_t *p_next_scroll_position, uint32_t *p_range, uint32_t *p_quadrant_cursor)
{
	uint32_t other_list = *p_other_list;
	uint32_t speed_value = *p_speed_value;
	uint32_t next_scroll_position = *p_next_scroll_position;
	uint32_t range = *p_range;
	uint32_t quadrant_cursor = *p_quadrant_cursor;
	uint32_t speed = 0;

	scroll_mode_timer--;
	if (scroll_mode_timer != 0u || (int16_t)scroll_speed < 2)
	{
		scroll_at_scroll_speed(&other_list, &speed, &speed_value, &next_scroll_position, &range, &quadrant_cursor);
		*p_other_list = other_list;
		*p_speed = speed;
		*p_speed_value = speed_value;
		*p_next_scroll_position = next_scroll_position;
		*p_range = range;
		*p_quadrant_cursor = quadrant_cursor;
		return;
	}
	scroll_speed--;
	scroll_mode_timer = scroll_mode_duration;
	scroll_at_scroll_speed(&other_list, &speed, &speed_value, &next_scroll_position, &range, &quadrant_cursor);
	*p_other_list = other_list;
	*p_speed = speed;
	*p_speed_value = speed_value;
	*p_next_scroll_position = next_scroll_position;
	*p_range = range;
	*p_quadrant_cursor = quadrant_cursor;
}

void scroll_speeding_up(uint32_t *p_other_list, uint32_t *p_speed, uint32_t *p_speed_value, uint32_t *p_next_scroll_position, uint32_t *p_range, uint32_t *p_quadrant_cursor)
{
	uint32_t other_list = *p_other_list;
	uint32_t speed = *p_speed;
	uint32_t speed_value = *p_speed_value;
	uint32_t next_scroll_position = *p_next_scroll_position;
	uint32_t range = *p_range;
	uint32_t quadrant_cursor = *p_quadrant_cursor;
	uint32_t r2 = 0, r4 = 0;

	scroll_mode_timer--;
	if (scroll_mode_timer != 0u)
	{
		speed = scroll_ramp_speed;
		advance_scroll_unless_ended(&other_list, &speed, &speed_value, &next_scroll_position, &range, &quadrant_cursor);
		*p_other_list = other_list;
		*p_speed = speed;
		*p_speed_value = speed_value;
		*p_next_scroll_position = next_scroll_position;
		*p_range = range;
		*p_quadrant_cursor = quadrant_cursor;
		return;
	}
	if ((int8_t)AS8(scroll_speed) <= (int8_t)scroll_ramp_speed)
	{
		scroll_mode = 0u;
		scroll_ramp_speed = 1u;
		set_camera_velocity(&r2, &speed, &r4, &next_scroll_position, &range, &quadrant_cursor);
		*p_other_list = r2;
		*p_speed = speed;
		*p_speed_value = r4;
		*p_next_scroll_position = next_scroll_position;
		*p_range = range;
		*p_quadrant_cursor = quadrant_cursor;
		return;
	}
	scroll_ramp_speed++;
	scroll_mode_timer = scroll_mode_duration;
	speed = scroll_ramp_speed;
	advance_scroll_unless_ended(&other_list, &speed, &speed_value, &next_scroll_position, &range, &quadrant_cursor);
	*p_other_list = other_list;
	*p_speed = speed;
	*p_speed_value = speed_value;
	*p_next_scroll_position = next_scroll_position;
	*p_range = range;
	*p_quadrant_cursor = quadrant_cursor;
}

void scroll_at_scroll_speed(uint32_t *p_other_list, uint32_t *p_speed, uint32_t *p_speed_value, uint32_t *p_next_scroll_position, uint32_t *p_range, uint32_t *p_quadrant_cursor)
{
	uint32_t other_list = *p_other_list;
	uint32_t speed_value = *p_speed_value;
	uint32_t next_scroll_position = *p_next_scroll_position;
	uint32_t range = *p_range;
	uint32_t quadrant_cursor = *p_quadrant_cursor;
	uint32_t speed = 0;

	speed = scroll_speed;
	advance_scroll_unless_ended(&other_list, &speed, &speed_value, &next_scroll_position, &range, &quadrant_cursor);
	*p_other_list = other_list;
	*p_speed = speed;
	*p_speed_value = speed_value;
	*p_next_scroll_position = next_scroll_position;
	*p_range = range;
	*p_quadrant_cursor = quadrant_cursor;
}

void advance_scroll_unless_ended(uint32_t *p_other_list, uint32_t *p_speed, uint32_t *p_speed_value, uint32_t *p_next_scroll_position, uint32_t *p_range, uint32_t *p_quadrant_cursor)
{
	uint32_t other_list = *p_other_list;
	uint32_t speed = *p_speed;
	uint32_t speed_value = *p_speed_value;
	uint32_t next_scroll_position = *p_next_scroll_position;
	uint32_t range = *p_range;
	uint32_t quadrant_cursor = *p_quadrant_cursor;
	uint32_t r2 = 0, r4 = 0, r5 = 0;

	if (scroll_ended < 0)
		return;
	advance_scroll(&r2, &speed, &r4, &r5, &range, &quadrant_cursor);
	*p_other_list = r2;
	*p_speed = speed;
	*p_speed_value = r4;
	*p_next_scroll_position = r5;
	*p_range = range;
	*p_quadrant_cursor = quadrant_cursor;
}

void advance_scroll(uint32_t *p_other_list, uint32_t *p_speed, uint32_t *p_speed_value, uint32_t *p_next_scroll_position, uint32_t *p_range, uint32_t *p_quadrant_cursor)
{
	uint32_t speed = *p_speed;
	uint32_t range = *p_range;
	uint32_t quadrant_cursor = *p_quadrant_cursor;
	uint32_t r2 = 0, r4 = 0, direction_y_list = 0, limit_x_cursor = 0, limit_y_cursor = 0, list = 0, mode_script_cursor = 0, next_scroll_position = 0;
	uint16_t direction_y = 0;

	next_scroll_position = move_camera_along_path(speed);
	update_camera_view();
	if ((int32_t)scroll_position < (int32_t)ROM16(section_end_list))
	{
		set_camera_velocity(&r2, &speed, &r4, &next_scroll_position, &range, &quadrant_cursor);
		*p_other_list = r2;
		*p_speed = speed;
		*p_speed_value = r4;
		*p_next_scroll_position = next_scroll_position;
		*p_range = range;
		*p_quadrant_cursor = quadrant_cursor;
		return;
	}
	back_edge_frames = 0u;
	list = section_speed_list;
	scroll_speed = ROM8(list);
	section_speed_list = list + 1u;
	list = follow_base_speed_list;
	follow_base_speed = mem_read8(list);
	follow_base_speed_list = list + 1u;
	limit_y_cursor = section_limit_y_list;
	section_limit_y = ROM16(limit_y_cursor);
	section_limit_y_list = limit_y_cursor + 2u;
	limit_x_cursor = section_limit_x_list;
	section_limit_x = ROM16(limit_x_cursor);
	section_limit_x_list = limit_x_cursor + 2u;
	mode_script_cursor = scroll_mode_script;
	scroll_mode = ROM16(mode_script_cursor);
	mode_script_cursor += 2u;
	scroll_mode_timer = ROM16(mode_script_cursor);
	scroll_mode_duration = scroll_mode_timer;
	scroll_mode_script = mode_script_cursor + 2u;
	list = section_direction_x_list;
	direction_y_list = section_direction_y_list;
	direction_y = ROM16(direction_y_list);
	section_direction_x = (int16_t)ROM16(list);
	section_direction_y = (int16_t)direction_y;
	section_direction_x_list = list + 2u;
	section_direction_y_list = direction_y_list + 2u;
	section_end_list += 2u;
	section_unused_cursor += 2u;
	set_camera_velocity(&r2, &speed, &r4, &next_scroll_position, &range, &quadrant_cursor);
	*p_other_list = r2;
	*p_speed = speed;
	*p_speed_value = r4;
	*p_next_scroll_position = next_scroll_position;
	*p_range = range;
	*p_quadrant_cursor = quadrant_cursor;
}

void set_camera_velocity(uint32_t *p_scroll_y, uint32_t *p_speed, uint32_t *p_speed_value, uint32_t *p_next_scroll_position, uint32_t *p_range, uint32_t *p_quadrant_cursor)
{
	uint32_t speed = *p_speed;
	uint32_t next_scroll_position = *p_next_scroll_position;
	uint32_t range = *p_range;
	uint32_t quadrant_cursor = *p_quadrant_cursor;
	uint32_t r2 = 0, r10 = 0, ignored = 0, speed_value = 0, velocity_y = 0;

	speed_value = (uint16_t)speed;
	velocity_y = section_direction_y * speed_value;
	camera_velocity_x = section_direction_x * speed_value;
	camera_velocity_y = velocity_y;
	camera_shake_length = 0x200u;
	camera_shake_wave = 0x200u;
	r2 = update_camera_with_shake();
	if (scroll_ended < 0)
	{
		*p_scroll_y = r2;
		*p_speed_value = speed_value;
		return;
	}
	update_background_scroll(&ignored, &r2, &speed, &speed_value, &next_scroll_position, &r10, &quadrant_cursor);
	*p_scroll_y = r2;
	*p_speed = speed;
	*p_speed_value = speed_value;
	*p_next_scroll_position = next_scroll_position;
	*p_range = r10;
	*p_quadrant_cursor = quadrant_cursor;
}

void update_background_scroll(uint32_t *p_fast_scroll_start, uint32_t *p_scroll_y, uint32_t *p_speed, uint32_t *p_speed_value, uint32_t *p_next_scroll_position, uint32_t *p_range, uint32_t *p_quadrant_cursor)
{
	uint32_t scroll_y = *p_scroll_y;
	uint32_t speed = *p_speed;
	uint32_t speed_value = *p_speed_value;
	uint32_t next_scroll_position = *p_next_scroll_position;
	uint32_t quadrant_cursor = *p_quadrant_cursor;
	uint32_t current_position = 0, fast_scroll_start = 0, range = 0, section_start = 0;

	update_nbg1_scroll();
	update_nbg3_scroll(&scroll_y, &speed, &speed_value, &next_scroll_position, &quadrant_cursor);
	speed = update_nbg2_scroll(speed);
	fast_scroll_start = stage;
	range = stage_fast_scroll_ranges[fast_scroll_start] + stage_fast_scroll_ranges_ADDRESS;
	section_start = mem_read16(section_end_list - 2u);
	do
	{
		if (ROM16(range) == 0xFFFFu)
		{
			*p_fast_scroll_start = fast_scroll_start;
			*p_scroll_y = scroll_y;
			*p_speed = speed;
			*p_speed_value = speed_value;
			*p_next_scroll_position = next_scroll_position;
			*p_range = range;
			*p_quadrant_cursor = quadrant_cursor;
			return;
		}
		fast_scroll_start = ROM16(range);
		range += 2u;
	} while ((int32_t)fast_scroll_start <= (int32_t)section_start);
	current_position = scroll_position;
	if ((int32_t)current_position < (int32_t)fast_scroll_start)
	{
		*p_fast_scroll_start = fast_scroll_start;
		*p_scroll_y = current_position;
		*p_speed = speed;
		*p_speed_value = speed_value;
		*p_next_scroll_position = next_scroll_position;
		*p_range = range;
		*p_quadrant_cursor = quadrant_cursor;
		return;
	}
	follow_base_speed = 0x10u;
	*p_fast_scroll_start = fast_scroll_start;
	*p_scroll_y = current_position;
	*p_speed = speed;
	*p_speed_value = speed_value;
	*p_next_scroll_position = next_scroll_position;
	*p_range = range;
	*p_quadrant_cursor = quadrant_cursor;
}

void spawn_debug_sprite_mover(uint32_t object)
{
	uint32_t free_count = 0;

	if (debug_sprite_spawned == 0u)
	{
		allocate_object_outside_loop(&free_count, &object);
		if ((uint8_t)free_count != 0u)
		{
			OBJECT(object)->handler = debug_sprite_mover_update_ADDRESS;
			debug_sprite_spawned++;
		}
	}
}

void debug_sprite_mover_update(void)
{
	uint32_t attribute = 0, text_position = 0, value = 0;

	OBJECT(current_object)->screen_x = debug_sprite_x;
	OBJECT(current_object)->screen_y = debug_sprite_y;
	OBJECT(current_object)->sprite_layer = 0x32u;
	OBJECT(current_object)->sprite_frame = 0x8DB8u;
	OBJECT(current_object)->sprite_palette = 0x130u;
	OBJECT(current_object)->sprite_flags = 0u;
	OBJECT(current_object)->sprite_scale = 0x100u;
	queue_sprite_in_layer();
	if ((input_ports_active & 0x10000u) != 0u)
		debug_sprite_x++;
	if ((input_ports_active & 0x10u) != 0u)
		debug_sprite_x--;
	if ((input_ports_active & 0x1000000u) != 0u)
		debug_sprite_y++;
	if ((input_ports_active & 0x2000000u) != 0u)
		debug_sprite_y--;
	value = debug_sprite_x;
	text_position = TEXT_CELL(3, 22);
	attribute = 0x200u;
	draw_hex_4(&value, &attribute, &text_position);
	value = debug_sprite_y;
	text_position = TEXT_CELL(8, 22);
	attribute = 0x200u;
	draw_hex_4(&value, &attribute, &text_position);
}

void start_stage_view(uint32_t *p_direction_y, uint32_t *p_direction_x, uint32_t *p_count, uint32_t *p_stage_offset)
{
	uint32_t direction_y = *p_direction_y;
	uint32_t direction_x = *p_direction_x;
	uint32_t count = *p_count;
	uint32_t stage_offset = *p_stage_offset;

	if (stage != 0x11u)
	{
		unused_scroll_flag = 1u;
		init_stage_view(&direction_y, &direction_x, &count, &stage_offset);
		*p_direction_y = direction_y;
		*p_direction_x = direction_x;
		*p_count = count;
		*p_stage_offset = stage_offset;
		return;
	}
	ending_scene_step = 0u;
}

void call_run_map_screen(uint32_t object)
{
	run_map_screen(0u, object);
}

void run_stage_05_switch(uint32_t speed, uint32_t *p_scroll_y, uint32_t *p_speed_value, uint32_t *p_dest)
{
	uint32_t scroll_y = *p_scroll_y;
	uint32_t speed_value = *p_speed_value;
	uint32_t dest = *p_dest;
	uint32_t r1 = 0, camera_view_y = 0, ignored = 0, layer_scroll_x = 0, next_scroll_position = 0, script = 0;

	r1 = (uint8_t)stage_05_switch_step * 3u;
	switch ((uint8_t)stage_05_switch_step)
	{
	case 0:
	case 4:
		mixer_text_blend |= 0x4000u;
		mixer_nbg0_blend |= 0x4000u;
		mixer_nbg1_blend |= 0x4000u;
		mixer_nbg2_blend |= 0x4000u;
		mixer_nbg3_blend |= 0x4000u;
		mixer_bitmap_blend |= 0x4000u;
		mixer_sprite_control |= 0x8000u;
		stage_05_switch_step++;
		return;
	case 1:
		fade_kind = 0;
		update_fade(r1);
		if (fade_kind >= 0)
			return;
		stage_05_switch_step++;
		fade_step = 0u;
		return;
	case 2:
		mem_write8(video_layer_disable, 0x3Fu);
		script = stage_00_coast_script_ADDRESS;
		run_vram_script(&r1, &script, &dest);
		script = stage_00_horizon_script_ADDRESS;
		run_vram_script(&r1, &script, &dest);
		script = blue_sky_script_ADDRESS;
		run_vram_script(&r1, &script, &dest);
		script = aerial_city_script_ADDRESS;
		run_vram_script(&r1, &script, &dest);
		script = stage_00_tower_top_page_1_script_ADDRESS;
		run_vram_script(&r1, &script, &dest);
		script = stage_00_tower_top_page_3_script_ADDRESS;
		run_vram_script(&r1, &script, &dest);
		script = stage_00_tower_top_page_0_script_ADDRESS;
		run_vram_script(&r1, &script, &dest);
		script = stage_00_tower_top_page_2_script_ADDRESS;
		run_vram_script(&r1, &script, &dest);
		nbg3_pages = 0x7070707u;
		camera_x = 0xF800000;
		camera_y = 0x6700000;
		camera_view_y = update_camera_view();
		layer_scroll_x = (camera_view_y & ~0xFFFFu) | camera_x_pixels;
		nbg0_scroll_y_int = camera_y_pixels;
		nbg0_scroll_x_int = layer_scroll_x;
		nbg0_scroll_x_int &= 0x3FFu;
		nbg0_scroll_y_int &= 0x1FFu;
		stage = 0u;
		update_background_scroll(&ignored, &layer_scroll_x, &speed, &speed_value, &next_scroll_position, &ignored, &dest);
		stage = 5u;
		mem_write8(video_layer_disable, mem_read8(video_layer_disable) & 0xE6u);
		stage_05_switch_step++;
		*p_scroll_y = layer_scroll_x;
		*p_speed_value = speed_value;
		*p_dest = dest;
		return;
	case 3:
		fade_kind = 2;
		update_fade(r1);
		if (fade_kind >= 0)
			return;
		stage_05_switch_step = -1;
		scroll_position = 0x5E5Au;
		stage = 0u;
		mixer_text_blend &= 0xBFFFu;
		mixer_nbg0_blend &= 0xBFFFu;
		mixer_nbg1_blend &= 0xBFFFu;
		mixer_nbg2_blend &= 0xBFFFu;
		mixer_nbg3_blend &= 0xBFFFu;
		mixer_bitmap_blend &= 0xBFFFu;
		mixer_sprite_control &= ~0x8000u;
		set_nbg3_page_5_flags(&ignored, &dest);
		loaded_tile_sets = 0x32A03u;
		*p_dest = dest;
		return;
	default:
		unknown_code(0x16D16u + (uint8_t)stage_05_switch_step * 3u);
	}
}

uint32_t update_fade(uint32_t delta)
{
	uint32_t jump_offset = 0;

	jump_offset = fade_step * 3u;
	switch (fade_step)
	{
	case 0:
	case 3:
		fade_kind &= 3u;
		switch ((jump_offset & ~0xFFu) | (uint8_t)fade_kind)
		{
		case 0:
		case 4:
			color_offset_red = 0x3Fu;
			color_offset_blue = 0x3Fu;
			color_offset_green = 0x3Fu;
			delta = (delta & ~0xFFFFu) | fade_speed;
			delta = (delta & ~0xFFFFu) | (uint16_t)(0u - delta);
			fade_delta = delta;
			break;
		case 1:
			color_offset_red = 0x1Fu;
			color_offset_blue = 0x1Fu;
			color_offset_green = 0x1Fu;
			delta = (delta & ~0xFFFFu) | fade_speed;
			delta = (delta & ~0xFFFFu) | (uint16_t)(0u - delta);
			fade_delta = delta;
			break;
		case 2:
			color_offset_red = 0x20u;
			color_offset_blue = 0x20u;
			color_offset_green = 0x20u;
			delta = (delta & ~0xFFFFu) | fade_speed;
			fade_delta = delta;
			break;
		case 3:
			color_offset_red = 0u;
			color_offset_blue = 0u;
			color_offset_green = 0u;
			delta = (delta & ~0xFFFFu) | fade_speed;
			fade_delta = delta;
			break;
		default:
			unknown_code(0x16EF2u + ((jump_offset & ~0xFFu) | (uint8_t)fade_kind) * 3u);
		}
		AS16(fade_delay) = 1u;
		fade_step++;
		return delta;
	case 1:
		AS16(fade_delay)--;
		if (AS16(fade_delay) != 0u)
			return delta;
		AS16(fade_delay) = 1u;
		color_offset_red += fade_delta;
		color_offset_blue += fade_delta;
		color_offset_green += fade_delta;
		switch ((jump_offset & ~0xFFu) | (uint8_t)fade_kind)
		{
		case 0:
		case 4:
			if ((int16_t)color_offset_red <= 0x20)
			{
				fade_step++;
				color_offset_red = 0x20u;
				color_offset_blue = 0x20u;
				color_offset_green = 0x20u;
			}
			break;
		case 1:
			if ((int16_t)color_offset_red <= 0)
			{
				fade_step++;
				color_offset_red = 0u;
				color_offset_blue = 0u;
				color_offset_green = 0u;
			}
			break;
		case 2:
			if ((int16_t)color_offset_red >= 0x3F)
			{
				fade_step++;
				color_offset_red = 0x3Fu;
				color_offset_blue = 0x3Fu;
				color_offset_green = 0x3Fu;
			}
			break;
		case 3:
			if ((int16_t)color_offset_red >= 0x1F)
			{
				fade_step++;
				color_offset_red = 0x1Fu;
				color_offset_blue = 0x1Fu;
				color_offset_green = 0x1Fu;
			}
			break;
		default:
			unknown_code(0x16FD3u + ((jump_offset & ~0xFFu) | (uint8_t)fade_kind) * 3u);
		}
		return delta;
	case 2:
		fade_kind = -1;
		return delta;
	default:
		unknown_code(0x16ED6u + fade_step * 3u);
	}
}

void update_stage_00_exit_scene(uint32_t dest, uint32_t *p_speed_value, uint32_t *p_script)
{
	uint32_t speed_value = *p_speed_value;
	uint32_t script = *p_script;
	uint16_t layer_zoom = 0;
	uint32_t r10 = 0, ignored = 0, layer_scroll_x = 0, next_scroll_position = 0, scroll_y_past_trigger = 0, speed = 0;
	uint16_t scroll_y_offset = 0;

	if ((int8_t)stage_00_exit_step < 2)
	{
		if (stage_00_exit_step == 0u)
		{
			scroll_speed = 0x35u;
			scroll_y_past_trigger = (uint16_t)(nbg0_scroll_y_int - 0x20u);
			if ((int16_t)scroll_y_past_trigger < 8 && (int16_t)scroll_y_past_trigger >= 0)
			{
				script = stage_00_exit_tower_wall_script_ADDRESS;
				run_vram_script(&scroll_y_past_trigger, &script, &dest);
				script = clear_tile_page_03_script_ADDRESS;
				run_vram_script(&scroll_y_past_trigger, &script, &dest);
				stage_00_exit_step++;
			}
		}
		if ((int8_t)stage_00_exit_step < 2)
		{
			scroll_y_offset = nbg0_scroll_y_int - 0xF6u;
			scroll_y_past_trigger = scroll_y_offset;
			if ((int16_t)scroll_y_offset >= 0 && (int16_t)scroll_y_past_trigger < 8)
			{
				nbg0_pages = 0x2020202u;
				script = stage_00_exit_beam_script_ADDRESS;
				run_vram_script(&scroll_y_past_trigger, &script, &dest);
				script = stage_00_exit_beam_end_script_ADDRESS;
				run_vram_script(&scroll_y_past_trigger, &script, &dest);
				mixer_text_blend &= 0xBFFFu;
				mixer_nbg0_blend |= 0x4000u;
				mixer_nbg1_blend |= 0x4000u;
				mixer_nbg2_blend |= 0x4000u;
				mixer_nbg3_blend |= 0x4000u;
				mixer_bitmap_blend &= 0xBFFFu;
				mixer_sprite_control |= 0x8000u;
				color_offset_red = 0u;
				color_offset_blue = 0u;
				color_offset_green = 0u;
				stage_00_exit_step++;
			}
		}
	}
	switch (stage_00_exit_step)
	{
	case 0:
	case 1:
	case 2:
	case 11:
		if (stage_00_exit_timer >= 0x8C)
		{
			nbg0_pages = 0x3020100u;
			stage_00_exit_step++;
			*p_script = script;
			return;
		}
		stage_00_exit_timer++;
		speed = scroll_speed;
		section_limit_y = -1;
		section_direction_y = -0x4000;
		section_direction_x = 0;
		advance_camera_y(speed);
		update_camera_view();
		layer_scroll_x = camera_x_pixels;
		nbg0_scroll_y_int = camera_y_pixels;
		nbg0_scroll_x_int = layer_scroll_x;
		nbg0_scroll_x_int &= 0x3FFu;
		nbg0_scroll_y_int &= 0x1FFu;
		update_background_scroll(&ignored, &layer_scroll_x, &speed, &speed_value, &next_scroll_position, &r10, &dest);
		*p_speed_value = speed_value;
		*p_script = r10;
		return;
	case 3:
		if (stage_00_exit_timer >= 0xA0)
		{
			nbg0_pages = 0x3030100u;
			stage_00_exit_step++;
			*p_script = script;
			return;
		}
		stage_00_exit_timer++;
		speed = scroll_speed;
		section_limit_y = -1;
		section_direction_y = -0x4000;
		section_direction_x = 0;
		advance_camera_y(speed);
		update_camera_view();
		layer_scroll_x = camera_x_pixels;
		nbg0_scroll_y_int = camera_y_pixels;
		nbg0_scroll_x_int = layer_scroll_x;
		nbg0_scroll_x_int &= 0x3FFu;
		nbg0_scroll_y_int &= 0x1FFu;
		update_background_scroll(&ignored, &layer_scroll_x, &speed, &speed_value, &next_scroll_position, &r10, &dest);
		*p_speed_value = speed_value;
		*p_script = r10;
		return;
	case 4:
		if (stage_00_exit_timer >= 0xAF)
		{
			nbg0_pages = 0x3030303u;
			stage_00_exit_step++;
			*p_script = script;
			return;
		}
		stage_00_exit_timer++;
		speed = scroll_speed;
		section_limit_y = -1;
		section_direction_y = -0x4000;
		section_direction_x = 0;
		advance_camera_y(speed);
		update_camera_view();
		layer_scroll_x = camera_x_pixels;
		nbg0_scroll_y_int = camera_y_pixels;
		nbg0_scroll_x_int = layer_scroll_x;
		nbg0_scroll_x_int &= 0x3FFu;
		nbg0_scroll_y_int &= 0x1FFu;
		update_background_scroll(&ignored, &layer_scroll_x, &speed, &speed_value, &next_scroll_position, &r10, &dest);
		*p_speed_value = speed_value;
		*p_script = r10;
		return;
	case 5:
		scroll_speed--;
		if ((int16_t)scroll_speed <= 2)
			scroll_speed = 2u;
		if (stage_00_exit_timer >= 0xD2)
		{
			stage_00_exit_step++;
			*p_script = script;
			return;
		}
		stage_00_exit_timer++;
		speed = scroll_speed;
		section_limit_y = -1;
		section_direction_y = -0x4000;
		section_direction_x = 0;
		advance_camera_y(speed);
		update_camera_view();
		layer_scroll_x = camera_x_pixels;
		nbg0_scroll_y_int = camera_y_pixels;
		nbg0_scroll_x_int = layer_scroll_x;
		nbg0_scroll_x_int &= 0x3FFu;
		nbg0_scroll_y_int &= 0x1FFu;
		update_background_scroll(&ignored, &layer_scroll_x, &speed, &speed_value, &next_scroll_position, &r10, &dest);
		*p_speed_value = speed_value;
		*p_script = r10;
		return;
	case 6:
		clamp_colour_offsets();
		stage_00_exit_delay++;
		if (stage_00_exit_delay != 5u)
		{
			stage_00_exit_timer++;
			speed = scroll_speed;
			section_limit_y = -1;
			section_direction_y = -0x4000;
			section_direction_x = 0;
			advance_camera_y(speed);
			update_camera_view();
			layer_scroll_x = camera_x_pixels;
			nbg0_scroll_y_int = camera_y_pixels;
			nbg0_scroll_x_int = layer_scroll_x;
			nbg0_scroll_x_int &= 0x3FFu;
			nbg0_scroll_y_int &= 0x1FFu;
			update_background_scroll(&ignored, &layer_scroll_x, &speed, &speed_value, &next_scroll_position, &r10, &dest);
			*p_speed_value = speed_value;
			*p_script = r10;
			return;
		}
		stage_00_exit_delay = 0u;
		color_offset_red++;
		color_offset_blue++;
		color_offset_green++;
		scroll_speed--;
		if ((int16_t)scroll_speed <= 2)
			scroll_speed = 2u;
		if ((int16_t)color_offset_red < 0x1F)
		{
			stage_00_exit_timer++;
			speed = scroll_speed;
			section_limit_y = -1;
			section_direction_y = -0x4000;
			section_direction_x = 0;
			advance_camera_y(speed);
			update_camera_view();
			layer_scroll_x = camera_x_pixels;
			nbg0_scroll_y_int = camera_y_pixels;
			nbg0_scroll_x_int = layer_scroll_x;
			nbg0_scroll_x_int &= 0x3FFu;
			nbg0_scroll_y_int &= 0x1FFu;
			update_background_scroll(&ignored, &layer_scroll_x, &speed, &speed_value, &next_scroll_position, &r10, &dest);
			*p_speed_value = speed_value;
			*p_script = r10;
			return;
		}
		nbg0_scroll_y_int = 0x58u;
		nbg0_scroll_x_int = 0xB8u;
		nbg1_scroll_y_int = 0x58u;
		nbg1_scroll_x_int = 0xB8u;
		nbg0_center_x = 0xA0u;
		nbg0_center_y = 0x90u;
		nbg1_center_x = 0xA0u;
		nbg1_center_y = 0x90u;
		mem_write32(video_layer_disable, mem_read32(video_layer_disable) | 1u);
		mem_write32(video_layer_disable, mem_read32(video_layer_disable) | 2u);
		stage_00_exit_step++;
		*p_script = script;
		return;
	case 7:
		nbg0_pages = 0x6060505u;
		video_layer_opaque_shadow = 0xBu;
		stage_00_exit_step++;
		*p_script = script;
		return;
	case 8:
		mem_write32(video_layer_disable, mem_read32(video_layer_disable) & ~1u);
		mem_write32(video_layer_disable, mem_read32(video_layer_disable) & ~2u);
		mem_write32(video_layer_disable, mem_read32(video_layer_disable) | 8u);
		mem_write32(video_control, mem_read32(video_control) | 0x1000u);
		mem_write32(video_control, mem_read32(video_control) | 0x2000u);
		mem_write32(video_control, mem_read32(video_control) | 0x4000u);
		mem_write16(nbg0_zoom_x, 0x100u);
		mem_write16(nbg0_zoom_y, 0x100u);
		mem_write16(nbg1_zoom_x, 0x100u);
		mem_write16(nbg1_zoom_y, 0x100u);
		script = stage_00_exit_zoom_page_0_script_ADDRESS;
		run_vram_script(&scroll_y_past_trigger, &script, &dest);
		script = stage_00_exit_zoom_page_1_script_ADDRESS;
		run_vram_script(&scroll_y_past_trigger, &script, &dest);
		script = stage_00_exit_zoom_page_2_script_ADDRESS;
		run_vram_script(&scroll_y_past_trigger, &script, &dest);
		mem_write16(backdrop_color, 0x1356u);
		nbg0_pages = 0x2020200u;
		nbg1_pages = 0x2020201u;
		stage_00_exit_step++;
		stage_00_exit_zoom = 0x200u;
		*p_script = script;
		return;
	case 9:
		stage_00_exit_zoom--;
		layer_zoom = (uint16_t)(divide_s32(0x2000000u, stage_00_exit_zoom) >> 8);
		mem_write16(nbg0_zoom_x, layer_zoom);
		mem_write16(nbg0_zoom_y, layer_zoom);
		mem_write16(nbg1_zoom_x, layer_zoom);
		mem_write16(nbg1_zoom_y, layer_zoom);
		clamp_colour_offsets();
		stage_00_exit_delay++;
		if (stage_00_exit_delay != 3u)
		{
			update_background_scroll(&ignored, &layer_scroll_x, &speed, &speed_value, &next_scroll_position, &r10, &dest);
			*p_speed_value = speed_value;
			*p_script = r10;
			return;
		}
		stage_00_exit_delay = 0u;
		color_offset_red--;
		color_offset_blue--;
		color_offset_green--;
		if (color_offset_red != 0u)
		{
			update_background_scroll(&ignored, &layer_scroll_x, &speed, &speed_value, &next_scroll_position, &r10, &dest);
			*p_speed_value = speed_value;
			*p_script = r10;
			return;
		}
		stage_00_exit_step++;
		*p_script = script;
		return;
	case 10:
		update_background_scroll(&ignored, &layer_scroll_x, &speed, &speed_value, &next_scroll_position, &r10, &dest);
		if ((int16_t)stage_00_exit_zoom < 0x42)
			stage_00_exit_zoom = 0x42u;
		stage_00_exit_zoom--;
		layer_zoom = (uint16_t)(divide_s32(0x2000000u, stage_00_exit_zoom) >> 8);
		mem_write16(nbg0_zoom_x, layer_zoom);
		mem_write16(nbg0_zoom_y, layer_zoom);
		mem_write16(nbg1_zoom_x, layer_zoom);
		mem_write16(nbg1_zoom_y, layer_zoom);
		*p_speed_value = speed_value;
		*p_script = r10;
		return;
	default:
		unknown_code(0x17160u + stage_00_exit_step * 3u);
	}
}

void clamp_colour_offsets(void)
{
	color_offset_red &= 0x1Fu;
	color_offset_blue &= 0x1Fu;
	color_offset_green &= 0x1Fu;
}

uint32_t update_camera_with_shake(void)
{
	uint32_t r2 = 0, shaken_y = 0, wave_table = 0;

	r2 = update_camera_view();
	if (camera_shake_active == 0u)
		return r2;
	switch (camera_shake_phase)
	{
	case 0:
	case 2:
		camera_shake_phase++;
		break;
	case 1:
		break;
	default:
		unknown_code(0x17610u + camera_shake_phase * 3u);
	}
	camera_shake_base_y = (uint32_t)(int16_t)camera_y_pixels << 16;
	camera_shake_frame++;
	wave_table = 0x100000u | (uint16_t)(0x1632u + camera_shake_wave);
	shaken_y = (int16_t)mem_read16((wave_table & ~0xFFFFu) | (uint16_t)(wave_table + camera_shake_step));
	camera_shake_step += 0xAu;
	shaken_y = shaken_y * 0x20u + camera_shake_base_y;
	if ((camera_shake_frame & 1u) != 0u)
		shaken_y = camera_shake_base_y;
	r2 = shaken_y;
	r2 = update_camera_view_y(r2);
	r2 = set_nbg0_scroll_from_camera(r2);
	if ((int16_t)camera_shake_length <= (int16_t)camera_shake_step)
	{
		camera_shake_active = 0u;
		AS16(camera_shake_frame) = 0u;
		camera_shake_step = 0u;
		camera_shake_phase = 0u;
	}
	return r2;
}

uint32_t set_nbg0_scroll_from_camera(uint32_t scroll_x)
{
	uint16_t layer_scroll_y = 0;
	uint16_t block_column = 0, block_row = 0;

	layer_scroll_y = camera_y_pixels;
	scroll_x = (scroll_x & ~0xFFFFu) | camera_x_pixels;
	nbg0_scroll_y_int = layer_scroll_y;
	nbg0_scroll_x_int = scroll_x;
	tile_block_row = layer_scroll_y;
	tile_block_column = scroll_x;
	tile_block_column &= ~0x1FFu;
	tile_block_row &= ~0xFFu;
	nbg0_scroll_x_int &= 0x3FFu;
	nbg0_scroll_y_int &= 0x1FFu;
	block_column = tile_block_column;
	tile_block_column = ((uint32_t)block_column << 7) | (block_column >> 9);
	block_row = tile_block_row;
	tile_block_row = ((uint32_t)block_row << 8) | (block_row >> 8);
	tile_block_row = (uint32_t)tile_block_row * stage_block_map_width;
	return scroll_x;
}

uint32_t update_camera_view(void)
{
	uint32_t y = 0;

	camera_x_pixels = (uint32_t)camera_x >> 16;
	view_x = camera_x_pixels - view_offset_x;
	y = camera_y;
	return update_camera_view_y(y);
}

uint32_t update_camera_view_y(uint32_t y)
{
	y = y >> 16;
	camera_y_pixels = y;
	y = camera_y_pixels;
	y = (uint16_t)(y - view_offset_y);
	view_y = y;
	return y;
}

uint32_t move_camera_along_path(uint32_t speed)
{
	uint16_t next_offset = 0;
	uint32_t current_position = 0, limit_list = 0;

	camera_limits_reached = 0u;
	if ((uint32_t)section_direction_y == 0u)
		camera_limits_reached |= 4u;
	else
	{
		next_offset = (uint16_t)(((int32_t)(section_direction_y * speed + camera_y) >> 16) - camera_origin_y);
		limit_list = section_limit_y_list;
		if ((int16_t)ROM16(limit_list - 4u) < (int16_t)ROM16(limit_list - 2u))
		{
			if (section_limit_y <= (int16_t)next_offset)
			{
				camera_limits_reached |= 4u;
				if ((uint32_t)section_direction_x == 0u)
					camera_limits_reached |= 2u;
				else
				{
					next_offset = (uint16_t)(((int32_t)(section_direction_x * speed + camera_x) >> 16) - camera_origin_x);
					limit_list = section_limit_x_list;
					if ((int16_t)ROM16(limit_list - 4u) < (int16_t)ROM16(limit_list - 2u))
					{
						if (section_limit_x <= (int16_t)next_offset)
						{
							camera_limits_reached |= 2u;
							current_position = scroll_position;
							current_position = (uint16_t)(current_position + speed);
							if ((int32_t)ROM16(section_end_list) < (int32_t)current_position)
								camera_limits_reached |= 1u;
							switch (camera_limits_reached)
							{
							case 0:
								advance_camera_scroll_x_y(speed);
								break;
							case 1:
								advance_camera_x_y(speed);
								break;
							case 2:
								advance_camera_y_scroll(speed);
								break;
							case 3:
								advance_camera_y(speed);
								break;
							case 4:
								advance_camera_x_scroll(speed);
								break;
							case 5:
								advance_camera_x(speed);
								break;
							case 6:
								advance_scroll_position((uint16_t)speed);
								break;
							case 7:
								snap_camera_to_section_end();
								break;
							case 8:
								advance_camera_scroll_x_y(speed);
								break;
							default:
								unknown_code(0x177FDu + camera_limits_reached * 3u);
							}
							return current_position;
						}
					}
					else if ((int16_t)next_offset <= section_limit_x)
						camera_limits_reached |= 2u;
				}
				current_position = scroll_position;
				current_position = (uint16_t)(current_position + speed);
				if ((int32_t)ROM16(section_end_list) < (int32_t)current_position)
					camera_limits_reached |= 1u;
				switch (camera_limits_reached)
				{
				case 0:
					advance_camera_scroll_x_y(speed);
					break;
				case 1:
					advance_camera_x_y(speed);
					break;
				case 2:
					advance_camera_y_scroll(speed);
					break;
				case 3:
					advance_camera_y(speed);
					break;
				case 4:
					advance_camera_x_scroll(speed);
					break;
				case 5:
					advance_camera_x(speed);
					break;
				case 6:
					advance_scroll_position((uint16_t)speed);
					break;
				case 7:
					snap_camera_to_section_end();
					break;
				case 8:
					advance_camera_scroll_x_y(speed);
					break;
				default:
					unknown_code(0x177FDu + camera_limits_reached * 3u);
				}
				return current_position;
			}
		}
		else if ((int16_t)next_offset <= section_limit_y)
			camera_limits_reached |= 4u;
	}
	if ((uint32_t)section_direction_x == 0u)
		camera_limits_reached |= 2u;
	else
	{
		next_offset = (uint16_t)(((int32_t)(section_direction_x * speed + camera_x) >> 16) - camera_origin_x);
		limit_list = section_limit_x_list;
		if ((int16_t)ROM16(limit_list - 4u) < (int16_t)ROM16(limit_list - 2u))
		{
			if (section_limit_x <= (int16_t)next_offset)
			{
				camera_limits_reached |= 2u;
				current_position = scroll_position;
				current_position = (uint16_t)(current_position + speed);
				if ((int32_t)ROM16(section_end_list) < (int32_t)current_position)
					camera_limits_reached |= 1u;
				switch (camera_limits_reached)
				{
				case 0:
					advance_camera_scroll_x_y(speed);
					break;
				case 1:
					advance_camera_x_y(speed);
					break;
				case 2:
					advance_camera_y_scroll(speed);
					break;
				case 3:
					advance_camera_y(speed);
					break;
				case 4:
					advance_camera_x_scroll(speed);
					break;
				case 5:
					advance_camera_x(speed);
					break;
				case 6:
					advance_scroll_position((uint16_t)speed);
					break;
				case 7:
					snap_camera_to_section_end();
					break;
				case 8:
					advance_camera_scroll_x_y(speed);
					break;
				default:
					unknown_code(0x177FDu + camera_limits_reached * 3u);
				}
				return current_position;
			}
		}
		else if ((int16_t)next_offset <= section_limit_x)
			camera_limits_reached |= 2u;
	}
	current_position = scroll_position;
	current_position = (uint16_t)(current_position + speed);
	if ((int32_t)ROM16(section_end_list) < (int32_t)current_position)
		camera_limits_reached |= 1u;
	switch (camera_limits_reached)
	{
	case 0:
		advance_camera_scroll_x_y(speed);
		break;
	case 1:
		advance_camera_x_y(speed);
		break;
	case 2:
		advance_camera_y_scroll(speed);
		break;
	case 3:
		advance_camera_y(speed);
		break;
	case 4:
		advance_camera_x_scroll(speed);
		break;
	case 5:
		advance_camera_x(speed);
		break;
	case 6:
		advance_scroll_position((uint16_t)speed);
		break;
	case 7:
		snap_camera_to_section_end();
		break;
	case 8:
		advance_camera_scroll_x_y(speed);
		break;
	default:
		unknown_code(0x177FDu + camera_limits_reached * 3u);
	}
	return current_position;
}

void advance_camera_scroll_x_y(uint32_t speed)
{
	advance_scroll_position((uint16_t)speed);
	advance_camera_x(speed);
	advance_camera_y(speed);
}

void advance_camera_x_y(uint32_t speed)
{
	advance_camera_x(speed);
	advance_camera_y(speed);
}

void advance_camera_y_scroll(uint32_t speed)
{
	advance_camera_y(speed);
	advance_scroll_position((uint16_t)speed);
}

void advance_camera_y(uint32_t speed)
{
	camera_y += section_direction_y * speed;
}

void advance_camera_x_scroll(uint32_t speed)
{
	advance_camera_x(speed);
	advance_scroll_position((uint16_t)speed);
}

void advance_camera_x(uint32_t speed)
{
	camera_x += section_direction_x * speed;
}

void advance_scroll_position(uint16_t speed)
{
	scroll_position += speed;
}

void snap_camera_to_section_end(void)
{
	uint32_t section_end_y = 0;

	section_end_y = (uint32_t)(uint16_t)(ROM16(section_limit_y_list - 2u) + camera_origin_y) << 16;
	camera_x = (uint32_t)(uint16_t)(ROM16(section_limit_x_list - 2u) + camera_origin_x) << 16;
	camera_y = section_end_y;
	scroll_position = ROM16(section_end_list);
}

void update_stage_06_background(uint32_t random_value, uint32_t dest, uint32_t *p_rolls_left, uint32_t *p_words_left, uint32_t *p_source)
{
	uint32_t rolls_left = *p_rolls_left;
	uint32_t words_left = *p_words_left;
	uint32_t source = *p_source;
	uint8_t sound_id = 0;
	uint16_t shake_y = 0;
	uint32_t r1 = 0, cycle_set = 0, ignored = 0, palette_stage = 0, script = 0, write_index_address = 0;
	uint8_t flipped_sign = 0;
	uint16_t previous_shake = 0;

	switch (stage_06_background_step)
	{
	case 0:
	case 8:
		mixer_blend &= 0x1F00u;
		mixer_blend |= 0xA4u;
		camera_x = 0;
		camera_y = 0;
		update_camera_view();
		stage_06_background_step++;
		return;
	case 1:
		if (mixer_blend != 0xA4u)
		{
			mixer_blend = 0xA4u;
			return;
		}
		load_default_palettes();
		palette_stage = 1u;
		load_palette_set_and_backdrop(palette_stage, &ignored, &source, &dest);
		if (mem_read16(palette_color_61D0) != 0x7B0u)
		{
			mixer_blend = 0xA4u;
			*p_source = source;
			return;
		}
		camera_x = 0;
		camera_y = -0x600000;
		update_camera_view();
		nbg3_scroll_x = 0u;
		nbg3_scroll_y = 0u;
		nbg1_scroll_x = 0u;
		nbg1_scroll_y = 0u;
		nbg0_scroll_x = 0u;
		nbg0_scroll_y = 0xFFA00000u;
		mem_write8(video_layer_disable, mem_read8(video_layer_disable) | 0x3Fu);
		script = cloudy_horizon_script_ADDRESS;
		run_vram_script(&palette_stage, &script, &dest);
		script = stage_06_forest_script_ADDRESS;
		run_vram_script(&palette_stage, &script, &dest);
		nbg3_pages = 0u;
		nbg0_pages = 0x5050505u;
		mem_write8(video_layer_disable, mem_read8(video_layer_disable) & 0x26u);
		stage_06_background_step++;
		*p_source = script;
		return;
	case 2:
		nbg0_scroll_x -= 0x2000u;
		nbg1_scroll_x -= 0x1000u;
		AS32(nbg3_scroll_x) = (int32_t)nbg1_scroll_x >> 16;
		return;
	case 3:
		mem_write8(video_layer_disable, mem_read8(video_layer_disable) | 0x3Fu);
		mixer_blend &= 0x1F00u;
		script = clear_tile_page_00_script_ADDRESS;
		run_vram_script(&r1, &script, &dest);
		script = stage_06_upper_jaw_script_ADDRESS;
		run_vram_script(&r1, &script, &dest);
		script = clear_tile_page_0a_script_ADDRESS;
		run_vram_script(&r1, &script, &dest);
		script = stage_06_lower_jaw_script_ADDRESS;
		run_vram_script(&r1, &script, &dest);
		nbg0_pages = 1u;
		nbg3_pages = 6u;
		mem_write8(video_layer_disable, mem_read8(video_layer_disable) & 0x26u);
		nbg0_scroll_x_int = 0u;
		nbg0_scroll_y_int = 0x90u;
		nbg3_scroll_x = 0u;
		nbg3_scroll_y = 0xFF70u;
		mem_write16(mixer_nbg0, mixer_default_nbg0 | 0xFu);
		mem_write16(mixer_nbg3, mixer_default_nbg3 | 0xFu);
		stage_06_background_step++;
		*p_source = script;
		return;
	case 4:
		nbg0_scroll_y_int -= 0x10u;
		if ((int16_t)nbg0_scroll_y_int <= 0)
			nbg0_scroll_y_int = 0u;
		nbg3_scroll_y += 0x10u;
		if ((int16_t)nbg3_scroll_y >= 0)
			nbg3_scroll_y = 0u;
		if ((nbg0_scroll_y_int | nbg3_scroll_y) != 0u)
			return;
		stage_06_background_step = 7u;
		stage_06_shake = 0x800u;
		stage_06_shake_sign = 0;
		sound_id = 0x1Eu;
		play_sound(sound_id, &r1, &write_index_address);
		return;
	case 5:
		mixer_blend &= 0x1F00u;
		mixer_blend |= 0xA4u;
		palette_cycle_set = 0x209090u;
		cycle_set = palette_cycle_set;
		mem_write32(cycle_set + 4u, PALETTE_RAM + 0x83A0u);
		mem_write32(cycle_set, stage_06_cycle_palettes_0_ADDRESS);
		mem_write8(cycle_set + 8u, 2u);
		mem_write8(cycle_set + 0xAu, 2u);
		mem_write8(cycle_set + 9u, 3u);
		mem_write8(cycle_set + 0xBu, 3u);
		mem_write8(cycle_set + 0xCu, 0u);
		mem_write8(cycle_set + 0xDu, 6u);
		cycle_set += 0x10u;
		mem_write32(cycle_set + 4u, PALETTE_RAM + 0x8400u);
		mem_write32(cycle_set, stage_06_cycle_palettes_1_ADDRESS);
		mem_write8(cycle_set + 8u, 7u);
		mem_write8(cycle_set + 0xAu, 7u);
		mem_write8(cycle_set + 9u, 2u);
		mem_write8(cycle_set + 0xBu, 2u);
		mem_write8(cycle_set + 0xCu, 0u);
		mem_write8(cycle_set + 0xDu, 6u);
		camera_x = 0;
		camera_y = 0;
		update_camera_view();
		nbg3_scroll_x = 0u;
		nbg3_scroll_y = 0u;
		nbg0_scroll_x_int = 0u;
		nbg0_scroll_y_int = 0u;
		mem_write8(video_layer_disable, mem_read8(video_layer_disable) | 0x3Fu);
		script = cloudy_horizon_script_ADDRESS;
		run_vram_script(&r1, &script, &dest);
		script = stage_06_island_tower_script_ADDRESS;
		run_vram_script(&r1, &script, &dest);
		nbg3_pages = 0x3020100u;
		nbg0_pages = 0x8070605u;
		mem_write8(video_layer_disable, mem_read8(video_layer_disable) & 0x26u);
		mem_write16(mixer_nbg0, mixer_default_nbg0);
		mem_write16(mixer_nbg3, mixer_default_nbg3);
		stage_06_background_step++;
		*p_source = script;
		return;
	case 6:
		if (palette_cycle_set_0_frame != 0u)
		{
			source = cycle_palette_set_0(source);
			source = cycle_palette_set_1(source);
			*p_rolls_left = rolls_left;
			*p_words_left = words_left;
			*p_source = source;
			return;
		}
		if (palette_cycle_set_1_frame == 0u)
		{
			rolls_left = 2u;
			do
			{
				next_random(&random_value, &words_left, &source);
				random_value &= 0xFu;
				if ((int8_t)random_value > 2)
				{
					*p_rolls_left = rolls_left;
					*p_words_left = words_left;
					*p_source = source;
					return;
				}
				rolls_left--;
			} while (rolls_left != 0u);
			source = cycle_palette_set_0(source);
		}
		source = cycle_palette_set_1(source);
		*p_rolls_left = rolls_left;
		*p_words_left = words_left;
		*p_source = source;
		return;
	case 7:
		if (stage_06_shake == 0u)
			return;
		previous_shake = stage_06_shake;
		stage_06_shake = previous_shake - 0x80u;
		if ((int16_t)previous_shake < 0x80)
			stage_06_shake = 0u;
		shake_y = AT8(stage_06_shake, 1);
		flipped_sign = ~stage_06_shake_sign;
		stage_06_shake_sign = flipped_sign;
		if (flipped_sign != 0u)
			shake_y = (uint16_t)(0u - shake_y);
		nbg0_scroll_y_int = shake_y;
		nbg3_scroll_y = shake_y;
		return;
	default:
		unknown_code(0x178B8u + stage_06_background_step * 3u);
	}
}

void escape_alarm_flash(void)
{
	uint8_t sound_id = 0;
	uint32_t write_index = 0, write_index_address = 0;

	if (escape_seconds == 0u)
	{
		delete_current_object();
		return;
	}
	if (OBJECT(current_object)->escape_alarm_flash.started == 0u)
	{
		mixer_nbg0_blend |= 0x4000u;
		mixer_nbg1_blend |= 0x4000u;
		mixer_nbg2_blend |= 0x4000u;
		mixer_nbg3_blend |= 0x4000u;
		mixer_bitmap_blend |= 0x4000u;
		escape_alarm_stray_flags |= 0x4000u;
		color_offset_red = 0u;
		color_offset_blue = 0x40u;
		color_offset_green = 0x40u;
		OBJECT(current_object)->escape_alarm_flash.counting_up = 0u;
		OBJECT(current_object)->escape_alarm_flash.step_timer = 0;
		OBJECT(current_object)->escape_alarm_flash.started++;
	}
	OBJECT(current_object)->escape_alarm_flash.step_timer++;
	if (OBJECT(current_object)->escape_alarm_flash.step_timer < 2)
		return;
	OBJECT(current_object)->escape_alarm_flash.step_timer = 0;
	if (OBJECT(current_object)->escape_alarm_flash.counting_up != 0u)
	{
		color_offset_blue++;
		color_offset_green++;
		if (color_offset_blue == 0x3Fu)
		{
			OBJECT(current_object)->escape_alarm_flash.counting_up = 0u;
			sound_id = 0x19u;
			play_sound(sound_id, &write_index, &write_index_address);
		}
	}
	else
	{
		color_offset_blue--;
		color_offset_green--;
		if (color_offset_blue == 0x28u)
		{
			OBJECT(current_object)->escape_alarm_flash.counting_up = 1u;
			sound_id = 0x19u;
			play_sound(sound_id, &write_index, &write_index_address);
		}
	}
}

void run_scroll_skip_mode(void)
{
	if (scroll_skip_open == 0u)
	{
		if ((input_ports_active & 0x21u) != 0x21u)
		{
			scroll_skip_open = 0u;
			return;
		}
		scroll_skip_last_point = scroll_skip_last_points[stage];
		stage_start_section = 0xDu;
		scroll_skip_value_index = 0u;
		scroll_skip_next_armed = 0u;
		scroll_skip_back_armed = 0u;
		scroll_skip_exit_armed = 0u;
		scroll_skip_call_armed = 0u;
		scroll_skip_open = 1u;
		scroll_skip_print_panel();
	}
	scroll_skip_draw_values();
	scroll_skip_next_point();
	scroll_skip_previous_point();
	scroll_skip_check_call();
	scroll_skip_check_exit();
}

void scroll_skip_next_point(void)
{
	uint32_t held_buttons = 0;

	held_buttons = input_ports_active & 0x1010000u;
	if (held_buttons != 0x1010000u)
	{
		if ((held_buttons & 0x10000u) != 0u)
		{
			if (scroll_skip_next_armed != 0u)
			{
				stage_start_section++;
				scroll_skip_next_armed = 0u;
			}
		}
		else
			scroll_skip_next_armed = 1u;
	}
	if (scroll_skip_last_point < (int16_t)stage_start_section)
		stage_start_section = scroll_skip_last_point;
}

void scroll_skip_previous_point(void)
{
	uint32_t held_buttons = 0;

	held_buttons = input_ports_active & 0x1010000u;
	if (held_buttons != 0x1010000u)
	{
		if ((held_buttons & 0x1000000u) != 0u)
		{
			if (scroll_skip_back_armed != 0u)
			{
				stage_start_section--;
				scroll_skip_back_armed = 0u;
			}
		}
		else
			scroll_skip_back_armed = 1u;
	}
	if ((int16_t)stage_start_section < 0)
		stage_start_section = 0u;
}

void scroll_skip_check_exit(void)
{
	uint32_t held_buttons = 0;

	held_buttons = input_ports_active & 0x30u;
	if (held_buttons == 0x30u)
		return;
	if ((held_buttons & 0x20u) != 0u)
	{
		if (scroll_skip_exit_armed != 0u)
		{
			scroll_skip_open = 0u;
			scroll_skip_erase_panel();
			scroll_skip_exit_armed = 0u;
		}
	}
	else
		scroll_skip_exit_armed = 1u;
}

void scroll_skip_erase_panel(void)
{
	uint32_t text = 0, text_position = 0;

	text = scroll_skip_blank_text_ADDRESS;
	print_text_list(&text, &text_position);
}

void scroll_skip_check_call(void)
{
	uint32_t held_buttons = 0, ignored = 0;

	held_buttons = input_ports_active & 0x30u;
	if (held_buttons == 0x30u)
		return;
	if ((held_buttons & 0x10u) != 0u)
	{
		if (scroll_skip_call_armed == 0u)
			return;
		move_camera_to_start_section(&ignored, &ignored, &ignored);
		return;
	}
	scroll_skip_call_armed = 1u;
}

void move_camera_to_start_section(uint32_t *p_y, uint32_t *p_section_offset, uint32_t *p_stage_index)
{
	uint32_t r2 = 0, section_offset = 0, stage_index = 0;

	load_camera_start_section(&section_offset, &stage_index);
	r2 = update_camera_view();
	*p_y = r2;
	*p_section_offset = section_offset;
	*p_stage_index = stage_index;
}

void load_camera_start_section(uint32_t *p_section_offset, uint32_t *p_stage_index)
{
	uint32_t direction_y_cursor = 0, limit_y_cursor = 0, list = 0, section_index = 0, section_offset = 0, stage_index = 0;
	uint16_t start_y = 0, limit_y = 0, direction_y = 0;

	stage_index = stage;
	section_offset = stage_start_section;
	section_offset = (uint16_t)(section_offset << 1);
	list = stage_section_limits_x[stage_index] + stage_section_limits_x_ADDRESS + section_offset;
	limit_y_cursor = stage_section_limits_y[stage_index] + stage_section_limits_y_ADDRESS + section_offset;
	start_y = ROM16(limit_y_cursor);
	limit_y_cursor += 2u;
	camera_x = (uint32_t)ROM16(list) << 16;
	camera_y = (uint32_t)start_y << 16;
	camera_x += (uint32_t)camera_origin_x << 16;
	camera_y += (uint32_t)camera_origin_y << 16;
	list += 2u;
	section_limit_x = ROM16(list);
	limit_y = ROM16(limit_y_cursor);
	section_limit_y = limit_y;
	section_limit_x_list = list + 2u;
	section_limit_y_list = limit_y_cursor + 2u;
	list = stage_section_directions_x[stage_index] + stage_section_directions_x_ADDRESS + section_offset;
	direction_y_cursor = stage_section_directions_y[stage_index] + stage_section_directions_y_ADDRESS + section_offset;
	section_direction_x = (int16_t)ROM16(list);
	direction_y = ROM16(direction_y_cursor);
	section_direction_y = (int16_t)direction_y;
	section_direction_x_list = list + 2u;
	section_direction_y_list = direction_y_cursor + 2u;
	list = stage_section_ends[stage_index] + stage_section_ends_ADDRESS + section_offset;
	scroll_position = ROM16(list);
	section_end_list = list + 2u;
	section_index = section_offset >> 1;
	list = stage_section_speeds[stage_index] + stage_section_speeds_ADDRESS + section_index;
	scroll_speed = mem_read8(list);
	section_speed_list = list + 1u;
	list = stage_section_follow_speeds[stage_index] + stage_section_follow_speeds_ADDRESS + section_index;
	follow_base_speed = mem_read8(list);
	follow_base_speed_list = list + 1u;
	list = stage_scroll_modes[stage_index] + stage_scroll_modes_ADDRESS + section_offset + section_offset;
	scroll_mode = ROM16(list);
	list += 2u;
	scroll_mode_timer = ROM16(list);
	scroll_mode_duration = scroll_mode_timer;
	AS16(scroll_ramp_speed) = 1u;
	scroll_mode_script = list + 2u;
	*p_section_offset = section_offset;
	*p_stage_index = stage_index;
}

void index_stage_list_pair(uint32_t section_offset, uint32_t stage_offset, uint32_t *p_first_entry, uint32_t *p_second_entry)
{
	uint32_t first_entry = *p_first_entry;
	uint32_t second_entry = *p_second_entry;

	second_entry = mem_read32((second_entry & ~0xFFFFu) | (uint16_t)(second_entry + stage_offset)) + section_offset;
	first_entry = mem_read32(first_entry + stage_offset) + section_offset;
	*p_first_entry = first_entry;
	*p_second_entry = second_entry;
}

uint32_t scroll_skip_x_position_cell(void)
{
	uint32_t text_position = 0;

	text_position = TEXT_CELL(34, 18);
	return text_position;
}

uint32_t scroll_skip_y_position_cell(void)
{
	uint32_t text_position = 0;

	text_position = TEXT_CELL(34, 19);
	return text_position;
}

uint32_t scroll_skip_total_cell(void)
{
	uint32_t text_position = 0;

	text_position = TEXT_CELL(34, 17);
	return text_position;
}

void scroll_skip_draw_values(void)
{
	uint16_t attribute_2 = 0;
	uint32_t attribute = 0, point = 0, text_position = 0, value = 0, value_list = 0;

	point = stage_start_section;
	value_list = ROM32(scroll_skip_value_lists_ADDRESS + scroll_skip_value_index * 2u * 2u);
	value = ROM16(mem_read16((value_list & ~0xFFFFu) | (uint16_t)(value_list + stage * 2u)) + value_list + (uint16_t)(point * 2u));
	switch (scroll_skip_value_index)
	{
	case 0:
		text_position = scroll_skip_x_position_cell();
		break;
	case 1:
		text_position = scroll_skip_y_position_cell();
		break;
	case 2:
		text_position = scroll_skip_total_cell();
		break;
	case 3:
		text_position = scroll_skip_x_position_cell();
		break;
	default:
		unknown_code(0x1AAC1u + scroll_skip_value_index * 3u);
	}
	attribute = 0x200u;
	draw_hex_4(&value, &attribute, &text_position);
	scroll_skip_value_index++;
	if (scroll_skip_value_index != 3u)
		return;
	scroll_skip_value_index = 0u;
	value = stage_start_section;
	text_position = TEXT_CELL(24, 15);
	attribute_2 = 0x600u;
	draw_hex_2(value, attribute_2, text_position);
}

void scroll_skip_print_panel(void)
{
	uint32_t text = 0, text_position = 0;

	text = scroll_skip_panel_text_ADDRESS;
	print_text_list(&text, &text_position);
}

void clear_bitmap_lower_half(void)
{
	bitmap_address = VIDEO_RAM + 0xE000u;
	fill_value = fill_value & ~0xFFFFu;
	set_halfwords(VIDEO_RAM + 0xE000u, 0x7000u, fill_value);
	mem_write32(video_layer_disable, mem_read32(video_layer_disable) & ~0x20u);
	AS16(bitmap_8bpp) = 1u;
	bitmap_window_left = 0;
	bitmap_window_top = 0;
	bitmap_window_right = 0x13F;
	bitmap_window_bottom = 0xDF;
}

void animate_stage_palette(uint32_t *p_copy_offset, uint32_t *p_copy_entry, uint32_t *p_palette_count, uint32_t *p_cycle, uint32_t *p_source, uint32_t *p_dest)
{
	uint32_t copy_entry = *p_copy_entry;
	uint32_t palette_count = *p_palette_count;
	uint32_t cycle = *p_cycle;
	uint32_t source = *p_source;
	uint32_t dest = *p_dest;
	uint32_t copy_offset = 0;

	switch (stage)
	{
	case 0:
		skip_stage_palette_animation();
		break;
	case 1:
		skip_stage_palette_animation();
		break;
	case 2:
		skip_stage_palette_animation();
		break;
	case 3:
		skip_stage_palette_animation();
		break;
	case 4:
		animate_palette_stage_04(&copy_offset, &copy_entry, &palette_count, &cycle, &source, &dest);
		break;
	case 5:
		skip_stage_palette_animation();
		break;
	case 6:
		skip_stage_palette_animation();
		break;
	case 7:
		skip_stage_palette_animation();
		break;
	case 8:
		skip_stage_palette_animation();
		break;
	case 9:
		animate_palette_stage_09(&copy_offset, &copy_entry, &palette_count, &cycle, &source, &dest);
		break;
	case 10:
		animate_palette_stage_0a(&copy_offset, &copy_entry, &palette_count, &cycle, &source, &dest);
		break;
	case 11:
		skip_stage_palette_animation();
		break;
	case 12:
		animate_palette_stage_0c(&copy_offset, &copy_entry, &palette_count, &cycle, &source, &dest);
		break;
	case 13:
		animate_palette_stage_0d_0e(&copy_offset, &copy_entry, &palette_count, &cycle, &source, &dest);
		break;
	case 14:
		animate_palette_stage_0d_0e(&copy_offset, &copy_entry, &palette_count, &cycle, &source, &dest);
		break;
	case 15:
		animate_palette_stage_0f(&copy_offset, &copy_entry, &palette_count, &cycle, &source, &dest);
		break;
	case 16:
		animate_palette_stage_10(&copy_offset, &copy_entry, &palette_count, &cycle, &source, &dest);
		break;
	case 17:
		skip_stage_palette_animation();
		break;
	case 18:
		skip_stage_palette_animation();
		break;
	case 19:
		skip_stage_palette_animation();
		break;
	default:
		unknown_code(0x1AD41u + stage * 3u);
	}
	*p_copy_offset = copy_offset;
	*p_copy_entry = copy_entry;
	*p_palette_count = palette_count;
	*p_cycle = cycle;
	*p_source = source;
	*p_dest = dest;
}

void skip_stage_palette_animation(void)
{
}

void animate_palette_stage_10(uint32_t *p_copy_offset, uint32_t *p_copy_entry, uint32_t *p_palette_count, uint32_t *p_cycle, uint32_t *p_source, uint32_t *p_dest)
{
	uint32_t copy_entry = *p_copy_entry;
	uint16_t halfwords = 0;
	uint32_t copy_offset = 0, cycle = 0, cycle_1 = 0, dest = 0, dest_1 = 0, ignored = 0, palette_count = 0, palette_count_1 = 0, source = 0, source_1 = 0;

	dest = PALETTE_RAM + 0x8B40u;
	palette_count = 6u;
	source = stage_10_cycle_palettes_0_ADDRESS;
	halfwords = 0x10u;
	cycle = palette_cycle_0_ADDRESS;
	palette_cycle_0 = 6u;
	palette_cycle_0_frames = 8u;
	cycle_palette(cycle, halfwords, &ignored, &copy_entry, &palette_count, &source, &dest);
	cycle_1 = palette_cycle_1_ADDRESS;
	dest_1 = PALETTE_RAM + 0x8940u;
	palette_count_1 = 1u;
	source_1 = stage_10_cycle_palettes_1_ADDRESS;
	halfwords = 0x10u;
	palette_cycle_1 = 4u;
	palette_cycle_1_frames = 8u;
	cycle_palette(cycle_1, halfwords, &copy_offset, &copy_entry, &palette_count_1, &source_1, &dest_1);
	*p_copy_offset = copy_offset;
	*p_copy_entry = copy_entry;
	*p_palette_count = palette_count_1;
	*p_cycle = cycle_1;
	*p_source = source_1;
	*p_dest = dest_1;
}

void animate_palette_stage_0c(uint32_t *p_copy_offset, uint32_t *p_copy_entry, uint32_t *p_palette_count, uint32_t *p_cycle, uint32_t *p_source, uint32_t *p_dest)
{
	uint32_t copy_entry = *p_copy_entry;
	uint16_t halfwords = 0;
	uint32_t copy_offset = 0, cycle = 0, dest = 0, palette_count = 0, source = 0;

	dest = PALETTE_RAM + 0x8800u;
	palette_count = 1u;
	source = stage_0c_cycle_palettes_ADDRESS;
	halfwords = 0x10u;
	cycle = palette_cycle_0_ADDRESS;
	palette_cycle_0 = 6u;
	palette_cycle_0_frames = 8u;
	cycle_palette(cycle, halfwords, &copy_offset, &copy_entry, &palette_count, &source, &dest);
	*p_copy_offset = copy_offset;
	*p_copy_entry = copy_entry;
	*p_palette_count = palette_count;
	*p_cycle = cycle;
	*p_source = source;
	*p_dest = dest;
}

void animate_palette_stage_0d_0e(uint32_t *p_copy_offset, uint32_t *p_copy_entry, uint32_t *p_palette_count, uint32_t *p_cycle, uint32_t *p_source, uint32_t *p_dest)
{
	uint32_t copy_entry = *p_copy_entry;
	uint16_t halfwords = 0;
	uint32_t copy_offset = 0, cycle = 0, dest = 0, palette_count = 0, source = 0;

	dest = PALETTE_RAM + 0x88C0u;
	palette_count = 1u;
	source = stage_0d_0e_cycle_palettes_ADDRESS;
	halfwords = 0x10u;
	cycle = palette_cycle_0_ADDRESS;
	palette_cycle_0 = 6u;
	palette_cycle_0_frames = 8u;
	cycle_palette(cycle, halfwords, &copy_offset, &copy_entry, &palette_count, &source, &dest);
	*p_copy_offset = copy_offset;
	*p_copy_entry = copy_entry;
	*p_palette_count = palette_count;
	*p_cycle = cycle;
	*p_source = source;
	*p_dest = dest;
}

void animate_palette_stage_0f(uint32_t *p_copy_offset, uint32_t *p_copy_entry, uint32_t *p_palette_count, uint32_t *p_cycle, uint32_t *p_source, uint32_t *p_dest)
{
	uint32_t copy_entry = *p_copy_entry;
	uint16_t halfwords = 0;
	uint32_t copy_offset = 0, cycle = 0, dest = 0, ignored = 0, palette_count = 0, source = 0;

	dest = PALETTE_RAM + 0x8920u;
	palette_count = 3u;
	source = stage_0f_cycle_palettes_0_ADDRESS;
	halfwords = 0x10u;
	cycle = palette_cycle_0_ADDRESS;
	palette_cycle_0 = 0xCu;
	palette_cycle_0_frames = 6u;
	cycle_palette(cycle, halfwords, &ignored, &copy_entry, &palette_count, &source, &dest);
	cycle = palette_cycle_1_ADDRESS;
	dest = PALETTE_RAM + 0x8900u;
	palette_count = 1u;
	source = stage_0f_cycle_palettes_1_ADDRESS;
	halfwords = 0x10u;
	palette_cycle_1 = 6u;
	palette_cycle_1_frames = 6u;
	cycle_palette(cycle, halfwords, &copy_offset, &copy_entry, &palette_count, &source, &dest);
	*p_copy_offset = copy_offset;
	*p_copy_entry = copy_entry;
	*p_palette_count = palette_count;
	*p_cycle = cycle;
	*p_source = source;
	*p_dest = dest;
}

void animate_palette_stage_0a(uint32_t *p_copy_offset, uint32_t *p_copy_entry, uint32_t *p_palette_count, uint32_t *p_cycle, uint32_t *p_source, uint32_t *p_dest)
{
	uint32_t copy_entry = *p_copy_entry;
	uint16_t halfwords = 0;
	uint32_t copy_offset = 0, cycle = 0, dest = 0, ignored = 0, palette_count = 0, source = 0;

	dest = PALETTE_RAM + 0x8A40u;
	palette_count = 1u;
	source = stage_0a_cycle_palettes_0_ADDRESS;
	halfwords = 0x10u;
	cycle = palette_cycle_0_ADDRESS;
	palette_cycle_0 = 5u;
	palette_cycle_0_frames = 6u;
	cycle_palette(cycle, halfwords, &ignored, &copy_entry, &palette_count, &source, &dest);
	cycle = palette_cycle_1_ADDRESS;
	dest = PALETTE_RAM + 0x8A60u;
	palette_count = 9u;
	source = stage_0a_cycle_palettes_1_ADDRESS;
	halfwords = 0x10u;
	palette_cycle_1 = 5u;
	palette_cycle_1_frames = 0xAu;
	cycle_palette(cycle, halfwords, &copy_offset, &copy_entry, &palette_count, &source, &dest);
	*p_copy_offset = copy_offset;
	*p_copy_entry = copy_entry;
	*p_palette_count = palette_count;
	*p_cycle = cycle;
	*p_source = source;
	*p_dest = dest;
}

void animate_palette_stage_09(uint32_t *p_copy_offset, uint32_t *p_copy_entry, uint32_t *p_palette_count, uint32_t *p_cycle, uint32_t *p_source, uint32_t *p_dest)
{
	uint32_t copy_entry = *p_copy_entry;
	uint16_t halfwords = 0;
	uint32_t copy_offset = 0, cycle = 0, dest = 0, palette_count = 0, source = 0;

	dest = PALETTE_RAM + 0x88A0u;
	palette_count = 2u;
	source = stage_09_cycle_palettes_ADDRESS;
	halfwords = 0x10u;
	cycle = palette_cycle_0_ADDRESS;
	palette_cycle_0 = 3u;
	palette_cycle_0_frames = 7u;
	cycle_palette(cycle, halfwords, &copy_offset, &copy_entry, &palette_count, &source, &dest);
	*p_copy_offset = copy_offset;
	*p_copy_entry = copy_entry;
	*p_palette_count = palette_count;
	*p_cycle = cycle;
	*p_source = source;
	*p_dest = dest;
}

void animate_palette_stage_04(uint32_t *p_copy_offset, uint32_t *p_copy_entry, uint32_t *p_palette_count, uint32_t *p_cycle, uint32_t *p_source, uint32_t *p_dest)
{
	uint32_t copy_entry = *p_copy_entry;
	uint16_t halfwords = 0;
	uint32_t copy_offset = 0, cycle = 0, dest = 0, palette_count = 0, source = 0;

	dest = PALETTE_RAM + 0x8500u;
	palette_count = 2u;
	source = stage_04_cycle_palettes_ADDRESS;
	halfwords = 0x10u;
	cycle = palette_cycle_0_ADDRESS;
	palette_cycle_0 = 4u;
	palette_cycle_0_frames = 8u;
	cycle_palette(cycle, halfwords, &copy_offset, &copy_entry, &palette_count, &source, &dest);
	*p_copy_offset = copy_offset;
	*p_copy_entry = copy_entry;
	*p_palette_count = palette_count;
	*p_cycle = cycle;
	*p_source = source;
	*p_dest = dest;
}

void cycle_palette(uint32_t cycle, uint16_t halfwords, uint32_t *p_copy_offset, uint32_t *p_copy_entry, uint32_t *p_palette_count, uint32_t *p_source, uint32_t *p_dest)
{
	uint32_t copy_entry = *p_copy_entry;
	uint32_t palette_count = *p_palette_count;
	uint32_t source = *p_source;
	uint32_t dest = *p_dest;
	uint32_t copy_offset = 0, frame_offset = 0;

	frame_offset = (uint32_t)mem_read8(cycle + 3u) << 5;
	source += frame_offset;
	if ((int8_t)mem_read8(cycle + 4u) > 0)
	{
		mem_write8(cycle + 4u, mem_read8(cycle + 4u) - 1u);
		*p_copy_offset = frame_offset;
		*p_source = source;
		return;
	}
	do
	{
		mem_write8(cycle + 4u, mem_read8(cycle));
		queue_vblank_copy(source, dest, halfwords, &copy_offset, &copy_entry);
		dest += 0x20u;
		palette_count--;
	} while (palette_count != 0u);
	mem_write8(cycle + 3u, mem_read8(cycle + 3u) + 1u);
	if ((int8_t)mem_read8(cycle + 3u) < (int8_t)mem_read8(cycle + 1u))
	{
		mem_write8(cycle + 4u, mem_read8(cycle + 4u) - 1u);
		*p_copy_offset = copy_offset;
		*p_copy_entry = copy_entry;
		*p_palette_count = palette_count;
		*p_source = source;
		*p_dest = dest;
		return;
	}
	mem_write8(cycle + 3u, 0u);
	mem_write8(cycle + 4u, mem_read8(cycle + 4u) - 1u);
	*p_copy_offset = copy_offset;
	*p_copy_entry = copy_entry;
	*p_palette_count = palette_count;
	*p_source = source;
	*p_dest = dest;
}

uint32_t cycle_palette_set_0(uint32_t source)
{
	palette_cycle_set = 0x209090u;
	source = cycle_palette_set(source);
	return source;
}

uint32_t cycle_palette_set_1(uint32_t source)
{
	palette_cycle_set = 0x2090A0u;
	source = cycle_palette_set(source);
	return source;
}

uint32_t cycle_palette_set(uint32_t source)
{
	uint16_t halfwords = 0;
	uint32_t cycle_set = 0, dest = 0, ignored = 0;

	cycle_set = palette_cycle_set;
	if (mem_read8(cycle_set + 8u) == 0u)
	{
		mem_write8(cycle_set + 8u, mem_read8(cycle_set + 0xAu));
		mem_write8(cycle_set + 9u, mem_read8(cycle_set + 0xBu));
		do
		{
			halfwords = 0x10u;
			dest = mem_read32(cycle_set + 4u) + ((uint32_t)(uint8_t)(mem_read8(cycle_set + 0xBu) - mem_read8(cycle_set + 9u)) << 5);
			source = mem_read32(cycle_set) + ((uint32_t)mem_read8(cycle_set + 0xCu) << 5);
			queue_vblank_copy(source, dest, halfwords, &ignored, &ignored);
			if (mem_read8(cycle_set + 9u) == 0u)
				break;
			mem_write8(cycle_set + 9u, mem_read8(cycle_set + 9u) - 1u);
		} while (mem_read8(cycle_set + 9u) != 0u);
		mem_write8(cycle_set + 0xCu, mem_read8(cycle_set + 0xCu) + 1u);
	}
	mem_write8(cycle_set + 8u, mem_read8(cycle_set + 8u) - 1u);
	if ((int8_t)mem_read8(cycle_set + 0xDu) <= (int8_t)mem_read8(cycle_set + 0xCu))
		mem_write8(cycle_set + 0xCu, 0u);
	return source;
}

void load_stage_palette_set(uint32_t *p_tile_bank_bits, uint32_t *p_dest)
{
	uint32_t ignored = 0, palette_dest = 0, stage_index = 0, tile_bank_bits = 0;

	stage_index = stage;
	load_palette_set_and_backdrop(stage_index, &tile_bank_bits, &ignored, &palette_dest);
	*p_tile_bank_bits = tile_bank_bits;
	*p_dest = palette_dest;
}

void load_palette_set_and_backdrop(uint32_t stage_index, uint32_t *p_tile_bank_bits, uint32_t *p_source, uint32_t *p_dest)
{
	uint32_t dest = 0, palette_list = 0, palette_set = 0, source = 0, tile_bank_bits = 0, tile_bank_entry = 0;

	mem_write32(video_control, mem_read32(video_control) & ~0x400u);
	tile_bank_entry = stage_tile_bank_bits_ADDRESS + stage_index;
	tile_bank_bits = (tile_bank_entry & ~0xFFFFu) | mem_read8(tile_bank_entry);
	tile_bank_bits = (tile_bank_bits & ~0xFFFFu) | (uint16_t)(tile_bank_bits << 10);
	mem_write16(video_control, mem_read16(video_control) | tile_bank_bits);
	palette_set = stage_palette_sets[stage_index];
	vblank_copies_pending = 0u;
	vblank_copy_count = 0u;
	palette_list = palette_set;
	load_palette_list(palette_list, &source, &dest);
	mem_write16(video_backdrop_control, mem_read16(video_backdrop_control) | 0x200u);
	switch (stage)
	{
	case 0:
	case 1:
	case 2:
	case 4:
	case 5:
	case 6:
	case 8:
	case 10:
	case 11:
	case 12:
	case 13:
	case 14:
	case 15:
	case 16:
	case 17:
	case 18:
		mem_write16(backdrop_color, 0u);
		*p_tile_bank_bits = tile_bank_bits;
		*p_source = source;
		*p_dest = dest;
		return;
	case 3:
	case 7:
		mem_write16(backdrop_color, 0x200u);
		*p_tile_bank_bits = tile_bank_bits;
		*p_source = source;
		*p_dest = dest;
		return;
	case 9:
	case 19:
		mem_write16(backdrop_color, 0x6212u);
		*p_tile_bank_bits = tile_bank_bits;
		*p_source = source;
		*p_dest = dest;
		return;
	default:
		unknown_code(0x1B033u + stage * 3u);
	}
}

void set_backdrop_color_19aa(void)
{
	mem_write16(backdrop_color, 0x19AAu);
}

uint32_t init_nbg0_picture_screen(void)
{
	uint32_t dest = 0, ignored = 0, palette_list = 0;
	uint8_t io_cnt_bits = 0;

	io_cnt_bits = mem_read8(io_cnt);
	mem_write8(io_cnt, io_cnt_bits & ~2u);
	display_off_frames = 3u;
	mem_write32(video_control, mem_read32(video_control) & ~0x400u);
	mem_write16(video_control, mem_read16(video_control) | 0x400u);
	vblank_copies_pending = 0u;
	vblank_copy_count = 0u;
	palette_list = tile_bank_1_palette_list_ADDRESS;
	load_palette_list(palette_list, &ignored, &dest);
	mem_write16(video_layer_disable, 0x2Eu);
	nbg0_scroll_x_int = 0u;
	nbg0_scroll_y_int = 0u;
	nbg0_pages = 0u;
	return dest;
}

void show_winners_dont_use_drugs_screen(void)
{
	uint32_t count = 0, dest = 0, script = 0;

	dest = init_nbg0_picture_screen();
	script = winners_dont_use_drugs_script_ADDRESS;
	run_vram_script(&count, &script, &dest);
}

void show_recycle_it_screen(void)
{
	uint32_t count = 0, dest = 0, script = 0;

	dest = init_nbg0_picture_screen();
	script = recycle_it_script_ADDRESS;
	run_vram_script(&count, &script, &dest);
}

void show_nbg0_picture(uint32_t *p_copy_entry, uint32_t *p_rows, uint32_t *p_source, uint32_t *p_dest)
{
	uint16_t halfwords = 0;
	uint32_t copy_entry = 0, count = 0, dest = 0, ignored = 0, rows = 0, script = 0, source = 0;

	dest = init_nbg0_picture_screen();
	nbg0_pages = 0x1010101u;
	script = sonic_head_pattern_script_ADDRESS;
	run_vram_script(&count, &script, &dest);
	dest = PALETTE_RAM + 0x8BE0u;
	rows = 1u;
	source = nbg0_picture_palette_ADDRESS;
	halfwords = 0x10u;
	do
	{
		queue_vblank_copy(source, dest, halfwords, &ignored, &copy_entry);
		dest += 0x20u;
		rows--;
	} while (rows != 0u);
	*p_copy_entry = copy_entry;
	*p_rows = rows;
	*p_source = source;
	*p_dest = dest;
}

uint32_t set_stage_layer_mixing(void)
{
	uint32_t count = 0, mixer_register = 0, priority_cursor = 0, stage_index = 0;

	priority_cursor = stage_layer_priorities_ADDRESS + ((uint32_t)stage << 2);
	mixer_register = mixer_nbg0;
	count = 4u;
	do
	{
		mem_write16(mixer_register, ROM8(priority_cursor) + 0x200u);
		count--;
		priority_cursor++;
		mixer_register += 2u;
	} while (count != 0u);
	stage_index = stage;
	mem_write8(video_layer_disable, mem_read8(video_layer_disable) & ~0xFu);
	mem_write8(video_layer_disable, mem_read8(video_layer_disable) | stage_layer_disables[stage_index]);
	mem_write16(video_rowscroll_control, stage_rowscroll_settings[stage_index]);
	return count;
}

uint32_t init_nbg1_layer(uint32_t dest)
{
	uint32_t count = 0, script = 0;

	nbg1_pages = 0x1211100Fu;
	script = clear_tile_page_0f_script_ADDRESS;
	run_vram_script(&count, &script, &dest);
	script = clear_tile_page_10_script_ADDRESS;
	run_vram_script(&count, &script, &dest);
	script = clear_tile_page_11_script_ADDRESS;
	run_vram_script(&count, &script, &dest);
	script = clear_tile_page_12_script_ADDRESS;
	run_vram_script(&count, &script, &dest);
	switch (stage)
	{
	case 0:
	case 1:
	case 2:
	case 3:
	case 4:
	case 5:
	case 6:
	case 7:
	case 8:
	case 10:
	case 11:
	case 12:
	case 13:
	case 14:
	case 15:
	case 17:
	case 18:
		mixer_nbg1_blend &= 0xFF7Fu;
		return dest;
	case 9:
	case 19:
		mixer_nbg0_blend &= ~0x780u;
		mixer_nbg1_blend &= ~0x780u;
		mixer_nbg2_blend &= ~0x780u;
		mixer_nbg3_blend &= ~0x780u;
		mixer_nbg0_blend |= 0x100u;
		return dest;
	case 16:
		mixer_nbg0_blend |= 0x100u;
		mixer_blend |= 0xB00u;
		mixer_nbg1_blend &= ~0xF00u;
		mixer_nbg1_blend &= 0xFF7Fu;
		return dest;
	default:
		unknown_code(0x1B30Bu + stage * 3u);
	}
}

void update_nbg1_scroll(void)
{
	switch (stage)
	{
	case 0:
		skip_nbg1_scroll();
		break;
	case 1:
		skip_nbg1_scroll();
		break;
	case 2:
		skip_nbg1_scroll();
		break;
	case 3:
		set_nbg1_scroll_to_camera();
		break;
	case 4:
		skip_nbg1_scroll();
		break;
	case 5:
		skip_nbg1_scroll();
		break;
	case 6:
		skip_nbg1_scroll();
		break;
	case 7:
		set_nbg1_scroll_to_camera();
		break;
	case 8:
		set_nbg1_scroll_to_camera();
		break;
	case 9:
		set_nbg1_scroll_to_camera();
		break;
	case 10:
		set_nbg1_scroll_to_camera();
		break;
	case 11:
		set_nbg1_scroll_to_camera();
		break;
	case 12:
		skip_nbg1_scroll();
		break;
	case 13:
		skip_nbg1_scroll();
		break;
	case 14:
		skip_nbg1_scroll();
		break;
	case 15:
		skip_nbg1_scroll();
		break;
	case 16:
		set_nbg1_scroll_to_camera();
		break;
	case 17:
		skip_nbg1_scroll();
		break;
	case 18:
		skip_nbg1_scroll();
		break;
	case 19:
		skip_nbg1_scroll();
		break;
	default:
		unknown_code(0x1B398u + stage * 3u);
	}
	skip_nbg1_scroll();
}

void skip_nbg1_scroll(void)
{
}

void set_nbg1_scroll_to_camera(void)
{
	uint16_t layer_scroll_y = 0;

	layer_scroll_y = camera_y_pixels;
	nbg1_scroll_x_int = camera_x_pixels;
	nbg1_scroll_y_int = layer_scroll_y;
}
