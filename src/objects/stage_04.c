/**
 *    Name:        objects/stage_04.c
 *    Description: stage 4's objects: the vines, the catapults, arrow boards, pebbles, the propped board, boulders, the door wall at the start and the rollers behind it, the fireball emitter and the bouncing fireballs, the electric fence and the flame machine at the end
 *    Version:     0.1.0
 *    Created:     12-10-2026
 *    Modified:    15-10-2026
 *    Author:      Sebastian Toyos
 *
 *    SegaSonic The Hedgehog (arcade, Sega System 32): the game's V60 program decompiled into C by
 *    tools/decompile.py, with the names of symbols.txt.
 */
#include "game.h"

void vine_init(void)
{
	uint32_t block = 0, free_count = 0, ignored = 0, object = 0, object_2 = 0, object_3 = 0, piece_count = 0, spawn_result = 0;
	uint16_t speed_x_fraction = 0;

	OBJECT(current_object)->flags = 0u;
	OBJECT(current_object)->sprite_offset_x = 0;
	OBJECT(current_object)->sprite_offset_y = 0;
	OBJECT(current_object)->speed_z = 0x3FF;
	OBJECT(current_object)->vine_init.base_x = OBJECT(current_object)->x;
	OBJECT(current_object)->speed = (uint32_t)OBJECT(current_object)->vine_init.phase_step << 16;
	OBJECT(current_object)->speed_x_int = OBJECT(current_object)->vine_init.segment_count;
	OBJECT(current_object)->speed_x_fraction = OBJECT(current_object)->speed_x_int;
	OBJECT(current_object)->vine_init.depth_offset = OBJECT(current_object)->speed_x_int * 8u;
	spawn_count = OBJECT(current_object)->speed_x_int;
	spawn_count--;
	add_pieces_to_current_object(&spawn_result, &free_count, &object_2, &block);
	if ((uint8_t)spawn_result == 0u)
		return;
	object_3 = block;
	spawn_count = 2u;
	spawn_object_with_pieces(0x00ECFFu, &spawn_result, &piece_count, &free_count, &object_2, &block);
	if ((uint8_t)spawn_result == 0u)
		return;
	OBJECT(object_3)->child = object_2;
	OBJECT(object_2)->parent = object_3;
	OBJECT(object_2)->direction = object_cursor;
	OBJECT(object_2)->hitbox_size_x = 0x14u;
	OBJECT(object_2)->hitbox_height = 8u;
	OBJECT(object_2)->kind = 0xAu;
	OBJECT(object_2)->vine_bar.anchor_z = OBJECT(object_cursor)->z_int;
	OBJECT(object_2)->handler = vine_bar_update_ADDRESS;
	object_2 = LINKED(object_2, OBJECT(object_2)->child);
	OBJECT(object_2)->sprite_scale = 0x100u;
	OBJECT(object_2)->sprite_frame = 0x5D90u;
	OBJECT(object_2)->sprite_palette = 0x7A0u;
	OBJECT(object_2)->sprite_flags = 8u;
	OBJECT(current_object)->handler = vine_swing_ADDRESS;
	OBJECT(current_object)->speed_x_fraction = OBJECT(current_object)->speed_x_int;
	current_player = current_object;
	object = LINKED(block, OBJECT(current_object)->child);
	do
	{
		OBJECT(object)->projected_y = OBJECT(object_cursor)->projected_y;
		OBJECT(object)->sprite_flags = 0u;
		OBJECT(object)->sprite_palette = 0x8350u;
		OBJECT(object)->sprite_frame = 0x6E68u;
		OBJECT(current_player)->child = object;
		OBJECT(object)->parent = current_player;
		current_player = (current_player & ~0xFFFFu) | (uint16_t)object;
		object = LINKED(object, OBJECT(object)->child);
		speed_x_fraction = OBJECT(current_object)->speed_x_fraction;
		OBJECT(current_object)->speed_x_fraction = speed_x_fraction - 1u;
	} while ((int16_t)speed_x_fraction > 1);
	OBJECT(current_player)->sprite_frame = 0x6E70u;
	OBJECT(current_player)->sprite_palette = 0x10E0u;
	OBJECT(current_object)->floor_z = OBJECT(current_object)->z;
	project_object(&ignored, &ignored);
}

void vine_swing(void)
{
	uint8_t bucket = 0;
	uint16_t screen_x = 0;
	uint32_t block_offset = 0, ignored = 0, in_front = 0, point_x = 0, point_y = 0, segment_angle_int = 0, segment_depth = 0, slope_height = 0, z = 0;
	uint16_t speed_x_fraction = 0;

	OBJECT(current_object)->vine_swing.holding = 1u;
	if (vines_hold_players == 0u && OBJECT(current_object)->speed_z_int >= 0x9F && OBJECT(current_object)->speed_z_int <= 0x110)
		OBJECT(current_object)->vine_swing.holding = 0u;
	screen_x = (uint16_t)OBJECT(current_object)->screen_x;
	if ((int16_t)screen_x >= 0x200 || (int16_t)screen_x <= -0x200)
	{
		delete_object_and_children(&ignored, &block_offset, &slope_height);
		vines_hold_players = 0xFFu;
		return;
	}
	OBJECT(current_object)->speed_z += OBJECT(current_object)->speed;
	OBJECT(current_object)->speed_z_int &= 0x3FFu;
	OBJECT(current_object)->speed_y = (uint32_t)sine_table[(uint16_t)OBJECT(current_object)->speed_z_int] * OBJECT(current_object)->vine_swing.curl_scale;
	OBJECT(current_object)->z = OBJECT(current_object)->floor_z;
	update_screen_position(&ignored, &segment_depth);
	in_front = 0u;
	if (OBJECT(object_cursor)->vine_swing.segment_angle_int >= 0x100 && OBJECT(object_cursor)->vine_swing.segment_angle_int < 0x300)
		in_front = 1u;
	OBJECT(current_object)->vine_swing.segment_angle = 0u;
	OBJECT(current_object)->x = OBJECT(current_object)->vine_swing.base_x;
	OBJECT(current_object)->speed_x_fraction = OBJECT(current_object)->speed_x_int;
	OBJECT(object_cursor)->vine_swing.segment_angle += OBJECT(object_cursor)->speed_y;
	do
	{
		current_object = LINKED(current_object, OBJECT(current_object)->child);
		OBJECT(object_cursor)->vine_swing.segment_angle += OBJECT(object_cursor)->speed_y;
		OBJECT(object_cursor)->vine_swing.segment_angle_int &= 0x3FFu;
		segment_angle_int = (uint16_t)OBJECT(object_cursor)->vine_swing.segment_angle_int;
		OBJECT(object_cursor)->x += sine_table[segment_angle_int] * 0x18u;
		point_x = OBJECT(object_cursor)->x;
		OBJECT(object_cursor)->z += cosine_table[segment_angle_int] * 0x18u;
		z = OBJECT(object_cursor)->z;
		point_y = OBJECT(object_cursor)->y;
		project_point(z, &ignored, &point_x, &point_y);
		OBJECT(current_object)->screen_x = point_x;
		OBJECT(current_object)->screen_y = point_y;
		if (in_front == 0u)
		{
			segment_depth = (uint16_t)(OBJECT(object_cursor)->screen_y - (OBJECT(object_cursor)->x_int - OBJECT(object_cursor)->vine_swing.base_x_int) + OBJECT(object_cursor)->vine_swing.depth_offset);
			queue_sprite_at_depth(&ignored, &segment_depth);
		}
		else
		{
			bucket = 0u;
			queue_sprite_in_bucket(bucket);
		}
		speed_x_fraction = OBJECT(object_cursor)->speed_x_fraction;
		OBJECT(object_cursor)->speed_x_fraction = speed_x_fraction - 1u;
	} while ((int16_t)speed_x_fraction > 1);
}

