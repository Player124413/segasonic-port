/**
 *    Name:        objects/flame_machine.c
 *    Description: object types 7, 8 and 9: the machine at the end of stage 4 with its arms, cannons, flames and spiked discs, its scroll stop, its wreck
 *    Version:     0.1.0
 *    Created:     12-10-2026
 *    Modified:    15-10-2026
 *    Author:      Sebastian Toyos
 *
 *    SegaSonic The Hedgehog (arcade, Sega System 32): the game's V60 program decompiled into C by
 *    tools/decompile.py, with the names of symbols.txt.
 */
#include "game.h"

void flame_machine_init(void)
{
	uint16_t back_wall = 0;
	uint32_t block = 0, free_count = 0, object = 0, piece_count = 0, spawned = 0;

	flame_machine_flags = 0u;
	OBJECT(current_object)->sprite_frame = 0x6258u;
	OBJECT(current_object)->sprite_palette = 0x91F0u;
	OBJECT(current_object)->sprite_flags = 0u;
	OBJECT(current_object)->flags |= 2u;
	OBJECT(current_object)->hitbox_size_x = 0x116u;
	OBJECT(current_object)->hitbox_size_y = 0x70u;
	OBJECT(current_object)->hitbox_offset_x = -0x7F;
	OBJECT(current_object)->hitbox_offset_y = 4;
	OBJECT(current_object)->hitbox_height = 0x100u;
	OBJECT(current_object)->kind = 0xCu;
	OBJECT(current_object)->health = 0x28;
	OBJECT(current_object)->flame_machine_init.time_left = 0xE10u;
	OBJECT(current_object)->handler = flame_machine_update_ADDRESS;
	current_object = LINKED(current_object, OBJECT(current_object)->child);
	OBJECT(current_object)->sprite_frame = 0x6260u;
	OBJECT(current_object)->sprite_flags = 8u;
	current_object = LINKED(current_object, OBJECT(current_object)->child);
	OBJECT(current_object)->sprite_frame = 0x6B20u;
	OBJECT(current_object)->sprite_palette = 0x9240u;
	current_object = LINKED(current_object, OBJECT(current_object)->child);
	OBJECT(current_object)->sprite_frame = 0x6B18u;
	OBJECT(current_object)->sprite_palette = 0x9230u;
	current_object = LINKED(current_object, OBJECT(current_object)->child);
	OBJECT(current_object)->sprite_frame = 0x6970u;
	OBJECT(current_object)->sprite_palette = 0x9FFEu;
	r14 = current_object;
	spawn_count = 3u;
	spawn_object_with_pieces(0x00FC8Fu, &spawned, &piece_count, &free_count, &object, &block);
	OBJECT(r14)->child = object;
	copy_position_to(object);
	OBJECT(object)->sprite_frame = 0x6268u;
	OBJECT(object)->sprite_palette = 0x9200u;
	OBJECT(object)->sprite_flags = 0u;
	OBJECT(object)->handler = flame_machine_back_wall_update_ADDRESS;
	back_wall = (uint16_t)object;
	object = LINKED(object, OBJECT(object)->child);
	OBJECT(object)->sprite_palette = 0x91E0u;
	OBJECT(object)->sprite_flags = 5u;
	object = LINKED(object, OBJECT(object)->child);
	OBJECT(object)->sprite_palette = 0x91E0u;
	OBJECT(object)->sprite_flags = 5u;
	r14 = object;
	spawn_count = 2u;
	spawn_object_with_pieces(0x00FCE4u, &spawned, &piece_count, &free_count, &object, &block);
	copy_position_to(object);
	OBJECT(r14)->child = object;
	OBJECT(object)->parent = back_wall;
	OBJECT(object)->spawn_arg_1 = 0xFFF5u;
	OBJECT(object)->spawn_arg_2 = 0x36u;
	OBJECT(object)->sprite_palette = 0x9220u;
	OBJECT(object)->direction = 1u;
	OBJECT(object)->handler = flame_machine_arm_update_ADDRESS;
	object = LINKED(object, OBJECT(object)->child);
	OBJECT(object)->sprite_palette = 0x9220u;
	r14 = object;
	spawn_count = 2u;
	spawn_object_with_pieces(0x00FD2Eu, &spawned, &piece_count, &free_count, &object, &block);
	copy_position_to(object);
	OBJECT(r14)->child = object;
	OBJECT(object)->parent = back_wall;
	OBJECT(object)->spawn_arg_1 = 0x40u;
	OBJECT(object)->spawn_arg_2 = 0x11u;
	OBJECT(object)->sprite_palette = 0x9220u;
	OBJECT(object)->direction = 0x1FFu;
	OBJECT(object)->handler = flame_machine_arm_update_ADDRESS;
	object = LINKED(object, OBJECT(object)->child);
	OBJECT(object)->sprite_palette = 0x9220u;
	r14 = object;
	spawn_count = 2u;
	spawn_objects(0x00FD7Au, &spawned, &object, &block);
	OBJECT(r14)->child = object;
	copy_position_to(object);
	OBJECT(object)->projected_x += 0x29u;
	OBJECT(object)->projected_y_int -= 0x7Eu;
	OBJECT(object)->sprite_frame = 0x61C8u;
	OBJECT(object)->sprite_palette = 0x91D0u;
	OBJECT(object)->sprite_flags = 8u;
	OBJECT(object)->spawn_arg_1 = 0u;
	OBJECT(object)->flags |= 2u;
	OBJECT(object)->hitbox_size_x = 0x11Eu;
	OBJECT(object)->hitbox_size_y = 0x30u;
	OBJECT(object)->hitbox_offset_x = -0x33;
	OBJECT(object)->hitbox_offset_y = 0x68;
	OBJECT(object)->hitbox_height = 0x50u;
	OBJECT(object)->kind = 0xCu;
	OBJECT(object)->handler = flame_machine_cannon_update_ADDRESS;
	object = LINKED(object, OBJECT(object)->child);
	copy_position_to(object);
	OBJECT(object)->projected_x -= 0x63u;
	OBJECT(object)->projected_y_int += 0x15u;
	OBJECT(object)->sprite_frame = 0x61D0u;
	OBJECT(object)->sprite_palette = 0x91D0u;
	OBJECT(object)->spawn_arg_1 = 1u;
	OBJECT(object)->sprite_flags = 0u;
	OBJECT(object)->flags |= 2u;
	OBJECT(object)->hitbox_size_x = 0x11Eu;
	OBJECT(object)->hitbox_size_y = 0x30u;
	OBJECT(object)->hitbox_offset_x = -0x33;
	OBJECT(object)->hitbox_offset_y = 0x68;
	OBJECT(object)->hitbox_height = 0x50u;
	OBJECT(object)->kind = 0xCu;
	OBJECT(object)->handler = flame_machine_cannon_update_ADDRESS;
	flame_machine_flags |= 2u;
}

