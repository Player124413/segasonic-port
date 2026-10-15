/**
 *    Name:        objects/signs.c
 *    Description: the signs (object_kinds): placed, shown, following; the unused screen signs and floor sign
 *    Version:     0.1.0
 *    Created:     12-10-2026
 *    Modified:    15-10-2026
 *    Author:      Sebastian Toyos
 *
 *    SegaSonic The Hedgehog (arcade, Sega System 32): the game's V60 program decompiled into C by
 *    tools/decompile.py, with the names of symbols.txt.
 */
#include "game.h"

void sign_start(void)
{
	uint32_t ignored = 0, kind_record = 0, sign = 0;

	kind_record = OBJECT(current_object)->sprite_frame * 0xCu + object_kinds_ADDRESS;
	OBJECT(current_object)->z_int = mem_read16(kind_record);
	OBJECT(current_object)->z_fraction = 0u;
	sign = current_object;
	kind_record += 2u;
	init_sign_from_kind(kind_record, &ignored, &sign);
}

void spawn_sign(uint16_t x, uint16_t y, uint16_t kind, uint16_t z, uint32_t *p_depth, uint32_t *p_sign)
{
	uint32_t depth = *p_depth;
	uint32_t sign = *p_sign;
	uint32_t r1 = 0, free_count = 0;

	allocate_object_outside_loop(&free_count, &sign);
	if (free_count == 0u)
	{
		*p_sign = sign;
		return;
	}
	place_sign(x, y, kind, z, &r1, &sign);
	*p_depth = r1;
	*p_sign = sign;
}

uint32_t spawn_sign_2(uint16_t x, uint16_t y, uint16_t kind, uint16_t z, uint32_t sign)
{
	uint32_t free_count = 0, ignored = 0;

	allocate_object(&free_count, &sign);
	if (free_count == 0u)
		return sign;
	place_sign(x, y, kind, z, &ignored, &sign);
	return sign;
}

void place_sign(uint16_t x, uint16_t y, uint32_t kind, uint16_t z, uint32_t *p_depth, uint32_t *p_sign)
{
	uint32_t sign = *p_sign;
	uint32_t r1 = 0;

	OBJECT(sign)->x_int = x;
	OBJECT(sign)->x_fraction = 0u;
	OBJECT(sign)->y_int = y;
	OBJECT(sign)->y_fraction = 0u;
	OBJECT(sign)->z_int = z;
	OBJECT(sign)->z_fraction = 0u;
	kind = (uint16_t)kind * 0xCu + 0x84FB7u;
	init_sign_from_kind(kind, &r1, &sign);
	*p_depth = r1;
	*p_sign = sign;
}

void init_sign_from_kind(uint32_t record, uint32_t *p_depth, uint32_t *p_sign)
{
	uint32_t sign = *p_sign;
	uint32_t depth = 0, ignored = 0;
	uint32_t saved_current_object = 0;

	OBJECT(sign)->sprite_frame = mem_read16(record);
	record += 2u;
	OBJECT(sign)->speed_x_fraction = mem_read16(record);
	record += 2u;
	AS32(OBJECT(sign)->var_80) = mem_read16(record);
	AS32(OBJECT(sign)->var_80) += 0x8514Du;
	OBJECT(sign)->handler = mem_read32(record + 2u);
	OBJECT(sign)->sprite_flags = 0x1000u;
	OBJECT(sign)->flags = 0u;
	OBJECT(sign)->sprite_offset_x = 0;
	OBJECT(sign)->sprite_offset_y = 0;
	AS8(OBJECT(sign)->var_84) = 0u;
	AT8(OBJECT(sign)->var_84, 1) = 4u;
	AS8(OBJECT(sign)->var_86) = 0u;
	OBJECT(sign)->parent = current_object;
	saved_current_object = current_object;
	current_object = sign;
	project_object(&ignored, &depth);
	sign = current_object;
	current_object = saved_current_object;
	*p_depth = depth;
	*p_sign = sign;
}

void sign_show_unless_mode_32(void)
{
	if (current_game_mode != 0x20u)
	{
		sign_show();
		return;
	}
	delete_current_object();
}

void sign_show_while_player_busy(void)
{
	uint32_t player = 0, player_2 = 0;

	player = OBJECT(LINKED(current_object, OBJECT(current_object)->parent))->player;
	player_2 = PLAYER_BLOCK(1) + (uint16_t)(player << 9);
	if (PLAYER(player_2)->state == 0x1Du || PLAYER(player_2)->state == 0x2Cu || PLAYER(player_2)->state == 0x32u)
	{
		sign_show();
		return;
	}
	delete_current_object();
}

void sign_show_until_goal_unlocked(void)
{
	if (goal_unlocked == 0u)
	{
		sign_show();
		return;
	}
	delete_current_object();
}

void sign_show_while_parent_still(void)
{
	if ((uint16_t)OBJECT(LINKED(current_object, OBJECT(current_object)->parent))->speed_z_int == 0u)
	{
		sign_show();
		return;
	}
	delete_current_object();
}

void sign_show_until_gate_closed(void)
{
	if (sign_stop_flag == 0u)
	{
		sign_show();
		return;
	}
	delete_current_object();
}

void sign_show_until_cage_bar_hit(void)
{
	if (cage_bar_hit == 0u)
	{
		sign_show_unless_stage_0();
		return;
	}
	delete_current_object();
}

void sign_show_unless_stage_0(void)
{
	if (stage != 0u)
	{
		sign_show();
		return;
	}
	delete_current_object();
}

