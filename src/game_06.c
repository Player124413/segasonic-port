/**
 *    Name:        game_06.c
 *    Description: the game's functions, part 06 (in the order of their V60 addresses)
 *    Version:     0.1.0
 *    Created:     12-10-2026
 *    Modified:    14-10-2026
 *    Author:      Sebastian Toyos
 *
 *    SegaSonic The Hedgehog (arcade, Sega System 32): the game's V60 program decompiled into C by
 *    tools/decompile.py, with the names of symbols.txt.
 */
#include "game.h"

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

void arrow_board_start(void)
{
	uint32_t ignored = 0;

	OBJECT(current_object)->sprite_flags = 0x1002u;
	OBJECT(current_object)->sprite_frame = 0x6EB8u;
	OBJECT(current_object)->sprite_palette = 0x9190u;
	OBJECT(current_object)->sprite_offset_x = 0;
	OBJECT(current_object)->sprite_offset_y = 0;
	OBJECT(current_object)->flags = 0u;
	OBJECT(current_object)->handler = arrow_board_wait_for_platform_ADDRESS;
	project_object(&ignored, &ignored);
	arrow_board_wait_for_platform();
}

void arrow_board_wait_for_platform(void)
{
	if ((OBJECT(current_object)->flags & 8u) != 0u)
		OBJECT(current_object)->handler = arrow_board_ride_ADDRESS;
	add_to_rider_list();
}

void arrow_board_ride(void)
{
	uint32_t r1 = 0, depth = 0, ignored = 0, off_screen = 0, platform_z = 0;

	if (grey_screen_active == 0u)
	{
		if (grey_screen_ended != 0u)
			OBJECT(current_object)->sprite_palette = 0x9190u;
	}
	else
		OBJECT(current_object)->sprite_palette = 0x8400u;
	platform_z = OBJECT(LINKED(current_object, OBJECT(current_object)->platform))->z;
	OBJECT(current_object)->floor_z = platform_z;
	OBJECT(current_object)->z = platform_z;
	off_screen = check_left_screen();
	if (off_screen == 0u)
	{
		update_screen_position(&ignored, &r1);
		depth = (uint16_t)(OBJECT(current_object)->screen_y - 0x20u);
		queue_sprite_at_depth(&ignored, &depth);
		return;
	}
	delete_current_object();
}

void pebble_start(void)
{
	uint32_t block_offset = 0, cursor = 0, ignored = 0, x = 0, y = 0;

	OBJECT(current_object)->sprite_flags = 0x1004u;
	OBJECT(current_object)->sprite_frame = 0x6C60u;
	OBJECT(current_object)->sprite_palette = 0x9020u;
	OBJECT(current_object)->sprite_offset_x = 0;
	OBJECT(current_object)->sprite_offset_y = 0;
	OBJECT(current_object)->flags = 0u;
	OBJECT(current_object)->handler = pebble_update_ADDRESS;
	find_floor(&x, &y, &block_offset, &ignored, &cursor);
	OBJECT(current_object)->z = OBJECT(current_object)->floor_z;
	project_object(&ignored, &ignored);
}

void pebble_update(void)
{
	uint32_t r1 = 0, depth = 0, ignored = 0, off_screen = 0;

	off_screen = check_left_screen();
	if (off_screen == 0u)
	{
		update_screen_position(&ignored, &r1);
		depth = (uint16_t)(OBJECT(current_object)->screen_y - 0x10u);
		queue_sprite_at_depth(&ignored, &depth);
		return;
	}
	delete_current_object();
}

void propped_board_start(void)
{
	uint16_t prop_dy = 0;
	uint32_t ignored = 0, object = 0;
	uint16_t props_left_before = 0;

	OBJECT(current_object)->sprite_flags = 0x1000u;
	OBJECT(current_object)->sprite_frame = 0x6EA8u;
	OBJECT(current_object)->sprite_palette = 0x9100u;
	OBJECT(current_object)->hitbox_size_x = 0x10u;
	OBJECT(current_object)->hitbox_size_y = 0x12Bu;
	OBJECT(current_object)->hitbox_height = 0xE0u;
	OBJECT(current_object)->sprite_offset_x = 0;
	OBJECT(current_object)->sprite_offset_y = -0x2C;
	OBJECT(current_object)->flags = 2u;
	OBJECT(current_object)->propped_board.shake_strength = 0;
	OBJECT(current_object)->propped_board.shake_dx = 0;
	OBJECT(current_object)->hitbox_offset_x = 0;
	OBJECT(current_object)->hitbox_offset_y = -0x80;
	OBJECT(current_object)->speed_z = 0;
	OBJECT(current_object)->z = 0x2300000;
	OBJECT(current_object)->kind = 0x11u;
	OBJECT(current_object)->handler = propped_board_update_ADDRESS;
	propped_board_object = current_object;
	project_object(&ignored, &ignored);
	if (free_object_stack < 2)
		return;
	board_props_left = 2;
	do
	{
		allocate_object(&ignored, &object);
		OBJECT(object)->sprite_flags = 0x1006u;
		OBJECT(object)->sprite_frame = 0x6EC0u;
		OBJECT(object)->sprite_palette = 0x91B0u;
		OBJECT(object)->sprite_offset_x = 0;
		OBJECT(object)->sprite_offset_y = 6;
		OBJECT(object)->flags = 2u;
		OBJECT(object)->hitbox_size_x = 0x30u;
		OBJECT(object)->hitbox_size_y = 0x20u;
		OBJECT(object)->hitbox_height = 0x50u;
		OBJECT(object)->hitbox_offset_x = -2;
		OBJECT(object)->hitbox_offset_y = -0xC;
		OBJECT(object)->kind = 0xCu;
		OBJECT(object)->x = OBJECT(current_object)->x;
		OBJECT(object)->x_int -= 0x20u;
		OBJECT(object)->y = OBJECT(current_object)->y;
		OBJECT(object)->z = OBJECT(current_object)->z;
		prop_dy = 0xFF90u;
		if ((uint16_t)board_props_left != 2u)
			prop_dy = (uint16_t)(0u - prop_dy);
		OBJECT(object)->y_int += prop_dy;
		OBJECT(object)->handler = board_prop_start_ADDRESS;
		props_left_before = board_props_left;
		board_props_left = props_left_before - 1u;
	} while ((int16_t)props_left_before > 1);
}

void propped_board_update(void)
{
	uint32_t r1 = 0, depth = 0, ignored = 0, shake_dx = 0;
	uint16_t shake_strength = 0;
	uint32_t flags = 0;

	OBJECT(current_object)->health = 0xA;
	if ((uint16_t)(OBJECT(current_object)->screen_x + 0x88u) >= 0x250u)
	{
		if ((OBJECT(current_object)->flags & 0x800u) != 0u)
		{
			delete_current_object();
			return;
		}
	}
	else
		OBJECT(current_object)->flags |= 0x800u;
	if (OBJECT(current_object)->x_int <= OBJECT(LINKED(current_object, group_leader_1))->x_int)
		OBJECT(current_object)->speed_z += 0x2000u;
	OBJECT(current_object)->z += OBJECT(current_object)->speed_z;
	update_screen_position(&shake_dx, &r1);
	if ((uint32_t)OBJECT(current_object)->speed_z == 0u)
		OBJECT(current_object)->propped_board.draw_depth = OBJECT(current_object)->screen_y - 0x10u;
	if (OBJECT(current_object)->screen_y >= 0x180)
	{
		delete_current_object();
		return;
	}
	if ((OBJECT(current_object)->flags & 4u) != 0u)
	{
		AS8(OBJECT(current_object)->propped_board.shake_strength) = 0x3Cu;
		flags = OBJECT(current_object)->flags;
		OBJECT(current_object)->flags = flags | 1u;
		if ((flags & 1u) == 0u)
			release_board_rings();
	}
	if ((uint16_t)OBJECT(current_object)->propped_board.shake_strength == 0u)
	{
		add_to_touchable_list(shake_dx);
		depth = OBJECT(current_object)->propped_board.draw_depth;
		queue_sprite_at_depth(&ignored, &depth);
		return;
	}
	shake_dx = (uint16_t)OBJECT(current_object)->propped_board.shake_strength >> 4;
	OBJECT(current_object)->propped_board.shake_step++;
	if ((OBJECT(current_object)->propped_board.shake_step & 1u) == 0u)
		shake_dx = (uint16_t)(0u - shake_dx);
	OBJECT(current_object)->screen_x += shake_dx;
	OBJECT(current_object)->propped_board.shake_dx = shake_dx;
	shake_strength = OBJECT(current_object)->propped_board.shake_strength;
	OBJECT(current_object)->propped_board.shake_strength = shake_strength - 2u;
	if ((int16_t)shake_strength <= 2)
		OBJECT(current_object)->propped_board.shake_strength = 0;
	add_to_touchable_list(shake_dx);
	depth = OBJECT(current_object)->propped_board.draw_depth;
	queue_sprite_at_depth(&ignored, &depth);
}

void board_prop_start(void)
{
	uint32_t ignored = 0;

	project_object(&ignored, &ignored);
	OBJECT(current_object)->handler = board_prop_update_ADDRESS;
}

void board_prop_update(void)
{
	uint32_t r1 = 0, depth = 0, ignored = 0, object = 0, off_screen = 0;

	off_screen = check_left_screen();
	if (off_screen != 0u)
	{
		delete_current_object();
		return;
	}
	OBJECT(current_object)->z += OBJECT(LINKED(current_object, propped_board_object))->speed_z;
	update_screen_position(&ignored, &r1);
	object = LINKED(current_object, propped_board_object);
	if ((uint32_t)OBJECT(object)->speed_z == 0u)
		OBJECT(current_object)->board_prop.draw_depth = OBJECT(current_object)->screen_y - 0xBu;
	OBJECT(current_object)->screen_x += AS16(OBJECT(object)->var_80);
	add_to_touchable_list(ignored);
	depth = OBJECT(current_object)->board_prop.draw_depth;
	queue_sprite_at_depth(&ignored, &depth);
}

void release_board_rings(void)
{
	uint32_t allocated = 0, count = 0, random_value = 0;
	uint32_t saved_x = 0, saved_y = 0, saved_z = 0;
	uint16_t rings_left_before = 0;

	saved_x = OBJECT(current_object)->x;
	saved_y = OBJECT(current_object)->y;
	saved_z = OBJECT(current_object)->z;
	OBJECT(current_object)->y_int -= 0x40u;
	OBJECT(current_object)->x_int -= 0x40u;
	OBJECT(current_object)->z_int -= 0x90u;
	board_rings_left = 8;
	do
	{
		count = 1u;
		scatter_rings_fixed(count, &random_value, &allocated);
		OBJECT(current_object)->y_int += 0x20u;
		rings_left_before = board_rings_left;
		board_rings_left = rings_left_before - 1u;
	} while ((int16_t)rings_left_before > 1);
	OBJECT(current_object)->z = saved_z;
	OBJECT(current_object)->y = saved_y;
	OBJECT(current_object)->x = saved_x;
}

