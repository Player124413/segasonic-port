/**
 *    Name:        game_02.c
 *    Description: the game's functions, part 02 (in the order of their V60 addresses)
 *    Version:     0.1.0
 *    Created:     12-10-2026
 *    Modified:    14-10-2026
 *    Author:      Sebastian Toyos
 *
 *    SegaSonic The Hedgehog (arcade, Sega System 32): the game's V60 program decompiled into C by
 *    tools/decompile.py, with the names of symbols.txt.
 */
#include "game.h"

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

void animate_with_flags(uint32_t *p_frame, uint32_t *p_at_last_step)
{
	uint32_t frame = *p_frame;
	uint32_t at_last_step = 0;
	uint8_t animation_timer = 0;

	at_last_step = 0u;
	animation_timer = OBJECT(current_object)->animation_timer;
	OBJECT(current_object)->animation_timer = animation_timer - 1u;
	if ((int8_t)animation_timer > 1)
	{
		*p_at_last_step = (*p_at_last_step & 0xFFFFFF00u) | (at_last_step & 0xFFu);
		return;
	}
	OBJECT(current_object)->animation_timer = OBJECT(current_object)->animation_speed;
	animate_step_with_flags(&frame, &at_last_step);
	*p_frame = frame;
	*p_at_last_step = (*p_at_last_step & 0xFFFFFF00u) | (at_last_step & 0xFFu);
}

void animate_step_with_flags(uint32_t *p_frame, uint32_t *p_at_last_step)
{
	uint8_t at_last_step = (uint8_t)*p_at_last_step;
	uint8_t last_frame = 0;
	uint32_t frame = 0, step_record = 0;

	step_record = OBJECT(current_object)->animation;
	last_frame = (uint8_t)mem_read16(step_record);
	frame = OBJECT(current_object)->animation_frame;
	frame = (uint8_t)(frame + 1u);
	step_record += 2u;
	if ((int8_t)last_frame <= (int8_t)frame)
	{
		if ((int8_t)last_frame < (int8_t)frame)
			frame = 0u;
		else
			at_last_step = 0xFFu;
	}
	OBJECT(current_object)->animation_frame = frame;
	frame = frame * 2u;
	step_record += frame * 2u;
	OBJECT(current_object)->sprite_flags = (OBJECT(current_object)->sprite_flags & ~0xC0u) | mem_read16(step_record);
	OBJECT(current_object)->sprite_frame = mem_read16(step_record + 2u);
	*p_frame = frame;
	*p_at_last_step = (*p_at_last_step & 0xFFFFFF00u) | at_last_step;
}

void animate(uint32_t *p_frame, uint32_t *p_wrapped, uint32_t *p_at_last_step)
{
	uint32_t frame = *p_frame;
	uint32_t at_last_step = 0, wrapped = 0;
	uint8_t animation_timer = 0;

	at_last_step = 0u;
	wrapped = 0u;
	animation_timer = OBJECT(current_object)->animation_timer;
	OBJECT(current_object)->animation_timer = animation_timer - 1u;
	if ((int8_t)animation_timer > 1)
	{
		*p_wrapped = (*p_wrapped & 0xFFFFFF00u) | (wrapped & 0xFFu);
		*p_at_last_step = (*p_at_last_step & 0xFFFFFF00u) | (at_last_step & 0xFFu);
		return;
	}
	OBJECT(current_object)->animation_timer = OBJECT(current_object)->animation_speed;
	animate_step(&frame, &wrapped, &at_last_step);
	*p_frame = frame;
	*p_wrapped = (*p_wrapped & 0xFFFFFF00u) | (wrapped & 0xFFu);
	*p_at_last_step = (*p_at_last_step & 0xFFFFFF00u) | (at_last_step & 0xFFu);
}

void animate_step(uint32_t *p_frame, uint32_t *p_wrapped, uint32_t *p_at_last_step)
{
	uint8_t wrapped = (uint8_t)*p_wrapped;
	uint8_t at_last_step = (uint8_t)*p_at_last_step;
	uint8_t last_frame = 0;
	uint32_t frame = 0, frame_list = 0;

	frame_list = OBJECT(current_object)->animation;
	last_frame = (uint8_t)mem_read16(frame_list);
	frame = OBJECT(current_object)->animation_frame;
	frame_list += 2u;
	if ((OBJECT(current_object)->flags & 0x80u) == 0u)
	{
		frame = (uint8_t)(frame + 1u);
		if ((int8_t)frame < (int8_t)last_frame)
		{
			OBJECT(current_object)->animation_frame = frame;
			OBJECT(current_object)->sprite_frame = mem_read16(frame_list + frame * 2u);
			*p_frame = frame;
			return;
		}
		if ((int8_t)last_frame < (int8_t)frame)
		{
			frame = 0u;
			wrapped = 0xFFu;
			OBJECT(current_object)->animation_frame = frame;
			OBJECT(current_object)->sprite_frame = mem_read16(frame_list + frame * 2u);
			*p_frame = frame;
			*p_wrapped = (*p_wrapped & 0xFFFFFF00u) | wrapped;
			return;
		}
		at_last_step = 0xFFu;
		OBJECT(current_object)->animation_frame = frame;
		OBJECT(current_object)->sprite_frame = mem_read16(frame_list + frame * 2u);
		*p_frame = frame;
		*p_at_last_step = (*p_at_last_step & 0xFFFFFF00u) | at_last_step;
		return;
	}
	frame = (uint8_t)(frame - 1u);
	if ((int8_t)frame > 0)
	{
		OBJECT(current_object)->animation_frame = frame;
		OBJECT(current_object)->sprite_frame = mem_read16(frame_list + frame * 2u);
		*p_frame = frame;
		return;
	}
	if ((int8_t)frame < 0)
	{
		frame = (uint8_t)last_frame;
		wrapped = 0xFFu;
		OBJECT(current_object)->animation_frame = frame;
		OBJECT(current_object)->sprite_frame = mem_read16(frame_list + frame * 2u);
		*p_frame = frame;
		*p_wrapped = (*p_wrapped & 0xFFFFFF00u) | wrapped;
		return;
	}
	at_last_step = 0xFFu;
	OBJECT(current_object)->animation_frame = frame;
	OBJECT(current_object)->sprite_frame = mem_read16(frame_list + frame * 2u);
	*p_frame = frame;
	*p_at_last_step = (*p_at_last_step & 0xFFFFFF00u) | at_last_step;
}

uint32_t animate_and_move(uint32_t at_last_step)
{
	uint8_t last_frame = 0;
	uint16_t delta = 0;
	uint32_t frame = 0, step_record = 0;
	uint8_t animation_timer = 0;

	at_last_step &= ~0xFFu;
	animation_timer = OBJECT(current_object)->animation_timer;
	OBJECT(current_object)->animation_timer = animation_timer - 1u;
	if ((int8_t)animation_timer > 1)
		return at_last_step;
	OBJECT(current_object)->animation_timer = OBJECT(current_object)->animation_speed;
	step_record = OBJECT(current_object)->animation;
	last_frame = (uint8_t)mem_read16(step_record);
	frame = OBJECT(current_object)->animation_frame;
	step_record += 2u;
	if ((OBJECT(current_object)->flags & 0x80u) == 0u)
	{
		frame = (uint8_t)(frame + 1u);
		if ((int8_t)last_frame <= (int8_t)frame)
		{
			if ((int8_t)last_frame < (int8_t)frame)
				frame = 0u;
			else
				at_last_step |= 0xFFu;
		}
	}
	else
	{
		frame = (uint8_t)(frame - 1u);
		if ((int8_t)frame <= 0)
		{
			if ((int8_t)frame < 0)
				frame = (uint8_t)last_frame;
			else
				at_last_step |= 0xFFu;
		}
	}
	OBJECT(current_object)->animation_frame = frame;
	step_record += frame * 4u;
	OBJECT(current_object)->sprite_frame = mem_read16(step_record);
	step_record += 2u;
	delta = (uint16_t)(int8_t)mem_read8(step_record);
	step_record++;
	if ((OBJECT(current_object)->sprite_flags & 0x40u) != 0u)
		delta = (uint16_t)(0u - delta);
	OBJECT(current_object)->projected_x += delta;
	delta = (uint16_t)(int8_t)mem_read8(step_record);
	if ((OBJECT(current_object)->sprite_flags & 0x80u) != 0u)
		delta = (uint16_t)(0u - delta);
	OBJECT(current_object)->projected_y_int += delta;
	return at_last_step;
}

uint32_t animate_around_anchor(void)
{
	uint8_t last_frame = 0;
	uint32_t at_last_step = 0, frame = 0, steps = 0;
	uint8_t animation_timer = 0;

	at_last_step = 0u;
	animation_timer = OBJECT(current_object)->animation_timer;
	OBJECT(current_object)->animation_timer = animation_timer - 1u;
	if ((int8_t)animation_timer > 1)
		return at_last_step;
	OBJECT(current_object)->animation_timer = OBJECT(current_object)->animation_speed;
	steps = OBJECT(current_object)->animation;
	last_frame = (uint8_t)mem_read16(steps);
	frame = OBJECT(current_object)->animation_frame;
	steps += 2u;
	if ((OBJECT(current_object)->flags & 0x80u) == 0u)
	{
		frame = (uint8_t)(frame + 1u);
		if ((int8_t)frame < (int8_t)last_frame)
		{
			OBJECT(current_object)->animation_frame = frame;
			set_step_around_anchor(frame, steps);
			return at_last_step;
		}
		if ((int8_t)last_frame < (int8_t)frame)
			frame = 0u;
		else
			at_last_step = 0xFFFFFFFFu;
	}
	else
	{
		frame = (uint8_t)(frame - 1u);
		if ((int8_t)frame > 0)
		{
			OBJECT(current_object)->animation_frame = frame;
			set_step_around_anchor(frame, steps);
			return at_last_step;
		}
		if ((int8_t)frame < 0)
			frame = (uint8_t)last_frame;
		else
			at_last_step = 0xFFFFFFFFu;
	}
	OBJECT(current_object)->animation_frame = frame;
	set_step_around_anchor(frame, steps);
	return at_last_step;
}

void set_step_around_anchor(uint32_t frame, uint32_t steps)
{
	uint16_t delta = 0;

	steps += frame * 4u;
	OBJECT(current_object)->sprite_frame = mem_read16(steps);
	steps += 2u;
	delta = (uint16_t)(int8_t)mem_read8(steps);
	steps++;
	if ((OBJECT(current_object)->sprite_flags & 0x40u) != 0u)
		delta = (uint16_t)(0u - delta);
	OBJECT(current_object)->projected_x = delta + OBJECT(current_object)->anchor_x;
	delta = (uint16_t)(int8_t)mem_read8(steps);
	if ((OBJECT(current_object)->sprite_flags & 0x80u) != 0u)
		delta = (uint16_t)(0u - delta);
	OBJECT(current_object)->projected_y_int = delta + OBJECT(current_object)->anchor_y;
}

uint32_t animate_with_screen_offset(uint32_t at_last_step)
{
	uint8_t last_frame = 0;
	uint32_t frame = 0, step_record = 0;
	uint8_t animation_timer = 0;

	at_last_step &= ~0xFFu;
	animation_timer = OBJECT(current_object)->animation_timer;
	OBJECT(current_object)->animation_timer = animation_timer - 1u;
	if ((int8_t)animation_timer > 1)
	{
		apply_step_screen_offset();
		return at_last_step;
	}
	OBJECT(current_object)->animation_timer = OBJECT(current_object)->animation_speed;
	step_record = OBJECT(current_object)->animation;
	last_frame = (uint8_t)mem_read16(step_record);
	frame = OBJECT(current_object)->animation_frame;
	step_record += 2u;
	if ((OBJECT(current_object)->flags & 0x80u) == 0u)
	{
		frame = (uint8_t)(frame + 1u);
		if ((int8_t)frame < (int8_t)last_frame)
		{
			OBJECT(current_object)->animation_frame = frame;
			step_record += frame * 4u;
			OBJECT(current_object)->sprite_frame = mem_read16(step_record);
			step_record += 2u;
			add_step_screen_offset(step_record);
			return at_last_step;
		}
		if ((int8_t)last_frame < (int8_t)frame)
			frame = 0u;
		else
			at_last_step |= 0xFFu;
	}
	else
	{
		frame = (uint8_t)(frame - 1u);
		if ((int8_t)frame > 0)
		{
			OBJECT(current_object)->animation_frame = frame;
			step_record += frame * 4u;
			OBJECT(current_object)->sprite_frame = mem_read16(step_record);
			step_record += 2u;
			add_step_screen_offset(step_record);
			return at_last_step;
		}
		if ((int8_t)frame < 0)
			frame = (uint8_t)last_frame;
		else
			at_last_step |= 0xFFu;
	}
	OBJECT(current_object)->animation_frame = frame;
	step_record += frame * 4u;
	OBJECT(current_object)->sprite_frame = mem_read16(step_record);
	step_record += 2u;
	add_step_screen_offset(step_record);
	return at_last_step;
}

void apply_step_screen_offset(void)
{
	uint32_t step_offset = 0;

	step_offset = OBJECT(current_object)->animation + 4u + OBJECT(current_object)->animation_frame * 4u;
	add_step_screen_offset(step_offset);
}

void add_step_screen_offset(uint32_t step_offset)
{
	uint16_t delta = 0;

	delta = (uint16_t)(int8_t)mem_read8(step_offset);
	step_offset++;
	if ((OBJECT(current_object)->sprite_flags & 0x40u) != 0u)
		delta = (uint16_t)(0u - delta);
	OBJECT(current_object)->screen_x += delta;
	delta = (uint16_t)(int8_t)mem_read8(step_offset);
	if ((OBJECT(current_object)->sprite_flags & 0x80u) != 0u)
		delta = (uint16_t)(0u - delta);
	OBJECT(current_object)->screen_y += delta;
}

void animate_part_list(void)
{
	uint8_t animation_timer = 0;

	animation_timer = OBJECT(current_object)->animation_timer;
	OBJECT(current_object)->animation_timer = animation_timer - 1u;
	if ((int8_t)animation_timer > 1)
		return;
	OBJECT(current_object)->animation_timer = OBJECT(current_object)->animation_speed;
	animate_part_list_step();
}

void animate_part_list_step(void)
{
	uint8_t max_frame = 0;
	uint32_t frame_offsets = 0, next_frame = 0;

	frame_offsets = OBJECT(current_object)->animation;
	max_frame = (uint8_t)mem_read16(frame_offsets);
	next_frame = OBJECT(current_object)->animation_frame;
	frame_offsets += 2u;
	if ((OBJECT(current_object)->flags & 0x80u) == 0u)
	{
		next_frame = (uint8_t)(next_frame + 1u);
		if ((int8_t)max_frame <= (int8_t)next_frame && (int8_t)max_frame < (int8_t)next_frame)
			next_frame = 0u;
	}
	else
	{
		next_frame = (uint8_t)(next_frame - 1u);
		if ((int8_t)next_frame <= 0 && (int8_t)next_frame < 0)
			next_frame = (uint8_t)max_frame;
	}
	OBJECT(current_object)->animation_frame = next_frame;
	OBJECT(current_object)->part_list = mem_read16(frame_offsets + next_frame * 2u) + frame_offsets - 2u;
}