void sign_show(void)
{
	uint8_t bucket = 0;
	uint32_t r1 = 0, ignored = 0;
	uint16_t speed_x_fraction = 0;

	update_screen_position(&ignored, &r1);
	animate_sign();
	speed_x_fraction = OBJECT(current_object)->speed_x_fraction;
	OBJECT(current_object)->speed_x_fraction = speed_x_fraction - 1u;
	if ((int16_t)speed_x_fraction > 1)
	{
		bucket = 0u;
		queue_sprite_in_bucket(bucket);
		return;
	}
	delete_current_object();
}

void sign_follow_parent(void)
{
	uint8_t bucket = 0;
	uint32_t object = 0;

	animate_sign();
	object = LINKED(current_object, OBJECT(current_object)->parent);
	OBJECT(current_object)->screen_x = OBJECT(object)->screen_x;
	OBJECT(current_object)->screen_y = ((int32_t)((uint32_t)OBJECT(object)->projected_y + OBJECT(object)->floor_z) >> 16) - view_y - 0x38u;
	if ((OBJECT(object)->flags & 0x40u) != 0u)
	{
		bucket = 0u;
		queue_sprite_in_bucket(bucket);
		return;
	}
	delete_current_object();
}

void sign_follow_player(void)
{
	uint8_t bucket = 0;
	uint32_t object = 0, player = 0;

	animate_sign();
	object = LINKED(current_object, OBJECT(current_object)->parent);
	OBJECT(current_object)->screen_x = OBJECT(object)->screen_x;
	OBJECT(current_object)->screen_y = ((int32_t)((uint32_t)OBJECT(object)->projected_y + OBJECT(object)->floor_z) >> 16) - view_y - 0x28u;
	player = PLAYER_BLOCK(1);
	if (OBJECT(object)->player != 0u)
	{
		player = PLAYER_BLOCK(2);
		if (OBJECT(object)->player != 1u)
			player = PLAYER_BLOCK(3);
	}
	if (PLAYER(player)->state == 0x22u)
	{
		bucket = 0u;
		queue_sprite_in_bucket(bucket);
		return;
	}
	delete_current_object();
}

void screen_sign_start(void)
{
	uint16_t sprite_frame = 0, x_int = 0, y_int = 0;
	uint32_t sign = 0;

	x_int = (uint16_t)OBJECT(current_object)->x_int;
	y_int = (uint16_t)OBJECT(current_object)->y_int;
	sprite_frame = OBJECT(current_object)->sprite_frame;
	sign = current_object;
	place_screen_sign(x_int, y_int, sprite_frame, sign);
}

void spawn_screen_sign(uint16_t screen_x, uint16_t screen_y, uint16_t kind, uint32_t sign)
{
	uint32_t free_count = 0;

	allocate_object_outside_loop(&free_count, &sign);
	if (free_count == 0u)
		return;
	place_screen_sign(screen_x, screen_y, kind, sign);
}

void spawn_screen_sign_2(uint16_t screen_x, uint16_t screen_y, uint16_t kind, uint32_t sign)
{
	uint32_t free_count = 0;

	allocate_object(&free_count, &sign);
	if (free_count == 0u)
		return;
	place_screen_sign(screen_x, screen_y, kind, sign);
}

void place_screen_sign(uint16_t screen_x, uint16_t screen_y, uint32_t kind, uint32_t sign)
{
	OBJECT(sign)->screen_x = screen_x;
	OBJECT(sign)->screen_y = screen_y;
	kind = (uint16_t)kind * 6u + screen_sign_kinds_ADDRESS;
	OBJECT(sign)->sprite_frame = ROM16(kind);
	kind += 2u;
	OBJECT(sign)->speed_x_fraction = ROM16(kind);
	OBJECT(sign)->screen_sign.palette_list = ROM16(kind + 2u);
	OBJECT(sign)->screen_sign.palette_list += 0x8514Du;
	OBJECT(sign)->sprite_flags = 0x1000u;
	OBJECT(sign)->flags = 0u;
	OBJECT(sign)->handler = screen_sign_show_ADDRESS;
	OBJECT(sign)->screen_sign.palette_index = 0u;
	OBJECT(sign)->screen_sign.palette_delay = 4u;
	OBJECT(sign)->screen_sign.palette_timer = 0u;
}

void screen_sign_show(void)
{
	uint8_t bucket = 0;
	uint16_t speed_x_fraction = 0;

	animate_sign();
	speed_x_fraction = OBJECT(current_object)->speed_x_fraction;
	OBJECT(current_object)->speed_x_fraction = speed_x_fraction - 1u;
	if ((int16_t)speed_x_fraction > 1)
	{
		bucket = 0u;
		queue_sprite_in_bucket(bucket);
		return;
	}
	delete_current_object();
}

void floor_sign_start(void)
{
	uint32_t block_offset = 0, cursor = 0, ignored = 0, x = 0, y = 0;

	OBJECT(current_object)->sprite_flags = 0x1004u;
	OBJECT(current_object)->sprite_palette = 0x8450u;
	OBJECT(current_object)->sprite_offset_x = 0;
	OBJECT(current_object)->sprite_offset_y = 0;
	OBJECT(current_object)->flags = 0u;
	OBJECT(current_object)->handler = floor_sign_update_ADDRESS;
	OBJECT(current_object)->sprite_frame = floor_sign_sprites[OBJECT(current_object)->sprite_frame];
	find_floor(&x, &y, &block_offset, &ignored, &cursor);
	OBJECT(current_object)->z = OBJECT(current_object)->floor_z;
	project_object(&ignored, &ignored);
}

void floor_sign_update(void)
{
	uint32_t r1 = 0, ignored = 0, off_screen = 0;

	off_screen = check_left_screen();
	if (off_screen == 0u)
	{
		update_screen_position(&ignored, &r1);
		r1 = queue_sprite(r1);
		return;
	}
	delete_current_object();
}
