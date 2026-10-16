/**
 *    Name:        scenes/ending.c
 *    Description: stage 0x11, the ending: the runners, the tower and the explosions, the flash to white, the credits (portraits, name plates, scrolling blocks), Eggman flying away and the three running across
 *    Version:     0.1.0
 *    Created:     12-10-2026
 *    Modified:    16-10-2026
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
	uint32_t dest = 0, ignored = 0, script = 0;
	uint8_t io_cnt_bits = 0;

	is_8bpp = (uint8_t)(ending_scene_step * 3u);
	switch (ending_scene_step)
	{
	case 0:
		io_cnt_bits = mem_read8(io_cnt);
		mem_write8(io_cnt, io_cnt_bits & ~2u);
		display_off_frames = 0x10u;
		color = set_stage_layer_mixing();
		clear_sprite_buckets();
		clear_bitmap_layer(is_8bpp, &ignored, &color, &ignored, &row_pairs_left, &blocks_per_row, &ignored);
		init_video_registers();
		load_stage_palette_set(&tile_bank_bits, &dest);
		sound_id = 0x94u;
		queue_sound(sound_id, &ignored, &write_index_address);
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
		run_vram_script(&ignored, &script, &dest);
		script = aerial_city_script_ADDRESS;
		run_vram_script(&ignored, &script, &dest);
		script = sonic_head_pattern_script_ADDRESS;
		run_vram_script(&ignored, &script, &dest);
		script = ending_eggman_machine_script_ADDRESS;
		run_vram_script(&ignored, &script, &dest);
		script = ending_logo_script_ADDRESS;
		run_vram_script(&ignored, &script, &dest);
		script = ending_bars_script_ADDRESS;
		run_vram_script(&ignored, &script, &dest);
		nbg0_pages = 0x8080707u;
		nbg1_pages = 0x1010101u;
		nbg2_pages = 0x5050505u;
		*p_write_index_address = write_index_address;
		*p_tile_bank_bits = tile_bank_bits;
		*p_color = color;
		*p_row_pairs_left = row_pairs_left;
		*p_blocks_per_row = blocks_per_row;
		*p_scene_actor = dest;
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
		invalid_jump(0x1BCD3u + ending_scene_step * 3u);
	}
}

void init_ending_scene_object(uint32_t object)
{
	OBJECT(object)->flags_low_byte = 0u;
	OBJECT(object)->step = 0u;
	OBJECT(object)->timer = 0;
	ending_scene_step = 2u;
}

void ending_tower_animate(void)
{
	uint32_t r11 = 0, frame_record = 0, ignored = 0, script = 0;

	if (OBJECT(current_object)->ending.started == 0u)
	{
		script = cloudy_horizon_script_ADDRESS;
		run_vram_script(&ignored, &script, &r11);
		mem_write16(video_layer_disable, mem_read16(video_layer_disable) & ~8u);
		nbg3_pages = 0u;
		OBJECT(current_object)->ending.list_index = 0u;
		OBJECT(current_object)->timer = 0;
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
				OBJECT(current_object)->timer++;
				run_timed_spawns_2(r11);
				return;
			}
			delete_current_object();
			return;
		}
		if (OBJECT(current_object)->timer < (int16_t)ROM16(frame_record))
			break;
		frame_record += 6u;
	}
	OBJECT(current_object)->sprite_frame = ROM16(frame_record + 2u);
	OBJECT(current_object)->sprite_palette = ROM16(frame_record + 4u);
	queue_sprite_in_layer();
	if (ROM16(frame_record + 4u) != 0xFFFFu)
	{
		OBJECT(current_object)->timer++;
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
		OBJECT(current_object)->timer = 0;
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
		OBJECT(current_object)->timer = 0;
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
		OBJECT(current_object)->timer = 0;
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
	OBJECT(current_object)->timer = 0;
}

void ending_big_explosion(void)
{
	uint8_t sound_id = 0;
	uint16_t speed_x = 0;
	uint32_t frame_record = 0, ignored = 0, second_frame_record = 0, second_object = 0;

	if (OBJECT(current_object)->ending.started == 0u)
	{
		OBJECT(current_object)->ending.frame_index = 0u;
		OBJECT(current_object)->timer = 0;
		OBJECT(current_object)->ending.started = 1u;
		OBJECT(current_object)->step = 0u;
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
	OBJECT(current_object)->timer++;
	if (OBJECT(current_object)->timer >= 6)
	{
		OBJECT(current_object)->timer = 0;
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
		OBJECT(current_object)->timer = 0;
	}
	OBJECT(current_object)->timer++;
	if (OBJECT(current_object)->timer >= 3)
	{
		color_offset_2_red++;
		color_offset_2_blue++;
		color_offset_2_green++;
		OBJECT(current_object)->timer = 0;
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
	OBJECT(current_object)->sprite_frame = ROM16(frame_entry);
	OBJECT(current_object)->sprite_layer = 6u;
	OBJECT(current_object)->sprite_flags = 0u;
	OBJECT(current_object)->sprite_scale = 0x100u;
	OBJECT(current_object)->sprite_palette = 0x10B0u;
	queue_sprite_in_layer();
	OBJECT(current_object)->timer++;
	if (OBJECT(current_object)->timer >= 6)
	{
		OBJECT(current_object)->draw_ending_blast_frame.frame_index++;
		OBJECT(current_object)->timer = 0;
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
	OBJECT(current_object)->sprite_frame = ROM16(frame_entry);
	OBJECT(current_object)->sprite_layer = 6u;
	OBJECT(current_object)->sprite_flags = 0u;
	OBJECT(current_object)->sprite_scale = 0x100u;
	OBJECT(current_object)->sprite_palette = 0x10B0u;
	queue_sprite_in_layer();
	OBJECT(current_object)->timer++;
	if (OBJECT(current_object)->timer >= 6)
	{
		OBJECT(current_object)->draw_ending_blast_frame.frame_index++;
		OBJECT(current_object)->timer = 0;
	}
}

void animate_ending_debris(uint32_t path)
{
	OBJECT(current_object)->animate_ending_debris.timer_low++;
	for (;;)
	{
		if ((uint16_t)DEBRIS_PATH_STEP(path)->end_time == 0xFFFFu)
		{
			delete_current_object();
			return;
		}
		if (OBJECT(current_object)->timer < DEBRIS_PATH_STEP(path)->end_time)
			break;
		path += 8u;
	}
	OBJECT(current_object)->sprite_palette = 0x10C0u;
	OBJECT(current_object)->sprite_layer = 6u;
	OBJECT(current_object)->screen_x = DEBRIS_PATH_STEP(path)->screen_x;
	OBJECT(current_object)->screen_y = DEBRIS_PATH_STEP(path)->screen_y;
	OBJECT(current_object)->sprite_frame = DEBRIS_PATH_STEP(path)->sprite_frame;
	OBJECT(current_object)->sprite_scale = 0x100u;
	queue_sprite_in_layer();
}

void run_timed_spawns_2(uint32_t object)
{
	uint32_t ignored = 0, spawn_record = 0;

	spawn_record = timed_spawns_2_ADDRESS + OBJECT(current_object)->run_timed_spawns.list_index * 4u;
	OBJECT(current_object)->z = ROM16(spawn_record + 2u);
	if (OBJECT(current_object)->z_fraction != 0xFFFFu && (int16_t)ROM16(spawn_record) <= OBJECT(current_object)->timer)
	{
		allocate_object_outside_loop(&ignored, &object);
		OBJECT(current_object)->z += 0x1BFC4u;
		OBJECT(object)->handler = OBJECT(current_object)->z;
		OBJECT(object)->flags_low_byte = 0u;
		OBJECT(current_object)->run_timed_spawns.list_index++;
	}
}

void ending_horizon_show(void)
{
	uint32_t ignored = 0, object = 0;

	if (OBJECT(current_object)->ending.started == 0u)
	{
		OBJECT(current_object)->ending.started++;
		OBJECT(current_object)->timer = 0;
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
	OBJECT(current_object)->timer++;
	if ((int16_t)OBJECT(current_object)->flags_low <= 1 && OBJECT(current_object)->timer >= 0x258)
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
		OBJECT(current_object)->timer = 0;
	}
	if (ending_first_scene_done == 1u)
	{
		delete_current_object();
		return;
	}
	OBJECT(current_object)->timer++;
	OBJECT(current_object)->sprite_layer = 6u;
	OBJECT(current_object)->screen_x = 0x9E;
	OBJECT(current_object)->sprite_scale = 0x100u;
	OBJECT(current_object)->sprite_flags = 0u;
	OBJECT(current_object)->sprite_palette = 0x10D0u;
	frame_pointer = OBJECT(current_object)->z;
	if ((uint16_t)DUST_CLOUD_FRAME(frame_pointer)->screen_y != 0xFFFFu && OBJECT(current_object)->timer >= 7)
	{
		OBJECT(current_object)->screen_y = DUST_CLOUD_FRAME(frame_pointer)->screen_y;
		frame_pointer += 2u;
		OBJECT(current_object)->sprite_frame = DUST_CLOUD_FRAME(frame_pointer - 0x2u)->sprite_frame;
		OBJECT(current_object)->z = frame_pointer + 2u;
		OBJECT(current_object)->timer = 0;
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
		OBJECT(current_object)->timer = 0;
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
		OBJECT(current_object)->timer = 0;
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
		OBJECT(current_object)->timer = 0;
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
		OBJECT(current_object)->timer = 0;
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
		OBJECT(current_object)->timer = 0;
		OBJECT(current_object)->ending.started = 1u;
		OBJECT(current_object)->step = 0u;
	}
	OBJECT(current_object)->timer++;
	if (OBJECT(current_object)->timer < 0x78)
		return;
	OBJECT(current_object)->timer = 0x79;
	if (OBJECT(current_object)->step == 0u)
	{
		allocate_object_outside_loop(&ignored, &object);
		OBJECT(object)->handler = ending_music_play_ADDRESS;
		OBJECT(object)->ending.started = 0u;
		OBJECT(current_object)->step = 1u;
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
		OBJECT(current_object)->timer = 0;
		OBJECT(current_object)->ending.list_index = 0u;
	}
	OBJECT(current_object)->timer++;
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
		OBJECT(current_object)->timer = 0;
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
	if ((int16_t)ROM16(cue_record) <= OBJECT(current_object)->timer)
	{
		sound_id = (uint8_t)ROM16(cue_record + 2u);
		queue_sound(sound_id, &ignored, &ignored);
		OBJECT(current_object)->ending.list_index++;
	}
	OBJECT(current_object)->timer++;
}

void ending_portrait_show(void)
{
	uint16_t speed_y = 0;
	uint32_t backdrop_object = 0, ignored = 0, palette_record = 0, timed_record = 0;

	if (OBJECT(current_object)->ending.started == 0u)
	{
		OBJECT(current_object)->ending.started = 1u;
		OBJECT(current_object)->timer = 0;
		allocate_object_2(&ignored, &ignored, &backdrop_object);
		OBJECT(current_object)->speed_y = backdrop_object;
	}
	OBJECT(current_object)->timer++;
	timed_record = ending_portrait_frames_ADDRESS;
	palette_record = ending_portrait_palettes_ADDRESS;
	while ((int16_t)ROM16(timed_record) < OBJECT(current_object)->timer)
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
	while ((int16_t)ROM16(timed_record) < OBJECT(current_object)->timer)
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
		OBJECT(current_object)->timer = 0;
		mem_write16(video_clip_1_left, 0xB1u);
		mem_write16(video_clip_1_right, 0x120u);
		mem_write16(video_clip_1_top, 0x21u);
		mem_write16(video_clip_1_bottom, 0x78u);
		allocate_object_2(&ignored, &ignored, &object);
		OBJECT(current_object)->z = object;
	}
	OBJECT(current_object)->timer++;
	frame_record = ending_name_plate_frames_ADDRESS;
	while ((int16_t)ROM16(frame_record) < OBJECT(current_object)->timer)
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
		OBJECT(current_object)->timer = 0;
		OBJECT(current_object)->step = 0u;
		OBJECT(current_object)->screen_y = 0xF4;
		OBJECT(current_object)->sprite_frame = 0x9850u;
		OBJECT(current_object)->sprite_palette = 0x1010u;
		init_credits_sprite();
		OBJECT(current_object)->speed_x = 0x1CC3D;
		for (;;)
		{
			field_cursor = OBJECT(object_cursor)->speed_x;
			if (CREDIT_PART(field_cursor)->sprite_frame == 0xFFFFu)
				break;
			allocate_object_2(&ignored, &ignored, &object);
			OBJECT(object)->sprite_frame = CREDIT_PART(field_cursor)->sprite_frame;
			field_cursor += 2u;
			OBJECT(object)->screen_y = CREDIT_PART(field_cursor - 0x2u)->screen_y;
			field_cursor += 2u;
			OBJECT(object)->sprite_palette = CREDIT_PART(field_cursor - 0x4u)->sprite_palette;
			field_cursor += 2u;
			current_object = (current_object & ~0xFFFFu) | (uint16_t)(current_object + CREDIT_PART(field_cursor - 0x6u)->slot);
			OBJECT(object_cursor)->speed_x = field_cursor + 2u;
			OBJECT(current_object)->credit_part_slot = object;
			current_object = object;
			init_credits_sprite();
		}
	}
	if (OBJECT(current_object)->step != 0u)
	{
		speed_y = (uint16_t)OBJECT(object_cursor)->speed_y;
		free_object_2(speed_y);
		delete_current_object();
		return;
	}
	OBJECT(object_cursor)->timer++;
	scroll_credits_sprite();
	current_object = OBJECT(current_object)->speed_y;
	scroll_credits_sprite();
	wrap_credits_scroll_timer();
	current_object = OBJECT(current_object)->speed_y;
	if (OBJECT(current_object)->screen_y <= -0x14)
	{
		OBJECT(object_cursor)->step = 1u;
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
	OBJECT(current_object)->timer = 0;
	OBJECT(current_object)->step = 0u;
	OBJECT(current_object)->screen_y = 0xF4;
	OBJECT(current_object)->sprite_frame = 0x97C8u;
	OBJECT(current_object)->sprite_palette = 0x1010u;
	init_credits_sprite();
	OBJECT(current_object)->speed_x = 0x1CD01;
	for (;;)
	{
		field_cursor = OBJECT(object_cursor)->speed_x;
		if (CREDIT_PART(field_cursor)->sprite_frame == 0xFFFFu)
			break;
		allocate_object_2(&ignored, &ignored, &object);
		OBJECT(object)->sprite_frame = CREDIT_PART(field_cursor)->sprite_frame;
		field_cursor += 2u;
		OBJECT(object)->screen_y = CREDIT_PART(field_cursor - 0x2u)->screen_y;
		field_cursor += 2u;
		OBJECT(object)->sprite_palette = CREDIT_PART(field_cursor - 0x4u)->sprite_palette;
		field_cursor += 2u;
		current_object = (current_object & ~0xFFFFu) | (uint16_t)(current_object + CREDIT_PART(field_cursor - 0x6u)->slot);
		OBJECT(object_cursor)->speed_x = field_cursor + 2u;
		OBJECT(current_object)->credit_part_slot = object;
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
	OBJECT(object_cursor)->timer++;
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
		if (OBJECT(current_object)->step == 0u)
		{
			OBJECT(object_cursor)->timer++;
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
				OBJECT(object_cursor)->step = 1u;
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
	OBJECT(current_object)->step = 0u;
	OBJECT(current_object)->timer = 0;
	OBJECT(current_object)->screen_y = 0xF4;
	OBJECT(current_object)->sprite_frame = 0x97C0u;
	OBJECT(current_object)->sprite_palette = 0x1010u;
	init_credits_sprite();
	OBJECT(current_object)->speed_x = 0x1CDEF;
	for (;;)
	{
		field_cursor = OBJECT(object_cursor)->speed_x;
		if (CREDIT_PART(field_cursor)->sprite_frame == 0xFFFFu)
			break;
		allocate_object_2(&ignored, &ignored, &object);
		OBJECT(object)->sprite_frame = CREDIT_PART(field_cursor)->sprite_frame;
		field_cursor += 2u;
		OBJECT(object)->screen_y = CREDIT_PART(field_cursor - 0x2u)->screen_y;
		field_cursor += 2u;
		OBJECT(object)->sprite_palette = CREDIT_PART(field_cursor - 0x4u)->sprite_palette;
		field_cursor += 2u;
		current_object = (current_object & ~0xFFFFu) | (uint16_t)(current_object + CREDIT_PART(field_cursor - 0x6u)->slot);
		OBJECT(object_cursor)->speed_x = field_cursor + 2u;
		OBJECT(current_object)->credit_part_slot = object;
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
		if (OBJECT(current_object)->step == 0u)
		{
			OBJECT(object_cursor)->timer++;
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
				OBJECT(object_cursor)->step = 1u;
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
	OBJECT(current_object)->step = 0u;
	OBJECT(current_object)->timer = 0;
	OBJECT(current_object)->screen_y = 0xF4;
	OBJECT(current_object)->sprite_frame = 0x97E8u;
	OBJECT(current_object)->sprite_palette = 0x1010u;
	init_credits_sprite();
	OBJECT(current_object)->speed_x = 0x1CEFE;
	for (;;)
	{
		field_cursor = OBJECT(object_cursor)->speed_x;
		if (CREDIT_PART(field_cursor)->sprite_frame == 0xFFFFu)
			break;
		allocate_object_2(&ignored, &ignored, &object);
		OBJECT(object)->sprite_frame = CREDIT_PART(field_cursor)->sprite_frame;
		field_cursor += 2u;
		OBJECT(object)->screen_y = CREDIT_PART(field_cursor - 0x2u)->screen_y;
		field_cursor += 2u;
		OBJECT(object)->sprite_palette = CREDIT_PART(field_cursor - 0x4u)->sprite_palette;
		field_cursor += 2u;
		current_object = (current_object & ~0xFFFFu) | (uint16_t)(current_object + CREDIT_PART(field_cursor - 0x6u)->slot);
		OBJECT(object_cursor)->speed_x = field_cursor + 2u;
		OBJECT(current_object)->credit_part_slot = object;
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
		if (OBJECT(current_object)->step == 0u)
		{
			OBJECT(object_cursor)->timer++;
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
				OBJECT(object_cursor)->step = 1u;
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
	OBJECT(current_object)->step = 0u;
	OBJECT(current_object)->timer = 0;
	OBJECT(current_object)->screen_y = 0xF4;
	OBJECT(current_object)->sprite_frame = 0x97E0u;
	OBJECT(current_object)->sprite_palette = 0x1010u;
	init_credits_sprite();
	OBJECT(current_object)->speed_x = 0x1D004;
	for (;;)
	{
		field_cursor = OBJECT(object_cursor)->speed_x;
		if (CREDIT_PART(field_cursor)->sprite_frame == 0xFFFFu)
			break;
		allocate_object_2(&ignored, &ignored, &object);
		OBJECT(object)->sprite_frame = CREDIT_PART(field_cursor)->sprite_frame;
		field_cursor += 2u;
		OBJECT(object)->screen_y = CREDIT_PART(field_cursor - 0x2u)->screen_y;
		field_cursor += 2u;
		OBJECT(object)->sprite_palette = CREDIT_PART(field_cursor - 0x4u)->sprite_palette;
		field_cursor += 2u;
		current_object = (current_object & ~0xFFFFu) | (uint16_t)(current_object + CREDIT_PART(field_cursor - 0x6u)->slot);
		OBJECT(object_cursor)->speed_x = field_cursor + 2u;
		OBJECT(current_object)->credit_part_slot = object;
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
		if (OBJECT(current_object)->step == 0u)
		{
			OBJECT(object_cursor)->timer++;
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
				OBJECT(object_cursor)->step = 1u;
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
	OBJECT(current_object)->step = 0u;
	OBJECT(current_object)->timer = 0;
	OBJECT(current_object)->screen_y = 0xF4;
	OBJECT(current_object)->sprite_frame = 0x97F0u;
	OBJECT(current_object)->sprite_palette = 0x1010u;
	init_credits_sprite();
	OBJECT(current_object)->speed_x = 0x1D113;
	for (;;)
	{
		field_cursor = OBJECT(object_cursor)->speed_x;
		if (CREDIT_PART(field_cursor)->sprite_frame == 0xFFFFu)
			break;
		allocate_object_2(&ignored, &ignored, &object);
		OBJECT(object)->sprite_frame = CREDIT_PART(field_cursor)->sprite_frame;
		field_cursor += 2u;
		OBJECT(object)->screen_y = CREDIT_PART(field_cursor - 0x2u)->screen_y;
		field_cursor += 2u;
		OBJECT(object)->sprite_palette = CREDIT_PART(field_cursor - 0x4u)->sprite_palette;
		field_cursor += 2u;
		current_object = (current_object & ~0xFFFFu) | (uint16_t)(current_object + CREDIT_PART(field_cursor - 0x6u)->slot);
		OBJECT(object_cursor)->speed_x = field_cursor + 2u;
		OBJECT(current_object)->credit_part_slot = object;
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
		if (OBJECT(current_object)->step == 0u)
		{
			OBJECT(object_cursor)->timer++;
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
				OBJECT(object_cursor)->step = 1u;
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
	OBJECT(current_object)->step = 0u;
	OBJECT(current_object)->timer = 0;
	OBJECT(current_object)->screen_y = 0xF4;
	OBJECT(current_object)->sprite_frame = 0x97D0u;
	OBJECT(current_object)->sprite_palette = 0x1010u;
	init_credits_sprite();
	OBJECT(current_object)->speed_x = 0x1D25D;
	for (;;)
	{
		field_cursor = OBJECT(object_cursor)->speed_x;
		if (CREDIT_PART(field_cursor)->sprite_frame == 0xFFFFu)
			break;
		allocate_object_2(&ignored, &ignored, &object);
		OBJECT(object)->sprite_frame = CREDIT_PART(field_cursor)->sprite_frame;
		field_cursor += 2u;
		OBJECT(object)->screen_y = CREDIT_PART(field_cursor - 0x2u)->screen_y;
		field_cursor += 2u;
		OBJECT(object)->sprite_palette = CREDIT_PART(field_cursor - 0x4u)->sprite_palette;
		field_cursor += 2u;
		current_object = (current_object & ~0xFFFFu) | (uint16_t)(current_object + CREDIT_PART(field_cursor - 0x6u)->slot);
		OBJECT(object_cursor)->speed_x = field_cursor + 2u;
		OBJECT(current_object)->credit_part_slot = object;
		current_object = object;
		init_credits_sprite();
	}
	scroll_credits_block();
}

void scroll_credits_sprite(void)
{
	if (OBJECT(object_cursor)->timer >= 2)
		OBJECT(current_object)->screen_y--;
	queue_sprite_in_layer();
	current_object = object_cursor;
}

void wrap_credits_scroll_timer(void)
{
	if (OBJECT(object_cursor)->timer >= 2)
		OBJECT(object_cursor)->timer = 0;
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
	if ((int16_t)ROM16(spawn_record) <= OBJECT(current_object)->timer)
	{
		allocate_object_outside_loop(&ignored, &object);
		OBJECT(current_object)->z += 0x1C82Fu;
		OBJECT(object)->handler = OBJECT(current_object)->z;
		OBJECT(object)->flags_low_byte = 0u;
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
	OBJECT(current_object)->sprite_frame = ROM16(frame_entry);
	OBJECT(current_object)->sprite_flags = 0x100u;
	queue_sprite_in_layer();
	OBJECT(current_object)->timer++;
	if ((OBJECT(current_object)->timer & 3u) == 2u)
		OBJECT(current_object)->sprite_scale++;
	if ((OBJECT(current_object)->timer & 1u) != 0u)
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
		OBJECT(current_object)->step = 0u;
	}
	nbg0_scroll_x_int++;
	mem_write16(video_clip_1_left, mem_read16(video_clip_1_left) - 1u);
	mem_write16(video_clip_1_right, mem_read16(video_clip_1_right) + 1u);
	mem_write16(video_clip_1_top, mem_read16(video_clip_1_top) - 1u);
	mem_write16(video_clip_1_bottom, mem_read16(video_clip_1_bottom) + 1u);
	if ((int16_t)mem_read16(video_clip_1_left) <= 0)
	{
		mem_write16(video_clip_1_left, 0u);
		OBJECT(current_object)->step |= 1u;
	}
	if ((int16_t)mem_read16(video_clip_1_top) <= 0)
	{
		mem_write16(video_clip_1_top, 0u);
		OBJECT(current_object)->step |= 2u;
	}
	if ((int16_t)mem_read16(video_clip_1_right) >= 0x140)
	{
		mem_write16(video_clip_1_right, 0x140u);
		OBJECT(current_object)->step |= 4u;
	}
	if ((int16_t)mem_read16(video_clip_1_bottom) >= 0xE0)
	{
		mem_write16(video_clip_1_bottom, 0xE0u);
		OBJECT(current_object)->step |= 8u;
	}
	if (OBJECT(current_object)->step != 0xFu)
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
	uint32_t ignored = 0, object = 0, script = 0;

	if (OBJECT(current_object)->ending.started != 0u)
	{
		update_fade(0u);
		color_offset_2_red = color_offset_red;
		color_offset_2_blue = color_offset_blue;
		color_offset_2_green = color_offset_green;
		if (fade_kind >= 0)
			return;
		allocate_object_outside_loop(&ignored, &object);
		OBJECT(object)->ending_eggman_fly_in.started = 0u;
		OBJECT(object)->handler = ending_eggman_fly_in_ADDRESS;
		delete_current_object();
		return;
	}
	mem_write16(video_layer_disable, mem_read16(video_layer_disable) | 0xBu);
	script = ending_stripes_script_ADDRESS;
	run_vram_script(&ignored, &script, &object);
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
	uint16_t exhaust_flame = 0, path = 0, puff_1 = 0, puff_2 = 0, puff_3 = 0, puff_4 = 0, puff_5 = 0, puff_6 = 0, puff_7 = 0, puff_8 = 0;
	uint32_t ignored = 0, object = 0, object_2 = 0, y_entry = 0;

	if (OBJECT(current_object)->ending_eggman_fly_in.started == 0u)
	{
		OBJECT(current_object)->ending_eggman_fly_in.started = 1u;
		OBJECT(current_object)->step = 0u;
		OBJECT(current_object)->screen_y = 0x70;
		OBJECT(current_object)->screen_x = -0x20;
		OBJECT(current_object)->ending_eggman_fly_in.path = ending_eggman_flight_path_ADDRESS;
		OBJECT(current_object)->timer = 0x1E0;
		allocate_object_2(&ignored, &ignored, &object);
		OBJECT(current_object)->ending_eggman_fly_in.exhaust_flame = object;
		allocate_object_2(&ignored, &ignored, &object);
		OBJECT(current_object)->ending_eggman_fly_in.puff_1 = object;
		init_eggman_smoke_puff(object);
		allocate_object_2(&ignored, &ignored, &object);
		OBJECT(current_object)->ending_eggman_fly_in.puff_2 = object;
		init_eggman_smoke_puff(object);
		allocate_object_2(&ignored, &ignored, &object);
		OBJECT(current_object)->ending_eggman_fly_in.puff_3 = object;
		init_eggman_smoke_puff(object);
		allocate_object_2(&ignored, &ignored, &object);
		OBJECT(current_object)->ending_eggman_fly_in.puff_4 = object;
		init_eggman_smoke_puff(object);
		allocate_object_2(&ignored, &ignored, &object);
		OBJECT(current_object)->ending_eggman_fly_in.puff_5 = object;
		init_eggman_smoke_puff(object);
		OBJECT(object)->sprite_frame = 0x9B80u;
		allocate_object_2(&ignored, &ignored, &object);
		OBJECT(current_object)->ending_eggman_fly_in.puff_6 = object;
		init_eggman_smoke_puff(object);
		OBJECT(object)->sprite_frame = 0x9B78u;
		allocate_object_2(&ignored, &ignored, &object);
		OBJECT(current_object)->ending_eggman_fly_in.puff_7 = object;
		init_eggman_smoke_puff(object);
		OBJECT(object)->sprite_frame = 0x9B70u;
		allocate_object_2(&ignored, &ignored, &object);
		OBJECT(current_object)->ending_eggman_fly_in.puff_8 = object;
		init_eggman_smoke_puff(object);
		OBJECT(object)->sprite_frame = 0x9B68u;
	}
	OBJECT(current_object)->timer--;
	if ((uint16_t)OBJECT(current_object)->timer == 0x104u)
	{
		sound_id = 0x75u;
		queue_sound(sound_id, &ignored, &ignored);
	}
	if ((uint16_t)OBJECT(current_object)->timer != 0u)
	{
		y_entry = OBJECT(current_object)->ending_eggman_fly_in.path;
		OBJECT(current_object)->screen_x = SCREEN_POINT(y_entry)->x;
		y_entry += 2u;
		OBJECT(current_object)->screen_y = SCREEN_POINT(y_entry - 0x2u)->y;
		OBJECT(current_object)->sprite_layer = 0u;
		OBJECT(current_object)->sprite_frame = 0x9AE0u;
		OBJECT(current_object)->sprite_palette = 0x1250u;
		OBJECT(current_object)->sprite_scale = 0x100u;
		OBJECT(current_object)->sprite_flags = 0u;
		OBJECT(current_object)->ending_eggman_fly_in.path = y_entry + 2u;
		queue_sprite_in_layer();
		if ((uint16_t)OBJECT(current_object)->screen_x != 0xA6u)
		{
			current_object = OBJECT(current_object)->ending_eggman_fly_in.exhaust_flame;
			OBJECT(current_object)->sprite_frame = 0x9AB0u;
			if ((OBJECT(object_cursor)->timer & 2u) != 0u)
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
			OBJECT(current_object)->ending_eggman_fly_in.path += 4u;
		current_object = object_cursor;
		if ((uint16_t)((int16_t)OBJECT(current_object)->timer % 0xA) == 0u)
		{
			object_2 = OBJECT(object_cursor)->ending_eggman_fly_in.puff_7;
			object = OBJECT(object_cursor)->ending_eggman_fly_in.puff_8;
			OBJECT(object)->screen_x = OBJECT(object_2)->screen_x;
			OBJECT(object)->screen_y = OBJECT(object_2)->screen_y;
			object_2 = OBJECT(object_cursor)->ending_eggman_fly_in.puff_6;
			object = OBJECT(object_cursor)->ending_eggman_fly_in.puff_7;
			OBJECT(object)->screen_x = OBJECT(object_2)->screen_x;
			OBJECT(object)->screen_y = OBJECT(object_2)->screen_y;
			object_2 = OBJECT(object_cursor)->ending_eggman_fly_in.puff_5;
			object = OBJECT(object_cursor)->ending_eggman_fly_in.puff_6;
			OBJECT(object)->screen_x = OBJECT(object_2)->screen_x;
			OBJECT(object)->screen_y = OBJECT(object_2)->screen_y;
			object_2 = OBJECT(object_cursor)->ending_eggman_fly_in.puff_4;
			object = OBJECT(object_cursor)->ending_eggman_fly_in.puff_5;
			OBJECT(object)->screen_x = OBJECT(object_2)->screen_x;
			OBJECT(object)->screen_y = OBJECT(object_2)->screen_y;
			object_2 = OBJECT(object_cursor)->ending_eggman_fly_in.puff_3;
			object = OBJECT(object_cursor)->ending_eggman_fly_in.puff_4;
			OBJECT(object)->screen_x = OBJECT(object_2)->screen_x;
			OBJECT(object)->screen_y = OBJECT(object_2)->screen_y;
			object_2 = OBJECT(object_cursor)->ending_eggman_fly_in.puff_2;
			object = OBJECT(object_cursor)->ending_eggman_fly_in.puff_3;
			OBJECT(object)->screen_x = OBJECT(object_2)->screen_x;
			OBJECT(object)->screen_y = OBJECT(object_2)->screen_y;
			object_2 = OBJECT(object_cursor)->ending_eggman_fly_in.puff_1;
			object = OBJECT(object_cursor)->ending_eggman_fly_in.puff_2;
			OBJECT(object)->screen_x = OBJECT(object_2)->screen_x;
			OBJECT(object)->screen_y = OBJECT(object_2)->screen_y;
			object = OBJECT(object_cursor)->ending_eggman_fly_in.puff_1;
			OBJECT(object)->screen_x = OBJECT(object_cursor)->screen_x;
			OBJECT(object)->screen_y = OBJECT(object_cursor)->screen_y;
			OBJECT(object)->screen_x -= 0x34u;
		}
		if (OBJECT(object_cursor)->timer > 0x32)
		{
			current_object = OBJECT(object_cursor)->ending_eggman_fly_in.puff_1;
			queue_sprite_in_layer();
			current_object = OBJECT(object_cursor)->ending_eggman_fly_in.puff_2;
			queue_sprite_in_layer();
			current_object = OBJECT(object_cursor)->ending_eggman_fly_in.puff_3;
			queue_sprite_in_layer();
			current_object = OBJECT(object_cursor)->ending_eggman_fly_in.puff_4;
			queue_sprite_in_layer();
			current_object = OBJECT(object_cursor)->ending_eggman_fly_in.puff_5;
			queue_sprite_in_layer();
			current_object = OBJECT(object_cursor)->ending_eggman_fly_in.puff_6;
			queue_sprite_in_layer();
			current_object = OBJECT(object_cursor)->ending_eggman_fly_in.puff_7;
			queue_sprite_in_layer();
			current_object = OBJECT(object_cursor)->ending_eggman_fly_in.puff_8;
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
	exhaust_flame = (uint16_t)OBJECT(current_object)->ending_eggman_fly_in.exhaust_flame;
	free_object_2(exhaust_flame);
	path = (uint16_t)OBJECT(current_object)->ending_eggman_fly_in.path;
	free_object_2(path);
	puff_1 = (uint16_t)OBJECT(current_object)->ending_eggman_fly_in.puff_1;
	free_object_2(puff_1);
	puff_2 = (uint16_t)OBJECT(current_object)->ending_eggman_fly_in.puff_2;
	free_object_2(puff_2);
	puff_3 = (uint16_t)OBJECT(current_object)->ending_eggman_fly_in.puff_3;
	free_object_2(puff_3);
	puff_4 = (uint16_t)OBJECT(current_object)->ending_eggman_fly_in.puff_4;
	free_object_2(puff_4);
	puff_5 = (uint16_t)OBJECT(current_object)->ending_eggman_fly_in.puff_5;
	free_object_2(puff_5);
	puff_6 = (uint16_t)OBJECT(current_object)->ending_eggman_fly_in.puff_6;
	free_object_2(puff_6);
	puff_7 = (uint16_t)OBJECT(current_object)->ending_eggman_fly_in.puff_7;
	free_object_2(puff_7);
	puff_8 = (uint16_t)OBJECT(current_object)->ending_eggman_fly_in.puff_8;
	free_object_2(puff_8);
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
		OBJECT(current_object)->timer = 0;
		OBJECT(current_object)->step = 0u;
	}
	OBJECT(current_object)->timer++;
	frame_record = ending_eggman_turn_frames_ADDRESS + OBJECT(current_object)->step * 4u;
	if (ROM16(frame_record) == 0xFFFFu)
	{
		allocate_object_outside_loop(&ignored, &object);
		OBJECT(object)->ending.started = 0u;
		OBJECT(object)->handler = ending_eggman_scene_fade_out_ADDRESS;
		delete_current_object();
		return;
	}
	if ((int16_t)ROM16(frame_record) < OBJECT(current_object)->timer)
		OBJECT(current_object)->step++;
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
	uint32_t cycle = 0, half_width_squared = 0, ignored = 0, left_entry = 0, lines_left = 0, object = 0, palette_count = 0, right_entry = 0, source = 0;

	if (OBJECT(current_object)->ending.started == 0u)
	{
		OBJECT(current_object)->ending.started = 1u;
		OBJECT(current_object)->step = 0u;
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
		queue_sound(sound_id, &ignored, &ignored);
	}
	object = PALETTE_RAM + 0x8BC0u;
	palette_count = 1u;
	source = ending_eggman_cycle_palettes_ADDRESS;
	halfwords = 0x10u;
	cycle = palette_cycle_0_ADDRESS;
	palette_cycle_0 = 6u;
	palette_cycle_0_frames = 9u;
	cycle_palette(cycle, halfwords, &ignored, &ignored, &palette_count, &source, &object);
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
	switch (OBJECT(object_cursor)->step)
	{
	case 0:
	case 7:
		sound_id = 9u;
		queue_sound(sound_id, &ignored, &ignored);
		nbg2_pages = 0x2020202u;
		OBJECT(object_cursor)->step = 1u;
		return;
	case 1:
		OBJECT(current_object)->sprite_scale += 5u;
		queue_sprite_in_layer();
		if ((int16_t)OBJECT(current_object)->sprite_scale >= 0x110)
			OBJECT(object_cursor)->step = 2u;
		return;
	case 2:
		OBJECT(current_object)->sprite_scale -= 5u;
		if ((int16_t)OBJECT(current_object)->sprite_scale >= 0)
		{
			queue_sprite_in_layer();
			return;
		}
		OBJECT(object_cursor)->step = 3u;
		return;
	case 3:
		speed_y = (uint16_t)OBJECT(object_cursor)->speed_y;
		free_object_2(speed_y);
		z = (uint16_t)OBJECT(object_cursor)->z;
		free_object_2(z);
		OBJECT(object_cursor)->step = 4u;
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
		OBJECT(object_cursor)->step = 5u;
		circle_wipe_radius_low = 0u;
		OBJECT(current_object)->cycle_count = 0u;
		fill_value = (fill_value & ~0xFFFFu) | 1u;
		set_halfwords(VIDEO_RAM + 0xA000u, 0x200u, fill_value);
		fill_value = fill_value & ~0xFFFFu;
		set_halfwords(VIDEO_RAM + 0xA400u, 0x200u, fill_value);
		return;
	case 5:
		OBJECT(current_object)->cycle_count++;
		if ((OBJECT(current_object)->cycle_count & 1u) != 0u)
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
			else if ((OBJECT(current_object)->cycle_count & 1u) != 0u)
			{
				if ((int16_t)((int32_t)(left_entry - (VIDEO_RAM + 0xA000u)) >> 1) <= (int16_t)(circle_wipe_radius + 0x70u))
				{
					circle_wipe_left = 0u;
					circle_wipe_right = 0x13Fu;
				}
				else
				{
					circle_wipe_left = 1u;
					circle_wipe_right = 0u;
				}
			}
			else if ((int16_t)(0x70u - circle_wipe_radius) <= (int16_t)((int32_t)(left_entry - (VIDEO_RAM + 0xA000u)) >> 1))
			{
				circle_wipe_left = 0u;
				circle_wipe_right = 0x13Fu;
			}
			else
			{
				circle_wipe_left = 1u;
				circle_wipe_right = 0u;
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
			OBJECT(object_cursor)->step = 6u;
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
		invalid_jump(0x1DB45u + OBJECT(object_cursor)->step * 3u);
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
	OBJECT(object_cursor)->ending.started = 1u;
	OBJECT(object_cursor)->timer = 0;
	OBJECT(object_cursor)->ending.cycle_count = 0;
	OBJECT(current_object)->screen_y = 0x68;
	OBJECT(current_object)->screen_x = 0;
	OBJECT(current_object)->sprite_palette |= 0x2000u;
	OBJECT(current_object)->sprite_flags = 0u;
	OBJECT(current_object)->sprite_flags |= 0x140u;
}

void animate_ending_runner_across(void)
{
	uint32_t second_frame_record = 0;

	OBJECT(object_cursor)->timer++;
	OBJECT(object_cursor)->cycle_count++;
	OBJECT(object_cursor)->timer &= 7u;
	OBJECT(object_cursor)->cycle_count &= 3u;
	second_frame_record = OBJECT(object_cursor)->animate_ending.leg_frames + OBJECT(object_cursor)->cycle_count * 2u;
	OBJECT(current_object)->sprite_frame = ROM16(OBJECT(object_cursor)->animate_ending.body_frames + ((uint16_t)OBJECT(object_cursor)->timer >> 2) * 2u);
	queue_sprite_in_layer();
	current_object = OBJECT(object_cursor)->animate_ending.legs;
	OBJECT(current_object)->sprite_frame = ROM16(second_frame_record);
	queue_sprite_in_layer();
	if (OBJECT(object_cursor)->screen_x >= 0x300)
	{
		OBJECT(current_object)->screen_x = 0;
		OBJECT(object_cursor)->screen_x = 0;
	}
}
