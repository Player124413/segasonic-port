/**
 *    Name:        engine/projection.c
 *    Description: between the map and the screen (project_object, update_screen_position), speeds from directions, the floor under an object (find_floor), the player's steps against it
 *    Version:     0.1.0
 *    Created:     12-10-2026
 *    Modified:    15-10-2026
 *    Author:      Sebastian Toyos
 *
 *    SegaSonic The Hedgehog (arcade, Sega System 32): the game's V60 program decompiled into C by
 *    tools/decompile.py, with the names of symbols.txt.
 */
#include "game.h"

void side_of_box(uint32_t other, uint32_t *p_hitbox_offset_y, uint32_t *p_sides)
{
	uint16_t box_start = 0, hitbox_offset_y = 0, sides = 0;

	sides = 0u;
	box_start = (uint16_t)(OBJECT(other)->x_int + OBJECT(other)->hitbox_offset_x);
	if ((int16_t)box_start < OBJECT(current_object)->x_int)
	{
		if ((int16_t)(box_start + OBJECT(other)->hitbox_size_x) <= OBJECT(current_object)->x_int)
			sides |= 4u;
	}
	else
		sides |= 8u;
	hitbox_offset_y = (uint16_t)OBJECT(other)->hitbox_offset_y;
	box_start = (uint16_t)(OBJECT(other)->y_int + hitbox_offset_y);
	if ((int16_t)box_start < OBJECT(current_object)->y_int)
	{
		if (OBJECT(current_object)->y_int < (int16_t)(box_start + OBJECT(other)->hitbox_size_y))
		{
			*p_hitbox_offset_y = (*p_hitbox_offset_y & 0xFFFF0000u) | hitbox_offset_y;
			*p_sides = (*p_sides & 0xFFFF0000u) | sides;
			return;
		}
		sides |= 1u;
		*p_hitbox_offset_y = (*p_hitbox_offset_y & 0xFFFF0000u) | hitbox_offset_y;
		*p_sides = (*p_sides & 0xFFFF0000u) | sides;
		return;
	}
	sides |= 2u;
	*p_hitbox_offset_y = (*p_hitbox_offset_y & 0xFFFF0000u) | hitbox_offset_y;
	*p_sides = (*p_sides & 0xFFFF0000u) | sides;
}

uint32_t set_speed_from_direction(void)
{
	uint32_t cos_speed = 0, direction = 0, speed_int = 0;

	direction = (int16_t)OBJECT(current_object)->direction;
	speed_int = (uint16_t)OBJECT(current_object)->speed_int;
	OBJECT(current_object)->speed_x = (int32_t)(sine_table[direction] * speed_int) >> 6;
	cos_speed = (int32_t)(cosine_table[direction] * speed_int) >> 6;
	OBJECT(current_object)->speed_y = 0u - cos_speed;
	return cos_speed;
}

void polar_to_xy(uint32_t *p_cos_part, uint32_t *p_angle, uint32_t *p_length)
{
	uint16_t angle = (uint16_t)*p_angle;
	uint16_t length = (uint16_t)*p_length;
	uint32_t angle_index = 0, cos_part = 0, dx = 0, dy = 0, magnitude = 0;

	angle_index = (int16_t)angle;
	magnitude = (uint16_t)length;
	dx = (int32_t)(sine_table[angle_index] * magnitude) >> 6;
	cos_part = (int32_t)(cosine_table[angle_index] * magnitude) >> 6;
	dy = 0u - cos_part;
	*p_cos_part = cos_part;
	*p_angle = dx;
	*p_length = dy;
}

uint32_t set_speed_from_player_speed(void)
{
	uint32_t cos_speed = 0, direction = 0, ground_speed_int = 0;

	direction = (int16_t)OBJECT(current_object)->direction;
	ground_speed_int = AT16(PLAYER(current_player)->ground_speed, 2);
	OBJECT(current_object)->speed_x = (int32_t)(sine_table[direction] * ground_speed_int) >> 6;
	cos_speed = (int32_t)(cosine_table[direction] * ground_speed_int) >> 6;
	OBJECT(current_object)->speed_y = 0u - cos_speed;
	return cos_speed;
}

