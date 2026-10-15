/**
 *    Name:        player/basic_states.c
 *    Description: player_move (the movement the basic states share) and player_animate: standing, walking, running, falling, jumping, and the player's drawing
 *    Version:     0.1.0
 *    Created:     12-10-2026
 *    Modified:    15-10-2026
 *    Author:      Sebastian Toyos
 *
 *    SegaSonic The Hedgehog (arcade, Sega System 32): the game's V60 program decompiled into C by
 *    tools/decompile.py, with the names of symbols.txt.
 */
#include "game.h"

void player_move(uint32_t depth, uint32_t cursor)
{
	uint32_t r0 = 0, block_offset = 0, ignored = 0, jump_offset = 0, state_entry = 0;
	uint32_t flags = 0, flags_2 = 0;

	for (;;)
	{
		if (PLAYER(current_player)->hit_stop != 0u)
			PLAYER(current_player)->hit_stop--;
		if (PLAYER(current_player)->control_lock != 0u)
			PLAYER(current_player)->control_lock--;
		if (PLAYER(current_player)->free_turn_timer != 0u)
			PLAYER(current_player)->free_turn_timer--;
		check_zone_contact();
		if (PLAYER(current_player)->state != 0x22u)
		{
			OBJECT(current_object)->flags &= ~0x1000u;
			check_player_contacts(&depth, &cursor);
			check_player_stalled();
			r0 = PLAYER(current_player)->state;
			if (player_state_stops_move[r0] != 0u)
				goto player_dispatch_state;
			push_player_out_of_wall(&r0, &depth, &cursor);
			if (PLAYER(current_player)->state == 0xEu)
				goto player_dispatch_state;
			if (per_player_view_mode != 0u || PLAYER(current_player)->state == 4u || PLAYER(current_player)->state == 0x17u || OBJECT(current_object)->floor_type == 0xAu || (OBJECT(current_object)->flags & 0x80000000u) != 0u)
			{
			player_move_steer:
				if (OBJECT(current_object)->floor_block < 0)
				{
				player_move_steer_in_air:
					player_steer_in_air();
				}
				else
				{
					apply_floor_push();
					if ((uint8_t)PLAYER(current_player)->pushed_out == 0u && PLAYER(current_player)->control_lock == 0u)
					{
						if (PLAYER(current_player)->state == 2u)
							goto player_move_steer_in_air;
						player_steer(r0);
					}
				}
				depth = set_speed_from_player_speed();
				flags = OBJECT(current_object)->flags;
				OBJECT(current_object)->flags = flags & ~0x800000u;
				if ((flags & 0x800000u) == 0u)
				{
					if (PLAYER(current_player)->state != 1u || (int16_t)(OBJECT(current_object)->speed_int - AT16(PLAYER(current_player)->animation_speed, 2)) >= 0x10)
					{
						if (PLAYER(current_player)->hit_stop == 0u)
						{
							PLAYER(current_player)->previous_x = OBJECT(current_object)->x;
							PLAYER(current_player)->previous_y = OBJECT(current_object)->y;
							OBJECT(current_object)->x += OBJECT(current_object)->speed_x;
							OBJECT(current_object)->y += OBJECT(current_object)->speed_y;
							OBJECT(current_object)->flags &= ~0x8000u;
						}
					}
					else
					{
						PLAYER(current_player)->walk_pending_x += OBJECT(current_object)->speed_x;
						PLAYER(current_player)->walk_pending_y += OBJECT(current_object)->speed_y;
						flags_2 = OBJECT(current_object)->flags;
						OBJECT(current_object)->flags = flags_2 & ~0x8000u;
						if ((flags_2 & 0x8000u) != 0u)
						{
							PLAYER(current_player)->previous_x = OBJECT(current_object)->x;
							PLAYER(current_player)->previous_y = OBJECT(current_object)->y;
							OBJECT(current_object)->x += PLAYER(current_player)->walk_pending_x;
							OBJECT(current_object)->y += PLAYER(current_player)->walk_pending_y;
							PLAYER(current_player)->walk_pending_x = 0;
							PLAYER(current_player)->walk_pending_y = 0;
						}
					}
				}
				if (per_player_view_mode == 0u)
					find_floor(&r0, &depth, &block_offset, &ignored, &cursor);
				depth = update_quicksand_object(depth);
				depth = update_sinking_floor(depth);
				if (OBJECT(current_object)->floor_type == 0u)
				{
				player_move_check_edge:
					set_floor_sprite_priority();
					check_player_floor_edge(&depth, &cursor);
					if (PLAYER(current_player)->state >= 5 && PLAYER(current_player)->state <= 9)
						goto player_dispatch_state;
					apply_moving_floor();
					if (per_player_view_mode == 0u)
					{
						if (PLAYER(current_player)->state != 4u || PLAYER(current_player)->attack_power <= 1)
							project_object(&ignored, &depth);
						else
						{
							AS8(temp_0) = OBJECT(current_object)->sprite_offset_y;
							OBJECT(current_object)->sprite_offset_y = 0x12;
							project_object(&ignored, &depth);
							OBJECT(current_object)->sprite_offset_y = AS8(temp_0);
						}
					}
					else
						depth = project_object_2();
					depth = keep_player_on_screen(depth);
					if (PLAYER(current_player)->state == 0x16u)
						goto player_dispatch_state;
					break;
				}
				state_entry = floor_type_player_states_ADDRESS + OBJECT(current_object)->floor_type;
				if (ROM8(state_entry) == 0u)
					goto player_move_check_edge;
				PLAYER(current_player)->previous_state = PLAYER(current_player)->state;
				PLAYER(current_player)->state = ROM8(state_entry);
				PLAYER(current_player)->saved_substate = PLAYER(current_player)->substate;
				PLAYER(current_player)->substate = 0u;
				PLAYER(current_player)->sequence_step = 0u;
				goto player_dispatch_state;
			}
			check_player_step(&r0, &cursor);
			if (PLAYER(current_player)->state == 0xEu)
				goto player_dispatch_state;
			goto player_move_steer;
		}
	player_dispatch_state:
		jump_offset = PLAYER(current_player)->state * 3u;
		switch (PLAYER(current_player)->state)
		{
		case 0:
		case 1:
		case 2:
		case 4:
		case 12:
		case 19:
		case 20:
		case 23:
		case 58:
			continue;
		case 3:
			player_state_inactive();
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
		case 21:
			player_state_held(depth, cursor);
			return;
		case 22:
			player_state_dying();
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
		default:
			unknown_code(0x2D3Eu + PLAYER(current_player)->state * 3u);
		}
	}
	if (OBJECT(current_object)->handler == player_respawn_ADDRESS)
		return;
	check_player_jump();
	update_player_blink();
	remember_platform();
	add_player_to_lists();
	player_animate(depth, cursor);
}

