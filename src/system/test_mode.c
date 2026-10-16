/**
 *    Name:        system/test_mode.c
 *    Description: the test mode: the menu, the memory, input and sound tests, the CRT test, the game and coin assignments, the bookkeeping screen, the backup clear, the output test, the exit
 *    Version:     0.1.0
 *    Created:     12-10-2026
 *    Modified:    16-10-2026
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
	uint32_t r0 = 0, r8 = 0, ignored = 0, scroll_y = 0;
	uint8_t cnt_bits = 0, port_d_bits = 0, seat_port_bits = 0;

	cnt_bits = mem_read8(io_cnt);
	mem_write8(io_cnt, cnt_bits & ~2u);
	show_eeprom_write_count = 0u;
	if ((0x1001u & input_ports_active) != 0u)
		show_eeprom_write_count = 1u;
	display_off_frames = 0x14u;
	load_default_palettes();
	init_video_registers();
	init_sprites(&ignored, &ignored);
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
	clear_bitmap_layer((uint8_t)r0, &scroll_y, &color, &ignored, &r8, &blocks_per_row, &ignored);
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
		invalid_jump(0x9CA0Eu + test_screen * 3u);
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
		if ((int8_t)TEXT_CELL_POSITION(cursor_entry)->column >= 0)
			break;
		test_cursor = 0u;
	}
	test_cursor_column = TEXT_CELL_POSITION(cursor_entry)->column;
	test_cursor_row = TEXT_CELL_POSITION(cursor_entry)->row;
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
	clear_text_layer(&ignored, &ignored, &ignored);
	if (show_eeprom_write_count != 0u)
	{
		write_count_bcd = (uint16_t)eeprom_write_count;
		convert_to_bcd(&write_count_bcd, &ignored);
		write_count_bcd &= 0xFFFFFu;
		print_position = TEXT_CELL(31, 26);
		attribute = 0x1C00u;
		draw_hex_8(write_count_bcd, attribute, print_position);
	}
	text = test_menu_text_ADDRESS;
	print_text_list(&text, &ignored);
	if (mem_read8(moving_seat_dip) != 0u)
	{
		text = test_menu_output_test_text_ADDRESS;
		print_text_list(&text, &ignored);
	}
}

void toggle_test_continue_mode(void)
{
	uint32_t ignored = 0, text = 0;

	text = individual_mode_text_ADDRESS;
	test_continue_mode = ~test_continue_mode;
	if (test_continue_mode != 0u)
		text = continue_mode_text_ADDRESS;
	print_text_list(&text, &ignored);
	test_screen = 0u;
	test_screen_ready = 1u;
}

void test_memory(uint32_t *p_results_left, uint32_t *p_blocks_per_row)
{
	uint32_t results_left = *p_results_left;
	uint32_t blocks_per_row = *p_blocks_per_row;
	uint8_t is_8bpp = 0, mode = 0;
	uint16_t scroll_x = 0, scroll_y = 0;
	uint32_t r0 = 0, r4 = 0, delay_count = 0, ignored = 0, offset_entry = 0, print_position = 0, result_index = 0, text = 0;
	uint8_t cnt_bits = 0;

	if (test_screen_ready == 0u)
	{
		interrupts_off();
		test_screen_ready++;
		clear_text_layer(&ignored, &ignored, &print_position);
		text = memory_test_text_ADDRESS;
		print_text_list(&text, &ignored);
		text = memory_test_dashes_text_ADDRESS;
		print_text_list(&text, &ignored);
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
		load_text_font(&r4, &ignored);
		init_video_registers();
		reset_objects(&ignored, &ignored);
		init_sound();
		init_sprites(&ignored, &ignored);
		clear_sprite_buckets();
		mode = 0u;
		set_screen_mode(mode);
		clear_text_layer(&r0, &ignored, &print_position);
		clear_bitmap_layer((uint8_t)r0, &ignored, &r4, &ignored, &ignored, &ignored, &ignored);
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
		print_text_list(&text, &ignored);
		text = memory_test_done_text_ADDRESS;
		print_text_list(&text, &ignored);
		text = memory_test_dashes_text_ADDRESS;
		print_text_list(&text, &ignored);
		if (memory_test_errors != 0u)
		{
			text = memory_test_error_text_ADDRESS;
			print_text_list(&text, &ignored);
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
			print_text_list(&text, &ignored);
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
				{
					test_work_ram_verify();
					return;
				}
				bytes_left--;
			} while (bytes_left != 0u);
			pattern_index--;
		} while (pattern_index != 0u);
		passes_left--;
	} while (passes_left != 0u);
	test_work_ram_verify();
}

void test_work_ram_verify(void)
{
	uint32_t bytes_left = 0, passes_left = 0, pattern = 0, pattern_index = 0, ram_cursor = 0;
	uint8_t expected = 0, actual = 0;

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
				{
					test_video_ram_verify();
					return;
				}
				bytes_left--;
			} while (bytes_left != 0u);
			pattern_index--;
		} while (pattern_index != 0u);
		passes_left--;
	} while (passes_left != 0u);
	test_video_ram_verify();
}

void test_video_ram_verify(void)
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
				expected = ROM8(pattern);
				actual = mem_read8(ram_cursor);
				ram_cursor++;
				pattern++;
				if (actual != expected)
				{
					if ((ram_cursor & 1u) != 0u)
					{
						if ((mem_read8(ram_cursor - 1u) & 0xFu) != (ROM8(pattern - 1u) & 0xFu))
							memory_test_errors |= 0x10000u;
						if ((mem_read8(ram_cursor - 1u) & ~0xFu) != (ROM8(pattern - 1u) & ~0xFu))
							memory_test_errors |= 0x8000u;
					}
					if ((mem_read8(ram_cursor - 1u) & 0xFu) != (ROM8(pattern - 1u) & 0xFu))
						memory_test_errors |= 0x4000u;
					if ((mem_read8(ram_cursor - 1u) & ~0xFu) != (ROM8(pattern - 1u) & ~0xFu))
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
				{
					test_sprite_ram_verify();
					return;
				}
				bytes_left--;
			} while (bytes_left != 0u);
			pattern_index--;
		} while (pattern_index != 0u);
		passes_left--;
	} while (passes_left != 0u);
	test_sprite_ram_verify();
}

void test_sprite_ram_verify(void)
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
				expected = ROM8(pattern);
				actual = mem_read8(ram_cursor);
				ram_cursor++;
				pattern++;
				if (actual != expected)
				{
					if ((ram_cursor & 1u) != 0u)
					{
						if ((mem_read8(ram_cursor - 1u) & 0xFu) != (ROM8(pattern - 1u) & 0xFu))
							memory_test_errors |= 0x1000u;
						if ((mem_read8(ram_cursor - 1u) & ~0xFu) != (ROM8(pattern - 1u) & ~0xFu))
							memory_test_errors |= 0x800u;
					}
					if ((mem_read8(ram_cursor - 1u) & 0xFu) != (ROM8(pattern - 1u) & 0xFu))
						memory_test_errors |= 0x400u;
					if ((mem_read8(ram_cursor - 1u) & ~0xFu) != (ROM8(pattern - 1u) & ~0xFu))
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
				{
					test_palette_ram_verify_first_half();
					return;
				}
				halfwords_left--;
			} while (halfwords_left != 0u);
			pattern_index--;
		} while (pattern_index != 0u);
		passes_left--;
	} while (passes_left != 0u);
	test_palette_ram_verify_first_half();
}

void test_palette_ram_verify_first_half(void)
{
	uint32_t halfwords_left = 0, passes_left = 0, pattern = 0, pattern_index = 0, ram_cursor = 0;
	uint16_t expected_first = 0, actual_first = 0;

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
					if ((mem_read16(ram_cursor - 2u) & 0xFFu) != (ROM16(pattern - 2u) & 0xFFu))
						memory_test_errors |= 0x100u;
					if ((mem_read16(ram_cursor - 2u) & ~0xFFu) != (ROM16(pattern - 2u) & ~0xFFu))
						memory_test_errors |= 0x80u;
				}
				if ((int32_t)ram_cursor >= 0x604000)
				{
					test_palette_ram_fill_second_half();
					return;
				}
				halfwords_left--;
			} while (halfwords_left != 0u);
			pattern_index--;
		} while (pattern_index != 0u);
		passes_left--;
	} while (passes_left != 0u);
	test_palette_ram_fill_second_half();
}

void test_palette_ram_fill_second_half(void)
{
	uint32_t halfwords_left = 0, passes_left = 0, pattern = 0, pattern_index = 0, ram_cursor = 0;

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
				{
					test_palette_ram_verify_second_half();
					return;
				}
				halfwords_left--;
			} while (halfwords_left != 0u);
			pattern_index--;
		} while (pattern_index != 0u);
		passes_left--;
	} while (passes_left != 0u);
	test_palette_ram_verify_second_half();
}

void test_palette_ram_verify_second_half(void)
{
	uint32_t halfwords_left = 0, passes_left = 0, pattern = 0, pattern_index = 0, ram_cursor = 0;
	uint16_t expected_second = 0, actual_second = 0;

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
					if ((mem_read16(ram_cursor - 2u) & 0xFFu) != (ROM16(pattern - 2u) & 0xFFu))
						memory_test_errors |= 0x40000u;
					if ((mem_read16(ram_cursor - 2u) & ~0xFFu) != (ROM16(pattern - 2u) & ~0xFFu))
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
				{
					test_sound_ram_verify();
					return;
				}
				bytes_left--;
			} while (bytes_left != 0u);
			pattern_index--;
		} while (pattern_index != 0u);
		passes_left--;
	} while (passes_left != 0u);
	test_sound_ram_verify();
}

void test_sound_ram_verify(void)
{
	uint32_t bytes_left = 0, passes_left = 0, pattern = 0, pattern_index = 0, ram_cursor = 0;
	uint8_t expected = 0, actual = 0;

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
	uint16_t sprite_block = 0;
	uint32_t bottom = 0, coin_text = 0, cursors_left = 0, first_panel_color = 0, ignored = 0, input_bits = 0, input_index = 0, inputs_left = 0, left = 0, object = 0, off_texts = 0, on_texts = 0, right = 0, text = 0, text_entry = 0, top = 0, value = 0;

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
			text = ROM32(text_entry);
			print_text_list(&text, &ignored);
			inputs_left--;
		} while (inputs_left != 0u);
		text = input_test_coin_1_on_text_ADDRESS;
		if ((input_ports_active & 4u) == 0u)
			text = input_test_coin_1_off_text_ADDRESS;
		print_text_list(&text, &ignored);
		coin_text = input_test_coin_2_on_text_ADDRESS;
		if ((input_ports_active & 8u) == 0u)
			coin_text = input_test_coin_2_off_text_ADDRESS;
		print_text_list(&coin_text, &ignored);
		if (setting_cabinet_type != 0u && setting_coin_chute_type != 0u)
		{
			coin_text = input_test_coin_3_on_text_ADDRESS;
			if ((input_ports_active & 0x40u) == 0u)
				coin_text = input_test_coin_3_off_text_ADDRESS;
			print_text_list(&coin_text, &ignored);
		}
		move_input_test_cursors(count, coin_text, &cursors_left, &blocks_per_row);
		if ((input_ports_active & 0x2002u) != 0u && input_test_switch_released == 0u)
		{
			*p_panel_color = cursors_left;
			*p_blocks_per_row = blocks_per_row;
			return;
		}
		input_test_switch_released = 1u;
		if ((input_ports_active & 0x2002u) != 0u)
		{
			input_test_switch_pressed = 1u;
			*p_panel_color = cursors_left;
			*p_blocks_per_row = blocks_per_row;
			return;
		}
		if (input_test_switch_pressed == 0u)
		{
			*p_panel_color = cursors_left;
			*p_blocks_per_row = blocks_per_row;
			return;
		}
		sprite_block = screen_sprite_block;
		free_object_2(sprite_block);
		init_sprites(&ignored, &ignored);
		mem_write32(video_control, mem_read32(video_control) | 0x800u);
		fill_value = (fill_value & ~0xFFFFu) | 0x909u;
		set_halfwords(VIDEO_RAM, 0xE000u, fill_value);
		if (test_continue_mode != 0u)
		{
			test_screen++;
			test_screen_ready = 0u;
			*p_panel_color = cursors_left;
			*p_blocks_per_row = blocks_per_row;
			return;
		}
		test_cursor = test_menu_item;
		test_screen = 0u;
		test_screen_ready = 1u;
		draw_test_menu();
		*p_panel_color = cursors_left;
		*p_blocks_per_row = blocks_per_row;
		return;
	}
	test_screen_ready = 1u;
	input_test_switch_pressed = 0u;
	input_test_switch_released = 0u;
	clear_text_layer(&value, &ignored, &object);
	mem_write16(mixer_bitmap, 0x21u);
	text = input_test_text_ADDRESS;
	print_text_list(&text, &object);
	if (setting_cabinet_type != 0u)
	{
		text = input_test_text_3p_ADDRESS;
		print_text_list(&text, &object);
	}
	text = input_test_chute_2_text_ADDRESS;
	print_text_list(&text, &object);
	if (setting_coin_chute_type != 0u && setting_cabinet_type != 0u)
	{
		text = input_test_chute_3_text_ADDRESS;
		print_text_list(&text, &object);
	}
	if (setting_no_start_buttons != 0u)
	{
		text = input_test_start_erase_text_ADDRESS;
		print_text_list(&text, &object);
	}
	left = (value & ~0xFFFFu) | 0xA0u;
	top = 0x30u;
	right = 0x138u;
	bottom = 0x60u;
	first_panel_color = (panel_color & ~0xFFFFu) | 2u;
	if ((mem_read32(video_control) & 0x200u) != 0u)
		flip_test_coordinates(&left, &top, &right, &bottom);
	fill_rect(top, right, bottom, &left, &first_panel_color, &blocks_per_row);
	left = (left & ~0xFFFFu) | 0xA0u;
	top = 0x68u;
	right = 0x138u;
	bottom = 0x98u;
	panel_color = (first_panel_color & ~0xFFFFu) | 0xEu;
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
	allocate_object_2(&ignored, &ignored, &object);
	screen_sprite_block = object;
	current_object = object;
	OBJECT(current_object)->screen_x = 0xEC;
	OBJECT(current_object)->handler_high = 0xECu;
	OBJECT(current_object)->projected_y_int = 0xEC;
	OBJECT(current_object)->screen_y = 0x48;
	OBJECT(current_object)->handler_low = 0x80u;
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
	uint32_t bottom_limit = 0, cursors_left = 0, ignored = 0, quadrant_bit = 0, sprite_entry = 0, top_limit = 0;

	current_object = (current_object & ~0xFFFFu) | screen_sprite_block;
	queue_sprite_in_layer();
	current_object = current_object + 0x10u;
	queue_sprite_in_layer();
	if (trackball_1_quadrants == 0xFu)
	{
		text = input_test_trackball_1_good_text_ADDRESS;
		print_text_list(&text, &ignored);
	}
	if (trackball_2_quadrants == 0xFu)
	{
		text = input_test_trackball_2_good_text_ADDRESS;
		print_text_list(&text, &ignored);
	}
	if (setting_cabinet_type != 0u)
	{
		current_object = current_object + 0x10u;
		queue_sprite_in_layer();
		if (trackball_3_quadrants == 0xFu)
		{
			text = input_test_trackball_3_good_text_ADDRESS;
			print_text_list(&text, &ignored);
		}
	}
	build_sprite_list(&ignored, &count, &height, &text, &sprite_entry);
	draw_queued_strings((uint16_t)count, (uint16_t)text, sprite_entry, &ignored, &ignored, &height);
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
	OBJECT(current_object)->handler_high += (int16_t)trackball_dx >> 2;
	OBJECT(current_object)->handler_low += (int16_t)trackball_dy >> 2;
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
		if (OBJECT(current_object)->screen_x <= 0xA5)
			OBJECT(current_object)->screen_x = 0xA5;
		current_object = current_object + 0x10u;
		cursors_left--;
	} while (cursors_left != 0u);
	current_object = (current_object & ~0xFFFFu) | screen_sprite_block;
	cursors_left = 3u;
	do
	{
		if (OBJECT(current_object)->screen_x >= 0x132)
			OBJECT(current_object)->screen_x = 0x132;
		current_object = current_object + 0x10u;
		cursors_left--;
	} while (cursors_left != 0u);
	current_object = (current_object & ~0xFFFFu) | screen_sprite_block;
	cursors_left = 3u;
	do
	{
		bottom_limit = input_test_cursor_bottom_limits_ADDRESS + cursors_left * 2u;
		if ((int16_t)ROM16(bottom_limit) <= OBJECT(current_object)->screen_y)
			OBJECT(current_object)->screen_y = ROM16(bottom_limit);
		current_object = current_object + 0x10u;
		cursors_left--;
	} while (cursors_left != 0u);
	current_object = (current_object & ~0xFFFFu) | screen_sprite_block;
	cursors_left = 3u;
	do
	{
		top_limit = input_test_cursor_top_limits_ADDRESS + cursors_left * 2u;
		if (OBJECT(current_object)->screen_y <= (int16_t)ROM16(top_limit))
			OBJECT(current_object)->screen_y = ROM16(top_limit);
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
	uint32_t attribute = 0, ignored = 0, print_position = 0, text = 0, text_position = 0, value = 0;

	if (test_screen_ready == 0u)
	{
		clear_text_layer(&ignored, &ignored, &print_position);
		sound_test_number = 0u;
		text = sound_test_text_ADDRESS;
		print_text_list(&text, &ignored);
		test_screen_ready++;
		sound_id = 0x94u;
		queue_sound(sound_id, &ignored, &ignored);
		value = sound_test_number;
		convert_to_bcd(&value, &ignored);
		text_position = TEXT_CELL(20, 12);
		attribute = 0x200u;
		draw_hex_4(&value, &attribute, &text_position);
		return;
	}
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

void test_crt(uint32_t start_y, uint32_t *p_color, uint32_t *p_blocks_per_row)
{
	uint32_t color = *p_color;
	uint32_t blocks_per_row = *p_blocks_per_row;
	uint16_t end_x = 0, end_y = 0;
	uint32_t bar_record = 0, bars_left = 0, bottom = 0, ignored = 0, line_record = 0, lines_left = 0, print_position = 0, right = 0, rows_left = 0, start_x = 0, text = 0, top = 0;

	if (test_screen_ready == 0u)
	{
		test_screen_ready = 1u;
		clear_text_layer(&ignored, &start_y, &print_position);
		text = crt_test_1_text_ADDRESS;
		print_text_list(&text, &ignored);
	}
	start_x = test_screen_ready * 3u;
	switch (test_screen_ready)
	{
	case 0:
	case 5:
		return;
	case 1:
		bar_record = crt_test_color_bars_ADDRESS;
		rows_left = 4u;
		do
		{
			crt_test_x = ROM16(bar_record);
			bar_record += 2u;
			crt_test_y = ROM16(bar_record);
			bar_record += 2u;
			crt_test_color = ROM8(bar_record);
			bars_left = 0x20u;
			bar_record++;
			do
			{
				start_x = (start_x & ~0xFFFFu) | crt_test_x;
				top = crt_test_y;
				color = (color & ~0xFFu) | crt_test_color;
				right = (uint16_t)(start_x + 7u);
				bottom = (uint16_t)(top + 0x1Fu);
				if ((mem_read32(video_control) & 0x200u) != 0u)
					flip_test_coordinates(&start_x, &top, &right, &bottom);
				fill_rect(top, right, bottom, &start_x, &color, &blocks_per_row);
				crt_test_x += 8u;
				crt_test_color++;
				bars_left--;
			} while (bars_left != 0u);
			rows_left--;
		} while (rows_left != 0u);
		test_screen_ready++;
		*p_color = color;
		*p_blocks_per_row = blocks_per_row;
		return;
	case 2:
		if ((0x2002u & inputs_pressed) == 0u)
			return;
		clear_text_layer(&ignored, &ignored, &ignored);
		test_screen_ready++;
		return;
	case 3:
		mem_write32(video_control, mem_read32(video_control) | 0x800u);
		fill_value = fill_value & ~0xFFFFu;
		set_halfwords(VIDEO_RAM, 0xE000u, fill_value);
		text = crt_test_2_text_ADDRESS;
		print_text_list(&text, &ignored);
		line_record = crt_test_border_lines_ADDRESS;
		lines_left = 4u;
		do
		{
			start_x = (start_x & ~0xFFFFu) | ROM16(line_record);
			line_record += 2u;
			start_y = (start_y & ~0xFFFFu) | ROM16(line_record);
			line_record += 2u;
			end_x = ROM16(line_record);
			line_record += 2u;
			end_y = ROM16(line_record);
			color = (color & ~0xFFu) | 0x7Eu;
			line_record += 2u;
			draw_line(end_x, end_y, &start_x, &start_y, &color, &lines_left);
			lines_left--;
		} while (lines_left != 0u);
		crt_test_x = 0u;
		crt_test_y = 0u;
		lines_left = 0x1Cu;
		do
		{
			start_x &= ~0xFFFFu;
			start_y = (start_y & ~0xFFFFu) | crt_test_x;
			end_x = 0x13Fu;
			end_y = crt_test_x;
			color = (color & ~0xFFFFu) | 0x7Eu;
			draw_line(end_x, end_y, &start_x, &start_y, &color, &lines_left);
			crt_test_x += 8u;
			lines_left--;
		} while (lines_left != 0u);
		lines_left = 0x28u;
		do
		{
			start_x = (start_x & ~0xFFFFu) | crt_test_y;
			start_y &= ~0xFFFFu;
			end_x = crt_test_y;
			end_y = 0xDFu;
			color = (color & ~0xFFFFu) | 0x7Eu;
			draw_line(end_x, end_y, &start_x, &start_y, &color, &lines_left);
			crt_test_y += 8u;
			lines_left--;
		} while (lines_left != 0u);
		test_screen_ready++;
		*p_color = color;
		return;
	case 4:
		if ((0x2002u & inputs_pressed) == 0u)
			return;
		mem_write32(video_control, mem_read32(video_control) | 0x800u);
		fill_value = (fill_value & ~0xFFFFu) | 0x909u;
		set_halfwords(VIDEO_RAM, 0xE000u, fill_value);
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
		*p_color = color;
		*p_blocks_per_row = blocks_per_row;
		return;
	default:
		invalid_jump(0x9D8CCu + test_screen_ready * 3u);
	}
}

void test_game_assignments(uint32_t *p_color, uint32_t *p_blocks_per_row)
{
	uint32_t color = *p_color;
	uint32_t blocks_per_row = *p_blocks_per_row;
	uint8_t mode = 0;
	uint32_t cabinet_type = 0, cursor_positions = 0, ignored = 0, text = 0;

	if (test_screen_ready == 0u)
	{
		test_settings_changed = 0u;
		test_screen_ready++;
		clear_text_layer(&ignored, &ignored, &ignored);
		text = game_assignments_text_ADDRESS;
		print_text_list(&text, &ignored);
		test_cursor_positions = game_assignments_cursor_positions_ADDRESS;
		test_cursor = 7u;
		draw_difficulty_setting(&color, &blocks_per_row);
		draw_vitality_setting(&color, &blocks_per_row);
		text = advertise_sound_off_text_ADDRESS;
		setting_advertise_sound &= 1u;
		if (setting_advertise_sound != 0u)
			text = advertise_sound_on_text_ADDRESS;
		print_text_list(&text, &ignored);
		text = monitor_flip_normal_text_ADDRESS;
		if ((setting_monitor_flip & 2u) != 0u)
			text = monitor_flip_reverse_text_ADDRESS;
		print_text_list(&text, &ignored);
		cabinet_type = setting_cabinet_type;
		text = cabinet_type_texts[cabinet_type & 1u];
		print_text_list(&text, &ignored);
		text = start_button_use_text_ADDRESS;
		setting_no_start_buttons &= 1u;
		if (setting_no_start_buttons != 0u)
			text = start_button_no_use_text_ADDRESS;
		print_text_list(&text, &ignored);
		if ((setting_cabinet_type & 1u) != 0u && setting_moving_seat != 0u)
		{
			setting_moving_seat = 0u;
			test_settings_changed = 1u;
		}
		text = moving_seat_no_use_text_ADDRESS;
		if (setting_moving_seat != 0u)
			text = moving_seat_use_text_ADDRESS;
		print_text_list(&text, &ignored);
		if (mem_read8(moving_seat_dip) == 0u)
		{
			text = moving_seat_erase_text_ADDRESS;
			print_text_list(&text, &ignored);
		}
	}
	cursor_positions = game_assignments_cursor_positions_ADDRESS;
	erase_test_cursor(cursor_positions);
	if ((0x1001u & inputs_pressed) != 0u)
	{
		test_cursor++;
		if (test_cursor == 6u && mem_read8(moving_seat_dip) == 0u)
			test_cursor++;
		set_test_cursor();
	}
	blink_test_cursor();
	if ((0x2002u & inputs_pressed) == 0u)
	{
		*p_color = color;
		*p_blocks_per_row = blocks_per_row;
		return;
	}
	switch (test_cursor)
	{
	case 0:
		test_settings_changed = 1u;
		setting_difficulty++;
		draw_difficulty_setting(&color, &blocks_per_row);
		*p_color = color;
		*p_blocks_per_row = blocks_per_row;
		return;
	case 1:
		test_settings_changed = 1u;
		setting_initial_vitality++;
		draw_vitality_setting(&color, &blocks_per_row);
		*p_color = color;
		*p_blocks_per_row = blocks_per_row;
		return;
	case 2:
		test_settings_changed = 1u;
		setting_advertise_sound++;
		setting_advertise_sound &= 1u;
		text = advertise_sound_off_text_ADDRESS;
		if (setting_advertise_sound == 0u)
		{
			print_text_list(&text, &ignored);
			*p_color = color;
			*p_blocks_per_row = blocks_per_row;
			return;
		}
		text = advertise_sound_on_text_ADDRESS;
		print_text_list(&text, &ignored);
		*p_color = color;
		*p_blocks_per_row = blocks_per_row;
		return;
	case 3:
		test_settings_changed = 1u;
		setting_monitor_flip += 2u;
		setting_monitor_flip &= 2u;
		text = monitor_flip_normal_text_ADDRESS;
		if (setting_monitor_flip != 0u)
			text = monitor_flip_reverse_text_ADDRESS;
		print_text_list(&text, &ignored);
		mem_write32(video_control, mem_read32(video_control) | 0x800u);
		fill_value = (fill_value & ~0xFFFFu) | 0x909u;
		set_halfwords(VIDEO_RAM, 0xE000u, fill_value);
		mode = 0u;
		set_screen_mode(mode);
		draw_difficulty_setting(&color, &blocks_per_row);
		draw_vitality_setting(&color, &blocks_per_row);
		*p_color = color;
		*p_blocks_per_row = blocks_per_row;
		return;
	case 4:
		test_settings_changed = 1u;
		setting_cabinet_type++;
		setting_cabinet_type &= 1u;
		text = cabinet_type_texts[setting_cabinet_type];
		print_text_list(&text, &ignored);
		if (mem_read8(moving_seat_dip) == 0u || setting_cabinet_type == 0u)
		{
			*p_color = color;
			*p_blocks_per_row = blocks_per_row;
			return;
		}
		setting_moving_seat = 1u;
		test_settings_changed = 1u;
		setting_moving_seat++;
		setting_moving_seat &= 1u;
		text = moving_seat_no_use_text_ADDRESS;
		if (setting_moving_seat == 0u)
		{
			print_text_list(&text, &ignored);
			*p_color = color;
			*p_blocks_per_row = blocks_per_row;
			return;
		}
		text = moving_seat_use_text_ADDRESS;
		print_text_list(&text, &ignored);
		*p_color = color;
		*p_blocks_per_row = blocks_per_row;
		return;
	case 5:
		test_settings_changed = 1u;
		setting_no_start_buttons++;
		setting_no_start_buttons &= 1u;
		text = start_button_use_text_ADDRESS;
		if (setting_no_start_buttons == 0u)
		{
			print_text_list(&text, &ignored);
			*p_color = color;
			*p_blocks_per_row = blocks_per_row;
			return;
		}
		text = start_button_no_use_text_ADDRESS;
		print_text_list(&text, &ignored);
		*p_color = color;
		*p_blocks_per_row = blocks_per_row;
		return;
	case 6:
		if (setting_cabinet_type != 0u)
		{
			*p_color = color;
			*p_blocks_per_row = blocks_per_row;
			return;
		}
		test_settings_changed = 1u;
		setting_moving_seat++;
		setting_moving_seat &= 1u;
		text = moving_seat_no_use_text_ADDRESS;
		if (setting_moving_seat == 0u)
		{
			print_text_list(&text, &ignored);
			*p_color = color;
			*p_blocks_per_row = blocks_per_row;
			return;
		}
		text = moving_seat_use_text_ADDRESS;
		print_text_list(&text, &ignored);
		*p_color = color;
		*p_blocks_per_row = blocks_per_row;
		return;
	case 7:
		setting_cabinet_config = setting_cabinet_type | ((uint32_t)setting_coin_chute_type << 1);
		if (test_settings_changed != 0u)
			eeprom_save_and_lock(&color, &ignored);
		mem_write32(video_control, mem_read32(video_control) | 0x800u);
		fill_value = (fill_value & ~0xFFFFu) | 0x909u;
		set_halfwords(VIDEO_RAM, 0xE000u, fill_value);
		if (test_continue_mode != 0u)
		{
			test_screen++;
			test_screen_ready = 0u;
			*p_color = color;
			*p_blocks_per_row = blocks_per_row;
			return;
		}
		test_cursor = test_menu_item;
		test_screen = 0u;
		test_screen_ready = 1u;
		draw_test_menu();
		*p_color = color;
		*p_blocks_per_row = blocks_per_row;
		return;
	case 8:
		test_settings_changed = 1u;
		setting_small_large++;
		setting_small_large &= 1u;
		text = small_large_texts[setting_small_large];
		print_text_list(&text, &ignored);
		*p_color = color;
		*p_blocks_per_row = blocks_per_row;
		return;
	default:
		invalid_jump(0x9DBDBu + test_cursor * 3u);
	}
}

void draw_vitality_setting(uint32_t *p_color, uint32_t *p_blocks_per_row)
{
	uint32_t color = *p_color;
	uint32_t blocks_per_row = *p_blocks_per_row;
	uint32_t attribute = 0, bar_back_color = 0, bottom = 0, left = 0, right = 0, text_position = 0, top = 0, vitality_number = 0;

	if ((setting_initial_vitality & ~3u) != 0u)
		setting_initial_vitality = 0u;
	vitality_number = vitality_setting_numbers[setting_initial_vitality];
	text_position = TEXT_CELL(25, 9);
	attribute = 0x200u;
	draw_hex_4(&vitality_number, &attribute, &text_position);
	left = (vitality_number & ~0xFFFFu) | 0x78u;
	top = 0x58u;
	right = 0xF8u;
	bottom = 0x60u;
	bar_back_color = (color & ~0xFFu) | 0x17u;
	if ((mem_read32(video_control) & 0x200u) != 0u)
		flip_test_coordinates(&left, &top, &right, &bottom);
	fill_rect(top, right, bottom, &left, &bar_back_color, &blocks_per_row);
	left = (left & ~0xFFFFu) | 0x78u;
	top = 0x58u;
	bottom = 0x60u;
	right = (uint16_t)(((setting_initial_vitality + 1u) << 5) + left);
	color = (bar_back_color & ~0xFFu) | 0x49u;
	if ((mem_read32(video_control) & 0x200u) == 0u)
	{
		fill_rect(top, right, bottom, &left, &color, &blocks_per_row);
		*p_color = color;
		*p_blocks_per_row = blocks_per_row;
		return;
	}
	flip_test_coordinates(&left, &top, &right, &bottom);
	fill_rect(top, right, bottom, &left, &color, &blocks_per_row);
	*p_color = color;
	*p_blocks_per_row = blocks_per_row;
}

void draw_difficulty_setting(uint32_t *p_color, uint32_t *p_blocks_per_row)
{
	uint32_t color = *p_color;
	uint32_t blocks_per_row = *p_blocks_per_row;
	uint16_t attribute = 0;
	uint32_t bar_back_color = 0, bottom = 0, difficulty_number = 0, left = 0, right = 0, text_position = 0, top = 0;

	if ((setting_difficulty & ~3u) != 0u)
		setting_difficulty = 0u;
	difficulty_number = (uint8_t)(setting_difficulty + 1u);
	text_position = TEXT_CELL(27, 5);
	attribute = 0x200u;
	difficulty_number = draw_hex_2(difficulty_number, attribute, text_position);
	left = (difficulty_number & ~0xFFFFu) | 0x78u;
	top = 0x38u;
	right = 0xF8u;
	bottom = 0x40u;
	bar_back_color = (color & ~0xFFu) | 0x17u;
	if ((mem_read32(video_control) & 0x200u) != 0u)
		flip_test_coordinates(&left, &top, &right, &bottom);
	fill_rect(top, right, bottom, &left, &bar_back_color, &blocks_per_row);
	left = (left & ~0xFFFFu) | 0x78u;
	top = 0x38u;
	bottom = 0x40u;
	right = (uint16_t)(((setting_difficulty + 1u) << 5) + left);
	color = (bar_back_color & ~0xFFu) | 0x49u;
	if ((mem_read32(video_control) & 0x200u) == 0u)
	{
		fill_rect(top, right, bottom, &left, &color, &blocks_per_row);
		*p_color = color;
		*p_blocks_per_row = blocks_per_row;
		return;
	}
	flip_test_coordinates(&left, &top, &right, &bottom);
	fill_rect(top, right, bottom, &left, &color, &blocks_per_row);
	*p_color = color;
	*p_blocks_per_row = blocks_per_row;
}

uint32_t test_coin_assignments(uint32_t color)
{
	uint8_t mode = 0;
	uint16_t attribute = 0;
	uint32_t credits_to_start = 0, cursor_positions = 0, ignored = 0, text = 0, text_position = 0, value = 0;

	if (test_screen_ready == 0u)
	{
		test_screen_ready++;
		test_settings_changed = 0u;
		test_cursor = 4u;
		test_coin_assignments_draw_main_page();
		return color;
	}
	if (coin_manual_page != 0u)
		return test_coin_assignments_manual_page(color);
	cursor_positions = coin_assignments_cursor_positions_ADDRESS;
	erase_test_cursor(cursor_positions);
	if ((0x1001u & inputs_pressed) != 0u)
	{
		test_cursor++;
		set_test_cursor();
	}
	blink_test_cursor();
	if ((0x2002u & inputs_pressed) == 0u)
		return color;
	switch (test_cursor)
	{
	case 0:
	case 5:
		test_settings_changed = 1u;
		setting_coin_chute_type++;
		setting_coin_chute_type &= 1u;
		text = coin_chute_type_individual_text_ADDRESS;
		if (setting_coin_chute_type == 0u)
			text = coin_chute_type_common_text_ADDRESS;
		print_text_list(&text, &ignored);
		if (setting_coin_number < 0)
		{
			text = coin_setting_erase_text_ADDRESS;
			print_text_list(&text, &ignored);
			text = coin_chute_2_label_erase_text_ADDRESS;
			print_text_list(&text, &ignored);
			text = coin_setting_number_manual_text_ADDRESS;
			print_text_list(&text, &ignored);
			if (setting_coin_chute_type == 0u)
			{
				text = coin_setting_text_manual_ADDRESS;
				print_text_list(&text, &ignored);
				text = coin_chute_labels_text_ADDRESS;
				print_text_list(&text, &ignored);
			}
			else
			{
				text = coin_setting_text_manual_individual_ADDRESS;
				print_text_list(&text, &ignored);
			}
			return color;
		}
		text = coin_setting_erase_text_ADDRESS;
		print_text_list(&text, &ignored);
		text = coin_chute_2_label_erase_text_ADDRESS;
		print_text_list(&text, &ignored);
		value = 1u;
		text_position = TEXT_CELL(31, 9);
		attribute = 0x1200u;
		draw_hex_2(value, attribute, text_position);
		if (setting_coin_chute_type == 0u)
		{
			text = coin_chute_labels_text_ADDRESS;
			print_text_list(&text, &ignored);
			text = coin_setting_text_1_ADDRESS;
			print_text_list(&text, &ignored);
			setting_coin_number = 0;
		}
		else
		{
			text = coin_setting_text_1_individual_ADDRESS;
			print_text_list(&text, &ignored);
			setting_coin_number_individual = 0;
		}
		apply_coin_setting();
		return color;
	case 1:
		test_settings_changed = 1u;
		setting_credits_to_start++;
		if (setting_credits_to_start == 3u)
			setting_credits_to_start = 1u;
		credits_to_start = setting_credits_to_start;
		text = credits_to_start_texts[credits_to_start >> 1];
		print_text_list(&text, &ignored);
		return color;
	case 2:
		setting_coin_number++;
		setting_coin_number_individual++;
		apply_coin_setting();
		return color;
	case 3:
		test_settings_changed = 1u;
		coin_manual_page++;
		test_screen_ready = 0u;
		mode = 1u;
		set_screen_mode(mode);
		mem_write16(video_control, mem_read16(video_control) | 0x8000u);
		return test_coin_assignments_manual_page(color);
	case 4:
		setting_cabinet_config = setting_cabinet_type | ((uint32_t)setting_coin_chute_type << 1);
		if (test_settings_changed != 0u)
			eeprom_save_and_lock(&color, &ignored);
		if (test_continue_mode != 0u)
		{
			test_screen++;
			test_screen_ready = 0u;
			return color;
		}
		test_cursor = test_menu_item;
		test_screen = 0u;
		test_screen_ready = 1u;
		draw_test_menu();
		return color;
	default:
		invalid_jump(0x9E0A9u + test_cursor * 3u);
	}
}

void test_coin_assignments_draw_main_page(void)
{
	uint16_t attribute = 0;
	uint32_t adder_setting = 0, credits_to_start = 0, ignored = 0, print_position = 0, setting = 0, setting_number = 0, text = 0;

	clear_text_layer(&ignored, &ignored, &ignored);
	coin_manual_page = 0u;
	test_cursor_positions = coin_assignments_cursor_positions_ADDRESS;
	if (setting_coin_number < 0)
		setting_coin_number_individual = 0;
	setting = setting_coins_per_credit_ADDRESS;
	clamp_coin_setting(setting);
	adder_setting = setting_bonus_adder_ADDRESS;
	clamp_coin_setting(adder_setting);
	if (setting_bonus_adder == 1u)
		setting_bonus_adder = 0u;
	setting = setting_chute_1_multiplier_ADDRESS;
	clamp_coin_setting(setting);
	setting = setting_chute_2_multiplier_ADDRESS;
	clamp_coin_setting(setting);
	if (setting_coin_chute_type == 0u)
	{
		text = coin_chute_type_common_text_ADDRESS;
		print_text_list(&text, &ignored);
		text = coin_assignments_common_text_ADDRESS;
		print_text_list(&text, &ignored);
		if (setting_coin_number >= 0)
		{
			text = coin_setting_texts[(uint8_t)setting_coin_number];
			print_text_list(&text, &ignored);
			setting_number = (uint8_t)setting_coin_number;
			setting_number = (uint8_t)(setting_number + 1u);
			convert_to_bcd(&setting_number, &ignored);
			print_position = TEXT_CELL(31, 9);
			attribute = 0x1200u;
			draw_hex_2(setting_number, attribute, print_position);
		}
	}
	else
	{
		text = coin_chute_type_individual_text_ADDRESS;
		print_text_list(&text, &ignored);
		text = coin_setting_texts_individual[(uint8_t)setting_coin_number_individual];
		print_text_list(&text, &ignored);
		text = coin_assignments_text_ADDRESS;
		print_text_list(&text, &ignored);
		setting_number = individual_coin_setting_numbers[(uint8_t)setting_coin_number_individual];
		convert_to_bcd(&setting_number, &ignored);
		print_position = TEXT_CELL(31, 9);
		attribute = 0x1200u;
		draw_hex_2(setting_number, attribute, print_position);
	}
	credits_to_start = setting_credits_to_start;
	text = credits_to_start_texts[credits_to_start >> 1];
	print_text_list(&text, &ignored);
	if (setting_coin_number >= 0)
		return;
	setting_coin_number_individual = -1;
	text = coin_setting_erase_text_ADDRESS;
	print_text_list(&text, &ignored);
	text = coin_setting_number_manual_text_ADDRESS;
	print_text_list(&text, &ignored);
	text = coin_setting_text_manual_ADDRESS;
	if (setting_coin_chute_type != 0u)
		text = coin_setting_text_manual_individual_ADDRESS;
	print_text_list(&text, &ignored);
}

void test_coin_assignments_spare_return(void)
{
}

void apply_coin_setting(void)
{
	uint32_t coin_setting = 0, ignored = 0, setting_number = 0, text = 0, values_record = 0;

	test_settings_changed = 1u;
	text = coin_setting_number_erase_text_ADDRESS;
	print_text_list(&text, &ignored);
	text = coin_setting_erase_text_ADDRESS;
	print_text_list(&text, &ignored);
	if (setting_coin_number > 0x19)
		setting_coin_number = 0;
	if (setting_coin_number_individual > 0xE)
		setting_coin_number_individual = 0;
	if (setting_coin_chute_type != 0u)
	{
		text = coin_setting_texts_individual[(uint8_t)setting_coin_number_individual];
		print_text_list(&text, &ignored);
		setting_number = individual_coin_setting_numbers[(uint8_t)setting_coin_number_individual];
		draw_coin_setting_number(setting_number);
		coin_setting = individual_coin_setting_numbers[(uint8_t)setting_coin_number_individual];
	}
	else
	{
		text = coin_setting_texts[(uint8_t)setting_coin_number];
		print_text_list(&text, &ignored);
		coin_setting = (uint8_t)setting_coin_number;
		coin_setting = (uint8_t)(coin_setting + 1u);
		draw_coin_setting_number(coin_setting);
		coin_setting = (uint8_t)setting_coin_number;
		coin_setting = (uint8_t)(coin_setting + 1u);
	}
	values_record = coin_setting_values_ADDRESS + (uint8_t)(coin_setting - 1u) * 4u;
	setting_coins_per_credit = ROM8(values_record);
	values_record++;
	setting_bonus_adder = ROM8(values_record);
	values_record++;
	setting_chute_1_multiplier = ROM8(values_record);
	setting_chute_2_multiplier = ROM8(values_record + 1u);
}

void draw_coin_setting_number(uint32_t setting_number)
{
	uint16_t attribute = 0;
	uint32_t ignored = 0, text_position = 0;

	convert_to_bcd(&setting_number, &ignored);
	text_position = TEXT_CELL(31, 9);
	attribute = 0x1200u;
	draw_hex_2(setting_number, attribute, text_position);
}

uint32_t test_coin_assignments_manual_page(uint32_t color)
{
	uint8_t mode = 0;
	uint16_t attribute = 0;
	uint32_t coins_left = 0, cursor_positions = 0, ignored = 0, manual_print_position = 0, text = 0, text_position = 0, value = 0;

	if (test_screen_ready == 0u)
	{
		test_screen_ready++;
		clear_text_layer(&ignored, &ignored, &manual_print_position);
		test_cursor_positions = coin_manual_cursor_positions_ADDRESS;
		test_cursor = 4u;
		text = coin_manual_page_common_text_ADDRESS;
		if (setting_coin_chute_type != 0u)
			text = coin_manual_page_text_ADDRESS;
		print_text_list(&text, &ignored);
		text = coins_per_credit_texts[setting_coins_per_credit];
		print_text_list(&text, &ignored);
		text = bonus_adder_texts[setting_bonus_adder];
		print_text_list(&text, &ignored);
		value = setting_chute_1_multiplier;
		text_position = TEXT_CELL(30, 12);
		attribute = 0x1200u;
		draw_hex_2(value, attribute, text_position);
		text = chute_1_coin_text_ADDRESS;
		if (setting_chute_1_multiplier != 1u)
			text = chute_1_coins_text_ADDRESS;
		print_text_list(&text, &ignored);
		coins_left = draw_coin_credit_table();
		if (setting_coin_chute_type != 0u)
			return coins_left;
		value = setting_chute_2_multiplier;
		text_position = TEXT_CELL(30, 18);
		attribute = 0x1200u;
		draw_hex_2(value, attribute, text_position);
		text = chute_2_coin_text_ADDRESS;
		if (setting_chute_2_multiplier != 1u)
			text = chute_2_coins_text_ADDRESS;
		print_text_list(&text, &ignored);
		return coins_left;
	}
	cursor_positions = coin_manual_cursor_positions_ADDRESS;
	erase_test_cursor(cursor_positions);
	if ((0x1001u & inputs_pressed) != 0u)
	{
		test_cursor++;
		if (setting_coin_chute_type != 0u && test_cursor == 3u)
			test_cursor++;
		set_test_cursor();
	}
	blink_test_cursor();
	if ((0x2002u & inputs_pressed) == 0u)
		return color;
	switch (test_cursor)
	{
	case 0:
	case 5:
		setting_coin_number = -1;
		setting_coins_per_credit++;
		if ((int8_t)setting_coins_per_credit > 9)
			setting_coins_per_credit = 1u;
		text = coins_per_credit_texts[setting_coins_per_credit];
		print_text_list(&text, &ignored);
		text = coin_credit_rows_erase_text_ADDRESS;
		print_text_list(&text, &ignored);
		coins_left = draw_coin_credit_table();
		return coins_left;
	case 1:
		setting_coin_number = -1;
		setting_bonus_adder++;
		if ((int8_t)setting_bonus_adder > 9)
			setting_bonus_adder = 0u;
		if (setting_bonus_adder == 1u)
			setting_bonus_adder = 2u;
		text = bonus_adder_texts[setting_bonus_adder];
		print_text_list(&text, &ignored);
		text = coin_credit_rows_erase_text_ADDRESS;
		print_text_list(&text, &ignored);
		coins_left = draw_coin_credit_table();
		return coins_left;
	case 2:
		setting_coin_number = -1;
		setting_chute_1_multiplier++;
		if ((int8_t)setting_chute_1_multiplier > 9)
			setting_chute_1_multiplier = 1u;
		value = setting_chute_1_multiplier;
		text_position = TEXT_CELL(30, 12);
		attribute = 0x1200u;
		draw_hex_2(value, attribute, text_position);
		text = chute_1_coin_text_ADDRESS;
		if (setting_chute_1_multiplier != 1u)
			text = chute_1_coins_text_ADDRESS;
		print_text_list(&text, &ignored);
		text = coin_credit_rows_erase_text_ADDRESS;
		print_text_list(&text, &ignored);
		coins_left = draw_coin_credit_table();
		return coins_left;
	case 3:
		setting_coin_number = -1;
		setting_chute_2_multiplier++;
		if ((int8_t)setting_chute_2_multiplier > 9)
			setting_chute_2_multiplier = 1u;
		value = setting_chute_2_multiplier;
		text_position = TEXT_CELL(30, 18);
		attribute = 0x1200u;
		draw_hex_2(value, attribute, text_position);
		text = chute_2_coin_text_ADDRESS;
		if (setting_chute_2_multiplier != 1u)
			text = chute_2_coins_text_ADDRESS;
		print_text_list(&text, &ignored);
		text = coin_credit_rows_erase_text_ADDRESS;
		print_text_list(&text, &ignored);
		coins_left = draw_coin_credit_table();
		return coins_left;
	case 4:
		test_cursor = 3u;
		mode = 0u;
		set_screen_mode(mode);
		test_coin_assignments_draw_main_page();
		return color;
	default:
		invalid_jump(0x9E479u + test_cursor * 3u);
	}
}

void test_coin_assignments_spare_return_2(void)
{
}

void clamp_coin_setting(uint32_t setting)
{
	if ((int8_t)mem_read8(setting) <= 0)
		mem_write8(setting, 1u);
	if ((int8_t)mem_read8(setting) > 9)
		mem_write8(setting, 9u);
}

uint32_t draw_coin_credit_table(void)
{
	uint16_t multiplier = 0;
	uint32_t cell_position = 0, coins = 0;

	coins = 9u;
	cell_position = chute_1_credit_cell_positions_ADDRESS;
	do
	{
		multiplier = setting_chute_1_multiplier;
		cell_position = draw_coin_credit_cell(multiplier, (uint16_t)coins, cell_position);
		coins--;
	} while (coins != 0u);
	if (setting_coin_chute_type != 0u)
		return coins;
	coins = 9u;
	cell_position = chute_2_credit_cell_positions_ADDRESS;
	do
	{
		multiplier = setting_chute_2_multiplier;
		cell_position = draw_coin_credit_cell(multiplier, (uint16_t)coins, cell_position);
		coins--;
	} while (coins != 0u);
	return coins;
}

uint32_t draw_coin_credit_cell(uint16_t multiplier, uint16_t coins, uint32_t cell_position)
{
	uint8_t attribute = 0;
	uint16_t attribute_2 = 0;
	uint32_t adder_setting = 0, coin_count = 0, coins_per_credit = 0, column = 0, fraction_coins = 0, ignored = 0, text_position = 0, value = 0, whole_credits = 0;

	coin_count = coins;
	coins_per_credit = setting_coins_per_credit;
	adder_setting = setting_bonus_adder;
	if (adder_setting == 0u)
		coin_count = divide_s32((coin_count * multiplier) << 16, coins_per_credit);
	else
	{
		multiplier = (uint16_t)(multiplier * coin_count);
		coin_count = divide_s32((multiplier + multiplier * adder_setting) << 16, coins_per_credit * adder_setting);
		coins_per_credit = setting_coins_per_credit;
	}
	whole_credits = coin_count >> 16;
	coin_count &= 0xFFFFu;
	fraction_coins = divide_s32(0x10000u, coins_per_credit) & 0xFFFFu;
	if (fraction_coins == 0u || coin_count == 0u)
		fraction_coins = 0u;
	else
		fraction_coins = divide_s32(coin_count, fraction_coins);
	if ((int32_t)whole_credits >= 9)
	{
		whole_credits = 9u;
		fraction_coins = 0u;
	}
	column = TEXT_CELL_POSITION(cell_position)->column;
	cell_position++;
	text_position = TEXT_CELL(0, 0) + (column << 1) + ((uint32_t)TEXT_CELL_POSITION(cell_position - 0x1u)->row << 7);
	attribute = 2u;
	cell_position++;
	if (fraction_coins != 0u)
	{
		draw_fraction(attribute, &fraction_coins, &coins_per_credit, &text_position);
		text_position -= 4u;
	}
	value = whole_credits;
	text_position -= 2u;
	convert_to_bcd(&value, &ignored);
	attribute_2 = 0x200u;
	draw_hex_2(value, attribute_2, text_position);
	return cell_position;
}

void draw_test_hex_4(uint32_t value, uint32_t text_position)
{
	uint32_t attribute = 0;

	attribute = 0x200u;
	draw_hex_4(&value, &attribute, &text_position);
}

void draw_test_hex_2(uint32_t value, uint32_t text_position)
{
	uint16_t attribute = 0;

	attribute = 0x200u;
	draw_hex_2(value, attribute, text_position);
}

void test_bookkeeping(void)
{
	uint32_t average_hours = 0, average_minutes = 0, average_seconds = 0, chute_3_coins = 0, days = 0, frames = 0, frames_high = 0, hours = 0, ignored = 0, minutes = 0, print_position = 0, seconds = 0, shortest_game_frames = 0, shortest_hours = 0, shortest_minutes = 0, shortest_seconds = 0, text = 0, text_position = 0, time_high = 0, total_coins = 0, value = 0;

	if (test_screen_ready == 0u)
	{
		test_screen_ready++;
		clear_text_layer(&ignored, &ignored, &print_position);
		text = bookkeeping_text_ADDRESS;
		print_text_list(&text, &ignored);
		test_cursor_positions = bookkeeping_cursor_positions_ADDRESS;
		test_cursor = 0u;
		current_object = bookkeeping_coin_credits;
		if (setting_coin_chute_type == 0u)
			text = bookkeeping_chute_3_erase_text_ADDRESS;
		if (setting_cabinet_type == 0u)
			text = bookkeeping_chute_3_erase_text_ADDRESS;
		print_text_list(&text, &ignored);
		value = mem_read16(current_object + 4u);
		convert_to_bcd(&value, &ignored);
		text_position = TEXT_CELL(26, 3);
		draw_test_hex_4(value, text_position);
		value = mem_read16(current_object + 6u);
		convert_to_bcd(&value, &ignored);
		text_position = TEXT_CELL(26, 4);
		draw_test_hex_4(value, text_position);
		if (setting_coin_chute_type != 0u && setting_cabinet_type != 0u)
		{
			value = mem_read16(current_object + 8u);
			convert_to_bcd(&value, &ignored);
			text_position = TEXT_CELL(26, 5);
			draw_test_hex_4(value, text_position);
		}
		chute_3_coins = mem_read16(current_object + 8u);
		total_coins = (uint32_t)mem_read16(current_object + 4u) + mem_read16(current_object + 6u);
		if (setting_coin_chute_type != 0u && setting_cabinet_type != 0u)
			total_coins += chute_3_coins;
		convert_to_bcd(&total_coins, &ignored);
		text_position = TEXT_CELL(26, 7);
		draw_test_hex_4(total_coins, text_position);
		value = mem_read16(current_object);
		convert_to_bcd(&value, &ignored);
		text_position = TEXT_CELL(26, 9);
		draw_test_hex_4(value, text_position);
		value = mem_read16(current_object + 2u);
		convert_to_bcd(&value, &ignored);
		text_position = TEXT_CELL(26, 10);
		draw_test_hex_4(value, text_position);
		value = (uint32_t)mem_read16(current_object) + mem_read16(current_object + 2u);
		convert_to_bcd(&value, &ignored);
		text_position = TEXT_CELL(26, 11);
		draw_test_hex_4(value, text_position);
		value = mem_read16(current_object + 0xAu);
		convert_to_bcd(&value, &ignored);
		text_position = TEXT_CELL(26, 13);
		draw_test_hex_4(value, text_position);
		frames = mem_read32(current_object + 0x10u);
		frames_high = mem_read16(current_object + 0x14u);
		split_frame_count(frames, frames_high, &seconds, &minutes, &hours, &days);
		value = days;
		convert_to_bcd(&value, &ignored);
		text_position = TEXT_CELL(21, 15);
		draw_test_hex_4(value, text_position);
		value = hours;
		convert_to_bcd(&value, &ignored);
		text_position = TEXT_CELL(26, 15);
		draw_test_hex_2(value, text_position);
		value = minutes;
		convert_to_bcd(&value, &ignored);
		text_position = TEXT_CELL(29, 15);
		draw_test_hex_2(value, text_position);
		value = seconds;
		convert_to_bcd(&value, &ignored);
		text_position = TEXT_CELL(32, 15);
		draw_test_hex_2(value, text_position);
		frames = mem_read32(current_object + 0x16u);
		frames_high = mem_read16(current_object + 0x1Au);
		split_frame_count(frames, frames_high, &seconds, &minutes, &hours, &days);
		value = days;
		convert_to_bcd(&value, &ignored);
		text_position = TEXT_CELL(21, 16);
		draw_test_hex_4(value, text_position);
		value = hours;
		convert_to_bcd(&value, &ignored);
		text_position = TEXT_CELL(26, 16);
		draw_test_hex_2(value, text_position);
		value = minutes;
		convert_to_bcd(&value, &ignored);
		text_position = TEXT_CELL(29, 16);
		draw_test_hex_2(value, text_position);
		value = seconds;
		convert_to_bcd(&value, &ignored);
		text_position = TEXT_CELL(32, 16);
		draw_test_hex_2(value, text_position);
		average_seconds = 0u;
		average_minutes = 0u;
		average_hours = 0u;
		if (mem_read16(current_object + 0xAu) != 0u)
		{
			frames = mem_read32(current_object + 0x1Cu);
			time_high = mem_read16(current_object + 0x20u);
			divx_pair(mem_read16(current_object + 0xAu), &frames, &time_high);
			frames_high = 0u;
			split_frame_count(frames, frames_high, &average_seconds, &average_minutes, &average_hours, &ignored);
		}
		value = average_hours;
		convert_to_bcd(&value, &ignored);
		text_position = TEXT_CELL(26, 17);
		draw_test_hex_2(value, text_position);
		value = average_minutes;
		convert_to_bcd(&value, &ignored);
		text_position = TEXT_CELL(29, 17);
		draw_test_hex_2(value, text_position);
		value = average_seconds;
		convert_to_bcd(&value, &ignored);
		text_position = TEXT_CELL(32, 17);
		draw_test_hex_2(value, text_position);
		frames = mem_read32(current_object + 0xCu);
		frames_high = 0u;
		split_frame_count(frames, frames_high, &seconds, &minutes, &hours, &ignored);
		value = hours;
		convert_to_bcd(&value, &ignored);
		text_position = TEXT_CELL(26, 18);
		draw_test_hex_2(value, text_position);
		value = minutes;
		convert_to_bcd(&value, &ignored);
		text_position = TEXT_CELL(29, 18);
		draw_test_hex_2(value, text_position);
		value = seconds;
		convert_to_bcd(&value, &ignored);
		text_position = TEXT_CELL(32, 18);
		draw_test_hex_2(value, text_position);
		shortest_seconds = 0u;
		shortest_minutes = 0u;
		shortest_hours = 0u;
		frames_high = 0u;
		shortest_game_frames = mem_read32(current_object + 0x22u);
		if (((int8_t)0xFFu & shortest_game_frames) + 1u != 0u)
			split_frame_count(shortest_game_frames, frames_high, &shortest_seconds, &shortest_minutes, &shortest_hours, &ignored);
		value = shortest_hours;
		convert_to_bcd(&value, &ignored);
		text_position = TEXT_CELL(26, 19);
		draw_test_hex_2(value, text_position);
		value = shortest_minutes;
		convert_to_bcd(&value, &ignored);
		text_position = TEXT_CELL(29, 19);
		draw_test_hex_2(value, text_position);
		value = shortest_seconds;
		convert_to_bcd(&value, &ignored);
		text_position = TEXT_CELL(32, 19);
		draw_test_hex_2(value, text_position);
	}
	if ((0x2002u & inputs_pressed) == 0u)
		return;
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
}

uint32_t test_backup_clear(uint32_t polynomial)
{
	uint32_t crc_polynomial = 0, cursor_positions = 0, ignored = 0, text = 0;

	if (test_screen_ready == 0u)
	{
		test_screen_ready++;
		clear_text_layer(&ignored, &ignored, &ignored);
		text = backup_clear_text_ADDRESS;
		print_text_list(&text, &ignored);
		test_cursor_positions = backup_clear_cursor_positions_ADDRESS;
		test_cursor = 1u;
		backup_clear_timer = 0u;
	}
	if (backup_clear_timer != 0u)
	{
		backup_clear_timer++;
		if ((int8_t)backup_clear_timer < 0x78)
			return polynomial;
		if (test_continue_mode == 0u)
		{
			test_cursor = test_menu_item;
			test_screen = 0u;
			test_screen_ready = 1u;
			draw_test_menu();
			return polynomial;
		}
		if (mem_read8(moving_seat_dip) != 0u)
		{
			test_screen++;
			test_screen_ready = 0u;
			return polynomial;
		}
		test_cursor = 0xAu;
		test_screen = 0u;
		test_screen_ready = 1u;
		draw_test_menu();
		return polynomial;
	}
	cursor_positions = backup_clear_cursor_positions_ADDRESS;
	erase_test_cursor(cursor_positions);
	if ((0x1001u & inputs_pressed) != 0u)
	{
		test_cursor++;
		set_test_cursor();
	}
	blink_test_cursor();
	if ((0x2002u & inputs_pressed) == 0u)
		return polynomial;
	switch (test_cursor)
	{
	case 0:
	case 2:
		text = backup_clear_completed_text_ADDRESS;
		print_text_list(&text, &ignored);
		reset_bookkeeping();
		crc_polynomial = reset_ranking();
		text = backup_clear_done_text_ADDRESS;
		print_text_list(&text, &ignored);
		backup_clear_timer++;
		return crc_polynomial;
	case 1:
		if (test_continue_mode == 0u)
		{
			test_cursor = test_menu_item;
			test_screen = 0u;
			test_screen_ready = 1u;
			draw_test_menu();
			return polynomial;
		}
		if (mem_read8(moving_seat_dip) != 0u)
		{
			test_screen++;
			test_screen_ready = 0u;
			return polynomial;
		}
		test_cursor = 0xAu;
		test_screen = 0u;
		test_screen_ready = 1u;
		draw_test_menu();
		return polynomial;
	default:
		invalid_jump(0x9EABDu + test_cursor * 3u);
	}
}

void test_outputs(void)
{
	uint32_t ignored = 0, setup_print_position = 0, text = 0;
	uint8_t port_d_bits = 0, port_d_bits_1p = 0, port_d_bits_2p = 0;

	if (test_screen_ready == 0u)
	{
		test_screen_ready++;
		clear_text_layer(&ignored, &ignored, &setup_print_position);
		text = output_test_text_ADDRESS;
		print_text_list(&text, &ignored);
		if (setting_moving_seat != 0u)
		{
			text = output_test_seat_text_ADDRESS;
			print_text_list(&text, &ignored);
			return;
		}
		text = output_test_no_use_text_ADDRESS;
		print_text_list(&text, &ignored);
		return;
	}
	port_d_bits = mem_read8(io_port_d);
	mem_write8(io_port_d, port_d_bits & ~0xCu);
	if ((0x2002u & inputs_pressed) != 0u)
	{
		if (test_continue_mode == 0u)
		{
			test_cursor = test_menu_item;
			test_screen = 0u;
			test_screen_ready = 1u;
			draw_test_menu();
			return;
		}
		test_cursor = 0xAu;
		test_screen = 0u;
		test_screen_ready = 1u;
		draw_test_menu();
		text = continue_mode_text_ADDRESS;
		print_text_list(&text, &ignored);
		return;
	}
	if (setting_moving_seat == 0u)
		return;
	text = output_test_1p_off_text_ADDRESS;
	if ((input_ports_active & 0x10000u) != 0u)
	{
		port_d_bits_1p = mem_read8(io_port_d);
		mem_write8(io_port_d, port_d_bits_1p | 4u);
		text = output_test_1p_on_text_ADDRESS;
	}
	print_text_list(&text, &ignored);
	text = output_test_2p_off_text_ADDRESS;
	if ((input_ports_active & 0x1000000u) != 0u)
	{
		port_d_bits_2p = mem_read8(io_port_d);
		mem_write8(io_port_d, port_d_bits_2p | 8u);
		text = output_test_2p_on_text_ADDRESS;
	}
	print_text_list(&text, &ignored);
}

void exit_test_mode(void)
{
	uint32_t ignored = 0, print_position = 0, text = 0;

	if (test_screen_ready != 0u)
	{
		if ((0x2002u & inputs_pressed) == 0u)
			return;
		p1_bonus_coins = 0u;
		p1_credits = 0u;
		p2_bonus_coins = 0u;
		p2_credits = 0u;
		p3_bonus_coins = 0u;
		p3_credits = 0u;
		test_mode = 0u;
		return;
	}
	if ((int16_t)(uint16_t)eeprom_write_count < 0x2710)
	{
		p1_bonus_coins = 0u;
		p1_credits = 0u;
		p2_bonus_coins = 0u;
		p2_credits = 0u;
		p3_bonus_coins = 0u;
		p3_credits = 0u;
		test_mode = 0u;
		return;
	}
	clear_text_layer(&ignored, &ignored, &print_position);
	text = eeprom_caution_text_ADDRESS;
	print_text_list(&text, &ignored);
	test_screen_ready++;
	if ((0x2002u & inputs_pressed) == 0u)
		return;
	p1_bonus_coins = 0u;
	p1_credits = 0u;
	p2_bonus_coins = 0u;
	p2_credits = 0u;
	p3_bonus_coins = 0u;
	p3_credits = 0u;
	test_mode = 0u;
}

void flip_test_coordinates(uint32_t *p_left, uint32_t *p_top, uint32_t *p_right, uint32_t *p_bottom)
{
	uint16_t left = (uint16_t)*p_left;
	uint16_t top = (uint16_t)*p_top;
	uint16_t right = (uint16_t)*p_right;
	uint16_t bottom = (uint16_t)*p_bottom;

	left = (uint16_t)(0u - (left - 0xA0u) + 0xA0u);
	right = (uint16_t)(0u - (right - 0xA0u) + 0xA0u);
	top = (uint16_t)(0u - (top - 0x70u) + 0x70u);
	bottom = (uint16_t)(0u - (bottom - 0x70u) + 0x70u);
	*p_left = (*p_left & 0xFFFF0000u) | left;
	*p_top = (*p_top & 0xFFFF0000u) | top;
	*p_right = (*p_right & 0xFFFF0000u) | right;
	*p_bottom = (*p_bottom & 0xFFFF0000u) | bottom;
}

void blink_test_cursor(void)
{
	uint32_t cursor_char = 0;

	cursor_char = test_cursor_arrow_ADDRESS;
	draw_blinking_test_cursor(cursor_char);
}

void draw_blinking_test_cursor(uint32_t cursor_char)
{
	uint8_t blink_before = 0;

	blink_before = test_cursor_blink;
	test_cursor_blink = blink_before - 1u;
	if ((int8_t)blink_before < 1)
		cursor_char = test_cursor_space_ADDRESS;
	if (test_cursor_blink <= -0xF)
		test_cursor_blink = 0x1E;
	mem_write16(TEXT_CELL(0, 0) + (((uint32_t)test_cursor_row << 7) + ((uint32_t)test_cursor_column << 1)), 0x200u | ROM8(cursor_char));
}

void erase_test_cursor(uint32_t cursor_positions)
{
	uint32_t cursor_char = 0;

	cursor_positions += (uint32_t)test_cursor << 1;
	test_cursor_column = TEXT_CELL_POSITION(cursor_positions)->column;
	test_cursor_row = TEXT_CELL_POSITION(cursor_positions)->row;
	cursor_char = test_cursor_space_ADDRESS;
	draw_blinking_test_cursor(cursor_char);
}

void set_test_cursor(void)
{
	uint32_t cursor_entry = 0;

	for (;;)
	{
		cursor_entry = test_cursor_positions + ((uint32_t)test_cursor << 1);
		if ((int8_t)TEXT_CELL_POSITION(cursor_entry)->column >= 0)
			break;
		test_cursor = 0u;
	}
	test_cursor_column = TEXT_CELL_POSITION(cursor_entry)->column;
	test_cursor_row = TEXT_CELL_POSITION(cursor_entry)->row;
}

uint32_t check_moving_seat_dip(uint32_t words_left)
{
	uint8_t seat_dip_on = 0;
	uint32_t dip_flag_address = 0, ignored = 0;
	uint8_t port_f_bits = 0;

	port_f_bits = mem_read8(io_port_f);
	dip_flag_address = moving_seat_dip;
	seat_dip_on = (uint8_t)((port_f_bits & 1u) ^ 1u);
	if ((mem_read8(moving_seat_dip) & 1u) == seat_dip_on)
		return words_left;
	setting_moving_seat = seat_dip_on;
	mem_write8(dip_flag_address, seat_dip_on);
	if (seat_dip_on != 0u)
	{
		setting_cabinet_type = 0u;
		setting_cabinet_config = setting_cabinet_type | ((uint32_t)setting_coin_chute_type << 1);
	}
	eeprom_save_and_lock(&ignored, &words_left);
	return words_left;
}
