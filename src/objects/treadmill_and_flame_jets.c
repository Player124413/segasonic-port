/**
 *    Name:        objects/treadmill_and_flame_jets.c
 *    Description: object types 10 and 11 (stage 8): the treadmill that raises a gate and the flame jets
 *    Version:     0.1.0
 *    Created:     12-10-2026
 *    Modified:    15-10-2026
 *    Author:      Sebastian Toyos
 *
 *    SegaSonic The Hedgehog (arcade, Sega System 32): the game's V60 program decompiled into C by
 *    tools/decompile.py, with the names of symbols.txt.
 */
#include "game.h"

void treadmill_init(void)
{
	uint16_t gate = 0;
	uint32_t block = 0, free_count = 0, object = 0, piece_count = 0, spawned = 0;

	OBJECT(current_object)->sprite_frame = 0x72B0u;
	OBJECT(current_object)->sprite_palette = 0x1150u;
	OBJECT(current_object)->sprite_palette |= 0x4000u;
	OBJECT(current_object)->sprite_flags = 0xAu;
	OBJECT(current_object)->flags |= 2u;
	OBJECT(current_object)->hitbox_size_x = 0x61u;
	OBJECT(current_object)->hitbox_size_y = 0xE9u;
	OBJECT(current_object)->hitbox_offset_x = -0x3D;
	OBJECT(current_object)->hitbox_offset_y = 0x4C;
	OBJECT(current_object)->hitbox_size_x = 0x10u;
	OBJECT(current_object)->hitbox_size_y = 0xE9u;
	OBJECT(current_object)->hitbox_offset_x = -0x15;
	OBJECT(current_object)->hitbox_offset_y = 0x4C;
	OBJECT(current_object)->hitbox_height = 0x20u;
	OBJECT(current_object)->kind = 0x16u;
	OBJECT(current_object)->handler = treadmill_update_ADDRESS;
	spawn_count = 2u;
	spawn_object_with_pieces(0x010946u, &spawned, &piece_count, &free_count, &object, &block);
	OBJECT(object_cursor)->child = object;
	copy_position_to(object);
	OBJECT(object)->projected_x += 0x27u;
	OBJECT(object)->projected_y_int += 0x18u;
	OBJECT(object)->sprite_frame = 0x72F0u;
	OBJECT(object)->sprite_palette = 0x1160u;
	OBJECT(object)->sprite_palette |= 0x4000u;
	OBJECT(object)->sprite_flags = 0u;
	OBJECT(object)->flags |= 2u;
	OBJECT(object)->hitbox_size_x = 0x62u;
	OBJECT(object)->hitbox_size_y = 0x28u;
	OBJECT(object)->hitbox_offset_x = -0x3A;
	OBJECT(object)->hitbox_offset_y = -0xE;
	OBJECT(object)->hitbox_height = 0x14u;
	OBJECT(object)->kind = 0xCu;
	OBJECT(object)->handler = treadmill_roller_update_ADDRESS;
	object = LINKED(object, OBJECT(object)->child);
	OBJECT(object)->sprite_frame = 0x7310u;
	OBJECT(object)->sprite_palette = 0x1170u;
	OBJECT(object)->sprite_palette |= 0x4000u;
	r14 = object;
	spawn_count = 2u;
	spawn_object_with_pieces(0x0109D1u, &spawned, &piece_count, &free_count, &object, &block);
	OBJECT(r14)->child = object;
	copy_position_to(object);
	OBJECT(object)->projected_x += 0x73u;
	OBJECT(object)->projected_y_int += 0x31u;
	OBJECT(object)->sprite_frame = 0x7340u;
	OBJECT(object)->sprite_palette = 0x1190u;
	OBJECT(object)->sprite_palette |= 0x4000u;
	OBJECT(object)->flags |= 2u;
	OBJECT(object)->hitbox_size_x = 0x1Eu;
	OBJECT(object)->hitbox_size_y = 0x71u;
	OBJECT(object)->hitbox_offset_x = 0x11;
	OBJECT(object)->hitbox_offset_y = -0x5C;
	OBJECT(object)->hitbox_height = 0x50u;
	OBJECT(object)->kind = 0xCu;
	OBJECT(object)->handler = treadmill_gate_update_ADDRESS;
	gate = (uint16_t)object;
	object = LINKED(object, OBJECT(object)->child);
	OBJECT(object)->sprite_frame = 0x6460u;
	OBJECT(object)->sprite_palette = 0x1170u;
	OBJECT(object)->sprite_palette |= 0x4000u;
	r14 = object;
	spawn_count = 2u;
	spawn_object_with_pieces(0x010A5Au, &spawned, &piece_count, &free_count, &object, &block);
	OBJECT(r14)->child = object;
	OBJECT(object)->parent = gate;
	copy_position_to(object);
	OBJECT(object)->projected_x += 0xC3u;
	OBJECT(object)->projected_y_int += 0x59u;
	OBJECT(object)->sprite_frame = 0x7340u;
	OBJECT(object)->sprite_palette = 0x1190u;
	OBJECT(object)->sprite_palette |= 0x4000u;
	OBJECT(object)->flags |= 2u;
	OBJECT(object)->hitbox_size_x = 0x1Eu;
	OBJECT(object)->hitbox_size_y = 0x71u;
	OBJECT(object)->hitbox_offset_x = 0x11;
	OBJECT(object)->hitbox_offset_y = -0x5C;
	OBJECT(object)->hitbox_height = 0x50u;
	OBJECT(object)->kind = 0xCu;
	OBJECT(object)->handler = treadmill_gate_half_update_ADDRESS;
	object = LINKED(object, OBJECT(object)->child);
	OBJECT(object)->sprite_frame = 0x6460u;
	OBJECT(object)->sprite_palette = 0x1170u;
	OBJECT(object)->sprite_palette |= 0x4000u;
}

