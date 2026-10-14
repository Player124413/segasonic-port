/**
 *    Name:        game_08.c
 *    Description: the game's functions, part 08 (in the order of their V60 addresses)
 *    Version:     0.1.0
 *    Created:     12-10-2026
 *    Modified:    14-10-2026
 *    Author:      Sebastian Toyos
 *
 *    SegaSonic The Hedgehog (arcade, Sega System 32): the game's V60 program decompiled into C by
 *    tools/decompile.py, with the names of symbols.txt.
 */
#include "game.h"

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

void brick_wall_init(void)
{
	uint32_t ignored = 0;

	OBJECT(current_object)->sprite_flags = 0x1002u;
	OBJECT(current_object)->sprite_frame = 0x7D90u;
	OBJECT(current_object)->sprite_palette = 0x10D0u;
	OBJECT(current_object)->sprite_offset_x = 0;
	OBJECT(current_object)->sprite_offset_y = 8;
	OBJECT(current_object)->flags = 0u;
	OBJECT(current_object)->floor_z = 0x1D80000;
	OBJECT(current_object)->z = OBJECT(current_object)->floor_z;
	OBJECT(current_object)->handler = brick_wall_wait_ADDRESS;
	project_object(&ignored, &ignored);
}

void brick_wall_wait(void)
{
	uint16_t screen_x = 0;
	uint32_t r1 = 0, brick_animation = 0, depth = 0, free_count = 0, ignored = 0, object = 0, piece_record = 0, pieces_left = 0;

	if (chaser_stopped == 0u)
	{
		update_screen_position(&ignored, &r1);
		depth = (uint16_t)(OBJECT(current_object)->screen_y - 0x10u);
		queue_sprite_at_depth(&ignored, &depth);
		return;
	}
	screen_x = (uint16_t)OBJECT(current_object)->screen_x;
	r14 = (uint16_t)OBJECT(current_object)->screen_y;
	pieces_left = brick_wall_bricks;
	piece_record = brick_wall_brick_records_ADDRESS;
	do
	{
		allocate_object(&free_count, &object);
		if (free_count == 0u)
		{
			delete_current_object();
			return;
		}
		OBJECT(object)->sprite_flags = ROM16(piece_record);
		OBJECT(object)->screen_x = screen_x;
		piece_record += 2u;
		OBJECT(object)->screen_x += ROM16(piece_record);
		OBJECT(object)->screen_y = r14;
		piece_record += 2u;
		OBJECT(object)->screen_y += ROM16(piece_record);
		piece_record += 2u;
		OBJECT(object)->speed_x_fraction = ROM16(piece_record);
		piece_record += 2u;
		OBJECT(object)->speed_y_fraction = ROM16(piece_record);
		piece_record += 2u;
		brick_animation = brick_animations_ADDRESS + ROM16(piece_record);
		OBJECT(object)->sprite_palette = ROM16(brick_animation);
		OBJECT(object)->animation = brick_animation + 2u;
		OBJECT(object)->projected_x = (uint32_t)OBJECT(object)->screen_x << 4;
		OBJECT(object)->projected_y_fraction = (uint32_t)OBJECT(object)->screen_y << 4;
		OBJECT(object)->animation_frame = 0u;
		OBJECT(object)->animation_speed = 4u;
		OBJECT(object)->animation_timer = 0;
		OBJECT(object)->flags = 0u;
		OBJECT(object)->sprite_scale = 0x100u;
		OBJECT(object)->handler = rock_debris_fly_ADDRESS;
		piece_record += 2u;
		pieces_left--;
	} while (pieces_left != 0u);
	screen_x = (uint16_t)OBJECT(current_object)->screen_x;
	r14 = (uint16_t)OBJECT(current_object)->screen_y;
	pieces_left = brick_wall_dust;
	piece_record = brick_wall_dust_records_ADDRESS;
	do
	{
		allocate_object(&free_count, &object);
		if (free_count == 0u)
			break;
		OBJECT(object)->sprite_flags = ROM16(piece_record);
		OBJECT(object)->screen_x = screen_x;
		piece_record += 2u;
		OBJECT(object)->screen_x += ROM16(piece_record);
		OBJECT(object)->screen_y = r14;
		piece_record += 2u;
		OBJECT(object)->screen_y += ROM16(piece_record);
		piece_record += 2u;
		AS16(OBJECT(object)->var_80) = ROM16(piece_record);
		OBJECT(object)->animation = brick_dust_animation_ADDRESS;
		OBJECT(object)->sprite_palette = 0x10E0u;
		OBJECT(object)->animation_frame = 0u;
		OBJECT(object)->animation_speed = 4u;
		OBJECT(object)->animation_timer = 0;
		OBJECT(object)->flags = 0u;
		OBJECT(object)->sprite_scale = 0x100u;
		OBJECT(object)->handler = brick_dust_animate_ADDRESS;
		piece_record += 2u;
		pieces_left--;
	} while (pieces_left != 0u);
	delete_current_object();
}

void brick_dust_animate(void)
{
	uint32_t r1 = 0, r7 = 0, at_last_step = 0;
	uint16_t wait_timer = 0;

	wait_timer = OBJECT(current_object)->brick_dust.wait_timer;
	OBJECT(current_object)->brick_dust.wait_timer = wait_timer - 1u;
	if ((int16_t)wait_timer > 1)
		return;
	OBJECT(current_object)->brick_dust.wait_timer = 0;
	animate(&r1, &r7, &at_last_step);
	if ((int8_t)at_last_step >= 0)
	{
		r1 = queue_sprite(r1);
		return;
	}
	delete_current_object();
}

void biting_head_init(void)
{
	uint32_t free_count = 0, head_record = 0, taken_object = 0;

	head_record = biting_head_paths_ADDRESS + biting_head_paths[OBJECT(current_object)->sprite_frame];
	OBJECT(current_object)->x_int = ROM16(head_record);
	OBJECT(current_object)->x_fraction = 0u;
	head_record += 2u;
	OBJECT(current_object)->y_int = ROM16(head_record);
	OBJECT(current_object)->y_fraction = 0u;
	OBJECT(current_object)->biting_head_view.path = head_record + 2u;
	OBJECT(current_object)->sprite_flags = 0xD004u;
	OBJECT(current_object)->flags = 0x40000000u;
	OBJECT(current_object)->biting_head_view.holding_back = 0u;
	OBJECT(current_object)->kind = 9u;
	OBJECT(current_object)->direction = 0x300u;
	OBJECT(current_object)->biting_head_view.wait_timer = 1;
	OBJECT(current_object)->hitbox_height = 0xF0u;
	OBJECT(current_object)->hitbox_size_x = 0x80u;
	OBJECT(current_object)->hitbox_size_y = 0x40u;
	OBJECT(current_object)->hitbox_offset_x = 0x20;
	OBJECT(current_object)->hitbox_offset_y = -0x20;
	OBJECT(current_object)->biting_head_view.bounce_index = -1;
	OBJECT(current_object)->floor_z = 0x1D80000;
	OBJECT(current_object)->z = 0x1D80000;
	AS8(OBJECT(current_object)->spawn_arg_2) = 0u;
	biting_head = current_object;
	biting_head_done = 0u;
	OBJECT(current_object)->animation_frame = 0u;
	OBJECT(current_object)->animation_timer = 0;
	OBJECT(current_object)->biting_head_view.bite_step = 0u;
	OBJECT(current_object)->biting_head_view.bite_timer = 0;
	take_free_object(&free_count, &taken_object);
	if (free_count != 0u)
	{
		move_halfwords_backward(taken_object, biting_head_parts_ADDRESS, 0x19u);
		OBJECT(current_object)->part_list = taken_object;
		move_halfwords_backward(taken_object + 0x40u, biting_head_clip_entry_ADDRESS, 0xAu);
		move_halfwords_backward(taken_object + 0x54u, biting_head_clip_entry_ADDRESS, 0xAu);
		take_free_object(&free_count, &taken_object);
		if (free_count != 0u)
		{
			mem_write16(taken_object + 0x10u, 0x200u);
			mem_write16(taken_object + 0x12u, 0x130u);
			mem_write16(taken_object + 0x24u, 0x300u);
			mem_write16(taken_object + 0x66u, 0xF0u);
			mem_write8(taken_object + 0x7Cu, 0u);
			mem_write8(taken_object + 0x7Du, 0x80u);
			mem_write32(taken_object + 0x30u, 0x40000000u);
			mem_write8(taken_object + 0x47u, 9u);
			OBJECT(current_object)->child = taken_object;
			OBJECT(current_object)->handler = biting_head_wait_ADDRESS;
		}
	}
}

void biting_head_wait(void)
{
	if (chaser_stopped == 0u)
	{
		if (scroll_position < 0x1200u)
			return;
		chaser_stopped = 0xFFu;
	}
	OBJECT(current_object)->handler = biting_head_chase_ADDRESS;
}

void biting_head_chase(void)
{
	uint16_t bounce_dy = 0, hold_threshold = 0, move_frame = 0, move_speed = 0, record_coordinate = 0;
	uint32_t bite_record = 0, depth = 0, direction = 0, floor_slope = 0, ignored = 0, move_frames = 0, next_bite_step = 0, next_frame = 0, object = 0, path_cursor = 0, point_x = 0, point_y = 0, slope_factor = 0, z = 0;
	uint8_t hold_timer = 0, animation_timer = 0, bob_phase = 0, bite_timer = 0;
	uint16_t wait_timer = 0, record_scroll = 0;
	uint32_t flags = 0;

	flags = OBJECT(current_object)->flags;
	OBJECT(current_object)->flags = flags | 0x80000000u;
	if ((flags & 0x80000000u) == 0u)
	{
	biting_head_read_script:
		wait_timer = OBJECT(current_object)->biting_head_view.wait_timer;
		OBJECT(current_object)->biting_head_view.wait_timer = wait_timer - 1u;
		if ((int16_t)wait_timer <= 1)
		{
			OBJECT(current_object)->biting_head_view.wait_timer = 0;
			path_cursor = OBJECT(current_object)->biting_head_view.path;
			record_scroll = mem_read16(path_cursor);
			path_cursor += 2u;
			if (record_scroll <= scroll_position)
			{
				path_cursor += 2u;
				goto biting_head_load_record;
			}
			record_coordinate = mem_read16(path_cursor);
			direction = OBJECT(current_object)->direction;
			path_cursor += 2u;
			switch (direction >> 8)
			{
			case 0:
			case 4:
				if (OBJECT(current_object)->y_int <= (int16_t)record_coordinate)
				{
				biting_head_load_record:
					if ((int16_t)mem_read16(path_cursor + 2u) < 0)
					{
						current_object = (current_object & ~0xFFFFu) | OBJECT(current_object)->sprite_frame;
						push_free_object();
						current_object = LINKED(current_object, OBJECT(object_cursor)->child);
						push_free_object();
						delete_current_object();
						return;
					}
					OBJECT(current_object)->biting_head_view.screen_threshold = mem_read16(path_cursor);
					path_cursor += 2u;
					OBJECT(current_object)->biting_head_view.catch_up_factor = mem_read16(path_cursor);
					path_cursor += 2u;
					OBJECT(current_object)->biting_head_view.base_speed = mem_read16(path_cursor);
					path_cursor += 2u;
					OBJECT(current_object)->biting_head_view.wait_timer = mem_read16(path_cursor);
					path_cursor += 2u;
					OBJECT(current_object)->direction = mem_read16(path_cursor);
					OBJECT(current_object)->biting_head_view.path = path_cursor + 2u;
				}
				break;
			case 1:
				if ((int16_t)record_coordinate <= OBJECT(current_object)->x_int)
					goto biting_head_load_record;
				break;
			case 2:
				if ((int16_t)record_coordinate <= OBJECT(current_object)->y_int)
					goto biting_head_load_record;
				break;
			case 3:
				if (OBJECT(current_object)->x_int <= (int16_t)record_coordinate)
					goto biting_head_load_record;
				break;
			default:
				unknown_code(0x8ABC3u + (direction >> 8) * 2u);
			}
		}
	}
	else if (OBJECT(current_object)->biting_head_view.holding_back == 0u)
	{
		if (no_player_in_play == 0u && player_mishap == 0u && player_dropping_in == 0u)
			goto biting_head_read_script;
		OBJECT(current_object)->biting_head_view.holding_back = 0xFFu;
		OBJECT(current_object)->biting_head_view.saved_base_speed = OBJECT(current_object)->biting_head_view.base_speed;
		OBJECT(current_object)->biting_head_view.base_speed = 0;
		OBJECT(current_object)->biting_head_view.hold_start_position = scroll_position;
		OBJECT(current_object)->biting_head_view.hold_timer = 0xFFu;
		OBJECT(current_object)->biting_head_view.saved_threshold = OBJECT(current_object)->biting_head_view.screen_threshold;
		hold_threshold = 0xFFE0u;
		if (OBJECT(current_object)->direction < 0x100 || OBJECT(current_object)->direction > 0x200)
			hold_threshold = 0x160u;
		OBJECT(current_object)->biting_head_view.screen_threshold = hold_threshold;
	}
	project_object(&ignored, &ignored);
	object = LINKED(current_object, OBJECT(current_object)->child);
	OBJECT(object)->x = OBJECT(current_object)->x;
	OBJECT(object)->x_int += 0x40u;
	OBJECT(object)->y = OBJECT(current_object)->y;
	OBJECT(object)->z = OBJECT(current_object)->z;
	current_object = LINKED(current_object, OBJECT(current_object)->child);
	add_to_zone_list(ignored);
	current_object = object_cursor;
	add_to_zone_list(ignored);
	if (OBJECT(current_object)->biting_head_view.holding_back != 0u)
	{
		if ((int16_t)(scroll_position - OBJECT(current_object)->biting_head_view.hold_start_position) >= 0x300)
			goto biting_head_stop_holding_back;
		if (game_mode != 3u)
			OBJECT(current_object)->biting_head_view.hold_timer = 0xFFu;
		hold_timer = OBJECT(current_object)->biting_head_view.hold_timer;
		OBJECT(current_object)->biting_head_view.hold_timer = hold_timer - 1u;
		if (hold_timer == 0u)
			goto biting_head_stop_holding_back;
	}
	if (OBJECT(current_object)->screen_y < 0)
	{
	biting_head_catch_up:
		OBJECT(current_object)->biting_head_view.wait_timer = 0;
		if (OBJECT(current_object)->biting_head_view.holding_back != 0u)
		{
		biting_head_stop_holding_back:
			OBJECT(current_object)->biting_head_view.holding_back = 0u;
			OBJECT(current_object)->biting_head_view.screen_threshold = OBJECT(current_object)->biting_head_view.saved_threshold;
			OBJECT(current_object)->biting_head_view.base_speed = OBJECT(current_object)->biting_head_view.saved_base_speed;
		}
		slope_factor = 0u;
		if ((OBJECT(current_object)->floor_slope | OBJECT(current_object)->floor_type) != 0u)
		{
			floor_slope = OBJECT(current_object)->floor_slope;
			slope_factor = floor_slope_factors[(uint16_t)(floor_slope * 2u)];
		}
		compute_camera_slope_speed(0u, slope_factor);
		move_speed = (uint16_t)((uint32_t)camera_slope_speed_int * OBJECT(current_object)->biting_head_view.catch_up_factor) >> 8;
		if ((int16_t)move_speed <= OBJECT(current_object)->biting_head_view.base_speed)
		{
		biting_head_use_base_speed:
			move_speed = (uint16_t)OBJECT(current_object)->biting_head_view.base_speed;
		}
	}
	else
	{
		direction = OBJECT(current_object)->direction;
		switch (direction >> 8)
		{
		case 0:
		case 3:
			if (OBJECT(current_object)->screen_x <= OBJECT(current_object)->biting_head_view.screen_threshold)
				goto biting_head_use_base_speed;
			goto biting_head_catch_up;
		case 1:
		case 2:
		case 4:
			if (OBJECT(current_object)->biting_head_view.screen_threshold <= OBJECT(current_object)->screen_x)
				goto biting_head_use_base_speed;
			goto biting_head_catch_up;
		default:
			unknown_code(0x8ACAAu + (direction >> 8) * 2u);
		}
	}
	chaser_speed = move_speed;
	if (OBJECT(current_object)->biting_head_view.wait_timer <= 0)
	{
		direction = OBJECT(current_object)->direction;
		switch (direction >> 8)
		{
		case 0:
			OBJECT(current_object)->y -= (uint32_t)chaser_speed << 12;
			break;
		case 1:
			OBJECT(current_object)->x += (uint32_t)chaser_speed << 12;
			break;
		case 2:
		case 4:
			OBJECT(current_object)->y += (uint32_t)chaser_speed << 12;
			break;
		case 3:
			OBJECT(current_object)->x -= (uint32_t)chaser_speed << 12;
			break;
		default:
			unknown_code(0x8AD42u + (direction >> 8) * 2u);
		}
	}
	if (OBJECT(current_object)->biting_head_view.holding_back == 0u && (uint16_t)chaser_speed != 0u)
	{
		animation_timer = OBJECT(current_object)->animation_timer;
		OBJECT(current_object)->animation_timer = animation_timer - 1u;
		if ((int8_t)animation_timer <= 1)
		{
			OBJECT(current_object)->animation_timer = 2;
			next_frame = OBJECT(current_object)->animation_frame;
			next_frame = (uint8_t)(next_frame + 1u);
			move_frames = biting_head_move_frames_ADDRESS;
			if ((int8_t)biting_head_frames < (int8_t)next_frame)
				next_frame = 0u;
			OBJECT(current_object)->animation_frame = next_frame;
			move_frame = ROM16(move_frames + next_frame * 2u);
			mem_write16(OBJECT(current_object)->part_list + 2u, move_frame);
			mem_write16(OBJECT(current_object)->part_list + 0x22u, move_frame);
		}
		OBJECT(current_object)->biting_head_view.bob_counter++;
		bob_phase = OBJECT(current_object)->biting_head_view.bob_counter & 3u;
		OBJECT(current_object)->biting_head_view.bob_counter = bob_phase;
		if (bob_phase == 0u)
			OBJECT(current_object)->screen_y--;
	}
	bite_timer = OBJECT(current_object)->biting_head_view.bite_timer;
	OBJECT(current_object)->biting_head_view.bite_timer = bite_timer - 1u;
	if ((int8_t)bite_timer <= 1)
	{
		next_bite_step = OBJECT(current_object)->biting_head_view.bite_step;
		next_bite_step = (uint8_t)(next_bite_step + 1u);
		bite_record = biting_head_bite_step_records_ADDRESS;
		if ((int8_t)biting_head_bite_steps < (int8_t)next_bite_step)
			next_bite_step = 0u;
		OBJECT(current_object)->biting_head_view.bite_step = next_bite_step;
		bite_record += next_bite_step * 4u;
		mem_write16(OBJECT(current_object)->part_list + 0x1Au, ROM16(bite_record));
		OBJECT(current_object)->biting_head_view.bite_timer = ROM8(bite_record + 2u);
		if (OBJECT(current_object)->biting_head_view.bite_step == 5u)
			OBJECT(current_object)->biting_head_view.bounce_index = 0;
	}
	if (OBJECT(current_object)->biting_head_view.bounce_index >= 0)
	{
		OBJECT(current_object)->biting_head_view.bounce_index++;
		bounce_dy = (uint16_t)biting_head_bounce[(uint8_t)OBJECT(current_object)->biting_head_view.bounce_index];
		if ((uint8_t)bounce_dy != 0x7Fu)
		{
		biting_head_apply_bounce:
			OBJECT(current_object)->screen_y += bounce_dy;
		}
		else
		{
			OBJECT(current_object)->biting_head_view.bounce_index = -1;
			if ((uint8_t)bounce_dy != 0x7Fu)
				goto biting_head_apply_bounce;
		}
	}
	if (OBJECT(current_object)->x_int <= -0x355)
	{
		OBJECT(LINKED(current_object, OBJECT(current_object)->child))->hitbox_height = 0x50u;
		OBJECT(current_object)->hitbox_height = 0x50u;
	}
	if (OBJECT(current_object)->x_int <= -0x431)
		biting_head_done = 0xFFu;
	if (scroll_position >= 0x1500u)
	{
		depth = (uint16_t)(OBJECT(current_object)->screen_y - 0x50u);
		queue_sprite_at_depth(&ignored, &depth);
		return;
	}
	point_x = 0x8210000u;
	point_y = 0x5EC0000u;
	z = 0x1D80000u;
	project_point(z, &ignored, &point_x, &point_y);
	mem_write16(OBJECT(current_object)->part_list + 0x48u, point_x);
	depth = (uint16_t)(OBJECT(current_object)->screen_y - 0x50u);
	biting_head_depth = depth;
	current_object = OBJECT(current_object)->part_list;
	current_object = current_object + 0x54u;
	queue_sprite_at_depth(&ignored, &depth);
	current_object = object_cursor;
	depth = biting_head_depth;
	queue_sprite_at_depth(&ignored, &depth);
	current_object = OBJECT(current_object)->part_list;
	depth = biting_head_depth;
	current_object = current_object + 0x40u;
	queue_sprite_at_depth(&ignored, &depth);
}

void fire_front_init(void)
{
	uint16_t flame_offset_x = 0;
	uint32_t flames_left = 0, ignored = 0, object = 0, path = 0;

	path = fire_front_paths_ADDRESS + fire_front_paths[OBJECT(current_object)->sprite_frame];
	OBJECT(current_object)->x_int = ROM16(path);
	OBJECT(current_object)->x_fraction = 0u;
	path += 2u;
	OBJECT(current_object)->y_int = ROM16(path);
	OBJECT(current_object)->y_fraction = 0u;
	path += 2u;
	OBJECT(current_object)->floor_z_int = ROM16(path);
	OBJECT(current_object)->floor_z_fraction = 0u;
	OBJECT(current_object)->z = OBJECT(current_object)->floor_z;
	path += 2u;
	OBJECT(current_object)->direction = ROM16(path);
	OBJECT(current_object)->fire_front.path = path + 2u;
	OBJECT(current_object)->flags = 0x40000000u;
	OBJECT(current_object)->fire_front.holding_back = 0u;
	OBJECT(current_object)->sprite_offset_x = 0;
	OBJECT(current_object)->sprite_offset_y = 0;
	OBJECT(current_object)->kind = 0x14u;
	OBJECT(current_object)->fire_front.wait_timer = 1;
	OBJECT(current_object)->hitbox_height = 0xF0u;
	OBJECT(current_object)->handler = fire_front_chase_ADDRESS;
	OBJECT(current_object)->hitbox_size_x = 0x132u;
	OBJECT(current_object)->hitbox_size_y = 0x200u;
	OBJECT(current_object)->hitbox_offset_x = -0x40;
	OBJECT(current_object)->hitbox_offset_y = 0;
	OBJECT(current_object)->speed_x = 0;
	OBJECT(current_object)->speed_y = 0;
	chaser_object = current_object;
	if (free_object_stack < 3)
		return;
	flames_left = 4u;
	flame_offset_x = 0x28u;
	do
	{
		allocate_object(&ignored, &object);
		OBJECT(object)->handler = spreading_flame_start_ADDRESS;
		OBJECT(object)->x = OBJECT(current_object)->x;
		OBJECT(object)->y = OBJECT(current_object)->y;
		OBJECT(object)->x_int += flame_offset_x;
		flame_offset_x += 0x38u;
		flames_left--;
	} while (flames_left != 0u);
}