void vine_bar_update(void)
{
	uint16_t kind = 0, screen_dy = 0, screen_x = 0, x_int = 0, y_int = 0, z = 0;
	uint32_t r11 = 0, depth = 0, height = 0, ignored = 0, vine = 0;

	OBJECT(current_object)->vine_bar.holding = mem_read8((0x200000u | OBJECT(current_object)->direction) + 0x80u);
	if ((OBJECT(current_object)->flags & 8u) != 0u && OBJECT(current_object)->floor_z_int < 0x800)
	{
		current_object = LINKED(current_object, OBJECT(object_cursor)->child);
		OBJECT(current_object)->screen_x = OBJECT(object_cursor)->screen_x;
		OBJECT(current_object)->screen_y = OBJECT(object_cursor)->screen_y;
		OBJECT(current_object)->screen_y += OBJECT(object_cursor)->floor_z_int - OBJECT(object_cursor)->z_int;
		queue_sprite(0u);
		current_object = object_cursor;
	}
	vine = 0x200000u | OBJECT(current_object)->direction;
	OBJECT(current_object)->x = mem_read32(vine + 0x4Cu);
	OBJECT(current_object)->y = mem_read32(vine + 0x50u);
	OBJECT(current_object)->z = mem_read32(vine + 0x40u);
	screen_x = (uint16_t)OBJECT(current_object)->screen_x;
	screen_dy = (uint16_t)OBJECT(current_object)->screen_y;
	project_object(&ignored, &depth);
	screen_dy -= OBJECT(current_object)->screen_y;
	OBJECT(current_object)->projected_x += screen_x - OBJECT(current_object)->screen_x;
	OBJECT(current_object)->projected_y_int += screen_dy;
	unproject_object(&ignored, &ignored, &ignored, &r11);
	height = (OBJECT(object_cursor)->floor_z_int - OBJECT(object_cursor)->z_int) & 0x7Fu;
	if ((int16_t)height > 0x50)
	{
		AT8(OBJECT(object_cursor)->hitbox_size_y, 1) = 0u;
		OBJECT(object_cursor)->hitbox_height = 8u;
	}
	else
	{
		AT8(OBJECT(object_cursor)->hitbox_size_y, 1) = height;
		OBJECT(object_cursor)->hitbox_height = height;
	}
	OBJECT(current_object)->health = 0x7F;
	add_to_touchable_list(height);
	add_to_rider_list();
	current_object = object_cursor;
	if (OBJECT(current_object)->floor_z_int < 0x800 && mem_read8((0x200000u | OBJECT(current_object)->direction) + 0x80u) == 1u)
	{
		x_int = (uint16_t)OBJECT(current_object)->x_int;
		y_int = (uint16_t)OBJECT(current_object)->y_int;
		kind = 7u;
		z = (uint16_t)(OBJECT(current_object)->z_int + 0x14u);
		spawn_sign(x_int, y_int, kind, z, &depth, &r11);
	}
}

void vine_scroll_stop_init(void)
{
	vines_hold_players = 1u;
	OBJECT(current_object)->handler = vine_scroll_stop_wait_ADDRESS;
	vine_scroll_stop_wait();
}

void vine_scroll_stop_wait(void)
{
	if ((int16_t)scroll_position < (int16_t)OBJECT(current_object)->spawn_arg_1)
	{
		vine_scroll_stop_hold();
		return;
	}
	scroll_mode = 1u;
	saved_scroll_mode = 1u;
	OBJECT(current_object)->handler = vine_scroll_stop_hold_ADDRESS;
	vine_scroll_stop_hold();
}

void vine_scroll_stop_hold(void)
{
	uint32_t all_clear = 0, player_index = 0, player_object = 0;

	player_index = 3u;
	all_clear = 1u;
	do
	{
		player_index--;
		if (player_index == 0xFFFFFFFFu)
		{
			if (all_clear == 0u)
			{
				vine_scroll_stop_end();
				return;
			}
			scroll_mode = 0u;
			saved_scroll_mode = 0u;
			scroll_speed = 8u;
			OBJECT(current_object)->handler = vine_scroll_stop_end_ADDRESS;
			vine_scroll_stop_end();
			return;
		}
		player_object = GAME_VARIABLES + vine_scroll_stop_player_blocks[player_index];
		player_object = (player_object & ~0xFFFFu) | mem_read16(player_object + 0x14u);
	} while ((mem_read32(player_object + 0x30u) & 0x400u) != 0u || (mem_read32(player_object + 0x30u) & 0x10000u) != 0u || (int16_t)mem_read16(player_object + 0x4Eu) >= -0x120);
	all_clear = 0u;
	if (all_clear == 0u)
	{
		vine_scroll_stop_end();
		return;
	}
	scroll_mode = 0u;
	saved_scroll_mode = 0u;
	scroll_speed = 8u;
	OBJECT(current_object)->handler = vine_scroll_stop_end_ADDRESS;
	vine_scroll_stop_end();
}

void vine_scroll_stop_end(void)
{
	uint32_t block_offset = 0, ignored = 0, slope_height = 0;

	if ((int16_t)OBJECT(current_object)->spawn_arg_2 < (int16_t)scroll_position)
	{
		scroll_mode = 7u;
		saved_scroll_mode = 7u;
		vines_hold_players = 0u;
		delete_object_and_children(&ignored, &block_offset, &slope_height);
	}
}