void flame_machine_update(void)
{
	uint8_t sound_id = 0;
	uint32_t r0 = 0, r2 = 0, r11 = 0, depth = 0, ignored = 0, object = 0, object_2 = 0, random_scale = 0, spawned = 0;
	uint8_t flash_steps_left = 0, odd_frame = 0;
	uint16_t flash_count = 0, time_left = 0, explosion_phase = 0;

	if (active_player_count == 0u)
	{
	flame_machine_draw:
		unproject_object(&ignored, &r2, &random_scale, &r11);
		depth = queue_sprite(0u);
		add_to_touchable_list(ignored);
		if ((uint8_t)OBJECT(current_object)->flame_machine_update.previous_health != (uint8_t)OBJECT(current_object)->health)
		{
			OBJECT(current_object)->flame_machine_update.previous_health = OBJECT(current_object)->health;
			flame_machine_flags |= 0x1000u;
			flame_machine_flash_step = 4;
			sound_id = 0x21u;
			play_sound(sound_id, &depth, &r2);
		}
		if ((flame_machine_flags & 0x1000u) != 0u)
		{
			flash_count = OBJECT(current_object)->flame_machine_update.flash_frame_count + 1u;
			OBJECT(current_object)->flame_machine_update.flash_frame_count = flash_count;
			if (flash_count != 0u)
			{
				flash_steps_left = flame_machine_flash_step;
				flame_machine_flash_step = flash_steps_left - 1u;
				if (flash_steps_left == 0u)
					flame_machine_flags &= ~0x1000u;
				else
				{
					OBJECT(current_object)->sprite_palette = flame_machine_hit_palettes[(uint8_t)flame_machine_flash_step];
					sound_id = 0x1Eu;
					play_sound(sound_id, &depth, &r2);
				}
			}
		}
		current_object = LINKED(current_object, OBJECT(current_object)->child);
		OBJECT(current_object)->screen_x = OBJECT(object_cursor)->screen_x;
		OBJECT(current_object)->screen_y = OBJECT(object_cursor)->screen_y;
		OBJECT(current_object)->screen_x -= 2u;
		OBJECT(current_object)->screen_y -= 0x40u;
		OBJECT(current_object)->sprite_palette = OBJECT(object_cursor)->sprite_palette;
		queue_sprite(depth);
		current_object = LINKED(current_object, OBJECT(current_object)->child);
		AT8(OBJECT(current_object)->direction, 1)++;
		odd_frame = AT8(OBJECT(current_object)->direction, 1) & 1u;
		AT8(OBJECT(current_object)->direction, 1) = odd_frame;
		if (odd_frame != 0u)
		{
			AS8(OBJECT(current_object)->direction)++;
			if ((int8_t)AS8(OBJECT(current_object)->direction) > 6)
				AS8(OBJECT(current_object)->direction) = 0u;
			OBJECT(current_object)->sprite_frame = flame_machine_capsule_sprites[AS8(OBJECT(current_object)->direction)];
		}
		OBJECT(current_object)->screen_x = OBJECT(object_cursor)->screen_x;
		depth = (uint16_t)OBJECT(object_cursor)->screen_y;
		OBJECT(current_object)->screen_y = depth;
		OBJECT(current_object)->screen_x += 0x2Du;
		OBJECT(current_object)->screen_y += 0x1Fu;
		depth += 2u;
		queue_sprite_at_depth(&ignored, &depth);
		current_object = LINKED(current_object, OBJECT(current_object)->child);
		OBJECT(current_object)->screen_x = OBJECT(object_cursor)->screen_x;
		depth = (uint16_t)OBJECT(object_cursor)->screen_y;
		OBJECT(current_object)->screen_y = depth;
		OBJECT(current_object)->screen_x += 0x2Du;
		OBJECT(current_object)->screen_y += 0x2Bu;
		depth++;
		queue_sprite_at_depth(&ignored, &depth);
		current_object = LINKED(current_object, OBJECT(current_object)->child);
		r0 = place_sprite_at_cursor_floor();
		queue_sprite_in_bucket(r0);
		if (OBJECT(object_cursor)->health > 0)
			return;
	}
	else
	{
		time_left = OBJECT(object_cursor)->flame_machine_update.time_left;
		OBJECT(object_cursor)->flame_machine_update.time_left = time_left - 1u;
		if (time_left != 0u)
			goto flame_machine_draw;
	}
	goal_unlocked = 0xFFu;
	flame_machine_flags |= 0x100000u;
	current_object = object_cursor;
	OBJECT(current_object)->health = 0;
	OBJECT(current_object)->flame_machine_update.destroyed_frames++;
	if (OBJECT(current_object)->flame_machine_update.destroyed_frames != 0x130u)
	{
		explosion_phase = OBJECT(current_object)->flame_machine_update.destroyed_frames & 7u;
		spawned = explosion_phase;
		if (explosion_phase != 0u)
			return;
		spawn_count = 1u;
		spawn_object_with_pieces(0x00FFC1u, &spawned, &depth, &r2, &random_scale, &r11);
		if ((uint8_t)spawned == 0u)
			return;
		object_2 = random_scale;
		copy_position_to(random_scale);
		next_random(&random_scale, &ignored, &ignored);
		random_scale = (random_scale & ~0xFFFFu) | (random_scale & 0x3FFu);
		OBJECT(object_2)->sprite_scale += random_scale;
		random_scale = (random_scale & ~0xFFFFu) | (random_scale & 0x7Fu);
		OBJECT(object_2)->projected_y_int += 0x13u;
		OBJECT(object_2)->projected_y_int += random_scale;
		next_random(&random_scale, &ignored, &ignored);
		OBJECT(object_2)->projected_x += random_scale & 0x7Fu;
		OBJECT(object_2)->projected_x -= 0x53u;
		OBJECT(object_2)->handler = flame_machine_explosion_start_ADDRESS;
		return;
	}
	OBJECT(current_object)->handler = flame_machine_remove_ADDRESS;
	spawn_count = 3u;
	spawn_object_with_pieces(0x010820u, &r0, &depth, &r2, &random_scale, &r11);
	OBJECT(random_scale)->projected_x = 0x1B2F;
	OBJECT(random_scale)->projected_y_int = 0x14F;
	OBJECT(random_scale)->z = 0;
	OBJECT(random_scale)->sprite_frame = 0x8360u;
	OBJECT(random_scale)->sprite_palette = 0x87D0u;
	OBJECT(random_scale)->handler = flame_machine_wreck_draw_ADDRESS;
	object = LINKED(random_scale, OBJECT(random_scale)->child);
	OBJECT(object)->sprite_frame = 0x8370u;
	OBJECT(object)->sprite_palette = 0x87E0u;
	object = LINKED(object, OBJECT(object)->child);
	OBJECT(object)->sprite_frame = 0x8368u;
	OBJECT(object)->sprite_palette = 0x87E0u;
	current_object = object_cursor;
}

