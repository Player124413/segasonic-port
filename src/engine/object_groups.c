/**
 *    Name:        engine/object_groups.c
 *    Description: spawn_object_groups: rows of stage objects led by group leaders; the crumbling floor rows
 *    Version:     0.1.0
 *    Created:     12-10-2026
 *    Modified:    15-10-2026
 *    Author:      Sebastian Toyos
 *
 *    SegaSonic The Hedgehog (arcade, Sega System 32): the game's V60 program decompiled into C by
 *    tools/decompile.py, with the names of symbols.txt.
 */
#include "game.h"

void spawn_object_groups(uint32_t *p_r1, uint32_t *p_free_stack, uint32_t *p_slope_z, uint32_t *p_count, uint32_t *p_object)
{
	uint32_t r1 = *p_r1;
	uint32_t free_stack = *p_free_stack;
	uint32_t slope_z = *p_slope_z;
	uint32_t count = *p_count;
	uint32_t object = *p_object;
	uint32_t floor_block_record = 0, free_count = 0, kind_index = 0, member_record = 0, record = 0, slope_number = 0, slope_record = 0, slot_cursor = 0, spawned = 0;
	uint16_t spawn_position = 0, slot_taken = 0, members_left = 0;
	uint32_t init_flags = 0;

	init_flags = stage_init_flags;
	stage_init_flags = init_flags | 0x200u;
	if ((init_flags & 0x200u) == 0u)
	{
		r1 = object_group_spawns[stage];
		group_spawn_cursor = object_group_spawns_ADDRESS + r1;
	}
	for (;;)
	{
		record = group_spawn_cursor;
		spawn_position = ROM16(record);
		record += 2u;
		if (scroll_position < spawn_position)
		{
			*p_r1 = r1;
			*p_free_stack = free_stack;
			*p_count = count;
			*p_object = object;
			return;
		}
		AS8(group_previous_member) = 0u;
		kind_index = ROM8(record) * 2u;
		record++;
		switch (kind_index)
		{
		case 0x0u:
			goto spawn_object_groups_no_depth_step;
		case 0x2u:
		case 0x10u:
			spawn_group_leader(record, &spawned, &object);
			if (spawned == 0u)
			{
				*p_r1 = r1;
				*p_free_stack = free_stack;
				*p_count = count;
				*p_object = object;
				return;
			}
			group_spawn_cursor += 0xEu;
			continue;
		case 0x4u:
			group_depth_step = -8;
			goto spawn_object_groups_build_group;
		case 0x6u:
			spawn_group_slot_trigger(record, &spawned, &count, &object);
			if (spawned == 0u)
			{
				*p_r1 = r1;
				*p_free_stack = free_stack;
				*p_count = count;
				*p_object = object;
				return;
			}
			group_spawn_cursor += 0xEu;
			continue;
		case 0x8u:
			spawn_ice_platform_leader(record, &spawned, &r1, &object);
			if (spawned == 0u)
			{
				*p_r1 = r1;
				*p_free_stack = free_stack;
				*p_count = count;
				*p_object = object;
				return;
			}
			group_spawn_cursor += 0xEu;
			continue;
		case 0xAu:
			spawn_sand_block_leader(record, &spawned, &object);
			if (spawned == 0u)
			{
				*p_r1 = r1;
				*p_free_stack = free_stack;
				*p_count = count;
				*p_object = object;
				return;
			}
			group_spawn_cursor += 0xEu;
			continue;
		case 0xCu:
			goto spawn_object_groups_mark_silent;
		case 0xEu:
			spawn_waterfall(record, &spawned, &free_stack, &object);
			if (spawned == 0u)
			{
				*p_r1 = r1;
				*p_free_stack = free_stack;
				*p_count = count;
				*p_object = object;
				return;
			}
			group_spawn_cursor += 0xEu;
			continue;
		default:
			unknown_code(0x9125Eu + kind_index);
		}
	}
spawn_object_groups_no_depth_step:
	group_depth_step = 0;
spawn_object_groups_build_group:
	if ((uint16_t)(scroll_position - mem_read16(record - 3u)) < 0x100u)
	{
		group_leader = group_leaders[ROM8(record)];
		record++;
		group_takes_slot = ROM8(record);
		record += 2u;
		group_floor_block = ROM32(record);
		record += 4u;
		group_member_count = ROM16(record);
		record += 2u;
		if (free_object_stack < group_member_count)
		{
			*p_r1 = r1;
			*p_free_stack = free_stack;
			*p_count = count;
			*p_object = object;
			return;
		}
		group_step_x = ROM16(record);
		record += 2u;
		group_step_y = ROM16(record);
		record += 2u;
		group_direction = ROM16(record);
		record += 2u;
		group_member_x = ROM16(record);
		record += 2u;
		group_member_y = ROM16(record);
		object = ROM16(record + 2u) + stage_object_records_ADDRESS;
		group_record = object;
		group_member_number = 0u;
		if (group_takes_slot != 0u)
		{
			slot_cursor = group_slots_ADDRESS;
			count = 0x40u;
			do
			{
				slot_taken = mem_read16(slot_cursor);
				slot_cursor += 2u;
				count--;
			} while ((count != 0u) & (slot_taken != 0u));
			mem_write16(slot_cursor - 2u, 1u);
			count = (count & ~0xFFFFu) | (uint16_t)(count + 1u);
			r1 = (r1 & ~0xFFFFu) | 0x40u;
			r1 = (r1 & ~0xFFFFu) | (uint16_t)(r1 - count);
			group_slot_index = r1;
		}
		do
		{
			allocate_object_outside_loop(&free_count, &object);
			if (free_count == 0u)
			{
				*p_r1 = r1;
				*p_free_stack = free_stack;
				*p_slope_z = slope_z;
				*p_count = count;
				*p_object = object;
				return;
			}
			OBJECT(object)->x_int = group_member_x;
			OBJECT(object)->x_fraction = 0u;
			group_member_x += group_step_x;
			OBJECT(object)->y_int = group_member_y;
			OBJECT(object)->y_fraction = 0u;
			group_member_y += group_step_y;
			member_record = group_record;
			OBJECT(object)->sprite_flags = ROM16(member_record);
			member_record += 2u;
			OBJECT(object)->sprite_frame = ROM16(member_record);
			member_record += 2u;
			OBJECT(object)->sprite_palette = ROM16(member_record);
			member_record += 2u;
			OBJECT(object)->handler = ROM32(member_record);
			member_record += 4u;
			OBJECT(object)->animation = ROM32(member_record);
			member_record += 4u;
			OBJECT(object)->hitbox_size_x = ROM16(member_record);
			member_record += 2u;
			OBJECT(object)->hitbox_size_y = ROM16(member_record);
			member_record += 2u;
			OBJECT(object)->sprite_offset_x = ROM8(member_record);
			member_record++;
			OBJECT(object)->sprite_offset_y = ROM8(member_record);
			OBJECT(object)->var_96 = ROM16(member_record + 1u);
			OBJECT(object)->floor_block = group_floor_block;
			OBJECT(object)->direction = group_direction;
			OBJECT(object)->parent = group_leader;
			AS16(OBJECT(object)->var_98) = OBJECT(object)->sprite_palette;
			AT8(OBJECT(object)->var_90, 1) = AS8(group_previous_member);
			AT8(OBJECT(object)->speed, 1) = group_member_number;
			group_member_number++;
			OBJECT(object)->var_8A = group_slot_index;
			AS8(OBJECT(object)->spawn_arg_1) = group_takes_slot;
			OBJECT(object)->animation_frame = 0u;
			OBJECT(object)->animation_timer = 0;
			OBJECT(object)->animation_speed = 6u;
			OBJECT(object)->floor_slope = mem_read8(floor_blocks + 2u + OBJECT(object)->floor_block + 0x13u);
			OBJECT(object)->flags = 0u;
			AS8(OBJECT(object)->speed) = 0u;
			if ((uint8_t)group_depth_step != 0u)
			{
				r1 = (r1 & ~0xFFFFu) | (uint16_t)group_depth_step;
				OBJECT(object)->var_96 += group_member_count * r1;
			}
			floor_block_record = floor_blocks + 2u + OBJECT(object)->floor_block;
			OBJECT(object)->floor_z_int = mem_read16(floor_block_record + 0x10u);
			OBJECT(object)->floor_z_fraction = 0u;
			if (mem_read8(floor_block_record + 0x13u) != 0u)
			{
				slope_number = mem_read8(floor_block_record + 0x13u);
				slope_record = floor_slopes_ADDRESS + (uint16_t)(slope_number * 2u) * 4u;
				if ((ROM32(slope_record) & 2u) == 0u)
					slope_z = OBJECT(object)->y - mem_read32(floor_block_record + 8u);
				else
					slope_z = OBJECT(object)->x - mem_read32(floor_block_record);
				slope_z = ((int32_t)slope_z >> 16) * ROM32(slope_record + 2u);
				OBJECT(object)->floor_z += slope_z;
			}
			OBJECT(object)->z = OBJECT(object)->floor_z;
			members_left = group_member_count;
			group_member_count = members_left - 1u;
		} while ((int16_t)members_left > 1);
	}
	group_spawn_cursor += 0x18u;
	*p_r1 = r1;
	*p_free_stack = free_stack;
	*p_slope_z = slope_z;
	*p_count = count;
	*p_object = object;
	return;
spawn_object_groups_mark_silent:
	AS8(group_previous_member) = 0xFFu;
	goto spawn_object_groups_no_depth_step;
}

