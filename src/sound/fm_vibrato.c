/**
 *    Name:        fm_vibrato.c
 *    Description: the FM vibrato the main loop steps; an unused fixed FM frequency
 *    Version:     0.1.0
 *    Created:     13-10-2026
 *    Modified:    15-10-2026
 *    Author:      Sebastian Toyos
 *
 *    SegaSonic The Hedgehog (arcade, Sega System 32): the Z80 sound program decompiled into C by
 *    tools/decompile_sound.py, with the names of sound_symbols.txt.
 */
#include "sound.h"

void step_fm_vibrato(uint16_t voice)
{
	uint8_t center_high = 0, center_low = 0, fall_block = 0, fall_limit = 0, fm_register = 0, rise_block = 0, rise_limit = 0, value = 0;
	uint16_t fnumber = 0, frequency = 0;
	uint8_t saved_center_high = 0, saved_center_low = 0;
	uint16_t word_copy = 0;
	uint8_t fall_limit_bits = 0;
	uint16_t fall_floor = 0, fall_fnumber = 0, center_for_fall = 0, rise_ceiling = 0, rise_fnumber = 0, center_for_rise = 0;

	z80_block(33);
	if (FM_VOICE(voice)->vibrato_delay != 0u)
	{
		z80_block(33);
		FM_VOICE(voice)->vibrato_delay--;
		return;
	}
	z80_block(33);
	if (FM_VOICE(voice)->vibrato_wait != 0u)
	{
		z80_block(33);
		FM_VOICE(voice)->vibrato_wait--;
		return;
	}
	z80_block(305);
	FM_VOICE(voice)->vibrato_wait = FM_VOICE(voice)->vibrato_rate;
	fm_channel = FM_VOICE(voice)->chip_channel;
	value = FM_VOICE(voice)->note;
	fnumber = fm_fnum_tables_ADDRESS + (uint8_t)((value & 0xFu) * 2u);
	frequency = pitch_bend;
	fnumber = SOUND_ROM16(fnumber) + frequency + frequency;
	fnumber = z80_read16(fnumber);
	vibrato_center = fnumber;
	fnumber = (fnumber & 0xFFu) | ((uint32_t)(uint8_t)(((value & 0x70u) >> 1) + (fnumber >> 8)) << 8);
	if ((FM_VOICE(voice)->vibrato_phase & 1u) != 0u)
	{
		z80_block(214);
		frequency = fnumber;
		fnumber = FM_VOICE(voice)->vibrato_offset - FM_VOICE(voice)->vibrato_fall;
		FM_VOICE(voice)->vibrato_offset = fnumber;
		fnumber += (frequency & 0xFFu) | (((frequency >> 8) & 7u) << 8);
		fnumber = (fnumber & 0xFFu) | ((uint32_t)(uint8_t)(((frequency >> 8) & ~7u) + (fnumber >> 8)) << 8);
		if ((FM_VOICE(voice)->vibrato_range & 0x70u) == 0u)
		{
		vibrato_fall_last_octave:
			z80_block(167);
			FM_VOICE(voice)->vibrato_range &= 0xFu;
			word_copy = fnumber;
			frequency = word_copy;
			fnumber = vibrato_center - (((FM_VOICE(voice)->vibrato_range & 7u) << 8) | FM_VOICE(voice)->vibrato_depth);
			if (((fnumber >> 8) & 0xC0u) != 0u)
			{
				z80_block(83);
				word_copy = frequency;
				fnumber = frequency;
				if (word_copy > 0u)
				{
					z80_block(97);
					FM_VOICE(voice)->vibrato_range += FM_VOICE(voice)->vibrato_phase & 0x70u;
					FM_VOICE(voice)->vibrato_phase--;
				}
			}
			else
			{
				z80_block(69);
				word_copy = frequency;
				fall_floor = fnumber;
				fnumber = frequency;
				if ((uint16_t)((((word_copy >> 8) & 7u) << 8) | (uint8_t)word_copy) <= fall_floor)
				{
					z80_block(10);
					z80_block(97);
					FM_VOICE(voice)->vibrato_range += FM_VOICE(voice)->vibrato_phase & 0x70u;
					FM_VOICE(voice)->vibrato_phase--;
					z80_block(94);
					fnumber = (fnumber & 0xFFu) | ((uint32_t)(uint8_t)((FM_VOICE(voice)->vibrato_block & ~7u) + (fnumber >> 8)) << 8);
					frequency = (fm_channel & 3u) << 8;
					value = (uint8_t)(fnumber >> 8);
					fm_register = 0xA4u | (frequency >> 8);
					write_fm_channel(fm_register, value);
					z80_block(36);
					value = (uint8_t)fnumber;
					fm_register = 0xA0u | (frequency >> 8);
					write_fm_channel(fm_register, value);
					z80_block(10);
					return;
				}
			}
		}
		else
		{
			z80_block(83);
			word_copy = fnumber;
			fall_fnumber = (fnumber & 0xFFu) | (((fnumber >> 8) & 7u) << 8);
			fnumber = word_copy;
			if (fall_fnumber < 0x269u)
			{
				z80_block(304);
				frequency = fnumber;
				center_for_fall = vibrato_center;
				center_high = (uint8_t)(center_for_fall >> 8);
				center_low = (uint8_t)center_for_fall;
				fnumber = 0x4CFu - (((uint32_t)center_high << 8) | center_low);
				FM_VOICE(voice)->vibrato_offset = fnumber;
				saved_center_high = center_high;
				saved_center_low = center_low;
				fnumber = (((uint32_t)saved_center_high << 8) | saved_center_low) + FM_VOICE(voice)->vibrato_offset;
				fnumber = (fnumber & 0xFFu) | ((uint32_t)(uint8_t)(((frequency >> 8) & 0x38u) + (fnumber >> 8)) << 8);
				frequency = ((fnumber >> 8) & 0x38u) << 8;
				fall_block = (FM_VOICE(voice)->vibrato_block & ~7u) + (frequency >> 8);
				value = 0u - ((FM_VOICE(voice)->vibrato_range & 0x70u) >> 1) + (frequency >> 8);
				fall_limit = value;
				fall_limit_bits = value & ~7u;
				value = fall_limit_bits;
				if (fall_limit_bits != 0u)
				{
					z80_block(17);
					if ((value & 0xC0u) != 0u)
						goto vibrato_fall_block_check;
					z80_block(18);
					if (fall_block <= fall_limit)
						goto vibrato_fall_into_last_octave;
					z80_block(10);
					z80_block(59);
					FM_VOICE(voice)->vibrato_block -= 8u;
					z80_block(94);
					fnumber = (fnumber & 0xFFu) | ((uint32_t)(uint8_t)((FM_VOICE(voice)->vibrato_block & ~7u) + (fnumber >> 8)) << 8);
					frequency = (fm_channel & 3u) << 8;
					value = (uint8_t)(fnumber >> 8);
					fm_register = 0xA4u | (frequency >> 8);
					write_fm_channel(fm_register, value);
					z80_block(36);
					value = (uint8_t)fnumber;
					fm_register = 0xA0u | (frequency >> 8);
					write_fm_channel(fm_register, value);
					z80_block(10);
					return;
				}
			vibrato_fall_block_check:
				z80_block(21);
				if (fall_block <= 8u)
				{
				vibrato_fall_into_last_octave:
					z80_block(49);
					FM_VOICE(voice)->vibrato_block -= 8u;
					goto vibrato_fall_last_octave;
				}
				z80_block(59);
				FM_VOICE(voice)->vibrato_block -= 8u;
			}
		}
	}
	else
	{
		z80_block(164);
		frequency = fnumber;
		fnumber = FM_VOICE(voice)->vibrato_offset + FM_VOICE(voice)->vibrato_rise;
		FM_VOICE(voice)->vibrato_offset = fnumber;
		fnumber += frequency;
		if ((FM_VOICE(voice)->vibrato_range & 0x70u) == 0u)
		{
		vibrato_rise_last_octave:
			z80_block(170);
			FM_VOICE(voice)->vibrato_range &= 0xFu;
			word_copy = fnumber;
			frequency = word_copy;
			fnumber = vibrato_center + (((FM_VOICE(voice)->vibrato_range & 7u) << 8) | FM_VOICE(voice)->vibrato_depth);
			if (((fnumber >> 8) & 0xFu) >= 4u)
			{
				z80_block(7);
				fnumber = (fnumber & 0xFFu) | 0x500u;
			}
			else
				z80_block(10);
			z80_block(84);
			word_copy = frequency;
			rise_ceiling = (fnumber & 0xFFu) | (((fnumber >> 8) & 7u) << 8);
			fnumber = frequency;
			if (rise_ceiling < (uint16_t)((((word_copy >> 8) & 7u) << 8) | (uint8_t)word_copy))
			{
				z80_block(97);
				FM_VOICE(voice)->vibrato_range += FM_VOICE(voice)->vibrato_phase & 0x70u;
				FM_VOICE(voice)->vibrato_phase++;
			}
		}
		else
		{
			z80_block(83);
			word_copy = fnumber;
			rise_fnumber = (fnumber & 0xFFu) | (((fnumber >> 8) & 7u) << 8);
			fnumber = word_copy;
			if (rise_fnumber >= 0x4CEu)
			{
				z80_block(314);
				frequency = fnumber;
				center_for_rise = vibrato_center;
				center_high = (uint8_t)(center_for_rise >> 8);
				center_low = (uint8_t)center_for_rise;
				fnumber = 0x269u - (((uint32_t)center_high << 8) | center_low);
				FM_VOICE(voice)->vibrato_offset = fnumber;
				saved_center_high = center_high;
				saved_center_low = center_low;
				fnumber = (((uint32_t)saved_center_high << 8) | saved_center_low) + FM_VOICE(voice)->vibrato_offset;
				fnumber = (fnumber & 0xFFu) | ((uint32_t)(uint8_t)(((frequency >> 8) & 0x38u) + (fnumber >> 8)) << 8);
				frequency = ((fnumber >> 8) & 0x38u) << 8;
				rise_block = ((FM_VOICE(voice)->vibrato_block & ~7u) + (frequency >> 8)) & 0xF8u;
				value = ((FM_VOICE(voice)->vibrato_range & 0x70u) >> 1) + (frequency >> 8) - 8u;
				rise_limit = value;
				if (value >= 0x30u)
				{
					z80_block(7);
					rise_limit = 0x28u;
				}
				z80_block(21);
				if ((rise_limit & 0xC0u) == 0u)
				{
					z80_block(18);
					if (rise_limit < rise_block)
						goto vibrato_rise_into_last_octave;
					z80_block(10);
					z80_block(45);
					FM_VOICE(voice)->vibrato_block += 8u;
					z80_block(94);
					fnumber = (fnumber & 0xFFu) | ((uint32_t)(uint8_t)((FM_VOICE(voice)->vibrato_block & ~7u) + (fnumber >> 8)) << 8);
					frequency = (fm_channel & 3u) << 8;
					value = (uint8_t)(fnumber >> 8);
					fm_register = 0xA4u | (frequency >> 8);
					write_fm_channel(fm_register, value);
					z80_block(36);
					value = (uint8_t)fnumber;
					fm_register = 0xA0u | (frequency >> 8);
					write_fm_channel(fm_register, value);
					z80_block(10);
					return;
				}
				z80_block(25);
				if ((uint8_t)(rise_limit + 8u) < rise_block)
				{
				vibrato_rise_into_last_octave:
					z80_block(45);
					FM_VOICE(voice)->vibrato_block += 8u;
					goto vibrato_rise_last_octave;
				}
				z80_block(45);
				FM_VOICE(voice)->vibrato_block += 8u;
			}
		}
	}
	z80_block(94);
	fnumber = (fnumber & 0xFFu) | ((uint32_t)(uint8_t)((FM_VOICE(voice)->vibrato_block & ~7u) + (fnumber >> 8)) << 8);
	frequency = (fm_channel & 3u) << 8;
	value = (uint8_t)(fnumber >> 8);
	fm_register = 0xA4u | (frequency >> 8);
	write_fm_channel(fm_register, value);
	z80_block(36);
	value = (uint8_t)fnumber;
	fm_register = 0xA0u | (frequency >> 8);
	write_fm_channel(fm_register, value);
	z80_block(10);
}

uint16_t load_fixed_fm_frequency(void)
{
	uint16_t fixed_frequency = 0;

	z80_block(24);
	fixed_frequency = 0x3AEu;
	return fixed_frequency;
}