void treadmill_update(void)
{
	uint32_t belt_phase = 0, block_offset = 0, bucket = 0, cursor = 0, deleted = 0, ignored = 0, player_block = 0, player_index = 0, slope_height = 0, speed_level = 0, total_speed = 0;

	unproject_object(&ignored, &block_offset, &slope_height, &cursor);
	delete_if_off_screen(&deleted, &block_offset, &slope_height);
	if ((uint8_t)deleted == 1u)
		return;
	OBJECT(current_object)->speed_z = 0;
	player_index = 2u;
	total_speed = 0u;
	do
	{
		player_block = GAME_VARIABLES + treadmill_player_blocks[player_index];
		if (mem_read8(player_block) == 0x22u)
			total_speed += mem_read32(((player_block & ~0xFFFFu) | mem_read16(player_block + 0x14u)) + 0x6Eu);
		player_index--;
	} while ((int32_t)player_index >= 0);
	speed_level = total_speed;
	if (speed_level != 0u)
	{
		speed_level = ((speed_level >> 24) & 0xFu) - OBJECT(current_object)->spawn_arg_1;
		if ((int32_t)speed_level < 0)
			speed_level = 0u;
		OBJECT(current_object)->speed_z = treadmill_lift_forces[speed_level];
		OBJECT(current_object)->treadmill_update.belt_phase += speed_level * 9u;
		OBJECT(current_object)->treadmill_update.belt_phase &= 0x7Fu;
		belt_phase = OBJECT(current_object)->treadmill_update.belt_phase;
		OBJECT(current_object)->sprite_frame = treadmill_belt_sprites[belt_phase >> 4];
	}
	bucket = 0xFFu;
	queue_sprite_in_bucket(bucket);
	if ((OBJECT(current_object)->flags & 1u) == 0u)
		add_to_zone_list(bucket);
	OBJECT(current_object)->flags &= ~0x8000000u;
}

void treadmill_roller_update(void)
{
	uint32_t cursor = 0, depth = 0, ignored = 0;

	unproject_object(&ignored, &ignored, &ignored, &cursor);
	r14 = OBJECT(LINKED(WORK_RAM, OBJECT(current_object)->parent))->var_80 >> 5;
	OBJECT(current_object)->sprite_frame = treadmill_roller_sprites[r14];
	queue_sprite(0u);
	current_object = LINKED(current_object, OBJECT(current_object)->child);
	OBJECT(current_object)->sprite_frame = treadmill_roller_part_sprites[r14];
	OBJECT(current_object)->screen_x = OBJECT(object_cursor)->screen_x;
	OBJECT(current_object)->screen_y = OBJECT(object_cursor)->screen_y;
	OBJECT(current_object)->screen_x += 0x10u;
	OBJECT(current_object)->screen_y -= 8u;
	depth = (uint16_t)(OBJECT(object_cursor)->screen_y + 2u);
	queue_sprite_at_depth(&ignored, &depth);
	current_object = object_cursor;
	OBJECT(current_object)->health = 0x7F;
	add_to_touchable_list(ignored);
}

