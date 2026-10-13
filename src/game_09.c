/**
 *    Name:        game_09.c
 *    Description: the game's functions, part 09 (in the order of their V60 addresses)
 *    Version:     0.1.0
 *    Created:     12-10-2026
 *    Modified:    13-10-2026
 *    Author:      Sebastian Toyos
 *
 *    SegaSonic The Hedgehog (arcade, Sega System 32): the game's V60 program decompiled into C by
 *    tools/decompile.py, with the names of symbols.txt.
 */
#include "game.h"

void title_smoke_puff_animate(void)
{
	uint32_t puff_frame = 0;

	if (OBJECT(current_object)->title.started == 0u)
	{
		OBJECT(current_object)->title.started = 1u;
		OBJECT(current_object)->sprite_frame = 0xBB60u;
		OBJECT(current_object)->screen_y = 0x70;
		OBJECT(current_object)->sprite_layer = 7u;
		OBJECT(current_object)->sprite_scale = 0x100u;
		OBJECT(current_object)->sprite_flags = 0u;
		OBJECT(current_object)->sprite_palette = 0xEB0u;
		OBJECT(current_object)->title.timer = 0;
		OBJECT(current_object)->title.frame_index = 0u;
	}
	if (title_done != 0u)
	{
		delete_current_object();
		return;
	}
	AS8(OBJECT(current_object)->title.timer)++;
	if (OBJECT(current_object)->title.timer < 3)
	{
		queue_sprite_in_layer();
		return;
	}
	OBJECT(current_object)->title.timer = 0;
	puff_frame = OBJECT(current_object)->title.frame_index;
	OBJECT(current_object)->title.frame_index++;
	puff_frame = 0x97538u + puff_frame * 2u;
	OBJECT(current_object)->sprite_frame = ROM16(puff_frame);
	if (ROM16(puff_frame) != 0xFFFFu)
	{
		queue_sprite_in_layer();
		return;
	}
	delete_current_object();
}

void title_clouds_1_drift(void)
{
	uint16_t animation = 0, cloud_4 = 0, floor_block = 0, floor_z = 0, previous_floor_z = 0, speed_x = 0, speed_y = 0, speed_z = 0, x = 0, y = 0, z = 0;
	uint32_t ignored = 0, object = 0;

	if (OBJECT(current_object)->title_clouds.started == 0u)
	{
		OBJECT(current_object)->title_clouds.started = 1u;
		OBJECT(current_object)->screen_x = 0x31;
		OBJECT(current_object)->screen_y = 0x25;
		OBJECT(current_object)->sprite_scale = 0xE0u;
		OBJECT(current_object)->sprite_frame = 0xB6D8u;
		OBJECT(current_object)->sprite_flags = 0u;
		OBJECT(current_object)->sprite_layer = 8u;
		OBJECT(current_object)->sprite_palette = 0xD80u;
		allocate_object_2(&ignored, &ignored, &object);
		OBJECT(current_object)->speed_z = object;
		OBJECT(object)->screen_x = 0xB3;
		OBJECT(object)->screen_y = 0x2A;
		OBJECT(object)->sprite_scale = 0xE0u;
		OBJECT(object)->sprite_frame = 0xB6F0u;
		OBJECT(object)->sprite_flags = 0u;
		OBJECT(object)->sprite_layer = 8u;
		OBJECT(object)->sprite_palette = 0xD80u;
		allocate_object_2(&ignored, &ignored, &object);
		OBJECT(current_object)->z = object;
		OBJECT(object)->screen_x = 0xB3;
		OBJECT(object)->screen_y = 0x2A;
		OBJECT(object)->sprite_scale = 0xE0u;
		OBJECT(object)->sprite_frame = 0xB6F0u;
		OBJECT(object)->sprite_flags = 0u;
		OBJECT(object)->sprite_layer = 8u;
		OBJECT(object)->sprite_palette = 0xD80u;
		allocate_object_2(&ignored, &ignored, &object);
		OBJECT(current_object)->title_clouds.cloud_4 = object;
		OBJECT(object)->screen_x = 0x113;
		OBJECT(object)->screen_y = 0x20;
		OBJECT(object)->sprite_scale = 0xE0u;
		OBJECT(object)->sprite_frame = 0xB6F0u;
		OBJECT(object)->sprite_flags = 0u;
		OBJECT(object)->sprite_layer = 8u;
		OBJECT(object)->sprite_palette = 0xD80u;
		allocate_object_2(&ignored, &ignored, &object);
		OBJECT(current_object)->animation = object;
		OBJECT(object)->screen_x = 0x3A;
		OBJECT(object)->screen_y = 0x5A;
		OBJECT(object)->sprite_scale = 0xB0u;
		OBJECT(object)->sprite_frame = 0xB6F0u;
		OBJECT(object)->sprite_flags = 0u;
		OBJECT(object)->sprite_layer = 8u;
		OBJECT(object)->sprite_palette = 0xD80u;
		allocate_object_2(&ignored, &ignored, &object);
		OBJECT(current_object)->x = object;
		OBJECT(object)->screen_x = 0x61;
		OBJECT(object)->screen_y = 0x46;
		OBJECT(object)->sprite_scale = 0x60u;
		OBJECT(object)->sprite_frame = 0xB6E0u;
		OBJECT(object)->sprite_flags = 0u;
		OBJECT(object)->sprite_layer = 8u;
		OBJECT(object)->sprite_palette = 0xD80u;
		allocate_object_2(&ignored, &ignored, &object);
		OBJECT(current_object)->y = object;
		OBJECT(object)->screen_x = 0xD0;
		OBJECT(object)->screen_y = 0x4D;
		OBJECT(object)->sprite_scale = 0x80u;
		OBJECT(object)->sprite_frame = 0xB6E0u;
		OBJECT(object)->sprite_flags = 0u;
		OBJECT(object)->sprite_layer = 8u;
		OBJECT(object)->sprite_palette = 0xD80u;
		allocate_object_2(&ignored, &ignored, &object);
		OBJECT(current_object)->floor_z = object;
		OBJECT(object)->screen_x = 0x8E;
		OBJECT(object)->screen_y = 0x63;
		OBJECT(object)->sprite_scale = 0x40u;
		OBJECT(object)->sprite_frame = 0xB6E0u;
		OBJECT(object)->sprite_flags = 0u;
		OBJECT(object)->sprite_layer = 8u;
		OBJECT(object)->sprite_palette = 0xD80u;
		allocate_object_2(&ignored, &ignored, &object);
		OBJECT(current_object)->previous_floor_z = object;
		OBJECT(object)->screen_x = 0x40;
		OBJECT(object)->screen_y = 0x7F;
		OBJECT(object)->sprite_scale = 0x60u;
		OBJECT(object)->sprite_frame = 0xB6E0u;
		OBJECT(object)->sprite_flags = 0u;
		OBJECT(object)->sprite_layer = 8u;
		OBJECT(object)->sprite_palette = 0xD80u;
		allocate_object_2(&ignored, &ignored, &object);
		OBJECT(current_object)->floor_block = object;
		OBJECT(object)->screen_x = 0x7D;
		OBJECT(object)->screen_y = 0x7A;
		OBJECT(object)->sprite_scale = 0x40u;
		OBJECT(object)->sprite_frame = 0xB6D8u;
		OBJECT(object)->sprite_flags = 0u;
		OBJECT(object)->sprite_layer = 8u;
		OBJECT(object)->sprite_palette = 0xD80u;
		allocate_object_2(&ignored, &ignored, &object);
		OBJECT(current_object)->speed_x = object;
		OBJECT(object)->screen_x = 0x115;
		OBJECT(object)->screen_y = 0x74;
		OBJECT(object)->sprite_scale = 0x40u;
		OBJECT(object)->sprite_frame = 0xB6D8u;
		OBJECT(object)->sprite_flags = 0u;
		OBJECT(object)->sprite_layer = 8u;
		OBJECT(object)->sprite_palette = 0xD80u;
		allocate_object_2(&ignored, &ignored, &object);
		OBJECT(current_object)->speed_y = object;
		OBJECT(object)->screen_x = 0x10D;
		OBJECT(object)->screen_y = 0x3E;
		OBJECT(object)->sprite_scale = 0x90u;
		OBJECT(object)->sprite_frame = 0xB6F0u;
		OBJECT(object)->sprite_flags = 0u;
		OBJECT(object)->sprite_layer = 8u;
		OBJECT(object)->sprite_palette = 0xD80u;
		OBJECT(current_object)->title_clouds.timer = 0;
	}
	if (title_done == 0u)
	{
		OBJECT(object_cursor)->title_clouds.timer++;
		if (OBJECT(object_cursor)->title_clouds.timer > 3)
		{
			OBJECT(object_cursor)->title_clouds.timer = 0;
			current_object = object_cursor;
			drift_title_cloud();
			current_object = OBJECT(object_cursor)->speed_y;
			drift_title_cloud();
			current_object = OBJECT(object_cursor)->speed_x;
			drift_title_cloud();
			current_object = OBJECT(object_cursor)->speed_z;
			drift_title_cloud();
			current_object = OBJECT(object_cursor)->z;
			drift_title_cloud();
			current_object = OBJECT(object_cursor)->title_clouds.cloud_4;
			drift_title_cloud();
			current_object = OBJECT(object_cursor)->animation;
			drift_title_cloud();
			current_object = OBJECT(object_cursor)->x;
			drift_title_cloud();
			current_object = OBJECT(object_cursor)->y;
			drift_title_cloud();
			current_object = OBJECT(object_cursor)->floor_z;
			drift_title_cloud();
			current_object = OBJECT(object_cursor)->previous_floor_z;
			drift_title_cloud();
			current_object = OBJECT(object_cursor)->floor_block;
			drift_title_cloud();
		}
		current_object = object_cursor;
		queue_sprite_in_layer();
		current_object = OBJECT(object_cursor)->speed_y;
		queue_sprite_in_layer();
		current_object = OBJECT(object_cursor)->speed_x;
		queue_sprite_in_layer();
		current_object = OBJECT(object_cursor)->speed_z;
		queue_sprite_in_layer();
		current_object = OBJECT(object_cursor)->z;
		queue_sprite_in_layer();
		current_object = OBJECT(object_cursor)->title_clouds.cloud_4;
		queue_sprite_in_layer();
		current_object = OBJECT(object_cursor)->animation;
		queue_sprite_in_layer();
		current_object = OBJECT(object_cursor)->x;
		queue_sprite_in_layer();
		current_object = OBJECT(object_cursor)->y;
		queue_sprite_in_layer();
		current_object = OBJECT(object_cursor)->floor_z;
		queue_sprite_in_layer();
		current_object = OBJECT(object_cursor)->previous_floor_z;
		queue_sprite_in_layer();
		current_object = OBJECT(object_cursor)->floor_block;
		queue_sprite_in_layer();
		return;
	}
	speed_y = (uint16_t)OBJECT(object_cursor)->speed_y;
	free_object_2(speed_y);
	speed_x = (uint16_t)OBJECT(object_cursor)->speed_x;
	free_object_2(speed_x);
	speed_z = (uint16_t)OBJECT(object_cursor)->speed_z;
	free_object_2(speed_z);
	z = (uint16_t)OBJECT(object_cursor)->z;
	free_object_2(z);
	cloud_4 = (uint16_t)OBJECT(object_cursor)->title_clouds.cloud_4;
	free_object_2(cloud_4);
	animation = (uint16_t)OBJECT(object_cursor)->animation;
	free_object_2(animation);
	x = (uint16_t)OBJECT(object_cursor)->x;
	free_object_2(x);
	y = (uint16_t)OBJECT(object_cursor)->y;
	free_object_2(y);
	floor_z = (uint16_t)OBJECT(object_cursor)->floor_z;
	free_object_2(floor_z);
	previous_floor_z = (uint16_t)OBJECT(object_cursor)->previous_floor_z;
	free_object_2(previous_floor_z);
	floor_block = (uint16_t)OBJECT(object_cursor)->floor_block;
	free_object_2(floor_block);
	delete_current_object();
}

void title_clouds_2_drift(void)
{
	uint16_t animation = 0, cloud_4 = 0, speed_z = 0, x = 0, y = 0, z = 0;
	uint32_t ignored = 0, object = 0;

	if (OBJECT(current_object)->title_clouds.started == 0u)
	{
		OBJECT(current_object)->title_clouds.started = 1u;
		OBJECT(current_object)->screen_x = 0x6B;
		OBJECT(current_object)->screen_y = 0x41;
		OBJECT(current_object)->sprite_scale = 0x60u;
		OBJECT(current_object)->sprite_frame = 0xB6E0u;
		OBJECT(current_object)->sprite_flags = 0u;
		OBJECT(current_object)->sprite_layer = 8u;
		OBJECT(current_object)->sprite_palette = 0xD80u;
		allocate_object_2(&ignored, &ignored, &object);
		OBJECT(current_object)->speed_z = object;
		OBJECT(object)->screen_x = 0xB6;
		OBJECT(object)->screen_y = 0x40;
		OBJECT(object)->sprite_scale = 0x60u;
		OBJECT(object)->sprite_frame = 0xB6E0u;
		OBJECT(object)->sprite_flags = 0u;
		OBJECT(object)->sprite_layer = 8u;
		OBJECT(object)->sprite_palette = 0xD80u;
		allocate_object_2(&ignored, &ignored, &object);
		OBJECT(current_object)->z = object;
		OBJECT(object)->screen_x = 0xFC;
		OBJECT(object)->screen_y = 0x36;
		OBJECT(object)->sprite_scale = 0xD0u;
		OBJECT(object)->sprite_frame = 0xB700u;
		OBJECT(object)->sprite_flags = 0u;
		OBJECT(object)->sprite_layer = 8u;
		OBJECT(object)->sprite_palette = 0xD80u;
		allocate_object_2(&ignored, &ignored, &object);
		OBJECT(current_object)->title_clouds.cloud_4 = object;
		OBJECT(object)->screen_x = 0x5B;
		OBJECT(object)->screen_y = 0x56;
		OBJECT(object)->sprite_scale = 0x60u;
		OBJECT(object)->sprite_frame = 0xB6F8u;
		OBJECT(object)->sprite_flags = 0u;
		OBJECT(object)->sprite_layer = 8u;
		OBJECT(object)->sprite_palette = 0xD80u;
		allocate_object_2(&ignored, &ignored, &object);
		OBJECT(current_object)->animation = object;
		OBJECT(object)->screen_x = 0xE6;
		OBJECT(object)->screen_y = 0x51;
		OBJECT(object)->sprite_scale = 0x40u;
		OBJECT(object)->sprite_frame = 0xB6E0u;
		OBJECT(object)->sprite_flags = 0u;
		OBJECT(object)->sprite_layer = 8u;
		OBJECT(object)->sprite_palette = 0xD80u;
		allocate_object_2(&ignored, &ignored, &object);
		OBJECT(current_object)->x = object;
		OBJECT(object)->screen_x = 0x69;
		OBJECT(object)->screen_y = 0x71;
		OBJECT(object)->sprite_scale = 0x40u;
		OBJECT(object)->sprite_frame = 0xB6E0u;
		OBJECT(object)->sprite_flags = 0u;
		OBJECT(object)->sprite_layer = 8u;
		OBJECT(object)->sprite_palette = 0xD80u;
		allocate_object_2(&ignored, &ignored, &object);
		OBJECT(current_object)->y = object;
		OBJECT(object)->screen_x = 0xC1;
		OBJECT(object)->screen_y = 0x71;
		OBJECT(object)->sprite_scale = 0x40u;
		OBJECT(object)->sprite_frame = 0xB6D8u;
		OBJECT(object)->sprite_flags = 0u;
		OBJECT(object)->sprite_layer = 8u;
		OBJECT(object)->sprite_palette = 0xD80u;
		OBJECT(current_object)->title_clouds.timer = 0;
	}
	if (title_done == 0u)
	{
		OBJECT(object_cursor)->title_clouds.timer++;
		if (OBJECT(object_cursor)->title_clouds.timer > 3)
		{
			OBJECT(object_cursor)->title_clouds.timer = 0;
			current_object = object_cursor;
			drift_title_cloud_2();
			current_object = OBJECT(object_cursor)->speed_z;
			drift_title_cloud_2();
			current_object = OBJECT(object_cursor)->z;
			drift_title_cloud_2();
			current_object = OBJECT(object_cursor)->title_clouds.cloud_4;
			drift_title_cloud_2();
			current_object = OBJECT(object_cursor)->animation;
			drift_title_cloud_2();
			current_object = OBJECT(object_cursor)->x;
			drift_title_cloud_2();
			current_object = OBJECT(object_cursor)->y;
			drift_title_cloud_2();
		}
		current_object = object_cursor;
		queue_sprite_in_layer();
		current_object = OBJECT(object_cursor)->speed_z;
		queue_sprite_in_layer();
		current_object = OBJECT(object_cursor)->z;
		queue_sprite_in_layer();
		current_object = OBJECT(object_cursor)->title_clouds.cloud_4;
		queue_sprite_in_layer();
		current_object = OBJECT(object_cursor)->animation;
		queue_sprite_in_layer();
		current_object = OBJECT(object_cursor)->x;
		queue_sprite_in_layer();
		current_object = OBJECT(object_cursor)->y;
		queue_sprite_in_layer();
		return;
	}
	speed_z = (uint16_t)OBJECT(object_cursor)->speed_z;
	free_object_2(speed_z);
	z = (uint16_t)OBJECT(object_cursor)->z;
	free_object_2(z);
	cloud_4 = (uint16_t)OBJECT(object_cursor)->title_clouds.cloud_4;
	free_object_2(cloud_4);
	animation = (uint16_t)OBJECT(object_cursor)->animation;
	free_object_2(animation);
	x = (uint16_t)OBJECT(object_cursor)->x;
	free_object_2(x);
	y = (uint16_t)OBJECT(object_cursor)->y;
	free_object_2(y);
	delete_current_object();
}

void title_clouds_3_drift(void)
{
	uint16_t animation = 0, cloud_4 = 0, speed_z = 0, x = 0, y = 0, z = 0;
	uint32_t ignored = 0, object = 0;

	if (OBJECT(current_object)->title_clouds.started == 0u)
	{
		OBJECT(current_object)->title_clouds.started = 1u;
		OBJECT(current_object)->screen_x = 0x156;
		OBJECT(current_object)->screen_y = 0x5F;
		OBJECT(current_object)->sprite_scale = 0x40u;
		OBJECT(current_object)->sprite_frame = 0xB6E0u;
		OBJECT(current_object)->sprite_flags = 0u;
		OBJECT(current_object)->sprite_layer = 8u;
		OBJECT(current_object)->sprite_palette = 0xD80u;
		allocate_object_2(&ignored, &ignored, &object);
		OBJECT(current_object)->speed_z = object;
		OBJECT(object)->screen_x = 0x1A1;
		OBJECT(object)->screen_y = 0x5E;
		OBJECT(object)->sprite_scale = 0x40u;
		OBJECT(object)->sprite_frame = 0xB6E0u;
		OBJECT(object)->sprite_flags = 0u;
		OBJECT(object)->sprite_layer = 8u;
		OBJECT(object)->sprite_palette = 0xD80u;
		allocate_object_2(&ignored, &ignored, &object);
		OBJECT(current_object)->z = object;
		OBJECT(object)->screen_x = 0x1E7;
		OBJECT(object)->screen_y = 0x54;
		OBJECT(object)->sprite_scale = 0xA0u;
		OBJECT(object)->sprite_frame = 0xB6F8u;
		OBJECT(object)->sprite_flags = 0u;
		OBJECT(object)->sprite_layer = 8u;
		OBJECT(object)->sprite_palette = 0xD80u;
		allocate_object_2(&ignored, &ignored, &object);
		OBJECT(current_object)->title_clouds.cloud_4 = object;
		OBJECT(object)->screen_x = 0x146;
		OBJECT(object)->screen_y = 0x74;
		OBJECT(object)->sprite_scale = 0x60u;
		OBJECT(object)->sprite_frame = 0xB6D8u;
		OBJECT(object)->sprite_flags = 0u;
		OBJECT(object)->sprite_layer = 8u;
		OBJECT(object)->sprite_palette = 0xD80u;
		allocate_object_2(&ignored, &ignored, &object);
		OBJECT(current_object)->animation = object;
		OBJECT(object)->screen_x = 0x1D1;
		OBJECT(object)->screen_y = 0x6F;
		OBJECT(object)->sprite_scale = 0x40u;
		OBJECT(object)->sprite_frame = 0xB6E0u;
		OBJECT(object)->sprite_flags = 0u;
		OBJECT(object)->sprite_layer = 8u;
		OBJECT(object)->sprite_palette = 0xD80u;
		allocate_object_2(&ignored, &ignored, &object);
		OBJECT(current_object)->x = object;
		OBJECT(object)->screen_x = 0x154;
		OBJECT(object)->screen_y = 0x8F;
		OBJECT(object)->sprite_scale = 0x40u;
		OBJECT(object)->sprite_frame = 0xB6E0u;
		OBJECT(object)->sprite_flags = 0u;
		OBJECT(object)->sprite_layer = 8u;
		OBJECT(object)->sprite_palette = 0xD80u;
		allocate_object_2(&ignored, &ignored, &object);
		OBJECT(current_object)->y = object;
		OBJECT(object)->screen_x = 0x1AC;
		OBJECT(object)->screen_y = 0x8F;
		OBJECT(object)->sprite_scale = 0x40u;
		OBJECT(object)->sprite_frame = 0xB6D8u;
		OBJECT(object)->sprite_flags = 0u;
		OBJECT(object)->sprite_layer = 8u;
		OBJECT(object)->sprite_palette = 0xD80u;
		OBJECT(current_object)->title_clouds.timer = 0;
	}
	if (title_done == 0u)
	{
		OBJECT(object_cursor)->title_clouds.timer++;
		if (OBJECT(object_cursor)->title_clouds.timer > 3)
		{
			OBJECT(object_cursor)->title_clouds.timer = 0;
			current_object = object_cursor;
			drift_title_cloud();
			current_object = OBJECT(object_cursor)->speed_z;
			drift_title_cloud();
			current_object = OBJECT(object_cursor)->z;
			drift_title_cloud();
			current_object = OBJECT(object_cursor)->title_clouds.cloud_4;
			drift_title_cloud();
			current_object = OBJECT(object_cursor)->animation;
			drift_title_cloud();
			current_object = OBJECT(object_cursor)->x;
			drift_title_cloud();
			current_object = OBJECT(object_cursor)->y;
			drift_title_cloud();
		}
		current_object = object_cursor;
		queue_sprite_in_layer();
		current_object = OBJECT(object_cursor)->speed_z;
		queue_sprite_in_layer();
		current_object = OBJECT(object_cursor)->z;
		queue_sprite_in_layer();
		current_object = OBJECT(object_cursor)->title_clouds.cloud_4;
		queue_sprite_in_layer();
		current_object = OBJECT(object_cursor)->animation;
		queue_sprite_in_layer();
		current_object = OBJECT(object_cursor)->x;
		queue_sprite_in_layer();
		current_object = OBJECT(object_cursor)->y;
		queue_sprite_in_layer();
		return;
	}
	speed_z = (uint16_t)OBJECT(object_cursor)->speed_z;
	free_object_2(speed_z);
	z = (uint16_t)OBJECT(object_cursor)->z;
	free_object_2(z);
	cloud_4 = (uint16_t)OBJECT(object_cursor)->title_clouds.cloud_4;
	free_object_2(cloud_4);
	animation = (uint16_t)OBJECT(object_cursor)->animation;
	free_object_2(animation);
	x = (uint16_t)OBJECT(object_cursor)->x;
	free_object_2(x);
	y = (uint16_t)OBJECT(object_cursor)->y;
	free_object_2(y);
	delete_current_object();
}