void player_animate(uint32_t depth, uint32_t cursor)
{
	uint32_t jump_offset = 0;

	switch (PLAYER(current_player)->state)
	{
	case 0:
		player_animate_standing();
		return;
	case 1:
		player_animate_walking();
		return;
	case 2:
		player_animate_falling();
		return;
	case 3:
		player_state_inactive();
		return;
	case 4:
		player_animate_jumping(cursor);
		return;
	case 5:
	case 6:
	case 7:
	case 8:
	case 9:
	case 10:
	case 11:
	case 13:
	case 14:
	case 15:
	case 16:
	case 17:
	case 18:
	case 21:
	case 22:
	case 24:
	case 25:
	case 26:
	case 27:
	case 28:
	case 29:
	case 30:
	case 31:
	case 32:
	case 33:
	case 34:
	case 35:
	case 36:
	case 37:
	case 38:
	case 39:
	case 40:
	case 41:
	case 42:
	case 43:
	case 44:
	case 45:
	case 46:
	case 47:
	case 48:
	case 49:
	case 50:
	case 51:
	case 52:
	case 53:
	case 54:
	case 55:
	case 56:
	case 57:
	case 58:
		break;
	case 12:
		player_animate_skidding(depth, cursor);
		return;
	case 19:
		player_animate_running();
		return;
	case 20:
		player_animate_running_fast();
		return;
	case 23:
		player_animate_hopping();
		return;
	case 59:
		player_animate_standing();
		return;
	default:
		unknown_code(0x2FFCu + PLAYER(current_player)->state * 3u);
	}
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

void player_animate_standing(void)
{
	uint32_t r1 = 0, r8 = 0, animation_table = 0, depth = 0, ignored = 0;

	if ((int32_t)OBJECT(current_object)->animation >= 0x10FD8C)
	{
		OBJECT(current_object)->animation_timer = 0;
		OBJECT(current_object)->animation_speed = ((uint32_t)OBJECT(current_object)->kind << 3) + 0x2Cu;
		OBJECT(current_object)->animation_frame = 0xFFu;
		PLAYER(current_player)->standing_started = 0xFFu;
	}
	animation_table = sonic_standing_animations_ADDRESS;
	if (OBJECT(current_object)->kind != 0u)
	{
		animation_table = ray_standing_animations_ADDRESS;
		if (OBJECT(current_object)->kind != 1u)
			animation_table = mighty_standing_animations_ADDRESS;
	}
	OBJECT(current_object)->animation = animation_table + ROM16(animation_table + PLAYER(current_player)->facing * 2u);
	animate_with_flags(&r1, &r8);
	if ((uint8_t)OBJECT(current_object)->animation_timer == OBJECT(current_object)->animation_speed && PLAYER(current_player)->standing_started == 0u && OBJECT(current_object)->animation_frame != 2u && OBJECT(current_object)->animation_frame != 6u)
		OBJECT(current_object)->animation_timer = 3;
	PLAYER(current_player)->standing_started = 0u;
	if ((OBJECT(current_object)->flags & 0x80000u) != 0u)
		return;
	if ((PLAYER(current_player)->flags & 1u) == 0u)
	{
		r1 = queue_sprite_at_base(r1);
		return;
	}
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
}

void player_animate_walking(void)
{
	uint32_t r1 = 0, at_last_step = 0, depth = 0, ignored = 0, walking_animations = 0;
	uint32_t flags = 0;

	if ((int32_t)OBJECT(current_object)->animation < 0x10FD8C || (int32_t)OBJECT(current_object)->animation >= 0x11230C)
	{
		OBJECT(current_object)->animation_timer = 0;
		OBJECT(current_object)->animation_frame = 0u;
		OBJECT(current_object)->flags |= 0x4000u;
	}
	if ((int32_t)OBJECT(current_object)->animation < 0x10FD8C || (int32_t)OBJECT(current_object)->animation >= 0x110A4C)
	{
		PLAYER(current_player)->walk_pending_x = 0;
		PLAYER(current_player)->walk_pending_y = 0;
		PLAYER(current_player)->walk_cycle = 0;
	}
	walking_animations = sonic_walking_animations_ADDRESS;
	if (OBJECT(current_object)->kind != 0u)
	{
		walking_animations = ray_walking_animations_ADDRESS;
		if (OBJECT(current_object)->kind != 1u)
			walking_animations = mighty_walking_animations_ADDRESS;
	}
	OBJECT(current_object)->animation = walking_animations + ROM16(walking_animations + PLAYER(current_player)->facing * 2u);
	flags = OBJECT(current_object)->flags;
	OBJECT(current_object)->flags = flags & ~0x4000u;
	if ((flags & 0x4000u) != 0u)
		animate_step_with_flags(&r1, &at_last_step);
	PLAYER(current_player)->walk_cycle += AT16(PLAYER(current_player)->animation_speed, 2);
	if (PLAYER(current_player)->walk_cycle >= 0x200)
	{
		AT8(PLAYER(current_player)->walk_cycle, 1) = 0u;
		OBJECT(current_object)->flags |= 0x4000u;
		OBJECT(current_object)->flags |= 0x8000u;
	}
	if ((OBJECT(current_object)->flags & 0x80000u) != 0u)
		return;
	if ((PLAYER(current_player)->flags & 1u) == 0u)
	{
		r1 = queue_sprite_at_base(r1);
		return;
	}
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
}

void player_animate_running_fast(void)
{
	uint32_t animation_table = 0;

	animation_table = sonic_running_fast_animations_ADDRESS;
	if (OBJECT(current_object)->kind == 0u)
	{
		player_animate_run_cycle(animation_table);
		return;
	}
	animation_table = ray_running_fast_animations_ADDRESS;
	if (OBJECT(current_object)->kind == 1u)
	{
		player_animate_run_cycle(animation_table);
		return;
	}
	animation_table = mighty_running_fast_animations_ADDRESS;
	player_animate_run_cycle(animation_table);
}

void player_animate_running(void)
{
	uint32_t animation_table = 0;

	animation_table = sonic_running_animations_ADDRESS;
	if (OBJECT(current_object)->kind == 0u)
	{
		player_animate_run_cycle(animation_table);
		return;
	}
	animation_table = ray_running_animations_ADDRESS;
	if (OBJECT(current_object)->kind == 1u)
	{
		player_animate_run_cycle(animation_table);
		return;
	}
	animation_table = mighty_running_animations_ADDRESS;
	player_animate_run_cycle(animation_table);
}

void player_animate_run_cycle(uint32_t animation_table)
{
	uint32_t floor_attribute = 0;
	uint8_t run_toggle = 0;

	if ((OBJECT(current_object)->flags & 0x80000000u) != 0u)
	{
		player_animate_run_cycle_alternate();
		return;
	}
	if (OBJECT(current_object)->floor_type != 1u || OBJECT(current_object)->floor_attribute < 1 || OBJECT(current_object)->floor_attribute > 4)
	{
		player_animate_run_from_table(animation_table);
		return;
	}
	floor_attribute = OBJECT(current_object)->floor_attribute;
	if ((uint8_t)(PLAYER(current_player)->facing - floor_1_facing_bases[floor_attribute]) >= 5u)
	{
		player_animate_run_from_table(animation_table);
		return;
	}
	if ((uint8_t)floor_attribute == 2u || (uint8_t)floor_attribute == 3u)
	{
		player_animate_run_cycle_alternate();
		return;
	}
	run_toggle = PLAYER(current_player)->alternate_run_toggle + 2u;
	PLAYER(current_player)->alternate_run_toggle = run_toggle;
	if ((int8_t)run_toggle < 0)
	{
		player_animate_run_from_table(animation_table);
		return;
	}
	player_animate_run_cycle_alternate();
}

void player_animate_run_cycle_alternate(void)
{
	uint32_t alternate_run_animations = 0;

	alternate_run_animations = sonic_alternate_run_animations_ADDRESS;
	if (OBJECT(current_object)->kind == 0u)
	{
		player_animate_run_from_table(alternate_run_animations);
		return;
	}
	alternate_run_animations = ray_alternate_run_animations_ADDRESS;
	if (OBJECT(current_object)->kind == 1u)
	{
		player_animate_run_from_table(alternate_run_animations);
		return;
	}
	alternate_run_animations = mighty_alternate_run_animations_ADDRESS;
	player_animate_run_from_table(alternate_run_animations);
}

void player_animate_run_from_table(uint32_t animation_table)
{
	uint32_t r1 = 0, at_last_step = 0, depth = 0, ignored = 0, walk_cycle_limit = 0;
	uint32_t flags = 0;

	if ((int32_t)OBJECT(current_object)->animation < 0x110A4C || (int32_t)OBJECT(current_object)->animation >= 0x11230C)
	{
		OBJECT(current_object)->animation_timer = 0;
		OBJECT(current_object)->animation_frame = 2u;
	}
	OBJECT(current_object)->animation = animation_table + mem_read16(animation_table + PLAYER(current_player)->facing * 2u);
	flags = OBJECT(current_object)->flags;
	OBJECT(current_object)->flags = flags & ~0x4000u;
	if ((flags & 0x4000u) != 0u)
		animate_step_with_flags(&r1, &at_last_step);
	PLAYER(current_player)->walk_cycle += AT16(PLAYER(current_player)->animation_speed, 2);
	walk_cycle_limit = 0x4F0u;
	if (PLAYER(current_player)->state != 0x14u)
	{
		walk_cycle_limit = 0x600u;
		if (OBJECT(current_object)->floor_type == 4u && floor_4_slow_run_steps[OBJECT(current_object)->animation_frame] != 0u)
			walk_cycle_limit = (walk_cycle_limit * 0x400u) >> 8;
	}
	if ((int16_t)walk_cycle_limit <= PLAYER(current_player)->walk_cycle)
	{
		AT8(PLAYER(current_player)->walk_cycle, 1) = 0u;
		OBJECT(current_object)->flags |= 0x4000u;
	}
	if ((OBJECT(current_object)->flags & 0x80000u) != 0u)
		return;
	if ((PLAYER(current_player)->flags & 1u) == 0u)
	{
		r1 = queue_sprite_at_base(r1);
		return;
	}
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
}

void player_animate_falling(void)
{
	uint8_t new_status = 0;
	uint32_t at_last_step = 0, far_hang_animations = 0, ignored = 0, near_hang_animations = 0, running_animations = 0, side = 0;
	uint32_t flags = 0;

	if (PLAYER(current_player)->substate == 0u)
	{
		PLAYER(current_player)->substate++;
		OBJECT(current_object)->speed_z = -0x10000;
		flags = PLAYER(current_player)->flags;
		PLAYER(current_player)->flags = flags & ~0x80u;
		if ((flags & 0x80u) != 0u)
			OBJECT(current_object)->speed_z = 0x10000;
	}
	OBJECT(current_object)->speed_z += 0x6000u;
	OBJECT(current_object)->z += OBJECT(current_object)->speed_z;
	if (OBJECT(current_object)->floor_z_int < OBJECT(current_object)->z_int)
	{
		OBJECT(current_object)->z = OBJECT(current_object)->floor_z;
		PLAYER(current_player)->state = 1u;
		PLAYER(current_player)->substate = 0u;
	}
	running_animations = sonic_running_animations_ADDRESS;
	if (OBJECT(current_object)->kind != 0u)
	{
		running_animations = ray_running_animations_ADDRESS;
		if (OBJECT(current_object)->kind != 1u)
			running_animations = mighty_running_animations_ADDRESS;
	}
	OBJECT(current_object)->animation = running_animations + ROM16(running_animations + PLAYER(current_player)->facing * 2u);
	animate_step_with_flags(&ignored, &at_last_step);
	if (OBJECT(current_object)->floor_z_int >= 0x7FF && OBJECT(current_object)->previous_floor_z_int <= OBJECT(current_object)->floor_z_int)
	{
		PLAYER(current_player)->state = 8u;
		PLAYER(current_player)->substate = 0u;
		get_ledge_side(&side, &ignored);
		PLAYER(current_player)->ledge_side = side;
		new_status = 0xFFu;
		if (OBJECT(current_object)->health <= 8)
		{
			new_status = 1u;
			OBJECT(current_object)->flags |= 0x400u;
		}
		player_status[OBJECT(current_object)->player] = new_status;
		PLAYER(current_player)->mishap_flag = 0xFFu;
		OBJECT(current_object)->animation_speed = 1u;
		OBJECT(current_object)->animation_timer = 0;
		OBJECT(current_object)->animation_frame = 0xFFu;
		far_hang_animations = sonic_far_edge_hang_animations_ADDRESS;
		near_hang_animations = sonic_near_edge_hang_animations_ADDRESS;
		if (OBJECT(current_object)->kind != 0u)
		{
			far_hang_animations = ray_far_edge_hang_animations_ADDRESS;
			near_hang_animations = ray_near_edge_hang_animations_ADDRESS;
			if (OBJECT(current_object)->kind != 1u)
			{
				far_hang_animations = mighty_far_edge_hang_animations_ADDRESS;
				near_hang_animations = mighty_near_edge_hang_animations_ADDRESS;
			}
		}
		switch ((uint16_t)PLAYER(current_player)->ledge_side)
		{
		case 0:
		case 4:
			OBJECT(current_object)->animation = far_hang_animations;
			OBJECT(current_object)->sprite_flags &= 0xFFBFu;
			break;
		case 1:
			OBJECT(current_object)->animation = far_hang_animations;
			OBJECT(current_object)->sprite_flags |= 0x40u;
			break;
		case 2:
			OBJECT(current_object)->animation = near_hang_animations;
			OBJECT(current_object)->sprite_flags |= 0x40u;
			break;
		case 3:
			OBJECT(current_object)->animation = near_hang_animations;
			OBJECT(current_object)->sprite_flags &= 0xFFBFu;
			break;
		default:
			unknown_code(0x3540u + (uint16_t)PLAYER(current_player)->ledge_side * 2u);
		}
	}
	if ((OBJECT(current_object)->flags & 0x80000u) != 0u)
		return;
	if (OBJECT(current_object)->floor_z_int < 0x7FF)
	{
		ignored = queue_sprite_at_floor();
		return;
	}
	ignored = queue_sprite_at_previous_floor();
}

void player_state_inactive(void)
{
}

void player_animate_jumping(uint32_t sign)
{
	uint8_t sound_id = 0;
	uint16_t kind = 0, new_attack_power = 0, x_int = 0, y = 0, y_int = 0;
	uint32_t r0 = 0, r1 = 0, animation_rate = 0, gravity = 0, jump_animation = 0, palette = 0, plain_jump_animations = 0, scale = 0, text = 0, text_x = 0, write_index_address = 0, z_int = 0;
	uint16_t state_timer = 0;

	if (PLAYER(current_player)->substate == 0u)
	{
		PLAYER(current_player)->substate++;
		PLAYER(current_player)->state_timer = 4;
		OBJECT(current_object)->speed_z = -0x90000;
		if (OBJECT(current_object)->floor_type == 4u)
			OBJECT(current_object)->speed_z = -0x48000;
		PLAYER(current_player)->unused_jump_byte = 0u;
		PLAYER(current_player)->attack_power = 0;
		PLAYER(current_player)->jump_frames = 0;
		OBJECT(current_object)->animation_speed = 4u;
		OBJECT(current_object)->animation_timer = 4;
		OBJECT(current_object)->animation_frame = 0xFFu;
		OBJECT(current_object)->flags |= 0x40u;
		if (stage == 5u && cage_bar_hit == 0u)
		{
			x_int = (uint16_t)OBJECT(current_object)->x_int;
			y_int = (uint16_t)OBJECT(current_object)->y_int;
			kind = 4u;
			z_int = (uint16_t)OBJECT(current_object)->z_int;
			spawn_sign_2(x_int, y_int, kind, (uint16_t)z_int, sign);
		}
	}
	bounce_off_wall_in_air();
	PLAYER(current_player)->jump_delay = 1;
	new_attack_power = 4u;
	if (4u == 0u)
		new_attack_power = 1u;
	if (new_attack_power != 4u || (uint8_t)PLAYER(current_player)->attack_power == new_attack_power)
		PLAYER(current_player)->attack_power = (uint8_t)new_attack_power;
	else
	{
		PLAYER(current_player)->attack_power = (uint8_t)new_attack_power;
		sound_id = 0x1Fu;
		play_sound(sound_id, &r1, &write_index_address);
	}
	OBJECT(current_object)->flags &= ~0x40u;
	if ((uint8_t)PLAYER(current_player)->attack_power != 0u)
	{
		OBJECT(current_object)->flags |= 0x40u;
		if ((int8_t)OBJECT(current_object)->animation_frame < 0)
			OBJECT(current_object)->animation_frame = 0u;
	}
	jump_animation = sonic_attack_jump_animations_ADDRESS;
	plain_jump_animations = sonic_plain_jump_animations_ADDRESS;
	if (OBJECT(current_object)->kind != 0u)
	{
		jump_animation = ray_attack_jump_animations_ADDRESS;
		plain_jump_animations = ray_plain_jump_animations_ADDRESS;
		if (OBJECT(current_object)->kind != 1u)
		{
			jump_animation = mighty_attack_jump_animations_ADDRESS;
			plain_jump_animations = mighty_plain_jump_animations_ADDRESS;
		}
	}
	if (PLAYER(current_player)->attack_power < 1)
		jump_animation = plain_jump_animations;
	r0 = PLAYER(current_player)->facing;
	jump_animation += ROM16(jump_animation + r0 * 2u);
	if ((int8_t)PLAYER(current_player)->substate < 0)
		jump_animation += 4u;
	else
	{
		PLAYER(current_player)->substate = 0xFFu;
		r0 = OBJECT(current_object)->sprite_flags;
		r0 = r0 & ~0xC0u;
		r0 = r0 | ROM16(jump_animation);
		OBJECT(current_object)->sprite_flags = r0;
		jump_animation += 2u;
		OBJECT(current_object)->sprite_frame = ROM16(jump_animation);
		PLAYER(current_player)->jump_first_tile = OBJECT(current_object)->sprite_frame;
		jump_animation += 2u;
	}
	OBJECT(current_object)->animation = jump_animation;
	animation_rate = (r0 & ~0xFFu) | 1u;
	if ((uint8_t)PLAYER(current_player)->attack_power != 4u)
		animation_rate = (animation_rate & ~0xFFu) | 2u;
	OBJECT(current_object)->animation_speed = animation_rate;
	animate_with_flags(&r1, &z_int);
	if (PLAYER(current_player)->jump_first_tile != OBJECT(current_object)->sprite_frame)
	{
		gravity = 0x9000u;
		if (OBJECT(current_object)->floor_type == 4u)
			gravity = 0x2400u;
		OBJECT(current_object)->speed_z += gravity;
		OBJECT(current_object)->z += OBJECT(current_object)->speed_z;
	}
	if (stage == 5u && cage_bar_hit == 0u)
	{
		text_x = (uint16_t)OBJECT(current_object)->screen_x;
		text_x = (uint16_t)(text_x - 0x18u);
		y = (uint16_t)(((int32_t)((uint32_t)OBJECT(current_object)->projected_y + OBJECT(current_object)->floor_z) >> 16) - view_y - 0x30u);
		palette = 0x8440u;
		scale = 0x100u;
		text = attack_text_ADDRESS;
		queue_string_at(text_x, y, &palette, &scale, &text);
	}
	PLAYER(current_player)->jump_frames++;
	if (PLAYER(current_player)->jump_frames > 0x50)
	{
		player_animate_jumping_land();
		return;
	}
	state_timer = PLAYER(current_player)->state_timer;
	PLAYER(current_player)->state_timer = state_timer - 1u;
	if ((int16_t)state_timer > 1)
	{
		player_draw();
		return;
	}
	PLAYER(current_player)->state_timer = 0;
	if (OBJECT(current_object)->z < OBJECT(current_object)->floor_z)
	{
		player_draw();
		return;
	}
	player_animate_jumping_land();
}

void player_animate_jumping_land(void)
{
	PLAYER(current_player)->state = 1u;
	PLAYER(current_player)->substate = 0u;
	OBJECT(current_object)->z = OBJECT(current_object)->floor_z;
	PLAYER(current_player)->jump_delay = 0;
	OBJECT(current_object)->flags &= ~0x2000u;
	OBJECT(current_object)->sprite_palette = mem_read16(OBJECT(current_object)->player_view.palette_list + 2u);
	OBJECT(current_object)->flags &= ~0x40u;
	OBJECT(current_object)->flags &= ~0x100000u;
	OBJECT(current_object)->flags &= ~0x200000u;
	player_draw();
}

void player_draw(void)
{
	uint32_t depth = 0, ignored = 0;

	if ((OBJECT(current_object)->flags & 0x80000u) != 0u)
		return;
	if ((PLAYER(current_player)->flags & 1u) == 0u)
	{
		if (OBJECT(current_object)->floor_block >= 0)
		{
			ignored = queue_sprite_at_floor();
			return;
		}
		ignored = queue_sprite_at_previous_floor();
		return;
	}
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
}