void project_object(uint32_t *p_screen_y, uint32_t *p_depth)
{
	uint32_t projected_x = 0, projected_y = 0, screen_y = 0;

	projected_y = OBJECT(current_object)->y;
	projected_x = ((int32_t)(OBJECT(current_object)->x + projected_y) >> 8) * 0xB5u;
	projected_y = ((int32_t)(projected_y - OBJECT(current_object)->x) >> 9) * 0xB5u;
	OBJECT(current_object)->projected_x = (int32_t)projected_x >> 16;
	OBJECT(current_object)->projected_y = projected_y;
	update_screen_position(&screen_y, &projected_y);
	*p_screen_y = screen_y;
	*p_depth = projected_y;
}

void update_screen_position(uint32_t *p_screen_y, uint32_t *p_depth)
{
	uint16_t sprite_offset_y = 0;
	uint32_t screen_y = 0;

	OBJECT(current_object)->screen_x = OBJECT(current_object)->projected_x - view_x + OBJECT(current_object)->sprite_offset_x;
	screen_y = (int32_t)((uint32_t)OBJECT(current_object)->projected_y + OBJECT(current_object)->z) >> 16;
	screen_y = (screen_y & ~0xFFFFu) | (uint16_t)(screen_y - view_y);
	sprite_offset_y = (uint16_t)OBJECT(current_object)->sprite_offset_y;
	screen_y = (screen_y & ~0xFFFFu) | (uint16_t)(screen_y + sprite_offset_y);
	OBJECT(current_object)->screen_y = screen_y;
	*p_screen_y = screen_y;
	*p_depth = (*p_depth & 0xFFFF0000u) | sprite_offset_y;
}

void project_point(uint32_t z, uint32_t *p_projected_y, uint32_t *p_point_x, uint32_t *p_point_y)
{
	uint32_t point_x = *p_point_x;
	uint32_t point_y = *p_point_y;
	uint32_t projected_x_fixed = 0, world_y = 0;

	world_y = point_y;
	projected_x_fixed = ((int32_t)(point_x + world_y) >> 8) * 0xB5u;
	world_y = ((int32_t)(world_y - point_x) >> 9) * 0xB5u;
	projected_x_fixed = (int32_t)projected_x_fixed >> 16;
	point_to_screen((uint16_t)projected_x_fixed, z, &world_y, &point_x, &point_y);
	*p_projected_y = world_y;
	*p_point_x = (*p_point_x & 0xFFFF0000u) | (point_x & 0xFFFFu);
	*p_point_y = (*p_point_y & 0xFFFF0000u) | (point_y & 0xFFFFu);
}

void point_to_screen(uint16_t projected_x, uint32_t z, uint32_t *p_projected_y, uint32_t *p_screen_x, uint32_t *p_screen_y)
{
	uint32_t projected_y = *p_projected_y;
	uint16_t screen_x = 0, screen_y = 0;

	screen_x = (uint16_t)(projected_x - view_x);
	projected_y = (int32_t)(projected_y + z) >> 16;
	projected_y = (projected_y & ~0xFFFFu) | (uint16_t)(projected_y - view_y);
	screen_y = (uint16_t)projected_y;
	*p_projected_y = projected_y;
	*p_screen_x = (*p_screen_x & 0xFFFF0000u) | screen_x;
	*p_screen_y = (*p_screen_y & 0xFFFF0000u) | screen_y;
}

