/**
 *    Name:        player/player.c
 *    Description: the player: starting, joining, resetting and placing a player, player_update, the movement (screen edges, trackball, turning, jumps, walls), the shadow, the floor checks, the restart point, the checks of every frame, and check_player_contacts (what touching each kind of object does)
 *    Version:     0.1.0
 *    Created:     12-10-2026
 *    Modified:    16-10-2026
 *    Author:      Sebastian Toyos
 *
 *    SegaSonic The Hedgehog (arcade, Sega System 32): the game's V60 program decompiled into C by
 *    tools/decompile.py, with the names of symbols.txt.
 */
#include "game.h"

void player_1_start(void)
{
	OBJECT(current_object)->player = 0u;
	player_start();
}

void player_2_start(void)
{
	OBJECT(current_object)->player = 1u;
	player_start();
}

void player_3_start(void)
{
	OBJECT(current_object)->player = 2u;
	player_start();
}

void player_start(void)
{
	uint32_t clip_cursor = 0, joined = 0, records_left = 0;

	current_player = PLAYER_BLOCK(1) + ((uint32_t)OBJECT(current_object)->player << 7) * 4u;
	unused_player_start_byte = 3u;
	PLAYER(current_player)->joined_health_setting = health_setting;
	PLAYER(current_player)->object = current_object;
	OBJECT(current_object)->player_view.block = current_player;
	OBJECT(current_object)->health = starting_health;
	OBJECT(current_object)->handler = player_start_stage_ADDRESS;
	PLAYER(current_player)->state = 0xBu;
	PLAYER(current_player)->stages_reached = 1u;
	PLAYER(current_player)->path_request = 0u;
	PLAYER(current_player)->unused_start_byte = 0u;
	PLAYER(current_player)->rings = 0u;
	PLAYER(current_player)->is_joining = 0u;
	PLAYER(current_player)->ending_done = 0u;
	PLAYER(current_player)->ending_step = 0u;
	PLAYER(current_player)->rings_to_health = 0;
	OBJECT(current_object)->kind = OBJECT(current_object)->player;
	records_left = 2u;
	clip_cursor = current_player + 0xF0u;
	do
	{
		clip_cursor += 2u;
		CLIP_ENTRY(clip_cursor - 0x2u)->command = 0x5000u;
		clip_cursor += 2u;
		CLIP_ENTRY(clip_cursor - 0x4u)->bottom = 0xE0u;
		clip_cursor += 2u;
		CLIP_ENTRY(clip_cursor - 0x6u)->left = 0u;
		clip_cursor += 2u;
		CLIP_ENTRY(clip_cursor - 0x8u)->right = 0x140u;
		clip_cursor += 2u;
		CLIP_ENTRY(clip_cursor - 0xAu)->unk_0A = 0u;
		clip_cursor += 2u;
		CLIP_ENTRY(clip_cursor - 0xCu)->unk_0C = 0u;
		clip_cursor += 2u;
		CLIP_ENTRY(clip_cursor - 0xEu)->unk_0E = 0u;
		clip_cursor += 2u;
		CLIP_ENTRY(clip_cursor - 0x10u)->unk_10 = 0u;
		clip_cursor += 2u;
		records_left--;
	} while (records_left != 0u);
	if ((cabinet_config & 1u) == 0u && OBJECT(current_object)->player == 2u)
		PLAYER(current_player)->state = 0x37u;
	PLAYER(current_player)->in_game = 0;
	joined = try_join();
	if (joined != 0u)
		PLAYER(current_player)->in_game = 1;
}

void player_resume(void)
{
	OBJECT(current_object)->handler = player_update_ADDRESS;
}

void player_join(void)
{
	uint32_t player = 0;

	current_player = PLAYER_BLOCK(1) + ((uint32_t)OBJECT(current_object)->player << 7) * 4u;
	if (current_game_mode != 0x1Bu || game_mode != 0x1Bu)
	{
		player = OBJECT(current_object)->player;
		player_status[player] = 4u;
		player_select_character(player);
		if (PLAYER(current_player)->substate != 0u)
			return;
	}
	OBJECT(current_object)->handler = player_reset_joined_ADDRESS;
	player_reset_joined();
}

void player_start_stage(void)
{
	current_player = PLAYER_BLOCK(1) + ((uint32_t)OBJECT(current_object)->player << 7) * 4u;
	OBJECT(current_object)->flags |= 0x2000000u;
	PLAYER(current_player)->flags &= ~0x10u;
	PLAYER(current_player)->stage_rings = 0u;
	player_hud_init();
	player_restart();
}

void player_respawn(void)
{
	OBJECT(current_object)->flags &= ~0x2000000u;
	player_restart();
}

void player_restart(void)
{
	current_player = PLAYER_BLOCK(1) + ((uint32_t)OBJECT(current_object)->player << 7) * 4u;
	PLAYER(current_player)->flags &= ~0x10u;
	if (PLAYER(current_player)->in_game > 0)
	{
		player_reset();
		return;
	}
	PLAYER(current_player)->previous_z_low = 0u;
	PLAYER(current_player)->in_game = 0;
	PLAYER(current_player)->state = 0xBu;
	if ((cabinet_config & 1u) == 0u && OBJECT(current_object)->player == 2u)
		PLAYER(current_player)->state = 0x37u;
	OBJECT(current_object)->handler = player_update_ADDRESS;
}

void player_reset_joined(void)
{
	uint32_t player = 0;

	OBJECT(current_object)->flags &= ~0x2000000u;
	current_player = PLAYER_BLOCK(1) + ((uint32_t)OBJECT(current_object)->player << 7) * 4u;
	PLAYER(current_player)->flags |= 0x10u;
	OBJECT(current_object)->health = starting_health;
	PLAYER(current_player)->rings_to_health = 0;
	if (stage_exit_started != 0u)
	{
		player_reset();
		return;
	}
	player = OBJECT(current_object)->player;
	unused_player_flags[player] = 0u;
	players_finished[player] = 0u;
	player_reset();
}

void player_reset(void)
{
	uint8_t sound_id = 0;
	uint32_t ignored = 0, stage_index = 0;

	OBJECT(current_object)->sprite_palette &= 0x5FFFu;
	PLAYER(current_player)->invulnerable_timer = 0;
	OBJECT(current_object)->flags &= 0xDFFFFFFFu;
	if ((OBJECT(current_object)->flags & 0x2000000u) == 0u)
	{
		PLAYER(current_player)->invulnerable_timer = 0x78;
		OBJECT(current_object)->flags |= 0x20000000u;
	}
	OBJECT(current_object)->sprite_flags = 0x104u;
	OBJECT(current_object)->sprite_frame = 0xCF0u;
	OBJECT(current_object)->sprite_scale = 0x100u;
	OBJECT(current_object)->flags &= 0x22000000u;
	OBJECT(current_object)->speed = 0;
	OBJECT(current_object)->speed_x = 0;
	OBJECT(current_object)->speed_y = 0;
	OBJECT(current_object)->speed_z = 0;
	OBJECT(current_object)->sprite_layer = 0u;
	OBJECT(current_object)->z = 0;
	PLAYER(current_player)->previous_z = 0x8000000;
	PLAYER(current_player)->trackball_x = 0;
	PLAYER(current_player)->trackball_y = 0;
	PLAYER(current_player)->substate = 0u;
	PLAYER(current_player)->control_lock = 0u;
	PLAYER(current_player)->hit_stop = 0u;
	PLAYER(current_player)->ignore_screen_edges = 0u;
	PLAYER(current_player)->walk_cycle = 0;
	PLAYER(current_player)->walk_pending_x = 0;
	PLAYER(current_player)->walk_pending_y = 0;
	OBJECT(current_object)->previous_floor_block = 0;
	OBJECT(current_object)->screen_x = 0;
	OBJECT(current_object)->screen_y = 0;
	PLAYER(current_player)->path = 0u;
	PLAYER(current_player)->mishap_flag = 0u;
	PLAYER(current_player)->scene_step_seen = 0u;
	PLAYER(current_player)->speed_scale = 0u;
	PLAYER(current_player)->goal_step = 0u;
	PLAYER(current_player)->entry_step = 0u;
	PLAYER(current_player)->effect_spawned = 0u;
	PLAYER(current_player)->flags &= 0x10u;
	PLAYER(current_player)->unused_reset_word = 0u;
	PLAYER(current_player)->ridden_object = 0u;
	PLAYER(current_player)->splash_zone = 0u;
	PLAYER(current_player)->was_in_splash_zone = 0u;
	PLAYER(current_player)->stage_b_path_done = 0u;
	PLAYER(current_player)->hurt_contact_delay = 0;
	PLAYER(current_player)->stall_frames = 0;
	PLAYER(current_player)->state = 0u;
	PLAYER(current_player)->enter_on_respawn = 0u;
	if (game_in_progress != 0u && (OBJECT(current_object)->flags & 0x2000000u) != 0u)
		PLAYER(current_player)->enter_on_respawn = 0xFFu;
	PLAYER(current_player)->facing = 8u;
	PLAYER(current_player)->target_heading = 0x200u;
	PLAYER(current_player)->heading = 0x200u;
	PLAYER(current_player)->facing_angle = 0x200u;
	OBJECT(current_object)->hitbox_size_x = 8u;
	OBJECT(current_object)->hitbox_size_y = 0u;
	OBJECT(current_object)->hitbox_height = 0x18u;
	PLAYER(current_player)->grey_screen_request = 0u;
	PLAYER(current_player)->unused_reset_byte = 0x78u;
	PLAYER(current_player)->sink_depth = 0x100;
	PLAYER(current_player)->free_turn_timer = 2u;
	OBJECT(current_object)->sprite_offset_x = 0;
	OBJECT(current_object)->sprite_offset_y = 0xA;
	OBJECT(current_object)->animation_frame = 0u;
	OBJECT(current_object)->animation_timer = 0;
	OBJECT(current_object)->animation = sonic_walking_animations_ADDRESS;
	stage_index = stage;
	OBJECT(current_object)->player_view.palette_list = ROM32((uint16_t)(stage_index * 0xCu) + player_palette_lists_ADDRESS + OBJECT(current_object)->kind * 4u);
	OBJECT(current_object)->sprite_palette = ROM16(OBJECT(current_object)->player_view.palette_list + 2u);
	if ((OBJECT(current_object)->flags & 0x2000000u) != 0u || current_game_mode == 0x1Bu)
	{
		player_place();
		return;
	}
	if ((PLAYER(current_player)->flags & 0x10u) != 0u)
	{
		sound_id = 0x50u;
		if (OBJECT(current_object)->kind != 0u)
		{
			sound_id = 0x6Fu;
			if (OBJECT(current_object)->kind != 1u)
				sound_id = 0x62u;
		}
	}
	else
	{
		sound_id = 0x5Du;
		if (OBJECT(current_object)->kind != 0u)
		{
			sound_id = 0x68u;
			if (OBJECT(current_object)->kind != 1u)
				sound_id = 0x52u;
		}
	}
	play_sound(sound_id, &ignored, &ignored);
	player_place();
}

void player_place(void)
{
	uint8_t entry_status = 0;
	uint32_t r0 = 0, ignored = 0;

	current_player = PLAYER_BLOCK(1) + ((uint32_t)OBJECT(current_object)->player << 7) * 4u;
	r0 = place_player_at_restart_point();
	find_floor(&r0, &ignored, &ignored, &ignored, &ignored);
	OBJECT(current_object)->z = OBJECT(current_object)->floor_z;
	PLAYER(current_player)->substate = 0u;
	PLAYER(current_player)->state = 0u;
	start_climb_at_spawn(r0);
	if (OBJECT(current_object)->floor_z_int >= 0x7FF)
	{
		entry_status = 0x81u;
		OBJECT(current_object)->handler = player_place_ADDRESS;
	}
	else if ((uint16_t)PLAYER(current_player)->previous_z_int != 0x800u)
	{
		if ((OBJECT(current_object)->flags & 4u) != 0u || (OBJECT(current_object)->flags & 0x8000000u) != 0u)
		{
			entry_status = 0x81u;
			OBJECT(current_object)->handler = player_place_ADDRESS;
		}
		else
		{
			OBJECT(current_object)->handler = player_update_ADDRESS;
			entry_status = 0xFFu;
		}
	}
	else
	{
		entry_status = 0xFFu;
		OBJECT(current_object)->handler = player_place_ADDRESS;
	}
	PLAYER(current_player)->entry_status = entry_status;
	set_player_entry_by_scene();
	player_status[OBJECT(current_object)->player] = PLAYER(current_player)->entry_status;
	player_status_spare = 0u;
	OBJECT(current_object)->flags &= 0xEFFFFFFFu;
	if (PLAYER(current_player)->entry_status != 0x82u)
	{
		PLAYER(current_player)->previous_z = OBJECT(current_object)->floor_z;
		OBJECT(current_object)->previous_floor_z = OBJECT(current_object)->floor_z;
		project_object(&ignored, &ignored);
		PLAYER(current_player)->screen_x_int = OBJECT(current_object)->projected_x;
		PLAYER(current_player)->screen_x_fraction = 0u;
		remember_platform();
		add_player_to_lists();
		player_hud_start();
		if (stage_exit_started == 0u && event_0d_active == 0u && (PLAYER(current_player)->flags & 0x10u) != 0u)
			PLAYER(current_player)->mishap_flag = 0xFFu;
	}
	else
	{
		OBJECT(current_object)->flags |= 0x10000000u;
		check_player_stalled();
		if (PLAYER(current_player)->state != 0xDu)
		{
			PLAYER(current_player)->previous_z = OBJECT(current_object)->floor_z;
			OBJECT(current_object)->previous_floor_z = OBJECT(current_object)->floor_z;
			project_object(&ignored, &ignored);
			PLAYER(current_player)->screen_x_int = OBJECT(current_object)->projected_x;
			PLAYER(current_player)->screen_x_fraction = 0u;
			remember_platform();
			add_player_to_lists();
			player_hud_start();
			if (stage_exit_started == 0u && event_0d_active == 0u && (PLAYER(current_player)->flags & 0x10u) != 0u)
				PLAYER(current_player)->mishap_flag = 0xFFu;
			return;
		}
		OBJECT(current_object)->handler = player_update_ADDRESS;
		PLAYER(current_player)->entry_status = 0xFFu;
		player_status[OBJECT(current_object)->player] = PLAYER(current_player)->entry_status;
	}
}

