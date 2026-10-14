/**
 *    Name:        game_05.c
 *    Description: the game's functions, part 05 (in the order of their V60 addresses)
 *    Version:     0.1.0
 *    Created:     12-10-2026
 *    Modified:    14-10-2026
 *    Author:      Sebastian Toyos
 *
 *    SegaSonic The Hedgehog (arcade, Sega System 32): the game's V60 program decompiled into C by
 *    tools/decompile.py, with the names of symbols.txt.
 */
#include "game.h"

void set_nbg2_scroll_to_camera_2(void)
{
	uint16_t layer_scroll_y = 0;

	layer_scroll_y = camera_y_pixels;
	nbg2_scroll_x = camera_x_pixels;
	nbg2_scroll_y = layer_scroll_y;
}

uint32_t set_nbg2_scroll_stages_03_07(void)
{
	uint16_t layer_scroll_y = 0;
	uint32_t current_position = 0;

	nbg2_pages = 0xD0C0B0Au;
	layer_scroll_y = (uint16_t)((uint32_t)camera_y >> 16);
	nbg2_scroll_x = (uint32_t)camera_x >> 16;
	nbg2_scroll_y = layer_scroll_y;
	current_position = scroll_position;
	if ((int32_t)current_position < 0x9159)
	{
		skip_nbg2_scroll();
		return current_position;
	}
	layer_scroll_y = (uint16_t)(camera_y_pixels + 0x10u);
	nbg2_scroll_x = camera_x_pixels + 0x10u;
	nbg2_scroll_y = layer_scroll_y;
	return current_position;
}

void set_nbg2_scroll_stage_04(void)
{
	uint16_t layer_scroll_y = 0;

	nbg2_pages = 0xD0C0B0Au;
	layer_scroll_y = camera_y_pixels;
	nbg2_scroll_x = camera_x_pixels + 0x20u - 0x20u;
	nbg2_scroll_y = layer_scroll_y;
}

void init_nbg3_layer(uint32_t dest)
{
	uint32_t ignored = 0, script = 0, stage_index = 0;

	stage_index = stage;
	nbg3_block_map_width = stage_nbg3_widths[stage_index];
	switch (stage_index)
	{
	case 0:
	case 1:
	case 2:
	case 3:
	case 5:
	case 6:
	case 7:
	case 17:
	case 18:
		script = default_nbg3_script_ADDRESS;
		run_vram_script(&stage_index, &script, &dest);
		nbg3_pages = 0x6060505u;
		nbg3_scroll_x = 0u;
		nbg3_scroll_y = 0u;
		set_nbg3_page_5_flags(&ignored, &ignored);
		return;
	case 4:
		script = stage_04_nbg3_script_ADDRESS;
		run_vram_script(&stage_index, &script, &dest);
		script = stage_04_nbg3_fill_script_ADDRESS;
		run_vram_script(&stage_index, &script, &dest);
		nbg3_pages = 0x6060505u;
		nbg3_scroll_x = 0u;
		nbg3_scroll_y = 0u;
		return;
	case 8:
	case 19:
		script = stage_08_nbg3_script_ADDRESS;
		run_vram_script(&stage_index, &script, &dest);
		nbg3_pages = 0x5050505u;
		nbg3_scroll_x = 0u;
		nbg3_scroll_y = 0u;
		return;
	case 9:
	case 11:
		script = clear_tile_page_05_script_ADDRESS;
		run_vram_script(&stage_index, &script, &dest);
		script = clear_tile_page_06_script_ADDRESS;
		run_vram_script(&stage_index, &script, &dest);
		script = clear_tile_page_07_script_ADDRESS;
		run_vram_script(&stage_index, &script, &dest);
		script = clear_tile_page_08_script_ADDRESS;
		run_vram_script(&stage_index, &script, &dest);
		nbg3_pages = 0x8070605u;
		video_layer_opaque_shadow = 0u;
		return;
	case 10:
		script = stage_0a_nbg3_script_ADDRESS;
		run_vram_script(&stage_index, &script, &dest);
		nbg3_pages = 0x7070707u;
		nbg3_scroll_x = 0u;
		nbg3_scroll_y = 0u;
		return;
	case 12:
		script = stage_0c_nbg3_script_ADDRESS;
		run_vram_script(&stage_index, &script, &dest);
		nbg3_pages = 0x5050505u;
		nbg3_scroll_x = 0u;
		nbg3_scroll_y = 0u;
		return;
	case 13:
		script = stage_0d_nbg3_script_ADDRESS;
		run_vram_script(&stage_index, &script, &dest);
		nbg3_pages = 0x5050505u;
		nbg3_scroll_x = 0u;
		nbg3_scroll_y = 0u;
		return;
	case 14:
		script = stage_0e_nbg3_script_ADDRESS;
		run_vram_script(&stage_index, &script, &dest);
		nbg3_pages = 0x5050505u;
		nbg3_scroll_x = 0u;
		nbg3_scroll_y = 0u;
		return;
	case 15:
		script = stage_0f_nbg3_script_ADDRESS;
		run_vram_script(&stage_index, &script, &dest);
		nbg3_pages = 0x5050505u;
		nbg3_scroll_x = 0u;
		nbg3_scroll_y = 0u;
		return;
	case 16:
		script = stage_10_nbg3_script_ADDRESS;
		run_vram_script(&stage_index, &script, &dest);
		nbg3_pages = 0x5050505u;
		nbg3_scroll_x = 0u;
		nbg3_scroll_y = 0u;
		return;
	default:
		unknown_code(0x1B63Fu + stage_index * 3u);
	}
}

void init_nbg3_layer_cleared_pages(uint32_t dest)
{
	uint32_t count = 0, ignored = 0, script = 0;

	script = clear_tile_page_0a_script_ADDRESS;
	run_vram_script(&count, &script, &dest);
	script = clear_tile_page_0b_script_ADDRESS;
	run_vram_script(&count, &script, &dest);
	script = clear_tile_page_0c_script_ADDRESS;
	run_vram_script(&count, &script, &dest);
	script = clear_tile_page_0d_script_ADDRESS;
	run_vram_script(&count, &script, &dest);
	nbg3_pages = 0xA0B0C0Du;
	nbg3_scroll_x = 0u;
	nbg3_scroll_y = 0u;
	set_nbg3_page_5_flags(&ignored, &ignored);
}

void set_nbg3_pages_5_6(void)
{
	nbg3_pages = 0x6060505u;
	nbg3_scroll_x = 0u;
	nbg3_scroll_y = 0u;
}

void set_nbg3_page_5_flags(uint32_t *p_count, uint32_t *p_dest)
{
	uint32_t count = 0, dest = 0;

	video_layer_opaque_shadow = 0xAu;
	mem_write16(palette_color_4000, 0xF70u);
	count = 0x400u;
	dest = VIDEO_RAM + 0x1400u;
	do
	{
		mem_write16(dest, mem_read16(dest) | 0x2000u);
		count--;
		dest += 2u;
	} while (count != 0u);
	*p_count = count;
	*p_dest = dest;
}

void update_nbg3_scroll(uint32_t *p_scroll_y, uint32_t *p_quadrants, uint32_t *p_speed_value, uint32_t *p_next_scroll_position, uint32_t *p_quadrant_cursor)
{
	uint32_t scroll_y = *p_scroll_y;
	uint32_t quadrants = *p_quadrants;
	uint32_t speed_value = *p_speed_value;
	uint32_t next_scroll_position = *p_next_scroll_position;
	uint32_t quadrant_cursor = *p_quadrant_cursor;
	uint16_t half_view_y = 0, layer_scroll_y = 0;
	uint32_t r4 = 0, r5 = 0, block_pages = 0;

	switch (stage)
	{
	case 0:
		if ((uint8_t)scroll_ended == 0u)
		{
			half_view_y = (uint16_t)((int16_t)view_y >> 1);
			nbg3_scroll_x = (int16_t)view_x >> 1;
			nbg3_scroll_y = half_view_y;
			nbg3_scroll_y += 0x80u;
			select_nbg3_pages(&block_pages, &quadrants, &r4, &r5, &quadrant_cursor);
			*p_scroll_y = block_pages;
			*p_quadrants = quadrants;
			*p_speed_value = r4;
			*p_next_scroll_position = r5;
			*p_quadrant_cursor = quadrant_cursor;
			return;
		}
		scroll_y = (scroll_y & ~0xFFFFu) | (uint16_t)view_y;
		scroll_y = (scroll_y & ~0xFFFFu) | (uint16_t)((int16_t)scroll_y >> 2);
		nbg3_scroll_x = (int16_t)view_x >> 2;
		nbg3_scroll_y = scroll_y;
		nbg3_scroll_y += 0x80u;
		*p_scroll_y = scroll_y;
		return;
	case 1:
	case 2:
	case 4:
	case 8:
	case 11:
	case 13:
	case 15:
	case 17:
	case 18:
	case 19:
		return;
	case 3:
	case 5:
	case 6:
	case 7:
	case 10:
		half_view_y = (uint16_t)((int16_t)view_y >> 1);
		nbg3_scroll_x = (int16_t)view_x >> 1;
		nbg3_scroll_y = half_view_y;
		nbg3_scroll_y += 0x80u;
		select_nbg3_pages(&block_pages, &quadrants, &r4, &r5, &quadrant_cursor);
		*p_scroll_y = block_pages;
		*p_quadrants = quadrants;
		*p_speed_value = r4;
		*p_next_scroll_position = r5;
		*p_quadrant_cursor = quadrant_cursor;
		return;
	case 9:
	case 20:
		layer_scroll_y = camera_y_pixels;
		nbg3_scroll_x = camera_x_pixels;
		nbg3_scroll_y = layer_scroll_y;
		return;
	case 12:
	case 14:
		scroll_y = (uint16_t)view_y * 3u;
		scroll_y = (scroll_y & ~0xFFFFu) | (uint16_t)((int16_t)scroll_y >> 2);
		nbg3_scroll_x = (int16_t)(view_x * 3u) >> 2;
		nbg3_scroll_y = scroll_y;
		*p_scroll_y = scroll_y;
		return;
	case 16:
		scroll_y = (uint16_t)view_y;
		scroll_y = (uint16_t)((int16_t)scroll_y >> 2);
		nbg3_scroll_x = (int16_t)view_x >> 2;
		nbg3_scroll_y = scroll_y;
		*p_scroll_y = scroll_y;
		return;
	default:
		unknown_code(0x1B89Eu + stage * 3u);
	}
}

void select_nbg3_pages(uint32_t *p_block_pages, uint32_t *p_quadrants, uint32_t *p_byte_mask, uint32_t *p_shift, uint32_t *p_quadrant_cursor)
{
	uint8_t byte_shift = 0;
	uint32_t block_column = 0, block_pages = 0, block_row = 0, byte_mask = 0, bytes_left = 0, gathered_pages = 0, gathered_quadrants = 0, map_cursor = 0, pages = 0, quadrant_cursor = 0, quadrants = 0, shift = 0, stage_index = 0;
	uint32_t next_mask = 0;

	get_nbg3_block();
	stage_index = stage;
	nbg3_block_row = (uint32_t)nbg3_block_row * nbg3_block_map_width;
	map_cursor = stage_nbg3_block_maps[stage_index];
	map_cursor = (map_cursor & ~0xFFFFu) | (uint16_t)(map_cursor + nbg3_block_row);
	map_cursor = (map_cursor & ~0xFFFFu) | (uint16_t)(map_cursor + nbg3_block_column);
	get_nbg3_block();
	nbg3_block_row = nbg3_block_row * (nbg3_block_map_width - 1u);
	block_row = nbg3_block_row;
	block_column = nbg3_block_column;
	quadrant_cursor = 0x1BCBCu + (block_row & 1u) * 3u + (block_column & 1u);
	nbg3_scroll_x &= 0x3FFu;
	nbg3_scroll_y &= 0x1FFu;
	bytes_left = 4u;
	byte_shift = 0u;
	gathered_pages = 0u;
	do
	{
		gathered_pages |= rotate_by(mem_read8(map_cursor), (uint8_t)byte_shift, 32);
		map_cursor++;
		byte_shift += 8u;
		if (bytes_left == 3u)
			map_cursor = (map_cursor & ~0xFFFFu) | (uint16_t)(map_cursor + (nbg3_block_map_width - 2u));
		bytes_left--;
	} while (bytes_left != 0u);
	block_pages = gathered_pages;
	bytes_left = 4u;
	byte_shift = 0u;
	gathered_quadrants = 0u;
	do
	{
		gathered_quadrants |= rotate_by(mem_read8(quadrant_cursor), (uint8_t)byte_shift, 32);
		quadrant_cursor++;
		byte_shift += 8u;
		if (bytes_left == 3u)
			quadrant_cursor = (quadrant_cursor & ~0xFFFFu) | (uint16_t)(quadrant_cursor + 1u);
		bytes_left--;
	} while (bytes_left != 0u);
	quadrants = gathered_quadrants;
	byte_mask = 0xFFu;
	shift = 0u;
	pages = 0u;
	do
	{
		pages |= shift_by(shift_by(block_pages & byte_mask, (uint8_t)shift, 32), (uint8_t)(shift_by(quadrants & byte_mask, (uint8_t)shift, 32) << 3), 32);
		next_mask = byte_mask << 8;
		byte_mask = next_mask;
		shift -= 8u;
	} while (next_mask != 0u);
	nbg3_pages = pages;
	*p_block_pages = block_pages;
	*p_quadrants = quadrants;
	*p_byte_mask = byte_mask;
	*p_shift = shift;
	*p_quadrant_cursor = quadrant_cursor;
}

void get_nbg3_block(void)
{
	nbg3_block_column = nbg3_scroll_x;
	nbg3_block_row = nbg3_scroll_y;
	nbg3_block_row += 0x400u;
	nbg3_block_column &= ~0x1FFu;
	nbg3_block_row &= ~0xFFu;
	nbg3_block_column = (int16_t)nbg3_block_column >> 9;
	nbg3_block_row = (int16_t)nbg3_block_row >> 8;
}

void build_ending_scene(uint32_t *p_write_index_address, uint32_t *p_tile_bank_bits, uint32_t *p_color, uint32_t *p_row_pairs_left, uint32_t *p_blocks_per_row, uint32_t *p_scene_actor)
{
	uint32_t write_index_address = *p_write_index_address;
	uint32_t tile_bank_bits = *p_tile_bank_bits;
	uint32_t color = *p_color;
	uint32_t row_pairs_left = *p_row_pairs_left;
	uint32_t blocks_per_row = *p_blocks_per_row;
	uint32_t scene_actor = *p_scene_actor;
	uint8_t is_8bpp = 0, sound_id = 0;
	uint32_t r1 = 0, r4 = 0, r11 = 0, ignored = 0, inner_count = 0, lower_row_cursor = 0, script = 0;
	uint8_t io_cnt_bits = 0;

	is_8bpp = (uint8_t)(ending_scene_step * 3u);
	switch (ending_scene_step)
	{
	case 0:
		io_cnt_bits = mem_read8(io_cnt);
		mem_write8(io_cnt, io_cnt_bits & ~2u);
		display_off_frames = 0x10u;
		r4 = set_stage_layer_mixing();
		clear_sprite_buckets();
		clear_bitmap_layer(is_8bpp, &ignored, &r4, &inner_count, &row_pairs_left, &blocks_per_row, &lower_row_cursor);
		init_video_registers();
		load_stage_palette_set(&tile_bank_bits, &r11);
		sound_id = 0x94u;
		queue_sound(sound_id, &r1, &write_index_address);
		color_offset_red = 0x20u;
		color_offset_blue = 0x20u;
		color_offset_green = 0x20u;
		mem_write16(video_layer_disable, mem_read16(video_layer_disable) | 0x2Fu);
		nbg0_pages = 0x3020100u;
		nbg1_pages = 0x1211100Fu;
		nbg2_pages = 0xD0C0B0Au;
		nbg3_pages = 0x8070605u;
		nbg0_scroll_x_int = 0u;
		nbg0_scroll_y_int = 0u;
		nbg1_scroll_x_int = 0u;
		nbg1_scroll_y_int = 0u;
		nbg2_scroll_x = 0u;
		nbg2_scroll_y = 0u;
		nbg3_scroll_x = 0u;
		nbg3_scroll_y = 0u;
		ending_scene_step = 1u;
		ending_first_scene_done = 0u;
		mixer_text_blend &= 0xBFFFu;
		mixer_nbg0_blend &= 0xBFFFu;
		mixer_nbg1_blend &= 0xBFFFu;
		mixer_nbg2_blend &= 0xBFFFu;
		mixer_nbg3_blend &= 0xBFFFu;
		mixer_bitmap_blend &= 0xBFFFu;
		mixer_sprite_control &= ~0x8000u;
		mem_write32(video_control, mem_read32(video_control) & ~0x1000u);
		mem_write32(video_control, mem_read32(video_control) & ~0x2000u);
		mem_write32(video_control, mem_read32(video_control) & ~0x4000u);
		mem_write16(nbg0_zoom_x, 0x200u);
		mem_write16(nbg0_zoom_y, 0x200u);
		mem_write16(nbg1_zoom_x, 0x200u);
		mem_write16(nbg1_zoom_y, 0x200u);
		nbg0_center_x = 0u;
		nbg0_center_y = 0u;
		nbg1_center_x = 0u;
		nbg1_center_y = 0u;
		mem_write16(video_clip_1_left, 0u);
		mem_write16(video_clip_1_right, 0u);
		mem_write16(video_clip_1_top, 0u);
		mem_write16(video_clip_1_bottom, 0u);
		script = blue_sky_script_ADDRESS;
		run_vram_script(&r1, &script, &r11);
		script = aerial_city_script_ADDRESS;
		run_vram_script(&r1, &script, &r11);
		script = sonic_head_pattern_script_ADDRESS;
		run_vram_script(&r1, &script, &r11);
		script = ending_eggman_machine_script_ADDRESS;
		run_vram_script(&r1, &script, &r11);
		script = ending_logo_script_ADDRESS;
		run_vram_script(&r1, &script, &r11);
		script = ending_bars_script_ADDRESS;
		run_vram_script(&r1, &script, &r11);
		nbg0_pages = 0x8080707u;
		nbg1_pages = 0x1010101u;
		nbg2_pages = 0x5050505u;
		*p_write_index_address = write_index_address;
		*p_tile_bank_bits = tile_bank_bits;
		*p_color = r4;
		*p_row_pairs_left = row_pairs_left;
		*p_blocks_per_row = blocks_per_row;
		*p_scene_actor = r11;
		return;
	case 1:
		allocate_object_outside_loop(&ignored, &scene_actor);
		OBJECT(scene_actor)->handler = ending_tower_animate_ADDRESS;
		init_ending_scene_object(scene_actor);
		allocate_object_outside_loop(&ignored, &scene_actor);
		OBJECT(scene_actor)->handler = ending_horizon_show_ADDRESS;
		init_ending_scene_object(scene_actor);
		allocate_object_outside_loop(&ignored, &scene_actor);
		OBJECT(scene_actor)->handler = ending_sonic_run_forward_ADDRESS;
		init_ending_scene_object(scene_actor);
		allocate_object_outside_loop(&ignored, &scene_actor);
		OBJECT(scene_actor)->handler = ending_ray_run_forward_ADDRESS;
		init_ending_scene_object(scene_actor);
		allocate_object_outside_loop(&ignored, &scene_actor);
		OBJECT(scene_actor)->handler = ending_mighty_run_forward_ADDRESS;
		init_ending_scene_object(scene_actor);
		allocate_object_outside_loop(&ignored, &scene_actor);
		OBJECT(scene_actor)->handler = ending_road_scroll_ADDRESS;
		init_ending_scene_object(scene_actor);
		*p_scene_actor = scene_actor;
		return;
	case 2:
	case 3:
		return;
	default:
		unknown_code(0x1BCD3u + ending_scene_step * 3u);
	}
}

void init_ending_scene_object(uint32_t object)
{
	AS8(OBJECT(object)->flags) = 0u;
	AT8(OBJECT(object)->flags, 1) = 0u;
	AT16(OBJECT(object)->flags, 2) = 0u;
	ending_scene_step = 2u;
}

void ending_tower_animate(void)
{
	uint32_t r11 = 0, count = 0, frame_record = 0, script = 0;

	if (OBJECT(current_object)->ending.started == 0u)
	{
		script = cloudy_horizon_script_ADDRESS;
		run_vram_script(&count, &script, &r11);
		mem_write16(video_layer_disable, mem_read16(video_layer_disable) & ~8u);
		nbg3_pages = 0u;
		OBJECT(current_object)->ending.list_index = 0u;
		OBJECT(current_object)->ending.timer = 0;
		OBJECT(current_object)->ending.started++;
	}
	OBJECT(current_object)->sprite_layer = 7u;
	OBJECT(current_object)->screen_x = 0xA0;
	OBJECT(current_object)->screen_y = 0x6B;
	OBJECT(current_object)->sprite_scale = 0x100u;
	OBJECT(current_object)->sprite_flags = 0u;
	frame_record = ending_tower_frames_ADDRESS;
	for (;;)
	{
		if (ROM16(frame_record) == 0xFFFFu)
		{
			if (ROM16(frame_record + 4u) != 0xFFFFu)
			{
				OBJECT(current_object)->ending.timer++;
				run_timed_spawns_2(r11);
				return;
			}
			delete_current_object();
			return;
		}
		if (OBJECT(current_object)->ending.timer < (int16_t)ROM16(frame_record))
			break;
		frame_record += 6u;
	}
	OBJECT(current_object)->sprite_frame = ROM16(frame_record + 2u);
	OBJECT(current_object)->sprite_palette = ROM16(frame_record + 4u);
	queue_sprite_in_layer();
	if (ROM16(frame_record + 4u) != 0xFFFFu)
	{
		OBJECT(current_object)->ending.timer++;
		run_timed_spawns_2(r11);
		return;
	}
	delete_current_object();
}