uint32_t project_object_2(void)
{
	uint32_t offset_x = 0, offset_y = 0, projected_x = 0, rotated_y = 0;

	rotated_y = OBJECT(current_object)->y;
	projected_x = ((int32_t)(OBJECT(current_object)->x + rotated_y) >> 8) * 0xB5u;
	rotated_y = ((int32_t)(rotated_y - OBJECT(current_object)->x) >> 9) * 0xB5u;
	OBJECT(current_object)->projected_x = (int32_t)projected_x >> 16;
	OBJECT(current_object)->projected_y = rotated_y;
	offset_x = (rotated_y & ~0xFFFFu) | (uint16_t)OBJECT(current_object)->sprite_offset_x;
	OBJECT(current_object)->screen_x = OBJECT(current_object)->projected_x - view_x + offset_x;
	if (scroll_position < 0xA5F0u)
	{
		offset_y = (offset_x & ~0xFFFFu) | (uint16_t)OBJECT(current_object)->sprite_offset_y;
		OBJECT(current_object)->screen_y = ((int32_t)((uint32_t)OBJECT(current_object)->projected_y + OBJECT(current_object)->z) >> 16) - view_y + offset_y;
		return offset_y;
	}
	offset_y = (offset_x & ~0xFFFFu) | (uint16_t)OBJECT(current_object)->sprite_offset_y;
	OBJECT(current_object)->screen_y = ((int32_t)((uint32_t)OBJECT(current_object)->projected_y + OBJECT(current_object)->z) >> 16) - PLAYER(current_player)->own_view_y + offset_y;
	return offset_y;
}

void find_nearby_floor_blocks(uint32_t *p_corner_y, uint32_t *p_floor_block, uint32_t *p_bounds_offset, uint32_t *p_nearby_count, uint32_t *p_corners_left, uint32_t *p_left, uint32_t *p_right)
{
	uint16_t corner_y = (uint16_t)*p_corner_y;
	uint16_t floor_block = (uint16_t)*p_floor_block;
	uint32_t bounds_offset = *p_bounds_offset;
	uint16_t nearby_count = (uint16_t)*p_nearby_count;
	uint32_t corners_left = *p_corners_left;
	uint16_t left = (uint16_t)*p_left;
	uint16_t right = (uint16_t)*p_right;
	uint32_t carry = 0;
	uint16_t bottom = 0, corner_x = 0, top = 0;
	uint32_t blocks_left = 0, bounds_cursor = 0, bounds_list = 0, list_cursor = 0;
	uint16_t box_left = 0, box_right = 0, box_top = 0;

	if (nearby_floor_blocks_locked != 0u)
		return;
	left = (uint16_t)view_x;
	right = (uint16_t)left;
	left -= 0x80u;
	right += 0x1C0u;
	top = (uint16_t)view_y;
	bottom = (uint16_t)top;
	top -= 0x80u;
	bottom += 0x160u;
	bounds_list = floor_block_bounds;
	blocks_left = ROM16(bounds_list);
	list_cursor = nearby_floor_blocks_ADDRESS;
	bounds_offset = 0u;
	nearby_count = 0u;
	floor_block = 0u;
	bounds_list += 2u;
	do
	{
		bounds_cursor = bounds_list + bounds_offset;
		box_left = ROM16(bounds_cursor);
		bounds_cursor += 2u;
		if ((int16_t)box_left <= (int16_t)right)
		{
			bounds_cursor += 2u;
			box_right = ROM16(bounds_cursor);
			bounds_cursor += 2u;
			if ((int16_t)left <= (int16_t)box_right)
			{
				bounds_cursor += 4u;
				box_top = ROM16(bounds_cursor);
				bounds_cursor += 2u;
				if ((int16_t)box_top <= (int16_t)bottom && (int16_t)top <= (int16_t)ROM16(bounds_cursor + 2u))
				{
					bounds_cursor = bounds_list + bounds_offset;
					corners_left = 4u;
					do
					{
						corner_x = ROM16(bounds_cursor);
						bounds_cursor += 2u;
						corner_y = ROM16(bounds_cursor);
						corner_x -= left;
						carry = corner_x < 0x240u;
						bounds_cursor += 2u;
						if (corner_x < 0x240u)
						{
							corner_y -= top;
							carry = corner_y < 0x1E0u;
						}
						corners_left--;
					} while ((corners_left != 0u) & (carry == 0u));
					if (carry)
					{
						nearby_count++;
						mem_write16(list_cursor, floor_block);
						list_cursor += 2u;
					}
				}
			}
		}
		bounds_offset = (uint16_t)(bounds_offset + 0x10u);
		floor_block += 0x16u;
		blocks_left--;
	} while (blocks_left != 0u);
	nearby_floor_block_count = nearby_count;
	*p_corner_y = (*p_corner_y & 0xFFFF0000u) | corner_y;
	*p_floor_block = (*p_floor_block & 0xFFFF0000u) | floor_block;
	*p_bounds_offset = bounds_offset;
	*p_nearby_count = (*p_nearby_count & 0xFFFF0000u) | nearby_count;
	*p_corners_left = corners_left;
	*p_left = (*p_left & 0xFFFF0000u) | left;
	*p_right = (*p_right & 0xFFFF0000u) | right;
}

