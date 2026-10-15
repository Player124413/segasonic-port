/**
 *    Name:        test_mode/tests.c
 *    Description: the test mode: start, the menu, the memory test, the input test, the sound test
 *    Version:     0.1.0
 *    Created:     12-10-2026
 *    Modified:    15-10-2026
 *    Author:      Sebastian Toyos
 *
 *    SegaSonic The Hedgehog (arcade, Sega System 32): the game's V60 program decompiled into C by
 *    tools/decompile.py, with the names of symbols.txt.
 */
#include "game.h"

void start_test_mode(uint32_t *p_color, uint32_t *p_blocks_per_row)
{
	uint32_t color = *p_color;
	uint32_t blocks_per_row = *p_blocks_per_row;
	uint8_t is_8bpp = 0, mode = 0, sound_id = 0;
	uint16_t scroll_x = 0;
	uint32_t r0 = 0, r8 = 0, r10 = 0, ignored = 0, inner_count = 0, scroll_y = 0;
	uint8_t cnt_bits = 0, port_d_bits = 0, seat_port_bits = 0;

	cnt_bits = mem_read8(io_cnt);
	mem_write8(io_cnt, cnt_bits & ~2u);
	show_eeprom_write_count = 0u;
	if ((0x1001u & input_ports_active) != 0u)
		show_eeprom_write_count = 1u;
	display_off_frames = 0x14u;
	load_default_palettes();
	init_video_registers();
	init_sprites(&r0, &r10);
	init_sound();
	port_d_bits = mem_read8(io_port_d);
	mem_write8(io_port_d, port_d_bits & ~8u);
	sound_id = 0x94u;
	queue_sound(sound_id, &ignored, &ignored);
	r8 = wait_for_frames();
	reset_objects(&ignored, &ignored);
	clear_sprite_buckets();
	mode = 0u;
	set_screen_mode(mode);
	clear_text_layer(&r0, &ignored, &ignored);
	clear_bitmap_layer((uint8_t)r0, &scroll_y, &color, &inner_count, &r8, &blocks_per_row, &r10);
	mem_write16(video_layer_disable, 0xFu);
	scroll_x = 0u;
	scroll_y &= ~0xFFFFu;
	is_8bpp = 1u;
	set_bitmap_scroll(scroll_x, (uint16_t)scroll_y, is_8bpp);
	test_mode = 0xFFu;
	test_screen = 0u;
	test_cursor = 0u;
	test_continue_mode = 0u;
	test_screen_ready = 0u;
	seat_port_bits = mem_read8(io_port_d);
	mem_write8(io_port_d, seat_port_bits & ~0xCu);
	color_offset_red = 0u;
	color_offset_blue = 0u;
	color_offset_green = 0u;
	color_offset_2_red = 0u;
	color_offset_2_blue = 0u;
	color_offset_2_green = 0u;
	backup_clear_timer = 0u;
	mem_write16(mixer_bitmap, 0x21u);
	mem_write32(video_control, mem_read32(video_control) | 0x800u);
	fill_value = (fill_value & ~0xFFFFu) | 0x909u;
	set_halfwords(VIDEO_RAM, 0xE000u, fill_value);
	run_test_mode(scroll_y, (uint16_t)r8, &color, &blocks_per_row);
	*p_color = color;
	*p_blocks_per_row = blocks_per_row;
}

void run_test_mode(uint32_t pressed, uint16_t count, uint32_t *p_color, uint32_t *p_blocks_per_row)
{
	uint32_t color = *p_color;
	uint32_t blocks_per_row = *p_blocks_per_row;

	switch (test_screen)
	{
	case 0:
		test_menu();
		break;
	case 1:
		toggle_test_continue_mode();
		break;
	case 2:
		test_memory(&color, &blocks_per_row);
		break;
	case 3:
		test_inputs(count, &color, &blocks_per_row);
		break;
	case 4:
		test_sound();
		break;
	case 5:
		test_crt(pressed, &color, &blocks_per_row);
		break;
	case 6:
		test_game_assignments(&color, &blocks_per_row);
		break;
	case 7:
		color = test_coin_assignments(color);
		break;
	case 8:
		test_bookkeeping();
		break;
	case 9:
		color = test_backup_clear(color);
		break;
	case 10:
		test_outputs();
		break;
	case 11:
		exit_test_mode();
		break;
	case 12:
		test_menu();
		break;
	default:
		unknown_code(0x9CA0Eu + test_screen * 3u);
	}
	*p_color = color;
	*p_blocks_per_row = blocks_per_row;
}

