/**
 *    Name:        game_04.c
 *    Description: the game's functions, part 04 (in the order of their V60 addresses)
 *    Version:     0.1.0
 *    Created:     12-10-2026
 *    Modified:    13-10-2026
 *    Author:      Sebastian Toyos
 *
 *    SegaSonic The Hedgehog (arcade, Sega System 32): the game's V60 program decompiled into C by
 *    tools/decompile.py, with the names of symbols.txt.
 */
#include "game.h"

void eel_den_collapse(void)
{
	uint16_t frame_timer = 0;

	frame_timer = OBJECT(current_object)->eel_den.frame_timer;
	OBJECT(current_object)->eel_den.frame_timer = frame_timer - 1u;
	if (frame_timer != 0u)
	{
		eel_den_draw();
		return;
	}
	OBJECT(current_object)->eel_den.frame_timer = 2u;
	OBJECT(current_object)->sprite_frame = eel_den_collapse_frames[(uint8_t)OBJECT(current_object)->eel_den.frame_index];
	OBJECT(current_object)->eel_den.frame_index++;
	if (OBJECT(current_object)->eel_den.frame_index <= 0x2C)
	{
		eel_den_draw();
		return;
	}
	OBJECT(current_object)->handler = eel_den_draw_ADDRESS;
	eel_den_draw();
}

void eel_den_draw(void)
{
	uint32_t r2 = 0, r3 = 0, r11 = 0, deleted = 0, depth = 0, ignored = 0;

	unproject_object(&ignored, &r2, &r3, &r11);
	delete_if_off_screen(&deleted, &r2, &r3);
	if ((uint8_t)deleted != 1u)
	{
		depth = (uint16_t)(OBJECT(current_object)->screen_y - 0x18u);
		queue_sprite_at_depth(&ignored, &depth);
		add_to_touchable_list(ignored);
	}
}

void stepped_wall_init(void)
{
	OBJECT(current_object)->sprite_flags = 6u;
	OBJECT(current_object)->sprite_frame = 0xB670u;
	OBJECT(current_object)->sprite_palette = 0x8D60u;
	OBJECT(current_object)->sprite_palette |= 0x2000u;
	OBJECT(current_object)->flags |= 2u;
	OBJECT(current_object)->hitbox_size_x = 0x1Du;
	OBJECT(current_object)->hitbox_size_y = 0xDCu;
	OBJECT(current_object)->hitbox_offset_x = 0x73;
	OBJECT(current_object)->hitbox_offset_y = -0x7E;
	OBJECT(current_object)->hitbox_height = 0x80u;
	OBJECT(current_object)->kind = 0xCu;
	OBJECT(current_object)->handler = stepped_wall_draw_ADDRESS;
}

void stepped_wall_draw(void)
{
	uint32_t r2 = 0, r3 = 0, r11 = 0, bucket = 0, deleted = 0, ignored = 0;

	unproject_object(&ignored, &r2, &r3, &r11);
	delete_if_off_screen(&deleted, &r2, &r3);
	if ((uint8_t)deleted == 0u)
	{
		bucket = 0xFFu;
		queue_sprite_in_bucket(bucket);
		add_to_touchable_list(bucket);
		return;
	}
}

void init_stage_view(uint32_t *p_r2, uint32_t *p_r3, uint32_t *p_r4, uint32_t *p_stage_offset)
{
	uint16_t camera_origin = 0;
	uint32_t r4 = 0, dest = 0, direction_x = 0, direction_y = 0, direction_y_list = 0, ignored = 0, list = 0, sets_left = 0, stage_number = 0, table_offset = 0, tile_set = 0, tile_set_cursor = 0;
	uint8_t io_cnt_bits = 0;
	uint32_t unrotated_sets = 0;

	io_cnt_bits = mem_read8(io_cnt);
	mem_write8(io_cnt, io_cnt_bits & ~2u);
	display_off_frames = 8u;
	dest = load_default_palettes();
	unused_view_value_1 = 0x30u;
	unused_view_value_2 = 0x30u;
	unused_view_value_3 = 0u;
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
	r4 = set_stage_layer_mixing();
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
	table_offset = table_offset;
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
	unused_view_byte = 0u;
	*p_r2 = direction_y;
	*p_r3 = direction_x;
	*p_r4 = r4;
	*p_stage_offset = table_offset;
}

void update_stage_tile_layers(uint32_t *p_shifted_tile_set, uint32_t *p_r2, uint32_t *p_columns_left, uint32_t *p_r4, uint32_t *p_stage_number, uint32_t *p_row_skip, uint32_t *p_cycle, uint32_t *p_r10, uint32_t *p_dest)
{
	uint32_t r2 = *p_r2;
	uint32_t columns_left = *p_columns_left;
	uint32_t r4 = *p_r4;
	uint32_t stage_number = *p_stage_number;
	uint32_t row_skip = *p_row_skip;
	uint32_t cycle = *p_cycle;
	uint32_t r10 = *p_r10;
	uint32_t dest = *p_dest;
	uint32_t r1 = 0, r2_2 = 0, r4_2 = 0, r5 = 0, r8 = 0, block_cursor = 0, rows_left = 0, set_shift = 0, shifted_tile_set = 0;

	animate_stage_palette(&r1, &r2, &r4, &cycle, &r10, &dest);
	if (scroll_ended < 0)
	{
		*p_shifted_tile_set = r1;
		*p_r2 = r2;
		*p_r4 = r4;
		*p_cycle = cycle;
		*p_r10 = r10;
		*p_dest = dest;
		return;
	}
	clamp_camera_pixels();
	set_nbg0_scroll_from_camera(r2);
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
		reload_changed_tile_set(&shifted_tile_set, &r2_2, &r4_2, &r5, &r8, &block_cursor, &dest);
		clamp_camera_pixels();
		*p_shifted_tile_set = shifted_tile_set;
		*p_r2 = r2_2;
		*p_columns_left = columns_left;
		*p_r4 = r4_2;
		*p_stage_number = r5;
		*p_row_skip = r8;
		*p_cycle = cycle;
		*p_r10 = block_cursor;
		*p_dest = dest;
		return;
	}
	tile_set_slot = 0u;
	clamp_camera_pixels();
	*p_shifted_tile_set = shifted_tile_set;
	*p_r2 = rows_left;
	*p_columns_left = columns_left;
	*p_r4 = set_shift;
	*p_stage_number = stage_number;
	*p_row_skip = row_skip;
	*p_cycle = cycle;
	*p_r10 = block_cursor;
	*p_dest = dest;
}

void reload_changed_tile_set(uint32_t *p_r1, uint32_t *p_r2, uint32_t *p_r4, uint32_t *p_r5, uint32_t *p_r8, uint32_t *p_script, uint32_t *p_dest)
{
	uint32_t r1 = *p_r1;
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
			load_tile_set(wanted_tile_set, &r1, &r5, &r10, &dest);
			*p_r1 = r1;
			*p_r2 = slots_left;
			*p_r4 = r4;
			*p_r5 = r5;
			*p_r8 = loaded_shift;
			*p_script = r10;
			*p_dest = dest;
			return;
		}
	}
	if ((int8_t)AS8(tile_set_slot) < 4)
	{
		tile_sets_in_progress = wanted_tile_sets;
		*p_r2 = slots_left;
		*p_r4 = r4;
		*p_r5 = slot_tile_set;
		*p_r8 = loaded_shift;
		return;
	}
	loaded_tile_sets = wanted_tile_sets;
	tile_set_slot = 0u;
	tile_sets_in_progress = wanted_tile_sets;
	*p_r2 = slots_left;
	*p_r4 = r4;
	*p_r5 = slot_tile_set;
	*p_r8 = loaded_shift;
}

void load_tile_set(uint32_t tile_set, uint32_t *p_r1, uint32_t *p_stage_offset, uint32_t *p_script, uint32_t *p_dest)
{
	uint32_t r1 = *p_r1;
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
		run_vram_script(&r1, &script, &dest);
	}
	if ((int8_t)AS8(tile_set_slot) < 4)
	{
		tile_sets_in_progress = wanted_tile_sets;
		*p_r1 = r1;
		*p_stage_offset = stage_offset;
		*p_script = script;
		*p_dest = dest;
		return;
	}
	loaded_tile_sets = wanted_tile_sets;
	tile_set_slot = 0u;
	tile_sets_in_progress = wanted_tile_sets;
	*p_r1 = r1;
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

void clamp_camera_pixels(void)
{
	if ((int16_t)camera_x_pixels <= 0)
		camera_x_pixels = 0u;
	if ((int16_t)camera_x_pixels >= 0x2000)
		camera_x_pixels = 0x1FFFu;
	if ((int16_t)camera_y_pixels <= 0)
		camera_y_pixels = 0u;
	if ((int16_t)camera_y_pixels >= 0x1000)
		camera_y_pixels = 0xFFFu;
}