void boulder_start(void)
{
	OBJECT(current_object)->sprite_flags = 4u;
	OBJECT(current_object)->sprite_palette = 0x90F0u;
	OBJECT(current_object)->sprite_scale = 0x200u;
	OBJECT(current_object)->hitbox_size_x = 0x80u;
	OBJECT(current_object)->hitbox_size_y = 0xC0u;
	OBJECT(current_object)->hitbox_height = 0x60u;
	OBJECT(current_object)->kind = 9u;
	OBJECT(current_object)->sprite_offset_x = 0x20;
	OBJECT(current_object)->sprite_offset_y = 0x10;
	OBJECT(current_object)->flags = 2u;
	OBJECT(current_object)->speed_z = 0;
	OBJECT(current_object)->screen_y = 0;
	OBJECT(current_object)->hitbox_offset_x = -0x20;
	OBJECT(current_object)->hitbox_offset_y = -0x60;
	OBJECT(current_object)->direction = 0x300u;
	OBJECT(current_object)->animation = boulder_animation_ADDRESS;
	OBJECT(current_object)->animation_frame = 0u;
	OBJECT(current_object)->animation_timer = 0;
	OBJECT(current_object)->animation_speed = 4u;
	OBJECT(current_object)->handler = boulder_wait_ADDRESS;
	OBJECT(current_object)->boulder.wait_timer = 0x78;
	OBJECT(current_object)->speed = 0x30000;
	group_leader_0_held = 0xFFu;
	add_to_rider_list();
}

void boulder_wait(void)
{
	uint16_t wait_timer = 0;

	if (scroll_position >= 0x3B00u)
	{
		OBJECT(current_object)->handler = boulder_roll_ADDRESS;
		boulder_roll();
		return;
	}
	if (scroll_position < 0x3800u)
	{
		add_to_rider_list();
		return;
	}
	wait_timer = OBJECT(current_object)->boulder.wait_timer;
	OBJECT(current_object)->boulder.wait_timer = wait_timer - 1u;
	if ((int16_t)wait_timer <= 1)
	{
		OBJECT(current_object)->handler = boulder_roll_ADDRESS;
		boulder_roll();
		return;
	}
	add_to_rider_list();
}

void boulder_roll(void)
{
	uint32_t r1 = 0, r2 = 0, r7 = 0, r8 = 0, cursor = 0, ignored = 0, sound_id = 0;

	OBJECT(current_object)->health = 0x7F;
	if ((OBJECT(current_object)->flags & 1u) == 0u && OBJECT(current_object)->x_int <= 0x180)
	{
		OBJECT(current_object)->flags |= 1u;
		camera_shake_active = 1u;
		sound_id = 0xFu;
		play_sound((uint8_t)sound_id, &r1, &r2);
	}
	group_leader_0_held = 0u;
	OBJECT(current_object)->x = OBJECT(LINKED(current_object, group_leaders[0]))->x;
	OBJECT(current_object)->x_int -= 0x30u;
	OBJECT(current_object)->speed_x = 0u - ((uint32_t)(uint16_t)chaser_speed << 12);
	OBJECT(current_object)->speed_y = 0;
	find_floor(&sound_id, &r1, &r2, &ignored, &cursor);
	if ((int16_t)AS16(OBJECT(current_object)->floor_block) < 0)
	{
		OBJECT(current_object)->speed_z += 0x6000u;
		OBJECT(current_object)->z += OBJECT(current_object)->speed_z;
	}
	else
	{
		OBJECT(current_object)->z = OBJECT(current_object)->floor_z;
		if ((uint16_t)OBJECT(current_object)->z_int == 0x7FFu)
			OBJECT(current_object)->z_int = 0x230;
	}
	animate(&r1, &r7, &r8);
	project_object(&ignored, &ignored);
	if (OBJECT(current_object)->screen_y < 0x180)
	{
		add_to_touchable_list(ignored);
		add_to_rider_list();
		ignored = queue_sprite_at_previous_floor();
		return;
	}
	delete_current_object();
}

void door_wall_start(void)
{
	uint32_t block_offset = 0, cursor = 0, ignored = 0, x = 0, y = 0;

	OBJECT(current_object)->sprite_flags = 0x1000u;
	OBJECT(current_object)->sprite_palette = 0x9110u;
	OBJECT(current_object)->sprite_frame = 0x6EB0u;
	OBJECT(current_object)->door_wall.fragment = door_fragments_ADDRESS;
	OBJECT(current_object)->door_wall.pieces_left = 0x10;
	OBJECT(current_object)->sprite_offset_x = 0;
	OBJECT(current_object)->sprite_offset_y = 0;
	OBJECT(current_object)->flags = 0u;
	OBJECT(current_object)->x = -0xE2E0000;
	OBJECT(current_object)->y = 0xF310000;
	OBJECT(current_object)->handler = door_wall_wait_to_burst_ADDRESS;
	find_floor(&x, &y, &block_offset, &ignored, &cursor);
	OBJECT(current_object)->z = OBJECT(current_object)->floor_z;
	project_object(&ignored, &ignored);
}

void door_wall_wait_to_burst(void)
{
	uint32_t carry = 0;
	uint32_t r1 = 0, fragment_animation = 0, free_count = 0, ignored = 0, object = 0, random_bits = 0, random_offsets = 0;
	uint8_t pieces_left = 0;

	if ((int8_t)scene_step < 3)
	{
		update_screen_position(&ignored, &r1);
		r1 = queue_sprite(r1);
		return;
	}
	for (;;)
	{
		pieces_left = OBJECT(current_object)->door_wall.pieces_left;
		OBJECT(current_object)->door_wall.pieces_left = pieces_left - 1u;
		if ((int8_t)pieces_left < 1)
			break;
		allocate_object(&free_count, &object);
		if (free_count == 0u)
			break;
		OBJECT(object)->screen_x = OBJECT(current_object)->screen_x;
		OBJECT(object)->screen_y = OBJECT(current_object)->screen_y;
		next_random(&random_bits, &ignored, &ignored);
		random_offsets = (random_bits & ~0xFFFFu) | (random_bits & 0x3Fu);
		OBJECT(object)->screen_x += random_offsets;
		OBJECT(object)->screen_y += (random_offsets >> 16) & 0x3Fu;
		random_bits = (random_bits & ~0xFFFFu) | (random_bits & 0xFFu);
		random_bits = (random_bits & ~0xFFFFu) | (uint16_t)(random_bits + 4u);
		OBJECT(object)->speed_x_fraction = random_bits;
		carry = (random_bits >> 16) & 1u;
		random_bits = random_bits >> 17;
		if (carry)
		{
			random_bits = random_bits & 0xFFu;
			random_bits = (uint16_t)(0u - random_bits);
			random_bits = (uint16_t)(random_bits - 0x10u);
		}
		else
		{
			random_bits = random_bits & 0xFFu;
			random_bits = (uint16_t)(random_bits + 4u);
		}
		OBJECT(object)->speed_y_fraction = random_bits;
		fragment_animation = OBJECT(current_object)->door_wall.fragment + 8u;
		OBJECT(object)->animation = rock_debris_animations_ADDRESS + mem_read16(fragment_animation);
		OBJECT(current_object)->door_wall.fragment = fragment_animation + 2u;
		OBJECT(object)->sprite_flags = 0u;
		OBJECT(object)->sprite_palette = 0x8240u;
		OBJECT(object)->projected_x = (uint32_t)OBJECT(object)->screen_x << 4;
		OBJECT(object)->projected_y_fraction = (uint32_t)OBJECT(object)->screen_y << 4;
		OBJECT(object)->animation_frame = 0u;
		OBJECT(object)->animation_speed = 4u;
		OBJECT(object)->animation_timer = 0;
		OBJECT(object)->flags = 0u;
		OBJECT(object)->sprite_scale = 0x100u;
		OBJECT(object)->handler = rock_debris_fly_ADDRESS;
	}
	delete_current_object();
}

void spawn_door_rollers(uint32_t *p_block_offset, uint32_t *p_slope_height, uint32_t *p_object)
{
	uint32_t block_offset = *p_block_offset;
	uint32_t slope_height = *p_slope_height;
	uint32_t object = *p_object;
	uint32_t free_count = 0, layout_record = 0, y = 0;
	uint16_t rollers_left_before = 0;

	door_rollers_left = 4;
	do
	{
		allocate_object_outside_loop(&free_count, &object);
		if (free_count == 0u)
		{
			*p_block_offset = block_offset;
			*p_slope_height = slope_height;
			*p_object = object;
			return;
		}
		current_object = object;
		layout_record = door_roller_layout_ADDRESS + (uint16_t)door_rollers_left * 3u * 2u;
		OBJECT(current_object)->x_int = ROM16(layout_record);
		OBJECT(current_object)->x_fraction = 0u;
		layout_record += 2u;
		OBJECT(current_object)->y_int = ROM16(layout_record);
		OBJECT(current_object)->y_fraction = 0u;
		OBJECT(current_object)->animation_frame = ROM8(layout_record + 2u);
		OBJECT(current_object)->sprite_flags = 0x1004u;
		OBJECT(current_object)->sprite_palette = 0x82F0u;
		OBJECT(current_object)->hitbox_size_x = 0x10u;
		OBJECT(current_object)->hitbox_size_y = 0x10u;
		OBJECT(current_object)->hitbox_height = 0x28u;
		OBJECT(current_object)->kind = 4u;
		OBJECT(current_object)->handler = door_roller_update_ADDRESS;
		OBJECT(current_object)->sprite_offset_x = 0;
		OBJECT(current_object)->sprite_offset_y = 5;
		OBJECT(current_object)->flags = 2u;
		OBJECT(current_object)->speed_z = -0x80000;
		OBJECT(current_object)->var_80 = 0u;
		OBJECT(current_object)->var_82 = 8u;
		AT8(OBJECT(current_object)->var_82, 1) = 0u;
		OBJECT(current_object)->animation_speed = 4u;
		OBJECT(current_object)->animation_timer = 0;
		OBJECT(current_object)->animation = spiked_roller_animation_ADDRESS;
		OBJECT(current_object)->sprite_frame = spiked_roller_frames[OBJECT(current_object)->animation_frame];
		find_floor(&free_count, &y, &block_offset, &slope_height, &object);
		OBJECT(current_object)->z = OBJECT(current_object)->floor_z;
		rollers_left_before = door_rollers_left;
		door_rollers_left = rollers_left_before - 1u;
	} while ((int16_t)rollers_left_before > 1);
	*p_block_offset = block_offset;
	*p_slope_height = slope_height;
	*p_object = object;
}