void ending_debris_1_fly(void)
{
	uint8_t sound_id = 0;
	uint32_t ignored = 0, path = 0;

	if (OBJECT(current_object)->ending.started == 0u)
	{
		sound_id = 0xBu;
		queue_sound(sound_id, &ignored, &ignored);
		OBJECT(current_object)->ending.started++;
		OBJECT(current_object)->ending.timer = 0;
	}
	path = ending_debris_1_path_ADDRESS;
	OBJECT(current_object)->sprite_flags = 0u;
	OBJECT(current_object)->sprite_flags |= 9u;
	animate_ending_debris(path);
}

void ending_debris_2_fly(void)
{
	uint8_t sound_id = 0;
	uint32_t ignored = 0, path = 0;

	if (OBJECT(current_object)->ending.started == 0u)
	{
		sound_id = 0xBu;
		queue_sound(sound_id, &ignored, &ignored);
		OBJECT(current_object)->ending.started++;
		OBJECT(current_object)->ending.timer = 0;
	}
	path = ending_debris_2_path_ADDRESS;
	OBJECT(current_object)->sprite_flags = 0u;
	OBJECT(current_object)->sprite_flags |= 0xAu;
	animate_ending_debris(path);
}

void ending_debris_3_fly(void)
{
	uint8_t sound_id = 0;
	uint32_t ignored = 0, path = 0;

	if (OBJECT(current_object)->ending.started == 0u)
	{
		sound_id = 0xBu;
		queue_sound(sound_id, &ignored, &ignored);
		OBJECT(current_object)->ending.started++;
		OBJECT(current_object)->ending.timer = 0;
	}
	path = ending_debris_3_path_ADDRESS;
	OBJECT(current_object)->sprite_flags = 0u;
	OBJECT(current_object)->sprite_flags |= 9u;
	animate_ending_debris(path);
}

void ending_tower_blast_1(void)
{
	uint8_t sound_id = 0;
	uint32_t ignored = 0;

	if (OBJECT(current_object)->ending.started == 0u)
	{
		sound_id = 0xAu;
		queue_sound(sound_id, &ignored, &ignored);
		start_ending_blast();
	}
	OBJECT(current_object)->screen_x = 0x90;
	OBJECT(current_object)->screen_y = 0xA0;
	animate_tower_blast();
}

void ending_tower_blast_2(void)
{
	uint8_t sound_id = 0;
	uint32_t ignored = 0;

	if (OBJECT(current_object)->ending.started == 0u)
	{
		sound_id = 0xAu;
		queue_sound(sound_id, &ignored, &ignored);
		start_ending_blast();
	}
	OBJECT(current_object)->screen_x = 0xA1;
	OBJECT(current_object)->screen_y = 0x98;
	animate_tower_blast();
}

void ending_tower_blast_3(void)
{
	uint8_t sound_id = 0;
	uint32_t ignored = 0;

	if (OBJECT(current_object)->ending.started == 0u)
	{
		sound_id = 0xAu;
		queue_sound(sound_id, &ignored, &ignored);
		start_ending_blast();
	}
	OBJECT(current_object)->screen_x = 0xAE;
	OBJECT(current_object)->screen_y = 0xA0;
	animate_tower_blast();
}

void ending_horizon_blast_1(void)
{
	uint8_t sound_id = 0;
	uint32_t ignored = 0;

	if (OBJECT(current_object)->ending.started == 0u)
	{
		sound_id = 9u;
		queue_sound(sound_id, &ignored, &ignored);
		start_ending_blast();
	}
	OBJECT(current_object)->screen_x = 0x35;
	OBJECT(current_object)->screen_y = 0xAA;
	animate_horizon_blast();
}

void ending_horizon_blast_2(void)
{
	uint8_t sound_id = 0;
	uint32_t ignored = 0;

	if (OBJECT(current_object)->ending.started == 0u)
	{
		sound_id = 9u;
		queue_sound(sound_id, &ignored, &ignored);
		start_ending_blast();
	}
	OBJECT(current_object)->screen_x = 0x52;
	OBJECT(current_object)->screen_y = 0xA8;
	animate_horizon_blast();
}

void ending_horizon_blast_3(void)
{
	uint8_t sound_id = 0;
	uint32_t ignored = 0;

	if (OBJECT(current_object)->ending.started == 0u)
	{
		sound_id = 9u;
		queue_sound(sound_id, &ignored, &ignored);
		start_ending_blast();
	}
	OBJECT(current_object)->screen_x = 0x73;
	OBJECT(current_object)->screen_y = 0xA5;
	animate_horizon_blast();
}

void ending_horizon_blast_4(void)
{
	uint8_t sound_id = 0;
	uint32_t ignored = 0;

	if (OBJECT(current_object)->ending.started == 0u)
	{
		sound_id = 9u;
		queue_sound(sound_id, &ignored, &ignored);
		start_ending_blast();
	}
	OBJECT(current_object)->screen_x = 0x93;
	OBJECT(current_object)->screen_y = 0xA0;
	animate_horizon_blast();
}

void ending_horizon_blast_5(void)
{
	uint8_t sound_id = 0;
	uint32_t ignored = 0;

	if (OBJECT(current_object)->ending.started == 0u)
	{
		sound_id = 9u;
		queue_sound(sound_id, &ignored, &ignored);
		start_ending_blast();
	}
	OBJECT(current_object)->screen_x = 0xAD;
	OBJECT(current_object)->screen_y = 0x9E;
	animate_horizon_blast();
}

void ending_horizon_blast_6(void)
{
	uint8_t sound_id = 0;
	uint32_t ignored = 0;

	if (OBJECT(current_object)->ending.started == 0u)
	{
		sound_id = 9u;
		queue_sound(sound_id, &ignored, &ignored);
		start_ending_blast();
	}
	OBJECT(current_object)->screen_x = 0xC6;
	OBJECT(current_object)->screen_y = 0xA2;
	animate_horizon_blast();
}

void ending_horizon_blast_7(void)
{
	uint8_t sound_id = 0;
	uint32_t ignored = 0;

	if (OBJECT(current_object)->ending.started == 0u)
	{
		sound_id = 9u;
		queue_sound(sound_id, &ignored, &ignored);
		start_ending_blast();
	}
	OBJECT(current_object)->screen_x = 0xCF;
	OBJECT(current_object)->screen_y = 0xAF;
	animate_horizon_blast();
}

void ending_horizon_blast_8(void)
{
	uint8_t sound_id = 0;
	uint32_t ignored = 0;

	if (OBJECT(current_object)->ending.started == 0u)
	{
		sound_id = 9u;
		queue_sound(sound_id, &ignored, &ignored);
		start_ending_blast();
	}
	OBJECT(current_object)->screen_x = 0xE4;
	OBJECT(current_object)->screen_y = 0xAC;
	animate_horizon_blast();
}

void ending_horizon_blast_9(void)
{
	uint8_t sound_id = 0;
	uint32_t ignored = 0;

	if (OBJECT(current_object)->ending.started != 0u)
	{
		OBJECT(current_object)->screen_x = 0xE4;
		OBJECT(current_object)->screen_y = 0xAC;
		animate_horizon_blast();
		return;
	}
	sound_id = 9u;
	queue_sound(sound_id, &ignored, &ignored);
	start_ending_blast();
	OBJECT(current_object)->screen_x = 0xF8;
	OBJECT(current_object)->screen_y = 0xB0;
	animate_horizon_blast();
}

void start_ending_blast(void)
{
	OBJECT(current_object)->start_ending_blast.started = 1u;
	OBJECT(current_object)->start_ending_blast.frame_index = 0u;
	OBJECT(current_object)->start_ending_blast.timer = 0;
}

void ending_big_explosion(void)
{
	uint8_t sound_id = 0;
	uint16_t speed_x = 0;
	uint32_t frame_record = 0, ignored = 0, second_frame_record = 0, second_object = 0;

	if (OBJECT(current_object)->ending.started == 0u)
	{
		OBJECT(current_object)->ending.frame_index = 0u;
		OBJECT(current_object)->ending.timer = 0;
		OBJECT(current_object)->ending.started = 1u;
		OBJECT(current_object)->ending.step = 0u;
		OBJECT(current_object)->screen_x = 0xA1;
		OBJECT(current_object)->screen_y = 0x62;
		OBJECT(current_object)->sprite_layer = 9u;
		OBJECT(current_object)->sprite_scale = 0x100u;
		OBJECT(current_object)->sprite_palette = 0x1100u;
		allocate_object_2(&ignored, &ignored, &second_object);
		OBJECT(current_object)->speed_x = second_object;
		current_object = second_object;
		OBJECT(current_object)->screen_x = 0xA1;
		OBJECT(current_object)->screen_y = 0x62;
		OBJECT(current_object)->sprite_layer = 9u;
		OBJECT(current_object)->sprite_scale = 0x100u;
		OBJECT(current_object)->sprite_palette = 0x1100u;
		sound_id = 7u;
		queue_sound(sound_id, &ignored, &ignored);
	}
	current_object = object_cursor;
	frame_record = ending_big_explosion_frames_ADDRESS + OBJECT(current_object)->ending.frame_index * 4u;
	second_frame_record = ending_big_explosion_frames_2_ADDRESS + OBJECT(current_object)->ending.frame_index * 4u;
	if (ROM16(frame_record) == 0xFFFFu)
	{
		speed_x = (uint16_t)OBJECT(object_cursor)->speed_x;
		free_object_2(speed_x);
		delete_current_object();
		return;
	}
	OBJECT(current_object)->sprite_frame = ROM16(frame_record);
	OBJECT(current_object)->sprite_flags = ROM16(frame_record + 2u);
	queue_sprite_in_layer();
	if (ROM16(second_frame_record) != 0xFFFFu)
	{
		current_object = OBJECT(current_object)->speed_x;
		OBJECT(current_object)->sprite_frame = ROM16(second_frame_record);
		OBJECT(current_object)->sprite_flags = ROM16(second_frame_record + 2u);
		queue_sprite_in_layer();
		current_object = object_cursor;
	}
	OBJECT(current_object)->ending.timer++;
	if (OBJECT(current_object)->ending.timer >= 6)
	{
		OBJECT(current_object)->ending.timer = 0;
		OBJECT(current_object)->ending.frame_index++;
	}
}

void ending_flash_to_white(void)
{
	uint32_t ignored = 0, object = 0;

	if (OBJECT(current_object)->ending.started == 0u)
	{
		OBJECT(current_object)->ending.started = 1u;
		mixer_offset_select = 0x80FFu;
		color_offset_2_red = 0u;
		color_offset_2_blue = 0u;
		color_offset_2_green = 0u;
		OBJECT(current_object)->ending.timer = 0;
	}
	OBJECT(current_object)->ending.timer++;
	if (OBJECT(current_object)->ending.timer >= 3)
	{
		color_offset_2_red++;
		color_offset_2_blue++;
		color_offset_2_green++;
		OBJECT(current_object)->ending.timer = 0;
	}
	if (color_offset_2_red != 0x1Fu)
		return;
	ending_first_scene_done = 1u;
	allocate_object_outside_loop(&ignored, &object);
	OBJECT(object)->handler = ending_credits_fade_in_ADDRESS;
	OBJECT(object)->ending.started = 0u;
	delete_current_object();
}

void animate_horizon_blast(void)
{
	uint32_t frame_entry = 0;

	frame_entry = ending_horizon_blast_frames_ADDRESS + OBJECT(current_object)->animate_horizon_blast.frame_index * 2u;
	if (ROM16(frame_entry) != 0xFFFFu)
	{
		draw_ending_blast_frame(frame_entry);
		return;
	}
	delete_current_object();
}

void draw_ending_blast_frame_copy(uint32_t frame_entry)
{
	OBJECT(current_object)->sprite_frame = mem_read16(frame_entry);
	OBJECT(current_object)->sprite_layer = 6u;
	OBJECT(current_object)->sprite_flags = 0u;
	OBJECT(current_object)->sprite_scale = 0x100u;
	OBJECT(current_object)->sprite_palette = 0x10B0u;
	queue_sprite_in_layer();
	OBJECT(current_object)->draw_ending_blast_frame.timer++;
	if (OBJECT(current_object)->draw_ending_blast_frame.timer >= 6)
	{
		OBJECT(current_object)->draw_ending_blast_frame.frame_index++;
		OBJECT(current_object)->draw_ending_blast_frame.timer = 0;
	}
}

void animate_tower_blast(void)
{
	uint32_t frame_entry = 0;

	frame_entry = ending_tower_blast_frames_ADDRESS + OBJECT(current_object)->animate_tower_blast.frame_index * 2u;
	if (ROM16(frame_entry) != 0xFFFFu)
	{
		draw_ending_blast_frame(frame_entry);
		return;
	}
	delete_current_object();
}

void draw_ending_blast_frame(uint32_t frame_entry)
{
	OBJECT(current_object)->sprite_frame = mem_read16(frame_entry);
	OBJECT(current_object)->sprite_layer = 6u;
	OBJECT(current_object)->sprite_flags = 0u;
	OBJECT(current_object)->sprite_scale = 0x100u;
	OBJECT(current_object)->sprite_palette = 0x10B0u;
	queue_sprite_in_layer();
	OBJECT(current_object)->draw_ending_blast_frame.timer++;
	if (OBJECT(current_object)->draw_ending_blast_frame.timer >= 6)
	{
		OBJECT(current_object)->draw_ending_blast_frame.frame_index++;
		OBJECT(current_object)->draw_ending_blast_frame.timer = 0;
	}
}

void animate_ending_debris(uint32_t path)
{
	AS8(OBJECT(current_object)->animate_ending.timer)++;
	for (;;)
	{
		if (mem_read16(path) == 0xFFFFu)
		{
			delete_current_object();
			return;
		}
		if (OBJECT(current_object)->animate_ending.timer < (int16_t)mem_read16(path))
			break;
		path += 8u;
	}
	OBJECT(current_object)->sprite_palette = 0x10C0u;
	OBJECT(current_object)->sprite_layer = 6u;
	OBJECT(current_object)->screen_x = mem_read16(path + 2u);
	OBJECT(current_object)->screen_y = mem_read16(path + 4u);
	OBJECT(current_object)->sprite_frame = mem_read16(path + 6u);
	OBJECT(current_object)->sprite_scale = 0x100u;
	queue_sprite_in_layer();
}

void run_timed_spawns_2(uint32_t object)
{
	uint32_t ignored = 0, spawn_record = 0;

	spawn_record = timed_spawns_2_ADDRESS + OBJECT(current_object)->run_timed_spawns.list_index * 4u;
	OBJECT(current_object)->z = ROM16(spawn_record + 2u);
	if (OBJECT(current_object)->z_fraction != 0xFFFFu && (int16_t)ROM16(spawn_record) <= OBJECT(current_object)->run_timed_spawns.timer)
	{
		allocate_object_outside_loop(&ignored, &object);
		OBJECT(current_object)->z += 0x1BFC4u;
		OBJECT(object)->handler = OBJECT(current_object)->z;
		AS8(OBJECT(object)->flags) = 0u;
		OBJECT(current_object)->run_timed_spawns.list_index++;
	}
}

void ending_horizon_show(void)
{
	uint32_t ignored = 0, object = 0;

	if (OBJECT(current_object)->ending.started == 0u)
	{
		OBJECT(current_object)->ending.started++;
		OBJECT(current_object)->ending.timer = 0;
	}
	if (ending_first_scene_done == 1u)
	{
		delete_current_object();
		return;
	}
	OBJECT(current_object)->sprite_layer = 5u;
	OBJECT(current_object)->screen_x = 0x9E;
	OBJECT(current_object)->screen_y = 0xBE;
	OBJECT(current_object)->sprite_scale = 0x100u;
	OBJECT(current_object)->sprite_frame = 0x9AC0u;
	OBJECT(current_object)->sprite_flags = 0u;
	OBJECT(current_object)->sprite_palette = 0x10F0u;
	queue_sprite_in_layer();
	OBJECT(current_object)->ending.timer++;
	if ((int16_t)AS16(OBJECT(current_object)->ending.started) <= 1 && OBJECT(current_object)->ending.timer >= 0x258)
	{
		allocate_object_outside_loop(&ignored, &object);
		OBJECT(object)->handler = ending_dust_cloud_rise_ADDRESS;
		OBJECT(object)->ending.started = 0u;
		OBJECT(current_object)->ending.started = 2u;
	}
}

void ending_dust_cloud_rise(void)
{
	uint32_t frame_pointer = 0;

	if (OBJECT(current_object)->ending.started == 0u)
	{
		OBJECT(current_object)->z = 0x1C636;
		OBJECT(current_object)->ending.started = 1u;
		OBJECT(current_object)->ending.timer = 0;
	}
	if (ending_first_scene_done == 1u)
	{
		delete_current_object();
		return;
	}
	OBJECT(current_object)->ending.timer++;
	OBJECT(current_object)->sprite_layer = 6u;
	OBJECT(current_object)->screen_x = 0x9E;
	OBJECT(current_object)->sprite_scale = 0x100u;
	OBJECT(current_object)->sprite_flags = 0u;
	OBJECT(current_object)->sprite_palette = 0x10D0u;
	frame_pointer = OBJECT(current_object)->z;
	if (mem_read16(frame_pointer) != 0xFFFFu && OBJECT(current_object)->ending.timer >= 7)
	{
		OBJECT(current_object)->screen_y = mem_read16(frame_pointer);
		frame_pointer += 2u;
		OBJECT(current_object)->sprite_frame = mem_read16(frame_pointer);
		OBJECT(current_object)->z = frame_pointer + 2u;
		OBJECT(current_object)->ending.timer = 0;
	}
	queue_sprite_in_layer();
}

void ending_sonic_run_forward(void)
{
	uint32_t frame_entry = 0;

	if (OBJECT(current_object)->ending.started == 0u)
	{
		OBJECT(current_object)->ending.started++;
		OBJECT(current_object)->ending.frame_index = 0u;
		OBJECT(current_object)->sprite_scale = 0x40u;
		OBJECT(current_object)->screen_y = 0xC0;
		OBJECT(current_object)->ending.timer = 0;
		OBJECT(current_object)->ending.cycle_count = 0;
	}
	frame_entry = ending_sonic_run_frames_ADDRESS + OBJECT(current_object)->ending.frame_index * 2u;
	OBJECT(current_object)->sprite_palette = 0x130u;
	OBJECT(current_object)->sprite_layer = 0u;
	OBJECT(current_object)->screen_x = 0xA0;
	OBJECT(current_object)->ending.cycle_count++;
	if ((uint16_t)OBJECT(current_object)->ending.cycle_count == 0x20u)
	{
		OBJECT(current_object)->screen_y--;
		OBJECT(current_object)->ending.cycle_count = 0;
	}
	animate_ending_runner(frame_entry);
}

void ending_ray_run_forward(void)
{
	uint32_t frame_entry = 0;

	if (OBJECT(current_object)->ending.started == 0u)
	{
		OBJECT(current_object)->ending.started++;
		OBJECT(current_object)->ending.frame_index = 0u;
		OBJECT(current_object)->sprite_scale = 0x40u;
		OBJECT(current_object)->screen_y = 0xC0;
		OBJECT(current_object)->screen_x = 0x90;
		OBJECT(current_object)->ending.timer = 0;
		OBJECT(current_object)->ending.cycle_count = 0;
	}
	frame_entry = ending_ray_run_frames_ADDRESS + OBJECT(current_object)->ending.frame_index * 2u;
	OBJECT(current_object)->sprite_palette = 0x140u;
	OBJECT(current_object)->sprite_layer = 1u;
	OBJECT(current_object)->ending.cycle_count++;
	if ((uint16_t)OBJECT(current_object)->ending.cycle_count == 0x20u)
	{
		OBJECT(current_object)->screen_x--;
		OBJECT(current_object)->screen_x--;
		OBJECT(current_object)->screen_y--;
		OBJECT(current_object)->ending.cycle_count = 0;
	}
	animate_ending_runner(frame_entry);
}

void ending_mighty_run_forward(void)
{
	uint32_t frame_entry = 0;

	if (OBJECT(current_object)->ending.started == 0u)
	{
		OBJECT(current_object)->ending.started++;
		OBJECT(current_object)->ending.frame_index = 0u;
		OBJECT(current_object)->sprite_scale = 0x40u;
		OBJECT(current_object)->screen_y = 0xC0;
		OBJECT(current_object)->screen_x = 0xB0;
		OBJECT(current_object)->ending.timer = 0;
		OBJECT(current_object)->ending.cycle_count = 0;
	}
	frame_entry = ending_mighty_run_frames_ADDRESS + OBJECT(current_object)->ending.frame_index * 2u;
	OBJECT(current_object)->sprite_palette = 0x150u;
	OBJECT(current_object)->sprite_layer = 1u;
	OBJECT(current_object)->ending.cycle_count++;
	if ((uint16_t)OBJECT(current_object)->ending.cycle_count == 0x20u)
	{
		OBJECT(current_object)->screen_x++;
		OBJECT(current_object)->screen_x++;
		OBJECT(current_object)->screen_y--;
		OBJECT(current_object)->ending.cycle_count = 0;
	}
	animate_ending_runner(frame_entry);
}

void ending_road_scroll(void)
{
	if (OBJECT(current_object)->ending.started == 0u)
	{
		OBJECT(current_object)->ending.started++;
		OBJECT(current_object)->ending.frame_index = 0u;
		OBJECT(current_object)->sprite_scale = 0x100u;
		OBJECT(current_object)->screen_y = 0xD4;
		OBJECT(current_object)->screen_x = 0x9F;
		OBJECT(current_object)->sprite_layer = 2u;
		OBJECT(current_object)->sprite_flags = 0u;
		OBJECT(current_object)->sprite_palette = 0x12B0u;
		OBJECT(current_object)->ending.timer = 0;
		OBJECT(current_object)->ending.cycle_count = 0;
	}
	OBJECT(current_object)->ending.cycle_count++;
	if (ending_first_scene_done == 1u)
	{
		delete_current_object();
		return;
	}
	if (OBJECT(current_object)->ending.cycle_count > 4)
		OBJECT(current_object)->ending.cycle_count = 0;
	OBJECT(current_object)->sprite_frame = ending_road_frames[(uint16_t)OBJECT(current_object)->ending.cycle_count];
	queue_sprite_in_layer();
}