void test_menu(void)
{
	uint32_t cursor_entry = 0, cursor_positions = 0, ignored = 0;

	cursor_positions = test_menu_cursor_positions_ADDRESS;
	erase_test_cursor(cursor_positions);
	if (test_screen_ready == 0u)
	{
		test_screen_ready++;
		clear_text_layer(&ignored, &ignored, &ignored);
		test_cursor = 0xAu;
		draw_test_menu();
		return;
	}
	if ((inputs_pressed & 1u) != 0u || (inputs_pressed & 0x1000u) != 0u)
	{
		test_cursor++;
		if (mem_read8(moving_seat_dip) == 0u && test_cursor == 9u)
			test_cursor++;
	}
	for (;;)
	{
		cursor_entry = test_menu_cursor_positions_ADDRESS + ((uint32_t)test_cursor << 1);
		if ((int8_t)ROM8(cursor_entry) >= 0)
			break;
		test_cursor = 0u;
	}
	test_cursor_column = ROM8(cursor_entry);
	test_cursor_row = ROM8(cursor_entry + 1u);
	blink_test_cursor();
	if ((inputs_pressed & 2u) != 0u || (inputs_pressed & 0x2000u) != 0u)
	{
		test_screen = test_cursor;
		test_screen++;
		test_menu_item = test_cursor;
		test_screen_ready = 0u;
	}
}

void draw_test_menu(void)
{
	uint16_t attribute = 0;
	uint32_t ignored = 0, print_position = 0, text = 0, write_count_bcd = 0;

	test_cursor_positions = test_menu_cursor_positions_ADDRESS;
	clear_text_layer(&ignored, &ignored, &print_position);
	if (show_eeprom_write_count != 0u)
	{
		write_count_bcd = AS16(eeprom_write_count);
		convert_to_bcd(&write_count_bcd, &ignored);
		write_count_bcd &= 0xFFFFFu;
		print_position = TEXT_CELL(31, 26);
		attribute = 0x1C00u;
		draw_hex_8(write_count_bcd, attribute, print_position);
	}
	text = test_menu_text_ADDRESS;
	print_text_list(&text, &print_position);
	if (mem_read8(moving_seat_dip) != 0u)
	{
		text = test_menu_output_test_text_ADDRESS;
		print_text_list(&text, &print_position);
	}
}

void toggle_test_continue_mode(void)
{
	uint32_t text = 0, text_position = 0;

	text = individual_mode_text_ADDRESS;
	test_continue_mode = ~test_continue_mode;
	if (test_continue_mode != 0u)
		text = continue_mode_text_ADDRESS;
	print_text_list(&text, &text_position);
	test_screen = 0u;
	test_screen_ready = 1u;
}

void test_memory(uint32_t *p_results_left, uint32_t *p_blocks_per_row)
{
	uint32_t results_left = *p_results_left;
	uint32_t blocks_per_row = *p_blocks_per_row;
	uint8_t is_8bpp = 0, mode = 0;
	uint16_t scroll_x = 0, scroll_y = 0;
	uint32_t r0 = 0, r4 = 0, r5 = 0, r10 = 0, r11 = 0, delay_count = 0, ignored = 0, offset_entry = 0, result_index = 0, row_pairs_left = 0, text = 0;
	uint8_t cnt_bits = 0;

	if (test_screen_ready == 0u)
	{
		interrupts_off();
		test_screen_ready++;
		clear_text_layer(&ignored, &ignored, &r11);
		text = memory_test_text_ADDRESS;
		print_text_list(&text, &r11);
		text = memory_test_dashes_text_ADDRESS;
		print_text_list(&text, &r11);
		memory_test_errors = 0u;
		mem_write8(sound_cpu_hold, 1u);
		delay_count = 0x1000u;
		do
		{
			delay_count--;
		} while (delay_count != 0u);
		check_rom_checksums();
		test_work_ram();
		test_video_ram();
		test_sprite_ram();
		test_palette_ram();
		test_sound_ram();
		cnt_bits = mem_read8(io_cnt);
		mem_write8(io_cnt, cnt_bits & ~2u);
		interrupts_on();
		display_off_frames = 0x78u;
		delay_count = 0x3Cu;
		do
		{
			delay_count--;
		} while (delay_count != 0u);
		load_default_palettes();
		load_text_font(&r4, &r5);
		init_video_registers();
		reset_objects(&ignored, &ignored);
		init_sound();
		init_sprites(&ignored, &r10);
		clear_sprite_buckets();
		mode = 0u;
		set_screen_mode(mode);
		clear_text_layer(&r0, &ignored, &r11);
		clear_bitmap_layer((uint8_t)r0, &ignored, &r4, &r5, &row_pairs_left, &blocks_per_row, &r10);
		mem_write16(video_layer_disable, 0xFu);
		scroll_x = 0u;
		scroll_y = 0u;
		is_8bpp = 1u;
		set_bitmap_scroll(scroll_x, scroll_y, is_8bpp);
		mem_write32(video_control, mem_read32(video_control) | 0x800u);
		fill_value = (fill_value & ~0xFFFFu) | 0x909u;
		set_halfwords(VIDEO_RAM, 0xE000u, fill_value);
		color_offset_red = 0u;
		color_offset_blue = 0u;
		color_offset_green = 0u;
		color_offset_2_red = 0u;
		color_offset_2_blue = 0u;
		color_offset_2_green = 0u;
		text = memory_test_text_ADDRESS;
		print_text_list(&text, &r11);
		text = memory_test_done_text_ADDRESS;
		print_text_list(&text, &r11);
		text = memory_test_dashes_text_ADDRESS;
		print_text_list(&text, &r11);
		if (memory_test_errors != 0u)
		{
			text = memory_test_error_text_ADDRESS;
			print_text_list(&text, &r11);
		}
		results_left = 0x13u;
		result_index = 0u;
		do
		{
			offset_entry = memory_test_good_offsets_ADDRESS + result_index * 2u;
			if ((memory_test_errors & shl32(1u, (int)(result_index))) != 0u)
				offset_entry = memory_test_bad_offsets_ADDRESS + result_index * 2u;
			blocks_per_row = memory_test_result_texts_ADDRESS;
			text = ROM16(offset_entry) + memory_test_result_texts_ADDRESS;
			print_text_list(&text, &r11);
			result_index = (uint8_t)(result_index + 1u);
			results_left--;
		} while (results_left != 0u);
	}
	if ((0x2002u & inputs_pressed) == 0u)
	{
		*p_results_left = results_left;
		*p_blocks_per_row = blocks_per_row;
		return;
	}
	if (test_continue_mode != 0u)
	{
		test_screen++;
		test_screen_ready = 0u;
		*p_results_left = results_left;
		*p_blocks_per_row = blocks_per_row;
		return;
	}
	test_cursor = test_menu_item;
	test_screen = 0u;
	test_screen_ready = 1u;
	draw_test_menu();
	*p_results_left = results_left;
	*p_blocks_per_row = blocks_per_row;
}

