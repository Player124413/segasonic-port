/**
 *    Name:        objects/river_rafts.c
 *    Description: stage 10: the river's rafts and their spawners; an unused object drawn twice between clip entries
 *    Version:     0.1.0
 *    Created:     12-10-2026
 *    Modified:    15-10-2026
 *    Author:      Sebastian Toyos
 *
 *    SegaSonic The Hedgehog (arcade, Sega System 32): the game's V60 program decompiled into C by
 *    tools/decompile.py, with the names of symbols.txt.
 */
#include "game.h"

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
