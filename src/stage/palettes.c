/**
 *    Name:        stage/palettes.c
 *    Description: the palettes animated per stage, the palette sets and the backdrop colour
 *    Version:     0.1.0
 *    Created:     12-10-2026
 *    Modified:    15-10-2026
 *    Author:      Sebastian Toyos
 *
 *    SegaSonic The Hedgehog (arcade, Sega System 32): the game's V60 program decompiled into C by
 *    tools/decompile.py, with the names of symbols.txt.
 */
#include "game.h"

void animate_stage_palette(uint32_t *p_copy_offset, uint32_t *p_copy_entry, uint32_t *p_palette_count, uint32_t *p_cycle, uint32_t *p_source, uint32_t *p_dest)
{
	uint32_t copy_entry = *p_copy_entry;
	uint32_t palette_count = *p_palette_count;
	uint32_t cycle = *p_cycle;
	uint32_t source = *p_source;
	uint32_t dest = *p_dest;
	uint32_t copy_offset = 0;

	switch (stage)
	{
	case 0:
		skip_stage_palette_animation();
		break;
	case 1:
		skip_stage_palette_animation();
		break;
	case 2:
		skip_stage_palette_animation();
		break;
	case 3:
		skip_stage_palette_animation();
		break;
	case 4:
		animate_palette_stage_04(&copy_offset, &copy_entry, &palette_count, &cycle, &source, &dest);
		break;
	case 5:
		skip_stage_palette_animation();
		break;
	case 6:
		skip_stage_palette_animation();
		break;
	case 7:
		skip_stage_palette_animation();
		break;
	case 8:
		skip_stage_palette_animation();
		break;
	case 9:
		animate_palette_stage_09(&copy_offset, &copy_entry, &palette_count, &cycle, &source, &dest);
		break;
	case 10:
		animate_palette_stage_0a(&copy_offset, &copy_entry, &palette_count, &cycle, &source, &dest);
		break;
	case 11:
		skip_stage_palette_animation();
		break;
	case 12:
		animate_palette_stage_0c(&copy_offset, &copy_entry, &palette_count, &cycle, &source, &dest);
		break;
	case 13:
		animate_palette_stage_0d_0e(&copy_offset, &copy_entry, &palette_count, &cycle, &source, &dest);
		break;
	case 14:
		animate_palette_stage_0d_0e(&copy_offset, &copy_entry, &palette_count, &cycle, &source, &dest);
		break;
	case 15:
		animate_palette_stage_0f(&copy_offset, &copy_entry, &palette_count, &cycle, &source, &dest);
		break;
	case 16:
		animate_palette_stage_10(&copy_offset, &copy_entry, &palette_count, &cycle, &source, &dest);
		break;
	case 17:
		skip_stage_palette_animation();
		break;
	case 18:
		skip_stage_palette_animation();
		break;
	case 19:
		skip_stage_palette_animation();
		break;
	default:
		unknown_code(0x1AD41u + stage * 3u);
	}
	*p_copy_offset = copy_offset;
	*p_copy_entry = copy_entry;
	*p_palette_count = palette_count;
	*p_cycle = cycle;
	*p_source = source;
	*p_dest = dest;
}

void skip_stage_palette_animation(void)
{
}

void animate_palette_stage_10(uint32_t *p_copy_offset, uint32_t *p_copy_entry, uint32_t *p_palette_count, uint32_t *p_cycle, uint32_t *p_source, uint32_t *p_dest)
{
	uint32_t copy_entry = *p_copy_entry;
	uint16_t halfwords = 0;
	uint32_t copy_offset = 0, cycle = 0, cycle_1 = 0, dest = 0, dest_1 = 0, ignored = 0, palette_count = 0, palette_count_1 = 0, source = 0, source_1 = 0;

	dest = PALETTE_RAM + 0x8B40u;
	palette_count = 6u;
	source = stage_10_cycle_palettes_0_ADDRESS;
	halfwords = 0x10u;
	cycle = palette_cycle_0_ADDRESS;
	palette_cycle_0 = 6u;
	palette_cycle_0_frames = 8u;
	cycle_palette(cycle, halfwords, &ignored, &copy_entry, &palette_count, &source, &dest);
	cycle_1 = palette_cycle_1_ADDRESS;
	dest_1 = PALETTE_RAM + 0x8940u;
	palette_count_1 = 1u;
	source_1 = stage_10_cycle_palettes_1_ADDRESS;
	halfwords = 0x10u;
	palette_cycle_1 = 4u;
	palette_cycle_1_frames = 8u;
	cycle_palette(cycle_1, halfwords, &copy_offset, &copy_entry, &palette_count_1, &source_1, &dest_1);
	*p_copy_offset = copy_offset;
	*p_copy_entry = copy_entry;
	*p_palette_count = palette_count_1;
	*p_cycle = cycle_1;
	*p_source = source_1;
	*p_dest = dest_1;
}

