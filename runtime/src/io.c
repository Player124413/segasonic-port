/**
 *    Name:        io.c
 *    Description: 315-5296 I/O chip, 93C46 serial EEPROM and uPD4701 trackball counters of the System 32 board
 *    Version:     0.1.0
 *    Created:     11-10-2026
 *    Modified:    11-10-2026
 *    Author:      Sebastian Toyos
 *
 *    license:BSD-3-Clause
 *    Behavior follows MAME sega/315_5296.cpp (hap), machine/eepromser.cpp and machine/eeprom.cpp
 *    (Aaron Giles), machine/upd4701.cpp, and the wiring in sega/segas32.cpp.
 */
#include "internal.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* ---- 93C46, 16-bit organization: 6 address bits, 16 data bits ---- */

enum
{
	EEPROM_IN_RESET,
	EEPROM_WAIT_FOR_START_BIT,
	EEPROM_WAIT_FOR_COMMAND,
	EEPROM_READING_DATA,
	EEPROM_WAIT_FOR_DATA,
	EEPROM_WAIT_FOR_COMPLETION
};

enum
{
	EEPROM_READ,
	EEPROM_WRITE,
	EEPROM_ERASE,
	EEPROM_LOCK,
	EEPROM_UNLOCK,
	EEPROM_WRITE_ALL,
	EEPROM_ERASE_ALL
};

#define EEPROM_ADDRESS_BITS 6
#define EEPROM_DATA_BITS    16

static uint64_t microseconds(uint32_t value)
{
	return (uint64_t)value * S32_CPU_CLOCK / 1000000u;
}

static bool eeprom_ready(void)
{
	return s32_board.cycles >= s32_board.eeprom.busy_until;
}

static void eeprom_busy(uint32_t duration_us)
{
	/* with frame timing the program takes no time, so it could never see a write end */
	s32_board.eeprom.busy_until = s32_board.cycles + (s32_board.frame_timing ? 0 : microseconds(duration_us));
	s32_board.eeprom.dirty = true;
}

/* With S32_LOG_EEPROM set in the environment, every command the chip takes goes to stderr (to compare
   two runs of the program) */
static void eeprom_log(const char *what, int address, uint32_t value)
{
	static int enabled = -1;

	if (enabled < 0)
		enabled = getenv("S32_LOG_EEPROM") != NULL;
	if (enabled)
		fprintf(stderr, "eeprom %s %02X %04X frame %llu\n", what, address, value,
		        (unsigned long long)s32_board.frame);
}

static void eeprom_execute_command(void)
{
	s32_eeprom *chip = &s32_board.eeprom;
	const uint32_t opcode = chip->command_bits >> EEPROM_ADDRESS_BITS;

	chip->address = (int)(chip->command_bits & ((1u << EEPROM_ADDRESS_BITS) - 1));
	chip->bits = 0;
	switch (opcode)
	{
		case 0:
		{
			static const int extended[4] = { EEPROM_LOCK, EEPROM_WRITE_ALL, EEPROM_ERASE_ALL, EEPROM_UNLOCK };
			chip->command = extended[chip->address >> (EEPROM_ADDRESS_BITS - 2)];
			chip->address = 0;
			break;
		}
		case 1: chip->command = EEPROM_WRITE; break;
		case 2: chip->command = EEPROM_READ; break;
		default: chip->command = EEPROM_ERASE; break;
	}
	eeprom_log("command", chip->address, (uint32_t)chip->command);

	switch (chip->command)
	{
		case EEPROM_READ:
			chip->shift = 0;    /* the dummy 0 bit before the data */
			chip->state = EEPROM_READING_DATA;
			break;
		case EEPROM_WRITE:
		case EEPROM_WRITE_ALL:
			chip->shift = 0;
			chip->state = EEPROM_WAIT_FOR_DATA;
			break;
		case EEPROM_ERASE:
			if (chip->locked)
			{
				chip->state = EEPROM_IN_RESET;
				break;
			}
			chip->data[chip->address] = 0xffff;
			eeprom_busy(1000);
			chip->state = EEPROM_WAIT_FOR_COMPLETION;
			break;
		case EEPROM_ERASE_ALL:
			if (chip->locked)
			{
				chip->state = EEPROM_IN_RESET;
				break;
			}
			memset(chip->data, 0xff, sizeof(chip->data));
			eeprom_busy(8000);
			chip->state = EEPROM_WAIT_FOR_COMPLETION;
			break;
		case EEPROM_LOCK:
			chip->locked = true;
			chip->state = EEPROM_IN_RESET;
			break;
		default:
			chip->locked = false;
			chip->state = EEPROM_IN_RESET;
			break;
	}
}

