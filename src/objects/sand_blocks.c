/**
 *    Name:        objects/sand_blocks.c
 *    Description: stage A: the sand block leader, the collapsing sand blocks, the swirl around a sinking player
 *    Version:     0.1.0
 *    Created:     12-10-2026
 *    Modified:    15-10-2026
 *    Author:      Sebastian Toyos
 *
 *    SegaSonic The Hedgehog (arcade, Sega System 32): the game's V60 program decompiled into C by
 *    tools/decompile.py, with the names of symbols.txt.
 */
#include "game.h"

void spawn_sand_block_leader(uint32_t record, uint32_t *p_spawned, uint32_t *p_leader)
{
	uint32_t leader = *p_leader;
	uint32_t free_count = 0, spawned = 0;

	allocate_object_outside_loop(&free_count, &leader);
	if (free_count == 0u)
	{
		*p_spawned = free_count;
		*p_leader = leader;
		return;
	}
	group_leaders[mem_read8(record) & 3u] = leader;
	record++;
	OBJECT(leader)->x_int = mem_read16(record);
	OBJECT(leader)->x_fraction = 0u;
	record += 2u;
	OBJECT(leader)->y_int = mem_read16(record);
	OBJECT(leader)->y_fraction = 0u;
	record += 2u;
	OBJECT(leader)->z_int = mem_read16(record);
	OBJECT(leader)->z_fraction = 0u;
	OBJECT(leader)->floor_z = OBJECT(leader)->z;
	record += 2u;
	OBJECT(leader)->direction = mem_read16(record);
	OBJECT(leader)->sand_block_leader.path = mem_read16(record + 2u) + group_leader_data_ADDRESS;
	OBJECT(leader)->handler = sand_block_leader_move_ADDRESS;
	OBJECT(leader)->flags = 0u;
	OBJECT(leader)->sprite_offset_x = 0;
	OBJECT(leader)->sprite_offset_y = 0;
	OBJECT(leader)->sand_block_leader.holding_back = 0u;
	OBJECT(leader)->sand_block_leader.wait_timer = 1;
	sand_block_leader_active = 0xFFu;
	if (group_leader_0_held != 0u)
		OBJECT(leader)->handler = sand_block_leader_wait_ADDRESS;
	spawned = 0xFFFFFFFFu;
	*p_spawned = spawned;
	*p_leader = leader;
}

void sand_block_leader_wait(void)
{
	if (scroll_position >= 0x5100u || group_leader_0_held == 0u)
		OBJECT(current_object)->handler = sand_block_leader_move_ADDRESS;
}

