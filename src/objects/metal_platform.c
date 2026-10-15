/**
 *    Name:        objects/metal_platform.c
 *    Description: stage 8: the carriers and the seesaw (game mode 0x1A), the metal platform and the fire wall, the fire trenches
 *    Version:     0.1.0
 *    Created:     12-10-2026
 *    Modified:    15-10-2026
 *    Author:      Sebastian Toyos
 *
 *    SegaSonic The Hedgehog (arcade, Sega System 32): the game's V60 program decompiled into C by
 *    tools/decompile.py, with the names of symbols.txt.
 */
#include "game.h"

void scene_carrier_start(void)
{
	uint32_t carrier_record = 0, sprite_frame = 0;

	carrier_record = scene_carrier_records[OBJECT(current_object)->sprite_frame] + scene_carrier_records_ADDRESS;
	OBJECT(current_object)->x_int = ROM16(carrier_record);
	OBJECT(current_object)->x_fraction = 0u;
	carrier_record += 2u;
	OBJECT(current_object)->y_int = ROM16(carrier_record);
	OBJECT(current_object)->y_fraction = 0u;
	carrier_record += 2u;
	OBJECT(current_object)->z_int = ROM16(carrier_record);
	OBJECT(current_object)->z_fraction = 0u;
	carrier_record += 2u;
	OBJECT(current_object)->speed_x = ROM32(carrier_record);
	carrier_record += 4u;
	OBJECT(current_object)->speed_y = ROM32(carrier_record);
	carrier_record += 4u;
	OBJECT(current_object)->speed_z = ROM32(carrier_record);
	OBJECT(current_object)->scene_carrier.end_position = ROM16(carrier_record + 4u);
	sprite_frame = OBJECT(current_object)->sprite_frame;
	if ((uint8_t)sprite_frame != 0u)
		player_carriers[(sprite_frame & ~0xFFu) | (uint8_t)(sprite_frame - 1u)] = current_object;
	else
		scene_object = current_object;
	OBJECT(current_object)->handler = scene_carrier_move_ADDRESS;
}

void scene_carrier_move(void)
{
	uint32_t ignored = 0;

	if (OBJECT(current_object)->scene_carrier.end_position <= scroll_position)
	{
		delete_current_object();
		return;
	}
	if (OBJECT(current_object)->sprite_frame == 0u || seesaw_launched != 0u)
	{
		OBJECT(current_object)->z += OBJECT(current_object)->speed_z;
		OBJECT(current_object)->y += OBJECT(current_object)->speed_y;
		OBJECT(current_object)->x += OBJECT(current_object)->speed_x;
	}
	project_object(&ignored, &ignored);
}

void seesaw_start(void)
{
	uint32_t free_count = 0, ignored = 0, part_list = 0;

	OBJECT(current_object)->sprite_flags = 0xD006u;
	OBJECT(current_object)->flags = 0u;
	OBJECT(current_object)->sprite_offset_x = 0;
	OBJECT(current_object)->sprite_offset_y = 0;
	allocate_object_2(&free_count, &ignored, &part_list);
	if (free_count != 0u)
	{
		move_halfwords_backward(part_list, seesaw_parts_ADDRESS, 0xDu);
		OBJECT(current_object)->part_list = part_list;
		OBJECT(current_object)->x = 0x700000;
		OBJECT(current_object)->y = 0xF680000;
		OBJECT(current_object)->z = 0x9E00000;
		project_object(&ignored, &ignored);
		OBJECT(current_object)->animation = seesaw_animation_ADDRESS;
		OBJECT(current_object)->animation_frame = 0u;
		OBJECT(current_object)->animation_speed = 2u;
		OBJECT(current_object)->animation_timer = 0;
		OBJECT(current_object)->handler = seesaw_tilt_ADDRESS;
	}
}