void flame_machine_update_spare_return(void)
{
}

void flame_machine_remove(void)
{
	uint32_t block_offset = 0, ignored = 0, slope_height = 0;

	electric_fence_state = 0xFFu;
	delete_object_and_children(&ignored, &block_offset, &slope_height);
}

void flame_machine_explosion_start(void)
{
	uint16_t count = 0;

	OBJECT(current_object)->speed = 0x20;
	OBJECT(current_object)->direction = 0x80u;
	count = 8u;
	spawn_debris(0u, count, 0u);
	explode_object();
}

void flame_machine_back_wall_update(void)
{
	uint16_t rod_sprite = 0;
	uint32_t cursor = 0, depth = 0, frame_index = 0, ignored = 0, object = 0, offset_record = 0, wall_frame = 0;
	uint16_t next_phase = 0;

	if ((flame_machine_flags & 0x100000u) == 0u)
	{
		move_object_projected(&ignored, &ignored, &ignored, &cursor);
		if ((flame_machine_flags & 0x10u) == 0u && (flame_machine_flags & 0x20u) == 0u)
		{
			if (OBJECT(current_object)->flame_machine_back_wall.pause_timer > 0)
				OBJECT(current_object)->flame_machine_back_wall.pause_timer--;
			else
			{
				OBJECT(current_object)->flame_machine_back_wall.frame_phase++;
				next_phase = OBJECT(current_object)->flame_machine_back_wall.frame_phase & 3u;
				OBJECT(current_object)->flame_machine_back_wall.frame_phase = next_phase;
				if (next_phase == 0u)
				{
					OBJECT(current_object)->flame_machine_back_wall.frame_phase = 1u;
					OBJECT(current_object)->flame_machine_back_wall.frame_index++;
					frame_index = (uint8_t)OBJECT(current_object)->flame_machine_back_wall.frame_index;
					if ((int8_t)frame_index > 9)
					{
						frame_index = 0u;
						OBJECT(current_object)->flame_machine_back_wall.frame_index = 0;
						OBJECT(current_object)->flame_machine_back_wall.pause_timer = 0x1E;
						flame_machine_flags |= 4u;
						flame_machine_flags |= 8u;
						flame_machine_flags |= 0x400u;
						flame_machine_flags |= 0x800u;
						flame_machine_flags &= ~0x200u;
					}
					wall_frame = flame_machine_back_wall_frames[frame_index];
					OBJECT(current_object)->flame_machine_back_wall.frame = wall_frame;
					rod_sprite = flame_machine_rod_sprites[wall_frame];
					current_object = LINKED(current_object, OBJECT(current_object)->child);
					OBJECT(current_object)->sprite_frame = rod_sprite;
					current_object = LINKED(current_object, OBJECT(current_object)->child);
					OBJECT(current_object)->sprite_frame = rod_sprite;
					offset_record = flame_machine_back_wall_offsets_ADDRESS + wall_frame * 4u;
					current_object = LINKED(current_object, OBJECT(object_cursor)->parent);
					OBJECT(object_cursor)->projected_x = OBJECT(current_object)->projected_x;
					OBJECT(object_cursor)->projected_y = OBJECT(current_object)->projected_y;
					OBJECT(object_cursor)->projected_x += ROM16(offset_record);
					OBJECT(object_cursor)->projected_y_int += ROM16(offset_record + 2u);
					if (wall_frame == 5u)
					{
						flame_machine_flags |= 0x10u;
						flame_machine_flags |= 0x20u;
					}
				}
			}
		}
	}
	current_object = object_cursor;
	queue_sprite(wall_frame);
	if ((flame_machine_flags & 0x100000u) == 0u)
	{
		if (OBJECT(current_object)->flame_machine_back_wall.frame == 5u)
			return;
		current_object = LINKED(current_object, OBJECT(object_cursor)->child);
		object = LINKED(WORK_RAM, OBJECT(object_cursor)->parent);
		OBJECT(current_object)->screen_x = OBJECT(object)->screen_x;
		OBJECT(current_object)->screen_y = OBJECT(object)->screen_y;
		OBJECT(current_object)->screen_x += 0xEu;
		OBJECT(current_object)->screen_y += 0x17u;
		current_object = LINKED(current_object, OBJECT(current_object)->child);
		OBJECT(current_object)->screen_x = OBJECT(object)->screen_x;
		OBJECT(current_object)->screen_y = OBJECT(object)->screen_y;
		OBJECT(current_object)->screen_x += 0x36u;
		OBJECT(current_object)->screen_y += 3u;
	}
	current_object = LINKED(current_object, OBJECT(object_cursor)->child);
	depth = (uint16_t)(OBJECT(object_cursor)->screen_y + 1u);
	queue_sprite_at_depth(&ignored, &depth);
	current_object = LINKED(current_object, OBJECT(current_object)->child);
	depth = (uint16_t)(OBJECT(object_cursor)->screen_y + 1u);
	queue_sprite_at_depth(&ignored, &depth);
}