void sand_block_leader_move(void)
{
	uint16_t move_speed = 0;
	uint32_t r3 = 0, block_offset = 0, block_record = 0, hold_threshold = 0, ignored = 0, path_cursor = 0, x = 0;
	uint8_t hold_timer = 0;
	uint16_t wait_timer = 0, record_scroll = 0;
	uint32_t flags = 0;

	flags = OBJECT(current_object)->flags;
	OBJECT(current_object)->flags = flags | 0x80000000u;
	if ((flags & 0x80000000u) == 0u)
	{
	sand_block_leader_read_script:
		wait_timer = OBJECT(current_object)->sand_block_leader.wait_timer;
		OBJECT(current_object)->sand_block_leader.wait_timer = wait_timer - 1u;
		if ((int16_t)wait_timer <= 1)
		{
			OBJECT(current_object)->sand_block_leader.wait_timer = 0;
			path_cursor = OBJECT(current_object)->sand_block_leader.path;
			record_scroll = mem_read16(path_cursor);
			path_cursor += 2u;
			if (record_scroll <= scroll_position)
			{
				if ((int16_t)mem_read16(path_cursor + 2u) < 0)
				{
					delete_current_object();
					return;
				}
				OBJECT(current_object)->sand_block_leader.screen_threshold = mem_read16(path_cursor);
				path_cursor += 2u;
				OBJECT(current_object)->sand_block_leader.catch_up_factor = mem_read16(path_cursor);
				path_cursor += 2u;
				OBJECT(current_object)->sand_block_leader.base_speed = mem_read16(path_cursor);
				path_cursor += 2u;
				OBJECT(current_object)->sand_block_leader.wait_timer = mem_read16(path_cursor);
				OBJECT(current_object)->sand_block_leader.path = path_cursor + 2u;
			}
		}
	}
	else if (OBJECT(current_object)->sand_block_leader.holding_back == 0u)
	{
		if (no_player_in_play == 0u && player_mishap == 0u && player_dropping_in == 0u)
			goto sand_block_leader_read_script;
		OBJECT(current_object)->sand_block_leader.holding_back = 0xFFu;
		OBJECT(current_object)->sand_block_leader.saved_base_speed = OBJECT(current_object)->sand_block_leader.base_speed;
		OBJECT(current_object)->sand_block_leader.base_speed = 0;
		OBJECT(current_object)->sand_block_leader.hold_start_position = scroll_position;
		OBJECT(current_object)->sand_block_leader.hold_timer = 0xFFu;
		OBJECT(current_object)->sand_block_leader.saved_threshold = OBJECT(current_object)->sand_block_leader.screen_threshold;
		hold_threshold = 0xFFE0u;
		if ((OBJECT(current_object)->direction & 1u) != 0u)
			hold_threshold = 0x160u;
		OBJECT(current_object)->sand_block_leader.screen_threshold = hold_threshold;
	}
	find_floor(&x, &hold_threshold, &block_offset, &r3, &block_record);
	project_object(&ignored, &ignored);
	add_to_rider_list();
	if (OBJECT(current_object)->sand_block_leader.holding_back != 0u)
	{
		if ((int16_t)(scroll_position - OBJECT(current_object)->sand_block_leader.hold_start_position) >= 0x300)
			goto sand_block_leader_stop_holding_back;
		if (no_player_in_play != 0u)
			OBJECT(current_object)->sand_block_leader.hold_timer = 0xFFu;
		hold_timer = OBJECT(current_object)->sand_block_leader.hold_timer;
		OBJECT(current_object)->sand_block_leader.hold_timer = hold_timer - 1u;
		if (hold_timer == 0u)
			goto sand_block_leader_stop_holding_back;
	}
	switch (OBJECT(current_object)->direction)
	{
	case 0:
	case 2:
	case 4:
		if (OBJECT(current_object)->sand_block_leader.screen_threshold <= OBJECT(current_object)->screen_x)
		{
		sand_block_leader_use_base_speed:
			move_speed = (uint16_t)OBJECT(current_object)->sand_block_leader.base_speed;
		}
		else
		{
		sand_block_leader_catch_up:
			OBJECT(current_object)->sand_block_leader.wait_timer = 0;
			if (OBJECT(current_object)->sand_block_leader.holding_back != 0u)
			{
			sand_block_leader_stop_holding_back:
				OBJECT(current_object)->sand_block_leader.holding_back = 0u;
				OBJECT(current_object)->sand_block_leader.screen_threshold = OBJECT(current_object)->sand_block_leader.saved_threshold;
				OBJECT(current_object)->sand_block_leader.base_speed = OBJECT(current_object)->sand_block_leader.saved_base_speed;
			}
			compute_camera_ground_speed(r3);
			move_speed = (uint16_t)((uint32_t)camera_ground_speed_int * OBJECT(current_object)->sand_block_leader.catch_up_factor) >> 8;
			if ((int16_t)move_speed <= OBJECT(current_object)->sand_block_leader.base_speed)
				goto sand_block_leader_use_base_speed;
		}
		break;
	case 1:
	case 3:
		if (OBJECT(current_object)->sand_block_leader.screen_threshold < OBJECT(current_object)->screen_x || OBJECT(current_object)->screen_y > 0xE0)
			goto sand_block_leader_catch_up;
		goto sand_block_leader_use_base_speed;
	default:
		unknown_code(0x8A046u + OBJECT(current_object)->direction * 2u);
	}
	chaser_speed = move_speed;
	if (OBJECT(current_object)->sand_block_leader.wait_timer <= 0)
	{
		block_record = floor_blocks + 2u + OBJECT(current_object)->floor_block;
		switch (OBJECT(current_object)->direction)
		{
		case 0:
		case 4:
			OBJECT(current_object)->y += (uint32_t)chaser_speed << 12;
			OBJECT(current_object)->x = ((int32_t)(mem_read32(block_record + 4u) - mem_read32(block_record)) >> 1) + mem_read32(block_record);
			break;
		case 1:
			OBJECT(current_object)->y -= (uint32_t)chaser_speed << 12;
			OBJECT(current_object)->x = ((int32_t)(mem_read32(block_record + 4u) - mem_read32(block_record)) >> 1) + mem_read32(block_record);
			break;
		case 2:
			OBJECT(current_object)->x += (uint32_t)chaser_speed << 12;
			OBJECT(current_object)->y = ((int32_t)(mem_read32(block_record + 0xCu) - mem_read32(block_record + 8u)) >> 1) + mem_read32(block_record + 8u);
			break;
		case 3:
			OBJECT(current_object)->x -= (uint32_t)chaser_speed << 12;
			OBJECT(current_object)->y = ((int32_t)(mem_read32(block_record + 0xCu) - mem_read32(block_record + 8u)) >> 1) + mem_read32(block_record + 8u);
			break;
		default:
			unknown_code(0x8A0D4u + OBJECT(current_object)->direction * 2u);
		}
	}
	if (OBJECT(current_object)->y_int <= 0xA2B)
		sand_block_leader_active = 0u;
}

