/**
 *    Name:        stage/game_modes.c
 *    Description: the game modes (run_game_mode) and what a stage frame runs: starting a stage, layer blending, the stage clear check, the player summary, stage slots and events, restart points, the player objects, the GAME OVER banner and the demo prompt
 *    Version:     0.1.0
 *    Created:     12-10-2026
 *    Modified:    16-10-2026
 *    Author:      Sebastian Toyos
 *
 *    SegaSonic The Hedgehog (arcade, Sega System 32): the game's V60 program decompiled into C by
 *    tools/decompile.py, with the names of symbols.txt.
 */
#include "game.h"

void run_game_mode(uint32_t pressed, uint32_t position, uint32_t speed, uint16_t next_scroll_position, uint32_t object, uint32_t *p_speed_value, uint32_t *p_count, uint32_t *p_cycle, uint32_t *p_list_cursor)
{
	uint32_t speed_value = *p_speed_value;
	uint32_t count = *p_count;
	uint32_t cycle = *p_cycle;
	uint32_t list_cursor = *p_list_cursor;
	uint32_t r8 = 0, r9 = 0, r10 = 0;

	switch (game_mode)
	{
	case 0:
	case 33:
		game_mode_start_game(&r8, &r10);
		*p_speed_value = speed_value;
		*p_count = r8;
		*p_cycle = cycle;
		*p_list_cursor = r10;
		return;
	case 1:
		game_mode_intro(position, speed, object, &speed_value, &count, &cycle, &r10);
		*p_speed_value = speed_value;
		*p_count = count;
		*p_cycle = cycle;
		*p_list_cursor = r10;
		return;
	case 2:
		start_stage(next_scroll_position, &position, &speed, &speed_value, &r8, &r9, &r10, &object);
		*p_speed_value = speed_value;
		*p_count = r8;
		*p_cycle = r9;
		*p_list_cursor = r10;
		return;
	case 3:
	case 5:
	case 6:
		game_mode_play_stage(pressed, position, speed, next_scroll_position, object, &speed_value, &count, &cycle, &list_cursor);
		*p_speed_value = speed_value;
		*p_count = count;
		*p_cycle = cycle;
		*p_list_cursor = list_cursor;
		return;
	case 4:
		speed_value = game_mode_stage_end_fade(speed_value, object);
		*p_speed_value = speed_value;
		*p_count = count;
		*p_cycle = cycle;
		*p_list_cursor = list_cursor;
		return;
	case 7:
		game_mode_game_over(pressed, position, (uint16_t)speed, &speed_value, &count, &cycle, &r10);
		*p_speed_value = speed_value;
		*p_count = count;
		*p_cycle = cycle;
		*p_list_cursor = r10;
		return;
	case 8:
		game_mode_stage_select(&count, &r10);
		*p_speed_value = speed_value;
		*p_count = count;
		*p_cycle = cycle;
		*p_list_cursor = r10;
		return;
	case 9:
		game_mode_event_09(speed, object, &speed_value, &count, &cycle, &list_cursor);
		*p_speed_value = speed_value;
		*p_count = count;
		*p_cycle = cycle;
		*p_list_cursor = list_cursor;
		return;
	case 10:
	case 11:
		game_mode_event_0A(pressed, position, speed, next_scroll_position, object, &speed_value, &count, &cycle, &list_cursor);
		*p_speed_value = speed_value;
		*p_count = count;
		*p_cycle = cycle;
		*p_list_cursor = list_cursor;
		return;
	case 12:
		game_mode_event_0C(position, speed, object, &speed_value, &count, &cycle, &r10);
		*p_speed_value = speed_value;
		*p_count = count;
		*p_cycle = cycle;
		*p_list_cursor = r10;
		return;
	case 13:
		game_mode_event_0D(position, speed, object, &speed_value, &count, &cycle, &r10);
		*p_speed_value = speed_value;
		*p_count = count;
		*p_cycle = cycle;
		*p_list_cursor = r10;
		return;
	case 14:
		game_mode_continue(position, &speed_value, &count, &cycle, &r10);
		*p_speed_value = speed_value;
		*p_count = count;
		*p_cycle = cycle;
		*p_list_cursor = r10;
		return;
	case 15:
		game_mode_event_0F(position, speed, object, &speed_value, &count, &cycle, &r10);
		*p_speed_value = speed_value;
		*p_count = count;
		*p_cycle = cycle;
		*p_list_cursor = r10;
		return;
	case 16:
		game_mode_event_10(speed, object, &speed_value, &cycle, &r10);
		*p_speed_value = speed_value;
		*p_count = count;
		*p_cycle = cycle;
		*p_list_cursor = r10;
		return;
	case 17:
		game_mode_screen_wipe(position, speed, next_scroll_position, object, &speed_value, &count, &cycle, &r10);
		*p_speed_value = speed_value;
		*p_count = count;
		*p_cycle = cycle;
		*p_list_cursor = r10;
		return;
	case 18:
		game_mode_event_12(pressed, position, speed, next_scroll_position, object, &speed_value, &count, &cycle, &list_cursor);
		*p_speed_value = speed_value;
		*p_count = count;
		*p_cycle = cycle;
		*p_list_cursor = list_cursor;
		return;
	case 19:
		game_mode_event_13(pressed, position, speed, next_scroll_position, object, &speed_value, &count, &cycle, &list_cursor);
		*p_speed_value = speed_value;
		*p_count = count;
		*p_cycle = cycle;
		*p_list_cursor = list_cursor;
		return;
	case 20:
		game_mode_event_14(pressed, position, speed, next_scroll_position, object, &speed_value, &count, &cycle, &list_cursor);
		*p_speed_value = speed_value;
		*p_count = count;
		*p_cycle = cycle;
		*p_list_cursor = list_cursor;
		return;
	case 21:
		game_mode_event_15(pressed, position, speed, next_scroll_position, object, &speed_value, &count, &cycle, &list_cursor);
		*p_speed_value = speed_value;
		*p_count = count;
		*p_cycle = cycle;
		*p_list_cursor = list_cursor;
		return;
	case 22:
		game_mode_event_16(pressed, position, speed, next_scroll_position, object, &speed_value, &count, &cycle, &list_cursor);
		*p_speed_value = speed_value;
		*p_count = count;
		*p_cycle = cycle;
		*p_list_cursor = list_cursor;
		return;
	case 23:
		game_mode_stage_clear(position, speed, object, &speed_value, &count, &cycle, &r10);
		*p_speed_value = speed_value;
		*p_count = count;
		*p_cycle = cycle;
		*p_list_cursor = r10;
		return;
	case 24:
		game_mode_event_18(position, speed, object, &speed_value, &count, &cycle, &r10);
		*p_speed_value = speed_value;
		*p_count = count;
		*p_cycle = cycle;
		*p_list_cursor = r10;
		return;
	case 25:
		game_mode_event_19(pressed, position, speed, next_scroll_position, object, &speed_value, &count, &cycle, &list_cursor);
		*p_speed_value = speed_value;
		*p_count = count;
		*p_cycle = cycle;
		*p_list_cursor = list_cursor;
		return;
	case 26:
		game_mode_event_1A(pressed, position, speed, next_scroll_position, object, &speed_value, &count, &cycle, &list_cursor);
		*p_speed_value = speed_value;
		*p_count = count;
		*p_cycle = cycle;
		*p_list_cursor = list_cursor;
		return;
	case 27:
		game_mode_character_select(position, speed, object, &speed_value, &count, &cycle, &r10);
		*p_speed_value = speed_value;
		*p_count = count;
		*p_cycle = cycle;
		*p_list_cursor = r10;
		return;
	case 28:
		game_mode_event_1C(pressed, position, speed, next_scroll_position, object, &speed_value, &count, &cycle, &list_cursor);
		*p_speed_value = speed_value;
		*p_count = count;
		*p_cycle = cycle;
		*p_list_cursor = list_cursor;
		return;
	case 29:
		game_mode_stage_results(position, speed, object, &speed_value, &count, &cycle, &r10);
		*p_speed_value = speed_value;
		*p_count = count;
		*p_cycle = cycle;
		*p_list_cursor = r10;
		return;
	case 30:
		game_mode_stage_change_fade(pressed, position, speed, next_scroll_position, object, &speed_value, &count, &cycle, &list_cursor);
		*p_speed_value = speed_value;
		*p_count = count;
		*p_cycle = cycle;
		*p_list_cursor = list_cursor;
		return;
	case 31:
		game_mode_event_1F(pressed, position, speed, next_scroll_position, object, &speed_value, &count, &cycle, &list_cursor);
		*p_speed_value = speed_value;
		*p_count = count;
		*p_cycle = cycle;
		*p_list_cursor = list_cursor;
		return;
	case 32:
		game_mode_you_failed(pressed, position, speed, object, &speed_value, &count, &cycle, &r10);
		*p_speed_value = speed_value;
		*p_count = count;
		*p_cycle = cycle;
		*p_list_cursor = r10;
		return;
	default:
		invalid_jump(0x65Fu + game_mode * 3u);
	}
}

void game_mode_start_game(uint32_t *p_count, uint32_t *p_list_cursor)
{
	uint8_t mode = 0, next_game_mode = 0;
	uint32_t control_register = 0, count = 0, handler_cursor = 0, ignored = 0;
	uint8_t new_game_cnt_bits = 0;

	display_off_frames = 2u;
	new_game_cnt_bits = mem_read8(io_cnt);
	mem_write8(io_cnt, new_game_cnt_bits & ~2u);
	clear_text_layer(&ignored, &ignored, &ignored);
	clear_page_0_and_scroll();
	if ((int8_t)game_in_progress < 0)
	{
		mem_write16(cleared_stages_ADDRESS, 0u);
		current_screen = 6u;
		game_frames = 0u;
	}
	else
		stage = ROM16(demo_input);
	stage_start_kind = 0u;
	player_status[0] = 0u;
	player_2_status = 0u;
	player_3_status = 0u;
	player_status_spare = 0xFFu;
	stage_init_flags = 0u;
	intro_character = -1;
	players_in_play = 0;
	mode = 0u;
	control_register = set_screen_mode(mode);
	count = clear_player_blocks();
	handler_cursor = start_player_objects(control_register);
	next_game_mode = 0x1Bu;
	if ((int8_t)game_in_progress < 0)
	{
		game_mode = next_game_mode;
		current_game_mode = 0u;
		reset_linked_objects_flag();
		*p_count = count;
		*p_list_cursor = handler_cursor;
		return;
	}
	next_game_mode = 2u;
	stage_start_kind = 0xFFu;
	game_mode = next_game_mode;
	current_game_mode = 0u;
	reset_linked_objects_flag();
	*p_count = count;
	*p_list_cursor = handler_cursor;
}

void start_stage(uint32_t stage_offset, uint32_t *p_write_index_address, uint32_t *p_floor_block, uint32_t *p_bounds_offset, uint32_t *p_corners_left, uint32_t *p_palette_record, uint32_t *p_source, uint32_t *p_object)
{
	uint32_t write_index_address = *p_write_index_address;
	uint32_t floor_block = *p_floor_block;
	uint32_t bounds_offset = *p_bounds_offset;
	uint32_t object = *p_object;
	uint8_t cabinet_event = 0, sound_id = 0, stages_past_2 = 0;
	uint32_t corners_left = 0, ignored = 0, palette_dest = 0, palette_record = 0, source = 0;
	uint16_t cleared_count = 0;

	if (stage_start_kind == 0u)
	{
		mem_write16(cleared_stages_ADDRESS, cleared_stages + 1u);
		if ((int8_t)(uint8_t)cleared_stages > 0xE)
			mem_write8(cleared_stages_ADDRESS, 0xEu);
	}
	if ((int8_t)stage_start_kind >= 0)
		floor_block = start_player_objects_alt(object);
	hitter_list = 0xFFFFu;
	touchable_list = 0xFFFFu;
	platform_list = 0xFFFFu;
	rider_list = 0xFFFFu;
	wall_list = 0xFFFFu;
	zone_list = 0xFFFFu;
	skip_player_summary = 0xFFu;
	unused_game_byte = 0u;
	camera_ground_speed_vx = 0u;
	camera_ground_speed_vy = 0u;
	camera_slope_speed_vx = 0u;
	camera_slope_speed_vy = 0u;
	scroll_drift_vx = 0u;
	scroll_drift_vy = 0u;
	camera_slope_speed = 0u;
	camera_slope_direction = 0u;
	scroll_drift_speed = 0u;
	scroll_drift_direction = 0u;
	camera_scroll_direction = 0u;
	unused_camera_speed_word = 0u;
	player_dropping_in = 0u;
	player_dropping_in_before = 0u;
	grey_screen_active = 0u;
	grey_screen_ended = 0u;
	player_mishap = 0u;
	all_players_path_request = 0u;
	players_finished[0] = 0u;
	players_finished[1] = 0u;
	players_finished[2] = 0u;
	goal_unlocked = 0u;
	unused_player_flags[0] = 0u;
	unused_player_flags[1] = 0u;
	unused_player_flags[2] = 0u;
	unused_player_flags[3] = 0u;
	unused_stage_byte = 0u;
	map_screen_active = 0u;
	per_player_view_mode = 0u;
	players_pulled = 0u;
	handle_rod_drop_players = 0u;
	stage_exit_started = 0u;
	chaser_object = 0u;
	scroll_resume_request = 0u;
	demo_prompt_shown = 0u;
	event_0d_active = 0u;
	players_frozen = 0u;
	rope_bridge_collapsing = 0u;
	barred_gate_closing = 0u;
	event_1c_riding = 0u;
	event_1c_ride_phase = 0u;
	ending_finished = 0u;
	event_12_fast_scroll = 0u;
	group_leader_0_held = 0u;
	group_leaders_1_3_held = 0u;
	sand_block_leader_active = 0u;
	chaser_stopped = 0u;
	sliding_door_opened = 0u;
	stage_b_sound_done = 0u;
	spiked_press_object = 0u;
	icicle_dropper_ready = 0xFFu;
	apply_stage_layer_blending(&write_index_address, &corners_left);
	start_stage_view(&write_index_address, &floor_block, &bounds_offset, &ignored);
	if ((int8_t)game_in_progress >= 0)
	{
		stage_start_section = ROM16(demo_input + 2u);
		move_camera_to_start_section(&write_index_address, &floor_block, &ignored);
		demo_input += 0x10u;
	}
	load_stage_map_pointers(&ignored, &ignored);
	load_stage_goal();
	find_nearby_floor_blocks(&ignored, &floor_block, &bounds_offset, &ignored, &corners_left, &ignored, &ignored);
	start_spawns_by_type();
	sound_id = stage_start_sounds[stage];
	play_sound(sound_id, &ignored, &write_index_address);
	game_mode = 3u;
	current_game_mode = 2u;
	stage_start_kind = 0u;
	if (game_in_progress == 0u)
	{
		load_stage_palette(&palette_record, &source, &palette_dest);
		*p_write_index_address = write_index_address;
		*p_floor_block = floor_block;
		*p_bounds_offset = bounds_offset;
		*p_corners_left = corners_left;
		*p_palette_record = palette_record;
		*p_source = source;
		*p_object = palette_dest;
		return;
	}
	cleared_count = cleared_stages;
	stages_past_2 = (uint8_t)(uint16_t)(cleared_count - 2u);
	if ((int16_t)cleared_count < 2)
		stages_past_2 = 0u;
	cabinet_event = 0x14u;
	if ((stages_past_2 & 1u) == 0u)
		cabinet_event = 0x15u;
	mem_write8(cabinet_events, cabinet_event);
	load_stage_palette(&palette_record, &source, &palette_dest);
	*p_write_index_address = write_index_address;
	*p_floor_block = floor_block;
	*p_bounds_offset = bounds_offset;
	*p_corners_left = corners_left;
	*p_palette_record = palette_record;
	*p_source = source;
	*p_object = palette_dest;
}

