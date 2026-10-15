/**
 *    Name:        stage/transitions.c
 *    Description: start_stage_view, stage 5's switch to stage 0, the fades to and from black or white (update_fade), stage 0's exit scene
 *    Version:     0.1.0
 *    Created:     12-10-2026
 *    Modified:    15-10-2026
 *    Author:      Sebastian Toyos
 *
 *    SegaSonic The Hedgehog (arcade, Sega System 32): the game's V60 program decompiled into C by
 *    tools/decompile.py, with the names of symbols.txt.
 */
#include "game.h"

void start_stage_view(uint32_t *p_direction_y, uint32_t *p_direction_x, uint32_t *p_count, uint32_t *p_stage_offset)
{
	uint32_t direction_y = *p_direction_y;
	uint32_t direction_x = *p_direction_x;
	uint32_t count = *p_count;
	uint32_t stage_offset = *p_stage_offset;

	if (stage != 0x11u)
	{
		unused_scroll_flag = 1u;
		init_stage_view(&direction_y, &direction_x, &count, &stage_offset);
		*p_direction_y = direction_y;
		*p_direction_x = direction_x;
		*p_count = count;
		*p_stage_offset = stage_offset;
		return;
	}
	ending_scene_step = 0u;
}

void call_run_map_screen(uint32_t object)
{
	run_map_screen(0u, object);
}

void run_stage_05_switch(uint32_t speed, uint32_t *p_scroll_y, uint32_t *p_speed_value, uint32_t *p_dest)
{
	uint32_t scroll_y = *p_scroll_y;
	uint32_t speed_value = *p_speed_value;
	uint32_t dest = *p_dest;
	uint32_t r1 = 0, camera_view_y = 0, ignored = 0, layer_scroll_x = 0, next_scroll_position = 0, script = 0;

	r1 = (uint8_t)stage_05_switch_step * 3u;
	switch ((uint8_t)stage_05_switch_step)
	{
	case 0:
	case 4:
		mixer_text_blend |= 0x4000u;
		mixer_nbg0_blend |= 0x4000u;
		mixer_nbg1_blend |= 0x4000u;
		mixer_nbg2_blend |= 0x4000u;
		mixer_nbg3_blend |= 0x4000u;
		mixer_bitmap_blend |= 0x4000u;
		mixer_sprite_control |= 0x8000u;
		stage_05_switch_step++;
		return;
	case 1:
		fade_kind = 0;
		update_fade(r1);
		if (fade_kind >= 0)
			return;
		stage_05_switch_step++;
		fade_step = 0u;
		return;
	case 2:
		mem_write8(video_layer_disable, 0x3Fu);
		script = stage_00_coast_script_ADDRESS;
		run_vram_script(&r1, &script, &dest);
		script = stage_00_horizon_script_ADDRESS;
		run_vram_script(&r1, &script, &dest);
		script = blue_sky_script_ADDRESS;
		run_vram_script(&r1, &script, &dest);
		script = aerial_city_script_ADDRESS;
		run_vram_script(&r1, &script, &dest);
		script = stage_00_tower_top_page_1_script_ADDRESS;
		run_vram_script(&r1, &script, &dest);
		script = stage_00_tower_top_page_3_script_ADDRESS;
		run_vram_script(&r1, &script, &dest);
		script = stage_00_tower_top_page_0_script_ADDRESS;
		run_vram_script(&r1, &script, &dest);
		script = stage_00_tower_top_page_2_script_ADDRESS;
		run_vram_script(&r1, &script, &dest);
		nbg3_pages = 0x7070707u;
		camera_x = 0xF800000;
		camera_y = 0x6700000;
		camera_view_y = update_camera_view();
		layer_scroll_x = (camera_view_y & ~0xFFFFu) | camera_x_pixels;
		nbg0_scroll_y_int = camera_y_pixels;
		nbg0_scroll_x_int = layer_scroll_x;
		nbg0_scroll_x_int &= 0x3FFu;
		nbg0_scroll_y_int &= 0x1FFu;
		stage = 0u;
		update_background_scroll(&ignored, &layer_scroll_x, &speed, &speed_value, &next_scroll_position, &ignored, &dest);
		stage = 5u;
		mem_write8(video_layer_disable, mem_read8(video_layer_disable) & 0xE6u);
		stage_05_switch_step++;
		*p_scroll_y = layer_scroll_x;
		*p_speed_value = speed_value;
		*p_dest = dest;
		return;
	case 3:
		fade_kind = 2;
		update_fade(r1);
		if (fade_kind >= 0)
			return;
		stage_05_switch_step = -1;
		scroll_position = 0x5E5Au;
		stage = 0u;
		mixer_text_blend &= 0xBFFFu;
		mixer_nbg0_blend &= 0xBFFFu;
		mixer_nbg1_blend &= 0xBFFFu;
		mixer_nbg2_blend &= 0xBFFFu;
		mixer_nbg3_blend &= 0xBFFFu;
		mixer_bitmap_blend &= 0xBFFFu;
		mixer_sprite_control &= ~0x8000u;
		set_nbg3_page_5_flags(&ignored, &dest);
		loaded_tile_sets = 0x32A03u;
		*p_dest = dest;
		return;
	default:
		unknown_code(0x16D16u + (uint8_t)stage_05_switch_step * 3u);
	}
}

