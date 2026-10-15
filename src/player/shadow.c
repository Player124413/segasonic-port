/**
 *    Name:        player/shadow.c
 *    Description: the player's shadow object (over water and quicksand too) and the attack spin effect
 *    Version:     0.1.0
 *    Created:     12-10-2026
 *    Modified:    15-10-2026
 *    Author:      Sebastian Toyos
 *
 *    SegaSonic The Hedgehog (arcade, Sega System 32): the game's V60 program decompiled into C by
 *    tools/decompile.py, with the names of symbols.txt.
 */
#include "game.h"

void shadow_1_start(void)
{
	OBJECT(current_object)->player = 0u;
	shadow_start();
}

void shadow_2_start(void)
{
	OBJECT(current_object)->player = 1u;
	shadow_start();
}

void shadow_3_start(void)
{
	OBJECT(current_object)->player = 2u;
	shadow_start();
}

void shadow_start(void)
{
	uint32_t free_count = 0, ignored = 0, object = 0, object_2 = 0;

	OBJECT(current_object)->sprite_flags = 0x1008u;
	OBJECT(current_object)->sprite_palette = 0x1FFEu;
	OBJECT(current_object)->sprite_frame = 0x5D90u;
	OBJECT(current_object)->sprite_scale = 0x100u;
	OBJECT(current_object)->flags = 0u;
	OBJECT(current_object)->sprite_layer = 0u;
	OBJECT(current_object)->z = 0;
	OBJECT(current_object)->shadow.flicker_timer = 0;
	OBJECT(current_object)->sprite_offset_x = 0;
	OBJECT(current_object)->sprite_offset_y = -2;
	OBJECT(current_object)->animation_frame = 0u;
	OBJECT(current_object)->animation_speed = 2u;
	OBJECT(current_object)->animation_timer = 0;
	OBJECT(current_object)->handler = shadow_update_ADDRESS;
	OBJECT(current_object)->child = 0xFFFFu;
	allocate_object_2(&free_count, &ignored, &object);
	if (free_count != 0u)
	{
		OBJECT(object)->sprite_flags = 0x1004u;
		OBJECT(object)->parent = current_object;
		OBJECT(current_object)->child = object;
		OBJECT(object)->child = 0xFFFFu;
		if (stage == 9u)
		{
			object_2 = object;
			allocate_object_2(&free_count, &ignored, &object);
			if (free_count != 0u)
			{
				OBJECT(object)->sprite_flags = 0x1088u;
				OBJECT(object)->parent = object_2;
				OBJECT(object_2)->child = object;
			}
		}
	}
}

