/**
 *    Name:        player/caught_states.c
 *    Description: states 0x2C-0x36: held still, stuck on an object, caught, sinking, knocked off
 *    Version:     0.1.0
 *    Created:     12-10-2026
 *    Modified:    15-10-2026
 *    Author:      Sebastian Toyos
 *
 *    SegaSonic The Hedgehog (arcade, Sega System 32): the game's V60 program decompiled into C by
 *    tools/decompile.py, with the names of symbols.txt.
 */
#include "game.h"

void player_state_held_still(void)
{
}

void player_state_stuck_on_object(uint32_t depth, uint32_t cursor)
{
	uint32_t r0 = 0, block_offset = 0, ignored = 0, jump_offset = 0;

	for (;;)
	{
		check_zone_contact();
		if ((OBJECT(current_object)->flags & 0x8000000u) == 0u)
			break;
		check_player_contacts(&depth, &cursor);
		jump_offset = PLAYER(current_player)->state * 3u;
		switch (PLAYER(current_player)->state)
		{
		case 0:
			player_move(depth, cursor);
			return;
		case 1:
			player_move(depth, cursor);
			return;
		case 2:
			player_move(depth, cursor);
			return;
		case 3:
			player_state_inactive();
			return;
		case 4:
			player_move(depth, cursor);
			return;
		case 5:
			player_state_teetering(cursor);
			return;
		case 6:
			player_state_falling_off_edge();
			return;
		case 7:
			player_state_over_edge(jump_offset, cursor);
			return;
		case 8:
			player_state_dropping(cursor);
			return;
		case 9:
			player_state_falling_into_pit();
			return;
		case 10:
			player_state_climbing(depth, cursor);
			return;
		case 11:
			player_state_not_playing();
			return;
		case 12:
			player_move(depth, cursor);
			return;
		case 13:
			player_state_hurt(depth, cursor);
			return;
		case 14:
			player_state_bouncing();
			return;
		case 15:
			player_state_launched();
			return;
		case 16:
			player_state_launched();
			return;
		case 17:
			player_state_launched();
			return;
		case 18:
			player_state_launched();
			return;
		case 19:
			player_move(depth, cursor);
			return;
		case 20:
			player_move(depth, cursor);
			return;
		case 21:
			player_state_held(depth, cursor);
			return;
		case 22:
			player_state_dying();
			return;
		case 23:
			player_move(depth, cursor);
			return;
		case 24:
			player_state_stage_scene(depth, cursor);
			return;
		case 25:
			player_state_scene_thrown(depth, cursor);
			return;
		case 26:
			player_state_inactive_2();
			return;
		case 27:
			player_state_short_animation();
			return;
		case 28:
			player_state_opening_scene();
			return;
		case 29:
			player_state_stuck(cursor);
			return;
		case 30:
			player_state_intro_turn(depth, cursor);
			return;
		case 31:
			player_state_blown_away(cursor);
			return;
		case 32:
			player_state_tumbling(depth, cursor);
			return;
		case 33:
			player_state_stage_exit(depth, cursor);
			return;
		case 34:
			player_state_in_zone(depth, cursor);
			return;
		case 35:
			player_state_continue();
			return;
		case 36:
			player_state_out_of_game();
			return;
		case 37:
			player_state_floor_8_sequence(cursor);
			return;
		case 38:
			player_state_pulled(cursor);
			return;
		case 39:
			player_state_climbing_object(cursor);
			return;
		case 40:
			player_state_swept_away();
			return;
		case 41:
			player_state_tripped(cursor);
			return;
		case 42:
			player_state_entering_stage(depth, cursor);
			return;
		case 43:
			player_state_final_stage();
			return;
		case 44:
			continue;
		case 45:
			player_state_held_still();
			return;
		case 46:
			player_state_game_over(jump_offset);
			return;
		case 47:
			player_state_stunned(depth, cursor);
			return;
		case 48:
			player_state_caught(cursor);
			return;
		case 49:
			player_state_carried_off(depth, cursor);
			return;
		case 50:
			player_state_trapped(depth, cursor);
			return;
		case 51:
			player_state_sinking(cursor);
			return;
		case 52:
			player_state_choosing_character(cursor);
			return;
		case 53:
			player_state_riding();
			return;
		case 54:
			player_state_knocked_off(depth, cursor);
			return;
		case 55:
			player_state_not_playing();
			return;
		case 56:
			player_state_stage_b_sequence(depth, cursor);
			return;
		case 57:
			player_state_you_failed();
			return;
		case 58:
			player_move(depth, cursor);
			return;
		default:
			unknown_code(0x2D3Eu + PLAYER(current_player)->state * 3u);
		}
	}
	switch (PLAYER(current_player)->substate)
	{
	case 0:
		player_stuck_on_object_begin(cursor);
		break;
	case 1:
		player_stuck_on_object_wait();
		break;
	case 2:
		player_stuck_on_object_shake();
		break;
	case 3:
		player_stuck_on_object_jump_out();
		break;
	case 4:
		player_stuck_on_object_begin(cursor);
		break;
	default:
		unknown_code(0x9D34u + PLAYER(current_player)->substate * 3u);
	}
	update_screen_position(&r0, &depth);
	find_floor(&r0, &depth, &block_offset, &ignored, &cursor);
	depth = keep_player_on_screen(depth);
	add_player_to_lists();
	depth = queue_sprite(depth);
}

