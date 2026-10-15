/**
 *    Name:        objects/geysers.c
 *    Description: stage 10: the geyser front and its geysers, the waterfall and the river water, the stone blocks, the geyser lines
 *    Version:     0.1.0
 *    Created:     12-10-2026
 *    Modified:    15-10-2026
 *    Author:      Sebastian Toyos
 *
 *    SegaSonic The Hedgehog (arcade, Sega System 32): the game's V60 program decompiled into C by
 *    tools/decompile.py, with the names of symbols.txt.
 */
#include "game.h"

void geyser_front_init(void)
{
	uint32_t block_offset = 0, free_count = 0, hitbox_object = 0, ignored = 0, path = 0, y = 0;

	path = geyser_fronts[OBJECT(current_object)->sprite_frame] + geyser_fronts_ADDRESS;
	OBJECT(current_object)->z_int = ROM16(path);
	OBJECT(current_object)->z_fraction = 0u;
	path += 2u;
	OBJECT(current_object)->geyser_front.top_height = ROM16(path);
	path += 2u;
	OBJECT(current_object)->geyser_front.next_listed_geyser = ROM16(path) + geyser_lists_ADDRESS;
	OBJECT(current_object)->geyser_front.path = path + 2u;
	find_floor(&free_count, &y, &block_offset, &ignored, &hitbox_object);
	OBJECT(current_object)->previous_floor_z_int = OBJECT(current_object)->z_int;
	OBJECT(current_object)->previous_floor_z_int += 0x30u;
	project_object(&ignored, &ignored);
	OBJECT(current_object)->geyser_front.projected_x_whole = OBJECT(current_object)->projected_x;
	AS16(OBJECT(current_object)->geyser_front.fixed_projected_x) = 0u;
	OBJECT(current_object)->geyser_front.row_pattern = geyser_row_patterns[OBJECT(current_object)->sprite_frame] + geyser_row_patterns_ADDRESS;
	OBJECT(current_object)->geyser_front.holding_back = 0u;
	OBJECT(current_object)->geyser_front.wait_timer = 0;
	geyser_front_start = 0u;
	OBJECT(current_object)->geyser_front.sound_timer = 0;
	take_free_object(&free_count, &hitbox_object);
	if (free_count != 0u)
	{
		mem_write16(hitbox_object + 0x66u, 0x40u);
		mem_write16(hitbox_object + 0x10u, 0x30u);
		mem_write16(hitbox_object + 0x12u, 0xC0u);
		mem_write32(hitbox_object + 0x30u, 2u);
		mem_write16(hitbox_object + 0x7Cu, 0u);
		mem_write16(hitbox_object + 0x24u, 0x300u);
		mem_write8(hitbox_object + 0x47u, 0x29u);
		OBJECT(current_object)->child = hitbox_object;
		OBJECT(current_object)->handler = geyser_front_wait_ADDRESS;
	}
}

void geyser_front_wait(void)
{
	uint16_t start_position = 0;

	spawn_listed_geyser(0u);
	if (geyser_front_start != 0u)
		OBJECT(current_object)->handler = geyser_front_rise_ADDRESS;
	else
	{
		start_position = 0x6500u;
		if (OBJECT(current_object)->sprite_frame != 0u)
			start_position = 0x8400u;
		if (start_position <= scroll_position)
		{
			OBJECT(current_object)->handler = geyser_front_rise_ADDRESS;
			return;
		}
	}
}