void treadmill_gate_update(void)
{
	uint8_t sound_id = 0;
	uint32_t r1 = 0, r2 = 0, cursor = 0, depth = 0, draw_bucket = 0, ignored = 0, object = 0;
	uint8_t sound_1e_timer = 0, sound_c_timer = 0;

	move_object_projected(&ignored, &r2, &ignored, &cursor);
	fall_to_floor(&ignored, &r1, &r2);
	OBJECT(current_object)->speed_z += 0u - OBJECT(LINKED(WORK_RAM, OBJECT(current_object)->parent))->speed_z;
	if ((uint16_t)(OBJECT(current_object)->floor_z_int - OBJECT(current_object)->z_int) != 0x28u)
	{
		if (OBJECT(current_object)->speed < OBJECT(current_object)->speed_z || (uint32_t)OBJECT(current_object)->speed_z == 0u)
		{
			if (OBJECT(current_object)->speed < OBJECT(current_object)->speed_z)
			{
				OBJECT(current_object)->speed = OBJECT(current_object)->speed_z;
				OBJECT(current_object)->speed += 0x3C000u;
				sound_1e_timer = OBJECT(current_object)->treadmill_gate_update.sound_1e_timer;
				OBJECT(current_object)->treadmill_gate_update.sound_1e_timer = sound_1e_timer - 1u;
				if (sound_1e_timer == 0u)
				{
					sound_id = 0x1Eu;
					play_sound(sound_id, &r1, &r2);
					OBJECT(current_object)->treadmill_gate_update.sound_1e_timer = 0x14u;
				}
			}
		}
		else
		{
			OBJECT(current_object)->speed = OBJECT(current_object)->speed_z;
			OBJECT(current_object)->speed -= 0x3C000u;
			sound_c_timer = OBJECT(current_object)->treadmill_gate_update.sound_c_timer;
			OBJECT(current_object)->treadmill_gate_update.sound_c_timer = sound_c_timer - 1u;
			if (sound_c_timer == 0u)
			{
				sound_id = 0xCu;
				play_sound(sound_id, &r1, &r2);
				OBJECT(current_object)->treadmill_gate_update.sound_c_timer = 0x3Cu;
			}
		}
	}
	object = LINKED(WORK_RAM, OBJECT(current_object)->parent);
	OBJECT(object)->flags &= ~1u;
	if ((int16_t)(OBJECT(current_object)->floor_z_int - OBJECT(current_object)->z_int) >= 0x28)
	{
		OBJECT(object)->flags |= 1u;
		if ((uint32_t)OBJECT(current_object)->speed_z != 0u && OBJECT(current_object)->treadmill_gate_update.reached_top == 0u)
		{
			sound_id = 0x1Eu;
			play_sound(sound_id, &r1, &r2);
			OBJECT(current_object)->treadmill_gate_update.reached_top = 1u;
		}
		OBJECT(current_object)->speed_z = 0;
	}
	depth = (uint16_t)(((int32_t)((uint32_t)OBJECT(current_object)->projected_y + OBJECT(current_object)->previous_floor_z) >> 16) - view_y - 0x25u);
	queue_sprite_at_depth(&ignored, &depth);
	current_object = LINKED(current_object, OBJECT(object_cursor)->child);
	OBJECT(current_object)->screen_x = OBJECT(object_cursor)->screen_x;
	OBJECT(current_object)->screen_y = OBJECT(object_cursor)->screen_y;
	OBJECT(current_object)->screen_y -= 0x63u;
	draw_bucket = 0u;
	queue_sprite_in_bucket(draw_bucket);
	current_object = object_cursor;
	OBJECT(current_object)->health = 0x7F;
	add_to_touchable_list(draw_bucket);
}

