/**
 *    Name:        objects/walls_and_rocks.c
 *    Description: object types 22-27: wall pieces, shaking blocks, the sand rock, snow mound and rock wall that drop rings, Eggman teleporting (never spawned)
 *    Version:     0.1.0
 *    Created:     12-10-2026
 *    Modified:    15-10-2026
 *    Author:      Sebastian Toyos
 *
 *    SegaSonic The Hedgehog (arcade, Sega System 32): the game's V60 program decompiled into C by
 *    tools/decompile.py, with the names of symbols.txt.
 */
#include "game.h"

void wall_piece_init(void)
{
	uint32_t box_record = 0, spawn_arg_1 = 0;

	spawn_arg_1 = OBJECT(current_object)->spawn_arg_1;
	OBJECT(current_object)->sprite_frame = wall_piece_frames[spawn_arg_1];
	OBJECT(current_object)->sprite_palette = 0x9080u;
	OBJECT(current_object)->handler = wall_piece_draw_ADDRESS;
	OBJECT(current_object)->flags |= 2u;
	OBJECT(current_object)->hitbox_height = 0x28u;
	box_record = wall_piece_boxes_ADDRESS + spawn_arg_1 * 6u;
	OBJECT(current_object)->hitbox_size_x = ROM16(box_record);
	box_record += 2u;
	OBJECT(current_object)->hitbox_size_y = ROM16(box_record);
	box_record += 2u;
	OBJECT(current_object)->hitbox_offset_x = ROM8(box_record);
	OBJECT(current_object)->hitbox_offset_y = ROM8(box_record + 1u);
}

void wall_piece_draw(void)
{
	uint32_t block_offset = 0, cursor = 0, deleted = 0, depth = 0, ignored = 0, slope_height = 0;

	unproject_object(&ignored, &block_offset, &slope_height, &cursor);
	delete_if_off_screen(&deleted, &block_offset, &slope_height);
	if ((uint8_t)deleted == 1u)
		return;
	depth = (uint16_t)OBJECT(current_object)->screen_y;
	if (OBJECT(current_object)->screen_y <= 0xE0)
	{
		depth = (uint16_t)(depth + 0x14u);
		if ((int16_t)depth > 0xEE)
			depth = 0xEEu;
	}
	queue_sprite_at_depth(&ignored, &depth);
}

void shaking_block_init(void)
{
	uint32_t box_record = 0, first = 0, ignored = 0, object = 0, spawned = 0;

	OBJECT(current_object)->sprite_palette = 0x9050u;
	OBJECT(current_object)->flags |= 2u;
	OBJECT(current_object)->hitbox_height = 0x28u;
	box_record = shaking_block_boxes_ADDRESS + OBJECT(current_object)->spawn_arg_1 * 6u;
	OBJECT(current_object)->hitbox_size_x = ROM16(box_record);
	box_record += 2u;
	OBJECT(current_object)->hitbox_size_y = ROM16(box_record);
	box_record += 2u;
	OBJECT(current_object)->hitbox_offset_x = ROM8(box_record);
	OBJECT(current_object)->hitbox_offset_y = ROM8(box_record + 1u);
	spawn_count = 1u;
	add_pieces_to_current_object(&spawned, &ignored, &first, &object);
	if ((uint8_t)spawned == 0u)
		return;
	if (OBJECT(current_object)->spawn_arg_1 != 0u)
	{
		OBJECT(object)->sprite_frame = 0x7AA0u;
		OBJECT(object)->sprite_palette = 0x9050u;
	}
	else
	{
		OBJECT(object)->sprite_frame = 0x7570u;
		OBJECT(object)->sprite_palette = 0x1060u;
	}
	OBJECT(current_object)->handler = shaking_block_animate_ADDRESS;
}

