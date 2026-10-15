/**
 *    Name:        objects/stage_04_props.c
 *    Description: stage 4: arrow boards, pebbles, the propped board, boulders, the door wall at the start and the rollers behind it
 *    Version:     0.1.0
 *    Created:     12-10-2026
 *    Modified:    15-10-2026
 *    Author:      Sebastian Toyos
 *
 *    SegaSonic The Hedgehog (arcade, Sega System 32): the game's V60 program decompiled into C by
 *    tools/decompile.py, with the names of symbols.txt.
 */
#include "game.h"

void arrow_board_start(void)
{
	uint32_t ignored = 0;

	OBJECT(current_object)->sprite_flags = 0x1002u;
	OBJECT(current_object)->sprite_frame = 0x6EB8u;
	OBJECT(current_object)->sprite_palette = 0x9190u;
	OBJECT(current_object)->sprite_offset_x = 0;
	OBJECT(current_object)->sprite_offset_y = 0;
	OBJECT(current_object)->flags = 0u;
	OBJECT(current_object)->handler = arrow_board_wait_for_platform_ADDRESS;
	project_object(&ignored, &ignored);
	arrow_board_wait_for_platform();
}

void arrow_board_wait_for_platform(void)
{
	if ((OBJECT(current_object)->flags & 8u) != 0u)
		OBJECT(current_object)->handler = arrow_board_ride_ADDRESS;
	add_to_rider_list();
}

void arrow_board_ride(void)
{
	uint32_t r1 = 0, depth = 0, ignored = 0, off_screen = 0, platform_z = 0;

	if (grey_screen_active == 0u)
	{
		if (grey_screen_ended != 0u)
			OBJECT(current_object)->sprite_palette = 0x9190u;
	}
	else
		OBJECT(current_object)->sprite_palette = 0x8400u;
	platform_z = OBJECT(LINKED(current_object, OBJECT(current_object)->platform))->z;
	OBJECT(current_object)->floor_z = platform_z;
	OBJECT(current_object)->z = platform_z;
	off_screen = check_left_screen();
	if (off_screen == 0u)
	{
		update_screen_position(&ignored, &r1);
		depth = (uint16_t)(OBJECT(current_object)->screen_y - 0x20u);
		queue_sprite_at_depth(&ignored, &depth);
		return;
	}
	delete_current_object();
}

void pebble_start(void)
{
	uint32_t block_offset = 0, cursor = 0, ignored = 0, x = 0, y = 0;

	OBJECT(current_object)->sprite_flags = 0x1004u;
	OBJECT(current_object)->sprite_frame = 0x6C60u;
	OBJECT(current_object)->sprite_palette = 0x9020u;
	OBJECT(current_object)->sprite_offset_x = 0;
	OBJECT(current_object)->sprite_offset_y = 0;
	OBJECT(current_object)->flags = 0u;
	OBJECT(current_object)->handler = pebble_update_ADDRESS;
	find_floor(&x, &y, &block_offset, &ignored, &cursor);
	OBJECT(current_object)->z = OBJECT(current_object)->floor_z;
	project_object(&ignored, &ignored);
}

void pebble_update(void)
{
	uint32_t r1 = 0, depth = 0, ignored = 0, off_screen = 0;

	off_screen = check_left_screen();
	if (off_screen == 0u)
	{
		update_screen_position(&ignored, &r1);
		depth = (uint16_t)(OBJECT(current_object)->screen_y - 0x10u);
		queue_sprite_at_depth(&ignored, &depth);
		return;
	}
	delete_current_object();
}