void drift_title_cloud(void)
{
	OBJECT(current_object)->screen_y--;
	OBJECT(current_object)->screen_x -= 3u;
	OBJECT(current_object)->sprite_scale += 2u;
	if ((int16_t)OBJECT(current_object)->sprite_scale > 0x140)
		OBJECT(current_object)->sprite_scale = 0x140u;
}

void drift_title_cloud_2(void)
{
	OBJECT(current_object)->screen_y--;
	OBJECT(current_object)->screen_x -= 3u;
	OBJECT(current_object)->sprite_scale += 2u;
	if ((int16_t)OBJECT(current_object)->sprite_scale > 0x120)
		OBJECT(current_object)->sprite_scale = 0x140u;
}

void title_sky_wave(void)
{
	uint16_t halfwords = 0;
	uint32_t r2 = 0, cycle = 0, dest = 0, ignored = 0, palette_count = 0, row_offset = 0, row_scroll = 0, sine_cursor = 0, source = 0, wave_rows_left = 0;

	if (OBJECT(current_object)->title.started == 0u)
	{
		fill_value = 0u;
		set_halfwords(VIDEO_RAM + 0x800u, 0x400u, fill_value);
		set_halfwords(VIDEO_RAM + 0x2000u, 0x400u, fill_value);
		mem_write16(video_rowscroll_control, 0x401u);
		OBJECT(current_object)->title.started = 1u;
		OBJECT(current_object)->speed_y = 0;
	}
	if (title_done != 0u)
	{
		delete_current_object();
		return;
	}
	sine_cursor = sine_table_ADDRESS;
	row_scroll = VIDEO_RAM + 0x900u;
	row_offset = VIDEO_RAM + 0x2100u;
	wave_rows_left = 0x80u;
	do
	{
		mem_write32(row_offset, mem_read32(row_offset) + ((uint32_t)mem_read16(sine_cursor) << 3));
		mem_write32(row_offset, mem_read32(row_offset) & 0x1FFFFFFu);
		mem_write16(row_scroll, mem_read32(row_offset) >> 16);
		row_scroll += 2u;
		sine_cursor = ((sine_cursor + 2u) & ~0xFFFFu) | (uint16_t)(sine_cursor + 2u + 2u);
		wave_rows_left--;
		row_offset += 4u;
	} while (wave_rows_left != 0u);
	OBJECT(current_object)->speed_y += 0x4000u;
	OBJECT(current_object)->speed_y &= 0x3FFFFFFu;
	nbg3_scroll_x = (uint32_t)OBJECT(current_object)->speed_y >> 16;
	dest = PALETTE_RAM + 0x8F60u;
	palette_count = 5u;
	source = title_sky_cycle_palettes_ADDRESS;
	halfwords = 0x10u;
	cycle = palette_cycle_0_ADDRESS;
	palette_cycle_0 = 3u;
	palette_cycle_0_frames = 0x10u;
	cycle_palette(cycle, halfwords, &ignored, &r2, &palette_count, &source, &dest);
}

void run_sega_logo(uint32_t r11, uint32_t *p_r4, uint32_t *p_r8, uint32_t *p_r9, uint32_t *p_r10)
{
	uint32_t r4 = *p_r4;
	uint32_t r8 = *p_r8;
	uint32_t r9 = *p_r9;
	uint32_t r10 = *p_r10;
	uint8_t is_8bpp = 0;
	uint16_t object = 0, scroll_x = 0, scroll_y = 0;
	uint32_t r0 = 0, r5 = 0, ignored = 0;
	uint8_t cnt_bits = 0;

	if (sega_logo_step == 7u)
		return;
	if (sega_logo_step == 0u)
	{
		cnt_bits = mem_read8(io_cnt);
		mem_write8(io_cnt, cnt_bits & ~2u);
		display_off_frames = 5u;
		init_sprites(&r0, &r10);
		r11 = clear_sprite_buckets();
		clear_bitmap_layer((uint8_t)r0, &ignored, &r4, &r5, &r8, &r9, &r10);
		title_scene_built = 1u;
		title_done = 0u;
		scroll_x = 0u;
		scroll_y = 0u;
		is_8bpp = 1u;
		set_bitmap_scroll(scroll_x, scroll_y, is_8bpp);
		mixer_text_blend &= 0xBFFFu;
		mixer_bitmap_blend |= 0x4000u;
		mixer_sprite_control |= 0x8000u;
		color_offset_red = 0x20u;
		color_offset_blue = 0x20u;
		color_offset_green = 0x20u;
		mem_write16(video_layer_disable, mem_read16(video_layer_disable) | 0xFu);
		mem_write32(video_backdrop_control, mem_read32(video_backdrop_control) & ~0x200u);
		sega_logo_frame = 0;
		sega_logo_timer = 0;
		sega_logo_scale = 0;
		mem_write16(mixer_bitmap, 0x21u);
		mem_write32(video_control, mem_read32(video_control) | 0x800u);
		fill_value = (fill_value & ~0xFFFFu) | 0x8E8Eu;
		set_halfwords(VIDEO_RAM, 0xE000u, fill_value);
		sega_logo_step++;
	}
	allocate_object_2(&ignored, &ignored, &r11);
	current_object = r11;
	screen_sprite_block = current_object;
	switch (sega_logo_step)
	{
	case 0:
		sega_logo_timer = 0;
		sega_logo_step++;
		set_sega_logo_sprite();
		OBJECT(current_object)->sprite_scale = sega_logo_scale;
		OBJECT(current_object)->sprite_frame = 0x53D0u;
		queue_sprite_in_layer();
		sega_logo_scale += 0x18u;
		if (sega_logo_scale < 0xFF)
		{
			object = screen_sprite_block;
			free_object_2(object);
			*p_r4 = r4;
			*p_r8 = r8;
			*p_r9 = r9;
			*p_r10 = r10;
			return;
		}
		sega_logo_step++;
		sega_logo_frame++;
		object = screen_sprite_block;
		free_object_2(object);
		*p_r4 = r4;
		*p_r8 = r8;
		*p_r9 = r9;
		*p_r10 = r10;
		return;
	case 1:
	case 7:
		sega_logo_timer++;
		if (sega_logo_timer < 2)
		{
			object = screen_sprite_block;
			free_object_2(object);
			*p_r4 = r4;
			*p_r8 = r8;
			*p_r9 = r9;
			*p_r10 = r10;
			return;
		}
		AS16(sega_logo_timer) = 0u;
		color_offset_red++;
		color_offset_blue++;
		color_offset_green++;
		if ((int16_t)color_offset_red < 0x3F)
		{
			object = screen_sprite_block;
			free_object_2(object);
			*p_r4 = r4;
			*p_r8 = r8;
			*p_r9 = r9;
			*p_r10 = r10;
			return;
		}
		sega_logo_timer = 0;
		sega_logo_step++;
		set_sega_logo_sprite();
		OBJECT(current_object)->sprite_scale = sega_logo_scale;
		OBJECT(current_object)->sprite_frame = 0x53D0u;
		queue_sprite_in_layer();
		sega_logo_scale += 0x18u;
		if (sega_logo_scale < 0xFF)
		{
			object = screen_sprite_block;
			free_object_2(object);
			*p_r4 = r4;
			*p_r8 = r8;
			*p_r9 = r9;
			*p_r10 = r10;
			return;
		}
		sega_logo_step++;
		sega_logo_frame++;
		object = screen_sprite_block;
		free_object_2(object);
		*p_r4 = r4;
		*p_r8 = r8;
		*p_r9 = r9;
		*p_r10 = r10;
		return;
	case 2:
		set_sega_logo_sprite();
		OBJECT(current_object)->sprite_scale = sega_logo_scale;
		OBJECT(current_object)->sprite_frame = 0x53D0u;
		queue_sprite_in_layer();
		sega_logo_scale += 0x18u;
		if (sega_logo_scale < 0xFF)
		{
			object = screen_sprite_block;
			free_object_2(object);
			*p_r4 = r4;
			*p_r8 = r8;
			*p_r9 = r9;
			*p_r10 = r10;
			return;
		}
		sega_logo_step++;
		sega_logo_frame++;
		object = screen_sprite_block;
		free_object_2(object);
		*p_r4 = r4;
		*p_r8 = r8;
		*p_r9 = r9;
		*p_r10 = r10;
		return;
	case 3:
		set_sega_logo_sprite();
		queue_sprite_in_layer();
		sega_logo_timer++;
		if ((sega_logo_timer & 2u) == 0u)
		{
			object = screen_sprite_block;
			free_object_2(object);
			*p_r4 = r4;
			*p_r8 = r8;
			*p_r9 = r9;
			*p_r10 = r10;
			return;
		}
		sega_logo_timer = 0;
		sega_logo_frame++;
		if (sega_logo_frame <= 0x1E)
		{
			object = screen_sprite_block;
			free_object_2(object);
			*p_r4 = r4;
			*p_r8 = r8;
			*p_r9 = r9;
			*p_r10 = r10;
			return;
		}
		sega_logo_step++;
		sega_logo_frame--;
		sega_logo_timer = 0;
		sega_logo_scale = 0;
		color_offset_red = 0x3Fu;
		color_offset_blue = 0x3Fu;
		color_offset_green = 0x3Fu;
		object = screen_sprite_block;
		free_object_2(object);
		*p_r4 = r4;
		*p_r8 = r8;
		*p_r9 = r9;
		*p_r10 = r10;
		return;
	case 4:
		set_sega_logo_sprite();
		queue_sprite_in_layer();
		sega_logo_timer++;
		if (sega_logo_timer < 0x69)
		{
			object = screen_sprite_block;
			free_object_2(object);
			*p_r4 = r4;
			*p_r8 = r8;
			*p_r9 = r9;
			*p_r10 = r10;
			return;
		}
		sega_logo_step++;
		object = screen_sprite_block;
		free_object_2(object);
		*p_r4 = r4;
		*p_r8 = r8;
		*p_r9 = r9;
		*p_r10 = r10;
		return;
	case 5:
		set_sega_logo_sprite();
		queue_sprite_in_layer();
		color_offset_red--;
		color_offset_blue--;
		color_offset_green--;
		if ((int16_t)color_offset_red >= 0x21)
		{
			object = screen_sprite_block;
			free_object_2(object);
			*p_r4 = r4;
			*p_r8 = r8;
			*p_r9 = r9;
			*p_r10 = r10;
			return;
		}
		sega_logo_step++;
		sega_logo_timer = 0;
		object = screen_sprite_block;
		free_object_2(object);
		*p_r4 = r4;
		*p_r8 = r8;
		*p_r9 = r9;
		*p_r10 = r10;
		return;
	case 6:
		sega_logo_timer++;
		if (sega_logo_timer < 0x3C)
		{
			object = screen_sprite_block;
			free_object_2(object);
			*p_r4 = r4;
			*p_r8 = r8;
			*p_r9 = r9;
			*p_r10 = r10;
			return;
		}
		sega_logo_step++;
		mem_write32(video_control, mem_read32(video_control) | 0x800u);
		fill_value = fill_value & ~0xFFFFu;
		set_halfwords(VIDEO_RAM, 0xE000u, fill_value);
		mixer_sprite_control &= ~0x8000u;
		mixer_bitmap_blend &= 0xBFFFu;
		title_scene_built = 0u;
		object = screen_sprite_block;
		free_object_2(object);
		*p_r4 = r4;
		*p_r8 = r8;
		*p_r9 = r9;
		*p_r10 = r10;
		return;
	default:
		unknown_code(0x9824Eu + sega_logo_step * 3u);
	}
}

void set_sega_logo_sprite(void)
{
	OBJECT(current_object)->screen_x = 0xA0;
	OBJECT(current_object)->screen_y = 0x70;
	OBJECT(current_object)->sprite_palette = 0x420u;
	OBJECT(current_object)->sprite_layer = 0u;
	OBJECT(current_object)->sprite_scale = 0x100u;
	OBJECT(current_object)->sprite_flags = 0u;
	OBJECT(current_object)->sprite_frame = ((uint32_t)(uint8_t)sega_logo_frame << 3) + 0x53D0u;
}

void start_ring_tally(uint32_t *p_r2, uint32_t *p_r3, uint32_t *p_r4, uint32_t *p_r8, uint32_t *p_r9, uint32_t *p_object)
{
	uint16_t bonus_percent = 0;
	uint32_t r4 = 0, ignored = 0, object = 0, object_2 = 0, object_3 = 0, object_4 = 0, percent_fixed = 0, player = 0, player_1 = 0, player_3 = 0, player_3_out_flags = 0, stage_index = 0, stage_rings_1 = 0, stage_rings_2 = 0, stage_rings_3 = 0;
	uint32_t out_flags = 0;

	stage_results_done = 0u;
	ring_tally_panels_counting = 0u;
	ring_tally_counted = 0u;
	player_1 = PLAYER_BLOCK(1);
	object = PLAYER_BLOCK(2);
	player_3 = PLAYER_BLOCK(3);
	object_2 = LINKED(WORK_RAM, player_1_object);
	object_3 = LINKED(WORK_RAM, player_2_object);
	object_4 = LINKED(WORK_RAM, player_3_object);
	stage_rings_1 = player_1_stage_rings;
	stage_rings_2 = player_2_stage_rings;
	stage_rings_3 = player_3_stage_rings;
	stage_index = stage;
	bonus_percent = stage_ring_bonus_percents[stage_index];
	r4 = stage_ring_counts[stage_index];
	percent_fixed = divide_s32((uint32_t)(uint16_t)(stage_rings_1 + stage_rings_2 + stage_rings_3) << 16, r4) * 0x64u;
	if ((percent_fixed & 0x800u) != 0u)
		percent_fixed += 0x100u;
	percent_fixed = percent_fixed >> 16;
	ring_tally_percent = percent_fixed;
	stage_rings_1 = stage_rings_1 | 0x100u;
	stage_rings_2 = stage_rings_2 | 0x100u;
	stage_rings_3 = stage_rings_3 | 0x100u;
	player = object;
	pick_ring_bonus_players(&stage_rings_1, &stage_rings_2, &stage_rings_3);
	if ((int16_t)percent_fixed < (int16_t)bonus_percent)
	{
		stage_rings_1 = stage_rings_1 & 0xFFu;
		stage_rings_2 = stage_rings_2 & 0xFFu;
		stage_rings_3 = stage_rings_3 & 0xFFu;
	}
	if ((OBJECT(object_2)->flags & 0x10000400u) == 0u)
	{
		r4 = (uint16_t)stage_rings_1;
		allocate_object_outside_loop(&ignored, &object);
		OBJECT(object)->handler = ring_tally_player_1_update_ADDRESS;
		AS8(OBJECT(object)->flags) = 0u;
		OBJECT(object)->hitbox_height = AS8(PLAYER(player_1)->stage_rings);
		r4 = r4 >> 8;
		AT8(OBJECT(object)->hitbox_height, 1) = r4;
		AT8(OBJECT(object)->flags, 1) = 1u;
		AS8(OBJECT(object)->touched_by) = OBJECT(object_2)->kind;
		ring_tally_panels_counting++;
		OBJECT(object)->speed_x = 0x208400;
	}
	if ((OBJECT(object_3)->flags & 0x10000400u) == 0u)
	{
		allocate_object_outside_loop(&ignored, &object);
		OBJECT(object)->handler = ring_tally_player_2_update_ADDRESS;
		AS8(OBJECT(object)->flags) = 0u;
		OBJECT(object)->hitbox_height = AS8(PLAYER(player)->stage_rings);
		AT8(OBJECT(object)->hitbox_height, 1) = (uint16_t)stage_rings_2 >> 8;
		AT8(OBJECT(object)->flags, 1) = 2u;
		AS8(OBJECT(object)->touched_by) = OBJECT(object_3)->kind;
		ring_tally_panels_counting++;
		OBJECT(object)->speed_x = 0x208600;
	}
	out_flags = OBJECT(object_4)->flags & 0x10000400u;
	player_3_out_flags = out_flags;
	if (out_flags == 0u)
	{
		allocate_object_outside_loop(&ignored, &object);
		OBJECT(object)->handler = ring_tally_player_3_update_ADDRESS;
		AS8(OBJECT(object)->flags) = 0u;
		OBJECT(object)->hitbox_height = AS8(PLAYER(player_3)->stage_rings);
		stage_rings_3 = (uint16_t)stage_rings_3 >> 8;
		AT8(OBJECT(object)->hitbox_height, 1) = stage_rings_3;
		AT8(OBJECT(object)->flags, 1) = 3u;
		AS8(OBJECT(object)->touched_by) = OBJECT(object_4)->kind;
		ring_tally_panels_counting++;
		OBJECT(object)->speed_x = 0x208800;
	}
	allocate_object_outside_loop(&ignored, &object);
	OBJECT(object)->handler = ring_tally_percent_update_ADDRESS;
	AS8(OBJECT(object)->flags) = 0u;
	OBJECT(object)->hit_list_next = ring_tally_percent;
	allocate_object_outside_loop(&ignored, &object);
	OBJECT(object)->handler = ring_tally_ticker_update_ADDRESS;
	AS8(OBJECT(object)->flags) = 0u;
	allocate_object_outside_loop(&ignored, &object);
	OBJECT(object)->handler = ring_tally_end_sound_wait_ADDRESS;
	*p_r2 = (*p_r2 & 0xFFFF0000u) | (stage_rings_3 & 0xFFFFu);
	*p_r3 = player_3_out_flags;
	*p_r4 = r4;
	*p_r8 = object_3;
	*p_r9 = object_4;
	*p_object = object;
}

void pick_ring_bonus_players(uint32_t *p_player_1_rings, uint32_t *p_player_2_rings, uint32_t *p_player_3_rings)
{
	uint16_t player_1_rings = (uint16_t)*p_player_1_rings;
	uint16_t player_2_rings = (uint16_t)*p_player_2_rings;
	uint16_t player_3_rings = (uint16_t)*p_player_3_rings;

	if ((uint8_t)player_2_rings == (uint8_t)player_1_rings)
	{
		if ((uint8_t)player_3_rings == (uint8_t)player_1_rings)
			return;
		if ((int8_t)player_3_rings < (int8_t)player_1_rings)
		{
			player_3_rings &= 0xFFu;
			*p_player_3_rings = (*p_player_3_rings & 0xFFFF0000u) | player_3_rings;
			return;
		}
		player_1_rings &= 0xFFu;
		player_2_rings &= 0xFFu;
		*p_player_1_rings = (*p_player_1_rings & 0xFFFF0000u) | player_1_rings;
		*p_player_2_rings = (*p_player_2_rings & 0xFFFF0000u) | player_2_rings;
		return;
	}
	if ((uint8_t)player_3_rings == (uint8_t)player_2_rings)
	{
		if ((int8_t)player_2_rings < (int8_t)player_1_rings)
		{
			player_2_rings &= 0xFFu;
			player_3_rings &= 0xFFu;
			*p_player_2_rings = (*p_player_2_rings & 0xFFFF0000u) | player_2_rings;
			*p_player_3_rings = (*p_player_3_rings & 0xFFFF0000u) | player_3_rings;
			return;
		}
		player_1_rings &= 0xFFu;
		*p_player_1_rings = (*p_player_1_rings & 0xFFFF0000u) | player_1_rings;
		return;
	}
	if ((uint8_t)player_3_rings == (uint8_t)player_1_rings)
	{
		if ((int8_t)player_2_rings < (int8_t)player_1_rings)
		{
			player_2_rings &= 0xFFu;
			*p_player_2_rings = (*p_player_2_rings & 0xFFFF0000u) | player_2_rings;
			return;
		}
		player_1_rings &= 0xFFu;
		player_3_rings &= 0xFFu;
		*p_player_1_rings = (*p_player_1_rings & 0xFFFF0000u) | player_1_rings;
		*p_player_3_rings = (*p_player_3_rings & 0xFFFF0000u) | player_3_rings;
		return;
	}
	if ((int8_t)player_2_rings < (int8_t)player_1_rings)
	{
		if ((int8_t)player_3_rings < (int8_t)player_1_rings)
		{
			player_2_rings &= 0xFFu;
			player_3_rings &= 0xFFu;
			*p_player_2_rings = (*p_player_2_rings & 0xFFFF0000u) | player_2_rings;
			*p_player_3_rings = (*p_player_3_rings & 0xFFFF0000u) | player_3_rings;
			return;
		}
		player_1_rings &= 0xFFu;
		player_2_rings &= 0xFFu;
		*p_player_1_rings = (*p_player_1_rings & 0xFFFF0000u) | player_1_rings;
		*p_player_2_rings = (*p_player_2_rings & 0xFFFF0000u) | player_2_rings;
		return;
	}
	if ((int8_t)player_3_rings < (int8_t)player_2_rings)
	{
		player_1_rings &= 0xFFu;
		player_3_rings &= 0xFFu;
		*p_player_1_rings = (*p_player_1_rings & 0xFFFF0000u) | player_1_rings;
		*p_player_3_rings = (*p_player_3_rings & 0xFFFF0000u) | player_3_rings;
		return;
	}
	player_1_rings &= 0xFFu;
	player_2_rings &= 0xFFu;
	*p_player_1_rings = (*p_player_1_rings & 0xFFFF0000u) | player_1_rings;
	*p_player_2_rings = (*p_player_2_rings & 0xFFFF0000u) | player_2_rings;
}

