/**
 *    Name:        objects/lava_rocks.c
 *    Description: stage 8: the rock slabs over lava, the lava columns, the slot trigger that times them
 *    Version:     0.1.0
 *    Created:     12-10-2026
 *    Modified:    15-10-2026
 *    Author:      Sebastian Toyos
 *
 *    SegaSonic The Hedgehog (arcade, Sega System 32): the game's V60 program decompiled into C by
 *    tools/decompile.py, with the names of symbols.txt.
 */
#include "game.h"

void lava_rock_update(void)
{
	uint8_t sound_id = 0;
	uint16_t part_list = 0;
	uint32_t r1 = 0, r7 = 0, r11 = 0, at_last_step = 0, depth = 0, free_count = 0, height = 0, ignored = 0, random_value = 0, slot_entry = 0, sprite_depth = 0, write_index_address = 0;
	uint16_t erupt_delay = 0;

	if (map_screen_active != 0u)
	{
		if ((OBJECT(current_object)->sprite_flags & 0xD000u) == 0xD000u)
		{
			part_list = (uint16_t)OBJECT(current_object)->part_list;
			free_object_2(part_list);
		}
		delete_current_object();
		return;
	}
	switch (OBJECT(current_object)->lava_rock.state)
	{
	case 0:
	case 4:
		OBJECT(current_object)->lava_rock.state++;
		OBJECT(current_object)->lava_rock.palette_list = lava_rock_glow_palettes_ADDRESS;
		OBJECT(current_object)->lava_rock.palette_index = 0u;
		OBJECT(current_object)->lava_rock.palette_timer = 0u;
		next_random(&random_value, &ignored, &ignored);
		OBJECT(current_object)->lava_rock.palette_delay = (random_value & 0xFu) + 4u;
		if ((OBJECT(current_object)->sprite_flags & 0xD000u) == 0xD000u)
		{
			allocate_object_2(&free_count, &ignored, &r11);
			if ((uint8_t)free_count == 0u)
			{
				delete_current_object();
				return;
			}
			move_halfwords_backward(r11, lava_rock_parts_ADDRESS, 0x10u);
			OBJECT(current_object)->part_list = r11;
		}
		project_object(&ignored, &r1);
		OBJECT(current_object)->previous_floor_z = OBJECT(current_object)->floor_z;
	case 1:
		update_screen_position(&ignored, &r1);
		current_player = current_object;
		current_player = LINKED(current_player, OBJECT(current_object)->parent);
		if ((OBJECT(current_player)->var_80 | OBJECT(current_player)->var_81) != 0u)
		{
			if (OBJECT(current_object)->lava_rock.takes_slot != 0u)
			{
				slot_entry = group_slots_ADDRESS + OBJECT(current_object)->lava_rock.group_slot * 2u;
				if ((int16_t)mem_read16(slot_entry) < 0)
				{
					OBJECT(current_object)->lava_rock.erupt_delay = mem_read16(slot_entry) & 0x7FFFu;
					OBJECT(current_object)->lava_rock.rise_limit = 0u;
					OBJECT(current_object)->lava_rock.state++;
					OBJECT(current_object)->speed_z = -0x10000;
				}
			}
		}
		else
		{
			switch (OBJECT(current_object)->direction)
			{
			case 0:
			case 4:
				if (OBJECT(current_object)->y_int <= (int16_t)(OBJECT(current_player)->y_int + 0x5Au))
				{
					OBJECT(current_object)->lava_rock.rise_limit = 0x10u;
					OBJECT(current_object)->lava_rock.state++;
					OBJECT(current_object)->speed_z = -0x10000;
					if ((uint16_t)(OBJECT(current_object)->screen_x + 0xA8u) >= 0x1E8u)
						return;
					if ((OBJECT(current_object)->sprite_flags & 0xD000u) != 0xD000u)
						animate_sign();
					else
						cycle_part_palettes();
					add_to_platform_list();
					sprite_depth = (uint16_t)(OBJECT(current_object)->screen_y + OBJECT(current_object)->lava_rock.depth_offset - OBJECT(current_object)->sprite_offset_y);
					OBJECT(current_object)->lava_rock.draw_depth = sprite_depth;
					queue_sprite_at_depth(&ignored, &sprite_depth);
					return;
				}
				if (OBJECT(current_object)->lava_rock.takes_slot != 0u)
				{
					slot_entry = group_slots_ADDRESS + OBJECT(current_object)->lava_rock.group_slot * 2u;
					if ((int16_t)mem_read16(slot_entry) < 0)
					{
						OBJECT(current_object)->lava_rock.erupt_delay = mem_read16(slot_entry) & 0x7FFFu;
						OBJECT(current_object)->lava_rock.rise_limit = 0u;
						OBJECT(current_object)->lava_rock.state++;
						OBJECT(current_object)->speed_z = -0x10000;
					}
				}
				if ((uint16_t)(OBJECT(current_object)->screen_x + 0xA8u) >= 0x1E8u)
					return;
				if ((OBJECT(current_object)->sprite_flags & 0xD000u) != 0xD000u)
					animate_sign();
				else
					cycle_part_palettes();
				add_to_platform_list();
				sprite_depth = (uint16_t)(OBJECT(current_object)->screen_y + OBJECT(current_object)->lava_rock.depth_offset - OBJECT(current_object)->sprite_offset_y);
				OBJECT(current_object)->lava_rock.draw_depth = sprite_depth;
				queue_sprite_at_depth(&ignored, &sprite_depth);
				return;
			case 1:
				if ((int16_t)(OBJECT(current_player)->y_int - 0x5Au) <= OBJECT(current_object)->y_int)
				{
					OBJECT(current_object)->lava_rock.rise_limit = 0x10u;
					OBJECT(current_object)->lava_rock.state++;
					OBJECT(current_object)->speed_z = -0x10000;
					if ((uint16_t)(OBJECT(current_object)->screen_x + 0xA8u) >= 0x1E8u)
						return;
					if ((OBJECT(current_object)->sprite_flags & 0xD000u) != 0xD000u)
						animate_sign();
					else
						cycle_part_palettes();
					add_to_platform_list();
					sprite_depth = (uint16_t)(OBJECT(current_object)->screen_y + OBJECT(current_object)->lava_rock.depth_offset - OBJECT(current_object)->sprite_offset_y);
					OBJECT(current_object)->lava_rock.draw_depth = sprite_depth;
					queue_sprite_at_depth(&ignored, &sprite_depth);
					return;
				}
				if (OBJECT(current_object)->lava_rock.takes_slot != 0u)
				{
					slot_entry = group_slots_ADDRESS + OBJECT(current_object)->lava_rock.group_slot * 2u;
					if ((int16_t)mem_read16(slot_entry) < 0)
					{
						OBJECT(current_object)->lava_rock.erupt_delay = mem_read16(slot_entry) & 0x7FFFu;
						OBJECT(current_object)->lava_rock.rise_limit = 0u;
						OBJECT(current_object)->lava_rock.state++;
						OBJECT(current_object)->speed_z = -0x10000;
					}
				}
				if ((uint16_t)(OBJECT(current_object)->screen_x + 0xA8u) >= 0x1E8u)
					return;
				if ((OBJECT(current_object)->sprite_flags & 0xD000u) != 0xD000u)
					animate_sign();
				else
					cycle_part_palettes();
				add_to_platform_list();
				sprite_depth = (uint16_t)(OBJECT(current_object)->screen_y + OBJECT(current_object)->lava_rock.depth_offset - OBJECT(current_object)->sprite_offset_y);
				OBJECT(current_object)->lava_rock.draw_depth = sprite_depth;
				queue_sprite_at_depth(&ignored, &sprite_depth);
				return;
			case 2:
				if (OBJECT(current_object)->x_int <= (int16_t)(OBJECT(current_player)->x_int + 0x5Au))
				{
					OBJECT(current_object)->lava_rock.rise_limit = 0x10u;
					OBJECT(current_object)->lava_rock.state++;
					OBJECT(current_object)->speed_z = -0x10000;
					if ((uint16_t)(OBJECT(current_object)->screen_x + 0xA8u) >= 0x1E8u)
						return;
					if ((OBJECT(current_object)->sprite_flags & 0xD000u) != 0xD000u)
						animate_sign();
					else
						cycle_part_palettes();
					add_to_platform_list();
					sprite_depth = (uint16_t)(OBJECT(current_object)->screen_y + OBJECT(current_object)->lava_rock.depth_offset - OBJECT(current_object)->sprite_offset_y);
					OBJECT(current_object)->lava_rock.draw_depth = sprite_depth;
					queue_sprite_at_depth(&ignored, &sprite_depth);
					return;
				}
				if (OBJECT(current_object)->lava_rock.takes_slot != 0u)
				{
					slot_entry = group_slots_ADDRESS + OBJECT(current_object)->lava_rock.group_slot * 2u;
					if ((int16_t)mem_read16(slot_entry) < 0)
					{
						OBJECT(current_object)->lava_rock.erupt_delay = mem_read16(slot_entry) & 0x7FFFu;
						OBJECT(current_object)->lava_rock.rise_limit = 0u;
						OBJECT(current_object)->lava_rock.state++;
						OBJECT(current_object)->speed_z = -0x10000;
					}
				}
				if ((uint16_t)(OBJECT(current_object)->screen_x + 0xA8u) >= 0x1E8u)
					return;
				if ((OBJECT(current_object)->sprite_flags & 0xD000u) != 0xD000u)
					animate_sign();
				else
					cycle_part_palettes();
				add_to_platform_list();
				sprite_depth = (uint16_t)(OBJECT(current_object)->screen_y + OBJECT(current_object)->lava_rock.depth_offset - OBJECT(current_object)->sprite_offset_y);
				OBJECT(current_object)->lava_rock.draw_depth = sprite_depth;
				queue_sprite_at_depth(&ignored, &sprite_depth);
				return;
			case 3:
				if ((int16_t)(OBJECT(current_player)->x_int - 0x5Au) <= OBJECT(current_object)->x_int)
				{
					OBJECT(current_object)->lava_rock.rise_limit = 0x10u;
					OBJECT(current_object)->lava_rock.state++;
					OBJECT(current_object)->speed_z = -0x10000;
					if ((uint16_t)(OBJECT(current_object)->screen_x + 0xA8u) >= 0x1E8u)
						return;
					if ((OBJECT(current_object)->sprite_flags & 0xD000u) != 0xD000u)
						animate_sign();
					else
						cycle_part_palettes();
					add_to_platform_list();
					sprite_depth = (uint16_t)(OBJECT(current_object)->screen_y + OBJECT(current_object)->lava_rock.depth_offset - OBJECT(current_object)->sprite_offset_y);
					OBJECT(current_object)->lava_rock.draw_depth = sprite_depth;
					queue_sprite_at_depth(&ignored, &sprite_depth);
					return;
				}
				if (OBJECT(current_object)->lava_rock.takes_slot != 0u)
				{
					slot_entry = group_slots_ADDRESS + OBJECT(current_object)->lava_rock.group_slot * 2u;
					if ((int16_t)mem_read16(slot_entry) < 0)
					{
						OBJECT(current_object)->lava_rock.erupt_delay = mem_read16(slot_entry) & 0x7FFFu;
						OBJECT(current_object)->lava_rock.rise_limit = 0u;
						OBJECT(current_object)->lava_rock.state++;
						OBJECT(current_object)->speed_z = -0x10000;
					}
				}
				if ((uint16_t)(OBJECT(current_object)->screen_x + 0xA8u) >= 0x1E8u)
					return;
				if ((OBJECT(current_object)->sprite_flags & 0xD000u) != 0xD000u)
					animate_sign();
				else
					cycle_part_palettes();
				add_to_platform_list();
				sprite_depth = (uint16_t)(OBJECT(current_object)->screen_y + OBJECT(current_object)->lava_rock.depth_offset - OBJECT(current_object)->sprite_offset_y);
				OBJECT(current_object)->lava_rock.draw_depth = sprite_depth;
				queue_sprite_at_depth(&ignored, &sprite_depth);
				return;
			default:
				unknown_code(0x919DDu + OBJECT(current_object)->direction * 2u);
			}
		}
		if ((uint16_t)(OBJECT(current_object)->screen_x + 0xA8u) >= 0x1E8u)
			return;
		if ((OBJECT(current_object)->sprite_flags & 0xD000u) != 0xD000u)
			animate_sign();
		else
			cycle_part_palettes();
		add_to_platform_list();
		sprite_depth = (uint16_t)(OBJECT(current_object)->screen_y + OBJECT(current_object)->lava_rock.depth_offset - OBJECT(current_object)->sprite_offset_y);
		OBJECT(current_object)->lava_rock.draw_depth = sprite_depth;
		queue_sprite_at_depth(&ignored, &sprite_depth);
		return;
	case 2:
		OBJECT(current_object)->z += OBJECT(current_object)->speed_z;
		height = (uint32_t)OBJECT(current_object)->floor_z - OBJECT(current_object)->z;
		if (height >= 0x20000u)
		{
			OBJECT(current_object)->speed_z = 0u - OBJECT(current_object)->speed_z;
			if ((int32_t)height >= 0 && (int16_t)(OBJECT(current_object)->previous_floor_z_int - OBJECT(current_object)->floor_z_int) <= (int16_t)OBJECT(current_object)->lava_rock.rise_limit)
				OBJECT(current_object)->floor_z_int--;
		}
		update_screen_position(&ignored, &r1);
		current_player = current_object;
		current_player = LINKED(current_player, OBJECT(current_object)->parent);
		if (OBJECT(current_object)->y_int <= OBJECT(current_player)->y_int)
		{
		lava_rock_erupt:
			OBJECT(current_object)->lava_rock.state++;
			OBJECT(current_object)->speed_z = 0;
			OBJECT(current_object)->animation_speed = 4u;
			OBJECT(current_object)->animation_timer = 0;
			OBJECT(current_object)->animation_frame = 0u;
			if (OBJECT(current_object)->lava_rock.takes_slot != 1u || OBJECT(current_object)->lava_rock.member == 0u)
			{
				if (OBJECT(current_object)->screen_x >= -0x90)
				{
					sound_id = 7u;
					play_sound(sound_id, &r1, &write_index_address);
				}
				r11 = spawn_lava_column(r11);
				if ((OBJECT(current_object)->sprite_flags & 0xD000u) == 0xD000u)
				{
					OBJECT(current_object)->x_int += 0x71u;
					spawn_lava_column(r11);
					OBJECT(current_object)->x_int -= 0x71u;
				}
			}
			else if (OBJECT(current_object)->lava_rock.member == 2u)
			{
				r11 = spawn_lava_column(r11);
				if ((OBJECT(current_object)->sprite_flags & 0xD000u) == 0xD000u)
				{
					OBJECT(current_object)->x_int += 0x71u;
					spawn_lava_column(r11);
					OBJECT(current_object)->x_int -= 0x71u;
				}
				if ((uint16_t)(OBJECT(current_object)->screen_x + 0xA0u) >= 0x281u)
					return;
				add_to_platform_list();
				sprite_depth = (uint16_t)(((int32_t)((uint32_t)OBJECT(current_object)->projected_y + OBJECT(current_object)->previous_floor_z) >> 16) - view_y + OBJECT(current_object)->lava_rock.depth_offset);
				OBJECT(current_object)->lava_rock.draw_depth = sprite_depth;
				queue_sprite_at_depth(&ignored, &sprite_depth);
				return;
			}
		}
		else
		{
			if (OBJECT(current_object)->y_int <= (int16_t)(OBJECT(current_player)->y_int + 0x30u))
				OBJECT(current_object)->lava_rock.rise_limit = 0x20u;
			if (OBJECT(current_object)->lava_rock.takes_slot != 0u)
			{
				r11 = group_slots_ADDRESS + OBJECT(current_object)->lava_rock.group_slot * 2u;
				if ((int16_t)mem_read16(r11) < 0)
				{
					erupt_delay = OBJECT(current_object)->lava_rock.erupt_delay;
					OBJECT(current_object)->lava_rock.erupt_delay = erupt_delay - 1u;
					if ((int16_t)erupt_delay <= 1)
					{
						OBJECT(current_object)->lava_rock.takes_slot = 1u;
						goto lava_rock_erupt;
					}
				}
			}
		}
		if ((uint16_t)(OBJECT(current_object)->screen_x + 0xA0u) >= 0x281u)
			return;
		add_to_platform_list();
		sprite_depth = (uint16_t)(((int32_t)((uint32_t)OBJECT(current_object)->projected_y + OBJECT(current_object)->previous_floor_z) >> 16) - view_y + OBJECT(current_object)->lava_rock.depth_offset);
		OBJECT(current_object)->lava_rock.draw_depth = sprite_depth;
		queue_sprite_at_depth(&ignored, &sprite_depth);
		return;
	case 3:
		OBJECT(current_object)->speed_z -= 0x6000u;
		OBJECT(current_object)->z += OBJECT(current_object)->speed_z;
		update_screen_position(&ignored, &r1);
		if (OBJECT(current_object)->screen_y > -0x28)
		{
			if ((int8_t)OBJECT(current_object)->animation_frame >= 0)
			{
				if ((OBJECT(current_object)->sprite_flags & 0xD000u) != 0xD000u)
				{
					animate(&r1, &r7, &at_last_step);
					if ((uint8_t)at_last_step != 0u)
						OBJECT(current_object)->animation_frame = 0xFFu;
				}
				else
				{
					animate_all_parts(&at_last_step, &r11);
					if ((uint8_t)at_last_step != 0u)
					{
						OBJECT(current_object)->animation_frame = 0xFFu;
						if ((int8_t)OBJECT(current_object)->animation_frame <= 2)
							add_to_platform_list();
						depth = (uint16_t)(((int32_t)((uint32_t)OBJECT(current_object)->projected_y + OBJECT(current_object)->floor_z) >> 16) - view_y + OBJECT(current_object)->lava_rock.depth_offset);
						queue_sprite_at_depth(&ignored, &depth);
						return;
					}
				}
			}
			if ((int8_t)OBJECT(current_object)->animation_frame <= 2)
				add_to_platform_list();
			depth = (uint16_t)(((int32_t)((uint32_t)OBJECT(current_object)->projected_y + OBJECT(current_object)->floor_z) >> 16) - view_y + OBJECT(current_object)->lava_rock.depth_offset);
			queue_sprite_at_depth(&ignored, &depth);
			return;
		}
		if ((OBJECT(current_object)->sprite_flags & 0xD000u) == 0xD000u)
		{
			part_list = (uint16_t)OBJECT(current_object)->part_list;
			free_object_2(part_list);
		}
		delete_current_object();
		return;
	default:
		unknown_code(0x9194Du + OBJECT(current_object)->lava_rock.state * 3u);
	}
}