void shaking_block_animate(void)
{
	uint8_t bucket = 0;
	uint16_t offset_x = 0, offset_y = 0;
	uint32_t block_offset = 0, bucket_2 = 0, cursor = 0, deleted = 0, ignored = 0, slope_height = 0;
	uint16_t frame_timer = 0;

	unproject_object(&ignored, &block_offset, &slope_height, &cursor);
	delete_if_off_screen(&deleted, &block_offset, &slope_height);
	if ((uint8_t)deleted == 1u)
		return;
	frame_timer = OBJECT(current_object)->shaking_block.frame_timer;
	OBJECT(current_object)->shaking_block.frame_timer = frame_timer - 1u;
	if (frame_timer == 0u)
	{
		OBJECT(current_object)->shaking_block.frame_timer = 2u;
		OBJECT(current_object)->sprite_frame = shaking_block_frames[(uint8_t)OBJECT(current_object)->shaking_block.frame_index + OBJECT(current_object)->spawn_arg_1 * 3u];
		OBJECT(current_object)->shaking_block.frame_index++;
		if (OBJECT(current_object)->shaking_block.frame_index > 2)
			OBJECT(current_object)->shaking_block.frame_index = 0;
	}
	if (OBJECT(current_object)->spawn_arg_1 == 0u)
	{
		bucket = 0xFFu;
		queue_sprite_in_bucket(bucket);
		current_object = LINKED(current_object, OBJECT(current_object)->child);
		bucket_2 = place_sprite_at_cursor_floor();
		OBJECT(current_object)->screen_x -= 0x10u;
		OBJECT(current_object)->screen_y += 7u;
		queue_sprite_in_bucket(bucket_2);
	}
	else
	{
		bucket = 0u;
		queue_sprite_in_bucket(bucket);
		current_object = LINKED(current_object, OBJECT(current_object)->child);
		offset_x = 0xFFD8u;
		offset_y = 0x20u;
		place_sprite_near_cursor(offset_x, offset_y);
		bucket = 0u;
		queue_sprite_in_bucket(bucket);
	}
}

void sand_rock_init(void)
{
	OBJECT(current_object)->sprite_flags = 0u;
	OBJECT(current_object)->sprite_frame = 0x7BA8u;
	OBJECT(current_object)->sprite_palette = 0x9010u;
	OBJECT(current_object)->flags |= 2u;
	OBJECT(current_object)->hitbox_size_x = 0x22u;
	OBJECT(current_object)->hitbox_size_y = 0x21u;
	OBJECT(current_object)->hitbox_offset_x = -0x21;
	OBJECT(current_object)->hitbox_offset_y = 3;
	OBJECT(current_object)->hitbox_height = 0x20u;
	OBJECT(current_object)->kind = 5u;
	OBJECT(current_object)->handler = sand_rock_update_ADDRESS;
	current_object = LINKED(current_object, OBJECT(current_object)->child);
	OBJECT(current_object)->sprite_flags = 0u;
	OBJECT(current_object)->sprite_palette = 0x9010u;
}