void animate_palette_stage_0c(uint32_t *p_copy_offset, uint32_t *p_copy_entry, uint32_t *p_palette_count, uint32_t *p_cycle, uint32_t *p_source, uint32_t *p_dest)
{
	uint32_t copy_entry = *p_copy_entry;
	uint16_t halfwords = 0;
	uint32_t copy_offset = 0, cycle = 0, dest = 0, palette_count = 0, source = 0;

	dest = PALETTE_RAM + 0x8800u;
	palette_count = 1u;
	source = stage_0c_cycle_palettes_ADDRESS;
	halfwords = 0x10u;
	cycle = palette_cycle_0_ADDRESS;
	palette_cycle_0 = 6u;
	palette_cycle_0_frames = 8u;
	cycle_palette(cycle, halfwords, &copy_offset, &copy_entry, &palette_count, &source, &dest);
	*p_copy_offset = copy_offset;
	*p_copy_entry = copy_entry;
	*p_palette_count = palette_count;
	*p_cycle = cycle;
	*p_source = source;
	*p_dest = dest;
}

void animate_palette_stage_0d_0e(uint32_t *p_copy_offset, uint32_t *p_copy_entry, uint32_t *p_palette_count, uint32_t *p_cycle, uint32_t *p_source, uint32_t *p_dest)
{
	uint32_t copy_entry = *p_copy_entry;
	uint16_t halfwords = 0;
	uint32_t copy_offset = 0, cycle = 0, dest = 0, palette_count = 0, source = 0;

	dest = PALETTE_RAM + 0x88C0u;
	palette_count = 1u;
	source = stage_0d_0e_cycle_palettes_ADDRESS;
	halfwords = 0x10u;
	cycle = palette_cycle_0_ADDRESS;
	palette_cycle_0 = 6u;
	palette_cycle_0_frames = 8u;
	cycle_palette(cycle, halfwords, &copy_offset, &copy_entry, &palette_count, &source, &dest);
	*p_copy_offset = copy_offset;
	*p_copy_entry = copy_entry;
	*p_palette_count = palette_count;
	*p_cycle = cycle;
	*p_source = source;
	*p_dest = dest;
}

void animate_palette_stage_0f(uint32_t *p_copy_offset, uint32_t *p_copy_entry, uint32_t *p_palette_count, uint32_t *p_cycle, uint32_t *p_source, uint32_t *p_dest)
{
	uint32_t copy_entry = *p_copy_entry;
	uint16_t halfwords = 0;
	uint32_t copy_offset = 0, cycle = 0, dest = 0, ignored = 0, palette_count = 0, source = 0;

	dest = PALETTE_RAM + 0x8920u;
	palette_count = 3u;
	source = stage_0f_cycle_palettes_0_ADDRESS;
	halfwords = 0x10u;
	cycle = palette_cycle_0_ADDRESS;
	palette_cycle_0 = 0xCu;
	palette_cycle_0_frames = 6u;
	cycle_palette(cycle, halfwords, &ignored, &copy_entry, &palette_count, &source, &dest);
	cycle = palette_cycle_1_ADDRESS;
	dest = PALETTE_RAM + 0x8900u;
	palette_count = 1u;
	source = stage_0f_cycle_palettes_1_ADDRESS;
	halfwords = 0x10u;
	palette_cycle_1 = 6u;
	palette_cycle_1_frames = 6u;
	cycle_palette(cycle, halfwords, &copy_offset, &copy_entry, &palette_count, &source, &dest);
	*p_copy_offset = copy_offset;
	*p_copy_entry = copy_entry;
	*p_palette_count = palette_count;
	*p_cycle = cycle;
	*p_source = source;
	*p_dest = dest;
}