void animate_all_parts(uint32_t *p_at_last_step, uint32_t *p_part)
{
	uint32_t part = *p_part;
	uint8_t at_last_step = 0, max_frame = 0;
	uint16_t frame_sprite = 0;
	uint32_t frame_sprites = 0, next_frame = 0, parts_left = 0;
	uint8_t animation_timer = 0;

	at_last_step = 0u;
	animation_timer = OBJECT(current_object)->animation_timer;
	OBJECT(current_object)->animation_timer = animation_timer - 1u;
	if ((int8_t)animation_timer > 1)
	{
		*p_at_last_step = (*p_at_last_step & 0xFFFFFF00u) | at_last_step;
		return;
	}
	OBJECT(current_object)->animation_timer = OBJECT(current_object)->animation_speed;
	frame_sprites = OBJECT(current_object)->animation;
	max_frame = (uint8_t)mem_read16(frame_sprites);
	next_frame = OBJECT(current_object)->animation_frame;
	frame_sprites += 2u;
	if ((OBJECT(current_object)->flags & 0x80u) == 0u)
	{
		next_frame = (uint8_t)(next_frame + 1u);
		if ((int8_t)max_frame <= (int8_t)next_frame)
		{
			if ((int8_t)max_frame < (int8_t)next_frame)
				next_frame = 0u;
			else
				at_last_step = 0xFFu;
		}
	}
	else
	{
		next_frame = (uint8_t)(next_frame - 1u);
		if ((int8_t)next_frame <= 0)
		{
			if ((int8_t)next_frame < 0)
				next_frame = (uint8_t)max_frame;
			else
				at_last_step = 0xFFu;
		}
	}
	OBJECT(current_object)->animation_frame = next_frame;
	frame_sprite = mem_read16(frame_sprites + next_frame * 2u);
	part = OBJECT(current_object)->part_list;
	parts_left = mem_read16(part);
	part += 2u;
	do
	{
		mem_write16(part, frame_sprite);
		part += 8u;
		parts_left--;
	} while (parts_left != 0u);
	*p_at_last_step = (*p_at_last_step & 0xFFFFFF00u) | at_last_step;
	*p_part = part;
}

uint32_t spawn_player_ride(uint32_t object)
{
	uint32_t free_count = 0;

	if (PLAYER(current_player)->ridden_object != 0u)
		return object;
	if (OBJECT(current_object)->x_int <= 0xAE0)
	{
		if (OBJECT(current_object)->x_int >= 0xA60)
			return object;
		ride_start_x = 0xA600000u;
	}
	else
		ride_start_x = 0xAE00000u;
	allocate_object(&free_count, &object);
	if (free_count != 0u)
	{
		OBJECT(object)->x = ride_start_x;
		OBJECT(object)->y = 0xEB20000;
		OBJECT(object)->z = 0x5300000;
		OBJECT(object)->handler = ridden_object_start_ADDRESS;
		PLAYER(current_player)->ridden_object = object;
	}
	return object;
}

void player_select_character(uint32_t player_number)
{
	uint8_t playing_count = 0;
	uint32_t player_bit = 0, players_left = 0, status_cursor = 0;

	if (PLAYER(current_player)->substate == 0u)
	{
		PLAYER(current_player)->is_joining = 0xFFu;
		PLAYER(current_player)->substate = 0xFFu;
		PLAYER(current_player)->state = 0x34u;
		status_cursor = player_status_ADDRESS;
		players_left = 3u;
		playing_count = 0u;
		do
		{
			if ((int8_t)mem_read8(status_cursor) < 0 || mem_read8(status_cursor) == 1u || mem_read8(status_cursor) == 4u)
				playing_count++;
			status_cursor++;
			players_left--;
		} while (players_left != 0u);
		if (playing_count == 1u)
			game_frames = 0u;
		PLAYER(current_player)->join_time = game_frames;
		player_number = start_join_character_chooser();
	}
	join_character_chooser_update(player_number);
	player_bit = 0u;
	if (OBJECT(current_object)->player != 0u)
	{
		player_bit = 1u;
		if (OBJECT(current_object)->player != 1u)
			player_bit = 2u;
	}
	if ((AS32(players_in_join_chooser) & shl32(1u, (int)(player_bit))) == 0u)
	{
		PLAYER(current_player)->substate = 0u;
		PLAYER(current_player)->is_joining = 0u;
	}
}

void give_result_bonus(uint32_t player)
{
	uint8_t max_health = 0;
	uint32_t joined_setting = 0, player_object = 0;

	player_object = (player & ~0xFFFFu) | mem_read16(player + 0x14u);
	mem_write8(player_object + 0x7Eu, mem_read8(player_object + 0x7Eu) + 8u);
	joined_setting = mem_read8(player + 0x75u);
	max_health = (uint8_t)max_health_by_setting[(uint16_t)(joined_setting * 2u)];
	if ((int8_t)max_health < (int8_t)mem_read8(player_object + 0x7Eu))
		mem_write8(player_object + 0x7Eu, max_health);
	mem_write32(player + 0x114u, mem_read32(player + 0x114u) | 0x40u);
	mem_write16(player + 0x13Au, mem_read16(player + 0x13Au) + ring_tally_percent * 2u);
}

void spawn_climb_zone_effect(uint32_t object)
{
	uint32_t free_count = 0;

	allocate_object(&free_count, &object);
	if (free_count != 0u)
	{
		OBJECT(object)->sprite_flags = 0x1000u;
		OBJECT(object)->sprite_palette = 0x9130u;
		OBJECT(object)->flags = 0u;
		OBJECT(object)->sprite_offset_x = 0;
		OBJECT(object)->sprite_offset_y = 0;
		OBJECT(object)->handler = climb_zone_effect_start_ADDRESS;
		OBJECT(object)->animation = 0xE459u;
		OBJECT(object)->animation_frame = 0u;
		OBJECT(object)->animation_speed = 3u;
		OBJECT(object)->animation_timer = 0;
		OBJECT(object)->x = 0x1CC0000;
		OBJECT(object)->y = OBJECT(current_object)->y;
		OBJECT(object)->z = 0x4CC0000;
		OBJECT(object)->parent = current_player;
	}
}

void climb_zone_effect_start(void)
{
	uint32_t ignored = 0;

	project_object(&ignored, &ignored);
	OBJECT(current_object)->handler = climb_zone_effect_update_ADDRESS;
	climb_zone_effect_update();
}

void climb_zone_effect_update(void)
{
	uint32_t r7 = 0, r8 = 0, depth = 0, frame = 0, ignored = 0, object = 0;

	object = LINKED(current_object, OBJECT(current_object)->parent);
	if (AS8(OBJECT(object)->draw_next) != 0xAu || mem_read8(object + 0x13Eu) == 0u)
	{
		delete_current_object();
		return;
	}
	OBJECT(current_object)->y = OBJECT((current_object & ~0xFFFFu) | (uint16_t)OBJECT(object)->anchor_x)->y;
	animate(&frame, &r7, &r8);
	project_object(&ignored, &ignored);
	depth = (uint16_t)(OBJECT(current_object)->screen_y + 0x28u);
	queue_sprite_at_depth(&ignored, &depth);
}

void spawn_intro_marker(uint32_t object)
{
	uint32_t free_count = 0;

	allocate_object(&free_count, &object);
	if (free_count != 0u)
	{
		OBJECT(object)->sprite_flags = 0u;
		OBJECT(object)->sprite_frame = 0xA0C0u;
		OBJECT(object)->sprite_palette = 0xAB0u;
		OBJECT(object)->sprite_scale = 0x100u;
		OBJECT(object)->flags = 0u;
		OBJECT(object)->sprite_offset_y = -0x50;
		OBJECT(object)->sprite_offset_x = 8;
		OBJECT(object)->handler = intro_marker_update_ADDRESS;
		OBJECT(object)->animation = 0xE46Du;
		OBJECT(object)->animation_frame = 0u;
		OBJECT(object)->animation_speed = 3u;
		OBJECT(object)->animation_timer = 0;
		OBJECT(object)->parent = current_object;
	}
}

void intro_marker_update(void)
{
	uint32_t r7 = 0, r8 = 0, object = 0, scale = 0;

	if ((int8_t)scene_step >= 2)
	{
		delete_current_object();
		return;
	}
	object = LINKED(current_object, OBJECT(current_object)->parent);
	OBJECT(current_object)->screen_x = OBJECT(object)->screen_x;
	OBJECT(current_object)->screen_y = OBJECT(object)->screen_y;
	OBJECT(current_object)->sprite_scale = OBJECT(object)->sprite_scale;
	scale = OBJECT(current_object)->sprite_scale;
	OBJECT(current_object)->screen_y += (OBJECT(current_object)->sprite_offset_y * scale) >> 8;
	OBJECT(current_object)->screen_x += OBJECT(current_object)->sprite_offset_x;
	animate(&scale, &r7, &r8);
	scale = queue_sprite(scale);
}

void update_hurt_output(void)
{
	uint8_t outputs = 0, seat_bit = 0;
	uint8_t port_value = 0;

	if (setting_moving_seat == 0u || OBJECT(current_object)->player == 2u)
		return;
	port_value = mem_read8(io_port_d);
	outputs = port_value;
	seat_bit = 4u;
	if (OBJECT(current_object)->player != 0u)
		seat_bit = 8u;
	outputs &= ~(uint32_t)seat_bit;
	if (game_in_progress != 0u && (PLAYER(current_player)->state == 0xDu || PLAYER(current_player)->state == 0x20u || PLAYER(current_player)->state == 0x28u || PLAYER(current_player)->state == 0x2Cu || PLAYER(current_player)->state == 0x33u))
		outputs |= seat_bit;
	mem_write8(io_port_d, outputs);
}

void check_player_stalled(void)
{
	if (stage == 5u || PLAYER(current_player)->last_scroll != scroll_position)
	{
		PLAYER(current_player)->last_scroll = scroll_position;
		PLAYER(current_player)->stall_frames = 0;
		return;
	}
	PLAYER(current_player)->stall_frames++;
	if (PLAYER(current_player)->stall_frames >= 0x1C20)
	{
		OBJECT(current_object)->health = 0;
		PLAYER(current_player)->state = 0xDu;
		PLAYER(current_player)->substate = 0u;
	}
}