void player_stuck_on_object_begin(uint32_t sign)
{
	uint8_t sound_id = 0;
	uint16_t kind = 0, sprite_frame = 0, x_int = 0, y_int = 0, z = 0;
	uint32_t ignored = 0, object = 0, write_index = 0, write_index_address = 0;

	PLAYER(current_player)->substate++;
	object = LINKED(current_object, OBJECT(current_object)->touched_by);
	OBJECT(current_object)->x = OBJECT(object)->x;
	OBJECT(current_object)->y = OBJECT(object)->y;
	OBJECT(current_object)->y_int -= 0x20u;
	PLAYER(current_player)->previous_x = OBJECT(current_object)->x;
	PLAYER(current_player)->previous_y = OBJECT(current_object)->y;
	sprite_frame = 0x1A40u;
	if (OBJECT(current_object)->kind != 0u)
	{
		sprite_frame = 0x5180u;
		if (OBJECT(current_object)->kind != 1u)
			sprite_frame = 0x35C8u;
	}
	OBJECT(current_object)->sprite_frame = sprite_frame;
	OBJECT(current_object)->sprite_flags &= 0xFFBFu;
	OBJECT(current_object)->sprite_palette = mem_read16(OBJECT(current_object)->player_view.palette_list + 2u);
	OBJECT(current_object)->flags &= ~0x40u;
	OBJECT(current_object)->flags &= ~0x2000u;
	PLAYER(current_player)->control_lock = 0u;
	OBJECT(current_object)->flags |= 0x1000u;
	set_floor_sprite_priority();
	PLAYER(current_player)->escape_progress = 0;
	PLAYER(current_player)->state_timer = 0x1E;
	x_int = (uint16_t)OBJECT(current_object)->x_int;
	y_int = (uint16_t)OBJECT(current_object)->y_int;
	kind = 0xDu;
	z = (uint16_t)(OBJECT(current_object)->z_int - 0x28u);
	spawn_sign_2(x_int, y_int, kind, z, sign);
	sound_id = 0x23u;
	play_sound(sound_id, &write_index, &write_index_address);
	project_object(&ignored, &ignored);
}

void player_stuck_on_object_wait(void)
{
	uint8_t sound_id = 0;
	uint32_t write_index = 0, write_index_address = 0;
	uint16_t state_timer = 0;

	state_timer = PLAYER(current_player)->state_timer;
	PLAYER(current_player)->state_timer = state_timer - 1u;
	if ((int16_t)state_timer > 1)
		return;
	PLAYER(current_player)->substate++;
	OBJECT(current_object)->animation_frame = 0u;
	OBJECT(current_object)->animation_timer = 0;
	OBJECT(current_object)->animation_speed = 4u;
	sound_id = 0x35u;
	if (OBJECT(current_object)->kind != 0u)
	{
		sound_id = 0x31u;
		if (OBJECT(current_object)->kind != 1u)
			sound_id = 0x3Du;
	}
	play_sound(sound_id, &write_index, &write_index_address);
}

