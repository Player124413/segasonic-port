/**
 *    Name:        objects/rope_bridge.c
 *    Description: spawn_object_groups_2 and stage 10's rope bridge
 *    Version:     0.1.0
 *    Created:     12-10-2026
 *    Modified:    15-10-2026
 *    Author:      Sebastian Toyos
 *
 *    SegaSonic The Hedgehog (arcade, Sega System 32): the game's V60 program decompiled into C by
 *    tools/decompile.py, with the names of symbols.txt.
 */
#include "game.h"

void spawn_object_groups_2(uint32_t *p_list_offset, uint32_t *p_count, uint32_t *p_object)
{
	uint32_t list_offset = *p_list_offset;
	uint32_t count = *p_count;
	uint32_t object = *p_object;
	uint8_t bridge_number = 0;
	uint32_t fill_word = 0, floor_block_record = 0, floor_type_record = 0, ignored = 0, kind_index = 0, last_plank_var = 0, member_record = 0, plank_count_var = 0, record = 0, slot_cursor = 0, spawned = 0;
	uint8_t slot_taken = 0;
	uint16_t spawn_position = 0, members_left = 0;
	uint32_t init_flags = 0;

	init_flags = stage_init_flags;
	stage_init_flags = init_flags | 0x10u;
	if ((init_flags & 0x10u) == 0u)
	{
		unused_group_spawn_word_2 = 0u;
		unused_group_spawn_word = 0u;
		unused_group_spawn_word_3 = 1u;
		list_offset = stage_record_spawns[stage];
		group_spawn_cursor_2 = stage_record_spawns_ADDRESS + list_offset;
		bridge_0_plank_count = 0u;
		bridge_1_plank_count = 0u;
		slot_cursor = group_slots_ADDRESS;
		count = 0x10u;
		fill_word = 0u;
		do
		{
			mem_write32(slot_cursor, fill_word);
			slot_cursor += 4u;
			count--;
		} while (count != 0u);
	}
	for (;;)
	{
		record = group_spawn_cursor_2;
		spawn_position = ROM16(record);
		record += 2u;
		if (scroll_position < spawn_position)
		{
			*p_list_offset = list_offset;
			*p_count = count;
			*p_object = object;
			return;
		}
		kind_index = ROM8(record) * 2u;
		record++;
		switch (kind_index)
		{
		case 0x0u:
			goto spawn_object_groups_2_pick_bridge;
		case 0x2u:
		case 0x8u:
			spawn_group_leader(record, &spawned, &object);
			if (spawned == 0u)
			{
				*p_list_offset = list_offset;
				*p_count = count;
				*p_object = object;
				return;
			}
			group_spawn_cursor_2 += 0xEu;
			continue;
		case 0x4u:
			group_bridge_number = 0u;
			plank_count_var = bridge_0_plank_count_ADDRESS;
			goto spawn_object_groups_2_build_group;
		case 0x6u:
			group_bridge_number = 1u;
			plank_count_var = bridge_1_plank_count_ADDRESS;
			goto spawn_object_groups_2_build_group;
		default:
			unknown_code(0x938A6u + kind_index);
		}
	}
spawn_object_groups_2_pick_bridge:
	if (free_object_stack < (int16_t)ROM16(record + 5u))
	{
		*p_list_offset = list_offset;
		*p_count = count;
		*p_object = object;
		return;
	}
	bridge_number = 0u;
	plank_count_var = bridge_0_plank_count_ADDRESS;
	if (bridge_0_plank_count != 0u)
	{
		bridge_number = 1u;
		plank_count_var = bridge_1_plank_count_ADDRESS;
		if (bridge_1_plank_count != 0u)
		{
			*p_list_offset = list_offset;
			*p_count = count;
			*p_object = object;
			return;
		}
	}
	group_bridge_number = bridge_number;
	mem_write16(plank_count_var, mem_read16(plank_count_var) + 1u);
	group_member_number = 0u;
spawn_object_groups_2_build_group:
	if ((uint16_t)(scroll_position - mem_read16(record - 3u)) < 0x100u)
	{
		group_leader = group_leaders[ROM8(record)];
		record++;
		group_takes_slot = ROM8(record);
		record += 2u;
		group_floor_block = ROM32(record);
		record += 4u;
		group_member_count = ROM16(record);
		group_previous_member = 0xFFFFu;
		mem_write16(plank_count_var, mem_read16(plank_count_var) + group_member_count);
		record += 2u;
		group_step_x = ROM16(record);
		record += 2u;
		group_step_y = ROM16(record);
		record += 2u;
		group_direction = ROM16(record);
		record += 2u;
		group_member_x = ROM16(record);
		record += 2u;
		group_member_y = ROM16(record);
		group_record = ROM16(record + 2u);
		if (group_takes_slot != 0u)
		{
			slot_cursor = group_slots_ADDRESS;
			count = 0x40u;
			do
			{
				slot_taken = mem_read8(slot_cursor);
				slot_cursor++;
				count--;
			} while ((count != 0u) & (slot_taken != 0u));
			mem_write8(slot_cursor - 1u, 1u);
			count = (count & ~0xFFFFu) | (uint16_t)(count + 1u);
			group_slot_index = 0x40u - count;
		}
		do
		{
			allocate_object_outside_loop(&ignored, &object);
			OBJECT(object)->x_int = group_member_x;
			OBJECT(object)->x_fraction = 0u;
			group_member_x += group_step_x;
			OBJECT(object)->y_int = group_member_y;
			OBJECT(object)->y_fraction = 0u;
			group_member_y += group_step_y;
			member_record = group_record;
			OBJECT(object)->kind = member_record;
			member_record += 0x93F59u;
			OBJECT(object)->sprite_flags = mem_read16(member_record);
			member_record += 2u;
			OBJECT(object)->sprite_frame = mem_read16(member_record);
			member_record += 2u;
			OBJECT(object)->sprite_palette = mem_read16(member_record);
			member_record += 2u;
			OBJECT(object)->handler = mem_read32(member_record);
			member_record += 4u;
			OBJECT(object)->animation = mem_read32(member_record);
			member_record += 4u;
			list_offset = mem_read32(member_record);
			member_record += 4u;
			if ((group_member_number & 1u) != 0u)
				OBJECT(object)->animation = list_offset;
			OBJECT(object)->hitbox_size_x = mem_read16(member_record);
			member_record += 2u;
			OBJECT(object)->hitbox_size_y = mem_read16(member_record);
			OBJECT(object)->var_96 = mem_read16(member_record + 2u);
			OBJECT(object)->sprite_offset_x = 0x2E;
			OBJECT(object)->sprite_offset_y = 0x11;
			AS8(OBJECT(object)->speed) = 0u;
			group_member_number++;
			AT8(OBJECT(object)->speed, 1) = group_member_number;
			OBJECT(object)->floor_block = group_floor_block;
			AT8(OBJECT(object)->spawn_arg_2, 1) = group_bridge_number;
			OBJECT(object)->direction = group_direction;
			OBJECT(object)->parent = group_leader;
			AS16(OBJECT(object)->player) = group_previous_member;
			group_previous_member = object;
			floor_block_record = floor_blocks + 2u + OBJECT(object)->floor_block;
			OBJECT(object)->z_int = mem_read16(floor_block_record + 0x10u);
			OBJECT(object)->z_fraction = 0u;
			OBJECT(object)->floor_z = OBJECT(object)->z;
			OBJECT(object)->speed_x = OBJECT(object)->z;
			OBJECT(object)->floor_slope = mem_read8(floor_block_record + 0x13u);
			floor_type_record = floor_types_ADDRESS + mem_read8(floor_block_record + 0x12u) * 4u;
			OBJECT(object)->floor_type = ROM8(floor_type_record);
			OBJECT(object)->floor_attribute = ROM16(floor_type_record + 2u);
			OBJECT(object)->flags = 0u;
			AS8(OBJECT(object)->spawn_arg_2) = 0u;
			OBJECT(object)->var_8A = group_slot_index;
			AS8(OBJECT(object)->spawn_arg_1) = group_takes_slot;
			members_left = group_member_count;
			group_member_count = members_left - 1u;
		} while ((int16_t)members_left > 1);
		AS8(OBJECT(object)->spawn_arg_2) = 0xFFu;
		last_plank_var = bridge_0_last_plank_ADDRESS;
		if (group_bridge_number != 0u)
			last_plank_var = bridge_1_last_plank_ADDRESS;
		mem_write16(last_plank_var, object);
	}
	group_spawn_cursor_2 += 0x18u;
	*p_list_offset = list_offset;
	*p_count = count;
	*p_object = object;
}

