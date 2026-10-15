/**
 *    Name:        objects/cactus.c
 *    Description: object type 19 (stage A): the cactus, and the whirlwinds the tornado throws
 *    Version:     0.1.0
 *    Created:     12-10-2026
 *    Modified:    15-10-2026
 *    Author:      Sebastian Toyos
 *
 *    SegaSonic The Hedgehog (arcade, Sega System 32): the game's V60 program decompiled into C by
 *    tools/decompile.py, with the names of symbols.txt.
 */
#include "game.h"

void cactus_init(void)
{
	OBJECT(current_object)->sprite_frame = 0x77D8u;
	OBJECT(current_object)->sprite_palette = 0x9070u;
	OBJECT(current_object)->handler = cactus_update_ADDRESS;
	if (OBJECT(current_object)->spawn_arg_1 != 0u)
		OBJECT(current_object)->sprite_flags |= 0x40u;
	current_object = LINKED(current_object, OBJECT(current_object)->child);
	OBJECT(current_object)->sprite_frame = 0x5D80u;
	OBJECT(current_object)->sprite_palette = 0x87A0u;
	OBJECT(current_object)->sprite_scale = 0x60u;
}

void cactus_update(void)
{
	uint32_t r2 = 0, bucket = 0, cursor = 0, deleted = 0, direction = 0, ignored = 0, slope_height = 0, speed_x = 0, speed_y = 0, spin_speed = 0;

	move_object_projected(&ignored, &r2, &slope_height, &cursor);
	delete_if_off_screen(&deleted, &r2, &slope_height);
	if ((uint8_t)deleted == 1u)
		return;
	if (OBJECT(current_object)->cactus.lifted != 0u)
	{
		OBJECT(current_object)->cactus.angle -= 0x10u;
		OBJECT(current_object)->cactus.angle &= 0x3FFu;
		OBJECT(current_object)->cactus.spiral_speed += 0x18u;
		direction = OBJECT(current_object)->cactus.angle;
		spin_speed = OBJECT(current_object)->cactus.spiral_speed;
		direction_to_speed(spin_speed, &direction, &speed_x, &speed_y);
		OBJECT(current_object)->speed_x = speed_x;
		OBJECT(current_object)->speed_y = speed_y;
		OBJECT(current_object)->speed_z -= 0xC00u;
	}
	else
	{
		r14 = tornado_object;
		distance_to_object();
		if (nearest_player_distance <= 0x60)
		{
			OBJECT(current_object)->cactus.lifted = 1u;
			OBJECT(current_object)->sprite_frame = 0x77E0u;
			OBJECT(current_object)->speed_z = -0xC001;
			OBJECT(current_object)->cactus.angle -= 0x10u;
			OBJECT(current_object)->cactus.angle &= 0x3FFu;
			OBJECT(current_object)->cactus.spiral_speed += 0x18u;
			direction = OBJECT(current_object)->cactus.angle;
			spin_speed = OBJECT(current_object)->cactus.spiral_speed;
			direction_to_speed(spin_speed, &direction, &speed_x, &speed_y);
			OBJECT(current_object)->speed_x = speed_x;
			OBJECT(current_object)->speed_y = speed_y;
			OBJECT(current_object)->speed_z -= 0xC00u;
			queue_sprite_at_floor();
			current_object = LINKED(current_object, OBJECT(current_object)->child);
			bucket = place_sprite_at_cursor_floor();
			queue_sprite_in_bucket(bucket);
			return;
		}
		fall_to_floor(&ignored, &spin_speed, &r2);
		if (nearest_player_distance <= 0x60 && (uint16_t)OBJECT(current_object)->floor_z_int == (uint16_t)OBJECT(current_object)->z_int)
			OBJECT(current_object)->z_int -= 3u;
	}
	queue_sprite_at_floor();
	current_object = LINKED(current_object, OBJECT(current_object)->child);
	bucket = place_sprite_at_cursor_floor();
	queue_sprite_in_bucket(bucket);
}

uint32_t spawn_whirlwind(uint32_t spawned, uint32_t free_count, uint32_t whirlwind, uint32_t object)
{
	uint32_t piece_count = 0;

	spawn_count = 1u;
	spawn_object_with_pieces(0x01217Cu, &spawned, &piece_count, &free_count, &whirlwind, &object);
	if ((uint8_t)spawned != 0u)
	{
		current_object = whirlwind;
		copy_position_to(whirlwind);
		whirlwind_spin_step += 0xBu;
		whirlwind_spin_step &= 0x3Fu;
		OBJECT(current_object)->sprite_palette = 0x90F0u;
		OBJECT(current_object)->var_86 = whirlwind_spin_step;
		OBJECT(current_object)->var_81 = OBJECT(current_object)->var_81;
		OBJECT(current_object)->hitbox_size_x = 8u;
		OBJECT(current_object)->hitbox_height = 0x28u;
		OBJECT(current_object)->kind = 0x1Fu;
		OBJECT(current_object)->handler = whirlwind_aim_ADDRESS;
	}
	return object;
}

