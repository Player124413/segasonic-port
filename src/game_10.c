/**
 *    Name:        game_10.c
 *    Description: the game's functions, part 10 (in the order of their V60 addresses)
 *    Version:     0.1.0
 *    Created:     12-10-2026
 *    Modified:    13-10-2026
 *    Author:      Sebastian Toyos
 *
 *    SegaSonic The Hedgehog (arcade, Sega System 32): the game's V60 program decompiled into C by
 *    tools/decompile.py, with the names of symbols.txt.
 */
#include "game.h"

void clip_line_end_inside(void)
{
}

void clip_line_to_left_edge(uint32_t end_x, uint32_t end_y, uint32_t *p_start_x, uint32_t *p_start_y, uint32_t *p_unclipped_y)
{
	uint32_t start_x = *p_start_x;
	uint32_t start_y = *p_start_y;
	uint32_t r8 = 0, dx = 0;

	dx = bitmap_window_left - start_x;
	clip_line_at_x(end_x, end_y, dx, &start_x, &start_y, &r8);
	*p_start_x = start_x;
	*p_start_y = start_y;
	*p_unclipped_y = r8;
}

void clip_line_to_right_edge(uint32_t end_x, uint32_t end_y, uint32_t *p_start_x, uint32_t *p_start_y, uint32_t *p_unclipped_y)
{
	uint32_t start_x = *p_start_x;
	uint32_t start_y = *p_start_y;
	uint32_t r8 = 0, dx = 0;

	dx = bitmap_window_right - start_x;
	clip_line_at_x(end_x, end_y, dx, &start_x, &start_y, &r8);
	*p_start_x = start_x;
	*p_start_y = start_y;
	*p_unclipped_y = r8;
}

void clip_line_at_x(uint32_t end_x, uint32_t end_y, uint32_t dx, uint32_t *p_start_x, uint32_t *p_start_y, uint32_t *p_unclipped_y)
{
	uint32_t start_x = *p_start_x;
	uint32_t start_y = *p_start_y;
	uint32_t dy_scaled = 0, line_dx = 0, unclipped_y = 0;

	line_dx = end_x - start_x;
	unclipped_y = start_y;
	dy_scaled = (end_y - unclipped_y) * dx;
	start_x += dx;
	if (line_dx != 0u)
	{
		start_y += divide_s32(dy_scaled, line_dx);
		*p_start_x = start_x;
		*p_start_y = start_y;
		*p_unclipped_y = unclipped_y;
		return;
	}
	*p_start_x = start_x;
	*p_unclipped_y = unclipped_y;
}

void clip_line_to_top_edge(uint32_t end_x, uint32_t end_y, uint32_t *p_start_x, uint32_t *p_start_y, uint32_t *p_unclipped_y)
{
	uint32_t start_x = *p_start_x;
	uint32_t start_y = *p_start_y;
	uint32_t r8 = 0, dy = 0;

	dy = bitmap_window_top - start_y;
	clip_line_at_y(end_x, end_y, dy, &start_x, &start_y, &r8);
	*p_start_x = start_x;
	*p_start_y = start_y;
	*p_unclipped_y = r8;
}

void clip_line_to_bottom_edge(uint32_t end_x, uint32_t end_y, uint32_t *p_start_x, uint32_t *p_start_y, uint32_t *p_unclipped_y)
{
	uint32_t start_x = *p_start_x;
	uint32_t start_y = *p_start_y;
	uint32_t r8 = 0, dy = 0;

	dy = bitmap_window_bottom - start_y;
	clip_line_at_y(end_x, end_y, dy, &start_x, &start_y, &r8);
	*p_start_x = start_x;
	*p_start_y = start_y;
	*p_unclipped_y = r8;
}

void clip_line_at_y(uint32_t end_x, uint32_t end_y, uint32_t dy, uint32_t *p_start_x, uint32_t *p_start_y, uint32_t *p_unclipped_y)
{
	uint32_t start_x = *p_start_x;
	uint32_t start_y = *p_start_y;
	uint32_t dx_scaled = 0, line_dy = 0, unclipped_y = 0;

	unclipped_y = start_y;
	line_dy = end_y - unclipped_y;
	dx_scaled = (end_x - start_x) * dy;
	start_y += dy;
	if (line_dy != 0u)
	{
		start_x += divide_s32(dx_scaled, line_dy);
		*p_start_x = start_x;
		*p_start_y = start_y;
		*p_unclipped_y = unclipped_y;
		return;
	}
	*p_start_y = start_y;
	*p_unclipped_y = unclipped_y;
}