void shadow_update(void)
{
	uint16_t priority_bits = 0;
	uint32_t r1 = 0, r7 = 0, r8 = 0, depth = 0, ignored = 0, offset_y = 0, player = 0, point_x = 0, point_y = 0, scaled_offset_y = 0, scaled_player = 0, scene_player = 0, sprite_scale = 0, z = 0;
	uint8_t flicker_timer = 0;

	player = PLAYER_BLOCK(1) + ((uint32_t)OBJECT(current_object)->player << 7) * 4u;
	current_player = LINKED(current_player, PLAYER(player)->object);
	PLAYER(player)->flags &= ~1u;
	if ((PLAYER(player)->flags & 8u) != 0u || PLAYER(player)->state == 0xBu || PLAYER(player)->state == 0x23u || PLAYER(player)->state == 0x24u || PLAYER(player)->state == 0x16u || PLAYER(player)->state == 0x2Eu || PLAYER(player)->state == 0x37u)
		return;
	if (PLAYER(player)->state == 0x10u)
	{
		draw_attack_effect(player);
		return;
	}
	if (PLAYER(player)->state == 0x11u || PLAYER(player)->state == 0x12u || PLAYER(player)->state == 0x15u || PLAYER(player)->state == 0x1Du || PLAYER(player)->state == 0x27u || PLAYER(player)->state == 0x2Du || PLAYER(player)->state == 0x34u || PLAYER(player)->state == 0x35u || PLAYER(player)->state == 0x39u || (PLAYER(player)->state == 0x1Eu && (uint8_t)intro_character != OBJECT(current_player)->kind) || (PLAYER(player)->state == 0x32u && (int8_t)PLAYER(player)->substate < 3) || (PLAYER(player)->state == 0x25u && ((PLAYER(player)->sequence_step >= 8 && PLAYER(player)->sequence_step <= 0xC) || PLAYER(player)->sequence_step == 0xEu)) || (OBJECT(current_player)->flags & 0x10000000u) != 0u || (current_game_mode == 0xAu && (int8_t)scene_step >= 6))
		return;
	if (current_game_mode != 0x10u)
	{
		if (stage != 9u || (OBJECT(current_player)->floor_type != 8u && (OBJECT(current_player)->floor_type != 7u || (OBJECT(current_player)->flags & 0x10u) != 0u)))
		{
			if (PLAYER(player)->state == 0x28u || (OBJECT(current_player)->floor_type != 0xAu && (OBJECT(current_player)->flags & 0x80000000u) == 0u && OBJECT(current_player)->floor_type != 0xBu))
			{
				if (PLAYER(player)->state == 0x33u)
				{
					OBJECT(current_object)->projected_x = OBJECT(current_player)->projected_x;
					OBJECT(current_object)->projected_y = OBJECT(current_player)->projected_y;
					OBJECT(current_object)->z = OBJECT(current_player)->floor_z;
					if (OBJECT(current_player)->floor_block < 0)
						OBJECT(current_object)->z = OBJECT(current_player)->previous_floor_z;
					OBJECT(current_object)->z_int += 8u;
					OBJECT(current_object)->sprite_flags = 0x1004u;
					OBJECT(current_object)->flags &= ~0x80000u;
					OBJECT(current_object)->sprite_palette = 0x2D50u;
					priority_bits = OBJECT(current_player)->sprite_palette & 0x6000u;
					OBJECT(current_object)->sprite_palette &= 0x9FFFu;
					OBJECT(current_object)->sprite_palette |= priority_bits;
					OBJECT(current_object)->animation = pit_sinking_shadow_animation_ADDRESS;
					animate(&offset_y, &r7, &r8);
					update_screen_position(&ignored, &offset_y);
					depth = (uint16_t)(OBJECT(current_object)->screen_y + 0x10u);
					queue_sprite_at_depth(&ignored, &depth);
					draw_attack_effect(player);
					return;
				}
				if (player_status[OBJECT(current_object)->player] == 0x82u)
					return;
				if (OBJECT(current_player)->floor_block < 0)
				{
					draw_attack_effect(player);
					return;
				}
				OBJECT(current_object)->projected_x = OBJECT(current_player)->projected_x;
				OBJECT(current_object)->projected_y = OBJECT(current_player)->projected_y;
				OBJECT(current_object)->projected_y_int -= 3u;
				OBJECT(current_object)->z = OBJECT(current_player)->floor_z;
				OBJECT(current_object)->sprite_flags = 0x1008u;
				OBJECT(current_object)->sprite_frame = 0x5D90u;
				OBJECT(current_object)->flags &= ~0x80000u;
				if (OBJECT(current_player)->floor_type == 9u)
				{
				shadow_update_flicker:
					OBJECT(current_object)->sprite_palette = 0x87A0u;
					flicker_timer = OBJECT(current_object)->shadow.flicker_timer;
					OBJECT(current_object)->shadow.flicker_timer = flicker_timer - 1u;
					if ((int8_t)flicker_timer <= 1)
					{
						OBJECT(current_object)->shadow.flicker_timer = 2;
						OBJECT(current_object)->flags |= 0x80000u;
					}
				}
				else
				{
					if (OBJECT(current_player)->floor_type == 4u || (stage == 0x10u && mem_read8(floor_blocks + 2u + OBJECT(current_player)->floor_block + 0x12u) == 0x20u))
					{
						OBJECT(current_object)->sprite_palette = 0x7A0u;
						flicker_timer = OBJECT(current_object)->shadow.flicker_timer;
						OBJECT(current_object)->shadow.flicker_timer = flicker_timer - 1u;
						if ((int8_t)flicker_timer <= 1)
						{
							OBJECT(current_object)->shadow.flicker_timer = 2;
							OBJECT(current_object)->flags |= 0x80000u;
						}
						priority_bits = OBJECT(current_player)->sprite_palette & 0x6000u;
						OBJECT(current_object)->sprite_palette &= 0x9FFFu;
						OBJECT(current_object)->sprite_palette |= priority_bits;
						update_screen_position(&ignored, &offset_y);
						if (per_player_view_mode != 0u && scroll_position >= 0xA5F0u)
						{
							offset_y = (offset_y & ~0xFFFFu) | (uint16_t)OBJECT(current_object)->sprite_offset_y;
							OBJECT(current_object)->screen_y = ((int32_t)((uint32_t)OBJECT(current_object)->projected_y + OBJECT(current_object)->z) >> 16) - PLAYER(player)->own_view_y + offset_y;
						}
						if ((OBJECT(current_object)->flags & 0x80000u) == 0u)
							queue_sprite(offset_y);
						draw_attack_effect(player);
						return;
					}
					if ((AS16(current_game_mode) == 0x1Au && (int8_t)PLAYER(player)->sequence_step >= 2) || (OBJECT(current_player)->flags & 0x10u) != 0u)
						goto shadow_update_flicker;
					OBJECT(current_object)->sprite_palette = 0x1FFEu;
				}
				priority_bits = OBJECT(current_player)->sprite_palette & 0x6000u;
				OBJECT(current_object)->sprite_palette &= 0x9FFFu;
				OBJECT(current_object)->sprite_palette |= priority_bits;
				update_screen_position(&ignored, &offset_y);
				if (per_player_view_mode != 0u && scroll_position >= 0xA5F0u)
				{
					offset_y = (offset_y & ~0xFFFFu) | (uint16_t)OBJECT(current_object)->sprite_offset_y;
					OBJECT(current_object)->screen_y = ((int32_t)((uint32_t)OBJECT(current_object)->projected_y + OBJECT(current_object)->z) >> 16) - PLAYER(player)->own_view_y + offset_y;
				}
				if ((OBJECT(current_object)->flags & 0x80000u) != 0u)
				{
					draw_attack_effect(player);
					return;
				}
				queue_sprite(offset_y);
				draw_attack_effect(player);
				return;
			}
			if (PLAYER(player)->state == 6u || PLAYER(player)->state == 8u || PLAYER(player)->state == 7u || PLAYER(player)->state == 9u)
			{
				draw_attack_effect(player);
				return;
			}
			PLAYER(player)->flags |= 1u;
			OBJECT(current_object)->projected_x = OBJECT(current_player)->projected_x;
			OBJECT(current_object)->projected_y = OBJECT(current_player)->projected_y;
			OBJECT(current_object)->z = OBJECT(current_player)->floor_z;
			if (OBJECT(current_player)->floor_block < 0)
				OBJECT(current_object)->z = OBJECT(current_player)->previous_floor_z;
			if (OBJECT(current_player)->floor_block < 0)
				z = OBJECT(current_player)->previous_floor_z;
			else
				z = OBJECT(current_player)->floor_z;
			point_x = OBJECT(current_player)->x;
			point_y = OBJECT(current_player)->y;
			project_point(z, &r1, &point_x, &point_y);
			PLAYER(player)->ground_clip_bottom = point_y;
			OBJECT(current_object)->sprite_flags = 0u;
			OBJECT(current_object)->flags &= ~0x80000u;
			OBJECT(current_object)->sprite_palette = 0x1020u;
			OBJECT(current_object)->sprite_scale = 0x120u;
			priority_bits = OBJECT(current_player)->sprite_palette & 0x6000u;
			OBJECT(current_object)->sprite_palette &= 0x9FFFu;
			OBJECT(current_object)->sprite_palette |= priority_bits;
			OBJECT(current_object)->animation = sinking_shadow_animation_ADDRESS;
			animate(&r1, &point_x, &point_y);
			update_screen_position(&ignored, &r1);
			depth = (uint16_t)(OBJECT(current_object)->screen_y + 0x10u);
			queue_sprite_at_depth(&ignored, &depth);
			draw_attack_effect(player);
			return;
		}
		if (OBJECT(current_player)->floor_block >= 0)
		{
			OBJECT(current_object)->projected_x = OBJECT(current_player)->projected_x;
			OBJECT(current_object)->projected_y = OBJECT(current_player)->projected_y;
			OBJECT(current_object)->z = (uint32_t)OBJECT(current_player)->floor_z - OBJECT(current_player)->z + OBJECT(current_player)->floor_z;
			OBJECT(current_object)->sprite_frame = OBJECT(current_player)->sprite_frame;
			OBJECT(current_object)->sprite_palette = OBJECT(current_player)->sprite_palette;
			OBJECT(current_object)->sprite_palette &= 0x1FFFu;
			OBJECT(current_object)->sprite_palette |= 0x2000u;
			OBJECT(current_object)->sprite_flags = 0x1188u;
			OBJECT(current_object)->sprite_flags |= OBJECT(current_player)->sprite_flags & 0x40u;
			update_screen_position(&ignored, &offset_y);
			depth = (uint16_t)(OBJECT(current_object)->screen_y - 0x18u);
			queue_sprite_at_depth(&ignored, &depth);
		}
		draw_attack_effect(player);
		return;
	}
	switch (scene_step)
	{
	case 0:
	case 1:
	case 4:
		OBJECT(current_object)->sprite_flags = 0u;
		OBJECT(current_object)->sprite_scale = 0x200u;
		OBJECT(current_object)->sprite_palette = 0x160u;
		scene_player = PLAYER_BLOCK(1) + ((uint32_t)OBJECT(current_object)->player << 7) * 4u;
		scene_player = LINKED(scene_player, PLAYER(scene_player)->object);
		OBJECT(current_object)->screen_x = OBJECT(scene_player)->screen_x;
		OBJECT(current_object)->screen_y = OBJECT(scene_player)->screen_y;
		OBJECT(current_object)->screen_y -= 8u;
		depth = (uint16_t)(OBJECT(current_object)->screen_y - 8u);
		queue_sprite_at_depth(&ignored, &depth);
		return;
	case 2:
		OBJECT(current_object)->sprite_flags = 0u;
		OBJECT(current_object)->sprite_frame = 0x1B60u;
		OBJECT(current_object)->sprite_palette = 0x8130u;
		OBJECT(current_object)->sprite_width = 0;
		OBJECT(current_object)->sprite_height = 8;
		return;
	case 3:
		scaled_player = PLAYER_BLOCK(1) + ((uint32_t)OBJECT(current_object)->player << 7) * 4u;
		scaled_player = LINKED(scaled_player, PLAYER(scaled_player)->object);
		OBJECT(current_object)->sprite_scale = OBJECT(scaled_player)->sprite_scale;
		OBJECT(current_object)->screen_x = OBJECT(scaled_player)->screen_x;
		OBJECT(current_object)->screen_y = OBJECT(scaled_player)->screen_y;
		sprite_scale = OBJECT(current_object)->sprite_scale;
		OBJECT(current_object)->screen_x += (int32_t)(OBJECT(current_object)->sprite_width * sprite_scale) >> 8;
		scaled_offset_y = (int32_t)(OBJECT(current_object)->sprite_height * sprite_scale) >> 8;
		OBJECT(current_object)->screen_y += scaled_offset_y;
		scaled_offset_y = queue_sprite(scaled_offset_y);
		return;
	default:
		unknown_code(0x6E3Eu + scene_step * 3u);
	}
}

