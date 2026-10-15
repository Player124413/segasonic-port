/**
 *    Name:        stage/camera.c
 *    Description: the camera: shake, views, the move along the section path; stage 6's background in its opening scene and stage B's escape alarm flash
 *    Version:     0.1.0
 *    Created:     12-10-2026
 *    Modified:    15-10-2026
 *    Author:      Sebastian Toyos
 *
 *    SegaSonic The Hedgehog (arcade, Sega System 32): the game's V60 program decompiled into C by
 *    tools/decompile.py, with the names of symbols.txt.
 */
#include "game.h"

uint32_t update_camera_with_shake(void)
{
	uint32_t r2 = 0, shaken_y = 0, wave_table = 0;

	r2 = update_camera_view();
	if (camera_shake_active == 0u)
		return r2;
	switch (camera_shake_phase)
	{
	case 0:
	case 2:
		camera_shake_phase++;
		break;
	case 1:
		break;
	default:
		unknown_code(0x17610u + camera_shake_phase * 3u);
	}
	camera_shake_base_y = (uint32_t)(int16_t)camera_y_pixels << 16;
	camera_shake_frame++;
	wave_table = 0x100000u | (uint16_t)(0x1632u + camera_shake_wave);
	shaken_y = (int16_t)mem_read16((wave_table & ~0xFFFFu) | (uint16_t)(wave_table + camera_shake_step));
	camera_shake_step += 0xAu;
	shaken_y = shaken_y * 0x20u + camera_shake_base_y;
	if ((camera_shake_frame & 1u) != 0u)
		shaken_y = camera_shake_base_y;
	r2 = shaken_y;
	r2 = update_camera_view_y(r2);
	r2 = set_nbg0_scroll_from_camera(r2);
	if ((int16_t)camera_shake_length <= (int16_t)camera_shake_step)
	{
		camera_shake_active = 0u;
		AS16(camera_shake_frame) = 0u;
		camera_shake_step = 0u;
		camera_shake_phase = 0u;
	}
	return r2;
}

uint32_t set_nbg0_scroll_from_camera(uint32_t scroll_x)
{
	uint16_t layer_scroll_y = 0;
	uint16_t block_column = 0, block_row = 0;

	layer_scroll_y = camera_y_pixels;
	scroll_x = (scroll_x & ~0xFFFFu) | camera_x_pixels;
	nbg0_scroll_y_int = layer_scroll_y;
	nbg0_scroll_x_int = scroll_x;
	tile_block_row = layer_scroll_y;
	tile_block_column = scroll_x;
	tile_block_column &= ~0x1FFu;
	tile_block_row &= ~0xFFu;
	nbg0_scroll_x_int &= 0x3FFu;
	nbg0_scroll_y_int &= 0x1FFu;
	block_column = tile_block_column;
	tile_block_column = ((uint32_t)block_column << 7) | (block_column >> 9);
	block_row = tile_block_row;
	tile_block_row = ((uint32_t)block_row << 8) | (block_row >> 8);
	tile_block_row = (uint32_t)tile_block_row * stage_block_map_width;
	return scroll_x;
}

uint32_t update_camera_view(void)
{
	uint32_t y = 0;

	camera_x_pixels = (uint32_t)camera_x >> 16;
	view_x = camera_x_pixels - view_offset_x;
	y = camera_y;
	return update_camera_view_y(y);
}

uint32_t update_camera_view_y(uint32_t y)
{
	y = y >> 16;
	camera_y_pixels = y;
	y = camera_y_pixels;
	y = (uint16_t)(y - view_offset_y);
	view_y = y;
	return y;
}