uint32_t update_fade(uint32_t delta)
{
	uint32_t jump_offset = 0;

	jump_offset = fade_step * 3u;
	switch (fade_step)
	{
	case 0:
	case 3:
		fade_kind &= 3u;
		switch ((jump_offset & ~0xFFu) | (uint8_t)fade_kind)
		{
		case 0:
		case 4:
			color_offset_red = 0x3Fu;
			color_offset_blue = 0x3Fu;
			color_offset_green = 0x3Fu;
			delta = (delta & ~0xFFFFu) | fade_speed;
			delta = (delta & ~0xFFFFu) | (uint16_t)(0u - delta);
			fade_delta = delta;
			break;
		case 1:
			color_offset_red = 0x1Fu;
			color_offset_blue = 0x1Fu;
			color_offset_green = 0x1Fu;
			delta = (delta & ~0xFFFFu) | fade_speed;
			delta = (delta & ~0xFFFFu) | (uint16_t)(0u - delta);
			fade_delta = delta;
			break;
		case 2:
			color_offset_red = 0x20u;
			color_offset_blue = 0x20u;
			color_offset_green = 0x20u;
			delta = (delta & ~0xFFFFu) | fade_speed;
			fade_delta = delta;
			break;
		case 3:
			color_offset_red = 0u;
			color_offset_blue = 0u;
			color_offset_green = 0u;
			delta = (delta & ~0xFFFFu) | fade_speed;
			fade_delta = delta;
			break;
		default:
			unknown_code(0x16EF2u + ((jump_offset & ~0xFFu) | (uint8_t)fade_kind) * 3u);
		}
		AS16(fade_delay) = 1u;
		fade_step++;
		return delta;
	case 1:
		AS16(fade_delay)--;
		if (AS16(fade_delay) != 0u)
			return delta;
		AS16(fade_delay) = 1u;
		color_offset_red += fade_delta;
		color_offset_blue += fade_delta;
		color_offset_green += fade_delta;
		switch ((jump_offset & ~0xFFu) | (uint8_t)fade_kind)
		{
		case 0:
		case 4:
			if ((int16_t)color_offset_red <= 0x20)
			{
				fade_step++;
				color_offset_red = 0x20u;
				color_offset_blue = 0x20u;
				color_offset_green = 0x20u;
			}
			break;
		case 1:
			if ((int16_t)color_offset_red <= 0)
			{
				fade_step++;
				color_offset_red = 0u;
				color_offset_blue = 0u;
				color_offset_green = 0u;
			}
			break;
		case 2:
			if ((int16_t)color_offset_red >= 0x3F)
			{
				fade_step++;
				color_offset_red = 0x3Fu;
				color_offset_blue = 0x3Fu;
				color_offset_green = 0x3Fu;
			}
			break;
		case 3:
			if ((int16_t)color_offset_red >= 0x1F)
			{
				fade_step++;
				color_offset_red = 0x1Fu;
				color_offset_blue = 0x1Fu;
				color_offset_green = 0x1Fu;
			}
			break;
		default:
			unknown_code(0x16FD3u + ((jump_offset & ~0xFFu) | (uint8_t)fade_kind) * 3u);
		}
		return delta;
	case 2:
		fade_kind = -1;
		return delta;
	default:
		unknown_code(0x16ED6u + fade_step * 3u);
	}
}