uint32_t spawn_lava_column(uint32_t block)
{
	uint16_t side = 0;
	uint32_t free_count = 0, ignored = 0, object = 0;

	if ((uint16_t)free_object_stack_2[0] != 0u)
	{
		allocate_object(&free_count, &block);
		if (free_count == 0u)
			return block;
		OBJECT(block)->sprite_flags = 0xD006u;
		OBJECT(block)->flags = 2u;
		OBJECT(block)->sprite_offset_x = 0;
		OBJECT(block)->sprite_offset_y = 0;
		OBJECT(block)->handler = lava_column_start_ADDRESS;
		OBJECT(block)->direction = 0x200u;
		OBJECT(block)->speed_x = 0;
		OBJECT(block)->speed_y = 0;
		OBJECT(block)->speed_z = 0;
		OBJECT(block)->lava_column.standing = 0;
		OBJECT(block)->parent = OBJECT(current_object)->parent;
		OBJECT(block)->x = OBJECT(current_object)->x;
		OBJECT(block)->y = OBJECT(current_object)->y;
		OBJECT(block)->z = OBJECT(current_object)->z;
		OBJECT(block)->z += 0xE00000u;
		OBJECT(block)->kind = 0x14u;
		OBJECT(block)->hitbox_size_x = 0x74u;
		OBJECT(block)->hitbox_size_y = 0x30u;
		OBJECT(block)->hitbox_height = 0xE0u;
		OBJECT(block)->hitbox_offset_x = 0;
		OBJECT(block)->hitbox_offset_y = 0;
		OBJECT(block)->animation = lava_column_animation_ADDRESS;
		OBJECT(block)->animation_speed = 2u;
		OBJECT(block)->animation_timer = 0;
		OBJECT(block)->animation_frame = 0u;
		side = 0u;
		if (OBJECT(block)->x_int >= 0xC97)
			side = 1u;
		OBJECT(block)->lava_column.side = side;
		if (OBJECT(current_object)->spawn_lava_column.takes_slot == 1u)
		{
			OBJECT(block)->handler = lava_column_toss_start_ADDRESS;
			OBJECT(block)->speed_z = -0xE0000;
			OBJECT(block)->lava_column.depth_offset = OBJECT(current_object)->spawn_lava_column.depth_offset;
			OBJECT(block)->floor_z = OBJECT(current_object)->previous_floor_z;
			OBJECT(block)->hitbox_size_x = 0x70u;
			OBJECT(block)->hitbox_size_y = 0x60u;
		}
		object = block;
		allocate_object_2(&ignored, &ignored, &block);
		move_halfwords_backward(block, lava_column_parts_ADDRESS, 9u);
		OBJECT(object)->part_list = block;
	}
	return block;
}