void find_floor(uint32_t *p_x, uint32_t *p_y, uint32_t *p_block_offset, uint32_t *p_slope_height, uint32_t *p_cursor)
{
	uint32_t x = *p_x;
	uint32_t y = *p_y;
	uint32_t block_offset = *p_block_offset;
	uint32_t cursor = *p_cursor;
	uint32_t carry = 0;
	uint32_t base_z = 0, block = 0, blocks_left = 0, floor_type_index = 0, object = 0, record = 0, slope_factor = 0, slope_height = 0, slope_index = 0;
	uint32_t x_min = 0, x_max = 0, y_min = 0;

	if ((OBJECT(current_object)->flags & 8u) == 0u)
	{
		blocks_left = nearby_floor_block_count;
		cursor = nearby_floor_blocks_ADDRESS;
		block = floor_blocks + 2u;
		if (blocks_left == 0u)
		{
		find_floor_no_block:
			if (OBJECT(current_object)->floor_block >= 0 && OBJECT(current_object)->floor_z_int < 0x7FF)
			{
				OBJECT(current_object)->previous_floor_block = OBJECT(current_object)->floor_block;
				OBJECT(current_object)->previous_floor_z = OBJECT(current_object)->floor_z;
			}
			OBJECT(current_object)->floor_block = -1;
			block = no_floor_block_ADDRESS;
		}
		else
		{
			x = OBJECT(current_object)->x;
			y = OBJECT(current_object)->y;
			for (;;)
			{
				block_offset = mem_read16(cursor);
				record = block + block_offset;
				x_min = ROM32(record);
				cursor += 2u;
				record += 4u;
				if ((int32_t)x < (int32_t)x_min)
				{
				find_floor_next_block:
					blocks_left--;
					if (blocks_left != 0u)
						continue;
					goto find_floor_no_block;
				}
				x_max = ROM32(record);
				record += 4u;
				if ((int32_t)x_max < (int32_t)x)
					goto find_floor_next_block;
				y_min = ROM32(record);
				record += 4u;
				if ((int32_t)y < (int32_t)y_min || (int32_t)ROM32(record) < (int32_t)y)
					goto find_floor_next_block;
				break;
			}
			if (OBJECT(current_object)->floor_block >= 0 && OBJECT(current_object)->floor_z_int < 0x7FF)
			{
				OBJECT(current_object)->previous_floor_block = OBJECT(current_object)->floor_block;
				OBJECT(current_object)->previous_floor_z = OBJECT(current_object)->floor_z;
			}
			OBJECT(current_object)->floor_block = block_offset;
			block += block_offset;
			x = ROM8(block + 0x12u);
			record = floor_types_ADDRESS + x * 4u;
			OBJECT(current_object)->floor_type = ROM8(record);
			OBJECT(current_object)->floor_attribute = ROM16(record + 2u);
		}
		OBJECT(current_object)->floor_z_int = ROM16(block + 0x10u);
		OBJECT(current_object)->floor_z_fraction = 0u;
		OBJECT(current_object)->floor_slope = ROM8(block + 0x13u);
		slope_height = 0u;
		if (ROM8(block + 0x13u) == 0u)
		{
			*p_x = x;
			*p_y = y;
			*p_block_offset = block_offset;
			*p_slope_height = slope_height;
			*p_cursor = cursor;
			return;
		}
		slope_index = ROM8(block + 0x13u);
		cursor = floor_slopes_ADDRESS + (uint16_t)(slope_index * 2u) * 4u;
		if ((ROM32(cursor) & 2u) == 0u)
			slope_height = OBJECT(current_object)->y - ROM32(block + 8u);
		else
			slope_height = OBJECT(current_object)->x - ROM32(block);
		cursor += 2u;
		slope_factor = ROM32(cursor);
		carry = (slope_height >> 11) & 1u;
		slope_height = (int32_t)slope_height >> 12;
		cursor += 4u;
		if (carry)
			slope_height++;
		slope_height = (int32_t)(slope_height * slope_factor) >> 4;
		OBJECT(current_object)->floor_z += slope_height;
		*p_x = slope_factor;
		*p_y = y;
		*p_block_offset = block_offset;
		*p_slope_height = slope_height;
		*p_cursor = cursor;
		return;
	}
	if (OBJECT(current_object)->floor_block >= 0 && OBJECT(current_object)->floor_z_int < 0x7FF)
	{
		OBJECT(current_object)->previous_floor_block = OBJECT(current_object)->floor_block;
		OBJECT(current_object)->previous_floor_z = OBJECT(current_object)->floor_z;
	}
	object = LINKED(WORK_RAM, OBJECT(current_object)->platform);
	OBJECT(current_object)->floor_block = OBJECT(object)->floor_block;
	OBJECT(current_object)->floor_z = OBJECT(object)->z;
	block = floor_blocks + 2u + OBJECT(current_object)->floor_block;
	floor_type_index = mem_read8(block + 0x12u);
	record = floor_types_ADDRESS + floor_type_index * 4u;
	OBJECT(current_object)->floor_type = ROM8(record);
	OBJECT(current_object)->floor_attribute = ROM16(record + 2u);
	OBJECT(current_object)->floor_slope = mem_read8(block + 0x13u);
	slope_height = 0u;
	if (mem_read8(block + 0x13u) == 0u)
	{
		*p_x = floor_type_index;
		*p_slope_height = slope_height;
		return;
	}
	slope_index = mem_read8(block + 0x13u);
	cursor = floor_slopes_ADDRESS + (uint16_t)(slope_index * 2u) * 4u;
	if ((ROM32(cursor) & 2u) == 0u)
		slope_height = OBJECT(current_object)->y - mem_read32(block + 8u);
	else
		slope_height = OBJECT(current_object)->x - mem_read32(block);
	cursor += 2u;
	base_z = (uint32_t)mem_read16(block + 0x10u) << 16;
	slope_height = ((int32_t)slope_height >> 16) * ROM32(cursor) + base_z;
	OBJECT(current_object)->floor_z = slope_height;
	cursor += 4u;
	*p_x = base_z;
	*p_slope_height = slope_height;
	*p_cursor = cursor;
}