void geyser_front_rise(void)
{
	uint8_t sound_id = 0;
	uint16_t wait = 0;
	uint32_t r1 = 0, r11 = 0, attribute_record = 0, cursor = 0, ignored = 0, point_x = 0, point_y = 0, row_delays = 0, row_pattern = 0, slope_step = 0, write_index_address = 0, x = 0, y = 0;
	uint8_t hold_timer = 0;
	uint16_t wait_timer = 0, geysers_remaining = 0, sound_timer = 0;
	uint32_t flags = 0;

	flags = OBJECT(current_object)->flags;
	OBJECT(current_object)->flags = flags | 0x200000u;
	if ((flags & 0x200000u) == 0u)
	{
	geyser_front_read_script:
		if ((uint16_t)OBJECT(current_object)->geyser_front.wait_timer == 0u)
		{
			cursor = OBJECT(current_object)->geyser_front.path;
			if (mem_read16(cursor) <= scroll_position || OBJECT(current_object)->z_int <= (int16_t)mem_read16(cursor + 2u))
			{
				cursor += 4u;
				if ((int16_t)mem_read16(cursor + 2u) < 0)
				{
					current_object = LINKED(current_object, OBJECT(current_object)->child);
					push_free_object();
					delete_current_object();
					return;
				}
				OBJECT(current_object)->geyser_front.screen_threshold = mem_read16(cursor);
				cursor += 2u;
				OBJECT(current_object)->geyser_front.catch_up_factor = mem_read16(cursor);
				cursor += 2u;
				OBJECT(current_object)->geyser_front.base_speed = mem_read16(cursor);
				cursor += 2u;
				OBJECT(current_object)->geyser_front.wait_timer = mem_read16(cursor);
				OBJECT(current_object)->geyser_front.path = cursor + 2u;
			}
		}
	}
	else if (OBJECT(current_object)->geyser_front.holding_back == 0u)
	{
		if (no_player_in_play == 0u && player_mishap == 0u && player_dropping_in == 0u)
			goto geyser_front_read_script;
		OBJECT(current_object)->geyser_front.holding_back = 0xFFu;
		OBJECT(current_object)->geyser_front.saved_base_speed = OBJECT(current_object)->geyser_front.base_speed;
		OBJECT(current_object)->geyser_front.base_speed = 0;
		OBJECT(current_object)->geyser_front.hold_start_position = scroll_position;
		OBJECT(current_object)->geyser_front.hold_timer = 0xFFu;
		OBJECT(current_object)->geyser_front.saved_threshold = OBJECT(current_object)->geyser_front.screen_threshold;
		OBJECT(current_object)->geyser_front.screen_threshold = 0x130;
	}
	if (OBJECT(current_object)->geyser_front.holding_back != 0u)
	{
		if ((int16_t)(scroll_position - OBJECT(current_object)->geyser_front.hold_start_position) >= 0x300)
			goto geyser_front_stop_holding_back;
		if (no_player_in_play != 0u)
			OBJECT(current_object)->geyser_front.hold_timer = 0xFFu;
		hold_timer = OBJECT(current_object)->geyser_front.hold_timer;
		OBJECT(current_object)->geyser_front.hold_timer = hold_timer - 1u;
		if (hold_timer == 0u)
			goto geyser_front_stop_holding_back;
	}
	update_screen_position(&slope_step, &r1);
	if (OBJECT(current_object)->screen_y <= OBJECT(current_object)->geyser_front.screen_threshold)
	{
	geyser_front_use_base_speed:
		r1 = (uint16_t)OBJECT(current_object)->geyser_front.base_speed;
	}
	else
	{
		OBJECT(current_object)->geyser_front.wait_timer = 0;
		if (OBJECT(current_object)->geyser_front.holding_back != 0u)
		{
		geyser_front_stop_holding_back:
			OBJECT(current_object)->geyser_front.holding_back = 0u;
			OBJECT(current_object)->geyser_front.screen_threshold = OBJECT(current_object)->geyser_front.saved_threshold;
			OBJECT(current_object)->geyser_front.base_speed = OBJECT(current_object)->geyser_front.saved_base_speed;
		}
		compute_camera_ground_speed(0u);
		r1 = (uint16_t)((uint32_t)camera_ground_speed_int * OBJECT(current_object)->geyser_front.catch_up_factor) >> 8;
		if ((int16_t)r1 <= OBJECT(current_object)->geyser_front.base_speed)
			goto geyser_front_use_base_speed;
	}
	chaser_speed = r1;
	wait_timer = OBJECT(current_object)->geyser_front.wait_timer;
	OBJECT(current_object)->geyser_front.wait_timer = wait_timer - 1u;
	if ((int16_t)wait_timer <= 1)
	{
		OBJECT(current_object)->geyser_front.wait_timer = 0;
		OBJECT(current_object)->z -= (uint32_t)chaser_speed << 12;
		attribute_record = OBJECT(current_object)->floor_attribute + floor_types_ADDRESS;
		slope_step = ((uint32_t)(int16_t)ROM16(attribute_record + 4u) * chaser_speed) >> 4;
		r1 = slope_step;
		if (ROM8(attribute_record) != 0u)
			r1 = 0u - r1;
		OBJECT(current_object)->geyser_front.fixed_projected_x += r1;
		OBJECT(current_object)->projected_x = OBJECT(current_object)->geyser_front.projected_x_whole;
		OBJECT(current_object)->projected_y -= slope_step >> 1;
		point_x = OBJECT(current_object)->geyser_front.fixed_projected_x;
		point_y = OBJECT(current_object)->projected_y * 2u;
		unproject_point(&point_x, &point_y);
		OBJECT(current_object)->x = point_x;
		OBJECT(current_object)->y = point_y;
	}
	current_object = LINKED(current_object, OBJECT(current_object)->child);
	OBJECT(current_object)->x = OBJECT(object_cursor)->x;
	OBJECT(current_object)->x_int -= 0x10u;
	OBJECT(current_object)->y = OBJECT(object_cursor)->y;
	OBJECT(current_object)->z = OBJECT(object_cursor)->z;
	OBJECT(current_object)->z_int += OBJECT(current_object)->hitbox_height;
	OBJECT(current_object)->z_int += 0x60u;
	add_to_touchable_list(slope_step);
	current_object = object_cursor;
	r11 = spawn_listed_geyser(0u);
	OBJECT(current_object)->flags &= ~1u;
	if (OBJECT(current_object)->z_int <= OBJECT(current_object)->geyser_front.top_height)
	{
		OBJECT(current_object)->geyser_front.top_height = 0;
		spawn_geyser_line(r11);
		goto geyser_front_spawn_geyser_row;
	}
	if ((int16_t)(OBJECT(current_object)->previous_floor_z_int - OBJECT(current_object)->z_int) >= 0x30)
	{
	geyser_front_spawn_geyser_row:
		OBJECT(current_object)->flags |= 1u;
		OBJECT(current_object)->previous_floor_z_int = OBJECT(current_object)->z_int;
		geysers_left = 5;
		if (free_object_stack < geysers_left)
			return;
		geyser_y = OBJECT(current_object)->y;
		geyser_y_int += 0x10u;
		row_pattern = OBJECT(current_object)->geyser_front.row_pattern;
		row_delays = mem_read8(row_pattern);
		OBJECT(current_object)->geyser_front.row_pattern = row_pattern + 1u;
		row_delays = row_delays * 0xAu + geyser_row_delays_ADDRESS;
		geyser_delay_cursor = row_delays;
		do
		{
			x = OBJECT(current_object)->x;
			y = geyser_y;
			geyser_y_int += 0x2Eu;
			r14 = OBJECT(current_object)->z;
			wait = mem_read16(geyser_delay_cursor);
			geyser_delay_cursor += 2u;
			spawn_geyser(x, y, wait, &ignored, &row_delays);
			geysers_remaining = geysers_left;
			geysers_left = geysers_remaining - 1u;
		} while ((int16_t)geysers_remaining > 1);
	}
	if (OBJECT(current_object)->screen_y <= 0x130)
	{
		sound_timer = OBJECT(current_object)->geyser_front.sound_timer;
		OBJECT(current_object)->geyser_front.sound_timer = sound_timer - 1u;
		if ((int16_t)sound_timer <= 1)
		{
			OBJECT(current_object)->geyser_front.sound_timer = 0x32;
			sound_id = 0x12u;
			play_sound(sound_id, &r1, &write_index_address);
		}
	}
}