void sand_block_update(void)
{
	uint8_t sound_id = 0;
	uint16_t child = 0, part_scale = 0, span_y = 0;
	uint32_t r1 = 0, r2 = 0, blocks_left = 0, depth = 0, floor_z = 0, ignored = 0, object = 0, object_2 = 0, object_3 = 0, part_cursor = 0, part_list = 0, point_x = 0, point_y = 0, stream_parts_left = 0;

	switch (OBJECT(current_object)->sand_block.state)
	{
	case 0:
	case 4:
		project_object(&ignored, &r1);
		OBJECT(current_object)->sand_block.state++;
		current_player = current_object;
		current_player = LINKED(current_player, OBJECT(current_object)->parent);
		switch (OBJECT(current_object)->direction)
		{
		case 0:
		case 1:
		case 2:
		case 3:
		case 4:
			break;
		default:
			unknown_code(0x8A1A5u + OBJECT(current_object)->direction * 2u);
		}
		if (OBJECT(current_player)->y_int <= (int16_t)(OBJECT(current_object)->y_int + OBJECT(current_object)->hitbox_size_y - 0x10u))
			OBJECT(current_object)->sand_block.state++;
		add_to_platform_list();
		update_screen_position(&ignored, &r1);
		if ((uint16_t)(OBJECT(current_object)->screen_x + 0x140u) >= 0x281u)
			return;
		depth = (uint16_t)(OBJECT(current_object)->screen_y + OBJECT(current_object)->sand_block.depth_offset - OBJECT(current_object)->sprite_offset_y);
		OBJECT(current_object)->sand_block.draw_depth = depth;
		queue_sprite_at_depth(&ignored, &depth);
		return;
	case 1:
		current_player = current_object;
		current_player = LINKED(current_player, OBJECT(current_object)->parent);
		switch (OBJECT(current_object)->direction)
		{
		case 0:
		case 1:
		case 2:
		case 3:
		case 4:
			break;
		default:
			unknown_code(0x8A1A5u + OBJECT(current_object)->direction * 2u);
		}
		if (OBJECT(current_player)->y_int <= (int16_t)(OBJECT(current_object)->y_int + OBJECT(current_object)->hitbox_size_y - 0x10u))
			OBJECT(current_object)->sand_block.state++;
		add_to_platform_list();
		update_screen_position(&ignored, &r1);
		if ((uint16_t)(OBJECT(current_object)->screen_x + 0x140u) >= 0x281u)
			return;
		depth = (uint16_t)(OBJECT(current_object)->screen_y + OBJECT(current_object)->sand_block.depth_offset - OBJECT(current_object)->sprite_offset_y);
		OBJECT(current_object)->sand_block.draw_depth = depth;
		queue_sprite_at_depth(&ignored, &depth);
		return;
	case 2:
		if (free_object_stack < 2 || free_object_stack_2[0] < 3)
		{
			add_to_platform_list();
			update_screen_position(&ignored, &r1);
			if ((uint16_t)(OBJECT(current_object)->screen_x + 0x140u) >= 0x281u)
				return;
			depth = (uint16_t)(OBJECT(current_object)->screen_y + OBJECT(current_object)->sand_block.depth_offset - OBJECT(current_object)->sprite_offset_y);
			OBJECT(current_object)->sand_block.draw_depth = depth;
			queue_sprite_at_depth(&ignored, &depth);
			return;
		}
		OBJECT(current_object)->sand_block.state++;
		allocate_object(&ignored, &object_2);
		OBJECT(object_2)->sprite_flags = 0xD002u;
		OBJECT(object_2)->handler = sand_face_update_ADDRESS;
		OBJECT(object_2)->flags = 0u;
		OBJECT(object_2)->speed_z = 0;
		OBJECT(object_2)->var_96 = 0x70u;
		OBJECT(object_2)->animation_frame = 0u;
		OBJECT(object_2)->animation_speed = 2u;
		OBJECT(object_2)->animation_timer = 0;
		OBJECT(object_2)->animation = sand_face_animation_ADDRESS;
		OBJECT(object_2)->x = OBJECT(current_object)->x;
		OBJECT(object_2)->y = OBJECT(current_object)->y;
		OBJECT(object_2)->sprite_offset_x = -6;
		OBJECT(object_2)->sprite_offset_y = 0x10;
		OBJECT(object_2)->z = OBJECT(current_object)->z;
		OBJECT(object_2)->parent = current_object;
		OBJECT(current_object)->sand_block.face_object = object_2;
		object_3 = object_2;
		allocate_object_2(&ignored, &ignored, &object_2);
		move_halfwords_backward(object_2, sand_face_parts_ADDRESS, 9u);
		part_list = OBJECT(current_object)->part_list;
		OBJECT(object_2)->sprite_scale = mem_read16(part_list + 8u);
		OBJECT(object_2)->hitbox_size_x = mem_read16(part_list + 8u);
		OBJECT(object_3)->part_list = object_2;
		allocate_object(&ignored, &object_2);
		OBJECT(object_2)->sprite_flags = 0xD002u;
		OBJECT(object_2)->handler = sand_streams_update_ADDRESS;
		OBJECT(object_2)->flags = 0u;
		move_halfwords_backward(object_2 + 0x40u, sand_stream_parts_ADDRESS, 0x19u);
		OBJECT(object_2)->part_list = object_2 + 0x40u;
		part_scale = (uint16_t)mem_read32(OBJECT(object_3)->part_list + 8u);
		part_cursor = OBJECT(object_2)->part_list;
		stream_parts_left = mem_read16(part_cursor);
		part_cursor += 2u;
		do
		{
			mem_write16(part_cursor + 6u, part_scale);
			part_cursor += 8u;
			stream_parts_left--;
		} while (stream_parts_left != 0u);
		OBJECT(object_2)->parent = object_3;
		OBJECT(object_3)->child = object_2;
		blocks_left = 2u;
		object_2 = current_object;
		do
		{
			object_3 = object_2;
			allocate_object_2(&ignored, &r2, &object_2);
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
			blocks_left--;
		} while (blocks_left != 0u);
		if (OBJECT(current_object)->screen_x <= 0x148 && OBJECT(current_object)->screen_y <= 0x130)
		{
			sound_id = 0x11u;
			play_sound(sound_id, &r1, &r2);
		}
	case 3:
		update_screen_position(&ignored, &r1);
		object = LINKED(current_object, OBJECT(current_object)->parent);
		span_y = (uint16_t)(OBJECT(object)->y_int - OBJECT(current_object)->y_int);
		OBJECT(current_object)->hitbox_size_y = span_y;
		if ((int16_t)span_y > 0)
		{
			point_x = OBJECT(current_object)->x + ((uint32_t)OBJECT(current_object)->hitbox_size_x << 16);
			point_y = OBJECT(object)->y;
			floor_z = OBJECT(current_object)->floor_z;
			project_point(floor_z, &ignored, &point_x, &point_y);
			OBJECT(LINKED(current_object, OBJECT(current_object)->child))->sprite_scale = point_x;
			add_to_platform_list();
			if ((uint16_t)(OBJECT(current_object)->screen_x + 0x140u) < 0x281u)
			{
				OBJECT(current_object)->sand_block.draw_depth = OBJECT(current_object)->screen_y + OBJECT(current_object)->sand_block.depth_offset - OBJECT(current_object)->sprite_offset_y;
				current_object = LINKED(current_object, OBJECT(current_object)->child);
				current_object = LINKED(current_object, OBJECT(current_object)->child);
				depth = OBJECT(object_cursor)->sand_block.draw_depth;
				queue_sprite_at_depth(&ignored, &depth);
				current_object = object_cursor;
				depth = OBJECT(object_cursor)->sand_block.draw_depth;
				queue_sprite_at_depth(&ignored, &depth);
				current_object = LINKED(current_object, OBJECT(current_object)->child);
				depth = OBJECT(object_cursor)->sand_block.draw_depth;
				queue_sprite_at_depth(&ignored, &depth);
			}
			return;
		}
		object = (current_object & ~0xFFFFu) | OBJECT(current_object)->sand_block.face_object;
		OBJECT(object)->flags |= 0x400u;
		child = OBJECT(current_object)->child;
		free_object_2(child);
		current_object = LINKED(current_object, OBJECT(current_object)->child);
		child = OBJECT(current_object)->child;
		free_object_2(child);
		delete_current_object();
		return;
	default:
		unknown_code(0x8A17Cu + OBJECT(current_object)->sand_block.state * 3u);
	}
}

