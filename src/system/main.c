/**
 *    Name:        system/main.c
 *    Description: the machine's main program: power-on and boot, the frame loop, the attract mode and the system modes, the frame wait and the controls, the interrupt handlers, the reset vector
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

uint32_t wait_for_frames(void)
{
	uint32_t r8 = 0, ignored = 0;

	frames_to_wait = frames_per_wait;
	wait_for_frame(&ignored, &ignored, &ignored, &r8, &ignored, &ignored);
	return r8;
}

void wait_for_frame(uint32_t *p_pressed, uint32_t *p_released, uint32_t *p_input_cursor, uint32_t *p_count, uint32_t *p_trackball_port, uint32_t *p_trackball_cursor)
{
	uint32_t r1 = 0, r2 = 0, r3 = 0, r8 = 0, r10 = 0, r11 = 0, inputs = 0, poll_count = 0;
	uint8_t frames_left = 0;

	poll_count = 0u;
	sprite_list_ready = 0xFFu;
	do
	{
		frame_wait(frames_to_wait_ADDRESS);
		frames_left = frames_to_wait;
		take_interrupts();
		poll_count--;
	} while ((poll_count != 0u) & ((int8_t)frames_left >= 0));
	frame_end();
	inputs = 0u - poll_count;
	frames_to_wait = frames_per_wait;
	sprite_buffer_flip &= 0xFF04u;
	AS16(sprite_list_start) = AS16(sprite_back_buffer);
	attract_timer++;
	read_controls(&inputs, &r1, &r2, &r3, &r8, &r10, &r11);
	*p_pressed = r1;
	*p_released = r2;
	*p_input_cursor = r3;
	*p_count = r8;
	*p_trackball_port = r10;
	*p_trackball_cursor = r11;
}

void read_controls(uint32_t *p_inputs, uint32_t *p_pressed, uint32_t *p_released, uint32_t *p_input_cursor, uint32_t *p_count, uint32_t *p_trackball_port, uint32_t *p_trackball_cursor)
{
	uint32_t inputs = *p_inputs;
	uint32_t axes_left = 0, count = 0, input_cursor = 0, pressed = 0, released = 0, trackball_cursor = 0, trackball_port = 0;

	inputs = (inputs & ~0xFFu) | mem_read8(io_port_b);
	inputs = (((inputs << 8) | (inputs >> 24)) & ~0xFFu) | mem_read8(io_port_a);
	inputs = (((inputs << 8) | (inputs >> 24)) & ~0xFFu) | mem_read8(io_port_f);
	inputs = (((inputs << 8) | (inputs >> 24)) & ~0xFFu) | mem_read8(io_port_e);
	inputs = play_demo_buttons(inputs);
	pressed = input_ports;
	input_ports = inputs;
	input_ports_active = ~inputs;
	released = ~pressed & inputs;
	inputs = ~inputs;
	pressed &= inputs;
	inputs_pressed = pressed;
	input_cursor = inputs_released_ADDRESS;
	inputs_released = released;
	trackball_port = IO_PORTS + 0x40u;
	trackball_cursor = trackball_1_x_ADDRESS;
	axes_left = 6u;
	do
	{
		inputs = (inputs & ~0xFFFFu) | (uint16_t)(int8_t)mem_read8(trackball_port);
		if ((int16_t)inputs <= 0x60 && (int16_t)inputs >= -0x60)
			mem_write16(trackball_cursor, inputs);
		trackball_cursor += 2u;
		axes_left--;
		trackball_port += 4u;
	} while (axes_left != 0u);
	trackball_port = IO_PORTS + 0x40u;
	inputs &= ~0xFFFFu;
	count = 3u;
	do
	{
		mem_write16(trackball_port, inputs);
		count--;
		trackball_port += 8u;
	} while (count != 0u);
	trackball_1_x = 0u - trackball_1_x;
	trackball_2_x = 0u - trackball_2_x;
	trackball_3_x = 0u - trackball_3_x;
	play_demo_trackballs(&count, &trackball_port, &trackball_cursor);
	*p_inputs = inputs;
	*p_pressed = pressed;
	*p_released = released;
	*p_input_cursor = input_cursor;
	*p_count = count;
	*p_trackball_port = trackball_port;
	*p_trackball_cursor = trackball_cursor;
}

uint32_t play_demo_buttons(uint32_t inputs)
{
	uint32_t demo_cursor = 0, ignored = 0;

	if ((int8_t)game_in_progress >= 0 && (int8_t)test_mode >= 0 && (current_screen == 4u || current_screen == 5u) && current_game_mode == 3u)
	{
		demo_patched_inputs = inputs;
		demo_cursor = demo_input;
		move_bits_down(demo_cursor, 0u, temp_0_ADDRESS, 0x19u, 1u, &ignored, &ignored);
		move_bits_down(demo_cursor, 1u, temp_0_ADDRESS, 0x1Au, 1u, &ignored, &ignored);
		move_bits_down(demo_cursor, 2u, temp_0_ADDRESS, 0x1Bu, 1u, &ignored, &ignored);
		inputs = demo_patched_inputs;
		demo_input++;
	}
	return inputs;
}

void play_demo_trackballs(uint32_t *p_count, uint32_t *p_demo_cursor, uint32_t *p_trackball_cursor)
{
	uint32_t count = *p_count;
	uint32_t demo_cursor = *p_demo_cursor;
	uint32_t trackball_cursor = *p_trackball_cursor;

	if ((int8_t)game_in_progress < 0 || (int8_t)test_mode < 0 || (current_screen != 4u && current_screen != 5u))
		return;
	if (current_game_mode != 3u)
		return;
	demo_cursor = demo_input;
	trackball_cursor = trackball_1_x_ADDRESS;
	count = 6u;
	do
	{
		mem_write16(trackball_cursor, (int8_t)mem_read8(demo_cursor));
		trackball_cursor += 2u;
		count--;
		demo_cursor++;
	} while (count != 0u);
	demo_input = demo_cursor;
	*p_count = count;
	*p_demo_cursor = demo_cursor;
	*p_trackball_cursor = trackball_cursor;
}

void pause_on_start_3(void)
{
	uint32_t ignored = 0;

	if ((inputs_pressed & 0x80u) == 0u)
		return;
	debug_pause++;
	do
	{
		attract_timer--;
		wait_for_frame(&ignored, &ignored, &ignored, &ignored, &ignored, &ignored);
	} while ((inputs_pressed & 0x80u) == 0u);
	debug_pause--;
}

void split_frame_count(uint32_t frames, uint32_t frames_high, uint32_t *p_seconds, uint32_t *p_minutes, uint32_t *p_hours, uint32_t *p_days)
{
	uint32_t days = 0, hours = 0, hours_dividend = 0, hours_dividend_high = 0, minutes = 0, minutes_dividend = 0, minutes_dividend_high = 0, seconds = 0, seconds_dividend = 0, seconds_dividend_high = 0;

	divx_pair(0x4E9FA0u, &frames, &frames_high);
	days = frames;
	hours_dividend = frames_high;
	hours_dividend_high = 0u;
	divx_pair(0x346A7u, &hours_dividend, &hours_dividend_high);
	hours = hours_dividend;
	minutes_dividend = hours_dividend_high;
	minutes_dividend_high = 0u;
	divx_pair(0xDFBu, &minutes_dividend, &minutes_dividend_high);
	minutes = minutes_dividend;
	seconds_dividend = minutes_dividend_high;
	seconds_dividend_high = 0u;
	divx_pair(0x3Cu, &seconds_dividend, &seconds_dividend_high);
	seconds = seconds_dividend;
	*p_seconds = seconds;
	*p_minutes = minutes;
	*p_hours = hours;
	*p_days = days;
}

void attract_warning(uint32_t *p_pixels, uint32_t *p_text)
{
	uint32_t pixels = *p_pixels;
	uint32_t text = *p_text;
	uint8_t new_mode = 0;
	uint32_t r0 = 0, r11 = 0, ignored = 0;
	uint8_t cnt_bits = 0;

	switch (system_mode_step)
	{
	case 0:
	case 4:
		break;
	case 1:
		wait_attract_timer();
		*p_pixels = pixels;
		*p_text = text;
		return;
	case 2:
		advance_attract_screen();
		*p_pixels = pixels;
		*p_text = text;
		return;
	default:
		unknown_code(0x9C4EFu + system_mode_step * 3u);
	}
	attract_timer = -0x258;
	system_mode_step++;
	display_off_frames = 3u;
	cnt_bits = mem_read8(io_cnt);
	mem_write8(io_cnt, cnt_bits & ~2u);
	load_text_font(&pixels, &ignored);
	init_video_registers();
	init_sprites(&r0, &ignored);
	new_mode = 1u;
	set_screen_mode(new_mode);
	init_sound();
	text = warning_screen_text_ADDRESS;
	print_text_list(&text, &r11);
	unused_coin_byte = 0u;
	current_screen = 0u;
	*p_pixels = pixels;
	*p_text = text;
}

void system_mode_idle(void)
{
	system_mode_step = 0u;
}

void wait_attract_timer(void)
{
	if ((uint16_t)attract_timer == 0u)
	{
		system_mode_step++;
		frame_counter = 0u;
	}
}

void irq_vblank_start(void)
{
	uint32_t zero = 0;
	uint32_t dest = 0, ignored = 0, source = 0;
	uint32_t saved_r14 = 0;
	uint8_t frames_left = 0, cnt_bits = 0;

	saved_r14 = r14;
	move_halfwords(MIXER_REGISTERS + 0x30u, mixer_text_blend_ADDRESS, 0x10u);
	if (vblank_copies_pending != 0u)
		run_vblank_copies(&source, &dest, &ignored, &ignored);
	call_code(vblank_start_hook);
	send_sound_command();
	if (test_mode == 0u)
	{
		count_frames_and_check_test_switch();
		if (free_play == 0u)
			update_coins();
	}
	frames_left = display_off_frames - 1u;
	zero = frames_left == 0u;
	display_off_frames = frames_left;
	if ((int8_t)frames_left >= 0)
	{
		if (zero == 0u)
		{
			mem_write8(irq_acknowledge, 0xFEu);
			vblank_counter++;
			r14 = saved_r14;
			return;
		}
		cnt_bits = mem_read8(io_cnt);
		mem_write8(io_cnt, cnt_bits | 2u);
	}
	display_off_frames = 0xFFu;
	mem_write8(irq_acknowledge, 0xFEu);
	vblank_counter++;
	r14 = saved_r14;
}

void irq_vblank_end(void)
{
	if (debug_pause == 0u)
		call_code(vblank_hook);
	frames_to_wait--;
	mem_write8(irq_acknowledge, 0xFDu);
}

void irq_sound(void)
{
	mem_write8(irq_acknowledge, 0xFBu);
}

void irq_timer0(void)
{
	timer_0_ticks++;
	mem_write16(timer_0_reload, 0x32u);
	mem_write8(irq_acknowledge, 0xF7u);
}

void irq_timer1(void)
{
	timer_1_ticks++;
	mem_write16(timer_1_reload, 0x23u);
	mem_write8(irq_acknowledge, 0xEFu);
}

void int_default(uint32_t *p_source, uint32_t *p_dest, uint32_t *p_copies_left, uint32_t *p_copy_entry)
{
	uint32_t source = *p_source;
	uint32_t dest = *p_dest;
	uint32_t r3 = 0, r4 = 0;

	halt();
	halt();
	halt();
	halt();
	int_vector_02(&source, &dest, &r3, &r4);
	*p_source = source;
	*p_dest = dest;
	*p_copies_left = r3;
	*p_copy_entry = r4;
}

void int_vector_02(uint32_t *p_source, uint32_t *p_dest, uint32_t *p_copies_left, uint32_t *p_copy_entry)
{
	uint32_t source = *p_source;
	uint32_t dest = *p_dest;
	uint32_t r3 = 0, r4 = 0;

	halt();
	halt();
	halt();
	halt();
	run_vblank_copies(&source, &dest, &r3, &r4);
	*p_source = source;
	*p_dest = dest;
	*p_copies_left = r3;
	*p_copy_entry = r4;
}

void run_vblank_copies(uint32_t *p_source, uint32_t *p_dest, uint32_t *p_copies_left, uint32_t *p_copy_entry)
{
	uint32_t source = *p_source;
	uint32_t dest = *p_dest;
	uint8_t repeats_left = 0;
	uint32_t copies_left = 0, copy_entry = 0, copy_halfwords = 0, dest_end = 0, ignored = 0;
	uint8_t repeats_before = 0;

	vblank_copies_pending = 0u;
	copies_left = vblank_copy_count;
	vblank_copy_count = 0u;
	copy_entry = vblank_copies_ADDRESS;
	if ((int16_t)copies_left <= 0)
	{
		*p_copies_left = copies_left;
		*p_copy_entry = copy_entry;
		return;
	}
	do
	{
		source = mem_read32(copy_entry);
		copy_entry += 4u;
		dest = mem_read32(copy_entry);
		copy_entry += 4u;
		copy_halfwords = mem_read16(copy_entry);
		repeats_left = (uint8_t)((dest << 8) | (dest >> 24));
		copy_entry += 2u;
		for (;;)
		{
			move_string(source, copy_halfwords, dest, copy_halfwords, 2u, 0u, 0u, fill_value, &dest_end, &ignored);
			repeats_before = repeats_left;
			repeats_left = (uint8_t)(repeats_before - 1u);
			if ((int8_t)repeats_before <= 1)
				break;
			dest = dest_end;
		}
		copies_left--;
	} while (copies_left != 0u);
	*p_source = source;
	*p_dest = dest;
	*p_copies_left = copies_left;
	*p_copy_entry = copy_entry;
}

void send_sound_command(void)
{
	uint32_t command_entry = 0, queue_base = 0, read_index = 0;

	read_index = sound_queue_read;
	queue_base = sound_queue_ADDRESS;
	if (read_index != sound_queue_write)
	{
		command_entry = sound_commands_ADDRESS + mem_read8(queue_base + read_index) * 2u;
		mem_write8(sound_command_group, ROM8(command_entry));
		mem_write8(sound_command, ROM8(command_entry + 1u));
		sound_queue_read = (read_index + 1u) & 0x1Fu;
	}
}

void read_service_inputs(void)
{
	uint16_t inputs = 0, previous = 0;
	uint8_t port_f_bits = 0, port_e_bits = 0;

	port_f_bits = mem_read8(io_port_f);
	port_e_bits = mem_read8(io_port_e);
	inputs = (uint16_t)((uint16_t)((uint32_t)port_f_bits << 8) | port_e_bits);
	previous = service_inputs;
	service_inputs = inputs;
	service_inputs_previous = previous;
	service_inputs_pressed = previous & ~inputs;
	unk_20F016 = ~previous & inputs;
}

void count_frames_and_check_test_switch(void)
{
	uint32_t carry = 0;
	uint8_t old_divider = 0;
	uint16_t old_frames = 0;

	old_frames = power_on_frames;
	carry = (uint16_t)(old_frames + 1u) < old_frames;
	power_on_frames = old_frames + 1u;
	if (carry)
		power_on_frames_high++;
	old_divider = power_time_save_divider;
	power_time_save_divider = old_divider - 4u;
	if (old_divider < 4u)
	{
		mem_write32(bookkeeping_power_on_time, AS32(power_on_frames));
		mem_write16(bookkeeping_power_on_time_high, AT16(power_on_frames_high, 2));
		update_bookkeeping_checksum();
		mem_write32(bookkeeping_backup_power_on_time, AS32(power_on_frames));
		mem_write16(bookkeeping_backup_power_on_time_high, AT16(power_on_frames_high, 2));
	}
	game_frames++;
	if ((inputs_pressed & 0x2002u) != 0u && (inputs_pressed & 1u) == 0u)
		test_mode = 0xFFu;
}

void reset(void)
{
	boot();
}