void flame_machine_init(void)
{
	uint16_t back_wall = 0;
	uint32_t block = 0, free_count = 0, object = 0, piece_count = 0, spawned = 0;

	flame_machine_flags = 0u;
	OBJECT(current_object)->sprite_frame = 0x6258u;
	OBJECT(current_object)->sprite_palette = 0x91F0u;
	OBJECT(current_object)->sprite_flags = 0u;
	OBJECT(current_object)->flags |= 2u;
	OBJECT(current_object)->hitbox_size_x = 0x116u;
	OBJECT(current_object)->hitbox_size_y = 0x70u;
	OBJECT(current_object)->hitbox_offset_x = -0x7F;
	OBJECT(current_object)->hitbox_offset_y = 4;
	OBJECT(current_object)->hitbox_height = 0x100u;
	OBJECT(current_object)->kind = 0xCu;
	OBJECT(current_object)->health = 0x28;
	OBJECT(current_object)->flame_machine_init.time_left = 0xE10u;
	OBJECT(current_object)->handler = flame_machine_update_ADDRESS;
	current_object = LINKED(current_object, OBJECT(current_object)->child);
	OBJECT(current_object)->sprite_frame = 0x6260u;
	OBJECT(current_object)->sprite_flags = 8u;
	current_object = LINKED(current_object, OBJECT(current_object)->child);
	OBJECT(current_object)->sprite_frame = 0x6B20u;
	OBJECT(current_object)->sprite_palette = 0x9240u;
	current_object = LINKED(current_object, OBJECT(current_object)->child);
	OBJECT(current_object)->sprite_frame = 0x6B18u;
	OBJECT(current_object)->sprite_palette = 0x9230u;
	current_object = LINKED(current_object, OBJECT(current_object)->child);
	OBJECT(current_object)->sprite_frame = 0x6970u;
	OBJECT(current_object)->sprite_palette = 0x9FFEu;
	r14 = current_object;
	spawn_count = 3u;
	spawn_object_with_pieces(0x00FC8Fu, &spawned, &piece_count, &free_count, &object, &block);
	OBJECT(r14)->child = object;
	copy_position_to(object);
	OBJECT(object)->sprite_frame = 0x6268u;
	OBJECT(object)->sprite_palette = 0x9200u;
	OBJECT(object)->sprite_flags = 0u;
	OBJECT(object)->handler = flame_machine_back_wall_update_ADDRESS;
	back_wall = (uint16_t)object;
	object = LINKED(object, OBJECT(object)->child);
	OBJECT(object)->sprite_palette = 0x91E0u;
	OBJECT(object)->sprite_flags = 5u;
	object = LINKED(object, OBJECT(object)->child);
	OBJECT(object)->sprite_palette = 0x91E0u;
	OBJECT(object)->sprite_flags = 5u;
	r14 = object;
	spawn_count = 2u;
	spawn_object_with_pieces(0x00FCE4u, &spawned, &piece_count, &free_count, &object, &block);
	copy_position_to(object);
	OBJECT(r14)->child = object;
	OBJECT(object)->parent = back_wall;
	OBJECT(object)->spawn_arg_1 = 0xFFF5u;
	OBJECT(object)->spawn_arg_2 = 0x36u;
	OBJECT(object)->sprite_palette = 0x9220u;
	OBJECT(object)->direction = 1u;
	OBJECT(object)->handler = flame_machine_arm_update_ADDRESS;
	object = LINKED(object, OBJECT(object)->child);
	OBJECT(object)->sprite_palette = 0x9220u;
	r14 = object;
	spawn_count = 2u;
	spawn_object_with_pieces(0x00FD2Eu, &spawned, &piece_count, &free_count, &object, &block);
	copy_position_to(object);
	OBJECT(r14)->child = object;
	OBJECT(object)->parent = back_wall;
	OBJECT(object)->spawn_arg_1 = 0x40u;
	OBJECT(object)->spawn_arg_2 = 0x11u;
	OBJECT(object)->sprite_palette = 0x9220u;
	OBJECT(object)->direction = 0x1FFu;
	OBJECT(object)->handler = flame_machine_arm_update_ADDRESS;
	object = LINKED(object, OBJECT(object)->child);
	OBJECT(object)->sprite_palette = 0x9220u;
	r14 = object;
	spawn_count = 2u;
	spawn_objects(0x00FD7Au, &spawned, &object, &block);
	OBJECT(r14)->child = object;
	copy_position_to(object);
	OBJECT(object)->projected_x += 0x29u;
	OBJECT(object)->projected_y_int -= 0x7Eu;
	OBJECT(object)->sprite_frame = 0x61C8u;
	OBJECT(object)->sprite_palette = 0x91D0u;
	OBJECT(object)->sprite_flags = 8u;
	OBJECT(object)->spawn_arg_1 = 0u;
	OBJECT(object)->flags |= 2u;
	OBJECT(object)->hitbox_size_x = 0x11Eu;
	OBJECT(object)->hitbox_size_y = 0x30u;
	OBJECT(object)->hitbox_offset_x = -0x33;
	OBJECT(object)->hitbox_offset_y = 0x68;
	OBJECT(object)->hitbox_height = 0x50u;
	OBJECT(object)->kind = 0xCu;
	OBJECT(object)->handler = flame_machine_cannon_update_ADDRESS;
	object = LINKED(object, OBJECT(object)->child);
	copy_position_to(object);
	OBJECT(object)->projected_x -= 0x63u;
	OBJECT(object)->projected_y_int += 0x15u;
	OBJECT(object)->sprite_frame = 0x61D0u;
	OBJECT(object)->sprite_palette = 0x91D0u;
	OBJECT(object)->spawn_arg_1 = 1u;
	OBJECT(object)->sprite_flags = 0u;
	OBJECT(object)->flags |= 2u;
	OBJECT(object)->hitbox_size_x = 0x11Eu;
	OBJECT(object)->hitbox_size_y = 0x30u;
	OBJECT(object)->hitbox_offset_x = -0x33;
	OBJECT(object)->hitbox_offset_y = 0x68;
	OBJECT(object)->hitbox_height = 0x50u;
	OBJECT(object)->kind = 0xCu;
	OBJECT(object)->handler = flame_machine_cannon_update_ADDRESS;
	flame_machine_flags |= 2u;
}