void update_scroll(uint32_t *p_r2, uint32_t *p_speed, uint32_t *p_r4, uint32_t *p_r5, uint32_t *p_r8, uint32_t *p_r9, uint32_t *p_end_script, uint32_t *p_r11)
{
	uint32_t r2 = *p_r2;
	uint32_t speed = *p_speed;
	uint32_t r4 = *p_r4;
	uint32_t r5 = *p_r5;
	uint32_t r8 = *p_r8;
	uint32_t r9 = *p_r9;
	uint32_t end_script = *p_end_script;
	uint32_t r11 = *p_r11;
	uint16_t layer_scroll_x = 0, target_heading = 0;
	uint32_t r2_2 = 0, r3 = 0, r4_2 = 0, r5_2 = 0, check_result = 0, count = 0, count_2 = 0, mode_script_cursor = 0, object = 0, object_2 = 0, object_3 = 0, player = 0, player_2 = 0, player_speed = 0, push_speed = 0, script = 0, sine_entry = 0, speed_value = 0, velocity_y = 0;

	if (scroll_ended < 0)
	{
	update_scroll_at_end:
		scroll_ended = -1;
		switch (stage)
		{
		case 0:
		case 1:
		case 2:
		case 4:
		case 5:
		case 6:
		case 7:
		case 8:
		case 9:
		case 10:
		case 11:
		case 12:
		case 13:
		case 14:
		case 15:
		case 16:
		case 17:
		case 18:
			*p_r2 = r2;
			return;
		case 3:
			break;
		case 19:
			*p_r2 = r2;
			return;
		default:
			unknown_code(0x16BE2u + stage * 3u);
		}
		if ((int8_t)stage_03_end_started <= 0)
		{
			stage_03_end_started++;
			script = stage_03_end_script_1_ADDRESS;
			run_vram_script(&count, &script, &r11);
			end_script = stage_03_end_script_2_ADDRESS;
			run_vram_script(&count, &end_script, &r11);
			nbg0_pages = 0x2020202u;
			scroll_mode_timer = 0x3Cu;
			scroll_mode_duration = scroll_mode_timer;
			scroll_mode = 4u;
			scroll_speed = 0x19u;
			stage_03_end_step = 0u;
			stage_03_end_timer = 0x78;
			stage_03_end_frames = 0u;
		}
		switch (stage_03_end_step)
		{
		case 0:
		case 4:
			stage_03_end_frames++;
			scroll_speeding_up(&r2, &speed, &r4, &r5, &end_script, &r11);
			if (AS8(scroll_mode) == 0u)
			{
				speed = (speed & ~0xFFFFu) | 0x25u;
				scroll_speed = 0x25u;
				stage_03_end_step++;
			}
			break;
		case 1:
			stage_03_end_frames++;
			speed = (speed & ~0xFFFFu) | scroll_speed;
			stage_03_end_timer--;
			if (stage_03_end_timer <= 0)
			{
				camera_y = 0x10000;
				end_script = stage_03_end_script_3_ADDRESS;
				run_vram_script(&count, &end_script, &r11);
				nbg0_pages = 0x2020200u;
				stage_03_end_step++;
				scroll_mode_timer = 5u;
				scroll_mode_duration = scroll_mode_timer;
				scroll_mode = 4u;
				scroll_speed = 0x19u;
			}
			break;
		case 2:
			stage_03_end_frames++;
			scroll_slowing_down(&r2, &speed, &r4, &r5, &end_script, &r11);
			if ((int16_t)scroll_speed <= 1)
			{
				scroll_speed = 0u;
				stage_03_end_step++;
				all_players_path_request = 0x23u;
				r11 = spawn_stage_03_end_pillar(r11);
				*p_r2 = r2;
				*p_speed = speed;
				*p_r4 = r4;
				*p_r5 = r5;
				*p_end_script = end_script;
				*p_r11 = r11;
				return;
			}
			break;
		case 3:
			camera_velocity_x = 0;
			camera_velocity_y = 0;
			*p_r2 = r2;
			*p_end_script = end_script;
			*p_r11 = r11;
			return;
		default:
			unknown_code(0x174D2u + stage_03_end_step * 3u);
		}
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
		speed_value = (uint16_t)speed;
		velocity_y = section_direction_y * speed_value;
		camera_velocity_x = section_direction_x * speed_value;
		camera_velocity_y = velocity_y;
		*p_r2 = velocity_y;
		*p_speed = speed;
		*p_r4 = speed_value;
		*p_r5 = r5;
		*p_end_script = end_script;
		*p_r11 = r11;
		return;
	}
	r2 = scroll_position;
	if ((int32_t)scroll_end <= (int32_t)r2)
		goto update_scroll_at_end;
	switch (scroll_mode)
	{
	case 0:
		scroll_at_scroll_speed(&r2, &speed, &r4, &r5, &end_script, &r11);
		*p_r2 = r2;
		*p_speed = speed;
		*p_r4 = r4;
		*p_r5 = r5;
		*p_r8 = r8;
		*p_r9 = r9;
		*p_end_script = end_script;
		*p_r11 = r11;
		return;
	case 1:
		scroll_speed = 0u;
		speed &= ~0xFFFFu;
		set_camera_velocity(&r2_2, &speed, &r4_2, &r5, &end_script, &r11);
		*p_r2 = r2_2;
		*p_speed = speed;
		*p_r4 = r4_2;
		*p_r5 = r5;
		*p_r8 = r8;
		*p_r9 = r9;
		*p_end_script = end_script;
		*p_r11 = r11;
		return;
	case 2:
		scroll_mode_timer--;
		if (scroll_mode_timer != 0u)
		{
			update_camera_view();
			speed = 0u;
			update_camera_view();
			set_camera_velocity(&r2_2, &speed, &r4_2, &r5, &end_script, &r11);
			*p_r2 = r2_2;
			*p_speed = speed;
			*p_r4 = r4_2;
			*p_r5 = r5;
			*p_r8 = r8;
			*p_r9 = r9;
			*p_end_script = end_script;
			*p_r11 = r11;
			return;
		}
		if (scroll_mode != 2u)
		{
			scroll_mode = 0u;
			scroll_ramp_speed = 1u;
			update_camera_view();
			set_camera_velocity(&r2_2, &speed, &r4_2, &r5, &end_script, &r11);
			*p_r2 = r2_2;
			*p_speed = speed;
			*p_r4 = r4_2;
			*p_r5 = r5;
			*p_r8 = r8;
			*p_r9 = r9;
			*p_end_script = end_script;
			*p_r11 = r11;
			return;
		}
		mode_script_cursor = scroll_mode_script;
		scroll_mode = ROM16(mode_script_cursor);
		mode_script_cursor += 2u;
		scroll_mode_timer = ROM16(mode_script_cursor);
		scroll_mode_duration = scroll_mode_timer;
		scroll_mode_script = mode_script_cursor + 2u;
		scroll_ramp_speed = 1u;
		set_camera_velocity(&r2_2, &speed, &r4_2, &r5, &end_script, &r11);
		*p_r2 = r2_2;
		*p_speed = speed;
		*p_r4 = r4_2;
		*p_r5 = r5;
		*p_r8 = r8;
		*p_r9 = r9;
		*p_end_script = end_script;
		*p_r11 = r11;
		return;
	case 3:
		scroll_slowing_down(&r2, &r3, &r4, &r5, &end_script, &r11);
		*p_r2 = r2;
		*p_speed = r3;
		*p_r4 = r4;
		*p_r5 = r5;
		*p_r8 = r8;
		*p_r9 = r9;
		*p_end_script = end_script;
		*p_r11 = r11;
		return;
	case 4:
		scroll_speeding_up(&r2, &speed, &r4, &r5, &end_script, &r11);
		*p_r2 = r2;
		*p_speed = speed;
		*p_r4 = r4;
		*p_r5 = r5;
		*p_r8 = r8;
		*p_r9 = r9;
		*p_end_script = end_script;
		*p_r11 = r11;
		return;
	case 5:
		nearby_floor_blocks_locked = 1u;
		mode_script_cursor = scroll_mode_script;
		scroll_mode = ROM16(mode_script_cursor);
		mode_script_cursor += 2u;
		scroll_mode_timer = ROM16(mode_script_cursor);
		scroll_mode_duration = scroll_mode_timer;
		scroll_mode_script = mode_script_cursor + 2u;
		scroll_ramp_speed = 1u;
		set_camera_velocity(&r2_2, &speed, &r4_2, &r5, &end_script, &r11);
		*p_r2 = r2_2;
		*p_speed = speed;
		*p_r4 = r4_2;
		*p_r5 = r5;
		*p_r8 = r8;
		*p_r9 = r9;
		*p_end_script = end_script;
		*p_r11 = r11;
		return;
	case 6:
		nearby_floor_blocks_locked = 0u;
		mode_script_cursor = scroll_mode_script;
		scroll_mode = ROM16(mode_script_cursor);
		mode_script_cursor += 2u;
		scroll_mode_timer = ROM16(mode_script_cursor);
		scroll_mode_duration = scroll_mode_timer;
		scroll_mode_script = mode_script_cursor + 2u;
		scroll_ramp_speed = 1u;
		set_camera_velocity(&r2_2, &speed, &r4_2, &r5, &end_script, &r11);
		*p_r2 = r2_2;
		*p_speed = speed;
		*p_r4 = r4_2;
		*p_r5 = r5;
		*p_r8 = r8;
		*p_r9 = r9;
		*p_end_script = end_script;
		*p_r11 = r11;
		return;
	case 7:
	case 8:
		player = PLAYER_BLOCK(1);
		player_2 = PLAYER_BLOCK(2);
		r14 = PLAYER_BLOCK(3);
		object = LINKED(WORK_RAM, player_1_object);
		object_2 = LINKED(WORK_RAM, player_2_object);
		object_3 = LINKED(WORK_RAM, player_3_object);
		players_pushing_edge = 0u;
		players_facing_scroll = 0u;
		players_in_play_bits = 0u;
		player_past_leading_edge = 0u;
		if ((OBJECT(object)->flags & 0x10000400u) == 0u)
			players_in_play_bits |= 1u;
		if ((OBJECT(object_2)->flags & 0x10000400u) == 0u)
			players_in_play_bits |= 2u;
		if ((OBJECT(object_3)->flags & 0x10000400u) == 0u)
			players_in_play_bits |= 4u;
		check_result = 0u;
		set_screen_push_edges(check_result, &count_2, &sine_entry);
		is_direction_in_quadrant(count_2, &check_result, &sine_entry);
		if ((uint8_t)check_result == 0u)
		{
			if (player_at_trailing_edge != 0u && (int16_t)((uint32_t)ROM16(section_end_list) - scroll_position) <= 0x180)
			{
				if ((int8_t)back_edge_frames >= 0x40)
					follow_base_speed = 6u;
				else
					back_edge_frames++;
			}
			push_speed = 0u;
			push_speed &= 0xFFu;
			push_speed = (uint16_t)(push_speed + follow_base_speed);
			advance_scroll(&r2_2, &push_speed, &r4_2, &r5_2, &player, &sine_entry);
			*p_r2 = r2_2;
			*p_speed = push_speed;
			*p_r4 = r4_2;
			*p_r5 = r5_2;
			*p_r8 = object_2;
			*p_r9 = object_3;
			*p_end_script = player;
			*p_r11 = sine_entry;
			return;
		}
		if ((players_in_play_bits & 1u) != 0u)
		{
			target_heading = PLAYER(player)->target_heading;
			check_result = is_player_facing_scroll(check_result, target_heading, (uint16_t)sine_entry);
			if ((uint8_t)check_result != 0u)
				players_facing_scroll |= 1u;
		}
		if ((players_in_play_bits & 2u) != 0u)
		{
			target_heading = PLAYER(player_2)->target_heading;
			check_result = is_player_facing_scroll(check_result, target_heading, (uint16_t)sine_entry);
			if ((uint8_t)check_result != 0u)
				players_facing_scroll |= 2u;
		}
		if ((players_in_play_bits & 4u) != 0u)
		{
			target_heading = PLAYER(r14)->target_heading;
			check_result = is_player_facing_scroll(check_result, target_heading, (uint16_t)sine_entry);
			if ((uint8_t)check_result != 0u)
				players_facing_scroll |= 4u;
		}
		player_at_trailing_edge = 0u;
		if ((OBJECT(object)->flags & 0x400u) == 0u)
		{
			check_result = is_player_pushing_edge(check_result, object);
			if ((uint8_t)check_result != 0u)
				players_pushing_edge |= 1u;
		}
		if ((OBJECT(object_2)->flags & 0x400u) == 0u)
		{
			object = object_2;
			check_result = is_player_pushing_edge(check_result, object);
			if ((uint8_t)check_result != 0u)
				players_pushing_edge |= 2u;
		}
		if ((OBJECT(object_3)->flags & 0x400u) == 0u)
		{
			object = object_3;
			check_result = is_player_pushing_edge(check_result, object);
			if ((uint8_t)check_result != 0u)
				players_pushing_edge |= 4u;
		}
		if (players_pushing_edge == 0u || (int8_t)players_pushing_edge > 7 || player_at_trailing_edge != 0u)
		{
			if (player_at_trailing_edge != 0u && (int16_t)((uint32_t)ROM16(section_end_list) - scroll_position) <= 0x180)
			{
				if ((int8_t)back_edge_frames >= 0x40)
					follow_base_speed = 6u;
				else
					back_edge_frames++;
			}
			push_speed = 0u;
			push_speed &= 0xFFu;
			push_speed = (uint16_t)(push_speed + follow_base_speed);
			advance_scroll(&r2_2, &push_speed, &r4_2, &r5_2, &player, &sine_entry);
			*p_r2 = r2_2;
			*p_speed = push_speed;
			*p_r4 = r4_2;
			*p_r5 = r5_2;
			*p_r8 = object_2;
			*p_r9 = object_3;
			*p_end_script = player;
			*p_r11 = sine_entry;
			return;
		}
		players_pushing_edge &= players_facing_scroll;
		players_pushing_edge &= players_in_play_bits;
		object = LINKED(object, PLAYER(player)->object);
		switch (players_pushing_edge)
		{
		case 0:
		case 8:
			if (players_facing_scroll != 0u || player_past_leading_edge == 0u)
			{
				speed = 5u;
				speed = (uint16_t)(speed + follow_base_speed);
				advance_scroll(&r2_2, &speed, &r4_2, &r5_2, &player, &sine_entry);
				*p_r2 = r2_2;
				*p_speed = speed;
				*p_r4 = r4_2;
				*p_r5 = r5_2;
				*p_r8 = object_2;
				*p_r9 = object_3;
				*p_end_script = player;
				*p_r11 = sine_entry;
				return;
			}
			speed = 5u;
			speed = (uint16_t)(speed + follow_base_speed);
			advance_scroll(&r2_2, &speed, &r4_2, &r5_2, &player, &sine_entry);
			*p_r2 = r2_2;
			*p_speed = speed;
			*p_r4 = r4_2;
			*p_r5 = r5_2;
			*p_r8 = object_2;
			*p_r9 = object_3;
			*p_end_script = player;
			*p_r11 = sine_entry;
			return;
		case 1:
		case 9:
			player_speed = (int32_t)OBJECT(object)->speed >> 16;
			if ((int32_t)player_speed > 0x500 || (int32_t)player_speed < 0)
			{
				speed = 5u;
				speed = (uint16_t)(speed + follow_base_speed);
				advance_scroll(&r2_2, &speed, &r4_2, &r5_2, &player, &sine_entry);
				*p_r2 = r2_2;
				*p_speed = speed;
				*p_r4 = r4_2;
				*p_r5 = r5_2;
				*p_r8 = object_2;
				*p_r9 = object_3;
				*p_end_script = player;
				*p_r11 = sine_entry;
				return;
			}
			push_speed = ((uint16_t)player_speed * 0x400u) >> 16;
			if (player_past_leading_edge == 0u)
			{
				push_speed &= 0xFFu;
				push_speed = (uint16_t)(push_speed + follow_base_speed);
				advance_scroll(&r2_2, &push_speed, &r4_2, &r5_2, &player, &sine_entry);
				*p_r2 = r2_2;
				*p_speed = push_speed;
				*p_r4 = r4_2;
				*p_r5 = r5_2;
				*p_r8 = object_2;
				*p_r9 = object_3;
				*p_end_script = player;
				*p_r11 = sine_entry;
				return;
			}
			push_speed = (push_speed & 0xFFu) + 5u;
			push_speed &= 0xFFu;
			push_speed = (uint16_t)(push_speed + follow_base_speed);
			advance_scroll(&r2_2, &push_speed, &r4_2, &r5_2, &player, &sine_entry);
			*p_r2 = r2_2;
			*p_speed = push_speed;
			*p_r4 = r4_2;
			*p_r5 = r5_2;
			*p_r8 = object_2;
			*p_r9 = object_3;
			*p_end_script = player;
			*p_r11 = sine_entry;
			return;
		case 2:
		case 10:
			object = LINKED(object, PLAYER(player_2)->object);
			player_speed = (int32_t)OBJECT(object)->speed >> 16;
			if ((int32_t)player_speed > 0x500 || (int32_t)player_speed < 0)
			{
				speed = 5u;
				speed = (uint16_t)(speed + follow_base_speed);
				advance_scroll(&r2_2, &speed, &r4_2, &r5_2, &player, &sine_entry);
				*p_r2 = r2_2;
				*p_speed = speed;
				*p_r4 = r4_2;
				*p_r5 = r5_2;
				*p_r8 = object_2;
				*p_r9 = object_3;
				*p_end_script = player;
				*p_r11 = sine_entry;
				return;
			}
			push_speed = ((uint16_t)player_speed * 0x400u) >> 16;
			if (player_past_leading_edge != 0u)
				push_speed = (push_speed & 0xFFu) + 5u;
			push_speed &= 0xFFu;
			push_speed = (uint16_t)(push_speed + follow_base_speed);
			advance_scroll(&r2_2, &push_speed, &r4_2, &r5_2, &player, &sine_entry);
			*p_r2 = r2_2;
			*p_speed = push_speed;
			*p_r4 = r4_2;
			*p_r5 = r5_2;
			*p_r8 = object_2;
			*p_r9 = object_3;
			*p_end_script = player;
			*p_r11 = sine_entry;
			return;
		case 3:
			if (OBJECT(object_2)->speed < OBJECT(object)->speed)
			{
				player_speed = (int32_t)OBJECT(object)->speed >> 16;
				if ((int32_t)player_speed > 0x500 || (int32_t)player_speed < 0)
				{
					speed = 5u;
					speed = (uint16_t)(speed + follow_base_speed);
					advance_scroll(&r2_2, &speed, &r4_2, &r5_2, &player, &sine_entry);
					*p_r2 = r2_2;
					*p_speed = speed;
					*p_r4 = r4_2;
					*p_r5 = r5_2;
					*p_r8 = object_2;
					*p_r9 = object_3;
					*p_end_script = player;
					*p_r11 = sine_entry;
					return;
				}
				push_speed = ((uint16_t)player_speed * 0x400u) >> 16;
				if (player_past_leading_edge != 0u)
					push_speed = (push_speed & 0xFFu) + 5u;
				push_speed &= 0xFFu;
				push_speed = (uint16_t)(push_speed + follow_base_speed);
				advance_scroll(&r2_2, &push_speed, &r4_2, &r5_2, &player, &sine_entry);
				*p_r2 = r2_2;
				*p_speed = push_speed;
				*p_r4 = r4_2;
				*p_r5 = r5_2;
				*p_r8 = object_2;
				*p_r9 = object_3;
				*p_end_script = player;
				*p_r11 = sine_entry;
				return;
			}
			object = object_2;
			player_speed = (int32_t)OBJECT(object)->speed >> 16;
			if ((int32_t)player_speed > 0x500 || (int32_t)player_speed < 0)
			{
				speed = 5u;
				speed = (uint16_t)(speed + follow_base_speed);
				advance_scroll(&r2_2, &speed, &r4_2, &r5_2, &player, &sine_entry);
				*p_r2 = r2_2;
				*p_speed = speed;
				*p_r4 = r4_2;
				*p_r5 = r5_2;
				*p_r8 = object_2;
				*p_r9 = object_3;
				*p_end_script = player;
				*p_r11 = sine_entry;
				return;
			}
			push_speed = ((uint16_t)player_speed * 0x400u) >> 16;
			if (player_past_leading_edge != 0u)
				push_speed = (push_speed & 0xFFu) + 5u;
			push_speed &= 0xFFu;
			push_speed = (uint16_t)(push_speed + follow_base_speed);
			advance_scroll(&r2_2, &push_speed, &r4_2, &r5_2, &player, &sine_entry);
			*p_r2 = r2_2;
			*p_speed = push_speed;
			*p_r4 = r4_2;
			*p_r5 = r5_2;
			*p_r8 = object_2;
			*p_r9 = object_3;
			*p_end_script = player;
			*p_r11 = sine_entry;
			return;
		case 4:
			object = LINKED(object, PLAYER(r14)->object);
			player_speed = (int32_t)OBJECT(object)->speed >> 16;
			if ((int32_t)player_speed > 0x500 || (int32_t)player_speed < 0)
			{
				speed = 5u;
				speed = (uint16_t)(speed + follow_base_speed);
				advance_scroll(&r2_2, &speed, &r4_2, &r5_2, &player, &sine_entry);
				*p_r2 = r2_2;
				*p_speed = speed;
				*p_r4 = r4_2;
				*p_r5 = r5_2;
				*p_r8 = object_2;
				*p_r9 = object_3;
				*p_end_script = player;
				*p_r11 = sine_entry;
				return;
			}
			push_speed = ((uint16_t)player_speed * 0x400u) >> 16;
			if (player_past_leading_edge != 0u)
				push_speed = (push_speed & 0xFFu) + 5u;
			push_speed &= 0xFFu;
			push_speed = (uint16_t)(push_speed + follow_base_speed);
			advance_scroll(&r2_2, &push_speed, &r4_2, &r5_2, &player, &sine_entry);
			*p_r2 = r2_2;
			*p_speed = push_speed;
			*p_r4 = r4_2;
			*p_r5 = r5_2;
			*p_r8 = object_2;
			*p_r9 = object_3;
			*p_end_script = player;
			*p_r11 = sine_entry;
			return;
		case 5:
			if (OBJECT(object_3)->speed < OBJECT(object)->speed)
			{
				player_speed = (int32_t)OBJECT(object)->speed >> 16;
				if ((int32_t)player_speed > 0x500 || (int32_t)player_speed < 0)
				{
					speed = 5u;
					speed = (uint16_t)(speed + follow_base_speed);
					advance_scroll(&r2_2, &speed, &r4_2, &r5_2, &player, &sine_entry);
					*p_r2 = r2_2;
					*p_speed = speed;
					*p_r4 = r4_2;
					*p_r5 = r5_2;
					*p_r8 = object_2;
					*p_r9 = object_3;
					*p_end_script = player;
					*p_r11 = sine_entry;
					return;
				}
				push_speed = ((uint16_t)player_speed * 0x400u) >> 16;
				if (player_past_leading_edge != 0u)
					push_speed = (push_speed & 0xFFu) + 5u;
				push_speed &= 0xFFu;
				push_speed = (uint16_t)(push_speed + follow_base_speed);
				advance_scroll(&r2_2, &push_speed, &r4_2, &r5_2, &player, &sine_entry);
				*p_r2 = r2_2;
				*p_speed = push_speed;
				*p_r4 = r4_2;
				*p_r5 = r5_2;
				*p_r8 = object_2;
				*p_r9 = object_3;
				*p_end_script = player;
				*p_r11 = sine_entry;
				return;
			}
			object = object_3;
			player_speed = (int32_t)OBJECT(object)->speed >> 16;
			if ((int32_t)player_speed > 0x500 || (int32_t)player_speed < 0)
			{
				speed = 5u;
				speed = (uint16_t)(speed + follow_base_speed);
				advance_scroll(&r2_2, &speed, &r4_2, &r5_2, &player, &sine_entry);
				*p_r2 = r2_2;
				*p_speed = speed;
				*p_r4 = r4_2;
				*p_r5 = r5_2;
				*p_r8 = object_2;
				*p_r9 = object_3;
				*p_end_script = player;
				*p_r11 = sine_entry;
				return;
			}
			push_speed = ((uint16_t)player_speed * 0x400u) >> 16;
			if (player_past_leading_edge != 0u)
				push_speed = (push_speed & 0xFFu) + 5u;
			push_speed &= 0xFFu;
			push_speed = (uint16_t)(push_speed + follow_base_speed);
			advance_scroll(&r2_2, &push_speed, &r4_2, &r5_2, &player, &sine_entry);
			*p_r2 = r2_2;
			*p_speed = push_speed;
			*p_r4 = r4_2;
			*p_r5 = r5_2;
			*p_r8 = object_2;
			*p_r9 = object_3;
			*p_end_script = player;
			*p_r11 = sine_entry;
			return;
		case 6:
			object = object_3;
			if (OBJECT(object_2)->speed < OBJECT(object)->speed)
			{
				player_speed = (int32_t)OBJECT(object)->speed >> 16;
				if ((int32_t)player_speed > 0x500 || (int32_t)player_speed < 0)
				{
					speed = 5u;
					speed = (uint16_t)(speed + follow_base_speed);
					advance_scroll(&r2_2, &speed, &r4_2, &r5_2, &player, &sine_entry);
					*p_r2 = r2_2;
					*p_speed = speed;
					*p_r4 = r4_2;
					*p_r5 = r5_2;
					*p_r8 = object_2;
					*p_r9 = object_3;
					*p_end_script = player;
					*p_r11 = sine_entry;
					return;
				}
				push_speed = ((uint16_t)player_speed * 0x400u) >> 16;
				if (player_past_leading_edge != 0u)
					push_speed = (push_speed & 0xFFu) + 5u;
				push_speed &= 0xFFu;
				push_speed = (uint16_t)(push_speed + follow_base_speed);
				advance_scroll(&r2_2, &push_speed, &r4_2, &r5_2, &player, &sine_entry);
				*p_r2 = r2_2;
				*p_speed = push_speed;
				*p_r4 = r4_2;
				*p_r5 = r5_2;
				*p_r8 = object_2;
				*p_r9 = object_3;
				*p_end_script = player;
				*p_r11 = sine_entry;
				return;
			}
			object = object_2;
			player_speed = (int32_t)OBJECT(object)->speed >> 16;
			if ((int32_t)player_speed > 0x500 || (int32_t)player_speed < 0)
			{
				speed = 5u;
				speed = (uint16_t)(speed + follow_base_speed);
				advance_scroll(&r2_2, &speed, &r4_2, &r5_2, &player, &sine_entry);
				*p_r2 = r2_2;
				*p_speed = speed;
				*p_r4 = r4_2;
				*p_r5 = r5_2;
				*p_r8 = object_2;
				*p_r9 = object_3;
				*p_end_script = player;
				*p_r11 = sine_entry;
				return;
			}
			push_speed = ((uint16_t)player_speed * 0x400u) >> 16;
			if (player_past_leading_edge != 0u)
				push_speed = (push_speed & 0xFFu) + 5u;
			push_speed &= 0xFFu;
			push_speed = (uint16_t)(push_speed + follow_base_speed);
			advance_scroll(&r2_2, &push_speed, &r4_2, &r5_2, &player, &sine_entry);
			*p_r2 = r2_2;
			*p_speed = push_speed;
			*p_r4 = r4_2;
			*p_r5 = r5_2;
			*p_r8 = object_2;
			*p_r9 = object_3;
			*p_end_script = player;
			*p_r11 = sine_entry;
			return;
		case 7:
			if (OBJECT(object)->speed <= OBJECT(object_2)->speed)
				object = object_2;
			if (OBJECT(object_3)->speed < OBJECT(object)->speed)
			{
				player_speed = (int32_t)OBJECT(object)->speed >> 16;
				if ((int32_t)player_speed > 0x500 || (int32_t)player_speed < 0)
				{
					speed = 5u;
					speed = (uint16_t)(speed + follow_base_speed);
					advance_scroll(&r2_2, &speed, &r4_2, &r5_2, &player, &sine_entry);
					*p_r2 = r2_2;
					*p_speed = speed;
					*p_r4 = r4_2;
					*p_r5 = r5_2;
					*p_r8 = object_2;
					*p_r9 = object_3;
					*p_end_script = player;
					*p_r11 = sine_entry;
					return;
				}
				push_speed = ((uint16_t)player_speed * 0x400u) >> 16;
				if (player_past_leading_edge != 0u)
					push_speed = (push_speed & 0xFFu) + 5u;
				push_speed &= 0xFFu;
				push_speed = (uint16_t)(push_speed + follow_base_speed);
				advance_scroll(&r2_2, &push_speed, &r4_2, &r5_2, &player, &sine_entry);
				*p_r2 = r2_2;
				*p_speed = push_speed;
				*p_r4 = r4_2;
				*p_r5 = r5_2;
				*p_r8 = object_2;
				*p_r9 = object_3;
				*p_end_script = player;
				*p_r11 = sine_entry;
				return;
			}
			object = object_3;
			player_speed = (int32_t)OBJECT(object)->speed >> 16;
			if ((int32_t)player_speed > 0x500 || (int32_t)player_speed < 0)
			{
				speed = 5u;
				speed = (uint16_t)(speed + follow_base_speed);
				advance_scroll(&r2_2, &speed, &r4_2, &r5_2, &player, &sine_entry);
				*p_r2 = r2_2;
				*p_speed = speed;
				*p_r4 = r4_2;
				*p_r5 = r5_2;
				*p_r8 = object_2;
				*p_r9 = object_3;
				*p_end_script = player;
				*p_r11 = sine_entry;
				return;
			}
			push_speed = ((uint16_t)player_speed * 0x400u) >> 16;
			if (player_past_leading_edge != 0u)
				push_speed = (push_speed & 0xFFu) + 5u;
			push_speed &= 0xFFu;
			push_speed = (uint16_t)(push_speed + follow_base_speed);
			advance_scroll(&r2_2, &push_speed, &r4_2, &r5_2, &player, &sine_entry);
			*p_r2 = r2_2;
			*p_speed = push_speed;
			*p_r4 = r4_2;
			*p_r5 = r5_2;
			*p_r8 = object_2;
			*p_r9 = object_3;
			*p_end_script = player;
			*p_r11 = sine_entry;
			return;
		default:
			unknown_code(0x16151u + players_pushing_edge * 3u);
		}
		break;
	default:
		unknown_code(0x15FA9u + scroll_mode * 3u);
	}
}