void game_mode_play_stage(uint32_t pressed, uint32_t position, uint32_t in_play_count, uint32_t next_scroll_position, uint32_t object, uint32_t *p_speed_value, uint32_t *p_count, uint32_t *p_cycle, uint32_t *p_list_cursor)
{
	uint32_t speed_value = *p_speed_value;
	uint32_t count = *p_count;
	uint32_t cycle = *p_cycle;
	uint32_t list_cursor = *p_list_cursor;
	uint32_t box_edge = 0, event = 0, ignored = 0, speed = 0;

	current_game_mode = 3u;
	next_stage_slot_request = 0u;
	update_player_summary(&in_play_count, &count, &list_cursor, &object);
	if (game_in_progress != 0u && joining_closed == 0u && stage == 0xBu)
	{
		joining_closed = 0xFFu;
		player_1_hud_message_shown = 0u;
		player_2_hud_message_shown = 0u;
		player_3_hud_message_shown = 0u;
	}
	if ((player_status[0] | player_2_status | player_3_status | player_status_spare) == 0u)
	{
		saved_game_mode = game_mode;
		game_mode = 7u;
		game_mode_timer = 0;
		run_game_mode(pressed, position, in_play_count, (uint16_t)next_scroll_position, object, &speed_value, &count, &cycle, &list_cursor);
		*p_speed_value = speed_value;
		*p_count = count;
		*p_cycle = cycle;
		*p_list_cursor = list_cursor;
		return;
	}
	check_stage_clear(&count, &list_cursor, &object);
	if (game_mode != 3u)
	{
		run_game_mode(pressed, position, in_play_count, (uint16_t)next_scroll_position, object, &speed_value, &count, &cycle, &list_cursor);
		*p_speed_value = speed_value;
		*p_count = count;
		*p_cycle = cycle;
		*p_list_cursor = list_cursor;
		return;
	}
	speed = update_grey_screen();
	update_scroll(&position, &speed, &speed_value, &next_scroll_position, &count, &cycle, &ignored, &object);
	update_stage_tile_layers(&box_edge, &position, &speed, &speed_value, &next_scroll_position, &count, &cycle, &ignored, &object);
	find_nearby_floor_blocks(&box_edge, &speed, &speed_value, &next_scroll_position, &count, &ignored, &object);
	continue_to_next_stage_slot(&box_edge, &ignored, &object);
	speed_value = update_restart_point(speed_value);
	spawn_objects_by_type(&box_edge, &position, &speed, &speed_value, &next_scroll_position, &count, &object);
	spawn_ring_rows(&box_edge, &position, &speed, &object);
	spawn_walls(&box_edge, &position, &next_scroll_position, &count, &cycle, &object);
	spawn_object_groups_2(&box_edge, &count, &object);
	spawn_object_groups(&box_edge, &position, &speed, &count, &object);
	spawn_objects_by_handler(&box_edge, &ignored, &object);
	box_edge = check_zones(box_edge);
	check_hits(&box_edge, &position, &speed, &speed_value, &next_scroll_position, &count, &object);
	box_edge = check_walls(box_edge);
	check_platforms(&box_edge, &position, &speed, &speed_value, &next_scroll_position, &count, &cycle, &ignored, &object);
	run_stage_events(&box_edge, &event);
	if (game_mode != 3u)
	{
		run_game_mode(box_edge, position, speed, (uint16_t)next_scroll_position, object, &speed_value, &count, &cycle, &event);
		*p_speed_value = speed_value;
		*p_count = count;
		*p_cycle = cycle;
		*p_list_cursor = event;
		return;
	}
	*p_speed_value = speed_value;
	*p_count = count;
	*p_cycle = cycle;
	*p_list_cursor = event;
}

void game_mode_intro(uint32_t position, uint32_t speed, uint32_t object, uint32_t *p_speed_value, uint32_t *p_count, uint32_t *p_cycle, uint32_t *p_list_cursor)
{
	uint32_t speed_value = *p_speed_value;
	uint32_t count = *p_count;
	uint32_t cycle = *p_cycle;
	uint8_t sound_id = 0;
	uint32_t ignored = 0, intro_status_cursor = 0;
	uint8_t p2_status = 0, p3_status = 0;
	uint32_t intro_init_flags = 0;

	intro_init_flags = stage_init_flags;
	stage_init_flags = intro_init_flags | 0x2000u;
	if ((intro_init_flags & 0x2000u) == 0u)
	{
		hitter_list = 0xFFFFu;
		touchable_list = 0xFFFFu;
		platform_list = 0xFFFFu;
		rider_list = 0xFFFFu;
		wall_list = 0xFFFFu;
		zone_list = 0xFFFFu;
		unused_game_byte = 0u;
		camera_ground_speed_vx = 0u;
		camera_ground_speed_vy = 0u;
		camera_slope_speed_vx = 0u;
		camera_slope_speed_vy = 0u;
		camera_slope_speed = 0u;
		camera_slope_direction = 0u;
		camera_scroll_direction = 0u;
		unused_camera_speed_word = 0u;
		player_dropping_in = 0u;
		player_dropping_in_before = 0u;
		grey_screen_active = 0u;
		grey_screen_ended = 0u;
		player_mishap = 0u;
		all_players_path_request = 0u;
		game_mode_timer = 0;
		players_pulled_word = 0u;
		stage_exit_started = 0u;
		stage_06_shake = 0u;
		start_stage_view(&ignored, &ignored, &speed_value, &ignored);
		stage_start_section = 6u;
		move_camera_to_start_section(&position, &speed, &ignored);
		load_stage_map_pointers(&ignored, &ignored);
		load_stage_goal();
		find_nearby_floor_blocks(&ignored, &speed, &speed_value, &ignored, &count, &ignored, &ignored);
		start_spawns_by_type();
		load_stage_palette(&cycle, &ignored, &object);
		current_game_mode = 1u;
		sound_id = stage_start_sounds[stage];
		play_sound(sound_id, &ignored, &position);
	}
	update_scroll(&position, &speed, &speed_value, &ignored, &count, &cycle, &ignored, &object);
	update_stage_tile_layers(&ignored, &position, &speed, &speed_value, &ignored, &count, &cycle, &ignored, &object);
	find_nearby_floor_blocks(&ignored, &speed, &speed_value, &ignored, &count, &ignored, &object);
	speed_value = update_restart_point(speed_value);
	spawn_objects_by_type(&ignored, &position, &speed, &speed_value, &ignored, &count, &object);
	spawn_objects_by_handler(&ignored, &ignored, &object);
	check_hits(&ignored, &position, &ignored, &speed_value, &ignored, &count, &ignored);
	check_walls(ignored);
	check_platforms(&ignored, &ignored, &ignored, &speed_value, &ignored, &count, &cycle, &ignored, &ignored);
	intro_status_cursor = player_2_status_ADDRESS;
	if (player_status[0] != 0xFFu || (inputs_pressed & 0x10000u) == 0u)
	{
		p2_status = player_2_status;
		intro_status_cursor++;
		if (p2_status != 0xFFu || (inputs_pressed & 0x1000000u) == 0u)
		{
			p3_status = player_3_status;
			intro_status_cursor++;
			if (p3_status != 0xFFu || (inputs_pressed & 0x40000u) == 0u)
			{
				game_mode_timer++;
				if (game_mode_timer < 0x21C)
				{
					*p_speed_value = speed_value;
					*p_count = count;
					*p_cycle = cycle;
					*p_list_cursor = intro_status_cursor;
					return;
				}
			}
		}
	}
	game_mode = 2u;
	system_mode_step = 0u;
	*p_speed_value = speed_value;
	*p_count = count;
	*p_cycle = cycle;
	*p_list_cursor = intro_status_cursor;
}

void game_mode_continue(uint32_t position, uint32_t *p_speed_value, uint32_t *p_count, uint32_t *p_cycle, uint32_t *p_list_cursor)
{
	uint32_t speed_value = *p_speed_value;
	uint32_t count = *p_count;
	uint32_t cycle = *p_cycle;
	uint16_t attribute = 0, cell = 0;
	uint32_t count_text = 0, ignored = 0, platform = 0, text = 0, text_position = 0, value = 0;
	uint8_t old_continue_count = 0;

	if (continue_count == 9u)
	{
		text = continue_text_ADDRESS;
		print_text(&text, &ignored);
	}
	current_game_mode = 0xEu;
	ignored = check_zones(0u);
	check_hits(&ignored, &position, &ignored, &speed_value, &ignored, &count, &ignored);
	check_walls(ignored);
	check_platforms(&ignored, &ignored, &ignored, &speed_value, &ignored, &count, &cycle, &platform, &ignored);
	if ((inputs_pressed & 0x10000u) != 0u || (inputs_pressed & 0x1000000u) != 0u || (inputs_pressed & 0x40000u) != 0u)
		continue_second_frames = 0x3C;
	continue_second_frames++;
	if (continue_second_frames >= 0x3C)
	{
		continue_second_frames = 0;
		old_continue_count = continue_count;
		continue_count = old_continue_count - 1u;
		if ((int8_t)old_continue_count <= 1)
		{
			game_mode = 7u;
			game_mode_timer = 0;
			text = continue_text_ADDRESS;
			erase_text(text);
			cell = 0u;
			count_text = 0x25C4u;
			text_position = TEXT_CELL(24, 14);
			erase_text_here(cell, &count_text, &text_position);
			*p_speed_value = (*p_speed_value & 0xFFFF0000u) | (speed_value & 0xFFFFu);
			*p_count = count;
			*p_cycle = cycle;
			*p_list_cursor = count_text;
			return;
		}
	}
	value = continue_count;
	attribute = 0u;
	text_position = TEXT_CELL(24, 14);
	draw_hex_2(value, attribute, text_position);
	switch (cabinet_config)
	{
	case 0:
	case 1:
	case 4:
		if (p1_credits == 0u || ((inputs_pressed & 0x10u) == 0u && (inputs_pressed & 0x20u) == 0u && (inputs_pressed & 0x80u) == 0u))
		{
			*p_speed_value = (*p_speed_value & 0xFFFF0000u) | (speed_value & 0xFFFFu);
			*p_count = count;
			*p_cycle = cycle;
			*p_list_cursor = platform;
			return;
		}
		game_mode = saved_game_mode;
		player_status[0] = 0u;
		player_2_status = 0u;
		player_3_status = 0u;
		player_status_spare = 0xFFu;
		hitter_list = 0xFFFFu;
		touchable_list = 0xFFFFu;
		wall_list = 0xFFFFu;
		rider_list = 0xFFFFu;
		platform_list = 0xFFFFu;
		zone_list = 0xFFFFu;
		text = continue_text_ADDRESS;
		erase_text(text);
		cell = 0u;
		count_text = 0x25C4u;
		text_position = TEXT_CELL(24, 14);
		erase_text_here(cell, &count_text, &text_position);
		*p_speed_value = (*p_speed_value & 0xFFFF0000u) | (speed_value & 0xFFFFu);
		*p_count = count;
		*p_cycle = cycle;
		*p_list_cursor = count_text;
		return;
	case 2:
		if ((p2_credits == 0u || (inputs_pressed & 0x20u) == 0u) && (p1_credits == 0u || (inputs_pressed & 0x10u) == 0u))
		{
			*p_speed_value = (*p_speed_value & 0xFFFF0000u) | (speed_value & 0xFFFFu);
			*p_count = count;
			*p_cycle = cycle;
			*p_list_cursor = platform;
			return;
		}
		game_mode = saved_game_mode;
		player_status[0] = 0u;
		player_2_status = 0u;
		player_3_status = 0u;
		player_status_spare = 0xFFu;
		hitter_list = 0xFFFFu;
		touchable_list = 0xFFFFu;
		wall_list = 0xFFFFu;
		rider_list = 0xFFFFu;
		platform_list = 0xFFFFu;
		zone_list = 0xFFFFu;
		text = continue_text_ADDRESS;
		erase_text(text);
		cell = 0u;
		count_text = 0x25C4u;
		text_position = TEXT_CELL(24, 14);
		erase_text_here(cell, &count_text, &text_position);
		*p_speed_value = (*p_speed_value & 0xFFFF0000u) | (speed_value & 0xFFFFu);
		*p_count = count;
		*p_cycle = cycle;
		*p_list_cursor = count_text;
		return;
	case 3:
		if (p3_credits == 0u || (inputs_pressed & 0x80u) == 0u)
		{
			if ((p2_credits == 0u || (inputs_pressed & 0x20u) == 0u) && (p1_credits == 0u || (inputs_pressed & 0x10u) == 0u))
			{
				*p_speed_value = (*p_speed_value & 0xFFFF0000u) | (speed_value & 0xFFFFu);
				*p_count = count;
				*p_cycle = cycle;
				*p_list_cursor = platform;
				return;
			}
			game_mode = saved_game_mode;
			player_status[0] = 0u;
			player_2_status = 0u;
			player_3_status = 0u;
			player_status_spare = 0xFFu;
			hitter_list = 0xFFFFu;
			touchable_list = 0xFFFFu;
			wall_list = 0xFFFFu;
			rider_list = 0xFFFFu;
			platform_list = 0xFFFFu;
			zone_list = 0xFFFFu;
			text = continue_text_ADDRESS;
			erase_text(text);
			cell = 0u;
			count_text = 0x25C4u;
			text_position = TEXT_CELL(24, 14);
			erase_text_here(cell, &count_text, &text_position);
			*p_speed_value = (*p_speed_value & 0xFFFF0000u) | (speed_value & 0xFFFFu);
			*p_count = count;
			*p_cycle = cycle;
			*p_list_cursor = count_text;
			return;
		}
		game_mode = saved_game_mode;
		player_status[0] = 0u;
		player_2_status = 0u;
		player_3_status = 0u;
		player_status_spare = 0xFFu;
		hitter_list = 0xFFFFu;
		touchable_list = 0xFFFFu;
		wall_list = 0xFFFFu;
		rider_list = 0xFFFFu;
		platform_list = 0xFFFFu;
		zone_list = 0xFFFFu;
		text = continue_text_ADDRESS;
		erase_text(text);
		cell = 0u;
		count_text = 0x25C4u;
		text_position = TEXT_CELL(24, 14);
		erase_text_here(cell, &count_text, &text_position);
		*p_speed_value = (*p_speed_value & 0xFFFF0000u) | (speed_value & 0xFFFFu);
		*p_count = count;
		*p_cycle = cycle;
		*p_list_cursor = count_text;
		return;
	default:
		invalid_jump(0xBDCu + cabinet_config * 2u);
	}
}

uint32_t game_mode_stage_end_fade(uint32_t bottom_speed, uint32_t object)
{
	uint8_t sound_id = 0;
	uint32_t ignored = 0;

	if (current_game_mode != 4u)
	{
		sound_id = 0x95u;
		play_sound(sound_id, &ignored, &ignored);
		sound_id = 3u;
		play_sound(sound_id, &ignored, &ignored);
	}
	current_game_mode = 4u;
	mixer_blend &= 0xF7FFu;
	bottom_speed = run_map_screen(bottom_speed, object);
	map_screen_active = 0xFFu;
	if (map_screen_step == 5u)
		game_mode = 2u;
	return bottom_speed;
}

void game_mode_screen_wipe(uint32_t position, uint32_t speed, uint32_t edges_done, uint32_t object, uint32_t *p_speed_value, uint32_t *p_count, uint32_t *p_cycle, uint32_t *p_list_cursor)
{
	uint32_t speed_value = *p_speed_value;
	uint32_t count = *p_count;
	uint32_t cycle = *p_cycle;
	uint32_t ignored = 0, platform = 0;

	current_game_mode = 0x11u;
	if (stage == 4u)
		scroll_mode = 1u;
	update_scroll(&position, &speed, &speed_value, &edges_done, &count, &cycle, &ignored, &object);
	update_stage_tile_layers(&ignored, &position, &speed, &speed_value, &edges_done, &count, &cycle, &ignored, &object);
	find_nearby_floor_blocks(&ignored, &speed, &speed_value, &edges_done, &count, &ignored, &object);
	speed_value = update_restart_point(speed_value);
	spawn_objects_by_type(&ignored, &position, &speed, &speed_value, &edges_done, &count, &object);
	spawn_ring_rows(&ignored, &position, &ignored, &object);
	spawn_walls(&ignored, &position, &edges_done, &count, &cycle, &object);
	spawn_object_groups_2(&ignored, &count, &object);
	spawn_object_groups(&ignored, &position, &ignored, &count, &object);
	spawn_objects_by_handler(&ignored, &ignored, &object);
	ignored = check_zones(ignored);
	check_hits(&ignored, &position, &ignored, &speed_value, &edges_done, &count, &ignored);
	check_walls(ignored);
	check_platforms(&ignored, &ignored, &ignored, &speed_value, &edges_done, &count, &cycle, &platform, &ignored);
	update_screen_wipe(&ignored, &ignored, &ignored, &edges_done);
	if ((edges_done & 0xFu) != 0xFu)
	{
		*p_speed_value = speed_value;
		*p_count = count;
		*p_cycle = cycle;
		*p_list_cursor = platform;
		return;
	}
	game_mode = 3u;
	*p_speed_value = speed_value;
	*p_count = count;
	*p_cycle = cycle;
	*p_list_cursor = platform;
}