void propped_board_start(void)
{
	uint16_t prop_dy = 0;
	uint32_t ignored = 0, object = 0;
	uint16_t props_left_before = 0;

	OBJECT(current_object)->sprite_flags = 0x1000u;
	OBJECT(current_object)->sprite_frame = 0x6EA8u;
	OBJECT(current_object)->sprite_palette = 0x9100u;
	OBJECT(current_object)->hitbox_size_x = 0x10u;
	OBJECT(current_object)->hitbox_size_y = 0x12Bu;
	OBJECT(current_object)->hitbox_height = 0xE0u;
	OBJECT(current_object)->sprite_offset_x = 0;
	OBJECT(current_object)->sprite_offset_y = -0x2C;
	OBJECT(current_object)->flags = 2u;
	OBJECT(current_object)->propped_board.shake_strength = 0;
	OBJECT(current_object)->propped_board.shake_dx = 0;
	OBJECT(current_object)->hitbox_offset_x = 0;
	OBJECT(current_object)->hitbox_offset_y = -0x80;
	OBJECT(current_object)->speed_z = 0;
	OBJECT(current_object)->z = 0x2300000;
	OBJECT(current_object)->kind = 0x11u;
	OBJECT(current_object)->handler = propped_board_update_ADDRESS;
	propped_board_object = current_object;
	project_object(&ignored, &ignored);
	if (free_object_stack < 2)
		return;
	board_props_left = 2;
	do
	{
		allocate_object(&ignored, &object);
		OBJECT(object)->sprite_flags = 0x1006u;
		OBJECT(object)->sprite_frame = 0x6EC0u;
		OBJECT(object)->sprite_palette = 0x91B0u;
		OBJECT(object)->sprite_offset_x = 0;
		OBJECT(object)->sprite_offset_y = 6;
		OBJECT(object)->flags = 2u;
		OBJECT(object)->hitbox_size_x = 0x30u;
		OBJECT(object)->hitbox_size_y = 0x20u;
		OBJECT(object)->hitbox_height = 0x50u;
		OBJECT(object)->hitbox_offset_x = -2;
		OBJECT(object)->hitbox_offset_y = -0xC;
		OBJECT(object)->kind = 0xCu;
		OBJECT(object)->x = OBJECT(current_object)->x;
		OBJECT(object)->x_int -= 0x20u;
		OBJECT(object)->y = OBJECT(current_object)->y;
		OBJECT(object)->z = OBJECT(current_object)->z;
		prop_dy = 0xFF90u;
		if ((uint16_t)board_props_left != 2u)
			prop_dy = (uint16_t)(0u - prop_dy);
		OBJECT(object)->y_int += prop_dy;
		OBJECT(object)->handler = board_prop_start_ADDRESS;
		props_left_before = board_props_left;
		board_props_left = props_left_before - 1u;
	} while ((int16_t)props_left_before > 1);
}

void propped_board_update(void)
{
	uint32_t r1 = 0, depth = 0, ignored = 0, shake_dx = 0;
	uint16_t shake_strength = 0;
	uint32_t flags = 0;

	OBJECT(current_object)->health = 0xA;
	if ((uint16_t)(OBJECT(current_object)->screen_x + 0x88u) >= 0x250u)
	{
		if ((OBJECT(current_object)->flags & 0x800u) != 0u)
		{
			delete_current_object();
			return;
		}
	}
	else
		OBJECT(current_object)->flags |= 0x800u;
	if (OBJECT(current_object)->x_int <= OBJECT(LINKED(current_object, group_leader_1))->x_int)
		OBJECT(current_object)->speed_z += 0x2000u;
	OBJECT(current_object)->z += OBJECT(current_object)->speed_z;
	update_screen_position(&shake_dx, &r1);
	if ((uint32_t)OBJECT(current_object)->speed_z == 0u)
		OBJECT(current_object)->propped_board.draw_depth = OBJECT(current_object)->screen_y - 0x10u;
	if (OBJECT(current_object)->screen_y >= 0x180)
	{
		delete_current_object();
		return;
	}
	if ((OBJECT(current_object)->flags & 4u) != 0u)
	{
		AS8(OBJECT(current_object)->propped_board.shake_strength) = 0x3Cu;
		flags = OBJECT(current_object)->flags;
		OBJECT(current_object)->flags = flags | 1u;
		if ((flags & 1u) == 0u)
			release_board_rings();
	}
	if ((uint16_t)OBJECT(current_object)->propped_board.shake_strength == 0u)
	{
		add_to_touchable_list(shake_dx);
		depth = OBJECT(current_object)->propped_board.draw_depth;
		queue_sprite_at_depth(&ignored, &depth);
		return;
	}
	shake_dx = (uint16_t)OBJECT(current_object)->propped_board.shake_strength >> 4;
	OBJECT(current_object)->propped_board.shake_step++;
	if ((OBJECT(current_object)->propped_board.shake_step & 1u) == 0u)
		shake_dx = (uint16_t)(0u - shake_dx);
	OBJECT(current_object)->screen_x += shake_dx;
	OBJECT(current_object)->propped_board.shake_dx = shake_dx;
	shake_strength = OBJECT(current_object)->propped_board.shake_strength;
	OBJECT(current_object)->propped_board.shake_strength = shake_strength - 2u;
	if ((int16_t)shake_strength <= 2)
		OBJECT(current_object)->propped_board.shake_strength = 0;
	add_to_touchable_list(shake_dx);
	depth = OBJECT(current_object)->propped_board.draw_depth;
	queue_sprite_at_depth(&ignored, &depth);
}