void draw_attack_effect(uint32_t player)
{
	uint16_t palette = 0, priority_bits = 0;
	uint32_t depth = 0, effect_record = 0, facing_offset = 0, ignored = 0, object = 0, object_2 = 0;

	if ((OBJECT(current_player)->flags & 0x40u) == 0u || PLAYER(player)->attack_power <= 2 || PLAYER(player)->state == 9u || PLAYER(player)->state == 0xAu || (OBJECT(current_player)->flags & 0x80000000u) != 0u || (OBJECT(current_player)->flags & 0x20000u) != 0u || (int16_t)OBJECT(current_object)->child < 0)
		return;
	object = LINKED(current_object, OBJECT(current_object)->child);
	effect_record = attack_power_3_effect_animations_ADDRESS;
	if (PLAYER(player)->attack_power > 3)
		effect_record = attack_power_4_effect_animations_ADDRESS;
	facing_offset = ROM16(effect_record + PLAYER(player)->facing * 2u);
	effect_record = effect_record + facing_offset + 2u + OBJECT(current_player)->animation_frame * 2u * 2u;
	OBJECT(object)->sprite_flags = (OBJECT(object)->sprite_flags & ~0xC0u) | ROM16(effect_record);
	OBJECT(object)->sprite_frame = ROM16(effect_record + 2u);
	palette = 0x8320u;
	if (OBJECT(current_player)->kind != 0u)
	{
		palette = 0x8700u;
		if (OBJECT(current_player)->kind != 1u)
			palette = 0x8710u;
	}
	OBJECT(object)->sprite_palette = palette;
	priority_bits = OBJECT(current_player)->sprite_palette & 0x6000u;
	OBJECT(object)->sprite_palette &= 0x9FFFu;
	OBJECT(object)->sprite_palette |= priority_bits;
	OBJECT(object)->screen_x = OBJECT(current_player)->screen_x;
	OBJECT(object)->screen_y = OBJECT(current_player)->screen_y;
	current_object = (current_object & ~0xFFFFu) | (uint16_t)object;
	depth = (uint16_t)OBJECT(current_object)->screen_y;
	depth = (uint16_t)(depth + 8u);
	queue_sprite_at_depth(&ignored, &depth);
	current_object = (current_object & ~0xFFFFu) | (uint16_t)object_cursor;
	if (OBJECT(current_player)->floor_block >= 0 && (int16_t)OBJECT(object)->child >= 0 && OBJECT(current_player)->floor_type == 7u && (OBJECT(current_player)->flags & 0x10u) == 0u)
	{
		object_2 = object;
		object = LINKED(object, OBJECT(object)->child);
		OBJECT(object)->screen_x = OBJECT(current_object)->screen_x;
		OBJECT(object)->screen_y = OBJECT(current_object)->screen_y;
		OBJECT(object)->sprite_frame = OBJECT(object_2)->sprite_frame;
		OBJECT(object)->sprite_flags &= ~0x40u;
		OBJECT(object)->sprite_flags |= OBJECT(object_2)->sprite_flags & 0x40u;
		OBJECT(object)->sprite_palette = OBJECT(object_2)->sprite_palette;
		priority_bits = OBJECT(object_cursor)->sprite_palette & 0xE000u;
		OBJECT(object)->sprite_palette &= 0x1FFFu;
		OBJECT(object)->sprite_palette |= priority_bits;
		current_object = (current_object & ~0xFFFFu) | (uint16_t)object;
		queue_sprite(depth);
		current_object = (current_object & ~0xFFFFu) | (uint16_t)object_cursor;
	}
}
