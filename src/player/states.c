/**
 *    Name:        player/states.c
 *    Description: the player's states in play: standing, walking, running, falling and jumping (player_move, player_animate), teetering on a ledge and going over it, dropping, falling into a pit, climbing a wall, skidding, hopping, dying, launched, hurt, bouncing
 *    Version:     0.1.0
 *    Created:     12-10-2026
 *    Modified:    16-10-2026
 *    Author:      Sebastian Toyos
 *
 *    SegaSonic The Hedgehog (arcade, Sega System 32): the game's V60 program decompiled into C by
 *    tools/decompile.py, with the names of symbols.txt.
 */
#include "game.h"

void player_move(uint32_t depth, uint32_t cursor)
{
	uint32_t r0 = 0, ignored = 0, jump_offset = 0, state_entry = 0;
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
					if (PLAYER(current_player)->state != 1u || (int16_t)(OBJECT(current_object)->speed_int - PLAYER(current_player)->animation_speed_int) >= 0x10)
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
					find_floor(&ignored, &depth, &ignored, &ignored, &cursor);
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
							saved_sprite_offset_y = OBJECT(current_object)->sprite_offset_y;
							OBJECT(current_object)->sprite_offset_y = 0x12;
							project_object(&ignored, &depth);
							OBJECT(current_object)->sprite_offset_y = saved_sprite_offset_y;
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
			invalid_jump(0x2D3Eu + PLAYER(current_player)->state * 3u);
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
		invalid_jump(0x2FFCu + PLAYER(current_player)->state * 3u);
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
		invalid_jump(0x2D3Eu + PLAYER(current_player)->state * 3u);
	}
}