void treadmill_gate_half_update(void)
{
	uint32_t cursor = 0, depth = 0, draw_bucket = 0, ignored = 0;

	current_object = LINKED(current_object, OBJECT(object_cursor)->parent);
	OBJECT(object_cursor)->z = OBJECT(current_object)->z;
	current_object = object_cursor;
	unproject_object(&ignored, &ignored, &ignored, &cursor);
	depth = (uint16_t)(((int32_t)((uint32_t)OBJECT(current_object)->projected_y + OBJECT(current_object)->previous_floor_z) >> 16) - view_y - 0x25u);
	queue_sprite_at_depth(&ignored, &depth);
	current_object = LINKED(current_object, OBJECT(object_cursor)->child);
	OBJECT(current_object)->screen_x = OBJECT(object_cursor)->screen_x;
	OBJECT(current_object)->screen_y = OBJECT(object_cursor)->screen_y;
	OBJECT(current_object)->screen_y -= 0x63u;
	draw_bucket = 0u;
	queue_sprite_in_bucket(draw_bucket);
	current_object = object_cursor;
	OBJECT(current_object)->health = 0x7F;
	add_to_touchable_list(draw_bucket);
}

void flame_jet_init(void)
{
	uint8_t piece_frame = 0;
	uint16_t piece_scale = 0;
	uint32_t block = 0, free_count = 0, object = 0, piece_count = 0, spawned = 0;

	OBJECT(current_object)->sprite_frame = flame_jet_nozzle_sprites[OBJECT(current_object)->spawn_arg_1];
	OBJECT(current_object)->sprite_palette = 0x9180u;
	OBJECT(current_object)->flags |= 2u;
	OBJECT(current_object)->hitbox_size_x = 0x4Cu;
	OBJECT(current_object)->hitbox_size_y = 6u;
	OBJECT(current_object)->hitbox_offset_x = -0x1A;
	OBJECT(current_object)->hitbox_offset_y = -2;
	OBJECT(current_object)->hitbox_height = 0x20u;
	OBJECT(current_object)->kind = 0x14u;
	OBJECT(current_object)->direction = OBJECT(current_object)->spawn_arg_2;
	OBJECT(current_object)->handler = flame_jet_nozzle_update_ADDRESS;
	current_object = LINKED(current_object, OBJECT(current_object)->child);
	OBJECT(current_object)->sprite_frame = 0x6990u;
	OBJECT(current_object)->sprite_palette = 0x9FFEu;
	OBJECT(current_object)->sprite_palette |= 0x4000u;
	OBJECT(current_object)->sprite_flags = 0xAu;
	if (OBJECT(object_cursor)->spawn_arg_1 != 0u)
	{
		OBJECT(current_object)->sprite_flags = 5u;
		OBJECT(current_object)->sprite_flags |= 0x40u;
		OBJECT(current_object)->sprite_flags |= 0x80u;
	}
	spawn_count = 0xAu;
	spawn_object_with_pieces(0x010E60u, &spawned, &piece_count, &free_count, &object, &block);
	current_object = LINKED(current_object, OBJECT(object_cursor)->child);
	OBJECT(current_object)->child = object;
	copy_position_to(object);
	if (OBJECT(current_object)->spawn_arg_1 != 0u)
	{
		OBJECT(object)->projected_x -= 0x26u;
		OBJECT(object)->projected_y_int -= 0x37u;
	}
	OBJECT(object)->sprite_scale = 0x100u;
	OBJECT(object)->spawn_arg_2 = 0x180u;
	OBJECT(object)->sprite_flags = 0u;
	OBJECT(object)->sprite_frame = 0x6910u;
	OBJECT(object)->sprite_palette = 0x7B0u;
	OBJECT(object)->sprite_palette |= 0x4000u;
	OBJECT(object)->flags |= 2u;
	OBJECT(object)->hitbox_size_x = 0x3Cu;
	OBJECT(object)->hitbox_size_y = 6u;
	OBJECT(object)->hitbox_offset_x = 0x10;
	OBJECT(object)->hitbox_offset_y = -0x22;
	OBJECT(object)->hitbox_height = 0x20u;
	OBJECT(object)->kind = 0x14u;
	OBJECT(object)->spawn_arg_1 = OBJECT(object_cursor)->spawn_arg_1;
	OBJECT(object)->flame_jet.growth_rate = 0x200;
	OBJECT(object)->var_88 = 0u;
	OBJECT(current_object)->direction = OBJECT(object_cursor)->spawn_arg_2;
	OBJECT(object)->flame_jet.angle = 0x158u;
	OBJECT(object)->handler = flame_jet_pilot_ADDRESS;
	if (OBJECT(object)->spawn_arg_1 != 0u)
	{
		OBJECT(object)->flame_jet.angle = 0x350u;
		OBJECT(object)->sprite_flags = 7u;
	}
	r14 = object;
	piece_scale = 0x180u;
	piece_frame = 0u;
	for (;;)
	{
		object = LINKED(object, OBJECT(object)->child);
		if ((uint16_t)object == 0xFFFFu)
			break;
		OBJECT(object)->sprite_frame = 0x6910u;
		OBJECT(object)->sprite_palette = 0x7B0u;
		OBJECT(object)->sprite_palette |= 0x4000u;
		OBJECT(object)->sprite_flags = OBJECT(r14)->sprite_flags;
		piece_frame = (uint8_t)((uint8_t)(piece_frame + 1u) & 3u);
		AT8(OBJECT(object)->projected_y, 1) = piece_frame;
		piece_scale -= 0x18u;
		OBJECT(object)->sprite_scale = piece_scale;
	}
}