void sand_rock_update(void)
{
	uint32_t r1 = 0, r2 = 0, r3 = 0, r11 = 0, crumble_frame = 0, deleted = 0, depth = 0, ignored = 0, random_value = 0, ring = 0;

	unproject_object(&ignored, &r2, &r3, &r11);
	fall_to_floor(&ignored, &r1, &r2);
	delete_if_off_screen(&deleted, &r2, &r3);
	if ((uint8_t)deleted == 1u)
		return;
	if (OBJECT(current_object)->sand_rock.crumbling != 0u)
	{
	sand_rock_crumble:
		OBJECT(current_object)->sand_rock.frame_count++;
		if (OBJECT(current_object)->sand_rock.frame_count <= 1)
		{
			depth = (uint16_t)(OBJECT(current_object)->screen_y + 0x14u);
			queue_sprite_at_depth(&ignored, &depth);
			if ((uint8_t)OBJECT(current_object)->sand_rock.crumble_frame == 0u)
				add_to_touchable_list(ignored);
			return;
		}
		if (OBJECT(current_object)->sand_rock.crumble_frame <= 0xD)
		{
			crumble_frame = (uint8_t)OBJECT(current_object)->sand_rock.crumble_frame;
			OBJECT(current_object)->sprite_frame = sand_rock_crumble_frames[crumble_frame];
			OBJECT(current_object)->sand_rock.frame_count = 0;
			OBJECT(current_object)->sand_rock.crumble_frame++;
			crumble_frame++;
			if ((int8_t)crumble_frame > 0xD)
			{
				depth = (uint16_t)(OBJECT(current_object)->screen_y + 0x14u);
				queue_sprite_at_depth(&ignored, &depth);
				if ((uint8_t)OBJECT(current_object)->sand_rock.crumble_frame == 0u)
					add_to_touchable_list(ignored);
				return;
			}
			OBJECT(LINKED(WORK_RAM, OBJECT(current_object)->child))->sprite_frame = sand_rock_crumble_frames[crumble_frame];
			depth = (uint16_t)(OBJECT(current_object)->screen_y + 0x14u);
			queue_sprite_at_depth(&ignored, &depth);
			if ((uint8_t)OBJECT(current_object)->sand_rock.crumble_frame == 0u)
				add_to_touchable_list(ignored);
			return;
		}
		OBJECT(current_object)->sand_rock.frame_count = 0;
		OBJECT(current_object)->sand_rock.crumble_frame = 0;
		OBJECT(current_object)->sand_rock.crumbling = 0u;
		OBJECT(current_object)->sprite_frame = sand_rock_crumble_frames[(uint8_t)OBJECT(current_object)->sand_rock.crumble_frame];
		delete_object_and_children(&ignored, &r2, &r3);
		return;
	}
	if ((OBJECT(current_object)->flags & 4u) == 0u)
	{
		depth = (uint16_t)(OBJECT(current_object)->screen_y + 0x14u);
		queue_sprite_at_depth(&ignored, &depth);
		if ((uint8_t)OBJECT(current_object)->sand_rock.crumble_frame == 0u)
			add_to_touchable_list(ignored);
		return;
	}
	OBJECT(current_object)->sand_rock.crumbling = 1u;
	if (OBJECT(current_object)->sand_rock.ring_count == 0u)
		goto sand_rock_crumble;
	released_ring_flip = 0u;
	rings_to_release = OBJECT(current_object)->sand_rock.ring_count;
	released_ring_angle = OBJECT(current_object)->spawn_arg_2;
	drop_rings(r3, &r11, &random_value, &ring);
	depth = (uint16_t)(OBJECT(current_object)->screen_y + 0x14u);
	queue_sprite_at_depth(&ignored, &depth);
	if ((uint8_t)OBJECT(current_object)->sand_rock.crumble_frame == 0u)
		add_to_touchable_list(ignored);
}

void sand_rock_draw_piece(void)
{
	uint32_t depth = 0, ignored = 0;

	current_object = LINKED(current_object, OBJECT(object_cursor)->child);
	OBJECT(current_object)->screen_x = OBJECT(object_cursor)->screen_x;
	OBJECT(current_object)->screen_y = OBJECT(object_cursor)->screen_y;
	depth = (uint16_t)(OBJECT(current_object)->screen_y + 0x1Fu);
	queue_sprite_at_depth(&ignored, &depth);
}