void player_stuck_on_object_shake(void)
{
	uint8_t sound_id = 0;
	uint32_t r7 = 0, r8 = 0, animation = 0, ignored = 0, speed = 0, struggle_index = 0, trackball_angle = 0, trackball_y = 0, write_index_address = 0;

	read_player_trackball();
	speed = PLAYER(current_player)->trackball_x;
	trackball_y = PLAYER(current_player)->trackball_y;
	speed = speed * speed + trackball_y * trackball_y;
	square_root(&speed, &ignored);
	speed &= 0xFFFFu;
	speed = speed * speed;
	if ((int32_t)speed > 0x5000000)
		speed = 0x5000000u;
	OBJECT(current_object)->speed = speed;
	trackball_angle = (uint16_t)PLAYER(current_player)->trackball_x;
	trackball_y = (uint16_t)PLAYER(current_player)->trackball_y;
	angle_of_vector(trackball_y, &trackball_angle, &ignored);
	PLAYER(current_player)->target_heading = trackball_angle;
	if (OBJECT(current_object)->speed_int < 0x100)
		PLAYER(current_player)->escape_progress = 0;
	else if (PLAYER(current_player)->target_heading >= 0x180 && PLAYER(current_player)->target_heading <= 0x380)
		PLAYER(current_player)->escape_progress += (uint16_t)OBJECT(current_object)->speed_int;
	if ((uint32_t)PLAYER(current_player)->escape_progress != 0u)
	{
		animation = sonic_stuck_struggle_animations_ADDRESS;
		if (OBJECT(current_object)->kind != 0u)
		{
			animation = ray_stuck_struggle_animations_ADDRESS;
			if (OBJECT(current_object)->kind != 1u)
				animation = mighty_stuck_struggle_animations_ADDRESS;
		}
		struggle_index = (int32_t)PLAYER(current_player)->escape_progress / 0x4000;
		if ((int16_t)struggle_index > 2)
			struggle_index = 2u;
		OBJECT(current_object)->animation = mem_read32(animation + struggle_index * 4u);
		animate(&trackball_y, &r7, &r8);
		if (PLAYER(current_player)->escape_progress < 0xC000)
			return;
		PLAYER(current_player)->substate++;
		animation = sonic_stuck_escape_animation_ADDRESS;
		if (OBJECT(current_object)->kind != 0u)
		{
			animation = ray_stuck_escape_animation_ADDRESS;
			if (OBJECT(current_object)->kind != 1u)
				animation = mighty_stuck_escape_animation_ADDRESS;
		}
		OBJECT(current_object)->animation = animation;
		OBJECT(current_object)->animation_frame = 0xFFu;
		OBJECT(current_object)->animation_timer = 0;
		OBJECT(current_object)->animation_speed = 3u;
		sound_id = 0x2Bu;
		play_sound(sound_id, &trackball_y, &write_index_address);
		return;
	}
	animation = sonic_stuck_animation_ADDRESS;
	if (OBJECT(current_object)->kind != 0u)
	{
		animation = ray_stuck_animation_ADDRESS;
		if (OBJECT(current_object)->kind != 1u)
			animation = mighty_stuck_animation_ADDRESS;
	}
	OBJECT(current_object)->animation = animation;
	animate(&trackball_y, &r7, &r8);
}

void player_stuck_on_object_jump_out(void)
{
	uint32_t r7 = 0, at_last_step = 0, bounce_record = 0, bounce_records = 0, frame = 0;

	animate(&frame, &r7, &at_last_step);
	if ((uint8_t)at_last_step == 0u)
		return;
	PLAYER(current_player)->bounce_direction = 1u;
	bounce_records = sonic_bounce_records_ADDRESS;
	if (OBJECT(current_object)->kind != 0u)
	{
		bounce_records = ray_bounce_records_ADDRESS;
		if (OBJECT(current_object)->kind != 1u)
			bounce_records = mighty_bounce_records_ADDRESS;
	}
	bounce_record = bounce_records + PLAYER(current_player)->bounce_direction * 3u * 2u;
	OBJECT(current_object)->direction = ROM16(bounce_record);
	OBJECT(current_object)->sprite_flags &= ~0x40u;
	bounce_record += 2u;
	OBJECT(current_object)->sprite_flags |= ROM16(bounce_record);
	OBJECT(current_object)->animation = ROM16(bounce_record + 2u) + bounce_records;
	face_movement_direction(0u);
	OBJECT(current_object)->speed = 0x2000000;
	OBJECT(current_object)->speed_z = -0x1A000;
	PLAYER(current_player)->bounce_speed_z = -0x1A000;
	PLAYER(current_player)->bounce_step = 0xD000;
	PLAYER(current_player)->bounce_speed_z += PLAYER(current_player)->bounce_step;
	set_speed_from_direction();
	OBJECT(current_object)->sprite_frame = mem_read16(OBJECT(current_object)->animation + 4u);
	OBJECT(current_object)->animation_frame = 0u;
	OBJECT(current_object)->animation_timer = 3;
	OBJECT(current_object)->animation_speed = 3u;
	PLAYER(current_player)->state_timer = 0x14;
	OBJECT(current_object)->flags &= ~0x1000u;
	PLAYER(current_player)->state = 0xEu;
	PLAYER(current_player)->substate = 1u;
}