void flame_jet_nozzle_update(void)
{
	uint8_t bucket = 0;
	uint16_t offset_x = 0, offset_y = 0;
	uint32_t block_offset = 0, cursor = 0, deleted = 0, ignored = 0, slope_height = 0;

	unproject_object(&ignored, &block_offset, &slope_height, &cursor);
	delete_if_off_screen(&deleted, &block_offset, &slope_height);
	if ((uint8_t)deleted == 1u)
		return;
	if (OBJECT(current_object)->spawn_arg_1 != 0u)
	{
		bucket = 0u;
		queue_sprite_in_bucket(bucket);
	}
	else
	{
		queue_sprite(0u);
		OBJECT(current_object)->health = 0x7F;
		current_object = LINKED(current_object, OBJECT(object_cursor)->child);
		current_object = LINKED(current_object, OBJECT(current_object)->child);
		r14 = (uint16_t)((OBJECT(current_object)->projected_x - OBJECT(object_cursor)->projected_x) * 3u) >> 1;
		current_object = object_cursor;
		OBJECT(current_object)->hitbox_size_y = r14;
		add_to_touchable_list(deleted);
	}
	current_object = LINKED(current_object, OBJECT(current_object)->child);
	if (OBJECT(object_cursor)->spawn_arg_1 != 0u)
	{
		offset_x = 3u;
		offset_y = 0x19u;
	}
	else
	{
		offset_x = 1u;
		offset_y = 1u;
	}
	place_sprite_near_cursor(offset_x, offset_y);
	bucket = 0xFFu;
	queue_sprite_in_bucket(bucket);
}

void flame_jet_pilot(void)
{
	if (stage_trigger_bits != 0u)
	{
		OBJECT(current_object)->sprite_scale = OBJECT(current_object)->spawn_arg_2;
		OBJECT(current_object)->flame_jet.growth_rate = 0x380;
		OBJECT(current_object)->handler = flame_jet_ignite_ADDRESS;
		flame_jet_ignite();
		return;
	}
	if (OBJECT(current_object)->flame_jet.growth_rate >= 0)
	{
		if (OBJECT(current_object)->speed <= 0x666)
		{
			flame_jet_burn();
			return;
		}
		OBJECT(current_object)->flame_jet.growth_rate = -0x80;
	}
	if (OBJECT(current_object)->speed > 0)
	{
		flame_jet_burn();
		return;
	}
	OBJECT(current_object)->flame_jet.growth_rate = 0x80;
	OBJECT(current_object)->speed = 0;
	flame_jet_burn();
}

void flame_jet_ignite(void)
{
	if (OBJECT(current_object)->speed <= 0xF000)
	{
		flame_jet_burn();
		return;
	}
	OBJECT(current_object)->flame_jet.growth_rate = 0;
	OBJECT(current_object)->handler = flame_jet_burn_ADDRESS;
	flame_jet_burn();
}

