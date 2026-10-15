/**
 *    Name:        player/player.c
 *    Description: the player object: starting, joining, resetting and placing a player, and its handler player_update
 *    Version:     0.1.0
 *    Created:     12-10-2026
 *    Modified:    15-10-2026
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
		OBJECT(current_object)->handler = player_reset_joined_ADDRESS;
		player_reset_joined();
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
	AS8(PLAYER(current_player)->previous_z) = 0u;
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
	uint32_t stage_index = 0, write_index = 0, write_index_address = 0;

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
	OBJECT(current_object)->sprite_palette = mem_read16(OBJECT(current_object)->player_view.palette_list + 2u);
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
	play_sound(sound_id, &write_index, &write_index_address);
	player_place();
}

void player_place(void)
{
	uint8_t entry_status_2 = 0;
	uint32_t r0 = 0, block_offset = 0, cursor = 0, entry_status = 0, ignored = 0;

	current_player = PLAYER_BLOCK(1) + ((uint32_t)OBJECT(current_object)->player << 7) * 4u;
	r0 = place_player_at_restart_point();
	find_floor(&r0, &entry_status, &block_offset, &ignored, &cursor);
	OBJECT(current_object)->z = OBJECT(current_object)->floor_z;
	PLAYER(current_player)->substate = 0u;
	PLAYER(current_player)->state = 0u;
	start_climb_at_spawn(r0);
	if (OBJECT(current_object)->floor_z_int >= 0x7FF)
	{
	player_place_drop_in:
		entry_status_2 = 0x81u;
	player_place_retry:
		OBJECT(current_object)->handler = player_place_ADDRESS;
	}
	else
	{
		if (AT16(PLAYER(current_player)->previous_z, 2) == 0x800u)
		{
			entry_status_2 = 0xFFu;
			goto player_place_retry;
		}
		if ((OBJECT(current_object)->flags & 4u) != 0u || (OBJECT(current_object)->flags & 0x8000000u) != 0u)
			goto player_place_drop_in;
		OBJECT(current_object)->handler = player_update_ADDRESS;
		entry_status_2 = 0xFFu;
	}
	PLAYER(current_player)->entry_status = entry_status_2;
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
		AS16(PLAYER(current_player)->screen_x) = 0u;
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
			AS16(PLAYER(current_player)->screen_x) = 0u;
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
	uint32_t r1 = 0, r11 = 0, jump_offset = 0;
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
	jump_offset = PLAYER(current_player)->state * 3u;
	switch (PLAYER(current_player)->state)
	{
	case 0:
		player_move(r1, r11);
		return;
	case 1:
		player_move(r1, r11);
		return;
	case 2:
		player_move(r1, r11);
		return;
	case 3:
		player_state_inactive();
		return;
	case 4:
		player_move(r1, r11);
		return;
	case 5:
		player_state_teetering(r11);
		return;
	case 6:
		player_state_falling_off_edge();
		return;
	case 7:
		player_state_over_edge(jump_offset, r11);
		return;
	case 8:
		player_state_dropping(r11);
		return;
	case 9:
		player_state_falling_into_pit();
		return;
	case 10:
		player_state_climbing(r1, r11);
		return;
	case 11:
		player_state_not_playing();
		return;
	case 12:
		player_move(r1, r11);
		return;
	case 13:
		player_state_hurt(r1, r11);
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
		player_move(r1, r11);
		return;
	case 20:
		player_move(r1, r11);
		return;
	case 21:
		player_state_held(r1, r11);
		return;
	case 22:
		player_state_dying();
		return;
	case 23:
		player_move(r1, r11);
		return;
	case 24:
		player_state_stage_scene(r1, r11);
		return;
	case 25:
		player_state_scene_thrown(r1, r11);
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
		player_state_stuck(r11);
		return;
	case 30:
		player_state_intro_turn(r1, r11);
		return;
	case 31:
		player_state_blown_away(r11);
		return;
	case 32:
		player_state_tumbling(r1, r11);
		return;
	case 33:
		player_state_stage_exit(r1, r11);
		return;
	case 34:
		player_state_in_zone(r1, r11);
		return;
	case 35:
		player_state_continue();
		return;
	case 36:
		player_state_out_of_game();
		return;
	case 37:
		player_state_floor_8_sequence(r11);
		return;
	case 38:
		player_state_pulled(r11);
		return;
	case 39:
		player_state_climbing_object(r11);
		return;
	case 40:
		player_state_swept_away();
		return;
	case 41:
		player_state_tripped(r11);
		return;
	case 42:
		player_state_entering_stage(r1, r11);
		return;
	case 43:
		player_state_final_stage();
		return;
	case 44:
		player_state_stuck_on_object(r1, r11);
		return;
	case 45:
		player_state_held_still();
		return;
	case 46:
		player_state_game_over(jump_offset);
		return;
	case 47:
		player_state_stunned(r1, r11);
		return;
	case 48:
		player_state_caught(r11);
		return;
	case 49:
		player_state_carried_off(r1, r11);
		return;
	case 50:
		player_state_trapped(r1, r11);
		return;
	case 51:
		player_state_sinking(r11);
		return;
	case 52:
		player_state_choosing_character(r11);
		return;
	case 53:
		player_state_riding();
		return;
	case 54:
		player_state_knocked_off(r1, r11);
		return;
	case 55:
		player_state_not_playing();
		return;
	case 56:
		player_state_stage_b_sequence(r1, r11);
		return;
	case 57:
		player_state_you_failed();
		return;
	case 58:
		player_move(r1, r11);
		return;
	default:
		unknown_code(0x2D3Eu + PLAYER(current_player)->state * 3u);
	}
}