void fire_front_chase(void)
{
	uint16_t direction = 0, speed = 0, threshold = 0, trigger_coordinate = 0;
	uint32_t cursor = 0, direction_2 = 0, ignored = 0, step = 0;
	uint8_t hold_timer = 0;
	uint16_t wait_timer = 0, trigger_scroll = 0, hold_threshold = 0;
	uint32_t flags = 0;

	flags = OBJECT(current_object)->flags;
	OBJECT(current_object)->flags = flags | 0x80000000u;
	if ((flags & 0x80000000u) == 0u)
	{
	fire_front_read_script:
		wait_timer = OBJECT(current_object)->fire_front.wait_timer;
		OBJECT(current_object)->fire_front.wait_timer = wait_timer - 1u;
		if ((int16_t)wait_timer <= 1)
		{
			OBJECT(current_object)->fire_front.wait_timer = 0;
			cursor = OBJECT(current_object)->fire_front.path;
			trigger_scroll = mem_read16(cursor);
			cursor += 2u;
			if (trigger_scroll <= scroll_position)
			{
				cursor += 2u;
				goto fire_front_load_record;
			}
			trigger_coordinate = mem_read16(cursor);
			cursor += 2u;
			if (trigger_coordinate != 0u)
			{
				direction_2 = OBJECT(current_object)->direction;
				switch (direction_2 >> 8)
				{
				case 0:
				case 4:
					if ((int16_t)(OBJECT(current_object)->y_int + OBJECT(current_object)->hitbox_offset_y) <= (int16_t)trigger_coordinate)
					{
					fire_front_load_record:
						if ((int16_t)mem_read16(cursor + 2u) < 0)
						{
							chaser_object = 0u;
							delete_current_object();
							return;
						}
						OBJECT(current_object)->fire_front.screen_threshold = mem_read16(cursor);
						cursor += 2u;
						OBJECT(current_object)->fire_front.catch_up_factor = mem_read16(cursor);
						cursor += 2u;
						OBJECT(current_object)->fire_front.base_speed = mem_read16(cursor);
						cursor += 2u;
						OBJECT(current_object)->fire_front.wait_timer = mem_read16(cursor);
						cursor += 2u;
						if ((uint16_t)OBJECT(current_object)->fire_front.wait_timer != 0u)
						{
							OBJECT(current_object)->speed_x = 0;
							OBJECT(current_object)->speed_y = 0;
						}
						OBJECT(current_object)->fire_front.path = cursor;
					}
					break;
				case 1:
					if ((int16_t)trigger_coordinate <= OBJECT(current_object)->x_int)
						goto fire_front_load_record;
					break;
				case 2:
					if ((int16_t)trigger_coordinate <= OBJECT(current_object)->y_int)
						goto fire_front_load_record;
					break;
				case 3:
					if (OBJECT(current_object)->x_int <= (int16_t)trigger_coordinate)
						goto fire_front_load_record;
					break;
				default:
					unknown_code(0x8B05Du + (direction_2 >> 8) * 2u);
				}
			}
		}
	}
	else if (OBJECT(current_object)->fire_front.holding_back == 0u)
	{
		if (no_player_in_play == 0u && player_mishap == 0u && player_dropping_in == 0u)
			goto fire_front_read_script;
		OBJECT(current_object)->fire_front.holding_back = 0xFFu;
		OBJECT(current_object)->fire_front.saved_base_speed = OBJECT(current_object)->fire_front.base_speed;
		OBJECT(current_object)->fire_front.base_speed = 0;
		OBJECT(current_object)->speed_x = 0;
		OBJECT(current_object)->speed_y = 0;
		OBJECT(current_object)->fire_front.hold_start_position = scroll_position;
		OBJECT(current_object)->fire_front.hold_timer = 0xFFu;
		OBJECT(current_object)->fire_front.saved_threshold = OBJECT(current_object)->fire_front.screen_threshold;
		direction = OBJECT(current_object)->direction;
		hold_threshold = direction ^ (direction >> 1);
		threshold = hold_threshold;
		if (hold_threshold == 0u)
			threshold = 0x160u;
		OBJECT(current_object)->fire_front.screen_threshold = threshold;
	}
	project_object(&ignored, &ignored);
	add_to_zone_list(ignored);
	if (OBJECT(current_object)->fire_front.holding_back != 0u)
	{
		if ((int16_t)(scroll_position - OBJECT(current_object)->fire_front.hold_start_position) >= 0x300)
			goto fire_front_stop_holding_back;
		if (game_mode != 3u)
			OBJECT(current_object)->fire_front.hold_timer = 0xFFu;
		hold_timer = OBJECT(current_object)->fire_front.hold_timer;
		OBJECT(current_object)->fire_front.hold_timer = hold_timer - 1u;
		if (hold_timer == 0u)
			goto fire_front_stop_holding_back;
	}
	switch (OBJECT(current_object)->direction >> 8)
	{
	case 0:
	case 3:
		if (OBJECT(current_object)->screen_x <= OBJECT(current_object)->fire_front.screen_threshold)
		{
		fire_front_use_base_speed:
			speed = (uint16_t)OBJECT(current_object)->fire_front.base_speed;
		}
		else
		{
		fire_front_catch_up:
			OBJECT(current_object)->fire_front.wait_timer = 0;
			if (OBJECT(current_object)->fire_front.holding_back != 0u)
			{
			fire_front_stop_holding_back:
				OBJECT(current_object)->fire_front.holding_back = 0u;
				OBJECT(current_object)->fire_front.screen_threshold = OBJECT(current_object)->fire_front.saved_threshold;
				OBJECT(current_object)->fire_front.base_speed = OBJECT(current_object)->fire_front.saved_base_speed;
			}
			compute_camera_ground_speed(0u);
			speed = (uint16_t)((uint32_t)camera_ground_speed_int * OBJECT(current_object)->fire_front.catch_up_factor) >> 8;
			if ((int16_t)speed <= OBJECT(current_object)->fire_front.base_speed)
				goto fire_front_use_base_speed;
		}
		break;
	case 1:
	case 2:
	case 4:
		if (OBJECT(current_object)->fire_front.screen_threshold <= OBJECT(current_object)->screen_x)
			goto fire_front_use_base_speed;
		goto fire_front_catch_up;
	default:
		unknown_code(0x8B117u + (OBJECT(current_object)->direction >> 8) * 2u);
	}
	chaser_speed = speed;
	if (OBJECT(current_object)->fire_front.wait_timer <= 0)
	{
		switch (OBJECT(current_object)->direction >> 8)
		{
		case 0:
			step = (uint32_t)chaser_speed << 12;
			OBJECT(current_object)->y -= step;
			OBJECT(current_object)->speed_y = 0u - step;
			OBJECT(current_object)->speed_x = 0;
			break;
		case 1:
			step = (uint32_t)chaser_speed << 12;
			OBJECT(current_object)->x += step;
			OBJECT(current_object)->speed_x = step;
			OBJECT(current_object)->speed_y = 0;
			break;
		case 2:
		case 4:
			step = (uint32_t)chaser_speed << 12;
			OBJECT(current_object)->y += step;
			OBJECT(current_object)->speed_y = step;
			OBJECT(current_object)->speed_x = 0;
			break;
		case 3:
			step = (uint32_t)chaser_speed << 12;
			OBJECT(current_object)->x -= step;
			OBJECT(current_object)->speed_x = 0u - step;
			OBJECT(current_object)->speed_y = 0;
			break;
		default:
			unknown_code(0x8B193u + (OBJECT(current_object)->direction >> 8) * 2u);
		}
	}
	if (mem_read16(OBJECT(current_object)->fire_front.path - 6u) == 0u)
	{
		OBJECT(current_object)->hitbox_size_x = 0xF2u;
		OBJECT(current_object)->hitbox_offset_x = 0;
		chaser_stopped = 0xFFu;
	}
}

void falling_grate_update(void)
{
	uint8_t sound_id = 0;
	uint32_t r1 = 0, r7 = 0, at_last_step = 0, depth = 0, ignored = 0, write_index_address = 0;
	uint8_t animation_timer = 0;

	switch (OBJECT(current_object)->falling_grate.state)
	{
	case 0:
	case 3:
		OBJECT(current_object)->falling_grate.state++;
		OBJECT(current_object)->falling_grate.rattle_step = 1;
		project_object(&ignored, &r1);
	case 1:
		update_screen_position(&ignored, &r1);
		current_player = current_object;
		current_player = LINKED(current_player, OBJECT(current_object)->parent);
		switch (OBJECT(current_object)->direction)
		{
		case 0:
		case 4:
			if (OBJECT(current_object)->y_int <= OBJECT(current_player)->y_int)
			{
				OBJECT(current_object)->falling_grate.state++;
				OBJECT(current_object)->speed_z = 0;
				OBJECT(current_object)->animation_speed = 6u;
				OBJECT(current_object)->animation_timer = 0;
				OBJECT(current_object)->animation_frame = 0xFFu;
				OBJECT(current_object)->animation = falling_grate_animation_ADDRESS;
				sound_id = 0xDu;
				play_sound(sound_id, &r1, &write_index_address);
			}
			else if (OBJECT(current_object)->y_int <= (int16_t)(OBJECT(current_player)->y_int + 0x60u))
			{
				animation_timer = OBJECT(current_object)->animation_timer;
				OBJECT(current_object)->animation_timer = animation_timer - 1u;
				if ((int8_t)animation_timer <= 1)
				{
					OBJECT(current_object)->animation_timer = OBJECT(current_object)->animation_speed;
					OBJECT(current_object)->floor_z_int += OBJECT(current_object)->falling_grate.rattle_step;
					OBJECT(current_object)->z_int += OBJECT(current_object)->falling_grate.rattle_step;
					OBJECT(current_object)->falling_grate.rattle_step = 0u - OBJECT(current_object)->falling_grate.rattle_step;
				}
			}
			break;
		case 1:
			if (OBJECT(current_player)->y_int <= OBJECT(current_object)->y_int)
			{
				OBJECT(current_object)->falling_grate.state++;
				OBJECT(current_object)->speed_z = 0;
				OBJECT(current_object)->animation_speed = 6u;
				OBJECT(current_object)->animation_timer = 0;
				OBJECT(current_object)->animation_frame = 0xFFu;
				OBJECT(current_object)->animation = falling_grate_animation_ADDRESS;
				sound_id = 0xDu;
				play_sound(sound_id, &r1, &write_index_address);
				add_to_platform_list();
				if ((uint16_t)(OBJECT(current_object)->screen_x + 0xA0u) >= 0x281u)
					return;
				depth = (uint16_t)(OBJECT(current_object)->screen_y + OBJECT(current_object)->falling_grate.depth_offset - OBJECT(current_object)->sprite_offset_y);
				OBJECT(current_object)->falling_grate.draw_depth = depth;
				queue_sprite_at_depth(&ignored, &depth);
				return;
			}
			if ((int16_t)(OBJECT(current_player)->y_int - 0x60u) <= OBJECT(current_object)->y_int)
			{
				animation_timer = OBJECT(current_object)->animation_timer;
				OBJECT(current_object)->animation_timer = animation_timer - 1u;
				if ((int8_t)animation_timer <= 1)
				{
					OBJECT(current_object)->animation_timer = OBJECT(current_object)->animation_speed;
					OBJECT(current_object)->floor_z_int += OBJECT(current_object)->falling_grate.rattle_step;
					OBJECT(current_object)->z_int += OBJECT(current_object)->falling_grate.rattle_step;
					OBJECT(current_object)->falling_grate.rattle_step = 0u - OBJECT(current_object)->falling_grate.rattle_step;
				}
				add_to_platform_list();
				if ((uint16_t)(OBJECT(current_object)->screen_x + 0xA0u) >= 0x281u)
					return;
				depth = (uint16_t)(OBJECT(current_object)->screen_y + OBJECT(current_object)->falling_grate.depth_offset - OBJECT(current_object)->sprite_offset_y);
				OBJECT(current_object)->falling_grate.draw_depth = depth;
				queue_sprite_at_depth(&ignored, &depth);
				return;
			}
			break;
		case 2:
			if (OBJECT(current_object)->x_int <= OBJECT(current_player)->x_int)
			{
				OBJECT(current_object)->falling_grate.state++;
				OBJECT(current_object)->speed_z = 0;
				OBJECT(current_object)->animation_speed = 6u;
				OBJECT(current_object)->animation_timer = 0;
				OBJECT(current_object)->animation_frame = 0xFFu;
				OBJECT(current_object)->animation = falling_grate_animation_ADDRESS;
				sound_id = 0xDu;
				play_sound(sound_id, &r1, &write_index_address);
				add_to_platform_list();
				if ((uint16_t)(OBJECT(current_object)->screen_x + 0xA0u) >= 0x281u)
					return;
				depth = (uint16_t)(OBJECT(current_object)->screen_y + OBJECT(current_object)->falling_grate.depth_offset - OBJECT(current_object)->sprite_offset_y);
				OBJECT(current_object)->falling_grate.draw_depth = depth;
				queue_sprite_at_depth(&ignored, &depth);
				return;
			}
			if (OBJECT(current_object)->x_int <= (int16_t)(OBJECT(current_player)->x_int + 0x60u))
			{
				animation_timer = OBJECT(current_object)->animation_timer;
				OBJECT(current_object)->animation_timer = animation_timer - 1u;
				if ((int8_t)animation_timer <= 1)
				{
					OBJECT(current_object)->animation_timer = OBJECT(current_object)->animation_speed;
					OBJECT(current_object)->floor_z_int += OBJECT(current_object)->falling_grate.rattle_step;
					OBJECT(current_object)->z_int += OBJECT(current_object)->falling_grate.rattle_step;
					OBJECT(current_object)->falling_grate.rattle_step = 0u - OBJECT(current_object)->falling_grate.rattle_step;
				}
				add_to_platform_list();
				if ((uint16_t)(OBJECT(current_object)->screen_x + 0xA0u) >= 0x281u)
					return;
				depth = (uint16_t)(OBJECT(current_object)->screen_y + OBJECT(current_object)->falling_grate.depth_offset - OBJECT(current_object)->sprite_offset_y);
				OBJECT(current_object)->falling_grate.draw_depth = depth;
				queue_sprite_at_depth(&ignored, &depth);
				return;
			}
			break;
		case 3:
			if (OBJECT(current_player)->x_int <= OBJECT(current_object)->x_int)
			{
				OBJECT(current_object)->falling_grate.state++;
				OBJECT(current_object)->speed_z = 0;
				OBJECT(current_object)->animation_speed = 6u;
				OBJECT(current_object)->animation_timer = 0;
				OBJECT(current_object)->animation_frame = 0xFFu;
				OBJECT(current_object)->animation = falling_grate_animation_ADDRESS;
				sound_id = 0xDu;
				play_sound(sound_id, &r1, &write_index_address);
				add_to_platform_list();
				if ((uint16_t)(OBJECT(current_object)->screen_x + 0xA0u) >= 0x281u)
					return;
				depth = (uint16_t)(OBJECT(current_object)->screen_y + OBJECT(current_object)->falling_grate.depth_offset - OBJECT(current_object)->sprite_offset_y);
				OBJECT(current_object)->falling_grate.draw_depth = depth;
				queue_sprite_at_depth(&ignored, &depth);
				return;
			}
			if ((int16_t)(OBJECT(current_player)->x_int - 0x60u) <= OBJECT(current_object)->x_int)
			{
				animation_timer = OBJECT(current_object)->animation_timer;
				OBJECT(current_object)->animation_timer = animation_timer - 1u;
				if ((int8_t)animation_timer <= 1)
				{
					OBJECT(current_object)->animation_timer = OBJECT(current_object)->animation_speed;
					OBJECT(current_object)->floor_z_int += OBJECT(current_object)->falling_grate.rattle_step;
					OBJECT(current_object)->z_int += OBJECT(current_object)->falling_grate.rattle_step;
					OBJECT(current_object)->falling_grate.rattle_step = 0u - OBJECT(current_object)->falling_grate.rattle_step;
				}
				add_to_platform_list();
				if ((uint16_t)(OBJECT(current_object)->screen_x + 0xA0u) >= 0x281u)
					return;
				depth = (uint16_t)(OBJECT(current_object)->screen_y + OBJECT(current_object)->falling_grate.depth_offset - OBJECT(current_object)->sprite_offset_y);
				OBJECT(current_object)->falling_grate.draw_depth = depth;
				queue_sprite_at_depth(&ignored, &depth);
				return;
			}
			break;
		default:
			unknown_code(0x8B256u + OBJECT(current_object)->direction * 2u);
		}
		add_to_platform_list();
		if ((uint16_t)(OBJECT(current_object)->screen_x + 0xA0u) >= 0x281u)
			return;
		depth = (uint16_t)(OBJECT(current_object)->screen_y + OBJECT(current_object)->falling_grate.depth_offset - OBJECT(current_object)->sprite_offset_y);
		OBJECT(current_object)->falling_grate.draw_depth = depth;
		queue_sprite_at_depth(&ignored, &depth);
		return;
	case 2:
		OBJECT(current_object)->speed_z += 0x2000u;
		OBJECT(current_object)->z += OBJECT(current_object)->speed_z;
		project_object(&ignored, &r1);
		if (OBJECT(current_object)->animation_timer >= 0)
		{
			animate(&r1, &r7, &at_last_step);
			if ((int8_t)at_last_step < 0)
				OBJECT(current_object)->animation_timer = -1;
		}
		if (OBJECT(current_object)->screen_y > 0x1C0)
		{
			delete_current_object();
			return;
		}
		if ((int8_t)OBJECT(current_object)->animation_frame <= 2)
			add_to_platform_list();
		depth = (uint16_t)(((int32_t)((uint32_t)OBJECT(current_object)->projected_y + OBJECT(current_object)->floor_z) >> 16) - view_y + OBJECT(current_object)->falling_grate.depth_offset);
		queue_sprite_at_depth(&ignored, &depth);
		return;
	default:
		unknown_code(0x8B224u + OBJECT(current_object)->falling_grate.state * 3u);
	}
}

void rising_saw_init(void)
{
	uint32_t block_offset = 0, ignored = 0, object = 0, object_2 = 0, path = 0, x = 0, y = 0;
	uint8_t blades_left = 0;

	OBJECT(current_object)->handler = rising_saw_init_ADDRESS;
	if (scroll_position < 0x3780u && biting_head_done == 0u)
		return;
	path = rising_saw_paths[OBJECT(current_object)->sprite_frame] + rising_saw_paths_ADDRESS;
	OBJECT(current_object)->x_int = ROM16(path);
	OBJECT(current_object)->x_fraction = 0u;
	path += 2u;
	OBJECT(current_object)->y_int = ROM16(path);
	OBJECT(current_object)->y_fraction = 0u;
	path += 2u;
	OBJECT(current_object)->z_int = ROM16(path);
	OBJECT(current_object)->z_fraction = 0u;
	OBJECT(current_object)->rising_saw.path = path + 2u;
	rising_saws[OBJECT(current_object)->sprite_frame] = current_object;
	find_floor(&x, &y, &block_offset, &ignored, &object_2);
	OBJECT(current_object)->previous_floor_z_int = OBJECT(current_object)->z_int;
	project_object(&ignored, &ignored);
	OBJECT(current_object)->rising_saw.projected_x_whole = OBJECT(current_object)->projected_x;
	OBJECT(current_object)->spawn_arg_2 = 0u;
	OBJECT(current_object)->flags = 2u;
	OBJECT(current_object)->hitbox_offset_x = -0x10;
	OBJECT(current_object)->hitbox_offset_y = -0x20;
	OBJECT(current_object)->hitbox_size_x = 0x50u;
	OBJECT(current_object)->hitbox_size_y = 0x40u;
	OBJECT(current_object)->hitbox_height = 0x20u;
	OBJECT(current_object)->kind = 0x26u;
	OBJECT(current_object)->direction = 0x200u;
	OBJECT(current_object)->rising_saw.holding_back = 0u;
	OBJECT(current_object)->rising_saw.wait_timer = 0;
	if (free_object_stack < 4)
		return;
	saw_blade_offset_x = 8u;
	saw_blades_left = 1;
	do
	{
		allocate_object(&ignored, &object_2);
		OBJECT(object_2)->sprite_flags = 0x1002u;
		OBJECT(object_2)->sprite_frame = 0x7F98u;
		OBJECT(object_2)->sprite_palette = 0x1190u;
		OBJECT(object_2)->sprite_offset_x = saw_blade_offset_x;
		saw_blade_offset_x += 0x10u;
		OBJECT(object_2)->sprite_offset_y = 0;
		OBJECT(object_2)->flags = 0u;
		AS16(OBJECT(object_2)->var_80) = OBJECT(current_object)->z_int;
		AS16(OBJECT(object_2)->var_80) += 0x1Du;
		AT8(OBJECT(object_2)->var_82, 1) = 0u;
		OBJECT(object_2)->var_84 = 0u;
		OBJECT(object_2)->handler = saw_blade_update_ADDRESS;
		OBJECT(object_2)->animation = saw_blade_animation_ADDRESS;
		OBJECT(object_2)->animation_frame = 0u;
		OBJECT(object_2)->animation_speed = 1u;
		OBJECT(object_2)->animation_timer = 0;
		OBJECT(object_2)->handler = saw_blade_update_ADDRESS;
		OBJECT(object_2)->parent = current_object;
		object = object_2;
		allocate_object(&ignored, &object_2);
		OBJECT(object_2)->sprite_flags = 0x100Au;
		OBJECT(object_2)->sprite_frame = 0x8378u;
		OBJECT(object_2)->sprite_palette = 0x1200u;
		OBJECT(object_2)->sprite_offset_x = -4;
		OBJECT(object_2)->sprite_offset_y = -0x30;
		OBJECT(object_2)->flags = 0u;
		OBJECT(object_2)->handler = saw_sparks_update_ADDRESS;
		OBJECT(object_2)->animation = saw_sparks_animation_ADDRESS;
		OBJECT(object_2)->animation_frame = 0u;
		OBJECT(object_2)->animation_speed = 2u;
		OBJECT(object_2)->animation_timer = 0;
		OBJECT(object_2)->handler = saw_sparks_update_ADDRESS;
		OBJECT(object_2)->parent = object;
		OBJECT(object)->child = object_2;
		blades_left = saw_blades_left;
		saw_blades_left = blades_left - 1u;
	} while ((int8_t)blades_left > 1);
	OBJECT(current_object)->handler = rising_saw_rise_ADDRESS;
}

