/**
 *    Name:        objects/stage_09_traps.c
 *    Description: object types 15-18 (stage 9): the spear traps, the ice cube launcher, the padded wall
 *    Version:     0.1.0
 *    Created:     12-10-2026
 *    Modified:    15-10-2026
 *    Author:      Sebastian Toyos
 *
 *    SegaSonic The Hedgehog (arcade, Sega System 32): the game's V60 program decompiled into C by
 *    tools/decompile.py, with the names of symbols.txt.
 */
#include "game.h"

void spear_trap_init_100(void)
{
	OBJECT(current_object)->direction = 0x100u;
	spear_trap_init();
}

void spear_trap_init_300(void)
{
	OBJECT(current_object)->direction = 0x300u;
	spear_trap_init();
}

void spear_trap_init(void)
{
	OBJECT(current_object)->sprite_flags |= 0x4000u;
	OBJECT(current_object)->sprite_flags = 0x5000u;
	OBJECT(current_object)->sprite_frame = 0x7FFu;
	OBJECT(current_object)->sprite_palette = 0u;
	OBJECT(current_object)->sprite_scale = 0x7FFu;
	OBJECT(current_object)->flags |= 2u;
	OBJECT(current_object)->hitbox_size_x = 0x18u;
	OBJECT(current_object)->hitbox_size_y = 0x18u;
	OBJECT(current_object)->hitbox_offset_x = -4;
	OBJECT(current_object)->hitbox_offset_y = -0x13;
	OBJECT(current_object)->hitbox_height = 0x100u;
	OBJECT(current_object)->spear_trap.phase_timer = OBJECT(current_object)->spawn_arg_2;
	OBJECT(current_object)->spear_trap.spear_z = OBJECT(current_object)->z;
	OBJECT(current_object)->handler = spear_trap_wait_ADDRESS;
	current_object = LINKED(current_object, OBJECT(current_object)->child);
	OBJECT(current_object)->sprite_frame = 0x73A0u;
	OBJECT(current_object)->sprite_palette = 0x9050u;
	OBJECT(current_object)->sprite_flags = 0u;
	current_object = LINKED(current_object, OBJECT(current_object)->child);
	OBJECT(current_object)->sprite_flags |= 0x4000u;
	OBJECT(current_object)->sprite_flags = 0x5000u;
	OBJECT(current_object)->sprite_palette = 0u;
	OBJECT(current_object)->sprite_scale = 0x7FFu;
	current_object = LINKED(current_object, OBJECT(current_object)->child);
	OBJECT(current_object)->sprite_flags = 0x5000u;
	OBJECT(current_object)->sprite_frame = 0x7FFu;
	OBJECT(current_object)->sprite_palette = 0u;
	OBJECT(current_object)->sprite_scale = 0x7FFu;
	current_object = LINKED(current_object, OBJECT(current_object)->child);
	OBJECT(current_object)->sprite_frame = 0x73A0u;
	OBJECT(current_object)->sprite_palette = 0x1050u;
	OBJECT(current_object)->sprite_palette |= 0x2000u;
	OBJECT(current_object)->sprite_flags = 0u;
	current_object = LINKED(current_object, OBJECT(current_object)->child);
	OBJECT(current_object)->sprite_flags = 0x5000u;
	OBJECT(current_object)->sprite_palette = 0u;
	OBJECT(current_object)->sprite_scale = 0x7FFu;
	current_object = LINKED(current_object, OBJECT(current_object)->child);
	OBJECT(current_object)->sprite_frame = 0x7390u;
	OBJECT(current_object)->sprite_palette = 0x1030u;
	OBJECT(current_object)->sprite_palette |= 0x2000u;
	current_object = LINKED(current_object, OBJECT(current_object)->child);
	OBJECT(current_object)->sprite_frame = 0x7388u;
	OBJECT(current_object)->sprite_palette = 0x1030u;
	OBJECT(current_object)->sprite_palette |= 0x2000u;
	current_object = LINKED(current_object, OBJECT(current_object)->child);
	OBJECT(current_object)->sprite_frame = 0x7398u;
	OBJECT(current_object)->sprite_palette = 0x9040u;
	current_object = LINKED(current_object, OBJECT(current_object)->child);
	OBJECT(current_object)->sprite_frame = 0x73A8u;
	OBJECT(current_object)->sprite_palette = 0x9040u;
}