void check_rom_checksums(void)
{
	uint32_t count = 0, crc = 0, data = 0, ignored = 0, stored_crc = 0;
	uint16_t ic8_crc = 0, ic18_crc = 0;

	data = 0u;
	count = 0x20000u;
	compute_crc16(count, &crc, &data, &ignored);
	stored_crc = rom_checksum_ic8_ADDRESS;
	if ((uint16_t)crc != rom_checksums)
		memory_test_errors |= 1u;
	data = 0x80000u;
	count = 0x1F000u;
	compute_crc16(count, &crc, &data, &ignored);
	ic8_crc = ROM16(stored_crc);
	stored_crc += 2u;
	if ((uint16_t)crc != ic8_crc)
		memory_test_errors |= 4u;
	data = 0x100000u;
	count = 0x40000u;
	crc = compute_crc16_alternate_bytes(data, count);
	ic18_crc = ROM16(stored_crc);
	stored_crc += 2u;
	if ((uint16_t)crc != ic18_crc)
		memory_test_errors |= 2u;
	data = 0x100001u;
	count = 0x3FFFFu;
	crc = compute_crc16_alternate_bytes(data, count);
	if ((uint16_t)crc != ROM16(stored_crc))
		memory_test_errors |= 8u;
}

void test_work_ram(void)
{
	uint32_t bytes_left = 0, passes_left = 0, pattern = 0, pattern_index = 0, ram_cursor = 0;
	uint8_t expected = 0, actual = 0;

	move_bytes(VIDEO_RAM, WORK_RAM, 0x10000u);
	ram_cursor = 0x200000u;
	passes_left = 0x42u;
	do
	{
		pattern_index = 4u;
		do
		{
			pattern = ram_test_patterns[pattern_index];
			bytes_left = 0xF9u;
			do
			{
				mem_write8(ram_cursor, ROM8(pattern));
				pattern++;
				ram_cursor++;
				if ((int32_t)ram_cursor >= 0x210000)
					goto test_work_ram_verify;
				bytes_left--;
			} while (bytes_left != 0u);
			pattern_index--;
		} while (pattern_index != 0u);
		passes_left--;
	} while (passes_left != 0u);
test_work_ram_verify:
	ram_cursor = 0x200000u;
	passes_left = 0x42u;
	do
	{
		pattern_index = 4u;
		do
		{
			pattern = ram_test_patterns[pattern_index];
			bytes_left = 0xF9u;
			do
			{
				expected = ROM8(pattern);
				actual = mem_read8(ram_cursor);
				pattern++;
				ram_cursor++;
				if (actual != expected)
				{
					if ((ram_cursor & 1u) != 0u)
						mem_write32(memory_test_video_word, mem_read32(memory_test_video_word) | 0x20u);
					else
						mem_write32(memory_test_video_word, mem_read32(memory_test_video_word) | 0x40u);
				}
				if ((int32_t)ram_cursor >= 0x210000)
				{
					move_bytes(WORK_RAM, VIDEO_RAM, 0x10000u);
					return;
				}
				bytes_left--;
			} while (bytes_left != 0u);
			pattern_index--;
		} while (pattern_index != 0u);
		passes_left--;
	} while (passes_left != 0u);
	move_bytes(WORK_RAM, VIDEO_RAM, 0x10000u);
}