uint32_t move_camera_along_path(uint32_t speed)
{
	uint16_t next_offset = 0;
	uint32_t current_position = 0, limit_list = 0;

	camera_limits_reached = 0u;
	if ((uint32_t)section_direction_y == 0u)
		camera_limits_reached |= 4u;
	else
	{
		next_offset = (uint16_t)(((int32_t)(section_direction_y * speed + camera_y) >> 16) - camera_origin_y);
		limit_list = section_limit_y_list;
		if ((int16_t)ROM16(limit_list - 4u) < (int16_t)ROM16(limit_list - 2u))
		{
			if (section_limit_y <= (int16_t)next_offset)
			{
				camera_limits_reached |= 4u;
				if ((uint32_t)section_direction_x == 0u)
					camera_limits_reached |= 2u;
				else
				{
					next_offset = (uint16_t)(((int32_t)(section_direction_x * speed + camera_x) >> 16) - camera_origin_x);
					limit_list = section_limit_x_list;
					if ((int16_t)ROM16(limit_list - 4u) < (int16_t)ROM16(limit_list - 2u))
					{
						if (section_limit_x <= (int16_t)next_offset)
						{
							camera_limits_reached |= 2u;
							current_position = scroll_position;
							current_position = (uint16_t)(current_position + speed);
							if ((int32_t)ROM16(section_end_list) < (int32_t)current_position)
								camera_limits_reached |= 1u;
							switch (camera_limits_reached)
							{
							case 0:
								advance_camera_scroll_x_y(speed);
								break;
							case 1:
								advance_camera_x_y(speed);
								break;
							case 2:
								advance_camera_y_scroll(speed);
								break;
							case 3:
								advance_camera_y(speed);
								break;
							case 4:
								advance_camera_x_scroll(speed);
								break;
							case 5:
								advance_camera_x(speed);
								break;
							case 6:
								advance_scroll_position((uint16_t)speed);
								break;
							case 7:
								snap_camera_to_section_end();
								break;
							case 8:
								advance_camera_scroll_x_y(speed);
								break;
							default:
								unknown_code(0x177FDu + camera_limits_reached * 3u);
							}
							return current_position;
						}
					}
					else if ((int16_t)next_offset <= section_limit_x)
						camera_limits_reached |= 2u;
				}
				current_position = scroll_position;
				current_position = (uint16_t)(current_position + speed);
				if ((int32_t)ROM16(section_end_list) < (int32_t)current_position)
					camera_limits_reached |= 1u;
				switch (camera_limits_reached)
				{
				case 0:
					advance_camera_scroll_x_y(speed);
					break;
				case 1:
					advance_camera_x_y(speed);
					break;
				case 2:
					advance_camera_y_scroll(speed);
					break;
				case 3:
					advance_camera_y(speed);
					break;
				case 4:
					advance_camera_x_scroll(speed);
					break;
				case 5:
					advance_camera_x(speed);
					break;
				case 6:
					advance_scroll_position((uint16_t)speed);
					break;
				case 7:
					snap_camera_to_section_end();
					break;
				case 8:
					advance_camera_scroll_x_y(speed);
					break;
				default:
					unknown_code(0x177FDu + camera_limits_reached * 3u);
				}
				return current_position;
			}
		}
		else if ((int16_t)next_offset <= section_limit_y)
			camera_limits_reached |= 4u;
	}
	if ((uint32_t)section_direction_x == 0u)
		camera_limits_reached |= 2u;
	else
	{
		next_offset = (uint16_t)(((int32_t)(section_direction_x * speed + camera_x) >> 16) - camera_origin_x);
		limit_list = section_limit_x_list;
		if ((int16_t)ROM16(limit_list - 4u) < (int16_t)ROM16(limit_list - 2u))
		{
			if (section_limit_x <= (int16_t)next_offset)
			{
				camera_limits_reached |= 2u;
				current_position = scroll_position;
				current_position = (uint16_t)(current_position + speed);
				if ((int32_t)ROM16(section_end_list) < (int32_t)current_position)
					camera_limits_reached |= 1u;
				switch (camera_limits_reached)
				{
				case 0:
					advance_camera_scroll_x_y(speed);
					break;
				case 1:
					advance_camera_x_y(speed);
					break;
				case 2:
					advance_camera_y_scroll(speed);
					break;
				case 3:
					advance_camera_y(speed);
					break;
				case 4:
					advance_camera_x_scroll(speed);
					break;
				case 5:
					advance_camera_x(speed);
					break;
				case 6:
					advance_scroll_position((uint16_t)speed);
					break;
				case 7:
					snap_camera_to_section_end();
					break;
				case 8:
					advance_camera_scroll_x_y(speed);
					break;
				default:
					unknown_code(0x177FDu + camera_limits_reached * 3u);
				}
				return current_position;
			}
		}
		else if ((int16_t)next_offset <= section_limit_x)
			camera_limits_reached |= 2u;
	}
	current_position = scroll_position;
	current_position = (uint16_t)(current_position + speed);
	if ((int32_t)ROM16(section_end_list) < (int32_t)current_position)
		camera_limits_reached |= 1u;
	switch (camera_limits_reached)
	{
	case 0:
		advance_camera_scroll_x_y(speed);
		break;
	case 1:
		advance_camera_x_y(speed);
		break;
	case 2:
		advance_camera_y_scroll(speed);
		break;
	case 3:
		advance_camera_y(speed);
		break;
	case 4:
		advance_camera_x_scroll(speed);
		break;
	case 5:
		advance_camera_x(speed);
		break;
	case 6:
		advance_scroll_position((uint16_t)speed);
		break;
	case 7:
		snap_camera_to_section_end();
		break;
	case 8:
		advance_camera_scroll_x_y(speed);
		break;
	default:
		unknown_code(0x177FDu + camera_limits_reached * 3u);
	}
	return current_position;
}