uint32_t is_player_pushing_edge(uint32_t pushing, uint32_t object)
{
	uint16_t edge_offset = 0;
	uint32_t current_position = 0;

	switch (push_edge_case)
	{
	case 0:
	case 9:
		pushing &= ~0xFFu;
		return pushing;
	case 1:
		pushing = is_player_at_left_edge(pushing, object);
		if ((uint8_t)pushing != 0u)
		{
			player_at_trailing_edge = 1u;
			pushing &= ~0xFFu;
			return pushing;
		}
		pushing = is_player_at_right_edge(pushing, object);
		if ((uint8_t)pushing != 0u)
		{
			pushing = (pushing & ~0xFFu) | 1u;
			return pushing;
		}
		pushing &= ~0xFFu;
		return pushing;
	case 2:
		pushing = is_player_at_left_edge(pushing, object);
		if ((uint8_t)pushing == 0u)
		{
			pushing = is_player_at_top_edge(pushing, object);
			if ((uint8_t)pushing != 0u)
			{
				player_at_trailing_edge = 1u;
				pushing &= ~0xFFu;
				return pushing;
			}
			edge_offset = 0xF0u;
			pushing = is_player_at_diagonal_edge_f0(pushing, object, edge_offset);
			if ((uint8_t)pushing != 0u)
			{
				pushing = (pushing & ~0xFFu) | 1u;
				return pushing;
			}
			pushing = is_player_at_right_edge(pushing, object);
			if ((uint8_t)pushing != 0u)
			{
				pushing = (pushing & ~0xFFu) | 1u;
				return pushing;
			}
			pushing = is_player_at_bottom_edge(pushing, object);
			if ((uint8_t)pushing != 0u)
			{
				pushing = (pushing & ~0xFFu) | 1u;
				return pushing;
			}
			pushing &= ~0xFFu;
			return pushing;
		}
		player_at_trailing_edge = 1u;
		pushing &= ~0xFFu;
		return pushing;
	case 3:
		pushing = is_player_at_top_edge(pushing, object);
		if ((uint8_t)pushing != 0u)
		{
			player_at_trailing_edge = 1u;
			pushing &= ~0xFFu;
			return pushing;
		}
		pushing = is_player_at_bottom_edge(pushing, object);
		if ((uint8_t)pushing != 0u)
		{
			pushing = (pushing & ~0xFFu) | 1u;
			return pushing;
		}
		pushing &= ~0xFFu;
		return pushing;
	case 4:
		pushing = is_player_at_right_edge(pushing, object);
		if ((uint8_t)pushing == 0u)
		{
			pushing = is_player_at_top_edge(pushing, object);
			if ((uint8_t)pushing != 0u)
			{
				player_at_trailing_edge = 1u;
				pushing &= ~0xFFu;
				return pushing;
			}
			edge_offset = 0x60u;
			pushing = is_player_at_diagonal_edge_60(pushing, object, edge_offset);
			if ((uint8_t)pushing != 0u)
			{
				pushing = (pushing & ~0xFFu) | 1u;
				return pushing;
			}
			pushing = is_player_at_left_edge(pushing, object);
			if ((uint8_t)pushing != 0u)
			{
				pushing = (pushing & ~0xFFu) | 1u;
				return pushing;
			}
			pushing = is_player_at_bottom_edge(pushing, object);
			if ((uint8_t)pushing != 0u)
			{
				pushing = (pushing & ~0xFFu) | 1u;
				return pushing;
			}
			pushing &= ~0xFFu;
			return pushing;
		}
		player_at_trailing_edge = 1u;
		pushing &= ~0xFFu;
		return pushing;
	case 5:
		pushing = is_player_at_right_edge(pushing, object);
		if ((uint8_t)pushing != 0u)
		{
			player_at_trailing_edge = 1u;
			pushing &= ~0xFFu;
			return pushing;
		}
		pushing = is_player_at_left_edge(pushing, object);
		if ((uint8_t)pushing != 0u)
		{
			pushing = (pushing & ~0xFFu) | 1u;
			return pushing;
		}
		pushing &= ~0xFFu;
		return pushing;
	case 6:
		pushing = is_player_at_bottom_edge(pushing, object);
		if ((uint8_t)pushing == 0u)
		{
			pushing = is_player_at_right_edge(pushing, object);
			if ((uint8_t)pushing != 0u)
			{
				player_at_trailing_edge = 1u;
				pushing &= ~0xFFu;
				return pushing;
			}
			edge_offset = 0xC0u;
			pushing = is_player_at_diagonal_edge_c0(pushing, object, edge_offset);
			if ((uint8_t)pushing != 0u)
			{
				pushing = (pushing & ~0xFFu) | 1u;
				return pushing;
			}
			pushing = is_player_at_left_edge(pushing, object);
			if ((uint8_t)pushing != 0u)
			{
				pushing = (pushing & ~0xFFu) | 1u;
				return pushing;
			}
			pushing = is_player_at_top_edge(pushing, object);
			if ((uint8_t)pushing != 0u)
			{
				pushing = (pushing & ~0xFFu) | 1u;
				return pushing;
			}
			pushing &= ~0xFFu;
			return pushing;
		}
		player_at_trailing_edge = 1u;
		pushing &= ~0xFFu;
		return pushing;
	case 7:
		pushing = is_player_at_bottom_edge(pushing, object);
		if ((uint8_t)pushing != 0u)
		{
			player_at_trailing_edge = 1u;
			pushing &= ~0xFFu;
			return pushing;
		}
		pushing = is_player_at_top_edge(pushing, object);
		if ((uint8_t)pushing != 0u)
		{
			pushing = (pushing & ~0xFFu) | 1u;
			return pushing;
		}
		pushing &= ~0xFFu;
		return pushing;
	case 8:
		pushing = is_player_at_bottom_edge(pushing, object);
		if ((uint8_t)pushing == 0u)
		{
			pushing = is_player_at_left_edge(pushing, object);
			if ((uint8_t)pushing != 0u)
			{
				player_at_trailing_edge = 1u;
				pushing &= ~0xFFu;
				return pushing;
			}
			if (stage != 3u)
			{
				edge_offset = 0x30u;
				pushing = is_player_at_diagonal_edge_30(pushing, object, edge_offset);
				if ((uint8_t)pushing != 0u)
				{
					pushing = (pushing & ~0xFFu) | 1u;
					return pushing;
				}
				pushing = is_player_at_right_edge(pushing, object);
				if ((uint8_t)pushing != 0u)
				{
					pushing = (pushing & ~0xFFu) | 1u;
					return pushing;
				}
				pushing = is_player_at_top_edge(pushing, object);
				if ((uint8_t)pushing != 0u)
				{
					pushing = (pushing & ~0xFFu) | 1u;
					return pushing;
				}
				pushing &= ~0xFFu;
				return pushing;
			}
			current_position = scroll_position;
			if ((int32_t)current_position < 0x87A9 || (int32_t)current_position > 0x96F1)
			{
				edge_offset = 0x30u;
				pushing = is_player_at_diagonal_edge_30(pushing, object, edge_offset);
				if ((uint8_t)pushing != 0u)
				{
					pushing = (pushing & ~0xFFu) | 1u;
					return pushing;
				}
				pushing = is_player_at_right_edge(pushing, object);
				if ((uint8_t)pushing != 0u)
				{
					pushing = (pushing & ~0xFFu) | 1u;
					return pushing;
				}
				pushing = is_player_at_top_edge(pushing, object);
				if ((uint8_t)pushing != 0u)
				{
					pushing = (pushing & ~0xFFu) | 1u;
					return pushing;
				}
				pushing &= ~0xFFu;
				return pushing;
			}
			pushing = is_player_at_top_edge(pushing, object);
			if ((uint8_t)pushing != 0u)
			{
				pushing = (pushing & ~0xFFu) | 1u;
				return pushing;
			}
			pushing &= ~0xFFu;
			return pushing;
		}
		player_at_trailing_edge = 1u;
		pushing &= ~0xFFu;
		return pushing;
	default:
		unknown_code(0x16274u + push_edge_case * 3u);
	}
}