void crumbling_floor_update(void)
{
	uint8_t sound_id = 0;
	uint32_t r1 = 0, at_last_step = 0, depth = 0, ignored = 0, sprite_depth = 0, write_index_address = 0;

	switch (OBJECT(current_object)->crumbling_floor.state)
	{
	case 0:
	case 3:
		project_object(&ignored, &r1);
		OBJECT(current_object)->crumbling_floor.state++;
	case 1:
		update_screen_position(&ignored, &r1);
		if (grey_screen_active == 0u)
		{
			if (grey_screen_ended != 0u)
				OBJECT(current_object)->sprite_palette = OBJECT(current_object)->crumbling_floor.normal_palette;
			if ((OBJECT(current_object)->flags & 8u) != 0u && (OBJECT(LINKED(current_object, OBJECT(current_object)->platform))->flags & 0x400000u) != 0u)
				OBJECT(current_object)->flags |= 0x400000u;
			if ((OBJECT(current_object)->flags & 0x400000u) != 0u)
			{
			crumbling_floor_play_crack_steps:
				animate_with_screen_offset(0u);
				if ((int8_t)OBJECT(current_object)->animation_frame >= 4)
				{
					OBJECT(current_object)->animation_timer = -1;
					if ((OBJECT(current_object)->flags & 0x400000u) != 0u)
					{
						OBJECT(current_object)->crumbling_floor.state++;
						OBJECT(current_object)->speed_z = 0;
						OBJECT(current_object)->animation_speed = 6u;
						OBJECT(current_object)->animation_timer = 0;
						OBJECT(current_object)->animation_frame = 4u;
						if (OBJECT(current_object)->crumbling_floor.silent_row == 0u)
						{
							switch (OBJECT(current_object)->direction)
							{
							case 0:
							case 4:
								if (OBJECT(current_object)->screen_x >= 0)
								{
									sound_id = 0xDu;
									play_sound(sound_id, &r1, &write_index_address);
								}
								break;
							case 1:
								if (OBJECT(current_object)->screen_x <= 0x180)
								{
									sound_id = 0xDu;
									play_sound(sound_id, &r1, &write_index_address);
									add_to_platform_list();
									if ((uint16_t)(OBJECT(current_object)->screen_x + 0xA0u) >= 0x281u)
										return;
									sprite_depth = (uint16_t)(OBJECT(current_object)->screen_y + OBJECT(current_object)->crumbling_floor.depth_offset - OBJECT(current_object)->sprite_offset_y);
									OBJECT(current_object)->crumbling_floor.draw_depth = sprite_depth;
									queue_sprite_at_depth(&ignored, &sprite_depth);
									return;
								}
								break;
							case 2:
								if (OBJECT(current_object)->screen_x >= -8)
								{
									sound_id = 0xDu;
									play_sound(sound_id, &r1, &write_index_address);
									add_to_platform_list();
									if ((uint16_t)(OBJECT(current_object)->screen_x + 0xA0u) >= 0x281u)
										return;
									sprite_depth = (uint16_t)(OBJECT(current_object)->screen_y + OBJECT(current_object)->crumbling_floor.depth_offset - OBJECT(current_object)->sprite_offset_y);
									OBJECT(current_object)->crumbling_floor.draw_depth = sprite_depth;
									queue_sprite_at_depth(&ignored, &sprite_depth);
									return;
								}
								break;
							case 3:
								if (OBJECT(current_object)->screen_x <= 0x1A0)
								{
									sound_id = 0xDu;
									play_sound(sound_id, &r1, &write_index_address);
									add_to_platform_list();
									if ((uint16_t)(OBJECT(current_object)->screen_x + 0xA0u) >= 0x281u)
										return;
									sprite_depth = (uint16_t)(OBJECT(current_object)->screen_y + OBJECT(current_object)->crumbling_floor.depth_offset - OBJECT(current_object)->sprite_offset_y);
									OBJECT(current_object)->crumbling_floor.draw_depth = sprite_depth;
									queue_sprite_at_depth(&ignored, &sprite_depth);
									return;
								}
								break;
							default:
								unknown_code(0x9161Cu + OBJECT(current_object)->direction * 2u);
							}
						}
					}
				}
			}
			else
			{
				current_player = current_object;
				current_player = LINKED(current_player, OBJECT(current_object)->parent);
				switch (OBJECT(current_object)->direction)
				{
				case 0:
				case 4:
					if (OBJECT(current_object)->y_int <= OBJECT(current_player)->y_int)
					{
						OBJECT(current_object)->crumbling_floor.state++;
						OBJECT(current_object)->speed_z = 0;
						OBJECT(current_object)->animation_speed = 6u;
						OBJECT(current_object)->animation_timer = 0;
						OBJECT(current_object)->animation_frame = 4u;
						if (OBJECT(current_object)->crumbling_floor.silent_row == 0u)
						{
							switch (OBJECT(current_object)->direction)
							{
							case 0:
							case 4:
								if (OBJECT(current_object)->screen_x >= 0)
								{
									sound_id = 0xDu;
									play_sound(sound_id, &r1, &write_index_address);
								}
								break;
							case 1:
								if (OBJECT(current_object)->screen_x <= 0x180)
								{
									sound_id = 0xDu;
									play_sound(sound_id, &r1, &write_index_address);
									add_to_platform_list();
									if ((uint16_t)(OBJECT(current_object)->screen_x + 0xA0u) >= 0x281u)
										return;
									sprite_depth = (uint16_t)(OBJECT(current_object)->screen_y + OBJECT(current_object)->crumbling_floor.depth_offset - OBJECT(current_object)->sprite_offset_y);
									OBJECT(current_object)->crumbling_floor.draw_depth = sprite_depth;
									queue_sprite_at_depth(&ignored, &sprite_depth);
									return;
								}
								break;
							case 2:
								if (OBJECT(current_object)->screen_x >= -8)
								{
									sound_id = 0xDu;
									play_sound(sound_id, &r1, &write_index_address);
									add_to_platform_list();
									if ((uint16_t)(OBJECT(current_object)->screen_x + 0xA0u) >= 0x281u)
										return;
									sprite_depth = (uint16_t)(OBJECT(current_object)->screen_y + OBJECT(current_object)->crumbling_floor.depth_offset - OBJECT(current_object)->sprite_offset_y);
									OBJECT(current_object)->crumbling_floor.draw_depth = sprite_depth;
									queue_sprite_at_depth(&ignored, &sprite_depth);
									return;
								}
								break;
							case 3:
								if (OBJECT(current_object)->screen_x <= 0x1A0)
								{
									sound_id = 0xDu;
									play_sound(sound_id, &r1, &write_index_address);
									add_to_platform_list();
									if ((uint16_t)(OBJECT(current_object)->screen_x + 0xA0u) >= 0x281u)
										return;
									sprite_depth = (uint16_t)(OBJECT(current_object)->screen_y + OBJECT(current_object)->crumbling_floor.depth_offset - OBJECT(current_object)->sprite_offset_y);
									OBJECT(current_object)->crumbling_floor.draw_depth = sprite_depth;
									queue_sprite_at_depth(&ignored, &sprite_depth);
									return;
								}
								break;
							default:
								unknown_code(0x9161Cu + OBJECT(current_object)->direction * 2u);
							}
						}
						add_to_platform_list();
						if ((uint16_t)(OBJECT(current_object)->screen_x + 0xA0u) >= 0x281u)
							return;
						sprite_depth = (uint16_t)(OBJECT(current_object)->screen_y + OBJECT(current_object)->crumbling_floor.depth_offset - OBJECT(current_object)->sprite_offset_y);
						OBJECT(current_object)->crumbling_floor.draw_depth = sprite_depth;
						queue_sprite_at_depth(&ignored, &sprite_depth);
						return;
					}
					if (OBJECT(current_object)->y_int <= (int16_t)(OBJECT(current_player)->y_int + 0x60u))
					{
					crumbling_floor_crack:
						if (OBJECT(current_object)->animation_timer >= 0)
							goto crumbling_floor_play_crack_steps;
						apply_step_screen_offset();
					}
					break;
				case 1:
					if (OBJECT(current_player)->y_int <= OBJECT(current_object)->y_int)
					{
						OBJECT(current_object)->crumbling_floor.state++;
						OBJECT(current_object)->speed_z = 0;
						OBJECT(current_object)->animation_speed = 6u;
						OBJECT(current_object)->animation_timer = 0;
						OBJECT(current_object)->animation_frame = 4u;
						if (OBJECT(current_object)->crumbling_floor.silent_row == 0u)
						{
							switch (OBJECT(current_object)->direction)
							{
							case 0:
							case 4:
								if (OBJECT(current_object)->screen_x >= 0)
								{
									sound_id = 0xDu;
									play_sound(sound_id, &r1, &write_index_address);
								}
								break;
							case 1:
								if (OBJECT(current_object)->screen_x <= 0x180)
								{
									sound_id = 0xDu;
									play_sound(sound_id, &r1, &write_index_address);
									add_to_platform_list();
									if ((uint16_t)(OBJECT(current_object)->screen_x + 0xA0u) >= 0x281u)
										return;
									sprite_depth = (uint16_t)(OBJECT(current_object)->screen_y + OBJECT(current_object)->crumbling_floor.depth_offset - OBJECT(current_object)->sprite_offset_y);
									OBJECT(current_object)->crumbling_floor.draw_depth = sprite_depth;
									queue_sprite_at_depth(&ignored, &sprite_depth);
									return;
								}
								break;
							case 2:
								if (OBJECT(current_object)->screen_x >= -8)
								{
									sound_id = 0xDu;
									play_sound(sound_id, &r1, &write_index_address);
									add_to_platform_list();
									if ((uint16_t)(OBJECT(current_object)->screen_x + 0xA0u) >= 0x281u)
										return;
									sprite_depth = (uint16_t)(OBJECT(current_object)->screen_y + OBJECT(current_object)->crumbling_floor.depth_offset - OBJECT(current_object)->sprite_offset_y);
									OBJECT(current_object)->crumbling_floor.draw_depth = sprite_depth;
									queue_sprite_at_depth(&ignored, &sprite_depth);
									return;
								}
								break;
							case 3:
								if (OBJECT(current_object)->screen_x <= 0x1A0)
								{
									sound_id = 0xDu;
									play_sound(sound_id, &r1, &write_index_address);
									add_to_platform_list();
									if ((uint16_t)(OBJECT(current_object)->screen_x + 0xA0u) >= 0x281u)
										return;
									sprite_depth = (uint16_t)(OBJECT(current_object)->screen_y + OBJECT(current_object)->crumbling_floor.depth_offset - OBJECT(current_object)->sprite_offset_y);
									OBJECT(current_object)->crumbling_floor.draw_depth = sprite_depth;
									queue_sprite_at_depth(&ignored, &sprite_depth);
									return;
								}
								break;
							default:
								unknown_code(0x9161Cu + OBJECT(current_object)->direction * 2u);
							}
						}
						add_to_platform_list();
						if ((uint16_t)(OBJECT(current_object)->screen_x + 0xA0u) >= 0x281u)
							return;
						sprite_depth = (uint16_t)(OBJECT(current_object)->screen_y + OBJECT(current_object)->crumbling_floor.depth_offset - OBJECT(current_object)->sprite_offset_y);
						OBJECT(current_object)->crumbling_floor.draw_depth = sprite_depth;
						queue_sprite_at_depth(&ignored, &sprite_depth);
						return;
					}
					if ((int16_t)(OBJECT(current_player)->y_int - 0x60u) <= OBJECT(current_object)->y_int)
						goto crumbling_floor_crack;
					break;
				case 2:
					if (OBJECT(current_object)->x_int <= OBJECT(current_player)->x_int)
					{
						OBJECT(current_object)->crumbling_floor.state++;
						OBJECT(current_object)->speed_z = 0;
						OBJECT(current_object)->animation_speed = 6u;
						OBJECT(current_object)->animation_timer = 0;
						OBJECT(current_object)->animation_frame = 4u;
						if (OBJECT(current_object)->crumbling_floor.silent_row == 0u)
						{
							switch (OBJECT(current_object)->direction)
							{
							case 0:
							case 4:
								if (OBJECT(current_object)->screen_x >= 0)
								{
									sound_id = 0xDu;
									play_sound(sound_id, &r1, &write_index_address);
								}
								break;
							case 1:
								if (OBJECT(current_object)->screen_x <= 0x180)
								{
									sound_id = 0xDu;
									play_sound(sound_id, &r1, &write_index_address);
									add_to_platform_list();
									if ((uint16_t)(OBJECT(current_object)->screen_x + 0xA0u) >= 0x281u)
										return;
									sprite_depth = (uint16_t)(OBJECT(current_object)->screen_y + OBJECT(current_object)->crumbling_floor.depth_offset - OBJECT(current_object)->sprite_offset_y);
									OBJECT(current_object)->crumbling_floor.draw_depth = sprite_depth;
									queue_sprite_at_depth(&ignored, &sprite_depth);
									return;
								}
								break;
							case 2:
								if (OBJECT(current_object)->screen_x >= -8)
								{
									sound_id = 0xDu;
									play_sound(sound_id, &r1, &write_index_address);
									add_to_platform_list();
									if ((uint16_t)(OBJECT(current_object)->screen_x + 0xA0u) >= 0x281u)
										return;
									sprite_depth = (uint16_t)(OBJECT(current_object)->screen_y + OBJECT(current_object)->crumbling_floor.depth_offset - OBJECT(current_object)->sprite_offset_y);
									OBJECT(current_object)->crumbling_floor.draw_depth = sprite_depth;
									queue_sprite_at_depth(&ignored, &sprite_depth);
									return;
								}
								break;
							case 3:
								if (OBJECT(current_object)->screen_x <= 0x1A0)
								{
									sound_id = 0xDu;
									play_sound(sound_id, &r1, &write_index_address);
									add_to_platform_list();
									if ((uint16_t)(OBJECT(current_object)->screen_x + 0xA0u) >= 0x281u)
										return;
									sprite_depth = (uint16_t)(OBJECT(current_object)->screen_y + OBJECT(current_object)->crumbling_floor.depth_offset - OBJECT(current_object)->sprite_offset_y);
									OBJECT(current_object)->crumbling_floor.draw_depth = sprite_depth;
									queue_sprite_at_depth(&ignored, &sprite_depth);
									return;
								}
								break;
							default:
								unknown_code(0x9161Cu + OBJECT(current_object)->direction * 2u);
							}
						}
						add_to_platform_list();
						if ((uint16_t)(OBJECT(current_object)->screen_x + 0xA0u) >= 0x281u)
							return;
						sprite_depth = (uint16_t)(OBJECT(current_object)->screen_y + OBJECT(current_object)->crumbling_floor.depth_offset - OBJECT(current_object)->sprite_offset_y);
						OBJECT(current_object)->crumbling_floor.draw_depth = sprite_depth;
						queue_sprite_at_depth(&ignored, &sprite_depth);
						return;
					}
					if (OBJECT(current_object)->x_int <= (int16_t)(OBJECT(current_player)->x_int + 0x60u))
						goto crumbling_floor_crack;
					break;
				case 3:
					if (OBJECT(current_player)->x_int <= OBJECT(current_object)->x_int)
					{
						OBJECT(current_object)->crumbling_floor.state++;
						OBJECT(current_object)->speed_z = 0;
						OBJECT(current_object)->animation_speed = 6u;
						OBJECT(current_object)->animation_timer = 0;
						OBJECT(current_object)->animation_frame = 4u;
						if (OBJECT(current_object)->crumbling_floor.silent_row == 0u)
						{
							switch (OBJECT(current_object)->direction)
							{
							case 0:
							case 4:
								if (OBJECT(current_object)->screen_x >= 0)
								{
									sound_id = 0xDu;
									play_sound(sound_id, &r1, &write_index_address);
								}
								break;
							case 1:
								if (OBJECT(current_object)->screen_x <= 0x180)
								{
									sound_id = 0xDu;
									play_sound(sound_id, &r1, &write_index_address);
									add_to_platform_list();
									if ((uint16_t)(OBJECT(current_object)->screen_x + 0xA0u) >= 0x281u)
										return;
									sprite_depth = (uint16_t)(OBJECT(current_object)->screen_y + OBJECT(current_object)->crumbling_floor.depth_offset - OBJECT(current_object)->sprite_offset_y);
									OBJECT(current_object)->crumbling_floor.draw_depth = sprite_depth;
									queue_sprite_at_depth(&ignored, &sprite_depth);
									return;
								}
								break;
							case 2:
								if (OBJECT(current_object)->screen_x >= -8)
								{
									sound_id = 0xDu;
									play_sound(sound_id, &r1, &write_index_address);
									add_to_platform_list();
									if ((uint16_t)(OBJECT(current_object)->screen_x + 0xA0u) >= 0x281u)
										return;
									sprite_depth = (uint16_t)(OBJECT(current_object)->screen_y + OBJECT(current_object)->crumbling_floor.depth_offset - OBJECT(current_object)->sprite_offset_y);
									OBJECT(current_object)->crumbling_floor.draw_depth = sprite_depth;
									queue_sprite_at_depth(&ignored, &sprite_depth);
									return;
								}
								break;
							case 3:
								if (OBJECT(current_object)->screen_x <= 0x1A0)
								{
									sound_id = 0xDu;
									play_sound(sound_id, &r1, &write_index_address);
									add_to_platform_list();
									if ((uint16_t)(OBJECT(current_object)->screen_x + 0xA0u) >= 0x281u)
										return;
									sprite_depth = (uint16_t)(OBJECT(current_object)->screen_y + OBJECT(current_object)->crumbling_floor.depth_offset - OBJECT(current_object)->sprite_offset_y);
									OBJECT(current_object)->crumbling_floor.draw_depth = sprite_depth;
									queue_sprite_at_depth(&ignored, &sprite_depth);
									return;
								}
								break;
							default:
								unknown_code(0x9161Cu + OBJECT(current_object)->direction * 2u);
							}
						}
						add_to_platform_list();
						if ((uint16_t)(OBJECT(current_object)->screen_x + 0xA0u) >= 0x281u)
							return;
						sprite_depth = (uint16_t)(OBJECT(current_object)->screen_y + OBJECT(current_object)->crumbling_floor.depth_offset - OBJECT(current_object)->sprite_offset_y);
						OBJECT(current_object)->crumbling_floor.draw_depth = sprite_depth;
						queue_sprite_at_depth(&ignored, &sprite_depth);
						return;
					}
					if ((int16_t)(OBJECT(current_player)->x_int - 0x60u) <= OBJECT(current_object)->x_int)
						goto crumbling_floor_crack;
					break;
				default:
					unknown_code(0x91555u + OBJECT(current_object)->direction * 2u);
				}
			}
		}
		else
			OBJECT(current_object)->sprite_palette = 0x83A0u;
		add_to_platform_list();
		if ((uint16_t)(OBJECT(current_object)->screen_x + 0xA0u) >= 0x281u)
			return;
		sprite_depth = (uint16_t)(OBJECT(current_object)->screen_y + OBJECT(current_object)->crumbling_floor.depth_offset - OBJECT(current_object)->sprite_offset_y);
		OBJECT(current_object)->crumbling_floor.draw_depth = sprite_depth;
		queue_sprite_at_depth(&ignored, &sprite_depth);
		return;
	case 2:
		OBJECT(current_object)->speed_z += 0x2000u;
		OBJECT(current_object)->z += OBJECT(current_object)->speed_z;
		project_object(&ignored, &ignored);
		if (OBJECT(current_object)->animation_timer < 0)
			apply_step_screen_offset();
		else
		{
			at_last_step = animate_with_screen_offset(0u);
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
		depth = (uint16_t)(((int32_t)((uint32_t)OBJECT(current_object)->projected_y + OBJECT(current_object)->floor_z) >> 16) - view_y + OBJECT(current_object)->crumbling_floor.depth_offset);
		queue_sprite_at_depth(&ignored, &depth);
		return;
	default:
		unknown_code(0x914DFu + OBJECT(current_object)->crumbling_floor.state * 3u);
	}
}

void spawn_group_leader(uint32_t record, uint32_t *p_spawned, uint32_t *p_leader)
{
	uint32_t leader = *p_leader;
	uint32_t free_count = 0, leader_index = 0, spawned = 0;

	allocate_object_outside_loop(&free_count, &leader);
	if (free_count == 0u)
	{
		*p_spawned = free_count;
		*p_leader = leader;
		return;
	}
	leader_index = mem_read8(record) & 3u;
	OBJECT(leader)->sprite_frame = leader_index;
	group_leaders[leader_index] = leader;
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
	OBJECT(leader)->animation = mem_read16(record + 2u) + group_leader_data_ADDRESS;
	OBJECT(leader)->handler = group_leader_move_ADDRESS;
	OBJECT(leader)->flags = 0u;
	OBJECT(leader)->sprite_offset_x = 0;
	OBJECT(leader)->sprite_offset_y = 0;
	AS8(OBJECT(leader)->speed_z_int) = 0u;
	OBJECT(leader)->var_80 = 1u;
	OBJECT(leader)->var_81 = 0u;
	if (group_leaders_1_3_held != 0u || group_leader_0_held != 0u)
		OBJECT(leader)->handler = group_leader_wait_ADDRESS;
	spawned = 0xFFFFFFFFu;
	*p_spawned = spawned;
	*p_leader = leader;
}

void group_leader_wait(void)
{
	if (OBJECT(current_object)->sprite_frame == 0u)
	{
		if (group_leader_0_held == 0u)
			OBJECT(current_object)->handler = group_leader_move_ADDRESS;
	}
	else if (group_leaders_1_3_held == 0u)
	{
		OBJECT(current_object)->handler = group_leader_move_ADDRESS;
		return;
	}
}

void group_leader_move(void)
{
	uint16_t hold_threshold = 0, speed = 0;
	uint32_t cursor = 0, ignored = 0;
	uint8_t hold_timer = 0;
	uint16_t wait_timer = 0, trigger_scroll = 0;
	uint32_t flags = 0;

	flags = OBJECT(current_object)->flags;
	OBJECT(current_object)->flags = flags | 1u;
	if ((flags & 1u) == 0u)
	{
	group_leader_read_script:
		wait_timer = OBJECT(current_object)->group_leader_move.wait_timer;
		OBJECT(current_object)->group_leader_move.wait_timer = wait_timer - 1u;
		if ((int16_t)wait_timer <= 1)
		{
			OBJECT(current_object)->group_leader_move.wait_timer = 0;
			cursor = OBJECT(current_object)->group_leader_move.path;
			trigger_scroll = mem_read16(cursor);
			cursor += 2u;
			if (trigger_scroll <= scroll_position)
			{
				if ((int16_t)mem_read16(cursor + 2u) < 0)
				{
					delete_current_object();
					return;
				}
				OBJECT(current_object)->group_leader_move.screen_threshold = mem_read16(cursor);
				cursor += 2u;
				OBJECT(current_object)->group_leader_move.catch_up_factor = mem_read16(cursor);
				cursor += 2u;
				OBJECT(current_object)->group_leader_move.base_speed = mem_read16(cursor);
				cursor += 2u;
				OBJECT(current_object)->group_leader_move.wait_timer = mem_read16(cursor);
				OBJECT(current_object)->group_leader_move.path = cursor + 2u;
			}
		}
	}
	else if (OBJECT(current_object)->group_leader_move.holding_back == 0u)
	{
		if ((stage != 8u || (current_game_mode != 0x17u && current_game_mode != 0x1Du)) && no_player_in_play == 0u && player_mishap == 0u && player_dropping_in == 0u)
			goto group_leader_read_script;
		OBJECT(current_object)->group_leader_move.holding_back = 0xFFu;
		OBJECT(current_object)->group_leader_move.saved_base_speed = OBJECT(current_object)->group_leader_move.base_speed;
		OBJECT(current_object)->group_leader_move.base_speed = 0;
		OBJECT(current_object)->group_leader_move.hold_start_position = scroll_position;
		OBJECT(current_object)->group_leader_move.hold_timer = 0xFFu;
		OBJECT(current_object)->group_leader_move.saved_threshold = OBJECT(current_object)->group_leader_move.screen_threshold;
		hold_threshold = 0xFF90u;
		if ((OBJECT(current_object)->direction & 1u) != 0u)
			hold_threshold = 0x1B0u;
		OBJECT(current_object)->group_leader_move.screen_threshold = hold_threshold;
	}
	project_object(&ignored, &ignored);
	if (OBJECT(current_object)->group_leader_move.holding_back != 0u)
	{
		if ((int16_t)(scroll_position - OBJECT(current_object)->group_leader_move.hold_start_position) >= 0x300)
			goto group_leader_stop_holding_back;
		if (no_player_in_play != 0u)
			OBJECT(current_object)->group_leader_move.hold_timer = 0xFFu;
		hold_timer = OBJECT(current_object)->group_leader_move.hold_timer;
		OBJECT(current_object)->group_leader_move.hold_timer = hold_timer - 1u;
		if (hold_timer == 0u)
			goto group_leader_stop_holding_back;
	}
	switch (OBJECT(current_object)->direction)
	{
	case 0:
	case 2:
	case 4:
		if (OBJECT(current_object)->group_leader_move.screen_threshold <= OBJECT(current_object)->screen_x)
			speed = (uint16_t)OBJECT(current_object)->group_leader_move.base_speed;
		else
		{
		group_leader_catch_up:
			OBJECT(current_object)->group_leader_move.wait_timer = 0;
			if (OBJECT(current_object)->group_leader_move.holding_back != 0u)
			{
			group_leader_stop_holding_back:
				OBJECT(current_object)->group_leader_move.holding_back = 0u;
				OBJECT(current_object)->group_leader_move.screen_threshold = OBJECT(current_object)->group_leader_move.saved_threshold;
				OBJECT(current_object)->group_leader_move.base_speed = OBJECT(current_object)->group_leader_move.saved_base_speed;
			}
			compute_camera_ground_speed(0u);
			speed = (uint16_t)((uint32_t)camera_ground_speed_int * OBJECT(current_object)->group_leader_move.catch_up_factor) >> 8;
			if ((int16_t)speed <= OBJECT(current_object)->group_leader_move.base_speed)
			{
				speed = (uint16_t)OBJECT(current_object)->group_leader_move.base_speed;
				chaser_speed = speed;
				if (OBJECT(current_object)->group_leader_move.wait_timer > 0 || grey_screen_active != 0u)
					return;
				switch (OBJECT(current_object)->direction)
				{
				case 0:
				case 4:
					OBJECT(current_object)->y += (uint32_t)chaser_speed << 12;
					break;
				case 1:
					OBJECT(current_object)->y -= (uint32_t)chaser_speed << 12;
					break;
				case 2:
					OBJECT(current_object)->x += (uint32_t)chaser_speed << 12;
					break;
				case 3:
					OBJECT(current_object)->x -= (uint32_t)chaser_speed << 12;
					break;
				default:
					unknown_code(0x918FAu + OBJECT(current_object)->direction * 2u);
				}
				return;
			}
		}
		break;
	case 1:
	case 3:
		if (OBJECT(current_object)->screen_x <= OBJECT(current_object)->group_leader_move.screen_threshold)
		{
			speed = (uint16_t)OBJECT(current_object)->group_leader_move.base_speed;
			chaser_speed = speed;
			if (OBJECT(current_object)->group_leader_move.wait_timer > 0 || grey_screen_active != 0u)
				return;
			switch (OBJECT(current_object)->direction)
			{
			case 0:
			case 4:
				OBJECT(current_object)->y += (uint32_t)chaser_speed << 12;
				break;
			case 1:
				OBJECT(current_object)->y -= (uint32_t)chaser_speed << 12;
				break;
			case 2:
				OBJECT(current_object)->x += (uint32_t)chaser_speed << 12;
				break;
			case 3:
				OBJECT(current_object)->x -= (uint32_t)chaser_speed << 12;
				break;
			default:
				unknown_code(0x918FAu + OBJECT(current_object)->direction * 2u);
			}
			return;
		}
		goto group_leader_catch_up;
	default:
		unknown_code(0x91887u + OBJECT(current_object)->direction * 2u);
	}
	chaser_speed = speed;
	if (OBJECT(current_object)->group_leader_move.wait_timer > 0 || grey_screen_active != 0u)
		return;
	switch (OBJECT(current_object)->direction)
	{
	case 0:
	case 4:
		OBJECT(current_object)->y += (uint32_t)chaser_speed << 12;
		break;
	case 1:
		OBJECT(current_object)->y -= (uint32_t)chaser_speed << 12;
		break;
	case 2:
		OBJECT(current_object)->x += (uint32_t)chaser_speed << 12;
		break;
	case 3:
		OBJECT(current_object)->x -= (uint32_t)chaser_speed << 12;
		break;
	default:
		unknown_code(0x918FAu + OBJECT(current_object)->direction * 2u);
	}
}
