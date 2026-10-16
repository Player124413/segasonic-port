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

void run_game_mode(uint32_t r1, uint32_t r2, uint32_t r3, uint32_t r5, uint32_t r11, uint32_t *p_r4, uint32_t *p_r8, uint32_t *p_r9, uint32_t *p_r10)
{
	uint32_t r4 = *p_r4;
	uint32_t r8 = *p_r8;
	uint32_t r9 = *p_r9;
	uint32_t r10 = *p_r10;
	uint8_t credits_needed = 0, mode = 0, next_game_mode = 0, playing_count = 0, sound_id = 0;
	uint16_t attribute = 0, cell = 0, kind = 0, y = 0;
	uint32_t r8_2 = 0, r9_2 = 0, r10_2 = 0, r11_2 = 0, attribute_2 = 0, clear_dest = 0, count_text = 0, exit_status_cursor = 0, fill_word = 0, finished_flags = 0, free_count = 0, ignored = 0, intro_status_cursor = 0, new_stage = 0, player_block = 0, player_number = 0, player_object = 0, players_left = 0, record = 0, scroll_script = 0, status_cursor = 0, text = 0, text_position = 0, value = 0;
	uint8_t p2_status = 0, p3_status = 0, old_continue_count = 0, event_12_status = 0, event_14_status = 0, event_19_status = 0, new_game_cnt_bits = 0, game_over_cnt_bits = 0;
	uint16_t event_0d_timer = 0, event_13_timer = 0, event_14_timer = 0, old_clear_timer = 0, event_1a_timer = 0;
	uint32_t intro_init_flags = 0, stage_select_init_flags = 0, stage_clear_init_flags = 0, character_select_init_flags = 0, stage_results_init_flags = 0;

	for (;;)
	{
		switch (game_mode)
		{
		case 0:
		case 33:
			goto game_mode_start_game;
		case 1:
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
				start_stage_view(&ignored, &ignored, &r4, &ignored);
				stage_start_section = 6u;
				move_camera_to_start_section(&r2, &r3, &ignored);
				load_stage_map_pointers(&ignored, &ignored);
				load_stage_goal();
				find_nearby_floor_blocks(&ignored, &r3, &r4, &ignored, &r8, &ignored, &ignored);
				start_spawns_by_type();
				load_stage_palette(&r9, &ignored, &r11);
				current_game_mode = 1u;
				sound_id = stage_start_sounds[stage];
				play_sound(sound_id, &ignored, &r2);
			}
			update_scroll(&r2, &r3, &r4, &ignored, &r8, &r9, &ignored, &r11);
			update_stage_tile_layers(&ignored, &r2, &r3, &r4, &ignored, &r8, &r9, &ignored, &r11);
			find_nearby_floor_blocks(&ignored, &r3, &r4, &ignored, &r8, &ignored, &r11);
			r4 = update_restart_point(r4);
			spawn_objects_by_type(&ignored, &r2, &r3, &r4, &ignored, &r8, &r11);
			spawn_objects_by_handler(&ignored, &ignored, &r11);
			check_hits(&ignored, &r2, &ignored, &r4, &ignored, &r8, &ignored);
			check_walls(ignored);
			check_platforms(&ignored, &ignored, &ignored, &r4, &ignored, &r8, &r9, &ignored, &ignored);
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
							*p_r4 = r4;
							*p_r8 = r8;
							*p_r9 = r9;
							*p_r10 = intro_status_cursor;
							return;
						}
					}
				}
			}
			game_mode = 2u;
			system_mode_step = 0u;
			*p_r4 = r4;
			*p_r8 = r8;
			*p_r9 = r9;
			*p_r10 = intro_status_cursor;
			return;
		case 2:
			start_stage(r5, &r2, &r3, &r4, &r8_2, &r9_2, &r10_2, &r11);
			*p_r4 = r4;
			*p_r8 = r8_2;
			*p_r9 = r9_2;
			*p_r10 = r10_2;
			return;
		case 3:
		case 5:
		case 6:
			current_game_mode = 3u;
			next_stage_slot_request = 0u;
			update_player_summary(&r3, &r8, &r10, &r11);
			if (game_in_progress != 0u && joining_closed == 0u && stage == 0xBu)
			{
				joining_closed = 0xFFu;
				player_1_hud_message_shown = 0u;
				player_2_hud_message_shown = 0u;
				player_3_hud_message_shown = 0u;
			}
			if ((player_status[0] | player_2_status | player_3_status | player_status_spare) != 0u)
			{
				check_stage_clear(&r8, &r10, &r11);
				if (game_mode != 3u)
					continue;
				r3 = update_grey_screen();
				update_scroll(&r2, &r3, &r4, &r5, &r8, &r9, &ignored, &r11);
				update_stage_tile_layers(&r1, &r2, &r3, &r4, &r5, &r8, &r9, &ignored, &r11);
				find_nearby_floor_blocks(&r1, &r3, &r4, &r5, &r8, &ignored, &r11);
				continue_to_next_stage_slot(&r1, &ignored, &r11);
				r4 = update_restart_point(r4);
				spawn_objects_by_type(&r1, &r2, &r3, &r4, &r5, &r8, &r11);
				spawn_ring_rows(&r1, &r2, &r3, &r11);
				spawn_walls(&r1, &r2, &r5, &r8, &r9, &r11);
				spawn_object_groups_2(&r1, &r8, &r11);
				spawn_object_groups(&r1, &r2, &r3, &r8, &r11);
				spawn_objects_by_handler(&r1, &ignored, &r11);
				r1 = check_zones(r1);
				check_hits(&r1, &r2, &r3, &r4, &r5, &r8, &r11);
				r1 = check_walls(r1);
				check_platforms(&r1, &r2, &r3, &r4, &r5, &r8, &r9, &ignored, &r11);
				run_stage_events(&r1, &r10);
				if (game_mode == 3u)
				{
					*p_r4 = r4;
					*p_r8 = r8;
					*p_r9 = r9;
					*p_r10 = r10;
					return;
				}
				continue;
			}
			saved_game_mode = game_mode;
			game_mode = 7u;
			game_mode_timer = 0;
			continue;
		case 4:
			if (current_game_mode != 4u)
			{
				sound_id = 0x95u;
				play_sound(sound_id, &ignored, &ignored);
				sound_id = 3u;
				play_sound(sound_id, &ignored, &ignored);
			}
			current_game_mode = 4u;
			mixer_blend &= 0xF7FFu;
			r4 = run_map_screen(r4, r11);
			map_screen_active = 0xFFu;
			if (map_screen_step != 5u)
			{
				*p_r4 = r4;
				*p_r8 = r8;
				*p_r9 = r9;
				*p_r10 = r10;
				return;
			}
			game_mode = 2u;
			*p_r4 = r4;
			*p_r8 = r8;
			*p_r9 = r9;
			*p_r10 = r10;
			return;
		case 7:
			goto game_mode_game_over;
		case 8:
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
				r8 = 0xE00u;
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
					clear_dest += 4u;
					r8--;
				} while (r8 != 0u);
			}
			new_stage = 0u;
			read_trackball_step(&new_stage, &r10_2);
			if ((uint16_t)new_stage != 0u)
			{
				new_stage = (uint16_t)(new_stage + stage);
				if ((int16_t)new_stage < 0)
					new_stage = 0x11u;
				if ((int16_t)new_stage > 0x11)
					new_stage = 0u;
				stage = new_stage;
			}
			value = stage;
			attribute_2 = 0u;
			text_position = TEXT_CELL(24, 21);
			draw_hex_4(&value, &attribute_2, &text_position);
			if ((inputs_pressed & 0x10u) == 0u)
			{
				*p_r4 = r4;
				*p_r8 = r8;
				*p_r9 = r9;
				*p_r10 = r10_2;
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
			*p_r4 = r4;
			*p_r8 = r8;
			*p_r9 = r9;
			*p_r10 = text;
			return;
		case 9:
			update_scroll(&ignored, &r3, &r4, &ignored, &r8, &r9, &r10, &r11);
			update_stage_tile_layers(&ignored, &ignored, &ignored, &r4, &ignored, &r8, &r9, &r10, &r11);
			continue_to_next_stage_slot(&ignored, &r10, &ignored);
			find_nearby_floor_blocks(&ignored, &ignored, &r4, &ignored, &r8, &r10, &ignored);
			*p_r4 = r4;
			*p_r8 = r8;
			*p_r9 = r9;
			*p_r10 = r10;
			return;
		case 10:
		case 11:
			current_game_mode = 0xAu;
			if ((player_status[0] | player_2_status | player_3_status | player_status_spare) != 0u)
			{
				update_stage_05_event(r2, r3, r11, &r4, &r8, &r9, &r10);
				*p_r4 = r4;
				*p_r8 = r8;
				*p_r9 = r9;
				*p_r10 = r10;
				return;
			}
			saved_game_mode = game_mode;
			game_mode = 7u;
			game_mode_timer = 0;
			continue;
		case 12:
			current_game_mode = 0xCu;
			switch (scene_step)
			{
			case 0:
				event_0c_start(&r2, &r3, &r11);
				break;
			case 1:
				r2 = event_0c_first_quake(r2);
				break;
			case 2:
				event_0c_second_quake(r5, &r2, &r8, &r9, &r11);
				break;
			case 3:
				event_0c_finish();
				break;
			case 4:
				event_0c_start(&r2, &r3, &r11);
				break;
			default:
				invalid_jump(0x9487Cu + scene_step * 3u);
			}
			update_scroll(&r2, &r3, &r4, &ignored, &r8, &r9, &ignored, &r11);
			update_stage_tile_layers(&ignored, &r2, &ignored, &r4, &ignored, &r8, &r9, &ignored, &r11);
			r4 = update_restart_point(r4);
			check_hits(&ignored, &r2, &ignored, &r4, &ignored, &r8, &ignored);
			check_walls(ignored);
			check_platforms(&ignored, &ignored, &ignored, &r4, &ignored, &r8, &r9, &r10_2, &ignored);
			*p_r4 = r4;
			*p_r8 = r8;
			*p_r9 = r9;
			*p_r10 = r10_2;
			return;
		case 13:
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
			if (scene_step == 2u)
			{
			event_0d_start_scroll:
				scroll_mode = 0u;
				scroll_speed = 0x1Cu;
			}
			else
			{
				event_0d_timer = scene_timer;
				scene_timer = event_0d_timer - 1u;
				if ((int16_t)event_0d_timer <= 1)
				{
					scene_step++;
					goto event_0d_start_scroll;
				}
			}
			find_nearby_floor_blocks(&ignored, &r3, &r4, &ignored, &r8, &ignored, &r11);
			update_scroll(&r2, &r3, &r4, &ignored, &r8, &r9, &ignored, &r11);
			update_stage_tile_layers(&ignored, &r2, &r3, &r4, &ignored, &r8, &r9, &ignored, &r11);
			spawn_objects_by_type(&ignored, &r2, &r3, &r4, &ignored, &r8, &r11);
			spawn_ring_rows(&ignored, &r2, &ignored, &r11);
			spawn_walls(&ignored, &r2, &ignored, &r8, &r9, &r11);
			spawn_object_groups_2(&ignored, &r8, &r11);
			spawn_object_groups(&ignored, &r2, &ignored, &r8, &r11);
			spawn_objects_by_handler(&ignored, &ignored, &r11);
			check_hits(&ignored, &r2, &ignored, &r4, &ignored, &r8, &ignored);
			check_walls(ignored);
			check_platforms(&ignored, &ignored, &ignored, &r4, &ignored, &r8, &r9, &r10_2, &ignored);
			if (scroll_position < 0xA00u)
			{
				*p_r4 = r4;
				*p_r8 = r8;
				*p_r9 = r9;
				*p_r10 = r10_2;
				return;
			}
			game_mode = 3u;
			event_0d_active = 0u;
			players_frozen = 0u;
			scroll_mode = 7u;
			*p_r4 = r4;
			*p_r8 = r8;
			*p_r9 = r9;
			*p_r10 = r10_2;
			return;
		case 14:
			if (continue_count == 9u)
			{
				text = continue_text_ADDRESS;
				print_text(&text, &ignored);
			}
			current_game_mode = 0xEu;
			r1 = check_zones(r1);
			check_hits(&r1, &r2, &ignored, &r4, &ignored, &r8, &ignored);
			check_walls(r1);
			check_platforms(&ignored, &ignored, &ignored, &r4, &ignored, &r8, &r9, &r10_2, &ignored);
			if ((inputs_pressed & 0x10000u) != 0u || (inputs_pressed & 0x1000000u) != 0u || (inputs_pressed & 0x40000u) != 0u)
				continue_second_frames = 0x3C;
			continue_second_frames++;
			if (continue_second_frames < 0x3C)
			{
			game_mode_continue_draw_count:
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
						*p_r4 = r4;
						*p_r8 = r8;
						*p_r9 = r9;
						*p_r10 = r10_2;
						return;
					}
					break;
				case 2:
					if ((p2_credits == 0u || (inputs_pressed & 0x20u) == 0u) && (p1_credits == 0u || (inputs_pressed & 0x10u) == 0u))
					{
						*p_r4 = r4;
						*p_r8 = r8;
						*p_r9 = r9;
						*p_r10 = r10_2;
						return;
					}
					break;
				case 3:
					if (p3_credits == 0u || (inputs_pressed & 0x80u) == 0u)
					{
						if ((p2_credits == 0u || (inputs_pressed & 0x20u) == 0u) && (p1_credits == 0u || (inputs_pressed & 0x10u) == 0u))
						{
							*p_r4 = r4;
							*p_r8 = r8;
							*p_r9 = r9;
							*p_r10 = r10_2;
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
						*p_r4 = r4;
						*p_r8 = r8;
						*p_r9 = r9;
						*p_r10 = count_text;
						return;
					}
					break;
				default:
					invalid_jump(0xBDCu + cabinet_config * 2u);
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
			}
			else
			{
				continue_second_frames = 0;
				old_continue_count = continue_count;
				continue_count = old_continue_count - 1u;
				if ((int8_t)old_continue_count > 1)
					goto game_mode_continue_draw_count;
				game_mode = 7u;
				game_mode_timer = 0;
			}
			text = continue_text_ADDRESS;
			erase_text(text);
			cell = 0u;
			count_text = 0x25C4u;
			text_position = TEXT_CELL(24, 14);
			erase_text_here(cell, &count_text, &text_position);
			*p_r4 = r4;
			*p_r8 = r8;
			*p_r9 = r9;
			*p_r10 = count_text;
			return;
		case 15:
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
				event_0f_show_messages(r5, &r8, &r11);
				break;
			case 3:
				event_0f_start();
				break;
			default:
				invalid_jump(0x94606u + scene_step * 3u);
			}
			update_scroll(&r2, &r3, &r4, &ignored, &r8, &r9, &ignored, &r11);
			update_stage_tile_layers(&ignored, &r2, &ignored, &r4, &ignored, &r8, &r9, &ignored, &r11);
			r4 = update_restart_point(r4);
			spawn_walls(&ignored, &r2, &ignored, &r8, &r9, &r11);
			check_hits(&ignored, &r2, &ignored, &r4, &ignored, &r8, &ignored);
			check_walls(ignored);
			check_platforms(&ignored, &ignored, &ignored, &r4, &ignored, &r8, &r9, &r10_2, &ignored);
			*p_r4 = r4;
			*p_r8 = r8;
			*p_r9 = r9;
			*p_r10 = r10_2;
			return;
		case 16:
			current_game_mode = 0x10u;
			switch (scene_step)
			{
			case 0:
			case 4:
				scene_step++;
				scene_timer = 0;
				scene_timer++;
				spawn_objects_by_handler(&ignored, &r10_2, &r11);
				if ((uint16_t)scene_timer == 0x168u)
					stage_06_background_step = 3u;
				update_stage_06_background(r3, r11, &r4, &r9, &r10_2);
				if (scene_timer >= 0xF0 && scene_timer <= 0x12B && (scene_timer & 7u) == 0u)
				{
					mixer_sprite_control |= 0x8000u;
					color_offset_red--;
					color_offset_green--;
					color_offset_blue--;
				}
				if (scene_timer < 0x1A4)
				{
					*p_r4 = r4;
					*p_r8 = r8;
					*p_r9 = r9;
					*p_r10 = r10_2;
					return;
				}
				scene_step++;
				stage_06_background_step = 5u;
				*p_r4 = r4;
				*p_r8 = r8;
				*p_r9 = r9;
				*p_r10 = r10_2;
				return;
			case 1:
				scene_timer++;
				spawn_objects_by_handler(&ignored, &r10_2, &r11);
				if ((uint16_t)scene_timer == 0x168u)
					stage_06_background_step = 3u;
				update_stage_06_background(r3, r11, &r4, &r9, &r10_2);
				if (scene_timer >= 0xF0 && scene_timer <= 0x12B && (scene_timer & 7u) == 0u)
				{
					mixer_sprite_control |= 0x8000u;
					color_offset_red--;
					color_offset_green--;
					color_offset_blue--;
				}
				if (scene_timer < 0x1A4)
				{
					*p_r4 = r4;
					*p_r8 = r8;
					*p_r9 = r9;
					*p_r10 = r10_2;
					return;
				}
				scene_step++;
				stage_06_background_step = 5u;
				*p_r4 = r4;
				*p_r8 = r8;
				*p_r9 = r9;
				*p_r10 = r10_2;
				return;
			case 2:
				scene_step++;
				scene_timer = 0;
				mixer_sprite_control &= ~0x8000u;
				color_offset_red = 0u;
				color_offset_green = 0u;
				color_offset_blue = 0u;
				scene_timer++;
				spawn_objects_by_handler(&ignored, &ignored, &r11);
				spawn_stage_06_lightning(&r10_2, &r11);
				update_stage_06_background(r3, r11, &r4, &r9, &r10_2);
				if (scene_timer < 0x12C)
				{
					*p_r4 = r4;
					*p_r8 = r8;
					*p_r9 = r9;
					*p_r10 = r10_2;
					return;
				}
				game_mode = 2u;
				*p_r4 = r4;
				*p_r8 = r8;
				*p_r9 = r9;
				*p_r10 = r10_2;
				return;
			case 3:
				scene_timer++;
				spawn_objects_by_handler(&ignored, &ignored, &r11);
				spawn_stage_06_lightning(&r10_2, &r11);
				update_stage_06_background(r3, r11, &r4, &r9, &r10_2);
				if (scene_timer < 0x12C)
				{
					*p_r4 = r4;
					*p_r8 = r8;
					*p_r9 = r9;
					*p_r10 = r10_2;
					return;
				}
				game_mode = 2u;
				*p_r4 = r4;
				*p_r8 = r8;
				*p_r9 = r9;
				*p_r10 = r10_2;
				return;
			default:
				invalid_jump(0x94510u + scene_step * 3u);
			}
			break;
		case 17:
			current_game_mode = 0x11u;
			if (stage == 4u)
				scroll_mode = 1u;
			update_scroll(&r2, &r3, &r4, &r5, &r8, &r9, &ignored, &r11);
			update_stage_tile_layers(&ignored, &r2, &r3, &r4, &r5, &r8, &r9, &ignored, &r11);
			find_nearby_floor_blocks(&ignored, &r3, &r4, &r5, &r8, &ignored, &r11);
			r4 = update_restart_point(r4);
			spawn_objects_by_type(&ignored, &r2, &r3, &r4, &r5, &r8, &r11);
			spawn_ring_rows(&ignored, &r2, &ignored, &r11);
			spawn_walls(&ignored, &r2, &r5, &r8, &r9, &r11);
			spawn_object_groups_2(&ignored, &r8, &r11);
			spawn_object_groups(&ignored, &r2, &ignored, &r8, &r11);
			spawn_objects_by_handler(&ignored, &ignored, &r11);
			ignored = check_zones(ignored);
			check_hits(&ignored, &r2, &ignored, &r4, &r5, &r8, &ignored);
			check_walls(ignored);
			check_platforms(&ignored, &ignored, &ignored, &r4, &r5, &r8, &r9, &r10_2, &ignored);
			update_screen_wipe(&ignored, &ignored, &ignored, &r5);
			if ((r5 & 0xFu) != 0xFu)
			{
				*p_r4 = r4;
				*p_r8 = r8;
				*p_r9 = r9;
				*p_r10 = r10_2;
				return;
			}
			game_mode = 3u;
			*p_r4 = r4;
			*p_r8 = r8;
			*p_r9 = r9;
			*p_r10 = r10_2;
			return;
		case 18:
			current_game_mode = 0x12u;
			if (scroll_position < 0x80B1u)
			{
			event_12_stop_scroll_at_80b1:
				if (scroll_position >= 0x80B1u)
					scroll_mode = 1u;
			event_12_stage_frame:
				next_stage_slot_request = 0u;
				update_player_summary(&r3, &r8, &r10, &r11);
				if ((player_status[0] | player_2_status | player_3_status | player_status_spare) != 0u)
				{
					if (scroll_mode == 7u && event_12_fast_scroll != 0u)
					{
						scroll_mode = 0u;
						scroll_speed = 0x20u;
					}
					update_scroll(&r2, &r3, &r4, &ignored, &r8, &r9, &ignored, &r11);
					update_stage_tile_layers(&ignored, &r2, &r3, &r4, &ignored, &r8, &r9, &ignored, &r11);
					find_nearby_floor_blocks(&ignored, &r3, &r4, &ignored, &r8, &ignored, &r11);
					r4 = update_restart_point(r4);
					spawn_objects_by_type(&ignored, &r2, &r3, &r4, &ignored, &r8, &r11);
					spawn_ring_rows(&ignored, &r2, &ignored, &r11);
					spawn_walls(&ignored, &r2, &ignored, &r8, &r9, &r11);
					spawn_object_groups_2(&ignored, &r8, &r11);
					spawn_object_groups(&ignored, &r2, &ignored, &r8, &r11);
					spawn_objects_by_handler(&ignored, &ignored, &r11);
					ignored = check_zones(ignored);
					check_hits(&ignored, &r2, &ignored, &r4, &ignored, &r8, &ignored);
					check_walls(ignored);
					check_platforms(&ignored, &ignored, &ignored, &r4, &ignored, &r8, &r9, &r10_2, &ignored);
					*p_r4 = r4;
					*p_r8 = r8;
					*p_r9 = r9;
					*p_r10 = r10_2;
					return;
				}
				saved_game_mode = game_mode;
				game_mode = 7u;
				game_mode_timer = 0;
				continue;
			}
			r3 &= ~0xFFu;
			r11 = PLAYER_BLOCK(1);
			r10 = player_status_ADDRESS;
			r8 = 3u;
			do
			{
				event_12_status = mem_read8(r10);
				r10++;
				if (event_12_status != 0xFFu || PLAYER(r11)->drift_done != 0u)
					r3 = (r3 & ~0xFFu) | (uint8_t)(r3 + 1u);
				r8--;
				r11 += 0x200u;
			} while (r8 != 0u);
			if ((uint8_t)r3 != 3u)
				goto event_12_stop_scroll_at_80b1;
			r5 = 0x644u;
			y = 0x149Eu;
			kind = 0xFu;
			r8 = (r8 & ~0xFFFFu) | 0x168u;
			spawn_sign((uint16_t)r5, y, kind, (uint16_t)r8, &r1, &r11);
			event_12_fast_scroll = 0u;
			scroll_mode = 7u;
			game_mode = 3u;
			continue;
		case 19:
			current_game_mode = 0x13u;
			switch (scene_step)
			{
			case 0:
			case 3:
				scene_step++;
				scene_timer = 0x78;
			case 1:
				scroll_mode = 1u;
				event_13_timer = scene_timer;
				scene_timer = event_13_timer - 1u;
				if ((int16_t)event_13_timer <= 1)
				{
					scene_step++;
					scroll_mode = 0u;
				}
				update_screen_wipe(&r1, &r2, &r3, &r5);
				goto event_12_stage_frame;
			case 2:
				if (scroll_position < 0x16ACu)
					goto event_12_stage_frame;
				game_mode = 3u;
				continue;
			default:
				invalid_jump(0x943A5u + scene_step * 2u);
			}
			break;
		case 20:
			current_game_mode = 0x14u;
			switch (scene_step)
			{
			case 0:
			case 3:
				scene_step++;
			event_14_stage_frame:
				if ((uint16_t)scene_timer == 0x3Cu)
				{
					r5 = 0xFDEFu;
					y = 0xAF0u;
					kind = 0xEu;
					r8 = (r8 & ~0xFFFFu) | 0x28u;
					spawn_sign((uint16_t)r5, y, kind, (uint16_t)r8, &r1, &r11);
				}
				update_player_summary(&r3, &r8, &r10, &r11);
				scroll_mode = 1u;
				if ((player_status[0] | player_2_status | player_3_status | player_status_spare) != 0u)
				{
					update_scroll(&r2, &r3, &r4, &ignored, &r8, &r9, &ignored, &r11);
					update_stage_tile_layers(&ignored, &r2, &r3, &r4, &ignored, &r8, &r9, &ignored, &r11);
					find_nearby_floor_blocks(&ignored, &r3, &r4, &ignored, &r8, &ignored, &r11);
					r4 = update_restart_point(r4);
					spawn_objects_by_type(&ignored, &r2, &r3, &r4, &ignored, &r8, &r11);
					spawn_ring_rows(&ignored, &r2, &ignored, &r11);
					spawn_walls(&ignored, &r2, &ignored, &r8, &r9, &r11);
					spawn_object_groups_2(&ignored, &r8, &r11);
					spawn_object_groups(&ignored, &r2, &ignored, &r8, &r11);
					spawn_objects_by_handler(&ignored, &ignored, &r11);
					ignored = check_zones(ignored);
					check_hits(&ignored, &r2, &ignored, &r4, &ignored, &r8, &ignored);
					check_walls(ignored);
					check_platforms(&ignored, &ignored, &ignored, &r4, &ignored, &r8, &r9, &r10_2, &ignored);
					*p_r4 = r4;
					*p_r8 = r8;
					*p_r9 = r9;
					*p_r10 = r10_2;
					return;
				}
				saved_game_mode = game_mode;
				game_mode = 7u;
				game_mode_timer = 0;
				continue;
			case 1:
				r3 &= ~0xFFu;
				playing_count = 3u;
				r10 = PLAYER_BLOCK(1);
				r11 = player_status_ADDRESS;
				r8 = 3u;
				do
				{
					event_14_status = mem_read8(r11);
					r11++;
					if (event_14_status == 0xFFu)
					{
						if (PLAYER(r10)->state == 0x25u && PLAYER(r10)->sequence_bounced == 0u)
							r3 = (r3 & ~0xFFu) | (uint8_t)(r3 + 1u);
					}
					else
						playing_count--;
					r8--;
					r10 += 0x200u;
				} while (r8 != 0u);
				if (playing_count == 0u || (uint8_t)r3 != 0u || free_object_stack < 2)
					goto event_14_stage_frame;
				scene_step++;
				allocate_object_outside_loop(&ignored, &r11);
				OBJECT(r11)->sprite_frame = 0u;
				OBJECT(r11)->handler = icicle_dropper_start_ADDRESS;
				allocate_object_outside_loop(&ignored, &r11);
				OBJECT(r11)->sprite_frame = 2u;
				OBJECT(r11)->handler = eggman_speech_start_ADDRESS;
				scene_timer = 0x12C;
			case 2:
				event_14_timer = scene_timer;
				scene_timer = event_14_timer - 1u;
				if ((int16_t)event_14_timer > 1)
					goto event_14_stage_frame;
				scroll_mode = 7u;
				game_mode = 3u;
				continue;
			default:
				invalid_jump(0x94253u + scene_step * 3u);
			}
			break;
		case 21:
			current_game_mode = 0x15u;
			next_stage_slot_request = 0u;
			if ((player_status[0] | player_2_status | player_3_status | player_status_spare) != 0u)
			{
				check_stage_clear(&r8, &r10, &r11);
				if (game_mode == 0x15u)
				{
					scroll_mode = 1u;
					update_scroll(&r2, &r3, &r4, &ignored, &r8, &r9, &ignored, &r11);
					update_stage_tile_layers(&ignored, &r2, &r3, &r4, &ignored, &r8, &r9, &ignored, &r11);
					find_nearby_floor_blocks(&ignored, &r3, &r4, &ignored, &r8, &ignored, &r11);
					r4 = update_restart_point(r4);
					spawn_objects_by_type(&ignored, &r2, &r3, &r4, &ignored, &r8, &r11);
					spawn_ring_rows(&ignored, &r2, &ignored, &r11);
					spawn_walls(&ignored, &r2, &ignored, &r8, &r9, &r11);
					spawn_object_groups_2(&ignored, &r8, &r11);
					spawn_object_groups(&ignored, &r2, &ignored, &r8, &r11);
					spawn_objects_by_handler(&ignored, &ignored, &r11);
					ignored = check_zones(ignored);
					check_hits(&ignored, &r2, &ignored, &r4, &ignored, &r8, &ignored);
					check_walls(ignored);
					check_platforms(&ignored, &ignored, &ignored, &r4, &ignored, &r8, &r9, &r10_2, &ignored);
					*p_r4 = r4;
					*p_r8 = r8;
					*p_r9 = r9;
					*p_r10 = r10_2;
					return;
				}
				continue;
			}
			saved_game_mode = game_mode;
			game_mode = 7u;
			game_mode_timer = 0;
			continue;
		case 22:
			current_game_mode = 0x16u;
			update_player_summary(&r3, &r8, &r10, &r11);
			if ((player_status[0] | player_2_status | player_3_status | player_status_spare) != 0u)
			{
				find_nearby_floor_blocks(&r1, &r3, &r4, &ignored, &r8, &ignored, &r11);
				r4 = update_restart_point(r4);
				spawn_objects_by_type(&r1, &r2, &r3, &r4, &ignored, &r8, &r11);
				spawn_ring_rows(&r1, &r2, &ignored, &r11);
				spawn_walls(&r1, &r2, &ignored, &r8, &r9, &r11);
				spawn_object_groups_2(&r1, &r8, &r11);
				spawn_object_groups(&r1, &r2, &ignored, &r8, &r11);
				spawn_objects_by_handler(&r1, &ignored, &r11);
				r1 = check_zones(r1);
				check_hits(&r1, &r2, &ignored, &r4, &ignored, &r8, &ignored);
				check_walls(r1);
				check_platforms(&ignored, &ignored, &ignored, &r4, &ignored, &r8, &r9, &r10_2, &ignored);
				if (scroll_resume_request == 0u)
				{
					*p_r4 = r4;
					*p_r8 = r8;
					*p_r9 = r9;
					*p_r10 = r10_2;
					return;
				}
				scroll_resume_request = 0u;
				scroll_mode = 7u;
				game_mode = 3u;
				*p_r4 = r4;
				*p_r8 = r8;
				*p_r9 = r9;
				*p_r10 = r10_2;
				return;
			}
			saved_game_mode = game_mode;
			game_mode = 7u;
			game_mode_timer = 0;
			continue;
		case 23:
			stage_clear_init_flags = stage_init_flags;
			stage_init_flags = stage_clear_init_flags | 0x4000u;
			if ((stage_clear_init_flags & 0x4000u) == 0u)
			{
				stage_name_shown = 0u;
				cleared_shown = 0u;
				banner_break = 0u;
				stage_clear_timer = 0x14Au;
				stage_exit_started = 0u;
				allocate_object_outside_loop(&free_count, &r11);
				if (free_count != 0u)
				{
					OBJECT(r11)->sprite_flags = 0x1002u;
					OBJECT(r11)->sprite_palette = 0x89A0u;
					OBJECT(r11)->handler = stage_clear_banner_reveal_ADDRESS;
					OBJECT(r11)->flags = 0u;
					OBJECT(r11)->stage_clear_banner.sparkle_offset = 0x10u;
					r10_2 = stage_name_banners[(uint8_t)stage] + stage_name_banners_ADDRESS;
					OBJECT(r11)->screen_x = ROM16(r10_2);
					r10_2 += 2u;
					OBJECT(r11)->screen_y = ROM16(r10_2);
					OBJECT(r11)->animation = r10_2 + 2u;
					OBJECT(r11)->animation_frame = 0xFFu;
					OBJECT(r11)->animation_speed = 4u;
					OBJECT(r11)->animation_timer = 4;
					allocate_object_outside_loop(&free_count, &r11);
					if (free_count != 0u)
					{
						OBJECT(r11)->sprite_flags = 0x1002u;
						OBJECT(r11)->sprite_palette = 0x89A0u;
						OBJECT(r11)->handler = cleared_banner_wait_ADDRESS;
						OBJECT(r11)->flags = 1u;
						OBJECT(r11)->var_80 = 0x10u;
						OBJECT(r11)->var_81 = 0u;
						OBJECT(r11)->screen_x = cleared_banner_x;
						OBJECT(r11)->screen_y = cleared_banner_y;
						OBJECT(r11)->animation = cleared_banner_animation_ADDRESS;
						OBJECT(r11)->animation_frame = 0xFFu;
						OBJECT(r11)->animation_speed = 4u;
						OBJECT(r11)->animation_timer = 4;
						sound_id = 0x94u;
						play_sound(sound_id, &ignored, &r2);
						sound_id = 0x7Bu;
						play_sound(sound_id, &ignored, &r2);
					}
				}
			}
			if ((int16_t)stage_clear_timer <= 0x78)
				banner_break = 0xFFu;
			current_game_mode = 0x17u;
			next_stage_slot_request = 0u;
			update_scroll(&r2, &r3, &r4, &ignored, &r8, &r9, &ignored, &r11);
			update_stage_tile_layers(&ignored, &r2, &r3, &r4, &ignored, &r8, &r9, &ignored, &r11);
			find_nearby_floor_blocks(&ignored, &r3, &r4, &ignored, &r8, &ignored, &r11);
			r4 = update_restart_point(r4);
			spawn_objects_by_type(&ignored, &r2, &r3, &r4, &ignored, &r8, &r11);
			spawn_ring_rows(&ignored, &r2, &ignored, &r11);
			spawn_walls(&ignored, &r2, &ignored, &r8, &r9, &r11);
			spawn_object_groups_2(&ignored, &r8, &r11);
			spawn_object_groups(&ignored, &r2, &ignored, &r8, &r11);
			spawn_objects_by_handler(&ignored, &ignored, &r11);
			ignored = check_zones(ignored);
			check_hits(&ignored, &r2, &ignored, &r4, &ignored, &r8, &ignored);
			check_walls(ignored);
			check_platforms(&ignored, &ignored, &ignored, &r4, &ignored, &r8, &r9, &r10_2, &ignored);
			old_clear_timer = stage_clear_timer;
			stage_clear_timer = old_clear_timer - 1u;
			if ((int16_t)old_clear_timer > 1)
			{
				*p_r4 = r4;
				*p_r8 = r8;
				*p_r9 = r9;
				*p_r10 = r10_2;
				return;
			}
			game_mode = 0x1Du;
			*p_r4 = r4;
			*p_r8 = r8;
			*p_r9 = r9;
			*p_r10 = r10_2;
			return;
		case 24:
			current_game_mode = 0x18u;
			update_ending(r2, r3, r11, &r4, &r8, &r9, &r10_2);
			*p_r4 = r4;
			*p_r8 = r8;
			*p_r9 = r9;
			*p_r10 = r10_2;
			return;
		case 25:
			current_game_mode = 0x19u;
			current_game_mode = 0x19u;
			next_stage_slot_request = 0u;
			update_player_summary(&r3, &r8, &r10, &r11);
			if ((player_status[0] | player_2_status | player_3_status | player_status_spare) != 0u)
			{
				status_cursor = player_status_ADDRESS;
				player_object = player_status_ADDRESS;
				player_block = PLAYER_BLOCK(1);
				players_left = 3u;
				for (;;)
				{
					event_19_status = mem_read8(status_cursor);
					status_cursor++;
					if (event_19_status != 0xFFu)
					{
					event_19_next_player:
						players_left--;
						player_block += 0x200u;
						if (players_left != 0u)
							continue;
						goto event_19_check_rope_bridge;
					}
					player_object = LINKED(player_object, PLAYER(player_block)->object);
					if (OBJECT(player_object)->x_int > 0x17C)
						break;
					goto event_19_next_player;
				}
				barred_gate_closing = 0xFFu;
			event_19_check_rope_bridge:
				if (rope_bridge_collapsing != 0u)
				{
					scroll_mode_low = 0u;
					scroll_speed_low = 0x1E;
				}
				else if (scroll_position >= 0xDA0u)
				{
					if (no_player_in_play == 0u)
					{
						scene_timer++;
						if (scene_timer >= 0x78)
							rope_bridge_collapsing = 0xFFu;
					}
					scroll_mode_low = 1u;
				}
				update_scroll(&r2, &r3, &r4, &ignored, &players_left, &r9, &ignored, &player_block);
				update_stage_tile_layers(&ignored, &r2, &r3, &r4, &ignored, &players_left, &r9, &ignored, &player_block);
				find_nearby_floor_blocks(&ignored, &r3, &r4, &ignored, &players_left, &ignored, &player_block);
				r4 = update_restart_point(r4);
				spawn_objects_by_type(&ignored, &r2, &r3, &r4, &ignored, &players_left, &player_block);
				spawn_ring_rows(&ignored, &r2, &ignored, &player_block);
				spawn_walls(&ignored, &r2, &ignored, &players_left, &r9, &player_block);
				spawn_object_groups_2(&ignored, &players_left, &player_block);
				spawn_object_groups(&ignored, &r2, &ignored, &players_left, &player_block);
				spawn_objects_by_handler(&ignored, &ignored, &player_block);
				ignored = check_zones(ignored);
				check_hits(&ignored, &r2, &ignored, &r4, &ignored, &players_left, &ignored);
				check_walls(ignored);
				check_platforms(&ignored, &ignored, &ignored, &r4, &ignored, &players_left, &r9, &r10_2, &ignored);
				if (scroll_position < 0x1A00u)
				{
					*p_r4 = r4;
					*p_r8 = players_left;
					*p_r9 = r9;
					*p_r10 = r10_2;
					return;
				}
				game_mode = 3u;
				scroll_speed_low = 5;
				*p_r4 = r4;
				*p_r8 = players_left;
				*p_r9 = r9;
				*p_r10 = r10_2;
				return;
			}
			saved_game_mode = game_mode;
			game_mode = 7u;
			game_mode_timer = 0;
			continue;
		case 26:
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
				r11 = event_1a_scroll_script_step_1_ADDRESS;
			}
			next_stage_slot_request = 0u;
			update_player_summary(&r3, &r8, &r10, &r11);
			if ((player_status[0] | player_2_status | player_3_status | player_status_spare) != 0u)
			{
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
				update_scroll(&r2, &r3, &r4, &ignored, &r8, &r9, &ignored, &scroll_script);
				update_stage_tile_layers(&ignored, &r2, &r3, &r4, &ignored, &r8, &r9, &ignored, &scroll_script);
				find_nearby_floor_blocks(&ignored, &r3, &r4, &ignored, &r8, &ignored, &scroll_script);
				r4 = update_restart_point(r4);
				spawn_objects_by_type(&ignored, &r2, &r3, &r4, &ignored, &r8, &scroll_script);
				spawn_ring_rows(&ignored, &r2, &ignored, &scroll_script);
				spawn_walls(&ignored, &r2, &ignored, &r8, &r9, &scroll_script);
				spawn_object_groups_2(&ignored, &r8, &scroll_script);
				spawn_object_groups(&ignored, &r2, &ignored, &r8, &scroll_script);
				spawn_objects_by_handler(&ignored, &ignored, &scroll_script);
				ignored = check_zones(ignored);
				check_hits(&ignored, &r2, &ignored, &r4, &ignored, &r8, &ignored);
				check_walls(ignored);
				check_platforms(&ignored, &ignored, &ignored, &r4, &ignored, &r8, &r9, &r10_2, &ignored);
				if (scroll_position < 0x6080u)
				{
					*p_r4 = r4;
					*p_r8 = r8;
					*p_r9 = r9;
					*p_r10 = r10_2;
					return;
				}
				game_mode = 3u;
				scroll_speed_low = 5;
				scroll_mode_low = 7u;
				*p_r4 = r4;
				*p_r8 = r8;
				*p_r9 = r9;
				*p_r10 = r10_2;
				return;
			}
			saved_game_mode = game_mode;
			game_mode = 7u;
			game_mode_timer = 0;
			continue;
		case 27:
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
				start_stage_view(&r2, &r3, &r4, &ignored);
				load_stage_map_pointers(&r1, &ignored);
				load_stage_goal();
				start_spawns_by_type();
				load_stage_palette(&r9, &ignored, &r11);
				current_game_mode = 0x1Bu;
				mem_write8(cabinet_events, 0x15u);
			}
			r4 = update_restart_point(r4);
			spawn_objects_by_type(&r1, &r2, &r3, &r4, &ignored, &r8, &r11);
			spawn_objects_by_handler(&r1, &ignored, &r11);
			check_hits(&r1, &r2, &ignored, &r4, &ignored, &r8, &ignored);
			check_walls(r1);
			check_platforms(&ignored, &ignored, &ignored, &r4, &ignored, &r8, &r9, &r10_2, &ignored);
			game_mode_timer++;
			if ((uint16_t)game_mode_timer < 3u || (uint16_t)character_select_timer != 0u)
			{
				*p_r4 = r4;
				*p_r8 = r8;
				*p_r9 = r9;
				*p_r10 = r10_2;
				return;
			}
			game_mode = 1u;
			system_mode_step = 0u;
			*p_r4 = r4;
			*p_r8 = r8;
			*p_r9 = r9;
			*p_r10 = r10_2;
			return;
		case 28:
			current_game_mode = 0x1Cu;
			if (scene_step == 0u)
			{
				scene_step = 1u;
				event_1c_riding = 0u;
			}
			next_stage_slot_request = 0u;
			update_player_summary(&r3, &r8, &r10, &r11);
			if ((player_status[0] | player_2_status | player_3_status | player_status_spare) != 0u)
			{
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
				update_scroll(&r2, &r3, &r4, &ignored, &r8, &r9, &ignored, &r11);
				update_stage_tile_layers(&ignored, &r2, &r3, &r4, &ignored, &r8, &r9, &ignored, &r11);
				find_nearby_floor_blocks(&ignored, &r3, &r4, &ignored, &r8, &ignored, &r11);
				r4 = update_restart_point(r4);
				spawn_objects_by_type(&ignored, &r2, &r3, &r4, &ignored, &r8, &r11);
				spawn_ring_rows(&ignored, &r2, &ignored, &r11);
				spawn_walls(&ignored, &r2, &ignored, &r8, &r9, &r11);
				spawn_object_groups_2(&ignored, &r8, &r11);
				spawn_object_groups(&ignored, &r2, &ignored, &r8, &r11);
				spawn_objects_by_handler(&ignored, &ignored, &r11);
				ignored = check_zones(ignored);
				check_hits(&ignored, &r2, &ignored, &r4, &ignored, &r8, &ignored);
				check_walls(ignored);
				check_platforms(&ignored, &ignored, &ignored, &r4, &ignored, &r8, &r9, &r10_2, &ignored);
				if (scroll_position < 0x84A0u)
				{
					*p_r4 = r4;
					*p_r8 = r8;
					*p_r9 = r9;
					*p_r10 = r10_2;
					return;
				}
				game_mode = 3u;
				scroll_speed_low = 5;
				bridge_0_plank_count = 0u;
				*p_r4 = r4;
				*p_r8 = r8;
				*p_r9 = r9;
				*p_r10 = r10_2;
				return;
			}
			saved_game_mode = game_mode;
			game_mode = 7u;
			game_mode_timer = 0;
			continue;
		case 29:
			stage_results_init_flags = stage_init_flags;
			stage_init_flags = stage_results_init_flags | 0x20000u;
			if ((stage_results_init_flags & 0x20000u) == 0u)
			{
				game_mode_timer = 0x1E0;
				start_ring_tally(&r2, &r3, &r4, &r8, &r9, &r11);
			}
			current_game_mode = 0x1Du;
			next_stage_slot_request = 0u;
			update_scroll(&r2, &r3, &r4, &ignored, &r8, &r9, &ignored, &r11);
			update_stage_tile_layers(&ignored, &r2, &r3, &r4, &ignored, &r8, &r9, &ignored, &r11);
			find_nearby_floor_blocks(&ignored, &r3, &r4, &ignored, &r8, &ignored, &r11);
			r4 = update_restart_point(r4);
			spawn_objects_by_type(&ignored, &r2, &r3, &r4, &ignored, &r8, &r11);
			spawn_ring_rows(&ignored, &r2, &ignored, &r11);
			spawn_walls(&ignored, &r2, &ignored, &r8, &r9, &r11);
			spawn_object_groups_2(&ignored, &r8, &r11);
			spawn_object_groups(&ignored, &r2, &ignored, &r8, &r11);
			spawn_objects_by_handler(&ignored, &ignored, &r11);
			ignored = check_zones(ignored);
			check_hits(&ignored, &r2, &ignored, &r4, &ignored, &r8, &ignored);
			check_walls(ignored);
			check_platforms(&ignored, &ignored, &ignored, &r4, &ignored, &r8, &r9, &r10_2, &ignored);
			if (stage_results_done == 0u)
			{
				*p_r4 = r4;
				*p_r8 = r8;
				*p_r9 = r9;
				*p_r10 = r10_2;
				return;
			}
			game_mode = 4u;
			game_mode_timer = 0;
			*p_r4 = r4;
			*p_r8 = r8;
			*p_r9 = r9;
			*p_r10 = r10_2;
			return;
		case 30:
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
				r1 = update_fade(r1);
				if (fade_kind >= 0)
				{
					current_game_mode = 0x1Eu;
					next_stage_slot_request = 0u;
					update_player_summary(&r3, &r8, &ignored, &r11);
					update_scroll(&r2, &r3, &r4, &ignored, &r8, &r9, &ignored, &r11);
					update_stage_tile_layers(&ignored, &r2, &r3, &r4, &ignored, &r8, &r9, &ignored, &r11);
					find_nearby_floor_blocks(&ignored, &r3, &r4, &ignored, &r8, &ignored, &r11);
					continue_to_next_stage_slot(&ignored, &ignored, &r11);
					r4 = update_restart_point(r4);
					spawn_objects_by_type(&ignored, &r2, &r3, &r4, &ignored, &r8, &r11);
					spawn_ring_rows(&ignored, &r2, &ignored, &r11);
					spawn_walls(&ignored, &r2, &ignored, &r8, &r9, &r11);
					spawn_object_groups_2(&ignored, &r8, &r11);
					spawn_object_groups(&ignored, &r2, &ignored, &r8, &r11);
					spawn_objects_by_handler(&ignored, &ignored, &r11);
					ignored = check_zones(ignored);
					check_hits(&ignored, &r2, &ignored, &r4, &ignored, &r8, &ignored);
					check_walls(ignored);
					check_platforms(&ignored, &ignored, &ignored, &r4, &ignored, &r8, &r9, &r10_2, &ignored);
					*p_r4 = r4;
					*p_r8 = r8;
					*p_r9 = r9;
					*p_r10 = r10_2;
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
				continue;
			}
			fade_kind = 0;
			update_fade(r1);
			if (fade_kind >= 0)
			{
				current_game_mode = 0x1Eu;
				next_stage_slot_request = 0u;
				update_player_summary(&r3, &r8, &ignored, &r11);
				update_scroll(&r2, &r3, &r4, &ignored, &r8, &r9, &ignored, &r11);
				update_stage_tile_layers(&ignored, &r2, &r3, &r4, &ignored, &r8, &r9, &ignored, &r11);
				find_nearby_floor_blocks(&ignored, &r3, &r4, &ignored, &r8, &ignored, &r11);
				continue_to_next_stage_slot(&ignored, &ignored, &r11);
				r4 = update_restart_point(r4);
				spawn_objects_by_type(&ignored, &r2, &r3, &r4, &ignored, &r8, &r11);
				spawn_ring_rows(&ignored, &r2, &ignored, &r11);
				spawn_walls(&ignored, &r2, &ignored, &r8, &r9, &r11);
				spawn_object_groups_2(&ignored, &r8, &r11);
				spawn_object_groups(&ignored, &r2, &ignored, &r8, &r11);
				spawn_objects_by_handler(&ignored, &ignored, &r11);
				ignored = check_zones(ignored);
				check_hits(&ignored, &r2, &ignored, &r4, &ignored, &r8, &ignored);
				check_walls(ignored);
				check_platforms(&ignored, &ignored, &ignored, &r4, &ignored, &r8, &r9, &r10_2, &ignored);
				*p_r4 = r4;
				*p_r8 = r8;
				*p_r9 = r9;
				*p_r10 = r10_2;
				return;
			}
			start_stage(r5, &r2, &r3, &r4, &r8, &r9, &r10_2, &r11);
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
				update_player_summary(&r3, &r8, &ignored, &r11);
				update_scroll(&r2, &r3, &r4, &ignored, &r8, &r9, &ignored, &r11);
				update_stage_tile_layers(&ignored, &r2, &r3, &r4, &ignored, &r8, &r9, &ignored, &r11);
				find_nearby_floor_blocks(&ignored, &r3, &r4, &ignored, &r8, &ignored, &r11);
				continue_to_next_stage_slot(&ignored, &ignored, &r11);
				r4 = update_restart_point(r4);
				spawn_objects_by_type(&ignored, &r2, &r3, &r4, &ignored, &r8, &r11);
				spawn_ring_rows(&ignored, &r2, &ignored, &r11);
				spawn_walls(&ignored, &r2, &ignored, &r8, &r9, &r11);
				spawn_object_groups_2(&ignored, &r8, &r11);
				spawn_object_groups(&ignored, &r2, &ignored, &r8, &r11);
				spawn_objects_by_handler(&ignored, &ignored, &r11);
				ignored = check_zones(ignored);
				check_hits(&ignored, &r2, &ignored, &r4, &ignored, &r8, &ignored);
				check_walls(ignored);
				check_platforms(&ignored, &ignored, &ignored, &r4, &ignored, &r8, &r9, &r10_2, &ignored);
				*p_r4 = r4;
				*p_r8 = r8;
				*p_r9 = r9;
				*p_r10 = r10_2;
				return;
			}
			game_mode = 3u;
			*p_r4 = r4;
			*p_r8 = r8;
			*p_r9 = r9;
			*p_r10 = r10_2;
			return;
		case 31:
			current_game_mode = 0x1Fu;
			if (scene_step == 0u)
			{
				scene_step = 0xFFu;
				unused_escape_byte = 0u;
				escape_alarm_timer = 0u;
				start_escape_countdown(&r2, &r8, &r11);
			}
			next_stage_slot_request = 0u;
			update_player_summary(&r3, &r8, &r10, &r11);
			if ((player_status[0] | player_2_status | player_3_status | player_status_spare) != 0u)
			{
				check_stage_clear(&r8, &r10, &r11);
				if (game_mode == 0x1Fu)
				{
					update_scroll(&r2, &r3, &r4, &ignored, &r8, &r9, &ignored, &r11);
					update_stage_tile_layers(&ignored, &r2, &r3, &r4, &ignored, &r8, &r9, &ignored, &r11);
					find_nearby_floor_blocks(&ignored, &r3, &r4, &ignored, &r8, &ignored, &r11);
					r4 = update_restart_point(r4);
					spawn_objects_by_type(&ignored, &r2, &r3, &r4, &ignored, &r8, &r11);
					spawn_ring_rows(&ignored, &r2, &ignored, &r11);
					spawn_walls(&ignored, &r2, &ignored, &r8, &r9, &r11);
					spawn_object_groups_2(&ignored, &r8, &r11);
					spawn_object_groups(&ignored, &r2, &ignored, &r8, &r11);
					spawn_objects_by_handler(&ignored, &ignored, &r11);
					ignored = check_zones(ignored);
					check_hits(&ignored, &r2, &ignored, &r4, &ignored, &r8, &r11);
					check_walls(ignored);
					check_platforms(&ignored, &ignored, &ignored, &r4, &ignored, &r8, &r9, &finished_flags, &r11);
					if (escape_started != 0u)
					{
						escape_alarm_timer++;
						if (escape_alarm_timer == 0x30Cu)
						{
						event_1f_play_alarm_sound:
							sound_id = 0x7Cu;
							play_sound(sound_id, &ignored, &ignored);
						}
						else if (escape_alarm_timer == 1u)
						{
							start_escape_alarm_flash(r11);
							camera_shake_active = 1u;
							sound_id = 0xFu;
							play_sound(sound_id, &ignored, &ignored);
							goto event_1f_play_alarm_sound;
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
						r8 = (r8 & ~0xFFu) | 3u;
						do
						{
							if ((int8_t)mem_read8(exit_status_cursor) >= 0)
								finished_flags = (finished_flags & ~0xFFu) | 0xFFu;
							finished_flags = (finished_flags & ~0xFFFFu) | (uint16_t)((finished_flags << 8) | ((uint16_t)finished_flags >> 8));
							r8--;
							exit_status_cursor++;
						} while (r8 != 0u);
						if (finished_flags != 0xFFFFFFFFu)
						{
							if (no_player_in_play == 0u)
								update_escape_countdown();
							if (escape_seconds != 0u)
							{
								*p_r4 = r4;
								*p_r8 = r8;
								*p_r9 = r9;
								*p_r10 = finished_flags;
								return;
							}
							game_mode = 0x20u;
							game_mode_timer = 0;
							*p_r4 = r4;
							*p_r8 = r8;
							*p_r9 = r9;
							*p_r10 = finished_flags;
							return;
						}
					}
					if (escape_seconds != 0u)
					{
						*p_r4 = r4;
						*p_r8 = r8;
						*p_r9 = r9;
						*p_r10 = finished_flags;
						return;
					}
					game_mode = 0x20u;
					game_mode_timer = 0;
					*p_r4 = r4;
					*p_r8 = r8;
					*p_r9 = r9;
					*p_r10 = finished_flags;
					return;
				}
				continue;
			}
			saved_game_mode = game_mode;
			game_mode = 7u;
			game_mode_timer = 0;
			continue;
		case 32:
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
				queue_sound(sound_id, &r1, &r2);
			}
			next_stage_slot_request = 0u;
			update_player_summary(&r3, &r8, &ignored, &r11);
			find_nearby_floor_blocks(&r1, &r3, &r4, &ignored, &r8, &ignored, &r11);
			r4 = update_restart_point(r4);
			spawn_objects_by_type(&r1, &r2, &r3, &r4, &ignored, &r8, &r11);
			spawn_ring_rows(&r1, &r2, &r3, &r11);
			spawn_walls(&r1, &r2, &ignored, &r8, &r9, &r11);
			spawn_object_groups_2(&r1, &r8, &r11);
			spawn_object_groups(&r1, &r2, &r3, &r8, &r11);
			spawn_objects_by_handler(&r1, &ignored, &r11);
			r1 = check_zones(r1);
			check_hits(&r1, &r2, &r3, &r4, &ignored, &r8, &ignored);
			r1 = check_walls(r1);
			check_platforms(&r1, &r2, &r3, &r4, &ignored, &r8, &r9, &r10_2, &ignored);
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
				play_sound(sound_id, &r1, &r2);
			}
			if (game_mode_timer >= 0x78)
			{
				record = you_failed_string_ADDRESS;
				queue_string_record(record, (uint16_t)r1, r3, &r2, &r4);
				mixer_sprite_control &= ~0x8000u;
			}
			if (game_mode_timer >= 0x12C)
				you_failed_done = 0xFFu;
			if ((player_status[0] | player_2_status | player_3_status | player_status_spare) != 0u)
			{
				*p_r4 = r4;
				*p_r8 = r8;
				*p_r9 = r9;
				*p_r10 = r10_2;
				return;
			}
			saved_game_mode = game_mode;
			game_mode = 7u;
			game_mode_timer = 0;
			*p_r4 = r4;
			*p_r8 = r8;
			*p_r9 = r9;
			*p_r10 = r10_2;
			return;
		default:
			invalid_jump(0x65Fu + game_mode * 3u);
		}
	}