void ring_tally_percent_update(void)
{
	uint32_t digit = 0, ignored = 0, object = 0, percent_bcd = 0, percent_digit = 0, shown_percent = 0;

	if (OBJECT(current_object)->ring_tally_percent_view.started != 0u)
	{
		if (stage_results_done != 0u)
		{
			delete_current_object();
			return;
		}
		queue_sprite_in_layer();
		current_object = OBJECT(object_cursor)->speed_y;
		queue_sprite_in_layer();
		if (ring_tally_counted != 0u)
		{
			shown_percent = OBJECT(object_cursor)->hit_list_next;
			update_ring_tally_percent();
			goto ring_tally_percent_set_digits;
		}
		OBJECT(object_cursor)->ring_tally_percent_view.roll_delay--;
		if (OBJECT(object_cursor)->ring_tally_percent_view.roll_delay == 0u)
		{
			OBJECT(object_cursor)->ring_tally_percent_view.roll_delay = 1u;
			next_random(&shown_percent, &ignored, &ignored);
		ring_tally_percent_set_digits:
			percent_bcd = shown_percent & 0xFFu;
			convert_to_bcd(&percent_bcd, &digit);
			current_object = OBJECT(object_cursor)->z;
			percent_bcd &= 0x1FFu;
			percent_digit = (digit & ~0xFFFFu) | percent_bcd;
			percent_digit = (percent_digit & ~0xFFFFu) | (percent_digit & 0xFu);
			OBJECT(current_object)->sprite_frame = percent_digit_sprites[percent_digit];
			current_object = OBJECT(object_cursor)->ring_tally_percent_view.tens_digit;
			percent_digit = (percent_digit & ~0xFFFFu) | percent_bcd;
			percent_digit = (percent_digit & ~0xFFFFu) | (percent_digit & 0xF0u);
			percent_digit = (percent_digit & ~0xFFFFu) | ((uint16_t)percent_digit >> 4);
			OBJECT(current_object)->sprite_frame = percent_digit_sprites[percent_digit];
			current_object = OBJECT(object_cursor)->animation;
			percent_digit = (percent_digit & ~0xFFFFu) | percent_bcd;
			percent_digit = (percent_digit & ~0xFFFFu) | (percent_digit & 0xF00u);
			OBJECT(current_object)->sprite_frame = percent_digit_sprites[(percent_digit & ~0xFFFFu) | ((uint16_t)percent_digit >> 8)];
		}
		current_object = OBJECT(object_cursor)->z;
		queue_sprite_in_layer();
		current_object = OBJECT(object_cursor)->ring_tally_percent_view.tens_digit;
		queue_sprite_in_layer();
		current_object = OBJECT(object_cursor)->animation;
		if (OBJECT(current_object)->sprite_frame == 0x9DF0u)
			queue_sprite_in_layer();
		return;
	}
	OBJECT(current_object)->ring_tally_percent_view.started = 1u;
	OBJECT(current_object)->sprite_frame = 0x9E38u;
	OBJECT(current_object)->sprite_palette = 0xC00u;
	OBJECT(current_object)->sprite_palette |= 0x8000u;
	OBJECT(current_object)->sprite_scale = 0x100u;
	OBJECT(current_object)->screen_y = 0x10;
	OBJECT(current_object)->screen_x = 0xA0;
	OBJECT(current_object)->sprite_layer = 0u;
	OBJECT(current_object)->sprite_flags = 0u;
	allocate_object_2(&ignored, &ignored, &object);
	OBJECT(current_object)->speed_y = object;
	OBJECT(object)->sprite_frame = 0x9DE0u;
	OBJECT(object)->sprite_palette = 0xBA0u;
	OBJECT(object)->sprite_palette |= 0x8000u;
	OBJECT(object)->sprite_scale = 0x100u;
	OBJECT(object)->screen_y = 0x30;
	OBJECT(object)->screen_x = 0xA0;
	OBJECT(object)->sprite_layer = 1u;
	OBJECT(object)->sprite_flags = 0u;
	allocate_object_2(&ignored, &ignored, &object);
	OBJECT(current_object)->z = object;
	OBJECT(object)->sprite_palette = 0xBF0u;
	OBJECT(object)->sprite_palette |= 0x8000u;
	OBJECT(object)->sprite_scale = 0x100u;
	OBJECT(object)->screen_y = 0x31;
	OBJECT(object)->screen_x = 0x9E;
	OBJECT(object)->sprite_layer = 0u;
	OBJECT(object)->sprite_flags = 0u;
	allocate_object_2(&ignored, &ignored, &object);
	OBJECT(current_object)->ring_tally_percent_view.tens_digit = object;
	OBJECT(object)->sprite_palette = 0xBF0u;
	OBJECT(object)->sprite_palette |= 0x8000u;
	OBJECT(object)->sprite_scale = 0x100u;
	OBJECT(object)->screen_y = 0x31;
	OBJECT(object)->screen_x = 0x8E;
	OBJECT(object)->sprite_layer = 0u;
	OBJECT(object)->sprite_flags = 0u;
	allocate_object_2(&ignored, &ignored, &object);
	OBJECT(current_object)->animation = object;
	OBJECT(object)->sprite_palette = 0xBF0u;
	OBJECT(object)->sprite_palette |= 0x8000u;
	OBJECT(object)->sprite_scale = 0x100u;
	OBJECT(object)->screen_y = 0x31;
	OBJECT(object)->screen_x = 0x7E;
	OBJECT(object)->sprite_layer = 0u;
	OBJECT(object)->sprite_flags = 0u;
	OBJECT(current_object)->ring_tally_percent_view.roll_delay = 1u;
	OBJECT(current_object)->ring_tally_percent_view.blink_delay = 2u;
	OBJECT(current_object)->ring_tally_percent_view.blink_index = 0u;
	OBJECT(current_object)->ring_tally_percent_view.results_timer = 0x12Cu;
}

void update_ring_tally_percent(void)
{
	uint16_t results_timer = 0;

	results_timer = OBJECT(object_cursor)->update_ring_tally_percent.results_timer;
	OBJECT(object_cursor)->update_ring_tally_percent.results_timer = results_timer - 1u;
	if (results_timer == 1u)
		stage_results_done = 1u;
	if ((int16_t)stage_ring_bonus_percents[stage] <= (int16_t)OBJECT(object_cursor)->hit_list_next)
	{
		OBJECT(object_cursor)->update_ring_tally_percent.blink_delay--;
		if (OBJECT(object_cursor)->update_ring_tally_percent.blink_delay == 0u)
		{
			OBJECT(object_cursor)->update_ring_tally_percent.blink_delay = 2u;
			OBJECT(object_cursor)->update_ring_tally_percent.blink_index++;
			OBJECT(object_cursor)->update_ring_tally_percent.blink_index &= 3u;
			current_object = OBJECT(object_cursor)->speed_y;
			OBJECT(current_object)->sprite_palette = ring_tally_blink_palettes[OBJECT(object_cursor)->update_ring_tally_percent.blink_index];
			OBJECT(current_object)->sprite_palette |= 0x8000u;
		}
	}
}

void ring_tally_end_sound_wait(void)
{
	uint8_t sound_id = 0;
	uint32_t ignored = 0;

	if (ring_tally_counted == 0u)
		return;
	if ((int16_t)ring_tally_percent < (int16_t)stage_ring_bonus_percents[stage])
	{
		sound_id = 0x73u;
		queue_sound(sound_id, &ignored, &ignored);
	}
	delete_current_object();
}

void ring_tally_ticker_update(void)
{
	uint8_t sound_id = 0;
	uint32_t ignored = 0;

	if (OBJECT(current_object)->ring_tally_ticker.started == 0u)
	{
		OBJECT(current_object)->ring_tally_ticker.started = 1u;
		OBJECT(current_object)->ring_tally_ticker.timer = 0;
	}
	if (ring_tally_counted != 0u)
	{
		delete_current_object();
		return;
	}
	OBJECT(current_object)->ring_tally_ticker.timer++;
	if (OBJECT(current_object)->ring_tally_ticker.timer >= 6)
	{
		OBJECT(current_object)->ring_tally_ticker.timer = 0;
		sound_id = 0x83u;
		queue_sound(sound_id, &ignored, &ignored);
	}
}

void ring_tally_player_1_update(void)
{
	uint32_t character = 0;

	if (OBJECT(current_object)->ring_tally_player.started != 0u)
	{
		update_ring_tally_panel();
		return;
	}
	character = OBJECT(current_object)->ring_tally_player.character;
	build_ring_tally_panel(character);
	OBJECT(current_object)->ring_tally_player.step_delay = 4u;
	OBJECT(object_cursor)->ring_tally_player.palette_delay = 2u;
	OBJECT(current_object)->ring_tally_player.step = 0u;
	OBJECT(current_object)->ring_tally_player.rings_counted = 0;
	OBJECT(current_object)->ring_tally_player.started = 1u;
}

void ring_tally_player_2_update(void)
{
	uint32_t character = 0;

	if (OBJECT(current_object)->ring_tally_player.started != 0u)
	{
		update_ring_tally_panel();
		return;
	}
	if ((cabinet_config & 1u) == 0u)
	{
		ring_tally_player_3_update();
		return;
	}
	character = OBJECT(current_object)->ring_tally_player.character;
	build_ring_tally_panel(character);
	OBJECT(object_cursor)->screen_x = 0xA0;
	mem_write16(OBJECT(current_object)->speed_y + 0xEu, 0xA0u);
	mem_write16(OBJECT(current_object)->z + 0xEu, 0xA0u);
	mem_write16(OBJECT(current_object)->ring_tally_player.part_3 + 0xEu, 0xA0u);
	mem_write16(OBJECT(current_object)->animation + 0xEu, 0xB6u);
	mem_write16(OBJECT(current_object)->x + 0xEu, 0xADu);
	mem_write16(OBJECT(current_object)->y + 0xEu, 0xA4u);
	OBJECT(current_object)->ring_tally_player.step_delay = 4u;
	OBJECT(object_cursor)->ring_tally_player.palette_delay = 2u;
	OBJECT(current_object)->ring_tally_player.step = 0u;
	OBJECT(current_object)->ring_tally_player.rings_counted = 0;
	OBJECT(current_object)->ring_tally_player.started = 1u;
}

void ring_tally_player_3_update(void)
{
	uint32_t character = 0;

	if (OBJECT(current_object)->ring_tally_player.started != 0u)
	{
		update_ring_tally_panel();
		return;
	}
	character = OBJECT(current_object)->ring_tally_player.character;
	build_ring_tally_panel(character);
	OBJECT(object_cursor)->screen_x = 0x100;
	mem_write16(OBJECT(current_object)->speed_y + 0xEu, 0x100u);
	mem_write16(OBJECT(current_object)->z + 0xEu, 0x100u);
	mem_write16(OBJECT(current_object)->ring_tally_player.part_3 + 0xEu, 0x100u);
	mem_write16(OBJECT(current_object)->animation + 0xEu, 0x116u);
	mem_write16(OBJECT(current_object)->x + 0xEu, 0x10Du);
	mem_write16(OBJECT(current_object)->y + 0xEu, 0x104u);
	OBJECT(current_object)->ring_tally_player.step_delay = 4u;
	OBJECT(object_cursor)->ring_tally_player.palette_delay = 2u;
	OBJECT(current_object)->ring_tally_player.step = 0u;
	OBJECT(current_object)->ring_tally_player.rings_counted = 0;
	OBJECT(current_object)->ring_tally_player.started = 1u;
}

void build_ring_tally_panel(uint32_t character)
{
	uint32_t ignored = 0, object = 0, portrait_record = 0;

	portrait_record = ring_tally_portraits[character];
	OBJECT(current_object)->sprite_palette = mem_read16(portrait_record);
	OBJECT(current_object)->sprite_palette |= 0x8000u;
	portrait_record += 2u;
	OBJECT(current_object)->sprite_frame = mem_read16(portrait_record);
	OBJECT(current_object)->sprite_flags = 0u;
	OBJECT(current_object)->sprite_scale = 0x100u;
	OBJECT(current_object)->screen_y = 0xA0;
	OBJECT(current_object)->screen_x = 0x40;
	OBJECT(current_object)->sprite_layer = 0u;
	OBJECT(current_object)->floor_block = portrait_record + 2u;
	allocate_object_2(&ignored, &ignored, &object);
	OBJECT(current_object)->speed_y = object;
	OBJECT(object)->sprite_frame = 0x9EF0u;
	if (OBJECT(current_object)->build_ring_tally_panel.gets_bonus == 0u)
		OBJECT(object)->sprite_frame = 0x9EF8u;
	OBJECT(object)->sprite_palette = 0xC40u;
	OBJECT(object)->sprite_palette |= 0x8000u;
	OBJECT(object)->screen_x = 0x40;
	OBJECT(object)->screen_y = 0x59;
	OBJECT(object)->sprite_layer = 0u;
	OBJECT(object)->sprite_flags = 0u;
	OBJECT(object)->sprite_scale = 0x100u;
	allocate_object_2(&ignored, &ignored, &object);
	OBJECT(current_object)->z = object;
	OBJECT(object)->screen_x = 0x40;
	OBJECT(object)->screen_y = 0xA0;
	OBJECT(object)->sprite_layer = 1u;
	OBJECT(object)->sprite_frame = 0x9DD8u;
	OBJECT(object)->sprite_palette = 0xC50u;
	OBJECT(object)->sprite_palette |= 0x8000u;
	OBJECT(object)->sprite_flags = 0u;
	OBJECT(object)->sprite_scale = 0x100u;
	allocate_object_2(&ignored, &ignored, &object);
	OBJECT(current_object)->build_ring_tally_panel.part_3 = object;
	OBJECT(object)->screen_x = 0x40;
	OBJECT(object)->screen_y = 0x70;
	OBJECT(object)->sprite_layer = 0u;
	OBJECT(object)->sprite_frame = 0x9E90u;
	OBJECT(object)->sprite_palette = 0xC20u;
	OBJECT(object)->sprite_palette |= 0x8000u;
	OBJECT(object)->sprite_flags = 0u;
	OBJECT(object)->sprite_scale = 0x100u;
	allocate_object_2(&ignored, &ignored, &object);
	OBJECT(current_object)->animation = object;
	OBJECT(object)->screen_x = 0x56;
	OBJECT(object)->screen_y = 0x75;
	OBJECT(object)->sprite_layer = 0u;
	OBJECT(object)->sprite_frame = 0x9E40u;
	OBJECT(object)->sprite_palette = 0xC10u;
	OBJECT(object)->sprite_palette |= 0x8000u;
	OBJECT(object)->sprite_flags = 0u;
	OBJECT(object)->sprite_scale = 0x100u;
	allocate_object_2(&ignored, &ignored, &object);
	OBJECT(current_object)->x = object;
	OBJECT(object)->screen_x = 0x4D;
	OBJECT(object)->screen_y = 0x75;
	OBJECT(object)->sprite_layer = 0u;
	OBJECT(object)->sprite_frame = 0x9E40u;
	OBJECT(object)->sprite_palette = 0xC10u;
	OBJECT(object)->sprite_palette |= 0x8000u;
	OBJECT(object)->sprite_flags = 0u;
	OBJECT(object)->sprite_scale = 0x100u;
	allocate_object_2(&ignored, &ignored, &object);
	OBJECT(current_object)->y = object;
	OBJECT(object)->screen_x = 0x44;
	OBJECT(object)->screen_y = 0x75;
	OBJECT(object)->sprite_layer = 0u;
	OBJECT(object)->sprite_frame = 0x9E40u;
	OBJECT(object)->sprite_palette = 0xC10u;
	OBJECT(object)->sprite_palette |= 0x8000u;
	OBJECT(object)->sprite_flags = 0u;
	OBJECT(object)->sprite_scale = 0x100u;
}

void update_ring_tally_panel(void)
{
	uint8_t sound_id = 0;
	uint32_t counted_bcd = 0, ignored = 0, speed_x = 0;

	if (stage_results_done != 0u)
	{
		delete_current_object();
		return;
	}
	switch (OBJECT(current_object)->update_ring_tally_panel.step)
	{
	case 0:
	update_ring_tally_panel_count_up:
		queue_sprite_in_layer();
		current_object = OBJECT(object_cursor)->update_ring_tally_panel.part_3;
		queue_sprite_in_layer();
		current_object = OBJECT(object_cursor)->animation;
		queue_sprite_in_layer();
		current_object = OBJECT(object_cursor)->y;
		if (OBJECT(current_object)->sprite_frame == 0x9E40u)
		{
			current_object = OBJECT(object_cursor)->x;
			if (OBJECT(current_object)->sprite_frame != 0x9E40u)
				queue_sprite_in_layer();
		}
		else
		{
			queue_sprite_in_layer();
			current_object = OBJECT(object_cursor)->x;
			queue_sprite_in_layer();
		}
		OBJECT(object_cursor)->update_ring_tally_panel.step_delay--;
		if (OBJECT(object_cursor)->update_ring_tally_panel.step_delay != 0u)
			return;
		if (OBJECT(object_cursor)->update_ring_tally_panel.ring_total <= OBJECT(object_cursor)->update_ring_tally_panel.rings_counted)
		{
			ring_tally_panels_counting--;
			if (ring_tally_panels_counting == 0u)
				ring_tally_counted = 1u;
			OBJECT(object_cursor)->update_ring_tally_panel.step++;
			if (OBJECT(object_cursor)->update_ring_tally_panel.gets_bonus == 0u)
			{
				OBJECT(object_cursor)->update_ring_tally_panel.step++;
				OBJECT(object_cursor)->update_ring_tally_panel.step++;
			}
		}
		counted_bcd = (uint8_t)OBJECT(object_cursor)->update_ring_tally_panel.rings_counted;
		convert_to_bcd(&counted_bcd, &ignored);
		current_object = OBJECT(object_cursor)->animation;
		OBJECT(current_object)->sprite_frame = ring_tally_digits[counted_bcd & 0xFu];
		current_object = OBJECT(object_cursor)->x;
		OBJECT(current_object)->sprite_frame = ring_tally_digits[(counted_bcd >> 4) & 0xFu];
		current_object = OBJECT(object_cursor)->y;
		OBJECT(current_object)->sprite_frame = ring_tally_digits[(counted_bcd >> 8) & 0xFu];
		OBJECT(object_cursor)->update_ring_tally_panel.step_delay = 2u;
		OBJECT(object_cursor)->update_ring_tally_panel.rings_counted++;
		return;
	case 1:
	case 5:
		speed_x = OBJECT(object_cursor)->speed_x;
		give_result_bonus(speed_x);
		sound_id = ring_tally_character_sounds[OBJECT(object_cursor)->update_ring_tally_panel.character];
		queue_sound(sound_id, &ignored, &ignored);
		sound_id = 0x88u;
		queue_sound(sound_id, &ignored, &ignored);
		OBJECT(object_cursor)->update_ring_tally_panel.step++;
		goto update_ring_tally_panel_count_up;
	case 2:
		OBJECT(object_cursor)->sprite_frame = mem_read16(OBJECT(object_cursor)->floor_block + 2u);
		queue_sprite_in_layer();
		current_object = OBJECT(object_cursor)->speed_y;
		queue_sprite_in_layer();
		cycle_ring_tally_label_palette();
		current_object = OBJECT(object_cursor)->z;
		queue_sprite_in_layer();
		current_object = OBJECT(object_cursor)->update_ring_tally_panel.part_3;
		queue_sprite_in_layer();
		current_object = OBJECT(object_cursor)->animation;
		queue_sprite_in_layer();
		current_object = OBJECT(object_cursor)->y;
		if (OBJECT(current_object)->sprite_frame == 0x9E40u)
		{
			current_object = OBJECT(object_cursor)->x;
			if (OBJECT(current_object)->sprite_frame != 0x9E40u)
				queue_sprite_in_layer();
		}
		else
		{
			queue_sprite_in_layer();
			current_object = OBJECT(object_cursor)->x;
			queue_sprite_in_layer();
		}
		update_ring_tally_panel_tail();
		return;
	case 3:
		sound_id = 0x89u;
		queue_sound(sound_id, &ignored, &ignored);
		OBJECT(object_cursor)->update_ring_tally_panel.step++;
		OBJECT(object_cursor)->sprite_frame = mem_read16(OBJECT(object_cursor)->floor_block);
		queue_sprite_in_layer();
		current_object = OBJECT(object_cursor)->speed_y;
		queue_sprite_in_layer();
		current_object = OBJECT(object_cursor)->update_ring_tally_panel.part_3;
		queue_sprite_in_layer();
		current_object = OBJECT(object_cursor)->animation;
		queue_sprite_in_layer();
		current_object = OBJECT(object_cursor)->y;
		if (OBJECT(current_object)->sprite_frame == 0x9E40u)
		{
			current_object = OBJECT(object_cursor)->x;
			if (OBJECT(current_object)->sprite_frame != 0x9E40u)
				queue_sprite_in_layer();
		}
		else
		{
			queue_sprite_in_layer();
			current_object = OBJECT(object_cursor)->x;
			queue_sprite_in_layer();
		}
		return;
	case 4:
		OBJECT(object_cursor)->sprite_frame = mem_read16(OBJECT(object_cursor)->floor_block);
		queue_sprite_in_layer();
		current_object = OBJECT(object_cursor)->speed_y;
		queue_sprite_in_layer();
		current_object = OBJECT(object_cursor)->update_ring_tally_panel.part_3;
		queue_sprite_in_layer();
		current_object = OBJECT(object_cursor)->animation;
		queue_sprite_in_layer();
		current_object = OBJECT(object_cursor)->y;
		if (OBJECT(current_object)->sprite_frame == 0x9E40u)
		{
			current_object = OBJECT(object_cursor)->x;
			if (OBJECT(current_object)->sprite_frame != 0x9E40u)
				queue_sprite_in_layer();
		}
		else
		{
			queue_sprite_in_layer();
			current_object = OBJECT(object_cursor)->x;
			queue_sprite_in_layer();
		}
		return;
	default:
		unknown_code(0x98C43u + OBJECT(current_object)->update_ring_tally_panel.step * 3u);
	}
}

void update_ring_tally_panel_tail(void)
{
}

void cycle_ring_tally_label_palette(void)
{
	OBJECT(object_cursor)->cycle_ring_tally_label_palette.palette_delay--;
	if (OBJECT(object_cursor)->cycle_ring_tally_label_palette.palette_delay != 0u)
		return;
	OBJECT(object_cursor)->cycle_ring_tally_label_palette.palette_delay = 2u;
	OBJECT(object_cursor)->cycle_ring_tally_label_palette.palette_index++;
	if (OBJECT(object_cursor)->cycle_ring_tally_label_palette.palette_index > 6)
		OBJECT(object_cursor)->cycle_ring_tally_label_palette.palette_index = 0;
	current_object = OBJECT(object_cursor)->z;
	OBJECT(current_object)->sprite_palette = ring_tally_label_palettes[(uint8_t)OBJECT(object_cursor)->cycle_ring_tally_label_palette.palette_index];
	OBJECT(current_object)->sprite_palette |= 0x8000u;
}

uint32_t wait_for_frames(void)
{
	uint32_t r8 = 0, ignored = 0;

	frames_to_wait = frames_per_wait;
	wait_for_frame(&ignored, &ignored, &ignored, &r8, &ignored, &ignored);
	return r8;
}

void wait_for_frame(uint32_t *p_r1, uint32_t *p_released, uint32_t *p_input_cursor, uint32_t *p_count, uint32_t *p_trackball_port, uint32_t *p_trackball_cursor)
{
	uint32_t r1 = 0, r2 = 0, r3 = 0, r8 = 0, r10 = 0, r11 = 0, inputs = 0, poll_count = 0;
	uint8_t frames_left = 0;

	poll_count = 0u;
	sprite_list_ready = 0xFFu;
	do
	{
		frame_wait(frames_to_wait_ADDRESS);
		frames_left = frames_to_wait;
		take_interrupts();
		poll_count--;
	} while ((poll_count != 0u) & ((int8_t)frames_left >= 0));
	frame_end();
	inputs = 0u - poll_count;
	frames_to_wait = frames_per_wait;
	sprite_buffer_flip &= 0xFF04u;
	AS16(sprite_list_start) = AS16(sprite_back_buffer);
	attract_timer++;
	read_controls(&inputs, &r1, &r2, &r3, &r8, &r10, &r11);
	*p_r1 = r1;
	*p_released = r2;
	*p_input_cursor = r3;
	*p_count = r8;
	*p_trackball_port = r10;
	*p_trackball_cursor = r11;
}

