/**
 *    Name:        player/ledges_and_climbing.c
 *    Description: states 5-0xA: teetering on a ledge, slipping off it, going over it, dropping, falling into a pit, climbing a wall
 *    Version:     0.1.0
 *    Created:     12-10-2026
 *    Modified:    15-10-2026
 *    Author:      Sebastian Toyos
 *
 *    SegaSonic The Hedgehog (arcade, Sega System 32): the game's V60 program decompiled into C by
 *    tools/decompile.py, with the names of symbols.txt.
 */
#include "game.h"

void player_state_teetering(uint32_t cursor)
{
	uint8_t new_status = 0, sound_id = 0;
	uint32_t r1 = 0, r7 = 0, r8 = 0, block = 0, depth = 0, edge_direction = 0, ignored = 0, ledge_side = 0, side = 0, teeter_animation = 0, teeter_variant = 0, write_index = 0, write_index_address = 0;
	uint16_t state_timer = 0;

	if (PLAYER(current_player)->substate == 0u)
	{
		PLAYER(current_player)->substate++;
		OBJECT(current_object)->animation_speed = 3u;
		OBJECT(current_object)->animation_timer = 3;
		OBJECT(current_object)->animation_frame = 0xFFu;
		get_ledge_side(&side, &block);
		PLAYER(current_player)->ledge_side = side;
		switch (side)
		{
		case 0:
		case 5:
			teeter_variant = 0u;
			OBJECT(current_object)->sprite_flags &= 0xFFBFu;
			OBJECT(current_object)->x -= OBJECT(current_object)->speed_x;
			OBJECT(current_object)->y = mem_read32(block + 8u);
			break;
		case 1:
			teeter_variant = 0u;
			OBJECT(current_object)->sprite_flags |= 0x40u;
			OBJECT(current_object)->x = mem_read32(block + 4u);
			OBJECT(current_object)->y -= OBJECT(current_object)->speed_y;
			break;
		case 2:
			teeter_variant = 1u;
			OBJECT(current_object)->sprite_flags |= 0x40u;
			OBJECT(current_object)->x -= OBJECT(current_object)->speed_x;
			OBJECT(current_object)->y = mem_read32(block + 0xCu);
			break;
		case 3:
			teeter_variant = 1u;
			OBJECT(current_object)->sprite_flags &= 0xFFBFu;
			OBJECT(current_object)->x = mem_read32(block);
			OBJECT(current_object)->y -= OBJECT(current_object)->speed_y;
			break;
		case 4:
			if (OBJECT(current_object)->floor_type == 0xCu || mem_read8(floor_blocks + 2u + OBJECT(current_object)->previous_floor_block + 0x12u) == 0x11u || mem_read8(floor_blocks + 2u + OBJECT(current_object)->floor_block + 0x12u) == 0x1Eu)
			{
				edge_direction = (uint16_t)(((OBJECT(current_object)->direction + 0x200u) & 0x3FFu) + 0x80u) & 0x3FFu;
				edge_direction = edge_direction >> 8;
			}
			else
				edge_direction = mem_read8(floor_blocks + 2u + OBJECT(current_object)->previous_floor_block + 0x15u);
			switch (edge_direction)
			{
			case 0:
			player_teetering_ledge_direction_2:
				teeter_variant = 1u;
				OBJECT(current_object)->sprite_flags |= 0x40u;
				PLAYER(current_player)->ledge_direction = 2;
				break;
			case 1:
				teeter_variant = 1u;
				OBJECT(current_object)->sprite_flags &= 0xFFBFu;
				PLAYER(current_player)->ledge_direction = 3;
				break;
			case 2:
				if (OBJECT(current_object)->floor_attribute == 4u)
					goto player_teetering_ledge_direction_2;
				teeter_variant = 0u;
				OBJECT(current_object)->sprite_flags &= 0xFFBFu;
				PLAYER(current_player)->ledge_direction = 0;
				break;
			case 3:
			case 4:
				teeter_variant = 0u;
				OBJECT(current_object)->sprite_flags |= 0x40u;
				PLAYER(current_player)->ledge_direction = 1;
				break;
			default:
				unknown_code(0x3903u + edge_direction * 2u);
			}
			break;
		default:
			unknown_code(0x3837u + side * 2u);
		}
		cursor = teetering_animations_ADDRESS;
		teeter_animation = ROM16(teetering_animations_ADDRESS + OBJECT(current_object)->kind * 4u + teeter_variant * 2u) + teetering_animations_ADDRESS;
		OBJECT(current_object)->sprite_frame = ROM16(teeter_animation);
		OBJECT(current_object)->animation = teeter_animation + 2u;
		PLAYER(current_player)->state_timer = 0x3C;
		if ((OBJECT(current_object)->flags & 4u) == 0u || OBJECT(LINKED(current_object, OBJECT(current_object)->touched_by))->kind != 0x19u)
		{
			if ((uint16_t)PLAYER(current_player)->ledge_side == 4u)
				PLAYER(current_player)->state_timer = 1;
		}
		else
			PLAYER(current_player)->state_timer = 0;
		PLAYER(current_player)->ledge_pullback = 0;
		OBJECT(current_object)->speed_x = 0;
		OBJECT(current_object)->speed_y = 0;
		PLAYER(current_player)->control_lock = 0u;
		OBJECT(current_object)->flags &= ~0x2000u;
		if (((OBJECT(current_object)->flags & 4u) == 0u || OBJECT(LINKED(current_object, OBJECT(current_object)->touched_by))->kind != 0x19u) && (uint16_t)PLAYER(current_player)->ledge_side != 4u)
		{
			sound_id = 0x38u;
			if (OBJECT(current_object)->kind != 0u)
			{
				sound_id = 0x3Cu;
				if (OBJECT(current_object)->kind != 1u)
					sound_id = 0x44u;
			}
			play_sound(sound_id, &write_index, &write_index_address);
		}
	}
	apply_moving_floor();
	r1 = update_ledge_pullback();
	if ((uint16_t)PLAYER(current_player)->ledge_side == 4u)
		PLAYER(current_player)->ledge_pullback = 0;
	if (PLAYER(current_player)->ledge_pullback < 0x10)
	{
		if ((uint16_t)PLAYER(current_player)->state_timer == 0u)
		{
		player_teetering_fall_off:
			PLAYER(current_player)->state = 6u;
			PLAYER(current_player)->substate = 0u;
			if (OBJECT(current_object)->floor_type != 0xDu)
			{
				new_status = 0xFFu;
				if (OBJECT(current_object)->health <= 8)
				{
					new_status = 1u;
					OBJECT(current_object)->flags |= 0x400u;
				}
				r1 = OBJECT(current_object)->player;
				player_status[r1] = new_status;
			}
			PLAYER(current_player)->mishap_flag = 0xFFu;
		}
		else
		{
			state_timer = PLAYER(current_player)->state_timer;
			PLAYER(current_player)->state_timer = state_timer - 1u;
			if ((int16_t)state_timer <= 1)
			{
				PLAYER(current_player)->state_timer = -1;
				if (OBJECT(current_object)->animation_speed == (uint8_t)OBJECT(current_object)->animation_timer && OBJECT(current_object)->animation_frame == 7u)
					goto player_teetering_fall_off;
			}
		}
	}
	else
	{
		PLAYER(current_player)->state = 0u;
		PLAYER(current_player)->substate = 0u;
		OBJECT(current_object)->speed_x = 0;
		OBJECT(current_object)->speed_y = 0;
		OBJECT(current_object)->speed = 0;
		PLAYER(current_player)->animation_speed = 0;
		ledge_side = (uint16_t)PLAYER(current_player)->ledge_side;
		PLAYER(current_player)->heading = ledge_recover_headings[ledge_side];
		PLAYER(current_player)->target_heading = ledge_recover_headings[ledge_side];
		PLAYER(current_player)->facing_angle = ledge_recover_headings[ledge_side];
		OBJECT(current_object)->floor_block = OBJECT(current_object)->previous_floor_block;
		OBJECT(current_object)->floor_z = OBJECT(current_object)->previous_floor_z;
	}
	if (PLAYER(current_player)->state != 6u)
	{
		check_zone_contact();
		check_player_contacts(&r1, &cursor);
	}
	update_player_blink();
	remember_platform();
	add_player_to_lists();
	animate(&r1, &r7, &r8);
	project_object(&ignored, &r1);
	update_sinking_floor(r1);
	if ((OBJECT(current_object)->flags & 0x80000u) != 0u)
		return;
	if ((PLAYER(current_player)->flags & 1u) == 0u)
	{
		r1 = queue_sprite_at_base(r1);
		return;
	}
	PLAYER(current_player)->draw_depth = ((int32_t)((uint32_t)OBJECT(current_object)->projected_y + OBJECT(current_object)->previous_floor_z) >> 16) - view_y;
	current_object = current_player + 0x102u;
	depth = PLAYER(current_player)->draw_depth;
	queue_sprite_at_depth(&ignored, &depth);
	current_object = object_cursor;
	depth = PLAYER(current_player)->draw_depth;
	queue_sprite_at_depth(&ignored, &depth);
	current_object = current_player + 0xF0u;
	depth = PLAYER(current_player)->draw_depth;
	queue_sprite_at_depth(&ignored, &depth);
}