void ending_credits_fade_in(void)
{
	uint32_t ignored = 0, object = 0;

	if (OBJECT(current_object)->ending.started == 0u)
	{
		object = load_credits_palettes();
		color_offset_2_red = 0x1Fu;
		color_offset_2_blue = 0x1Fu;
		color_offset_2_green = 0x1Fu;
		allocate_object_outside_loop(&ignored, &object);
		OBJECT(object)->handler = ending_credits_spawner_run_ADDRESS;
		OBJECT(object)->ending.started = 0u;
		mem_write16(video_layer_disable, mem_read16(video_layer_disable) & 0xFF0u);
		mem_write16(video_layer_disable, mem_read16(video_layer_disable) | 0x10A8u);
		mem_write16(video_backdrop_control, mem_read16(video_backdrop_control) | 0x200u);
		mem_write16(backdrop_color, 0x5FD2u);
		nbg2_scroll_x = 0u;
		nbg2_scroll_y = 0u;
		nbg1_scroll_x_int = 0u;
		nbg1_scroll_y_int = 0u;
		mixer_blend &= 0x1F00u;
		OBJECT(current_object)->ending.cycle_count = 0;
		OBJECT(current_object)->ending.timer = 0;
		OBJECT(current_object)->ending.started = 1u;
		OBJECT(current_object)->ending.step = 0u;
	}
	OBJECT(current_object)->ending.timer++;
	if (OBJECT(current_object)->ending.timer < 0x78)
		return;
	OBJECT(current_object)->ending.timer = 0x79;
	if (OBJECT(current_object)->ending.step == 0u)
	{
		allocate_object_outside_loop(&ignored, &object);
		OBJECT(object)->handler = ending_music_play_ADDRESS;
		OBJECT(object)->ending.started = 0u;
		OBJECT(current_object)->ending.step = 1u;
	}
	if (color_offset_2_red == 0u)
	{
		delete_current_object();
		return;
	}
	OBJECT(current_object)->ending.cycle_count++;
	if (OBJECT(current_object)->ending.cycle_count >= 3)
	{
		color_offset_2_red--;
		color_offset_2_blue--;
		color_offset_2_green--;
		OBJECT(current_object)->ending.cycle_count = 0;
	}
}

void ending_credits_spawner_run(void)
{
	if (OBJECT(current_object)->ending.started == 0u)
	{
		OBJECT(current_object)->ending.started = 1u;
		OBJECT(current_object)->ending.timer = 0;
		OBJECT(current_object)->ending.list_index = 0u;
	}
	OBJECT(current_object)->ending.timer++;
	run_timed_spawns_3();
}

uint32_t load_credits_palettes(void)
{
	uint32_t dest = 0, ignored = 0, palette_list = 0;

	mem_write32(video_control, mem_read32(video_control) & ~0x400u);
	mem_write16(video_control, mem_read16(video_control) | 0x400u);
	vblank_copies_pending = 0u;
	vblank_copy_count = 0u;
	palette_list = tile_bank_1_palette_list_ADDRESS;
	load_palette_list(palette_list, &ignored, &dest);
	return dest;
}

void ending_music_play(void)
{
	uint8_t sound_id = 0;
	uint32_t cue_record = 0, ignored = 0;

	if (OBJECT(current_object)->ending.started == 0u)
	{
		OBJECT(current_object)->ending.started = 1u;
		OBJECT(current_object)->ending.timer = 0;
		sound_id = 0x94u;
		queue_sound(sound_id, &ignored, &ignored);
		sound_id = 0x82u;
		queue_sound(sound_id, &ignored, &ignored);
		OBJECT(current_object)->sprite_offset_x = 0;
		OBJECT(current_object)->sprite_offset_y = 0;
		OBJECT(current_object)->ending.list_index = 0u;
	}
	cue_record = ending_music_cues_ADDRESS + OBJECT(current_object)->ending.list_index * 4u;
	if (ROM16(cue_record) == 0xFFFFu)
	{
		delete_current_object();
		return;
	}
	if ((int16_t)ROM16(cue_record) <= OBJECT(current_object)->ending.timer)
	{
		sound_id = (uint8_t)ROM16(cue_record + 2u);
		queue_sound(sound_id, &ignored, &ignored);
		OBJECT(current_object)->ending.list_index++;
	}
	OBJECT(current_object)->ending.timer++;
}

void ending_portrait_show(void)
{
	uint16_t speed_y = 0;
	uint32_t backdrop_object = 0, ignored = 0, palette_record = 0, timed_record = 0;

	if (OBJECT(current_object)->ending.started == 0u)
	{
		OBJECT(current_object)->ending.started = 1u;
		OBJECT(current_object)->ending.timer = 0;
		allocate_object_2(&ignored, &ignored, &backdrop_object);
		OBJECT(current_object)->speed_y = backdrop_object;
	}
	OBJECT(current_object)->ending.timer++;
	timed_record = ending_portrait_frames_ADDRESS;
	palette_record = ending_portrait_palettes_ADDRESS;
	while ((int16_t)ROM16(timed_record) < OBJECT(current_object)->ending.timer)
	{
		timed_record += 4u;
		palette_record += 4u;
		if (ROM16(timed_record) == 0xFFFFu)
		{
			speed_y = (uint16_t)OBJECT(current_object)->speed_y;
			free_object_2(speed_y);
			delete_current_object();
			return;
		}
	}
	OBJECT(current_object)->sprite_frame = ROM16(timed_record + 2u);
	OBJECT(current_object)->sprite_palette = ROM16(palette_record + 2u);
	OBJECT(current_object)->screen_x = 0xE8;
	OBJECT(current_object)->screen_y = 0x4C;
	OBJECT(current_object)->sprite_layer = 0u;
	OBJECT(current_object)->sprite_scale = 0x100u;
	OBJECT(current_object)->sprite_flags = 0u;
	queue_sprite_in_layer();
	timed_record = ending_portrait_backdrop_palettes_ADDRESS;
	while ((int16_t)ROM16(timed_record) < OBJECT(current_object)->ending.timer)
	{
		timed_record += 4u;
		if (ROM16(timed_record) == 0xFFFFu)
			return;
	}
	current_object = OBJECT(current_object)->speed_y;
	OBJECT(current_object)->sprite_frame = 0x9880u;
	OBJECT(current_object)->sprite_palette = ROM16(timed_record + 2u);
	OBJECT(current_object)->screen_x = 0xE8;
	OBJECT(current_object)->screen_y = 0x4C;
	OBJECT(current_object)->sprite_layer = 0u;
	OBJECT(current_object)->sprite_scale = 0x100u;
	OBJECT(current_object)->sprite_flags = 0u;
	queue_sprite_in_layer();
}

void ending_name_plate_show(void)
{
	uint32_t frame_record = 0, ignored = 0, object = 0;

	if (OBJECT(current_object)->ending.started == 0u)
	{
		OBJECT(current_object)->ending.started = 1u;
		OBJECT(current_object)->ending.timer = 0;
		mem_write16(video_clip_1_left, 0xB1u);
		mem_write16(video_clip_1_right, 0x120u);
		mem_write16(video_clip_1_top, 0x21u);
		mem_write16(video_clip_1_bottom, 0x78u);
		allocate_object_2(&ignored, &ignored, &object);
		OBJECT(current_object)->z = object;
	}
	OBJECT(current_object)->ending.timer++;
	frame_record = ending_name_plate_frames_ADDRESS;
	while ((int16_t)ROM16(frame_record) < OBJECT(current_object)->ending.timer)
	{
		frame_record += 4u;
		if (ROM16(frame_record) == 0xFFFFu)
		{
			allocate_object_outside_loop(&ignored, &object);
			OBJECT(object)->handler = ending_window_open_ADDRESS;
			OBJECT(object)->ending.started = 0u;
			delete_current_object();
			return;
		}
	}
	nbg0_scroll_x_int++;
	OBJECT(current_object)->sprite_frame = ROM16(frame_record + 2u);
	OBJECT(current_object)->screen_x = 0xE8;
	OBJECT(current_object)->screen_y = 0xA3;
	OBJECT(current_object)->sprite_layer = 0u;
	OBJECT(current_object)->sprite_scale = 0x100u;
	OBJECT(current_object)->sprite_flags = 0u;
	OBJECT(current_object)->sprite_palette = 0x1020u;
	queue_sprite_in_layer();
	current_object = OBJECT(current_object)->z;
	OBJECT(current_object)->sprite_frame = 0x9800u;
	OBJECT(current_object)->sprite_palette = 0x1030u;
	OBJECT(current_object)->screen_x = 0xE9;
	OBJECT(current_object)->screen_y = 0x18;
	OBJECT(current_object)->sprite_layer = 0u;
	OBJECT(current_object)->sprite_scale = 0x100u;
	OBJECT(current_object)->sprite_flags = 0u;
	queue_sprite_in_layer();
}

void ending_credits_team_logo_scroll(void)
{
	uint16_t speed_y = 0;
	uint32_t field_cursor = 0, ignored = 0, object = 0;

	if (OBJECT(current_object)->ending.started == 0u)
	{
		OBJECT(current_object)->ending.started = 1u;
		OBJECT(current_object)->ending.timer = 0;
		OBJECT(current_object)->ending.step = 0u;
		OBJECT(current_object)->screen_y = 0xF4;
		OBJECT(current_object)->sprite_frame = 0x9850u;
		OBJECT(current_object)->sprite_palette = 0x1010u;
		init_credits_sprite();
		OBJECT(current_object)->speed_x = 0x1CC3D;
		for (;;)
		{
			field_cursor = OBJECT(object_cursor)->speed_x;
			if (mem_read16(field_cursor) == 0xFFFFu)
				break;
			allocate_object_2(&ignored, &ignored, &object);
			OBJECT(object)->sprite_frame = mem_read16(field_cursor);
			field_cursor += 2u;
			OBJECT(object)->screen_y = mem_read16(field_cursor);
			field_cursor += 2u;
			OBJECT(object)->sprite_palette = mem_read16(field_cursor);
			field_cursor += 2u;
			current_object = (current_object & ~0xFFFFu) | (uint16_t)(current_object + mem_read16(field_cursor));
			OBJECT(object_cursor)->speed_x = field_cursor + 2u;
			AS32(OBJECT(current_object)->draw_next) = object;
			current_object = object;
			init_credits_sprite();
		}
	}
	if (OBJECT(current_object)->ending.step != 0u)
	{
		speed_y = (uint16_t)OBJECT(object_cursor)->speed_y;
		free_object_2(speed_y);
		delete_current_object();
		return;
	}
	OBJECT(object_cursor)->ending.timer++;
	scroll_credits_sprite();
	current_object = OBJECT(current_object)->speed_y;
	scroll_credits_sprite();
	wrap_credits_scroll_timer();
	current_object = OBJECT(current_object)->speed_y;
	if (OBJECT(current_object)->screen_y <= -0x14)
	{
		OBJECT(object_cursor)->ending.step = 1u;
		return;
	}
}

void ending_credits_director_scroll(void)
{
	uint32_t field_cursor = 0, ignored = 0, object = 0;

	if (OBJECT(current_object)->ending.started != 0u)
	{
		scroll_credits_block();
		return;
	}
	OBJECT(current_object)->ending.started = 1u;
	OBJECT(current_object)->ending.timer = 0;
	OBJECT(current_object)->ending.step = 0u;
	OBJECT(current_object)->screen_y = 0xF4;
	OBJECT(current_object)->sprite_frame = 0x97C8u;
	OBJECT(current_object)->sprite_palette = 0x1010u;
	init_credits_sprite();
	OBJECT(current_object)->speed_x = 0x1CD01;
	for (;;)
	{
		field_cursor = OBJECT(object_cursor)->speed_x;
		if (mem_read16(field_cursor) == 0xFFFFu)
			break;
		allocate_object_2(&ignored, &ignored, &object);
		OBJECT(object)->sprite_frame = mem_read16(field_cursor);
		field_cursor += 2u;
		OBJECT(object)->screen_y = mem_read16(field_cursor);
		field_cursor += 2u;
		OBJECT(object)->sprite_palette = mem_read16(field_cursor);
		field_cursor += 2u;
		current_object = (current_object & ~0xFFFFu) | (uint16_t)(current_object + mem_read16(field_cursor));
		OBJECT(object_cursor)->speed_x = field_cursor + 2u;
		AS32(OBJECT(current_object)->draw_next) = object;
		current_object = object;
		init_credits_sprite();
	}
	scroll_credits_block();
}

void scroll_credits_block(void)
{
	uint16_t speed_y = 0, z = 0;

	if (OBJECT(current_object)->scroll_credits.scrolled_out != 0u)
	{
		speed_y = (uint16_t)OBJECT(object_cursor)->speed_y;
		free_object_2(speed_y);
		z = (uint16_t)OBJECT(object_cursor)->z;
		free_object_2(z);
		delete_current_object();
		return;
	}
	OBJECT(object_cursor)->scroll_credits.timer++;
	scroll_credits_sprite();
	current_object = OBJECT(current_object)->speed_y;
	scroll_credits_sprite();
	current_object = OBJECT(current_object)->z;
	scroll_credits_sprite();
	wrap_credits_scroll_timer();
	current_object = OBJECT(current_object)->z;
	if (OBJECT(current_object)->screen_y <= -0x14)
	{
		OBJECT(object_cursor)->scroll_credits.scrolled_out = 1u;
		return;
	}
}

void ending_credits_program_scroll(void)
{
	uint16_t animation = 0, part_3 = 0, speed_y = 0, z = 0;
	uint32_t field_cursor = 0, ignored = 0, object = 0;

	if (OBJECT(current_object)->ending.started != 0u)
	{
		if (OBJECT(current_object)->ending.step == 0u)
		{
			OBJECT(object_cursor)->ending.timer++;
			scroll_credits_sprite();
			current_object = OBJECT(current_object)->speed_y;
			scroll_credits_sprite();
			current_object = OBJECT(current_object)->z;
			scroll_credits_sprite();
			current_object = OBJECT(current_object)->ending.part_3;
			scroll_credits_sprite();
			current_object = OBJECT(current_object)->animation;
			scroll_credits_sprite();
			wrap_credits_scroll_timer();
			current_object = OBJECT(current_object)->animation;
			if (OBJECT(current_object)->screen_y <= -0x14)
			{
				OBJECT(object_cursor)->ending.step = 1u;
				return;
			}
			return;
		}
		speed_y = (uint16_t)OBJECT(object_cursor)->speed_y;
		free_object_2(speed_y);
		z = (uint16_t)OBJECT(object_cursor)->z;
		free_object_2(z);
		part_3 = (uint16_t)OBJECT(object_cursor)->ending.part_3;
		free_object_2(part_3);
		animation = (uint16_t)OBJECT(object_cursor)->animation;
		free_object_2(animation);
		delete_current_object();
		return;
	}
	OBJECT(current_object)->ending.started = 1u;
	OBJECT(current_object)->ending.step = 0u;
	OBJECT(current_object)->ending.timer = 0;
	OBJECT(current_object)->screen_y = 0xF4;
	OBJECT(current_object)->sprite_frame = 0x97C0u;
	OBJECT(current_object)->sprite_palette = 0x1010u;
	init_credits_sprite();
	OBJECT(current_object)->speed_x = 0x1CDEF;
	for (;;)
	{
		field_cursor = OBJECT(object_cursor)->speed_x;
		if (mem_read16(field_cursor) == 0xFFFFu)
			break;
		allocate_object_2(&ignored, &ignored, &object);
		OBJECT(object)->sprite_frame = mem_read16(field_cursor);
		field_cursor += 2u;
		OBJECT(object)->screen_y = mem_read16(field_cursor);
		field_cursor += 2u;
		OBJECT(object)->sprite_palette = mem_read16(field_cursor);
		field_cursor += 2u;
		current_object = (current_object & ~0xFFFFu) | (uint16_t)(current_object + mem_read16(field_cursor));
		OBJECT(object_cursor)->speed_x = field_cursor + 2u;
		AS32(OBJECT(current_object)->draw_next) = object;
		current_object = object;
		init_credits_sprite();
	}
	scroll_credits_block();
}

void ending_credits_design_scroll(void)
{
	uint16_t animation = 0, part_3 = 0, speed_y = 0, x = 0, z = 0;
	uint32_t field_cursor = 0, ignored = 0, object = 0;

	if (OBJECT(current_object)->ending.started != 0u)
	{
		if (OBJECT(current_object)->ending.step == 0u)
		{
			OBJECT(object_cursor)->ending.timer++;
			scroll_credits_sprite();
			current_object = OBJECT(current_object)->speed_y;
			scroll_credits_sprite();
			current_object = OBJECT(current_object)->z;
			scroll_credits_sprite();
			current_object = OBJECT(current_object)->ending.part_3;
			scroll_credits_sprite();
			current_object = OBJECT(current_object)->animation;
			scroll_credits_sprite();
			current_object = OBJECT(current_object)->x;
			scroll_credits_sprite();
			wrap_credits_scroll_timer();
			current_object = OBJECT(current_object)->x;
			if (OBJECT(current_object)->screen_y <= -0x14)
			{
				OBJECT(object_cursor)->ending.step = 1u;
				return;
			}
			return;
		}
		speed_y = (uint16_t)OBJECT(object_cursor)->speed_y;
		free_object_2(speed_y);
		z = (uint16_t)OBJECT(object_cursor)->z;
		free_object_2(z);
		part_3 = (uint16_t)OBJECT(object_cursor)->ending.part_3;
		free_object_2(part_3);
		animation = (uint16_t)OBJECT(object_cursor)->animation;
		free_object_2(animation);
		x = (uint16_t)OBJECT(object_cursor)->x;
		free_object_2(x);
		delete_current_object();
		return;
	}
	OBJECT(current_object)->ending.started = 1u;
	OBJECT(current_object)->ending.step = 0u;
	OBJECT(current_object)->ending.timer = 0;
	OBJECT(current_object)->screen_y = 0xF4;
	OBJECT(current_object)->sprite_frame = 0x97E8u;
	OBJECT(current_object)->sprite_palette = 0x1010u;
	init_credits_sprite();
	OBJECT(current_object)->speed_x = 0x1CEFE;
	for (;;)
	{
		field_cursor = OBJECT(object_cursor)->speed_x;
		if (mem_read16(field_cursor) == 0xFFFFu)
			break;
		allocate_object_2(&ignored, &ignored, &object);
		OBJECT(object)->sprite_frame = mem_read16(field_cursor);
		field_cursor += 2u;
		OBJECT(object)->screen_y = mem_read16(field_cursor);
		field_cursor += 2u;
		OBJECT(object)->sprite_palette = mem_read16(field_cursor);
		field_cursor += 2u;
		current_object = (current_object & ~0xFFFFu) | (uint16_t)(current_object + mem_read16(field_cursor));
		OBJECT(object_cursor)->speed_x = field_cursor + 2u;
		AS32(OBJECT(current_object)->draw_next) = object;
		current_object = object;
		init_credits_sprite();
	}
	scroll_credits_block();
}

void ending_credits_sound_scroll(void)
{
	uint16_t animation = 0, part_3 = 0, speed_y = 0, z = 0;
	uint32_t field_cursor = 0, ignored = 0, object = 0;

	if (OBJECT(current_object)->ending.started != 0u)
	{
		if (OBJECT(current_object)->ending.step == 0u)
		{
			OBJECT(object_cursor)->ending.timer++;
			scroll_credits_sprite();
			current_object = OBJECT(current_object)->speed_y;
			scroll_credits_sprite();
			current_object = OBJECT(current_object)->z;
			scroll_credits_sprite();
			current_object = OBJECT(current_object)->ending.part_3;
			scroll_credits_sprite();
			current_object = OBJECT(current_object)->animation;
			scroll_credits_sprite();
			wrap_credits_scroll_timer();
			current_object = OBJECT(current_object)->animation;
			if (OBJECT(current_object)->screen_y <= -0x14)
			{
				OBJECT(object_cursor)->ending.step = 1u;
				return;
			}
			return;
		}
		speed_y = (uint16_t)OBJECT(object_cursor)->speed_y;
		free_object_2(speed_y);
		z = (uint16_t)OBJECT(object_cursor)->z;
		free_object_2(z);
		part_3 = (uint16_t)OBJECT(object_cursor)->ending.part_3;
		free_object_2(part_3);
		animation = (uint16_t)OBJECT(object_cursor)->animation;
		free_object_2(animation);
		delete_current_object();
		return;
	}
	OBJECT(current_object)->ending.started = 1u;
	OBJECT(current_object)->ending.step = 0u;
	OBJECT(current_object)->ending.timer = 0;
	OBJECT(current_object)->screen_y = 0xF4;
	OBJECT(current_object)->sprite_frame = 0x97E0u;
	OBJECT(current_object)->sprite_palette = 0x1010u;
	init_credits_sprite();
	OBJECT(current_object)->speed_x = 0x1D004;
	for (;;)
	{
		field_cursor = OBJECT(object_cursor)->speed_x;
		if (mem_read16(field_cursor) == 0xFFFFu)
			break;
		allocate_object_2(&ignored, &ignored, &object);
		OBJECT(object)->sprite_frame = mem_read16(field_cursor);
		field_cursor += 2u;
		OBJECT(object)->screen_y = mem_read16(field_cursor);
		field_cursor += 2u;
		OBJECT(object)->sprite_palette = mem_read16(field_cursor);
		field_cursor += 2u;
		current_object = (current_object & ~0xFFFFu) | (uint16_t)(current_object + mem_read16(field_cursor));
		OBJECT(object_cursor)->speed_x = field_cursor + 2u;
		AS32(OBJECT(current_object)->draw_next) = object;
		current_object = object;
		init_credits_sprite();
	}
	scroll_credits_block();
}