void player_update(void)
{
	uint32_t r1 = 0, r11 = 0;
	uint16_t invulnerable_left = 0;

	current_player = PLAYER_BLOCK(1) + ((uint32_t)OBJECT(current_object)->player << 7) * 4u;
	OBJECT(current_object)->flags &= ~0x2000000u;
	skip_player_summary = 0u;
	update_hurt_output();
	player_hud_update(&r1, &r11);
	invulnerable_left = PLAYER(current_player)->invulnerable_timer - 1u;
	PLAYER(current_player)->invulnerable_timer = invulnerable_left;
	if ((int16_t)invulnerable_left < 0)
		PLAYER(current_player)->invulnerable_timer = -1;
	check_stage_entry_path();
	check_player_reached_goal();
	r11 = check_scene_states(r11);
	player_dispatch_state(r1, r11);
}

void player_dispatch_state(uint32_t depth, uint32_t cursor)
{
	uint32_t jump_offset = 0;

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
		player_move(depth, cursor);
		return;
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
	case 55:
		player_state_not_playing();
		return;
	case 13:
		player_state_hurt(depth, cursor);
		return;
	case 14:
		player_state_bouncing();
		return;
	case 15:
	case 16:
	case 17:
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

uint32_t keep_player_on_screen(uint32_t depth)
{
	uint8_t sound_id = 0;
	uint16_t base_x = 0, base_y = 0, edge = 0, scroll_direction = 0;
	uint32_t direction_row = 0, floor_slope = 0, ignored = 0, offset_x = 0, slope_direction = 0, slope_factor = 0, slope_speed = 0, voice_range = 0;
	uint8_t health = 0;

	if (nearby_floor_blocks_locked != 0u || (OBJECT(current_object)->flags & 0x4000000u) != 0u)
		return depth;
	offset_x = (depth & ~0xFFFFu) | (uint16_t)OBJECT(current_object)->sprite_offset_x;
	base_x = (uint16_t)(OBJECT(current_object)->screen_x - offset_x);
	depth = (offset_x & ~0xFFFFu) | (uint16_t)OBJECT(current_object)->sprite_offset_y;
	base_y = (uint16_t)(OBJECT(current_object)->screen_y - depth);
	if ((stage != 8u || scroll_position >= 0x4000u) && ((uint16_t)(base_x + 0x10u) >= 0x160u || (uint16_t)(base_y + 0xE0u) >= 0x1E0u))
	{
		if (stage != 5u)
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
			return depth;
		}
		OBJECT(current_object)->handler = player_respawn_ADDRESS;
		return depth;
	}
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
			play_sound(sound_id, &ignored, &ignored);
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
	return depth;
}

uint32_t push_player_from_screen_edge(void)
{
	uint16_t relative_direction = 0;
	uint32_t push_cos = 0, push_record = 0, push_x = 0, push_y = 0;

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
	polar_to_xy(&push_cos, &push_x, &push_y);
	OBJECT(current_object)->x += push_x;
	OBJECT(current_object)->y += push_y;
	PLAYER(current_player)->previous_x = OBJECT(current_object)->x;
	PLAYER(current_player)->previous_y = OBJECT(current_object)->y;
	return push_cos;
}

void compute_scroll_drift(uint32_t block, uint8_t drift_byte)
{
	uint16_t dy = 0;
	uint32_t divisor = 0, drift_angle = 0, ignored = 0, scroll_angle = 0, slope = 0, vx = 0, vy = 0;

	if (scroll_drift_block == block && scroll_drift_vx == (uint32_t)camera_velocity_x && scroll_drift_vy == (uint32_t)camera_velocity_y)
		return;
	unused_drift_byte = drift_byte;
	scroll_drift_block = block;
	if (FLOOR_BLOCK(block)->slope == 0u)
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
		scroll_angle = (int32_t)camera_velocity_x >> 5;
		dy = (uint16_t)((int32_t)camera_velocity_y >> 5);
		angle_of_vector(dy, &scroll_angle, &ignored);
		camera_scroll_direction = scroll_angle;
		scroll_drift_vx = camera_velocity_x;
		scroll_drift_vy = camera_velocity_y;
		return;
	}
	slope = FLOOR_BLOCK(block)->slope;
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
	uint32_t ignored = 0;

	animate(&ignored, &ignored, &ignored);
	if (OBJECT(current_object)->animation_frame == 0u)
	{
		delete_current_object();
		return;
	}
	update_screen_position(&ignored, &ignored);
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
	uint32_t cos_speed = 0;

	cos_speed = set_speed_from_trackball();
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
		invalid_jump(0xB36Fu + (uint16_t)PLAYER(current_player)->ledge_side * 2u);
	}
	return cos_speed;
}