void game_mode_stage_clear(uint32_t position, uint32_t speed, uint32_t object, uint32_t *p_speed_value, uint32_t *p_count, uint32_t *p_cycle, uint32_t *p_list_cursor)
{
	uint32_t speed_value = *p_speed_value;
	uint32_t count = *p_count;
	uint32_t cycle = *p_cycle;
	uint8_t sound_id = 0;
	uint32_t banner_record = 0, free_count = 0, ignored = 0, platform = 0;
	uint16_t old_clear_timer = 0;
	uint32_t stage_clear_init_flags = 0;

	stage_clear_init_flags = stage_init_flags;
	stage_init_flags = stage_clear_init_flags | 0x4000u;
	if ((stage_clear_init_flags & 0x4000u) == 0u)
	{
		stage_name_shown = 0u;
		cleared_shown = 0u;
		banner_break = 0u;
		stage_clear_timer = 0x14Au;
		stage_exit_started = 0u;
		allocate_object_outside_loop(&free_count, &object);
		if (free_count != 0u)
		{
			OBJECT(object)->sprite_flags = 0x1002u;
			OBJECT(object)->sprite_palette = 0x89A0u;
			OBJECT(object)->handler = stage_clear_banner_reveal_ADDRESS;
			OBJECT(object)->flags = 0u;
			OBJECT(object)->stage_clear_banner.sparkle_offset = 0x10u;
			banner_record = stage_name_banners[(uint8_t)stage] + stage_name_banners_ADDRESS;
			OBJECT(object)->screen_x = ROM16(banner_record);
			banner_record += 2u;
			OBJECT(object)->screen_y = ROM16(banner_record);
			OBJECT(object)->animation = banner_record + 2u;
			OBJECT(object)->animation_frame = 0xFFu;
			OBJECT(object)->animation_speed = 4u;
			OBJECT(object)->animation_timer = 4;
			allocate_object_outside_loop(&free_count, &object);
			if (free_count != 0u)
			{
				OBJECT(object)->sprite_flags = 0x1002u;
				OBJECT(object)->sprite_palette = 0x89A0u;
				OBJECT(object)->handler = cleared_banner_wait_ADDRESS;
				OBJECT(object)->flags = 1u;
				OBJECT(object)->var_80 = 0x10u;
				OBJECT(object)->var_81 = 0u;
				OBJECT(object)->screen_x = cleared_banner_x;
				OBJECT(object)->screen_y = cleared_banner_y;
				OBJECT(object)->animation = cleared_banner_animation_ADDRESS;
				OBJECT(object)->animation_frame = 0xFFu;
				OBJECT(object)->animation_speed = 4u;
				OBJECT(object)->animation_timer = 4;
				sound_id = 0x94u;
				play_sound(sound_id, &ignored, &position);
				sound_id = 0x7Bu;
				play_sound(sound_id, &ignored, &position);
			}
		}
	}
	if ((int16_t)stage_clear_timer <= 0x78)
		banner_break = 0xFFu;
	current_game_mode = 0x17u;
	next_stage_slot_request = 0u;
	update_scroll(&position, &speed, &speed_value, &ignored, &count, &cycle, &ignored, &object);
	update_stage_tile_layers(&ignored, &position, &speed, &speed_value, &ignored, &count, &cycle, &ignored, &object);
	find_nearby_floor_blocks(&ignored, &speed, &speed_value, &ignored, &count, &ignored, &object);
	speed_value = update_restart_point(speed_value);
	spawn_objects_by_type(&ignored, &position, &speed, &speed_value, &ignored, &count, &object);
	spawn_ring_rows(&ignored, &position, &ignored, &object);
	spawn_walls(&ignored, &position, &ignored, &count, &cycle, &object);
	spawn_object_groups_2(&ignored, &count, &object);
	spawn_object_groups(&ignored, &position, &ignored, &count, &object);
	spawn_objects_by_handler(&ignored, &ignored, &object);
	ignored = check_zones(ignored);
	check_hits(&ignored, &position, &ignored, &speed_value, &ignored, &count, &ignored);
	check_walls(ignored);
	check_platforms(&ignored, &ignored, &ignored, &speed_value, &ignored, &count, &cycle, &platform, &ignored);
	old_clear_timer = stage_clear_timer;
	stage_clear_timer = old_clear_timer - 1u;
	if ((int16_t)old_clear_timer > 1)
	{
		*p_speed_value = speed_value;
		*p_count = count;
		*p_cycle = cycle;
		*p_list_cursor = platform;
		return;
	}
	game_mode = 0x1Du;
	*p_speed_value = speed_value;
	*p_count = count;
	*p_cycle = cycle;
	*p_list_cursor = platform;
}

void game_mode_game_over(uint32_t box_edge, uint32_t position, uint16_t scale, uint32_t *p_speed_value, uint32_t *p_count, uint32_t *p_cycle, uint32_t *p_list_cursor)
{
	uint32_t speed_value = *p_speed_value;
	uint32_t count = *p_count;
	uint32_t cycle = *p_cycle;
	uint8_t credits_needed = 0, sound_id = 0;
	uint32_t handler_cursor = 0, ignored = 0, platform = 0, record = 0, rounds_left = 0;
	uint8_t game_over_cnt_bits = 0;

	if ((uint16_t)game_mode_timer == 0u)
	{
		sound_id = 0x94u;
		queue_sound(sound_id, &ignored, &ignored);
		sound_id = 0x73u;
		if (stage == 0x11u)
			sound_id = 0x76u;
		queue_sound(sound_id, &box_edge, &position);
		joining_closed = 0u;
		character_select_timer = 0;
		mem_write8(cabinet_events, 0x1Fu);
	}
	current_game_mode = 7u;
	if (ending_finished == 0u)
		draw_game_over_banner();
	else
	{
		record = game_over_string_ADDRESS;
		queue_string_record(record, (uint16_t)box_edge, scale, &position, &speed_value);
	}
	box_edge = check_zones(box_edge);
	check_hits(&box_edge, &position, &ignored, &speed_value, &ignored, &count, &ignored);
	check_walls(box_edge);
	check_platforms(&ignored, &ignored, &ignored, &speed_value, &ignored, &count, &cycle, &platform, &ignored);
	game_mode_timer++;
	if (game_mode_timer >= 0x78)
	{
		game_in_progress = 0u;
		attract_step = 0u;
		demo_prompt_shown = 0u;
		stage_init_flags &= ~0x8000u;
		advance_attract_screen();
		display_off_frames = 2u;
		game_over_cnt_bits = mem_read8(io_cnt);
		mem_write8(io_cnt, game_over_cnt_bits & ~2u);
		mixer_text_blend &= 0xBFFFu;
		mixer_nbg0_blend &= 0xBFFFu;
		mixer_nbg1_blend &= 0xBFFFu;
		mixer_nbg2_blend &= 0xBFFFu;
		mixer_nbg3_blend &= 0xBFFFu;
		mixer_sprite_control &= ~0x8000u;
		color_offset_red = 0u;
		color_offset_green = 0u;
		color_offset_blue = 0u;
	}
	credits_needed = (uint8_t)credits_per_start;
	if (free_play != 0u)
		credits_needed = 0u;
	switch (cabinet_config)
	{
	case 0:
	case 1:
	case 4:
		if (p1_credits < (uint8_t)credits_needed || ((inputs_pressed & 0x10u) == 0u && (inputs_pressed & 0x20u) == 0u && (inputs_pressed & 0x80u) == 0u))
		{
			*p_speed_value = speed_value;
			*p_count = count;
			*p_cycle = cycle;
			*p_list_cursor = platform;
			return;
		}
		system_mode = 6u;
		game_mode = 0u;
		system_mode_step = 0u;
		game_mode_start_game(&rounds_left, &handler_cursor);
		*p_speed_value = speed_value;
		*p_count = rounds_left;
		*p_cycle = cycle;
		*p_list_cursor = handler_cursor;
		return;
	case 2:
		if ((p2_credits < (uint8_t)credits_needed || (inputs_pressed & 0x20u) == 0u) && (p1_credits < (uint8_t)credits_needed || (inputs_pressed & 0x10u) == 0u))
		{
			*p_speed_value = speed_value;
			*p_count = count;
			*p_cycle = cycle;
			*p_list_cursor = platform;
			return;
		}
		system_mode = 6u;
		game_mode = 0u;
		system_mode_step = 0u;
		game_mode_start_game(&rounds_left, &handler_cursor);
		*p_speed_value = speed_value;
		*p_count = rounds_left;
		*p_cycle = cycle;
		*p_list_cursor = handler_cursor;
		return;
	case 3:
		if ((uint8_t)credits_needed <= p3_credits && (inputs_pressed & 0x80u) != 0u)
		{
			system_mode = 6u;
			game_mode = 0u;
			system_mode_step = 0u;
			game_mode_start_game(&rounds_left, &handler_cursor);
			*p_speed_value = speed_value;
			*p_count = rounds_left;
			*p_cycle = cycle;
			*p_list_cursor = handler_cursor;
			return;
		}
		if ((p2_credits < (uint8_t)credits_needed || (inputs_pressed & 0x20u) == 0u) && (p1_credits < (uint8_t)credits_needed || (inputs_pressed & 0x10u) == 0u))
		{
			*p_speed_value = speed_value;
			*p_count = count;
			*p_cycle = cycle;
			*p_list_cursor = platform;
			return;
		}
		system_mode = 6u;
		game_mode = 0u;
		system_mode_step = 0u;
		game_mode_start_game(&rounds_left, &handler_cursor);
		*p_speed_value = speed_value;
		*p_count = rounds_left;
		*p_cycle = cycle;
		*p_list_cursor = handler_cursor;
		return;
	default:
		invalid_jump(0xF8Fu + cabinet_config * 2u);
	}
}

void game_mode_character_select(uint32_t position, uint32_t speed, uint32_t object, uint32_t *p_speed_value, uint32_t *p_count, uint32_t *p_cycle, uint32_t *p_list_cursor)
{
	uint32_t speed_value = *p_speed_value;
	uint32_t count = *p_count;
	uint32_t cycle = *p_cycle;
	uint32_t ignored = 0, platform = 0;
	uint32_t character_select_init_flags = 0;

	character_select_init_flags = stage_init_flags;
	stage_init_flags = character_select_init_flags | 0x10000u;
	if ((character_select_init_flags & 0x10000u) == 0u)
	{
		hitter_list = 0xFFFFu;
		touchable_list = 0xFFFFu;
		platform_list = 0xFFFFu;
		rider_list = 0xFFFFu;
		wall_list = 0xFFFFu;
		zone_list = 0xFFFFu;
		unused_game_byte = 0u;
		camera_ground_speed_vx = 0u;
		camera_ground_speed_vy = 0u;
		camera_slope_speed_vx = 0u;
		camera_slope_speed_vy = 0u;
		camera_slope_speed = 0u;
		camera_slope_direction = 0u;
		camera_scroll_direction = 0u;
		unused_camera_speed_word = 0u;
		player_dropping_in = 0u;
		player_dropping_in_before = 0u;
		grey_screen_active = 0u;
		grey_screen_ended = 0u;
		player_mishap = 0u;
		all_players_path_request = 0u;
		game_mode_timer = 0;
		players_pulled_word = 0u;
		stage_exit_started = 0u;
		stage_06_shake = 0u;
		nearby_floor_block_count = 0u;
		characters_taken = 0u;
		start_stage_view(&position, &speed, &speed_value, &ignored);
		load_stage_map_pointers(&ignored, &ignored);
		load_stage_goal();
		start_spawns_by_type();
		load_stage_palette(&cycle, &ignored, &object);
		current_game_mode = 0x1Bu;
		mem_write8(cabinet_events, 0x15u);
	}
	speed_value = update_restart_point(speed_value);
	spawn_objects_by_type(&ignored, &position, &speed, &speed_value, &ignored, &count, &object);
	spawn_objects_by_handler(&ignored, &ignored, &object);
	check_hits(&ignored, &position, &ignored, &speed_value, &ignored, &count, &ignored);
	check_walls(ignored);
	check_platforms(&ignored, &ignored, &ignored, &speed_value, &ignored, &count, &cycle, &platform, &ignored);
	game_mode_timer++;
	if ((uint16_t)game_mode_timer < 3u || (uint16_t)character_select_timer != 0u)
	{
		*p_speed_value = speed_value;
		*p_count = count;
		*p_cycle = cycle;
		*p_list_cursor = platform;
		return;
	}
	game_mode = 1u;
	system_mode_step = 0u;
	*p_speed_value = speed_value;
	*p_count = count;
	*p_cycle = cycle;
	*p_list_cursor = platform;
}

void game_mode_stage_results(uint32_t position, uint32_t speed, uint32_t object, uint32_t *p_speed_value, uint32_t *p_count, uint32_t *p_cycle, uint32_t *p_list_cursor)
{
	uint32_t speed_value = *p_speed_value;
	uint32_t count = *p_count;
	uint32_t cycle = *p_cycle;
	uint32_t ignored = 0, platform = 0;
	uint32_t stage_results_init_flags = 0;

	stage_results_init_flags = stage_init_flags;
	stage_init_flags = stage_results_init_flags | 0x20000u;
	if ((stage_results_init_flags & 0x20000u) == 0u)
	{
		game_mode_timer = 0x1E0;
		start_ring_tally(&position, &speed, &speed_value, &count, &cycle, &object);
	}
	current_game_mode = 0x1Du;
	next_stage_slot_request = 0u;
	update_scroll(&position, &speed, &speed_value, &ignored, &count, &cycle, &ignored, &object);
	update_stage_tile_layers(&ignored, &position, &speed, &speed_value, &ignored, &count, &cycle, &ignored, &object);
	find_nearby_floor_blocks(&ignored, &speed, &speed_value, &ignored, &count, &ignored, &object);
	speed_value = update_restart_point(speed_value);
	spawn_objects_by_type(&ignored, &position, &speed, &speed_value, &ignored, &count, &object);
	spawn_ring_rows(&ignored, &position, &ignored, &object);
	spawn_walls(&ignored, &position, &ignored, &count, &cycle, &object);
	spawn_object_groups_2(&ignored, &count, &object);
	spawn_object_groups(&ignored, &position, &ignored, &count, &object);
	spawn_objects_by_handler(&ignored, &ignored, &object);
	ignored = check_zones(ignored);
	check_hits(&ignored, &position, &ignored, &speed_value, &ignored, &count, &ignored);
	check_walls(ignored);
	check_platforms(&ignored, &ignored, &ignored, &speed_value, &ignored, &count, &cycle, &platform, &ignored);
	if (stage_results_done == 0u)
	{
		*p_speed_value = speed_value;
		*p_count = count;
		*p_cycle = cycle;
		*p_list_cursor = platform;
		return;
	}
	game_mode = 4u;
	game_mode_timer = 0;
	*p_speed_value = speed_value;
	*p_count = count;
	*p_cycle = cycle;
	*p_list_cursor = platform;
}

