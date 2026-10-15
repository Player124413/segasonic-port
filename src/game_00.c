/**
 *    Name:        game_00.c
 *    Description: the game's functions, part 00 (in the order of their V60 addresses)
 *    Version:     0.1.0
 *    Created:     12-10-2026
 *    Modified:    14-10-2026
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

void run_game_mode(uint32_t r1, uint32_t r2, uint32_t r3, uint32_t r5, uint32_t r11, uint32_t *p_r4, uint32_t *p_r8, uint32_t *p_r9, uint32_t *p_r10)
{
	uint32_t r4 = *p_r4;
	uint32_t r8 = *p_r8;
	uint32_t r9 = *p_r9;
	uint32_t r10 = *p_r10;
	uint8_t credits_needed = 0, mode = 0, next_game_mode = 0, playing_count = 0, sound_id = 0;
	uint16_t attribute = 0, cell = 0, kind = 0, y = 0;
	uint32_t r0 = 0, r1_2 = 0, r8_2 = 0, r9_2 = 0, r10_2 = 0, r11_2 = 0, attribute_2 = 0, clear_dest = 0, count_text = 0, exit_status_cursor = 0, fill_word = 0, finished_flags = 0, free_count = 0, ignored = 0, intro_status_cursor = 0, new_stage = 0, player_block = 0, player_number = 0, player_object = 0, players_left = 0, record = 0, scroll_script = 0, status_cursor = 0, text = 0, text_position = 0, value = 0;
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
				AS16(players_pulled) = 0u;
				stage_exit_started = 0u;
				stage_06_shake = 0u;
				start_stage_view(&r2, &r3, &r4, &r5);
				stage_start_section = 6u;
				move_camera_to_start_section(&r2, &r3, &r5);
				load_stage_map_pointers(&r1_2, &r10_2);
				load_stage_goal();
				find_nearby_floor_blocks(&r1_2, &r3, &r4, &r5, &r8, &r10_2, &r11);
				start_spawns_by_type();
				load_stage_palette(&r9, &r10, &r11);
				current_game_mode = 1u;
				sound_id = stage_start_sounds[stage];
				play_sound(sound_id, &r1_2, &r2);
			}
			update_scroll(&r2, &r3, &r4, &r5, &r8, &r9, &r10, &r11);
			update_stage_tile_layers(&r1_2, &r2, &r3, &r4, &r5, &r8, &r9, &r10, &r11);
			find_nearby_floor_blocks(&r1_2, &r3, &r4, &r5, &r8, &r10, &r11);
			r4 = update_restart_point(r4);
			spawn_objects_by_type(&r1_2, &r2, &r3, &r4, &r5, &r8, &r11);
			spawn_objects_by_handler(&r1_2, &ignored, &r11);
			check_hits(&r1_2, &r2, &r3, &r4, &r5, &r8, &r11);
			check_walls(r1_2);
			check_platforms(&r1_2, &r2, &r3, &r4, &r5, &r8, &r9, &ignored, &r11);
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
				update_scroll(&r2, &r3, &r4, &r5, &r8, &r9, &r10, &r11);
				update_stage_tile_layers(&r1, &r2, &r3, &r4, &r5, &r8, &r9, &r10, &r11);
				find_nearby_floor_blocks(&r1, &r3, &r4, &r5, &r8, &r10, &r11);
				continue_to_next_stage_slot(&r1, &r10, &r11);
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
				play_sound(sound_id, &r1, &r2);
				sound_id = 3u;
				play_sound(sound_id, &r1, &r2);
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
				print_text_here(&r0, &text, &text_position);
				text = stage_number_text_ADDRESS;
				text_position = TEXT_CELL(18, 21);
				print_text_here(&r0, &text, &text_position);
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
			update_scroll(&r2, &r3, &r4, &r5, &r8, &r9, &r10, &r11);
			update_stage_tile_layers(&r1_2, &r2, &r3, &r4, &r5, &r8, &r9, &r10, &r11);
			continue_to_next_stage_slot(&r1_2, &r10, &r11);
			find_nearby_floor_blocks(&r1_2, &r3, &r4, &r5, &r8, &r10, &r11);
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
				unknown_code(0x9487Cu + scene_step * 3u);
			}
			update_scroll(&r2, &r3, &r4, &r5, &r8, &r9, &r10, &r11);
			update_stage_tile_layers(&r1_2, &r2, &r3, &r4, &r5, &r8, &r9, &r10, &r11);
			r4 = update_restart_point(r4);
			check_hits(&r1_2, &r2, &r3, &r4, &r5, &r8, &r11);
			check_walls(r1_2);
			check_platforms(&r1_2, &r2, &r3, &r4, &r5, &r8, &r9, &r10_2, &r11);
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
				unknown_code(0x946A8u + scene_step * 2u);
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
			find_nearby_floor_blocks(&r1, &r3, &r4, &r5, &r8, &r10, &r11);
			update_scroll(&r2, &r3, &r4, &r5, &r8, &r9, &r10, &r11);
			update_stage_tile_layers(&r1_2, &r2, &r3, &r4, &r5, &r8, &r9, &r10, &r11);
			spawn_objects_by_type(&r1_2, &r2, &r3, &r4, &r5, &r8, &r11);
			spawn_ring_rows(&r1_2, &r2, &r3, &r11);
			spawn_walls(&r1_2, &r2, &r5, &r8, &r9, &r11);
			spawn_object_groups_2(&r1_2, &r8, &r11);
			spawn_object_groups(&r1_2, &r2, &r3, &r8, &r11);
			spawn_objects_by_handler(&r1_2, &ignored, &r11);
			check_hits(&r1_2, &r2, &r3, &r4, &r5, &r8, &r11);
			check_walls(r1_2);
			check_platforms(&r1_2, &r2, &r3, &r4, &r5, &r8, &r9, &r10_2, &r11);
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
				print_text(&text, &r11);
			}
			current_game_mode = 0xEu;
			r1 = check_zones(r1);
			check_hits(&r1, &r2, &r3, &r4, &r5, &r8, &r11);
			check_walls(r1);
			check_platforms(&r1, &r2, &r3, &r4, &r5, &r8, &r9, &r10_2, &r11);
			if ((inputs_pressed & 0x10000u) != 0u || (inputs_pressed & 0x1000000u) != 0u || (inputs_pressed & 0x40000u) != 0u)
				AS8(game_mode_timer) = 0x3Cu;
			AS8(game_mode_timer)++;
			if ((int8_t)AS8(game_mode_timer) < 0x3C)
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
					unknown_code(0xBDCu + cabinet_config * 2u);
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
				AS8(game_mode_timer) = 0u;
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
				unknown_code(0x94606u + scene_step * 3u);
			}
			update_scroll(&r2, &r3, &r4, &r5, &r8, &r9, &r10, &r11);
			update_stage_tile_layers(&r1_2, &r2, &r3, &r4, &r5, &r8, &r9, &r10, &r11);
			r4 = update_restart_point(r4);
			spawn_walls(&r1_2, &r2, &r5, &r8, &r9, &r11);
			check_hits(&r1_2, &r2, &r3, &r4, &r5, &r8, &r11);
			check_walls(r1_2);
			check_platforms(&r1_2, &r2, &r3, &r4, &r5, &r8, &r9, &r10_2, &r11);
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
				spawn_objects_by_handler(&r1, &r10_2, &r11);
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
				spawn_objects_by_handler(&r1, &r10_2, &r11);
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
				spawn_objects_by_handler(&r1, &ignored, &r11);
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
				spawn_objects_by_handler(&r1, &ignored, &r11);
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
				unknown_code(0x94510u + scene_step * 3u);
			}
			break;
		case 17:
			current_game_mode = 0x11u;
			if (stage == 4u)
				scroll_mode = 1u;
			update_scroll(&r2, &r3, &r4, &r5, &r8, &r9, &r10, &r11);
			update_stage_tile_layers(&r1_2, &r2, &r3, &r4, &r5, &r8, &r9, &r10, &r11);
			find_nearby_floor_blocks(&r1_2, &r3, &r4, &r5, &r8, &r10, &r11);
			r4 = update_restart_point(r4);
			spawn_objects_by_type(&r1_2, &r2, &r3, &r4, &r5, &r8, &r11);
			spawn_ring_rows(&r1_2, &r2, &r3, &r11);
			spawn_walls(&r1_2, &r2, &r5, &r8, &r9, &r11);
			spawn_object_groups_2(&r1_2, &r8, &r11);
			spawn_object_groups(&r1_2, &r2, &r3, &r8, &r11);
			spawn_objects_by_handler(&r1_2, &ignored, &r11);
			r1_2 = check_zones(r1_2);
			check_hits(&r1_2, &r2, &r3, &r4, &r5, &r8, &r11);
			check_walls(r1_2);
			check_platforms(&r1_2, &r2, &r3, &r4, &r5, &r8, &r9, &r10_2, &r11);
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
					update_scroll(&r2, &r3, &r4, &r5, &r8, &r9, &r10, &r11);
					update_stage_tile_layers(&r1_2, &r2, &r3, &r4, &r5, &r8, &r9, &r10, &r11);
					find_nearby_floor_blocks(&r1_2, &r3, &r4, &r5, &r8, &r10, &r11);
					r4 = update_restart_point(r4);
					spawn_objects_by_type(&r1_2, &r2, &r3, &r4, &r5, &r8, &r11);
					spawn_ring_rows(&r1_2, &r2, &r3, &r11);
					spawn_walls(&r1_2, &r2, &r5, &r8, &r9, &r11);
					spawn_object_groups_2(&r1_2, &r8, &r11);
					spawn_object_groups(&r1_2, &r2, &r3, &r8, &r11);
					spawn_objects_by_handler(&r1_2, &ignored, &r11);
					r1_2 = check_zones(r1_2);
					check_hits(&r1_2, &r2, &r3, &r4, &r5, &r8, &r11);
					check_walls(r1_2);
					check_platforms(&r1_2, &r2, &r3, &r4, &r5, &r8, &r9, &r10_2, &r11);
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
				if (event_12_status != 0xFFu || mem_read8(r11 + 0x55u) != 0u)
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
				unknown_code(0x943A5u + scene_step * 2u);
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
					update_scroll(&r2, &r3, &r4, &r5, &r8, &r9, &r10, &r11);
					update_stage_tile_layers(&r1_2, &r2, &r3, &r4, &r5, &r8, &r9, &r10, &r11);
					find_nearby_floor_blocks(&r1_2, &r3, &r4, &r5, &r8, &r10, &r11);
					r4 = update_restart_point(r4);
					spawn_objects_by_type(&r1_2, &r2, &r3, &r4, &r5, &r8, &r11);
					spawn_ring_rows(&r1_2, &r2, &r3, &r11);
					spawn_walls(&r1_2, &r2, &r5, &r8, &r9, &r11);
					spawn_object_groups_2(&r1_2, &r8, &r11);
					spawn_object_groups(&r1_2, &r2, &r3, &r8, &r11);
					spawn_objects_by_handler(&r1_2, &ignored, &r11);
					r1_2 = check_zones(r1_2);
					check_hits(&r1_2, &r2, &r3, &r4, &r5, &r8, &r11);
					check_walls(r1_2);
					check_platforms(&r1_2, &r2, &r3, &r4, &r5, &r8, &r9, &r10_2, &r11);
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
						if (mem_read8(r10) == 0x25u && mem_read8(r10 + 0x12Fu) == 0u)
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
				allocate_object_outside_loop(&r0, &r11);
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
				unknown_code(0x94253u + scene_step * 3u);
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
					update_scroll(&r2, &r3, &r4, &r5, &r8, &r9, &r10, &r11);
					update_stage_tile_layers(&r1_2, &r2, &r3, &r4, &r5, &r8, &r9, &r10, &r11);
					find_nearby_floor_blocks(&r1_2, &r3, &r4, &r5, &r8, &r10, &r11);
					r4 = update_restart_point(r4);
					spawn_objects_by_type(&r1_2, &r2, &r3, &r4, &r5, &r8, &r11);
					spawn_ring_rows(&r1_2, &r2, &r3, &r11);
					spawn_walls(&r1_2, &r2, &r5, &r8, &r9, &r11);
					spawn_object_groups_2(&r1_2, &r8, &r11);
					spawn_object_groups(&r1_2, &r2, &r3, &r8, &r11);
					spawn_objects_by_handler(&r1_2, &ignored, &r11);
					r1_2 = check_zones(r1_2);
					check_hits(&r1_2, &r2, &r3, &r4, &r5, &r8, &r11);
					check_walls(r1_2);
					check_platforms(&r1_2, &r2, &r3, &r4, &r5, &r8, &r9, &r10_2, &r11);
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
				find_nearby_floor_blocks(&r1, &r3, &r4, &r5, &r8, &r10, &r11);
				r4 = update_restart_point(r4);
				spawn_objects_by_type(&r1, &r2, &r3, &r4, &r5, &r8, &r11);
				spawn_ring_rows(&r1, &r2, &r3, &r11);
				spawn_walls(&r1, &r2, &r5, &r8, &r9, &r11);
				spawn_object_groups_2(&r1, &r8, &r11);
				spawn_object_groups(&r1, &r2, &r3, &r8, &r11);
				spawn_objects_by_handler(&r1, &ignored, &r11);
				r1 = check_zones(r1);
				check_hits(&r1, &r2, &r3, &r4, &r5, &r8, &r11);
				check_walls(r1);
				check_platforms(&r1, &r2, &r3, &r4, &r5, &r8, &r9, &r10_2, &r11);
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
					OBJECT(r11)->var_80 = 0x10u;
					OBJECT(r11)->var_81 = 0u;
					r10 = stage_name_banners[AS8(stage)] + stage_name_banners_ADDRESS;
					OBJECT(r11)->screen_x = ROM16(r10);
					r10 += 2u;
					OBJECT(r11)->screen_y = ROM16(r10);
					OBJECT(r11)->animation = r10 + 2u;
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
						play_sound(sound_id, &r1, &r2);
						sound_id = 0x7Bu;
						play_sound(sound_id, &r1, &r2);
					}
				}
			}
			if ((int16_t)stage_clear_timer <= 0x78)
				banner_break = 0xFFu;
			current_game_mode = 0x17u;
			next_stage_slot_request = 0u;
			update_scroll(&r2, &r3, &r4, &r5, &r8, &r9, &r10, &r11);
			update_stage_tile_layers(&r1_2, &r2, &r3, &r4, &r5, &r8, &r9, &r10, &r11);
			find_nearby_floor_blocks(&r1_2, &r3, &r4, &r5, &r8, &r10, &r11);
			r4 = update_restart_point(r4);
			spawn_objects_by_type(&r1_2, &r2, &r3, &r4, &r5, &r8, &r11);
			spawn_ring_rows(&r1_2, &r2, &r3, &r11);
			spawn_walls(&r1_2, &r2, &r5, &r8, &r9, &r11);
			spawn_object_groups_2(&r1_2, &r8, &r11);
			spawn_object_groups(&r1_2, &r2, &r3, &r8, &r11);
			spawn_objects_by_handler(&r1_2, &ignored, &r11);
			r1_2 = check_zones(r1_2);
			check_hits(&r1_2, &r2, &r3, &r4, &r5, &r8, &r11);
			check_walls(r1_2);
			check_platforms(&r1_2, &r2, &r3, &r4, &r5, &r8, &r9, &r10_2, &r11);
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
					player_object = (player_object & ~0xFFFFu) | mem_read16(player_block + 0x14u);
					if (OBJECT(player_object)->x_int > 0x17C)
						break;
					goto event_19_next_player;
				}
				barred_gate_closing = 0xFFu;
			event_19_check_rope_bridge:
				if (rope_bridge_collapsing != 0u)
				{
					AS8(scroll_mode) = 0u;
					AS8(scroll_speed) = 0x1Eu;
				}
				else if (scroll_position >= 0xDA0u)
				{
					if (no_player_in_play == 0u)
					{
						scene_timer++;
						if (scene_timer >= 0x78)
							rope_bridge_collapsing = 0xFFu;
					}
					AS8(scroll_mode) = 1u;
				}
				update_scroll(&r2, &r3, &r4, &r5, &players_left, &r9, &player_object, &player_block);
				update_stage_tile_layers(&r1_2, &r2, &r3, &r4, &r5, &players_left, &r9, &player_object, &player_block);
				find_nearby_floor_blocks(&r1_2, &r3, &r4, &r5, &players_left, &player_object, &player_block);
				r4 = update_restart_point(r4);
				spawn_objects_by_type(&r1_2, &r2, &r3, &r4, &r5, &players_left, &player_block);
				spawn_ring_rows(&r1_2, &r2, &r3, &player_block);
				spawn_walls(&r1_2, &r2, &r5, &players_left, &r9, &player_block);
				spawn_object_groups_2(&r1_2, &players_left, &player_block);
				spawn_object_groups(&r1_2, &r2, &r3, &players_left, &player_block);
				spawn_objects_by_handler(&r1_2, &ignored, &player_block);
				r1_2 = check_zones(r1_2);
				check_hits(&r1_2, &r2, &r3, &r4, &r5, &players_left, &player_block);
				check_walls(r1_2);
				check_platforms(&r1_2, &r2, &r3, &r4, &r5, &players_left, &r9, &r10_2, &player_block);
				if (scroll_position < 0x1A00u)
				{
					*p_r4 = r4;
					*p_r8 = players_left;
					*p_r9 = r9;
					*p_r10 = r10_2;
					return;
				}
				game_mode = 3u;
				AS8(scroll_speed) = 5u;
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
				update_scroll(&r2, &r3, &r4, &r5, &r8, &r9, &r10, &scroll_script);
				update_stage_tile_layers(&r1_2, &r2, &r3, &r4, &r5, &r8, &r9, &r10, &scroll_script);
				find_nearby_floor_blocks(&r1_2, &r3, &r4, &r5, &r8, &r10, &scroll_script);
				r4 = update_restart_point(r4);
				spawn_objects_by_type(&r1_2, &r2, &r3, &r4, &r5, &r8, &scroll_script);
				spawn_ring_rows(&r1_2, &r2, &r3, &scroll_script);
				spawn_walls(&r1_2, &r2, &r5, &r8, &r9, &scroll_script);
				spawn_object_groups_2(&r1_2, &r8, &scroll_script);
				spawn_object_groups(&r1_2, &r2, &r3, &r8, &scroll_script);
				spawn_objects_by_handler(&r1_2, &ignored, &scroll_script);
				r1_2 = check_zones(r1_2);
				check_hits(&r1_2, &r2, &r3, &r4, &r5, &r8, &scroll_script);
				check_walls(r1_2);
				check_platforms(&r1_2, &r2, &r3, &r4, &r5, &r8, &r9, &r10_2, &scroll_script);
				if (scroll_position < 0x6080u)
				{
					*p_r4 = r4;
					*p_r8 = r8;
					*p_r9 = r9;
					*p_r10 = r10_2;
					return;
				}
				game_mode = 3u;
				AS8(scroll_speed) = 5u;
				AS8(scroll_mode) = 7u;
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
				AS16(players_pulled) = 0u;
				stage_exit_started = 0u;
				stage_06_shake = 0u;
				nearby_floor_block_count = 0u;
				characters_taken = 0u;
				start_stage_view(&r2, &r3, &r4, &r5);
				load_stage_map_pointers(&r1, &ignored);
				load_stage_goal();
				start_spawns_by_type();
				load_stage_palette(&r9, &ignored, &r11);
				current_game_mode = 0x1Bu;
				mem_write8(cabinet_events, 0x15u);
			}
			r4 = update_restart_point(r4);
			spawn_objects_by_type(&r1, &r2, &r3, &r4, &r5, &r8, &r11);
			spawn_objects_by_handler(&r1, &ignored, &r11);
			check_hits(&r1, &r2, &r3, &r4, &r5, &r8, &r11);
			check_walls(r1);
			check_platforms(&r1, &r2, &r3, &r4, &r5, &r8, &r9, &r10_2, &r11);
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
					unknown_code(0x9510Eu + scene_step * 2u);
				}
				update_scroll(&r2, &r3, &r4, &r5, &r8, &r9, &r10, &r11);
				update_stage_tile_layers(&r1_2, &r2, &r3, &r4, &r5, &r8, &r9, &r10, &r11);
				find_nearby_floor_blocks(&r1_2, &r3, &r4, &r5, &r8, &r10, &r11);
				r4 = update_restart_point(r4);
				spawn_objects_by_type(&r1_2, &r2, &r3, &r4, &r5, &r8, &r11);
				spawn_ring_rows(&r1_2, &r2, &r3, &r11);
				spawn_walls(&r1_2, &r2, &r5, &r8, &r9, &r11);
				spawn_object_groups_2(&r1_2, &r8, &r11);
				spawn_object_groups(&r1_2, &r2, &r3, &r8, &r11);
				spawn_objects_by_handler(&r1_2, &ignored, &r11);
				r1_2 = check_zones(r1_2);
				check_hits(&r1_2, &r2, &r3, &r4, &r5, &r8, &r11);
				check_walls(r1_2);
				check_platforms(&r1_2, &r2, &r3, &r4, &r5, &r8, &r9, &r10_2, &r11);
				if (scroll_position < 0x84A0u)
				{
					*p_r4 = r4;
					*p_r8 = r8;
					*p_r9 = r9;
					*p_r10 = r10_2;
					return;
				}
				game_mode = 3u;
				AS8(scroll_speed) = 5u;
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
			update_scroll(&r2, &r3, &r4, &r5, &r8, &r9, &r10, &r11);
			update_stage_tile_layers(&r1_2, &r2, &r3, &r4, &r5, &r8, &r9, &r10, &r11);
			find_nearby_floor_blocks(&r1_2, &r3, &r4, &r5, &r8, &r10, &r11);
			r4 = update_restart_point(r4);
			spawn_objects_by_type(&r1_2, &r2, &r3, &r4, &r5, &r8, &r11);
			spawn_ring_rows(&r1_2, &r2, &r3, &r11);
			spawn_walls(&r1_2, &r2, &r5, &r8, &r9, &r11);
			spawn_object_groups_2(&r1_2, &r8, &r11);
			spawn_object_groups(&r1_2, &r2, &r3, &r8, &r11);
			spawn_objects_by_handler(&r1_2, &ignored, &r11);
			r1_2 = check_zones(r1_2);
			check_hits(&r1_2, &r2, &r3, &r4, &r5, &r8, &r11);
			check_walls(r1_2);
			check_platforms(&r1_2, &r2, &r3, &r4, &r5, &r8, &r9, &r10_2, &r11);
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
					update_player_summary(&r3, &r8, &r10, &r11);
					update_scroll(&r2, &r3, &r4, &r5, &r8, &r9, &r10, &r11);
					update_stage_tile_layers(&r1_2, &r2, &r3, &r4, &r5, &r8, &r9, &r10, &r11);
					find_nearby_floor_blocks(&r1_2, &r3, &r4, &r5, &r8, &r10, &r11);
					continue_to_next_stage_slot(&r1_2, &r10, &r11);
					r4 = update_restart_point(r4);
					spawn_objects_by_type(&r1_2, &r2, &r3, &r4, &r5, &r8, &r11);
					spawn_ring_rows(&r1_2, &r2, &r3, &r11);
					spawn_walls(&r1_2, &r2, &r5, &r8, &r9, &r11);
					spawn_object_groups_2(&r1_2, &r8, &r11);
					spawn_object_groups(&r1_2, &r2, &r3, &r8, &r11);
					spawn_objects_by_handler(&r1_2, &ignored, &r11);
					r1_2 = check_zones(r1_2);
					check_hits(&r1_2, &r2, &r3, &r4, &r5, &r8, &r11);
					check_walls(r1_2);
					check_platforms(&r1_2, &r2, &r3, &r4, &r5, &r8, &r9, &r10_2, &r11);
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
				update_player_summary(&r3, &r8, &r10, &r11);
				update_scroll(&r2, &r3, &r4, &r5, &r8, &r9, &r10, &r11);
				update_stage_tile_layers(&r1_2, &r2, &r3, &r4, &r5, &r8, &r9, &r10, &r11);
				find_nearby_floor_blocks(&r1_2, &r3, &r4, &r5, &r8, &r10, &r11);
				continue_to_next_stage_slot(&r1_2, &r10, &r11);
				r4 = update_restart_point(r4);
				spawn_objects_by_type(&r1_2, &r2, &r3, &r4, &r5, &r8, &r11);
				spawn_ring_rows(&r1_2, &r2, &r3, &r11);
				spawn_walls(&r1_2, &r2, &r5, &r8, &r9, &r11);
				spawn_object_groups_2(&r1_2, &r8, &r11);
				spawn_object_groups(&r1_2, &r2, &r3, &r8, &r11);
				spawn_objects_by_handler(&r1_2, &ignored, &r11);
				r1_2 = check_zones(r1_2);
				check_hits(&r1_2, &r2, &r3, &r4, &r5, &r8, &r11);
				check_walls(r1_2);
				check_platforms(&r1_2, &r2, &r3, &r4, &r5, &r8, &r9, &r10_2, &r11);
				*p_r4 = r4;
				*p_r8 = r8;
				*p_r9 = r9;
				*p_r10 = r10_2;
				return;
			}
			start_stage(r5, &r2, &r3, &r4, &r8, &r9, &r10, &r11);
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
				update_player_summary(&r3, &r8, &r10, &r11);
				update_scroll(&r2, &r3, &r4, &r5, &r8, &r9, &r10, &r11);
				update_stage_tile_layers(&r1_2, &r2, &r3, &r4, &r5, &r8, &r9, &r10, &r11);
				find_nearby_floor_blocks(&r1_2, &r3, &r4, &r5, &r8, &r10, &r11);
				continue_to_next_stage_slot(&r1_2, &r10, &r11);
				r4 = update_restart_point(r4);
				spawn_objects_by_type(&r1_2, &r2, &r3, &r4, &r5, &r8, &r11);
				spawn_ring_rows(&r1_2, &r2, &r3, &r11);
				spawn_walls(&r1_2, &r2, &r5, &r8, &r9, &r11);
				spawn_object_groups_2(&r1_2, &r8, &r11);
				spawn_object_groups(&r1_2, &r2, &r3, &r8, &r11);
				spawn_objects_by_handler(&r1_2, &ignored, &r11);
				r1_2 = check_zones(r1_2);
				check_hits(&r1_2, &r2, &r3, &r4, &r5, &r8, &r11);
				check_walls(r1_2);
				check_platforms(&r1_2, &r2, &r3, &r4, &r5, &r8, &r9, &r10_2, &r11);
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
			*p_r10 = r10;
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
					update_scroll(&r2, &r3, &r4, &r5, &r8, &r9, &r10, &r11);
					update_stage_tile_layers(&r1_2, &r2, &r3, &r4, &r5, &r8, &r9, &r10, &r11);
					find_nearby_floor_blocks(&r1_2, &r3, &r4, &r5, &r8, &r10, &r11);
					r4 = update_restart_point(r4);
					spawn_objects_by_type(&r1_2, &r2, &r3, &r4, &r5, &r8, &r11);
					spawn_ring_rows(&r1_2, &r2, &r3, &r11);
					spawn_walls(&r1_2, &r2, &r5, &r8, &r9, &r11);
					spawn_object_groups_2(&r1_2, &r8, &r11);
					spawn_object_groups(&r1_2, &r2, &r3, &r8, &r11);
					spawn_objects_by_handler(&r1_2, &ignored, &r11);
					r1_2 = check_zones(r1_2);
					check_hits(&r1_2, &r2, &r3, &r4, &r5, &r8, &r11);
					check_walls(r1_2);
					check_platforms(&r1_2, &r2, &r3, &r4, &r5, &r8, &r9, &finished_flags, &r11);
					if (escape_started != 0u)
					{
						escape_alarm_timer++;
						if (escape_alarm_timer == 0x30Cu)
						{
						event_1f_play_alarm_sound:
							sound_id = 0x7Cu;
							play_sound(sound_id, &r1_2, &r2);
						}
						else if (escape_alarm_timer == 1u)
						{
							start_escape_alarm_flash(r11);
							camera_shake_active = 1u;
							sound_id = 0xFu;
							play_sound(sound_id, &r1_2, &r2);
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
			update_player_summary(&r3, &r8, &r10, &r11);
			find_nearby_floor_blocks(&r1, &r3, &r4, &r5, &r8, &r10, &r11);
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
			check_platforms(&r1, &r2, &r3, &r4, &r5, &r8, &r9, &r10_2, &r11);
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
			unknown_code(0x65Fu + game_mode * 3u);
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
		stage = mem_read16(demo_input);
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
	check_hits(&r1, &r2, &r3, &r4, &r5, &r8, &r11);
	check_walls(r1);
	check_platforms(&r1, &r2, &r3, &r4, &r5, &r8, &r9, &r10_2, &r11);
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
		unknown_code(0xF8Fu + cabinet_config * 2u);
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
	uint32_t r1 = 0, r8 = 0, r10 = 0, r11 = 0, palette_record = 0, source = 0;
	uint16_t cleared_count = 0;

	if (stage_start_kind == 0u)
	{
		mem_write16(cleared_stages_ADDRESS, cleared_stages + 1u);
		if ((int8_t)AS8(cleared_stages) > 0xE)
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
	apply_stage_layer_blending(&r2, &r8);
	start_stage_view(&r2, &r3, &bounds_offset, &stage_offset);
	if ((int8_t)game_in_progress >= 0)
	{
		stage_start_section = mem_read16(demo_input + 2u);
		move_camera_to_start_section(&r2, &r3, &stage_offset);
		demo_input += 0x10u;
	}
	load_stage_map_pointers(&r1, &r10);
	load_stage_goal();
	find_nearby_floor_blocks(&r1, &r3, &bounds_offset, &stage_offset, &r8, &r10, &object);
	start_spawns_by_type();
	sound_id = stage_start_sounds[stage];
	play_sound(sound_id, &r1, &r2);
	game_mode = 3u;
	current_game_mode = 2u;
	stage_start_kind = 0u;
	if (game_in_progress == 0u)
	{
		load_stage_palette(&palette_record, &source, &r11);
		*p_r2 = r2;
		*p_r3 = r3;
		*p_bounds_offset = bounds_offset;
		*p_corners_left = r8;
		*p_palette_record = palette_record;
		*p_source = source;
		*p_object = r11;
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
	load_stage_palette(&palette_record, &source, &r11);
	*p_r2 = r2;
	*p_r3 = r3;
	*p_bounds_offset = bounds_offset;
	*p_corners_left = r8;
	*p_palette_record = palette_record;
	*p_source = source;
	*p_object = r11;
}

void check_stage_select_buttons(void)
{
	if ((input_ports_active & 1u) != 0u && (inputs_pressed & 0x10000u) != 0u)
		AS16(game_mode) = 8u;
}

void cleared_banner_wait(void)
{
	if (stage_name_shown != 0u)
		OBJECT(current_object)->handler = stage_clear_banner_reveal_ADDRESS;
}

void stage_clear_banner_reveal(void)
{
	uint16_t sparkle_offset = 0;
	uint32_t r7 = 0, at_last_step = 0, frame = 0;

	if ((uint8_t)OBJECT(current_object)->animation_timer == OBJECT(current_object)->animation_speed)
	{
		sparkle_offset = OBJECT(current_object)->stage_clear_banner.sparkle_offset;
		spawn_banner_sparkle(sparkle_offset);
		OBJECT(current_object)->stage_clear_banner.sparkle_offset += 0x10u;
	}
	animate(&frame, &r7, &at_last_step);
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
	uint32_t depth = 0, free_count = 0, ignored = 0, object = 0, write_index = 0, write_index_address = 0;
	uint16_t letters_left = 0;

	if (banner_break != 0u)
	{
		sound_id = 0x1Bu;
		play_sound(sound_id, &write_index, &write_index_address);
		banner_letters_left = mem_read16(OBJECT(current_object)->animation);
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
	uint32_t r8 = 0, depth = 0, frame = 0, ignored = 0, wrapped = 0;

	animate(&frame, &wrapped, &r8);
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
	uint32_t r8 = 0, depth = 0, frame = 0, free_count = 0, ignored = 0, object = 0, wrapped = 0;

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
	animate(&frame, &wrapped, &r8);
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

uint32_t check_zones(uint32_t box_edge)
{
	uint16_t box_start = 0;
	uint32_t hitter = 0, object = 0;

	hitter = unk_20F016_ADDRESS;
	object = unk_20F016_ADDRESS;
	if ((int16_t)zone_list < 0)
	{
		zone_list = 0xFFFFu;
		return box_edge;
	}
	while ((int16_t)OBJECT(hitter)->hit_list_next >= 0)
	{
		hitter = LINKED(hitter, OBJECT(hitter)->hit_list_next);
		object = LINKED(object, zone_list);
		for (;;)
		{
			box_edge = (box_edge & ~0xFFFFu) | (uint16_t)OBJECT(object)->hitbox_offset_x;
			box_start = (uint16_t)(OBJECT(object)->x_int + box_edge);
			if (OBJECT(hitter)->x_int < (int16_t)box_start || (int16_t)(box_start + OBJECT(object)->hitbox_size_x) < OBJECT(hitter)->x_int)
			{
				if ((int16_t)OBJECT(object)->hit_list_next < 0)
					break;
				object = LINKED(object, OBJECT(object)->hit_list_next);
				continue;
			}
			box_edge = (box_edge & ~0xFFFFu) | (uint16_t)OBJECT(object)->hitbox_offset_y;
			box_start = (uint16_t)(OBJECT(object)->y_int + box_edge);
			if (OBJECT(hitter)->y_int < (int16_t)box_start || (int16_t)(box_start + OBJECT(object)->hitbox_size_y) < OBJECT(hitter)->y_int)
			{
				if ((int16_t)OBJECT(object)->hit_list_next < 0)
					break;
				object = LINKED(object, OBJECT(object)->hit_list_next);
				continue;
			}
			box_edge = (box_edge & ~0xFFFFu) | (uint16_t)OBJECT(object)->z_int;
			box_edge = (box_edge & ~0xFFFFu) | (uint16_t)(box_edge - OBJECT(object)->hitbox_height);
			if (OBJECT(hitter)->z_int < (int16_t)box_edge)
			{
				if ((int16_t)OBJECT(object)->hit_list_next < 0)
					break;
				object = LINKED(object, OBJECT(object)->hit_list_next);
				continue;
			}
			box_edge = (box_edge & ~0xFFFFu) | (uint16_t)OBJECT(hitter)->z_int;
			box_edge = (box_edge & ~0xFFFFu) | (uint16_t)(box_edge - OBJECT(hitter)->hitbox_height);
			if (OBJECT(object)->z_int < (int16_t)box_edge)
			{
				if ((int16_t)OBJECT(object)->hit_list_next < 0)
					break;
				object = LINKED(object, OBJECT(object)->hit_list_next);
				continue;
			}
			OBJECT(object)->flags |= 0x8000000u;
			OBJECT(hitter)->flags |= 0x8000000u;
			OBJECT(hitter)->anchor_x = object;
			OBJECT(object)->anchor_x = hitter;
			break;
		}
	}
	zone_list = 0xFFFFu;
	return box_edge;
}

uint32_t add_to_zone_list(uint32_t old_head)
{
	old_head = LINKED(old_head, zone_list);
	zone_list = current_object;
	OBJECT(current_object)->hit_list_next = old_head;
	OBJECT(current_object)->flags &= ~0x8000000u;
	return old_head;
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
	} while ((count != 0u) & (mem_read8(player_block + 0x9Fu) == 0u));
	if (mem_read8(player_block + 0x9Fu) != 0u)
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
	uint32_t player_block = 0, players_left = 0, status_cursor = 0;
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
		if (mem_read16(scroll_mode_script - 4u) != 0u)
			AS8(scroll_speed) = 5u;
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
	player_object = PLAYER_BLOCK(1);
	players_left = 3u;
	for (;;)
	{
		player_object = (player_object & ~0xFFFFu) | mem_read16(player_block + 0x14u);
		if ((mem_read32(player_object + 0x30u) & 0x400u) != 0u || mem_read8(player_block + 0xB4u) == 0u)
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
			*p_player_object = player_object;
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
	*p_player_object = player_object;
}

void continue_to_next_stage_slot(uint32_t *p_table_offset, uint32_t *p_link_record, uint32_t *p_block)
{
	uint32_t table_offset = *p_table_offset;
	uint32_t link_record = *p_link_record;
	uint32_t block = *p_block;
	uint32_t r10 = 0, current_stage = 0, from_block = 0, from_record = 0, link_point = 0, to_block = 0;

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
	load_stage_map_pointers(&table_offset, &r10);
	stage_init_flags &= 0xFFFFFFF0u;
	*p_table_offset = table_offset;
	*p_link_record = r10;
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
	if ((int16_t)(mem_read16(step_sum) ^ mem_read16(trackball)) < 0)
		mem_write16(step_sum, 0u);
	mem_write16(step_sum, mem_read16(step_sum) + mem_read16(trackball));
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
	uint32_t r3 = 0, ignored = 0;

	player_handler_list = player_objects_alt_ADDRESS;
	reset_objects_from_list(object, &r3, &ignored);
	return r3;
}

uint32_t start_player_objects(uint32_t object)
{
	uint32_t r10 = 0, ignored = 0;

	player_handler_list = player_objects_ADDRESS;
	reset_objects_from_list(object, &ignored, &r10);
	return r10;
}

void reset_objects_from_list(uint32_t object, uint32_t *p_player_number, uint32_t *p_handler_cursor)
{
	uint8_t status = 0;
	uint32_t free_count = 0, handler_cursor = 0, handlers_left = 0, ignored = 0, player_number = 0;
	uint32_t handler_count = 0;

	player_block_cursor = PLAYER_BLOCK(1);
	reset_objects(&ignored, &player_number);
	handler_cursor = object_handler_list;
	handlers_left = mem_read16(handler_cursor);
	handler_cursor += 2u;
	object = allocate_object_or_start_list(object);
	OBJECT(object)->handler = mem_read32(handler_cursor);
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
		OBJECT(object)->handler = mem_read32(handler_cursor);
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

void check_hits(uint32_t *p_box_edge, uint32_t *p_distance, uint32_t *p_r3, uint32_t *p_r4, uint32_t *p_radius, uint32_t *p_centre_offset, uint32_t *p_previous)
{
	uint32_t box_edge = *p_box_edge;
	uint32_t distance = *p_distance;
	uint32_t r3 = *p_r3;
	uint32_t r4 = *p_r4;
	uint32_t radius = *p_radius;
	uint32_t centre_offset = *p_centre_offset;
	uint32_t previous = *p_previous;
	uint32_t object = 0, other = 0;

	if ((int16_t)hitter_list < 0)
	{
		hitter_list = 0xFFFFu;
		touchable_list = 0xFFFFu;
		return;
	}
	other = 0x200000u;
	previous = 0x200000u;
	object = LINKED(WORK_RAM, hitter_list);
	do
	{
		if ((int16_t)OBJECT(object)->hit_list_next < 0)
		{
		check_hits_against_touchables:
			other = LINKED(other, touchable_list);
			if ((int16_t)other >= 0)
			{
				previous = input_ports_ADDRESS;
				check_hit(other, object, &box_edge, &distance, &r3, &r4, &radius, &centre_offset, &previous);
			}
		}
		else
		{
			other = LINKED(other, OBJECT(object)->hit_list_next);
			previous = (previous & ~0xFFFFu) | (uint16_t)object;
			check_hit(other, object, &box_edge, &distance, &r3, &r4, &radius, &centre_offset, &previous);
			if ((OBJECT(object)->flags & 4u) == 0u)
				goto check_hits_against_touchables;
		}
		object = LINKED(object, OBJECT(object)->hit_list_next);
	} while ((int16_t)object >= 0);
	hitter_list = 0xFFFFu;
	touchable_list = 0xFFFFu;
	*p_box_edge = box_edge;
	*p_distance = (*p_distance & 0xFFFF0000u) | (distance & 0xFFFFu);
	*p_r3 = (*p_r3 & 0xFFFF0000u) | (r3 & 0xFFFFu);
	*p_r4 = (*p_r4 & 0xFFFF0000u) | (r4 & 0xFFFFu);
	*p_radius = (*p_radius & 0xFFFF0000u) | (radius & 0xFFFFu);
	*p_centre_offset = (*p_centre_offset & 0xFFFF0000u) | (centre_offset & 0xFFFFu);
	*p_previous = previous;
}

void check_hit(uint32_t other, uint32_t hitter, uint32_t *p_box_edge, uint32_t *p_distance, uint32_t *p_r3, uint32_t *p_r4, uint32_t *p_radius, uint32_t *p_centre_offset, uint32_t *p_previous)
{
	uint32_t box_edge = *p_box_edge;
	uint16_t distance = (uint16_t)*p_distance;
	uint16_t r3 = (uint16_t)*p_r3;
	uint16_t r4 = (uint16_t)*p_r4;
	uint16_t radius = (uint16_t)*p_radius;
	uint16_t centre_offset = (uint16_t)*p_centre_offset;
	uint16_t previous = (uint16_t)*p_previous;
	uint16_t box_start = 0, diameter = 0;
	uint32_t floor_slope = 0, quadrant = 0, slope_factor = 0, slope_kind = 0, slope_record = 0;

	for (;;)
	{
		if ((int8_t)OBJECT(other)->kind <= 2 && (int8_t)OBJECT(hitter)->kind <= 2)
		{
		check_hit_next_pair:
			if ((int16_t)OBJECT(other)->hit_list_next < 0)
			{
				*p_box_edge = box_edge;
				*p_distance = (*p_distance & 0xFFFF0000u) | distance;
				*p_r3 = (*p_r3 & 0xFFFF0000u) | r3;
				*p_r4 = (*p_r4 & 0xFFFF0000u) | r4;
				*p_radius = (*p_radius & 0xFFFF0000u) | radius;
				*p_centre_offset = (*p_centre_offset & 0xFFFF0000u) | centre_offset;
				*p_previous = (*p_previous & 0xFFFF0000u) | previous;
				return;
			}
			previous = (uint16_t)other;
			other = LINKED(other, OBJECT(other)->hit_list_next);
			continue;
		}
		if ((OBJECT(hitter)->flags & 2u) == 0u)
		{
			if ((OBJECT(other)->flags & 2u) == 0u)
			{
				radius = (uint16_t)(OBJECT(hitter)->hitbox_size_x + OBJECT(other)->hitbox_size_x);
				diameter = (uint16_t)(radius * 2u + 1u);
				centre_offset = (uint16_t)(int8_t)AS8(OBJECT(hitter)->hitbox_size_y);
				r3 = (uint16_t)(OBJECT(hitter)->x_int + centre_offset);
				distance = (uint16_t)(OBJECT(other)->x_int + (int8_t)AS8(OBJECT(other)->hitbox_size_y) - r3);
				if (diameter <= (uint16_t)(distance + radius))
					goto check_hit_next_pair;
				centre_offset = (uint16_t)(int8_t)AT8(OBJECT(hitter)->hitbox_size_y, 1);
				r4 = (uint16_t)(OBJECT(hitter)->y_int + centre_offset);
				r3 = (uint16_t)(OBJECT(other)->y_int + (int8_t)AT8(OBJECT(other)->hitbox_size_y, 1) - r4);
				if (diameter <= (uint16_t)(r3 + radius))
					goto check_hit_next_pair;
				if (OBJECT(hitter)->floor_slope != 0u)
				{
					floor_slope = OBJECT(hitter)->floor_slope;
					slope_record = floor_slopes_ADDRESS + (uint16_t)(floor_slope * 2u) * 4u;
					slope_kind = ROM8(slope_record) * 2u;
					slope_record++;
					switch (slope_kind)
					{
					case 0x0u:
					case 0x2u:
					case 0x8u:
						box_edge = (int16_t)r3;
						break;
					case 0x4u:
					case 0x6u:
						box_edge = (int16_t)distance;
						break;
					default:
						unknown_code(0x1FDEu + slope_kind);
					}
					if ((int16_t)box_edge < 0)
						box_edge = 0u - box_edge;
					slope_factor = ROM32(slope_record + 1u);
					if ((int32_t)slope_factor < 0)
						slope_factor = 0u - slope_factor;
					box_edge = (((box_edge * slope_factor) >> 8) * 0xB5u) >> 16;
					quadrant = 0u;
					if ((int16_t)distance < 0)
						quadrant = (uint16_t)(quadrant + 2u);
					if ((int16_t)r3 < 0)
						quadrant = (uint16_t)(quadrant + 1u);
					switch (quadrant)
					{
					case 0:
					check_hit_add_slope_to_dx:
						distance += box_edge;
						r3 -= box_edge;
						break;
					case 1:
					case 4:
						if ((int16_t)r3 < (int16_t)distance)
							goto check_hit_add_slope_to_dx;
					case 2:
					check_hit_sub_slope_from_dx:
						distance -= box_edge;
						r3 += box_edge;
						break;
					case 3:
						if ((int16_t)r3 <= (int16_t)distance)
							goto check_hit_add_slope_to_dx;
						goto check_hit_sub_slope_from_dx;
					default:
						unknown_code(0x202Eu + quadrant * 2u);
					}
				}
				r3 = (uint16_t)((uint32_t)r3 * r3);
				distance = (uint16_t)((uint32_t)distance * distance + r3);
				radius = (uint16_t)((uint32_t)radius * radius);
				if ((int16_t)radius < (int16_t)distance)
					goto check_hit_next_pair;
			}
			else
			{
				box_edge = (box_edge & ~0xFFFFu) | (uint16_t)OBJECT(other)->hitbox_offset_x;
				box_start = (uint16_t)(OBJECT(other)->x_int + box_edge);
				if (OBJECT(hitter)->x_int < (int16_t)box_start || (int16_t)(box_start + OBJECT(other)->hitbox_size_x) < OBJECT(hitter)->x_int)
					goto check_hit_next_pair;
				box_edge = (box_edge & ~0xFFFFu) | (uint16_t)OBJECT(other)->hitbox_offset_y;
				box_start = (uint16_t)(OBJECT(other)->y_int + box_edge);
				if (OBJECT(hitter)->y_int < (int16_t)box_start || (int16_t)(box_start + OBJECT(other)->hitbox_size_y) < OBJECT(hitter)->y_int)
					goto check_hit_next_pair;
			}
		}
		else if ((OBJECT(other)->flags & 2u) == 0u)
		{
			r3 = (uint16_t)OBJECT(hitter)->hitbox_offset_x;
			distance = (uint16_t)(OBJECT(hitter)->x_int + r3);
			if (OBJECT(other)->x_int < (int16_t)distance)
			{
				box_edge = (box_edge & ~0xFFFFu) | (uint16_t)OBJECT(other)->x_int;
				box_edge = (box_edge & ~0xFFFFu) | (uint16_t)(box_edge + OBJECT(other)->hitbox_size_x);
				if ((int16_t)box_edge < (int16_t)distance)
					goto check_hit_next_pair;
			}
			distance += OBJECT(hitter)->hitbox_size_x;
			if ((int16_t)distance < OBJECT(other)->x_int)
			{
				box_edge = (box_edge & ~0xFFFFu) | (uint16_t)OBJECT(other)->x_int;
				box_edge = (box_edge & ~0xFFFFu) | (uint16_t)(box_edge - OBJECT(other)->hitbox_size_x);
				if ((int16_t)distance < (int16_t)box_edge)
					goto check_hit_next_pair;
			}
			r3 = (uint16_t)OBJECT(hitter)->hitbox_offset_y;
			distance = (uint16_t)(OBJECT(hitter)->y_int + r3);
			if (OBJECT(other)->y_int < (int16_t)distance)
			{
				box_edge = (box_edge & ~0xFFFFu) | (uint16_t)OBJECT(other)->y_int;
				box_edge = (box_edge & ~0xFFFFu) | (uint16_t)(box_edge + OBJECT(other)->hitbox_size_x);
				if ((int16_t)box_edge < (int16_t)distance)
					goto check_hit_next_pair;
			}
			distance += OBJECT(hitter)->hitbox_size_y;
			if ((int16_t)distance < OBJECT(other)->y_int)
			{
				box_edge = (box_edge & ~0xFFFFu) | (uint16_t)OBJECT(other)->y_int;
				box_edge = (box_edge & ~0xFFFFu) | (uint16_t)(box_edge - OBJECT(other)->hitbox_size_x);
				if ((int16_t)distance < (int16_t)box_edge)
					goto check_hit_next_pair;
			}
		}
		else
		{
			r3 = (uint16_t)OBJECT(hitter)->hitbox_offset_x;
			distance = (uint16_t)(OBJECT(hitter)->x_int + r3);
			radius = (uint16_t)OBJECT(other)->hitbox_offset_x;
			r4 = (uint16_t)(OBJECT(other)->x_int + radius);
			if ((int16_t)(distance + OBJECT(hitter)->hitbox_size_x) < (int16_t)r4)
				goto check_hit_next_pair;
			r4 += OBJECT(other)->hitbox_size_x;
			if ((int16_t)r4 < (int16_t)distance)
				goto check_hit_next_pair;
			r3 = (uint16_t)OBJECT(hitter)->hitbox_offset_y;
			distance = (uint16_t)(OBJECT(hitter)->y_int + r3);
			radius = (uint16_t)OBJECT(other)->hitbox_offset_y;
			r4 = (uint16_t)(OBJECT(other)->y_int + radius);
			if ((int16_t)(distance + OBJECT(hitter)->hitbox_size_y) < (int16_t)r4)
				goto check_hit_next_pair;
			r4 += OBJECT(other)->hitbox_size_y;
			if ((int16_t)r4 < (int16_t)distance)
				goto check_hit_next_pair;
		}
		box_edge = (box_edge & ~0xFFFFu) | (uint16_t)OBJECT(other)->z_int;
		box_edge = (box_edge & ~0xFFFFu) | (uint16_t)(box_edge - OBJECT(other)->hitbox_height);
		if (OBJECT(hitter)->z_int < (int16_t)box_edge)
			goto check_hit_next_pair;
		box_edge = (box_edge & ~0xFFFFu) | (uint16_t)OBJECT(hitter)->z_int;
		box_edge = (box_edge & ~0xFFFFu) | (uint16_t)(box_edge - OBJECT(hitter)->hitbox_height);
		if ((int16_t)box_edge <= OBJECT(other)->z_int)
			break;
		goto check_hit_next_pair;
	}
	OBJECT(other)->flags |= 4u;
	OBJECT(hitter)->flags |= 4u;
	OBJECT(hitter)->touch_distance = distance;
	OBJECT(other)->touch_distance = distance;
	OBJECT(hitter)->touched_by = other;
	OBJECT(other)->touched_by = hitter;
	*p_box_edge = box_edge;
	*p_distance = (*p_distance & 0xFFFF0000u) | distance;
	*p_r3 = (*p_r3 & 0xFFFF0000u) | r3;
	*p_r4 = (*p_r4 & 0xFFFF0000u) | r4;
	*p_radius = (*p_radius & 0xFFFF0000u) | radius;
	*p_centre_offset = (*p_centre_offset & 0xFFFF0000u) | centre_offset;
	*p_previous = (*p_previous & 0xFFFF0000u) | previous;
}

void add_to_hitter_list(void)
{
	uint16_t old_head = 0;

	old_head = hitter_list;
	hitter_list = current_object;
	OBJECT(current_object)->hit_list_next = old_head;
	OBJECT(current_object)->flags &= ~4u;
	OBJECT(current_object)->flags &= ~0x8000000u;
}

uint32_t add_to_touchable_list(uint32_t old_head)
{
	old_head = LINKED(old_head, touchable_list);
	touchable_list = current_object;
	OBJECT(current_object)->hit_list_next = old_head;
	OBJECT(current_object)->flags &= ~4u;
	OBJECT(current_object)->flags &= ~0x40u;
	return old_head;
}

void add_to_rider_list(void)
{
	uint16_t old_head = 0;

	old_head = rider_list;
	rider_list = current_object;
	OBJECT(current_object)->platform_list_next = old_head;
	OBJECT(current_object)->flags &= ~8u;
	OBJECT(current_object)->flags &= ~0x20u;
}

void add_to_platform_list(void)
{
	uint16_t old_head = 0;

	old_head = platform_list;
	platform_list = current_object;
	OBJECT(current_object)->platform_list_next = old_head;
	OBJECT(current_object)->flags &= ~8u;
}

void check_platforms(uint32_t *p_middle, uint32_t *p_plank, uint32_t *p_span, uint32_t *p_sag, uint32_t *p_sag_step, uint32_t *p_load_count, uint32_t *p_passes_left, uint32_t *p_platform, uint32_t *p_load_list)
{
	uint32_t middle = *p_middle;
	uint32_t plank = *p_plank;
	uint32_t span = *p_span;
	uint32_t sag = *p_sag;
	uint32_t sag_step = *p_sag_step;
	uint32_t load_count = *p_load_count;
	uint32_t passes_left = *p_passes_left;
	uint32_t load_list = *p_load_list;
	uint16_t plank_count = 0, x_int = 0, y_int = 0;
	uint32_t r8 = 0, r11 = 0, load_list_2 = 0, object = 0, rider = 0;
	uint32_t flags = 0;

	bridge_0_load = 2u;
	bridge_0_load_first = 0u;
	bridge_0_load_last = AS8(bridge_0_plank_count);
	bridge_1_load = 2u;
	bridge_1_load_first = 0u;
	bridge_1_load_last = AS8(bridge_1_plank_count);
	rider = 0x20F01Eu;
	object = 0x20F01Eu;
	if ((int16_t)platform_list < 0)
	{
		rider_list = 0xFFFFu;
		platform_list = 0xFFFFu;
		*p_platform = object;
		return;
	}
	for (;;)
	{
	check_platforms_next_rider:
		if ((int16_t)OBJECT(rider)->platform_list_next < 0)
			break;
		rider = LINKED(rider, OBJECT(rider)->platform_list_next);
		object = LINKED(object, platform_list);
		for (;;)
		{
			x_int = (uint16_t)OBJECT(object)->x_int;
			if (OBJECT(rider)->x_int < (int16_t)x_int || (int16_t)(x_int + OBJECT(object)->hitbox_size_x) < OBJECT(rider)->x_int)
			{
				if ((int16_t)OBJECT(object)->platform_list_next < 0)
					break;
				object = LINKED(object, OBJECT(object)->platform_list_next);
				continue;
			}
			y_int = (uint16_t)OBJECT(object)->y_int;
			if (OBJECT(rider)->y_int < (int16_t)y_int || (int16_t)(y_int + OBJECT(object)->hitbox_size_y) < OBJECT(rider)->y_int)
			{
				if ((int16_t)OBJECT(object)->platform_list_next < 0)
					break;
				object = LINKED(object, OBJECT(object)->platform_list_next);
				continue;
			}
			goto check_platforms_link_rider;
		}
	}
	if (bridge_0_plank_count != 0u)
	{
		load_list_2 = bridge_0_load_ADDRESS;
		current_player = bridge_0_sag_ADDRESS;
		plank_count = bridge_0_plank_count;
		compute_bridge_sag(load_list_2, plank_count, &middle, &plank, &span, &sag, &sag_step, &load_count, &passes_left, &object, &load_list);
	}
	if (bridge_1_plank_count == 0u)
	{
		rider_list = 0xFFFFu;
		platform_list = 0xFFFFu;
		*p_middle = (*p_middle & 0xFFFF0000u) | (middle & 0xFFFFu);
		*p_plank = plank;
		*p_span = span;
		*p_sag = (*p_sag & 0xFFFF0000u) | (sag & 0xFFFFu);
		*p_sag_step = (*p_sag_step & 0xFFFF0000u) | (sag_step & 0xFFFFu);
		*p_load_count = load_count;
		*p_passes_left = passes_left;
		*p_platform = object;
		*p_load_list = load_list;
		return;
	}
	load_list_2 = bridge_1_load_ADDRESS;
	current_player = bridge_1_sag_ADDRESS;
	plank_count = bridge_1_plank_count;
	compute_bridge_sag(load_list_2, plank_count, &middle, &plank, &span, &sag, &sag_step, &r8, &passes_left, &object, &r11);
	rider_list = 0xFFFFu;
	platform_list = 0xFFFFu;
	*p_middle = (*p_middle & 0xFFFF0000u) | (middle & 0xFFFFu);
	*p_plank = plank;
	*p_span = span;
	*p_sag = (*p_sag & 0xFFFF0000u) | (sag & 0xFFFFu);
	*p_sag_step = (*p_sag_step & 0xFFFF0000u) | (sag_step & 0xFFFFu);
	*p_load_count = r8;
	*p_passes_left = passes_left;
	*p_platform = object;
	*p_load_list = r11;
	return;
check_platforms_link_rider:
	OBJECT(rider)->flags |= 8u;
	OBJECT(rider)->platform = object;
	OBJECT(object)->platform = rider;
	if (OBJECT(rider)->z_int < OBJECT(object)->floor_z_int)
		goto check_platforms_next_rider;
	flags = OBJECT(object)->flags;
	OBJECT(object)->flags = flags | 8u;
	if ((flags & 8u) != 0u)
		goto check_platforms_next_rider;
	load_list = bridge_0_load_ADDRESS;
	if (AT8(OBJECT(object)->spawn_arg_2, 1) != 0u)
		load_list = bridge_1_load_ADDRESS;
	mem_write8(load_list, mem_read8(load_list) + 1u);
	load_count = mem_read8(load_list);
	mem_write8(load_list + load_count, AT8(OBJECT(object)->speed, 1));
	goto check_platforms_next_rider;
}

void compute_bridge_sag(uint32_t load_list, uint16_t plank_count, uint32_t *p_middle, uint32_t *p_plank, uint32_t *p_span, uint32_t *p_sag, uint32_t *p_sag_step, uint32_t *p_count, uint32_t *p_passes_left, uint32_t *p_load_entry, uint32_t *p_sag_curve)
{
	uint16_t middle = (uint16_t)*p_middle;
	uint32_t plank = *p_plank;
	uint32_t span = *p_span;
	uint16_t sag = (uint16_t)*p_sag;
	uint16_t sag_step = (uint16_t)*p_sag_step;
	uint32_t passes_left = *p_passes_left;
	uint32_t load_entry = *p_load_entry;
	uint8_t smallest = 0, swapped = 0;
	uint16_t plank_total = 0;
	uint32_t clear_dest = 0, compares_left = 0, count = 0, curve_index = 0, fill_word = 0, next_curve_index = 0, sag_cursor = 0, sag_curve = 0, scan = 0, slot = 0;

	if (mem_read8(load_list) == 2u)
	{
		count = ((uint16_t)plank_count >> 1) + 1u;
		clear_dest = current_player;
		fill_word = 0u;
		do
		{
			mem_write32(clear_dest, fill_word);
			count--;
			clear_dest += 4u;
		} while (count != 0u);
		*p_count = count;
		*p_sag_curve = clear_dest;
		return;
	}
	slot = load_list;
	passes_left = mem_read8(slot);
	passes_left = (uint8_t)(passes_left - 1u);
	slot++;
	do
	{
		compares_left = passes_left;
		smallest = mem_read8(slot);
		scan = slot + 1u;
		do
		{
			if ((int8_t)mem_read8(scan) < (int8_t)smallest)
			{
				swapped = (uint8_t)smallest;
				smallest = mem_read8(scan);
				mem_write8(scan, swapped);
			}
			scan++;
			compares_left--;
		} while (compares_left != 0u);
		mem_write8(slot, smallest);
		passes_left--;
		slot++;
	} while (passes_left != 0u);
	sag_cursor = current_player;
	load_entry = load_list;
	count = mem_read8(load_entry);
	count = (uint8_t)(count - 1u);
	sag_curve = bridge_sag_curve_ADDRESS;
	load_entry++;
	do
	{
		plank = mem_read8(load_entry);
		curve_index = plank;
		middle = (uint16_t)plank_count;
		plank_total = (uint16_t)middle;
		middle = middle >> 1;
		load_entry++;
		if ((int16_t)middle < (int16_t)curve_index)
		{
			curve_index = (uint16_t)(0u - curve_index);
			curve_index = (uint16_t)(curve_index + plank_total);
		}
		sag = ROM8(sag_curve + curve_index);
		span = mem_read8(load_entry);
		next_curve_index = span;
		if ((int16_t)middle < (int16_t)next_curve_index)
		{
			next_curve_index = (uint16_t)(0u - next_curve_index);
			next_curve_index = (uint16_t)(next_curve_index + plank_total);
		}
		sag = (uint16_t)((uint32_t)sag << 8);
		span = (uint16_t)(span - plank);
		sag_step = divide_s16((uint16_t)(((uint32_t)ROM8(sag_curve + next_curve_index) << 8) - sag), span);
		do
		{
			mem_write16(sag_cursor, sag);
			sag += sag_step;
			span--;
			sag_cursor += 2u;
		} while (span != 0u);
		count--;
	} while (count != 0u);
	*p_middle = (*p_middle & 0xFFFF0000u) | middle;
	*p_plank = plank;
	*p_span = span;
	*p_sag = (*p_sag & 0xFFFF0000u) | sag;
	*p_sag_step = (*p_sag_step & 0xFFFF0000u) | sag_step;
	*p_count = count;
	*p_passes_left = passes_left;
	*p_load_entry = load_entry;
	*p_sag_curve = sag_curve;
}

void add_to_wall_list(void)
{
	uint16_t old_head = 0;

	old_head = wall_list;
	wall_list = current_object;
	OBJECT(current_object)->platform_list_next = old_head;
	OBJECT(current_object)->flags &= ~0x20u;
}

uint32_t check_walls(uint32_t hitbox_offset)
{
	uint16_t box_start = 0;
	uint32_t object = 0, rider = 0;

	rider = 0x20F01Eu;
	object = 0x20F01Eu;
	if ((int16_t)wall_list < 0)
	{
		wall_list = 0xFFFFu;
		return hitbox_offset;
	}
	while ((int16_t)OBJECT(rider)->platform_list_next >= 0)
	{
		rider = LINKED(rider, OBJECT(rider)->platform_list_next);
		object = LINKED(object, wall_list);
		for (;;)
		{
			hitbox_offset = (hitbox_offset & ~0xFFFFu) | (uint16_t)OBJECT(object)->hitbox_offset_x;
			box_start = (uint16_t)(OBJECT(object)->x_int + hitbox_offset);
			if (OBJECT(rider)->x_int < (int16_t)box_start || (int16_t)(box_start + OBJECT(object)->hitbox_size_x) < OBJECT(rider)->x_int)
			{
				if ((int16_t)OBJECT(object)->platform_list_next < 0)
					break;
				object = LINKED(object, OBJECT(object)->platform_list_next);
				continue;
			}
			hitbox_offset = (hitbox_offset & ~0xFFFFu) | (uint16_t)OBJECT(object)->hitbox_offset_y;
			box_start = (uint16_t)(OBJECT(object)->y_int + hitbox_offset);
			if (OBJECT(rider)->y_int < (int16_t)box_start || (int16_t)(box_start + OBJECT(object)->hitbox_size_y) < OBJECT(rider)->y_int || OBJECT(object)->hitbox_height <= (uint16_t)(OBJECT(object)->z_int - OBJECT(rider)->z_int))
			{
				if ((int16_t)OBJECT(object)->platform_list_next < 0)
					break;
				object = LINKED(object, OBJECT(object)->platform_list_next);
				continue;
			}
			OBJECT(rider)->flags |= 0x20u;
			OBJECT(object)->flags |= 0x20u;
			OBJECT(rider)->wall = object;
			OBJECT(object)->wall = rider;
			break;
		}
	}
	wall_list = 0xFFFFu;
	return hitbox_offset;
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

void player_state_teetering(uint32_t cursor)
{
	uint8_t new_status = 0, sound_id = 0;
	uint32_t r1 = 0, r7 = 0, r8 = 0, block = 0, depth = 0, edge_direction = 0, ignored = 0, ledge_side = 0, side = 0, teeter_animation = 0, teeter_variant = 0, write_index = 0, write_index_address = 0;
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
			OBJECT(current_object)->y = mem_read32(block + 8u);
			break;
		case 1:
			teeter_variant = 0u;
			OBJECT(current_object)->sprite_flags |= 0x40u;
			OBJECT(current_object)->x = mem_read32(block + 4u);
			OBJECT(current_object)->y -= OBJECT(current_object)->speed_y;
			break;
		case 2:
			teeter_variant = 1u;
			OBJECT(current_object)->sprite_flags |= 0x40u;
			OBJECT(current_object)->x -= OBJECT(current_object)->speed_x;
			OBJECT(current_object)->y = mem_read32(block + 0xCu);
			break;
		case 3:
			teeter_variant = 1u;
			OBJECT(current_object)->sprite_flags &= 0xFFBFu;
			OBJECT(current_object)->x = mem_read32(block);
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
				unknown_code(0x3903u + edge_direction * 2u);
			}
			break;
		default:
			unknown_code(0x3837u + side * 2u);
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
			play_sound(sound_id, &write_index, &write_index_address);
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
	animate(&r1, &r7, &r8);
	project_object(&ignored, &r1);
	update_sinking_floor(r1);
	if ((OBJECT(current_object)->flags & 0x80000u) != 0u)
		return;
	if ((PLAYER(current_player)->flags & 1u) == 0u)
	{
		r1 = queue_sprite_at_base(r1);
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
	uint16_t x_shift = 0;
	uint32_t at_last_step = 0, depth = 0, floor_block_record = 0, ignored = 0, sink_threshold = 0, write_index_address = 0;
	uint8_t health = 0;

	switch (PLAYER(current_player)->substate)
	{
	case 0:
	case 2:
		set_fall_sprite_priority();
		PLAYER(current_player)->fall_draw_depth = ((int32_t)((uint32_t)OBJECT(current_object)->projected_y + OBJECT(current_object)->previous_floor_z) >> 16) - view_y;
		floor_block_record = floor_blocks + 2u + OBJECT(current_object)->floor_block;
		if (mem_read8(floor_block_record + 0x12u) == 0x1Du)
		{
			if (OBJECT(current_object)->x < (int32_t)(((mem_read32(floor_block_record + 4u) - mem_read32(floor_block_record)) >> 1) + mem_read32(floor_block_record)))
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
						PLAYER(current_player)->fall_draw_depth = OBJECT(LINKED(current_object, OBJECT(current_object)->platform))->var_88 - 1u;
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
					PLAYER(current_player)->fall_draw_depth = OBJECT(LINKED(current_object, OBJECT(current_object)->platform))->var_88 - 1u;
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
			play_sound(sound_id, &sink_threshold, &write_index_address);
		}
		PLAYER(current_player)->substate++;
		break;
	case 1:
		break;
	default:
		unknown_code(0x3B56u + PLAYER(current_player)->substate * 3u);
	}
	OBJECT(current_object)->speed_z += 0x3000u;
	OBJECT(current_object)->z += OBJECT(current_object)->speed_z;
	if (OBJECT(current_object)->player_view.floor_block_high >= 0)
	{
		floor_block_record = floor_blocks + 2u + OBJECT(current_object)->floor_block;
		if (mem_read8(floor_block_record + 0x12u) == 0x22u)
		{
			sink_threshold = 0x1DC0000u;
			goto player_falling_off_edge_check_sinking;
		}
		if (mem_read8(floor_block_record + 0x12u) == 0x26u)
		{
			sink_threshold = 0x240000u;
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
				update_screen_position(&ignored, &sink_threshold);
				if (PLAYER(current_player)->ledge_side >= 2 || (OBJECT(current_object)->flags & 0x20000u) == 0u)
				{
					depth = PLAYER(current_player)->fall_draw_depth;
					queue_sprite_at_depth(&ignored, &depth);
					return;
				}
				depth = (uint16_t)(OBJECT(LINKED(current_object, OBJECT(current_object)->touched_by))->var_88 - 1u);
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
	update_screen_position(&ignored, &sink_threshold);
	if (PLAYER(current_player)->ledge_side >= 2 || (OBJECT(current_object)->flags & 0x20000u) == 0u)
	{
		depth = PLAYER(current_player)->fall_draw_depth;
		queue_sprite_at_depth(&ignored, &depth);
		return;
	}
	depth = (uint16_t)(OBJECT(LINKED(current_object, OBJECT(current_object)->touched_by))->var_88 - 1u);
	queue_sprite_at_depth(&ignored, &depth);
}

void player_state_falling_off_edge_spare_return(void)
{
}

void player_state_over_edge(uint32_t jump_offset, uint32_t cursor)
{
	uint8_t new_status = 0;
	uint16_t margin_x = 0, margin_y = 0;
	uint32_t r1 = 0, r2 = 0, r7 = 0, animation = 0, at_last_step = 0, depth = 0, far_hang_animations = 0, ignored = 0, ledge_side = 0, object = 0, off_screen = 0, side = 0, slip_limit = 0;
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
				PLAYER(current_player)->fall_draw_depth = OBJECT(LINKED(current_object, OBJECT(current_object)->platform))->var_88 - 1u;
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
			if (((mem_read16(animation + 2u) | mem_read16(animation + 6u)) & ~0xC0u) == 0u)
				animate_with_flags(&r1, &at_last_step);
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
			depth = (uint16_t)(OBJECT(LINKED(current_object, OBJECT(current_object)->touched_by))->var_88 + 1u);
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
			unknown_code(0x3FE9u + (uint16_t)PLAYER(current_player)->ledge_side * 2u);
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
				unknown_code(0x405Bu + OBJECT(object)->direction * 2u);
			}
		}
		animate(&r1, &r7, &at_last_step);
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
			depth = (uint16_t)(OBJECT(LINKED(current_object, OBJECT(current_object)->touched_by))->var_88 + 1u);
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
		jump_offset = 0x47u;
		if (OBJECT(current_object)->kind != 0u)
		{
			jump_offset = 0x4Fu;
			if (OBJECT(current_object)->kind != 1u)
				jump_offset = 0x67u;
		}
		play_sound((uint8_t)jump_offset, &r1, &r2);
		PLAYER(current_player)->substate++;
	case 5:
		find_floor(&jump_offset, &r1, &r2, &ignored, &cursor);
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
				unknown_code(0x41EDu + OBJECT(object)->direction * 2u);
			}
		}
		if ((int8_t)OBJECT(current_object)->animation_speed >= 0)
		{
			animate(&r1, &r7, &at_last_step);
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
		unknown_code(0x3E05u + ((jump_offset & ~0xFFu) | PLAYER(current_player)->substate) * 3u);
	}
}

void player_state_dropping(uint32_t cursor)
{
	uint8_t sound_id = 0;
	uint32_t r1 = 0, depth = 0, free_count = 0, ignored = 0, object = 0, write_index_address = 0;
	uint8_t health = 0;
	uint16_t state_timer = 0;

	switch (PLAYER(current_player)->substate)
	{
	case 0:
	case 4:
		OBJECT(current_object)->sprite_palette |= 0x8000u;
		set_fall_sprite_priority();
		OBJECT(current_object)->sprite_frame = mem_read16(OBJECT(current_object)->animation + 0x36u);
		PLAYER(current_player)->state_timer = 0x1E;
		PLAYER(current_player)->substate++;
		state_timer = PLAYER(current_player)->state_timer;
		PLAYER(current_player)->state_timer = state_timer - 1u;
		if ((int16_t)state_timer <= 1)
			PLAYER(current_player)->substate++;
		update_screen_position(&ignored, &r1);
		if (PLAYER(current_player)->ledge_side >= 2)
		{
			ignored = queue_sprite_at_previous_floor();
			return;
		}
		if ((PLAYER(current_player)->flags & 2u) != 0u)
		{
			depth = (uint16_t)(OBJECT(LINKED(current_object, OBJECT(current_object)->touched_by))->var_88 - 1u);
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
		depth = OBJECT(LINKED(current_object, OBJECT(current_object)->touched_by))->var_88;
		queue_sprite_at_depth(&ignored, &depth);
		return;
	case 1:
		state_timer = PLAYER(current_player)->state_timer;
		PLAYER(current_player)->state_timer = state_timer - 1u;
		if ((int16_t)state_timer <= 1)
			PLAYER(current_player)->substate++;
		update_screen_position(&ignored, &r1);
		if (PLAYER(current_player)->ledge_side >= 2)
		{
			ignored = queue_sprite_at_previous_floor();
			return;
		}
		if ((PLAYER(current_player)->flags & 2u) != 0u)
		{
			depth = (uint16_t)(OBJECT(LINKED(current_object, OBJECT(current_object)->touched_by))->var_88 - 1u);
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
		depth = OBJECT(LINKED(current_object, OBJECT(current_object)->touched_by))->var_88;
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
		OBJECT(current_object)->sprite_frame = mem_read16(OBJECT(current_object)->animation + 0x38u);
		OBJECT(current_object)->speed_y = 0;
		if (OBJECT(current_object)->kind == 2u)
			cursor = spawn_slip_piece(cursor);
		if (OBJECT(current_object)->kind == 1u)
			spawn_slip_pieces(cursor);
		sound_id = 0x16u;
		play_sound(sound_id, &r1, &write_index_address);
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
					update_screen_position(&ignored, &r1);
					if (PLAYER(current_player)->ledge_side >= 2)
					{
						ignored = queue_sprite_at_previous_floor();
						return;
					}
					if ((PLAYER(current_player)->flags & 2u) != 0u)
					{
						depth = (uint16_t)(OBJECT(LINKED(current_object, OBJECT(current_object)->touched_by))->var_88 - 1u);
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
					depth = OBJECT(object)->var_88;
					queue_sprite_at_depth(&ignored, &depth);
					return;
				}
				OBJECT(current_object)->health = 0;
				PLAYER(current_player)->state = 0x16u;
				PLAYER(current_player)->substate = 0u;
			}
		}
		update_screen_position(&ignored, &r1);
		if (PLAYER(current_player)->ledge_side >= 2)
		{
			ignored = queue_sprite_at_previous_floor();
			return;
		}
		if ((PLAYER(current_player)->flags & 2u) != 0u)
		{
			depth = (uint16_t)(OBJECT(LINKED(current_object, OBJECT(current_object)->touched_by))->var_88 - 1u);
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
		depth = OBJECT(object)->var_88;
		queue_sprite_at_depth(&ignored, &depth);
		return;
	default:
		unknown_code(0x4301u + PLAYER(current_player)->substate * 3u);
	}
}

void player_state_dropping_spare_return(void)
{
}

void player_state_falling_into_pit(void)
{
	uint32_t r8 = 0, depth = 0, floor_block_record = 0, ignored = 0, side = 0, sink_threshold = 0;
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
		if (mem_read8(floor_block_record + 0x12u) == 0x1Du)
		{
			if (OBJECT(current_object)->x < (int32_t)(((mem_read32(floor_block_record + 4u) - mem_read32(floor_block_record)) >> 1) + mem_read32(floor_block_record)))
				goto player_falling_into_pit_depth_minus_10;
			PLAYER(current_player)->fall_draw_depth -= 0x70u;
			goto player_falling_into_pit_depth_minus_10;
		}
		if ((uint16_t)PLAYER(current_player)->ledge_side != 4u)
		{
			if (PLAYER(current_player)->ledge_side < 2)
			{
			player_falling_into_pit_depth_at_platform:
				PLAYER(current_player)->fall_draw_depth = OBJECT(LINKED(current_object, OBJECT(current_object)->platform))->var_88 - 1u;
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
		unknown_code(0x4547u + PLAYER(current_player)->substate * 3u);
	}
	OBJECT(current_object)->speed_z += 0x6000u;
	OBJECT(current_object)->z += OBJECT(current_object)->speed_z;
	if (OBJECT(current_object)->player_view.floor_block_high >= 0)
	{
		floor_block_record = floor_blocks + 2u + OBJECT(current_object)->floor_block;
		if (mem_read8(floor_block_record + 0x12u) == 0x22u)
		{
			sink_threshold = 0x1DC0000u;
			goto player_falling_into_pit_check_sinking;
		}
		if (mem_read8(floor_block_record + 0x12u) == 0x26u)
		{
			sink_threshold = 0x240000u;
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
		animate_with_flags(&sink_threshold, &r8);
	OBJECT(current_object)->x += OBJECT(current_object)->speed_x;
	OBJECT(current_object)->y += OBJECT(current_object)->speed_y;
	AS8(temp_0) = OBJECT(current_object)->sprite_offset_y;
	OBJECT(current_object)->sprite_offset_y = 0x12;
	project_object(&ignored, &ignored);
	OBJECT(current_object)->sprite_offset_y = AS8(temp_0);
	depth = PLAYER(current_player)->fall_draw_depth;
	queue_sprite_at_depth(&ignored, &depth);
}

void player_state_falling_into_pit_spare_return(void)
{
}

void player_state_climbing(uint32_t depth, uint32_t cursor)
{
	uint16_t climb_speed = 0, new_heading = 0, side_threshold = 0, wall_limit = 0, wall_start = 0;
	uint32_t r7 = 0, across_wall = 0, along_wall = 0, at_last_step = 0, attribute_record = 0, climb_animation = 0, climb_over_animation = 0, climb_sideways_animation = 0, floor_block_record = 0, ignored = 0, jump_offset = 0, screen_shift = 0, side_step = 0, step_target = 0, step_x = 0, world_x = 0, world_y = 0, z_step = 0;

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
				unknown_code(0x2D3Eu + PLAYER(current_player)->state * 3u);
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
				unknown_code(0x4755u + PLAYER(current_player)->substate * 3u);
			}
		}
	}
player_climbing_start:
	OBJECT(current_object)->flags &= ~0x40u;
	OBJECT(current_object)->sprite_palette = mem_read16(OBJECT(current_object)->player_view.palette_list + 2u);
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
			OBJECT(current_object)->x = mem_read32(floor_block_record);
		else
			OBJECT(current_object)->y = mem_read32(floor_block_record + 0xCu);
		OBJECT(current_object)->z = OBJECT(current_object)->floor_z;
		PLAYER(current_player)->climb_base_z = OBJECT(current_object)->floor_z;
		project_object(&ignored, &depth);
		PLAYER(current_player)->screen_x_int = OBJECT(current_object)->projected_x;
		AS16(PLAYER(current_player)->screen_x) = 0u;
	player_climbing_begin_climb:
		PLAYER(current_player)->step_accumulator = 0;
		PLAYER(current_player)->substate = 3u;
	player_climbing_move:
		if ((int8_t)OBJECT(current_object)->animation_speed >= 0)
		{
		player_climbing_check_vertical_step:
			if ((int16_t)AT16(PLAYER(current_player)->step_accumulator, 2) >= 4)
			{
				OBJECT(current_object)->flags |= 0x80u;
				goto player_climbing_start_vertical_step;
			}
			if ((int16_t)AT16(PLAYER(current_player)->step_accumulator, 2) <= -4)
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
				animate(&depth, &r7, &at_last_step);
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
				if ((int16_t)AT16(PLAYER(current_player)->step_accumulator, 2) < 0)
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
			if ((int16_t)AT16(PLAYER(current_player)->side_step_accumulator, 2) < 0)
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
			if ((int16_t)side_threshold <= (int16_t)AT16(PLAYER(current_player)->side_step_accumulator, 2))
			{
				OBJECT(current_object)->flags &= ~0x80u;
				goto player_climbing_side_step;
			}
			if ((int16_t)AT16(PLAYER(current_player)->side_step_accumulator, 2) <= (int16_t)(0u - side_threshold))
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
				AS32(temp_0) = step_x;
				side_step = (int32_t)side_step >> 1;
				if (ROM8(attribute_record) != 0u)
					side_step = 0u - side_step;
				side_step += OBJECT(current_object)->projected_y;
				AT32(temp_1, 2) = side_step;
				side_step = side_step * 2u;
				unproject_point(&step_x, &side_step);
				if (ROM8(attribute_record) != 0u)
					side_step = step_x;
				side_step = (int32_t)side_step >> 16;
				wall_start = ROM16(attribute_record + 0xAu);
				if ((int16_t)wall_start <= (int16_t)side_step && (int16_t)side_step <= (int16_t)(wall_start + ROM16(attribute_record + 0xCu)) && (uint16_t)(((int32_t)(AT32(temp_1, 2) + OBJECT(current_object)->z + depth) >> 16) - view_y - 0x10u) < 0xC0u)
				{
					PLAYER(current_player)->saved_screen_x = PLAYER(current_player)->screen_x;
					PLAYER(current_player)->saved_screen_y = OBJECT(current_object)->projected_y;
					PLAYER(current_player)->screen_x = AS32(temp_0);
					OBJECT(current_object)->projected_x = PLAYER(current_player)->screen_x_int;
					OBJECT(current_object)->projected_y = AT32(temp_1, 2);
					animate_step(&depth, &r7, &step_x);
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
		AS8(temp_0) = at_last_step;
		update_screen_position(&ignored, &depth);
		if (AS8(temp_0) != 0u)
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
				OBJECT(current_object)->x = mem_read32(floor_block_record + 4u);
				OBJECT(current_object)->x_int += 0xAu;
			}
			else
			{
				OBJECT(current_object)->y = mem_read32(floor_block_record + 8u);
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
	AS16(PLAYER(current_player)->screen_x) = 0u;
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
	uint32_t r8 = 0, ground_slowdown = 0, ignored = 0, skid_animations = 0, skid_facing = 0, speed_slowdown = 0, write_index_address = 0;

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
		play_sound(sound_id, &depth, &write_index_address);
		PLAYER(current_player)->substate++;
		player_animate_skidding_slow_down(cursor);
		return;
	case 1:
		player_animate_skidding_slow_down(cursor);
		return;
	case 2:
		animate_with_flags(&depth, &r8);
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
			depth = queue_sprite_at_base(depth);
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
		unknown_code(0x4EF5u + PLAYER(current_player)->substate * 3u);
	}
}

void player_animate_skidding_slow_down(uint32_t cursor)
{
	uint32_t r1 = 0, r8 = 0, depth = 0, ignored = 0;
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
		animate_with_flags(&r1, &r8);
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
		PLAYER(current_player)->continue_frames = 0;
		PLAYER(current_player)->continue_seconds = 0;
		PLAYER(current_player)->state_timer = 0;
	}
}

void player_state_short_animation(void)
{
	uint32_t r7 = 0, at_last_step = 0, frame = 0, ignored = 0, state_1b_animation = 0;
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
		OBJECT(current_object)->sprite_palette = mem_read16(OBJECT(current_object)->player_view.palette_list + 2u);
	}
	if ((int8_t)OBJECT(current_object)->animation_frame >= 0)
	{
		animate(&frame, &r7, &at_last_step);
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
	uint32_t r8 = 0, frame = 0, ignored = 0, running_animations = 0, speed_int = 0;

	if (PLAYER(current_player)->substate == 0u)
	{
		PLAYER(current_player)->substate++;
		OBJECT(current_object)->speed_z = -0x80000;
		if (OBJECT(current_object)->floor_type == 6u)
		{
			speed_int = (uint16_t)OBJECT(current_object)->speed_int;
			OBJECT(current_object)->speed_z_int = hop_speeds_by_speed[speed_int >> 8];
		}
		OBJECT(current_object)->sprite_palette = mem_read16(OBJECT(current_object)->player_view.palette_list + 2u);
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
	animate_with_flags(&frame, &r8);
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
		AS16(PLAYER(current_player)->screen_x) = 0u;
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
		OBJECT(current_object)->sprite_palette = mem_read16(OBJECT(current_object)->player_view.palette_list + 2u);
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
		AS8(PLAYER(current_player)->previous_z) = 0u;
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
	uint32_t r8 = 0, depth = 0, frame = 0, ignored = 0;

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
		unknown_code(0x5437u + PLAYER(current_player)->substate * 3u);
	}
	animate_with_flags(&frame, &r8);
	AS8(temp_0) = OBJECT(current_object)->sprite_offset_y;
	OBJECT(current_object)->sprite_offset_y = 0x12;
	project_object(&ignored, &depth);
	OBJECT(current_object)->sprite_offset_y = AS8(temp_0);
	keep_player_on_screen(depth);
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
	OBJECT(current_object)->sprite_palette = mem_read16(OBJECT(current_object)->player_view.palette_list + 2u);
	OBJECT(current_object)->flags &= ~0x2000u;
	PLAYER(current_player)->control_lock = 0u;
	OBJECT(current_object)->flags |= 0x1000u;
	PLAYER(current_player)->substate++;
	player_launched_fly();
}

void player_launched_fly(void)
{
	uint32_t block_offset = 0, cursor = 0, ignored = 0, x = 0, y = 0;

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
	find_floor(&x, &y, &block_offset, &ignored, &cursor);
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
	uint32_t at_last_step = 0, gravity = 0, hurt_animations = 0, ignored = 0, jump_offset = 0, kind = 0, knock_speed = 0, knock_speed_x = 0, knock_speed_y = 0, object = 0, shocked_animations = 0, step_record = 0;
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
		push_player_out_of_wall(&jump_offset, &depth, &cursor);
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
				jump_offset = OBJECT(object)->direction;
				OBJECT(current_object)->direction = jump_offset;
				OBJECT(current_object)->speed = knock_speed_x << 8;
				if (jump_offset != 0u)
				{
					if (jump_offset != 0x200u)
					{
						if (jump_offset != 0x100u)
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
			face_movement_direction(jump_offset);
			sound_id = 0x21u;
			play_sound(sound_id, &depth, &knock_speed_x);
			sound_id = 0x55u;
			if (OBJECT(current_object)->kind != 0u)
			{
				sound_id = 0x4Au;
				if (OBJECT(current_object)->kind != 1u)
					sound_id = 0x5Bu;
			}
			play_sound(sound_id, &depth, &knock_speed_x);
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
			OBJECT(current_object)->sprite_flags |= mem_read16(step_record);
			OBJECT(current_object)->sprite_frame = mem_read16(step_record + 2u);
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
			find_floor(&gravity, &depth, &knock_speed_x, &ignored, &cursor);
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
					play_sound(sound_id, &depth, &knock_speed_x);
					OBJECT(current_object)->animation_frame++;
					step_record = OBJECT(current_object)->animation + 2u + OBJECT(current_object)->animation_frame * 4u;
					OBJECT(current_object)->sprite_flags &= ~0xC0u;
					OBJECT(current_object)->sprite_flags |= mem_read16(step_record);
					OBJECT(current_object)->sprite_frame = mem_read16(step_record + 2u);
					add_player_to_lists();
					project_object(&ignored, &depth);
					if (current_game_mode != 0xDu)
						keep_player_on_screen(depth);
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
				add_player_to_lists();
				project_object(&ignored, &depth);
				if (current_game_mode != 0xDu)
					keep_player_on_screen(depth);
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
				unknown_code(0x2D3Eu + PLAYER(current_player)->state * 3u);
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
			find_floor(&jump_offset, &depth, &knock_speed_x, &ignored, &cursor);
			project_object(&ignored, &depth);
			keep_player_on_screen(depth);
			add_player_to_lists();
			depth = queue_sprite_at_base(depth);
			return;
		case 4:
			jump_offset = 0x6000u;
			if (OBJECT(current_object)->floor_type == 4u)
				jump_offset = jump_offset >> 2;
			OBJECT(current_object)->speed_z += jump_offset;
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
					animate_with_flags(&depth, &at_last_step);
					if (OBJECT(current_object)->animation_frame == 5u)
						OBJECT(current_object)->animation_speed = 0xFFu;
				}
				if ((int8_t)OBJECT(current_object)->player_view.hurt_flash_period >= 0)
					flash_hurt_palette();
				find_floor(&jump_offset, &depth, &knock_speed_x, &ignored, &cursor);
				if (current_game_mode == 0x1Au)
				{
					project_object(&ignored, &depth);
					keep_player_on_screen(depth);
					add_player_to_lists();
					ignored = queue_sprite_at_floor();
					return;
				}
				check_fall_into_pit();
				if (PLAYER(current_player)->state != 9u)
				{
					project_object(&ignored, &depth);
					keep_player_on_screen(depth);
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
					find_floor(&jump_offset, &depth, &knock_speed_x, &ignored, &cursor);
					OBJECT(current_object)->z = OBJECT(current_object)->floor_z;
					project_object(&ignored, &depth);
					keep_player_on_screen(depth);
					add_player_to_lists();
					if ((OBJECT(current_object)->flags & 0x80000u) != 0u)
						return;
					depth = queue_sprite_at_base(depth);
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
					find_floor(&jump_offset, &depth, &knock_speed_x, &ignored, &cursor);
					OBJECT(current_object)->z = OBJECT(current_object)->floor_z;
					project_object(&ignored, &depth);
					keep_player_on_screen(depth);
					add_player_to_lists();
					if ((OBJECT(current_object)->flags & 0x80000u) != 0u)
						return;
					depth = queue_sprite_at_base(depth);
					return;
				}
			}
			update_player_blink();
			find_floor(&jump_offset, &depth, &knock_speed_x, &ignored, &cursor);
			OBJECT(current_object)->z = OBJECT(current_object)->floor_z;
			project_object(&ignored, &depth);
			keep_player_on_screen(depth);
			add_player_to_lists();
			if ((OBJECT(current_object)->flags & 0x80000u) != 0u)
				return;
			depth = queue_sprite_at_base(depth);
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
					switch (AT8(OBJECT(object)->spawn_arg_2, 1))
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
						unknown_code(0x5AE7u + AT8(OBJECT(object)->spawn_arg_2, 1) * 2u);
					}
				}
				else
				{
					OBJECT(current_object)->speed_x = 0;
					OBJECT(current_object)->speed_y = 0;
					OBJECT(current_object)->direction = 0x300u;
				}
				face_movement_direction(jump_offset);
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
				play_sound(sound_id, &depth, &knock_speed_x);
				sound_id = 0x2Fu;
				if (OBJECT(current_object)->kind != 0u)
				{
					sound_id = 0x37u;
					if (OBJECT(current_object)->kind != 1u)
						sound_id = 0x77u;
				}
				play_sound(sound_id, &depth, &knock_speed_x);
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
				OBJECT(current_object)->sprite_palette = mem_read16(OBJECT(current_object)->player_view.palette_list + 2u);
				goto player_hurt_set_animation;
			}
			PLAYER(current_player)->state = 0x16u;
			PLAYER(current_player)->substate = 0u;
			goto player_dispatch_state;
		case 8:
			goto player_hurt_shocked;
		default:
			unknown_code(0x55C8u + PLAYER(current_player)->substate * 3u);
		}
	}
player_hurt_land_after_hop:
	PLAYER(current_player)->substate++;
	OBJECT(current_object)->z = OBJECT(current_object)->floor_z;
	OBJECT(current_object)->animation_timer = 0;
player_hurt_get_up:
	OBJECT(current_object)->animation_speed = 3u;
	animate_with_flags(&depth, &at_last_step);
	if ((uint8_t)at_last_step != 0u)
	{
		PLAYER(current_player)->substate++;
		PLAYER(current_player)->mishap_flag = 0u;
		OBJECT(current_object)->sprite_palette = mem_read16(OBJECT(current_object)->player_view.palette_list + 2u);
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
	find_floor(&jump_offset, &depth, &knock_speed_x, &ignored, &cursor);
	OBJECT(current_object)->z = OBJECT(current_object)->floor_z;
	project_object(&ignored, &depth);
	keep_player_on_screen(depth);
	add_player_to_lists();
	depth = queue_sprite_at_base(depth);
}

void player_state_hurt_spare_return(void)
{
}

void player_state_bouncing(void)
{
	uint8_t sound_id = 0;
	uint32_t r0 = 0, r1 = 0, r2 = 0, r7 = 0, r11 = 0, at_last_step = 0, bounce_record = 0, bounce_records = 0, depth = 0, gravity = 0, ignored = 0, new_heading = 0, rebound_speed = 0;
	uint16_t state_timer = 0;

	push_player_out_of_wall(&r0, &r1, &r11);
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
		face_movement_direction(r0);
		OBJECT(current_object)->speed = 0x1000000;
		OBJECT(current_object)->speed_z = -0x68000;
		PLAYER(current_player)->bounce_speed_z = -0x68000;
		PLAYER(current_player)->bounce_step = 0x34000;
		PLAYER(current_player)->bounce_speed_z += PLAYER(current_player)->bounce_step;
		set_speed_from_direction();
		OBJECT(current_object)->sprite_frame = mem_read16(OBJECT(current_object)->animation + 2u);
		OBJECT(current_object)->animation_frame = 0u;
		OBJECT(current_object)->animation_timer = 3;
		OBJECT(current_object)->animation_speed = 3u;
		PLAYER(current_player)->state_timer = 0x14;
		sound_id = 0x25u;
		play_sound(sound_id, &r1, &r2);
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
		find_floor(&gravity, &r1, &r2, &ignored, &r11);
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
				OBJECT(current_object)->sprite_frame = mem_read16(OBJECT(current_object)->animation + 2u + OBJECT(current_object)->animation_frame * 2u);
			}
			update_player_blink();
			add_to_rider_list();
			project_object(&ignored, &depth);
			keep_player_on_screen(depth);
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
				OBJECT(current_object)->sprite_frame = mem_read16(OBJECT(current_object)->animation + 2u + OBJECT(current_object)->animation_frame * 2u);
			}
		}
		update_player_blink();
		add_to_rider_list();
		project_object(&ignored, &depth);
		keep_player_on_screen(depth);
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
			animate(&r1, &r7, &at_last_step);
			if ((uint8_t)at_last_step != 0u)
				OBJECT(current_object)->animation_speed = 0xFFu;
		}
		update_player_blink();
		update_screen_position(&ignored, &r1);
		keep_player_on_screen(r1);
		if ((OBJECT(current_object)->flags & 0x80000u) != 0u)
			return;
		r1 = queue_sprite_at_base(r1);
		return;
	default:
		unknown_code(0x5C0Fu + PLAYER(current_player)->substate * 3u);
	}
}

void player_state_out_of_game(void)
{
	uint16_t cell = 0;
	uint32_t joined = 0, player = 0, player_number = 0, text = 0, text_position = 0;

	if ((uint16_t)PLAYER(current_player)->state_timer == 0u)
	{
		player_status[OBJECT(current_object)->player] = 0u;
		player = OBJECT(current_object)->player;
		text_position = 0x310000u | hud_name_positions_3p[player];
		if ((cabinet_config & 1u) == 0u)
			text_position = (text_position & ~0xFFFFu) | hud_name_positions_2p[player];
		text = hud_mighty_name_text_ADDRESS;
		cell = 0u;
		erase_text_here(cell, &text, &text_position);
		text = hud_game_over_text_ADDRESS;
		player_number = OBJECT(current_object)->player;
		text_position = 0x310000u | hud_game_over_positions_3p[player_number];
		if ((cabinet_config & 1u) == 0u)
			text_position = (text_position & ~0xFFFFu) | hud_game_over_positions_2p[player_number];
		print_text_here(&player_number, &text, &text_position);
		PLAYER(current_player)->stage_rings = 0u;
	}
	if (joining_closed != 0u)
	{
		PLAYER(current_player)->state_timer++;
		if (PLAYER(current_player)->state_timer < 0x78)
			return;
		PLAYER(current_player)->state = 0xBu;
	}
	else
	{
		joined = try_join();
		if (joined == 0u)
		{
			PLAYER(current_player)->state_timer++;
			if (PLAYER(current_player)->state_timer < 0x78)
				return;
			PLAYER(current_player)->state = 0xBu;
			text = hud_game_over_text_ADDRESS;
			player = OBJECT(current_object)->player;
			text_position = 0x310000u | hud_game_over_positions_3p[player];
			if ((cabinet_config & 1u) == 0u)
				text_position = (text_position & ~0xFFFFu) | hud_game_over_positions_2p[player];
			cell = 0u;
			erase_text_here(cell, &text, &text_position);
			return;
		}
		OBJECT(current_object)->handler = player_join_ADDRESS;
		PLAYER(current_player)->substate = 0u;
		PLAYER(current_player)->is_joining = 0xFFu;
		PLAYER(current_player)->in_game = 1;
		PLAYER(current_player)->rings = 0u;
		PLAYER(current_player)->stages_reached = 1u;
	}
	text = hud_game_over_text_ADDRESS;
	player = OBJECT(current_object)->player;
	text_position = 0x310000u | hud_game_over_positions_3p[player];
	if ((cabinet_config & 1u) == 0u)
		text_position = (text_position & ~0xFFFFu) | hud_game_over_positions_2p[player];
	cell = 0u;
	erase_text_here(cell, &text, &text_position);
}

void player_state_continue(void)
{
	uint8_t cabinet_event = 0, cost = 0;
	uint16_t attribute = 0, cell = 0;
	uint32_t character_bit = 0, coins_per_credit = 0, credit_block = 0, credits = 0, ignored = 0, joined = 0, player = 0, player_2 = 0, player_number = 0, start_bit = 0, text = 0, text_position = 0, value = 0;
	uint16_t saved_credits_per_start = 0;
	uint8_t continue_seconds = 0;

	if (AS16(PLAYER(current_player)->continue_frames) == 0x900u)
	{
		player_status[OBJECT(current_object)->player] = 2u;
		OBJECT(current_object)->flags |= 0x400u;
		text = hud_ring_count_blank_text_ADDRESS;
		player = OBJECT(current_object)->player;
		text_position = 0x310000u | hud_ring_count_positions_3p[player];
		if ((cabinet_config & 1u) == 0u)
			text_position = (text_position & ~0xFFFFu) | hud_ring_count_positions_2p[player];
		cell = 0u;
		erase_text_here(cell, &text, &text_position);
		player_number = OBJECT(current_object)->player;
		text_position = 0x310000u | hud_continue_positions_3p[player_number];
		if ((cabinet_config & 1u) == 0u)
			text_position = (text_position & ~0xFFFFu) | hud_continue_positions_2p[player_number];
		text = hud_continue_text_ADDRESS;
		print_text_here(&player_number, &text, &text_position);
		record_player_game();
		cabinet_event = 0u;
		if (OBJECT(current_object)->player != 0u)
		{
			cabinet_event = 2u;
			if (OBJECT(current_object)->player != 1u)
				cabinet_event = 4u;
		}
		mem_write8(cabinet_events, cabinet_event);
		character_bit = 0u;
		if (OBJECT(current_object)->kind != 0u)
		{
			character_bit = 1u;
			if (OBJECT(current_object)->kind != 1u)
				character_bit = 2u;
		}
		characters_taken &= ~shl32(1u, (int)(character_bit));
	}
	if (joining_closed != 0u)
	{
		PLAYER(current_player)->state = 0x2Eu;
		PLAYER(current_player)->continue_frames = 0;
		PLAYER(current_player)->continue_seconds = 0;
		PLAYER(current_player)->state_timer = 0;
		text = hud_message_blank_text_ADDRESS;
		player = OBJECT(current_object)->player;
		text_position = 0x310000u | hud_text_positions_3p[player];
		if ((cabinet_config & 1u) == 0u)
			text_position = (text_position & ~0xFFFFu) | hud_text_positions_2p[player];
		erase_text_lines(&text, &text_position);
	}
	else
	{
		switch (cabinet_config)
		{
		case 0:
		case 1:
		case 4:
			credit_block = p1_coins_ADDRESS;
			break;
		case 2:
		case 3:
			credit_block = p1_coins_ADDRESS + OBJECT(current_object)->player * 4u;
			break;
		default:
			unknown_code(0x5FF0u + cabinet_config * 3u);
		}
		player = OBJECT(current_object)->player;
		text_position = 0x310000u | credit_positions_3p[player];
		if ((cabinet_config & 1u) == 0u)
			text_position = (text_position & ~0xFFFFu) | credit_positions_2p[player];
		draw_credit_count_now(credit_block, &credits, &coins_per_credit, &ignored, &text_position);
		value = (uint8_t)PLAYER(current_player)->continue_seconds;
		attribute = 0u;
		player_2 = OBJECT(current_object)->player;
		text_position = 0x310000u | hud_continue_positions_3p[player_2];
		if ((cabinet_config & 1u) == 0u)
			text_position = (text_position & ~0xFFFFu) | hud_continue_positions_2p[player_2];
		text_position += 0x14u;
		draw_hex_2(value, attribute, text_position);
		saved_credits_per_start = credits_per_start;
		credits_per_start = 1u;
		draw_player_start_prompt();
		credits_per_start = saved_credits_per_start;
		if (PLAYER(current_player)->continue_seconds < 7)
		{
			switch (OBJECT(current_object)->player)
			{
			case 0:
			case 3:
				start_bit = 0x10u;
				break;
			case 1:
				start_bit = 0x18u;
				break;
			case 2:
				start_bit = 0x12u;
				break;
			default:
				unknown_code(0x609Eu + OBJECT(current_object)->player * 2u);
			}
			if ((inputs_pressed & shl32(1u, (int)(start_bit))) != 0u)
				PLAYER(current_player)->continue_frames = 0x3C;
		}
		cost = 1u;
		joined = try_continue(cost);
		if (joined != 0u)
		{
			OBJECT(current_object)->handler = player_join_ADDRESS;
			PLAYER(current_player)->substate = 0u;
			PLAYER(current_player)->is_joining = 0xFFu;
			PLAYER(current_player)->in_game = 1;
			text = hud_message_blank_text_ADDRESS;
			player = OBJECT(current_object)->player;
			text_position = 0x310000u | hud_text_positions_3p[player];
			if ((cabinet_config & 1u) == 0u)
				text_position = (text_position & ~0xFFFFu) | hud_text_positions_2p[player];
			erase_text_lines(&text, &text_position);
			return;
		}
		PLAYER(current_player)->continue_frames++;
		if (PLAYER(current_player)->continue_frames >= 0x3C)
		{
			PLAYER(current_player)->continue_frames = 0;
			continue_seconds = PLAYER(current_player)->continue_seconds;
			PLAYER(current_player)->continue_seconds = continue_seconds - 1u;
			if ((int8_t)continue_seconds <= 1)
			{
				PLAYER(current_player)->state = 0x2Eu;
				PLAYER(current_player)->continue_frames = 0;
				PLAYER(current_player)->continue_seconds = 0;
				PLAYER(current_player)->state_timer = 0;
				text = hud_message_blank_text_ADDRESS;
				player = OBJECT(current_object)->player;
				text_position = 0x310000u | hud_text_positions_3p[player];
				if ((cabinet_config & 1u) == 0u)
					text_position = (text_position & ~0xFFFFu) | hud_text_positions_2p[player];
				erase_text_lines(&text, &text_position);
				return;
			}
		}
	}
}

void player_state_in_zone(uint32_t depth, uint32_t cursor)
{
	uint8_t next_state = 0;
	uint16_t kind = 0, x_int = 0, y_int = 0, z_int = 0;
	uint32_t ignored = 0, jump_offset = 0, speed_diff = 0, steps_left = 0, trackball_speed = 0, trackball_y = 0;
	uint32_t speed = 0, target_speed = 0;

	for (;;)
	{
		switch (PLAYER(current_player)->substate)
		{
		case 0:
		case 2:
			PLAYER(current_player)->substate++;
			OBJECT(current_object)->z = OBJECT(current_object)->floor_z;
			OBJECT(current_object)->flags &= ~0x40u;
			OBJECT(current_object)->sprite_palette = mem_read16(OBJECT(current_object)->player_view.palette_list + 2u);
			OBJECT(current_object)->flags &= ~0x2000u;
			PLAYER(current_player)->control_lock = 0u;
			OBJECT(current_object)->direction = 0x100u;
			face_movement_direction(jump_offset);
			add_player_to_lists();
			if (PLAYER(current_player)->effect_spawned == 0u)
			{
				x_int = (uint16_t)OBJECT(current_object)->x_int;
				y_int = (uint16_t)OBJECT(current_object)->y_int;
				kind = 0xBu;
				z_int = (uint16_t)OBJECT(current_object)->z_int;
				spawn_sign_2(x_int, y_int, kind, z_int, cursor);
				PLAYER(current_player)->effect_spawned = 0xFFu;
			}
			depth = queue_sprite_at_base(depth);
			return;
		case 1:
			check_player_contacts(&depth, &cursor);
			if (PLAYER(current_player)->state != 0xDu && PLAYER(current_player)->state != 0x20u)
				goto player_in_zone_steer;
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
				continue;
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
			break;
		default:
			unknown_code(0x615Bu + PLAYER(current_player)->substate * 3u);
		}
	}
player_in_zone_steer:
	read_player_trackball();
	if ((PLAYER(current_player)->trackball_x | PLAYER(current_player)->trackball_y) != 0u || OBJECT(current_object)->speed_int >= 0x20)
	{
		trackball_speed = PLAYER(current_player)->trackball_x;
		trackball_y = PLAYER(current_player)->trackball_y;
		trackball_speed = trackball_speed * trackball_speed + trackball_y * trackball_y;
		square_root(&trackball_speed, &steps_left);
		trackball_speed &= 0xFFFFu;
		trackball_speed = trackball_speed * trackball_speed;
		if ((int32_t)trackball_speed > 0x5000000)
			trackball_speed = 0x5000000u;
		speed = OBJECT(current_object)->speed;
		target_speed = trackball_speed;
		speed_diff = target_speed - speed;
		if ((int32_t)target_speed <= (int32_t)speed)
			OBJECT(current_object)->speed += (int32_t)speed_diff >> 5;
		else
			OBJECT(current_object)->speed += speed_diff;
		PLAYER(current_player)->animation_speed = OBJECT(current_object)->speed;
		next_state = 0u;
		depth = (steps_left & ~0xFFFFu) | AT16(PLAYER(current_player)->animation_speed, 2);
		if ((uint16_t)depth != 0u)
		{
			next_state = 1u;
			if ((int16_t)depth > 0x200)
			{
				next_state = 0x13u;
				if ((int16_t)depth > 0x4F0)
					next_state = 0x14u;
			}
		}
		PLAYER(current_player)->state = next_state;
	}
	else
	{
		OBJECT(current_object)->speed = 0;
		PLAYER(current_player)->state = 0u;
	}
	update_screen_position(&ignored, &depth);
	update_player_blink();
	player_animate(depth, cursor);
	PLAYER(current_player)->free_turn_timer = 0x10u;
	if ((OBJECT(current_object)->flags & 0x8000000u) != 0u)
		PLAYER(current_player)->state = 0x22u;
	add_player_to_lists();
}

void player_state_entering_stage(uint32_t depth, uint32_t cursor)
{
	uint8_t sound_id = 0;
	uint32_t write_index_address = 0;

	switch (stage)
	{
	case 0:
	case 1:
	case 2:
	case 5:
	case 6:
	case 7:
	case 8:
	case 9:
	case 17:
	case 18:
	case 19:
		return;
	case 3:
	case 10:
	case 11:
	case 12:
	case 13:
	case 14:
	case 15:
	case 16:
		switch (PLAYER(current_player)->entry_step)
		{
		case 0:
		case 2:
			PLAYER(current_player)->path_request = (uint8_t)stage_player_entries[stage];
			OBJECT(current_object)->flags |= 0x4000000u;
			PLAYER(current_player)->entry_step++;
			PLAYER(current_player)->state = PLAYER(current_player)->previous_state;
			player_move(depth, cursor);
			return;
		case 1:
			break;
		default:
			unknown_code(0x6345u + PLAYER(current_player)->entry_step * 3u);
		}
		if (PLAYER(current_player)->path == 0u)
		{
			OBJECT(current_object)->flags &= ~0x4000000u;
			PLAYER(current_player)->enter_on_respawn = 0u;
			PLAYER(current_player)->state = 0u;
			PLAYER(current_player)->substate = 0u;
		}
		PLAYER(current_player)->state = PLAYER(current_player)->previous_state;
		player_move(depth, cursor);
		return;
	case 4:
		switch (PLAYER(current_player)->entry_step)
		{
		case 0:
		case 2:
			PLAYER(current_player)->path_request = (uint8_t)stage_player_entries[stage];
			OBJECT(current_object)->flags |= 0x4000000u;
			PLAYER(current_player)->entry_step++;
			sound_id = 0x64u;
			if (OBJECT(current_object)->kind != 0u)
			{
				sound_id = 0x6Bu;
				if (OBJECT(current_object)->kind != 1u)
					sound_id = 0x51u;
			}
			play_sound(sound_id, &depth, &write_index_address);
			PLAYER(current_player)->state = PLAYER(current_player)->previous_state;
			player_move(depth, cursor);
			return;
		case 1:
			if (PLAYER(current_player)->path == 0u)
			{
				OBJECT(current_object)->flags &= ~0x4000000u;
				PLAYER(current_player)->enter_on_respawn = 0u;
				PLAYER(current_player)->state = 0u;
				PLAYER(current_player)->substate = 0u;
			}
			PLAYER(current_player)->state = PLAYER(current_player)->previous_state;
			player_move(depth, cursor);
			return;
		default:
			unknown_code(0x62EBu + PLAYER(current_player)->entry_step * 3u);
		}
		break;
	default:
		unknown_code(0x62A3u + stage * 3u);
	}
}

void player_state_stage_exit(uint32_t depth, uint32_t cursor)
{
	uint8_t sound_id = 0;
	uint16_t dy = 0;
	uint32_t goal_angle = 0, goal_dx = 0, goal_dy = 0, ignored = 0, write_index_address = 0;
	uint16_t state_timer = 0;

	switch (stage)
	{
	case 0:
	case 1:
	case 2:
	case 3:
	case 5:
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
		switch (PLAYER(current_player)->goal_step)
		{
		case 0:
		case 3:
			PLAYER(current_player)->goal_step++;
			OBJECT(current_object)->flags |= 0x4000000u;
			PLAYER(current_player)->control_lock = 0u;
			PLAYER(current_player)->path_request = stage_goal_exit_paths[stage_goals[AS8(stage)]];
			stage_exit_started = 0xFFu;
			if (stage != 5u && stage != 0xCu && stage != 0xDu && stage != 0xEu && stage != 0xFu && stage != 0xBu)
				PLAYER(current_player)->stages_reached++;
			OBJECT(current_object)->z = OBJECT(current_object)->floor_z;
			player_move(depth, cursor);
			return;
		case 1:
			if (PLAYER(current_player)->path_request != 0u || PLAYER(current_player)->path != 0u)
			{
				PLAYER(current_player)->state = PLAYER(current_player)->previous_state;
				player_move(depth, cursor);
				return;
			}
			PLAYER(current_player)->goal_step++;
			players_finished[OBJECT(current_object)->player] = 0xFFu;
			return;
		case 2:
			PLAYER(current_player)->flags |= 8u;
			return;
		default:
			unknown_code(0x63F1u + PLAYER(current_player)->goal_step * 3u);
		}
		break;
	case 4:
		switch (PLAYER(current_player)->goal_step)
		{
		case 0:
		case 5:
			PLAYER(current_player)->goal_step++;
			PLAYER(current_player)->state_timer = 0x168;
			stage_exit_started = 0xFFu;
			OBJECT(current_object)->z = OBJECT(current_object)->floor_z;
			OBJECT(current_object)->sprite_palette = mem_read16(OBJECT(current_object)->player_view.palette_list + 2u);
			OBJECT(current_object)->flags &= ~0x40u;
			OBJECT(current_object)->flags &= ~0x2000u;
			OBJECT(current_object)->flags &= ~0x80000u;
			OBJECT(current_object)->flags &= 0xDFFFFFFFu;
			PLAYER(current_player)->stages_reached++;
			goal_angle = (uint16_t)(0x1225u - OBJECT(current_object)->x_int);
			dy = (uint16_t)(0x1439u - OBJECT(current_object)->y_int);
			angle_of_vector(dy, &goal_angle, &ignored);
			OBJECT(current_object)->direction = goal_angle;
			face_movement_direction(goal_angle);
			OBJECT(current_object)->speed = 0x4000000;
			PLAYER(current_player)->animation_speed = OBJECT(current_object)->speed;
			set_speed_from_direction();
			OBJECT(current_object)->animation_timer = 0;
			player_animate_standing();
			return;
		case 1:
			state_timer = PLAYER(current_player)->state_timer;
			PLAYER(current_player)->state_timer = state_timer - 1u;
			if ((int16_t)state_timer <= 1)
				PLAYER(current_player)->goal_step++;
			update_screen_position(&ignored, &depth);
			OBJECT(current_object)->animation_timer = 2;
			player_animate_standing();
			return;
		case 2:
			OBJECT(current_object)->x += OBJECT(current_object)->speed_x;
			OBJECT(current_object)->y += OBJECT(current_object)->speed_y;
			project_object(&ignored, &ignored);
			goal_dx = (int16_t)(0x1225u - OBJECT(current_object)->x_int);
			goal_dy = (int16_t)(0x1439u - OBJECT(current_object)->y_int);
			if ((int32_t)(goal_dy * goal_dy + goal_dx * goal_dx) <= 0x6400)
			{
				PLAYER(current_player)->goal_step++;
				OBJECT(current_object)->speed_z = -0x40000;
				PLAYER(current_player)->flags |= 8u;
				sound_id = 0x16u;
				play_sound(sound_id, &goal_dy, &write_index_address);
			}
			player_animate_running();
			return;
		case 3:
			OBJECT(current_object)->speed_z += 0x6000u;
			OBJECT(current_object)->z += OBJECT(current_object)->speed_z;
			OBJECT(current_object)->x += OBJECT(current_object)->speed_x;
			OBJECT(current_object)->y += OBJECT(current_object)->speed_y;
			project_object(&ignored, &ignored);
			if (OBJECT(current_object)->screen_y >= 0xB8)
			{
				PLAYER(current_player)->goal_step++;
				players_finished[OBJECT(current_object)->player] = 0xFFu;
			}
			if (OBJECT(current_object)->speed_z_int >= 0)
			{
				depth = 0xFFF0u;
				queue_sprite_at_depth(&ignored, &depth);
				return;
			}
			ignored = queue_sprite_at_base(ignored);
			return;
		case 4:
			PLAYER(current_player)->flags |= 8u;
			return;
		default:
			unknown_code(0x74A8u + PLAYER(current_player)->goal_step * 3u);
		}
		break;
	default:
		unknown_code(0x63AAu + stage * 3u);
	}
}

void player_state_tumbling(uint32_t depth, uint32_t cursor)
{
	uint16_t r0_2 = 0;
	uint32_t r0 = 0, r7 = 0, at_last_step = 0, ignored = 0, knock_speed_x = 0, knock_speed_y = 0, knocked_out_animations = 0, object = 0, sprite_depth = 0;
	uint8_t hurt_contact_delay = 0, health = 0;
	uint16_t state_timer = 0, state_timer_2 = 0;
	uint32_t push_speed_3 = 0, push_speed_4 = 0;

	hurt_contact_delay = PLAYER(current_player)->hurt_contact_delay;
	PLAYER(current_player)->hurt_contact_delay = hurt_contact_delay - 1u;
	if ((int8_t)hurt_contact_delay <= 1)
	{
		PLAYER(current_player)->hurt_contact_delay = 0;
		check_player_contacts(&depth, &cursor);
	}
	if (stage != 8u || scroll_position >= 0x3800u)
		push_player_out_of_wall(&r0, &depth, &cursor);
	PLAYER(current_player)->state = 0x20u;
	switch (PLAYER(current_player)->substate)
	{
	case 0:
	case 9:
		if (stage != 8u || scroll_position >= 0x4000u)
		{
			health = OBJECT(current_object)->health;
			OBJECT(current_object)->health = health - 4u;
			if ((int8_t)health <= 4)
				OBJECT(current_object)->health = 0;
		}
		PLAYER(current_player)->invulnerable_timer = 0x258;
		OBJECT(current_object)->flags &= ~0x1000000u;
	player_tumbling_start:
		OBJECT(current_object)->flags &= ~4u;
		OBJECT(current_object)->flags &= ~0x40u;
		OBJECT(current_object)->flags &= ~0x2000u;
		PLAYER(current_player)->control_lock = 0u;
		PLAYER(current_player)->mishap_flag = 0xFFu;
		PLAYER(current_player)->hurt_contact_delay = 0x78;
		PLAYER(current_player)->substate = 2u;
		OBJECT(current_object)->sprite_flags &= ~0xFu;
		OBJECT(current_object)->sprite_flags |= 4u;
		if ((uint8_t)OBJECT(current_object)->health == 0u)
			player_status[OBJECT(current_object)->player] = 1u;
		OBJECT(current_object)->speed_z = -0x80000;
		object = LINKED(WORK_RAM, OBJECT(current_object)->touched_by);
		if (OBJECT(object)->kind == 0x17u)
		{
			OBJECT(current_object)->speed = 0;
			OBJECT(current_object)->speed_x = 0;
			OBJECT(current_object)->speed_y = 0;
			player_tumbling_launch(object, cursor);
			return;
		}
		knock_speed_x = 0x40000u;
		knock_speed_y = 0u;
		if (OBJECT(current_object)->floor_type != 0xDu)
			r0_2 = OBJECT(object)->direction;
		else
			r0_2 = 0x100u;
		if (r0_2 != 0u)
		{
			if (r0_2 != 0x200u)
			{
				if (r0_2 != 0x100u)
					knock_speed_x = 0u - knock_speed_x;
			}
			else
			{
				push_speed_3 = knock_speed_x;
				knock_speed_x = knock_speed_y;
				knock_speed_y = push_speed_3;
			}
		}
		else
		{
			push_speed_4 = knock_speed_x;
			knock_speed_x = knock_speed_y;
			knock_speed_y = 0u - push_speed_4;
		}
		OBJECT(current_object)->speed_x = knock_speed_x;
		OBJECT(current_object)->speed_y = knock_speed_y;
		player_tumbling_set_launch_speed(object, cursor);
		return;
	case 1:
		OBJECT(current_object)->flags |= 0x1000000u;
		goto player_tumbling_start;
	case 2:
		player_tumbling_fly();
		return;
	case 3:
	case 5:
		state_timer = PLAYER(current_player)->state_timer;
		PLAYER(current_player)->state_timer = state_timer - 1u;
		if ((int16_t)state_timer <= 1)
		{
			PLAYER(current_player)->substate++;
			OBJECT(current_object)->speed_z = -0x40000;
			OBJECT(current_object)->animation_timer = 0;
			OBJECT(current_object)->animation_speed = 6u;
			OBJECT(current_object)->animation_frame = 8u;
			OBJECT(current_object)->speed_x = (int32_t)OBJECT(current_object)->speed_x >> 1;
			OBJECT(current_object)->speed_y = (int32_t)OBJECT(current_object)->speed_y >> 1;
		}
		if ((int8_t)OBJECT(current_object)->player_view.hurt_flash_period >= 0)
			flash_hurt_palette();
		find_floor(&r0, &depth, &knock_speed_x, &ignored, &cursor);
		project_object(&ignored, &sprite_depth);
		keep_player_on_screen(sprite_depth);
		add_player_to_lists();
		sprite_depth = queue_sprite_at_base(sprite_depth);
		PLAYER(current_player)->draw_depth = sprite_depth;
		return;
	case 4:
		OBJECT(current_object)->speed_z += 0x6000u;
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
				animate_with_flags(&depth, &at_last_step);
				if (OBJECT(current_object)->animation_frame == 9u)
					OBJECT(current_object)->animation_frame = 5u;
				if (OBJECT(current_object)->animation_frame == 7u)
					OBJECT(current_object)->animation_speed = 0xFFu;
			}
		}
		else
		{
			PLAYER(current_player)->substate++;
			OBJECT(current_object)->z = OBJECT(current_object)->floor_z;
			OBJECT(current_object)->speed_z = -0x40000;
			OBJECT(current_object)->animation_timer = 0;
			OBJECT(current_object)->animation_frame = 7u;
			animate_step_with_flags(&depth, &at_last_step);
		}
		if ((int8_t)OBJECT(current_object)->player_view.hurt_flash_period >= 0)
			flash_hurt_palette();
		find_floor(&r0, &depth, &knock_speed_x, &ignored, &cursor);
		project_object(&ignored, &depth);
		keep_player_on_screen(depth);
		add_player_to_lists();
		depth = queue_sprite_at_floor();
		PLAYER(current_player)->draw_depth = depth;
		return;
	case 6:
		OBJECT(current_object)->speed_z += 0x6000u;
		PLAYER(current_player)->previous_z = OBJECT(current_object)->z;
		OBJECT(current_object)->z += OBJECT(current_object)->speed_z;
		PLAYER(current_player)->previous_x = OBJECT(current_object)->x;
		PLAYER(current_player)->previous_y = OBJECT(current_object)->y;
		OBJECT(current_object)->x += OBJECT(current_object)->speed_x;
		OBJECT(current_object)->y += OBJECT(current_object)->speed_y;
		if (OBJECT(current_object)->z_int < OBJECT(current_object)->floor_z_int)
		{
			if ((int8_t)OBJECT(current_object)->animation_frame >= 0)
			{
				animate_with_flags(&depth, &at_last_step);
				if ((uint8_t)at_last_step != 0u)
					OBJECT(current_object)->animation_frame = 0xFFu;
			}
		}
		else
		{
			PLAYER(current_player)->substate++;
			OBJECT(current_object)->z = OBJECT(current_object)->floor_z;
			PLAYER(current_player)->mishap_flag = 0u;
			OBJECT(current_object)->sprite_palette = mem_read16(OBJECT(current_object)->player_view.palette_list + 2u);
			OBJECT(current_object)->player_view.hurt_flash_timer = 0x7Fu;
			PLAYER(current_player)->state_timer = 0x1E;
			if ((OBJECT(current_object)->flags & 0x1000000u) == 0u)
			{
				OBJECT(current_object)->flags |= 0x20000000u;
				PLAYER(current_player)->invulnerable_timer = 0x78;
			}
			if (OBJECT(current_object)->health <= 0)
			{
				PLAYER(current_player)->substate = 8u;
				knocked_out_animations = sonic_knocked_out_animations_ADDRESS;
				depth = 0xA90u;
				if (OBJECT(current_object)->kind != 0u)
				{
					knocked_out_animations = ray_knocked_out_animations_ADDRESS;
					depth = 0xAA0u;
					if (OBJECT(current_object)->kind != 1u)
					{
						knocked_out_animations = mighty_knocked_out_animations_ADDRESS;
						depth = 0xA80u;
					}
				}
				OBJECT(current_object)->sprite_palette &= ~0xFFFu;
				OBJECT(current_object)->sprite_palette |= depth;
				r0 = PLAYER(current_player)->facing;
				cursor = knocked_out_animations + (r0 >> 2) * 4u;
				OBJECT(current_object)->sprite_flags &= ~0x40u;
				OBJECT(current_object)->sprite_flags |= ROM16(cursor);
				OBJECT(current_object)->animation = knocked_out_animations + ROM16(cursor + 2u);
				OBJECT(current_object)->animation_frame = 0xFFu;
				OBJECT(current_object)->animation_timer = 0;
				OBJECT(current_object)->animation_speed = 3u;
			}
		}
		if ((int8_t)OBJECT(current_object)->player_view.hurt_flash_period >= 0)
			flash_hurt_palette();
		find_floor(&r0, &depth, &knock_speed_x, &ignored, &cursor);
		project_object(&ignored, &depth);
		keep_player_on_screen(depth);
		add_player_to_lists();
		depth = queue_sprite_at_floor();
		PLAYER(current_player)->draw_depth = depth;
		return;
	case 7:
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
				find_floor(&r0, &depth, &knock_speed_x, &ignored, &cursor);
				OBJECT(current_object)->z = OBJECT(current_object)->floor_z;
				project_object(&ignored, &depth);
				keep_player_on_screen(depth);
				add_player_to_lists();
				if ((OBJECT(current_object)->flags & 0x80000u) != 0u)
					return;
				depth = queue_sprite_at_base(depth);
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
				find_floor(&r0, &depth, &knock_speed_x, &ignored, &cursor);
				OBJECT(current_object)->z = OBJECT(current_object)->floor_z;
				project_object(&ignored, &depth);
				keep_player_on_screen(depth);
				add_player_to_lists();
				if ((OBJECT(current_object)->flags & 0x80000u) != 0u)
					return;
				depth = queue_sprite_at_base(depth);
				return;
			}
		}
		update_player_blink();
		find_floor(&r0, &depth, &knock_speed_x, &ignored, &cursor);
		OBJECT(current_object)->z = OBJECT(current_object)->floor_z;
		project_object(&ignored, &depth);
		keep_player_on_screen(depth);
		add_player_to_lists();
		if ((OBJECT(current_object)->flags & 0x80000u) != 0u)
			return;
		depth = queue_sprite_at_base(depth);
		return;
	case 8:
		animate(&depth, &r7, &at_last_step);
		if ((uint8_t)at_last_step != 0u)
		{
			PLAYER(current_player)->state = 0x16u;
			PLAYER(current_player)->substate = 0u;
			OBJECT(current_object)->sprite_palette = mem_read16(OBJECT(current_object)->player_view.palette_list + 2u);
		}
		project_object(&ignored, &depth);
		keep_player_on_screen(depth);
		ignored = queue_sprite_at_floor();
		return;
	default:
		unknown_code(0x64D9u + PLAYER(current_player)->substate * 3u);
	}
}

void player_tumbling_knock_with_toucher_speed(uint32_t toucher, uint32_t effect)
{
	OBJECT(current_object)->speed_x = mem_read32(toucher + 0x34u);
	OBJECT(current_object)->speed_x = (uint32_t)OBJECT(current_object)->speed_x << 1;
	OBJECT(current_object)->speed_y = mem_read32(toucher + 0x38u);
	OBJECT(current_object)->speed_y = (uint32_t)OBJECT(current_object)->speed_y << 1;
	player_tumbling_set_launch_speed(toucher, effect);
}

void player_tumbling_set_launch_speed(uint32_t toucher, uint32_t effect)
{
	OBJECT(current_object)->speed = 0x4000000;
	player_tumbling_launch(toucher, effect);
}

void player_tumbling_launch(uint32_t toucher, uint32_t effect)
{
	uint8_t sound_id = 0;
	uint32_t tumble_animations = 0, write_index = 0, write_index_address = 0;

	if (OBJECT(current_object)->floor_type != 0xDu)
		OBJECT(current_object)->direction = OBJECT(toucher)->direction;
	else
		OBJECT(current_object)->direction = 0x100u;
	face_movement_direction(0u);
	tumble_animations = sonic_tumble_animations_ADDRESS;
	if (OBJECT(current_object)->kind != 0u)
	{
		tumble_animations = ray_tumble_animations_ADDRESS;
		if (OBJECT(current_object)->kind != 1u)
			tumble_animations = mighty_tumble_animations_ADDRESS;
	}
	OBJECT(current_object)->animation = tumble_animations + ROM16(tumble_animations + PLAYER(current_player)->facing * 2u);
	OBJECT(current_object)->animation_frame = 0u;
	OBJECT(current_object)->animation_timer = 0;
	OBJECT(current_object)->animation_speed = 4u;
	OBJECT(current_object)->sprite_palette = mem_read16(OBJECT(current_object)->player_view.palette_list + 2u);
	OBJECT(current_object)->player_view.hurt_flash_period = 3u;
	OBJECT(current_object)->player_view.hurt_flash_timer = 3u;
	OBJECT(current_object)->player_view.hurt_flash_on = 0u;
	sound_id = 0x3Au;
	if (OBJECT(current_object)->kind != 0u)
	{
		sound_id = 0x61u;
		if (OBJECT(current_object)->kind != 1u)
			sound_id = 0x6Eu;
	}
	play_sound(sound_id, &write_index, &write_index_address);
	spawn_tumble_effect(effect);
	player_tumbling_fly();
}

void player_tumbling_fly(void)
{
	uint8_t pose_frame = 0, sound_id = 0;
	uint32_t r1 = 0, r2 = 0, r8 = 0, cursor = 0, depth = 0, ignored = 0, x = 0;

	OBJECT(current_object)->speed_z += 0x6000u;
	PLAYER(current_player)->previous_z = OBJECT(current_object)->z;
	OBJECT(current_object)->z += OBJECT(current_object)->speed_z;
	PLAYER(current_player)->previous_x = OBJECT(current_object)->x;
	PLAYER(current_player)->previous_y = OBJECT(current_object)->y;
	OBJECT(current_object)->x += OBJECT(current_object)->speed_x;
	OBJECT(current_object)->y += OBJECT(current_object)->speed_y;
	find_floor(&x, &r1, &r2, &ignored, &cursor);
	if ((int16_t)(OBJECT(current_object)->z_int - OBJECT(current_object)->floor_z_int) >= 8)
	{
		OBJECT(current_object)->x = PLAYER(current_player)->previous_x;
		OBJECT(current_object)->y = PLAYER(current_player)->previous_y;
		OBJECT(current_object)->floor_block = OBJECT(current_object)->previous_floor_block;
		OBJECT(current_object)->floor_z = OBJECT(current_object)->previous_floor_z;
	}
	if (PLAYER(current_player)->hurt_contact_delay > 0x50 || OBJECT(current_object)->z_int < OBJECT(current_object)->floor_z_int)
	{
		if (OBJECT(current_object)->speed_z_int < 6)
		{
			animate_with_flags(&r1, &r8);
			if (OBJECT(current_object)->animation_frame == 5u)
				OBJECT(current_object)->animation_frame = 0xFFu;
		}
		else
		{
			pose_frame = 5u;
			if (OBJECT(current_object)->speed_z_int >= 7)
				pose_frame = 6u;
			OBJECT(current_object)->animation_frame = pose_frame;
			animate_step_with_flags(&ignored, &r8);
		}
	}
	else
	{
		PLAYER(current_player)->substate++;
		PLAYER(current_player)->state_timer = 2;
		OBJECT(current_object)->z = OBJECT(current_object)->floor_z;
		OBJECT(current_object)->animation_frame = 7u;
		animate_step_with_flags(&r1, &r8);
		sound_id = 0x20u;
		play_sound(sound_id, &r1, &r2);
	}
	add_player_to_lists();
	project_object(&ignored, &depth);
	keep_player_on_screen(depth);
	if (OBJECT(current_object)->floor_block >= 0)
		depth = queue_sprite_at_floor();
	else
		depth = queue_sprite_at_previous_floor();
	PLAYER(current_player)->draw_depth = depth;
}

void player_state_blown_away(uint32_t cursor)
{
	uint8_t sound_id = 0;
	uint32_t r1 = 0, r8 = 0, depth = 0, free_count = 0, ignored = 0, tumble_animations = 0, write_index_address = 0;
	uint8_t health = 0;

	if (PLAYER(current_player)->substate == 0u)
	{
		PLAYER(current_player)->substate = 0xFFu;
		tumble_animations = sonic_tumble_animations_ADDRESS;
		if (OBJECT(current_object)->kind != 0u)
		{
			tumble_animations = ray_tumble_animations_ADDRESS;
			if (OBJECT(current_object)->kind != 1u)
				tumble_animations = mighty_tumble_animations_ADDRESS;
		}
		OBJECT(current_object)->animation = tumble_animations + ROM16(tumble_animations + 4u);
		OBJECT(current_object)->animation_frame = 0u;
		OBJECT(current_object)->animation_speed = 4u;
		OBJECT(current_object)->animation_timer = 0;
		OBJECT(current_object)->speed_z = -0x80000;
		OBJECT(current_object)->speed_x = -0x10000;
		OBJECT(current_object)->speed_y = 0;
		PLAYER(current_player)->mishap_flag = 0xFFu;
		sound_id = 0x3Au;
		if (OBJECT(current_object)->kind != 0u)
		{
			sound_id = 0x61u;
			if (OBJECT(current_object)->kind != 1u)
				sound_id = 0x6Eu;
		}
		play_sound(sound_id, &r1, &write_index_address);
		allocate_object(&free_count, &cursor);
		if (free_count != 0u)
		{
			OBJECT(cursor)->sprite_flags = 0x1004u;
			OBJECT(cursor)->sprite_palette = 0x8360u;
			OBJECT(cursor)->sprite_offset_x = 0;
			OBJECT(cursor)->sprite_offset_y = 0;
			OBJECT(cursor)->flags = 0u;
			OBJECT(cursor)->handler = blown_away_effect_update_ADDRESS;
			OBJECT(cursor)->animation = blown_away_effect_animation_ADDRESS;
			OBJECT(cursor)->animation_frame = 0u;
			OBJECT(cursor)->animation_speed = 4u;
			OBJECT(cursor)->animation_timer = 0;
			OBJECT(cursor)->parent = current_object;
		}
	}
	animate_with_flags(&r1, &r8);
	if ((uint8_t)OBJECT(current_object)->animation_timer == OBJECT(current_object)->animation_speed && OBJECT(current_object)->animation_frame == 5u)
		OBJECT(current_object)->animation_frame = 0xFFu;
	OBJECT(current_object)->x += OBJECT(current_object)->speed_x;
	OBJECT(current_object)->y += OBJECT(current_object)->speed_y;
	OBJECT(current_object)->speed_z += 0x6000u;
	OBJECT(current_object)->z += OBJECT(current_object)->speed_z;
	project_object(&ignored, &depth);
	if (OBJECT(current_object)->screen_y < 0x108)
	{
		depth = queue_sprite(depth);
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
	depth = queue_sprite(depth);
}

void blown_away_effect_update(void)
{
	uint32_t r7 = 0, r8 = 0, depth = 0, frame = 0, ignored = 0, object = 0;

	object = LINKED(current_object, OBJECT(current_object)->parent);
	OBJECT(current_object)->screen_x = OBJECT(object)->screen_x;
	OBJECT(current_object)->screen_y = OBJECT(object)->screen_y;
	if (OBJECT(current_object)->screen_y >= 0x100)
	{
		delete_current_object();
		return;
	}
	animate(&frame, &r7, &r8);
	if ((uint8_t)OBJECT(current_object)->animation_timer == OBJECT(current_object)->animation_speed && OBJECT(current_object)->animation_frame == 2u)
		OBJECT(current_object)->animation_frame = 0xFFu;
	depth = (uint16_t)(OBJECT(current_object)->screen_y + 0x10u);
	queue_sprite_at_depth(&ignored, &depth);
}

void player_state_intro_turn(uint32_t depth, uint32_t cursor)
{
	if ((uint8_t)intro_character != OBJECT(current_object)->kind)
		return;
	if (PLAYER(current_player)->scene_step_seen == 0u)
	{
		PLAYER(current_player)->scene_step_seen = 0xFFu;
		PLAYER(current_player)->path_request = 0x14u;
	}
	PLAYER(current_player)->state = PLAYER(current_player)->previous_state;
	player_move(depth, cursor);
}

void player_state_opening_scene(void)
{
	uint8_t sound_id = 0;
	uint16_t character_frame = 0, shake = 0, sprite_height = 0, width = 0;
	uint32_t r1 = 0, r7 = 0, depth = 0, ignored = 0, layout_row = 0, object = 0, opening_run_animation = 0, opening_voice_animation = 0, players_left = 0, position_record = 0, sprite_scale = 0, status_cursor = 0, write_index_address = 0;
	uint8_t status = 0;

	switch (scene_step)
	{
	case 0:
	case 2:
		return;
	case 1:
	case 4:
		if (PLAYER(current_player)->scene_step_seen != scene_step)
		{
			PLAYER(current_player)->scene_step_seen = scene_step;
			opening_run_animation = sonic_opening_run_animation_ADDRESS;
			if (OBJECT(current_object)->kind != 0u)
			{
				opening_run_animation = ray_opening_run_animation_ADDRESS;
				if (OBJECT(current_object)->kind != 1u)
					opening_run_animation = mighty_opening_run_animation_ADDRESS;
			}
			OBJECT(current_object)->animation = opening_run_animation;
			OBJECT(current_object)->animation_frame = 0u;
			OBJECT(current_object)->animation_speed = 2u;
			OBJECT(current_object)->animation_timer = 0;
			status_cursor = player_status_ADDRESS;
			players_left = 3u;
			layout_row = 0xFFFFFFFFu;
			do
			{
				status = mem_read8(status_cursor);
				status_cursor++;
				if (status == 0xFFu)
					layout_row++;
				players_left--;
			} while (players_left != 0u);
			position_record = opening_scene_player_positions_ADDRESS + layout_row * 0x12u + OBJECT(current_object)->player * 3u * 2u;
			OBJECT(current_object)->screen_x = mem_read16(position_record);
			position_record += 2u;
			OBJECT(current_object)->projected_y_int = mem_read16(position_record);
			OBJECT(current_object)->projected_y_fraction = 0u;
			position_record += 2u;
			AT16(PLAYER(current_player)->scene_zoom, 2) = mem_read16(position_record);
			AS16(PLAYER(current_player)->scene_zoom) = 0u;
			position_record += 2u;
			spawn_intro_marker(position_record);
		}
		if ((uint16_t)scene_timer < 0x12Cu)
		{
			PLAYER(current_player)->scene_zoom += 0x6000u;
			OBJECT(current_object)->projected_y += 0x3000u;
		}
		OBJECT(current_object)->sprite_scale = AT16(PLAYER(current_player)->scene_zoom, 2);
		OBJECT(current_object)->screen_y = OBJECT(current_object)->projected_y_int;
		shake = AT8(stage_06_shake, 1);
		if (stage_06_shake_sign < 0)
			shake = (uint16_t)(0u - shake);
		OBJECT(current_object)->screen_y += shake;
		if ((uint16_t)scene_timer == 0x140u)
		{
			opening_voice_animation = sonic_opening_voice_animation_ADDRESS;
			if (OBJECT(current_object)->kind != 0u)
			{
				opening_voice_animation = ray_opening_voice_animation_ADDRESS;
				if (OBJECT(current_object)->kind != 1u)
					opening_voice_animation = mighty_opening_voice_animation_ADDRESS;
			}
			OBJECT(current_object)->animation = opening_voice_animation;
			sound_id = 0x36u;
			if (OBJECT(current_object)->kind != 0u)
			{
				sound_id = 0x46u;
				if (OBJECT(current_object)->kind != 1u)
					sound_id = 0x77u;
			}
			play_sound(sound_id, &r1, &write_index_address);
		}
		animate(&r1, &r7, &players_left);
		depth = (uint16_t)OBJECT(current_object)->screen_y;
		if (OBJECT(current_object)->player != 1u && (int16_t)depth >= 0xED)
			depth = 0xEDu;
		queue_sprite_at_depth(&ignored, &depth);
		return;
	case 3:
		if (PLAYER(current_player)->scene_step_seen != scene_step)
		{
			PLAYER(current_player)->scene_step_seen = scene_step;
			OBJECT(current_object)->sprite_flags = 0u;
			OBJECT(current_object)->sprite_palette = mem_read16(OBJECT(current_object)->player_view.palette_list + 2u);
			character_frame = 0x1B58u;
			if (OBJECT(current_object)->kind != 0u)
			{
				character_frame = 0x5278u;
				if (OBJECT(current_object)->kind != 1u)
					character_frame = 0x36C0u;
			}
			OBJECT(current_object)->sprite_frame = character_frame;
			width = 0u;
			sprite_height = 0x30u;
			if (OBJECT(current_object)->player != 0u)
			{
				width = 0xFFE0u;
				sprite_height = 0x2Eu;
				if (OBJECT(current_object)->player != 1u)
				{
					width = 0x20u;
					sprite_height = 0x2Eu;
				}
			}
			OBJECT(current_object)->sprite_width = width;
			OBJECT(current_object)->sprite_height = sprite_height;
		}
		object = (current_object & ~0xFFFFu) | opening_machine_object;
		OBJECT(current_object)->sprite_scale = OBJECT(object)->sprite_scale;
		OBJECT(current_object)->screen_x = OBJECT(object)->screen_x;
		OBJECT(current_object)->screen_y = OBJECT(object)->screen_y;
		sprite_scale = OBJECT(current_object)->sprite_scale;
		OBJECT(current_object)->screen_x += (int32_t)(OBJECT(current_object)->sprite_width * sprite_scale) >> 8;
		OBJECT(current_object)->screen_y += (int32_t)(OBJECT(current_object)->sprite_height * sprite_scale) >> 8;
		depth = (uint16_t)(OBJECT(object)->screen_y - 8u);
		queue_sprite_at_depth(&ignored, &depth);
		return;
	default:
		unknown_code(0x6C19u + scene_step * 3u);
	}
}

void player_state_opening_scene_spare_return(void)
{
}

void shadow_update_spare_return(void)
{
}
