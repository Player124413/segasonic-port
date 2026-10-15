/**
 *    Name:        stage/scroll_skip.c
 *    Description: the camera put at a start section (the demos start mid-stage) and the unused "Scroll skip mode" debug panel that jumps between sections
 *    Version:     0.1.0
 *    Created:     12-10-2026
 *    Modified:    15-10-2026
 *    Author:      Sebastian Toyos
 *
 *    SegaSonic The Hedgehog (arcade, Sega System 32): the game's V60 program decompiled into C by
 *    tools/decompile.py, with the names of symbols.txt.
 */
#include "game.h"

void run_scroll_skip_mode(void)
{
	if (scroll_skip_open == 0u)
	{
		if ((input_ports_active & 0x21u) != 0x21u)
		{
			scroll_skip_open = 0u;
			return;
		}
		scroll_skip_last_point = scroll_skip_last_points[stage];
		stage_start_section = 0xDu;
		scroll_skip_value_index = 0u;
		scroll_skip_next_armed = 0u;
		scroll_skip_back_armed = 0u;
		scroll_skip_exit_armed = 0u;
		scroll_skip_call_armed = 0u;
		scroll_skip_open = 1u;
		scroll_skip_print_panel();
	}
	scroll_skip_draw_values();
	scroll_skip_next_point();
	scroll_skip_previous_point();
	scroll_skip_check_call();
	scroll_skip_check_exit();
}

void scroll_skip_next_point(void)
{
	uint32_t held_buttons = 0;

	held_buttons = input_ports_active & 0x1010000u;
	if (held_buttons != 0x1010000u)
	{
		if ((held_buttons & 0x10000u) != 0u)
		{
			if (scroll_skip_next_armed != 0u)
			{
				stage_start_section++;
				scroll_skip_next_armed = 0u;
			}
		}
		else
			scroll_skip_next_armed = 1u;
	}
	if (scroll_skip_last_point < (int16_t)stage_start_section)
		stage_start_section = scroll_skip_last_point;
}

void scroll_skip_previous_point(void)
{
	uint32_t held_buttons = 0;

	held_buttons = input_ports_active & 0x1010000u;
	if (held_buttons != 0x1010000u)
	{
		if ((held_buttons & 0x1000000u) != 0u)
		{
			if (scroll_skip_back_armed != 0u)
			{
				stage_start_section--;
				scroll_skip_back_armed = 0u;
			}
		}
		else
			scroll_skip_back_armed = 1u;
	}
	if ((int16_t)stage_start_section < 0)
		stage_start_section = 0u;
}

void scroll_skip_check_exit(void)
{
	uint32_t held_buttons = 0;

	held_buttons = input_ports_active & 0x30u;
	if (held_buttons == 0x30u)
		return;
	if ((held_buttons & 0x20u) != 0u)
	{
		if (scroll_skip_exit_armed != 0u)
		{
			scroll_skip_open = 0u;
			scroll_skip_erase_panel();
			scroll_skip_exit_armed = 0u;
		}
	}
	else
		scroll_skip_exit_armed = 1u;
}

void scroll_skip_erase_panel(void)
{
	uint32_t text = 0, text_position = 0;

	text = scroll_skip_blank_text_ADDRESS;
	print_text_list(&text, &text_position);
}

void scroll_skip_check_call(void)
{
	uint32_t held_buttons = 0, ignored = 0;

	held_buttons = input_ports_active & 0x30u;
	if (held_buttons == 0x30u)
		return;
	if ((held_buttons & 0x10u) != 0u)
	{
		if (scroll_skip_call_armed == 0u)
			return;
		move_camera_to_start_section(&ignored, &ignored, &ignored);
		return;
	}
	scroll_skip_call_armed = 1u;
}

void move_camera_to_start_section(uint32_t *p_y, uint32_t *p_section_offset, uint32_t *p_stage_index)
{
	uint32_t r2 = 0, section_offset = 0, stage_index = 0;

	load_camera_start_section(&section_offset, &stage_index);
	r2 = update_camera_view();
	*p_y = r2;
	*p_section_offset = section_offset;
	*p_stage_index = stage_index;
}

