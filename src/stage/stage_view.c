/**
 *    Name:        stage/stage_view.c
 *    Description: init_stage_view (display, layers, block sets, camera) and the tile sets loaded around the camera
 *    Version:     0.1.0
 *    Created:     12-10-2026
 *    Modified:    15-10-2026
 *    Author:      Sebastian Toyos
 *
 *    SegaSonic The Hedgehog (arcade, Sega System 32): the game's V60 program decompiled into C by
 *    tools/decompile.py, with the names of symbols.txt.
 */
#include "game.h"

void init_stage_view(uint32_t *p_direction_y, uint32_t *p_direction_x, uint32_t *p_count, uint32_t *p_stage_offset)
{
	uint16_t camera_origin = 0;
	uint32_t count = 0, dest = 0, direction_x = 0, direction_y = 0, direction_y_list = 0, ignored = 0, list = 0, sets_left = 0, stage_number = 0, table_offset = 0, tile_set = 0, tile_set_cursor = 0;
	uint8_t io_cnt_bits = 0;
	uint32_t unrotated_sets = 0;

	io_cnt_bits = mem_read8(io_cnt);
	mem_write8(io_cnt, io_cnt_bits & ~2u);
	display_off_frames = 8u;
	dest = load_default_palettes();
	debug_sprite_x = 0x30u;
	debug_sprite_y = 0x30u;
	debug_sprite_spawned = 0u;
	circle_wipe_left = 0x82u;
	circle_wipe_right = 0xBEu;
	circle_wipe_line = 0x32u;
	unused_circle_wipe_byte = 0u;
	AS8(circle_wipe_radius) = 0xAu;
	loaded_tile_sets = 0u;
	nbg0_pages = 0x3020100u;
	nbg2_pages = 0xD0C0B0Au;
	mem_write32(nbg0_page_select, 0x3020100u);
	tile_set_cursor = stage_initial_tile_sets[stage];
	sets_left = 4u;
	mem_write8(video_layer_disable, mem_read8(video_layer_disable) | 0xFu);
	fade_speed = 3u;
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
	tile_set_cursor += 0x19712u;
	do
	{
		tile_set = ROM32(tile_set_cursor);
		AS8(loaded_tile_sets) += tile_set;
		unrotated_sets = loaded_tile_sets;
		loaded_tile_sets = (unrotated_sets << 8) | (unrotated_sets >> 24);
		tile_set_cursor += 4u;
		load_tile_set(tile_set, &list, &ignored, &ignored, &dest);
		sets_left--;
	} while (sets_left != 0u);
	wanted_tile_sets = loaded_tile_sets;
	mixer_blend &= 0x1F00u;
	switch (stage)
	{
	case 0:
	case 9:
	case 16:
		break;
	case 1:
	case 2:
	case 3:
	case 4:
	case 5:
	case 6:
	case 7:
	case 8:
	case 10:
	case 11:
	case 12:
	case 13:
	case 14:
	case 15:
	case 17:
	case 18:
	case 19:
		mixer_blend = 0u;
		break;
	default:
		unknown_code(0x15A33u + stage * 3u);
	}
	stage_06_background_step = 0u;
	scroll_ended = 0;
	fade_step = 0u;
	stage_03_end_started = 0u;
	stage_03_end_step = 0u;
	stage_03_end_frames = 0u;
	unused_view_word = 0u;
	stage_00_exit_step = 0u;
	stage_00_exit_delay = 0u;
	stage_00_exit_timer = 0;
	stage_00_exit_zoom = 0u;
	color_offset_red = 0u;
	color_offset_blue = 0u;
	color_offset_green = 0u;
	AS16(stage_05_switch_step) = 0u;
	map_screen_step = 0u;
	dest = init_nbg2_layer(dest);
	dest = init_nbg1_layer(dest);
	init_nbg3_layer(dest);
	count = set_stage_layer_mixing();
	load_stage_palette_set(&ignored, &ignored);
	view_y = 0;
	view_x = 0;
	camera_x_pixels = 0u;
	camera_y_pixels = 0u;
	tile_set_slot = 0u;
	camera_x = 0;
	camera_y = 0;
	view_x = 0;
	view_y = 0;
	scroll_position = 0u;
	table_offset = stage;
	stage_block_map_width = stage_block_map_widths[table_offset];
	table_offset = (uint16_t)(table_offset * 2u);
	camera_origin = ROM16(stage_camera_origins_x_ADDRESS + table_offset);
	camera_origin_x = camera_origin;
	camera_x += (uint32_t)camera_origin << 16;
	camera_origin = ROM16(stage_camera_origins_y_ADDRESS + table_offset);
	camera_origin_y = camera_origin;
	camera_y += (uint32_t)camera_origin << 16;
	view_offset_y = ROM16(stage_view_offsets_y_ADDRESS + table_offset);
	view_offset_x = ROM16(stage_view_offsets_x_ADDRESS + table_offset);
	scroll_end = ROM16(stage_scroll_ends_ADDRESS + table_offset);
	stage_number = stage;
	list = stage_section_speeds[stage_number] + stage_section_speeds_ADDRESS;
	scroll_speed = ROM8(list);
	section_speed_list = list + 1u;
	list = stage_section_follow_speeds[stage_number] + stage_section_follow_speeds_ADDRESS;
	follow_base_speed = ROM8(list);
	follow_base_speed_list = list + 1u;
	section_end_list = stage_section_ends[stage_number] + stage_section_ends_ADDRESS;
	section_end_list += 2u;
	section_unused_cursor = stage_section_unused[stage_number];
	list = stage_section_limits_x[stage_number] + stage_section_limits_x_ADDRESS;
	list = (list & ~0xFFFFu) | (uint16_t)(list + 2u);
	section_limit_x = mem_read16(list);
	section_limit_x_list = list + 2u;
	list = stage_section_limits_y[stage_number] + stage_section_limits_y_ADDRESS;
	list = (list & ~0xFFFFu) | (uint16_t)(list + 2u);
	section_limit_y = mem_read16(list);
	section_limit_y_list = list + 2u;
	list = stage_scroll_modes[stage_number] + stage_scroll_modes_ADDRESS;
	scroll_mode = ROM16(list);
	list += 2u;
	scroll_mode_timer = ROM16(list);
	scroll_mode_duration = scroll_mode_timer;
	scroll_ramp_speed = 1u;
	scroll_mode_script = list + 2u;
	direction_y_list = stage_section_directions_y[stage_number] + stage_section_directions_y_ADDRESS;
	list = stage_section_directions_x[stage_number] + stage_section_directions_x_ADDRESS;
	direction_x = (int16_t)ROM16(list);
	direction_y = (int16_t)ROM16(direction_y_list);
	section_direction_x = direction_x;
	section_direction_y = direction_y;
	section_direction_x_list = list + 2u;
	section_direction_y_list = direction_y_list + 2u;
	unused_scroll_flag = 0u;
	unused_scroll_offset = -0xA;
	nbg0_scroll_x_int = (int32_t)camera_x >> 16;
	nbg0_scroll_y_int = (int32_t)camera_y >> 16;
	nbg2_scroll_x = nbg0_scroll_x_int;
	nbg1_scroll_x_int = nbg0_scroll_x_int;
	nbg2_scroll_y = nbg0_scroll_y_int;
	nbg1_scroll_y_int = nbg0_scroll_y_int;
	camera_shake_phase = 0u;
	camera_shake_step = 0u;
	camera_shake_length = 0u;
	camera_shake_active = 0u;
	AS16(camera_shake_frame) = 0u;
	back_edge_frames = 0u;
	nearby_floor_blocks_locked = 0u;
	scroll_skip_open = 0u;
	*p_direction_y = direction_y;
	*p_direction_x = direction_x;
	*p_count = count;
	*p_stage_offset = table_offset;
}