void ending_credits_voice_actor_scroll(void)
{
	uint16_t animation = 0, part_3 = 0, speed_y = 0, x = 0, z = 0;
	uint32_t field_cursor = 0, ignored = 0, object = 0;

	if (OBJECT(current_object)->ending.started != 0u)
	{
		if (OBJECT(current_object)->ending.step == 0u)
		{
			OBJECT(object_cursor)->ending.timer++;
			scroll_credits_sprite();
			current_object = OBJECT(current_object)->speed_y;
			scroll_credits_sprite();
			current_object = OBJECT(current_object)->z;
			scroll_credits_sprite();
			current_object = OBJECT(current_object)->ending.part_3;
			scroll_credits_sprite();
			current_object = OBJECT(current_object)->animation;
			scroll_credits_sprite();
			current_object = OBJECT(current_object)->x;
			scroll_credits_sprite();
			wrap_credits_scroll_timer();
			current_object = OBJECT(current_object)->x;
			if (OBJECT(current_object)->screen_y <= -0x14)
			{
				OBJECT(object_cursor)->ending.step = 1u;
				return;
			}
			return;
		}
		speed_y = (uint16_t)OBJECT(object_cursor)->speed_y;
		free_object_2(speed_y);
		z = (uint16_t)OBJECT(object_cursor)->z;
		free_object_2(z);
		part_3 = (uint16_t)OBJECT(object_cursor)->ending.part_3;
		free_object_2(part_3);
		animation = (uint16_t)OBJECT(object_cursor)->animation;
		free_object_2(animation);
		x = (uint16_t)OBJECT(object_cursor)->x;
		free_object_2(x);
		delete_current_object();
		return;
	}
	OBJECT(current_object)->ending.started = 1u;
	OBJECT(current_object)->ending.step = 0u;
	OBJECT(current_object)->ending.timer = 0;
	OBJECT(current_object)->screen_y = 0xF4;
	OBJECT(current_object)->sprite_frame = 0x97F0u;
	OBJECT(current_object)->sprite_palette = 0x1010u;
	init_credits_sprite();
	OBJECT(current_object)->speed_x = 0x1D113;
	for (;;)
	{
		field_cursor = OBJECT(object_cursor)->speed_x;
		if (mem_read16(field_cursor) == 0xFFFFu)
			break;
		allocate_object_2(&ignored, &ignored, &object);
		OBJECT(object)->sprite_frame = mem_read16(field_cursor);
		field_cursor += 2u;
		OBJECT(object)->screen_y = mem_read16(field_cursor);
		field_cursor += 2u;
		OBJECT(object)->sprite_palette = mem_read16(field_cursor);
		field_cursor += 2u;
		current_object = (current_object & ~0xFFFFu) | (uint16_t)(current_object + mem_read16(field_cursor));
		OBJECT(object_cursor)->speed_x = field_cursor + 2u;
		AS32(OBJECT(current_object)->draw_next) = object;
		current_object = object;
		init_credits_sprite();
	}
	scroll_credits_block();
}

void ending_credits_special_thanks_scroll(void)
{
	uint16_t animation = 0, floor_z = 0, part_3 = 0, previous_floor_z = 0, speed_y = 0, x = 0, y = 0, z = 0;
	uint32_t field_cursor = 0, ignored = 0, object = 0;

	if (OBJECT(current_object)->ending.started != 0u)
	{
		if (OBJECT(current_object)->ending.step == 0u)
		{
			OBJECT(object_cursor)->ending.timer++;
			scroll_credits_sprite();
			current_object = OBJECT(current_object)->speed_y;
			scroll_credits_sprite();
			current_object = OBJECT(current_object)->z;
			scroll_credits_sprite();
			current_object = OBJECT(current_object)->ending.part_3;
			scroll_credits_sprite();
			current_object = OBJECT(current_object)->animation;
			scroll_credits_sprite();
			current_object = OBJECT(current_object)->x;
			scroll_credits_sprite();
			current_object = OBJECT(current_object)->y;
			scroll_credits_sprite();
			current_object = OBJECT(current_object)->floor_z;
			scroll_credits_sprite();
			current_object = OBJECT(current_object)->previous_floor_z;
			scroll_credits_sprite();
			wrap_credits_scroll_timer();
			current_object = OBJECT(current_object)->previous_floor_z;
			if (OBJECT(current_object)->screen_y <= -0x14)
			{
				OBJECT(object_cursor)->ending.step = 1u;
				return;
			}
			return;
		}
		speed_y = (uint16_t)OBJECT(object_cursor)->speed_y;
		free_object_2(speed_y);
		z = (uint16_t)OBJECT(object_cursor)->z;
		free_object_2(z);
		part_3 = (uint16_t)OBJECT(object_cursor)->ending.part_3;
		free_object_2(part_3);
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
		delete_current_object();
		return;
	}
	OBJECT(current_object)->ending.started = 1u;
	OBJECT(current_object)->ending.step = 0u;
	OBJECT(current_object)->ending.timer = 0;
	OBJECT(current_object)->screen_y = 0xF4;
	OBJECT(current_object)->sprite_frame = 0x97D0u;
	OBJECT(current_object)->sprite_palette = 0x1010u;
	init_credits_sprite();
	OBJECT(current_object)->speed_x = 0x1D25D;
	for (;;)
	{
		field_cursor = OBJECT(object_cursor)->speed_x;
		if (mem_read16(field_cursor) == 0xFFFFu)
			break;
		allocate_object_2(&ignored, &ignored, &object);
		OBJECT(object)->sprite_frame = mem_read16(field_cursor);
		field_cursor += 2u;
		OBJECT(object)->screen_y = mem_read16(field_cursor);
		field_cursor += 2u;
		OBJECT(object)->sprite_palette = mem_read16(field_cursor);
		field_cursor += 2u;
		current_object = (current_object & ~0xFFFFu) | (uint16_t)(current_object + mem_read16(field_cursor));
		OBJECT(object_cursor)->speed_x = field_cursor + 2u;
		AS32(OBJECT(current_object)->draw_next) = object;
		current_object = object;
		init_credits_sprite();
	}
	scroll_credits_block();
}

void scroll_credits_sprite(void)
{
	if (OBJECT(object_cursor)->scroll_credits.timer >= 2)
		OBJECT(current_object)->screen_y--;
	queue_sprite_in_layer();
	current_object = object_cursor;
}

void wrap_credits_scroll_timer(void)
{
	if ((int16_t)AT16(OBJECT(object_cursor)->flags, 2) >= 2)
		AT16(OBJECT(object_cursor)->flags, 2) = 0u;
}

void init_credits_sprite(void)
{
	OBJECT(current_object)->screen_x = 0x58;
	OBJECT(current_object)->sprite_layer = 0u;
	OBJECT(current_object)->sprite_flags = 0u;
	OBJECT(current_object)->sprite_scale = 0x100u;
	current_object = object_cursor;
}

void run_timed_spawns_3(void)
{
	uint32_t ignored = 0, object = 0, spawn_record = 0;

	spawn_record = timed_spawns_3_ADDRESS + OBJECT(current_object)->run_timed_spawns.list_index * 4u;
	OBJECT(current_object)->z = ROM16(spawn_record + 2u);
	if (OBJECT(current_object)->z_fraction == 0xFFFFu)
	{
		delete_current_object();
		return;
	}
	if ((int16_t)ROM16(spawn_record) <= OBJECT(current_object)->run_timed_spawns.timer)
	{
		allocate_object_outside_loop(&ignored, &object);
		OBJECT(current_object)->z += 0x1C82Fu;
		OBJECT(object)->handler = OBJECT(current_object)->z;
		AS8(OBJECT(object)->flags) = 0u;
		OBJECT(current_object)->run_timed_spawns.list_index++;
	}
}

void animate_ending_runner(uint32_t frame_entry)
{
	if (ending_first_scene_done == 1u)
	{
		delete_current_object();
		return;
	}
	OBJECT(current_object)->sprite_frame = mem_read16(frame_entry);
	OBJECT(current_object)->sprite_flags = 0x100u;
	queue_sprite_in_layer();
	OBJECT(current_object)->animate_ending.timer++;
	if ((OBJECT(current_object)->animate_ending.timer & 3u) == 2u)
		OBJECT(current_object)->sprite_scale++;
	if ((OBJECT(current_object)->animate_ending.timer & 1u) != 0u)
	{
		OBJECT(current_object)->animate_ending.frame_index++;
		if (OBJECT(current_object)->animate_ending.frame_index > 0xB)
			OBJECT(current_object)->animate_ending.frame_index = 0;
	}
}

void ending_window_open(void)
{
	uint32_t ignored = 0, object = 0;

	if (OBJECT(current_object)->ending.started == 0u)
	{
		OBJECT(current_object)->ending.started = 1u;
		OBJECT(current_object)->ending.step = 0u;
	}
	nbg0_scroll_x_int++;
	mem_write16(video_clip_1_left, mem_read16(video_clip_1_left) - 1u);
	mem_write16(video_clip_1_right, mem_read16(video_clip_1_right) + 1u);
	mem_write16(video_clip_1_top, mem_read16(video_clip_1_top) - 1u);
	mem_write16(video_clip_1_bottom, mem_read16(video_clip_1_bottom) + 1u);
	if ((int16_t)mem_read16(video_clip_1_left) <= 0)
	{
		mem_write16(video_clip_1_left, 0u);
		OBJECT(current_object)->ending.step |= 1u;
	}
	if ((int16_t)mem_read16(video_clip_1_top) <= 0)
	{
		mem_write16(video_clip_1_top, 0u);
		OBJECT(current_object)->ending.step |= 2u;
	}
	if ((int16_t)mem_read16(video_clip_1_right) >= 0x140)
	{
		mem_write16(video_clip_1_right, 0x140u);
		OBJECT(current_object)->ending.step |= 4u;
	}
	if ((int16_t)mem_read16(video_clip_1_bottom) >= 0xE0)
	{
		mem_write16(video_clip_1_bottom, 0xE0u);
		OBJECT(current_object)->ending.step |= 8u;
	}
	if (OBJECT(current_object)->ending.step != 0xFu)
		return;
	allocate_object_outside_loop(&ignored, &object);
	OBJECT(object)->ending.started = 0u;
	OBJECT(object)->handler = ending_credits_fade_out_ADDRESS;
	delete_current_object();
}

void ending_credits_fade_out(void)
{
	uint32_t ignored = 0, object = 0;

	if (OBJECT(current_object)->ending.started == 0u)
	{
		fade_step = 0u;
		fade_kind = 0;
		OBJECT(current_object)->ending.started = 1u;
	}
	update_fade(0u);
	color_offset_2_red = color_offset_red;
	color_offset_2_blue = color_offset_blue;
	color_offset_2_green = color_offset_green;
	if (fade_kind >= 0)
		return;
	allocate_object_outside_loop(&ignored, &object);
	OBJECT(object)->ending.started = 0u;
	OBJECT(object)->handler = ending_eggman_scene_fade_in_ADDRESS;
	delete_current_object();
}

void ending_eggman_scene_fade_in(void)
{
	uint32_t r1 = 0, ignored = 0, object = 0, script = 0;

	if (OBJECT(current_object)->ending.started != 0u)
	{
		update_fade(0u);
		color_offset_2_red = color_offset_red;
		color_offset_2_blue = color_offset_blue;
		color_offset_2_green = color_offset_green;
		if (fade_kind >= 0)
			return;
		allocate_object_outside_loop(&ignored, &object);
		OBJECT(object)->ending.started = 0u;
		OBJECT(object)->handler = ending_eggman_fly_in_ADDRESS;
		delete_current_object();
		return;
	}
	mem_write16(video_layer_disable, mem_read16(video_layer_disable) | 0xBu);
	script = ending_stripes_script_ADDRESS;
	run_vram_script(&r1, &script, &object);
	nbg0_pages = 0x3030303u;
	nbg1_pages = 0x2020202u;
	nbg2_pages = 0x8080707u;
	nbg3_pages = 0xF0F0F0Fu;
	mem_write16(video_layer_disable, mem_read16(video_layer_disable) & ~0xFu);
	OBJECT(current_object)->ending.started = 1u;
	fade_step = 0u;
	fade_kind = 2;
	nbg0_scroll_x_int = 0u;
}

void ending_eggman_fly_in(void)
{
	uint8_t sound_id = 0;
	uint16_t animation = 0, floor_block = 0, floor_z = 0, part_3 = 0, previous_floor_z = 0, speed_x = 0, speed_y = 0, x = 0, y = 0, z = 0;
	uint32_t animation_2 = 0, animation_3 = 0, floor_block_2 = 0, floor_z_2 = 0, floor_z_3 = 0, ignored = 0, object = 0, part_3_2 = 0, part_3_3 = 0, previous_floor_z_2 = 0, previous_floor_z_3 = 0, x_2 = 0, x_3 = 0, y_2 = 0, y_3 = 0, y_entry = 0, z_2 = 0, z_3 = 0;

	if (OBJECT(current_object)->ending.started == 0u)
	{
		OBJECT(current_object)->ending.started = 1u;
		OBJECT(current_object)->ending.step = 0u;
		OBJECT(current_object)->screen_y = 0x70;
		OBJECT(current_object)->screen_x = -0x20;
		OBJECT(current_object)->speed_y = 0x103B76;
		OBJECT(current_object)->ending.timer = 0x1E0;
		allocate_object_2(&ignored, &ignored, &object);
		OBJECT(current_object)->speed_x = object;
		allocate_object_2(&ignored, &ignored, &object);
		OBJECT(current_object)->z = object;
		init_eggman_smoke_puff(object);
		allocate_object_2(&ignored, &ignored, &object);
		OBJECT(current_object)->ending.part_3 = object;
		init_eggman_smoke_puff(object);
		allocate_object_2(&ignored, &ignored, &object);
		OBJECT(current_object)->animation = object;
		init_eggman_smoke_puff(object);
		allocate_object_2(&ignored, &ignored, &object);
		OBJECT(current_object)->x = object;
		init_eggman_smoke_puff(object);
		allocate_object_2(&ignored, &ignored, &object);
		OBJECT(current_object)->y = object;
		init_eggman_smoke_puff(object);
		OBJECT(object)->sprite_frame = 0x9B80u;
		allocate_object_2(&ignored, &ignored, &object);
		OBJECT(current_object)->floor_z = object;
		init_eggman_smoke_puff(object);
		OBJECT(object)->sprite_frame = 0x9B78u;
		allocate_object_2(&ignored, &ignored, &object);
		OBJECT(current_object)->previous_floor_z = object;
		init_eggman_smoke_puff(object);
		OBJECT(object)->sprite_frame = 0x9B70u;
		allocate_object_2(&ignored, &ignored, &object);
		OBJECT(current_object)->floor_block = object;
		init_eggman_smoke_puff(object);
		OBJECT(object)->sprite_frame = 0x9B68u;
	}
	OBJECT(current_object)->ending.timer--;
	if ((uint16_t)OBJECT(current_object)->ending.timer == 0x104u)
	{
		sound_id = 0x75u;
		queue_sound(sound_id, &ignored, &ignored);
	}
	if ((uint16_t)OBJECT(current_object)->ending.timer != 0u)
	{
		y_entry = OBJECT(current_object)->speed_y;
		OBJECT(current_object)->screen_x = mem_read16(y_entry);
		y_entry += 2u;
		OBJECT(current_object)->screen_y = mem_read16(y_entry);
		OBJECT(current_object)->sprite_layer = 0u;
		OBJECT(current_object)->sprite_frame = 0x9AE0u;
		OBJECT(current_object)->sprite_palette = 0x1250u;
		OBJECT(current_object)->sprite_scale = 0x100u;
		OBJECT(current_object)->sprite_flags = 0u;
		OBJECT(current_object)->speed_y = y_entry + 2u;
		queue_sprite_in_layer();
		if ((uint16_t)OBJECT(current_object)->screen_x != 0xA6u)
		{
			current_object = OBJECT(current_object)->speed_x;
			OBJECT(current_object)->sprite_frame = 0x9AB0u;
			if ((OBJECT(object_cursor)->ending.timer & 2u) != 0u)
				OBJECT(current_object)->sprite_frame = 0x9AB8u;
			OBJECT(current_object)->screen_x = OBJECT(object_cursor)->screen_x;
			OBJECT(current_object)->screen_y = OBJECT(object_cursor)->screen_y;
			OBJECT(current_object)->sprite_layer = 0u;
			OBJECT(current_object)->sprite_scale = 0x100u;
			OBJECT(current_object)->screen_x -= 0x20u;
			OBJECT(current_object)->sprite_flags = 0u;
			OBJECT(current_object)->sprite_palette = 0x10E0u;
			queue_sprite_in_layer();
		}
		else
			OBJECT(current_object)->speed_y += 4u;
		current_object = object_cursor;
		if ((uint16_t)((int16_t)OBJECT(current_object)->ending.timer % 0xA) == 0u)
		{
			previous_floor_z_3 = OBJECT(object_cursor)->previous_floor_z;
			floor_block_2 = OBJECT(object_cursor)->floor_block;
			mem_write16(floor_block_2 + 0xEu, mem_read16(previous_floor_z_3 + 0xEu));
			mem_write16(floor_block_2 + 0xCu, mem_read16(previous_floor_z_3 + 0xCu));
			floor_z_3 = OBJECT(object_cursor)->floor_z;
			previous_floor_z_2 = OBJECT(object_cursor)->previous_floor_z;
			mem_write16(previous_floor_z_2 + 0xEu, mem_read16(floor_z_3 + 0xEu));
			mem_write16(previous_floor_z_2 + 0xCu, mem_read16(floor_z_3 + 0xCu));
			y_3 = OBJECT(object_cursor)->y;
			floor_z_2 = OBJECT(object_cursor)->floor_z;
			mem_write16(floor_z_2 + 0xEu, mem_read16(y_3 + 0xEu));
			mem_write16(floor_z_2 + 0xCu, mem_read16(y_3 + 0xCu));
			x_3 = OBJECT(object_cursor)->x;
			y_2 = OBJECT(object_cursor)->y;
			mem_write16(y_2 + 0xEu, mem_read16(x_3 + 0xEu));
			mem_write16(y_2 + 0xCu, mem_read16(x_3 + 0xCu));
			animation_3 = OBJECT(object_cursor)->animation;
			x_2 = OBJECT(object_cursor)->x;
			mem_write16(x_2 + 0xEu, mem_read16(animation_3 + 0xEu));
			mem_write16(x_2 + 0xCu, mem_read16(animation_3 + 0xCu));
			part_3_3 = OBJECT(object_cursor)->ending.part_3;
			animation_2 = OBJECT(object_cursor)->animation;
			mem_write16(animation_2 + 0xEu, mem_read16(part_3_3 + 0xEu));
			mem_write16(animation_2 + 0xCu, mem_read16(part_3_3 + 0xCu));
			z_3 = OBJECT(object_cursor)->z;
			part_3_2 = OBJECT(object_cursor)->ending.part_3;
			mem_write16(part_3_2 + 0xEu, mem_read16(z_3 + 0xEu));
			mem_write16(part_3_2 + 0xCu, mem_read16(z_3 + 0xCu));
			z_2 = OBJECT(object_cursor)->z;
			mem_write16(z_2 + 0xEu, OBJECT(object_cursor)->screen_x);
			mem_write16(z_2 + 0xCu, OBJECT(object_cursor)->screen_y);
			mem_write16(z_2 + 0xEu, mem_read16(z_2 + 0xEu) - 0x34u);
		}
		if (OBJECT(object_cursor)->ending.timer > 0x32)
		{
			current_object = OBJECT(object_cursor)->z;
			queue_sprite_in_layer();
			current_object = OBJECT(object_cursor)->ending.part_3;
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
		}
		return;
	}
	allocate_object_outside_loop(&ignored, &object);
	OBJECT(object)->ending.started = 0u;
	OBJECT(object)->handler = ending_eggman_turn_ADDRESS;
	OBJECT(object)->sprite_frame = OBJECT(current_object)->sprite_frame;
	OBJECT(object)->screen_x = OBJECT(current_object)->screen_x;
	OBJECT(object)->screen_y = OBJECT(current_object)->screen_y;
	OBJECT(object)->sprite_layer = OBJECT(current_object)->sprite_layer;
	OBJECT(object)->sprite_flags = OBJECT(current_object)->sprite_flags;
	OBJECT(object)->sprite_palette = OBJECT(current_object)->sprite_palette;
	OBJECT(object)->sprite_scale = OBJECT(current_object)->sprite_scale;
	speed_x = (uint16_t)OBJECT(current_object)->speed_x;
	free_object_2(speed_x);
	speed_y = (uint16_t)OBJECT(current_object)->speed_y;
	free_object_2(speed_y);
	z = (uint16_t)OBJECT(current_object)->z;
	free_object_2(z);
	part_3 = (uint16_t)OBJECT(current_object)->ending.part_3;
	free_object_2(part_3);
	animation = (uint16_t)OBJECT(current_object)->animation;
	free_object_2(animation);
	x = (uint16_t)OBJECT(current_object)->x;
	free_object_2(x);
	y = (uint16_t)OBJECT(current_object)->y;
	free_object_2(y);
	floor_z = (uint16_t)OBJECT(current_object)->floor_z;
	free_object_2(floor_z);
	previous_floor_z = (uint16_t)OBJECT(current_object)->previous_floor_z;
	free_object_2(previous_floor_z);
	floor_block = (uint16_t)OBJECT(current_object)->floor_block;
	free_object_2(floor_block);
	delete_current_object();
}

void init_eggman_smoke_puff(uint32_t piece)
{
	OBJECT(piece)->sprite_frame = 0x9B88u;
	OBJECT(piece)->sprite_layer = 0u;
	OBJECT(piece)->screen_x = -0x10;
	OBJECT(piece)->screen_y = 0;
	OBJECT(piece)->sprite_scale = 0x100u;
	OBJECT(piece)->sprite_flags = 0u;
	OBJECT(piece)->sprite_palette = 0x1280u;
}