void game_mode_stage_change_fade(uint32_t delta, uint32_t position, uint32_t in_play_count, uint16_t next_scroll_position, uint32_t object, uint32_t *p_speed_value, uint32_t *p_count, uint32_t *p_cycle, uint32_t *p_list_cursor)
{
	uint32_t speed_value = *p_speed_value;
	uint32_t count = *p_count;
	uint32_t cycle = *p_cycle;
	uint32_t list_cursor = *p_list_cursor;
	uint32_t ignored = 0, platform = 0;

	if ((uint16_t)game_mode_timer == 0u)
	{
		game_mode_timer = 1;
		fade_step = 0u;
		mixer_text_blend |= 0x4000u;
		mixer_nbg0_blend |= 0x4000u;
		mixer_nbg1_blend |= 0x4000u;
		mixer_nbg2_blend |= 0x4000u;
		mixer_nbg3_blend |= 0x4000u;
		mixer_bitmap_blend |= 0x4000u;
		mixer_sprite_control |= 0x8000u;
	}
	if (game_mode_timer < 0)
	{
		fade_kind = 2;
		delta = update_fade(delta);
		if (fade_kind >= 0)
		{
			current_game_mode = 0x1Eu;
			next_stage_slot_request = 0u;
			update_player_summary(&in_play_count, &count, &ignored, &object);
			update_scroll(&position, &in_play_count, &speed_value, &ignored, &count, &cycle, &ignored, &object);
			update_stage_tile_layers(&ignored, &position, &in_play_count, &speed_value, &ignored, &count, &cycle, &ignored, &object);
			find_nearby_floor_blocks(&ignored, &in_play_count, &speed_value, &ignored, &count, &ignored, &object);
			continue_to_next_stage_slot(&ignored, &ignored, &object);
			speed_value = update_restart_point(speed_value);
			spawn_objects_by_type(&ignored, &position, &in_play_count, &speed_value, &ignored, &count, &object);
			spawn_ring_rows(&ignored, &position, &ignored, &object);
			spawn_walls(&ignored, &position, &ignored, &count, &cycle, &object);
			spawn_object_groups_2(&ignored, &count, &object);
			spawn_object_groups(&ignored, &position, &ignored, &count, &object);
			spawn_objects_by_handler(&ignored, &ignored, &object);
			ignored = check_zones(ignored);
			check_hits(&ignored, &position, &ignored, &speed_value, &ignored, &count, &ignored);
			check_walls(ignored);
			check_platforms(&ignored, &ignored, &ignored, &speed_value, &ignored, &count, &cycle, &platform, &ignored);
			*p_speed_value = speed_value;
			*p_count = count;
			*p_cycle = cycle;
			*p_list_cursor = platform;
			return;
		}
		game_mode = 3u;
		mixer_text_blend &= 0xBFFFu;
		mixer_nbg0_blend &= 0xBFFFu;
		mixer_nbg1_blend &= 0xBFFFu;
		mixer_nbg2_blend &= 0xBFFFu;
		mixer_nbg3_blend &= 0xBFFFu;
		mixer_bitmap_blend &= 0xBFFFu;
		mixer_sprite_control &= ~0x8000u;
		run_game_mode(delta, position, in_play_count, next_scroll_position, object, &speed_value, &count, &cycle, &list_cursor);
		*p_speed_value = speed_value;
		*p_count = count;
		*p_cycle = cycle;
		*p_list_cursor = list_cursor;
		return;
	}
	fade_kind = 0;
	update_fade(delta);
	if (fade_kind >= 0)
	{
		current_game_mode = 0x1Eu;
		next_stage_slot_request = 0u;
		update_player_summary(&in_play_count, &count, &ignored, &object);
		update_scroll(&position, &in_play_count, &speed_value, &ignored, &count, &cycle, &ignored, &object);
		update_stage_tile_layers(&ignored, &position, &in_play_count, &speed_value, &ignored, &count, &cycle, &ignored, &object);
		find_nearby_floor_blocks(&ignored, &in_play_count, &speed_value, &ignored, &count, &ignored, &object);
		continue_to_next_stage_slot(&ignored, &ignored, &object);
		speed_value = update_restart_point(speed_value);
		spawn_objects_by_type(&ignored, &position, &in_play_count, &speed_value, &ignored, &count, &object);
		spawn_ring_rows(&ignored, &position, &ignored, &object);
		spawn_walls(&ignored, &position, &ignored, &count, &cycle, &object);
		spawn_object_groups_2(&ignored, &count, &object);
		spawn_object_groups(&ignored, &position, &ignored, &count, &object);
		spawn_objects_by_handler(&ignored, &ignored, &object);
		ignored = check_zones(ignored);
		check_hits(&ignored, &position, &ignored, &speed_value, &ignored, &count, &ignored);
		check_walls(ignored);
		check_platforms(&ignored, &ignored, &ignored, &speed_value, &ignored, &count, &cycle, &platform, &ignored);
		*p_speed_value = speed_value;
		*p_count = count;
		*p_cycle = cycle;
		*p_list_cursor = platform;
		return;
	}
	start_stage(next_scroll_position, &position, &in_play_count, &speed_value, &count, &cycle, &platform, &object);
	game_mode = 0x1Eu;
	game_mode_timer = -1;
	fade_step = 0u;
	mixer_text_blend |= 0x4000u;
	mixer_nbg0_blend |= 0x4000u;
	mixer_nbg1_blend |= 0x4000u;
	mixer_nbg2_blend |= 0x4000u;
	mixer_nbg3_blend |= 0x4000u;
	mixer_bitmap_blend |= 0x4000u;
	mixer_sprite_control |= 0x8000u;
	if (stage != 0x11u)
	{
		current_game_mode = 0x1Eu;
		next_stage_slot_request = 0u;
		update_player_summary(&in_play_count, &count, &ignored, &object);
		update_scroll(&position, &in_play_count, &speed_value, &ignored, &count, &cycle, &ignored, &object);
		update_stage_tile_layers(&ignored, &position, &in_play_count, &speed_value, &ignored, &count, &cycle, &ignored, &object);
		find_nearby_floor_blocks(&ignored, &in_play_count, &speed_value, &ignored, &count, &ignored, &object);
		continue_to_next_stage_slot(&ignored, &ignored, &object);
		speed_value = update_restart_point(speed_value);
		spawn_objects_by_type(&ignored, &position, &in_play_count, &speed_value, &ignored, &count, &object);
		spawn_ring_rows(&ignored, &position, &ignored, &object);
		spawn_walls(&ignored, &position, &ignored, &count, &cycle, &object);
		spawn_object_groups_2(&ignored, &count, &object);
		spawn_object_groups(&ignored, &position, &ignored, &count, &object);
		spawn_objects_by_handler(&ignored, &ignored, &object);
		ignored = check_zones(ignored);
		check_hits(&ignored, &position, &ignored, &speed_value, &ignored, &count, &ignored);
		check_walls(ignored);
		check_platforms(&ignored, &ignored, &ignored, &speed_value, &ignored, &count, &cycle, &platform, &ignored);
		*p_speed_value = speed_value;
		*p_count = count;
		*p_cycle = cycle;
		*p_list_cursor = platform;
		return;
	}
	game_mode = 3u;
	*p_speed_value = speed_value;
	*p_count = count;
	*p_cycle = cycle;
	*p_list_cursor = platform;
}

void game_mode_you_failed(uint32_t box_edge, uint32_t position, uint32_t in_play_count, uint32_t object, uint32_t *p_speed_value, uint32_t *p_count, uint32_t *p_cycle, uint32_t *p_list_cursor)
{
	uint32_t speed_value = *p_speed_value;
	uint32_t count = *p_count;
	uint32_t cycle = *p_cycle;
	uint8_t sound_id = 0;
	uint32_t ignored = 0, platform = 0, record = 0;

	if ((uint16_t)game_mode_timer == 0u)
	{
		game_mode_timer++;
		current_game_mode = 0x20u;
		you_failed_done = 0u;
		you_failed_fade_red = 0u;
		you_failed_fade_green = 0u;
		you_failed_fade_blue = 0u;
		color_offset_red = 0u;
		color_offset_green = 0u;
		color_offset_blue = 0u;
		mixer_nbg0_blend |= 0x4000u;
		mixer_nbg1_blend |= 0x4000u;
		mixer_nbg2_blend |= 0x4000u;
		mixer_nbg3_blend |= 0x4000u;
		mixer_sprite_control |= 0x8000u;
		sound_id = 0x94u;
		queue_sound(sound_id, &box_edge, &position);
	}
	next_stage_slot_request = 0u;
	update_player_summary(&in_play_count, &count, &ignored, &object);
	find_nearby_floor_blocks(&box_edge, &in_play_count, &speed_value, &ignored, &count, &ignored, &object);
	speed_value = update_restart_point(speed_value);
	spawn_objects_by_type(&box_edge, &position, &in_play_count, &speed_value, &ignored, &count, &object);
	spawn_ring_rows(&box_edge, &position, &in_play_count, &object);
	spawn_walls(&box_edge, &position, &ignored, &count, &cycle, &object);
	spawn_object_groups_2(&box_edge, &count, &object);
	spawn_object_groups(&box_edge, &position, &in_play_count, &count, &object);
	spawn_objects_by_handler(&box_edge, &ignored, &object);
	box_edge = check_zones(box_edge);
	check_hits(&box_edge, &position, &in_play_count, &speed_value, &ignored, &count, &ignored);
	box_edge = check_walls(box_edge);
	check_platforms(&box_edge, &position, &in_play_count, &speed_value, &ignored, &count, &cycle, &platform, &ignored);
	if ((int16_t)color_offset_red < 0x1F)
	{
		you_failed_fade_red++;
		color_offset_red = you_failed_fade_red >> 2;
	}
	if ((int16_t)color_offset_green < 0x1F)
	{
		you_failed_fade_green++;
		color_offset_green = you_failed_fade_green >> 2;
	}
	if ((int16_t)color_offset_blue < 0x1F)
	{
		you_failed_fade_blue++;
		color_offset_blue = you_failed_fade_blue >> 2;
	}
	if (you_failed_done == 0u && (int16_t)color_offset_red >= 0x1F && (int16_t)color_offset_green >= 0x1F && (int16_t)color_offset_blue >= 0x1F)
		game_mode_timer++;
	if ((uint16_t)game_mode_timer == 2u)
	{
		sound_id = 0x85u;
		play_sound(sound_id, &box_edge, &position);
	}
	if (game_mode_timer >= 0x78)
	{
		record = you_failed_string_ADDRESS;
		queue_string_record(record, (uint16_t)box_edge, in_play_count, &position, &speed_value);
		mixer_sprite_control &= ~0x8000u;
	}
	if (game_mode_timer >= 0x12C)
		you_failed_done = 0xFFu;
	if ((player_status[0] | player_2_status | player_3_status | player_status_spare) != 0u)
	{
		*p_speed_value = speed_value;
		*p_count = count;
		*p_cycle = cycle;
		*p_list_cursor = platform;
		return;
	}
	saved_game_mode = game_mode;
	game_mode = 7u;
	game_mode_timer = 0;
	*p_speed_value = speed_value;
	*p_count = count;
	*p_cycle = cycle;
	*p_list_cursor = platform;
}

void check_stage_select_buttons(void)
{
	if ((input_ports_active & 1u) != 0u && (inputs_pressed & 0x10000u) != 0u)
		game_mode_word = 8u;
}

void game_mode_stage_select(uint32_t *p_count, uint32_t *p_list_cursor)
{
	uint32_t count = *p_count;
	uint16_t cell = 0;
	uint32_t attribute = 0, clear_dest = 0, fill_word = 0, ignored = 0, new_stage = 0, player_number = 0, text = 0, text_position = 0, trackball = 0, value = 0;
	uint32_t stage_select_init_flags = 0;

	stage_select_init_flags = stage_init_flags;
	stage_init_flags = stage_select_init_flags | 0x40u;
	if ((stage_select_init_flags & 0x40u) == 0u)
	{
		stage = 3u;
		text = stage_select_mode_text_ADDRESS;
		text_position = TEXT_CELL(14, 20);
		print_text_here(&ignored, &text, &text_position);
		text = stage_number_text_ADDRESS;
		text_position = TEXT_CELL(18, 21);
		print_text_here(&ignored, &text, &text_position);
		player_number = 0u;
		reset_trackball_step(player_number);
		clear_dest = VIDEO_RAM + 0xE000u;
		count = 0xE00u;
		fill_word = 0u;
		do
		{
			mem_write32(clear_dest, fill_word);
			clear_dest += 4u;
			mem_write32(clear_dest, fill_word);
			clear_dest += 4u;
			mem_write32(clear_dest, fill_word);
			clear_dest += 4u;
			mem_write32(clear_dest, fill_word);
			count--;
			clear_dest += 4u;
		} while (count != 0u);
	}
	new_stage = 0u;
	read_trackball_step(&new_stage, &trackball);
	if ((uint16_t)new_stage == 0u)
	{
		value = stage;
		attribute = 0u;
		text_position = TEXT_CELL(24, 21);
		draw_hex_4(&value, &attribute, &text_position);
		if ((inputs_pressed & 0x10u) == 0u)
		{
			*p_count = count;
			*p_list_cursor = trackball;
			return;
		}
		mem_write16(cleared_stages_ADDRESS, stage_select_cleared_values[stage]);
		game_mode = 2u;
		stage_init_flags &= ~0x40u;
		stage_start_kind = 1u;
		text = stage_select_mode_text_ADDRESS;
		text_position = TEXT_CELL(14, 20);
		cell = 0u;
		erase_text_here(cell, &text, &text_position);
		text = stage_number_text_ADDRESS;
		text_position = TEXT_CELL(18, 21);
		erase_text_here(cell, &text, &text_position);
		text = stage_number_text_ADDRESS;
		text_position = TEXT_CELL(24, 21);
		erase_text_here(cell, &text, &text_position);
		*p_count = count;
		*p_list_cursor = text;
		return;
	}
	new_stage = (uint16_t)(new_stage + stage);
	if ((int16_t)new_stage < 0)
		new_stage = 0x11u;
	if ((int16_t)new_stage > 0x11)
		new_stage = 0u;
	stage = new_stage;
	value = stage;
	attribute = 0u;
	text_position = TEXT_CELL(24, 21);
	draw_hex_4(&value, &attribute, &text_position);
	if ((inputs_pressed & 0x10u) == 0u)
	{
		*p_count = count;
		*p_list_cursor = trackball;
		return;
	}
	mem_write16(cleared_stages_ADDRESS, stage_select_cleared_values[stage]);
	game_mode = 2u;
	stage_init_flags &= ~0x40u;
	stage_start_kind = 1u;
	text = stage_select_mode_text_ADDRESS;
	text_position = TEXT_CELL(14, 20);
	cell = 0u;
	erase_text_here(cell, &text, &text_position);
	text = stage_number_text_ADDRESS;
	text_position = TEXT_CELL(18, 21);
	erase_text_here(cell, &text, &text_position);
	text = stage_number_text_ADDRESS;
	text_position = TEXT_CELL(24, 21);
	erase_text_here(cell, &text, &text_position);
	*p_count = count;
	*p_list_cursor = text;
}

void game_mode_event_09(uint32_t speed, uint32_t object, uint32_t *p_speed_value, uint32_t *p_count, uint32_t *p_cycle, uint32_t *p_list_cursor)
{
	uint32_t speed_value = *p_speed_value;
	uint32_t count = *p_count;
	uint32_t cycle = *p_cycle;
	uint32_t list_cursor = *p_list_cursor;
	uint32_t ignored = 0;

	update_scroll(&ignored, &speed, &speed_value, &ignored, &count, &cycle, &list_cursor, &object);
	update_stage_tile_layers(&ignored, &ignored, &ignored, &speed_value, &ignored, &count, &cycle, &list_cursor, &object);
	continue_to_next_stage_slot(&ignored, &list_cursor, &ignored);
	find_nearby_floor_blocks(&ignored, &ignored, &speed_value, &ignored, &count, &list_cursor, &ignored);
	*p_speed_value = speed_value;
	*p_count = count;
	*p_cycle = cycle;
	*p_list_cursor = list_cursor;
}

void game_mode_event_0A(uint32_t pressed, uint32_t position, uint32_t speed, uint16_t next_scroll_position, uint32_t object, uint32_t *p_speed_value, uint32_t *p_count, uint32_t *p_cycle, uint32_t *p_list_cursor)
{
	uint32_t speed_value = *p_speed_value;
	uint32_t count = *p_count;
	uint32_t cycle = *p_cycle;
	uint32_t list_cursor = *p_list_cursor;

	current_game_mode = 0xAu;
	if ((player_status[0] | player_2_status | player_3_status | player_status_spare) != 0u)
	{
		update_stage_05_event(position, speed, object, &speed_value, &count, &cycle, &list_cursor);
		*p_speed_value = speed_value;
		*p_count = count;
		*p_cycle = cycle;
		*p_list_cursor = list_cursor;
		return;
	}
	saved_game_mode = game_mode;
	game_mode = 7u;
	game_mode_timer = 0;
	run_game_mode(pressed, position, speed, next_scroll_position, object, &speed_value, &count, &cycle, &list_cursor);
	*p_speed_value = speed_value;
	*p_count = count;
	*p_cycle = cycle;
	*p_list_cursor = list_cursor;
}