void snow_mound_init(void)
{
	OBJECT(current_object)->projected_y_int += 0x20u;
	add_to_rider_list();
	OBJECT(current_object)->sprite_flags = 0u;
	OBJECT(current_object)->sprite_frame = 0x7B30u;
	OBJECT(current_object)->sprite_palette = 0x91B0u;
	OBJECT(current_object)->flags |= 2u;
	OBJECT(current_object)->hitbox_size_x = 0x22u;
	OBJECT(current_object)->hitbox_size_y = 0x21u;
	OBJECT(current_object)->hitbox_offset_x = -0x21;
	OBJECT(current_object)->hitbox_offset_y = 3;
	OBJECT(current_object)->hitbox_height = 0x20u;
	OBJECT(current_object)->kind = 5u;
	OBJECT(current_object)->snow_mound.part_3_frame = 4u;
	OBJECT(current_object)->handler = snow_mound_update_ADDRESS;
	current_object = LINKED(current_object, OBJECT(current_object)->child);
	OBJECT(current_object)->sprite_flags = 0u;
	OBJECT(current_object)->sprite_palette = 0x91B0u;
	current_object = LINKED(current_object, OBJECT(current_object)->child);
	OBJECT(current_object)->sprite_flags = 0u;
	OBJECT(current_object)->sprite_flags |= 0x40u;
	OBJECT(current_object)->sprite_palette = 0x91B0u;
	current_object = LINKED(current_object, OBJECT(current_object)->child);
	OBJECT(current_object)->sprite_flags = 0u;
	OBJECT(current_object)->sprite_flags |= 0x80u;
	OBJECT(current_object)->sprite_palette = 0x11C0u;
	OBJECT(current_object)->sprite_palette |= 0x2000u;
}