void board_prop_start(void)
{
	uint32_t ignored = 0;

	project_object(&ignored, &ignored);
	OBJECT(current_object)->handler = board_prop_update_ADDRESS;
}

void board_prop_update(void)
{
	uint32_t r1 = 0, depth = 0, ignored = 0, object = 0, off_screen = 0;

	off_screen = check_left_screen();
	if (off_screen != 0u)
	{
		delete_current_object();
		return;
	}
	OBJECT(current_object)->z += OBJECT(LINKED(current_object, propped_board_object))->speed_z;
	update_screen_position(&ignored, &r1);
	object = LINKED(current_object, propped_board_object);
	if ((uint32_t)OBJECT(object)->speed_z == 0u)
		OBJECT(current_object)->board_prop.draw_depth = OBJECT(current_object)->screen_y - 0xBu;
	OBJECT(current_object)->screen_x += AS16(OBJECT(object)->var_80);
	add_to_touchable_list(ignored);
	depth = OBJECT(current_object)->board_prop.draw_depth;
	queue_sprite_at_depth(&ignored, &depth);
}

void release_board_rings(void)
{
	uint32_t allocated = 0, count = 0, random_value = 0;
	uint32_t saved_x = 0, saved_y = 0, saved_z = 0;
	uint16_t rings_left_before = 0;

	saved_x = OBJECT(current_object)->x;
	saved_y = OBJECT(current_object)->y;
	saved_z = OBJECT(current_object)->z;
	OBJECT(current_object)->y_int -= 0x40u;
	OBJECT(current_object)->x_int -= 0x40u;
	OBJECT(current_object)->z_int -= 0x90u;
	board_rings_left = 8;
	do
	{
		count = 1u;
		scatter_rings_fixed(count, &random_value, &allocated);
		OBJECT(current_object)->y_int += 0x20u;
		rings_left_before = board_rings_left;
		board_rings_left = rings_left_before - 1u;
	} while ((int16_t)rings_left_before > 1);
	OBJECT(current_object)->z = saved_z;
	OBJECT(current_object)->y = saved_y;
	OBJECT(current_object)->x = saved_x;
}

void boulder_start(void)
{
	OBJECT(current_object)->sprite_flags = 4u;
	OBJECT(current_object)->sprite_palette = 0x90F0u;
	OBJECT(current_object)->sprite_scale = 0x200u;
	OBJECT(current_object)->hitbox_size_x = 0x80u;
	OBJECT(current_object)->hitbox_size_y = 0xC0u;
	OBJECT(current_object)->hitbox_height = 0x60u;
	OBJECT(current_object)->kind = 9u;
	OBJECT(current_object)->sprite_offset_x = 0x20;
	OBJECT(current_object)->sprite_offset_y = 0x10;
	OBJECT(current_object)->flags = 2u;
	OBJECT(current_object)->speed_z = 0;
	OBJECT(current_object)->screen_y = 0;
	OBJECT(current_object)->hitbox_offset_x = -0x20;
	OBJECT(current_object)->hitbox_offset_y = -0x60;
	OBJECT(current_object)->direction = 0x300u;
	OBJECT(current_object)->animation = boulder_animation_ADDRESS;
	OBJECT(current_object)->animation_frame = 0u;
	OBJECT(current_object)->animation_timer = 0;
	OBJECT(current_object)->animation_speed = 4u;
	OBJECT(current_object)->handler = boulder_wait_ADDRESS;
	OBJECT(current_object)->boulder.wait_timer = 0x78;
	OBJECT(current_object)->speed = 0x30000;
	group_leader_0_held = 0xFFu;
	add_to_rider_list();
}