void game_mode_event_0C(uint32_t position, uint32_t speed, uint32_t object, uint32_t *p_speed_value, uint32_t *p_count, uint32_t *p_cycle, uint32_t *p_list_cursor)
{
	uint32_t speed_value = *p_speed_value;
	uint32_t count = *p_count;
	uint32_t cycle = *p_cycle;
	uint32_t ignored = 0, platform = 0;

	current_game_mode = 0xCu;
	switch (scene_step)
	{
	case 0:
		event_0c_start(&position, &speed, &object);
		break;
	case 1:
		position = event_0c_first_quake(position);
		break;
	case 2:
		event_0c_second_quake(&position, &count, &cycle, &object);
		break;
	case 3:
		event_0c_finish();
		break;
	case 4:
		event_0c_start(&position, &speed, &object);
		break;
	default:
		invalid_jump(0x9487Cu + scene_step * 3u);
	}
	update_scroll(&position, &speed, &speed_value, &ignored, &count, &cycle, &ignored, &object);
	update_stage_tile_layers(&ignored, &position, &ignored, &speed_value, &ignored, &count, &cycle, &ignored, &object);
	speed_value = update_restart_point(speed_value);
	check_hits(&ignored, &position, &ignored, &speed_value, &ignored, &count, &ignored);
	check_walls(ignored);
	check_platforms(&ignored, &ignored, &ignored, &speed_value, &ignored, &count, &cycle, &platform, &ignored);
	*p_speed_value = speed_value;
	*p_count = count;
	*p_cycle = cycle;
	*p_list_cursor = platform;
}

void game_mode_event_0D(uint32_t position, uint32_t speed, uint32_t object, uint32_t *p_speed_value, uint32_t *p_count, uint32_t *p_cycle, uint32_t *p_list_cursor)
{
	uint32_t speed_value = *p_speed_value;
	uint32_t count = *p_count;
	uint32_t cycle = *p_cycle;
	uint32_t ignored = 0, platform = 0;
	uint16_t event_0d_timer = 0;

	current_game_mode = 0xDu;
	switch (scene_step)
	{
	case 0:
	case 3:
		scene_timer = 0xB4;
		event_0d_active = 0xFFu;
		players_frozen = 0xFFu;
		scene_step++;
		break;
	case 1:
	case 2:
		break;
	default:
		invalid_jump(0x946A8u + scene_step * 2u);
	}
	if (scene_step != 2u)
	{
		event_0d_timer = scene_timer;
		scene_timer = event_0d_timer - 1u;
		if ((int16_t)event_0d_timer > 1)
		{
			find_nearby_floor_blocks(&ignored, &speed, &speed_value, &ignored, &count, &ignored, &object);
			update_scroll(&position, &speed, &speed_value, &ignored, &count, &cycle, &ignored, &object);
			update_stage_tile_layers(&ignored, &position, &speed, &speed_value, &ignored, &count, &cycle, &ignored, &object);
			spawn_objects_by_type(&ignored, &position, &speed, &speed_value, &ignored, &count, &object);
			spawn_ring_rows(&ignored, &position, &ignored, &object);
			spawn_walls(&ignored, &position, &ignored, &count, &cycle, &object);
			spawn_object_groups_2(&ignored, &count, &object);
			spawn_object_groups(&ignored, &position, &ignored, &count, &object);
			spawn_objects_by_handler(&ignored, &ignored, &object);
			check_hits(&ignored, &position, &ignored, &speed_value, &ignored, &count, &ignored);
			check_walls(ignored);
			check_platforms(&ignored, &ignored, &ignored, &speed_value, &ignored, &count, &cycle, &platform, &ignored);
			if (scroll_position < 0xA00u)
			{
				*p_speed_value = speed_value;
				*p_count = count;
				*p_cycle = cycle;
				*p_list_cursor = platform;
				return;
			}
			game_mode = 3u;
			event_0d_active = 0u;
			players_frozen = 0u;
			scroll_mode = 7u;
			*p_speed_value = speed_value;
			*p_count = count;
			*p_cycle = cycle;
			*p_list_cursor = platform;
			return;
		}
		scene_step++;
	}
	scroll_mode = 0u;
	scroll_speed = 0x1Cu;
	find_nearby_floor_blocks(&ignored, &speed, &speed_value, &ignored, &count, &ignored, &object);
	update_scroll(&position, &speed, &speed_value, &ignored, &count, &cycle, &ignored, &object);
	update_stage_tile_layers(&ignored, &position, &speed, &speed_value, &ignored, &count, &cycle, &ignored, &object);
	spawn_objects_by_type(&ignored, &position, &speed, &speed_value, &ignored, &count, &object);
	spawn_ring_rows(&ignored, &position, &ignored, &object);
	spawn_walls(&ignored, &position, &ignored, &count, &cycle, &object);
	spawn_object_groups_2(&ignored, &count, &object);
	spawn_object_groups(&ignored, &position, &ignored, &count, &object);
	spawn_objects_by_handler(&ignored, &ignored, &object);
	check_hits(&ignored, &position, &ignored, &speed_value, &ignored, &count, &ignored);
	check_walls(ignored);
	check_platforms(&ignored, &ignored, &ignored, &speed_value, &ignored, &count, &cycle, &platform, &ignored);
	if (scroll_position < 0xA00u)
	{
		*p_speed_value = speed_value;
		*p_count = count;
		*p_cycle = cycle;
		*p_list_cursor = platform;
		return;
	}
	game_mode = 3u;
	event_0d_active = 0u;
	players_frozen = 0u;
	scroll_mode = 7u;
	*p_speed_value = speed_value;
	*p_count = count;
	*p_cycle = cycle;
	*p_list_cursor = platform;
}

void game_mode_event_1A(uint32_t pressed, uint32_t position, uint32_t in_play_count, uint16_t next_scroll_position, uint32_t object, uint32_t *p_speed_value, uint32_t *p_count, uint32_t *p_cycle, uint32_t *p_list_cursor)
{
	uint32_t speed_value = *p_speed_value;
	uint32_t count = *p_count;
	uint32_t cycle = *p_cycle;
	uint32_t list_cursor = *p_list_cursor;
	uint32_t ignored = 0, platform = 0, scroll_script = 0;
	uint16_t event_1a_timer = 0;

	current_game_mode = 0x1Au;
	if (scene_step == 0u)
	{
		scene_step++;
		player_1_sequence_step = 0u;
		player_2_sequence_step = 0u;
		player_3_sequence_step = 0u;
		event_1a_scroll_mode = event_1a_first_scroll_mode;
		event_1a_scroll_speed = event_1a_first_scroll_speed;
		seesaw_launched = 0u;
		scene_timer = 0x1E;
		scene_object = 0xFFFFu;
		object = event_1a_scroll_script_step_1_ADDRESS;
	}
	next_stage_slot_request = 0u;
	update_player_summary(&in_play_count, &count, &list_cursor, &object);
	if ((player_status[0] | player_2_status | player_3_status | player_status_spare) == 0u)
	{
		saved_game_mode = game_mode;
		game_mode = 7u;
		game_mode_timer = 0;
		run_game_mode(pressed, position, in_play_count, next_scroll_position, object, &speed_value, &count, &cycle, &list_cursor);
		*p_speed_value = speed_value;
		*p_count = count;
		*p_cycle = cycle;
		*p_list_cursor = list_cursor;
		return;
	}
	scroll_script = event_1a_scroll_script_ADDRESS + scene_step * 3u * 2u;
	if (scroll_position < ROM16(scroll_script))
	{
		if ((int16_t)ROM16(scroll_script) < 0 && seesaw_launched != 0u)
		{
			event_1a_timer = scene_timer;
			scene_timer = event_1a_timer - 1u;
			if ((int16_t)event_1a_timer <= 1)
			{
				scene_timer = 0;
				scroll_script += 2u;
				event_1a_scroll_mode = ROM16(scroll_script);
				scroll_script += 2u;
				event_1a_scroll_speed = ROM16(scroll_script);
				scene_step++;
				scroll_script += 2u;
			}
		}
	}
	else
	{
		scroll_script += 2u;
		event_1a_scroll_mode = ROM16(scroll_script);
		scroll_script += 2u;
		event_1a_scroll_speed = ROM16(scroll_script);
		scene_step++;
		scroll_script += 2u;
	}
	scroll_mode = event_1a_scroll_mode;
	scroll_speed = event_1a_scroll_speed;
	update_scroll(&position, &in_play_count, &speed_value, &ignored, &count, &cycle, &ignored, &scroll_script);
	update_stage_tile_layers(&ignored, &position, &in_play_count, &speed_value, &ignored, &count, &cycle, &ignored, &scroll_script);
	find_nearby_floor_blocks(&ignored, &in_play_count, &speed_value, &ignored, &count, &ignored, &scroll_script);
	speed_value = update_restart_point(speed_value);
	spawn_objects_by_type(&ignored, &position, &in_play_count, &speed_value, &ignored, &count, &scroll_script);
	spawn_ring_rows(&ignored, &position, &ignored, &scroll_script);
	spawn_walls(&ignored, &position, &ignored, &count, &cycle, &scroll_script);
	spawn_object_groups_2(&ignored, &count, &scroll_script);
	spawn_object_groups(&ignored, &position, &ignored, &count, &scroll_script);
	spawn_objects_by_handler(&ignored, &ignored, &scroll_script);
	ignored = check_zones(ignored);
	check_hits(&ignored, &position, &ignored, &speed_value, &ignored, &count, &ignored);
	check_walls(ignored);
	check_platforms(&ignored, &ignored, &ignored, &speed_value, &ignored, &count, &cycle, &platform, &ignored);
	if (scroll_position < 0x6080u)
	{
		*p_speed_value = speed_value;
		*p_count = count;
		*p_cycle = cycle;
		*p_list_cursor = platform;
		return;
	}
	game_mode = 3u;
	scroll_speed_low = 5;
	scroll_mode_low = 7u;
	*p_speed_value = speed_value;
	*p_count = count;
	*p_cycle = cycle;
	*p_list_cursor = platform;
}

void game_mode_event_0F(uint32_t position, uint32_t speed, uint32_t object, uint32_t *p_speed_value, uint32_t *p_count, uint32_t *p_cycle, uint32_t *p_list_cursor)
{
	uint32_t speed_value = *p_speed_value;
	uint32_t count = *p_count;
	uint32_t cycle = *p_cycle;
	uint32_t ignored = 0, platform = 0;

	current_game_mode = 0xFu;
	switch (scene_step)
	{
	case 0:
		event_0f_start();
		break;
	case 1:
		event_0f_stop_scroll();
		break;
	case 2:
		event_0f_show_messages(&count, &object);
		break;
	case 3:
		event_0f_start();
		break;
	default:
		invalid_jump(0x94606u + scene_step * 3u);
	}
	update_scroll(&position, &speed, &speed_value, &ignored, &count, &cycle, &ignored, &object);
	update_stage_tile_layers(&ignored, &position, &ignored, &speed_value, &ignored, &count, &cycle, &ignored, &object);
	speed_value = update_restart_point(speed_value);
	spawn_walls(&ignored, &position, &ignored, &count, &cycle, &object);
	check_hits(&ignored, &position, &ignored, &speed_value, &ignored, &count, &ignored);
	check_walls(ignored);
	check_platforms(&ignored, &ignored, &ignored, &speed_value, &ignored, &count, &cycle, &platform, &ignored);
	*p_speed_value = speed_value;
	*p_count = count;
	*p_cycle = cycle;
	*p_list_cursor = platform;
}

void game_mode_event_10(uint32_t random_value, uint32_t object, uint32_t *p_rolls_left, uint32_t *p_words_left, uint32_t *p_list_cursor)
{
	uint32_t rolls_left = *p_rolls_left;
	uint32_t words_left = *p_words_left;
	uint32_t ignored = 0, record = 0;

	current_game_mode = 0x10u;
	switch (scene_step)
	{
	case 0:
	case 4:
		scene_step++;
		scene_timer = 0;
		scene_timer++;
		spawn_objects_by_handler(&ignored, &record, &object);
		if ((uint16_t)scene_timer == 0x168u)
			stage_06_background_step = 3u;
		update_stage_06_background(random_value, object, &rolls_left, &words_left, &record);
		if (scene_timer >= 0xF0 && scene_timer <= 0x12B && (scene_timer & 7u) == 0u)
		{
			mixer_sprite_control |= 0x8000u;
			color_offset_red--;
			color_offset_green--;
			color_offset_blue--;
		}
		if (scene_timer < 0x1A4)
		{
			*p_rolls_left = rolls_left;
			*p_words_left = words_left;
			*p_list_cursor = record;
			return;
		}
		scene_step++;
		stage_06_background_step = 5u;
		*p_rolls_left = rolls_left;
		*p_words_left = words_left;
		*p_list_cursor = record;
		return;
	case 1:
		scene_timer++;
		spawn_objects_by_handler(&ignored, &record, &object);
		if ((uint16_t)scene_timer == 0x168u)
			stage_06_background_step = 3u;
		update_stage_06_background(random_value, object, &rolls_left, &words_left, &record);
		if (scene_timer >= 0xF0 && scene_timer <= 0x12B && (scene_timer & 7u) == 0u)
		{
			mixer_sprite_control |= 0x8000u;
			color_offset_red--;
			color_offset_green--;
			color_offset_blue--;
		}
		if (scene_timer < 0x1A4)
		{
			*p_rolls_left = rolls_left;
			*p_words_left = words_left;
			*p_list_cursor = record;
			return;
		}
		scene_step++;
		stage_06_background_step = 5u;
		*p_rolls_left = rolls_left;
		*p_words_left = words_left;
		*p_list_cursor = record;
		return;
	case 2:
		scene_step++;
		scene_timer = 0;
		mixer_sprite_control &= ~0x8000u;
		color_offset_red = 0u;
		color_offset_green = 0u;
		color_offset_blue = 0u;
		scene_timer++;
		spawn_objects_by_handler(&ignored, &ignored, &object);
		spawn_stage_06_lightning(&record, &object);
		update_stage_06_background(random_value, object, &rolls_left, &words_left, &record);
		if (scene_timer < 0x12C)
		{
			*p_rolls_left = rolls_left;
			*p_words_left = words_left;
			*p_list_cursor = record;
			return;
		}
		game_mode = 2u;
		*p_rolls_left = rolls_left;
		*p_words_left = words_left;
		*p_list_cursor = record;
		return;
	case 3:
		scene_timer++;
		spawn_objects_by_handler(&ignored, &ignored, &object);
		spawn_stage_06_lightning(&record, &object);
		update_stage_06_background(random_value, object, &rolls_left, &words_left, &record);
		if (scene_timer < 0x12C)
		{
			*p_rolls_left = rolls_left;
			*p_words_left = words_left;
			*p_list_cursor = record;
			return;
		}
		game_mode = 2u;
		*p_rolls_left = rolls_left;
		*p_words_left = words_left;
		*p_list_cursor = record;
		return;
	default:
		invalid_jump(0x94510u + scene_step * 3u);
	}
}