void player_state_caught(uint32_t cursor)
{
	uint32_t r1 = 0, r2 = 0, at_last_step = 0, depth = 0, hurt_animations = 0, ignored = 0, near_edge_fall_animation = 0, object = 0, sound_number = 0, step_record = 0;

	switch (PLAYER(current_player)->substate)
	{
	case 0:
	case 3:
		PLAYER(current_player)->substate++;
		near_edge_fall_animation = sonic_near_edge_fall_animation_ADDRESS;
		if (OBJECT(current_object)->kind != 0u)
		{
			near_edge_fall_animation = ray_near_edge_fall_animation_ADDRESS;
			if (OBJECT(current_object)->kind != 1u)
				near_edge_fall_animation = mighty_near_edge_fall_animation_ADDRESS;
		}
		OBJECT(current_object)->animation = near_edge_fall_animation;
		OBJECT(current_object)->animation_frame = 0xFFu;
		OBJECT(current_object)->animation_speed = 3u;
		OBJECT(current_object)->animation_timer = 0;
		OBJECT(current_object)->sprite_flags &= ~0x4Fu;
		OBJECT(current_object)->sprite_flags |= 4u;
		OBJECT(current_object)->speed_z = 0;
		OBJECT(current_object)->speed = 0;
		OBJECT(current_object)->speed_x = 0;
		OBJECT(current_object)->speed_y = 0;
		OBJECT(current_object)->sprite_palette = mem_read16(OBJECT(current_object)->player_view.palette_list + 2u);
		OBJECT(current_object)->flags &= ~0x40u;
		OBJECT(current_object)->flags &= ~0x2000u;
		OBJECT(current_object)->flags &= ~0x2000u;
		OBJECT(current_object)->flags &= ~8u;
		OBJECT(current_object)->flags &= ~0x10u;
		PLAYER(current_player)->control_lock = 0u;
	case 1:
		if ((int8_t)OBJECT(current_object)->animation_speed >= 0)
		{
			at_last_step = animate_and_move(0u);
			if ((uint8_t)at_last_step != 0u)
			{
				OBJECT(current_object)->animation_speed = 0xFFu;
				sound_number = 0x36u;
				if (OBJECT(current_object)->kind != 0u)
				{
					sound_number = 0x37u;
					if (OBJECT(current_object)->kind != 1u)
						sound_number = 0x77u;
				}
				play_sound((uint8_t)sound_number, &r1, &r2);
			}
		}
		if (current_game_mode == 0x19u)
		{
			object = LINKED(current_object, scene_object);
			OBJECT(current_object)->x += (int32_t)((uint32_t)OBJECT(object)->x - OBJECT(current_object)->x) >> 6;
			OBJECT(current_object)->y += (int32_t)(OBJECT(object)->y + ((uint32_t)OBJECT(current_object)->player << 21) - OBJECT(current_object)->y) >> 6;
			OBJECT(current_object)->z += (int32_t)((uint32_t)OBJECT(object)->z - OBJECT(current_object)->z) >> 3;
		}
		else
			OBJECT(current_object)->z += 0x84000u;
		if (OBJECT(current_object)->z_int >= 0x4C8)
			PLAYER(current_player)->substate++;
		find_floor(&sound_number, &r1, &r2, &ignored, &cursor);
		project_object(&ignored, &depth);
		depth = queue_sprite(depth);
		return;
	case 2:
		cursor = spawn_player_effect(cursor);
		OBJECT(cursor)->sprite_palette &= 0xBFFFu;
		OBJECT(cursor)->var_96 = 0x70u;
		PLAYER(current_player)->state = 0xDu;
		PLAYER(current_player)->substate = 2u;
		OBJECT(current_object)->speed_z = 0;
		OBJECT(current_object)->direction = 0x200u;
		face_movement_direction(0u);
		hurt_animations = sonic_hurt_animations_ADDRESS;
		if (OBJECT(current_object)->kind != 0u)
		{
			hurt_animations = ray_hurt_animations_ADDRESS;
			if (OBJECT(current_object)->kind != 1u)
				hurt_animations = mighty_hurt_animations_ADDRESS;
		}
		OBJECT(current_object)->animation = hurt_animations + ROM16(hurt_animations + PLAYER(current_player)->facing * 2u);
		OBJECT(current_object)->animation_frame = 0u;
		step_record = OBJECT(current_object)->animation + 2u;
		OBJECT(current_object)->sprite_flags &= ~0xC0u;
		OBJECT(current_object)->sprite_flags |= mem_read16(step_record);
		OBJECT(current_object)->sprite_frame = mem_read16(step_record + 2u);
		OBJECT(current_object)->player_view.hurt_flash_period = 0xFFu;
		OBJECT(current_object)->sprite_palette &= 0x1FFFu;
		OBJECT(current_object)->sprite_palette |= 0x4000u;
		sound_number = 0x87u;
		play_sound((uint8_t)sound_number, &r1, &r2);
		find_floor(&sound_number, &r1, &r2, &ignored, &cursor);
		project_object(&ignored, &depth);
		depth = queue_sprite(depth);
		return;
	default:
		unknown_code(0xA01Au + PLAYER(current_player)->substate * 3u);
	}
}