void get_ledge_side(uint32_t *p_side, uint32_t *p_block)
{
	uint32_t block = 0, bound_cursor = 0, side = 0;
	uint32_t x_min = 0, x_max = 0, y_min = 0;

	bound_cursor = floor_blocks + OBJECT(current_object)->previous_floor_block + 2u;
	block = bound_cursor;
	side = 3u;
	x_min = FLOOR_BLOCK(bound_cursor)->x_min;
	bound_cursor += 4u;
	if (OBJECT(current_object)->x < (int32_t)x_min)
	{
		*p_side = side;
		*p_block = block;
		return;
	}
	side = 1u;
	x_max = FLOOR_BLOCK(bound_cursor - 0x4u)->x_max;
	bound_cursor += 4u;
	if ((int32_t)x_max < OBJECT(current_object)->x)
	{
		*p_side = side;
		*p_block = block;
		return;
	}
	side = 0u;
	y_min = FLOOR_BLOCK(bound_cursor - 0x8u)->y_min;
	bound_cursor += 4u;
	if (OBJECT(current_object)->y < (int32_t)y_min)
	{
		*p_side = side;
		*p_block = block;
		return;
	}
	side = 2u;
	if (FLOOR_BLOCK(bound_cursor - 0xCu)->y_max < OBJECT(current_object)->y)
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
	uint32_t block_2 = 0, draw_depth = 0, edge_speed_cos = 0, edge_speed_x = 0, edge_speed_y = 0, side = 0;

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
				draw_depth = (draw_depth & ~0xFFFFu) | OBJECT(LINKED(current_object, OBJECT(current_object)->platform))->draw_depth;
				draw_depth = (draw_depth & ~0xFFFFu) | (uint16_t)(draw_depth - 1u);
				PLAYER(current_player)->fall_draw_depth = draw_depth;
			}
		}
		else
		{
			if (PLAYER(current_player)->ledge_side >= 2)
			{
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
				OBJECT(current_object)->sprite_palette = ROM16(OBJECT(current_object)->check_player.palette_list + 2u);
				OBJECT(current_object)->flags &= ~0x40u;
				OBJECT(current_object)->flags &= ~0x20000u;
				PLAYER(current_player)->flags |= 0x80u;
				OBJECT(current_object)->flags &= ~0x2000u;
				*p_depth = draw_depth;
				*p_block = block;
				return;
			}
			if (stage == 0xAu)
				PLAYER(current_player)->fall_draw_depth -= 0x20u;
		}
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
		OBJECT(current_object)->sprite_palette = ROM16(OBJECT(current_object)->check_player.palette_list + 2u);
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
		polar_to_xy(&edge_speed_cos, &edge_speed_x, &edge_speed_y);
		OBJECT(current_object)->speed_x = edge_speed_x;
		OBJECT(current_object)->speed_y = edge_speed_y;
		*p_depth = edge_speed_cos;
		return;
	}
	PLAYER(current_player)->substate = 0u;
	next_state = 5u;
	if (OBJECT(current_object)->floor_type != 1u && (OBJECT(current_object)->flags & 0x10u) == 0u)
	{
		block_2 = floor_blocks + 2u + OBJECT(current_object)->previous_floor_block;
		if (FLOOR_BLOCK(block_2)->type_index != 0x12u && FLOOR_BLOCK(block_2)->type_index != 0x23u && FLOOR_BLOCK(block_2)->type_index != 0x18u && FLOOR_BLOCK(block_2)->type_index != 0x19u && OBJECT(current_object)->speed_int > 0x200)
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
	OBJECT(current_object)->flags_low &= ~0x10u;
	if ((OBJECT(current_object)->flags & 8u) != 0u)
		OBJECT(current_object)->flags_low |= 0x10u;
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
	if ((int8_t)jump_delay < 1)
	{
		PLAYER(current_player)->jump_delay = 0;
		if (PLAYER(current_player)->state != 2u && PLAYER(current_player)->control_lock == 0u)
		{
			start_jump_if_pressed();
			return;
		}
	}
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
	uint16_t drift_bounce_direction = 0;
	uint32_t drift_angle = 0, rebound_direction = 0, wall_push_direction = 0, wall_record_index = 0;
	uint8_t wall_timer = 0;

	if (PLAYER(current_player)->path_request != 0u || PLAYER(current_player)->path != 0u || all_players_path_request != 0u)
		return;
	if ((OBJECT(current_object)->flags & 0x20u) == 0u)
	{
		PLAYER(current_player)->last_wall = 0xFFFFu;
		return;
	}
	if (OBJECT(current_object)->wall != PLAYER(current_player)->last_wall)
	{
		PLAYER(current_player)->wall_timer = 0x1E;
		PLAYER(current_player)->last_wall = OBJECT(current_object)->wall;
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
		{
			if ((stage != 8u || scroll_position >= 0x4000u) && PLAYER(current_player)->state != 0x25u && PLAYER(current_player)->state != 0x26u)
				OBJECT(current_object)->flags |= 0x1000u;
			OBJECT(current_object)->x = PLAYER(current_player)->previous_x;
			OBJECT(current_object)->y = PLAYER(current_player)->previous_y;
			OBJECT(current_object)->floor_block = OBJECT(current_object)->previous_floor_block;
			OBJECT(current_object)->floor_z = OBJECT(current_object)->previous_floor_z;
			PLAYER(current_player)->control_lock = 0u;
		}
		else
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
	if (PLAYER(current_player)->state != 0x25u && PLAYER(current_player)->state != 0x26u)
	{
		if (OBJECT(current_object)->floor_type == 4u || OBJECT(current_object)->speed_int >= 0x200)
		{
			push_player_out_of_wall_deflect(&angle, &wall_push_direction, &wall_record_index);
			*p_angle = angle;
			*p_push_direction = (*p_push_direction & 0xFFFF0000u) | (wall_push_direction & 0xFFFFu);
			*p_record_index = wall_record_index;
			return;
		}
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
	drift_angle = (angle & ~0xFFFFu) | scroll_drift_direction;
	drift_angle = (drift_angle & ~0xFFFFu) | (uint16_t)(drift_angle + 0x200u);
	drift_angle = (drift_angle & ~0xFFFFu) | (drift_angle & 0x3FFu);
	drift_bounce_direction = sonic_bounce_records[OBJECT(LINKED(current_object, OBJECT(current_object)->wall))->direction_low * 3u];
	drift_angle = (drift_angle & ~0xFFFFu) | (uint16_t)(drift_angle - drift_bounce_direction);
	drift_angle = (drift_angle & ~0xFFFFu) | (drift_angle & 0x3FFu);
	if ((int16_t)drift_angle < 0x200)
	{
		drift_bounce_direction -= 0x80u;
		scroll_drift_direction = drift_bounce_direction & 0x3FFu;
		push_player_out_of_wall_deflect(&drift_angle, &wall_push_direction, &wall_record_index);
		*p_angle = drift_angle;
		*p_push_direction = (*p_push_direction & 0xFFFF0000u) | (wall_push_direction & 0xFFFFu);
		*p_record_index = wall_record_index;
		return;
	}
	drift_bounce_direction += 0x80u;
	scroll_drift_direction = drift_bounce_direction & 0x3FFu;
	push_player_out_of_wall_deflect(&drift_angle, &wall_push_direction, &wall_record_index);
	*p_angle = drift_angle;
	*p_push_direction = (*p_push_direction & 0xFFFF0000u) | (wall_push_direction & 0xFFFFu);
	*p_record_index = wall_record_index;
}

void push_player_out_of_wall_deflect(uint32_t *p_angle, uint32_t *p_push_direction, uint32_t *p_record_index)
{
	uint16_t angle = 0, new_direction = 0, push_direction = 0;
	uint32_t record_index = 0;

	record_index = OBJECT(LINKED(current_object, OBJECT(current_object)->wall))->direction_low * 3u;
	push_direction = sonic_bounce_records[record_index];
	new_direction = (uint16_t)push_direction;
	angle = (((OBJECT(current_object)->direction + 0x200u) & 0x3FFu) - push_direction) & 0x3FFu;
	if ((int16_t)angle < 0x200)
		new_direction -= 0x80u;
	else
		new_direction += 0x80u;
	OBJECT(current_object)->direction = new_direction & 0x3FFu;
	PLAYER(current_player)->control_lock = 4u;
	if (stage != 8u || scroll_position >= 0x4000u)
	{
		*p_angle = (*p_angle & 0xFFFF0000u) | angle;
		*p_push_direction = (*p_push_direction & 0xFFFF0000u) | push_direction;
		*p_record_index = record_index;
		return;
	}
	angle = (((uint32_t)OBJECT(LINKED(current_object, OBJECT(current_object)->wall))->direction << 8) + 0x200u) & 0x3FFu;
	OBJECT(current_object)->direction = angle;
	*p_angle = (*p_angle & 0xFFFF0000u) | angle;
	*p_push_direction = (*p_push_direction & 0xFFFF0000u) | push_direction;
	*p_record_index = record_index;
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
	wanted_speed = follow_player_path(wanted_speed);
	if (PLAYER(current_player)->path != 0u)
	{
		player_steer_approach_speed(wanted_speed);
		return;
	}
	read_player_trackball();
	if (players_frozen != 0u)
	{
		PLAYER(current_player)->trackball_x = 0;
		PLAYER(current_player)->trackball_y = 0;
	}
	if (PLAYER(current_player)->state == 4u || PLAYER(current_player)->state == 0x17u || PLAYER(current_player)->state == 0x25u || PLAYER(current_player)->state == 0x26u || PLAYER(current_player)->state == 0x2Du || (PLAYER(current_player)->trackball_x | PLAYER(current_player)->trackball_y) != 0u || PLAYER(current_player)->push_xy != 0u)
	{
		if ((OBJECT(current_object)->flags & 0x2000u) != 0u)
		{
			player_steer_speed_from_trackball();
			return;
		}
		set_target_heading_from_trackball();
		player_steer_speed_from_trackball();
		return;
	}
	if (OBJECT(current_object)->speed_int >= 0x20)
	{
		player_steer_speed_from_trackball();
		return;
	}
	OBJECT(current_object)->speed = 0;
	PLAYER(current_player)->previous_state = PLAYER(current_player)->state;
	PLAYER(current_player)->state = 0u;
	player_steer_turn_and_ground_speed();
}

void player_steer_speed_from_trackball(void)
{
	uint32_t ignored = 0, input_y = 0, wanted_speed = 0;

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
	{
		player_steer_approach_speed(wanted_speed);
		return;
	}
	wanted_speed = (wanted_speed >> 8) * 0x60u;
	player_steer_approach_speed(wanted_speed);
}

void player_steer_approach_speed(uint32_t wanted_speed)
{
	uint8_t next_state = 0;
	uint16_t animation_speed_int = 0;
	uint32_t animation_speed_diff = 0, ignored = 0, input_animation_speed = 0, run_threshold = 0, speed_diff = 0, trackball_y = 0;
	uint32_t speed = 0, target_speed = 0, speed_2 = 0, target_speed_2 = 0, animation_speed = 0, target_animation_speed = 0;

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
	if (OBJECT(current_object)->floor_type == 7u || PLAYER(current_player)->push_xy != 0u || PLAYER(current_player)->speed_scale != 0u)
	{
		if ((PLAYER(current_player)->trackball_x | PLAYER(current_player)->trackball_y) != 0u || (OBJECT(current_object)->floor_type != 7u && PLAYER(current_player)->animation_speed_int >= 0x20))
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
	if (PLAYER(current_player)->state == 4u || PLAYER(current_player)->state == 0x17u || PLAYER(current_player)->state == 0x25u || PLAYER(current_player)->state == 0x26u || PLAYER(current_player)->state == 0x2Du)
	{
		player_steer_turn_and_ground_speed();
		return;
	}
	next_state = 0u;
	animation_speed_int = (uint16_t)PLAYER(current_player)->animation_speed_int;
	if (animation_speed_int == 0u)
	{
		PLAYER(current_player)->previous_state = PLAYER(current_player)->state;
		PLAYER(current_player)->state = next_state;
		player_steer_turn_and_ground_speed();
		return;
	}
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
	PLAYER(current_player)->previous_state = PLAYER(current_player)->state;
	PLAYER(current_player)->state = next_state;
	player_steer_turn_and_ground_speed();
}

void player_steer_turn_and_ground_speed(void)
{
	uint32_t carry = 0;
	uint32_t direction = 0, floor_slope = 0, rounded_heading = 0, rounded_speed = 0, slope_pair = 0, speed_factor = 0;

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
	if (PLAYER(current_player)->push_xy == 0u)
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
	{
		PLAYER(current_player)->heading = PLAYER(current_player)->target_heading;
		PLAYER(current_player)->heading &= 0x3FFu;
		set_heading_direction();
		return;
	}
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
		PLAYER(current_player)->heading &= 0x3FFu;
		set_heading_direction();
		return;
	}
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
	axis_value = (uint16_t)TRACKBALL_MOTION(trackball)->x;
	trackball += 2u;
	if ((int16_t)axis_value >= 0)
		axis_value = (uint16_t)((axis_value & ~1u) << 8);
	else
		axis_value = (uint16_t)(0u - (((0u - axis_value) & 0xFFFEu) << 8));
	PLAYER(current_player)->trackball_x = axis_value;
	axis_value = (uint16_t)TRACKBALL_MOTION(trackball - 0x2u)->y;
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
	uint32_t depth = 0, ignored = 0, offset_y = 0, player = 0, point_x = 0, point_y = 0, scaled_offset_y = 0, scaled_player = 0, scene_player = 0, sprite_scale = 0, z = 0;
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
		if (stage == 9u && (OBJECT(current_player)->floor_type == 8u || (OBJECT(current_player)->floor_type == 7u && (OBJECT(current_player)->flags & 0x10u) == 0u)))
		{
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
				update_screen_position(&ignored, &ignored);
				depth = (uint16_t)(OBJECT(current_object)->screen_y - 0x18u);
				queue_sprite_at_depth(&ignored, &depth);
			}
			draw_attack_effect(player);
			return;
		}
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
				animate(&ignored, &ignored, &ignored);
				update_screen_position(&ignored, &ignored);
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
				if ((current_game_mode_word != 0x1Au || (int8_t)PLAYER(player)->sequence_step < 2) && (OBJECT(current_player)->flags & 0x10u) == 0u)
					OBJECT(current_object)->sprite_palette = 0x1FFEu;
				else
				{
					OBJECT(current_object)->sprite_palette = 0x87A0u;
					flicker_timer = OBJECT(current_object)->shadow.flicker_timer;
					OBJECT(current_object)->shadow.flicker_timer = flicker_timer - 1u;
					if ((int8_t)flicker_timer <= 1)
					{
						OBJECT(current_object)->shadow.flicker_timer = 2;
						OBJECT(current_object)->flags |= 0x80000u;
					}
				}
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
		project_point(z, &ignored, &point_x, &point_y);
		PLAYER(player)->ground_clip_bottom = point_y;
		OBJECT(current_object)->sprite_flags = 0u;
		OBJECT(current_object)->flags &= ~0x80000u;
		OBJECT(current_object)->sprite_palette = 0x1020u;
		OBJECT(current_object)->sprite_scale = 0x120u;
		priority_bits = OBJECT(current_player)->sprite_palette & 0x6000u;
		OBJECT(current_object)->sprite_palette &= 0x9FFFu;
		OBJECT(current_object)->sprite_palette |= priority_bits;
		OBJECT(current_object)->animation = sinking_shadow_animation_ADDRESS;
		animate(&ignored, &ignored, &ignored);
		update_screen_position(&ignored, &ignored);
		depth = (uint16_t)(OBJECT(current_object)->screen_y + 0x10u);
		queue_sprite_at_depth(&ignored, &depth);
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
		invalid_jump(0x6E3Eu + scene_step * 3u);
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

void check_player_step(uint32_t *p_height_diff, uint32_t *p_cursor)
{
	uint32_t height_diff = *p_height_diff;
	uint32_t cursor = *p_cursor;
	uint32_t object = 0;

	step_floor_z = OBJECT(current_object)->floor_z;
	if (stage == 8u && scroll_position < 0x4000u)
	{
		PLAYER(current_player)->previous_z = OBJECT(current_object)->z;
		OBJECT(current_object)->z = step_floor_z;
		return;
	}
	if ((OBJECT(current_object)->flags & 8u) == 0u)
	{
		check_player_step_against_floor(&height_diff, &cursor);
		*p_height_diff = height_diff;
		*p_cursor = cursor;
		return;
	}
	object = LINKED(current_object, OBJECT(current_object)->platform);
	if (OBJECT(object)->floor_slope != 0u)
	{
		check_player_step_against_floor(&height_diff, &cursor);
		*p_height_diff = height_diff;
		*p_cursor = cursor;
		return;
	}
	step_floor_z = OBJECT(object)->z;
	if (mem_read8(floor_blocks + 2u + OBJECT(current_object)->previous_floor_block + 0x12u) != 0x24u)
	{
		check_player_step_against_floor(&height_diff, &cursor);
		*p_height_diff = height_diff;
		*p_cursor = cursor;
		return;
	}
	PLAYER(current_player)->previous_z = OBJECT(current_object)->z;
	OBJECT(current_object)->z = step_floor_z;
}

void check_player_step_against_floor(uint32_t *p_height_diff, uint32_t *p_cursor)
{
	uint32_t height_diff = *p_height_diff;
	uint32_t cursor = *p_cursor;
	uint32_t floor_block = 0, object_z = 0, opposite_side = 0, x = 0, y = 0;
	uint16_t floor_z_int = 0, z_int = 0, height_diff_2 = 0;

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
	if ((uint16_t)height_diff < 0x11u)
	{
		PLAYER(current_player)->previous_z = OBJECT(current_object)->z;
		OBJECT(current_object)->z = step_floor_z;
		*p_height_diff = height_diff;
		return;
	}
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
	x_min = FLOOR_BLOCK(floor_block)->x_min;
	floor_block += 4u;
	if ((int32_t)x < (int32_t)x_min)
	{
		side = 3u;
		return side;
	}
	x_max = FLOOR_BLOCK(floor_block - 0x4u)->x_max;
	floor_block += 4u;
	if ((int32_t)x <= (int32_t)x_max)
	{
		if (FLOOR_BLOCK(floor_block - 0x8u)->y_min <= (int32_t)y)
		{
			side = 2u;
			return side;
		}
		side = 0u;
		return side;
	}
	side = 1u;
	return side;
}

void get_floor_block_side_or_corner(uint32_t block, uint32_t x, uint32_t y, uint32_t *p_outcode, uint32_t *p_side_code)
{
	uint32_t outcode = 0, side_code = 0;

	outcode = 0u;
	block = block + floor_blocks + 2u;
	if (FLOOR_BLOCK(block)->x_min <= (int32_t)x)
	{
		if (FLOOR_BLOCK(block)->x_max < (int32_t)x)
			outcode = outcode | 1u;
	}
	else
		outcode = outcode | 2u;
	if (FLOOR_BLOCK(block)->y_min <= (int32_t)y)
	{
		if ((int32_t)y <= FLOOR_BLOCK(block)->y_max)
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
		invalid_jump(0xCC13u + kind_offset);
	}
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
	palette = ROM16(OBJECT(current_object)->flash_hurt_palette.palette_list + 2u);
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
			if (PLAYER(current_player)->sink_depth >= 0x2000)
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
		}
	}
	return sink_offset;
}

uint32_t update_sinking_floor(uint32_t sink_offset)
{
	uint8_t health = 0;

	if (OBJECT(current_object)->floor_type != 0xAu)
		return sink_offset;
	if (OBJECT(current_object)->floor_block >= 0)
	{
		PLAYER(current_player)->sink_depth += 0x33u;
		PLAYER(current_player)->sink_depth -= PLAYER(current_player)->struggle_speed >> 4;
		if (PLAYER(current_player)->sink_depth < 0x100)
			PLAYER(current_player)->sink_depth = 0x100;
		if (sink_depth_limits[OBJECT(current_object)->floor_attribute] < PLAYER(current_player)->sink_depth)
			PLAYER(current_player)->sink_depth -= 0xA0u;
		sink_offset = (uint32_t)(uint16_t)PLAYER(current_player)->sink_depth << 8;
		OBJECT(current_object)->z = OBJECT(current_object)->floor_z + sink_offset;
	}
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
	return sink_offset;
}

void apply_moving_floor(void)
{
	if (OBJECT(current_object)->floor_type == 7u && ROM8(floor_attribute_slopes_ADDRESS + OBJECT(current_object)->floor_attribute) == 1u)
		OBJECT(current_object)->x += OBJECT(LINKED(current_object, moving_floor_object))->speed_x;
	if (PLAYER(current_player)->state != 4u && (PLAYER(current_player)->state != 5u || ROM8(floor_types_ADDRESS + mem_read8(floor_blocks + 2u + OBJECT(current_object)->previous_floor_block + 0x12u) * 4u) == 0xCu) && OBJECT(current_object)->floor_type == 0xCu)
		OBJECT(current_object)->y += OBJECT(LINKED(current_object, OBJECT(current_object)->platform))->speed_y;
}

void check_zone_contact(void)
{
	uint32_t object = 0;

	if ((OBJECT(current_object)->flags & 0x8000000u) == 0u)
		return;
	object = (current_object & ~0xFFFFu) | (uint16_t)OBJECT(current_object)->anchor_x;
	if (OBJECT(object)->kind != 0x16u)
	{
		if (OBJECT(object)->kind == 0x18u || OBJECT(object)->kind == 0xCu || OBJECT(object)->kind == 0x1Eu || OBJECT(object)->kind == 9u || OBJECT(object)->kind == 0x21u || OBJECT(object)->kind == 0x14u)
		{
			OBJECT(current_object)->flags |= 4u;
			OBJECT(current_object)->touched_by = OBJECT(current_object)->anchor_x;
		}
	}
	else
	{
		PLAYER(current_player)->state = 0x22u;
		PLAYER(current_player)->substate = 0u;
	}
}

void check_stage_entry_path(void)
{
	if (PLAYER(current_player)->enter_on_respawn == 0u)
		return;
	if (stage_player_entries[stage] != 0u)
	{
		PLAYER(current_player)->previous_state = PLAYER(current_player)->state;
		PLAYER(current_player)->state = 0x2Au;
		return;
	}
	PLAYER(current_player)->enter_on_respawn = 0u;
}

