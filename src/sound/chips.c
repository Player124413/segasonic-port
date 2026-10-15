/**
 *    Name:        chips.c
 *    Description: the two YM3438 and the RF5C68 set up and written; the tracks' and MIDI parts' start values; read_table_word
 *    Version:     0.1.0
 *    Created:     13-10-2026
 *    Modified:    15-10-2026
 *    Author:      Sebastian Toyos
 *
 *    SegaSonic The Hedgehog (arcade, Sega System 32): the Z80 sound program decompiled into C by
 *    tools/decompile_sound.py, with the names of sound_symbols.txt.
 */
#include "sound.h"

uint16_t init_fm_chips(void)
{
	uint8_t fm_register = 0, init_register = 0, pairs_left = 0, value = 0;
	uint16_t init_entry = 0, leftover_bc = 0;

	z80_block(17);
	init_entry = fm_global_init_ADDRESS;
	pairs_left = 0xBu;
	for (;;)
	{
		z80_block(43);
		init_register = SOUND_ROM8(init_entry);
		init_entry++;
		value = SOUND_ROM8(init_entry);
		init_entry++;
		fm_register = init_register;
		write_fm1(fm_register, value);
		z80_block(67);
		fm_register = init_register;
		write_fm2(fm_register, value);
		z80_block(8);
		pairs_left--;
		if (pairs_left == 0u)
			break;
		z80_cycles(5);
	}
	z80_block(31);
	value = 0xFBu;
	fm_register = 0x24u;
	write_fm1(fm_register, value);
	z80_block(81);
	value = 3u;
	fm_register = 0x25u;
	write_fm1(fm_register, value);
	z80_block(17);
	init_entry = fm_operator_init_ADDRESS;
	pairs_left = 0x24u;
	for (;;)
	{
		z80_block(43);
		init_register = SOUND_ROM8(init_entry);
		init_entry++;
		value = SOUND_ROM8(init_entry);
		init_entry++;
		fm_register = init_register;
		write_fm1(fm_register, value);
		z80_block(67);
		fm_register = init_register;
		write_fm2(fm_register, value);
		z80_block(67);
		fm_register = init_register;
		write_fm1_part2(fm_register, value);
		z80_block(67);
		fm_register = init_register;
		write_fm2_part2(fm_register, value);
		z80_block(8);
		pairs_left--;
		if (pairs_left == 0u)
			break;
		z80_cycles(5);
	}
	z80_block(10);
	leftover_bc = ((uint32_t)pairs_left << 8) | init_register;
	return leftover_bc;
}

uint8_t write_fm_global(uint8_t fm_register, uint8_t value)
{
	uint8_t chip_channel = 0;

	z80_block(39);
	chip_channel = fm_channel;
	if (((chip_channel >> 3) & 1u) == 0u)
	{
		write_fm1(fm_register, value);
		return value;
	}
	z80_block(10);
	write_fm2(fm_register, value);
	return value;
}

void write_fm_channel(uint8_t fm_register, uint8_t value)
{
	uint8_t channel_then_value = 0, saved_value = 0;
	uint8_t fm2_chip_channel = 0, fm1_chip_channel = 0;

	z80_block(35);
	saved_value = value;
	channel_then_value = fm_channel;
	if (((channel_then_value >> 3) & 1u) != 0u)
	{
		z80_block(22);
		fm2_chip_channel = channel_then_value;
		channel_then_value = saved_value;
		if (((fm2_chip_channel >> 2) & 1u) != 0u)
		{
			write_fm2_part2(fm_register, channel_then_value);
			return;
		}
		write_fm2(fm_register, channel_then_value);
		return;
	}
	z80_block(22);
	fm1_chip_channel = channel_then_value;
	channel_then_value = saved_value;
	if (((fm1_chip_channel >> 2) & 1u) != 0u)
	{
		write_fm1_part2(fm_register, channel_then_value);
		return;
	}
	write_fm1(fm_register, channel_then_value);
}

void write_fm1(uint8_t fm_register, uint8_t value)
{
	uint8_t saved_value = 0;

	z80_block(73);
	saved_value = value;
	interrupts_off();
	port_out(FM1_ADDRESS, (uint8_t)fm_register);
	port_out(FM1_DATA, saved_value);
	interrupts_on();
}

void write_fm1_part2(uint8_t fm_register, uint8_t value)
{
	uint8_t saved_value = 0;

	z80_block(73);
	saved_value = value;
	interrupts_off();
	port_out(FM1_PART2_ADDRESS, (uint8_t)fm_register);
	port_out(FM1_PART2_DATA, saved_value);
	interrupts_on();
}

void write_fm2(uint8_t fm_register, uint8_t value)
{
	uint8_t saved_value = 0;

	z80_block(73);
	saved_value = value;
	interrupts_off();
	port_out(FM2_ADDRESS, (uint8_t)fm_register);
	port_out(FM2_DATA, saved_value);
	interrupts_on();
}

void write_fm2_part2(uint8_t fm_register, uint8_t value)
{
	uint8_t saved_value = 0;

	z80_block(73);
	saved_value = value;
	interrupts_off();
	port_out(FM2_PART2_ADDRESS, (uint8_t)fm_register);
	port_out(FM2_PART2_DATA, saved_value);
	interrupts_on();
}

