/**
 *    Name:        fm_voices.c
 *    Description: the YM3438 voices: note off, stop, release with the sustain pedal, the music and channel volumes, the patches
 *    Version:     0.1.0
 *    Created:     13-10-2026
 *    Modified:    15-10-2026
 *    Author:      Sebastian Toyos
 *
 *    SegaSonic The Hedgehog (arcade, Sega System 32): the Z80 sound program decompiled into C by
 *    tools/decompile_sound.py, with the names of sound_symbols.txt.
 */
#include "sound.h"

void fm_note_off(uint16_t *p_de, uint16_t *p_saved_channel, uint8_t *p_a)
{
	uint16_t saved_channel = (uint16_t)*p_saved_channel;
	uint8_t channel = 0, flags = 0, key_off_value = 0, voices_left = 0;
	uint16_t channel_offset = 0, controls_entry = 0, voice = 0, voice_size = 0;

	z80_block(51);
	voice = FM_VOICE_ADDRESS(0);
	voice_size = 0x12u;
	voices_left = 0xCu;
	channel = midi_status & 0xFu;
	for (;;)
	{
		z80_block(29);
		if (channel != FM_VOICE(voice)->midi_channel)
		{
		fm_note_off_next_voice:
			z80_block(23);
			voice += voice_size;
			voices_left--;
			if (voices_left == 0u)
			{
				z80_block(10);
				*p_de = voice_size;
				*p_saved_channel = saved_channel;
				*p_a = channel;
				return;
			}
			z80_cycles(5);
			continue;
		}
		z80_block(46);
		saved_channel = (saved_channel & ~0xFFu) | channel;
		if (note_code != FM_VOICE(voice)->note)
		{
		fm_note_off_not_this:
			z80_block(4);
			channel = (uint8_t)saved_channel;
			goto fm_note_off_next_voice;
		}
		z80_block(37);
		flags = FM_VOICE(voice)->flags;
		if (((flags >> 1) & 1u) != 0u)
			goto fm_note_off_not_this;
		z80_block(17);
		if ((flags & 0x7Fu) != 0u)
			break;
		goto fm_note_off_not_this;
	}
	z80_block(65);
	channel = (uint8_t)saved_channel;
	channel_offset = channel;
	controls_entry = channel_controls_ADDRESS + channel_offset;
	if (z80_read8(controls_entry) >> 7 == 0u)
	{
		key_off_value = stop_fm_voice(voice);
		*p_de = channel_offset;
		*p_saved_channel = controls_entry;
		*p_a = key_off_value;
		return;
	}
	z80_block(33);
	FM_VOICE(voice)->flags |= 2u;
	*p_de = channel_offset;
	*p_saved_channel = controls_entry;
	*p_a = channel;
}

uint8_t stop_fm_voice(uint16_t voice)
{
	uint8_t fm_register = 0, register_value = 0;

	z80_block(212);
	FM_VOICE(voice)->flags = 0u;
	FM_VOICE(voice)->midi_channel = 0u;
	FM_VOICE(voice)->vibrato_range = 0u;
	FM_VOICE(voice)->vibrato_depth = 0u;
	AS8(FM_VOICE(voice)->vibrato_offset) = 0u;
	AT8(FM_VOICE(voice)->vibrato_offset, 1) = 0u;
	FM_VOICE(voice)->vibrato_block = 0u;
	FM_VOICE(voice)->vibrato_phase = 0u;
	register_value = FM_VOICE(voice)->chip_channel;
	fm_channel = register_value;
	register_value &= 7u;
	fm_register = 0x28u;
	return write_fm_global(fm_register, register_value);
}

void release_held_fm_voice(uint16_t voice)
{
	uint8_t fm_register = 0, key_on_register = 0, register_value = 0;

	z80_block(99);
	FM_VOICE(voice)->flags = 0u;
	FM_VOICE(voice)->midi_channel = 0u;
	key_on_register = 0x28u;
	register_value = FM_VOICE(voice)->chip_channel;
	fm_channel = register_value;
	if (((register_value >> 3) & 1u) == 0u)
	{
		z80_block(17);
		register_value &= 7u;
		fm_register = key_on_register;
		write_fm1(fm_register, register_value);
		return;
	}
	z80_block(17);
	register_value &= 7u;
	fm_register = key_on_register;
	write_fm2(fm_register, register_value);
}