void clip_rect_to_window(uint32_t *p_left, uint32_t *p_top, uint32_t *p_right, uint32_t *p_bottom, uint32_t *p_unclipped_edges)
{
	uint16_t left = (uint16_t)*p_left;
	uint16_t top = (uint16_t)*p_top;
	uint16_t right = (uint16_t)*p_right;
	uint16_t bottom = (uint16_t)*p_bottom;
	uint16_t window_max = 0, window_min = 0;
	uint32_t unclipped_edges = 0;
	uint16_t old_left = 0, old_top = 0;

	if ((int16_t)right < (int16_t)left)
	{
		old_left = left;
		left = (uint16_t)right;
		right = old_left;
	}
	if ((int16_t)bottom < (int16_t)top)
	{
		old_top = top;
		top = (uint16_t)bottom;
		bottom = old_top;
	}
	unclipped_edges = 0xFu;
	window_min = (uint16_t)bitmap_window_left;
	window_max = (uint16_t)bitmap_window_right;
	if (window_max < (uint16_t)left || (int16_t)right < (int16_t)window_min)
	{
		unclipped_edges = 0u;
		*p_left = (*p_left & 0xFFFF0000u) | left;
		*p_top = (*p_top & 0xFFFF0000u) | top;
		*p_right = (*p_right & 0xFFFF0000u) | right;
		*p_bottom = (*p_bottom & 0xFFFF0000u) | bottom;
		*p_unclipped_edges = unclipped_edges;
		return;
	}
	if ((int16_t)left < (int16_t)window_min)
	{
		left = (uint16_t)window_min;
		unclipped_edges = 0xEu;
	}
	if ((int16_t)window_max < (int16_t)right)
	{
		right = (uint16_t)window_max;
		unclipped_edges = unclipped_edges & 0xDu;
	}
	window_min = (uint16_t)bitmap_window_top;
	window_max = (uint16_t)bitmap_window_bottom;
	if (window_max < (uint16_t)top || (int16_t)bottom < (int16_t)window_min)
	{
		unclipped_edges = 0u;
		*p_left = (*p_left & 0xFFFF0000u) | left;
		*p_top = (*p_top & 0xFFFF0000u) | top;
		*p_right = (*p_right & 0xFFFF0000u) | right;
		*p_bottom = (*p_bottom & 0xFFFF0000u) | bottom;
		*p_unclipped_edges = unclipped_edges;
		return;
	}
	if ((int16_t)top < (int16_t)window_min)
	{
		top = (uint16_t)window_min;
		unclipped_edges = unclipped_edges & 0xBu;
	}
	if ((int16_t)bottom <= (int16_t)window_max)
	{
		*p_left = (*p_left & 0xFFFF0000u) | left;
		*p_top = (*p_top & 0xFFFF0000u) | top;
		*p_right = (*p_right & 0xFFFF0000u) | right;
		*p_bottom = (*p_bottom & 0xFFFF0000u) | bottom;
		*p_unclipped_edges = unclipped_edges;
		return;
	}
	bottom = (uint16_t)window_max;
	unclipped_edges = unclipped_edges & 7u;
	*p_left = (*p_left & 0xFFFF0000u) | left;
	*p_top = (*p_top & 0xFFFF0000u) | top;
	*p_right = (*p_right & 0xFFFF0000u) | right;
	*p_bottom = (*p_bottom & 0xFFFF0000u) | bottom;
	*p_unclipped_edges = unclipped_edges;
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

void eeprom_lock(void)
{
	uint8_t command = 0;
	uint32_t ignored = 0, lines = 0, port_d = 0;

	command = 0u;
	eeprom_select(&lines, &ignored, &port_d);
	lines = eeprom_send_command(lines, command, port_d);
	eeprom_deselect(lines, port_d);
	interrupts_on();
}

void load_settings(uint32_t crc)
{
	uint32_t ignored = 0;
	uint8_t port_f_bits = 0;

	eeprom_read_all(crc);
	if (eeprom_magic == 0x53333241u)
	{
		if (eeprom_rom_stamp != (uint16_t)(rom_checksums + rom_checksum_ic8))
		{
			move_halfwords(setting_advertise_sound_ADDRESS, default_settings_ADDRESS, 0x19u);
			port_f_bits = mem_read8(io_port_f);
			mem_write8(moving_seat_dip, port_f_bits & 1u);
			mem_write16(bookkeeping_rom_stamp, 0u);
			eeprom_rom_stamp = rom_checksums + rom_checksum_ic8;
			compute_eeprom_checksum(&crc, &ignored);
			eeprom_checksum = crc;
			eeprom_save(&ignored, &ignored);
			return;
		}
		compute_eeprom_checksum(&crc, &ignored);
		if ((uint16_t)crc == eeprom_checksum)
		{
			crc = compute_eeprom_backup_checksum();
			if ((uint16_t)crc != eeprom_backup_checksum || (uint16_t)crc != eeprom_checksum)
			{
				eeprom_save(&ignored, &ignored);
				return;
			}
			return;
		}
		crc = compute_eeprom_backup_checksum();
		if ((uint16_t)crc == eeprom_backup_checksum)
		{
			move_halfwords(eeprom_magic_ADDRESS, eeprom_backup_ADDRESS, 0x20u);
			eeprom_save(&ignored, &ignored);
			return;
		}
		move_halfwords(setting_advertise_sound_ADDRESS, default_settings_ADDRESS, 0x19u);
		port_f_bits = mem_read8(io_port_f);
		mem_write8(moving_seat_dip, port_f_bits & 1u);
		mem_write16(bookkeeping_rom_stamp, 0u);
		eeprom_rom_stamp = rom_checksums + rom_checksum_ic8;
		compute_eeprom_checksum(&crc, &ignored);
		eeprom_checksum = crc;
		eeprom_save(&ignored, &ignored);
		return;
	}
	if (eeprom_magic != 0x41474553u)
	{
		move_halfwords(eeprom_magic_ADDRESS, eeprom_defaults_ADDRESS, 0x20u);
		port_f_bits = mem_read8(io_port_f);
		mem_write8(moving_seat_dip, port_f_bits & 1u);
		mem_write16(bookkeeping_rom_stamp, 0u);
		eeprom_rom_stamp = rom_checksums + rom_checksum_ic8;
		compute_eeprom_checksum(&crc, &ignored);
		eeprom_checksum = crc;
		eeprom_save(&ignored, &ignored);
		return;
	}
	move_halfwords(eeprom_magic_ADDRESS, eeprom_defaults_ADDRESS, 0x20u);
	eeprom_write_count = 0x64u;
	port_f_bits = mem_read8(io_port_f);
	mem_write8(moving_seat_dip, port_f_bits & 1u);
	mem_write16(bookkeeping_rom_stamp, 0u);
	eeprom_rom_stamp = rom_checksums + rom_checksum_ic8;
	compute_eeprom_checksum(&crc, &ignored);
	eeprom_checksum = crc;
	eeprom_save(&ignored, &ignored);
}

void eeprom_read_all(uint32_t data)
{
	uint8_t command = 0;
	uint32_t eeprom_words_left = 0, lines = 0, port_d = 0, port_f = 0, word_dest = 0;

	word_dest = eeprom_magic_ADDRESS;
	command = 0x80u;
	eeprom_words_left = 0x40u;
	eeprom_select(&lines, &port_f, &port_d);
	do
	{
		eeprom_read_word(command, port_f, port_d, &data, &lines);
		command++;
		mem_write16(word_dest, data);
		eeprom_words_left--;
		word_dest += 2u;
	} while (eeprom_words_left != 0u);
}

void eeprom_save_and_lock(uint32_t *p_command, uint32_t *p_words_left)
{
	uint32_t command = 0, words_left = 0;

	eeprom_save(&command, &words_left);
	eeprom_lock();
	*p_command = command;
	*p_words_left = words_left;
}

void eeprom_save(uint32_t *p_command, uint32_t *p_words_left)
{
	uint16_t data = 0;
	uint32_t command = 0, crc = 0, ignored = 0, lines = 0, polynomial = 0, port_d = 0, port_f = 0, word_source = 0, words_left = 0;

	eeprom_write_count++;
	compute_eeprom_checksum(&crc, &polynomial);
	eeprom_checksum = crc;
	move_halfwords(eeprom_backup_ADDRESS, eeprom_magic_ADDRESS, 0x20u);
	command = (polynomial & ~0xFFu) | 0x30u;
	eeprom_select(&lines, &ignored, &port_d);
	eeprom_send_command(lines, (uint8_t)command, port_d);
	word_source = eeprom_magic_ADDRESS;
	command = (command & ~0xFFu) | 0x40u;
	words_left = 0x40u;
	eeprom_select(&lines, &port_f, &port_d);
	do
	{
		data = mem_read16(word_source);
		word_source += 2u;
		lines = eeprom_write_word(data, lines, (uint8_t)command, port_f, port_d);
		command = (command & ~0xFFu) | (uint8_t)(command + 1u);
		words_left--;
	} while (words_left != 0u);
	*p_command = command;
	*p_words_left = words_left;
}

void eeprom_read_word(uint8_t command, uint32_t port_f, uint32_t port_d, uint32_t *p_data, uint32_t *p_lines)
{
	uint32_t data = *p_data;
	uint32_t lines = *p_lines;
	uint32_t bits_left = 0;

	lines = eeprom_send_command(lines, command, port_d);
	lines = (lines & 0x3Fu) | 0x20u;
	bits_left = 0x12u;
	do
	{
		data = eeprom_set_lines(data, (uint8_t)lines, port_d);
		data = ((data & ~0xFFu) | mem_read8(port_f)) * 2u;
		lines = lines ^ 0x40u;
		data = eeprom_set_lines(data, (uint8_t)lines, port_d);
		lines = lines ^ 0x40u;
		bits_left--;
	} while (bits_left != 0u);
	lines = eeprom_deselect(lines, port_d);
	data = data >> 9;
	*p_data = data;
	*p_lines = (*p_lines & 0xFFFFFF00u) | (lines & 0xFFu);
}

uint32_t eeprom_write_word(uint32_t data, uint32_t lines, uint8_t command, uint32_t port_f, uint32_t port_d)
{
	uint16_t data_bits = 0;
	uint32_t clocks_left = 0;

	data_bits = (uint16_t)data;
	lines = eeprom_send_command(lines, command, port_d);
	data_bits = (uint16_t)(((uint32_t)data_bits << 8) | (data_bits >> 8));
	clocks_left = 0x10u;
	do
	{
		data = (data & ~0xFFu) | (uint8_t)data_bits;
		data = (data & ~0xFFu) | (data & 0x80u);
		data_bits = (uint16_t)(((uint32_t)data_bits << 1) | (data_bits >> 15));
		lines = ((lines & ~0xFFu) | (lines & 0x1Fu)) + 0x20u + data;
		data = eeprom_set_lines(data, (uint8_t)lines, port_d);
		lines = (lines & ~0xFFu) | (uint8_t)(lines ^ 0x40u);
		data = eeprom_set_lines(data, (uint8_t)lines, port_d);
		clocks_left--;
	} while (clocks_left != 0u);
	clocks_left = 2u;
	do
	{
		lines = (lines & ~0xFFu) | (lines & 0x1Fu);
		data = eeprom_set_lines(data, (uint8_t)lines, port_d);
		lines = (lines & ~0xFFu) | (uint8_t)(lines ^ 0x40u);
		data = eeprom_set_lines(data, (uint8_t)lines, port_d);
		clocks_left--;
	} while (clocks_left != 0u);
	lines = (lines & ~0xFFu) | (lines & 0x1Fu);
	do
	{
		lines = (lines & ~0xFFu) | (uint8_t)(lines | 0x20u);
		eeprom_set_lines(data, (uint8_t)lines, port_d);
		data = mem_read8(port_f);
		lines = (lines & ~0xFFu) | (uint8_t)(lines ^ 0x40u);
		data = eeprom_set_lines(data, (uint8_t)lines, port_d);
	} while ((int8_t)data >= 0);
	lines = eeprom_deselect(lines, port_d);
	return lines;
}

uint32_t eeprom_send_command(uint32_t lines, uint8_t command, uint32_t port_d)
{
	uint8_t command_bits = 0;
	uint32_t bits_left = 0, data = 0, data_bit = 0;

	command_bits = (uint8_t)command;
	lines = (lines & ~0xFFu) | (lines & 0x1Fu);
	data = eeprom_set_lines(0u, (uint8_t)lines, port_d);
	lines = (lines & ~0xFFu) | (uint8_t)(lines ^ 0x40u);
	data = eeprom_set_lines(data, (uint8_t)lines, port_d);
	lines = (lines & ~0xFFu) | (uint8_t)(lines ^ 0x60u);
	data = eeprom_set_lines(data, (uint8_t)lines, port_d);
	lines = (lines & ~0xFFu) | (uint8_t)(lines ^ 0x40u);
	data = eeprom_set_lines(data, (uint8_t)lines, port_d);
	lines = (lines & ~0xFFu) | (uint8_t)(lines ^ 0xC0u);
	data = eeprom_set_lines(data, (uint8_t)lines, port_d);
	lines = (lines & ~0xFFu) | (uint8_t)(lines ^ 0x40u);
	eeprom_set_lines(data, (uint8_t)lines, port_d);
	bits_left = 8u;
	do
	{
		data_bit = command_bits;
		command_bits = (uint8_t)(command_bits * 2u);
		data_bit = data_bit & 0x80u;
		lines = (lines & ~0xFFu) | (lines & 0x3Fu);
		lines = (lines & ~0xFFu) | (uint8_t)(lines | data_bit);
		data_bit = eeprom_set_lines(data_bit, (uint8_t)lines, port_d);
		lines = (lines & ~0xFFu) | (uint8_t)(lines ^ 0x40u);
		eeprom_set_lines(data_bit, (uint8_t)lines, port_d);
		bits_left--;
	} while (bits_left != 0u);
	return lines;
}

void eeprom_select(uint32_t *p_lines, uint32_t *p_port_f, uint32_t *p_port_d)
{
	uint8_t lines = 0;
	uint32_t port_d = 0, port_f = 0;

	port_d = io_port_d;
	port_f = io_port_f;
	lines = mem_read8(io_port_d);
	interrupts_off();
	*p_lines = (*p_lines & 0xFFFFFF00u) | lines;
	*p_port_f = port_f;
	*p_port_d = port_d;
}

uint32_t compute_eeprom_backup_checksum(void)
{
	uint32_t crc = 0, ignored = 0;

	current_object = 0x20F64Au;
	compute_eeprom_crc(&crc, &ignored);
	return crc;
}

void compute_eeprom_checksum(uint32_t *p_crc, uint32_t *p_polynomial)
{
	uint32_t r4 = 0, crc = 0;

	current_object = eeprom_write_count_ADDRESS;
	compute_eeprom_crc(&crc, &r4);
	*p_crc = crc;
	*p_polynomial = r4;
}

void compute_eeprom_crc(uint32_t *p_crc, uint32_t *p_polynomial)
{
	uint32_t carry = 0;
	uint32_t bits_left = 0, crc = 0, polynomial = 0;

	free_objects = 0x36u;
	polynomial = 0x10210000u;
	crc = 0xDEBDEB00u;
	do
	{
		crc = (crc & ~0xFFu) | mem_read8(current_object);
		bits_left = 8u;
		current_object = current_object + 1u;
		do
		{
			carry = crc * 2u < crc;
			crc = crc * 2u;
			if (carry)
				crc ^= polynomial;
			bits_left--;
		} while (bits_left != 0u);
		free_objects = free_objects - 1u;
	} while (free_objects != 0u);
	bits_left = 8u;
	do
	{
		carry = crc * 2u < crc;
		crc = crc * 2u;
		if (carry)
			crc ^= polynomial;
		bits_left--;
	} while (bits_left != 0u);
	crc = crc >> 16;
	*p_crc = crc;
	*p_polynomial = polynomial;
}

uint32_t eeprom_deselect(uint32_t lines, uint32_t port_d)
{
	uint32_t data = 0;

	lines = (lines & ~0xFFu) | (lines & 0x1Fu);
	data = eeprom_set_lines(0u, (uint8_t)lines, port_d);
	return lines;
}

uint32_t eeprom_set_lines(uint32_t data, uint8_t lines, uint32_t port_d)
{
	mem_write8(port_d, lines);
	data = data;
	return data;
}

void start_test_mode(uint32_t *p_color, uint32_t *p_blocks_per_row)
{
	uint32_t color = *p_color;
	uint32_t blocks_per_row = *p_blocks_per_row;
	uint8_t is_8bpp = 0, mode = 0, sound_id = 0;
	uint16_t scroll_x = 0;
	uint32_t r0 = 0, r5 = 0, r8 = 0, r10 = 0, ignored = 0, scroll_y = 0;
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
	clear_bitmap_layer((uint8_t)r0, &scroll_y, &color, &r5, &r8, &blocks_per_row, &r10);
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

void run_test_mode(uint32_t r1, uint16_t r8, uint32_t *p_r4, uint32_t *p_r9)
{
	uint32_t r4 = *p_r4;
	uint32_t r9 = *p_r9;

	switch (test_screen)
	{
	case 0:
		test_menu();
		break;
	case 1:
		toggle_test_continue_mode();
		break;
	case 2:
		test_memory(&r4, &r9);
		break;
	case 3:
		test_inputs(r8, &r4, &r9);
		break;
	case 4:
		test_sound();
		break;
	case 5:
		test_crt(r1, &r4, &r9);
		break;
	case 6:
		test_game_assignments(&r4, &r9);
		break;
	case 7:
		r4 = test_coin_assignments(r4);
		break;
	case 8:
		test_bookkeeping();
		break;
	case 9:
		r4 = test_backup_clear(r4);
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
	*p_r4 = r4;
	*p_r9 = r9;
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

void test_memory(uint32_t *p_results_left, uint32_t *p_r9)
{
	uint32_t results_left = *p_results_left;
	uint32_t r9 = *p_r9;
	uint8_t is_8bpp = 0, mode = 0;
	uint16_t scroll_x = 0, scroll_y = 0;
	uint32_t r0 = 0, r4 = 0, r5 = 0, r8 = 0, r10 = 0, r11 = 0, delay_count = 0, ignored = 0, offset_entry = 0, result_index = 0, text = 0;
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
		clear_bitmap_layer((uint8_t)r0, &ignored, &r4, &r5, &r8, &r9, &r10);
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
			r9 = memory_test_result_texts_ADDRESS;
			text = ROM16(offset_entry) + memory_test_result_texts_ADDRESS;
			print_text_list(&text, &r11);
			result_index = (uint8_t)(result_index + 1u);
			results_left--;
		} while (results_left != 0u);
	}
	if ((0x2002u & inputs_pressed) == 0u)
	{
		*p_results_left = results_left;
		*p_r9 = r9;
		return;
	}
	if (test_continue_mode != 0u)
	{
		test_screen++;
		test_screen_ready = 0u;
		*p_results_left = results_left;
		*p_r9 = r9;
		return;
	}
	test_cursor = test_menu_item;
	test_screen = 0u;
	test_screen_ready = 1u;
	draw_test_menu();
	*p_results_left = results_left;
	*p_r9 = r9;
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

void test_inputs(uint16_t r8, uint32_t *p_panel_color, uint32_t *p_r9)
{
	uint32_t panel_color = *p_panel_color;
	uint32_t r9 = *p_r9;
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
		move_input_test_cursors(r8, coin_text, &r4, &r9);
		if ((input_ports_active & 0x2002u) != 0u && input_test_switch_released == 0u)
		{
			*p_panel_color = r4;
			*p_r9 = r9;
			return;
		}
		input_test_switch_released = 1u;
		if ((input_ports_active & 0x2002u) == 0u)
		{
			if (input_test_switch_pressed == 0u)
			{
				*p_panel_color = r4;
				*p_r9 = r9;
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
				*p_r9 = r9;
				return;
			}
			test_cursor = test_menu_item;
			test_screen = 0u;
			test_screen_ready = 1u;
			draw_test_menu();
			*p_panel_color = r4;
			*p_r9 = r9;
			return;
		}
		input_test_switch_pressed = 1u;
		*p_panel_color = r4;
		*p_r9 = r9;
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
	fill_rect(top, right, bottom, &left, &r4, &r9);
	left = (left & ~0xFFFFu) | 0xA0u;
	top = 0x68u;
	right = 0x138u;
	bottom = 0x98u;
	panel_color = (r4 & ~0xFFFFu) | 0xEu;
	if ((mem_read32(video_control) & 0x200u) != 0u)
		flip_test_coordinates(&left, &top, &right, &bottom);
	fill_rect(top, right, bottom, &left, &panel_color, &r9);
	if (setting_cabinet_type != 0u)
	{
		left = (left & ~0xFFFFu) | 0xA0u;
		top = 0xA0u;
		right = 0x138u;
		bottom = 0xD0u;
		panel_color = (panel_color & ~0xFFFFu) | 6u;
		if ((mem_read32(video_control) & 0x200u) != 0u)
			flip_test_coordinates(&left, &top, &right, &bottom);
		fill_rect(top, right, bottom, &left, &panel_color, &r9);
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
	*p_r9 = r9;
}

void move_input_test_cursors(uint32_t r8, uint32_t r10, uint32_t *p_r4, uint32_t *p_r9)
{
	uint32_t r9 = *p_r9;
	uint16_t trackball_dx = 0, trackball_dy = 0;
	uint32_t r4 = 0, bottom_limit = 0, cursors_left = 0, ignored = 0, quadrant_bit = 0, sprite_entry = 0, text_position = 0, top_limit = 0;

	current_object = (current_object & ~0xFFFFu) | screen_sprite_block;
	queue_sprite_in_layer();
	current_object = current_object + 0x10u;
	queue_sprite_in_layer();
	if (trackball_1_quadrants == 0xFu)
	{
		r10 = input_test_trackball_1_good_text_ADDRESS;
		print_text_list(&r10, &text_position);
	}
	if (trackball_2_quadrants == 0xFu)
	{
		r10 = input_test_trackball_2_good_text_ADDRESS;
		print_text_list(&r10, &text_position);
	}
	if (setting_cabinet_type != 0u)
	{
		current_object = current_object + 0x10u;
		queue_sprite_in_layer();
		if (trackball_3_quadrants == 0xFu)
		{
			r10 = input_test_trackball_3_good_text_ADDRESS;
			print_text_list(&r10, &text_position);
		}
	}
	build_sprite_list(&r4, &r8, &r9, &r10, &sprite_entry);
	draw_queued_strings((uint16_t)r8, (uint16_t)r10, sprite_entry, &r4, &ignored, &r9);
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
	*p_r4 = cursors_left;
	*p_r9 = r9;
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

void test_crt(uint32_t r1, uint32_t *p_color, uint32_t *p_blocks_per_row)
{
	uint32_t color = *p_color;
	uint32_t blocks_per_row = *p_blocks_per_row;
	uint16_t end_x = 0, end_y = 0;
	uint32_t r11 = 0, bar_record = 0, bars_left = 0, bottom = 0, ignored = 0, line_record = 0, lines_left = 0, right = 0, rows_left = 0, start_x = 0, text = 0, top = 0;

	if (test_screen_ready == 0u)
	{
		test_screen_ready = 1u;
		clear_text_layer(&ignored, &r1, &r11);
		text = crt_test_1_text_ADDRESS;
		print_text_list(&text, &r11);
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
		print_text_list(&text, &r11);
		line_record = crt_test_border_lines_ADDRESS;
		lines_left = 4u;
		do
		{
			start_x = (start_x & ~0xFFFFu) | ROM16(line_record);
			line_record += 2u;
			r1 = (r1 & ~0xFFFFu) | ROM16(line_record);
			line_record += 2u;
			end_x = ROM16(line_record);
			line_record += 2u;
			end_y = ROM16(line_record);
			color = (color & ~0xFFu) | 0x7Eu;
			line_record += 2u;
			draw_line(end_x, end_y, &start_x, &r1, &color, &lines_left);
			lines_left--;
		} while (lines_left != 0u);
		crt_test_x = 0u;
		crt_test_y = 0u;
		lines_left = 0x1Cu;
		do
		{
			start_x &= ~0xFFFFu;
			r1 = (r1 & ~0xFFFFu) | crt_test_x;
			end_x = 0x13Fu;
			end_y = crt_test_x;
			color = (color & ~0xFFFFu) | 0x7Eu;
			draw_line(end_x, end_y, &start_x, &r1, &color, &lines_left);
			crt_test_x += 8u;
			lines_left--;
		} while (lines_left != 0u);
		lines_left = 0x28u;
		do
		{
			start_x = (start_x & ~0xFFFFu) | crt_test_y;
			r1 &= ~0xFFFFu;
			end_x = crt_test_y;
			end_y = 0xDFu;
			color = (color & ~0xFFFFu) | 0x7Eu;
			draw_line(end_x, end_y, &start_x, &r1, &color, &lines_left);
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
		unknown_code(0x9D8CCu + test_screen_ready * 3u);
	}
}

void test_game_assignments(uint32_t *p_r4, uint32_t *p_blocks_per_row)
{
	uint32_t r4 = *p_r4;
	uint32_t blocks_per_row = *p_blocks_per_row;
	uint8_t mode = 0;
	uint32_t cabinet_type = 0, cursor_positions = 0, ignored = 0, print_position = 0, text = 0;

	if (test_screen_ready == 0u)
	{
		test_settings_changed = 0u;
		test_screen_ready++;
		clear_text_layer(&ignored, &ignored, &print_position);
		text = game_assignments_text_ADDRESS;
		print_text_list(&text, &print_position);
		test_cursor_positions = game_assignments_cursor_positions_ADDRESS;
		test_cursor = 7u;
		draw_difficulty_setting(&r4, &blocks_per_row);
		draw_vitality_setting(&r4, &blocks_per_row);
		text = advertise_sound_off_text_ADDRESS;
		setting_advertise_sound &= 1u;
		if (setting_advertise_sound != 0u)
			text = advertise_sound_on_text_ADDRESS;
		print_text_list(&text, &print_position);
		text = monitor_flip_normal_text_ADDRESS;
		if ((setting_monitor_flip & 2u) != 0u)
			text = monitor_flip_reverse_text_ADDRESS;
		print_text_list(&text, &print_position);
		cabinet_type = setting_cabinet_type;
		text = cabinet_type_texts[cabinet_type & 1u];
		print_text_list(&text, &print_position);
		text = start_button_use_text_ADDRESS;
		setting_no_start_buttons &= 1u;
		if (setting_no_start_buttons != 0u)
			text = start_button_no_use_text_ADDRESS;
		print_text_list(&text, &print_position);
		if ((setting_cabinet_type & 1u) != 0u && setting_moving_seat != 0u)
		{
			setting_moving_seat = 0u;
			test_settings_changed = 1u;
		}
		text = moving_seat_no_use_text_ADDRESS;
		if (setting_moving_seat != 0u)
			text = moving_seat_use_text_ADDRESS;
		print_text_list(&text, &print_position);
		if (mem_read8(moving_seat_dip) == 0u)
		{
			text = moving_seat_erase_text_ADDRESS;
			print_text_list(&text, &print_position);
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
		*p_r4 = r4;
		*p_blocks_per_row = blocks_per_row;
		return;
	}
	switch (test_cursor)
	{
	case 0:
		test_settings_changed = 1u;
		setting_difficulty++;
		draw_difficulty_setting(&r4, &blocks_per_row);
		*p_r4 = r4;
		*p_blocks_per_row = blocks_per_row;
		return;
	case 1:
		test_settings_changed = 1u;
		setting_initial_vitality++;
		draw_vitality_setting(&r4, &blocks_per_row);
		*p_r4 = r4;
		*p_blocks_per_row = blocks_per_row;
		return;
	case 2:
		test_settings_changed = 1u;
		setting_advertise_sound++;
		setting_advertise_sound &= 1u;
		text = advertise_sound_off_text_ADDRESS;
		if (setting_advertise_sound == 0u)
		{
			print_text_list(&text, &print_position);
			*p_r4 = r4;
			*p_blocks_per_row = blocks_per_row;
			return;
		}
		text = advertise_sound_on_text_ADDRESS;
		print_text_list(&text, &print_position);
		*p_r4 = r4;
		*p_blocks_per_row = blocks_per_row;
		return;
	case 3:
		test_settings_changed = 1u;
		setting_monitor_flip += 2u;
		setting_monitor_flip &= 2u;
		text = monitor_flip_normal_text_ADDRESS;
		if (setting_monitor_flip != 0u)
			text = monitor_flip_reverse_text_ADDRESS;
		print_text_list(&text, &print_position);
		mem_write32(video_control, mem_read32(video_control) | 0x800u);
		fill_value = (fill_value & ~0xFFFFu) | 0x909u;
		set_halfwords(VIDEO_RAM, 0xE000u, fill_value);
		mode = 0u;
		set_screen_mode(mode);
		draw_difficulty_setting(&r4, &blocks_per_row);
		draw_vitality_setting(&r4, &blocks_per_row);
		*p_r4 = r4;
		*p_blocks_per_row = blocks_per_row;
		return;
	case 4:
		test_settings_changed = 1u;
		setting_cabinet_type++;
		setting_cabinet_type &= 1u;
		text = cabinet_type_texts[setting_cabinet_type];
		print_text_list(&text, &print_position);
		if (mem_read8(moving_seat_dip) == 0u || setting_cabinet_type == 0u)
		{
			*p_r4 = r4;
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
			print_text_list(&text, &print_position);
			*p_r4 = r4;
			*p_blocks_per_row = blocks_per_row;
			return;
		}
		text = moving_seat_use_text_ADDRESS;
		print_text_list(&text, &print_position);
		*p_r4 = r4;
		*p_blocks_per_row = blocks_per_row;
		return;
	case 5:
		test_settings_changed = 1u;
		setting_no_start_buttons++;
		setting_no_start_buttons &= 1u;
		text = start_button_use_text_ADDRESS;
		if (setting_no_start_buttons == 0u)
		{
			print_text_list(&text, &print_position);
			*p_r4 = r4;
			*p_blocks_per_row = blocks_per_row;
			return;
		}
		text = start_button_no_use_text_ADDRESS;
		print_text_list(&text, &print_position);
		*p_r4 = r4;
		*p_blocks_per_row = blocks_per_row;
		return;
	case 6:
		if (setting_cabinet_type != 0u)
		{
			*p_r4 = r4;
			*p_blocks_per_row = blocks_per_row;
			return;
		}
		test_settings_changed = 1u;
		setting_moving_seat++;
		setting_moving_seat &= 1u;
		text = moving_seat_no_use_text_ADDRESS;
		if (setting_moving_seat == 0u)
		{
			print_text_list(&text, &print_position);
			*p_r4 = r4;
			*p_blocks_per_row = blocks_per_row;
			return;
		}
		text = moving_seat_use_text_ADDRESS;
		print_text_list(&text, &print_position);
		*p_r4 = r4;
		*p_blocks_per_row = blocks_per_row;
		return;
	case 7:
		setting_cabinet_config = setting_cabinet_type | ((uint32_t)setting_coin_chute_type << 1);
		if (test_settings_changed != 0u)
			eeprom_save_and_lock(&r4, &ignored);
		mem_write32(video_control, mem_read32(video_control) | 0x800u);
		fill_value = (fill_value & ~0xFFFFu) | 0x909u;
		set_halfwords(VIDEO_RAM, 0xE000u, fill_value);
		if (test_continue_mode != 0u)
		{
			test_screen++;
			test_screen_ready = 0u;
			*p_r4 = r4;
			*p_blocks_per_row = blocks_per_row;
			return;
		}
		test_cursor = test_menu_item;
		test_screen = 0u;
		test_screen_ready = 1u;
		draw_test_menu();
		*p_r4 = r4;
		*p_blocks_per_row = blocks_per_row;
		return;
	case 8:
		test_settings_changed = 1u;
		setting_small_large++;
		setting_small_large &= 1u;
		text = small_large_texts[setting_small_large];
		print_text_list(&text, &print_position);
		*p_r4 = r4;
		*p_blocks_per_row = blocks_per_row;
		return;
	default:
		unknown_code(0x9DBDBu + test_cursor * 3u);
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

uint32_t test_coin_assignments(uint32_t r4)
{
	uint8_t mode = 0;
	uint16_t attribute = 0;
	uint32_t r4_2 = 0, r11 = 0, adder_setting = 0, credits_to_start = 0, cursor_positions = 0, ignored = 0, print_position = 0, setting = 0, setting_number = 0, text = 0, text_position = 0, value = 0;

	if (test_screen_ready == 0u)
	{
		test_screen_ready++;
		test_settings_changed = 0u;
		test_cursor = 4u;
		goto test_coin_assignments_draw_main_page;
	}
	if (coin_manual_page != 0u)
	{
	test_coin_assignments_manual_page:
		if (test_screen_ready == 0u)
		{
			test_screen_ready++;
			clear_text_layer(&ignored, &ignored, &r11);
			test_cursor_positions = coin_manual_cursor_positions_ADDRESS;
			test_cursor = 4u;
			text = coin_manual_page_common_text_ADDRESS;
			if (setting_coin_chute_type != 0u)
				text = coin_manual_page_text_ADDRESS;
			print_text_list(&text, &r11);
			text = coins_per_credit_texts[setting_coins_per_credit];
			print_text_list(&text, &r11);
			text = bonus_adder_texts[setting_bonus_adder];
			print_text_list(&text, &r11);
			value = setting_chute_1_multiplier;
			text_position = TEXT_CELL(30, 12);
			attribute = 0x1200u;
			draw_hex_2(value, attribute, text_position);
			text = chute_1_coin_text_ADDRESS;
			if (setting_chute_1_multiplier != 1u)
				text = chute_1_coins_text_ADDRESS;
			print_text_list(&text, &text_position);
			r4_2 = draw_coin_credit_table();
			if (setting_coin_chute_type != 0u)
				return r4_2;
			value = setting_chute_2_multiplier;
			text_position = TEXT_CELL(30, 18);
			attribute = 0x1200u;
			draw_hex_2(value, attribute, text_position);
			text = chute_2_coin_text_ADDRESS;
			if (setting_chute_2_multiplier != 1u)
				text = chute_2_coins_text_ADDRESS;
			print_text_list(&text, &text_position);
			return r4_2;
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
			return r4;
		switch (test_cursor)
		{
		case 0:
		case 5:
			setting_coin_number = -1;
			setting_coins_per_credit++;
			if ((int8_t)setting_coins_per_credit > 9)
				setting_coins_per_credit = 1u;
			text = coins_per_credit_texts[setting_coins_per_credit];
			print_text_list(&text, &print_position);
			text = coin_credit_rows_erase_text_ADDRESS;
			print_text_list(&text, &print_position);
			r4_2 = draw_coin_credit_table();
			return r4_2;
		case 1:
			setting_coin_number = -1;
			setting_bonus_adder++;
			if ((int8_t)setting_bonus_adder > 9)
				setting_bonus_adder = 0u;
			if (setting_bonus_adder == 1u)
				setting_bonus_adder = 2u;
			text = bonus_adder_texts[setting_bonus_adder];
			print_text_list(&text, &print_position);
			text = coin_credit_rows_erase_text_ADDRESS;
			print_text_list(&text, &print_position);
			r4_2 = draw_coin_credit_table();
			return r4_2;
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
			print_text_list(&text, &text_position);
			text = coin_credit_rows_erase_text_ADDRESS;
			print_text_list(&text, &text_position);
			r4_2 = draw_coin_credit_table();
			return r4_2;
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
			print_text_list(&text, &text_position);
			text = coin_credit_rows_erase_text_ADDRESS;
			print_text_list(&text, &text_position);
			r4_2 = draw_coin_credit_table();
			return r4_2;
		case 4:
			test_cursor = 3u;
			mode = 0u;
			set_screen_mode(mode);
		test_coin_assignments_draw_main_page:
			clear_text_layer(&ignored, &ignored, &print_position);
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
			if (setting_coin_chute_type != 0u)
			{
				text = coin_chute_type_individual_text_ADDRESS;
				print_text_list(&text, &print_position);
				text = coin_setting_texts_individual[(uint8_t)setting_coin_number_individual];
				print_text_list(&text, &print_position);
				text = coin_assignments_text_ADDRESS;
				print_text_list(&text, &print_position);
				setting_number = individual_coin_setting_numbers[(uint8_t)setting_coin_number_individual];
				goto test_coin_assignments_draw_setting_number;
			}
			text = coin_chute_type_common_text_ADDRESS;
			print_text_list(&text, &print_position);
			text = coin_assignments_common_text_ADDRESS;
			print_text_list(&text, &print_position);
			if (setting_coin_number >= 0)
			{
				text = coin_setting_texts[(uint8_t)setting_coin_number];
				print_text_list(&text, &print_position);
				setting_number = (uint8_t)setting_coin_number;
				setting_number = (uint8_t)(setting_number + 1u);
			test_coin_assignments_draw_setting_number:
				convert_to_bcd(&setting_number, &ignored);
				print_position = TEXT_CELL(31, 9);
				attribute = 0x1200u;
				draw_hex_2(setting_number, attribute, print_position);
			}
			credits_to_start = setting_credits_to_start;
			text = credits_to_start_texts[credits_to_start >> 1];
			print_text_list(&text, &print_position);
			if (setting_coin_number >= 0)
				return r4;
			setting_coin_number_individual = -1;
			text = coin_setting_erase_text_ADDRESS;
			print_text_list(&text, &print_position);
			text = coin_setting_number_manual_text_ADDRESS;
			print_text_list(&text, &print_position);
			text = coin_setting_text_manual_ADDRESS;
			if (setting_coin_chute_type != 0u)
				text = coin_setting_text_manual_individual_ADDRESS;
			print_text_list(&text, &print_position);
			return r4;
		default:
			unknown_code(0x9E479u + test_cursor * 3u);
		}
	}
	else
	{
		cursor_positions = coin_assignments_cursor_positions_ADDRESS;
		erase_test_cursor(cursor_positions);
		if ((0x1001u & inputs_pressed) != 0u)
		{
			test_cursor++;
			set_test_cursor();
		}
		blink_test_cursor();
		if ((0x2002u & inputs_pressed) == 0u)
			return r4;
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
			print_text_list(&text, &print_position);
			if (setting_coin_number < 0)
			{
				text = coin_setting_erase_text_ADDRESS;
				print_text_list(&text, &print_position);
				text = coin_chute_2_label_erase_text_ADDRESS;
				print_text_list(&text, &print_position);
				text = coin_setting_number_manual_text_ADDRESS;
				print_text_list(&text, &print_position);
				if (setting_coin_chute_type == 0u)
				{
					text = coin_setting_text_manual_ADDRESS;
					print_text_list(&text, &print_position);
					text = coin_chute_labels_text_ADDRESS;
					print_text_list(&text, &print_position);
				}
				else
				{
					text = coin_setting_text_manual_individual_ADDRESS;
					print_text_list(&text, &print_position);
				}
				return r4;
			}
			text = coin_setting_erase_text_ADDRESS;
			print_text_list(&text, &print_position);
			text = coin_chute_2_label_erase_text_ADDRESS;
			print_text_list(&text, &print_position);
			value = 1u;
			text_position = TEXT_CELL(31, 9);
			attribute = 0x1200u;
			draw_hex_2(value, attribute, text_position);
			if (setting_coin_chute_type == 0u)
			{
				text = coin_chute_labels_text_ADDRESS;
				print_text_list(&text, &text_position);
				text = coin_setting_text_1_ADDRESS;
				print_text_list(&text, &text_position);
				setting_coin_number = 0;
			}
			else
			{
				text = coin_setting_text_1_individual_ADDRESS;
				print_text_list(&text, &text_position);
				setting_coin_number_individual = 0;
			}
			apply_coin_setting();
			return r4;
		case 1:
			test_settings_changed = 1u;
			setting_credits_to_start++;
			if (setting_credits_to_start == 3u)
				setting_credits_to_start = 1u;
			credits_to_start = setting_credits_to_start;
			text = credits_to_start_texts[credits_to_start >> 1];
			print_text_list(&text, &print_position);
			return r4;
		case 2:
			setting_coin_number++;
			setting_coin_number_individual++;
			apply_coin_setting();
			return r4;
		case 3:
			test_settings_changed = 1u;
			coin_manual_page++;
			test_screen_ready = 0u;
			mode = 1u;
			set_screen_mode(mode);
			mem_write16(video_control, mem_read16(video_control) | 0x8000u);
			goto test_coin_assignments_manual_page;
		case 4:
			setting_cabinet_config = setting_cabinet_type | ((uint32_t)setting_coin_chute_type << 1);
			if (test_settings_changed != 0u)
				eeprom_save_and_lock(&r4, &ignored);
			if (test_continue_mode != 0u)
			{
				test_screen++;
				test_screen_ready = 0u;
				return r4;
			}
			test_cursor = test_menu_item;
			test_screen = 0u;
			test_screen_ready = 1u;
			draw_test_menu();
			return r4;
		default:
			unknown_code(0x9E0A9u + test_cursor * 3u);
		}
	}
}

void apply_coin_setting(void)
{
	uint32_t coin_setting = 0, setting_number = 0, text = 0, text_position = 0, values_record = 0;

	test_settings_changed = 1u;
	text = coin_setting_number_erase_text_ADDRESS;
	print_text_list(&text, &text_position);
	text = coin_setting_erase_text_ADDRESS;
	print_text_list(&text, &text_position);
	if (setting_coin_number > 0x19)
		setting_coin_number = 0;
	if (setting_coin_number_individual > 0xE)
		setting_coin_number_individual = 0;
	if (setting_coin_chute_type != 0u)
	{
		text = coin_setting_texts_individual[(uint8_t)setting_coin_number_individual];
		print_text_list(&text, &text_position);
		setting_number = individual_coin_setting_numbers[(uint8_t)setting_coin_number_individual];
		draw_coin_setting_number(setting_number);
		coin_setting = individual_coin_setting_numbers[(uint8_t)setting_coin_number_individual];
	}
	else
	{
		text = coin_setting_texts[(uint8_t)setting_coin_number];
		print_text_list(&text, &text_position);
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
	column = mem_read8(cell_position);
	cell_position++;
	text_position = TEXT_CELL(0, 0) + (column << 1) + ((uint32_t)mem_read8(cell_position) << 7);
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
	uint32_t r11 = 0, average_hours = 0, average_minutes = 0, average_seconds = 0, chute_3_coins = 0, days = 0, frames = 0, frames_high = 0, hours = 0, ignored = 0, minutes = 0, seconds = 0, shortest_game_frames = 0, shortest_hours = 0, shortest_minutes = 0, shortest_seconds = 0, text = 0, text_position = 0, time_high = 0, total_coins = 0, value = 0;

	if (test_screen_ready == 0u)
	{
		test_screen_ready++;
		clear_text_layer(&ignored, &ignored, &r11);
		text = bookkeeping_text_ADDRESS;
		print_text_list(&text, &r11);
		test_cursor_positions = bookkeeping_cursor_positions_ADDRESS;
		test_cursor = 0u;
		current_object = bookkeeping_coin_credits;
		if (setting_coin_chute_type == 0u)
			text = bookkeeping_chute_3_erase_text_ADDRESS;
		if (setting_cabinet_type == 0u)
			text = bookkeeping_chute_3_erase_text_ADDRESS;
		print_text_list(&text, &r11);
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
	uint32_t r4 = 0, cursor_positions = 0, ignored = 0, print_position = 0, text = 0;

	if (test_screen_ready == 0u)
	{
		test_screen_ready++;
		clear_text_layer(&ignored, &ignored, &print_position);
		text = backup_clear_text_ADDRESS;
		print_text_list(&text, &print_position);
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
		print_text_list(&text, &print_position);
		reset_bookkeeping();
		r4 = reset_ranking();
		text = backup_clear_done_text_ADDRESS;
		print_text_list(&text, &print_position);
		backup_clear_timer++;
		return r4;
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
		unknown_code(0x9EABDu + test_cursor * 3u);
	}
}

void test_outputs(void)
{
	uint32_t r11 = 0, ignored = 0, print_position = 0, text = 0;
	uint8_t port_d_bits = 0, port_d_bits_1p = 0, port_d_bits_2p = 0;

	if (test_screen_ready != 0u)
	{
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
			print_text_list(&text, &print_position);
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
		print_text_list(&text, &print_position);
		text = output_test_2p_off_text_ADDRESS;
		if ((input_ports_active & 0x1000000u) != 0u)
		{
			port_d_bits_2p = mem_read8(io_port_d);
			mem_write8(io_port_d, port_d_bits_2p | 8u);
			text = output_test_2p_on_text_ADDRESS;
		}
		print_text_list(&text, &print_position);
		return;
	}
	test_screen_ready++;
	clear_text_layer(&ignored, &ignored, &r11);
	text = output_test_text_ADDRESS;
	print_text_list(&text, &r11);
	if (setting_moving_seat != 0u)
	{
		text = output_test_seat_text_ADDRESS;
		print_text_list(&text, &r11);
		return;
	}
	text = output_test_no_use_text_ADDRESS;
	print_text_list(&text, &r11);
}

void exit_test_mode(void)
{
	uint32_t r11 = 0, ignored = 0, text = 0;

	if (test_screen_ready != 0u)
	{
		if ((0x2002u & inputs_pressed) == 0u)
			return;
	}
	else if ((int16_t)AS16(eeprom_write_count) >= 0x2710)
	{
		clear_text_layer(&ignored, &ignored, &r11);
		text = eeprom_caution_text_ADDRESS;
		print_text_list(&text, &r11);
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
		return;
	}
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
	mem_write16(TEXT_CELL(0, 0) + (((uint32_t)test_cursor_row << 7) + ((uint32_t)test_cursor_column << 1)), 0x200u | mem_read8(cursor_char));
}

void erase_test_cursor(uint32_t cursor_positions)
{
	uint32_t cursor_char = 0;

	cursor_positions += (uint32_t)test_cursor << 1;
	test_cursor_column = mem_read8(cursor_positions);
	test_cursor_row = mem_read8(cursor_positions + 1u);
	cursor_char = test_cursor_space_ADDRESS;
	draw_blinking_test_cursor(cursor_char);
}

void set_test_cursor(void)
{
	uint32_t cursor_entry = 0;

	for (;;)
	{
		cursor_entry = test_cursor_positions + ((uint32_t)test_cursor << 1);
		if ((int8_t)mem_read8(cursor_entry) >= 0)
			break;
		test_cursor = 0u;
	}
	test_cursor_column = mem_read8(cursor_entry);
	test_cursor_row = mem_read8(cursor_entry + 1u);
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

void int_default(uint32_t *p_r0, uint32_t *p_r2, uint32_t *p_r3, uint32_t *p_r4)
{
	uint32_t r0 = *p_r0;
	uint32_t r2 = *p_r2;
	uint32_t r3 = 0, r4 = 0;

	halt();
	halt();
	halt();
	halt();
	int_vector_02(&r0, &r2, &r3, &r4);
	*p_r0 = r0;
	*p_r2 = r2;
	*p_r3 = r3;
	*p_r4 = r4;
}

void int_vector_02(uint32_t *p_r0, uint32_t *p_r2, uint32_t *p_r3, uint32_t *p_r4)
{
	uint32_t r0 = *p_r0;
	uint32_t r2 = *p_r2;
	uint32_t r3 = 0, r4 = 0;

	halt();
	halt();
	halt();
	halt();
	run_vblank_copies(&r0, &r2, &r3, &r4);
	*p_r0 = r0;
	*p_r2 = r2;
	*p_r3 = r3;
	*p_r4 = r4;
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

void apply_settings(void)
{
	uint16_t credit_cap = 0;
	uint32_t block_cursor = 0, credit_blocks_left = 0, credits = 0, saved_cursor = 0, vitality = 0;

	credit_coin_cost = setting_coins_per_credit;
	credits_per_start = setting_credits_to_start;
	health_setting = setting_difficulty;
	vitality = setting_initial_vitality;
	starting_health = starting_health_by_vitality[vitality & 3u];
	AS32(coin_counter_1_on) = 0u;
	chute_1_multiplier = setting_chute_1_multiplier;
	AS32(coin_counter_2_on) = 0u;
	chute_2_multiplier = setting_chute_2_multiplier;
	service_coin_sounds = 0u;
	coin_sounds = 0u;
	AS16(shared_coin_counter) = (uint8_t)((chute_2_multiplier == chute_1_multiplier) * 0xFCu);
	bonus_adder = setting_bonus_adder;
	free_play = 0u;
	if (bonus_adder == 1u)
		free_play = 1u;
	credit_cap = 0x900u;
	saved_cursor = saved_credits;
	block_cursor = p1_coins_ADDRESS;
	credit_blocks_left = 3u;
	do
	{
		credits = mem_read16(saved_cursor);
		credits = credits & ~0xFFu;
		saved_cursor += 2u;
		if ((int16_t)credit_cap < (int16_t)credits)
			credits = credit_cap;
		mem_write32(block_cursor, credits);
		block_cursor += 4u;
		credit_blocks_left--;
	} while (credit_blocks_left != 0u);
}

void update_coins(void)
{
	uint8_t changed_flag = 0, max_credits = 0, sound_id = 0;
	uint32_t block_cursor = 0, bookkeeping_chute = 0, chute_offset = 0, coin_credit_block = 0, coin_credits_added = 0, coin_inputs = 0, counter_bit = 0, counter_state = 0, credit_block = 0, credit_blocks_left = 0, ignored = 0, pending_offset = 0, service_credits_added = 0;
	uint32_t coins_released = 0;

	block_cursor = p1_coins_ADDRESS;
	changed_flag = 0u;
	credit_blocks_left = 3u;
	do
	{
		mem_write8(block_cursor + 3u, changed_flag);
		block_cursor += 4u;
		credit_blocks_left--;
	} while (credit_blocks_left != 0u);
	if (free_play != 0u)
		return;
	if (coin_sounds != 0u)
	{
		sound_id = 0x7Fu;
		queue_sound(sound_id, &ignored, &ignored);
		coin_sounds--;
	}
	if (service_coin_sounds != 0u)
	{
		sound_id = 0x7Fu;
		queue_sound(sound_id, &ignored, &ignored);
		service_coin_sounds--;
	}
	max_credits = 9u;
	service_credits_added = 0u;
	if ((inputs_pressed & 0x1001u) != 0u)
	{
		switch (cabinet_config)
		{
		case 0:
		case 1:
			break;
		case 2:
		update_coins_service_p2:
			credit_block = p2_coins_ADDRESS;
			service_credits_added = add_service_credit(service_credits_added, credit_block, max_credits);
			break;
		case 3:
		case 4:
			credit_block = p3_coins_ADDRESS;
			service_credits_added = add_service_credit(service_credits_added, credit_block, max_credits);
			goto update_coins_service_p2;
		default:
			unknown_code(0xFF7CDu + cabinet_config * 2u);
		}
		credit_block = p1_coins_ADDRESS;
		service_credits_added = add_service_credit(service_credits_added, credit_block, max_credits);
		mem_write16(bookkeeping_service_credits, mem_read16(bookkeeping_service_credits) + service_credits_added);
	}
	coin_credits_added = 0u;
	coins_released = inputs_released & 0x4Cu;
	coin_inputs = coins_released;
	if (coins_released != 0u)
	{
		coin_credit_block = p1_coins_ADDRESS;
		pending_offset = 0x7408u;
		chute_offset = 0u;
		switch (cabinet_config)
		{
		case 0:
		case 1:
		case 4:
			if ((coin_inputs & 8u) != 0u)
			{
				bookkeeping_chute = bookkeeping_coin_chute_2;
				if (shared_coin_counter == 0u)
				{
					mem_write8(cabinet_events, 0x11u);
					pending_offset = 0x740Cu;
					chute_offset = 4u;
				}
				else
					mem_write8(cabinet_events, 0x11u);
				coin_credits_added = add_coin(chute_offset, pending_offset, bookkeeping_chute, coin_credits_added, coin_credit_block, max_credits);
				pending_offset = 0x7408u;
				chute_offset = 0u;
			}
			break;
		case 2:
		update_coins_individual_chute_2:
			if ((coin_inputs & 8u) != 0u)
			{
				mem_write8(cabinet_events, 0x11u);
				bookkeeping_chute = bookkeeping_coin_chute_2;
				coin_credits_added = add_coin(chute_offset, pending_offset, bookkeeping_chute, coin_credits_added, coin_credit_block, max_credits);
			}
			coin_credit_block += 4u;
			break;
		case 3:
			if ((coin_inputs & 0x40u) != 0u)
			{
				mem_write8(cabinet_events, 0x11u);
				bookkeeping_chute = bookkeeping_coin_chute_3;
				coin_credits_added = add_coin(chute_offset, pending_offset, bookkeeping_chute, coin_credits_added, coin_credit_block, max_credits);
			}
			coin_credit_block += 4u;
			goto update_coins_individual_chute_2;
		default:
			unknown_code(0xFF821u + cabinet_config * 3u);
		}
		if ((coin_inputs & 4u) != 0u)
		{
			mem_write8(cabinet_events, 0x11u);
			bookkeeping_chute = bookkeeping_coin_chute_1;
			coin_credits_added = add_coin(chute_offset, pending_offset, bookkeeping_chute, coin_credits_added, coin_credit_block, max_credits);
		}
		mem_write16(bookkeeping_coin_credits, mem_read16(bookkeeping_coin_credits) + coin_credits_added);
	}
	if (shared_coin_counter == 0u)
	{
		counter_state = coin_counter_2_on_ADDRESS;
		counter_bit = 1u;
		pulse_coin_counter(counter_bit, counter_state, (uint16_t)service_credits_added, (uint16_t)coin_credits_added);
	}
	counter_state = coin_counter_1_on_ADDRESS;
	counter_bit = 0u;
	pulse_coin_counter(counter_bit, counter_state, (uint16_t)service_credits_added, (uint16_t)coin_credits_added);
}

void pulse_coin_counter(uint32_t counter_bit, uint32_t counter_state, uint16_t service_credits, uint16_t coin_credits)
{
	uint32_t pulse_timers = 0;
	uint8_t on_frames = 0;
	uint32_t port_word_before_on = 0, port_word_before_off = 0;

	pulse_timers = mem_read16(counter_state);
	counter_state += 2u;
	if (pulse_timers == 0u)
	{
		if (mem_read8(counter_state) != 0u)
		{
			mem_write8(counter_state, mem_read8(counter_state) - 1u);
			port_word_before_on = mem_read32(io_port_d);
			mem_write32(io_port_d, port_word_before_on | shl32(1u, (int)(counter_bit)));
			mem_write16(counter_state - 2u, 0x606u);
		}
	}
	else if ((uint8_t)pulse_timers == 0u)
	{
		counter_state--;
		mem_write8(counter_state, mem_read8(counter_state) - 1u);
	}
	else
	{
		on_frames = mem_read8(counter_state - 2u);
		mem_write8(counter_state - 2u, on_frames - 1u);
		if (on_frames == 1u)
		{
			port_word_before_off = mem_read32(io_port_d);
			mem_write32(io_port_d, port_word_before_off & ~shl32(1u, (int)(counter_bit)));
		}
	}
	if ((uint16_t)service_credits != 0u)
	{
		save_credits();
		return;
	}
	if ((uint16_t)coin_credits == 0u)
		return;
	save_credits();
}

uint32_t add_service_credit(uint32_t service_credits, uint32_t credit_block, uint8_t max_credits)
{
	if ((uint8_t)max_credits <= mem_read8(credit_block + 1u))
		return service_credits;
	mem_write8(credit_block + 3u, 1u);
	service_credits = (service_credits & ~0xFFFFu) | (uint16_t)(service_credits + 1u);
	mem_write8(credit_block + 1u, mem_read8(credit_block + 1u) + 1u);
	service_coin_sounds++;
	if ((uint8_t)max_credits <= mem_read8(credit_block + 1u))
	{
		mem_write8(credit_block + 2u, 0u);
		mem_write8(credit_block, 0u);
	}
	return service_credits;
}

uint32_t add_coin(uint32_t chute_offset, uint32_t pending_offset, uint32_t bookkeeping_chute, uint32_t coin_credits, uint32_t credit_block, uint8_t max_credits)
{
	uint8_t chute_multiplier = 0, coin_cost = 0, coin_tally = 0;
	uint32_t coins_per_bonus = 0;
	uint32_t counter_pending = 0;

	mem_write16(bookkeeping_chute, mem_read16(bookkeeping_chute) + 1u);
	counter_pending = GAME_VARIABLES + pending_offset;
	mem_write8(counter_pending, mem_read8(counter_pending) + 1u);
	if ((uint8_t)max_credits <= mem_read8(credit_block + 1u))
		return coin_credits;
	mem_write8(credit_block + 3u, 1u);
	coin_sounds++;
	chute_multiplier = mem_read8(chute_1_multiplier_ADDRESS + chute_offset);
	coins_per_bonus = bonus_adder;
	if (coins_per_bonus != 0u)
	{
		coin_tally = (uint8_t)(mem_read8(credit_block + 2u) + chute_multiplier);
		while (coins_per_bonus <= coin_tally)
		{
			mem_write8(credit_block, mem_read8(credit_block) + 1u);
			coin_tally -= coins_per_bonus;
		}
		mem_write8(credit_block + 2u, coin_tally);
	}
	coin_cost = credit_coin_cost;
	coin_tally = (uint8_t)(mem_read8(credit_block) + chute_multiplier);
	do
	{
		if (coin_tally < coin_cost)
		{
			mem_write8(credit_block, coin_tally);
			return coin_credits;
		}
		coin_tally -= coin_cost;
		coin_credits = (coin_credits & ~0xFFFFu) | (uint16_t)(coin_credits + 1u);
		mem_write8(credit_block + 1u, mem_read8(credit_block + 1u) + 1u);
	} while (mem_read8(credit_block + 1u) < (uint8_t)max_credits);
	mem_write8(credit_block + 1u, max_credits);
	mem_write8(credit_block + 2u, 0u);
	mem_write8(credit_block, 0u);
	return coin_credits;
}

void save_credits(void)
{
	uint32_t credit_block = 0, dest = 0;

	dest = saved_credits;
	credit_block = p1_coins_ADDRESS;
	copy_credit_counts(dest, credit_block);
	update_bookkeeping_checksum();
	dest = saved_credits_backup;
	credit_block = p1_coins_ADDRESS;
	copy_credit_counts(dest, credit_block);
}

void copy_credit_counts(uint32_t dest, uint32_t credit_block)
{
	r14 = 3u;
	do
	{
		mem_write16(dest, mem_read16(credit_block));
		credit_block += 4u;
		r14 = r14 - 1u;
		dest += 2u;
	} while (r14 != 0u);
}

void load_bookkeeping(void)
{
	uint32_t copy_dest = 0, copy_source = 0, crc = 0, data = 0;

	if ((uint16_t)(rom_checksums + rom_checksum_ic8) == mem_read16(bookkeeping_rom_stamp))
	{
		copy_source = bookkeeping_coin_credits;
		compute_bookkeeping_crc(&crc, &copy_source);
		if ((uint16_t)crc == mem_read16(bookkeeping_checksum))
		{
			copy_dest = copy_source;
			copy_source -= 0x40u;
			move_halfwords(copy_dest, copy_source, 0x20u);
			update_bookkeeping_checksum();
			AS32(power_on_frames) = mem_read32(bookkeeping_power_on_time);
			AT16(power_on_frames_high, 2) = mem_read16(bookkeeping_power_on_time_high);
			return;
		}
		data = bookkeeping_backup;
		compute_bookkeeping_crc(&crc, &data);
		if ((uint16_t)crc == mem_read16(bookkeeping_checksum))
		{
			copy_dest = bookkeeping_coin_credits;
			copy_source = bookkeeping_backup;
			move_halfwords(copy_dest, copy_source, 0x20u);
			update_bookkeeping_checksum();
		}
		else
			reset_bookkeeping();
	}
	else
		reset_bookkeeping();
	AS32(power_on_frames) = mem_read32(bookkeeping_power_on_time);
	AT16(power_on_frames_high, 2) = mem_read16(bookkeeping_power_on_time_high);
}

void reset_bookkeeping(void)
{
	uint16_t rom_stamp = 0;

	move_halfwords(bookkeeping_coin_credits, bookkeeping_defaults_ADDRESS, 0x20u);
	move_halfwords(bookkeeping_backup, bookkeeping_defaults_ADDRESS, 0x20u);
	rom_stamp = (uint16_t)(rom_checksums + rom_checksum_ic8);
	mem_write16(bookkeeping_rom_stamp, rom_stamp);
	mem_write16(bookkeeping_backup_rom_stamp, rom_stamp);
	update_bookkeeping_checksum();
}

void update_bookkeeping_checksum(void)
{
	uint32_t crc = 0, data = 0;

	data = bookkeeping_coin_credits;
	compute_bookkeeping_crc(&crc, &data);
	mem_write16(bookkeeping_checksum, crc);
}

void compute_bookkeeping_crc(uint32_t *p_crc, uint32_t *p_data)
{
	uint32_t data = *p_data;
	uint32_t count = 0, crc = 0, ignored = 0;

	count = 0x40u;
	compute_crc16(count, &crc, &data, &ignored);
	*p_crc = crc;
	*p_data = data;
}

void compute_crc16(uint32_t count, uint32_t *p_crc, uint32_t *p_data, uint32_t *p_polynomial)
{
	uint32_t data = *p_data;
	uint32_t carry = 0;
	uint32_t bits_left = 0, crc = 0, polynomial = 0;

	polynomial = 0x10210000u;
	crc = 0xDEBDEB00u;
	do
	{
		crc = (crc & ~0xFFu) | mem_read8(data);
		bits_left = 8u;
		data++;
		do
		{
			carry = crc * 2u < crc;
			crc = crc * 2u;
			if (carry)
				crc ^= polynomial;
			bits_left--;
		} while (bits_left != 0u);
		count--;
	} while (count != 0u);
	bits_left = 8u;
	do
	{
		carry = crc * 2u < crc;
		crc = crc * 2u;
		if (carry)
			crc ^= polynomial;
		bits_left--;
	} while (bits_left != 0u);
	crc = crc >> 16;
	*p_crc = crc;
	*p_data = data;
	*p_polynomial = polynomial;
}

uint32_t compute_crc16_alternate_bytes(uint32_t data, uint32_t count)
{
	uint32_t carry = 0;
	uint32_t bits_left = 0, crc_polynomial = 0, shift_register = 0;

	crc_polynomial = 0x10210000u;
	shift_register = 0xDEBDEB00u;
	do
	{
		shift_register = (shift_register & ~0xFFu) | mem_read8(data);
		data = ((data + 1u) & ~0xFFu) | (uint8_t)(data + 1u + 1u);
		bits_left = 8u;
		do
		{
			carry = shift_register * 2u < shift_register;
			shift_register = shift_register * 2u;
			if (carry)
				shift_register ^= crc_polynomial;
			bits_left--;
		} while (bits_left != 0u);
		count--;
	} while (count != 0u);
	bits_left = 8u;
	do
	{
		carry = shift_register * 2u < shift_register;
		shift_register = shift_register * 2u;
		if (carry)
			shift_register ^= crc_polynomial;
		bits_left--;
	} while (bits_left != 0u);
	shift_register = shift_register >> 16;
	return shift_register;
}

void add_play_time(void)
{
	uint32_t carry = 0;
	uint32_t bookkeeping_base = 0, game_time = 0;
	uint32_t play_time = 0;

	bookkeeping_base = bookkeeping_coin_credits;
	game_time = game_frames;
	play_time = mem_read32(bookkeeping_play_time);
	carry = play_time + game_time < play_time;
	mem_write32(bookkeeping_play_time, play_time + game_time);
	if (carry)
		mem_write16(bookkeeping_base + 0x1Au, mem_read16(bookkeeping_base + 0x1Au) + 1u);
}

void record_player_game(void)
{
	uint32_t carry = 0;
	uint32_t bookkeeping_block = 0, player_time = 0, players_left = 0, status_cursor = 0;
	uint32_t total_time = 0;

	status_cursor = player_status_ADDRESS;
	players_left = 3u;
	do
	{
		if ((int8_t)mem_read8(status_cursor) < 0 || mem_read8(status_cursor) == 1u || mem_read8(status_cursor) == 4u)
		{
			bookkeeping_block = bookkeeping_coin_credits;
			mem_write16(bookkeeping_games, mem_read16(bookkeeping_games) + 1u);
			player_time = game_frames - PLAYER(current_player)->join_time;
			total_time = mem_read32(bookkeeping_player_time);
			carry = total_time + player_time < total_time;
			mem_write32(bookkeeping_player_time, total_time + player_time);
			if (carry)
				mem_write16(bookkeeping_block + 0x20u, mem_read16(bookkeeping_block + 0x20u) + 1u);
			if (mem_read32(bookkeeping_block + 0xCu) <= player_time)
				mem_write32(bookkeeping_block + 0xCu, player_time);
			if (player_time < mem_read32(bookkeeping_block + 0x22u))
				mem_write32(bookkeeping_block + 0x22u, player_time);
			update_bookkeeping_checksum();
			move_halfwords(bookkeeping_block + 0x40u, bookkeeping_block, 0x20u);
			return;
		}
		players_left--;
		status_cursor++;
	} while (players_left != 0u);
	add_play_time();
	bookkeeping_block = bookkeeping_coin_credits;
	mem_write16(bookkeeping_games, mem_read16(bookkeeping_games) + 1u);
	player_time = game_frames - PLAYER(current_player)->join_time;
	total_time = mem_read32(bookkeeping_player_time);
	carry = total_time + player_time < total_time;
	mem_write32(bookkeeping_player_time, total_time + player_time);
	if (carry)
		mem_write16(bookkeeping_block + 0x20u, mem_read16(bookkeeping_block + 0x20u) + 1u);
	if (mem_read32(bookkeeping_block + 0xCu) <= player_time)
		mem_write32(bookkeeping_block + 0xCu, player_time);
	if (player_time < mem_read32(bookkeeping_block + 0x22u))
		mem_write32(bookkeeping_block + 0x22u, player_time);
	update_bookkeeping_checksum();
	move_halfwords(bookkeeping_block + 0x40u, bookkeeping_block, 0x20u);
}

void draw_attract_credits(uint32_t *p_r1, uint32_t *p_r2, uint32_t *p_message_text, uint32_t *p_text_position)
{
	uint32_t r1 = *p_r1;
	uint32_t r2 = *p_r2;
	uint32_t r10 = 0, credit_block = 0, message_text = 0, prompt_texts = 0, text_position = 0;

	switch (cabinet_config)
	{
	case 0:
	case 1:
	case 4:
		credit_block = p1_coins_ADDRESS;
		text_position = 0x310000u | common_credit_position;
		draw_credit_count(credit_block, &r1, &r2, &r10, &text_position);
		r1 = blink_demo_prompt(r1, credit_block);
		if (credits_per_start != 2u || free_play != 0u)
		{
			*p_r1 = r1;
			*p_r2 = r2;
			*p_message_text = r10;
			*p_text_position = text_position;
			return;
		}
		message_text = two_credits_one_play_text_ADDRESS;
		print_text(&message_text, &text_position);
		*p_r1 = r1;
		*p_r2 = r2;
		*p_message_text = message_text;
		*p_text_position = text_position;
		return;
	case 2:
		credit_block = p2_coins_ADDRESS;
		text_position = 0x310000u | credit_position_p3;
		draw_credit_count(credit_block, &r1, &r2, &r10, &text_position);
		text_position = 0x310000u | start_prompt_position_p3;
		prompt_texts = start_prompt_texts_ADDRESS;
		blink_start_prompt(credit_block, prompt_texts, &r2, &r10, &text_position);
		credit_block = p1_coins_ADDRESS;
		text_position = 0x310000u | credit_positions_3p[0];
		draw_credit_count(credit_block, &r1, &r2, &r10, &text_position);
		text_position = 0x310000u | start_prompt_positions_3p[0];
		prompt_texts = start_prompt_texts_ADDRESS;
		blink_start_prompt(credit_block, prompt_texts, &r2, &r10, &text_position);
		*p_r1 = r1;
		*p_r2 = r2;
		*p_message_text = r10;
		*p_text_position = text_position;
		return;
	case 3:
		credit_block = p3_coins_ADDRESS;
		text_position = 0x310000u | credit_position_p3;
		draw_credit_count(credit_block, &r1, &r2, &r10, &text_position);
		text_position = 0x310000u | start_prompt_position_p3;
		prompt_texts = start_prompt_texts_ADDRESS;
		blink_start_prompt(credit_block, prompt_texts, &r2, &r10, &text_position);
		credit_block = p2_coins_ADDRESS;
		text_position = 0x310000u | credit_position_p2;
		draw_credit_count(credit_block, &r1, &r2, &r10, &text_position);
		text_position = 0x310000u | start_prompt_position_p2;
		prompt_texts = start_prompt_texts_ADDRESS;
		blink_start_prompt(credit_block, prompt_texts, &r2, &r10, &text_position);
		credit_block = p1_coins_ADDRESS;
		text_position = 0x310000u | credit_positions_3p[0];
		draw_credit_count(credit_block, &r1, &r2, &r10, &text_position);
		text_position = 0x310000u | start_prompt_positions_3p[0];
		prompt_texts = start_prompt_texts_ADDRESS;
		blink_start_prompt(credit_block, prompt_texts, &r2, &r10, &text_position);
		*p_r1 = r1;
		*p_r2 = r2;
		*p_message_text = r10;
		*p_text_position = text_position;
		return;
	default:
		unknown_code(0xFFBA6u + cabinet_config * 3u);
	}
}

void blink_start_prompt(uint32_t credit_block, uint32_t prompt_texts, uint32_t *p_blink_bit, uint32_t *p_text, uint32_t *p_text_position)
{
	uint32_t blink_bit = *p_blink_bit;
	uint32_t text = *p_text;
	uint32_t text_position = *p_text_position;
	uint16_t credits = 0;
	uint32_t frame_bit = 0, prompt_index = 0;
	uint8_t port_d_bits = 0;

	if ((frame_counter & 0xFu) != 0u)
		return;
	prompt_index = 0u;
	frame_bit = (uint8_t)(5u - frames_per_wait);
	if ((AS32(frame_counter) & shl32(1u, (int)(frame_bit))) != 0u)
	{
		port_d_bits = mem_read8(io_port_d);
		mem_write8(io_port_d, port_d_bits & ~0xCu);
		text = prompt_texts + mem_read16(prompt_texts + prompt_index * 2u);
		erase_text_lines(&text, &text_position);
		*p_blink_bit = frame_bit;
		*p_text = text;
		*p_text_position = text_position;
		return;
	}
	if (free_play != 0u)
	{
		if ((cabinet_config & 1u) != 0u || setting_moving_seat != 0u)
		{
			text = prompt_texts + mem_read16(prompt_texts + prompt_index * 2u);
			print_text_lines(&text, &text_position);
			*p_blink_bit = frame_bit;
			*p_text = text;
			*p_text_position = text_position;
			return;
		}
		r14 = p1_coins_ADDRESS;
		text = prompt_texts + mem_read16(prompt_texts + prompt_index * 2u);
		print_text_lines(&text, &text_position);
		*p_blink_bit = frame_bit;
		*p_text = text;
		*p_text_position = text_position;
		return;
	}
	credits = mem_read8(credit_block + 1u);
	if (credits_per_start <= credits)
	{
		if ((cabinet_config & 1u) != 0u || setting_moving_seat != 0u)
		{
			text = prompt_texts + mem_read16(prompt_texts + prompt_index * 2u);
			print_text_lines(&text, &text_position);
			*p_blink_bit = frame_bit;
			*p_text = text;
			*p_text_position = text_position;
			return;
		}
		r14 = p1_coins_ADDRESS;
		text = prompt_texts + mem_read16(prompt_texts + prompt_index * 2u);
		print_text_lines(&text, &text_position);
		*p_blink_bit = frame_bit;
		*p_text = text;
		*p_text_position = text_position;
		return;
	}
	prompt_index = 1u;
	if (credits_per_start == 2u)
	{
		frame_bit++;
		if ((AS32(frame_counter) & shl32(1u, (int)(frame_bit))) != 0u)
			prompt_index = 3u;
	}
	if (mem_read8(credit_block) != 0u || credits != 0u)
	{
		text = prompt_texts + mem_read16(prompt_texts + prompt_index * 2u);
		print_text_lines(&text, &text_position);
		*p_blink_bit = frame_bit;
		*p_text = text;
		*p_text_position = text_position;
		return;
	}
	prompt_index = 2u;
	text = prompt_texts + mem_read16(prompt_texts + prompt_index * 2u);
	print_text_lines(&text, &text_position);
	*p_blink_bit = frame_bit;
	*p_text = text;
	*p_text_position = text_position;
}

void draw_credit_count(uint32_t credit_block, uint32_t *p_r1, uint32_t *p_coins_per_credit, uint32_t *p_r10, uint32_t *p_text_position)
{
	uint32_t r1 = *p_r1;
	uint32_t coins_per_credit = *p_coins_per_credit;
	uint32_t text_position = *p_text_position;
	uint32_t r10 = 0, compared_block = 0, drawn_bit = 0;
	uint32_t old_flags = 0;

	drawn_bit = 0xFu;
	compared_block = p1_coins_ADDRESS;
	if (credit_block != p1_coins_ADDRESS)
	{
		drawn_bit = 0x12u;
		compared_block = p2_coins_ADDRESS;
		if (credit_block != p2_coins_ADDRESS)
			drawn_bit = 0x13u;
	}
	old_flags = stage_init_flags;
	stage_init_flags = old_flags | shl32(1u, (int)(drawn_bit));
	if ((old_flags & shl32(1u, (int)(drawn_bit))) == 0u || mem_read8(credit_block + 3u) != 0u)
	{
		draw_credit_count_now(credit_block, &r1, &coins_per_credit, &r10, &text_position);
		*p_r1 = r1;
		*p_coins_per_credit = coins_per_credit;
		*p_r10 = r10;
		*p_text_position = text_position;
		return;
	}
	*p_r10 = compared_block;
}

void draw_credit_count_now(uint32_t credit_block, uint32_t *p_credits, uint32_t *p_coins_per_credit, uint32_t *p_text, uint32_t *p_text_position)
{
	uint32_t credits = *p_credits;
	uint32_t coins_per_credit = *p_coins_per_credit;
	uint32_t text_position = *p_text_position;
	uint8_t attribute = 0;
	uint16_t tens = 0;
	uint32_t r1 = 0, coin_cost = 0, coins = 0, plural_cell = 0, text = 0;
	uint8_t tens_nibble = 0;

	credit_text_position = text_position;
	if (free_play != 0u)
	{
		text = free_play_text_ADDRESS;
		print_text_here(&plural_cell, &text, &text_position);
		*p_credits = credits;
		*p_coins_per_credit = coins_per_credit;
		*p_text = text;
		*p_text_position = text_position;
		return;
	}
	text = credits_label_text_ADDRESS;
	print_text_here(&plural_cell, &text, &text_position);
	r1 = (credits & ~0xFFu) | mem_read8(credit_block + 1u);
	if ((uint8_t)r1 >= 2u)
	{
		plural_cell = (plural_cell & ~0xFFu) | 0x73u;
		mem_write16(text_position - 4u, plural_cell);
	}
	tens = (uint16_t)((plural_cell & ~0xFFu) | (uint8_t)r1);
	tens = (uint16_t)((tens & ~0xFFu) | ((uint8_t)tens / 0xAu));
	r1 = (r1 & ~0xFFu) | ((uint8_t)r1 % 0xAu);
	tens_nibble = (uint32_t)tens << 4;
	tens = (uint16_t)((tens & ~0xFFu) | tens_nibble);
	if (tens_nibble == 0u)
	{
		mem_write16(text_position + 2u, 0u);
		text_position = (text_position & ~0xFFFFu) | (uint16_t)(text_position - 2u);
	}
	tens = (uint16_t)((tens & ~0xFFu) | (uint8_t)(tens | r1));
	r1 = (r1 & ~0xFFFFu) | (uint16_t)tens;
	draw_credit_digits(tens, &r1, &text_position);
	text_position = (text_position & ~0xFFFFu) | (uint16_t)(text_position + 4u);
	coin_cost = credit_coin_cost;
	if (coin_cost == 1u)
	{
		mem_write32(text_position, 0u);
		*p_credits = r1;
		*p_coins_per_credit = coin_cost;
		*p_text = text;
		*p_text_position = text_position;
		return;
	}
	coins = mem_read8(credit_block);
	if (coins == 0u)
	{
		mem_write32(text_position, 0u);
		*p_credits = coins;
		*p_coins_per_credit = coin_cost;
		*p_text = text;
		*p_text_position = text_position;
		return;
	}
	attribute = 0u;
	draw_fraction(attribute, &coins, &coin_cost, &text_position);
	*p_credits = coins;
	*p_coins_per_credit = coin_cost;
	*p_text = text;
	*p_text_position = text_position;
}

void reset(void)
{
	boot();
}
