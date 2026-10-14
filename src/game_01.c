/**
 *    Name:        game_01.c
 *    Description: the game's functions, part 01 (in the order of their V60 addresses)
 *    Version:     0.1.0
 *    Created:     12-10-2026
 *    Modified:    14-10-2026
 *    Author:      Sebastian Toyos
 *
 *    SegaSonic The Hedgehog (arcade, Sega System 32): the game's V60 program decompiled into C by
 *    tools/decompile.py, with the names of symbols.txt.
 */
#include "game.h"

void player_state_stage_scene(uint32_t depth, uint32_t cursor)
{
	uint8_t sound_id = 0;
	uint16_t y_shift = 0;
	uint32_t r7 = 0, r8 = 0, geyser_hop_animation = 0, ignored = 0, kind = 0, position_record = 0, shrink_away_animation = 0, sprite_zoom = 0, standing_animations = 0, write_index_address = 0, x_shift = 0;
	uint16_t state_timer = 0, sprite_scale = 0;

	switch (scene_step)
	{
	case 0:
		player_move(depth, cursor);
		return;
	case 1:
	case 8:
		if (scene_step == PLAYER(current_player)->scene_step_seen)
		{
			if (PLAYER(current_player)->path_request != 0u || PLAYER(current_player)->path != 0u)
			{
				PLAYER(current_player)->state = PLAYER(current_player)->previous_state;
				player_move(depth, cursor);
				return;
			}
			players_finished[OBJECT(current_object)->player] = 0xFFu;
			return;
		}
		PLAYER(current_player)->scene_step_seen = scene_step;
		OBJECT(current_object)->flags |= 0x4000000u;
		PLAYER(current_player)->control_lock = 0u;
		PLAYER(current_player)->path_request = PLAYER(current_player)->scene_path;
		stage_exit_started = 0xFFu;
		OBJECT(current_object)->z = OBJECT(current_object)->floor_z;
		PLAYER(current_player)->state = PLAYER(current_player)->previous_state;
		player_move(depth, cursor);
		return;
	case 2:
		if (scene_step != PLAYER(current_player)->scene_step_seen)
		{
			PLAYER(current_player)->scene_step_seen = scene_step;
			PLAYER(current_player)->heading = 0x280u;
			OBJECT(current_object)->speed = 0;
			PLAYER(current_player)->path_request = 0xEu;
			PLAYER(current_player)->control_lock = 0u;
			OBJECT(current_object)->x = 0x1C00000;
			OBJECT(current_object)->y = 0x154D0000;
		}
		if (stage_05_switch_step < 0)
		{
			player_move(depth, cursor);
			return;
		}
		return;
	case 3:
		if (scene_step == PLAYER(current_player)->scene_step_seen)
		{
			project_object(&ignored, &ignored);
			ignored = queue_sprite_at_base(ignored);
			return;
		}
		PLAYER(current_player)->scene_step_seen = scene_step;
		standing_animations = sonic_standing_animations_ADDRESS;
		if (OBJECT(current_object)->kind != 0u)
		{
			standing_animations = ray_standing_animations_ADDRESS;
			if (OBJECT(current_object)->kind != 1u)
				standing_animations = mighty_standing_animations_ADDRESS;
		}
		OBJECT(current_object)->animation = standing_animations + ROM16(standing_animations + PLAYER(current_player)->facing * 2u);
		OBJECT(current_object)->animation_frame = 0xFFu;
		animate_step_with_flags(&ignored, &r8);
		project_object(&ignored, &ignored);
		ignored = queue_sprite_at_base(ignored);
		return;
	case 4:
		if (scene_step != PLAYER(current_player)->scene_step_seen)
		{
			PLAYER(current_player)->scene_step_seen = scene_step;
			PLAYER(current_player)->path_request = 5u;
		}
		player_move(depth, cursor);
		return;
	case 5:
		geyser_hop_animation = sonic_geyser_hop_animation_ADDRESS;
		if (OBJECT(current_object)->kind != 0u)
		{
			geyser_hop_animation = ray_geyser_hop_animation_ADDRESS;
			if (OBJECT(current_object)->kind != 1u)
				geyser_hop_animation = mighty_geyser_hop_animation_ADDRESS;
		}
		OBJECT(current_object)->animation = geyser_hop_animation;
		OBJECT(current_object)->animation_frame = 0u;
		OBJECT(current_object)->animation_speed = 4u;
		OBJECT(current_object)->animation_timer = 0;
		OBJECT(current_object)->sprite_palette = mem_read16(OBJECT(current_object)->player_view.palette_list + 2u);
		PLAYER(current_player)->scene_hop_z = 0;
		OBJECT(current_object)->speed_z = -0x30000;
		OBJECT(current_object)->x = 0x17D0000;
		OBJECT(current_object)->y = 0x154D0000;
		y_shift = 0u;
		if (OBJECT(current_object)->player != 0u)
		{
			y_shift = 0x20u;
			if (OBJECT(current_object)->player != 1u)
				y_shift = (uint16_t)(0u - y_shift);
		}
		OBJECT(current_object)->y_int += y_shift;
		sound_id = 0x36u;
		if (OBJECT(current_object)->kind != 0u)
		{
			sound_id = 0x46u;
			if (OBJECT(current_object)->kind != 1u)
				sound_id = 0x77u;
		}
		play_sound(sound_id, &depth, &write_index_address);
		project_object(&ignored, &depth);
		PLAYER(current_player)->screen_x_int = OBJECT(current_object)->projected_x;
		AS16(PLAYER(current_player)->screen_x) = 0u;
		depth = queue_sprite(depth);
		return;
	case 6:
		if (scene_timer > 0x90)
		{
			x_shift = 0u;
			if (OBJECT(current_object)->kind != 0u)
			{
				x_shift = 0xFFFF0000u;
				if (OBJECT(current_object)->kind != 1u)
					x_shift = 0x10000u;
			}
			PLAYER(current_player)->screen_x -= x_shift;
			OBJECT(current_object)->projected_x = PLAYER(current_player)->screen_x_int;
			OBJECT(current_object)->z_int -= 0xDu;
		}
		else
		{
			OBJECT(current_object)->floor_z = OBJECT((current_object & ~0xFFFFu) | stage_00_geyser)->z;
			OBJECT(current_object)->floor_z_int -= 0x20u;
			OBJECT(current_object)->z = OBJECT(current_object)->floor_z;
			kind = OBJECT(current_object)->kind;
			OBJECT(current_object)->speed_z += (uint16_t)(kind << 12) + 0x6000u;
			PLAYER(current_player)->scene_hop_z += OBJECT(current_object)->speed_z;
			OBJECT(current_object)->z += PLAYER(current_player)->scene_hop_z;
			if ((int16_t)AT16(PLAYER(current_player)->scene_hop_z, 2) >= 0)
			{
				OBJECT(current_object)->speed_z = -0x30000;
				PLAYER(current_player)->scene_hop_z = 0;
			}
		}
		animate(&depth, &r7, &r8);
		update_screen_position(&ignored, &depth);
		depth = queue_sprite(depth);
		return;
	case 7:
		if (scene_step != PLAYER(current_player)->scene_step_seen)
		{
			PLAYER(current_player)->scene_step_seen = scene_step;
			shrink_away_animation = sonic_shrink_away_animation_ADDRESS;
			if (OBJECT(current_object)->kind != 0u)
			{
				shrink_away_animation = ray_shrink_away_animation_ADDRESS;
				if (OBJECT(current_object)->kind != 1u)
					shrink_away_animation = mighty_shrink_away_animation_ADDRESS;
			}
			OBJECT(current_object)->animation = shrink_away_animation;
			OBJECT(current_object)->animation_frame = 0u;
			OBJECT(current_object)->animation_timer = 0;
			OBJECT(current_object)->animation_speed = 4u;
			OBJECT(current_object)->sprite_flags &= ~0xCu;
			position_record = shrink_away_positions_ADDRESS + OBJECT(current_object)->kind * 4u;
			PLAYER(current_player)->screen_x = (int16_t)(ROM16(position_record) - 0xA0u);
			OBJECT(current_object)->projected_y = (int16_t)(ROM16(position_record + 2u) - 0x70u);
			PLAYER(current_player)->scene_zoom = 0;
			PLAYER(current_player)->state_timer = OBJECT(current_object)->kind * 0x1Eu + 0x3Cu;
		}
		if ((uint16_t)PLAYER(current_player)->state_timer == 1u)
		{
			sound_id = 0x16u;
			play_sound(sound_id, &depth, &write_index_address);
			sound_id = 0x36u;
			if (OBJECT(current_object)->kind != 0u)
			{
				sound_id = 0x37u;
				if (OBJECT(current_object)->kind != 1u)
					sound_id = 0x77u;
			}
			play_sound(sound_id, &depth, &write_index_address);
		}
		state_timer = PLAYER(current_player)->state_timer;
		PLAYER(current_player)->state_timer = state_timer - 1u;
		if ((int16_t)state_timer > 1)
			return;
		PLAYER(current_player)->state_timer = 0;
		if ((int16_t)OBJECT(current_object)->sprite_scale > 0x20)
		{
			PLAYER(current_player)->scene_zoom += 8u;
			OBJECT(current_object)->sprite_scale = divide_s32(0x8000u, PLAYER(current_player)->scene_zoom + 0x10u);
			sprite_zoom = OBJECT(current_object)->sprite_scale;
			OBJECT(current_object)->screen_x = ((int32_t)(PLAYER(current_player)->screen_x * sprite_zoom) >> 11) + 0xA0u;
			OBJECT(current_object)->screen_y = ((int32_t)(OBJECT(current_object)->projected_y * sprite_zoom) >> 11) + 0x70u;
			animate(&sprite_zoom, &r7, &r8);
			sprite_zoom = queue_sprite(sprite_zoom);
			return;
		}
		sprite_scale = OBJECT(current_object)->sprite_scale;
		OBJECT(current_object)->sprite_scale = sprite_scale - 1u;
		if ((int16_t)sprite_scale <= 1)
			return;
		sprite_zoom = OBJECT(current_object)->sprite_scale;
		OBJECT(current_object)->screen_x = ((int32_t)(PLAYER(current_player)->screen_x * sprite_zoom) >> 11) + 0xA0u;
		OBJECT(current_object)->screen_y = ((int32_t)(OBJECT(current_object)->projected_y * sprite_zoom) >> 11) + 0x70u;
		animate(&sprite_zoom, &r7, &r8);
		sprite_zoom = queue_sprite(sprite_zoom);
		return;
	default:
		unknown_code(0x6F06u + scene_step * 3u);
	}
}

void player_state_stunned(uint32_t depth, uint32_t cursor)
{
	uint32_t r7 = 0, at_last_step = 0, block_offset = 0, ignored = 0, jump_offset = 0, stun_recover_animation = 0;
	uint8_t health = 0;
	uint16_t state_timer = 0;

	for (;;)
	{
		check_zone_contact();
		check_player_contacts(&depth, &cursor);
		if (PLAYER(current_player)->state != 0xDu)
			break;
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
			continue;
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
	case 3:
		PLAYER(current_player)->substate++;
		OBJECT(current_object)->flags &= ~0x40u;
		OBJECT(current_object)->sprite_flags &= 0xFFBFu;
		OBJECT(current_object)->sprite_palette = mem_read16(OBJECT(current_object)->player_view.palette_list + 2u);
		OBJECT(current_object)->flags &= ~0x2000u;
		OBJECT(current_object)->speed = 0;
		PLAYER(current_player)->animation_speed = 0;
		PLAYER(current_player)->mishap_flag = 0xFFu;
		PLAYER(current_player)->state_timer = 0x3C;
		jump_offset = 0x8410u;
		if (OBJECT(current_object)->kind != 0u)
		{
			jump_offset = 0x8450u;
			if (OBJECT(current_object)->kind != 1u)
				jump_offset = 0x83D0u;
		}
		OBJECT(current_object)->sprite_frame = jump_offset;
		if (OBJECT(current_object)->kind == 1u)
		{
			OBJECT(current_object)->animation = ray_stunned_animation_ADDRESS;
			OBJECT(current_object)->animation_speed = 4u;
			OBJECT(current_object)->animation_frame = 0u;
			OBJECT(current_object)->animation_timer = 0;
		}
		health = OBJECT(current_object)->health;
		OBJECT(current_object)->health = health - 4u;
		if ((int8_t)health <= 4)
			OBJECT(current_object)->health = 0;
	case 1:
		state_timer = PLAYER(current_player)->state_timer;
		PLAYER(current_player)->state_timer = state_timer - 1u;
		if ((int16_t)state_timer <= 1)
		{
			PLAYER(current_player)->substate++;
			if (OBJECT(current_object)->health > 0)
			{
				stun_recover_animation = sonic_stun_recover_animation_ADDRESS;
				if (OBJECT(current_object)->kind != 0u)
				{
					stun_recover_animation = ray_stun_recover_animation_ADDRESS;
					if (OBJECT(current_object)->kind != 1u)
						stun_recover_animation = mighty_stun_recover_animation_ADDRESS;
				}
				OBJECT(current_object)->animation = stun_recover_animation;
				OBJECT(current_object)->animation_speed = 3u;
				OBJECT(current_object)->animation_frame = 0xFFu;
				OBJECT(current_object)->animation_timer = 0;
			}
			else
			{
				PLAYER(current_player)->state = 0x16u;
				PLAYER(current_player)->substate = 0u;
			}
		}
		if (OBJECT(current_object)->kind == 1u)
			animate(&depth, &r7, &at_last_step);
		find_floor(&jump_offset, &depth, &block_offset, &ignored, &cursor);
		OBJECT(current_object)->z = OBJECT(current_object)->floor_z;
		update_screen_position(&ignored, &depth);
		add_player_to_lists();
		depth = queue_sprite_at_base(depth);
		return;
	case 2:
		animate(&depth, &r7, &at_last_step);
		if ((uint8_t)at_last_step != 0u)
		{
			PLAYER(current_player)->state = 0u;
			PLAYER(current_player)->substate = 0u;
			PLAYER(current_player)->control_lock = 4u;
			OBJECT(current_object)->direction = 0x300u;
			face_movement_direction(jump_offset);
		}
		find_floor(&jump_offset, &depth, &block_offset, &ignored, &cursor);
		OBJECT(current_object)->z = OBJECT(current_object)->floor_z;
		update_screen_position(&ignored, &depth);
		add_player_to_lists();
		depth = queue_sprite_at_base(depth);
		return;
	default:
		unknown_code(0x72D0u + PLAYER(current_player)->substate * 3u);
	}
}

void player_state_choosing_character(uint32_t cursor)
{
	if (PLAYER(current_player)->substate == 0u)
	{
		PLAYER(current_player)->substate = 0xFFu;
		PLAYER(current_player)->join_time = game_frames;
		if (game_mode != 0x1Bu)
		{
			OBJECT(current_object)->handler = player_start_stage_ADDRESS;
			PLAYER(current_player)->is_joining = 0u;
			if (intro_character < 0)
				intro_character = OBJECT(current_object)->kind;
			return;
		}
		start_character_chooser(cursor);
		if (game_mode != 0x1Bu)
		{
			OBJECT(current_object)->handler = player_start_stage_ADDRESS;
			PLAYER(current_player)->is_joining = 0u;
			if (intro_character < 0)
				intro_character = OBJECT(current_object)->kind;
		}
		return;
	}
	if (game_mode != 0x1Bu)
	{
		OBJECT(current_object)->handler = player_start_stage_ADDRESS;
		PLAYER(current_player)->is_joining = 0u;
		if (intro_character < 0)
			intro_character = OBJECT(current_object)->kind;
	}
}

void player_state_you_failed(void)
{
	uint32_t depth = 0;

	if (you_failed_done != 0u)
	{
		player_status[OBJECT(current_object)->player] = 0xFFu;
		AS8(PLAYER(current_player)->previous_z) = 0u;
		PLAYER(current_player)->in_game = 0;
		PLAYER(current_player)->continue_seconds = 9;
		PLAYER(current_player)->continue_frames = 0;
		PLAYER(current_player)->state = 0x23u;
	}
	if (game_mode_timer >= 0x3C)
		return;
	depth = queue_sprite(0u);
}

