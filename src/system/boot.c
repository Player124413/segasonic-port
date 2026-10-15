/**
 *    Name:        system/boot.c
 *    Description: power-on and the frame loop: boot, run_frame, the attract screens (warning, title, ranking, demos), init_system and the set-up it calls
 *    Version:     0.1.0
 *    Created:     12-10-2026
 *    Modified:    15-10-2026
 *    Author:      Sebastian Toyos
 *
 *    SegaSonic The Hedgehog (arcade, Sega System 32): the game's V60 program decompiled into C by
 *    tools/decompile.py, with the names of symbols.txt.
 */
#include "game.h"

void boot(void)
{
	uint32_t r4 = 0, r5 = 0, r9 = 0, bytes_left = 0, count = 0, dest = 0, ignored = 0, input_cursor = 0, pairs_left = 0, pressed = 0, released = 0, source = 0, trackball_cursor = 0, trackball_port = 0;
	uint32_t stack_100004 = 0;
	uint32_t psw1 = 0;

	load_privileged(5u, vector_table_ADDRESS);
	source = 0x7Eu;
	dest = 0xE00000u;
	pairs_left = 8u;
	do
	{
		mem_write8(dest, mem_read8(source));
		dest++;
		source++;
		mem_write8(dest, mem_read8(source));
		dest++;
		pairs_left--;
		source++;
	} while (pairs_left != 0u);
	dest = 0xD00000u;
	bytes_left = 0xCu;
	do
	{
		mem_write8(dest, mem_read8(source));
		dest++;
		bytes_left--;
		source++;
	} while (bytes_left != 0u);
	stack_100004 = 0u;
	psw1 = stack_100004;
	psw_write(psw1);
	load_privileged(2u, 0x20FE00u);
	load_privileged(3u, 0x20FE00u);
	load_privileged(4u, 0x20FE00u);
	timer_0_ticks = 0u;
	timer_1_ticks = 0u;
	system_mode_and_step = 0u;
	current_screen = 0u;
	delay_after_reset();
	for (;;)
	{
		init_system(&r4, &r5);
		for (;;)
		{
			wait_for_frame(&pressed, &released, &input_cursor, &count, &trackball_port, &trackball_cursor);
			if (test_mode != 0u)
				break;
			run_frame(pressed, released, input_cursor, count, trackball_port, trackball_cursor, &r4, &r5, &r9);
		}
		start_test_mode(&r4, &r9);
		do
		{
			wait_for_frame(&pressed, &ignored, &ignored, &count, &ignored, &ignored);
			run_test_mode(pressed, (uint16_t)count, &r4, &r9);
		} while (test_mode != 0u);
		system_mode_and_step = 0x300u;
	}
}