void flame_machine_arm_update(void)
{
	uint32_t r2 = 0, r3 = 0, r11 = 0, frame_number = 0, ignored = 0;
	uint8_t odd_sway_frame = 0;
	uint16_t odd_frame = 0;

	if ((flame_machine_flags & 0x100000u) != 0u)
		return;
	unproject_object(&ignored, &r2, &r3, &r11);
	if ((flame_machine_flags & shl32(1u, (int)(OBJECT(current_object)->flame_machine_arm.arm_index + 0xAu))) != 0u)
		flame_machine_arm_take_disc(r2, r3, r11);
	if ((flame_machine_flags & shl32(1u, (int)(OBJECT(current_object)->flame_machine_arm.arm_index + 4u))) == 0u)
	{
		flame_machine_arm_draw();
		return;
	}
	if ((flame_machine_flags & shl32(1u, (int)(OBJECT(current_object)->flame_machine_arm.arm_index + 6u))) == 0u)
	{
		OBJECT(current_object)->flame_machine_arm.frame_toggle++;
		odd_frame = OBJECT(current_object)->flame_machine_arm.frame_toggle & 1u;
		OBJECT(current_object)->flame_machine_arm.frame_toggle = odd_frame;
		if (odd_frame != 0u)
		{
			flame_machine_arm_draw();
			return;
		}
		OBJECT(current_object)->flame_machine_arm.frame_index++;
		frame_number = flame_machine_arm_frames[OBJECT(current_object)->flame_machine_arm.frame_index];
		if (frame_number != 0xCu)
		{
			if (frame_number != 0xDu)
			{
				OBJECT(current_object)->sprite_frame = flame_machine_arm_sprites[frame_number];
				current_object = LINKED(current_object, OBJECT(current_object)->child);
				OBJECT(current_object)->sprite_frame = flame_machine_arm_part_sprites[frame_number];
				current_object = object_cursor;
				flame_machine_arm_draw();
				return;
			}
			OBJECT(current_object)->flame_machine_arm.frame_index = 0u;
			flame_machine_flags &= ~shl32(1u, (int)(OBJECT(current_object)->flame_machine_arm.arm_index + 4u));
			flame_machine_arm_draw();
			return;
		}
		flame_machine_flags |= shl32(1u, (int)(OBJECT(current_object)->flame_machine_arm.arm_index + 6u));
		OBJECT(current_object)->sprite_frame = flame_machine_arm_sprites[frame_number];
		current_object = LINKED(current_object, OBJECT(current_object)->child);
		OBJECT(current_object)->sprite_frame = flame_machine_arm_part_sprites[frame_number];
		current_object = object_cursor;
		flame_machine_arm_draw();
		return;
	}
	OBJECT(current_object)->flame_machine_arm.sway_toggle++;
	odd_sway_frame = OBJECT(current_object)->flame_machine_arm.sway_toggle & 1u;
	OBJECT(current_object)->flame_machine_arm.sway_toggle = odd_sway_frame;
	if (odd_sway_frame != 0u)
	{
		flame_machine_arm_draw();
		return;
	}
	OBJECT(current_object)->flame_machine_arm.sway_count++;
	if (OBJECT(current_object)->flame_machine_arm.sway_count > 0x26)
	{
		flame_machine_flags |= 0x200u;
		OBJECT(current_object)->var_84 = 0x1Eu;
		OBJECT(current_object)->handler = flame_machine_arm_rest_ADDRESS;
		flame_machine_arm_rest();
		return;
	}
	OBJECT(current_object)->flame_machine_arm.sway_step++;
	OBJECT(current_object)->flame_machine_arm.sway_step = OBJECT(current_object)->flame_machine_arm.sway_step % 0x29u;
	OBJECT(current_object)->flame_machine_arm.sway_count++;
	flame_machine_arm_draw();
}