void flame_machine_update(void)
{
	uint8_t sound_id = 0;
	uint32_t r0 = 0, r2 = 0, r11 = 0, depth = 0, ignored = 0, object = 0, object_2 = 0, random_scale = 0, spawned = 0;
	uint8_t flash_steps_left = 0, odd_frame = 0;
	uint16_t flash_count = 0, time_left = 0, explosion_phase = 0;

	if (active_player_count == 0u)
	{
	flame_machine_draw:
		unproject_object(&ignored, &r2, &random_scale, &r11);
		depth = queue_sprite(0u);
		add_to_touchable_list(ignored);
		if ((uint8_t)OBJECT(current_object)->flame_machine_update.previous_health != (uint8_t)OBJECT(current_object)->health)
		{
			OBJECT(current_object)->flame_machine_update.previous_health = OBJECT(current_object)->health;
			flame_machine_flags |= 0x1000u;
			flame_machine_flash_step = 4;
			sound_id = 0x21u;
			play_sound(sound_id, &depth, &r2);
		}
		if ((flame_machine_flags & 0x1000u) != 0u)
		{
			flash_count = OBJECT(current_object)->flame_machine_update.flash_frame_count + 1u;
			OBJECT(current_object)->flame_machine_update.flash_frame_count = flash_count;
			if (flash_count != 0u)
			{
				flash_steps_left = flame_machine_flash_step;
				flame_machine_flash_step = flash_steps_left - 1u;
				if (flash_steps_left == 0u)
					flame_machine_flags &= ~0x1000u;
				else
				{
					OBJECT(current_object)->sprite_palette = flame_machine_hit_palettes[(uint8_t)flame_machine_flash_step];
					sound_id = 0x1Eu;
					play_sound(sound_id, &depth, &r2);
				}
			}
		}
		current_object = LINKED(current_object, OBJECT(current_object)->child);
		OBJECT(current_object)->screen_x = OBJECT(object_cursor)->screen_x;
		OBJECT(current_object)->screen_y = OBJECT(object_cursor)->screen_y;
		OBJECT(current_object)->screen_x -= 2u;
		OBJECT(current_object)->screen_y -= 0x40u;
		OBJECT(current_object)->sprite_palette = OBJECT(object_cursor)->sprite_palette;
		queue_sprite(depth);
		current_object = LINKED(current_object, OBJECT(current_object)->child);
		AT8(OBJECT(current_object)->direction, 1)++;
		odd_frame = AT8(OBJECT(current_object)->direction, 1) & 1u;
		AT8(OBJECT(current_object)->direction, 1) = odd_frame;
		if (odd_frame != 0u)
		{
			AS8(OBJECT(current_object)->direction)++;
			if ((int8_t)AS8(OBJECT(current_object)->direction) > 6)
				AS8(OBJECT(current_object)->direction) = 0u;
			OBJECT(current_object)->sprite_frame = flame_machine_capsule_sprites[AS8(OBJECT(current_object)->direction)];
		}
		OBJECT(current_object)->screen_x = OBJECT(object_cursor)->screen_x;
		depth = (uint16_t)OBJECT(object_cursor)->screen_y;
		OBJECT(current_object)->screen_y = depth;
		OBJECT(current_object)->screen_x += 0x2Du;
		OBJECT(current_object)->screen_y += 0x1Fu;
		depth += 2u;
		queue_sprite_at_depth(&ignored, &depth);
		current_object = LINKED(current_object, OBJECT(current_object)->child);
		OBJECT(current_object)->screen_x = OBJECT(object_cursor)->screen_x;
		depth = (uint16_t)OBJECT(object_cursor)->screen_y;
		OBJECT(current_object)->screen_y = depth;
		OBJECT(current_object)->screen_x += 0x2Du;
		OBJECT(current_object)->screen_y += 0x2Bu;
		depth++;
		queue_sprite_at_depth(&ignored, &depth);
		current_object = LINKED(current_object, OBJECT(current_object)->child);
		r0 = place_sprite_at_cursor_floor();
		queue_sprite_in_bucket(r0);
		if (OBJECT(object_cursor)->health > 0)
			return;
	}
	else
	{
		time_left = OBJECT(object_cursor)->flame_machine_update.time_left;
		OBJECT(object_cursor)->flame_machine_update.time_left = time_left - 1u;
		if (time_left != 0u)
			goto flame_machine_draw;
	}
	goal_unlocked = 0xFFu;
	flame_machine_flags |= 0x100000u;
	current_object = object_cursor;
	OBJECT(current_object)->health = 0;
	OBJECT(current_object)->flame_machine_update.destroyed_frames++;
	if (OBJECT(current_object)->flame_machine_update.destroyed_frames != 0x130u)
	{
		explosion_phase = OBJECT(current_object)->flame_machine_update.destroyed_frames & 7u;
		spawned = explosion_phase;
		if (explosion_phase != 0u)
			return;
		spawn_count = 1u;
		spawn_object_with_pieces(0x00FFC1u, &spawned, &depth, &r2, &random_scale, &r11);
		if ((uint8_t)spawned == 0u)
			return;
		object_2 = random_scale;
		copy_position_to(random_scale);
		next_random(&random_scale, &ignored, &ignored);
		random_scale = (random_scale & ~0xFFFFu) | (random_scale & 0x3FFu);
		OBJECT(object_2)->sprite_scale += random_scale;
		random_scale = (random_scale & ~0xFFFFu) | (random_scale & 0x7Fu);
		OBJECT(object_2)->projected_y_int += 0x13u;
		OBJECT(object_2)->projected_y_int += random_scale;
		next_random(&random_scale, &ignored, &ignored);
		OBJECT(object_2)->projected_x += random_scale & 0x7Fu;
		OBJECT(object_2)->projected_x -= 0x53u;
		OBJECT(object_2)->handler = flame_machine_explosion_start_ADDRESS;
		return;
	}
	OBJECT(current_object)->handler = flame_machine_remove_ADDRESS;
	spawn_count = 3u;
	spawn_object_with_pieces(0x010820u, &r0, &depth, &r2, &random_scale, &r11);
	OBJECT(random_scale)->projected_x = 0x1B2F;
	OBJECT(random_scale)->projected_y_int = 0x14F;
	OBJECT(random_scale)->z = 0;
	OBJECT(random_scale)->sprite_frame = 0x8360u;
	OBJECT(random_scale)->sprite_palette = 0x87D0u;
	OBJECT(random_scale)->handler = flame_machine_wreck_draw_ADDRESS;
	object = LINKED(random_scale, OBJECT(random_scale)->child);
	OBJECT(object)->sprite_frame = 0x8370u;
	OBJECT(object)->sprite_palette = 0x87E0u;
	object = LINKED(object, OBJECT(object)->child);
	OBJECT(object)->sprite_frame = 0x8368u;
	OBJECT(object)->sprite_palette = 0x87E0u;
	current_object = object_cursor;
}

void flame_machine_update_spare_return(void)
{
}

void flame_machine_remove(void)
{
	uint32_t block_offset = 0, ignored = 0, slope_height = 0;

	electric_fence_state = 0xFFu;
	delete_object_and_children(&ignored, &block_offset, &slope_height);
}

void flame_machine_explosion_start(void)
{
	uint16_t count = 0;

	OBJECT(current_object)->speed = 0x20;
	OBJECT(current_object)->direction = 0x80u;
	count = 8u;
	spawn_debris(0u, count, 0u);
	explode_object();
}