void load_stage_goal(void)
{
	uint32_t goal_record = 0;

	goal_record = stage_goals[stage] + stage_goals_ADDRESS;
	goal_scroll_position = ROM16(goal_record);
	goal_record += 2u;
	goal_direction = ROM16(goal_record);
	goal_record += 2u;
	goal_line = ROM16(goal_record);
	goal_record = ((goal_record + 2u) & ~0xFFFFu) | (uint16_t)(goal_record + 2u + 2u);
	goal_line_min = ROM16(goal_record);
	goal_line_max = ROM16(goal_record + 2u);
}

void check_player_reached_goal(void)
{
	uint32_t object = 0;

	if ((OBJECT(current_object)->flags & 0x400u) != 0u || (uint8_t)OBJECT(current_object)->health == 0u || PLAYER(current_player)->state == 9u || PLAYER(current_player)->state == 0x24u || (uint16_t)OBJECT(current_object)->floor_z_int == 0x800u || current_game_mode == 0x20u || PLAYER(current_player)->state == 0x28u)
		return;
	switch ((uint8_t)stage)
	{
	case 0:
		PLAYER(current_player)->previous_state = PLAYER(current_player)->state;
		PLAYER(current_player)->state = 0x18u;
		return;
	case 1:
	case 2:
	case 3:
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
	case 19:
		if (scroll_position < goal_scroll_position)
			return;
		switch (goal_direction)
		{
		case 0:
		case 4:
			if (((uint16_t)goal_line_min == 0u || (goal_line_min <= OBJECT(current_object)->x_int && OBJECT(current_object)->x_int <= goal_line_max)) && OBJECT(current_object)->y_int <= goal_line)
			{
				PLAYER(current_player)->previous_state = PLAYER(current_player)->state;
				PLAYER(current_player)->state = 0x21u;
			}
			break;
		case 1:
			if (((uint16_t)goal_line_min == 0u || (goal_line_min <= OBJECT(current_object)->y_int && OBJECT(current_object)->y_int <= goal_line_max)) && goal_line <= OBJECT(current_object)->x_int)
			{
				PLAYER(current_player)->previous_state = PLAYER(current_player)->state;
				PLAYER(current_player)->state = 0x21u;
				return;
			}
			break;
		case 2:
			if (((uint16_t)goal_line_min == 0u || (goal_line_min <= OBJECT(current_object)->x_int && OBJECT(current_object)->x_int <= goal_line_max)) && goal_line <= OBJECT(current_object)->y_int)
			{
				PLAYER(current_player)->previous_state = PLAYER(current_player)->state;
				PLAYER(current_player)->state = 0x21u;
				return;
			}
			break;
		case 3:
			if (((uint16_t)goal_line_min == 0u || (goal_line_min <= OBJECT(current_object)->y_int && OBJECT(current_object)->y_int <= goal_line_max)) && OBJECT(current_object)->x_int <= goal_line)
			{
				PLAYER(current_player)->previous_state = PLAYER(current_player)->state;
				PLAYER(current_player)->state = 0x21u;
				return;
			}
			break;
		default:
			invalid_jump(0xD0BEu + goal_direction * 2u);
		}
		return;
	case 4:
		if (goal_scroll_position <= scroll_position && goal_unlocked != 0u)
			PLAYER(current_player)->state = 0x21u;
		return;
	case 5:
		if (scroll_position < goal_scroll_position)
			return;
		if (((uint16_t)goal_line_min == 0u || (goal_line_min <= OBJECT(current_object)->y_int && OBJECT(current_object)->y_int <= goal_line_max)) && goal_line <= OBJECT(current_object)->x_int)
		{
			PLAYER(current_player)->previous_state = PLAYER(current_player)->state;
			PLAYER(current_player)->state = 0x18u;
			PLAYER(current_player)->scene_path = 0xBu;
			return;
		}
		object = LINKED(current_object, spiked_press_object);
		if ((uint16_t)object != 0u && OBJECT(object)->spiked_press.stopped_frames >= 0x258)
		{
			PLAYER(current_player)->previous_state = PLAYER(current_player)->state;
			PLAYER(current_player)->state = 0x18u;
			PLAYER(current_player)->scene_path = 0x11u;
		}
		return;
	default:
		invalid_jump(0xD06Du + (uint8_t)stage * 3u);
	}
}

void spawn_tumble_effect(uint32_t effect)
{
	uint32_t free_count = 0;

	allocate_object(&free_count, &effect);
	if (free_count != 0u)
	{
		OBJECT(effect)->sprite_flags = 0x1004u;
		OBJECT(effect)->sprite_palette = 0x8360u;
		OBJECT(effect)->sprite_offset_x = 0;
		OBJECT(effect)->sprite_offset_y = 0;
		OBJECT(effect)->flags = 0u;
		OBJECT(effect)->handler = tumble_effect_update_ADDRESS;
		OBJECT(effect)->animation = tumble_effect_animation_ADDRESS;
		OBJECT(effect)->parent = current_object;
		if (PLAYER(current_player)->facing >= 1 && PLAYER(current_player)->facing <= 7)
			OBJECT(effect)->sprite_flags |= 0x40u;
	}
}

void tumble_effect_update(void)
{
	uint32_t animation_frame = 0, depth = 0, ignored = 0, object = 0, player = 0;

	object = LINKED(current_object, OBJECT(current_object)->parent);
	OBJECT(current_object)->screen_x = OBJECT(object)->screen_x;
	OBJECT(current_object)->screen_y = OBJECT(object)->screen_y;
	player = PLAYER_BLOCK(1);
	if (OBJECT(object)->player != 0u)
	{
		player = PLAYER_BLOCK(2);
		if (OBJECT(object)->player != 1u)
			player = PLAYER_BLOCK(3);
	}
	if (PLAYER(player)->state != 0x20u || (int8_t)OBJECT(object)->animation_frame >= 0xB)
	{
		delete_current_object();
		return;
	}
	animation_frame = OBJECT(object)->animation_frame;
	OBJECT(current_object)->sprite_frame = ROM16(OBJECT(current_object)->animation + 2u + animation_frame * 2u);
	OBJECT(current_object)->screen_y += tumble_effect_y_offsets[animation_frame];
	depth = (uint16_t)(PLAYER(player)->draw_depth - 0x10u + tumble_effect_depth_offsets[PLAYER(player)->facing]);
	queue_sprite_at_depth(&ignored, &depth);
}

uint32_t apply_speed_scale(uint32_t target_speed)
{
	if (PLAYER(current_player)->speed_scale != 0u)
		target_speed = (target_speed * PLAYER(current_player)->speed_scale) >> 4;
	return target_speed;
}

void set_player_entry_by_scene(void)
{
	if (current_game_mode == 0xAu && (int8_t)scene_step > 1)
	{
		PLAYER(current_player)->entry_status = 0x82u;
		OBJECT(current_object)->handler = player_place_ADDRESS;
	}
	if (current_game_mode == 2u && player_status[OBJECT(current_object)->player] == 0x82u)
	{
		PLAYER(current_player)->entry_status = 0x82u;
		OBJECT(current_object)->handler = player_place_ADDRESS;
	}
	if (current_game_mode == 0x10u)
	{
		if ((OBJECT(current_object)->flags & 0x2000000u) != 0u)
		{
			OBJECT(current_object)->flags &= ~0x2000000u;
			PLAYER(current_player)->state = 0x1Cu;
			PLAYER(current_player)->entry_status = 0xFFu;
			OBJECT(current_object)->handler = player_update_ADDRESS;
		}
		else
		{
			PLAYER(current_player)->entry_status = 0x82u;
			OBJECT(current_object)->handler = player_place_ADDRESS;
		}
	}
	if (current_game_mode == 1u && OBJECT(current_object)->set_player_entry_by_scene.floor_block_high != 0xFFFFu)
	{
		PLAYER(current_player)->state = 0x1Eu;
		PLAYER(current_player)->entry_status = 0xFFu;
		OBJECT(current_object)->handler = player_update_ADDRESS;
	}
	if (current_game_mode == 0x18u)
	{
		PLAYER(current_player)->state = 0x2Bu;
		PLAYER(current_player)->entry_status = 0xFFu;
		OBJECT(current_object)->handler = player_update_ADDRESS;
	}
	if (current_game_mode == 0x1Bu && game_mode == 0x1Bu)
	{
		PLAYER(current_player)->state = 0x34u;
		PLAYER(current_player)->entry_status = 0xFFu;
		OBJECT(current_object)->handler = player_update_ADDRESS;
	}
	if (stage_exit_started != 0u)
	{
		PLAYER(current_player)->entry_status = 0x82u;
		OBJECT(current_object)->handler = player_place_ADDRESS;
	}
	if (current_game_mode == 0x1Du)
	{
		PLAYER(current_player)->entry_status = 0x82u;
		OBJECT(current_object)->handler = player_place_ADDRESS;
	}
	if (current_game_mode == 0x15u && stage_exit_started != 0u)
	{
		PLAYER(current_player)->entry_status = 0x82u;
		OBJECT(current_object)->handler = player_place_ADDRESS;
	}
	if ((OBJECT(current_object)->flags & 0x2000000u) == 0u && (current_game_mode == 0x13u || (current_game_mode == 0x12u && event_12_fast_scroll != 0u)))
	{
		PLAYER(current_player)->entry_status = 0x82u;
		OBJECT(current_object)->handler = player_place_ADDRESS;
	}
	if (current_game_mode == 0x19u && rope_bridge_collapsing != 0u)
	{
		PLAYER(current_player)->entry_status = 0x82u;
		OBJECT(current_object)->handler = player_place_ADDRESS;
	}
	if (event_1c_ride_phase != 0u)
	{
		PLAYER(current_player)->entry_status = 0x82u;
		OBJECT(current_object)->handler = player_place_ADDRESS;
	}
	if (current_game_mode == 0xDu && (OBJECT(current_object)->flags & 0x2000000u) == 0u)
	{
		PLAYER(current_player)->entry_status = 0x82u;
		OBJECT(current_object)->handler = player_place_ADDRESS;
	}
	if (current_game_mode == 0x1Au)
	{
		PLAYER(current_player)->entry_status = 0x82u;
		OBJECT(current_object)->handler = player_place_ADDRESS;
	}
}

uint32_t try_continue(uint8_t cost)
{
	uint32_t joined = 0;

	joined = 0u;
	if (PLAYER(current_player)->state == 0x37u)
		return joined;
	if (game_in_progress != 0u)
		return pay_and_join(cost, joined);
	joined = 0xFFFFFFFFu;
	return joined;
}

uint32_t try_join(void)
{
	uint8_t cost = 0;
	uint32_t joined = 0;

	joined = 0u;
	if (PLAYER(current_player)->state == 0x37u)
		return joined;
	if (game_in_progress != 0u)
	{
		cost = (uint8_t)credits_per_start;
		return pay_and_join(cost, joined);
	}
	joined = 0xFFFFFFFFu;
	return joined;
}

uint32_t pay_and_join(uint8_t cost, uint32_t joined)
{
	uint8_t cabinet_event = 0;
	uint32_t bit = 0, credit_block = 0, start_bit = 0;
	uint32_t port_d = 0;

	if (free_play != 0u)
		cost = 0u;
	switch (cabinet_config)
	{
	case 0:
	case 1:
	case 4:
		if (p1_credits < (uint8_t)cost)
			return joined;
		credit_block = p1_coins_ADDRESS;
		switch (OBJECT(current_object)->player)
		{
		case 0:
		case 3:
			start_bit = 4u;
			if (setting_no_start_buttons != 0u)
				start_bit = 0x10u;
			break;
		case 1:
			start_bit = 5u;
			if (setting_no_start_buttons != 0u)
				start_bit = 0x18u;
			break;
		case 2:
			start_bit = 7u;
			if (setting_no_start_buttons != 0u)
				start_bit = 0x12u;
			break;
		default:
			invalid_jump(0xD50Cu + OBJECT(current_object)->player * 2u);
		}
		if ((inputs_pressed & shl32(1u, (int)(start_bit))) == 0u)
			return joined;
		break;
	case 2:
		if (OBJECT(current_object)->player != 1u)
		{
			if (OBJECT(current_object)->player != 0u || p1_credits < (uint8_t)cost)
				return joined;
			credit_block = p1_coins_ADDRESS;
			bit = 0x10u;
			if (setting_no_start_buttons == 0u)
				bit = 4u;
			if ((inputs_pressed & shl32(1u, (int)(bit))) == 0u)
				return joined;
		}
		else
		{
			if (p2_credits < (uint8_t)cost)
				return joined;
			credit_block = p2_coins_ADDRESS;
			bit = 0x18u;
			if (setting_no_start_buttons == 0u)
				bit = 5u;
			if ((inputs_pressed & shl32(1u, (int)(bit))) == 0u)
				return joined;
		}
		break;
	case 3:
		if (OBJECT(current_object)->player != 2u)
		{
			if (OBJECT(current_object)->player != 1u)
			{
				if (OBJECT(current_object)->player != 0u || p1_credits < (uint8_t)cost)
					return joined;
				credit_block = p1_coins_ADDRESS;
				bit = 0x10u;
				if (setting_no_start_buttons == 0u)
					bit = 4u;
				if ((inputs_pressed & shl32(1u, (int)(bit))) == 0u)
					return joined;
			}
			else
			{
				if (p2_credits < (uint8_t)cost)
					return joined;
				credit_block = p2_coins_ADDRESS;
				bit = 0x18u;
				if (setting_no_start_buttons == 0u)
					bit = 5u;
				if ((inputs_pressed & shl32(1u, (int)(bit))) == 0u)
					return joined;
			}
		}
		else
		{
			if (p3_credits < (uint8_t)cost)
				return joined;
			credit_block = p3_coins_ADDRESS;
			bit = 0x12u;
			if (setting_no_start_buttons == 0u)
				bit = 7u;
			if ((inputs_pressed & shl32(1u, (int)(bit))) == 0u)
				return joined;
		}
		break;
	default:
		invalid_jump(0xD4E6u + cabinet_config * 3u);
	}
	CREDIT_BLOCK(credit_block)->credits -= cost;
	CREDIT_BLOCK(credit_block)->bonus_coins = 0u;
	cabinet_event = 1u;
	if (OBJECT(current_object)->player != 0u)
	{
		cabinet_event = 3u;
		if (OBJECT(current_object)->player != 1u)
			cabinet_event = 5u;
	}
	mem_write8(cabinet_events, cabinet_event);
	if (PLAYER(current_player)->state == 0x23u)
		mem_write8(cabinet_events, cabinet_event);
	if ((cabinet_config & 1u) != 0u)
	{
		save_credits();
		joined = 0xFFFFFFFFu;
		return joined;
	}
	bit = 2u;
	if (OBJECT(current_object)->player != 0u)
		bit = 3u;
	port_d = mem_read32(io_port_d);
	mem_write32(io_port_d, port_d & ~shl32(1u, (int)(bit)));
	save_credits();
	joined = 0xFFFFFFFFu;
	return joined;
}