void seesaw_tilt(void)
{
	uint16_t part_list = 0;
	uint32_t r1 = 0, depth = 0, ignored = 0, next_step = 0, step_list = 0;
	uint8_t animation_timer = 0;
	uint16_t last_step = 0;

	if (scroll_position >= 0x4D00u)
	{
		part_list = (uint16_t)OBJECT(current_object)->part_list;
		free_object_2(part_list);
		delete_current_object();
		return;
	}
	if ((int8_t)OBJECT(current_object)->animation_speed >= 0 && seesaw_launched != 0u)
	{
		animation_timer = OBJECT(current_object)->animation_timer;
		OBJECT(current_object)->animation_timer = animation_timer - 1u;
		if ((int8_t)animation_timer > 1)
		{
			update_screen_position(&ignored, &r1);
			depth = 0u;
			queue_sprite_at_depth(&ignored, &depth);
			return;
		}
		OBJECT(current_object)->animation_timer = OBJECT(current_object)->animation_speed;
		next_step = OBJECT(current_object)->animation_frame;
		next_step = (uint8_t)(next_step + 1u);
		step_list = OBJECT(current_object)->animation;
		last_step = mem_read16(step_list);
		step_list += 2u;
		if (next_step == last_step)
			OBJECT(current_object)->animation_speed = 0xFFu;
		OBJECT(current_object)->animation_frame = next_step;
		mem_write16(OBJECT(current_object)->part_list + 0xAu, mem_read16(step_list + next_step * 2u));
	}
	update_screen_position(&ignored, &r1);
	depth = 0u;
	queue_sprite_at_depth(&ignored, &depth);
}

void metal_platform_base_start(void)
{
	uint32_t ignored = 0;

	OBJECT(current_object)->sprite_flags = 0x1002u;
	OBJECT(current_object)->sprite_frame = 0xAFB8u;
	OBJECT(current_object)->sprite_palette = 0x1230u;
	OBJECT(current_object)->flags = 0u;
	OBJECT(current_object)->sprite_offset_x = -6;
	OBJECT(current_object)->sprite_offset_y = 0x1B;
	OBJECT(current_object)->z = 0x5300000;
	project_object(&ignored, &ignored);
	OBJECT(current_object)->handler = metal_platform_base_draw_ADDRESS;
	metal_platform_base_draw();
}

void metal_platform_base_draw(void)
{
	uint16_t unseen_margin = 0;
	uint32_t depth = 0, ignored = 0, margin = 0, off_screen = 0;

	unseen_margin = 0x10u;
	margin = 0x40u;
	off_screen = check_left_screen_margins((uint16_t)margin, unseen_margin);
	if (off_screen == 0u)
	{
		update_screen_position(&ignored, &margin);
		depth = (uint16_t)(OBJECT(current_object)->screen_y - OBJECT(current_object)->sprite_offset_y - 0x28u);
		queue_sprite_at_depth(&ignored, &depth);
		return;
	}
	delete_current_object();
}

void metal_platform_start(void)
{
	uint32_t free_count = 0, ignored = 0, object = 0;

	OBJECT(current_object)->sprite_flags = 0x1002u;
	OBJECT(current_object)->sprite_frame = 0xAFC0u;
	OBJECT(current_object)->sprite_palette = 0x1230u;
	OBJECT(current_object)->flags = 0u;
	OBJECT(current_object)->sprite_offset_x = -6;
	OBJECT(current_object)->sprite_offset_y = 0x1B;
	OBJECT(current_object)->metal_platform.state = 0u;
	OBJECT(current_object)->metal_platform.timer = 0x12C;
	OBJECT(current_object)->z = 0x5300000;
	project_object(&ignored, &ignored);
	allocate_object(&free_count, &object);
	if (free_count != 0u)
	{
		OBJECT(object)->sprite_flags = 0x1001u;
		OBJECT(object)->sprite_frame = 0xAFC8u;
		OBJECT(object)->sprite_palette = 0x1230u;
		OBJECT(object)->flags = 0u;
		OBJECT(object)->sprite_offset_x = 0;
		OBJECT(object)->sprite_offset_y = 0;
		OBJECT(object)->x = 0x1310000;
		OBJECT(object)->y = 0xF240000;
		OBJECT(object)->z = 0x5300000;
		OBJECT(object)->handler = metal_platform_update_ADDRESS;
		OBJECT(object)->metal_platform.state = 3u;
		OBJECT(object)->parent = current_object;
		OBJECT(current_object)->handler = metal_platform_update_ADDRESS;
	}
}