void sand_face_update(void)
{
	uint16_t part_list = 0;
	uint32_t r8 = 0, depth = 0, ignored = 0, object = 0, part = 0;

	if ((OBJECT(current_object)->flags & 0x400u) == 0u)
	{
		object = LINKED(current_object, OBJECT(current_object)->parent);
		object = LINKED(object, OBJECT(object)->parent);
		OBJECT(current_object)->y = OBJECT(object)->y;
		OBJECT(current_object)->z = OBJECT(object)->z;
	}
	else
	{
		if (OBJECT(current_object)->screen_y >= 0x150)
		{
			part_list = (uint16_t)OBJECT(current_object)->part_list;
			free_object_2(part_list);
			object = LINKED(current_object, OBJECT(current_object)->child);
			OBJECT(object)->flags |= 0x400u;
			delete_current_object();
			return;
		}
		OBJECT(current_object)->sand_face.depth_offset = 0x38;
		OBJECT(current_object)->animation = sand_face_fall_animation_ADDRESS;
		OBJECT(current_object)->speed_z += 0x6000u;
		OBJECT(current_object)->z += OBJECT(current_object)->speed_z;
	}
	animate_all_parts(&r8, &part);
	project_object(&ignored, &ignored);
	depth = (uint16_t)(OBJECT(current_object)->screen_y - OBJECT(current_object)->sand_face.depth_offset);
	queue_sprite_at_depth(&ignored, &depth);
}

