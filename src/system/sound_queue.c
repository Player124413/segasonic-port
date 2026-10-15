/**
 *    Name:        system/sound_queue.c
 *    Description: the sound queue: init_sound, play_sound, queue_sound; then load_stage_palette and split_frame_count
 *    Version:     0.1.0
 *    Created:     12-10-2026
 *    Modified:    15-10-2026
 *    Author:      Sebastian Toyos
 *
 *    SegaSonic The Hedgehog (arcade, Sega System 32): the game's V60 program decompiled into C by
 *    tools/decompile.py, with the names of symbols.txt.
 */
#include "game.h"

uint32_t init_sound(void)
{
	uint32_t sound_hold = 0;
	uint8_t cnt_bits = 0, port_d_bits = 0;

	sound_queue_read = 0u;
	sound_queue_write = 1u;
	sound_queue[0] = 0x96u;
	sound_queue[1] = 0u;
	cnt_bits = mem_read8(io_cnt);
	mem_write8(io_cnt, cnt_bits | 4u);
	port_d_bits = mem_read8(io_port_d);
	mem_write8(io_port_d, port_d_bits & ~8u);
	sound_hold = 0u;
	mem_write8(sound_cpu_hold, 0u);
	return sound_hold;
}

void set_port_d_bit_3_if_sound_allowed(void)
{
	uint8_t port_d_bits = 0;

	if (unused_coin_byte != 0u || game_in_progress != 0u || setting_advertise_sound != 0u)
	{
		port_d_bits = mem_read8(io_port_d);
		mem_write8(io_port_d, port_d_bits | 8u);
	}
}

void queue_coin_sound(void)
{
	uint8_t sound_id = 0;
	uint32_t ignored = 0;
	uint8_t port_d_bits = 0;

	port_d_bits = mem_read8(io_port_d);
	mem_write8(io_port_d, port_d_bits | 8u);
	sound_id = 0x7Fu;
	queue_sound(sound_id, &ignored, &ignored);
}

void play_sound(uint8_t sound_id, uint32_t *p_write_index, uint32_t *p_write_index_address)
{
	uint32_t write_index = *p_write_index;
	uint32_t write_index_address = *p_write_index_address;
	uint32_t r1 = 0, r2 = 0, mask_bit = 0, mask_index = 0;

	if (game_in_progress != 0u)
	{
		queue_sound(sound_id, &r1, &r2);
		*p_write_index = r1;
		*p_write_index_address = r2;
		return;
	}
	if (setting_advertise_sound == 0u)
		return;
	mask_index = (uint8_t)sound_id;
	mask_bit = mask_index & 0x1Fu;
	mask_index = mask_index >> 5;
	if ((attract_sound_mask[mask_index] & shl32(1u, (int)(mask_bit))) == 0u)
	{
		*p_write_index = mask_bit;
		*p_write_index_address = mask_index;
		return;
	}
	queue_sound(sound_id, &r1, &r2);
	*p_write_index = r1;
	*p_write_index_address = r2;
}

void queue_sound(uint8_t sound_id, uint32_t *p_write_index, uint32_t *p_write_index_address)
{
	uint32_t write_index = 0, write_index_address = 0;

	write_index = sound_queue_write;
	sound_queue[write_index] = sound_id;
	write_index = (uint8_t)(write_index + 1u);
	write_index = write_index & 0x1Fu;
	write_index_address = sound_queue_write_ADDRESS;
	sound_queue_write = write_index;
	*p_write_index = write_index;
	*p_write_index_address = write_index_address;
}

void load_stage_palette(uint32_t *p_palette_record, uint32_t *p_source, uint32_t *p_dest)
{
	uint32_t dest = 0, halfword_count = 0, palette_record = 0, source = 0;

	palette_record = stage_palettes_ADDRESS + ((uint32_t)stage << 3);
	source = ROM32(palette_record);
	palette_record += 4u;
	halfword_count = ROM32(palette_record);
	dest = PALETTE_RAM + 0xB800u;
	move_halfwords(PALETTE_RAM + 0xB800u, source, halfword_count);
	*p_palette_record = palette_record;
	*p_source = source;
	*p_dest = dest;
}

void split_frame_count(uint32_t frames, uint32_t frames_high, uint32_t *p_seconds, uint32_t *p_minutes, uint32_t *p_hours, uint32_t *p_days)
{
	uint32_t days = 0, hours = 0, hours_dividend = 0, hours_dividend_high = 0, minutes = 0, minutes_dividend = 0, minutes_dividend_high = 0, seconds = 0, seconds_dividend = 0, seconds_dividend_high = 0;

	divx_pair(0x4E9FA0u, &frames, &frames_high);
	days = frames;
	hours_dividend = frames_high;
	hours_dividend_high = 0u;
	divx_pair(0x346A7u, &hours_dividend, &hours_dividend_high);
	hours = hours_dividend;
	minutes_dividend = hours_dividend_high;
	minutes_dividend_high = 0u;
	divx_pair(0xDFBu, &minutes_dividend, &minutes_dividend_high);
	minutes = minutes_dividend;
	seconds_dividend = minutes_dividend_high;
	seconds_dividend_high = 0u;
	divx_pair(0x3Cu, &seconds_dividend, &seconds_dividend_high);
	seconds = seconds_dividend;
	*p_seconds = seconds;
	*p_minutes = minutes;
	*p_hours = hours;
	*p_days = days;
}