void snow_mound_update(void)
{
	uint8_t bucket = 0;
	uint16_t offset_x = 0, offset_y = 0;
	uint32_t r2 = 0, r3 = 0, r11 = 0, burst_frame = 0, deleted = 0, depth = 0, ignored = 0, object = 0, object_2 = 0, random_value = 0, ring = 0;

	move_object_projected(&ignored, &r2, &r3, &r11);
	fall_to_floor(&ignored, &depth, &r2);
	delete_if_off_screen(&deleted, &r2, &r3);
	if ((uint8_t)deleted == 1u)
		return;
	if (OBJECT(current_object)->snow_mound.bursting != 0u)
	{
	snow_mound_burst:
		OBJECT(current_object)->snow_mound.frame_count++;
		OBJECT(current_object)->snow_mound.part_3_count++;
		if (OBJECT(current_object)->snow_mound.frame_count <= 1)
		{
		snow_mound_draw:
			depth = (depth & ~0xFFFFu) | (uint16_t)OBJECT(current_object)->screen_y;
			depth = (depth & ~0xFFFFu) | (uint16_t)(depth + 0x12u);
			queue_sprite_at_depth(&ignored, &depth);
			if ((uint8_t)OBJECT(current_object)->snow_mound.burst_frame != 0u)
			{
				current_object = LINKED(current_object, OBJECT(object_cursor)->child);
				OBJECT(current_object)->screen_x = OBJECT(object_cursor)->screen_x;
				OBJECT(current_object)->screen_y = OBJECT(object_cursor)->screen_y;
				depth = (depth & ~0xFFFFu) | (uint16_t)OBJECT(current_object)->screen_y;
				depth = (depth & ~0xFFFFu) | (uint16_t)(depth + 0x11u);
				queue_sprite_at_depth(&ignored, &depth);
				current_object = LINKED(current_object, OBJECT(object_cursor)->child);
				current_object = LINKED(current_object, OBJECT(current_object)->child);
				OBJECT(current_object)->screen_x = OBJECT(object_cursor)->screen_x;
				OBJECT(current_object)->screen_y = OBJECT(object_cursor)->screen_y;
				OBJECT(current_object)->screen_y -= 0x16u;
				queue_sprite(depth);
			}
			else
			{
				add_to_touchable_list(ignored);
				add_to_rider_list();
			}
			current_object = LINKED(current_object, OBJECT(object_cursor)->child);
			object = current_object;
			current_object = LINKED(current_object, OBJECT(current_object)->child);
			current_object = LINKED(current_object, OBJECT(current_object)->child);
			OBJECT(current_object)->sprite_frame = OBJECT(object_cursor)->sprite_frame;
			if ((uint8_t)OBJECT(object_cursor)->snow_mound.burst_frame != 0u)
				OBJECT(current_object)->sprite_frame = OBJECT(object)->sprite_frame;
			offset_x = 0u;
			offset_y = 0x15u;
			place_sprite_near_cursor(offset_x, offset_y);
			bucket = 0xFFu;
			queue_sprite_in_bucket(bucket);
			return;
		}
		if (OBJECT(current_object)->snow_mound.burst_frame <= 0xE)
		{
			burst_frame = (uint8_t)OBJECT(current_object)->snow_mound.burst_frame;
			OBJECT(current_object)->sprite_frame = snow_mound_burst_frames[burst_frame];
			OBJECT(current_object)->snow_mound.frame_count = 0;
			OBJECT(current_object)->snow_mound.burst_frame++;
			burst_frame += 2u;
			if ((int8_t)burst_frame > 0xE)
				goto snow_mound_draw;
			object_2 = LINKED(WORK_RAM, OBJECT(current_object)->child);
			OBJECT(object_2)->sprite_frame = snow_mound_burst_frames[burst_frame];
			if (OBJECT(current_object)->snow_mound.part_3_count <= 2)
				goto snow_mound_draw;
			object_2 = LINKED(object_2, OBJECT(current_object)->child);
			object_2 = LINKED(object_2, OBJECT(object_2)->child);
			OBJECT(current_object)->snow_mound.part_3_count = 0;
			OBJECT(current_object)->snow_mound.part_3_frame++;
			OBJECT(object_2)->sprite_frame = snow_mound_burst_frames[OBJECT(current_object)->snow_mound.part_3_frame];
			goto snow_mound_draw;
		}
		OBJECT(current_object)->snow_mound.frame_count = 0;
		OBJECT(current_object)->snow_mound.part_3_count = 0;
		OBJECT(current_object)->snow_mound.burst_frame = 0;
		OBJECT(current_object)->snow_mound.bursting = 0u;
		OBJECT(current_object)->snow_mound.part_3_frame = 4u;
		OBJECT(current_object)->sprite_frame = snow_mound_burst_frames[(uint8_t)OBJECT(current_object)->snow_mound.burst_frame];
		delete_object_and_children(&ignored, &r2, &r3);
		return;
	}
	if ((OBJECT(current_object)->flags & 4u) == 0u)
		goto snow_mound_draw;
	OBJECT(current_object)->snow_mound.bursting = 1u;
	if (OBJECT(current_object)->snow_mound.ring_count == 0u)
		goto snow_mound_burst;
	released_ring_flip = 0u;
	rings_to_release = OBJECT(current_object)->snow_mound.ring_count;
	released_ring_angle = OBJECT(current_object)->spawn_arg_2;
	released_rings_random = OBJECT(current_object)->snow_mound.random_scatter;
	drop_rings(r3, &r11, &random_value, &ring);
	goto snow_mound_burst;
}

void rock_wall_init(void)
{
	OBJECT(current_object)->sprite_frame = 0x7C18u;
	OBJECT(current_object)->sprite_palette = 0x87C0u;
	OBJECT(current_object)->flags |= 2u;
	OBJECT(current_object)->hitbox_size_x = 0x18u;
	OBJECT(current_object)->hitbox_size_y = 0x4Du;
	OBJECT(current_object)->hitbox_offset_x = 9;
	OBJECT(current_object)->hitbox_offset_y = -0x39;
	OBJECT(current_object)->hitbox_height = 0xB4u;
	OBJECT(current_object)->kind = 0xCu;
	OBJECT(current_object)->rock_wall.frame_count = 2;
	OBJECT(current_object)->handler = rock_wall_update_ADDRESS;
	current_object = LINKED(current_object, OBJECT(current_object)->child);
	OBJECT(current_object)->sprite_palette = 0x8A10u;
	current_object = LINKED(current_object, OBJECT(current_object)->child);
	OBJECT(current_object)->sprite_palette = 0x8A10u;
}