void read_controls(uint32_t *p_inputs, uint32_t *p_pressed, uint32_t *p_released, uint32_t *p_input_cursor, uint32_t *p_count, uint32_t *p_trackball_port, uint32_t *p_trackball_cursor)
{
	uint32_t inputs = *p_inputs;
	uint32_t axes_left = 0, count = 0, input_cursor = 0, pressed = 0, released = 0, trackball_cursor = 0, trackball_port = 0;

	inputs = (inputs & ~0xFFu) | mem_read8(io_port_b);
	inputs = (((inputs << 8) | (inputs >> 24)) & ~0xFFu) | mem_read8(io_port_a);
	inputs = (((inputs << 8) | (inputs >> 24)) & ~0xFFu) | mem_read8(io_port_f);
	inputs = (((inputs << 8) | (inputs >> 24)) & ~0xFFu) | mem_read8(io_port_e);
	inputs = play_demo_buttons(inputs);
	pressed = input_ports;
	input_ports = inputs;
	input_ports_active = ~inputs;
	released = ~pressed & inputs;
	inputs = ~inputs;
	pressed &= inputs;
	inputs_pressed = pressed;
	input_cursor = inputs_released_ADDRESS;
	inputs_released = released;
	trackball_port = IO_PORTS + 0x40u;
	trackball_cursor = trackball_1_x_ADDRESS;
	axes_left = 6u;
	do
	{
		inputs = (inputs & ~0xFFFFu) | (uint16_t)(int8_t)mem_read8(trackball_port);
		if ((int16_t)inputs <= 0x60 && (int16_t)inputs >= -0x60)
			mem_write16(trackball_cursor, inputs);
		trackball_cursor += 2u;
		axes_left--;
		trackball_port += 4u;
	} while (axes_left != 0u);
	trackball_port = IO_PORTS + 0x40u;
	inputs &= ~0xFFFFu;
	count = 3u;
	do
	{
		mem_write16(trackball_port, inputs);
		count--;
		trackball_port += 8u;
	} while (count != 0u);
	trackball_1_x = 0u - trackball_1_x;
	trackball_2_x = 0u - trackball_2_x;
	trackball_3_x = 0u - trackball_3_x;
	play_demo_trackballs(&count, &trackball_port, &trackball_cursor);
	*p_inputs = inputs;
	*p_pressed = pressed;
	*p_released = released;
	*p_input_cursor = input_cursor;
	*p_count = count;
	*p_trackball_port = trackball_port;
	*p_trackball_cursor = trackball_cursor;
}

uint32_t play_demo_buttons(uint32_t inputs)
{
	uint32_t demo_cursor = 0, ignored = 0;

	if ((int8_t)game_in_progress >= 0 && (int8_t)test_mode >= 0 && (current_screen == 4u || current_screen == 5u) && current_game_mode == 3u)
	{
		demo_patched_inputs = inputs;
		demo_cursor = demo_input;
		move_bits_down(demo_cursor, 0u, temp_0_ADDRESS, 0x19u, 1u, &ignored, &ignored);
		move_bits_down(demo_cursor, 1u, temp_0_ADDRESS, 0x1Au, 1u, &ignored, &ignored);
		move_bits_down(demo_cursor, 2u, temp_0_ADDRESS, 0x1Bu, 1u, &ignored, &ignored);
		inputs = demo_patched_inputs;
		demo_input++;
	}
	return inputs;
}

void play_demo_trackballs(uint32_t *p_count, uint32_t *p_demo_cursor, uint32_t *p_trackball_cursor)
{
	uint32_t count = *p_count;
	uint32_t demo_cursor = *p_demo_cursor;
	uint32_t trackball_cursor = *p_trackball_cursor;

	if ((int8_t)game_in_progress < 0 || (int8_t)test_mode < 0 || (current_screen != 4u && current_screen != 5u))
		return;
	if (current_game_mode != 3u)
		return;
	demo_cursor = demo_input;
	trackball_cursor = trackball_1_x_ADDRESS;
	count = 6u;
	do
	{
		mem_write16(trackball_cursor, (int8_t)mem_read8(demo_cursor));
		trackball_cursor += 2u;
		count--;
		demo_cursor++;
	} while (count != 0u);
	demo_input = demo_cursor;
	*p_count = count;
	*p_demo_cursor = demo_cursor;
	*p_trackball_cursor = trackball_cursor;
}

void square_root(uint32_t *p_value, uint32_t *p_r1)
{
	uint32_t value = *p_value;
	uint32_t carry = 0;
	uint32_t radicand = 0, root_remainder = 0, steps_left = 0;

	radicand = value;
	steps_left = 0x10u;
	root_remainder = 0u;
	value = 0u;
	for (;;)
	{
		carry = radicand * 2u < radicand;
		radicand = radicand * 2u;
		root_remainder = root_remainder * 2u + carry;
		carry = radicand * 2u < radicand;
		radicand = radicand * 2u;
		root_remainder = root_remainder * 2u + carry;
		value = value * 2u + 1u;
		if (root_remainder < value)
		{
			value--;
			steps_left--;
			if (steps_left == 0u)
			{
				value = value >> 1;
				*p_value = value;
				*p_r1 = steps_left;
				return;
			}
			continue;
		}
		root_remainder -= value;
		value++;
		steps_left--;
		if (steps_left != 0u)
			continue;
		break;
	}
	value = value >> 1;
	*p_value = value;
	*p_r1 = steps_left;
}

void angle_of_vector(uint32_t dy, uint32_t *p_dx, uint32_t *p_octant)
{
	uint32_t dx = *p_dx;
	uint32_t octant = 0, octant_index = 0;
	uint32_t abs_dx = 0;

	octant_index = 0u;
	dx = (uint16_t)dx;
	if ((int16_t)dx < 0)
	{
		dx = (uint16_t)(0u - dx);
		octant_index = (uint8_t)(octant_index + 1u);
	}
	dy = (uint16_t)dy;
	if ((int16_t)dy < 0)
	{
		dy = (uint16_t)(0u - dy);
		octant_index = (uint8_t)(octant_index + 2u);
	}
	if (dy < dx)
		dy = divide_u32(dy << 8, dx);
	else
	{
		abs_dx = dx;
		dx = dy;
		dy = abs_dx;
		octant_index = (uint8_t)(octant_index + 4u);
		if (dx != 0u)
		{
			dy = divide_u32(dy << 8, dx);
			dx = mem_read8(angle_ratios_ADDRESS + dy);
			octant = angle_octants[octant_index];
			if ((int16_t)octant >= 0)
			{
				dx = (uint16_t)(dx + octant);
				dx = dx & 0x3FFu;
				*p_dx = dx;
				*p_octant = octant;
				return;
			}
			octant = (uint16_t)~octant;
			dx = (uint16_t)(0u - dx);
			dx = (uint16_t)(dx + octant);
			dx = dx & 0x3FFu;
			*p_dx = dx;
			*p_octant = octant;
			return;
		}
	}
	dx = mem_read8(angle_ratios_ADDRESS + dy);
	octant = angle_octants[octant_index];
	if ((int16_t)octant >= 0)
	{
		dx = (uint16_t)(dx + octant);
		dx = dx & 0x3FFu;
		*p_dx = dx;
		*p_octant = octant;
		return;
	}
	octant = (uint16_t)~octant;
	dx = (uint16_t)(0u - dx);
	dx = (uint16_t)(dx + octant);
	dx = dx & 0x3FFu;
	*p_dx = dx;
	*p_octant = octant;
}

void convert_to_bcd(uint32_t *p_value, uint32_t *p_digit)
{
	uint32_t value = *p_value;
	uint32_t bcd = 0, digit = 0, digits_left = 0;

	digits_left = 8u;
	bcd = 0u;
	do
	{
		digit = 0u;
		divux_pair(0xAu, &value, &digit);
		bcd += digit;
		bcd = (bcd << 28) | (bcd >> 4);
		digits_left--;
	} while ((digits_left != 0u) & (value != 0u));
	if (value != 0u)
	{
		digit = (digit & ~0xFFu) | (uint8_t)value;
		digit = (digit & ~0xFFu) | ((uint8_t)digit % 0xAu);
		digit = (digit & ~0xFFu) | (uint8_t)(digit + ((uint32_t)((uint8_t)value / 0xAu) << 4));
		value = bcd;
		*p_value = value;
		*p_digit = digit;
		return;
	}
	if (digits_left == 0u)
	{
		value = bcd;
		*p_value = value;
		*p_digit = digit;
		return;
	}
	do
	{
		bcd = (bcd << 28) | (bcd >> 4);
		digits_left--;
	} while (digits_left != 0u);
	value = bcd;
	*p_value = value;
	*p_digit = digit;
}

void reset_objects(uint32_t *p_block, uint32_t *p_stack_slot)
{
	uint32_t r0 = 0, r3 = 0;

	reset_object_list();
	reset_object_pool_2(&r0, &r3);
	run_objects();
	*p_block = r0;
	*p_stack_slot = r3;
}

void run_objects(void)
{
	free_objects = free_object_stack_ADDRESS;
	if (object_count == 0u)
		return;
	first_object = first_object_address;
	last_object = last_object_address;
	object_cursor = last_object;
	do
	{
		object_cursor = LINKED(object_cursor, OBJECT(object_cursor)->next);
		current_object = object_cursor;
		call_code(OBJECT(current_object)->handler);
	} while ((uint16_t)object_cursor != (uint16_t)last_object);
	AS16(first_object_address) = first_object;
	AS16(last_object_address) = last_object;
}

void reset_object_list(void)
{
	uint16_t object_offset = 0;
	uint32_t objects_left = 0, stack_slot = 0;

	object_offset = 0u;
	first_object_address = 0x200000u;
	last_object_address = 0x200000u;
	objects_left = 0x80u;
	free_object_stack = 0x80;
	stack_slot = 0x20E902u;
	do
	{
		mem_write16(stack_slot, object_offset);
		objects_left--;
		stack_slot += 2u;
		object_offset += 0xA0u;
	} while (objects_left != 0u);
	object_count = objects_left;
}

void take_free_object(uint32_t *p_free_count, uint32_t *p_object)
{
	uint32_t object = *p_object;
	uint32_t free_count = 0;

	free_count = mem_read16(free_objects);
	if (free_count == 0u)
	{
		*p_free_count = free_count;
		return;
	}
	object = 0x200000u | mem_read16(free_objects + free_count * 2u);
	mem_write16(free_objects, mem_read16(free_objects) - 1u);
	*p_free_count = free_count;
	*p_object = object;
}

uint32_t allocate_object_or_start_list(uint32_t object)
{
	uint32_t ignored = 0, new_object = 0;

	if (object_count != 0u)
	{
		allocate_object(&ignored, &object);
		return object;
	}
	new_object = 0x200000u | mem_read16(free_objects + mem_read16(free_objects) * 2u);
	mem_write16(free_objects, mem_read16(free_objects) - 1u);
	AS16(first_object_address) = new_object;
	first_object = new_object;
	last_object = new_object;
	link_object(new_object);
	return new_object;
}

void allocate_object(uint32_t *p_free_count, uint32_t *p_object)
{
	uint32_t object = *p_object;
	uint32_t free_count = 0;

	free_count = mem_read16(free_objects);
	if (free_count == 0u)
	{
		*p_free_count = free_count;
		return;
	}
	object = 0x200000u | mem_read16(free_objects + free_count * 2u);
	mem_write16(free_objects, mem_read16(free_objects) - 1u);
	link_object(object);
	*p_free_count = free_count;
	*p_object = object;
}

void link_object(uint32_t object)
{
	mem_write16(object + 0x20u, last_object);
	OBJECT(last_object)->next = object;
	OBJECT(first_object)->previous = object;
	mem_write16(object + 0x22u, first_object);
	AS16(last_object_address) = object;
	last_object = object;
	object_count++;
}

void delete_current_object(void)
{
	uint32_t object = 0, object_2 = 0;

	object_count--;
	object = LINKED(object_cursor, OBJECT(object_cursor)->previous);
	object_2 = LINKED(object_cursor, OBJECT(object_cursor)->next);
	OBJECT(object)->next = object_2;
	OBJECT(object_2)->previous = object;
	if ((uint16_t)object_cursor == (uint16_t)first_object)
		first_object = (first_object & ~0xFFFFu) | (uint16_t)object_2;
	if ((uint16_t)object_cursor == (uint16_t)last_object)
		last_object = (last_object & ~0xFFFFu) | (uint16_t)object;
	current_object = object_cursor;
	object_cursor = (object_cursor & ~0xFFFFu) | (uint16_t)object;
	push_free_object();
}

void push_free_object(void)
{
	mem_write16(free_objects, mem_read16(free_objects) + 1u);
	mem_write16(free_objects + mem_read16(free_objects) * 2u, current_object);
}

void delete_object(uint32_t object, uint32_t *p_free_count, uint32_t *p_previous)
{
	uint32_t free_count = 0, object_2 = 0, object_3 = 0;

	object_count--;
	object_2 = LINKED(object, OBJECT(object)->previous);
	object_3 = LINKED(object, OBJECT(object)->next);
	OBJECT(object_3)->previous = object_2;
	OBJECT(object_2)->next = object_3;
	if ((uint16_t)object == (uint16_t)first_object)
		first_object = (first_object & ~0xFFFFu) | (uint16_t)object_3;
	if ((uint16_t)object != (uint16_t)last_object)
	{
		mem_write16(free_objects, mem_read16(free_objects) + 1u);
		free_count = mem_read16(free_objects);
		mem_write16(free_objects + free_count * 2u, object);
		*p_free_count = free_count;
		*p_previous = object_2;
		return;
	}
	last_object = (last_object & ~0xFFFFu) | (uint16_t)object_2;
	mem_write16(free_objects, mem_read16(free_objects) + 1u);
	free_count = mem_read16(free_objects);
	mem_write16(free_objects + free_count * 2u, object);
	*p_free_count = free_count;
	*p_previous = object_2;
}

void reset_object_pool_2(uint32_t *p_block, uint32_t *p_stack_slot)
{
	uint32_t block = 0, pool_blocks_left = 0, stack_slot = 0;

	block = 0x205000u;
	pool_blocks_left = 0xFFu;
	free_object_stack_2[0] = 0xFF;
	stack_slot = 0x20EA12u;
	do
	{
		mem_write16(stack_slot, block);
		pool_blocks_left--;
		stack_slot += 2u;
		block += 0x30u;
	} while (pool_blocks_left != 0u);
	*p_block = block;
	*p_stack_slot = stack_slot;
}

void allocate_object_2(uint32_t *p_free_count, uint32_t *p_free_stack, uint32_t *p_object)
{
	uint32_t object = *p_object;
	uint32_t free_count = 0, free_stack = 0;

	free_stack = free_object_stack_2_ADDRESS;
	free_count = (uint16_t)free_object_stack_2[0];
	if (free_count == 0u)
	{
		*p_free_count = free_count;
		*p_free_stack = free_stack;
		return;
	}
	object = 0x200000u | (uint16_t)free_object_stack_2[free_count];
	free_object_stack_2[0]--;
	*p_free_count = free_count;
	*p_free_stack = free_stack;
	*p_object = object;
}

uint32_t free_object_2(uint16_t object)
{
	uint32_t free_stack = 0;

	free_stack = free_object_stack_2_ADDRESS;
	free_object_stack_2[0]++;
	free_object_stack_2[(uint16_t)free_object_stack_2[0]] = object;
	return free_stack;
}

void load_text_font(uint32_t *p_pixels, uint32_t *p_color_index)
{
	uint8_t color = 0;
	uint16_t count = 0;
	uint32_t r4 = 0, r5 = 0, dest = 0, dest_end = 0, ignored = 0, source = 0, source_end = 0;

	mem_write16(video_text_control, 0x1F7u);
	move_string(text_font_ADDRESS, 0x9A0u, VIDEO_RAM + 0x1C000u, 0x9A0u, 2u, 0u, 0u, fill_value, &dest_end, &source_end);
	source = source_end;
	dest = dest_end;
	count = 0x5Bu;
	color = 1u;
	expand_font(color, count, source, dest, &r4, &r5);
	clear_text_layer(&ignored, &ignored, &ignored);
	*p_pixels = r4;
	*p_color_index = r5;
}

void clear_text_layer(uint32_t *p_value, uint32_t *p_count, uint32_t *p_dest)
{
	uint32_t r0 = 0, count = 0, dest = 0;

	dest = TEXT_CELL(0, 0);
	count = 0xE0u;
	clear_blocks(&r0, &count, &dest);
	*p_value = r0;
	*p_count = count;
	*p_dest = dest;
}

void expand_font(uint8_t color, uint32_t count, uint32_t source, uint32_t dest, uint32_t *p_pixels, uint32_t *p_color_index)
{
	uint32_t carry = 0;
	uint8_t font_row = 0;
	uint32_t char_rows_left = 0, color_index = 0, pairs_left = 0, pixel_mask = 0, pixels = 0, set_pixels = 0, shadow_pixels = 0;

	color_index = (uint8_t)color & 0xFu;
	count = (uint16_t)count;
	do
	{
		char_rows_left = 8u;
		shadow_pixels = 0u;
		do
		{
			font_row = mem_read8(source);
			pairs_left = 4u;
			pixels = 0u;
			source++;
			do
			{
				carry = (uint8_t)(font_row * 2u) < font_row;
				font_row = (uint8_t)(font_row * 2u);
				pixels = rotate_with_carry_by(pixels, 4u, carry, 32);
				carry = (uint8_t)(font_row * 2u) < font_row;
				font_row = (uint8_t)(font_row * 2u);
				pixels = rotate_with_carry_by(pixels, 4u, carry, 32);
				pairs_left--;
			} while (pairs_left != 0u);
			pixels = (pixels << 29) | (pixels >> 3);
			set_pixels = pixels;
			pixel_mask = pixels * 0xFu;
			pixels = pixels * color_index;
			mem_write32(dest, byte_swap32((shadow_pixels & ~pixel_mask) | pixels));
			shadow_pixels = (pixels + set_pixels) >> 4;
			char_rows_left--;
			dest += 4u;
		} while (char_rows_left != 0u);
		count--;
	} while (count != 0u);
	*p_pixels = pixels;
	*p_color_index = color_index;
}

void init_video_registers(void)
{
	uint8_t mode = 0;
	uint16_t value = 0;
	uint32_t count = 0, defaults_cursor = 0, ignored = 0, rects_left = 0, register_cursor = 0, right_bottom = 0;

	vblank_start_hook = apply_video_registers_ADDRESS;
	defaults_cursor = video_register_defaults_ADDRESS;
	register_cursor = video_control;
	count = 1u;
	copy_blocks(count, &defaults_cursor, &register_cursor);
	count = 4u;
	clear_blocks(&ignored, &count, &register_cursor);
	value = 0x200u;
	count = 6u;
	register_cursor = fill_halfwords(value, count, register_cursor);
	mem_write32(register_cursor, mem_read32(defaults_cursor));
	register_cursor += 4u;
	right_bottom = 0xDF013Fu;
	rects_left = 5u;
	defaults_cursor += 4u;
	do
	{
		mem_write32(register_cursor, 0u);
		register_cursor += 4u;
		mem_write32(register_cursor, right_bottom);
		rects_left--;
		register_cursor += 4u;
	} while (rects_left != 0u);
	mem_write32(register_cursor, mem_read32(defaults_cursor));
	mem_write32(register_cursor + 4u, mem_read32(defaults_cursor + 4u));
	mode = 0u;
	set_screen_mode(mode);
	ignored = clear_page_0_and_scroll();
}

uint32_t clear_page_0_and_scroll(void)
{
	uint32_t count = 0, dest = 0, ignored = 0;

	dest = 0x300000u;
	count = 0x40u;
	clear_blocks(&ignored, &count, &dest);
	dest = nbg0_scroll_x_ADDRESS;
	count = 8u;
	clear_blocks(&ignored, &count, &dest);
	return dest;
}

void apply_video_registers(void)
{
	move_halfwords(VIDEO_RAM + 0x1FF10u, video_registers_latched_ADDRESS, 0x20u);
	mem_write8(video_layer_opaque_flags, video_layer_opaque_latched);
	move_halfwords(video_registers_latched_ADDRESS, nbg0_scroll_x_ADDRESS, 0x21u);
}

uint32_t set_screen_mode(uint16_t mode)
{
	uint16_t control_bits = 0;
	uint32_t control_register = 0;

	mode = (uint8_t)(mode ^ (setting_monitor_flip & 2u));
	screen_mode = (uint8_t)mode;
	control_register = video_control;
	control_bits = (uint16_t)(mode & 1u);
	mem_write8(sprite_wide_screen, (uint8_t)control_bits);
	mem_write16(video_control, mem_read16(video_control) & 0x7DFFu);
	mode &= 2u;
	mode = (uint16_t)(((uint32_t)mode << 8) | (mode >> 8));
	control_bits = (uint16_t)(((uint32_t)control_bits << 15) | (control_bits >> 1) | mode);
	mode = (uint16_t)(mode * 3u);
	mem_write8(sprite_flip, ((uint32_t)mode << 7) | (mode >> 9));
	mem_write16(video_control, mem_read16(video_control) | control_bits);
	return control_register;
}

void copy_blocks(uint32_t count, uint32_t *p_source, uint32_t *p_dest)
{
	uint32_t source = *p_source;
	uint32_t dest = *p_dest;

	do
	{
		mem_write32(dest, mem_read32(source));
		dest += 4u;
		source += 4u;
		mem_write32(dest, mem_read32(source));
		dest += 4u;
		source += 4u;
		mem_write32(dest, mem_read32(source));
		dest += 4u;
		source += 4u;
		mem_write32(dest, mem_read32(source));
		dest += 4u;
		count--;
		source += 4u;
	} while (count != 0u);
	*p_source = source;
	*p_dest = dest;
}

void clear_blocks(uint32_t *p_value, uint32_t *p_count, uint32_t *p_dest)
{
	uint32_t count = *p_count;
	uint32_t dest = *p_dest;
	uint32_t value = 0;

	value = 0u;
	do
	{
		mem_write32(dest, value);
		dest += 4u;
		mem_write32(dest, value);
		dest += 4u;
		mem_write32(dest, value);
		dest += 4u;
		mem_write32(dest, value);
		count--;
		dest += 4u;
	} while (count != 0u);
	*p_value = value;
	*p_count = count;
	*p_dest = dest;
}

uint32_t fill_halfwords(uint16_t value, uint32_t count, uint32_t dest)
{
	do
	{
		mem_write16(dest, value);
		count--;
		dest += 2u;
	} while (count != 0u);
	return dest;
}

void draw_fraction(uint16_t attribute, uint32_t *p_numerator, uint32_t *p_denominator, uint32_t *p_text_position)
{
	uint16_t numerator = (uint16_t)*p_numerator;
	uint16_t denominator = (uint16_t)*p_denominator;
	uint32_t text_position = *p_text_position;

	attribute = (uint16_t)((uint32_t)attribute << 8);
	numerator &= 0xFu;
	denominator &= 0xFu;
	numerator = (uint8_t)(numerator - 0x80u) | attribute;
	denominator = (uint8_t)(denominator - 0x70u) | attribute;
	mem_write16(text_position, numerator);
	text_position += 2u;
	mem_write16(text_position, denominator);
	*p_numerator = (*p_numerator & 0xFFFF0000u) | numerator;
	*p_denominator = (*p_denominator & 0xFFFF0000u) | denominator;
	*p_text_position = text_position;
}

void print_text_list(uint32_t *p_text, uint32_t *p_text_position)
{
	uint32_t text = *p_text;
	uint32_t text_position = *p_text_position;

	while (mem_read16(text) != 0u)
	{
		print_text(&text, &text_position);
	}
	*p_text = text;
	*p_text_position = text_position;
}

void print_text(uint32_t *p_text, uint32_t *p_text_position)
{
	uint32_t text = *p_text;
	uint32_t r0 = 0, text_position = 0;

	text_position = 0x310000u | mem_read16(text);
	text += 2u;
	print_text_here(&r0, &text, &text_position);
	*p_text = text;
	*p_text_position = text_position;
}