void check_player_contacts(uint32_t *p_health_limit, uint32_t *p_cursor)
{
	uint32_t health_limit = *p_health_limit;
	uint32_t cursor = *p_cursor;
	uint8_t new_substate = 0, sound_id = 0;
	uint16_t dy = 0, held_frame = 0, saved_sides = 0;
	uint32_t r1 = 0, carried_x = 0, carried_y = 0, distance = 0, draw_depth = 0, dx = 0, dy_2 = 0, ignored = 0, joined_health_setting = 0, move_angle = 0, object = 0, object_2 = 0, push_cos = 0, sides = 0, write_index_address = 0;
	uint8_t attack_power = 0, health = 0, push_out_delay = 0, push_out_delay_2 = 0;
	uint32_t flags = 0, flags_2 = 0;

	if (PLAYER(current_player)->path_request != 0u || all_players_path_request != 0u || (PLAYER(current_player)->path != 0u && (PLAYER(current_player)->path < 0x14 || PLAYER(current_player)->path > 0x16)) || (OBJECT(current_object)->flags & 4u) == 0u)
	{
		PLAYER(current_player)->pushed_out = 0;
		PLAYER(current_player)->last_hazard = 0xFFFFu;
		PLAYER(current_player)->last_damaged = 0xFFFFu;
		OBJECT(current_object)->flags &= ~0x40000u;
		OBJECT(current_object)->flags &= ~0x800000u;
		OBJECT(current_object)->flags &= 0x7FFFFFFFu;
		PLAYER(current_player)->flags &= ~2u;
		*p_health_limit = health_limit;
		return;
	}
	object = LINKED(WORK_RAM, OBJECT(current_object)->touched_by);
	if ((OBJECT(object)->flags & 0x200u) != 0u)
	{
		PLAYER(current_player)->pushed_out = 0;
		PLAYER(current_player)->last_hazard = 0xFFFFu;
		PLAYER(current_player)->last_damaged = 0xFFFFu;
		OBJECT(current_object)->flags &= ~0x40000u;
		OBJECT(current_object)->flags &= ~0x800000u;
		OBJECT(current_object)->flags &= 0x7FFFFFFFu;
		PLAYER(current_player)->flags &= ~2u;
		*p_health_limit = health_limit;
		return;
	}
	if (OBJECT(object)->kind != 8u)
		OBJECT(current_object)->flags &= ~0x40000u;
	switch (OBJECT(object)->kind)
	{
	case 0:
	case 1:
	case 2:
		return;
	case 3:
	case 42:
		PLAYER(current_player)->stage_rings++;
		PLAYER(current_player)->rings++;
		PLAYER(current_player)->flags |= 0x40u;
		switch (health_setting)
		{
		case 0:
		case 4:
			OBJECT(current_object)->health++;
			break;
		case 1:
		case 2:
			health_limit = (health_limit & ~0xFFu) | 4u;
		check_player_contacts_ring_health:
			if (players_in_play >= 2)
				health_limit = (health_limit & ~0xFFFFu) | ((uint16_t)health_limit >> 1);
			PLAYER(current_player)->rings_to_health++;
			if ((int8_t)health_limit <= PLAYER(current_player)->rings_to_health)
			{
				OBJECT(current_object)->health++;
				PLAYER(current_player)->rings_to_health = 0;
			}
			break;
		case 3:
			health_limit = (health_limit & ~0xFFu) | 8u;
			goto check_player_contacts_ring_health;
		default:
			unknown_code(0xE578u + health_setting * 2u);
		}
		joined_health_setting = PLAYER(current_player)->joined_health_setting;
		health_limit = (health_limit & ~0xFFu) | (uint8_t)max_health_by_setting[(uint16_t)(joined_health_setting * 2u)];
		if (OBJECT(current_object)->health <= (int8_t)health_limit)
		{
			PLAYER(current_player)->pushed_out = 0;
			PLAYER(current_player)->last_hazard = 0xFFFFu;
			PLAYER(current_player)->last_damaged = 0xFFFFu;
			OBJECT(current_object)->flags &= ~0x40000u;
			OBJECT(current_object)->flags &= ~0x800000u;
			OBJECT(current_object)->flags &= 0x7FFFFFFFu;
			PLAYER(current_player)->flags &= ~2u;
			*p_health_limit = health_limit;
			return;
		}
		OBJECT(current_object)->health = health_limit;
		PLAYER(current_player)->pushed_out = 0;
		PLAYER(current_player)->last_hazard = 0xFFFFu;
		PLAYER(current_player)->last_damaged = 0xFFFFu;
		OBJECT(current_object)->flags &= ~0x40000u;
		OBJECT(current_object)->flags &= ~0x800000u;
		OBJECT(current_object)->flags &= 0x7FFFFFFFu;
		PLAYER(current_player)->flags &= ~2u;
		*p_health_limit = health_limit;
		return;
	case 4:
	case 15:
	case 16:
	case 22:
	check_player_contacts_hurt_unless_invulnerable:
		if (PLAYER(current_player)->invulnerable_timer >= 0)
		{
			PLAYER(current_player)->pushed_out = 0;
			PLAYER(current_player)->last_hazard = 0xFFFFu;
			PLAYER(current_player)->last_damaged = 0xFFFFu;
			OBJECT(current_object)->flags &= ~0x40000u;
			OBJECT(current_object)->flags &= ~0x800000u;
			OBJECT(current_object)->flags &= 0x7FFFFFFFu;
			PLAYER(current_player)->flags &= ~2u;
			*p_health_limit = health_limit;
			return;
		}
	case 9:
	case 13:
	case 24:
	case 40:
	check_player_contacts_hurt:
		new_substate = 0u;
	check_player_contacts_attack_or_hurt:
		if (PLAYER(current_player)->state == 4u)
		{
		check_player_contacts_attack:
			if (PLAYER(current_player)->last_damaged == (uint16_t)object)
			{
				*p_health_limit = health_limit;
				return;
			}
			PLAYER(current_player)->last_damaged = object;
			OBJECT(object)->flags |= 0x40u;
			attack_power = PLAYER(current_player)->attack_power;
			health = OBJECT(object)->health;
			OBJECT(object)->health = health - attack_power;
			if ((int8_t)attack_power < (int8_t)health || OBJECT(object)->kind == 0xCu || OBJECT(object)->kind == 9u || OBJECT(object)->kind == 0xBu || OBJECT(object)->kind == 0xDu || OBJECT(object)->kind == 0xEu || OBJECT(object)->kind == 0x11u || OBJECT(object)->kind == 0x18u || OBJECT(object)->kind == 0x19u)
			{
				OBJECT(current_object)->flags |= 0x2000u;
				if ((OBJECT(object)->flags & 0x40000000u) == 0u)
				{
					if ((uint16_t)OBJECT(current_object)->speed_int != 0u)
						move_angle = (uint16_t)(OBJECT(current_object)->direction + 0x200u);
					else
						move_angle = OBJECT(object)->direction;
					OBJECT(current_object)->direction = move_angle & 0x3FFu;
				}
				else
					OBJECT(current_object)->direction = OBJECT(object)->direction;
				face_movement_direction(move_angle);
				OBJECT(current_object)->speed = 0x4000000;
				OBJECT(current_object)->speed_z = -0x18000;
				PLAYER(current_player)->state_timer = 6;
				OBJECT(current_object)->x = PLAYER(current_player)->previous_x;
				OBJECT(current_object)->y = PLAYER(current_player)->previous_y;
				*p_health_limit = health_limit;
				return;
			}
			PLAYER(current_player)->hit_stop = 3u;
			*p_health_limit = health_limit;
			return;
		}
		if ((uint16_t)object != PLAYER(current_player)->last_hazard)
		{
			PLAYER(current_player)->substate = new_substate;
			PLAYER(current_player)->last_hazard = object;
			PLAYER(current_player)->hurt_contact_delay = 2;
		}
		PLAYER(current_player)->state = 0xDu;
		return;
	case 5:
		if (PLAYER(current_player)->invulnerable_timer >= 0)
		{
			PLAYER(current_player)->pushed_out = 0;
			PLAYER(current_player)->last_hazard = 0xFFFFu;
			PLAYER(current_player)->last_damaged = 0xFFFFu;
			OBJECT(current_object)->flags &= ~0x40000u;
			OBJECT(current_object)->flags &= ~0x800000u;
			OBJECT(current_object)->flags &= 0x7FFFFFFFu;
			PLAYER(current_player)->flags &= ~2u;
			*p_health_limit = health_limit;
			return;
		}
	case 12:
	case 32:
	case 33:
	check_player_contacts_solid:
		if (PLAYER(current_player)->state == 4u)
			goto check_player_contacts_attack;
		if (PLAYER(current_player)->state == 0xAu)
		{
			if (PLAYER(current_player)->invulnerable_timer >= 0)
			{
				PLAYER(current_player)->pushed_out = 0;
				PLAYER(current_player)->last_hazard = 0xFFFFu;
				PLAYER(current_player)->last_damaged = 0xFFFFu;
				OBJECT(current_object)->flags &= ~0x40000u;
				OBJECT(current_object)->flags &= ~0x800000u;
				OBJECT(current_object)->flags &= 0x7FFFFFFFu;
				PLAYER(current_player)->flags &= ~2u;
				*p_health_limit = health_limit;
				return;
			}
			if ((uint8_t)PLAYER(current_player)->pushed_out != 0u)
				return;
			if (PLAYER(current_player)->last_hazard != (uint16_t)object)
			{
				PLAYER(current_player)->push_out_delay = 0x1E;
				PLAYER(current_player)->last_hazard = object;
			}
			push_out_delay = PLAYER(current_player)->push_out_delay;
			PLAYER(current_player)->push_out_delay = push_out_delay - 1u;
			if ((int8_t)push_out_delay <= 1)
				PLAYER(current_player)->pushed_out = -1;
			PLAYER(current_player)->screen_x = PLAYER(current_player)->saved_screen_x;
			OBJECT(current_object)->projected_y = PLAYER(current_player)->saved_screen_y;
			OBJECT(current_object)->z = PLAYER(current_player)->previous_z;
			PLAYER(current_player)->climb_remaining = PLAYER(current_player)->saved_climb_remaining;
			OBJECT(current_object)->animation_speed = 0xFFu;
			OBJECT(current_object)->animation_frame = 0u;
			PLAYER(current_player)->step_accumulator = 0;
			PLAYER(current_player)->side_step_accumulator = 0;
			return;
		}
		if (PLAYER(current_player)->state == 0x2Fu)
			return;
		if (PLAYER(current_player)->pushed_out < 0)
		{
			if ((uint8_t)PLAYER(current_player)->pushed_out == 0u)
			{
				aim_at_screen_centre(&health_limit, &dx, &dy_2);
				PLAYER(current_player)->push_out_x = dx;
				PLAYER(current_player)->push_out_y = dy_2;
				OBJECT(current_object)->speed = 0;
				PLAYER(current_player)->ground_speed = 0;
			}
			PLAYER(current_player)->pushed_out = -1;
			PLAYER(current_player)->previous_x = OBJECT(current_object)->x;
			PLAYER(current_player)->previous_y = OBJECT(current_object)->y;
			OBJECT(current_object)->x += PLAYER(current_player)->push_out_x;
			OBJECT(current_object)->y += PLAYER(current_player)->push_out_y;
			PLAYER(current_player)->jump_delay = 2;
			PLAYER(current_player)->state = 0u;
			*p_health_limit = health_limit;
			return;
		}
		if (PLAYER(current_player)->last_hazard != (uint16_t)object)
		{
			PLAYER(current_player)->push_out_delay = 0x1E;
			PLAYER(current_player)->last_hazard = object;
			OBJECT(current_object)->flags |= 0x1000u;
			OBJECT(current_object)->x = PLAYER(current_player)->previous_x;
			OBJECT(current_object)->y = PLAYER(current_player)->previous_y;
			OBJECT(current_object)->z = PLAYER(current_player)->previous_z;
			OBJECT(current_object)->floor_block = OBJECT(current_object)->previous_floor_block;
			OBJECT(current_object)->floor_z = OBJECT(current_object)->previous_floor_z;
			if (OBJECT(object)->kind != 0x20u && OBJECT(current_object)->speed_int >= 0x500)
			{
				AS16(PLAYER(current_player)->bounce_direction) = ((OBJECT(current_object)->direction + 0x80u) & 0x3FFu) >> 8;
				PLAYER(current_player)->state = 0xEu;
				PLAYER(current_player)->substate = 0u;
				return;
			}
			OBJECT(current_object)->speed = 0;
			OBJECT(current_object)->speed_x = 0;
			OBJECT(current_object)->speed_y = 0;
			PLAYER(current_player)->control_lock = 0u;
			OBJECT(current_object)->flags |= 0x800000u;
			PLAYER(current_player)->free_turn_timer = 3u;
			PLAYER(current_player)->state = 0u;
			return;
		}
		push_out_delay_2 = PLAYER(current_player)->push_out_delay;
		PLAYER(current_player)->push_out_delay = push_out_delay_2 - 1u;
		if ((int8_t)push_out_delay_2 <= 1)
		{
			if ((uint8_t)PLAYER(current_player)->pushed_out == 0u)
			{
				aim_at_screen_centre(&health_limit, &dx, &dy_2);
				PLAYER(current_player)->push_out_x = dx;
				PLAYER(current_player)->push_out_y = dy_2;
				OBJECT(current_object)->speed = 0;
				PLAYER(current_player)->ground_speed = 0;
			}
			PLAYER(current_player)->pushed_out = -1;
			PLAYER(current_player)->previous_x = OBJECT(current_object)->x;
			PLAYER(current_player)->previous_y = OBJECT(current_object)->y;
			OBJECT(current_object)->x += PLAYER(current_player)->push_out_x;
			OBJECT(current_object)->y += PLAYER(current_player)->push_out_y;
			PLAYER(current_player)->jump_delay = 2;
			PLAYER(current_player)->state = 0u;
			*p_health_limit = health_limit;
			return;
		}
		OBJECT(current_object)->flags |= 0x1000u;
		OBJECT(current_object)->x = PLAYER(current_player)->previous_x;
		OBJECT(current_object)->y = PLAYER(current_player)->previous_y;
		OBJECT(current_object)->z = PLAYER(current_player)->previous_z;
		OBJECT(current_object)->floor_block = OBJECT(current_object)->previous_floor_block;
		OBJECT(current_object)->floor_z = OBJECT(current_object)->previous_floor_z;
		if (OBJECT(object)->kind != 0x20u && OBJECT(current_object)->speed_int >= 0x500)
		{
			AS16(PLAYER(current_player)->bounce_direction) = ((OBJECT(current_object)->direction + 0x80u) & 0x3FFu) >> 8;
			PLAYER(current_player)->state = 0xEu;
			PLAYER(current_player)->substate = 0u;
			return;
		}
		OBJECT(current_object)->speed = 0;
		OBJECT(current_object)->speed_x = 0;
		OBJECT(current_object)->speed_y = 0;
		PLAYER(current_player)->control_lock = 0u;
		OBJECT(current_object)->flags |= 0x800000u;
		PLAYER(current_player)->free_turn_timer = 3u;
		PLAYER(current_player)->state = 0u;
		return;
	case 6:
		new_substate = 0u;
		object_2 = LINKED(current_object, OBJECT(current_object)->touched_by);
		if (AS8(OBJECT(object_2)->speed_x) != 1u)
		{
			*p_cursor = object_2;
			return;
		}
		PLAYER(current_player)->state = 0xFu;
		PLAYER(current_player)->substate = new_substate;
		*p_cursor = object_2;
		return;
	case 7:
		new_substate = 1u;
		object_2 = LINKED(current_object, OBJECT(current_object)->touched_by);
		if (AS8(OBJECT(object_2)->speed_x) != 1u)
		{
			*p_cursor = object_2;
			return;
		}
		PLAYER(current_player)->state = 0xFu;
		PLAYER(current_player)->substate = new_substate;
		*p_cursor = object_2;
		return;
	case 8:
	case 10:
		if ((OBJECT(current_object)->flags & 0x200000u) != 0u || PLAYER(current_player)->state != 4u)
			return;
		PLAYER(current_player)->holder = OBJECT(current_object)->touched_by;
		PLAYER(current_player)->state = 0x15u;
		PLAYER(current_player)->substate = 0u;
		OBJECT(current_object)->sprite_flags &= 0xFFBFu;
		held_frame = 0x1B10u;
		if (OBJECT(current_object)->kind != 0u)
		{
			held_frame = 0x5230u;
			if (OBJECT(current_object)->kind != 1u)
				held_frame = 0x3678u;
		}
		OBJECT(current_object)->sprite_frame = held_frame;
		OBJECT(current_object)->sprite_palette = mem_read16(OBJECT(current_object)->check_player.palette_list + 2u);
		OBJECT(current_object)->flags &= ~0x40u;
		OBJECT(current_object)->flags &= ~0x2000u;
		return;
	case 11:
		new_substate = 1u;
		goto check_player_contacts_attack_or_hurt;
	case 14:
		new_substate = 7u;
		if ((uint16_t)object != PLAYER(current_player)->last_hazard)
		{
			PLAYER(current_player)->substate = new_substate;
			PLAYER(current_player)->last_hazard = object;
			PLAYER(current_player)->hurt_contact_delay = 2;
		}
		PLAYER(current_player)->state = 0xDu;
		return;
	case 17:
		if (PLAYER(current_player)->state == 4u)
			goto check_player_contacts_attack;
		if (OBJECT(current_object)->speed_int >= 0x200)
		{
			PLAYER(current_player)->state = 0x1Du;
			PLAYER(current_player)->substate = 0u;
			return;
		}
		goto check_player_contacts_solid;
	case 18:
		if (PLAYER(current_player)->state == 0x2Fu)
			return;
		PLAYER(current_player)->state = 0x2Fu;
		PLAYER(current_player)->substate = 0u;
		return;
	case 19:
		PLAYER(current_player)->state = 0x1Fu;
		PLAYER(current_player)->substate = 0u;
		return;
	case 20:
		if ((uint16_t)object == PLAYER(current_player)->last_hazard)
		{
			PLAYER(current_player)->state = 0x20u;
			return;
		}
		PLAYER(current_player)->substate = 0u;
		PLAYER(current_player)->last_hazard = object;
		PLAYER(current_player)->hurt_contact_delay = 2;
		PLAYER(current_player)->state = 0x20u;
		return;
	case 21:
		new_substate = 7u;
		if (PLAYER(current_player)->invulnerable_timer >= 0)
		{
			PLAYER(current_player)->pushed_out = 0;
			PLAYER(current_player)->last_hazard = 0xFFFFu;
			PLAYER(current_player)->last_damaged = 0xFFFFu;
			OBJECT(current_object)->flags &= ~0x40000u;
			OBJECT(current_object)->flags &= ~0x800000u;
			OBJECT(current_object)->flags &= 0x7FFFFFFFu;
			PLAYER(current_player)->flags &= ~2u;
			*p_health_limit = health_limit;
			return;
		}
		if ((uint16_t)object != PLAYER(current_player)->last_hazard)
		{
			PLAYER(current_player)->substate = new_substate;
			PLAYER(current_player)->last_hazard = object;
			PLAYER(current_player)->hurt_contact_delay = 2;
		}
		PLAYER(current_player)->state = 0xDu;
		return;
	case 23:
		if (PLAYER(current_player)->invulnerable_timer >= 0)
		{
			PLAYER(current_player)->pushed_out = 0;
			PLAYER(current_player)->last_hazard = 0xFFFFu;
			PLAYER(current_player)->last_damaged = 0xFFFFu;
			OBJECT(current_object)->flags &= ~0x40000u;
			OBJECT(current_object)->flags &= ~0x800000u;
			OBJECT(current_object)->flags &= 0x7FFFFFFFu;
			PLAYER(current_player)->flags &= ~2u;
			*p_health_limit = health_limit;
			return;
		}
		if ((uint16_t)object == PLAYER(current_player)->last_hazard)
		{
			PLAYER(current_player)->state = 0x20u;
			return;
		}
		PLAYER(current_player)->substate = 0u;
		PLAYER(current_player)->last_hazard = object;
		PLAYER(current_player)->hurt_contact_delay = 2;
		PLAYER(current_player)->state = 0x20u;
		return;
	case 25:
		if (PLAYER(current_player)->invulnerable_timer >= 0)
		{
			PLAYER(current_player)->pushed_out = 0;
			PLAYER(current_player)->last_hazard = 0xFFFFu;
			PLAYER(current_player)->last_damaged = 0xFFFFu;
			OBJECT(current_object)->flags &= ~0x40000u;
			OBJECT(current_object)->flags &= ~0x800000u;
			OBJECT(current_object)->flags &= 0x7FFFFFFFu;
			PLAYER(current_player)->flags &= ~2u;
			*p_health_limit = health_limit;
			return;
		}
		contact_saved_x = OBJECT(current_object)->x;
		contact_saved_y = OBJECT(current_object)->y;
		OBJECT(current_object)->x = PLAYER(current_player)->previous_x;
		OBJECT(current_object)->y = PLAYER(current_player)->previous_y;
		OBJECT(current_object)->z = PLAYER(current_player)->previous_z;
		OBJECT(current_object)->floor_block = OBJECT(current_object)->previous_floor_block;
		OBJECT(current_object)->floor_z = OBJECT(current_object)->previous_floor_z;
		side_of_box(object, &health_limit, &sides);
		contact_box_side = sides;
		if ((uint16_t)sides == 8u || (uint16_t)sides == 0u || PLAYER(current_player)->state != 4u)
		{
			OBJECT(current_object)->flags |= 0x1000u;
			saved_sides = contact_box_side;
			if (saved_sides == 0u)
			{
			check_player_contacts_move_with_object:
				carried_x = OBJECT(object)->speed_x;
				carried_y = OBJECT(object)->speed_y * 2u;
				unproject_point(&carried_x, &carried_y);
				OBJECT(current_object)->x += carried_x;
				OBJECT(current_object)->x += carried_x;
				PLAYER(current_player)->previous_x = OBJECT(current_object)->x;
				OBJECT(current_object)->y += carried_y;
				PLAYER(current_player)->previous_y = OBJECT(current_object)->y;
			}
			else
			{
				if (saved_sides == 8u)
				{
					OBJECT(current_object)->x = contact_saved_x;
					OBJECT(current_object)->y = contact_saved_y;
					goto check_player_contacts_move_with_object;
				}
				if (OBJECT(current_object)->speed_int >= 0x500)
				{
					AS16(PLAYER(current_player)->bounce_direction) = ((OBJECT(current_object)->direction + 0x80u) & 0x3FFu) >> 8;
					PLAYER(current_player)->state = 0xEu;
					PLAYER(current_player)->substate = 0u;
					*p_health_limit = health_limit;
					return;
				}
			}
			OBJECT(current_object)->speed = 0;
			OBJECT(current_object)->speed_x = 0;
			OBJECT(current_object)->speed_y = 0;
			PLAYER(current_player)->control_lock = 0u;
			OBJECT(current_object)->flags |= 0x800000u;
			PLAYER(current_player)->free_turn_timer = 3u;
			PLAYER(current_player)->state = 0u;
			PLAYER(current_player)->state_timer = -1;
			OBJECT(current_object)->flags &= ~0x2000u;
			OBJECT(current_object)->sprite_palette = mem_read16(OBJECT(current_object)->check_player.palette_list + 2u);
			OBJECT(current_object)->flags &= ~0x40u;
			OBJECT(current_object)->flags &= ~0x100000u;
			OBJECT(current_object)->flags &= ~0x200000u;
			*p_health_limit = health_limit;
			return;
		}
		goto check_player_contacts_attack;
	case 26:
		object_2 = LINKED(current_object, OBJECT(object)->parent);
		if (OBJECT(object_2)->var_82 != 0x300u)
		{
			*p_cursor = object_2;
			return;
		}
		PLAYER(current_player)->state = 0x27u;
		PLAYER(current_player)->substate = 0u;
		PLAYER(current_player)->climb_remaining = OBJECT(object)->spawn_arg_2;
		*p_cursor = object_2;
		return;
	case 27:
		OBJECT(current_object)->flags |= 0x20000u;
		PLAYER(current_player)->flags &= ~2u;
		PLAYER(current_player)->flags &= ~0x80u;
		if (PLAYER(current_player)->state == 7u || PLAYER(current_player)->state == 8u)
			return;
		if ((uint32_t)OBJECT(current_object)->speed_x == 0u && (uint32_t)OBJECT(current_object)->speed_y == 0u)
		{
			OBJECT(current_object)->speed = 0;
			OBJECT(current_object)->direction = OBJECT(object)->direction;
		}
		if (PLAYER(current_player)->state == 0xCu && (uint16_t)OBJECT(current_object)->speed_int == 0u)
		{
			OBJECT(current_object)->speed = 0x1000000;
			set_speed_from_direction();
		}
		PLAYER(current_player)->previous_state = PLAYER(current_player)->state;
		PLAYER(current_player)->state = 7u;
		PLAYER(current_player)->substate = 1u;
		OBJECT(current_object)->floor_block = -1;
		OBJECT(current_object)->floor_z = 0x8000000;
		PLAYER(current_player)->ledge_pullback = 0;
		PLAYER(current_player)->slip_distance = 0;
		PLAYER(current_player)->control_lock = 0u;
		OBJECT(current_object)->flags &= ~0x100u;
		OBJECT(current_object)->sprite_palette = mem_read16(OBJECT(current_object)->check_player.palette_list + 2u);
		OBJECT(current_object)->flags &= ~0x40u;
		OBJECT(current_object)->flags &= ~0x2000u;
		PLAYER(current_player)->heading = OBJECT(object)->direction;
		OBJECT(current_object)->speed_x = OBJECT(current_object)->speed_x * 2u;
		OBJECT(current_object)->speed_y = OBJECT(current_object)->speed_y * 2u;
		PLAYER(current_player)->ledge_side = OBJECT(object)->direction >> 8;
		draw_depth = (int32_t)((uint32_t)OBJECT(current_object)->projected_y + OBJECT(current_object)->previous_floor_z) >> 16;
		draw_depth = (draw_depth & ~0xFFFFu) | (uint16_t)(draw_depth - view_y);
		PLAYER(current_player)->fall_draw_depth = draw_depth;
		*p_health_limit = draw_depth;
		return;
	case 28:
		if (PLAYER(current_player)->invulnerable_timer >= 0)
		{
			PLAYER(current_player)->pushed_out = 0;
			PLAYER(current_player)->last_hazard = 0xFFFFu;
			PLAYER(current_player)->last_damaged = 0xFFFFu;
			OBJECT(current_object)->flags &= ~0x40000u;
			OBJECT(current_object)->flags &= ~0x800000u;
			OBJECT(current_object)->flags &= 0x7FFFFFFFu;
			PLAYER(current_player)->flags &= ~2u;
			*p_health_limit = health_limit;
			return;
		}
		flags = OBJECT(current_object)->flags;
		OBJECT(current_object)->flags = flags | 0x80000000u;
		if ((flags & 0x80000000u) == 0u)
		{
			sound_id = 0x36u;
			if (OBJECT(current_object)->kind != 0u)
			{
				sound_id = 0x46u;
				if (OBJECT(current_object)->kind != 1u)
					sound_id = 0x56u;
			}
			play_sound(sound_id, &health_limit, &write_index_address);
		}
		distance = (uint16_t)OBJECT(current_object)->touch_distance;
		square_root(&distance, &r1);
		OBJECT(current_object)->touch_distance = distance;
		if (OBJECT(current_object)->touch_distance <= 8)
		{
			flags_2 = PLAYER(current_player)->flags;
			PLAYER(current_player)->flags = flags_2 | 4u;
			if ((flags_2 & 4u) == 0u)
			{
				PLAYER(current_player)->sink_depth = 0;
				PLAYER(current_player)->mishap_flag = 0xFFu;
				sound_id = 0x35u;
				if (OBJECT(current_object)->kind != 0u)
				{
					sound_id = 0x31u;
					if (OBJECT(current_object)->kind != 1u)
						sound_id = 0x3Du;
				}
				play_sound(sound_id, &r1, &write_index_address);
				cursor = spawn_sink_swirl(cursor);
			}
		}
		object = LINKED(current_object, OBJECT(current_object)->touched_by);
		move_angle = (uint16_t)(OBJECT(object)->x_int - OBJECT(current_object)->x_int);
		dy = (uint16_t)(OBJECT(object)->y_int - OBJECT(current_object)->y_int);
		angle_of_vector(dy, &move_angle, &ignored);
		move_angle = (uint16_t)(move_angle - 0x80u) & 0x3FFu;
		PLAYER(current_player)->push_x = (int32_t)(sine_table[move_angle] * 0x6000u) >> 16;
		push_cos = (int32_t)(cosine_table[move_angle] * 0x6000u) >> 16;
		PLAYER(current_player)->push_y = 0u - push_cos;
		*p_health_limit = push_cos;
		*p_cursor = cursor;
		return;
	case 29:
		PLAYER(current_player)->flags |= 2u;
		OBJECT(current_object)->flags &= ~0x20000u;
		return;
	case 30:
		if ((PLAYER(current_player)->flags & 4u) != 0u)
			return;
		PLAYER(current_player)->state = 0x28u;
		PLAYER(current_player)->substate = 0u;
		return;
	case 31:
		if (PLAYER(current_player)->invulnerable_timer >= 0)
		{
			PLAYER(current_player)->pushed_out = 0;
			PLAYER(current_player)->last_hazard = 0xFFFFu;
			PLAYER(current_player)->last_damaged = 0xFFFFu;
			OBJECT(current_object)->flags &= ~0x40000u;
			OBJECT(current_object)->flags &= ~0x800000u;
			OBJECT(current_object)->flags &= 0x7FFFFFFFu;
			PLAYER(current_player)->flags &= ~2u;
			*p_health_limit = health_limit;
			return;
		}
		if (PLAYER(current_player)->state == 0x29u || PLAYER(current_player)->state == 5u || PLAYER(current_player)->state == 7u || PLAYER(current_player)->state == 0xDu || OBJECT(current_object)->floor_type == 0xAu || OBJECT(current_object)->floor_type == 0xBu)
			return;
		PLAYER(current_player)->state = 0x29u;
		PLAYER(current_player)->substate = 0u;
		return;
	case 34:
		if (PLAYER(current_player)->invulnerable_timer < 0)
		{
			PLAYER(current_player)->state = 0x1Fu;
			PLAYER(current_player)->substate = 0u;
			return;
		}
		PLAYER(current_player)->pushed_out = 0;
		PLAYER(current_player)->last_hazard = 0xFFFFu;
		PLAYER(current_player)->last_damaged = 0xFFFFu;
		OBJECT(current_object)->flags &= ~0x40000u;
		OBJECT(current_object)->flags &= ~0x800000u;
		OBJECT(current_object)->flags &= 0x7FFFFFFFu;
		PLAYER(current_player)->flags &= ~2u;
		*p_health_limit = health_limit;
		return;
	case 35:
		PLAYER(current_player)->state = 0x2Cu;
		PLAYER(current_player)->substate = 0u;
		return;
	case 36:
		new_substate = 0u;
		if (PLAYER(current_player)->invulnerable_timer >= 0)
		{
			PLAYER(current_player)->pushed_out = 0;
			PLAYER(current_player)->last_hazard = 0xFFFFu;
			PLAYER(current_player)->last_damaged = 0xFFFFu;
			OBJECT(current_object)->flags &= ~0x40000u;
			OBJECT(current_object)->flags &= ~0x800000u;
			OBJECT(current_object)->flags &= 0x7FFFFFFFu;
			PLAYER(current_player)->flags &= ~2u;
			*p_health_limit = health_limit;
			return;
		}
		if ((uint16_t)object != PLAYER(current_player)->last_hazard)
		{
			PLAYER(current_player)->substate = new_substate;
			PLAYER(current_player)->last_hazard = object;
			PLAYER(current_player)->hurt_contact_delay = 2;
		}
		PLAYER(current_player)->state = 0xDu;
		return;
	case 37:
		PLAYER(current_player)->state = 0x2Du;
		PLAYER(current_player)->substate = 0u;
		return;
	case 38:
		PLAYER(current_player)->state = 0x36u;
		PLAYER(current_player)->substate = 0u;
		return;
	case 39:
		OBJECT(current_object)->flags |= 0x1000u;
		goto check_player_contacts_hurt_unless_invulnerable;
	case 41:
		OBJECT(current_object)->flags |= 0x1000u;
		goto check_player_contacts_hurt;
	default:
		unknown_code(0xE4DFu + OBJECT(object)->kind * 3u);
	}
}