void get_floor_z_at_point(uint32_t x, uint32_t y, uint32_t *p_floor_z, uint32_t *p_block)
{
	uint32_t floor_z = *p_floor_z;
	uint32_t block = 0, block_offset = 0, blocks_left = 0, bounds = 0, cursor = 0, slope = 0, slope_distance = 0, slope_record = 0, test_x = 0, test_y = 0;
	uint32_t x_min = 0, x_max = 0, y_min = 0;

	blocks_left = nearby_floor_block_count;
	cursor = nearby_floor_blocks_ADDRESS;
	block = floor_blocks + 2u;
	if (blocks_left == 0u)
	{
		*p_block = block;
		return;
	}
	test_x = x;
	test_y = y;
	for (;;)
	{
		block_offset = mem_read16(cursor);
		bounds = block + block_offset;
		x_min = ROM32(bounds);
		cursor += 2u;
		bounds += 4u;
		if ((int32_t)test_x < (int32_t)x_min)
		{
		get_floor_z_at_point_next_block:
			blocks_left--;
			if (blocks_left != 0u)
				continue;
			block = no_floor_block_ADDRESS;
			floor_z = (uint32_t)ROM16(block + 0x10u) << 16;
			if (ROM8(block + 0x13u) == 0u)
			{
				*p_floor_z = floor_z;
				*p_block = block;
				return;
			}
			slope = ROM8(block + 0x13u);
			slope_record = floor_slopes_ADDRESS + (uint16_t)(slope * 2u) * 4u;
			if ((ROM32(slope_record) & 2u) == 0u)
				slope_distance = y - ROM32(block + 8u);
			else
				slope_distance = x - ROM32(block);
			floor_z += (slope_distance >> 16) * ROM32(slope_record + 2u);
			*p_floor_z = floor_z;
			*p_block = block;
			return;
		}
		x_max = ROM32(bounds);
		bounds += 4u;
		if ((int32_t)x_max < (int32_t)test_x)
			goto get_floor_z_at_point_next_block;
		y_min = ROM32(bounds);
		bounds += 4u;
		if ((int32_t)test_y < (int32_t)y_min || (int32_t)ROM32(bounds) < (int32_t)test_y)
			goto get_floor_z_at_point_next_block;
		break;
	}
	block += block_offset;
	floor_z = (uint32_t)ROM16(block + 0x10u) << 16;
	if (ROM8(block + 0x13u) == 0u)
	{
		*p_floor_z = floor_z;
		*p_block = block;
		return;
	}
	slope = ROM8(block + 0x13u);
	slope_record = floor_slopes_ADDRESS + (uint16_t)(slope * 2u) * 4u;
	if ((ROM32(slope_record) & 2u) == 0u)
		slope_distance = y - ROM32(block + 8u);
	else
		slope_distance = x - ROM32(block);
	floor_z += (slope_distance >> 16) * ROM32(slope_record + 2u);
	*p_floor_z = floor_z;
	*p_block = block;
}

