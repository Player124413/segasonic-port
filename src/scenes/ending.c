/**
 *    Name:        scenes/ending.c
 *    Description: stage 0x11, the ending: the scene set-up and the first scene (the runners, the tower, the explosions, the flash to white)
 *    Version:     0.1.0
 *    Created:     12-10-2026
 *    Modified:    15-10-2026
 *    Author:      Sebastian Toyos
 *
 *    SegaSonic The Hedgehog (arcade, Sega System 32): the game's V60 program decompiled into C by
 *    tools/decompile.py, with the names of symbols.txt.
 */
#include "game.h"

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