void metal_platform_update(void)
{
	uint32_t r1 = 0, depth = 0, free_count = 0, ignored = 0, object = 0, object_2 = 0;
	uint16_t timer = 0, timer_2 = 0;

	switch (OBJECT(current_object)->metal_platform.state)
	{
	case 0:
	case 4:
		timer = OBJECT(current_object)->metal_platform.timer;
		OBJECT(current_object)->metal_platform.timer = timer - 1u;
		if ((int16_t)timer <= 1)
			OBJECT(current_object)->metal_platform.state++;
		update_screen_position(&ignored, &r1);
		depth = (uint16_t)(OBJECT(current_object)->screen_y - OBJECT(current_object)->sprite_offset_y - 0x28u);
		queue_sprite_at_depth(&ignored, &depth);
		return;
	case 1:
		OBJECT(current_object)->metal_platform.state++;
		metal_platform_lifted = 0xFFu;
		allocate_object(&free_count, &object_2);
		if (free_count != 0u)
			OBJECT(object_2)->handler = fire_wall_start_ADDRESS;
		OBJECT(current_object)->speed_z = 0x100000;
		OBJECT(current_object)->sprite_frame = 0xAFD0u;
		OBJECT(current_object)->metal_platform.frame = 0xAFD0u;
		OBJECT(current_object)->metal_platform.timer = 0xA;
		OBJECT(current_object)->floor_z = OBJECT(current_object)->z;
		OBJECT(current_object)->sprite_offset_y = 0xC;
		timer_2 = OBJECT(current_object)->metal_platform.timer;
		OBJECT(current_object)->metal_platform.timer = timer_2 - 1u;
		if ((int16_t)timer_2 <= 1)
		{
			OBJECT(current_object)->metal_platform.timer = 0;
			OBJECT(current_object)->speed_z += 0x6000u;
			OBJECT(current_object)->z -= OBJECT(current_object)->speed_z;
		}
		if (OBJECT(current_object)->screen_y <= -0x58)
		{
			delete_current_object();
			return;
		}
		if (OBJECT(current_object)->screen_y <= 0x28)
			OBJECT(current_object)->sprite_frame = OBJECT(current_object)->metal_platform.frame;
		project_object(&ignored, &ignored);
		depth = (uint16_t)(((int32_t)((uint32_t)OBJECT(current_object)->projected_y + OBJECT(current_object)->floor_z) >> 16) - view_y);
		queue_sprite_at_depth(&ignored, &depth);
		return;
	case 2:
		timer_2 = OBJECT(current_object)->metal_platform.timer;
		OBJECT(current_object)->metal_platform.timer = timer_2 - 1u;
		if ((int16_t)timer_2 <= 1)
		{
			OBJECT(current_object)->metal_platform.timer = 0;
			OBJECT(current_object)->speed_z += 0x6000u;
			OBJECT(current_object)->z -= OBJECT(current_object)->speed_z;
		}
		if (OBJECT(current_object)->screen_y <= -0x58)
		{
			delete_current_object();
			return;
		}
		if (OBJECT(current_object)->screen_y <= 0x28)
			OBJECT(current_object)->sprite_frame = OBJECT(current_object)->metal_platform.frame;
		project_object(&ignored, &ignored);
		depth = (uint16_t)(((int32_t)((uint32_t)OBJECT(current_object)->projected_y + OBJECT(current_object)->floor_z) >> 16) - view_y);
		queue_sprite_at_depth(&ignored, &depth);
		return;
	case 3:
		object = LINKED(current_object, OBJECT(current_object)->parent);
		if ((int8_t)OBJECT(object)->var_80 >= 1)
		{
			OBJECT(current_object)->sprite_flags = 0x1002u;
			OBJECT(current_object)->x = OBJECT(object)->x;
			OBJECT(current_object)->y = OBJECT(object)->y;
			OBJECT(current_object)->z = OBJECT(object)->z;
			OBJECT(current_object)->x_int += 0x2Du;
			OBJECT(current_object)->metal_platform.timer = 0xA;
			OBJECT(current_object)->sprite_offset_x = -0x24;
			OBJECT(current_object)->sprite_offset_y = 0x1C;
			OBJECT(current_object)->speed_z = 0x80000;
			OBJECT(current_object)->floor_z = OBJECT(current_object)->z;
			OBJECT(current_object)->sprite_frame = 0xAFD8u;
			OBJECT(current_object)->metal_platform.frame = 0xAFE8u;
			OBJECT(current_object)->metal_platform.state = 2u;
		}
		project_object(&ignored, &depth);
		depth = queue_sprite(depth);
		return;
	default:
		unknown_code(0x86DF9u + OBJECT(current_object)->metal_platform.state * 3u);
	}
}