uint32_t find_floor_block_at_point(uint32_t x, uint32_t y)
{
	uint32_t block = 0, block_offset = 0, blocks_left = 0, bounds = 0, cursor = 0, test_x = 0, test_y = 0;
	uint32_t x_min = 0, x_max = 0, y_min = 0;

	blocks_left = nearby_floor_block_count;
	cursor = nearby_floor_blocks_ADDRESS;
	block = floor_blocks + 2u;
	if (blocks_left == 0u)
		return block;
	test_x = x;
	test_y = y;
	do
	{
		block_offset = mem_read16(cursor);
		bounds = block + block_offset;
		x_min = ROM32(bounds);
		cursor += 2u;
		bounds += 4u;
		if ((int32_t)x_min <= (int32_t)test_x)
		{
			x_max = ROM32(bounds);
			bounds += 4u;
			if ((int32_t)test_x <= (int32_t)x_max)
			{
				y_min = ROM32(bounds);
				bounds += 4u;
				if ((int32_t)y_min <= (int32_t)test_y && (int32_t)test_y <= (int32_t)ROM32(bounds))
				{
					block += block_offset;
					return block;
				}
			}
		}
		blocks_left--;
	} while (blocks_left != 0u);
	block = no_floor_block_ADDRESS;
	return block;
}