void lava_column_start(void)
{
	uint32_t ignored = 0;

	project_object(&ignored, &ignored);
	OBJECT(current_object)->handler = lava_column_rise_ADDRESS;
	lava_column_rise();
}

void lava_column_rise(void)
{
	uint16_t part_list = 0;
	uint32_t r1 = 0, r8 = 0, depth = 0, ignored = 0, part = 0, sprite_depth = 0;

	if (OBJECT(current_object)->screen_x <= -0x88)
	{
		part_list = (uint16_t)OBJECT(current_object)->part_list;
		free_object_2(part_list);
		delete_current_object();
		return;
	}
	if (map_screen_active != 0u || (OBJECT(current_object)->lava_column.standing < 0 && lava_columns[OBJECT(current_object)->lava_column.side] != (uint16_t)current_object))
	{
		part_list = (uint16_t)OBJECT(current_object)->part_list;
		free_object_2(part_list);
		delete_current_object();
		return;
	}
	if (OBJECT(current_object)->lava_column.standing >= 0)
	{
		OBJECT(current_object)->speed_z -= 0x6000u;
		OBJECT(current_object)->z += OBJECT(current_object)->speed_z;
	}
	update_screen_position(&ignored, &r1);
	if (OBJECT(current_object)->z_int <= 0x570)
	{
		OBJECT(current_object)->lava_column.standing = -1;
		lava_columns[OBJECT(current_object)->lava_column.side] = current_object;
	}
	animate_all_parts(&r8, &part);
	add_to_touchable_list(ignored);
	if (OBJECT(current_object)->lava_column.standing < 0)
	{
		depth = 0xFFF1u;
		queue_sprite_at_depth(&ignored, &depth);
		return;
	}
	sprite_depth = (uint16_t)(OBJECT(current_object)->screen_y - 0x128u);
	if ((int16_t)sprite_depth <= -0xF)
		sprite_depth = 0xFFF2u;
	queue_sprite_at_depth(&ignored, &sprite_depth);
}