static void eeprom_execute_write(void)
{
	s32_eeprom *chip = &s32_board.eeprom;

	if (chip->locked)
	{
		chip->state = EEPROM_IN_RESET;
		return;
	}
	eeprom_log(chip->locked ? "write-locked" : "write", chip->address, chip->shift & 0xffff);
	if (chip->command == EEPROM_WRITE)
	{
		chip->data[chip->address] = (uint16_t)chip->shift;
		eeprom_busy(1750);
	}
	else
	{
		/* write all assumes a previous erase all, so it can only clear bits */
		for (int index = 0; index < S32_EEPROM_WORDS; index++)
			chip->data[index] &= (uint16_t)chip->shift;
		eeprom_busy(8000);
	}
	chip->state = EEPROM_WAIT_FOR_COMPLETION;
}

static void eeprom_clock_rising(bool cs_rose_now)
{
	s32_eeprom *chip = &s32_board.eeprom;

	switch (chip->state)
	{
		case EEPROM_WAIT_FOR_START_BIT:
			/* a clock edge written together with the CS edge is ignored */
			if (chip->di && eeprom_ready() && !cs_rose_now)
			{
				chip->command_bits = 0;
				chip->bits = 0;
				chip->state = EEPROM_WAIT_FOR_COMMAND;
			}
			break;
		case EEPROM_WAIT_FOR_COMMAND:
			chip->command_bits = (chip->command_bits << 1) | chip->di;
			if (++chip->bits == 2 + EEPROM_ADDRESS_BITS)
				eeprom_execute_command();
			break;
		case EEPROM_READING_DATA:
			if (chip->bits++ == 0)
				chip->shift = (uint32_t)chip->data[chip->address] << (32 - EEPROM_DATA_BITS);
			else
				chip->shift = (chip->shift << 1) | 1;
			break;
		case EEPROM_WAIT_FOR_DATA:
			chip->shift = (chip->shift << 1) | chip->di;
			if (++chip->bits == EEPROM_DATA_BITS)
				eeprom_execute_write();
			break;
		default:
			break;
	}
}

static void eeprom_write_lines(bool di, bool cs, bool clk)
{
	s32_eeprom *chip = &s32_board.eeprom;
	bool cs_rose_now = false;

	/* same order as MAME's port D callbacks: DI, then CS, then CLK */
	chip->di = di;
	if (cs != chip->cs)
	{
		chip->cs = cs;
		if (cs)
		{
			cs_rose_now = true;
			if (chip->state == EEPROM_IN_RESET)
				chip->state = EEPROM_WAIT_FOR_START_BIT;
		}
		else
			chip->state = EEPROM_IN_RESET;
	}
	if (clk != chip->clk)
	{
		chip->clk = clk;
		if (clk && chip->cs)
			eeprom_clock_rising(cs_rose_now);
	}
}

static int eeprom_data_out(void)
{
	const s32_eeprom *chip = &s32_board.eeprom;

	if (chip->state == EEPROM_WAIT_FOR_START_BIT)
		return eeprom_ready() ? 1 : 0;     /* DO doubles as READY/BUSY */
	if (chip->state == EEPROM_READING_DATA)
		return (chip->shift & 0x80000000u) ? 1 : 0;
	return 1;                              /* tristated, pulled up */
}

/* ---- uPD4701 trackball counters ---- */