void flame_jet_burn(void)
{
	uint16_t flame_step_y = 0, offset_x = 0, offset_y = 0, piece_number = 0;
	uint32_t cursor = 0, depth = 0, flame_angle = 0, ignored = 0, speed = 0, speed_x = 0, speed_y = 0;
	uint16_t projected_x = 0, nozzle_x = 0, projected_y_int = 0, nozzle_y = 0;

	OBJECT(current_object)->speed += OBJECT(current_object)->flame_jet.growth_rate;
	flame_angle = OBJECT(current_object)->flame_jet.angle;
	speed = OBJECT(current_object)->speed;
	direction_to_speed(speed, &flame_angle, &speed_x, &speed_y);
	OBJECT(current_object)->speed_x = speed_x;
	OBJECT(current_object)->speed_y = speed_y;
	current_object = LINKED(current_object, OBJECT(object_cursor)->parent);
	OBJECT(object_cursor)->projected_x = OBJECT(current_object)->projected_x;
	OBJECT(object_cursor)->projected_y = OBJECT(current_object)->projected_y;
	current_object = object_cursor;
	if (OBJECT(current_object)->spawn_arg_1 == 0u)
	{
		OBJECT(current_object)->projected_x += 0xCu;
		OBJECT(current_object)->projected_y_int += 6u;
	}
	else
	{
		OBJECT(current_object)->projected_x -= 0x16u;
		OBJECT(current_object)->projected_y_int += 0xDu;
	}
	move_object_projected(&ignored, &ignored, &ignored, &cursor);
	OBJECT(current_object)->speed_x = 0;
	OBJECT(current_object)->speed_y = 0;
	OBJECT(current_object)->flame_jet.frame_phase++;
	OBJECT(current_object)->flame_jet.frame_phase &= 3u;
	OBJECT(current_object)->sprite_frame = flame_jet_flame_sprites[(uint8_t)OBJECT(current_object)->flame_jet.frame_index];
	OBJECT(current_object)->flame_jet.frame_index++;
	if (OBJECT(current_object)->flame_jet.frame_index > 4)
		OBJECT(current_object)->flame_jet.frame_index = 0;
	depth = (uint16_t)OBJECT(current_object)->screen_y;
	OBJECT(current_object)->screen_y -= 0x28u;
	queue_sprite_at_depth(&ignored, &depth);
	if (OBJECT(current_object)->spawn_arg_1 != 0u)
	{
		current_object = LINKED(current_object, OBJECT(object_cursor)->parent);
		r14 = (uint16_t)(OBJECT(current_object)->projected_x - OBJECT(object_cursor)->projected_x);
		current_object = object_cursor;
		OBJECT(current_object)->health = 0x7F;
		OBJECT(current_object)->hitbox_size_y = r14;
		add_to_touchable_list(ignored);
	}
	if (stage_trigger_bits == 0u)
		return;
	current_object = LINKED(current_object, OBJECT(object_cursor)->parent);
	r14 = (uint16_t)OBJECT(current_object)->projected_x;
	flame_step_y = (uint16_t)OBJECT(current_object)->projected_y_int;
	current_object = object_cursor;
	if (OBJECT(current_object)->spawn_arg_1 == 0u)
	{
		r14 = (uint16_t)(r14 - 6u);
		flame_step_y += 5u;
	}
	else
		flame_step_y += 0x18u;
	projected_x = OBJECT(object_cursor)->projected_x;
	nozzle_x = r14;
	r14 = (uint16_t)(nozzle_x - projected_x);
	if (nozzle_x != projected_x)
		r14 = (uint16_t)((int16_t)r14 / 0xA);
	projected_y_int = OBJECT(object_cursor)->projected_y_int;
	nozzle_y = flame_step_y;
	flame_step_y = (uint16_t)(nozzle_y - projected_y_int);
	if (nozzle_y != projected_y_int)
		flame_step_y = (uint16_t)((int16_t)flame_step_y / 0xA);
	current_object = LINKED(current_object, OBJECT(object_cursor)->child);
	piece_number = 1u;
	do
	{
		OBJECT(current_object)->sprite_frame = OBJECT(object_cursor)->sprite_frame;
		offset_x = (uint16_t)(r14 * piece_number);
		offset_y = (uint16_t)((uint32_t)flame_step_y * piece_number);
		place_sprite_near_cursor(offset_x, offset_y);
		OBJECT(current_object)->direction++;
		OBJECT(current_object)->direction &= 3u;
		OBJECT(current_object)->sprite_frame = flame_jet_flame_sprites[AT8(OBJECT(current_object)->projected_y, 1)];
		AT8(OBJECT(current_object)->projected_y, 1)++;
		if ((int8_t)AT8(OBJECT(current_object)->projected_y, 1) > 4)
			AT8(OBJECT(current_object)->projected_y, 1) = 0u;
		depth = (uint16_t)(OBJECT(current_object)->screen_y + 0x28u);
		queue_sprite_at_depth(&ignored, &depth);
		piece_number++;
		current_object = LINKED(current_object, OBJECT(current_object)->child);
	} while ((uint16_t)current_object != 0xFFFFu);
}
