/**
 *    Name:        sound_commands.c
 *    Description: the V60's sound commands (poll_sound_command): songs and sequences started on players; the volume fades and next_sequence_byte
 *    Version:     0.1.0
 *    Created:     13-10-2026
 *    Modified:    15-10-2026
 *    Author:      Sebastian Toyos
 *
 *    SegaSonic The Hedgehog (arcade, Sega System 32): the Z80 sound program decompiled into C by
 *    tools/decompile_sound.py, with the names of sound_symbols.txt.
 */
#include "sound.h"

void poll_sound_command(void)
{
	uint8_t byte = 0, command_number = 0;
	uint16_t address = 0, header = 0;
	uint16_t exchanged_player = 0;

	z80_block(26);
	address = sound_command_group_ADDRESS;
	byte = sound_command_group;
	if (byte == 0u)
	{
		z80_cycles(6);
		return;
	}
	z80_block(55);
	sound_command_group = 0u;
	command_number = z80_read8((address & ~0xFFu) | (uint8_t)(address - 1u));
	byte &= 0x7Fu;
	address = sound_groups_ADDRESS;
	address = read_table_word(address, byte);
	z80_block(16);
	if (z80_read8(address) < command_number)
	{
		z80_cycles(6);
		return;
	}
	z80_block(37);
	byte = command_number;
	address++;
	header = SEQUENCE_PLAYER_ADDRESS(0);
	address = read_table_word(address, byte);
	z80_block(29);
	exchanged_player = header;
	header = address;
	address = exchanged_player;
	byte = z80_read8(header);
	if (byte >> 7 != 0u)
	{
		z80_block(18);
		if (((byte >> 6) & 1u) != 0u)
		{
			z80_block(29);
			address = (uint32_t)(uint8_t)((address >> 8) + 1u) << 8;
			address = (uint32_t)(uint8_t)((address >> 8) + 1u) << 8;
			address = (uint32_t)(uint8_t)((address >> 8) + 1u) << 8;
			select_channel_player(&header, &address);
			z80_block(46);
			byte = z80_read8(header) & 0xFu;
			if (byte < (z80_read8(address) & 0xFu))
			{
				z80_block(10);
				return;
			}
			z80_block(17);
			address = clear_player(address);
			z80_block(87);
			header++;
			SEQUENCE_PLAYER(address)->flags = byte;
			address = (address & ~0xFFu) | 2u;
			AS8(SEQUENCE_PLAYER(address - 0x2u)->position) = (uint8_t)header;
			address = (address & ~0xFFu) | (uint8_t)(address + 1u);
			AT8(SEQUENCE_PLAYER(address - 0x3u)->position, 1) = header >> 8;
			address = (address & ~0xFFu) | 6u;
			AS8(SEQUENCE_PLAYER(address - 0x6u)->delta_time) = 1u;
			address &= ~0xFFu;
			SEQUENCE_PLAYER(address)->flags |= 0x80u;
			return;
		}
		z80_block(10);
		z80_block(54);
		address = (uint32_t)(uint8_t)((address >> 8) + 1u) << 8;
		address = (uint32_t)(uint8_t)((address >> 8) + 1u) << 8;
		byte = z80_read8(header) & 0xFu;
		if (byte < (SEQUENCE_PLAYER(address)->flags & 0xFu))
		{
			z80_block(10);
			return;
		}
		z80_block(17);
		address = clear_player(address);
		z80_block(7);
		SEQUENCE_PLAYER(address)->flags = byte;
		z80_block(80);
		header++;
		address = (address & ~0xFFu) | 2u;
		AS8(SEQUENCE_PLAYER(address - 0x2u)->position) = (uint8_t)header;
		address = (address & ~0xFFu) | (uint8_t)(address + 1u);
		AT8(SEQUENCE_PLAYER(address - 0x3u)->position, 1) = header >> 8;
		address = (address & ~0xFFu) | 6u;
		AS8(SEQUENCE_PLAYER(address - 0x6u)->delta_time) = 1u;
		address &= ~0xFFu;
		SEQUENCE_PLAYER(address)->flags |= 0x80u;
		return;
	}
	z80_block(18);
	if (((byte >> 6) & 1u) != 0u)
	{
		z80_block(80);
		header++;
		address = (address & ~0xFFu) | 2u;
		AS8(SEQUENCE_PLAYER(address - 0x2u)->position) = (uint8_t)header;
		address = (address & ~0xFFu) | (uint8_t)(address + 1u);
		AT8(SEQUENCE_PLAYER(address - 0x3u)->position, 1) = header >> 8;
		address = (address & ~0xFFu) | 6u;
		AS8(SEQUENCE_PLAYER(address - 0x6u)->delta_time) = 1u;
		address &= ~0xFFu;
		SEQUENCE_PLAYER(address)->flags |= 0x80u;
		return;
	}
	z80_block(18);
	if (((byte >> 5) & 1u) != 0u)
	{
		start_sequence(header, address);
		return;
	}
	z80_block(10);
	z80_block(17);
	address = clear_player(address);
	z80_block(82);
	header++;
	byte = z80_read8(header);
	SEQUENCE_PLAYER(address)->flags |= 0x20u;
	header++;
	address = (address & ~0xFFu) | 0x20u;
	AS8(SEQUENCE_PLAYER(address - 0x20u)->playlist) = (uint8_t)header;
	address++;
	AT8(SEQUENCE_PLAYER(address - 0x21u)->playlist, 1) = header >> 8;
	header = address & ~0xFFu;
	start_song(header, byte);
}