void player_state_falling_off_edge(void)
{
	uint8_t sound_id = 0;
	uint16_t x_shift = 0;
	uint32_t at_last_step = 0, depth = 0, floor_block_record = 0, ignored = 0, sink_threshold = 0, write_index_address = 0;
	uint8_t health = 0;

	switch (PLAYER(current_player)->substate)
	{
	case 0:
	case 2:
		set_fall_sprite_priority();
		PLAYER(current_player)->fall_draw_depth = ((int32_t)((uint32_t)OBJECT(current_object)->projected_y + OBJECT(current_object)->previous_floor_z) >> 16) - view_y;
		floor_block_record = floor_blocks + 2u + OBJECT(current_object)->floor_block;
		if (mem_read8(floor_block_record + 0x12u) == 0x1Du)
		{
			if (OBJECT(current_object)->x < (int32_t)(((mem_read32(floor_block_record + 4u) - mem_read32(floor_block_record)) >> 1) + mem_read32(floor_block_record)))
				goto player_falling_off_edge_depth_minus_10;
			PLAYER(current_player)->fall_draw_depth -= 0x70u;
			goto player_falling_off_edge_depth_minus_10;
		}
		if (mem_read8(floor_blocks + 2u + OBJECT(current_object)->previous_floor_block + 0x12u) != 0x1Fu && mem_read8(floor_blocks + 2u + OBJECT(current_object)->previous_floor_block + 0x12u) != 0x11u)
		{
			if (OBJECT(current_object)->floor_type == 1u)
			{
				if ((uint16_t)PLAYER(current_player)->ledge_side != 4u)
				{
					if (PLAYER(current_player)->ledge_side < 2)
					{
					player_falling_off_edge_floor_1_depth:
						PLAYER(current_player)->fall_draw_depth = OBJECT(LINKED(current_object, OBJECT(current_object)->platform))->var_88 - 1u;
					}
				player_falling_off_edge_depth_minus_10:
					PLAYER(current_player)->fall_draw_depth -= 0x10u;
				}
				else if (OBJECT(current_object)->floor_attribute != 5u)
				{
					if (PLAYER(current_player)->ledge_direction >= 2)
						goto player_falling_off_edge_depth_minus_10;
					goto player_falling_off_edge_floor_1_depth;
				}
			}
			else if (PLAYER(current_player)->ledge_side < 2)
			{
				if (stage != 0xAu)
					goto player_falling_off_edge_depth_minus_10;
				PLAYER(current_player)->fall_draw_depth -= 0x20u;
				goto player_falling_off_edge_depth_minus_10;
			}
		}
		else
		{
			if ((uint16_t)PLAYER(current_player)->ledge_side != 4u)
			{
				if (PLAYER(current_player)->ledge_side < 2)
				{
				player_falling_off_edge_block_1f_depth:
					PLAYER(current_player)->fall_draw_depth = OBJECT(LINKED(current_object, OBJECT(current_object)->platform))->var_88 - 1u;
				}
			}
			else if (PLAYER(current_player)->ledge_direction < 2)
				goto player_falling_off_edge_block_1f_depth;
			PLAYER(current_player)->fall_draw_depth -= 4u;
		}
		OBJECT(current_object)->animation += 0x1Cu;
		OBJECT(current_object)->animation_frame = 0xFFu;
		OBJECT(current_object)->animation_speed = 3u;
		OBJECT(current_object)->animation_timer = 0;
		OBJECT(current_object)->sprite_flags &= ~0xFu;
		OBJECT(current_object)->sprite_flags |= 0xAu;
		OBJECT(current_object)->sprite_palette |= 0x8000u;
		OBJECT(current_object)->projected_y -= 0x200000u;
		OBJECT(current_object)->sprite_layer = OBJECT(current_object)->projected_y_int;
		x_shift = 0x18u;
		if ((int32_t)OBJECT(current_object)->animation >= 0x1128EE)
			x_shift = 0x14u;
		OBJECT(current_object)->projected_x -= x_shift;
		OBJECT(current_object)->speed_z = 0;
		if (OBJECT(current_object)->floor_type != 0xDu && OBJECT(current_object)->health <= 8)
		{
			sound_id = 0x2Fu;
			if (OBJECT(current_object)->kind != 0u)
			{
				sound_id = 0x37u;
				if (OBJECT(current_object)->kind != 1u)
					sound_id = 0x77u;
			}
			play_sound(sound_id, &sink_threshold, &write_index_address);
		}
		PLAYER(current_player)->substate++;
		break;
	case 1:
		break;
	default:
		unknown_code(0x3B56u + PLAYER(current_player)->substate * 3u);
	}
	OBJECT(current_object)->speed_z += 0x3000u;
	OBJECT(current_object)->z += OBJECT(current_object)->speed_z;
	if (OBJECT(current_object)->player_view.floor_block_high >= 0)
	{
		floor_block_record = floor_blocks + 2u + OBJECT(current_object)->floor_block;
		if (mem_read8(floor_block_record + 0x12u) == 0x22u)
		{
			sink_threshold = 0x1DC0000u;
			goto player_falling_off_edge_check_sinking;
		}
		if (mem_read8(floor_block_record + 0x12u) == 0x26u)
		{
			sink_threshold = 0x240000u;
		player_falling_off_edge_check_sinking:
			if ((int16_t)sink_threshold <= OBJECT(current_object)->z_int && OBJECT(current_object)->y_int < OBJECT(LINKED(current_object, group_leaders[0]))->y_int)
			{
				PLAYER(current_player)->previous_state = PLAYER(current_player)->state;
				PLAYER(current_player)->state = 0x33u;
				PLAYER(current_player)->substate = 0u;
			}
		}
	}
	if (OBJECT(current_object)->floor_type == 0xDu && OBJECT(current_object)->z_int >= 0x530)
	{
		PLAYER(current_player)->state = 0x20u;
		PLAYER(current_player)->substate = 0u;
	}
	if (OBJECT(current_object)->screen_y >= 0x100)
	{
		if (stage == 5u || (stage == 8u && scroll_position < 0x4000u))
			OBJECT(current_object)->handler = player_respawn_ADDRESS;
		else
		{
			health = OBJECT(current_object)->health;
			OBJECT(current_object)->health = health - 8u;
			if ((int8_t)health > 8)
			{
				OBJECT(current_object)->handler = player_respawn_ADDRESS;
				if ((int8_t)OBJECT(current_object)->animation_speed >= 0)
				{
					at_last_step = animate_and_move(0u);
					if ((uint8_t)at_last_step != 0u)
						OBJECT(current_object)->animation_speed = 0xFFu;
				}
				update_screen_position(&ignored, &sink_threshold);
				if (PLAYER(current_player)->ledge_side >= 2 || (OBJECT(current_object)->flags & 0x20000u) == 0u)
				{
					depth = PLAYER(current_player)->fall_draw_depth;
					queue_sprite_at_depth(&ignored, &depth);
					return;
				}
				depth = (uint16_t)(OBJECT(LINKED(current_object, OBJECT(current_object)->touched_by))->var_88 - 1u);
				queue_sprite_at_depth(&ignored, &depth);
				return;
			}
			OBJECT(current_object)->health = 0;
			PLAYER(current_player)->state = 0x16u;
			PLAYER(current_player)->substate = 0u;
		}
	}
	if ((int8_t)OBJECT(current_object)->animation_speed >= 0)
	{
		at_last_step = animate_and_move(0u);
		if ((uint8_t)at_last_step != 0u)
			OBJECT(current_object)->animation_speed = 0xFFu;
	}
	update_screen_position(&ignored, &sink_threshold);
	if (PLAYER(current_player)->ledge_side >= 2 || (OBJECT(current_object)->flags & 0x20000u) == 0u)
	{
		depth = PLAYER(current_player)->fall_draw_depth;
		queue_sprite_at_depth(&ignored, &depth);
		return;
	}
	depth = (uint16_t)(OBJECT(LINKED(current_object, OBJECT(current_object)->touched_by))->var_88 - 1u);
	queue_sprite_at_depth(&ignored, &depth);
}

void player_state_falling_off_edge_spare_return(void)
{
}