void rising_saw_rise(void)
{
	uint16_t speed = 0;
	uint32_t attribute_record = 0, cursor = 0, point_x = 0, point_y = 0, projected_dx = 0, slope_step = 0;
	uint8_t hold_timer = 0;
	uint16_t wait_timer = 0;
	uint32_t flags = 0;

	flags = OBJECT(current_object)->flags;
	OBJECT(current_object)->flags = flags | 0x80000000u;
	if ((flags & 0x80000000u) == 0u)
	{
	rising_saw_read_script:
		if ((uint16_t)OBJECT(current_object)->rising_saw.wait_timer == 0u)
		{
			cursor = OBJECT(current_object)->rising_saw.path;
			if (mem_read16(cursor) <= scroll_position || OBJECT(current_object)->z_int <= (int16_t)mem_read16(cursor + 2u))
			{
				cursor += 4u;
				if ((int16_t)mem_read16(cursor + 2u) < 0)
				{
					delete_current_object();
					return;
				}
				OBJECT(current_object)->rising_saw.screen_threshold = mem_read16(cursor);
				cursor += 2u;
				OBJECT(current_object)->rising_saw.catch_up_factor = mem_read16(cursor);
				cursor += 2u;
				OBJECT(current_object)->rising_saw.base_speed = mem_read16(cursor);
				cursor += 2u;
				OBJECT(current_object)->rising_saw.wait_timer = mem_read16(cursor);
				OBJECT(current_object)->rising_saw.path = cursor + 2u;
			}
		}
	}
	else if (OBJECT(current_object)->rising_saw.holding_back == 0u)
	{
		if (no_player_in_play == 0u && player_mishap == 0u && player_dropping_in == 0u)
			goto rising_saw_read_script;
		OBJECT(current_object)->rising_saw.holding_back = 0xFFu;
		OBJECT(current_object)->rising_saw.saved_base_speed = OBJECT(current_object)->rising_saw.base_speed;
		OBJECT(current_object)->rising_saw.base_speed = 0;
		OBJECT(current_object)->rising_saw.hold_start_position = scroll_position;
		OBJECT(current_object)->rising_saw.hold_timer = 0xFFu;
		OBJECT(current_object)->rising_saw.saved_threshold = OBJECT(current_object)->rising_saw.screen_threshold;
		OBJECT(current_object)->rising_saw.screen_threshold = 0x130;
	}
	if (OBJECT(current_object)->rising_saw.holding_back != 0u)
	{
		if ((int16_t)(scroll_position - OBJECT(current_object)->rising_saw.hold_start_position) >= 0x300)
		{
		rising_saw_stop_holding_back:
			OBJECT(current_object)->rising_saw.holding_back = 0u;
			OBJECT(current_object)->rising_saw.screen_threshold = OBJECT(current_object)->rising_saw.saved_threshold;
			OBJECT(current_object)->rising_saw.base_speed = OBJECT(current_object)->rising_saw.saved_base_speed;
		}
		else
		{
			if (no_player_in_play != 0u)
				OBJECT(current_object)->rising_saw.hold_timer = 0xFFu;
			hold_timer = OBJECT(current_object)->rising_saw.hold_timer;
			OBJECT(current_object)->rising_saw.hold_timer = hold_timer - 1u;
			if (hold_timer == 0u)
				goto rising_saw_stop_holding_back;
		}
	}
	update_screen_position(&slope_step, &projected_dx);
	if (OBJECT(current_object)->screen_y <= OBJECT(current_object)->rising_saw.screen_threshold)
	{
	rising_saw_use_base_speed:
		speed = (uint16_t)OBJECT(current_object)->rising_saw.base_speed;
	}
	else
	{
		OBJECT(current_object)->rising_saw.wait_timer = 0;
		compute_camera_ground_speed(0u);
		speed = (uint16_t)((uint32_t)camera_ground_speed_int * OBJECT(current_object)->rising_saw.catch_up_factor) >> 8;
		if ((int16_t)speed <= OBJECT(current_object)->rising_saw.base_speed)
			goto rising_saw_use_base_speed;
	}
	OBJECT(current_object)->speed_fraction = speed;
	wait_timer = OBJECT(current_object)->rising_saw.wait_timer;
	OBJECT(current_object)->rising_saw.wait_timer = wait_timer - 1u;
	if ((int16_t)wait_timer > 1)
	{
		add_to_touchable_list(slope_step);
		return;
	}
	OBJECT(current_object)->rising_saw.wait_timer = 0;
	OBJECT(current_object)->z -= (uint32_t)(int16_t)OBJECT(current_object)->speed_fraction << 12;
	attribute_record = OBJECT(current_object)->floor_attribute + floor_types_ADDRESS;
	slope_step = ((uint32_t)(int16_t)ROM16(attribute_record + 4u) * (int16_t)OBJECT(current_object)->speed_fraction) >> 4;
	projected_dx = slope_step;
	if (ROM8(attribute_record) != 0u)
		projected_dx = 0u - projected_dx;
	OBJECT(current_object)->rising_saw.fixed_projected_x += projected_dx;
	OBJECT(current_object)->projected_x = OBJECT(current_object)->rising_saw.projected_x_whole;
	OBJECT(current_object)->projected_y -= slope_step >> 1;
	point_x = OBJECT(current_object)->rising_saw.fixed_projected_x;
	point_y = OBJECT(current_object)->projected_y * 2u;
	unproject_point(&point_x, &point_y);
	OBJECT(current_object)->x = point_x;
	OBJECT(current_object)->y = point_y;
	add_to_touchable_list(slope_step);
}

void saw_blade_update(void)
{
	uint8_t sound_id = 0;
	uint16_t child = 0, object_4 = 0;
	uint32_t r1 = 0, r2 = 0, r7 = 0, r8 = 0, clip_x = 0, clips_left = 0, depth = 0, free_count = 0, ignored = 0, object = 0, object_2 = 0, object_3 = 0, object_5 = 0, point_y = 0, z = 0;
	uint16_t sound_timer = 0;

	switch (OBJECT(current_object)->saw_blade.state)
	{
	case 0:
	case 4:
		OBJECT(current_object)->saw_blade.state++;
		object = LINKED(current_object, OBJECT(current_object)->parent);
		OBJECT(current_object)->x = OBJECT(object)->x;
		OBJECT(current_object)->y = OBJECT(object)->y;
		OBJECT(current_object)->y_int -= 0x30u;
		OBJECT(current_object)->z = OBJECT(object)->z;
		clips_left = 2u;
		object_2 = current_object;
		do
		{
			object_3 = object_2;
			allocate_object_2(&ignored, &r2, &object_2);
			object_5 = object_2 + 2u;
			OBJECT(object_5 - 0x2u)->sprite_flags = 0x5000u;
			object_5 += 2u;
			OBJECT(object_5 - 0x4u)->sprite_frame = 0xE0u;
			object_5 += 2u;
			OBJECT(object_5 - 0x6u)->sprite_palette = 0u;
			object_5 += 2u;
			OBJECT(object_5 - 0x8u)->sprite_scale = 0x140u;
			object_5 += 2u;
			OBJECT(object_5 - 0xAu)->sprite_layer = 0u;
			object_5 += 2u;
			OBJECT(object_5 - 0xCu)->screen_y = 0;
			object_5 += 2u;
			OBJECT(object_5 - 0xEu)->screen_x = 0;
			OBJECT(object_5 - 0xEu)->hitbox_size_x = 0u;
			OBJECT(object_2)->parent = object_3;
			OBJECT(object_3)->child = object_2;
			clips_left--;
		} while (clips_left != 0u);
		sound_id = 0xAu;
		play_sound(sound_id, &r1, &r2);
		return;
	case 1:
		OBJECT(current_object)->y += 0x20000u;
		object = LINKED(current_object, OBJECT(current_object)->parent);
		if (OBJECT(object)->y_int <= OBJECT(current_object)->y_int)
		{
			OBJECT(current_object)->saw_blade.state++;
			OBJECT(current_object)->y = OBJECT(object)->y;
		}
		animate(&r1, &r7, &r8);
		project_object(&ignored, &ignored);
		OBJECT(current_object)->saw_blade.draw_depth = OBJECT(current_object)->screen_y;
		depth = OBJECT(current_object)->saw_blade.draw_depth;
		current_object = LINKED(current_object, OBJECT(current_object)->child);
		current_object = LINKED(current_object, OBJECT(current_object)->child);
		queue_sprite_at_depth(&ignored, &depth);
		current_object = object_cursor;
		depth = OBJECT(current_object)->saw_blade.draw_depth;
		queue_sprite_at_depth(&ignored, &depth);
		clip_x = OBJECT(current_object)->x;
		point_y = OBJECT(LINKED(current_object, OBJECT(current_object)->parent))->y;
		z = OBJECT(current_object)->z;
		project_point(z, &ignored, &clip_x, &point_y);
		clip_x = (uint16_t)(clip_x + OBJECT(current_object)->sprite_offset_x);
		current_object = LINKED(current_object, OBJECT(current_object)->child);
		OBJECT(current_object)->sprite_palette = clip_x;
		depth = OBJECT(object_cursor)->saw_blade.draw_depth;
		queue_sprite_at_depth(&ignored, &depth);
		return;
	case 2:
		OBJECT(current_object)->saw_blade.state++;
		current_object = LINKED(current_object, OBJECT(current_object)->child);
		child = OBJECT(current_object)->child;
		free_object_2(child);
		object_4 = (uint16_t)current_object;
		free_object_2(object_4);
		current_object = object_cursor;
	case 3:
		object_3 = LINKED(current_object, OBJECT(current_object)->parent);
		OBJECT(current_object)->x = OBJECT(object_3)->x;
		OBJECT(current_object)->y = OBJECT(object_3)->y;
		OBJECT(current_object)->z = OBJECT(object_3)->z;
		project_object(&ignored, &r1);
		if ((int16_t)(OBJECT(current_object)->saw_blade.next_shaft_z - OBJECT(current_object)->z_int) >= 0x1D)
		{
			OBJECT(current_object)->saw_blade.next_shaft_z -= 0x1Du;
			allocate_object(&free_count, &object_2);
			if (free_count != 0u)
			{
				OBJECT(object_2)->sprite_flags = 0x1004u;
				OBJECT(object_2)->sprite_frame = 0x7FC0u;
				OBJECT(object_2)->sprite_palette = 0x11A0u;
				OBJECT(object_2)->sprite_offset_x = 8;
				OBJECT(object_2)->sprite_offset_y = -0x1C;
				OBJECT(object_2)->flags = 0u;
				OBJECT(object_2)->saw_shaft_update.state = 0u;
				OBJECT(object_2)->animation = saw_shaft_animation_ADDRESS;
				OBJECT(object_2)->animation_frame = 1u;
				OBJECT(object_2)->projected_x = OBJECT(current_object)->projected_x;
				OBJECT(object_2)->projected_y = OBJECT(current_object)->projected_y;
				OBJECT(object_2)->z = OBJECT(current_object)->z;
				OBJECT(object_2)->saw_shaft_update.next_step_z = OBJECT(object_2)->z_int;
				OBJECT(object_2)->handler = saw_shaft_update_ADDRESS;
				OBJECT(object_2)->parent = current_object;
			}
		}
		animate(&r1, &r7, &r8);
		sound_timer = OBJECT(current_object)->saw_blade.sound_timer;
		OBJECT(current_object)->saw_blade.sound_timer = sound_timer - 1u;
		if ((int16_t)sound_timer <= 1)
		{
			OBJECT(current_object)->saw_blade.sound_timer = 0x3C;
			sound_id = 8u;
			play_sound(sound_id, &r1, &r2);
		}
		if (OBJECT(current_object)->screen_y >= 0x140)
			return;
		OBJECT(current_object)->saw_blade.draw_depth = OBJECT(current_object)->screen_y;
		r1 = queue_sprite(r1);
		return;
	default:
		unknown_code(0x8B6FBu + OBJECT(current_object)->saw_blade.state * 3u);
	}
}

void saw_sparks_update(void)
{
	uint32_t r7 = 0, r8 = 0, depth = 0, frame = 0, ignored = 0, object = 0;

	object = LINKED(current_object, OBJECT(current_object)->parent);
	OBJECT(current_object)->screen_x = OBJECT(object)->screen_x;
	OBJECT(current_object)->screen_y = OBJECT(object)->screen_y;
	OBJECT(current_object)->screen_x += OBJECT(current_object)->sprite_offset_x;
	OBJECT(current_object)->screen_y += OBJECT(current_object)->sprite_offset_y;
	animate(&frame, &r7, &r8);
	if (OBJECT(current_object)->screen_y >= 0x168)
		return;
	object = LINKED(current_object, OBJECT(current_object)->parent);
	if (OBJECT(LINKED(object, OBJECT(object)->parent))->var_88 == 0u)
		return;
	depth = (uint16_t)(OBJECT(object)->var_88 + 1u);
	queue_sprite_at_depth(&ignored, &depth);
}

void saw_shaft_update(void)
{
	uint32_t r1 = 0, at_last_step = 0, depth = 0, ignored = 0, off_screen = 0, wrapped = 0;

	switch (OBJECT(current_object)->saw_shaft_update.state)
	{
	case 0:
	case 2:
		if ((int16_t)(OBJECT(current_object)->saw_shaft_update.next_step_z - OBJECT(LINKED(current_object, OBJECT(current_object)->parent))->z_int) >= 5)
		{
			OBJECT(current_object)->saw_shaft_update.next_step_z -= 5u;
			wrapped = 0u;
			animate_step(&r1, &wrapped, &at_last_step);
			if ((uint8_t)wrapped != 0u)
				OBJECT(current_object)->saw_shaft_update.state++;
		}
		update_screen_position(&ignored, &r1);
		depth = 0xFFE9u;
		queue_sprite_at_depth(&ignored, &depth);
		return;
	case 1:
		off_screen = is_off_screen();
		if (off_screen == 0u)
		{
			update_screen_position(&ignored, &r1);
			depth = 0xFFF7u;
			queue_sprite_at_depth(&ignored, &depth);
			return;
		}
		delete_current_object();
		return;
	default:
		unknown_code(0x8B957u + OBJECT(current_object)->saw_shaft_update.state * 2u);
	}
}

void grate_steps_init(void)
{
	uint32_t r0 = 0, r1 = 0, block_offset = 0, cursor = 0, ignored = 0;

	OBJECT(current_object)->x = -0x3BD0000;
	OBJECT(current_object)->y = 0x4F40000;
	OBJECT(current_object)->z = 0x80000;
	project_object(&r0, &r1);
	OBJECT(current_object)->grate_steps.row_projected_x = OBJECT(current_object)->projected_x;
	OBJECT(current_object)->spawn_arg_2 = 0u;
	OBJECT(current_object)->grate_steps.row_projected_y = OBJECT(current_object)->projected_y;
	find_floor(&r0, &r1, &block_offset, &ignored, &cursor);
	OBJECT(current_object)->grate_steps.rows_left = 0xF;
	rising_saws[0] = 0xFFFFu;
	rising_saw_1 = 0xFFFFu;
	rising_saw_2 = 0xFFFFu;
	OBJECT(current_object)->handler = build_grate_steps_ADDRESS;
}

void build_grate_steps(void)
{
	uint32_t ignored = 0, object = 0, slope_shift = 0;
	uint8_t rows_to_build = 0;
	uint16_t rows_left = 0;

	if (free_object_stack < 6)
		return;
	grate_rows_to_build = 1;
	do
	{
		OBJECT(current_object)->projected_y = OBJECT(current_object)->build_grate_steps.row_projected_y;
		OBJECT(current_object)->projected_x = OBJECT(current_object)->build_grate_steps.row_projected_x_whole;
		grate_step_number = 0;
		do
		{
			allocate_object(&ignored, &object);
			OBJECT(object)->sprite_flags = 0x1009u;
			OBJECT(object)->sprite_frame = 0x7FF0u;
			OBJECT(object)->sprite_palette = 0x11B0u;
			OBJECT(object)->sprite_offset_x = 0;
			OBJECT(object)->sprite_offset_y = 0;
			OBJECT(object)->flags = 0u;
			OBJECT(object)->screen_y = 0;
			OBJECT(object)->kind = grate_step_number;
			OBJECT(object)->handler = grate_step_update_ADDRESS;
			OBJECT(object)->animation = grate_step_cut_animation_ADDRESS;
			OBJECT(object)->animation_frame = 0u;
			OBJECT(object)->projected_x = OBJECT(current_object)->projected_x;
			OBJECT(object)->projected_y = OBJECT(current_object)->projected_y;
			OBJECT(object)->z = OBJECT(current_object)->z;
			AS32(OBJECT(object)->grate_step_update.cut_z) = AS32(OBJECT(object)->z_int);
			OBJECT(object)->grate_step_update.cut_z += 0x1Eu;
			OBJECT(current_object)->projected_x += 0x35u;
			OBJECT(current_object)->projected_y -= 0x1A8000u;
			grate_step_number++;
		} while (grate_step_number < 3);
		slope_shift = (int16_t)ROM16(floor_attribute_slopes_ADDRESS + OBJECT(current_object)->floor_attribute) * 0x1Eu;
		OBJECT(current_object)->build_grate_steps.row_projected_x += slope_shift;
		OBJECT(current_object)->build_grate_steps.row_projected_y += slope_shift >> 1;
		OBJECT(current_object)->z_int += 0x1Eu;
		rows_to_build = grate_rows_to_build;
		grate_rows_to_build = rows_to_build - 1u;
	} while ((int8_t)rows_to_build > 1);
	rows_left = OBJECT(current_object)->build_grate_steps.rows_left;
	OBJECT(current_object)->build_grate_steps.rows_left = rows_left - 1u;
	if ((int16_t)rows_left > 1)
		return;
	delete_current_object();
}

void grate_step_update(void)
{
	uint8_t sound_id = 0;
	uint16_t cut_gap = 0;
	uint32_t r1 = 0, at_last_step = 0, depth = 0, ignored = 0, object = 0, wrapped = 0, write_index_address = 0;
	uint16_t cut_z = 0, saw_top_z = 0;

	if (OBJECT(current_object)->screen_y >= 0x118)
	{
		delete_current_object();
		return;
	}
	if ((int8_t)OBJECT(current_object)->animation_frame >= 0)
	{
		object = (current_object & ~0xFFFFu) | rising_saws[OBJECT(current_object)->kind];
		if ((int16_t)object >= 0)
		{
			cut_z = OBJECT(current_object)->grate_step_update.cut_z;
			saw_top_z = OBJECT(object)->z_int - 0x20u;
			cut_gap = (uint16_t)(saw_top_z - cut_z);
			if ((int16_t)saw_top_z <= (int16_t)cut_z)
			{
				if ((int16_t)cut_gap >= -0x20)
				{
					spawn_grate_step_debris();
					sound_id = 0x28u;
					play_sound(sound_id, &r1, &write_index_address);
				}
				OBJECT(current_object)->grate_step_update.cut_z -= 7u;
				at_last_step = 0u;
				animate_step(&r1, &wrapped, &at_last_step);
				if ((uint8_t)at_last_step != 0u)
					OBJECT(current_object)->animation_frame = 0xFFu;
			}
		}
	}
	update_screen_position(&ignored, &r1);
	if (OBJECT(current_object)->screen_y < -0x38)
		return;
	depth = 0xFFF8u;
	queue_sprite_at_depth(&ignored, &depth);
}

void spawn_grate_step_debris(void)
{
	uint32_t ignored = 0, object = 0, random_value = 0, record = 0;
	uint32_t stack_4 = 0;
	uint16_t debris_left = 0;

	record = grate_step_debris_ADDRESS;
	if (free_object_stack < (int16_t)grate_step_debris)
		return;
	grate_debris_left = ROM16(record);
	grate_debris_record = record + 2u;
	do
	{
		allocate_object(&ignored, &object);
		OBJECT(object)->sprite_flags = 0x1000u;
		OBJECT(object)->sprite_palette = 0x11C0u;
		OBJECT(object)->flags = 0u;
		OBJECT(object)->sprite_offset_x = 0;
		OBJECT(object)->sprite_offset_y = 0;
		OBJECT(object)->handler = grate_step_debris_fly_ADDRESS;
		OBJECT(object)->grate_step_debris_view.projected_x_whole = OBJECT(current_object)->projected_x;
		OBJECT(object)->spawn_arg_2 = 0u;
		OBJECT(object)->projected_y = OBJECT(current_object)->projected_y;
		OBJECT(object)->z_int = OBJECT(current_object)->spawn_grate_step_debris.cut_z;
		OBJECT(object)->z_fraction = 0u;
		record = grate_debris_record;
		OBJECT(object)->grate_step_debris_view.projected_x_whole += mem_read16(record);
		record += 2u;
		OBJECT(object)->projected_y_int += mem_read16(record);
		record += 2u;
		OBJECT(object)->speed_x = (uint32_t)(int16_t)mem_read16(record) << 8;
		record += 2u;
		OBJECT(object)->speed_z = (uint32_t)(int16_t)mem_read16(record) << 8;
		stack_4 = record + 2u;
		next_random(&random_value, &ignored, &ignored);
		record = stack_4;
		OBJECT(object)->speed_x += (int16_t)random_value;
		random_value = (int32_t)random_value >> 16;
		OBJECT(object)->speed_z += random_value;
		OBJECT(object)->animation = mem_read16(record) + grate_step_debris_animations_ADDRESS;
		OBJECT(object)->animation_frame = 0xFFu;
		OBJECT(object)->animation_speed = 3u;
		OBJECT(object)->animation_timer = 0;
		grate_debris_record = record + 2u;
		debris_left = grate_debris_left;
		grate_debris_left = debris_left - 1u;
	} while ((int16_t)debris_left > 1);
}

void grate_step_debris_fly(void)
{
	uint32_t r1 = 0, r7 = 0, depth = 0, ignored = 0, off_screen = 0;

	OBJECT(current_object)->speed_z += 0x6000u;
	OBJECT(current_object)->z += OBJECT(current_object)->speed_z;
	OBJECT(current_object)->grate_step_debris_view.fixed_projected_x += OBJECT(current_object)->speed_x;
	OBJECT(current_object)->projected_x = OBJECT(current_object)->grate_step_debris_view.projected_x_whole;
	update_screen_position(&ignored, &r1);
	off_screen = is_off_screen();
	if (off_screen == 0u)
	{
		animate(&r1, &r7, &off_screen);
		depth = 0xE0u;
		queue_sprite_at_depth(&ignored, &depth);
		return;
	}
	delete_current_object();
}