void run_frame(uint32_t pressed, uint32_t released, uint32_t input_cursor, uint32_t count, uint32_t trackball_port, uint32_t trackball_cursor, uint32_t *p_scale, uint32_t *p_cursor, uint32_t *p_height)
{
	uint32_t scale = *p_scale;
	uint16_t cursor = (uint16_t)*p_cursor;
	uint32_t height = *p_height;
	uint32_t r5 = 0, r10 = 0, r11 = 0, credits_needed = 0, sprite_entry = 0, start_bit = 0;

	frame_counter++;
	if (system_mode == 0u || system_mode == 6u)
	{
		switch (system_mode)
		{
		case 0:
			attract_warning(&scale, &trackball_port);
			break;
		case 1:
			system_mode_idle();
			break;
		case 2:
			attract_ranking(&scale, &trackball_port);
			break;
		case 3:
			attract_title(&scale, &count, &height, &trackball_port);
			break;
		case 4:
			attract_demo_1(pressed, released, input_cursor, cursor, trackball_cursor, &scale, &count, &height, &trackball_port);
			break;
		case 5:
			attract_demo_2(pressed, released, input_cursor, cursor, trackball_cursor, &scale, &count, &height, &trackball_port);
			break;
		case 6:
			run_game_mode(pressed, released, input_cursor, cursor, trackball_cursor, &scale, &count, &height, &trackball_port);
			break;
		case 7:
			wait_attract_timer();
			break;
		default:
			unknown_code(system_mode_handlers[system_mode]);
		}
		run_objects();
		build_sprite_list(&scale, &count, &height, &trackball_port, &sprite_entry);
		draw_queued_strings((uint16_t)count, (uint16_t)trackball_port, sprite_entry, &scale, &r5, &height);
		*p_scale = scale;
		*p_cursor = r5;
		*p_height = height;
		return;
	}
	draw_attract_credits(&pressed, &released, &r10, &r11);
	credits_needed = (input_cursor & ~0xFFFFu) | credits_per_start;
	if (free_play != 0u)
		credits_needed &= ~0xFFFFu;
	switch (cabinet_config)
	{
	case 0:
	case 1:
	case 4:
		if (p1_credits < (uint8_t)credits_needed)
		{
			switch (system_mode)
			{
			case 0:
				attract_warning(&scale, &r10);
				break;
			case 1:
				system_mode_idle();
				break;
			case 2:
				attract_ranking(&scale, &r10);
				break;
			case 3:
				attract_title(&scale, &count, &height, &r10);
				break;
			case 4:
				attract_demo_1(pressed, released, credits_needed, cursor, r11, &scale, &count, &height, &r10);
				break;
			case 5:
				attract_demo_2(pressed, released, credits_needed, cursor, r11, &scale, &count, &height, &r10);
				break;
			case 6:
				run_game_mode(pressed, released, credits_needed, cursor, r11, &scale, &count, &height, &r10);
				break;
			case 7:
				wait_attract_timer();
				break;
			default:
				unknown_code(system_mode_handlers[system_mode]);
			}
			run_objects();
			build_sprite_list(&scale, &count, &height, &r10, &sprite_entry);
			draw_queued_strings((uint16_t)count, (uint16_t)r10, sprite_entry, &scale, &r5, &height);
			*p_scale = scale;
			*p_cursor = r5;
			*p_height = height;
			return;
		}
		if (setting_no_start_buttons != 0u)
		{
			if ((inputs_pressed & 0x10000u) == 0u && (inputs_pressed & 0x1000000u) == 0u)
			{
			run_frame_check_p3_start:
				if ((cabinet_config & 1u) == 0u)
				{
					switch (system_mode)
					{
					case 0:
						attract_warning(&scale, &r10);
						break;
					case 1:
						system_mode_idle();
						break;
					case 2:
						attract_ranking(&scale, &r10);
						break;
					case 3:
						attract_title(&scale, &count, &height, &r10);
						break;
					case 4:
						attract_demo_1(pressed, released, credits_needed, cursor, r11, &scale, &count, &height, &r10);
						break;
					case 5:
						attract_demo_2(pressed, released, credits_needed, cursor, r11, &scale, &count, &height, &r10);
						break;
					case 6:
						run_game_mode(pressed, released, credits_needed, cursor, r11, &scale, &count, &height, &r10);
						break;
					case 7:
						wait_attract_timer();
						break;
					default:
						unknown_code(system_mode_handlers[system_mode]);
					}
					run_objects();
					build_sprite_list(&scale, &count, &height, &r10, &sprite_entry);
					draw_queued_strings((uint16_t)count, (uint16_t)r10, sprite_entry, &scale, &r5, &height);
					*p_scale = scale;
					*p_cursor = r5;
					*p_height = height;
					return;
				}
				start_bit = 0x12u;
				if (setting_no_start_buttons == 0u)
					start_bit = 7u;
				if ((inputs_pressed & shl32(1u, (int)(start_bit))) == 0u)
				{
					switch (system_mode)
					{
					case 0:
						attract_warning(&scale, &r10);
						break;
					case 1:
						system_mode_idle();
						break;
					case 2:
						attract_ranking(&scale, &r10);
						break;
					case 3:
						attract_title(&scale, &count, &height, &r10);
						break;
					case 4:
						attract_demo_1(pressed, released, credits_needed, cursor, r11, &scale, &count, &height, &r10);
						break;
					case 5:
						attract_demo_2(pressed, released, credits_needed, cursor, r11, &scale, &count, &height, &r10);
						break;
					case 6:
						run_game_mode(pressed, released, credits_needed, cursor, r11, &scale, &count, &height, &r10);
						break;
					case 7:
						wait_attract_timer();
						break;
					default:
						unknown_code(system_mode_handlers[system_mode]);
					}
					run_objects();
					build_sprite_list(&scale, &count, &height, &r10, &sprite_entry);
					draw_queued_strings((uint16_t)count, (uint16_t)r10, sprite_entry, &scale, &r5, &height);
					*p_scale = scale;
					*p_cursor = r5;
					*p_height = height;
					return;
				}
			}
		}
		else if ((inputs_pressed & 0x10u) == 0u && (inputs_pressed & 0x20u) == 0u)
			goto run_frame_check_p3_start;
		break;
	case 2:
	run_frame_check_p2_start:
		if (p2_credits < (uint8_t)credits_needed)
		{
		run_frame_check_p1_start:
			if (p1_credits < (uint8_t)credits_needed)
			{
				switch (system_mode)
				{
				case 0:
					attract_warning(&scale, &r10);
					break;
				case 1:
					system_mode_idle();
					break;
				case 2:
					attract_ranking(&scale, &r10);
					break;
				case 3:
					attract_title(&scale, &count, &height, &r10);
					break;
				case 4:
					attract_demo_1(pressed, released, credits_needed, cursor, r11, &scale, &count, &height, &r10);
					break;
				case 5:
					attract_demo_2(pressed, released, credits_needed, cursor, r11, &scale, &count, &height, &r10);
					break;
				case 6:
					run_game_mode(pressed, released, credits_needed, cursor, r11, &scale, &count, &height, &r10);
					break;
				case 7:
					wait_attract_timer();
					break;
				default:
					unknown_code(system_mode_handlers[system_mode]);
				}
				run_objects();
				build_sprite_list(&scale, &count, &height, &r10, &sprite_entry);
				draw_queued_strings((uint16_t)count, (uint16_t)r10, sprite_entry, &scale, &r5, &height);
				*p_scale = scale;
				*p_cursor = r5;
				*p_height = height;
				return;
			}
			start_bit = 0x10u;
			if (setting_no_start_buttons == 0u)
				start_bit = 4u;
			if ((inputs_pressed & shl32(1u, (int)(start_bit))) == 0u)
			{
				switch (system_mode)
				{
				case 0:
					attract_warning(&scale, &r10);
					break;
				case 1:
					system_mode_idle();
					break;
				case 2:
					attract_ranking(&scale, &r10);
					break;
				case 3:
					attract_title(&scale, &count, &height, &r10);
					break;
				case 4:
					attract_demo_1(pressed, released, credits_needed, cursor, r11, &scale, &count, &height, &r10);
					break;
				case 5:
					attract_demo_2(pressed, released, credits_needed, cursor, r11, &scale, &count, &height, &r10);
					break;
				case 6:
					run_game_mode(pressed, released, credits_needed, cursor, r11, &scale, &count, &height, &r10);
					break;
				case 7:
					wait_attract_timer();
					break;
				default:
					unknown_code(system_mode_handlers[system_mode]);
				}
				run_objects();
				build_sprite_list(&scale, &count, &height, &r10, &sprite_entry);
				draw_queued_strings((uint16_t)count, (uint16_t)r10, sprite_entry, &scale, &r5, &height);
				*p_scale = scale;
				*p_cursor = r5;
				*p_height = height;
				return;
			}
		}
		else
		{
			start_bit = 0x18u;
			if (setting_no_start_buttons == 0u)
				start_bit = 5u;
			if ((inputs_pressed & shl32(1u, (int)(start_bit))) == 0u)
				goto run_frame_check_p1_start;
		}
		break;
	case 3:
		if (p3_credits < (uint8_t)credits_needed)
			goto run_frame_check_p2_start;
		start_bit = 0x12u;
		if (setting_no_start_buttons == 0u)
			start_bit = 7u;
		if ((inputs_pressed & shl32(1u, (int)(start_bit))) == 0u)
			goto run_frame_check_p2_start;
		break;
	default:
		unknown_code(0x115u + cabinet_config * 3u);
	}
	system_mode = 6u;
	game_mode = 0u;
	system_mode_step = 0u;
	game_in_progress = 0xFFu;
	switch (system_mode)
	{
	case 0:
		attract_warning(&scale, &r10);
		break;
	case 1:
		system_mode_idle();
		break;
	case 2:
		attract_ranking(&scale, &r10);
		break;
	case 3:
		attract_title(&scale, &count, &height, &r10);
		break;
	case 4:
		attract_demo_1(pressed, released, credits_needed, cursor, r11, &scale, &count, &height, &r10);
		break;
	case 5:
		attract_demo_2(pressed, released, credits_needed, cursor, r11, &scale, &count, &height, &r10);
		break;
	case 6:
		run_game_mode(pressed, released, credits_needed, cursor, r11, &scale, &count, &height, &r10);
		break;
	case 7:
		wait_attract_timer();
		break;
	default:
		unknown_code(system_mode_handlers[system_mode]);
	}
	run_objects();
	build_sprite_list(&scale, &count, &height, &r10, &sprite_entry);
	draw_queued_strings((uint16_t)count, (uint16_t)r10, sprite_entry, &scale, &r5, &height);
	*p_scale = scale;
	*p_cursor = r5;
	*p_height = height;
}