void door_roller_update(void)
{
	uint32_t r7 = 0, r8 = 0, frame = 0, ignored = 0;

	switch (OBJECT(current_object)->door_roller.state)
	{
	case 0:
		door_roller_wait();
		break;
	case 1:
		door_roller_drop();
		break;
	case 2:
		door_roller_roll_out();
		break;
	case 3:
		door_roller_ride();
		break;
	case 4:
		door_roller_wait();
		break;
	default:
		unknown_code(0x82AC6u + OBJECT(current_object)->door_roller.state * 3u);
	}
	if (AT8(OBJECT(current_object)->var_82, 1) != 0u)
	{
		delete_current_object();
		return;
	}
	animate(&frame, &r7, &r8);
	project_object(&ignored, &ignored);
	if (OBJECT(current_object)->var_80 == 0u)
		return;
	ignored = queue_sprite_at_base(ignored);
}

void door_roller_wait(void)
{
	OBJECT(current_object)->health = 0x7F;
	if ((int8_t)scene_step >= 3)
		OBJECT(current_object)->door_roller.state++;
}

void door_roller_drop(void)
{
	uint32_t r1 = 0, r7 = 0, r8 = 0, block_offset = 0, cursor = 0, ignored = 0, x = 0;

	OBJECT(current_object)->health = 0x7F;
	OBJECT(current_object)->speed_z += 0x6000u;
	OBJECT(current_object)->z += OBJECT(current_object)->speed_z;
	OBJECT(current_object)->y_int += 5u;
	animate(&r1, &r7, &r8);
	find_floor(&x, &r1, &block_offset, &ignored, &cursor);
	if (OBJECT(current_object)->z_int < OBJECT(current_object)->floor_z_int)
	{
		add_to_rider_list();
		return;
	}
	OBJECT(current_object)->door_roller.state++;
	OBJECT(current_object)->z = OBJECT(current_object)->floor_z;
	if (OBJECT(current_object)->z_int >= 0x800)
	{
		delete_current_object();
		return;
	}
	camera_shake_active = 1u;
	add_to_rider_list();
}

void door_roller_roll_out(void)
{
	uint16_t roll_out_timer = 0;

	OBJECT(current_object)->y_int++;
	roll_out_timer = OBJECT(current_object)->door_roller_roll_out.roll_out_timer;
	OBJECT(current_object)->door_roller_roll_out.roll_out_timer = roll_out_timer - 1u;
	if ((int16_t)roll_out_timer <= 1)
	{
		OBJECT(current_object)->flags |= 0x400000u;
		OBJECT(current_object)->door_roller_roll_out.state++;
	}
}

void door_roller_ride(void)
{
	uint32_t z = 0;

	if (OBJECT(current_object)->screen_y < 0x130)
	{
		z = OBJECT(LINKED(current_object, OBJECT(current_object)->platform))->z;
		OBJECT(current_object)->floor_z = z;
		OBJECT(current_object)->z = z;
		add_to_rider_list();
		return;
	}
	OBJECT(current_object)->door_roller.off_screen = 0xFFu;
}

void fireball_emitter_start(void)
{
	uint32_t block_offset = 0, ignored = 0, list_cursor = 0, x = 0, y = 0;

	list_cursor = fireball_emitter_lists[OBJECT(current_object)->sprite_frame] + fireball_emitter_lists_ADDRESS;
	OBJECT(current_object)->x_int = ROM16(list_cursor);
	OBJECT(current_object)->x_fraction = 0u;
	list_cursor += 2u;
	OBJECT(current_object)->y_int = ROM16(list_cursor);
	OBJECT(current_object)->y_fraction = 0u;
	list_cursor += 2u;
	OBJECT(current_object)->floor_z_int = ROM16(list_cursor);
	OBJECT(current_object)->floor_z_fraction = 0u;
	OBJECT(current_object)->animation = list_cursor + 2u;
	OBJECT(current_object)->flags = 0u;
	find_floor(&x, &y, &block_offset, &ignored, &list_cursor);
	OBJECT(current_object)->handler = fireball_emitter_update_ADDRESS;
}

void fireball_emitter_update(void)
{
	uint16_t scale = 0;
	uint32_t floor_slope = 0, ignored = 0, list_record = 0, object = 0, slope_direction = 0, slope_factor = 0, slope_speed = 0, type_record = 0;

	project_object(&slope_direction, &floor_slope);
	if (OBJECT(current_object)->screen_y >= -0x40)
	{
		slope_factor = 0u;
		if ((OBJECT(current_object)->floor_slope | OBJECT(current_object)->floor_type) != 0u)
		{
			floor_slope = OBJECT(current_object)->floor_slope;
			slope_factor = floor_slope_factors[(uint16_t)(floor_slope * 2u)];
		}
		compute_camera_slope_speed(0u, slope_factor);
		slope_direction = camera_slope_direction;
		slope_speed = camera_slope_speed >> 14;
		OBJECT(current_object)->x += (int32_t)(sine_table[slope_direction] * slope_speed) >> 7;
		OBJECT(current_object)->y -= (int32_t)(cosine_table[slope_direction] * slope_speed) >> 7;
	}
	find_floor(&slope_direction, &floor_slope, &slope_speed, &ignored, &object);
	OBJECT(current_object)->z = OBJECT(current_object)->floor_z;
	if (mem_read16(OBJECT(current_object)->animation) <= scroll_position)
	{
		if (free_object_stack <= 0 || free_object_stack_2[0] <= 0)
			return;
		allocate_object(&ignored, &object);
		OBJECT(object)->handler = bouncing_fireball_update_ADDRESS;
		OBJECT(object)->flags = 0u;
		OBJECT(object)->hitbox_size_y = 0u;
		OBJECT(object)->kind = 0x17u;
		OBJECT(object)->z = OBJECT(current_object)->floor_z;
		OBJECT(object)->y = OBJECT(current_object)->y;
		list_record = OBJECT(current_object)->animation + 2u;
		OBJECT(object)->x_int = mem_read16(list_record);
		OBJECT(object)->x_fraction = 0u;
		list_record += 2u;
		OBJECT(object)->y_int += mem_read16(list_record);
		list_record += 2u;
		OBJECT(object)->speed_y = (uint32_t)(int16_t)mem_read16(list_record) << 8;
		OBJECT(object)->speed_x = 0;
		list_record += 2u;
		OBJECT(object)->speed_z = (uint32_t)(int16_t)mem_read16(list_record) << 8;
		type_record = mem_read16(list_record + 2u) + fireball_types_ADDRESS;
		OBJECT(object)->sprite_flags = ROM16(type_record);
		type_record += 2u;
		OBJECT(object)->sprite_palette = ROM16(type_record);
		type_record += 2u;
		OBJECT(object)->sprite_scale = ROM16(type_record);
		type_record += 2u;
		OBJECT(object)->hitbox_height = ROM16(type_record);
		type_record += 2u;
		OBJECT(object)->hitbox_size_x = ROM16(type_record);
		type_record += 2u;
		OBJECT(object)->sprite_offset_x = ROM8(type_record);
		type_record++;
		OBJECT(object)->sprite_offset_y = ROM8(type_record);
		type_record++;
		AS8(OBJECT(object)->hitbox_size_y) = ROM8(type_record);
		type_record++;
		AT8(OBJECT(object)->hitbox_size_y, 1) = ROM8(type_record);
		OBJECT(object)->animation = ROM16(type_record + 1u) + 0x83329u;
		OBJECT(object)->animation_frame = 0u;
		OBJECT(object)->animation_timer = 0;
		OBJECT(object)->animation_speed = 4u;
		current_object = object;
		scale = 0x180u;
		if (OBJECT(current_object)->hitbox_size_x == 0u)
			scale = 0x30u;
		create_child_shadow(scale, object);
		current_object = object_cursor;
		OBJECT(current_object)->animation += 0xCu;
		if ((int16_t)mem_read16(OBJECT(current_object)->animation + 0xAu) < 0)
		{
			delete_current_object();
			return;
		}
	}
	add_to_rider_list();
}

void bouncing_fireball_update(void)
{
	uint8_t sound_id = 0;
	uint16_t child = 0, shadow_dx = 0, shadow_dy = 0;
	uint32_t r1 = 0, r2 = 0, r7 = 0, cursor = 0, depth = 0, ignored = 0, off_screen = 0, x = 0;
	uint8_t blink_phase = 0;

	OBJECT(current_object)->x += OBJECT(current_object)->speed_x;
	OBJECT(current_object)->y += OBJECT(current_object)->speed_y;
	find_floor(&x, &r1, &r2, &ignored, &cursor);
	OBJECT(current_object)->speed_z += 0x6000u;
	OBJECT(current_object)->z += OBJECT(current_object)->speed_z;
	if (OBJECT(current_object)->floor_z <= OBJECT(current_object)->z)
	{
		OBJECT(current_object)->speed_z = 0u - OBJECT(current_object)->speed_z;
		OBJECT(current_object)->speed_z = (int32_t)OBJECT(current_object)->speed_z >> 1;
		OBJECT(current_object)->z = OBJECT(current_object)->floor_z;
		if (OBJECT(current_object)->speed_z_int <= -2)
		{
			sound_id = 0x21u;
			play_sound(sound_id, &r1, &r2);
		}
	}
	project_object(&ignored, &r1);
	off_screen = check_left_screen();
	if (off_screen == 0u)
	{
		animate(&r1, &r7, &off_screen);
		if (OBJECT(current_object)->hitbox_size_x != 0u)
			add_to_touchable_list(ignored);
		OBJECT(current_object)->flags &= ~0x10u;
		if ((OBJECT(current_object)->flags & 8u) != 0u)
			OBJECT(current_object)->flags |= 0x10u;
		add_to_rider_list();
		queue_sprite(r1);
		shadow_dx = 8u;
		shadow_dy = 0xFFFDu;
		if (OBJECT(current_object)->hitbox_size_x == 0u)
		{
			shadow_dx = 0u;
			shadow_dy = 0xFFFDu;
		}
		if (OBJECT(current_object)->floor_block < 0 || (int16_t)OBJECT(current_object)->child < 0)
			return;
		current_object = LINKED(current_object, OBJECT(object_cursor)->child);
		OBJECT(current_object)->screen_x = OBJECT(object_cursor)->projected_x - view_x + shadow_dx;
		OBJECT(current_object)->screen_y = ((int32_t)((uint32_t)OBJECT(object_cursor)->projected_y + OBJECT(object_cursor)->floor_z) >> 16) - view_y + shadow_dy;
		OBJECT(current_object)->sprite_palette = 0x9FFEu;
		if ((OBJECT(object_cursor)->flags & 0x10u) == 0u)
		{
			depth = (uint16_t)(OBJECT(current_object)->screen_y - 0x10u);
			queue_sprite_at_depth(&ignored, &depth);
			current_object = (current_object & ~0xFFFFu) | (uint16_t)object_cursor;
		}
		else
		{
			OBJECT(current_object)->sprite_palette = 0x87A0u;
			blink_phase = ~AS8(OBJECT(current_object)->hitbox_size_x);
			AS8(OBJECT(current_object)->hitbox_size_x) = blink_phase;
			if (blink_phase == 0u)
			{
				depth = (uint16_t)(OBJECT(current_object)->screen_y - 0x10u);
				queue_sprite_at_depth(&ignored, &depth);
				current_object = (current_object & ~0xFFFFu) | (uint16_t)object_cursor;
				return;
			}
		}
		return;
	}
	child = OBJECT(current_object)->child;
	if ((int16_t)child >= 0)
		free_object_2(child);
	delete_current_object();
}