void lava_column_toss_start(void)
{
	uint32_t ignored = 0;

	project_object(&ignored, &ignored);
	OBJECT(current_object)->handler = lava_column_toss_ADDRESS;
	lava_column_toss();
}

void lava_column_toss(void)
{
	uint16_t part_list = 0;
	uint32_t r1 = 0, r8 = 0, depth = 0, ignored = 0, part = 0;

	if (OBJECT(current_object)->screen_x <= -0x88)
	{
		part_list = (uint16_t)OBJECT(current_object)->part_list;
		free_object_2(part_list);
		delete_current_object();
		return;
	}
	OBJECT(current_object)->speed_z += 0x6000u;
	OBJECT(current_object)->z += OBJECT(current_object)->speed_z;
	if (OBJECT(current_object)->z_int < 0x660)
	{
		update_screen_position(&ignored, &r1);
		animate_all_parts(&r8, &part);
		add_to_touchable_list(ignored);
		depth = (uint16_t)(((int32_t)((uint32_t)OBJECT(current_object)->projected_y + OBJECT(current_object)->floor_z) >> 16) - view_y + OBJECT(current_object)->lava_column.depth_offset);
		queue_sprite_at_depth(&ignored, &depth);
		return;
	}
	OBJECT(current_object)->handler = lava_column_land_ADDRESS;
	part_list = (uint16_t)OBJECT(current_object)->part_list;
	free_object_2(part_list);
	lava_column_land();
}