void advance_camera_scroll_x_y(uint32_t speed)
{
	advance_scroll_position((uint16_t)speed);
	advance_camera_x(speed);
	advance_camera_y(speed);
}

void advance_camera_x_y(uint32_t speed)
{
	advance_camera_x(speed);
	advance_camera_y(speed);
}

void advance_camera_y_scroll(uint32_t speed)
{
	advance_camera_y(speed);
	advance_scroll_position((uint16_t)speed);
}

void advance_camera_y(uint32_t speed)
{
	camera_y += section_direction_y * speed;
}

void advance_camera_x_scroll(uint32_t speed)
{
	advance_camera_x(speed);
	advance_scroll_position((uint16_t)speed);
}

void advance_camera_x(uint32_t speed)
{
	camera_x += section_direction_x * speed;
}

void advance_scroll_position(uint16_t speed)
{
	scroll_position += speed;
}

void snap_camera_to_section_end(void)
{
	uint32_t section_end_y = 0;

	section_end_y = (uint32_t)(uint16_t)(ROM16(section_limit_y_list - 2u) + camera_origin_y) << 16;
	camera_x = (uint32_t)(uint16_t)(ROM16(section_limit_x_list - 2u) + camera_origin_x) << 16;
	camera_y = section_end_y;
	scroll_position = ROM16(section_end_list);
}