void sand_streams_update(void)
{
	uint16_t stream_frame = 0;
	uint32_t depth = 0, ignored = 0, object = 0, part_cursor = 0, parts_left = 0;

	if ((OBJECT(current_object)->flags & 0x400u) != 0u)
	{
		delete_current_object();
		return;
	}
	object = LINKED(current_object, OBJECT(current_object)->parent);
	OBJECT(current_object)->screen_x = OBJECT(object)->screen_x;
	OBJECT(current_object)->screen_x -= OBJECT(object)->sprite_offset_x;
	OBJECT(current_object)->screen_y = OBJECT(object)->screen_y;
	OBJECT(current_object)->screen_y -= OBJECT(object)->sprite_offset_y;
	stream_frame = sand_stream_frames[OBJECT(object)->animation_frame];
	part_cursor = OBJECT(current_object)->part_list;
	parts_left = mem_read16(part_cursor);
	part_cursor += 2u;
	do
	{
		mem_write16(part_cursor, stream_frame);
		part_cursor += 8u;
		parts_left--;
	} while (parts_left != 0u);
	depth = (uint16_t)(OBJECT(object)->screen_y - OBJECT(object)->var_96);
	queue_sprite_at_depth(&ignored, &depth);
}

uint32_t spawn_sink_swirl(uint32_t allocated)
{
	uint32_t effect_record = 0, ignored = 0, object = 0, swirls_left = 0;

	if (free_object_stack < 4)
		return allocated;
	effect_record = sink_swirl_effects_ADDRESS;
	swirls_left = 2u;
	do
	{
		allocate_object(&ignored, &allocated);
		OBJECT(allocated)->sprite_flags = 0x1004u;
		OBJECT(allocated)->sprite_palette = 0x1210u;
		AS16(OBJECT(allocated)->flags) = 0u;
		OBJECT(allocated)->sprite_offset_x = 0;
		OBJECT(allocated)->sprite_offset_y = 0;
		OBJECT(allocated)->sink_swirl.depth_offset = ROM16(effect_record);
		OBJECT(allocated)->handler = sink_swirl_start_ADDRESS;
		effect_record += 2u;
		OBJECT(allocated)->animation = ROM32(effect_record);
		OBJECT(allocated)->animation_frame = 0xFFu;
		OBJECT(allocated)->animation_speed = 0xAu;
		OBJECT(allocated)->animation_timer = 0;
		OBJECT(allocated)->x = OBJECT(current_object)->x;
		OBJECT(allocated)->y = OBJECT(current_object)->y;
		OBJECT(allocated)->z = OBJECT(current_object)->z;
		OBJECT(allocated)->z_int += 8u;
		object = allocated;
		effect_record += 4u;
		take_free_object(&ignored, &allocated);
		mem_write16(allocated + 2u, 0x1004u);
		mem_write16(allocated + 6u, 0x1230u);
		mem_write16(allocated + 0x30u, 0u);
		mem_write32(allocated + 0x48u, ROM32(effect_record));
		mem_write8(allocated + 0x46u, 0xFFu);
		mem_write8(allocated + 0x44u, 4u);
		mem_write8(allocated + 0x45u, 0u);
		OBJECT(object)->child = allocated;
		swirls_left--;
		effect_record += 4u;
	} while (swirls_left != 0u);
	return allocated;
}