void player_state_held(uint32_t depth, uint32_t cursor)
{
	uint32_t ignored = 0, jump_offset = 0, object = 0;

	for (;;)
	{
		OBJECT(current_object)->flags |= 0x10000u;
		if (PLAYER(current_player)->substate == 0u || OBJECT(LINKED(current_object, PLAYER(current_player)->holder))->var_80 != 0u)
			break;
		PLAYER(current_player)->state = 0x17u;
		OBJECT(current_object)->flags |= 0x200000u;
		OBJECT(current_object)->flags &= ~0x10000u;
		PLAYER(current_player)->substate = 0u;
		OBJECT(current_object)->speed_x = 0;
		OBJECT(current_object)->speed_y = 0;
		OBJECT(current_object)->speed = 0;
		PLAYER(current_player)->animation_speed = 0;
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
			continue;
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
	PLAYER(current_player)->substate = 1u;
	object = LINKED(current_object, PLAYER(current_player)->holder);
	OBJECT(current_object)->x = OBJECT(object)->x;
	OBJECT(current_object)->y = OBJECT(object)->y;
	OBJECT(current_object)->y_int -= 8u;
	OBJECT(current_object)->z = OBJECT(object)->z;
	OBJECT(current_object)->z_int += 0x20u;
	update_player_blink();
	remember_platform();
	add_to_rider_list();
	project_object(&ignored, &ignored);
	depth = 0xFFu;
	queue_sprite_at_depth(&ignored, &depth);
}

void player_state_stuck(uint32_t cursor)
{
	uint32_t r0 = 0, block_offset = 0, depth = 0, ignored = 0, object = 0;

	switch (PLAYER(current_player)->substate)
	{
	case 0:
		player_stuck_begin(cursor);
		break;
	case 1:
		player_stuck_wait_release();
		break;
	case 2:
		player_stuck_shake();
		break;
	case 3:
		player_stuck_jump_out();
		break;
	case 4:
		player_stuck_begin(cursor);
		break;
	default:
		unknown_code(0x7709u + PLAYER(current_player)->substate * 3u);
	}
	remember_platform();
	object = LINKED(current_object, OBJECT(current_object)->touched_by);
	if ((uint32_t)OBJECT(object)->speed_z == 0u)
	{
		depth = (uint16_t)OBJECT(current_object)->screen_y;
		depth = (uint16_t)(depth - OBJECT(current_object)->sprite_offset_y);
		PLAYER(current_player)->draw_depth = depth;
	}
	else
		OBJECT(current_object)->z += OBJECT(object)->speed_z;
	update_screen_position(&r0, &depth);
	OBJECT(current_object)->screen_x += AS16(OBJECT(LINKED(current_object, OBJECT(current_object)->touched_by))->var_80);
	find_floor(&r0, &depth, &block_offset, &ignored, &cursor);
	keep_player_on_screen(depth);
	depth = PLAYER(current_player)->draw_depth;
	queue_sprite_at_depth(&ignored, &depth);
}

void player_stuck_begin(uint32_t object)
{
	uint8_t sound_id = 0;
	uint16_t kind = 0, stuck_frame = 0, x_int = 0, y_int = 0, z = 0;
	uint32_t free_count = 0, ignored = 0, write_index = 0, write_index_address = 0;

	PLAYER(current_player)->substate++;
	stuck_frame = 0x1A40u;
	if (OBJECT(current_object)->kind != 0u)
	{
		stuck_frame = 0x5180u;
		if (OBJECT(current_object)->kind != 1u)
			stuck_frame = 0x35C8u;
	}
	OBJECT(current_object)->sprite_frame = stuck_frame;
	OBJECT(current_object)->sprite_flags &= 0xFFBFu;
	OBJECT(current_object)->sprite_palette = mem_read16(OBJECT(current_object)->player_view.palette_list + 2u);
	OBJECT(current_object)->flags &= ~0x40u;
	OBJECT(current_object)->flags &= ~0x2000u;
	PLAYER(current_player)->control_lock = 0u;
	PLAYER(current_player)->escape_progress = 0;
	allocate_object(&free_count, &object);
	if (free_count != 0u)
	{
		OBJECT(object)->sprite_flags = 0x1004u;
		OBJECT(object)->sprite_palette = 0x8380u;
		OBJECT(object)->flags = 0u;
		OBJECT(object)->sprite_offset_x = 0;
		OBJECT(object)->sprite_offset_y = 0;
		OBJECT(object)->projected_y = OBJECT(current_object)->projected_y;
		OBJECT(object)->projected_y_int += 0xAu;
		OBJECT(object)->projected_x = OBJECT(current_object)->projected_x;
		OBJECT(object)->projected_x += 5u;
		OBJECT(object)->z = OBJECT(current_object)->z;
		OBJECT(object)->handler = stuck_effect_update_ADDRESS;
		OBJECT(object)->animation = stuck_effect_animation_ADDRESS;
		OBJECT(object)->animation_frame = 0u;
		OBJECT(object)->animation_timer = 0;
		OBJECT(object)->animation_speed = 2u;
		OBJECT(object)->kind = OBJECT(current_object)->kind;
		OBJECT(object)->player = OBJECT(current_object)->player;
		OBJECT(object)->parent = current_object;
	}
	x_int = (uint16_t)OBJECT(current_object)->x_int;
	y_int = (uint16_t)OBJECT(current_object)->y_int;
	kind = 0xDu;
	z = (uint16_t)(OBJECT(current_object)->z_int - 0x28u);
	spawn_sign_2(x_int, y_int, kind, z, object);
	sound_id = 0x23u;
	play_sound(sound_id, &write_index, &write_index_address);
	project_object(&ignored, &ignored);
}

void player_stuck_wait_release(void)
{
	uint8_t sound_id = 0;
	uint32_t write_index = 0, write_index_address = 0;

	if ((OBJECT(LINKED(current_object, OBJECT(current_object)->touched_by))->var_80 | OBJECT(LINKED(current_object, OBJECT(current_object)->touched_by))->var_81) != 0u)
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

void player_stuck_shake(void)
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
	if (OBJECT(current_object)->speed_int < 0x100 || PLAYER(current_player)->target_heading < 0x180 || PLAYER(current_player)->target_heading > 0x380)
		PLAYER(current_player)->escape_progress = 0;
	else
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
		struggle_index = (int32_t)PLAYER(current_player)->escape_progress / 0x5555;
		if ((int16_t)struggle_index > 2)
			struggle_index = 2u;
		OBJECT(current_object)->animation = mem_read32(animation + struggle_index * 4u);
		animate(&trackball_y, &r7, &r8);
		if (PLAYER(current_player)->escape_progress < 0x10000)
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

void player_stuck_jump_out(void)
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
	OBJECT(current_object)->speed = 0x4000000;
	OBJECT(current_object)->speed_z = -0x34000;
	PLAYER(current_player)->bounce_speed_z = -0x34000;
	PLAYER(current_player)->bounce_step = 0x1A000;
	PLAYER(current_player)->bounce_speed_z += PLAYER(current_player)->bounce_step;
	set_speed_from_direction();
	OBJECT(current_object)->sprite_frame = mem_read16(OBJECT(current_object)->animation + 4u);
	OBJECT(current_object)->animation_frame = 0u;
	OBJECT(current_object)->animation_timer = 3;
	OBJECT(current_object)->animation_speed = 3u;
	PLAYER(current_player)->state_timer = 0x14;
	PLAYER(current_player)->state = 0xEu;
	PLAYER(current_player)->substate = 1u;
}

void stuck_effect_update(void)
{
	uint32_t r1 = 0, r7 = 0, depth = 0, ignored = 0, off_screen = 0;

	OBJECT(current_object)->z += OBJECT(LINKED(current_object, propped_board_object))->speed_z;
	update_screen_position(&ignored, &r1);
	if ((uint32_t)OBJECT(LINKED(current_object, propped_board_object))->speed_z == 0u)
		OBJECT(current_object)->stuck_effect.draw_depth = OBJECT(current_object)->screen_y - 0x20u;
	off_screen = check_left_screen();
	if (off_screen != 0u)
	{
		delete_current_object();
		return;
	}
	if ((int8_t)OBJECT(current_object)->animation_frame < 0)
	{
		OBJECT(current_object)->screen_x += AS16(OBJECT(LINKED(current_object, propped_board_object))->var_80);
		depth = OBJECT(current_object)->stuck_effect.draw_depth;
		queue_sprite_at_depth(&ignored, &depth);
		return;
	}
	r14 = PLAYER_BLOCK(1) + ((uint32_t)OBJECT(current_object)->player << 9);
	switch (PLAYER(r14)->substate)
	{
	case 0:
	case 1:
	case 4:
		animate(&r1, &r7, &off_screen);
		break;
	case 2:
		OBJECT(current_object)->sprite_frame = 0x1AA8u;
		OBJECT(current_object)->sprite_palette = 0x8390u;
		break;
	case 3:
		break;
	default:
		unknown_code(0x7A7Cu + PLAYER(r14)->substate * 2u);
	}
	if (PLAYER(r14)->state == 0xEu)
	{
		OBJECT(current_object)->sprite_frame = 0x1AB0u;
		OBJECT(current_object)->animation_frame = 0xFFu;
	}
	OBJECT(current_object)->screen_x += AS16(OBJECT(LINKED(current_object, propped_board_object))->var_80);
	depth = OBJECT(current_object)->stuck_effect.draw_depth;
	queue_sprite_at_depth(&ignored, &depth);
}

void player_state_stage_b_sequence(uint32_t depth, uint32_t cursor)
{
	uint8_t sound_id = 0;
	uint32_t r0 = 0, at_last_step = 0, ignored = 0, standing_animations = 0, write_index_address = 0;

	for (;;)
	{
		switch (PLAYER(current_player)->ending_step)
		{
		case 0:
		case 6:
			PLAYER(current_player)->ending_step++;
			PLAYER(current_player)->path_request = 0x4Du;
			PLAYER(current_player)->control_lock = 0u;
			OBJECT(current_object)->z = OBJECT(current_object)->floor_z;
			player_move(depth, cursor);
			return;
		case 1:
		case 3:
			if (PLAYER(current_player)->path_request != 0u || PLAYER(current_player)->path != 0u)
			{
				PLAYER(current_player)->state = PLAYER(current_player)->previous_state;
				player_move(depth, cursor);
				return;
			}
			PLAYER(current_player)->ending_step++;
			PLAYER(current_player)->control_lock = 0u;
			OBJECT(current_object)->direction = 0u;
			face_movement_direction(r0);
			standing_animations = sonic_standing_animations_ADDRESS;
			if (OBJECT(current_object)->kind != 0u)
			{
				standing_animations = ray_standing_animations_ADDRESS;
				if (OBJECT(current_object)->kind != 1u)
					standing_animations = mighty_standing_animations_ADDRESS;
			}
			OBJECT(current_object)->animation = standing_animations + ROM16(standing_animations + PLAYER(current_player)->facing * 2u);
			PLAYER(current_player)->stage_b_path_done = 0xFFu;
			OBJECT(current_object)->animation_frame = 0u;
			animate_step_with_flags(&depth, &at_last_step);
			continue;
		case 2:
			if (sliding_door_opened != 0u)
			{
				PLAYER(current_player)->ending_step++;
				PLAYER(current_player)->path_request = 0x50u;
				scroll_mode = 0u;
				scroll_speed = 5u;
				if (stage_b_sound_done != 0u)
				{
					update_screen_position(&ignored, &depth);
					depth = queue_sprite(depth);
					return;
				}
				stage_b_sound_done = 0xFFu;
				sound_id = 0x94u;
				play_sound(sound_id, &depth, &write_index_address);
				sound_id = 0x58u;
				if (OBJECT(current_object)->kind != 0u)
				{
					sound_id = 0x4Eu;
					if (OBJECT(current_object)->kind != 1u)
						sound_id = 0x60u;
				}
				play_sound(sound_id, &depth, &write_index_address);
			}
			update_screen_position(&ignored, &depth);
			depth = queue_sprite(depth);
			return;
		case 4:
			if (stage_trigger_bits != 0u)
				PLAYER(current_player)->ending_step++;
			update_screen_position(&ignored, &depth);
			depth = queue_sprite(depth);
			return;
		case 5:
			if (stage_trigger_bits != 0u)
			{
				update_screen_position(&ignored, &depth);
				depth = queue_sprite(depth);
				return;
			}
			PLAYER(current_player)->ending_done = 0xFFu;
			escape_countdown_running = 0xFFu;
			PLAYER(current_player)->state = 0x13u;
			PLAYER(current_player)->path_request = 0x53u;
			update_screen_position(&ignored, &depth);
			depth = queue_sprite(depth);
			return;
		default:
			unknown_code(0x7AD5u + PLAYER(current_player)->ending_step * 3u);
		}
	}
}

void player_state_scene_thrown(uint32_t depth, uint32_t cursor)
{
	uint8_t sound_id = 0;
	uint32_t r0 = 0, r2 = 0, hurt_animations = 0, ignored = 0, step_record = 0;

	switch (scene_step)
	{
	case 0:
	case 1:
	case 3:
		if (scene_step == PLAYER(current_player)->scene_step_seen)
		{
			if (PLAYER(current_player)->previous_state == 0xDu)
			{
				player_state_hurt(depth, cursor);
				return;
			}
			find_floor(&r0, &depth, &r2, &ignored, &cursor);
			OBJECT(current_object)->z = OBJECT(current_object)->floor_z;
			project_object(&ignored, &ignored);
			add_to_rider_list();
			player_animate_standing();
			return;
		}
		PLAYER(current_player)->scene_step_seen = scene_step;
		PLAYER(current_player)->state = 0xDu;
		PLAYER(current_player)->substate = 2u;
		OBJECT(current_object)->player_view.hurt_flash_period = 0xFFu;
		OBJECT(current_object)->flags |= 0x1000000u;
		OBJECT(current_object)->z_int -= 0xA0u;
		OBJECT(current_object)->z_int -= (uint32_t)OBJECT(current_object)->kind << 5;
		OBJECT(current_object)->speed_z = 0;
		OBJECT(current_object)->speed_x = 0;
		OBJECT(current_object)->speed_y = 0;
		hurt_animations = sonic_hurt_animations_ADDRESS;
		if (OBJECT(current_object)->kind != 0u)
		{
			hurt_animations = ray_hurt_animations_ADDRESS;
			if (OBJECT(current_object)->kind != 1u)
				hurt_animations = mighty_hurt_animations_ADDRESS;
		}
		depth = ROM16(hurt_animations + PLAYER(current_player)->facing * 2u);
		OBJECT(current_object)->animation = hurt_animations + depth;
		OBJECT(current_object)->animation_frame = 0u;
		step_record = OBJECT(current_object)->animation;
		OBJECT(current_object)->sprite_flags &= ~0xC0u;
		step_record += 2u;
		OBJECT(current_object)->sprite_flags |= mem_read16(step_record);
		OBJECT(current_object)->sprite_frame = mem_read16(step_record + 2u);
		player_state_hurt(depth, cursor);
		return;
	case 2:
		if (scene_step == PLAYER(current_player)->scene_step_seen)
		{
			player_state_hurt(depth, cursor);
			return;
		}
		PLAYER(current_player)->scene_step_seen = scene_step;
		PLAYER(current_player)->state = 0xDu;
		PLAYER(current_player)->substate = 2u;
		OBJECT(current_object)->player_view.hurt_flash_period = 0xFFu;
		OBJECT(current_object)->flags |= 0x1000000u;
		OBJECT(current_object)->speed_z = -0xE0000;
		OBJECT(current_object)->speed_x = 0;
		OBJECT(current_object)->speed_y = 0x80000;
		OBJECT(current_object)->direction = 0x200u;
		face_movement_direction(0u);
		hurt_animations = sonic_hurt_animations_ADDRESS;
		if (OBJECT(current_object)->kind != 0u)
		{
			hurt_animations = ray_hurt_animations_ADDRESS;
			if (OBJECT(current_object)->kind != 1u)
				hurt_animations = mighty_hurt_animations_ADDRESS;
		}
		depth = ROM16(hurt_animations + PLAYER(current_player)->facing * 2u);
		OBJECT(current_object)->animation = hurt_animations + depth;
		OBJECT(current_object)->animation_frame = 0u;
		step_record = OBJECT(current_object)->animation;
		OBJECT(current_object)->sprite_flags &= ~0xC0u;
		step_record += 2u;
		OBJECT(current_object)->sprite_flags |= mem_read16(step_record);
		OBJECT(current_object)->sprite_frame = mem_read16(step_record + 2u);
		sound_id = 0x36u;
		if (OBJECT(current_object)->kind != 0u)
		{
			sound_id = 0x37u;
			if (OBJECT(current_object)->kind != 1u)
				sound_id = 0x77u;
		}
		play_sound(sound_id, &depth, &r2);
		player_state_hurt(depth, cursor);
		return;
	default:
		unknown_code(0x7C18u + scene_step * 3u);
	}
}

void player_state_carried_off(uint32_t depth, uint32_t cursor)
{
	uint8_t sound_id = 0;
	uint32_t r2 = 0, r7 = 0, at_last_step = 0, hurt_animations = 0, ignored = 0, near_edge_fall_animation = 0, object = 0, seesaw_launch_animation = 0, sound_number = 0;

	switch (PLAYER(current_player)->sequence_step)
	{
	case 0:
	case 7:
		PLAYER(current_player)->sequence_step++;
		OBJECT(current_object)->speed_z = 0;
		OBJECT(current_object)->speed = 0x5000000;
		OBJECT(current_object)->sprite_palette = mem_read16(OBJECT(current_object)->player_view.palette_list + 2u);
		OBJECT(current_object)->flags &= ~0x40u;
		OBJECT(current_object)->flags &= ~0x2000u;
		PLAYER(current_player)->path_request = 0x2Fu;
		OBJECT(current_object)->flags &= ~0x2000u;
		OBJECT(current_object)->flags &= ~4u;
		OBJECT(current_object)->flags &= ~0x8000000u;
		PLAYER(current_player)->control_lock = 0u;
		PLAYER(current_player)->invulnerable_timer = 0;
		PLAYER(current_player)->previous_state = 0x13u;
		OBJECT(current_object)->z = OBJECT(current_object)->floor_z;
	case 1:
		if (OBJECT(current_object)->player_view.floor_block_high >= 0)
		{
			PLAYER(current_player)->state = PLAYER(current_player)->previous_state;
			player_move(depth, cursor);
			return;
		}
		PLAYER(current_player)->sequence_step++;
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
		sound_number = 0x36u;
		if (OBJECT(current_object)->kind != 0u)
		{
			sound_number = 0x37u;
			if (OBJECT(current_object)->kind != 1u)
				sound_number = 0x77u;
		}
		play_sound((uint8_t)sound_number, &depth, &r2);
	case 2:
		if ((int8_t)OBJECT(current_object)->animation_speed >= 0)
		{
			at_last_step = animate_and_move(0u);
			if ((uint8_t)at_last_step != 0u)
				OBJECT(current_object)->animation_speed = 0xFFu;
		}
		if (OBJECT(current_object)->z_int < 0x9C0)
		{
			object = LINKED(current_object, scene_object);
			if ((uint16_t)object != 0xFFFFu)
			{
				OBJECT(current_object)->x += (int32_t)((uint32_t)OBJECT(object)->x - OBJECT(current_object)->x) >> 4;
				OBJECT(current_object)->y += (int32_t)(OBJECT(object)->y + ((uint32_t)OBJECT(current_object)->player << 21) - OBJECT(current_object)->y) >> 4;
				OBJECT(current_object)->z += (int32_t)((uint32_t)OBJECT(object)->z - OBJECT(current_object)->z) >> 3;
			}
			else
				OBJECT(current_object)->z += 0x40000u;
		}
		else
			OBJECT(current_object)->z += 0x40000u;
		if (OBJECT(current_object)->z_int >= 0x9D0)
			PLAYER(current_player)->sequence_step++;
		find_floor(&sound_number, &depth, &r2, &ignored, &cursor);
		project_object(&ignored, &depth);
		depth = queue_sprite(depth);
		return;
	case 3:
		PLAYER(current_player)->sequence_step++;
		PLAYER(current_player)->state = 0xDu;
		PLAYER(current_player)->previous_state = PLAYER(current_player)->state;
		PLAYER(current_player)->substate = 3u;
		OBJECT(current_object)->player_view.hurt_flash_period = 0xFFu;
		OBJECT(current_object)->flags |= 0x1000000u;
		OBJECT(current_object)->flags &= ~4u;
		OBJECT(current_object)->flags &= ~0x8000000u;
		PLAYER(current_player)->state_timer = 2;
		OBJECT(current_object)->z = OBJECT(current_object)->floor_z;
		OBJECT(current_object)->speed_x = 0;
		OBJECT(current_object)->speed_y = 0;
		PLAYER(current_player)->path_request = 0u;
		PLAYER(current_player)->path = 0u;
		hurt_animations = sonic_hurt_animations_ADDRESS;
		if (OBJECT(current_object)->kind != 0u)
		{
			hurt_animations = ray_hurt_animations_ADDRESS;
			if (OBJECT(current_object)->kind != 1u)
				hurt_animations = mighty_hurt_animations_ADDRESS;
		}
		OBJECT(current_object)->animation = hurt_animations + ROM16(hurt_animations + PLAYER(current_player)->facing * 2u);
		OBJECT(current_object)->animation_frame = 2u;
		depth = OBJECT(current_object)->animation_frame;
		cursor = OBJECT(current_object)->animation + 2u + depth * 4u;
		OBJECT(current_object)->sprite_flags &= ~0xC0u;
		OBJECT(current_object)->sprite_flags |= mem_read16(cursor);
		cursor += 2u;
		OBJECT(current_object)->sprite_frame = mem_read16(cursor);
	case 4:
		if (PLAYER(current_player)->previous_state == 0xDu)
		{
			PLAYER(current_player)->state = 0xDu;
			player_state_hurt(depth, cursor);
			return;
		}
		burning_boulder_released = 0xFFu;
		find_floor(&sound_number, &depth, &r2, &ignored, &cursor);
		OBJECT(current_object)->z = OBJECT(current_object)->floor_z;
		OBJECT(current_object)->z_fraction -= 0x10u;
		project_object(&ignored, &depth);
		check_player_stalled();
		if (PLAYER(current_player)->state == 0xDu)
		{
			player_state_hurt(depth, cursor);
			return;
		}
		if (seesaw_launched == 0u)
		{
			player_animate_standing();
			return;
		}
		PLAYER(current_player)->sequence_step++;
		seesaw_launch_animation = sonic_seesaw_launch_animation_ADDRESS;
		if (OBJECT(current_object)->kind != 0u)
		{
			seesaw_launch_animation = ray_seesaw_launch_animation_ADDRESS;
			if (OBJECT(current_object)->kind != 1u)
				seesaw_launch_animation = mighty_seesaw_launch_animation_ADDRESS;
		}
		OBJECT(current_object)->animation = seesaw_launch_animation;
		OBJECT(current_object)->animation_frame = 0xFFu;
		OBJECT(current_object)->animation_speed = 3u;
		OBJECT(current_object)->animation_timer = 0;
		sound_id = 0x36u;
		if (OBJECT(current_object)->kind != 0u)
		{
			sound_id = 0x37u;
			if (OBJECT(current_object)->kind != 1u)
				sound_id = 0x77u;
		}
		play_sound(sound_id, &depth, &r2);
		OBJECT(current_object)->z -= 0xC0000u;
		animate(&depth, &r7, &at_last_step);
		project_object(&ignored, &depth);
		if (OBJECT(current_object)->screen_y < 0)
			PLAYER(current_player)->sequence_step++;
		depth = queue_sprite(depth);
		return;
	case 5:
		OBJECT(current_object)->z -= 0xC0000u;
		animate(&depth, &r7, &at_last_step);
		project_object(&ignored, &depth);
		if (OBJECT(current_object)->screen_y < 0)
			PLAYER(current_player)->sequence_step++;
		depth = queue_sprite(depth);
		return;
	case 6:
		object = (current_object & ~0xFFFFu) | player_carriers[OBJECT(current_object)->player];
		OBJECT(current_object)->x += (int32_t)((uint32_t)OBJECT(object)->x - OBJECT(current_object)->x) >> 4;
		OBJECT(current_object)->y += (int32_t)((uint32_t)OBJECT(object)->y - OBJECT(current_object)->y) >> 4;
		OBJECT(current_object)->z += (int32_t)((uint32_t)OBJECT(object)->z - OBJECT(current_object)->z) >> 3;
		animate(&depth, &r7, &at_last_step);
		project_object(&ignored, &depth);
		if (OBJECT(current_object)->z_int <= 0x530)
		{
			PLAYER(current_player)->state = 0x32u;
			PLAYER(current_player)->substate = 0u;
		}
		depth = queue_sprite(depth);
		return;
	default:
		unknown_code(0x7DB5u + PLAYER(current_player)->sequence_step * 3u);
	}
}

void player_state_trapped(uint32_t depth, uint32_t cursor)
{
	uint8_t sound_id = 0;
	uint16_t kind = 0, x_int = 0, y_int = 0, z = 0;
	uint32_t r2 = 0, r7 = 0, at_last_step = 0, character_frame = 0, free_count = 0, ignored = 0, jump_offset = 0, running_animations = 0, speed_int = 0, struggle_index = 0, trackball_speed = 0, trackball_y = 0, trap_struggle_animations = 0, trapped_animation = 0;
	uint32_t escape_progress = 0;

	for (;;)
	{
		check_player_contacts(&depth, &cursor);
		if (PLAYER(current_player)->state != 0x20u)
			break;
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
			continue;
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
	case 6:
		PLAYER(current_player)->substate++;
		trapped_animation = sonic_trapped_animation_ADDRESS;
		if (OBJECT(current_object)->kind != 0u)
		{
			trapped_animation = ray_trapped_animation_ADDRESS;
			if (OBJECT(current_object)->kind != 1u)
				trapped_animation = mighty_trapped_animation_ADDRESS;
		}
		OBJECT(current_object)->animation = trapped_animation;
		OBJECT(current_object)->animation_frame = 0xFFu;
		OBJECT(current_object)->animation_timer = 0;
		OBJECT(current_object)->animation_speed = 2u;
		OBJECT(current_object)->sprite_flags &= 0xFFBFu;
		allocate_object(&free_count, &cursor);
		if (free_count != 0u)
		{
			OBJECT(cursor)->sprite_flags = 0x1000u;
			OBJECT(cursor)->sprite_frame = 0xB4F0u;
			OBJECT(cursor)->sprite_palette = 0xD10u;
			OBJECT(cursor)->flags = 0u;
			OBJECT(cursor)->sprite_offset_x = 0;
			OBJECT(cursor)->sprite_offset_y = 0;
			OBJECT(cursor)->handler = trap_flash_start_ADDRESS;
			OBJECT(cursor)->x = OBJECT(current_object)->x;
			OBJECT(cursor)->y = OBJECT(current_object)->y;
			OBJECT(cursor)->z = OBJECT(current_object)->z;
			OBJECT(cursor)->animation_speed = 8u;
			allocate_object(&free_count, &cursor);
			if (free_count != 0u)
			{
				OBJECT(cursor)->sprite_flags = 0x1000u;
				OBJECT(cursor)->sprite_frame = 0xB4F8u;
				OBJECT(cursor)->sprite_palette = 0xD20u;
				OBJECT(cursor)->flags = 0u;
				OBJECT(cursor)->sprite_offset_x = 0;
				OBJECT(cursor)->sprite_offset_y = 2;
				OBJECT(cursor)->trap.player_block = OBJECT(current_object)->player_view.block;
				OBJECT(cursor)->x = OBJECT(current_object)->x;
				OBJECT(cursor)->y = OBJECT(current_object)->y;
				OBJECT(cursor)->z = OBJECT(current_object)->z;
				OBJECT(cursor)->handler = trap_start_ADDRESS;
				sound_id = 0x21u;
				play_sound(sound_id, &depth, &r2);
			}
		}
		return;
	case 1:
		animate(&depth, &r7, &at_last_step);
		if ((uint8_t)at_last_step != 0u)
		{
			PLAYER(current_player)->substate++;
			OBJECT(current_object)->animation_frame = 0xFFu;
			OBJECT(current_object)->animation_timer = 0;
			OBJECT(current_object)->animation_speed = 4u;
			PLAYER(current_player)->escape_progress = 0;
			x_int = (uint16_t)OBJECT(current_object)->x_int;
			y_int = (uint16_t)OBJECT(current_object)->y_int;
			kind = 0x14u;
			z = (uint16_t)(OBJECT(current_object)->z_int - 0x20u);
			spawn_sign_2(x_int, y_int, kind, z, cursor);
		}
		project_object(&ignored, &depth);
		depth = queue_sprite(depth);
		return;
	case 2:
		read_player_trackball();
		trackball_speed = PLAYER(current_player)->trackball_x;
		trackball_y = PLAYER(current_player)->trackball_y;
		trackball_speed = trackball_speed * trackball_speed + trackball_y * trackball_y;
		square_root(&trackball_speed, &ignored);
		trackball_speed &= 0xFFFFu;
		trackball_speed = trackball_speed * trackball_speed;
		if ((int32_t)trackball_speed > 0x5000000)
			trackball_speed = 0x5000000u;
		OBJECT(current_object)->speed = trackball_speed;
		speed_int = (uint16_t)OBJECT(current_object)->speed_int;
		if ((int16_t)speed_int > 0x100)
			PLAYER(current_player)->escape_progress += speed_int * 4u;
		else
		{
			escape_progress = PLAYER(current_player)->escape_progress;
			PLAYER(current_player)->escape_progress = escape_progress - 0x2000u;
			if ((int32_t)escape_progress <= 0x2000)
				PLAYER(current_player)->escape_progress = 0;
		}
		trap_struggle_animations = sonic_trap_struggle_animations_ADDRESS;
		if (OBJECT(current_object)->kind != 0u)
		{
			trap_struggle_animations = ray_trap_struggle_animations_ADDRESS;
			if (OBJECT(current_object)->kind != 1u)
				trap_struggle_animations = mighty_trap_struggle_animations_ADDRESS;
		}
		struggle_index = AT16(PLAYER(current_player)->escape_progress, 2);
		if ((int16_t)struggle_index >= 4)
			struggle_index = 4u;
		OBJECT(current_object)->animation = ROM32(trap_struggle_animations + struggle_index * 4u);
		animate(&struggle_index, &r7, &at_last_step);
		project_object(&ignored, &depth);
		if ((int16_t)AT16(PLAYER(current_player)->escape_progress, 2) < 4)
		{
			add_player_to_lists();
			depth = queue_sprite(depth);
			return;
		}
		PLAYER(current_player)->substate++;
		character_frame = 0xB4E8u;
		if (OBJECT(current_object)->kind != 0u)
		{
			character_frame = 0xB580u;
			if (OBJECT(current_object)->kind != 1u)
				character_frame = 0xB468u;
		}
		OBJECT(current_object)->sprite_frame = character_frame;
		OBJECT(current_object)->animation_timer = 0x10;
		OBJECT(current_object)->animation_speed = 2u;
		OBJECT(current_object)->animation_frame = 0xFFu;
		OBJECT(current_object)->speed_z = -0x80000;
		OBJECT(current_object)->speed_x = 0x20000;
		OBJECT(current_object)->direction = 0x300u;
		face_movement_direction(character_frame);
		add_player_to_lists();
		depth = queue_sprite(depth);
		return;
	case 3:
	case 4:
		OBJECT(current_object)->speed_z += 0x6000u;
		OBJECT(current_object)->z += OBJECT(current_object)->speed_z;
		OBJECT(current_object)->x += OBJECT(current_object)->speed_x;
		running_animations = sonic_running_animations_ADDRESS;
		if (OBJECT(current_object)->kind != 0u)
		{
			running_animations = ray_running_animations_ADDRESS;
			if (OBJECT(current_object)->kind != 1u)
				running_animations = mighty_running_animations_ADDRESS;
		}
		OBJECT(current_object)->animation = running_animations + ROM16(running_animations + PLAYER(current_player)->facing * 2u);
		animate_with_flags(&depth, &at_last_step);
		find_floor(&jump_offset, &depth, &r2, &ignored, &cursor);
		if (OBJECT(current_object)->floor_z_int <= OBJECT(current_object)->z_int)
		{
			PLAYER(current_player)->substate++;
			OBJECT(current_object)->z = OBJECT(current_object)->floor_z;
			OBJECT(current_object)->speed_z = -0x40000;
		}
		project_object(&ignored, &ignored);
		ignored = queue_sprite_at_floor();
		return;
	case 5:
		OBJECT(current_object)->speed = 0;
		PLAYER(current_player)->state = 0u;
		PLAYER(current_player)->substate = 0u;
		project_object(&ignored, &ignored);
		ignored = queue_sprite_at_base(ignored);
		return;
	default:
		unknown_code(0x811Fu + PLAYER(current_player)->substate * 3u);
	}
}

void trap_flash_start(void)
{
	uint32_t ignored = 0;

	project_object(&ignored, &ignored);
	OBJECT(current_object)->handler = trap_flash_update_ADDRESS;
}

void trap_flash_update(void)
{
	uint32_t r1 = 0, ignored = 0;
	uint8_t animation_speed = 0;

	update_screen_position(&ignored, &r1);
	animation_speed = OBJECT(current_object)->animation_speed;
	OBJECT(current_object)->animation_speed = animation_speed - 1u;
	if ((int8_t)animation_speed <= 1)
	{
		delete_current_object();
		return;
	}
	if ((OBJECT(current_object)->animation_speed & 2u) == 0u)
		return;
	r1 = queue_sprite(r1);
}

void trap_start(void)
{
	uint32_t ignored = 0;

	project_object(&ignored, &ignored);
	OBJECT(current_object)->handler = trap_update_ADDRESS;
}

void trap_update(void)
{
	uint32_t r1 = 0, ignored = 0, off_screen = 0;

	off_screen = is_off_screen();
	if (off_screen != 0u)
	{
		delete_current_object();
		return;
	}
	if ((int8_t)mem_read8(OBJECT(current_object)->trap.player_block + 3u) >= 3)
		OBJECT(current_object)->sprite_frame = 0xB500u;
	update_screen_position(&ignored, &r1);
	r1 = queue_sprite(r1);
}

void player_state_riding(void)
{
	uint16_t character_frame = 0;
	uint32_t block_offset = 0, cursor = 0, depth = 0, ignored = 0, object = 0, object_2 = 0, x = 0, y = 0;

	if (PLAYER(current_player)->substate == 0u)
	{
		PLAYER(current_player)->substate = 0xFFu;
		if (OBJECT(current_object)->x_int <= 0xAE0 && OBJECT(current_object)->x_int >= 0xA60)
			PLAYER(current_player)->ridden_object = 0u;
		if (OBJECT(current_object)->y_int > 0xE74)
		{
			if (OBJECT(current_object)->y_int >= 0xEF2)
				OBJECT(current_object)->y = 0xEF20000;
		}
		else
			OBJECT(current_object)->y = 0xE740000;
		if (OBJECT(current_object)->z_int < 0x530 || OBJECT(current_object)->z_int > 0x564)
			OBJECT(current_object)->z = 0x5300000;
		OBJECT(current_object)->sprite_palette = mem_read16(OBJECT(current_object)->player_view.palette_list + 2u);
		OBJECT(current_object)->flags &= ~0x40u;
		OBJECT(current_object)->flags &= ~0x2000u;
		PLAYER(current_player)->control_lock = 0u;
		OBJECT(current_object)->sprite_flags &= ~0xFu;
		OBJECT(current_object)->sprite_flags |= 4u;
	}
	OBJECT(current_object)->flags &= ~8u;
	OBJECT(current_object)->flags &= ~0x10u;
	find_floor(&x, &y, &block_offset, &ignored, &cursor);
	if (PLAYER(current_player)->ridden_object != 0u)
	{
		object = (current_object & ~0xFFFFu) | PLAYER(current_player)->ridden_object;
		OBJECT(current_object)->x = OBJECT(object)->x;
		OBJECT(current_object)->z = OBJECT(object)->z;
	}
	else
	{
		object_2 = LINKED(current_object, OBJECT(current_object)->platform);
		OBJECT(current_object)->x = OBJECT(object_2)->x;
		OBJECT(current_object)->z = OBJECT(object_2)->z;
	}
	OBJECT(current_object)->z_int += 0x20u;
	OBJECT(current_object)->x_int += 0x10u;
	project_object(&ignored, &depth);
	character_frame = 0xB2B8u;
	if (OBJECT(current_object)->kind != 0u)
	{
		character_frame = 0xB288u;
		if (OBJECT(current_object)->kind != 1u)
			character_frame = 0xB2E8u;
	}
	OBJECT(current_object)->sprite_frame = character_frame;
	OBJECT(current_object)->sprite_flags &= 0xFFBFu;
	if ((OBJECT(LINKED(current_object, OBJECT(current_object)->platform))->flags & 1u) == 0u)
	{
		depth = queue_sprite(depth);
		return;
	}
	if (OBJECT(current_object)->y_int < 0xE77)
		OBJECT(current_object)->y = 0xE770000;
	if (OBJECT(current_object)->y_int > 0xED5)
		OBJECT(current_object)->y = 0xED50000;
	OBJECT(current_object)->x_int -= 0x10u;
	if (OBJECT(current_object)->x_int <= 0xC11)
		OBJECT(current_object)->x_int = 0xC19;
	start_climb();
	depth = queue_sprite(depth);
}

void player_state_final_stage(void)
{
	if (ending_finished != 0u)
	{
		AS8(PLAYER(current_player)->previous_z) = 0u;
		PLAYER(current_player)->in_game = 0;
		PLAYER(current_player)->continue_seconds = 9;
		PLAYER(current_player)->continue_frames = 0;
		PLAYER(current_player)->state = 0x23u;
		PLAYER(current_player)->rings += 0xC8u;
		PLAYER(current_player)->stages_reached++;
	}
}

void player_state_tripped(uint32_t cursor)
{
	uint8_t sound_id = 0;
	uint32_t r1 = 0, r8 = 0, flail_animation = 0, hurt_animations = 0, ignored = 0, step_record = 0, write_index_address = 0;

	switch (PLAYER(current_player)->substate)
	{
	case 0:
	case 2:
		PLAYER(current_player)->substate++;
		OBJECT(current_object)->flags &= ~0x40u;
		OBJECT(current_object)->sprite_palette = mem_read16(OBJECT(current_object)->player_view.palette_list + 2u);
		OBJECT(current_object)->flags &= ~0x2000u;
		PLAYER(current_player)->control_lock = 0u;
		PLAYER(current_player)->state_timer = 0;
		OBJECT(current_object)->speed = 0;
		OBJECT(current_object)->speed_x = 0;
		OBJECT(current_object)->speed_y = 0;
		OBJECT(current_object)->speed_z = -0x80000;
		PLAYER(current_player)->ground_speed = 0;
		PLAYER(current_player)->previous_x = OBJECT(current_object)->x;
		PLAYER(current_player)->previous_y = OBJECT(current_object)->y;
		flail_animation = flail_animation_sonic_ADDRESS;
		if (OBJECT(current_object)->kind != 0u)
		{
			flail_animation = flail_animation_ray_ADDRESS;
			if (OBJECT(current_object)->kind != 1u)
				flail_animation = flail_animation_mighty_ADDRESS;
		}
		OBJECT(current_object)->animation = flail_animation;
		OBJECT(current_object)->animation_frame = 0u;
		OBJECT(current_object)->animation_speed = 0u;
		OBJECT(current_object)->animation_timer = 0;
		sound_id = 0x47u;
		if (OBJECT(current_object)->kind != 0u)
		{
			sound_id = 0x4Fu;
			if (OBJECT(current_object)->kind != 1u)
				sound_id = 0x67u;
		}
		play_sound(sound_id, &r1, &write_index_address);
		break;
	case 1:
		break;
	default:
		unknown_code(0x85F7u + PLAYER(current_player)->substate * 3u);
	}
	OBJECT(current_object)->speed_z += 0x6000u;
	OBJECT(current_object)->z += OBJECT(current_object)->speed_z;
	animate_with_flags(&r1, &r8);
	if (OBJECT(current_object)->speed_z_int < 0)
	{
		project_object(&ignored, &r1);
		check_zone_contact();
		check_player_contacts(&r1, &cursor);
		add_player_to_lists();
		ignored = queue_sprite_at_floor();
		return;
	}
	PLAYER(current_player)->state = 0xDu;
	PLAYER(current_player)->substate = 2u;
	OBJECT(current_object)->flags |= 0x1000000u;
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
	project_object(&ignored, &r1);
	check_zone_contact();
	check_player_contacts(&r1, &cursor);
	add_player_to_lists();
	ignored = queue_sprite_at_floor();
}

void player_state_swept_away(void)
{
	uint8_t sound_id = 0;
	uint16_t dy = 0;
	uint32_t r1 = 0, r8 = 0, away_angle = 0, chaser_angle = 0, chaser_dy = 0, flail_animation = 0, ignored = 0, object = 0, write_index_address = 0;
	uint8_t health = 0;

	if (PLAYER(current_player)->substate == 0u)
	{
		PLAYER(current_player)->substate = 0xFFu;
		PLAYER(current_player)->mishap_flag = 0xFFu;
		PLAYER(current_player)->swirl_turn_rate = 0x220u;
		OBJECT(current_object)->flags &= ~0x40u;
		OBJECT(current_object)->sprite_palette = mem_read16(OBJECT(current_object)->player_view.palette_list + 2u);
		OBJECT(current_object)->flags &= ~0x2000u;
		OBJECT(current_object)->speed = 0x2000000;
		OBJECT(current_object)->speed_z = 0;
		object = LINKED(current_object, chaser_object);
		away_angle = (uint16_t)(OBJECT(current_object)->x_int - OBJECT(object)->x_int);
		chaser_dy = (uint16_t)(OBJECT(current_object)->y_int - OBJECT(object)->y_int);
		angle_of_vector(chaser_dy, &away_angle, &ignored);
		OBJECT(current_object)->direction = (away_angle - 0x100u) & 0x3FFu;
		flail_animation = flail_animation_sonic_ADDRESS;
		if (OBJECT(current_object)->kind != 0u)
		{
			flail_animation = flail_animation_ray_ADDRESS;
			if (OBJECT(current_object)->kind != 1u)
				flail_animation = flail_animation_mighty_ADDRESS;
		}
		OBJECT(current_object)->animation = flail_animation;
		OBJECT(current_object)->animation_frame = 0u;
		OBJECT(current_object)->animation_speed = 3u;
		OBJECT(current_object)->animation_timer = 0;
		sound_id = 0x2Fu;
		if (OBJECT(current_object)->kind != 0u)
		{
			sound_id = 0x37u;
			if (OBJECT(current_object)->kind != 1u)
				sound_id = 0x77u;
		}
		play_sound(sound_id, &chaser_dy, &write_index_address);
	}
	object = LINKED(current_object, chaser_object);
	chaser_angle = (uint16_t)(OBJECT(object)->x_int - OBJECT(current_object)->x_int);
	dy = (uint16_t)(OBJECT(object)->y_int - OBJECT(current_object)->y_int);
	angle_of_vector(dy, &chaser_angle, &ignored);
	OBJECT(current_object)->direction -= divide_s16((uint16_t)(((OBJECT(current_object)->direction - chaser_angle) & 0x3FFu) << 4), PLAYER(current_player)->swirl_turn_rate);
	OBJECT(current_object)->direction &= 0x3FFu;
	OBJECT(current_object)->speed += 0x100000u;
	if (OBJECT(current_object)->speed_int >= 0xA00)
		OBJECT(current_object)->speed = 0xA000000;
	OBJECT(current_object)->speed_z += 0x280u;
	OBJECT(current_object)->z -= OBJECT(current_object)->speed_z;
	set_speed_from_direction();
	OBJECT(current_object)->x += OBJECT(current_object)->speed_x;
	OBJECT(current_object)->y += OBJECT(current_object)->speed_y;
	project_object(&ignored, &r1);
	animate_with_flags(&r1, &r8);
	if (OBJECT(current_object)->screen_y > -0x40)
	{
		r1 = queue_sprite(r1);
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
		OBJECT(current_object)->screen_x = 0xA0;
	}
	r1 = queue_sprite(r1);
}

void player_state_climbing_object(uint32_t cursor)
{
	uint8_t sound_id = 0;
	uint16_t x_shift = 0;
	uint32_t r1 = 0, depth = 0, ignored = 0, near_edge_fall_animation = 0, object = 0, write_index_address = 0;

	switch (PLAYER(current_player)->substate)
	{
	case 0:
		player_object_climb_begin();
		break;
	case 1:
		player_object_climb_push(cursor);
		break;
	case 2:
		player_object_climb_move();
		break;
	case 3:
		player_object_climb_begin();
		break;
	default:
		unknown_code(0x8A0Au + PLAYER(current_player)->substate * 3u);
	}
	update_player_blink();
	OBJECT(current_object)->projected_x = PLAYER(current_player)->screen_x_int;
	project_object(&ignored, &r1);
	if (OBJECT(current_object)->x_int <= 0x450 && handle_rod_drop_players != 0u)
	{
		PLAYER(current_player)->state = 6u;
		PLAYER(current_player)->substate = 1u;
		PLAYER(current_player)->ledge_side = 4;
		OBJECT(current_object)->floor_block = -1;
		OBJECT(current_object)->floor_z = 0x8000000;
		OBJECT(current_object)->sprite_offset_x = 0;
		OBJECT(current_object)->sprite_offset_y = 0xA;
		PLAYER(current_player)->fall_draw_depth = ((int32_t)((uint32_t)OBJECT(current_object)->projected_y + OBJECT(current_object)->z) >> 16) - view_y;
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
		OBJECT(current_object)->sprite_flags &= ~0xFu;
		OBJECT(current_object)->sprite_flags |= 0xAu;
		OBJECT(current_object)->sprite_palette |= 0x8000u;
		OBJECT(current_object)->projected_y -= 0x200000u;
		OBJECT(current_object)->sprite_layer = OBJECT(current_object)->projected_y_int;
		x_shift = 0x18u;
		if ((int32_t)OBJECT(current_object)->animation >= 0x1128EE)
			x_shift = 0x14u;
		OBJECT(current_object)->projected_x -= x_shift;
		OBJECT(current_object)->speed_z = 0;
		if (OBJECT(current_object)->health <= 8)
		{
			sound_id = 0x2Fu;
			if (OBJECT(current_object)->kind != 0u)
			{
				sound_id = 0x37u;
				if (OBJECT(current_object)->kind != 1u)
					sound_id = 0x77u;
			}
			play_sound(sound_id, &r1, &write_index_address);
		}
	}
	if (PLAYER(current_player)->climb_remaining <= OBJECT(current_object)->x_int)
	{
		PLAYER(current_player)->substate = 0u;
		PLAYER(current_player)->state = 0u;
		OBJECT(current_object)->speed = 0;
		PLAYER(current_player)->animation_speed = 0;
		OBJECT(current_object)->sprite_flags &= ~0xFu;
		OBJECT(current_object)->sprite_flags |= 4u;
		OBJECT(current_object)->sprite_offset_x = 0;
		OBJECT(current_object)->sprite_offset_y = 0xA;
		OBJECT(current_object)->flags &= ~4u;
	}
	object = LINKED(current_object, OBJECT(current_object)->touched_by);
	object = LINKED(object, OBJECT(object)->parent);
	PLAYER(current_player)->draw_depth = (AS8(OBJECT(object)->spawn_arg_1) - PLAYER(current_player)->object_climb_steps) * 4u + OBJECT(object)->hitbox_size_x;
	depth = (uint16_t)(PLAYER(current_player)->draw_depth + 2u);
	queue_sprite_at_depth(&ignored, &depth);
}

void player_object_climb_begin(void)
{
	uint32_t animation = 0, object = 0;

	PLAYER(current_player)->substate++;
	OBJECT(current_object)->sprite_flags &= ~0x4Fu;
	OBJECT(current_object)->sprite_flags |= 0xAu;
	OBJECT(current_object)->speed = 0;
	PLAYER(current_player)->animation_speed = 0;
	PLAYER(current_player)->step_accumulator = 0;
	PLAYER(current_player)->object_climb_steps = 1u;
	OBJECT(current_object)->sprite_palette = mem_read16(OBJECT(current_object)->player_view.palette_list + 2u);
	OBJECT(current_object)->flags &= ~0x40u;
	OBJECT(current_object)->flags &= ~0x2000u;
	object = LINKED(current_object, OBJECT(current_object)->touched_by);
	OBJECT(current_object)->x = OBJECT(object)->x;
	OBJECT(current_object)->y = OBJECT(object)->y;
	OBJECT(current_object)->z = 0x780000;
	OBJECT(current_object)->sprite_offset_x = -0x10;
	OBJECT(current_object)->sprite_offset_y = -0x26;
	PLAYER(current_player)->screen_x = ((int32_t)((uint32_t)OBJECT(current_object)->x + OBJECT(current_object)->y) >> 8) * 0xB5u;
	AS16(PLAYER(current_player)->step_accumulator) = OBJECT(current_object)->x_int;
	animation = 0x9CB3u;
	if (OBJECT(current_object)->kind != 0u)
		animation = 0x9CDDu;
	OBJECT(current_object)->animation = animation;
	PLAYER(current_player)->climb_hand = 0;
	if (OBJECT(current_object)->kind == 1u)
	{
		OBJECT(current_object)->sprite_offset_x = -0x16;
		OBJECT(current_object)->sprite_offset_y = -0x33;
		OBJECT(current_object)->animation = 0x9CCFu;
		PLAYER(current_player)->climb_hand = -1;
	}
	OBJECT(current_object)->sprite_frame = mem_read16(OBJECT(current_object)->animation + 2u);
}

void player_object_climb_push(uint32_t cursor)
{
	uint8_t sound_id = 0;
	uint32_t r1 = 0, write_index_address = 0;

	set_climb_speed_from_trackball();
	PLAYER(current_player)->target_heading = OBJECT(current_object)->direction;
	if ((uint16_t)OBJECT(current_object)->speed_int != 0u && ((OBJECT(current_object)->direction - 0x380u) & 0x3FFu) < 0x200u)
	{
		AT16(PLAYER(current_player)->step_accumulator, 2) += OBJECT(current_object)->speed_int;
		if ((int16_t)AT16(PLAYER(current_player)->step_accumulator, 2) >= 0x400)
		{
			AT16(PLAYER(current_player)->step_accumulator, 2) = 0u;
			if ((uint16_t)(PLAYER(current_player)->screen_x_int + 0x12u - view_x - 0x10u) >= 0x120u || (uint16_t)(((int32_t)((uint32_t)OBJECT(current_object)->projected_y + OBJECT(current_object)->z - 0x90000u) >> 16) - view_y - 0x10u) >= 0xC0u)
				return;
			PLAYER(current_player)->substate++;
			PLAYER(current_player)->object_climb_steps++;
			OBJECT(current_object)->animation_frame = 0u;
			OBJECT(current_object)->animation_speed = 4u;
			OBJECT(current_object)->animation_timer = 0;
			spawn_climb_hand(cursor);
			sound_id = 0x64u;
			if (OBJECT(current_object)->kind != 0u)
			{
				sound_id = 0x6Bu;
				if (OBJECT(current_object)->kind != 1u)
					sound_id = 0x51u;
			}
			play_sound(sound_id, &r1, &write_index_address);
		}
	}
}

void player_object_climb_move(void)
{
	uint32_t negative = 0;
	uint32_t r7 = 0, at_last_step = 0, frame = 0, world_x = 0, world_y = 0;
	uint8_t next_hand = 0;

	animate(&frame, &r7, &at_last_step);
	if ((uint8_t)at_last_step == 0u)
		return;
	PLAYER(current_player)->substate = 1u;
	next_hand = 0u - PLAYER(current_player)->climb_hand;
	negative = next_hand >> 7;
	PLAYER(current_player)->climb_hand = next_hand;
	if (next_hand != 0u)
	{
		if (negative)
		{
			OBJECT(current_object)->animation = 0x9CCFu;
			OBJECT(current_object)->sprite_offset_x = -0x16;
			OBJECT(current_object)->sprite_offset_y = -0x33;
		}
		else
		{
			OBJECT(current_object)->animation = 0x9CC1u;
			OBJECT(current_object)->sprite_offset_x = -0x10;
			OBJECT(current_object)->sprite_offset_y = -0x2A;
		}
	}
	OBJECT(current_object)->sprite_frame = mem_read16(OBJECT(current_object)->animation + 2u);
	PLAYER(current_player)->screen_x += 0x120000u;
	OBJECT(current_object)->projected_y -= 0x90000u;
	world_x = PLAYER(current_player)->screen_x;
	world_y = OBJECT(current_object)->projected_y * 2u;
	unproject_point(&world_x, &world_y);
	OBJECT(current_object)->x = world_x;
	OBJECT(current_object)->y = world_y;
}

void spawn_climb_hand(uint32_t hand)
{
	uint16_t sprite_frame = 0;
	uint32_t free_count = 0;

	allocate_object(&free_count, &hand);
	if (free_count == 0u)
		return;
	OBJECT(hand)->sprite_flags = 0x110Au;
	OBJECT(hand)->sprite_palette = OBJECT(current_object)->sprite_palette;
	OBJECT(hand)->sprite_offset_x = OBJECT(current_object)->sprite_offset_x;
	OBJECT(hand)->sprite_offset_y = OBJECT(current_object)->sprite_offset_y;
	OBJECT(hand)->flags = 0u;
	OBJECT(hand)->parent = current_object;
	if ((uint8_t)PLAYER(current_player)->climb_hand == 0u)
	{
		sprite_frame = 0x8300u;
		if (OBJECT(current_object)->kind != 0u)
			sprite_frame = 0x8258u;
	}
	else if (PLAYER(current_player)->climb_hand < 0)
		sprite_frame = 0x82C8u;
	else
		sprite_frame = 0x82C0u;
	OBJECT(hand)->sprite_frame = sprite_frame;
	OBJECT(hand)->projected_x = OBJECT(current_object)->projected_x;
	OBJECT(hand)->projected_y = OBJECT(current_object)->projected_y;
	OBJECT(hand)->z = OBJECT(current_object)->z;
	OBJECT(hand)->handler = climb_hand_update_ADDRESS;
}

void climb_hand_update(void)
{
	uint32_t r1 = 0, depth = 0, ignored = 0, object = 0;

	if (PLAYER(PLAYER_BLOCK(1) + ((uint32_t)OBJECT(LINKED(current_object, OBJECT(current_object)->parent))->player << 9))->state != 0x27u)
	{
		delete_current_object();
		return;
	}
	object = LINKED(current_object, OBJECT(current_object)->parent);
	if ((uint8_t)OBJECT(object)->animation_timer != OBJECT(object)->animation_speed || OBJECT(object)->animation_frame != 4u)
	{
		update_screen_position(&ignored, &r1);
		depth = (uint16_t)(PLAYER(PLAYER_BLOCK(1) + ((uint32_t)OBJECT(LINKED(current_object, OBJECT(current_object)->parent))->player << 9))->draw_depth + 5u);
		queue_sprite_at_depth(&ignored, &depth);
		return;
	}
	delete_current_object();
}

void player_state_pulled(uint32_t cursor)
{
	uint8_t sound_id = 0;
	uint16_t new_heading = 0, pull_distance = 0;
	uint32_t r0 = 0, r1 = 0, r2 = 0, r8 = 0, flail_animation = 0, ignored = 0, jump_offset = 0, object = 0;

	for (;;)
	{
		switch (PLAYER(current_player)->substate)
		{
		case 0:
		case 6:
			PLAYER(current_player)->substate++;
			OBJECT(current_object)->sprite_flags &= ~0xFu;
			OBJECT(current_object)->sprite_flags |= 4u;
			OBJECT(current_object)->sprite_offset_x = 0;
			OBJECT(current_object)->sprite_offset_y = 0xA;
			OBJECT(current_object)->flags &= ~0x2000u;
			PLAYER(current_player)->control_lock = 0u;
			OBJECT(current_object)->sprite_palette = mem_read16(OBJECT(current_object)->player_view.palette_list + 2u);
			OBJECT(current_object)->flags &= ~0x40u;
			OBJECT(current_object)->flags &= ~4u;
			OBJECT(current_object)->flags &= ~8u;
			OBJECT(current_object)->flags &= ~0x10u;
			PLAYER(current_player)->pull_angle = 0x300u;
			pull_distance = (uint16_t)((int32_t)(0x5D00000u - OBJECT(current_object)->x) >> 12);
			PLAYER(current_player)->pull_start_distance = pull_distance;
			PLAYER(current_player)->pull_distance_factor = (int16_t)(0x1200u - pull_distance) >> 5;
			sound_id = 0x36u;
			if (OBJECT(current_object)->kind != 0u)
			{
				sound_id = 0x46u;
				if (OBJECT(current_object)->kind != 1u)
					sound_id = 0x56u;
			}
			play_sound(sound_id, &r1, &r2);
			object = LINKED(current_object, scene_object);
			OBJECT(current_object)->x += (int32_t)((uint32_t)OBJECT(object)->x - OBJECT(current_object)->x) >> 3;
			OBJECT(current_object)->z += (int32_t)((uint32_t)OBJECT(object)->z - OBJECT(current_object)->z) >> 3;
			PLAYER(current_player)->pull_angle = OBJECT(object)->direction;
			project_object(&r0, &r1);
			if ((int16_t)PLAYER(current_player)->pull_angle <= 0x1D0)
			{
				PLAYER(current_player)->substate++;
				OBJECT(current_object)->speed = 0;
				OBJECT(current_object)->speed_x = 0xA0000;
				OBJECT(current_object)->speed_y = 0;
				OBJECT(current_object)->speed_z = 0;
			}
			update_player_blink();
			find_floor(&r0, &r1, &r2, &ignored, &cursor);
			ignored = queue_sprite_at_floor();
			return;
		case 1:
			object = LINKED(current_object, scene_object);
			OBJECT(current_object)->x += (int32_t)((uint32_t)OBJECT(object)->x - OBJECT(current_object)->x) >> 3;
			OBJECT(current_object)->z += (int32_t)((uint32_t)OBJECT(object)->z - OBJECT(current_object)->z) >> 3;
			PLAYER(current_player)->pull_angle = OBJECT(object)->direction;
			project_object(&r0, &r1);
			if ((int16_t)PLAYER(current_player)->pull_angle <= 0x1D0)
			{
				PLAYER(current_player)->substate++;
				OBJECT(current_object)->speed = 0;
				OBJECT(current_object)->speed_x = 0xA0000;
				OBJECT(current_object)->speed_y = 0;
				OBJECT(current_object)->speed_z = 0;
			}
			update_player_blink();
			find_floor(&r0, &r1, &r2, &ignored, &cursor);
			ignored = queue_sprite_at_floor();
			return;
		case 2:
			find_floor(&jump_offset, &r1, &r2, &ignored, &cursor);
			if (OBJECT(current_object)->floor_z_int <= OBJECT(current_object)->z_int)
				PLAYER(current_player)->substate++;
			OBJECT(current_object)->speed_z += 0x6000u;
			OBJECT(current_object)->z += OBJECT(current_object)->speed_z;
			PLAYER(current_player)->previous_x = OBJECT(current_object)->x;
			PLAYER(current_player)->previous_y = OBJECT(current_object)->y;
			OBJECT(current_object)->x += OBJECT(current_object)->speed_x;
			OBJECT(current_object)->y += OBJECT(current_object)->speed_y;
			project_object(&ignored, &ignored);
			ignored = queue_sprite_at_floor();
			return;
		case 3:
			PLAYER(current_player)->substate++;
			PLAYER(current_player)->drift_anim_step = 1u;
			PLAYER(current_player)->previous_state = PLAYER(current_player)->state;
			PLAYER(current_player)->drift_turned_back = 0u;
			PLAYER(current_player)->drift_done = 0u;
			OBJECT(current_object)->speed = 0;
			PLAYER(current_player)->ground_speed = 0;
			OBJECT(current_object)->sprite_flags &= 0xFF7Fu;
			OBJECT(current_object)->sprite_flags |= 0x40u;
			OBJECT(current_object)->animation_frame = 0u;
			OBJECT(current_object)->animation_speed = 2u;
			OBJECT(current_object)->animation_timer = 0;
		case 4:
			PLAYER(current_player)->control_lock = 0x7Fu;
			player_move_with_scroll(&r1, &cursor);
			if (PLAYER(current_player)->state != 0x16u)
			{
				if (OBJECT(current_object)->handler != player_respawn_ADDRESS)
				{
					flail_animation = flail_animation_sonic_ADDRESS;
					if (OBJECT(current_object)->kind != 0u)
					{
						flail_animation = flail_animation_ray_ADDRESS;
						if (OBJECT(current_object)->kind != 1u)
							flail_animation = flail_animation_mighty_ADDRESS;
					}
					OBJECT(current_object)->animation = flail_animation;
					animate_with_flags(&r1, &r8);
					if (OBJECT(current_object)->x_int >= 0xCD0)
						PLAYER(current_player)->drift_turned_back = 0xFFu;
				}
				if (PLAYER(current_player)->drift_turned_back != 0u && OBJECT(current_object)->x_int <= 0x630)
				{
					PLAYER(current_player)->substate++;
					PLAYER(current_player)->drift_done = 0xFFu;
					PLAYER(current_player)->control_lock = 0u;
				}
				return;
			}
			jump_offset = PLAYER(current_player)->state * 3u;
			switch (PLAYER(current_player)->state)
			{
			case 0:
				player_move(r1, cursor);
				return;
			case 1:
				player_move(r1, cursor);
				return;
			case 2:
				player_move(r1, cursor);
				return;
			case 3:
				player_state_inactive();
				return;
			case 4:
				player_move(r1, cursor);
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
				player_state_climbing(r1, cursor);
				return;
			case 11:
				player_state_not_playing();
				return;
			case 12:
				player_move(r1, cursor);
				return;
			case 13:
				player_state_hurt(r1, cursor);
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
				player_move(r1, cursor);
				return;
			case 20:
				player_move(r1, cursor);
				return;
			case 21:
				player_state_held(r1, cursor);
				return;
			case 22:
				player_state_dying();
				return;
			case 23:
				player_move(r1, cursor);
				return;
			case 24:
				player_state_stage_scene(r1, cursor);
				return;
			case 25:
				player_state_scene_thrown(r1, cursor);
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
				player_state_intro_turn(r1, cursor);
				return;
			case 31:
				player_state_blown_away(cursor);
				return;
			case 32:
				player_state_tumbling(r1, cursor);
				return;
			case 33:
				player_state_stage_exit(r1, cursor);
				return;
			case 34:
				player_state_in_zone(r1, cursor);
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
				continue;
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
				player_state_entering_stage(r1, cursor);
				return;
			case 43:
				player_state_final_stage();
				return;
			case 44:
				player_state_stuck_on_object(r1, cursor);
				return;
			case 45:
				player_state_held_still();
				return;
			case 46:
				player_state_game_over(jump_offset);
				return;
			case 47:
				player_state_stunned(r1, cursor);
				return;
			case 48:
				player_state_caught(cursor);
				return;
			case 49:
				player_state_carried_off(r1, cursor);
				return;
			case 50:
				player_state_trapped(r1, cursor);
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
				player_state_knocked_off(r1, cursor);
				return;
			case 55:
				player_state_not_playing();
				return;
			case 56:
				player_state_stage_b_sequence(r1, cursor);
				return;
			case 57:
				player_state_you_failed();
				return;
			case 58:
				player_move(r1, cursor);
				return;
			default:
				unknown_code(0x2D3Eu + PLAYER(current_player)->state * 3u);
			}
			break;
		case 5:
			PLAYER(current_player)->heading = 0x2A0u;
			new_heading = (PLAYER(current_player)->heading + 0x200u) & 0x3FFu;
			PLAYER(current_player)->target_heading = new_heading;
			PLAYER(current_player)->facing_angle = new_heading;
			set_heading_direction();
			OBJECT(current_object)->speed = 0x5000000;
			r1 = compute_ground_speed();
			PLAYER(current_player)->substate = 0u;
			PLAYER(current_player)->previous_state = PLAYER(current_player)->state;
			PLAYER(current_player)->state = 0xCu;
			sound_id = 0x48u;
			if (OBJECT(current_object)->kind != 0u)
			{
				sound_id = 0x6Du;
				if (OBJECT(current_object)->kind != 1u)
					sound_id = 0x49u;
			}
			play_sound(sound_id, &r1, &r2);
			player_animate_skidding(r1, cursor);
			return;
		default:
			unknown_code(0x8CD7u + PLAYER(current_player)->substate * 3u);
		}
	}
}

void player_state_floor_8_sequence(uint32_t cursor)
{
	uint8_t drift_byte = 0, sound_id = 0;
	uint16_t x_int = 0, y_int = 0;
	uint32_t r0 = 0, r1 = 0, r2 = 0, drift_animation = 0, drift_direction = 0, drift_speed = 0, drift_x = 0, drift_y = 0, floor_8_escape_animation = 0, floor_8_idle_animation = 0, floor_8_landing_animation = 0, floor_8_ride_animation = 0, floor_block_record = 0, hurt_animations = 0, ignored = 0, input_dy = 0, jump_offset = 0, kind = 0, running_animations = 0, step_record = 0, trackball_speed = 0, trackball_y = 0, z = 0;
	uint16_t state_timer = 0, state_timer_2 = 0, state_timer_3 = 0;

	for (;;)
	{
		switch (PLAYER(current_player)->sequence_step)
		{
		case 0:
		case 17:
			PLAYER(current_player)->sequence_step++;
			PLAYER(current_player)->flags |= 8u;
			PLAYER(current_player)->sequence_bounced = 0u;
			if (scene_step == 2u)
				PLAYER(current_player)->sequence_step++;
			return;
		case 1:
			if (scene_step == 2u)
				PLAYER(current_player)->sequence_step++;
			return;
		case 2:
			PLAYER(current_player)->sequence_step++;
			scroll_mode = 1u;
			OBJECT(current_object)->player_view.hurt_flash_period = 0xFFu;
			OBJECT(current_object)->flags |= 0x1000000u;
			PLAYER(current_player)->flags &= ~8u;
			OBJECT(current_object)->z_int -= 0xA0u;
			OBJECT(current_object)->z_int -= (uint32_t)OBJECT(current_object)->kind << 5;
			OBJECT(current_object)->speed_z = 0;
			OBJECT(current_object)->speed_x = 0;
			OBJECT(current_object)->speed_y = 0;
			OBJECT(current_object)->speed = 0;
			PLAYER(current_player)->ground_speed = 0;
			OBJECT(current_object)->direction = 0x200u;
			face_movement_direction(jump_offset);
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
			sound_id = 0x57u;
			if (OBJECT(current_object)->kind != 0u)
			{
				sound_id = 0x65u;
				if (OBJECT(current_object)->kind != 1u)
					sound_id = 0x67u;
			}
			play_sound(sound_id, &r1, &r2);
			OBJECT(current_object)->speed_z += 0x6000u;
			PLAYER(current_player)->previous_z = OBJECT(current_object)->z;
			OBJECT(current_object)->z += OBJECT(current_object)->speed_z;
			if (OBJECT(current_object)->floor_z_int <= OBJECT(current_object)->z_int)
			{
				PLAYER(current_player)->sequence_step++;
				PLAYER(current_player)->state_timer = 2;
				OBJECT(current_object)->z = OBJECT(current_object)->floor_z;
				OBJECT(current_object)->animation_frame++;
				step_record = OBJECT(current_object)->animation + 2u + OBJECT(current_object)->animation_frame * 4u;
				OBJECT(current_object)->sprite_flags &= ~0xC0u;
				OBJECT(current_object)->sprite_flags |= mem_read16(step_record);
				OBJECT(current_object)->sprite_frame = mem_read16(step_record + 2u);
				project_object(&ignored, &ignored);
				ignored = queue_sprite_at_floor();
				return;
			}
			if (OBJECT(current_object)->animation_frame == 0u && OBJECT(current_object)->speed_z_int >= 0)
			{
				OBJECT(current_object)->animation_frame++;
				step_record = OBJECT(current_object)->animation + 2u + OBJECT(current_object)->animation_frame * 4u;
				OBJECT(current_object)->sprite_flags &= ~0xC0u;
				OBJECT(current_object)->sprite_flags |= mem_read16(step_record);
				OBJECT(current_object)->sprite_frame = mem_read16(step_record + 2u);
			}
			project_object(&ignored, &ignored);
			ignored = queue_sprite_at_floor();
			return;
		case 3:
			OBJECT(current_object)->speed_z += 0x6000u;
			PLAYER(current_player)->previous_z = OBJECT(current_object)->z;
			OBJECT(current_object)->z += OBJECT(current_object)->speed_z;
			if (OBJECT(current_object)->floor_z_int <= OBJECT(current_object)->z_int)
			{
				PLAYER(current_player)->sequence_step++;
				PLAYER(current_player)->state_timer = 2;
				OBJECT(current_object)->z = OBJECT(current_object)->floor_z;
				OBJECT(current_object)->animation_frame++;
				step_record = OBJECT(current_object)->animation + 2u + OBJECT(current_object)->animation_frame * 4u;
				OBJECT(current_object)->sprite_flags &= ~0xC0u;
				OBJECT(current_object)->sprite_flags |= mem_read16(step_record);
				OBJECT(current_object)->sprite_frame = mem_read16(step_record + 2u);
				project_object(&ignored, &ignored);
				ignored = queue_sprite_at_floor();
				return;
			}
			if (OBJECT(current_object)->animation_frame == 0u && OBJECT(current_object)->speed_z_int >= 0)
			{
				OBJECT(current_object)->animation_frame++;
				step_record = OBJECT(current_object)->animation + 2u + OBJECT(current_object)->animation_frame * 4u;
				OBJECT(current_object)->sprite_flags &= ~0xC0u;
				OBJECT(current_object)->sprite_flags |= mem_read16(step_record);
				OBJECT(current_object)->sprite_frame = mem_read16(step_record + 2u);
			}
			project_object(&ignored, &ignored);
			ignored = queue_sprite_at_floor();
			return;
		case 4:
			state_timer = PLAYER(current_player)->state_timer;
			PLAYER(current_player)->state_timer = state_timer - 1u;
			if ((int16_t)state_timer <= 1)
			{
				PLAYER(current_player)->sequence_step++;
				OBJECT(current_object)->speed_z = -0x40000;
				OBJECT(current_object)->animation_timer = 0;
				OBJECT(current_object)->animation_speed = 3u;
				OBJECT(current_object)->speed_x = 0;
				OBJECT(current_object)->speed_y = 0x40000;
				scroll_mode = 0u;
				AS8(scroll_speed) = 0x10u;
			}
			find_floor(&jump_offset, &r1, &r2, &ignored, &cursor);
			project_object(&ignored, &ignored);
			ignored = queue_sprite_at_base(ignored);
			return;
		case 5:
			OBJECT(current_object)->speed_z += 0x6000u;
			PLAYER(current_player)->previous_z = OBJECT(current_object)->z;
			OBJECT(current_object)->z += OBJECT(current_object)->speed_z;
			PLAYER(current_player)->previous_x = OBJECT(current_object)->x;
			PLAYER(current_player)->previous_y = OBJECT(current_object)->y;
			OBJECT(current_object)->x += OBJECT(current_object)->speed_x;
			OBJECT(current_object)->y += OBJECT(current_object)->speed_y;
			if (OBJECT(current_object)->floor_z_int <= OBJECT(current_object)->z_int)
			{
				PLAYER(current_player)->sequence_step++;
				OBJECT(current_object)->z = OBJECT(current_object)->floor_z;
			}
			if ((int8_t)OBJECT(current_object)->animation_speed >= 0)
			{
				animate_with_flags(&r1, &z);
				if (OBJECT(current_object)->animation_frame == 3u)
					OBJECT(current_object)->animation_speed = 0xFFu;
			}
			find_floor(&jump_offset, &r1, &r2, &ignored, &cursor);
			project_object(&ignored, &ignored);
			ignored = queue_sprite_at_floor();
			return;
		case 6:
			PLAYER(current_player)->sequence_step++;
			PLAYER(current_player)->drift_anim_step = 1u;
			PLAYER(current_player)->previous_state = PLAYER(current_player)->state;
			OBJECT(current_object)->sprite_flags &= 0xFF7Fu;
			OBJECT(current_object)->sprite_flags |= 0x40u;
			OBJECT(current_object)->animation_frame = 0u;
			OBJECT(current_object)->animation_speed = 4u;
			OBJECT(current_object)->animation_timer = 0;
		case 7:
			PLAYER(current_player)->control_lock = 0x10u;
			player_move_with_scroll(&r1, &cursor);
			if (PLAYER(current_player)->state != 0x16u)
			{
				if (OBJECT(current_object)->handler != player_respawn_ADDRESS && OBJECT(current_object)->y_int >= 0x4F2)
				{
					PLAYER(current_player)->sequence_step++;
					OBJECT(current_object)->speed_z = 0;
				}
				return;
			}
		player_dispatch_state:
			jump_offset = PLAYER(current_player)->state * 3u;
			switch (PLAYER(current_player)->state)
			{
			case 0:
				player_move(r1, cursor);
				return;
			case 1:
				player_move(r1, cursor);
				return;
			case 2:
				player_move(r1, cursor);
				return;
			case 3:
				player_state_inactive();
				return;
			case 4:
				player_move(r1, cursor);
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
				player_state_climbing(r1, cursor);
				return;
			case 11:
				player_state_not_playing();
				return;
			case 12:
				player_move(r1, cursor);
				return;
			case 13:
				player_state_hurt(r1, cursor);
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
				player_move(r1, cursor);
				return;
			case 20:
				player_move(r1, cursor);
				return;
			case 21:
				player_state_held(r1, cursor);
				return;
			case 22:
				player_state_dying();
				return;
			case 23:
				player_move(r1, cursor);
				return;
			case 24:
				player_state_stage_scene(r1, cursor);
				return;
			case 25:
				player_state_scene_thrown(r1, cursor);
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
				player_state_intro_turn(r1, cursor);
				return;
			case 31:
				player_state_blown_away(cursor);
				return;
			case 32:
				player_state_tumbling(r1, cursor);
				return;
			case 33:
				player_state_stage_exit(r1, cursor);
				return;
			case 34:
				player_state_in_zone(r1, cursor);
				return;
			case 35:
				player_state_continue();
				return;
			case 36:
				player_state_out_of_game();
				return;
			case 37:
				continue;
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
				player_state_entering_stage(r1, cursor);
				return;
			case 43:
				player_state_final_stage();
				return;
			case 44:
				player_state_stuck_on_object(r1, cursor);
				return;
			case 45:
				player_state_held_still();
				return;
			case 46:
				player_state_game_over(jump_offset);
				return;
			case 47:
				player_state_stunned(r1, cursor);
				return;
			case 48:
				player_state_caught(cursor);
				return;
			case 49:
				player_state_carried_off(r1, cursor);
				return;
			case 50:
				player_state_trapped(r1, cursor);
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
				player_state_knocked_off(r1, cursor);
				return;
			case 55:
				player_state_not_playing();
				return;
			case 56:
				player_state_stage_b_sequence(r1, cursor);
				return;
			case 57:
				player_state_you_failed();
				return;
			case 58:
				player_move(r1, cursor);
				return;
			default:
				unknown_code(0x2D3Eu + PLAYER(current_player)->state * 3u);
			}
			break;
		case 8:
			OBJECT(current_object)->speed_z += 0x6000u;
			PLAYER(current_player)->previous_z = OBJECT(current_object)->z;
			OBJECT(current_object)->z += OBJECT(current_object)->speed_z;
			floor_block_record = floor_blocks + 2u + OBJECT(current_object)->floor_block;
			drift_byte = 0u;
			compute_scroll_drift(floor_block_record, drift_byte);
			drift_direction = scroll_drift_direction;
			drift_speed = scroll_drift_speed >> 14;
			drift_x = (int32_t)(sine_table[drift_direction] * drift_speed) >> 8;
			PLAYER(current_player)->previous_x = OBJECT(current_object)->x;
			OBJECT(current_object)->x += drift_x;
			drift_y = (int32_t)(cosine_table[drift_direction] * drift_speed) >> 8;
			PLAYER(current_player)->previous_y = OBJECT(current_object)->y;
			OBJECT(current_object)->y -= drift_y;
			find_floor(&drift_direction, &drift_y, &drift_speed, &ignored, &cursor);
			OBJECT(current_object)->floor_z_int += 0x20u;
			if (OBJECT(current_object)->floor_z_int <= OBJECT(current_object)->z_int)
				PLAYER(current_player)->sequence_step++;
			animate(&drift_y, &kind, &floor_block_record);
			project_object(&ignored, &ignored);
			ignored = queue_sprite_at_floor();
			return;
		case 9:
			PLAYER(current_player)->sequence_step++;
			PLAYER(current_player)->control_lock = 0u;
			PLAYER(current_player)->previous_state = PLAYER(current_player)->state;
			OBJECT(current_object)->sprite_flags &= 0xFFBFu;
			PLAYER(current_player)->drift_anim_step = 1u;
			OBJECT(current_object)->sprite_palette = (OBJECT(current_object)->sprite_palette & 0x1FFFu) | 0x2000u;
			floor_8_ride_animation = sonic_floor_8_ride_animation_ADDRESS;
			if (OBJECT(current_object)->kind != 0u)
			{
				floor_8_ride_animation = ray_floor_8_ride_animation_ADDRESS;
				if (OBJECT(current_object)->kind != 1u)
					floor_8_ride_animation = mighty_floor_8_ride_animation_ADDRESS;
			}
			OBJECT(current_object)->animation = floor_8_ride_animation;
			OBJECT(current_object)->animation_frame = 0u;
			OBJECT(current_object)->animation_speed = 4u;
			OBJECT(current_object)->animation_timer = 0;
			cursor = spawn_player_effect(cursor);
		case 10:
			player_move_with_scroll(&r1, &cursor);
			animate(&r1, &kind, &z);
			if (PLAYER(current_player)->state == 0x16u)
				goto player_dispatch_state;
			if (OBJECT(current_object)->handler == player_respawn_ADDRESS)
				return;
			if (scroll_drift_speed == 0u)
			{
				PLAYER(current_player)->previous_y = OBJECT(current_object)->y;
				OBJECT(current_object)->y += 0x40000u;
			}
			if (OBJECT(current_object)->y_int >= 0x800)
			{
				PLAYER(current_player)->control_lock = 0x7Fu;
				OBJECT(current_object)->speed = 0;
				PLAYER(current_player)->ground_speed = 0;
			}
			if (OBJECT(current_object)->y_int < 0x9B0)
				return;
			PLAYER(current_player)->sequence_step++;
			PLAYER(current_player)->control_lock = 0u;
			PLAYER(current_player)->state_timer = 0x258;
			OBJECT(current_object)->speed_z = -0x20000;
			OBJECT(current_object)->speed_x = 0;
			OBJECT(current_object)->speed_y = 0x40000;
			drift_animation = sonic_drift_animation_ADDRESS;
			if (OBJECT(current_object)->kind != 0u)
			{
				drift_animation = ray_drift_animation_ADDRESS;
				if (OBJECT(current_object)->kind != 1u)
					drift_animation = mighty_drift_animation_ADDRESS;
			}
			OBJECT(current_object)->animation = drift_animation;
			OBJECT(current_object)->sprite_flags |= 0x40u;
			spawn_player_effect(cursor);
			return;
		case 11:
			OBJECT(current_object)->speed_z += 0x2000u;
			PLAYER(current_player)->previous_z = OBJECT(current_object)->z;
			OBJECT(current_object)->z += OBJECT(current_object)->speed_z;
			PLAYER(current_player)->previous_x = OBJECT(current_object)->x;
			PLAYER(current_player)->previous_y = OBJECT(current_object)->y;
			OBJECT(current_object)->x += OBJECT(current_object)->speed_x;
			OBJECT(current_object)->y += OBJECT(current_object)->speed_y;
			if (OBJECT(current_object)->y_int < 0xA1C)
			{
				animate(&r1, &kind, &z);
				project_object(&ignored, &ignored);
				ignored = queue_sprite_at_floor();
				return;
			}
			PLAYER(current_player)->sequence_step++;
			AS16(PLAYER(current_player)->escape_progress) = 0u;
			OBJECT(current_object)->animation_frame = 0xFFu;
			OBJECT(current_object)->flags &= ~0x80u;
			OBJECT(current_object)->sprite_flags &= 0xFFBFu;
			OBJECT(current_object)->speed_z = 0;
			cursor = spawn_struggle_marker(cursor);
			x_int = (uint16_t)OBJECT(current_object)->x_int;
			y_int = (uint16_t)OBJECT(current_object)->y_int;
			kind = 0xCu;
			z = (uint16_t)(OBJECT(current_object)->z_int - 0x28u);
			spawn_sign_2(x_int, y_int, (uint16_t)kind, (uint16_t)z, cursor);
			sound_id = 0x38u;
			if (OBJECT(current_object)->kind != 0u)
			{
				sound_id = 0x3Cu;
				if (OBJECT(current_object)->kind != 1u)
					sound_id = 0x44u;
			}
			play_sound(sound_id, &r1, &r2);
			animate(&r1, &kind, &z);
			project_object(&ignored, &ignored);
			ignored = queue_sprite_at_floor();
			return;
		case 12:
			goto player_floor_8_sequence_struggle;
		case 13:
			OBJECT(current_object)->speed_z += 0x6000u;
			PLAYER(current_player)->previous_z = OBJECT(current_object)->z;
			OBJECT(current_object)->z += OBJECT(current_object)->speed_z;
			PLAYER(current_player)->previous_x = OBJECT(current_object)->x;
			PLAYER(current_player)->previous_y = OBJECT(current_object)->y;
			OBJECT(current_object)->x += OBJECT(current_object)->speed_x;
			OBJECT(current_object)->y += OBJECT(current_object)->speed_y;
			running_animations = sonic_running_animations_ADDRESS;
			if (OBJECT(current_object)->kind != 0u)
			{
				running_animations = ray_running_animations_ADDRESS;
				if (OBJECT(current_object)->kind != 1u)
					running_animations = mighty_running_animations_ADDRESS;
			}
			OBJECT(current_object)->animation = running_animations + ROM16(running_animations + PLAYER(current_player)->facing * 2u);
			animate_with_flags(&r1, &z);
			if ((OBJECT(current_object)->sprite_palette & 0x8000u) == 0u && OBJECT(current_object)->speed_z_int >= 0)
				OBJECT(current_object)->sprite_palette = (OBJECT(current_object)->sprite_palette & 0x1FFFu) | 0x8000u;
			if (OBJECT(current_object)->speed_z_int >= 0 && OBJECT(current_object)->floor_z <= OBJECT(current_object)->z)
			{
				PLAYER(current_player)->sequence_step = 0xFu;
				OBJECT(current_object)->z = OBJECT(current_object)->floor_z;
				PLAYER(current_player)->sequence_bounced = 0xFFu;
				OBJECT(current_object)->speed_z = 0u - OBJECT(current_object)->speed_z;
				OBJECT(current_object)->speed_z = (int32_t)OBJECT(current_object)->speed_z >> 1;
				OBJECT(current_object)->speed_x = (int32_t)OBJECT(current_object)->speed_x >> 1;
				OBJECT(current_object)->speed_y = (int32_t)OBJECT(current_object)->speed_y >> 1;
				PLAYER(current_player)->sequence_step = 0x10u;
			}
			project_object(&ignored, &ignored);
			ignored = queue_sprite_at_floor();
			return;
		case 14:
			OBJECT(current_object)->speed_z += 0x6000u;
			OBJECT(current_object)->z += OBJECT(current_object)->speed_z;
			update_screen_position(&ignored, &r1);
			keep_player_on_screen(r1);
			ignored = queue_sprite_at_floor();
			return;
		case 15:
			animate(&r1, &kind, &z);
			state_timer_2 = PLAYER(current_player)->state_timer;
			PLAYER(current_player)->state_timer = state_timer_2 - 1u;
			if ((int16_t)state_timer_2 <= 1)
			{
				PLAYER(current_player)->state = 1u;
				PLAYER(current_player)->substate = 0u;
				OBJECT(current_object)->speed = 0;
			}
			project_object(&ignored, &ignored);
			ignored = queue_sprite_at_floor();
			return;
		case 16:
			OBJECT(current_object)->speed_z += 0x6000u;
			PLAYER(current_player)->previous_z = OBJECT(current_object)->z;
			OBJECT(current_object)->z += OBJECT(current_object)->speed_z;
			PLAYER(current_player)->previous_x = OBJECT(current_object)->x;
			PLAYER(current_player)->previous_y = OBJECT(current_object)->y;
			OBJECT(current_object)->x += OBJECT(current_object)->speed_x;
			OBJECT(current_object)->y += OBJECT(current_object)->speed_y;
			animate_with_flags(&r1, &z);
			if (OBJECT(current_object)->speed_z_int < 0 || OBJECT(current_object)->z < OBJECT(current_object)->floor_z)
			{
				project_object(&ignored, &ignored);
				ignored = queue_sprite_at_floor();
				return;
			}
			PLAYER(current_player)->sequence_step = 0xFu;
			OBJECT(current_object)->z = OBJECT(current_object)->floor_z;
			OBJECT(current_object)->speed_x = 0;
			OBJECT(current_object)->speed_y = 0;
			OBJECT(current_object)->speed_z = 0;
			floor_8_landing_animation = sonic_floor_8_landing_animation_ADDRESS;
			if (OBJECT(current_object)->kind != 0u)
			{
				floor_8_landing_animation = ray_floor_8_landing_animation_ADDRESS;
				if (OBJECT(current_object)->kind != 1u)
					floor_8_landing_animation = mighty_floor_8_landing_animation_ADDRESS;
			}
			OBJECT(current_object)->animation = floor_8_landing_animation;
			OBJECT(current_object)->animation_speed = 2u;
			OBJECT(current_object)->animation_frame = 0xFFu;
			OBJECT(current_object)->animation_timer = 0;
			PLAYER(current_player)->state_timer = 0x3C;
			OBJECT(current_object)->sprite_flags &= 0xFFBFu;
			project_object(&ignored, &ignored);
			ignored = queue_sprite_at_floor();
			return;
		default:
			unknown_code(0x8F3Fu + PLAYER(current_player)->sequence_step * 3u);
		}
	}
player_floor_8_sequence_struggle:
	read_player_trackball();
	trackball_speed = PLAYER(current_player)->trackball_x;
	trackball_y = PLAYER(current_player)->trackball_y;
	trackball_speed = trackball_speed * trackball_speed + trackball_y * trackball_y;
	square_root(&trackball_speed, &ignored);
	trackball_speed &= 0xFFFFu;
	trackball_speed = trackball_speed * trackball_speed;
	if ((int32_t)trackball_speed > 0x5000000)
		trackball_speed = 0x5000000u;
	OBJECT(current_object)->speed = trackball_speed;
	r0 = (uint16_t)PLAYER(current_player)->trackball_x;
	input_dy = (uint16_t)PLAYER(current_player)->trackball_y;
	angle_of_vector(input_dy, &r0, &ignored);
	PLAYER(current_player)->target_heading = r0;
	if ((uint16_t)OBJECT(current_object)->speed_int == 0u)
	{
	player_floor_8_sequence_reset_escape:
		AS16(PLAYER(current_player)->escape_progress) = 0u;
	}
	else
	{
		r0 = (r0 & ~0xFFFFu) | PLAYER(current_player)->target_heading;
		r0 = (r0 & ~0xFFFFu) | (uint16_t)(r0 - 0x280u);
		r0 = (r0 & ~0xFFFFu) | (r0 & 0x3FFu);
		if ((uint16_t)r0 < 0x101u)
			goto player_floor_8_sequence_reset_escape;
		r0 = (r0 & ~0xFFFFu) | (uint16_t)OBJECT(current_object)->speed_int;
		r0 = (r0 & ~0xFFFFu) | ((uint16_t)r0 >> 4);
		AS16(PLAYER(current_player)->escape_progress) += r0;
	}
	if (AS16(PLAYER(current_player)->escape_progress) != 0u)
	{
		floor_8_escape_animation = floor_8_escape_animation_sonic_ADDRESS;
		if (OBJECT(current_object)->kind != 0u)
		{
			floor_8_escape_animation = floor_8_escape_animation_ray_ADDRESS;
			if (OBJECT(current_object)->kind != 1u)
				floor_8_escape_animation = floor_8_escape_animation_mighty_ADDRESS;
		}
		OBJECT(current_object)->animation = floor_8_escape_animation;
		r0 = (r0 & ~0xFFFFu) | AS16(PLAYER(current_player)->escape_progress);
		r0 = (r0 & ~0xFFFFu) | ((uint16_t)r0 >> 8);
		if ((int16_t)ROM16(floor_8_escape_animation) < (int16_t)r0)
			r0 = (r0 & ~0xFFFFu) | ROM16(floor_8_escape_animation);
		OBJECT(current_object)->sprite_frame = mem_read16(floor_8_escape_animation + 2u + r0 * 2u);
	}
	else
	{
		floor_8_idle_animation = floor_8_idle_animation_sonic_ADDRESS;
		if (OBJECT(current_object)->kind != 0u)
		{
			floor_8_idle_animation = floor_8_idle_animation_ray_ADDRESS;
			if (OBJECT(current_object)->kind != 1u)
				floor_8_idle_animation = floor_8_idle_animation_mighty_ADDRESS;
		}
		OBJECT(current_object)->animation = floor_8_idle_animation;
		animate(&input_dy, &kind, &z);
	}
	state_timer_3 = PLAYER(current_player)->state_timer;
	PLAYER(current_player)->state_timer = state_timer_3 - 1u;
	if ((int16_t)state_timer_3 <= 1)
		PLAYER(current_player)->sequence_step = 0xEu;
	if ((int16_t)AS16(PLAYER(current_player)->escape_progress) >= 0x400)
	{
		PLAYER(current_player)->sequence_step++;
		OBJECT(current_object)->speed_z = -0x80000;
		OBJECT(current_object)->speed_y = 0x20000;
		OBJECT(current_object)->speed_x = 0;
		OBJECT(current_object)->animation_speed = 2u;
		OBJECT(current_object)->animation_timer = 0;
		OBJECT(current_object)->animation_frame = 0u;
		OBJECT(current_object)->direction = 0x200u;
		face_movement_direction(r0);
	}
	update_screen_position(&ignored, &input_dy);
	ignored = queue_sprite_at_floor();
}

void player_move_with_scroll(uint32_t *p_depth, uint32_t *p_cursor)
{
	uint32_t cursor = *p_cursor;
	uint8_t drift_byte = 0;
	uint16_t heading_16 = 0, nudge_x = 0;
	uint32_t r0 = 0, r7 = 0, r8 = 0, animation = 0, depth = 0, drift_angle = 0, drift_dx = 0, drift_speed = 0, ignored = 0;

	if (PLAYER(current_player)->hit_stop != 0u)
		PLAYER(current_player)->hit_stop--;
	if (PLAYER(current_player)->control_lock != 0u)
		PLAYER(current_player)->control_lock--;
	push_player_out_of_wall(&r0, &depth, &cursor);
	PLAYER(current_player)->state = PLAYER(current_player)->previous_state;
	PLAYER(current_player)->push_x = 0;
	PLAYER(current_player)->push_y = 0;
	find_floor(&r0, &depth, &drift_speed, &ignored, &cursor);
	OBJECT(current_object)->z = OBJECT(current_object)->floor_z;
	if (PLAYER(current_player)->state == 0x25u && PLAYER(current_player)->sequence_step == 0xAu)
		OBJECT(current_object)->z_int += 0x20u;
	project_object(&r0, &ignored);
	if ((uint8_t)PLAYER(current_player)->pushed_out == 0u && PLAYER(current_player)->control_lock == 0u)
		player_steer(r0);
	set_speed_from_player_speed();
	PLAYER(current_player)->state = PLAYER(current_player)->previous_state;
	if ((OBJECT(current_object)->flags & 0x20u) == 0u)
	{
		r8 = floor_blocks + 2u + OBJECT(current_object)->floor_block;
		drift_byte = 0u;
		compute_scroll_drift(r8, drift_byte);
	}
	if (PLAYER(current_player)->state == 0x26u && PLAYER(current_player)->drift_turned_back != 0u)
		scroll_drift_direction = 0x300u;
	drift_angle = scroll_drift_direction;
	drift_speed = scroll_drift_speed >> 14;
	drift_dx = (int32_t)(sine_table[drift_angle] * drift_speed) >> 8;
	PLAYER(current_player)->previous_x = OBJECT(current_object)->x;
	OBJECT(current_object)->x += drift_dx;
	depth = (int32_t)(cosine_table[drift_angle] * drift_speed) >> 8;
	PLAYER(current_player)->previous_y = OBJECT(current_object)->y;
	OBJECT(current_object)->y -= depth;
	if (PLAYER(current_player)->state == 0x26u && scroll_drift_speed == 0u)
	{
		PLAYER(current_player)->control_lock = 0x3Cu;
		nudge_x = 0xAu;
		depth = (depth & ~0xFFFFu) | 0x100u;
		if (PLAYER(current_player)->drift_turned_back != 0u)
		{
			nudge_x = (uint16_t)(0u - nudge_x);
			depth = (depth & ~0xFFFFu) | 0x300u;
		}
		OBJECT(current_object)->x_int += nudge_x;
		OBJECT(current_object)->direction = depth;
	}
	if (PLAYER(current_player)->hit_stop == 0u)
	{
		OBJECT(current_object)->x += OBJECT(current_object)->speed_x;
		OBJECT(current_object)->y += OBJECT(current_object)->speed_y;
	}
	depth = keep_player_on_screen(depth);
	if (PLAYER(current_player)->state == 0x16u || OBJECT(current_object)->handler == player_respawn_ADDRESS)
	{
		*p_depth = depth;
		*p_cursor = cursor;
		return;
	}
	add_player_to_lists();
	if ((PLAYER(current_player)->state != 0x25u || PLAYER(current_player)->sequence_step != 0xAu) && PLAYER(current_player)->state != 0x26u)
	{
		switch (PLAYER(current_player)->drift_anim_step)
		{
		case 0:
		case 1:
		case 7:
			if (OBJECT(current_object)->speed_int <= 0x80)
			{
				OBJECT(current_object)->flags &= ~0x80u;
				animation = sonic_drift_animation_ADDRESS;
				if (OBJECT(current_object)->kind != 0u)
				{
					animation = ray_drift_animation_ADDRESS;
					if (OBJECT(current_object)->kind != 1u)
						animation = mighty_drift_animation_ADDRESS;
				}
				OBJECT(current_object)->animation = animation;
				animate(&depth, &r7, &r8);
			}
			else
			{
				heading_16 = (uint16_t)((uint32_t)OBJECT(current_object)->direction << 6);
				depth = (depth & ~0xFFFFu) | heading_16;
				if ((uint16_t)(heading_16 + 0x7000u) >= 0x6040u)
				{
					depth = (depth & ~0xFFFFu) | (uint16_t)(depth - 0x1000u);
					if ((uint16_t)depth >= 0x6040u)
					{
						OBJECT(current_object)->flags &= ~0x80u;
						animation = sonic_drift_animation_ADDRESS;
						if (OBJECT(current_object)->kind != 0u)
						{
							animation = ray_drift_animation_ADDRESS;
							if (OBJECT(current_object)->kind != 1u)
								animation = mighty_drift_animation_ADDRESS;
						}
						OBJECT(current_object)->animation = animation;
						animate(&depth, &r7, &r8);
						if (PLAYER(current_player)->state != 0x25u || PLAYER(current_player)->sequence_step != 0xAu)
						{
							depth = queue_sprite(depth);
							*p_depth = depth;
							*p_cursor = cursor;
							return;
						}
						depth = (depth & ~0xFFFFu) | (uint16_t)OBJECT(current_object)->screen_y;
						depth = (depth & ~0xFFFFu) | (uint16_t)(depth - 0x20u);
						queue_sprite_at_depth(&ignored, &depth);
						*p_depth = depth;
						*p_cursor = cursor;
						return;
					}
					PLAYER(current_player)->drift_anim_step = 2u;
					OBJECT(current_object)->animation = drift_plus_x_start_animation_ADDRESS;
					OBJECT(current_object)->animation_frame = 0xFFu;
					OBJECT(current_object)->animation_timer = 0;
					OBJECT(current_object)->flags &= ~0x80u;
					animate(&depth, &r7, &r8);
					if ((uint8_t)r8 != 0u)
						PLAYER(current_player)->drift_anim_step++;
					if (PLAYER(current_player)->state != 0x25u || PLAYER(current_player)->sequence_step != 0xAu)
					{
						depth = queue_sprite(depth);
						*p_depth = depth;
						*p_cursor = cursor;
						return;
					}
					depth = (depth & ~0xFFFFu) | (uint16_t)OBJECT(current_object)->screen_y;
					depth = (depth & ~0xFFFFu) | (uint16_t)(depth - 0x20u);
					queue_sprite_at_depth(&ignored, &depth);
					*p_depth = depth;
					*p_cursor = cursor;
					return;
				}
				PLAYER(current_player)->drift_anim_step = 4u;
				OBJECT(current_object)->animation = drift_minus_x_start_animation_ADDRESS;
				OBJECT(current_object)->animation_frame = 0xFFu;
				OBJECT(current_object)->animation_timer = 0;
				OBJECT(current_object)->flags &= ~0x80u;
				animate(&depth, &r7, &r8);
				if ((uint8_t)r8 != 0u)
					PLAYER(current_player)->drift_anim_step++;
			}
			break;
		case 2:
		case 4:
			animate(&depth, &r7, &r8);
			if ((uint8_t)r8 != 0u)
				PLAYER(current_player)->drift_anim_step++;
			if (PLAYER(current_player)->state != 0x25u || PLAYER(current_player)->sequence_step != 0xAu)
			{
				depth = queue_sprite(depth);
				*p_depth = depth;
				*p_cursor = cursor;
				return;
			}
			depth = (depth & ~0xFFFFu) | (uint16_t)OBJECT(current_object)->screen_y;
			depth = (depth & ~0xFFFFu) | (uint16_t)(depth - 0x20u);
			queue_sprite_at_depth(&ignored, &depth);
			*p_depth = depth;
			*p_cursor = cursor;
			return;
		case 3:
			if (OBJECT(current_object)->speed_int <= 0x80 || (uint16_t)(((uint32_t)OBJECT(current_object)->direction << 6) - 0x1000u) >= 0x6040u)
			{
				PLAYER(current_player)->drift_anim_step = 6u;
				OBJECT(current_object)->animation = drift_plus_x_start_animation_ADDRESS;
				OBJECT(current_object)->animation_frame = 3u;
				OBJECT(current_object)->animation_timer = 0;
				OBJECT(current_object)->flags |= 0x80u;
				animate(&depth, &r7, &r8);
				if ((uint8_t)r8 != 0u)
				{
					OBJECT(current_object)->animation_timer = 0;
					PLAYER(current_player)->drift_anim_step = 1u;
				}
			}
			else
			{
				OBJECT(current_object)->flags &= ~0x80u;
				OBJECT(current_object)->animation = drift_plus_x_animation_ADDRESS;
				animate(&depth, &r7, &r8);
			}
			break;
		case 5:
			if (OBJECT(current_object)->speed_int <= 0x80 || (uint16_t)(((uint32_t)OBJECT(current_object)->direction << 6) + 0x7000u) >= 0x6040u)
			{
				PLAYER(current_player)->drift_anim_step = 6u;
				OBJECT(current_object)->animation = drift_minus_x_start_animation_ADDRESS;
				OBJECT(current_object)->animation_frame = 3u;
				OBJECT(current_object)->animation_timer = 0;
				OBJECT(current_object)->flags |= 0x80u;
				animate(&depth, &r7, &r8);
				if ((uint8_t)r8 != 0u)
				{
					OBJECT(current_object)->animation_timer = 0;
					PLAYER(current_player)->drift_anim_step = 1u;
				}
				if (PLAYER(current_player)->state != 0x25u || PLAYER(current_player)->sequence_step != 0xAu)
				{
					depth = queue_sprite(depth);
					*p_depth = depth;
					*p_cursor = cursor;
					return;
				}
				depth = (depth & ~0xFFFFu) | (uint16_t)OBJECT(current_object)->screen_y;
				depth = (depth & ~0xFFFFu) | (uint16_t)(depth - 0x20u);
				queue_sprite_at_depth(&ignored, &depth);
				*p_depth = depth;
				*p_cursor = cursor;
				return;
			}
			OBJECT(current_object)->flags &= ~0x80u;
			OBJECT(current_object)->animation = drift_minus_x_animation_ADDRESS;
			animate(&depth, &r7, &r8);
			break;
		case 6:
			animate(&depth, &r7, &r8);
			if ((uint8_t)r8 != 0u)
			{
				OBJECT(current_object)->animation_timer = 0;
				PLAYER(current_player)->drift_anim_step = 1u;
			}
			if (PLAYER(current_player)->state != 0x25u || PLAYER(current_player)->sequence_step != 0xAu)
			{
				depth = queue_sprite(depth);
				*p_depth = depth;
				*p_cursor = cursor;
				return;
			}
			depth = (depth & ~0xFFFFu) | (uint16_t)OBJECT(current_object)->screen_y;
			depth = (depth & ~0xFFFFu) | (uint16_t)(depth - 0x20u);
			queue_sprite_at_depth(&ignored, &depth);
			*p_depth = depth;
			*p_cursor = cursor;
			return;
		default:
			unknown_code(0x984Du + PLAYER(current_player)->drift_anim_step * 3u);
		}
	}
	if (PLAYER(current_player)->state != 0x25u || PLAYER(current_player)->sequence_step != 0xAu)
	{
		depth = queue_sprite(depth);
		*p_depth = depth;
		*p_cursor = cursor;
		return;
	}
	depth = (depth & ~0xFFFFu) | (uint16_t)OBJECT(current_object)->screen_y;
	depth = (depth & ~0xFFFFu) | (uint16_t)(depth - 0x20u);
	queue_sprite_at_depth(&ignored, &depth);
	*p_depth = depth;
	*p_cursor = cursor;
}

uint32_t spawn_player_effect(uint32_t effect)
{
	uint32_t free_count = 0;

	allocate_object(&free_count, &effect);
	if (free_count != 0u)
	{
		OBJECT(effect)->sprite_flags = 0x1004u;
		OBJECT(effect)->sprite_palette = 0xD190u;
		OBJECT(effect)->handler = player_effect_start_ADDRESS;
		OBJECT(effect)->flags = 0u;
		OBJECT(effect)->sprite_offset_x = 0;
		OBJECT(effect)->sprite_offset_y = 0;
		OBJECT(effect)->player_effect.depth_offset = 0x28;
		OBJECT(effect)->x = OBJECT(current_object)->x;
		OBJECT(effect)->x_int += 0x20u;
		OBJECT(effect)->y = OBJECT(current_object)->y;
		OBJECT(effect)->y_int -= 0x10u;
		OBJECT(effect)->z = OBJECT(current_object)->z;
		OBJECT(effect)->z_int += 0x10u;
		OBJECT(effect)->animation = 0x9BF7u;
		OBJECT(effect)->animation_frame = 0xFFu;
		OBJECT(effect)->animation_speed = 4u;
		OBJECT(effect)->animation_timer = 0;
	}
	return effect;
}

void player_effect_start(void)
{
	uint32_t ignored = 0;

	project_object(&ignored, &ignored);
	OBJECT(current_object)->handler = player_effect_update_ADDRESS;
	player_effect_update();
}

void player_effect_update(void)
{
	uint32_t r1 = 0, r7 = 0, at_last_step = 0, depth = 0, ignored = 0;

	animate(&r1, &r7, &at_last_step);
	if ((uint8_t)at_last_step == 0u)
	{
		update_screen_position(&ignored, &r1);
		depth = (uint16_t)(OBJECT(current_object)->screen_y + OBJECT(current_object)->player_effect.depth_offset);
		queue_sprite_at_depth(&ignored, &depth);
		return;
	}
	delete_current_object();
}

uint32_t spawn_struggle_marker(uint32_t marker)
{
	uint32_t free_count = 0;

	allocate_object(&free_count, &marker);
	if (free_count != 0u)
	{
		OBJECT(marker)->sprite_flags = 0x1004u;
		OBJECT(marker)->sprite_frame = 0x77A0u;
		OBJECT(marker)->sprite_palette = OBJECT(current_object)->sprite_palette;
		OBJECT(marker)->sprite_palette &= 0x1FFFu;
		OBJECT(marker)->sprite_palette |= 0x8000u;
		OBJECT(marker)->handler = struggle_marker_update_ADDRESS;
		OBJECT(marker)->flags = 0u;
		OBJECT(marker)->sprite_offset_x = 2;
		OBJECT(marker)->sprite_offset_y = -0xF;
		OBJECT(marker)->parent = current_object;
	}
	return marker;
}

void struggle_marker_update(void)
{
	uint32_t r1 = 0, depth = 0, ignored = 0, object = 0, player = 0;

	object = LINKED(current_object, OBJECT(current_object)->parent);
	player = PLAYER_BLOCK(1);
	if (OBJECT(object)->player != 0u)
	{
		player = PLAYER_BLOCK(2);
		if (OBJECT(object)->player != 1u)
			player = PLAYER_BLOCK(3);
	}
	if (PLAYER(player)->state != 0x25u || PLAYER(player)->sequence_step != 0xCu)
	{
		delete_current_object();
		return;
	}
	OBJECT(current_object)->projected_x = OBJECT(object)->projected_x;
	OBJECT(current_object)->projected_y = OBJECT(object)->projected_y;
	OBJECT(current_object)->z = OBJECT(object)->z;
	update_screen_position(&ignored, &r1);
	object = LINKED(current_object, OBJECT(current_object)->parent);
	depth = (uint16_t)(((int32_t)((uint32_t)OBJECT(object)->projected_y + OBJECT(object)->floor_z) >> 16) - view_y + 1u);
	queue_sprite_at_depth(&ignored, &depth);
}

void scene_object_start(void)
{
	OBJECT(current_object)->x = 0x4D70000;
	OBJECT(current_object)->y = 0x145B0000;
	OBJECT(current_object)->z = 0x780000;
	OBJECT(current_object)->scene_object_view.end_position = 0x5C00u;
	OBJECT(current_object)->scene_object_view.angle = 0xF90u;
	OBJECT(current_object)->direction = 0x300u;
	OBJECT(current_object)->handler = scene_object_update_ADDRESS;
	scene_object = current_object;
}

void scene_object_update(void)
{
	uint32_t direction = 0, radius = 0;

	if (scroll_position < 0x4FF0u)
		return;
	if (OBJECT(current_object)->scene_object_view.end_position <= scroll_position)
	{
		delete_current_object();
		return;
	}
	OBJECT(current_object)->scene_object_view.angle += 4u;
	OBJECT(current_object)->direction -= 0xFu;
	OBJECT(current_object)->direction &= 0x3FFu;
	direction = OBJECT(current_object)->direction;
	radius = (int16_t)OBJECT(current_object)->scene_object_view.angle;
	radius = (radius & ~0xFFFFu) | ((uint16_t)radius >> 2);
	OBJECT(current_object)->x = sine_table[direction] * radius + 0x5D00000u;
	OBJECT(current_object)->z = 0u - cosine_table[direction] * radius + 0x780000u;
}

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

void move_object_to_next_stage_slot(void)
{
	if (next_stage_slot_request != 0u)
	{
		OBJECT(current_object)->x = (uint32_t)OBJECT(current_object)->x - stage_link_from_x + stage_link_to_x;
		OBJECT(current_object)->y = (uint32_t)OBJECT(current_object)->y - stage_link_from_y + stage_link_to_y;
		OBJECT(current_object)->previous_floor_z_int = OBJECT(current_object)->previous_floor_z_int - stage_link_from_height + stage_link_to_height;
		OBJECT(current_object)->z_int = OBJECT(current_object)->z_int - stage_link_from_height + stage_link_to_height;
		OBJECT(current_object)->previous_floor_block = (uint16_t)(OBJECT(current_object)->previous_floor_block - stage_link_from_block + stage_link_to_block);
	}
}

void spawn_skid_dust(uint32_t dust)
{
	uint16_t priority_bits = 0;
	uint32_t free_count = 0;

	allocate_object(&free_count, &dust);
	if (free_count != 0u)
	{
		OBJECT(dust)->sprite_flags = 0x1000u;
		OBJECT(dust)->sprite_palette = 0x8260u;
		OBJECT(dust)->animation_frame = 0u;
		OBJECT(dust)->animation_speed = 3u;
		OBJECT(dust)->animation_timer = 3;
		OBJECT(dust)->animation = skid_dust_animation_ADDRESS;
		OBJECT(dust)->sprite_frame = mem_read16(OBJECT(dust)->animation + 2u);
		OBJECT(dust)->projected_x = OBJECT(current_object)->projected_x;
		OBJECT(dust)->projected_y = OBJECT(current_object)->projected_y;
		OBJECT(dust)->z = OBJECT(current_object)->z;
		OBJECT(dust)->sprite_offset_x = 0;
		OBJECT(dust)->sprite_offset_y = 0;
		OBJECT(dust)->handler = skid_dust_update_ADDRESS;
		priority_bits = OBJECT(current_object)->sprite_palette & 0x6000u;
		OBJECT(dust)->sprite_palette &= 0x9FFFu;
		OBJECT(dust)->sprite_palette |= priority_bits;
	}
}

void skid_dust_update(void)
{
	uint32_t r1 = 0, r8 = 0, ignored = 0, wrapped = 0;

	animate(&r1, &wrapped, &r8);
	if ((uint8_t)wrapped == 0u)
	{
		update_screen_position(&ignored, &r1);
		r1 = queue_sprite(r1);
		return;
	}
	delete_current_object();
}

void player_hud_init(void)
{
	uint32_t gauge_position = 0, object = 0, player = 0, player_2 = 0, portrait_anim = 0, portrait_position = 0, portrait_sprite = 0;

	PLAYER(current_player)->hud_blink_on = 0u;
	PLAYER(current_player)->stage_rings_shown = 0xFFFFu;
	PLAYER(current_player)->hud_blink_timer = 0x1E;
	player = current_player + 0x142u;
	PLAYER(player - 0x142u)->hud_gauge_flags = 0x900Au;
	PLAYER(player - 0x142u)->hud_gauge_tile = 0x89E0u;
	PLAYER(player - 0x142u)->hud_gauge_address = mem_read16(((sprite_frames & ~0xFFFFu) | PLAYER(player - 0x142u)->hud_gauge_tile) + 6u);
	PLAYER(player - 0x142u)->hud_gauge_health = OBJECT(current_object)->health;
	PLAYER(player - 0x142u)->hud_gauge_frame = OBJECT(current_object)->player_view.health_pad;
	player_2 = OBJECT(current_object)->player;
	gauge_position = hud_gauge_positions_3p_ADDRESS + player_2 * 4u;
	if ((cabinet_config & 1u) == 0u)
		gauge_position = hud_gauge_positions_2p_ADDRESS + player_2 * 4u;
	PLAYER(player - 0x142u)->hud_gauge_x = ROM16(gauge_position);
	PLAYER(player - 0x142u)->hud_gauge_y = ROM16(gauge_position + 2u);
	PLAYER(player - 0x142u)->hud_gauge_palette = 0x88E0u;
	PLAYER(player - 0x142u)->hud_gauge_height = 8u;
	PLAYER(current_player)->hud_gauge = player;
	PLAYER(player - 0x142u)->hud_gauge_anim = 0xCD4Du;
	PLAYER(player - 0x142u)->hud_gauge_anim_delay = 3u;
	PLAYER(player - 0x142u)->hud_gauge_frame = 0u;
	PLAYER(player - 0x142u)->hud_gauge_anim_repeats = 0xFFu;
	PLAYER(player - 0x142u)->hud_gauge_reset = 0u;
	player = current_player + 0x172u;
	PLAYER(player - 0x172u)->hud_portrait_flags = 0x100Au;
	player_2 = OBJECT(current_object)->player;
	portrait_position = hud_portrait_positions_3p_ADDRESS + player_2 * 4u;
	if ((cabinet_config & 1u) == 0u)
		portrait_position = hud_portrait_positions_2p_ADDRESS + player_2 * 4u;
	PLAYER(player - 0x172u)->hud_portrait_x = ROM16(portrait_position);
	PLAYER(player - 0x172u)->hud_portrait_y = ROM16(portrait_position + 2u);
	portrait_sprite = hud_portrait_sprites_ADDRESS + OBJECT(current_object)->kind * 4u;
	PLAYER(player - 0x172u)->hud_portrait_tile = ROM16(portrait_sprite);
	PLAYER(player - 0x172u)->hud_portrait_palette = ROM16(portrait_sprite + 2u);
	PLAYER(player - 0x172u)->hud_portrait_height = 0xAu;
	PLAYER(current_player)->hud_portrait = player;
	PLAYER(player - 0x172u)->hud_portrait_anim_delay = 3u;
	PLAYER(player - 0x172u)->hud_portrait_frame = 0u;
	PLAYER(player - 0x172u)->hud_portrait_anim_repeats = 0xFFu;
	portrait_anim = 0xCD63u;
	if (OBJECT(current_object)->kind != 0u)
	{
		portrait_anim = 0xCD6Bu;
		if (OBJECT(current_object)->kind != 1u)
			portrait_anim = 0xCD73u;
	}
	PLAYER(player - 0x172u)->hud_portrait_anim = portrait_anim;
	player = current_player + 0x1A2u;
	PLAYER(player - 0x1A2u)->hud_ring_effect_flags = 0x1000u;
	PLAYER(player - 0x1A2u)->hud_ring_effect_palette = 0x1B0u;
	PLAYER(player - 0x1A2u)->hud_ring_effect_anim = ring_sparkle_animation_ADDRESS;
	PLAYER(player - 0x1A2u)->hud_ring_effect_frame = 0xFFu;
	PLAYER(player - 0x1A2u)->hud_ring_effect_anim_timer = 0u;
	PLAYER(player - 0x1A2u)->hud_ring_effect_anim_delay = 2u;
	object = LINKED(current_object, PLAYER(current_player)->hud_gauge);
	PLAYER(player - 0x1A2u)->hud_ring_effect_x = OBJECT(object)->screen_x;
	PLAYER(player - 0x1A2u)->hud_ring_effect_x += 0x10u;
	PLAYER(player - 0x1A2u)->hud_ring_effect_y = OBJECT(object)->screen_y;
	PLAYER(current_player)->hud_ring_effect = player;
	PLAYER(current_player)->unused_hud_byte = 0u;
	PLAYER(current_player)->unused_hud_word = 0xFFFFu;
}

void player_hud_start(void)
{
	uint32_t r1 = 0, init_bit = 0, object = 0, player = 0, sprite_record = 0, text = 0;

	sprite_record = hud_portrait_sprites_ADDRESS + OBJECT(current_object)->kind * 4u;
	object = LINKED(current_object, PLAYER(current_player)->hud_portrait);
	OBJECT(object)->sprite_frame = ROM16(sprite_record);
	OBJECT(object)->sprite_palette = ROM16(sprite_record + 2u);
	player = OBJECT(current_object)->player;
	if ((int8_t)player_status[player] >= 0 || player_status[player] == 0x82u)
	{
		player_hud_update(&r1, &object);
		return;
	}
	text = hud_message_blank_text_ADDRESS;
	player = OBJECT(current_object)->player;
	object = 0x310000u | hud_text_positions_3p[player];
	if ((cabinet_config & 1u) == 0u)
		object = (object & ~0xFFFFu) | hud_text_positions_2p[player];
	erase_text_lines(&text, &object);
	if (game_in_progress != 0u)
	{
		player_hud_update(&r1, &object);
		return;
	}
	init_bit = 0xFu;
	if (OBJECT(current_object)->player != 0u)
	{
		init_bit = 0x12u;
		if (OBJECT(current_object)->player != 1u)
			init_bit = 0x13u;
	}
	stage_init_flags &= ~shl32(1u, (int)(init_bit));
	player_hud_update(&r1, &object);
}

void player_hud_update(uint32_t *p_digit_attribute, uint32_t *p_text_position)
{
	uint32_t digit_attribute = *p_digit_attribute;
	uint32_t text_position = *p_text_position;
	uint32_t r11 = 0, digit = 0, message_position = 0, object = 0, player = 0, player_2 = 0, portrait_anim = 0, text = 0, value = 0, wrapped = 0;
	uint32_t flags = 0;

	player = OBJECT(current_object)->player;
	if (player_status[player] == 0x82u)
	{
		text = hud_wait_text_ADDRESS;
		text_position = blink_player_hud_message(text, text_position);
		AT8(OBJECT(LINKED(current_object, PLAYER(current_player)->hud_gauge))->projected_y, 1) = 0xFFu;
		*p_text_position = text_position;
		return;
	}
	if (PLAYER(current_player)->state == 0x23u || PLAYER(current_player)->state == 0x2Eu || PLAYER(current_player)->state == 0x34u || PLAYER(current_player)->state == 0x24u || PLAYER(current_player)->state == 0x37u)
	{
		AT8(OBJECT(LINKED(current_object, PLAYER(current_player)->hud_gauge))->projected_y, 1) = 0xFFu;
		return;
	}
	if (PLAYER(current_player)->state == 0x2Bu)
	{
		if (PLAYER(current_player)->hud_message_shown == 0u)
		{
			AT8(OBJECT(LINKED(current_object, PLAYER(current_player)->hud_gauge))->projected_y, 1) = 0xFFu;
			return;
		}
		PLAYER(current_player)->hud_message_shown = 0u;
		text = hud_message_blank_text_ADDRESS;
		player = OBJECT(current_object)->player;
		text_position = 0x310000u | hud_text_positions_3p[player];
		if ((cabinet_config & 1u) == 0u)
			text_position = (text_position & ~0xFFFFu) | hud_text_positions_2p[player];
		erase_text_lines(&text, &text_position);
		AT8(OBJECT(LINKED(current_object, PLAYER(current_player)->hud_gauge))->projected_y, 1) = 0xFFu;
		*p_text_position = text_position;
		return;
	}
	if (PLAYER(current_player)->state == 0x39u && game_mode_timer >= 0x3C)
	{
		AT8(OBJECT(LINKED(current_object, PLAYER(current_player)->hud_gauge))->projected_y, 1) = 0xFFu;
		return;
	}
	if ((int8_t)player_status[player] < 0 || player_status[player] == 1u)
	{
		object = LINKED(current_object, PLAYER(current_player)->hud_gauge);
		current_object = LINKED(current_object, PLAYER(current_player)->hud_portrait);
		if (AT8(OBJECT(object)->projected_y, 1) != 0u)
		{
			AT8(OBJECT(object)->projected_y, 1) = 0u;
			AS8(OBJECT(object)->direction) = OBJECT(object_cursor)->health;
			AS8(OBJECT(object)->projected_y) = 0u;
			AS8(OBJECT(current_object)->projected_y) = 0u;
			OBJECT(object)->sprite_palette = mem_read16(AS32(OBJECT(object)->child) + 2u);
			portrait_anim = 0xCD63u;
			if (OBJECT(object_cursor)->kind != 0u)
			{
				portrait_anim = 0xCD6Bu;
				if (OBJECT(object_cursor)->kind != 1u)
					portrait_anim = 0xCD73u;
			}
			AS32(OBJECT(current_object)->child) = portrait_anim;
			OBJECT(current_object)->sprite_palette = mem_read16(AS32(OBJECT(current_object)->child) + 2u);
		}
		flags = PLAYER(current_player)->flags;
		PLAYER(current_player)->flags = flags & ~0x40u;
		if ((flags & 0x40u) == 0u)
		{
			if (OBJECT(object_cursor)->health < (int8_t)AS8(OBJECT(object)->direction))
			{
				OBJECT(object)->child = 0xCD4Du;
				OBJECT(object)->projected_x = 0;
				AT8(OBJECT(object)->direction, 1) = 0u;
				AT8(OBJECT(object)->parent, 1) = 0u;
				AS8(OBJECT(object)->projected_y) = 3u;
				portrait_anim = 0xCD63u;
				if (OBJECT(object_cursor)->kind != 0u)
				{
					portrait_anim = 0xCD6Bu;
					if (OBJECT(object_cursor)->kind != 1u)
						portrait_anim = 0xCD73u;
				}
				AS32(OBJECT(current_object)->child) = portrait_anim;
				AT8(OBJECT(current_object)->direction, 1) = 0u;
				AT8(OBJECT(current_object)->parent, 1) = 0u;
				AS8(OBJECT(current_object)->projected_y) = 3u;
			}
		}
		else
		{
			OBJECT(object)->child = 0xCD37u;
			OBJECT(object)->projected_x = 0;
			AT8(OBJECT(object)->direction, 1) = 0u;
			AT8(OBJECT(object)->parent, 1) = 0u;
			AS8(OBJECT(object)->projected_y) = 1u;
			AT8(OBJECT((current_object & ~0xFFFFu) | PLAYER(current_player)->hud_ring_effect)->direction, 1) = 0u;
		}
		AS8(OBJECT(object)->direction) = OBJECT(object_cursor)->health;
		if (OBJECT(object_cursor)->health <= 8 && (int8_t)AS8(OBJECT(object)->projected_y) <= 0)
		{
			OBJECT(object)->child = 0xCD55u;
			OBJECT(object)->projected_x = 0;
			AT8(OBJECT(object)->direction, 1) = 0u;
			AT8(OBJECT(object)->parent, 1) = 0u;
			AS8(OBJECT(object)->projected_y) = 3u;
			portrait_anim = 0xCD7Bu;
			if (OBJECT(object_cursor)->kind != 0u)
			{
				portrait_anim = 0xCD89u;
				if (OBJECT(object_cursor)->kind != 1u)
					portrait_anim = 0xCD97u;
			}
			AS32(OBJECT(current_object)->child) = portrait_anim;
			AT8(OBJECT(current_object)->direction, 1) = 0u;
			AT8(OBJECT(current_object)->parent, 1) = 0u;
			AS8(OBJECT(current_object)->projected_y) = 3u;
		}
		if (game_in_progress != 0u)
		{
			value = PLAYER(current_player)->rings;
			convert_to_bcd(&value, &digit);
			digit_attribute = (digit & ~0xFFFFu) | 0x800u;
			if (OBJECT(object_cursor)->kind != 0u)
			{
				digit_attribute = (digit_attribute & ~0xFFFFu) | 0xA00u;
				if (OBJECT(object_cursor)->kind != 1u)
					digit_attribute = (digit_attribute & ~0xFFFFu) | 0xC00u;
			}
			player_2 = OBJECT(object_cursor)->player;
			text_position = 0x310000u | hud_ring_count_positions_3p[player_2];
			if ((cabinet_config & 1u) == 0u)
				text_position = (text_position & ~0xFFFFu) | hud_ring_count_positions_2p[player_2];
			draw_hex_4(&value, &digit_attribute, &text_position);
		}
		current_object = (current_object & ~0xFFFFu) | PLAYER(current_player)->hud_ring_effect;
		if ((int8_t)AT8(OBJECT(current_object)->direction, 1) >= 0)
		{
			animate_hud_tiles(&digit_attribute, &wrapped);
			if ((int8_t)wrapped < 0)
				AT8(OBJECT(current_object)->direction, 1) = 0xFFu;
			queue_sprite_on_top();
		}
		current_object = LINKED(current_object, PLAYER(current_player)->hud_portrait);
		if ((int8_t)AS8(OBJECT(current_object)->projected_y) > 0)
		{
			animate_hud_palette(&digit_attribute, &wrapped);
			if (wrapped != 0u)
				AS8(OBJECT(current_object)->projected_y)--;
		}
		queue_sprite_on_top();
		current_object = LINKED(current_object, PLAYER(current_player)->hud_gauge);
		if ((int8_t)AS8(OBJECT(current_object)->projected_y) > 0)
		{
			animate_hud_palette(&digit_attribute, &wrapped);
			if (wrapped != 0u)
				AS8(OBJECT(current_object)->projected_y)--;
		}
		OBJECT(current_object)->sprite_width = (uint8_t)OBJECT(object_cursor)->health * 2u;
		if (OBJECT(current_object)->sprite_width <= 0)
		{
			current_object = (current_object & ~0xFFFFu) | (uint16_t)object_cursor;
			*p_digit_attribute = digit_attribute;
			*p_text_position = text_position;
			return;
		}
		OBJECT(current_object)->sprite_width++;
		queue_sprite_on_top();
		current_object = (current_object & ~0xFFFFu) | (uint16_t)object_cursor;
		*p_digit_attribute = digit_attribute;
		*p_text_position = text_position;
		return;
	}
	AT8(OBJECT(LINKED(current_object, PLAYER(current_player)->hud_gauge))->projected_y, 1) = 0xFFu;
	if (current_game_mode == 0x18u)
	{
		if (PLAYER(current_player)->hud_message_shown == 0u)
		{
			AT8(OBJECT(LINKED(current_object, PLAYER(current_player)->hud_gauge))->projected_y, 1) = 0xFFu;
			return;
		}
		PLAYER(current_player)->hud_message_shown = 0u;
		text = hud_message_blank_text_ADDRESS;
		player = OBJECT(current_object)->player;
		text_position = 0x310000u | hud_text_positions_3p[player];
		if ((cabinet_config & 1u) == 0u)
			text_position = (text_position & ~0xFFFFu) | hud_text_positions_2p[player];
		erase_text_lines(&text, &text_position);
		AT8(OBJECT(LINKED(current_object, PLAYER(current_player)->hud_gauge))->projected_y, 1) = 0xFFu;
		*p_text_position = text_position;
		return;
	}
	if (joining_closed == 0u)
	{
		r11 = draw_player_start_prompt();
		*p_digit_attribute = digit_attribute;
		*p_text_position = r11;
		return;
	}
	if (PLAYER(current_player)->hud_message_shown != 0u)
		return;
	PLAYER(current_player)->hud_message_shown = 0xFFu;
	text = hud_message_blank_text_ADDRESS;
	player = OBJECT(current_object)->player;
	message_position = 0x310000u | hud_text_positions_3p[player];
	if ((cabinet_config & 1u) == 0u)
		message_position = (message_position & ~0xFFFFu) | hud_text_positions_2p[player];
	erase_text_lines(&text, &message_position);
	player = OBJECT(current_object)->player;
	text_position = 0x310000u | start_prompt_positions_3p[player];
	if ((cabinet_config & 1u) == 0u)
		text_position = (text_position & ~0xFFFFu) | start_prompt_positions_2p[player];
	text = 0xCDA5u;
	print_text_lines(&text, &text_position);
	*p_text_position = text_position;
}

uint32_t draw_player_start_prompt(void)
{
	uint32_t blink_bit = 0, credit_block = 0, player = 0, prompt_position = 0, prompt_texts = 0, text = 0;

	switch (cabinet_config)
	{
	case 0:
	case 1:
	case 4:
		credit_block = p1_coins_ADDRESS;
		player = OBJECT(current_object)->player;
		prompt_position = 0x310000u | start_prompt_positions_3p[player];
		if ((cabinet_config & 1u) == 0u)
			prompt_position = (prompt_position & ~0xFFFFu) | start_prompt_positions_2p[player];
		prompt_texts = start_prompt_texts_ADDRESS;
		blink_start_prompt(credit_block, prompt_texts, &blink_bit, &text, &prompt_position);
		return prompt_position;
	case 2:
	case 3:
		player = OBJECT(current_object)->player;
		credit_block = p1_coins_ADDRESS + player * 4u;
		prompt_position = 0x310000u | start_prompt_positions_3p[player];
		if ((cabinet_config & 1u) == 0u)
			prompt_position = (prompt_position & ~0xFFFFu) | start_prompt_positions_2p[player];
		prompt_texts = start_prompt_texts_ADDRESS;
		blink_start_prompt(credit_block, prompt_texts, &blink_bit, &text, &prompt_position);
		return prompt_position;
	default:
		unknown_code(0xA9E6u + cabinet_config * 2u);
	}
}

void animate_hud_palette(uint32_t *p_list_index, uint32_t *p_wrapped)
{
	uint32_t list_index = *p_list_index;
	uint8_t last_frame = 0;
	uint32_t palettes = 0, wrapped = 0;
	uint8_t anim_timer = 0;

	wrapped = 0u;
	anim_timer = OBJECT(current_object)->animate_hud.anim_timer;
	OBJECT(current_object)->animate_hud.anim_timer = anim_timer - 1u;
	if ((int8_t)anim_timer > 1)
	{
		*p_wrapped = wrapped;
		return;
	}
	OBJECT(current_object)->animate_hud.anim_timer = OBJECT(current_object)->animate_hud.anim_delay;
	palettes = OBJECT(current_object)->animate_hud.anim;
	last_frame = (uint8_t)mem_read16(palettes);
	list_index = OBJECT(current_object)->animate_hud.frame;
	list_index = (uint8_t)(list_index + 1u);
	palettes += 2u;
	if ((int8_t)last_frame < (int8_t)list_index)
	{
		wrapped = 0xFFFFFFFFu;
		list_index = 0u;
	}
	OBJECT(current_object)->animate_hud.frame = list_index;
	OBJECT(current_object)->sprite_palette = mem_read16(palettes + list_index * 2u);
	*p_list_index = list_index;
	*p_wrapped = wrapped;
}

void animate_hud_tiles(uint32_t *p_list_index, uint32_t *p_wrapped)
{
	uint32_t list_index = *p_list_index;
	uint8_t last_frame = 0;
	uint32_t frame_list = 0, wrapped = 0;
	uint8_t anim_timer = 0;

	wrapped = 0u;
	anim_timer = OBJECT(current_object)->animate_hud.anim_timer;
	OBJECT(current_object)->animate_hud.anim_timer = anim_timer - 1u;
	if ((int8_t)anim_timer > 1)
	{
		*p_wrapped = wrapped;
		return;
	}
	OBJECT(current_object)->animate_hud.anim_timer = OBJECT(current_object)->animate_hud.anim_delay;
	frame_list = OBJECT(current_object)->animate_hud.anim;
	last_frame = (uint8_t)mem_read16(frame_list);
	list_index = OBJECT(current_object)->animate_hud.frame;
	list_index = (uint8_t)(list_index + 1u);
	frame_list += 2u;
	if ((int8_t)list_index <= (int8_t)last_frame)
		OBJECT(current_object)->sprite_frame = mem_read16(frame_list + list_index * 2u);
	else
	{
		wrapped = 0xFFFFFFFFu;
		list_index = 0u;
	}
	OBJECT(current_object)->animate_hud.frame = list_index;
	*p_list_index = list_index;
	*p_wrapped = wrapped;
}

uint32_t blink_player_hud_message(uint32_t text, uint32_t text_position)
{
	uint32_t player = 0, prompt_position = 0;
	uint8_t hud_blink_timer = 0;

	hud_blink_timer = PLAYER(current_player)->hud_blink_timer;
	PLAYER(current_player)->hud_blink_timer = hud_blink_timer - 1u;
	if ((int8_t)hud_blink_timer > 1)
		return text_position;
	PLAYER(current_player)->hud_blink_timer = 0x1E;
	PLAYER(current_player)->hud_blink_on = ~PLAYER(current_player)->hud_blink_on;
	player = OBJECT(current_object)->player;
	prompt_position = 0x310000u | hud_prompt_positions_3p[player];
	if ((cabinet_config & 1u) == 0u)
		prompt_position = (prompt_position & ~0xFFFFu) | hud_prompt_positions_2p[player];
	if (PLAYER(current_player)->hud_blink_on == 0u)
	{
		erase_text_lines(&text, &prompt_position);
		return prompt_position;
	}
	print_text_lines(&text, &prompt_position);
	return prompt_position;
}

void draw_player_stage_rings(void)
{
	uint16_t attribute = 0;
	uint32_t text_position = 0, value = 0;

	if (PLAYER(current_player)->stage_rings_shown != PLAYER(current_player)->stage_rings)
	{
		PLAYER(current_player)->stage_rings_shown = PLAYER(current_player)->stage_rings;
		text_position = 0x310000u | hud_stage_rings_positions[OBJECT(current_object)->player];
		attribute = 0u;
		value = PLAYER(current_player)->stage_rings_shown;
		draw_hex_2(value, attribute, text_position);
	}
}

uint32_t keep_player_on_screen(uint32_t depth)
{
	uint8_t sound_id = 0;
	uint16_t base_x = 0, base_y = 0, edge = 0, scroll_direction = 0;
	uint32_t direction_row = 0, floor_slope = 0, offset_x = 0, slope_direction = 0, slope_factor = 0, slope_speed = 0, voice_range = 0;
	uint8_t health = 0;

	if (nearby_floor_blocks_locked != 0u || (OBJECT(current_object)->flags & 0x4000000u) != 0u)
		return depth;
	offset_x = (depth & ~0xFFFFu) | (uint16_t)OBJECT(current_object)->sprite_offset_x;
	base_x = (uint16_t)(OBJECT(current_object)->screen_x - offset_x);
	depth = (offset_x & ~0xFFFFu) | (uint16_t)OBJECT(current_object)->sprite_offset_y;
	base_y = (uint16_t)(OBJECT(current_object)->screen_y - depth);
	if ((stage != 8u || scroll_position >= 0x4000u) && ((uint16_t)(base_x + 0x10u) >= 0x160u || (uint16_t)(base_y + 0xE0u) >= 0x1E0u))
	{
		if (stage == 5u)
			OBJECT(current_object)->handler = player_respawn_ADDRESS;
		else
		{
			health = OBJECT(current_object)->health;
			OBJECT(current_object)->health = health - 8u;
			if ((int8_t)health > 8)
			{
				OBJECT(current_object)->handler = player_respawn_ADDRESS;
				return depth;
			}
			OBJECT(current_object)->health = 0;
			PLAYER(current_player)->state = 0x16u;
			PLAYER(current_player)->substate = 0u;
		}
	}
	else
	{
		if ((OBJECT(current_object)->flags & 0x1000u) != 0u || PLAYER(current_player)->ignore_screen_edges != 0u || OBJECT(current_object)->floor_z_int >= 0x7FF)
			return depth;
		if ((int16_t)base_x > 0x10)
		{
			if ((int16_t)base_x < 0x128)
			{
				if (PLAYER(current_player)->state == 4u)
					base_y -= 9u;
				base_y += OBJECT(current_object)->floor_z_int - OBJECT(current_object)->z_int;
				if ((int16_t)base_y > 0x20)
				{
					if ((int16_t)base_y < 0xD0)
						return depth;
					edge = 2u;
				}
				else
					edge = 0u;
			}
			else
				edge = 1u;
		}
		else
			edge = 3u;
		PLAYER(current_player)->screen_edge = edge;
		if ((PLAYER(current_player)->flags & 0x20u) == 0u)
		{
			voice_range = edge_voice_scroll_ranges_ADDRESS + stage * 4u;
			if ((uint16_t)(scroll_position - ROM16(voice_range)) < ROM16(voice_range + 2u) && players_in_play >= 2)
			{
				sound_id = 0x39u;
				if (OBJECT(current_object)->kind != 0u)
				{
					sound_id = 0x43u;
					if (OBJECT(current_object)->kind != 1u)
						sound_id = 0x4Cu;
				}
				play_sound(sound_id, &depth, &slope_speed);
				PLAYER(current_player)->flags |= 0x20u;
			}
		}
		slope_factor = 0u;
		if ((OBJECT(current_object)->floor_slope | OBJECT(current_object)->floor_type) != 0u)
		{
			floor_slope = OBJECT(current_object)->floor_slope;
			slope_factor = floor_slope_factors[(uint16_t)(floor_slope * 2u)];
		}
		compute_camera_slope_speed(0u, slope_factor);
		if (camera_slope_speed == 0u)
			return push_player_from_screen_edge();
		scroll_direction = camera_scroll_direction;
		direction_row = 0u;
		if ((uint8_t)scroll_direction != 0u)
			direction_row = (uint16_t)(direction_row + 4u);
		direction_row = (uint16_t)(direction_row + (scroll_direction >> 8));
		if ((int8_t)ROM8((uint16_t)(direction_row << 2) + screen_edge_scroll_carry_flags_ADDRESS + PLAYER(current_player)->screen_edge) >= 0)
			return push_player_from_screen_edge();
		push_player_from_screen_edge();
		slope_direction = (int16_t)camera_slope_direction;
		slope_speed = camera_slope_speed >> 14;
		OBJECT(current_object)->x += (int32_t)(sine_table[slope_direction] * slope_speed) >> 8;
		PLAYER(current_player)->previous_x = OBJECT(current_object)->x;
		depth = (int32_t)(cosine_table[slope_direction] * slope_speed) >> 8;
		OBJECT(current_object)->y -= depth;
		PLAYER(current_player)->previous_y = OBJECT(current_object)->y;
	}
	return depth;
}

uint32_t push_player_from_screen_edge(void)
{
	uint16_t relative_direction = 0;
	uint32_t r1 = 0, push_record = 0, push_x = 0, push_y = 0;

	push_record = screen_edge_push_directions_ADDRESS + PLAYER(current_player)->screen_edge * 4u;
	relative_direction = (OBJECT(current_object)->direction - ROM16(push_record)) & 0x3FFu;
	push_record += 2u;
	if (relative_direction >= 0x101u)
	{
		OBJECT(current_object)->x = PLAYER(current_player)->previous_x;
		OBJECT(current_object)->y = PLAYER(current_player)->previous_y;
		OBJECT(current_object)->floor_block = OBJECT(current_object)->previous_floor_block;
		OBJECT(current_object)->floor_z = OBJECT(current_object)->previous_floor_z;
	}
	push_x = ROM16(push_record);
	push_y = 0x200u;
	polar_to_xy(&r1, &push_x, &push_y);
	OBJECT(current_object)->x += push_x;
	OBJECT(current_object)->y += push_y;
	PLAYER(current_player)->previous_x = OBJECT(current_object)->x;
	PLAYER(current_player)->previous_y = OBJECT(current_object)->y;
	return r1;
}

uint32_t compute_camera_ground_speed(uint32_t octant)
{
	uint16_t dy = 0;
	uint32_t ground_angle = 0, ignored = 0, map_angle = 0, vx = 0, vy = 0;

	if (camera_ground_speed_vx != (uint32_t)camera_velocity_x || camera_ground_speed_vy != (uint32_t)camera_velocity_y)
	{
		vx = (int32_t)camera_velocity_x >> 5;
		vy = (int32_t)camera_velocity_y >> 4;
		vx = vx * vx + vy * vy;
		square_root(&vx, &ignored);
		camera_ground_speed = (vx & 0xFFFFu) << 13;
		ground_angle = (int32_t)camera_velocity_x >> 9;
		dy = (uint16_t)((int32_t)camera_velocity_y >> 8);
		angle_of_vector(dy, &ground_angle, &ignored);
		camera_ground_direction = (ground_angle + 0x80u) & 0x3FFu;
		map_angle = (int32_t)camera_velocity_x >> 8;
		dy = (uint16_t)((int32_t)camera_velocity_y >> 8);
		angle_of_vector(dy, &map_angle, &octant);
		camera_map_direction = map_angle;
		camera_ground_speed_vx = camera_velocity_x;
		camera_ground_speed_vy = camera_velocity_y;
	}
	return octant;
}

uint32_t compute_camera_slope_speed(uint32_t octant, uint32_t slope_factor)
{
	uint16_t dy = 0;
	uint32_t ignored = 0, scroll_angle = 0, slope_angle = 0, vx = 0, vy = 0;

	if (((uint32_t)camera_velocity_x != 0u || (uint32_t)camera_velocity_y != 0u || camera_slope_speed == 0u) && camera_slope_factor == (uint16_t)slope_factor && camera_slope_speed_vx == (uint32_t)camera_velocity_x && camera_slope_speed_vy == (uint32_t)camera_velocity_y)
		return octant;
	camera_slope_factor = slope_factor;
	if (slope_factor == 0u)
	{
		vx = (int32_t)camera_velocity_x >> 5;
		vy = (int32_t)camera_velocity_y >> 4;
		vx = vx * vx + vy * vy;
		square_root(&vx, &ignored);
		camera_slope_speed = (vx & 0xFFFFu) << 13;
		slope_angle = (int32_t)camera_velocity_x >> 9;
		dy = (uint16_t)((int32_t)camera_velocity_y >> 8);
		angle_of_vector(dy, &slope_angle, &ignored);
		camera_slope_direction = (slope_angle + 0x80u) & 0x3FFu;
	}
	else
	{
		if ((int32_t)slope_factor < 0)
			slope_factor = 0u - slope_factor;
		vy = divide_s32((uint32_t)camera_velocity_y << 11, slope_factor + 0x10000u);
		slope_velocity_y = vy;
		vx = (int32_t)camera_velocity_x >> 5;
		vx = vx * vx + vy * vy;
		square_root(&vx, &ignored);
		camera_slope_speed = (vx & 0xFFFFu) << 13;
		slope_angle = (int32_t)camera_velocity_x >> 5;
		dy = (uint16_t)slope_velocity_y;
		angle_of_vector(dy, &slope_angle, &ignored);
		camera_slope_direction = (slope_angle + 0x80u) & 0x3FFu;
	}
	scroll_angle = (int32_t)camera_velocity_x >> 5;
	dy = (uint16_t)((int32_t)camera_velocity_y >> 5);
	angle_of_vector(dy, &scroll_angle, &octant);
	camera_scroll_direction = scroll_angle;
	camera_slope_speed_vx = camera_velocity_x;
	camera_slope_speed_vy = camera_velocity_y;
	return octant;
}

void compute_scroll_drift(uint32_t block, uint8_t drift_byte)
{
	uint16_t dy = 0;
	uint32_t divisor = 0, drift_angle = 0, ignored = 0, scroll_angle = 0, slope = 0, vx = 0, vy = 0;

	if (scroll_drift_block == block && scroll_drift_vx == (uint32_t)camera_velocity_x && scroll_drift_vy == (uint32_t)camera_velocity_y)
		return;
	unused_drift_byte = drift_byte;
	scroll_drift_block = block;
	if (mem_read8(block + 0x13u) == 0u)
	{
		vx = (int32_t)camera_velocity_x >> 5;
		vy = (int32_t)camera_velocity_y >> 4;
		vx = vx * vx + vy * vy;
		square_root(&vx, &ignored);
		scroll_drift_speed = (vx & 0xFFFFu) << 13;
		drift_angle = (int32_t)camera_velocity_x >> 9;
		dy = (uint16_t)((int32_t)camera_velocity_y >> 8);
		angle_of_vector(dy, &drift_angle, &ignored);
		scroll_drift_direction = (drift_angle + 0x80u) & 0x3FFu;
	}
	else
	{
		slope = mem_read8(block + 0x13u);
		divisor = floor_slope_factors[(uint16_t)(slope * 2u)];
		if ((int32_t)divisor < 0)
			divisor = 0u - divisor;
		vy = camera_velocity_y;
		divisor += 0x10000u;
		unused_drift_divisor = divisor;
		vy = divide_s32(vy << 11, divisor);
		vx = (int32_t)camera_velocity_x >> 5;
		vx = vx * vx + vy * vy;
		square_root(&vx, &ignored);
		scroll_drift_speed = (vx & 0xFFFFu) << 13;
		scroll_drift_direction = (uint32_t)ROM8(scroll_drift_block + 0x15u) << 8;
	}
	scroll_angle = (int32_t)camera_velocity_x >> 5;
	dy = (uint16_t)((int32_t)camera_velocity_y >> 5);
	angle_of_vector(dy, &scroll_angle, &ignored);
	camera_scroll_direction = scroll_angle;
	scroll_drift_vx = camera_velocity_x;
	scroll_drift_vy = camera_velocity_y;
}

void slip_effect_update(void)
{
	uint8_t bucket = 0;
	uint32_t r1 = 0, r7 = 0, r8 = 0, ignored = 0;

	animate(&r1, &r7, &r8);
	if (OBJECT(current_object)->animation_frame == 0u)
	{
		delete_current_object();
		return;
	}
	update_screen_position(&ignored, &r1);
	if ((int16_t)OBJECT(current_object)->direction >= 2)
	{
		bucket = 0u;
		queue_sprite_in_bucket(bucket);
		return;
	}
	bucket = 0xFFu;
	queue_sprite_in_bucket(bucket);
}

uint32_t set_speed_from_trackball(void)
{
	uint16_t trackball_y_2 = 0;
	uint32_t ignored = 0, speed = 0, trackball_angle = 0, trackball_y = 0;

	read_player_trackball();
	speed = PLAYER(current_player)->trackball_x;
	trackball_y = PLAYER(current_player)->trackball_y;
	speed = speed * speed + trackball_y * trackball_y;
	square_root(&speed, &ignored);
	speed = (uint16_t)speed;
	speed = speed * speed;
	if ((int32_t)speed > 0x5000000)
		speed = 0x5000000u;
	OBJECT(current_object)->speed = speed;
	trackball_angle = (uint16_t)PLAYER(current_player)->trackball_x;
	trackball_y_2 = (uint16_t)PLAYER(current_player)->trackball_y;
	angle_of_vector(trackball_y_2, &trackball_angle, &ignored);
	OBJECT(current_object)->direction = (trackball_angle + 0x80u) & 0x3FFu;
	return set_speed_from_direction();
}

uint32_t set_climb_speed_from_trackball(void)
{
	uint16_t trackball_y_2 = 0;
	uint32_t ignored = 0, speed = 0, trackball_angle = 0, trackball_y = 0;

	read_player_trackball();
	speed = PLAYER(current_player)->trackball_x;
	trackball_y = PLAYER(current_player)->trackball_y;
	speed = speed * speed + trackball_y * trackball_y;
	square_root(&speed, &ignored);
	speed = (uint16_t)speed;
	speed = speed * speed;
	if ((int32_t)speed > 0x3000000)
		speed = 0x3000000u;
	OBJECT(current_object)->speed = speed;
	trackball_angle = (uint16_t)PLAYER(current_player)->trackball_x;
	trackball_y_2 = (uint16_t)PLAYER(current_player)->trackball_y;
	angle_of_vector(trackball_y_2, &trackball_angle, &ignored);
	OBJECT(current_object)->direction = trackball_angle & 0x3FFu;
	return set_speed_from_direction();
}

uint32_t update_ledge_pullback(void)
{
	uint32_t r1 = 0;

	r1 = set_speed_from_trackball();
	switch ((uint16_t)PLAYER(current_player)->ledge_side)
	{
	case 0:
	case 5:
		if (OBJECT(current_object)->speed_y_int >= 0)
			PLAYER(current_player)->ledge_pullback += OBJECT(current_object)->speed_y_int;
		break;
	case 1:
		if (OBJECT(current_object)->speed_x_int < 0)
			PLAYER(current_player)->ledge_pullback -= OBJECT(current_object)->speed_x_int;
		break;
	case 2:
		if (OBJECT(current_object)->speed_y_int < 0)
			PLAYER(current_player)->ledge_pullback -= OBJECT(current_object)->speed_y_int;
		break;
	case 3:
		if (OBJECT(current_object)->speed_x_int >= 0)
			PLAYER(current_player)->ledge_pullback += OBJECT(current_object)->speed_x_int;
		break;
	case 4:
		break;
	default:
		unknown_code(0xB36Fu + (uint16_t)PLAYER(current_player)->ledge_side * 2u);
	}
	return r1;
}

void get_ledge_side(uint32_t *p_side, uint32_t *p_block)
{
	uint32_t block = 0, bound_cursor = 0, side = 0;
	uint32_t x_min = 0, x_max = 0, y_min = 0;

	bound_cursor = floor_blocks + OBJECT(current_object)->previous_floor_block + 2u;
	block = bound_cursor;
	side = 3u;
	x_min = mem_read32(bound_cursor);
	bound_cursor += 4u;
	if (OBJECT(current_object)->x < (int32_t)x_min)
	{
		*p_side = side;
		*p_block = block;
		return;
	}
	side = 1u;
	x_max = mem_read32(bound_cursor);
	bound_cursor += 4u;
	if ((int32_t)x_max < OBJECT(current_object)->x)
	{
		*p_side = side;
		*p_block = block;
		return;
	}
	side = 0u;
	y_min = mem_read32(bound_cursor);
	bound_cursor += 4u;
	if (OBJECT(current_object)->y < (int32_t)y_min)
	{
		*p_side = side;
		*p_block = block;
		return;
	}
	side = 2u;
	if ((int32_t)mem_read32(bound_cursor) < OBJECT(current_object)->y)
	{
		*p_side = side;
		*p_block = block;
		return;
	}
	side = 4u;
	*p_side = side;
	*p_block = block;
}

void check_player_floor_edge(uint32_t *p_depth, uint32_t *p_block)
{
	uint32_t depth = *p_depth;
	uint32_t block = *p_block;
	uint8_t next_state = 0;
	uint32_t r1 = 0, block_2 = 0, draw_depth = 0, edge_speed_x = 0, edge_speed_y = 0, side = 0;

	if (PLAYER(current_player)->state == 0x17u)
	{
		check_fall_into_pit();
		*p_depth = depth;
		*p_block = block;
		return;
	}
	if (PLAYER(current_player)->state == 4u)
	{
		if (OBJECT(current_object)->floor_block >= 0)
		{
			check_fall_into_pit();
			return;
		}
		PLAYER(current_player)->previous_state = PLAYER(current_player)->state;
		PLAYER(current_player)->state = 7u;
		PLAYER(current_player)->substate = 1u;
		get_ledge_side(&side, &block);
		if ((uint16_t)side == 4u)
			side = (mem_read8(floor_blocks + 2u + OBJECT(current_object)->previous_floor_block + 0x15u) + 2u) & 3u;
		PLAYER(current_player)->ledge_side = side;
		draw_depth = (int32_t)((uint32_t)OBJECT(current_object)->projected_y + OBJECT(current_object)->previous_floor_z) >> 16;
		draw_depth = (draw_depth & ~0xFFFFu) | (uint16_t)(draw_depth - view_y);
		PLAYER(current_player)->fall_draw_depth = draw_depth;
		if (OBJECT(current_object)->floor_type == 1u)
		{
			if (PLAYER(current_player)->ledge_side < 2)
			{
				draw_depth = (draw_depth & ~0xFFFFu) | OBJECT(LINKED(current_object, OBJECT(current_object)->platform))->var_88;
				draw_depth = (draw_depth & ~0xFFFFu) | (uint16_t)(draw_depth - 1u);
				PLAYER(current_player)->fall_draw_depth = draw_depth;
			}
			PLAYER(current_player)->fall_draw_depth -= 0x10u;
		}
		else if (PLAYER(current_player)->ledge_side < 2)
		{
			if (stage != 0xAu)
			{
				PLAYER(current_player)->fall_draw_depth -= 0x10u;
				OBJECT(current_object)->flags &= ~0x100u;
				if (mem_read8(floor_blocks + 2u + OBJECT(current_object)->previous_floor_block + 0x12u) == 0x10u || OBJECT(current_object)->floor_type == 1u)
				{
					OBJECT(current_object)->flags |= 0x100u;
					PLAYER(current_player)->edge_object = OBJECT(LINKED(current_object, OBJECT(current_object)->platform))->parent;
				}
				PLAYER(current_player)->heading = OBJECT(current_object)->direction;
				PLAYER(current_player)->ledge_pullback = 0;
				PLAYER(current_player)->slip_distance = 0;
				PLAYER(current_player)->control_lock = 0u;
				OBJECT(current_object)->sprite_palette = mem_read16(OBJECT(current_object)->check_player.palette_list + 2u);
				OBJECT(current_object)->flags &= ~0x40u;
				OBJECT(current_object)->flags &= ~0x20000u;
				PLAYER(current_player)->flags |= 0x80u;
				OBJECT(current_object)->flags &= ~0x2000u;
				*p_depth = draw_depth;
				*p_block = block;
				return;
			}
			PLAYER(current_player)->fall_draw_depth -= 0x20u;
			PLAYER(current_player)->fall_draw_depth -= 0x10u;
			OBJECT(current_object)->flags &= ~0x100u;
			if (mem_read8(floor_blocks + 2u + OBJECT(current_object)->previous_floor_block + 0x12u) == 0x10u || OBJECT(current_object)->floor_type == 1u)
			{
				OBJECT(current_object)->flags |= 0x100u;
				PLAYER(current_player)->edge_object = OBJECT(LINKED(current_object, OBJECT(current_object)->platform))->parent;
			}
			PLAYER(current_player)->heading = OBJECT(current_object)->direction;
			PLAYER(current_player)->ledge_pullback = 0;
			PLAYER(current_player)->slip_distance = 0;
			PLAYER(current_player)->control_lock = 0u;
			OBJECT(current_object)->sprite_palette = mem_read16(OBJECT(current_object)->check_player.palette_list + 2u);
			OBJECT(current_object)->flags &= ~0x40u;
			OBJECT(current_object)->flags &= ~0x20000u;
			PLAYER(current_player)->flags |= 0x80u;
			OBJECT(current_object)->flags &= ~0x2000u;
			*p_depth = draw_depth;
			*p_block = block;
			return;
		}
		OBJECT(current_object)->flags &= ~0x100u;
		if (mem_read8(floor_blocks + 2u + OBJECT(current_object)->previous_floor_block + 0x12u) == 0x10u || OBJECT(current_object)->floor_type == 1u)
		{
			OBJECT(current_object)->flags |= 0x100u;
			PLAYER(current_player)->edge_object = OBJECT(LINKED(current_object, OBJECT(current_object)->platform))->parent;
		}
		PLAYER(current_player)->heading = OBJECT(current_object)->direction;
		PLAYER(current_player)->ledge_pullback = 0;
		PLAYER(current_player)->slip_distance = 0;
		PLAYER(current_player)->control_lock = 0u;
		OBJECT(current_object)->sprite_palette = mem_read16(OBJECT(current_object)->check_player.palette_list + 2u);
		OBJECT(current_object)->flags &= ~0x40u;
		OBJECT(current_object)->flags &= ~0x20000u;
		PLAYER(current_player)->flags |= 0x80u;
		OBJECT(current_object)->flags &= ~0x2000u;
		*p_depth = draw_depth;
		*p_block = block;
		return;
	}
	if (PLAYER(current_player)->state == 2u || OBJECT(current_object)->floor_z_int <= 0x7FF)
		return;
	if ((PLAYER(current_player)->flags & 2u) != 0u)
	{
		PLAYER(current_player)->previous_state = PLAYER(current_player)->state;
		PLAYER(current_player)->state = 7u;
		PLAYER(current_player)->substate = 0u;
		if (OBJECT(current_object)->speed_int >= 0x200)
			return;
		edge_speed_x = OBJECT(current_object)->direction;
		edge_speed_y = 0x100u;
		polar_to_xy(&r1, &edge_speed_x, &edge_speed_y);
		OBJECT(current_object)->speed_x = edge_speed_x;
		OBJECT(current_object)->speed_y = edge_speed_y;
		*p_depth = r1;
		return;
	}
	PLAYER(current_player)->substate = 0u;
	next_state = 5u;
	if (OBJECT(current_object)->floor_type != 1u && (OBJECT(current_object)->flags & 0x10u) == 0u)
	{
		block_2 = floor_blocks + 2u + OBJECT(current_object)->previous_floor_block;
		if (mem_read8(block_2 + 0x12u) != 0x12u && mem_read8(block_2 + 0x12u) != 0x23u && mem_read8(block_2 + 0x12u) != 0x18u && mem_read8(block_2 + 0x12u) != 0x19u && OBJECT(current_object)->speed_int > 0x200)
			next_state = 7u;
	}
	PLAYER(current_player)->previous_state = PLAYER(current_player)->state;
	PLAYER(current_player)->state = next_state;
}

void check_fall_into_pit(void)
{
	if (OBJECT(current_object)->speed_z_int >= 0 && OBJECT(current_object)->floor_z_int >= 0x7FF && OBJECT(current_object)->floor_type != 6u && OBJECT(current_object)->previous_floor_z_int <= OBJECT(current_object)->z_int)
	{
		PLAYER(current_player)->substate = 0u;
		PLAYER(current_player)->state = 9u;
	}
}

void add_player_to_lists(void)
{
	add_to_hitter_list();
	add_to_rider_list();
}

void remember_platform(void)
{
	OBJECT(current_object)->flags &= 0xFFFFFFEFu;
	if ((OBJECT(current_object)->flags & 8u) != 0u)
		OBJECT(current_object)->flags |= 0x10u;
}

void update_player_blink(void)
{
	if (PLAYER(current_player)->invulnerable_timer < 0)
	{
		OBJECT(current_object)->flags &= ~0x80000u;
		OBJECT(current_object)->flags &= 0xDFFFFFFFu;
		return;
	}
	if ((OBJECT(current_object)->flags & 0x20000000u) == 0u)
		return;
	toggle_hidden();
}

void toggle_hidden(void)
{
	OBJECT(current_object)->flags ^= 0x80000u;
}

void check_player_jump(void)
{
	uint8_t jump_delay = 0;

	if (players_frozen != 0u)
		return;
	if (OBJECT(current_object)->floor_type == 6u)
	{
		if ((uint8_t)PLAYER(current_player)->jump_delay != 2u)
		{
			PLAYER(current_player)->state = 0x17u;
			PLAYER(current_player)->substate = 0u;
			PLAYER(current_player)->ledge_side = -1;
		}
		return;
	}
	jump_delay = PLAYER(current_player)->jump_delay;
	PLAYER(current_player)->jump_delay = jump_delay - 1u;
	if ((int8_t)jump_delay >= 1)
		return;
	PLAYER(current_player)->jump_delay = 0;
	if (PLAYER(current_player)->state == 2u || PLAYER(current_player)->control_lock != 0u)
		return;
	start_jump_if_pressed();
}

void start_jump_if_pressed(void)
{
	uint32_t button_bit = 0;

	if ((uint8_t)PLAYER(current_player)->pushed_out != 0u || PLAYER(current_player)->path != 0u)
		return;
	if (game_in_progress != 0u)
	{
		button_bit = 0x10u;
		if (OBJECT(current_object)->player != 0u)
		{
			button_bit = 0x18u;
			if (OBJECT(current_object)->player != 1u)
				button_bit = 0x12u;
		}
	}
	else
	{
		button_bit = 0x19u;
		if (OBJECT(current_object)->player != 0u)
		{
			button_bit = 0x1Au;
			if (OBJECT(current_object)->player != 1u)
				button_bit = 0x1Bu;
		}
	}
	if ((inputs_pressed & shl32(1u, (int)(button_bit))) != 0u)
	{
		PLAYER(current_player)->state = 4u;
		PLAYER(current_player)->substate = 0u;
		PLAYER(current_player)->ledge_side = -1;
	}
}

void push_player_out_of_wall(uint32_t *p_angle, uint32_t *p_push_direction, uint32_t *p_record_index)
{
	uint32_t angle = *p_angle;
	uint16_t push_direction = (uint16_t)*p_push_direction;
	uint32_t record_index = *p_record_index;
	uint16_t drift_bounce_direction = 0, new_direction = 0;
	uint32_t rebound_direction = 0;
	uint8_t wall_timer = 0;

	if (PLAYER(current_player)->path_request != 0u || PLAYER(current_player)->path != 0u || all_players_path_request != 0u)
		return;
	if ((OBJECT(current_object)->flags & 0x20u) != 0u)
	{
		if (OBJECT(current_object)->wall != PLAYER(current_player)->last_wall)
		{
			PLAYER(current_player)->wall_timer = 0x1E;
			PLAYER(current_player)->last_wall = OBJECT(current_object)->wall;
		push_player_out_of_wall_push_back:
			if ((stage != 8u || scroll_position >= 0x4000u) && PLAYER(current_player)->state != 0x25u && PLAYER(current_player)->state != 0x26u)
				OBJECT(current_object)->flags |= 0x1000u;
			OBJECT(current_object)->x = PLAYER(current_player)->previous_x;
			OBJECT(current_object)->y = PLAYER(current_player)->previous_y;
			OBJECT(current_object)->floor_block = OBJECT(current_object)->previous_floor_block;
			OBJECT(current_object)->floor_z = OBJECT(current_object)->previous_floor_z;
			PLAYER(current_player)->control_lock = 0u;
		}
		else
		{
			wall_timer = PLAYER(current_player)->wall_timer;
			PLAYER(current_player)->wall_timer = wall_timer - 1u;
			if ((int8_t)wall_timer > 1)
				goto push_player_out_of_wall_push_back;
			PLAYER(current_player)->wall_timer = 0;
		}
		if (PLAYER(current_player)->state == 4u || PLAYER(current_player)->state == 0x17u)
		{
			rebound_direction = (angle & ~0xFFFFu) | OBJECT(LINKED(current_object, OBJECT(current_object)->wall))->direction;
			rebound_direction = (rebound_direction & ~0xFFFFu) | (uint16_t)(rebound_direction << 8);
			rebound_direction = (rebound_direction & ~0xFFFFu) | (uint16_t)(rebound_direction + 0x200u);
			rebound_direction = (rebound_direction & ~0xFFFFu) | (rebound_direction & 0x3FFu);
			OBJECT(current_object)->direction = rebound_direction;
			rebound_direction = face_movement_direction(rebound_direction);
			OBJECT(current_object)->speed = 0x4000000;
			OBJECT(current_object)->speed_z = -0x18000;
			PLAYER(current_player)->state_timer = 6;
			OBJECT(current_object)->flags |= 0x2000u;
			PLAYER(current_player)->control_lock = 0xAu;
			*p_angle = rebound_direction;
			return;
		}
		if (current_game_mode == 0xAu || PLAYER(current_player)->state == 0xDu || PLAYER(current_player)->state == 0x20u)
		{
			angle = 0u;
			OBJECT(current_object)->speed = 0;
			OBJECT(current_object)->speed_x = 0;
			OBJECT(current_object)->speed_y = 0;
			PLAYER(current_player)->free_turn_timer = 3u;
			OBJECT(current_object)->flags |= 0x800000u;
			PLAYER(current_player)->previous_state = PLAYER(current_player)->state;
			PLAYER(current_player)->state = 0u;
			*p_angle = angle;
			return;
		}
		if (PLAYER(current_player)->state == 0x25u || PLAYER(current_player)->state == 0x26u)
		{
			angle = (angle & ~0xFFFFu) | scroll_drift_direction;
			angle = (angle & ~0xFFFFu) | (uint16_t)(angle + 0x200u);
			angle = (angle & ~0xFFFFu) | (angle & 0x3FFu);
			drift_bounce_direction = sonic_bounce_records[AS8(OBJECT(LINKED(current_object, OBJECT(current_object)->wall))->direction) * 3u];
			angle = (angle & ~0xFFFFu) | (uint16_t)(angle - drift_bounce_direction);
			angle = (angle & ~0xFFFFu) | (angle & 0x3FFu);
			if ((int16_t)angle < 0x200)
				drift_bounce_direction -= 0x80u;
			else
				drift_bounce_direction += 0x80u;
			scroll_drift_direction = drift_bounce_direction & 0x3FFu;
		push_player_out_of_wall_deflect:
			angle = (angle & ~0xFFFFu) | OBJECT(current_object)->direction;
			angle = (angle & ~0xFFFFu) | (uint16_t)(angle + 0x200u);
			angle = (angle & ~0xFFFFu) | (angle & 0x3FFu);
			record_index = AS8(OBJECT(LINKED(current_object, OBJECT(current_object)->wall))->direction) * 3u;
			push_direction = sonic_bounce_records[record_index];
			new_direction = (uint16_t)push_direction;
			angle = (angle & ~0xFFFFu) | (uint16_t)(angle - push_direction);
			angle = (angle & ~0xFFFFu) | (angle & 0x3FFu);
			if ((int16_t)angle < 0x200)
				new_direction -= 0x80u;
			else
				new_direction += 0x80u;
			OBJECT(current_object)->direction = new_direction & 0x3FFu;
			PLAYER(current_player)->control_lock = 4u;
			if (stage != 8u || scroll_position >= 0x4000u)
			{
				*p_angle = angle;
				*p_push_direction = (*p_push_direction & 0xFFFF0000u) | push_direction;
				*p_record_index = record_index;
				return;
			}
			angle = (angle & ~0xFFFFu) | OBJECT(LINKED(current_object, OBJECT(current_object)->wall))->direction;
			angle = (angle & ~0xFFFFu) | (uint16_t)(angle << 8);
			angle = (angle & ~0xFFFFu) | (uint16_t)(angle + 0x200u);
			angle = (angle & ~0xFFFFu) | (angle & 0x3FFu);
			OBJECT(current_object)->direction = angle;
			*p_angle = angle;
			*p_push_direction = (*p_push_direction & 0xFFFF0000u) | push_direction;
			*p_record_index = record_index;
			return;
		}
		if (OBJECT(current_object)->floor_type == 4u || OBJECT(current_object)->speed_int >= 0x200)
			goto push_player_out_of_wall_deflect;
		angle = 0u;
		OBJECT(current_object)->speed = 0;
		OBJECT(current_object)->speed_x = 0;
		OBJECT(current_object)->speed_y = 0;
		PLAYER(current_player)->free_turn_timer = 3u;
		OBJECT(current_object)->flags |= 0x800000u;
		PLAYER(current_player)->previous_state = PLAYER(current_player)->state;
		PLAYER(current_player)->state = 0u;
		*p_angle = angle;
		return;
	}
	PLAYER(current_player)->last_wall = 0xFFFFu;
}

void player_steer_in_air(void)
{
	uint32_t ignored = 0, input_speed = 0, speed_diff = 0, trackball_y = 0;
	uint32_t speed = 0, target_speed = 0;

	read_player_trackball();
	input_speed = PLAYER(current_player)->trackball_x;
	trackball_y = PLAYER(current_player)->trackball_y;
	input_speed = input_speed * input_speed + trackball_y * trackball_y;
	square_root(&input_speed, &ignored);
	input_speed &= 0xFFFFu;
	input_speed = input_speed * input_speed;
	if ((int32_t)input_speed > 0x5000000)
		input_speed = 0x5000000u;
	speed = OBJECT(current_object)->speed;
	target_speed = input_speed;
	speed_diff = target_speed - speed;
	if ((int32_t)target_speed <= (int32_t)speed)
		OBJECT(current_object)->speed += (int32_t)speed_diff >> 5;
	else
		OBJECT(current_object)->speed += speed_diff;
	if ((PLAYER(current_player)->trackball_x | PLAYER(current_player)->trackball_y) != 0u && (OBJECT(current_object)->flags & 0x2000u) == 0u)
		set_target_heading_from_trackball();
	turn_player();
}

void player_steer(uint32_t wanted_speed)
{
	uint32_t carry = 0;
	uint8_t next_state = 0;
	uint16_t animation_speed_int = 0;
	uint32_t animation_speed_diff = 0, direction = 0, floor_slope = 0, ignored = 0, input_animation_speed = 0, input_y = 0, rounded_heading = 0, rounded_speed = 0, run_threshold = 0, slope_pair = 0, speed_diff = 0, speed_factor = 0, trackball_y = 0;
	uint32_t speed = 0, target_speed = 0, speed_2 = 0, target_speed_2 = 0, animation_speed = 0, target_animation_speed = 0;

	wanted_speed = follow_player_path(wanted_speed);
	if (PLAYER(current_player)->path != 0u)
	{
	player_steer_approach_speed:
		wanted_speed = apply_speed_scale(wanted_speed);
		if (OBJECT(current_object)->floor_type == 7u && PLAYER(current_player)->path == 0u)
		{
			speed = OBJECT(current_object)->speed;
			target_speed = wanted_speed;
			speed_diff = target_speed - speed;
			if ((int32_t)target_speed <= (int32_t)speed)
				OBJECT(current_object)->speed += (int32_t)speed_diff >> 6;
			else
				OBJECT(current_object)->speed += (int32_t)speed_diff >> 5;
		}
		else
		{
			speed_2 = OBJECT(current_object)->speed;
			target_speed_2 = wanted_speed;
			speed_diff = target_speed_2 - speed_2;
			if ((int32_t)target_speed_2 <= (int32_t)speed_2)
				OBJECT(current_object)->speed += (int32_t)speed_diff >> 5;
			else
				OBJECT(current_object)->speed += speed_diff;
		}
		if (OBJECT(current_object)->floor_type == 7u || (PLAYER(current_player)->push_x | PLAYER(current_player)->push_y) != 0u || PLAYER(current_player)->speed_scale != 0u)
		{
			if ((PLAYER(current_player)->trackball_x | PLAYER(current_player)->trackball_y) != 0u || (OBJECT(current_object)->floor_type != 7u && (int16_t)AT16(PLAYER(current_player)->animation_speed, 2) >= 0x20))
			{
				input_animation_speed = PLAYER(current_player)->trackball_x;
				trackball_y = PLAYER(current_player)->trackball_y;
				input_animation_speed = input_animation_speed * input_animation_speed + trackball_y * trackball_y;
				square_root(&input_animation_speed, &ignored);
				input_animation_speed &= 0xFFFFu;
				input_animation_speed = input_animation_speed * input_animation_speed;
				input_animation_speed += input_animation_speed >> 1;
				if ((int32_t)input_animation_speed > 0x5000000)
					input_animation_speed = 0x5000000u;
				animation_speed = PLAYER(current_player)->animation_speed;
				target_animation_speed = input_animation_speed;
				animation_speed_diff = target_animation_speed - animation_speed;
				if ((int32_t)target_animation_speed <= (int32_t)animation_speed)
					PLAYER(current_player)->animation_speed += (int32_t)animation_speed_diff >> 5;
				else
					PLAYER(current_player)->animation_speed += animation_speed_diff;
			}
			else
				PLAYER(current_player)->animation_speed = 0;
		}
		else
			PLAYER(current_player)->animation_speed = OBJECT(current_object)->speed;
		if (PLAYER(current_player)->state != 4u && PLAYER(current_player)->state != 0x17u && PLAYER(current_player)->state != 0x25u && PLAYER(current_player)->state != 0x26u && PLAYER(current_player)->state != 0x2Du)
		{
			next_state = 0u;
			animation_speed_int = AT16(PLAYER(current_player)->animation_speed, 2);
			if (animation_speed_int != 0u)
			{
				next_state = 1u;
				run_threshold = 0x200u;
				if (OBJECT(current_object)->floor_type == 4u)
					run_threshold = (run_threshold * 0x60u) >> 8;
				if ((int16_t)run_threshold < (int16_t)animation_speed_int)
				{
					next_state = 0x13u;
					if (OBJECT(current_object)->floor_type != 4u && (int16_t)animation_speed_int > 0x4F0)
						next_state = 0x14u;
				}
			}
			PLAYER(current_player)->previous_state = PLAYER(current_player)->state;
			PLAYER(current_player)->state = next_state;
		}
	}
	else
	{
		read_player_trackball();
		if (players_frozen != 0u)
		{
			PLAYER(current_player)->trackball_x = 0;
			PLAYER(current_player)->trackball_y = 0;
		}
		if (PLAYER(current_player)->state == 4u || PLAYER(current_player)->state == 0x17u || PLAYER(current_player)->state == 0x25u || PLAYER(current_player)->state == 0x26u || PLAYER(current_player)->state == 0x2Du || (PLAYER(current_player)->trackball_x | PLAYER(current_player)->trackball_y) != 0u || (PLAYER(current_player)->push_x | PLAYER(current_player)->push_y) != 0u)
		{
			if ((OBJECT(current_object)->flags & 0x2000u) == 0u)
				set_target_heading_from_trackball();
		player_steer_speed_from_trackball:
			wanted_speed = (uint32_t)PLAYER(current_player)->trackball_x + PLAYER(current_player)->push_x;
			input_y = (uint32_t)PLAYER(current_player)->trackball_y + PLAYER(current_player)->push_y;
			wanted_speed = wanted_speed * wanted_speed + input_y * input_y;
			square_root(&wanted_speed, &ignored);
			wanted_speed &= 0xFFFFu;
			wanted_speed = wanted_speed * wanted_speed;
			wanted_speed += wanted_speed >> 1;
			if ((int32_t)wanted_speed > 0x5000000)
				wanted_speed = 0x5000000u;
			if ((OBJECT(current_object)->flags & 0x80000000u) != 0u)
			{
				wanted_speed = wanted_speed >> 2;
				if ((PLAYER(current_player)->flags & 4u) != 0u)
				{
					wanted_speed = 0u;
					OBJECT(current_object)->speed = 0;
				}
			}
			if (OBJECT(current_object)->floor_block >= 0 && OBJECT(current_object)->floor_type == 0xAu)
			{
				PLAYER(current_player)->struggle_speed = wanted_speed >> 16;
				wanted_speed = (wanted_speed >> 10) * (uint16_t)(0x100u - ((uint16_t)PLAYER(current_player)->sink_depth >> 8) * 6u);
			}
			if (OBJECT(current_object)->floor_type != 4u)
				goto player_steer_approach_speed;
			wanted_speed = (wanted_speed >> 8) * 0x60u;
			goto player_steer_approach_speed;
		}
		if (OBJECT(current_object)->speed_int >= 0x20)
			goto player_steer_speed_from_trackball;
		OBJECT(current_object)->speed = 0;
		PLAYER(current_player)->previous_state = PLAYER(current_player)->state;
		PLAYER(current_player)->state = 0u;
	}
	turn_player();
	if ((OBJECT(current_object)->flags & 0x80000000u) == 0u)
	{
		rounded_heading = (uint16_t)(PLAYER(current_player)->heading + 0x20u) & 0x3FFu;
		PLAYER(current_player)->ground_speed = ((uint32_t)OBJECT(current_object)->speed >> 8) * heading_speed_factors[rounded_heading >> 6];
	}
	else
		PLAYER(current_player)->ground_speed = OBJECT(current_object)->speed;
	if (OBJECT(current_object)->floor_slope == 0u)
		return;
	floor_slope = OBJECT(current_object)->floor_slope;
	direction = OBJECT(current_object)->direction;
	slope_pair = slope_speed_direction_pairs_ADDRESS + floor_slope_direction_tables[(uint16_t)(floor_slope << 2)] + (direction >> 6) * 4u;
	speed_factor = ROM16(slope_pair);
	rounded_speed = PLAYER(current_player)->ground_speed;
	carry = (rounded_speed >> 11) & 1u;
	rounded_speed = rounded_speed >> 12;
	slope_pair += 2u;
	if (carry)
		rounded_speed++;
	PLAYER(current_player)->ground_speed = rounded_speed * speed_factor;
	OBJECT(current_object)->direction = ROM16(slope_pair);
}

void set_target_heading_from_trackball(void)
{
	uint16_t dy = 0;
	uint32_t ignored = 0, push_angle = 0, trackball_angle = 0, trackball_y = 0;

	if ((PLAYER(current_player)->trackball_x | PLAYER(current_player)->trackball_y) != 0u)
	{
		trackball_angle = (uint16_t)PLAYER(current_player)->trackball_x;
		dy = (uint16_t)((int32_t)(PLAYER(current_player)->trackball_y * 0x180u) >> 8);
		angle_of_vector(dy, &trackball_angle, &ignored);
		PLAYER(current_player)->facing_angle = trackball_angle;
	}
	if ((PLAYER(current_player)->push_x | PLAYER(current_player)->push_y) == 0u)
	{
		PLAYER(current_player)->target_heading = PLAYER(current_player)->facing_angle;
		return;
	}
	push_angle = (uint16_t)(PLAYER(current_player)->trackball_x + PLAYER(current_player)->push_x);
	trackball_y = PLAYER(current_player)->trackball_y;
	if ((OBJECT(current_object)->flags & 0x80000000u) == 0u)
		trackball_y = (int32_t)(trackball_y * 0x180u) >> 8;
	trackball_y = (uint16_t)(trackball_y + PLAYER(current_player)->push_y);
	angle_of_vector(trackball_y, &push_angle, &ignored);
	PLAYER(current_player)->target_heading = push_angle;
}

void turn_player(void)
{
	uint16_t max_turn = 0, turn = 0;
	uint32_t speed_int = 0;
	uint16_t heading_delta = 0;

	if (PLAYER(current_player)->free_turn_timer != 0u || PLAYER(current_player)->state == 2u || PLAYER(current_player)->state == 4u || PLAYER(current_player)->state == 0x17u || PLAYER(current_player)->state == 0x25u || PLAYER(current_player)->state == 0x26u || (OBJECT(current_object)->flags & 0x80000000u) != 0u)
		PLAYER(current_player)->heading = PLAYER(current_player)->target_heading;
	else
	{
		speed_int = (uint16_t)OBJECT(current_object)->speed_int;
		if ((int16_t)speed_int <= 0x200)
		{
			PLAYER(current_player)->heading = PLAYER(current_player)->target_heading;
			PLAYER(current_player)->heading &= 0x3FFu;
			set_heading_direction();
			return;
		}
		max_turn = (uint16_t)divide_s32(0x180000u, speed_int);
		heading_delta = ((uint32_t)PLAYER(current_player)->target_heading << 6) - ((uint32_t)PLAYER(current_player)->heading << 6);
		turn = heading_delta;
		if ((int16_t)heading_delta < 0)
		{
			turn = (uint16_t)(0u - turn);
			if (turn >= 0x4000u)
			{
				PLAYER(current_player)->substate = 0u;
				PLAYER(current_player)->previous_state = PLAYER(current_player)->state;
				PLAYER(current_player)->state = 0xCu;
				set_heading_direction();
				return;
			}
			if ((int16_t)max_turn < (int16_t)turn)
				turn = (uint16_t)max_turn;
			PLAYER(current_player)->heading -= turn >> 6;
		}
		else
		{
			if (turn >= 0x4000u)
			{
				PLAYER(current_player)->substate = 0u;
				PLAYER(current_player)->previous_state = PLAYER(current_player)->state;
				PLAYER(current_player)->state = 0xCu;
				set_heading_direction();
				return;
			}
			if ((int16_t)max_turn < (int16_t)turn)
				turn = (uint16_t)max_turn;
			PLAYER(current_player)->heading += turn >> 6;
		}
	}
	PLAYER(current_player)->heading &= 0x3FFu;
	set_heading_direction();
}

void set_heading_direction(void)
{
	uint32_t snapped_heading = 0;

	snapped_heading = (PLAYER(current_player)->heading + 0x20u) & 0x3C0u;
	PLAYER(current_player)->heading_snapped = snapped_heading;
	OBJECT(current_object)->direction = (snapped_heading + 0x80u) & 0x3C0u;
	snapped_heading = set_facing(snapped_heading);
}

uint32_t set_facing(uint32_t facing)
{
	uint32_t new_facing = 0;

	new_facing = (facing & ~0xFFFFu) | PLAYER(current_player)->facing_angle;
	new_facing = (new_facing & ~0xFFFFu) | (uint16_t)(new_facing + 0x20u);
	new_facing = (new_facing & ~0xFFFFu) | ((uint16_t)new_facing >> 6);
	new_facing = (new_facing & ~0xFFFFu) | (new_facing & 0xFu);
	PLAYER(current_player)->facing = new_facing;
	return new_facing;
}

uint32_t face_movement_direction(uint32_t facing)
{
	uint32_t new_heading = 0;

	new_heading = (facing & ~0xFFFFu) | OBJECT(current_object)->direction;
	new_heading = (new_heading & ~0xFFFFu) | (uint16_t)(new_heading - 0xA0u);
	new_heading = (new_heading & ~0xFFFFu) | (new_heading & 0x3FFu);
	PLAYER(current_player)->target_heading = new_heading;
	PLAYER(current_player)->heading = new_heading;
	PLAYER(current_player)->facing_angle = new_heading;
	return set_facing(new_heading);
}

uint32_t compute_ground_speed(void)
{
	uint32_t carry = 0;
	uint32_t r1 = 0, direction = 0, floor_slope = 0, rounded_heading = 0, rounded_speed = 0, slope_pair = 0, speed_factor = 0;

	rounded_heading = (uint16_t)(PLAYER(current_player)->heading + 0x20u) & 0x3FFu;
	r1 = heading_speed_factors[rounded_heading >> 6];
	PLAYER(current_player)->ground_speed = ((uint32_t)OBJECT(current_object)->speed >> 8) * r1;
	if (OBJECT(current_object)->floor_slope == 0u)
		return r1;
	floor_slope = OBJECT(current_object)->floor_slope;
	r1 = floor_slope_direction_tables[(uint16_t)(floor_slope << 2)];
	direction = OBJECT(current_object)->direction;
	slope_pair = slope_speed_direction_pairs_ADDRESS + r1 + (direction >> 6) * 4u;
	speed_factor = ROM16(slope_pair);
	rounded_speed = PLAYER(current_player)->ground_speed;
	carry = (rounded_speed >> 11) & 1u;
	rounded_speed = rounded_speed >> 12;
	slope_pair += 2u;
	if (carry)
		rounded_speed++;
	PLAYER(current_player)->ground_speed = rounded_speed * speed_factor;
	OBJECT(current_object)->direction = ROM16(slope_pair);
	return r1;
}

void read_player_trackball(void)
{
	uint16_t axis_value = 0;
	uint32_t trackball = 0;

	trackball = trackball_1_x_ADDRESS + OBJECT(current_object)->player * 4u;
	axis_value = mem_read16(trackball);
	trackball += 2u;
	if ((int16_t)axis_value >= 0)
		axis_value = (uint16_t)((axis_value & ~1u) << 8);
	else
		axis_value = (uint16_t)(0u - (((0u - axis_value) & 0xFFFEu) << 8));
	PLAYER(current_player)->trackball_x = axis_value;
	axis_value = mem_read16(trackball);
	if ((int16_t)axis_value >= 0)
		axis_value = (uint16_t)((axis_value & ~1u) << 8);
	else
		axis_value = (uint16_t)(0u - (((0u - axis_value) & 0xFFFEu) << 8));
	PLAYER(current_player)->trackball_y = axis_value;
}

void shadow_1_start(void)
{
	OBJECT(current_object)->player = 0u;
	shadow_start();
}

void shadow_2_start(void)
{
	OBJECT(current_object)->player = 1u;
	shadow_start();
}

void shadow_3_start(void)
{
	OBJECT(current_object)->player = 2u;
	shadow_start();
}

void shadow_start(void)
{
	uint32_t free_count = 0, ignored = 0, object = 0, object_2 = 0;

	OBJECT(current_object)->sprite_flags = 0x1008u;
	OBJECT(current_object)->sprite_palette = 0x1FFEu;
	OBJECT(current_object)->sprite_frame = 0x5D90u;
	OBJECT(current_object)->sprite_scale = 0x100u;
	OBJECT(current_object)->flags = 0u;
	OBJECT(current_object)->sprite_layer = 0u;
	OBJECT(current_object)->z = 0;
	OBJECT(current_object)->shadow.flicker_timer = 0;
	OBJECT(current_object)->sprite_offset_x = 0;
	OBJECT(current_object)->sprite_offset_y = -2;
	OBJECT(current_object)->animation_frame = 0u;
	OBJECT(current_object)->animation_speed = 2u;
	OBJECT(current_object)->animation_timer = 0;
	OBJECT(current_object)->handler = shadow_update_ADDRESS;
	OBJECT(current_object)->child = 0xFFFFu;
	allocate_object_2(&free_count, &ignored, &object);
	if (free_count != 0u)
	{
		OBJECT(object)->sprite_flags = 0x1004u;
		OBJECT(object)->parent = current_object;
		OBJECT(current_object)->child = object;
		OBJECT(object)->child = 0xFFFFu;
		if (stage == 9u)
		{
			object_2 = object;
			allocate_object_2(&free_count, &ignored, &object);
			if (free_count != 0u)
			{
				OBJECT(object)->sprite_flags = 0x1088u;
				OBJECT(object)->parent = object_2;
				OBJECT(object_2)->child = object;
			}
		}
	}
}

void shadow_update(void)
{
	uint16_t priority_bits = 0;
	uint32_t r1 = 0, r7 = 0, r8 = 0, depth = 0, ignored = 0, offset_y = 0, player = 0, point_x = 0, point_y = 0, scaled_offset_y = 0, scaled_player = 0, scene_player = 0, sprite_scale = 0, z = 0;
	uint8_t flicker_timer = 0;

	player = PLAYER_BLOCK(1) + ((uint32_t)OBJECT(current_object)->player << 7) * 4u;
	current_player = LINKED(current_player, PLAYER(player)->object);
	PLAYER(player)->flags &= ~1u;
	if ((PLAYER(player)->flags & 8u) != 0u || PLAYER(player)->state == 0xBu || PLAYER(player)->state == 0x23u || PLAYER(player)->state == 0x24u || PLAYER(player)->state == 0x16u || PLAYER(player)->state == 0x2Eu || PLAYER(player)->state == 0x37u)
		return;
	if (PLAYER(player)->state == 0x10u)
	{
		draw_attack_effect(player);
		return;
	}
	if (PLAYER(player)->state == 0x11u || PLAYER(player)->state == 0x12u || PLAYER(player)->state == 0x15u || PLAYER(player)->state == 0x1Du || PLAYER(player)->state == 0x27u || PLAYER(player)->state == 0x2Du || PLAYER(player)->state == 0x34u || PLAYER(player)->state == 0x35u || PLAYER(player)->state == 0x39u || (PLAYER(player)->state == 0x1Eu && (uint8_t)intro_character != OBJECT(current_player)->kind) || (PLAYER(player)->state == 0x32u && (int8_t)PLAYER(player)->substate < 3) || (PLAYER(player)->state == 0x25u && ((PLAYER(player)->sequence_step >= 8 && PLAYER(player)->sequence_step <= 0xC) || PLAYER(player)->sequence_step == 0xEu)) || (OBJECT(current_player)->flags & 0x10000000u) != 0u || (current_game_mode == 0xAu && (int8_t)scene_step >= 6))
		return;
	if (current_game_mode != 0x10u)
	{
		if (stage != 9u || (OBJECT(current_player)->floor_type != 8u && (OBJECT(current_player)->floor_type != 7u || (OBJECT(current_player)->flags & 0x10u) != 0u)))
		{
			if (PLAYER(player)->state == 0x28u || (OBJECT(current_player)->floor_type != 0xAu && (OBJECT(current_player)->flags & 0x80000000u) == 0u && OBJECT(current_player)->floor_type != 0xBu))
			{
				if (PLAYER(player)->state == 0x33u)
				{
					OBJECT(current_object)->projected_x = OBJECT(current_player)->projected_x;
					OBJECT(current_object)->projected_y = OBJECT(current_player)->projected_y;
					OBJECT(current_object)->z = OBJECT(current_player)->floor_z;
					if (OBJECT(current_player)->floor_block < 0)
						OBJECT(current_object)->z = OBJECT(current_player)->previous_floor_z;
					OBJECT(current_object)->z_int += 8u;
					OBJECT(current_object)->sprite_flags = 0x1004u;
					OBJECT(current_object)->flags &= ~0x80000u;
					OBJECT(current_object)->sprite_palette = 0x2D50u;
					priority_bits = OBJECT(current_player)->sprite_palette & 0x6000u;
					OBJECT(current_object)->sprite_palette &= 0x9FFFu;
					OBJECT(current_object)->sprite_palette |= priority_bits;
					OBJECT(current_object)->animation = pit_sinking_shadow_animation_ADDRESS;
					animate(&offset_y, &r7, &r8);
					update_screen_position(&ignored, &offset_y);
					depth = (uint16_t)(OBJECT(current_object)->screen_y + 0x10u);
					queue_sprite_at_depth(&ignored, &depth);
					draw_attack_effect(player);
					return;
				}
				if (player_status[OBJECT(current_object)->player] == 0x82u)
					return;
				if (OBJECT(current_player)->floor_block < 0)
				{
					draw_attack_effect(player);
					return;
				}
				OBJECT(current_object)->projected_x = OBJECT(current_player)->projected_x;
				OBJECT(current_object)->projected_y = OBJECT(current_player)->projected_y;
				OBJECT(current_object)->projected_y_int -= 3u;
				OBJECT(current_object)->z = OBJECT(current_player)->floor_z;
				OBJECT(current_object)->sprite_flags = 0x1008u;
				OBJECT(current_object)->sprite_frame = 0x5D90u;
				OBJECT(current_object)->flags &= ~0x80000u;
				if (OBJECT(current_player)->floor_type == 9u)
				{
				shadow_update_flicker:
					OBJECT(current_object)->sprite_palette = 0x87A0u;
					flicker_timer = OBJECT(current_object)->shadow.flicker_timer;
					OBJECT(current_object)->shadow.flicker_timer = flicker_timer - 1u;
					if ((int8_t)flicker_timer <= 1)
					{
						OBJECT(current_object)->shadow.flicker_timer = 2;
						OBJECT(current_object)->flags |= 0x80000u;
					}
				}
				else
				{
					if (OBJECT(current_player)->floor_type == 4u || (stage == 0x10u && mem_read8(floor_blocks + 2u + OBJECT(current_player)->floor_block + 0x12u) == 0x20u))
					{
						OBJECT(current_object)->sprite_palette = 0x7A0u;
						flicker_timer = OBJECT(current_object)->shadow.flicker_timer;
						OBJECT(current_object)->shadow.flicker_timer = flicker_timer - 1u;
						if ((int8_t)flicker_timer <= 1)
						{
							OBJECT(current_object)->shadow.flicker_timer = 2;
							OBJECT(current_object)->flags |= 0x80000u;
						}
						priority_bits = OBJECT(current_player)->sprite_palette & 0x6000u;
						OBJECT(current_object)->sprite_palette &= 0x9FFFu;
						OBJECT(current_object)->sprite_palette |= priority_bits;
						update_screen_position(&ignored, &offset_y);
						if (per_player_view_mode != 0u && scroll_position >= 0xA5F0u)
						{
							offset_y = (offset_y & ~0xFFFFu) | (uint16_t)OBJECT(current_object)->sprite_offset_y;
							OBJECT(current_object)->screen_y = ((int32_t)((uint32_t)OBJECT(current_object)->projected_y + OBJECT(current_object)->z) >> 16) - PLAYER(player)->own_view_y + offset_y;
						}
						if ((OBJECT(current_object)->flags & 0x80000u) == 0u)
							queue_sprite(offset_y);
						draw_attack_effect(player);
						return;
					}
					if ((AS16(current_game_mode) == 0x1Au && (int8_t)PLAYER(player)->sequence_step >= 2) || (OBJECT(current_player)->flags & 0x10u) != 0u)
						goto shadow_update_flicker;
					OBJECT(current_object)->sprite_palette = 0x1FFEu;
				}
				priority_bits = OBJECT(current_player)->sprite_palette & 0x6000u;
				OBJECT(current_object)->sprite_palette &= 0x9FFFu;
				OBJECT(current_object)->sprite_palette |= priority_bits;
				update_screen_position(&ignored, &offset_y);
				if (per_player_view_mode != 0u && scroll_position >= 0xA5F0u)
				{
					offset_y = (offset_y & ~0xFFFFu) | (uint16_t)OBJECT(current_object)->sprite_offset_y;
					OBJECT(current_object)->screen_y = ((int32_t)((uint32_t)OBJECT(current_object)->projected_y + OBJECT(current_object)->z) >> 16) - PLAYER(player)->own_view_y + offset_y;
				}
				if ((OBJECT(current_object)->flags & 0x80000u) != 0u)
				{
					draw_attack_effect(player);
					return;
				}
				queue_sprite(offset_y);
				draw_attack_effect(player);
				return;
			}
			if (PLAYER(player)->state == 6u || PLAYER(player)->state == 8u || PLAYER(player)->state == 7u || PLAYER(player)->state == 9u)
			{
				draw_attack_effect(player);
				return;
			}
			PLAYER(player)->flags |= 1u;
			OBJECT(current_object)->projected_x = OBJECT(current_player)->projected_x;
			OBJECT(current_object)->projected_y = OBJECT(current_player)->projected_y;
			OBJECT(current_object)->z = OBJECT(current_player)->floor_z;
			if (OBJECT(current_player)->floor_block < 0)
				OBJECT(current_object)->z = OBJECT(current_player)->previous_floor_z;
			if (OBJECT(current_player)->floor_block < 0)
				z = OBJECT(current_player)->previous_floor_z;
			else
				z = OBJECT(current_player)->floor_z;
			point_x = OBJECT(current_player)->x;
			point_y = OBJECT(current_player)->y;
			project_point(z, &r1, &point_x, &point_y);
			PLAYER(player)->ground_clip_bottom = point_y;
			OBJECT(current_object)->sprite_flags = 0u;
			OBJECT(current_object)->flags &= ~0x80000u;
			OBJECT(current_object)->sprite_palette = 0x1020u;
			OBJECT(current_object)->sprite_scale = 0x120u;
			priority_bits = OBJECT(current_player)->sprite_palette & 0x6000u;
			OBJECT(current_object)->sprite_palette &= 0x9FFFu;
			OBJECT(current_object)->sprite_palette |= priority_bits;
			OBJECT(current_object)->animation = sinking_shadow_animation_ADDRESS;
			animate(&r1, &point_x, &point_y);
			update_screen_position(&ignored, &r1);
			depth = (uint16_t)(OBJECT(current_object)->screen_y + 0x10u);
			queue_sprite_at_depth(&ignored, &depth);
			draw_attack_effect(player);
			return;
		}
		if (OBJECT(current_player)->floor_block >= 0)
		{
			OBJECT(current_object)->projected_x = OBJECT(current_player)->projected_x;
			OBJECT(current_object)->projected_y = OBJECT(current_player)->projected_y;
			OBJECT(current_object)->z = (uint32_t)OBJECT(current_player)->floor_z - OBJECT(current_player)->z + OBJECT(current_player)->floor_z;
			OBJECT(current_object)->sprite_frame = OBJECT(current_player)->sprite_frame;
			OBJECT(current_object)->sprite_palette = OBJECT(current_player)->sprite_palette;
			OBJECT(current_object)->sprite_palette &= 0x1FFFu;
			OBJECT(current_object)->sprite_palette |= 0x2000u;
			OBJECT(current_object)->sprite_flags = 0x1188u;
			OBJECT(current_object)->sprite_flags |= OBJECT(current_player)->sprite_flags & 0x40u;
			update_screen_position(&ignored, &offset_y);
			depth = (uint16_t)(OBJECT(current_object)->screen_y - 0x18u);
			queue_sprite_at_depth(&ignored, &depth);
		}
		draw_attack_effect(player);
		return;
	}
	switch (scene_step)
	{
	case 0:
	case 1:
	case 4:
		OBJECT(current_object)->sprite_flags = 0u;
		OBJECT(current_object)->sprite_scale = 0x200u;
		OBJECT(current_object)->sprite_palette = 0x160u;
		scene_player = PLAYER_BLOCK(1) + ((uint32_t)OBJECT(current_object)->player << 7) * 4u;
		scene_player = LINKED(scene_player, PLAYER(scene_player)->object);
		OBJECT(current_object)->screen_x = OBJECT(scene_player)->screen_x;
		OBJECT(current_object)->screen_y = OBJECT(scene_player)->screen_y;
		OBJECT(current_object)->screen_y -= 8u;
		depth = (uint16_t)(OBJECT(current_object)->screen_y - 8u);
		queue_sprite_at_depth(&ignored, &depth);
		return;
	case 2:
		OBJECT(current_object)->sprite_flags = 0u;
		OBJECT(current_object)->sprite_frame = 0x1B60u;
		OBJECT(current_object)->sprite_palette = 0x8130u;
		OBJECT(current_object)->sprite_width = 0;
		OBJECT(current_object)->sprite_height = 8;
		return;
	case 3:
		scaled_player = PLAYER_BLOCK(1) + ((uint32_t)OBJECT(current_object)->player << 7) * 4u;
		scaled_player = LINKED(scaled_player, PLAYER(scaled_player)->object);
		OBJECT(current_object)->sprite_scale = OBJECT(scaled_player)->sprite_scale;
		OBJECT(current_object)->screen_x = OBJECT(scaled_player)->screen_x;
		OBJECT(current_object)->screen_y = OBJECT(scaled_player)->screen_y;
		sprite_scale = OBJECT(current_object)->sprite_scale;
		OBJECT(current_object)->screen_x += (int32_t)(OBJECT(current_object)->sprite_width * sprite_scale) >> 8;
		scaled_offset_y = (int32_t)(OBJECT(current_object)->sprite_height * sprite_scale) >> 8;
		OBJECT(current_object)->screen_y += scaled_offset_y;
		scaled_offset_y = queue_sprite(scaled_offset_y);
		return;
	default:
		unknown_code(0x6E3Eu + scene_step * 3u);
	}
}

void draw_attack_effect(uint32_t player)
{
	uint16_t palette = 0, priority_bits = 0;
	uint32_t depth = 0, effect_record = 0, facing_offset = 0, ignored = 0, object = 0, object_2 = 0;

	if ((OBJECT(current_player)->flags & 0x40u) == 0u || PLAYER(player)->attack_power <= 2 || PLAYER(player)->state == 9u || PLAYER(player)->state == 0xAu || (OBJECT(current_player)->flags & 0x80000000u) != 0u || (OBJECT(current_player)->flags & 0x20000u) != 0u || (int16_t)OBJECT(current_object)->child < 0)
		return;
	object = LINKED(current_object, OBJECT(current_object)->child);
	effect_record = attack_power_3_effect_animations_ADDRESS;
	if (PLAYER(player)->attack_power > 3)
		effect_record = attack_power_4_effect_animations_ADDRESS;
	facing_offset = ROM16(effect_record + PLAYER(player)->facing * 2u);
	effect_record = effect_record + facing_offset + 2u + OBJECT(current_player)->animation_frame * 2u * 2u;
	OBJECT(object)->sprite_flags = (OBJECT(object)->sprite_flags & ~0xC0u) | ROM16(effect_record);
	OBJECT(object)->sprite_frame = ROM16(effect_record + 2u);
	palette = 0x8320u;
	if (OBJECT(current_player)->kind != 0u)
	{
		palette = 0x8700u;
		if (OBJECT(current_player)->kind != 1u)
			palette = 0x8710u;
	}
	OBJECT(object)->sprite_palette = palette;
	priority_bits = OBJECT(current_player)->sprite_palette & 0x6000u;
	OBJECT(object)->sprite_palette &= 0x9FFFu;
	OBJECT(object)->sprite_palette |= priority_bits;
	OBJECT(object)->screen_x = OBJECT(current_player)->screen_x;
	OBJECT(object)->screen_y = OBJECT(current_player)->screen_y;
	current_object = (current_object & ~0xFFFFu) | (uint16_t)object;
	depth = (uint16_t)OBJECT(current_object)->screen_y;
	depth = (uint16_t)(depth + 8u);
	queue_sprite_at_depth(&ignored, &depth);
	current_object = (current_object & ~0xFFFFu) | (uint16_t)object_cursor;
	if (OBJECT(current_player)->floor_block >= 0 && (int16_t)OBJECT(object)->child >= 0 && OBJECT(current_player)->floor_type == 7u && (OBJECT(current_player)->flags & 0x10u) == 0u)
	{
		object_2 = object;
		object = LINKED(object, OBJECT(object)->child);
		OBJECT(object)->screen_x = OBJECT(current_object)->screen_x;
		OBJECT(object)->screen_y = OBJECT(current_object)->screen_y;
		OBJECT(object)->sprite_frame = OBJECT(object_2)->sprite_frame;
		OBJECT(object)->sprite_flags &= ~0x40u;
		OBJECT(object)->sprite_flags |= OBJECT(object_2)->sprite_flags & 0x40u;
		OBJECT(object)->sprite_palette = OBJECT(object_2)->sprite_palette;
		priority_bits = OBJECT(object_cursor)->sprite_palette & 0xE000u;
		OBJECT(object)->sprite_palette &= 0x1FFFu;
		OBJECT(object)->sprite_palette |= priority_bits;
		current_object = (current_object & ~0xFFFFu) | (uint16_t)object;
		queue_sprite(depth);
		current_object = (current_object & ~0xFFFFu) | (uint16_t)object_cursor;
	}
}

void side_of_box(uint32_t other, uint32_t *p_hitbox_offset_y, uint32_t *p_sides)
{
	uint16_t box_start = 0, hitbox_offset_y = 0, sides = 0;

	sides = 0u;
	box_start = (uint16_t)(OBJECT(other)->x_int + OBJECT(other)->hitbox_offset_x);
	if ((int16_t)box_start < OBJECT(current_object)->x_int)
	{
		if ((int16_t)(box_start + OBJECT(other)->hitbox_size_x) <= OBJECT(current_object)->x_int)
			sides |= 4u;
	}
	else
		sides |= 8u;
	hitbox_offset_y = (uint16_t)OBJECT(other)->hitbox_offset_y;
	box_start = (uint16_t)(OBJECT(other)->y_int + hitbox_offset_y);
	if ((int16_t)box_start < OBJECT(current_object)->y_int)
	{
		if (OBJECT(current_object)->y_int < (int16_t)(box_start + OBJECT(other)->hitbox_size_y))
		{
			*p_hitbox_offset_y = (*p_hitbox_offset_y & 0xFFFF0000u) | hitbox_offset_y;
			*p_sides = (*p_sides & 0xFFFF0000u) | sides;
			return;
		}
		sides |= 1u;
		*p_hitbox_offset_y = (*p_hitbox_offset_y & 0xFFFF0000u) | hitbox_offset_y;
		*p_sides = (*p_sides & 0xFFFF0000u) | sides;
		return;
	}
	sides |= 2u;
	*p_hitbox_offset_y = (*p_hitbox_offset_y & 0xFFFF0000u) | hitbox_offset_y;
	*p_sides = (*p_sides & 0xFFFF0000u) | sides;
}

uint32_t set_speed_from_direction(void)
{
	uint32_t cos_speed = 0, direction = 0, speed_int = 0;

	direction = (int16_t)OBJECT(current_object)->direction;
	speed_int = (uint16_t)OBJECT(current_object)->speed_int;
	OBJECT(current_object)->speed_x = (int32_t)(sine_table[direction] * speed_int) >> 6;
	cos_speed = (int32_t)(cosine_table[direction] * speed_int) >> 6;
	OBJECT(current_object)->speed_y = 0u - cos_speed;
	return cos_speed;
}

void polar_to_xy(uint32_t *p_cos_part, uint32_t *p_angle, uint32_t *p_length)
{
	uint16_t angle = (uint16_t)*p_angle;
	uint16_t length = (uint16_t)*p_length;
	uint32_t angle_index = 0, cos_part = 0, dx = 0, dy = 0, magnitude = 0;

	angle_index = (int16_t)angle;
	magnitude = (uint16_t)length;
	dx = (int32_t)(sine_table[angle_index] * magnitude) >> 6;
	cos_part = (int32_t)(cosine_table[angle_index] * magnitude) >> 6;
	dy = 0u - cos_part;
	*p_cos_part = cos_part;
	*p_angle = dx;
	*p_length = dy;
}

uint32_t set_speed_from_player_speed(void)
{
	uint32_t cos_speed = 0, direction = 0, ground_speed_int = 0;

	direction = (int16_t)OBJECT(current_object)->direction;
	ground_speed_int = AT16(PLAYER(current_player)->ground_speed, 2);
	OBJECT(current_object)->speed_x = (int32_t)(sine_table[direction] * ground_speed_int) >> 6;
	cos_speed = (int32_t)(cosine_table[direction] * ground_speed_int) >> 6;
	OBJECT(current_object)->speed_y = 0u - cos_speed;
	return cos_speed;
}

void project_object(uint32_t *p_screen_y, uint32_t *p_depth)
{
	uint32_t projected_x = 0, projected_y = 0, screen_y = 0;

	projected_y = OBJECT(current_object)->y;
	projected_x = ((int32_t)(OBJECT(current_object)->x + projected_y) >> 8) * 0xB5u;
	projected_y = ((int32_t)(projected_y - OBJECT(current_object)->x) >> 9) * 0xB5u;
	OBJECT(current_object)->projected_x = (int32_t)projected_x >> 16;
	OBJECT(current_object)->projected_y = projected_y;
	update_screen_position(&screen_y, &projected_y);
	*p_screen_y = screen_y;
	*p_depth = projected_y;
}

void update_screen_position(uint32_t *p_screen_y, uint32_t *p_depth)
{
	uint16_t sprite_offset_y = 0;
	uint32_t screen_y = 0;

	OBJECT(current_object)->screen_x = OBJECT(current_object)->projected_x - view_x + OBJECT(current_object)->sprite_offset_x;
	screen_y = (int32_t)((uint32_t)OBJECT(current_object)->projected_y + OBJECT(current_object)->z) >> 16;
	screen_y = (screen_y & ~0xFFFFu) | (uint16_t)(screen_y - view_y);
	sprite_offset_y = (uint16_t)OBJECT(current_object)->sprite_offset_y;
	screen_y = (screen_y & ~0xFFFFu) | (uint16_t)(screen_y + sprite_offset_y);
	OBJECT(current_object)->screen_y = screen_y;
	*p_screen_y = screen_y;
	*p_depth = (*p_depth & 0xFFFF0000u) | sprite_offset_y;
}

void project_point(uint32_t z, uint32_t *p_projected_y, uint32_t *p_point_x, uint32_t *p_point_y)
{
	uint32_t point_x = *p_point_x;
	uint32_t point_y = *p_point_y;
	uint32_t projected_x_fixed = 0, world_y = 0;

	world_y = point_y;
	projected_x_fixed = ((int32_t)(point_x + world_y) >> 8) * 0xB5u;
	world_y = ((int32_t)(world_y - point_x) >> 9) * 0xB5u;
	projected_x_fixed = (int32_t)projected_x_fixed >> 16;
	point_to_screen((uint16_t)projected_x_fixed, z, &world_y, &point_x, &point_y);
	*p_projected_y = world_y;
	*p_point_x = (*p_point_x & 0xFFFF0000u) | (point_x & 0xFFFFu);
	*p_point_y = (*p_point_y & 0xFFFF0000u) | (point_y & 0xFFFFu);
}

void point_to_screen(uint16_t projected_x, uint32_t z, uint32_t *p_projected_y, uint32_t *p_screen_x, uint32_t *p_screen_y)
{
	uint32_t projected_y = *p_projected_y;
	uint16_t screen_x = 0, screen_y = 0;

	screen_x = (uint16_t)(projected_x - view_x);
	projected_y = (int32_t)(projected_y + z) >> 16;
	projected_y = (projected_y & ~0xFFFFu) | (uint16_t)(projected_y - view_y);
	screen_y = (uint16_t)projected_y;
	*p_projected_y = projected_y;
	*p_screen_x = (*p_screen_x & 0xFFFF0000u) | screen_x;
	*p_screen_y = (*p_screen_y & 0xFFFF0000u) | screen_y;
}

uint32_t project_object_2(void)
{
	uint32_t offset_x = 0, offset_y = 0, projected_x = 0, rotated_y = 0;

	rotated_y = OBJECT(current_object)->y;
	projected_x = ((int32_t)(OBJECT(current_object)->x + rotated_y) >> 8) * 0xB5u;
	rotated_y = ((int32_t)(rotated_y - OBJECT(current_object)->x) >> 9) * 0xB5u;
	OBJECT(current_object)->projected_x = (int32_t)projected_x >> 16;
	OBJECT(current_object)->projected_y = rotated_y;
	offset_x = (rotated_y & ~0xFFFFu) | (uint16_t)OBJECT(current_object)->sprite_offset_x;
	OBJECT(current_object)->screen_x = OBJECT(current_object)->projected_x - view_x + offset_x;
	if (scroll_position < 0xA5F0u)
	{
		offset_y = (offset_x & ~0xFFFFu) | (uint16_t)OBJECT(current_object)->sprite_offset_y;
		OBJECT(current_object)->screen_y = ((int32_t)((uint32_t)OBJECT(current_object)->projected_y + OBJECT(current_object)->z) >> 16) - view_y + offset_y;
		return offset_y;
	}
	offset_y = (offset_x & ~0xFFFFu) | (uint16_t)OBJECT(current_object)->sprite_offset_y;
	OBJECT(current_object)->screen_y = ((int32_t)((uint32_t)OBJECT(current_object)->projected_y + OBJECT(current_object)->z) >> 16) - PLAYER(current_player)->own_view_y + offset_y;
	return offset_y;
}

void find_nearby_floor_blocks(uint32_t *p_corner_y, uint32_t *p_floor_block, uint32_t *p_bounds_offset, uint32_t *p_nearby_count, uint32_t *p_corners_left, uint32_t *p_left, uint32_t *p_right)
{
	uint16_t corner_y = (uint16_t)*p_corner_y;
	uint16_t floor_block = (uint16_t)*p_floor_block;
	uint32_t bounds_offset = *p_bounds_offset;
	uint16_t nearby_count = (uint16_t)*p_nearby_count;
	uint32_t corners_left = *p_corners_left;
	uint16_t left = (uint16_t)*p_left;
	uint16_t right = (uint16_t)*p_right;
	uint32_t carry = 0;
	uint16_t bottom = 0, corner_x = 0, top = 0;
	uint32_t blocks_left = 0, bounds_cursor = 0, bounds_list = 0, list_cursor = 0;
	uint16_t box_left = 0, box_right = 0, box_top = 0;

	if (nearby_floor_blocks_locked != 0u)
		return;
	left = (uint16_t)view_x;
	right = (uint16_t)left;
	left -= 0x80u;
	right += 0x1C0u;
	top = (uint16_t)view_y;
	bottom = (uint16_t)top;
	top -= 0x80u;
	bottom += 0x160u;
	bounds_list = floor_block_bounds;
	blocks_left = ROM16(bounds_list);
	list_cursor = nearby_floor_blocks_ADDRESS;
	bounds_offset = 0u;
	nearby_count = 0u;
	floor_block = 0u;
	bounds_list += 2u;
	do
	{
		bounds_cursor = bounds_list + bounds_offset;
		box_left = ROM16(bounds_cursor);
		bounds_cursor += 2u;
		if ((int16_t)box_left <= (int16_t)right)
		{
			bounds_cursor += 2u;
			box_right = ROM16(bounds_cursor);
			bounds_cursor += 2u;
			if ((int16_t)left <= (int16_t)box_right)
			{
				bounds_cursor += 4u;
				box_top = ROM16(bounds_cursor);
				bounds_cursor += 2u;
				if ((int16_t)box_top <= (int16_t)bottom && (int16_t)top <= (int16_t)ROM16(bounds_cursor + 2u))
				{
					bounds_cursor = bounds_list + bounds_offset;
					corners_left = 4u;
					do
					{
						corner_x = ROM16(bounds_cursor);
						bounds_cursor += 2u;
						corner_y = ROM16(bounds_cursor);
						corner_x -= left;
						carry = corner_x < 0x240u;
						bounds_cursor += 2u;
						if (corner_x < 0x240u)
						{
							corner_y -= top;
							carry = corner_y < 0x1E0u;
						}
						corners_left--;
					} while ((corners_left != 0u) & (carry == 0u));
					if (carry)
					{
						nearby_count++;
						mem_write16(list_cursor, floor_block);
						list_cursor += 2u;
					}
				}
			}
		}
		bounds_offset = (uint16_t)(bounds_offset + 0x10u);
		floor_block += 0x16u;
		blocks_left--;
	} while (blocks_left != 0u);
	nearby_floor_block_count = nearby_count;
	*p_corner_y = (*p_corner_y & 0xFFFF0000u) | corner_y;
	*p_floor_block = (*p_floor_block & 0xFFFF0000u) | floor_block;
	*p_bounds_offset = bounds_offset;
	*p_nearby_count = (*p_nearby_count & 0xFFFF0000u) | nearby_count;
	*p_corners_left = corners_left;
	*p_left = (*p_left & 0xFFFF0000u) | left;
	*p_right = (*p_right & 0xFFFF0000u) | right;
}

void find_floor(uint32_t *p_x, uint32_t *p_y, uint32_t *p_block_offset, uint32_t *p_slope_height, uint32_t *p_cursor)
{
	uint32_t x = *p_x;
	uint32_t y = *p_y;
	uint32_t block_offset = *p_block_offset;
	uint32_t cursor = *p_cursor;
	uint32_t carry = 0;
	uint32_t base_z = 0, block = 0, blocks_left = 0, floor_type_index = 0, object = 0, record = 0, slope_factor = 0, slope_height = 0, slope_index = 0;
	uint32_t x_min = 0, x_max = 0, y_min = 0;

	if ((OBJECT(current_object)->flags & 8u) == 0u)
	{
		blocks_left = nearby_floor_block_count;
		cursor = nearby_floor_blocks_ADDRESS;
		block = floor_blocks + 2u;
		if (blocks_left == 0u)
		{
		find_floor_no_block:
			if (OBJECT(current_object)->floor_block >= 0 && OBJECT(current_object)->floor_z_int < 0x7FF)
			{
				OBJECT(current_object)->previous_floor_block = OBJECT(current_object)->floor_block;
				OBJECT(current_object)->previous_floor_z = OBJECT(current_object)->floor_z;
			}
			OBJECT(current_object)->floor_block = -1;
			block = no_floor_block_ADDRESS;
		}
		else
		{
			x = OBJECT(current_object)->x;
			y = OBJECT(current_object)->y;
			for (;;)
			{
				block_offset = mem_read16(cursor);
				record = block + block_offset;
				x_min = ROM32(record);
				cursor += 2u;
				record += 4u;
				if ((int32_t)x < (int32_t)x_min)
				{
				find_floor_next_block:
					blocks_left--;
					if (blocks_left != 0u)
						continue;
					goto find_floor_no_block;
				}
				x_max = ROM32(record);
				record += 4u;
				if ((int32_t)x_max < (int32_t)x)
					goto find_floor_next_block;
				y_min = ROM32(record);
				record += 4u;
				if ((int32_t)y < (int32_t)y_min || (int32_t)ROM32(record) < (int32_t)y)
					goto find_floor_next_block;
				break;
			}
			if (OBJECT(current_object)->floor_block >= 0 && OBJECT(current_object)->floor_z_int < 0x7FF)
			{
				OBJECT(current_object)->previous_floor_block = OBJECT(current_object)->floor_block;
				OBJECT(current_object)->previous_floor_z = OBJECT(current_object)->floor_z;
			}
			OBJECT(current_object)->floor_block = block_offset;
			block += block_offset;
			x = ROM8(block + 0x12u);
			record = floor_types_ADDRESS + x * 4u;
			OBJECT(current_object)->floor_type = ROM8(record);
			OBJECT(current_object)->floor_attribute = ROM16(record + 2u);
		}
		OBJECT(current_object)->floor_z_int = ROM16(block + 0x10u);
		OBJECT(current_object)->floor_z_fraction = 0u;
		OBJECT(current_object)->floor_slope = ROM8(block + 0x13u);
		slope_height = 0u;
		if (ROM8(block + 0x13u) == 0u)
		{
			*p_x = x;
			*p_y = y;
			*p_block_offset = block_offset;
			*p_slope_height = slope_height;
			*p_cursor = cursor;
			return;
		}
		slope_index = ROM8(block + 0x13u);
		cursor = floor_slopes_ADDRESS + (uint16_t)(slope_index * 2u) * 4u;
		if ((ROM32(cursor) & 2u) == 0u)
			slope_height = OBJECT(current_object)->y - ROM32(block + 8u);
		else
			slope_height = OBJECT(current_object)->x - ROM32(block);
		cursor += 2u;
		slope_factor = ROM32(cursor);
		carry = (slope_height >> 11) & 1u;
		slope_height = (int32_t)slope_height >> 12;
		cursor += 4u;
		if (carry)
			slope_height++;
		slope_height = (int32_t)(slope_height * slope_factor) >> 4;
		OBJECT(current_object)->floor_z += slope_height;
		*p_x = slope_factor;
		*p_y = y;
		*p_block_offset = block_offset;
		*p_slope_height = slope_height;
		*p_cursor = cursor;
		return;
	}
	if (OBJECT(current_object)->floor_block >= 0 && OBJECT(current_object)->floor_z_int < 0x7FF)
	{
		OBJECT(current_object)->previous_floor_block = OBJECT(current_object)->floor_block;
		OBJECT(current_object)->previous_floor_z = OBJECT(current_object)->floor_z;
	}
	object = LINKED(WORK_RAM, OBJECT(current_object)->platform);
	OBJECT(current_object)->floor_block = OBJECT(object)->floor_block;
	OBJECT(current_object)->floor_z = OBJECT(object)->z;
	block = floor_blocks + 2u + OBJECT(current_object)->floor_block;
	floor_type_index = mem_read8(block + 0x12u);
	record = floor_types_ADDRESS + floor_type_index * 4u;
	OBJECT(current_object)->floor_type = ROM8(record);
	OBJECT(current_object)->floor_attribute = ROM16(record + 2u);
	OBJECT(current_object)->floor_slope = mem_read8(block + 0x13u);
	slope_height = 0u;
	if (mem_read8(block + 0x13u) == 0u)
	{
		*p_x = floor_type_index;
		*p_slope_height = slope_height;
		return;
	}
	slope_index = mem_read8(block + 0x13u);
	cursor = floor_slopes_ADDRESS + (uint16_t)(slope_index * 2u) * 4u;
	if ((ROM32(cursor) & 2u) == 0u)
		slope_height = OBJECT(current_object)->y - mem_read32(block + 8u);
	else
		slope_height = OBJECT(current_object)->x - mem_read32(block);
	cursor += 2u;
	base_z = (uint32_t)mem_read16(block + 0x10u) << 16;
	slope_height = ((int32_t)slope_height >> 16) * ROM32(cursor) + base_z;
	OBJECT(current_object)->floor_z = slope_height;
	cursor += 4u;
	*p_x = base_z;
	*p_slope_height = slope_height;
	*p_cursor = cursor;
}

void get_floor_z_at_point(uint32_t x, uint32_t y, uint32_t *p_floor_z, uint32_t *p_block)
{
	uint32_t floor_z = *p_floor_z;
	uint32_t block = 0, block_offset = 0, blocks_left = 0, bounds = 0, cursor = 0, slope = 0, slope_distance = 0, slope_record = 0, test_x = 0, test_y = 0;
	uint32_t x_min = 0, x_max = 0, y_min = 0;

	blocks_left = nearby_floor_block_count;
	cursor = nearby_floor_blocks_ADDRESS;
	block = floor_blocks + 2u;
	if (blocks_left == 0u)
	{
		*p_block = block;
		return;
	}
	test_x = x;
	test_y = y;
	for (;;)
	{
		block_offset = mem_read16(cursor);
		bounds = block + block_offset;
		x_min = ROM32(bounds);
		cursor += 2u;
		bounds += 4u;
		if ((int32_t)test_x < (int32_t)x_min)
		{
		get_floor_z_at_point_next_block:
			blocks_left--;
			if (blocks_left != 0u)
				continue;
			block = no_floor_block_ADDRESS;
			floor_z = (uint32_t)ROM16(block + 0x10u) << 16;
			if (ROM8(block + 0x13u) == 0u)
			{
				*p_floor_z = floor_z;
				*p_block = block;
				return;
			}
			slope = ROM8(block + 0x13u);
			slope_record = floor_slopes_ADDRESS + (uint16_t)(slope * 2u) * 4u;
			if ((ROM32(slope_record) & 2u) == 0u)
				slope_distance = y - ROM32(block + 8u);
			else
				slope_distance = x - ROM32(block);
			floor_z += (slope_distance >> 16) * ROM32(slope_record + 2u);
			*p_floor_z = floor_z;
			*p_block = block;
			return;
		}
		x_max = ROM32(bounds);
		bounds += 4u;
		if ((int32_t)x_max < (int32_t)test_x)
			goto get_floor_z_at_point_next_block;
		y_min = ROM32(bounds);
		bounds += 4u;
		if ((int32_t)test_y < (int32_t)y_min || (int32_t)ROM32(bounds) < (int32_t)test_y)
			goto get_floor_z_at_point_next_block;
		break;
	}
	block += block_offset;
	floor_z = (uint32_t)ROM16(block + 0x10u) << 16;
	if (ROM8(block + 0x13u) == 0u)
	{
		*p_floor_z = floor_z;
		*p_block = block;
		return;
	}
	slope = ROM8(block + 0x13u);
	slope_record = floor_slopes_ADDRESS + (uint16_t)(slope * 2u) * 4u;
	if ((ROM32(slope_record) & 2u) == 0u)
		slope_distance = y - ROM32(block + 8u);
	else
		slope_distance = x - ROM32(block);
	floor_z += (slope_distance >> 16) * ROM32(slope_record + 2u);
	*p_floor_z = floor_z;
	*p_block = block;
}

uint32_t find_floor_block_at_point(uint32_t x, uint32_t y)
{
	uint32_t block = 0, block_offset = 0, blocks_left = 0, bounds = 0, cursor = 0, test_x = 0, test_y = 0;
	uint32_t x_min = 0, x_max = 0, y_min = 0;

	blocks_left = nearby_floor_block_count;
	cursor = nearby_floor_blocks_ADDRESS;
	block = floor_blocks + 2u;
	if (blocks_left == 0u)
		return block;
	test_x = x;
	test_y = y;
	do
	{
		block_offset = mem_read16(cursor);
		bounds = block + block_offset;
		x_min = ROM32(bounds);
		cursor += 2u;
		bounds += 4u;
		if ((int32_t)x_min <= (int32_t)test_x)
		{
			x_max = ROM32(bounds);
			bounds += 4u;
			if ((int32_t)test_x <= (int32_t)x_max)
			{
				y_min = ROM32(bounds);
				bounds += 4u;
				if ((int32_t)y_min <= (int32_t)test_y && (int32_t)test_y <= (int32_t)ROM32(bounds))
				{
					block += block_offset;
					return block;
				}
			}
		}
		blocks_left--;
	} while (blocks_left != 0u);
	block = no_floor_block_ADDRESS;
	return block;
}

void check_player_step(uint32_t *p_height_diff, uint32_t *p_cursor)
{
	uint32_t height_diff = *p_height_diff;
	uint32_t cursor = *p_cursor;
	uint32_t floor_block = 0, object = 0, object_z = 0, opposite_side = 0, x = 0, y = 0;
	uint16_t floor_z_int = 0, z_int = 0, height_diff_2 = 0;

	step_floor_z = OBJECT(current_object)->floor_z;
	if (stage != 8u || scroll_position >= 0x4000u)
	{
		if ((OBJECT(current_object)->flags & 8u) == 0u)
		{
		check_player_step_against_floor:
			if (PLAYER(current_player)->path != 0u)
			{
				PLAYER(current_player)->previous_z = OBJECT(current_object)->z;
				OBJECT(current_object)->z = step_floor_z;
				*p_height_diff = height_diff;
				return;
			}
			object_z = (height_diff & ~0xFFFFu) | (uint16_t)OBJECT(current_object)->z_int;
			floor_z_int = step_floor_z_int;
			z_int = object_z;
			height_diff = (object_z & ~0xFFFFu) | (uint16_t)(z_int - floor_z_int);
			if (z_int == floor_z_int)
			{
				PLAYER(current_player)->previous_z = OBJECT(current_object)->z;
				OBJECT(current_object)->z = step_floor_z;
				*p_height_diff = height_diff;
				return;
			}
			height_diff = (height_diff & ~0xFFFFu) | (uint16_t)(height_diff + 8u);
			if ((uint16_t)height_diff >= 0x11u)
			{
				height_diff_2 = height_diff - 8u;
				height_diff = (height_diff & ~0xFFFFu) | height_diff_2;
				if ((int16_t)height_diff_2 < 0)
				{
					if (PLAYER(current_player)->state == 2u)
					{
						*p_height_diff = height_diff;
						return;
					}
					PLAYER(current_player)->state = 2u;
					PLAYER(current_player)->substate = 0u;
					*p_height_diff = height_diff;
					return;
				}
				if (OBJECT(current_object)->floor_slope != 0u)
				{
					PLAYER(current_player)->previous_z = OBJECT(current_object)->z;
					OBJECT(current_object)->z = step_floor_z;
					*p_height_diff = height_diff;
					return;
				}
				OBJECT(current_object)->x = PLAYER(current_player)->previous_x;
				OBJECT(current_object)->y = PLAYER(current_player)->previous_y;
				step_blocking_floor_block = OBJECT(current_object)->floor_block;
				OBJECT(current_object)->floor_block = OBJECT(current_object)->previous_floor_block;
				OBJECT(current_object)->floor_z = OBJECT(current_object)->previous_floor_z;
				PLAYER(current_player)->control_lock = 0u;
				if (OBJECT(current_object)->speed_int >= 0x500)
				{
					floor_block = step_blocking_floor_block;
					x = OBJECT(current_object)->x;
					y = OBJECT(current_object)->y;
					opposite_side = get_floor_block_side(floor_block, x, y);
					opposite_side = (opposite_side & ~0xFFu) | (uint8_t)(opposite_side + 2u);
					AS16(PLAYER(current_player)->bounce_direction) = (opposite_side & ~0xFFu) | (opposite_side & 3u);
					PLAYER(current_player)->state = 0xEu;
					PLAYER(current_player)->substate = 0u;
					*p_height_diff = height_diff;
					*p_cursor = x;
					return;
				}
				height_diff = 0u;
				OBJECT(current_object)->speed = 0;
				OBJECT(current_object)->speed_x = 0;
				OBJECT(current_object)->speed_y = 0;
				PLAYER(current_player)->control_lock = 0u;
				PLAYER(current_player)->free_turn_timer = 3u;
				OBJECT(current_object)->flags |= 0x800000u;
				PLAYER(current_player)->state = 0u;
				if (step_floor_z <= OBJECT(current_object)->z)
				{
					*p_height_diff = height_diff;
					return;
				}
				if (PLAYER(current_player)->state == 2u)
				{
					*p_height_diff = height_diff;
					return;
				}
				PLAYER(current_player)->state = 2u;
				PLAYER(current_player)->substate = 0u;
				*p_height_diff = height_diff;
				return;
			}
			PLAYER(current_player)->previous_z = OBJECT(current_object)->z;
			OBJECT(current_object)->z = step_floor_z;
			*p_height_diff = height_diff;
			return;
		}
		object = LINKED(current_object, OBJECT(current_object)->platform);
		if (OBJECT(object)->floor_slope != 0u)
			goto check_player_step_against_floor;
		step_floor_z = OBJECT(object)->z;
		if (mem_read8(floor_blocks + 2u + OBJECT(current_object)->previous_floor_block + 0x12u) == 0x24u)
		{
			PLAYER(current_player)->previous_z = OBJECT(current_object)->z;
			OBJECT(current_object)->z = step_floor_z;
			*p_height_diff = height_diff;
			return;
		}
		goto check_player_step_against_floor;
	}
	PLAYER(current_player)->previous_z = OBJECT(current_object)->z;
	OBJECT(current_object)->z = step_floor_z;
	*p_height_diff = height_diff;
}

void bounce_off_wall_in_air(void)
{
	uint32_t floor_block = 0, new_direction = 0, x = 0, y = 0;

	if (OBJECT(current_object)->z_int <= OBJECT(current_object)->floor_z_int || (int16_t)(OBJECT(current_object)->previous_floor_z_int - OBJECT(current_object)->floor_z_int) < 8)
		return;
	OBJECT(current_object)->x = PLAYER(current_player)->previous_x;
	OBJECT(current_object)->y = PLAYER(current_player)->previous_y;
	OBJECT(current_object)->floor_z = OBJECT(current_object)->previous_floor_z;
	floor_block = OBJECT(current_object)->floor_block;
	OBJECT(current_object)->floor_block = OBJECT(current_object)->previous_floor_block;
	x = OBJECT(current_object)->x;
	y = OBJECT(current_object)->y;
	new_direction = get_floor_block_side(floor_block, x, y);
	new_direction = ((new_direction << 8) + 0x200u) & 0x3FFu;
	if ((int16_t)(((uint32_t)OBJECT(current_object)->direction << 6) - (new_direction << 6)) >= 0)
		new_direction = (uint16_t)(new_direction + 0x180u);
	else
		new_direction = (uint16_t)(new_direction - 0x180u);
	OBJECT(current_object)->direction = new_direction & 0x3FFu;
	face_movement_direction(0u);
	PLAYER(current_player)->control_lock = 0xAu;
	OBJECT(current_object)->flags |= 0x2000u;
}

uint32_t get_floor_block_side(uint32_t floor_block, uint32_t x, uint32_t y)
{
	uint32_t side = 0;
	uint32_t x_min = 0, x_max = 0;

	floor_block = floor_block + floor_blocks + 2u;
	x_min = mem_read32(floor_block);
	floor_block += 4u;
	if ((int32_t)x_min <= (int32_t)x)
	{
		x_max = mem_read32(floor_block);
		floor_block += 4u;
		if ((int32_t)x <= (int32_t)x_max)
		{
			if ((int32_t)mem_read32(floor_block) <= (int32_t)y)
				side = 2u;
			else
				side = 0u;
		}
		else
			side = 1u;
	}
	else
		side = 3u;
	return side;
}

void get_floor_block_side_or_corner(uint32_t block, uint32_t x, uint32_t y, uint32_t *p_outcode, uint32_t *p_side_code)
{
	uint32_t outcode = 0, side_code = 0;

	outcode = 0u;
	block = block + floor_blocks + 2u;
	if ((int32_t)mem_read32(block) <= (int32_t)x)
	{
		if ((int32_t)mem_read32(block + 4u) < (int32_t)x)
			outcode = outcode | 1u;
	}
	else
		outcode = outcode | 2u;
	if ((int32_t)mem_read32(block + 8u) <= (int32_t)y)
	{
		if ((int32_t)y <= (int32_t)mem_read32(block + 0xCu))
		{
			side_code = floor_block_side_codes[outcode];
			*p_outcode = outcode;
			*p_side_code = side_code;
			return;
		}
		outcode = outcode | 8u;
		side_code = floor_block_side_codes[outcode];
		*p_outcode = outcode;
		*p_side_code = side_code;
		return;
	}
	outcode = outcode | 4u;
	side_code = floor_block_side_codes[outcode];
	*p_outcode = outcode;
	*p_side_code = side_code;
}

void animate_sign(void)
{
	uint8_t last_index = 0;
	uint32_t next_index = 0, palettes = 0;
	uint8_t palette_timer = 0;

	palette_timer = OBJECT(current_object)->animate_sign.palette_timer;
	OBJECT(current_object)->animate_sign.palette_timer = palette_timer - 1u;
	if ((int8_t)palette_timer > 1)
		return;
	OBJECT(current_object)->animate_sign.palette_timer = OBJECT(current_object)->animate_sign.palette_delay;
	palettes = OBJECT(current_object)->animate_sign.palette_list;
	last_index = (uint8_t)mem_read16(palettes);
	next_index = OBJECT(current_object)->animate_sign.palette_index;
	next_index = (uint8_t)(next_index + 1u);
	palettes += 2u;
	if ((int8_t)last_index <= (int8_t)next_index && (int8_t)last_index < (int8_t)next_index)
		next_index = 0u;
	OBJECT(current_object)->animate_sign.palette_index = next_index;
	OBJECT(current_object)->sprite_palette = mem_read16(palettes + next_index * 2u);
}

void cycle_part_palettes(void)
{
	uint8_t last_index = 0;
	uint16_t palette = 0;
	uint32_t next_index = 0, palettes = 0, part = 0, parts_left = 0;
	uint8_t palette_timer = 0;

	palette_timer = OBJECT(current_object)->cycle_part_palettes.palette_timer;
	OBJECT(current_object)->cycle_part_palettes.palette_timer = palette_timer - 1u;
	if ((int8_t)palette_timer > 1)
		return;
	OBJECT(current_object)->cycle_part_palettes.palette_timer = OBJECT(current_object)->cycle_part_palettes.palette_delay;
	palettes = OBJECT(current_object)->cycle_part_palettes.palette_list;
	last_index = (uint8_t)mem_read16(palettes);
	next_index = OBJECT(current_object)->cycle_part_palettes.palette_index;
	next_index = (uint8_t)(next_index + 1u);
	palettes += 2u;
	if ((int8_t)last_index <= (int8_t)next_index && (int8_t)last_index < (int8_t)next_index)
		next_index = 0u;
	OBJECT(current_object)->cycle_part_palettes.palette_index = next_index;
	palette = mem_read16(palettes + next_index * 2u);
	part = OBJECT(current_object)->part_list;
	parts_left = mem_read16(part);
	part += 2u;
	do
	{
		mem_write16(part + 6u, palette);
		part += 8u;
		parts_left--;
	} while (parts_left != 0u);
}

uint32_t place_player_at_restart_point(void)
{
	uint8_t offset_shift = 0;
	uint32_t kind_offset = 0, player_offset = 0, record = 0, restart_x = 0, restart_y = 0, spacing_step = 0, stage_index = 0, z = 0;

	if ((OBJECT(current_object)->flags & 0x2000000u) != 0u && game_in_progress != 0u && current_game_mode != 1u)
	{
		stage_index = stage;
		record = stage_restart_points_ADDRESS + stage_restart_points[stage_index] - 6u;
		offset_shift = (uint8_t)ROM16(record);
		record += 2u;
		OBJECT(current_object)->x_int = ROM16(record);
		player_offset = OBJECT(current_object)->player;
		player_offset = shift_signed_by(player_offset, (uint8_t)offset_shift, 16);
		OBJECT(current_object)->x_int += player_offset;
		OBJECT(current_object)->x_fraction = 0u;
		OBJECT(current_object)->y_int = ROM16(record + 2u);
		OBJECT(current_object)->y_fraction = 0u;
		return player_offset;
	}
	record = current_restart_point + 2u;
	kind_offset = ROM8(record) * 3u;
	record++;
	switch (kind_offset)
	{
	case 0x0u:
	case 0x6u:
		record++;
		OBJECT(current_object)->x_int = ROM16(record);
		player_offset = OBJECT(current_object)->player;
		record += 2u;
		if (player_offset == 2u)
			player_offset = 0xFFFFu;
		player_offset = (uint16_t)(player_offset << 1);
		OBJECT(current_object)->x_int += player_offset;
		OBJECT(current_object)->x_fraction = 0u;
		OBJECT(current_object)->y_int = ROM16(record);
		OBJECT(current_object)->y_fraction = 0u;
		return player_offset;
	case 0x3u:
		restart_y = ROM8(record);
		record++;
		restart_x = ROM16(record);
		z = (uint32_t)ROM16(record + 2u) << 16;
		screen_point_to_ground(z, &restart_x, &restart_y);
		spacing_step = OBJECT(current_object)->player;
		if (spacing_step == 2u)
			spacing_step = 0xFFFFu;
		spacing_step = spacing_step << 17;
		OBJECT(current_object)->x = restart_x + spacing_step;
		OBJECT(current_object)->y = restart_y;
		return spacing_step;
	default:
		unknown_code(0xCC13u + kind_offset);
	}
}

void screen_point_to_ground(uint32_t z, uint32_t *p_point_x, uint32_t *p_point_y)
{
	uint32_t point_x = *p_point_x;
	uint32_t point_y = *p_point_y;

	point_x = (uint32_t)(uint16_t)(point_x + view_x) << 16;
	point_y = (((uint32_t)(uint16_t)(point_y + view_y) << 16) - z) * 2u;
	unproject_point(&point_x, &point_y);
	*p_point_x = point_x;
	*p_point_y = point_y;
}

void unproject_point(uint32_t *p_point_x, uint32_t *p_point_y)
{
	uint32_t point_x = *p_point_x;
	uint32_t point_y = *p_point_y;
	uint32_t new_x = 0;

	new_x = ((int32_t)(point_x - point_y) >> 8) * 0xB5u;
	point_y = ((int32_t)(point_y + point_x) >> 8) * 0xB5u;
	point_x = new_x;
	*p_point_x = point_x;
	*p_point_y = point_y;
}

void aim_at_screen_centre(uint32_t *p_cos_part, uint32_t *p_dx, uint32_t *p_dy)
{
	uint16_t centre_dy = 0;
	uint32_t centre_angle = 0, centre_x = 0, centre_y = 0, cos_part = 0, dx = 0, dy = 0, ignored = 0, z = 0;

	centre_x = 0xA0u;
	centre_y = 0x70u;
	z = OBJECT(current_object)->z;
	screen_point_to_ground(z, &centre_x, &centre_y);
	centre_angle = (int32_t)(centre_x - OBJECT(current_object)->x) >> 12;
	centre_dy = (uint16_t)((int32_t)(centre_y - OBJECT(current_object)->y) >> 12);
	angle_of_vector(centre_dy, &centre_angle, &ignored);
	dx = (uint16_t)centre_angle;
	dy = 0x200u;
	polar_to_xy(&cos_part, &dx, &dy);
	*p_cos_part = cos_part;
	*p_dx = dx;
	*p_dy = dy;
}

void flash_hurt_palette(void)
{
	uint16_t palette = 0;
	uint8_t flash_timer = 0, flashing = 0;

	flash_timer = OBJECT(current_object)->flash_hurt_palette.flash_timer;
	OBJECT(current_object)->flash_hurt_palette.flash_timer = flash_timer - 1u;
	if ((int8_t)flash_timer > 1)
		return;
	OBJECT(current_object)->flash_hurt_palette.flash_timer = OBJECT(current_object)->flash_hurt_palette.flash_period;
	palette = mem_read16(OBJECT(current_object)->flash_hurt_palette.palette_list + 2u);
	flashing = ~OBJECT(current_object)->flash_hurt_palette.flash_on;
	OBJECT(current_object)->flash_hurt_palette.flash_on = flashing;
	if (flashing != 0u)
		palette = 0x83B0u;
	OBJECT(current_object)->sprite_palette = palette;
}

void set_floor_sprite_priority(void)
{
	if (OBJECT(current_object)->floor_type == 4u)
	{
		OBJECT(current_object)->sprite_palette &= 0x1FFFu;
		OBJECT(current_object)->sprite_palette |= 0x4000u;
	}
	if (ROM8(floor_types_ADDRESS + mem_read8(floor_blocks + 2u + OBJECT(current_object)->previous_floor_block + 0x12u) * 4u) == 0xCu && OBJECT(current_object)->floor_type == 0xCu)
	{
		OBJECT(current_object)->sprite_palette &= 0x1FFFu;
		OBJECT(current_object)->sprite_palette |= 0x2000u;
		OBJECT(current_object)->sprite_palette |= 0x8000u;
	}
}

uint32_t update_quicksand_object(uint32_t sink_offset)
{
	uint8_t health = 0;

	if ((OBJECT(current_object)->flags & 0x80000000u) != 0u)
	{
		sink_offset = 0x4EC4u * (uint16_t)(0x68u - OBJECT(current_object)->touch_distance);
		OBJECT(current_object)->floor_z += sink_offset;
		OBJECT(current_object)->z = OBJECT(current_object)->floor_z;
		if ((PLAYER(current_player)->flags & 4u) != 0u)
		{
			PLAYER(current_player)->sink_depth += 0x33u;
			OBJECT(current_object)->z += (uint32_t)(uint16_t)PLAYER(current_player)->sink_depth << 8;
			if (PLAYER(current_player)->sink_depth < 0x2000)
				return sink_offset;
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
	}
	return sink_offset;
}