void test_video_ram(void)
{
	uint32_t bytes_left = 0, passes_left = 0, pattern = 0, pattern_index = 0, ram_cursor = 0;
	uint8_t expected = 0, actual = 0;

	ram_cursor = 0x300000u;
	passes_left = 0x84u;
	do
	{
		pattern_index = 4u;
		do
		{
			pattern = ram_test_patterns[pattern_index];
			bytes_left = 0xF9u;
			do
			{
				mem_write8(ram_cursor, ROM8(pattern));
				ram_cursor++;
				pattern++;
				if ((int32_t)ram_cursor >= 0x320000)
					goto test_video_ram_verify;
				bytes_left--;
			} while (bytes_left != 0u);
			pattern_index--;
		} while (pattern_index != 0u);
		passes_left--;
	} while (passes_left != 0u);
test_video_ram_verify:
	ram_cursor = 0x300000u;
	passes_left = 0x84u;
	do
	{
		pattern_index = 4u;
		do
		{
			pattern = ram_test_patterns[pattern_index];
			bytes_left = 0xF9u;
			do
			{
				expected = ROM8(pattern);
				actual = mem_read8(ram_cursor);
				ram_cursor++;
				pattern++;
				if (actual != expected)
				{
					if ((ram_cursor & 1u) != 0u)
					{
						if ((mem_read8(ram_cursor - 1u) & 0xFu) != (mem_read8(pattern - 1u) & 0xFu))
							memory_test_errors |= 0x10000u;
						if ((mem_read8(ram_cursor - 1u) & ~0xFu) != (mem_read8(pattern - 1u) & ~0xFu))
							memory_test_errors |= 0x8000u;
					}
					if ((mem_read8(ram_cursor - 1u) & 0xFu) != (mem_read8(pattern - 1u) & 0xFu))
						memory_test_errors |= 0x4000u;
					if ((mem_read8(ram_cursor - 1u) & ~0xFu) != (mem_read8(pattern - 1u) & ~0xFu))
						memory_test_errors |= 0x2000u;
				}
				if ((int32_t)ram_cursor >= 0x320000)
					return;
				bytes_left--;
			} while (bytes_left != 0u);
			pattern_index--;
		} while (pattern_index != 0u);
		passes_left--;
	} while (passes_left != 0u);
}

void test_sprite_ram(void)
{
	uint32_t bytes_left = 0, passes_left = 0, pattern = 0, pattern_index = 0, ram_cursor = 0;
	uint8_t expected = 0, actual = 0;

	ram_cursor = 0x400000u;
	passes_left = 0x84u;
	do
	{
		pattern_index = 4u;
		do
		{
			pattern = ram_test_patterns[pattern_index];
			bytes_left = 0xF9u;
			do
			{
				mem_write8(ram_cursor, ROM8(pattern));
				ram_cursor++;
				pattern++;
				if ((int32_t)ram_cursor >= 0x420000)
					goto test_sprite_ram_verify;
				bytes_left--;
			} while (bytes_left != 0u);
			pattern_index--;
		} while (pattern_index != 0u);
		passes_left--;
	} while (passes_left != 0u);
test_sprite_ram_verify:
	ram_cursor = 0x400000u;
	passes_left = 0x84u;
	do
	{
		pattern_index = 4u;
		do
		{
			pattern = ram_test_patterns[pattern_index];
			bytes_left = 0xF9u;
			do
			{
				expected = ROM8(pattern);
				actual = mem_read8(ram_cursor);
				ram_cursor++;
				pattern++;
				if (actual != expected)
				{
					if ((ram_cursor & 1u) != 0u)
					{
						if ((mem_read8(ram_cursor - 1u) & 0xFu) != (mem_read8(pattern - 1u) & 0xFu))
							memory_test_errors |= 0x1000u;
						if ((mem_read8(ram_cursor - 1u) & ~0xFu) != (mem_read8(pattern - 1u) & ~0xFu))
							memory_test_errors |= 0x800u;
					}
					if ((mem_read8(ram_cursor - 1u) & 0xFu) != (mem_read8(pattern - 1u) & 0xFu))
						memory_test_errors |= 0x400u;
					if ((mem_read8(ram_cursor - 1u) & ~0xFu) != (mem_read8(pattern - 1u) & ~0xFu))
						memory_test_errors |= 0x200u;
				}
				if ((int32_t)ram_cursor >= 0x420000)
					return;
				bytes_left--;
			} while (bytes_left != 0u);
			pattern_index--;
		} while (pattern_index != 0u);
		passes_left--;
	} while (passes_left != 0u);
}