void whirlwind_aim(void)
{
	uint16_t farthest_distance = 0;

	face_nearest_player(0u, 0u);
	distance_to_player_1++;
	distance_to_player_2++;
	distance_to_player_3++;
	farthest_distance = (uint16_t)distance_to_player_1;
	OBJECT(current_object)->direction = direction_to_player_1;
	if ((int16_t)farthest_distance < distance_to_player_2)
	{
		farthest_distance = (uint16_t)(distance_to_player_2 + 1u);
		OBJECT(current_object)->direction = direction_to_player_2;
	}
	if ((int16_t)farthest_distance < distance_to_player_3)
		OBJECT(current_object)->direction = direction_to_player_2;
	OBJECT(current_object)->speed = (uint16_t)nearest_player_distance * 3u;
	OBJECT(current_object)->whirlwind.timer = 0xF0;
	OBJECT(current_object)->handler = whirlwind_move_ADDRESS;
	whirlwind_move();
}

void whirlwind_move(void)
{
	uint32_t aim_speed = 0, block_offset = 0, cursor = 0, deleted = 0, direction = 0, frame_index = 0, ignored = 0, slope_height = 0, speed = 0, speed_x = 0, speed_y = 0, spin_angle = 0;

	if (OBJECT(current_object)->whirlwind.timer <= 0)
	{
		OBJECT(current_object)->speed_x = 0;
		OBJECT(current_object)->speed_y = 0;
		OBJECT(current_object)->whirlwind.frame_index = 0;
		OBJECT(current_object)->handler = whirlwind_vanish_ADDRESS;
		whirlwind_vanish();
		return;
	}
	OBJECT(current_object)->whirlwind.timer--;
	if ((OBJECT(current_object)->flags & 4u) != 0u)
	{
		OBJECT(current_object)->speed_x = 0;
		OBJECT(current_object)->speed_y = 0;
		OBJECT(current_object)->whirlwind.frame_index = 0;
		OBJECT(current_object)->handler = whirlwind_vanish_ADDRESS;
		whirlwind_vanish();
		return;
	}
	move_object_projected(&ignored, &block_offset, &slope_height, &cursor);
	delete_if_off_screen(&deleted, &block_offset, &slope_height);
	if ((uint8_t)deleted != 1u)
	{
		spin_angle = (OBJECT(current_object)->whirlwind.spin_angle + OBJECT(current_object)->whirlwind.spin_step) & 0x3FFu;
		OBJECT(current_object)->whirlwind.spin_angle = spin_angle;
		speed = 0x100u;
		direction_to_speed(speed, &spin_angle, &speed_x, &speed_y);
		OBJECT(current_object)->speed_x = speed_x;
		OBJECT(current_object)->speed_y = speed_y;
		direction = OBJECT(current_object)->direction;
		aim_speed = OBJECT(current_object)->speed;
		direction_to_speed(aim_speed, &direction, &speed_x, &speed_y);
		OBJECT(current_object)->speed_x += speed_x;
		OBJECT(current_object)->speed_y += speed_y;
		frame_index = (uint8_t)OBJECT(current_object)->whirlwind.frame_index;
		frame_index = (uint8_t)(frame_index + 1u);
		frame_index = frame_index & 3u;
		OBJECT(current_object)->whirlwind.frame_index = frame_index;
		OBJECT(current_object)->sprite_frame = whirlwind_sprites[frame_index];
		queue_sprite(aim_speed);
		add_to_touchable_list(frame_index);
	}
}

void whirlwind_vanish(void)
{
	uint32_t block_offset = 0, cursor = 0, ignored = 0, slope_height = 0;
	uint16_t odd_frame = 0;

	move_object_projected(&ignored, &block_offset, &slope_height, &cursor);
	OBJECT(current_object)->whirlwind.timer++;
	odd_frame = OBJECT(current_object)->whirlwind.timer & 1u;
	OBJECT(current_object)->whirlwind.timer = odd_frame;
	if (odd_frame != 0u)
	{
		queue_sprite(0u);
		add_to_touchable_list(ignored);
		return;
	}
	OBJECT(current_object)->sprite_frame = whirlwind_vanish_sprites[(uint8_t)OBJECT(current_object)->whirlwind.frame_index];
	OBJECT(current_object)->whirlwind.frame_index++;
	if (OBJECT(current_object)->whirlwind.frame_index <= 7)
	{
		queue_sprite(0u);
		add_to_touchable_list(ignored);
		return;
	}
	delete_object_and_children(&ignored, &block_offset, &slope_height);
}