void ending_eggman_turn(void)
{
	uint32_t frame_record = 0, ignored = 0, object = 0;

	if (OBJECT(current_object)->ending.started == 0u)
	{
		OBJECT(current_object)->ending.started = 1u;
		OBJECT(current_object)->ending.timer = 0;
		OBJECT(current_object)->ending.step = 0u;
	}
	OBJECT(current_object)->ending.timer++;
	frame_record = ending_eggman_turn_frames_ADDRESS + OBJECT(current_object)->ending.step * 4u;
	if (ROM16(frame_record) == 0xFFFFu)
	{
		allocate_object_outside_loop(&ignored, &object);
		OBJECT(object)->ending.started = 0u;
		OBJECT(object)->handler = ending_eggman_scene_fade_out_ADDRESS;
		delete_current_object();
		return;
	}
	if ((int16_t)ROM16(frame_record) < OBJECT(current_object)->ending.timer)
		OBJECT(current_object)->ending.step++;
	OBJECT(current_object)->sprite_frame = ROM16(frame_record + 2u);
	queue_sprite_in_layer();
}

void ending_eggman_scene_fade_out(void)
{
	uint32_t ignored = 0, object = 0;

	if (OBJECT(current_object)->ending.started == 0u)
	{
		fade_step = 0u;
		fade_kind = 0;
		OBJECT(current_object)->ending.started = 1u;
	}
	update_fade(0u);
	color_offset_2_red = color_offset_red;
	color_offset_2_blue = color_offset_blue;
	color_offset_2_green = color_offset_green;
	if (fade_kind >= 0)
		return;
	allocate_object_outside_loop(&ignored, &object);
	OBJECT(object)->ending.started = 0u;
	OBJECT(object)->handler = ending_close_up_fade_in_ADDRESS;
	allocate_object_outside_loop(&ignored, &object);
	OBJECT(object)->ending.started = 0u;
	OBJECT(object)->handler = ending_eggman_fly_away_ADDRESS;
	delete_current_object();
}

void ending_close_up_fade_in(void)
{
	if (OBJECT(current_object)->ending.started == 0u)
	{
		OBJECT(current_object)->ending.started = 1u;
		fade_step = 0u;
		fade_kind = 2;
	}
	update_fade(0u);
	color_offset_2_red = color_offset_red;
	color_offset_2_blue = color_offset_blue;
	color_offset_2_green = color_offset_green;
	if (fade_kind >= 0)
		return;
	delete_current_object();
}

void ending_eggman_fly_away(void)
{
	uint8_t sound_id = 0;
	uint16_t halfwords = 0, line_dy = 0, scale_step = 0, speed_y = 0, z = 0;
	uint32_t r2 = 0, cycle = 0, half_width_squared = 0, ignored = 0, left_entry = 0, lines_left = 0, object = 0, palette_count = 0, right_entry = 0, source = 0;

	if (OBJECT(current_object)->ending.started == 0u)
	{
		OBJECT(current_object)->ending.started = 1u;
		OBJECT(current_object)->ending.step = 0u;
		mem_write16(video_layer_disable, 0x20u);
		mem_write16(video_layer_disable, mem_read16(video_layer_disable) | 4u);
		mixer_nbg1_blend = 0x580u;
		mixer_blend = 0u;
		OBJECT(current_object)->screen_x = 0xA0;
		OBJECT(current_object)->screen_y = 0x70;
		OBJECT(current_object)->sprite_layer = 1u;
		OBJECT(current_object)->sprite_scale = 0x200u;
		OBJECT(current_object)->sprite_flags = 0u;
		OBJECT(current_object)->sprite_frame = 0x9AD8u;
		OBJECT(current_object)->sprite_palette = 0x1230u;
		allocate_object_2(&ignored, &ignored, &object);
		OBJECT(current_object)->speed_y = object;
		OBJECT(object)->screen_x = 0xA0;
		OBJECT(object)->screen_y = 0x70;
		OBJECT(object)->sprite_layer = 0u;
		OBJECT(object)->sprite_scale = 0x200u;
		OBJECT(object)->sprite_flags = 0u;
		OBJECT(object)->sprite_flags += 8u;
		OBJECT(object)->sprite_frame = 0x9AD0u;
		OBJECT(object)->sprite_palette = 0x1240u;
		allocate_object_2(&ignored, &ignored, &object);
		OBJECT(current_object)->z = object;
		OBJECT(object)->screen_x = 0xA0;
		OBJECT(object)->screen_y = 0x70;
		OBJECT(object)->sprite_layer = 0u;
		OBJECT(object)->sprite_scale = 0u;
		OBJECT(object)->sprite_flags = 0u;
		OBJECT(object)->sprite_frame = 0x9B50u;
		OBJECT(object)->sprite_palette = 0x1270u;
		sound_id = 0x72u;
		queue_sound(sound_id, &ignored, &r2);
	}
	object = PALETTE_RAM + 0x8BC0u;
	palette_count = 1u;
	source = ending_eggman_cycle_palettes_ADDRESS;
	halfwords = 0x10u;
	cycle = palette_cycle_0_ADDRESS;
	palette_cycle_0 = 6u;
	palette_cycle_0_frames = 9u;
	cycle_palette(cycle, halfwords, &ignored, &r2, &palette_count, &source, &object);
	if ((int16_t)OBJECT(current_object)->sprite_scale >= 0x20)
	{
		current_object = object_cursor;
		scale_step = 3u;
		if ((int16_t)OBJECT(current_object)->sprite_scale < 0x100)
			scale_step = 2u;
		OBJECT(current_object)->sprite_scale -= scale_step;
		queue_sprite_in_layer();
		current_object = OBJECT(current_object)->speed_y;
		OBJECT(current_object)->sprite_scale -= scale_step;
		queue_sprite_in_layer();
		return;
	}
	current_object = OBJECT(current_object)->z;
	switch (OBJECT(object_cursor)->ending.step)
	{
	case 0:
	case 7:
		sound_id = 9u;
		queue_sound(sound_id, &ignored, &ignored);
		nbg2_pages = 0x2020202u;
		OBJECT(object_cursor)->ending.step = 1u;
		return;
	case 1:
		OBJECT(current_object)->sprite_scale += 5u;
		queue_sprite_in_layer();
		if ((int16_t)OBJECT(current_object)->sprite_scale >= 0x110)
			OBJECT(object_cursor)->ending.step = 2u;
		return;
	case 2:
		OBJECT(current_object)->sprite_scale -= 5u;
		if ((int16_t)OBJECT(current_object)->sprite_scale >= 0)
		{
			queue_sprite_in_layer();
			return;
		}
		OBJECT(object_cursor)->ending.step = 3u;
		return;
	case 3:
		speed_y = (uint16_t)OBJECT(object_cursor)->speed_y;
		free_object_2(speed_y);
		z = (uint16_t)OBJECT(object_cursor)->z;
		free_object_2(z);
		OBJECT(object_cursor)->ending.step = 4u;
		return;
	case 4:
		mem_write16(video_rowscroll_control, 0x5015u);
		mem_write16(video_clip_select, 0x400u);
		mem_write32(nbg2_page_select, 0x2020202u);
		mixer_nbg2_blend = 0x580u;
		mixer_blend = 0u;
		mem_write16(video_layer_disable, 0x2122u);
		nbg2_scroll_x_fraction = 0u;
		nbg2_scroll_y_fraction = 0u;
		OBJECT(object_cursor)->ending.step = 5u;
		AS8(circle_wipe_radius) = 0u;
		AT16(OBJECT(current_object)->previous_floor_block, 2) = 0u;
		fill_value = (fill_value & ~0xFFFFu) | 1u;
		set_halfwords(VIDEO_RAM + 0xA000u, 0x200u, fill_value);
		fill_value = fill_value & ~0xFFFFu;
		set_halfwords(VIDEO_RAM + 0xA400u, 0x200u, fill_value);
		return;
	case 5:
		AT16(OBJECT(current_object)->previous_floor_block, 2)++;
		if ((OBJECT(current_object)->previous_floor_block & 0x10000u) != 0u)
		{
			left_entry = VIDEO_RAM + 0xA0E0u;
			right_entry = VIDEO_RAM + 0xA4E0u;
			circle_wipe_line = 0x70u;
		}
		else
		{
			circle_wipe_radius += 3u;
			left_entry = VIDEO_RAM + 0xA000u;
			right_entry = VIDEO_RAM + 0xA400u;
			circle_wipe_line = 0u;
		}
		lines_left = 0x70u;
		do
		{
			half_width_squared = (uint16_t)circle_wipe_radius;
			line_dy = (uint16_t)(circle_wipe_line - 0x70u);
			half_width_squared = (uint16_t)(half_width_squared * half_width_squared - (uint32_t)line_dy * line_dy);
			if ((int16_t)half_width_squared >= 0)
			{
				square_root(&half_width_squared, &ignored);
				circle_wipe_left = 0xA0u;
				circle_wipe_right = 0xA0u;
				circle_wipe_right += half_width_squared;
				circle_wipe_left -= half_width_squared;
			}
			else
			{
				if ((OBJECT(current_object)->previous_floor_block & 0x10000u) == 0u)
				{
					if ((int16_t)(0x70u - circle_wipe_radius) <= (int16_t)((int32_t)(left_entry - (VIDEO_RAM + 0xA000u)) >> 1))
						goto ending_eggman_fly_away_whole_line;
					goto ending_eggman_fly_away_empty_line;
				}
				if ((int16_t)((int32_t)(left_entry - (VIDEO_RAM + 0xA000u)) >> 1) <= (int16_t)(circle_wipe_radius + 0x70u))
				{
				ending_eggman_fly_away_whole_line:
					circle_wipe_left = 0u;
					circle_wipe_right = 0x13Fu;
				}
				else
				{
				ending_eggman_fly_away_empty_line:
					circle_wipe_left = 1u;
					circle_wipe_right = 0u;
				}
			}
			if ((int16_t)circle_wipe_left <= 0)
				circle_wipe_left = 0u;
			if ((int16_t)circle_wipe_right >= 0x13F)
				circle_wipe_right = 0x13Fu;
			mem_write16(left_entry, circle_wipe_left);
			mem_write16(right_entry, circle_wipe_right);
			circle_wipe_line++;
			lines_left--;
			left_entry += 2u;
			right_entry += 2u;
		} while (lines_left != 0u);
		if (circle_wipe_radius > 0xE0)
			OBJECT(object_cursor)->ending.step = 6u;
		return;
	case 6:
		allocate_object_outside_loop(&ignored, &object);
		OBJECT(object)->handler = ending_sonic_run_across_ADDRESS;
		OBJECT(object)->ending.started = 0u;
		allocate_object_outside_loop(&ignored, &object);
		OBJECT(object)->handler = ending_ray_run_across_ADDRESS;
		OBJECT(object)->ending.started = 0u;
		allocate_object_outside_loop(&ignored, &object);
		OBJECT(object)->handler = ending_mighty_run_across_ADDRESS;
		OBJECT(object)->ending.started = 0u;
		delete_current_object();
		return;
	default:
		unknown_code(0x1DB45u + OBJECT(object_cursor)->ending.step * 3u);
	}
}

void ending_eggman_fly_away_spare_return(void)
{
}

void ending_sonic_run_across(void)
{
	uint32_t ignored = 0, second_object = 0;

	if (OBJECT(current_object)->ending.started != 0u)
	{
		OBJECT(object_cursor)->hit_list_next++;
		if ((int16_t)OBJECT(object_cursor)->hit_list_next >= 0x78)
			ending_finished = 1u;
		animate_ending_runner_across();
		OBJECT(object_cursor)->screen_x += 5u;
		OBJECT(current_object)->screen_x += 5u;
		return;
	}
	OBJECT(current_object)->sprite_layer = 1u;
	OBJECT(current_object)->sprite_palette = 0x1220u;
	OBJECT(current_object)->sprite_frame = 0xA810u;
	OBJECT(current_object)->sprite_scale = 0x100u;
	init_ending_runner_across();
	allocate_object_2(&ignored, &ignored, &second_object);
	OBJECT(current_object)->speed_y = second_object;
	current_object = second_object;
	OBJECT(current_object)->sprite_frame = 0xA820u;
	OBJECT(current_object)->sprite_scale = 0x100u;
	OBJECT(current_object)->sprite_palette = 0x1220u;
	OBJECT(current_object)->sprite_layer = 0u;
	init_ending_runner_across();
	OBJECT(current_object)->screen_y += 0x1Au;
	OBJECT(current_object)->screen_x -= 4u;
	OBJECT(object_cursor)->x = 0x1E054;
	OBJECT(object_cursor)->y = 0x1E058;
	OBJECT(object_cursor)->hit_list_next = 0u;
}

void ending_ray_run_across(void)
{
	uint32_t ignored = 0, second_object = 0;

	if (OBJECT(current_object)->ending.started != 0u)
	{
		animate_ending_runner_across();
		OBJECT(object_cursor)->screen_x += 4u;
		OBJECT(current_object)->screen_x += 4u;
		return;
	}
	OBJECT(current_object)->sprite_layer = 3u;
	OBJECT(current_object)->sprite_palette = 0x1210u;
	OBJECT(current_object)->sprite_frame = 0xA7E0u;
	OBJECT(current_object)->sprite_scale = 0xF0u;
	init_ending_runner_across();
	allocate_object_2(&ignored, &ignored, &second_object);
	OBJECT(current_object)->speed_y = second_object;
	current_object = second_object;
	OBJECT(current_object)->sprite_frame = 0xA7F0u;
	OBJECT(current_object)->sprite_scale = 0xF0u;
	OBJECT(current_object)->sprite_palette = 0x1210u;
	OBJECT(current_object)->sprite_layer = 2u;
	init_ending_runner_across();
	OBJECT(current_object)->screen_y += 0x1Au;
	OBJECT(current_object)->screen_x -= 4u;
	OBJECT(object_cursor)->x = 0x1E060;
	OBJECT(object_cursor)->y = 0x1E064;
}

void ending_mighty_run_across(void)
{
	uint32_t ignored = 0, second_object = 0;

	if (OBJECT(current_object)->ending.started != 0u)
	{
		animate_ending_runner_across();
		OBJECT(object_cursor)->screen_x += 3u;
		OBJECT(current_object)->screen_x += 3u;
		return;
	}
	OBJECT(current_object)->sprite_layer = 4u;
	OBJECT(current_object)->sprite_palette = 0x1200u;
	OBJECT(current_object)->sprite_frame = 0xA7B0u;
	OBJECT(current_object)->sprite_scale = 0xE0u;
	init_ending_runner_across();
	allocate_object_2(&ignored, &ignored, &second_object);
	OBJECT(current_object)->speed_y = second_object;
	current_object = second_object;
	OBJECT(current_object)->sprite_frame = 0xA7C0u;
	OBJECT(current_object)->sprite_scale = 0xE0u;
	OBJECT(current_object)->sprite_palette = 0x1200u;
	OBJECT(current_object)->sprite_layer = 3u;
	init_ending_runner_across();
	OBJECT(current_object)->screen_y += 0x1Au;
	OBJECT(current_object)->screen_x -= 4u;
	OBJECT(object_cursor)->x = 0x1E06C;
	OBJECT(object_cursor)->y = 0x1E070;
}

void init_ending_runner_across(void)
{
	AS8(OBJECT(object_cursor)->flags) = 1u;
	AT16(OBJECT(object_cursor)->flags, 2) = 0u;
	AT16(OBJECT(object_cursor)->previous_floor_block, 2) = 0u;
	OBJECT(current_object)->screen_y = 0x68;
	OBJECT(current_object)->screen_x = 0;
	OBJECT(current_object)->sprite_palette |= 0x2000u;
	OBJECT(current_object)->sprite_flags = 0u;
	OBJECT(current_object)->sprite_flags |= 0x140u;
}

void animate_ending_runner_across(void)
{
	uint32_t second_frame_record = 0;

	OBJECT(object_cursor)->animate_ending.timer++;
	OBJECT(object_cursor)->animate_ending.cycle_count++;
	OBJECT(object_cursor)->animate_ending.timer &= 7u;
	OBJECT(object_cursor)->animate_ending.cycle_count &= 3u;
	second_frame_record = OBJECT(object_cursor)->y + OBJECT(object_cursor)->animate_ending.cycle_count * 2u;
	OBJECT(current_object)->sprite_frame = mem_read16(OBJECT(object_cursor)->x + ((uint16_t)OBJECT(object_cursor)->animate_ending.timer >> 2) * 2u);
	queue_sprite_in_layer();
	current_object = OBJECT(object_cursor)->speed_y;
	OBJECT(current_object)->sprite_frame = mem_read16(second_frame_record);
	queue_sprite_in_layer();
	if (OBJECT(object_cursor)->screen_x >= 0x300)
	{
		OBJECT(current_object)->screen_x = 0;
		OBJECT(object_cursor)->screen_x = 0;
	}
}

void spawn_ring_rows(uint32_t *p_stage_list_offset, uint32_t *p_free_stack, uint32_t *p_slope_height, uint32_t *p_object)
{
	uint32_t stage_list_offset = *p_stage_list_offset;
	uint32_t free_stack = *p_free_stack;
	uint32_t slope_height = *p_slope_height;
	uint32_t object = *p_object;
	uint16_t scroll_past_row = 0;
	uint32_t r0 = 0, ignored = 0, row_cursor = 0, template_cursor = 0;
	uint16_t rings_left = 0;
	uint32_t old_init_flags = 0;

	old_init_flags = stage_init_flags;
	stage_init_flags = old_init_flags | 4u;
	if ((old_init_flags & 4u) == 0u)
	{
		stage_list_offset = stage_template_spawns[stage];
		ring_spawn_cursor = stage_template_spawns_ADDRESS + stage_list_offset;
	}
	row_cursor = ring_spawn_cursor;
	if (scroll_position < ROM16(row_cursor))
	{
		*p_stage_list_offset = stage_list_offset;
		return;
	}
	scroll_past_row = (uint16_t)(scroll_position - ROM16(row_cursor));
	row_cursor += 2u;
	if ((int16_t)scroll_past_row < 0x100)
	{
		if (free_object_stack < (int16_t)ROM16(row_cursor + 2u) || free_object_stack_2[0] < (int16_t)ROM16(row_cursor + 2u))
		{
			*p_stage_list_offset = stage_list_offset;
			return;
		}
		ring_row_template = ROM16(row_cursor);
		row_cursor += 2u;
		ring_row_rings_left = ROM16(row_cursor);
		row_cursor += 2u;
		ring_row_x = ROM16(row_cursor);
		row_cursor += 2u;
		ring_row_y = ROM16(row_cursor);
		row_cursor += 2u;
		ring_row_dx = (int8_t)ROM8(row_cursor);
		ring_row_dy = (int8_t)ROM8(row_cursor + 1u);
		ring_row_next_80 = 0u;
		do
		{
			allocate_object_outside_loop(&r0, &object);
			template_cursor = stage_object_templates_ADDRESS + ring_row_template;
			OBJECT(object)->sprite_flags = ROM16(template_cursor);
			template_cursor += 2u;
			OBJECT(object)->sprite_frame = ROM16(template_cursor);
			template_cursor += 2u;
			OBJECT(object)->sprite_palette = ROM16(template_cursor);
			template_cursor += 2u;
			OBJECT(object)->hitbox_size_x = ROM16(template_cursor);
			OBJECT(object)->hitbox_size_y = 0u;
			template_cursor += 2u;
			OBJECT(object)->var_82 = ROM16(template_cursor);
			template_cursor += 2u;
			OBJECT(object)->animation_speed = ROM8(template_cursor);
			template_cursor++;
			OBJECT(object)->animation_timer = ROM8(template_cursor);
			template_cursor++;
			OBJECT(object)->animation_frame = ROM8(template_cursor);
			template_cursor++;
			OBJECT(object)->kind = ROM8(template_cursor);
			template_cursor++;
			OBJECT(object)->animation = ROM32(template_cursor);
			template_cursor += 4u;
			OBJECT(object)->handler = ROM32(template_cursor);
			template_cursor += 4u;
			OBJECT(object)->sprite_offset_x = ROM8(template_cursor);
			template_cursor++;
			OBJECT(object)->sprite_offset_y = ROM8(template_cursor);
			OBJECT(object)->hitbox_height = ROM16(template_cursor + 1u);
			AS16(OBJECT(object)->health) = scroll_position;
			AS16(OBJECT(object)->var_80) = ring_row_next_80;
			ring_row_next_80 += 0x20u;
			OBJECT(object)->flags = 0u;
			OBJECT(object)->x_int = ring_row_x;
			OBJECT(object)->x_fraction = 0u;
			OBJECT(object)->y_int = ring_row_y;
			OBJECT(object)->y_fraction = 0u;
			ring_row_x += ring_row_dx;
			ring_row_y += ring_row_dy;
			current_object = object;
			find_floor(&r0, &stage_list_offset, &free_stack, &slope_height, &object);
			OBJECT(current_object)->z = OBJECT(current_object)->floor_z;
			OBJECT(current_object)->z_int -= OBJECT(current_object)->var_82;
			project_object(&ignored, &stage_list_offset);
			object_cursor = current_object;
			allocate_object_2(&ignored, &free_stack, &object);
			OBJECT(object)->sprite_flags = 0x1000u;
			OBJECT(object)->sprite_palette = 0x9FFEu;
			OBJECT(object)->sprite_frame = 0x5D90u;
			AS8(OBJECT(object)->hitbox_size_x) = 0u;
			OBJECT(object_cursor)->child = object;
			OBJECT(object)->parent = current_object;
			rings_left = ring_row_rings_left;
			ring_row_rings_left = rings_left - 1u;
		} while ((int16_t)rings_left > 1);
	}
	ring_spawn_cursor += 0xCu;
	*p_stage_list_offset = stage_list_offset;
	*p_free_stack = free_stack;
	*p_slope_height = slope_height;
	*p_object = object;
}

