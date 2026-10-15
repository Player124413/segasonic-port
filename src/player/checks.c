/**
 *    Name:        player/checks.c
 *    Description: the checks player_update and player_move run: floor types, zones, the goal, scene states, continuing and joining with credits, climbs, slip pieces, the autopilot path
 *    Version:     0.1.0
 *    Created:     12-10-2026
 *    Modified:    15-10-2026
 *    Author:      Sebastian Toyos
 *
 *    SegaSonic The Hedgehog (arcade, Sega System 32): the game's V60 program decompiled into C by
 *    tools/decompile.py, with the names of symbols.txt.
 */
#include "game.h"

void set_floor_sprite_priority(void)
{
	if (OBJECT(current_object)->floor_type == 4u)
	{
		OBJECT(current_object)->sprite_palette &= 0x1FFFu;
		OBJECT(current_object)->sprite_palette |= 0x4000u;
	}
	if (ROM8(floor_types_ADDRESS + mem_read8(floor_blocks + 2u + OBJECT(current_object)->previous_floor_block + 0x12u) * 4u) == 0xCu && OBJECT(current_object)->floor_type == 0xCu)
	{
		OBJECT(current_object)->sprite_palette &= 0x1FFFu;
		OBJECT(current_object)->sprite_palette |= 0x2000u;
		OBJECT(current_object)->sprite_palette |= 0x8000u;
	}
}