void fire_wall_start(void)
{
	uint8_t sound_id = 0;
	uint32_t r1 = 0, r2 = 0, free_count = 0, ignored = 0, part_list = 0;

	OBJECT(current_object)->sprite_flags = 0xD006u;
	OBJECT(current_object)->sprite_offset_x = 0;
	OBJECT(current_object)->sprite_offset_y = 0;
	OBJECT(current_object)->flags = 2u;
	OBJECT(current_object)->hitbox_size_x = 0x5Bu;
	OBJECT(current_object)->hitbox_size_y = 0xF0u;
	OBJECT(current_object)->hitbox_height = 0x100u;
	OBJECT(current_object)->hitbox_offset_x = -0x2A;
	OBJECT(current_object)->hitbox_offset_y = -1;
	OBJECT(current_object)->kind = 0x14u;
	OBJECT(current_object)->direction = 0x100u;
	OBJECT(current_object)->speed_x = 0;
	OBJECT(current_object)->speed_y = 0;
	OBJECT(current_object)->x = 0x1000000;
	OBJECT(current_object)->y = 0xE470000;
	OBJECT(current_object)->z = 0x6100000;
	OBJECT(current_object)->animation = fire_wall_animation_ADDRESS;
	OBJECT(current_object)->animation_speed = 2u;
	OBJECT(current_object)->animation_timer = 0;
	OBJECT(current_object)->animation_frame = 0u;
	OBJECT(current_object)->speed_z = -0x100000;
	project_object(&ignored, &r1);
	OBJECT(current_object)->child = 0xFFFFu;
	allocate_object_2(&free_count, &r2, &part_list);
	if (free_count != 0u)
	{
		move_halfwords_backward(part_list, fire_wall_parts_ADDRESS, 9u);
		OBJECT(current_object)->part_list = part_list;
		sound_id = 7u;
		play_sound(sound_id, &r1, &r2);
		OBJECT(current_object)->handler = fire_wall_rise_ADDRESS;
	}
}

void fire_wall_rise(void)
{
	uint16_t part_list = 0;
	uint32_t r1 = 0, r8 = 0, depth = 0, ignored = 0, part = 0;

	if (scroll_position >= 0x6800u)
	{
		part_list = (uint16_t)OBJECT(current_object)->part_list;
		free_object_2(part_list);
		delete_current_object();
		return;
	}
	if (OBJECT(current_object)->z_int > 0x550)
	{
		OBJECT(current_object)->speed_z += 0x6000u;
		OBJECT(current_object)->z += OBJECT(current_object)->speed_z;
	}
	update_screen_position(&ignored, &r1);
	animate_all_parts(&r8, &part);
	add_to_touchable_list(ignored);
	depth = (uint16_t)(OBJECT(current_object)->screen_y + 0x70u);
	queue_sprite_at_depth(&ignored, &depth);
}