void check_player_step(uint32_t *p_height_diff, uint32_t *p_cursor)
{
	uint32_t height_diff = *p_height_diff;
	uint32_t cursor = *p_cursor;
	uint32_t floor_block = 0, object = 0, object_z = 0, opposite_side = 0, x = 0, y = 0;
	uint16_t floor_z_int = 0, z_int = 0, height_diff_2 = 0;

	step_floor_z = OBJECT(current_object)->floor_z;
	if (stage != 8u || scroll_position >= 0x4000u)
	{
		if ((OBJECT(current_object)->flags & 8u) == 0u)
		{
		check_player_step_against_floor:
			if (PLAYER(current_player)->path != 0u)
			{
				PLAYER(current_player)->previous_z = OBJECT(current_object)->z;
				OBJECT(current_object)->z = step_floor_z;
				*p_height_diff = height_diff;
				return;
			}
			object_z = (height_diff & ~0xFFFFu) | (uint16_t)OBJECT(current_object)->z_int;
			floor_z_int = step_floor_z_int;
			z_int = object_z;
			height_diff = (object_z & ~0xFFFFu) | (uint16_t)(z_int - floor_z_int);
			if (z_int == floor_z_int)
			{
				PLAYER(current_player)->previous_z = OBJECT(current_object)->z;
				OBJECT(current_object)->z = step_floor_z;
				*p_height_diff = height_diff;
				return;
			}
			height_diff = (height_diff & ~0xFFFFu) | (uint16_t)(height_diff + 8u);
			if ((uint16_t)height_diff >= 0x11u)
			{
				height_diff_2 = height_diff - 8u;
				height_diff = (height_diff & ~0xFFFFu) | height_diff_2;
				if ((int16_t)height_diff_2 < 0)
				{
					if (PLAYER(current_player)->state == 2u)
					{
						*p_height_diff = height_diff;
						return;
					}
					PLAYER(current_player)->state = 2u;
					PLAYER(current_player)->substate = 0u;
					*p_height_diff = height_diff;
					return;
				}
				if (OBJECT(current_object)->floor_slope != 0u)
				{
					PLAYER(current_player)->previous_z = OBJECT(current_object)->z;
					OBJECT(current_object)->z = step_floor_z;
					*p_height_diff = height_diff;
					return;
				}
				OBJECT(current_object)->x = PLAYER(current_player)->previous_x;
				OBJECT(current_object)->y = PLAYER(current_player)->previous_y;
				step_blocking_floor_block = OBJECT(current_object)->floor_block;
				OBJECT(current_object)->floor_block = OBJECT(current_object)->previous_floor_block;
				OBJECT(current_object)->floor_z = OBJECT(current_object)->previous_floor_z;
				PLAYER(current_player)->control_lock = 0u;
				if (OBJECT(current_object)->speed_int >= 0x500)
				{
					floor_block = step_blocking_floor_block;
					x = OBJECT(current_object)->x;
					y = OBJECT(current_object)->y;
					opposite_side = get_floor_block_side(floor_block, x, y);
					opposite_side = (opposite_side & ~0xFFu) | (uint8_t)(opposite_side + 2u);
					AS16(PLAYER(current_player)->bounce_direction) = (opposite_side & ~0xFFu) | (opposite_side & 3u);
					PLAYER(current_player)->state = 0xEu;
					PLAYER(current_player)->substate = 0u;
					*p_height_diff = height_diff;
					*p_cursor = x;
					return;
				}
				height_diff = 0u;
				OBJECT(current_object)->speed = 0;
				OBJECT(current_object)->speed_x = 0;
				OBJECT(current_object)->speed_y = 0;
				PLAYER(current_player)->control_lock = 0u;
				PLAYER(current_player)->free_turn_timer = 3u;
				OBJECT(current_object)->flags |= 0x800000u;
				PLAYER(current_player)->state = 0u;
				if (step_floor_z <= OBJECT(current_object)->z)
				{
					*p_height_diff = height_diff;
					return;
				}
				if (PLAYER(current_player)->state == 2u)
				{
					*p_height_diff = height_diff;
					return;
				}
				PLAYER(current_player)->state = 2u;
				PLAYER(current_player)->substate = 0u;
				*p_height_diff = height_diff;
				return;
			}
			PLAYER(current_player)->previous_z = OBJECT(current_object)->z;
			OBJECT(current_object)->z = step_floor_z;
			*p_height_diff = height_diff;
			return;
		}
		object = LINKED(current_object, OBJECT(current_object)->platform);
		if (OBJECT(object)->floor_slope != 0u)
			goto check_player_step_against_floor;
		step_floor_z = OBJECT(object)->z;
		if (mem_read8(floor_blocks + 2u + OBJECT(current_object)->previous_floor_block + 0x12u) == 0x24u)
		{
			PLAYER(current_player)->previous_z = OBJECT(current_object)->z;
			OBJECT(current_object)->z = step_floor_z;
			*p_height_diff = height_diff;
			return;
		}
		goto check_player_step_against_floor;
	}
	PLAYER(current_player)->previous_z = OBJECT(current_object)->z;
	OBJECT(current_object)->z = step_floor_z;
	*p_height_diff = height_diff;
}