void vine_init(void)
{
	uint32_t block = 0, free_count = 0, ignored = 0, object = 0, object_2 = 0, object_3 = 0, piece_count = 0, spawn_result = 0;
	uint16_t speed_x_fraction = 0;

	OBJECT(current_object)->flags = 0u;
	OBJECT(current_object)->sprite_offset_x = 0;
	OBJECT(current_object)->sprite_offset_y = 0;
	OBJECT(current_object)->speed_z = 0x3FF;
	OBJECT(current_object)->vine_init.base_x = OBJECT(current_object)->x;
	OBJECT(current_object)->speed = (uint32_t)OBJECT(current_object)->vine_init.phase_step << 16;
	OBJECT(current_object)->speed_x_int = OBJECT(current_object)->vine_init.segment_count;
	OBJECT(current_object)->speed_x_fraction = OBJECT(current_object)->speed_x_int;
	OBJECT(current_object)->vine_init.depth_offset = OBJECT(current_object)->speed_x_int * 8u;
	spawn_count = OBJECT(current_object)->speed_x_int;
	spawn_count--;
	add_pieces_to_current_object(&spawn_result, &free_count, &object_2, &block);
	if ((uint8_t)spawn_result == 0u)
		return;
	object_3 = block;
	spawn_count = 2u;
	spawn_object_with_pieces(0x00ECFFu, &spawn_result, &piece_count, &free_count, &object_2, &block);
	if ((uint8_t)spawn_result == 0u)
		return;
	OBJECT(object_3)->child = object_2;
	OBJECT(object_2)->parent = object_3;
	OBJECT(object_2)->direction = object_cursor;
	OBJECT(object_2)->hitbox_size_x = 0x14u;
	OBJECT(object_2)->hitbox_height = 8u;
	OBJECT(object_2)->kind = 0xAu;
	OBJECT(object_2)->vine_bar.anchor_z = OBJECT(object_cursor)->z_int;
	OBJECT(object_2)->handler = vine_bar_update_ADDRESS;
	object_2 = LINKED(object_2, OBJECT(object_2)->child);
	OBJECT(object_2)->sprite_scale = 0x100u;
	OBJECT(object_2)->sprite_frame = 0x5D90u;
	OBJECT(object_2)->sprite_palette = 0x7A0u;
	OBJECT(object_2)->sprite_flags = 8u;
	OBJECT(current_object)->handler = vine_swing_ADDRESS;
	OBJECT(current_object)->speed_x_fraction = OBJECT(current_object)->speed_x_int;
	current_player = current_object;
	object = LINKED(block, OBJECT(current_object)->child);
	do
	{
		OBJECT(object)->projected_y = OBJECT(object_cursor)->projected_y;
		OBJECT(object)->sprite_flags = 0u;
		OBJECT(object)->sprite_palette = 0x8350u;
		OBJECT(object)->sprite_frame = 0x6E68u;
		OBJECT(current_player)->child = object;
		OBJECT(object)->parent = current_player;
		current_player = (current_player & ~0xFFFFu) | (uint16_t)object;
		object = LINKED(object, OBJECT(object)->child);
		speed_x_fraction = OBJECT(current_object)->speed_x_fraction;
		OBJECT(current_object)->speed_x_fraction = speed_x_fraction - 1u;
	} while ((int16_t)speed_x_fraction > 1);
	OBJECT(current_player)->sprite_frame = 0x6E70u;
	OBJECT(current_player)->sprite_palette = 0x10E0u;
	OBJECT(current_object)->floor_z = OBJECT(current_object)->z;
	project_object(&ignored, &ignored);
}

