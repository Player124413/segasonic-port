/**
 *    Name:        system/bookkeeping.c
 *    Description: the bookkeeping in the backup RAM: load, reset, checksums (compute_crc16), play time, games
 *    Version:     0.1.0
 *    Created:     12-10-2026
 *    Modified:    15-10-2026
 *    Author:      Sebastian Toyos
 *
 *    SegaSonic The Hedgehog (arcade, Sega System 32): the game's V60 program decompiled into C by
 *    tools/decompile.py, with the names of symbols.txt.
 */
#include "game.h"

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
	uint32_t r0 = 0, data = 0;

	data = bookkeeping_coin_credits;
	compute_bookkeeping_crc(&r0, &data);
	mem_write16(bookkeeping_checksum, r0);
	return r0;
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