void flame_machine_arm_rest(void)
{
	OBJECT(current_object)->var_84--;
	if ((int16_t)OBJECT(current_object)->var_84 > 0)
	{
		flame_machine_arm_draw();
		return;
	}
	flame_machine_flags &= ~shl32(1u, (int)(OBJECT(current_object)->flame_machine_arm.arm_index + 6u));
	OBJECT(current_object)->flame_machine_arm.frame_toggle = 1u;
	OBJECT(current_object)->handler = flame_machine_arm_update_ADDRESS;
	flame_machine_arm_draw();
}

void flame_machine_arm_draw(void)
{
	uint8_t bucket = 0;
	uint16_t side = 0, sway_dy = 0;
	uint32_t depth = 0, ignored = 0, object = 0, sway_offset = 0;

	current_object = LINKED(current_object, OBJECT(object_cursor)->parent);
	OBJECT(object_cursor)->projected_x = OBJECT(current_object)->projected_x;
	OBJECT(object_cursor)->projected_y = OBJECT(current_object)->projected_y;
	OBJECT(object_cursor)->projected_x += OBJECT(object_cursor)->spawn_arg_1;
	OBJECT(object_cursor)->projected_y_int += OBJECT(object_cursor)->spawn_arg_2;
	current_object = object_cursor;
	sway_offset = flame_machine_arm_sway_offsets_ADDRESS + OBJECT(current_object)->flame_machine_arm.sway_step * 2u;
	side = (uint16_t)OBJECT(current_object)->flame_machine_arm.side;
	sway_dy = (uint16_t)((uint32_t)(int8_t)ROM8(sway_offset + 1u) * side);
	OBJECT(current_object)->projected_x += (uint32_t)(int8_t)ROM8(sway_offset) * side;
	OBJECT(current_object)->projected_y_int += sway_dy;
	object = LINKED(WORK_RAM, OBJECT(current_object)->parent);
	object = LINKED(object, OBJECT(object)->child);
	object = LINKED(object, OBJECT(object)->child);
	object = LINKED(object, OBJECT(object)->child);
	object = LINKED(object, OBJECT(object)->child);
	depth = (uint16_t)(OBJECT(LINKED(object, OBJECT(object)->child))->screen_y - 1u);
	r14 = depth;
	queue_sprite_at_depth(&ignored, &depth);
	current_object = LINKED(current_object, OBJECT(current_object)->child);
	OBJECT(current_object)->screen_x = OBJECT(object_cursor)->screen_x;
	OBJECT(current_object)->screen_y = OBJECT(object_cursor)->screen_y;
	bucket = (uint8_t)((uint32_t)OBJECT(object_cursor)->flame_machine_arm.arm_index << 2);
	queue_sprite_in_bucket(bucket);
}