void player_state_over_edge(uint32_t jump_offset, uint32_t cursor)
{
	uint8_t new_status = 0;
	uint16_t margin_x = 0, margin_y = 0;
	uint32_t r1 = 0, r2 = 0, r7 = 0, animation = 0, at_last_step = 0, depth = 0, far_hang_animations = 0, ignored = 0, ledge_side = 0, object = 0, off_screen = 0, side = 0, slip_limit = 0;
	uint16_t state_timer = 0, state_timer_2 = 0;

	switch ((jump_offset & ~0xFFu) | PLAYER(current_player)->substate)
	{
	case 0:
	case 6:
		PLAYER(current_player)->substate++;
		get_ledge_side(&side, &cursor);
		PLAYER(current_player)->ledge_side = side;
		PLAYER(current_player)->fall_draw_depth = ((int32_t)((uint32_t)OBJECT(current_object)->projected_y + OBJECT(current_object)->previous_floor_z) >> 16) - view_y;
		if (OBJECT(current_object)->floor_type == 1u)
		{
			if (PLAYER(current_player)->ledge_side < 2)
				PLAYER(current_player)->fall_draw_depth = OBJECT(LINKED(current_object, OBJECT(current_object)->platform))->var_88 - 1u;
		player_over_edge_depth_minus_10:
			PLAYER(current_player)->fall_draw_depth -= 0x10u;
		}
		else if (PLAYER(current_player)->ledge_side < 2)
		{
			if (stage != 0xAu)
				goto player_over_edge_depth_minus_10;
			PLAYER(current_player)->fall_draw_depth -= 0x20u;
			goto player_over_edge_depth_minus_10;
		}
		set_speed_from_direction();
		OBJECT(current_object)->flags &= ~0x100u;
		if (OBJECT(current_object)->floor_type == 1u)
			OBJECT(current_object)->flags |= 0x100u;
		PLAYER(current_player)->heading = OBJECT(current_object)->direction;
		PLAYER(current_player)->ledge_pullback = 0;
		PLAYER(current_player)->slip_distance = 0;
		PLAYER(current_player)->control_lock = 0u;
		OBJECT(current_object)->flags &= ~0x20000u;
		PLAYER(current_player)->flags &= ~0x80u;
		OBJECT(current_object)->flags &= ~0x2000u;
	case 1:
		OBJECT(current_object)->x += OBJECT(current_object)->speed_x;
		OBJECT(current_object)->y += OBJECT(current_object)->speed_y;
		project_object(&ignored, &r1);
		if ((uint32_t)OBJECT(current_object)->speed == 0u)
		{
		player_over_edge_stop_sliding:
			PLAYER(current_player)->substate++;
		}
		else
		{
			PLAYER(current_player)->slip_distance += OBJECT(current_object)->speed;
			if ((PLAYER(current_player)->flags & 0x80u) != 0u)
				goto player_over_edge_stop_sliding;
			slip_limit = 0x20000000u;
			if (OBJECT(current_object)->floor_type == 9u || (OBJECT(current_object)->flags & 0x20000u) != 0u)
				slip_limit = (int32_t)slip_limit >> 2;
			if ((int32_t)slip_limit <= PLAYER(current_player)->slip_distance)
				goto player_over_edge_stop_sliding;
		}
		update_player_blink();
		if (PLAYER(current_player)->previous_state != 0xCu)
		{
			animation = OBJECT(current_object)->animation;
			if (((mem_read16(animation + 2u) | mem_read16(animation + 6u)) & ~0xC0u) == 0u)
				animate_with_flags(&r1, &at_last_step);
		}
		if ((OBJECT(current_object)->flags & 0x20000u) == 0u)
		{
			check_zone_contact();
			check_player_contacts(&r1, &cursor);
		}
		add_player_to_lists();
		if ((OBJECT(current_object)->flags & 0x80000u) != 0u)
			return;
		if (PLAYER(current_player)->ledge_side >= 2)
		{
			ignored = queue_sprite_at_previous_floor();
			return;
		}
		if ((PLAYER(current_player)->flags & 2u) != 0u || (OBJECT(current_object)->flags & 0x20000u) != 0u)
		{
			depth = (uint16_t)(OBJECT(LINKED(current_object, OBJECT(current_object)->touched_by))->var_88 + 1u);
			queue_sprite_at_depth(&ignored, &depth);
			return;
		}
		depth = PLAYER(current_player)->fall_draw_depth;
		queue_sprite_at_depth(&ignored, &depth);
		return;
	case 2:
		PLAYER(current_player)->substate++;
		OBJECT(current_object)->animation_speed = 1u;
		OBJECT(current_object)->animation_timer = 0;
		OBJECT(current_object)->animation_frame = 0xFFu;
		far_hang_animations = sonic_far_edge_hang_animations_ADDRESS;
		cursor = sonic_near_edge_hang_animations_ADDRESS;
		if (OBJECT(current_object)->kind != 0u)
		{
			far_hang_animations = ray_far_edge_hang_animations_ADDRESS;
			cursor = ray_near_edge_hang_animations_ADDRESS;
			if (OBJECT(current_object)->kind != 1u)
			{
				far_hang_animations = mighty_far_edge_hang_animations_ADDRESS;
				cursor = mighty_near_edge_hang_animations_ADDRESS;
			}
		}
		switch ((uint16_t)PLAYER(current_player)->ledge_side)
		{
		case 0:
		case 4:
			OBJECT(current_object)->animation = far_hang_animations;
			OBJECT(current_object)->sprite_flags &= 0xFFBFu;
			break;
		case 1:
			OBJECT(current_object)->animation = far_hang_animations;
			OBJECT(current_object)->sprite_flags |= 0x40u;
			break;
		case 2:
			OBJECT(current_object)->animation = cursor;
			OBJECT(current_object)->sprite_flags |= 0x40u;
			break;
		case 3:
			OBJECT(current_object)->animation = cursor;
			OBJECT(current_object)->sprite_flags &= 0xFFBFu;
			break;
		default:
			unknown_code(0x3FE9u + (uint16_t)PLAYER(current_player)->ledge_side * 2u);
		}
		PLAYER(current_player)->state_timer = 0x3C;
		PLAYER(current_player)->ledge_pullback = 0;
		OBJECT(current_object)->speed = 0;
		OBJECT(current_object)->speed_x = 0;
		OBJECT(current_object)->speed_y = 0;
	case 3:
		update_ledge_pullback();
		OBJECT(current_object)->speed = 0;
		if ((OBJECT(current_object)->flags & 0x100u) == 0u)
		{
		player_over_edge_check_pull_up:
			if (PLAYER(current_player)->ledge_pullback >= 0x14)
				PLAYER(current_player)->substate++;
			if ((PLAYER(current_player)->flags & 2u) != 0u || (OBJECT(current_object)->flags & 0x20000u) != 0u)
			{
				state_timer = PLAYER(current_player)->state_timer;
				PLAYER(current_player)->state_timer = state_timer - 1u;
				if ((int16_t)state_timer <= 1)
				{
					PLAYER(current_player)->state_timer = 0;
					if ((int16_t)OBJECT(LINKED(current_object, OBJECT(current_object)->touched_by))->sprite_scale >= 0x100)
					{
					player_over_edge_drop:
						PLAYER(current_player)->state = 8u;
						PLAYER(current_player)->substate = 0u;
						new_status = 0xFFu;
						if (OBJECT(current_object)->health <= 8)
						{
							new_status = 1u;
							OBJECT(current_object)->flags |= 0x400u;
						}
						player_status[OBJECT(current_object)->player] = new_status;
						PLAYER(current_player)->mishap_flag = 0xFFu;
					}
				}
			}
			else
			{
				state_timer_2 = PLAYER(current_player)->state_timer;
				PLAYER(current_player)->state_timer = state_timer_2 - 1u;
				if ((int16_t)state_timer_2 <= 1)
					goto player_over_edge_drop;
			}
		}
		else
		{
			object = LINKED(current_object, PLAYER(current_player)->edge_object);
			switch (OBJECT(object)->direction)
			{
			case 0:
			case 4:
				if (OBJECT(current_object)->y_int <= OBJECT(object)->y_int)
					goto player_over_edge_drop;
				goto player_over_edge_check_pull_up;
			case 1:
				if (OBJECT(object)->y_int <= OBJECT(current_object)->y_int)
					goto player_over_edge_drop;
				goto player_over_edge_check_pull_up;
			case 2:
				if (OBJECT(current_object)->x_int <= OBJECT(object)->x_int)
					goto player_over_edge_drop;
				goto player_over_edge_check_pull_up;
			case 3:
				if (OBJECT(object)->x_int <= OBJECT(current_object)->x_int)
					goto player_over_edge_drop;
				goto player_over_edge_check_pull_up;
			default:
				unknown_code(0x405Bu + OBJECT(object)->direction * 2u);
			}
		}
		animate(&r1, &r7, &at_last_step);
		update_player_blink();
		project_object(&ignored, &r1);
		if ((OBJECT(current_object)->flags & 0x20000u) == 0u)
		{
			check_zone_contact();
			check_player_contacts(&r1, &cursor);
		}
		add_player_to_lists();
		if ((OBJECT(current_object)->flags & 0x80000u) != 0u)
			return;
		if (PLAYER(current_player)->ledge_side >= 2)
		{
			ignored = queue_sprite_at_previous_floor();
			return;
		}
		if ((PLAYER(current_player)->flags & 2u) != 0u || (OBJECT(current_object)->flags & 0x20000u) != 0u)
		{
			depth = (uint16_t)(OBJECT(LINKED(current_object, OBJECT(current_object)->touched_by))->var_88 + 1u);
			queue_sprite_at_depth(&ignored, &depth);
			return;
		}
		depth = PLAYER(current_player)->fall_draw_depth;
		queue_sprite_at_depth(&ignored, &depth);
		return;
	case 4:
		OBJECT(current_object)->animation_speed = 2u;
		OBJECT(current_object)->animation_timer = 0;
		OBJECT(current_object)->animation_frame = 0xFFu;
		OBJECT(current_object)->animation += 0x16u;
		OBJECT(current_object)->speed = 0x4000000;
		OBJECT(current_object)->direction = (PLAYER(current_player)->heading + 0x200u) & 0x3FFu;
		set_speed_from_direction();
		jump_offset = 0x47u;
		if (OBJECT(current_object)->kind != 0u)
		{
			jump_offset = 0x4Fu;
			if (OBJECT(current_object)->kind != 1u)
				jump_offset = 0x67u;
		}
		play_sound((uint8_t)jump_offset, &r1, &r2);
		PLAYER(current_player)->substate++;
	case 5:
		find_floor(&jump_offset, &r1, &r2, &ignored, &cursor);
		if ((OBJECT(current_object)->flags & 0x100u) == 0u)
		{
		player_over_edge_check_on_floor:
			if ((OBJECT(current_object)->flags & 0x20000u) == 0u)
			{
				if (OBJECT(current_object)->floor_block >= 0 && OBJECT(current_object)->floor_type != 2u && (uint32_t)OBJECT(current_object)->previous_floor_block == (uint32_t)OBJECT(current_object)->floor_block)
				{
				player_over_edge_back_on_floor:
					PLAYER(current_player)->state = 0u;
					PLAYER(current_player)->substate = 0u;
					OBJECT(current_object)->flags &= ~0x20000u;
					ledge_side = (uint16_t)PLAYER(current_player)->ledge_side;
					PLAYER(current_player)->heading = ledge_recover_headings[ledge_side];
					PLAYER(current_player)->target_heading = ledge_recover_headings[ledge_side];
				}
			}
			else if ((OBJECT(current_object)->flags & 4u) == 0u)
				goto player_over_edge_back_on_floor;
		}
		else
		{
			object = LINKED(current_object, PLAYER(current_player)->edge_object);
			switch (OBJECT(object)->direction)
			{
			case 0:
			case 4:
				if (OBJECT(current_object)->y_int <= OBJECT(object)->y_int)
					goto player_over_edge_back_on_floor;
				goto player_over_edge_check_on_floor;
			case 1:
				if (OBJECT(object)->y_int <= OBJECT(current_object)->y_int)
					goto player_over_edge_back_on_floor;
				goto player_over_edge_check_on_floor;
			case 2:
				if (OBJECT(current_object)->x_int <= OBJECT(object)->x_int)
					goto player_over_edge_back_on_floor;
				goto player_over_edge_check_on_floor;
			case 3:
				if (OBJECT(object)->x_int <= OBJECT(current_object)->x_int)
					goto player_over_edge_back_on_floor;
				goto player_over_edge_check_on_floor;
			default:
				unknown_code(0x41EDu + OBJECT(object)->direction * 2u);
			}
		}
		if ((int8_t)OBJECT(current_object)->animation_speed >= 0)
		{
			animate(&r1, &r7, &at_last_step);
			if ((uint8_t)at_last_step != 0u)
				OBJECT(current_object)->animation_speed = 0xFFu;
		}
		update_player_blink();
		OBJECT(current_object)->x += OBJECT(current_object)->speed_x;
		OBJECT(current_object)->y += OBJECT(current_object)->speed_y;
		project_object(&ignored, &r1);
		check_zone_contact();
		check_player_contacts(&r1, &cursor);
		add_player_to_lists();
		margin_x = 0x50u;
		margin_y = 0x50u;
		off_screen = is_off_screen_margin(margin_x, margin_y);
		if (off_screen != 0u)
		{
			if (OBJECT(current_object)->health > 0)
				OBJECT(current_object)->handler = player_respawn_ADDRESS;
			else
			{
				OBJECT(current_object)->health = 0;
				PLAYER(current_player)->state = 0x16u;
				PLAYER(current_player)->substate = 0u;
			}
		}
		if ((OBJECT(current_object)->flags & 0x80000u) != 0u)
			return;
		ignored = queue_sprite_at_previous_floor();
		return;
	default:
		unknown_code(0x3E05u + ((jump_offset & ~0xFFu) | PLAYER(current_player)->substate) * 3u);
	}
}