void update_stage_tile_layers(uint32_t *p_shifted_tile_set, uint32_t *p_copy_entry, uint32_t *p_columns_left, uint32_t *p_palette_count, uint32_t *p_stage_number, uint32_t *p_row_skip, uint32_t *p_cycle, uint32_t *p_block_cursor, uint32_t *p_dest)
{
	uint32_t copy_entry = *p_copy_entry;
	uint32_t columns_left = *p_columns_left;
	uint32_t palette_count = *p_palette_count;
	uint32_t stage_number = *p_stage_number;
	uint32_t row_skip = *p_row_skip;
	uint32_t cycle = *p_cycle;
	uint32_t block_cursor = *p_block_cursor;
	uint32_t dest = *p_dest;
	uint32_t r1 = 0, r2 = 0, r4 = 0, r5 = 0, r8 = 0, rows_left = 0, set_shift = 0, shifted_tile_set = 0;

	animate_stage_palette(&r1, &copy_entry, &palette_count, &cycle, &block_cursor, &dest);
	if (scroll_ended < 0)
	{
		*p_shifted_tile_set = r1;
		*p_copy_entry = copy_entry;
		*p_palette_count = palette_count;
		*p_cycle = cycle;
		*p_block_cursor = block_cursor;
		*p_dest = dest;
		return;
	}
	clamp_camera_pixels();
	set_nbg0_scroll_from_camera(copy_entry);
	stage_number = stage;
	block_cursor = stage_block_maps[stage_number] + stage_block_maps_ADDRESS + tile_block_row + tile_block_column;
	columns_left = 2u;
	rows_left = 2u;
	set_shift = 0u;
	wanted_tile_sets = 0u;
	for (;;)
	{
		shifted_tile_set = rotate_by(ROM8(block_cursor), (uint8_t)set_shift, 32);
		wanted_tile_sets += shifted_tile_set;
		set_shift = (uint16_t)(set_shift + 8u);
		columns_left--;
		block_cursor++;
		if (columns_left != 0u)
			continue;
		row_skip = stage_block_map_width;
		row_skip = (uint16_t)(row_skip - 2u);
		block_cursor += row_skip;
		columns_left = 2u;
		rows_left--;
		if (rows_left != 0u)
			continue;
		break;
	}
	if (wanted_tile_sets != loaded_tile_sets)
	{
		if (tile_sets_in_progress != wanted_tile_sets && (int8_t)AS8(tile_set_slot) > 1)
			wanted_tile_sets = tile_sets_in_progress;
		reload_changed_tile_set(&shifted_tile_set, &r2, &r4, &r5, &r8, &block_cursor, &dest);
		clamp_camera_pixels();
		*p_shifted_tile_set = shifted_tile_set;
		*p_copy_entry = r2;
		*p_columns_left = columns_left;
		*p_palette_count = r4;
		*p_stage_number = r5;
		*p_row_skip = r8;
		*p_cycle = cycle;
		*p_block_cursor = block_cursor;
		*p_dest = dest;
		return;
	}
	tile_set_slot = 0u;
	clamp_camera_pixels();
	*p_shifted_tile_set = shifted_tile_set;
	*p_copy_entry = rows_left;
	*p_columns_left = columns_left;
	*p_palette_count = set_shift;
	*p_stage_number = stage_number;
	*p_row_skip = row_skip;
	*p_cycle = cycle;
	*p_block_cursor = block_cursor;
	*p_dest = dest;
}