void game_mode_event_12(uint32_t pressed, uint32_t position, uint32_t done_count, uint16_t next_scroll_position, uint32_t object, uint32_t *p_speed_value, uint32_t *p_count, uint32_t *p_cycle, uint32_t *p_list_cursor)
{
	uint32_t speed_value = *p_speed_value;
	uint32_t count = *p_count;
	uint32_t cycle = *p_cycle;
	uint32_t list_cursor = *p_list_cursor;
	uint16_t kind = 0, x = 0, y = 0;
	uint32_t z = 0;
	uint8_t event_12_status = 0;

	current_game_mode = 0x12u;
	if (scroll_position < 0x80B1u)
	{
		if (scroll_position < 0x80B1u)
		{
			event_12_stage_frame(pressed, position, done_count, next_scroll_position, object, &speed_value, &count, &cycle, &list_cursor);
			*p_speed_value = speed_value;
			*p_count = count;
			*p_cycle = cycle;
			*p_list_cursor = list_cursor;
			return;
		}
		scroll_mode = 1u;
		event_12_stage_frame(pressed, position, done_count, next_scroll_position, object, &speed_value, &count, &cycle, &list_cursor);
		*p_speed_value = speed_value;
		*p_count = count;
		*p_cycle = cycle;
		*p_list_cursor = list_cursor;
		return;
	}
	done_count &= ~0xFFu;
	object = PLAYER_BLOCK(1);
	list_cursor = player_status_ADDRESS;
	count = 3u;
	do
	{
		event_12_status = mem_read8(list_cursor);
		list_cursor++;
		if (event_12_status != 0xFFu || PLAYER(object)->drift_done != 0u)
		{
			done_count = (done_count & ~0xFFu) | (uint8_t)(done_count + 1u);
		}
		object += 0x200u;
		count--;
	} while (count != 0u);
	if ((uint8_t)done_count != 3u)
	{
		if (scroll_position < 0x80B1u)
		{
			event_12_stage_frame(pressed, position, done_count, next_scroll_position, object, &speed_value, &count, &cycle, &list_cursor);
			*p_speed_value = speed_value;
			*p_count = count;
			*p_cycle = cycle;
			*p_list_cursor = list_cursor;
			return;
		}
		scroll_mode = 1u;
		event_12_stage_frame(pressed, position, done_count, next_scroll_position, object, &speed_value, &count, &cycle, &list_cursor);
		*p_speed_value = speed_value;
		*p_count = count;
		*p_cycle = cycle;
		*p_list_cursor = list_cursor;
		return;
	}
	x = 0x644u;
	y = 0x149Eu;
	kind = 0xFu;
	z = (count & ~0xFFFFu) | 0x168u;
	spawn_sign(x, y, kind, (uint16_t)z, &pressed, &object);
	event_12_fast_scroll = 0u;
	scroll_mode = 7u;
	game_mode = 3u;
	run_game_mode(pressed, position, done_count, x, object, &speed_value, &z, &cycle, &list_cursor);
	*p_speed_value = speed_value;
	*p_count = z;
	*p_cycle = cycle;
	*p_list_cursor = list_cursor;
}

void game_mode_event_13(uint32_t pressed, uint32_t position, uint32_t in_play_count, uint32_t edges_done, uint32_t object, uint32_t *p_speed_value, uint32_t *p_count, uint32_t *p_cycle, uint32_t *p_list_cursor)
{
	uint32_t speed_value = *p_speed_value;
	uint32_t count = *p_count;
	uint32_t cycle = *p_cycle;
	uint32_t list_cursor = *p_list_cursor;
	uint32_t clip_bottom = 0, clip_right = 0, clip_top = 0;
	uint16_t event_13_timer = 0;

	current_game_mode = 0x13u;
	switch (scene_step)
	{
	case 0:
	case 3:
		scene_step++;
		scene_timer = 0x78;
		break;
	case 1:
		break;
	case 2:
		if (scroll_position < 0x16ACu)
		{
			event_12_stage_frame(pressed, position, in_play_count, (uint16_t)edges_done, object, &speed_value, &count, &cycle, &list_cursor);
			*p_speed_value = speed_value;
			*p_count = count;
			*p_cycle = cycle;
			*p_list_cursor = list_cursor;
			return;
		}
		game_mode = 3u;
		run_game_mode(pressed, position, in_play_count, (uint16_t)edges_done, object, &speed_value, &count, &cycle, &list_cursor);
		*p_speed_value = speed_value;
		*p_count = count;
		*p_cycle = cycle;
		*p_list_cursor = list_cursor;
		return;
	default:
		invalid_jump(0x943A5u + scene_step * 2u);
	}
	scroll_mode = 1u;
	event_13_timer = scene_timer;
	scene_timer = event_13_timer - 1u;
	if ((int16_t)event_13_timer > 1)
	{
		update_screen_wipe(&clip_right, &clip_top, &clip_bottom, &edges_done);
		event_12_stage_frame(clip_right, clip_top, clip_bottom, (uint16_t)edges_done, object, &speed_value, &count, &cycle, &list_cursor);
		*p_speed_value = speed_value;
		*p_count = count;
		*p_cycle = cycle;
		*p_list_cursor = list_cursor;
		return;
	}
	scene_step++;
	scroll_mode = 0u;
	update_screen_wipe(&clip_right, &clip_top, &clip_bottom, &edges_done);
	event_12_stage_frame(clip_right, clip_top, clip_bottom, (uint16_t)edges_done, object, &speed_value, &count, &cycle, &list_cursor);
	*p_speed_value = speed_value;
	*p_count = count;
	*p_cycle = cycle;
	*p_list_cursor = list_cursor;
}

void game_mode_event_14(uint32_t pressed, uint32_t position, uint32_t unbounced_count, uint16_t x, uint32_t object, uint32_t *p_speed_value, uint32_t *p_count, uint32_t *p_cycle, uint32_t *p_list_cursor)
{
	uint32_t speed_value = *p_speed_value;
	uint32_t count = *p_count;
	uint32_t cycle = *p_cycle;
	uint32_t list_cursor = *p_list_cursor;
	uint8_t playing_count = 0;
	uint16_t kind = 0, y = 0;
	uint32_t ignored = 0, platform = 0;
	uint8_t event_14_status = 0;
	uint16_t event_14_timer = 0;

	current_game_mode = 0x14u;
	switch (scene_step)
	{
	case 0:
	case 3:
		scene_step++;
		break;
	case 1:
		unbounced_count &= ~0xFFu;
		playing_count = 3u;
		list_cursor = PLAYER_BLOCK(1);
		object = player_status_ADDRESS;
		count = 3u;
		do
		{
			event_14_status = mem_read8(object);
			object++;
			if (event_14_status == 0xFFu)
			{
				if (PLAYER(list_cursor)->state == 0x25u && PLAYER(list_cursor)->sequence_bounced == 0u)
					unbounced_count = (unbounced_count & ~0xFFu) | (uint8_t)(unbounced_count + 1u);
			}
			else
				playing_count--;
			list_cursor += 0x200u;
			count--;
		} while (count != 0u);
		if (playing_count != 0u && (uint8_t)unbounced_count == 0u && free_object_stack >= 2)
		{
			scene_step++;
			allocate_object_outside_loop(&ignored, &object);
			OBJECT(object)->sprite_frame = 0u;
			OBJECT(object)->handler = icicle_dropper_start_ADDRESS;
			allocate_object_outside_loop(&ignored, &object);
			OBJECT(object)->sprite_frame = 2u;
			OBJECT(object)->handler = eggman_speech_start_ADDRESS;
			scene_timer = 0x12C;
			event_14_timer = scene_timer;
			scene_timer = event_14_timer - 1u;
			if ((int16_t)event_14_timer <= 1)
			{
				scroll_mode = 7u;
				game_mode = 3u;
				run_game_mode(pressed, position, unbounced_count, x, object, &speed_value, &count, &cycle, &list_cursor);
				*p_speed_value = speed_value;
				*p_count = count;
				*p_cycle = cycle;
				*p_list_cursor = list_cursor;
				return;
			}
		}
		break;
	case 2:
		event_14_timer = scene_timer;
		scene_timer = event_14_timer - 1u;
		if ((int16_t)event_14_timer <= 1)
		{
			scroll_mode = 7u;
			game_mode = 3u;
			run_game_mode(pressed, position, unbounced_count, x, object, &speed_value, &count, &cycle, &list_cursor);
			*p_speed_value = speed_value;
			*p_count = count;
			*p_cycle = cycle;
			*p_list_cursor = list_cursor;
			return;
		}
		break;
	default:
		invalid_jump(0x94253u + scene_step * 3u);
	}
	if ((uint16_t)scene_timer == 0x3Cu)
	{
		x = 0xFDEFu;
		y = 0xAF0u;
		kind = 0xEu;
		count = (count & ~0xFFFFu) | 0x28u;
		spawn_sign(x, y, kind, (uint16_t)count, &pressed, &object);
	}
	update_player_summary(&unbounced_count, &count, &list_cursor, &object);
	scroll_mode = 1u;
	if ((player_status[0] | player_2_status | player_3_status | player_status_spare) == 0u)
	{
		saved_game_mode = game_mode;
		game_mode = 7u;
		game_mode_timer = 0;
		run_game_mode(pressed, position, unbounced_count, x, object, &speed_value, &count, &cycle, &list_cursor);
		*p_speed_value = speed_value;
		*p_count = count;
		*p_cycle = cycle;
		*p_list_cursor = list_cursor;
		return;
	}
	update_scroll(&position, &unbounced_count, &speed_value, &ignored, &count, &cycle, &ignored, &object);
	update_stage_tile_layers(&ignored, &position, &unbounced_count, &speed_value, &ignored, &count, &cycle, &ignored, &object);
	find_nearby_floor_blocks(&ignored, &unbounced_count, &speed_value, &ignored, &count, &ignored, &object);
	speed_value = update_restart_point(speed_value);
	spawn_objects_by_type(&ignored, &position, &unbounced_count, &speed_value, &ignored, &count, &object);
	spawn_ring_rows(&ignored, &position, &ignored, &object);
	spawn_walls(&ignored, &position, &ignored, &count, &cycle, &object);
	spawn_object_groups_2(&ignored, &count, &object);
	spawn_object_groups(&ignored, &position, &ignored, &count, &object);
	spawn_objects_by_handler(&ignored, &ignored, &object);
	ignored = check_zones(ignored);
	check_hits(&ignored, &position, &ignored, &speed_value, &ignored, &count, &ignored);
	check_walls(ignored);
	check_platforms(&ignored, &ignored, &ignored, &speed_value, &ignored, &count, &cycle, &platform, &ignored);
	*p_speed_value = speed_value;
	*p_count = count;
	*p_cycle = cycle;
	*p_list_cursor = platform;
}

void game_mode_event_15(uint32_t pressed, uint32_t position, uint32_t speed, uint16_t next_scroll_position, uint32_t object, uint32_t *p_speed_value, uint32_t *p_count, uint32_t *p_cycle, uint32_t *p_list_cursor)
{
	uint32_t speed_value = *p_speed_value;
	uint32_t count = *p_count;
	uint32_t cycle = *p_cycle;
	uint32_t list_cursor = *p_list_cursor;
	uint32_t ignored = 0, platform = 0;

	current_game_mode = 0x15u;
	next_stage_slot_request = 0u;
	if ((player_status[0] | player_2_status | player_3_status | player_status_spare) == 0u)
	{
		saved_game_mode = game_mode;
		game_mode = 7u;
		game_mode_timer = 0;
		run_game_mode(pressed, position, speed, next_scroll_position, object, &speed_value, &count, &cycle, &list_cursor);
		*p_speed_value = speed_value;
		*p_count = count;
		*p_cycle = cycle;
		*p_list_cursor = list_cursor;
		return;
	}
	check_stage_clear(&count, &list_cursor, &object);
	if (game_mode != 0x15u)
	{
		run_game_mode(pressed, position, speed, next_scroll_position, object, &speed_value, &count, &cycle, &list_cursor);
		*p_speed_value = speed_value;
		*p_count = count;
		*p_cycle = cycle;
		*p_list_cursor = list_cursor;
		return;
	}
	scroll_mode = 1u;
	update_scroll(&position, &speed, &speed_value, &ignored, &count, &cycle, &ignored, &object);
	update_stage_tile_layers(&ignored, &position, &speed, &speed_value, &ignored, &count, &cycle, &ignored, &object);
	find_nearby_floor_blocks(&ignored, &speed, &speed_value, &ignored, &count, &ignored, &object);
	speed_value = update_restart_point(speed_value);
	spawn_objects_by_type(&ignored, &position, &speed, &speed_value, &ignored, &count, &object);
	spawn_ring_rows(&ignored, &position, &ignored, &object);
	spawn_walls(&ignored, &position, &ignored, &count, &cycle, &object);
	spawn_object_groups_2(&ignored, &count, &object);
	spawn_object_groups(&ignored, &position, &ignored, &count, &object);
	spawn_objects_by_handler(&ignored, &ignored, &object);
	ignored = check_zones(ignored);
	check_hits(&ignored, &position, &ignored, &speed_value, &ignored, &count, &ignored);
	check_walls(ignored);
	check_platforms(&ignored, &ignored, &ignored, &speed_value, &ignored, &count, &cycle, &platform, &ignored);
	*p_speed_value = speed_value;
	*p_count = count;
	*p_cycle = cycle;
	*p_list_cursor = platform;
}

void game_mode_event_16(uint32_t box_edge, uint32_t position, uint32_t in_play_count, uint16_t next_scroll_position, uint32_t object, uint32_t *p_speed_value, uint32_t *p_count, uint32_t *p_cycle, uint32_t *p_list_cursor)
{
	uint32_t speed_value = *p_speed_value;
	uint32_t count = *p_count;
	uint32_t cycle = *p_cycle;
	uint32_t list_cursor = *p_list_cursor;
	uint32_t ignored = 0, platform = 0;

	current_game_mode = 0x16u;
	update_player_summary(&in_play_count, &count, &list_cursor, &object);
	if ((player_status[0] | player_2_status | player_3_status | player_status_spare) == 0u)
	{
		saved_game_mode = game_mode;
		game_mode = 7u;
		game_mode_timer = 0;
		run_game_mode(box_edge, position, in_play_count, next_scroll_position, object, &speed_value, &count, &cycle, &list_cursor);
		*p_speed_value = speed_value;
		*p_count = count;
		*p_cycle = cycle;
		*p_list_cursor = list_cursor;
		return;
	}
	find_nearby_floor_blocks(&box_edge, &in_play_count, &speed_value, &ignored, &count, &ignored, &object);
	speed_value = update_restart_point(speed_value);
	spawn_objects_by_type(&box_edge, &position, &in_play_count, &speed_value, &ignored, &count, &object);
	spawn_ring_rows(&box_edge, &position, &ignored, &object);
	spawn_walls(&box_edge, &position, &ignored, &count, &cycle, &object);
	spawn_object_groups_2(&box_edge, &count, &object);
	spawn_object_groups(&box_edge, &position, &ignored, &count, &object);
	spawn_objects_by_handler(&box_edge, &ignored, &object);
	box_edge = check_zones(box_edge);
	check_hits(&box_edge, &position, &ignored, &speed_value, &ignored, &count, &ignored);
	check_walls(box_edge);
	check_platforms(&ignored, &ignored, &ignored, &speed_value, &ignored, &count, &cycle, &platform, &ignored);
	if (scroll_resume_request == 0u)
	{
		*p_speed_value = speed_value;
		*p_count = count;
		*p_cycle = cycle;
		*p_list_cursor = platform;
		return;
	}
	scroll_resume_request = 0u;
	scroll_mode = 7u;
	game_mode = 3u;
	*p_speed_value = speed_value;
	*p_count = count;
	*p_cycle = cycle;
	*p_list_cursor = platform;
}

void game_mode_event_18(uint32_t position, uint32_t speed, uint32_t object, uint32_t *p_speed_value, uint32_t *p_count, uint32_t *p_cycle, uint32_t *p_list_cursor)
{
	uint32_t speed_value = *p_speed_value;
	uint32_t count = *p_count;
	uint32_t cycle = *p_cycle;
	uint32_t platform = 0;

	current_game_mode = 0x18u;
	update_ending(position, speed, object, &speed_value, &count, &cycle, &platform);
	*p_speed_value = speed_value;
	*p_count = count;
	*p_cycle = cycle;
	*p_list_cursor = platform;
}

void game_mode_event_19(uint32_t pressed, uint32_t position, uint32_t in_play_count, uint16_t next_scroll_position, uint32_t object, uint32_t *p_speed_value, uint32_t *p_count, uint32_t *p_cycle, uint32_t *p_list_cursor)
{
	uint32_t speed_value = *p_speed_value;
	uint32_t count = *p_count;
	uint32_t cycle = *p_cycle;
	uint32_t list_cursor = *p_list_cursor;
	uint32_t platform = 0, player_block = 0, player_object = 0, players_left = 0, status_cursor = 0;
	uint8_t event_19_status = 0;

	current_game_mode = 0x19u;
	current_game_mode = 0x19u;
	next_stage_slot_request = 0u;
	update_player_summary(&in_play_count, &count, &list_cursor, &object);
	if ((player_status[0] | player_2_status | player_3_status | player_status_spare) == 0u)
	{
		saved_game_mode = game_mode;
		game_mode = 7u;
		game_mode_timer = 0;
		run_game_mode(pressed, position, in_play_count, next_scroll_position, object, &speed_value, &count, &cycle, &list_cursor);
		*p_speed_value = speed_value;
		*p_count = count;
		*p_cycle = cycle;
		*p_list_cursor = list_cursor;
		return;
	}
	status_cursor = player_status_ADDRESS;
	player_object = player_status_ADDRESS;
	player_block = PLAYER_BLOCK(1);
	players_left = 3u;
	for (;;)
	{
		event_19_status = mem_read8(status_cursor);
		status_cursor++;
		if (event_19_status == 0xFFu)
		{
			player_object = LINKED(player_object, PLAYER(player_block)->object);
			if (OBJECT(player_object)->x_int > 0x17C)
				break;
		}
		players_left--;
		player_block += 0x200u;
		if (players_left == 0u)
		{
			event_19_check_rope_bridge(position, in_play_count, player_block, &speed_value, &players_left, &cycle, &platform);
			*p_speed_value = speed_value;
			*p_count = players_left;
			*p_cycle = cycle;
			*p_list_cursor = platform;
			return;
		}
	}
	barred_gate_closing = 0xFFu;
	event_19_check_rope_bridge(position, in_play_count, player_block, &speed_value, &players_left, &cycle, &platform);
	*p_speed_value = speed_value;
	*p_count = players_left;
	*p_cycle = cycle;
	*p_list_cursor = platform;
}