uint32_t check_scene_states(uint32_t object)
{
	if (PLAYER(current_player)->state == 0xBu || PLAYER(current_player)->state == 0x23u || PLAYER(current_player)->state == 0x24u || PLAYER(current_player)->state == 0x16u || PLAYER(current_player)->state == 0x2Eu || PLAYER(current_player)->state == 0x37u)
		return object;
	if (stage == 5u && OBJECT(current_object)->y_int >= 0x400)
		scroll_mode = 7u;
	if (current_game_mode == 0xDu)
	{
		PLAYER(current_player)->previous_state = PLAYER(current_player)->state;
		PLAYER(current_player)->state = 0x19u;
	}
	if (current_game_mode == 1u)
	{
		PLAYER(current_player)->previous_state = PLAYER(current_player)->state;
		PLAYER(current_player)->state = 0x1Eu;
	}
	if (current_game_mode == 0x18u)
	{
		PLAYER(current_player)->previous_state = PLAYER(current_player)->state;
		PLAYER(current_player)->state = 0x2Bu;
	}
	if (current_game_mode == 0x19u && PLAYER(current_player)->state != 0x30u && rope_bridge_collapsing != 0u)
	{
		PLAYER(current_player)->substate = 0u;
		PLAYER(current_player)->previous_state = PLAYER(current_player)->state;
		PLAYER(current_player)->state = 0x30u;
	}
	if (current_game_mode == 0x1Au && PLAYER(current_player)->state != 0x32u)
	{
		PLAYER(current_player)->previous_state = PLAYER(current_player)->state;
		PLAYER(current_player)->state = 0x31u;
	}
	if (current_game_mode == 0x1Cu && event_1c_ride_phase != 0u)
	{
		if (event_1c_riding == 0u)
			object = spawn_player_ride(object);
		else if (PLAYER(current_player)->state != 6u && PLAYER(current_player)->state != 9u && PLAYER(current_player)->state != 0xAu)
		{
			PLAYER(current_player)->previous_state = PLAYER(current_player)->state;
			PLAYER(current_player)->state = 0x35u;
			if (PLAYER(current_player)->previous_state != 0x35u)
				PLAYER(current_player)->substate = 0u;
		}
	}
	if (current_game_mode == 0x17u || current_game_mode == 0x1Du || current_game_mode == 4u)
	{
		PLAYER(current_player)->state = 0x21u;
		PLAYER(current_player)->goal_step = 2u;
		if (stage == 4u)
			PLAYER(current_player)->goal_step = 4u;
	}
	if (PLAYER(current_player)->state != 0x26u && PLAYER(current_player)->state != 6u && PLAYER(current_player)->state != 8u && PLAYER(current_player)->state != 9u && players_pulled != 0u)
	{
		PLAYER(current_player)->previous_state = PLAYER(current_player)->state;
		PLAYER(current_player)->state = 0x26u;
		PLAYER(current_player)->substate = 0u;
	}
	if (current_game_mode == 3u && stage == 0xBu && (uint8_t)OBJECT(current_object)->health != 0u && PLAYER(current_player)->state != 9u && (uint16_t)OBJECT(current_object)->floor_z_int != 0x800u && PLAYER(current_player)->ending_done == 0u && OBJECT(current_object)->y_int <= 0x500)
	{
		PLAYER(current_player)->previous_state = PLAYER(current_player)->state;
		PLAYER(current_player)->state = 0x38u;
	}
	if (current_game_mode == 0x20u)
	{
		PLAYER(current_player)->previous_state = PLAYER(current_player)->state;
		PLAYER(current_player)->state = 0x39u;
	}
	return object;
}

void set_fall_sprite_priority(void)
{
	uint8_t priority = 0;
	uint32_t record = 0, side = 0;

	record = floor_blocks + 2u + OBJECT(current_object)->previous_floor_block;
	if (FLOOR_BLOCK(record)->type_index == 0x1Fu || FLOOR_BLOCK(record)->type_index == 0x11u)
	{
		side = 3u;
		if ((int32_t)((((uint32_t)FLOOR_BLOCK(record)->x_max - FLOOR_BLOCK(record)->x_min) >> 1) + FLOOR_BLOCK(record)->x_min) <= OBJECT(current_object)->x)
			side = 1u;
		record = fall_sprite_priorities_ADDRESS + FLOOR_BLOCK(record)->fall_priorities * 4u;
	}
	else
	{
		record = floor_blocks + 2u + OBJECT(current_object)->floor_block;
		if (FLOOR_BLOCK(record)->type_index == 0x1Eu || FLOOR_BLOCK(record)->type_index == 0x1Du)
		{
			side = 3u;
			if ((int32_t)((((uint32_t)FLOOR_BLOCK(record)->x_max - FLOOR_BLOCK(record)->x_min) >> 1) + FLOOR_BLOCK(record)->x_min) <= OBJECT(current_object)->x)
				side = 1u;
			record = fall_sprite_priorities_ADDRESS + FLOOR_BLOCK(record)->fall_priorities * 4u;
			priority = ROM8(record + side);
			if ((int8_t)priority >= 0)
			{
				OBJECT(current_object)->sprite_palette &= 0x9FFFu;
				OBJECT(current_object)->sprite_palette |= (uint32_t)priority << 13;
			}
			return;
		}
		record = fall_sprite_priorities_ADDRESS + mem_read8(floor_blocks + 2u + OBJECT(current_object)->previous_floor_block + 0x14u) * 4u;
		side = (uint8_t)PLAYER(current_player)->ledge_side;
		if (side == 4u)
			side = (uint8_t)PLAYER(current_player)->ledge_direction;
	}
	priority = ROM8(record + side);
	if ((int8_t)priority >= 0)
	{
		OBJECT(current_object)->sprite_palette &= 0x9FFFu;
		OBJECT(current_object)->sprite_palette |= (uint32_t)priority << 13;
	}
}

void start_climb_at_spawn(uint32_t climbed_height)
{
	uint32_t attribute_record = 0, block = 0, climb_animation = 0, distance = 0;

	if (OBJECT(current_object)->floor_type != 2u)
		return;
	climb_animation = sonic_climb_animation_ADDRESS;
	if (OBJECT(current_object)->kind != 0u)
	{
		climb_animation = ray_climb_animation_ADDRESS;
		if (OBJECT(current_object)->kind != 1u)
			climb_animation = mighty_climb_animation_ADDRESS;
	}
	OBJECT(current_object)->animation = climb_animation;
	OBJECT(current_object)->sprite_flags |= 0x40u;
	if (floor_types[OBJECT(current_object)->floor_attribute] != 0u)
		OBJECT(current_object)->sprite_flags &= 0xFFBFu;
	OBJECT(current_object)->animation_timer = 0;
	OBJECT(current_object)->animation_speed = 3u;
	OBJECT(current_object)->animation_frame = 0xFFu;
	attribute_record = OBJECT(current_object)->floor_attribute + floor_types_ADDRESS;
	PLAYER(current_player)->climb_remaining = ROM16(attribute_record + 6u);
	climbed_height = (climbed_height & ~0xFFFFu) | ROM16(attribute_record + 0xEu);
	block = floor_blocks + 2u + OBJECT(current_object)->floor_block;
	if (ROM8(attribute_record) == 0u)
		distance = (uint32_t)OBJECT(current_object)->x - FLOOR_BLOCK(block)->x_min;
	else
		distance = (uint32_t)FLOOR_BLOCK(block)->y_max - OBJECT(current_object)->y;
	climbed_height = climbed_height * ((int32_t)distance >> 8);
	OBJECT(current_object)->z -= climbed_height;
	PLAYER(current_player)->climb_remaining -= climbed_height >> 16;
	PLAYER(current_player)->climb_base_z_int = FLOOR_BLOCK(block)->height;
	PLAYER(current_player)->climb_base_z_fraction = 0u;
	PLAYER(current_player)->step_accumulator = 0;
	PLAYER(current_player)->substate = 3u;
	PLAYER(current_player)->state = 0xAu;
}

void start_climb(void)
{
	uint32_t animation = 0, ignored = 0;

	if (OBJECT(current_object)->floor_type != 2u)
		return;
	animation = sonic_climb_animation_ADDRESS;
	if (OBJECT(current_object)->kind != 0u)
	{
		animation = ray_climb_animation_ADDRESS;
		if (OBJECT(current_object)->kind != 1u)
			animation = mighty_climb_animation_ADDRESS;
	}
	OBJECT(current_object)->animation = animation;
	OBJECT(current_object)->sprite_flags |= 0x40u;
	if (floor_types[OBJECT(current_object)->floor_attribute] != 0u)
		OBJECT(current_object)->sprite_flags &= 0xFFBFu;
	OBJECT(current_object)->animation_timer = 0;
	OBJECT(current_object)->animation_speed = 3u;
	OBJECT(current_object)->animation_frame = 0xFFu;
	OBJECT(current_object)->x += 0x41000u;
	PLAYER(current_player)->climb_base_z_int = mem_read16(floor_blocks + 2u + OBJECT(current_object)->floor_block + 0x10u);
	PLAYER(current_player)->climb_base_z_fraction = 0u;
	PLAYER(current_player)->climb_remaining = ROM16(floor_attribute_wall_heights_ADDRESS + OBJECT(current_object)->floor_attribute);
	PLAYER(current_player)->climb_remaining -= PLAYER(current_player)->climb_base_z_int - OBJECT(current_object)->z_int;
	project_object(&ignored, &ignored);
	PLAYER(current_player)->screen_x_int = OBJECT(current_object)->projected_x;
	PLAYER(current_player)->screen_x_fraction = 0u;
	PLAYER(current_player)->step_accumulator = 0;
	PLAYER(current_player)->substate = 3u;
	PLAYER(current_player)->state = 0xAu;
}

void add_push_to_player_motion(void)
{
	uint16_t dy = 0;
	uint32_t angle = 0, ignored = 0, length = 0, sum_dx = 0, sum_dy = 0;

	if (PLAYER(current_player)->push_xy == 0u)
	{
		PLAYER(current_player)->push_xy = 0u;
		AS32(PLAYER(current_player)->push_y) = 0u;
		return;
	}
	length = (uint16_t)OBJECT(current_object)->speed_int;
	angle = PLAYER(current_player)->heading_snapped;
	polar_to_xy(&ignored, &angle, &length);
	push_motion_x = angle;
	push_motion_y = length;
	push_motion_x += PLAYER(current_player)->push_xy;
	push_motion_y += AS32(PLAYER(current_player)->push_y);
	sum_dx = (int32_t)push_motion_x >> 4;
	push_motion_x = sum_dx;
	sum_dy = (int32_t)push_motion_y >> 4;
	push_motion_y = sum_dy;
	sum_dx = sum_dx * sum_dx + sum_dy * sum_dy;
	square_root(&sum_dx, &ignored);
	OBJECT(current_object)->speed = (uint32_t)(uint16_t)sum_dx << 12;
	if (OBJECT(current_object)->speed_int < 0)
		OBJECT(current_object)->speed = 0;
	if (OBJECT(current_object)->speed > 0x5000000)
		OBJECT(current_object)->speed = 0x5000000;
	sum_dx = push_motion_x;
	dy = (uint16_t)push_motion_y;
	angle_of_vector(dy, &sum_dx, &ignored);
	PLAYER(current_player)->heading = sum_dx;
	PLAYER(current_player)->push_xy = 0u;
	AS32(PLAYER(current_player)->push_y) = 0u;
}

void apply_floor_push(void)
{
	uint32_t attribute_record = 0;

	if ((OBJECT(current_object)->flags & 0x80000000u) == 0u)
	{
		PLAYER(current_player)->push_xy = 0u;
		if (PLAYER(current_player)->invulnerable_timer < 0 && ((OBJECT(current_object)->floor_type == 7u && OBJECT(current_object)->floor_attribute != 0u) || OBJECT(current_object)->floor_type == 0xBu || OBJECT(current_object)->floor_type == 4u || (OBJECT(current_object)->floor_type == 5u && PLAYER(current_player)->state != 0x17u)))
		{
			attribute_record = OBJECT(current_object)->floor_attribute + floor_types_ADDRESS;
			PLAYER(current_player)->push_x += ROM16(attribute_record);
			PLAYER(current_player)->push_y += ROM16(attribute_record + 2u);
		}
	}
}