void ring_spin(void)
{
	uint8_t sound_id = 0;
	uint16_t child = 0;
	uint32_t r1 = 0, r7 = 0, r8 = 0, depth = 0, ignored = 0, object = 0, off_screen = 0, player = 0, write_index_address = 0;

	if (map_screen_active != 0u || current_game_mode == 0x20u)
	{
		child = OBJECT(current_object)->child;
		free_object_2(child);
		delete_current_object();
		return;
	}
	if ((OBJECT(current_object)->flags & 4u) == 0u)
	{
	ring_spin_keep_spinning:
		animate(&r1, &r7, &r8);
		if (OBJECT(current_object)->animation_speed == (uint8_t)OBJECT(current_object)->animation_timer && OBJECT(current_object)->animation_frame == 0x15u)
		{
			OBJECT(current_object)->ring_spin.unused_glint_timer = 0x78u;
			spawn_ring_glint(player);
		}
		update_screen_position(&ignored, &r1);
		off_screen = check_left_screen();
		if (off_screen != 0u)
		{
			child = OBJECT(current_object)->child;
			free_object_2(child);
			delete_current_object();
			return;
		}
		add_to_touchable_list(ignored);
		queue_sprite_at_floor();
		if (OBJECT(current_object)->floor_type != 7u || (PLAYER(current_player)->step_accumulator & 0x10u) != 0u)
		{
			draw_child_shadow();
			return;
		}
		if (OBJECT(current_object)->floor_block >= 0 && (int16_t)OBJECT(current_object)->child >= 0)
		{
			current_object = LINKED(current_object, OBJECT(object_cursor)->child);
			OBJECT(current_object)->sprite_frame = OBJECT(object_cursor)->sprite_frame;
			OBJECT(current_object)->sprite_palette = OBJECT(object_cursor)->sprite_palette;
			OBJECT(current_object)->sprite_palette &= 0x1FFFu;
			OBJECT(current_object)->sprite_palette |= 0x2000u;
			OBJECT(current_object)->sprite_flags = 0x1004u;
			OBJECT(current_object)->screen_x = OBJECT(object_cursor)->projected_x - view_x + OBJECT(object_cursor)->sprite_offset_x;
			OBJECT(current_object)->screen_y = ((int32_t)(OBJECT(object_cursor)->projected_y + ((uint32_t)OBJECT(object_cursor)->floor_z - OBJECT(object_cursor)->z + OBJECT(object_cursor)->floor_z + 0x180000u)) >> 16) - view_y - OBJECT(object_cursor)->sprite_offset_y;
			depth = (uint16_t)(OBJECT(current_object)->screen_y - 0x18u);
			queue_sprite_at_depth(&ignored, &depth);
		}
		return;
	}
	object = LINKED(current_object, OBJECT(current_object)->touched_by);
	if ((int8_t)OBJECT(object)->kind > 2)
	{
		sound_id = 0x13u;
		play_sound(sound_id, &r1, &write_index_address);
		spawn_ring_sparkle(player);
		child = OBJECT(current_object)->child;
		free_object_2(child);
		delete_current_object();
		return;
	}
	player = PLAYER_BLOCK(1) + ((uint32_t)OBJECT(object)->player << 9);
	if ((PLAYER(player)->state == 0xDu || PLAYER(player)->state == 0x20u) && (uint8_t)PLAYER(player)->hurt_contact_delay != 0u)
		goto ring_spin_keep_spinning;
	sound_id = 0x13u;
	play_sound(sound_id, &r1, &write_index_address);
	spawn_ring_sparkle(player);
	child = OBJECT(current_object)->child;
	free_object_2(child);
	delete_current_object();
}

void still_ring_wait(void)
{
	uint8_t sound_id = 0;
	uint16_t child = 0;
	uint32_t r1 = 0, ignored = 0, off_screen = 0, write_index_address = 0;

	if ((OBJECT(current_object)->flags & 4u) != 0u)
	{
		sound_id = 0x13u;
		play_sound(sound_id, &r1, &write_index_address);
		child = OBJECT(current_object)->child;
		free_object_2(child);
		delete_current_object();
		return;
	}
	update_screen_position(&ignored, &r1);
	off_screen = is_off_screen();
	if (off_screen == 0u)
	{
		add_to_touchable_list(ignored);
		queue_sprite_at_floor();
		draw_child_shadow();
		return;
	}
	child = OBJECT(current_object)->child;
	free_object_2(child);
	delete_current_object();
}

void carried_ring_wait(void)
{
	if ((OBJECT(current_object)->flags & 8u) != 0u)
		OBJECT(current_object)->handler = carried_ring_spin_ADDRESS;
	add_to_rider_list();
	OBJECT(LINKED(current_object, OBJECT(current_object)->child))->sprite_palette = 0x87A0u;
}

void carried_ring_spin(void)
{
	uint8_t sound_id = 0;
	uint16_t child = 0;
	uint32_t r1 = 0, r7 = 0, r8 = 0, ignored = 0, object = 0, off_screen = 0, platform_z = 0, player = 0, write_index_address = 0;

	if (map_screen_active != 0u)
	{
		child = OBJECT(current_object)->child;
		free_object_2(child);
		delete_current_object();
		return;
	}
	if ((OBJECT(current_object)->flags & 4u) == 0u)
	{
	carried_ring_keep_spinning:
		OBJECT(current_object)->flags &= ~0x10u;
		if ((OBJECT(current_object)->flags & 8u) != 0u)
			OBJECT(current_object)->flags |= 0x10u;
		object = LINKED(current_object, OBJECT(current_object)->platform);
		platform_z = OBJECT(object)->z;
		OBJECT(current_object)->floor_z = platform_z;
		OBJECT(current_object)->z = platform_z;
		OBJECT(current_object)->z_int -= OBJECT(current_object)->carried_ring.hover_height;
		OBJECT(current_object)->floor_block = OBJECT(object)->floor_block;
		animate(&r1, &r7, &r8);
		if (OBJECT(current_object)->animation_speed == (uint8_t)OBJECT(current_object)->animation_timer && OBJECT(current_object)->animation_frame == 0x15u)
		{
			OBJECT(current_object)->carried_ring.glint_timer = 0x78u;
			OBJECT(current_object)->carried_ring.glint_timer_high = 0u;
			spawn_ring_glint(player);
		}
		update_screen_position(&ignored, &r1);
		off_screen = check_left_screen();
		if (off_screen == 0u)
		{
			add_to_touchable_list(ignored);
			queue_sprite_at_floor();
			draw_child_shadow();
			return;
		}
		child = OBJECT(current_object)->child;
		free_object_2(child);
		delete_current_object();
		return;
	}
	object = LINKED(current_object, OBJECT(current_object)->touched_by);
	if ((int8_t)OBJECT(object)->kind > 2)
	{
		sound_id = 0x13u;
		play_sound(sound_id, &r1, &write_index_address);
		spawn_ring_sparkle(player);
		child = OBJECT(current_object)->child;
		free_object_2(child);
		delete_current_object();
		return;
	}
	player = PLAYER_BLOCK(1) + ((uint32_t)OBJECT(object)->player << 9);
	if ((PLAYER(player)->state == 0xDu || PLAYER(player)->state == 0x20u) && (uint8_t)PLAYER(player)->hurt_contact_delay != 0u)
		goto carried_ring_keep_spinning;
	sound_id = 0x13u;
	play_sound(sound_id, &r1, &write_index_address);
	spawn_ring_sparkle(player);
	child = OBJECT(current_object)->child;
	free_object_2(child);
	delete_current_object();
}

void carried_ring_spin_spare_return(void)
{
}

void scatter_rings_fixed(uint32_t count, uint32_t *p_random_value, uint32_t *p_allocated)
{
	uint32_t random_value = *p_random_value;
	uint32_t allocated = *p_allocated;
	uint32_t ring = 0;

	scatter_fixed = 0xFFFFu;
	scatter_rings(count, &random_value, &allocated, &ring);
	*p_random_value = random_value;
	*p_allocated = allocated;
}

void scatter_rings_random(uint32_t random_value, uint32_t count, uint32_t *p_allocated, uint32_t *p_ring)
{
	uint32_t allocated = *p_allocated;
	uint32_t ring = *p_ring;

	scatter_fixed = 0u;
	scatter_rings(count, &random_value, &allocated, &ring);
	*p_allocated = allocated;
	*p_ring = ring;
}

void scatter_rings(uint32_t count, uint32_t *p_random_value, uint32_t *p_allocated, uint32_t *p_ring)
{
	uint32_t random_value = *p_random_value;
	uint32_t allocated = *p_allocated;
	uint32_t ring = *p_ring;
	uint32_t free_count = 0, ignored = 0;

	if ((int16_t)count > 0xA)
		count = (count & ~0xFFFFu) | 0xAu;
	do
	{
		if (free_object_stack_2[0] <= 0)
		{
			current_object = object_cursor;
			*p_random_value = random_value;
			*p_allocated = allocated;
			*p_ring = ring;
			return;
		}
		allocate_object(&free_count, &allocated);
		if (free_count == 0u)
		{
			current_object = object_cursor;
			*p_random_value = random_value;
			*p_allocated = allocated;
			*p_ring = ring;
			return;
		}
		OBJECT(allocated)->sprite_flags = 0x1004u;
		OBJECT(allocated)->sprite_frame = 0x5E70u;
		OBJECT(allocated)->sprite_palette = 0x8430u;
		OBJECT(allocated)->hitbox_size_x = 0x10u;
		OBJECT(allocated)->hitbox_size_y = 0u;
		OBJECT(allocated)->hitbox_height = 0x20u;
		OBJECT(allocated)->kind = 3u;
		OBJECT(allocated)->x = OBJECT(current_object)->x;
		OBJECT(allocated)->y = OBJECT(current_object)->y;
		OBJECT(allocated)->z = OBJECT(current_object)->z;
		OBJECT(allocated)->animation_speed = 2u;
		OBJECT(allocated)->animation_timer = 2;
		OBJECT(allocated)->animation_frame = 0u;
		OBJECT(allocated)->animation = ring_animation_ADDRESS;
		OBJECT(allocated)->flags = 0x204u;
		OBJECT(allocated)->scattered_ring.lifetime = 0x12C;
		OBJECT(allocated)->scattered_ring.pickup_delay = 0x1E;
		OBJECT(allocated)->sprite_offset_x = 0;
		OBJECT(allocated)->sprite_offset_y = 0;
		OBJECT(allocated)->handler = scattered_ring_wait_ADDRESS;
		OBJECT(allocated)->speed_z = 0;
		if (scatter_fixed_jump == 0u)
		{
			next_random(&random_value, &ignored, &ignored);
			random_value = (random_value & 0x1FFFFu) + 0x80000u;
			OBJECT(allocated)->speed_z = 0u - random_value;
		}
		OBJECT(allocated)->speed = 0;
		if (AS8(scatter_fixed) == 0u)
		{
			next_random(&random_value, &ignored, &ignored);
			random_value = (random_value & 0x7FFFFFu) + 0x800000u;
			OBJECT(allocated)->speed = random_value;
		}
		if ((count & 1u) == 0u)
			OBJECT(allocated)->direction = 0u - (scatter_fixed - 0x200u) + 0x200u;
		else
		{
			next_random(&random_value, &ignored, &ignored);
			random_value = (random_value & ~0xFFFFu) | (random_value & 0xFFu);
			random_value = (random_value & ~0xFFFFu) | (uint16_t)(random_value + 0x180u);
			OBJECT(allocated)->direction = random_value;
			scatter_fixed = random_value;
		}
		current_object = allocated;
		set_speed_from_direction();
		if ((OBJECT(object_cursor)->flags & 0x10u) != 0u)
		{
			OBJECT(current_object)->flags |= 8u;
			OBJECT(current_object)->platform = OBJECT(object_cursor)->platform;
		}
		ring = allocated;
		allocate_object_2(&ignored, &ignored, &allocated);
		OBJECT(allocated)->sprite_flags = 0x1000u;
		OBJECT(allocated)->sprite_palette = 0x9FFEu;
		OBJECT(allocated)->sprite_frame = 0x5D90u;
		OBJECT(ring)->child = allocated;
		OBJECT(allocated)->parent = ring;
		count--;
	} while (count != 0u);
	current_object = object_cursor;
	*p_random_value = random_value;
	*p_allocated = allocated;
	*p_ring = ring;
}

void scattered_ring_wait(void)
{
	uint16_t pickup_delay = 0;

	pickup_delay = OBJECT(current_object)->scattered_ring.pickup_delay;
	OBJECT(current_object)->scattered_ring.pickup_delay = pickup_delay - 1u;
	if ((int16_t)pickup_delay > 1)
	{
		move_scattered_ring(0u);
		return;
	}
	OBJECT(current_object)->flags &= ~0x200u;
	OBJECT(current_object)->handler = scattered_ring_live_ADDRESS;
	scattered_ring_live();
}

void scattered_ring_live(void)
{
	uint8_t sound_id = 0;
	uint16_t child = 0;
	uint32_t object = 0, player = 0, write_index = 0, write_index_address = 0;
	uint16_t lifetime = 0;

	lifetime = OBJECT(current_object)->scattered_ring.lifetime;
	OBJECT(current_object)->scattered_ring.lifetime = lifetime - 1u;
	if ((int16_t)lifetime <= 1)
	{
		OBJECT(current_object)->sprite_flags = 4u;
		OBJECT(current_object)->sprite_scale = 0x100u;
		object = LINKED(current_object, OBJECT(current_object)->child);
		OBJECT(object)->sprite_flags = 0u;
		OBJECT(object)->sprite_scale = 0x100u;
		OBJECT(current_object)->handler = scattered_ring_vanish_ADDRESS;
		scattered_ring_vanish();
		return;
	}
	if ((OBJECT(current_object)->flags & 4u) == 0u)
	{
		move_scattered_ring(player);
		return;
	}
	object = LINKED(current_object, OBJECT(current_object)->touched_by);
	if ((int8_t)OBJECT(object)->kind <= 2)
	{
		player = PLAYER_BLOCK(1) + ((uint32_t)OBJECT(object)->player << 9);
		if ((PLAYER(player)->state == 0xDu || PLAYER(player)->state == 0x20u) && (uint8_t)PLAYER(player)->hurt_contact_delay != 0u)
		{
			move_scattered_ring(player);
			return;
		}
	}
	sound_id = 0x13u;
	play_sound(sound_id, &write_index, &write_index_address);
	spawn_ring_sparkle(player);
	child = OBJECT(current_object)->child;
	free_object_2(child);
	delete_current_object();
}

void move_scattered_ring(uint32_t cursor)
{
	uint16_t margin_x = 0, margin_y = 0;
	uint32_t r1 = 0, r7 = 0, r8 = 0, block_offset = 0, ignored = 0, object = 0, off_screen = 0, rebound_speed = 0, y = 0;

	OBJECT(current_object)->x += OBJECT(current_object)->speed_x;
	OBJECT(current_object)->y += OBJECT(current_object)->speed_y;
	find_floor(&rebound_speed, &y, &block_offset, &ignored, &cursor);
	if ((int16_t)(OBJECT(current_object)->z_int - OBJECT(current_object)->floor_z_int) > 8)
	{
		OBJECT(current_object)->x -= OBJECT(current_object)->speed_x;
		OBJECT(current_object)->y -= OBJECT(current_object)->speed_y;
		OBJECT(current_object)->speed_x = 0u - OBJECT(current_object)->speed_x;
		OBJECT(current_object)->speed_y = 0u - OBJECT(current_object)->speed_y;
		OBJECT(current_object)->floor_block = OBJECT(current_object)->previous_floor_block;
		OBJECT(current_object)->floor_z = OBJECT(current_object)->previous_floor_z;
	}
	OBJECT(current_object)->speed_z += 0x6000u;
	OBJECT(current_object)->z += OBJECT(current_object)->speed_z;
	if (OBJECT(current_object)->floor_z_int <= OBJECT(current_object)->z_int)
	{
		OBJECT(current_object)->z = OBJECT(current_object)->floor_z;
		rebound_speed = 0u - OBJECT(current_object)->speed_z;
		OBJECT(current_object)->speed_z = rebound_speed - ((int32_t)rebound_speed >> 2);
	}
	project_object(&ignored, &r1);
	animate(&r1, &r7, &r8);
	if (OBJECT(current_object)->animation_speed == (uint8_t)OBJECT(current_object)->animation_timer && OBJECT(current_object)->animation_frame == 0x15u)
		spawn_ring_glint(cursor);
	margin_x = 0x20u;
	margin_y = 0x58u;
	off_screen = is_off_screen_margin(margin_x, margin_y);
	if (off_screen == 0u)
	{
		OBJECT(current_object)->flags &= ~0x10u;
		if ((OBJECT(current_object)->flags & 8u) != 0u)
			OBJECT(current_object)->flags |= 0x10u;
		add_to_touchable_list(ignored);
		add_to_rider_list();
		queue_sprite_at_floor();
		draw_child_shadow();
		return;
	}
	OBJECT(current_object)->sprite_flags = 4u;
	OBJECT(current_object)->sprite_scale = 0x100u;
	object = LINKED(current_object, OBJECT(current_object)->child);
	OBJECT(object)->sprite_flags = 0u;
	OBJECT(object)->sprite_scale = 0x100u;
	OBJECT(current_object)->handler = scattered_ring_vanish_ADDRESS;
	scattered_ring_vanish();
}

void scattered_ring_vanish(void)
{
	uint16_t child = 0;
	uint32_t ignored = 0, object = 0;
	uint16_t sprite_scale = 0;

	OBJECT(current_object)->x += OBJECT(current_object)->speed_x;
	OBJECT(current_object)->y += OBJECT(current_object)->speed_y;
	object = LINKED(current_object, OBJECT(current_object)->child);
	OBJECT(object)->sprite_scale -= 0x10u;
	sprite_scale = OBJECT(current_object)->sprite_scale;
	OBJECT(current_object)->sprite_scale = sprite_scale - 0x10u;
	if ((int16_t)sprite_scale > 0x10)
	{
		project_object(&ignored, &ignored);
		queue_sprite_at_floor();
		draw_child_shadow();
		return;
	}
	child = OBJECT(current_object)->child;
	free_object_2(child);
	delete_current_object();
}

void spawn_ring_sparkle(uint32_t sparkle)
{
	uint32_t free_count = 0;

	allocate_object(&free_count, &sparkle);
	if (free_count == 0u)
		return;
	OBJECT(sparkle)->animation = ring_sparkle_animation_ADDRESS;
	OBJECT(sparkle)->sprite_offset_x = 0;
	OBJECT(sparkle)->sprite_offset_y = -0x10;
	init_sparkle_object(sparkle);
}

void spawn_ring_glint(uint32_t sparkle)
{
	uint32_t free_count = 0;

	allocate_object(&free_count, &sparkle);
	if (free_count == 0u)
		return;
	OBJECT(sparkle)->animation = ring_glint_animation_ADDRESS;
	OBJECT(sparkle)->sprite_offset_x = 0x10;
	OBJECT(sparkle)->sprite_offset_y = -8;
	init_sparkle_object(sparkle);
}

void init_sparkle_object(uint32_t sparkle)
{
	OBJECT(sparkle)->sprite_flags = 0x1000u;
	OBJECT(sparkle)->sprite_palette = 0x1B0u;
	OBJECT(sparkle)->flags = 0u;
	OBJECT(sparkle)->handler = sparkle_start_ADDRESS;
	OBJECT(sparkle)->x = OBJECT(current_object)->x;
	OBJECT(sparkle)->y = OBJECT(current_object)->y;
	OBJECT(sparkle)->floor_z = OBJECT(current_object)->floor_z;
	OBJECT(sparkle)->z = OBJECT(current_object)->z;
	OBJECT(sparkle)->animation_frame = 0xFFu;
	OBJECT(sparkle)->animation_timer = 0;
	OBJECT(sparkle)->animation_speed = 2u;
}

void sparkle_start(void)
{
	uint32_t ignored = 0;

	project_object(&ignored, &ignored);
	OBJECT(current_object)->handler = sparkle_animate_ADDRESS;
	sparkle_animate();
}

void sparkle_animate(void)
{
	uint32_t r1 = 0, r7 = 0, at_last_step = 0, depth = 0, ignored = 0;

	animate(&r1, &r7, &at_last_step);
	if ((int8_t)at_last_step >= 0)
	{
		update_screen_position(&ignored, &r1);
		depth = (uint16_t)(((int32_t)((uint32_t)OBJECT(current_object)->projected_y + OBJECT(current_object)->floor_z) >> 16) - view_y + 0x10u);
		queue_sprite_at_depth(&ignored, &depth);
		return;
	}
	delete_current_object();
}

void spawn_gauge_ring(uint32_t ring)
{
	uint8_t sound_id = 0;
	uint32_t free_count = 0, write_index = 0, write_index_address = 0;

	allocate_object(&free_count, &ring);
	if (free_count != 0u)
	{
		OBJECT(ring)->sprite_flags = 0u;
		OBJECT(ring)->sprite_palette = 0x8430u;
		OBJECT(ring)->sprite_frame = 0x5E70u;
		OBJECT(ring)->sprite_offset_x = 0;
		OBJECT(ring)->sprite_offset_y = 0;
		OBJECT(ring)->handler = gauge_ring_update_ADDRESS;
		OBJECT(ring)->flags = 0u;
		OBJECT(ring)->sprite_scale = 0x100u;
		OBJECT(ring)->animation = ring_animation_ADDRESS;
		OBJECT(ring)->animation_speed = 4u;
		OBJECT(ring)->animation_timer = 0;
		OBJECT(ring)->animation_frame = 0u;
		OBJECT(ring)->parent = current_object;
		OBJECT(ring)->screen_x = OBJECT(current_object)->screen_x;
		OBJECT(ring)->screen_y = OBJECT(current_object)->screen_y - 0x20u;
		OBJECT(ring)->x_int = OBJECT(current_object)->screen_x;
		OBJECT(ring)->x_fraction = 0u;
		OBJECT(ring)->y_int = OBJECT(ring)->screen_y;
		OBJECT(ring)->y_fraction = 0u;
		OBJECT(ring)->gauge_ring.base_y = OBJECT(ring)->screen_y;
		OBJECT(ring)->direction = 0u;
		OBJECT(ring)->gauge_ring.step = 0u;
		OBJECT(ring)->kind = OBJECT(current_object)->kind;
		OBJECT(ring)->speed_x = (int32_t)((uint32_t)(uint16_t)(ROM16(hud_gauge_positions_3p_ADDRESS + OBJECT(current_object)->player * 4u) + 0x10u - OBJECT(ring)->screen_x) << 16) / 0x3C;
		OBJECT(ring)->gauge_ring.turn_speed = 0x88u;
		OBJECT(ring)->gauge_ring.hop_timer = 0x1E;
		sound_id = 0x7Fu;
		play_sound(sound_id, &write_index, &write_index_address);
	}
}