uint32_t update_quicksand_object(uint32_t sink_offset)
{
	uint8_t health = 0;

	if ((OBJECT(current_object)->flags & 0x80000000u) != 0u)
	{
		sink_offset = 0x4EC4u * (uint16_t)(0x68u - OBJECT(current_object)->touch_distance);
		OBJECT(current_object)->floor_z += sink_offset;
		OBJECT(current_object)->z = OBJECT(current_object)->floor_z;
		if ((PLAYER(current_player)->flags & 4u) != 0u)
		{
			PLAYER(current_player)->sink_depth += 0x33u;
			OBJECT(current_object)->z += (uint32_t)(uint16_t)PLAYER(current_player)->sink_depth << 8;
			if (PLAYER(current_player)->sink_depth < 0x2000)
				return sink_offset;
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
	}
	return sink_offset;
}

uint32_t update_sinking_floor(uint32_t sink_offset)
{
	uint8_t health = 0;

	if (OBJECT(current_object)->floor_type != 0xAu)
		return sink_offset;
	if (OBJECT(current_object)->floor_block >= 0)
	{
		PLAYER(current_player)->sink_depth += 0x33u;
		PLAYER(current_player)->sink_depth -= PLAYER(current_player)->struggle_speed >> 4;
		if (PLAYER(current_player)->sink_depth < 0x100)
			PLAYER(current_player)->sink_depth = 0x100;
		if (sink_depth_limits[OBJECT(current_object)->floor_attribute] < PLAYER(current_player)->sink_depth)
			PLAYER(current_player)->sink_depth -= 0xA0u;
		sink_offset = (uint32_t)(uint16_t)PLAYER(current_player)->sink_depth << 8;
		OBJECT(current_object)->z = OBJECT(current_object)->floor_z + sink_offset;
	}
	if (PLAYER(current_player)->sink_depth < 0x2000)
		return sink_offset;
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
	return sink_offset;
}

void apply_moving_floor(void)
{
	if (OBJECT(current_object)->floor_type == 7u && ROM8(floor_attribute_slopes_ADDRESS + OBJECT(current_object)->floor_attribute) == 1u)
		OBJECT(current_object)->x += OBJECT(LINKED(current_object, moving_floor_object))->speed_x;
	if (PLAYER(current_player)->state != 4u && (PLAYER(current_player)->state != 5u || ROM8(floor_types_ADDRESS + mem_read8(floor_blocks + 2u + OBJECT(current_object)->previous_floor_block + 0x12u) * 4u) == 0xCu) && OBJECT(current_object)->floor_type == 0xCu)
		OBJECT(current_object)->y += OBJECT(LINKED(current_object, OBJECT(current_object)->platform))->speed_y;
}

void check_zone_contact(void)
{
	uint32_t object = 0;

	if ((OBJECT(current_object)->flags & 0x8000000u) == 0u)
		return;
	object = (current_object & ~0xFFFFu) | (uint16_t)OBJECT(current_object)->anchor_x;
	if (OBJECT(object)->kind != 0x16u)
	{
		if (OBJECT(object)->kind == 0x18u || OBJECT(object)->kind == 0xCu || OBJECT(object)->kind == 0x1Eu || OBJECT(object)->kind == 9u || OBJECT(object)->kind == 0x21u || OBJECT(object)->kind == 0x14u)
		{
			OBJECT(current_object)->flags |= 4u;
			OBJECT(current_object)->touched_by = OBJECT(current_object)->anchor_x;
		}
	}
	else
	{
		PLAYER(current_player)->state = 0x22u;
		PLAYER(current_player)->substate = 0u;
	}
}

void check_stage_entry_path(void)
{
	if (PLAYER(current_player)->enter_on_respawn == 0u)
		return;
	if (stage_player_entries[stage] != 0u)
	{
		PLAYER(current_player)->previous_state = PLAYER(current_player)->state;
		PLAYER(current_player)->state = 0x2Au;
		return;
	}
	PLAYER(current_player)->enter_on_respawn = 0u;
}

void load_stage_goal(void)
{
	uint32_t goal_record = 0;

	goal_record = stage_goals[stage] + stage_goals_ADDRESS;
	goal_scroll_position = ROM16(goal_record);
	goal_record += 2u;
	goal_direction = ROM16(goal_record);
	goal_record += 2u;
	goal_line = ROM16(goal_record);
	goal_record = ((goal_record + 2u) & ~0xFFFFu) | (uint16_t)(goal_record + 2u + 2u);
	goal_line_min = mem_read16(goal_record);
	goal_line_max = mem_read16(goal_record + 2u);
}

void check_player_reached_goal(void)
{
	uint32_t object = 0;

	if ((OBJECT(current_object)->flags & 0x400u) != 0u || (uint8_t)OBJECT(current_object)->health == 0u || PLAYER(current_player)->state == 9u || PLAYER(current_player)->state == 0x24u || (uint16_t)OBJECT(current_object)->floor_z_int == 0x800u || current_game_mode == 0x20u || PLAYER(current_player)->state == 0x28u)
		return;
	switch (AS8(stage))
	{
	case 0:
		PLAYER(current_player)->previous_state = PLAYER(current_player)->state;
		PLAYER(current_player)->state = 0x18u;
		return;
	case 1:
	case 2:
	case 3:
	case 6:
	case 7:
	case 8:
	case 9:
	case 10:
	case 11:
	case 12:
	case 13:
	case 14:
	case 15:
	case 16:
	case 17:
	case 18:
	case 19:
		if (scroll_position < goal_scroll_position)
			return;
		switch (goal_direction)
		{
		case 0:
		case 4:
			if (((uint16_t)goal_line_min == 0u || (goal_line_min <= OBJECT(current_object)->x_int && OBJECT(current_object)->x_int <= goal_line_max)) && OBJECT(current_object)->y_int <= goal_line)
			{
				PLAYER(current_player)->previous_state = PLAYER(current_player)->state;
				PLAYER(current_player)->state = 0x21u;
			}
			break;
		case 1:
			if (((uint16_t)goal_line_min == 0u || (goal_line_min <= OBJECT(current_object)->y_int && OBJECT(current_object)->y_int <= goal_line_max)) && goal_line <= OBJECT(current_object)->x_int)
			{
				PLAYER(current_player)->previous_state = PLAYER(current_player)->state;
				PLAYER(current_player)->state = 0x21u;
				return;
			}
			break;
		case 2:
			if (((uint16_t)goal_line_min == 0u || (goal_line_min <= OBJECT(current_object)->x_int && OBJECT(current_object)->x_int <= goal_line_max)) && goal_line <= OBJECT(current_object)->y_int)
			{
				PLAYER(current_player)->previous_state = PLAYER(current_player)->state;
				PLAYER(current_player)->state = 0x21u;
				return;
			}
			break;
		case 3:
			if (((uint16_t)goal_line_min == 0u || (goal_line_min <= OBJECT(current_object)->y_int && OBJECT(current_object)->y_int <= goal_line_max)) && OBJECT(current_object)->x_int <= goal_line)
			{
				PLAYER(current_player)->previous_state = PLAYER(current_player)->state;
				PLAYER(current_player)->state = 0x21u;
				return;
			}
			break;
		default:
			unknown_code(0xD0BEu + goal_direction * 2u);
		}
		return;
	case 4:
		if (goal_scroll_position <= scroll_position && goal_unlocked != 0u)
			PLAYER(current_player)->state = 0x21u;
		return;
	case 5:
		if (scroll_position < goal_scroll_position)
			return;
		if (((uint16_t)goal_line_min == 0u || (goal_line_min <= OBJECT(current_object)->y_int && OBJECT(current_object)->y_int <= goal_line_max)) && goal_line <= OBJECT(current_object)->x_int)
		{
			PLAYER(current_player)->previous_state = PLAYER(current_player)->state;
			PLAYER(current_player)->state = 0x18u;
			PLAYER(current_player)->scene_path = 0xBu;
			return;
		}
		object = (current_object & ~0xFFFFu) | spiked_press_object;
		if ((uint16_t)object != 0u && (int16_t)OBJECT(object)->var_84 >= 0x258)
		{
			PLAYER(current_player)->previous_state = PLAYER(current_player)->state;
			PLAYER(current_player)->state = 0x18u;
			PLAYER(current_player)->scene_path = 0x11u;
		}
		return;
	default:
		unknown_code(0xD06Du + AS8(stage) * 3u);
	}
}

void spawn_tumble_effect(uint32_t effect)
{
	uint32_t free_count = 0;

	allocate_object(&free_count, &effect);
	if (free_count != 0u)
	{
		OBJECT(effect)->sprite_flags = 0x1004u;
		OBJECT(effect)->sprite_palette = 0x8360u;
		OBJECT(effect)->sprite_offset_x = 0;
		OBJECT(effect)->sprite_offset_y = 0;
		OBJECT(effect)->flags = 0u;
		OBJECT(effect)->handler = tumble_effect_update_ADDRESS;
		OBJECT(effect)->animation = tumble_effect_animation_ADDRESS;
		OBJECT(effect)->parent = current_object;
		if (PLAYER(current_player)->facing >= 1 && PLAYER(current_player)->facing <= 7)
			OBJECT(effect)->sprite_flags |= 0x40u;
	}
}

void tumble_effect_update(void)
{
	uint32_t animation_frame = 0, depth = 0, ignored = 0, object = 0, player = 0;

	object = LINKED(current_object, OBJECT(current_object)->parent);
	OBJECT(current_object)->screen_x = OBJECT(object)->screen_x;
	OBJECT(current_object)->screen_y = OBJECT(object)->screen_y;
	player = PLAYER_BLOCK(1);
	if (OBJECT(object)->player != 0u)
	{
		player = PLAYER_BLOCK(2);
		if (OBJECT(object)->player != 1u)
			player = PLAYER_BLOCK(3);
	}
	if (PLAYER(player)->state != 0x20u || (int8_t)OBJECT(object)->animation_frame >= 0xB)
	{
		delete_current_object();
		return;
	}
	animation_frame = OBJECT(object)->animation_frame;
	OBJECT(current_object)->sprite_frame = mem_read16(OBJECT(current_object)->animation + 2u + animation_frame * 2u);
	OBJECT(current_object)->screen_y += tumble_effect_y_offsets[animation_frame];
	depth = (uint16_t)(PLAYER(player)->draw_depth - 0x10u + tumble_effect_depth_offsets[PLAYER(player)->facing]);
	queue_sprite_at_depth(&ignored, &depth);
}

uint32_t apply_speed_scale(uint32_t target_speed)
{
	if (PLAYER(current_player)->speed_scale != 0u)
		target_speed = (target_speed * PLAYER(current_player)->speed_scale) >> 4;
	return target_speed;
}

void set_player_entry_by_scene(void)
{
	if (current_game_mode == 0xAu && (int8_t)scene_step > 1)
	{
		PLAYER(current_player)->entry_status = 0x82u;
		OBJECT(current_object)->handler = player_place_ADDRESS;
	}
	if (current_game_mode == 2u && player_status[OBJECT(current_object)->player] == 0x82u)
	{
		PLAYER(current_player)->entry_status = 0x82u;
		OBJECT(current_object)->handler = player_place_ADDRESS;
	}
	if (current_game_mode == 0x10u)
	{
		if ((OBJECT(current_object)->flags & 0x2000000u) != 0u)
		{
			OBJECT(current_object)->flags &= ~0x2000000u;
			PLAYER(current_player)->state = 0x1Cu;
			PLAYER(current_player)->entry_status = 0xFFu;
			OBJECT(current_object)->handler = player_update_ADDRESS;
		}
		else
		{
			PLAYER(current_player)->entry_status = 0x82u;
			OBJECT(current_object)->handler = player_place_ADDRESS;
		}
	}
	if (current_game_mode == 1u && OBJECT(current_object)->set_player_entry_by_scene.floor_block_high != 0xFFFFu)
	{
		PLAYER(current_player)->state = 0x1Eu;
		PLAYER(current_player)->entry_status = 0xFFu;
		OBJECT(current_object)->handler = player_update_ADDRESS;
	}
	if (current_game_mode == 0x18u)
	{
		PLAYER(current_player)->state = 0x2Bu;
		PLAYER(current_player)->entry_status = 0xFFu;
		OBJECT(current_object)->handler = player_update_ADDRESS;
	}
	if (current_game_mode == 0x1Bu && game_mode == 0x1Bu)
	{
		PLAYER(current_player)->state = 0x34u;
		PLAYER(current_player)->entry_status = 0xFFu;
		OBJECT(current_object)->handler = player_update_ADDRESS;
	}
	if (stage_exit_started != 0u)
	{
		PLAYER(current_player)->entry_status = 0x82u;
		OBJECT(current_object)->handler = player_place_ADDRESS;
	}
	if (current_game_mode == 0x1Du)
	{
		PLAYER(current_player)->entry_status = 0x82u;
		OBJECT(current_object)->handler = player_place_ADDRESS;
	}
	if (current_game_mode == 0x15u && stage_exit_started != 0u)
	{
		PLAYER(current_player)->entry_status = 0x82u;
		OBJECT(current_object)->handler = player_place_ADDRESS;
	}
	if ((OBJECT(current_object)->flags & 0x2000000u) == 0u && (current_game_mode == 0x13u || (current_game_mode == 0x12u && event_12_fast_scroll != 0u)))
	{
		PLAYER(current_player)->entry_status = 0x82u;
		OBJECT(current_object)->handler = player_place_ADDRESS;
	}
	if (current_game_mode == 0x19u && rope_bridge_collapsing != 0u)
	{
		PLAYER(current_player)->entry_status = 0x82u;
		OBJECT(current_object)->handler = player_place_ADDRESS;
	}
	if (event_1c_ride_phase != 0u)
	{
		PLAYER(current_player)->entry_status = 0x82u;
		OBJECT(current_object)->handler = player_place_ADDRESS;
	}
	if (current_game_mode == 0xDu && (OBJECT(current_object)->flags & 0x2000000u) == 0u)
	{
		PLAYER(current_player)->entry_status = 0x82u;
		OBJECT(current_object)->handler = player_place_ADDRESS;
	}
	if (current_game_mode == 0x1Au)
	{
		PLAYER(current_player)->entry_status = 0x82u;
		OBJECT(current_object)->handler = player_place_ADDRESS;
	}
}

uint32_t try_continue(uint8_t cost)
{
	uint32_t joined = 0;

	joined = 0u;
	if (PLAYER(current_player)->state == 0x37u)
		return joined;
	if (game_in_progress != 0u)
		return pay_and_join(cost, joined);
	joined = 0xFFFFFFFFu;
	return joined;
}

uint32_t try_join(void)
{
	uint8_t cost = 0;
	uint32_t joined = 0;

	joined = 0u;
	if (PLAYER(current_player)->state == 0x37u)
		return joined;
	if (game_in_progress != 0u)
	{
		cost = (uint8_t)credits_per_start;
		return pay_and_join(cost, joined);
	}
	joined = 0xFFFFFFFFu;
	return joined;
}

uint32_t pay_and_join(uint8_t cost, uint32_t joined)
{
	uint8_t cabinet_event = 0;
	uint32_t bit = 0, credit_block = 0, start_bit = 0;
	uint32_t port_d = 0;

	if (free_play != 0u)
		cost = 0u;
	switch (cabinet_config)
	{
	case 0:
	case 1:
	case 4:
		if (p1_credits < (uint8_t)cost)
			return joined;
		credit_block = p1_coins_ADDRESS;
		switch (OBJECT(current_object)->player)
		{
		case 0:
		case 3:
			start_bit = 4u;
			if (setting_no_start_buttons != 0u)
				start_bit = 0x10u;
			break;
		case 1:
			start_bit = 5u;
			if (setting_no_start_buttons != 0u)
				start_bit = 0x18u;
			break;
		case 2:
			start_bit = 7u;
			if (setting_no_start_buttons != 0u)
				start_bit = 0x12u;
			break;
		default:
			unknown_code(0xD50Cu + OBJECT(current_object)->player * 2u);
		}
		if ((inputs_pressed & shl32(1u, (int)(start_bit))) == 0u)
			return joined;
		break;
	case 2:
	pay_and_join_own_credits:
		if (OBJECT(current_object)->player != 1u)
		{
			if (OBJECT(current_object)->player != 0u || p1_credits < (uint8_t)cost)
				return joined;
			credit_block = p1_coins_ADDRESS;
			bit = 0x10u;
			if (setting_no_start_buttons == 0u)
				bit = 4u;
			if ((inputs_pressed & shl32(1u, (int)(bit))) == 0u)
				return joined;
		}
		else
		{
			if (p2_credits < (uint8_t)cost)
				return joined;
			credit_block = p2_coins_ADDRESS;
			bit = 0x18u;
			if (setting_no_start_buttons == 0u)
				bit = 5u;
			if ((inputs_pressed & shl32(1u, (int)(bit))) == 0u)
				return joined;
		}
		break;
	case 3:
		if (OBJECT(current_object)->player != 2u)
			goto pay_and_join_own_credits;
		if (p3_credits < (uint8_t)cost)
			return joined;
		credit_block = p3_coins_ADDRESS;
		bit = 0x12u;
		if (setting_no_start_buttons == 0u)
			bit = 7u;
		if ((inputs_pressed & shl32(1u, (int)(bit))) == 0u)
			return joined;
		break;
	default:
		unknown_code(0xD4E6u + cabinet_config * 3u);
	}
	mem_write8(credit_block + 1u, mem_read8(credit_block + 1u) - cost);
	mem_write8(credit_block + 2u, 0u);
	cabinet_event = 1u;
	if (OBJECT(current_object)->player != 0u)
	{
		cabinet_event = 3u;
		if (OBJECT(current_object)->player != 1u)
			cabinet_event = 5u;
	}
	mem_write8(cabinet_events, cabinet_event);
	if (PLAYER(current_player)->state == 0x23u)
		mem_write8(cabinet_events, cabinet_event);
	if ((cabinet_config & 1u) == 0u)
	{
		bit = 2u;
		if (OBJECT(current_object)->player != 0u)
			bit = 3u;
		port_d = mem_read32(io_port_d);
		mem_write32(io_port_d, port_d & ~shl32(1u, (int)(bit)));
	}
	save_credits();
	joined = 0xFFFFFFFFu;
	return joined;
}

uint32_t check_scene_states(uint32_t object)
{
	if (PLAYER(current_player)->state == 0xBu || PLAYER(current_player)->state == 0x23u || PLAYER(current_player)->state == 0x24u || PLAYER(current_player)->state == 0x16u || PLAYER(current_player)->state == 0x2Eu || PLAYER(current_player)->state == 0x37u)
		return object;
	if (stage == 5u && OBJECT(current_object)->y_int >= 0x400)
		scroll_mode = 7u;
	if (current_game_mode == 0xDu)
	{
		PLAYER(current_player)->previous_state = PLAYER(current_player)->state;
		PLAYER(current_player)->state = 0x19u;
	}
	if (current_game_mode == 1u)
	{
		PLAYER(current_player)->previous_state = PLAYER(current_player)->state;
		PLAYER(current_player)->state = 0x1Eu;
	}
	if (current_game_mode == 0x18u)
	{
		PLAYER(current_player)->previous_state = PLAYER(current_player)->state;
		PLAYER(current_player)->state = 0x2Bu;
	}
	if (current_game_mode == 0x19u && PLAYER(current_player)->state != 0x30u && rope_bridge_collapsing != 0u)
	{
		PLAYER(current_player)->substate = 0u;
		PLAYER(current_player)->previous_state = PLAYER(current_player)->state;
		PLAYER(current_player)->state = 0x30u;
	}
	if (current_game_mode == 0x1Au && PLAYER(current_player)->state != 0x32u)
	{
		PLAYER(current_player)->previous_state = PLAYER(current_player)->state;
		PLAYER(current_player)->state = 0x31u;
	}
	if (current_game_mode == 0x1Cu && event_1c_ride_phase != 0u)
	{
		if (event_1c_riding == 0u)
			object = spawn_player_ride(object);
		else if (PLAYER(current_player)->state != 6u && PLAYER(current_player)->state != 9u && PLAYER(current_player)->state != 0xAu)
		{
			PLAYER(current_player)->previous_state = PLAYER(current_player)->state;
			PLAYER(current_player)->state = 0x35u;
			if (PLAYER(current_player)->previous_state != 0x35u)
				PLAYER(current_player)->substate = 0u;
		}
	}
	if (current_game_mode == 0x17u || current_game_mode == 0x1Du || current_game_mode == 4u)
	{
		PLAYER(current_player)->state = 0x21u;
		PLAYER(current_player)->goal_step = 2u;
		if (stage == 4u)
			PLAYER(current_player)->goal_step = 4u;
	}
	if (PLAYER(current_player)->state != 0x26u && PLAYER(current_player)->state != 6u && PLAYER(current_player)->state != 8u && PLAYER(current_player)->state != 9u && players_pulled != 0u)
	{
		PLAYER(current_player)->previous_state = PLAYER(current_player)->state;
		PLAYER(current_player)->state = 0x26u;
		PLAYER(current_player)->substate = 0u;
	}
	if (current_game_mode == 3u && stage == 0xBu && (uint8_t)OBJECT(current_object)->health != 0u && PLAYER(current_player)->state != 9u && (uint16_t)OBJECT(current_object)->floor_z_int != 0x800u && PLAYER(current_player)->ending_done == 0u && OBJECT(current_object)->y_int <= 0x500)
	{
		PLAYER(current_player)->previous_state = PLAYER(current_player)->state;
		PLAYER(current_player)->state = 0x38u;
	}
	if (current_game_mode == 0x20u)
	{
		PLAYER(current_player)->previous_state = PLAYER(current_player)->state;
		PLAYER(current_player)->state = 0x39u;
	}
	return object;
}

void set_fall_sprite_priority(void)
{
	uint8_t priority = 0;
	uint32_t record = 0, side = 0;

	record = floor_blocks + 2u + OBJECT(current_object)->previous_floor_block;
	if (mem_read8(record + 0x12u) == 0x1Fu || mem_read8(record + 0x12u) == 0x11u)
	{
		side = 3u;
		if ((int32_t)(((mem_read32(record + 4u) - mem_read32(record)) >> 1) + mem_read32(record)) <= OBJECT(current_object)->x)
			side = 1u;
		record = fall_sprite_priorities_ADDRESS + mem_read8(record + 0x14u) * 4u;
	}
	else
	{
		record = floor_blocks + 2u + OBJECT(current_object)->floor_block;
		if (mem_read8(record + 0x12u) == 0x1Eu || mem_read8(record + 0x12u) == 0x1Du)
		{
			side = 3u;
			if ((int32_t)(((mem_read32(record + 4u) - mem_read32(record)) >> 1) + mem_read32(record)) <= OBJECT(current_object)->x)
				side = 1u;
			record = fall_sprite_priorities_ADDRESS + mem_read8(record + 0x14u) * 4u;
			priority = ROM8(record + side);
			if ((int8_t)priority >= 0)
			{
				OBJECT(current_object)->sprite_palette &= 0x9FFFu;
				OBJECT(current_object)->sprite_palette |= (uint32_t)priority << 13;
			}
			return;
		}
		record = fall_sprite_priorities_ADDRESS + mem_read8(floor_blocks + 2u + OBJECT(current_object)->previous_floor_block + 0x14u) * 4u;
		side = AS8(PLAYER(current_player)->ledge_side);
		if (side == 4u)
			side = (uint8_t)PLAYER(current_player)->ledge_direction;
	}
	priority = ROM8(record + side);
	if ((int8_t)priority >= 0)
	{
		OBJECT(current_object)->sprite_palette &= 0x9FFFu;
		OBJECT(current_object)->sprite_palette |= (uint32_t)priority << 13;
	}
}

void start_climb_at_spawn(uint32_t climbed_height)
{
	uint32_t attribute_record = 0, block = 0, climb_animation = 0, distance = 0;

	if (OBJECT(current_object)->floor_type != 2u)
		return;
	climb_animation = sonic_climb_animation_ADDRESS;
	if (OBJECT(current_object)->kind != 0u)
	{
		climb_animation = ray_climb_animation_ADDRESS;
		if (OBJECT(current_object)->kind != 1u)
			climb_animation = mighty_climb_animation_ADDRESS;
	}
	OBJECT(current_object)->animation = climb_animation;
	OBJECT(current_object)->sprite_flags |= 0x40u;
	if (floor_types[OBJECT(current_object)->floor_attribute] != 0u)
		OBJECT(current_object)->sprite_flags &= 0xFFBFu;
	OBJECT(current_object)->animation_timer = 0;
	OBJECT(current_object)->animation_speed = 3u;
	OBJECT(current_object)->animation_frame = 0xFFu;
	attribute_record = OBJECT(current_object)->floor_attribute + floor_types_ADDRESS;
	PLAYER(current_player)->climb_remaining = ROM16(attribute_record + 6u);
	climbed_height = (climbed_height & ~0xFFFFu) | ROM16(attribute_record + 0xEu);
	block = floor_blocks + 2u + OBJECT(current_object)->floor_block;
	if (ROM8(attribute_record) == 0u)
		distance = OBJECT(current_object)->x - mem_read32(block);
	else
		distance = mem_read32(block + 0xCu) - OBJECT(current_object)->y;
	climbed_height = climbed_height * ((int32_t)distance >> 8);
	OBJECT(current_object)->z -= climbed_height;
	PLAYER(current_player)->climb_remaining -= climbed_height >> 16;
	AT16(PLAYER(current_player)->climb_base_z, 2) = mem_read16(block + 0x10u);
	AS16(PLAYER(current_player)->climb_base_z) = 0u;
	PLAYER(current_player)->step_accumulator = 0;
	PLAYER(current_player)->substate = 3u;
	PLAYER(current_player)->state = 0xAu;
}

void start_climb(void)
{
	uint32_t animation = 0, ignored = 0;

	if (OBJECT(current_object)->floor_type != 2u)
		return;
	animation = sonic_climb_animation_ADDRESS;
	if (OBJECT(current_object)->kind != 0u)
	{
		animation = ray_climb_animation_ADDRESS;
		if (OBJECT(current_object)->kind != 1u)
			animation = mighty_climb_animation_ADDRESS;
	}
	OBJECT(current_object)->animation = animation;
	OBJECT(current_object)->sprite_flags |= 0x40u;
	if (floor_types[OBJECT(current_object)->floor_attribute] != 0u)
		OBJECT(current_object)->sprite_flags &= 0xFFBFu;
	OBJECT(current_object)->animation_timer = 0;
	OBJECT(current_object)->animation_speed = 3u;
	OBJECT(current_object)->animation_frame = 0xFFu;
	OBJECT(current_object)->x += 0x41000u;
	AT16(PLAYER(current_player)->climb_base_z, 2) = mem_read16(floor_blocks + 2u + OBJECT(current_object)->floor_block + 0x10u);
	AS16(PLAYER(current_player)->climb_base_z) = 0u;
	PLAYER(current_player)->climb_remaining = ROM16(floor_attribute_wall_heights_ADDRESS + OBJECT(current_object)->floor_attribute);
	PLAYER(current_player)->climb_remaining -= AT16(PLAYER(current_player)->climb_base_z, 2) - OBJECT(current_object)->z_int;
	project_object(&ignored, &ignored);
	PLAYER(current_player)->screen_x_int = OBJECT(current_object)->projected_x;
	AS16(PLAYER(current_player)->screen_x) = 0u;
	PLAYER(current_player)->step_accumulator = 0;
	PLAYER(current_player)->substate = 3u;
	PLAYER(current_player)->state = 0xAu;
}

void add_push_to_player_motion(void)
{
	uint16_t dy = 0;
	uint32_t angle = 0, ignored = 0, length = 0, sum_dx = 0, sum_dy = 0;

	if ((PLAYER(current_player)->push_x | PLAYER(current_player)->push_y) == 0u)
	{
		PLAYER(current_player)->push_x = 0;
		PLAYER(current_player)->push_y = 0;
		AS32(PLAYER(current_player)->push_y) = 0u;
		return;
	}
	length = (uint16_t)OBJECT(current_object)->speed_int;
	angle = PLAYER(current_player)->heading_snapped;
	polar_to_xy(&ignored, &angle, &length);
	push_motion_x = angle;
	push_motion_y = length;
	push_motion_x += AS32(PLAYER(current_player)->push_x);
	push_motion_y += AS32(PLAYER(current_player)->push_y);
	sum_dx = (int32_t)push_motion_x >> 4;
	push_motion_x = sum_dx;
	sum_dy = (int32_t)push_motion_y >> 4;
	push_motion_y = sum_dy;
	sum_dx = sum_dx * sum_dx + sum_dy * sum_dy;
	square_root(&sum_dx, &ignored);
	OBJECT(current_object)->speed = (uint32_t)(uint16_t)sum_dx << 12;
	if (OBJECT(current_object)->speed_int < 0)
		OBJECT(current_object)->speed = 0;
	if (OBJECT(current_object)->speed > 0x5000000)
		OBJECT(current_object)->speed = 0x5000000;
	sum_dx = push_motion_x;
	dy = (uint16_t)push_motion_y;
	angle_of_vector(dy, &sum_dx, &ignored);
	PLAYER(current_player)->heading = sum_dx;
	PLAYER(current_player)->push_x = 0;
	PLAYER(current_player)->push_y = 0;
	AS32(PLAYER(current_player)->push_y) = 0u;
}

void apply_floor_push(void)
{
	uint32_t attribute_record = 0;

	if ((OBJECT(current_object)->flags & 0x80000000u) == 0u)
	{
		PLAYER(current_player)->push_x = 0;
		PLAYER(current_player)->push_y = 0;
		if (PLAYER(current_player)->invulnerable_timer < 0 && ((OBJECT(current_object)->floor_type == 7u && OBJECT(current_object)->floor_attribute != 0u) || OBJECT(current_object)->floor_type == 0xBu || OBJECT(current_object)->floor_type == 4u || (OBJECT(current_object)->floor_type == 5u && PLAYER(current_player)->state != 0x17u)))
		{
			attribute_record = OBJECT(current_object)->floor_attribute + floor_types_ADDRESS;
			PLAYER(current_player)->push_x += ROM16(attribute_record);
			PLAYER(current_player)->push_y += ROM16(attribute_record + 2u);
		}
	}
}

uint32_t spawn_slip_piece(uint32_t piece)
{
	uint32_t free_count = 0, ignored = 0;

	allocate_object(&free_count, &piece);
	if (free_count == 0u)
		return piece;
	OBJECT(piece)->sprite_flags = 0x1004u;
	if ((OBJECT(current_object)->sprite_flags & 0x40u) != 0u)
		OBJECT(piece)->sprite_flags |= 0x40u;
	OBJECT(piece)->sprite_frame = mem_read16(OBJECT(current_object)->animation + 0x3Au);
	OBJECT(piece)->sprite_palette = OBJECT(current_object)->sprite_palette;
	OBJECT(piece)->sprite_offset_x = OBJECT(current_object)->sprite_offset_x;
	OBJECT(piece)->sprite_offset_y = OBJECT(current_object)->sprite_offset_y;
	OBJECT(piece)->flags = 0u;
	OBJECT(piece)->handler = slip_piece_update_ADDRESS;
	OBJECT(piece)->speed_x_fraction = 0x10u;
	OBJECT(piece)->speed_z = 0;
	OBJECT(piece)->direction = PLAYER(current_player)->ledge_side;
	OBJECT(piece)->x = OBJECT(current_object)->x;
	OBJECT(piece)->y = OBJECT(current_object)->y;
	OBJECT(piece)->z = OBJECT(current_object)->z;
	current_object = piece;
	project_object(&ignored, &ignored);
	current_object = object_cursor;
	return piece;
}

void slip_piece_update(void)
{
	uint8_t bucket = 0;
	uint32_t r1 = 0, ignored = 0, off_screen = 0;
	uint16_t speed_x_fraction = 0;

	off_screen = is_off_screen();
	if (off_screen != 0u)
	{
		delete_current_object();
		return;
	}
	speed_x_fraction = OBJECT(current_object)->speed_x_fraction;
	OBJECT(current_object)->speed_x_fraction = speed_x_fraction - 1u;
	if ((int16_t)speed_x_fraction <= 1)
	{
		OBJECT(current_object)->speed_z += 0x6000u;
		OBJECT(current_object)->z += OBJECT(current_object)->speed_z;
	}
	update_screen_position(&ignored, &r1);
	if ((int16_t)OBJECT(current_object)->direction >= 2)
	{
		r1 = queue_sprite_at_base(r1);
		return;
	}
	bucket = 0xFFu;
	queue_sprite_in_bucket(bucket);
}

void spawn_slip_pieces(uint32_t piece)
{
	uint32_t ignored = 0;
	uint16_t pieces_left = 0;

	if (free_object_stack < 2)
		return;
	slip_pieces_left = 2;
	slip_piece_speed = 0x8000u;
	do
	{
		allocate_object(&ignored, &piece);
		OBJECT(piece)->sprite_flags = 0x1000u;
		OBJECT(piece)->animation = OBJECT(current_object)->animation + 0x3Au;
		OBJECT(piece)->sprite_palette = OBJECT(current_object)->sprite_palette;
		OBJECT(piece)->sprite_offset_x = 0;
		OBJECT(piece)->sprite_offset_y = 0;
		OBJECT(piece)->flags = 0u;
		OBJECT(piece)->handler = slip_pieces_update_ADDRESS;
		OBJECT(piece)->speed_z = -0x80000;
		OBJECT(piece)->direction = PLAYER(current_player)->ledge_side;
		OBJECT(piece)->animation_frame = 0u;
		OBJECT(piece)->animation_speed = 4u;
		OBJECT(piece)->animation_timer = 0;
		OBJECT(piece)->x = OBJECT(current_object)->x;
		OBJECT(piece)->y = OBJECT(current_object)->y;
		OBJECT(piece)->z = OBJECT(current_object)->z;
		switch (OBJECT(piece)->direction)
		{
		case 0:
		case 2:
		case 4:
			OBJECT(piece)->speed_x = slip_piece_speed;
			OBJECT(piece)->speed_y = 0;
			break;
		case 1:
		case 3:
			OBJECT(piece)->speed_y = slip_piece_speed;
			OBJECT(piece)->speed_x = 0;
			break;
		default:
			unknown_code(0xDC37u + OBJECT(piece)->direction * 2u);
		}
		slip_piece_speed = 0u - slip_piece_speed;
		pieces_left = slip_pieces_left;
		slip_pieces_left = pieces_left - 1u;
	} while ((int16_t)pieces_left > 1);
}

void slip_pieces_update(void)
{
	uint8_t bucket = 0;
	uint16_t margin_y = 0;
	uint32_t r7 = 0, ignored = 0, margin_x = 0, off_screen = 0;

	OBJECT(current_object)->x += OBJECT(current_object)->speed_x;
	OBJECT(current_object)->y += OBJECT(current_object)->speed_y;
	OBJECT(current_object)->speed_z += 0x6000u;
	OBJECT(current_object)->z += OBJECT(current_object)->speed_z;
	project_object(&ignored, &ignored);
	margin_x = 0x20u;
	margin_y = 0x50u;
	off_screen = is_off_screen_margin((uint16_t)margin_x, margin_y);
	if (off_screen != 0u)
	{
		delete_current_object();
		return;
	}
	animate(&margin_x, &r7, &off_screen);
	if ((int16_t)OBJECT(current_object)->direction >= 2)
	{
		margin_x = queue_sprite_at_base(margin_x);
		return;
	}
	bucket = 0xFFu;
	queue_sprite_in_bucket(bucket);
}

uint32_t follow_player_path(uint32_t target_speed)
{
	uint16_t dy = 0, x_offset = 0;
	uint32_t ignored = 0, path = 0, path_heading = 0, point = 0;
	uint16_t path_wait = 0;
	uint32_t flags = 0;

	if (PLAYER(current_player)->path_request == 0u)
	{
		if (all_players_path_request == 0u)
		{
			if (PLAYER(current_player)->path == 0u)
				return target_speed;
		follow_player_path_run:
			all_players_path_request = 0u;
			if ((uint16_t)PLAYER(current_player)->path_wait == 0u)
			{
				point = PLAYER(current_player)->path_pointer;
				if ((ROM32(point + 6u) & 2u) == 0u)
				{
					point += 2u;
					x_offset = (uint16_t)(OBJECT(current_object)->x_int - ROM16(point) + 8u);
					point += 2u;
					if (x_offset >= 0x11u || (uint16_t)(OBJECT(current_object)->y_int - ROM16(point) + 8u) >= 0x11u)
					{
						point = PLAYER(current_player)->path_pointer + 2u;
						path_heading = (uint16_t)(ROM16(point) - OBJECT(current_object)->x_int);
						dy = (uint16_t)(ROM16(point + 2u) - OBJECT(current_object)->y_int);
						angle_of_vector(dy, &path_heading, &ignored);
						path_heading = (path_heading - 0x80u) & 0x3FFu;
						PLAYER(current_player)->target_heading = path_heading;
						PLAYER(current_player)->facing_angle = path_heading;
						flags = OBJECT(current_object)->flags;
						OBJECT(current_object)->flags = flags & ~1u;
						if ((flags & 1u) != 0u)
							PLAYER(current_player)->heading = path_heading;
						target_speed = (uint32_t)PLAYER(current_player)->path_speed << 16;
						if ((int32_t)target_speed <= 0x200000 && OBJECT(current_object)->speed_int < 0x20)
						{
							target_speed = 0u;
							OBJECT(current_object)->speed = 0;
						}
						return target_speed;
					}
				}
			follow_player_path_next_record:
				if ((int16_t)ROM16(PLAYER(current_player)->path_pointer + 8u) >= 0)
				{
					PLAYER(current_player)->path_pointer += 8u;
				follow_player_path_load_record:
					PLAYER(current_player)->path_speed = ROM16(PLAYER(current_player)->path_pointer);
					if ((ROM32(PLAYER(current_player)->path_pointer + 6u) & 1u) != 0u)
					{
						PLAYER(current_player)->state = 4u;
						PLAYER(current_player)->substate = 0u;
						PLAYER(current_player)->ledge_side = -1;
					}
					if (PLAYER(current_player)->path_speed != 0u)
					{
						PLAYER(current_player)->path_wait = 0;
						point = PLAYER(current_player)->path_pointer + 2u;
						path_heading = (uint16_t)(ROM16(point) - OBJECT(current_object)->x_int);
						dy = (uint16_t)(ROM16(point + 2u) - OBJECT(current_object)->y_int);
						angle_of_vector(dy, &path_heading, &ignored);
						path_heading = (path_heading - 0x80u) & 0x3FFu;
						PLAYER(current_player)->target_heading = path_heading;
						PLAYER(current_player)->facing_angle = path_heading;
						flags = OBJECT(current_object)->flags;
						OBJECT(current_object)->flags = flags & ~1u;
						if ((flags & 1u) != 0u)
							PLAYER(current_player)->heading = path_heading;
					}
					else
						PLAYER(current_player)->path_wait = ROM16(PLAYER(current_player)->path_pointer + 2u);
				}
				else
					PLAYER(current_player)->path = 0u;
				target_speed = (uint32_t)PLAYER(current_player)->path_speed << 16;
				if ((int32_t)target_speed <= 0x200000 && OBJECT(current_object)->speed_int < 0x20)
				{
					target_speed = 0u;
					OBJECT(current_object)->speed = 0;
				}
				return target_speed;
			}
			path_wait = PLAYER(current_player)->path_wait;
			PLAYER(current_player)->path_wait = path_wait - 1u;
			if ((int16_t)path_wait <= 1)
				goto follow_player_path_next_record;
			target_speed = 0u;
			if (OBJECT(current_object)->speed_int < 0x20)
				OBJECT(current_object)->speed = 0;
			return target_speed;
		}
		if (PLAYER(current_player)->path == all_players_path_request)
			goto follow_player_path_run;
		PLAYER(current_player)->path = all_players_path_request;
	follow_player_path_start:
		path = PLAYER(current_player)->path;
		PLAYER(current_player)->path_pointer = player_path_offsets_ADDRESS + player_path_offsets[(uint8_t)(path + OBJECT(current_object)->player)];
		OBJECT(current_object)->speed = 0;
		OBJECT(current_object)->z = OBJECT(current_object)->floor_z;
		OBJECT(current_object)->speed_z = 0;
		PLAYER(current_player)->state_timer = 0;
		OBJECT(current_object)->sprite_palette = mem_read16(OBJECT(current_object)->follow_player_path.palette_list + 2u);
		OBJECT(current_object)->flags &= ~0x40u;
		OBJECT(current_object)->flags &= ~0x2000u;
		PLAYER(current_player)->control_lock = 0u;
		PLAYER(current_player)->state = 1u;
		OBJECT(current_object)->flags |= 1u;
		goto follow_player_path_load_record;
	}
	PLAYER(current_player)->path = PLAYER(current_player)->path_request;
	PLAYER(current_player)->path_request = 0u;
	goto follow_player_path_start;
}