void player_state_sinking(uint32_t cursor)
{
	uint8_t sound_id = 0;
	uint32_t r1 = 0, r7 = 0, clip_cursor = 0, depth = 0, edge_direction = 0, floor_z = 0, ignored = 0, pit_sinking_animation = 0, point_x = 0, point_y = 0, records_left = 0, sink_z = 0, write_index_address = 0;
	uint8_t health = 0;
	uint16_t state_timer = 0;

	if (PLAYER(current_player)->substate == 0u)
	{
		PLAYER(current_player)->substate = 0xFFu;
		OBJECT(current_object)->flags &= ~0x40u;
		OBJECT(current_object)->sprite_palette = mem_read16(OBJECT(current_object)->player_view.palette_list + 2u);
		OBJECT(current_object)->flags &= ~0x2000u;
		PLAYER(current_player)->state_timer = 0x1E;
		OBJECT(current_object)->sprite_flags &= ~0xFu;
		OBJECT(current_object)->sprite_flags |= 4u;
		OBJECT(current_object)->sprite_palette &= 0x9FFFu;
		OBJECT(current_object)->sprite_palette |= 0x2000u;
		if (PLAYER(current_player)->previous_state != 9u)
		{
			edge_direction = (uint16_t)PLAYER(current_player)->ledge_side;
			if (edge_direction == 4u)
				edge_direction = (uint8_t)PLAYER(current_player)->ledge_direction;
			switch (edge_direction)
			{
			case 0:
			case 4:
				OBJECT(current_object)->y_int -= 0xCu;
				break;
			case 1:
				OBJECT(current_object)->x_int += 0xCu;
				break;
			case 2:
				OBJECT(current_object)->y_int += 0xCu;
				break;
			case 3:
				OBJECT(current_object)->x_int -= 0xCu;
				break;
			default:
				unknown_code(0xA258u + edge_direction * 2u);
			}
		}
		sink_z = 0x1D80000u;
		if (mem_read8(floor_blocks + 2u + OBJECT(current_object)->floor_block + 0x12u) != 0x22u)
			sink_z = 0x200000u;
		OBJECT(current_object)->floor_z = sink_z;
		OBJECT(current_object)->z = sink_z;
		pit_sinking_animation = sonic_pit_sinking_animation_ADDRESS;
		if (OBJECT(current_object)->kind != 0u)
		{
			pit_sinking_animation = ray_pit_sinking_animation_ADDRESS;
			if (OBJECT(current_object)->kind != 1u)
				pit_sinking_animation = mighty_pit_sinking_animation_ADDRESS;
		}
		OBJECT(current_object)->animation = pit_sinking_animation;
		OBJECT(current_object)->animation_frame = 0xFFu;
		OBJECT(current_object)->animation_timer = 0;
		OBJECT(current_object)->animation_speed = 4u;
		cursor = spawn_player_effect(cursor);
		OBJECT(cursor)->sprite_palette &= 0x9FFFu;
		OBJECT(cursor)->sprite_palette |= 0x2000u;
		OBJECT(cursor)->sinking_effect.depth_offset = 0x70;
		OBJECT(cursor)->x_int -= 0x10u;
		OBJECT(cursor)->handler = sinking_effect_update_ADDRESS;
		records_left = 2u;
		clip_cursor = current_player + 0xF0u;
		do
		{
			clip_cursor += 2u;
			mem_write16(clip_cursor, 0x5000u);
			clip_cursor += 2u;
			mem_write16(clip_cursor, 0xE0u);
			clip_cursor += 2u;
			mem_write16(clip_cursor, 0u);
			clip_cursor += 2u;
			mem_write16(clip_cursor, 0x140u);
			clip_cursor += 2u;
			mem_write16(clip_cursor, 0u);
			clip_cursor += 2u;
			mem_write16(clip_cursor, 0u);
			clip_cursor += 2u;
			mem_write16(clip_cursor, 0u);
			clip_cursor += 2u;
			mem_write16(clip_cursor, 0u);
			records_left--;
			clip_cursor += 2u;
		} while (records_left != 0u);
		sound_id = 0x87u;
		play_sound(sound_id, &r1, &write_index_address);
	}
	animate(&r1, &r7, &records_left);
	state_timer = PLAYER(current_player)->state_timer;
	PLAYER(current_player)->state_timer = state_timer - 1u;
	if ((int16_t)state_timer <= 1)
	{
		PLAYER(current_player)->state_timer = 0;
		OBJECT(current_object)->z += 0xA000u;
	}
	OBJECT(current_object)->y += 0x18000u;
	project_object(&ignored, &ignored);
	if (OBJECT(current_object)->z_int < (int16_t)(OBJECT(current_object)->floor_z_int + 0x20u))
	{
		floor_z = OBJECT(current_object)->floor_z;
		point_x = OBJECT(current_object)->x;
		point_y = OBJECT(current_object)->y;
		project_point(floor_z, &ignored, &point_x, &point_y);
		PLAYER(current_player)->ground_clip_bottom = point_y;
		PLAYER(current_player)->draw_depth = ((int32_t)((uint32_t)OBJECT(current_object)->projected_y + OBJECT(current_object)->floor_z) >> 16) - view_y;
		current_object = current_player + 0x102u;
		depth = PLAYER(current_player)->draw_depth;
		queue_sprite_at_depth(&ignored, &depth);
		current_object = object_cursor;
		depth = PLAYER(current_player)->draw_depth;
		queue_sprite_at_depth(&ignored, &depth);
		current_object = current_player + 0xF0u;
		depth = PLAYER(current_player)->draw_depth;
		queue_sprite_at_depth(&ignored, &depth);
		return;
	}
	health = OBJECT(current_object)->health;
	OBJECT(current_object)->health = health - 8u;
	if ((int8_t)health > 8)
		OBJECT(current_object)->handler = player_respawn_ADDRESS;
	else
	{
		OBJECT(current_object)->health = 0;
		PLAYER(current_player)->state = 0x16u;
		PLAYER(current_player)->substate = 0u;
	}
}