void fire_trench_start(void)
{
	uint32_t ignored = 0;

	OBJECT(current_object)->sprite_flags = 0x100Au;
	OBJECT(current_object)->sprite_frame = 0xB200u;
	OBJECT(current_object)->sprite_palette = 0x51F0u;
	OBJECT(current_object)->flags = 0u;
	OBJECT(current_object)->sprite_offset_x = 0;
	OBJECT(current_object)->sprite_offset_y = -0x16;
	OBJECT(current_object)->fire_trench_start.flame_timer = 0;
	OBJECT(current_object)->handler = fire_trench_update_ADDRESS;
	OBJECT(current_object)->z = 0x5300000;
	project_object(&ignored, &ignored);
}

void fire_trench_update(void)
{
	uint32_t depth = 0, ignored = 0, margin = 0, off_screen = 0;
	uint16_t flame_timer = 0;

	margin = 0x70u;
	off_screen = check_left_screen_margin((uint16_t)margin);
	if (off_screen != 0u)
	{
		delete_current_object();
		return;
	}
	flame_timer = OBJECT(current_object)->fire_trench_update.flame_timer;
	OBJECT(current_object)->fire_trench_update.flame_timer = flame_timer - 1u;
	if ((int16_t)flame_timer <= 1)
	{
		OBJECT(current_object)->fire_trench_update.flame_timer = 0x12C;
		spawn_fire_trench_flames();
	}
	update_screen_position(&ignored, &margin);
	depth = (uint16_t)(OBJECT(current_object)->screen_y - OBJECT(current_object)->sprite_offset_y - 0x20u);
	queue_sprite_at_depth(&ignored, &depth);
}

void spawn_fire_trench_flames(void)
{
	uint8_t sound_id = 0;
	uint32_t ignored = 0, object = 0, write_index = 0, write_index_address = 0;
	uint16_t flames_left_before = 0;

	if (free_object_stack < 3)
		return;
	trench_flame_y = 0xE47u;
	trench_flames_left = 3;
	do
	{
		allocate_object(&ignored, &object);
		OBJECT(object)->sprite_flags = 0x1006u;
		OBJECT(object)->sprite_palette = 0x5200u;
		OBJECT(object)->flags = 2u;
		OBJECT(object)->sprite_offset_x = -8;
		OBJECT(object)->sprite_offset_y = 0x3E;
		OBJECT(object)->hitbox_offset_x = 0;
		OBJECT(object)->hitbox_offset_y = 0;
		OBJECT(object)->kind = 0x14u;
		OBJECT(object)->direction = 0x100u;
		OBJECT(object)->hitbox_size_x = 0x40u;
		OBJECT(object)->hitbox_size_y = 0x80u;
		OBJECT(object)->handler = fire_trench_flame_burn_ADDRESS;
		OBJECT(object)->x = OBJECT(current_object)->x;
		OBJECT(object)->y_int = trench_flame_y;
		OBJECT(object)->y_fraction = 0u;
		OBJECT(object)->z = 0x5300000;
		trench_flame_y += 0x50u;
		OBJECT(object)->animation = fire_trench_flame_animation_ADDRESS;
		OBJECT(object)->animation_frame = 0xFFu;
		OBJECT(object)->animation_timer = 0;
		OBJECT(object)->animation_speed = 2u;
		flames_left_before = trench_flames_left;
		trench_flames_left = flames_left_before - 1u;
	} while ((int16_t)flames_left_before > 1);
	if (OBJECT(current_object)->screen_x <= 0x140)
	{
		sound_id = 7u;
		play_sound(sound_id, &write_index, &write_index_address);
	}
}

void fire_trench_flame_burn(void)
{
	uint32_t r7 = 0, at_last_step = 0, depth = 0, frame = 0, ignored = 0;

	animate(&frame, &r7, &at_last_step);
	if ((uint8_t)at_last_step == 0u)
	{
		OBJECT(current_object)->hitbox_height = fire_trench_flame_heights[OBJECT(current_object)->animation_frame];
		project_object(&ignored, &ignored);
		add_to_touchable_list(ignored);
		depth = (uint16_t)(OBJECT(current_object)->screen_y - OBJECT(current_object)->sprite_offset_y);
		queue_sprite_at_depth(&ignored, &depth);
		return;
	}
	delete_current_object();
}