void flame_machine_cannon_update(void)
{
	uint32_t r0 = 0, r2 = 0, r11 = 0, ignored = 0, object = 0, offset_record = 0, spawn_arg_1 = 0;

	unproject_object(&ignored, &r2, &object, &r11);
	queue_sprite(0u);
	OBJECT(current_object)->health = 0x7F;
	add_to_touchable_list(ignored);
	if (flame_machine_flash_step >= 0)
		OBJECT(current_object)->sprite_palette = flame_machine_cannon_hit_palettes[(uint8_t)flame_machine_flash_step];
	r0 = OBJECT(current_object)->spawn_arg_1 + 2u;
	if ((flame_machine_flags & shl32(1u, (int)(r0))) == 0u)
		return;
	spawn_count = 2u;
	spawn_object_with_pieces(0x010440u, &r0, &offset_record, &r2, &object, &r11);
	if ((uint8_t)r0 != 0u)
	{
		copy_position_to(object);
		spawn_arg_1 = OBJECT(current_object)->spawn_arg_1;
		offset_record = flame_machine_flame_offsets_ADDRESS + spawn_arg_1 * 4u;
		OBJECT(object)->direction = flame_machine_flame_directions[spawn_arg_1];
		OBJECT(object)->projected_x += ROM16(offset_record);
		OBJECT(object)->projected_y_int += ROM16(offset_record + 2u);
		OBJECT(object)->sprite_frame = 0x6910u;
		OBJECT(object)->sprite_palette = 0x7B0u;
		OBJECT(object)->flags |= 2u;
		OBJECT(object)->hitbox_size_x = 0x30u;
		OBJECT(object)->hitbox_size_y = 0xF8u;
		OBJECT(object)->hitbox_offset_x = -8;
		OBJECT(object)->hitbox_offset_y = -5;
		OBJECT(object)->hitbox_height = 0x28u;
		OBJECT(object)->kind = 0x17u;
		OBJECT(object)->sprite_flags = 0xAu;
		OBJECT(object)->handler = flame_machine_flame_burn_ADDRESS;
		object = LINKED(object, OBJECT(object)->child);
		OBJECT(object)->sprite_palette = 0x9FFEu;
		OBJECT(object)->sprite_flags = 0xAu;
		flame_machine_flags &= ~shl32(1u, (int)(spawn_arg_1 + 2u));
	}
}

