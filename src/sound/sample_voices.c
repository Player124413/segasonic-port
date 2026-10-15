/**
 *    Name:        sample_voices.c
 *    Description: the RF5C68's sample voices: starting a sample, refilling its pages as it plays, stopping it
 *    Version:     0.1.0
 *    Created:     13-10-2026
 *    Modified:    15-10-2026
 *    Author:      Sebastian Toyos
 *
 *    SegaSonic The Hedgehog (arcade, Sega System 32): the Z80 sound program decompiled into C by
 *    tools/decompile_sound.py, with the names of sound_symbols.txt.
 */
#include "sound.h"

void start_pcm_sample(uint16_t voice)
{
	uint8_t bank_high = 0, bank_low = 0, channels_off = 0, next_wave_page = 0, pcm_register = 0, sample = 0, wave_page = 0;
	uint16_t header = 0, sample_address = 0, wave_address = 0;
	uint8_t saved_start_page = 0, saved_start_offset = 0;

	z80_block(52);
	channels_off = ~pcm_channels_on;
	pcm_register = 8u;
	write_pcm_register(pcm_register, channels_off);
	z80_block(40);
	sample = sample_or_patch;
	header = pcm_sample_headers_ADDRESS;
	header = read_table_word(header, sample);
	z80_block(293);
	bank_high = z80_read8(header);
	SAMPLE_VOICE(voice)->sample_bank_high = bank_high;
	bank_high &= 7u;
	rom_bank_high = bank_high;
	port_out(ROM_BANK_HIGH, bank_high);
	header++;
	bank_low = z80_read8(header);
	SAMPLE_VOICE(voice)->sample_bank_low = bank_low;
	rom_bank_low = bank_low;
	port_out(ROM_BANK_LOW, bank_low);
	wave_page = SAMPLE_VOICE(voice)->wave_page;
	header++;
	saved_start_page = z80_read8(header);
	saved_start_offset = 0u;
	header++;
	SAMPLE_VOICE(voice)->header_bank_step = z80_read8(header);
	header++;
	SAMPLE_VOICE(voice)->header_address_low = z80_read8(header);
	SAMPLE_VOICE(voice)->header_address_high = z80_read8((uint16_t)(header + 1u));
	sample_address = ((uint32_t)saved_start_page << 8) | saved_start_offset;
	wave_address = (uint32_t)wave_page << 8;
	copy_sample_page(&wave_address, &sample_address);
	next_wave_page = (uint8_t)(wave_address >> 8);
	z80_block(40);
	SAMPLE_VOICE(voice)->wave_page = next_wave_page;
	if (0xC0u == sample_address >> 8)
	{
		z80_block(30);
		sample_address = ROM_BANK_WINDOW;
		SAMPLE_VOICE(voice)->sample_bank_low++;
	}
	z80_block(39);
	SAMPLE_VOICE(voice)->source_page = sample_address >> 8;
}