void test_palette_ram(void)
{
	uint32_t halfwords_left = 0, passes_left = 0, pattern = 0, pattern_index = 0, ram_cursor = 0;
	uint16_t expected_first = 0, actual_first = 0, expected_second = 0, actual_second = 0;

	ram_cursor = 0x600000u;
	passes_left = 8u;
	do
	{
		pattern_index = 4u;
		do
		{
			pattern = ram_test_patterns[pattern_index];
			halfwords_left = 0xF9u;
			do
			{
				mem_write16(ram_cursor, ROM16(pattern));
				ram_cursor += 2u;
				pattern += 2u;
				if ((int32_t)ram_cursor >= 0x604000)
					goto test_palette_ram_verify_first_half;
				halfwords_left--;
			} while (halfwords_left != 0u);
			pattern_index--;
		} while (pattern_index != 0u);
		passes_left--;
	} while (passes_left != 0u);
test_palette_ram_verify_first_half:
	ram_cursor = 0x600000u;
	passes_left = 8u;
	do
	{
		pattern_index = 4u;
		do
		{
			pattern = ram_test_patterns[pattern_index];
			halfwords_left = 0xF9u;
			do
			{
				expected_first = ROM16(pattern);
				actual_first = mem_read16(ram_cursor);
				ram_cursor += 2u;
				pattern += 2u;
				if (actual_first != expected_first)
				{
					if ((mem_read16(ram_cursor - 2u) & 0xFFu) != (mem_read16(pattern - 2u) & 0xFFu))
						memory_test_errors |= 0x100u;
					if ((mem_read16(ram_cursor - 2u) & ~0xFFu) != (mem_read16(pattern - 2u) & ~0xFFu))
						memory_test_errors |= 0x80u;
				}
				if ((int32_t)ram_cursor >= 0x604000)
					goto test_palette_ram_fill_second_half;
				halfwords_left--;
			} while (halfwords_left != 0u);
			pattern_index--;
		} while (pattern_index != 0u);
		passes_left--;
	} while (passes_left != 0u);
test_palette_ram_fill_second_half:
	ram_cursor = PALETTE_RAM + 0x4000u;
	passes_left = 8u;
	do
	{
		pattern_index = 4u;
		do
		{
			pattern = ram_test_patterns[pattern_index];
			halfwords_left = 0xF9u;
			do
			{
				mem_write16(ram_cursor, ROM16(pattern));
				ram_cursor += 2u;
				pattern += 2u;
				if ((int32_t)ram_cursor >= 0x608000)
					goto test_palette_ram_verify_second_half;
				halfwords_left--;
			} while (halfwords_left != 0u);
			pattern_index--;
		} while (pattern_index != 0u);
		passes_left--;
	} while (passes_left != 0u);
test_palette_ram_verify_second_half:
	ram_cursor = PALETTE_RAM + 0x4000u;
	passes_left = 8u;
	do
	{
		pattern_index = 4u;
		do
		{
			pattern = ram_test_patterns[pattern_index];
			halfwords_left = 0xF9u;
			do
			{
				expected_second = ROM16(pattern);
				actual_second = mem_read16(ram_cursor);
				ram_cursor += 2u;
				pattern += 2u;
				if (actual_second != expected_second)
				{
					if ((mem_read16(ram_cursor - 2u) & 0xFFu) != (mem_read16(pattern - 2u) & 0xFFu))
						memory_test_errors |= 0x40000u;
					if ((mem_read16(ram_cursor - 2u) & ~0xFFu) != (mem_read16(pattern - 2u) & ~0xFFu))
						memory_test_errors |= 0x20000u;
				}
				if ((int32_t)ram_cursor >= 0x608000)
					return;
				halfwords_left--;
			} while (halfwords_left != 0u);
			pattern_index--;
		} while (pattern_index != 0u);
		passes_left--;
	} while (passes_left != 0u);
}

void test_sound_ram(void)
{
	uint32_t bytes_left = 0, passes_left = 0, pattern = 0, pattern_index = 0, ram_cursor = 0;
	uint8_t expected = 0, actual = 0;

	move_halfwords(VIDEO_RAM, SOUND_RAM, 0x1000u);
	ram_cursor = 0x700000u;
	passes_left = 9u;
	do
	{
		pattern_index = 4u;
		do
		{
			pattern = ram_test_patterns[pattern_index];
			bytes_left = 0xF9u;
			do
			{
				mem_write8(ram_cursor, ROM8(pattern));
				pattern++;
				ram_cursor++;
				if ((int32_t)ram_cursor >= 0x702000)
					goto test_sound_ram_verify;
				bytes_left--;
			} while (bytes_left != 0u);
			pattern_index--;
		} while (pattern_index != 0u);
		passes_left--;
	} while (passes_left != 0u);
test_sound_ram_verify:
	ram_cursor = 0x700000u;
	passes_left = 9u;
	do
	{
		pattern_index = 4u;
		do
		{
			pattern = ram_test_patterns[pattern_index];
			bytes_left = 0xF9u;
			do
			{
				expected = ROM8(pattern);
				actual = mem_read8(ram_cursor);
				pattern++;
				ram_cursor++;
				if (actual != expected)
					memory_test_errors |= 0x10u;
				if ((int32_t)ram_cursor >= 0x702000)
				{
					move_bytes(SOUND_RAM, VIDEO_RAM, 0x2000u);
					return;
				}
				bytes_left--;
			} while (bytes_left != 0u);
			pattern_index--;
		} while (pattern_index != 0u);
		passes_left--;
	} while (passes_left != 0u);
	move_bytes(SOUND_RAM, VIDEO_RAM, 0x2000u);
}