uint32_t spawn_listed_geyser(uint32_t geyser)
{
	uint16_t wait = 0;
	uint32_t record = 0, spawned = 0, x = 0, y = 0;

	if (mem_read16(OBJECT(current_object)->spawn_listed_geyser.next_listed_geyser) <= scroll_position)
	{
		record = OBJECT(current_object)->spawn_listed_geyser.next_listed_geyser + 2u;
		x = (uint32_t)mem_read16(record) << 16;
		record += 2u;
		y = (uint32_t)mem_read16(record) << 16;
		record += 2u;
		r14 = (uint32_t)mem_read16(record) << 16;
		record += 2u;
		wait = mem_read16(record);
		OBJECT(current_object)->spawn_listed_geyser.next_listed_geyser = record + 2u;
		spawn_geyser(x, y, wait, &spawned, &geyser);
		if (spawned != 0u)
		{
			OBJECT(geyser)->flags |= 0x80000000u;
			OBJECT(geyser)->animation_speed = 0x20u;
		}
	}
	return geyser;
}

void spawn_geyser(uint32_t x, uint32_t y, uint16_t wait, uint32_t *p_spawned, uint32_t *p_geyser)
{
	uint32_t geyser = *p_geyser;
	uint32_t free_count = 0, spawned = 0;

	allocate_object(&free_count, &geyser);
	if (free_count == 0u)
	{
		*p_spawned = free_count;
		*p_geyser = geyser;
		return;
	}
	OBJECT(geyser)->x = x;
	OBJECT(geyser)->y = y;
	OBJECT(geyser)->z = r14;
	OBJECT(geyser)->geyser_wait.erupt_delay = wait;
	OBJECT(geyser)->sprite_flags = 0x1000u;
	OBJECT(geyser)->sprite_frame = 0x5DA0u;
	OBJECT(geyser)->sprite_palette = 0x8280u;
	OBJECT(geyser)->hitbox_size_x = 0x20u;
	OBJECT(geyser)->hitbox_size_y = 0x30u;
	OBJECT(geyser)->hitbox_height = 0x20u;
	OBJECT(geyser)->kind = 0x27u;
	OBJECT(geyser)->geyser_wait.draw_depth = 0xEFu;
	OBJECT(geyser)->handler = geyser_wait_ADDRESS;
	OBJECT(geyser)->direction = 0x300u;
	OBJECT(geyser)->sprite_offset_x = 0;
	OBJECT(geyser)->sprite_offset_y = -0x10;
	OBJECT(geyser)->flags = 2u;
	OBJECT(geyser)->hitbox_offset_x = -5;
	OBJECT(geyser)->hitbox_offset_y = -0x18;
	OBJECT(geyser)->parent = current_object;
	OBJECT(geyser)->animation_speed = 8u;
	OBJECT(geyser)->animation_timer = 0;
	OBJECT(geyser)->animation_frame = 0xFFu;
	OBJECT(geyser)->animation = geyser_animation_ADDRESS;
	spawned = 0xFFFFFFFFu;
	*p_spawned = spawned;
	*p_geyser = geyser;
}

void geyser_wait(void)
{
	uint32_t depth = 0, ignored = 0;
	uint16_t erupt_delay = 0;

	if ((OBJECT(current_object)->flags & 0x80000000u) != 0u)
	{
		project_object(&ignored, &depth);
		OBJECT(current_object)->anchor_x = OBJECT(current_object)->projected_x;
		OBJECT(current_object)->anchor_y = OBJECT(current_object)->projected_y_int;
		if (OBJECT(LINKED(current_object, OBJECT(current_object)->parent))->var_8E == 0u && OBJECT(current_object)->geyser_wait.erupt_delay <= scroll_position)
			OBJECT(current_object)->handler = geyser_erupt_ADDRESS;
		depth = queue_sprite(depth);
		return;
	}
	project_object(&ignored, &ignored);
	OBJECT(current_object)->anchor_x = OBJECT(current_object)->projected_x;
	OBJECT(current_object)->anchor_y = OBJECT(current_object)->projected_y_int;
	if ((OBJECT(LINKED(current_object, OBJECT(current_object)->parent))->flags & 1u) != 0u)
		OBJECT(current_object)->geyser_wait.draw_depth--;
	if (OBJECT(LINKED(current_object, OBJECT(current_object)->parent))->var_8E == 0u)
	{
		erupt_delay = OBJECT(current_object)->geyser_wait.erupt_delay;
		OBJECT(current_object)->geyser_wait.erupt_delay = erupt_delay - 1u;
		if ((int16_t)erupt_delay <= 1)
			OBJECT(current_object)->handler = geyser_erupt_ADDRESS;
	}
}