void bouncing_fireball_update_spare_return(void)
{
}

void bouncing_fireball_knocked_away(void)
{
	uint16_t turn = 0;
	uint32_t toucher = 0;

	OBJECT(current_object)->speed_z = -0x80000;
	toucher = LINKED(current_object, OBJECT(current_object)->touched_by);
	OBJECT(current_object)->direction = OBJECT(toucher)->direction;
	OBJECT(current_object)->speed = OBJECT(toucher)->speed;
	turn = 0x80u;
	if ((OBJECT(current_object)->speed_int & 1u) != 0u)
		turn = (uint16_t)(0u - turn);
	OBJECT(current_object)->direction += turn;
	OBJECT(current_object)->direction &= 0x3FFu;
	set_speed_from_direction();
	OBJECT(current_object)->handler = knocked_object_fly_ADDRESS;
	knocked_object_fly();
}

void knocked_object_fly(void)
{
	uint16_t child = 0;
	uint32_t r1 = 0, r7 = 0, ignored = 0, off_screen = 0;

	OBJECT(current_object)->speed_z += 0x6000u;
	OBJECT(current_object)->z += OBJECT(current_object)->speed_z;
	OBJECT(current_object)->x += OBJECT(current_object)->speed_x;
	OBJECT(current_object)->y += OBJECT(current_object)->speed_y;
	project_object(&ignored, &r1);
	off_screen = is_off_screen();
	if (off_screen == 0u)
	{
		animate(&r1, &r7, &off_screen);
		r1 = queue_sprite(r1);
		return;
	}
	child = OBJECT(current_object)->child;
	if ((int16_t)child >= 0)
		free_object_2(child);
	delete_current_object();
}

uint32_t spawn_chaser_back_zone(uint32_t zone_index, uint32_t zone)
{
	uint32_t free_count = 0, step_record = 0;

	chaser_zone_index = zone_index;
	allocate_object(&free_count, &zone);
	if (free_count != 0u)
	{
		OBJECT(zone)->kind = 9u;
		OBJECT(zone)->flags = 0x40000002u;
		step_record = object_steps_ADDRESS + chaser_zone_index * 3u * 2u;
		AS16(OBJECT(zone)->var_80) = mem_read16(step_record);
		OBJECT(zone)->handler = mem_read32(step_record + 2u);
		OBJECT(zone)->parent = current_object;
	}
	return zone;
}

void chaser_back_zone_check_parent(void)
{
	OBJECT(current_object)->kind = 0x14u;
	if (AS8(OBJECT(LINKED(current_object, OBJECT(current_object)->parent))->var_96) == 3u)
	{
		delete_current_object();
		return;
	}
	chaser_back_zone_update();
}

void chaser_back_zone_update(void)
{
	uint32_t direction = 0, object = 0;

	if (OBJECT(current_object)->chaser_back_zone.end_position <= scroll_position)
	{
		delete_current_object();
		return;
	}
	object = LINKED(current_object, OBJECT(current_object)->parent);
	OBJECT(current_object)->hitbox_size_x = OBJECT(object)->hitbox_size_x;
	OBJECT(current_object)->hitbox_size_y = OBJECT(object)->hitbox_size_y;
	OBJECT(current_object)->x = OBJECT(object)->x;
	OBJECT(current_object)->y = OBJECT(object)->y;
	OBJECT(current_object)->hitbox_offset_x = OBJECT(object)->hitbox_offset_x;
	OBJECT(current_object)->hitbox_offset_y = OBJECT(object)->hitbox_offset_y;
	OBJECT(current_object)->z = OBJECT(object)->z;
	OBJECT(current_object)->direction = OBJECT(object)->direction;
	OBJECT(current_object)->hitbox_height = OBJECT(object)->hitbox_height;
	direction = OBJECT(object)->direction;
	switch (direction >> 8)
	{
	case 0:
	case 4:
		OBJECT(current_object)->y_int += OBJECT(current_object)->hitbox_offset_y;
		OBJECT(current_object)->y_int += OBJECT(current_object)->hitbox_size_y;
		OBJECT(current_object)->hitbox_size_y = 0x200u;
		OBJECT(current_object)->hitbox_offset_y = 0;
		break;
	case 1:
		OBJECT(current_object)->x_int += OBJECT(current_object)->hitbox_offset_x;
		OBJECT(current_object)->hitbox_size_x = 0x200u;
		OBJECT(current_object)->x_int -= OBJECT(current_object)->hitbox_size_x;
		OBJECT(current_object)->hitbox_offset_x = 0;
		break;
	case 2:
		OBJECT(current_object)->y_int += OBJECT(current_object)->hitbox_offset_y;
		OBJECT(current_object)->hitbox_size_y = 0x200u;
		OBJECT(current_object)->y_int -= OBJECT(current_object)->hitbox_size_y;
		OBJECT(current_object)->hitbox_offset_y = 0;
		break;
	case 3:
		OBJECT(current_object)->x_int += OBJECT(current_object)->hitbox_offset_x;
		OBJECT(current_object)->x_int += OBJECT(current_object)->hitbox_size_x;
		OBJECT(current_object)->hitbox_size_x = 0x200u;
		OBJECT(current_object)->hitbox_offset_x = 0;
		break;
	default:
		unknown_code(0x82F30u + (direction >> 8) * 2u);
	}
	direction = add_to_zone_list(direction);
}

void spawn_roller_back_zone(uint32_t zone_index, uint32_t zone)
{
	uint32_t free_count = 0;

	roller_zone_index = zone_index;
	allocate_object(&free_count, &zone);
	if (free_count != 0u)
	{
		OBJECT(zone)->kind = 0x18u;
		OBJECT(zone)->flags = 0x40000002u;
		OBJECT(zone)->roller_back_zone.limit = object_steps[roller_zone_index * 3u];
		OBJECT(zone)->handler = roller_back_zone_update_ADDRESS;
		OBJECT(zone)->parent = object_cursor;
	}
}