void attract_title(uint32_t *p_color, uint32_t *p_row_pairs_left, uint32_t *p_blocks_per_row, uint32_t *p_text)
{
	uint32_t color = *p_color;
	uint32_t row_pairs_left = *p_row_pairs_left;
	uint32_t blocks_per_row = *p_blocks_per_row;
	uint8_t mode = 0;
	uint32_t ignored = 0, text = 0, text_position = 0;
	uint8_t cnt_bits = 0;

	switch (system_mode_step)
	{
	case 0:
	case 2:
		display_off_frames = 2u;
		cnt_bits = mem_read8(io_cnt);
		mem_write8(io_cnt, cnt_bits & ~2u);
		mode = 0u;
		set_screen_mode(mode);
		clear_text_layer(&ignored, &ignored, &ignored);
		reset_objects(&ignored, &ignored);
		unused_title_byte_3 = 0u;
		sega_logo_step = 0u;
		title_scene_built = 0u;
		scroll_skip_open = 0u;
		unused_title_byte_1 = 0u;
		unused_title_byte_2 = 0u;
		current_screen = 3u;
		system_mode_step++;
		stage_init_flags &= ~0x8000u;
		stage_init_flags &= ~0x40000u;
		stage_init_flags &= ~0x80000u;
		break;
	case 1:
		break;
	default:
		unknown_code(0x221u + system_mode_step * 2u);
	}
	text = 0x644u;
	print_text(&text, &text_position);
	run_title_screen(text_position, &color, &row_pairs_left, &blocks_per_row, &text);
	if (title_done != 0u)
	{
		advance_attract_screen();
		*p_color = color;
		*p_row_pairs_left = row_pairs_left;
		*p_blocks_per_row = blocks_per_row;
		*p_text = text;
		return;
	}
	*p_color = color;
	*p_row_pairs_left = row_pairs_left;
	*p_blocks_per_row = blocks_per_row;
	*p_text = text;
}

