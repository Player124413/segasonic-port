/**
 *    Name:        sound.c
 *    Description: the Z80's side of the System 32 sound board: its memory, its I/O ports and its
 *                 interrupt controller
 *    Version:     0.1.0
 *    Created:     13-10-2026
 *    Modified:    13-10-2026
 *    Author:      Sebastian Toyos
 *
 *    license:BSD-3-Clause
 *    Behavior follows MAME's sega/segas32.cpp (Aaron Giles): the sound CPU's maps and handlers.
 *    MAME's terms are in tools/reference/mame/LICENSE.
 *
 *    Memory: 0x0000-0x9FFF the start of the sound ROM; 0xA000-0xBFFF an 8 KB page of it, picked by the
 *    bank ports; 0xC000-0xDFFF the RF5C68 (its registers, written only, then a 4 KB window on its wave
 *    memory); 0xE000-0xFFFF the RAM it shares with the V60 (0x700000 there).
 *    Ports: 0x80-0x8F and 0x90-0x9F the two YM3438; 0xA0-0xAF and 0xB0-0xBF the bank; 0xC0-0xCF
 *    acknowledge interrupts (odd ports) and interrupt the V60 (bit 2); 0xD0-0xD7 map the inputs to the
 *    vectors (0xD0-0xD2) and mask them (0xD3); 0xF1 a byte that reads back.
 */
#include "internal.h"

/* The Z80 interrupt line follows the highest vector with an unmasked input pending; vector N puts
   2 x N on the bus */
static void update_sound_irq(void)
{
	const uint8_t pending = s32_board.sound_irq_input & ~s32_board.sound_irq_control[3] & 0x07;
	int vector = 0;

	while (vector < 3 && !(pending & (1u << vector)))
		vector++;
	if (s32_board.sound_irq_changed)
		s32_board.sound_irq_changed(vector < 3, (uint8_t)(2 * (vector < 3 ? vector : 0)));
}

void s32_sound_signal_irq(int input)
{
	for (int vector = 0; vector < 3; vector++)
		if (s32_board.sound_irq_control[vector] == input)
			s32_board.sound_irq_input |= (uint8_t)(1u << vector);
	update_sound_irq();
}

void s32_sound_clear_irq(int input)
{
	for (int vector = 0; vector < 3; vector++)
		if (s32_board.sound_irq_control[vector] == input)
			s32_board.sound_irq_input &= (uint8_t)~(1u << vector);
	update_sound_irq();
}

uint8_t s32_z80_read(uint16_t address)
{
	if (address < 0xa000)
		return s32_board.sound_rom[address];
	if (address < 0xc000)
		return s32_board.sound_rom[((uint32_t)s32_board.sound_bank * 0x2000 + (address & 0x1fff)) % S32_SOUND_ROM_SIZE];
	if (address < 0xd000)
		return 0;    /* the RF5C68's registers read as nothing */
	if (address < 0xe000)
		return s32_board.chips.pcm_memory_read(address & 0xfff);
	return s32_board.shared_ram[address & 0x1fff];
}

void s32_z80_write(uint16_t address, uint8_t data)
{
	if (address < 0xc000)
		return;
	if (address < 0xd000)
	{
		/* registers 0-8, mirrored every 16 bytes */
		if ((address & 0x0f) <= 8)
			s32_board.chips.pcm_write(address & 0x0f, data);
		return;
	}
	if (address < 0xe000)
		s32_board.chips.pcm_memory_write(address & 0xfff, data);
	else
		s32_board.shared_ram[address & 0x1fff] = data;
}

uint8_t s32_z80_in(uint8_t port)
{
	switch (port & 0xf0)
	{
		case 0x80: return s32_board.chips.fm_read(0, port & 3);
		case 0x90: return s32_board.chips.fm_read(1, port & 3);
		default: return port == 0xf1 ? s32_board.sound_spare : 0xff;
	}
}

void s32_z80_out(uint8_t port, uint8_t data)
{
	switch (port & 0xf0)
	{
		case 0x80:
			s32_board.chips.fm_write(0, port & 3, data);
			break;
		case 0x90:
			s32_board.chips.fm_write(1, port & 3, data);
			break;
		case 0xa0:
			s32_board.sound_bank = (uint16_t)((s32_board.sound_bank & ~0x3f) | (data & 0x3f));
			break;
		case 0xb0:
			s32_board.sound_bank = (uint16_t)((s32_board.sound_bank & 0x3f) | (data & 0x04) << 4 | (data & 0x03) << 7);
			break;
		case 0xc0:
			if (port & 1)
			{
				s32_board.sound_irq_input &= data;
				update_sound_irq();
			}
			if (port & 4)
				s32_signal_irq(S32_IRQ_SOUND);
			break;
		case 0xd0:
			if (!(port & 8))
			{
				s32_board.sound_irq_control[port & 3] = data;
				update_sound_irq();
			}
			break;
		default:
			if (port == 0xf1)
				s32_board.sound_spare = data;
			break;
	}
}