void geyser_erupt(void)
{
	uint8_t sound_id = 0;
	uint32_t r1 = 0, at_last_step = 0, depth = 0, ignored = 0, write_index_address = 0;

	if ((int8_t)OBJECT(current_object)->animation_frame >= 3)
		OBJECT(current_object)->animation_speed = 2u;
	at_last_step = animate_around_anchor();
	if (at_last_step != 0u)
		OBJECT(current_object)->animation = geyser_jet_animation_ADDRESS;
	if (OBJECT(current_object)->animation_frame == 3u && (uint8_t)OBJECT(current_object)->animation_timer == OBJECT(current_object)->animation_speed)
	{
		sound_id = 0xAu;
		play_sound(sound_id, &r1, &write_index_address);
	}
	update_screen_position(&ignored, &r1);
	if (OBJECT(current_object)->screen_y > 0x180)
	{
		delete_current_object();
		return;
	}
	if (OBJECT(current_object)->animation == geyser_jet_animation_ADDRESS || (int8_t)OBJECT(current_object)->animation_frame > 3)
		add_to_touchable_list(ignored);
	if ((OBJECT(LINKED(current_object, OBJECT(current_object)->parent))->flags & 1u) != 0u)
		OBJECT(current_object)->geyser_erupt.draw_depth--;
	if ((OBJECT(current_object)->flags & 0x80000000u) != 0u || (OBJECT(current_object)->animation != geyser_jet_animation_ADDRESS && (int8_t)OBJECT(current_object)->animation_frame < 4))
	{
		r1 = queue_sprite(r1);
		return;
	}
	depth = OBJECT(current_object)->geyser_erupt.draw_depth;
	queue_sprite_at_depth(&ignored, &depth);
}

void spawn_waterfall(uint32_t record, uint32_t *p_spawned, uint32_t *p_free_stack, uint32_t *p_block)
{
	uint32_t free_stack = *p_free_stack;
	uint32_t block = *p_block;
	uint32_t free_count = 0, ignored = 0, object = 0, spawned = 0;

	allocate_object_outside_loop(&free_count, &block);
	if (free_count == 0u || (uint16_t)free_object_stack_2[0] == 0u)
	{
		*p_spawned = free_count;
		*p_block = block;
		return;
	}
	group_leaders[mem_read8(record) & 3u] = block;
	record++;
	OBJECT(block)->x_int = mem_read16(record);
	OBJECT(block)->x_fraction = 0u;
	record += 2u;
	OBJECT(block)->y_int = mem_read16(record);
	OBJECT(block)->y_fraction = 0u;
	record += 2u;
	OBJECT(block)->z_int = mem_read16(record);
	OBJECT(block)->z_fraction = 0u;
	OBJECT(block)->floor_z = OBJECT(block)->z;
	record += 2u;
	OBJECT(block)->direction = mem_read16(record);
	OBJECT(block)->waterfall.path = mem_read16(record + 2u) + group_leader_data_ADDRESS;
	OBJECT(block)->handler = waterfall_move_ADDRESS;
	OBJECT(block)->sprite_flags = 0xD00Au;
	OBJECT(block)->flags = 0u;
	OBJECT(block)->sprite_offset_x = 0;
	OBJECT(block)->sprite_offset_y = 0;
	OBJECT(block)->waterfall.holding_back = 0u;
	OBJECT(block)->waterfall.sound_timer = 0u;
	OBJECT(block)->waterfall.wait_timer = 1;
	OBJECT(block)->animation = waterfall_animation_ADDRESS;
	OBJECT(block)->animation_frame = 0u;
	OBJECT(block)->animation_speed = 3u;
	OBJECT(block)->animation_timer = 0;
	object = block;
	allocate_object_2(&ignored, &free_stack, &block);
	move_halfwords_backward(block, waterfall_parts_ADDRESS, 0xDu);
	OBJECT(object)->part_list = block;
	spawned = 0xFFFFFFFFu;
	*p_spawned = spawned;
	*p_free_stack = free_stack;
	*p_block = block;
}