void vine_swing(void)
{
	uint8_t bucket = 0;
	uint16_t screen_x = 0;
	uint32_t block_offset = 0, ignored = 0, in_front = 0, point_x = 0, point_y = 0, segment_angle_int = 0, segment_depth = 0, slope_height = 0, z = 0;
	uint16_t speed_x_fraction = 0;

	OBJECT(current_object)->vine_swing.holding = 1u;
	if (vines_hold_players == 0u && OBJECT(current_object)->speed_z_int >= 0x9F && OBJECT(current_object)->speed_z_int <= 0x110)
		OBJECT(current_object)->vine_swing.holding = 0u;
	screen_x = (uint16_t)OBJECT(current_object)->screen_x;
	if ((int16_t)screen_x >= 0x200 || (int16_t)screen_x <= -0x200)
	{
		delete_object_and_children(&ignored, &block_offset, &slope_height);
		vines_hold_players = 0xFFu;
		return;
	}
	OBJECT(current_object)->speed_z += OBJECT(current_object)->speed;
	OBJECT(current_object)->speed_z_int &= 0x3FFu;
	OBJECT(current_object)->speed_y = (uint32_t)sine_table[(uint16_t)OBJECT(current_object)->speed_z_int] * OBJECT(current_object)->vine_swing.curl_scale;
	OBJECT(current_object)->z = OBJECT(current_object)->floor_z;
	update_screen_position(&ignored, &segment_depth);
	in_front = 0u;
	if (OBJECT(object_cursor)->vine_swing.segment_angle_int >= 0x100 && OBJECT(object_cursor)->vine_swing.segment_angle_int < 0x300)
		in_front = 1u;
	OBJECT(current_object)->vine_swing.segment_angle = 0u;
	OBJECT(current_object)->x = OBJECT(current_object)->vine_swing.base_x;
	OBJECT(current_object)->speed_x_fraction = OBJECT(current_object)->speed_x_int;
	OBJECT(object_cursor)->vine_swing.segment_angle += OBJECT(object_cursor)->speed_y;
	do
	{
		current_object = LINKED(current_object, OBJECT(current_object)->child);
		OBJECT(object_cursor)->vine_swing.segment_angle += OBJECT(object_cursor)->speed_y;
		OBJECT(object_cursor)->vine_swing.segment_angle_int &= 0x3FFu;
		segment_angle_int = (uint16_t)OBJECT(object_cursor)->vine_swing.segment_angle_int;
		OBJECT(object_cursor)->x += sine_table[segment_angle_int] * 0x18u;
		point_x = OBJECT(object_cursor)->x;
		OBJECT(object_cursor)->z += cosine_table[segment_angle_int] * 0x18u;
		z = OBJECT(object_cursor)->z;
		point_y = OBJECT(object_cursor)->y;
		project_point(z, &ignored, &point_x, &point_y);
		OBJECT(current_object)->screen_x = point_x;
		OBJECT(current_object)->screen_y = point_y;
		if (in_front == 0u)
		{
			segment_depth = (uint16_t)(OBJECT(object_cursor)->screen_y - (OBJECT(object_cursor)->x_int - OBJECT(object_cursor)->vine_swing.base_x_int) + OBJECT(object_cursor)->vine_swing.depth_offset);
			queue_sprite_at_depth(&ignored, &segment_depth);
		}
		else
		{
			bucket = 0u;
			queue_sprite_in_bucket(bucket);
		}
		speed_x_fraction = OBJECT(object_cursor)->speed_x_fraction;
		OBJECT(object_cursor)->speed_x_fraction = speed_x_fraction - 1u;
	} while ((int16_t)speed_x_fraction > 1);
}

void vine_bar_update(void)
{
	uint16_t kind = 0, screen_dy = 0, screen_x = 0, x_int = 0, y_int = 0, z = 0;
	uint32_t r11 = 0, depth = 0, height = 0, ignored = 0, vine = 0;

	OBJECT(current_object)->vine_bar.holding = mem_read8((0x200000u | OBJECT(current_object)->direction) + 0x80u);
	if ((OBJECT(current_object)->flags & 8u) != 0u && OBJECT(current_object)->floor_z_int < 0x800)
	{
		current_object = LINKED(current_object, OBJECT(object_cursor)->child);
		OBJECT(current_object)->screen_x = OBJECT(object_cursor)->screen_x;
		OBJECT(current_object)->screen_y = OBJECT(object_cursor)->screen_y;
		OBJECT(current_object)->screen_y += OBJECT(object_cursor)->floor_z_int - OBJECT(object_cursor)->z_int;
		queue_sprite(0u);
		current_object = object_cursor;
	}
	vine = 0x200000u | OBJECT(current_object)->direction;
	OBJECT(current_object)->x = mem_read32(vine + 0x4Cu);
	OBJECT(current_object)->y = mem_read32(vine + 0x50u);
	OBJECT(current_object)->z = mem_read32(vine + 0x40u);
	screen_x = (uint16_t)OBJECT(current_object)->screen_x;
	screen_dy = (uint16_t)OBJECT(current_object)->screen_y;
	project_object(&ignored, &depth);
	screen_dy -= OBJECT(current_object)->screen_y;
	OBJECT(current_object)->projected_x += screen_x - OBJECT(current_object)->screen_x;
	OBJECT(current_object)->projected_y_int += screen_dy;
	unproject_object(&ignored, &ignored, &ignored, &r11);
	height = (OBJECT(object_cursor)->floor_z_int - OBJECT(object_cursor)->z_int) & 0x7Fu;
	if ((int16_t)height > 0x50)
	{
		AT8(OBJECT(object_cursor)->hitbox_size_y, 1) = 0u;
		OBJECT(object_cursor)->hitbox_height = 8u;
	}
	else
	{
		AT8(OBJECT(object_cursor)->hitbox_size_y, 1) = height;
		OBJECT(object_cursor)->hitbox_height = height;
	}
	OBJECT(current_object)->health = 0x7F;
	add_to_touchable_list(height);
	add_to_rider_list();
	current_object = object_cursor;
	if (OBJECT(current_object)->floor_z_int < 0x800 && mem_read8((0x200000u | OBJECT(current_object)->direction) + 0x80u) == 1u)
	{
		x_int = (uint16_t)OBJECT(current_object)->x_int;
		y_int = (uint16_t)OBJECT(current_object)->y_int;
		kind = 7u;
		z = (uint16_t)(OBJECT(current_object)->z_int + 0x14u);
		spawn_sign(x_int, y_int, kind, z, &depth, &r11);
	}
}

void vine_scroll_stop_init(void)
{
	vines_hold_players = 1u;
	OBJECT(current_object)->handler = vine_scroll_stop_wait_ADDRESS;
	vine_scroll_stop_wait();
}

void vine_scroll_stop_wait(void)
{
	if ((int16_t)scroll_position < (int16_t)OBJECT(current_object)->spawn_arg_1)
	{
		vine_scroll_stop_hold();
		return;
	}
	scroll_mode = 1u;
	saved_scroll_mode = 1u;
	OBJECT(current_object)->handler = vine_scroll_stop_hold_ADDRESS;
	vine_scroll_stop_hold();
}

void vine_scroll_stop_hold(void)
{
	uint32_t all_clear = 0, player_index = 0, player_object = 0;

	player_index = 3u;
	all_clear = 1u;
	do
	{
		player_index--;
		if (player_index == 0xFFFFFFFFu)
		{
			if (all_clear == 0u)
			{
				vine_scroll_stop_end();
				return;
			}
			scroll_mode = 0u;
			saved_scroll_mode = 0u;
			scroll_speed = 8u;
			OBJECT(current_object)->handler = vine_scroll_stop_end_ADDRESS;
			vine_scroll_stop_end();
			return;
		}
		player_object = GAME_VARIABLES + vine_scroll_stop_player_blocks[player_index];
		player_object = (player_object & ~0xFFFFu) | mem_read16(player_object + 0x14u);
	} while ((mem_read32(player_object + 0x30u) & 0x400u) != 0u || (mem_read32(player_object + 0x30u) & 0x10000u) != 0u || (int16_t)mem_read16(player_object + 0x4Eu) >= -0x120);
	all_clear = 0u;
	if (all_clear == 0u)
	{
		vine_scroll_stop_end();
		return;
	}
	scroll_mode = 0u;
	saved_scroll_mode = 0u;
	scroll_speed = 8u;
	OBJECT(current_object)->handler = vine_scroll_stop_end_ADDRESS;
	vine_scroll_stop_end();
}

void vine_scroll_stop_end(void)
{
	uint32_t block_offset = 0, ignored = 0, slope_height = 0;

	if ((int16_t)OBJECT(current_object)->spawn_arg_2 < (int16_t)scroll_position)
	{
		scroll_mode = 7u;
		saved_scroll_mode = 7u;
		vines_hold_players = 0u;
		delete_object_and_children(&ignored, &block_offset, &slope_height);
	}
}

void spike_panel_init(void)
{
	uint32_t block = 0, first = 0, object = 0, parts_left = 0, spawned = 0;

	spawn_count = 4u;
	spawn_objects(0x00F092u, &spawned, &first, &block);
	if ((uint8_t)spawned == 0u)
		return;
	OBJECT(current_object)->spike_panel.step = OBJECT(current_object)->spike_panel.start_step;
	OBJECT(current_object)->spike_panel.step_timer = OBJECT(current_object)->spawn_arg_2;
	OBJECT(current_object)->spike_panel.trigger_delay = 0x1Eu;
	OBJECT(current_object)->child = first;
	OBJECT(current_object)->sprite_palette = 0x9020u;
	OBJECT(current_object)->sprite_flags = 6u;
	OBJECT(current_object)->flags |= 2u;
	OBJECT(current_object)->hitbox_offset_y = -0x16;
	OBJECT(current_object)->hitbox_height = 0x3Cu;
	OBJECT(current_object)->kind = 0xCu;
	OBJECT(current_object)->handler = spike_panel_hold_ADDRESS;
	parts_left = 2u;
	do
	{
		current_object = LINKED(current_object, OBJECT(current_object)->child);
		OBJECT(current_object)->z = OBJECT(object_cursor)->z;
		OBJECT(current_object)->floor_z = OBJECT(object_cursor)->floor_z;
		OBJECT(current_object)->sprite_palette = 0x9020u;
		OBJECT(current_object)->flags |= 2u;
		OBJECT(current_object)->hitbox_offset_y = -0x1A;
		OBJECT(current_object)->hitbox_height = 0x3Cu;
		OBJECT(current_object)->kind = 0xCu;
		OBJECT(current_object)->handler = spike_panel_part_idle_ADDRESS;
		parts_left--;
	} while (parts_left != 0u);
	OBJECT(current_object)->sprite_flags = 6u;
	OBJECT(current_object)->projected_x = OBJECT(object_cursor)->projected_x;
	OBJECT(current_object)->projected_y = OBJECT(object_cursor)->projected_y;
	OBJECT(current_object)->projected_x -= 0x7Bu;
	OBJECT(current_object)->projected_y_int -= 0x3Cu;
	current_object = LINKED(current_object, OBJECT(current_object)->child);
	OBJECT(current_object)->direction = 0x300u;
	OBJECT(current_object)->flags |= 2u;
	OBJECT(current_object)->hitbox_size_x = 0x8Du;
	OBJECT(current_object)->hitbox_size_y = 0xBCu;
	OBJECT(current_object)->hitbox_offset_x = 0;
	OBJECT(current_object)->hitbox_offset_y = 0;
	OBJECT(current_object)->hitbox_height = 0x3Cu;
	OBJECT(current_object)->kind = 0x12u;
	OBJECT(current_object)->handler = spike_panel_hitbox_update_ADDRESS;
	object = LINKED(first, OBJECT(current_object)->child);
	OBJECT(current_object)->child = 0xFFFFu;
	copy_position_to(object);
	OBJECT(object)->projected_x -= 0x7Cu;
	OBJECT(object)->projected_y_int += 0x38u;
	OBJECT(object)->sprite_frame = 0x6B58u;
	OBJECT(object)->sprite_palette = 0x11E0u;
	OBJECT(object)->handler = spike_panel_base_draw_ADDRESS;
}

void spike_panel_hold(void)
{
	uint16_t trigger_delay = 0;

	if (OBJECT(current_object)->spike_panel.start_step != 0u)
	{
		find_nearest_player_x();
		if (nearest_player_distance > 0x40)
		{
			OBJECT(current_object)->spike_panel.step_timer = 0;
			OBJECT(current_object)->spike_panel.step = OBJECT(current_object)->spike_panel.start_step;
			spike_panel_update();
			return;
		}
	}
	trigger_delay = OBJECT(current_object)->spike_panel.trigger_delay;
	OBJECT(current_object)->spike_panel.trigger_delay = trigger_delay - 1u;
	if (trigger_delay == 0u)
	{
		OBJECT(current_object)->spike_panel.start_step = 0u;
		OBJECT(current_object)->handler = spike_panel_update_ADDRESS;
		spike_panel_update();
		return;
	}
	OBJECT(current_object)->spike_panel.step_timer = 0;
	OBJECT(current_object)->spike_panel.step = OBJECT(current_object)->spike_panel.start_step;
	spike_panel_update();
}

void spike_panel_update(void)
{
	uint8_t bucket = 0, sound_id = 0;
	uint16_t count = 0;
	uint32_t r0 = 0, r2 = 0, r11 = 0, block_offset = 0, deleted = 0, depth = 0, frame_index = 0, ignored = 0, offset_record = 0, part_2_frame = 0, slope_height = 0;

	unproject_object(&ignored, &block_offset, &slope_height, &r11);
	delete_if_off_screen(&deleted, &block_offset, &slope_height);
	if ((uint8_t)deleted != 0u)
		return;
	r14 = LINKED(WORK_RAM, chaser_object);
	r0 = face_object();
	if ((int16_t)OBJECT(current_object)->direction <= 0x100 || (int16_t)OBJECT(current_object)->direction >= 0x378)
	{
		OBJECT(current_object)->spike_panel.step_timer--;
		if (OBJECT(current_object)->spike_panel.step_timer < 0 && OBJECT(current_object)->spike_panel.start_step == 0u)
		{
			OBJECT(current_object)->spike_panel.step++;
			OBJECT(current_object)->spike_panel.step_timer = spike_panel_step_durations[(uint8_t)OBJECT(current_object)->spike_panel.step];
			if (OBJECT(current_object)->spike_panel.step > 8)
			{
				OBJECT(current_object)->spike_panel.step = 0;
				OBJECT(current_object)->spike_panel.step_timer = OBJECT(current_object)->spawn_arg_2;
			}
		}
		frame_index = spike_panel_frame_sequence[(uint8_t)OBJECT(current_object)->spike_panel.step];
		OBJECT(current_object)->spike_panel.frame_index = frame_index;
		offset_record = spike_panel_head_offsets_ADDRESS + frame_index * 4u;
		OBJECT(current_object)->screen_x += ROM16(offset_record);
		OBJECT(current_object)->screen_y += ROM16(offset_record + 2u);
		OBJECT(current_object)->sprite_frame = spike_panel_head_sprites[frame_index];
		if ((int8_t)frame_index >= 5)
		{
			OBJECT(current_object)->hitbox_size_x = 0x65u;
			OBJECT(current_object)->hitbox_size_y = 0x30u;
			OBJECT(current_object)->hitbox_offset_x = -0x11;
		}
		else
		{
			OBJECT(current_object)->hitbox_size_x = 0x54u;
			OBJECT(current_object)->hitbox_size_y = 0x30u;
			OBJECT(current_object)->hitbox_offset_x = -1;
		}
		depth = (uint16_t)(OBJECT(current_object)->screen_y - 0xFu);
		queue_sprite_at_depth(&ignored, &depth);
		OBJECT(current_object)->health = 0x7F;
		add_to_touchable_list(ignored);
		current_object = LINKED(current_object, OBJECT(current_object)->child);
		OBJECT(current_object)->direction = 0x300u;
		OBJECT(current_object)->projected_x = OBJECT(object_cursor)->projected_x;
		OBJECT(current_object)->projected_y = OBJECT(object_cursor)->projected_y;
		OBJECT(current_object)->z = OBJECT(object_cursor)->z;
		frame_index = (uint8_t)OBJECT(object_cursor)->spike_panel.frame_index;
		offset_record = spike_panel_part_1_offsets_ADDRESS + (frame_index * 2u * 2u + frame_index + frame_index);
		OBJECT(current_object)->projected_x += mem_read16(offset_record);
		offset_record += 2u;
		OBJECT(current_object)->projected_y_int += mem_read16(offset_record);
		OBJECT(current_object)->z_int += mem_read16(offset_record + 2u);
		OBJECT(current_object)->sprite_flags = spike_panel_part_1_sprite_flags[frame_index];
		OBJECT(current_object)->sprite_frame = spike_panel_part_1_sprites[frame_index];
		unproject_object(&ignored, &r2, &ignored, &r11);
		if (OBJECT(object_cursor)->spike_panel.frame_index >= 3)
			queue_sprite(offset_record);
		else if ((uint8_t)OBJECT(object_cursor)->spike_panel.frame_index != 5u)
			queue_sprite_at_previous_floor_2();
		else
		{
			sound_id = 0x25u;
			play_sound(sound_id, &offset_record, &r2);
			bucket = 0u;
			queue_sprite_in_bucket(bucket);
		}
		current_object = LINKED(current_object, OBJECT(current_object)->child);
		unproject_object(&ignored, &ignored, &ignored, &r11);
		part_2_frame = (uint8_t)OBJECT(object_cursor)->spike_panel.frame_index;
		offset_record = spike_panel_part_2_offsets_ADDRESS + part_2_frame * 4u;
		OBJECT(current_object)->sprite_frame = spike_panel_part_2_sprites[part_2_frame];
		if (part_2_frame != 3u)
		{
			OBJECT(current_object)->screen_x += ROM16(offset_record);
			OBJECT(current_object)->screen_y += ROM16(offset_record + 2u);
			depth = (uint16_t)(OBJECT(current_object)->screen_y - 0xFu);
			queue_sprite_at_depth(&part_2_frame, &depth);
		}
		if ((int8_t)part_2_frame >= 5)
		{
			OBJECT(current_object)->hitbox_size_x = 0x59u;
			OBJECT(current_object)->hitbox_size_y = 0x2Au;
			OBJECT(current_object)->hitbox_offset_x = -0x14;
		}
		else
		{
			OBJECT(current_object)->hitbox_size_x = 0x4Bu;
			OBJECT(current_object)->hitbox_size_y = 0x2Au;
			OBJECT(current_object)->hitbox_offset_x = -5;
		}
		OBJECT(current_object)->health = 0x7F;
		add_to_touchable_list(part_2_frame);
		return;
	}
	count = 0xFu;
	OBJECT(current_object)->projected_x -= 0x90u;
	OBJECT(current_object)->projected_y_int -= 0x40u;
	spawn_debris(r0, count, r11);
	delete_object_and_children(&ignored, &block_offset, &slope_height);
}