void load_camera_start_section(uint32_t *p_section_offset, uint32_t *p_stage_index)
{
	uint32_t direction_y_cursor = 0, limit_y_cursor = 0, list = 0, section_index = 0, section_offset = 0, stage_index = 0;
	uint16_t start_y = 0, limit_y = 0, direction_y = 0;

	stage_index = stage;
	section_offset = stage_start_section;
	section_offset = (uint16_t)(section_offset << 1);
	list = stage_section_limits_x[stage_index] + stage_section_limits_x_ADDRESS + section_offset;
	limit_y_cursor = stage_section_limits_y[stage_index] + stage_section_limits_y_ADDRESS + section_offset;
	start_y = ROM16(limit_y_cursor);
	limit_y_cursor += 2u;
	camera_x = (uint32_t)ROM16(list) << 16;
	camera_y = (uint32_t)start_y << 16;
	camera_x += (uint32_t)camera_origin_x << 16;
	camera_y += (uint32_t)camera_origin_y << 16;
	list += 2u;
	section_limit_x = ROM16(list);
	limit_y = ROM16(limit_y_cursor);
	section_limit_y = limit_y;
	section_limit_x_list = list + 2u;
	section_limit_y_list = limit_y_cursor + 2u;
	list = stage_section_directions_x[stage_index] + stage_section_directions_x_ADDRESS + section_offset;
	direction_y_cursor = stage_section_directions_y[stage_index] + stage_section_directions_y_ADDRESS + section_offset;
	section_direction_x = (int16_t)ROM16(list);
	direction_y = ROM16(direction_y_cursor);
	section_direction_y = (int16_t)direction_y;
	section_direction_x_list = list + 2u;
	section_direction_y_list = direction_y_cursor + 2u;
	list = stage_section_ends[stage_index] + stage_section_ends_ADDRESS + section_offset;
	scroll_position = ROM16(list);
	section_end_list = list + 2u;
	section_index = section_offset >> 1;
	list = stage_section_speeds[stage_index] + stage_section_speeds_ADDRESS + section_index;
	scroll_speed = mem_read8(list);
	section_speed_list = list + 1u;
	list = stage_section_follow_speeds[stage_index] + stage_section_follow_speeds_ADDRESS + section_index;
	follow_base_speed = mem_read8(list);
	follow_base_speed_list = list + 1u;
	list = stage_scroll_modes[stage_index] + stage_scroll_modes_ADDRESS + section_offset + section_offset;
	scroll_mode = ROM16(list);
	list += 2u;
	scroll_mode_timer = ROM16(list);
	scroll_mode_duration = scroll_mode_timer;
	AS16(scroll_ramp_speed) = 1u;
	scroll_mode_script = list + 2u;
	*p_section_offset = section_offset;
	*p_stage_index = stage_index;
}

void index_stage_list_pair(uint32_t section_offset, uint32_t stage_offset, uint32_t *p_first_entry, uint32_t *p_second_entry)
{
	uint32_t first_entry = *p_first_entry;
	uint32_t second_entry = *p_second_entry;

	second_entry = mem_read32((second_entry & ~0xFFFFu) | (uint16_t)(second_entry + stage_offset)) + section_offset;
	first_entry = mem_read32(first_entry + stage_offset) + section_offset;
	*p_first_entry = first_entry;
	*p_second_entry = second_entry;
}

uint32_t scroll_skip_x_position_cell(void)
{
	uint32_t text_position = 0;

	text_position = TEXT_CELL(34, 18);
	return text_position;
}

uint32_t scroll_skip_y_position_cell(void)
{
	uint32_t text_position = 0;

	text_position = TEXT_CELL(34, 19);
	return text_position;
}

uint32_t scroll_skip_total_cell(void)
{
	uint32_t text_position = 0;

	text_position = TEXT_CELL(34, 17);
	return text_position;
}

void scroll_skip_draw_values(void)
{
	uint16_t attribute_2 = 0;
	uint32_t attribute = 0, point = 0, text_position = 0, value = 0, value_list = 0;

	point = stage_start_section;
	value_list = ROM32(scroll_skip_value_lists_ADDRESS + scroll_skip_value_index * 2u * 2u);
	value = ROM16(mem_read16((value_list & ~0xFFFFu) | (uint16_t)(value_list + stage * 2u)) + value_list + (uint16_t)(point * 2u));
	switch (scroll_skip_value_index)
	{
	case 0:
		text_position = scroll_skip_x_position_cell();
		break;
	case 1:
		text_position = scroll_skip_y_position_cell();
		break;
	case 2:
		text_position = scroll_skip_total_cell();
		break;
	case 3:
		text_position = scroll_skip_x_position_cell();
		break;
	default:
		unknown_code(0x1AAC1u + scroll_skip_value_index * 3u);
	}
	attribute = 0x200u;
	draw_hex_4(&value, &attribute, &text_position);
	scroll_skip_value_index++;
	if (scroll_skip_value_index != 3u)
		return;
	scroll_skip_value_index = 0u;
	value = stage_start_section;
	text_position = TEXT_CELL(24, 15);
	attribute_2 = 0x600u;
	draw_hex_2(value, attribute_2, text_position);
}

void scroll_skip_print_panel(void)
{
	uint32_t text = 0, text_position = 0;

	text = scroll_skip_panel_text_ADDRESS;
	print_text_list(&text, &text_position);
}

void clear_bitmap_lower_half(void)
{
	bitmap_address = VIDEO_RAM + 0xE000u;
	fill_value = fill_value & ~0xFFFFu;
	set_halfwords(VIDEO_RAM + 0xE000u, 0x7000u, fill_value);
	mem_write32(video_layer_disable, mem_read32(video_layer_disable) & ~0x20u);
	AS16(bitmap_8bpp) = 1u;
	bitmap_window_left = 0;
	bitmap_window_top = 0;
	bitmap_window_right = 0x13F;
	bitmap_window_bottom = 0xDF;
}