void player_animate_standing(void)
{
	uint32_t animation_table = 0, depth = 0, ignored = 0;

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
	animate_with_flags(&ignored, &ignored);
	if ((uint8_t)OBJECT(current_object)->animation_timer == OBJECT(current_object)->animation_speed && PLAYER(current_player)->standing_started == 0u && OBJECT(current_object)->animation_frame != 2u && OBJECT(current_object)->animation_frame != 6u)
		OBJECT(current_object)->animation_timer = 3;
	PLAYER(current_player)->standing_started = 0u;
	if ((OBJECT(current_object)->flags & 0x80000u) != 0u)
		return;
	if ((PLAYER(current_player)->flags & 1u) == 0u)
	{
		ignored = queue_sprite_at_base(ignored);
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
	uint32_t depth = 0, ignored = 0, walking_animations = 0;
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
		animate_step_with_flags(&ignored, &ignored);
	PLAYER(current_player)->walk_cycle += PLAYER(current_player)->animation_speed_int;
	if (PLAYER(current_player)->walk_cycle >= 0x200)
	{
		PLAYER(current_player)->walk_cycle_high = 0u;
		OBJECT(current_object)->flags |= 0x4000u;
		OBJECT(current_object)->flags |= 0x8000u;
	}
	if ((OBJECT(current_object)->flags & 0x80000u) != 0u)
		return;
	if ((PLAYER(current_player)->flags & 1u) == 0u)
	{
		ignored = queue_sprite_at_base(ignored);
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
	uint32_t depth = 0, ignored = 0, walk_cycle_limit = 0;
	uint32_t flags = 0;

	if ((int32_t)OBJECT(current_object)->animation < 0x110A4C || (int32_t)OBJECT(current_object)->animation >= 0x11230C)
	{
		OBJECT(current_object)->animation_timer = 0;
		OBJECT(current_object)->animation_frame = 2u;
	}
	OBJECT(current_object)->animation = animation_table + ROM16(animation_table + PLAYER(current_player)->facing * 2u);
	flags = OBJECT(current_object)->flags;
	OBJECT(current_object)->flags = flags & ~0x4000u;
	if ((flags & 0x4000u) != 0u)
		animate_step_with_flags(&ignored, &ignored);
	PLAYER(current_player)->walk_cycle += PLAYER(current_player)->animation_speed_int;
	walk_cycle_limit = 0x4F0u;
	if (PLAYER(current_player)->state != 0x14u)
	{
		walk_cycle_limit = 0x600u;
		if (OBJECT(current_object)->floor_type == 4u && floor_4_slow_run_steps[OBJECT(current_object)->animation_frame] != 0u)
			walk_cycle_limit = (walk_cycle_limit * 0x400u) >> 8;
	}
	if ((int16_t)walk_cycle_limit <= PLAYER(current_player)->walk_cycle)
	{
		PLAYER(current_player)->walk_cycle_high = 0u;
		OBJECT(current_object)->flags |= 0x4000u;
	}
	if ((OBJECT(current_object)->flags & 0x80000u) != 0u)
		return;
	if ((PLAYER(current_player)->flags & 1u) == 0u)
	{
		ignored = queue_sprite_at_base(ignored);
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
	uint32_t far_hang_animations = 0, ignored = 0, near_hang_animations = 0, running_animations = 0, side = 0;
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
	animate_step_with_flags(&ignored, &ignored);
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
			invalid_jump(0x3540u + (uint16_t)PLAYER(current_player)->ledge_side * 2u);
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
	uint16_t kind = 0, new_attack_power = 0, x_int = 0, y = 0, y_int = 0, z_int = 0;
	uint32_t r0 = 0, animation_rate = 0, gravity = 0, ignored = 0, jump_animation = 0, palette = 0, plain_jump_animations = 0, scale = 0, text = 0, text_x = 0;
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
			spawn_sign_2(x_int, y_int, kind, z_int, sign);
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
		play_sound(sound_id, &ignored, &ignored);
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
	animate_with_flags(&ignored, &ignored);
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
	OBJECT(current_object)->sprite_palette = ROM16(OBJECT(current_object)->player_view.palette_list + 2u);
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

void player_state_teetering(uint32_t cursor)
{
	uint8_t new_status = 0, sound_id = 0;
	uint32_t r1 = 0, block = 0, depth = 0, edge_direction = 0, ignored = 0, ledge_side = 0, side = 0, teeter_animation = 0, teeter_variant = 0;
	uint16_t state_timer = 0;

	if (PLAYER(current_player)->substate == 0u)
	{
		PLAYER(current_player)->substate++;
		OBJECT(current_object)->animation_speed = 3u;
		OBJECT(current_object)->animation_timer = 3;
		OBJECT(current_object)->animation_frame = 0xFFu;
		get_ledge_side(&side, &block);
		PLAYER(current_player)->ledge_side = side;
		switch (side)
		{
		case 0:
		case 5:
			teeter_variant = 0u;
			OBJECT(current_object)->sprite_flags &= 0xFFBFu;
			OBJECT(current_object)->x -= OBJECT(current_object)->speed_x;
			OBJECT(current_object)->y = FLOOR_BLOCK(block)->y_min;
			break;
		case 1:
			teeter_variant = 0u;
			OBJECT(current_object)->sprite_flags |= 0x40u;
			OBJECT(current_object)->x = FLOOR_BLOCK(block)->x_max;
			OBJECT(current_object)->y -= OBJECT(current_object)->speed_y;
			break;
		case 2:
			teeter_variant = 1u;
			OBJECT(current_object)->sprite_flags |= 0x40u;
			OBJECT(current_object)->x -= OBJECT(current_object)->speed_x;
			OBJECT(current_object)->y = FLOOR_BLOCK(block)->y_max;
			break;
		case 3:
			teeter_variant = 1u;
			OBJECT(current_object)->sprite_flags &= 0xFFBFu;
			OBJECT(current_object)->x = FLOOR_BLOCK(block)->x_min;
			OBJECT(current_object)->y -= OBJECT(current_object)->speed_y;
			break;
		case 4:
			if (OBJECT(current_object)->floor_type == 0xCu || mem_read8(floor_blocks + 2u + OBJECT(current_object)->previous_floor_block + 0x12u) == 0x11u || mem_read8(floor_blocks + 2u + OBJECT(current_object)->floor_block + 0x12u) == 0x1Eu)
			{
				edge_direction = (uint16_t)(((OBJECT(current_object)->direction + 0x200u) & 0x3FFu) + 0x80u) & 0x3FFu;
				edge_direction = edge_direction >> 8;
			}
			else
				edge_direction = mem_read8(floor_blocks + 2u + OBJECT(current_object)->previous_floor_block + 0x15u);
			switch (edge_direction)
			{
			case 0:
			player_teetering_ledge_direction_2:
				teeter_variant = 1u;
				OBJECT(current_object)->sprite_flags |= 0x40u;
				PLAYER(current_player)->ledge_direction = 2;
				break;
			case 1:
				teeter_variant = 1u;
				OBJECT(current_object)->sprite_flags &= 0xFFBFu;
				PLAYER(current_player)->ledge_direction = 3;
				break;
			case 2:
				if (OBJECT(current_object)->floor_attribute == 4u)
					goto player_teetering_ledge_direction_2;
				teeter_variant = 0u;
				OBJECT(current_object)->sprite_flags &= 0xFFBFu;
				PLAYER(current_player)->ledge_direction = 0;
				break;
			case 3:
			case 4:
				teeter_variant = 0u;
				OBJECT(current_object)->sprite_flags |= 0x40u;
				PLAYER(current_player)->ledge_direction = 1;
				break;
			default:
				invalid_jump(0x3903u + edge_direction * 2u);
			}
			break;
		default:
			invalid_jump(0x3837u + side * 2u);
		}
		cursor = teetering_animations_ADDRESS;
		teeter_animation = ROM16(teetering_animations_ADDRESS + OBJECT(current_object)->kind * 4u + teeter_variant * 2u) + teetering_animations_ADDRESS;
		OBJECT(current_object)->sprite_frame = ROM16(teeter_animation);
		OBJECT(current_object)->animation = teeter_animation + 2u;
		PLAYER(current_player)->state_timer = 0x3C;
		if ((OBJECT(current_object)->flags & 4u) == 0u || OBJECT(LINKED(current_object, OBJECT(current_object)->touched_by))->kind != 0x19u)
		{
			if ((uint16_t)PLAYER(current_player)->ledge_side == 4u)
				PLAYER(current_player)->state_timer = 1;
		}
		else
			PLAYER(current_player)->state_timer = 0;
		PLAYER(current_player)->ledge_pullback = 0;
		OBJECT(current_object)->speed_x = 0;
		OBJECT(current_object)->speed_y = 0;
		PLAYER(current_player)->control_lock = 0u;
		OBJECT(current_object)->flags &= ~0x2000u;
		if (((OBJECT(current_object)->flags & 4u) == 0u || OBJECT(LINKED(current_object, OBJECT(current_object)->touched_by))->kind != 0x19u) && (uint16_t)PLAYER(current_player)->ledge_side != 4u)
		{
			sound_id = 0x38u;
			if (OBJECT(current_object)->kind != 0u)
			{
				sound_id = 0x3Cu;
				if (OBJECT(current_object)->kind != 1u)
					sound_id = 0x44u;
			}
			play_sound(sound_id, &ignored, &ignored);
		}
	}
	apply_moving_floor();
	r1 = update_ledge_pullback();
	if ((uint16_t)PLAYER(current_player)->ledge_side == 4u)
		PLAYER(current_player)->ledge_pullback = 0;
	if (PLAYER(current_player)->ledge_pullback < 0x10)
	{
		if ((uint16_t)PLAYER(current_player)->state_timer == 0u)
		{
		player_teetering_fall_off:
			PLAYER(current_player)->state = 6u;
			PLAYER(current_player)->substate = 0u;
			if (OBJECT(current_object)->floor_type != 0xDu)
			{
				new_status = 0xFFu;
				if (OBJECT(current_object)->health <= 8)
				{
					new_status = 1u;
					OBJECT(current_object)->flags |= 0x400u;
				}
				r1 = OBJECT(current_object)->player;
				player_status[r1] = new_status;
			}
			PLAYER(current_player)->mishap_flag = 0xFFu;
		}
		else
		{
			state_timer = PLAYER(current_player)->state_timer;
			PLAYER(current_player)->state_timer = state_timer - 1u;
			if ((int16_t)state_timer <= 1)
			{
				PLAYER(current_player)->state_timer = -1;
				if (OBJECT(current_object)->animation_speed == (uint8_t)OBJECT(current_object)->animation_timer && OBJECT(current_object)->animation_frame == 7u)
					goto player_teetering_fall_off;
			}
		}
	}
	else
	{
		PLAYER(current_player)->state = 0u;
		PLAYER(current_player)->substate = 0u;
		OBJECT(current_object)->speed_x = 0;
		OBJECT(current_object)->speed_y = 0;
		OBJECT(current_object)->speed = 0;
		PLAYER(current_player)->animation_speed = 0;
		ledge_side = (uint16_t)PLAYER(current_player)->ledge_side;
		PLAYER(current_player)->heading = ledge_recover_headings[ledge_side];
		PLAYER(current_player)->target_heading = ledge_recover_headings[ledge_side];
		PLAYER(current_player)->facing_angle = ledge_recover_headings[ledge_side];
		OBJECT(current_object)->floor_block = OBJECT(current_object)->previous_floor_block;
		OBJECT(current_object)->floor_z = OBJECT(current_object)->previous_floor_z;
	}
	if (PLAYER(current_player)->state != 6u)
	{
		check_zone_contact();
		check_player_contacts(&r1, &cursor);
	}
	update_player_blink();
	remember_platform();
	add_player_to_lists();
	animate(&ignored, &ignored, &ignored);
	project_object(&ignored, &depth);
	update_sinking_floor(depth);
	if ((OBJECT(current_object)->flags & 0x80000u) != 0u)
		return;
	if ((PLAYER(current_player)->flags & 1u) == 0u)
	{
		depth = queue_sprite_at_base(depth);
		return;
	}
	PLAYER(current_player)->draw_depth = ((int32_t)((uint32_t)OBJECT(current_object)->projected_y + OBJECT(current_object)->previous_floor_z) >> 16) - view_y;
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

void player_state_falling_off_edge(void)
{
	uint8_t sound_id = 0;
	uint16_t sink_threshold = 0, x_shift = 0;
	uint32_t at_last_step = 0, depth = 0, floor_block_record = 0, ignored = 0;
	uint8_t health = 0;

	switch (PLAYER(current_player)->substate)
	{
	case 0:
	case 2:
		set_fall_sprite_priority();
		PLAYER(current_player)->fall_draw_depth = ((int32_t)((uint32_t)OBJECT(current_object)->projected_y + OBJECT(current_object)->previous_floor_z) >> 16) - view_y;
		floor_block_record = floor_blocks + 2u + OBJECT(current_object)->floor_block;
		if (FLOOR_BLOCK(floor_block_record)->type_index == 0x1Du)
		{
			if (OBJECT(current_object)->x < (int32_t)((((uint32_t)FLOOR_BLOCK(floor_block_record)->x_max - FLOOR_BLOCK(floor_block_record)->x_min) >> 1) + FLOOR_BLOCK(floor_block_record)->x_min))
				goto player_falling_off_edge_depth_minus_10;
			PLAYER(current_player)->fall_draw_depth -= 0x70u;
			goto player_falling_off_edge_depth_minus_10;
		}
		if (mem_read8(floor_blocks + 2u + OBJECT(current_object)->previous_floor_block + 0x12u) != 0x1Fu && mem_read8(floor_blocks + 2u + OBJECT(current_object)->previous_floor_block + 0x12u) != 0x11u)
		{
			if (OBJECT(current_object)->floor_type == 1u)
			{
				if ((uint16_t)PLAYER(current_player)->ledge_side != 4u)
				{
					if (PLAYER(current_player)->ledge_side < 2)
					{
					player_falling_off_edge_floor_1_depth:
						PLAYER(current_player)->fall_draw_depth = OBJECT(LINKED(current_object, OBJECT(current_object)->platform))->draw_depth - 1u;
					}
				player_falling_off_edge_depth_minus_10:
					PLAYER(current_player)->fall_draw_depth -= 0x10u;
				}
				else if (OBJECT(current_object)->floor_attribute != 5u)
				{
					if (PLAYER(current_player)->ledge_direction >= 2)
						goto player_falling_off_edge_depth_minus_10;
					goto player_falling_off_edge_floor_1_depth;
				}
			}
			else if (PLAYER(current_player)->ledge_side < 2)
			{
				if (stage != 0xAu)
					goto player_falling_off_edge_depth_minus_10;
				PLAYER(current_player)->fall_draw_depth -= 0x20u;
				goto player_falling_off_edge_depth_minus_10;
			}
		}
		else
		{
			if ((uint16_t)PLAYER(current_player)->ledge_side != 4u)
			{
				if (PLAYER(current_player)->ledge_side < 2)
				{
				player_falling_off_edge_block_1f_depth:
					PLAYER(current_player)->fall_draw_depth = OBJECT(LINKED(current_object, OBJECT(current_object)->platform))->draw_depth - 1u;
				}
			}
			else if (PLAYER(current_player)->ledge_direction < 2)
				goto player_falling_off_edge_block_1f_depth;
			PLAYER(current_player)->fall_draw_depth -= 4u;
		}
		OBJECT(current_object)->animation += 0x1Cu;
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
		if (OBJECT(current_object)->floor_type != 0xDu && OBJECT(current_object)->health <= 8)
		{
			sound_id = 0x2Fu;
			if (OBJECT(current_object)->kind != 0u)
			{
				sound_id = 0x37u;
				if (OBJECT(current_object)->kind != 1u)
					sound_id = 0x77u;
			}
			play_sound(sound_id, &ignored, &ignored);
		}
		PLAYER(current_player)->substate++;
		break;
	case 1:
		break;
	default:
		invalid_jump(0x3B56u + PLAYER(current_player)->substate * 3u);
	}
	OBJECT(current_object)->speed_z += 0x3000u;
	OBJECT(current_object)->z += OBJECT(current_object)->speed_z;
	if (OBJECT(current_object)->player_view.floor_block_high >= 0)
	{
		floor_block_record = floor_blocks + 2u + OBJECT(current_object)->floor_block;
		if (FLOOR_BLOCK(floor_block_record)->type_index == 0x22u)
		{
			sink_threshold = 0u;
			goto player_falling_off_edge_check_sinking;
		}
		if (FLOOR_BLOCK(floor_block_record)->type_index == 0x26u)
		{
			sink_threshold = 0u;
		player_falling_off_edge_check_sinking:
			if ((int16_t)sink_threshold <= OBJECT(current_object)->z_int && OBJECT(current_object)->y_int < OBJECT(LINKED(current_object, group_leaders[0]))->y_int)
			{
				PLAYER(current_player)->previous_state = PLAYER(current_player)->state;
				PLAYER(current_player)->state = 0x33u;
				PLAYER(current_player)->substate = 0u;
			}
		}
	}
	if (OBJECT(current_object)->floor_type == 0xDu && OBJECT(current_object)->z_int >= 0x530)
	{
		PLAYER(current_player)->state = 0x20u;
		PLAYER(current_player)->substate = 0u;
	}
	if (OBJECT(current_object)->screen_y >= 0x100)
	{
		if (stage == 5u || (stage == 8u && scroll_position < 0x4000u))
			OBJECT(current_object)->handler = player_respawn_ADDRESS;
		else
		{
			health = OBJECT(current_object)->health;
			OBJECT(current_object)->health = health - 8u;
			if ((int8_t)health > 8)
			{
				OBJECT(current_object)->handler = player_respawn_ADDRESS;
				if ((int8_t)OBJECT(current_object)->animation_speed >= 0)
				{
					at_last_step = animate_and_move(0u);
					if ((uint8_t)at_last_step != 0u)
						OBJECT(current_object)->animation_speed = 0xFFu;
				}
				update_screen_position(&ignored, &ignored);
				if (PLAYER(current_player)->ledge_side >= 2 || (OBJECT(current_object)->flags & 0x20000u) == 0u)
				{
					depth = PLAYER(current_player)->fall_draw_depth;
					queue_sprite_at_depth(&ignored, &depth);
					return;
				}
				depth = (uint16_t)(OBJECT(LINKED(current_object, OBJECT(current_object)->touched_by))->draw_depth - 1u);
				queue_sprite_at_depth(&ignored, &depth);
				return;
			}
			OBJECT(current_object)->health = 0;
			PLAYER(current_player)->state = 0x16u;
			PLAYER(current_player)->substate = 0u;
		}
	}
	if ((int8_t)OBJECT(current_object)->animation_speed >= 0)
	{
		at_last_step = animate_and_move(0u);
		if ((uint8_t)at_last_step != 0u)
			OBJECT(current_object)->animation_speed = 0xFFu;
	}
	update_screen_position(&ignored, &ignored);
	if (PLAYER(current_player)->ledge_side >= 2 || (OBJECT(current_object)->flags & 0x20000u) == 0u)
	{
		depth = PLAYER(current_player)->fall_draw_depth;
		queue_sprite_at_depth(&ignored, &depth);
		return;
	}
	depth = (uint16_t)(OBJECT(LINKED(current_object, OBJECT(current_object)->touched_by))->draw_depth - 1u);
	queue_sprite_at_depth(&ignored, &depth);
}

void player_state_falling_off_edge_spare_return(void)
{
}

void player_state_over_edge(uint32_t jump_offset, uint32_t cursor)
{
	uint8_t new_status = 0, sound_id = 0;
	uint16_t margin_x = 0, margin_y = 0;
	uint32_t r1 = 0, animation = 0, at_last_step = 0, depth = 0, far_hang_animations = 0, ignored = 0, ledge_side = 0, object = 0, off_screen = 0, side = 0, slip_limit = 0;
	uint16_t state_timer = 0, state_timer_2 = 0;

	switch ((jump_offset & ~0xFFu) | PLAYER(current_player)->substate)
	{
	case 0:
	case 6:
		PLAYER(current_player)->substate++;
		get_ledge_side(&side, &cursor);
		PLAYER(current_player)->ledge_side = side;
		PLAYER(current_player)->fall_draw_depth = ((int32_t)((uint32_t)OBJECT(current_object)->projected_y + OBJECT(current_object)->previous_floor_z) >> 16) - view_y;
		if (OBJECT(current_object)->floor_type == 1u)
		{
			if (PLAYER(current_player)->ledge_side < 2)
				PLAYER(current_player)->fall_draw_depth = OBJECT(LINKED(current_object, OBJECT(current_object)->platform))->draw_depth - 1u;
		player_over_edge_depth_minus_10:
			PLAYER(current_player)->fall_draw_depth -= 0x10u;
		}
		else if (PLAYER(current_player)->ledge_side < 2)
		{
			if (stage != 0xAu)
				goto player_over_edge_depth_minus_10;
			PLAYER(current_player)->fall_draw_depth -= 0x20u;
			goto player_over_edge_depth_minus_10;
		}
		set_speed_from_direction();
		OBJECT(current_object)->flags &= ~0x100u;
		if (OBJECT(current_object)->floor_type == 1u)
			OBJECT(current_object)->flags |= 0x100u;
		PLAYER(current_player)->heading = OBJECT(current_object)->direction;
		PLAYER(current_player)->ledge_pullback = 0;
		PLAYER(current_player)->slip_distance = 0;
		PLAYER(current_player)->control_lock = 0u;
		OBJECT(current_object)->flags &= ~0x20000u;
		PLAYER(current_player)->flags &= ~0x80u;
		OBJECT(current_object)->flags &= ~0x2000u;
	case 1:
		OBJECT(current_object)->x += OBJECT(current_object)->speed_x;
		OBJECT(current_object)->y += OBJECT(current_object)->speed_y;
		project_object(&ignored, &r1);
		if ((uint32_t)OBJECT(current_object)->speed == 0u)
		{
		player_over_edge_stop_sliding:
			PLAYER(current_player)->substate++;
		}
		else
		{
			PLAYER(current_player)->slip_distance += OBJECT(current_object)->speed;
			if ((PLAYER(current_player)->flags & 0x80u) != 0u)
				goto player_over_edge_stop_sliding;
			slip_limit = 0x20000000u;
			if (OBJECT(current_object)->floor_type == 9u || (OBJECT(current_object)->flags & 0x20000u) != 0u)
				slip_limit = (int32_t)slip_limit >> 2;
			if ((int32_t)slip_limit <= PLAYER(current_player)->slip_distance)
				goto player_over_edge_stop_sliding;
		}
		update_player_blink();
		if (PLAYER(current_player)->previous_state != 0xCu)
		{
			animation = OBJECT(current_object)->animation;
			if (((ROM16(animation + 2u) | ROM16(animation + 6u)) & ~0xC0u) == 0u)
				animate_with_flags(&r1, &ignored);
		}
		if ((OBJECT(current_object)->flags & 0x20000u) == 0u)
		{
			check_zone_contact();
			check_player_contacts(&r1, &cursor);
		}
		add_player_to_lists();
		if ((OBJECT(current_object)->flags & 0x80000u) != 0u)
			return;
		if (PLAYER(current_player)->ledge_side >= 2)
		{
			ignored = queue_sprite_at_previous_floor();
			return;
		}
		if ((PLAYER(current_player)->flags & 2u) != 0u || (OBJECT(current_object)->flags & 0x20000u) != 0u)
		{
			depth = (uint16_t)(OBJECT(LINKED(current_object, OBJECT(current_object)->touched_by))->draw_depth + 1u);
			queue_sprite_at_depth(&ignored, &depth);
			return;
		}
		depth = PLAYER(current_player)->fall_draw_depth;
		queue_sprite_at_depth(&ignored, &depth);
		return;
	case 2:
		PLAYER(current_player)->substate++;
		OBJECT(current_object)->animation_speed = 1u;
		OBJECT(current_object)->animation_timer = 0;
		OBJECT(current_object)->animation_frame = 0xFFu;
		far_hang_animations = sonic_far_edge_hang_animations_ADDRESS;
		cursor = sonic_near_edge_hang_animations_ADDRESS;
		if (OBJECT(current_object)->kind != 0u)
		{
			far_hang_animations = ray_far_edge_hang_animations_ADDRESS;
			cursor = ray_near_edge_hang_animations_ADDRESS;
			if (OBJECT(current_object)->kind != 1u)
			{
				far_hang_animations = mighty_far_edge_hang_animations_ADDRESS;
				cursor = mighty_near_edge_hang_animations_ADDRESS;
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
			OBJECT(current_object)->animation = cursor;
			OBJECT(current_object)->sprite_flags |= 0x40u;
			break;
		case 3:
			OBJECT(current_object)->animation = cursor;
			OBJECT(current_object)->sprite_flags &= 0xFFBFu;
			break;
		default:
			invalid_jump(0x3FE9u + (uint16_t)PLAYER(current_player)->ledge_side * 2u);
		}
		PLAYER(current_player)->state_timer = 0x3C;
		PLAYER(current_player)->ledge_pullback = 0;
		OBJECT(current_object)->speed = 0;
		OBJECT(current_object)->speed_x = 0;
		OBJECT(current_object)->speed_y = 0;
	case 3:
		update_ledge_pullback();
		OBJECT(current_object)->speed = 0;
		if ((OBJECT(current_object)->flags & 0x100u) == 0u)
		{
		player_over_edge_check_pull_up:
			if (PLAYER(current_player)->ledge_pullback >= 0x14)
				PLAYER(current_player)->substate++;
			if ((PLAYER(current_player)->flags & 2u) != 0u || (OBJECT(current_object)->flags & 0x20000u) != 0u)
			{
				state_timer = PLAYER(current_player)->state_timer;
				PLAYER(current_player)->state_timer = state_timer - 1u;
				if ((int16_t)state_timer <= 1)
				{
					PLAYER(current_player)->state_timer = 0;
					if ((int16_t)OBJECT(LINKED(current_object, OBJECT(current_object)->touched_by))->sprite_scale >= 0x100)
					{
					player_over_edge_drop:
						PLAYER(current_player)->state = 8u;
						PLAYER(current_player)->substate = 0u;
						new_status = 0xFFu;
						if (OBJECT(current_object)->health <= 8)
						{
							new_status = 1u;
							OBJECT(current_object)->flags |= 0x400u;
						}
						player_status[OBJECT(current_object)->player] = new_status;
						PLAYER(current_player)->mishap_flag = 0xFFu;
					}
				}
			}
			else
			{
				state_timer_2 = PLAYER(current_player)->state_timer;
				PLAYER(current_player)->state_timer = state_timer_2 - 1u;
				if ((int16_t)state_timer_2 <= 1)
					goto player_over_edge_drop;
			}
		}
		else
		{
			object = LINKED(current_object, PLAYER(current_player)->edge_object);
			switch (OBJECT(object)->direction)
			{
			case 0:
			case 4:
				if (OBJECT(current_object)->y_int <= OBJECT(object)->y_int)
					goto player_over_edge_drop;
				goto player_over_edge_check_pull_up;
			case 1:
				if (OBJECT(object)->y_int <= OBJECT(current_object)->y_int)
					goto player_over_edge_drop;
				goto player_over_edge_check_pull_up;
			case 2:
				if (OBJECT(current_object)->x_int <= OBJECT(object)->x_int)
					goto player_over_edge_drop;
				goto player_over_edge_check_pull_up;
			case 3:
				if (OBJECT(object)->x_int <= OBJECT(current_object)->x_int)
					goto player_over_edge_drop;
				goto player_over_edge_check_pull_up;
			default:
				invalid_jump(0x405Bu + OBJECT(object)->direction * 2u);
			}
		}
		animate(&ignored, &ignored, &ignored);
		update_player_blink();
		project_object(&ignored, &r1);
		if ((OBJECT(current_object)->flags & 0x20000u) == 0u)
		{
			check_zone_contact();
			check_player_contacts(&r1, &cursor);
		}
		add_player_to_lists();
		if ((OBJECT(current_object)->flags & 0x80000u) != 0u)
			return;
		if (PLAYER(current_player)->ledge_side >= 2)
		{
			ignored = queue_sprite_at_previous_floor();
			return;
		}
		if ((PLAYER(current_player)->flags & 2u) != 0u || (OBJECT(current_object)->flags & 0x20000u) != 0u)
		{
			depth = (uint16_t)(OBJECT(LINKED(current_object, OBJECT(current_object)->touched_by))->draw_depth + 1u);
			queue_sprite_at_depth(&ignored, &depth);
			return;
		}
		depth = PLAYER(current_player)->fall_draw_depth;
		queue_sprite_at_depth(&ignored, &depth);
		return;
	case 4:
		OBJECT(current_object)->animation_speed = 2u;
		OBJECT(current_object)->animation_timer = 0;
		OBJECT(current_object)->animation_frame = 0xFFu;
		OBJECT(current_object)->animation += 0x16u;
		OBJECT(current_object)->speed = 0x4000000;
		OBJECT(current_object)->direction = (PLAYER(current_player)->heading + 0x200u) & 0x3FFu;
		set_speed_from_direction();
		sound_id = 0x47u;
		if (OBJECT(current_object)->kind != 0u)
		{
			sound_id = 0x4Fu;
			if (OBJECT(current_object)->kind != 1u)
				sound_id = 0x67u;
		}
		play_sound(sound_id, &ignored, &ignored);
		PLAYER(current_player)->substate++;
	case 5:
		find_floor(&ignored, &ignored, &ignored, &ignored, &cursor);
		if ((OBJECT(current_object)->flags & 0x100u) == 0u)
		{
		player_over_edge_check_on_floor:
			if ((OBJECT(current_object)->flags & 0x20000u) == 0u)
			{
				if (OBJECT(current_object)->floor_block >= 0 && OBJECT(current_object)->floor_type != 2u && (uint32_t)OBJECT(current_object)->previous_floor_block == (uint32_t)OBJECT(current_object)->floor_block)
				{
				player_over_edge_back_on_floor:
					PLAYER(current_player)->state = 0u;
					PLAYER(current_player)->substate = 0u;
					OBJECT(current_object)->flags &= ~0x20000u;
					ledge_side = (uint16_t)PLAYER(current_player)->ledge_side;
					PLAYER(current_player)->heading = ledge_recover_headings[ledge_side];
					PLAYER(current_player)->target_heading = ledge_recover_headings[ledge_side];
				}
			}
			else if ((OBJECT(current_object)->flags & 4u) == 0u)
				goto player_over_edge_back_on_floor;
		}
		else
		{
			object = LINKED(current_object, PLAYER(current_player)->edge_object);
			switch (OBJECT(object)->direction)
			{
			case 0:
			case 4:
				if (OBJECT(current_object)->y_int <= OBJECT(object)->y_int)
					goto player_over_edge_back_on_floor;
				goto player_over_edge_check_on_floor;
			case 1:
				if (OBJECT(object)->y_int <= OBJECT(current_object)->y_int)
					goto player_over_edge_back_on_floor;
				goto player_over_edge_check_on_floor;
			case 2:
				if (OBJECT(current_object)->x_int <= OBJECT(object)->x_int)
					goto player_over_edge_back_on_floor;
				goto player_over_edge_check_on_floor;
			case 3:
				if (OBJECT(object)->x_int <= OBJECT(current_object)->x_int)
					goto player_over_edge_back_on_floor;
				goto player_over_edge_check_on_floor;
			default:
				invalid_jump(0x41EDu + OBJECT(object)->direction * 2u);
			}
		}
		if ((int8_t)OBJECT(current_object)->animation_speed >= 0)
		{
			animate(&ignored, &ignored, &at_last_step);
			if ((uint8_t)at_last_step != 0u)
				OBJECT(current_object)->animation_speed = 0xFFu;
		}
		update_player_blink();
		OBJECT(current_object)->x += OBJECT(current_object)->speed_x;
		OBJECT(current_object)->y += OBJECT(current_object)->speed_y;
		project_object(&ignored, &r1);
		check_zone_contact();
		check_player_contacts(&r1, &cursor);
		add_player_to_lists();
		margin_x = 0x50u;
		margin_y = 0x50u;
		off_screen = is_off_screen_margin(margin_x, margin_y);
		if (off_screen != 0u)
		{
			if (OBJECT(current_object)->health > 0)
				OBJECT(current_object)->handler = player_respawn_ADDRESS;
			else
			{
				OBJECT(current_object)->health = 0;
				PLAYER(current_player)->state = 0x16u;
				PLAYER(current_player)->substate = 0u;
			}
		}
		if ((OBJECT(current_object)->flags & 0x80000u) != 0u)
			return;
		ignored = queue_sprite_at_previous_floor();
		return;
	default:
		invalid_jump(0x3E05u + ((jump_offset & ~0xFFu) | PLAYER(current_player)->substate) * 3u);
	}
}

void player_state_dropping(uint32_t cursor)
{
	uint8_t sound_id = 0;
	uint32_t depth = 0, free_count = 0, ignored = 0, object = 0;
	uint8_t health = 0;
	uint16_t state_timer = 0;

	switch (PLAYER(current_player)->substate)
	{
	case 0:
	case 4:
		OBJECT(current_object)->sprite_palette |= 0x8000u;
		set_fall_sprite_priority();
		OBJECT(current_object)->sprite_frame = ROM16(OBJECT(current_object)->animation + 0x36u);
		PLAYER(current_player)->state_timer = 0x1E;
		PLAYER(current_player)->substate++;
		state_timer = PLAYER(current_player)->state_timer;
		PLAYER(current_player)->state_timer = state_timer - 1u;
		if ((int16_t)state_timer <= 1)
			PLAYER(current_player)->substate++;
		update_screen_position(&ignored, &ignored);
		if (PLAYER(current_player)->ledge_side >= 2)
		{
			ignored = queue_sprite_at_previous_floor();
			return;
		}
		if ((PLAYER(current_player)->flags & 2u) != 0u)
		{
			depth = (uint16_t)(OBJECT(LINKED(current_object, OBJECT(current_object)->touched_by))->draw_depth - 1u);
			queue_sprite_at_depth(&ignored, &depth);
			return;
		}
		if ((OBJECT(current_object)->flags & 0x20000u) == 0u)
		{
			depth = PLAYER(current_player)->fall_draw_depth;
			queue_sprite_at_depth(&ignored, &depth);
			return;
		}
		OBJECT(current_object)->sprite_palette &= 0x9FFFu;
		depth = OBJECT(LINKED(current_object, OBJECT(current_object)->touched_by))->draw_depth;
		queue_sprite_at_depth(&ignored, &depth);
		return;
	case 1:
		state_timer = PLAYER(current_player)->state_timer;
		PLAYER(current_player)->state_timer = state_timer - 1u;
		if ((int16_t)state_timer <= 1)
			PLAYER(current_player)->substate++;
		update_screen_position(&ignored, &ignored);
		if (PLAYER(current_player)->ledge_side >= 2)
		{
			ignored = queue_sprite_at_previous_floor();
			return;
		}
		if ((PLAYER(current_player)->flags & 2u) != 0u)
		{
			depth = (uint16_t)(OBJECT(LINKED(current_object, OBJECT(current_object)->touched_by))->draw_depth - 1u);
			queue_sprite_at_depth(&ignored, &depth);
			return;
		}
		if ((OBJECT(current_object)->flags & 0x20000u) == 0u)
		{
			depth = PLAYER(current_player)->fall_draw_depth;
			queue_sprite_at_depth(&ignored, &depth);
			return;
		}
		OBJECT(current_object)->sprite_palette &= 0x9FFFu;
		depth = OBJECT(LINKED(current_object, OBJECT(current_object)->touched_by))->draw_depth;
		queue_sprite_at_depth(&ignored, &depth);
		return;
	case 2:
		allocate_object(&free_count, &cursor);
		if (free_count == 0u)
			return;
		OBJECT(cursor)->sprite_flags = (OBJECT(current_object)->sprite_flags & 0xCFF0u) | 0x100Au;
		OBJECT(cursor)->sprite_palette = OBJECT(current_object)->sprite_palette;
		OBJECT(cursor)->animation_frame = 0u;
		OBJECT(cursor)->animation_speed = 3u;
		OBJECT(cursor)->animation_timer = 0;
		OBJECT(cursor)->animation = OBJECT(current_object)->animation;
		OBJECT(cursor)->animation += 0x22u;
		OBJECT(cursor)->direction = PLAYER(current_player)->ledge_side;
		OBJECT(cursor)->projected_x = OBJECT(current_object)->projected_x;
		OBJECT(cursor)->projected_x -= 0x10u;
		OBJECT(cursor)->projected_y = OBJECT(current_object)->projected_y;
		OBJECT(cursor)->projected_y -= 0x280000u;
		OBJECT(cursor)->z = OBJECT(current_object)->z;
		OBJECT(cursor)->sprite_offset_x = OBJECT(current_object)->sprite_offset_x;
		OBJECT(cursor)->sprite_offset_y = OBJECT(current_object)->sprite_offset_y;
		OBJECT(cursor)->handler = slip_effect_update_ADDRESS;
		OBJECT(current_object)->sprite_frame = ROM16(OBJECT(current_object)->animation + 0x38u);
		OBJECT(current_object)->speed_y = 0;
		if (OBJECT(current_object)->kind == 2u)
			cursor = spawn_slip_piece(cursor);
		if (OBJECT(current_object)->kind == 1u)
			spawn_slip_pieces(cursor);
		sound_id = 0x16u;
		play_sound(sound_id, &ignored, &ignored);
		if (OBJECT(current_object)->health <= 8)
		{
			sound_id = 0x2Fu;
			if (OBJECT(current_object)->kind != 0u)
			{
				sound_id = 0x37u;
				if (OBJECT(current_object)->kind != 1u)
					sound_id = 0x77u;
			}
			play_sound(sound_id, &ignored, &ignored);
		}
		PLAYER(current_player)->substate++;
	case 3:
		OBJECT(current_object)->speed_y += 0x6000u;
		OBJECT(current_object)->projected_y += OBJECT(current_object)->speed_y;
		if (OBJECT(current_object)->screen_y >= 0x110)
		{
			if (stage == 5u || (stage == 8u && scroll_position < 0x4000u))
				OBJECT(current_object)->handler = player_respawn_ADDRESS;
			else
			{
				health = OBJECT(current_object)->health;
				OBJECT(current_object)->health = health - 8u;
				if ((int8_t)health > 8)
				{
					OBJECT(current_object)->handler = player_respawn_ADDRESS;
					update_screen_position(&ignored, &ignored);
					if (PLAYER(current_player)->ledge_side >= 2)
					{
						ignored = queue_sprite_at_previous_floor();
						return;
					}
					if ((PLAYER(current_player)->flags & 2u) != 0u)
					{
						depth = (uint16_t)(OBJECT(LINKED(current_object, OBJECT(current_object)->touched_by))->draw_depth - 1u);
						queue_sprite_at_depth(&ignored, &depth);
						return;
					}
					if ((OBJECT(current_object)->flags & 0x20000u) == 0u)
					{
						depth = PLAYER(current_player)->fall_draw_depth;
						queue_sprite_at_depth(&ignored, &depth);
						return;
					}
					object = LINKED(current_object, OBJECT(current_object)->touched_by);
					OBJECT(current_object)->sprite_palette &= 0x9FFFu;
					if ((int16_t)(OBJECT(object)->screen_y + 0x20u) <= OBJECT(current_object)->screen_y)
						OBJECT(current_object)->sprite_palette |= 0x2000u;
					depth = OBJECT(object)->draw_depth;
					queue_sprite_at_depth(&ignored, &depth);
					return;
				}
				OBJECT(current_object)->health = 0;
				PLAYER(current_player)->state = 0x16u;
				PLAYER(current_player)->substate = 0u;
			}
		}
		update_screen_position(&ignored, &ignored);
		if (PLAYER(current_player)->ledge_side >= 2)
		{
			ignored = queue_sprite_at_previous_floor();
			return;
		}
		if ((PLAYER(current_player)->flags & 2u) != 0u)
		{
			depth = (uint16_t)(OBJECT(LINKED(current_object, OBJECT(current_object)->touched_by))->draw_depth - 1u);
			queue_sprite_at_depth(&ignored, &depth);
			return;
		}
		if ((OBJECT(current_object)->flags & 0x20000u) == 0u)
		{
			depth = PLAYER(current_player)->fall_draw_depth;
			queue_sprite_at_depth(&ignored, &depth);
			return;
		}
		object = LINKED(current_object, OBJECT(current_object)->touched_by);
		OBJECT(current_object)->sprite_palette &= 0x9FFFu;
		if ((int16_t)(OBJECT(object)->screen_y + 0x20u) <= OBJECT(current_object)->screen_y)
			OBJECT(current_object)->sprite_palette |= 0x2000u;
		depth = OBJECT(object)->draw_depth;
		queue_sprite_at_depth(&ignored, &depth);
		return;
	default:
		invalid_jump(0x4301u + PLAYER(current_player)->substate * 3u);
	}
}

void player_state_dropping_spare_return(void)
{
}

void player_state_falling_into_pit(void)
{
	uint16_t sink_threshold = 0;
	uint32_t depth = 0, floor_block_record = 0, ignored = 0, side = 0;
	uint8_t health = 0;

	switch (PLAYER(current_player)->substate)
	{
	case 0:
	case 2:
		get_ledge_side(&side, &ignored);
		PLAYER(current_player)->ledge_side = side;
		if ((uint16_t)PLAYER(current_player)->ledge_side == 4u)
			PLAYER(current_player)->ledge_direction = ((OBJECT(current_object)->direction + 0x80u) & 0x3FFu) >> 8;
		set_fall_sprite_priority();
		PLAYER(current_player)->fall_draw_depth = ((int32_t)((uint32_t)OBJECT(current_object)->projected_y + OBJECT(current_object)->previous_floor_z) >> 16) - view_y;
		floor_block_record = floor_blocks + 2u + OBJECT(current_object)->floor_block;
		if (FLOOR_BLOCK(floor_block_record)->type_index == 0x1Du)
		{
			if (OBJECT(current_object)->x < (int32_t)((((uint32_t)FLOOR_BLOCK(floor_block_record)->x_max - FLOOR_BLOCK(floor_block_record)->x_min) >> 1) + FLOOR_BLOCK(floor_block_record)->x_min))
				goto player_falling_into_pit_depth_minus_10;
			PLAYER(current_player)->fall_draw_depth -= 0x70u;
			goto player_falling_into_pit_depth_minus_10;
		}
		if ((uint16_t)PLAYER(current_player)->ledge_side != 4u)
		{
			if (PLAYER(current_player)->ledge_side < 2)
			{
			player_falling_into_pit_depth_at_platform:
				PLAYER(current_player)->fall_draw_depth = OBJECT(LINKED(current_object, OBJECT(current_object)->platform))->draw_depth - 1u;
			}
		player_falling_into_pit_depth_minus_10:
			PLAYER(current_player)->fall_draw_depth -= 0x10u;
		}
		else if (OBJECT(current_object)->floor_attribute != 5u)
		{
			if (PLAYER(current_player)->ledge_direction >= 2)
				goto player_falling_into_pit_depth_minus_10;
			goto player_falling_into_pit_depth_at_platform;
		}
		PLAYER(current_player)->mishap_flag = 0xFFu;
		PLAYER(current_player)->substate++;
		break;
	case 1:
		break;
	default:
		invalid_jump(0x4547u + PLAYER(current_player)->substate * 3u);
	}
	OBJECT(current_object)->speed_z += 0x6000u;
	OBJECT(current_object)->z += OBJECT(current_object)->speed_z;
	if (OBJECT(current_object)->player_view.floor_block_high >= 0)
	{
		floor_block_record = floor_blocks + 2u + OBJECT(current_object)->floor_block;
		if (FLOOR_BLOCK(floor_block_record)->type_index == 0x22u)
		{
			sink_threshold = 0u;
			goto player_falling_into_pit_check_sinking;
		}
		if (FLOOR_BLOCK(floor_block_record)->type_index == 0x26u)
		{
			sink_threshold = 0u;
		player_falling_into_pit_check_sinking:
			if ((int16_t)sink_threshold <= OBJECT(current_object)->z_int && OBJECT(current_object)->y_int < OBJECT(LINKED(current_object, group_leaders[0]))->y_int)
			{
				PLAYER(current_player)->previous_state = PLAYER(current_player)->state;
				PLAYER(current_player)->state = 0x33u;
				PLAYER(current_player)->substate = 0u;
			}
		}
	}
	if (OBJECT(current_object)->floor_type == 0xDu && OBJECT(current_object)->z_int >= 0x530)
	{
		PLAYER(current_player)->state = 0x20u;
		PLAYER(current_player)->substate = 0u;
	}
	if (OBJECT(current_object)->screen_y >= 0x130)
	{
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
	if ((int8_t)OBJECT(current_object)->animation_frame >= 0)
		animate_with_flags(&ignored, &ignored);
	OBJECT(current_object)->x += OBJECT(current_object)->speed_x;
	OBJECT(current_object)->y += OBJECT(current_object)->speed_y;
	saved_sprite_offset_y = OBJECT(current_object)->sprite_offset_y;
	OBJECT(current_object)->sprite_offset_y = 0x12;
	project_object(&ignored, &ignored);
	OBJECT(current_object)->sprite_offset_y = saved_sprite_offset_y;
	depth = PLAYER(current_player)->fall_draw_depth;
	queue_sprite_at_depth(&ignored, &depth);
}

void player_state_falling_into_pit_spare_return(void)
{
}

void player_state_climbing(uint32_t depth, uint32_t cursor)
{
	uint16_t climb_speed = 0, new_heading = 0, side_threshold = 0, wall_limit = 0, wall_start = 0;
	uint32_t across_wall = 0, along_wall = 0, at_last_step = 0, attribute_record = 0, climb_animation = 0, climb_over_animation = 0, climb_sideways_animation = 0, floor_block_record = 0, ignored = 0, jump_offset = 0, screen_shift = 0, side_step = 0, step_target = 0, step_x = 0, world_x = 0, world_y = 0, z_step = 0;

	for (;;)
	{
		check_zone_contact();
		check_player_contacts(&depth, &cursor);
		check_player_stalled();
		if (PLAYER(current_player)->state == 0x1Fu || PLAYER(current_player)->state == 0xDu || PLAYER(current_player)->state == 0x36u || PLAYER(current_player)->state == 0x20u)
		{
		player_dispatch_state:
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
				continue;
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
				invalid_jump(0x2D3Eu + PLAYER(current_player)->state * 3u);
			}
		}
		else
		{
			PLAYER(current_player)->state = 0xAu;
			attribute_record = OBJECT(current_object)->floor_attribute + floor_types_ADDRESS;
			update_player_blink();
			if ((OBJECT(current_object)->flags & 4u) != 0u && OBJECT(LINKED(current_object, OBJECT(current_object)->touched_by))->kind == 5u)
				goto player_climbing_update_position;
			switch (PLAYER(current_player)->substate)
			{
			case 0:
			case 5:
				PLAYER(current_player)->climb_from_top = 0u;
				if (OBJECT(current_object)->z_int <= (int16_t)ROM16(attribute_record + 8u))
					PLAYER(current_player)->climb_from_top = 0xFFu;
				if (PLAYER(current_player)->climb_from_top == 0u || PLAYER(current_player)->previous_state != 4u)
					goto player_climbing_start;
				PLAYER(current_player)->state = 9u;
				PLAYER(current_player)->substate = 0u;
				goto player_dispatch_state;
			case 1:
				goto player_climbing_grab_wall;
			case 2:
				goto player_climbing_descend_over_edge;
			case 3:
				goto player_climbing_move;
			case 4:
				goto player_climbing_pull_up;
			default:
				invalid_jump(0x4755u + PLAYER(current_player)->substate * 3u);
			}
		}
	}
player_climbing_start:
	OBJECT(current_object)->flags &= ~0x40u;
	OBJECT(current_object)->sprite_palette = ROM16(OBJECT(current_object)->player_view.palette_list + 2u);
	OBJECT(current_object)->flags &= ~0x2000u;
	OBJECT(current_object)->speed = 0;
	PLAYER(current_player)->control_lock = 0u;
	PLAYER(current_player)->substate++;
player_climbing_grab_wall:
	along_wall = current_object + 0x50u;
	if (ROM8(attribute_record) != 0u)
		along_wall = current_object + 0x4Cu;
	wall_limit = ROM16(attribute_record + 0xAu);
	if ((int16_t)mem_read16(along_wall + 2u) < (int16_t)wall_limit)
	{
	player_climbing_clamp_to_wall:
		mem_write16(along_wall, 0u);
		mem_write16(along_wall + 2u, wall_limit);
	}
	else
	{
		wall_limit += ROM16(attribute_record + 0xCu);
		if ((int16_t)wall_limit < (int16_t)mem_read16(along_wall + 2u))
			goto player_climbing_clamp_to_wall;
	}
	if (PLAYER(current_player)->climb_from_top == 0u)
	{
		PLAYER(current_player)->climb_remaining = ROM16(attribute_record + 6u);
		OBJECT(current_object)->flags &= ~0x80u;
		climb_animation = sonic_climb_animation_ADDRESS;
		if (OBJECT(current_object)->kind != 0u)
		{
			climb_animation = ray_climb_animation_ADDRESS;
			if (OBJECT(current_object)->kind != 1u)
				climb_animation = mighty_climb_animation_ADDRESS;
		}
		OBJECT(current_object)->animation = climb_animation;
		OBJECT(current_object)->sprite_flags |= 0x40u;
		if (ROM8(attribute_record) != 0u)
			OBJECT(current_object)->sprite_flags &= 0xFFBFu;
		OBJECT(current_object)->animation_timer = 0;
		OBJECT(current_object)->animation_speed = 3u;
		OBJECT(current_object)->animation_frame = 0xFFu;
		floor_block_record = floor_blocks + 2u + OBJECT(current_object)->floor_block;
		if (ROM8(attribute_record) == 0u)
			OBJECT(current_object)->x = FLOOR_BLOCK(floor_block_record)->x_min;
		else
			OBJECT(current_object)->y = FLOOR_BLOCK(floor_block_record)->y_max;
		OBJECT(current_object)->z = OBJECT(current_object)->floor_z;
		PLAYER(current_player)->climb_base_z = OBJECT(current_object)->floor_z;
		project_object(&ignored, &depth);
		PLAYER(current_player)->screen_x_int = OBJECT(current_object)->projected_x;
		PLAYER(current_player)->screen_x_fraction = 0u;
	player_climbing_begin_climb:
		PLAYER(current_player)->step_accumulator = 0;
		PLAYER(current_player)->substate = 3u;
	player_climbing_move:
		if ((int8_t)OBJECT(current_object)->animation_speed >= 0)
		{
		player_climbing_check_vertical_step:
			if (PLAYER(current_player)->step_accumulator_int >= 4)
			{
				OBJECT(current_object)->flags |= 0x80u;
				goto player_climbing_start_vertical_step;
			}
			if (PLAYER(current_player)->step_accumulator_int <= -4)
			{
				OBJECT(current_object)->flags &= ~0x80u;
			player_climbing_start_vertical_step:
				PLAYER(current_player)->step_accumulator = 0;
				PLAYER(current_player)->side_step_accumulator = 0;
				climb_animation = sonic_climb_animation_ADDRESS;
				if (OBJECT(current_object)->kind != 0u)
				{
					climb_animation = ray_climb_animation_ADDRESS;
					if (OBJECT(current_object)->kind != 1u)
						climb_animation = mighty_climb_animation_ADDRESS;
				}
				if (OBJECT(current_object)->animation != climb_animation)
				{
					OBJECT(current_object)->animation = climb_animation;
					OBJECT(current_object)->animation_frame = 0u;
				}
				OBJECT(current_object)->animation_speed = 4u;
				OBJECT(current_object)->animation_timer = 0;
				depth = 0x180000u;
				if ((OBJECT(current_object)->flags & 0x80u) == 0u)
					depth = 0u - depth;
				step_target = depth + OBJECT(current_object)->z;
				if (PLAYER(current_player)->climb_base_z < (int32_t)step_target)
				{
				player_climbing_block_step:
					OBJECT(current_object)->animation_speed = 0xFFu;
					PLAYER(current_player)->step_accumulator = 0;
				}
				else
				{
					step_target = (uint16_t)(((int32_t)(step_target + OBJECT(current_object)->projected_y) >> 16) - view_y);
					if ((OBJECT(current_object)->flags & 0x80u) != 0u)
					{
						if ((int16_t)step_target >= 0xD0)
							goto player_climbing_block_step;
					}
					else if ((int16_t)step_target <= 0x10)
						goto player_climbing_block_step;
				}
			}
			if ((int8_t)OBJECT(current_object)->animation_speed >= 0)
			{
				climb_speed = (uint16_t)OBJECT(current_object)->speed_y_int;
				if ((int16_t)climb_speed < 0)
					climb_speed = (uint16_t)(0u - climb_speed);
				if ((int16_t)climb_speed > 3)
					climb_speed = 3u;
				depth = (depth & ~0xFFFFu) | 4u;
				depth = (depth & ~0xFFFFu) | (uint16_t)(depth - climb_speed);
				if (PLAYER(current_player)->fast_climb != 0u)
					depth = (depth & ~0xFFFFu) | (uint16_t)(depth << 2);
				OBJECT(current_object)->animation_speed = depth;
				animate(&depth, &ignored, &ignored);
				if (OBJECT(current_object)->animation_speed == (uint8_t)OBJECT(current_object)->animation_timer)
				{
					if (OBJECT(current_object)->animation_frame != 6u)
					{
						if (OBJECT(current_object)->animation_frame == 0u)
							OBJECT(current_object)->animation_speed = 0xFFu;
					}
					else
						OBJECT(current_object)->animation_speed = 0xFFu;
					PLAYER(current_player)->previous_z = OBJECT(current_object)->z;
					z_step = 2u;
					if ((OBJECT(current_object)->flags & 0x80u) == 0u)
						z_step = 0u - z_step;
					OBJECT(current_object)->z_int += z_step;
					PLAYER(current_player)->saved_climb_remaining = PLAYER(current_player)->climb_remaining;
					PLAYER(current_player)->climb_remaining += z_step;
					PLAYER(current_player)->saved_screen_x = PLAYER(current_player)->screen_x;
					screen_shift = (int16_t)ROM16(attribute_record + 4u) * z_step;
					depth = screen_shift;
					if (ROM8(attribute_record) == 0u)
						depth = 0u - depth;
					PLAYER(current_player)->screen_x += depth;
					OBJECT(current_object)->projected_x = PLAYER(current_player)->screen_x_int;
					PLAYER(current_player)->saved_screen_y = OBJECT(current_object)->projected_y;
					OBJECT(current_object)->projected_y += (int32_t)screen_shift >> 1;
				}
			}
		}
		else
		{
			depth = set_climb_speed_from_trackball();
			if ((uint8_t)PLAYER(current_player)->pushed_out != 0u)
			{
				OBJECT(current_object)->direction = 0u;
				OBJECT(current_object)->speed = 0x3000000;
				depth = set_speed_from_direction();
			}
			PLAYER(current_player)->fast_climb = 0u;
			if (mem_read8(floor_blocks + 2u + OBJECT(current_object)->floor_block + 0x12u) == 0x20u && OBJECT(current_object)->z_int > 0x4DE)
				PLAYER(current_player)->fast_climb = 0xFFu;
			PLAYER(current_player)->target_heading = OBJECT(current_object)->direction;
			if ((uint16_t)((OBJECT(current_object)->direction & 0x1FFu) - 0x80u) >= 0x100u)
			{
				if (PLAYER(current_player)->step_accumulator_int < 0)
				{
					if (OBJECT(current_object)->speed_y_int >= 0)
					{
					player_climbing_reset_vertical_input:
						PLAYER(current_player)->step_accumulator = 0;
					}
				}
				else if (OBJECT(current_object)->speed_y_int < 0)
					goto player_climbing_reset_vertical_input;
				PLAYER(current_player)->step_accumulator += OBJECT(current_object)->speed_y;
				goto player_climbing_check_vertical_step;
			}
			if (PLAYER(current_player)->side_step_accumulator_int < 0)
			{
				if (OBJECT(current_object)->speed_x_int >= 0)
				{
				player_climbing_reset_side_input:
					PLAYER(current_player)->side_step_accumulator = 0;
				}
			}
			else if (OBJECT(current_object)->speed_x_int < 0)
				goto player_climbing_reset_side_input;
			PLAYER(current_player)->side_step_accumulator += OBJECT(current_object)->speed_x;
			side_threshold = 2u;
			if (PLAYER(current_player)->fast_climb != 0u)
				side_threshold = (uint16_t)((uint32_t)side_threshold << 2);
			if ((int16_t)side_threshold <= PLAYER(current_player)->side_step_accumulator_int)
			{
				OBJECT(current_object)->flags &= ~0x80u;
				goto player_climbing_side_step;
			}
			if (PLAYER(current_player)->side_step_accumulator_int <= (int16_t)(0u - side_threshold))
			{
				OBJECT(current_object)->flags |= 0x80u;
			player_climbing_side_step:
				PLAYER(current_player)->side_step_accumulator = 0;
				PLAYER(current_player)->step_accumulator = 0;
				climb_sideways_animation = sonic_climb_sideways_animation_ADDRESS;
				if (OBJECT(current_object)->kind != 0u)
				{
					climb_sideways_animation = ray_climb_sideways_animation_ADDRESS;
					if (OBJECT(current_object)->kind != 1u)
						climb_sideways_animation = mighty_climb_sideways_animation_ADDRESS;
				}
				if (OBJECT(current_object)->animation != climb_sideways_animation)
				{
					OBJECT(current_object)->animation = climb_sideways_animation;
					OBJECT(current_object)->animation_frame = 0u;
				}
				side_step = 0x20000u;
				if ((OBJECT(current_object)->flags & 0x80u) != 0u)
					side_step = 0u - side_step;
				step_x = PLAYER(current_player)->screen_x + side_step;
				side_step_screen_x = step_x;
				side_step = (int32_t)side_step >> 1;
				if (ROM8(attribute_record) != 0u)
					side_step = 0u - side_step;
				side_step += OBJECT(current_object)->projected_y;
				step_projected_y = side_step;
				side_step = side_step * 2u;
				unproject_point(&step_x, &side_step);
				if (ROM8(attribute_record) != 0u)
					side_step = step_x;
				side_step = (int32_t)side_step >> 16;
				wall_start = ROM16(attribute_record + 0xAu);
				if ((int16_t)wall_start <= (int16_t)side_step && (int16_t)side_step <= (int16_t)(wall_start + ROM16(attribute_record + 0xCu)) && (uint16_t)(((int32_t)((uint32_t)step_projected_y + OBJECT(current_object)->z + depth) >> 16) - view_y - 0x10u) < 0xC0u)
				{
					PLAYER(current_player)->saved_screen_x = PLAYER(current_player)->screen_x;
					PLAYER(current_player)->saved_screen_y = OBJECT(current_object)->projected_y;
					PLAYER(current_player)->screen_x = side_step_screen_x;
					OBJECT(current_object)->projected_x = PLAYER(current_player)->screen_x_int;
					OBJECT(current_object)->projected_y = step_projected_y;
					animate_step(&depth, &ignored, &ignored);
				}
			}
		}
	player_climbing_update_position:
		if (PLAYER(current_player)->climb_remaining > 0)
		{
			if ((int16_t)ROM16(attribute_record + 6u) <= PLAYER(current_player)->climb_remaining)
			{
				across_wall = current_object + 0x4Eu;
				if (ROM8(attribute_record) != 0u)
					across_wall = current_object + 0x52u;
				mem_write16(across_wall, mem_read16(across_wall) - 2u);
				PLAYER(current_player)->substate = 0u;
				PLAYER(current_player)->state = 0u;
				PLAYER(current_player)->free_turn_timer = 3u;
				OBJECT(current_object)->flags &= ~0x80u;
			}
			update_screen_position(&ignored, &depth);
			world_x = PLAYER(current_player)->screen_x;
			world_y = OBJECT(current_object)->projected_y * 2u;
			unproject_point(&world_x, &world_y);
			OBJECT(current_object)->x = world_x;
			OBJECT(current_object)->y = world_y;
			add_player_to_lists();
			if (mem_read8(floor_blocks + 2u + OBJECT(current_object)->floor_block + 0x12u) == 0x20u)
			{
				OBJECT(current_object)->sprite_palette &= 0x1FFFu;
				OBJECT(current_object)->sprite_palette |= 0x8000u;
				if (OBJECT(current_object)->z_int >= 0x4B7)
				{
					OBJECT(current_object)->sprite_palette &= 0x1FFFu;
					OBJECT(current_object)->sprite_palette |= 0x4000u;
				}
				PLAYER(current_player)->splash_zone = 0u;
				if (OBJECT(current_object)->z_int >= 0x4C7 && OBJECT(current_object)->z_int <= 0x4E9)
				{
					PLAYER(current_player)->splash_zone = 0xFFu;
					if (PLAYER(current_player)->was_in_splash_zone == 0u)
						spawn_climb_zone_effect(cursor);
				}
				PLAYER(current_player)->was_in_splash_zone = PLAYER(current_player)->splash_zone;
			}
			if ((OBJECT(current_object)->flags & 0x80000u) != 0u)
				return;
			depth = queue_sprite(depth);
			return;
		}
		climb_over_animation = sonic_climb_over_animation_ADDRESS;
		if (OBJECT(current_object)->kind != 0u)
		{
			climb_over_animation = ray_climb_over_animation_ADDRESS;
			if (OBJECT(current_object)->kind != 1u)
				climb_over_animation = mighty_climb_over_animation_ADDRESS;
		}
		OBJECT(current_object)->animation = climb_over_animation;
		OBJECT(current_object)->animation_frame = 0xFFu;
		OBJECT(current_object)->animation_timer = 0;
		OBJECT(current_object)->animation_speed = 3u;
		OBJECT(current_object)->anchor_x = OBJECT(current_object)->projected_x;
		OBJECT(current_object)->anchor_y = OBJECT(current_object)->projected_y_int;
		PLAYER(current_player)->substate++;
	player_climbing_pull_up:
		at_last_step = animate_around_anchor();
		pull_up_done = at_last_step;
		update_screen_position(&ignored, &depth);
		if (pull_up_done != 0u)
		{
			OBJECT(current_object)->z_int = ROM16(attribute_record + 8u);
			OBJECT(current_object)->z_fraction = 0u;
			OBJECT(current_object)->floor_z = OBJECT(current_object)->z;
			OBJECT(current_object)->flags &= ~0x80u;
			PLAYER(current_player)->state = 0u;
			PLAYER(current_player)->free_turn_timer = 3u;
			PLAYER(current_player)->substate = 0u;
			OBJECT(current_object)->speed = 0;
			OBJECT(current_object)->speed_x = 0;
			OBJECT(current_object)->speed_y = 0;
			PLAYER(current_player)->animation_speed = 0;
			floor_block_record = floor_blocks + 2u + OBJECT(current_object)->floor_block;
			if (ROM8(attribute_record) == 0u)
			{
				OBJECT(current_object)->x = FLOOR_BLOCK(floor_block_record)->x_max;
				OBJECT(current_object)->x_int += 0xAu;
			}
			else
			{
				OBJECT(current_object)->y = FLOOR_BLOCK(floor_block_record)->y_min;
				OBJECT(current_object)->y_int -= 0xAu;
			}
			new_heading = 0x80u;
			if (ROM8(attribute_record) != 0u)
				new_heading = 0x380u;
			PLAYER(current_player)->target_heading = new_heading;
			turn_player();
		}
		add_player_to_lists();
		if ((OBJECT(current_object)->flags & 0x80000u) != 0u)
			return;
		depth = queue_sprite(depth);
		return;
	}
	OBJECT(current_object)->x = mem_read32(floor_blocks + 2u + OBJECT(current_object)->floor_block + 4u);
	project_object(&ignored, &depth);
	OBJECT(current_object)->projected_x -= 0x18u;
	OBJECT(current_object)->projected_y_int -= 0x1Cu;
	OBJECT(current_object)->z_int += 0x14u;
	OBJECT(current_object)->anchor_x = OBJECT(current_object)->projected_x;
	OBJECT(current_object)->anchor_x += 0xDu;
	OBJECT(current_object)->anchor_y = OBJECT(current_object)->projected_y_int;
	OBJECT(current_object)->anchor_y += 0x1Eu;
	climb_over_animation = sonic_climb_over_animation_ADDRESS;
	if (OBJECT(current_object)->kind != 0u)
	{
		climb_over_animation = ray_climb_over_animation_ADDRESS;
		if (OBJECT(current_object)->kind != 1u)
			climb_over_animation = mighty_climb_over_animation_ADDRESS;
	}
	OBJECT(current_object)->animation = climb_over_animation;
	OBJECT(current_object)->sprite_flags |= 0x40u;
	OBJECT(current_object)->animation_frame = 0u;
	OBJECT(current_object)->animation_timer = 0;
	OBJECT(current_object)->animation_speed = 3u;
	PLAYER(current_player)->climb_remaining = 0;
	OBJECT(current_object)->flags |= 0x80u;
	PLAYER(current_player)->substate++;
player_climbing_descend_over_edge:
	at_last_step = animate_around_anchor();
	if ((uint8_t)at_last_step == 0u)
	{
		update_screen_position(&ignored, &depth);
		add_player_to_lists();
		if ((OBJECT(current_object)->flags & 0x80000u) != 0u)
			return;
		depth = queue_sprite(depth);
		return;
	}
	climb_animation = sonic_climb_animation_ADDRESS;
	if (OBJECT(current_object)->kind != 0u)
	{
		climb_animation = ray_climb_animation_ADDRESS;
		if (OBJECT(current_object)->kind != 1u)
			climb_animation = mighty_climb_animation_ADDRESS;
	}
	OBJECT(current_object)->animation = climb_animation;
	OBJECT(current_object)->animation_timer = 0;
	OBJECT(current_object)->animation_speed = 3u;
	OBJECT(current_object)->animation_frame = 6u;
	PLAYER(current_player)->screen_x_int = OBJECT(current_object)->anchor_x;
	PLAYER(current_player)->screen_x_fraction = 0u;
	OBJECT(current_object)->projected_y_int = OBJECT(current_object)->anchor_y;
	OBJECT(current_object)->projected_y_fraction = 0u;
	goto player_climbing_begin_climb;
}

void player_state_not_playing(void)
{
	uint32_t joined = 0, player = 0, text = 0, text_position = 0;

	OBJECT(current_object)->flags |= 0x400u;
	PLAYER(current_player)->in_game = 0;
	OBJECT(current_object)->health = 0;
	PLAYER(current_player)->enter_on_respawn = 0u;
	player_status[OBJECT(current_object)->player] = 0u;
	if (joining_closed == 0u)
	{
		joined = try_join();
		if (joined == 0u)
			return;
		OBJECT(current_object)->handler = player_join_ADDRESS;
		PLAYER(current_player)->substate = 0u;
		PLAYER(current_player)->is_joining = 0xFFu;
		PLAYER(current_player)->in_game = 1;
		PLAYER(current_player)->rings = 0u;
		PLAYER(current_player)->stages_reached = 1u;
		text = hud_message_blank_text_ADDRESS;
		player = OBJECT(current_object)->player;
		text_position = 0x310000u | hud_text_positions_3p[player];
		if ((cabinet_config & 1u) == 0u)
			text_position = (text_position & ~0xFFFFu) | hud_text_positions_2p[player];
		erase_text_lines(&text, &text_position);
	}
}

void player_animate_skidding(uint32_t depth, uint32_t cursor)
{
	uint8_t sound_id = 0;
	uint32_t ground_slowdown = 0, ignored = 0, skid_animations = 0, skid_facing = 0, speed_slowdown = 0;

	PLAYER(current_player)->control_lock = 0xFFu;
	start_jump_if_pressed();
	if (PLAYER(current_player)->state == 4u)
	{
		PLAYER(current_player)->control_lock = 0u;
		PLAYER(current_player)->free_turn_timer = 3u;
		PLAYER(current_player)->animation_speed = 0;
		player_animate(depth, cursor);
		return;
	}
	switch (PLAYER(current_player)->substate)
	{
	case 0:
	case 3:
		skid_animations = sonic_skid_animations_ADDRESS;
		if (OBJECT(current_object)->kind != 0u)
		{
			skid_animations = ray_skid_animations_ADDRESS;
			if (OBJECT(current_object)->kind != 1u)
				skid_animations = mighty_skid_animations_ADDRESS;
		}
		skid_facing = PLAYER(current_player)->facing;
		skid_facing = (uint8_t)(skid_facing + 8u);
		OBJECT(current_object)->animation = skid_animations + ROM16(skid_animations + (skid_facing & 0xFu) * 2u);
		OBJECT(current_object)->animation_speed = 3u;
		OBJECT(current_object)->animation_timer = 0;
		OBJECT(current_object)->animation_frame = 0u;
		PLAYER(current_player)->skid_dust_timer = 0;
		speed_slowdown = (int32_t)OBJECT(current_object)->speed >> 3;
		if (OBJECT(current_object)->floor_type == 7u)
			speed_slowdown = (int32_t)speed_slowdown >> 2;
		PLAYER(current_player)->skid_speed_slowdown = speed_slowdown;
		ground_slowdown = (int32_t)PLAYER(current_player)->ground_speed >> 3;
		if (OBJECT(current_object)->floor_type == 7u)
			ground_slowdown = (int32_t)ground_slowdown >> 2;
		PLAYER(current_player)->skid_slowdown = ground_slowdown;
		sound_id = 0x27u;
		play_sound(sound_id, &ignored, &ignored);
		PLAYER(current_player)->substate++;
		player_animate_skidding_slow_down(cursor);
		return;
	case 1:
		player_animate_skidding_slow_down(cursor);
		return;
	case 2:
		animate_with_flags(&ignored, &ignored);
		if (OBJECT(current_object)->animation_frame == 0u)
		{
			PLAYER(current_player)->control_lock = 0u;
			PLAYER(current_player)->free_turn_timer = 3u;
			PLAYER(current_player)->state = 1u;
		}
		if ((OBJECT(current_object)->flags & 0x80000u) != 0u)
			return;
		if ((PLAYER(current_player)->flags & 1u) == 0u)
		{
			ignored = queue_sprite_at_base(ignored);
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
		return;
	default:
		invalid_jump(0x4EF5u + PLAYER(current_player)->substate * 3u);
	}
}

void player_animate_skidding_slow_down(uint32_t cursor)
{
	uint32_t depth = 0, ignored = 0;
	uint8_t skid_dust_timer = 0;
	uint32_t skid_speed_slowdown = 0, speed = 0;

	skid_dust_timer = PLAYER(current_player)->skid_dust_timer;
	PLAYER(current_player)->skid_dust_timer = skid_dust_timer - 1u;
	if ((int8_t)skid_dust_timer <= 1)
	{
		PLAYER(current_player)->skid_dust_timer = 4;
		if (OBJECT(current_object)->floor_type != 7u && (PLAYER(current_player)->flags & 1u) == 0u)
			spawn_skid_dust(cursor);
	}
	if ((int8_t)OBJECT(current_object)->animation_speed >= 0)
	{
		animate_with_flags(&ignored, &ignored);
		if ((int8_t)OBJECT(current_object)->animation_frame >= 2)
			OBJECT(current_object)->animation_speed = 0xFFu;
	}
	PLAYER(current_player)->ground_speed -= PLAYER(current_player)->skid_slowdown;
	skid_speed_slowdown = PLAYER(current_player)->skid_speed_slowdown;
	speed = OBJECT(current_object)->speed;
	OBJECT(current_object)->speed = speed - skid_speed_slowdown;
	if ((int32_t)speed <= (int32_t)skid_speed_slowdown)
	{
		OBJECT(current_object)->speed = 0;
		PLAYER(current_player)->ground_speed = 0;
		OBJECT(current_object)->animation_speed = 3u;
		PLAYER(current_player)->substate++;
	}
	if ((OBJECT(current_object)->flags & 0x80000u) != 0u)
		return;
	if ((PLAYER(current_player)->flags & 1u) == 0u)
	{
		ignored = queue_sprite_at_base(ignored);
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

void player_state_game_over(uint32_t jump_offset)
{
	if (PLAYER(current_player)->state_timer <= 0)
	{
		PLAYER(current_player)->state_timer++;
		start_ranking_entry();
		jump_offset = OBJECT(current_object)->player;
		player_status[jump_offset] = 3u;
	}
	enter_ranking_initials(jump_offset);
	if (PLAYER(current_player)->game_over_done != 0u)
	{
		PLAYER(current_player)->state = 0x24u;
		PLAYER(current_player)->continue_countdown = 0u;
		PLAYER(current_player)->state_timer = 0;
	}
}

void player_state_short_animation(void)
{
	uint32_t at_last_step = 0, ignored = 0, state_1b_animation = 0;
	uint16_t state_timer = 0;

	if (PLAYER(current_player)->substate == 0u)
	{
		PLAYER(current_player)->substate++;
		state_1b_animation = sonic_state_1b_animation_ADDRESS;
		if (OBJECT(current_object)->kind != 0u)
		{
			state_1b_animation = ray_state_1b_animation_ADDRESS;
			if (OBJECT(current_object)->kind != 1u)
				state_1b_animation = mighty_state_1b_animation_ADDRESS;
		}
		OBJECT(current_object)->animation = state_1b_animation;
		OBJECT(current_object)->animation_frame = 0u;
		OBJECT(current_object)->animation_speed = 5u;
		OBJECT(current_object)->animation_timer = 0;
		PLAYER(current_player)->state_timer = 0x41;
		OBJECT(current_object)->sprite_palette = ROM16(OBJECT(current_object)->player_view.palette_list + 2u);
	}
	if ((int8_t)OBJECT(current_object)->animation_frame >= 0)
	{
		animate(&ignored, &ignored, &at_last_step);
		if ((uint8_t)at_last_step != 0u)
			OBJECT(current_object)->animation_frame = 0xFFu;
	}
	state_timer = PLAYER(current_player)->state_timer;
	PLAYER(current_player)->state_timer = state_timer - 1u;
	if ((int16_t)state_timer <= 1)
	{
		PLAYER(current_player)->grey_screen_request = 0u;
		PLAYER(current_player)->state = 0u;
		PLAYER(current_player)->substate = 0u;
	}
	project_object(&ignored, &ignored);
	ignored = queue_sprite_at_base(ignored);
}

void player_state_inactive_2(void)
{
}

void player_animate_hopping(void)
{
	uint32_t ignored = 0, running_animations = 0, speed_int = 0;

	if (PLAYER(current_player)->substate == 0u)
	{
		PLAYER(current_player)->substate++;
		OBJECT(current_object)->speed_z = -0x80000;
		if (OBJECT(current_object)->floor_type == 6u)
		{
			speed_int = (uint16_t)OBJECT(current_object)->speed_int;
			OBJECT(current_object)->speed_z_int = hop_speeds_by_speed[speed_int >> 8];
		}
		OBJECT(current_object)->sprite_palette = ROM16(OBJECT(current_object)->player_view.palette_list + 2u);
		OBJECT(current_object)->flags &= ~0x40u;
		OBJECT(current_object)->animation_speed = 1u;
		OBJECT(current_object)->animation_timer = 0;
		OBJECT(current_object)->animation_frame = 0xFFu;
	}
	bounce_off_wall_in_air();
	PLAYER(current_player)->jump_delay = 2;
	running_animations = sonic_running_animations_ADDRESS;
	if (OBJECT(current_object)->kind != 0u)
	{
		running_animations = ray_running_animations_ADDRESS;
		if (OBJECT(current_object)->kind != 1u)
			running_animations = mighty_running_animations_ADDRESS;
	}
	OBJECT(current_object)->animation = running_animations + ROM16(running_animations + PLAYER(current_player)->facing * 2u);
	animate_with_flags(&ignored, &ignored);
	OBJECT(current_object)->speed_z += 0x6000u;
	OBJECT(current_object)->z += OBJECT(current_object)->speed_z;
	if (OBJECT(current_object)->floor_z <= OBJECT(current_object)->z)
	{
		PLAYER(current_player)->state = 1u;
		PLAYER(current_player)->substate = 0u;
		OBJECT(current_object)->z = OBJECT(current_object)->floor_z;
		PLAYER(current_player)->jump_delay = 0;
		OBJECT(current_object)->flags &= ~0x2000u;
		OBJECT(current_object)->flags &= ~0x100000u;
		OBJECT(current_object)->flags &= ~0x200000u;
	}
	if ((OBJECT(current_object)->flags & 0x80000u) != 0u)
		return;
	if (OBJECT(current_object)->floor_block >= 0)
	{
		ignored = queue_sprite_at_floor();
		return;
	}
	ignored = queue_sprite_at_previous_floor();
}

void player_state_dying(void)
{
	uint16_t character_frame = 0;
	uint32_t depth = 0, direction = 0, ignored = 0, speed_x_int = 0;
	uint16_t state_timer = 0;

	if (PLAYER(current_player)->substate == 0u)
	{
		PLAYER(current_player)->substate = 1u;
		PLAYER(current_player)->screen_x_int = OBJECT(current_object)->screen_x;
		PLAYER(current_player)->screen_x_fraction = 0u;
		if (PLAYER(current_player)->screen_x_int < 0x20)
			PLAYER(current_player)->screen_x_int = 0x20;
		if (PLAYER(current_player)->screen_x_int > 0x100)
			PLAYER(current_player)->screen_x_int = 0x100;
		OBJECT(current_object)->projected_y_int = OBJECT(current_object)->screen_y;
		if (OBJECT(current_object)->projected_y_int > 0xE0)
		{
			OBJECT(current_object)->projected_y_int = 0xE0;
			if (PLAYER(current_player)->screen_x_int < 0x78)
				PLAYER(current_player)->screen_x_int = 0x78;
			if (PLAYER(current_player)->screen_x_int > 0xC8)
				PLAYER(current_player)->screen_x_int = 0xC8;
		}
		OBJECT(current_object)->projected_y_fraction = 0u;
		OBJECT(current_object)->direction = 0u;
		OBJECT(current_object)->speed_y = -0x10000;
		PLAYER(current_player)->state_timer = 0x40;
		OBJECT(current_object)->speed_x = 0x60000;
		OBJECT(current_object)->sprite_flags &= 0xFFBFu;
		OBJECT(current_object)->wall = 7u;
		if (PLAYER(current_player)->screen_x_int < 0xA0)
		{
			OBJECT(current_object)->sprite_flags |= 0x40u;
			OBJECT(current_object)->wall = 0u - OBJECT(current_object)->wall;
		}
		character_frame = 0x1B50u;
		if (OBJECT(current_object)->kind != 0u)
		{
			character_frame = 0x5270u;
			if (OBJECT(current_object)->kind != 1u)
				character_frame = 0x36B8u;
		}
		OBJECT(current_object)->sprite_frame = character_frame;
		OBJECT(current_object)->sprite_palette = ROM16(OBJECT(current_object)->player_view.palette_list + 2u);
		OBJECT(current_object)->sprite_palette &= 0x9FFFu;
		OBJECT(current_object)->flags |= 0x400u;
		player_status[OBJECT(current_object)->player] = 1u;
	}
	state_timer = PLAYER(current_player)->state_timer;
	PLAYER(current_player)->state_timer = state_timer - 1u;
	if ((int16_t)state_timer <= 1)
	{
		PLAYER(current_player)->state_timer = 0;
		OBJECT(current_object)->speed_x += 0x8000u;
	}
	speed_x_int = (uint16_t)OBJECT(current_object)->speed_x_int;
	OBJECT(current_object)->direction += OBJECT(current_object)->wall;
	direction = OBJECT(current_object)->direction;
	PLAYER(current_player)->screen_x += sine_table[direction & 0x3FFu] * speed_x_int;
	OBJECT(current_object)->screen_x = PLAYER(current_player)->screen_x_int;
	if (PLAYER(current_player)->state_timer <= 0)
		OBJECT(current_object)->speed_y -= 0x3000u;
	OBJECT(current_object)->projected_y += OBJECT(current_object)->speed_y;
	OBJECT(current_object)->screen_y = OBJECT(current_object)->projected_y_int;
	if (OBJECT(current_object)->screen_y <= -0x28)
	{
		player_status[OBJECT(current_object)->player] = 0xFFu;
		PLAYER(current_player)->previous_z_low = 0u;
		PLAYER(current_player)->in_game = 0;
		PLAYER(current_player)->continue_seconds = 9;
		PLAYER(current_player)->continue_frames = 0;
		PLAYER(current_player)->state = 0x23u;
	}
	PLAYER(current_player)->invulnerable_timer = 0x3C;
	toggle_hidden();
	if ((OBJECT(current_object)->flags & 0x80000u) != 0u)
		return;
	depth = 0xE0u;
	queue_sprite_at_depth(&ignored, &depth);
}

void player_state_launched(void)
{
	uint32_t ignored = 0;

	switch (PLAYER(current_player)->substate)
	{
	case 0:
		player_launch_angle_100();
		break;
	case 1:
		player_launch_angle_200();
		break;
	case 2:
		player_launched_fly();
		break;
	case 3:
		player_launched_fly();
		break;
	case 4:
		player_launched_land();
		break;
	case 5:
		player_launch_angle_200();
		break;
	default:
		invalid_jump(0x5437u + PLAYER(current_player)->substate * 3u);
	}
	animate_with_flags(&ignored, &ignored);
	saved_sprite_offset_y = OBJECT(current_object)->sprite_offset_y;
	OBJECT(current_object)->sprite_offset_y = 0x12;
	project_object(&ignored, &ignored);
	OBJECT(current_object)->sprite_offset_y = saved_sprite_offset_y;
	keep_player_on_screen(ignored);
	add_player_to_lists();
	if (OBJECT(current_object)->floor_block >= 0)
	{
		ignored = queue_sprite_at_floor();
		return;
	}
	ignored = queue_sprite_at_previous_floor();
}

void player_state_launched_spare_return(void)
{
}

void player_launch_angle_200(void)
{
	OBJECT(current_object)->direction = 0x200u;
	player_launch();
}

void player_launch_angle_100(void)
{
	OBJECT(current_object)->direction = 0x100u;
	PLAYER(current_player)->substate = 1u;
	player_launch();
}

void player_launch(void)
{
	uint32_t animation_record = 0;

	face_movement_direction(0u);
	OBJECT(current_object)->speed_z = -0x100000;
	OBJECT(current_object)->speed = 0x5000000;
	set_speed_from_direction();
	animation_record = sonic_attack_jump_animations_ADDRESS;
	if (OBJECT(current_object)->kind != 0u)
	{
		animation_record = ray_attack_jump_animations_ADDRESS;
		if (OBJECT(current_object)->kind != 1u)
			animation_record = mighty_attack_jump_animations_ADDRESS;
	}
	animation_record += ROM16(animation_record + PLAYER(current_player)->facing * 2u);
	OBJECT(current_object)->sprite_flags = (OBJECT(current_object)->sprite_flags & ~0xC0u) | ROM16(animation_record);
	animation_record += 2u;
	OBJECT(current_object)->sprite_frame = ROM16(animation_record);
	OBJECT(current_object)->animation = animation_record + 2u;
	OBJECT(current_object)->animation_speed = 3u;
	OBJECT(current_object)->animation_timer = 0;
	OBJECT(current_object)->animation_frame = 0u;
	OBJECT(current_object)->flags &= ~0x40u;
	OBJECT(current_object)->sprite_palette = ROM16(OBJECT(current_object)->player_view.palette_list + 2u);
	OBJECT(current_object)->flags &= ~0x2000u;
	PLAYER(current_player)->control_lock = 0u;
	OBJECT(current_object)->flags |= 0x1000u;
	PLAYER(current_player)->substate++;
	player_launched_fly();
}

void player_launched_fly(void)
{
	uint32_t ignored = 0;

	if (OBJECT(current_object)->screen_x >= 0x120)
	{
		OBJECT(current_object)->speed_x = 0u - OBJECT(current_object)->speed_x;
		OBJECT(current_object)->speed_y = 0u - OBJECT(current_object)->speed_y;
		OBJECT(current_object)->speed_z = 0u - OBJECT(current_object)->speed_z;
	}
	OBJECT(current_object)->speed_z += 0xA000u;
	OBJECT(current_object)->z += OBJECT(current_object)->speed_z;
	PLAYER(current_player)->previous_x = OBJECT(current_object)->x;
	OBJECT(current_object)->x += OBJECT(current_object)->speed_x;
	PLAYER(current_player)->previous_y = OBJECT(current_object)->y;
	OBJECT(current_object)->y += OBJECT(current_object)->speed_y;
	find_floor(&ignored, &ignored, &ignored, &ignored, &ignored);
	if (OBJECT(current_object)->player_view.floor_block_low >= 0 && OBJECT(current_object)->floor_z_int <= OBJECT(current_object)->z_int)
	{
		PLAYER(current_player)->substate++;
		OBJECT(current_object)->z = OBJECT(current_object)->floor_z;
		OBJECT(current_object)->speed_x = (int32_t)OBJECT(current_object)->speed_x >> 1;
		OBJECT(current_object)->speed_y = (int32_t)OBJECT(current_object)->speed_y >> 1;
		OBJECT(current_object)->speed_z = 0u - ((int32_t)OBJECT(current_object)->speed_z >> 1);
	}
}

void player_launched_land(void)
{
	OBJECT(current_object)->speed_z = 0;
	PLAYER(current_player)->state = 1u;
	PLAYER(current_player)->substate = 0u;
	OBJECT(current_object)->flags &= ~0x1000u;
	OBJECT(current_object)->speed = 0;
	PLAYER(current_player)->animation_speed = 0;
	OBJECT(current_object)->speed_x = 0;
	OBJECT(current_object)->speed_y = 0;
}

void player_state_hurt(uint32_t depth, uint32_t cursor)
{
	uint8_t sound_id = 0;
	uint32_t r0 = 0, at_last_step = 0, gravity = 0, hurt_animations = 0, ignored = 0, jump_offset = 0, kind = 0, knock_speed = 0, knock_speed_x = 0, knock_speed_y = 0, object = 0, shocked_animations = 0, step_record = 0;
	uint8_t hurt_contact_delay = 0, health = 0, health_2 = 0;
	uint16_t state_timer = 0, state_timer_2 = 0, state_timer_3 = 0;
	uint32_t push_speed = 0, push_speed_2 = 0;

	for (;;)
	{
		hurt_contact_delay = PLAYER(current_player)->hurt_contact_delay;
		PLAYER(current_player)->hurt_contact_delay = hurt_contact_delay - 1u;
		if ((int8_t)hurt_contact_delay <= 1)
		{
			PLAYER(current_player)->hurt_contact_delay = 0;
			check_zone_contact();
			check_player_contacts(&depth, &cursor);
			if (PLAYER(current_player)->state != 0x28u)
				goto player_hurt_dispatch_substate;
			goto player_dispatch_state;
		}
	player_hurt_dispatch_substate:
		push_player_out_of_wall(&r0, &depth, &cursor);
		PLAYER(current_player)->state = 0xDu;
		switch (PLAYER(current_player)->substate)
		{
		case 0:
		case 9:
			if ((uint8_t)OBJECT(current_object)->health == 0u)
			{
				PLAYER(current_player)->state = 0x16u;
				PLAYER(current_player)->substate = 0u;
				goto player_dispatch_state;
			}
			if (stage != 5u)
			{
				health = OBJECT(current_object)->health;
				OBJECT(current_object)->health = health - 4u;
				if ((int8_t)health <= 4)
					OBJECT(current_object)->health = 0;
			}
			PLAYER(current_player)->invulnerable_timer = 0x258;
			OBJECT(current_object)->flags &= ~0x1000000u;
		player_hurt_knockback:
			OBJECT(current_object)->flags &= ~4u;
			OBJECT(current_object)->flags &= ~0x40u;
			OBJECT(current_object)->flags &= ~0x2000u;
			PLAYER(current_player)->control_lock = 0u;
			PLAYER(current_player)->mishap_flag = 0xFFu;
			PLAYER(current_player)->hurt_contact_delay = 0xA;
			PLAYER(current_player)->substate = 2u;
			OBJECT(current_object)->speed_z = -0x80000;
			if (OBJECT(current_object)->floor_type == 4u)
				OBJECT(current_object)->speed_z = (int32_t)OBJECT(current_object)->speed_z >> 1;
			object = LINKED(WORK_RAM, OBJECT(current_object)->touched_by);
			if (OBJECT(object)->kind != 0x24u)
			{
				knock_speed_x = 0x40000u;
				if (OBJECT(current_object)->floor_type == 4u)
					knock_speed_x = (int32_t)knock_speed_x >> 1;
				knock_speed_y = 0u;
				r0 = OBJECT(object)->direction;
				OBJECT(current_object)->direction = r0;
				OBJECT(current_object)->speed = knock_speed_x << 8;
				if (r0 != 0u)
				{
					if (r0 != 0x200u)
					{
						if (r0 != 0x100u)
							knock_speed_x = 0u - knock_speed_x;
					}
					else
					{
						push_speed = knock_speed_x;
						knock_speed_x = knock_speed_y;
						knock_speed_y = push_speed;
					}
				}
				else
				{
					push_speed_2 = knock_speed_x;
					knock_speed_x = knock_speed_y;
					knock_speed_y = 0u - push_speed_2;
				}
			}
			else
			{
				knock_speed_x = 0u;
				knock_speed_y = 0u;
			}
			OBJECT(current_object)->speed_x = knock_speed_x;
			OBJECT(current_object)->speed_y = knock_speed_y;
			face_movement_direction(r0);
			sound_id = 0x21u;
			play_sound(sound_id, &ignored, &ignored);
			sound_id = 0x55u;
			if (OBJECT(current_object)->kind != 0u)
			{
				sound_id = 0x4Au;
				if (OBJECT(current_object)->kind != 1u)
					sound_id = 0x5Bu;
			}
			play_sound(sound_id, &ignored, &ignored);
		player_hurt_set_animation:
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
			OBJECT(current_object)->sprite_flags |= FLAGGED_STEP(step_record)->flags;
			OBJECT(current_object)->sprite_frame = FLAGGED_STEP(step_record)->frame;
			OBJECT(current_object)->player_view.hurt_flash_period = 3u;
			if ((OBJECT(current_object)->flags & 0x1000000u) != 0u)
				OBJECT(current_object)->player_view.hurt_flash_period = 0xFFu;
			OBJECT(current_object)->player_view.hurt_flash_timer = 3u;
			OBJECT(current_object)->player_view.hurt_flash_on = 0u;
		case 2:
			gravity = 0x6000u;
			if (OBJECT(current_object)->floor_type == 4u && OBJECT(LINKED(current_object, OBJECT(current_object)->touched_by))->kind != 0x27u)
				gravity = gravity >> 2;
			OBJECT(current_object)->speed_z += gravity;
			PLAYER(current_player)->previous_z = OBJECT(current_object)->z;
			OBJECT(current_object)->z += OBJECT(current_object)->speed_z;
			PLAYER(current_player)->previous_x = OBJECT(current_object)->x;
			PLAYER(current_player)->previous_y = OBJECT(current_object)->y;
			OBJECT(current_object)->x += OBJECT(current_object)->speed_x;
			OBJECT(current_object)->y += OBJECT(current_object)->speed_y;
			find_floor(&ignored, &depth, &ignored, &ignored, &cursor);
			check_fall_into_pit();
			if (PLAYER(current_player)->state != 9u)
			{
				if ((int16_t)(OBJECT(current_object)->z_int - OBJECT(current_object)->floor_z_int) >= 8)
				{
					OBJECT(current_object)->x = PLAYER(current_player)->previous_x;
					OBJECT(current_object)->y = PLAYER(current_player)->previous_y;
					OBJECT(current_object)->floor_block = OBJECT(current_object)->previous_floor_block;
					OBJECT(current_object)->floor_z = OBJECT(current_object)->previous_floor_z;
				}
				if (OBJECT(current_object)->floor_z_int <= OBJECT(current_object)->z_int)
				{
					PLAYER(current_player)->substate++;
					PLAYER(current_player)->state_timer = 2;
					OBJECT(current_object)->z = OBJECT(current_object)->floor_z;
					sound_id = 0x20u;
					play_sound(sound_id, &ignored, &ignored);
					OBJECT(current_object)->animation_frame++;
					step_record = OBJECT(current_object)->animation + 2u + OBJECT(current_object)->animation_frame * 4u;
					OBJECT(current_object)->sprite_flags &= ~0xC0u;
					OBJECT(current_object)->sprite_flags |= FLAGGED_STEP(step_record)->flags;
					OBJECT(current_object)->sprite_frame = FLAGGED_STEP(step_record)->frame;
					add_player_to_lists();
					project_object(&ignored, &ignored);
					if (current_game_mode != 0xDu)
						keep_player_on_screen(ignored);
					ignored = queue_sprite_at_floor();
					return;
				}
				if (OBJECT(current_object)->animation_frame == 0u && OBJECT(current_object)->speed_z_int >= 0)
				{
					OBJECT(current_object)->animation_frame++;
					step_record = OBJECT(current_object)->animation + 2u + OBJECT(current_object)->animation_frame * 4u;
					OBJECT(current_object)->sprite_flags &= ~0xC0u;
					OBJECT(current_object)->sprite_flags |= FLAGGED_STEP(step_record)->flags;
					OBJECT(current_object)->sprite_frame = FLAGGED_STEP(step_record)->frame;
				}
				add_player_to_lists();
				project_object(&ignored, &ignored);
				if (current_game_mode != 0xDu)
					keep_player_on_screen(ignored);
				ignored = queue_sprite_at_floor();
				return;
			}
			OBJECT(current_object)->animation_frame = 0xFFu;
		player_dispatch_state:
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
				continue;
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
				invalid_jump(0x2D3Eu + PLAYER(current_player)->state * 3u);
			}
			break;
		case 1:
			OBJECT(current_object)->flags |= 0x1000000u;
			goto player_hurt_knockback;
		case 3:
			state_timer = PLAYER(current_player)->state_timer;
			PLAYER(current_player)->state_timer = state_timer - 1u;
			if ((int16_t)state_timer <= 1)
			{
				PLAYER(current_player)->substate++;
				OBJECT(current_object)->speed_z = -0x40000;
				if (OBJECT(current_object)->floor_type == 4u)
					OBJECT(current_object)->speed_z = (int32_t)OBJECT(current_object)->speed_z >> 1;
				OBJECT(current_object)->animation_timer = 0;
				OBJECT(current_object)->animation_speed = 3u;
				OBJECT(current_object)->speed_x = 0;
				OBJECT(current_object)->speed_y = 0;
			}
			if ((int8_t)OBJECT(current_object)->player_view.hurt_flash_period >= 0)
				flash_hurt_palette();
			find_floor(&ignored, &ignored, &ignored, &ignored, &ignored);
			project_object(&ignored, &ignored);
			keep_player_on_screen(ignored);
			add_player_to_lists();
			ignored = queue_sprite_at_base(ignored);
			return;
		case 4:
			r0 = 0x6000u;
			if (OBJECT(current_object)->floor_type == 4u)
				r0 = r0 >> 2;
			OBJECT(current_object)->speed_z += r0;
			PLAYER(current_player)->previous_z = OBJECT(current_object)->z;
			OBJECT(current_object)->z += OBJECT(current_object)->speed_z;
			PLAYER(current_player)->previous_x = OBJECT(current_object)->x;
			PLAYER(current_player)->previous_y = OBJECT(current_object)->y;
			OBJECT(current_object)->x += OBJECT(current_object)->speed_x;
			OBJECT(current_object)->y += OBJECT(current_object)->speed_y;
			if (OBJECT(current_object)->z_int < OBJECT(current_object)->floor_z_int)
			{
				if ((int8_t)OBJECT(current_object)->animation_speed >= 0)
				{
					animate_with_flags(&depth, &ignored);
					if (OBJECT(current_object)->animation_frame == 5u)
						OBJECT(current_object)->animation_speed = 0xFFu;
				}
				if ((int8_t)OBJECT(current_object)->player_view.hurt_flash_period >= 0)
					flash_hurt_palette();
				find_floor(&ignored, &depth, &ignored, &ignored, &cursor);
				if (current_game_mode == 0x1Au)
				{
					project_object(&ignored, &ignored);
					keep_player_on_screen(ignored);
					add_player_to_lists();
					ignored = queue_sprite_at_floor();
					return;
				}
				check_fall_into_pit();
				if (PLAYER(current_player)->state != 9u)
				{
					project_object(&ignored, &ignored);
					keep_player_on_screen(ignored);
					add_player_to_lists();
					ignored = queue_sprite_at_floor();
					return;
				}
				OBJECT(current_object)->animation_frame = 0xFFu;
				goto player_dispatch_state;
			}
			goto player_hurt_land_after_hop;
		case 5:
			goto player_hurt_get_up;
		case 6:
			state_timer_2 = PLAYER(current_player)->state_timer;
			PLAYER(current_player)->state_timer = state_timer_2 - 1u;
			if ((int16_t)state_timer_2 <= 1)
			{
				PLAYER(current_player)->state = 1u;
				OBJECT(current_object)->flags &= ~4u;
				PLAYER(current_player)->substate = 0u;
				OBJECT(current_object)->speed = 0;
				PLAYER(current_player)->animation_speed = 0;
			}
			else
			{
				start_jump_if_pressed();
				if (PLAYER(current_player)->state == 4u)
				{
					OBJECT(current_object)->flags &= ~4u;
					PLAYER(current_player)->substate = 0u;
					OBJECT(current_object)->speed = 0;
					PLAYER(current_player)->animation_speed = 0;
					update_player_blink();
					find_floor(&ignored, &ignored, &ignored, &ignored, &ignored);
					OBJECT(current_object)->z = OBJECT(current_object)->floor_z;
					project_object(&ignored, &ignored);
					keep_player_on_screen(ignored);
					add_player_to_lists();
					if ((OBJECT(current_object)->flags & 0x80000u) != 0u)
						return;
					ignored = queue_sprite_at_base(ignored);
					return;
				}
				read_player_trackball();
				if ((PLAYER(current_player)->trackball_x | PLAYER(current_player)->trackball_y) != 0u)
				{
					PLAYER(current_player)->state = 1u;
					OBJECT(current_object)->flags &= ~4u;
					PLAYER(current_player)->substate = 0u;
					OBJECT(current_object)->speed = 0;
					PLAYER(current_player)->animation_speed = 0;
					update_player_blink();
					find_floor(&ignored, &ignored, &ignored, &ignored, &ignored);
					OBJECT(current_object)->z = OBJECT(current_object)->floor_z;
					project_object(&ignored, &ignored);
					keep_player_on_screen(ignored);
					add_player_to_lists();
					if ((OBJECT(current_object)->flags & 0x80000u) != 0u)
						return;
					ignored = queue_sprite_at_base(ignored);
					return;
				}
			}
			update_player_blink();
			find_floor(&ignored, &ignored, &ignored, &ignored, &ignored);
			OBJECT(current_object)->z = OBJECT(current_object)->floor_z;
			project_object(&ignored, &ignored);
			keep_player_on_screen(ignored);
			add_player_to_lists();
			if ((OBJECT(current_object)->flags & 0x80000u) != 0u)
				return;
			ignored = queue_sprite_at_base(ignored);
			return;
		case 7:
			if ((uint8_t)OBJECT(current_object)->health != 0u)
			{
				health_2 = OBJECT(current_object)->health;
				OBJECT(current_object)->health = health_2 - 4u;
				if ((int8_t)health_2 <= 4)
					OBJECT(current_object)->health = 0;
				PLAYER(current_player)->invulnerable_timer = 0x258;
				OBJECT(current_object)->flags &= ~0x2000u;
				OBJECT(current_object)->flags &= ~0x40u;
				PLAYER(current_player)->control_lock = 0u;
				PLAYER(current_player)->mishap_flag = 0xFFu;
				PLAYER(current_player)->substate++;
				PLAYER(current_player)->state_timer = 4;
				OBJECT(current_object)->speed_z = -0x80000;
				object = LINKED(current_object, OBJECT(current_object)->touched_by);
				if (OBJECT(object)->kind != 0x15u)
				{
					knock_speed = 0x40000u;
					switch (OBJECT(object)->spawn_arg_2_high)
					{
					case 0:
					case 4:
						OBJECT(current_object)->speed_x = 0;
						OBJECT(current_object)->speed_y = 0u - knock_speed;
						OBJECT(current_object)->direction = 0x200u;
						break;
					case 1:
						OBJECT(current_object)->speed_x = 0;
						OBJECT(current_object)->speed_y = knock_speed;
						OBJECT(current_object)->direction = 0u;
						break;
					case 2:
						OBJECT(current_object)->speed_x = knock_speed;
						OBJECT(current_object)->speed_y = 0;
						OBJECT(current_object)->direction = 0x300u;
						break;
					case 3:
						OBJECT(current_object)->speed_x = 0u - knock_speed;
						OBJECT(current_object)->speed_y = 0;
						OBJECT(current_object)->direction = 0x100u;
						break;
					default:
						invalid_jump(0x5AE7u + OBJECT(object)->spawn_arg_2_high * 2u);
					}
				}
				else
				{
					OBJECT(current_object)->speed_x = 0;
					OBJECT(current_object)->speed_y = 0;
					OBJECT(current_object)->direction = 0x300u;
				}
				face_movement_direction(r0);
				shocked_animations = sonic_shocked_animations_ADDRESS;
				if (OBJECT(current_object)->kind != 0u)
				{
					shocked_animations = ray_shocked_animations_ADDRESS;
					if (OBJECT(current_object)->kind != 1u)
						shocked_animations = mighty_shocked_animations_ADDRESS;
				}
				depth = ROM16(shocked_animations + PLAYER(current_player)->facing * 2u);
				OBJECT(current_object)->animation = shocked_animations + depth;
				OBJECT(current_object)->animation_frame = 0u;
				OBJECT(current_object)->animation_speed = 2u;
				OBJECT(current_object)->animation_timer = 0;
				sound_id = 0x18u;
				play_sound(sound_id, &depth, &ignored);
				sound_id = 0x2Fu;
				if (OBJECT(current_object)->kind != 0u)
				{
					sound_id = 0x37u;
					if (OBJECT(current_object)->kind != 1u)
						sound_id = 0x77u;
				}
				play_sound(sound_id, &depth, &ignored);
			player_hurt_shocked:
				animate_with_flags(&depth, &at_last_step);
				if ((uint8_t)at_last_step == 0u)
				{
					kind = OBJECT(current_object)->kind;
					OBJECT(current_object)->sprite_palette = ROM16((uint16_t)(kind << 4) + shocked_palettes_ADDRESS + OBJECT(current_object)->animation_frame * 2u);
					update_screen_position(&ignored, &depth);
					keep_player_on_screen(depth);
					add_player_to_lists();
					depth = queue_sprite_at_base(depth);
					return;
				}
				state_timer_3 = PLAYER(current_player)->state_timer;
				PLAYER(current_player)->state_timer = state_timer_3 - 1u;
				if ((int16_t)state_timer_3 > 1)
				{
					kind = OBJECT(current_object)->kind;
					OBJECT(current_object)->sprite_palette = ROM16((uint16_t)(kind << 4) + shocked_palettes_ADDRESS + OBJECT(current_object)->animation_frame * 2u);
					update_screen_position(&ignored, &depth);
					keep_player_on_screen(depth);
					add_player_to_lists();
					depth = queue_sprite_at_base(depth);
					return;
				}
				PLAYER(current_player)->substate = 2u;
				OBJECT(current_object)->sprite_palette = ROM16(OBJECT(current_object)->player_view.palette_list + 2u);
				goto player_hurt_set_animation;
			}
			PLAYER(current_player)->state = 0x16u;
			PLAYER(current_player)->substate = 0u;
			goto player_dispatch_state;
		case 8:
			goto player_hurt_shocked;
		default:
			invalid_jump(0x55C8u + PLAYER(current_player)->substate * 3u);
		}
	}
player_hurt_land_after_hop:
	PLAYER(current_player)->substate++;
	OBJECT(current_object)->z = OBJECT(current_object)->floor_z;
	OBJECT(current_object)->animation_timer = 0;
player_hurt_get_up:
	OBJECT(current_object)->animation_speed = 3u;
	animate_with_flags(&ignored, &at_last_step);
	if ((uint8_t)at_last_step != 0u)
	{
		PLAYER(current_player)->substate++;
		PLAYER(current_player)->mishap_flag = 0u;
		OBJECT(current_object)->sprite_palette = ROM16(OBJECT(current_object)->player_view.palette_list + 2u);
		set_floor_sprite_priority();
		OBJECT(current_object)->player_view.hurt_flash_timer = 0x7Fu;
		PLAYER(current_player)->state_timer = 0x1E;
		if ((OBJECT(current_object)->flags & 0x1000000u) == 0u)
		{
			PLAYER(current_player)->invulnerable_timer = 0x78;
			OBJECT(current_object)->flags |= 0x20000000u;
		}
		if (OBJECT(current_object)->health <= 0)
		{
			PLAYER(current_player)->state = 0x16u;
			PLAYER(current_player)->substate = 0u;
		}
	}
	if ((int8_t)OBJECT(current_object)->player_view.hurt_flash_period >= 0)
		flash_hurt_palette();
	find_floor(&ignored, &ignored, &ignored, &ignored, &ignored);
	OBJECT(current_object)->z = OBJECT(current_object)->floor_z;
	project_object(&ignored, &ignored);
	keep_player_on_screen(ignored);
	add_player_to_lists();
	ignored = queue_sprite_at_base(ignored);
}

void player_state_hurt_spare_return(void)
{
}

void player_state_bouncing(void)
{
	uint8_t sound_id = 0;
	uint32_t at_last_step = 0, bounce_record = 0, bounce_records = 0, gravity = 0, ignored = 0, new_heading = 0, rebound_speed = 0;
	uint16_t state_timer = 0;

	push_player_out_of_wall(&ignored, &ignored, &ignored);
	if ((OBJECT(current_object)->flags & 0x20u) != 0u)
		PLAYER(current_player)->state = 0xEu;
	switch (PLAYER(current_player)->substate)
	{
	case 0:
	case 4:
		PLAYER(current_player)->substate++;
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
		face_movement_direction(ignored);
		OBJECT(current_object)->speed = 0x1000000;
		OBJECT(current_object)->speed_z = -0x68000;
		PLAYER(current_player)->bounce_speed_z = -0x68000;
		PLAYER(current_player)->bounce_step = 0x34000;
		PLAYER(current_player)->bounce_speed_z += PLAYER(current_player)->bounce_step;
		set_speed_from_direction();
		OBJECT(current_object)->sprite_frame = ROM16(OBJECT(current_object)->animation + 2u);
		OBJECT(current_object)->animation_frame = 0u;
		OBJECT(current_object)->animation_timer = 3;
		OBJECT(current_object)->animation_speed = 3u;
		PLAYER(current_player)->state_timer = 0x14;
		sound_id = 0x25u;
		play_sound(sound_id, &ignored, &ignored);
		add_player_to_lists();
		ignored = queue_sprite_at_floor();
		return;
	case 1:
	case 2:
		gravity = 0x8000u;
		if (OBJECT(current_object)->floor_type == 4u)
			gravity = gravity >> 2;
		OBJECT(current_object)->speed_z += gravity;
		PLAYER(current_player)->previous_z = OBJECT(current_object)->z;
		OBJECT(current_object)->z += OBJECT(current_object)->speed_z;
		PLAYER(current_player)->previous_x = OBJECT(current_object)->x;
		OBJECT(current_object)->x += OBJECT(current_object)->speed_x;
		PLAYER(current_player)->previous_y = OBJECT(current_object)->y;
		OBJECT(current_object)->y += OBJECT(current_object)->speed_y;
		find_floor(&ignored, &ignored, &ignored, &ignored, &ignored);
		if (OBJECT(current_object)->floor_z_int <= OBJECT(current_object)->z_int)
		{
			PLAYER(current_player)->substate++;
			OBJECT(current_object)->z = OBJECT(current_object)->floor_z;
			rebound_speed = (int32_t)OBJECT(current_object)->speed_z >> 1;
			OBJECT(current_object)->speed_z = 0u - rebound_speed;
			PLAYER(current_player)->bounce_speed_z = 0u - rebound_speed;
			PLAYER(current_player)->bounce_step = (int32_t)(rebound_speed * 2u) / 3;
			PLAYER(current_player)->bounce_speed_z += PLAYER(current_player)->bounce_step;
			if ((int8_t)OBJECT(current_object)->animation_frame < 6)
			{
				OBJECT(current_object)->animation_frame++;
				OBJECT(current_object)->sprite_frame = ROM16(OBJECT(current_object)->animation + 2u + OBJECT(current_object)->animation_frame * 2u);
			}
			update_player_blink();
			add_to_rider_list();
			project_object(&ignored, &ignored);
			keep_player_on_screen(ignored);
			if ((OBJECT(current_object)->flags & 0x80000u) != 0u)
				return;
			ignored = queue_sprite_at_floor();
			return;
		}
		if (PLAYER(current_player)->bounce_speed_z <= OBJECT(current_object)->speed_z)
		{
			PLAYER(current_player)->bounce_speed_z += PLAYER(current_player)->bounce_step;
			if ((int8_t)OBJECT(current_object)->animation_frame < 6)
			{
				OBJECT(current_object)->animation_frame++;
				OBJECT(current_object)->sprite_frame = ROM16(OBJECT(current_object)->animation + 2u + OBJECT(current_object)->animation_frame * 2u);
			}
		}
		update_player_blink();
		add_to_rider_list();
		project_object(&ignored, &ignored);
		keep_player_on_screen(ignored);
		if ((OBJECT(current_object)->flags & 0x80000u) != 0u)
			return;
		ignored = queue_sprite_at_floor();
		return;
	case 3:
		state_timer = PLAYER(current_player)->state_timer;
		PLAYER(current_player)->state_timer = state_timer - 1u;
		if ((int16_t)state_timer <= 1)
		{
			PLAYER(current_player)->state = 0u;
			goto player_bouncing_end;
		}
		if ((int8_t)OBJECT(current_object)->animation_speed < 0)
		{
			start_jump_if_pressed();
			if (PLAYER(current_player)->state == 4u)
			{
			player_bouncing_end:
				OBJECT(current_object)->flags &= ~4u;
				PLAYER(current_player)->substate = 0u;
				OBJECT(current_object)->speed = 0;
				PLAYER(current_player)->animation_speed = 0;
				new_heading = (PLAYER(current_player)->target_heading + 0x200u) & 0x3FFu;
				PLAYER(current_player)->target_heading = new_heading;
				PLAYER(current_player)->heading = new_heading;
				PLAYER(current_player)->facing_angle = new_heading;
				set_facing(new_heading);
			}
			else
			{
				read_player_trackball();
				if ((PLAYER(current_player)->trackball_x | PLAYER(current_player)->trackball_y) != 0u)
				{
					PLAYER(current_player)->state = 1u;
					goto player_bouncing_end;
				}
			}
		}
		if ((int8_t)OBJECT(current_object)->animation_speed >= 0)
		{
			animate(&ignored, &ignored, &at_last_step);
			if ((uint8_t)at_last_step != 0u)
				OBJECT(current_object)->animation_speed = 0xFFu;
		}
		update_player_blink();
		update_screen_position(&ignored, &ignored);
		keep_player_on_screen(ignored);
		if ((OBJECT(current_object)->flags & 0x80000u) != 0u)
			return;
		ignored = queue_sprite_at_base(ignored);
		return;
	default:
		invalid_jump(0x5C0Fu + PLAYER(current_player)->substate * 3u);
	}
}