void print_text_here(uint32_t *p_cell, uint32_t *p_text, uint32_t *p_text_position)
{
	uint32_t text = *p_text;
	uint32_t text_position = *p_text_position;
	uint16_t cell = 0;
	uint8_t next_char = 0;

	cell = (uint16_t)((uint32_t)mem_read8(text) << 8);
	text++;
	cell |= mem_read8(text);
	text++;
	if ((uint8_t)cell == 0u)
	{
		*p_cell = (*p_cell & 0xFFFF0000u) | cell;
		*p_text = text;
		return;
	}
	do
	{
		mem_write16(text_position, cell);
		next_char = mem_read8(text);
		text++;
		cell = (uint16_t)((cell & ~0xFFu) | next_char);
		text_position += 2u;
	} while ((uint8_t)cell != 0u);
	*p_cell = (*p_cell & 0xFFFF0000u) | cell;
	*p_text = text;
	*p_text_position = text_position;
}

void erase_text(uint32_t text)
{
	uint16_t cell = 0;
	uint32_t text_position = 0;

	cell = 0u;
	text_position = 0x310000u | mem_read16(text);
	text += 2u;
	text = erase_text_here(cell, text, text_position);
}

uint32_t erase_text_here(uint16_t cell, uint32_t text, uint32_t text_position)
{
	uint8_t first_char = 0, next_char = 0;

	text++;
	first_char = mem_read8(text);
	text++;
	if (first_char != 0u)
	{
		do
		{
			mem_write16(text_position, cell);
			next_char = mem_read8(text);
			text++;
			text_position += 2u;
		} while (next_char != 0u);
	}
	return text;
}

void print_text_lines(uint32_t *p_text, uint32_t *p_text_position)
{
	uint32_t text = *p_text;
	uint32_t text_position = *p_text_position;
	uint16_t text_cell = 0;
	uint32_t line_start = 0;

	line_start = text_position;
	text_cell = (uint16_t)((uint32_t)mem_read8(text) << 8);
	text++;
	for (;;)
	{
		text_cell = (uint16_t)((text_cell & ~0xFFu) | mem_read8(text));
		text++;
		if ((uint8_t)text_cell == 0u)
			break;
		if ((int8_t)text_cell >= 0)
		{
			mem_write16(text_position, text_cell);
			text_position += 2u;
			continue;
		}
		line_start += 0x80u;
		text_position = line_start;
	}
	*p_text = text;
	*p_text_position = text_position;
}

void erase_text_lines(uint32_t *p_text, uint32_t *p_text_position)
{
	uint32_t text = *p_text;
	uint32_t text_position = *p_text_position;
	uint32_t negative = 0;
	uint16_t blank_cell = 0;
	uint32_t line_start = 0;
	uint8_t text_char = 0;

	line_start = text_position;
	blank_cell = 0u;
	text++;
	for (;;)
	{
		text_char = mem_read8(text);
		negative = text_char >> 7;
		text++;
		if (text_char == 0u)
			break;
		if (negative == 0u)
		{
			mem_write16(text_position, blank_cell);
			text_position += 2u;
			continue;
		}
		line_start += 0x80u;
		text_position = line_start;
	}
	*p_text = text;
	*p_text_position = text_position;
}

void run_vram_script(uint32_t *p_count, uint32_t *p_script, uint32_t *p_dest)
{
	uint32_t count = *p_count;
	uint32_t script = *p_script;
	uint32_t dest = *p_dest;
	uint32_t dictionary_cursor = 0, script_code = 0;

	dictionary_cursor = vram_script_dictionary_ADDRESS;
	for (;;)
	{
		script_code = mem_read8(script);
		script++;
		if ((int8_t)script_code >= 0)
		{
			mem_write16(dest, vram_script_dictionary[script_code]);
			dest += 2u;
			continue;
		}
		script_code = script_code & 0x7Fu;
		switch (script_code)
		{
		case 0x0u:
			vram_script_set_address(script_code, &script, &dest);
			continue;
		case 0x3u:
			vram_script_skip(&script, &dest);
			continue;
		case 0x6u:
			vram_script_fill(&count, &script, &dest);
			continue;
		case 0x9u:
			vram_script_count_up(&count, &script, &dest);
			continue;
		case 0xCu:
			vram_script_count_down(&count, &script, &dest);
			continue;
		case 0xFu:
			vram_script_bytes(&count, &script, &dest);
			continue;
		case 0x12u:
			vram_script_words(&count, &script, &dest);
			continue;
		case 0x15u:
			vram_script_words_to_dictionary(&count, &script, &dest, &dictionary_cursor);
			continue;
		case 0x18u:
			vram_script_clear_pages(&script, &dest);
			continue;
		case 0x1Bu:
			vram_script_fill_down(&count, &script, &dest);
			continue;
		case 0x1Eu:
			vram_script_fill_down_wide(&count, &script, &dest);
			continue;
		case 0x21u:
			vram_script_fill_up(&count, &script, &dest);
			continue;
		case 0x24u:
			vram_script_repeat(dest, &count, &script);
			continue;
		case 0x27u:
			vram_script_skip_long(&script, &dest);
			continue;
		case 0x2Au:
			*p_count = count;
			*p_script = script;
			*p_dest = dest;
			return;
		default:
			unknown_code(0x9988Eu + script_code);
		}
	}
	*p_count = count;
	*p_script = script;
	*p_dest = dest;
}

void vram_script_end(void)
{
}

void vram_script_set_address(uint32_t r0, uint32_t *p_script, uint32_t *p_r11)
{
	uint32_t script = *p_script;
	uint32_t dest = 0;

	dest = 0x300000u + ((r0 & ~0xFFFFu) | mem_read16(script)) * 2u;
	script += 2u;
	*p_script = script;
	*p_r11 = dest;
}

void vram_script_skip(uint32_t *p_script, uint32_t *p_dest)
{
	uint32_t script = *p_script;
	uint32_t dest = *p_dest;

	dest += (int8_t)mem_read8(script) * 2u;
	script++;
	*p_script = script;
	*p_dest = dest;
}

void vram_script_skip_long(uint32_t *p_script, uint32_t *p_dest)
{
	uint32_t script = *p_script;
	uint32_t dest = *p_dest;

	dest += (int16_t)mem_read16(script) * 2u;
	script += 2u;
	*p_script = script;
	*p_dest = dest;
}

void vram_script_fill(uint32_t *p_count, uint32_t *p_script, uint32_t *p_dest)
{
	uint32_t script = *p_script;
	uint32_t dest = *p_dest;
	uint16_t tile = 0;
	uint32_t count = 0;

	count = mem_read8(script);
	script++;
	tile = mem_read16(script);
	script += 2u;
	do
	{
		mem_write16(dest, tile);
		count--;
		dest += 2u;
	} while (count != 0u);
	*p_count = count;
	*p_script = script;
	*p_dest = dest;
}

void vram_script_count_up(uint32_t *p_count, uint32_t *p_script, uint32_t *p_dest)
{
	uint32_t script = *p_script;
	uint32_t dest = *p_dest;
	uint16_t tile = 0;
	uint32_t count = 0;

	count = mem_read8(script);
	script++;
	tile = mem_read16(script);
	script += 2u;
	do
	{
		mem_write16(dest, tile);
		tile++;
		count--;
		dest += 2u;
	} while (count != 0u);
	*p_count = count;
	*p_script = script;
	*p_dest = dest;
}

void vram_script_count_down(uint32_t *p_count, uint32_t *p_script, uint32_t *p_dest)
{
	uint32_t script = *p_script;
	uint32_t dest = *p_dest;
	uint16_t tile = 0;
	uint32_t count = 0;

	count = mem_read8(script);
	script++;
	tile = mem_read16(script);
	script += 2u;
	do
	{
		mem_write16(dest, tile);
		tile--;
		count--;
		dest += 2u;
	} while (count != 0u);
	*p_count = count;
	*p_script = script;
	*p_dest = dest;
}

void vram_script_bytes(uint32_t *p_count, uint32_t *p_script, uint32_t *p_dest)
{
	uint32_t script = *p_script;
	uint32_t dest = *p_dest;
	uint16_t tile = 0;
	uint32_t count = 0;

	count = mem_read8(script);
	script++;
	tile = (uint16_t)((uint32_t)mem_read8(script) << 8);
	script++;
	do
	{
		tile = (uint16_t)((tile & ~0xFFu) | mem_read8(script));
		mem_write16(dest, tile);
		dest += 2u;
		count--;
		script++;
	} while (count != 0u);
	*p_count = count;
	*p_script = script;
	*p_dest = dest;
}

void vram_script_words(uint32_t *p_count, uint32_t *p_script, uint32_t *p_dest)
{
	uint32_t script = *p_script;
	uint32_t dest = *p_dest;
	uint32_t count = 0;

	count = mem_read8(script);
	script++;
	do
	{
		mem_write16(dest, mem_read16(script));
		dest += 2u;
		count--;
		script += 2u;
	} while (count != 0u);
	*p_count = count;
	*p_script = script;
	*p_dest = dest;
}

void vram_script_words_to_dictionary(uint32_t *p_count, uint32_t *p_script, uint32_t *p_dest, uint32_t *p_dictionary_cursor)
{
	uint32_t script = *p_script;
	uint32_t dest = *p_dest;
	uint32_t dictionary_cursor = *p_dictionary_cursor;
	uint16_t tile = 0;
	uint32_t count = 0;

	count = mem_read8(script);
	script++;
	do
	{
		tile = mem_read16(script);
		mem_write16(dest, tile);
		dest += 2u;
		mem_write16(dictionary_cursor, tile);
		dictionary_cursor = (dictionary_cursor & ~0xFFu) | (uint8_t)(dictionary_cursor + 2u);
		count--;
		script += 2u;
	} while (count != 0u);
	*p_count = count;
	*p_script = script;
	*p_dest = dest;
	*p_dictionary_cursor = (*p_dictionary_cursor & 0xFFFFFF00u) | (dictionary_cursor & 0xFFu);
}

void vram_script_clear_pages(uint32_t *p_script, uint32_t *p_dest)
{
	uint32_t script = *p_script;
	uint32_t dest = 0, halfword_count = 0;

	dest = 0x300000u + mem_read16(script) * 2u;
	script += 2u;
	halfword_count = (uint32_t)mem_read8(script) << 9;
	fill_value = 0u;
	script++;
	set_halfwords(dest, halfword_count, fill_value);
	*p_script = script;
	*p_dest = dest;
}

void vram_script_fill_down(uint32_t *p_count, uint32_t *p_script, uint32_t *p_dest)
{
	uint32_t script = *p_script;
	uint32_t dest = *p_dest;
	uint16_t tile = 0;
	uint32_t count = 0;

	count = mem_read8(script);
	script++;
	tile = mem_read16(script);
	script += 2u;
	do
	{
		mem_write16(dest, tile);
		dest += 0x40u;
		count--;
	} while (count != 0u);
	*p_count = count;
	*p_script = script;
	*p_dest = dest;
}

void vram_script_fill_up(uint32_t *p_count, uint32_t *p_script, uint32_t *p_dest)
{
	uint32_t script = *p_script;
	uint32_t dest = *p_dest;
	uint16_t tile = 0;
	uint32_t count = 0;

	count = mem_read8(script);
	script++;
	tile = mem_read16(script);
	script += 2u;
	do
	{
		mem_write16(dest, tile);
		dest -= 0x40u;
		count--;
	} while (count != 0u);
	*p_count = count;
	*p_script = script;
	*p_dest = dest;
}

void vram_script_fill_down_wide(uint32_t *p_count, uint32_t *p_script, uint32_t *p_dest)
{
	uint32_t script = *p_script;
	uint32_t dest = *p_dest;
	uint16_t tile = 0;
	uint32_t count = 0;

	count = mem_read8(script);
	script++;
	tile = mem_read16(script);
	script += 2u;
	do
	{
		mem_write16(dest, tile);
		dest += 0x80u;
		count--;
	} while (count != 0u);
	*p_count = count;
	*p_script = script;
	*p_dest = dest;
}

void vram_script_repeat(uint32_t dest, uint32_t *p_count, uint32_t *p_script)
{
	uint32_t script = *p_script;
	uint16_t tile = 0;
	uint32_t count = 0;

	count = mem_read8(script);
	script++;
	tile = mem_read16(script);
	script += 2u;
	do
	{
		mem_write16(dest, tile);
		count--;
	} while (count != 0u);
	*p_count = count;
	*p_script = script;
}

uint32_t draw_hex_2(uint32_t value, uint32_t attribute, uint32_t text_position)
{
	uint32_t digit_count = 0;

	value = (uint8_t)value;
	digit_count = 2u;
	draw_hex_digits(digit_count, &value, &attribute, &text_position);
	return value;
}

void draw_hex_4(uint32_t *p_value, uint32_t *p_attribute, uint32_t *p_text_position)
{
	uint32_t value = *p_value;
	uint32_t attribute = *p_attribute;
	uint32_t text_position = *p_text_position;
	uint32_t digit_count = 0;

	value = (uint16_t)value;
	digit_count = 4u;
	draw_hex_digits(digit_count, &value, &attribute, &text_position);
	*p_value = value;
	*p_attribute = (*p_attribute & 0xFFFFFF00u) | (attribute & 0xFFu);
	*p_text_position = text_position;
}

void draw_hex_8(uint32_t value, uint32_t attribute, uint32_t text_position)
{
	uint32_t digit_count = 0;

	digit_count = 8u;
	draw_hex_digits(digit_count, &value, &attribute, &text_position);
}

void draw_hex_digits(uint32_t digit_count, uint32_t *p_value, uint32_t *p_attribute, uint32_t *p_text_position)
{
	uint32_t value = *p_value;
	uint16_t attribute = (uint16_t)*p_attribute;
	uint32_t text_position = *p_text_position;
	uint32_t remaining = 0;

	text_position += digit_count * 2u;
	do
	{
		attribute = (uint16_t)((attribute & ~0xFFu) | (uint8_t)value);
		attribute = (uint16_t)((attribute & ~0xFFu) | (attribute & 0xFu));
		if ((uint8_t)attribute >= 0xAu)
			attribute = (uint16_t)((attribute & ~0xFFu) | (uint8_t)(attribute + 7u));
		attribute = (uint16_t)((attribute & ~0xFFu) | (uint8_t)(attribute + 0x30u));
		text_position -= 2u;
		mem_write16(text_position, attribute);
		remaining = value >> 4;
		value = remaining;
		digit_count--;
	} while ((digit_count != 0u) & (remaining != 0u));
	if (digit_count == 0u)
	{
		*p_value = value;
		*p_attribute = (*p_attribute & 0xFFFFFF00u) | (attribute & 0xFFu);
		*p_text_position = text_position;
		return;
	}
	do
	{
		text_position -= 2u;
		mem_write16(text_position, value);
		digit_count--;
	} while (digit_count != 0u);
	*p_value = value;
	*p_attribute = (*p_attribute & 0xFFFFFF00u) | (attribute & 0xFFu);
	*p_text_position = text_position;
}

void draw_credit_digits(uint32_t value, uint32_t *p_attribute, uint32_t *p_text_position)
{
	uint16_t attribute = (uint16_t)*p_attribute;
	uint32_t text_position = *p_text_position;
	uint8_t shift = 0;
	uint32_t digits_left = 0;
	uint32_t remaining = 0;

	value = (uint8_t)value;
	digits_left = 2u;
	text_position += digits_left * 2u;
	shift = 0xFCu;
	do
	{
		attribute = (uint16_t)((attribute & ~0xFFu) | (uint8_t)value);
		attribute = (uint16_t)((attribute & ~0xFFu) | (attribute & 0xFu));
		attribute = (uint16_t)((attribute & ~0xFFu) | (uint8_t)(attribute + 1u));
		text_position -= 2u;
		mem_write16(text_position, attribute);
		remaining = shift_by(value, shift, 32);
		value = remaining;
		digits_left--;
	} while ((digits_left != 0u) & (remaining != 0u));
	if (digits_left == 0u)
	{
		*p_attribute = (*p_attribute & 0xFFFFFF00u) | (attribute & 0xFFu);
		*p_text_position = text_position;
		return;
	}
	do
	{
		text_position -= 2u;
		mem_write16(text_position, value);
		digits_left--;
	} while (digits_left != 0u);
	*p_attribute = (*p_attribute & 0xFFFFFF00u) | (attribute & 0xFFu);
	*p_text_position = text_position;
}

void init_sprites(uint32_t *p_end_mark, uint32_t *p_source)
{
	uint16_t end_mark = 0;
	uint32_t control_cursor = 0, control_init = 0, dest_end = 0, ignored = 0, list_start = 0, registers_left = 0;

	vblank_hook = swap_sprite_buffers_ADDRESS;
	frames_to_wait = 0;
	frames_per_wait = 0u;
	control_init = sprite_control_init_ADDRESS;
	mem_write16(sprite_flip, setting_monitor_flip);
	registers_left = 4u;
	control_cursor = SPRITE_CONTROL + 0x6u;
	do
	{
		mem_write16(control_cursor, ROM8(control_init));
		registers_left--;
		control_init++;
		control_cursor += 2u;
	} while (registers_left != 0u);
	move_string(sprite_list_header_ADDRESS, 0x10u, 0x400000u, 0x10u, 2u, 0u, 0u, fill_value, &dest_end, &ignored);
	list_start = dest_end;
	sprite_jump_init = 0xA400u;
	sprite_buffer_flip = 0u;
	sprite_back_buffer = list_start;
	sprite_list_start = list_start;
	end_mark = 0xFFFFu;
	mem_write16(list_start, 0xFFFFu);
	mem_write16(list_start + 0x1FF0u, 0xFFFFu);
	mem_write16(list_start + 0x3FE0u, 0xFFFFu);
	mem_write16(list_start + 0x5FD0u, 0xFFFFu);
	sprite_list_end_command = 0xFFFFu;
	sprite_frames = 0x130000u;
	ignored = clear_sprite_buckets();
	*p_end_mark = (*p_end_mark & 0xFFFF0000u) | end_mark;
	*p_source = control_init;
}

uint32_t clear_sprite_buckets(void)
{
	uint32_t bucket = 0, buckets_left = 0;

	sprite_buckets_end = 0xFFFFu;
	buckets_left = 0x100u;
	bucket = sprite_buckets_ADDRESS;
	do
	{
		mem_write16(bucket, bucket);
		bucket += 2u;
		buckets_left--;
	} while (buckets_left != 0u);
	return bucket;
}

uint32_t queue_sprite(uint32_t depth)
{
	uint32_t ignored = 0;

	depth = (depth & ~0xFFFFu) | (uint16_t)OBJECT(current_object)->screen_y;
	queue_sprite_at_depth(&ignored, &depth);
	return depth;
}

void queue_sprite_at_depth(uint32_t *p_bucket_head, uint32_t *p_depth)
{
	uint32_t depth = *p_depth;
	uint32_t bucket_head = 0;

	depth = (depth & ~0xFFFFu) | (uint16_t)(depth + 0x10u);
	if ((int16_t)depth < 0xFE)
	{
		if ((int16_t)depth >= 0)
		{
			bucket_head = sprite_buckets_ADDRESS + ((uint16_t)(0xFFu - depth) & 0xFFu) * 2u;
			OBJECT(current_object)->draw_next = mem_read16(bucket_head);
			mem_write16(bucket_head, current_object);
			*p_bucket_head = bucket_head;
			*p_depth = depth;
			return;
		}
		depth = 0u;
		bucket_head = sprite_buckets_ADDRESS + ((uint16_t)(0xFFu - depth) & 0xFFu) * 2u;
		OBJECT(current_object)->draw_next = mem_read16(bucket_head);
		mem_write16(bucket_head, current_object);
		*p_bucket_head = bucket_head;
		*p_depth = depth;
		return;
	}
	depth = 0xFEu;
	bucket_head = sprite_buckets_ADDRESS + ((uint16_t)(0xFFu - depth) & 0xFFu) * 2u;
	OBJECT(current_object)->draw_next = mem_read16(bucket_head);
	mem_write16(bucket_head, current_object);
	*p_bucket_head = bucket_head;
	*p_depth = depth;
}

void queue_sprite_in_layer(void)
{
	uint32_t bucket_head = 0;

	bucket_head = sprite_buckets_ADDRESS + (OBJECT(current_object)->sprite_layer & 0xFFu) * 2u;
	OBJECT(current_object)->draw_next = mem_read16(bucket_head);
	mem_write16(bucket_head, current_object);
}

void queue_sprite_on_top(void)
{
	uint8_t bucket = 0;

	bucket = 0u;
	queue_sprite_in_bucket(bucket);
}

void queue_sprite_in_bucket(uint32_t bucket)
{
	bucket = sprite_buckets_ADDRESS + (bucket & 0xFFu) * 2u;
	OBJECT(current_object)->draw_next = mem_read16(bucket);
	mem_write16(bucket, current_object);
}

uint32_t queue_sprite_at_floor(void)
{
	uint32_t bucket_head = 0, floor_y = 0;

	floor_y = (int32_t)((uint32_t)OBJECT(current_object)->projected_y + OBJECT(current_object)->floor_z) >> 16;
	floor_y = (floor_y & ~0xFFFFu) | (uint16_t)(floor_y - view_y);
	floor_y = (floor_y & ~0xFFFFu) | (uint16_t)(floor_y + 0x10u);
	if ((int16_t)floor_y < 0xFE)
	{
		if ((int16_t)floor_y < 0)
			floor_y = 0u;
	}
	else
		floor_y = 0xFEu;
	bucket_head = sprite_buckets_ADDRESS + ((uint16_t)(0xFFu - floor_y) & 0xFFu) * 2u;
	OBJECT(current_object)->draw_next = mem_read16(bucket_head);
	mem_write16(bucket_head, current_object);
	return floor_y;
}

uint32_t queue_sprite_at_previous_floor(void)
{
	uint32_t bucket_head = 0, floor_y = 0;

	floor_y = (int32_t)((uint32_t)OBJECT(current_object)->projected_y + OBJECT(current_object)->previous_floor_z) >> 16;
	floor_y = (floor_y & ~0xFFFFu) | (uint16_t)(floor_y - view_y);
	floor_y = (floor_y & ~0xFFFFu) | (uint16_t)(floor_y + 0x10u);
	if ((int16_t)floor_y < 0xFE)
	{
		if ((int16_t)floor_y < 0)
			floor_y = 0u;
	}
	else
		floor_y = 0xFEu;
	bucket_head = sprite_buckets_ADDRESS + ((uint16_t)(0xFFu - floor_y) & 0xFFu) * 2u;
	OBJECT(current_object)->draw_next = mem_read16(bucket_head);
	mem_write16(bucket_head, current_object);
	return floor_y;
}

uint32_t queue_sprite_at_base(uint32_t depth)
{
	uint32_t bucket_head = 0;

	depth = (depth & ~0xFFFFu) | (uint16_t)OBJECT(current_object)->screen_y;
	depth = (depth & ~0xFFFFu) | (uint16_t)(depth - OBJECT(current_object)->sprite_offset_y);
	depth = (depth & ~0xFFFFu) | (uint16_t)(depth + 0x10u);
	if ((int16_t)depth < 0xFE)
	{
		if ((int16_t)depth < 0)
			depth = 0u;
	}
	else
		depth = 0xFEu;
	bucket_head = sprite_buckets_ADDRESS + ((uint16_t)(0xFFu - depth) & 0xFFu) * 2u;
	OBJECT(current_object)->draw_next = mem_read16(bucket_head);
	mem_write16(bucket_head, current_object);
	return depth;
}