void sinking_effect_update(void)
{
	uint32_t r7 = 0, at_last_step = 0, depth = 0, frame = 0, ignored = 0;

	OBJECT(current_object)->y += 0x18000u;
	animate(&frame, &r7, &at_last_step);
	if ((uint8_t)at_last_step == 0u)
	{
		project_object(&ignored, &ignored);
		depth = (uint16_t)(OBJECT(current_object)->screen_y + OBJECT(current_object)->sinking_effect.depth_offset);
		queue_sprite_at_depth(&ignored, &depth);
		return;
	}
	delete_current_object();
}

void player_state_knocked_off(uint32_t depth, uint32_t cursor)
{
	uint8_t sound_id = 0;
	uint32_t gravity = 0, hit_direction = 0, hurt_animations = 0, ignored = 0, jump_offset = 0, knock_speed_x = 0, knock_speed_y = 0, object = 0, step_record = 0;
	uint8_t health = 0;
	uint32_t push_speed_5 = 0, push_speed_6 = 0;

	for (;;)
	{
		switch (PLAYER(current_player)->substate)
		{
		case 0:
		case 2:
			PLAYER(current_player)->substate++;
			if ((uint8_t)OBJECT(current_object)->health != 0u)
				goto player_knocked_off_start;
			PLAYER(current_player)->state = 0x16u;
			PLAYER(current_player)->substate = 0u;
			jump_offset = PLAYER(current_player)->state * 3u;
			switch (PLAYER(current_player)->state)
			{
			case 0:
				player_move(depth, cursor);
				return;
			case 1:
				player_move(depth, cursor);
				return;
			case 2:
				player_move(depth, cursor);
				return;
			case 3:
				player_state_inactive();
				return;
			case 4:
				player_move(depth, cursor);
				return;
			case 5:
				player_state_teetering(cursor);
				return;
			case 6:
				player_state_falling_off_edge();
				return;
			case 7:
				player_state_over_edge(jump_offset, cursor);
				return;
			case 8:
				player_state_dropping(cursor);
				return;
			case 9:
				player_state_falling_into_pit();
				return;
			case 10:
				player_state_climbing(depth, cursor);
				return;
			case 11:
				player_state_not_playing();
				return;
			case 12:
				player_move(depth, cursor);
				return;
			case 13:
				player_state_hurt(depth, cursor);
				return;
			case 14:
				player_state_bouncing();
				return;
			case 15:
				player_state_launched();
				return;
			case 16:
				player_state_launched();
				return;
			case 17:
				player_state_launched();
				return;
			case 18:
				player_state_launched();
				return;
			case 19:
				player_move(depth, cursor);
				return;
			case 20:
				player_move(depth, cursor);
				return;
			case 21:
				player_state_held(depth, cursor);
				return;
			case 22:
				player_state_dying();
				return;
			case 23:
				player_move(depth, cursor);
				return;
			case 24:
				player_state_stage_scene(depth, cursor);
				return;
			case 25:
				player_state_scene_thrown(depth, cursor);
				return;
			case 26:
				player_state_inactive_2();
				return;
			case 27:
				player_state_short_animation();
				return;
			case 28:
				player_state_opening_scene();
				return;
			case 29:
				player_state_stuck(cursor);
				return;
			case 30:
				player_state_intro_turn(depth, cursor);
				return;
			case 31:
				player_state_blown_away(cursor);
				return;
			case 32:
				player_state_tumbling(depth, cursor);
				return;
			case 33:
				player_state_stage_exit(depth, cursor);
				return;
			case 34:
				player_state_in_zone(depth, cursor);
				return;
			case 35:
				player_state_continue();
				return;
			case 36:
				player_state_out_of_game();
				return;
			case 37:
				player_state_floor_8_sequence(cursor);
				return;
			case 38:
				player_state_pulled(cursor);
				return;
			case 39:
				player_state_climbing_object(cursor);
				return;
			case 40:
				player_state_swept_away();
				return;
			case 41:
				player_state_tripped(cursor);
				return;
			case 42:
				player_state_entering_stage(depth, cursor);
				return;
			case 43:
				player_state_final_stage();
				return;
			case 44:
				player_state_stuck_on_object(depth, cursor);
				return;
			case 45:
				player_state_held_still();
				return;
			case 46:
				player_state_game_over(jump_offset);
				return;
			case 47:
				player_state_stunned(depth, cursor);
				return;
			case 48:
				player_state_caught(cursor);
				return;
			case 49:
				player_state_carried_off(depth, cursor);
				return;
			case 50:
				player_state_trapped(depth, cursor);
				return;
			case 51:
				player_state_sinking(cursor);
				return;
			case 52:
				player_state_choosing_character(cursor);
				return;
			case 53:
				player_state_riding();
				return;
			case 54:
				continue;
			case 55:
				player_state_not_playing();
				return;
			case 56:
				player_state_stage_b_sequence(depth, cursor);
				return;
			case 57:
				player_state_you_failed();
				return;
			case 58:
				player_move(depth, cursor);
				return;
			default:
				unknown_code(0x2D3Eu + PLAYER(current_player)->state * 3u);
			}
			break;
		case 1:
			goto player_knocked_off_fly;
		default:
			unknown_code(0xA433u + PLAYER(current_player)->substate * 3u);
		}
	}
player_knocked_off_start:
	PLAYER(current_player)->invulnerable_timer = 0x258;
	OBJECT(current_object)->flags &= ~0x2000u;
	PLAYER(current_player)->control_lock = 0u;
	PLAYER(current_player)->mishap_flag = 0xFFu;
	PLAYER(current_player)->hurt_contact_delay = 0xA;
	OBJECT(current_object)->speed_z = -0x80000;
	if (OBJECT(current_object)->floor_type == 4u)
		OBJECT(current_object)->speed_z = (int32_t)OBJECT(current_object)->speed_z >> 1;
	object = LINKED(WORK_RAM, OBJECT(current_object)->touched_by);
	knock_speed_x = 0x40000u;
	if (OBJECT(current_object)->floor_type == 4u)
		knock_speed_x = (int32_t)knock_speed_x >> 1;
	knock_speed_y = 0u;
	hit_direction = OBJECT(object)->direction;
	OBJECT(current_object)->direction = hit_direction;
	OBJECT(current_object)->speed = knock_speed_x << 8;
	if (hit_direction != 0u)
	{
		if (hit_direction != 0x200u)
		{
			if (hit_direction != 0x100u)
				knock_speed_x = 0u - knock_speed_x;
		}
		else
		{
			push_speed_5 = knock_speed_x;
			knock_speed_x = knock_speed_y;
			knock_speed_y = push_speed_5;
		}
	}
	else
	{
		push_speed_6 = knock_speed_x;
		knock_speed_x = knock_speed_y;
		knock_speed_y = 0u - push_speed_6;
	}
	OBJECT(current_object)->speed_x = knock_speed_x;
	OBJECT(current_object)->speed_y = knock_speed_y;
	face_movement_direction(hit_direction);
	sound_id = 0x1Du;
	play_sound(sound_id, &depth, &knock_speed_x);
	sound_id = 0x55u;
	if (OBJECT(current_object)->kind != 0u)
	{
		sound_id = 0x4Au;
		if (OBJECT(current_object)->kind != 1u)
			sound_id = 0x5Bu;
	}
	play_sound(sound_id, &depth, &knock_speed_x);
	hurt_animations = sonic_hurt_animations_ADDRESS;
	if (OBJECT(current_object)->kind != 0u)
	{
		hurt_animations = ray_hurt_animations_ADDRESS;
		if (OBJECT(current_object)->kind != 1u)
			hurt_animations = mighty_hurt_animations_ADDRESS;
	}
	OBJECT(current_object)->animation = hurt_animations + ROM16(hurt_animations + PLAYER(current_player)->facing * 2u);
	OBJECT(current_object)->animation_frame = 0u;
	step_record = OBJECT(current_object)->animation;
	OBJECT(current_object)->sprite_flags &= ~0xC0u;
	step_record += 2u;
	OBJECT(current_object)->sprite_flags |= mem_read16(step_record);
	OBJECT(current_object)->sprite_frame = mem_read16(step_record + 2u);
	OBJECT(current_object)->player_view.hurt_flash_period = 3u;
	OBJECT(current_object)->player_view.hurt_flash_timer = 3u;
	OBJECT(current_object)->player_view.hurt_flash_on = 0u;
player_knocked_off_fly:
	gravity = 0x6000u;
	if (OBJECT(current_object)->floor_type == 4u)
		gravity = gravity >> 2;
	OBJECT(current_object)->speed_z += gravity;
	PLAYER(current_player)->previous_z = OBJECT(current_object)->z;
	OBJECT(current_object)->z += OBJECT(current_object)->speed_z;
	PLAYER(current_player)->previous_x = OBJECT(current_object)->x;
	PLAYER(current_player)->previous_y = OBJECT(current_object)->y;
	OBJECT(current_object)->x += OBJECT(current_object)->speed_x;
	OBJECT(current_object)->y += OBJECT(current_object)->speed_y;
	find_floor(&gravity, &depth, &knock_speed_x, &ignored, &cursor);
	if (OBJECT(current_object)->animation_frame == 0u && OBJECT(current_object)->speed_z_int >= 0)
	{
		OBJECT(current_object)->animation_frame++;
		step_record = OBJECT(current_object)->animation + 2u + OBJECT(current_object)->animation_frame * 4u;
		OBJECT(current_object)->sprite_flags &= ~0xC0u;
		OBJECT(current_object)->sprite_flags |= mem_read16(step_record);
		OBJECT(current_object)->sprite_frame = mem_read16(step_record + 2u);
	}
	project_object(&ignored, &ignored);
	if (OBJECT(current_object)->screen_y < 0x120)
	{
		ignored = queue_sprite_at_floor();
		return;
	}
	health = OBJECT(current_object)->health;
	OBJECT(current_object)->health = health - 8u;
	if ((int8_t)health > 8)
		OBJECT(current_object)->handler = player_respawn_ADDRESS;
	else
	{
		OBJECT(current_object)->health = 0;
		PLAYER(current_player)->state = 0x16u;
		PLAYER(current_player)->substate = 0u;
	}
	ignored = queue_sprite_at_floor();
}