void boulder_wait(void)
{
	uint16_t wait_timer = 0;

	if (scroll_position >= 0x3B00u)
	{
		OBJECT(current_object)->handler = boulder_roll_ADDRESS;
		boulder_roll();
		return;
	}
	if (scroll_position < 0x3800u)
	{
		add_to_rider_list();
		return;
	}
	wait_timer = OBJECT(current_object)->boulder.wait_timer;
	OBJECT(current_object)->boulder.wait_timer = wait_timer - 1u;
	if ((int16_t)wait_timer <= 1)
	{
		OBJECT(current_object)->handler = boulder_roll_ADDRESS;
		boulder_roll();
		return;
	}
	add_to_rider_list();
}

void boulder_roll(void)
{
	uint32_t r1 = 0, r2 = 0, r7 = 0, r8 = 0, cursor = 0, ignored = 0, sound_id = 0;

	OBJECT(current_object)->health = 0x7F;
	if ((OBJECT(current_object)->flags & 1u) == 0u && OBJECT(current_object)->x_int <= 0x180)
	{
		OBJECT(current_object)->flags |= 1u;
		camera_shake_active = 1u;
		sound_id = 0xFu;
		play_sound((uint8_t)sound_id, &r1, &r2);
	}
	group_leader_0_held = 0u;
	OBJECT(current_object)->x = OBJECT(LINKED(current_object, group_leaders[0]))->x;
	OBJECT(current_object)->x_int -= 0x30u;
	OBJECT(current_object)->speed_x = 0u - ((uint32_t)(uint16_t)chaser_speed << 12);
	OBJECT(current_object)->speed_y = 0;
	find_floor(&sound_id, &r1, &r2, &ignored, &cursor);
	if ((int16_t)AS16(OBJECT(current_object)->floor_block) < 0)
	{
		OBJECT(current_object)->speed_z += 0x6000u;
		OBJECT(current_object)->z += OBJECT(current_object)->speed_z;
	}
	else
	{
		OBJECT(current_object)->z = OBJECT(current_object)->floor_z;
		if ((uint16_t)OBJECT(current_object)->z_int == 0x7FFu)
			OBJECT(current_object)->z_int = 0x230;
	}
	animate(&r1, &r7, &r8);
	project_object(&ignored, &ignored);
	if (OBJECT(current_object)->screen_y < 0x180)
	{
		add_to_touchable_list(ignored);
		add_to_rider_list();
		ignored = queue_sprite_at_previous_floor();
		return;
	}
	delete_current_object();
}

void door_wall_start(void)
{
	uint32_t block_offset = 0, cursor = 0, ignored = 0, x = 0, y = 0;

	OBJECT(current_object)->sprite_flags = 0x1000u;
	OBJECT(current_object)->sprite_palette = 0x9110u;
	OBJECT(current_object)->sprite_frame = 0x6EB0u;
	OBJECT(current_object)->door_wall.fragment = door_fragments_ADDRESS;
	OBJECT(current_object)->door_wall.pieces_left = 0x10;
	OBJECT(current_object)->sprite_offset_x = 0;
	OBJECT(current_object)->sprite_offset_y = 0;
	OBJECT(current_object)->flags = 0u;
	OBJECT(current_object)->x = -0xE2E0000;
	OBJECT(current_object)->y = 0xF310000;
	OBJECT(current_object)->handler = door_wall_wait_to_burst_ADDRESS;
	find_floor(&x, &y, &block_offset, &ignored, &cursor);
	OBJECT(current_object)->z = OBJECT(current_object)->floor_z;
	project_object(&ignored, &ignored);
}