void rock_wall_update(void)
{
	uint8_t bucket = 0;
	uint16_t offset_x = 0, offset_y = 0;
	uint32_t r3 = 0, r11 = 0, block_offset = 0, deleted = 0, depth = 0, ignored = 0, random_value = 0, ring = 0;
	uint16_t twinkle_delay = 0, direction = 0, direction_2 = 0;

	unproject_object(&ignored, &block_offset, &r3, &r11);
	delete_if_off_screen(&deleted, &block_offset, &r3);
	if ((uint8_t)deleted == 1u)
		return;
	if (OBJECT(current_object)->rock_wall.breaking == 0u)
	{
		if ((OBJECT(current_object)->flags & 4u) == 0u || (OBJECT(LINKED(WORK_RAM, OBJECT(current_object)->touched_by))->flags & 0x40u) == 0u)
			goto rock_wall_keep_touchable;
		OBJECT(current_object)->rock_wall.breaking = 1u;
	}
	if (OBJECT(current_object)->rock_wall.break_frame < 0xD)
	{
		OBJECT(current_object)->rock_wall.frame_count++;
		if (OBJECT(current_object)->rock_wall.frame_count > 2)
		{
			OBJECT(current_object)->rock_wall.break_frame++;
			OBJECT(current_object)->sprite_frame = rock_wall_break_frames[(uint8_t)OBJECT(current_object)->rock_wall.break_frame];
			OBJECT(current_object)->rock_wall.frame_count = 0;
			if ((uint8_t)OBJECT(current_object)->rock_wall.break_frame == 5u && OBJECT(current_object)->spawn_arg_1 != 0u)
			{
				rings_to_release = OBJECT(current_object)->spawn_arg_1;
				released_ring_angle = OBJECT(current_object)->spawn_arg_2;
				released_ring_flip = 0u;
				released_rings_random = 0u;
				drop_rings(r3, &r11, &random_value, &ring);
			}
		}
	rock_wall_keep_touchable:
		add_to_touchable_list(deleted);
	}
	bucket = 0xFFu;
	queue_sprite_in_bucket(bucket);
	if (OBJECT(current_object)->rock_wall.break_frame < 1)
	{
		twinkle_delay = OBJECT(current_object)->rock_wall.twinkle_delay;
		OBJECT(current_object)->rock_wall.twinkle_delay = twinkle_delay - 1u;
		if (twinkle_delay != 0u)
			return;
		OBJECT(current_object)->rock_wall.twinkle_delay = 0u;
		current_object = LINKED(current_object, OBJECT(current_object)->child);
		direction = OBJECT(current_object)->direction;
		OBJECT(current_object)->direction = direction - 1u;
		if (direction == 0u)
		{
			OBJECT(current_object)->direction = 2u;
			OBJECT(current_object)->sprite_frame = rock_wall_sparkle_frames[AT8(OBJECT(current_object)->projected_y, 1)];
			AT8(OBJECT(current_object)->projected_y, 1)++;
			if ((int8_t)AT8(OBJECT(current_object)->projected_y, 1) > 0x19)
				AT8(OBJECT(current_object)->projected_y, 1) = 0u;
		}
		offset_x = 8u;
		offset_y = 0xFFEBu;
		place_sprite_near_cursor(offset_x, offset_y);
		depth = (uint16_t)(OBJECT(object_cursor)->screen_y - 1u);
		queue_sprite_at_depth(&ignored, &depth);
		current_object = LINKED(current_object, OBJECT(current_object)->child);
		direction_2 = OBJECT(current_object)->direction;
		OBJECT(current_object)->direction = direction_2 - 1u;
		if (direction_2 == 0u)
		{
			OBJECT(current_object)->direction = 4u;
			OBJECT(current_object)->sprite_frame = rock_wall_glint_frames[rock_wall_glint_steps[AT8(OBJECT(current_object)->projected_y, 1)]];
			AT8(OBJECT(current_object)->projected_y, 1)++;
			if ((int8_t)AT8(OBJECT(current_object)->projected_y, 1) > 0x13)
			{
				AT8(OBJECT(current_object)->projected_y, 1) = 0u;
				OBJECT(object_cursor)->rock_wall.twinkle_delay = 0x1Eu;
			}
		}
		offset_x = 5u;
		offset_y = 0xFFD1u;
		place_sprite_near_cursor(offset_x, offset_y);
		depth = (uint16_t)(OBJECT(object_cursor)->screen_y - 1u);
		queue_sprite_at_depth(&ignored, &depth);
	}
}