void build_saw_shaft_base(void)
{
	uint16_t piece_frame = 0;
	uint32_t block_offset = 0, ignored = 0, object = 0, slope_shift = 0, x = 0, y = 0;

	OBJECT(current_object)->handler = build_saw_shaft_base_ADDRESS;
	if (scroll_position < 0x3780u && biting_head_done == 0u)
		return;
	OBJECT(current_object)->flags = 0u;
	find_floor(&x, &y, &block_offset, &ignored, &object);
	OBJECT(current_object)->z = OBJECT(current_object)->floor_z;
	OBJECT(current_object)->z_int -= 0x3Au;
	project_object(&ignored, &ignored);
	OBJECT(current_object)->build_saw_shaft_base.projected_x_whole = OBJECT(current_object)->projected_x;
	OBJECT(current_object)->spawn_arg_2 = 0u;
	saw_shaft_part = 0;
	do
	{
		allocate_object(&ignored, &object);
		OBJECT(object)->sprite_flags = 0x1004u;
		OBJECT(object)->sprite_palette = 0x11A0u;
		OBJECT(object)->sprite_offset_x = 0;
		OBJECT(object)->sprite_offset_y = 0;
		OBJECT(object)->flags = 0u;
		OBJECT(object)->saw_shaft_base_draw.state = 1u;
		OBJECT(object)->handler = saw_shaft_base_draw_ADDRESS;
		piece_frame = 0x7FB8u;
		if ((uint16_t)saw_shaft_part == 0u)
		{
		build_saw_shaft_base_end_piece:
			piece_frame = 0x7FE8u;
		}
		else if ((uint16_t)saw_shaft_part == 2u)
		{
			OBJECT(object)->sprite_flags |= 0x80u;
			goto build_saw_shaft_base_end_piece;
		}
		OBJECT(object)->sprite_frame = piece_frame;
		OBJECT(object)->projected_x = OBJECT(current_object)->build_saw_shaft_base.projected_x_whole;
		OBJECT(object)->projected_y = OBJECT(current_object)->projected_y;
		OBJECT(object)->z = OBJECT(current_object)->z;
		OBJECT(current_object)->z_int += 0x1Du;
		slope_shift = (int16_t)ROM16(floor_attribute_slopes_ADDRESS + OBJECT(current_object)->floor_attribute) * 0x1Du;
		OBJECT(current_object)->build_saw_shaft_base.fixed_projected_x += slope_shift;
		OBJECT(current_object)->projected_y += slope_shift >> 1;
		saw_shaft_part++;
	} while (saw_shaft_part < 3);
	delete_current_object();
}

void saw_shaft_base_draw(void)
{
	uint32_t r1 = 0, depth = 0, ignored = 0, off_screen = 0;

	update_screen_position(&ignored, &r1);
	off_screen = check_left_screen();
	if (off_screen == 0u)
	{
		depth = 0xFFF7u;
		queue_sprite_at_depth(&ignored, &depth);
		return;
	}
	delete_current_object();
}

void river_raft_spawner_init(void)
{
	uint32_t record = 0;

	record = river_raft_spawners[OBJECT(current_object)->sprite_frame] + river_raft_spawners_ADDRESS;
	OBJECT(current_object)->x_int = ROM16(record);
	OBJECT(current_object)->x_fraction = 0u;
	record += 2u;
	OBJECT(current_object)->y_int = ROM16(record);
	OBJECT(current_object)->y_fraction = 0u;
	record += 2u;
	OBJECT(current_object)->floor_z_int = ROM16(record);
	OBJECT(current_object)->floor_z_fraction = 0u;
	record += 2u;
	OBJECT(current_object)->river_raft_spawner.end_position = ROM16(record);
	record += 2u;
	OBJECT(current_object)->river_raft_spawner.entry = record;
	OBJECT(current_object)->river_raft_spawner.list_start = record;
	OBJECT(current_object)->river_raft_spawner.last_raft_y = OBJECT(current_object)->y;
	OBJECT(current_object)->river_raft_spawner.last_raft_speed = 0u;
	OBJECT(current_object)->z = OBJECT(current_object)->floor_z;
	OBJECT(current_object)->handler = river_raft_spawner_update_ADDRESS;
}

void river_raft_spawner_update(void)
{
	uint32_t floor_block_record = 0, ignored = 0, object = 0, object_2 = 0, raft_entry = 0;

	if (OBJECT(current_object)->river_raft_spawner.end_position <= scroll_position)
	{
		delete_current_object();
		return;
	}
	if (scroll_position >= 0x9C00u)
		goal_unlocked = 0xFFu;
	OBJECT(current_object)->river_raft_spawner.last_raft_y += OBJECT(current_object)->river_raft_spawner.last_raft_speed >> 8;
	if (OBJECT(current_object)->y_int <= OBJECT(current_object)->river_raft_spawner.last_raft_y_int && OBJECT(current_object)->y_int <= OBJECT(LINKED(current_object, group_leaders[0]))->y_int)
	{
		while ((int16_t)mem_read16(OBJECT(current_object)->river_raft_spawner.entry) >= 0)
		{
			if (free_object_stack < 2)
			{
				project_object(&ignored, &ignored);
				if (OBJECT(current_object)->screen_x >= -0x10)
				{
					compute_camera_ground_speed(0u);
					OBJECT(current_object)->y -= camera_ground_speed >> 7;
				}
				return;
			}
			allocate_object(&ignored, &object);
			OBJECT(object)->sprite_flags = 0x100Au;
			OBJECT(object)->sprite_frame = 0x8DF0u;
			OBJECT(object)->sprite_palette = 0xB1A0u;
			OBJECT(object)->hitbox_size_x = 0x30u;
			OBJECT(object)->hitbox_size_y = 0x88u;
			OBJECT(object)->direction = 0u;
			AS8(OBJECT(object)->var_84) = 0u;
			OBJECT(object)->sprite_offset_x = -8;
			OBJECT(object)->sprite_offset_y = -0x10;
			OBJECT(object)->flags = 0u;
			OBJECT(object)->handler = river_raft_update_ADDRESS;
			raft_entry = OBJECT(current_object)->river_raft_spawner.entry;
			OBJECT(object)->y = OBJECT(current_object)->y;
			OBJECT(object)->y -= (uint32_t)mem_read16(raft_entry) << 16;
			raft_entry += 2u;
			OBJECT(object)->speed_int = mem_read16(raft_entry);
			OBJECT(object)->speed_fraction = 0u;
			raft_entry += 2u;
			OBJECT(object)->floor_block = mem_read32(raft_entry);
			OBJECT(current_object)->river_raft_spawner.entry = raft_entry + 4u;
			floor_block_record = floor_blocks + 2u + OBJECT(object)->floor_block;
			OBJECT(object)->floor_z_int = mem_read16(floor_block_record + 0x10u);
			OBJECT(object)->floor_z_fraction = 0u;
			OBJECT(object)->z = OBJECT(object)->floor_z;
			OBJECT(object)->x = mem_read32(floor_block_record);
			OBJECT(object)->x -= 0x10000u;
			OBJECT(current_object)->river_raft_spawner.last_raft_y = OBJECT(object)->y;
			OBJECT(current_object)->river_raft_spawner.last_raft_speed = OBJECT(object)->speed;
			object_2 = object;
			allocate_object(&ignored, &object);
			OBJECT(object)->sprite_flags = OBJECT(object_2)->sprite_flags;
			OBJECT(object)->sprite_offset_x = OBJECT(object_2)->sprite_offset_x;
			OBJECT(object)->sprite_offset_y = OBJECT(object_2)->sprite_offset_y;
			OBJECT(object)->flags = 0u;
			AS8(OBJECT(object)->var_84) = 0u;
			OBJECT(object)->sprite_palette = 0xB190u;
			OBJECT(object)->handler = river_raft_foam_update_ADDRESS;
			OBJECT(object)->animation = river_raft_foam_animation_ADDRESS;
			OBJECT(object)->animation_frame = 0xFFu;
			OBJECT(object)->animation_timer = 0;
			OBJECT(object)->animation_speed = 4u;
			OBJECT(object)->parent = object_2;
			OBJECT(object_2)->child = object;
		}
		if (mem_read16(OBJECT(current_object)->river_raft_spawner.entry) == 0x8000u)
			OBJECT(current_object)->river_raft_spawner.entry = OBJECT(current_object)->river_raft_spawner.list_start;
	}
	project_object(&ignored, &ignored);
	if (OBJECT(current_object)->screen_x >= -0x10)
	{
		compute_camera_ground_speed(0u);
		OBJECT(current_object)->y -= camera_ground_speed >> 7;
	}
}

void river_raft_update(void)
{
	uint16_t draw_depth = 0;
	uint32_t r7 = 0, r8 = 0, depth = 0, frame = 0, ignored = 0, object = 0, off_screen = 0;

	switch (OBJECT(current_object)->river_raft_update.state)
	{
	case 0:
	case 4:
		OBJECT(current_object)->river_raft_update.state++;
		OBJECT(current_object)->speed_y = (uint32_t)OBJECT(current_object)->speed >> 8;
		OBJECT(current_object)->speed_x = 0;
		project_object(&ignored, &ignored);
		return;
	case 1:
		off_screen = check_left_screen();
		if (off_screen != 0u)
		{
			current_object = LINKED(current_object, OBJECT(current_object)->child);
			OBJECT(current_object)->flags |= 0x400u;
			delete_current_object();
			return;
		}
		OBJECT(current_object)->y += OBJECT(current_object)->speed_y;
		OBJECT(current_object)->z_int = OBJECT(current_object)->floor_z_int;
		if ((OBJECT(current_object)->flags & 8u) != 0u)
			OBJECT(current_object)->z_int += 2u;
		project_object(&ignored, &ignored);
		add_to_platform_list();
		if (OBJECT(LINKED(current_object, group_leaders[0]))->y_int <= (int16_t)(OBJECT(current_object)->y_int + OBJECT(current_object)->hitbox_size_y))
			OBJECT(current_object)->river_raft_update.state++;
		draw_depth = (uint16_t)(OBJECT(current_object)->screen_y - OBJECT(current_object)->sprite_offset_y - 0x10u);
		if ((int16_t)draw_depth <= -0x10)
			draw_depth = 0xFFF1u;
		OBJECT(current_object)->river_raft_update.draw_depth = draw_depth;
		current_object = LINKED(current_object, OBJECT(current_object)->child);
		depth = OBJECT(object_cursor)->river_raft_update.draw_depth;
		queue_sprite_at_depth(&ignored, &depth);
		current_object = object_cursor;
		depth = OBJECT(current_object)->river_raft_update.draw_depth;
		queue_sprite_at_depth(&ignored, &depth);
		return;
	case 2:
		off_screen = check_left_screen();
		if (off_screen != 0u)
		{
			current_object = LINKED(current_object, OBJECT(current_object)->child);
			OBJECT(current_object)->flags |= 0x400u;
			delete_current_object();
			return;
		}
		OBJECT(current_object)->y += OBJECT(current_object)->speed_y;
		OBJECT(current_object)->z_int = OBJECT(current_object)->floor_z_int;
		if ((OBJECT(current_object)->flags & 8u) != 0u)
			OBJECT(current_object)->z_int += 2u;
		add_to_platform_list();
		if (OBJECT(LINKED(current_object, group_leaders[0]))->y_int <= (int16_t)(OBJECT(current_object)->y_int + (OBJECT(current_object)->hitbox_size_y >> 1)))
		{
			OBJECT(current_object)->river_raft_update.state++;
			OBJECT(current_object)->animation = river_raft_fall_animation_ADDRESS;
			OBJECT(current_object)->animation_frame = 0xFFu;
			OBJECT(current_object)->animation_speed = 4u;
			OBJECT(current_object)->animation_timer = 0;
			OBJECT(current_object)->sprite_frame = mem_read16(OBJECT(current_object)->animation + 2u);
			OBJECT(current_object)->sprite_flags = 0x1000u;
			OBJECT(current_object)->sprite_offset_x = 0;
			OBJECT(current_object)->sprite_offset_y = 0;
			OBJECT(current_object)->x_int += 0x16u;
			OBJECT(current_object)->y_int += 0x44u;
			OBJECT(current_object)->speed_z = 0;
			object = LINKED(current_object, OBJECT(current_object)->child);
			OBJECT(object)->flags |= 0x400u;
		}
		project_object(&ignored, &ignored);
		OBJECT(current_object)->river_raft_update.draw_depth = OBJECT(current_object)->screen_y - OBJECT(current_object)->sprite_offset_y - 0x10u;
		current_object = LINKED(current_object, OBJECT(current_object)->child);
		if ((OBJECT(current_object)->flags & 0x400u) == 0u)
		{
			depth = OBJECT(object_cursor)->river_raft_update.draw_depth;
			queue_sprite_at_depth(&ignored, &depth);
		}
		current_object = object_cursor;
		depth = OBJECT(current_object)->river_raft_update.draw_depth;
		queue_sprite_at_depth(&ignored, &depth);
		return;
	case 3:
		if (OBJECT(current_object)->screen_y >= 0xE8)
		{
			delete_current_object();
			return;
		}
		r8 = check_left_screen();
		if (r8 != 0u)
		{
			delete_current_object();
			return;
		}
		OBJECT(current_object)->y += OBJECT(current_object)->speed_y;
		OBJECT(current_object)->speed_z += 0x2000u;
		OBJECT(current_object)->z += OBJECT(current_object)->speed_z;
		if ((int8_t)OBJECT(current_object)->animation_speed >= 0)
		{
			animate(&frame, &r7, &r8);
			if ((uint8_t)r8 != 0u)
				OBJECT(current_object)->animation_speed = 0xFFu;
		}
		OBJECT(current_object)->sprite_palette = river_raft_fall_palettes[OBJECT(current_object)->animation_frame];
		project_object(&ignored, &ignored);
		depth = (uint16_t)(OBJECT(current_object)->screen_y - OBJECT(current_object)->sprite_offset_y - 0x10u);
		queue_sprite_at_depth(&ignored, &depth);
		return;
	default:
		unknown_code(0x8C340u + OBJECT(current_object)->river_raft_update.state * 3u);
	}
}

void river_raft_foam_update(void)
{
	uint32_t r7 = 0, r8 = 0, frame = 0, object = 0;

	if ((OBJECT(current_object)->flags & 0x400u) == 0u)
	{
		object = LINKED(current_object, OBJECT(current_object)->parent);
		OBJECT(current_object)->screen_x = OBJECT(object)->screen_x;
		OBJECT(current_object)->screen_y = OBJECT(object)->screen_y;
		animate(&frame, &r7, &r8);
		return;
	}
	delete_current_object();
}

void clipped_copy_sprite_start(uint32_t copy)
{
	uint32_t block_offset = 0, clips_left = 0, ignored = 0, object = 0, object_2 = 0, object_3 = 0, x = 0, y = 0;

	OBJECT(current_object)->sprite_flags = 4u;
	OBJECT(current_object)->sprite_scale = 0x100u;
	OBJECT(current_object)->sprite_frame = 0x7058u;
	OBJECT(current_object)->sprite_palette = 0x5050u;
	OBJECT(current_object)->sprite_offset_x = 0;
	OBJECT(current_object)->sprite_offset_y = 0;
	OBJECT(current_object)->flags = 0u;
	find_floor(&x, &y, &block_offset, &ignored, &copy);
	OBJECT(current_object)->z = OBJECT(current_object)->floor_z;
	project_object(&ignored, &ignored);
	if (free_object_stack <= 0 || free_object_stack_2[0] < 2)
		return;
	take_free_object(&ignored, &copy);
	move_halfwords_backward(copy, current_object, 0x50u);
	mem_write32(copy + 6u, mem_read32(copy + 6u) | 0x8000u);
	mem_write16(copy + 6u, mem_read16(copy + 6u) & 0x9FFFu);
	OBJECT(current_object)->clipped_copy_sprite.copy_object = copy;
	clips_left = 2u;
	object = current_object;
	do
	{
		object_2 = object;
		allocate_object_2(&ignored, &ignored, &object);
		object_3 = object + 2u;
		OBJECT(object_3 - 0x2u)->sprite_flags = 0x5000u;
		object_3 += 2u;
		OBJECT(object_3 - 0x4u)->sprite_frame = 0xE0u;
		object_3 += 2u;
		OBJECT(object_3 - 0x6u)->sprite_palette = 0u;
		object_3 += 2u;
		OBJECT(object_3 - 0x8u)->sprite_scale = 0x140u;
		object_3 += 2u;
		OBJECT(object_3 - 0xAu)->sprite_layer = 0u;
		object_3 += 2u;
		OBJECT(object_3 - 0xCu)->screen_y = 0;
		object_3 += 2u;
		OBJECT(object_3 - 0xEu)->screen_x = 0;
		OBJECT(object_3 - 0xEu)->hitbox_size_x = 0u;
		OBJECT(object)->parent = object_2;
		OBJECT(object_2)->child = object;
		clips_left--;
	} while (clips_left != 0u);
	OBJECT(current_object)->handler = clipped_copy_sprite_update_ADDRESS;
}

void clipped_copy_sprite_update(void)
{
	uint16_t child = 0, object = 0, projected_x = 0;
	uint32_t r1 = 0, r7 = 0, r8 = 0, depth = 0, ignored = 0, projected_y = 0, z = 0;

	r8 = check_left_screen();
	if (r8 == 0u)
	{
		update_screen_position(&ignored, &r1);
		OBJECT(current_object)->clipped_copy_sprite.draw_depth = ((int32_t)((uint32_t)OBJECT(current_object)->projected_y + OBJECT(current_object)->floor_z) >> 16) - view_y;
		depth = OBJECT(current_object)->clipped_copy_sprite.draw_depth;
		current_object = LINKED(current_object, OBJECT(current_object)->child);
		current_object = LINKED(current_object, OBJECT(current_object)->child);
		queue_sprite_at_depth(&ignored, &depth);
		current_object = object_cursor;
		depth = OBJECT(current_object)->clipped_copy_sprite.draw_depth;
		queue_sprite_at_depth(&ignored, &depth);
		projected_x = (uint16_t)OBJECT(current_object)->projected_x;
		projected_y = OBJECT(current_object)->projected_y;
		z = OBJECT(current_object)->floor_z - 0x600000u;
		point_to_screen(projected_x, z, &projected_y, &r7, &r8);
		current_object = LINKED(current_object, OBJECT(current_object)->child);
		OBJECT(current_object)->sprite_flags &= ~0xFFFu;
		OBJECT(current_object)->sprite_flags |= r8 & 0xFFFu;
		depth = OBJECT(object_cursor)->clipped_copy_sprite.draw_depth;
		queue_sprite_at_depth(&ignored, &depth);
		current_object = (current_object & ~0xFFFFu) | OBJECT(object_cursor)->clipped_copy_sprite.copy_object;
		OBJECT(current_object)->screen_x = OBJECT(object_cursor)->screen_x;
		OBJECT(current_object)->screen_y = OBJECT(object_cursor)->screen_y;
		depth = OBJECT(object_cursor)->clipped_copy_sprite.draw_depth;
		queue_sprite_at_depth(&ignored, &depth);
		return;
	}
	current_object = (current_object & ~0xFFFFu) | OBJECT(object_cursor)->clipped_copy_sprite.copy_object;
	push_free_object();
	current_object = LINKED(current_object, OBJECT(object_cursor)->child);
	child = OBJECT(current_object)->child;
	free_object_2(child);
	object = (uint16_t)current_object;
	free_object_2(object);
	delete_current_object();
}

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

void propeller_machine_init(void)
{
	uint32_t free_count = 0, parts = 0, zone_index = 0;

	OBJECT(current_object)->propeller_machine.path = propeller_machine_path_ADDRESS;
	OBJECT(current_object)->sprite_flags = 0xD004u;
	OBJECT(current_object)->kind = 0x18u;
	OBJECT(current_object)->flags = 0x40000002u;
	OBJECT(current_object)->hitbox_size_x = 0xF2u;
	OBJECT(current_object)->hitbox_size_y = 0x80u;
	OBJECT(current_object)->direction = 0x200u;
	OBJECT(current_object)->hitbox_height = 0xF0u;
	OBJECT(current_object)->sprite_offset_x = 0;
	OBJECT(current_object)->sprite_offset_y = -8;
	OBJECT(current_object)->hitbox_offset_x = -8;
	OBJECT(current_object)->hitbox_offset_y = -0x70;
	chaser_object = current_object;
	OBJECT(current_object)->propeller_machine.sound_timer = 0u;
	OBJECT(current_object)->animation = propeller_machine_animation_ADDRESS;
	OBJECT(current_object)->animation_frame = 0u;
	OBJECT(current_object)->animation_speed = 5u;
	OBJECT(current_object)->animation_timer = 0;
	OBJECT(current_object)->propeller_machine.holding_back = 0u;
	OBJECT(current_object)->propeller_machine.wait_timer = 0;
	OBJECT(current_object)->handler = propeller_machine_init_ADDRESS;
	take_free_object(&free_count, &parts);
	if (free_count != 0u)
	{
		move_halfwords(parts, propeller_machine_parts_ADDRESS, 0x2Du);
		OBJECT(current_object)->part_list = parts;
		zone_index = 0u;
		spawn_chaser_back_zone(zone_index, parts);
		OBJECT(current_object)->handler = propeller_machine_chase_ADDRESS;
	}
}