void animate_palette_stage_0a(uint32_t *p_copy_offset, uint32_t *p_copy_entry, uint32_t *p_palette_count, uint32_t *p_cycle, uint32_t *p_source, uint32_t *p_dest)
{
	uint32_t copy_entry = *p_copy_entry;
	uint16_t halfwords = 0;
	uint32_t copy_offset = 0, cycle = 0, dest = 0, ignored = 0, palette_count = 0, source = 0;

	dest = PALETTE_RAM + 0x8A40u;
	palette_count = 1u;
	source = stage_0a_cycle_palettes_0_ADDRESS;
	halfwords = 0x10u;
	cycle = palette_cycle_0_ADDRESS;
	palette_cycle_0 = 5u;
	palette_cycle_0_frames = 6u;
	cycle_palette(cycle, halfwords, &ignored, &copy_entry, &palette_count, &source, &dest);
	cycle = palette_cycle_1_ADDRESS;
	dest = PALETTE_RAM + 0x8A60u;
	palette_count = 9u;
	source = stage_0a_cycle_palettes_1_ADDRESS;
	halfwords = 0x10u;
	palette_cycle_1 = 5u;
	palette_cycle_1_frames = 0xAu;
	cycle_palette(cycle, halfwords, &copy_offset, &copy_entry, &palette_count, &source, &dest);
	*p_copy_offset = copy_offset;
	*p_copy_entry = copy_entry;
	*p_palette_count = palette_count;
	*p_cycle = cycle;
	*p_source = source;
	*p_dest = dest;
}

void animate_palette_stage_09(uint32_t *p_copy_offset, uint32_t *p_copy_entry, uint32_t *p_palette_count, uint32_t *p_cycle, uint32_t *p_source, uint32_t *p_dest)
{
	uint32_t copy_entry = *p_copy_entry;
	uint16_t halfwords = 0;
	uint32_t copy_offset = 0, cycle = 0, dest = 0, palette_count = 0, source = 0;

	dest = PALETTE_RAM + 0x88A0u;
	palette_count = 2u;
	source = stage_09_cycle_palettes_ADDRESS;
	halfwords = 0x10u;
	cycle = palette_cycle_0_ADDRESS;
	palette_cycle_0 = 3u;
	palette_cycle_0_frames = 7u;
	cycle_palette(cycle, halfwords, &copy_offset, &copy_entry, &palette_count, &source, &dest);
	*p_copy_offset = copy_offset;
	*p_copy_entry = copy_entry;
	*p_palette_count = palette_count;
	*p_cycle = cycle;
	*p_source = source;
	*p_dest = dest;
}

void animate_palette_stage_04(uint32_t *p_copy_offset, uint32_t *p_copy_entry, uint32_t *p_palette_count, uint32_t *p_cycle, uint32_t *p_source, uint32_t *p_dest)
{
	uint32_t copy_entry = *p_copy_entry;
	uint16_t halfwords = 0;
	uint32_t copy_offset = 0, cycle = 0, dest = 0, palette_count = 0, source = 0;

	dest = PALETTE_RAM + 0x8500u;
	palette_count = 2u;
	source = stage_04_cycle_palettes_ADDRESS;
	halfwords = 0x10u;
	cycle = palette_cycle_0_ADDRESS;
	palette_cycle_0 = 4u;
	palette_cycle_0_frames = 8u;
	cycle_palette(cycle, halfwords, &copy_offset, &copy_entry, &palette_count, &source, &dest);
	*p_copy_offset = copy_offset;
	*p_copy_entry = copy_entry;
	*p_palette_count = palette_count;
	*p_cycle = cycle;
	*p_source = source;
	*p_dest = dest;
}

void cycle_palette(uint32_t cycle, uint16_t halfwords, uint32_t *p_copy_offset, uint32_t *p_copy_entry, uint32_t *p_palette_count, uint32_t *p_source, uint32_t *p_dest)
{
	uint32_t copy_entry = *p_copy_entry;
	uint32_t palette_count = *p_palette_count;
	uint32_t source = *p_source;
	uint32_t dest = *p_dest;
	uint32_t copy_offset = 0, frame_offset = 0;

	frame_offset = (uint32_t)mem_read8(cycle + 3u) << 5;
	source += frame_offset;
	if ((int8_t)mem_read8(cycle + 4u) > 0)
	{
		mem_write8(cycle + 4u, mem_read8(cycle + 4u) - 1u);
		*p_copy_offset = frame_offset;
		*p_source = source;
		return;
	}
	do
	{
		mem_write8(cycle + 4u, mem_read8(cycle));
		queue_vblank_copy(source, dest, halfwords, &copy_offset, &copy_entry);
		dest += 0x20u;
		palette_count--;
	} while (palette_count != 0u);
	mem_write8(cycle + 3u, mem_read8(cycle + 3u) + 1u);
	if ((int8_t)mem_read8(cycle + 3u) < (int8_t)mem_read8(cycle + 1u))
	{
		mem_write8(cycle + 4u, mem_read8(cycle + 4u) - 1u);
		*p_copy_offset = copy_offset;
		*p_copy_entry = copy_entry;
		*p_palette_count = palette_count;
		*p_source = source;
		*p_dest = dest;
		return;
	}
	mem_write8(cycle + 3u, 0u);
	mem_write8(cycle + 4u, mem_read8(cycle + 4u) - 1u);
	*p_copy_offset = copy_offset;
	*p_copy_entry = copy_entry;
	*p_palette_count = palette_count;
	*p_source = source;
	*p_dest = dest;
}