void waterfall_move(void)
{
	uint8_t sound_id = 0;
	uint16_t part_list = 0;
	uint32_t r2 = 0, r3 = 0, r8 = 0, r11 = 0, cursor = 0, depth = 0, ignored = 0, speed = 0, x = 0;
	uint8_t hold_timer = 0;
	uint16_t wait_timer = 0, trigger_scroll = 0, sound_timer = 0;
	uint32_t flags = 0;

	flags = OBJECT(current_object)->flags;
	OBJECT(current_object)->flags = flags | 0x80000000u;
	if ((flags & 0x80000000u) == 0u)
	{
	waterfall_read_script:
		wait_timer = OBJECT(current_object)->waterfall.wait_timer;
		OBJECT(current_object)->waterfall.wait_timer = wait_timer - 1u;
		if ((int16_t)wait_timer <= 1)
		{
			OBJECT(current_object)->waterfall.wait_timer = 0;
			cursor = OBJECT(current_object)->waterfall.path;
			trigger_scroll = mem_read16(cursor);
			cursor += 2u;
			if (trigger_scroll <= scroll_position)
			{
				if ((int16_t)mem_read16(cursor + 2u) < 0)
				{
					part_list = (uint16_t)OBJECT(current_object)->part_list;
					free_object_2(part_list);
					delete_current_object();
					return;
				}
				OBJECT(current_object)->waterfall.screen_threshold = mem_read16(cursor);
				cursor += 2u;
				OBJECT(current_object)->waterfall.catch_up_factor = mem_read16(cursor);
				cursor += 2u;
				OBJECT(current_object)->waterfall.base_speed = mem_read16(cursor);
				cursor += 2u;
				OBJECT(current_object)->waterfall.wait_timer = mem_read16(cursor);
				OBJECT(current_object)->waterfall.path = cursor + 2u;
			}
		}
	}
	else if (OBJECT(current_object)->waterfall.holding_back == 0u)
	{
		if (no_player_in_play == 0u && player_mishap == 0u && player_dropping_in == 0u)
			goto waterfall_read_script;
		OBJECT(current_object)->waterfall.holding_back = 0xFFu;
		OBJECT(current_object)->waterfall.saved_base_speed = OBJECT(current_object)->waterfall.base_speed;
		OBJECT(current_object)->waterfall.base_speed = 0;
		OBJECT(current_object)->waterfall.hold_start_position = scroll_position;
		OBJECT(current_object)->waterfall.hold_timer = 0xFFu;
		OBJECT(current_object)->waterfall.saved_threshold = OBJECT(current_object)->waterfall.screen_threshold;
		OBJECT(current_object)->waterfall.screen_threshold = 0x160;
	}
	find_floor(&x, &speed, &r2, &r3, &r11);
	project_object(&ignored, &ignored);
	if (OBJECT(current_object)->waterfall.holding_back != 0u)
	{
		if ((int16_t)(scroll_position - OBJECT(current_object)->waterfall.hold_start_position) >= 0x300)
			goto waterfall_stop_holding_back;
		if (no_player_in_play != 0u)
			OBJECT(current_object)->waterfall.hold_timer = 0xFFu;
		hold_timer = OBJECT(current_object)->waterfall.hold_timer;
		OBJECT(current_object)->waterfall.hold_timer = hold_timer - 1u;
		if (hold_timer == 0u)
			goto waterfall_stop_holding_back;
	}
	if (OBJECT(current_object)->screen_x <= OBJECT(current_object)->waterfall.screen_threshold && OBJECT(current_object)->screen_y <= 0xE0)
	{
		speed = (uint16_t)OBJECT(current_object)->waterfall.base_speed;
		chaser_speed = speed;
		if (OBJECT(current_object)->waterfall.wait_timer <= 0)
			OBJECT(current_object)->y -= (uint32_t)chaser_speed << 12;
		if (OBJECT(current_object)->y_int <= 0x1480)
			geyser_front_start = 0xFFu;
		if (map_screen_active == 0u)
		{
			sound_timer = OBJECT(current_object)->waterfall.sound_timer;
			OBJECT(current_object)->waterfall.sound_timer = sound_timer - 1u;
			if ((int16_t)sound_timer <= 1)
			{
				OBJECT(current_object)->waterfall.sound_timer = 0x3Cu;
				sound_id = 0x10u;
				play_sound(sound_id, &speed, &r2);
			}
		}
		animate_all_parts(&r8, &r11);
		depth = (uint16_t)(OBJECT(current_object)->screen_y - 0x60u);
		queue_sprite_at_depth(&ignored, &depth);
		return;
	}
	OBJECT(current_object)->waterfall.wait_timer = 0;
	if (OBJECT(current_object)->waterfall.holding_back != 0u)
	{
	waterfall_stop_holding_back:
		OBJECT(current_object)->waterfall.holding_back = 0u;
		OBJECT(current_object)->waterfall.screen_threshold = OBJECT(current_object)->waterfall.saved_threshold;
		OBJECT(current_object)->waterfall.base_speed = OBJECT(current_object)->waterfall.saved_base_speed;
	}
	compute_camera_ground_speed(r3);
	speed = (uint16_t)((uint32_t)camera_ground_speed_int * OBJECT(current_object)->waterfall.catch_up_factor) >> 8;
	if ((int16_t)speed <= OBJECT(current_object)->waterfall.base_speed)
		speed = (uint16_t)OBJECT(current_object)->waterfall.base_speed;
	chaser_speed = speed;
	if (OBJECT(current_object)->waterfall.wait_timer <= 0)
		OBJECT(current_object)->y -= (uint32_t)chaser_speed << 12;
	if (OBJECT(current_object)->y_int <= 0x1480)
		geyser_front_start = 0xFFu;
	if (map_screen_active == 0u)
	{
		sound_timer = OBJECT(current_object)->waterfall.sound_timer;
		OBJECT(current_object)->waterfall.sound_timer = sound_timer - 1u;
		if ((int16_t)sound_timer <= 1)
		{
			OBJECT(current_object)->waterfall.sound_timer = 0x3Cu;
			sound_id = 0x10u;
			play_sound(sound_id, &speed, &r2);
		}
	}
	animate_all_parts(&r8, &r11);
	depth = (uint16_t)(OBJECT(current_object)->screen_y - 0x60u);
	queue_sprite_at_depth(&ignored, &depth);
}