void player_state_dropping(uint32_t cursor)
{
	uint8_t sound_id = 0;
	uint32_t r1 = 0, depth = 0, free_count = 0, ignored = 0, object = 0, write_index_address = 0;
	uint8_t health = 0;
	uint16_t state_timer = 0;

	switch (PLAYER(current_player)->substate)
	{
	case 0:
	case 4:
		OBJECT(current_object)->sprite_palette |= 0x8000u;
		set_fall_sprite_priority();
		OBJECT(current_object)->sprite_frame = mem_read16(OBJECT(current_object)->animation + 0x36u);
		PLAYER(current_player)->state_timer = 0x1E;
		PLAYER(current_player)->substate++;
		state_timer = PLAYER(current_player)->state_timer;
		PLAYER(current_player)->state_timer = state_timer - 1u;
		if ((int16_t)state_timer <= 1)
			PLAYER(current_player)->substate++;
		update_screen_position(&ignored, &r1);
		if (PLAYER(current_player)->ledge_side >= 2)
		{
			ignored = queue_sprite_at_previous_floor();
			return;
		}
		if ((PLAYER(current_player)->flags & 2u) != 0u)
		{
			depth = (uint16_t)(OBJECT(LINKED(current_object, OBJECT(current_object)->touched_by))->var_88 - 1u);
			queue_sprite_at_depth(&ignored, &depth);
			return;
		}
		if ((OBJECT(current_object)->flags & 0x20000u) == 0u)
		{
			depth = PLAYER(current_player)->fall_draw_depth;
			queue_sprite_at_depth(&ignored, &depth);
			return;
		}
		OBJECT(current_object)->sprite_palette &= 0x9FFFu;
		depth = OBJECT(LINKED(current_object, OBJECT(current_object)->touched_by))->var_88;
		queue_sprite_at_depth(&ignored, &depth);
		return;
	case 1:
		state_timer = PLAYER(current_player)->state_timer;
		PLAYER(current_player)->state_timer = state_timer - 1u;
		if ((int16_t)state_timer <= 1)
			PLAYER(current_player)->substate++;
		update_screen_position(&ignored, &r1);
		if (PLAYER(current_player)->ledge_side >= 2)
		{
			ignored = queue_sprite_at_previous_floor();
			return;
		}
		if ((PLAYER(current_player)->flags & 2u) != 0u)
		{
			depth = (uint16_t)(OBJECT(LINKED(current_object, OBJECT(current_object)->touched_by))->var_88 - 1u);
			queue_sprite_at_depth(&ignored, &depth);
			return;
		}
		if ((OBJECT(current_object)->flags & 0x20000u) == 0u)
		{
			depth = PLAYER(current_player)->fall_draw_depth;
			queue_sprite_at_depth(&ignored, &depth);
			return;
		}
		OBJECT(current_object)->sprite_palette &= 0x9FFFu;
		depth = OBJECT(LINKED(current_object, OBJECT(current_object)->touched_by))->var_88;
		queue_sprite_at_depth(&ignored, &depth);
		return;
	case 2:
		allocate_object(&free_count, &cursor);
		if (free_count == 0u)
			return;
		OBJECT(cursor)->sprite_flags = (OBJECT(current_object)->sprite_flags & 0xCFF0u) | 0x100Au;
		OBJECT(cursor)->sprite_palette = OBJECT(current_object)->sprite_palette;
		OBJECT(cursor)->animation_frame = 0u;
		OBJECT(cursor)->animation_speed = 3u;
		OBJECT(cursor)->animation_timer = 0;
		OBJECT(cursor)->animation = OBJECT(current_object)->animation;
		OBJECT(cursor)->animation += 0x22u;
		OBJECT(cursor)->direction = PLAYER(current_player)->ledge_side;
		OBJECT(cursor)->projected_x = OBJECT(current_object)->projected_x;
		OBJECT(cursor)->projected_x -= 0x10u;
		OBJECT(cursor)->projected_y = OBJECT(current_object)->projected_y;
		OBJECT(cursor)->projected_y -= 0x280000u;
		OBJECT(cursor)->z = OBJECT(current_object)->z;
		OBJECT(cursor)->sprite_offset_x = OBJECT(current_object)->sprite_offset_x;
		OBJECT(cursor)->sprite_offset_y = OBJECT(current_object)->sprite_offset_y;
		OBJECT(cursor)->handler = slip_effect_update_ADDRESS;
		OBJECT(current_object)->sprite_frame = mem_read16(OBJECT(current_object)->animation + 0x38u);
		OBJECT(current_object)->speed_y = 0;
		if (OBJECT(current_object)->kind == 2u)
			cursor = spawn_slip_piece(cursor);
		if (OBJECT(current_object)->kind == 1u)
			spawn_slip_pieces(cursor);
		sound_id = 0x16u;
		play_sound(sound_id, &r1, &write_index_address);
		if (OBJECT(current_object)->health <= 8)
		{
			sound_id = 0x2Fu;
			if (OBJECT(current_object)->kind != 0u)
			{
				sound_id = 0x37u;
				if (OBJECT(current_object)->kind != 1u)
					sound_id = 0x77u;
			}
			play_sound(sound_id, &r1, &write_index_address);
		}
		PLAYER(current_player)->substate++;
	case 3:
		OBJECT(current_object)->speed_y += 0x6000u;
		OBJECT(current_object)->projected_y += OBJECT(current_object)->speed_y;
		if (OBJECT(current_object)->screen_y >= 0x110)
		{
			if (stage == 5u || (stage == 8u && scroll_position < 0x4000u))
				OBJECT(current_object)->handler = player_respawn_ADDRESS;
			else
			{
				health = OBJECT(current_object)->health;
				OBJECT(current_object)->health = health - 8u;
				if ((int8_t)health > 8)
				{
					OBJECT(current_object)->handler = player_respawn_ADDRESS;
					update_screen_position(&ignored, &r1);
					if (PLAYER(current_player)->ledge_side >= 2)
					{
						ignored = queue_sprite_at_previous_floor();
						return;
					}
					if ((PLAYER(current_player)->flags & 2u) != 0u)
					{
						depth = (uint16_t)(OBJECT(LINKED(current_object, OBJECT(current_object)->touched_by))->var_88 - 1u);
						queue_sprite_at_depth(&ignored, &depth);
						return;
					}
					if ((OBJECT(current_object)->flags & 0x20000u) == 0u)
					{
						depth = PLAYER(current_player)->fall_draw_depth;
						queue_sprite_at_depth(&ignored, &depth);
						return;
					}
					object = LINKED(current_object, OBJECT(current_object)->touched_by);
					OBJECT(current_object)->sprite_palette &= 0x9FFFu;
					if ((int16_t)(OBJECT(object)->screen_y + 0x20u) <= OBJECT(current_object)->screen_y)
						OBJECT(current_object)->sprite_palette |= 0x2000u;
					depth = OBJECT(object)->var_88;
					queue_sprite_at_depth(&ignored, &depth);
					return;
				}
				OBJECT(current_object)->health = 0;
				PLAYER(current_player)->state = 0x16u;
				PLAYER(current_player)->substate = 0u;
			}
		}
		update_screen_position(&ignored, &r1);
		if (PLAYER(current_player)->ledge_side >= 2)
		{
			ignored = queue_sprite_at_previous_floor();
			return;
		}
		if ((PLAYER(current_player)->flags & 2u) != 0u)
		{
			depth = (uint16_t)(OBJECT(LINKED(current_object, OBJECT(current_object)->touched_by))->var_88 - 1u);
			queue_sprite_at_depth(&ignored, &depth);
			return;
		}
		if ((OBJECT(current_object)->flags & 0x20000u) == 0u)
		{
			depth = PLAYER(current_player)->fall_draw_depth;
			queue_sprite_at_depth(&ignored, &depth);
			return;
		}
		object = LINKED(current_object, OBJECT(current_object)->touched_by);
		OBJECT(current_object)->sprite_palette &= 0x9FFFu;
		if ((int16_t)(OBJECT(object)->screen_y + 0x20u) <= OBJECT(current_object)->screen_y)
			OBJECT(current_object)->sprite_palette |= 0x2000u;
		depth = OBJECT(object)->var_88;
		queue_sprite_at_depth(&ignored, &depth);
		return;
	default:
		unknown_code(0x4301u + PLAYER(current_player)->substate * 3u);
	}
}