void update_stage_06_background(uint32_t random_value, uint32_t dest, uint32_t *p_rolls_left, uint32_t *p_words_left, uint32_t *p_source)
{
	uint32_t rolls_left = *p_rolls_left;
	uint32_t words_left = *p_words_left;
	uint32_t source = *p_source;
	uint8_t sound_id = 0;
	uint16_t shake_y = 0;
	uint32_t r1 = 0, cycle_set = 0, ignored = 0, palette_stage = 0, script = 0, write_index_address = 0;
	uint8_t flipped_sign = 0;
	uint16_t previous_shake = 0;

	switch (stage_06_background_step)
	{
	case 0:
	case 8:
		mixer_blend &= 0x1F00u;
		mixer_blend |= 0xA4u;
		camera_x = 0;
		camera_y = 0;
		update_camera_view();
		stage_06_background_step++;
		return;
	case 1:
		if (mixer_blend != 0xA4u)
		{
			mixer_blend = 0xA4u;
			return;
		}
		load_default_palettes();
		palette_stage = 1u;
		load_palette_set_and_backdrop(palette_stage, &ignored, &source, &dest);
		if (mem_read16(palette_color_61D0) != 0x7B0u)
		{
			mixer_blend = 0xA4u;
			*p_source = source;
			return;
		}
		camera_x = 0;
		camera_y = -0x600000;
		update_camera_view();
		nbg3_scroll_x = 0u;
		nbg3_scroll_y = 0u;
		nbg1_scroll_x = 0u;
		nbg1_scroll_y = 0u;
		nbg0_scroll_x = 0u;
		nbg0_scroll_y = 0xFFA00000u;
		mem_write8(video_layer_disable, mem_read8(video_layer_disable) | 0x3Fu);
		script = cloudy_horizon_script_ADDRESS;
		run_vram_script(&palette_stage, &script, &dest);
		script = stage_06_forest_script_ADDRESS;
		run_vram_script(&palette_stage, &script, &dest);
		nbg3_pages = 0u;
		nbg0_pages = 0x5050505u;
		mem_write8(video_layer_disable, mem_read8(video_layer_disable) & 0x26u);
		stage_06_background_step++;
		*p_source = script;
		return;
	case 2:
		nbg0_scroll_x -= 0x2000u;
		nbg1_scroll_x -= 0x1000u;
		AS32(nbg3_scroll_x) = (int32_t)nbg1_scroll_x >> 16;
		return;
	case 3:
		mem_write8(video_layer_disable, mem_read8(video_layer_disable) | 0x3Fu);
		mixer_blend &= 0x1F00u;
		script = clear_tile_page_00_script_ADDRESS;
		run_vram_script(&r1, &script, &dest);
		script = stage_06_upper_jaw_script_ADDRESS;
		run_vram_script(&r1, &script, &dest);
		script = clear_tile_page_0a_script_ADDRESS;
		run_vram_script(&r1, &script, &dest);
		script = stage_06_lower_jaw_script_ADDRESS;
		run_vram_script(&r1, &script, &dest);
		nbg0_pages = 1u;
		nbg3_pages = 6u;
		mem_write8(video_layer_disable, mem_read8(video_layer_disable) & 0x26u);
		nbg0_scroll_x_int = 0u;
		nbg0_scroll_y_int = 0x90u;
		nbg3_scroll_x = 0u;
		nbg3_scroll_y = 0xFF70u;
		mem_write16(mixer_nbg0, mixer_default_nbg0 | 0xFu);
		mem_write16(mixer_nbg3, mixer_default_nbg3 | 0xFu);
		stage_06_background_step++;
		*p_source = script;
		return;
	case 4:
		nbg0_scroll_y_int -= 0x10u;
		if ((int16_t)nbg0_scroll_y_int <= 0)
			nbg0_scroll_y_int = 0u;
		nbg3_scroll_y += 0x10u;
		if ((int16_t)nbg3_scroll_y >= 0)
			nbg3_scroll_y = 0u;
		if ((nbg0_scroll_y_int | nbg3_scroll_y) != 0u)
			return;
		stage_06_background_step = 7u;
		stage_06_shake = 0x800u;
		stage_06_shake_sign = 0;
		sound_id = 0x1Eu;
		play_sound(sound_id, &r1, &write_index_address);
		return;
	case 5:
		mixer_blend &= 0x1F00u;
		mixer_blend |= 0xA4u;
		palette_cycle_set = 0x209090u;
		cycle_set = palette_cycle_set;
		mem_write32(cycle_set + 4u, PALETTE_RAM + 0x83A0u);
		mem_write32(cycle_set, stage_06_cycle_palettes_0_ADDRESS);
		mem_write8(cycle_set + 8u, 2u);
		mem_write8(cycle_set + 0xAu, 2u);
		mem_write8(cycle_set + 9u, 3u);
		mem_write8(cycle_set + 0xBu, 3u);
		mem_write8(cycle_set + 0xCu, 0u);
		mem_write8(cycle_set + 0xDu, 6u);
		cycle_set += 0x10u;
		mem_write32(cycle_set + 4u, PALETTE_RAM + 0x8400u);
		mem_write32(cycle_set, stage_06_cycle_palettes_1_ADDRESS);
		mem_write8(cycle_set + 8u, 7u);
		mem_write8(cycle_set + 0xAu, 7u);
		mem_write8(cycle_set + 9u, 2u);
		mem_write8(cycle_set + 0xBu, 2u);
		mem_write8(cycle_set + 0xCu, 0u);
		mem_write8(cycle_set + 0xDu, 6u);
		camera_x = 0;
		camera_y = 0;
		update_camera_view();
		nbg3_scroll_x = 0u;
		nbg3_scroll_y = 0u;
		nbg0_scroll_x_int = 0u;
		nbg0_scroll_y_int = 0u;
		mem_write8(video_layer_disable, mem_read8(video_layer_disable) | 0x3Fu);
		script = cloudy_horizon_script_ADDRESS;
		run_vram_script(&r1, &script, &dest);
		script = stage_06_island_tower_script_ADDRESS;
		run_vram_script(&r1, &script, &dest);
		nbg3_pages = 0x3020100u;
		nbg0_pages = 0x8070605u;
		mem_write8(video_layer_disable, mem_read8(video_layer_disable) & 0x26u);
		mem_write16(mixer_nbg0, mixer_default_nbg0);
		mem_write16(mixer_nbg3, mixer_default_nbg3);
		stage_06_background_step++;
		*p_source = script;
		return;
	case 6:
		if (palette_cycle_set_0_frame != 0u)
		{
			source = cycle_palette_set_0(source);
			source = cycle_palette_set_1(source);
			*p_rolls_left = rolls_left;
			*p_words_left = words_left;
			*p_source = source;
			return;
		}
		if (palette_cycle_set_1_frame == 0u)
		{
			rolls_left = 2u;
			do
			{
				next_random(&random_value, &words_left, &source);
				random_value &= 0xFu;
				if ((int8_t)random_value > 2)
				{
					*p_rolls_left = rolls_left;
					*p_words_left = words_left;
					*p_source = source;
					return;
				}
				rolls_left--;
			} while (rolls_left != 0u);
			source = cycle_palette_set_0(source);
		}
		source = cycle_palette_set_1(source);
		*p_rolls_left = rolls_left;
		*p_words_left = words_left;
		*p_source = source;
		return;
	case 7:
		if (stage_06_shake == 0u)
			return;
		previous_shake = stage_06_shake;
		stage_06_shake = previous_shake - 0x80u;
		if ((int16_t)previous_shake < 0x80)
			stage_06_shake = 0u;
		shake_y = AT8(stage_06_shake, 1);
		flipped_sign = ~stage_06_shake_sign;
		stage_06_shake_sign = flipped_sign;
		if (flipped_sign != 0u)
			shake_y = (uint16_t)(0u - shake_y);
		nbg0_scroll_y_int = shake_y;
		nbg3_scroll_y = shake_y;
		return;
	default:
		unknown_code(0x178B8u + stage_06_background_step * 3u);
	}
}