uint32_t is_player_at_diagonal_edge_30(uint32_t at_edge, uint32_t object, uint16_t edge_offset)
{
	uint16_t screen_y = 0;
	uint32_t passes_left = 0;

	passes_left = 2u;
	at_edge &= ~0xFFFFu;
	player_past_leading_edge = 0u;
	do
	{
		screen_y = (uint16_t)OBJECT(object)->screen_y;
		if ((int16_t)(((int16_t)OBJECT(object)->screen_x >> 1) + edge_offset) < (int16_t)screen_y)
		{
			if ((int16_t)((uint32_t)(screen_y - edge_offset) << 1) <= OBJECT(object)->screen_x)
			{
				at_edge = (at_edge & ~0xFFu) | 1u;
				if ((uint16_t)passes_left == 1u)
					player_past_leading_edge = 1u;
			}
		}
		else
		{
			at_edge = (at_edge & ~0xFFu) | 1u;
			if ((uint16_t)passes_left == 1u)
				player_past_leading_edge = 1u;
		}
		edge_offset -= 0xAu;
		passes_left--;
	} while (passes_left != 0u);
	return at_edge;
}

uint32_t is_player_at_diagonal_edge_f0(uint32_t at_edge, uint32_t object, uint16_t edge_offset)
{
	uint16_t screen_y = 0;
	uint32_t passes_left = 0;

	passes_left = 2u;
	at_edge &= ~0xFFFFu;
	player_past_leading_edge = 0u;
	do
	{
		screen_y = (uint16_t)OBJECT(object)->screen_y;
		if ((int16_t)screen_y < (int16_t)(0u - ((int16_t)OBJECT(object)->screen_x >> 1) + edge_offset))
		{
			if ((int16_t)((0u - (screen_y - edge_offset)) << 1) <= OBJECT(object)->screen_x)
			{
				at_edge = (at_edge & ~0xFFu) | 1u;
				if ((uint16_t)passes_left == 1u)
					player_past_leading_edge = 1u;
			}
		}
		else
		{
			at_edge = (at_edge & ~0xFFu) | 1u;
			if ((uint16_t)passes_left == 1u)
				player_past_leading_edge = 1u;
		}
		edge_offset -= 0xAu;
		passes_left--;
	} while (passes_left != 0u);
	return at_edge;
}

uint32_t is_player_at_diagonal_edge_c0(uint32_t at_edge, uint32_t object, uint16_t edge_offset)
{
	uint16_t screen_y = 0;
	uint32_t passes_left = 0;

	passes_left = 2u;
	at_edge &= ~0xFFFFu;
	player_past_leading_edge = 0u;
	do
	{
		screen_y = (uint16_t)OBJECT(object)->screen_y;
		if ((int16_t)(0u - ((int16_t)OBJECT(object)->screen_x >> 1) + edge_offset) < (int16_t)screen_y)
		{
			if (OBJECT(object)->screen_x <= (int16_t)((0u - (screen_y - edge_offset)) << 1))
			{
				at_edge = (at_edge & ~0xFFu) | 1u;
				if ((uint16_t)passes_left == 1u)
					player_past_leading_edge = 1u;
			}
		}
		else
		{
			at_edge = (at_edge & ~0xFFu) | 1u;
			if ((uint16_t)passes_left == 1u)
				player_past_leading_edge = 1u;
		}
		edge_offset -= 0xAu;
		passes_left--;
	} while (passes_left != 0u);
	return at_edge;
}