void rope_bridge_plank_update(void)
{
	uint8_t slot_mark = 0, sound_id = 0;
	uint16_t child = 0, last_plank = 0;
	uint32_t r1 = 0, r2 = 0, r7 = 0, at_last_step = 0, hitbox_record = 0, ignored = 0, object = 0, object_2 = 0, object_3 = 0, odd_plank_animation = 0, plank_count_address = 0, plank_index = 0, rope_offsets = 0, ropes_left = 0, sag = 0, sag_table = 0, slot_address = 0, sprite_depth = 0;

	switch (OBJECT(current_object)->rope_bridge_plank.state)
	{
	case 0:
	case 3:
		project_object(&ignored, &ignored);
		OBJECT(current_object)->child = 0xFFFFu;
		if ((OBJECT(current_object)->rope_bridge_plank.plank_index & 3u) == 0u && free_object_stack_2[0] >= 2)
		{
			ropes_left = 2u;
			object = current_object;
			rope_offsets = rope_bridge_rope_offsets_ADDRESS;
			if (OBJECT(current_object)->kind == 0x30u)
				rope_offsets = wide_rope_bridge_rope_offsets_ADDRESS;
			do
			{
				allocate_object_2(&ignored, &r2, &object_2);
				OBJECT(object_2)->sprite_flags = 0x1004u;
				OBJECT(object_2)->sprite_palette = 0x8290u;
				OBJECT(object_2)->sprite_frame = 0x9D18u;
				OBJECT(object_2)->projected_x = ROM16(rope_offsets);
				rope_offsets += 2u;
				OBJECT(object_2)->projected_y_fraction = ROM16(rope_offsets);
				OBJECT(object)->child = object_2;
				OBJECT(object_2)->parent = object;
				object = (object & ~0xFFFFu) | (uint16_t)object_2;
				rope_offsets += 2u;
				ropes_left--;
			} while (ropes_left != 0u);
		}
		OBJECT(current_object)->rope_bridge_plank.state++;
	case 1:
		OBJECT(current_object)->previous_floor_z = OBJECT(current_object)->floor_z;
		plank_index = OBJECT(current_object)->rope_bridge_plank.plank_index;
		sag_table = bridge_0_sag_ADDRESS;
		if (OBJECT(current_object)->rope_bridge_plank.bridge != 0u)
			sag_table = bridge_1_sag_ADDRESS;
		sag = mem_read16(sag_table + plank_index * 2u);
		if ((sag & ~0xFFu) == 0u)
			sag = 0u;
		OBJECT(current_object)->previous_floor_z += sag << 8;
		OBJECT(current_object)->speed_x += (int32_t)((uint32_t)OBJECT(current_object)->previous_floor_z - OBJECT(current_object)->speed_x) >> 2;
		OBJECT(current_object)->z_int = OBJECT(current_object)->speed_x_int;
		update_screen_position(&ignored, &sag);
		if (rope_bridge_collapsing != 0u)
		{
		rope_bridge_plank_start_falling:
			OBJECT(current_object)->rope_bridge_plank.state++;
			last_plank = bridge_0_last_plank;
			plank_count_address = bridge_0_plank_count_ADDRESS;
			if (OBJECT(current_object)->rope_bridge_plank.bridge != 0u)
			{
				last_plank = bridge_1_last_plank;
				plank_count_address = bridge_1_plank_count_ADDRESS;
			}
			if (last_plank == (uint16_t)current_object)
				mem_write16(plank_count_address, 0u);
			OBJECT(current_object)->speed_z = 0;
			OBJECT(current_object)->animation_speed = 2u;
			OBJECT(current_object)->animation_timer = 0;
			OBJECT(current_object)->animation_frame = 0u;
			if ((uint16_t)OBJECT(current_object)->screen_x < 0x200u)
			{
				sound_id = 0x28u;
				play_sound(sound_id, &sag, &r2);
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
					goto rope_bridge_plank_start_falling;
				break;
			case 1:
				if (OBJECT(current_player)->y_int <= OBJECT(current_object)->y_int)
					goto rope_bridge_plank_start_falling;
				break;
			case 2:
				if (OBJECT(current_object)->x_int <= OBJECT(current_player)->x_int)
					goto rope_bridge_plank_start_falling;
				break;
			case 3:
				if (OBJECT(current_player)->x_int <= OBJECT(current_object)->x_int)
					goto rope_bridge_plank_start_falling;
				break;
			default:
				unknown_code(0x93BC7u + OBJECT(current_object)->direction * 2u);
			}
		}
		if (OBJECT(current_object)->rope_bridge_plank.takes_slot < 0)
		{
			if ((OBJECT(current_object)->flags & 8u) != 0u)
			{
				object_3 = LINKED(current_object, OBJECT(current_object)->platform);
				if (OBJECT(object_3)->kind == 0xFu)
				{
					switch (OBJECT(current_object)->direction)
					{
					case 0:
					case 1:
					case 4:
						slot_mark = 0xFFu;
						if ((int16_t)((OBJECT(current_object)->hitbox_size_x >> 1) + OBJECT(current_object)->x_int) <= OBJECT(object_3)->x_int)
							slot_mark = 0x80u;
						break;
					case 2:
					case 3:
						slot_mark = 0xFFu;
						if ((int16_t)((OBJECT(current_object)->hitbox_size_y >> 1) + OBJECT(current_object)->y_int) <= OBJECT(object_3)->y_int)
							slot_mark = 0x80u;
						break;
					default:
						unknown_code(0x93C6Cu + OBJECT(current_object)->direction * 2u);
					}
					RAM8(group_slots_ADDRESS + OBJECT(current_object)->rope_bridge_plank.group_slot) = slot_mark;
				}
			}
			slot_address = group_slots_ADDRESS + OBJECT(current_object)->rope_bridge_plank.group_slot;
			if ((int8_t)mem_read8(slot_address) < 0)
			{
				switch (OBJECT(current_object)->direction)
				{
				case 0:
				case 1:
				case 4:
					OBJECT(current_object)->x_int += OBJECT(current_object)->hitbox_size_x >> 1;
					OBJECT(current_object)->floor_block += 0x16u;
					if (mem_read8(slot_address) != 0xFFu)
					{
						OBJECT(current_object)->rope_bridge_plank.depth_offset = (uint32_t)OBJECT(current_object)->rope_bridge_plank.depth_offset << 1;
						OBJECT(current_object)->floor_block += 0x16u;
					}
					break;
				case 2:
				case 3:
					OBJECT(current_object)->floor_block += 0x16u;
					if (mem_read8(slot_address) == 0xFFu)
					{
						OBJECT(current_object)->y_int += OBJECT(current_object)->hitbox_size_y >> 1;
						OBJECT(current_object)->floor_block += 0x16u;
						OBJECT(current_object)->rope_bridge_plank.depth_offset -= 0x28u;
					}
					break;
				default:
					unknown_code(0x93CD1u + OBJECT(current_object)->direction * 2u);
				}
				OBJECT(current_object)->sprite_flags = stage_object_records_2;
				OBJECT(current_object)->sprite_frame = rope_plank_frame;
				OBJECT(current_object)->sprite_palette = rope_plank_palette;
				OBJECT(current_object)->animation = rope_plank_animation;
				odd_plank_animation = rope_plank_animation_2;
				hitbox_record = rope_plank_hitbox_size_ADDRESS;
				if ((OBJECT(current_object)->rope_bridge_plank.plank_index & 1u) != 0u)
					OBJECT(current_object)->animation = odd_plank_animation;
				OBJECT(current_object)->hitbox_size_x = ROM16(hitbox_record);
				OBJECT(current_object)->hitbox_size_y = ROM16(hitbox_record + 2u);
				OBJECT(current_object)->rope_bridge_plank.takes_slot = 1;
				if ((int16_t)OBJECT(current_object)->child >= 0)
				{
					object_2 = LINKED(current_object, OBJECT(current_object)->child);
					OBJECT(object_2)->projected_x = rope_bridge_rope_offsets;
					OBJECT(object_2)->projected_y_fraction = rope_bridge_rope_1_dy;
					object_2 = LINKED(object_2, OBJECT(object_2)->child);
					OBJECT(object_2)->projected_x = rope_bridge_rope_2_dx;
					OBJECT(object_2)->projected_y_fraction = rope_bridge_rope_2_dy;
				}
				project_object(&ignored, &ignored);
			}
		}
		if ((uint16_t)(OBJECT(current_object)->screen_x + 0x50u) >= 0x1B0u)
			return;
		add_to_platform_list();
		draw_rope_bridge_ropes();
		sprite_depth = (uint16_t)(((int32_t)((uint32_t)OBJECT(current_object)->projected_y + OBJECT(current_object)->floor_z) >> 16) - view_y + OBJECT(current_object)->rope_bridge_plank.depth_offset);
		OBJECT(current_object)->rope_bridge_plank.draw_depth = sprite_depth;
		queue_sprite_at_depth(&ignored, &sprite_depth);
		return;
	case 2:
		OBJECT(current_object)->speed_z += 0x6000u;
		OBJECT(current_object)->z += OBJECT(current_object)->speed_z;
		if (OBJECT(current_object)->animation_timer >= 0)
		{
			if ((OBJECT(current_object)->sprite_flags & ~0xFFFu) != 0xD000u)
			{
				animate(&r1, &r7, &at_last_step);
				if ((int8_t)at_last_step < 0)
					OBJECT(current_object)->animation_timer = -1;
			}
			else if ((int32_t)OBJECT(current_object)->part_list >= 0x205000)
			{
				animate_all_parts(&at_last_step, &object_2);
				if ((int8_t)at_last_step < 0)
					OBJECT(current_object)->animation_timer = -1;
			}
		}
		update_screen_position(&ignored, &r1);
		if (OBJECT(current_object)->screen_y <= 0x1C0)
		{
			draw_rope_bridge_ropes();
			ignored = queue_sprite_at_floor();
			return;
		}
		if ((int16_t)OBJECT(current_object)->child >= 0)
		{
			child = OBJECT(current_object)->child;
			free_object_2(child);
			current_object = LINKED(current_object, OBJECT(current_object)->child);
			child = OBJECT(current_object)->child;
			free_object_2(child);
		}
		delete_current_object();
		return;
	default:
		unknown_code(0x93AEAu + OBJECT(current_object)->rope_bridge_plank.state * 3u);
	}
}