void spike_panel_base_draw(void)
{
	uint8_t bucket = 0;
	uint32_t block_offset = 0, cursor = 0, deleted = 0, ignored = 0, slope_height = 0;

	unproject_object(&ignored, &block_offset, &slope_height, &cursor);
	delete_if_off_screen(&deleted, &block_offset, &slope_height);
	if ((uint8_t)deleted != 1u)
	{
		bucket = 0xFFu;
		queue_sprite_in_bucket(bucket);
		return;
	}
}

void spike_panel_hitbox_update(void)
{
	uint32_t cursor = 0, ignored = 0;

	current_object = LINKED(current_object, OBJECT(object_cursor)->parent);
	if ((int8_t)OBJECT(current_object)->var_80 < 4)
		return;
	OBJECT(object_cursor)->kind = 0x12u;
	if ((int8_t)OBJECT(current_object)->var_81 >= 5)
		OBJECT(object_cursor)->kind = 0xCu;
	current_object = LINKED(current_object, OBJECT(current_object)->child);
	OBJECT(object_cursor)->projected_x = OBJECT(current_object)->projected_x;
	OBJECT(object_cursor)->projected_y = OBJECT(current_object)->projected_y;
	OBJECT(object_cursor)->z = OBJECT(current_object)->z;
	OBJECT(object_cursor)->floor_z = OBJECT(current_object)->floor_z;
	OBJECT(object_cursor)->projected_x -= 0xD8u;
	current_object = (current_object & ~0xFFFFu) | (uint16_t)object_cursor;
	unproject_object(&ignored, &ignored, &ignored, &cursor);
	OBJECT(current_object)->health = 0x7F;
	add_to_touchable_list(ignored);
}

void spike_panel_part_idle(void)
{
}

void electric_pole_init(void)
{
	OBJECT(current_object)->sprite_frame = 0x67D8u;
	OBJECT(current_object)->sprite_palette = 0x87F0u;
	OBJECT(current_object)->hitbox_size_x = 0x19u;
	OBJECT(current_object)->hitbox_height = 0x53u;
	OBJECT(current_object)->health = 4;
	OBJECT(current_object)->kind = 5u;
	OBJECT(current_object)->handler = electric_pole_wait_ADDRESS;
	current_object = LINKED(current_object, OBJECT(current_object)->child);
	OBJECT(current_object)->sprite_palette = 0x83D0u;
	current_object = LINKED(current_object, OBJECT(current_object)->child);
	OBJECT(current_object)->sprite_frame = 0x6850u;
	OBJECT(current_object)->sprite_palette = 0x83D0u;
	current_object = LINKED(current_object, OBJECT(current_object)->child);
	OBJECT(current_object)->direction = 0x401u;
	OBJECT(current_object)->sprite_frame = 0x6850u;
	OBJECT(current_object)->sprite_palette = 0x83D0u;
}

void electric_pole_wait(void)
{
	face_nearest_player(0u, 0u);
	if (nearest_player_distance > 0x70)
	{
		electric_pole_update();
		return;
	}
	OBJECT(current_object)->handler = electric_pole_rise_ADDRESS;
	electric_pole_rise();
}

void electric_pole_rise(void)
{
	uint16_t odd_frame = 0;

	OBJECT(current_object)->electric_pole.frame_toggle++;
	odd_frame = OBJECT(current_object)->electric_pole.frame_toggle & 1u;
	OBJECT(current_object)->electric_pole.frame_toggle = odd_frame;
	if (odd_frame != 0u)
	{
		electric_pole_update();
		return;
	}
	OBJECT(current_object)->electric_pole.frame_index++;
	OBJECT(current_object)->sprite_frame = electric_pole_sprites[electric_pole_rise_frames[(uint8_t)OBJECT(current_object)->electric_pole.frame_index]];
	if (OBJECT(current_object)->electric_pole.frame_index < 0xA)
	{
		electric_pole_update();
		return;
	}
	OBJECT(current_object)->projected_y_int -= 0xBu;
	OBJECT(current_object)->electric_pole.draw_offset_y = 0xBu;
	OBJECT(current_object)->handler = electric_pole_update_ADDRESS;
	electric_pole_update();
}

void electric_pole_update(void)
{
	uint8_t sound_id = 0;
	uint32_t r1 = 0, block_offset = 0, charging_spark_y = 0, cursor = 0, deleted = 0, ignored = 0, slope_height = 0, spark_y = 0, write_index_address = 0;
	uint8_t front_odd_frame = 0, side_odd_frame = 0;

	r14 = LINKED(WORK_RAM, chaser_object);
	distance_xy_to_object(&ignored, &r1);
	if (distance_to_player_1 <= 0xF)
	{
		OBJECT(current_object)->sprite_scale = 0x200u;
		OBJECT(current_object)->projected_y_int += 0xBu;
		OBJECT(current_object)->electric_pole.frame_index = 0;
		OBJECT(current_object)->electric_pole.last_frame = 0xF;
		OBJECT(current_object)->sprite_palette = 0x9000u;
		OBJECT(current_object)->handler = explosion_animate_ADDRESS;
		OBJECT(current_object)->electric_pole.frame_toggle = 0u;
		sound_id = 7u;
		play_sound(sound_id, &r1, &write_index_address);
		return;
	}
	if ((OBJECT(current_object)->flags & 4u) == 0u)
	{
	electric_pole_draw:
		unproject_object(&ignored, &block_offset, &slope_height, &cursor);
		delete_if_off_screen(&deleted, &block_offset, &slope_height);
		if ((uint8_t)deleted == 1u)
			return;
		OBJECT(current_object)->screen_y += OBJECT(current_object)->electric_pole.draw_offset_y;
		queue_sprite(r1);
		if ((int16_t)AS16(OBJECT(object_cursor)->electric_pole.frame_index) >= 0xA)
		{
			current_object = LINKED(current_object, OBJECT(current_object)->child);
			AT8(OBJECT(current_object)->direction, 1)++;
			front_odd_frame = AT8(OBJECT(current_object)->direction, 1) & 1u;
			AT8(OBJECT(current_object)->direction, 1) = front_odd_frame;
			if (front_odd_frame == 0u)
			{
				AS8(OBJECT(current_object)->direction)++;
				if ((int8_t)AS8(OBJECT(current_object)->direction) > 6)
					OBJECT(current_object)->direction = 0u;
				OBJECT(current_object)->sprite_frame = electric_pole_front_spark_sprites[AS8(OBJECT(current_object)->direction)];
			}
			OBJECT(current_object)->screen_x = OBJECT(object_cursor)->screen_x;
			spark_y = (uint16_t)(OBJECT(object_cursor)->screen_y - 0xAu);
			OBJECT(current_object)->screen_y = spark_y;
			spark_y += 0xBu;
			queue_sprite_at_depth(&ignored, &spark_y);
			r14 = 2u;
			do
			{
				current_object = LINKED(current_object, OBJECT(current_object)->child);
				AT8(OBJECT(current_object)->direction, 1)++;
				side_odd_frame = AT8(OBJECT(current_object)->direction, 1) & 1u;
				AT8(OBJECT(current_object)->direction, 1) = side_odd_frame;
				if (side_odd_frame == 0u)
				{
					AS8(OBJECT(current_object)->direction)++;
					if ((int8_t)AS8(OBJECT(current_object)->direction) > 6)
						OBJECT(current_object)->direction = 0u;
					OBJECT(current_object)->sprite_frame = electric_pole_side_spark_sprites[AS8(OBJECT(current_object)->direction)];
				}
				OBJECT(current_object)->screen_x = OBJECT(object_cursor)->screen_x;
				OBJECT(current_object)->screen_x += electric_pole_side_spark_offsets_x[r14 - 1u];
				spark_y = (uint16_t)(OBJECT(object_cursor)->screen_y - 0x10u);
				OBJECT(current_object)->screen_y = spark_y;
				spark_y += 0x11u;
				queue_sprite_at_depth(&ignored, &spark_y);
				r14 = r14 - 1u;
			} while (r14 != 0u);
		}
		else
		{
			current_object = LINKED(current_object, OBJECT(current_object)->child);
			if (OBJECT(object_cursor)->electric_pole.frame_index < 2)
				return;
			OBJECT(current_object)->sprite_frame = electric_pole_side_spark_sprites[(uint8_t)OBJECT(object_cursor)->electric_pole.frame_index - 2u];
			OBJECT(current_object)->screen_x = OBJECT(object_cursor)->screen_x;
			charging_spark_y = (uint16_t)OBJECT(object_cursor)->screen_y;
			OBJECT(current_object)->screen_y = charging_spark_y;
			charging_spark_y++;
			queue_sprite_at_depth(&ignored, &charging_spark_y);
		}
		current_object = object_cursor;
		add_to_touchable_list(ignored);
		return;
	}
	if (OBJECT(current_object)->health < 4)
	{
		OBJECT(current_object)->sprite_scale = 0x200u;
		OBJECT(current_object)->projected_y_int += 0xBu;
		OBJECT(current_object)->electric_pole.frame_index = 0;
		OBJECT(current_object)->electric_pole.last_frame = 0xF;
		OBJECT(current_object)->sprite_palette = 0x9000u;
		OBJECT(current_object)->handler = explosion_animate_ADDRESS;
		OBJECT(current_object)->electric_pole.frame_toggle = 0u;
		sound_id = 7u;
		play_sound(sound_id, &r1, &write_index_address);
		return;
	}
	OBJECT(current_object)->kind = 0x15u;
	goto electric_pole_draw;
}

void metal_wall_init(void)
{
	OBJECT(current_object)->handler = metal_wall_wait_ADDRESS;
	OBJECT(current_object)->metal_wall.rise_limit = 3;
	OBJECT(current_object)->sprite_palette = 0x8310u;
	OBJECT(current_object)->hitbox_size_x = 0x6Cu;
	OBJECT(current_object)->hitbox_size_y = 0x38u;
	OBJECT(current_object)->hitbox_offset_x = -0x10;
	OBJECT(current_object)->hitbox_offset_y = -0x30;
	OBJECT(current_object)->hitbox_height = 0x4Cu;
	OBJECT(current_object)->kind = 5u;
	OBJECT(current_object)->health = 4;
	OBJECT(current_object)->metal_wall.last_health = OBJECT(current_object)->health;
	OBJECT(current_object)->flags |= 2u;
	if (OBJECT(current_object)->metal_wall.flipped != 0u)
	{
		OBJECT(current_object)->sprite_flags |= 0x40u;
		OBJECT(current_object)->hitbox_size_x = 0x35u;
		OBJECT(current_object)->hitbox_size_y = 0x6Fu;
		OBJECT(current_object)->hitbox_offset_x = -0xD;
		OBJECT(current_object)->hitbox_offset_y = -0x60;
		OBJECT(current_object)->hitbox_height = 0x4Cu;
	}
	current_object = LINKED(current_object, OBJECT(current_object)->child);
	OBJECT(current_object)->sprite_palette = 0x8310u;
	OBJECT(current_object)->sprite_flags = OBJECT(object_cursor)->sprite_flags;
	current_object = LINKED(current_object, OBJECT(current_object)->child);
	OBJECT(current_object)->sprite_palette = 0x89B0u;
	OBJECT(current_object)->sprite_flags = OBJECT(object_cursor)->sprite_flags;
	current_object = LINKED(current_object, OBJECT(current_object)->child);
	OBJECT(current_object)->sprite_palette = 0x89C0u;
	OBJECT(current_object)->sprite_flags = OBJECT(object_cursor)->sprite_flags;
	current_object = LINKED(current_object, OBJECT(current_object)->child);
	OBJECT(current_object)->sprite_palette = 0x8260u;
	OBJECT(current_object)->sprite_flags = OBJECT(object_cursor)->sprite_flags;
	current_object = LINKED(current_object, OBJECT(current_object)->child);
	OBJECT(current_object)->sprite_palette = 0x8260u;
	OBJECT(current_object)->sprite_flags = OBJECT(object_cursor)->sprite_flags;
	current_object = LINKED(current_object, OBJECT(current_object)->child);
	OBJECT(current_object)->sprite_palette = 0x8260u;
	OBJECT(current_object)->direction = 3u;
	OBJECT(current_object)->sprite_flags = OBJECT(object_cursor)->sprite_flags;
	current_object = LINKED(current_object, OBJECT(current_object)->child);
	OBJECT(current_object)->sprite_palette = 0x8260u;
	OBJECT(current_object)->direction = 3u;
	OBJECT(current_object)->sprite_flags = OBJECT(object_cursor)->sprite_flags;
}

void metal_wall_wait(void)
{
	uint32_t octant = 0;

	OBJECT(current_object)->projected_x += 0x10u;
	OBJECT(current_object)->projected_y_int += 0x10u;
	octant = face_nearest_player(0u, 0u);
	OBJECT(current_object)->projected_x -= 0x10u;
	OBJECT(current_object)->projected_y_int -= 0x10u;
	if (nearest_player_distance > 0x80)
	{
		metal_wall_update(octant);
		return;
	}
	OBJECT(current_object)->metal_wall.dust_timer = 0x14u;
	OBJECT(current_object)->handler = metal_wall_rise_ADDRESS;
	metal_wall_rise(octant);
}

void metal_wall_rise(uint32_t octant)
{
	uint16_t frame_count = 0;

	frame_count = (uint16_t)OBJECT(current_object)->metal_wall.frame_count;
	OBJECT(current_object)->metal_wall.frame_count++;
	if ((int16_t)frame_count <= 2)
	{
		metal_wall_update(octant);
		return;
	}
	OBJECT(current_object)->metal_wall.frame_count = 0;
	OBJECT(current_object)->metal_wall.frame_index++;
	if (OBJECT(current_object)->metal_wall.frame_index <= OBJECT(current_object)->metal_wall.rise_limit)
	{
		metal_wall_update(octant);
		return;
	}
	OBJECT(current_object)->metal_wall.rise_limit = 7;
	OBJECT(current_object)->handler = metal_wall_update_ADDRESS;
	metal_wall_update(octant);
}