uint32_t cycle_palette_set_0(uint32_t source)
{
	palette_cycle_set = 0x209090u;
	source = cycle_palette_set(source);
	return source;
}

uint32_t cycle_palette_set_1(uint32_t source)
{
	palette_cycle_set = 0x2090A0u;
	source = cycle_palette_set(source);
	return source;
}

uint32_t cycle_palette_set(uint32_t source)
{
	uint16_t halfwords = 0;
	uint32_t cycle_set = 0, dest = 0, ignored = 0;

	cycle_set = palette_cycle_set;
	if (mem_read8(cycle_set + 8u) == 0u)
	{
		mem_write8(cycle_set + 8u, mem_read8(cycle_set + 0xAu));
		mem_write8(cycle_set + 9u, mem_read8(cycle_set + 0xBu));
		do
		{
			halfwords = 0x10u;
			dest = mem_read32(cycle_set + 4u) + ((uint32_t)(uint8_t)(mem_read8(cycle_set + 0xBu) - mem_read8(cycle_set + 9u)) << 5);
			source = mem_read32(cycle_set) + ((uint32_t)mem_read8(cycle_set + 0xCu) << 5);
			queue_vblank_copy(source, dest, halfwords, &ignored, &ignored);
			if (mem_read8(cycle_set + 9u) == 0u)
				break;
			mem_write8(cycle_set + 9u, mem_read8(cycle_set + 9u) - 1u);
		} while (mem_read8(cycle_set + 9u) != 0u);
		mem_write8(cycle_set + 0xCu, mem_read8(cycle_set + 0xCu) + 1u);
	}
	mem_write8(cycle_set + 8u, mem_read8(cycle_set + 8u) - 1u);
	if ((int8_t)mem_read8(cycle_set + 0xDu) <= (int8_t)mem_read8(cycle_set + 0xCu))
		mem_write8(cycle_set + 0xCu, 0u);
	return source;
}

void load_stage_palette_set(uint32_t *p_tile_bank_bits, uint32_t *p_dest)
{
	uint32_t ignored = 0, palette_dest = 0, stage_index = 0, tile_bank_bits = 0;

	stage_index = stage;
	load_palette_set_and_backdrop(stage_index, &tile_bank_bits, &ignored, &palette_dest);
	*p_tile_bank_bits = tile_bank_bits;
	*p_dest = palette_dest;
}

void load_palette_set_and_backdrop(uint32_t stage_index, uint32_t *p_tile_bank_bits, uint32_t *p_source, uint32_t *p_dest)
{
	uint32_t dest = 0, palette_list = 0, palette_set = 0, source = 0, tile_bank_bits = 0, tile_bank_entry = 0;

	mem_write32(video_control, mem_read32(video_control) & ~0x400u);
	tile_bank_entry = stage_tile_bank_bits_ADDRESS + stage_index;
	tile_bank_bits = (tile_bank_entry & ~0xFFFFu) | mem_read8(tile_bank_entry);
	tile_bank_bits = (tile_bank_bits & ~0xFFFFu) | (uint16_t)(tile_bank_bits << 10);
	mem_write16(video_control, mem_read16(video_control) | tile_bank_bits);
	palette_set = stage_palette_sets[stage_index];
	vblank_copies_pending = 0u;
	vblank_copy_count = 0u;
	palette_list = palette_set;
	load_palette_list(palette_list, &source, &dest);
	mem_write16(video_backdrop_control, mem_read16(video_backdrop_control) | 0x200u);
	switch (stage)
	{
	case 0:
	case 1:
	case 2:
	case 4:
	case 5:
	case 6:
	case 8:
	case 10:
	case 11:
	case 12:
	case 13:
	case 14:
	case 15:
	case 16:
	case 17:
	case 18:
		mem_write16(backdrop_color, 0u);
		*p_tile_bank_bits = tile_bank_bits;
		*p_source = source;
		*p_dest = dest;
		return;
	case 3:
	case 7:
		mem_write16(backdrop_color, 0x200u);
		*p_tile_bank_bits = tile_bank_bits;
		*p_source = source;
		*p_dest = dest;
		return;
	case 9:
	case 19:
		mem_write16(backdrop_color, 0x6212u);
		*p_tile_bank_bits = tile_bank_bits;
		*p_source = source;
		*p_dest = dest;
		return;
	default:
		unknown_code(0x1B033u + stage * 3u);
	}
}

void set_backdrop_color_19aa(void)
{
	mem_write16(backdrop_color, 0x19AAu);
}