uint32_t is_player_at_diagonal_edge_60(uint32_t at_edge, uint32_t object, uint16_t edge_offset)
{
	uint16_t screen_y = 0;
	uint32_t passes_left = 0;

	passes_left = 2u;
	at_edge &= ~0xFFFFu;
	player_past_leading_edge = 0u;
	do
	{
		screen_y = (uint16_t)OBJECT(object)->screen_y;
		if ((int16_t)screen_y < (int16_t)(((int16_t)OBJECT(object)->screen_x >> 1) + edge_offset))
		{
			if (OBJECT(object)->screen_x <= (int16_t)((uint32_t)(screen_y - edge_offset) << 1))
			{
				at_edge = (at_edge & ~0xFFu) | 1u;
				if ((uint16_t)passes_left == 1u)
					player_past_leading_edge = 1u;
			}
		}
		else
		{
			at_edge = (at_edge & ~0xFFu) | 1u;
			if ((uint16_t)passes_left == 1u)
				player_past_leading_edge = 1u;
		}
		edge_offset -= 0xAu;
		passes_left--;
	} while (passes_left != 0u);
	return at_edge;
}

uint32_t is_player_at_right_edge(uint32_t at_edge, uint32_t object)
{
	uint32_t leading_limit = 0;

	if (OBJECT(object)->screen_x < (int16_t)push_edge_right)
	{
		at_edge &= ~0xFFu;
		return at_edge;
	}
	leading_limit = (at_edge & ~0xFFFFu) | push_edge_right;
	leading_limit = (leading_limit & ~0xFFFFu) | (uint16_t)(leading_limit + 0xAu);
	if ((int16_t)leading_limit <= OBJECT(object)->screen_x)
		player_past_leading_edge = 1u;
	at_edge = (leading_limit & ~0xFFu) | 1u;
	return at_edge;
}

uint32_t is_player_at_left_edge(uint32_t at_edge, uint32_t object)
{
	uint32_t leading_limit = 0;

	if ((int16_t)push_edge_left < OBJECT(object)->screen_x)
	{
		at_edge &= ~0xFFu;
		return at_edge;
	}
	leading_limit = (at_edge & ~0xFFFFu) | push_edge_left;
	leading_limit = (leading_limit & ~0xFFFFu) | (uint16_t)(leading_limit - 0xAu);
	if (OBJECT(object)->screen_x <= (int16_t)leading_limit)
		player_past_leading_edge = 1u;
	at_edge = (leading_limit & ~0xFFu) | 1u;
	return at_edge;
}

uint32_t is_player_at_top_edge(uint32_t at_edge, uint32_t object)
{
	uint32_t leading_limit = 0;

	if ((int16_t)push_edge_top < OBJECT(object)->screen_y)
	{
		at_edge &= ~0xFFu;
		return at_edge;
	}
	leading_limit = (at_edge & ~0xFFFFu) | push_edge_top;
	leading_limit = (leading_limit & ~0xFFFFu) | (uint16_t)(leading_limit - 0xAu);
	if (OBJECT(object)->screen_y <= (int16_t)leading_limit)
		player_past_leading_edge = 1u;
	at_edge = (leading_limit & ~0xFFu) | 1u;
	return at_edge;
}

uint32_t is_player_at_bottom_edge(uint32_t at_edge, uint32_t object)
{
	uint32_t leading_limit = 0;

	if (OBJECT(object)->screen_y < (int16_t)push_edge_bottom)
	{
		at_edge &= ~0xFFu;
		return at_edge;
	}
	leading_limit = (at_edge & ~0xFFFFu) | push_edge_bottom;
	leading_limit = (leading_limit & ~0xFFFFu) | (uint16_t)(leading_limit + 0xAu);
	if ((int16_t)leading_limit <= OBJECT(object)->screen_y)
		player_past_leading_edge = 1u;
	at_edge = (leading_limit & ~0xFFu) | 1u;
	return at_edge;
}

void set_screen_push_edges(uint32_t quadrant_bits, uint32_t *p_count, uint32_t *p_sine_entry)
{
	uint32_t count = 0, sine_entry = 0;

	sine_entry = sine_table_ADDRESS;
	if ((int16_t)AS16(section_direction_y) < 0)
	{
		if ((int16_t)AS16(section_direction_x) >= 0)
		{
			sine_entry += 0x200u;
			quadrant_bits |= 8u;
		}
		sine_entry += 0x200u;
		quadrant_bits |= 4u;
		goto set_screen_push_edges_past_quadrant_0;
	}
	if ((int16_t)AS16(section_direction_x) < 0)
	{
	set_screen_push_edges_past_quadrant_0:
		sine_entry += 0x200u;
		quadrant_bits |= 2u;
	}
	count = 0x100u;
	push_edge_top = 0xFFFFu;
	push_edge_bottom = 0xFFFFu;
	push_edge_right = 0xFFFFu;
	push_edge_left = 0xFFFFu;
	switch (quadrant_bits | 1u)
	{
	case 0:
	case 2:
	case 4:
	case 5:
	case 6:
	case 8:
	case 9:
	case 10:
	case 11:
	case 12:
	case 13:
	case 14:
	case 16:
		*p_count = count;
		*p_sine_entry = sine_entry;
		return;
	case 1:
		push_edge_case = 0u;
		if (AS16(section_direction_y) != 0u)
		{
			push_edge_bottom = 0x88u;
			push_edge_top = 0x20u;
			push_edge_case++;
		}
		push_edge_case++;
		push_edge_right = 0xB8u;
		push_edge_left = 0x40u;
		*p_count = count;
		*p_sine_entry = sine_entry;
		return;
	case 3:
		push_edge_case = 2u;
		if (AS16(section_direction_x) != 0u)
		{
			push_edge_left = 0x88u;
			push_edge_right = 0x100u;
			push_edge_case++;
		}
		push_edge_case++;
		push_edge_bottom = 0x88u;
		push_edge_top = 0x20u;
		*p_count = count;
		*p_sine_entry = sine_entry;
		return;
	case 7:
		push_edge_case = 4u;
		if (AS16(section_direction_y) != 0u)
		{
			push_edge_top = 0x58u;
			push_edge_bottom = 0xC0u;
			push_edge_case++;
		}
		push_edge_case++;
		push_edge_left = 0x88u;
		push_edge_right = 0x100u;
		*p_count = count;
		*p_sine_entry = sine_entry;
		return;
	case 15:
		push_edge_case = 6u;
		if (AS16(section_direction_x) != 0u)
		{
			push_edge_right = 0xB8u;
			push_edge_left = 0x40u;
			push_edge_case++;
		}
		push_edge_case++;
		push_edge_top = 0x58u;
		push_edge_bottom = 0xC0u;
		*p_count = count;
		*p_sine_entry = sine_entry;
		return;
	default:
		unknown_code(0x16726u + (quadrant_bits | 1u) * 3u);
	}
}

uint32_t is_player_facing_scroll(uint32_t facing, uint16_t heading, uint16_t sine_entry)
{
	uint16_t sine_offset = 0;

	heading = (uint16_t)((uint32_t)heading << 1);
	sine_offset = (uint16_t)(sine_entry - sine_table_ADDRESS);
	if ((int16_t)sine_offset < 0x200)
	{
		if ((int16_t)heading < 0x100 || (int16_t)heading > 0x500)
		{
			facing &= ~0xFFu;
			return facing;
		}
		facing = (facing & ~0xFFu) | 1u;
		return facing;
	}
	if ((int16_t)sine_offset < 0x400)
	{
		if ((int16_t)heading < 0x300 || (int16_t)heading > 0x700)
		{
			facing &= ~0xFFu;
			return facing;
		}
		facing = (facing & ~0xFFu) | 1u;
		return facing;
	}
	if ((int16_t)sine_offset < 0x600)
	{
		if ((int16_t)heading < 0x500)
			heading += 0x800u;
		if ((int16_t)heading > 0x900)
		{
			facing &= ~0xFFu;
			return facing;
		}
		facing = (facing & ~0xFFu) | 1u;
		return facing;
	}
	if ((int16_t)heading < 0x700)
		heading += 0x800u;
	if ((int16_t)heading > 0xB00)
	{
		facing &= ~0xFFu;
		return facing;
	}
	facing = (facing & ~0xFFu) | 1u;
	return facing;
}

void is_direction_in_quadrant(uint32_t count, uint32_t *p_found, uint32_t *p_sine_entry)
{
	uint32_t sine_entry = *p_sine_entry;
	uint8_t entry_at_least = 0, found = 0, next_at_least = 0;
	uint16_t direction_y = 0, next_entry = 0;

	for (;;)
	{
		if (AS16(section_direction_y) != 0u)
		{
			if (AS16(section_direction_x) == 0u && (AS16(section_direction_y) == 0x4000u || AS16(section_direction_y) == 0xC000u))
			{
				found = 1u;
				*p_found = (*p_found & 0xFFFFFF00u) | found;
				*p_sine_entry = sine_entry;
				return;
			}
		}
		else if (AS16(section_direction_x) == 0x4000u || AS16(section_direction_x) == 0xC000u)
			break;
		direction_y = AS16(section_direction_y);
		next_entry = mem_read16(sine_entry + 2u);
		if ((int16_t)mem_read16(sine_entry) < (int16_t)direction_y)
			entry_at_least = 0u;
		else
			entry_at_least = 1u;
		if ((int16_t)next_entry < (int16_t)direction_y)
			next_at_least = 0u;
		else
			next_at_least = 1u;
		if (next_at_least != entry_at_least)
		{
			found = 1u;
			*p_found = (*p_found & 0xFFFFFF00u) | found;
			*p_sine_entry = sine_entry;
			return;
		}
		sine_entry += 2u;
		count--;
		if (count == 0u)
		{
			found = 0u;
			*p_found = (*p_found & 0xFFFFFF00u) | found;
			*p_sine_entry = sine_entry;
			return;
		}
	}
	found = 1u;
	*p_found = (*p_found & 0xFFFFFF00u) | found;
	*p_sine_entry = sine_entry;
}

void scroll_slowing_down(uint32_t *p_r2, uint32_t *p_speed, uint32_t *p_r4, uint32_t *p_r5, uint32_t *p_r10, uint32_t *p_r11)
{
	uint32_t r2 = *p_r2;
	uint32_t r4 = *p_r4;
	uint32_t r5 = *p_r5;
	uint32_t r10 = *p_r10;
	uint32_t r11 = *p_r11;
	uint32_t speed = 0;

	scroll_mode_timer--;
	if (scroll_mode_timer != 0u || (int16_t)scroll_speed < 2)
	{
		scroll_at_scroll_speed(&r2, &speed, &r4, &r5, &r10, &r11);
		*p_r2 = r2;
		*p_speed = speed;
		*p_r4 = r4;
		*p_r5 = r5;
		*p_r10 = r10;
		*p_r11 = r11;
		return;
	}
	scroll_speed--;
	scroll_mode_timer = scroll_mode_duration;
	scroll_at_scroll_speed(&r2, &speed, &r4, &r5, &r10, &r11);
	*p_r2 = r2;
	*p_speed = speed;
	*p_r4 = r4;
	*p_r5 = r5;
	*p_r10 = r10;
	*p_r11 = r11;
}

void scroll_speeding_up(uint32_t *p_r2, uint32_t *p_speed, uint32_t *p_r4, uint32_t *p_r5, uint32_t *p_r10, uint32_t *p_r11)
{
	uint32_t r2 = *p_r2;
	uint32_t speed = *p_speed;
	uint32_t r4 = *p_r4;
	uint32_t r5 = *p_r5;
	uint32_t r10 = *p_r10;
	uint32_t r11 = *p_r11;
	uint32_t r2_2 = 0, r4_2 = 0;

	scroll_mode_timer--;
	if (scroll_mode_timer != 0u)
	{
		speed = scroll_ramp_speed;
		advance_scroll_unless_ended(&r2, &speed, &r4, &r5, &r10, &r11);
		*p_r2 = r2;
		*p_speed = speed;
		*p_r4 = r4;
		*p_r5 = r5;
		*p_r10 = r10;
		*p_r11 = r11;
		return;
	}
	if ((int8_t)AS8(scroll_speed) <= (int8_t)scroll_ramp_speed)
	{
		scroll_mode = 0u;
		scroll_ramp_speed = 1u;
		set_camera_velocity(&r2_2, &speed, &r4_2, &r5, &r10, &r11);
		*p_r2 = r2_2;
		*p_speed = speed;
		*p_r4 = r4_2;
		*p_r5 = r5;
		*p_r10 = r10;
		*p_r11 = r11;
		return;
	}
	scroll_ramp_speed++;
	scroll_mode_timer = scroll_mode_duration;
	speed = scroll_ramp_speed;
	advance_scroll_unless_ended(&r2, &speed, &r4, &r5, &r10, &r11);
	*p_r2 = r2;
	*p_speed = speed;
	*p_r4 = r4;
	*p_r5 = r5;
	*p_r10 = r10;
	*p_r11 = r11;
}