void metal_wall_update(uint32_t octant)
{
	uint32_t r1 = 0, r2 = 0, r11 = 0, block_offset = 0, deleted = 0, health = 0, ignored = 0, ring = 0, ring_count = 0, spawn_arg_1 = 0;

	if (OBJECT(current_object)->health <= 0)
	{
		OBJECT(current_object)->health = 0;
		OBJECT(current_object)->handler = metal_wall_broken_ADDRESS;
		if (OBJECT(current_object)->metal_wall.ring_count != 0u)
		{
			ring_count = OBJECT(current_object)->metal_wall.ring_count;
			scatter_rings_random(octant, ring_count, &r11, &ring);
		}
	}
	health = (uint8_t)OBJECT(current_object)->health;
	r1 = OBJECT(current_object)->metal_wall.last_health;
	if (health != r1)
	{
		OBJECT(current_object)->metal_wall.last_health = OBJECT(current_object)->health;
		OBJECT(current_object)->metal_wall.frame_index = metal_wall_damage_frames[health];
		if ((int8_t)(r1 - health) > 1)
			OBJECT(current_object)->z -= 0x16000u;
		OBJECT(current_object)->screen_x += 2u;
		spawn_arg_1 = OBJECT(current_object)->spawn_arg_1;
		r1 = (4u - spawn_arg_1) * 2u;
		if ((int32_t)r1 > 0)
			spawn_debris(spawn_arg_1, (uint16_t)r1, r11);
	}
	delete_if_off_screen(&deleted, &block_offset, &octant);
	if ((uint8_t)deleted == 1u)
		return;
	move_object_projected(&ignored, &r2, &ignored, &r11);
	fall_to_floor(&ignored, &r1, &r2);
	if ((uint8_t)OBJECT(current_object)->health == 0u)
	{
		metal_wall_draw();
		return;
	}
	add_to_touchable_list(ignored);
	metal_wall_draw();
}

void metal_wall_broken(void)
{
	uint32_t block_offset = 0, deleted = 0, slope_height = 0;

	delete_if_off_screen(&deleted, &block_offset, &slope_height);
	if ((uint8_t)deleted == 1u)
		return;
	update_screen_position_no_offset();
	metal_wall_draw();
}

void metal_wall_draw(void)
{
	uint8_t bucket = 0, health = 0;
	uint16_t dust_frame = 0, dust_x = 0, dust_y = 0, part_dy = 0, screen_offset = 0;
	uint32_t depth = 0, direction = 0, dust_sprite_base = 0, dust_y_2 = 0, flip_offset = 0, frame_index = 0, ignored = 0, offset_pair = 0, part_index = 0, parts_left = 0;

	OBJECT(current_object)->sprite_frame = metal_wall_sprites[(uint8_t)OBJECT(current_object)->metal_wall.frame_index];
	if ((uint8_t)OBJECT(current_object)->health == 0u)
	{
		bucket = 0xFEu;
		queue_sprite_in_bucket(bucket);
	}
	else
	{
		depth = (uint16_t)(OBJECT(current_object)->screen_y - 0x14u);
		queue_sprite_at_depth(&ignored, &depth);
	}
	parts_left = 3u;
	flip_offset = (uint32_t)OBJECT(object_cursor)->metal_wall.flipped << 4;
	do
	{
		current_object = LINKED(current_object, OBJECT(current_object)->child);
		part_index = (parts_left & ~0xFFu) | (uint8_t)(parts_left - 1u);
		frame_index = (uint8_t)OBJECT(object_cursor)->metal_wall.frame_index;
		offset_pair = metal_wall_part_tables[(part_index & ~0xFFu) | (uint8_t)(part_index + 3u)] + (uint8_t)(frame_index * 2u) + flip_offset;
		screen_offset = (uint16_t)(int8_t)mem_read8(offset_pair);
		part_dy = (uint16_t)(int8_t)mem_read8(offset_pair + 1u);
		OBJECT(current_object)->screen_x = OBJECT(object_cursor)->screen_x;
		OBJECT(current_object)->screen_x += screen_offset;
		OBJECT(current_object)->screen_y = OBJECT(object_cursor)->screen_y;
		OBJECT(current_object)->screen_y += part_dy;
		OBJECT(current_object)->sprite_frame = mem_read16(metal_wall_part_tables[part_index] + (uint8_t)OBJECT(object_cursor)->metal_wall.frame_index * 2u);
		health = (uint8_t)OBJECT(object_cursor)->health;
		if ((uint8_t)part_index != 2u)
		{
			bucket = 0xFEu;
			if (parts_left != 2u)
				bucket = 0xFFu;
		metal_wall_draw_part_in_bucket:
			queue_sprite_in_bucket(bucket);
		}
		else
		{
			bucket = 0xFEu;
			if (health == 0u)
				goto metal_wall_draw_part_in_bucket;
			depth = (uint16_t)(OBJECT(current_object)->screen_y - 0x20u);
			queue_sprite_at_depth(&ignored, &depth);
		}
		parts_left--;
	} while (parts_left != 0u);
	if (OBJECT(object_cursor)->metal_wall.dust_timer != 0u)
	{
		OBJECT(object_cursor)->metal_wall.dust_timer--;
		if ((OBJECT(object_cursor)->metal_wall.dust_timer & 1u) == 0u)
			return;
		dust_x = 0xFFF8u;
		dust_y = 6u;
		dust_sprite_base = 0u;
		parts_left = 4u;
		do
		{
			current_object = LINKED(current_object, OBJECT(current_object)->child);
			dust_frame = (uint16_t)(OBJECT(current_object)->direction + 1u);
			if ((int16_t)dust_frame > 7)
				dust_frame = 0u;
			if (parts_left == 2u)
			{
				dust_x = 0xFFF8u;
				dust_sprite_base = 0u;
			}
			OBJECT(current_object)->direction = dust_frame;
			direction = OBJECT(current_object)->direction;
			OBJECT(current_object)->sprite_frame = metal_wall_dust_sprites[dust_sprite_base + direction];
			if ((int32_t)parts_left <= 2)
			{
				screen_offset = (uint16_t)(0x20u - (direction << 2));
				dust_y -= screen_offset;
			}
			OBJECT(current_object)->screen_x = OBJECT(object_cursor)->screen_x;
			OBJECT(current_object)->screen_x += dust_x;
			OBJECT(current_object)->screen_y = OBJECT(object_cursor)->screen_y;
			OBJECT(current_object)->screen_y += dust_y;
			dust_y_2 = (uint16_t)OBJECT(current_object)->screen_y;
			if ((int32_t)parts_left <= 2)
				dust_y_2 = (uint16_t)(dust_y_2 + screen_offset);
			dust_y_2 = (uint16_t)(dust_y_2 - 0x14u);
			queue_sprite_at_depth(&ignored, &dust_y_2);
			dust_x = 0x2Cu;
			dust_y = 6u;
			dust_sprite_base = 8u;
			parts_left--;
		} while (parts_left != 0u);
	}
}

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

void push_crate_init(void)
{
	uint16_t post_distance = 0;
	uint32_t block = 0, free_count = 0, object = 0, piece_count = 0, spawned = 0;

	OBJECT(current_object)->sprite_frame = 0x6968u;
	OBJECT(current_object)->sprite_palette = 0x9D0u;
	OBJECT(current_object)->flags |= 2u;
	OBJECT(current_object)->hitbox_size_x = 0x40u;
	OBJECT(current_object)->hitbox_size_y = 0x4Bu;
	OBJECT(current_object)->hitbox_offset_x = -9;
	OBJECT(current_object)->hitbox_offset_y = -0x42;
	OBJECT(current_object)->hitbox_height = 0x20u;
	OBJECT(current_object)->kind = 0x20u;
	OBJECT(current_object)->push_crate.frame_count = 0x12Cu;
	OBJECT(current_object)->handler = push_crate_update_ADDRESS;
	spawn_count = 2u;
	spawn_object_with_pieces(0x011203u, &spawned, &piece_count, &free_count, &object, &block);
	OBJECT(object_cursor)->child = object;
	copy_position_to(object);
	post_distance = OBJECT(object_cursor)->push_crate.post_distance;
	OBJECT(object)->projected_x -= post_distance * 0x22u + 0x11u;
	OBJECT(object)->projected_y_int -= post_distance * 0x11u + 0x1Du;
	OBJECT(object)->sprite_frame = 0x73B0u;
	OBJECT(object)->sprite_palette = 0x89E0u;
	OBJECT(object)->handler = push_crate_stop_post_update_ADDRESS;
	object = LINKED(object, OBJECT(object)->child);
	OBJECT(object)->sprite_frame = 0x73B0u;
	OBJECT(object)->sprite_palette = 0x89E0u;
}

void push_crate_update(void)
{
	uint32_t direction = 0, distance_y = 0, ignored = 0, object = 0, speed = 0, speed_x = 0, speed_y = 0;

	r14 = LINKED(WORK_RAM, chaser_object);
	distance_xy_to_object(&ignored, &distance_y);
	if ((int16_t)distance_y <= 3)
	{
		OBJECT(current_object)->speed_x = 0;
		OBJECT(current_object)->speed_y = 0;
		OBJECT(current_object)->handler = push_crate_break_ADDRESS;
		push_crate_break();
		return;
	}
	if ((OBJECT(current_object)->flags & 4u) == 0u)
	{
		push_crate_move(0u, 0u, 0u);
		return;
	}
	object = LINKED(WORK_RAM, OBJECT(current_object)->touched_by);
	if ((OBJECT(object)->flags & 0x40u) != 0u)
	{
		OBJECT(current_object)->speed_x = 0;
		OBJECT(current_object)->speed_y = 0;
		OBJECT(current_object)->handler = push_crate_break_ADDRESS;
		push_crate_break();
		return;
	}
	if (OBJECT(current_object)->push_crate.stopped != 0u || OBJECT(current_object)->push_crate.push_quadrant != (uint16_t)((int16_t)(OBJECT(object)->direction + 0x40u) / 0x40 - 1u))
	{
		push_crate_move(0u, 0u, 0u);
		return;
	}
	direction = push_crate_push_angles[OBJECT(current_object)->push_crate.push_quadrant];
	speed = (uint32_t)OBJECT(object)->speed >> 16;
	direction_to_speed(speed, &direction, &speed_x, &speed_y);
	OBJECT(current_object)->speed_x = speed_x;
	OBJECT(current_object)->speed_y = speed_y;
	push_crate_move(0u, 0u, 0u);
}

void push_crate_break(void)
{
	uint32_t r11 = 0, r16 = 0, r17 = 0, break_frame = 0;
	uint16_t odd_frame = 0;

	OBJECT(current_object)->push_crate.frame_count++;
	odd_frame = OBJECT(current_object)->push_crate.frame_count & 1u;
	OBJECT(current_object)->push_crate.frame_count = odd_frame;
	if (odd_frame != 0u)
	{
		push_crate_move(0u, 0u, 0u);
		return;
	}
	OBJECT(current_object)->push_crate.break_frame++;
	break_frame = OBJECT(current_object)->push_crate.break_frame;
	OBJECT(current_object)->sprite_frame = push_crate_break_sprites[break_frame];
	if (break_frame != 2u)
	{
		push_crate_move(0u, 0u, 0u);
		return;
	}
	OBJECT(current_object)->handler = push_crate_move_ADDRESS;
	if (OBJECT(current_object)->push_crate.ring_count <= 0)
	{
		push_crate_move(0u, 0u, 0u);
		return;
	}
	push_crate_release_all_rings(&r11, &r16, &r17);
	push_crate_move(r11, r16, r17);
}

void push_crate_move(uint32_t cursor, uint32_t random_value, uint32_t ring)
{
	uint32_t r3 = 0, block_offset = 0, deleted = 0, depth = 0, ignored = 0;
	uint16_t ring_delay = 0;

	move_object_projected(&ignored, &block_offset, &r3, &cursor);
	delete_if_off_screen(&deleted, &block_offset, &r3);
	if ((uint8_t)deleted == 1u)
		return;
	if (OBJECT(current_object)->push_crate.break_frame != 2u)
	{
		OBJECT(current_object)->health = 0x7F;
		add_to_touchable_list(deleted);
		depth = (uint16_t)(OBJECT(current_object)->screen_y - 0xDu);
		queue_sprite_at_depth(&ignored, &depth);
		if ((uint32_t)OBJECT(current_object)->speed_x != 0u)
		{
			ring_delay = OBJECT(current_object)->push_crate.ring_timer - 1u;
			OBJECT(current_object)->push_crate.ring_timer = ring_delay;
			if ((int16_t)ring_delay < 0 && OBJECT(current_object)->push_crate.ring_count > 0)
			{
				OBJECT(current_object)->push_crate.ring_count--;
				OBJECT(current_object)->push_crate.ring_timer = 0x10;
				rings_to_release = 1u;
				push_crate_release_rings(r3, &cursor, &random_value, &ring);
			}
		}
	}
	else
	{
		deleted = (uint16_t)(deleted + 0xFFu);
		queue_sprite_in_bucket(deleted);
	}
}

void push_crate_stop_post_update(void)
{
	uint16_t offset_x = 0, offset_y = 0;
	uint32_t r1 = 0, cursor = 0, depth = 0, ignored = 0;

	unproject_object(&ignored, &ignored, &ignored, &cursor);
	r14 = LINKED(WORK_RAM, OBJECT(current_object)->parent);
	distance_xy_to_object(&ignored, &r1);
	if (distance_to_player_2 <= 0x33)
	{
		OBJECT(r14)->var_81 = 1u;
		OBJECT(r14)->speed_x = 0;
		OBJECT(r14)->speed_y = 0;
	}
	queue_sprite(r1);
	current_object = LINKED(current_object, OBJECT(current_object)->child);
	offset_x = 0u;
	offset_y = 0xFFF1u;
	place_sprite_near_cursor(offset_x, offset_y);
	depth = (uint16_t)(OBJECT(current_object)->screen_y + 0x11u);
	queue_sprite_at_depth(&ignored, &depth);
}

void push_crate_release_all_rings(uint32_t *p_allocated, uint32_t *p_random_value, uint32_t *p_ring)
{
	uint32_t allocated = *p_allocated;
	uint32_t random_value = *p_random_value;
	uint32_t ring = *p_ring;

	rings_to_release = OBJECT(current_object)->push_crate.ring_count;
	push_crate_release_rings(0u, &allocated, &random_value, &ring);
	*p_allocated = allocated;
	*p_random_value = random_value;
	*p_ring = ring;
}

void push_crate_release_rings(uint32_t seed, uint32_t *p_allocated, uint32_t *p_random_value, uint32_t *p_ring)
{
	uint32_t allocated = *p_allocated;
	uint32_t random_value = *p_random_value;
	uint32_t ring = *p_ring;
	uint32_t opposite_quadrant = 0;

	opposite_quadrant = OBJECT(current_object)->push_crate.push_quadrant;
	opposite_quadrant = (uint8_t)(opposite_quadrant + 2u);
	released_ring_angle = push_crate_push_angles[opposite_quadrant & 3u];
	released_ring_flip = 0u;
	released_rings_random = 0u;
	drop_rings(seed, &allocated, &random_value, &ring);
	*p_allocated = allocated;
	*p_random_value = random_value;
	*p_ring = ring;
}

void sign_start_from_type(void)
{
	uint16_t spawn_arg_1 = 0, x_int = 0, y_int = 0, z_int = 0;
	uint32_t cursor = 0, ignored = 0, object = 0;

	unproject_object(&ignored, &ignored, &ignored, &cursor);
	x_int = (uint16_t)OBJECT(current_object)->x_int;
	y_int = (uint16_t)OBJECT(current_object)->y_int;
	spawn_arg_1 = OBJECT(current_object)->spawn_arg_1;
	z_int = (uint16_t)OBJECT(current_object)->z_int;
	object = current_object;
	place_sign(x_int, y_int, spawn_arg_1, z_int, &ignored, &object);
	OBJECT(object)->speed_x_fraction = OBJECT(current_object)->spawn_arg_2;
}