static uint8_t trackball_read(int which, uint32_t offset)
{
	/* offset bit 1 selects Y, bit 0 the upper byte; no switches are wired */
	const uint16_t position = (offset & 2) ? s32_board.input.trackball_y[which] : s32_board.input.trackball_x[which];
	const uint16_t start = (offset & 2) ? s32_board.trackball_start_y[which] : s32_board.trackball_start_x[which];
	const uint16_t counter = (uint16_t)((position - start) & 0xfff);

	return (offset & 1) ? (uint8_t)(counter >> 8) : (uint8_t)counter;
}

static void trackball_reset(int which)
{
	s32_board.trackball_start_x[which] = s32_board.input.trackball_x[which];
	s32_board.trackball_start_y[which] = s32_board.input.trackball_y[which];
}

/* ---- 315-5296 ---- */

static uint8_t io_port_input(int port)
{
	switch (port)
	{
		case 0: return s32_board.input.port_a;
		case 1: return s32_board.input.port_b;
		case 2: return s32_board.input.port_c;
		case 4: return s32_board.input.port_e;
		case 5: return (uint8_t)((s32_board.input.port_f & 0x7f) | (eeprom_data_out() << 7));
		default: return 0xff;
	}
}

static void io_port_output(int port, uint8_t data)
{
	switch (port)
	{
		case 3:    /* port D: coin counters, EEPROM DI (bit 7), CS (bit 5), CLK (bit 6) */
			eeprom_write_lines((data >> 7) & 1, (data >> 5) & 1, (data >> 6) & 1);
			break;
		case 7:    /* port H: external tile bank */
			s32_board.tilebank_external = data;
			break;
		default:
			break;
	}
}

static uint8_t io_chip_read(uint32_t offset)
{
	switch (offset)
	{
		case 0x0: case 0x1: case 0x2: case 0x3: case 0x4: case 0x5: case 0x6: case 0x7:
			if (s32_board.io_direction & (1u << offset))
				return s32_board.io_latch[offset];
			return io_port_input((int)offset);
		case 0x8: return 'S';
		case 0x9: return 'E';
		case 0xa: return 'G';
		case 0xb: return 'A';
		case 0xc: case 0xe: return s32_board.io_cnt;
		default: return s32_board.io_direction;
	}
}

static void io_chip_write(uint32_t offset, uint8_t data)
{
	switch (offset)
	{
		case 0x0: case 0x1: case 0x2: case 0x3: case 0x4: case 0x5: case 0x6: case 0x7:
			if (s32_board.io_direction & (1u << offset))
				io_port_output((int)offset, data);
			s32_board.io_latch[offset] = data;
			break;
		case 0xe:   /* CNT: bit 1 enables the display, bit 2 releases the sound CPU reset */
			if (((s32_board.io_cnt ^ data) & 4) && s32_board.sound_reset_changed)
				s32_board.sound_reset_changed(!(data & 4));
			s32_board.io_cnt = data;
			break;
		case 0xf:
			for (int port = 0; port < 8; port++)
				if ((s32_board.io_direction ^ data) & (1u << port))
					io_port_output(port, (data & (1u << port)) ? s32_board.io_latch[port] : 0);
			s32_board.io_direction = data;
			break;
		default:
			break;
	}
}

uint8_t s32_io_read(uint32_t offset)
{
	if (offset < 0x20)
		return io_chip_read((offset >> 1) & 0xf);
	if (offset >= 0x40 && offset < 0x58)
		return trackball_read((int)((offset - 0x40) >> 3), (offset >> 1) & 3);
	return 0xff;
}

void s32_io_write(uint32_t offset, uint8_t data)
{
	if (offset < 0x20)
		io_chip_write((offset >> 1) & 0xf, data);
	else if (offset >= 0x40 && offset < 0x58)
		trackball_reset((int)((offset - 0x40) >> 3));
}

void s32_io_reset(void)
{
	/* all ports become inputs and the output latches clear */
	s32_board.io_direction = 0;
	s32_board.io_cnt = 0;
	if (s32_board.sound_reset_changed)
		s32_board.sound_reset_changed(true);
	memset(s32_board.io_latch, 0, sizeof(s32_board.io_latch));
	for (int port = 0; port < 8; port++)
		io_port_output(port, 0);
}