uint32_t spawn_slip_piece(uint32_t piece)
{
	uint32_t free_count = 0, ignored = 0;

	allocate_object(&free_count, &piece);
	if (free_count == 0u)
		return piece;
	OBJECT(piece)->sprite_flags = 0x1004u;
	if ((OBJECT(current_object)->sprite_flags & 0x40u) != 0u)
		OBJECT(piece)->sprite_flags |= 0x40u;
	OBJECT(piece)->sprite_frame = ROM16(OBJECT(current_object)->animation + 0x3Au);
	OBJECT(piece)->sprite_palette = OBJECT(current_object)->sprite_palette;
	OBJECT(piece)->sprite_offset_x = OBJECT(current_object)->sprite_offset_x;
	OBJECT(piece)->sprite_offset_y = OBJECT(current_object)->sprite_offset_y;
	OBJECT(piece)->flags = 0u;
	OBJECT(piece)->handler = slip_piece_update_ADDRESS;
	OBJECT(piece)->speed_x_fraction = 0x10u;
	OBJECT(piece)->speed_z = 0;
	OBJECT(piece)->direction = PLAYER(current_player)->ledge_side;
	OBJECT(piece)->x = OBJECT(current_object)->x;
	OBJECT(piece)->y = OBJECT(current_object)->y;
	OBJECT(piece)->z = OBJECT(current_object)->z;
	current_object = piece;
	project_object(&ignored, &ignored);
	current_object = object_cursor;
	return piece;
}

void slip_piece_update(void)
{
	uint8_t bucket = 0;
	uint32_t ignored = 0, off_screen = 0;
	uint16_t speed_x_fraction = 0;

	off_screen = is_off_screen();
	if (off_screen != 0u)
	{
		delete_current_object();
		return;
	}
	speed_x_fraction = OBJECT(current_object)->speed_x_fraction;
	OBJECT(current_object)->speed_x_fraction = speed_x_fraction - 1u;
	if ((int16_t)speed_x_fraction <= 1)
	{
		OBJECT(current_object)->speed_z += 0x6000u;
		OBJECT(current_object)->z += OBJECT(current_object)->speed_z;
	}
	update_screen_position(&ignored, &ignored);
	if ((int16_t)OBJECT(current_object)->direction >= 2)
	{
		ignored = queue_sprite_at_base(ignored);
		return;
	}
	bucket = 0xFFu;
	queue_sprite_in_bucket(bucket);
}

void spawn_slip_pieces(uint32_t piece)
{
	uint32_t ignored = 0;
	uint16_t pieces_left = 0;

	if (free_object_stack < 2)
		return;
	slip_pieces_left = 2;
	slip_piece_speed = 0x8000u;
	do
	{
		allocate_object(&ignored, &piece);
		OBJECT(piece)->sprite_flags = 0x1000u;
		OBJECT(piece)->animation = OBJECT(current_object)->animation + 0x3Au;
		OBJECT(piece)->sprite_palette = OBJECT(current_object)->sprite_palette;
		OBJECT(piece)->sprite_offset_x = 0;
		OBJECT(piece)->sprite_offset_y = 0;
		OBJECT(piece)->flags = 0u;
		OBJECT(piece)->handler = slip_pieces_update_ADDRESS;
		OBJECT(piece)->speed_z = -0x80000;
		OBJECT(piece)->direction = PLAYER(current_player)->ledge_side;
		OBJECT(piece)->animation_frame = 0u;
		OBJECT(piece)->animation_speed = 4u;
		OBJECT(piece)->animation_timer = 0;
		OBJECT(piece)->x = OBJECT(current_object)->x;
		OBJECT(piece)->y = OBJECT(current_object)->y;
		OBJECT(piece)->z = OBJECT(current_object)->z;
		switch (OBJECT(piece)->direction)
		{
		case 0:
		case 2:
		case 4:
			OBJECT(piece)->speed_x = slip_piece_speed;
			OBJECT(piece)->speed_y = 0;
			break;
		case 1:
		case 3:
			OBJECT(piece)->speed_y = slip_piece_speed;
			OBJECT(piece)->speed_x = 0;
			break;
		default:
			invalid_jump(0xDC37u + OBJECT(piece)->direction * 2u);
		}
		slip_piece_speed = 0u - slip_piece_speed;
		pieces_left = slip_pieces_left;
		slip_pieces_left = pieces_left - 1u;
	} while ((int16_t)pieces_left > 1);
}

void slip_pieces_update(void)
{
	uint8_t bucket = 0;
	uint16_t margin_x = 0, margin_y = 0;
	uint32_t ignored = 0, off_screen = 0;

	OBJECT(current_object)->x += OBJECT(current_object)->speed_x;
	OBJECT(current_object)->y += OBJECT(current_object)->speed_y;
	OBJECT(current_object)->speed_z += 0x6000u;
	OBJECT(current_object)->z += OBJECT(current_object)->speed_z;
	project_object(&ignored, &ignored);
	margin_x = 0x20u;
	margin_y = 0x50u;
	off_screen = is_off_screen_margin(margin_x, margin_y);
	if (off_screen != 0u)
	{
		delete_current_object();
		return;
	}
	animate(&ignored, &ignored, &ignored);
	if ((int16_t)OBJECT(current_object)->direction >= 2)
	{
		ignored = queue_sprite_at_base(ignored);
		return;
	}
	bucket = 0xFFu;
	queue_sprite_in_bucket(bucket);
}

uint32_t follow_player_path(uint32_t target_speed)
{
	if (PLAYER(current_player)->path_request != 0u)
	{
		PLAYER(current_player)->path = PLAYER(current_player)->path_request;
		PLAYER(current_player)->path_request = 0u;
		return follow_player_path_start();
	}
	if (all_players_path_request == 0u)
	{
		if (PLAYER(current_player)->path != 0u)
			return follow_player_path_run();
		return target_speed;
	}
	if (PLAYER(current_player)->path == all_players_path_request)
		return follow_player_path_run();
	PLAYER(current_player)->path = all_players_path_request;
	return follow_player_path_start();
}

uint32_t follow_player_path_start(void)
{
	uint32_t path = 0;

	path = PLAYER(current_player)->path;
	PLAYER(current_player)->path_pointer = player_path_offsets_ADDRESS + player_path_offsets[(uint8_t)(path + OBJECT(current_object)->player)];
	OBJECT(current_object)->speed = 0;
	OBJECT(current_object)->z = OBJECT(current_object)->floor_z;
	OBJECT(current_object)->speed_z = 0;
	PLAYER(current_player)->state_timer = 0;
	OBJECT(current_object)->sprite_palette = ROM16(OBJECT(current_object)->follow_player_path.palette_list + 2u);
	OBJECT(current_object)->flags &= ~0x40u;
	OBJECT(current_object)->flags &= ~0x2000u;
	PLAYER(current_player)->control_lock = 0u;
	PLAYER(current_player)->state = 1u;
	OBJECT(current_object)->flags |= 1u;
	return follow_player_path_load_record();
}

uint32_t follow_player_path_run(void)
{
	uint16_t x_offset = 0;
	uint32_t point = 0, target_speed = 0;
	uint16_t path_wait = 0;

	all_players_path_request = 0u;
	if ((uint16_t)PLAYER(current_player)->path_wait == 0u)
	{
		point = PLAYER(current_player)->path_pointer;
		if ((ROM32(point + 6u) & 2u) == 0u)
		{
			point += 2u;
			x_offset = (uint16_t)(OBJECT(current_object)->x_int - ROM16(point) + 8u);
			point += 2u;
			if (x_offset >= 0x11u || (uint16_t)(OBJECT(current_object)->y_int - ROM16(point) + 8u) >= 0x11u)
				return follow_player_path_aim_at_point();
		}
		return follow_player_path_next_record();
	}
	path_wait = PLAYER(current_player)->path_wait;
	PLAYER(current_player)->path_wait = path_wait - 1u;
	if ((int16_t)path_wait <= 1)
		return follow_player_path_next_record();
	target_speed = 0u;
	if (OBJECT(current_object)->speed_int < 0x20)
		OBJECT(current_object)->speed = 0;
	return target_speed;
}

uint32_t follow_player_path_next_record(void)
{
	if ((int16_t)ROM16(PLAYER(current_player)->path_pointer + 8u) >= 0)
	{
		PLAYER(current_player)->path_pointer += 8u;
		return follow_player_path_load_record();
	}
	PLAYER(current_player)->path = 0u;
	return follow_player_path_target_speed();
}

uint32_t follow_player_path_load_record(void)
{
	PLAYER(current_player)->path_speed = ROM16(PLAYER(current_player)->path_pointer);
	if ((ROM32(PLAYER(current_player)->path_pointer + 6u) & 1u) != 0u)
	{
		PLAYER(current_player)->state = 4u;
		PLAYER(current_player)->substate = 0u;
		PLAYER(current_player)->ledge_side = -1;
	}
	if (PLAYER(current_player)->path_speed != 0u)
	{
		PLAYER(current_player)->path_wait = 0;
		return follow_player_path_aim_at_point();
	}
	PLAYER(current_player)->path_wait = ROM16(PLAYER(current_player)->path_pointer + 2u);
	return follow_player_path_target_speed();
}

uint32_t follow_player_path_aim_at_point(void)
{
	uint16_t dy = 0;
	uint32_t ignored = 0, path_heading = 0, point = 0;
	uint32_t flags = 0;

	point = PLAYER(current_player)->path_pointer + 2u;
	path_heading = (uint16_t)(ROM16(point) - OBJECT(current_object)->x_int);
	dy = (uint16_t)(ROM16(point + 2u) - OBJECT(current_object)->y_int);
	angle_of_vector(dy, &path_heading, &ignored);
	path_heading = (path_heading - 0x80u) & 0x3FFu;
	PLAYER(current_player)->target_heading = path_heading;
	PLAYER(current_player)->facing_angle = path_heading;
	flags = OBJECT(current_object)->flags;
	OBJECT(current_object)->flags = flags & ~1u;
	if ((flags & 1u) == 0u)
		return follow_player_path_target_speed();
	PLAYER(current_player)->heading = path_heading;
	return follow_player_path_target_speed();
}

uint32_t follow_player_path_target_speed(void)
{
	uint32_t target_speed = 0;

	target_speed = (uint32_t)PLAYER(current_player)->path_speed << 16;
	if ((int32_t)target_speed <= 0x200000 && OBJECT(current_object)->speed_int < 0x20)
	{
		target_speed = 0u;
		OBJECT(current_object)->speed = 0;
	}
	return target_speed;
}