void door_wall_wait_to_burst(void)
{
	uint32_t carry = 0;
	uint32_t r1 = 0, fragment_animation = 0, free_count = 0, ignored = 0, object = 0, random_bits = 0, random_offsets = 0;
	uint8_t pieces_left = 0;

	if ((int8_t)scene_step < 3)
	{
		update_screen_position(&ignored, &r1);
		r1 = queue_sprite(r1);
		return;
	}
	for (;;)
	{
		pieces_left = OBJECT(current_object)->door_wall.pieces_left;
		OBJECT(current_object)->door_wall.pieces_left = pieces_left - 1u;
		if ((int8_t)pieces_left < 1)
			break;
		allocate_object(&free_count, &object);
		if (free_count == 0u)
			break;
		OBJECT(object)->screen_x = OBJECT(current_object)->screen_x;
		OBJECT(object)->screen_y = OBJECT(current_object)->screen_y;
		next_random(&random_bits, &ignored, &ignored);
		random_offsets = (random_bits & ~0xFFFFu) | (random_bits & 0x3Fu);
		OBJECT(object)->screen_x += random_offsets;
		OBJECT(object)->screen_y += (random_offsets >> 16) & 0x3Fu;
		random_bits = (random_bits & ~0xFFFFu) | (random_bits & 0xFFu);
		random_bits = (random_bits & ~0xFFFFu) | (uint16_t)(random_bits + 4u);
		OBJECT(object)->speed_x_fraction = random_bits;
		carry = (random_bits >> 16) & 1u;
		random_bits = random_bits >> 17;
		if (carry)
		{
			random_bits = random_bits & 0xFFu;
			random_bits = (uint16_t)(0u - random_bits);
			random_bits = (uint16_t)(random_bits - 0x10u);
		}
		else
		{
			random_bits = random_bits & 0xFFu;
			random_bits = (uint16_t)(random_bits + 4u);
		}
		OBJECT(object)->speed_y_fraction = random_bits;
		fragment_animation = OBJECT(current_object)->door_wall.fragment + 8u;
		OBJECT(object)->animation = rock_debris_animations_ADDRESS + mem_read16(fragment_animation);
		OBJECT(current_object)->door_wall.fragment = fragment_animation + 2u;
		OBJECT(object)->sprite_flags = 0u;
		OBJECT(object)->sprite_palette = 0x8240u;
		OBJECT(object)->projected_x = (uint32_t)OBJECT(object)->screen_x << 4;
		OBJECT(object)->projected_y_fraction = (uint32_t)OBJECT(object)->screen_y << 4;
		OBJECT(object)->animation_frame = 0u;
		OBJECT(object)->animation_speed = 4u;
		OBJECT(object)->animation_timer = 0;
		OBJECT(object)->flags = 0u;
		OBJECT(object)->sprite_scale = 0x100u;
		OBJECT(object)->handler = rock_debris_fly_ADDRESS;
	}
	delete_current_object();
}

void spawn_door_rollers(uint32_t *p_block_offset, uint32_t *p_slope_height, uint32_t *p_object)
{
	uint32_t block_offset = *p_block_offset;
	uint32_t slope_height = *p_slope_height;
	uint32_t object = *p_object;
	uint32_t free_count = 0, layout_record = 0, y = 0;
	uint16_t rollers_left_before = 0;

	door_rollers_left = 4;
	do
	{
		allocate_object_outside_loop(&free_count, &object);
		if (free_count == 0u)
		{
			*p_block_offset = block_offset;
			*p_slope_height = slope_height;
			*p_object = object;
			return;
		}
		current_object = object;
		layout_record = door_roller_layout_ADDRESS + (uint16_t)door_rollers_left * 3u * 2u;
		OBJECT(current_object)->x_int = ROM16(layout_record);
		OBJECT(current_object)->x_fraction = 0u;
		layout_record += 2u;
		OBJECT(current_object)->y_int = ROM16(layout_record);
		OBJECT(current_object)->y_fraction = 0u;
		OBJECT(current_object)->animation_frame = ROM8(layout_record + 2u);
		OBJECT(current_object)->sprite_flags = 0x1004u;
		OBJECT(current_object)->sprite_palette = 0x82F0u;
		OBJECT(current_object)->hitbox_size_x = 0x10u;
		OBJECT(current_object)->hitbox_size_y = 0x10u;
		OBJECT(current_object)->hitbox_height = 0x28u;
		OBJECT(current_object)->kind = 4u;
		OBJECT(current_object)->handler = door_roller_update_ADDRESS;
		OBJECT(current_object)->sprite_offset_x = 0;
		OBJECT(current_object)->sprite_offset_y = 5;
		OBJECT(current_object)->flags = 2u;
		OBJECT(current_object)->speed_z = -0x80000;
		OBJECT(current_object)->var_80 = 0u;
		OBJECT(current_object)->var_82 = 8u;
		AT8(OBJECT(current_object)->var_82, 1) = 0u;
		OBJECT(current_object)->animation_speed = 4u;
		OBJECT(current_object)->animation_timer = 0;
		OBJECT(current_object)->animation = spiked_roller_animation_ADDRESS;
		OBJECT(current_object)->sprite_frame = spiked_roller_frames[OBJECT(current_object)->animation_frame];
		find_floor(&free_count, &y, &block_offset, &slope_height, &object);
		OBJECT(current_object)->z = OBJECT(current_object)->floor_z;
		rollers_left_before = door_rollers_left;
		door_rollers_left = rollers_left_before - 1u;
	} while ((int16_t)rollers_left_before > 1);
	*p_block_offset = block_offset;
	*p_slope_height = slope_height;
	*p_object = object;
}