void propeller_machine_chase(void)
{
	uint32_t r1 = 0, animation = 0, cursor = 0, depth = 0, frame_entry = 0, ignored = 0, part_frame = 0, slope_factor = 0, sound_id = 0;
	uint8_t hold_timer = 0, animation_timer = 0;
	uint16_t wait_timer = 0, trigger_scroll = 0, trigger_y = 0, sound_timer = 0;
	uint32_t flags = 0;

	flags = OBJECT(current_object)->flags;
	OBJECT(current_object)->flags = flags | 0x80000000u;
	if ((flags & 0x80000000u) == 0u)
	{
	propeller_machine_read_script:
		wait_timer = OBJECT(current_object)->propeller_machine.wait_timer;
		OBJECT(current_object)->propeller_machine.wait_timer = wait_timer - 1u;
		if ((int16_t)wait_timer <= 1)
		{
			OBJECT(current_object)->propeller_machine.wait_timer = 0;
			cursor = OBJECT(current_object)->propeller_machine.path;
			trigger_scroll = mem_read16(cursor);
			cursor += 2u;
			if (trigger_scroll <= scroll_position)
			{
				cursor += 2u;
				goto propeller_machine_load_record;
			}
			trigger_y = mem_read16(cursor);
			cursor += 2u;
			if ((int16_t)trigger_y <= OBJECT(current_object)->y_int)
			{
			propeller_machine_load_record:
				if ((int16_t)mem_read16(cursor + 2u) < 0)
				{
					current_object = (current_object & ~0xFFFFu) | OBJECT(current_object)->sprite_frame;
					push_free_object();
					delete_current_object();
					return;
				}
				OBJECT(current_object)->propeller_machine.screen_threshold = mem_read16(cursor);
				cursor += 2u;
				OBJECT(current_object)->propeller_machine.catch_up_factor = mem_read16(cursor);
				cursor += 2u;
				OBJECT(current_object)->propeller_machine.base_speed = mem_read16(cursor);
				cursor += 2u;
				OBJECT(current_object)->propeller_machine.wait_timer = mem_read16(cursor);
				OBJECT(current_object)->propeller_machine.path = cursor + 2u;
			}
		}
	}
	else if (OBJECT(current_object)->propeller_machine.holding_back == 0u)
	{
		if (no_player_in_play == 0u && player_mishap == 0u && player_dropping_in == 0u)
			goto propeller_machine_read_script;
		OBJECT(current_object)->propeller_machine.holding_back = 0xFFu;
		OBJECT(current_object)->propeller_machine.saved_base_speed = OBJECT(current_object)->propeller_machine.base_speed;
		OBJECT(current_object)->propeller_machine.base_speed = 0;
		OBJECT(current_object)->propeller_machine.hold_start_position = scroll_position;
		OBJECT(current_object)->propeller_machine.hold_timer = 0xFFu;
		OBJECT(current_object)->propeller_machine.saved_threshold = OBJECT(current_object)->propeller_machine.screen_threshold;
		OBJECT(current_object)->propeller_machine.screen_threshold = -0x50;
	}
	if (OBJECT(current_object)->propeller_machine.holding_back != 0u)
	{
		if ((int16_t)(scroll_position - OBJECT(current_object)->propeller_machine.hold_start_position) >= 0x300)
		{
		propeller_machine_stop_holding_back:
			OBJECT(current_object)->propeller_machine.holding_back = 0u;
			OBJECT(current_object)->propeller_machine.screen_threshold = OBJECT(current_object)->propeller_machine.saved_threshold;
			OBJECT(current_object)->propeller_machine.base_speed = OBJECT(current_object)->propeller_machine.saved_base_speed;
		}
		else
		{
			if (no_player_in_play != 0u)
				OBJECT(current_object)->propeller_machine.hold_timer = 0xFFu;
			hold_timer = OBJECT(current_object)->propeller_machine.hold_timer;
			OBJECT(current_object)->propeller_machine.hold_timer = hold_timer - 1u;
			if (hold_timer == 0u)
				goto propeller_machine_stop_holding_back;
		}
	}
	if (OBJECT(current_object)->propeller_machine.wait_timer <= 0)
	{
		if (OBJECT(current_object)->propeller_machine.screen_threshold <= OBJECT(current_object)->screen_x)
		{
		propeller_machine_use_base_speed:
			r1 = (uint16_t)OBJECT(current_object)->propeller_machine.base_speed;
		}
		else
		{
			slope_factor = 0u;
			compute_camera_slope_speed(0u, slope_factor);
			r1 = (uint16_t)((uint32_t)camera_slope_speed_int * OBJECT(current_object)->propeller_machine.catch_up_factor) >> 8;
			if ((int16_t)r1 <= OBJECT(current_object)->propeller_machine.base_speed)
				goto propeller_machine_use_base_speed;
		}
		chaser_speed = r1;
		OBJECT(current_object)->y += (uint32_t)chaser_speed << 12;
	}
	animation_timer = OBJECT(current_object)->animation_timer;
	OBJECT(current_object)->animation_timer = animation_timer - 1u;
	if ((int8_t)animation_timer <= 1)
	{
		OBJECT(current_object)->animation_timer = OBJECT(current_object)->animation_speed;
		animation = OBJECT(current_object)->animation;
		r1 = OBJECT(current_object)->animation_frame;
		r1 = (uint16_t)(r1 + 1u);
		if ((int16_t)mem_read16(animation) < (int16_t)r1)
			r1 = 0u;
		OBJECT(current_object)->animation_frame = r1;
		frame_entry = animation + 2u + r1 * 4u;
		part_frame = mem_read16(frame_entry);
		mem_write16(OBJECT(current_object)->part_list + 0x22u, mem_read16(frame_entry + 2u));
		mem_write16(OBJECT(current_object)->part_list + 0x32u, part_frame);
		mem_write16(OBJECT(current_object)->part_list + 0x4Au, part_frame);
	}
	if (OBJECT(current_object)->y_int >= 0x1BB0)
		geyser_front_start = 0xFFu;
	if (OBJECT(current_object)->y_int >= 0x1B00)
		OBJECT(current_object)->hitbox_height = 0x68u;
	if (OBJECT(current_object)->screen_y < 0x130)
	{
		if (OBJECT(current_object)->screen_x < -0x80)
			OBJECT(current_object)->propeller_machine.sound_timer = 0u;
		else
		{
			sound_timer = OBJECT(current_object)->propeller_machine.sound_timer;
			OBJECT(current_object)->propeller_machine.sound_timer = sound_timer - 1u;
			if ((int16_t)sound_timer <= 1)
			{
				OBJECT(current_object)->propeller_machine.sound_timer = 0x3Cu;
				sound_id = 0xBu;
				play_sound((uint8_t)sound_id, &r1, &part_frame);
			}
		}
	}
	find_floor(&sound_id, &r1, &part_frame, &ignored, &frame_entry);
	OBJECT(current_object)->z = OBJECT(current_object)->floor_z;
	project_object(&ignored, &ignored);
	add_to_zone_list(ignored);
	depth = (uint16_t)(OBJECT(current_object)->screen_y - 0x40u);
	queue_sprite_at_depth(&ignored, &depth);
}

void barred_gate_init(void)
{
	uint32_t ignored = 0;

	OBJECT(current_object)->sprite_flags = 0x1008u;
	OBJECT(current_object)->sprite_frame = 0x9F00u;
	OBJECT(current_object)->sprite_palette = 0x92F0u;
	OBJECT(current_object)->sprite_offset_x = 0;
	OBJECT(current_object)->sprite_offset_y = -0x49;
	OBJECT(current_object)->flags = 0u;
	OBJECT(current_object)->animation = barred_gate_animation_ADDRESS;
	OBJECT(current_object)->animation_frame = 0u;
	OBJECT(current_object)->animation_speed = 2u;
	OBJECT(current_object)->animation_timer = 0;
	OBJECT(current_object)->z = 0x1580000;
	project_object(&ignored, &ignored);
	OBJECT(current_object)->anchor_x = OBJECT(current_object)->projected_x;
	OBJECT(current_object)->anchor_y = OBJECT(current_object)->projected_y_int;
	OBJECT(current_object)->handler = barred_gate_update_ADDRESS;
}

void barred_gate_update(void)
{
	uint8_t sound_id = 0;
	uint32_t r7 = 0, r8 = 0, depth = 0, ignored = 0, margin = 0, write_index_address = 0;

	margin = 0x50u;
	r8 = check_left_screen_margin((uint16_t)margin);
	if (r8 != 0u)
	{
		delete_current_object();
		return;
	}
	if ((int8_t)OBJECT(current_object)->animation_frame >= 0 && barred_gate_closing != 0u)
	{
		animate(&margin, &r7, &r8);
		if ((uint8_t)r8 != 0u)
		{
			OBJECT(current_object)->animation_frame = 0xFFu;
			sound_id = 0x29u;
			play_sound(sound_id, &margin, &write_index_address);
		}
	}
	update_screen_position(&ignored, &margin);
	depth = (uint16_t)(OBJECT(current_object)->screen_y - 0x40u);
	queue_sprite_at_depth(&ignored, &depth);
}

void scene_anchor_init(void)
{
	uint32_t record = 0;

	record = OBJECT(current_object)->sprite_frame * 8u + scene_anchor_positions_ADDRESS;
	OBJECT(current_object)->x_int = ROM16(record);
	OBJECT(current_object)->x_fraction = 0u;
	record += 2u;
	OBJECT(current_object)->y_int = ROM16(record);
	OBJECT(current_object)->y_fraction = 0u;
	record += 2u;
	OBJECT(current_object)->floor_z_int = ROM16(record);
	OBJECT(current_object)->floor_z_fraction = 0u;
	OBJECT(current_object)->scene_anchor.end_position = ROM16(record + 2u);
	OBJECT(current_object)->handler = scene_anchor_update_ADDRESS;
	scene_object = current_object;
}

void scene_anchor_update(void)
{
	uint32_t floor_z = 0, ignored = 0, point_x = 0, point_y = 0;

	if (OBJECT(current_object)->scene_anchor.end_position <= scroll_position)
	{
		delete_current_object();
		return;
	}
	point_x = OBJECT(current_object)->x;
	point_y = OBJECT(current_object)->y;
	floor_z = OBJECT(current_object)->floor_z;
	project_point(floor_z, &ignored, &point_x, &point_y);
	OBJECT(current_object)->z_int = 0x70u + view_y - (point_y + view_y - OBJECT(current_object)->floor_z_int) + 0x30u;
	OBJECT(current_object)->z_fraction = 0u;
}

void railing_spawner_init(void)
{
	OBJECT(current_object)->railing_spawner.last_spawn_position = 0u;
	OBJECT(current_object)->z = 0x5200000;
	OBJECT(current_object)->handler = railing_spawner_update_ADDRESS;
}

void railing_spawner_update(void)
{
	uint32_t ignored = 0, object = 0;
	uint16_t segments_left = 0;

	if (scroll_position >= 0x5000u)
	{
		delete_current_object();
		return;
	}
	if ((int16_t)(scroll_position - OBJECT(current_object)->railing_spawner.last_spawn_position) < 0x800 || free_object_stack < 0xA)
		return;
	OBJECT(current_object)->railing_spawner.last_spawn_position = scroll_position;
	railing_segments_left = 0xA;
	OBJECT(current_object)->y_int += railing_segments_left * 0xB8u;
	railing_segment_y = OBJECT(current_object)->y;
	do
	{
		allocate_object(&ignored, &object);
		OBJECT(object)->sprite_flags = 0xD002u;
		OBJECT(object)->part_list = railing_parts_ADDRESS;
		OBJECT(object)->flags = 0u;
		OBJECT(object)->sprite_offset_x = 0;
		OBJECT(object)->sprite_offset_y = 4;
		OBJECT(object)->handler = railing_segment_start_ADDRESS;
		OBJECT(object)->z = 0x5280000;
		railing_segment_y_int -= 0xB8u;
		OBJECT(object)->x = OBJECT(current_object)->x;
		OBJECT(object)->y = railing_segment_y;
		OBJECT(object)->z = OBJECT(current_object)->z;
		segments_left = railing_segments_left;
		railing_segments_left = segments_left - 1u;
	} while ((int16_t)segments_left > 1);
}

void railing_segment_start(void)
{
	uint32_t ignored = 0;

	project_object(&ignored, &ignored);
	OBJECT(current_object)->handler = railing_segment_draw_ADDRESS;
}

void railing_segment_draw(void)
{
	uint32_t depth = 0, ignored = 0, margin = 0, off_screen = 0;

	if (scroll_position >= 0x6700u)
	{
		delete_current_object();
		return;
	}
	margin = 0x88u;
	off_screen = check_left_screen_margin((uint16_t)margin);
	if (off_screen != 0u)
	{
		delete_current_object();
		return;
	}
	update_screen_position(&ignored, &margin);
	if ((uint16_t)(OBJECT(current_object)->screen_x + 0x88u) >= 0x1C8u)
		return;
	depth = (uint16_t)(OBJECT(current_object)->screen_y + 0x40u);
	queue_sprite_at_depth(&ignored, &depth);
}

void reset_linked_objects_flag(void)
{
	electric_fence_state = 0u;
}

void start_spawns_by_type(void)
{
	uint32_t stage_number = 0;

	stage_number = stage;
	type_spawn_stage = stage_number;
	type_spawn_cursor[0] = stage_spawns_by_type[stage_number];
	unused_type_spawn_byte = 0u;
	string_count = 0;
	stage_trigger_bits = 0u;
}

void spawn_objects_by_type(uint32_t *p_player_object, uint32_t *p_player_index, uint32_t *p_first, uint32_t *p_projected_x, uint32_t *p_projected_y, uint32_t *p_object_type, uint32_t *p_object)
{
	uint32_t player_object = *p_player_object;
	uint32_t player_index = *p_player_index;
	uint32_t first = *p_first;
	uint32_t projected_x = *p_projected_x;
	uint32_t projected_y = *p_projected_y;
	uint32_t object_type = *p_object_type;
	uint32_t object = *p_object;
	uint16_t spawn_z = 0;
	uint32_t r0 = 0, r3 = 0, ignored = 0, record = 0, trigger_scroll = 0;

	if (type_spawn_stage != stage)
	{
		start_spawns_by_type();
		*p_player_object = player_object;
		*p_player_index = player_index;
		*p_first = first;
		*p_projected_x = projected_x;
		*p_projected_y = projected_y;
		*p_object_type = object_type;
		*p_object = object;
		return;
	}
	record = type_spawn_cursor[0];
	r0 = scroll_position;
	trigger_scroll = ROM16(record);
	record += 2u;
	if (trigger_scroll == 0xFFFFu || r0 <= trigger_scroll)
	{
		count_active_players(&player_object, &player_index);
		*p_player_object = player_object;
		*p_player_index = player_index;
		return;
	}
	projected_x = ROM16(record);
	record += 2u;
	projected_y = (int16_t)ROM16(record);
	record += 2u;
	spawn_z = (uint16_t)(int16_t)ROM16(record);
	record += 2u;
	if ((int32_t)trigger_scroll <= (int32_t)r0)
	{
		object_type = 0x100000u + ROM16(record) * 8u;
		spawn_count = ROM32(object_type);
		record += 2u;
		object_type += 4u;
		spawn_object_with_pieces(0x08DCB1u, &r0, &trigger_scroll, &player_index, &first, &object);
		if ((uint8_t)r0 == 0u)
		{
			type_spawn_cursor[0] += 0xEu;
			count_active_players(&player_object, &player_index);
			*p_player_object = player_object;
			*p_player_index = player_index;
			*p_first = first;
			*p_projected_x = projected_x;
			*p_projected_y = projected_y;
			*p_object_type = object_type;
			*p_object = object;
			return;
		}
		current_object = first;
		projected_y = projected_y << 16;
		OBJECT(current_object)->projected_x = projected_x;
		OBJECT(current_object)->projected_y = projected_y;
		OBJECT(current_object)->z = (uint32_t)spawn_z << 16;
		OBJECT(current_object)->flags = 0u;
		OBJECT(current_object)->handler = ROM32(object_type);
		OBJECT(current_object)->spawn_arg_1 = ROM16(record);
		record += 2u;
		OBJECT(current_object)->spawn_arg_2 = ROM16(record);
		type_spawn_cursor[0] = record + 2u;
		unproject_object(&ignored, &ignored, &r3, &object);
		count_active_players(&player_object, &player_index);
		*p_player_object = player_object;
		*p_player_index = player_index;
		*p_first = r3;
		*p_projected_x = projected_x;
		*p_projected_y = projected_y;
		*p_object_type = object_type;
		*p_object = object;
		return;
	}
	type_spawn_cursor[0] += 0xEu;
	count_active_players(&player_object, &player_index);
	*p_player_object = player_object;
	*p_player_index = player_index;
	*p_first = first;
	*p_projected_x = projected_x;
	*p_projected_y = projected_y;
	*p_object_type = object_type;
	*p_object = object;
}

void count_active_players(uint32_t *p_player_object, uint32_t *p_player_index)
{
	uint32_t object = 0, player = 0, player_index = 0, players_left = 0;

	players_left = 3u;
	player_index = 2u;
	active_player_count = 0u;
	do
	{
		player = GAME_VARIABLES + player_block_offsets[player_index];
		object = LINKED(player, PLAYER(player)->object);
		if ((OBJECT(object)->flags & 0x400u) == 0u)
			active_player_count++;
		players_left--;
		player_index--;
	} while (players_left != 0u);
	*p_player_object = object;
	*p_player_index = player_index;
}

void spawn_object_with_pieces(uint32_t return_address, uint32_t *p_spawned, uint32_t *p_piece_count, uint32_t *p_free_count, uint32_t *p_first, uint32_t *p_block)
{
	uint32_t spawned = *p_spawned;
	uint16_t piece_count = (uint16_t)*p_piece_count;
	uint32_t free_count = *p_free_count;
	uint32_t first = *p_first;
	uint32_t block = *p_block;
	uint32_t r2 = 0, ignored = 0, pieces_to_add = 0, pool_2_free = 0;
	uint16_t free_blocks = 0, spawn_total = 0;

	if ((int16_t)spawn_count <= 0)
	{
		current_object = object_cursor;
		return;
	}
	free_objects = free_object_stack_ADDRESS;
	first_object = first_object_address;
	last_object = last_object_address;
	spawned &= ~0xFFu;
	free_count = LINKED(free_count, free_object_stack);
	if (free_count == 0u)
	{
		current_object = object_cursor;
		*p_spawned = spawned;
		*p_free_count = free_count;
		return;
	}
	piece_count = (uint16_t)(spawn_count - 1u);
	if (piece_count != 0u)
	{
		pool_2_free = LINKED(WORK_RAM, free_object_stack_2[0]);
		free_blocks = pool_2_free;
		free_count = (pool_2_free & ~0xFFFFu) | (uint16_t)(free_blocks - piece_count);
		if (free_blocks < piece_count)
		{
			current_object = object_cursor;
			*p_spawned = spawned;
			*p_piece_count = (*p_piece_count & 0xFFFF0000u) | piece_count;
			*p_free_count = free_count;
			return;
		}
	}
	allocate_object(&ignored, &block);
	init_object(block);
	OBJECT(block)->next_handler = return_address;
	OBJECT(block)->parent = object_cursor;
	OBJECT(block)->child = 0xFFFFu;
	first = block;
	spawned = 1u;
	spawn_total = spawn_count;
	pieces_to_add = (uint16_t)(spawn_total - 1u);
	if (spawn_total == 1u)
	{
		current_object = object_cursor;
		*p_spawned = spawned;
		*p_piece_count = (*p_piece_count & 0xFFFF0000u) | (pieces_to_add & 0xFFFFu);
		*p_free_count = free_count;
		*p_first = first;
		*p_block = block;
		return;
	}
	add_object_pieces(&spawned, &pieces_to_add, &r2, &first, &block);
	*p_spawned = spawned;
	*p_piece_count = (*p_piece_count & 0xFFFF0000u) | (pieces_to_add & 0xFFFFu);
	*p_free_count = r2;
	*p_first = first;
	*p_block = block;
}

void add_pieces_to_current_object(uint32_t *p_spawned, uint32_t *p_free_count, uint32_t *p_first, uint32_t *p_block)
{
	uint32_t spawned = *p_spawned;
	uint32_t first = *p_first;
	uint32_t r2 = 0, block = 0, piece_count = 0;

	block = object_cursor;
	piece_count = spawn_count;
	add_object_pieces(&spawned, &piece_count, &r2, &first, &block);
	*p_spawned = spawned;
	*p_free_count = r2;
	*p_first = first;
	*p_block = block;
}

void add_object_pieces(uint32_t *p_spawned, uint32_t *p_piece_count, uint32_t *p_free_count, uint32_t *p_first, uint32_t *p_block)
{
	uint32_t spawned = *p_spawned;
	uint16_t piece_count = (uint16_t)*p_piece_count;
	uint32_t first = *p_first;
	uint32_t block = *p_block;
	uint32_t r2 = 0, free_count = 0, ignored = 0, object = 0, pool_2_free = 0;
	uint16_t free_blocks = 0;

	spawn_first = block;
	spawn_last = block;
	spawned &= ~0xFFu;
	piece_count = (uint16_t)piece_count;
	pool_2_free = LINKED(WORK_RAM, free_object_stack_2[0]);
	free_blocks = pool_2_free;
	free_count = (pool_2_free & ~0xFFFFu) | (uint16_t)(free_blocks - piece_count);
	if (free_blocks < piece_count)
	{
		current_object = object_cursor;
		*p_spawned = spawned;
		*p_piece_count = (*p_piece_count & 0xFFFF0000u) | piece_count;
		*p_free_count = free_count;
		return;
	}
	spawn_count = piece_count;
	do
	{
		allocate_object_2(&ignored, &r2, &block);
		init_object_2(block);
		object = spawn_last;
		AS16(spawn_last) = block;
		OBJECT(object)->child = block;
		OBJECT(block)->parent = AS16(spawn_first);
		OBJECT(block)->child = 0xFFFFu;
		spawn_count--;
	} while (spawn_count != 0u);
	first = spawn_first;
	spawned = 1u;
	current_object = object_cursor;
	*p_spawned = spawned;
	*p_piece_count = (*p_piece_count & 0xFFFF0000u) | piece_count;
	*p_free_count = r2;
	*p_first = first;
	*p_block = block;
}

void spawn_objects(uint32_t return_address, uint32_t *p_spawned, uint32_t *p_first, uint32_t *p_block)
{
	uint32_t spawned = *p_spawned;
	uint32_t block = *p_block;
	uint32_t first = 0, ignored = 0;

	if ((int16_t)spawn_count <= 0)
	{
		first = spawn_first;
		current_object = object_cursor;
		*p_first = first;
		return;
	}
	free_objects = free_object_stack_ADDRESS;
	first_object = first_object_address;
	last_object = last_object_address;
	spawned = 0u;
	if ((uint16_t)free_object_stack < spawn_count)
	{
		first = spawn_first;
		current_object = object_cursor;
		*p_spawned = spawned;
		*p_first = first;
		return;
	}
	allocate_object(&ignored, &block);
	spawn_first = block;
	spawn_last = block;
	OBJECT(block)->parent = object_cursor;
	OBJECT(block)->child = 0xFFFFu;
	init_object(block);
	OBJECT(block)->next_handler = return_address;
	for (;;)
	{
		spawn_last = block;
		spawn_count--;
		spawned = 1u;
		if (spawn_count == 0u)
			break;
		allocate_object(&ignored, &block);
		init_object(block);
		OBJECT(block)->next_handler = return_address;
		mem_write16(spawn_last + 0x28u, block);
		OBJECT(block)->parent = object_cursor;
		OBJECT(block)->child = 0xFFFFu;
	}
	first = spawn_first;
	current_object = object_cursor;
	*p_spawned = spawned;
	*p_first = first;
	*p_block = block;
}

void unlink_child_chain(uint32_t object)
{
	uint16_t next_child = 0;

	for (;;)
	{
		next_child = mem_read16(object + 0x28u);
		if (mem_read16(object + 0x28u) == 0xFFFFu)
			break;
		mem_write16(object + 0x28u, 0xFFFFu);
		object = (object & ~0xFFFFu) | next_child;
	}
}

void init_object(uint32_t object)
{
	uint8_t mode = 0;

	mode = 0u;
	init_object_fields(object, mode);
}