void draw_rope_bridge_ropes(void)
{
	uint32_t depth = 0, ropes_left = 0, slot_address = 0;

	if ((int16_t)OBJECT(current_object)->child < 0)
		return;
	ropes_left = 2u;
	do
	{
		current_object = LINKED(current_object, OBJECT(current_object)->child);
		if ((uint8_t)OBJECT(object_cursor)->draw_rope_bridge_ropes.takes_slot == 0u)
		{
		draw_rope_bridge_ropes_show_rope:
			if (OBJECT(object_cursor)->draw_rope_bridge_ropes.state == 2u)
				OBJECT(current_object)->sprite_frame = 0x9D20u;
			OBJECT(current_object)->screen_x = OBJECT(object_cursor)->screen_x + OBJECT(current_object)->projected_x;
			OBJECT(current_object)->screen_y = OBJECT(object_cursor)->screen_y + OBJECT(current_object)->projected_y_fraction;
			depth = queue_sprite(depth);
		}
		else
		{
			slot_address = group_slots_ADDRESS + OBJECT(object_cursor)->draw_rope_bridge_ropes.group_slot;
			if ((uint16_t)ropes_left == 1u)
			{
				if (mem_read8(slot_address) != 0x80u)
					goto draw_rope_bridge_ropes_show_rope;
			}
			else if (mem_read8(slot_address) != 0xFFu)
				goto draw_rope_bridge_ropes_show_rope;
		}
		ropes_left--;
	} while (ropes_left != 0u);
	current_object = object_cursor;
}