void river_water_update(void)
{
	uint16_t child = 0;
	uint32_t r1 = 0, clip_x = 0, clips_left = 0, depth = 0, floor_z = 0, ignored = 0, object = 0, object_2 = 0, object_3 = 0, point_y = 0;

	switch (OBJECT(current_object)->river_water.state)
	{
	case 0:
	case 4:
		project_object(&ignored, &r1);
		OBJECT(current_object)->river_water.state++;
		current_player = current_object;
		current_player = LINKED(current_player, OBJECT(current_object)->parent);
		if (OBJECT(current_player)->y_int <= OBJECT(current_object)->y_int)
			OBJECT(current_object)->river_water.state++;
		if (OBJECT(current_object)->screen_y > 0xE8)
		{
			delete_current_object();
			return;
		}
		OBJECT(current_object)->sprite_frame = mem_read16(OBJECT(current_object)->animation + 2u + OBJECT(LINKED(current_object, OBJECT(current_object)->parent))->animation_frame * 2u);
		update_screen_position(&ignored, &r1);
		if ((uint16_t)(OBJECT(current_object)->screen_x + 0x10u) >= 0x1A1u || OBJECT(current_object)->screen_y <= -0xB0)
			return;
		depth = (uint16_t)(OBJECT(current_object)->screen_y + OBJECT(current_object)->river_water.depth_offset);
		queue_sprite_at_depth(&ignored, &depth);
		return;
	case 1:
		current_player = current_object;
		current_player = LINKED(current_player, OBJECT(current_object)->parent);
		if (OBJECT(current_player)->y_int <= OBJECT(current_object)->y_int)
			OBJECT(current_object)->river_water.state++;
		if (OBJECT(current_object)->screen_y > 0xE8)
		{
			delete_current_object();
			return;
		}
		OBJECT(current_object)->sprite_frame = mem_read16(OBJECT(current_object)->animation + 2u + OBJECT(LINKED(current_object, OBJECT(current_object)->parent))->animation_frame * 2u);
		update_screen_position(&ignored, &r1);
		if ((uint16_t)(OBJECT(current_object)->screen_x + 0x10u) >= 0x1A1u || OBJECT(current_object)->screen_y <= -0xB0)
			return;
		depth = (uint16_t)(OBJECT(current_object)->screen_y + OBJECT(current_object)->river_water.depth_offset);
		queue_sprite_at_depth(&ignored, &depth);
		return;
	case 2:
		if (free_object_stack_2[0] < 2)
		{
			if (OBJECT(current_object)->screen_y > 0xE8)
			{
				delete_current_object();
				return;
			}
			OBJECT(current_object)->sprite_frame = mem_read16(OBJECT(current_object)->animation + 2u + OBJECT(LINKED(current_object, OBJECT(current_object)->parent))->animation_frame * 2u);
			update_screen_position(&ignored, &r1);
			if ((uint16_t)(OBJECT(current_object)->screen_x + 0x10u) >= 0x1A1u || OBJECT(current_object)->screen_y <= -0xB0)
				return;
			depth = (uint16_t)(OBJECT(current_object)->screen_y + OBJECT(current_object)->river_water.depth_offset);
			queue_sprite_at_depth(&ignored, &depth);
			return;
		}
		OBJECT(current_object)->river_water.state++;
		clips_left = 2u;
		object_2 = current_object;
		do
		{
			object_3 = object_2;
			allocate_object_2(&ignored, &ignored, &object_2);
			object = object_2 + 2u;
			OBJECT(object - 0x2u)->sprite_flags = 0x5000u;
			object += 2u;
			OBJECT(object - 0x4u)->sprite_frame = 0xE0u;
			object += 2u;
			OBJECT(object - 0x6u)->sprite_palette = 0u;
			object += 2u;
			OBJECT(object - 0x8u)->sprite_scale = 0x140u;
			object += 2u;
			OBJECT(object - 0xAu)->sprite_layer = 0u;
			object += 2u;
			OBJECT(object - 0xCu)->screen_y = 0;
			object += 2u;
			OBJECT(object - 0xEu)->screen_x = 0;
			OBJECT(object - 0xEu)->hitbox_size_x = 0u;
			OBJECT(object_2)->parent = object_3;
			OBJECT(object_3)->child = object_2;
			clips_left--;
		} while (clips_left != 0u);
	case 3:
		OBJECT(current_object)->sprite_frame = mem_read16(OBJECT(current_object)->animation + 2u + OBJECT(LINKED(current_object, OBJECT(current_object)->parent))->animation_frame * 2u);
		update_screen_position(&ignored, &r1);
		object = LINKED(current_object, OBJECT(current_object)->parent);
		if ((int16_t)(OBJECT(current_object)->y_int - 0x5Cu) < OBJECT(object)->y_int)
		{
			clip_x = OBJECT(current_object)->x;
			point_y = OBJECT(object)->y;
			floor_z = OBJECT(current_object)->floor_z;
			project_point(floor_z, &ignored, &clip_x, &point_y);
			OBJECT(LINKED(current_object, OBJECT(current_object)->child))->sprite_scale = clip_x;
			if ((uint16_t)(OBJECT(current_object)->screen_x + 0x10u) < 0x1A1u)
			{
				OBJECT(current_object)->river_water.draw_depth = OBJECT(current_object)->screen_y + OBJECT(current_object)->river_water.depth_offset;
				current_object = LINKED(current_object, OBJECT(current_object)->child);
				current_object = LINKED(current_object, OBJECT(current_object)->child);
				depth = OBJECT(object_cursor)->river_water.draw_depth;
				queue_sprite_at_depth(&ignored, &depth);
				current_object = object_cursor;
				depth = OBJECT(object_cursor)->river_water.draw_depth;
				queue_sprite_at_depth(&ignored, &depth);
				current_object = LINKED(current_object, OBJECT(current_object)->child);
				depth = OBJECT(object_cursor)->river_water.draw_depth;
				queue_sprite_at_depth(&ignored, &depth);
			}
			return;
		}
		child = OBJECT(current_object)->child;
		free_object_2(child);
		current_object = LINKED(current_object, OBJECT(current_object)->child);
		child = OBJECT(current_object)->child;
		free_object_2(child);
		delete_current_object();
		return;
	default:
		unknown_code(0x8CEC1u + OBJECT(current_object)->river_water.state * 3u);
	}
}

