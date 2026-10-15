/**
 *    Name:        player/contacts.c
 *    Description: check_player_contacts: what touching each kind of object does to the player
 *    Version:     0.1.0
 *    Created:     12-10-2026
 *    Modified:    15-10-2026
 *    Author:      Sebastian Toyos
 *
 *    SegaSonic The Hedgehog (arcade, Sega System 32): the game's V60 program decompiled into C by
 *    tools/decompile.py, with the names of symbols.txt.
 */
#include "game.h"

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