void apply_fm_music_volume(uint8_t chip_channel, uint16_t level, uint16_t part)
{
	uint8_t channel_code = 0, effective_volume = 0, fm_register = 0, level_register = 0, operator_level = 0, operators_left = 0, part_channel = 0, patch = 0, program = 0, register_number = 0, volume_curve = 0;
	uint16_t instrument = 0, patch_levels = 0, register_list = 0;
	uint8_t saved_operators_left = 0, saved_part_channel = 0;

	z80_block(104);
	channel_code = (uint8_t)chip_channel;
	fm_channel = channel_code;
	part_channel = channel_code & 3u;
	level = ((level + 4u) & 0xFFu) | ((uint32_t)z80_read8((uint16_t)(level + 4u)) << 8);
	effective_volume = MIDI_PART(part)->effective_volume;
	volume_curve = effective_volume;
	if (effective_volume != 0u)
	{
		z80_block(20);
		volume_curve += (uint8_t)~effective_volume >> 1;
	}
	z80_block(17);
	level = multiply_bytes(volume_curve, level);
	z80_block(78);
	note_level = 0x7Fu - ((uint16_t)((uint32_t)((level >> 8) >> 1) << 8) >> 8);
	program = MIDI_PART(part)->program;
	instrument = instruments_ADDRESS;
	instrument = read_table_word(instrument, program);
	z80_block(40);
	patch = z80_read8((uint16_t)(instrument + 1u));
	patch_levels = fm_patches_ADDRESS;
	patch_levels = read_table_word(patch_levels, patch);
	z80_block(38);
	patch_levels += 0x15u;
	operators_left = 4u;
	register_list = fm_level_registers_ADDRESS;
	for (;;)
	{
		z80_block(63);
		saved_operators_left = operators_left;
		saved_part_channel = part_channel;
		register_number = SOUND_ROM8(register_list) + part_channel;
		register_list++;
		level_register = register_number;
		operator_level = z80_read8(patch_levels);
		patch_levels++;
		if (operator_level >> 7 != 0u)
		{
			z80_block(18);
			part_channel = saved_part_channel;
			operators_left = saved_operators_left - 1u;
			if (operators_left == 0u)
				break;
			z80_cycles(5);
			continue;
		}
		z80_block(38);
		operator_level += note_level;
		fm_register = level_register;
		write_fm_channel(fm_register, operator_level);
		z80_block(18);
		part_channel = saved_part_channel;
		operators_left = saved_operators_left - 1u;
		if (operators_left == 0u)
			break;
		z80_cycles(5);
	}
	z80_block(20);
}

void apply_fm_channel_volume(uint8_t chip_channel, uint16_t level, uint16_t part)
{
	uint8_t channel_code = 0, effective_volume = 0, fm_register = 0, level_register = 0, operator_level = 0, operators_left = 0, part_channel = 0, patch = 0, program = 0, register_number = 0, volume_curve = 0;
	uint16_t instrument = 0, patch_levels = 0, register_list = 0;
	uint8_t saved_operators_left = 0, saved_part_channel = 0;

	z80_block(104);
	channel_code = (uint8_t)chip_channel;
	fm_channel = channel_code;
	part_channel = channel_code & 3u;
	level = ((level + 4u) & 0xFFu) | ((uint32_t)z80_read8((uint16_t)(level + 4u)) << 8);
	effective_volume = MIDI_PART(part)->effective_volume;
	volume_curve = effective_volume;
	if (effective_volume != 0u)
	{
		z80_block(20);
		volume_curve += (uint8_t)~effective_volume >> 1;
	}
	z80_block(17);
	level = multiply_bytes(volume_curve, level);
	z80_block(78);
	note_level = 0x7Fu - ((uint16_t)((uint32_t)((level >> 8) >> 1) << 8) >> 8);
	program = MIDI_PART(part)->program;
	instrument = instruments_ADDRESS;
	instrument = read_table_word(instrument, program);
	z80_block(40);
	patch = z80_read8((uint16_t)(instrument + 1u));
	patch_levels = fm_patches_ADDRESS;
	patch_levels = read_table_word(patch_levels, patch);
	z80_block(38);
	patch_levels += 0x15u;
	operators_left = 4u;
	register_list = fm_level_registers_ADDRESS;
	for (;;)
	{
		z80_block(63);
		saved_operators_left = operators_left;
		saved_part_channel = part_channel;
		register_number = SOUND_ROM8(register_list) + part_channel;
		register_list++;
		level_register = register_number;
		operator_level = z80_read8(patch_levels);
		patch_levels++;
		if (operator_level >> 7 != 0u)
		{
			z80_block(18);
			part_channel = saved_part_channel;
			operators_left = saved_operators_left - 1u;
			if (operators_left == 0u)
				break;
			z80_cycles(5);
			continue;
		}
		z80_block(38);
		operator_level += note_level;
		fm_register = level_register;
		write_fm_channel(fm_register, operator_level);
		z80_block(18);
		part_channel = saved_part_channel;
		operators_left = saved_operators_left - 1u;
		if (operators_left == 0u)
			break;
		z80_cycles(5);
	}
	z80_block(20);
}

void load_fm_patch(uint16_t *p_channel_in_part, uint16_t *p_register_list, uint16_t *p_patch_data)
{
	uint8_t fm_register = 0, part_channel = 0, patch = 0, patch_byte = 0, patch_register = 0, register_number = 0, registers_left = 0;
	uint16_t channel_in_part = 0, patch_data = 0, register_list = 0;
	uint8_t saved_registers_left = 0, saved_part_channel = 0;

	z80_block(64);
	part_channel = fm_channel & 3u;
	patch = sample_or_patch;
	patch_data = fm_patches_ADDRESS;
	patch_data = read_table_word(patch_data, patch);
	z80_block(17);
	register_list = fm_patch_registers_ADDRESS;
	registers_left = 0x15u;
	for (;;)
	{
		z80_block(62);
		saved_registers_left = registers_left;
		saved_part_channel = part_channel;
		register_number = SOUND_ROM8(register_list) + part_channel;
		register_list++;
		patch_register = register_number;
		patch_byte = z80_read8(patch_data);
		patch_data++;
		fm_register = patch_register;
		write_fm_channel(fm_register, patch_byte);
		z80_block(18);
		part_channel = saved_part_channel;
		registers_left = saved_registers_left - 1u;
		if (registers_left == 0u)
			break;
		z80_cycles(5);
	}
	z80_block(10);
	channel_in_part = ((uint32_t)registers_left << 8) | part_channel;
	*p_channel_in_part = channel_in_part;
	*p_register_list = register_list;
	*p_patch_data = patch_data;
}