void flame_machine_back_wall_update(void)
{
	uint16_t rod_sprite = 0;
	uint32_t cursor = 0, depth = 0, frame_index = 0, ignored = 0, object = 0, offset_record = 0, wall_frame = 0;
	uint16_t next_phase = 0;

	if ((flame_machine_flags & 0x100000u) == 0u)
	{
		move_object_projected(&ignored, &ignored, &ignored, &cursor);
		if ((flame_machine_flags & 0x10u) == 0u && (flame_machine_flags & 0x20u) == 0u)
		{
			if (OBJECT(current_object)->flame_machine_back_wall.pause_timer > 0)
				OBJECT(current_object)->flame_machine_back_wall.pause_timer--;
			else
			{
				OBJECT(current_object)->flame_machine_back_wall.frame_phase++;
				next_phase = OBJECT(current_object)->flame_machine_back_wall.frame_phase & 3u;
				OBJECT(current_object)->flame_machine_back_wall.frame_phase = next_phase;
				if (next_phase == 0u)
				{
					OBJECT(current_object)->flame_machine_back_wall.frame_phase = 1u;
					OBJECT(current_object)->flame_machine_back_wall.frame_index++;
					frame_index = (uint8_t)OBJECT(current_object)->flame_machine_back_wall.frame_index;
					if ((int8_t)frame_index > 9)
					{
						frame_index = 0u;
						OBJECT(current_object)->flame_machine_back_wall.frame_index = 0;
						OBJECT(current_object)->flame_machine_back_wall.pause_timer = 0x1E;
						flame_machine_flags |= 4u;
						flame_machine_flags |= 8u;
						flame_machine_flags |= 0x400u;
						flame_machine_flags |= 0x800u;
						flame_machine_flags &= ~0x200u;
					}
					wall_frame = flame_machine_back_wall_frames[frame_index];
					OBJECT(current_object)->flame_machine_back_wall.frame = wall_frame;
					rod_sprite = flame_machine_rod_sprites[wall_frame];
					current_object = LINKED(current_object, OBJECT(current_object)->child);
					OBJECT(current_object)->sprite_frame = rod_sprite;
					current_object = LINKED(current_object, OBJECT(current_object)->child);
					OBJECT(current_object)->sprite_frame = rod_sprite;
					offset_record = flame_machine_back_wall_offsets_ADDRESS + wall_frame * 4u;
					current_object = LINKED(current_object, OBJECT(object_cursor)->parent);
					OBJECT(object_cursor)->projected_x = OBJECT(current_object)->projected_x;
					OBJECT(object_cursor)->projected_y = OBJECT(current_object)->projected_y;
					OBJECT(object_cursor)->projected_x += ROM16(offset_record);
					OBJECT(object_cursor)->projected_y_int += ROM16(offset_record + 2u);
					if (wall_frame == 5u)
					{
						flame_machine_flags |= 0x10u;
						flame_machine_flags |= 0x20u;
					}
				}
			}
		}
	}
	current_object = object_cursor;
	queue_sprite(wall_frame);
	if ((flame_machine_flags & 0x100000u) == 0u)
	{
		if (OBJECT(current_object)->flame_machine_back_wall.frame == 5u)
			return;
		current_object = LINKED(current_object, OBJECT(object_cursor)->child);
		object = LINKED(WORK_RAM, OBJECT(object_cursor)->parent);
		OBJECT(current_object)->screen_x = OBJECT(object)->screen_x;
		OBJECT(current_object)->screen_y = OBJECT(object)->screen_y;
		OBJECT(current_object)->screen_x += 0xEu;
		OBJECT(current_object)->screen_y += 0x17u;
		current_object = LINKED(current_object, OBJECT(current_object)->child);
		OBJECT(current_object)->screen_x = OBJECT(object)->screen_x;
		OBJECT(current_object)->screen_y = OBJECT(object)->screen_y;
		OBJECT(current_object)->screen_x += 0x36u;
		OBJECT(current_object)->screen_y += 3u;
	}
	current_object = LINKED(current_object, OBJECT(object_cursor)->child);
	depth = (uint16_t)(OBJECT(object_cursor)->screen_y + 1u);
	queue_sprite_at_depth(&ignored, &depth);
	current_object = LINKED(current_object, OBJECT(current_object)->child);
	depth = (uint16_t)(OBJECT(object_cursor)->screen_y + 1u);
	queue_sprite_at_depth(&ignored, &depth);
}

void flame_machine_arm_update(void)
{
	uint32_t r2 = 0, r3 = 0, r11 = 0, frame_number = 0, ignored = 0;
	uint8_t odd_sway_frame = 0;
	uint16_t odd_frame = 0;

	if ((flame_machine_flags & 0x100000u) != 0u)
		return;
	unproject_object(&ignored, &r2, &r3, &r11);
	if ((flame_machine_flags & shl32(1u, (int)(OBJECT(current_object)->flame_machine_arm.arm_index + 0xAu))) != 0u)
		flame_machine_arm_take_disc(r2, r3, r11);
	if ((flame_machine_flags & shl32(1u, (int)(OBJECT(current_object)->flame_machine_arm.arm_index + 4u))) == 0u)
	{
		flame_machine_arm_draw();
		return;
	}
	if ((flame_machine_flags & shl32(1u, (int)(OBJECT(current_object)->flame_machine_arm.arm_index + 6u))) == 0u)
	{
		OBJECT(current_object)->flame_machine_arm.frame_toggle++;
		odd_frame = OBJECT(current_object)->flame_machine_arm.frame_toggle & 1u;
		OBJECT(current_object)->flame_machine_arm.frame_toggle = odd_frame;
		if (odd_frame != 0u)
		{
			flame_machine_arm_draw();
			return;
		}
		OBJECT(current_object)->flame_machine_arm.frame_index++;
		frame_number = flame_machine_arm_frames[OBJECT(current_object)->flame_machine_arm.frame_index];
		if (frame_number != 0xCu)
		{
			if (frame_number != 0xDu)
			{
				OBJECT(current_object)->sprite_frame = flame_machine_arm_sprites[frame_number];
				current_object = LINKED(current_object, OBJECT(current_object)->child);
				OBJECT(current_object)->sprite_frame = flame_machine_arm_part_sprites[frame_number];
				current_object = object_cursor;
				flame_machine_arm_draw();
				return;
			}
			OBJECT(current_object)->flame_machine_arm.frame_index = 0u;
			flame_machine_flags &= ~shl32(1u, (int)(OBJECT(current_object)->flame_machine_arm.arm_index + 4u));
			flame_machine_arm_draw();
			return;
		}
		flame_machine_flags |= shl32(1u, (int)(OBJECT(current_object)->flame_machine_arm.arm_index + 6u));
		OBJECT(current_object)->sprite_frame = flame_machine_arm_sprites[frame_number];
		current_object = LINKED(current_object, OBJECT(current_object)->child);
		OBJECT(current_object)->sprite_frame = flame_machine_arm_part_sprites[frame_number];
		current_object = object_cursor;
		flame_machine_arm_draw();
		return;
	}
	OBJECT(current_object)->flame_machine_arm.sway_toggle++;
	odd_sway_frame = OBJECT(current_object)->flame_machine_arm.sway_toggle & 1u;
	OBJECT(current_object)->flame_machine_arm.sway_toggle = odd_sway_frame;
	if (odd_sway_frame != 0u)
	{
		flame_machine_arm_draw();
		return;
	}
	OBJECT(current_object)->flame_machine_arm.sway_count++;
	if (OBJECT(current_object)->flame_machine_arm.sway_count > 0x26)
	{
		flame_machine_flags |= 0x200u;
		OBJECT(current_object)->var_84 = 0x1Eu;
		OBJECT(current_object)->handler = flame_machine_arm_rest_ADDRESS;
		flame_machine_arm_rest();
		return;
	}
	OBJECT(current_object)->flame_machine_arm.sway_step++;
	OBJECT(current_object)->flame_machine_arm.sway_step = OBJECT(current_object)->flame_machine_arm.sway_step % 0x29u;
	OBJECT(current_object)->flame_machine_arm.sway_count++;
	flame_machine_arm_draw();
}