void handle_rod_init(void)
{
	uint16_t angle_offset = 0, angle_step = 0;
	uint32_t r0 = 0, block = 0, handle_index = 0, object = 0, object_2 = 0, object_3 = 0, rod_dx = 0, rod_dz = 0, rod_length = 0;

	OBJECT(current_object)->sprite_scale = 0x100u;
	OBJECT(current_object)->sprite_frame = 0x73B8u;
	OBJECT(current_object)->sprite_palette = 0x9170u;
	OBJECT(current_object)->handle_rod.angle = 0x322u;
	OBJECT(current_object)->handle_rod.swing_accel = 0x60;
	r0 = OBJECT(current_object)->handle_rod.handle_count;
	spawn_count = r0;
	r0 = (uint16_t)(r0 * 0x12u);
	OBJECT(current_object)->z_int -= r0;
	spawn_objects(0x011474u, &r0, &object_2, &block);
	if ((uint8_t)r0 == 0u)
		return;
	OBJECT(current_object)->child = object_2;
	OBJECT(current_object)->parent = object_2;
	object_3 = LINKED(WORK_RAM, OBJECT(object_2)->child);
	OBJECT(current_object)->handler = handle_rod_wait_ADDRESS;
	handle_index = (r0 & ~0xFFFFu) | OBJECT(current_object)->handle_rod.handle_count;
	angle_step = divide_s16(0x400u, (uint16_t)handle_index);
	do
	{
		current_object = LINKED(current_object, OBJECT(current_object)->child);
		r14 = (uint16_t)current_object;
		copy_position_from_cursor();
		OBJECT(current_object)->sprite_frame = OBJECT(object_cursor)->sprite_frame;
		OBJECT(current_object)->sprite_palette = OBJECT(object_cursor)->sprite_palette;
		OBJECT(current_object)->handler = handle_rod_handle_init_ADDRESS;
		OBJECT(current_object)->var_82 = OBJECT(object_cursor)->handle_rod.angle;
		OBJECT(current_object)->var_84 = OBJECT(object_cursor)->handle_rod.angle_speed;
		OBJECT(current_object)->sprite_scale = OBJECT(object_cursor)->sprite_scale;
		AS16(OBJECT(current_object)->var_80) = handle_index;
		angle_offset = (uint16_t)(angle_step * handle_index);
		OBJECT(current_object)->spawn_arg_1 = 0x400u;
		OBJECT(current_object)->spawn_arg_1 -= angle_offset;
		OBJECT(current_object)->spawn_arg_2 += angle_offset;
		OBJECT(current_object)->z_int += handle_index * 0x12u;
		handle_index--;
	} while (handle_index != 0u);
	OBJECT(object_3)->var_81 = 1u;
	OBJECT(object_3)->var_86 = object_cursor;
	OBJECT(object_3)->spawn_arg_2 = OBJECT(object_cursor)->x_int;
	OBJECT(object_3)->hitbox_size_x = 0x10u;
	OBJECT(object_3)->kind = 0x1Au;
	current_object = LINKED(current_object, OBJECT(object_cursor)->child);
	OBJECT(object_cursor)->child = 0xFFFFu;
	OBJECT(object_cursor)->handle_rod.angle = 0x300u;
	object = LINKED(WORK_RAM, OBJECT(current_object)->child);
	OBJECT(current_object)->child = 0xFFFFu;
	OBJECT(current_object)->sprite_frame = 0x74C8u;
	OBJECT(current_object)->sprite_palette = 0x1060u;
	OBJECT(current_object)->projected_x = OBJECT(object)->projected_x;
	OBJECT(current_object)->projected_y = OBJECT(object)->projected_y;
	OBJECT(current_object)->projected_x -= 0x11u;
	OBJECT(current_object)->projected_y_int += 3u;
	OBJECT(current_object)->z = OBJECT(object_cursor)->z;
	OBJECT(current_object)->handler = handle_rod_post_init_ADDRESS;
	OBJECT(current_object)->sprite_scale = 0x100u;
	rod_length = OBJECT(current_object)->var_80 * 0x12u;
	rod_dx = sine_of_300 * 2u * 2u * rod_length;
	rod_dz = cosine_of_300 * 2u * 2u * rod_length;
	OBJECT(current_object)->projected_x += (int32_t)rod_dx >> 16;
	OBJECT(current_object)->projected_y += 0u - ((int32_t)rod_dx >> 1);
	OBJECT(current_object)->z += rod_dz;
	spawn_count = 1u;
	spawn_objects(0x0115D3u, &object, &object_2, &block);
	copy_position_to(object_2);
	OBJECT(object_2)->projected_x -= 0x12u;
	OBJECT(object_2)->projected_y_int -= 6u;
	OBJECT(object_2)->handle_rod_post.fallen = 1u;
	OBJECT(object_2)->handler = handle_rod_post_init_ADDRESS;
	OBJECT(object_cursor)->z_int -= 0x18u;
}

void handle_rod_handle_init(void)
{
	uint32_t first = 0, ignored = 0, object = 0, spawned = 0;

	spawn_count = 1u;
	add_pieces_to_current_object(&spawned, &ignored, &first, &object);
	if ((uint8_t)spawned != 0u)
	{
		OBJECT(current_object)->handler = handle_rod_handle_update_ADDRESS;
		OBJECT(object)->sprite_frame = 0x5D80u;
		OBJECT(object)->sprite_palette = 0x87A0u;
		OBJECT(object)->sprite_scale = 0x80u;
	}
}

void handle_rod_wait(void)
{
	uint32_t cursor = 0, depth = 0, ignored = 0;

	unproject_object(&ignored, &ignored, &ignored, &cursor);
	if (OBJECT(LINKED(WORK_RAM, OBJECT(current_object)->parent))->var_81 == 0u)
	{
		depth = (uint16_t)OBJECT(current_object)->screen_y;
		OBJECT(current_object)->hitbox_size_x = depth;
		queue_sprite_at_depth(&ignored, &depth);
		add_to_rider_list();
		return;
	}
	OBJECT(current_object)->handler = handle_rod_start_swing_ADDRESS;
	if (OBJECT(current_object)->handle_rod.drops_players == 0u)
	{
		players_pulled = 1u;
		event_12_fast_scroll = 1u;
	}
	else
		handle_rod_drop_players = 1u;
	depth = (uint16_t)OBJECT(current_object)->screen_y;
	OBJECT(current_object)->hitbox_size_x = depth;
	queue_sprite_at_depth(&ignored, &depth);
	add_to_rider_list();
}

void handle_rod_start_swing(void)
{
	handle_rod_drop_players = 0u;
	players_pulled = 0u;
	OBJECT(current_object)->handler = handle_rod_swing_ADDRESS;
	handle_rod_swing();
}

void handle_rod_swing(void)
{
	uint32_t cursor = 0, depth = 0, ignored = 0;

	if (OBJECT(current_object)->handle_rod.end_position < scroll_position)
	{
		OBJECT(current_object)->handle_rod.end_position = 0xFFFFu;
		OBJECT(current_object)->handler = handle_rod_remove_ADDRESS;
		return;
	}
	unproject_object(&ignored, &ignored, &ignored, &cursor);
	if (OBJECT(current_object)->handle_rod.swing_side != 0u)
	{
		if (OBJECT(current_object)->handle_rod.drops_players == 0u)
		{
			if ((int16_t)OBJECT(current_object)->handle_rod.angle >= 0x200)
			{
				if (OBJECT(current_object)->handle_rod.angle_speed < -0x100)
					OBJECT(current_object)->handle_rod.angle_speed += 0x100u;
				OBJECT(current_object)->handle_rod.angle_speed++;
				OBJECT(current_object)->handle_rod.swing_accel = 0u - OBJECT(current_object)->handle_rod.swing_accel;
				OBJECT(current_object)->handle_rod.swing_side = 0u;
			}
		}
		else
		{
			OBJECT(current_object)->handle_rod.angle = 0u;
			OBJECT(current_object)->handle_rod.angle_speed = (int16_t)OBJECT(current_object)->handle_rod.angle_speed >> 1;
			OBJECT(current_object)->handle_rod.angle_speed = 0u - OBJECT(current_object)->handle_rod.angle_speed;
			OBJECT(current_object)->handle_rod.swing_accel = 0u - OBJECT(current_object)->handle_rod.swing_accel;
			OBJECT(current_object)->handle_rod.swing_side = 0u;
		}
		OBJECT(current_object)->handle_rod.angle_speed += OBJECT(current_object)->handle_rod.swing_accel;
		OBJECT(current_object)->handle_rod.angle += OBJECT(current_object)->handle_rod.angle_speed_int;
		OBJECT(current_object)->handle_rod.angle &= 0x3FFu;
	}
	else
	{
		if (OBJECT(current_object)->handle_rod.angle != 0u || OBJECT(current_object)->handle_rod.angle_speed > 0x100)
		{
			if ((int16_t)OBJECT(current_object)->handle_rod.angle > 0x200)
			{
				OBJECT(current_object)->handle_rod.angle_speed += OBJECT(current_object)->handle_rod.swing_accel;
				OBJECT(current_object)->handle_rod.angle += OBJECT(current_object)->handle_rod.angle_speed_int;
				OBJECT(current_object)->handle_rod.angle &= 0x3FFu;
				depth = (uint16_t)OBJECT(current_object)->screen_y;
				OBJECT(current_object)->hitbox_size_x = depth;
				queue_sprite_at_depth(&ignored, &depth);
				add_to_rider_list();
				return;
			}
			if (OBJECT(current_object)->handle_rod.angle_speed > 0x100)
				OBJECT(current_object)->handle_rod.angle_speed -= 0x100u;
			OBJECT(current_object)->handle_rod.angle_speed--;
			OBJECT(current_object)->handle_rod.swing_accel = 0u - OBJECT(current_object)->handle_rod.swing_accel;
			OBJECT(current_object)->handle_rod.swing_side = 1u;
			OBJECT(current_object)->handle_rod.angle_speed += OBJECT(current_object)->handle_rod.swing_accel;
			OBJECT(current_object)->handle_rod.angle += OBJECT(current_object)->handle_rod.angle_speed_int;
			OBJECT(current_object)->handle_rod.angle &= 0x3FFu;
			depth = (uint16_t)OBJECT(current_object)->screen_y;
			OBJECT(current_object)->hitbox_size_x = depth;
			queue_sprite_at_depth(&ignored, &depth);
			add_to_rider_list();
			return;
		}
		OBJECT(current_object)->handle_rod.angle = 0u;
		OBJECT(current_object)->handler = handle_rod_hang_ADDRESS;
	}
	depth = (uint16_t)OBJECT(current_object)->screen_y;
	OBJECT(current_object)->hitbox_size_x = depth;
	queue_sprite_at_depth(&ignored, &depth);
	add_to_rider_list();
}

void handle_rod_hang(void)
{
	uint32_t r1 = 0, cursor = 0, depth = 0, ignored = 0, z = 0;

	if ((OBJECT(current_object)->flags & 8u) == 0u && OBJECT(current_object)->floor_z_int >= 0x800)
		fall_to_floor(&ignored, &r1, &z);
	move_object_projected(&ignored, &ignored, &ignored, &cursor);
	if (OBJECT(current_object)->handle_rod.end_position < scroll_position)
	{
		OBJECT(current_object)->handle_rod.end_position = 0xFFFFu;
		OBJECT(current_object)->handler = handle_rod_remove_ADDRESS;
		return;
	}
	depth = (uint16_t)OBJECT(current_object)->screen_y;
	OBJECT(current_object)->hitbox_size_x = depth;
	queue_sprite_at_depth(&ignored, &depth);
	add_to_rider_list();
}

void handle_rod_remove(void)
{
	uint32_t block_offset = 0, ignored = 0, slope_height = 0;

	delete_object_and_children(&ignored, &block_offset, &slope_height);
}

void handle_rod_handle_update(void)
{
	uint32_t cursor = 0, depth = 0, handle_distance = 0, ignored = 0, rod_angle = 0, rod_dx = 0, rod_dz = 0;

	r14 = LINKED(WORK_RAM, OBJECT(current_object)->parent);
	if (OBJECT(r14)->spawn_arg_2 == 0xFFFFu)
	{
		delete_object_and_children(&ignored, &rod_dz, &handle_distance);
		return;
	}
	if (OBJECT(current_object)->handle_rod_handle.grip == 0u)
		move_object_projected_no_floor();
	else
	{
		move_object_projected(&ignored, &ignored, &ignored, &cursor);
		add_to_touchable_list(ignored);
		add_to_rider_list();
	}
	OBJECT(current_object)->speed_x = 0;
	OBJECT(current_object)->speed_y = 0;
	OBJECT(current_object)->speed_z = 0;
	if (OBJECT(current_object)->handle_rod_handle.start_angle == 0x300u || OBJECT(r14)->var_82 != 0u)
	{
		rod_angle = OBJECT(r14)->var_82;
		handle_distance = OBJECT(current_object)->handle_rod_handle.index * 0x12u;
		rod_dx = sine_table[rod_angle] * 2u * 2u * handle_distance;
		rod_dz = cosine_table[rod_angle] * 2u * 2u * handle_distance;
		OBJECT(current_object)->speed_x = rod_dx;
		OBJECT(current_object)->speed_y = 0u - ((int32_t)rod_dx >> 1);
		OBJECT(current_object)->speed_z = rod_dz;
	}
	else
	{
		OBJECT(current_object)->speed_x = 0;
		OBJECT(current_object)->speed_y = 0;
		OBJECT(current_object)->speed_z = 0x10000u * (OBJECT(current_object)->handle_rod_handle.index * 0x12u);
	}
	OBJECT(current_object)->projected_x = OBJECT(r14)->projected_x;
	OBJECT(current_object)->projected_y = OBJECT(r14)->projected_y;
	OBJECT(current_object)->z = OBJECT(r14)->z;
	OBJECT(current_object)->floor_z = OBJECT(r14)->floor_z;
	if (OBJECT(current_object)->screen_x <= 0x145 && OBJECT(current_object)->screen_y >= 0)
	{
		depth = OBJECT(current_object)->handle_rod_handle.index;
		depth = (uint16_t)(depth * 2u);
		depth = (uint16_t)(depth * 2u);
		depth = (uint16_t)(depth + OBJECT(r14)->hitbox_size_x);
		queue_sprite_at_depth(&ignored, &depth);
		if (AT8(OBJECT(r14)->spawn_arg_1, 1) == 0u)
		{
			current_object = LINKED(current_object, OBJECT(current_object)->child);
			place_sprite_at_cursor_floor();
			queue_sprite(depth);
		}
	}
}

void handle_rod_post_init(void)
{
	uint32_t r3 = 0, cursor = 0, ignored = 0, object = 0;

	unproject_object(&ignored, &ignored, &r3, &cursor);
	add_to_rider_list();
	OBJECT(current_object)->sprite_frame = 0x74C8u;
	if (OBJECT(current_object)->handle_rod_post.fallen != 0u)
		OBJECT(current_object)->sprite_frame = 0x74D8u;
	OBJECT(current_object)->sprite_palette = 0x9060u;
	OBJECT(current_object)->handler = handle_rod_post_update_ADDRESS;
	if (OBJECT(current_object)->handle_rod_post.fallen == 0u)
	{
		spawn_count = 1u;
		add_pieces_to_current_object(&ignored, &ignored, &r3, &object);
		OBJECT(object)->sprite_frame = 0x74D0u;
		OBJECT(object)->sprite_palette = OBJECT(object_cursor)->sprite_palette;
	}
}