void check_player_contacts(uint32_t *p_health_limit, uint32_t *p_cursor)
{
	uint32_t health_limit = *p_health_limit;
	uint32_t cursor = *p_cursor;
	uint8_t new_substate = 0, sound_id = 0;
	uint16_t dy = 0, held_frame = 0, saved_sides = 0;
	uint32_t carried_x = 0, carried_y = 0, catapult = 0, distance = 0, draw_depth = 0, ignored = 0, joined_health_setting = 0, max_health = 0, move_angle = 0, object = 0, object_2 = 0, push_cos = 0, sides = 0;
	uint32_t flags = 0, flags_2 = 0;

	if (PLAYER(current_player)->path_request != 0u || all_players_path_request != 0u || (PLAYER(current_player)->path != 0u && (PLAYER(current_player)->path < 0x14 || PLAYER(current_player)->path > 0x16)) || (OBJECT(current_object)->flags & 4u) == 0u)
	{
		check_player_contacts_clear_state();
		*p_health_limit = health_limit;
		*p_cursor = cursor;
		return;
	}
	object = LINKED(WORK_RAM, OBJECT(current_object)->touched_by);
	if ((OBJECT(object)->flags & 0x200u) != 0u)
	{
		check_player_contacts_clear_state();
		*p_health_limit = health_limit;
		*p_cursor = cursor;
		return;
	}
	if (OBJECT(object)->kind != 8u)
		OBJECT(current_object)->flags &= ~0x40000u;
	switch (OBJECT(object)->kind)
	{
	case 0:
	case 1:
	case 2:
		return;
	case 3:
	case 42:
		PLAYER(current_player)->stage_rings++;
		PLAYER(current_player)->rings++;
		PLAYER(current_player)->flags |= 0x40u;
		switch (health_setting)
		{
		case 0:
		case 4:
			OBJECT(current_object)->health++;
			joined_health_setting = PLAYER(current_player)->joined_health_setting;
			max_health = (health_limit & ~0xFFu) | (uint8_t)max_health_by_setting[(uint16_t)(joined_health_setting * 2u)];
			if (OBJECT(current_object)->health <= (int8_t)max_health)
			{
				check_player_contacts_clear_state();
				*p_health_limit = max_health;
				*p_cursor = cursor;
				return;
			}
			OBJECT(current_object)->health = max_health;
			check_player_contacts_clear_state();
			*p_health_limit = max_health;
			*p_cursor = cursor;
			return;
		case 1:
		case 2:
			health_limit = (health_limit & ~0xFFu) | 4u;
			break;
		case 3:
			health_limit = (health_limit & ~0xFFu) | 8u;
			break;
		default:
			invalid_jump(0xE578u + health_setting * 2u);
		}
		if (players_in_play >= 2)
			health_limit = (health_limit & ~0xFFFFu) | ((uint16_t)health_limit >> 1);
		PLAYER(current_player)->rings_to_health++;
		if (PLAYER(current_player)->rings_to_health < (int8_t)health_limit)
		{
			joined_health_setting = PLAYER(current_player)->joined_health_setting;
			max_health = (health_limit & ~0xFFu) | (uint8_t)max_health_by_setting[(uint16_t)(joined_health_setting * 2u)];
			if (OBJECT(current_object)->health <= (int8_t)max_health)
			{
				check_player_contacts_clear_state();
				*p_health_limit = max_health;
				*p_cursor = cursor;
				return;
			}
			OBJECT(current_object)->health = max_health;
			check_player_contacts_clear_state();
			*p_health_limit = max_health;
			*p_cursor = cursor;
			return;
		}
		OBJECT(current_object)->health++;
		PLAYER(current_player)->rings_to_health = 0;
		joined_health_setting = PLAYER(current_player)->joined_health_setting;
		max_health = (health_limit & ~0xFFu) | (uint8_t)max_health_by_setting[(uint16_t)(joined_health_setting * 2u)];
		if (OBJECT(current_object)->health <= (int8_t)max_health)
		{
			check_player_contacts_clear_state();
			*p_health_limit = max_health;
			*p_cursor = cursor;
			return;
		}
		OBJECT(current_object)->health = max_health;
		check_player_contacts_clear_state();
		*p_health_limit = max_health;
		*p_cursor = cursor;
		return;
	case 4:
	case 15:
	case 16:
	case 22:
		check_player_contacts_hurt_unless_invulnerable(object);
		*p_health_limit = health_limit;
		*p_cursor = cursor;
		return;
	case 5:
		if (PLAYER(current_player)->invulnerable_timer >= 0)
		{
			check_player_contacts_clear_state();
			*p_health_limit = health_limit;
			*p_cursor = cursor;
			return;
		}
		health_limit = check_player_contacts_solid(health_limit, object);
		*p_health_limit = health_limit;
		*p_cursor = cursor;
		return;
	case 6:
		new_substate = 0u;
		catapult = LINKED(current_object, OBJECT(current_object)->touched_by);
		if (OBJECT(catapult)->catapult.state != 1u)
		{
			*p_cursor = catapult;
			return;
		}
		PLAYER(current_player)->state = 0xFu;
		PLAYER(current_player)->substate = new_substate;
		*p_cursor = catapult;
		return;
	case 7:
		new_substate = 1u;
		catapult = LINKED(current_object, OBJECT(current_object)->touched_by);
		if (OBJECT(catapult)->catapult.state != 1u)
		{
			*p_cursor = catapult;
			return;
		}
		PLAYER(current_player)->state = 0xFu;
		PLAYER(current_player)->substate = new_substate;
		*p_cursor = catapult;
		return;
	case 8:
	case 10:
		if ((OBJECT(current_object)->flags & 0x200000u) != 0u || PLAYER(current_player)->state != 4u)
			return;
		PLAYER(current_player)->holder = OBJECT(current_object)->touched_by;
		PLAYER(current_player)->state = 0x15u;
		PLAYER(current_player)->substate = 0u;
		OBJECT(current_object)->sprite_flags &= 0xFFBFu;
		held_frame = 0x1B10u;
		if (OBJECT(current_object)->kind != 0u)
		{
			held_frame = 0x5230u;
			if (OBJECT(current_object)->kind != 1u)
				held_frame = 0x3678u;
		}
		OBJECT(current_object)->sprite_frame = held_frame;
		OBJECT(current_object)->sprite_palette = ROM16(OBJECT(current_object)->check_player.palette_list + 2u);
		OBJECT(current_object)->flags &= ~0x40u;
		OBJECT(current_object)->flags &= ~0x2000u;
		return;
	case 9:
	case 13:
	case 24:
	case 40:
		check_player_contacts_hurt(object);
		*p_health_limit = health_limit;
		*p_cursor = cursor;
		return;
	case 11:
		new_substate = 1u;
		check_player_contacts_attack_or_hurt(new_substate, object);
		*p_health_limit = health_limit;
		*p_cursor = cursor;
		return;
	case 12:
	case 32:
	case 33:
		health_limit = check_player_contacts_solid(health_limit, object);
		*p_health_limit = health_limit;
		*p_cursor = cursor;
		return;
	case 14:
		new_substate = 7u;
		if ((uint16_t)object != PLAYER(current_player)->last_hazard)
		{
			PLAYER(current_player)->substate = new_substate;
			PLAYER(current_player)->last_hazard = object;
			PLAYER(current_player)->hurt_contact_delay = 2;
		}
		PLAYER(current_player)->state = 0xDu;
		return;
	case 17:
		if (PLAYER(current_player)->state == 4u)
		{
			check_player_contacts_attack(object);
			*p_health_limit = health_limit;
			*p_cursor = cursor;
			return;
		}
		if (OBJECT(current_object)->speed_int < 0x200)
		{
			health_limit = check_player_contacts_solid(health_limit, object);
			*p_health_limit = health_limit;
			*p_cursor = cursor;
			return;
		}
		PLAYER(current_player)->state = 0x1Du;
		PLAYER(current_player)->substate = 0u;
		return;
	case 18:
		if (PLAYER(current_player)->state == 0x2Fu)
			return;
		PLAYER(current_player)->state = 0x2Fu;
		PLAYER(current_player)->substate = 0u;
		return;
	case 19:
		PLAYER(current_player)->state = 0x1Fu;
		PLAYER(current_player)->substate = 0u;
		return;
	case 20:
		if ((uint16_t)object == PLAYER(current_player)->last_hazard)
		{
			PLAYER(current_player)->state = 0x20u;
			return;
		}
		PLAYER(current_player)->substate = 0u;
		PLAYER(current_player)->last_hazard = object;
		PLAYER(current_player)->hurt_contact_delay = 2;
		PLAYER(current_player)->state = 0x20u;
		return;
	case 21:
		new_substate = 7u;
		if (PLAYER(current_player)->invulnerable_timer >= 0)
		{
			check_player_contacts_clear_state();
			*p_health_limit = health_limit;
			*p_cursor = cursor;
			return;
		}
		if ((uint16_t)object != PLAYER(current_player)->last_hazard)
		{
			PLAYER(current_player)->substate = new_substate;
			PLAYER(current_player)->last_hazard = object;
			PLAYER(current_player)->hurt_contact_delay = 2;
		}
		PLAYER(current_player)->state = 0xDu;
		return;
	case 23:
		if (PLAYER(current_player)->invulnerable_timer >= 0)
		{
			check_player_contacts_clear_state();
			*p_health_limit = health_limit;
			*p_cursor = cursor;
			return;
		}
		if ((uint16_t)object == PLAYER(current_player)->last_hazard)
		{
			PLAYER(current_player)->state = 0x20u;
			return;
		}
		PLAYER(current_player)->substate = 0u;
		PLAYER(current_player)->last_hazard = object;
		PLAYER(current_player)->hurt_contact_delay = 2;
		PLAYER(current_player)->state = 0x20u;
		return;
	case 25:
		if (PLAYER(current_player)->invulnerable_timer >= 0)
		{
			check_player_contacts_clear_state();
			*p_health_limit = health_limit;
			*p_cursor = cursor;
			return;
		}
		contact_saved_x = OBJECT(current_object)->x;
		contact_saved_y = OBJECT(current_object)->y;
		OBJECT(current_object)->x = PLAYER(current_player)->previous_x;
		OBJECT(current_object)->y = PLAYER(current_player)->previous_y;
		OBJECT(current_object)->z = PLAYER(current_player)->previous_z;
		OBJECT(current_object)->floor_block = OBJECT(current_object)->previous_floor_block;
		OBJECT(current_object)->floor_z = OBJECT(current_object)->previous_floor_z;
		side_of_box(object, &health_limit, &sides);
		contact_box_side = sides;
		if ((uint16_t)sides != 8u && (uint16_t)sides != 0u && PLAYER(current_player)->state == 4u)
		{
			check_player_contacts_attack(object);
			*p_health_limit = health_limit;
			*p_cursor = cursor;
			return;
		}
		OBJECT(current_object)->flags |= 0x1000u;
		saved_sides = contact_box_side;
		if (saved_sides != 0u)
		{
			if (saved_sides != 8u)
			{
				if (OBJECT(current_object)->speed_int >= 0x500)
				{
					AS16(PLAYER(current_player)->bounce_direction) = ((OBJECT(current_object)->direction + 0x80u) & 0x3FFu) >> 8;
					PLAYER(current_player)->state = 0xEu;
					PLAYER(current_player)->substate = 0u;
					*p_health_limit = health_limit;
					return;
				}
				OBJECT(current_object)->speed = 0;
				OBJECT(current_object)->speed_x = 0;
				OBJECT(current_object)->speed_y = 0;
				PLAYER(current_player)->control_lock = 0u;
				OBJECT(current_object)->flags |= 0x800000u;
				PLAYER(current_player)->free_turn_timer = 3u;
				PLAYER(current_player)->state = 0u;
				PLAYER(current_player)->state_timer = -1;
				OBJECT(current_object)->flags &= ~0x2000u;
				OBJECT(current_object)->sprite_palette = ROM16(OBJECT(current_object)->check_player.palette_list + 2u);
				OBJECT(current_object)->flags &= ~0x40u;
				OBJECT(current_object)->flags &= ~0x100000u;
				OBJECT(current_object)->flags &= ~0x200000u;
				*p_health_limit = health_limit;
				return;
			}
			OBJECT(current_object)->x = contact_saved_x;
			OBJECT(current_object)->y = contact_saved_y;
		}
		carried_x = OBJECT(object)->speed_x;
		carried_y = OBJECT(object)->speed_y * 2u;
		unproject_point(&carried_x, &carried_y);
		OBJECT(current_object)->x += carried_x;
		OBJECT(current_object)->x += carried_x;
		PLAYER(current_player)->previous_x = OBJECT(current_object)->x;
		OBJECT(current_object)->y += carried_y;
		PLAYER(current_player)->previous_y = OBJECT(current_object)->y;
		OBJECT(current_object)->speed = 0;
		OBJECT(current_object)->speed_x = 0;
		OBJECT(current_object)->speed_y = 0;
		PLAYER(current_player)->control_lock = 0u;
		OBJECT(current_object)->flags |= 0x800000u;
		PLAYER(current_player)->free_turn_timer = 3u;
		PLAYER(current_player)->state = 0u;
		PLAYER(current_player)->state_timer = -1;
		OBJECT(current_object)->flags &= ~0x2000u;
		OBJECT(current_object)->sprite_palette = ROM16(OBJECT(current_object)->check_player.palette_list + 2u);
		OBJECT(current_object)->flags &= ~0x40u;
		OBJECT(current_object)->flags &= ~0x100000u;
		OBJECT(current_object)->flags &= ~0x200000u;
		*p_health_limit = health_limit;
		return;
	case 26:
		object_2 = LINKED(current_object, OBJECT(object)->parent);
		if (OBJECT(object_2)->var_82 != 0x300u)
		{
			*p_cursor = object_2;
			return;
		}
		PLAYER(current_player)->state = 0x27u;
		PLAYER(current_player)->substate = 0u;
		PLAYER(current_player)->climb_remaining = OBJECT(object)->spawn_arg_2;
		*p_cursor = object_2;
		return;
	case 27:
		OBJECT(current_object)->flags |= 0x20000u;
		PLAYER(current_player)->flags &= ~2u;
		PLAYER(current_player)->flags &= ~0x80u;
		if (PLAYER(current_player)->state == 7u || PLAYER(current_player)->state == 8u)
			return;
		if ((uint32_t)OBJECT(current_object)->speed_x == 0u && (uint32_t)OBJECT(current_object)->speed_y == 0u)
		{
			OBJECT(current_object)->speed = 0;
			OBJECT(current_object)->direction = OBJECT(object)->direction;
		}
		if (PLAYER(current_player)->state == 0xCu && (uint16_t)OBJECT(current_object)->speed_int == 0u)
		{
			OBJECT(current_object)->speed = 0x1000000;
			set_speed_from_direction();
		}
		PLAYER(current_player)->previous_state = PLAYER(current_player)->state;
		PLAYER(current_player)->state = 7u;
		PLAYER(current_player)->substate = 1u;
		OBJECT(current_object)->floor_block = -1;
		OBJECT(current_object)->floor_z = 0x8000000;
		PLAYER(current_player)->ledge_pullback = 0;
		PLAYER(current_player)->slip_distance = 0;
		PLAYER(current_player)->control_lock = 0u;
		OBJECT(current_object)->flags &= ~0x100u;
		OBJECT(current_object)->sprite_palette = ROM16(OBJECT(current_object)->check_player.palette_list + 2u);
		OBJECT(current_object)->flags &= ~0x40u;
		OBJECT(current_object)->flags &= ~0x2000u;
		PLAYER(current_player)->heading = OBJECT(object)->direction;
		OBJECT(current_object)->speed_x = OBJECT(current_object)->speed_x * 2u;
		OBJECT(current_object)->speed_y = OBJECT(current_object)->speed_y * 2u;
		PLAYER(current_player)->ledge_side = OBJECT(object)->direction >> 8;
		draw_depth = (int32_t)((uint32_t)OBJECT(current_object)->projected_y + OBJECT(current_object)->previous_floor_z) >> 16;
		draw_depth = (draw_depth & ~0xFFFFu) | (uint16_t)(draw_depth - view_y);
		PLAYER(current_player)->fall_draw_depth = draw_depth;
		*p_health_limit = draw_depth;
		return;
	case 28:
		if (PLAYER(current_player)->invulnerable_timer >= 0)
		{
			check_player_contacts_clear_state();
			*p_health_limit = health_limit;
			*p_cursor = cursor;
			return;
		}
		flags = OBJECT(current_object)->flags;
		OBJECT(current_object)->flags = flags | 0x80000000u;
		if ((flags & 0x80000000u) == 0u)
		{
			sound_id = 0x36u;
			if (OBJECT(current_object)->kind != 0u)
			{
				sound_id = 0x46u;
				if (OBJECT(current_object)->kind != 1u)
					sound_id = 0x56u;
			}
			play_sound(sound_id, &ignored, &ignored);
		}
		distance = (uint16_t)OBJECT(current_object)->touch_distance;
		square_root(&distance, &ignored);
		OBJECT(current_object)->touch_distance = distance;
		if (OBJECT(current_object)->touch_distance <= 8)
		{
			flags_2 = PLAYER(current_player)->flags;
			PLAYER(current_player)->flags = flags_2 | 4u;
			if ((flags_2 & 4u) == 0u)
			{
				PLAYER(current_player)->sink_depth = 0;
				PLAYER(current_player)->mishap_flag = 0xFFu;
				sound_id = 0x35u;
				if (OBJECT(current_object)->kind != 0u)
				{
					sound_id = 0x31u;
					if (OBJECT(current_object)->kind != 1u)
						sound_id = 0x3Du;
				}
				play_sound(sound_id, &ignored, &ignored);
				cursor = spawn_sink_swirl(cursor);
			}
		}
		object = LINKED(current_object, OBJECT(current_object)->touched_by);
		move_angle = (uint16_t)(OBJECT(object)->x_int - OBJECT(current_object)->x_int);
		dy = (uint16_t)(OBJECT(object)->y_int - OBJECT(current_object)->y_int);
		angle_of_vector(dy, &move_angle, &ignored);
		move_angle = (uint16_t)(move_angle - 0x80u) & 0x3FFu;
		PLAYER(current_player)->push_x = (int32_t)(sine_table[move_angle] * 0x6000u) >> 16;
		push_cos = (int32_t)(cosine_table[move_angle] * 0x6000u) >> 16;
		PLAYER(current_player)->push_y = 0u - push_cos;
		*p_health_limit = push_cos;
		*p_cursor = cursor;
		return;
	case 29:
		PLAYER(current_player)->flags |= 2u;
		OBJECT(current_object)->flags &= ~0x20000u;
		return;
	case 30:
		if ((PLAYER(current_player)->flags & 4u) != 0u)
			return;
		PLAYER(current_player)->state = 0x28u;
		PLAYER(current_player)->substate = 0u;
		return;
	case 31:
		if (PLAYER(current_player)->invulnerable_timer >= 0)
		{
			check_player_contacts_clear_state();
			*p_health_limit = health_limit;
			*p_cursor = cursor;
			return;
		}
		if (PLAYER(current_player)->state == 0x29u || PLAYER(current_player)->state == 5u || PLAYER(current_player)->state == 7u || PLAYER(current_player)->state == 0xDu || OBJECT(current_object)->floor_type == 0xAu || OBJECT(current_object)->floor_type == 0xBu)
			return;
		PLAYER(current_player)->state = 0x29u;
		PLAYER(current_player)->substate = 0u;
		return;
	case 34:
		if (PLAYER(current_player)->invulnerable_timer >= 0)
		{
			check_player_contacts_clear_state();
			*p_health_limit = health_limit;
			*p_cursor = cursor;
			return;
		}
		PLAYER(current_player)->state = 0x1Fu;
		PLAYER(current_player)->substate = 0u;
		return;
	case 35:
		PLAYER(current_player)->state = 0x2Cu;
		PLAYER(current_player)->substate = 0u;
		return;
	case 36:
		new_substate = 0u;
		if (PLAYER(current_player)->invulnerable_timer >= 0)
		{
			check_player_contacts_clear_state();
			*p_health_limit = health_limit;
			*p_cursor = cursor;
			return;
		}
		if ((uint16_t)object != PLAYER(current_player)->last_hazard)
		{
			PLAYER(current_player)->substate = new_substate;
			PLAYER(current_player)->last_hazard = object;
			PLAYER(current_player)->hurt_contact_delay = 2;
		}
		PLAYER(current_player)->state = 0xDu;
		return;
	case 37:
		PLAYER(current_player)->state = 0x2Du;
		PLAYER(current_player)->substate = 0u;
		return;
	case 38:
		PLAYER(current_player)->state = 0x36u;
		PLAYER(current_player)->substate = 0u;
		return;
	case 39:
		OBJECT(current_object)->flags |= 0x1000u;
		check_player_contacts_hurt_unless_invulnerable(object);
		*p_health_limit = health_limit;
		*p_cursor = cursor;
		return;
	case 41:
		OBJECT(current_object)->flags |= 0x1000u;
		check_player_contacts_hurt(object);
		*p_health_limit = health_limit;
		*p_cursor = cursor;
		return;
	default:
		invalid_jump(0xE4DFu + OBJECT(object)->kind * 3u);
	}
}