void sink_swirl_start(void)
{
	uint32_t ignored = 0;

	project_object(&ignored, &ignored);
	OBJECT(current_object)->handler = sink_swirl_animate_ADDRESS;
	sink_swirl_animate();
}

void sink_swirl_animate(void)
{
	uint32_t r1 = 0, r7 = 0, at_last_step = 0, depth = 0, ignored = 0;

	animate(&r1, &r7, &at_last_step);
	if ((uint8_t)at_last_step != 0u)
	{
		current_object = LINKED(current_object, OBJECT(current_object)->child);
		push_free_object();
		delete_current_object();
		return;
	}
	update_screen_position(&ignored, &r1);
	current_object = LINKED(current_object, OBJECT(current_object)->child);
	animate(&r1, &r7, &at_last_step);
	OBJECT(current_object)->screen_x = OBJECT(object_cursor)->screen_x;
	OBJECT(current_object)->screen_y = OBJECT(object_cursor)->screen_y;
	depth = (uint16_t)(OBJECT(current_object)->screen_y + OBJECT(object_cursor)->sink_swirl.depth_offset);
	queue_sprite_at_depth(&ignored, &depth);
	current_object = object_cursor;
	depth = (uint16_t)(OBJECT(current_object)->screen_y + OBJECT(current_object)->sink_swirl.depth_offset);
	queue_sprite_at_depth(&ignored, &depth);
}