void init_object_2(uint32_t object)
{
	uint8_t mode = 0;

	mode = 1u;
	init_object_fields(object, mode);
}

void init_object_fields(uint32_t object, uint8_t mode)
{
	OBJECT(object)->draw_next = 0u;
	OBJECT(object)->sprite_flags = 4u;
	OBJECT(object)->sprite_frame = 0u;
	OBJECT(object)->sprite_palette = 0u;
	OBJECT(object)->sprite_scale = 0x100u;
	OBJECT(object)->sprite_layer = 0u;
	OBJECT(object)->screen_y = 0;
	OBJECT(object)->screen_x = 0;
	OBJECT(object)->hitbox_size_x = 0u;
	OBJECT(object)->hitbox_size_y = 0u;
	OBJECT(object)->anchor_x = 0;
	OBJECT(object)->anchor_y = 0;
	OBJECT(object)->sprite_height = 0;
	OBJECT(object)->sprite_width = 0;
	OBJECT(object)->handler = delete_object_and_children_ADDRESS;
	OBJECT(object)->direction = 0u;
	OBJECT(object)->projected_x = 0;
	OBJECT(object)->projected_y = 0;
	OBJECT(object)->projected_y_int = 0;
	if ((uint8_t)mode != 0u)
		return;
	OBJECT(object)->flags = 0u;
	init_object_fields_tail(object, mode);
}

void clear_object_speeds(void)
{
	uint8_t mode = 0;
	uint32_t object = 0;

	mode = 2u;
	object = current_object;
	init_object_fields_tail(object, mode);
}

void init_object_fields_tail(uint32_t object, uint8_t mode)
{
	OBJECT(object)->speed_x = 0;
	OBJECT(object)->speed_y = 0;
	OBJECT(object)->speed_z = 0;
	if ((uint8_t)mode != 2u)
	{
		OBJECT(object)->z = 0;
		OBJECT(object)->animation_speed = 0u;
		OBJECT(object)->animation_timer = 0;
		OBJECT(object)->animation_frame = 0u;
		OBJECT(object)->kind = 0u;
		OBJECT(object)->animation = 0u;
		OBJECT(object)->x = 0;
		OBJECT(object)->y = 0;
		OBJECT(object)->floor_z = 0;
		OBJECT(object)->previous_floor_z = 0;
		OBJECT(object)->floor_block = 0;
		OBJECT(object)->previous_floor_block = 0;
		OBJECT(object)->sprite_offset_x = 0;
		OBJECT(object)->sprite_offset_y = 0;
		OBJECT(object)->hitbox_height = 0u;
		OBJECT(object)->hit_list_next = 0u;
		OBJECT(object)->touched_by = 0u;
		OBJECT(object)->touch_distance = 0;
		OBJECT(object)->speed = 0;
		OBJECT(object)->platform_list_next = 0u;
		OBJECT(object)->platform = 0u;
		OBJECT(object)->floor_slope = 0u;
		OBJECT(object)->floor_type = 0u;
		OBJECT(object)->wall = 0u;
		OBJECT(object)->floor_attribute = 0u;
		OBJECT(object)->hitbox_offset_x = 0;
		OBJECT(object)->hitbox_offset_y = 0;
		OBJECT(object)->health = 0;
		OBJECT(object)->var_7F = 0u;
		OBJECT(object)->var_80 = 0u;
		OBJECT(object)->var_81 = 0u;
		OBJECT(object)->var_82 = 0u;
		OBJECT(object)->var_84 = 0u;
		OBJECT(object)->var_86 = 0u;
		OBJECT(object)->var_88 = 0u;
		OBJECT(object)->var_8A = 0u;
		OBJECT(object)->var_8C = 0u;
		OBJECT(object)->var_8E = 0u;
		OBJECT(object)->var_8F = 0u;
		OBJECT(object)->var_90 = 0u;
		OBJECT(object)->spawn_arg_1 = 0u;
		OBJECT(object)->spawn_arg_2 = 0u;
		OBJECT(object)->var_96 = 0u;
		AS32(OBJECT(object)->var_98) = 0u;
		OBJECT(object)->next_handler = 0u;
	}
}

void move_object_projected(uint32_t *p_screen_y, uint32_t *p_block_offset, uint32_t *p_slope_height, uint32_t *p_cursor)
{
	uint32_t cursor = *p_cursor;
	uint32_t r2 = 0, screen_y = 0, slope_height = 0;

	OBJECT(current_object)->move_object_projected.projected_x_carry += OBJECT(current_object)->speed_x;
	OBJECT(current_object)->projected_x += OBJECT(current_object)->move_object_projected.projected_x_carry_int;
	OBJECT(current_object)->projected_y += OBJECT(current_object)->speed_y;
	OBJECT(current_object)->z += OBJECT(current_object)->speed_z;
	OBJECT(current_object)->move_object_projected.projected_x_carry_int = 0;
	unproject_object(&screen_y, &r2, &slope_height, &cursor);
	*p_screen_y = screen_y;
	*p_block_offset = r2;
	*p_slope_height = slope_height;
	*p_cursor = cursor;
}

void unproject_object(uint32_t *p_screen_y, uint32_t *p_block_offset, uint32_t *p_slope_height, uint32_t *p_cursor)
{
	uint32_t cursor = *p_cursor;
	uint32_t r2 = 0, scaled_x = 0, scaled_y = 0, screen_y = 0, slope_height = 0;

	scaled_x = (uint32_t)OBJECT(current_object)->projected_x << 8;
	scaled_y = (uint32_t)((int32_t)OBJECT(current_object)->projected_y >> 16) << 9;
	r2 = scaled_x;
	OBJECT(current_object)->x = (scaled_x - scaled_y) * 0xB5u;
	OBJECT(current_object)->y = (scaled_y + r2) * 0xB5u;
	find_floor(&scaled_x, &scaled_y, &r2, &slope_height, &cursor);
	screen_y = update_screen_position_no_offset();
	*p_screen_y = screen_y;
	*p_block_offset = r2;
	*p_slope_height = slope_height;
	*p_cursor = cursor;
}

uint32_t update_screen_position_no_offset(void)
{
	uint32_t unscrolled_y = 0;

	OBJECT(current_object)->screen_x = OBJECT(current_object)->projected_x - view_x;
	unscrolled_y = (int32_t)((uint32_t)OBJECT(current_object)->projected_y + OBJECT(current_object)->z) >> 16;
	unscrolled_y = (unscrolled_y & ~0xFFFFu) | (uint16_t)(unscrolled_y - view_y);
	OBJECT(current_object)->screen_y = unscrolled_y;
	return unscrolled_y;
}

void move_object_projected_no_floor(void)
{
	OBJECT(current_object)->move_object_projected.projected_x_carry += OBJECT(current_object)->speed_x;
	OBJECT(current_object)->projected_x += OBJECT(current_object)->move_object_projected.projected_x_carry_int;
	OBJECT(current_object)->projected_y += OBJECT(current_object)->speed_y;
	OBJECT(current_object)->z += OBJECT(current_object)->speed_z;
	OBJECT(current_object)->move_object_projected.projected_x_carry_int = 0;
	OBJECT(current_object)->screen_x = OBJECT(current_object)->projected_x - view_x;
	OBJECT(current_object)->screen_y = ((int32_t)((uint32_t)OBJECT(current_object)->projected_y + OBJECT(current_object)->z) >> 16) - view_y;
}

uint32_t is_on_screen(void)
{
	uint16_t screen_x = 0, screen_y = 0;
	uint32_t on_screen = 0;

	screen_x = (uint16_t)OBJECT(current_object)->screen_x;
	screen_y = (uint16_t)OBJECT(current_object)->screen_y;
	on_screen = 0u;
	if ((int16_t)screen_x >= -0xC0 && (int16_t)screen_x <= 0x200 && (int16_t)screen_y >= -0xC0 && (int16_t)screen_y <= 0x1A0)
		on_screen = 1u;
	return on_screen;
}

void delete_if_off_screen(uint32_t *p_deleted, uint32_t *p_block_offset, uint32_t *p_slope_height)
{
	uint32_t block_offset = *p_block_offset;
	uint32_t slope_height = *p_slope_height;
	uint16_t screen_x = 0, screen_y = 0;
	uint32_t r0 = 0, deleted = 0;

	screen_x = (uint16_t)OBJECT(current_object)->screen_x;
	screen_y = (uint16_t)OBJECT(current_object)->screen_y;
	if ((int16_t)screen_x < -0xC0 || (int16_t)screen_x > 0x200 || (int16_t)screen_y < -0xC0 || (int16_t)screen_y > 0x1A0)
	{
		delete_object_and_children(&r0, &block_offset, &slope_height);
		*p_deleted = r0;
		*p_block_offset = block_offset;
		*p_slope_height = slope_height;
		return;
	}
	deleted = 0u;
	*p_deleted = deleted;
}

void delete_object_and_children(uint32_t *p_deleted, uint32_t *p_block_offset, uint32_t *p_slope_height)
{
	uint32_t block_offset = *p_block_offset;
	uint32_t slope_height = *p_slope_height;
	uint32_t r2 = 0, r3 = 0, child = 0, deleted = 0, free_blocks = 0, ignored = 0, object = 0;

	if (OBJECT(object_cursor)->child == 0xFFFFu)
	{
		delete_current_object();
		deleted = 1u;
		*p_deleted = deleted;
		return;
	}
	child = LINKED(WORK_RAM, OBJECT(object_cursor)->child);
	deleted_child = child;
	for (;;)
	{
		if ((int16_t)child < 0x5000)
		{
			delete_object(child, &ignored, &r2);
		delete_object_and_children_next_child:
			object = LINKED(WORK_RAM, deleted_child);
			child = LINKED(object, OBJECT(object)->child);
			if ((uint16_t)child == 0xFFFFu)
			{
				delete_current_object();
				deleted = 1u;
				*p_deleted = deleted;
				*p_block_offset = r2;
				return;
			}
			deleted_child = child;
			continue;
		}
		r2 = free_object_2((uint16_t)child);
		free_blocks = LINKED(WORK_RAM, free_object_stack_2[0]);
		if ((int16_t)free_blocks <= 0x100)
			goto delete_object_and_children_next_child;
		break;
	}
	int_default(&free_blocks, &r2, &r3, &ignored);
	*p_deleted = free_blocks;
	*p_block_offset = r2;
	*p_slope_height = r3;
}

void copy_position_to(uint32_t object)
{
	OBJECT(object)->x = OBJECT(object_cursor)->x;
	OBJECT(object)->y = OBJECT(object_cursor)->y;
	OBJECT(object)->projected_x = OBJECT(object_cursor)->projected_x;
	OBJECT(object)->projected_y = OBJECT(object_cursor)->projected_y;
	OBJECT(object)->z = OBJECT(object_cursor)->z;
	OBJECT(object)->floor_z = OBJECT(object_cursor)->floor_z;
}

void place_sprite_near_cursor(uint16_t offset_x, uint16_t offset_y)
{
	OBJECT(current_object)->screen_x = OBJECT(object_cursor)->screen_x;
	OBJECT(current_object)->screen_y = OBJECT(object_cursor)->screen_y;
	OBJECT(current_object)->screen_x += offset_x;
	OBJECT(current_object)->screen_y += offset_y;
}

void copy_position_to_cursor(void)
{
	OBJECT(object_cursor)->projected_x = OBJECT(current_object)->projected_x;
	OBJECT(object_cursor)->projected_y = OBJECT(current_object)->projected_y;
	OBJECT(object_cursor)->z = OBJECT(current_object)->z;
	OBJECT(object_cursor)->floor_z = OBJECT(current_object)->floor_z;
}

void copy_position_from_cursor(void)
{
	OBJECT(current_object)->x = OBJECT(object_cursor)->x;
	OBJECT(current_object)->y = OBJECT(object_cursor)->y;
	OBJECT(current_object)->projected_x = OBJECT(object_cursor)->projected_x;
	OBJECT(current_object)->projected_y = OBJECT(object_cursor)->projected_y;
	OBJECT(current_object)->z = OBJECT(object_cursor)->z;
	OBJECT(current_object)->floor_z = OBJECT(object_cursor)->floor_z;
}

uint32_t face_nearest_player(uint32_t direction, uint32_t octant)
{
	uint16_t dy = 0, player_dx = 0, player_dy = 0;
	uint32_t abs_dy = 0, distance = 0, ignored = 0, player_index = 0, player_object = 0;
	uint16_t diff_x = 0, diff_y = 0;
	uint32_t players_left = 0;

	nearest_player_distance = 0x7FFF;
	player_index = 3u;
	for (;;)
	{
		players_left = player_index;
		player_index = players_left - 1u;
		if (players_left == 0u)
			break;
		player_object = GAME_VARIABLES + player_block_offsets[player_index];
		player_object = LINKED(player_object, PLAYER(player_object)->object);
		if ((OBJECT(player_object)->flags & 0x400u) == 0u)
		{
			diff_x = OBJECT(player_object)->projected_x - OBJECT(current_object)->projected_x;
			distance = diff_x;
			player_dx = (uint16_t)distance;
			if ((int16_t)diff_x < 0)
				distance = (uint16_t)(0u - distance);
			diff_y = OBJECT(player_object)->projected_y_int - OBJECT(current_object)->projected_y_int;
			abs_dy = diff_y;
			player_dy = (uint16_t)abs_dy;
			if ((int16_t)diff_y < 0)
				abs_dy = (uint16_t)(0u - abs_dy);
			distance = (int16_t)distance;
			abs_dy = (int16_t)abs_dy;
			distance = distance * distance + abs_dy * abs_dy;
			square_root(&distance, &ignored);
			if ((int16_t)distance <= nearest_player_distance)
			{
				nearest_player = player_index;
				nearest_player_distance = distance;
				nearest_player_dx = player_dx;
				nearest_player_dy = player_dy;
			}
			RAM16(GAME_VARIABLES + distance_to_player_offsets[player_index]) = distance;
			direction = player_dx;
			dy = (uint16_t)player_dy;
			angle_of_vector(dy, &direction, &octant);
			RAM16(GAME_VARIABLES + direction_to_player_offsets[player_index]) = direction;
			continue;
		}
		RAM16(GAME_VARIABLES + distance_to_player_offsets[player_index]) = 0x7FFFu;
		RAM16(GAME_VARIABLES + direction_to_player_offsets[player_index]) = 0x7FFFu;
	}
	player_index = nearest_player;
	player_object = GAME_VARIABLES + player_block_offsets[player_index];
	player_object = LINKED(player_object, PLAYER(player_object)->object);
	nearest_player_z = OBJECT(player_object)->z_int;
	nearest_player_floor_z = OBJECT(player_object)->floor_z_int;
	direction = (uint16_t)(direction + direction_to_player_offsets[player_index]);
	OBJECT(current_object)->direction = direction;
	direction_to_nearest_player = direction;
	return octant;
}

void find_nearest_player_x(void)
{
	uint16_t distance_x = 0;
	uint32_t player_index = 0, player_object = 0;
	uint16_t diff_x = 0;
	uint32_t checked_index = 0;

	nearest_player_distance = 0x7FFF;
	player_index = 2u;
	do
	{
		player_object = GAME_VARIABLES + player_block_offsets[player_index];
		player_object = LINKED(player_object, PLAYER(player_object)->object);
		diff_x = OBJECT(player_object)->x_int - OBJECT(current_object)->x_int;
		distance_x = diff_x;
		if ((int16_t)diff_x < 0)
			distance_x = (uint16_t)(0u - distance_x);
		if ((int16_t)distance_x <= nearest_player_distance)
			nearest_player_distance = distance_x;
		checked_index = player_index;
		player_index = checked_index - 1u;
	} while (checked_index != 0u);
}

uint32_t face_object(void)
{
	uint16_t dy = 0;
	uint32_t dx = 0, ignored = 0;

	dx = (uint16_t)(OBJECT(r14)->projected_x - OBJECT(current_object)->projected_x);
	dy = (uint16_t)(OBJECT(r14)->projected_y_int - OBJECT(current_object)->projected_y_int);
	angle_of_vector(dy, &dx, &ignored);
	OBJECT(current_object)->direction = dx;
	return distance_to_object();
}

uint32_t distance_to_object(void)
{
	uint32_t distance = 0, dy = 0, ignored = 0;

	distance = (int16_t)(OBJECT(r14)->projected_x - OBJECT(current_object)->projected_x);
	dy = (int16_t)(OBJECT(r14)->projected_y_int - OBJECT(current_object)->projected_y_int);
	if ((int32_t)distance < 0)
		distance = 0u - distance;
	if ((int32_t)dy < 0)
		dy = 0u - dy;
	distance = distance * distance + dy * dy;
	square_root(&distance, &ignored);
	nearest_player_distance = distance;
	return distance;
}

uint32_t direction_to_object(void)
{
	uint16_t dy = 0;
	uint32_t dx = 0, octant = 0;

	dx = (uint16_t)(mem_read16(r14 + 0x2Au) - OBJECT(current_object)->projected_x);
	dy = (uint16_t)(mem_read16(r14 + 0x2Eu) - OBJECT(current_object)->projected_y_int);
	angle_of_vector(dy, &dx, &octant);
	OBJECT(current_object)->direction = dx;
	return octant;
}

void distance_xy_to_object(uint32_t *p_distance_x, uint32_t *p_distance_y)
{
	uint32_t distance_x = 0, distance_y = 0, x_int = 0, y_int = 0;
	uint16_t dx = 0, dy = 0;

	x_int = (uint16_t)OBJECT(r14)->x_int;
	dx = x_int - OBJECT(current_object)->x_int;
	distance_x = dx;
	if ((int16_t)dx < 0)
		distance_x = (uint16_t)(0u - distance_x);
	distance_to_player_1 = distance_x;
	y_int = (uint16_t)OBJECT(r14)->y_int;
	dy = y_int - OBJECT(current_object)->y_int;
	distance_y = dy;
	if ((int16_t)dy >= 0)
	{
		distance_to_player_2 = distance_y;
		*p_distance_x = distance_x;
		*p_distance_y = distance_y;
		return;
	}
	distance_y = (uint16_t)(0u - distance_y);
	distance_to_player_2 = distance_y;
	*p_distance_x = distance_x;
	*p_distance_y = distance_y;
}

void fall_to_floor(uint32_t *p_height, uint32_t *p_over_pit, uint32_t *p_z)
{
	uint32_t z = *p_z;
	uint8_t over_pit = 0;
	uint32_t height = 0;

	over_pit = 1u;
	if (OBJECT(current_object)->floor_z_int < 0x800)
		over_pit = 0u;
	if ((uint32_t)OBJECT(current_object)->z != (uint32_t)OBJECT(current_object)->floor_z)
	{
		OBJECT(current_object)->speed_z += 0x6000u;
		if ((int32_t)((uint32_t)OBJECT(current_object)->z + OBJECT(current_object)->speed_z) < OBJECT(current_object)->floor_z)
		{
			z = OBJECT(current_object)->z;
			height = OBJECT(current_object)->floor_z - z;
			*p_height = height;
			*p_over_pit = (*p_over_pit & 0xFFFFFF00u) | over_pit;
			*p_z = z;
			return;
		}
		OBJECT(current_object)->z = OBJECT(current_object)->floor_z;
		OBJECT(current_object)->speed_z = 0;
		height = 0u;
		*p_height = height;
		*p_over_pit = (*p_over_pit & 0xFFFFFF00u) | over_pit;
		return;
	}
	OBJECT(current_object)->speed_z = 0;
	height = 0u;
	*p_height = height;
	*p_over_pit = (*p_over_pit & 0xFFFFFF00u) | over_pit;
}

uint32_t place_sprite_at_cursor_floor(void)
{
	uint32_t bucket = 0;

	OBJECT(current_object)->screen_x = OBJECT(object_cursor)->screen_x;
	OBJECT(current_object)->screen_y = OBJECT(object_cursor)->screen_y;
	OBJECT(current_object)->screen_y += OBJECT(object_cursor)->floor_z_int - OBJECT(object_cursor)->z_int;
	bucket = 0xFFu;
	return bucket;
}

void direction_to_speed(uint32_t speed, uint32_t *p_direction, uint32_t *p_speed_x, uint32_t *p_speed_y)
{
	uint32_t direction = *p_direction;
	uint32_t speed_x = 0, speed_y = 0;

	direction &= 0x3FFu;
	speed_x = (int32_t)(sine_table[direction] * speed) >> 6;
	speed_y = 0u - ((int32_t)(cosine_table[direction] * speed) >> 6);
	*p_direction = direction;
	*p_speed_x = speed_x;
	*p_speed_y = speed_y;
}

uint32_t cursor_floor_screen_y(uint32_t floor_screen_y)
{
	floor_screen_y = (floor_screen_y & ~0xFFFFu) | (uint16_t)OBJECT(object_cursor)->floor_z_int;
	floor_screen_y = (floor_screen_y & ~0xFFFFu) | (uint16_t)(floor_screen_y - OBJECT(object_cursor)->z_int);
	floor_screen_y = (floor_screen_y & ~0xFFFFu) | (uint16_t)(floor_screen_y + OBJECT(current_object)->screen_y);
	return floor_screen_y;
}

void explode_object_medium(void)
{
	OBJECT(current_object)->explode_object.frame_index = 0xF;
	OBJECT(current_object)->explode_object.last_frame = 0x1F;
	OBJECT(current_object)->sprite_palette = 0x82A0u;
	OBJECT(current_object)->handler = explosion_animate_ADDRESS;
	explode_object();
}

void explode_object(void)
{
	uint8_t sound_id = 0;
	uint32_t write_index = 0, write_index_address = 0;

	OBJECT(current_object)->explode_object.frame_index = 0x1F;
	OBJECT(current_object)->explode_object.last_frame = 0x2B;
	OBJECT(current_object)->sprite_palette = 0x86D0u;
	OBJECT(current_object)->handler = explosion_animate_ADDRESS;
	OBJECT(current_object)->explode_object.frame_toggle = 0u;
	sound_id = 7u;
	play_sound(sound_id, &write_index, &write_index_address);
}

void explosion_animate(void)
{
	uint32_t block_offset = 0, cursor = 0, ignored = 0, slope_height = 0;
	uint16_t toggle = 0;

	unproject_object(&ignored, &block_offset, &slope_height, &cursor);
	OBJECT(current_object)->explosion.frame_toggle++;
	toggle = OBJECT(current_object)->explosion.frame_toggle & 1u;
	OBJECT(current_object)->explosion.frame_toggle = toggle;
	if (toggle != 0u)
	{
		OBJECT(current_object)->sprite_frame = explosion_frames[(uint8_t)OBJECT(current_object)->explosion.frame_index];
		queue_sprite(0u);
		return;
	}
	OBJECT(current_object)->explosion.frame_index++;
	if (OBJECT(current_object)->explosion.frame_index <= OBJECT(current_object)->explosion.last_frame)
	{
		OBJECT(current_object)->sprite_frame = explosion_frames[(uint8_t)OBJECT(current_object)->explosion.frame_index];
		queue_sprite(0u);
		return;
	}
	delete_object_and_children(&ignored, &block_offset, &slope_height);
}