void spear_trap_wait(void)
{
	uint32_t block_offset = 0, deleted = 0, slope_height = 0;
	uint16_t phase_timer = 0;

	OBJECT(current_object)->health = 0x7F;
	delete_if_off_screen(&deleted, &block_offset, &slope_height);
	if ((uint8_t)deleted == 1u)
		return;
	phase_timer = OBJECT(current_object)->spear_trap.phase_timer;
	OBJECT(current_object)->spear_trap.phase_timer = phase_timer - 1u;
	if (phase_timer != 0u)
	{
		spear_trap_draw();
		return;
	}
	OBJECT(current_object)->spear_trap.phase_timer = 0xCu;
	OBJECT(current_object)->kind = 4u;
	OBJECT(current_object)->handler = spear_trap_rise_ADDRESS;
	spear_trap_rise();
}

void spear_trap_rise(void)
{
	uint8_t sound_id = 0;
	uint32_t write_index = 0, write_index_address = 0;
	uint16_t phase_timer = 0;

	OBJECT(current_object)->speed_z -= 0x10F00u;
	phase_timer = OBJECT(current_object)->spear_trap.phase_timer;
	OBJECT(current_object)->spear_trap.phase_timer = phase_timer - 1u;
	if (phase_timer != 0u)
	{
		spear_trap_draw_touchable();
		return;
	}
	sound_id = 0x1Bu;
	play_sound(sound_id, &write_index, &write_index_address);
	OBJECT(current_object)->speed_z = 0;
	OBJECT(current_object)->spear_trap.phase_timer = 0x3Cu;
	OBJECT(current_object)->kind = 0xCu;
	OBJECT(current_object)->handler = spear_trap_stay_up_ADDRESS;
	spear_trap_stay_up();
}

void spear_trap_stay_up(void)
{
	uint16_t phase_timer = 0;

	phase_timer = OBJECT(current_object)->spear_trap.phase_timer;
	OBJECT(current_object)->spear_trap.phase_timer = phase_timer - 1u;
	if (phase_timer != 0u)
	{
		spear_trap_draw_touchable();
		return;
	}
	OBJECT(current_object)->handler = spear_trap_retract_ADDRESS;
	spear_trap_retract();
}

void spear_trap_retract(void)
{
	uint32_t r1 = 0, height = 0, z = 0;

	OBJECT(current_object)->z = OBJECT(current_object)->spear_trap.spear_z;
	fall_to_floor(&height, &r1, &z);
	if (height != 0u)
	{
		spear_trap_draw_touchable();
		return;
	}
	OBJECT(current_object)->spear_trap.phase_timer = OBJECT(current_object)->spawn_arg_1;
	OBJECT(current_object)->handler = spear_trap_wait_ADDRESS;
	spear_trap_draw_touchable();
}

void spear_trap_draw_touchable(void)
{
	add_to_touchable_list(0u);
	spear_trap_draw();
}