void flame_machine_arm_rest(void)
{
	OBJECT(current_object)->var_84--;
	if ((int16_t)OBJECT(current_object)->var_84 > 0)
	{
		flame_machine_arm_draw();
		return;
	}
	flame_machine_flags &= ~shl32(1u, (int)(OBJECT(current_object)->flame_machine_arm.arm_index + 6u));
	OBJECT(current_object)->flame_machine_arm.frame_toggle = 1u;
	OBJECT(current_object)->handler = flame_machine_arm_update_ADDRESS;
	flame_machine_arm_draw();
}

void flame_machine_arm_draw(void)
{
	uint8_t bucket = 0;
	uint16_t side = 0, sway_dy = 0;
	uint32_t depth = 0, ignored = 0, object = 0, sway_offset = 0;

	current_object = LINKED(current_object, OBJECT(object_cursor)->parent);
	OBJECT(object_cursor)->projected_x = OBJECT(current_object)->projected_x;
	OBJECT(object_cursor)->projected_y = OBJECT(current_object)->projected_y;
	OBJECT(object_cursor)->projected_x += OBJECT(object_cursor)->spawn_arg_1;
	OBJECT(object_cursor)->projected_y_int += OBJECT(object_cursor)->spawn_arg_2;
	current_object = object_cursor;
	sway_offset = flame_machine_arm_sway_offsets_ADDRESS + OBJECT(current_object)->flame_machine_arm.sway_step * 2u;
	side = (uint16_t)OBJECT(current_object)->flame_machine_arm.side;
	sway_dy = (uint16_t)((uint32_t)(int8_t)ROM8(sway_offset + 1u) * side);
	OBJECT(current_object)->projected_x += (uint32_t)(int8_t)ROM8(sway_offset) * side;
	OBJECT(current_object)->projected_y_int += sway_dy;
	object = LINKED(WORK_RAM, OBJECT(current_object)->parent);
	object = LINKED(object, OBJECT(object)->child);
	object = LINKED(object, OBJECT(object)->child);
	object = LINKED(object, OBJECT(object)->child);
	object = LINKED(object, OBJECT(object)->child);
	depth = (uint16_t)(OBJECT(LINKED(object, OBJECT(object)->child))->screen_y - 1u);
	r14 = depth;
	queue_sprite_at_depth(&ignored, &depth);
	current_object = LINKED(current_object, OBJECT(current_object)->child);
	OBJECT(current_object)->screen_x = OBJECT(object_cursor)->screen_x;
	OBJECT(current_object)->screen_y = OBJECT(object_cursor)->screen_y;
	bucket = (uint8_t)((uint32_t)OBJECT(object_cursor)->flame_machine_arm.arm_index << 2);
	queue_sprite_in_bucket(bucket);
}

void flame_machine_cannon_update(void)
{
	uint32_t r0 = 0, r2 = 0, r11 = 0, ignored = 0, object = 0, offset_record = 0, spawn_arg_1 = 0;

	unproject_object(&ignored, &r2, &object, &r11);
	queue_sprite(0u);
	OBJECT(current_object)->health = 0x7F;
	add_to_touchable_list(ignored);
	if (flame_machine_flash_step >= 0)
		OBJECT(current_object)->sprite_palette = flame_machine_cannon_hit_palettes[(uint8_t)flame_machine_flash_step];
	r0 = OBJECT(current_object)->spawn_arg_1 + 2u;
	if ((flame_machine_flags & shl32(1u, (int)(r0))) == 0u)
		return;
	spawn_count = 2u;
	spawn_object_with_pieces(0x010440u, &r0, &offset_record, &r2, &object, &r11);
	if ((uint8_t)r0 != 0u)
	{
		copy_position_to(object);
		spawn_arg_1 = OBJECT(current_object)->spawn_arg_1;
		offset_record = flame_machine_flame_offsets_ADDRESS + spawn_arg_1 * 4u;
		OBJECT(object)->direction = flame_machine_flame_directions[spawn_arg_1];
		OBJECT(object)->projected_x += ROM16(offset_record);
		OBJECT(object)->projected_y_int += ROM16(offset_record + 2u);
		OBJECT(object)->sprite_frame = 0x6910u;
		OBJECT(object)->sprite_palette = 0x7B0u;
		OBJECT(object)->flags |= 2u;
		OBJECT(object)->hitbox_size_x = 0x30u;
		OBJECT(object)->hitbox_size_y = 0xF8u;
		OBJECT(object)->hitbox_offset_x = -8;
		OBJECT(object)->hitbox_offset_y = -5;
		OBJECT(object)->hitbox_height = 0x28u;
		OBJECT(object)->kind = 0x17u;
		OBJECT(object)->sprite_flags = 0xAu;
		OBJECT(object)->handler = flame_machine_flame_burn_ADDRESS;
		object = LINKED(object, OBJECT(object)->child);
		OBJECT(object)->sprite_palette = 0x9FFEu;
		OBJECT(object)->sprite_flags = 0xAu;
		flame_machine_flags &= ~shl32(1u, (int)(spawn_arg_1 + 2u));
	}
}

void flame_machine_flame_burn(void)
{
	uint32_t block_offset = 0, bucket = 0, cursor = 0, depth = 0, flame_frame = 0, ignored = 0, slope_height = 0;
	uint16_t odd_frame = 0;

	OBJECT(object_cursor)->z_int -= 0x20u;
	unproject_object(&ignored, &block_offset, &slope_height, &cursor);
	OBJECT(current_object)->flame_machine_flame.frame_toggle++;
	odd_frame = OBJECT(current_object)->flame_machine_flame.frame_toggle & 1u;
	OBJECT(current_object)->flame_machine_flame.frame_toggle = odd_frame;
	if (odd_frame != 0u)
	{
		OBJECT(current_object)->flame_machine_flame.frame_index++;
		flame_frame = flame_machine_flame_frames[OBJECT(current_object)->flame_machine_flame.frame_index];
		OBJECT(current_object)->spawn_arg_2 = flame_frame;
		OBJECT(current_object)->sprite_frame = flame_machine_flame_sprites[flame_frame];
		if (flame_frame == 0xBu)
		{
			delete_object_and_children(&ignored, &block_offset, &slope_height);
			return;
		}
		if ((int16_t)OBJECT(current_object)->spawn_arg_2 >= 2)
		{
			OBJECT(current_object)->health = 0x7F;
			add_to_touchable_list(ignored);
		}
	}
	depth = (uint16_t)(OBJECT(current_object)->screen_y + 0x60u);
	queue_sprite_at_depth(&ignored, &depth);
	if ((int16_t)OBJECT(current_object)->spawn_arg_2 >= 2 && (int16_t)OBJECT(current_object)->spawn_arg_2 <= 4)
	{
		current_object = LINKED(current_object, OBJECT(current_object)->child);
		OBJECT(current_object)->sprite_frame = flame_machine_flame_shadow_sprites[OBJECT(object_cursor)->spawn_arg_2 - 2u];
		bucket = place_sprite_at_cursor_floor();
		queue_sprite_in_bucket(bucket);
	}
	OBJECT(object_cursor)->z_int += 0x20u;
	if ((flame_machine_flags & 0x100000u) == 0u)
		return;
	current_object = object_cursor;
	explode_object();
}