void drop_rings(uint32_t shadow, uint32_t *p_allocated, uint32_t *p_random_value, uint32_t *p_ring)
{
	uint32_t allocated = *p_allocated;
	uint32_t random_value = *p_random_value;
	uint32_t ring = *p_ring;
	uint32_t count = 0, ignored = 0, new_ring = 0, random_bits = 0;

	if ((int16_t)rings_to_release <= 0)
		return;
	r14 = 0u;
	do
	{
		if (released_rings_random != 0u)
		{
			next_random(&shadow, &ignored, &ignored);
			random_value = shadow;
		}
		count = 1u;
		scatter_rings_random(shadow, count, &allocated, &ring);
		new_ring = ring;
		copy_position_to(new_ring);
		mem_write16(new_ring + 0x26u, object_cursor);
		mem_write8(new_ring + 0x44u, 0u);
		mem_write8(new_ring + 0x45u, 0u);
		mem_write16(new_ring + 0x3Eu, mem_read16(new_ring + 0x3Eu) + 1u);
		mem_write32(new_ring + 0x92u, mem_read32(new_ring + 0x3Cu));
		mem_write16(new_ring + 0x24u, released_ring_angle);
		mem_write32(new_ring + 0x6Eu, 0x60u);
		mem_write32(new_ring + 0x6Eu, mem_read32(new_ring + 0x6Eu) & 0x3FFu);
		mem_write16(new_ring + 0x84u, r14);
		mem_write32(new_ring + 0x1Cu, released_ring_wait_ADDRESS);
		r14 = (uint16_t)(r14 + 0x10u);
		if (released_rings_random != 0u)
		{
			random_bits = random_value & 0x7Fu;
			mem_write32(new_ring + 0x6Eu, mem_read32(new_ring + 0x6Eu) + random_bits);
			mem_write16(new_ring + 0x24u, mem_read16(new_ring + 0x24u) + (random_bits & 0x3Fu) * 2u);
			mem_write16(new_ring + 0x24u, mem_read16(new_ring + 0x24u) - 0x40u);
		}
		shadow = (new_ring & ~0xFFFFu) | mem_read16(new_ring + 0x28u);
		mem_write16(shadow + 0x28u, 0xFFFFu);
		if (released_ring_flip == 1u)
		{
			mem_write16(ring + 6u, mem_read16(ring + 6u) | 1u);
			mem_write16(shadow + 6u, mem_read16(shadow + 6u) | 0x2000u);
			mem_write16(ring + 6u, mem_read16(ring + 6u) ^ 0x8000u);
			mem_write16(shadow + 6u, mem_read16(shadow + 6u) ^ 0x8000u);
		}
		if (released_ring_flip == 2u)
		{
			mem_write16(ring + 6u, mem_read16(ring + 6u) | 0x4000u);
			mem_write16(shadow + 6u, mem_read16(shadow + 6u) | 0x4000u);
			mem_write16(ring + 6u, mem_read16(ring + 6u) ^ 0x8000u);
			mem_write16(shadow + 6u, mem_read16(shadow + 6u) ^ 0x8000u);
		}
		rings_to_release--;
	} while ((int16_t)rings_to_release >= 1);
	*p_allocated = allocated;
	*p_random_value = random_value;
	*p_ring = ring;
}

void released_ring_wait(void)
{
	uint8_t sound_id = 0;
	uint32_t write_index = 0, write_index_address = 0;
	uint16_t release_delay = 0;

	release_delay = OBJECT(current_object)->released_ring.release_delay;
	OBJECT(current_object)->released_ring.release_delay = release_delay - 1u;
	if (release_delay != 0u)
		return;
	sound_id = 0x13u;
	play_sound(sound_id, &write_index, &write_index_address);
	OBJECT(current_object)->released_ring.release_delay = 0u;
	OBJECT(current_object)->handler = released_ring_start_ADDRESS;
	released_ring_start();
}

void released_ring_start(void)
{
	uint16_t pickup_delay = 0;

	OBJECT(current_object)->released_ring.projected_x_carry_int = 0;
	pickup_delay = OBJECT(current_object)->released_ring.pickup_delay;
	OBJECT(current_object)->released_ring.pickup_delay = pickup_delay - 1u;
	if ((int16_t)pickup_delay > 1)
	{
		released_ring_move(0u);
		return;
	}
	OBJECT(current_object)->flags &= ~0x200u;
	OBJECT(current_object)->handler = released_ring_bounce_ADDRESS;
	released_ring_bounce();
}