void flame_machine_flame_burn(void)
{
	uint32_t block_offset = 0, bucket = 0, cursor = 0, depth = 0, flame_frame = 0, ignored = 0, slope_height = 0;
	uint16_t odd_frame = 0;

	OBJECT(object_cursor)->z_int -= 0x20u;
	unproject_object(&ignored, &block_offset, &slope_height, &cursor);
	OBJECT(current_object)->flame_machine_flame.frame_toggle++;
	odd_frame = OBJECT(current_object)->flame_machine_flame.frame_toggle & 1u;
	OBJECT(current_object)->flame_machine_flame.frame_toggle = odd_frame;
	if (odd_frame != 0u)
	{
		OBJECT(current_object)->flame_machine_flame.frame_index++;
		flame_frame = flame_machine_flame_frames[OBJECT(current_object)->flame_machine_flame.frame_index];
		OBJECT(current_object)->spawn_arg_2 = flame_frame;
		OBJECT(current_object)->sprite_frame = flame_machine_flame_sprites[flame_frame];
		if (flame_frame == 0xBu)
		{
			delete_object_and_children(&ignored, &block_offset, &slope_height);
			return;
		}
		if ((int16_t)OBJECT(current_object)->spawn_arg_2 >= 2)
		{
			OBJECT(current_object)->health = 0x7F;
			add_to_touchable_list(ignored);
		}
	}
	depth = (uint16_t)(OBJECT(current_object)->screen_y + 0x60u);
	queue_sprite_at_depth(&ignored, &depth);
	if ((int16_t)OBJECT(current_object)->spawn_arg_2 >= 2 && (int16_t)OBJECT(current_object)->spawn_arg_2 <= 4)
	{
		current_object = LINKED(current_object, OBJECT(current_object)->child);
		OBJECT(current_object)->sprite_frame = flame_machine_flame_shadow_sprites[OBJECT(object_cursor)->spawn_arg_2 - 2u];
		bucket = place_sprite_at_cursor_floor();
		queue_sprite_in_bucket(bucket);
	}
	OBJECT(object_cursor)->z_int += 0x20u;
	if ((flame_machine_flags & 0x100000u) == 0u)
		return;
	current_object = object_cursor;
	explode_object();
}

void flame_machine_arm_take_disc(uint32_t free_count, uint32_t disc, uint32_t block)
{
	uint32_t disc_flag_bit = 0, object = 0, piece_count = 0, spawned = 0;

	spawn_count = 2u;
	spawn_object_with_pieces(0x01061Fu, &spawned, &piece_count, &free_count, &disc, &block);
	copy_position_to(disc);
	OBJECT(disc)->direction = 0x200u;
	OBJECT(disc)->sprite_scale = 0xC0u;
	OBJECT(disc)->sprite_frame = 0x6278u;
	OBJECT(disc)->sprite_palette = 0x1210u;
	OBJECT(disc)->sprite_flags = 0u;
	OBJECT(disc)->flags |= 2u;
	OBJECT(disc)->hitbox_size_x = 0x18u;
	OBJECT(disc)->hitbox_size_y = 0x3Cu;
	OBJECT(disc)->hitbox_offset_x = -0x31;
	OBJECT(disc)->hitbox_offset_y = 3;
	OBJECT(disc)->hitbox_height = 0x28u;
	OBJECT(disc)->kind = 0x24u;
	OBJECT(disc)->handler = spiked_disc_hold_ADDRESS;
	disc_flag_bit = OBJECT(current_object)->flame_machine_arm.arm_index + 0xAu;
	OBJECT(disc)->spawn_arg_1 = OBJECT(current_object)->flame_machine_arm.arm_index;
	flame_machine_flags &= ~shl32(1u, (int)(disc_flag_bit));
	object = LINKED(disc, OBJECT(disc)->child);
	OBJECT(object)->sprite_frame = 0x6988u;
	OBJECT(object)->sprite_palette = 0x9FFEu;
}

void spiked_disc_hold(void)
{
	uint32_t arm_frame_index = 0, offset_record = 0;

	if ((flame_machine_flags & 0x200u) != 0u)
		OBJECT(current_object)->handler = spiked_disc_drop_ADDRESS;
	current_object = LINKED(current_object, OBJECT(object_cursor)->parent);
	copy_position_to_cursor();
	arm_frame_index = OBJECT(current_object)->var_80;
	OBJECT(object_cursor)->spiked_disc.arm_frame = arm_frame_index;
	offset_record = spiked_disc_hold_offsets_ADDRESS + arm_frame_index * 4u;
	OBJECT(object_cursor)->projected_x += ROM16(offset_record);
	OBJECT(object_cursor)->z_int += ROM16(offset_record + 2u);
	current_object = object_cursor;
	spiked_disc_draw_held();
}

void spiked_disc_drop(void)
{
	uint32_t r1 = 0, height = 0, z = 0;

	fall_to_floor(&height, &r1, &z);
	if (height != 0u)
	{
		spiked_disc_draw_held();
		return;
	}
	OBJECT(current_object)->handler = spiked_disc_roll_ADDRESS;
	spiked_disc_draw_held();
}