void door_roller_update(void)
{
	uint32_t r7 = 0, r8 = 0, frame = 0, ignored = 0;

	switch (OBJECT(current_object)->door_roller.state)
	{
	case 0:
		door_roller_wait();
		break;
	case 1:
		door_roller_drop();
		break;
	case 2:
		door_roller_roll_out();
		break;
	case 3:
		door_roller_ride();
		break;
	case 4:
		door_roller_wait();
		break;
	default:
		unknown_code(0x82AC6u + OBJECT(current_object)->door_roller.state * 3u);
	}
	if (AT8(OBJECT(current_object)->var_82, 1) != 0u)
	{
		delete_current_object();
		return;
	}
	animate(&frame, &r7, &r8);
	project_object(&ignored, &ignored);
	if (OBJECT(current_object)->var_80 == 0u)
		return;
	ignored = queue_sprite_at_base(ignored);
}

void door_roller_wait(void)
{
	OBJECT(current_object)->health = 0x7F;
	if ((int8_t)scene_step >= 3)
		OBJECT(current_object)->door_roller.state++;
}

void door_roller_drop(void)
{
	uint32_t r1 = 0, r7 = 0, r8 = 0, block_offset = 0, cursor = 0, ignored = 0, x = 0;

	OBJECT(current_object)->health = 0x7F;
	OBJECT(current_object)->speed_z += 0x6000u;
	OBJECT(current_object)->z += OBJECT(current_object)->speed_z;
	OBJECT(current_object)->y_int += 5u;
	animate(&r1, &r7, &r8);
	find_floor(&x, &r1, &block_offset, &ignored, &cursor);
	if (OBJECT(current_object)->z_int < OBJECT(current_object)->floor_z_int)
	{
		add_to_rider_list();
		return;
	}
	OBJECT(current_object)->door_roller.state++;
	OBJECT(current_object)->z = OBJECT(current_object)->floor_z;
	if (OBJECT(current_object)->z_int >= 0x800)
	{
		delete_current_object();
		return;
	}
	camera_shake_active = 1u;
	add_to_rider_list();
}

void door_roller_roll_out(void)
{
	uint16_t roll_out_timer = 0;

	OBJECT(current_object)->y_int++;
	roll_out_timer = OBJECT(current_object)->door_roller_roll_out.roll_out_timer;
	OBJECT(current_object)->door_roller_roll_out.roll_out_timer = roll_out_timer - 1u;
	if ((int16_t)roll_out_timer <= 1)
	{
		OBJECT(current_object)->flags |= 0x400000u;
		OBJECT(current_object)->door_roller_roll_out.state++;
	}
}

void door_roller_ride(void)
{
	uint32_t z = 0;

	if (OBJECT(current_object)->screen_y < 0x130)
	{
		z = OBJECT(LINKED(current_object, OBJECT(current_object)->platform))->z;
		OBJECT(current_object)->floor_z = z;
		OBJECT(current_object)->z = z;
		add_to_rider_list();
		return;
	}
	OBJECT(current_object)->door_roller.off_screen = 0xFFu;
}