void attract_ranking(uint32_t *p_rows, uint32_t *p_source)
{
	uint32_t rows = *p_rows;
	uint32_t source = *p_source;
	uint8_t mode = 0;
	uint32_t r11 = 0, ignored = 0;

	switch (system_mode_step)
	{
	case 0:
	case 2:
		mode = 0u;
		set_screen_mode(mode);
		clear_text_layer(&ignored, &ignored, &ignored);
		r11 = clear_page_0_and_scroll();
		reset_objects(&ignored, &ignored);
		ranking_done = 0u;
		start_ranking_display(r11, &rows, &source);
		current_screen = 2u;
		system_mode_step++;
		stage_init_flags &= ~0x8000u;
		stage_init_flags &= ~0x40000u;
		stage_init_flags &= ~0x80000u;
		break;
	case 1:
		break;
	default:
		unknown_code(0x2B9u + system_mode_step * 3u);
	}
	if (ranking_done != 0u)
	{
		advance_attract_screen();
		*p_rows = rows;
		*p_source = source;
		return;
	}
	*p_rows = rows;
	*p_source = source;
}

void attract_demo_1(uint32_t pressed, uint32_t released, uint32_t input_cursor, uint16_t cursor, uint32_t trackball_cursor, uint32_t *p_scale, uint32_t *p_count, uint32_t *p_height, uint32_t *p_trackball_port)
{
	uint32_t scale = *p_scale;
	uint32_t count = *p_count;
	uint32_t height = *p_height;
	uint32_t trackball_port = *p_trackball_port;

	switch (system_mode_step)
	{
	case 0:
	case 2:
		system_mode_step++;
		current_screen = 4u;
		stage_init_flags &= ~0x8000u;
		stage_init_flags &= ~0x40000u;
		stage_init_flags &= ~0x80000u;
		game_mode = 0u;
		attract_timer = -0x240;
		demo_input = attract_demo_1_inputs_ADDRESS;
		break;
	case 1:
		break;
	default:
		unknown_code(0x31Eu + system_mode_step * 3u);
	}
	if ((uint16_t)attract_timer == 0u)
	{
		advance_attract_screen();
		*p_scale = scale;
		*p_count = count;
		*p_height = height;
		*p_trackball_port = trackball_port;
		return;
	}
	run_game_mode(pressed, released, input_cursor, cursor, trackball_cursor, &scale, &count, &height, &trackball_port);
	*p_scale = scale;
	*p_count = count;
	*p_height = height;
	*p_trackball_port = trackball_port;
}