void spiked_disc_draw_held(void)
{
	uint8_t bucket = 0;
	uint32_t cursor = 0, ignored = 0;

	move_object_projected(&ignored, &ignored, &ignored, &cursor);
	bucket = (uint8_t)((OBJECT(current_object)->spawn_arg_1 + 1u) * 2u);
	queue_sprite_in_bucket(bucket);
	if ((int16_t)AS16(OBJECT(current_object)->spiked_disc.arm_frame) >= 8)
	{
		spiked_disc_draw_shadow();
		return;
	}
	spiked_disc_spin();
}

void spiked_disc_roll(void)
{
	uint32_t r1 = 0, r2 = 0, cursor = 0, deleted = 0, depth = 0, direction = 0, ignored = 0, slope_height = 0, speed = 0, speed_x = 0, speed_y = 0;

	move_object_projected(&ignored, &r2, &slope_height, &cursor);
	fall_to_floor(&ignored, &r1, &r2);
	delete_if_off_screen(&deleted, &r2, &slope_height);
	if ((uint8_t)deleted == 1u)
		return;
	direction = 0x154u;
	speed = 0x300u;
	direction_to_speed(speed, &direction, &speed_x, &speed_y);
	OBJECT(current_object)->speed_x = speed_x;
	OBJECT(current_object)->speed_y = speed_y;
	depth = (uint16_t)(OBJECT(current_object)->screen_y + 0x18u);
	queue_sprite_at_depth(&ignored, &depth);
	spiked_disc_draw_shadow();
}

void spiked_disc_draw_shadow(void)
{
	uint32_t bucket = 0;

	current_object = LINKED(current_object, OBJECT(current_object)->child);
	bucket = place_sprite_at_cursor_floor();
	OBJECT(current_object)->screen_y += 0x28u;
	queue_sprite_in_bucket(bucket);
	current_object = object_cursor;
	spiked_disc_spin();
}

void spiked_disc_spin(void)
{
	uint16_t odd_frame = 0;

	if ((flame_machine_flags & 0x100000u) != 0u)
	{
		explode_object();
		return;
	}
	OBJECT(current_object)->spiked_disc.spin_toggle++;
	odd_frame = OBJECT(current_object)->spiked_disc.spin_toggle & 1u;
	OBJECT(current_object)->spiked_disc.spin_toggle = odd_frame;
	if (odd_frame == 0u)
	{
		OBJECT(current_object)->spiked_disc.spin_toggle = 1u;
		OBJECT(current_object)->spiked_disc.spin_frame++;
		OBJECT(current_object)->spiked_disc.spin_frame &= 3u;
		OBJECT(current_object)->sprite_frame = spiked_disc_sprites[OBJECT(current_object)->spiked_disc.spin_frame];
		OBJECT(current_object)->health = 0x7F;
		add_to_touchable_list(0u);
		add_to_rider_list();
	}
}

void flame_machine_scroll_stop_wait(void)
{
	if ((int16_t)scroll_position < (int16_t)OBJECT(current_object)->spawn_arg_1)
		return;
	scroll_mode = 1u;
	OBJECT(current_object)->handler = flame_machine_scroll_stop_hold_ADDRESS;
	flame_machine_scroll_stop_hold();
}

void flame_machine_scroll_stop_hold(void)
{
	if ((flame_machine_flags & 0x100000u) != 0u)
		scroll_mode = 7u;
}

void object_type_9_idle(void)
{
}

void flame_machine_wreck_draw(void)
{
	uint8_t bucket = 0;
	uint16_t offset_x = 0, offset_y = 0;
	uint32_t block_offset = 0, cursor = 0, ignored = 0, slope_height = 0;

	if (map_screen_active != 0u)
	{
		delete_object_and_children(&ignored, &block_offset, &slope_height);
		return;
	}
	unproject_object(&ignored, &ignored, &ignored, &cursor);
	bucket = 0xFEu;
	queue_sprite_in_bucket(bucket);
	current_object = LINKED(current_object, OBJECT(current_object)->child);
	offset_x = 0x6Cu;
	offset_y = 0xFF9Au;
	place_sprite_near_cursor(offset_x, offset_y);
	bucket = 0xFDu;
	queue_sprite_in_bucket(bucket);
	current_object = LINKED(current_object, OBJECT(current_object)->child);
	offset_x = 0xFFDFu;
	offset_y = 0xFFD9u;
	place_sprite_near_cursor(offset_x, offset_y);
	bucket = 0xFCu;
	queue_sprite_in_bucket(bucket);
}