void test_inputs(uint16_t count, uint32_t *p_panel_color, uint32_t *p_blocks_per_row)
{
	uint32_t panel_color = *p_panel_color;
	uint32_t blocks_per_row = *p_blocks_per_row;
	uint32_t r4 = 0, r11 = 0, bottom = 0, coin_text = 0, ignored = 0, input_bits = 0, input_index = 0, inputs_left = 0, left = 0, off_texts = 0, on_texts = 0, right = 0, sprite_block = 0, text = 0, text_entry = 0, text_position = 0, top = 0, value = 0;

	if (test_screen_ready != 0u)
	{
		if (setting_no_start_buttons != 0u)
		{
			inputs_left = 6u;
			input_bits = input_test_bits_no_start_2p_ADDRESS;
			on_texts = input_test_on_texts_no_start_2p_ADDRESS;
			off_texts = input_test_off_texts_no_start_2p_ADDRESS;
			if (setting_cabinet_type != 0u)
			{
				inputs_left = 7u;
				input_bits = input_test_bits_no_start_3p_ADDRESS;
				on_texts = input_test_on_texts_no_start_3p_ADDRESS;
				off_texts = input_test_off_texts_no_start_3p_ADDRESS;
			}
		}
		else
		{
			inputs_left = 8u;
			input_bits = input_test_bits_2p_ADDRESS;
			on_texts = input_test_on_texts_2p_ADDRESS;
			off_texts = input_test_off_texts_2p_ADDRESS;
			if (setting_cabinet_type != 0u)
			{
				inputs_left = 0xAu;
				input_bits = input_test_bits_3p_ADDRESS;
				on_texts = input_test_on_texts_3p_ADDRESS;
				off_texts = input_test_off_texts_3p_ADDRESS;
			}
		}
		do
		{
			input_index = (inputs_left & ~0xFFu) | (uint8_t)(inputs_left - 1u);
			if ((input_ports_active & shl32(1u, (int)(mem_read8(input_bits + input_index)))) == 0u)
				text_entry = off_texts + input_index * 4u;
			else
				text_entry = on_texts + input_index * 4u;
			text = mem_read32(text_entry);
			print_text_list(&text, &text_position);
			inputs_left--;
		} while (inputs_left != 0u);
		text = input_test_coin_1_on_text_ADDRESS;
		if ((input_ports_active & 4u) == 0u)
			text = input_test_coin_1_off_text_ADDRESS;
		print_text_list(&text, &text_position);
		coin_text = input_test_coin_2_on_text_ADDRESS;
		if ((input_ports_active & 8u) == 0u)
			coin_text = input_test_coin_2_off_text_ADDRESS;
		print_text_list(&coin_text, &text_position);
		if (setting_cabinet_type != 0u && setting_coin_chute_type != 0u)
		{
			coin_text = input_test_coin_3_on_text_ADDRESS;
			if ((input_ports_active & 0x40u) == 0u)
				coin_text = input_test_coin_3_off_text_ADDRESS;
			print_text_list(&coin_text, &text_position);
		}
		move_input_test_cursors(count, coin_text, &r4, &blocks_per_row);
		if ((input_ports_active & 0x2002u) != 0u && input_test_switch_released == 0u)
		{
			*p_panel_color = r4;
			*p_blocks_per_row = blocks_per_row;
			return;
		}
		input_test_switch_released = 1u;
		if ((input_ports_active & 0x2002u) == 0u)
		{
			if (input_test_switch_pressed == 0u)
			{
				*p_panel_color = r4;
				*p_blocks_per_row = blocks_per_row;
				return;
			}
			sprite_block = screen_sprite_block;
			free_object_2((uint16_t)sprite_block);
			init_sprites(&sprite_block, &ignored);
			mem_write32(video_control, mem_read32(video_control) | 0x800u);
			fill_value = (fill_value & ~0xFFFFu) | 0x909u;
			set_halfwords(VIDEO_RAM, 0xE000u, fill_value);
			if (test_continue_mode != 0u)
			{
				test_screen++;
				test_screen_ready = 0u;
				*p_panel_color = r4;
				*p_blocks_per_row = blocks_per_row;
				return;
			}
			test_cursor = test_menu_item;
			test_screen = 0u;
			test_screen_ready = 1u;
			draw_test_menu();
			*p_panel_color = r4;
			*p_blocks_per_row = blocks_per_row;
			return;
		}
		input_test_switch_pressed = 1u;
		*p_panel_color = r4;
		*p_blocks_per_row = blocks_per_row;
		return;
	}
	test_screen_ready = 1u;
	input_test_switch_pressed = 0u;
	input_test_switch_released = 0u;
	clear_text_layer(&value, &ignored, &r11);
	mem_write16(mixer_bitmap, 0x21u);
	text = input_test_text_ADDRESS;
	print_text_list(&text, &r11);
	if (setting_cabinet_type != 0u)
	{
		text = input_test_text_3p_ADDRESS;
		print_text_list(&text, &r11);
	}
	text = input_test_chute_2_text_ADDRESS;
	print_text_list(&text, &r11);
	if (setting_coin_chute_type != 0u && setting_cabinet_type != 0u)
	{
		text = input_test_chute_3_text_ADDRESS;
		print_text_list(&text, &r11);
	}
	if (setting_no_start_buttons != 0u)
	{
		text = input_test_start_erase_text_ADDRESS;
		print_text_list(&text, &r11);
	}
	left = (value & ~0xFFFFu) | 0xA0u;
	top = 0x30u;
	right = 0x138u;
	bottom = 0x60u;
	r4 = (panel_color & ~0xFFFFu) | 2u;
	if ((mem_read32(video_control) & 0x200u) != 0u)
		flip_test_coordinates(&left, &top, &right, &bottom);
	fill_rect(top, right, bottom, &left, &r4, &blocks_per_row);
	left = (left & ~0xFFFFu) | 0xA0u;
	top = 0x68u;
	right = 0x138u;
	bottom = 0x98u;
	panel_color = (r4 & ~0xFFFFu) | 0xEu;
	if ((mem_read32(video_control) & 0x200u) != 0u)
		flip_test_coordinates(&left, &top, &right, &bottom);
	fill_rect(top, right, bottom, &left, &panel_color, &blocks_per_row);
	if (setting_cabinet_type != 0u)
	{
		left = (left & ~0xFFFFu) | 0xA0u;
		top = 0xA0u;
		right = 0x138u;
		bottom = 0xD0u;
		panel_color = (panel_color & ~0xFFFFu) | 6u;
		if ((mem_read32(video_control) & 0x200u) != 0u)
			flip_test_coordinates(&left, &top, &right, &bottom);
		fill_rect(top, right, bottom, &left, &panel_color, &blocks_per_row);
	}
	allocate_object_2(&ignored, &ignored, &r11);
	screen_sprite_block = r11;
	current_object = r11;
	OBJECT(current_object)->screen_x = 0xEC;
	AT16(OBJECT(current_object)->handler, 2) = 0xECu;
	OBJECT(current_object)->projected_y_int = 0xEC;
	OBJECT(current_object)->screen_y = 0x48;
	AS16(OBJECT(current_object)->handler) = 0x80u;
	OBJECT(current_object)->projected_y_fraction = 0xB8u;
	OBJECT(current_object)->sprite_layer = 0u;
	OBJECT(current_object)->sprite_width = 0;
	OBJECT(current_object)->projected_x = 0;
	OBJECT(current_object)->sprite_scale = 0x80u;
	OBJECT(current_object)->sprite_height = 0x80;
	OBJECT(current_object)->child = 0x80u;
	OBJECT(current_object)->sprite_frame = 0x1120u;
	OBJECT(current_object)->anchor_x = 0x1120;
	OBJECT(current_object)->direction = 0x1120u;
	OBJECT(current_object)->sprite_palette = 0x130u;
	OBJECT(current_object)->anchor_y = 0x140;
	OBJECT(current_object)->parent = 0x150u;
	OBJECT(current_object)->sprite_flags = 0u;
	OBJECT(current_object)->hitbox_size_y = 0u;
	OBJECT(current_object)->next = 0u;
	trackball_1_quadrants = 0u;
	trackball_2_quadrants = 0u;
	trackball_3_quadrants = 0u;
	*p_panel_color = panel_color;
	*p_blocks_per_row = blocks_per_row;
}