void update_stage_00_exit_scene(uint32_t dest, uint32_t *p_speed_value, uint32_t *p_script)
{
	uint32_t speed_value = *p_speed_value;
	uint32_t script = *p_script;
	uint16_t layer_zoom = 0;
	uint32_t r10 = 0, ignored = 0, layer_scroll_x = 0, next_scroll_position = 0, scroll_y_past_trigger = 0, speed = 0;
	uint16_t scroll_y_offset = 0;

	if ((int8_t)stage_00_exit_step < 2)
	{
		if (stage_00_exit_step == 0u)
		{
			scroll_speed = 0x35u;
			scroll_y_past_trigger = (uint16_t)(nbg0_scroll_y_int - 0x20u);
			if ((int16_t)scroll_y_past_trigger < 8 && (int16_t)scroll_y_past_trigger >= 0)
			{
				script = stage_00_exit_tower_wall_script_ADDRESS;
				run_vram_script(&scroll_y_past_trigger, &script, &dest);
				script = clear_tile_page_03_script_ADDRESS;
				run_vram_script(&scroll_y_past_trigger, &script, &dest);
				stage_00_exit_step++;
			}
		}
		if ((int8_t)stage_00_exit_step < 2)
		{
			scroll_y_offset = nbg0_scroll_y_int - 0xF6u;
			scroll_y_past_trigger = scroll_y_offset;
			if ((int16_t)scroll_y_offset >= 0 && (int16_t)scroll_y_past_trigger < 8)
			{
				nbg0_pages = 0x2020202u;
				script = stage_00_exit_beam_script_ADDRESS;
				run_vram_script(&scroll_y_past_trigger, &script, &dest);
				script = stage_00_exit_beam_end_script_ADDRESS;
				run_vram_script(&scroll_y_past_trigger, &script, &dest);
				mixer_text_blend &= 0xBFFFu;
				mixer_nbg0_blend |= 0x4000u;
				mixer_nbg1_blend |= 0x4000u;
				mixer_nbg2_blend |= 0x4000u;
				mixer_nbg3_blend |= 0x4000u;
				mixer_bitmap_blend &= 0xBFFFu;
				mixer_sprite_control |= 0x8000u;
				color_offset_red = 0u;
				color_offset_blue = 0u;
				color_offset_green = 0u;
				stage_00_exit_step++;
			}
		}
	}
	switch (stage_00_exit_step)
	{
	case 0:
	case 1:
	case 2:
	case 11:
		if (stage_00_exit_timer >= 0x8C)
		{
			nbg0_pages = 0x3020100u;
			stage_00_exit_step++;
			*p_script = script;
			return;
		}
		stage_00_exit_timer++;
		speed = scroll_speed;
		section_limit_y = -1;
		section_direction_y = -0x4000;
		section_direction_x = 0;
		advance_camera_y(speed);
		update_camera_view();
		layer_scroll_x = camera_x_pixels;
		nbg0_scroll_y_int = camera_y_pixels;
		nbg0_scroll_x_int = layer_scroll_x;
		nbg0_scroll_x_int &= 0x3FFu;
		nbg0_scroll_y_int &= 0x1FFu;
		update_background_scroll(&ignored, &layer_scroll_x, &speed, &speed_value, &next_scroll_position, &r10, &dest);
		*p_speed_value = speed_value;
		*p_script = r10;
		return;
	case 3:
		if (stage_00_exit_timer >= 0xA0)
		{
			nbg0_pages = 0x3030100u;
			stage_00_exit_step++;
			*p_script = script;
			return;
		}
		stage_00_exit_timer++;
		speed = scroll_speed;
		section_limit_y = -1;
		section_direction_y = -0x4000;
		section_direction_x = 0;
		advance_camera_y(speed);
		update_camera_view();
		layer_scroll_x = camera_x_pixels;
		nbg0_scroll_y_int = camera_y_pixels;
		nbg0_scroll_x_int = layer_scroll_x;
		nbg0_scroll_x_int &= 0x3FFu;
		nbg0_scroll_y_int &= 0x1FFu;
		update_background_scroll(&ignored, &layer_scroll_x, &speed, &speed_value, &next_scroll_position, &r10, &dest);
		*p_speed_value = speed_value;
		*p_script = r10;
		return;
	case 4:
		if (stage_00_exit_timer >= 0xAF)
		{
			nbg0_pages = 0x3030303u;
			stage_00_exit_step++;
			*p_script = script;
			return;
		}
		stage_00_exit_timer++;
		speed = scroll_speed;
		section_limit_y = -1;
		section_direction_y = -0x4000;
		section_direction_x = 0;
		advance_camera_y(speed);
		update_camera_view();
		layer_scroll_x = camera_x_pixels;
		nbg0_scroll_y_int = camera_y_pixels;
		nbg0_scroll_x_int = layer_scroll_x;
		nbg0_scroll_x_int &= 0x3FFu;
		nbg0_scroll_y_int &= 0x1FFu;
		update_background_scroll(&ignored, &layer_scroll_x, &speed, &speed_value, &next_scroll_position, &r10, &dest);
		*p_speed_value = speed_value;
		*p_script = r10;
		return;
	case 5:
		scroll_speed--;
		if ((int16_t)scroll_speed <= 2)
			scroll_speed = 2u;
		if (stage_00_exit_timer >= 0xD2)
		{
			stage_00_exit_step++;
			*p_script = script;
			return;
		}
		stage_00_exit_timer++;
		speed = scroll_speed;
		section_limit_y = -1;
		section_direction_y = -0x4000;
		section_direction_x = 0;
		advance_camera_y(speed);
		update_camera_view();
		layer_scroll_x = camera_x_pixels;
		nbg0_scroll_y_int = camera_y_pixels;
		nbg0_scroll_x_int = layer_scroll_x;
		nbg0_scroll_x_int &= 0x3FFu;
		nbg0_scroll_y_int &= 0x1FFu;
		update_background_scroll(&ignored, &layer_scroll_x, &speed, &speed_value, &next_scroll_position, &r10, &dest);
		*p_speed_value = speed_value;
		*p_script = r10;
		return;
	case 6:
		clamp_colour_offsets();
		stage_00_exit_delay++;
		if (stage_00_exit_delay != 5u)
		{
			stage_00_exit_timer++;
			speed = scroll_speed;
			section_limit_y = -1;
			section_direction_y = -0x4000;
			section_direction_x = 0;
			advance_camera_y(speed);
			update_camera_view();
			layer_scroll_x = camera_x_pixels;
			nbg0_scroll_y_int = camera_y_pixels;
			nbg0_scroll_x_int = layer_scroll_x;
			nbg0_scroll_x_int &= 0x3FFu;
			nbg0_scroll_y_int &= 0x1FFu;
			update_background_scroll(&ignored, &layer_scroll_x, &speed, &speed_value, &next_scroll_position, &r10, &dest);
			*p_speed_value = speed_value;
			*p_script = r10;
			return;
		}
		stage_00_exit_delay = 0u;
		color_offset_red++;
		color_offset_blue++;
		color_offset_green++;
		scroll_speed--;
		if ((int16_t)scroll_speed <= 2)
			scroll_speed = 2u;
		if ((int16_t)color_offset_red < 0x1F)
		{
			stage_00_exit_timer++;
			speed = scroll_speed;
			section_limit_y = -1;
			section_direction_y = -0x4000;
			section_direction_x = 0;
			advance_camera_y(speed);
			update_camera_view();
			layer_scroll_x = camera_x_pixels;
			nbg0_scroll_y_int = camera_y_pixels;
			nbg0_scroll_x_int = layer_scroll_x;
			nbg0_scroll_x_int &= 0x3FFu;
			nbg0_scroll_y_int &= 0x1FFu;
			update_background_scroll(&ignored, &layer_scroll_x, &speed, &speed_value, &next_scroll_position, &r10, &dest);
			*p_speed_value = speed_value;
			*p_script = r10;
			return;
		}
		nbg0_scroll_y_int = 0x58u;
		nbg0_scroll_x_int = 0xB8u;
		nbg1_scroll_y_int = 0x58u;
		nbg1_scroll_x_int = 0xB8u;
		nbg0_center_x = 0xA0u;
		nbg0_center_y = 0x90u;
		nbg1_center_x = 0xA0u;
		nbg1_center_y = 0x90u;
		mem_write32(video_layer_disable, mem_read32(video_layer_disable) | 1u);
		mem_write32(video_layer_disable, mem_read32(video_layer_disable) | 2u);
		stage_00_exit_step++;
		*p_script = script;
		return;
	case 7:
		nbg0_pages = 0x6060505u;
		video_layer_opaque_shadow = 0xBu;
		stage_00_exit_step++;
		*p_script = script;
		return;
	case 8:
		mem_write32(video_layer_disable, mem_read32(video_layer_disable) & ~1u);
		mem_write32(video_layer_disable, mem_read32(video_layer_disable) & ~2u);
		mem_write32(video_layer_disable, mem_read32(video_layer_disable) | 8u);
		mem_write32(video_control, mem_read32(video_control) | 0x1000u);
		mem_write32(video_control, mem_read32(video_control) | 0x2000u);
		mem_write32(video_control, mem_read32(video_control) | 0x4000u);
		mem_write16(nbg0_zoom_x, 0x100u);
		mem_write16(nbg0_zoom_y, 0x100u);
		mem_write16(nbg1_zoom_x, 0x100u);
		mem_write16(nbg1_zoom_y, 0x100u);
		script = stage_00_exit_zoom_page_0_script_ADDRESS;
		run_vram_script(&scroll_y_past_trigger, &script, &dest);
		script = stage_00_exit_zoom_page_1_script_ADDRESS;
		run_vram_script(&scroll_y_past_trigger, &script, &dest);
		script = stage_00_exit_zoom_page_2_script_ADDRESS;
		run_vram_script(&scroll_y_past_trigger, &script, &dest);
		mem_write16(backdrop_color, 0x1356u);
		nbg0_pages = 0x2020200u;
		nbg1_pages = 0x2020201u;
		stage_00_exit_step++;
		stage_00_exit_zoom = 0x200u;
		*p_script = script;
		return;
	case 9:
		stage_00_exit_zoom--;
		layer_zoom = (uint16_t)(divide_s32(0x2000000u, stage_00_exit_zoom) >> 8);
		mem_write16(nbg0_zoom_x, layer_zoom);
		mem_write16(nbg0_zoom_y, layer_zoom);
		mem_write16(nbg1_zoom_x, layer_zoom);
		mem_write16(nbg1_zoom_y, layer_zoom);
		clamp_colour_offsets();
		stage_00_exit_delay++;
		if (stage_00_exit_delay != 3u)
		{
			update_background_scroll(&ignored, &layer_scroll_x, &speed, &speed_value, &next_scroll_position, &r10, &dest);
			*p_speed_value = speed_value;
			*p_script = r10;
			return;
		}
		stage_00_exit_delay = 0u;
		color_offset_red--;
		color_offset_blue--;
		color_offset_green--;
		if (color_offset_red != 0u)
		{
			update_background_scroll(&ignored, &layer_scroll_x, &speed, &speed_value, &next_scroll_position, &r10, &dest);
			*p_speed_value = speed_value;
			*p_script = r10;
			return;
		}
		stage_00_exit_step++;
		*p_script = script;
		return;
	case 10:
		update_background_scroll(&ignored, &layer_scroll_x, &speed, &speed_value, &next_scroll_position, &r10, &dest);
		if ((int16_t)stage_00_exit_zoom < 0x42)
			stage_00_exit_zoom = 0x42u;
		stage_00_exit_zoom--;
		layer_zoom = (uint16_t)(divide_s32(0x2000000u, stage_00_exit_zoom) >> 8);
		mem_write16(nbg0_zoom_x, layer_zoom);
		mem_write16(nbg0_zoom_y, layer_zoom);
		mem_write16(nbg1_zoom_x, layer_zoom);
		mem_write16(nbg1_zoom_y, layer_zoom);
		*p_speed_value = speed_value;
		*p_script = r10;
		return;
	default:
		unknown_code(0x17160u + stage_00_exit_step * 3u);
	}
}

void clamp_colour_offsets(void)
{
	color_offset_red &= 0x1Fu;
	color_offset_blue &= 0x1Fu;
	color_offset_green &= 0x1Fu;
}