void game_mode_event_1C(uint32_t pressed, uint32_t position, uint32_t in_play_count, uint16_t next_scroll_position, uint32_t object, uint32_t *p_speed_value, uint32_t *p_count, uint32_t *p_cycle, uint32_t *p_list_cursor)
{
	uint32_t speed_value = *p_speed_value;
	uint32_t count = *p_count;
	uint32_t cycle = *p_cycle;
	uint32_t list_cursor = *p_list_cursor;
	uint32_t ignored = 0, platform = 0;

	current_game_mode = 0x1Cu;
	if (scene_step == 0u)
	{
		scene_step = 1u;
		event_1c_riding = 0u;
	}
	next_stage_slot_request = 0u;
	update_player_summary(&in_play_count, &count, &list_cursor, &object);
	if ((player_status[0] | player_2_status | player_3_status | player_status_spare) == 0u)
	{
		saved_game_mode = game_mode;
		game_mode = 7u;
		game_mode_timer = 0;
		run_game_mode(pressed, position, in_play_count, next_scroll_position, object, &speed_value, &count, &cycle, &list_cursor);
		*p_speed_value = speed_value;
		*p_count = count;
		*p_cycle = cycle;
		*p_list_cursor = list_cursor;
		return;
	}
	switch (scene_step)
	{
	case 0:
		event_1c_follow();
		break;
	case 1:
		event_1c_follow();
		break;
	case 2:
		event_1c_stop();
		break;
	case 3:
		event_1c_scroll();
		break;
	case 4:
		event_1c_follow();
		break;
	default:
		invalid_jump(0x9510Eu + scene_step * 2u);
	}
	update_scroll(&position, &in_play_count, &speed_value, &ignored, &count, &cycle, &ignored, &object);
	update_stage_tile_layers(&ignored, &position, &in_play_count, &speed_value, &ignored, &count, &cycle, &ignored, &object);
	find_nearby_floor_blocks(&ignored, &in_play_count, &speed_value, &ignored, &count, &ignored, &object);
	speed_value = update_restart_point(speed_value);
	spawn_objects_by_type(&ignored, &position, &in_play_count, &speed_value, &ignored, &count, &object);
	spawn_ring_rows(&ignored, &position, &ignored, &object);
	spawn_walls(&ignored, &position, &ignored, &count, &cycle, &object);
	spawn_object_groups_2(&ignored, &count, &object);
	spawn_object_groups(&ignored, &position, &ignored, &count, &object);
	spawn_objects_by_handler(&ignored, &ignored, &object);
	ignored = check_zones(ignored);
	check_hits(&ignored, &position, &ignored, &speed_value, &ignored, &count, &ignored);
	check_walls(ignored);
	check_platforms(&ignored, &ignored, &ignored, &speed_value, &ignored, &count, &cycle, &platform, &ignored);
	if (scroll_position < 0x84A0u)
	{
		*p_speed_value = speed_value;
		*p_count = count;
		*p_cycle = cycle;
		*p_list_cursor = platform;
		return;
	}
	game_mode = 3u;
	scroll_speed_low = 5;
	bridge_0_plank_count = 0u;
	*p_speed_value = speed_value;
	*p_count = count;
	*p_cycle = cycle;
	*p_list_cursor = platform;
}

void game_mode_event_1F(uint32_t pressed, uint32_t position, uint32_t in_play_count, uint16_t next_scroll_position, uint32_t object, uint32_t *p_speed_value, uint32_t *p_count, uint32_t *p_cycle, uint32_t *p_list_cursor)
{
	uint32_t speed_value = *p_speed_value;
	uint32_t count = *p_count;
	uint32_t cycle = *p_cycle;
	uint32_t list_cursor = *p_list_cursor;
	uint8_t sound_id = 0;
	uint32_t exit_status_cursor = 0, finished_flags = 0, ignored = 0;

	current_game_mode = 0x1Fu;
	if (scene_step == 0u)
	{
		scene_step = 0xFFu;
		unused_escape_byte = 0u;
		escape_alarm_timer = 0u;
		start_escape_countdown(&position, &count, &object);
	}
	next_stage_slot_request = 0u;
	update_player_summary(&in_play_count, &count, &list_cursor, &object);
	if ((player_status[0] | player_2_status | player_3_status | player_status_spare) == 0u)
	{
		saved_game_mode = game_mode;
		game_mode = 7u;
		game_mode_timer = 0;
		run_game_mode(pressed, position, in_play_count, next_scroll_position, object, &speed_value, &count, &cycle, &list_cursor);
		*p_speed_value = speed_value;
		*p_count = count;
		*p_cycle = cycle;
		*p_list_cursor = list_cursor;
		return;
	}
	check_stage_clear(&count, &list_cursor, &object);
	if (game_mode != 0x1Fu)
	{
		run_game_mode(pressed, position, in_play_count, next_scroll_position, object, &speed_value, &count, &cycle, &list_cursor);
		*p_speed_value = speed_value;
		*p_count = count;
		*p_cycle = cycle;
		*p_list_cursor = list_cursor;
		return;
	}
	update_scroll(&position, &in_play_count, &speed_value, &ignored, &count, &cycle, &ignored, &object);
	update_stage_tile_layers(&ignored, &position, &in_play_count, &speed_value, &ignored, &count, &cycle, &ignored, &object);
	find_nearby_floor_blocks(&ignored, &in_play_count, &speed_value, &ignored, &count, &ignored, &object);
	speed_value = update_restart_point(speed_value);
	spawn_objects_by_type(&ignored, &position, &in_play_count, &speed_value, &ignored, &count, &object);
	spawn_ring_rows(&ignored, &position, &ignored, &object);
	spawn_walls(&ignored, &position, &ignored, &count, &cycle, &object);
	spawn_object_groups_2(&ignored, &count, &object);
	spawn_object_groups(&ignored, &position, &ignored, &count, &object);
	spawn_objects_by_handler(&ignored, &ignored, &object);
	ignored = check_zones(ignored);
	check_hits(&ignored, &position, &ignored, &speed_value, &ignored, &count, &object);
	check_walls(ignored);
	check_platforms(&ignored, &ignored, &ignored, &speed_value, &ignored, &count, &cycle, &finished_flags, &object);
	if (escape_started != 0u)
	{
		escape_alarm_timer++;
		if (escape_alarm_timer == 0x30Cu)
		{
			sound_id = 0x7Cu;
			play_sound(sound_id, &ignored, &ignored);
		}
		else if (escape_alarm_timer == 1u)
		{
			start_escape_alarm_flash(object);
			camera_shake_active = 1u;
			sound_id = 0xFu;
			play_sound(sound_id, &ignored, &ignored);
			sound_id = 0x7Cu;
			play_sound(sound_id, &ignored, &ignored);
		}
	}
	if (stage_exit_started == 0u)
	{
		if (no_player_in_play == 0u)
			update_escape_countdown();
	}
	else
	{
		exit_status_cursor = player_status_ADDRESS;
		finished_flags = AS32(players_finished[0]);
		count = (count & ~0xFFu) | 3u;
		do
		{
			if ((int8_t)mem_read8(exit_status_cursor) >= 0)
				finished_flags = (finished_flags & ~0xFFu) | 0xFFu;
			finished_flags = (finished_flags & ~0xFFFFu) | (uint16_t)((finished_flags << 8) | ((uint16_t)finished_flags >> 8));
			exit_status_cursor++;
			count--;
		} while (count != 0u);
		if (finished_flags != 0xFFFFFFFFu)
		{
			if (no_player_in_play == 0u)
				update_escape_countdown();
			if (escape_seconds != 0u)
			{
				*p_speed_value = speed_value;
				*p_count = count;
				*p_cycle = cycle;
				*p_list_cursor = finished_flags;
				return;
			}
			game_mode = 0x20u;
			game_mode_timer = 0;
			*p_speed_value = speed_value;
			*p_count = count;
			*p_cycle = cycle;
			*p_list_cursor = finished_flags;
			return;
		}
	}
	if (escape_seconds != 0u)
	{
		*p_speed_value = speed_value;
		*p_count = count;
		*p_cycle = cycle;
		*p_list_cursor = finished_flags;
		return;
	}
	game_mode = 0x20u;
	game_mode_timer = 0;
	*p_speed_value = speed_value;
	*p_count = count;
	*p_cycle = cycle;
	*p_list_cursor = finished_flags;
}

void cleared_banner_wait(void)
{
	if (stage_name_shown != 0u)
		OBJECT(current_object)->handler = stage_clear_banner_reveal_ADDRESS;
}

void stage_clear_banner_reveal(void)
{
	uint16_t sparkle_offset = 0;
	uint32_t at_last_step = 0, ignored = 0;

	if ((uint8_t)OBJECT(current_object)->animation_timer == OBJECT(current_object)->animation_speed)
	{
		sparkle_offset = OBJECT(current_object)->stage_clear_banner.sparkle_offset;
		spawn_banner_sparkle(sparkle_offset);
		OBJECT(current_object)->stage_clear_banner.sparkle_offset += 0x10u;
	}
	animate(&ignored, &ignored, &at_last_step);
	if ((uint8_t)at_last_step == 0u)
	{
		stage_clear_banner_hold();
		return;
	}
	OBJECT(current_object)->handler = stage_clear_banner_hold_ADDRESS;
	if ((OBJECT(current_object)->flags & 1u) != 0u)
	{
		cleared_shown = 0xFFu;
		stage_clear_banner_hold();
		return;
	}
	stage_name_shown = 0xFFu;
	stage_clear_banner_hold();
}

void stage_clear_banner_hold(void)
{
	uint8_t sound_id = 0;
	uint32_t depth = 0, free_count = 0, ignored = 0, object = 0;
	uint16_t letters_left = 0;

	if (banner_break == 0u)
	{
		if ((int8_t)OBJECT(current_object)->animation_frame < 0)
			return;
		depth = 0xFFu;
		queue_sprite_at_depth(&ignored, &depth);
		return;
	}
	sound_id = 0x1Bu;
	play_sound(sound_id, &ignored, &ignored);
	banner_letters_left = ROM16(OBJECT(current_object)->animation);
	banner_letter_offset_x = 8u;
	object = current_object;
	do
	{
		OBJECT(object)->sprite_flags = 0x1000u;
		if ((temp_1 & 1u) != 0u)
			OBJECT(object)->sprite_flags |= 0x80u;
		OBJECT(object)->sprite_palette = 0x8980u;
		OBJECT(object)->flags = 0u;
		OBJECT(object)->handler = banner_letter_spin_ADDRESS;
		OBJECT(object)->kind = (uint8_t)banner_letters_left;
		OBJECT(object)->animation = banner_letter_animation_ADDRESS;
		OBJECT(object)->animation_frame = 0xFFu;
		OBJECT(object)->animation_timer = 0;
		OBJECT(object)->animation_speed = 2u;
		OBJECT(object)->screen_x = OBJECT(current_object)->screen_x;
		OBJECT(object)->screen_x += banner_letter_offset_x;
		banner_letter_offset_x += 0x10u;
		OBJECT(object)->screen_y = OBJECT(current_object)->screen_y;
		letters_left = banner_letters_left;
		banner_letters_left = letters_left - 1u;
		if ((int16_t)letters_left <= 1)
			break;
		allocate_object(&free_count, &object);
	} while (free_count != 0u);
	if ((int8_t)OBJECT(current_object)->animation_frame < 0)
		return;
	depth = 0xFFu;
	queue_sprite_at_depth(&ignored, &depth);
}

void spawn_banner_sparkle(uint16_t offset_x)
{
	uint32_t free_count = 0, object = 0;

	sparkle_offset_x = offset_x;
	allocate_object(&free_count, &object);
	if (free_count != 0u)
	{
		OBJECT(object)->sprite_flags = 0x1000u;
		OBJECT(object)->sprite_palette = 0x81B0u;
		OBJECT(object)->flags = 0u;
		OBJECT(object)->handler = banner_sparkle_animate_ADDRESS;
		OBJECT(object)->animation = ring_sparkle_animation_ADDRESS;
		OBJECT(object)->animation_frame = 0xFFu;
		OBJECT(object)->animation_timer = 0;
		OBJECT(object)->animation_speed = 2u;
		OBJECT(object)->screen_x = OBJECT(current_object)->screen_x;
		OBJECT(object)->screen_x += sparkle_offset_x;
		OBJECT(object)->screen_y = OBJECT(current_object)->screen_y;
	}
}

void banner_sparkle_animate(void)
{
	uint32_t depth = 0, ignored = 0, wrapped = 0;

	animate(&ignored, &wrapped, &ignored);
	if ((uint8_t)wrapped == 0u)
	{
		depth = 0xFFu;
		queue_sprite_at_depth(&ignored, &depth);
		return;
	}
	delete_current_object();
}

void banner_letter_spin(void)
{
	uint32_t depth = 0, free_count = 0, ignored = 0, object = 0, wrapped = 0;

	if (OBJECT(current_object)->animation_speed == (uint8_t)OBJECT(current_object)->animation_timer && OBJECT(current_object)->animation_frame == 0xCu && (OBJECT(current_object)->kind & 1u) != 0u)
	{
		allocate_object(&free_count, &object);
		if (free_count == 0u)
		{
			animate(&ignored, &wrapped, &ignored);
			if ((uint8_t)wrapped == 0u)
			{
				depth = 0xFFu;
				queue_sprite_at_depth(&ignored, &depth);
				return;
			}
			delete_current_object();
			return;
		}
		OBJECT(object)->sprite_flags = 0x1000u;
		if ((OBJECT(current_object)->kind & 2u) != 0u)
			OBJECT(object)->sprite_flags |= 0x80u;
		OBJECT(object)->sprite_palette = 0x8990u;
		OBJECT(object)->flags = 0u;
		OBJECT(object)->handler = banner_sparkle_animate_ADDRESS;
		OBJECT(object)->animation = banner_sparkle_animation_ADDRESS;
		OBJECT(object)->animation_frame = 0xFFu;
		OBJECT(object)->animation_timer = 0;
		OBJECT(object)->animation_speed = 2u;
		OBJECT(object)->screen_x = OBJECT(current_object)->screen_x;
		OBJECT(object)->screen_y = OBJECT(current_object)->screen_y;
	}
	animate(&ignored, &wrapped, &ignored);
	if ((uint8_t)wrapped == 0u)
	{
		depth = 0xFFu;
		queue_sprite_at_depth(&ignored, &depth);
		return;
	}
	delete_current_object();
}

void apply_stage_layer_blending(uint32_t *p_layer_bit, uint32_t *p_count)
{
	uint32_t layer_bit = *p_layer_bit;
	uint16_t blend_mask = 0;
	uint32_t count = 0, cursor = 0, layer_shadow = 0;

	cursor = mixer_text_blend_ADDRESS;
	count = 7u;
	do
	{
		mem_write32(cursor, mem_read32(cursor) & ~0x1000u);
		cursor += 2u;
		count--;
	} while (count != 0u);
	cursor = stage_layer_blending_ADDRESS + stage_layer_blending[stage];
	if (ROM8(cursor) == 0u)
	{
		*p_count = count;
		return;
	}
	count = ROM8(cursor);
	cursor++;
	do
	{
		blend_mask = ROM8(cursor);
		cursor++;
		layer_bit = ROM8(cursor);
		cursor++;
		layer_shadow = GAME_VARIABLES + ROM16(cursor);
		mem_write16(layer_shadow, mem_read16(layer_shadow) & ~0x3Fu);
		mem_write16(layer_shadow, mem_read16(layer_shadow) | blend_mask);
		mem_write32(layer_shadow, mem_read32(layer_shadow) | shl32(1u, (int)(layer_bit)));
		mixer_blend &= ~0xF00u;
		cursor += 2u;
		mixer_blend |= ROM16(cursor);
		mixer_blend |= 0x800u;
		count--;
		cursor += 2u;
	} while (count != 0u);
	*p_layer_bit = layer_bit;
	*p_count = count;
}