void roller_back_zone_update(void)
{
	uint32_t direction = 0, object = 0;

	object = LINKED(current_object, OBJECT(current_object)->parent);
	OBJECT(current_object)->hitbox_size_x = OBJECT(object)->hitbox_size_x;
	OBJECT(current_object)->hitbox_size_y = OBJECT(object)->hitbox_size_y;
	OBJECT(current_object)->x = OBJECT(object)->x;
	OBJECT(current_object)->y = OBJECT(object)->y;
	OBJECT(current_object)->hitbox_offset_x = OBJECT(object)->hitbox_offset_x;
	OBJECT(current_object)->hitbox_offset_y = OBJECT(object)->hitbox_offset_y;
	OBJECT(current_object)->z = OBJECT(object)->z;
	OBJECT(current_object)->direction = OBJECT(object)->direction;
	OBJECT(current_object)->hitbox_height = OBJECT(object)->hitbox_height;
	direction = OBJECT(object)->direction;
	switch (direction >> 8)
	{
	case 0:
	case 4:
		if (OBJECT(current_object)->y_int <= OBJECT(current_object)->roller_back_zone.limit)
		{
			delete_current_object();
			return;
		}
		OBJECT(current_object)->y_int += OBJECT(current_object)->hitbox_size_y;
		OBJECT(current_object)->hitbox_size_y = 0x200u;
		OBJECT(current_object)->hitbox_size_x = 0x1A2u;
		break;
	case 1:
		if (OBJECT(current_object)->roller_back_zone.limit <= OBJECT(current_object)->x_int)
		{
			delete_current_object();
			return;
		}
		OBJECT(current_object)->x_int += OBJECT(current_object)->hitbox_offset_x;
		OBJECT(current_object)->hitbox_size_x = 0x200u;
		OBJECT(current_object)->x_int -= OBJECT(current_object)->hitbox_size_x;
		OBJECT(current_object)->y_int -= 0xB0u;
		OBJECT(current_object)->hitbox_size_y = 0x1A2u;
		OBJECT(current_object)->hitbox_offset_x = 0;
		break;
	case 2:
		if (OBJECT(current_object)->roller_back_zone.limit <= OBJECT(current_object)->y_int)
		{
			delete_current_object();
			return;
		}
		OBJECT(current_object)->y_int += OBJECT(current_object)->hitbox_offset_y;
		OBJECT(current_object)->hitbox_size_y = 0x200u;
		OBJECT(current_object)->y_int -= OBJECT(current_object)->hitbox_size_y;
		OBJECT(current_object)->x_int -= 0xB0u;
		OBJECT(current_object)->hitbox_size_x = 0x1A2u;
		OBJECT(current_object)->hitbox_offset_y = 0;
		break;
	case 3:
		if (OBJECT(current_object)->x_int <= OBJECT(current_object)->roller_back_zone.limit)
		{
			delete_current_object();
			return;
		}
		OBJECT(current_object)->x_int += OBJECT(current_object)->hitbox_size_x;
		OBJECT(current_object)->hitbox_size_x = 0x200u;
		OBJECT(current_object)->hitbox_size_y = 0x1A2u;
		break;
	default:
		unknown_code(0x8303Bu + (direction >> 8) * 3u);
	}
	direction = add_to_zone_list(direction);
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

void how_to_play_button_hand_start(void)
{
	OBJECT(current_object)->sprite_flags = 0xD000u;
	OBJECT(current_object)->sprite_offset_x = 0;
	OBJECT(current_object)->sprite_offset_y = 0;
	OBJECT(current_object)->flags = 0u;
	OBJECT(current_object)->handler = how_to_play_button_hand_update_ADDRESS;
	OBJECT(current_object)->part_list = how_to_play_button_hand_parts_ADDRESS;
	OBJECT(current_object)->animation = how_to_play_button_hand_animation_ADDRESS;
	OBJECT(current_object)->animation_frame = 0u;
	OBJECT(current_object)->how_to_play_button_hand.next_press = how_to_play_button_presses_ADDRESS;
	OBJECT(current_object)->screen_x = 0x42;
	OBJECT(current_object)->screen_y = 0xB5;
	OBJECT(current_object)->handler = how_to_play_button_hand_update_ADDRESS;
}

void how_to_play_button_hand_update(void)
{
	uint16_t frames = 0, kind = 0, screen_x = 0, screen_y = 0;
	uint32_t depth = 0, ignored = 0;

	if (stage != 7u)
	{
		delete_current_object();
		return;
	}
	if ((uint16_t)game_mode_timer == 0xC8u)
	{
		screen_x = 0xA0u;
		screen_y = 0x7Cu;
		kind = 0xCu;
		frames = 0x12Cu;
		spawn_message_box(screen_x, screen_y, kind, frames, 0u);
	}
	if ((uint16_t)game_mode_timer == mem_read16(OBJECT(current_object)->how_to_play_button_hand.next_press))
	{
		OBJECT(current_object)->animation_frame = (uint8_t)mem_read16(OBJECT(current_object)->how_to_play_button_hand.next_press + 2u);
		OBJECT(current_object)->how_to_play_button_hand.next_press += 4u;
		animate_part_list_step();
	}
	depth = 0xEEu;
	queue_sprite_at_depth(&ignored, &depth);
}

void how_to_play_trackball_hand_start(void)
{
	uint32_t free_count = 0, ignored = 0, object = 0;

	OBJECT(current_object)->sprite_flags = 0x1000u;
	OBJECT(current_object)->sprite_frame = 0x71C8u;
	OBJECT(current_object)->sprite_palette = 0x8530u;
	OBJECT(current_object)->sprite_offset_x = 0;
	OBJECT(current_object)->sprite_offset_y = 0;
	OBJECT(current_object)->flags = 0u;
	OBJECT(current_object)->handler = how_to_play_trackball_hand_update_ADDRESS;
	OBJECT(current_object)->how_to_play_trackball_hand.step = 0u;
	OBJECT(current_object)->screen_x = 0xFB;
	OBJECT(current_object)->screen_y = 0xB3;
	OBJECT(current_object)->projected_x = OBJECT(current_object)->screen_x;
	OBJECT(current_object)->handler = how_to_play_trackball_hand_update_ADDRESS;
	OBJECT(current_object)->animation = how_to_play_trackball_animation_ADDRESS;
	OBJECT(current_object)->animation_frame = 0u;
	OBJECT(current_object)->animation_speed = 4u;
	OBJECT(current_object)->animation_timer = 0;
	OBJECT(current_object)->child = 0u;
	allocate_object_2(&free_count, &ignored, &object);
	if (free_count != 0u)
	{
		OBJECT(object)->sprite_flags = 0x1000u;
		OBJECT(object)->sprite_frame = 0x71C0u;
		OBJECT(object)->sprite_palette = 0x8550u;
		OBJECT(object)->screen_x = OBJECT(current_object)->screen_x;
		OBJECT(object)->screen_x -= 0x38u;
		OBJECT(object)->screen_y = OBJECT(current_object)->screen_y;
		OBJECT(object)->screen_y -= 8u;
		OBJECT(current_object)->child = object;
	}
}

void how_to_play_trackball_hand_update(void)
{
	uint8_t last_step = 0;
	uint16_t child = 0, child_palette = 0, frames = 0, kind = 0, screen_x = 0, screen_y = 0;
	uint32_t box = 0, depth = 0, ignored = 0, next_step = 0, step_list = 0;
	uint8_t pause_timer = 0, animation_timer = 0;

	if (stage == 7u)
	{
		if (game_mode_timer >= 0x32)
		{
			switch (OBJECT(current_object)->how_to_play_trackball_hand.step)
			{
			case 0:
			case 3:
				OBJECT(current_object)->screen_x -= 0xAu;
				OBJECT(current_object)->screen_y += 0xAu;
				if ((int16_t)(OBJECT(current_object)->screen_x - OBJECT(current_object)->projected_x) <= -0x20)
				{
					OBJECT(current_object)->how_to_play_trackball_hand.step++;
					OBJECT(current_object)->how_to_play_trackball_hand.pause_timer = 8;
				}
				break;
			case 1:
				pause_timer = OBJECT(current_object)->how_to_play_trackball_hand.pause_timer;
				OBJECT(current_object)->how_to_play_trackball_hand.pause_timer = pause_timer - 1u;
				if ((int8_t)pause_timer <= 1)
					OBJECT(current_object)->how_to_play_trackball_hand.step++;
				break;
			case 2:
				OBJECT(current_object)->screen_x += 4u;
				OBJECT(current_object)->screen_y -= 4u;
				if (OBJECT(current_object)->projected_x <= OBJECT(current_object)->screen_x)
					OBJECT(current_object)->how_to_play_trackball_hand.step = 0u;
				break;
			default:
				unknown_code(0x83E6Du + OBJECT(current_object)->how_to_play_trackball_hand.step * 2u);
			}
		}
		if ((uint16_t)game_mode_timer == 0xAu)
			OBJECT(current_object)->sprite_palette = 0x8760u;
		else if ((uint16_t)game_mode_timer == 0xBEu)
			OBJECT(current_object)->sprite_palette = 0x8530u;
		if ((uint16_t)game_mode_timer == 0xAu)
		{
			screen_x = 0x40u;
			screen_y = 0x30u;
			kind = 0xDu;
			frames = 0x1E0u;
			box = spawn_message_box(screen_x, screen_y, kind, frames, 0u);
			screen_x = 0xA0u;
			screen_y = 0x7Cu;
			kind = 0xBu;
			frames = 0xB4u;
			spawn_message_box(screen_x, screen_y, kind, frames, box);
		}
		if (game_mode_timer >= 0x20)
		{
			scroll_mode = 0u;
			AS8(scroll_speed) = 0x10u;
		}
		depth = 0xEEu;
		queue_sprite_at_depth(&ignored, &depth);
		current_object = LINKED(current_object, OBJECT(current_object)->child);
		if ((uint16_t)current_object == 0u)
			return;
		if (game_mode_timer >= 0x32)
		{
			animation_timer = OBJECT(object_cursor)->animation_timer;
			OBJECT(object_cursor)->animation_timer = animation_timer - 1u;
			if ((int8_t)animation_timer <= 1)
			{
				OBJECT(object_cursor)->animation_timer = OBJECT(object_cursor)->animation_speed;
				step_list = OBJECT(object_cursor)->animation;
				last_step = (uint8_t)mem_read16(step_list);
				next_step = OBJECT(object_cursor)->animation_frame;
				next_step = (uint8_t)(next_step + 1u);
				step_list += 2u;
				if ((int8_t)last_step < (int8_t)next_step)
					next_step = 0u;
				OBJECT(object_cursor)->animation_frame = next_step;
				OBJECT(current_object)->sprite_frame = mem_read16(step_list + next_step * 2u);
			}
		}
		child_palette = 0x8750u;
		if (OBJECT(object_cursor)->sprite_palette != 0x8760u)
			child_palette = 0x8550u;
		OBJECT(current_object)->sprite_palette = child_palette;
		depth = 0xEDu;
		queue_sprite_at_depth(&ignored, &depth);
		return;
	}
	if (OBJECT(current_object)->child != 0u)
	{
		child = OBJECT(current_object)->child;
		free_object_2(child);
	}
	delete_current_object();
}

void how_to_play_panel_start(void)
{
	OBJECT(current_object)->sprite_flags = 0x100Au;
	OBJECT(current_object)->sprite_frame = 0x71F0u;
	OBJECT(current_object)->sprite_palette = 0x8780u;
	OBJECT(current_object)->screen_x = 0;
	OBJECT(current_object)->screen_y = 0x88;
	OBJECT(current_object)->handler = how_to_play_panel_show_ADDRESS;
	how_to_play_panel_show();
}

void how_to_play_panel_show(void)
{
	uint32_t depth = 0, ignored = 0;

	if (stage == 7u)
	{
		depth = 0xECu;
		queue_sprite_at_depth(&ignored, &depth);
		return;
	}
	delete_current_object();
}

void opening_ground_start(void)
{
	OBJECT(current_object)->sprite_flags = 0x1000u;
	OBJECT(current_object)->sprite_palette = 0x9120u;
	OBJECT(current_object)->flags = 0u;
	OBJECT(current_object)->speed_y = 0;
	OBJECT(current_object)->handler = opening_ground_run_ADDRESS;
	OBJECT(current_object)->x = 0xA00000;
	OBJECT(current_object)->y = 0xB80000;
	OBJECT(current_object)->animation_frame = 0u;
	OBJECT(current_object)->animation = opening_ground_animation_ADDRESS;
	OBJECT(current_object)->animation_timer = 0;
	OBJECT(current_object)->animation_speed = 1u;
}

void opening_ground_run(void)
{
	uint16_t shake_dy = 0;
	uint32_t r7 = 0, r8 = 0, depth = 0, frame = 0, ignored = 0;

	if ((int8_t)scene_step >= 2)
	{
		delete_current_object();
		return;
	}
	OBJECT(current_object)->y += OBJECT(current_object)->speed_y;
	OBJECT(current_object)->screen_x = OBJECT(current_object)->x_int;
	OBJECT(current_object)->screen_y = OBJECT(current_object)->y_int;
	shake_dy = AT8(stage_06_shake, 1);
	if (stage_06_shake_sign < 0)
		shake_dy = (uint16_t)(0u - shake_dy);
	OBJECT(current_object)->screen_y += shake_dy;
	animate(&frame, &r7, &r8);
	depth = 0x20u;
	queue_sprite_at_depth(&ignored, &depth);
}

void opening_machine_start(void)
{
	OBJECT(current_object)->opening_machine.scene_step_seen = 0xFFu;
	OBJECT(current_object)->handler = opening_machine_update_ADDRESS;
	opening_machine_update();
}

void opening_machine_update(void)
{
	uint16_t shake_dy = 0;
	uint32_t depth = 0, free_count = 0, ignored = 0, object = 0, scale = 0;

	switch (scene_step)
	{
	case 0:
	case 1:
	case 4:
		if (OBJECT(current_object)->opening_machine.scene_step_seen != scene_step)
		{
			OBJECT(current_object)->opening_machine.scene_step_seen = scene_step;
			OBJECT(current_object)->sprite_flags = 0xC000u;
			OBJECT(current_object)->opening_machine.zoom = 0xC00000;
			OBJECT(current_object)->sprite_scale = OBJECT(current_object)->opening_machine.zoom_int;
			OBJECT(current_object)->opening_machine.zoom_speed = 0x8000;
			OBJECT(current_object)->flags = 0u;
			OBJECT(current_object)->opening_machine.phase = 0;
			OBJECT(current_object)->speed_y = -0xC0000;
			OBJECT(current_object)->x = 0xA00000;
			OBJECT(current_object)->y = 0xF80000;
			OBJECT(current_object)->animation_frame = 0u;
			OBJECT(current_object)->animation = opening_machine_leap_animation_ADDRESS;
			OBJECT(current_object)->animation_timer = 0;
			OBJECT(current_object)->animation_speed = 2u;
			opening_machine_object = current_object;
		}
		OBJECT(current_object)->opening_machine.draw_depth = OBJECT(current_object)->screen_y;
		switch ((uint8_t)OBJECT(current_object)->opening_machine.phase)
		{
		case 0:
			opening_machine_leap_in();
			break;
		case 1:
			opening_machine_approach();
			break;
		case 2:
			opening_machine_lunge();
			break;
		case 3:
			opening_machine_show_mouth();
			break;
		case 4:
			opening_machine_engulf();
			break;
		case 5:
			opening_machine_hold();
			break;
		case 6:
			opening_machine_leap_in();
			break;
		default:
			unknown_code(0x84157u + (uint8_t)OBJECT(current_object)->opening_machine.phase * 3u);
		}
		OBJECT(current_object)->screen_x = OBJECT(current_object)->x_int;
		OBJECT(current_object)->screen_y = OBJECT(current_object)->y_int;
		shake_dy = AT8(stage_06_shake, 1);
		if (stage_06_shake_sign < 0)
			shake_dy = (uint16_t)(0u - shake_dy);
		OBJECT(current_object)->screen_y += shake_dy;
		if (OBJECT(current_object)->opening_machine.phase < 3)
			animate_part_list();
		depth = OBJECT(current_object)->opening_machine.draw_depth;
		queue_sprite_at_depth(&ignored, &depth);
		return;
	case 2:
	case 3:
		if (OBJECT(current_object)->opening_machine.scene_step_seen != scene_step)
		{
			OBJECT(current_object)->opening_machine.scene_step_seen = scene_step;
			if (OBJECT(current_object)->opening_machine.scene_step_seen == 2u)
			{
				OBJECT(current_object)->sprite_flags = 0xC000u;
				OBJECT(current_object)->opening_machine.zoom = 0;
				OBJECT(current_object)->flags = 0u;
				OBJECT(current_object)->y = -0x600000;
				OBJECT(current_object)->animation_frame = 0u;
				OBJECT(current_object)->animation = opening_machine_fly_away_animation_ADDRESS;
				OBJECT(current_object)->animation_timer = 0;
				OBJECT(current_object)->animation_speed = 2u;
				OBJECT(current_object)->child = 0xFFFFu;
				allocate_object_2(&free_count, &ignored, &object);
				if (free_count != 0u)
				{
					OBJECT(object)->sprite_flags = 0u;
					OBJECT(object)->sprite_frame = 0x7078u;
					OBJECT(object)->sprite_palette = 0x87A0u;
					OBJECT(object)->sprite_width = 0;
					OBJECT(object)->sprite_height = 0x18;
					OBJECT(current_object)->child = object;
				}
			}
		}
		OBJECT(current_object)->opening_machine.zoom += 2u;
		OBJECT(current_object)->sprite_scale = divide_s32(0x4000u, OBJECT(current_object)->opening_machine.zoom + 0x10u);
		OBJECT(current_object)->y += 0x10000u;
		scale = OBJECT(current_object)->sprite_scale;
		OBJECT(current_object)->screen_y = ((int32_t)(OBJECT(current_object)->y_int * scale) >> 10) + 0xB8u;
		animate_part_list();
		queue_sprite(scale);
		draw_opening_machine_shadow();
		return;
	default:
		unknown_code(0x84089u + scene_step * 3u);
	}
}

void opening_machine_leap_in(void)
{
	uint8_t sound_id = 0;
	uint32_t write_index = 0, write_index_address = 0;

	if (OBJECT(current_object)->speed_y_int < 0)
		OBJECT(current_object)->opening_machine.draw_depth = 0x10u;
	if (scene_timer >= 0x3C)
	{
		OBJECT(current_object)->speed_y += 0x6000u;
		OBJECT(current_object)->y += OBJECT(current_object)->speed_y;
		if (OBJECT(current_object)->speed_y_int >= 0 && OBJECT(current_object)->y_int >= 0x63)
		{
			OBJECT(current_object)->y = 0x630000;
			stage_06_shake = 0x800u;
			stage_06_shake_sign = 0;
			OBJECT(current_object)->animation_frame = 0u;
			OBJECT(current_object)->animation = opening_machine_landed_animation_ADDRESS;
			OBJECT(current_object)->animation_timer = 0;
			OBJECT(current_object)->animation_speed = 2u;
			sound_id = 0xAu;
			play_sound(sound_id, &write_index, &write_index_address);
			OBJECT(current_object)->opening_machine.phase++;
		}
	}
}

void opening_machine_approach(void)
{
	uint16_t shake_before = 0;

	if (stage_06_shake != 0u)
	{
		shake_before = stage_06_shake;
		stage_06_shake = shake_before - 0x80u;
		if ((int16_t)shake_before < 0x80)
			stage_06_shake = 0u;
		stage_06_shake_sign = ~stage_06_shake_sign;
	}
	OBJECT(current_object)->opening_machine.zoom += OBJECT(current_object)->opening_machine.zoom_speed;
	OBJECT(current_object)->sprite_scale = OBJECT(current_object)->opening_machine.zoom_int;
	if (scene_timer > 0xC0)
		OBJECT(current_object)->opening_machine.phase++;
}

void opening_machine_lunge(void)
{
	OBJECT(current_object)->opening_machine.zoom_speed += 0x1000u;
	OBJECT(current_object)->opening_machine.zoom += OBJECT(current_object)->opening_machine.zoom_speed;
	OBJECT(current_object)->sprite_scale = OBJECT(current_object)->opening_machine.zoom_int;
	if ((int16_t)OBJECT(current_object)->sprite_scale >= 0x1B3)
		OBJECT(current_object)->y -= 0xE000u;
	if (OBJECT(current_object)->opening_machine.zoom_int >= 0x32B)
		OBJECT(current_object)->opening_machine.phase++;
}

void opening_machine_show_mouth(void)
{
	OBJECT(current_object)->opening_machine.phase++;
	OBJECT(current_object)->sprite_flags = 0u;
	OBJECT(current_object)->sprite_frame = 0x7198u;
	OBJECT(current_object)->sprite_palette = 0x8510u;
	OBJECT(current_object)->y += (OBJECT(current_object)->sprite_scale * 0x1Eu) << 8;
	opening_machine_engulf();
}

void opening_machine_engulf(void)
{
	OBJECT(current_object)->opening_machine.zoom_speed += 0x10000u;
	OBJECT(current_object)->opening_machine.zoom += OBJECT(current_object)->opening_machine.zoom_speed;
	OBJECT(current_object)->sprite_scale = OBJECT(current_object)->opening_machine.zoom_int;
	if (OBJECT(current_object)->opening_machine.zoom_int < 0x5B0)
	{
		opening_machine_hold();
		return;
	}
	OBJECT(current_object)->opening_machine.phase++;
	OBJECT(current_object)->sprite_palette = 0x87A0u;
	opening_machine_hold();
}

void opening_machine_hold(void)
{
}

void draw_opening_machine_shadow(void)
{
	uint16_t scaled_dy = 0;
	uint32_t depth = 0, ignored = 0, sprite_scale = 0;

	current_object = LINKED(current_object, OBJECT(current_object)->child);
	sprite_scale = OBJECT(object_cursor)->sprite_scale;
	OBJECT(current_object)->screen_x = ((int32_t)(OBJECT(current_object)->sprite_width * sprite_scale) >> 8) + OBJECT(object_cursor)->screen_x;
	scaled_dy = (uint16_t)((int32_t)(OBJECT(current_object)->sprite_height * sprite_scale) >> 8);
	OBJECT(current_object)->screen_y = OBJECT(object_cursor)->screen_y;
	OBJECT(current_object)->screen_y += scaled_dy;
	OBJECT(current_object)->sprite_scale = OBJECT(object_cursor)->sprite_scale;
	depth = (uint16_t)(OBJECT(object_cursor)->screen_y - 0x10u);
	queue_sprite_at_depth(&ignored, &depth);
}

void draw_opening_machine_shadow_spare_return(void)
{
}

void player_pacer_start(void)
{
	OBJECT(current_object)->speed_x_fraction = pacer_end_positions[OBJECT(current_object)->sprite_frame];
	OBJECT(current_object)->handler = player_pacer_update_ADDRESS;
	player_pacer_update();
}

void player_pacer_update(void)
{
	uint32_t axis_field = 0, decreasing = 0, leader_object = 0, middle_player = 0, other_object = 0, player = 0, player_1 = 0, player_2 = 0, player_3 = 0, player_4 = 0, players_left = 0, playing_count = 0, status_cursor = 0;
	uint8_t status = 0;
	uint32_t swap = 0, swap_2 = 0, swap_3 = 0, swap_4 = 0;

	if (OBJECT(current_object)->speed_x_fraction <= scroll_position)
	{
		player_1_speed_scale = 0u;
		player_2_speed_scale = 0u;
		player_3_speed_scale = 0u;
		delete_current_object();
		return;
	}
	player_1_speed_scale = 0u;
	player_2_speed_scale = 0u;
	player_3_speed_scale = 0u;
	status_cursor = player_status_ADDRESS;
	players_left = 3u;
	playing_count = 0u;
	do
	{
		status = mem_read8(status_cursor);
		status_cursor++;
		if (status == 0xFFu)
			playing_count = (uint8_t)(playing_count + 1u);
		players_left--;
	} while (players_left != 0u);
	switch (playing_count)
	{
	case 0:
	case 1:
		return;
	case 2:
	case 4:
		player = PLAYER_BLOCK(1);
		player_2 = PLAYER_BLOCK(2);
		player_3 = PLAYER_BLOCK(3);
		if (player_status[0] == 0xFFu)
		{
			if (player_2_status != 0xFFu)
				player_2 = player_3;
		}
		else
		{
			player = player_2;
			player_2 = player_3;
		}
		get_pacing_axis(player, &axis_field, &decreasing);
		if ((int16_t)mem_read16(LINKED(current_object, PLAYER(player)->object) + axis_field * 2u) < (int16_t)mem_read16(LINKED(current_object, PLAYER(player_2)->object) + axis_field * 2u))
		{
			swap = player;
			player = player_2;
			player_2 = swap;
		}
		if ((uint16_t)decreasing != 0u)
		{
			mem_write16(player_2 + 0xC4u, 0xDu);
			return;
		}
		PLAYER(player)->speed_scale = 0xDu;
		return;
	case 3:
		player_1 = PLAYER_BLOCK(1);
		get_pacing_axis(player_1, &axis_field, &decreasing);
		player = PLAYER_BLOCK(1);
		middle_player = PLAYER_BLOCK(2);
		player_4 = PLAYER_BLOCK(3);
		leader_object = LINKED(current_object, player_1_object);
		other_object = LINKED(current_object, player_2_object);
		if ((int16_t)mem_read16(leader_object + axis_field * 2u) < (int16_t)mem_read16(other_object + axis_field * 2u))
		{
			swap_2 = player;
			player = middle_player;
			middle_player = swap_2;
			leader_object = LINKED(leader_object, PLAYER(player)->object);
		}
		other_object = LINKED(other_object, PLAYER(player_4)->object);
		if ((int16_t)mem_read16(leader_object + axis_field * 2u) < (int16_t)mem_read16(other_object + axis_field * 2u))
		{
			swap_3 = player;
			player = player_4;
			player_4 = swap_3;
		}
		if ((int16_t)mem_read16(((leader_object & ~0xFFFFu) | mem_read16(middle_player + 0x14u)) + axis_field * 2u) < (int16_t)mem_read16(((other_object & ~0xFFFFu) | mem_read16(player_4 + 0x14u)) + axis_field * 2u))
		{
			swap_4 = middle_player;
			middle_player = player_4;
			player_4 = swap_4;
		}
		if ((uint16_t)decreasing != 0u)
		{
			mem_write16(player_4 + 0xC4u, 0xDu);
			mem_write16(middle_player + 0xC4u, 0xEu);
			return;
		}
		PLAYER(player)->speed_scale = 0xDu;
		mem_write16(middle_player + 0xC4u, 0xEu);
		return;
	default:
		unknown_code(0x84428u + playing_count * 2u);
	}
}

void get_pacing_axis(uint32_t player, uint32_t *p_axis_field, uint32_t *p_decreasing)
{
	uint16_t decreasing = 0;
	uint32_t axis_field = 0;

	switch (mem_read8(floor_blocks + 2u + OBJECT(LINKED(current_object, PLAYER(player)->object))->floor_block + 0x15u))
	{
	case 0:
	case 4:
		axis_field = 0x29u;
		decreasing = 0xFFFFu;
		*p_axis_field = axis_field;
		*p_decreasing = (*p_decreasing & 0xFFFF0000u) | decreasing;
		return;
	case 1:
		axis_field = 0x27u;
		decreasing = 0u;
		*p_axis_field = axis_field;
		*p_decreasing = (*p_decreasing & 0xFFFF0000u) | decreasing;
		return;
	case 2:
		axis_field = 0x29u;
		decreasing = 0u;
		*p_axis_field = axis_field;
		*p_decreasing = (*p_decreasing & 0xFFFF0000u) | decreasing;
		return;
	case 3:
		axis_field = 0x27u;
		decreasing = 0xFFFFu;
		*p_axis_field = axis_field;
		*p_decreasing = (*p_decreasing & 0xFFFF0000u) | decreasing;
		return;
	default:
		unknown_code(0x8451Du + mem_read8(floor_blocks + 2u + OBJECT(LINKED(current_object, PLAYER(player)->object))->floor_block + 0x15u) * 2u);
	}
}

void exit_door_start(void)
{
	uint32_t ignored = 0;

	OBJECT(current_object)->sprite_frame = 0x7348u;
	OBJECT(current_object)->sprite_palette = 0x91A0u;
	OBJECT(current_object)->sprite_flags = 0x1004u;
	OBJECT(current_object)->sprite_offset_x = 0;
	OBJECT(current_object)->sprite_offset_y = 0;
	OBJECT(current_object)->flags = 0u;
	OBJECT(current_object)->handler = exit_door_wait_for_switch_ADDRESS;
	OBJECT(current_object)->z = 0;
	project_object(&ignored, &ignored);
}

void exit_door_wait_for_switch(void)
{
	uint32_t ignored = 0;

	if ((uint8_t)stage_05_switch_step != 2u)
	{
		exit_door_show();
		return;
	}
	OBJECT(current_object)->sprite_frame = 0x71A0u;
	OBJECT(current_object)->sprite_palette = 0x9140u;
	OBJECT(current_object)->x = 0x1E20000;
	OBJECT(current_object)->y = 0x154C0000;
	OBJECT(current_object)->handler = exit_door_show_ADDRESS;
	project_object(&ignored, &ignored);
}

void exit_door_show(void)
{
	uint32_t r1 = 0, ignored = 0;

	update_screen_position(&ignored, &r1);
	if (OBJECT(current_object)->screen_y < 0x130)
	{
		r1 = queue_sprite(r1);
		return;
	}
	delete_current_object();
}

void stage_05_floor_patch_start(void)
{
	uint32_t ignored = 0;

	OBJECT(current_object)->sprite_frame = 0x7050u;
	OBJECT(current_object)->sprite_palette = 0xB0E0u;
	OBJECT(current_object)->sprite_flags = 0x1008u;
	OBJECT(current_object)->sprite_offset_x = 0;
	OBJECT(current_object)->sprite_offset_y = 0;
	OBJECT(current_object)->flags = 0u;
	OBJECT(current_object)->handler = stage_05_floor_patch_show_ADDRESS;
	OBJECT(current_object)->z = 0;
	project_object(&ignored, &ignored);
}

void stage_05_floor_patch_show(void)
{
	uint32_t r1 = 0, ignored = 0;

	update_screen_position(&ignored, &r1);
	if (stage == 5u)
	{
		r1 = queue_sprite(r1);
		return;
	}
	delete_current_object();
}

void bubble_start(void)
{
	uint32_t ignored = 0;

	OBJECT(current_object)->z_int = bubble_heights[OBJECT(current_object)->sprite_frame];
	OBJECT(current_object)->z_fraction = 0u;
	OBJECT(current_object)->bubble_start.variant = OBJECT(current_object)->sprite_frame;
	OBJECT(current_object)->bubble_start.base_x = OBJECT(current_object)->x_int;
	OBJECT(current_object)->bubble_start.base_y = OBJECT(current_object)->y_int;
	OBJECT(current_object)->sprite_flags = 0x1004u;
	OBJECT(current_object)->sprite_palette = 0x9000u;
	if (stage != 5u && stage != 8u)
		OBJECT(current_object)->sprite_palette = 0x10B0u;
	if (stage == 8u)
		OBJECT(current_object)->sprite_palette |= 0x4000u;
	OBJECT(current_object)->sprite_offset_x = 0;
	OBJECT(current_object)->sprite_offset_y = 0;
	OBJECT(current_object)->flags = 0u;
	OBJECT(current_object)->handler = bubble_update_ADDRESS;
	OBJECT(current_object)->animation_frame = 0u;
	OBJECT(current_object)->animation = bubble_animation_ADDRESS;
	OBJECT(current_object)->animation_timer = 0;
	OBJECT(current_object)->animation_speed = 4u;
	project_object(&ignored, &ignored);
}

void bubble_update(void)
{
	if ((stage == 0xCu || stage == 8u || stage == 0xBu || stage == 5u) && current_game_mode != 0x20u)
	{
		OBJECT(current_object)->bubble_update.draw_depth = OBJECT(current_object)->screen_y;
		bubble_pop();
		return;
	}
	delete_current_object();
}

void bubble_near_parent_update(void)
{
	if ((int16_t)(OBJECT(LINKED(current_object, OBJECT(current_object)->parent))->y_int + 0xB0u) < OBJECT(current_object)->y_int)
	{
		OBJECT(current_object)->bubble_near_parent_update.draw_depth = 0xFFF0u;
		bubble_pop();
		return;
	}
	delete_current_object();
}

void bubble_pop(void)
{
	uint16_t jitter_y = 0;
	uint32_t r1 = 0, depth = 0, ignored = 0, jitter = 0, off_screen = 0, wrapped = 0;

	if (map_screen_active != 0u)
	{
		delete_current_object();
		return;
	}
	off_screen = check_left_screen();
	if (off_screen != 0u)
	{
		delete_current_object();
		return;
	}
	update_screen_position(&ignored, &r1);
	animate(&r1, &wrapped, &off_screen);
	if ((uint8_t)wrapped == 0u)
	{
		depth = OBJECT(current_object)->bubble_pop.draw_depth;
		queue_sprite_at_depth(&ignored, &depth);
		return;
	}
	next_random(&jitter, &ignored, &ignored);
	if (OBJECT(current_object)->bubble_pop.variant != 6u)
	{
		jitter = (jitter & ~0xFFFFu) | (jitter & 0xFu);
		if ((int32_t)jitter < 0)
			jitter = (uint16_t)(0u - jitter);
		jitter_y = (uint16_t)jitter;
		jitter = (uint16_t)(jitter + OBJECT(current_object)->bubble_pop.base_x);
		OBJECT(current_object)->x_int = jitter;
		OBJECT(current_object)->y_int = jitter_y + OBJECT(current_object)->bubble_pop.base_y;
		project_object(&ignored, &ignored);
	}
	OBJECT(current_object)->animation_speed = (jitter & 7u) + 1u;
	depth = OBJECT(current_object)->bubble_pop.draw_depth;
	queue_sprite_at_depth(&ignored, &depth);
}

void spiked_press_start(void)
{
	uint32_t r0 = 0, r1 = 0, r2 = 0, r3 = 0, r11 = 0;

	OBJECT(current_object)->sprite_flags = 0x1104u;
	OBJECT(current_object)->sprite_frame = 0x7070u;
	OBJECT(current_object)->sprite_palette = 0x9100u;
	OBJECT(current_object)->sprite_offset_x = 0x38;
	OBJECT(current_object)->sprite_offset_y = 0;
	OBJECT(current_object)->kind = 9u;
	OBJECT(current_object)->flags = 2u;
	OBJECT(current_object)->hitbox_size_x = 0xE0u;
	OBJECT(current_object)->hitbox_size_y = 0x20u;
	OBJECT(current_object)->hitbox_offset_x = 0;
	OBJECT(current_object)->hitbox_offset_y = -0x10;
	OBJECT(current_object)->hitbox_height = 0xE0u;
	OBJECT(current_object)->spiked_press.sound_timer = 0;
	OBJECT(current_object)->spiked_press.frame_count = 0;
	OBJECT(current_object)->spiked_press.stopped_frames = 0;
	OBJECT(current_object)->spiked_press.holding_back = 0u;
	OBJECT(current_object)->handler = spiked_press_update_ADDRESS;
	OBJECT(current_object)->speed_x = 0;
	OBJECT(current_object)->speed_y = 0x2000;
	OBJECT(current_object)->direction = 0x200u;
	find_floor(&r0, &r1, &r2, &r3, &r11);
	OBJECT(current_object)->z = OBJECT(current_object)->floor_z;
	spiked_press_object = current_object;
	scroll_mode = 1u;
	players_frozen = 0xFFu;
	spawn_eggman_booth(r0, r1, r3, &r2, &r11);
}

void spiked_press_update(void)
{
	uint8_t sound = 0;
	uint16_t frames = 0, kind = 0, screen_x = 0, screen_y = 0, x = 0, y = 0, z = 0;
	uint32_t r11 = 0, depth = 0, ignored = 0, player = 0, player_index = 0, players_left = 0, status_cursor = 0, write_index = 0, write_index_address = 0;
	uint8_t hold_timer = 0, status = 0;
	uint16_t sound_timer = 0;

	if (current_game_mode == 3u || current_game_mode == 0xAu)
		OBJECT(current_object)->spiked_press.frame_count++;
	if (OBJECT(current_object)->spiked_press.frame_count >= 0xC8 && OBJECT(current_object)->spiked_press.holding_back == 0u)
	{
		if (no_player_in_play != 0u || player_dropping_in != 0u)
		{
			OBJECT(current_object)->spiked_press.holding_back = 0xFFu;
			OBJECT(current_object)->spiked_press.hold_timer = 0xFFu;
		}
		else
			OBJECT(current_object)->y += OBJECT(current_object)->speed_y;
	}
	if (OBJECT(current_object)->spiked_press.holding_back != 0u)
	{
		if (current_game_mode != 3u && current_game_mode != 0xAu)
			OBJECT(current_object)->spiked_press.hold_timer = 0xFFu;
		hold_timer = OBJECT(current_object)->spiked_press.hold_timer;
		OBJECT(current_object)->spiked_press.hold_timer = hold_timer - 1u;
		if (hold_timer == 0u)
			OBJECT(current_object)->spiked_press.holding_back = 0u;
	}
	if ((uint16_t)OBJECT(current_object)->spiked_press.frame_count == 0xC8u)
	{
		x = 0xFE0Du;
		y = 0x3E0u;
		kind = 1u;
		z = 0xFFB0u;
		r11 = spawn_sign_2(x, y, kind, z, 0u);
		x = 0xFE0Du;
		y = 0x3E0u;
		kind = 2u;
		z = 0xFFC2u;
		r11 = spawn_sign_2(x, y, kind, z, r11);
		players_frozen = 0u;
	}
	if ((uint16_t)OBJECT(current_object)->spiked_press.frame_count == 0x168u)
	{
		x = 0xFE70u;
		y = 0x480u;
		kind = 3u;
		z = 0xFFB0u;
		r11 = spawn_sign_2(x, y, kind, z, r11);
	}
	if ((uint16_t)OBJECT(current_object)->spiked_press.frame_count == 0x14u)
	{
		players_left = 3u;
		player_index = 0u;
		status_cursor = player_status_ADDRESS;
		do
		{
			status = mem_read8(status_cursor);
			status_cursor++;
			if (status == 0xFFu)
				break;
			player_index = (uint16_t)(player_index + 1u);
			players_left--;
		} while (players_left != 0u);
		player = PLAYER_BLOCK(1) + (player_index << 9);
		caged_character = OBJECT(LINKED(player, PLAYER(player)->object))->kind;
		kind = 0xFu;
		if (caged_character != 0u)
		{
			kind = 0x1Au;
			if (caged_character != 1u)
				kind = 0x1Bu;
		}
		frames = 0x78u;
		screen_x = 0xA0u;
		screen_y = 0x40u;
		spawn_message_box(screen_x, screen_y, kind, frames, r11);
	}
	if ((uint16_t)OBJECT(current_object)->spiked_press.frame_count == 0x14u)
	{
		sound = 0x34u;
		play_sound(sound, &write_index, &write_index_address);
	}
	if ((uint16_t)OBJECT(current_object)->spiked_press.frame_count == 0x50u)
	{
		sound = 0x3Fu;
		if (caged_character != 0u)
		{
			sound = 0x5Cu;
			if (caged_character != 1u)
				sound = 0x40u;
		}
		play_sound(sound, &write_index, &write_index_address);
	}
	if (OBJECT(current_object)->y_int >= 0x3D0)
	{
		OBJECT(current_object)->speed_y = 0;
		OBJECT(current_object)->spiked_press.stopped_frames++;
		scroll_mode = 0u;
		scroll_speed = 5u;
	}
	if (stage != 5u)
	{
		delete_current_object();
		return;
	}
	if (OBJECT(current_object)->speed_y_fraction != 0u && OBJECT(current_object)->spiked_press.frame_count >= 0xC8 && OBJECT(current_object)->spiked_press.holding_back == 0u)
	{
		sound_timer = OBJECT(current_object)->spiked_press.sound_timer;
		OBJECT(current_object)->spiked_press.sound_timer = sound_timer - 1u;
		if ((int16_t)sound_timer <= 1)
		{
			OBJECT(current_object)->spiked_press.sound_timer = 0x3C;
			sound = 0xDu;
			play_sound(sound, &write_index, &write_index_address);
		}
	}
	OBJECT(current_object)->health = 0x7F;
	project_object(&ignored, &ignored);
	add_to_touchable_list(ignored);
	depth = (uint16_t)(OBJECT(current_object)->screen_y - 0x50u);
	queue_sprite_at_depth(&ignored, &depth);
}

void cage_start(void)
{
	uint32_t ignored = 0, object = 0;
	uint16_t bars_left_before = 0;

	cage_bars_left = 0xA;
	if (free_object_stack < cage_bars_left)
		return;
	cage_bar_x = OBJECT(current_object)->x_int;
	cage_bar_y = OBJECT(current_object)->y_int;
	cage_bar_hit = 0u;
	do
	{
		allocate_object(&ignored, &object);
		OBJECT(object)->sprite_flags = 0x1006u;
		OBJECT(object)->sprite_frame = 0x7058u;
		OBJECT(object)->sprite_palette = 0x90F0u;
		OBJECT(object)->sprite_offset_x = -8;
		OBJECT(object)->sprite_offset_y = 8;
		OBJECT(object)->kind = 0xCu;
		OBJECT(object)->flags = 2u;
		OBJECT(object)->hitbox_size_x = 0x20u;
		OBJECT(object)->hitbox_size_y = 0x10u;
		OBJECT(object)->hitbox_offset_x = -0x10;
		OBJECT(object)->hitbox_offset_y = -0xC;
		OBJECT(object)->hitbox_height = 0xE0u;
		OBJECT(object)->animation = cage_bar_animation_ADDRESS;
		OBJECT(object)->animation_frame = 0u;
		OBJECT(object)->animation_timer = -1;
		OBJECT(object)->health = 5;
		OBJECT(object)->handler = cage_bar_start_ADDRESS;
		OBJECT(object)->x_int = cage_bar_x;
		OBJECT(object)->x_fraction = 0u;
		OBJECT(object)->y_int = cage_bar_y;
		OBJECT(object)->y_fraction = 0u;
		OBJECT(object)->z = 0;
		cage_bar_x += 0x18u;
		bars_left_before = cage_bars_left;
		cage_bars_left = bars_left_before - 1u;
	} while ((int16_t)bars_left_before > 1);
	delete_current_object();
}

void cage_bar_start(void)
{
	uint32_t ignored = 0;

	project_object(&ignored, &ignored);
	OBJECT(current_object)->handler = cage_bar_update_ADDRESS;
	cage_bar_update();
}

void cage_bar_update(void)
{
	uint8_t sound_id = 0;
	uint32_t r1 = 0, animation_frame = 0, depth = 0, ignored = 0, off_screen = 0, step_list = 0, write_index_address = 0;
	uint8_t animation_timer = 0;
	uint16_t last_step = 0;

	OBJECT(current_object)->health = 5;
	if ((int16_t)OBJECT((current_object & ~0xFFFFu) | spiked_press_object)->var_82 >= 0xC8 && (int8_t)OBJECT(current_object)->animation_frame >= 0 && (OBJECT(current_object)->flags & 4u) != 0u && (OBJECT(current_object)->flags & 0x40u) != 0u)
	{
		OBJECT(current_object)->animation_frame++;
		animation_frame = OBJECT(current_object)->animation_frame;
		step_list = OBJECT(current_object)->animation;
		last_step = mem_read16(step_list);
		step_list += 2u;
		if ((int16_t)animation_frame < (int16_t)last_step)
		{
			OBJECT(current_object)->sprite_frame = mem_read16(step_list + animation_frame * 2u);
			OBJECT(current_object)->health = 1;
			cage_bar_hit = 0xFFu;
		}
		else
		{
			OBJECT(current_object)->animation_timer = 8;
			OBJECT(current_object)->animation_frame = 0xFFu;
		}
		sound_id = 0x21u;
		play_sound(sound_id, &r1, &write_index_address);
	}
	if ((int8_t)OBJECT(current_object)->animation_frame < 0 && OBJECT(current_object)->animation_timer >= 0)
	{
		animation_timer = OBJECT(current_object)->animation_timer;
		OBJECT(current_object)->animation_timer = animation_timer - 1u;
		if ((int8_t)animation_timer < 1)
		{
			OBJECT(current_object)->sprite_frame = mem_read16(OBJECT(current_object)->animation + 6u);
			OBJECT(current_object)->projected_x++;
		}
	}
	update_screen_position(&ignored, &r1);
	if (stage != 5u)
	{
		delete_current_object();
		return;
	}
	off_screen = check_left_screen();
	if (off_screen != 0u)
	{
		delete_current_object();
		return;
	}
	if ((int8_t)OBJECT(current_object)->animation_frame >= 0)
		add_to_touchable_list(ignored);
	depth = (uint16_t)(OBJECT(current_object)->screen_y - 8u);
	queue_sprite_at_depth(&ignored, &depth);
}

void sign_start(void)
{
	uint32_t ignored = 0, kind_record = 0, sign = 0;

	kind_record = OBJECT(current_object)->sprite_frame * 0xCu + object_kinds_ADDRESS;
	OBJECT(current_object)->z_int = mem_read16(kind_record);
	OBJECT(current_object)->z_fraction = 0u;
	sign = current_object;
	kind_record += 2u;
	init_sign_from_kind(kind_record, &ignored, &sign);
}

void spawn_sign(uint16_t x, uint16_t y, uint16_t kind, uint16_t z, uint32_t *p_depth, uint32_t *p_sign)
{
	uint32_t depth = *p_depth;
	uint32_t sign = *p_sign;
	uint32_t r1 = 0, free_count = 0;

	allocate_object_outside_loop(&free_count, &sign);
	if (free_count == 0u)
	{
		*p_sign = sign;
		return;
	}
	place_sign(x, y, kind, z, &r1, &sign);
	*p_depth = r1;
	*p_sign = sign;
}

uint32_t spawn_sign_2(uint16_t x, uint16_t y, uint16_t kind, uint16_t z, uint32_t sign)
{
	uint32_t free_count = 0, ignored = 0;

	allocate_object(&free_count, &sign);
	if (free_count == 0u)
		return sign;
	place_sign(x, y, kind, z, &ignored, &sign);
	return sign;
}