uint32_t depth_to_bucket(uint32_t bucket, uint16_t depth)
{
	depth += 0x10u;
	if ((int16_t)depth < 0xFE)
	{
		if ((int16_t)depth < 0)
			depth = 0u;
	}
	else
		depth = 0xFEu;
	bucket = (bucket & ~0xFFFFu) | 0xFFu;
	bucket = (bucket & ~0xFFFFu) | (uint16_t)(bucket - depth);
	return bucket;
}

void queue_sprite_at_previous_floor_2(void)
{
	uint16_t floor_depth = 0;
	uint32_t bucket_head = 0;

	floor_depth = (uint16_t)(((int32_t)((uint32_t)OBJECT(current_object)->projected_y + OBJECT(current_object)->previous_floor_z) >> 16) - view_y + 0x10u);
	if ((int16_t)floor_depth < 0xFE)
	{
		if ((int16_t)floor_depth < 0)
			floor_depth = 0u;
	}
	else
		floor_depth = 0xFEu;
	bucket_head = sprite_buckets_ADDRESS + ((uint16_t)(0xFFu - floor_depth) & 0xFFu) * 2u;
	OBJECT(current_object)->draw_next = mem_read16(bucket_head);
	mem_write16(bucket_head, current_object);
}

void swap_sprite_buffers(void)
{
	uint32_t swap_entry = 0;
	uint8_t status = 0;

	if (sprite_list_ready == 0u)
		return;
	sprite_list_ready = 0u;
	sprite_buffer_flip += 2u;
	sprite_buffer_flip += 2u;
	swap_entry = sprite_buffer_swaps_ADDRESS + (6u & sprite_buffer_flip) * 2u;
	AS16(sprite_back_buffer) = ROM16(swap_entry);
	mem_write16(sprite_list_jump, ROM16(swap_entry + 2u));
	status = mem_read8(sprite_status);
}

void build_sprite_list(uint32_t *p_height, uint32_t *p_scale_x, uint32_t *p_scale_y, uint32_t *p_sprite_flags, uint32_t *p_sprite_entry)
{
	uint32_t height = *p_height;
	uint32_t scale_x = *p_scale_x;
	uint32_t scale_y = *p_scale_y;
	uint16_t sprite_flags = (uint16_t)*p_sprite_flags;
	uint8_t scale_shift = 0;
	uint16_t dest_height = 0, dest_width = 0, part_dest_width = 0, screen_x = 0, screen_y = 0, sized_width = 0, source_width_2 = 0;
	uint32_t r4 = 0, flags_word = 0, frame_data = 0, ignored = 0, object = 0, part_scale = 0, parts_left = 0, rounding = 0, source_width = 0, sprite_entry = 0, sprite_scale = 0;
	uint16_t part_width = 0, part_height = 0, width_bits = 0;

	sprite_entry = sprite_list_start;
	last_object = 0x20FC02u;
	frame_data = sprite_frames;
	rounding = 0x80u;
	scale_shift = 0xF8u;
	for (;;)
	{
		last_object = last_object - 2u;
		current_object = LINKED(current_object, OBJECT(last_object)->draw_next);
		if ((uint16_t)last_object == (uint16_t)current_object)
			continue;
		if ((uint16_t)last_object < (uint16_t)current_object)
		{
			*p_height = height;
			*p_scale_x = scale_x;
			*p_scale_y = scale_y;
			*p_sprite_flags = (*p_sprite_flags & 0xFFFF0000u) | sprite_flags;
			*p_sprite_entry = sprite_entry;
			return;
		}
		do
		{
			sprite_flags = OBJECT(current_object)->sprite_flags;
			flags_word = sprite_flags;
			flags_word = (uint16_t)((flags_word << 6) | (flags_word >> 10));
			switch (flags_word & 0x34u)
			{
			case 0x0u:
			case 0x37u:
				frame_data = (frame_data & ~0xFFFFu) | OBJECT(current_object)->sprite_frame;
				sprite_scale = OBJECT(current_object)->sprite_scale;
				source_width = mem_read16(frame_data);
				frame_data += 2u;
				height = (height & ~0xFFFFu) | mem_read16(frame_data);
				dest_width = (uint16_t)shift_by(source_width * sprite_scale + rounding, scale_shift, 32);
				dest_height = (uint16_t)shift_by((uint8_t)height * sprite_scale + rounding, scale_shift, 32);
				source_width = ((uint16_t)(source_width << 6) & ~0xFFu) | (uint8_t)height;
				height &= ~0xFFu;
				height = (height & ~0xFFFFu) | (uint16_t)(height | dest_height);
				frame_data += 2u;
			build_sprite_list_write_entry:
				dest_width |= mem_read16(frame_data);
				mem_write16(sprite_entry, sprite_flags);
				sprite_entry += 2u;
				mem_write16(sprite_entry, rotate_by(source_width, scale_shift, 16));
				sprite_entry += 2u;
				mem_write16(sprite_entry, height);
				sprite_entry += 2u;
				mem_write16(sprite_entry, dest_width);
				sprite_entry += 2u;
				mem_write32(sprite_entry, AS32(OBJECT(current_object)->screen_y));
				sprite_entry += 4u;
				frame_data += 2u;
				mem_write16(sprite_entry, mem_read16(frame_data));
				sprite_entry += 2u;
				mem_write16(sprite_entry, OBJECT(current_object)->sprite_palette);
				sprite_entry += 2u;
				break;
			case 0x3u:
			case 0x4u:
				frame_data = (frame_data & ~0xFFFFu) | OBJECT(current_object)->sprite_frame;
				source_width = mem_read16(frame_data);
				dest_width = (uint16_t)source_width;
				frame_data += 2u;
				height = (height & ~0xFFFFu) | mem_read16(frame_data);
				source_width = ((uint16_t)(source_width << 6) & ~0xFFu) | (uint8_t)height;
				sprite_flags &= 0x2FFFu;
				frame_data += 2u;
				goto build_sprite_list_write_entry;
			case 0x7u:
			case 0x8u:
			case 0x9u:
			case 0xAu:
			case 0xBu:
			case 0xCu:
			case 0xDu:
			case 0xEu:
			case 0xFu:
			case 0x10u:
			case 0x13u:
			case 0x14u:
				object = current_object + 2u;
				mem_write32(sprite_entry, AS32(OBJECT(object - 0x2u)->sprite_flags));
				sprite_entry += 4u;
				object += 4u;
				mem_write32(sprite_entry, AS32(OBJECT(object - 0x6u)->sprite_palette));
				sprite_entry += 4u;
				object += 4u;
				mem_write32(sprite_entry, AS32(OBJECT(object - 0xAu)->sprite_layer));
				sprite_entry += 4u;
				mem_write32(sprite_entry, AS32(OBJECT(object - 0xAu)->screen_x));
				sprite_entry += 4u;
				break;
			case 0x17u:
			case 0x18u:
			case 0x19u:
			case 0x1Au:
			case 0x1Bu:
			case 0x1Cu:
			case 0x1Du:
			case 0x1Eu:
			case 0x1Fu:
			case 0x20u:
				mem_write16(sprite_entry, (sprite_entry >> 4) - 0x7FFFu);
				sprite_entry += 2u;
				mem_write32(sprite_entry, AS32(OBJECT(current_object)->screen_y));
				sprite_entry += 0xEu;
				break;
			case 0x23u:
			case 0x24u:
				sprite_flags &= 0x2FFFu;
				frame_data = (frame_data & ~0xFFFFu) | OBJECT(current_object)->sprite_frame;
				source_width = mem_read16(frame_data);
				frame_data += 2u;
				height = (height & ~0xFFFFu) | mem_read16(frame_data);
				source_width = rotate_by(((uint16_t)(source_width << 6) & ~0xFFu) | (uint8_t)height, scale_shift, 16);
				height &= ~0xFFu;
				height = (height & ~0xFFFFu) | (uint16_t)(height | OBJECT(current_object)->sprite_height);
				frame_data += 2u;
				sized_width = (uint16_t)(OBJECT(current_object)->sprite_width | mem_read16(frame_data));
				mem_write16(sprite_entry, sprite_flags);
				sprite_entry += 2u;
				mem_write16(sprite_entry, source_width);
				sprite_entry += 2u;
				mem_write16(sprite_entry, height);
				sprite_entry += 2u;
				mem_write16(sprite_entry, sized_width);
				sprite_entry += 2u;
				mem_write32(sprite_entry, AS32(OBJECT(current_object)->screen_y));
				sprite_entry += 4u;
				mem_write16(sprite_entry, OBJECT(current_object)->sprite_scale);
				sprite_entry += 2u;
				mem_write16(sprite_entry, OBJECT(current_object)->sprite_palette);
				sprite_entry += 2u;
				frame_data += 2u;
				break;
			case 0x27u:
			case 0x28u:
			case 0x29u:
			case 0x2Au:
			case 0x2Bu:
			case 0x2Cu:
			case 0x2Du:
			case 0x2Eu:
			case 0x2Fu:
			case 0x30u:
				part_scale = OBJECT(current_object)->sprite_scale;
			build_sprite_list_write_parts:
				switch ((sprite_flags >> 5) & 6u)
				{
				case 0x0u:
					scale_x = part_scale;
				build_sprite_list_y_not_flipped:
					scale_y = part_scale;
					break;
				case 0x2u:
					scale_x = 0u - part_scale;
					goto build_sprite_list_y_not_flipped;
				case 0x4u:
					scale_x = part_scale;
					scale_y = 0u - part_scale;
					break;
				case 0x6u:
					scale_x = 0u - part_scale;
					scale_y = 0u - part_scale;
					break;
				default:
					unknown_code(0x9A3C4u + ((sprite_flags >> 5) & 6u));
				}
				sprite_flags &= 0xF0u;
				screen_x = (uint16_t)OBJECT(current_object)->screen_x;
				screen_y = (uint16_t)OBJECT(current_object)->screen_y;
				r14 = OBJECT(current_object)->part_list;
				parts_left = mem_read16(r14);
				r14 = r14 + 2u;
				if (parts_left >= 0x11u)
				{
					int_default(&flags_word, &source_width, &ignored, &r4);
					*p_height = r4;
					*p_scale_x = scale_x;
					*p_scale_y = scale_y;
					*p_sprite_flags = (*p_sprite_flags & 0xFFFF0000u) | sprite_flags;
					*p_sprite_entry = sprite_entry;
					return;
				}
				do
				{
					frame_data = (frame_data & ~0xFFFFu) | mem_read16(r14);
					part_width = mem_read16(frame_data);
					frame_data += 2u;
					source_width_2 = part_width;
					part_height = mem_read16(frame_data);
					frame_data += 2u;
					height = (height & ~0xFFFFu) | part_height;
					part_dest_width = (uint16_t)shift_by(source_width_2 * part_scale + rounding, scale_shift, 32);
					dest_height = (uint16_t)shift_by((uint8_t)height * part_scale + rounding, scale_shift, 32);
					source_width_2 = (uint16_t)(((uint16_t)((uint32_t)source_width_2 << 6) & ~0xFFu) | (uint8_t)height);
					height &= ~0xFFu;
					height = (height & ~0xFFFFu) | (uint16_t)(height | dest_height);
					width_bits = mem_read16(frame_data);
					frame_data += 2u;
					r14 = r14 + 2u;
					mem_write16(sprite_entry, mem_read16(r14) ^ sprite_flags);
					sprite_entry += 2u;
					mem_write16(sprite_entry, rotate_by(source_width_2, scale_shift, 16));
					sprite_entry += 2u;
					mem_write16(sprite_entry, height);
					sprite_entry += 2u;
					mem_write16(sprite_entry, part_dest_width | width_bits);
					sprite_entry += 2u;
					r14 = r14 + 2u;
					source_width = (int8_t)mem_read8(r14) * scale_x;
					r14 = r14 + 1u;
					mem_write16(sprite_entry, shift_signed_by((int8_t)mem_read8(r14) * scale_y, scale_shift, 32) + screen_y);
					sprite_entry += 2u;
					mem_write16(sprite_entry, shift_signed_by(source_width, scale_shift, 32) + screen_x);
					sprite_entry += 2u;
					mem_write16(sprite_entry, mem_read16(frame_data));
					sprite_entry += 2u;
					r14 = r14 + 1u;
					mem_write16(sprite_entry, mem_read16(r14));
					sprite_entry += 2u;
					parts_left--;
					r14 = r14 + 2u;
				} while (parts_left != 0u);
				break;
			case 0x33u:
			case 0x34u:
				part_scale = 0x100u;
				goto build_sprite_list_write_parts;
			default:
				unknown_code(0x9A2E0u + (flags_word & 0x34u));
			}
			current_object = LINKED(current_object, OBJECT(current_object)->draw_next);
		} while ((uint16_t)current_object != (uint16_t)last_object);
		OBJECT(last_object)->draw_next = last_object;
	}
	int_default(&flags_word, &source_width, &ignored, &r4);
	*p_height = r4;
	*p_scale_x = scale_x;
	*p_scale_y = scale_y;
	*p_sprite_flags = (*p_sprite_flags & 0xFFFF0000u) | sprite_flags;
	*p_sprite_entry = sprite_entry;
}

uint32_t load_default_palettes(void)
{
	uint32_t r11 = 0, ignored = 0, palette_list = 0;

	vblank_copies_pending = 0u;
	vblank_copy_count = 0u;
	palette_list = default_palette_list_ADDRESS;
	load_palette_list(palette_list, &ignored, &r11);
	return r11;
}

void load_palette_list(uint32_t palette_list, uint32_t *p_source, uint32_t *p_dest)
{
	uint32_t copy_mode = 0, dest = 0, entries_left = 0, size = 0, source = 0;

	entries_left = mem_read16(palette_list);
	dest = 0x600000u;
	palette_list += 2u;
	do
	{
		source = mem_read32(palette_list);
		palette_list += 4u;
		dest = (dest & ~0xFFFFu) | mem_read16(palette_list);
		palette_list += 2u;
		copy_mode = mem_read16(palette_list);
		palette_list += 2u;
		size = mem_read32(palette_list) - source;
		switch (copy_mode)
		{
		case 0x0u:
			copy_palettes_with_pen_0((uint16_t)copy_mode, size, &source, &dest);
			break;
		case 0x2u:
			copy_palette_block(size, &source, &dest);
			break;
		case 0x4u:
			repeat_palette_block(size, &source, &dest, &palette_list);
			break;
		case 0x6u:
			copy_palettes_with_pen_0((uint16_t)copy_mode, size, &source, &dest);
			break;
		default:
			unknown_code(0x9A4DFu + copy_mode);
		}
		entries_left--;
	} while (entries_left != 0u);
	*p_source = source;
	*p_dest = dest;
}

void copy_palettes_with_pen_0(uint16_t pen_0_color, uint32_t size, uint32_t *p_source, uint32_t *p_dest)
{
	uint32_t source = *p_source;
	uint32_t dest = *p_dest;
	uint16_t pen_15 = 0;
	uint32_t pens_1_2 = 0, pens_3_4 = 0, pens_5_6 = 0, pens_7_8 = 0, pens_9_10 = 0, pens_11_12 = 0, pens_13_14 = 0, halfwords_left = 0;

	size = size >> 1;
	do
	{
		mem_write16(dest, pen_0_color);
		pens_1_2 = mem_read32(source);
		source += 4u;
		dest += 2u;
		mem_write32(dest, pens_1_2);
		pens_3_4 = mem_read32(source);
		source += 4u;
		dest += 4u;
		mem_write32(dest, pens_3_4);
		pens_5_6 = mem_read32(source);
		source += 4u;
		dest += 4u;
		mem_write32(dest, pens_5_6);
		pens_7_8 = mem_read32(source);
		source += 4u;
		dest += 4u;
		mem_write32(dest, pens_7_8);
		pens_9_10 = mem_read32(source);
		source += 4u;
		dest += 4u;
		mem_write32(dest, pens_9_10);
		pens_11_12 = mem_read32(source);
		source += 4u;
		dest += 4u;
		mem_write32(dest, pens_11_12);
		pens_13_14 = mem_read32(source);
		source += 4u;
		dest += 4u;
		mem_write32(dest, pens_13_14);
		pen_15 = mem_read16(source);
		source += 2u;
		dest += 4u;
		mem_write16(dest, pen_15);
		halfwords_left = size;
		size = halfwords_left - 0xFu;
		dest += 2u;
	} while ((int32_t)halfwords_left > 0xF);
	*p_source = source;
	*p_dest = dest;
}

void copy_palette_block(uint32_t size, uint32_t *p_source, uint32_t *p_dest)
{
	uint32_t source = *p_source;
	uint32_t dest = *p_dest;
	uint32_t dest_end = 0, source_end = 0;

	size = size >> 1;
	move_string(source, size, dest, size, 2u, 0u, 0u, fill_value, &dest_end, &source_end);
	source = source_end;
	dest = dest_end;
	*p_source = source;
	*p_dest = dest;
}

void repeat_palette_block(uint32_t size, uint32_t *p_source, uint32_t *p_dest, uint32_t *p_palette_list)
{
	uint32_t source = *p_source;
	uint32_t dest = *p_dest;
	uint32_t palette_list = *p_palette_list;
	uint32_t dest_end = 0, repeat_counts = 0, repeats_left = 0, source_end = 0;

	repeat_counts = source;
	source = mem_read32(palette_list);
	size = size >> 1;
	palette_list += 4u;
	do
	{
		repeats_left = mem_read16(repeat_counts);
		repeat_counts += 2u;
		do
		{
			move_string(source, 0x10u, dest, 0x10u, 2u, 0u, 0u, fill_value, &dest_end, &source_end);
			dest = dest_end;
			repeats_left--;
		} while (repeats_left != 0u);
		source = source_end;
		size--;
	} while (size != 0u);
	*p_source = source;
	*p_dest = dest;
	*p_palette_list = palette_list;
}

void queue_vblank_copy(uint32_t source, uint32_t dest, uint16_t halfwords, uint32_t *p_copy_offset, uint32_t *p_copy_entry)
{
	uint32_t copy_entry = 0, copy_offset = 0;

	vblank_copies_pending = 0u;
	copy_offset = vblank_copy_count;
	vblank_copy_count++;
	copy_offset = copy_offset * 5u;
	copy_entry = vblank_copies_ADDRESS + copy_offset * 2u;
	mem_write32(copy_entry, source);
	copy_entry += 4u;
	mem_write32(copy_entry, dest);
	copy_entry += 4u;
	mem_write16(copy_entry, halfwords);
	vblank_copies_pending = 1u;
	copy_entry += 2u;
	*p_copy_offset = copy_offset;
	*p_copy_entry = copy_entry;
}

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

void play_sound(uint8_t sound_id, uint32_t *p_r1, uint32_t *p_r2)
{
	uint32_t r1 = *p_r1;
	uint32_t r2 = *p_r2;
	uint32_t r1_2 = 0, r2_2 = 0, mask_bit = 0, mask_index = 0;

	if (game_in_progress != 0u)
	{
		queue_sound(sound_id, &r1_2, &r2_2);
		*p_r1 = r1_2;
		*p_r2 = r2_2;
		return;
	}
	if (setting_advertise_sound == 0u)
		return;
	mask_index = (uint8_t)sound_id;
	mask_bit = mask_index & 0x1Fu;
	mask_index = mask_index >> 5;
	if ((attract_sound_mask[mask_index] & shl32(1u, (int)(mask_bit))) == 0u)
	{
		*p_r1 = mask_bit;
		*p_r2 = mask_index;
		return;
	}
	queue_sound(sound_id, &r1_2, &r2_2);
	*p_r1 = r1_2;
	*p_r2 = r2_2;
}