void stone_block_init(void)
{
	uint32_t ignored = 0, record = 0;

	record = stone_block_positions_ADDRESS + OBJECT(current_object)->sprite_frame * 2u * 4u;
	OBJECT(current_object)->x_int = ROM16(record);
	OBJECT(current_object)->x_fraction = 0u;
	record += 2u;
	OBJECT(current_object)->y_int = ROM16(record);
	OBJECT(current_object)->y_fraction = 0u;
	record += 2u;
	OBJECT(current_object)->z_int = ROM16(record);
	OBJECT(current_object)->z_fraction = 0u;
	OBJECT(current_object)->stone_block.depth_offset = ROM16(record + 2u);
	OBJECT(current_object)->sprite_flags = 0x1000u;
	OBJECT(current_object)->sprite_frame = 0x8A00u;
	OBJECT(current_object)->sprite_palette = 0x9120u;
	OBJECT(current_object)->flags = 0u;
	OBJECT(current_object)->sprite_offset_x = 0;
	OBJECT(current_object)->sprite_offset_y = 0;
	project_object(&ignored, &ignored);
	OBJECT(current_object)->handler = stone_block_draw_ADDRESS;
}

void stone_block_draw(void)
{
	uint32_t r1 = 0, depth = 0, ignored = 0, off_screen = 0;

	off_screen = check_left_screen();
	if (off_screen == 0u)
	{
		update_screen_position(&ignored, &r1);
		depth = (uint16_t)(OBJECT(current_object)->screen_y + OBJECT(current_object)->stone_block.depth_offset);
		queue_sprite_at_depth(&ignored, &depth);
		return;
	}
	delete_current_object();
}

void spawn_geyser_line(uint32_t line)
{
	uint32_t free_count = 0, record = 0;

	allocate_object(&free_count, &line);
	if (free_count != 0u)
	{
		record = ((uint32_t)OBJECT(current_object)->sprite_frame << 3) + geyser_line_starts_ADDRESS;
		OBJECT(line)->x_int = ROM16(record);
		OBJECT(line)->x_fraction = 0u;
		record += 2u;
		OBJECT(line)->y_int = ROM16(record);
		OBJECT(line)->y_fraction = 0u;
		record += 2u;
		OBJECT(line)->z_int = ROM16(record);
		OBJECT(line)->z_fraction = 0u;
		OBJECT(line)->floor_z = OBJECT(line)->z;
		OBJECT(line)->geyser_line.path = ROM16(record + 2u) + geyser_line_scripts_ADDRESS;
		OBJECT(line)->geyser_line.holding_back = 0u;
		OBJECT(line)->geyser_line.wait_timer = 0;
		OBJECT(line)->flags = 0u;
		OBJECT(line)->handler = geyser_line_move_ADDRESS;
		OBJECT(line)->geyser_line.travel = 0;
	}
}