void escape_alarm_flash(void)
{
	uint8_t sound_id = 0;
	uint32_t write_index = 0, write_index_address = 0;

	if (escape_seconds == 0u)
	{
		delete_current_object();
		return;
	}
	if (OBJECT(current_object)->escape_alarm_flash.started == 0u)
	{
		mixer_nbg0_blend |= 0x4000u;
		mixer_nbg1_blend |= 0x4000u;
		mixer_nbg2_blend |= 0x4000u;
		mixer_nbg3_blend |= 0x4000u;
		mixer_bitmap_blend |= 0x4000u;
		escape_alarm_stray_flags |= 0x4000u;
		color_offset_red = 0u;
		color_offset_blue = 0x40u;
		color_offset_green = 0x40u;
		OBJECT(current_object)->escape_alarm_flash.counting_up = 0u;
		OBJECT(current_object)->escape_alarm_flash.step_timer = 0;
		OBJECT(current_object)->escape_alarm_flash.started++;
	}
	OBJECT(current_object)->escape_alarm_flash.step_timer++;
	if (OBJECT(current_object)->escape_alarm_flash.step_timer < 2)
		return;
	OBJECT(current_object)->escape_alarm_flash.step_timer = 0;
	if (OBJECT(current_object)->escape_alarm_flash.counting_up != 0u)
	{
		color_offset_blue++;
		color_offset_green++;
		if (color_offset_blue == 0x3Fu)
		{
			OBJECT(current_object)->escape_alarm_flash.counting_up = 0u;
			sound_id = 0x19u;
			play_sound(sound_id, &write_index, &write_index_address);
		}
	}
	else
	{
		color_offset_blue--;
		color_offset_green--;
		if (color_offset_blue == 0x28u)
		{
			OBJECT(current_object)->escape_alarm_flash.counting_up = 1u;
			sound_id = 0x19u;
			play_sound(sound_id, &write_index, &write_index_address);
		}
	}
}