void lava_column_land(void)
{
	OBJECT(current_object)->lava_column.base_x = 0x40;
	lava_column_become_bubble();
}

void lava_column_land_in_place(void)
{
	OBJECT(current_object)->lava_column.base_x = 0;
	lava_column_become_bubble();
}

void lava_column_become_bubble(void)
{
	uint32_t ignored = 0;

	OBJECT(current_object)->lava_column.base_x += OBJECT(current_object)->x_int;
	OBJECT(current_object)->lava_column.base_y = OBJECT(current_object)->y_int;
	OBJECT(current_object)->z = 0x5400000;
	OBJECT(current_object)->sprite_flags = 0x1004u;
	OBJECT(current_object)->sprite_palette = 0xD000u;
	OBJECT(current_object)->sprite_offset_x = 0;
	OBJECT(current_object)->sprite_offset_y = 0;
	OBJECT(current_object)->flags = 0u;
	OBJECT(current_object)->handler = bubble_near_parent_update_ADDRESS;
	OBJECT(current_object)->animation_frame = 0u;
	OBJECT(current_object)->animation = bubble_animation_ADDRESS;
	OBJECT(current_object)->animation_timer = 0;
	OBJECT(current_object)->animation_speed = 4u;
	project_object(&ignored, &ignored);
}

