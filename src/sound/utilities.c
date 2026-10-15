/**
 *    Name:        utilities.c
 *    Description: multiply_bytes (the volume scaling) and copy_sample_page (256 LDIs from the sample window to the wave memory)
 *    Version:     0.1.0
 *    Created:     13-10-2026
 *    Modified:    15-10-2026
 *    Author:      Sebastian Toyos
 *
 *    SegaSonic The Hedgehog (arcade, Sega System 32): the Z80 sound program decompiled into C by
 *    tools/decompile_sound.py, with the names of sound_symbols.txt.
 */
#include "sound.h"

uint16_t multiply_bytes(uint16_t factor, uint16_t value)
{
	uint32_t carry = 0;

	z80_block(35);
	factor &= 0xFFu;
	value &= ~0xFFu;
	carry = (int16_t)value < 0;
	value = value * 2u;
	if (carry)
	{
		z80_block(11);
		value += factor;
	}
	z80_block(21);
	carry = (int16_t)value < 0;
	value = value * 2u;
	if (carry)
	{
		z80_block(11);
		value += factor;
	}
	z80_block(21);
	carry = (int16_t)value < 0;
	value = value * 2u;
	if (carry)
	{
		z80_block(11);
		value += factor;
	}
	z80_block(21);
	carry = (int16_t)value < 0;
	value = value * 2u;
	if (carry)
	{
		z80_block(11);
		value += factor;
	}
	z80_block(21);
	carry = (int16_t)value < 0;
	value = value * 2u;
	if (carry)
	{
		z80_block(11);
		value += factor;
	}
	z80_block(21);
	carry = (int16_t)value < 0;
	value = value * 2u;
	if (carry)
	{
		z80_block(11);
		value += factor;
	}
	z80_block(21);
	carry = (int16_t)value < 0;
	value = value * 2u;
	if (carry)
	{
		z80_block(11);
		value += factor;
	}
	z80_block(16);
	carry = (int16_t)value < 0;
	value = value * 2u;
	if (carry == 0u)
	{
		z80_cycles(6);
		return value;
	}
	z80_block(21);
	value += factor;
	return value;
}

void copy_sample_page(uint16_t *p_wave_address, uint16_t *p_sample_address)
{
	uint16_t wave_address = (uint16_t)*p_wave_address;
	uint16_t sample_address = (uint16_t)*p_sample_address;

	z80_block(4106);
	for (uint32_t repeat = 0; repeat < 256u; repeat++)
	{
		z80_write8(wave_address, z80_read8(sample_address));
		sample_address++;
		wave_address++;
	}
	*p_wave_address = wave_address;
	*p_sample_address = sample_address;
}
