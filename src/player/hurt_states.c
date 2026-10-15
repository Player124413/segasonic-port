/**
 *    Name:        player/hurt_states.c
 *    Description: states 0xB-0x17 and 0x2E: not playing, skidding, game over, a short animation, hopping, dying, launched, hurt, bouncing
 *    Version:     0.1.0
 *    Created:     12-10-2026
 *    Modified:    15-10-2026
 *    Author:      Sebastian Toyos
 *
 *    SegaSonic The Hedgehog (arcade, Sega System 32): the game's V60 program decompiled into C by
 *    tools/decompile.py, with the names of symbols.txt.
 */
#include "game.h"

void player_state_not_playing(void)
{
	uint32_t joined = 0, player = 0, text = 0, text_position = 0;

	OBJECT(current_object)->flags |= 0x400u;
	PLAYER(current_player)->in_game = 0;
	OBJECT(current_object)->health = 0;
	PLAYER(current_player)->enter_on_respawn = 0u;
	player_status[OBJECT(current_object)->player] = 0u;
	if (joining_closed == 0u)
	{
		joined = try_join();
		if (joined == 0u)
			return;
		OBJECT(current_object)->handler = player_join_ADDRESS;
		PLAYER(current_player)->substate = 0u;
		PLAYER(current_player)->is_joining = 0xFFu;
		PLAYER(current_player)->in_game = 1;
		PLAYER(current_player)->rings = 0u;
		PLAYER(current_player)->stages_reached = 1u;
		text = hud_message_blank_text_ADDRESS;
		player = OBJECT(current_object)->player;
		text_position = 0x310000u | hud_text_positions_3p[player];
		if ((cabinet_config & 1u) == 0u)
			text_position = (text_position & ~0xFFFFu) | hud_text_positions_2p[player];
		erase_text_lines(&text, &text_position);
	}
}

void player_animate_skidding(uint32_t depth, uint32_t cursor)
{
	uint8_t sound_id = 0;
	uint32_t r8 = 0, ground_slowdown = 0, ignored = 0, skid_animations = 0, skid_facing = 0, speed_slowdown = 0, write_index_address = 0;

	PLAYER(current_player)->control_lock = 0xFFu;
	start_jump_if_pressed();
	if (PLAYER(current_player)->state == 4u)
	{
		PLAYER(current_player)->control_lock = 0u;
		PLAYER(current_player)->free_turn_timer = 3u;
		PLAYER(current_player)->animation_speed = 0;
		player_animate(depth, cursor);
		return;
	}
	switch (PLAYER(current_player)->substate)
	{
	case 0:
	case 3:
		skid_animations = sonic_skid_animations_ADDRESS;
		if (OBJECT(current_object)->kind != 0u)
		{
			skid_animations = ray_skid_animations_ADDRESS;
			if (OBJECT(current_object)->kind != 1u)
				skid_animations = mighty_skid_animations_ADDRESS;
		}
		skid_facing = PLAYER(current_player)->facing;
		skid_facing = (uint8_t)(skid_facing + 8u);
		OBJECT(current_object)->animation = skid_animations + ROM16(skid_animations + (skid_facing & 0xFu) * 2u);
		OBJECT(current_object)->animation_speed = 3u;
		OBJECT(current_object)->animation_timer = 0;
		OBJECT(current_object)->animation_frame = 0u;
		PLAYER(current_player)->skid_dust_timer = 0;
		speed_slowdown = (int32_t)OBJECT(current_object)->speed >> 3;
		if (OBJECT(current_object)->floor_type == 7u)
			speed_slowdown = (int32_t)speed_slowdown >> 2;
		PLAYER(current_player)->skid_speed_slowdown = speed_slowdown;
		ground_slowdown = (int32_t)PLAYER(current_player)->ground_speed >> 3;
		if (OBJECT(current_object)->floor_type == 7u)
			ground_slowdown = (int32_t)ground_slowdown >> 2;
		PLAYER(current_player)->skid_slowdown = ground_slowdown;
		sound_id = 0x27u;
		play_sound(sound_id, &depth, &write_index_address);
		PLAYER(current_player)->substate++;
		player_animate_skidding_slow_down(cursor);
		return;
	case 1:
		player_animate_skidding_slow_down(cursor);
		return;
	case 2:
		animate_with_flags(&depth, &r8);
		if (OBJECT(current_object)->animation_frame == 0u)
		{
			PLAYER(current_player)->control_lock = 0u;
			PLAYER(current_player)->free_turn_timer = 3u;
			PLAYER(current_player)->state = 1u;
		}
		if ((OBJECT(current_object)->flags & 0x80000u) != 0u)
			return;
		if ((PLAYER(current_player)->flags & 1u) == 0u)
		{
			depth = queue_sprite_at_base(depth);
			return;
		}
		PLAYER(current_player)->draw_depth = ((int32_t)((uint32_t)OBJECT(current_object)->projected_y + OBJECT(current_object)->floor_z) >> 16) - view_y;
		current_object = current_player + 0x102u;
		depth = PLAYER(current_player)->draw_depth;
		queue_sprite_at_depth(&ignored, &depth);
		current_object = object_cursor;
		depth = PLAYER(current_player)->draw_depth;
		queue_sprite_at_depth(&ignored, &depth);
		current_object = current_player + 0xF0u;
		depth = PLAYER(current_player)->draw_depth;
		queue_sprite_at_depth(&ignored, &depth);
		return;
	default:
		unknown_code(0x4EF5u + PLAYER(current_player)->substate * 3u);
	}
}