void scroll_at_scroll_speed(uint32_t *p_r2, uint32_t *p_speed, uint32_t *p_r4, uint32_t *p_r5, uint32_t *p_r10, uint32_t *p_r11)
{
	uint32_t r2 = *p_r2;
	uint32_t r4 = *p_r4;
	uint32_t r5 = *p_r5;
	uint32_t r10 = *p_r10;
	uint32_t r11 = *p_r11;
	uint32_t speed = 0;

	speed = scroll_speed;
	advance_scroll_unless_ended(&r2, &speed, &r4, &r5, &r10, &r11);
	*p_r2 = r2;
	*p_speed = speed;
	*p_r4 = r4;
	*p_r5 = r5;
	*p_r10 = r10;
	*p_r11 = r11;
}

void advance_scroll_unless_ended(uint32_t *p_r2, uint32_t *p_speed, uint32_t *p_r4, uint32_t *p_r5, uint32_t *p_r10, uint32_t *p_r11)
{
	uint32_t r2 = *p_r2;
	uint32_t speed = *p_speed;
	uint32_t r4 = *p_r4;
	uint32_t r5 = *p_r5;
	uint32_t r10 = *p_r10;
	uint32_t r11 = *p_r11;
	uint32_t r2_2 = 0, r4_2 = 0, r5_2 = 0;

	if (scroll_ended < 0)
		return;
	advance_scroll(&r2_2, &speed, &r4_2, &r5_2, &r10, &r11);
	*p_r2 = r2_2;
	*p_speed = speed;
	*p_r4 = r4_2;
	*p_r5 = r5_2;
	*p_r10 = r10;
	*p_r11 = r11;
}

void advance_scroll(uint32_t *p_other_list, uint32_t *p_speed, uint32_t *p_r4, uint32_t *p_next_scroll_position, uint32_t *p_r10, uint32_t *p_r11)
{
	uint32_t speed = *p_speed;
	uint32_t r10 = *p_r10;
	uint32_t r11 = *p_r11;
	uint32_t r2 = 0, r4 = 0, r5 = 0, direction_y_list = 0, limit_x_cursor = 0, limit_y_cursor = 0, list = 0, mode_script_cursor = 0;
	uint16_t direction_y = 0;

	r5 = move_camera_along_path(speed);
	update_camera_view();
	if ((int32_t)scroll_position < (int32_t)ROM16(section_end_list))
	{
		set_camera_velocity(&r2, &speed, &r4, &r5, &r10, &r11);
		*p_other_list = r2;
		*p_speed = speed;
		*p_r4 = r4;
		*p_next_scroll_position = r5;
		*p_r10 = r10;
		*p_r11 = r11;
		return;
	}
	back_edge_frames = 0u;
	list = section_speed_list;
	scroll_speed = ROM8(list);
	section_speed_list = list + 1u;
	list = follow_base_speed_list;
	follow_base_speed = mem_read8(list);
	follow_base_speed_list = list + 1u;
	limit_y_cursor = section_limit_y_list;
	section_limit_y = ROM16(limit_y_cursor);
	section_limit_y_list = limit_y_cursor + 2u;
	limit_x_cursor = section_limit_x_list;
	section_limit_x = ROM16(limit_x_cursor);
	section_limit_x_list = limit_x_cursor + 2u;
	mode_script_cursor = scroll_mode_script;
	scroll_mode = ROM16(mode_script_cursor);
	mode_script_cursor += 2u;
	scroll_mode_timer = ROM16(mode_script_cursor);
	scroll_mode_duration = scroll_mode_timer;
	scroll_mode_script = mode_script_cursor + 2u;
	list = section_direction_x_list;
	direction_y_list = section_direction_y_list;
	direction_y = ROM16(direction_y_list);
	section_direction_x = (int16_t)ROM16(list);
	section_direction_y = (int16_t)direction_y;
	section_direction_x_list = list + 2u;
	section_direction_y_list = direction_y_list + 2u;
	section_end_list += 2u;
	section_unused_cursor += 2u;
	set_camera_velocity(&r2, &speed, &r4, &r5, &r10, &r11);
	*p_other_list = r2;
	*p_speed = speed;
	*p_r4 = r4;
	*p_next_scroll_position = r5;
	*p_r10 = r10;
	*p_r11 = r11;
}

void set_camera_velocity(uint32_t *p_r2, uint32_t *p_speed, uint32_t *p_r4, uint32_t *p_r5, uint32_t *p_r10, uint32_t *p_r11)
{
	uint32_t speed = *p_speed;
	uint32_t r5 = *p_r5;
	uint32_t r10 = *p_r10;
	uint32_t r11 = *p_r11;
	uint32_t r2 = 0, r10_2 = 0, ignored = 0, speed_value = 0, velocity_y = 0;

	speed_value = (uint16_t)speed;
	velocity_y = section_direction_y * speed_value;
	camera_velocity_x = section_direction_x * speed_value;
	camera_velocity_y = velocity_y;
	camera_shake_length = 0x200u;
	camera_shake_wave = 0x200u;
	r2 = update_camera_with_shake();
	if (scroll_ended < 0)
	{
		*p_r2 = r2;
		*p_r4 = speed_value;
		return;
	}
	update_background_scroll(&ignored, &r2, &speed, &speed_value, &r5, &r10_2, &r11);
	*p_r2 = r2;
	*p_speed = speed;
	*p_r4 = speed_value;
	*p_r5 = r5;
	*p_r10 = r10_2;
	*p_r11 = r11;
}

void update_background_scroll(uint32_t *p_r1, uint32_t *p_r2, uint32_t *p_speed, uint32_t *p_r4, uint32_t *p_r5, uint32_t *p_range, uint32_t *p_r11)
{
	uint32_t r2 = *p_r2;
	uint32_t speed = *p_speed;
	uint32_t r4 = *p_r4;
	uint32_t r5 = *p_r5;
	uint32_t r11 = *p_r11;
	uint32_t current_position = 0, fast_scroll_start = 0, range = 0, section_start = 0;

	update_nbg1_scroll();
	update_nbg3_scroll(&r2, &speed, &r4, &r5, &r11);
	speed = update_nbg2_scroll(speed);
	fast_scroll_start = stage;
	range = stage_fast_scroll_ranges[fast_scroll_start] + stage_fast_scroll_ranges_ADDRESS;
	section_start = mem_read16(section_end_list - 2u);
	do
	{
		if (ROM16(range) == 0xFFFFu)
		{
			*p_r1 = fast_scroll_start;
			*p_r2 = r2;
			*p_speed = speed;
			*p_r4 = r4;
			*p_r5 = r5;
			*p_range = range;
			*p_r11 = r11;
			return;
		}
		fast_scroll_start = ROM16(range);
		range += 2u;
	} while ((int32_t)fast_scroll_start <= (int32_t)section_start);
	current_position = scroll_position;
	if ((int32_t)current_position < (int32_t)fast_scroll_start)
	{
		*p_r1 = fast_scroll_start;
		*p_r2 = current_position;
		*p_speed = speed;
		*p_r4 = r4;
		*p_r5 = r5;
		*p_range = range;
		*p_r11 = r11;
		return;
	}
	follow_base_speed = 0x10u;
	*p_r1 = fast_scroll_start;
	*p_r2 = current_position;
	*p_speed = speed;
	*p_r4 = r4;
	*p_r5 = r5;
	*p_range = range;
	*p_r11 = r11;
}

void start_stage_view(uint32_t *p_r2, uint32_t *p_r3, uint32_t *p_r4, uint32_t *p_stage_offset)
{
	uint32_t r2 = *p_r2;
	uint32_t r3 = *p_r3;
	uint32_t r4 = *p_r4;
	uint32_t stage_offset = *p_stage_offset;
	uint32_t r2_2 = 0, r3_2 = 0, r4_2 = 0;

	if (stage != 0x11u)
	{
		unused_scroll_flag = 1u;
		init_stage_view(&r2_2, &r3_2, &r4_2, &stage_offset);
		*p_r2 = r2_2;
		*p_r3 = r3_2;
		*p_r4 = r4_2;
		*p_stage_offset = stage_offset;
		return;
	}
	ending_scene_step = 0u;
}

void run_stage_05_switch(uint32_t r3, uint32_t *p_r2, uint32_t *p_r4, uint32_t *p_dest)
{
	uint32_t r2 = *p_r2;
	uint32_t r4 = *p_r4;
	uint32_t dest = *p_dest;
	uint32_t r1 = 0, r5 = 0, camera_view_y = 0, ignored = 0, layer_scroll_x = 0, script = 0;

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
		update_background_scroll(&ignored, &layer_scroll_x, &r3, &r4, &r5, &ignored, &dest);
		stage = 5u;
		mem_write8(video_layer_disable, mem_read8(video_layer_disable) & 0xE6u);
		stage_05_switch_step++;
		*p_r2 = layer_scroll_x;
		*p_r4 = r4;
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

void update_stage_00_exit_scene(uint32_t dest, uint32_t *p_r4, uint32_t *p_script)
{
	uint32_t r4 = *p_r4;
	uint32_t script = *p_script;
	uint16_t layer_zoom = 0;
	uint32_t r5 = 0, r10 = 0, ignored = 0, layer_scroll_x = 0, scroll_y_past_trigger = 0, speed = 0;
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
		update_background_scroll(&ignored, &layer_scroll_x, &speed, &r4, &r5, &r10, &dest);
		*p_r4 = r4;
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
		update_background_scroll(&ignored, &layer_scroll_x, &speed, &r4, &r5, &r10, &dest);
		*p_r4 = r4;
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
		update_background_scroll(&ignored, &layer_scroll_x, &speed, &r4, &r5, &r10, &dest);
		*p_r4 = r4;
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
		update_background_scroll(&ignored, &layer_scroll_x, &speed, &r4, &r5, &r10, &dest);
		*p_r4 = r4;
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
			update_background_scroll(&ignored, &layer_scroll_x, &speed, &r4, &r5, &r10, &dest);
			*p_r4 = r4;
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
			update_background_scroll(&ignored, &layer_scroll_x, &speed, &r4, &r5, &r10, &dest);
			*p_r4 = r4;
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
			update_background_scroll(&ignored, &layer_scroll_x, &speed, &r4, &r5, &r10, &dest);
			*p_r4 = r4;
			*p_script = r10;
			return;
		}
		stage_00_exit_delay = 0u;
		color_offset_red--;
		color_offset_blue--;
		color_offset_green--;
		if (color_offset_red != 0u)
		{
			update_background_scroll(&ignored, &layer_scroll_x, &speed, &r4, &r5, &r10, &dest);
			*p_r4 = r4;
			*p_script = r10;
			return;
		}
		stage_00_exit_step++;
		*p_script = script;
		return;
	case 10:
		update_background_scroll(&ignored, &layer_scroll_x, &speed, &r4, &r5, &r10, &dest);
		if ((int16_t)stage_00_exit_zoom < 0x42)
			stage_00_exit_zoom = 0x42u;
		stage_00_exit_zoom--;
		layer_zoom = (uint16_t)(divide_s32(0x2000000u, stage_00_exit_zoom) >> 8);
		mem_write16(nbg0_zoom_x, layer_zoom);
		mem_write16(nbg0_zoom_y, layer_zoom);
		mem_write16(nbg1_zoom_x, layer_zoom);
		mem_write16(nbg1_zoom_y, layer_zoom);
		*p_r4 = r4;
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

void update_stage_06_background(uint32_t r3, uint32_t dest, uint32_t *p_rolls_left, uint32_t *p_r9, uint32_t *p_r10)
{
	uint32_t rolls_left = *p_rolls_left;
	uint32_t r9 = *p_r9;
	uint32_t r10 = *p_r10;
	uint8_t sound_id = 0;
	uint16_t shake_y = 0;
	uint32_t r1 = 0, r2 = 0, r10_2 = 0, cycle_set = 0, ignored = 0, palette_stage = 0, script = 0;
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
		load_palette_set_and_backdrop(palette_stage, &ignored, &r10_2, &dest);
		if (mem_read16(palette_color_61D0) != 0x7B0u)
		{
			mixer_blend = 0xA4u;
			*p_r10 = r10_2;
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
		*p_r10 = script;
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
		*p_r10 = script;
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
		play_sound(sound_id, &r1, &r2);
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
		*p_r10 = script;
		return;
	case 6:
		if (palette_cycle_set_0_frame != 0u)
		{
			r10 = cycle_palette_set_0(r10);
			r10 = cycle_palette_set_1(r10);
			*p_rolls_left = rolls_left;
			*p_r9 = r9;
			*p_r10 = r10;
			return;
		}
		if (palette_cycle_set_1_frame == 0u)
		{
			rolls_left = 2u;
			do
			{
				next_random(&r3, &r9, &r10);
				r3 &= 0xFu;
				if ((int8_t)r3 > 2)
				{
					*p_rolls_left = rolls_left;
					*p_r9 = r9;
					*p_r10 = r10;
					return;
				}
				rolls_left--;
			} while (rolls_left != 0u);
			r10 = cycle_palette_set_0(r10);
		}
		r10 = cycle_palette_set_1(r10);
		*p_rolls_left = rolls_left;
		*p_r9 = r9;
		*p_r10 = r10;
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
	uint32_t r1 = 0, r2 = 0;

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
			play_sound(sound_id, &r1, &r2);
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
			play_sound(sound_id, &r1, &r2);
		}
	}
}