void gauge_ring_update(void)
{
	uint32_t r1 = 0, r7 = 0, r8 = 0;

	switch (OBJECT(current_object)->gauge_ring.step)
	{
	case 0:
		gauge_ring_rise();
		break;
	case 1:
		gauge_ring_fall_into_gauge();
		break;
	case 2:
		gauge_ring_rise();
		break;
	default:
		unknown_code(0x1EC2Du + OBJECT(current_object)->gauge_ring.step * 3u);
	}
	animate(&r1, &r7, &r8);
	r1 = queue_sprite(r1);
}

void gauge_ring_rise(void)
{
	uint16_t gauge_y = 0, y = 0;
	uint32_t direction = 0;
	uint8_t hop_timer = 0;

	OBJECT(current_object)->x += OBJECT(current_object)->speed_x;
	OBJECT(current_object)->screen_x = OBJECT(current_object)->x_int;
	OBJECT(current_object)->direction += OBJECT(current_object)->gauge_ring.turn_speed;
	OBJECT(current_object)->direction &= 0x3FFFu;
	direction = OBJECT(current_object)->direction;
	y = (uint16_t)(((int16_t)(0u - sine_table[direction >> 4]) >> 8) + OBJECT(current_object)->gauge_ring.base_y);
	OBJECT(current_object)->y_int = y;
	OBJECT(current_object)->screen_y = y;
	hop_timer = OBJECT(current_object)->gauge_ring.hop_timer;
	OBJECT(current_object)->gauge_ring.hop_timer = hop_timer - 1u;
	if ((int8_t)hop_timer <= 1)
	{
		OBJECT(current_object)->gauge_ring.step++;
		gauge_y = ROM16(hud_gauge_positions_3p_y_ADDRESS + OBJECT(current_object)->kind * 4u);
		OBJECT(current_object)->gauge_ring.base_y = gauge_y;
		OBJECT(current_object)->gauge_ring.drop_height = 0u - (uint16_t)((uint32_t)(gauge_y - OBJECT(current_object)->screen_y) << 2);
		OBJECT(current_object)->gauge_ring.turn_speed = 0x88u;
	}
}

void gauge_ring_fall_into_gauge(void)
{
	uint32_t direction = 0, object = 0;

	OBJECT(current_object)->x += OBJECT(current_object)->speed_x;
	OBJECT(current_object)->screen_x = OBJECT(current_object)->x_int;
	OBJECT(current_object)->direction += OBJECT(current_object)->gauge_ring.turn_speed;
	OBJECT(current_object)->direction &= 0x3FFFu;
	direction = OBJECT(current_object)->direction;
	OBJECT(current_object)->y = (uint32_t)sine_table[direction >> 4] * OBJECT(current_object)->gauge_ring.drop_height + ((uint32_t)OBJECT(current_object)->gauge_ring.base_y << 16);
	OBJECT(current_object)->screen_y = OBJECT(current_object)->y_int;
	OBJECT(current_object)->sprite_scale -= 8u;
	if ((int16_t)OBJECT(current_object)->sprite_scale > 0)
		return;
	object = LINKED(current_object, OBJECT(current_object)->parent);
	OBJECT(object)->health++;
	delete_current_object();
}

void spawn_stage_template_0(uint16_t x, uint16_t y, uint16_t z, uint32_t object)
{
	uint32_t r0 = 0, r1 = 0, block_offset = 0, free_count = 0, ignored = 0;
	uint32_t saved_current_object = 0;

	saved_current_object = current_object;
	template_spawn_x = x;
	template_spawn_y = y;
	template_spawn_z = z;
	if (free_object_stack_2[0] > 0)
	{
		allocate_object(&free_count, &object);
		if (free_count != 0u)
		{
			OBJECT(object)->sprite_flags = stage_object_templates;
			OBJECT(object)->sprite_frame = stage_template_0_sprite_frame;
			OBJECT(object)->sprite_palette = stage_template_0_sprite_palette;
			OBJECT(object)->hitbox_size_x = stage_template_0_hitbox_size;
			OBJECT(object)->hitbox_size_y = 0u;
			OBJECT(object)->animation_speed = stage_template_0_animation_speed;
			OBJECT(object)->animation_timer = stage_template_0_animation_timer;
			OBJECT(object)->animation_frame = stage_template_0_animation_frame;
			OBJECT(object)->kind = stage_template_0_kind;
			OBJECT(object)->animation = stage_template_0_animation;
			OBJECT(object)->handler = stage_template_0_handler;
			OBJECT(object)->sprite_offset_x = stage_template_0_offset_x;
			OBJECT(object)->sprite_offset_y = stage_template_0_offset_y;
			OBJECT(object)->hitbox_height = stage_template_0_hitbox_height;
			AS16(OBJECT(object)->health) = scroll_position;
			OBJECT(object)->flags = 0u;
			OBJECT(object)->x_int = template_spawn_x;
			OBJECT(object)->x_fraction = 0u;
			OBJECT(object)->y_int = template_spawn_y;
			OBJECT(object)->y_fraction = 0u;
			OBJECT(object)->z_int = template_spawn_z;
			OBJECT(object)->z_fraction = 0u;
			current_object = object;
			project_object(&r0, &r1);
			find_floor(&r0, &r1, &block_offset, &ignored, &object);
			current_player = current_object;
			allocate_object_2(&ignored, &ignored, &object);
			OBJECT(object)->sprite_flags = 0x1000u;
			OBJECT(object)->sprite_palette = 0x9FFEu;
			OBJECT(object)->sprite_frame = 0x5D90u;
			OBJECT(current_player)->child = object;
			OBJECT(object)->parent = current_player;
		}
	}
	current_object = saved_current_object;
}

void spawn_stage_template(uint16_t template_index, uint16_t x, uint16_t y, uint16_t z, uint32_t object)
{
	uint32_t r0 = 0, r1 = 0, block_offset = 0, free_count = 0, ignored = 0, template_cursor = 0;
	uint32_t saved_current_object = 0;

	saved_current_object = current_object;
	template_spawn_x = x;
	template_spawn_y = y;
	template_spawn_z = z;
	template_spawn_index = template_index;
	if (free_object_stack_2[0] > 0)
	{
		allocate_object(&free_count, &object);
		if (free_count != 0u)
		{
			template_cursor = stage_object_templates_ADDRESS + template_spawn_index * 0x1Au;
			OBJECT(object)->sprite_flags = mem_read16(template_cursor);
			template_cursor += 2u;
			OBJECT(object)->sprite_frame = mem_read16(template_cursor);
			template_cursor += 2u;
			OBJECT(object)->sprite_palette = mem_read16(template_cursor);
			template_cursor += 2u;
			OBJECT(object)->hitbox_size_x = mem_read16(template_cursor);
			OBJECT(object)->hitbox_size_y = 0u;
			template_cursor += 4u;
			OBJECT(object)->animation_speed = mem_read8(template_cursor);
			template_cursor++;
			OBJECT(object)->animation_timer = mem_read8(template_cursor);
			template_cursor++;
			OBJECT(object)->animation_frame = mem_read8(template_cursor);
			template_cursor++;
			OBJECT(object)->kind = mem_read8(template_cursor);
			template_cursor++;
			OBJECT(object)->animation = mem_read32(template_cursor);
			template_cursor += 4u;
			OBJECT(object)->handler = mem_read32(template_cursor);
			template_cursor += 4u;
			OBJECT(object)->sprite_offset_x = mem_read8(template_cursor);
			template_cursor++;
			OBJECT(object)->sprite_offset_y = mem_read8(template_cursor);
			OBJECT(object)->hitbox_height = mem_read16(template_cursor + 1u);
			AS16(OBJECT(object)->health) = scroll_position;
			OBJECT(object)->flags = 0u;
			OBJECT(object)->x_int = template_spawn_x;
			OBJECT(object)->x_fraction = 0u;
			OBJECT(object)->y_int = template_spawn_y;
			OBJECT(object)->y_fraction = 0u;
			OBJECT(object)->z_int = template_spawn_z;
			OBJECT(object)->z_fraction = 0u;
			current_object = object;
			project_object(&r0, &r1);
			find_floor(&r0, &r1, &block_offset, &ignored, &object);
			current_player = current_object;
			allocate_object_2(&ignored, &ignored, &object);
			OBJECT(object)->sprite_flags = 0x1000u;
			OBJECT(object)->sprite_palette = 0x9FFEu;
			OBJECT(object)->sprite_frame = 0x5D90u;
			OBJECT(current_player)->child = object;
			OBJECT(object)->parent = current_player;
		}
	}
	current_object = saved_current_object;
}

void next_random(uint32_t *p_random_value, uint32_t *p_words_left, uint32_t *p_random_offset)
{
	uint32_t random_value = *p_random_value;
	uint32_t random_offset = 0, words_left = 0;

	random_offset = random_position;
	words_left = 2u;
	do
	{
		random_offset = (random_offset + mem_read16(random_offset + 0xD80000u)) & 0x7FFFFu;
		random_value = (random_value & ~0xFFFFu) | mem_read16(random_offset + 0xD80000u);
		random_value = (random_value << 16) | (random_value >> 16);
		words_left--;
	} while (words_left != 0u);
	random_position = random_offset;
	*p_random_value = random_value;
	*p_words_left = words_left;
	*p_random_offset = random_offset;
}

void create_child_shadow_unscaled(uint32_t shadow)
{
	uint32_t free_count = 0, ignored = 0, owner = 0;

	OBJECT(current_object)->child = 0xFFFFu;
	owner = current_object;
	allocate_object_2(&free_count, &ignored, &shadow);
	if (free_count != 0u)
	{
		OBJECT(shadow)->sprite_flags = 0x1000u;
		OBJECT(shadow)->sprite_palette = 0x9FFEu;
		OBJECT(shadow)->sprite_frame = 0x5D90u;
		AS8(OBJECT(shadow)->hitbox_size_x) = 0u;
		OBJECT(owner)->child = shadow;
		OBJECT(shadow)->parent = current_object;
	}
}

void create_child_shadow_9ffe(uint16_t scale, uint32_t shadow)
{
	uint32_t free_count = 0, ignored = 0;

	OBJECT(current_object)->child = 0xFFFFu;
	current_player = current_object;
	allocate_object_2(&free_count, &ignored, &shadow);
	if (free_count != 0u)
	{
		OBJECT(shadow)->sprite_flags = 0u;
		OBJECT(shadow)->sprite_palette = 0x9FFEu;
		OBJECT(shadow)->sprite_frame = 0x5D90u;
		OBJECT(shadow)->sprite_scale = scale;
		AS8(OBJECT(shadow)->hitbox_size_x) = 0u;
		OBJECT(current_player)->child = shadow;
		OBJECT(shadow)->parent = current_object;
	}
}

void create_child_shadow(uint16_t scale, uint32_t shadow)
{
	uint32_t free_count = 0, ignored = 0;

	OBJECT(current_object)->child = 0xFFFFu;
	current_player = current_object;
	allocate_object_2(&free_count, &ignored, &shadow);
	if (free_count != 0u)
	{
		OBJECT(shadow)->sprite_flags = 0u;
		OBJECT(shadow)->sprite_palette = 0x87A0u;
		OBJECT(shadow)->sprite_frame = 0x5D90u;
		OBJECT(shadow)->sprite_scale = scale;
		AS8(OBJECT(shadow)->hitbox_size_x) = 0u;
		OBJECT(current_player)->child = shadow;
		OBJECT(shadow)->parent = current_object;
	}
}

void draw_child_shadow(void)
{
	uint32_t depth = 0, ignored = 0;
	uint8_t shadow_blink = 0;

	if (OBJECT(current_object)->floor_block < 0 || (int16_t)OBJECT(current_object)->child < 0)
		return;
	current_object = LINKED(current_object, OBJECT(object_cursor)->child);
	OBJECT(current_object)->screen_x = OBJECT(object_cursor)->projected_x - view_x + OBJECT(object_cursor)->sprite_offset_x;
	OBJECT(current_object)->screen_y = ((int32_t)((uint32_t)OBJECT(object_cursor)->projected_y + OBJECT(object_cursor)->floor_z) >> 16) - view_y + OBJECT(object_cursor)->sprite_offset_y;
	OBJECT(current_object)->sprite_palette = 0x9FFEu;
	if ((OBJECT(object_cursor)->flags & 0x10u) == 0u)
	{
		depth = (uint16_t)(OBJECT(current_object)->screen_y - 0x10u);
		queue_sprite_at_depth(&ignored, &depth);
		current_object = (current_object & ~0xFFFFu) | (uint16_t)object_cursor;
	}
	else
	{
		OBJECT(current_object)->sprite_palette = 0x87A0u;
		shadow_blink = ~AS8(OBJECT(current_object)->hitbox_size_x);
		AS8(OBJECT(current_object)->hitbox_size_x) = shadow_blink;
		if (shadow_blink == 0u)
		{
			depth = (uint16_t)(OBJECT(current_object)->screen_y - 0x10u);
			queue_sprite_at_depth(&ignored, &depth);
			current_object = (current_object & ~0xFFFFu) | (uint16_t)object_cursor;
			return;
		}
	}
}

uint32_t is_floor_block_off_screen(void)
{
	uint32_t zero = 0;
	uint16_t floor_block = 0;
	uint32_t blocks_left = 0, cursor = 0, off_screen = 0;
	uint16_t listed_block = 0;

	off_screen = 0u;
	blocks_left = nearby_floor_block_count;
	cursor = nearby_floor_blocks_ADDRESS;
	if (blocks_left == 0u)
		off_screen = 0xFFFFFFFFu;
	else
	{
		floor_block = AS16(OBJECT(current_object)->floor_block);
		do
		{
			listed_block = mem_read16(cursor);
			zero = (uint16_t)(floor_block - listed_block) == 0u;
			cursor += 2u;
			blocks_left--;
		} while ((blocks_left != 0u) & (floor_block != listed_block));
		if (zero == 0u)
		{
			off_screen = 0xFFFFFFFFu;
			return off_screen;
		}
	}
	return off_screen;
}

uint32_t is_off_screen(void)
{
	uint32_t off_screen = 0;

	off_screen = 0u;
	if ((uint16_t)(OBJECT(current_object)->screen_x + 0x20u) >= 0x180u || (uint16_t)(OBJECT(current_object)->screen_y + 0x20u) >= 0x120u)
		off_screen = 0xFFFFFFFFu;
	return off_screen;
}

uint32_t is_off_screen_margin(uint16_t margin_x, uint16_t margin_y)
{
	uint32_t off_screen = 0;

	off_screen = 0u;
	if ((uint16_t)(0x140u + margin_x * 2u) <= (uint16_t)(OBJECT(current_object)->screen_x + margin_x) || (uint16_t)(0xE0u + margin_y * 2u) <= (uint16_t)(OBJECT(current_object)->screen_y + margin_y))
		off_screen = 0xFFFFFFFFu;
	return off_screen;
}

uint32_t check_left_screen(void)
{
	uint16_t edge_margin = 0;
	uint32_t off_screen = 0;

	off_screen = 0u;
	edge_margin = 0x20u;
	if ((OBJECT(current_object)->flags & 0x800u) == 0u)
		edge_margin = 0u;
	if (0x140u + edge_margin * 2u <= (uint16_t)(OBJECT(current_object)->screen_x + edge_margin) || 0xE0u + edge_margin * 2u <= (uint16_t)(OBJECT(current_object)->screen_y + edge_margin))
	{
		if ((OBJECT(current_object)->flags & 0x800u) != 0u)
			off_screen = 0xFFFFFFFFu;
	}
	else
		OBJECT(current_object)->flags |= 0x800u;
	return off_screen;
}

uint32_t check_left_screen_margin(uint16_t margin)
{
	uint32_t off_screen = 0;

	off_screen = 0u;
	if ((OBJECT(current_object)->flags & 0x800u) == 0u)
		margin = 0u;
	if ((uint16_t)(0x140u + margin * 2u) <= (uint16_t)(OBJECT(current_object)->screen_x + margin) || (uint16_t)(0xE0u + margin * 2u) <= (uint16_t)(OBJECT(current_object)->screen_y + margin))
	{
		if ((OBJECT(current_object)->flags & 0x800u) != 0u)
			off_screen = 0xFFFFFFFFu;
	}
	else
		OBJECT(current_object)->flags |= 0x800u;
	return off_screen;
}

uint32_t check_left_screen_margins(uint16_t margin, uint16_t unseen_margin)
{
	uint32_t off_screen = 0;

	off_screen = 0u;
	if ((OBJECT(current_object)->flags & 0x800u) == 0u)
		margin = (uint16_t)unseen_margin;
	if ((uint16_t)(0x140u + margin * 2u) <= (uint16_t)(OBJECT(current_object)->screen_x + margin) || (uint16_t)(0xE0u + margin * 2u) <= (uint16_t)(OBJECT(current_object)->screen_y + margin))
	{
		if ((OBJECT(current_object)->flags & 0x800u) != 0u)
			off_screen = 0xFFFFFFFFu;
	}
	else
		OBJECT(current_object)->flags |= 0x800u;
	return off_screen;
}

void allocate_object_outside_loop(uint32_t *p_free_count, uint32_t *p_object)
{
	uint32_t object = *p_object;
	uint32_t free_count = 0;

	free_objects = free_object_stack_ADDRESS;
	first_object = first_object_address;
	last_object = last_object_address;
	allocate_object(&free_count, &object);
	*p_free_count = free_count;
	*p_object = object;
}

void delete_object_outside_loop(uint32_t object, uint32_t *p_free_count, uint32_t *p_previous)
{
	uint32_t free_count = 0, previous = 0;

	free_objects = free_object_stack_ADDRESS;
	first_object = first_object_address;
	last_object = last_object_address;
	delete_object(object, &free_count, &previous);
	first_object_address = first_object;
	last_object_address = last_object;
	*p_free_count = free_count;
	*p_previous = previous;
}

void spawn_objects_by_handler(uint32_t *p_list_offset, uint32_t *p_record, uint32_t *p_object)
{
	uint32_t list_offset = *p_list_offset;
	uint32_t object = *p_object;
	uint32_t free_count = 0, record = 0;
	uint32_t old_init_flags = 0;

	old_init_flags = stage_init_flags;
	stage_init_flags = old_init_flags | 1u;
	if ((old_init_flags & 1u) == 0u)
	{
		list_offset = stage_spawns_by_handler[stage];
		handler_spawn_cursor = stage_spawns_by_handler_ADDRESS + list_offset;
	}
	record = handler_spawn_cursor;
	if (scroll_position < ROM16(record))
	{
		*p_list_offset = list_offset;
		*p_record = record;
		return;
	}
	if ((uint16_t)(scroll_position - ROM16(record)) < 0x100u)
	{
		allocate_object_outside_loop(&free_count, &object);
		if (free_count == 0u)
		{
			*p_list_offset = list_offset;
			*p_record = record;
			*p_object = object;
			return;
		}
		record = handler_spawn_cursor + 2u;
		OBJECT(object)->x_int = ROM16(record);
		OBJECT(object)->x_fraction = 0u;
		record += 2u;
		OBJECT(object)->y_int = ROM16(record);
		OBJECT(object)->y_fraction = 0u;
		record += 2u;
		OBJECT(object)->sprite_frame = ROM16(record);
		record += 2u;
		OBJECT(object)->handler = ROM32(record);
		record += 4u;
	}
	handler_spawn_cursor += 0xCu;
	*p_list_offset = list_offset;
	*p_record = record;
	*p_object = object;
}

void stage_03_floor_block_start(void)
{
	uint32_t block_kind = 0, ignored = 0, kind_entry = 0, kind_record = 0;

	block_kind = OBJECT(current_object)->sprite_frame;
	OBJECT(current_object)->speed_x_int = block_kind;
	kind_entry = stage_03_floor_block_kinds_ADDRESS + block_kind * 4u;
	kind_record = ROM16(kind_entry) + stage_03_floor_block_kinds_ADDRESS;
	OBJECT(current_object)->sprite_offset_x = ROM8(kind_record);
	kind_record++;
	OBJECT(current_object)->sprite_offset_y = ROM8(kind_record);
	kind_record++;
	OBJECT(current_object)->speed_y_fraction = ROM16(kind_record);
	OBJECT(current_object)->animation = kind_record + 2u;
	OBJECT(current_object)->speed_x_fraction = ROM16(kind_entry + 2u);
	OBJECT(current_object)->sprite_flags = 0x1004u;
	OBJECT(current_object)->sprite_palette = 0x9040u;
	OBJECT(current_object)->handler = stage_03_floor_block_update_ADDRESS;
	OBJECT(current_object)->hitbox_size_x = 0x10u;
	OBJECT(current_object)->hitbox_size_y = 0x10u;
	OBJECT(current_object)->speed_z = 0;
	OBJECT(current_object)->flags = 2u;
	stage_03_floor_block_falls[0] = 0u;
	stage_03_floor_block_falls_1 = 0u;
	OBJECT(current_object)->sprite_frame = mem_read16(OBJECT(current_object)->animation + 2u);
	OBJECT(current_object)->animation_frame = 0u;
	OBJECT(current_object)->animation_speed = 8u;
	OBJECT(current_object)->animation_timer = 0;
	OBJECT(current_object)->z = 0;
	project_object(&ignored, &ignored);
}