game_mode_start_game:
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
	r11_2 = set_screen_mode(mode);
	r8_2 = clear_player_blocks();
	r10_2 = start_player_objects(r11_2);
	next_game_mode = 0x1Bu;
	if ((int8_t)game_in_progress < 0)
	{
		game_mode = next_game_mode;
		current_game_mode = 0u;
		reset_linked_objects_flag();
		*p_r4 = r4;
		*p_r8 = r8_2;
		*p_r9 = r9;
		*p_r10 = r10_2;
		return;
	}
	next_game_mode = 2u;
	stage_start_kind = 0xFFu;
	game_mode = next_game_mode;
	current_game_mode = 0u;
	reset_linked_objects_flag();
	*p_r4 = r4;
	*p_r8 = r8_2;
	*p_r9 = r9;
	*p_r10 = r10_2;
	return;
game_mode_game_over:
	if ((uint16_t)game_mode_timer == 0u)
	{
		sound_id = 0x94u;
		queue_sound(sound_id, &ignored, &ignored);
		sound_id = 0x73u;
		if (stage == 0x11u)
			sound_id = 0x76u;
		queue_sound(sound_id, &r1, &r2);
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
		queue_string_record(record, (uint16_t)r1, r3, &r2, &r4);
	}
	r1 = check_zones(r1);
	check_hits(&r1, &r2, &ignored, &r4, &ignored, &r8, &ignored);
	check_walls(r1);
	check_platforms(&ignored, &ignored, &ignored, &r4, &ignored, &r8, &r9, &r10_2, &ignored);
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
			*p_r4 = r4;
			*p_r8 = r8;
			*p_r9 = r9;
			*p_r10 = r10_2;
			return;
		}
		break;
	case 2:
	game_over_check_p2_start:
		if ((p2_credits < (uint8_t)credits_needed || (inputs_pressed & 0x20u) == 0u) && (p1_credits < (uint8_t)credits_needed || (inputs_pressed & 0x10u) == 0u))
		{
			*p_r4 = r4;
			*p_r8 = r8;
			*p_r9 = r9;
			*p_r10 = r10_2;
			return;
		}
		break;
	case 3:
		if (p3_credits < (uint8_t)credits_needed || (inputs_pressed & 0x80u) == 0u)
			goto game_over_check_p2_start;
		break;
	default:
		invalid_jump(0xF8Fu + cabinet_config * 2u);
	}
	system_mode = 6u;
	game_mode = 0u;
	system_mode_step = 0u;
	goto game_mode_start_game;
}