void bounce_off_wall_in_air(void)
{
	uint32_t floor_block = 0, new_direction = 0, x = 0, y = 0;

	if (OBJECT(current_object)->z_int <= OBJECT(current_object)->floor_z_int || (int16_t)(OBJECT(current_object)->previous_floor_z_int - OBJECT(current_object)->floor_z_int) < 8)
		return;
	OBJECT(current_object)->x = PLAYER(current_player)->previous_x;
	OBJECT(current_object)->y = PLAYER(current_player)->previous_y;
	OBJECT(current_object)->floor_z = OBJECT(current_object)->previous_floor_z;
	floor_block = OBJECT(current_object)->floor_block;
	OBJECT(current_object)->floor_block = OBJECT(current_object)->previous_floor_block;
	x = OBJECT(current_object)->x;
	y = OBJECT(current_object)->y;
	new_direction = get_floor_block_side(floor_block, x, y);
	new_direction = ((new_direction << 8) + 0x200u) & 0x3FFu;
	if ((int16_t)(((uint32_t)OBJECT(current_object)->direction << 6) - (new_direction << 6)) >= 0)
		new_direction = (uint16_t)(new_direction + 0x180u);
	else
		new_direction = (uint16_t)(new_direction - 0x180u);
	OBJECT(current_object)->direction = new_direction & 0x3FFu;
	face_movement_direction(0u);
	PLAYER(current_player)->control_lock = 0xAu;
	OBJECT(current_object)->flags |= 0x2000u;
}

uint32_t get_floor_block_side(uint32_t floor_block, uint32_t x, uint32_t y)
{
	uint32_t side = 0;
	uint32_t x_min = 0, x_max = 0;

	floor_block = floor_block + floor_blocks + 2u;
	x_min = mem_read32(floor_block);
	floor_block += 4u;
	if ((int32_t)x_min <= (int32_t)x)
	{
		x_max = mem_read32(floor_block);
		floor_block += 4u;
		if ((int32_t)x <= (int32_t)x_max)
		{
			if ((int32_t)mem_read32(floor_block) <= (int32_t)y)
				side = 2u;
			else
				side = 0u;
		}
		else
			side = 1u;
	}
	else
		side = 3u;
	return side;
}

void get_floor_block_side_or_corner(uint32_t block, uint32_t x, uint32_t y, uint32_t *p_outcode, uint32_t *p_side_code)
{
	uint32_t outcode = 0, side_code = 0;

	outcode = 0u;
	block = block + floor_blocks + 2u;
	if ((int32_t)mem_read32(block) <= (int32_t)x)
	{
		if ((int32_t)mem_read32(block + 4u) < (int32_t)x)
			outcode = outcode | 1u;
	}
	else
		outcode = outcode | 2u;
	if ((int32_t)mem_read32(block + 8u) <= (int32_t)y)
	{
		if ((int32_t)y <= (int32_t)mem_read32(block + 0xCu))
		{
			side_code = floor_block_side_codes[outcode];
			*p_outcode = outcode;
			*p_side_code = side_code;
			return;
		}
		outcode = outcode | 8u;
		side_code = floor_block_side_codes[outcode];
		*p_outcode = outcode;
		*p_side_code = side_code;
		return;
	}
	outcode = outcode | 4u;
	side_code = floor_block_side_codes[outcode];
	*p_outcode = outcode;
	*p_side_code = side_code;
}
