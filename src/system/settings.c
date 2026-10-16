/**
 *    Name:        system/settings.c
 *    Description: what the board keeps when it is off: the settings in the 93C46 EEPROM and the bookkeeping in the backup RAM, with their checksums
 *    Version:     0.1.0
 *    Created:     12-10-2026
 *    Modified:    16-10-2026
 *    Author:      Sebastian Toyos
 *
 *    SegaSonic The Hedgehog (arcade, Sega System 32): the game's V60 program decompiled into C by
 *    tools/decompile.py, with the names of symbols.txt.
 */
#include "game.h"

uint32_t compute_crc16_strided(uint32_t crc, uint32_t count, uint32_t data, uint32_t byte_gap)
{
	uint32_t carry = 0;
	uint32_t bits_left = 0, crc_polynomial = 0, shift_register = 0;

	byte_gap++;
	shift_register = (crc & ~0xFFu) | mem_read8(data);
	data += byte_gap;
	shift_register = (shift_register & ~0xFFFFu) | (uint16_t)(shift_register << 8) | mem_read8(data);
	data += byte_gap;
	shift_register = ((shift_register << 8) | mem_read8(data)) << 8;
	count -= 3u;
	crc_polynomial = 0x10210000u;
	do
	{
		data += byte_gap;
		shift_register = (shift_register & ~0xFFu) | mem_read8(data);
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

void eeprom_unlock(void)
{
	uint8_t command = 0;
	uint32_t ignored = 0, lines = 0, port_d = 0;

	command = 0x30u;
	eeprom_select(&lines, &ignored, &port_d);
	lines = eeprom_send_command(lines, command, port_d);
	eeprom_deselect(lines, port_d);
	interrupts_on();
}

uint32_t eeprom_read_word_at(uint32_t data, uint8_t command)
{
	uint32_t lines = 0, port_d = 0, port_f = 0;

	command |= 0x80u;
	eeprom_select(&lines, &port_f, &port_d);
	eeprom_read_word(command, port_f, port_d, &data, &lines);
	interrupts_on();
	return data;
}

void load_settings(uint32_t crc)
{
	uint32_t ignored = 0;
	uint8_t port_f_bits = 0;

	eeprom_read_all(crc);
	if (eeprom_magic != 0x53333241u)
	{
		if (eeprom_magic != 0x41474553u)
			move_halfwords(eeprom_magic_ADDRESS, eeprom_defaults_ADDRESS, 0x20u);
		else
		{
			move_halfwords(eeprom_magic_ADDRESS, eeprom_defaults_ADDRESS, 0x20u);
			eeprom_write_count = 0x64u;
		}
		port_f_bits = mem_read8(io_port_f);
		mem_write8(moving_seat_dip, port_f_bits & 1u);
		mem_write16(bookkeeping_rom_stamp, 0u);
		eeprom_rom_stamp = rom_checksums + rom_checksum_ic8;
		compute_eeprom_checksum(&crc, &ignored);
		eeprom_checksum = crc;
		eeprom_save(&ignored, &ignored);
		return;
	}
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
}

void eeprom_read_all_and_enable_interrupts(uint32_t data)
{
	eeprom_read_all(data);
	interrupts_on();
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

void eeprom_write_word_at(uint16_t data, uint8_t command)
{
	uint32_t lines = 0, port_d = 0, port_f = 0;

	command |= 0x40u;
	eeprom_select(&lines, &port_f, &port_d);
	eeprom_write_word(data, lines, command, port_f, port_d);
	interrupts_on();
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
	uint32_t bits_left = 0, data_bit = 0, ignored = 0;

	command_bits = (uint8_t)command;
	lines = (lines & ~0xFFu) | (lines & 0x1Fu);
	ignored = eeprom_set_lines(0u, (uint8_t)lines, port_d);
	lines = (lines & ~0xFFu) | (uint8_t)(lines ^ 0x40u);
	ignored = eeprom_set_lines(ignored, (uint8_t)lines, port_d);
	lines = (lines & ~0xFFu) | (uint8_t)(lines ^ 0x60u);
	ignored = eeprom_set_lines(ignored, (uint8_t)lines, port_d);
	lines = (lines & ~0xFFu) | (uint8_t)(lines ^ 0x40u);
	ignored = eeprom_set_lines(ignored, (uint8_t)lines, port_d);
	lines = (lines & ~0xFFu) | (uint8_t)(lines ^ 0xC0u);
	ignored = eeprom_set_lines(ignored, (uint8_t)lines, port_d);
	lines = (lines & ~0xFFu) | (uint8_t)(lines ^ 0x40u);
	eeprom_set_lines(ignored, (uint8_t)lines, port_d);
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
	uint32_t crc = 0, polynomial = 0;

	current_object = eeprom_write_count_ADDRESS;
	compute_eeprom_crc(&crc, &polynomial);
	*p_crc = crc;
	*p_polynomial = polynomial;
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
	uint32_t ignored = 0;

	lines = (lines & ~0xFFu) | (lines & 0x1Fu);
	ignored = eeprom_set_lines(0u, (uint8_t)lines, port_d);
	return lines;
}

uint32_t eeprom_set_lines(uint32_t data, uint8_t lines, uint32_t port_d)
{
	mem_write8(port_d, lines);
	return data;
}

void load_bookkeeping(void)
{
	uint32_t copy_dest = 0, copy_source = 0, crc = 0, data = 0;

	if ((uint16_t)(rom_checksums + rom_checksum_ic8) != mem_read16(bookkeeping_rom_stamp))
	{
		reset_bookkeeping();
		power_on_time = mem_read32(bookkeeping_power_on_time);
		power_on_frames_top = mem_read16(bookkeeping_power_on_time_high);
		return;
	}
	copy_source = bookkeeping_coin_credits;
	compute_bookkeeping_crc(&crc, &copy_source);
	if ((uint16_t)crc == mem_read16(bookkeeping_checksum))
	{
		copy_dest = copy_source;
		copy_source -= 0x40u;
		move_halfwords(copy_dest, copy_source, 0x20u);
		update_bookkeeping_checksum();
		power_on_time = mem_read32(bookkeeping_power_on_time);
		power_on_frames_top = mem_read16(bookkeeping_power_on_time_high);
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
		power_on_time = mem_read32(bookkeeping_power_on_time);
		power_on_frames_top = mem_read16(bookkeeping_power_on_time_high);
		return;
	}
	reset_bookkeeping();
	power_on_time = mem_read32(bookkeeping_power_on_time);
	power_on_frames_top = mem_read16(bookkeeping_power_on_time_high);
}

void reset_bookkeeping(void)
{
	uint16_t rom_stamp = 0;
	uint32_t ignored = 0;

	move_halfwords(bookkeeping_coin_credits, bookkeeping_defaults_ADDRESS, 0x20u);
	move_halfwords(bookkeeping_backup, bookkeeping_defaults_ADDRESS, 0x20u);
	rom_stamp = (uint16_t)(rom_checksums + rom_checksum_ic8);
	mem_write16(bookkeeping_rom_stamp, rom_stamp);
	mem_write16(bookkeeping_backup_rom_stamp, rom_stamp);
	ignored = update_bookkeeping_checksum();
}

uint32_t update_bookkeeping_checksum(void)
{
	uint32_t crc = 0, data = 0;

	data = bookkeeping_coin_credits;
	compute_bookkeeping_crc(&crc, &data);
	mem_write16(bookkeeping_checksum, crc);
	return crc;
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
		shift_register = (shift_register & ~0xFFu) | ROM8(data);
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
	while ((int8_t)mem_read8(status_cursor) >= 0 && mem_read8(status_cursor) != 1u && mem_read8(status_cursor) != 4u)
	{
		players_left--;
		status_cursor++;
		if (players_left == 0u)
		{
			add_play_time();
			break;
		}
	}
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