void flame_machine_arm_take_disc(uint32_t free_count, uint32_t disc, uint32_t block)
{
	uint32_t disc_flag_bit = 0, object = 0, piece_count = 0, spawned = 0;

	spawn_count = 2u;
	spawn_object_with_pieces(0x01061Fu, &spawned, &piece_count, &free_count, &disc, &block);
	copy_position_to(disc);
	OBJECT(disc)->direction = 0x200u;
	OBJECT(disc)->sprite_scale = 0xC0u;
	OBJECT(disc)->sprite_frame = 0x6278u;
	OBJECT(disc)->sprite_palette = 0x1210u;
	OBJECT(disc)->sprite_flags = 0u;
	OBJECT(disc)->flags |= 2u;
	OBJECT(disc)->hitbox_size_x = 0x18u;
	OBJECT(disc)->hitbox_size_y = 0x3Cu;
	OBJECT(disc)->hitbox_offset_x = -0x31;
	OBJECT(disc)->hitbox_offset_y = 3;
	OBJECT(disc)->hitbox_height = 0x28u;
	OBJECT(disc)->kind = 0x24u;
	OBJECT(disc)->handler = spiked_disc_hold_ADDRESS;
	disc_flag_bit = OBJECT(current_object)->flame_machine_arm.arm_index + 0xAu;
	OBJECT(disc)->spawn_arg_1 = OBJECT(current_object)->flame_machine_arm.arm_index;
	flame_machine_flags &= ~shl32(1u, (int)(disc_flag_bit));
	object = LINKED(disc, OBJECT(disc)->child);
	OBJECT(object)->sprite_frame = 0x6988u;
	OBJECT(object)->sprite_palette = 0x9FFEu;
}

void spiked_disc_hold(void)
{
	uint32_t arm_frame_index = 0, offset_record = 0;

	if ((flame_machine_flags & 0x200u) != 0u)
		OBJECT(current_object)->handler = spiked_disc_drop_ADDRESS;
	current_object = LINKED(current_object, OBJECT(object_cursor)->parent);
	copy_position_to_cursor();
	arm_frame_index = OBJECT(current_object)->var_80;
	OBJECT(object_cursor)->spiked_disc.arm_frame = arm_frame_index;
	offset_record = spiked_disc_hold_offsets_ADDRESS + arm_frame_index * 4u;
	OBJECT(object_cursor)->projected_x += ROM16(offset_record);
	OBJECT(object_cursor)->z_int += ROM16(offset_record + 2u);
	current_object = object_cursor;
	spiked_disc_draw_held();
}

void spiked_disc_drop(void)
{
	uint32_t r1 = 0, height = 0, z = 0;

	fall_to_floor(&height, &r1, &z);
	if (height != 0u)
	{
		spiked_disc_draw_held();
		return;
	}
	OBJECT(current_object)->handler = spiked_disc_roll_ADDRESS;
	spiked_disc_draw_held();
}

void spiked_disc_draw_held(void)
{
	uint8_t bucket = 0;
	uint32_t cursor = 0, ignored = 0;

	move_object_projected(&ignored, &ignored, &ignored, &cursor);
	bucket = (uint8_t)((OBJECT(current_object)->spawn_arg_1 + 1u) * 2u);
	queue_sprite_in_bucket(bucket);
	if ((int16_t)AS16(OBJECT(current_object)->spiked_disc.arm_frame) >= 8)
	{
		spiked_disc_draw_shadow();
		return;
	}
	spiked_disc_spin();
}

void spiked_disc_roll(void)
{
	uint32_t r1 = 0, r2 = 0, cursor = 0, deleted = 0, depth = 0, direction = 0, ignored = 0, slope_height = 0, speed = 0, speed_x = 0, speed_y = 0;

	move_object_projected(&ignored, &r2, &slope_height, &cursor);
	fall_to_floor(&ignored, &r1, &r2);
	delete_if_off_screen(&deleted, &r2, &slope_height);
	if ((uint8_t)deleted == 1u)
		return;
	direction = 0x154u;
	speed = 0x300u;
	direction_to_speed(speed, &direction, &speed_x, &speed_y);
	OBJECT(current_object)->speed_x = speed_x;
	OBJECT(current_object)->speed_y = speed_y;
	depth = (uint16_t)(OBJECT(current_object)->screen_y + 0x18u);
	queue_sprite_at_depth(&ignored, &depth);
	spiked_disc_draw_shadow();
}

void spiked_disc_draw_shadow(void)
{
	uint32_t bucket = 0;

	current_object = LINKED(current_object, OBJECT(current_object)->child);
	bucket = place_sprite_at_cursor_floor();
	OBJECT(current_object)->screen_y += 0x28u;
	queue_sprite_in_bucket(bucket);
	current_object = object_cursor;
	spiked_disc_spin();
}

void spiked_disc_spin(void)
{
	uint16_t odd_frame = 0;

	if ((flame_machine_flags & 0x100000u) != 0u)
	{
		explode_object();
		return;
	}
	OBJECT(current_object)->spiked_disc.spin_toggle++;
	odd_frame = OBJECT(current_object)->spiked_disc.spin_toggle & 1u;
	OBJECT(current_object)->spiked_disc.spin_toggle = odd_frame;
	if (odd_frame == 0u)
	{
		OBJECT(current_object)->spiked_disc.spin_toggle = 1u;
		OBJECT(current_object)->spiked_disc.spin_frame++;
		OBJECT(current_object)->spiked_disc.spin_frame &= 3u;
		OBJECT(current_object)->sprite_frame = spiked_disc_sprites[OBJECT(current_object)->spiked_disc.spin_frame];
		OBJECT(current_object)->health = 0x7F;
		add_to_touchable_list(0u);
		add_to_rider_list();
	}
}

void flame_machine_scroll_stop_wait(void)
{
	if ((int16_t)scroll_position < (int16_t)OBJECT(current_object)->spawn_arg_1)
		return;
	scroll_mode = 1u;
	OBJECT(current_object)->handler = flame_machine_scroll_stop_hold_ADDRESS;
	flame_machine_scroll_stop_hold();
}

void flame_machine_scroll_stop_hold(void)
{
	if ((flame_machine_flags & 0x100000u) != 0u)
		scroll_mode = 7u;
}