void spawn_eggman_teleport(uint32_t free_count, uint32_t teleport, uint32_t block)
{
	uint32_t piece_count = 0, spawned = 0;

	spawn_count = 2u;
	spawn_object_with_pieces(0x013434u, &spawned, &piece_count, &free_count, &teleport, &block);
	OBJECT(teleport)->handler = eggman_teleport_init_ADDRESS;
}

void eggman_teleport_init(void)
{
	OBJECT(current_object)->sprite_scale = 0x180u;
	OBJECT(current_object)->sprite_flags |= 0x100u;
	OBJECT(current_object)->sprite_palette = 0x8220u;
	OBJECT(current_object)->handler = eggman_teleport_animate_ADDRESS;
	current_object = LINKED(current_object, OBJECT(current_object)->child);
	OBJECT(current_object)->sprite_frame = 0x79A8u;
}

void eggman_teleport_animate(void)
{
	uint32_t block_offset = 0, cursor = 0, depth = 0, frame_index = 0, ignored = 0, slope_height = 0;
	uint16_t step_timer = 0, odd_frame = 0, flicker_step = 0;

	unproject_object(&ignored, &block_offset, &slope_height, &cursor);
	step_timer = OBJECT(current_object)->eggman_teleport.step_timer;
	OBJECT(current_object)->eggman_teleport.step_timer = step_timer - 1u;
	if (step_timer == 0u)
	{
		frame_index = eggman_teleport_steps[OBJECT(current_object)->eggman_teleport.step];
		OBJECT(current_object)->eggman_teleport.frame_index = frame_index;
		if (frame_index == 0xFFu)
		{
			delete_object_and_children(&ignored, &block_offset, &slope_height);
			return;
		}
		OBJECT(current_object)->sprite_frame = eggman_teleport_frames[frame_index];
		OBJECT(current_object)->eggman_teleport.step_timer = eggman_teleport_durations[frame_index];
		OBJECT(current_object)->eggman_teleport.step++;
	}
	OBJECT(current_object)->eggman_teleport.beam_phase++;
	odd_frame = OBJECT(current_object)->eggman_teleport.beam_phase & 1u;
	OBJECT(current_object)->eggman_teleport.beam_phase = odd_frame;
	if (odd_frame != 0u)
	{
		current_object = LINKED(current_object, OBJECT(current_object)->child);
		place_sprite_at_cursor_floor();
		OBJECT(current_object)->sprite_palette = 0u;
		OBJECT(current_object)->screen_x += 8u;
		depth = (uint16_t)(OBJECT(current_object)->screen_y - 2u);
		queue_sprite_at_depth(&ignored, &depth);
		current_object = object_cursor;
	}
	OBJECT(current_object)->eggman_teleport.flicker_phase++;
	flicker_step = OBJECT(current_object)->eggman_teleport.flicker_phase & 3u;
	OBJECT(current_object)->eggman_teleport.flicker_phase = flicker_step;
	if (flicker_step != 0u)
		queue_sprite_at_floor();
}