void released_ring_bounce(void)
{
	uint8_t sound_id = 0;
	uint32_t r2 = 0, ignored = 0, object = 0, player = 0, slope_height = 0, write_index = 0;
	uint16_t lifetime = 0;

	lifetime = OBJECT(current_object)->released_ring.lifetime;
	OBJECT(current_object)->released_ring.lifetime = lifetime - 1u;
	if ((int16_t)lifetime > 1)
	{
		if ((OBJECT(current_object)->flags & 0x20u) != 0u)
		{
			OBJECT(current_object)->direction += 0x300u;
			OBJECT(current_object)->direction &= 0x3FFu;
			OBJECT(current_object)->speed_x = 0;
			OBJECT(current_object)->speed_y = 0;
		}
		if ((OBJECT(current_object)->flags & 4u) == 0u)
		{
			released_ring_move(player);
			return;
		}
		object = LINKED(current_object, OBJECT(current_object)->touched_by);
		if ((int8_t)OBJECT(object)->kind > 2)
		{
			sound_id = 0x13u;
			play_sound(sound_id, &write_index, &r2);
			delete_object_and_children(&ignored, &r2, &slope_height);
			return;
		}
		player = PLAYER_BLOCK(1) + ((uint32_t)OBJECT(object)->player << 9);
		if ((PLAYER(player)->state == 0xDu || PLAYER(player)->state == 0x20u) && (uint8_t)PLAYER(player)->hurt_contact_delay != 0u)
		{
			released_ring_move(player);
			return;
		}
		sound_id = 0x13u;
		play_sound(sound_id, &write_index, &r2);
		delete_object_and_children(&ignored, &r2, &slope_height);
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

void released_ring_move(uint32_t cursor)
{
	uint32_t r1 = 0, r2 = 0, r7 = 0, r8 = 0, bounce_speed = 0, direction = 0, height = 0, ignored = 0, speed = 0, speed_x = 0, speed_y = 0;

	move_object_projected(&ignored, &r2, &ignored, &cursor);
	fall_to_floor(&height, &r1, &r2);
	OBJECT(current_object)->animation_frame += 3u;
	if (height == 0u)
	{
		OBJECT(current_object)->animation_frame -= 3u;
		bounce_speed = OBJECT(current_object)->released_ring.bounce_speed;
		bounce_speed = bounce_speed - ((int32_t)bounce_speed >> 2) + 0x10000u;
		OBJECT(current_object)->released_ring.bounce_speed = bounce_speed;
		OBJECT(current_object)->speed_z = bounce_speed;
	}
	direction = OBJECT(current_object)->direction;
	speed = OBJECT(current_object)->speed;
	direction_to_speed(speed, &direction, &speed_x, &speed_y);
	OBJECT(current_object)->speed_x = speed_x;
	OBJECT(current_object)->speed_y = speed_y;
	animate(&speed, &r7, &r8);
	if (OBJECT(current_object)->animation_frame == 4u)
		spawn_ring_glint(cursor);
	queue_sprite_at_floor();
	add_to_touchable_list(direction);
	add_to_rider_list();
	draw_child_shadow();
}

void spawn_debris(uint32_t spawned, uint16_t count, uint32_t block)
{
	uint32_t r3 = 0, ignored = 0, object = 0, random_byte = 0;

	spawn_count = count;
	spawn_objects(0x08E73Au, &spawned, &r3, &block);
	if (spawned == 0u)
		return;
	r14 = r3;
	do
	{
		OBJECT(r14)->x = OBJECT(current_object)->x;
		OBJECT(r14)->y = OBJECT(current_object)->y;
		OBJECT(r14)->projected_x = OBJECT(current_object)->projected_x;
		OBJECT(r14)->projected_y = OBJECT(current_object)->projected_y;
		OBJECT(r14)->z = OBJECT(current_object)->z;
		OBJECT(r14)->z_int--;
		OBJECT(r14)->floor_z = OBJECT(current_object)->floor_z;
		OBJECT(r14)->sprite_palette = 0x8300u;
		OBJECT(r14)->var_81 = 9u;
		OBJECT(r14)->var_82 = 3u;
		OBJECT(r14)->var_84 = 0xFu;
		next_random(&r3, &ignored, &ignored);
		OBJECT(r14)->sprite_scale += r3 & 0xFFu;
		OBJECT(r14)->speed_z = -0x40000;
		random_byte = r3 & 0x3FFFFu;
		OBJECT(r14)->speed_z -= random_byte;
		OBJECT(r14)->direction = OBJECT(current_object)->direction;
		random_byte = random_byte & 0xFFu;
		OBJECT(r14)->direction += random_byte;
		OBJECT(r14)->speed = OBJECT(current_object)->speed;
		random_byte &= 0x3Fu;
		OBJECT(r14)->speed += random_byte;
		if ((random_byte & 1u) != 0u)
		{
			OBJECT(r14)->var_80 = 0xAu;
			OBJECT(r14)->var_81 = 0x13u;
		}
		OBJECT(r14)->handler = debris_fly_ADDRESS;
		object = r14;
		r14 = LINKED(r14, OBJECT(r14)->child);
		OBJECT(object)->child = 0xFFFFu;
	} while ((uint16_t)r14 != 0xFFFFu);
}

void debris_fly(void)
{
	uint32_t r1 = 0, r2 = 0, cursor = 0, deleted = 0, direction = 0, height = 0, ignored = 0, slope_height = 0, speed = 0, speed_x = 0, speed_y = 0;
	uint16_t phase = 0, lifetime = 0;

	move_object_projected(&ignored, &r2, &slope_height, &cursor);
	delete_if_off_screen(&deleted, &r2, &slope_height);
	if ((uint8_t)deleted != 0u)
		return;
	fall_to_floor(&height, &r1, &r2);
	if (height != 0u)
	{
		direction = OBJECT(current_object)->direction;
		speed = OBJECT(current_object)->speed;
		direction_to_speed(speed, &direction, &speed_x, &speed_y);
		OBJECT(current_object)->speed_x += speed_x;
		OBJECT(current_object)->speed_y += speed_y;
		OBJECT(current_object)->debris.frame_timer++;
		phase = OBJECT(current_object)->debris.frame_timer & 3u;
		OBJECT(current_object)->debris.frame_timer = phase;
		if (phase == 0u)
		{
			OBJECT(current_object)->debris.frame_index++;
			if (OBJECT(current_object)->debris.last_frame < OBJECT(current_object)->debris.frame_index)
				OBJECT(current_object)->debris.frame_index -= 9u;
			OBJECT(current_object)->sprite_frame = debris_frames[(uint8_t)OBJECT(current_object)->debris.frame_index];
		}
	}
	else
	{
		OBJECT(current_object)->speed_x = 0;
		OBJECT(current_object)->speed_y = 0;
		lifetime = OBJECT(current_object)->debris.lifetime;
		OBJECT(current_object)->debris.lifetime = lifetime - 1u;
		if (lifetime == 0u)
		{
			delete_current_object();
			return;
		}
		if ((OBJECT(current_object)->debris.lifetime & 1u) != 0u)
			return;
	}
	queue_sprite_at_floor();
}

void queue_string_record(uint32_t record, uint16_t y, uint32_t scale, uint32_t *p_palette, uint32_t *p_text)
{
	uint32_t palette = *p_palette;
	uint32_t text = *p_text;
	uint8_t mode = 0;

	mode = 0u;
	queue_string(record, y, mode, &palette, &scale, &text);
	*p_palette = (*p_palette & 0xFFFF0000u) | (palette & 0xFFFFu);
	*p_text = text;
}

void queue_string_at(uint32_t x, uint16_t y, uint32_t *p_palette, uint32_t *p_scale, uint32_t *p_text)
{
	uint32_t palette = *p_palette;
	uint32_t scale = *p_scale;
	uint32_t text = *p_text;
	uint8_t mode = 0;

	mode = 1u;
	queue_string(x, y, mode, &palette, &scale, &text);
	*p_palette = (*p_palette & 0xFFFF0000u) | (palette & 0xFFFFu);
	*p_scale = (*p_scale & 0xFFFF0000u) | (scale & 0xFFFFu);
	*p_text = text;
}

void queue_string(uint32_t x, uint16_t y, uint8_t mode, uint32_t *p_palette, uint32_t *p_scale, uint32_t *p_text)
{
	uint16_t palette = (uint16_t)*p_palette;
	uint16_t scale = (uint16_t)*p_scale;
	uint32_t text = *p_text;
	uint32_t entry = 0;

	if (string_count > 0x2A)
		return;
	if ((uint8_t)mode == 0u)
	{
		text = x;
		x = mem_read16(text);
		text += 2u;
		y = mem_read16(text);
		text += 2u;
		palette = mem_read16(text);
		text += 2u;
		scale = mem_read16(text);
		text += 2u;
	}
	entry = (uint16_t)string_count * 0xCu + string_queue_ADDRESS;
	mem_write16(entry, x);
	entry += 2u;
	mem_write16(entry, y);
	entry += 2u;
	mem_write16(entry, palette);
	entry += 2u;
	mem_write16(entry, scale);
	mem_write32(entry + 2u, text);
	string_count++;
	*p_palette = (*p_palette & 0xFFFF0000u) | palette;
	*p_scale = (*p_scale & 0xFFFF0000u) | scale;
	*p_text = text;
}

void draw_queued_strings(uint16_t size, uint16_t width, uint32_t sprite_entry, uint32_t *p_scale, uint32_t *p_cursor, uint32_t *p_height)
{
	uint32_t scale = *p_scale;
	uint16_t height = (uint16_t)*p_height;
	uint8_t shift = 0;
	uint16_t advance = 0, char_x = 0, palette = 0, string_y = 0;
	uint32_t char_code = 0, cursor = 0, frame_record = 0, leading_blanks = 0, rounding = 0, text = 0;

	for (;;)
	{
		string_count--;
		cursor = (uint16_t)string_count;
		if ((int16_t)cursor < 0)
			break;
		advance = 0u;
		leading_blanks = 0u;
		rounding = 0x80u;
		shift = 0xF8u;
		frame_record = sprite_frames;
		cursor = cursor * 0xCu + string_queue_ADDRESS;
		char_x = (uint16_t)(int16_t)mem_read16(cursor);
		cursor += 2u;
		string_y = (uint16_t)(int16_t)mem_read16(cursor);
		cursor += 2u;
		palette = mem_read16(cursor);
		cursor += 2u;
		scale = mem_read16(cursor);
		text = mem_read32(cursor + 2u);
		char_code = mem_read8(text);
		r14 = 0u;
		text++;
		do
		{
			frame_record = (frame_record & ~0xFFFFu) | text_frames[char_code];
			if ((uint16_t)frame_record != 0u)
			{
				if (r14 == 0u)
				{
					r14 = 1u;
					size = mem_read16(frame_record);
					frame_record += 2u;
					height = mem_read16(frame_record);
					width = (uint16_t)shift_by(size * scale + rounding, shift, 32);
					advance = (uint16_t)shift_by((uint8_t)height * scale + rounding, shift, 32);
					size = (uint16_t)(((uint16_t)((uint32_t)size << 6) & ~0xFFu) | (uint8_t)height);
					height = (uint16_t)((height & ~0xFFu) | advance);
					size = rotate_by(size, shift, 16);
					frame_record += 2u;
					width |= mem_read16(frame_record);
					frame_record += 2u;
					if (leading_blanks != 0u)
						char_x += leading_blanks * advance;
				}
				else
					frame_record += 6u;
				leading_blanks = 0u;
				mem_write16(sprite_entry, 0xAu);
				sprite_entry += 2u;
				mem_write16(sprite_entry, size);
				sprite_entry += 2u;
				mem_write16(sprite_entry, height);
				sprite_entry += 2u;
				mem_write16(sprite_entry, width);
				sprite_entry += 2u;
				mem_write16(sprite_entry, string_y);
				sprite_entry += 2u;
				mem_write16(sprite_entry, char_x);
				sprite_entry += 2u;
				mem_write16(sprite_entry, mem_read16(frame_record));
				sprite_entry += 2u;
				mem_write16(sprite_entry, palette);
				sprite_entry += 2u;
			}
			else
				leading_blanks++;
			char_x += advance;
			char_code = mem_read8(text);
			text++;
		} while (char_code != 0u);
	}
	string_count = 0;
	mem_write16(sprite_entry, sprite_list_end_command);
	*p_scale = scale;
	*p_cursor = cursor;
	*p_height = (*p_height & 0xFFFF0000u) | height;
}

void start_ranking_entry(void)
{
	uint8_t ranking_size = 0;
	uint16_t rings = 0;
	uint32_t block = 0, entry_x_index = 0, next_block = 0, object = 0, piece = 0, piece_copy = 0, pieces_left = 0, player = 0, previous_block = 0, record = 0, x_index = 0;

	PLAYER(current_player)->game_over_done = 0u;
	rings = PLAYER(current_player)->rings;
	r14 = 0xFu;
	ranking_size = 0xFu;
	record = ranking_ADDRESS;
	do
	{
		if ((int16_t)rings < (int16_t)mem_read16(record))
			break;
		r14 = r14 - 1u;
		record += 8u;
	} while (r14 != 0u);
	if ((int8_t)r14 < (int8_t)ranking_size)
	{
		player = OBJECT(object_cursor)->player;
		ranking_entry_players |= shl32(1u, (int)(player));
		block = ranking_entry_blocks[player];
		next_block = (block & ~0xFFFFu) | (uint16_t)(block + 0xA0u);
		mem_write16(block + 0x28u, next_block);
		object = block;
		init_object(object);
		pieces_left = 4u;
		do
		{
			piece = next_block;
			next_block = (next_block & ~0xFFFFu) | (uint16_t)(next_block + 0x30u);
			mem_write16(piece + 0x28u, next_block);
			piece_copy = piece;
			init_object_2(piece_copy);
			pieces_left--;
		} while (pieces_left != 0u);
		mem_write16(piece + 0x28u, 0xFFFFu);
		mem_write32(block + 0x10u, current_player);
		mem_write8(block + 0x47u, player);
		entry_x_index = player;
		if ((cabinet_config & 1u) == 0u)
			entry_x_index += 3u;
		mem_write16(block + 0xEu, ranking_entry_x[entry_x_index]);
		mem_write16(block + 0xCu, 0xD0u);
		mem_write32(block + 0x3Cu, ranking_entry_trackballs[player]);
		mem_write8(block + 0x80u, ranking_entry_buttons[player]);
		mem_write16(block + 0x84u, 0x4B0u);
		mem_write16(block + 0x24u, r14);
		mem_write8(block + 0x36u, OBJECT(current_object)->kind);
		mem_write16(block + 0x34u, PLAYER(current_player)->rings);
		mem_write8(block + 0x37u, PLAYER(current_player)->stages_reached);
		mem_write32(block + 0x1Cu, 0u);
		previous_block = (piece_copy & ~0xFFFFu) | (uint16_t)block;
		block = (block & ~0xFFFFu) | mem_read16(block + 0x28u);
		mem_write16(block + 4u, 0xA238u);
		mem_write16(block + 0xEu, mem_read16(previous_block + 0xEu));
		mem_write16(block + 0xCu, mem_read16(previous_block + 0xCu));
		mem_write16(block + 0xEu, mem_read16(block + 0xEu) + 0xDu);
		mem_write16(block + 6u, 0x8AD0u);
		mem_write8(block + 0x2Au, 0x80u);
		previous_block = (previous_block & ~0xFFFFu) | (uint16_t)block;
		block = (block & ~0xFFFFu) | mem_read16(block + 0x28u);
		mem_write16(block + 4u, 0xA238u);
		mem_write16(block + 0xEu, mem_read16(previous_block + 0xEu));
		mem_write16(block + 0xCu, mem_read16(previous_block + 0xCu));
		mem_write16(block + 0xEu, mem_read16(block + 0xEu) + 0xDu);
		mem_write16(block + 6u, 0x8AD0u);
		mem_write8(block + 0x2Au, 0x80u);
		previous_block = (previous_block & ~0xFFFFu) | (uint16_t)block;
		block = (block & ~0xFFFFu) | mem_read16(block + 0x28u);
		mem_write16(block + 4u, 0xA238u);
		mem_write16(block + 0xEu, mem_read16(previous_block + 0xEu));
		mem_write16(block + 0xCu, mem_read16(previous_block + 0xCu));
		mem_write16(block + 0xEu, mem_read16(block + 0xEu) + 0xDu);
		mem_write16(block + 6u, 0x8AD0u);
		mem_write8(block + 0x2Au, 0x80u);
		previous_block = (previous_block & ~0xFFFFu) | (uint16_t)block;
		block = (block & ~0xFFFFu) | mem_read16(block + 0x28u);
		mem_write16(block + 0xEu, mem_read16(previous_block + 0xEu));
		mem_write16(block + 0xCu, mem_read16(previous_block + 0xCu));
		mem_write16(block + 0xEu, mem_read16(block + 0xEu) + 0xDu);
		mem_write8(block + 0x2Du, 0x22u);
		mem_write16(block + 4u, 0xA230u);
		mem_write16(block + 6u, 0x8AD0u);
		return;
	}
	player = OBJECT(object_cursor)->player;
	ranking_entry_players |= shl32(1u, (int)(player));
	block = ranking_entry_blocks[player];
	x_index = OBJECT(object_cursor)->player;
	if ((cabinet_config & 1u) == 0u)
		x_index += 3u;
	mem_write16(block + 0xEu, ranking_entry_x[x_index]);
	mem_write16(block + 0x82u, 0x78u);
	mem_write32(block + 0x10u, current_player);
	mem_write32(block + 0x1Cu, 1u);
}

void enter_ranking_initials(uint32_t jump_offset)
{
	uint16_t abs_dx = 0, abs_dy = 0, dx = 0, dy = 0, speed_x_fraction = 0, y = 0;
	uint32_t blink_timer = 0, button_bit = 0, copy_dest = 0, copy_source = 0, count = 0, crc = 0, data = 0, dest = 0, ignored = 0, letters_left = 0, links_left = 0, object = 0, palette = 0, player = 0, read_cursor = 0, record_initials = 0, record_rings = 0, records_left = 0, scale = 0, screen_x = 0, slot = 0, speed_y = 0, text = 0, text_x = 0, trackball = 0, x = 0;
	uint16_t out_of_rank_timer = 0, time_left = 0, slot_position = 0, letter_position = 0, direction = 0;

	player = OBJECT(object_cursor)->player;
	if ((ranking_entry_players & shl32(1u, (int)(player))) == 0u)
		return;
	current_object = ranking_entry_blocks[player];
	if (OBJECT(current_object)->handler != 0u)
	{
		out_of_rank_timer = OBJECT(current_object)->var_82;
		OBJECT(current_object)->var_82 = out_of_rank_timer - 1u;
		if (out_of_rank_timer == 0u)
		{
			mem_write8(AS32(OBJECT(current_object)->hitbox_size_x) + 0x135u, 1u);
			ranking_entry_players &= ~shl32(1u, (int)(OBJECT(object_cursor)->player));
			return;
		}
		blink_timer = (jump_offset & ~0xFFFFu) | OBJECT(current_object)->var_82;
		if ((blink_timer & 3u) != 0u)
		{
			x = (blink_timer & ~0xFFFFu) | (uint16_t)OBJECT(current_object)->screen_x;
			y = 0xC8u;
			palette = 0x8F70u;
			scale = 0x100u;
			text = out_of_rank_text_ADDRESS;
			queue_string_at(x, y, &palette, &scale, &text);
		}
		return;
	}
	r14 = current_object;
	button_bit = OBJECT(current_object)->var_80;
	if ((inputs_pressed & shl32(1u, (int)(button_bit))) == 0u)
	{
	enter_ranking_initials_count_down:
		time_left = OBJECT(current_object)->var_84;
		OBJECT(current_object)->var_84 = time_left - 1u;
		if (time_left != 0u)
		{
			screen_x = (uint16_t)OBJECT(current_object)->screen_x;
			y = (uint16_t)(OBJECT(current_object)->screen_y - 0x20u);
			palette = 0x8AD0u;
			scale = 0x100u;
			text = 0x8F08Eu + (int32_t)OBJECT(current_object)->var_84 / 0x3C * 3u;
			queue_string_at(screen_x, y, &palette, &scale, &text);
			text_x = (uint16_t)OBJECT(current_object)->screen_x;
			text_x = (uint16_t)(text_x + 0x20u);
			y = (uint16_t)(OBJECT(current_object)->screen_y - 0x20u);
			palette = 0x8AD0u;
			scale = 0x100u;
			text = 0x8EFF8u + OBJECT(current_object)->direction * 0xAu;
			queue_string_at(text_x, y, &palette, &scale, &text);
			trackball = OBJECT(current_object)->speed_z;
			abs_dx = mem_read16(trackball);
			dx = (uint16_t)abs_dx;
			trackball += 2u;
			if ((int16_t)abs_dx < 0)
				abs_dx = (uint16_t)(0u - abs_dx);
			abs_dy = mem_read16(trackball);
			dy = (uint16_t)abs_dy;
			if ((int16_t)abs_dy < 0)
				abs_dy = (uint16_t)(0u - abs_dy);
			if ((int16_t)abs_dy < (int16_t)abs_dx)
			{
				slot_position = OBJECT(current_object)->spawn_arg_1 + dx;
				OBJECT(current_object)->spawn_arg_1 = slot_position;
				if ((int16_t)slot_position < 0)
					OBJECT(current_object)->spawn_arg_1 = 0u;
				if (AT8(OBJECT(current_object)->spawn_arg_2, 1) != AT8(OBJECT(current_object)->spawn_arg_1, 1))
				{
					AS8(OBJECT(current_object)->spawn_arg_1) = 0x80u;
					OBJECT(current_object)->spawn_arg_2 = OBJECT(current_object)->spawn_arg_1;
				}
			}
			links_left = AT8(OBJECT(current_object)->spawn_arg_1, 1);
			slot = links_left;
			links_left = (uint8_t)(links_left + 1u);
			if ((int8_t)links_left > 4)
			{
				AT8(OBJECT(current_object)->spawn_arg_1, 1) = 3u;
				links_left = 4u;
				slot = 3u;
			}
			do
			{
				current_object = LINKED(current_object, OBJECT(current_object)->child);
				links_left--;
			} while (links_left != 0u);
			AS8(OBJECT(current_object)->projected_y_int) = 1u;
			if (slot != 3u && dy != 0u)
			{
				letter_position = OBJECT(current_object)->projected_x + dy;
				OBJECT(current_object)->projected_x = letter_position;
				if ((int16_t)letter_position < 0)
					AT16(OBJECT(current_object)->projected_x, 1) = 0x2Bu;
				if ((int8_t)AT8(OBJECT(current_object)->projected_x, 1) > 0x2B)
					AT8(OBJECT(current_object)->projected_x, 1) = 0u;
				if (AT8(OBJECT(current_object)->projected_y, 1) != AT8(OBJECT(current_object)->projected_x, 1))
					AS8(OBJECT(current_object)->projected_x) = 0x80u;
				OBJECT(current_object)->projected_y_fraction = OBJECT(current_object)->projected_x;
				mem_write8(ranking_initial_fields[slot] + r14, AT8(OBJECT(current_object)->projected_x, 1));
			}
			current_object = ranking_entry_blocks[OBJECT(object_cursor)->player];
			letters_left = 3u;
			do
			{
				current_object = LINKED(current_object, OBJECT(current_object)->child);
				if (AS8(OBJECT(current_object)->projected_y_int) == 0u)
				{
				enter_ranking_initials_draw_letter:
					queue_sprite_on_top();
				}
				else
				{
					AS8(OBJECT(current_object)->projected_y_int) = 0u;
					OBJECT(current_object)->sprite_frame = ranking_letter_sprites[AT8(OBJECT(current_object)->projected_x, 1)];
					OBJECT(current_object)->direction++;
					if ((OBJECT(current_object)->direction & 3u) != 0u)
						goto enter_ranking_initials_draw_letter;
				}
				object = current_object;
				letters_left--;
			} while (letters_left != 0u);
			current_object = LINKED(current_object, OBJECT(current_object)->child);
			if (AS8(OBJECT(current_object)->projected_y_int) == 0u)
			{
				OBJECT(current_object)->screen_x = OBJECT(object)->screen_x;
				OBJECT(current_object)->screen_y = OBJECT(object)->screen_y;
				OBJECT(current_object)->screen_x += 0x18u;
				queue_sprite_on_top();
			}
			else
			{
				AS8(OBJECT(current_object)->projected_y_int) = 0u;
				direction = OBJECT(current_object)->direction;
				OBJECT(current_object)->direction = direction - 1u;
				if (direction != 0u)
				{
					OBJECT(current_object)->screen_x = OBJECT(object)->screen_x;
					OBJECT(current_object)->screen_y = OBJECT(object)->screen_y;
					OBJECT(current_object)->screen_x += 0x18u;
					queue_sprite_on_top();
					current_object = object_cursor;
					return;
				}
				OBJECT(current_object)->direction = 3u;
			}
			current_object = object_cursor;
			return;
		}
	enter_ranking_initials_save_record:
		mem_write8(AS32(OBJECT(current_object)->hitbox_size_x) + 0x135u, 1u);
		speed_x_fraction = OBJECT(current_object)->speed_x_fraction;
		read_cursor = ranking_ADDRESS;
		if ((int16_t)speed_x_fraction < (int16_t)AS16(ranking))
		{
			current_object = object_cursor;
			ranking_entry_players &= ~shl32(1u, (int)(OBJECT(object_cursor)->player));
			return;
		}
		dest = read_cursor;
		r14 = 0xEu;
		read_cursor += 8u;
		do
		{
			record_rings = mem_read32(read_cursor);
			read_cursor += 4u;
			if ((int16_t)speed_x_fraction < (int16_t)record_rings)
				break;
			record_initials = mem_read32(read_cursor);
			mem_write32(dest, record_rings);
			dest += 4u;
			mem_write32(dest, record_initials);
			r14 = r14 - 1u;
			read_cursor += 4u;
			dest += 4u;
		} while (r14 != 0u);
		speed_y = OBJECT(current_object)->speed_y;
		if (speed_y == 0x170412u || speed_y == 0x171717u || speed_y == 0x172B17u || speed_y == 0x2B1717u)
			OBJECT(current_object)->speed_y = 0xA0013;
		mem_write16(dest, OBJECT(current_object)->speed_x_fraction);
		dest += 2u;
		mem_write8(dest, AS8(OBJECT(current_object)->speed_x_int));
		dest++;
		mem_write8(dest, AT8(OBJECT(current_object)->speed_x_int, 1));
		dest++;
		mem_write8(dest, AS8(OBJECT(current_object)->speed_y));
		dest++;
		mem_write8(dest, AT8(OBJECT(current_object)->speed_y, 1));
		dest++;
		mem_write8(dest, AS8(OBJECT(current_object)->speed_y_int));
		mem_write8(dest + 1u, AT8(OBJECT(current_object)->speed_y_int, 1));
		copy_source = ranking_ADDRESS;
		copy_dest = saved_ranking;
		records_left = 0xFu;
		do
		{
			mem_write32(copy_dest, mem_read32(copy_source));
			copy_source += 4u;
			copy_dest += 4u;
			mem_write32(copy_dest, mem_read32(copy_source));
			records_left--;
			copy_source += 4u;
			copy_dest += 4u;
		} while (records_left != 0u);
		data = ranking_ADDRESS;
		count = 0x78u;
		compute_crc16(count, &crc, &data, &ignored);
		mem_write16(ranking_checksum, crc);
		current_object = object_cursor;
		ranking_entry_players &= ~shl32(1u, (int)(OBJECT(object_cursor)->player));
		return;
	}
	if (AT8(OBJECT(current_object)->spawn_arg_1, 1) == 3u)
		goto enter_ranking_initials_save_record;
	AT8(OBJECT(current_object)->spawn_arg_1, 1)++;
	if ((int8_t)AT8(OBJECT(current_object)->spawn_arg_1, 1) <= 3)
		goto enter_ranking_initials_count_down;
	AT8(OBJECT(current_object)->spawn_arg_1, 1) = 3u;
	goto enter_ranking_initials_count_down;
}

uint32_t load_ranking(void)
{
	uint32_t count = 0, count_2 = 0, crc = 0, data = 0, dest = 0, ignored = 0, source = 0;

	data = saved_ranking;
	count_2 = 0x78u;
	compute_crc16(count_2, &crc, &data, &ignored);
	if (mem_read16(ranking_checksum) != (uint16_t)crc)
		return reset_ranking();
	source = saved_ranking;
	dest = ranking_ADDRESS;
	count = 0xFu;
	return copy_ranking_records(source, dest, count);
}

uint32_t reset_ranking(void)
{
	uint32_t count = 0, dest = 0, letter = 0, read_cursor = 0, records_left = 0, source = 0, write_cursor = 0;
	uint8_t second_letter = 0, third_letter = 0, last_byte = 0;

	read_cursor = default_ranking_ADDRESS;
	write_cursor = ranking_ADDRESS;
	records_left = 0xFu;
	do
	{
		mem_write16(write_cursor, ROM16(read_cursor));
		write_cursor += 2u;
		read_cursor += 2u;
		mem_write16(write_cursor, ROM16(read_cursor));
		write_cursor += 2u;
		read_cursor += 2u;
		letter = ROM8(read_cursor);
		read_cursor++;
		letter = ascii_to_ranking_letter(letter);
		mem_write8(write_cursor, letter);
		second_letter = ROM8(read_cursor);
		read_cursor++;
		letter = second_letter;
		write_cursor++;
		letter = ascii_to_ranking_letter(letter);
		mem_write8(write_cursor, letter);
		third_letter = ROM8(read_cursor);
		read_cursor++;
		letter = third_letter;
		write_cursor++;
		letter = ascii_to_ranking_letter(letter);
		mem_write8(write_cursor, letter);
		last_byte = ROM8(read_cursor);
		read_cursor++;
		write_cursor++;
		mem_write8(write_cursor, last_byte);
		records_left--;
		write_cursor++;
	} while (records_left != 0u);
	source = ranking_ADDRESS;
	dest = saved_ranking;
	count = 0xFu;
	return copy_ranking_records(source, dest, count);
}

uint32_t copy_ranking_records(uint32_t source, uint32_t dest, uint32_t count)
{
	uint32_t count_2 = 0, crc = 0, data = 0, polynomial = 0;

	do
	{
		mem_write32(dest, mem_read32(source));
		source += 4u;
		dest += 4u;
		mem_write32(dest, mem_read32(source));
		count--;
		source += 4u;
		dest += 4u;
	} while (count != 0u);
	data = ranking_ADDRESS;
	count_2 = 0x78u;
	compute_crc16(count_2, &crc, &data, &polynomial);
	mem_write16(ranking_checksum, crc);
	return polynomial;
}

uint32_t ascii_to_ranking_letter(uint32_t letter)
{
	if ((int8_t)letter >= 0x61)
	{
		letter = (letter & ~0xFFu) | (uint8_t)(letter - 0x61u);
		return letter;
	}
	if ((uint8_t)letter == 0x20u)
	{
		letter = (letter & ~0xFFu) | 0x2Bu;
		return letter;
	}
	do
	{
	} while ((uint8_t)letter != 0x33u);
	letter = (letter & ~0xFFu) | 0x23u;
	return letter;
}

void start_ranking_display(uint32_t display, uint32_t *p_rows, uint32_t *p_source)
{
	uint32_t ignored = 0, object = 0, rows = 0, source = 0;

	display = allocate_object_or_start_list(display);
	OBJECT(display)->parent = object_cursor;
	OBJECT(display)->child = 0xFFFFu;
	init_object(display);
	object = display;
	show_nbg0_picture(&ignored, &rows, &source, &ignored);
	OBJECT(object)->screen_x = 0xA0;
	OBJECT(object)->screen_y = -0x1A4;
	OBJECT(object)->handler = build_ranking_display_ADDRESS;
	*p_rows = rows;
	*p_source = source;
}

void build_ranking_display(void)
{
	uint32_t block = 0, character = 0, first_initial = 0, free_count = 0, hundreds_digit = 0, label_second_char = 0, object = 0, rank_label = 0, record = 0, remainder = 0, rows_left = 0, second_initial = 0, stages = 0, tens_digit = 0, third_initial = 0, thousands_digit = 0;

	record = ranking_ADDRESS;
	current_player = 0u;
	rows_left = 0xFu;
	OBJECT(current_object)->speed_z = 0x180;
	OBJECT(current_object)->handler = ranking_display_scroll_in_ADDRESS;
	do
	{
		spawn_count = 0xCu;
		spawn_object_with_pieces(0x08F28Bu, &third_initial, &tens_digit, &free_count, &object, &block);
		OBJECT(object)->screen_x = 0xC;
		rank_label = 0x8F858u + ((current_player & ~0xFFu) | (uint8_t)(current_player * 2u));
		OBJECT(object)->sprite_frame = ranking_letter_sprites[mem_read8(rank_label)];
		OBJECT(object)->sprite_palette = 0x8AD0u;
		OBJECT(object)->spawn_arg_1 = rows_left;
		OBJECT(object)->handler = ranking_row_update_ADDRESS;
		object = LINKED(object, OBJECT(object)->child);
		OBJECT(object)->screen_x = 0x18;
		label_second_char = mem_read8(rank_label + 1u);
		OBJECT(object)->sprite_frame = ranking_letter_sprites[label_second_char];
		OBJECT(object)->sprite_palette = 0x8AD0u;
		remainder = mem_read16(record);
		remainder = remainder & 0x7FFFu;
		object = LINKED(object, OBJECT(object)->child);
		OBJECT(object)->screen_x = 0xC0;
		thousands_digit = (int32_t)remainder / 0x3E8;
		thousands_digit = (thousands_digit & ~0xFFu) | (uint8_t)(thousands_digit + 0x20u);
		OBJECT(object)->sprite_frame = ranking_letter_sprites[thousands_digit];
		OBJECT(object)->sprite_palette = 0x8AC0u;
		record += 2u;
		if ((uint8_t)thousands_digit != 0x20u)
			remainder -= ((thousands_digit & ~0xFFu) | (uint8_t)(thousands_digit - 0x20u)) * 0x3E8u;
		object = LINKED(object, OBJECT(object)->child);
		OBJECT(object)->screen_x = 0xCC;
		hundreds_digit = (int32_t)(uint16_t)remainder / 0x64;
		hundreds_digit = (hundreds_digit & ~0xFFu) | (uint8_t)(hundreds_digit + 0x20u);
		OBJECT(object)->sprite_frame = ranking_letter_sprites[hundreds_digit];
		OBJECT(object)->sprite_palette = 0x8AC0u;
		if ((uint8_t)hundreds_digit != 0x20u)
			remainder -= ((hundreds_digit & ~0xFFu) | (uint8_t)(hundreds_digit - 0x20u)) * 0x64u;
		object = LINKED(object, OBJECT(object)->child);
		OBJECT(object)->screen_x = 0xD8;
		tens_digit = (int32_t)(uint16_t)remainder / 0xA;
		tens_digit = (tens_digit & ~0xFFu) | (uint8_t)(tens_digit + 0x20u);
		OBJECT(object)->sprite_frame = ranking_letter_sprites[tens_digit];
		OBJECT(object)->sprite_palette = 0x8AC0u;
		if ((uint8_t)tens_digit != 0x20u)
			remainder -= ((tens_digit & ~0xFFu) | (uint8_t)(tens_digit - 0x20u)) * 0xAu;
		object = LINKED(object, OBJECT(object)->child);
		OBJECT(object)->screen_x = 0xE4;
		OBJECT(object)->sprite_frame = ranking_letter_sprites[(remainder & ~0xFFu) | (uint8_t)(remainder + 0x20u)];
		OBJECT(object)->sprite_palette = 0x8AC0u;
		character = mem_read8(record);
		character = character % 3u;
		object = LINKED(object, OBJECT(object)->child);
		OBJECT(object)->sprite_flags |= 0x100u;
		OBJECT(object)->screen_x = 0x68;
		OBJECT(object)->sprite_palette = ranking_portrait_palettes[character];
		AS8(OBJECT(object)->projected_y_int) = character;
		object = LINKED(object, OBJECT(object)->child);
		OBJECT(object)->screen_x = 0x96;
		OBJECT(object)->sprite_frame = ranking_name_sprites[character];
		OBJECT(object)->sprite_palette = 0x8B10u;
		record++;
		stages = mem_read8(record);
		stages = stages % 9u;
		object = LINKED(object, OBJECT(object)->child);
		OBJECT(object)->screen_x = 0x114;
		OBJECT(object)->sprite_frame = ranking_stage_sprites[(uint8_t)(stages - 1u)];
		OBJECT(object)->sprite_palette = 0x8AD0u;
		object = LINKED(object, OBJECT(object)->child);
		OBJECT(object)->screen_x = 0x30;
		record++;
		first_initial = mem_read8(record);
		OBJECT(object)->sprite_frame = ranking_letter_sprites[first_initial % 0x2Cu];
		OBJECT(object)->sprite_palette = 0x8AD0u;
		object = LINKED(object, OBJECT(object)->child);
		OBJECT(object)->screen_x = 0x3C;
		record++;
		second_initial = mem_read8(record);
		OBJECT(object)->sprite_frame = ranking_letter_sprites[second_initial % 0x2Cu];
		OBJECT(object)->sprite_palette = 0x8AD0u;
		object = LINKED(object, OBJECT(object)->child);
		OBJECT(object)->screen_x = 0x48;
		record++;
		third_initial = mem_read8(record);
		OBJECT(object)->sprite_frame = ranking_letter_sprites[third_initial % 0x2Cu];
		OBJECT(object)->sprite_palette = 0x8AD0u;
		current_player = (current_player & ~0xFFu) | (uint8_t)(current_player + 1u);
		rows_left--;
		record += 2u;
	} while (rows_left != 0u);
	spawn_count = 5u;
	spawn_object_with_pieces(0x08F482u, &third_initial, &tens_digit, &free_count, &object, &block);
	ranking_title_layout++;
	ranking_title_layout &= 1u;
	OBJECT(object)->screen_x = 0xA0;
	OBJECT(object)->screen_y = 0x70;
	OBJECT(object)->sprite_scale = 0x10u;
	OBJECT(object)->sprite_frame = 0xA2B0u;
	OBJECT(object)->sprite_palette = 0x8B20u;
	OBJECT(object)->handler = ranking_title_spin_in_ADDRESS;
	object = LINKED(object, OBJECT(object)->child);
	OBJECT(object)->sprite_frame = 0xA2A8u;
	OBJECT(object)->sprite_palette = 0x8B20u;
	object = LINKED(object, OBJECT(object)->child);
	OBJECT(object)->sprite_frame = 0xA340u;
	OBJECT(object)->sprite_palette = 0x8B20u;
	object = LINKED(object, OBJECT(object)->child);
	OBJECT(object)->sprite_frame = 0xA2D0u;
	OBJECT(object)->sprite_palette = 0x8B20u;
	object = LINKED(object, OBJECT(object)->child);
	OBJECT(object)->sprite_frame = 0xA360u;
	OBJECT(object)->sprite_palette = 0x8B20u;
}

void ranking_title_spin_in(void)
{
	uint32_t direction = 0, speed = 0, speed_x = 0, speed_y = 0;

	if ((int16_t)OBJECT(current_object)->sprite_scale < 0x200)
		OBJECT(current_object)->sprite_scale += 2u;
	OBJECT(current_object)->direction += 8u;
	OBJECT(current_object)->direction &= 0x3FFu;
	direction = OBJECT(current_object)->direction;
	speed = 0x2000u;
	direction_to_speed(speed, &direction, &speed_x, &speed_y);
	OBJECT(current_object)->speed_x = speed_x;
	OBJECT(current_object)->speed_y = speed_y;
	if ((int16_t)OBJECT(current_object)->sprite_scale < 0x200 || OBJECT(current_object)->direction != 0x300u)
	{
		draw_ranking_title();
		return;
	}
	OBJECT(current_object)->ranking_title.pause_timer = 0x1C;
	OBJECT(current_object)->handler = ranking_title_pause_ADDRESS;
	ranking_title_pause();
}

void ranking_title_pause(void)
{
	uint16_t pause_timer = 0;

	pause_timer = OBJECT(current_object)->ranking_title.pause_timer;
	OBJECT(current_object)->ranking_title.pause_timer = pause_timer - 1u;
	if (pause_timer != 0u)
	{
		draw_ranking_title();
		return;
	}
	OBJECT(current_object)->handler = ranking_title_rise_ADDRESS;
	ranking_title_rise();
}

void ranking_title_rise(void)
{
	OBJECT(current_object)->screen_y--;
	if (OBJECT(current_object)->screen_y > 0x23)
	{
		draw_ranking_title();
		return;
	}
	OBJECT(current_object)->screen_y = 0x23;
	OBJECT(current_object)->handler = draw_ranking_title_ADDRESS;
	draw_ranking_title();
}