void stage_03_floor_block_update(void)
{
	uint32_t r1 = 0, r7 = 0, at_last_step = 0, depth = 0, ignored = 0;

	if (stage_03_floor_block_falls[OBJECT(current_object)->speed_x_fraction] != 0u)
	{
		OBJECT(current_object)->speed_z += 0x1000u;
		OBJECT(current_object)->z += OBJECT(current_object)->speed_z;
		if (OBJECT(current_object)->speed_x_int < 4)
		{
			OBJECT(current_object)->sprite_offset_x = 0;
			OBJECT(current_object)->sprite_offset_y = 0;
		}
		if ((int8_t)OBJECT(current_object)->animation_speed >= 0)
		{
			animate(&r1, &r7, &at_last_step);
			if ((uint8_t)at_last_step != 0u)
				OBJECT(current_object)->animation_speed = 0xFFu;
		}
	}
	update_screen_position(&ignored, &r1);
	if (OBJECT(current_object)->screen_y < 0x158)
	{
		depth = (uint16_t)(OBJECT(current_object)->screen_y - OBJECT(current_object)->speed_y_fraction - OBJECT(current_object)->sprite_offset_y);
		queue_sprite_at_depth(&ignored, &depth);
		return;
	}
	delete_current_object();
}

void spiked_roller_start(void)
{
	uint32_t r11 = 0, block_offset = 0, free_count = 0, ignored = 0, object = 0, record_cursor = 0, roller_kind = 0, y = 0, zone_index = 0;
	uint8_t parts_left = 0;

	roller_kind = OBJECT(current_object)->sprite_frame;
	OBJECT(current_object)->platform = roller_kind;
	OBJECT(current_object)->spiked_roller.event_offset = spiked_roller_events[roller_kind];
	record_cursor = spiked_roller_kinds_ADDRESS + spiked_roller_kinds[roller_kind];
	roller_parts_left = ROM8(record_cursor);
	record_cursor++;
	r11 = ROM16(record_cursor);
	record_cursor += 2u;
	roller_first_state = ROM16(record_cursor);
	record_cursor += 2u;
	roller_back_zone_index = ROM16(record_cursor);
	OBJECT(current_object)->spiked_roller.path = record_cursor + 2u;
	r11 += 0x8219Cu;
	roller_step_x = (uint32_t)(int8_t)ROM8(r11) << 16;
	r11++;
	roller_step_y = (uint32_t)(int8_t)ROM8(r11) << 16;
	r11++;
	roller_part_record = r11;
	OBJECT(current_object)->flags = 0u;
	find_floor(&roller_kind, &y, &block_offset, &ignored, &r11);
	OBJECT(current_object)->z = OBJECT(current_object)->floor_z;
	OBJECT(current_object)->spiked_roller.screen_threshold = 0;
	OBJECT(current_object)->spiked_roller.catch_up_factor = 0u;
	OBJECT(current_object)->spiked_roller.base_speed = 0;
	OBJECT(current_object)->spiked_roller.wait_timer = 0;
	OBJECT(current_object)->spiked_roller.rumble_timer = 0u;
	OBJECT(current_object)->spiked_roller.substate = 0u;
	OBJECT(current_object)->player = 0u;
	OBJECT(current_object)->spiked_roller.holding_back = 0u;
	roller_ram_count = 0u;
	roller_ram_hit = 0u;
	machine_fire_done = 0u;
	OBJECT(current_object)->spiked_roller.rumble_off_screen = 0xFFu;
	if (chaser_object == 0u)
		chaser_object = current_object;
	roller_part_x = OBJECT(current_object)->x;
	roller_part_y = OBJECT(current_object)->y;
	roller_part_frame_cursor = spiked_roller_part_frames_ADDRESS;
	object = current_object;
	for (;;)
	{
		OBJECT(current_object)->sprite_palette = 0x82F0u;
		OBJECT(current_object)->hitbox_height = 0xF0u;
		OBJECT(current_object)->kind = 0xCu;
		OBJECT(current_object)->handler = spiked_roller_update_ADDRESS;
		OBJECT(current_object)->sprite_offset_x = 0;
		OBJECT(current_object)->sprite_offset_y = 5;
		OBJECT(current_object)->speed_z = 0;
		OBJECT(current_object)->var_80 = 2u;
		OBJECT(current_object)->player = 0u;
		OBJECT(current_object)->speed_x = 0;
		OBJECT(current_object)->speed_y = 0;
		record_cursor = roller_part_record;
		OBJECT(current_object)->sprite_flags = mem_read16(record_cursor);
		record_cursor += 2u;
		OBJECT(current_object)->hitbox_size_x = mem_read16(record_cursor);
		record_cursor += 2u;
		OBJECT(current_object)->hitbox_size_y = mem_read16(record_cursor);
		record_cursor += 2u;
		OBJECT(current_object)->spawn_arg_2 = mem_read16(record_cursor);
		record_cursor += 2u;
		OBJECT(current_object)->flags = mem_read32(record_cursor);
		record_cursor += 4u;
		OBJECT(current_object)->hitbox_offset_x = mem_read8(record_cursor);
		OBJECT(current_object)->hitbox_offset_y = mem_read8(record_cursor + 1u);
		OBJECT(current_object)->direction = (uint32_t)OBJECT(current_object)->spawn_arg_2 << 8;
		OBJECT(current_object)->var_86 = object_cursor;
		OBJECT(current_object)->parent = object;
		OBJECT(object)->child = current_object;
		object = (object & ~0xFFFFu) | (uint16_t)current_object;
		OBJECT(current_object)->animation_timer = 0;
		OBJECT(current_object)->animation = spiked_roller_animation_ADDRESS;
		OBJECT(current_object)->animation_frame = mem_read8(roller_part_frame_cursor);
		roller_part_frame_cursor++;
		OBJECT(current_object)->sprite_frame = spiked_roller_frames[OBJECT(current_object)->animation_frame];
		parts_left = roller_parts_left;
		roller_parts_left = parts_left - 1u;
		if ((int8_t)parts_left <= 1)
			break;
		allocate_object(&free_count, &r11);
		if (free_count == 0u)
			return;
		current_object = r11;
		roller_part_x += roller_step_x;
		OBJECT(current_object)->x = roller_part_x;
		roller_part_y += roller_step_y;
		OBJECT(current_object)->y = roller_part_y;
		OBJECT(current_object)->z = OBJECT(object_cursor)->z;
		OBJECT(current_object)->spawn_arg_2 = OBJECT(object_cursor)->spiked_roller.path_direction;
	}
	OBJECT(current_object)->child = 0xFFFFu;
	AS16(OBJECT(object_cursor)->spiked_roller.state) = roller_first_state;
	zone_index = roller_back_zone_index;
	spawn_roller_back_zone(zone_index, r11);
	if (OBJECT(object_cursor)->spiked_roller.state == 0u)
		players_frozen = 0xFFu;
	if (OBJECT(object_cursor)->spiked_roller.state == 0xEu)
	{
		OBJECT(object_cursor)->speed_z = -0x100000;
		OBJECT(object_cursor)->z = 0x500000;
		OBJECT(object_cursor)->speed_x = 0x8000;
		OBJECT(object_cursor)->speed_y = 0;
		OBJECT(object_cursor)->flags |= 1u;
	}
}

void spiked_roller_update(void)
{
	uint32_t r1 = 0, ignored = 0;

	switch (OBJECT(current_object)->spiked_roller.state)
	{
	case 0:
		spiked_roller_chase();
		break;
	case 1:
		spiked_roller_fall_off_ledge();
		break;
	case 2:
		spiked_roller_follow_leader();
		break;
	case 3:
		spiked_roller_drop_in();
		break;
	case 4:
		spiked_roller_ram();
		break;
	case 5:
		spiked_roller_remove();
		break;
	case 6:
		spiked_roller_crash_into_machine();
		break;
	case 7:
		spiked_roller_sink_and_jump();
		break;
	case 8:
		spiked_roller_fall();
		break;
	case 9:
		spiked_roller_burst_out();
		break;
	case 10:
		spiked_roller_smash_wall();
		break;
	case 11:
		spiked_roller_wait_for_turn();
		break;
	case 12:
		spiked_roller_wait_to_burst_out();
		break;
	case 13:
		spiked_roller_shift_box();
		break;
	case 14:
		spiked_roller_wait_hidden();
		break;
	case 15:
		spiked_roller_spawn_fixed_zone();
		break;
	case 16:
		spiked_roller_chase();
		break;
	default:
		unknown_code(0x803BBu + OBJECT(current_object)->spiked_roller.state * 3u);
	}
	if (OBJECT(current_object)->player == 0u)
	{
		if ((OBJECT(current_object)->flags & 1u) != 0u)
			return;
		add_to_zone_list(0u);
		if (OBJECT(current_object)->spiked_roller.state == 1u)
		{
			ignored = queue_sprite_at_previous_floor();
			return;
		}
		r1 = queue_sprite_at_base(0u);
		return;
	}
	if (chaser_object == (uint16_t)current_object)
		chaser_object = 0u;
	delete_current_object();
}

void spiked_roller_chase(void)
{
	uint8_t sound_id = 0;
	uint16_t height_above_floor = 0;
	uint32_t r2 = 0, r3 = 0, r7 = 0, chase_speed = 0, cursor = 0, event_kind = 0, ignored = 0, move_step = 0, off_screen = 0, record_cursor = 0, speed_level = 0, x = 0, y = 0;
	uint8_t hold_timer = 0, rumble_timer = 0;
	uint16_t record_position = 0, z_int = 0, floor_z_int = 0, event_position = 0, max_y = 0, min_x = 0, min_y = 0, max_x = 0;
	uint32_t flags = 0;

	flags = OBJECT(current_object)->flags;
	OBJECT(current_object)->flags = flags | 0x80000000u;
	if ((flags & 0x80000000u) == 0u)
	{
	spiked_roller_chase_read_script:
		if ((uint16_t)OBJECT(current_object)->spiked_roller.wait_timer == 0u)
		{
			record_cursor = OBJECT(current_object)->spiked_roller.path;
			record_position = mem_read16(record_cursor);
			record_cursor += 2u;
			if (record_position <= scroll_position)
			{
				if ((int16_t)mem_read16(record_cursor + 2u) >= 0)
				{
					OBJECT(current_object)->spiked_roller.screen_threshold = mem_read16(record_cursor);
					record_cursor += 2u;
					OBJECT(current_object)->spiked_roller.catch_up_factor = mem_read16(record_cursor);
					record_cursor += 2u;
					OBJECT(current_object)->spiked_roller.base_speed = mem_read16(record_cursor);
					record_cursor += 2u;
					OBJECT(current_object)->spiked_roller.wait_timer = mem_read16(record_cursor);
					OBJECT(current_object)->spiked_roller.path = record_cursor + 2u;
					if ((uint16_t)OBJECT(current_object)->spiked_roller.wait_timer == 0u)
					{
						OBJECT(current_object)->kind = 0x18u;
						players_frozen = 0u;
					}
				}
				else
				{
					OBJECT(current_object)->spiked_roller.catch_up_factor = 0u;
					OBJECT(current_object)->spiked_roller.base_speed = 0;
					off_screen = is_off_screen();
					if (off_screen != 0u)
					{
						OBJECT(current_object)->spiked_roller.state = 5u;
						return;
					}
				}
			}
		}
	}
	else if (OBJECT(current_object)->spiked_roller.holding_back == 0u)
	{
		if (no_player_in_play == 0u && player_mishap == 0u && player_dropping_in == 0u)
			goto spiked_roller_chase_read_script;
		OBJECT(current_object)->spiked_roller.holding_back = 0xFFu;
		OBJECT(current_object)->spiked_roller.saved_base_speed = OBJECT(current_object)->spiked_roller.base_speed;
		OBJECT(current_object)->spiked_roller.base_speed = 0;
		OBJECT(current_object)->spiked_roller.hold_start_position = scroll_position;
		OBJECT(current_object)->spiked_roller.hold_timer = 0xFFu;
		OBJECT(current_object)->spiked_roller.saved_threshold = OBJECT(current_object)->spiked_roller.screen_threshold;
		switch (OBJECT(current_object)->spiked_roller.path_direction)
		{
		case 0:
		case 3:
		case 4:
			OBJECT(current_object)->spiked_roller.screen_threshold = 0x160;
			break;
		case 1:
		case 2:
			OBJECT(current_object)->spiked_roller.screen_threshold = -0x20;
			break;
		default:
			unknown_code(0x80448u + OBJECT(current_object)->spiked_roller.path_direction * 2u);
		}
	}
	if (OBJECT(current_object)->platform != 5u && OBJECT(current_object)->platform != 2u)
	{
		find_floor(&x, &y, &r2, &r3, &cursor);
		z_int = OBJECT(current_object)->z_int;
		floor_z_int = OBJECT(current_object)->floor_z_int;
		height_above_floor = (uint16_t)(floor_z_int - z_int);
		if (floor_z_int == z_int || (int16_t)height_above_floor <= 8)
			OBJECT(current_object)->z = OBJECT(current_object)->floor_z;
		else
		{
			OBJECT(current_object)->spiked_roller.state = 1u;
			chaser_object = 0u;
		}
	}
	project_object(&ignored, &ignored);
	if (OBJECT(current_object)->spiked_roller.holding_back != 0u)
	{
		if ((int16_t)(scroll_position - OBJECT(current_object)->spiked_roller.hold_start_position) >= 0x300)
			goto spiked_roller_chase_stop_holding_back;
		if (no_player_in_play != 0u)
			OBJECT(current_object)->spiked_roller.hold_timer = 0xFFu;
		hold_timer = OBJECT(current_object)->spiked_roller.hold_timer;
		OBJECT(current_object)->spiked_roller.hold_timer = hold_timer - 1u;
		if (hold_timer == 0u || AS32(players_finished[0]) == 0xFFFFFFu)
			goto spiked_roller_chase_stop_holding_back;
	}
	switch (OBJECT(current_object)->spiked_roller.path_direction)
	{
	case 0:
	case 3:
		if (OBJECT(current_object)->screen_x <= OBJECT(current_object)->spiked_roller.screen_threshold)
		{
		spiked_roller_chase_use_base_speed:
			chase_speed = (uint16_t)OBJECT(current_object)->spiked_roller.base_speed;
		}
		else
		{
		spiked_roller_chase_catch_up:
			OBJECT(current_object)->spiked_roller.wait_timer = 0;
			if (OBJECT(current_object)->spiked_roller.holding_back != 0u)
			{
			spiked_roller_chase_stop_holding_back:
				OBJECT(current_object)->spiked_roller.holding_back = 0u;
				OBJECT(current_object)->spiked_roller.screen_threshold = OBJECT(current_object)->spiked_roller.saved_threshold;
				OBJECT(current_object)->spiked_roller.base_speed = OBJECT(current_object)->spiked_roller.saved_base_speed;
			}
			compute_camera_ground_speed(r3);
			chase_speed = (uint16_t)((uint32_t)camera_ground_speed_int * OBJECT(current_object)->spiked_roller.catch_up_factor) >> 8;
			if ((int16_t)chase_speed <= OBJECT(current_object)->spiked_roller.base_speed)
				goto spiked_roller_chase_use_base_speed;
		}
		break;
	case 1:
	case 2:
	case 4:
		if (OBJECT(current_object)->spiked_roller.screen_threshold <= OBJECT(current_object)->screen_x)
			goto spiked_roller_chase_use_base_speed;
		goto spiked_roller_chase_catch_up;
	default:
		unknown_code(0x80548u + OBJECT(current_object)->spiked_roller.path_direction * 2u);
	}
	chaser_speed = chase_speed;
	if (current_game_mode != 0x11u)
		OBJECT(current_object)->spiked_roller.wait_timer--;
	if (OBJECT(current_object)->spiked_roller.wait_timer <= 0)
	{
		OBJECT(current_object)->spiked_roller.wait_timer = 0;
		if (grey_screen_active == 0u)
		{
			switch (OBJECT(current_object)->spiked_roller.path_direction)
			{
			case 0:
				move_step = (uint32_t)chaser_speed << 12;
				OBJECT(current_object)->y -= move_step;
				OBJECT(current_object)->speed_y = move_step;
				OBJECT(current_object)->speed_x = 0;
				break;
			case 1:
				move_step = (uint32_t)chaser_speed << 12;
				OBJECT(current_object)->x += move_step;
				OBJECT(current_object)->speed_x = move_step;
				OBJECT(current_object)->speed_y = 0;
				break;
			case 2:
			case 4:
				move_step = (uint32_t)chaser_speed << 12;
				OBJECT(current_object)->y += move_step;
				OBJECT(current_object)->speed_y = move_step;
				OBJECT(current_object)->speed_x = 0;
				break;
			case 3:
				move_step = (uint32_t)chaser_speed << 12;
				OBJECT(current_object)->x -= move_step;
				OBJECT(current_object)->speed_x = move_step;
				OBJECT(current_object)->speed_y = 0;
				break;
			default:
				unknown_code(0x805E4u + OBJECT(current_object)->spiked_roller.path_direction * 2u);
			}
		}
		else
			chaser_speed = 0;
	}
	if ((uint16_t)chaser_speed != 0u)
	{
		speed_level = (uint16_t)chaser_speed >> 4;
		if ((int16_t)speed_level > 5)
			speed_level = 5u;
		OBJECT(current_object)->animation_speed = spiked_roller_animation_speeds[speed_level];
		animate(&chase_speed, &r7, &off_screen);
	}
	switch (OBJECT(current_object)->spiked_roller.path_direction)
	{
	case 0:
	case 4:
		if (OBJECT(current_object)->screen_x > 0x140)
		{
		spiked_roller_chase_no_rumble:
			OBJECT(current_object)->spiked_roller.rumble_off_screen = 0xFFu;
		}
		else
		{
		spiked_roller_chase_rumble:
			if (OBJECT(current_object)->spiked_roller.rumble_off_screen != 0u)
			{
				OBJECT(current_object)->spiked_roller.rumble_off_screen = 0u;
				OBJECT(current_object)->spiked_roller.rumble_timer = 0u;
			}
			if ((uint16_t)chaser_speed == 0u)
				goto spiked_roller_chase_no_rumble;
			rumble_timer = OBJECT(current_object)->spiked_roller.rumble_timer;
			OBJECT(current_object)->spiked_roller.rumble_timer = rumble_timer - 1u;
			if (rumble_timer == 0u)
			{
				sound_id = 0xBu;
				play_sound(sound_id, &chase_speed, &r2);
				speed_level = (uint16_t)chaser_speed;
				speed_level = speed_level >> 4;
				if ((int16_t)speed_level < 0)
					speed_level = 0u;
				if ((int16_t)speed_level > 4)
					speed_level = 4u;
				OBJECT(current_object)->spiked_roller.rumble_timer = spiked_roller_rumble_delays[speed_level];
			}
		}
		break;
	case 1:
		if (OBJECT(current_object)->screen_x < -0x50)
			goto spiked_roller_chase_no_rumble;
		goto spiked_roller_chase_rumble;
	case 2:
		if (OBJECT(current_object)->screen_x < 0)
			goto spiked_roller_chase_no_rumble;
		goto spiked_roller_chase_rumble;
	case 3:
		if (OBJECT(current_object)->screen_x > 0x140)
			goto spiked_roller_chase_no_rumble;
		goto spiked_roller_chase_rumble;
	default:
		unknown_code(0x80678u + OBJECT(current_object)->spiked_roller.path_direction * 2u);
	}
	record_cursor = OBJECT(current_object)->spiked_roller.event_offset + spiked_roller_events_ADDRESS;
	event_position = ROM16(record_cursor);
	record_cursor += 2u;
	if (scroll_position < event_position || OBJECT(current_object)->spiked_roller.substate != 0u)
	{
		roller_ram_hit = 0u;
		return;
	}
	event_kind = ROM16(record_cursor) * 2u;
	record_cursor += 2u;
	switch (event_kind)
	{
	case 0x0u:
	case 0x8u:
		max_y = ROM16(record_cursor);
		record_cursor += 2u;
		if (OBJECT(current_object)->y_int <= (int16_t)max_y)
		{
			OBJECT(current_object)->spiked_roller.state = (uint8_t)ROM16(record_cursor);
			AS32(OBJECT(current_object)->spiked_roller.event_offset) += 8u;
		}
		break;
	case 0x2u:
		min_x = ROM16(record_cursor);
		record_cursor += 2u;
		if ((int16_t)min_x <= OBJECT(current_object)->x_int)
		{
			OBJECT(current_object)->spiked_roller.state = (uint8_t)ROM16(record_cursor);
			AS32(OBJECT(current_object)->spiked_roller.event_offset) += 8u;
			roller_ram_hit = 0u;
			return;
		}
		break;
	case 0x4u:
		min_y = ROM16(record_cursor);
		record_cursor += 2u;
		if ((int16_t)min_y <= OBJECT(current_object)->y_int)
		{
			OBJECT(current_object)->spiked_roller.state = (uint8_t)ROM16(record_cursor);
			AS32(OBJECT(current_object)->spiked_roller.event_offset) += 8u;
			roller_ram_hit = 0u;
			return;
		}
		break;
	case 0x6u:
		max_x = ROM16(record_cursor);
		record_cursor += 2u;
		if (OBJECT(current_object)->x_int <= (int16_t)max_x)
		{
			OBJECT(current_object)->spiked_roller.state = (uint8_t)ROM16(record_cursor);
			AS32(OBJECT(current_object)->spiked_roller.event_offset) += 8u;
			roller_ram_hit = 0u;
			return;
		}
		break;
	default:
		unknown_code(0x8071Bu + event_kind);
	}
	roller_ram_hit = 0u;
}