void reload_changed_tile_set(uint32_t *p_shifted_tile_set, uint32_t *p_slots_left, uint32_t *p_r4, uint32_t *p_slot_tile_set, uint32_t *p_loaded_shift, uint32_t *p_script, uint32_t *p_dest)
{
	uint32_t shifted_tile_set = *p_shifted_tile_set;
	uint32_t script = *p_script;
	uint32_t dest = *p_dest;
	uint8_t shift_per_slot = 0;
	uint32_t r4 = 0, r5 = 0, r10 = 0, loaded_shift = 0, slot_tile_set = 0, slots_left = 0, wanted_tile_set = 0;

	slots_left = 4u;
	r4 = 0u;
	shift_per_slot = 0xF8u;
	loaded_shift = 0u;
	if (tile_sets_in_progress != wanted_tile_sets)
		tile_set_slot = 0u;
	wanted_tile_set = shift_by(wanted_tile_sets, (uint8_t)((uint32_t)shift_per_slot * tile_set_slot), 32) & 0xFFu;
	tile_set_slot++;
	for (;;)
	{
		slot_tile_set = shift_by(loaded_tile_sets, (uint8_t)loaded_shift, 32) & 0xFFu;
		if (wanted_tile_set == slot_tile_set)
			break;
		loaded_shift = (uint16_t)(loaded_shift - 8u);
		slots_left--;
		if (slots_left == 0u)
		{
			load_tile_set(wanted_tile_set, &shifted_tile_set, &r5, &r10, &dest);
			*p_shifted_tile_set = shifted_tile_set;
			*p_slots_left = slots_left;
			*p_r4 = r4;
			*p_slot_tile_set = r5;
			*p_loaded_shift = loaded_shift;
			*p_script = r10;
			*p_dest = dest;
			return;
		}
	}
	if ((int8_t)AS8(tile_set_slot) < 4)
	{
		tile_sets_in_progress = wanted_tile_sets;
		*p_slots_left = slots_left;
		*p_r4 = r4;
		*p_slot_tile_set = slot_tile_set;
		*p_loaded_shift = loaded_shift;
		return;
	}
	loaded_tile_sets = wanted_tile_sets;
	tile_set_slot = 0u;
	tile_sets_in_progress = wanted_tile_sets;
	*p_slots_left = slots_left;
	*p_r4 = r4;
	*p_slot_tile_set = slot_tile_set;
	*p_loaded_shift = loaded_shift;
}