void check_stage_clear(uint32_t *p_count, uint32_t *p_finished_flag, uint32_t *p_status)
{
	uint32_t count = *p_count;
	uint32_t finished_flag = *p_finished_flag;
	uint32_t status = *p_status;
	uint8_t next_game_mode = 0;

	if (scroll_position < goal_scroll_position || player_status_spare != 0u || stage_exit_started == 0u)
		return;
	finished_flag = players_finished_ADDRESS;
	status = player_status_ADDRESS;
	count = (count & ~0xFFu) | 3u;
	do
	{
		if (mem_read8(status) == 0u || mem_read8(status) == 0x82u || mem_read8(status) == 3u || mem_read8(status) == 4u || mem_read8(status) == 2u)
			mem_write8(finished_flag, 0xFFu);
		finished_flag++;
		status++;
		count--;
	} while (count != 0u);
	if (AS32(players_finished[0]) != 0xFFFFFFFFu)
	{
		*p_count = count;
		*p_finished_flag = finished_flag;
		*p_status = status;
		return;
	}
	game_mode_timer = 0;
	next_game_mode = 0x17u;
	if (stage == 0xBu || stage == 0xCu || stage == 0xDu || stage == 0xEu || stage == 0xFu)
		next_game_mode = 0x1Eu;
	game_mode = next_game_mode;
	*p_count = count;
	*p_finished_flag = finished_flag;
	*p_status = status;
}

uint32_t clear_player_blocks(void)
{
	uint32_t count = 0, dest = 0, fill_word = 0;

	dest = PLAYER_BLOCK(1);
	count = 0x30u;
	fill_word = 0u;
	do
	{
		mem_write32(dest, fill_word);
		dest += 4u;
		mem_write32(dest, fill_word);
		dest += 4u;
		mem_write32(dest, fill_word);
		dest += 4u;
		mem_write32(dest, fill_word);
		dest += 4u;
		mem_write32(dest, fill_word);
		dest += 4u;
		mem_write32(dest, fill_word);
		dest += 4u;
		mem_write32(dest, fill_word);
		dest += 4u;
		mem_write32(dest, fill_word);
		count--;
		dest += 4u;
	} while (count != 0u);
	return count;
}

uint32_t update_grey_screen(void)
{
	uint32_t count = 0, player_block = 0;

	grey_screen_ended = 0u;
	player_block = 0x208200u;
	count = 3u;
	do
	{
		player_block += 0x200u;
		count--;
	} while ((count != 0u) & (PLAYER(player_block)->grey_screen_request == 0u));
	if (PLAYER(player_block)->grey_screen_request != 0u)
	{
		grey_screen_active = 0xFFu;
		mixer_offset_select |= 0x1Eu;
		return count;
	}
	if (grey_screen_active != 0u)
	{
		grey_screen_ended = 0xFFu;
		grey_screen_active = 0u;
		mixer_offset_select &= ~0x1Eu;
	}
	return count;
}

void update_player_summary(uint32_t *p_in_play_count, uint32_t *p_count, uint32_t *p_cursor, uint32_t *p_player_object)
{
	uint8_t in_play_count = (uint8_t)*p_in_play_count;
	uint32_t count = *p_count;
	uint32_t cursor = *p_cursor;
	uint32_t player_object = *p_player_object;
	uint32_t zero = 0, negative = 0;
	uint32_t object = 0, player_block = 0, players_left = 0, status_cursor = 0;
	uint8_t drop_in_status = 0, in_play_status = 0, status = 0;
	uint32_t init_flags = 0;

	init_flags = stage_init_flags;
	stage_init_flags = init_flags | 0x800u;
	if ((init_flags & 0x800u) == 0u)
		unused_summary_byte = 0u;
	player_dropping_in = 0u;
	no_player_in_play = 0u;
	player_mishap = 0u;
	if (skip_player_summary != 0u)
		return;
	status_cursor = player_status_ADDRESS;
	players_left = 3u;
	do
	{
		drop_in_status = mem_read8(status_cursor);
		zero = (uint8_t)(drop_in_status + 0x7Fu) == 0u;
		status_cursor++;
		players_left--;
	} while ((players_left != 0u) & (drop_in_status != 0x81u));
	if (zero == 0u)
	{
		if (player_dropping_in_before != 0u)
			scroll_mode = saved_scroll_mode;
	}
	else
	{
		player_dropping_in = 0xFFu;
		if (player_dropping_in_before == 0u || scroll_mode != 0u)
			saved_scroll_mode = scroll_mode;
		scroll_mode = 0u;
		if (ROM16(scroll_mode_script - 4u) != 0u)
			scroll_speed_low = 5;
	}
	player_dropping_in_before = player_dropping_in;
	status_cursor = player_status_ADDRESS;
	players_left = 3u;
	do
	{
		in_play_status = mem_read8(status_cursor);
		negative = in_play_status >> 7;
		status_cursor++;
		players_left--;
	} while ((players_left != 0u) & ((int8_t)in_play_status >= 0));
	if (negative == 0u)
		no_player_in_play = 0xFFu;
	player_block = PLAYER_BLOCK(1);
	object = PLAYER_BLOCK(1);
	players_left = 3u;
	for (;;)
	{
		object = LINKED(object, PLAYER(player_block)->object);
		if ((OBJECT(object)->flags & 0x400u) != 0u || PLAYER(player_block)->mishap_flag == 0u)
		{
			players_left--;
			player_block += 0x200u;
			if (players_left == 0u)
				break;
			continue;
		}
		player_mishap = 0xFFu;
		player_1_mishap_flag = 0u;
		player_2_mishap_flag = 0u;
		player_3_mishap_flag = 0u;
		break;
	}
	count = 3u;
	in_play_count = 0u;
	cursor = player_status_ADDRESS;
	do
	{
		status = mem_read8(cursor);
		cursor++;
		if ((int8_t)status < 0)
			in_play_count++;
		count--;
	} while (count != 0u);
	players_in_play = in_play_count;
	*p_in_play_count = (*p_in_play_count & 0xFFFFFF00u) | in_play_count;
	*p_count = count;
	*p_cursor = cursor;
	*p_player_object = object;
}

void continue_to_next_stage_slot(uint32_t *p_table_offset, uint32_t *p_link_record, uint32_t *p_block)
{
	uint32_t table_offset = *p_table_offset;
	uint32_t link_record = *p_link_record;
	uint32_t block = *p_block;
	uint32_t bounds_list = 0, current_stage = 0, from_block = 0, from_record = 0, link_point = 0, to_block = 0;

	if (next_stage_slot_request == 0u)
		return;
	link_point = stage_link_points_ADDRESS + stage_link_points[stage];
	from_block = ROM16(link_point);
	from_record = floor_blocks + 2u + from_block;
	stage_link_from_block = from_block;
	stage_link_from_x = ROM32(from_record);
	stage_link_from_y = ROM32(from_record + 8u);
	stage_link_from_height = ROM16(from_record + 0x10u);
	current_stage = stage;
	block = stage_floor_blocks_ADDRESS + stage_floor_blocks[(uint16_t)(current_stage + 1u)];
	to_block = ROM16(link_point + 2u);
	block = block + 2u + to_block;
	stage_link_to_block = to_block;
	stage_link_to_x = ROM32(block);
	stage_link_to_y = ROM32(block + 8u);
	stage_link_to_height = ROM16(block + 0x10u);
	stage++;
	load_stage_map_pointers(&table_offset, &bounds_list);
	stage_init_flags &= 0xFFFFFFF0u;
	*p_table_offset = table_offset;
	*p_link_record = bounds_list;
	*p_block = block;
}

void load_stage_map_pointers(uint32_t *p_table_offset, uint32_t *p_bounds_list)
{
	uint32_t bounds_list = 0, stage_index = 0, table_offset = 0;

	current_restart_point = stage_restart_points_ADDRESS + stage_restart_points[stage];
	stage_index = stage;
	floor_blocks = stage_floor_blocks_ADDRESS + stage_floor_blocks[stage_index];
	table_offset = stage_floor_block_bounds[stage_index];
	bounds_list = stage_floor_block_bounds_ADDRESS + table_offset;
	floor_block_bounds = bounds_list;
	*p_table_offset = table_offset;
	*p_bounds_list = bounds_list;
}

void run_stage_events(uint32_t *p_table_offset, uint32_t *p_event)
{
	uint32_t table_offset = *p_table_offset;
	uint16_t distance_past = 0;
	uint32_t event = 0;
	uint32_t init_flags = 0;

	init_flags = stage_init_flags;
	stage_init_flags = init_flags | 0x100u;
	if ((init_flags & 0x100u) == 0u)
	{
		table_offset = stage_events[stage];
		stage_event_cursor = stage_events_ADDRESS + table_offset;
	}
	event = stage_event_cursor;
	if (scroll_position < ROM16(event))
	{
		*p_table_offset = table_offset;
		*p_event = event;
		return;
	}
	distance_past = (uint16_t)(scroll_position - ROM16(event));
	event += 2u;
	if (stage == 0x11u || distance_past < 0x100u)
	{
		game_mode = ROM8(event);
		scene_step = 0u;
		scene_timer = 0;
	}
	stage_event_cursor += 4u;
	*p_table_offset = table_offset;
	*p_event = event;
}

void reset_trackball_step(uint32_t player_number)
{
	uint32_t ignored = 0;

	trackball_step_sums[player_number] = 0u;
	read_trackball_step(&player_number, &ignored);
}

void read_trackball_step(uint32_t *p_player_number, uint32_t *p_trackball)
{
	uint32_t player_number = *p_player_number;
	uint16_t step = 0;
	uint32_t step_sum = 0, trackball = 0;

	trackball = trackball_1_x_ADDRESS + player_number * 4u;
	step_sum = trackball_step_sums_ADDRESS + player_number * 2u;
	if ((int16_t)(mem_read16(step_sum) ^ TRACKBALL_MOTION(trackball)->x) < 0)
		mem_write16(step_sum, 0u);
	mem_write16(step_sum, mem_read16(step_sum) + TRACKBALL_MOTION(trackball)->x);
	step = 0u;
	if ((int16_t)mem_read16(step_sum) >= 0x40)
	{
		step = 1u;
		mem_write16(step_sum, 0u);
		*p_player_number = (*p_player_number & 0xFFFF0000u) | step;
		*p_trackball = trackball;
		return;
	}
	if ((int16_t)mem_read16(step_sum) > -0x40)
	{
		*p_player_number = (*p_player_number & 0xFFFF0000u) | step;
		*p_trackball = trackball;
		return;
	}
	step = 0xFFFFu;
	mem_write16(step_sum, 0u);
	*p_player_number = (*p_player_number & 0xFFFF0000u) | step;
	*p_trackball = trackball;
}

uint32_t update_restart_point(uint32_t position)
{
	uint32_t restart_point = 0;

	restart_point = current_restart_point - 8u;
	position = (position & ~0xFFFFu) | scroll_position;
	do
	{
		restart_point += 8u;
	} while (ROM16(restart_point) <= (uint16_t)position);
	current_restart_point = restart_point;
	return position;
}

uint32_t start_player_objects_alt(uint32_t object)
{
	uint32_t ignored = 0, player_number = 0;

	player_handler_list = player_objects_alt_ADDRESS;
	reset_objects_from_list(object, &player_number, &ignored);
	return player_number;
}

uint32_t start_player_objects(uint32_t object)
{
	uint32_t handler_cursor = 0, ignored = 0;

	player_handler_list = player_objects_ADDRESS;
	reset_objects_from_list(object, &ignored, &handler_cursor);
	return handler_cursor;
}

void reset_objects_from_list(uint32_t object, uint32_t *p_player_number, uint32_t *p_handler_cursor)
{
	uint8_t status = 0;
	uint32_t free_count = 0, handler_cursor = 0, handlers_left = 0, ignored = 0, player_number = 0;
	uint32_t handler_count = 0;

	player_block_cursor = PLAYER_BLOCK(1);
	reset_objects(&ignored, &player_number);
	handler_cursor = object_handler_list;
	handlers_left = ROM16(handler_cursor);
	handler_cursor += 2u;
	object = allocate_object_or_start_list(object);
	OBJECT(object)->handler = ROM32(handler_cursor);
	handler_cursor += 4u;
	if (OBJECT(object)->handler == player_start_stage_ADDRESS)
	{
		if (mem_read8(player_block_cursor + 9u) == 0u)
		{
			status = player_status[0];
			if (status == 2u || status == 1u || status == 3u)
				OBJECT(object)->handler = player_resume_ADDRESS;
		}
		else
			OBJECT(object)->handler = player_join_ADDRESS;
	}
	player_block_cursor += 0x200u;
	handler_count = handlers_left;
	handlers_left = handler_count - 1u;
	if ((int32_t)handler_count <= 1)
	{
		*p_player_number = player_number;
		*p_handler_cursor = handler_cursor;
		return;
	}
	do
	{
		allocate_object(&free_count, &object);
		if (free_count == 0u)
		{
			*p_player_number = player_number;
			*p_handler_cursor = handler_cursor;
			return;
		}
		OBJECT(object)->handler = ROM32(handler_cursor);
		handler_cursor += 4u;
		if (OBJECT(object)->handler == player_start_stage_ADDRESS)
		{
			if (mem_read8(player_block_cursor + 9u) == 0u)
			{
				player_number = 1u;
				if (player_block_cursor != PLAYER_BLOCK(2))
					player_number = 2u;
				status = player_status[player_number];
				if (status == 2u || status == 1u || status == 3u)
					OBJECT(object)->handler = player_resume_ADDRESS;
			}
			else
				OBJECT(object)->handler = player_join_ADDRESS;
		}
		player_block_cursor += 0x200u;
		handlers_left--;
	} while (handlers_left != 0u);
	*p_player_number = player_number;
	*p_handler_cursor = handler_cursor;
}

void draw_game_over_banner(void)
{
	current_object = 0x209FC0u;
	banner_sprite_flags = 0x1000u;
	banner_sprite_x = 0xA0u;
	banner_sprite_y = 0x6Cu;
	banner_sprite_frame = 0x89A0u;
	banner_sprite_palette = 0x8970u;
	queue_sprite_on_top();
}

uint32_t blink_demo_prompt(uint32_t prompt_frame, uint32_t credit_block)
{
	uint8_t port_d = 0, shown = 0;

	if ((frame_counter & 0x1Fu) != 0u)
	{
		if ((current_screen == 4u || current_screen == 5u) && demo_prompt_shown != 0u)
		{
			current_object = 0x209FC0u;
			queue_sprite_on_top();
			return prompt_frame;
		}
		port_d = mem_read8(io_port_d);
		mem_write8(io_port_d, port_d & ~0xCu);
		return prompt_frame;
	}
	shown = ~demo_prompt_shown;
	demo_prompt_shown = shown;
	if (shown == 0u)
	{
		port_d = mem_read8(io_port_d);
		mem_write8(io_port_d, port_d & ~0xCu);
		return prompt_frame;
	}
	current_object = 0x209FC0u;
	banner_sprite_flags = 0x1000u;
	banner_sprite_x = 0xA0u;
	banner_sprite_y = 0x64u;
	banner_sprite_palette = 0x8960u;
	prompt_frame = (prompt_frame & ~0xFFFFu) | 0x89B8u;
	if (free_play == 0u && p1_credits < credits_per_start)
		prompt_frame = (prompt_frame & ~0xFFFFu) | 0x89A8u;
	OBJECT(current_object)->sprite_frame = prompt_frame;
	if ((current_screen == 4u || current_screen == 5u) && demo_prompt_shown != 0u)
	{
		current_object = 0x209FC0u;
		queue_sprite_on_top();
		return prompt_frame;
	}
	port_d = mem_read8(io_port_d);
	mem_write8(io_port_d, port_d & ~0xCu);
	return prompt_frame;
}