void queue_sound(uint8_t sound_id, uint32_t *p_r1, uint32_t *p_r2)
{
	uint32_t write_index = 0, write_index_address = 0;

	write_index = sound_queue_write;
	sound_queue[write_index] = sound_id;
	write_index = (uint8_t)(write_index + 1u);
	write_index = write_index & 0x1Fu;
	write_index_address = sound_queue_write_ADDRESS;
	sound_queue_write = write_index;
	*p_r1 = write_index;
	*p_r2 = write_index_address;
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

void clear_bitmap_layer(uint8_t is_8bpp, uint32_t *p_top, uint32_t *p_color, uint32_t *p_r5, uint32_t *p_r8, uint32_t *p_blocks_per_row, uint32_t *p_r10)
{
	uint32_t color = *p_color;
	uint32_t r5 = *p_r5;
	uint32_t r8 = *p_r8;
	uint32_t blocks_per_row = *p_blocks_per_row;
	uint32_t r10 = *p_r10;
	uint8_t is_8bpp_2 = 0;
	uint16_t bottom = 0, right = 0, scroll_x = 0, scroll_y = 0, window_top = 0, x = 0, y = 0;
	uint32_t clip_right_bottom = 0, top = 0, video_base = 0;

	is_8bpp_2 = (uint8_t)is_8bpp != 0u;
	scroll_x = 0u;
	scroll_y = 0u;
	set_bitmap_scroll(scroll_x, scroll_y, is_8bpp_2);
	x = 0u;
	y = 0u;
	get_bitmap_address(x, y);
	video_base = video_control;
	mem_write16(video_layer_disable, mem_read16(video_layer_disable) & 0x7BDFu);
	mem_write32(video_bitmap_clip_left, 0u);
	clip_right_bottom = 0xDF013Fu;
	if ((int16_t)mem_read16(video_control) < 0)
		clip_right_bottom = (clip_right_bottom & ~0xFFFFu) | 0x19Fu;
	mem_write32(video_base + 0x84u, clip_right_bottom);
	clip_right_bottom &= ~0xFFFFu;
	window_top = 0u;
	right = 0x1FFu;
	bottom = 0xDFu;
	set_bitmap_window((uint16_t)clip_right_bottom, window_top, right, bottom);
	clip_right_bottom |= (uint16_t)bitmap_window_left;
	top = (uint16_t)bitmap_window_top;
	right = (uint16_t)bitmap_window_right;
	bottom = (uint16_t)bitmap_window_bottom;
	color &= ~0xFFu;
	fill_rect_unclipped(right, bottom, &clip_right_bottom, &top, &color, &r5, &r8, &blocks_per_row, &r10);
	*p_top = top;
	*p_color = color;
	*p_r5 = r5;
	*p_r8 = r8;
	*p_blocks_per_row = blocks_per_row;
	*p_r10 = r10;
}

void get_bitmap_address(uint16_t x, uint16_t y)
{
	uint8_t x_shift = 0;

	x_shift = (uint8_t)(0xFFu + bitmap_8bpp);
	bitmap_address = 0x300000u + (shift_signed_by((uint32_t)(int16_t)x, x_shift, 32) + shift_by((uint32_t)(int16_t)y, (uint8_t)(x_shift + 9u), 32));
}

void set_bitmap_scroll(uint16_t scroll_x, uint16_t scroll_y, uint8_t is_8bpp)
{
	mem_write16(video_bitmap_scroll_x, scroll_x);
	mem_write16(video_bitmap_scroll_y, scroll_y);
	is_8bpp &= 1u;
	bitmap_8bpp = is_8bpp;
	mem_write16(video_control, mem_read16(video_control) & ~0x800u);
	mem_write16(video_control, mem_read16(video_control) | ((uint32_t)is_8bpp << 11));
}

void set_bitmap_window(uint16_t left, uint16_t top, uint16_t right, uint16_t bottom)
{
	uint16_t old_left = 0, old_top = 0;

	if ((int16_t)right < (int16_t)left)
	{
		old_left = left;
		left = (uint16_t)right;
		right = old_left;
	}
	if ((int16_t)bottom < (int16_t)top)
	{
		old_top = top;
		top = (uint16_t)bottom;
		bottom = old_top;
	}
	bitmap_window_left = left;
	bitmap_window_top = top;
	bitmap_window_right = right;
	bitmap_window_bottom = bottom;
}

void draw_line(uint32_t end_x, uint32_t end_y, uint32_t *p_start_x, uint32_t *p_start_y, uint32_t *p_color, uint32_t *p_r8)
{
	uint32_t start_x = *p_start_x;
	uint32_t start_y = *p_start_y;
	uint32_t color = *p_color;
	uint32_t r8 = *p_r8;
	uint32_t zero = 0, negative = 0, overflow = 0, carry = 0;
	uint8_t high_nibble_mask = 0, low_nibble_color = 0;
	uint16_t fraction_4bpp = 0, fraction_8bpp = 0;
	uint32_t direction_index = 0, end_outcode = 0, end_x_copy = 0, end_y_copy = 0, end_y_copy_8bpp = 0, ignored = 0, major_step = 0, minor_step = 0, outcode = 0, row_step = 0, row_step_4bpp = 0, slope = 0, span_end_copy = 0, span_length = 0, steep_slope = 0, step_record = 0, steps_left = 0, top_row = 0;
	uint16_t end_y_4bpp = 0, dy_4bpp = 0, end_x_4bpp = 0, dx_4bpp = 0, end_y_8bpp = 0, dy_8bpp = 0, span_end_x = 0, end_x_8bpp = 0, dx_8bpp = 0;
	uint32_t pixels_left = 0, saved_step = 0, saved_dy = 0;

	for (;;)
	{
		get_window_outcode(&start_x, &start_y, &end_x, &end_y, &outcode);
		end_outcode = outcode;
		get_window_outcode(&start_x, &start_y, &end_x, &end_y, &outcode);
		if ((uint16_t)(outcode & end_outcode) != 0u)
		{
			start_x = 0xFFFFFFFFu;
			*p_start_x = start_x;
			*p_start_y = start_y;
			*p_r8 = r8;
			return;
		}
		if ((uint16_t)(outcode | end_outcode) == 0u)
			break;
		clip_line_to_window(outcode, end_outcode, &start_x, &start_y, &end_x, &end_y, &r8);
	}
	if (bitmap_8bpp == 0u)
	{
		row_step_4bpp = 0x100u;
		end_y_copy = (uint16_t)end_y;
		end_y_4bpp = end_y_copy;
		dy_4bpp = end_y_4bpp - start_y;
		overflow = (uint32_t)(int16_t)end_y_4bpp - (int16_t)start_y != (uint32_t)(int16_t)(end_y_4bpp - start_y);
		zero = dy_4bpp == 0u;
		negative = dy_4bpp >> 15;
		slope = dy_4bpp;
		if (end_y_4bpp == (uint16_t)start_y)
		{
			fill_span_4bpp((uint16_t)end_x, &start_x, &start_y, &color, &ignored);
			*p_start_x = start_x;
			*p_start_y = start_y;
			*p_color = color;
			*p_r8 = r8;
			return;
		}
		if ((negative ^ overflow) | zero)
		{
			row_step_4bpp = 0u - row_step_4bpp;
			slope = (uint16_t)(0u - slope);
		}
		end_x_copy = (uint16_t)end_x;
		end_x_4bpp = end_x_copy;
		dx_4bpp = end_x_4bpp - start_x;
		overflow = (uint32_t)(int16_t)end_x_4bpp - (int16_t)start_x != (uint32_t)(int16_t)(end_x_4bpp - start_x);
		zero = dx_4bpp == 0u;
		negative = dx_4bpp >> 15;
		steep_slope = dx_4bpp;
		if (end_x_4bpp == (uint16_t)start_x)
		{
			draw_vertical_span_4bpp((uint16_t)start_x, row_step_4bpp, &start_y, &color, &slope);
			*p_start_x = start_x;
			*p_start_y = start_y;
			*p_color = color;
			*p_r8 = r8;
			return;
		}
		if ((negative ^ overflow) | zero)
		{
			steep_slope = (uint16_t)(0u - steep_slope);
			start_x = (start_x & ~0xFFFFu) | (uint16_t)end_x;
			start_y = (uint16_t)end_y;
			row_step_4bpp = 0u - row_step_4bpp;
		}
		start_y = ((uint32_t)(uint16_t)start_y << 8) + ((uint16_t)start_x >> 1) + bitmap_address;
		color = (color & ~0xFFu) | (color & 0xFu);
		low_nibble_color = (uint8_t)color;
		color = (color & ~0xFFu) | (uint8_t)(color << 4);
		high_nibble_mask = 0xF0u;
		slope = (uint16_t)(slope + 1u);
		steep_slope = (uint16_t)(steep_slope + 1u);
		if (steep_slope < slope)
		{
			steep_slope = divide_u32(steep_slope << 16, slope);
			fraction_4bpp = 0u;
			if ((start_x & 1u) != 0u)
			{
				do
				{
				draw_line_steep_odd_pixel:
					start_x = (start_x & ~0xFFu) | mem_read8(start_y);
					start_x = (start_x & ~0xFFu) | (start_x & 0xFu);
					start_x = (start_x & ~0xFFu) | (uint8_t)(start_x | color);
					mem_write8(start_y, start_x);
					start_y += row_step_4bpp;
					carry = (uint16_t)(fraction_4bpp + steep_slope) < fraction_4bpp;
					fraction_4bpp += steep_slope;
					slope--;
				} while ((slope != 0u) & (carry == 0u));
				start_y++;
				if (slope == 0u)
				{
					*p_start_x = start_x;
					*p_start_y = start_y;
					*p_color = color;
					*p_r8 = r8;
					return;
				}
			}
			do
			{
				start_x = (start_x & ~0xFFu) | mem_read8(start_y);
				start_x = (start_x & ~0xFFu) | (start_x & high_nibble_mask);
				start_x = (start_x & ~0xFFu) | (uint8_t)(start_x | low_nibble_color);
				mem_write8(start_y, start_x);
				start_y += row_step_4bpp;
				carry = (uint16_t)(fraction_4bpp + steep_slope) < fraction_4bpp;
				fraction_4bpp += steep_slope;
				slope--;
			} while ((slope != 0u) & (carry == 0u));
			if (slope == 0u)
			{
				*p_start_x = start_x;
				*p_start_y = start_y;
				*p_color = color;
				*p_r8 = r8;
				return;
			}
			goto draw_line_steep_odd_pixel;
		}
		if (steep_slope == slope)
		{
			if ((start_x & 1u) != 0u)
			{
			draw_line_diagonal_odd_pixel:
				start_x = (start_x & ~0xFFu) | mem_read8(start_y);
				start_x = (start_x & ~0xFFu) | (start_x & 0xFu);
				start_x = (start_x & ~0xFFu) | (uint8_t)(start_x | color);
				mem_write8(start_y, start_x);
				start_y = start_y + 1u + row_step_4bpp;
				steep_slope--;
				if (steep_slope == 0u)
				{
					*p_start_x = start_x;
					*p_start_y = start_y;
					*p_color = color;
					*p_r8 = r8;
					return;
				}
			}
			start_x = (start_x & ~0xFFu) | mem_read8(start_y);
			start_x = (start_x & ~0xFFu) | (start_x & high_nibble_mask);
			start_x = (start_x & ~0xFFu) | (uint8_t)(start_x | low_nibble_color);
			mem_write8(start_y, start_x);
			start_y += row_step_4bpp;
			pixels_left = steep_slope;
			steep_slope = pixels_left - 1u;
			if (pixels_left == 1u)
			{
				*p_start_x = start_x;
				*p_start_y = start_y;
				*p_color = color;
				*p_r8 = r8;
				return;
			}
			goto draw_line_diagonal_odd_pixel;
		}
		slope = divide_u32(slope << 16, steep_slope);
		fraction_4bpp = 0u;
		if ((start_x & 1u) != 0u)
		{
		draw_line_shallow_odd_pixel:
			start_x = (start_x & ~0xFFu) | mem_read8(start_y);
		draw_line_shallow_set_high_nibble:
			start_x = (start_x & ~0xFFu) | (start_x & 0xFu);
			start_x = (start_x & ~0xFFu) | (uint8_t)(start_x | color);
			mem_write8(start_y, start_x);
			carry = (uint16_t)(fraction_4bpp + slope) < fraction_4bpp;
			fraction_4bpp += slope;
			steep_slope--;
			start_y++;
			if ((steep_slope != 0u) & (carry == 0u))
			{
			draw_line_shallow_even_pixel:
				start_x = (start_x & ~0xFFu) | mem_read8(start_y);
				start_x = (start_x & ~0xFFu) | (start_x & high_nibble_mask);
				start_x = (start_x & ~0xFFu) | (uint8_t)(start_x | low_nibble_color);
				carry = (uint16_t)(fraction_4bpp + slope) < fraction_4bpp;
				fraction_4bpp += slope;
				steep_slope--;
				if ((steep_slope != 0u) & (carry == 0u))
					goto draw_line_shallow_set_high_nibble;
				mem_write8(start_y, start_x);
				if (steep_slope == 0u)
				{
					*p_start_x = start_x;
					*p_start_y = start_y;
					*p_color = color;
					*p_r8 = r8;
					return;
				}
				start_y += row_step_4bpp;
				goto draw_line_shallow_odd_pixel;
			}
			start_y += row_step_4bpp;
			if (steep_slope == 0u)
			{
				*p_start_x = start_x;
				*p_start_y = start_y;
				*p_color = color;
				*p_r8 = r8;
				return;
			}
			goto draw_line_shallow_even_pixel;
		}
		goto draw_line_shallow_even_pixel;
	}
	direction_index = 0u;
	end_y_copy_8bpp = end_y;
	end_y_8bpp = end_y_copy_8bpp;
	dy_8bpp = end_y_8bpp - start_y;
	overflow = (uint32_t)(int16_t)end_y_8bpp - (int16_t)start_y != (uint32_t)(int16_t)(end_y_8bpp - start_y);
	zero = dy_8bpp == 0u;
	negative = dy_8bpp >> 15;
	slope = (end_y_copy_8bpp & ~0xFFFFu) | dy_8bpp;
	if (end_y_8bpp == (uint16_t)start_y)
	{
		start_y = ((uint32_t)(uint16_t)start_y << 9) + start_x + bitmap_address;
		span_end_copy = (uint16_t)end_x;
		span_end_x = span_end_copy;
		span_length = (uint16_t)(span_end_x - start_x);
		if ((int16_t)start_x <= (int16_t)span_end_x)
		{
			fill_span_8bpp(span_length, &start_x, &start_y, &color);
			*p_start_x = start_x;
			*p_start_y = start_y;
			*p_color = color;
			*p_r8 = r8;
			return;
		}
		span_length = (uint16_t)(0u - span_length);
		start_y -= span_length;
		fill_span_8bpp(span_length, &start_x, &start_y, &color);
		*p_start_x = start_x;
		*p_start_y = start_y;
		*p_color = color;
		*p_r8 = r8;
		return;
	}
	if ((negative ^ overflow) | zero)
	{
		slope = (slope & ~0xFFFFu) | (uint16_t)(0u - slope);
		direction_index = (uint8_t)(direction_index + 1u);
	}
	steps_left = end_x;
	end_x_8bpp = steps_left;
	dx_8bpp = end_x_8bpp - start_x;
	overflow = (uint32_t)(int16_t)end_x_8bpp - (int16_t)start_x != (uint32_t)(int16_t)(end_x_8bpp - start_x);
	zero = dx_8bpp == 0u;
	negative = dx_8bpp >> 15;
	steps_left = (steps_left & ~0xFFFFu) | dx_8bpp;
	if (end_x_8bpp == (uint16_t)start_x)
	{
		if (direction_index == 0u)
		{
			start_y = (uint32_t)(uint16_t)start_y << 9;
			start_y = ((start_y & ~0xFFFFu) | (uint16_t)(start_y + start_x)) + bitmap_address;
			row_step = 0x200u;
			draw_vertical_span_8bpp(row_step, (uint8_t)color, &start_y, &slope);
			*p_start_x = start_x;
			*p_start_y = start_y;
			*p_color = color;
			*p_r8 = r8;
			return;
		}
		top_row = end_y;
		top_row = (uint32_t)(uint16_t)top_row << 9;
		top_row = ((top_row & ~0xFFFFu) | (uint16_t)(top_row + start_x)) + bitmap_address;
		row_step = 0x200u;
		draw_vertical_span_8bpp(row_step, (uint8_t)color, &top_row, &slope);
		*p_start_x = start_x;
		*p_start_y = top_row;
		*p_color = color;
		*p_r8 = r8;
		return;
	}
	if ((negative ^ overflow) | zero)
	{
		steps_left = (steps_left & ~0xFFFFu) | (uint16_t)(0u - steps_left);
		direction_index = (uint8_t)(direction_index + 2u);
	}
	start_y = ((uint32_t)(int16_t)start_y << 9) + start_x + bitmap_address;
	step_record = 0x9C4D2u + direction_index * 4u;
	minor_step = (int16_t)ROM16(step_record);
	major_step = (int16_t)ROM16(step_record + 2u);
	if ((uint16_t)steps_left == (uint16_t)slope)
	{
		minor_step += major_step;
		slope = (slope & ~0xFFFFu) | (uint16_t)(slope + 1u);
		carry = slope & 1u;
		slope = slope >> 1;
		if (carry)
			slope = (slope & ~0xFFFFu) | (uint16_t)(slope + 1u);
		do
		{
			mem_write8(start_y, color);
			start_y += minor_step;
			mem_write8(start_y, color);
			start_y += minor_step;
			slope--;
		} while (slope != 0u);
		start_x = (uint16_t)slope;
		*p_start_x = start_x;
		*p_start_y = start_y;
		*p_r8 = r8;
		return;
	}
	if ((uint16_t)steps_left < (uint16_t)slope)
	{
		saved_step = major_step;
		major_step = minor_step;
		minor_step = saved_step;
		saved_dy = slope;
		slope = steps_left;
		steps_left = saved_dy;
	}
	steps_left = (steps_left & ~0xFFFFu) | (uint16_t)(steps_left + 1u);
	slope = divide_s32((uint32_t)(uint16_t)(slope + 1u) << 16, steps_left);
	fraction_8bpp = 0u;
	for (;;)
	{
		mem_write8(start_y, color);
		start_y += major_step;
		carry = (uint16_t)(fraction_8bpp + slope) < fraction_8bpp;
		fraction_8bpp += slope;
		steps_left--;
		if ((steps_left != 0u) & (carry == 0u))
			continue;
		start_y += minor_step;
		if (steps_left != 0u)
			continue;
		break;
	}
	start_x = (uint16_t)steps_left;
	*p_start_x = start_x;
	*p_start_y = start_y;
	*p_r8 = r8;
}

void fill_span_8bpp(uint32_t length, uint32_t *p_blocks, uint32_t *p_pixel_address, uint32_t *p_color)
{
	uint32_t blocks = *p_blocks;
	uint32_t pixel_address = *p_pixel_address;
	uint32_t color = *p_color;
	uint32_t zero = 0;
	uint8_t tail_count = 0;
	uint32_t length_left = 0, block_count = 0, tail_pixels = 0;

	if ((pixel_address & 1u) != 0u)
	{
		mem_write8(pixel_address, color);
		length_left = length - 1u;
		length = length_left;
		pixel_address++;
		if ((int32_t)length_left < 0)
		{
			pixel_address = (pixel_address & ~0xFFFFu) | (uint16_t)(pixel_address - 1u);
			*p_pixel_address = pixel_address;
			return;
		}
	}
	if (length != 0u)
	{
		color = (uint8_t)color * 0x1010101u;
		length++;
		block_count = length >> 4;
		blocks = block_count;
		if (block_count != 0u)
		{
			do
			{
				mem_write32(pixel_address, color);
				pixel_address += 4u;
				mem_write32(pixel_address, color);
				pixel_address += 4u;
				mem_write32(pixel_address, color);
				pixel_address += 4u;
				mem_write32(pixel_address, color);
				blocks--;
				pixel_address += 4u;
			} while (blocks != 0u);
		}
		length &= 0xFu;
		if (length >= 8u)
		{
			mem_write32(pixel_address, color);
			pixel_address += 4u;
			mem_write32(pixel_address, color);
			length -= 8u;
			pixel_address += 4u;
		}
		if (length >= 4u)
		{
			mem_write32(pixel_address, color);
			pixel_address += 4u;
		}
		tail_pixels = length & 3u;
		tail_count = (uint8_t)tail_pixels;
		if (tail_pixels == 0u)
		{
			pixel_address = (pixel_address & ~0xFFFFu) | (uint16_t)(pixel_address - 1u);
			*p_blocks = blocks;
			*p_pixel_address = pixel_address;
			*p_color = color;
			return;
		}
		zero = (uint8_t)(tail_count - 2u) == 0u;
		if (tail_count >= 2u)
		{
			mem_write16(pixel_address, color);
			pixel_address += 2u;
			if (zero)
			{
				pixel_address = (pixel_address & ~0xFFFFu) | (uint16_t)(pixel_address - 1u);
				*p_blocks = blocks;
				*p_pixel_address = pixel_address;
				*p_color = color;
				return;
			}
		}
	}
	mem_write8(pixel_address, color);
	pixel_address++;
	pixel_address = (pixel_address & ~0xFFFFu) | (uint16_t)(pixel_address - 1u);
	*p_blocks = blocks;
	*p_pixel_address = pixel_address;
	*p_color = color;
}

void draw_vertical_span_8bpp(uint32_t row_step, uint8_t color, uint32_t *p_pixel_address, uint32_t *p_length)
{
	uint32_t pixel_address = *p_pixel_address;
	uint32_t length = *p_length;
	uint32_t carry = 0;

	length = (length & ~0xFFFFu) | (uint16_t)(length + 1u);
	carry = length & 1u;
	length = (length & ~0xFFFFu) | ((uint16_t)length >> 1);
	if (carry == 0u)
	{
		mem_write8(pixel_address, color);
		pixel_address += row_step;
	}
	else
		length = (length & ~0xFFFFu) | (uint16_t)(length + 1u);
	for (;;)
	{
		mem_write8(pixel_address, color);
		pixel_address += row_step;
		length--;
		if (length == 0u)
			break;
		mem_write8(pixel_address, color);
		pixel_address += row_step;
	}
	pixel_address -= row_step;
	*p_pixel_address = pixel_address;
	*p_length = length;
}

void fill_span_4bpp(uint16_t end_x, uint32_t *p_start_x, uint32_t *p_start_y, uint32_t *p_color, uint32_t *p_length)
{
	uint32_t start_x = *p_start_x;
	uint32_t start_y = *p_start_y;
	uint32_t color = *p_color;
	uint32_t carry = 0;
	uint32_t end_x_copy = 0, keep_mask = 0, length = 0, longs_left = 0;
	uint16_t end_column = 0;
	uint32_t length_left = 0, tail_pixels = 0, pixels_before_byte = 0, pixels_before_second = 0;

	end_x_copy = (uint16_t)end_x;
	end_column = end_x_copy;
	length = (uint16_t)(end_column - start_x);
	if ((int16_t)end_column < (int16_t)start_x)
	{
		start_x = (start_x & ~0xFFFFu) | (uint16_t)end_x;
		length = (uint16_t)(0u - length);
	}
	color = (color & ~0xFFu) | (color & 0xFu);
	start_y = ((uint32_t)(uint16_t)start_y << 8) + bitmap_address;
	carry = start_x & 1u;
	start_x = (start_x & ~0xFFFFu) | ((uint16_t)start_x >> 1);
	if (carry)
	{
		start_y = (start_y & ~0xFFFFu) | (uint16_t)(start_y + start_x);
		start_x = (start_x & ~0xFFu) | mem_read8(start_y);
		start_x = (start_x & ~0xFFu) | (start_x & 0xFu);
		start_x = (start_x & ~0xFFu) | (uint8_t)(start_x | (color << 4));
		mem_write8(start_y, start_x);
		length_left = length - 1u;
		length = length_left;
		start_y++;
		if ((int32_t)length_left < 0)
		{
			*p_start_x = start_x;
			*p_start_y = start_y;
			*p_color = color;
			*p_length = length;
			return;
		}
		start_x = 0u;
	}
	start_y = (start_y & ~0xFFFFu) | (uint16_t)(start_y + start_x);
	length++;
	if ((start_y & 1u) == 0u)
	{
		color = color * 0x11111111u;
		if ((uint16_t)length >= 8u)
		{
			longs_left = length >> 3;
			do
			{
				mem_write32(start_y, color);
				start_y += 4u;
				longs_left--;
			} while (longs_left != 0u);
			tail_pixels = length & 7u;
			length = tail_pixels;
			if (tail_pixels == 0u)
			{
				*p_start_x = start_x;
				*p_start_y = start_y;
				*p_color = color;
				*p_length = length;
				return;
			}
		}
		length = (length & ~0xFFu) | (uint8_t)(length * 2u);
		length = (length & ~0xFFu) | (uint8_t)(length * 2u);
		keep_mask = shift_by(0xFFFFFFFFu, (uint8_t)length, 32);
		start_x = ~keep_mask;
		color = color & start_x & start_x;
		mem_write32(start_y, (keep_mask & mem_read32(start_y)) | color);
		*p_start_x = start_x;
		*p_start_y = start_y;
		*p_color = color;
		*p_length = length;
		return;
	}
	pixels_before_byte = length;
	length = pixels_before_byte - 1u;
	if (pixels_before_byte != 1u)
	{
		start_x = (start_x & ~0xFFu) | (uint8_t)color;
		start_x = (start_x & ~0xFFu) | (uint8_t)(start_x << 4);
		start_x = (start_x & ~0xFFu) | (uint8_t)(start_x | color);
		mem_write8(start_y, start_x);
		pixels_before_second = length;
		length = pixels_before_second - 1u;
		start_y++;
		if (pixels_before_second == 1u)
		{
			*p_start_x = start_x;
			*p_start_y = start_y;
			*p_color = color;
			*p_length = length;
			return;
		}
		color = color * 0x11111111u;
		if ((uint16_t)length >= 8u)
		{
			longs_left = length >> 3;
			do
			{
				mem_write32(start_y, color);
				start_y += 4u;
				longs_left--;
			} while (longs_left != 0u);
			tail_pixels = length & 7u;
			length = tail_pixels;
			if (tail_pixels == 0u)
			{
				*p_start_x = start_x;
				*p_start_y = start_y;
				*p_color = color;
				*p_length = length;
				return;
			}
		}
		length = (length & ~0xFFu) | (uint8_t)(length * 2u);
		length = (length & ~0xFFu) | (uint8_t)(length * 2u);
		keep_mask = shift_by(0xFFFFFFFFu, (uint8_t)length, 32);
		start_x = ~keep_mask;
		color = color & start_x & start_x;
		mem_write32(start_y, (keep_mask & mem_read32(start_y)) | color);
		*p_start_x = start_x;
		*p_start_y = start_y;
		*p_color = color;
		*p_length = length;
		return;
	}
	start_x = (start_x & ~0xFFu) | 0xF0u;
	start_x = (start_x & ~0xFFu) | (start_x & mem_read8(start_y));
	start_x = (start_x & ~0xFFu) | (uint8_t)(start_x | color);
	mem_write8(start_y, start_x);
	*p_start_x = start_x;
	*p_start_y = start_y;
	*p_color = color;
	*p_length = length;
}

void draw_vertical_span_4bpp(uint16_t start_x, uint32_t row_step, uint32_t *p_start_y, uint32_t *p_color, uint32_t *p_length)
{
	uint32_t start_y = *p_start_y;
	uint8_t color = (uint8_t)*p_color;
	uint32_t length = *p_length;
	uint32_t carry = 0;
	uint8_t keep_mask = 0;

	start_y = (uint32_t)(uint16_t)start_y << 8;
	start_y = ((start_y & ~0xFFFFu) | (uint16_t)(start_y + ((uint16_t)start_x >> 1))) + bitmap_address;
	length = (length & ~0xFFFFu) | (uint16_t)(length + 1u);
	keep_mask = 0xF0u;
	color &= 0xFu;
	if ((start_x & 1u) != 0u)
	{
		color = (uint8_t)((uint32_t)color << 4);
		keep_mask = 0xFu;
	}
	carry = length & 1u;
	length = length >> 1;
	if (carry == 0u)
	{
		mem_write8(start_y, (mem_read8(start_y) & keep_mask) | color);
		start_y += row_step;
	}
	else
		length++;
	for (;;)
	{
		mem_write8(start_y, (mem_read8(start_y) & keep_mask) | color);
		start_y += row_step;
		length--;
		if (length == 0u)
			break;
		mem_write8(start_y, (mem_read8(start_y) & keep_mask) | color);
		start_y += row_step;
	}
	*p_start_y = start_y;
	*p_color = (*p_color & 0xFFFFFF00u) | color;
	*p_length = length;
}

void fill_rect(uint32_t top, uint32_t right, uint32_t bottom, uint32_t *p_left, uint32_t *p_color, uint32_t *p_blocks_per_row)
{
	uint32_t left = *p_left;
	uint32_t color = *p_color;
	uint32_t blocks_per_row = *p_blocks_per_row;
	uint32_t inner_count = 0, row_pairs_left = 0, unclipped_edges = 0;

	clip_rect_to_window(&left, &top, &right, &bottom, &unclipped_edges);
	if (unclipped_edges == 0u)
	{
		left = 0xFFFFFFFFu;
		*p_left = left;
		return;
	}
	fill_rect_unclipped(right, bottom, &left, &top, &color, &inner_count, &row_pairs_left, &blocks_per_row, &unclipped_edges);
	*p_left = left;
	*p_color = color;
	*p_blocks_per_row = blocks_per_row;
}

void fill_rect_unclipped(uint32_t right, uint32_t bottom, uint32_t *p_left, uint32_t *p_top, uint32_t *p_color, uint32_t *p_inner_count, uint32_t *p_row_pairs_left, uint32_t *p_blocks_per_row, uint32_t *p_r10)
{
	uint32_t left = *p_left;
	uint32_t top = *p_top;
	uint32_t color = *p_color;
	uint32_t inner_count = *p_inner_count;
	uint32_t row_pairs_left = *p_row_pairs_left;
	uint32_t blocks_per_row = *p_blocks_per_row;
	uint32_t r10 = *p_r10;
	uint32_t carry = 0;
	uint8_t byte_color = 0, color_low = 0, high_nibble_color = 0, tail_width = 0;
	uint16_t straddle_keep_mask = 0, tail_keep_mask = 0;
	uint32_t r5 = 0, block_cursor = 0, bottom_copy = 0, column_cursor = 0, first_row = 0, last_column = 0, length = 0, lower_cursor = 0, pair_skip = 0, pixel_columns = 0, right_copy = 0, row_bytes = 0, row_offset_halfwords = 0, row_offset_longs = 0, row_pitch_4bpp = 0, row_pitch_8bpp = 0, row_step = 0, row_step_2 = 0, rows_left = 0, rows_to_fill = 0, upper_cursor = 0;
	uint16_t block_count = 0, bottom_row = 0, right_column = 0, rect_dx = 0;
	uint32_t columns_left = 0, tail_columns = 0, tail_pixels = 0;

	if (bitmap_8bpp != 0u)
	{
		bottom = (uint16_t)bottom;
		top = (uint16_t)top;
		bottom = (uint16_t)(bottom - top);
		top = top << 9;
		top = ((top & ~0xFFFFu) | (uint16_t)(top + left)) + bitmap_address;
		color = (uint8_t)color * 0x1010101u;
		right = (uint16_t)right;
		right = (uint16_t)(right - left);
		bottom = (uint16_t)(bottom + 1u);
		row_pitch_8bpp = 0x200u;
		carry = bottom & 1u;
		bottom = bottom >> 1;
		if (carry)
		{
			first_row = top;
			row_pairs_left = bottom;
			length = right;
			fill_span_8bpp(length, &left, &top, &color);
			if (row_pairs_left == 0u)
			{
				*p_left = left;
				*p_top = top;
				*p_color = color;
				*p_row_pairs_left = row_pairs_left;
				return;
			}
			top = first_row + row_pitch_8bpp;
			bottom = row_pairs_left;
		}
		if ((top & 1u) != 0u)
		{
			left = top;
			inner_count = bottom;
			top++;
			do
			{
				mem_write8(left, color);
				mem_write8(left + row_pitch_8bpp, color);
				left += row_pitch_8bpp * 2u;
				inner_count--;
			} while (inner_count != 0u);
			columns_left = right - 1u;
			right = columns_left;
			if ((int32_t)columns_left < 0)
			{
				*p_left = left;
				*p_top = top;
				*p_color = color;
				*p_inner_count = inner_count;
				*p_row_pairs_left = row_pairs_left;
				return;
			}
		}
		if (right != 0u)
		{
			right = (right & ~0xFFFFu) | (uint16_t)(right + 1u);
			row_bytes = right;
			block_count = (uint16_t)row_bytes >> 4;
			blocks_per_row = (row_bytes & ~0xFFFFu) | block_count;
			if (block_count != 0u)
			{
				left = top;
				row_pairs_left = bottom;
				top += 0xFFFFFFF0u & right;
				do
				{
					inner_count = blocks_per_row;
					upper_cursor = left;
					r10 = left + row_pitch_8bpp;
					do
					{
						mem_write32(upper_cursor, color);
						upper_cursor += 4u;
						mem_write32(upper_cursor, color);
						upper_cursor += 4u;
						mem_write32(upper_cursor, color);
						upper_cursor += 4u;
						mem_write32(upper_cursor, color);
						mem_write32(r10, color);
						r10 += 4u;
						mem_write32(r10, color);
						r10 += 4u;
						mem_write32(r10, color);
						r10 += 4u;
						mem_write32(r10, color);
						inner_count--;
						upper_cursor += 4u;
						r10 += 4u;
					} while (inner_count != 0u);
					left += row_pitch_8bpp * 2u;
					row_pairs_left--;
				} while (row_pairs_left != 0u);
			}
			right &= 0xFu;
			if (right >= 8u)
			{
				left = top;
				inner_count = bottom;
				lower_cursor = left + row_pitch_8bpp;
				pair_skip = 0x3F8u;
				top += 8u;
				do
				{
					mem_write32(left, color);
					left += 4u;
					mem_write32(left, color);
					mem_write32(lower_cursor, color);
					lower_cursor += 4u;
					mem_write32(lower_cursor, color);
					left = left + 4u + pair_skip;
					lower_cursor = lower_cursor + 4u + pair_skip;
					inner_count--;
				} while (inner_count != 0u);
				row_pitch_8bpp = 0x200u;
				right -= 8u;
			}
			if (right >= 4u)
			{
				left = top;
				inner_count = bottom;
				row_offset_longs = 0x80u;
				top += 4u;
				do
				{
					mem_write32(left, color);
					mem_write32(left + row_offset_longs * 4u, color);
					left += row_pitch_8bpp * 2u;
					inner_count--;
				} while (inner_count != 0u);
			}
			tail_columns = right & 3u;
			tail_width = (uint8_t)tail_columns;
			if (tail_columns == 0u)
			{
				*p_left = left;
				*p_top = top;
				*p_color = color;
				*p_inner_count = inner_count;
				*p_row_pairs_left = row_pairs_left;
				*p_blocks_per_row = blocks_per_row;
				*p_r10 = r10;
				return;
			}
			if (tail_width >= 2u)
			{
				inner_count = bottom;
				left = top;
				row_offset_halfwords = 0x100u;
				top += 2u;
				do
				{
					mem_write16(left, color);
					mem_write16(left + row_offset_halfwords * 2u, color);
					left += row_pitch_8bpp * 2u;
					inner_count--;
				} while (inner_count != 0u);
				if ((tail_width & 1u) == 0u)
				{
					*p_left = left;
					*p_top = top;
					*p_color = color;
					*p_inner_count = inner_count;
					*p_row_pairs_left = row_pairs_left;
					*p_blocks_per_row = blocks_per_row;
					*p_r10 = r10;
					return;
				}
			}
		}
		do
		{
			mem_write8(top, color);
			mem_write8(top + row_pitch_8bpp, color);
			top += row_pitch_8bpp * 2u;
			bottom--;
		} while (bottom != 0u);
		*p_left = left;
		*p_top = top;
		*p_color = color;
		*p_inner_count = inner_count;
		*p_row_pairs_left = row_pairs_left;
		*p_blocks_per_row = blocks_per_row;
		*p_r10 = r10;
		return;
	}
	color = (color & ~0xFFu) | (color & 0xFu);
	bottom_copy = (uint16_t)bottom;
	top = (uint16_t)top;
	bottom_row = bottom_copy;
	rows_left = (uint16_t)(bottom_row - top);
	if (bottom_row == top)
	{
		fill_span_4bpp((uint16_t)right, &left, &top, &color, &r5);
		*p_left = left;
		*p_top = top;
		*p_color = color;
		*p_inner_count = r5;
		*p_row_pairs_left = row_pairs_left;
		*p_blocks_per_row = blocks_per_row;
		*p_r10 = r10;
		return;
	}
	right_copy = (uint16_t)right;
	right_column = right_copy;
	last_column = (uint16_t)(right_column - left);
	if (right_column == (uint16_t)left)
	{
		row_step_2 = 0x100u;
		draw_vertical_span_4bpp((uint16_t)left, row_step_2, &top, &color, &rows_left);
		*p_left = left;
		*p_top = top;
		*p_color = color;
		*p_inner_count = rows_left;
		*p_row_pairs_left = row_pairs_left;
		*p_blocks_per_row = blocks_per_row;
		*p_r10 = row_step_2;
		return;
	}
	top = top << 8;
	top = ((top & ~0xFFFFu) | (uint16_t)(top + ((uint16_t)left >> 1))) + bitmap_address;
	row_pitch_4bpp = 0x100u;
	rows_left = (uint16_t)(rows_left + 1u);
	rect_dx = last_column;
	pixel_columns = (uint16_t)(rect_dx - 1u);
	if (rect_dx == 1u)
	{
		if ((left & 1u) == 0u)
		{
			color_low = (uint8_t)color;
			color = (color & ~0xFFu) | (uint8_t)(color << 4);
			color = (color & ~0xFFu) | (uint8_t)(color | color_low);
			row_step = row_pitch_4bpp;
			draw_vertical_span_8bpp(row_step, (uint8_t)color, &top, &rows_left);
			*p_left = left;
			*p_top = top;
			*p_color = color;
			*p_inner_count = rows_left;
			*p_row_pairs_left = row_pitch_4bpp;
			*p_blocks_per_row = blocks_per_row;
			*p_r10 = r10;
			return;
		}
		color = (color & ~0xFFFFu) | (uint16_t)(color * 0x110u);
		straddle_keep_mask = 0xF00Fu;
		do
		{
			left = (left & ~0xFFFFu) | mem_read16(top);
			left = (left & ~0xFFFFu) | (left & straddle_keep_mask);
			left = (left & ~0xFFFFu) | (uint16_t)(left | color);
			mem_write16(top, left);
			top += row_pitch_4bpp;
			rows_left--;
		} while (rows_left != 0u);
		*p_left = left;
		*p_top = top;
		*p_color = color;
		*p_inner_count = rows_left;
		*p_row_pairs_left = row_pitch_4bpp;
		return;
	}
	pixel_columns = (uint16_t)(pixel_columns + 2u);
	if ((left & 1u) != 0u)
	{
		high_nibble_color = (uint8_t)(color << 4);
		rows_to_fill = rows_left;
		column_cursor = top;
		top++;
		do
		{
			left = (left & ~0xFFu) | mem_read8(column_cursor);
			left = (left & ~0xFFu) | (left & 0xFu);
			left = (left & ~0xFFu) | (uint8_t)(left | high_nibble_color);
			mem_write8(column_cursor, left);
			column_cursor += row_pitch_4bpp;
			rows_to_fill--;
		} while (rows_to_fill != 0u);
		pixel_columns--;
	}
	if ((top & 1u) != 0u)
	{
		byte_color = (uint8_t)color;
		color = (color & ~0xFFu) | (uint8_t)(color << 4);
		byte_color |= color;
		column_cursor = top;
		rows_to_fill = rows_left;
		top++;
		do
		{
			mem_write8(column_cursor, byte_color);
			column_cursor += row_pitch_4bpp;
			rows_to_fill--;
		} while (rows_to_fill != 0u);
		pixel_columns -= 2u;
	}
	if (pixel_columns >= 4u)
	{
		color = (color & ~0xFFFFu) | (uint16_t)(color * 0x1111u);
		rows_to_fill = rows_left;
		block_cursor = (uint16_t)(rows_left << 8) + top;
		blocks_per_row = pixel_columns >> 2;
		do
		{
			r10 = blocks_per_row;
			block_cursor -= row_pitch_4bpp;
			do
			{
				mem_write16(block_cursor, color);
				block_cursor += 2u;
				r10--;
			} while (r10 != 0u);
			rows_to_fill--;
		} while (rows_to_fill != 0u);
		top = block_cursor;
		tail_pixels = pixel_columns & 3u;
		pixel_columns = tail_pixels;
		if (tail_pixels == 0u)
		{
			*p_left = left;
			*p_top = top;
			*p_color = color;
			*p_inner_count = rows_left;
			*p_row_pairs_left = row_pitch_4bpp;
			*p_blocks_per_row = blocks_per_row;
			*p_r10 = r10;
			return;
		}
	}
	color = (color & ~0xFFFFu) | (color & 0xFu);
	color = (color & ~0xFFFFu) | (uint16_t)(color * 0x1111u);
	tail_keep_mask = shift_by(0xFFFFu, (uint8_t)(pixel_columns * 2u * 2u), 16);
	left = (left & ~0xFFFFu) | (uint16_t)~tail_keep_mask;
	color = (color & ~0xFFFFu) | (uint16_t)(color & left);
	do
	{
		left = (left & ~0xFFFFu) | mem_read16(top);
		left = (left & ~0xFFFFu) | (left & tail_keep_mask);
		left = (left & ~0xFFFFu) | (uint16_t)(left | color);
		mem_write16(top, left);
		top += row_pitch_4bpp;
		rows_left--;
	} while (rows_left != 0u);
	*p_left = left;
	*p_top = top;
	*p_color = color;
	*p_inner_count = rows_left;
	*p_row_pairs_left = row_pitch_4bpp;
	*p_blocks_per_row = blocks_per_row;
	*p_r10 = r10;
}

void get_window_outcode(uint32_t *p_start_x, uint32_t *p_start_y, uint32_t *p_end_x, uint32_t *p_end_y, uint32_t *p_outcode)
{
	uint32_t start_x = *p_start_x;
	uint32_t start_y = *p_start_y;
	uint16_t end_x = (uint16_t)*p_end_x;
	uint16_t end_y = (uint16_t)*p_end_y;
	uint32_t old_start_x = 0, old_start_y = 0, outcode = 0, point_x = 0, point_y = 0;
	uint32_t old_end_x = 0, old_end_y = 0;

	old_end_x = end_x;
	old_start_x = start_x;
	point_x = old_end_x;
	old_end_y = end_y;
	old_start_y = start_y;
	point_y = old_end_y;
	outcode = 0u;
	if ((int16_t)point_x < bitmap_window_left)
		outcode = (uint8_t)(outcode + 1u);
	if (bitmap_window_right < (int16_t)point_x)
		outcode = (uint8_t)(outcode + 2u);
	if ((int16_t)point_y < bitmap_window_top)
		outcode = (uint8_t)(outcode + 4u);
	if ((int16_t)point_y <= bitmap_window_bottom)
	{
		point_x = (int16_t)point_x;
		point_y = (int16_t)point_y;
		*p_start_x = point_x;
		*p_start_y = point_y;
		*p_end_x = old_start_x;
		*p_end_y = old_start_y;
		*p_outcode = outcode;
		return;
	}
	outcode = (uint8_t)(outcode + 8u);
	point_x = (int16_t)point_x;
	point_y = (int16_t)point_y;
	*p_start_x = point_x;
	*p_start_y = point_y;
	*p_end_x = old_start_x;
	*p_end_y = old_start_y;
	*p_outcode = outcode;
}

void clip_line_to_window(uint32_t outcode, uint32_t end_outcode, uint32_t *p_start_x, uint32_t *p_start_y, uint32_t *p_end_x, uint32_t *p_end_y, uint32_t *p_unclipped_y)
{
	uint32_t start_x = *p_start_x;
	uint32_t start_y = *p_start_y;
	uint32_t end_x = *p_end_x;
	uint32_t end_y = *p_end_y;
	uint32_t unclipped_y = *p_unclipped_y;
	uint32_t r8 = 0, swapped_end_x = 0, swapped_end_y = 0, swapped_start_x = 0, swapped_start_y = 0;
	uint32_t old_start_x = 0, old_start_y = 0;

	switch (outcode)
	{
	case 0:
		clip_line_end_inside();
		break;
	case 1:
		clip_line_to_left_edge(end_x, end_y, &start_x, &start_y, &unclipped_y);
		break;
	case 2:
		clip_line_to_right_edge(end_x, end_y, &start_x, &start_y, &unclipped_y);
		break;
	case 3:
		clip_line_end_inside();
		break;
	case 4:
		clip_line_to_top_edge(end_x, end_y, &start_x, &start_y, &unclipped_y);
		break;
	case 5:
		clip_line_to_top_edge(end_x, end_y, &start_x, &start_y, &unclipped_y);
		break;
	case 6:
		clip_line_to_top_edge(end_x, end_y, &start_x, &start_y, &unclipped_y);
		break;
	case 7:
		clip_line_end_inside();
		break;
	case 8:
		clip_line_to_bottom_edge(end_x, end_y, &start_x, &start_y, &unclipped_y);
		break;
	case 9:
		clip_line_to_bottom_edge(end_x, end_y, &start_x, &start_y, &unclipped_y);
		break;
	case 10:
		clip_line_to_bottom_edge(end_x, end_y, &start_x, &start_y, &unclipped_y);
		break;
	case 11:
		clip_line_end_inside();
		break;
	default:
		unknown_code(0x9C3D2u + outcode * 3u);
	}
	old_start_x = start_x;
	swapped_start_x = end_x;
	swapped_end_x = old_start_x;
	old_start_y = start_y;
	swapped_start_y = end_y;
	swapped_end_y = old_start_y;
	switch (end_outcode)
	{
	case 0:
		clip_line_end_inside();
		*p_start_x = swapped_start_x;
		*p_start_y = swapped_start_y;
		*p_end_x = swapped_end_x;
		*p_end_y = swapped_end_y;
		*p_unclipped_y = unclipped_y;
		return;
	case 1:
		clip_line_to_left_edge(swapped_end_x, swapped_end_y, &swapped_start_x, &swapped_start_y, &r8);
		*p_start_x = swapped_start_x;
		*p_start_y = swapped_start_y;
		*p_end_x = swapped_end_x;
		*p_end_y = swapped_end_y;
		*p_unclipped_y = r8;
		return;
	case 2:
		clip_line_to_right_edge(swapped_end_x, swapped_end_y, &swapped_start_x, &swapped_start_y, &r8);
		*p_start_x = swapped_start_x;
		*p_start_y = swapped_start_y;
		*p_end_x = swapped_end_x;
		*p_end_y = swapped_end_y;
		*p_unclipped_y = r8;
		return;
	case 3:
		clip_line_end_inside();
		*p_start_x = swapped_start_x;
		*p_start_y = swapped_start_y;
		*p_end_x = swapped_end_x;
		*p_end_y = swapped_end_y;
		*p_unclipped_y = unclipped_y;
		return;
	case 4:
		clip_line_to_top_edge(swapped_end_x, swapped_end_y, &swapped_start_x, &swapped_start_y, &r8);
		*p_start_x = swapped_start_x;
		*p_start_y = swapped_start_y;
		*p_end_x = swapped_end_x;
		*p_end_y = swapped_end_y;
		*p_unclipped_y = r8;
		return;
	case 5:
		clip_line_to_top_edge(swapped_end_x, swapped_end_y, &swapped_start_x, &swapped_start_y, &r8);
		*p_start_x = swapped_start_x;
		*p_start_y = swapped_start_y;
		*p_end_x = swapped_end_x;
		*p_end_y = swapped_end_y;
		*p_unclipped_y = r8;
		return;
	case 6:
		clip_line_to_top_edge(swapped_end_x, swapped_end_y, &swapped_start_x, &swapped_start_y, &r8);
		*p_start_x = swapped_start_x;
		*p_start_y = swapped_start_y;
		*p_end_x = swapped_end_x;
		*p_end_y = swapped_end_y;
		*p_unclipped_y = r8;
		return;
	case 7:
		clip_line_end_inside();
		*p_start_x = swapped_start_x;
		*p_start_y = swapped_start_y;
		*p_end_x = swapped_end_x;
		*p_end_y = swapped_end_y;
		*p_unclipped_y = unclipped_y;
		return;
	case 8:
		clip_line_to_bottom_edge(swapped_end_x, swapped_end_y, &swapped_start_x, &swapped_start_y, &r8);
		*p_start_x = swapped_start_x;
		*p_start_y = swapped_start_y;
		*p_end_x = swapped_end_x;
		*p_end_y = swapped_end_y;
		*p_unclipped_y = r8;
		return;
	case 9:
		clip_line_to_bottom_edge(swapped_end_x, swapped_end_y, &swapped_start_x, &swapped_start_y, &r8);
		*p_start_x = swapped_start_x;
		*p_start_y = swapped_start_y;
		*p_end_x = swapped_end_x;
		*p_end_y = swapped_end_y;
		*p_unclipped_y = r8;
		return;
	case 10:
		clip_line_to_bottom_edge(swapped_end_x, swapped_end_y, &swapped_start_x, &swapped_start_y, &r8);
		*p_start_x = swapped_start_x;
		*p_start_y = swapped_start_y;
		*p_end_x = swapped_end_x;
		*p_end_y = swapped_end_y;
		*p_unclipped_y = r8;
		return;
	case 11:
		clip_line_end_inside();
		*p_start_x = swapped_start_x;
		*p_start_y = swapped_start_y;
		*p_end_x = swapped_end_x;
		*p_end_y = swapped_end_y;
		*p_unclipped_y = unclipped_y;
		return;
	default:
		unknown_code(0x9C3D2u + end_outcode * 3u);
	}
}