void player_state_dropping_spare_return(void)
{
}

void player_state_falling_into_pit(void)
{
	uint32_t r8 = 0, depth = 0, floor_block_record = 0, ignored = 0, side = 0, sink_threshold = 0;
	uint8_t health = 0;

	switch (PLAYER(current_player)->substate)
	{
	case 0:
	case 2:
		get_ledge_side(&side, &ignored);
		PLAYER(current_player)->ledge_side = side;
		if ((uint16_t)PLAYER(current_player)->ledge_side == 4u)
			PLAYER(current_player)->ledge_direction = ((OBJECT(current_object)->direction + 0x80u) & 0x3FFu) >> 8;
		set_fall_sprite_priority();
		PLAYER(current_player)->fall_draw_depth = ((int32_t)((uint32_t)OBJECT(current_object)->projected_y + OBJECT(current_object)->previous_floor_z) >> 16) - view_y;
		floor_block_record = floor_blocks + 2u + OBJECT(current_object)->floor_block;
		if (mem_read8(floor_block_record + 0x12u) == 0x1Du)
		{
			if (OBJECT(current_object)->x < (int32_t)(((mem_read32(floor_block_record + 4u) - mem_read32(floor_block_record)) >> 1) + mem_read32(floor_block_record)))
				goto player_falling_into_pit_depth_minus_10;
			PLAYER(current_player)->fall_draw_depth -= 0x70u;
			goto player_falling_into_pit_depth_minus_10;
		}
		if ((uint16_t)PLAYER(current_player)->ledge_side != 4u)
		{
			if (PLAYER(current_player)->ledge_side < 2)
			{
			player_falling_into_pit_depth_at_platform:
				PLAYER(current_player)->fall_draw_depth = OBJECT(LINKED(current_object, OBJECT(current_object)->platform))->var_88 - 1u;
			}
		player_falling_into_pit_depth_minus_10:
			PLAYER(current_player)->fall_draw_depth -= 0x10u;
		}
		else if (OBJECT(current_object)->floor_attribute != 5u)
		{
			if (PLAYER(current_player)->ledge_direction >= 2)
				goto player_falling_into_pit_depth_minus_10;
			goto player_falling_into_pit_depth_at_platform;
		}
		PLAYER(current_player)->mishap_flag = 0xFFu;
		PLAYER(current_player)->substate++;
		break;
	case 1:
		break;
	default:
		unknown_code(0x4547u + PLAYER(current_player)->substate * 3u);
	}
	OBJECT(current_object)->speed_z += 0x6000u;
	OBJECT(current_object)->z += OBJECT(current_object)->speed_z;
	if (OBJECT(current_object)->player_view.floor_block_high >= 0)
	{
		floor_block_record = floor_blocks + 2u + OBJECT(current_object)->floor_block;
		if (mem_read8(floor_block_record + 0x12u) == 0x22u)
		{
			sink_threshold = 0x1DC0000u;
			goto player_falling_into_pit_check_sinking;
		}
		if (mem_read8(floor_block_record + 0x12u) == 0x26u)
		{
			sink_threshold = 0x240000u;
		player_falling_into_pit_check_sinking:
			if ((int16_t)sink_threshold <= OBJECT(current_object)->z_int && OBJECT(current_object)->y_int < OBJECT(LINKED(current_object, group_leaders[0]))->y_int)
			{
				PLAYER(current_player)->previous_state = PLAYER(current_player)->state;
				PLAYER(current_player)->state = 0x33u;
				PLAYER(current_player)->substate = 0u;
			}
		}
	}
	if (OBJECT(current_object)->floor_type == 0xDu && OBJECT(current_object)->z_int >= 0x530)
	{
		PLAYER(current_player)->state = 0x20u;
		PLAYER(current_player)->substate = 0u;
	}
	if (OBJECT(current_object)->screen_y >= 0x130)
	{
		health = OBJECT(current_object)->health;
		OBJECT(current_object)->health = health - 8u;
		if ((int8_t)health > 8)
			OBJECT(current_object)->handler = player_respawn_ADDRESS;
		else
		{
			OBJECT(current_object)->health = 0;
			PLAYER(current_player)->state = 0x16u;
			PLAYER(current_player)->substate = 0u;
		}
	}
	if ((int8_t)OBJECT(current_object)->animation_frame >= 0)
		animate_with_flags(&sink_threshold, &r8);
	OBJECT(current_object)->x += OBJECT(current_object)->speed_x;
	OBJECT(current_object)->y += OBJECT(current_object)->speed_y;
	AS8(temp_0) = OBJECT(current_object)->sprite_offset_y;
	OBJECT(current_object)->sprite_offset_y = 0x12;
	project_object(&ignored, &ignored);
	OBJECT(current_object)->sprite_offset_y = AS8(temp_0);
	depth = PLAYER(current_player)->fall_draw_depth;
	queue_sprite_at_depth(&ignored, &depth);
}