void player_animate_skidding_slow_down(uint32_t cursor)
{
	uint32_t r1 = 0, r8 = 0, depth = 0, ignored = 0;
	uint8_t skid_dust_timer = 0;
	uint32_t skid_speed_slowdown = 0, speed = 0;

	skid_dust_timer = PLAYER(current_player)->skid_dust_timer;
	PLAYER(current_player)->skid_dust_timer = skid_dust_timer - 1u;
	if ((int8_t)skid_dust_timer <= 1)
	{
		PLAYER(current_player)->skid_dust_timer = 4;
		if (OBJECT(current_object)->floor_type != 7u && (PLAYER(current_player)->flags & 1u) == 0u)
			spawn_skid_dust(cursor);
	}
	if ((int8_t)OBJECT(current_object)->animation_speed >= 0)
	{
		animate_with_flags(&r1, &r8);
		if ((int8_t)OBJECT(current_object)->animation_frame >= 2)
			OBJECT(current_object)->animation_speed = 0xFFu;
	}
	PLAYER(current_player)->ground_speed -= PLAYER(current_player)->skid_slowdown;
	skid_speed_slowdown = PLAYER(current_player)->skid_speed_slowdown;
	speed = OBJECT(current_object)->speed;
	OBJECT(current_object)->speed = speed - skid_speed_slowdown;
	if ((int32_t)speed <= (int32_t)skid_speed_slowdown)
	{
		OBJECT(current_object)->speed = 0;
		PLAYER(current_player)->ground_speed = 0;
		OBJECT(current_object)->animation_speed = 3u;
		PLAYER(current_player)->substate++;
	}
	if ((OBJECT(current_object)->flags & 0x80000u) != 0u)
		return;
	if ((PLAYER(current_player)->flags & 1u) == 0u)
	{
		r1 = queue_sprite_at_base(r1);
		return;
	}
	PLAYER(current_player)->draw_depth = ((int32_t)((uint32_t)OBJECT(current_object)->projected_y + OBJECT(current_object)->floor_z) >> 16) - view_y;
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

void player_state_game_over(uint32_t jump_offset)
{
	if (PLAYER(current_player)->state_timer <= 0)
	{
		PLAYER(current_player)->state_timer++;
		start_ranking_entry();
		jump_offset = OBJECT(current_object)->player;
		player_status[jump_offset] = 3u;
	}
	enter_ranking_initials(jump_offset);
	if (PLAYER(current_player)->game_over_done != 0u)
	{
		PLAYER(current_player)->state = 0x24u;
		PLAYER(current_player)->continue_frames = 0;
		PLAYER(current_player)->continue_seconds = 0;
		PLAYER(current_player)->state_timer = 0;
	}
}

void player_state_short_animation(void)
{
	uint32_t r7 = 0, at_last_step = 0, frame = 0, ignored = 0, state_1b_animation = 0;
	uint16_t state_timer = 0;

	if (PLAYER(current_player)->substate == 0u)
	{
		PLAYER(current_player)->substate++;
		state_1b_animation = sonic_state_1b_animation_ADDRESS;
		if (OBJECT(current_object)->kind != 0u)
		{
			state_1b_animation = ray_state_1b_animation_ADDRESS;
			if (OBJECT(current_object)->kind != 1u)
				state_1b_animation = mighty_state_1b_animation_ADDRESS;
		}
		OBJECT(current_object)->animation = state_1b_animation;
		OBJECT(current_object)->animation_frame = 0u;
		OBJECT(current_object)->animation_speed = 5u;
		OBJECT(current_object)->animation_timer = 0;
		PLAYER(current_player)->state_timer = 0x41;
		OBJECT(current_object)->sprite_palette = mem_read16(OBJECT(current_object)->player_view.palette_list + 2u);
	}
	if ((int8_t)OBJECT(current_object)->animation_frame >= 0)
	{
		animate(&frame, &r7, &at_last_step);
		if ((uint8_t)at_last_step != 0u)
			OBJECT(current_object)->animation_frame = 0xFFu;
	}
	state_timer = PLAYER(current_player)->state_timer;
	PLAYER(current_player)->state_timer = state_timer - 1u;
	if ((int16_t)state_timer <= 1)
	{
		PLAYER(current_player)->grey_screen_request = 0u;
		PLAYER(current_player)->state = 0u;
		PLAYER(current_player)->substate = 0u;
	}
	project_object(&ignored, &ignored);
	ignored = queue_sprite_at_base(ignored);
}

void player_state_inactive_2(void)
{
}

void player_animate_hopping(void)
{
	uint32_t r8 = 0, frame = 0, ignored = 0, running_animations = 0, speed_int = 0;

	if (PLAYER(current_player)->substate == 0u)
	{
		PLAYER(current_player)->substate++;
		OBJECT(current_object)->speed_z = -0x80000;
		if (OBJECT(current_object)->floor_type == 6u)
		{
			speed_int = (uint16_t)OBJECT(current_object)->speed_int;
			OBJECT(current_object)->speed_z_int = hop_speeds_by_speed[speed_int >> 8];
		}
		OBJECT(current_object)->sprite_palette = mem_read16(OBJECT(current_object)->player_view.palette_list + 2u);
		OBJECT(current_object)->flags &= ~0x40u;
		OBJECT(current_object)->animation_speed = 1u;
		OBJECT(current_object)->animation_timer = 0;
		OBJECT(current_object)->animation_frame = 0xFFu;
	}
	bounce_off_wall_in_air();
	PLAYER(current_player)->jump_delay = 2;
	running_animations = sonic_running_animations_ADDRESS;
	if (OBJECT(current_object)->kind != 0u)
	{
		running_animations = ray_running_animations_ADDRESS;
		if (OBJECT(current_object)->kind != 1u)
			running_animations = mighty_running_animations_ADDRESS;
	}
	OBJECT(current_object)->animation = running_animations + ROM16(running_animations + PLAYER(current_player)->facing * 2u);
	animate_with_flags(&frame, &r8);
	OBJECT(current_object)->speed_z += 0x6000u;
	OBJECT(current_object)->z += OBJECT(current_object)->speed_z;
	if (OBJECT(current_object)->floor_z <= OBJECT(current_object)->z)
	{
		PLAYER(current_player)->state = 1u;
		PLAYER(current_player)->substate = 0u;
		OBJECT(current_object)->z = OBJECT(current_object)->floor_z;
		PLAYER(current_player)->jump_delay = 0;
		OBJECT(current_object)->flags &= ~0x2000u;
		OBJECT(current_object)->flags &= ~0x100000u;
		OBJECT(current_object)->flags &= ~0x200000u;
	}
	if ((OBJECT(current_object)->flags & 0x80000u) != 0u)
		return;
	if (OBJECT(current_object)->floor_block >= 0)
	{
		ignored = queue_sprite_at_floor();
		return;
	}
	ignored = queue_sprite_at_previous_floor();
}

void player_state_dying(void)
{
	uint16_t character_frame = 0;
	uint32_t depth = 0, direction = 0, ignored = 0, speed_x_int = 0;
	uint16_t state_timer = 0;

	if (PLAYER(current_player)->substate == 0u)
	{
		PLAYER(current_player)->substate = 1u;
		PLAYER(current_player)->screen_x_int = OBJECT(current_object)->screen_x;
		AS16(PLAYER(current_player)->screen_x) = 0u;
		if (PLAYER(current_player)->screen_x_int < 0x20)
			PLAYER(current_player)->screen_x_int = 0x20;
		if (PLAYER(current_player)->screen_x_int > 0x100)
			PLAYER(current_player)->screen_x_int = 0x100;
		OBJECT(current_object)->projected_y_int = OBJECT(current_object)->screen_y;
		if (OBJECT(current_object)->projected_y_int > 0xE0)
		{
			OBJECT(current_object)->projected_y_int = 0xE0;
			if (PLAYER(current_player)->screen_x_int < 0x78)
				PLAYER(current_player)->screen_x_int = 0x78;
			if (PLAYER(current_player)->screen_x_int > 0xC8)
				PLAYER(current_player)->screen_x_int = 0xC8;
		}
		OBJECT(current_object)->projected_y_fraction = 0u;
		OBJECT(current_object)->direction = 0u;
		OBJECT(current_object)->speed_y = -0x10000;
		PLAYER(current_player)->state_timer = 0x40;
		OBJECT(current_object)->speed_x = 0x60000;
		OBJECT(current_object)->sprite_flags &= 0xFFBFu;
		OBJECT(current_object)->wall = 7u;
		if (PLAYER(current_player)->screen_x_int < 0xA0)
		{
			OBJECT(current_object)->sprite_flags |= 0x40u;
			OBJECT(current_object)->wall = 0u - OBJECT(current_object)->wall;
		}
		character_frame = 0x1B50u;
		if (OBJECT(current_object)->kind != 0u)
		{
			character_frame = 0x5270u;
			if (OBJECT(current_object)->kind != 1u)
				character_frame = 0x36B8u;
		}
		OBJECT(current_object)->sprite_frame = character_frame;
		OBJECT(current_object)->sprite_palette = mem_read16(OBJECT(current_object)->player_view.palette_list + 2u);
		OBJECT(current_object)->sprite_palette &= 0x9FFFu;
		OBJECT(current_object)->flags |= 0x400u;
		player_status[OBJECT(current_object)->player] = 1u;
	}
	state_timer = PLAYER(current_player)->state_timer;
	PLAYER(current_player)->state_timer = state_timer - 1u;
	if ((int16_t)state_timer <= 1)
	{
		PLAYER(current_player)->state_timer = 0;
		OBJECT(current_object)->speed_x += 0x8000u;
	}
	speed_x_int = (uint16_t)OBJECT(current_object)->speed_x_int;
	OBJECT(current_object)->direction += OBJECT(current_object)->wall;
	direction = OBJECT(current_object)->direction;
	PLAYER(current_player)->screen_x += sine_table[direction & 0x3FFu] * speed_x_int;
	OBJECT(current_object)->screen_x = PLAYER(current_player)->screen_x_int;
	if (PLAYER(current_player)->state_timer <= 0)
		OBJECT(current_object)->speed_y -= 0x3000u;
	OBJECT(current_object)->projected_y += OBJECT(current_object)->speed_y;
	OBJECT(current_object)->screen_y = OBJECT(current_object)->projected_y_int;
	if (OBJECT(current_object)->screen_y <= -0x28)
	{
		player_status[OBJECT(current_object)->player] = 0xFFu;
		AS8(PLAYER(current_player)->previous_z) = 0u;
		PLAYER(current_player)->in_game = 0;
		PLAYER(current_player)->continue_seconds = 9;
		PLAYER(current_player)->continue_frames = 0;
		PLAYER(current_player)->state = 0x23u;
	}
	PLAYER(current_player)->invulnerable_timer = 0x3C;
	toggle_hidden();
	if ((OBJECT(current_object)->flags & 0x80000u) != 0u)
		return;
	depth = 0xE0u;
	queue_sprite_at_depth(&ignored, &depth);
}

void player_state_launched(void)
{
	uint32_t r8 = 0, depth = 0, frame = 0, ignored = 0;

	switch (PLAYER(current_player)->substate)
	{
	case 0:
		player_launch_angle_100();
		break;
	case 1:
		player_launch_angle_200();
		break;
	case 2:
		player_launched_fly();
		break;
	case 3:
		player_launched_fly();
		break;
	case 4:
		player_launched_land();
		break;
	case 5:
		player_launch_angle_200();
		break;
	default:
		unknown_code(0x5437u + PLAYER(current_player)->substate * 3u);
	}
	animate_with_flags(&frame, &r8);
	AS8(temp_0) = OBJECT(current_object)->sprite_offset_y;
	OBJECT(current_object)->sprite_offset_y = 0x12;
	project_object(&ignored, &depth);
	OBJECT(current_object)->sprite_offset_y = AS8(temp_0);
	keep_player_on_screen(depth);
	add_player_to_lists();
	if (OBJECT(current_object)->floor_block >= 0)
	{
		ignored = queue_sprite_at_floor();
		return;
	}
	ignored = queue_sprite_at_previous_floor();
}

void player_state_launched_spare_return(void)
{
}

void player_launch_angle_200(void)
{
	OBJECT(current_object)->direction = 0x200u;
	player_launch();
}

void player_launch_angle_100(void)
{
	OBJECT(current_object)->direction = 0x100u;
	PLAYER(current_player)->substate = 1u;
	player_launch();
}

void player_launch(void)
{
	uint32_t animation_record = 0;

	face_movement_direction(0u);
	OBJECT(current_object)->speed_z = -0x100000;
	OBJECT(current_object)->speed = 0x5000000;
	set_speed_from_direction();
	animation_record = sonic_attack_jump_animations_ADDRESS;
	if (OBJECT(current_object)->kind != 0u)
	{
		animation_record = ray_attack_jump_animations_ADDRESS;
		if (OBJECT(current_object)->kind != 1u)
			animation_record = mighty_attack_jump_animations_ADDRESS;
	}
	animation_record += ROM16(animation_record + PLAYER(current_player)->facing * 2u);
	OBJECT(current_object)->sprite_flags = (OBJECT(current_object)->sprite_flags & ~0xC0u) | ROM16(animation_record);
	animation_record += 2u;
	OBJECT(current_object)->sprite_frame = ROM16(animation_record);
	OBJECT(current_object)->animation = animation_record + 2u;
	OBJECT(current_object)->animation_speed = 3u;
	OBJECT(current_object)->animation_timer = 0;
	OBJECT(current_object)->animation_frame = 0u;
	OBJECT(current_object)->flags &= ~0x40u;
	OBJECT(current_object)->sprite_palette = mem_read16(OBJECT(current_object)->player_view.palette_list + 2u);
	OBJECT(current_object)->flags &= ~0x2000u;
	PLAYER(current_player)->control_lock = 0u;
	OBJECT(current_object)->flags |= 0x1000u;
	PLAYER(current_player)->substate++;
	player_launched_fly();
}

void player_launched_fly(void)
{
	uint32_t block_offset = 0, cursor = 0, ignored = 0, x = 0, y = 0;

	if (OBJECT(current_object)->screen_x >= 0x120)
	{
		OBJECT(current_object)->speed_x = 0u - OBJECT(current_object)->speed_x;
		OBJECT(current_object)->speed_y = 0u - OBJECT(current_object)->speed_y;
		OBJECT(current_object)->speed_z = 0u - OBJECT(current_object)->speed_z;
	}
	OBJECT(current_object)->speed_z += 0xA000u;
	OBJECT(current_object)->z += OBJECT(current_object)->speed_z;
	PLAYER(current_player)->previous_x = OBJECT(current_object)->x;
	OBJECT(current_object)->x += OBJECT(current_object)->speed_x;
	PLAYER(current_player)->previous_y = OBJECT(current_object)->y;
	OBJECT(current_object)->y += OBJECT(current_object)->speed_y;
	find_floor(&x, &y, &block_offset, &ignored, &cursor);
	if (OBJECT(current_object)->player_view.floor_block_low >= 0 && OBJECT(current_object)->floor_z_int <= OBJECT(current_object)->z_int)
	{
		PLAYER(current_player)->substate++;
		OBJECT(current_object)->z = OBJECT(current_object)->floor_z;
		OBJECT(current_object)->speed_x = (int32_t)OBJECT(current_object)->speed_x >> 1;
		OBJECT(current_object)->speed_y = (int32_t)OBJECT(current_object)->speed_y >> 1;
		OBJECT(current_object)->speed_z = 0u - ((int32_t)OBJECT(current_object)->speed_z >> 1);
	}
}

void player_launched_land(void)
{
	OBJECT(current_object)->speed_z = 0;
	PLAYER(current_player)->state = 1u;
	PLAYER(current_player)->substate = 0u;
	OBJECT(current_object)->flags &= ~0x1000u;
	OBJECT(current_object)->speed = 0;
	PLAYER(current_player)->animation_speed = 0;
	OBJECT(current_object)->speed_x = 0;
	OBJECT(current_object)->speed_y = 0;
}

void player_state_hurt(uint32_t depth, uint32_t cursor)
{
	uint8_t sound_id = 0;
	uint32_t at_last_step = 0, gravity = 0, hurt_animations = 0, ignored = 0, jump_offset = 0, kind = 0, knock_speed = 0, knock_speed_x = 0, knock_speed_y = 0, object = 0, shocked_animations = 0, step_record = 0;
	uint8_t hurt_contact_delay = 0, health = 0, health_2 = 0;
	uint16_t state_timer = 0, state_timer_2 = 0, state_timer_3 = 0;
	uint32_t push_speed = 0, push_speed_2 = 0;

	for (;;)
	{
		hurt_contact_delay = PLAYER(current_player)->hurt_contact_delay;
		PLAYER(current_player)->hurt_contact_delay = hurt_contact_delay - 1u;
		if ((int8_t)hurt_contact_delay <= 1)
		{
			PLAYER(current_player)->hurt_contact_delay = 0;
			check_zone_contact();
			check_player_contacts(&depth, &cursor);
			if (PLAYER(current_player)->state != 0x28u)
				goto player_hurt_dispatch_substate;
			goto player_dispatch_state;
		}
	player_hurt_dispatch_substate:
		push_player_out_of_wall(&jump_offset, &depth, &cursor);
		PLAYER(current_player)->state = 0xDu;
		switch (PLAYER(current_player)->substate)
		{
		case 0:
		case 9:
			if ((uint8_t)OBJECT(current_object)->health == 0u)
			{
				PLAYER(current_player)->state = 0x16u;
				PLAYER(current_player)->substate = 0u;
				goto player_dispatch_state;
			}
			if (stage != 5u)
			{
				health = OBJECT(current_object)->health;
				OBJECT(current_object)->health = health - 4u;
				if ((int8_t)health <= 4)
					OBJECT(current_object)->health = 0;
			}
			PLAYER(current_player)->invulnerable_timer = 0x258;
			OBJECT(current_object)->flags &= ~0x1000000u;
		player_hurt_knockback:
			OBJECT(current_object)->flags &= ~4u;
			OBJECT(current_object)->flags &= ~0x40u;
			OBJECT(current_object)->flags &= ~0x2000u;
			PLAYER(current_player)->control_lock = 0u;
			PLAYER(current_player)->mishap_flag = 0xFFu;
			PLAYER(current_player)->hurt_contact_delay = 0xA;
			PLAYER(current_player)->substate = 2u;
			OBJECT(current_object)->speed_z = -0x80000;
			if (OBJECT(current_object)->floor_type == 4u)
				OBJECT(current_object)->speed_z = (int32_t)OBJECT(current_object)->speed_z >> 1;
			object = LINKED(WORK_RAM, OBJECT(current_object)->touched_by);
			if (OBJECT(object)->kind != 0x24u)
			{
				knock_speed_x = 0x40000u;
				if (OBJECT(current_object)->floor_type == 4u)
					knock_speed_x = (int32_t)knock_speed_x >> 1;
				knock_speed_y = 0u;
				jump_offset = OBJECT(object)->direction;
				OBJECT(current_object)->direction = jump_offset;
				OBJECT(current_object)->speed = knock_speed_x << 8;
				if (jump_offset != 0u)
				{
					if (jump_offset != 0x200u)
					{
						if (jump_offset != 0x100u)
							knock_speed_x = 0u - knock_speed_x;
					}
					else
					{
						push_speed = knock_speed_x;
						knock_speed_x = knock_speed_y;
						knock_speed_y = push_speed;
					}
				}
				else
				{
					push_speed_2 = knock_speed_x;
					knock_speed_x = knock_speed_y;
					knock_speed_y = 0u - push_speed_2;
				}
			}
			else
			{
				knock_speed_x = 0u;
				knock_speed_y = 0u;
			}
			OBJECT(current_object)->speed_x = knock_speed_x;
			OBJECT(current_object)->speed_y = knock_speed_y;
			face_movement_direction(jump_offset);
			sound_id = 0x21u;
			play_sound(sound_id, &depth, &knock_speed_x);
			sound_id = 0x55u;
			if (OBJECT(current_object)->kind != 0u)
			{
				sound_id = 0x4Au;
				if (OBJECT(current_object)->kind != 1u)
					sound_id = 0x5Bu;
			}
			play_sound(sound_id, &depth, &knock_speed_x);
		player_hurt_set_animation:
			hurt_animations = sonic_hurt_animations_ADDRESS;
			if (OBJECT(current_object)->kind != 0u)
			{
				hurt_animations = ray_hurt_animations_ADDRESS;
				if (OBJECT(current_object)->kind != 1u)
					hurt_animations = mighty_hurt_animations_ADDRESS;
			}
			depth = ROM16(hurt_animations + PLAYER(current_player)->facing * 2u);
			OBJECT(current_object)->animation = hurt_animations + depth;
			OBJECT(current_object)->animation_frame = 0u;
			step_record = OBJECT(current_object)->animation;
			OBJECT(current_object)->sprite_flags &= ~0xC0u;
			step_record += 2u;
			OBJECT(current_object)->sprite_flags |= mem_read16(step_record);
			OBJECT(current_object)->sprite_frame = mem_read16(step_record + 2u);
			OBJECT(current_object)->player_view.hurt_flash_period = 3u;
			if ((OBJECT(current_object)->flags & 0x1000000u) != 0u)
				OBJECT(current_object)->player_view.hurt_flash_period = 0xFFu;
			OBJECT(current_object)->player_view.hurt_flash_timer = 3u;
			OBJECT(current_object)->player_view.hurt_flash_on = 0u;
		case 2:
			gravity = 0x6000u;
			if (OBJECT(current_object)->floor_type == 4u && OBJECT(LINKED(current_object, OBJECT(current_object)->touched_by))->kind != 0x27u)
				gravity = gravity >> 2;
			OBJECT(current_object)->speed_z += gravity;
			PLAYER(current_player)->previous_z = OBJECT(current_object)->z;
			OBJECT(current_object)->z += OBJECT(current_object)->speed_z;
			PLAYER(current_player)->previous_x = OBJECT(current_object)->x;
			PLAYER(current_player)->previous_y = OBJECT(current_object)->y;
			OBJECT(current_object)->x += OBJECT(current_object)->speed_x;
			OBJECT(current_object)->y += OBJECT(current_object)->speed_y;
			find_floor(&gravity, &depth, &knock_speed_x, &ignored, &cursor);
			check_fall_into_pit();
			if (PLAYER(current_player)->state != 9u)
			{
				if ((int16_t)(OBJECT(current_object)->z_int - OBJECT(current_object)->floor_z_int) >= 8)
				{
					OBJECT(current_object)->x = PLAYER(current_player)->previous_x;
					OBJECT(current_object)->y = PLAYER(current_player)->previous_y;
					OBJECT(current_object)->floor_block = OBJECT(current_object)->previous_floor_block;
					OBJECT(current_object)->floor_z = OBJECT(current_object)->previous_floor_z;
				}
				if (OBJECT(current_object)->floor_z_int <= OBJECT(current_object)->z_int)
				{
					PLAYER(current_player)->substate++;
					PLAYER(current_player)->state_timer = 2;
					OBJECT(current_object)->z = OBJECT(current_object)->floor_z;
					sound_id = 0x20u;
					play_sound(sound_id, &depth, &knock_speed_x);
					OBJECT(current_object)->animation_frame++;
					step_record = OBJECT(current_object)->animation + 2u + OBJECT(current_object)->animation_frame * 4u;
					OBJECT(current_object)->sprite_flags &= ~0xC0u;
					OBJECT(current_object)->sprite_flags |= mem_read16(step_record);
					OBJECT(current_object)->sprite_frame = mem_read16(step_record + 2u);
					add_player_to_lists();
					project_object(&ignored, &depth);
					if (current_game_mode != 0xDu)
						keep_player_on_screen(depth);
					ignored = queue_sprite_at_floor();
					return;
				}
				if (OBJECT(current_object)->animation_frame == 0u && OBJECT(current_object)->speed_z_int >= 0)
				{
					OBJECT(current_object)->animation_frame++;
					step_record = OBJECT(current_object)->animation + 2u + OBJECT(current_object)->animation_frame * 4u;
					OBJECT(current_object)->sprite_flags &= ~0xC0u;
					OBJECT(current_object)->sprite_flags |= mem_read16(step_record);
					OBJECT(current_object)->sprite_frame = mem_read16(step_record + 2u);
				}
				add_player_to_lists();
				project_object(&ignored, &depth);
				if (current_game_mode != 0xDu)
					keep_player_on_screen(depth);
				ignored = queue_sprite_at_floor();
				return;
			}
			OBJECT(current_object)->animation_frame = 0xFFu;
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
				player_state_climbing(depth, cursor);
				return;
			case 11:
				player_state_not_playing();
				return;
			case 12:
				player_move(depth, cursor);
				return;
			case 13:
				continue;
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
			break;
		case 1:
			OBJECT(current_object)->flags |= 0x1000000u;
			goto player_hurt_knockback;
		case 3:
			state_timer = PLAYER(current_player)->state_timer;
			PLAYER(current_player)->state_timer = state_timer - 1u;
			if ((int16_t)state_timer <= 1)
			{
				PLAYER(current_player)->substate++;
				OBJECT(current_object)->speed_z = -0x40000;
				if (OBJECT(current_object)->floor_type == 4u)
					OBJECT(current_object)->speed_z = (int32_t)OBJECT(current_object)->speed_z >> 1;
				OBJECT(current_object)->animation_timer = 0;
				OBJECT(current_object)->animation_speed = 3u;
				OBJECT(current_object)->speed_x = 0;
				OBJECT(current_object)->speed_y = 0;
			}
			if ((int8_t)OBJECT(current_object)->player_view.hurt_flash_period >= 0)
				flash_hurt_palette();
			find_floor(&jump_offset, &depth, &knock_speed_x, &ignored, &cursor);
			project_object(&ignored, &depth);
			keep_player_on_screen(depth);
			add_player_to_lists();
			depth = queue_sprite_at_base(depth);
			return;
		case 4:
			jump_offset = 0x6000u;
			if (OBJECT(current_object)->floor_type == 4u)
				jump_offset = jump_offset >> 2;
			OBJECT(current_object)->speed_z += jump_offset;
			PLAYER(current_player)->previous_z = OBJECT(current_object)->z;
			OBJECT(current_object)->z += OBJECT(current_object)->speed_z;
			PLAYER(current_player)->previous_x = OBJECT(current_object)->x;
			PLAYER(current_player)->previous_y = OBJECT(current_object)->y;
			OBJECT(current_object)->x += OBJECT(current_object)->speed_x;
			OBJECT(current_object)->y += OBJECT(current_object)->speed_y;
			if (OBJECT(current_object)->z_int < OBJECT(current_object)->floor_z_int)
			{
				if ((int8_t)OBJECT(current_object)->animation_speed >= 0)
				{
					animate_with_flags(&depth, &at_last_step);
					if (OBJECT(current_object)->animation_frame == 5u)
						OBJECT(current_object)->animation_speed = 0xFFu;
				}
				if ((int8_t)OBJECT(current_object)->player_view.hurt_flash_period >= 0)
					flash_hurt_palette();
				find_floor(&jump_offset, &depth, &knock_speed_x, &ignored, &cursor);
				if (current_game_mode == 0x1Au)
				{
					project_object(&ignored, &depth);
					keep_player_on_screen(depth);
					add_player_to_lists();
					ignored = queue_sprite_at_floor();
					return;
				}
				check_fall_into_pit();
				if (PLAYER(current_player)->state != 9u)
				{
					project_object(&ignored, &depth);
					keep_player_on_screen(depth);
					add_player_to_lists();
					ignored = queue_sprite_at_floor();
					return;
				}
				OBJECT(current_object)->animation_frame = 0xFFu;
				goto player_dispatch_state;
			}
			goto player_hurt_land_after_hop;
		case 5:
			goto player_hurt_get_up;
		case 6:
			state_timer_2 = PLAYER(current_player)->state_timer;
			PLAYER(current_player)->state_timer = state_timer_2 - 1u;
			if ((int16_t)state_timer_2 <= 1)
			{
				PLAYER(current_player)->state = 1u;
				OBJECT(current_object)->flags &= ~4u;
				PLAYER(current_player)->substate = 0u;
				OBJECT(current_object)->speed = 0;
				PLAYER(current_player)->animation_speed = 0;
			}
			else
			{
				start_jump_if_pressed();
				if (PLAYER(current_player)->state == 4u)
				{
					OBJECT(current_object)->flags &= ~4u;
					PLAYER(current_player)->substate = 0u;
					OBJECT(current_object)->speed = 0;
					PLAYER(current_player)->animation_speed = 0;
					update_player_blink();
					find_floor(&jump_offset, &depth, &knock_speed_x, &ignored, &cursor);
					OBJECT(current_object)->z = OBJECT(current_object)->floor_z;
					project_object(&ignored, &depth);
					keep_player_on_screen(depth);
					add_player_to_lists();
					if ((OBJECT(current_object)->flags & 0x80000u) != 0u)
						return;
					depth = queue_sprite_at_base(depth);
					return;
				}
				read_player_trackball();
				if ((PLAYER(current_player)->trackball_x | PLAYER(current_player)->trackball_y) != 0u)
				{
					PLAYER(current_player)->state = 1u;
					OBJECT(current_object)->flags &= ~4u;
					PLAYER(current_player)->substate = 0u;
					OBJECT(current_object)->speed = 0;
					PLAYER(current_player)->animation_speed = 0;
					update_player_blink();
					find_floor(&jump_offset, &depth, &knock_speed_x, &ignored, &cursor);
					OBJECT(current_object)->z = OBJECT(current_object)->floor_z;
					project_object(&ignored, &depth);
					keep_player_on_screen(depth);
					add_player_to_lists();
					if ((OBJECT(current_object)->flags & 0x80000u) != 0u)
						return;
					depth = queue_sprite_at_base(depth);
					return;
				}
			}
			update_player_blink();
			find_floor(&jump_offset, &depth, &knock_speed_x, &ignored, &cursor);
			OBJECT(current_object)->z = OBJECT(current_object)->floor_z;
			project_object(&ignored, &depth);
			keep_player_on_screen(depth);
			add_player_to_lists();
			if ((OBJECT(current_object)->flags & 0x80000u) != 0u)
				return;
			depth = queue_sprite_at_base(depth);
			return;
		case 7:
			if ((uint8_t)OBJECT(current_object)->health != 0u)
			{
				health_2 = OBJECT(current_object)->health;
				OBJECT(current_object)->health = health_2 - 4u;
				if ((int8_t)health_2 <= 4)
					OBJECT(current_object)->health = 0;
				PLAYER(current_player)->invulnerable_timer = 0x258;
				OBJECT(current_object)->flags &= ~0x2000u;
				OBJECT(current_object)->flags &= ~0x40u;
				PLAYER(current_player)->control_lock = 0u;
				PLAYER(current_player)->mishap_flag = 0xFFu;
				PLAYER(current_player)->substate++;
				PLAYER(current_player)->state_timer = 4;
				OBJECT(current_object)->speed_z = -0x80000;
				object = LINKED(current_object, OBJECT(current_object)->touched_by);
				if (OBJECT(object)->kind != 0x15u)
				{
					knock_speed = 0x40000u;
					switch (AT8(OBJECT(object)->spawn_arg_2, 1))
					{
					case 0:
					case 4:
						OBJECT(current_object)->speed_x = 0;
						OBJECT(current_object)->speed_y = 0u - knock_speed;
						OBJECT(current_object)->direction = 0x200u;
						break;
					case 1:
						OBJECT(current_object)->speed_x = 0;
						OBJECT(current_object)->speed_y = knock_speed;
						OBJECT(current_object)->direction = 0u;
						break;
					case 2:
						OBJECT(current_object)->speed_x = knock_speed;
						OBJECT(current_object)->speed_y = 0;
						OBJECT(current_object)->direction = 0x300u;
						break;
					case 3:
						OBJECT(current_object)->speed_x = 0u - knock_speed;
						OBJECT(current_object)->speed_y = 0;
						OBJECT(current_object)->direction = 0x100u;
						break;
					default:
						unknown_code(0x5AE7u + AT8(OBJECT(object)->spawn_arg_2, 1) * 2u);
					}
				}
				else
				{
					OBJECT(current_object)->speed_x = 0;
					OBJECT(current_object)->speed_y = 0;
					OBJECT(current_object)->direction = 0x300u;
				}
				face_movement_direction(jump_offset);
				shocked_animations = sonic_shocked_animations_ADDRESS;
				if (OBJECT(current_object)->kind != 0u)
				{
					shocked_animations = ray_shocked_animations_ADDRESS;
					if (OBJECT(current_object)->kind != 1u)
						shocked_animations = mighty_shocked_animations_ADDRESS;
				}
				depth = ROM16(shocked_animations + PLAYER(current_player)->facing * 2u);
				OBJECT(current_object)->animation = shocked_animations + depth;
				OBJECT(current_object)->animation_frame = 0u;
				OBJECT(current_object)->animation_speed = 2u;
				OBJECT(current_object)->animation_timer = 0;
				sound_id = 0x18u;
				play_sound(sound_id, &depth, &knock_speed_x);
				sound_id = 0x2Fu;
				if (OBJECT(current_object)->kind != 0u)
				{
					sound_id = 0x37u;
					if (OBJECT(current_object)->kind != 1u)
						sound_id = 0x77u;
				}
				play_sound(sound_id, &depth, &knock_speed_x);
			player_hurt_shocked:
				animate_with_flags(&depth, &at_last_step);
				if ((uint8_t)at_last_step == 0u)
				{
					kind = OBJECT(current_object)->kind;
					OBJECT(current_object)->sprite_palette = ROM16((uint16_t)(kind << 4) + shocked_palettes_ADDRESS + OBJECT(current_object)->animation_frame * 2u);
					update_screen_position(&ignored, &depth);
					keep_player_on_screen(depth);
					add_player_to_lists();
					depth = queue_sprite_at_base(depth);
					return;
				}
				state_timer_3 = PLAYER(current_player)->state_timer;
				PLAYER(current_player)->state_timer = state_timer_3 - 1u;
				if ((int16_t)state_timer_3 > 1)
				{
					kind = OBJECT(current_object)->kind;
					OBJECT(current_object)->sprite_palette = ROM16((uint16_t)(kind << 4) + shocked_palettes_ADDRESS + OBJECT(current_object)->animation_frame * 2u);
					update_screen_position(&ignored, &depth);
					keep_player_on_screen(depth);
					add_player_to_lists();
					depth = queue_sprite_at_base(depth);
					return;
				}
				PLAYER(current_player)->substate = 2u;
				OBJECT(current_object)->sprite_palette = mem_read16(OBJECT(current_object)->player_view.palette_list + 2u);
				goto player_hurt_set_animation;
			}
			PLAYER(current_player)->state = 0x16u;
			PLAYER(current_player)->substate = 0u;
			goto player_dispatch_state;
		case 8:
			goto player_hurt_shocked;
		default:
			unknown_code(0x55C8u + PLAYER(current_player)->substate * 3u);
		}
	}
player_hurt_land_after_hop:
	PLAYER(current_player)->substate++;
	OBJECT(current_object)->z = OBJECT(current_object)->floor_z;
	OBJECT(current_object)->animation_timer = 0;
player_hurt_get_up:
	OBJECT(current_object)->animation_speed = 3u;
	animate_with_flags(&depth, &at_last_step);
	if ((uint8_t)at_last_step != 0u)
	{
		PLAYER(current_player)->substate++;
		PLAYER(current_player)->mishap_flag = 0u;
		OBJECT(current_object)->sprite_palette = mem_read16(OBJECT(current_object)->player_view.palette_list + 2u);
		set_floor_sprite_priority();
		OBJECT(current_object)->player_view.hurt_flash_timer = 0x7Fu;
		PLAYER(current_player)->state_timer = 0x1E;
		if ((OBJECT(current_object)->flags & 0x1000000u) == 0u)
		{
			PLAYER(current_player)->invulnerable_timer = 0x78;
			OBJECT(current_object)->flags |= 0x20000000u;
		}
		if (OBJECT(current_object)->health <= 0)
		{
			PLAYER(current_player)->state = 0x16u;
			PLAYER(current_player)->substate = 0u;
		}
	}
	if ((int8_t)OBJECT(current_object)->player_view.hurt_flash_period >= 0)
		flash_hurt_palette();
	find_floor(&jump_offset, &depth, &knock_speed_x, &ignored, &cursor);
	OBJECT(current_object)->z = OBJECT(current_object)->floor_z;
	project_object(&ignored, &depth);
	keep_player_on_screen(depth);
	add_player_to_lists();
	depth = queue_sprite_at_base(depth);
}

void player_state_hurt_spare_return(void)
{
}

void player_state_bouncing(void)
{
	uint8_t sound_id = 0;
	uint32_t r0 = 0, r1 = 0, r2 = 0, r7 = 0, r11 = 0, at_last_step = 0, bounce_record = 0, bounce_records = 0, depth = 0, gravity = 0, ignored = 0, new_heading = 0, rebound_speed = 0;
	uint16_t state_timer = 0;

	push_player_out_of_wall(&r0, &r1, &r11);
	if ((OBJECT(current_object)->flags & 0x20u) != 0u)
		PLAYER(current_player)->state = 0xEu;
	switch (PLAYER(current_player)->substate)
	{
	case 0:
	case 4:
		PLAYER(current_player)->substate++;
		bounce_records = sonic_bounce_records_ADDRESS;
		if (OBJECT(current_object)->kind != 0u)
		{
			bounce_records = ray_bounce_records_ADDRESS;
			if (OBJECT(current_object)->kind != 1u)
				bounce_records = mighty_bounce_records_ADDRESS;
		}
		bounce_record = bounce_records + PLAYER(current_player)->bounce_direction * 3u * 2u;
		OBJECT(current_object)->direction = ROM16(bounce_record);
		OBJECT(current_object)->sprite_flags &= ~0x40u;
		bounce_record += 2u;
		OBJECT(current_object)->sprite_flags |= ROM16(bounce_record);
		OBJECT(current_object)->animation = ROM16(bounce_record + 2u) + bounce_records;
		face_movement_direction(r0);
		OBJECT(current_object)->speed = 0x1000000;
		OBJECT(current_object)->speed_z = -0x68000;
		PLAYER(current_player)->bounce_speed_z = -0x68000;
		PLAYER(current_player)->bounce_step = 0x34000;
		PLAYER(current_player)->bounce_speed_z += PLAYER(current_player)->bounce_step;
		set_speed_from_direction();
		OBJECT(current_object)->sprite_frame = mem_read16(OBJECT(current_object)->animation + 2u);
		OBJECT(current_object)->animation_frame = 0u;
		OBJECT(current_object)->animation_timer = 3;
		OBJECT(current_object)->animation_speed = 3u;
		PLAYER(current_player)->state_timer = 0x14;
		sound_id = 0x25u;
		play_sound(sound_id, &r1, &r2);
		add_player_to_lists();
		ignored = queue_sprite_at_floor();
		return;
	case 1:
	case 2:
		gravity = 0x8000u;
		if (OBJECT(current_object)->floor_type == 4u)
			gravity = gravity >> 2;
		OBJECT(current_object)->speed_z += gravity;
		PLAYER(current_player)->previous_z = OBJECT(current_object)->z;
		OBJECT(current_object)->z += OBJECT(current_object)->speed_z;
		PLAYER(current_player)->previous_x = OBJECT(current_object)->x;
		OBJECT(current_object)->x += OBJECT(current_object)->speed_x;
		PLAYER(current_player)->previous_y = OBJECT(current_object)->y;
		OBJECT(current_object)->y += OBJECT(current_object)->speed_y;
		find_floor(&gravity, &r1, &r2, &ignored, &r11);
		if (OBJECT(current_object)->floor_z_int <= OBJECT(current_object)->z_int)
		{
			PLAYER(current_player)->substate++;
			OBJECT(current_object)->z = OBJECT(current_object)->floor_z;
			rebound_speed = (int32_t)OBJECT(current_object)->speed_z >> 1;
			OBJECT(current_object)->speed_z = 0u - rebound_speed;
			PLAYER(current_player)->bounce_speed_z = 0u - rebound_speed;
			PLAYER(current_player)->bounce_step = (int32_t)(rebound_speed * 2u) / 3;
			PLAYER(current_player)->bounce_speed_z += PLAYER(current_player)->bounce_step;
			if ((int8_t)OBJECT(current_object)->animation_frame < 6)
			{
				OBJECT(current_object)->animation_frame++;
				OBJECT(current_object)->sprite_frame = mem_read16(OBJECT(current_object)->animation + 2u + OBJECT(current_object)->animation_frame * 2u);
			}
			update_player_blink();
			add_to_rider_list();
			project_object(&ignored, &depth);
			keep_player_on_screen(depth);
			if ((OBJECT(current_object)->flags & 0x80000u) != 0u)
				return;
			ignored = queue_sprite_at_floor();
			return;
		}
		if (PLAYER(current_player)->bounce_speed_z <= OBJECT(current_object)->speed_z)
		{
			PLAYER(current_player)->bounce_speed_z += PLAYER(current_player)->bounce_step;
			if ((int8_t)OBJECT(current_object)->animation_frame < 6)
			{
				OBJECT(current_object)->animation_frame++;
				OBJECT(current_object)->sprite_frame = mem_read16(OBJECT(current_object)->animation + 2u + OBJECT(current_object)->animation_frame * 2u);
			}
		}
		update_player_blink();
		add_to_rider_list();
		project_object(&ignored, &depth);
		keep_player_on_screen(depth);
		if ((OBJECT(current_object)->flags & 0x80000u) != 0u)
			return;
		ignored = queue_sprite_at_floor();
		return;
	case 3:
		state_timer = PLAYER(current_player)->state_timer;
		PLAYER(current_player)->state_timer = state_timer - 1u;
		if ((int16_t)state_timer <= 1)
		{
			PLAYER(current_player)->state = 0u;
			goto player_bouncing_end;
		}
		if ((int8_t)OBJECT(current_object)->animation_speed < 0)
		{
			start_jump_if_pressed();
			if (PLAYER(current_player)->state == 4u)
			{
			player_bouncing_end:
				OBJECT(current_object)->flags &= ~4u;
				PLAYER(current_player)->substate = 0u;
				OBJECT(current_object)->speed = 0;
				PLAYER(current_player)->animation_speed = 0;
				new_heading = (PLAYER(current_player)->target_heading + 0x200u) & 0x3FFu;
				PLAYER(current_player)->target_heading = new_heading;
				PLAYER(current_player)->heading = new_heading;
				PLAYER(current_player)->facing_angle = new_heading;
				set_facing(new_heading);
			}
			else
			{
				read_player_trackball();
				if ((PLAYER(current_player)->trackball_x | PLAYER(current_player)->trackball_y) != 0u)
				{
					PLAYER(current_player)->state = 1u;
					goto player_bouncing_end;
				}
			}
		}
		if ((int8_t)OBJECT(current_object)->animation_speed >= 0)
		{
			animate(&r1, &r7, &at_last_step);
			if ((uint8_t)at_last_step != 0u)
				OBJECT(current_object)->animation_speed = 0xFFu;
		}
		update_player_blink();
		update_screen_position(&ignored, &r1);
		keep_player_on_screen(r1);
		if ((OBJECT(current_object)->flags & 0x80000u) != 0u)
			return;
		r1 = queue_sprite_at_base(r1);
		return;
	default:
		unknown_code(0x5C0Fu + PLAYER(current_player)->substate * 3u);
	}
}