void move_camera_to_start_section(uint32_t *p_r2, uint32_t *p_section_offset, uint32_t *p_stage_index)
{
	uint32_t r2 = 0, section_offset = 0, stage_index = 0;

	load_camera_start_section(&section_offset, &stage_index);
	r2 = update_camera_view();
	*p_r2 = r2;
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

void animate_stage_palette(uint32_t *p_r1, uint32_t *p_r2, uint32_t *p_palette_count, uint32_t *p_cycle, uint32_t *p_source, uint32_t *p_dest)
{
	uint32_t r2 = *p_r2;
	uint32_t palette_count = *p_palette_count;
	uint32_t cycle = *p_cycle;
	uint32_t source = *p_source;
	uint32_t dest = *p_dest;
	uint32_t r1 = 0;

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
		animate_palette_stage_04(&r1, &r2, &palette_count, &cycle, &source, &dest);
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
		animate_palette_stage_09(&r1, &r2, &palette_count, &cycle, &source, &dest);
		break;
	case 10:
		animate_palette_stage_0a(&r1, &r2, &palette_count, &cycle, &source, &dest);
		break;
	case 11:
		skip_stage_palette_animation();
		break;
	case 12:
		animate_palette_stage_0c(&r1, &r2, &palette_count, &cycle, &source, &dest);
		break;
	case 13:
		animate_palette_stage_0d_0e(&r1, &r2, &palette_count, &cycle, &source, &dest);
		break;
	case 14:
		animate_palette_stage_0d_0e(&r1, &r2, &palette_count, &cycle, &source, &dest);
		break;
	case 15:
		animate_palette_stage_0f(&r1, &r2, &palette_count, &cycle, &source, &dest);
		break;
	case 16:
		animate_palette_stage_10(&r1, &r2, &palette_count, &cycle, &source, &dest);
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
	*p_r1 = r1;
	*p_r2 = r2;
	*p_palette_count = palette_count;
	*p_cycle = cycle;
	*p_source = source;
	*p_dest = dest;
}

void skip_stage_palette_animation(void)
{
}

void animate_palette_stage_10(uint32_t *p_r1, uint32_t *p_r2, uint32_t *p_r4, uint32_t *p_r9, uint32_t *p_r10, uint32_t *p_r11)
{
	uint32_t r2 = *p_r2;
	uint16_t halfwords = 0;
	uint32_t r1 = 0, cycle = 0, cycle_1 = 0, dest = 0, dest_1 = 0, ignored = 0, palette_count = 0, palette_count_1 = 0, source = 0, source_1 = 0;

	dest = PALETTE_RAM + 0x8B40u;
	palette_count = 6u;
	source = stage_10_cycle_palettes_0_ADDRESS;
	halfwords = 0x10u;
	cycle = palette_cycle_0_ADDRESS;
	palette_cycle_0 = 6u;
	palette_cycle_0_frames = 8u;
	cycle_palette(cycle, halfwords, &ignored, &r2, &palette_count, &source, &dest);
	cycle_1 = palette_cycle_1_ADDRESS;
	dest_1 = PALETTE_RAM + 0x8940u;
	palette_count_1 = 1u;
	source_1 = stage_10_cycle_palettes_1_ADDRESS;
	halfwords = 0x10u;
	palette_cycle_1 = 4u;
	palette_cycle_1_frames = 8u;
	cycle_palette(cycle_1, halfwords, &r1, &r2, &palette_count_1, &source_1, &dest_1);
	*p_r1 = r1;
	*p_r2 = r2;
	*p_r4 = palette_count_1;
	*p_r9 = cycle_1;
	*p_r10 = source_1;
	*p_r11 = dest_1;
}

void animate_palette_stage_0c(uint32_t *p_r1, uint32_t *p_r2, uint32_t *p_palette_count, uint32_t *p_cycle, uint32_t *p_source, uint32_t *p_dest)
{
	uint32_t r2 = *p_r2;
	uint16_t halfwords = 0;
	uint32_t r1 = 0, cycle = 0, dest = 0, palette_count = 0, source = 0;

	dest = PALETTE_RAM + 0x8800u;
	palette_count = 1u;
	source = stage_0c_cycle_palettes_ADDRESS;
	halfwords = 0x10u;
	cycle = palette_cycle_0_ADDRESS;
	palette_cycle_0 = 6u;
	palette_cycle_0_frames = 8u;
	cycle_palette(cycle, halfwords, &r1, &r2, &palette_count, &source, &dest);
	*p_r1 = r1;
	*p_r2 = r2;
	*p_palette_count = palette_count;
	*p_cycle = cycle;
	*p_source = source;
	*p_dest = dest;
}

void animate_palette_stage_0d_0e(uint32_t *p_r1, uint32_t *p_r2, uint32_t *p_palette_count, uint32_t *p_cycle, uint32_t *p_source, uint32_t *p_dest)
{
	uint32_t r2 = *p_r2;
	uint16_t halfwords = 0;
	uint32_t r1 = 0, cycle = 0, dest = 0, palette_count = 0, source = 0;

	dest = PALETTE_RAM + 0x88C0u;
	palette_count = 1u;
	source = stage_0d_0e_cycle_palettes_ADDRESS;
	halfwords = 0x10u;
	cycle = palette_cycle_0_ADDRESS;
	palette_cycle_0 = 6u;
	palette_cycle_0_frames = 8u;
	cycle_palette(cycle, halfwords, &r1, &r2, &palette_count, &source, &dest);
	*p_r1 = r1;
	*p_r2 = r2;
	*p_palette_count = palette_count;
	*p_cycle = cycle;
	*p_source = source;
	*p_dest = dest;
}

void animate_palette_stage_0f(uint32_t *p_r1, uint32_t *p_r2, uint32_t *p_palette_count, uint32_t *p_cycle, uint32_t *p_source, uint32_t *p_dest)
{
	uint32_t r2 = *p_r2;
	uint16_t halfwords = 0;
	uint32_t r1 = 0, cycle = 0, dest = 0, ignored = 0, palette_count = 0, source = 0;

	dest = PALETTE_RAM + 0x8920u;
	palette_count = 3u;
	source = stage_0f_cycle_palettes_0_ADDRESS;
	halfwords = 0x10u;
	cycle = palette_cycle_0_ADDRESS;
	palette_cycle_0 = 0xCu;
	palette_cycle_0_frames = 6u;
	cycle_palette(cycle, halfwords, &ignored, &r2, &palette_count, &source, &dest);
	cycle = palette_cycle_1_ADDRESS;
	dest = PALETTE_RAM + 0x8900u;
	palette_count = 1u;
	source = stage_0f_cycle_palettes_1_ADDRESS;
	halfwords = 0x10u;
	palette_cycle_1 = 6u;
	palette_cycle_1_frames = 6u;
	cycle_palette(cycle, halfwords, &r1, &r2, &palette_count, &source, &dest);
	*p_r1 = r1;
	*p_r2 = r2;
	*p_palette_count = palette_count;
	*p_cycle = cycle;
	*p_source = source;
	*p_dest = dest;
}

void animate_palette_stage_0a(uint32_t *p_r1, uint32_t *p_r2, uint32_t *p_palette_count, uint32_t *p_cycle, uint32_t *p_source, uint32_t *p_dest)
{
	uint32_t r2 = *p_r2;
	uint16_t halfwords = 0;
	uint32_t r1 = 0, cycle = 0, dest = 0, ignored = 0, palette_count = 0, source = 0;

	dest = PALETTE_RAM + 0x8A40u;
	palette_count = 1u;
	source = stage_0a_cycle_palettes_0_ADDRESS;
	halfwords = 0x10u;
	cycle = palette_cycle_0_ADDRESS;
	palette_cycle_0 = 5u;
	palette_cycle_0_frames = 6u;
	cycle_palette(cycle, halfwords, &ignored, &r2, &palette_count, &source, &dest);
	cycle = palette_cycle_1_ADDRESS;
	dest = PALETTE_RAM + 0x8A60u;
	palette_count = 9u;
	source = stage_0a_cycle_palettes_1_ADDRESS;
	halfwords = 0x10u;
	palette_cycle_1 = 5u;
	palette_cycle_1_frames = 0xAu;
	cycle_palette(cycle, halfwords, &r1, &r2, &palette_count, &source, &dest);
	*p_r1 = r1;
	*p_r2 = r2;
	*p_palette_count = palette_count;
	*p_cycle = cycle;
	*p_source = source;
	*p_dest = dest;
}

void animate_palette_stage_09(uint32_t *p_r1, uint32_t *p_r2, uint32_t *p_palette_count, uint32_t *p_cycle, uint32_t *p_source, uint32_t *p_dest)
{
	uint32_t r2 = *p_r2;
	uint16_t halfwords = 0;
	uint32_t r1 = 0, cycle = 0, dest = 0, palette_count = 0, source = 0;

	dest = PALETTE_RAM + 0x88A0u;
	palette_count = 2u;
	source = stage_09_cycle_palettes_ADDRESS;
	halfwords = 0x10u;
	cycle = palette_cycle_0_ADDRESS;
	palette_cycle_0 = 3u;
	palette_cycle_0_frames = 7u;
	cycle_palette(cycle, halfwords, &r1, &r2, &palette_count, &source, &dest);
	*p_r1 = r1;
	*p_r2 = r2;
	*p_palette_count = palette_count;
	*p_cycle = cycle;
	*p_source = source;
	*p_dest = dest;
}

void animate_palette_stage_04(uint32_t *p_r1, uint32_t *p_r2, uint32_t *p_palette_count, uint32_t *p_cycle, uint32_t *p_source, uint32_t *p_dest)
{
	uint32_t r2 = *p_r2;
	uint16_t halfwords = 0;
	uint32_t r1 = 0, cycle = 0, dest = 0, palette_count = 0, source = 0;

	dest = PALETTE_RAM + 0x8500u;
	palette_count = 2u;
	source = stage_04_cycle_palettes_ADDRESS;
	halfwords = 0x10u;
	cycle = palette_cycle_0_ADDRESS;
	palette_cycle_0 = 4u;
	palette_cycle_0_frames = 8u;
	cycle_palette(cycle, halfwords, &r1, &r2, &palette_count, &source, &dest);
	*p_r1 = r1;
	*p_r2 = r2;
	*p_palette_count = palette_count;
	*p_cycle = cycle;
	*p_source = source;
	*p_dest = dest;
}

void cycle_palette(uint32_t cycle, uint16_t halfwords, uint32_t *p_r1, uint32_t *p_r2, uint32_t *p_palette_count, uint32_t *p_source, uint32_t *p_dest)
{
	uint32_t r2 = *p_r2;
	uint32_t palette_count = *p_palette_count;
	uint32_t source = *p_source;
	uint32_t dest = *p_dest;
	uint32_t r1 = 0, r2_2 = 0, frame_offset = 0;

	frame_offset = (uint32_t)mem_read8(cycle + 3u) << 5;
	source += frame_offset;
	if ((int8_t)mem_read8(cycle + 4u) > 0)
	{
		mem_write8(cycle + 4u, mem_read8(cycle + 4u) - 1u);
		*p_r1 = frame_offset;
		*p_source = source;
		return;
	}
	do
	{
		mem_write8(cycle + 4u, mem_read8(cycle));
		queue_vblank_copy(source, dest, halfwords, &r1, &r2_2);
		dest += 0x20u;
		palette_count--;
	} while (palette_count != 0u);
	mem_write8(cycle + 3u, mem_read8(cycle + 3u) + 1u);
	if ((int8_t)mem_read8(cycle + 3u) < (int8_t)mem_read8(cycle + 1u))
	{
		mem_write8(cycle + 4u, mem_read8(cycle + 4u) - 1u);
		*p_r1 = r1;
		*p_r2 = r2_2;
		*p_palette_count = palette_count;
		*p_source = source;
		*p_dest = dest;
		return;
	}
	mem_write8(cycle + 3u, 0u);
	mem_write8(cycle + 4u, mem_read8(cycle + 4u) - 1u);
	*p_r1 = r1;
	*p_r2 = r2_2;
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

uint32_t init_nbg0_picture_screen(void)
{
	uint32_t dest = 0, ignored = 0, palette_list = 0;
	uint8_t io_cnt_bits = 0;

	io_cnt_bits = mem_read8(io_cnt);
	mem_write8(io_cnt, io_cnt_bits & ~2u);
	display_off_frames = 3u;
	mem_write32(video_control, mem_read32(video_control) & ~0x400u);
	mem_write16(video_control, mem_read16(video_control) | 0x400u);
	vblank_copies_pending = 0u;
	vblank_copy_count = 0u;
	palette_list = tile_bank_1_palette_list_ADDRESS;
	load_palette_list(palette_list, &ignored, &dest);
	mem_write16(video_layer_disable, 0x2Eu);
	nbg0_scroll_x_int = 0u;
	nbg0_scroll_y_int = 0u;
	nbg0_pages = 0u;
	return dest;
}

void show_nbg0_picture(uint32_t *p_copy_entry, uint32_t *p_rows, uint32_t *p_source, uint32_t *p_dest)
{
	uint16_t halfwords = 0;
	uint32_t copy_entry = 0, count = 0, dest = 0, ignored = 0, rows = 0, script = 0, source = 0;

	dest = init_nbg0_picture_screen();
	nbg0_pages = 0x1010101u;
	script = sonic_head_pattern_script_ADDRESS;
	run_vram_script(&count, &script, &dest);
	dest = PALETTE_RAM + 0x8BE0u;
	rows = 1u;
	source = nbg0_picture_palette_ADDRESS;
	halfwords = 0x10u;
	do
	{
		queue_vblank_copy(source, dest, halfwords, &ignored, &copy_entry);
		dest += 0x20u;
		rows--;
	} while (rows != 0u);
	*p_copy_entry = copy_entry;
	*p_rows = rows;
	*p_source = source;
	*p_dest = dest;
}

uint32_t set_stage_layer_mixing(void)
{
	uint32_t count = 0, mixer_register = 0, priority_cursor = 0, stage_index = 0;

	priority_cursor = stage_layer_priorities_ADDRESS + ((uint32_t)stage << 2);
	mixer_register = mixer_nbg0;
	count = 4u;
	do
	{
		mem_write16(mixer_register, ROM8(priority_cursor) + 0x200u);
		count--;
		priority_cursor++;
		mixer_register += 2u;
	} while (count != 0u);
	stage_index = stage;
	mem_write8(video_layer_disable, mem_read8(video_layer_disable) & ~0xFu);
	mem_write8(video_layer_disable, mem_read8(video_layer_disable) | stage_layer_disables[stage_index]);
	mem_write16(video_rowscroll_control, stage_rowscroll_settings[stage_index]);
	return count;
}

uint32_t init_nbg1_layer(uint32_t dest)
{
	uint32_t count = 0, script = 0;

	nbg1_pages = 0x1211100Fu;
	script = clear_tile_page_0f_script_ADDRESS;
	run_vram_script(&count, &script, &dest);
	script = clear_tile_page_10_script_ADDRESS;
	run_vram_script(&count, &script, &dest);
	script = clear_tile_page_11_script_ADDRESS;
	run_vram_script(&count, &script, &dest);
	script = clear_tile_page_12_script_ADDRESS;
	run_vram_script(&count, &script, &dest);
	switch (stage)
	{
	case 0:
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
		mixer_nbg1_blend &= 0xFF7Fu;
		return dest;
	case 9:
	case 19:
		mixer_nbg0_blend &= ~0x780u;
		mixer_nbg1_blend &= ~0x780u;
		mixer_nbg2_blend &= ~0x780u;
		mixer_nbg3_blend &= ~0x780u;
		mixer_nbg0_blend |= 0x100u;
		return dest;
	case 16:
		mixer_nbg0_blend |= 0x100u;
		mixer_blend |= 0xB00u;
		mixer_nbg1_blend &= ~0xF00u;
		mixer_nbg1_blend &= 0xFF7Fu;
		return dest;
	default:
		unknown_code(0x1B30Bu + stage * 3u);
	}
}

void update_nbg1_scroll(void)
{
	switch (stage)
	{
	case 0:
		skip_nbg1_scroll();
		break;
	case 1:
		skip_nbg1_scroll();
		break;
	case 2:
		skip_nbg1_scroll();
		break;
	case 3:
		set_nbg1_scroll_to_camera();
		break;
	case 4:
		skip_nbg1_scroll();
		break;
	case 5:
		skip_nbg1_scroll();
		break;
	case 6:
		skip_nbg1_scroll();
		break;
	case 7:
		set_nbg1_scroll_to_camera();
		break;
	case 8:
		set_nbg1_scroll_to_camera();
		break;
	case 9:
		set_nbg1_scroll_to_camera();
		break;
	case 10:
		set_nbg1_scroll_to_camera();
		break;
	case 11:
		set_nbg1_scroll_to_camera();
		break;
	case 12:
		skip_nbg1_scroll();
		break;
	case 13:
		skip_nbg1_scroll();
		break;
	case 14:
		skip_nbg1_scroll();
		break;
	case 15:
		skip_nbg1_scroll();
		break;
	case 16:
		set_nbg1_scroll_to_camera();
		break;
	case 17:
		skip_nbg1_scroll();
		break;
	case 18:
		skip_nbg1_scroll();
		break;
	case 19:
		skip_nbg1_scroll();
		break;
	default:
		unknown_code(0x1B398u + stage * 3u);
	}
	skip_nbg1_scroll();
}

void skip_nbg1_scroll(void)
{
}

void set_nbg1_scroll_to_camera(void)
{
	uint16_t layer_scroll_y = 0;

	layer_scroll_y = camera_y_pixels;
	nbg1_scroll_x_int = camera_x_pixels;
	nbg1_scroll_y_int = layer_scroll_y;
}

uint32_t init_nbg2_layer(uint32_t dest)
{
	uint32_t count = 0, entries_left = 0, script = 0, table_entry = 0, tables_left = 0, wave_index = 0;

	nbg2_pages = 0xD0C0B0Au;
	script = clear_tile_page_0a_script_ADDRESS;
	run_vram_script(&count, &script, &dest);
	script = clear_tile_page_0b_script_ADDRESS;
	run_vram_script(&count, &script, &dest);
	script = clear_tile_page_0c_script_ADDRESS;
	run_vram_script(&count, &script, &dest);
	script = clear_tile_page_0d_script_ADDRESS;
	run_vram_script(&count, &script, &dest);
	switch (stage)
	{
	case 0:
	case 17:
	case 18:
		mixer_nbg2_blend &= 0xFF7Fu;
		return dest;
	case 1:
	case 2:
		break;
	case 3:
	case 5:
	case 6:
	case 7:
	case 8:
	case 9:
	case 10:
	case 11:
	case 12:
	case 13:
	case 14:
	case 15:
		mixer_nbg2_blend &= 0xFF7Fu;
		return dest;
	case 4:
		mixer_nbg2_blend |= 0x80u;
		mixer_blend &= 0xFFu;
		mixer_blend |= 0xB00u;
		return dest;
	case 16:
	case 19:
		mixer_nbg2_blend &= ~0xF00u;
		return dest;
	default:
		unknown_code(0x1B44Fu + stage * 3u);
	}
	row_scroll_wave_step = 0u;
	entries_left = 0x100u;
	table_entry = VIDEO_RAM + 0xA800u;
	tables_left = 2u;
	for (;;)
	{
		row_scroll_wave_step &= 0x7Fu;
		wave_index = row_scroll_wave_step;
		mem_write16(table_entry, (int32_t)(((uint32_t)(int16_t)ROM16(sine_table_ADDRESS + (uint16_t)(wave_index << 4)) << 3) & ~0xFFFFu) >> 16);
		row_scroll_wave_step++;
		table_entry += 2u;
		entries_left--;
		if (entries_left != 0u)
			continue;
		entries_left = 0x100u;
		table_entry = VIDEO_RAM + 0xA000u;
		tables_left--;
		if (tables_left != 0u)
			continue;
		break;
	}
	mixer_nbg2_blend |= 0x80u;
	mixer_blend &= 0xFFu;
	mixer_blend |= 0xB00u;
	return dest;
}

uint32_t update_nbg2_scroll(uint32_t position)
{
	switch (stage)
	{
	case 0:
		skip_nbg2_scroll();
		break;
	case 1:
		set_nbg2_scroll_to_camera_2();
		break;
	case 2:
		set_nbg2_scroll_to_camera_2();
		break;
	case 3:
		position = set_nbg2_scroll_stages_03_07();
		break;
	case 4:
		set_nbg2_scroll_stage_04();
		break;
	case 5:
		skip_nbg2_scroll();
		break;
	case 6:
		skip_nbg2_scroll();
		break;
	case 7:
		position = set_nbg2_scroll_stages_03_07();
		break;
	case 8:
		skip_nbg2_scroll();
		break;
	case 9:
		set_nbg2_scroll_to_camera();
		break;
	case 10:
		set_nbg2_scroll_to_camera();
		break;
	case 11:
		skip_nbg2_scroll();
		break;
	case 12:
		skip_nbg2_scroll();
		break;
	case 13:
		skip_nbg2_scroll();
		break;
	case 14:
		skip_nbg2_scroll();
		break;
	case 15:
		skip_nbg2_scroll();
		break;
	case 16:
		set_nbg2_scroll_to_camera();
		break;
	case 17:
		skip_nbg2_scroll();
		break;
	case 18:
		skip_nbg2_scroll();
		break;
	case 19:
		set_nbg2_scroll_to_camera();
		break;
	default:
		unknown_code(0x1B525u + stage * 3u);
	}
	skip_nbg2_scroll();
	return position;
}

void set_nbg2_scroll_to_camera(void)
{
	uint16_t layer_scroll_y = 0;

	layer_scroll_y = camera_y_pixels;
	nbg2_scroll_x = camera_x_pixels;
	nbg2_scroll_y = layer_scroll_y;
}

void skip_nbg2_scroll(void)
{
}

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

void update_nbg3_scroll(uint32_t *p_scroll_y, uint32_t *p_quadrants, uint32_t *p_r4, uint32_t *p_r5, uint32_t *p_quadrant_cursor)
{
	uint32_t scroll_y = *p_scroll_y;
	uint32_t quadrants = *p_quadrants;
	uint32_t r4 = *p_r4;
	uint32_t r5 = *p_r5;
	uint32_t quadrant_cursor = *p_quadrant_cursor;
	uint16_t half_view_y = 0, layer_scroll_y = 0;
	uint32_t r4_2 = 0, r5_2 = 0, block_pages = 0;

	switch (stage)
	{
	case 0:
		if ((uint8_t)scroll_ended == 0u)
		{
			half_view_y = (uint16_t)((int16_t)view_y >> 1);
			nbg3_scroll_x = (int16_t)view_x >> 1;
			nbg3_scroll_y = half_view_y;
			nbg3_scroll_y += 0x80u;
			select_nbg3_pages(&block_pages, &quadrants, &r4_2, &r5_2, &quadrant_cursor);
			*p_scroll_y = block_pages;
			*p_quadrants = quadrants;
			*p_r4 = r4_2;
			*p_r5 = r5_2;
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
		select_nbg3_pages(&block_pages, &quadrants, &r4_2, &r5_2, &quadrant_cursor);
		*p_scroll_y = block_pages;
		*p_quadrants = quadrants;
		*p_r4 = r4_2;
		*p_r5 = r5_2;
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

void select_nbg3_pages(uint32_t *p_block_pages, uint32_t *p_quadrants, uint32_t *p_r4, uint32_t *p_r5, uint32_t *p_quadrant_cursor)
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
	*p_r4 = byte_mask;
	*p_r5 = shift;
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

void build_ending_scene(uint32_t *p_r2, uint32_t *p_tile_bank_bits, uint32_t *p_r4, uint32_t *p_r8, uint32_t *p_blocks_per_row, uint32_t *p_scene_actor)
{
	uint32_t r2 = *p_r2;
	uint32_t tile_bank_bits = *p_tile_bank_bits;
	uint32_t r4 = *p_r4;
	uint32_t r8 = *p_r8;
	uint32_t blocks_per_row = *p_blocks_per_row;
	uint32_t scene_actor = *p_scene_actor;
	uint8_t is_8bpp = 0, sound_id = 0;
	uint32_t r1 = 0, r2_2 = 0, r4_2 = 0, r5 = 0, r10 = 0, r11 = 0, ignored = 0, script = 0;
	uint8_t io_cnt_bits = 0;

	is_8bpp = (uint8_t)(ending_scene_step * 3u);
	switch (ending_scene_step)
	{
	case 0:
		io_cnt_bits = mem_read8(io_cnt);
		mem_write8(io_cnt, io_cnt_bits & ~2u);
		display_off_frames = 0x10u;
		r4_2 = set_stage_layer_mixing();
		clear_sprite_buckets();
		clear_bitmap_layer(is_8bpp, &ignored, &r4_2, &r5, &r8, &blocks_per_row, &r10);
		init_video_registers();
		load_stage_palette_set(&tile_bank_bits, &r11);
		sound_id = 0x94u;
		queue_sound(sound_id, &r1, &r2_2);
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
		*p_r2 = r2_2;
		*p_tile_bank_bits = tile_bank_bits;
		*p_r4 = r4_2;
		*p_r8 = r8;
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