void player_state_falling_into_pit_spare_return(void)
{
}

void player_state_climbing(uint32_t depth, uint32_t cursor)
{
	uint16_t climb_speed = 0, new_heading = 0, side_threshold = 0, wall_limit = 0, wall_start = 0;
	uint32_t r7 = 0, across_wall = 0, along_wall = 0, at_last_step = 0, attribute_record = 0, climb_animation = 0, climb_over_animation = 0, climb_sideways_animation = 0, floor_block_record = 0, ignored = 0, jump_offset = 0, screen_shift = 0, side_step = 0, step_target = 0, step_x = 0, world_x = 0, world_y = 0, z_step = 0;

	for (;;)
	{
		check_zone_contact();
		check_player_contacts(&depth, &cursor);
		check_player_stalled();
		if (PLAYER(current_player)->state == 0x1Fu || PLAYER(current_player)->state == 0xDu || PLAYER(current_player)->state == 0x36u || PLAYER(current_player)->state == 0x20u)
		{
		player_dispatch_state:
			jump_offset = PLAYER(current_player)->state * 3u;
			switch (PLAYER(current_player)->state)
			{
			case 0:
				player_move(depth, cursor);
				return;
			case 1:
				player_move(depth, cursor);
				return;
			case 2:
				player_move(depth, cursor);
				return;
			case 3:
				player_state_inactive();
				return;
			case 4:
				player_move(depth, cursor);
				return;
			case 5:
				player_state_teetering(cursor);
				return;
			case 6:
				player_state_falling_off_edge();
				return;
			case 7:
				player_state_over_edge(jump_offset, cursor);
				return;
			case 8:
				player_state_dropping(cursor);
				return;
			case 9:
				player_state_falling_into_pit();
				return;
			case 10:
				continue;
			case 11:
				player_state_not_playing();
				return;
			case 12:
				player_move(depth, cursor);
				return;
			case 13:
				player_state_hurt(depth, cursor);
				return;
			case 14:
				player_state_bouncing();
				return;
			case 15:
				player_state_launched();
				return;
			case 16:
				player_state_launched();
				return;
			case 17:
				player_state_launched();
				return;
			case 18:
				player_state_launched();
				return;
			case 19:
				player_move(depth, cursor);
				return;
			case 20:
				player_move(depth, cursor);
				return;
			case 21:
				player_state_held(depth, cursor);
				return;
			case 22:
				player_state_dying();
				return;
			case 23:
				player_move(depth, cursor);
				return;
			case 24:
				player_state_stage_scene(depth, cursor);
				return;
			case 25:
				player_state_scene_thrown(depth, cursor);
				return;
			case 26:
				player_state_inactive_2();
				return;
			case 27:
				player_state_short_animation();
				return;
			case 28:
				player_state_opening_scene();
				return;
			case 29:
				player_state_stuck(cursor);
				return;
			case 30:
				player_state_intro_turn(depth, cursor);
				return;
			case 31:
				player_state_blown_away(cursor);
				return;
			case 32:
				player_state_tumbling(depth, cursor);
				return;
			case 33:
				player_state_stage_exit(depth, cursor);
				return;
			case 34:
				player_state_in_zone(depth, cursor);
				return;
			case 35:
				player_state_continue();
				return;
			case 36:
				player_state_out_of_game();
				return;
			case 37:
				player_state_floor_8_sequence(cursor);
				return;
			case 38:
				player_state_pulled(cursor);
				return;
			case 39:
				player_state_climbing_object(cursor);
				return;
			case 40:
				player_state_swept_away();
				return;
			case 41:
				player_state_tripped(cursor);
				return;
			case 42:
				player_state_entering_stage(depth, cursor);
				return;
			case 43:
				player_state_final_stage();
				return;
			case 44:
				player_state_stuck_on_object(depth, cursor);
				return;
			case 45:
				player_state_held_still();
				return;
			case 46:
				player_state_game_over(jump_offset);
				return;
			case 47:
				player_state_stunned(depth, cursor);
				return;
			case 48:
				player_state_caught(cursor);
				return;
			case 49:
				player_state_carried_off(depth, cursor);
				return;
			case 50:
				player_state_trapped(depth, cursor);
				return;
			case 51:
				player_state_sinking(cursor);
				return;
			case 52:
				player_state_choosing_character(cursor);
				return;
			case 53:
				player_state_riding();
				return;
			case 54:
				player_state_knocked_off(depth, cursor);
				return;
			case 55:
				player_state_not_playing();
				return;
			case 56:
				player_state_stage_b_sequence(depth, cursor);
				return;
			case 57:
				player_state_you_failed();
				return;
			case 58:
				player_move(depth, cursor);
				return;
			default:
				unknown_code(0x2D3Eu + PLAYER(current_player)->state * 3u);
			}
		}
		else
		{
			PLAYER(current_player)->state = 0xAu;
			attribute_record = OBJECT(current_object)->floor_attribute + floor_types_ADDRESS;
			update_player_blink();
			if ((OBJECT(current_object)->flags & 4u) != 0u && OBJECT(LINKED(current_object, OBJECT(current_object)->touched_by))->kind == 5u)
				goto player_climbing_update_position;
			switch (PLAYER(current_player)->substate)
			{
			case 0:
			case 5:
				PLAYER(current_player)->climb_from_top = 0u;
				if (OBJECT(current_object)->z_int <= (int16_t)ROM16(attribute_record + 8u))
					PLAYER(current_player)->climb_from_top = 0xFFu;
				if (PLAYER(current_player)->climb_from_top == 0u || PLAYER(current_player)->previous_state != 4u)
					goto player_climbing_start;
				PLAYER(current_player)->state = 9u;
				PLAYER(current_player)->substate = 0u;
				goto player_dispatch_state;
			case 1:
				goto player_climbing_grab_wall;
			case 2:
				goto player_climbing_descend_over_edge;
			case 3:
				goto player_climbing_move;
			case 4:
				goto player_climbing_pull_up;
			default:
				unknown_code(0x4755u + PLAYER(current_player)->substate * 3u);
			}
		}
	}
player_climbing_start:
	OBJECT(current_object)->flags &= ~0x40u;
	OBJECT(current_object)->sprite_palette = mem_read16(OBJECT(current_object)->player_view.palette_list + 2u);
	OBJECT(current_object)->flags &= ~0x2000u;
	OBJECT(current_object)->speed = 0;
	PLAYER(current_player)->control_lock = 0u;
	PLAYER(current_player)->substate++;
player_climbing_grab_wall:
	along_wall = current_object + 0x50u;
	if (ROM8(attribute_record) != 0u)
		along_wall = current_object + 0x4Cu;
	wall_limit = ROM16(attribute_record + 0xAu);
	if ((int16_t)mem_read16(along_wall + 2u) < (int16_t)wall_limit)
	{
	player_climbing_clamp_to_wall:
		mem_write16(along_wall, 0u);
		mem_write16(along_wall + 2u, wall_limit);
	}
	else
	{
		wall_limit += ROM16(attribute_record + 0xCu);
		if ((int16_t)wall_limit < (int16_t)mem_read16(along_wall + 2u))
			goto player_climbing_clamp_to_wall;
	}
	if (PLAYER(current_player)->climb_from_top == 0u)
	{
		PLAYER(current_player)->climb_remaining = ROM16(attribute_record + 6u);
		OBJECT(current_object)->flags &= ~0x80u;
		climb_animation = sonic_climb_animation_ADDRESS;
		if (OBJECT(current_object)->kind != 0u)
		{
			climb_animation = ray_climb_animation_ADDRESS;
			if (OBJECT(current_object)->kind != 1u)
				climb_animation = mighty_climb_animation_ADDRESS;
		}
		OBJECT(current_object)->animation = climb_animation;
		OBJECT(current_object)->sprite_flags |= 0x40u;
		if (ROM8(attribute_record) != 0u)
			OBJECT(current_object)->sprite_flags &= 0xFFBFu;
		OBJECT(current_object)->animation_timer = 0;
		OBJECT(current_object)->animation_speed = 3u;
		OBJECT(current_object)->animation_frame = 0xFFu;
		floor_block_record = floor_blocks + 2u + OBJECT(current_object)->floor_block;
		if (ROM8(attribute_record) == 0u)
			OBJECT(current_object)->x = mem_read32(floor_block_record);
		else
			OBJECT(current_object)->y = mem_read32(floor_block_record + 0xCu);
		OBJECT(current_object)->z = OBJECT(current_object)->floor_z;
		PLAYER(current_player)->climb_base_z = OBJECT(current_object)->floor_z;
		project_object(&ignored, &depth);
		PLAYER(current_player)->screen_x_int = OBJECT(current_object)->projected_x;
		AS16(PLAYER(current_player)->screen_x) = 0u;
	player_climbing_begin_climb:
		PLAYER(current_player)->step_accumulator = 0;
		PLAYER(current_player)->substate = 3u;
	player_climbing_move:
		if ((int8_t)OBJECT(current_object)->animation_speed >= 0)
		{
		player_climbing_check_vertical_step:
			if ((int16_t)AT16(PLAYER(current_player)->step_accumulator, 2) >= 4)
			{
				OBJECT(current_object)->flags |= 0x80u;
				goto player_climbing_start_vertical_step;
			}
			if ((int16_t)AT16(PLAYER(current_player)->step_accumulator, 2) <= -4)
			{
				OBJECT(current_object)->flags &= ~0x80u;
			player_climbing_start_vertical_step:
				PLAYER(current_player)->step_accumulator = 0;
				PLAYER(current_player)->side_step_accumulator = 0;
				climb_animation = sonic_climb_animation_ADDRESS;
				if (OBJECT(current_object)->kind != 0u)
				{
					climb_animation = ray_climb_animation_ADDRESS;
					if (OBJECT(current_object)->kind != 1u)
						climb_animation = mighty_climb_animation_ADDRESS;
				}
				if (OBJECT(current_object)->animation != climb_animation)
				{
					OBJECT(current_object)->animation = climb_animation;
					OBJECT(current_object)->animation_frame = 0u;
				}
				OBJECT(current_object)->animation_speed = 4u;
				OBJECT(current_object)->animation_timer = 0;
				depth = 0x180000u;
				if ((OBJECT(current_object)->flags & 0x80u) == 0u)
					depth = 0u - depth;
				step_target = depth + OBJECT(current_object)->z;
				if (PLAYER(current_player)->climb_base_z < (int32_t)step_target)
				{
				player_climbing_block_step:
					OBJECT(current_object)->animation_speed = 0xFFu;
					PLAYER(current_player)->step_accumulator = 0;
				}
				else
				{
					step_target = (uint16_t)(((int32_t)(step_target + OBJECT(current_object)->projected_y) >> 16) - view_y);
					if ((OBJECT(current_object)->flags & 0x80u) != 0u)
					{
						if ((int16_t)step_target >= 0xD0)
							goto player_climbing_block_step;
					}
					else if ((int16_t)step_target <= 0x10)
						goto player_climbing_block_step;
				}
			}
			if ((int8_t)OBJECT(current_object)->animation_speed >= 0)
			{
				climb_speed = (uint16_t)OBJECT(current_object)->speed_y_int;
				if ((int16_t)climb_speed < 0)
					climb_speed = (uint16_t)(0u - climb_speed);
				if ((int16_t)climb_speed > 3)
					climb_speed = 3u;
				depth = (depth & ~0xFFFFu) | 4u;
				depth = (depth & ~0xFFFFu) | (uint16_t)(depth - climb_speed);
				if (PLAYER(current_player)->fast_climb != 0u)
					depth = (depth & ~0xFFFFu) | (uint16_t)(depth << 2);
				OBJECT(current_object)->animation_speed = depth;
				animate(&depth, &r7, &at_last_step);
				if (OBJECT(current_object)->animation_speed == (uint8_t)OBJECT(current_object)->animation_timer)
				{
					if (OBJECT(current_object)->animation_frame != 6u)
					{
						if (OBJECT(current_object)->animation_frame == 0u)
							OBJECT(current_object)->animation_speed = 0xFFu;
					}
					else
						OBJECT(current_object)->animation_speed = 0xFFu;
					PLAYER(current_player)->previous_z = OBJECT(current_object)->z;
					z_step = 2u;
					if ((OBJECT(current_object)->flags & 0x80u) == 0u)
						z_step = 0u - z_step;
					OBJECT(current_object)->z_int += z_step;
					PLAYER(current_player)->saved_climb_remaining = PLAYER(current_player)->climb_remaining;
					PLAYER(current_player)->climb_remaining += z_step;
					PLAYER(current_player)->saved_screen_x = PLAYER(current_player)->screen_x;
					screen_shift = (int16_t)ROM16(attribute_record + 4u) * z_step;
					depth = screen_shift;
					if (ROM8(attribute_record) == 0u)
						depth = 0u - depth;
					PLAYER(current_player)->screen_x += depth;
					OBJECT(current_object)->projected_x = PLAYER(current_player)->screen_x_int;
					PLAYER(current_player)->saved_screen_y = OBJECT(current_object)->projected_y;
					OBJECT(current_object)->projected_y += (int32_t)screen_shift >> 1;
				}
			}
		}
		else
		{
			depth = set_climb_speed_from_trackball();
			if ((uint8_t)PLAYER(current_player)->pushed_out != 0u)
			{
				OBJECT(current_object)->direction = 0u;
				OBJECT(current_object)->speed = 0x3000000;
				depth = set_speed_from_direction();
			}
			PLAYER(current_player)->fast_climb = 0u;
			if (mem_read8(floor_blocks + 2u + OBJECT(current_object)->floor_block + 0x12u) == 0x20u && OBJECT(current_object)->z_int > 0x4DE)
				PLAYER(current_player)->fast_climb = 0xFFu;
			PLAYER(current_player)->target_heading = OBJECT(current_object)->direction;
			if ((uint16_t)((OBJECT(current_object)->direction & 0x1FFu) - 0x80u) >= 0x100u)
			{
				if ((int16_t)AT16(PLAYER(current_player)->step_accumulator, 2) < 0)
				{
					if (OBJECT(current_object)->speed_y_int >= 0)
					{
					player_climbing_reset_vertical_input:
						PLAYER(current_player)->step_accumulator = 0;
					}
				}
				else if (OBJECT(current_object)->speed_y_int < 0)
					goto player_climbing_reset_vertical_input;
				PLAYER(current_player)->step_accumulator += OBJECT(current_object)->speed_y;
				goto player_climbing_check_vertical_step;
			}
			if ((int16_t)AT16(PLAYER(current_player)->side_step_accumulator, 2) < 0)
			{
				if (OBJECT(current_object)->speed_x_int >= 0)
				{
				player_climbing_reset_side_input:
					PLAYER(current_player)->side_step_accumulator = 0;
				}
			}
			else if (OBJECT(current_object)->speed_x_int < 0)
				goto player_climbing_reset_side_input;
			PLAYER(current_player)->side_step_accumulator += OBJECT(current_object)->speed_x;
			side_threshold = 2u;
			if (PLAYER(current_player)->fast_climb != 0u)
				side_threshold = (uint16_t)((uint32_t)side_threshold << 2);
			if ((int16_t)side_threshold <= (int16_t)AT16(PLAYER(current_player)->side_step_accumulator, 2))
			{
				OBJECT(current_object)->flags &= ~0x80u;
				goto player_climbing_side_step;
			}
			if ((int16_t)AT16(PLAYER(current_player)->side_step_accumulator, 2) <= (int16_t)(0u - side_threshold))
			{
				OBJECT(current_object)->flags |= 0x80u;
			player_climbing_side_step:
				PLAYER(current_player)->side_step_accumulator = 0;
				PLAYER(current_player)->step_accumulator = 0;
				climb_sideways_animation = sonic_climb_sideways_animation_ADDRESS;
				if (OBJECT(current_object)->kind != 0u)
				{
					climb_sideways_animation = ray_climb_sideways_animation_ADDRESS;
					if (OBJECT(current_object)->kind != 1u)
						climb_sideways_animation = mighty_climb_sideways_animation_ADDRESS;
				}
				if (OBJECT(current_object)->animation != climb_sideways_animation)
				{
					OBJECT(current_object)->animation = climb_sideways_animation;
					OBJECT(current_object)->animation_frame = 0u;
				}
				side_step = 0x20000u;
				if ((OBJECT(current_object)->flags & 0x80u) != 0u)
					side_step = 0u - side_step;
				step_x = PLAYER(current_player)->screen_x + side_step;
				AS32(temp_0) = step_x;
				side_step = (int32_t)side_step >> 1;
				if (ROM8(attribute_record) != 0u)
					side_step = 0u - side_step;
				side_step += OBJECT(current_object)->projected_y;
				AT32(temp_1, 2) = side_step;
				side_step = side_step * 2u;
				unproject_point(&step_x, &side_step);
				if (ROM8(attribute_record) != 0u)
					side_step = step_x;
				side_step = (int32_t)side_step >> 16;
				wall_start = ROM16(attribute_record + 0xAu);
				if ((int16_t)wall_start <= (int16_t)side_step && (int16_t)side_step <= (int16_t)(wall_start + ROM16(attribute_record + 0xCu)) && (uint16_t)(((int32_t)(AT32(temp_1, 2) + OBJECT(current_object)->z + depth) >> 16) - view_y - 0x10u) < 0xC0u)
				{
					PLAYER(current_player)->saved_screen_x = PLAYER(current_player)->screen_x;
					PLAYER(current_player)->saved_screen_y = OBJECT(current_object)->projected_y;
					PLAYER(current_player)->screen_x = AS32(temp_0);
					OBJECT(current_object)->projected_x = PLAYER(current_player)->screen_x_int;
					OBJECT(current_object)->projected_y = AT32(temp_1, 2);
					animate_step(&depth, &r7, &step_x);
				}
			}
		}
	player_climbing_update_position:
		if (PLAYER(current_player)->climb_remaining > 0)
		{
			if ((int16_t)ROM16(attribute_record + 6u) <= PLAYER(current_player)->climb_remaining)
			{
				across_wall = current_object + 0x4Eu;
				if (ROM8(attribute_record) != 0u)
					across_wall = current_object + 0x52u;
				mem_write16(across_wall, mem_read16(across_wall) - 2u);
				PLAYER(current_player)->substate = 0u;
				PLAYER(current_player)->state = 0u;
				PLAYER(current_player)->free_turn_timer = 3u;
				OBJECT(current_object)->flags &= ~0x80u;
			}
			update_screen_position(&ignored, &depth);
			world_x = PLAYER(current_player)->screen_x;
			world_y = OBJECT(current_object)->projected_y * 2u;
			unproject_point(&world_x, &world_y);
			OBJECT(current_object)->x = world_x;
			OBJECT(current_object)->y = world_y;
			add_player_to_lists();
			if (mem_read8(floor_blocks + 2u + OBJECT(current_object)->floor_block + 0x12u) == 0x20u)
			{
				OBJECT(current_object)->sprite_palette &= 0x1FFFu;
				OBJECT(current_object)->sprite_palette |= 0x8000u;
				if (OBJECT(current_object)->z_int >= 0x4B7)
				{
					OBJECT(current_object)->sprite_palette &= 0x1FFFu;
					OBJECT(current_object)->sprite_palette |= 0x4000u;
				}
				PLAYER(current_player)->splash_zone = 0u;
				if (OBJECT(current_object)->z_int >= 0x4C7 && OBJECT(current_object)->z_int <= 0x4E9)
				{
					PLAYER(current_player)->splash_zone = 0xFFu;
					if (PLAYER(current_player)->was_in_splash_zone == 0u)
						spawn_climb_zone_effect(cursor);
				}
				PLAYER(current_player)->was_in_splash_zone = PLAYER(current_player)->splash_zone;
			}
			if ((OBJECT(current_object)->flags & 0x80000u) != 0u)
				return;
			depth = queue_sprite(depth);
			return;
		}
		climb_over_animation = sonic_climb_over_animation_ADDRESS;
		if (OBJECT(current_object)->kind != 0u)
		{
			climb_over_animation = ray_climb_over_animation_ADDRESS;
			if (OBJECT(current_object)->kind != 1u)
				climb_over_animation = mighty_climb_over_animation_ADDRESS;
		}
		OBJECT(current_object)->animation = climb_over_animation;
		OBJECT(current_object)->animation_frame = 0xFFu;
		OBJECT(current_object)->animation_timer = 0;
		OBJECT(current_object)->animation_speed = 3u;
		OBJECT(current_object)->anchor_x = OBJECT(current_object)->projected_x;
		OBJECT(current_object)->anchor_y = OBJECT(current_object)->projected_y_int;
		PLAYER(current_player)->substate++;
	player_climbing_pull_up:
		at_last_step = animate_around_anchor();
		AS8(temp_0) = at_last_step;
		update_screen_position(&ignored, &depth);
		if (AS8(temp_0) != 0u)
		{
			OBJECT(current_object)->z_int = ROM16(attribute_record + 8u);
			OBJECT(current_object)->z_fraction = 0u;
			OBJECT(current_object)->floor_z = OBJECT(current_object)->z;
			OBJECT(current_object)->flags &= ~0x80u;
			PLAYER(current_player)->state = 0u;
			PLAYER(current_player)->free_turn_timer = 3u;
			PLAYER(current_player)->substate = 0u;
			OBJECT(current_object)->speed = 0;
			OBJECT(current_object)->speed_x = 0;
			OBJECT(current_object)->speed_y = 0;
			PLAYER(current_player)->animation_speed = 0;
			floor_block_record = floor_blocks + 2u + OBJECT(current_object)->floor_block;
			if (ROM8(attribute_record) == 0u)
			{
				OBJECT(current_object)->x = mem_read32(floor_block_record + 4u);
				OBJECT(current_object)->x_int += 0xAu;
			}
			else
			{
				OBJECT(current_object)->y = mem_read32(floor_block_record + 8u);
				OBJECT(current_object)->y_int -= 0xAu;
			}
			new_heading = 0x80u;
			if (ROM8(attribute_record) != 0u)
				new_heading = 0x380u;
			PLAYER(current_player)->target_heading = new_heading;
			turn_player();
		}
		add_player_to_lists();
		if ((OBJECT(current_object)->flags & 0x80000u) != 0u)
			return;
		depth = queue_sprite(depth);
		return;
	}
	OBJECT(current_object)->x = mem_read32(floor_blocks + 2u + OBJECT(current_object)->floor_block + 4u);
	project_object(&ignored, &depth);
	OBJECT(current_object)->projected_x -= 0x18u;
	OBJECT(current_object)->projected_y_int -= 0x1Cu;
	OBJECT(current_object)->z_int += 0x14u;
	OBJECT(current_object)->anchor_x = OBJECT(current_object)->projected_x;
	OBJECT(current_object)->anchor_x += 0xDu;
	OBJECT(current_object)->anchor_y = OBJECT(current_object)->projected_y_int;
	OBJECT(current_object)->anchor_y += 0x1Eu;
	climb_over_animation = sonic_climb_over_animation_ADDRESS;
	if (OBJECT(current_object)->kind != 0u)
	{
		climb_over_animation = ray_climb_over_animation_ADDRESS;
		if (OBJECT(current_object)->kind != 1u)
			climb_over_animation = mighty_climb_over_animation_ADDRESS;
	}
	OBJECT(current_object)->animation = climb_over_animation;
	OBJECT(current_object)->sprite_flags |= 0x40u;
	OBJECT(current_object)->animation_frame = 0u;
	OBJECT(current_object)->animation_timer = 0;
	OBJECT(current_object)->animation_speed = 3u;
	PLAYER(current_player)->climb_remaining = 0;
	OBJECT(current_object)->flags |= 0x80u;
	PLAYER(current_player)->substate++;
player_climbing_descend_over_edge:
	at_last_step = animate_around_anchor();
	if ((uint8_t)at_last_step == 0u)
	{
		update_screen_position(&ignored, &depth);
		add_player_to_lists();
		if ((OBJECT(current_object)->flags & 0x80000u) != 0u)
			return;
		depth = queue_sprite(depth);
		return;
	}
	climb_animation = sonic_climb_animation_ADDRESS;
	if (OBJECT(current_object)->kind != 0u)
	{
		climb_animation = ray_climb_animation_ADDRESS;
		if (OBJECT(current_object)->kind != 1u)
			climb_animation = mighty_climb_animation_ADDRESS;
	}
	OBJECT(current_object)->animation = climb_animation;
	OBJECT(current_object)->animation_timer = 0;
	OBJECT(current_object)->animation_speed = 3u;
	OBJECT(current_object)->animation_frame = 6u;
	PLAYER(current_player)->screen_x_int = OBJECT(current_object)->anchor_x;
	AS16(PLAYER(current_player)->screen_x) = 0u;
	OBJECT(current_object)->projected_y_int = OBJECT(current_object)->anchor_y;
	OBJECT(current_object)->projected_y_fraction = 0u;
	goto player_climbing_begin_climb;
}