void start_stage(uint32_t stage_offset, uint32_t *p_r2, uint32_t *p_r3, uint32_t *p_bounds_offset, uint32_t *p_corners_left, uint32_t *p_palette_record, uint32_t *p_source, uint32_t *p_object)
{
	uint32_t r2 = *p_r2;
	uint32_t r3 = *p_r3;
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
		r3 = start_player_objects_alt(object);
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
	apply_stage_layer_blending(&r2, &corners_left);
	start_stage_view(&r2, &r3, &bounds_offset, &ignored);
	if ((int8_t)game_in_progress >= 0)
	{
		stage_start_section = ROM16(demo_input + 2u);
		move_camera_to_start_section(&r2, &r3, &ignored);
		demo_input += 0x10u;
	}
	load_stage_map_pointers(&ignored, &ignored);
	load_stage_goal();
	find_nearby_floor_blocks(&ignored, &r3, &bounds_offset, &ignored, &corners_left, &ignored, &ignored);
	start_spawns_by_type();
	sound_id = stage_start_sounds[stage];
	play_sound(sound_id, &ignored, &r2);
	game_mode = 3u;
	current_game_mode = 2u;
	stage_start_kind = 0u;
	if (game_in_progress == 0u)
	{
		load_stage_palette(&palette_record, &source, &palette_dest);
		*p_r2 = r2;
		*p_r3 = r3;
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
	*p_r2 = r2;
	*p_r3 = r3;
	*p_bounds_offset = bounds_offset;
	*p_corners_left = corners_left;
	*p_palette_record = palette_record;
	*p_source = source;
	*p_object = palette_dest;
}

void check_stage_select_buttons(void)
{
	if ((input_ports_active & 1u) != 0u && (inputs_pressed & 0x10000u) != 0u)
		game_mode_word = 8u;
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

	if (banner_break != 0u)
	{
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
	}
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
		if (free_count != 0u)
		{
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
			if (players_left != 0u)
				continue;
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
			return;
		}
		break;
	}
	player_mishap = 0xFFu;
	player_1_mishap_flag = 0u;
	player_2_mishap_flag = 0u;
	player_3_mishap_flag = 0u;
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
	if ((int16_t)mem_read16(step_sum) < 0x40)
	{
		if ((int16_t)mem_read16(step_sum) > -0x40)
		{
			*p_player_number = (*p_player_number & 0xFFFF0000u) | step;
			*p_trackball = trackball;
			return;
		}
		step = 0xFFFFu;
	}
	else
		step = 1u;
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
	uint8_t shown = 0, port_d = 0;

	if ((frame_counter & 0x1Fu) == 0u)
	{
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
	}
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