void start_song(uint16_t player, uint8_t song_number)
{
	uint16_t address = 0, song_header = 0;
	uint16_t exchanged_player = 0;

	z80_block(27);
	address = songs_ADDRESS;
	address = read_table_word(address, song_number);
	z80_block(21);
	exchanged_player = player;
	song_header = address;
	address = exchanged_player;
	start_sequence(song_header, address);
}

void start_sequence(uint16_t header, uint16_t player)
{
	uint8_t bank = 0;
	uint16_t player_field = 0;

	z80_block(127);
	header++;
	player_field = (player & ~0xFFu) | 0x1Bu;
	bank = z80_read8(header);
	header++;
	SEQUENCE_PLAYER(player_field - 0x1Bu)->bank = bank;
	player_field = (player_field & ~0xFFu) | 2u;
	AS8(SEQUENCE_PLAYER(player_field - 0x2u)->position) = z80_read8(header);
	player_field = (player_field & ~0xFFu) | (uint8_t)(player_field + 1u);
	AT8(SEQUENCE_PLAYER(player_field - 0x3u)->position, 1) = z80_read8((uint16_t)(header + 1u));
	player_field = (player_field & ~0xFFu) | 6u;
	AS8(SEQUENCE_PLAYER(player_field - 0x6u)->delta_time) = 1u;
	player_field &= ~0xFFu;
	SEQUENCE_PLAYER(player_field)->flags |= 0x80u;
}

uint16_t find_lowest_player(uint16_t player_field)
{
	uint8_t byte = 0, lowest = 0, players_left = 0;

	z80_block(30);
	players_left = 2u;
	lowest = z80_read8(player_field);
	lowest_player_field = player_field;
	for (;;)
	{
		z80_block(25);
		player_field = (player_field & 0xFFu) | ((uint32_t)(uint8_t)((player_field >> 8) + 1u) << 8);
		byte = z80_read8(player_field);
		if (lowest <= byte)
		{
			z80_block(8);
			players_left--;
			if (players_left == 0u)
				break;
			z80_cycles(5);
			continue;
		}
		z80_block(20);
		lowest = byte;
		lowest_player_field = player_field;
		z80_block(8);
		players_left--;
		if (players_left == 0u)
			break;
		z80_cycles(5);
	}
	z80_block(26);
	player_field = lowest_player_field;
	return player_field;
}

void select_channel_player(uint16_t *p_header, uint16_t *p_player)
{
	uint16_t header = (uint16_t)*p_header;
	uint16_t player = (uint16_t)*p_player;
	uint8_t player_page = 0;

	z80_block(55);
	header++;
	player_page = (z80_read8(header) & 0xFu) - 0xAu + (player >> 8);
	header--;
	player = (uint32_t)player_page << 8;
	*p_header = header;
	*p_player = (*p_player & 0xFFu) | player;
}

uint16_t clear_player(uint16_t player)
{
	uint16_t saved_player = 0;

	z80_block(101);
	saved_player = player;
	SEQUENCE_PLAYER(player)->flags = 0u;
	move_block(player, (uint16_t)(saved_player + 1u), 0x30u, 1u);
	player = (player + 0x30u) & 0xFF00u;
	return player;
}