void refill_pcm_voice(void)
{
	uint8_t bank_high = 0, bank_low = 0, four_pages = 0, wave_offset = 0, wave_page = 0;
	uint16_t progress = 0, sample_address = 0, voice = 0, voices_left_at = 0, wave_address = 0;
	uint8_t old_voices_left = 0;

	z80_block(50);
	voice = refill_voice;
	if (((SAMPLE_VOICE(voice)->flags >> 6) & 1u) == 0u)
	{
		z80_block(53);
		voice += 0x12u;
		voices_left_at = refill_voices_left_ADDRESS;
		old_voices_left = refill_voices_left;
		refill_voices_left = old_voices_left - 1u;
		if (old_voices_left != 1u)
		{
			z80_block(30);
			refill_voice = voice;
			return;
		}
		z80_cycles(5);
		z80_block(46);
		refill_voices_left = 8u;
		refill_voice = SAMPLE_VOICE_ADDRESS(0);
		return;
	}
	z80_block(105);
	progress = SAMPLE_VOICE(voice)->stream_position + SAMPLE_VOICE(voice)->pitch_step;
	if ((((progress >> 8) >> 4) & 1u) == 0u)
	{
		z80_block(25);
		four_pages = 0u;
		if ((((progress >> 8) >> 3) & 1u) != 0u)
		{
		refill_copy_pages:
			z80_block(252);
			progress = (progress & 0xFFu) | (((progress >> 8) & ~8u) << 8);
			SAMPLE_VOICE(voice)->stream_position = progress;
			z80_write8(PCM_CONTROL, (SAMPLE_VOICE(voice)->chip_channel * 2u) | 0x80u);
			bank_high = SAMPLE_VOICE(voice)->sample_bank_high & 7u;
			rom_bank_high = bank_high;
			port_out(ROM_BANK_HIGH, bank_high);
			bank_low = SAMPLE_VOICE(voice)->sample_bank_low;
			rom_bank_low = bank_low;
			port_out(ROM_BANK_LOW, bank_low);
			wave_page = SAMPLE_VOICE(voice)->wave_page;
			wave_offset = 0u;
			sample_address = (uint32_t)SAMPLE_VOICE(voice)->source_page << 8;
			if (four_pages == 0u)
			{
			refill_copy_two_pages:
				z80_block(17);
				wave_address = ((uint32_t)wave_page << 8) | wave_offset;
				copy_sample_page(&wave_address, &sample_address);
				wave_page = (uint8_t)(wave_address >> 8);
				wave_offset = (uint8_t)wave_address;
				z80_block(27);
				sample_address--;
				if (z80_read8(sample_address) == 0xFFu)
				{
					z80_cycles(5);
					stop_pcm_voice(voice);
					return;
				}
				z80_block(27);
				sample_address++;
				if (0xC0u == sample_address >> 8)
				{
					z80_block(73);
					sample_address = (sample_address & 0xFFu) | 0xA000u;
					SAMPLE_VOICE(voice)->sample_bank_low++;
					bank_low = SAMPLE_VOICE(voice)->sample_bank_low;
					rom_bank_low = bank_low;
					port_out(ROM_BANK_LOW, bank_low);
				}
				z80_block(21);
				if (0xE0u == wave_page)
				{
					z80_block(7);
					wave_page = 0xD0u;
				}
				z80_block(17);
				wave_address = ((uint32_t)wave_page << 8) | wave_offset;
				copy_sample_page(&wave_address, &sample_address);
				wave_page = (uint8_t)(wave_address >> 8);
				z80_block(27);
				sample_address--;
				if (z80_read8(sample_address) == 0xFFu)
				{
					z80_cycles(5);
					stop_pcm_voice(voice);
					return;
				}
				z80_block(27);
				sample_address++;
				if (0xC0u == sample_address >> 8)
				{
					z80_block(30);
					sample_address = ROM_BANK_WINDOW;
					SAMPLE_VOICE(voice)->sample_bank_low++;
				}
				z80_block(21);
				if (0xE0u == wave_page)
				{
					z80_block(7);
					wave_page = 0xD0u;
				}
				z80_block(38);
				SAMPLE_VOICE(voice)->wave_page = wave_page;
				SAMPLE_VOICE(voice)->source_page = sample_address >> 8;
				z80_block(53);
				voice += 0x12u;
				voices_left_at = refill_voices_left_ADDRESS;
				old_voices_left = refill_voices_left;
				refill_voices_left = old_voices_left - 1u;
				if (old_voices_left != 1u)
				{
					z80_block(30);
					refill_voice = voice;
					return;
				}
				z80_cycles(5);
				z80_block(46);
				refill_voices_left = 8u;
				refill_voice = SAMPLE_VOICE_ADDRESS(0);
				return;
			}
			z80_block(17);
			wave_address = ((uint32_t)wave_page << 8) | wave_offset;
			copy_sample_page(&wave_address, &sample_address);
			wave_page = (uint8_t)(wave_address >> 8);
			wave_offset = (uint8_t)wave_address;
			z80_block(30);
			sample_address--;
			if (z80_read8(sample_address) == 0xFFu)
			{
				stop_pcm_voice(voice);
				return;
			}
			z80_block(27);
			sample_address++;
			if (0xC0u == sample_address >> 8)
			{
				z80_block(73);
				sample_address = (sample_address & 0xFFu) | 0xA000u;
				SAMPLE_VOICE(voice)->sample_bank_low++;
				bank_low = SAMPLE_VOICE(voice)->sample_bank_low;
				rom_bank_low = bank_low;
				port_out(ROM_BANK_LOW, bank_low);
			}
			z80_block(21);
			if (0xE0u == wave_page)
			{
				z80_block(7);
				wave_page = 0xD0u;
			}
			z80_block(17);
			wave_address = ((uint32_t)wave_page << 8) | wave_offset;
			copy_sample_page(&wave_address, &sample_address);
			wave_page = (uint8_t)(wave_address >> 8);
			wave_offset = (uint8_t)wave_address;
			z80_block(27);
			sample_address--;
			if (z80_read8(sample_address) == 0xFFu)
			{
				z80_cycles(5);
				stop_pcm_voice(voice);
				return;
			}
			z80_block(27);
			sample_address++;
			if (0xC0u == sample_address >> 8)
			{
				z80_block(73);
				sample_address = (sample_address & 0xFFu) | 0xA000u;
				SAMPLE_VOICE(voice)->sample_bank_low++;
				bank_low = SAMPLE_VOICE(voice)->sample_bank_low;
				rom_bank_low = bank_low;
				port_out(ROM_BANK_LOW, bank_low);
			}
			z80_block(21);
			if (0xE0u != wave_page)
				goto refill_copy_two_pages;
			z80_block(7);
			wave_page = 0xD0u;
			goto refill_copy_two_pages;
		}
		z80_block(48);
		SAMPLE_VOICE(voice)->stream_position = progress;
		z80_block(53);
		voice += 0x12u;
		voices_left_at = refill_voices_left_ADDRESS;
		old_voices_left = refill_voices_left;
		refill_voices_left = old_voices_left - 1u;
		if (old_voices_left != 1u)
		{
			z80_block(30);
			refill_voice = voice;
			return;
		}
		z80_cycles(5);
		z80_block(46);
		refill_voices_left = 8u;
		refill_voice = SAMPLE_VOICE_ADDRESS(0);
		return;
	}
	z80_block(25);
	progress = (progress & 0xFFu) | (((progress >> 8) & 0xEFu) << 8);
	four_pages = 1u;
	goto refill_copy_pages;
}