void spear_trap_draw(void)
{
	uint8_t bucket = 0;
	uint32_t cursor = 0, depth = 0, ignored = 0;

	OBJECT(current_object)->z = OBJECT(current_object)->spear_trap.spear_z;
	move_object_projected(&ignored, &ignored, &ignored, &cursor);
	r14 = (uint16_t)(OBJECT(current_object)->floor_z_int - OBJECT(current_object)->z_int + OBJECT(current_object)->screen_y);
	depth = r14;
	queue_sprite_at_depth(&ignored, &depth);
	current_object = LINKED(current_object, OBJECT(current_object)->child);
	OBJECT(current_object)->screen_x = OBJECT(object_cursor)->screen_x;
	OBJECT(current_object)->screen_y = OBJECT(object_cursor)->screen_y;
	OBJECT(current_object)->screen_y += 0x39u;
	depth = (uint16_t)r14;
	queue_sprite_at_depth(&ignored, &depth);
	current_object = LINKED(current_object, OBJECT(current_object)->child);
	OBJECT(current_object)->sprite_frame = 0xFFFBu + r14;
	depth = (uint16_t)r14;
	queue_sprite_at_depth(&ignored, &depth);
	current_object = LINKED(current_object, OBJECT(current_object)->child);
	bucket = 0xFFu;
	queue_sprite_in_bucket(bucket);
	current_object = LINKED(current_object, OBJECT(current_object)->child);
	OBJECT(current_object)->screen_x = OBJECT(object_cursor)->screen_x;
	OBJECT(current_object)->screen_y = OBJECT(object_cursor)->screen_y;
	OBJECT(current_object)->screen_y += 0x39u;
	bucket = 0xFFu;
	queue_sprite_in_bucket(bucket);
	current_object = LINKED(current_object, OBJECT(current_object)->child);
	OBJECT(current_object)->sprite_frame = 0x20u + r14;
	bucket = 0xFFu;
	queue_sprite_in_bucket(bucket);
	current_object = LINKED(current_object, OBJECT(current_object)->child);
	OBJECT(current_object)->screen_x = OBJECT(object_cursor)->screen_x;
	OBJECT(current_object)->screen_y = r14;
	OBJECT(current_object)->screen_y += 0x20u;
	bucket = 0xFEu;
	queue_sprite_in_bucket(bucket);
	current_object = LINKED(current_object, OBJECT(current_object)->child);
	OBJECT(current_object)->screen_x = OBJECT(object_cursor)->screen_x;
	OBJECT(current_object)->screen_y = r14;
	OBJECT(current_object)->screen_y += 0x20u;
	bucket = 0xFFu;
	queue_sprite_in_bucket(bucket);
	current_object = LINKED(current_object, OBJECT(current_object)->child);
	OBJECT(current_object)->screen_x = OBJECT(object_cursor)->screen_x;
	OBJECT(current_object)->screen_y = r14;
	depth = (uint16_t)(OBJECT(current_object)->screen_y - 0xFu);
	queue_sprite_at_depth(&ignored, &depth);
	current_object = LINKED(current_object, OBJECT(current_object)->child);
	OBJECT(current_object)->screen_x = OBJECT(object_cursor)->screen_x;
	OBJECT(current_object)->screen_y = r14;
	depth = (uint16_t)(OBJECT(current_object)->screen_y - 0xFu);
	queue_sprite_at_depth(&ignored, &depth);
	current_object = object_cursor;
	OBJECT(current_object)->spear_trap.spear_z = OBJECT(current_object)->z;
	OBJECT(current_object)->z = OBJECT(current_object)->floor_z;
}

void ice_launcher_init(void)
{
	OBJECT(current_object)->sprite_frame = 0x7420u;
	OBJECT(current_object)->sprite_palette = 0x9110u;
	OBJECT(current_object)->ice_launcher.last_step = 0xF;
	OBJECT(current_object)->sprite_flags = 9u;
	OBJECT(current_object)->flags |= 2u;
	OBJECT(current_object)->hitbox_height = 0x28u;
	OBJECT(current_object)->kind = 5u;
	OBJECT(current_object)->handler = ice_launcher_update_ADDRESS;
}