void spawn_group_slot_trigger(uint32_t record, uint32_t *p_spawned, uint32_t *p_count, uint32_t *p_trigger)
{
	uint32_t count = *p_count;
	uint32_t trigger = *p_trigger;
	uint32_t fill_word = 0, free_count = 0, slot_cursor = 0, spawned = 0;

	allocate_object_outside_loop(&free_count, &trigger);
	if (free_count == 0u)
	{
		*p_spawned = free_count;
		*p_trigger = trigger;
		return;
	}
	OBJECT(trigger)->group_slot_trigger.next_trigger = mem_read16(record + 9u) + group_slot_triggers_ADDRESS;
	OBJECT(trigger)->handler = group_slot_trigger_update_ADDRESS;
	OBJECT(trigger)->flags = 0u;
	slot_cursor = group_slots_ADDRESS;
	count = 0x20u;
	fill_word = 0u;
	do
	{
		mem_write32(slot_cursor, fill_word);
		slot_cursor += 4u;
		count--;
	} while (count != 0u);
	spawned = 0xFFFFFFFFu;
	*p_spawned = spawned;
	*p_count = count;
	*p_trigger = trigger;
}

void group_slot_trigger_update(void)
{
	uint32_t cursor = 0, slot = 0;
	uint16_t trigger_scroll = 0;
	uint32_t slot_address = 0;

	cursor = OBJECT(current_object)->group_slot_trigger.next_trigger;
	trigger_scroll = mem_read16(cursor);
	cursor += 2u;
	if (scroll_position < trigger_scroll)
		return;
	if ((int16_t)mem_read16(cursor) >= 0)
	{
		slot = mem_read16(cursor);
		cursor += 2u;
		if (group_slots[slot] != 0u)
		{
			group_slots[slot] = mem_read16(cursor);
			slot_address = group_slots_ADDRESS + slot * 2u;
			mem_write16(slot_address, mem_read16(slot_address) | 0x8000u);
			AS16(OBJECT(current_object)->group_slot_trigger.next_trigger) = cursor + 2u;
		}
		return;
	}
	if (mem_read16(cursor) == 0x8000u)
		goal_unlocked = 0xFFu;
	delete_current_object();
}