void stop_pcm_voice(uint16_t voice)
{
	uint8_t own_order = 0, pcm_register = 0, register_value = 0, voices_left = 0;
	uint16_t other_order = 0, voice_size = 0;

	z80_block(50);
	register_value = SAMPLE_VOICE(voice)->chip_channel | 0xC0u;
	pcm_register = 7u;
	write_pcm_register(pcm_register, register_value);
	z80_block(72);
	register_value = ((register_value & 7u) * 2u) | 0x80u;
	register_value = ((uint32_t)register_value << 1) | (register_value >> 7);
	register_value = ((uint32_t)register_value << 1) | (register_value >> 7);
	register_value = ((uint32_t)register_value << 1) | (register_value >> 7);
	register_value = ((((uint32_t)register_value << 1) | (register_value >> 7)) & 0xF0u) + 0x18u;
	pcm_register = 5u;
	write_pcm_register(pcm_register, register_value);
	z80_block(227);
	SAMPLE_VOICE(voice)->flags = 0u;
	pcm_channels_on ^= pcm_channel_bits[SAMPLE_VOICE(voice)->chip_channel];
	SAMPLE_VOICE(voice)->midi_channel = 0u;
	SAMPLE_VOICE(voice)->wave_page = 0xD0u;
	AS8(SAMPLE_VOICE(voice)->stream_position) = 0xFFu;
	AT8(SAMPLE_VOICE(voice)->stream_position, 1) = 7u;
	pcm_voices_used--;
	if (SAMPLE_VOICE(voice)->allocation_order >= 0x10u)
	{
		z80_cycles(6);
		return;
	}
	z80_block(46);
	other_order = voice_0_allocation_order_ADDRESS;
	voice_size = 0x12u;
	own_order = SAMPLE_VOICE(voice)->allocation_order;
	voices_left = 8u;
	for (;;)
	{
		z80_block(17);
		if (SAMPLE_VOICE(other_order - 0x6u)->allocation_order <= own_order)
		{
			z80_block(19);
			other_order += voice_size;
			voices_left--;
			if (voices_left == 0u)
				break;
			z80_cycles(5);
			continue;
		}
		z80_block(11);
		SAMPLE_VOICE(other_order - 0x6u)->allocation_order--;
		z80_block(19);
		other_order += voice_size;
		voices_left--;
		if (voices_left == 0u)
			break;
		z80_cycles(5);
	}
	z80_block(10);
}