void ice_launcher_update(void)
{
	uint32_t r0 = 0, r11 = 0, block_offset = 0, depth = 0, frame_index = 0, ignored = 0, object = 0, offset_record = 0, size_record = 0;
	uint16_t step_timer = 0;

	unproject_object(&r0, &block_offset, &object, &r11);
	if (OBJECT(current_object)->spawn_arg_2 <= scroll_position)
	{
		delete_object_and_children(&ignored, &block_offset, &object);
		return;
	}
	step_timer = OBJECT(current_object)->ice_launcher.step_timer;
	OBJECT(current_object)->ice_launcher.step_timer = step_timer - 1u;
	if (step_timer != 0u)
	{
		add_to_touchable_list(r0);
		depth = (uint16_t)(OBJECT(current_object)->screen_y + 0x30u);
		queue_sprite_at_depth(&ignored, &depth);
		return;
	}
	OBJECT(current_object)->ice_launcher.step++;
	if (OBJECT(current_object)->ice_launcher.step <= OBJECT(current_object)->ice_launcher.last_step)
	{
		frame_index = ice_launcher_frames[(uint8_t)OBJECT(current_object)->ice_launcher.step];
		OBJECT(current_object)->sprite_frame = ice_launcher_sprites[frame_index];
		size_record = ice_launcher_hitbox_sizes_ADDRESS + frame_index * 4u;
		OBJECT(current_object)->hitbox_size_x = ROM16(size_record);
		OBJECT(current_object)->hitbox_size_y = ROM16(size_record + 2u);
		offset_record = ice_launcher_hitbox_offsets_ADDRESS + frame_index * 2u;
		OBJECT(current_object)->hitbox_offset_x = ROM8(offset_record);
		offset_record++;
		OBJECT(current_object)->hitbox_offset_y = ROM8(offset_record);
		r0 = OBJECT(current_object)->ice_launcher.step_duration_index;
		OBJECT(current_object)->ice_launcher.step_timer = ice_launcher_frame_durations[r0];
		if ((uint8_t)OBJECT(current_object)->ice_launcher.step == 0xCu)
		{
			spawn_count = 2u;
			spawn_object_with_pieces(0x011D7Du, &r0, &size_record, &offset_record, &object, &r11);
			if ((uint8_t)r0 != 0u)
			{
				OBJECT(object_cursor)->ice_launcher.step_timer += 3u;
				copy_position_to(object);
				OBJECT(object)->projected_x -= 0x2Bu;
				OBJECT(object)->projected_y_int += 0x4Fu;
				OBJECT(object)->sprite_frame = 0x7418u;
				OBJECT(object)->sprite_palette = OBJECT(object_cursor)->sprite_palette;
				OBJECT(object)->flags |= 2u;
				OBJECT(object)->hitbox_size_x = 0x22u;
				OBJECT(object)->hitbox_size_y = 0x42u;
				OBJECT(object)->hitbox_offset_x = -0xB;
				OBJECT(object)->hitbox_offset_y = -0x3C;
				OBJECT(object)->hitbox_height = 0x28u;
				OBJECT(object)->kind = 0x19u;
				OBJECT(object)->handler = ice_cube_slide_ADDRESS;
				OBJECT(object)->speed = ice_cube_speeds[OBJECT(current_object)->ice_launcher.cube_speed_index];
				object = LINKED(object, OBJECT(object)->child);
				OBJECT(object)->sprite_frame = 0x7418u;
				OBJECT(object)->sprite_palette = 0x11D0u;
				OBJECT(object)->sprite_palette |= 0x2000u;
			}
		}
	}
	else
	{
		OBJECT(current_object)->ice_launcher.step = 0;
		OBJECT(current_object)->sprite_frame = 0x7420u;
		OBJECT(current_object)->ice_launcher.step_timer = 0u;
	}
	add_to_touchable_list(r0);
	depth = (uint16_t)(OBJECT(current_object)->screen_y + 0x30u);
	queue_sprite_at_depth(&ignored, &depth);
}

void ice_cube_slide(void)
{
	uint8_t bucket = 0;
	uint32_t r1 = 0, r2 = 0, cursor = 0, deleted = 0, depth = 0, direction = 0, ignored = 0, slope_height = 0, speed = 0, speed_x = 0, speed_y = 0;

	OBJECT(current_object)->health = 0x7F;
	move_object_projected(&ignored, &r2, &slope_height, &cursor);
	fall_to_floor(&ignored, &r1, &r2);
	delete_if_off_screen(&deleted, &r2, &slope_height);
	if ((uint8_t)deleted != 1u)
	{
		direction = 0x2B6u;
		speed = OBJECT(current_object)->speed;
		direction_to_speed(speed, &direction, &speed_x, &speed_y);
		OBJECT(current_object)->speed_x = speed_x;
		OBJECT(current_object)->speed_y = speed_y;
		depth = (uint16_t)(OBJECT(current_object)->screen_y - 0x10u);
		queue_sprite_at_depth(&ignored, &depth);
		current_object = LINKED(current_object, OBJECT(current_object)->child);
		OBJECT(current_object)->screen_x = OBJECT(object_cursor)->screen_x;
		OBJECT(current_object)->screen_y = OBJECT(object_cursor)->screen_y;
		OBJECT(current_object)->screen_y += 0x1Eu;
		bucket = 0xFFu;
		queue_sprite_in_bucket(bucket);
		current_object = object_cursor;
		add_to_hitter_list();
	}
}