void move_input_test_cursors(uint32_t count, uint32_t text, uint32_t *p_cursors_left, uint32_t *p_height)
{
	uint32_t height = *p_height;
	uint16_t trackball_dx = 0, trackball_dy = 0;
	uint32_t bottom_limit = 0, cursors_left = 0, ignored = 0, quadrant_bit = 0, sprite_entry = 0, text_position = 0, top_limit = 0;

	current_object = (current_object & ~0xFFFFu) | screen_sprite_block;
	queue_sprite_in_layer();
	current_object = current_object + 0x10u;
	queue_sprite_in_layer();
	if (trackball_1_quadrants == 0xFu)
	{
		text = input_test_trackball_1_good_text_ADDRESS;
		print_text_list(&text, &text_position);
	}
	if (trackball_2_quadrants == 0xFu)
	{
		text = input_test_trackball_2_good_text_ADDRESS;
		print_text_list(&text, &text_position);
	}
	if (setting_cabinet_type != 0u)
	{
		current_object = current_object + 0x10u;
		queue_sprite_in_layer();
		if (trackball_3_quadrants == 0xFu)
		{
			text = input_test_trackball_3_good_text_ADDRESS;
			print_text_list(&text, &text_position);
		}
	}
	build_sprite_list(&cursors_left, &count, &height, &text, &sprite_entry);
	draw_queued_strings((uint16_t)count, (uint16_t)text, sprite_entry, &cursors_left, &ignored, &height);
	current_object = (current_object & ~0xFFFFu) | screen_sprite_block;
	trackball_dx = (uint16_t)trackball_1_x;
	trackball_dy = (uint16_t)trackball_1_y;
	quadrant_bit = get_trackball_quadrant((uint8_t)trackball_dx, (uint8_t)trackball_dy, 0u);
	trackball_1_quadrants |= quadrant_bit;
	OBJECT(current_object)->screen_x += (int16_t)trackball_dx >> 2;
	OBJECT(current_object)->screen_y += (int16_t)trackball_dy >> 2;
	trackball_dx = (uint16_t)trackball_2_x;
	trackball_dy = (uint16_t)trackball_2_y;
	quadrant_bit = get_trackball_quadrant((uint8_t)trackball_dx, (uint8_t)trackball_dy, quadrant_bit);
	trackball_2_quadrants |= quadrant_bit;
	AT16(OBJECT(current_object)->handler, 2) += (int16_t)trackball_dx >> 2;
	AS16(OBJECT(current_object)->handler) += (int16_t)trackball_dy >> 2;
	trackball_dx = (uint16_t)trackball_3_x;
	trackball_dy = (uint16_t)trackball_3_y;
	quadrant_bit = get_trackball_quadrant((uint8_t)trackball_dx, (uint8_t)trackball_dy, quadrant_bit);
	trackball_3_quadrants |= quadrant_bit;
	OBJECT(current_object)->projected_y_int += (int16_t)trackball_dx >> 2;
	OBJECT(current_object)->projected_y_fraction += (int16_t)trackball_dy >> 2;
	current_object = (current_object & ~0xFFFFu) | screen_sprite_block;
	cursors_left = 3u;
	do
	{
		if ((int16_t)mem_read16(current_object + 0xEu) <= 0xA5)
			mem_write16(current_object + 0xEu, 0xA5u);
		current_object = current_object + 0x10u;
		cursors_left--;
	} while (cursors_left != 0u);
	current_object = (current_object & ~0xFFFFu) | screen_sprite_block;
	cursors_left = 3u;
	do
	{
		if ((int16_t)mem_read16(current_object + 0xEu) >= 0x132)
			mem_write16(current_object + 0xEu, 0x132u);
		current_object = current_object + 0x10u;
		cursors_left--;
	} while (cursors_left != 0u);
	current_object = (current_object & ~0xFFFFu) | screen_sprite_block;
	cursors_left = 3u;
	do
	{
		bottom_limit = input_test_cursor_bottom_limits_ADDRESS + cursors_left * 2u;
		if ((int16_t)mem_read16(bottom_limit) <= (int16_t)mem_read16(current_object + 0xCu))
			mem_write16(current_object + 0xCu, mem_read16(bottom_limit));
		current_object = current_object + 0x10u;
		cursors_left--;
	} while (cursors_left != 0u);
	current_object = (current_object & ~0xFFFFu) | screen_sprite_block;
	cursors_left = 3u;
	do
	{
		top_limit = input_test_cursor_top_limits_ADDRESS + cursors_left * 2u;
		if ((int16_t)mem_read16(current_object + 0xCu) <= (int16_t)mem_read16(top_limit))
			mem_write16(current_object + 0xCu, mem_read16(top_limit));
		current_object = current_object + 0x10u;
		cursors_left--;
	} while (cursors_left != 0u);
	*p_cursors_left = cursors_left;
	*p_height = height;
}