void fade_music_volume(uint16_t mailbox_page)
{
	uint8_t queue_page = 0, write_index = 0;
	uint16_t fade_field = 0;

	z80_block(28);
	fade_field = music_fade_wait_ADDRESS;
	if (music_fade_wait != 0u)
	{
		z80_block(21);
		SEQUENCE_PLAYER(fade_field - 0x19u)->fade_wait--;
		return;
	}
	z80_cycles(5);
	z80_block(63);
	queue_page = 0xE1u;
	write_index = midi_queue_write;
	fade_field = (fade_field & ~0xFFu) | 0x15u;
	SEQUENCE_PLAYER(fade_field - 0x15u)->fade_volume--;
	if (SEQUENCE_PLAYER(fade_field - 0x15u)->fade_volume == 0u)
	{
		z80_block(223);
		fade_field &= ~0xFFu;
		SEQUENCE_PLAYER(fade_field)->flags &= 0x7Fu;
		SEQUENCE_PLAYER(fade_field)->fade_speed = 0u;
		z80_write8((uint16_t)(((uint32_t)queue_page << 8) | write_index), 0xBEu);
		write_index++;
		z80_write8((uint16_t)(((uint32_t)queue_page << 8) | write_index), 0x50u);
		write_index++;
		z80_write8((uint16_t)(((uint32_t)queue_page << 8) | write_index), 0x7Eu);
		write_index++;
		midi_queue_count++;
		z80_write8((uint16_t)(((uint32_t)queue_page << 8) | write_index), 0xBEu);
		write_index++;
		z80_write8((uint16_t)(((uint32_t)queue_page << 8) | write_index), 0x79u);
		write_index++;
		z80_write8((uint16_t)(((uint32_t)queue_page << 8) | write_index), 0u);
		midi_queue_count++;
		midi_queue_write = write_index + 1u;
		return;
	}
	z80_block(134);
	z80_write8((uint16_t)(((uint32_t)queue_page << 8) | write_index), 0xBEu);
	write_index++;
	z80_write8((uint16_t)(((uint32_t)queue_page << 8) | write_index), 0x10u);
	write_index++;
	z80_write8((uint16_t)(((uint32_t)queue_page << 8) | write_index), SEQUENCE_PLAYER(fade_field - 0x15u)->fade_volume);
	midi_queue_write = write_index + 1u;
	midi_queue_count++;
	fade_field = (fade_field & ~0xFFu) | 0x18u;
	SEQUENCE_PLAYER(fade_field & ~0xFFu)->fade_wait = SEQUENCE_PLAYER(fade_field - 0x18u)->fade_speed;
}

void fade_player_volume(uint16_t queue_pointer, uint16_t player, uint16_t mailbox_page)
{
	uint8_t write_index = 0;
	uint16_t fade_field = 0;

	write_index = (uint8_t)queue_pointer;
	z80_block(23);
	fade_field = (player & ~0xFFu) | 0x18u;
	if (SEQUENCE_PLAYER(fade_field - 0x18u)->fade_speed == 0u)
	{
		z80_cycles(6);
		return;
	}
	z80_block(28);
	fade_field = (fade_field & ~0xFFu) | 0x19u;
	if (SEQUENCE_PLAYER(fade_field - 0x19u)->fade_wait != 0u)
	{
		z80_block(21);
		SEQUENCE_PLAYER(fade_field - 0x19u)->fade_wait--;
		return;
	}
	z80_block(39);
	fade_field = (fade_field & ~0xFFu) | 0x1Au;
	SEQUENCE_PLAYER(fade_field - 0x1Au)->fade_channel_volume--;
	if (SEQUENCE_PLAYER(fade_field - 0x1Au)->fade_channel_volume == 0u)
	{
		z80_block(138);
		fade_field &= ~0xFFu;
		SEQUENCE_PLAYER(fade_field)->flags = 0u;
		SEQUENCE_PLAYER(fade_field)->fade_speed = 0u;
		z80_write8((queue_pointer & ~0xFFu) | write_index, 0xBAu);
		write_index++;
		z80_write8((queue_pointer & ~0xFFu) | write_index, 0x7Bu);
		write_index++;
		z80_write8((queue_pointer & ~0xFFu) | write_index, 0u);
		midi_queue_count++;
		midi_queue_write = write_index + 1u;
		return;
	}
	z80_block(134);
	z80_write8((queue_pointer & ~0xFFu) | write_index, 0xBAu);
	write_index++;
	z80_write8((queue_pointer & ~0xFFu) | write_index, 7u);
	write_index++;
	z80_write8((queue_pointer & ~0xFFu) | write_index, SEQUENCE_PLAYER(fade_field - 0x1Au)->fade_channel_volume);
	midi_queue_write = write_index + 1u;
	midi_queue_count++;
	fade_field = (fade_field & ~0xFFu) | 0x18u;
	SEQUENCE_PLAYER(fade_field & ~0xFFu)->fade_wait = SEQUENCE_PLAYER(fade_field - 0x18u)->fade_speed;
}

uint16_t next_sequence_byte(uint16_t cursor, uint16_t player)
{
	z80_block(38);
	cursor++;
	if (cursor >> 8 == 0xC0u)
	{
		z80_block(60);
		SEQUENCE_PLAYER(player)->bank++;
		port_out(ROM_BANK_LOW, SEQUENCE_PLAYER(player)->bank);
		cursor = (cursor & 0xFFu) | 0xA000u;
	}
	z80_block(20);
	return cursor;
}