void attract_demo_2(uint32_t pressed, uint32_t released, uint32_t input_cursor, uint16_t cursor, uint32_t trackball_cursor, uint32_t *p_scale, uint32_t *p_count, uint32_t *p_height, uint32_t *p_trackball_port)
{
	uint32_t scale = *p_scale;
	uint32_t count = *p_count;
	uint32_t height = *p_height;
	uint32_t trackball_port = *p_trackball_port;

	switch (system_mode_step)
	{
	case 0:
	case 2:
		system_mode_step++;
		current_screen = 5u;
		stage_init_flags &= ~0x8000u;
		stage_init_flags &= ~0x40000u;
		stage_init_flags &= ~0x80000u;
		game_mode = 0u;
		attract_timer = -0x240;
		demo_input = attract_demo_2_inputs_ADDRESS;
		break;
	case 1:
		break;
	default:
		unknown_code(0x36Fu + system_mode_step * 2u);
	}
	if ((uint16_t)attract_timer == 0u)
	{
		advance_attract_screen();
		*p_scale = scale;
		*p_count = count;
		*p_height = height;
		*p_trackball_port = trackball_port;
		return;
	}
	run_game_mode(pressed, released, input_cursor, cursor, trackball_cursor, &scale, &count, &height, &trackball_port);
	*p_scale = scale;
	*p_count = count;
	*p_height = height;
	*p_trackball_port = trackball_port;
}

void advance_attract_screen(void)
{
	uint32_t next_step = 0;

	system_mode_step = 0u;
	next_step = (uint8_t)(attract_step + 1u) & 7u;
	system_mode = attract_sequence[next_step];
	attract_step = next_step;
}

void init_system(uint32_t *p_polynomial, uint32_t *p_words_left)
{
	uint32_t r0 = 0, r4 = 0, r5 = 0, ignored = 0;

	interrupts_off();
	clear_work_ram();
	init_io_ports();
	AS32(frames_to_wait) = 0u;
	test_mode = 0u;
	vblank_copies_pending = 0u;
	AS32(sprite_list_ready) = 0u;
	vblank_copy_count = 0u;
	attract_step = 0u;
	stage_init_flags = 0u;
	random_position = 0u;
	r0 = init_sound();
	load_settings(r0);
	load_bookkeeping();
	apply_settings();
	init_mixer();
	load_default_palettes();
	init_sprites(&r0, &ignored);
	load_text_font(&ignored, &r5);
	init_video_registers();
	reset_objects(&r0, &ignored);
	read_controls(&r0, &ignored, &ignored, &ignored, &ignored, &ignored, &ignored);
	read_controls(&r0, &ignored, &ignored, &ignored, &ignored, &ignored, &ignored);
	read_controls(&r0, &ignored, &ignored, &ignored, &ignored, &ignored, &ignored);
	mem_write8(irq_acknowledge, 0u);
	mem_write8(irq_control_5, 0u);
	r5 = check_moving_seat_dip(r5);
	r4 = init_game_settings();
	interrupts_on();
	*p_polynomial = r4;
	*p_words_left = r5;
}

void init_mixer(void)
{
	move_halfwords_backward(MIXER_REGISTERS, 0x5BCu, 0x28u);
	move_halfwords_backward(mixer_text_blend_ADDRESS, 0x5ECu, 0x10u);
}

void init_io_ports(void)
{
	mem_write8(io_port_d, 0x10u);
	mem_write8(io_port_g, 0u);
	mem_write8(io_port_h, 0u);
	mem_write8(io_cnt, 0x80u);
	mem_write8(io_direction, 0xC8u);
	mem_write8(io_port_d, 0x10u);
	mem_write8(io_port_g, 0u);
	mem_write8(io_port_h, 0u);
	mem_write8(cabinet_events, 0x1Fu);
}

void clear_work_ram(void)
{
	uint32_t blocks_left = 0, dest = 0, fill_word = 0;
	uint16_t saved_system_mode_and_step = 0;

	saved_system_mode_and_step = system_mode_and_step;
	dest = GAME_VARIABLES;
	blocks_left = 0xFCu;
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
		dest += 4u;
		mem_write32(dest, fill_word);
		dest += 4u;
		mem_write32(dest, fill_word);
		dest += 4u;
		mem_write32(dest, fill_word);
		blocks_left--;
		dest += 4u;
	} while (blocks_left != 0u);
	system_mode_and_step = saved_system_mode_and_step;
}

uint32_t init_game_settings(void)
{
	uint32_t r4 = 0;

	cabinet_config = setting_cabinet_config;
	game_in_progress = 0u;
	demo_prompt_shown = 0u;
	joining_closed = 0u;
	r4 = load_ranking();
	move_halfwords_backward(stage_order_copy_ADDRESS, stage_order_ADDRESS, 0x11u);
	return r4;
}

void delay_after_reset(void)
{
	uint32_t count = 0;

	count = 0x8000u;
	do
	{
		count--;
	} while (count != 0u);
}