uint32_t get_trackball_quadrant(uint8_t dx, uint8_t dy, uint32_t quadrant)
{
	if ((int8_t)dx < 0)
	{
		if ((int8_t)dy < 0)
			quadrant = (quadrant & ~0xFFu) | 8u;
		else
			quadrant = (quadrant & ~0xFFu) | 4u;
	}
	else if ((int8_t)dy < 0)
		quadrant = (quadrant & ~0xFFu) | 2u;
	else
		quadrant = (quadrant & ~0xFFu) | 1u;
	return quadrant;
}

void test_sound(void)
{
	uint8_t sound_id = 0;
	uint32_t r11 = 0, attribute = 0, ignored = 0, text = 0, text_position = 0, value = 0;

	if (test_screen_ready != 0u)
	{
		if ((0x2002u & inputs_pressed) != 0u)
		{
			sound_id = 0x94u;
			queue_sound(sound_id, &ignored, &ignored);
			if (test_continue_mode != 0u)
			{
				test_screen++;
				test_screen_ready = 0u;
				return;
			}
			test_cursor = test_menu_item;
			test_screen = 0u;
			test_screen_ready = 1u;
			draw_test_menu();
			return;
		}
		if ((0x1001u & inputs_pressed) == 0u)
			return;
		sound_test_number++;
		sound_id = 0x94u;
		queue_sound(sound_id, &ignored, &ignored);
		sound_id = 0x95u;
		queue_sound(sound_id, &ignored, &ignored);
		wrap_sound_test_number();
		sound_id = sound_test_number;
		if (sound_test_number == 0u)
		{
			sound_id = 0x94u;
			queue_sound(sound_id, &ignored, &ignored);
			sound_id = 0x95u;
		}
		queue_sound(sound_id, &ignored, &ignored);
		value = sound_test_number;
		convert_to_bcd(&value, &ignored);
		text_position = TEXT_CELL(20, 12);
		attribute = 0x200u;
		draw_hex_4(&value, &attribute, &text_position);
		return;
	}
	clear_text_layer(&ignored, &ignored, &r11);
	sound_test_number = 0u;
	text = sound_test_text_ADDRESS;
	print_text_list(&text, &r11);
	test_screen_ready++;
	sound_id = 0x94u;
	queue_sound(sound_id, &ignored, &ignored);
	value = sound_test_number;
	convert_to_bcd(&value, &ignored);
	text_position = TEXT_CELL(20, 12);
	attribute = 0x200u;
	draw_hex_4(&value, &attribute, &text_position);
}

void wrap_sound_test_number(void)
{
	if ((int16_t)(sound_test_number - 0x8Cu) > 0)
		sound_test_number = 0u;
}

void wrap_sound_test_number_down(void)
{
	if ((int16_t)sound_test_number > 0x8C)
		sound_test_number = 0x8Cu;
}