void init_pcm_chip(uint32_t unused_c)
{
	uint8_t bank_pairs_left = 0, init_register = 0, pairs_left = 0, pcm_register = 0, value = 0;
	uint16_t init_entry = 0;
	uint8_t saved_bank_pairs_left = 0;

	z80_block(14);
	value = 0u;
	bank_pairs_left = 8u;
	for (;;)
	{
		z80_block(35);
		saved_bank_pairs_left = bank_pairs_left;
		pcm_register = 7u;
		write_pcm_register(pcm_register, value);
		z80_block(84);
		z80_write8(PCM_WAVE_WINDOW, 0x80u);
		move_block(PCM_WAVE_WINDOW, 0xD001u, 0x1000u, 1u);
		value++;
		pcm_register = 7u;
		write_pcm_register(pcm_register, value);
		z80_block(134);
		z80_write8(PCM_WAVE_WINDOW, 0xFFu);
		move_block(PCM_WAVE_WINDOW, 0xD001u, 0x1000u, 1u);
		z80_write8(PCM_SILENT_LOOP, 0x80u);
		move_block(PCM_SILENT_LOOP, 0xD2BDu, 0x800u, 1u);
		value++;
		bank_pairs_left = saved_bank_pairs_left - 1u;
		if (bank_pairs_left == 0u)
			break;
		z80_cycles(5);
	}
	z80_block(17);
	init_entry = pcm_register_init_ADDRESS;
	pairs_left = 0x29u;
	for (;;)
	{
		z80_block(43);
		init_register = SOUND_ROM8(init_entry);
		init_entry++;
		value = SOUND_ROM8(init_entry);
		init_entry++;
		pcm_register = init_register;
		write_pcm_register(pcm_register, value);
		z80_block(8);
		pairs_left--;
		if (pairs_left == 0u)
			break;
		z80_cycles(5);
	}
	z80_block(10);
}

void write_pcm_register(uint8_t pcm_register, uint8_t register_value)
{
	z80_block(45);
	z80_write8((pcm_register & 0xFFu) | 0xC000u, register_value);
}

void init_track_channels(uint32_t unused_c)
{
	uint8_t voices_left = 0;
	uint16_t code_entry = 0, sample_voice = 0;
	uint8_t saved_voices_left = 0;

	z80_block(27);
	sample_voice = 0xE401u;
	code_entry = voice_chip_channels_ADDRESS;
	voices_left = 0x14u;
	for (;;)
	{
		z80_block(70);
		saved_voices_left = voices_left;
		SAMPLE_VOICE(sample_voice - 0x1u)->chip_channel = SOUND_ROM8(code_entry);
		code_entry++;
		sample_voice += 0x12u;
		voices_left = saved_voices_left - 1u;
		if (voices_left == 0u)
			break;
		z80_cycles(5);
	}
	z80_block(10);
}

void init_pcm_tracks(void)
{
	uint8_t voices_left = 0;
	uint16_t sample_voice = 0;

	z80_block(17);
	sample_voice = 0xE409u;
	voices_left = 8u;
	for (;;)
	{
		z80_block(98);
		SAMPLE_VOICE(sample_voice - 0x9u)->wave_page = 0xD0u;
		sample_voice += 7u;
		AS8(SAMPLE_VOICE(sample_voice - 0x10u)->stream_position) = 0xFFu;
		sample_voice++;
		AT8(SAMPLE_VOICE(sample_voice - 0x11u)->stream_position, 1) = 7u;
		sample_voice += 0xAu;
		voices_left--;
		if (voices_left == 0u)
			break;
		z80_cycles(5);
	}
	z80_block(10);
}

void init_midi_parts(void)
{
	uint8_t factor = 0, value = 0;
	uint16_t count = 0, part = 0, template_then_product = 0;

	z80_block(34);
	value = 1u;
	template_then_product = part_templates_ADDRESS;
	template_then_product = read_table_word(template_then_product, value);
	z80_block(70);
	move_block(template_then_product, MIDI_PART_ADDRESS(0), 0x48u, 1u);
	value = 0u;
	template_then_product = part_templates_ADDRESS;
	template_then_product = read_table_word(template_then_product, value);
	z80_block(57);
	move_block(template_then_product, MIDI_PART_ADDRESS(8), 0x2Du, 1u);
	template_then_product += 0x2Du;
	part = MIDI_PART_ADDRESS(0);
	count = 0xD00u;
	for (;;)
	{
		z80_block(56);
		template_then_product = (template_then_product & 0xFFu) | ((uint32_t)MIDI_PART(part)->volume << 8);
		value = 0xFFu;
		if ((MIDI_PART(part)->flags & 1u) != 0u)
		{
		scale_part_volume:
			z80_block(21);
			factor = value;
			template_then_product = multiply_bytes(factor, template_then_product);
			z80_block(18);
			value = (uint8_t)(template_then_product >> 8);
			if (value != 0u)
			{
				z80_block(4);
				value++;
			}
			z80_block(52);
			MIDI_PART(part)->effective_volume = value;
			part += 9u;
			count = (uint32_t)(uint8_t)((count >> 8) - 1u) << 8;
			if (count >> 8 == 0u)
				break;
			z80_cycles(5);
			continue;
		}
		z80_block(13);
		value = music_volume;
		goto scale_part_volume;
	}
	z80_block(10);
}

uint16_t read_table_word(uint16_t table, uint8_t index)
{
	uint16_t offset = 0, word = 0;

	z80_block(88);
	offset = index;
	table = table + offset + offset;
	word = z80_read16(table);
	return word;
}