void object_type_17_idle(void)
{
}

void padded_wall_init(void)
{
	uint16_t pad_offset_x = 0, pad_offset_y = 0;
	uint32_t block = 0, object = 0, pads_left = 0, spawned = 0;

	spawn_count = 4u;
	spawn_objects(0x011F2Bu, &spawned, &object, &block);
	if ((uint8_t)spawned == 0u)
		return;
	OBJECT(current_object)->sprite_frame = 0x77D0u;
	OBJECT(current_object)->sprite_palette = 0x91A0u;
	OBJECT(current_object)->handler = padded_wall_update_ADDRESS;
	OBJECT(current_object)->sprite_flags = 0xAu;
	pads_left = 4u;
	pad_offset_x = 0u;
	pad_offset_y = 0u;
	do
	{
		copy_position_to(object);
		OBJECT(object)->direction = 0x300u;
		OBJECT(object)->projected_x += 7u;
		OBJECT(object)->projected_y_int -= 0xAu;
		pad_offset_x += 0x26u;
		pad_offset_y += 0x13u;
		OBJECT(object)->projected_x += pad_offset_x;
		OBJECT(object)->projected_y_int += pad_offset_y;
		OBJECT(object)->sprite_frame = 0x77A8u;
		OBJECT(object)->sprite_palette = 0x9180u;
		OBJECT(object)->spawn_arg_1 = OBJECT(object_cursor)->spawn_arg_1;
		OBJECT(object)->sprite_flags = 9u;
		OBJECT(object)->flags |= 2u;
		OBJECT(object)->hitbox_size_x = 0x17u;
		OBJECT(object)->hitbox_size_y = 0x35u;
		OBJECT(object)->hitbox_offset_x = -0x6E;
		OBJECT(object)->hitbox_offset_y = 0x15;
		OBJECT(object)->hitbox_height = 0x20u;
		OBJECT(object)->kind = 0xBu;
		OBJECT(object)->handler = wall_pad_update_ADDRESS;
		current_object = LINKED(current_object, OBJECT(object)->child);
		OBJECT(object)->child = 0xFFFFu;
		object = current_object;
		pads_left--;
	} while (pads_left != 0u);
}

void padded_wall_update(void)
{
	uint32_t block_offset = 0, cursor = 0, ignored = 0, slope_height = 0;

	unproject_object(&ignored, &block_offset, &slope_height, &cursor);
	if (scroll_position <= OBJECT(current_object)->spawn_arg_1)
	{
		queue_sprite(0u);
		return;
	}
	delete_object_and_children(&ignored, &block_offset, &slope_height);
}

void wall_pad_update(void)
{
	uint32_t block_offset = 0, cursor = 0, frame_index = 0, ignored = 0, pad_step = 0, slope_height = 0;
	uint16_t frame_timer = 0;

	unproject_object(&pad_step, &block_offset, &slope_height, &cursor);
	if (OBJECT(current_object)->spawn_arg_1 < scroll_position)
	{
		delete_object_and_children(&ignored, &block_offset, &slope_height);
		return;
	}
	if (OBJECT(current_object)->wall_pad.frame_step != 0u || (OBJECT(current_object)->flags & 4u) != 0u)
	{
		frame_timer = OBJECT(current_object)->wall_pad.frame_timer;
		OBJECT(current_object)->wall_pad.frame_timer = frame_timer - 1u;
		if (frame_timer == 0u)
		{
			OBJECT(current_object)->wall_pad.frame_step++;
			OBJECT(current_object)->wall_pad.frame_step = OBJECT(current_object)->wall_pad.frame_step % 0xBu;
			pad_step = OBJECT(current_object)->wall_pad.frame_step;
			frame_index = wall_pad_frames[pad_step];
			OBJECT(current_object)->sprite_frame = wall_pad_sprites[frame_index];
			OBJECT(current_object)->wall_pad.frame_timer = wall_pad_frame_durations[pad_step];
		}
	}
	queue_sprite(frame_index);
	add_to_touchable_list(pad_step);
}