void load_tile_set(uint32_t tile_set, uint32_t *p_count, uint32_t *p_stage_offset, uint32_t *p_script, uint32_t *p_dest)
{
	uint32_t count = *p_count;
	uint32_t dest = *p_dest;
	uint32_t script = 0, script_list = 0, stage_offset = 0;

	script_list = stage_tile_sets[stage];
	get_tile_set_list(tile_set, &stage_offset, &script_list);
	script_list = get_tile_set_list_bank(script_list);
	for (;;)
	{
		script = mem_read32(script_list);
		script_list += 4u;
		if ((int32_t)script < 0)
			break;
		script = get_tile_set_script_bank(script);
		run_vram_script(&count, &script, &dest);
	}
	if ((int8_t)AS8(tile_set_slot) < 4)
	{
		tile_sets_in_progress = wanted_tile_sets;
		*p_count = count;
		*p_stage_offset = stage_offset;
		*p_script = script;
		*p_dest = dest;
		return;
	}
	loaded_tile_sets = wanted_tile_sets;
	tile_set_slot = 0u;
	tile_sets_in_progress = wanted_tile_sets;
	*p_count = count;
	*p_stage_offset = stage_offset;
	*p_script = script;
	*p_dest = dest;
}

uint32_t get_tile_set_script_bank(uint32_t script)
{
	uint32_t stage_number = 0;

	stage_number = AS8(stage);
	switch ((uint16_t)(stage_number << 1))
	{
	case 0x0u:
	case 0x2u:
	case 0x4u:
	case 0x22u:
	case 0x24u:
		return script;
	case 0x6u:
	case 0x8u:
	case 0xAu:
	case 0xCu:
	case 0xEu:
	case 0x10u:
	case 0x12u:
	case 0x14u:
	case 0x16u:
	case 0x18u:
	case 0x1Au:
	case 0x1Cu:
	case 0x1Eu:
	case 0x20u:
	case 0x26u:
		script += 0x100000u;
		return script;
	default:
		unknown_code(0x15E85u + (uint16_t)(stage_number << 1));
	}
}

uint32_t get_tile_set_list_bank(uint32_t script_list)
{
	uint32_t stage_number = 0;

	stage_number = AS8(stage);
	switch ((uint16_t)(stage_number << 1))
	{
	case 0x0u:
	case 0x2u:
	case 0x4u:
	case 0x22u:
	case 0x24u:
		return script_list;
	case 0x6u:
	case 0x8u:
	case 0xAu:
	case 0xCu:
	case 0xEu:
	case 0x10u:
	case 0x12u:
	case 0x14u:
	case 0x16u:
	case 0x18u:
	case 0x1Au:
	case 0x1Cu:
	case 0x1Eu:
	case 0x20u:
	case 0x26u:
		script_list += 0x100000u;
		return script_list;
	default:
		unknown_code(0x15EC0u + (uint16_t)(stage_number << 1));
	}
}

void get_tile_set_list(uint32_t tile_set, uint32_t *p_stage_offset, uint32_t *p_script_list)
{
	uint32_t script_list = *p_script_list;
	uint32_t stage_offset = 0;

	stage_offset = AS8(stage);
	stage_offset = (uint16_t)(stage_offset << 1);
	switch (stage_offset)
	{
	case 0x0u:
	case 0x2u:
	case 0x4u:
	case 0x22u:
	case 0x24u:
	case 0x26u:
		script_list = mem_read16((script_list & ~0xFFFFu) | (uint16_t)(script_list + (tile_set << 1))) + blank_tile_set_lists_ADDRESS;
		*p_stage_offset = stage_offset;
		*p_script_list = script_list;
		return;
	case 0x6u:
	case 0x8u:
	case 0xAu:
	case 0xCu:
	case 0xEu:
	case 0x10u:
	case 0x12u:
	case 0x14u:
	case 0x16u:
	case 0x18u:
	case 0x1Au:
	case 0x1Cu:
	case 0x1Eu:
	case 0x20u:
		script_list = mem_read32(script_list + (tile_set << 2));
		*p_stage_offset = stage_offset;
		*p_script_list = script_list;
		return;
	default:
		unknown_code(0x15EFAu + stage_offset);
	}
}