void check_player_contacts_clear_state(void)
{
	PLAYER(current_player)->pushed_out = 0;
	PLAYER(current_player)->last_hazard = 0xFFFFu;
	PLAYER(current_player)->last_damaged = 0xFFFFu;
	OBJECT(current_object)->flags &= ~0x40000u;
	OBJECT(current_object)->flags &= ~0x800000u;
	OBJECT(current_object)->flags &= 0x7FFFFFFFu;
	PLAYER(current_player)->flags &= ~2u;
}

void check_player_contacts_hurt_unless_invulnerable(uint32_t toucher)
{
	if (PLAYER(current_player)->invulnerable_timer >= 0)
	{
		check_player_contacts_clear_state();
		return;
	}
	check_player_contacts_hurt(toucher);
}

void check_player_contacts_hurt(uint32_t toucher)
{
	uint8_t new_substate = 0;

	new_substate = 0u;
	check_player_contacts_attack_or_hurt(new_substate, toucher);
}

void check_player_contacts_attack_or_hurt(uint8_t new_substate, uint32_t toucher)
{
	if (PLAYER(current_player)->state == 4u)
	{
		check_player_contacts_attack(toucher);
		return;
	}
	if ((uint16_t)toucher != PLAYER(current_player)->last_hazard)
	{
		PLAYER(current_player)->substate = new_substate;
		PLAYER(current_player)->last_hazard = toucher;
		PLAYER(current_player)->hurt_contact_delay = 2;
	}
	PLAYER(current_player)->state = 0xDu;
}

uint32_t check_player_contacts_solid(uint32_t health_limit, uint32_t toucher)
{
	uint32_t dx = 0, dy = 0;
	uint8_t push_out_delay = 0, push_out_delay_2 = 0;

	if (PLAYER(current_player)->state == 4u)
	{
		check_player_contacts_attack(toucher);
		return health_limit;
	}
	if (PLAYER(current_player)->state == 0xAu)
	{
		if (PLAYER(current_player)->invulnerable_timer >= 0)
		{
			check_player_contacts_clear_state();
			return health_limit;
		}
		if ((uint8_t)PLAYER(current_player)->pushed_out != 0u)
			return health_limit;
		if (PLAYER(current_player)->last_hazard != (uint16_t)toucher)
		{
			PLAYER(current_player)->push_out_delay = 0x1E;
			PLAYER(current_player)->last_hazard = toucher;
		}
		push_out_delay = PLAYER(current_player)->push_out_delay;
		PLAYER(current_player)->push_out_delay = push_out_delay - 1u;
		if ((int8_t)push_out_delay <= 1)
			PLAYER(current_player)->pushed_out = -1;
		PLAYER(current_player)->screen_x = PLAYER(current_player)->saved_screen_x;
		OBJECT(current_object)->projected_y = PLAYER(current_player)->saved_screen_y;
		OBJECT(current_object)->z = PLAYER(current_player)->previous_z;
		PLAYER(current_player)->climb_remaining = PLAYER(current_player)->saved_climb_remaining;
		OBJECT(current_object)->animation_speed = 0xFFu;
		OBJECT(current_object)->animation_frame = 0u;
		PLAYER(current_player)->step_accumulator = 0;
		PLAYER(current_player)->side_step_accumulator = 0;
		return health_limit;
	}
	if (PLAYER(current_player)->state == 0x2Fu)
		return health_limit;
	if (PLAYER(current_player)->pushed_out < 0)
	{
		if ((uint8_t)PLAYER(current_player)->pushed_out == 0u)
		{
			aim_at_screen_centre(&health_limit, &dx, &dy);
			PLAYER(current_player)->push_out_x = dx;
			PLAYER(current_player)->push_out_y = dy;
			OBJECT(current_object)->speed = 0;
			PLAYER(current_player)->ground_speed = 0;
		}
		PLAYER(current_player)->pushed_out = -1;
		PLAYER(current_player)->previous_x = OBJECT(current_object)->x;
		PLAYER(current_player)->previous_y = OBJECT(current_object)->y;
		OBJECT(current_object)->x += PLAYER(current_player)->push_out_x;
		OBJECT(current_object)->y += PLAYER(current_player)->push_out_y;
		PLAYER(current_player)->jump_delay = 2;
		PLAYER(current_player)->state = 0u;
		return health_limit;
	}
	if (PLAYER(current_player)->last_hazard != (uint16_t)toucher)
	{
		PLAYER(current_player)->push_out_delay = 0x1E;
		PLAYER(current_player)->last_hazard = toucher;
		OBJECT(current_object)->flags |= 0x1000u;
		OBJECT(current_object)->x = PLAYER(current_player)->previous_x;
		OBJECT(current_object)->y = PLAYER(current_player)->previous_y;
		OBJECT(current_object)->z = PLAYER(current_player)->previous_z;
		OBJECT(current_object)->floor_block = OBJECT(current_object)->previous_floor_block;
		OBJECT(current_object)->floor_z = OBJECT(current_object)->previous_floor_z;
		if (OBJECT(toucher)->kind != 0x20u && OBJECT(current_object)->speed_int >= 0x500)
		{
			AS16(PLAYER(current_player)->bounce_direction) = ((OBJECT(current_object)->direction + 0x80u) & 0x3FFu) >> 8;
			PLAYER(current_player)->state = 0xEu;
			PLAYER(current_player)->substate = 0u;
			return health_limit;
		}
		OBJECT(current_object)->speed = 0;
		OBJECT(current_object)->speed_x = 0;
		OBJECT(current_object)->speed_y = 0;
		PLAYER(current_player)->control_lock = 0u;
		OBJECT(current_object)->flags |= 0x800000u;
		PLAYER(current_player)->free_turn_timer = 3u;
		PLAYER(current_player)->state = 0u;
		return health_limit;
	}
	push_out_delay_2 = PLAYER(current_player)->push_out_delay;
	PLAYER(current_player)->push_out_delay = push_out_delay_2 - 1u;
	if ((int8_t)push_out_delay_2 <= 1)
	{
		if ((uint8_t)PLAYER(current_player)->pushed_out == 0u)
		{
			aim_at_screen_centre(&health_limit, &dx, &dy);
			PLAYER(current_player)->push_out_x = dx;
			PLAYER(current_player)->push_out_y = dy;
			OBJECT(current_object)->speed = 0;
			PLAYER(current_player)->ground_speed = 0;
		}
		PLAYER(current_player)->pushed_out = -1;
		PLAYER(current_player)->previous_x = OBJECT(current_object)->x;
		PLAYER(current_player)->previous_y = OBJECT(current_object)->y;
		OBJECT(current_object)->x += PLAYER(current_player)->push_out_x;
		OBJECT(current_object)->y += PLAYER(current_player)->push_out_y;
		PLAYER(current_player)->jump_delay = 2;
		PLAYER(current_player)->state = 0u;
		return health_limit;
	}
	OBJECT(current_object)->flags |= 0x1000u;
	OBJECT(current_object)->x = PLAYER(current_player)->previous_x;
	OBJECT(current_object)->y = PLAYER(current_player)->previous_y;
	OBJECT(current_object)->z = PLAYER(current_player)->previous_z;
	OBJECT(current_object)->floor_block = OBJECT(current_object)->previous_floor_block;
	OBJECT(current_object)->floor_z = OBJECT(current_object)->previous_floor_z;
	if (OBJECT(toucher)->kind != 0x20u && OBJECT(current_object)->speed_int >= 0x500)
	{
		AS16(PLAYER(current_player)->bounce_direction) = ((OBJECT(current_object)->direction + 0x80u) & 0x3FFu) >> 8;
		PLAYER(current_player)->state = 0xEu;
		PLAYER(current_player)->substate = 0u;
		return health_limit;
	}
	OBJECT(current_object)->speed = 0;
	OBJECT(current_object)->speed_x = 0;
	OBJECT(current_object)->speed_y = 0;
	PLAYER(current_player)->control_lock = 0u;
	OBJECT(current_object)->flags |= 0x800000u;
	PLAYER(current_player)->free_turn_timer = 3u;
	PLAYER(current_player)->state = 0u;
	return health_limit;
}

void check_player_contacts_attack(uint32_t toucher)
{
	uint32_t move_angle = 0;
	uint8_t attack_power = 0, health = 0;

	if (PLAYER(current_player)->last_damaged == (uint16_t)toucher)
		return;
	PLAYER(current_player)->last_damaged = toucher;
	OBJECT(toucher)->flags |= 0x40u;
	attack_power = PLAYER(current_player)->attack_power;
	health = OBJECT(toucher)->health;
	OBJECT(toucher)->health = health - attack_power;
	if ((int8_t)health <= (int8_t)attack_power && OBJECT(toucher)->kind != 0xCu && OBJECT(toucher)->kind != 9u && OBJECT(toucher)->kind != 0xBu && OBJECT(toucher)->kind != 0xDu && OBJECT(toucher)->kind != 0xEu && OBJECT(toucher)->kind != 0x11u && OBJECT(toucher)->kind != 0x18u && OBJECT(toucher)->kind != 0x19u)
	{
		PLAYER(current_player)->hit_stop = 3u;
		return;
	}
	OBJECT(current_object)->flags |= 0x2000u;
	if ((OBJECT(toucher)->flags & 0x40000000u) != 0u)
	{
		OBJECT(current_object)->direction = OBJECT(toucher)->direction;
		face_movement_direction(move_angle);
		OBJECT(current_object)->speed = 0x4000000;
		OBJECT(current_object)->speed_z = -0x18000;
		PLAYER(current_player)->state_timer = 6;
		OBJECT(current_object)->x = PLAYER(current_player)->previous_x;
		OBJECT(current_object)->y = PLAYER(current_player)->previous_y;
		return;
	}
	if ((uint16_t)OBJECT(current_object)->speed_int != 0u)
		move_angle = (uint16_t)(OBJECT(current_object)->direction + 0x200u);
	else
		move_angle = OBJECT(toucher)->direction;
	OBJECT(current_object)->direction = move_angle & 0x3FFu;
	face_movement_direction(move_angle);
	OBJECT(current_object)->speed = 0x4000000;
	OBJECT(current_object)->speed_z = -0x18000;
	PLAYER(current_player)->state_timer = 6;
	OBJECT(current_object)->x = PLAYER(current_player)->previous_x;
	OBJECT(current_object)->y = PLAYER(current_player)->previous_y;
}