void object_type_9_idle(void)
{
}

void flame_machine_wreck_draw(void)
{
	uint8_t bucket = 0;
	uint16_t offset_x = 0, offset_y = 0;
	uint32_t block_offset = 0, cursor = 0, ignored = 0, slope_height = 0;

	if (map_screen_active != 0u)
	{
		delete_object_and_children(&ignored, &block_offset, &slope_height);
		return;
	}
	unproject_object(&ignored, &ignored, &ignored, &cursor);
	bucket = 0xFEu;
	queue_sprite_in_bucket(bucket);
	current_object = LINKED(current_object, OBJECT(current_object)->child);
	offset_x = 0x6Cu;
	offset_y = 0xFF9Au;
	place_sprite_near_cursor(offset_x, offset_y);
	bucket = 0xFDu;
	queue_sprite_in_bucket(bucket);
	current_object = LINKED(current_object, OBJECT(current_object)->child);
	offset_x = 0xFFDFu;
	offset_y = 0xFFD9u;
	place_sprite_near_cursor(offset_x, offset_y);
	bucket = 0xFCu;
	queue_sprite_in_bucket(bucket);
}

void fence_post_init(void)
{
	uint16_t arc_phase = 0;
	uint32_t block = 0, free_count = 0, object = 0, object_2 = 0, piece_count = 0, rom_record = 0, row_direction = 0;
	uint32_t posts_left = 0;

	electric_fence_state = 1u;
	r14 = OBJECT(current_object)->fence_post.extra_posts;
	row_direction = OBJECT(current_object)->fence_post.row_direction;
	arc_phase = OBJECT(current_object)->fence_post.arc_phase;
	for (;;)
	{
		OBJECT(current_object)->sprite_frame = 0x6CA8u;
		OBJECT(current_object)->sprite_palette = 0x9060u;
		OBJECT(current_object)->flags |= 2u;
		OBJECT(current_object)->hitbox_height = 0x64u;
		OBJECT(current_object)->kind = 0xEu;
		rom_record = fence_post_boxes_ADDRESS + row_direction * 4u;
		OBJECT(current_object)->hitbox_size_x = ROM16(rom_record);
		OBJECT(current_object)->hitbox_size_y = ROM16(rom_record + 2u);
		rom_record = fence_post_box_offsets_ADDRESS + row_direction * 2u;
		OBJECT(current_object)->hitbox_offset_x = ROM8(rom_record);
		OBJECT(current_object)->hitbox_offset_y = ROM8(rom_record + 1u);
		OBJECT(current_object)->handler = fence_post_start_ADDRESS;
		object = LINKED(WORK_RAM, OBJECT(current_object)->child);
		OBJECT(object)->direction = arc_phase;
		OBJECT(object)->sprite_frame = 0x6C88u;
		OBJECT(object)->sprite_palette = 0x9030u;
		OBJECT(object)->sprite_flags = fence_arc_flags[row_direction];
		posts_left = r14;
		r14 = posts_left - 1u;
		if (posts_left == 0u)
			break;
		current_player = current_object;
		spawn_count = 2u;
		spawn_object_with_pieces(0x013956u, &object, &piece_count, &free_count, &object_2, &block);
		if ((uint8_t)object == 0u)
			break;
		OBJECT(object_2)->screen_x = OBJECT(current_player)->screen_x;
		OBJECT(object_2)->screen_y = OBJECT(current_player)->screen_y;
		OBJECT(object_2)->projected_x = OBJECT(current_player)->projected_x;
		OBJECT(object_2)->projected_y = OBJECT(current_player)->projected_y;
		OBJECT(object_2)->z = OBJECT(current_player)->z;
		OBJECT(object_2)->floor_z = OBJECT(current_player)->floor_z;
		OBJECT(object_2)->spawn_arg_2 = OBJECT(current_player)->spawn_arg_2;
		rom_record = fence_post_offsets_ADDRESS + row_direction * 4u;
		OBJECT(object_2)->projected_x += ROM16(rom_record);
		OBJECT(object_2)->projected_y_int += ROM16(rom_record + 2u);
		current_object = object_2;
	}
}

void fence_post_start(void)
{
	uint32_t cursor = 0, ignored = 0;

	add_to_rider_list();
	unproject_object(&ignored, &ignored, &ignored, &cursor);
	OBJECT(current_object)->handler = fence_post_update_ADDRESS;
}

void fence_post_update(void)
{
	uint16_t offset_x = 0, offset_y = 0;
	uint32_t arc_offset_record = 0, block_offset = 0, depth = 0, ignored = 0, slope_height = 0;
	uint16_t odd_frame = 0;

	if (electric_fence_state != 1u)
	{
		if (electric_fence_state == 0xFFu)
		{
			delete_object_and_children(&ignored, &block_offset, &slope_height);
			return;
		}
		OBJECT(current_object)->kind = 5u;
		OBJECT(current_object)->hitbox_size_x = 0x1Bu;
		OBJECT(current_object)->hitbox_size_y = 0x19u;
		OBJECT(current_object)->hitbox_offset_x = -9;
		OBJECT(current_object)->hitbox_offset_y = -0x13;
		OBJECT(current_object)->handler = fence_post_draw_ADDRESS;
	}
	current_object = LINKED(current_object, OBJECT(current_object)->child);
	arc_offset_record = fence_arc_offsets_ADDRESS + OBJECT(object_cursor)->fence_post.row_direction * 4u;
	offset_x = ROM16(arc_offset_record);
	offset_y = ROM16(arc_offset_record + 2u);
	place_sprite_near_cursor(offset_x, offset_y);
	AS16(OBJECT(current_object)->handler)++;
	odd_frame = AS16(OBJECT(current_object)->handler) & 1u;
	AS16(OBJECT(current_object)->handler) = odd_frame;
	if (odd_frame == 0u)
	{
		OBJECT(current_object)->direction++;
		OBJECT(current_object)->direction &= 3u;
		OBJECT(current_object)->sprite_frame = fence_arc_frames[OBJECT(current_object)->direction];
	}
	depth = (uint16_t)(OBJECT(current_object)->screen_y + 8u);
	queue_sprite_at_depth(&ignored, &depth);
	current_object = object_cursor;
	fence_post_draw();
}

void fence_post_draw(void)
{
	uint32_t r1 = 0, r2 = 0, cursor = 0, deleted = 0, ignored = 0, slope_height = 0;

	unproject_object(&ignored, &r2, &slope_height, &cursor);
	fall_to_floor(&ignored, &r1, &r2);
	delete_if_off_screen(&deleted, &r2, &slope_height);
	if ((uint8_t)deleted != 1u)
	{
		queue_sprite(r1);
		OBJECT(current_object)->health = 0x7F;
		add_to_touchable_list(deleted);
		add_to_rider_list();
	}
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
		OBJECT(object)->animation = ROM16(type_record + 1u) + blank_fireball_animation_ADDRESS;
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