void geyser_line_move(void)
{
	uint32_t cursor = 0, hold_distance = 0, ignored = 0, speed = 0;
	uint8_t hold_timer = 0;
	uint16_t wait_timer = 0, trigger_scroll = 0, trigger_y = 0;

	if (OBJECT(current_object)->geyser_line.holding_back == 0u)
	{
		if (no_player_in_play != 0u || player_mishap != 0u || player_dropping_in != 0u)
		{
			OBJECT(current_object)->geyser_line.holding_back = 0xFFu;
			OBJECT(current_object)->geyser_line.hold_start_position = scroll_position;
			OBJECT(current_object)->geyser_line.hold_timer = 0xFFu;
		}
		else
		{
			wait_timer = OBJECT(current_object)->geyser_line.wait_timer;
			OBJECT(current_object)->geyser_line.wait_timer = wait_timer - 1u;
			if ((int16_t)wait_timer <= 1)
			{
				OBJECT(current_object)->geyser_line.wait_timer = 0;
				cursor = OBJECT(current_object)->geyser_line.path;
				trigger_scroll = mem_read16(cursor);
				cursor += 2u;
				if (trigger_scroll <= scroll_position)
				{
					cursor += 2u;
					goto geyser_line_load_record;
				}
				trigger_y = mem_read16(cursor);
				cursor += 2u;
				if (OBJECT(current_object)->y_int <= (int16_t)trigger_y)
				{
				geyser_line_load_record:
					if ((int16_t)mem_read16(cursor + 2u) < 0)
					{
						spawn_geyser_burst();
						delete_current_object();
						return;
					}
					OBJECT(current_object)->geyser_line.speed = mem_read16(cursor);
					cursor += 2u;
					OBJECT(current_object)->geyser_line.wait_timer = mem_read16(cursor);
					OBJECT(current_object)->geyser_line.path = cursor + 2u;
				}
			}
		}
	}
	project_object(&hold_distance, &ignored);
	if (OBJECT(current_object)->geyser_line.holding_back != 0u)
	{
		hold_distance = (hold_distance & ~0xFFFFu) | scroll_position;
		hold_distance = (hold_distance & ~0xFFFFu) | (uint16_t)(hold_distance - OBJECT(current_object)->geyser_line.hold_start_position);
		if ((int16_t)hold_distance >= 0x300)
			OBJECT(current_object)->geyser_line.holding_back = 0u;
		else
		{
			if (no_player_in_play != 0u)
				OBJECT(current_object)->geyser_line.hold_timer = 0xFFu;
			hold_timer = OBJECT(current_object)->geyser_line.hold_timer;
			OBJECT(current_object)->geyser_line.hold_timer = hold_timer - 1u;
			if (hold_timer == 0u)
			{
				OBJECT(current_object)->geyser_line.holding_back = 0u;
				if (OBJECT(current_object)->geyser_line.holding_back == 0u)
				{
					speed = (hold_distance & ~0xFFFFu) | (uint16_t)OBJECT(current_object)->geyser_line.speed;
					OBJECT(current_object)->geyser_line.travel += speed;
					OBJECT(current_object)->y -= speed << 12;
					if (OBJECT(current_object)->geyser_line.travel >= 0x300)
					{
						spawn_geyser_burst();
						OBJECT(current_object)->geyser_line.travel = 0;
					}
				}
				return;
			}
		}
	}
	if (OBJECT(current_object)->geyser_line.holding_back == 0u)
	{
		speed = (hold_distance & ~0xFFFFu) | (uint16_t)OBJECT(current_object)->geyser_line.speed;
		OBJECT(current_object)->geyser_line.travel += speed;
		OBJECT(current_object)->y -= speed << 12;
		if (OBJECT(current_object)->geyser_line.travel >= 0x300)
		{
			spawn_geyser_burst();
			OBJECT(current_object)->geyser_line.travel = 0;
		}
	}
}

void spawn_geyser_burst(void)
{
	uint32_t free_count = 0, object = 0;

	allocate_object(&free_count, &object);
	if (free_count != 0u)
	{
		OBJECT(object)->sprite_flags = 0x1004u;
		OBJECT(object)->sprite_palette = 0x9130u;
		OBJECT(object)->handler = geyser_burst_start_ADDRESS;
		OBJECT(object)->hitbox_size_x = 0x30u;
		OBJECT(object)->hitbox_size_y = 0x30u;
		OBJECT(object)->hitbox_offset_x = 0;
		OBJECT(object)->hitbox_offset_y = 0;
		OBJECT(object)->sprite_offset_x = 0x14;
		OBJECT(object)->sprite_offset_y = 8;
		OBJECT(object)->hitbox_height = 0xA0u;
		OBJECT(object)->flags = 2u;
		OBJECT(object)->kind = 0x29u;
		OBJECT(object)->direction = 0x100u;
		OBJECT(object)->animation = geyser_burst_animation_ADDRESS;
		OBJECT(object)->animation_frame = 0xFFu;
		OBJECT(object)->animation_speed = 6u;
		OBJECT(object)->animation_timer = 0;
		OBJECT(object)->x = OBJECT(current_object)->x;
		OBJECT(object)->y = OBJECT(current_object)->y;
		OBJECT(object)->z = OBJECT(current_object)->z;
	}
}

void geyser_burst_start(void)
{
	uint32_t ignored = 0;

	project_object(&ignored, &ignored);
	OBJECT(current_object)->handler = geyser_burst_erupt_ADDRESS;
}

void geyser_burst_erupt(void)
{
	uint8_t sound_id = 0;
	uint32_t r1 = 0, r7 = 0, r8 = 0, depth = 0, ignored = 0, write_index_address = 0;

	r8 = is_off_screen();
	if (r8 != 0u)
	{
		delete_current_object();
		return;
	}
	animate(&r1, &r7, &r8);
	if (OBJECT(current_object)->animation != geyser_burst_spray_animation_ADDRESS)
	{
		if (OBJECT(current_object)->animation_frame != 4u)
		{
			if ((uint8_t)r8 != 0u)
				OBJECT(current_object)->animation = geyser_burst_spray_animation_ADDRESS;
		}
		else
			OBJECT(current_object)->animation_speed = 2u;
	}
	if (OBJECT(current_object)->animation != geyser_burst_spray_animation_ADDRESS && OBJECT(current_object)->animation_frame == 4u && OBJECT(current_object)->animation_speed == (uint8_t)OBJECT(current_object)->animation_timer)
	{
		sound_id = 0xAu;
		play_sound(sound_id, &r1, &write_index_address);
	}
	update_screen_position(&ignored, &r1);
	if (OBJECT(current_object)->animation == geyser_burst_spray_animation_ADDRESS)
		add_to_touchable_list(ignored);
	depth = (uint16_t)(OBJECT(current_object)->screen_y - 0x10u);
	queue_sprite_at_depth(&ignored, &depth);
}
