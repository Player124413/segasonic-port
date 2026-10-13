/**
 *    Name:        game_07.c
 *    Description: the game's functions, part 07 (in the order of their V60 addresses)
 *    Version:     0.1.0
 *    Created:     12-10-2026
 *    Modified:    13-10-2026
 *    Author:      Sebastian Toyos
 *
 *    SegaSonic The Hedgehog (arcade, Sega System 32): the game's V60 program decompiled into C by
 *    tools/decompile.py, with the names of symbols.txt.
 */
#include "game.h"

void place_sign(uint16_t x, uint16_t y, uint32_t kind, uint16_t z, uint32_t *p_r1, uint32_t *p_sign)
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
	*p_r1 = r1;
	*p_sign = sign;
}

void init_sign_from_kind(uint32_t record, uint32_t *p_r1, uint32_t *p_sign)
{
	uint32_t sign = *p_sign;
	uint32_t r1 = 0, ignored = 0;
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
	project_object(&ignored, &r1);
	sign = current_object;
	current_object = saved_current_object;
	*p_r1 = r1;
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
	uint32_t r0 = 0, block_offset = 0, cursor = 0, ignored = 0, y = 0;

	OBJECT(current_object)->sprite_flags = 0x1004u;
	OBJECT(current_object)->sprite_palette = 0x8450u;
	OBJECT(current_object)->sprite_offset_x = 0;
	OBJECT(current_object)->sprite_offset_y = 0;
	OBJECT(current_object)->flags = 0u;
	OBJECT(current_object)->handler = floor_sign_update_ADDRESS;
	OBJECT(current_object)->sprite_frame = floor_sign_sprites[OBJECT(current_object)->sprite_frame];
	find_floor(&r0, &y, &block_offset, &ignored, &cursor);
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

void spreading_flame_start(void)
{
	uint32_t r0 = 0, block_offset = 0, cursor = 0, ignored = 0, y = 0;

	OBJECT(current_object)->sprite_flags = 0x1044u;
	OBJECT(current_object)->sprite_palette = 0x10C0u;
	OBJECT(current_object)->sprite_offset_x = 0;
	OBJECT(current_object)->sprite_offset_y = 0;
	OBJECT(current_object)->flags = 0u;
	OBJECT(current_object)->animation = flame_animation_ADDRESS;
	OBJECT(current_object)->animation_frame = 0u;
	OBJECT(current_object)->animation_speed = 2u;
	OBJECT(current_object)->animation_timer = 0;
	OBJECT(current_object)->handler = spreading_flame_burn_ADDRESS;
	find_floor(&r0, &y, &block_offset, &ignored, &cursor);
	OBJECT(current_object)->z = OBJECT(current_object)->floor_z;
	project_object(&ignored, &ignored);
}

void spreading_flame_burn(void)
{
	uint16_t chaser_gap = 0;
	uint32_t r1 = 0, r7 = 0, r8 = 0, depth = 0, fireball = 0, free_count = 0, ignored = 0, object = 0, off_screen = 0, target_dy = 0;

	if (chaser_object == 0u)
		OBJECT(current_object)->flags |= 0x400u;
	if ((OBJECT(current_object)->flags & 0x400u) == 0u)
	{
		if ((OBJECT(current_object)->flags & 0x400u) == 0u)
		{
			object = LINKED(current_object, chaser_object);
			if (OBJECT(current_object)->y_int < OBJECT(object)->y_int)
			{
				update_screen_position(&ignored, &r1);
				animate(&r1, &r7, &r8);
				depth = (uint16_t)(OBJECT(current_object)->screen_y + 0x30u);
				queue_sprite_at_depth(&ignored, &depth);
				return;
			}
			chaser_gap = (uint16_t)(OBJECT(current_object)->y_int - OBJECT(object)->y_int);
			if (stage == 0xCu && chaser_speed > 8)
				chaser_gap += 0x40u;
			flame_chaser_gap = chaser_gap;
			allocate_object(&free_count, &fireball);
			if (free_count == 0u)
			{
				update_screen_position(&ignored, &r1);
				animate(&r1, &r7, &r8);
				depth = (uint16_t)(OBJECT(current_object)->screen_y + 0x30u);
				queue_sprite_at_depth(&ignored, &depth);
				return;
			}
			current_object = fireball;
			OBJECT(current_object)->x = OBJECT(object_cursor)->x;
			OBJECT(current_object)->y = OBJECT(object_cursor)->y;
			OBJECT(current_object)->var_96 = OBJECT(current_object)->x_int;
			AS16(OBJECT(current_object)->player) = OBJECT(current_object)->y_int;
			next_random(&target_dy, &ignored, &ignored);
			target_dy = target_dy & 0x3Fu;
			if ((int16_t)target_dy < flame_chaser_gap)
				target_dy = (uint16_t)flame_chaser_gap;
			AS16(OBJECT(current_object)->player) -= target_dy;
			OBJECT(current_object)->z = OBJECT(object_cursor)->z;
			OBJECT(current_object)->var_7F = 0u;
			OBJECT(current_object)->next_handler = spreading_flame_start_ADDRESS;
			start_fireball_flipped();
			current_object = object_cursor;
			OBJECT(current_object)->flags |= 0x400u;
		}
		update_screen_position(&ignored, &r1);
		animate(&r1, &r7, &r8);
		depth = (uint16_t)(OBJECT(current_object)->screen_y + 0x30u);
		queue_sprite_at_depth(&ignored, &depth);
		return;
	}
	off_screen = is_off_screen();
	if (off_screen == 0u)
	{
		update_screen_position(&ignored, &r1);
		animate(&r1, &r7, &off_screen);
		depth = (uint16_t)(OBJECT(current_object)->screen_y + 0x30u);
		queue_sprite_at_depth(&ignored, &depth);
		return;
	}
	delete_current_object();
}

uint32_t spawn_stage_03_end_pillar(uint32_t pillar)
{
	uint32_t free_count = 0;

	allocate_object_outside_loop(&free_count, &pillar);
	if (free_count != 0u)
	{
		OBJECT(pillar)->sprite_flags = 0x1004u;
		OBJECT(pillar)->sprite_frame = 0x6600u;
		OBJECT(pillar)->sprite_palette = 0x8740u;
		OBJECT(pillar)->sprite_offset_x = 0;
		OBJECT(pillar)->sprite_offset_y = 0;
		OBJECT(pillar)->flags = 0u;
		OBJECT(pillar)->handler = stage_03_end_pillar_show_ADDRESS;
		OBJECT(pillar)->screen_x = 0x146;
		OBJECT(pillar)->screen_y = 0x75;
	}
	return pillar;
}

void stage_03_end_pillar_show(void)
{
	uint32_t depth = 0, ignored = 0;

	if (map_screen_active == 0u)
	{
		depth = (uint16_t)(OBJECT(current_object)->screen_y + 0x20u);
		queue_sprite_at_depth(&ignored, &depth);
		return;
	}
	delete_current_object();
}

void exit_gate_start(uint32_t sign)
{
	uint16_t kind = 0, x = 0, y_2 = 0, z = 0;
	uint32_t r0 = 0, block_offset = 0, ignored = 0, y = 0;

	OBJECT(current_object)->sprite_flags = 0x1005u;
	OBJECT(current_object)->sprite_frame = 0x6520u;
	OBJECT(current_object)->sprite_palette = 0x86A0u;
	OBJECT(current_object)->flags = 0u;
	OBJECT(current_object)->sprite_offset_x = 0;
	OBJECT(current_object)->sprite_offset_y = 0;
	OBJECT(current_object)->z = 0;
	OBJECT(current_object)->speed_z = 0;
	OBJECT(current_object)->handler = exit_gate_update_ADDRESS;
	sign_stop_flag = 0u;
	find_floor(&r0, &y, &block_offset, &ignored, &sign);
	OBJECT(current_object)->z = OBJECT(current_object)->floor_z;
	OBJECT(current_object)->z_int -= 0xE0u;
	project_object(&ignored, &ignored);
	x = 0x942u;
	y_2 = 0x10E0u;
	kind = 8u;
	z = (uint16_t)(OBJECT(current_object)->floor_z_int - 0x50u);
	spawn_sign_2(x, y_2, kind, z, sign);
}

void exit_gate_update(void)
{
	uint8_t sound_id = 0;
	uint32_t r1 = 0, r2 = 0, ignored = 0;

	if (map_screen_active != 0u)
	{
		delete_current_object();
		return;
	}
	if ((uint32_t)OBJECT(current_object)->z != (uint32_t)OBJECT(current_object)->floor_z && AS32(players_finished[0]) == 0xFFFFFFu)
	{
		OBJECT(current_object)->speed_z += 0x6000u;
		OBJECT(current_object)->z += OBJECT(current_object)->speed_z;
		if (OBJECT(current_object)->floor_z_int <= OBJECT(current_object)->z_int)
		{
			OBJECT(current_object)->z = OBJECT(current_object)->floor_z;
			sign_stop_flag = 0xFFu;
			sound_id = 0x20u;
			play_sound(sound_id, &r1, &r2);
		}
	}
	update_screen_position(&ignored, &r1);
	r1 = queue_sprite(r1);
}

void ram_target_start(void)
{
	uint32_t r0 = 0, block_offset = 0, cursor = 0, ignored = 0, y = 0;

	OBJECT(current_object)->sprite_flags = 0x1045u;
	OBJECT(current_object)->sprite_frame = 0x6A58u;
	OBJECT(current_object)->sprite_palette = 0x91A0u;
	OBJECT(current_object)->flags = 2u;
	OBJECT(current_object)->handler = ram_target_update_ADDRESS;
	OBJECT(current_object)->sprite_offset_x = 6;
	OBJECT(current_object)->sprite_offset_y = 0x16;
	OBJECT(current_object)->hitbox_offset_x = -0x30;
	OBJECT(current_object)->hitbox_offset_y = -0x20;
	OBJECT(current_object)->hitbox_size_x = 0x30u;
	OBJECT(current_object)->hitbox_size_y = 0x10u;
	OBJECT(current_object)->hitbox_height = 0x28u;
	OBJECT(current_object)->kind = 0x21u;
	OBJECT(current_object)->direction = 0x300u;
	OBJECT(current_object)->ram_target.appear_state = 0;
	OBJECT(current_object)->ram_target.hit_count = 0;
	OBJECT(current_object)->animation = ram_target_animation_ADDRESS;
	OBJECT(current_object)->animation_frame = 0u;
	OBJECT(current_object)->animation_speed = 4u;
	OBJECT(current_object)->animation_timer = 0;
	OBJECT(current_object)->ram_target.damage_script = ram_target_damage_script_ADDRESS;
	find_floor(&r0, &y, &block_offset, &ignored, &cursor);
	OBJECT(current_object)->z = OBJECT(current_object)->floor_z;
	project_object(&ignored, &ignored);
}

void ram_target_update(void)
{
	uint16_t margin = 0;
	uint32_t off_screen = 0;

	OBJECT(current_object)->health = 0x7F;
	margin = 0x40u;
	off_screen = check_left_screen_margin(margin);
	if (off_screen != 0u)
	{
		delete_current_object();
		return;
	}
	if ((uint8_t)OBJECT(current_object)->ram_target.appear_state == 0u && OBJECT(current_object)->screen_x >= 0x50)
		OBJECT(current_object)->ram_target.appear_state = -1;
	ram_target_draw();
}

void ram_target_check_hits(void)
{
	uint32_t animation = 0, script_record = 0;

	if (roller_ram_hit == 0u)
	{
		ram_target_draw();
		return;
	}
	OBJECT(current_object)->animation_frame = 0u;
	OBJECT(current_object)->ram_target.hit_count++;
	if (OBJECT(current_object)->ram_target.hit_count < (int8_t)mem_read8(OBJECT(current_object)->ram_target.damage_script))
	{
		ram_target_draw();
		return;
	}
	script_record = OBJECT(current_object)->ram_target.damage_script + 2u;
	animation = mem_read16(script_record) + ram_target_damage_script_ADDRESS;
	OBJECT(current_object)->animation = animation;
	OBJECT(current_object)->ram_target.damage_script = script_record + 2u;
	if (OBJECT(current_object)->ram_target.hit_count < 4)
	{
		ram_target_draw();
		return;
	}
	ram_target_burst(animation);
	ram_target_draw();
}

void ram_target_draw(void)
{
	uint32_t r1 = 0, r7 = 0, at_last_step = 0, depth = 0, ignored = 0;

	if ((uint8_t)OBJECT(current_object)->ram_target.appear_state != 0u && (int8_t)OBJECT(current_object)->animation_frame >= 0)
	{
		animate(&r1, &r7, &at_last_step);
		if ((uint8_t)at_last_step != 0u)
		{
			OBJECT(current_object)->ram_target.appear_state = 1;
			OBJECT(current_object)->animation_frame = 0xFFu;
		}
	}
	if ((uint8_t)OBJECT(current_object)->ram_target.appear_state != 0u && OBJECT(current_object)->ram_target.appear_state >= 0)
		add_to_zone_list(0u);
	update_screen_position(&ignored, &r1);
	depth = (uint16_t)(OBJECT(current_object)->screen_y - OBJECT(current_object)->sprite_offset_y + 0x10u);
	if ((uint8_t)OBJECT(current_object)->ram_target.appear_state == 0u)
		depth = (uint16_t)(depth - 0x30u);
	queue_sprite_at_depth(&ignored, &depth);
}

void ram_target_burst(uint32_t piece)
{
	uint16_t screen_x = 0;
	uint32_t free_count = 0, piece_record = 0, pieces_left = 0;

	screen_x = (uint16_t)OBJECT(current_object)->screen_x;
	r14 = (uint16_t)OBJECT(current_object)->screen_y;
	pieces_left = ram_target_debris_count;
	piece_record = ram_target_debris_ADDRESS;
	do
	{
		allocate_object_outside_loop(&free_count, &piece);
		if (free_count == 0u)
			break;
		OBJECT(piece)->sprite_flags = ROM16(piece_record);
		OBJECT(piece)->screen_x = screen_x;
		piece_record += 2u;
		OBJECT(piece)->screen_x += ROM16(piece_record);
		OBJECT(piece)->screen_y = r14;
		piece_record += 2u;
		OBJECT(piece)->screen_y += ROM16(piece_record);
		piece_record += 2u;
		OBJECT(piece)->speed_x_fraction = ROM16(piece_record);
		piece_record += 2u;
		OBJECT(piece)->speed_y_fraction = ROM16(piece_record);
		piece_record += 2u;
		OBJECT(piece)->animation = ram_target_debris_animations_ADDRESS + ROM16(piece_record);
		OBJECT(piece)->sprite_palette = 0x8240u;
		OBJECT(piece)->projected_x = (uint32_t)OBJECT(piece)->screen_x << 4;
		OBJECT(piece)->projected_y_fraction = (uint32_t)OBJECT(piece)->screen_y << 4;
		OBJECT(piece)->animation_frame = 0u;
		OBJECT(piece)->animation_speed = 4u;
		OBJECT(piece)->animation_timer = 0;
		OBJECT(piece)->flags = 0u;
		OBJECT(piece)->sprite_scale = 0x100u;
		OBJECT(piece)->handler = rock_debris_fly_ADDRESS;
		piece_record += 2u;
		pieces_left--;
	} while (pieces_left != 0u);
}

void ram_target_part_start(void)
{
	uint32_t r0 = 0, block_offset = 0, cursor = 0, ignored = 0, part_list = 0, y = 0;

	OBJECT(current_object)->ram_target.variant = OBJECT(current_object)->sprite_frame;
	part_list = ram_target_part_list_2_ADDRESS;
	if (OBJECT(current_object)->sprite_frame != 0u)
		part_list = ram_target_part_list_3_ADDRESS;
	OBJECT(current_object)->part_list = part_list;
	OBJECT(current_object)->sprite_flags = 0xD006u;
	OBJECT(current_object)->flags = 0u;
	OBJECT(current_object)->sprite_offset_x = 2;
	OBJECT(current_object)->sprite_offset_y = 1;
	OBJECT(current_object)->ram_target.hit_count = 0;
	OBJECT(current_object)->handler = ram_target_part_update_ADDRESS;
	find_floor(&r0, &y, &block_offset, &ignored, &cursor);
	OBJECT(current_object)->z = OBJECT(current_object)->floor_z;
	project_object(&ignored, &ignored);
}

void ram_target_part_update(void)
{
	uint32_t depth = 0, ignored = 0, margin = 0, off_screen = 0;

	if (OBJECT(LINKED(current_object, chaser_object))->y_int <= (int16_t)(OBJECT(current_object)->y_int + 0x60u) && roller_ram_hit != 0u)
	{
		OBJECT(current_object)->ram_target.hit_count++;
		if (OBJECT(current_object)->ram_target.hit_count >= 4)
		{
			if (OBJECT(current_object)->ram_target.variant != 0u)
			{
				delete_current_object();
				return;
			}
			OBJECT(current_object)->sprite_flags = 0x1049u;
			OBJECT(current_object)->sprite_frame = 0x6A88u;
			OBJECT(current_object)->sprite_palette = 0x11B0u;
			OBJECT(current_object)->sprite_offset_x = 2;
			OBJECT(current_object)->sprite_offset_y = -0x19;
		}
	}
	margin = 0x88u;
	off_screen = check_left_screen_margin((uint16_t)margin);
	if (off_screen == 0u)
	{
		update_screen_position(&ignored, &margin);
		depth = (uint16_t)(OBJECT(current_object)->screen_y - 0x28u);
		queue_sprite_at_depth(&ignored, &depth);
		return;
	}
	delete_current_object();
}

void eggman_speech_start(void)
{
	OBJECT(current_object)->eggman_speech.frame_count = 0;
	OBJECT(current_object)->eggman_speech.script = eggman_speech_scripts[OBJECT(current_object)->sprite_frame] + eggman_speech_scripts_ADDRESS;
	OBJECT(current_object)->handler = eggman_speech_run_ADDRESS;
	eggman_speech_run();
}

void eggman_speech_run(void)
{
	uint8_t sound_id = 0;
	uint16_t frames = 0, kind = 0, screen_x = 0, screen_y = 0;
	uint32_t r1 = 0, r2 = 0, r11 = 0, command = 0, script_cursor = 0;
	uint16_t delay = 0;

	if (current_game_mode == 0x11u)
		return;
	OBJECT(current_object)->eggman_speech.frame_count++;
	script_cursor = OBJECT(current_object)->eggman_speech.script;
	delay = mem_read16(script_cursor);
	script_cursor += 2u;
	if (OBJECT(current_object)->eggman_speech.frame_count < (int16_t)delay)
		return;
	command = mem_read16(script_cursor) * 2u;
	script_cursor += 2u;
	switch (command)
	{
	case 0x0u:
	case 0x8u:
		delete_current_object();
		return;
	case 0x2u:
		OBJECT(current_object)->eggman_speech.script = script_cursor;
		OBJECT(current_object)->eggman_speech.frame_count = 0;
		spawn_eggman_booth(command, 0u, 0u, &r2, &r11);
		return;
	case 0x4u:
		break;
	case 0x6u:
		sound_id = (uint8_t)mem_read16(script_cursor);
		OBJECT(current_object)->eggman_speech.script = script_cursor + 2u;
		OBJECT(current_object)->eggman_speech.frame_count = 0;
		play_sound(sound_id, &r1, &r2);
		return;
	case 0xEu:
		return;
	default:
		unknown_code(0x85794u + command);
	}
	screen_x = mem_read16(script_cursor);
	script_cursor += 2u;
	screen_y = mem_read16(script_cursor);
	script_cursor += 2u;
	kind = mem_read16(script_cursor);
	script_cursor += 2u;
	frames = mem_read16(script_cursor);
	OBJECT(current_object)->eggman_speech.script = script_cursor + 2u;
	OBJECT(current_object)->eggman_speech.frame_count = 0;
	r11 = spawn_message_box(screen_x, screen_y, kind, frames, 0u);
}

void scenery_sprite_start(void)
{
	uint32_t block_offset = 0, cursor = 0, ignored = 0, kind_record = 0, sprite_kind = 0, y = 0;

	sprite_kind = OBJECT(current_object)->sprite_frame;
	scenery_sprite_kind = sprite_kind;
	kind_record = scenery_sprite_kinds[sprite_kind] + scenery_sprite_kinds_ADDRESS;
	OBJECT(current_object)->sprite_flags = ROM16(kind_record);
	kind_record += 2u;
	OBJECT(current_object)->sprite_frame = ROM16(kind_record);
	kind_record += 2u;
	OBJECT(current_object)->sprite_palette = ROM16(kind_record);
	kind_record += 2u;
	OBJECT(current_object)->scenery_sprite.depth_offset = ROM16(kind_record);
	OBJECT(current_object)->handler = ROM32(kind_record + 2u);
	OBJECT(current_object)->sprite_offset_x = 0;
	OBJECT(current_object)->sprite_offset_y = 0;
	OBJECT(current_object)->flags = 0u;
	OBJECT(current_object)->scenery_sprite.unused_stage = stage;
	if (scenery_sprite_kind != 0xDu)
	{
		find_floor(&sprite_kind, &y, &block_offset, &ignored, &cursor);
		OBJECT(current_object)->z = OBJECT(current_object)->floor_z;
	}
	else
		OBJECT(current_object)->z = 0x8D0000;
	project_object(&ignored, &ignored);
}

void scenery_sprite_draw_until_you_failed(void)
{
	if (current_game_mode != 0x20u || game_mode_timer < 0x3C)
	{
		scenery_sprite_draw();
		return;
	}
	delete_current_object();
}

void scenery_sprite_draw(void)
{
	uint32_t r1 = 0, depth = 0, ignored = 0;

	if (map_screen_active == 0u)
	{
		update_screen_position(&ignored, &r1);
		depth = (uint16_t)(OBJECT(current_object)->screen_y + OBJECT(current_object)->scenery_sprite.depth_offset);
		queue_sprite_at_depth(&ignored, &depth);
		return;
	}
	delete_current_object();
}

void scenery_sprite_draw_until_offscreen(void)
{
	uint32_t r1 = 0, depth = 0, ignored = 0, off_screen = 0;

	off_screen = check_left_screen();
	if (off_screen == 0u)
	{
		update_screen_position(&ignored, &r1);
		depth = (uint16_t)(OBJECT(current_object)->screen_y + OBJECT(current_object)->scenery_sprite.depth_offset);
		queue_sprite_at_depth(&ignored, &depth);
		return;
	}
	delete_current_object();
}

void scenery_sprite_draw_wide_margin(void)
{
	uint32_t depth = 0, ignored = 0, margin = 0, off_screen = 0;

	margin = 0x50u;
	off_screen = check_left_screen_margin((uint16_t)margin);
	if (off_screen == 0u)
	{
		update_screen_position(&ignored, &margin);
		depth = (uint16_t)(OBJECT(current_object)->screen_y + OBJECT(current_object)->scenery_sprite.depth_offset);
		queue_sprite_at_depth(&ignored, &depth);
		return;
	}
	delete_current_object();
}

void scenery_sprite_cycle_palette_start(void)
{
	OBJECT(current_object)->scenery_sprite.palette_index = 0u;
	OBJECT(current_object)->scenery_sprite.palette_delay = 6u;
	OBJECT(current_object)->scenery_sprite.palette_timer = 0u;
	OBJECT(current_object)->scenery_sprite.palette_list = scenery_sprite_palettes_ADDRESS;
	OBJECT(current_object)->sprite_offset_x = 0;
	OBJECT(current_object)->sprite_offset_y = -0x10;
	OBJECT(current_object)->handler = scenery_sprite_cycle_palette_ADDRESS;
}

void scenery_sprite_cycle_palette(void)
{
	uint32_t r1 = 0, depth = 0, ignored = 0;

	if (map_screen_active == 0u)
	{
		animate_sign();
		update_screen_position(&ignored, &r1);
		depth = (uint16_t)(OBJECT(current_object)->screen_y + OBJECT(current_object)->scenery_sprite.depth_offset);
		queue_sprite_at_depth(&ignored, &depth);
		return;
	}
	delete_current_object();
}

void fire_vent_spawner_start(void)
{
	fire_vent_cursor = fire_vent_list_ADDRESS;
	OBJECT(current_object)->handler = fire_vent_spawner_update_ADDRESS;
	fire_vent_spawner_update();
}

void fire_vent_spawner_update(void)
{
	uint32_t free_count = 0, object = 0, vent_record = 0;
	uint16_t spawn_position = 0;

	vent_record = fire_vent_cursor;
	spawn_position = ROM16(vent_record);
	vent_record += 2u;
	if (scroll_position < spawn_position)
		return;
	if ((int16_t)ROM16(vent_record + 6u) < 0)
	{
		delete_current_object();
		return;
	}
	allocate_object(&free_count, &object);
	if (free_count != 0u)
	{
		OBJECT(object)->x_int = ROM16(vent_record);
		OBJECT(object)->x_fraction = 0u;
		vent_record += 2u;
		OBJECT(object)->y_int = ROM16(vent_record);
		OBJECT(object)->y_fraction = 0u;
		vent_record += 2u;
		OBJECT(object)->z_int = ROM16(vent_record);
		OBJECT(object)->z_fraction = 0u;
		OBJECT(object)->fire_vent.blast_delay = ROM16(vent_record + 2u);
		OBJECT(object)->sprite_flags = 0x1006u;
		OBJECT(object)->sprite_frame = 0x6A48u;
		OBJECT(object)->sprite_palette = 0x9110u;
		OBJECT(object)->flags = 2u;
		OBJECT(object)->handler = fire_vent_start_ADDRESS;
		OBJECT(object)->fire_vent.burst_state = 0u;
		OBJECT(object)->fire_vent.palette_list = fire_vent_glow_palettes_ADDRESS;
		OBJECT(object)->fire_vent.palette_index = 0u;
		OBJECT(object)->fire_vent.palette_delay = 8u;
		OBJECT(object)->fire_vent.palette_timer = 0u;
		OBJECT(object)->sprite_offset_x = 0;
		OBJECT(object)->sprite_offset_y = 0x10;
		OBJECT(object)->kind = 0x22u;
		OBJECT(object)->hitbox_size_x = 0x90u;
		OBJECT(object)->hitbox_size_y = 0x48u;
		OBJECT(object)->hitbox_height = 0x28u;
		OBJECT(object)->hitbox_offset_x = -0x48;
		OBJECT(object)->hitbox_offset_y = -8;
		fire_vent_cursor += 0xAu;
	}
}

void fire_vent_start(void)
{
	uint32_t ignored = 0;

	project_object(&ignored, &ignored);
	OBJECT(current_object)->handler = fire_vent_update_ADDRESS;
	fire_vent_update();
}

void fire_vent_update(void)
{
	uint32_t zero = 0, negative = 0, overflow = 0;
	uint8_t sound_id = 0;
	uint32_t r2 = 0, depth = 0, free_count = 0, ignored = 0, margin = 0, object = 0, off_screen = 0;
	uint16_t blast_delay = 0, next_delay = 0;

	margin = 0x40u;
	off_screen = check_left_screen_margin((uint16_t)margin);
	if (off_screen != 0u)
	{
		delete_current_object();
		return;
	}
	if ((int8_t)OBJECT(current_object)->fire_vent.palette_index >= 0)
	{
		if (OBJECT(current_object)->fire_vent.blast_delay <= 0x1E)
			OBJECT(current_object)->fire_vent.palette_delay = 2u;
		animate_sign();
	}
	if ((uint16_t)OBJECT(current_object)->fire_vent.blast_delay == 0u)
	{
	fire_vent_blast:
		OBJECT(current_object)->fire_vent.blast_delay = -1;
		OBJECT(current_object)->fire_vent.burst_state = 1u;
		OBJECT(current_object)->sprite_frame = 0x6A50u;
		OBJECT(current_object)->sprite_palette = 0x9120u;
		OBJECT(current_object)->fire_vent.palette_index = 0xFFu;
		allocate_object(&free_count, &object);
		if (free_count != 0u)
		{
			OBJECT(object)->sprite_flags = 0x1009u;
			OBJECT(object)->sprite_palette = 0x90B0u;
			OBJECT(object)->sprite_offset_x = 0;
			OBJECT(object)->sprite_offset_y = 0;
			OBJECT(object)->flags = 0u;
			OBJECT(object)->handler = fire_vent_burst_animate_ADDRESS;
			OBJECT(object)->parent = current_object;
			OBJECT(object)->animation = fire_vent_blast_animation_ADDRESS;
			OBJECT(object)->animation_frame = 0u;
			OBJECT(object)->animation_speed = 3u;
			OBJECT(object)->animation_timer = 0;
			OBJECT(object)->projected_x = OBJECT(current_object)->projected_x;
			OBJECT(object)->projected_x += 0x3Au;
			OBJECT(object)->anchor_x = OBJECT(object)->projected_x;
			OBJECT(object)->projected_y = OBJECT(current_object)->projected_y;
			OBJECT(object)->projected_y_int -= 0x31u;
			OBJECT(object)->anchor_y = OBJECT(object)->projected_y_int;
			OBJECT(object)->z = OBJECT(current_object)->z;
			sound_id = 0xAu;
			play_sound(sound_id, &margin, &r2);
		}
	}
	else if (OBJECT(current_object)->screen_y >= 0x18)
	{
		blast_delay = OBJECT(current_object)->fire_vent.blast_delay;
		next_delay = blast_delay - 1u;
		overflow = blast_delay == 0x8000u;
		zero = next_delay == 0u;
		negative = next_delay >> 15;
		OBJECT(current_object)->fire_vent.blast_delay = next_delay;
		if (blast_delay == 1u)
			goto fire_vent_blast;
		if ((negative ^ overflow) | zero)
			OBJECT(current_object)->fire_vent.blast_delay = -1;
	}
	update_screen_position(&ignored, &margin);
	if (OBJECT(current_object)->fire_vent.burst_state == 0u)
	{
		depth = (uint16_t)(OBJECT(current_object)->screen_y - 0x48u);
		queue_sprite_at_depth(&ignored, &depth);
		return;
	}
	if ((int8_t)OBJECT(current_object)->fire_vent.burst_state < 0)
	{
		OBJECT(current_object)->fire_vent.burst_state = 1u;
		OBJECT(current_object)->kind = 0xCu;
	}
	add_to_touchable_list(ignored);
	depth = (uint16_t)(OBJECT(current_object)->screen_y - 0x48u);
	queue_sprite_at_depth(&ignored, &depth);
}

void fire_vent_burst_animate(void)
{
	uint32_t r1 = 0, r7 = 0, at_last_step = 0, depth = 0, ignored = 0;

	animate(&r1, &r7, &at_last_step);
	if ((uint8_t)OBJECT(current_object)->animation_timer == OBJECT(current_object)->animation_speed)
		OBJECT(current_object)->sprite_palette = fire_vent_blast_palettes[OBJECT(current_object)->animation_frame];
	if ((uint8_t)at_last_step == 0u)
	{
		update_screen_position(&ignored, &r1);
		depth = (uint16_t)(OBJECT(current_object)->screen_y + 0x68u);
		queue_sprite_at_depth(&ignored, &depth);
		return;
	}
	AS8(OBJECT(LINKED(current_object, OBJECT(current_object)->parent))->var_88) = 0xFFu;
	delete_current_object();
}

void group_leader_hold_start(void)
{
	uint32_t held_flag = 0, hold_record = 0;

	hold_record = group_leader_hold_records[OBJECT(current_object)->sprite_frame] + group_leader_hold_records_ADDRESS;
	OBJECT(current_object)->group_leader_hold.leader = ROM8(hold_record);
	hold_record++;
	OBJECT(current_object)->direction = ROM8(hold_record);
	hold_record++;
	OBJECT(current_object)->group_leader_hold.coordinate = ROM16(hold_record);
	hold_record += 2u;
	OBJECT(current_object)->group_leader_hold.end_position = ROM16(hold_record);
	OBJECT(current_object)->group_leader_hold.which_flag = ROM16(hold_record + 2u);
	held_flag = group_leader_0_held_ADDRESS;
	if (AS8(OBJECT(current_object)->group_leader_hold.which_flag) != 0u)
		held_flag = group_leaders_1_3_held_ADDRESS;
	mem_write8(held_flag, 0xFFu);
	OBJECT(current_object)->handler = group_leader_hold_wait_ADDRESS;
}

void group_leader_hold_wait(void)
{
	uint32_t held_flag = 0, object = 0;

	if (scroll_position < OBJECT(current_object)->group_leader_hold.end_position)
	{
		object = (current_object & ~0xFFFFu) | group_leaders[OBJECT(current_object)->group_leader_hold.leader];
		switch (OBJECT(current_object)->direction)
		{
		case 0:
		case 4:
			if (OBJECT(current_object)->group_leader_hold.coordinate < OBJECT(object)->y_int)
				return;
			break;
		case 1:
			if (OBJECT(object)->x_int < OBJECT(current_object)->group_leader_hold.coordinate)
				return;
			break;
		case 2:
			if (OBJECT(object)->y_int < OBJECT(current_object)->group_leader_hold.coordinate)
				return;
			break;
		case 3:
			if (OBJECT(current_object)->group_leader_hold.coordinate < OBJECT(object)->x_int)
				return;
			break;
		default:
			unknown_code(0x85BE6u + OBJECT(current_object)->direction * 2u);
		}
	}
	held_flag = group_leader_0_held_ADDRESS;
	if (OBJECT(current_object)->group_leader_hold.which_flag != 0u)
		held_flag = group_leaders_1_3_held_ADDRESS;
	mem_write8(held_flag, 0u);
	delete_current_object();
}

void collapsing_bridge_plank_update(void)
{
	uint8_t sound_id = 0, start_frame = 0;
	uint16_t x = 0, y = 0;
	uint32_t r1 = 0, r2 = 0, angle_whole = 0, depth = 0, ignored = 0, kind = 0, object = 0, plank_index = 0, sag = 0, sag_table = 0, sign = 0, swing_dx = 0, swing_dz = 0, z = 0;

	switch (OBJECT(current_object)->collapsing_bridge_plank.state)
	{
	case 0:
	case 3:
		OBJECT(current_object)->collapsing_bridge_plank.state++;
		OBJECT(current_object)->collapsing_bridge_plank.angle = 0x3000000u;
		OBJECT(current_object)->sprite_offset_x = -4;
		OBJECT(current_object)->sprite_offset_y = -4;
		OBJECT(current_object)->collapsing_bridge_plank.angle_speed = 0;
		OBJECT(current_object)->collapsing_bridge_plank.swing_count = 0u;
		OBJECT(current_object)->collapsing_bridge_plank.plank_index--;
		OBJECT(current_object)->collapsing_bridge_plank.roll_timer = 0u;
		OBJECT(current_object)->collapsing_bridge_plank.roll_frames = OBJECT(current_object)->collapsing_bridge_plank.plank_index * 2u;
		OBJECT(current_object)->animation = collapsing_bridge_plank_animation_ADDRESS;
		start_frame = OBJECT(current_object)->collapsing_bridge_plank.plank_index & 7u;
		OBJECT(current_object)->animation_frame = start_frame;
		OBJECT(current_object)->animation_speed = 3u;
		OBJECT(current_object)->animation_timer = start_frame & 3u;
		if (OBJECT(current_object)->collapsing_bridge_plank.plank_index == 0u)
		{
			bridge_swing_angle = 0;
			bridge_swing_speed = 0;
		}
		project_object(&ignored, &ignored);
		return;
	case 1:
		OBJECT(current_object)->previous_floor_z = OBJECT(current_object)->floor_z;
		plank_index = OBJECT(current_object)->collapsing_bridge_plank.plank_index;
		sag_table = bridge_0_sag_ADDRESS;
		if (OBJECT(current_object)->collapsing_bridge_plank.bridge != 0u)
			sag_table = bridge_1_sag_ADDRESS;
		sag = mem_read16(sag_table + plank_index * 2u);
		if ((sag & ~0xFFu) == 0u)
			sag = 0u;
		OBJECT(current_object)->previous_floor_z += sag << 8;
		OBJECT(current_object)->speed_x += (int32_t)((uint32_t)OBJECT(current_object)->previous_floor_z - OBJECT(current_object)->speed_x) >> 2;
		OBJECT(current_object)->z_int = OBJECT(current_object)->speed_x_int;
		update_screen_position(&ignored, &sag);
		if (event_1c_riding != 0u)
		{
			OBJECT(current_object)->collapsing_bridge_plank.state++;
			if (OBJECT(current_object)->collapsing_bridge_plank.plank_index == 0u)
			{
				sound_id = 9u;
				play_sound(sound_id, &sag, &r2);
			}
		}
		if ((uint16_t)(OBJECT(current_object)->screen_x + 0x50u) >= 0x1B0u)
			return;
		add_to_platform_list();
		depth = (uint16_t)(((int32_t)((uint32_t)OBJECT(current_object)->projected_y + OBJECT(current_object)->floor_z) >> 16) - view_y + OBJECT(current_object)->collapsing_bridge_plank.depth_offset);
		OBJECT(current_object)->collapsing_bridge_plank.draw_depth = depth;
		queue_sprite_at_depth(&ignored, &depth);
		return;
	case 2:
		if (scroll_position >= 0x9000u)
		{
			delete_current_object();
			return;
		}
		if (OBJECT(current_object)->collapsing_bridge_plank.plank_index != 0u)
		{
			object = (current_object & ~0xFFFFu) | OBJECT(current_object)->collapsing_bridge_plank.previous_plank;
			OBJECT(current_object)->collapsing_bridge_plank.angle = AS32(OBJECT(object)->var_80);
			OBJECT(current_object)->collapsing_bridge_plank.angle += bridge_swing_angle;
			OBJECT(current_object)->collapsing_bridge_plank.angle_whole &= 0x3FFu;
			if (OBJECT(current_object)->collapsing_bridge_plank.angle_whole < 0x200)
				OBJECT(current_object)->collapsing_bridge_plank.angle = 0x2000000u;
			angle_whole = (uint16_t)OBJECT(current_object)->collapsing_bridge_plank.angle_whole;
			swing_dx = sine_table[angle_whole] * 0x40u;
			OBJECT(current_object)->x = OBJECT(object)->x;
			OBJECT(current_object)->x += swing_dx;
			swing_dz = cosine_table[angle_whole] * 0x40u;
			OBJECT(current_object)->z = OBJECT(object)->z;
			OBJECT(current_object)->z -= swing_dz;
			OBJECT(current_object)->flags &= 0xFFFFFFFEu;
			AS16(OBJECT(current_object)->flags) |= AS16(OBJECT(object)->flags) & 1u;
			OBJECT(current_object)->collapsing_bridge_plank.swing_count = AS8(OBJECT(object)->var_8A);
		}
		else if ((OBJECT(current_object)->flags & 1u) == 0u)
		{
			if (OBJECT(current_object)->collapsing_bridge_plank.swing_count != 0u)
			{
				bridge_swing_speed -= 0x200u;
				bridge_swing_angle += bridge_swing_speed;
				if ((int16_t)AT16(bridge_swing_speed, 2) < 0 && bridge_swing_angle <= 0)
				{
					OBJECT(current_object)->collapsing_bridge_plank.swing_count++;
					if ((int8_t)OBJECT(current_object)->collapsing_bridge_plank.swing_count >= 6)
					{
						OBJECT(current_object)->flags |= 1u;
						event_1c_ride_phase = 0u;
						x = 0xBA0u;
						y = 0xEA4u;
						kind = 0x16u;
						z = 0x660u;
						spawn_sign(x, y, (uint16_t)kind, (uint16_t)z, &swing_dz, &sign);
					}
					bridge_swing_speed = 0u - bridge_swing_speed;
					bridge_swing_speed = (int32_t)(bridge_swing_speed * 0x5Eu) >> 8;
					sound_id = 0x20u;
					play_sound(sound_id, &swing_dz, &r2);
				}
			}
			else
			{
				OBJECT(current_object)->collapsing_bridge_plank.angle_speed -= 0x3000u;
				OBJECT(current_object)->collapsing_bridge_plank.angle += OBJECT(current_object)->collapsing_bridge_plank.angle_speed;
				bridge_head_angle = OBJECT(current_object)->collapsing_bridge_plank.angle_whole;
				if (OBJECT(current_object)->collapsing_bridge_plank.angle_whole <= 0x200)
				{
					OBJECT(current_object)->collapsing_bridge_plank.angle = 0x2000000u;
					OBJECT(current_object)->collapsing_bridge_plank.angle_speed = 0;
					OBJECT(current_object)->collapsing_bridge_plank.swing_count++;
					bridge_swing_speed = -0x10000;
				}
				bridge_swing_angle += 0x2000u;
			}
		}
		if ((int8_t)OBJECT(current_object)->collapsing_bridge_plank.swing_count < 3)
			animate(&swing_dz, &kind, &z);
		else
		{
			if ((int16_t)OBJECT(current_object)->collapsing_bridge_plank.roll_timer < (int16_t)OBJECT(current_object)->collapsing_bridge_plank.roll_frames)
			{
				OBJECT(current_object)->collapsing_bridge_plank.roll_timer++;
				animate(&swing_dz, &kind, &z);
				project_object(&ignored, &r1);
				if (OBJECT(current_object)->screen_y > 0xE0)
					return;
				if ((int8_t)OBJECT(current_object)->collapsing_bridge_plank.swing_count < 1)
				{
					r1 = queue_sprite(r1);
					return;
				}
				depth = 0u;
				queue_sprite_at_depth(&ignored, &depth);
				return;
			}
			OBJECT(current_object)->sprite_frame = mem_read16(OBJECT(current_object)->animation + 2u);
		}
		project_object(&ignored, &r1);
		if (OBJECT(current_object)->screen_y > 0xE0)
			return;
		if ((int8_t)OBJECT(current_object)->collapsing_bridge_plank.swing_count < 1)
		{
			r1 = queue_sprite(r1);
			return;
		}
		depth = 0u;
		queue_sprite_at_depth(&ignored, &depth);
		return;
	default:
		unknown_code(0x85F8Eu + OBJECT(current_object)->collapsing_bridge_plank.state * 3u);
	}
}

void burning_boulder_start(void)
{
	uint32_t r0 = 0, block_offset = 0, boulder_record = 0, ignored = 0, object = 0, object_2 = 0, zone_index = 0;
	uint8_t flames_left_before = 0;

	boulder_record = burning_boulder_records_ADDRESS + OBJECT(current_object)->sprite_frame * 2u * 4u;
	OBJECT(current_object)->x_int = ROM16(boulder_record);
	OBJECT(current_object)->x_fraction = 0u;
	boulder_record += 2u;
	OBJECT(current_object)->burning_boulder.start_delay = ROM16(boulder_record);
	OBJECT(current_object)->burning_boulder.path = ROM32(boulder_record + 2u);
	OBJECT(current_object)->y = 0x3E00000;
	OBJECT(current_object)->y_int -= OBJECT(current_object)->sprite_frame * 0x60u;
	OBJECT(current_object)->sprite_flags = 0x1004u;
	OBJECT(current_object)->sprite_frame = 0xAA58u;
	OBJECT(current_object)->sprite_palette = 0x51B0u;
	OBJECT(current_object)->kind = 0x14u;
	OBJECT(current_object)->flags = 2u;
	OBJECT(current_object)->hitbox_size_x = 0x90u;
	OBJECT(current_object)->hitbox_size_y = 0x90u;
	OBJECT(current_object)->burning_boulder.state = 0u;
	OBJECT(current_object)->burning_boulder.depth_offset = 0x20u;
	OBJECT(current_object)->speed_x = 0;
	OBJECT(current_object)->direction = 0x200u;
	OBJECT(current_object)->hitbox_height = 0xA0u;
	OBJECT(current_object)->sprite_offset_x = 0;
	OBJECT(current_object)->sprite_offset_y = 0;
	OBJECT(current_object)->hitbox_offset_x = -0x20;
	OBJECT(current_object)->hitbox_offset_y = -0x70;
	OBJECT(current_object)->speed_z = 0;
	OBJECT(current_object)->burning_boulder.rumble_timer = 0x3Cu;
	burning_boulder_released = 0u;
	burning_boulder_release_timer = 0;
	OBJECT(current_object)->animation = burning_boulder_roll_animation_ADDRESS;
	OBJECT(current_object)->animation_frame = 0u;
	OBJECT(current_object)->animation_speed = 2u;
	OBJECT(current_object)->animation_timer = 0;
	if (AS16(OBJECT(current_object)->burning_boulder.path) == 0xFFFFu)
	{
		OBJECT(current_object)->hitbox_size_x = 0x120u;
		zone_index = 2u;
		object = spawn_chaser_back_zone(zone_index, 0u);
	}
	if (AS16(OBJECT(current_object)->burning_boulder.path) != 0xFFFFu)
		lead_burning_boulder = current_object;
	find_floor(&r0, &zone_index, &block_offset, &ignored, &object);
	OBJECT(current_object)->z = OBJECT(current_object)->floor_z;
	OBJECT(current_object)->z_int -= 0x70u;
	project_object(&ignored, &ignored);
	OBJECT(current_object)->burning_boulder.holding_back = 0u;
	OBJECT(current_object)->burning_boulder.wait_timer = 0;
	if (free_object_stack < 4)
		return;
	boulder_flames_left = 4;
	boulder_flame_number = 0u;
	object_2 = current_object;
	do
	{
		allocate_object(&ignored, &object);
		OBJECT(object)->sprite_flags = 0x100Au;
		OBJECT(object)->sprite_palette = 0x51C0u;
		OBJECT(object)->flags = 0u;
		OBJECT(object)->sprite_offset_x = 0;
		OBJECT(object)->sprite_offset_y = 0;
		OBJECT(object)->kind = boulder_flame_number;
		boulder_flame_number++;
		OBJECT(object)->handler = burning_boulder_flame_start_ADDRESS;
		OBJECT(object)->parent = current_object;
		OBJECT(object_2)->child = object;
		object_2 = object;
		flames_left_before = boulder_flames_left;
		boulder_flames_left = flames_left_before - 1u;
	} while ((int8_t)flames_left_before > 1);
	OBJECT(current_object)->handler = burning_boulder_update_ADDRESS;
}

void burning_boulder_update(void)
{
	uint32_t zero = 0;
	uint8_t sound_id = 0;
	uint16_t animation_speed = 0, roll_speed = 0;
	uint32_t r0 = 0, r1 = 0, r2 = 0, r7 = 0, cursor = 0, depth = 0, drop_step = 0, flames_left = 0, floor_slope = 0, ignored = 0, object = 0, path_cursor = 0, slope_factor = 0, speed_level = 0, step_y = 0;
	uint8_t hold_timer = 0;
	uint16_t start_delay = 0, wait_timer = 0, record_scroll = 0, record_y = 0, next_slow_timer = 0, rumble_before = 0, rumble_after = 0, start_delay_2 = 0;

	switch (OBJECT(current_object)->burning_boulder.state)
	{
	case 0:
	case 6:
		start_delay = OBJECT(current_object)->burning_boulder.start_delay;
		OBJECT(current_object)->burning_boulder.start_delay = start_delay - 1u;
		if ((int16_t)start_delay > 1)
			return;
		OBJECT(current_object)->burning_boulder.start_delay = 0;
		OBJECT(current_object)->speed_z += 0x6000u;
		OBJECT(current_object)->z += OBJECT(current_object)->speed_z;
		find_floor(&r0, &step_y, &r2, &ignored, &cursor);
		if (OBJECT(current_object)->floor_z_int <= OBJECT(current_object)->z_int)
		{
			OBJECT(current_object)->z = OBJECT(current_object)->floor_z;
			OBJECT(current_object)->burning_boulder.state++;
			if (OBJECT(current_object)->burning_boulder.path_high == 0xFFFFu)
				OBJECT(current_object)->burning_boulder.state = 2u;
			camera_shake_active = 1u;
			sound_id = 7u;
			play_sound(sound_id, &step_y, &r2);
		}
		project_object(&ignored, &ignored);
		add_to_zone_list(ignored);
		depth = (uint16_t)(OBJECT(current_object)->screen_y - OBJECT(current_object)->burning_boulder.depth_offset);
		if (OBJECT(current_object)->burning_boulder.path_high == 0xFFFFu)
			depth = (uint16_t)(depth - 0x20u);
		queue_sprite_at_depth(&ignored, &depth);
		return;
	case 1:
		if (OBJECT(current_object)->burning_boulder.holding_back == 0u)
		{
			if (no_player_in_play != 0u || player_mishap != 0u || player_dropping_in != 0u)
			{
				OBJECT(current_object)->burning_boulder.holding_back = 0xFFu;
				OBJECT(current_object)->burning_boulder.saved_base_speed = OBJECT(current_object)->burning_boulder.base_speed;
				OBJECT(current_object)->burning_boulder.base_speed = 0;
				OBJECT(current_object)->burning_boulder.hold_start_position = scroll_position;
				OBJECT(current_object)->burning_boulder.hold_timer = 0xFFu;
				OBJECT(current_object)->burning_boulder.saved_threshold = OBJECT(current_object)->burning_boulder.screen_threshold;
				OBJECT(current_object)->burning_boulder.screen_threshold = -0x20;
			}
			else
			{
				wait_timer = OBJECT(current_object)->burning_boulder.wait_timer;
				OBJECT(current_object)->burning_boulder.wait_timer = wait_timer - 1u;
				if ((int16_t)wait_timer <= 1)
				{
					OBJECT(current_object)->burning_boulder.wait_timer = 0;
					path_cursor = OBJECT(current_object)->burning_boulder.path;
					record_scroll = mem_read16(path_cursor);
					path_cursor += 2u;
					if (record_scroll <= scroll_position)
					{
						path_cursor += 2u;
						goto burning_boulder_load_record;
					}
					record_y = mem_read16(path_cursor);
					path_cursor += 2u;
					if ((int16_t)record_y <= OBJECT(current_object)->y_int)
					{
					burning_boulder_load_record:
						OBJECT(current_object)->burning_boulder.screen_threshold = mem_read16(path_cursor);
						path_cursor += 2u;
						OBJECT(current_object)->burning_boulder.catch_up_factor = mem_read16(path_cursor);
						path_cursor += 2u;
						OBJECT(current_object)->burning_boulder.base_speed = mem_read16(path_cursor);
						path_cursor += 2u;
						OBJECT(current_object)->burning_boulder.wait_timer = mem_read16(path_cursor);
						OBJECT(current_object)->burning_boulder.path = path_cursor + 2u;
					}
				}
			}
		}
		if (OBJECT(current_object)->burning_boulder.holding_back != 0u)
		{
			OBJECT(current_object)->burning_boulder.rumble_timer = 0x3Cu;
			if ((int16_t)(scroll_position - OBJECT(current_object)->burning_boulder.hold_start_position) >= 0x300)
			{
			burning_boulder_stop_holding_back:
				OBJECT(current_object)->burning_boulder.holding_back = 0u;
				OBJECT(current_object)->burning_boulder.screen_threshold = OBJECT(current_object)->burning_boulder.saved_threshold;
				OBJECT(current_object)->burning_boulder.base_speed = OBJECT(current_object)->burning_boulder.saved_base_speed;
				OBJECT(current_object)->burning_boulder.slow_timer = 0x78;
				OBJECT(current_object)->burning_boulder.rumble_timer = 0u;
			}
			else
			{
				if (no_player_in_play != 0u)
					OBJECT(current_object)->burning_boulder.hold_timer = 0xFFu;
				hold_timer = OBJECT(current_object)->burning_boulder.hold_timer;
				OBJECT(current_object)->burning_boulder.hold_timer = hold_timer - 1u;
				if (hold_timer == 0u)
					goto burning_boulder_stop_holding_back;
			}
		}
		next_slow_timer = OBJECT(current_object)->burning_boulder.slow_timer - 1u;
		OBJECT(current_object)->burning_boulder.slow_timer = next_slow_timer;
		if ((int16_t)next_slow_timer < 0)
			OBJECT(current_object)->burning_boulder.slow_timer = 0;
		if (OBJECT(current_object)->burning_boulder.wait_timer <= 0)
		{
			if (OBJECT(current_object)->burning_boulder.screen_threshold <= OBJECT(current_object)->screen_y)
			{
			burning_boulder_use_base_speed:
				roll_speed = (uint16_t)OBJECT(current_object)->burning_boulder.base_speed;
				if ((uint16_t)OBJECT(current_object)->burning_boulder.slow_timer != 0u)
					roll_speed = 0x10u;
			}
			else
			{
				slope_factor = 0u;
				if (OBJECT(current_object)->floor_slope != 0u)
				{
					floor_slope = OBJECT(current_object)->floor_slope;
					slope_factor = floor_slope_factors[(uint16_t)(floor_slope * 2u)];
				}
				compute_camera_slope_speed(0u, slope_factor);
				roll_speed = (uint16_t)((uint32_t)camera_slope_speed_int * OBJECT(current_object)->burning_boulder.catch_up_factor) >> 8;
				if ((int16_t)roll_speed <= OBJECT(current_object)->burning_boulder.base_speed)
					goto burning_boulder_use_base_speed;
			}
			OBJECT(current_object)->speed_fraction = roll_speed;
			step_y = (uint32_t)(int16_t)OBJECT(current_object)->speed_fraction << 12;
			OBJECT(current_object)->y += step_y;
			OBJECT(current_object)->speed_y = step_y;
		}
		speed_level = OBJECT(current_object)->speed_fraction;
		if (speed_level != 0u)
		{
			speed_level = speed_level >> 4;
			if ((int16_t)speed_level > 5)
				speed_level = 5u;
			OBJECT(current_object)->animation_speed = burning_boulder_roll_rates[speed_level];
			animate(&step_y, &r7, &slope_factor);
		}
		find_floor(&speed_level, &step_y, &r2, &ignored, &cursor);
		rumble_before = AS16(OBJECT(current_object)->burning_boulder.rumble_timer);
		rumble_after = rumble_before - 1u;
		zero = rumble_after == 0u;
		AS16(OBJECT(current_object)->burning_boulder.rumble_timer) = rumble_after;
		if ((int16_t)rumble_before <= 1)
		{
			animation_speed = OBJECT(current_object)->animation_speed;
			if (zero == 0u)
			{
				AS16(OBJECT(current_object)->burning_boulder.rumble_timer) = animation_speed * 0xAu;
				sound_id = 0xDu;
				play_sound(sound_id, &step_y, &r2);
			}
		}
		if (scroll_position < 0x3300u || (int16_t)AT16(OBJECT(current_object)->floor_block, 2) >= 0)
			OBJECT(current_object)->z = OBJECT(current_object)->floor_z;
		else
			OBJECT(current_object)->burning_boulder.state = 3u;
		project_object(&ignored, &ignored);
		add_to_zone_list(ignored);
		depth = (uint16_t)(OBJECT(current_object)->screen_y - OBJECT(current_object)->burning_boulder.depth_offset);
		OBJECT(current_object)->burning_boulder.draw_depth = depth;
		queue_sprite_at_depth(&ignored, &depth);
		return;
	case 2:
		object = (current_object & ~0xFFFFu) | lead_burning_boulder;
		step_y = (int16_t)OBJECT(object)->speed_fraction;
		if ((int8_t)AS8(OBJECT(object)->var_96) >= 3)
			step_y = (int16_t)OBJECT(current_object)->speed_fraction;
		step_y = step_y << 12;
		OBJECT(current_object)->y += step_y;
		OBJECT(current_object)->speed_y = step_y;
		speed_level = OBJECT(object)->speed_fraction;
		OBJECT(current_object)->speed_fraction = speed_level;
		if (speed_level != 0u)
		{
			speed_level = speed_level >> 4;
			if ((int16_t)speed_level > 5)
				speed_level = 5u;
			OBJECT(current_object)->animation_speed = burning_boulder_roll_rates[speed_level];
			animate(&step_y, &r7, &slope_factor);
		}
		find_floor(&speed_level, &step_y, &r2, &ignored, &cursor);
		if ((int16_t)AT16(OBJECT(current_object)->floor_block, 2) >= 0)
			OBJECT(current_object)->z = OBJECT(current_object)->floor_z;
		if (AS8(OBJECT((current_object & ~0xFFFFu) | lead_burning_boulder)->var_96) == 3u)
			OBJECT(current_object)->burning_boulder.state = 3u;
		project_object(&ignored, &ignored);
		add_to_zone_list(ignored);
		depth = (uint16_t)(OBJECT(current_object)->screen_y - OBJECT(current_object)->burning_boulder.depth_offset);
		OBJECT(current_object)->burning_boulder.draw_depth = depth;
		queue_sprite_at_depth(&ignored, &depth);
		return;
	case 3:
		drop_step = 0x60000u;
		if (scroll_position >= 0x4800u)
			drop_step = 0u;
		OBJECT(current_object)->z += drop_step;
		OBJECT(current_object)->x += 0x167D2u;
		OBJECT(current_object)->y += 0x1638Eu;
		OBJECT(current_object)->animation_speed = 2u;
		animate(&step_y, &r7, &slope_factor);
		project_object(&ignored, &ignored);
		if (scroll_position < 0x4C50u && OBJECT(current_object)->screen_y > -0x10)
		{
			depth = 0u;
			queue_sprite_at_depth(&ignored, &depth);
			return;
		}
		OBJECT(current_object)->burning_boulder.state++;
		OBJECT(current_object)->burning_boulder.start_delay = 0x1E;
		flames_left = 4u;
		do
		{
			current_object = LINKED(current_object, OBJECT(current_object)->child);
			OBJECT(current_object)->flags |= 0x400u;
			flames_left--;
		} while (flames_left != 0u);
		current_object = object_cursor;
		if (AS16(OBJECT(current_object)->burning_boulder.path) != 0xFFFFu)
		{
			depth = 0u;
			queue_sprite_at_depth(&ignored, &depth);
			return;
		}
		delete_current_object();
		return;
	case 4:
		burning_boulder_release_timer++;
		if (burning_boulder_release_timer >= 0x258)
			burning_boulder_released = 0xFFu;
		if (burning_boulder_released != 0u)
		{
			start_delay_2 = OBJECT(current_object)->burning_boulder.start_delay;
			OBJECT(current_object)->burning_boulder.start_delay = start_delay_2 - 1u;
			if ((int16_t)start_delay_2 <= 1)
			{
				OBJECT(current_object)->burning_boulder.state++;
				OBJECT(current_object)->flags &= ~1u;
				OBJECT(current_object)->x = 0x1800000;
				OBJECT(current_object)->y = 0xEF00000;
				OBJECT(current_object)->z = 0x9700000;
				OBJECT(current_object)->speed_z = 0xC0000;
				project_object(&ignored, &ignored);
				OBJECT(current_object)->sprite_palette = 0x1240u;
			}
		}
		return;
	case 5:
		if (OBJECT(current_object)->screen_y >= 0x150)
		{
			delete_current_object();
			return;
		}
		if ((OBJECT(current_object)->flags & 1u) == 0u)
		{
			if (OBJECT(current_object)->z_int >= 0xA28)
			{
				OBJECT(current_object)->flags |= 1u;
				camera_shake_active = 1u;
				sound_id = 0xAu;
				play_sound(sound_id, &step_y, &r2);
			}
			OBJECT(current_object)->speed_z += 0x6000u;
			OBJECT(current_object)->z += OBJECT(current_object)->speed_z;
		}
		if (OBJECT(current_object)->z_int >= 0x9E0)
			seesaw_launched = 0xFFu;
		project_object(&ignored, &r1);
		r1 = queue_sprite(r1);
		return;
	default:
		unknown_code(0x863F8u + OBJECT(current_object)->burning_boulder.state * 3u);
	}
}

void burning_boulder_flame_start(void)
{
	uint32_t layout_record = 0;

	layout_record = OBJECT(current_object)->kind * 6u + burning_boulder_flame_layout_ADDRESS;
	OBJECT(current_object)->burning_boulder_flame.offset_x = ROM16(layout_record);
	layout_record += 2u;
	OBJECT(current_object)->burning_boulder_flame.offset_y = ROM16(layout_record);
	OBJECT(current_object)->animation = ROM16(layout_record + 2u) + burning_boulder_flame_animations_ADDRESS;
	OBJECT(current_object)->animation_frame = 0xFFu;
	OBJECT(current_object)->animation_speed = 1u;
	OBJECT(current_object)->animation_timer = 0;
	OBJECT(current_object)->burning_boulder_flame.pause_timer = 0;
	OBJECT(current_object)->handler = burning_boulder_flame_flicker_ADDRESS;
}

void burning_boulder_flame_flicker(void)
{
	uint32_t r8 = 0, depth = 0, frame = 0, ignored = 0, object = 0, random_bits = 0, wrapped = 0;
	uint16_t pause_timer = 0;

	if ((OBJECT(current_object)->flags & 0x400u) != 0u)
	{
		delete_current_object();
		return;
	}
	pause_timer = OBJECT(current_object)->burning_boulder_flame.pause_timer;
	OBJECT(current_object)->burning_boulder_flame.pause_timer = pause_timer - 1u;
	if ((int16_t)pause_timer <= 1)
	{
		OBJECT(current_object)->burning_boulder_flame.pause_timer = 0;
		if ((int16_t)OBJECT(LINKED(current_object, OBJECT(current_object)->parent))->speed_fraction <= 0)
			return;
		object = LINKED(current_object, OBJECT(current_object)->parent);
		OBJECT(current_object)->screen_x = OBJECT(object)->screen_x;
		OBJECT(current_object)->screen_y = OBJECT(object)->screen_y;
		OBJECT(current_object)->screen_x += OBJECT(current_object)->burning_boulder_flame.offset_x;
		OBJECT(current_object)->screen_y += OBJECT(current_object)->burning_boulder_flame.offset_y;
		animate(&frame, &wrapped, &r8);
		if ((uint8_t)wrapped == 0u)
		{
			depth = (uint16_t)(AS16(OBJECT(LINKED(current_object, OBJECT(current_object)->parent))->player) + 1u);
			queue_sprite_at_depth(&ignored, &depth);
			return;
		}
		next_random(&random_bits, &ignored, &ignored);
		OBJECT(current_object)->burning_boulder_flame.pause_timer = random_bits & 0x3Fu;
	}
}

void ember_emitter_start(void)
{
	uint32_t r0 = 0, block_offset = 0, cursor = 0, ignored = 0, y = 0;

	OBJECT(current_object)->ember_emitter.spawn_timer = 0;
	OBJECT(current_object)->floor_slope = 0u;
	OBJECT(current_object)->floor_type = 0u;
	OBJECT(current_object)->flags = 0u;
	OBJECT(current_object)->ember_emitter.end_position = ember_emitter_end_positions[OBJECT(current_object)->sprite_frame];
	find_floor(&r0, &y, &block_offset, &ignored, &cursor);
	OBJECT(current_object)->z = OBJECT(current_object)->floor_z;
	OBJECT(current_object)->handler = ember_emitter_move_ADDRESS;
}

void ember_emitter_move(void)
{
	uint8_t random_low = 0;
	uint32_t floor_slope = 0, free_count = 0, ignored = 0, move_step = 0, object = 0, random_bits = 0, slope_factor = 0, speed_entry = 0, travel_direction = 0;
	uint16_t spawn_timer = 0;

	if (OBJECT(current_object)->ember_emitter.end_position <= scroll_position)
	{
		delete_current_object();
		return;
	}
	travel_direction = mem_read8(floor_blocks + 2u + OBJECT(current_object)->floor_block + 0x15u);
	OBJECT(current_object)->ember_emitter.travel_direction = travel_direction;
	switch (travel_direction)
	{
	case 0:
	case 3:
		if (OBJECT(current_object)->screen_x >= 0x40)
		{
		ember_emitter_keep_up:
			slope_factor = 0u;
			if (OBJECT(current_object)->floor_slope != 0u)
			{
				floor_slope = OBJECT(current_object)->floor_slope;
				slope_factor = floor_slope_factors[(uint16_t)(floor_slope * 2u)];
			}
			random_bits = compute_camera_slope_speed(0u, slope_factor);
			move_step = camera_slope_speed >> 7;
			switch (OBJECT(current_object)->ember_emitter.travel_direction)
			{
			case 0:
			case 4:
				OBJECT(current_object)->y -= move_step;
				break;
			case 1:
				OBJECT(current_object)->x += move_step;
				break;
			case 2:
				OBJECT(current_object)->y += move_step;
				break;
			case 3:
				OBJECT(current_object)->x -= move_step;
				break;
			default:
				unknown_code(0x869ABu + OBJECT(current_object)->ember_emitter.travel_direction * 2u);
			}
		}
		break;
	case 1:
	case 2:
	case 4:
		if (OBJECT(current_object)->screen_x <= 0x40)
			goto ember_emitter_keep_up;
		break;
	default:
		unknown_code(0x86958u + travel_direction * 2u);
	}
	if (OBJECT(current_object)->sprite_frame != 3u)
	{
		find_floor(&travel_direction, &floor_slope, &move_step, &random_bits, &object);
		OBJECT(current_object)->z = OBJECT(current_object)->floor_z;
	}
	project_object(&ignored, &ignored);
	spawn_timer = OBJECT(current_object)->ember_emitter.spawn_timer;
	OBJECT(current_object)->ember_emitter.spawn_timer = spawn_timer - 1u;
	if ((int16_t)spawn_timer > 1)
		return;
	allocate_object(&free_count, &object);
	if (free_count != 0u)
	{
		OBJECT(object)->sprite_flags = 0x1000u;
		OBJECT(object)->sprite_palette = 0x51D0u;
		OBJECT(object)->flags = 0u;
		OBJECT(object)->sprite_offset_x = 0;
		OBJECT(object)->sprite_offset_y = 0;
		OBJECT(object)->falling_ember.state = 0u;
		OBJECT(object)->falling_ember.travel_direction = OBJECT(current_object)->ember_emitter.travel_direction;
		OBJECT(object)->handler = falling_ember_update_ADDRESS;
		OBJECT(object)->x = OBJECT(current_object)->x;
		OBJECT(object)->y = OBJECT(current_object)->y;
		OBJECT(object)->z = OBJECT(current_object)->z;
		next_random(&random_bits, &ignored, &ignored);
		random_low = (uint8_t)random_bits;
		random_bits = (random_bits & ~0xFFFFu) | (random_bits & 0x7Fu);
		OBJECT(object)->x_int += random_bits;
		random_bits = random_bits >> 16;
		random_bits = random_bits & 0x7Fu;
		OBJECT(object)->y_int += random_bits;
		speed_entry = falling_ember_speeds_ADDRESS + (random_low & 3u) * 4u;
		OBJECT(object)->speed_z = (uint32_t)(int16_t)ROM16(speed_entry) << 8;
		OBJECT(object)->speed_y = (uint32_t)(int16_t)ROM16(speed_entry + 2u) << 8;
		OBJECT(object)->animation = falling_ember_animation_ADDRESS;
		OBJECT(object)->animation_frame = 0xFFu;
		OBJECT(object)->animation_speed = 2u;
		OBJECT(object)->animation_timer = 0;
	}
	next_random(&random_bits, &ignored, &ignored);
	OBJECT(current_object)->ember_emitter.spawn_timer = random_bits & 0x3Fu;
}

void falling_ember_update(void)
{
	uint32_t r0 = 0, r1 = 0, block_offset = 0, burst_animation = 0, cursor = 0, ignored = 0, off_screen = 0, wrapped = 0;

	switch (OBJECT(current_object)->falling_ember.state)
	{
	case 0:
	case 3:
		OBJECT(current_object)->falling_ember.state++;
		do
		{
			OBJECT(current_object)->z_int -= 0x70u;
			project_object(&ignored, &ignored);
		} while (OBJECT(current_object)->screen_y > 0);
		return;
	case 1:
		off_screen = check_left_screen();
		if (off_screen != 0u)
		{
			delete_current_object();
			return;
		}
		OBJECT(current_object)->y += OBJECT(current_object)->speed_y;
		OBJECT(current_object)->z += OBJECT(current_object)->speed_z;
		animate(&r1, &wrapped, &off_screen);
		find_floor(&r0, &r1, &block_offset, &ignored, &cursor);
		if (OBJECT(current_object)->falling_ember.floor_block_high < 0)
			OBJECT(current_object)->floor_z = 0x5300000;
		project_object(&ignored, &r1);
		if (OBJECT(current_object)->z_int < OBJECT(current_object)->floor_z_int)
		{
			r1 = queue_sprite(r1);
			return;
		}
		OBJECT(current_object)->falling_ember.state++;
		burst_animation = falling_ember_burst_animation_ADDRESS;
		if (OBJECT(current_object)->falling_ember.floor_block_high >= 0 && (OBJECT(current_object)->floor_slope | OBJECT(current_object)->floor_type) != 0u)
			burst_animation = falling_ember_slope_burst_animation_ADDRESS;
		OBJECT(current_object)->animation = burst_animation;
		OBJECT(current_object)->animation_frame = 0xFFu;
		OBJECT(current_object)->animation_speed = 2u;
		OBJECT(current_object)->animation_timer = 0;
		OBJECT(current_object)->z = OBJECT(current_object)->floor_z;
		r1 = queue_sprite(r1);
		return;
	case 2:
		off_screen = check_left_screen();
		if ((uint8_t)off_screen != 0u)
		{
			delete_current_object();
			return;
		}
		animate(&r1, &wrapped, &off_screen);
		if ((uint8_t)wrapped == 0u)
		{
			update_screen_position(&ignored, &r1);
			r1 = queue_sprite(r1);
			return;
		}
		delete_current_object();
		return;
	default:
		unknown_code(0x86AA8u + OBJECT(current_object)->falling_ember.state * 3u);
	}
}

void scene_carrier_start(void)
{
	uint32_t carrier_record = 0, sprite_frame = 0;

	carrier_record = scene_carrier_records[OBJECT(current_object)->sprite_frame] + scene_carrier_records_ADDRESS;
	OBJECT(current_object)->x_int = ROM16(carrier_record);
	OBJECT(current_object)->x_fraction = 0u;
	carrier_record += 2u;
	OBJECT(current_object)->y_int = ROM16(carrier_record);
	OBJECT(current_object)->y_fraction = 0u;
	carrier_record += 2u;
	OBJECT(current_object)->z_int = ROM16(carrier_record);
	OBJECT(current_object)->z_fraction = 0u;
	carrier_record += 2u;
	OBJECT(current_object)->speed_x = ROM32(carrier_record);
	carrier_record += 4u;
	OBJECT(current_object)->speed_y = ROM32(carrier_record);
	carrier_record += 4u;
	OBJECT(current_object)->speed_z = ROM32(carrier_record);
	OBJECT(current_object)->scene_carrier.end_position = ROM16(carrier_record + 4u);
	sprite_frame = OBJECT(current_object)->sprite_frame;
	if ((uint8_t)sprite_frame != 0u)
		player_carriers[(sprite_frame & ~0xFFu) | (uint8_t)(sprite_frame - 1u)] = current_object;
	else
		scene_object = current_object;
	OBJECT(current_object)->handler = scene_carrier_move_ADDRESS;
}

void scene_carrier_move(void)
{
	uint32_t ignored = 0;

	if (OBJECT(current_object)->scene_carrier.end_position <= scroll_position)
	{
		delete_current_object();
		return;
	}
	if (OBJECT(current_object)->sprite_frame == 0u || seesaw_launched != 0u)
	{
		OBJECT(current_object)->z += OBJECT(current_object)->speed_z;
		OBJECT(current_object)->y += OBJECT(current_object)->speed_y;
		OBJECT(current_object)->x += OBJECT(current_object)->speed_x;
	}
	project_object(&ignored, &ignored);
}

void seesaw_start(void)
{
	uint32_t free_count = 0, ignored = 0, part_list = 0;

	OBJECT(current_object)->sprite_flags = 0xD006u;
	OBJECT(current_object)->flags = 0u;
	OBJECT(current_object)->sprite_offset_x = 0;
	OBJECT(current_object)->sprite_offset_y = 0;
	allocate_object_2(&free_count, &ignored, &part_list);
	if (free_count != 0u)
	{
		move_halfwords_backward(part_list, seesaw_parts_ADDRESS, 0xDu);
		OBJECT(current_object)->part_list = part_list;
		OBJECT(current_object)->x = 0x700000;
		OBJECT(current_object)->y = 0xF680000;
		OBJECT(current_object)->z = 0x9E00000;
		project_object(&ignored, &ignored);
		OBJECT(current_object)->animation = seesaw_animation_ADDRESS;
		OBJECT(current_object)->animation_frame = 0u;
		OBJECT(current_object)->animation_speed = 2u;
		OBJECT(current_object)->animation_timer = 0;
		OBJECT(current_object)->handler = seesaw_tilt_ADDRESS;
	}
}

void seesaw_tilt(void)
{
	uint16_t part_list = 0;
	uint32_t r1 = 0, depth = 0, ignored = 0, next_step = 0, step_list = 0;
	uint8_t animation_timer = 0;
	uint16_t last_step = 0;

	if (scroll_position >= 0x4D00u)
	{
		part_list = (uint16_t)OBJECT(current_object)->part_list;
		free_object_2(part_list);
		delete_current_object();
		return;
	}
	if ((int8_t)OBJECT(current_object)->animation_speed >= 0 && seesaw_launched != 0u)
	{
		animation_timer = OBJECT(current_object)->animation_timer;
		OBJECT(current_object)->animation_timer = animation_timer - 1u;
		if ((int8_t)animation_timer > 1)
		{
			update_screen_position(&ignored, &r1);
			depth = 0u;
			queue_sprite_at_depth(&ignored, &depth);
			return;
		}
		OBJECT(current_object)->animation_timer = OBJECT(current_object)->animation_speed;
		next_step = OBJECT(current_object)->animation_frame;
		next_step = (uint8_t)(next_step + 1u);
		step_list = OBJECT(current_object)->animation;
		last_step = mem_read16(step_list);
		step_list += 2u;
		if (next_step == last_step)
			OBJECT(current_object)->animation_speed = 0xFFu;
		OBJECT(current_object)->animation_frame = next_step;
		mem_write16(OBJECT(current_object)->part_list + 0xAu, mem_read16(step_list + next_step * 2u));
	}
	update_screen_position(&ignored, &r1);
	depth = 0u;
	queue_sprite_at_depth(&ignored, &depth);
}

void metal_platform_base_start(void)
{
	uint32_t ignored = 0;

	OBJECT(current_object)->sprite_flags = 0x1002u;
	OBJECT(current_object)->sprite_frame = 0xAFB8u;
	OBJECT(current_object)->sprite_palette = 0x1230u;
	OBJECT(current_object)->flags = 0u;
	OBJECT(current_object)->sprite_offset_x = -6;
	OBJECT(current_object)->sprite_offset_y = 0x1B;
	OBJECT(current_object)->z = 0x5300000;
	project_object(&ignored, &ignored);
	OBJECT(current_object)->handler = metal_platform_base_draw_ADDRESS;
	metal_platform_base_draw();
}

void metal_platform_base_draw(void)
{
	uint16_t unseen_margin = 0;
	uint32_t depth = 0, ignored = 0, margin = 0, off_screen = 0;

	unseen_margin = 0x10u;
	margin = 0x40u;
	off_screen = check_left_screen_margins((uint16_t)margin, unseen_margin);
	if (off_screen == 0u)
	{
		update_screen_position(&ignored, &margin);
		depth = (uint16_t)(OBJECT(current_object)->screen_y - OBJECT(current_object)->sprite_offset_y - 0x28u);
		queue_sprite_at_depth(&ignored, &depth);
		return;
	}
	delete_current_object();
}

void metal_platform_start(void)
{
	uint32_t free_count = 0, ignored = 0, object = 0;

	OBJECT(current_object)->sprite_flags = 0x1002u;
	OBJECT(current_object)->sprite_frame = 0xAFC0u;
	OBJECT(current_object)->sprite_palette = 0x1230u;
	OBJECT(current_object)->flags = 0u;
	OBJECT(current_object)->sprite_offset_x = -6;
	OBJECT(current_object)->sprite_offset_y = 0x1B;
	OBJECT(current_object)->metal_platform.state = 0u;
	OBJECT(current_object)->metal_platform.timer = 0x12C;
	OBJECT(current_object)->z = 0x5300000;
	project_object(&ignored, &ignored);
	allocate_object(&free_count, &object);
	if (free_count != 0u)
	{
		OBJECT(object)->sprite_flags = 0x1001u;
		OBJECT(object)->sprite_frame = 0xAFC8u;
		OBJECT(object)->sprite_palette = 0x1230u;
		OBJECT(object)->flags = 0u;
		OBJECT(object)->sprite_offset_x = 0;
		OBJECT(object)->sprite_offset_y = 0;
		OBJECT(object)->x = 0x1310000;
		OBJECT(object)->y = 0xF240000;
		OBJECT(object)->z = 0x5300000;
		OBJECT(object)->handler = metal_platform_update_ADDRESS;
		OBJECT(object)->metal_platform.state = 3u;
		OBJECT(object)->parent = current_object;
		OBJECT(current_object)->handler = metal_platform_update_ADDRESS;
	}
}

void metal_platform_update(void)
{
	uint32_t r1 = 0, depth = 0, free_count = 0, ignored = 0, object = 0, object_2 = 0;
	uint16_t timer = 0, timer_2 = 0;

	switch (OBJECT(current_object)->metal_platform.state)
	{
	case 0:
	case 4:
		timer = OBJECT(current_object)->metal_platform.timer;
		OBJECT(current_object)->metal_platform.timer = timer - 1u;
		if ((int16_t)timer <= 1)
			OBJECT(current_object)->metal_platform.state++;
		update_screen_position(&ignored, &r1);
		depth = (uint16_t)(OBJECT(current_object)->screen_y - OBJECT(current_object)->sprite_offset_y - 0x28u);
		queue_sprite_at_depth(&ignored, &depth);
		return;
	case 1:
		OBJECT(current_object)->metal_platform.state++;
		metal_platform_lifted = 0xFFu;
		allocate_object(&free_count, &object_2);
		if (free_count != 0u)
			OBJECT(object_2)->handler = fire_wall_start_ADDRESS;
		OBJECT(current_object)->speed_z = 0x100000;
		OBJECT(current_object)->sprite_frame = 0xAFD0u;
		OBJECT(current_object)->metal_platform.frame = 0xAFD0u;
		OBJECT(current_object)->metal_platform.timer = 0xA;
		OBJECT(current_object)->floor_z = OBJECT(current_object)->z;
		OBJECT(current_object)->sprite_offset_y = 0xC;
		timer_2 = OBJECT(current_object)->metal_platform.timer;
		OBJECT(current_object)->metal_platform.timer = timer_2 - 1u;
		if ((int16_t)timer_2 <= 1)
		{
			OBJECT(current_object)->metal_platform.timer = 0;
			OBJECT(current_object)->speed_z += 0x6000u;
			OBJECT(current_object)->z -= OBJECT(current_object)->speed_z;
		}
		if (OBJECT(current_object)->screen_y <= -0x58)
		{
			delete_current_object();
			return;
		}
		if (OBJECT(current_object)->screen_y <= 0x28)
			OBJECT(current_object)->sprite_frame = OBJECT(current_object)->metal_platform.frame;
		project_object(&ignored, &ignored);
		depth = (uint16_t)(((int32_t)((uint32_t)OBJECT(current_object)->projected_y + OBJECT(current_object)->floor_z) >> 16) - view_y);
		queue_sprite_at_depth(&ignored, &depth);
		return;
	case 2:
		timer_2 = OBJECT(current_object)->metal_platform.timer;
		OBJECT(current_object)->metal_platform.timer = timer_2 - 1u;
		if ((int16_t)timer_2 <= 1)
		{
			OBJECT(current_object)->metal_platform.timer = 0;
			OBJECT(current_object)->speed_z += 0x6000u;
			OBJECT(current_object)->z -= OBJECT(current_object)->speed_z;
		}
		if (OBJECT(current_object)->screen_y <= -0x58)
		{
			delete_current_object();
			return;
		}
		if (OBJECT(current_object)->screen_y <= 0x28)
			OBJECT(current_object)->sprite_frame = OBJECT(current_object)->metal_platform.frame;
		project_object(&ignored, &ignored);
		depth = (uint16_t)(((int32_t)((uint32_t)OBJECT(current_object)->projected_y + OBJECT(current_object)->floor_z) >> 16) - view_y);
		queue_sprite_at_depth(&ignored, &depth);
		return;
	case 3:
		object = LINKED(current_object, OBJECT(current_object)->parent);
		if ((int8_t)OBJECT(object)->var_80 >= 1)
		{
			OBJECT(current_object)->sprite_flags = 0x1002u;
			OBJECT(current_object)->x = OBJECT(object)->x;
			OBJECT(current_object)->y = OBJECT(object)->y;
			OBJECT(current_object)->z = OBJECT(object)->z;
			OBJECT(current_object)->x_int += 0x2Du;
			OBJECT(current_object)->metal_platform.timer = 0xA;
			OBJECT(current_object)->sprite_offset_x = -0x24;
			OBJECT(current_object)->sprite_offset_y = 0x1C;
			OBJECT(current_object)->speed_z = 0x80000;
			OBJECT(current_object)->floor_z = OBJECT(current_object)->z;
			OBJECT(current_object)->sprite_frame = 0xAFD8u;
			OBJECT(current_object)->metal_platform.frame = 0xAFE8u;
			OBJECT(current_object)->metal_platform.state = 2u;
		}
		project_object(&ignored, &r1);
		r1 = queue_sprite(r1);
		return;
	default:
		unknown_code(0x86DF9u + OBJECT(current_object)->metal_platform.state * 3u);
	}
}

void fire_wall_start(void)
{
	uint8_t sound_id = 0;
	uint32_t r1 = 0, r2 = 0, free_count = 0, ignored = 0, part_list = 0;

	OBJECT(current_object)->sprite_flags = 0xD006u;
	OBJECT(current_object)->sprite_offset_x = 0;
	OBJECT(current_object)->sprite_offset_y = 0;
	OBJECT(current_object)->flags = 2u;
	OBJECT(current_object)->hitbox_size_x = 0x5Bu;
	OBJECT(current_object)->hitbox_size_y = 0xF0u;
	OBJECT(current_object)->hitbox_height = 0x100u;
	OBJECT(current_object)->hitbox_offset_x = -0x2A;
	OBJECT(current_object)->hitbox_offset_y = -1;
	OBJECT(current_object)->kind = 0x14u;
	OBJECT(current_object)->direction = 0x100u;
	OBJECT(current_object)->speed_x = 0;
	OBJECT(current_object)->speed_y = 0;
	OBJECT(current_object)->x = 0x1000000;
	OBJECT(current_object)->y = 0xE470000;
	OBJECT(current_object)->z = 0x6100000;
	OBJECT(current_object)->animation = fire_wall_animation_ADDRESS;
	OBJECT(current_object)->animation_speed = 2u;
	OBJECT(current_object)->animation_timer = 0;
	OBJECT(current_object)->animation_frame = 0u;
	OBJECT(current_object)->speed_z = -0x100000;
	project_object(&ignored, &r1);
	OBJECT(current_object)->child = 0xFFFFu;
	allocate_object_2(&free_count, &r2, &part_list);
	if (free_count != 0u)
	{
		move_halfwords_backward(part_list, fire_wall_parts_ADDRESS, 9u);
		OBJECT(current_object)->part_list = part_list;
		sound_id = 7u;
		play_sound(sound_id, &r1, &r2);
		OBJECT(current_object)->handler = fire_wall_rise_ADDRESS;
	}
}

void fire_wall_rise(void)
{
	uint16_t part_list = 0;
	uint32_t r1 = 0, r8 = 0, depth = 0, ignored = 0, part = 0;

	if (scroll_position >= 0x6800u)
	{
		part_list = (uint16_t)OBJECT(current_object)->part_list;
		free_object_2(part_list);
		delete_current_object();
		return;
	}
	if (OBJECT(current_object)->z_int > 0x550)
	{
		OBJECT(current_object)->speed_z += 0x6000u;
		OBJECT(current_object)->z += OBJECT(current_object)->speed_z;
	}
	update_screen_position(&ignored, &r1);
	animate_all_parts(&r8, &part);
	add_to_touchable_list(ignored);
	depth = (uint16_t)(OBJECT(current_object)->screen_y + 0x70u);
	queue_sprite_at_depth(&ignored, &depth);
}

void fire_trench_start(void)
{
	uint32_t ignored = 0;

	OBJECT(current_object)->sprite_flags = 0x100Au;
	OBJECT(current_object)->sprite_frame = 0xB200u;
	OBJECT(current_object)->sprite_palette = 0x51F0u;
	OBJECT(current_object)->flags = 0u;
	OBJECT(current_object)->sprite_offset_x = 0;
	OBJECT(current_object)->sprite_offset_y = -0x16;
	OBJECT(current_object)->fire_trench_start.flame_timer = 0;
	OBJECT(current_object)->handler = fire_trench_update_ADDRESS;
	OBJECT(current_object)->z = 0x5300000;
	project_object(&ignored, &ignored);
}

void fire_trench_update(void)
{
	uint32_t depth = 0, ignored = 0, margin = 0, off_screen = 0;
	uint16_t flame_timer = 0;

	margin = 0x70u;
	off_screen = check_left_screen_margin((uint16_t)margin);
	if (off_screen != 0u)
	{
		delete_current_object();
		return;
	}
	flame_timer = OBJECT(current_object)->fire_trench_update.flame_timer;
	OBJECT(current_object)->fire_trench_update.flame_timer = flame_timer - 1u;
	if ((int16_t)flame_timer <= 1)
	{
		OBJECT(current_object)->fire_trench_update.flame_timer = 0x12C;
		spawn_fire_trench_flames();
	}
	update_screen_position(&ignored, &margin);
	depth = (uint16_t)(OBJECT(current_object)->screen_y - OBJECT(current_object)->sprite_offset_y - 0x20u);
	queue_sprite_at_depth(&ignored, &depth);
}

void spawn_fire_trench_flames(void)
{
	uint8_t sound_id = 0;
	uint32_t r1 = 0, r2 = 0, ignored = 0, object = 0;
	uint16_t flames_left_before = 0;

	if (free_object_stack < 3)
		return;
	trench_flame_y = 0xE47u;
	trench_flames_left = 3;
	do
	{
		allocate_object(&ignored, &object);
		OBJECT(object)->sprite_flags = 0x1006u;
		OBJECT(object)->sprite_palette = 0x5200u;
		OBJECT(object)->flags = 2u;
		OBJECT(object)->sprite_offset_x = -8;
		OBJECT(object)->sprite_offset_y = 0x3E;
		OBJECT(object)->hitbox_offset_x = 0;
		OBJECT(object)->hitbox_offset_y = 0;
		OBJECT(object)->kind = 0x14u;
		OBJECT(object)->direction = 0x100u;
		OBJECT(object)->hitbox_size_x = 0x40u;
		OBJECT(object)->hitbox_size_y = 0x80u;
		OBJECT(object)->handler = fire_trench_flame_burn_ADDRESS;
		OBJECT(object)->x = OBJECT(current_object)->x;
		OBJECT(object)->y_int = trench_flame_y;
		OBJECT(object)->y_fraction = 0u;
		OBJECT(object)->z = 0x5300000;
		trench_flame_y += 0x50u;
		OBJECT(object)->animation = fire_trench_flame_animation_ADDRESS;
		OBJECT(object)->animation_frame = 0xFFu;
		OBJECT(object)->animation_timer = 0;
		OBJECT(object)->animation_speed = 2u;
		flames_left_before = trench_flames_left;
		trench_flames_left = flames_left_before - 1u;
	} while ((int16_t)flames_left_before > 1);
	if (OBJECT(current_object)->screen_x <= 0x140)
	{
		sound_id = 7u;
		play_sound(sound_id, &r1, &r2);
	}
}

void fire_trench_flame_burn(void)
{
	uint32_t r7 = 0, at_last_step = 0, depth = 0, frame = 0, ignored = 0;

	animate(&frame, &r7, &at_last_step);
	if ((uint8_t)at_last_step == 0u)
	{
		OBJECT(current_object)->hitbox_height = fire_trench_flame_heights[OBJECT(current_object)->animation_frame];
		project_object(&ignored, &ignored);
		add_to_touchable_list(ignored);
		depth = (uint16_t)(OBJECT(current_object)->screen_y - OBJECT(current_object)->sprite_offset_y);
		queue_sprite_at_depth(&ignored, &depth);
		return;
	}
	delete_current_object();
}

void ridden_object_start(void)
{
	OBJECT(current_object)->flags = 2u;
	OBJECT(current_object)->hitbox_size_x = 0x10u;
	OBJECT(current_object)->hitbox_size_y = 0x10u;
	OBJECT(current_object)->handler = ridden_object_fall_ADDRESS;
	scene_object = current_object;
	ridden_object_fall();
}

void ridden_object_fall(void)
{
	if ((OBJECT(current_object)->flags & 8u) != 0u)
		OBJECT(current_object)->handler = ridden_object_follow_ADDRESS;
	add_to_rider_list();
}

void ridden_object_follow(void)
{
	uint32_t object = 0;

	if (scroll_position < 0x8D00u)
	{
		object = LINKED(current_object, OBJECT(current_object)->platform);
		OBJECT(current_object)->x = OBJECT(object)->x;
		OBJECT(current_object)->z = OBJECT(object)->z;
		return;
	}
	delete_current_object();
}

void bridge_fire_start(void)
{
	uint32_t r0 = 0, block_offset = 0, ignored = 0, object = 0, y = 0;
	uint16_t flames_left_before = 0;

	OBJECT(current_object)->flags = 2u;
	OBJECT(current_object)->hitbox_size_x = 0x40u;
	OBJECT(current_object)->hitbox_size_y = 0x80u;
	OBJECT(current_object)->hitbox_height = 0x70u;
	OBJECT(current_object)->hitbox_offset_x = -0x20;
	OBJECT(current_object)->hitbox_offset_y = -0x10;
	OBJECT(current_object)->kind = 0x13u;
	OBJECT(current_object)->bridge_fire.state = 2u;
	OBJECT(current_object)->bridge_fire.off_screen = 0u;
	OBJECT(current_object)->bridge_fire.sound_timer = 0u;
	OBJECT(current_object)->z = 0x8680000;
	OBJECT(current_object)->floor_z = OBJECT(current_object)->z;
	OBJECT(current_object)->player = 0u;
	rising_fire_object = current_object;
	OBJECT(current_object)->bridge_fire.path = bridge_fire_script_ADDRESS;
	OBJECT(current_object)->bridge_fire.holding_back = 0u;
	OBJECT(current_object)->bridge_fire.wait_timer = 0;
	find_floor(&r0, &y, &block_offset, &ignored, &object);
	project_object(&ignored, &ignored);
	OBJECT(current_object)->bridge_fire.projected_x_whole = OBJECT(current_object)->projected_x;
	OBJECT(current_object)->spawn_arg_2 = 0u;
	bridge_flames_left = 3;
	if (free_object_stack < bridge_flames_left)
		return;
	bridge_flame_offset_y = 0u;
	do
	{
		allocate_object(&ignored, &object);
		OBJECT(object)->rising_flame.state = 0u;
		OBJECT(object)->handler = rising_flame_update_ADDRESS;
		OBJECT(object)->x = OBJECT(current_object)->x;
		OBJECT(object)->y = OBJECT(current_object)->y;
		bridge_flame_offset_y += 0x200000u;
		OBJECT(object)->y += bridge_flame_offset_y;
		OBJECT(object)->z = OBJECT(current_object)->z;
		OBJECT(object)->z_int -= OBJECT(current_object)->hitbox_height;
		flames_left_before = bridge_flames_left;
		bridge_flames_left = flames_left_before - 1u;
	} while ((int16_t)flames_left_before > 1);
	OBJECT(current_object)->handler = bridge_fire_rise_ADDRESS;
}

void bridge_fire_rise(void)
{
	uint32_t r2 = 0, climb_speed = 0, path_cursor = 0, point_x = 0, point_y = 0, projected_dx = 0, sound_id = 0;
	uint8_t hold_timer = 0, sound_timer = 0;
	uint16_t record_height = 0, wait_timer = 0;
	uint32_t flags = 0;

	flags = OBJECT(current_object)->flags;
	OBJECT(current_object)->flags = flags | 0x80000000u;
	if ((flags & 0x80000000u) == 0u)
	{
	bridge_fire_read_script:
		path_cursor = OBJECT(current_object)->bridge_fire.path;
		record_height = mem_read16(path_cursor);
		path_cursor += 2u;
		if ((int16_t)(OBJECT(current_object)->z_int - OBJECT(current_object)->hitbox_height) <= (int16_t)record_height || (mem_read16(path_cursor) != 0u && (uint16_t)(mem_read16(path_cursor) - OBJECT(current_object)->hitbox_height) <= scroll_position))
		{
			path_cursor += 2u;
			if ((int16_t)mem_read16(path_cursor + 2u) < 0)
			{
				delete_current_object();
				return;
			}
			if ((int16_t)mem_read16(path_cursor) < 0)
				OBJECT(current_object)->bridge_fire.state = 7u;
			OBJECT(current_object)->bridge_fire.screen_threshold = mem_read16(path_cursor);
			path_cursor += 2u;
			OBJECT(current_object)->bridge_fire.catch_up_factor = mem_read16(path_cursor);
			path_cursor += 2u;
			OBJECT(current_object)->bridge_fire.base_speed = mem_read16(path_cursor);
			path_cursor += 2u;
			OBJECT(current_object)->bridge_fire.wait_timer = mem_read16(path_cursor);
			OBJECT(current_object)->bridge_fire.path = path_cursor + 2u;
		}
	}
	else if (OBJECT(current_object)->bridge_fire.holding_back == 0u)
	{
		if (no_player_in_play == 0u && player_mishap == 0u && player_dropping_in == 0u)
			goto bridge_fire_read_script;
		OBJECT(current_object)->bridge_fire.holding_back = 0xFFu;
		OBJECT(current_object)->bridge_fire.saved_base_speed = OBJECT(current_object)->bridge_fire.base_speed;
		OBJECT(current_object)->bridge_fire.base_speed = 0;
		OBJECT(current_object)->bridge_fire.hold_start_position = scroll_position;
		OBJECT(current_object)->bridge_fire.hold_timer = 0xFFu;
		OBJECT(current_object)->bridge_fire.saved_threshold = OBJECT(current_object)->bridge_fire.screen_threshold;
		OBJECT(current_object)->bridge_fire.screen_threshold = 0xE0;
	}
	if (OBJECT(current_object)->bridge_fire.holding_back != 0u)
	{
		if ((int16_t)(scroll_position - OBJECT(current_object)->bridge_fire.hold_start_position) >= 0x300)
			goto bridge_fire_stop_holding_back;
		if (no_player_in_play != 0u)
			OBJECT(current_object)->bridge_fire.hold_timer = 0xFFu;
		hold_timer = OBJECT(current_object)->bridge_fire.hold_timer;
		OBJECT(current_object)->bridge_fire.hold_timer = hold_timer - 1u;
		if (hold_timer == 0u)
			goto bridge_fire_stop_holding_back;
	}
	if ((int16_t)(OBJECT(current_object)->screen_y - OBJECT(current_object)->hitbox_height) <= OBJECT(current_object)->bridge_fire.screen_threshold)
	{
	bridge_fire_use_base_speed:
		climb_speed = (uint16_t)OBJECT(current_object)->bridge_fire.base_speed;
	}
	else
	{
		if (OBJECT(current_object)->bridge_fire.holding_back != 0u)
		{
		bridge_fire_stop_holding_back:
			OBJECT(current_object)->bridge_fire.holding_back = 0u;
			OBJECT(current_object)->bridge_fire.screen_threshold = OBJECT(current_object)->bridge_fire.saved_threshold;
			OBJECT(current_object)->bridge_fire.base_speed = OBJECT(current_object)->bridge_fire.saved_base_speed;
		}
		compute_camera_ground_speed(0u);
		climb_speed = (uint16_t)((uint32_t)camera_ground_speed_int * OBJECT(current_object)->bridge_fire.catch_up_factor) >> 8;
		if ((int16_t)climb_speed <= OBJECT(current_object)->bridge_fire.base_speed)
			goto bridge_fire_use_base_speed;
	}
	OBJECT(current_object)->speed_fraction = climb_speed;
	wait_timer = OBJECT(current_object)->bridge_fire.wait_timer;
	OBJECT(current_object)->bridge_fire.wait_timer = wait_timer - 1u;
	if ((int16_t)wait_timer <= 1)
	{
		OBJECT(current_object)->bridge_fire.wait_timer = 0;
		OBJECT(current_object)->z -= (uint32_t)(int16_t)OBJECT(current_object)->speed_fraction << 12;
	}
	projected_dx = (int32_t)((uint32_t)(int16_t)ROM16(floor_attribute_slopes_ADDRESS + OBJECT(current_object)->floor_attribute) * OBJECT(current_object)->speed_fraction) >> 4;
	OBJECT(current_object)->bridge_fire.fixed_projected_x += projected_dx;
	OBJECT(current_object)->projected_x = OBJECT(current_object)->bridge_fire.projected_x_whole;
	OBJECT(current_object)->projected_y -= (int32_t)projected_dx >> 1;
	update_screen_position(&sound_id, &climb_speed);
	if (OBJECT(current_object)->screen_y > 0x160)
		OBJECT(current_object)->bridge_fire.off_screen = 0xFFu;
	else
	{
		if (OBJECT(current_object)->bridge_fire.off_screen != 0u)
		{
			OBJECT(current_object)->bridge_fire.off_screen = 0u;
			OBJECT(current_object)->bridge_fire.sound_timer = 0u;
		}
		if (current_game_mode != 7u)
		{
			sound_timer = OBJECT(current_object)->bridge_fire.sound_timer;
			OBJECT(current_object)->bridge_fire.sound_timer = sound_timer - 1u;
			if ((int8_t)sound_timer <= 1)
			{
				OBJECT(current_object)->bridge_fire.sound_timer = 0x3Cu;
				sound_id = 9u;
				play_sound((uint8_t)sound_id, &climb_speed, &r2);
			}
		}
	}
	point_x = OBJECT(current_object)->bridge_fire.fixed_projected_x;
	point_y = OBJECT(current_object)->projected_y * 2u;
	unproject_point(&point_x, &point_y);
	OBJECT(current_object)->x = point_x;
	OBJECT(current_object)->y = point_y;
	if (OBJECT(current_object)->player == 0u && (int16_t)(OBJECT(current_object)->z_int - OBJECT(current_object)->hitbox_height) <= 0x534)
	{
		stage_trigger_bits = 0xFFFFFFFFu;
		OBJECT(current_object)->player = 0xFFu;
	}
	sound_id = add_to_touchable_list(sound_id);
}

void lava_flow_start(void)
{
	uint32_t free_count = 0, ignored = 0, part_list = 0;

	allocate_object_2(&free_count, &ignored, &part_list);
	if (free_count != 0u)
	{
		move_halfwords_backward(part_list, lava_flow_parts_ADDRESS, 9u);
		OBJECT(current_object)->part_list = part_list;
		OBJECT(current_object)->sprite_flags = 0xD008u;
		OBJECT(current_object)->flags = 2u;
		OBJECT(current_object)->parent = current_object;
		OBJECT(current_object)->child = 0xFFFFu;
		OBJECT(current_object)->lava_flow.path = lava_flow_script_ADDRESS;
		OBJECT(current_object)->lava_flow.holding_back = 0u;
		OBJECT(current_object)->lava_flow.wait_timer = 0x78;
		OBJECT(current_object)->spawn_arg_2 = 2u;
		OBJECT(current_object)->lava_flow.unused_word = 0u;
		OBJECT(current_object)->lava_flow.sound_timer = 0xFFu;
		OBJECT(current_object)->handler = lava_flow_wait_ADDRESS;
		metal_platform_lifted = 0u;
		OBJECT(current_object)->sprite_offset_x = 0;
		OBJECT(current_object)->sprite_offset_y = 0;
		OBJECT(current_object)->hitbox_offset_x = -0x6A;
		OBJECT(current_object)->hitbox_offset_y = 0;
		OBJECT(current_object)->hitbox_size_x = 0x60u;
		OBJECT(current_object)->hitbox_size_y = 0xE7u;
		OBJECT(current_object)->hitbox_height = 0x50u;
		OBJECT(current_object)->kind = 0x14u;
		OBJECT(current_object)->direction = 0x100u;
		OBJECT(current_object)->animation = lava_flow_animation_ADDRESS;
		OBJECT(current_object)->animation_frame = 0u;
		OBJECT(current_object)->animation_speed = 6u;
		OBJECT(current_object)->animation_timer = 0;
		OBJECT(current_object)->z = 0x5300000;
	}
}

void lava_flow_wait(void)
{
	uint16_t wait_timer = 0;

	if (metal_platform_lifted != 0u)
	{
		wait_timer = OBJECT(current_object)->lava_flow.wait_timer;
		OBJECT(current_object)->lava_flow.wait_timer = wait_timer - 1u;
		if ((int16_t)wait_timer <= 1)
		{
			OBJECT(current_object)->handler = lava_flow_advance_ADDRESS;
			spawn_lava_steam();
		}
	}
}

void lava_flow_advance(void)
{
	uint16_t part_list = 0;
	uint32_t r0 = 0, r2 = 0, r3 = 0, r8 = 0, r11 = 0, depth = 0, flow_speed = 0, ignored = 0, path_cursor = 0;
	uint8_t hold_timer = 0, sound_timer = 0;
	uint16_t record_scroll = 0, wait_timer = 0;
	uint32_t flags = 0;

	flags = OBJECT(current_object)->flags;
	OBJECT(current_object)->flags = flags | 0x80000000u;
	if ((flags & 0x80000000u) == 0u)
	{
	lava_flow_read_script:
		path_cursor = OBJECT(current_object)->lava_flow.path;
		record_scroll = mem_read16(path_cursor);
		path_cursor += 2u;
		if (record_scroll <= scroll_position || (mem_read16(path_cursor) != 0u && (int16_t)mem_read16(path_cursor) <= OBJECT(current_object)->x_int))
		{
			path_cursor += 2u;
			if ((int16_t)mem_read16(path_cursor + 2u) < 0)
			{
				part_list = (uint16_t)OBJECT(current_object)->part_list;
				free_object_2(part_list);
				delete_current_object();
				return;
			}
			OBJECT(current_object)->lava_flow.screen_threshold = mem_read16(path_cursor);
			path_cursor += 2u;
			OBJECT(current_object)->lava_flow.catch_up_factor = mem_read16(path_cursor);
			path_cursor += 2u;
			OBJECT(current_object)->lava_flow.base_speed = mem_read16(path_cursor);
			path_cursor += 2u;
			OBJECT(current_object)->lava_flow.wait_timer = mem_read16(path_cursor);
			OBJECT(current_object)->lava_flow.path = path_cursor + 2u;
		}
	}
	else if (OBJECT(current_object)->lava_flow.holding_back == 0u)
	{
		if (no_player_in_play == 0u && player_mishap == 0u && player_dropping_in == 0u)
			goto lava_flow_read_script;
		OBJECT(current_object)->lava_flow.holding_back = 0xFFu;
		OBJECT(current_object)->lava_flow.saved_base_speed = OBJECT(current_object)->lava_flow.base_speed;
		OBJECT(current_object)->lava_flow.base_speed = 0;
		OBJECT(current_object)->lava_flow.hold_start_position = scroll_position;
		OBJECT(current_object)->lava_flow.hold_timer = 0xFFu;
		OBJECT(current_object)->lava_flow.saved_threshold = OBJECT(current_object)->lava_flow.screen_threshold;
		OBJECT(current_object)->lava_flow.screen_threshold = -0x50;
	}
	project_object(&r0, &ignored);
	if (OBJECT(current_object)->lava_flow.holding_back != 0u)
	{
		if ((int16_t)(scroll_position - OBJECT(current_object)->lava_flow.hold_start_position) >= 0x300)
			goto lava_flow_stop_holding_back;
		if (no_player_in_play != 0u)
			OBJECT(current_object)->lava_flow.hold_timer = 0xFFu;
		hold_timer = OBJECT(current_object)->lava_flow.hold_timer;
		OBJECT(current_object)->lava_flow.hold_timer = hold_timer - 1u;
		if (hold_timer == 0u)
			goto lava_flow_stop_holding_back;
	}
	if (OBJECT(current_object)->lava_flow.screen_threshold <= OBJECT(current_object)->screen_x)
	{
	lava_flow_use_base_speed:
		flow_speed = (uint16_t)OBJECT(current_object)->lava_flow.base_speed;
	}
	else
	{
		if (OBJECT(current_object)->lava_flow.holding_back != 0u)
		{
		lava_flow_stop_holding_back:
			OBJECT(current_object)->lava_flow.holding_back = 0u;
			OBJECT(current_object)->lava_flow.screen_threshold = OBJECT(current_object)->lava_flow.saved_threshold;
			OBJECT(current_object)->lava_flow.base_speed = OBJECT(current_object)->lava_flow.saved_base_speed;
		}
		r3 = compute_camera_ground_speed(0u);
		flow_speed = (uint16_t)((uint32_t)camera_ground_speed_int * OBJECT(current_object)->lava_flow.catch_up_factor) >> 8;
		if ((int16_t)flow_speed <= OBJECT(current_object)->lava_flow.base_speed)
			goto lava_flow_use_base_speed;
	}
	chaser_speed = flow_speed;
	wait_timer = OBJECT(current_object)->lava_flow.wait_timer;
	OBJECT(current_object)->lava_flow.wait_timer = wait_timer - 1u;
	if ((int16_t)wait_timer <= 1)
	{
		OBJECT(current_object)->lava_flow.wait_timer = 0;
		r0 = (uint32_t)chaser_speed << 12;
		OBJECT(current_object)->x += r0;
		OBJECT(current_object)->speed_x = r0;
		OBJECT(current_object)->speed_y = 0;
	}
	animate_all_parts(&r8, &r11);
	if (OBJECT(current_object)->screen_x < -0x40)
		OBJECT(current_object)->lava_flow.sound_timer = 0xFFu;
	else
	{
		if (OBJECT(current_object)->lava_flow.sound_timer != 0u)
		{
			OBJECT(current_object)->lava_flow.sound_timer = 0u;
			OBJECT(current_object)->lava_flow.sound_timer = 0u;
		}
		if (current_game_mode != 7u)
		{
			sound_timer = OBJECT(current_object)->lava_flow.sound_timer;
			OBJECT(current_object)->lava_flow.sound_timer = sound_timer - 1u;
			if (sound_timer == 0u)
			{
				OBJECT(current_object)->lava_flow.sound_timer = 0x40u;
				r0 = 0x10u;
				play_sound((uint8_t)r0, &flow_speed, &r2);
			}
		}
	}
	r11 = spawn_lava_flame(r3, r11);
	extend_lava_flow(r11);
	add_to_touchable_list(r0);
	depth = (uint16_t)(OBJECT(current_object)->screen_y + 0x50u);
	queue_sprite_at_depth(&ignored, &depth);
}

void extend_lava_flow(uint32_t block)
{
	uint32_t ignored = 0, object = 0;

	if ((uint16_t)free_object_stack != 0u && (uint16_t)free_object_stack_2[0] != 0u && (int16_t)OBJECT(current_object)->child < 0 && OBJECT(current_object)->screen_x >= -0x50 && scroll_position >= 0x6180u)
	{
		allocate_object(&ignored, &block);
		OBJECT(block)->sprite_flags = 0xD008u;
		OBJECT(block)->flags = 0u;
		OBJECT(block)->sprite_offset_x = 0;
		OBJECT(block)->sprite_offset_y = 0;
		OBJECT(block)->child = 0xFFFFu;
		OBJECT(block)->spawn_arg_2 = 0u;
		OBJECT(block)->parent = current_object;
		OBJECT(current_object)->child = block;
		OBJECT(block)->handler = lava_flow_segment_follow_ADDRESS;
		OBJECT(block)->z = OBJECT(current_object)->z;
		object = block;
		allocate_object_2(&ignored, &ignored, &block);
		move_halfwords_backward(block, lava_flow_parts_ADDRESS, 9u);
		OBJECT(object)->part_list = block;
	}
}

void lava_flow_segment_follow(void)
{
	uint16_t part_list = 0;
	uint32_t r1 = 0, r11 = 0, depth = 0, ignored = 0, object = 0, part_list_2 = 0, part_list_3 = 0;

	if (scroll_position >= 0x7E00u)
	{
		part_list = (uint16_t)OBJECT(current_object)->part_list;
		free_object_2(part_list);
		delete_current_object();
		return;
	}
	object = LINKED(current_object, OBJECT(current_object)->parent);
	OBJECT(current_object)->projected_x = OBJECT(object)->projected_x;
	OBJECT(current_object)->projected_x -= 0x40u;
	OBJECT(current_object)->projected_y = OBJECT(object)->projected_y;
	OBJECT(current_object)->projected_y_int += 0x20u;
	update_screen_position(&ignored, &r1);
	if (OBJECT(current_object)->screen_x > -0xA0)
	{
		object = LINKED(current_object, OBJECT(current_object)->parent);
		OBJECT(current_object)->x = OBJECT(object)->x;
		OBJECT(current_object)->x_int -= 0x50u;
		OBJECT(current_object)->y = OBJECT(object)->y;
		part_list_2 = OBJECT(LINKED(current_object, OBJECT(current_object)->parent))->part_list;
		part_list_3 = OBJECT(current_object)->part_list;
		mem_write16(part_list_3 + 2u, mem_read16(part_list_2 + 2u));
		mem_write16(part_list_3 + 0xAu, mem_read16(part_list_2 + 2u));
		r11 = spawn_lava_flame(0u, 0u);
		extend_lava_flow(r11);
		depth = (uint16_t)(OBJECT(LINKED(current_object, OBJECT(current_object)->parent))->screen_y + 0x48u);
		queue_sprite_at_depth(&ignored, &depth);
		return;
	}
	OBJECT(LINKED(current_object, OBJECT(current_object)->parent))->child = 0xFFFFu;
	part_list = (uint16_t)OBJECT(current_object)->part_list;
	free_object_2(part_list);
	delete_current_object();
}

uint32_t spawn_lava_flame(uint32_t random_value, uint32_t flame)
{
	uint32_t flame_random = 0, free_count = 0, ignored = 0;
	uint32_t saved_current_object = 0;
	uint16_t spawn_arg_2 = 0;

	spawn_arg_2 = OBJECT(current_object)->spawn_arg_2;
	OBJECT(current_object)->spawn_arg_2 = spawn_arg_2 - 1u;
	if ((int16_t)spawn_arg_2 <= 1)
	{
		next_random(&random_value, &ignored, &ignored);
		lava_flame_random = random_value;
		OBJECT(current_object)->spawn_arg_2 = random_value & 0x1Fu;
		allocate_object(&free_count, &flame);
		if (free_count == 0u)
			return flame;
		OBJECT(flame)->sprite_flags = 0x1004u;
		OBJECT(flame)->sprite_palette = 0x50C0u;
		OBJECT(flame)->flags = 0u;
		OBJECT(flame)->sprite_offset_x = 0;
		OBJECT(flame)->sprite_offset_y = 0;
		OBJECT(flame)->handler = lava_flame_burn_ADDRESS;
		OBJECT(flame)->z = OBJECT(current_object)->z;
		OBJECT(flame)->parent = current_object;
		OBJECT(flame)->animation = lava_flame_animation_ADDRESS;
		flame_random = lava_flame_random;
		if ((flame_random & 1u) != 0u)
			OBJECT(flame)->animation = lava_flame_animation_2_ADDRESS;
		OBJECT(flame)->animation_frame = 0u;
		OBJECT(flame)->animation_speed = 4u;
		OBJECT(flame)->animation_timer = 0;
		OBJECT(flame)->anchor_y = (((uint16_t)flame_random & 0xFFu) * 0xC0C1u + 0x100000u) >> 16;
		OBJECT(flame)->anchor_x = 0u - (((flame_random >> 16) & 0xFFu) + 0x30u);
		saved_current_object = current_object;
		current_object = flame;
		project_object(&ignored, &ignored);
		current_object = saved_current_object;
	}
	return flame;
}

void lava_flame_burn(void)
{
	uint32_t r1 = 0, r8 = 0, depth = 0, ignored = 0, object = 0, wrapped = 0;

	object = LINKED(current_object, OBJECT(current_object)->parent);
	OBJECT(current_object)->x = OBJECT(object)->x;
	OBJECT(current_object)->x_int += OBJECT(current_object)->anchor_x;
	OBJECT(current_object)->y = OBJECT(object)->y;
	OBJECT(current_object)->y_int += OBJECT(current_object)->anchor_y;
	project_object(&ignored, &r1);
	animate(&r1, &wrapped, &r8);
	if ((uint8_t)wrapped == 0u)
	{
		depth = (uint16_t)(OBJECT(current_object)->screen_y + 0x50u);
		queue_sprite_at_depth(&ignored, &depth);
		return;
	}
	delete_current_object();
}

void spawn_lava_steam(void)
{
	uint32_t free_count = 0, object = 0, offset_cursor = 0, puffs_left = 0;

	puffs_left = lava_steam_puff_count;
	offset_cursor = lava_steam_puff_offsets_ADDRESS;
	do
	{
		allocate_object(&free_count, &object);
		if (free_count == 0u)
			break;
		OBJECT(object)->sprite_flags = 0x1005u;
		OBJECT(object)->sprite_palette = 0x50D0u;
		OBJECT(object)->flags = 0u;
		OBJECT(object)->sprite_offset_x = 0;
		OBJECT(object)->sprite_offset_y = 0;
		OBJECT(object)->handler = lava_steam_puff_update_ADDRESS;
		OBJECT(object)->spawn_arg_2 = 2u;
		OBJECT(object)->animation = lava_steam_animation_ADDRESS;
		OBJECT(object)->animation_frame = 0xFFu;
		OBJECT(object)->animation_timer = 0;
		OBJECT(object)->animation_speed = 4u;
		OBJECT(object)->anchor_x = ROM16(offset_cursor);
		offset_cursor += 2u;
		OBJECT(object)->anchor_y = ROM16(offset_cursor);
		OBJECT(object)->parent = current_object;
		offset_cursor += 2u;
		puffs_left--;
	} while (puffs_left != 0u);
}

void lava_steam_puff_update(void)
{
	uint32_t r1 = 0, r8 = 0, depth = 0, ignored = 0, object = 0, random_bits = 0, wrapped = 0;
	uint16_t spawn_arg_2 = 0;

	if (scroll_position >= 0x7E00u)
	{
		delete_current_object();
		return;
	}
	if ((int8_t)OBJECT(current_object)->animation_frame >= 0)
		update_screen_position(&ignored, &r1);
	else
	{
		spawn_arg_2 = OBJECT(current_object)->spawn_arg_2;
		OBJECT(current_object)->spawn_arg_2 = spawn_arg_2 - 1u;
		if ((int16_t)spawn_arg_2 > 1)
			return;
		OBJECT(current_object)->animation_frame = 0u;
		object = LINKED(current_object, OBJECT(current_object)->parent);
		OBJECT(current_object)->x = OBJECT(object)->x;
		OBJECT(current_object)->x_int += OBJECT(current_object)->anchor_x;
		OBJECT(current_object)->y = OBJECT(object)->y;
		OBJECT(current_object)->y_int += OBJECT(current_object)->anchor_y;
		OBJECT(current_object)->z = OBJECT(object)->z;
		project_object(&ignored, &r1);
	}
	animate(&r1, &wrapped, &r8);
	if ((uint8_t)wrapped != 0u)
	{
		next_random(&random_bits, &ignored, &ignored);
		OBJECT(current_object)->spawn_arg_2 = random_bits & 0x3Fu;
		OBJECT(current_object)->animation_frame = 0xFFu;
	}
	depth = (uint16_t)(OBJECT(current_object)->screen_y + 0x50u);
	queue_sprite_at_depth(&ignored, &depth);
}

void ice_block_row_start(void)
{
	uint32_t ignored = 0, object = 0;
	uint16_t blocks_left = 0;

	OBJECT(current_object)->ice_block_row.blocks_left = 0xE;
	OBJECT(current_object)->handler = ice_block_row_start_ADDRESS;
	if (free_object_stack < OBJECT(current_object)->ice_block_row.blocks_left)
		return;
	do
	{
		allocate_object(&ignored, &object);
		OBJECT(object)->sprite_flags = 0x1002u;
		OBJECT(object)->sprite_frame = 0x7498u;
		OBJECT(object)->sprite_palette = 0x9130u;
		OBJECT(object)->handler = ice_block_update_ADDRESS;
		OBJECT(object)->flags = 2u;
		OBJECT(object)->hitbox_size_x = 0x20u;
		OBJECT(object)->hitbox_size_y = 0x5Cu;
		OBJECT(object)->hitbox_height = 0x50u;
		OBJECT(object)->sprite_offset_x = 0;
		OBJECT(object)->sprite_offset_y = 0;
		OBJECT(object)->hitbox_offset_x = 0;
		OBJECT(object)->hitbox_offset_y = 0;
		OBJECT(object)->kind = 0xCu;
		OBJECT(object)->speed_z = 0;
		OBJECT(object)->ice_block.state = 0u;
		OBJECT(object)->x = -0x2B20000;
		OBJECT(object)->y = OBJECT(current_object)->y;
		OBJECT(object)->z = 0x780000;
		OBJECT(current_object)->y_int -= 0x5Cu;
		blocks_left = OBJECT(current_object)->ice_block_row.blocks_left;
		OBJECT(current_object)->ice_block_row.blocks_left = blocks_left - 1u;
	} while ((int16_t)blocks_left > 1);
	delete_current_object();
}

void ice_block_update(void)
{
	uint32_t depth = 0, ignored = 0, margin = 0, object = 0, off_screen = 0, push_speed = 0;

	switch (OBJECT(current_object)->ice_block.state)
	{
	case 0:
	case 3:
		OBJECT(current_object)->ice_block.state++;
		project_object(&ignored, &ignored);
		return;
	case 1:
		margin = 0x50u;
		off_screen = check_left_screen_margin((uint16_t)margin);
		if (off_screen != 0u)
		{
			delete_current_object();
			return;
		}
		if ((OBJECT(current_object)->flags & 4u) == 0u)
		{
			add_to_touchable_list(push_speed);
			update_screen_position(&ignored, &margin);
			depth = (uint16_t)(OBJECT(current_object)->screen_y + 0x20u);
			queue_sprite_at_depth(&ignored, &depth);
			return;
		}
		OBJECT(current_object)->ice_block.state++;
		object = LINKED(current_object, OBJECT(current_object)->touched_by);
		push_speed = 0x10000u;
		if ((int8_t)OBJECT(object)->kind > 2)
			push_speed = (uint32_t)OBJECT(object)->speed_fraction << 8;
		OBJECT(current_object)->speed_x = push_speed;
		add_to_touchable_list(push_speed);
		update_screen_position(&ignored, &margin);
		depth = (uint16_t)(OBJECT(current_object)->screen_y + 0x20u);
		queue_sprite_at_depth(&ignored, &depth);
		return;
	case 2:
		OBJECT(current_object)->x -= OBJECT(current_object)->speed_x;
		if (OBJECT(current_object)->x_int <= -0x2BA)
		{
			OBJECT(current_object)->speed_z += 0x6000u;
			OBJECT(current_object)->z += OBJECT(current_object)->speed_z;
		}
		project_object(&ignored, &ignored);
		if (OBJECT(current_object)->screen_y < 0x120)
		{
			depth = (uint16_t)(OBJECT(current_object)->screen_y + 0x20u);
			queue_sprite_at_depth(&ignored, &depth);
			return;
		}
		delete_current_object();
		return;
	default:
		unknown_code(0x87E2Fu + OBJECT(current_object)->ice_block.state * 2u);
	}
}

void spawn_ice_platform_leader(uint32_t record, uint32_t *p_spawned, uint32_t *p_travel, uint32_t *p_leader)
{
	uint16_t travel = (uint16_t)*p_travel;
	uint32_t leader = *p_leader;
	uint16_t reference_x = 0;
	uint32_t free_count = 0, spawned = 0;

	allocate_object_outside_loop(&free_count, &leader);
	if (free_count == 0u)
	{
		*p_spawned = free_count;
		*p_leader = leader;
		return;
	}
	group_leaders[mem_read8(record) & 3u] = leader;
	record++;
	OBJECT(leader)->ice_platform_leader.end_position = mem_read16(record);
	record += 2u;
	reference_x = mem_read16(record);
	record += 2u;
	travel = (uint16_t)(mem_read16(record) - reference_x);
	OBJECT(leader)->ice_platform_leader.travel = travel;
	OBJECT(leader)->x_int = mem_read16(record);
	OBJECT(leader)->x_fraction = 0u;
	record += 2u;
	OBJECT(leader)->y_int = mem_read16(record);
	OBJECT(leader)->y_fraction = 0u;
	OBJECT(leader)->z_int = mem_read16(record + 2u);
	OBJECT(leader)->z_fraction = 0u;
	OBJECT(leader)->floor_z = OBJECT(leader)->z;
	OBJECT(leader)->handler = ice_platform_leader_move_ADDRESS;
	OBJECT(leader)->speed_x = 0x10000;
	OBJECT(leader)->speed_y = 0;
	OBJECT(leader)->flags = 0u;
	OBJECT(leader)->sprite_offset_x = 0;
	OBJECT(leader)->sprite_offset_y = 0;
	moving_floor_object = leader;
	spawned = 0xFFFFFFFFu;
	*p_spawned = spawned;
	*p_travel = (*p_travel & 0xFFFF0000u) | travel;
	*p_leader = leader;
}

void ice_platform_leader_move(void)
{
	uint8_t sound_id = 0;
	uint32_t r1 = 0, r2 = 0, ignored = 0;

	project_object(&ignored, &r1);
	if (OBJECT(current_object)->ice_platform_leader.end_position <= scroll_position)
	{
		delete_current_object();
		return;
	}
	OBJECT(current_object)->x += OBJECT(current_object)->speed_x;
	OBJECT(current_object)->ice_platform_leader.travel += OBJECT(current_object)->speed_x_int;
	if ((uint16_t)(OBJECT(current_object)->ice_platform_leader.travel + 0x87u) >= 0x10Fu)
	{
		OBJECT(current_object)->ice_platform_leader.travel = 0;
		OBJECT(current_object)->speed_x = 0u - OBJECT(current_object)->speed_x;
		sound_id = 0xFu;
		play_sound(sound_id, &r1, &r2);
	}
}

void ice_platform_piece_update(void)
{
	uint16_t margin = 0;
	uint32_t r1 = 0, depth = 0, ignored = 0, object = 0, off_screen = 0;

	switch (OBJECT(current_object)->ice_platform_piece.state)
	{
	case 0:
	case 3:
		OBJECT(current_object)->ice_platform_piece.state++;
		OBJECT(current_object)->speed_z = 0;
		project_object(&ignored, &ignored);
		if (OBJECT(current_object)->ice_platform_piece.member == 3u)
			OBJECT(current_object)->part_list = ice_platform_piece_parts_2_ADDRESS;
		return;
	case 1:
		margin = 0x60u;
		off_screen = check_left_screen_margin(margin);
		if (off_screen != 0u)
		{
			delete_current_object();
			return;
		}
		if (OBJECT(current_object)->ice_platform_piece.icicle_landed != 0u)
		{
			OBJECT(current_object)->ice_platform_piece.state++;
			OBJECT(current_object)->part_list = ice_platform_piece_parts_2_ADDRESS;
		}
		object = LINKED(current_object, OBJECT(current_object)->parent);
		OBJECT(current_object)->x = OBJECT(object)->x;
		OBJECT(current_object)->y = OBJECT(object)->y;
		OBJECT(current_object)->x_int -= OBJECT(current_object)->ice_platform_piece.member * 0x2Du;
		add_to_platform_list();
		project_object(&ignored, &ignored);
		depth = (uint16_t)(OBJECT(current_object)->screen_y + OBJECT(current_object)->ice_platform_piece.depth_offset - OBJECT(current_object)->sprite_offset_y);
		OBJECT(current_object)->ice_platform_piece.draw_depth = depth;
		queue_sprite_at_depth(&ignored, &depth);
		return;
	case 2:
		off_screen = is_off_screen();
		if (off_screen == 0u)
		{
			OBJECT(current_object)->speed_z += 0x6000u;
			OBJECT(current_object)->z += OBJECT(current_object)->speed_z;
			update_screen_position(&ignored, &r1);
			depth = OBJECT(current_object)->ice_platform_piece.draw_depth;
			queue_sprite_at_depth(&ignored, &depth);
			return;
		}
		delete_current_object();
		return;
	default:
		unknown_code(0x87FB3u + OBJECT(current_object)->ice_platform_piece.state * 3u);
	}
}

void ice_floor_start(void)
{
	uint32_t cubes_left = 0, ignored = 0, object = 0, rows_left = 0;

	if (free_object_stack < 9)
		return;
	ice_row_start_x = OBJECT(current_object)->x;
	ice_cube_floor_block = ice_floor_floors[OBJECT(current_object)->sprite_frame];
	rows_left = 5u;
	do
	{
		cubes_left = 7u;
		do
		{
			allocate_object(&ignored, &object);
			OBJECT(object)->sprite_flags = 0x100Au;
			OBJECT(object)->sprite_frame = 0x7490u;
			OBJECT(object)->sprite_palette = 0x9120u;
			OBJECT(object)->handler = ice_floor_block_update_ADDRESS;
			OBJECT(object)->flags = 0u;
			OBJECT(object)->hitbox_size_x = 0x30u;
			OBJECT(object)->hitbox_size_y = 0x30u;
			OBJECT(object)->hitbox_height = 0x30u;
			OBJECT(object)->sprite_offset_x = 2;
			OBJECT(object)->sprite_offset_y = -0x10;
			OBJECT(object)->hitbox_offset_x = 0;
			OBJECT(object)->hitbox_offset_y = 0;
			OBJECT(object)->speed_z = 0;
			OBJECT(object)->ice_floor_block.state = 0u;
			OBJECT(object)->floor_block = ice_cube_floor_block;
			if ((uint16_t)cubes_left == 8u || (uint16_t)cubes_left == 7u)
				OBJECT(object)->sprite_palette |= 0x2000u;
			OBJECT(object)->x = OBJECT(current_object)->x;
			OBJECT(object)->y = OBJECT(current_object)->y;
			OBJECT(current_object)->x_int += 0x2Eu;
			OBJECT(object)->floor_z_int = mem_read16(floor_blocks + 2u + OBJECT(object)->floor_block + 0x10u);
			OBJECT(object)->floor_z_fraction = 0u;
			OBJECT(object)->z = OBJECT(object)->floor_z;
			cubes_left--;
		} while (cubes_left != 0u);
		OBJECT(current_object)->y_int += 0x2Eu;
		OBJECT(current_object)->x = ice_row_start_x;
		rows_left--;
	} while (rows_left != 0u);
	delete_current_object();
}

void ice_floor_block_update(void)
{
	uint8_t sound_id = 0;
	uint32_t r1 = 0, r2 = 0, depth = 0, ignored = 0, object = 0, off_screen = 0;
	uint8_t shake_timer = 0;
	uint16_t shake_duration = 0;

	switch (OBJECT(current_object)->ice_floor_block.state)
	{
	case 0:
	case 4:
		OBJECT(current_object)->ice_floor_block.state++;
		project_object(&ignored, &ignored);
		return;
	case 1:
		off_screen = check_left_screen();
		if (off_screen != 0u)
		{
			delete_current_object();
			return;
		}
		if ((OBJECT(current_object)->flags & 8u) != 0u)
		{
			object = LINKED(current_object, OBJECT(current_object)->platform);
			if ((int8_t)OBJECT(object)->kind <= 2)
			{
				OBJECT(current_object)->ice_floor_block.state++;
				OBJECT(current_object)->ice_floor_block.shake_offset = 1;
				OBJECT(current_object)->ice_floor_block.shake_timer = 0u;
				OBJECT(current_object)->ice_floor_block.shake_duration = 0x1E;
				sound_id = 0x14u;
				play_sound(sound_id, &r1, &r2);
				if (PLAYER(PLAYER_BLOCK(1) + ((uint32_t)OBJECT(object)->player << 8) * 2u)->invulnerable_timer > 0)
					OBJECT(current_object)->ice_floor_block.shake_duration = 0x12C;
			}
		}
		add_to_platform_list();
		update_screen_position(&ignored, &r1);
		depth = (uint16_t)OBJECT(current_object)->screen_y;
		OBJECT(current_object)->ice_floor_block.draw_depth = depth;
		queue_sprite_at_depth(&ignored, &depth);
		return;
	case 2:
		shake_duration = OBJECT(current_object)->ice_floor_block.shake_duration;
		OBJECT(current_object)->ice_floor_block.shake_duration = shake_duration - 1u;
		if ((int16_t)shake_duration <= 1)
		{
			OBJECT(current_object)->ice_floor_block.state++;
			OBJECT(current_object)->sprite_palette |= 0x2000u;
			sound_id = 0x2Au;
			play_sound(sound_id, &r1, &r2);
		}
		shake_timer = OBJECT(current_object)->ice_floor_block.shake_timer;
		OBJECT(current_object)->ice_floor_block.shake_timer = shake_timer - 1u;
		if ((int8_t)shake_timer <= 1)
		{
			OBJECT(current_object)->ice_floor_block.shake_timer = 2u;
			OBJECT(current_object)->z_int += OBJECT(current_object)->ice_floor_block.shake_offset;
			OBJECT(current_object)->ice_floor_block.shake_offset = 0u - OBJECT(current_object)->ice_floor_block.shake_offset;
		}
		add_to_platform_list();
		update_screen_position(&ignored, &r1);
		depth = (uint16_t)OBJECT(current_object)->screen_y;
		OBJECT(current_object)->ice_floor_block.draw_depth = depth;
		queue_sprite_at_depth(&ignored, &depth);
		return;
	case 3:
		off_screen = is_off_screen();
		if (off_screen != 0u)
		{
			delete_current_object();
			return;
		}
		OBJECT(current_object)->speed_z += 0x6000u;
		OBJECT(current_object)->z += OBJECT(current_object)->speed_z;
		update_screen_position(&ignored, &r1);
		depth = (uint16_t)(((int32_t)((uint32_t)OBJECT(current_object)->projected_y + OBJECT(current_object)->floor_z) >> 16) - view_y + OBJECT(current_object)->sprite_offset_y);
		OBJECT(current_object)->ice_floor_block.draw_depth = depth;
		queue_sprite_at_depth(&ignored, &depth);
		return;
	default:
		unknown_code(0x8816Cu + OBJECT(current_object)->ice_floor_block.state * 3u);
	}
}

void icicle_dropper_start(void)
{
	uint32_t dropper_record = 0;

	if (icicle_dropper_ready == 0u)
		return;
	icicle_dropper_ready = 0u;
	dropper_record = icicle_dropper_records_ADDRESS + icicle_dropper_records[OBJECT(current_object)->sprite_frame];
	OBJECT(current_object)->x_int = ROM16(dropper_record);
	OBJECT(current_object)->x_fraction = 0u;
	dropper_record += 2u;
	OBJECT(current_object)->y_int = ROM16(dropper_record);
	OBJECT(current_object)->y_fraction = 0u;
	dropper_record += 2u;
	OBJECT(current_object)->z_int = ROM16(dropper_record);
	OBJECT(current_object)->z_fraction = 0u;
	OBJECT(current_object)->floor_z = OBJECT(current_object)->z;
	dropper_record += 2u;
	OBJECT(current_object)->direction = ROM16(dropper_record);
	dropper_record += 2u;
	OBJECT(current_object)->var_96 = ROM16(dropper_record);
	OBJECT(current_object)->icicle_dropper.path = icicle_dropper_scripts_ADDRESS + ROM16(dropper_record + 2u);
	OBJECT(current_object)->handler = icicle_dropper_move_ADDRESS;
	OBJECT(current_object)->sprite_offset_x = 0;
	OBJECT(current_object)->sprite_offset_y = 0;
	OBJECT(current_object)->icicle_dropper.holding_back = 0u;
	OBJECT(current_object)->icicle_dropper.platform_offset = 0;
	OBJECT(current_object)->icicle_dropper.travel = 0x180;
	OBJECT(current_object)->icicle_dropper.wait_timer = 1;
	OBJECT(current_object)->flags = 0u;
	current_icicle_dropper = current_object;
}

void icicle_dropper_move(void)
{
	uint16_t move_speed = 0, row_spacing = 0;
	uint32_t r3 = 0, r11 = 0, block_offset = 0, direction = 0, floor_slope = 0, hold_threshold = 0, ignored = 0, path_cursor = 0, slope_factor = 0, step = 0;
	uint8_t hold_timer = 0;
	uint16_t wait_timer = 0, record_scroll = 0;
	uint32_t flags = 0;

	flags = OBJECT(current_object)->flags;
	OBJECT(current_object)->flags = flags | 0x80000000u;
	if ((flags & 0x80000000u) != 0u)
	{
		if (OBJECT(current_object)->icicle_dropper.holding_back != 0u)
			goto icicle_dropper_move_step;
		if (no_player_in_play != 0u || player_mishap != 0u || player_dropping_in != 0u)
		{
			OBJECT(current_object)->icicle_dropper.holding_back = 0xFFu;
			OBJECT(current_object)->icicle_dropper.saved_base_speed = OBJECT(current_object)->icicle_dropper.base_speed;
			OBJECT(current_object)->icicle_dropper.base_speed = 0;
			OBJECT(current_object)->icicle_dropper.hold_start_position = scroll_position;
			OBJECT(current_object)->icicle_dropper.hold_timer = 0xFFu;
			OBJECT(current_object)->icicle_dropper.saved_threshold = OBJECT(current_object)->icicle_dropper.screen_threshold;
			hold_threshold = 0xFFE0u;
			if (OBJECT(current_object)->direction < 0x100 || OBJECT(current_object)->direction > 0x200)
				hold_threshold = 0x160u;
			OBJECT(current_object)->icicle_dropper.screen_threshold = hold_threshold;
			goto icicle_dropper_move_step;
		}
	}
	wait_timer = OBJECT(current_object)->icicle_dropper.wait_timer;
	OBJECT(current_object)->icicle_dropper.wait_timer = wait_timer - 1u;
	if ((int16_t)wait_timer > 1)
	{
	icicle_dropper_move_step:
		if (OBJECT(current_object)->icicle_dropper.rides_platform == 0u)
		{
			find_floor(&direction, &hold_threshold, &block_offset, &r3, &r11);
			OBJECT(current_object)->z = OBJECT(current_object)->floor_z;
		}
		if (OBJECT(current_object)->icicle_dropper.rides_platform != 0u)
		{
			OBJECT(current_object)->x = OBJECT(LINKED(current_object, moving_floor_object))->x;
			OBJECT(current_object)->x -= OBJECT(current_object)->icicle_dropper.platform_offset;
			OBJECT(current_object)->x_int += 0x2Du;
		}
		project_object(&ignored, &ignored);
		if (OBJECT(current_object)->icicle_dropper.holding_back != 0u)
		{
			if ((int16_t)(scroll_position - OBJECT(current_object)->icicle_dropper.hold_start_position) >= 0x300 || (OBJECT(current_object)->icicle_dropper.last_dropper != 0u && AS32(players_finished[0]) == 0xFFFFFFu))
				goto icicle_dropper_stop_holding_back;
			if (no_player_in_play != 0u)
				OBJECT(current_object)->icicle_dropper.hold_timer = 0xFFu;
			hold_timer = OBJECT(current_object)->icicle_dropper.hold_timer;
			OBJECT(current_object)->icicle_dropper.hold_timer = hold_timer - 1u;
			if (hold_timer == 0u)
				goto icicle_dropper_stop_holding_back;
		}
		direction = OBJECT(current_object)->direction;
		switch (direction >> 8)
		{
		case 0:
		case 3:
			if (OBJECT(current_object)->screen_x <= OBJECT(current_object)->icicle_dropper.screen_threshold)
			{
			icicle_dropper_use_base_speed:
				move_speed = (uint16_t)OBJECT(current_object)->icicle_dropper.base_speed;
			}
			else
			{
			icicle_dropper_catch_up:
				OBJECT(current_object)->icicle_dropper.wait_timer = 0;
				if (OBJECT(current_object)->icicle_dropper.holding_back != 0u)
				{
				icicle_dropper_stop_holding_back:
					OBJECT(current_object)->icicle_dropper.holding_back = 0u;
					OBJECT(current_object)->icicle_dropper.screen_threshold = OBJECT(current_object)->icicle_dropper.saved_threshold;
					OBJECT(current_object)->icicle_dropper.base_speed = OBJECT(current_object)->icicle_dropper.saved_base_speed;
				}
				slope_factor = 0u;
				if ((OBJECT(current_object)->floor_slope | OBJECT(current_object)->floor_type) != 0u)
				{
					floor_slope = OBJECT(current_object)->floor_slope;
					slope_factor = floor_slope_factors[(uint16_t)(floor_slope * 2u)];
				}
				r3 = compute_camera_slope_speed(r3, slope_factor);
				move_speed = (uint16_t)((uint32_t)camera_slope_speed_int * OBJECT(current_object)->icicle_dropper.catch_up_factor) >> 8;
				if ((int16_t)move_speed <= OBJECT(current_object)->icicle_dropper.base_speed)
					goto icicle_dropper_use_base_speed;
			}
			break;
		case 1:
		case 2:
		case 4:
			if (OBJECT(current_object)->icicle_dropper.screen_threshold <= OBJECT(current_object)->screen_x)
				goto icicle_dropper_use_base_speed;
			goto icicle_dropper_catch_up;
		default:
			unknown_code(0x884C0u + (direction >> 8) * 2u);
		}
		chaser_speed = move_speed;
		if (OBJECT(current_object)->icicle_dropper.wait_timer <= 0)
		{
			if (OBJECT(current_object)->icicle_dropper.last_dropper != 0u && AS32(players_finished[0]) == 0xFFFFFFu)
				chaser_speed = 0x400;
			direction = OBJECT(current_object)->direction;
			switch (direction >> 8)
			{
			case 0:
				step = chaser_speed;
				OBJECT(current_object)->icicle_dropper.travel += step;
				step = step << 12;
				OBJECT(current_object)->icicle_dropper.platform_offset += step;
				if (OBJECT(current_object)->icicle_dropper.rides_platform == 0u)
					OBJECT(current_object)->y -= step;
				break;
			case 1:
				step = chaser_speed;
				OBJECT(current_object)->icicle_dropper.travel += step;
				step = step << 12;
				OBJECT(current_object)->icicle_dropper.platform_offset += step;
				if (OBJECT(current_object)->icicle_dropper.rides_platform == 0u)
					OBJECT(current_object)->x += step;
				break;
			case 2:
			case 4:
				step = chaser_speed;
				OBJECT(current_object)->icicle_dropper.travel += step;
				step = step << 12;
				OBJECT(current_object)->icicle_dropper.platform_offset += step;
				if (OBJECT(current_object)->icicle_dropper.rides_platform == 0u)
					OBJECT(current_object)->y += step;
				break;
			case 3:
				step = chaser_speed;
				OBJECT(current_object)->icicle_dropper.travel += step;
				step = step << 12;
				OBJECT(current_object)->icicle_dropper.platform_offset += step;
				if (OBJECT(current_object)->icicle_dropper.rides_platform == 0u)
					OBJECT(current_object)->x -= step;
				break;
			default:
				unknown_code(0x88572u + (direction >> 8) * 2u);
			}
		}
		if ((OBJECT(current_object)->flags & 1u) != 0u)
			return;
		row_spacing = 0x400u;
		if (OBJECT(current_object)->icicle_dropper.rides_platform != 0u)
			row_spacing = 0x300u;
		if ((int16_t)row_spacing <= OBJECT(current_object)->icicle_dropper.travel)
		{
			OBJECT(current_object)->icicle_dropper.travel -= row_spacing;
			drop_icicle_row(r3, r11);
		}
		return;
	}
	OBJECT(current_object)->icicle_dropper.wait_timer = 0;
	path_cursor = OBJECT(current_object)->icicle_dropper.path;
	if (OBJECT(current_object)->icicle_dropper.rides_platform != 0u)
	{
		if ((int16_t)mem_read16(path_cursor - 2u) <= OBJECT(current_object)->icicle_dropper.platform_offset_int)
		{
			icicle_dropper_ready = 0xFFu;
			if (OBJECT(current_object)->icicle_dropper.last_dropper != 0u)
			{
				OBJECT(current_object)->icicle_dropper.finish_timer = 3u;
				OBJECT(current_object)->handler = icicle_dropper_finish_ADDRESS;
				icicle_dropper_finish();
				return;
			}
			delete_current_object();
			return;
		}
		goto icicle_dropper_check_scroll_position;
	}
	direction = OBJECT(current_object)->direction;
	switch (direction >> 8)
	{
	case 0:
		if (OBJECT(current_object)->y_int <= (int16_t)mem_read16(path_cursor - 2u))
		{
		icicle_dropper_coordinate_reached:
			path_cursor += 2u;
		icicle_dropper_load_record:
			if ((int16_t)mem_read16(path_cursor + 2u) >= 0)
			{
				OBJECT(current_object)->icicle_dropper.screen_threshold = mem_read16(path_cursor);
				path_cursor += 2u;
				OBJECT(current_object)->icicle_dropper.catch_up_factor = mem_read16(path_cursor);
				path_cursor += 2u;
				OBJECT(current_object)->icicle_dropper.base_speed = mem_read16(path_cursor);
				path_cursor += 2u;
				OBJECT(current_object)->icicle_dropper.wait_timer = mem_read16(path_cursor);
				OBJECT(current_object)->icicle_dropper.path = path_cursor + 4u;
				goto icicle_dropper_move_step;
			}
			icicle_dropper_ready = 0xFFu;
			if (OBJECT(current_object)->icicle_dropper.last_dropper != 0u)
			{
				OBJECT(current_object)->icicle_dropper.finish_timer = 3u;
				OBJECT(current_object)->handler = icicle_dropper_finish_ADDRESS;
				icicle_dropper_finish();
				return;
			}
			delete_current_object();
			return;
		}
	icicle_dropper_check_scroll_position:
		record_scroll = mem_read16(path_cursor);
		path_cursor += 2u;
		if (scroll_position < record_scroll)
			goto icicle_dropper_move_step;
		goto icicle_dropper_load_record;
	case 1:
		if ((int16_t)mem_read16(path_cursor - 2u) <= OBJECT(current_object)->x_int)
			goto icicle_dropper_coordinate_reached;
		goto icicle_dropper_check_scroll_position;
	case 2:
	case 4:
		if ((int16_t)mem_read16(path_cursor - 2u) <= OBJECT(current_object)->y_int)
			goto icicle_dropper_coordinate_reached;
		goto icicle_dropper_check_scroll_position;
	case 3:
		if ((int16_t)mem_read16(path_cursor - 2u) < OBJECT(current_object)->x_int)
			goto icicle_dropper_check_scroll_position;
		goto icicle_dropper_coordinate_reached;
	default:
		unknown_code(0x883C6u + (direction >> 8) * 2u);
	}
}

void icicle_dropper_finish(void)
{
	uint16_t finish_timer = 0;

	if ((int8_t)goal_unlocked < 3)
		return;
	finish_timer = OBJECT(current_object)->icicle_dropper.finish_timer;
	OBJECT(current_object)->icicle_dropper.finish_timer = finish_timer - 1u;
	if ((int16_t)finish_timer > 1)
		return;
	goal_unlocked = 0xFFu;
	delete_current_object();
}

void drop_icicle_row(uint32_t random_value, uint32_t block)
{
	uint16_t end_coordinate = 0, line_coordinate = 0;
	uint32_t direction = 0, free_count = 0, icicles_left = 0, ignored = 0, object = 0;

	icicles_left = 3u;
	icicle_row_offset = 0x20u;
	do
	{
		allocate_object(&free_count, &block);
		if (free_count == 0u)
			break;
		OBJECT(block)->sprite_flags = 0x1004u;
		OBJECT(block)->sprite_frame = 0x74A0u;
		OBJECT(block)->sprite_palette = 0x9140u;
		OBJECT(block)->handler = icicle_update_ADDRESS;
		OBJECT(block)->hitbox_size_x = 0x28u;
		AS8(OBJECT(block)->hitbox_size_y) = 0u;
		AT8(OBJECT(block)->hitbox_size_y, 1) = 0xF0u;
		OBJECT(block)->sprite_offset_x = 0;
		OBJECT(block)->sprite_offset_y = 0;
		OBJECT(block)->hitbox_height = 0xA0u;
		OBJECT(block)->flags = 0x40000000u;
		OBJECT(block)->kind = 0x28u;
		OBJECT(block)->speed_z = 1;
		OBJECT(block)->direction = OBJECT(current_object)->direction;
		OBJECT(block)->speed = 0;
		OBJECT(block)->var_96 = OBJECT(current_object)->var_96;
		AS8(OBJECT(block)->var_82) = 0u;
		if (AS8(OBJECT(block)->var_96) != 0u)
			AS8(OBJECT(block)->var_82) = 4u;
		OBJECT(block)->x = OBJECT(current_object)->x;
		OBJECT(block)->y = OBJECT(current_object)->y;
		OBJECT(block)->z = OBJECT(current_object)->z;
		OBJECT(block)->z_int -= 0xE0u;
		OBJECT(block)->floor_z = OBJECT(current_object)->z;
		direction = OBJECT(current_object)->direction;
		switch (direction >> 8)
		{
		case 0:
		case 2:
		case 4:
		case 5:
			OBJECT(block)->x_int += icicle_row_offset;
			break;
		case 1:
		case 3:
			OBJECT(block)->y_int += icicle_row_offset;
			break;
		default:
			unknown_code(0x886F7u + (direction >> 8) * 2u);
		}
		icicle_row_offset += 0x48u;
		if (AS8(OBJECT(block)->var_96) == 0u)
		{
			end_coordinate = mem_read16(OBJECT(current_object)->drop_icicle_row.path - 2u);
			line_coordinate = (uint16_t)end_coordinate;
			direction = OBJECT(current_object)->direction;
			switch (direction >> 8)
			{
			case 0:
				if ((int16_t)(end_coordinate - OBJECT(current_object)->y_int) >= -0x40)
				{
					OBJECT(block)->y_int = line_coordinate + 0x20u;
				drop_icicle_row_line_up:
					OBJECT(block)->flags |= 1u;
					OBJECT(current_object)->flags |= 1u;
					OBJECT(block)->var_84 = mem_read16(OBJECT(current_object)->drop_icicle_row.path - 2u);
				}
				break;
			case 1:
				if ((int16_t)(end_coordinate - OBJECT(current_object)->x_int) <= 0x40)
				{
					OBJECT(block)->x_int = line_coordinate - 0x20u;
					goto drop_icicle_row_line_up;
				}
				break;
			case 2:
			case 4:
				if ((int16_t)(end_coordinate - OBJECT(current_object)->y_int) <= 0x40)
				{
					OBJECT(block)->y_int = line_coordinate - 0x20u;
					goto drop_icicle_row_line_up;
				}
				break;
			case 3:
				if ((int16_t)(end_coordinate - OBJECT(current_object)->x_int) >= -0x40)
				{
					OBJECT(block)->x_int = line_coordinate + 0x20u;
					goto drop_icicle_row_line_up;
				}
				break;
			default:
				unknown_code(0x8873Cu + (direction >> 8) * 2u);
			}
		}
		next_random(&random_value, &ignored, &ignored);
		random_value = (random_value & ~0xFFFFu) | (random_value & 0x1Fu);
		random_value = (random_value & ~0xFFFFu) | (uint16_t)(random_value + 0x3Cu);
		AS16(OBJECT(block)->var_80) = random_value;
		object = block;
		allocate_object(&free_count, &block);
		if (free_count == 0u)
			break;
		OBJECT(block)->sprite_flags = 0u;
		OBJECT(block)->sprite_palette = 0x1FFEu;
		OBJECT(block)->sprite_frame = 0x5D90u;
		OBJECT(block)->sprite_scale = 1u;
		OBJECT(block)->handler = icicle_shadow_follow_ADDRESS;
		OBJECT(block)->sprite_offset_x = 0;
		OBJECT(block)->sprite_offset_y = 0;
		OBJECT(block)->flags = 0u;
		OBJECT(block)->speed_z = 0;
		OBJECT(block)->var_86 = 0u;
		OBJECT(block)->parent = object;
		OBJECT(block)->x = OBJECT(object)->x;
		OBJECT(block)->y = OBJECT(object)->y;
		OBJECT(block)->z = OBJECT(current_object)->z;
		OBJECT(block)->floor_z = OBJECT(current_object)->z;
		if (OBJECT(current_object)->drop_icicle_row.rides_platform != 0u)
			OBJECT(current_object)->sprite_palette = 0x87A0u;
		icicles_left--;
	} while (icicles_left != 0u);
}

void icicle_update(void)
{
	uint8_t sound_id = 0;
	uint16_t child = 0, margin_y = 0, projected_x = 0, swing_limit = 0;
	uint32_t r1 = 0, r2 = 0, r7 = 0, r8 = 0, depth = 0, direction = 0, floor_z = 0, ignored = 0, landing_offset = 0, margin_x = 0, object = 0, object_2 = 0, object_3 = 0, off_screen = 0, parts_left = 0, platform_shift = 0, projected_y = 0;
	uint16_t timer = 0, timer_2 = 0;

	switch (OBJECT(current_object)->icicle_update.state)
	{
	case 0:
	case 8:
		OBJECT(current_object)->icicle_update.state++;
		project_object(&ignored, &ignored);
		return;
	case 1:
		margin_x = 0x50u;
		margin_y = 0x140u;
		off_screen = is_off_screen_margin((uint16_t)margin_x, margin_y);
		if (off_screen != 0u)
		{
			delete_current_object();
			return;
		}
		timer = OBJECT(current_object)->icicle_update.timer;
		OBJECT(current_object)->icicle_update.timer = timer - 1u;
		if ((int16_t)timer <= 1)
		{
			OBJECT(current_object)->icicle_update.timer = 0;
			OBJECT(current_object)->speed_z += 0x6000u;
			OBJECT(current_object)->z += OBJECT(current_object)->speed_z;
			if (OBJECT(current_object)->floor_z_int <= OBJECT(current_object)->z_int)
				OBJECT(current_object)->icicle_update.state++;
		}
		update_screen_position(&ignored, &margin_x);
		margin_x = queue_sprite(margin_x);
		return;
	case 2:
		if (free_object_stack >= 2 && free_object_stack_2[0] >= 2)
		{
			if ((OBJECT(current_object)->flags & 1u) == 0u)
			{
				allocate_object(&ignored, &object_2);
				OBJECT(object_2)->sprite_flags = OBJECT(current_object)->sprite_flags;
				OBJECT(object_2)->sprite_frame = OBJECT(current_object)->sprite_frame;
				OBJECT(object_2)->sprite_palette = OBJECT(current_object)->sprite_palette;
				OBJECT(object_2)->sprite_palette &= 0x1FFFu;
				OBJECT(object_2)->sprite_palette |= 0x2000u;
				OBJECT(object_2)->handler = icicle_buried_part_draw_ADDRESS;
				OBJECT(object_2)->x = OBJECT(current_object)->x;
				OBJECT(object_2)->y = OBJECT(current_object)->y;
				OBJECT(object_2)->floor_z = OBJECT(current_object)->floor_z;
				OBJECT(object_2)->parent = current_object;
				OBJECT(current_object)->child = object_2;
			}
			else
				object_2 = current_object;
			parts_left = 2u;
			do
			{
				object_3 = object_2;
				allocate_object_2(&ignored, &r2, &object_2);
				object = object_2 + 2u;
				OBJECT(object - 0x2u)->sprite_flags = 0x5000u;
				object += 2u;
				OBJECT(object - 0x4u)->sprite_frame = 0xE0u;
				object += 2u;
				OBJECT(object - 0x6u)->sprite_palette = 0u;
				object += 2u;
				OBJECT(object - 0x8u)->sprite_scale = 0x140u;
				object += 2u;
				OBJECT(object - 0xAu)->sprite_layer = 0u;
				object += 2u;
				OBJECT(object - 0xCu)->screen_y = 0;
				object += 2u;
				OBJECT(object - 0xEu)->screen_x = 0;
				OBJECT(object - 0xEu)->hitbox_size_x = 0u;
				OBJECT(object_2)->parent = object_3;
				OBJECT(object_3)->child = object_2;
				parts_left--;
			} while (parts_left != 0u);
			allocate_object(&ignored, &object_2);
			OBJECT(object_2)->sprite_flags = 0x1004u;
			OBJECT(object_2)->sprite_palette = 0x9150u;
			OBJECT(object_2)->handler = icicle_cracks_spread_ADDRESS;
			OBJECT(object_2)->x = OBJECT(current_object)->x;
			OBJECT(object_2)->y = OBJECT(current_object)->y;
			OBJECT(object_2)->floor_z = OBJECT(current_object)->floor_z;
			OBJECT(object_2)->z = OBJECT(object_2)->floor_z;
			OBJECT(object_2)->flags = 0u;
			OBJECT(object_2)->sprite_offset_x = 0;
			OBJECT(object_2)->sprite_offset_y = 0;
			OBJECT(object_2)->parent = current_object;
			OBJECT(object_2)->animation = icicle_crack_animation_ADDRESS;
			OBJECT(object_2)->animation_frame = 0xFFu;
			OBJECT(object_2)->animation_speed = 0u;
			OBJECT(object_2)->animation_timer = 0;
			OBJECT(current_object)->icicle_update.state++;
			if ((OBJECT(current_object)->flags & 1u) != 0u)
				OBJECT(current_object)->icicle_update.state = 7u;
		}
		if ((uint16_t)(OBJECT(current_object)->screen_x + 8u) < 0x150u)
		{
			sound_id = 0x29u;
			play_sound(sound_id, &r1, &r2);
		}
		update_screen_position(&ignored, &r1);
		add_to_touchable_list(ignored);
		depth = (uint16_t)(((int32_t)((uint32_t)OBJECT(current_object)->projected_y + OBJECT(current_object)->floor_z) >> 16) - view_y);
		OBJECT(current_object)->icicle_update.draw_depth = depth;
		queue_sprite_at_depth(&ignored, &depth);
		return;
	case 3:
		if ((uint32_t)OBJECT(current_object)->speed_z != 0u)
		{
			OBJECT(current_object)->z += OBJECT(current_object)->speed_z;
			if ((int16_t)(OBJECT(current_object)->z_int - OBJECT(current_object)->floor_z_int) >= 0x70)
			{
				OBJECT(current_object)->speed_z = 0;
				set_icicle_wall_box();
			}
		}
		if (map_screen_active != 0u)
		{
			delete_current_object();
			return;
		}
		margin_x = 0x30u;
		margin_y = 0xA0u;
		off_screen = is_off_screen_margin((uint16_t)margin_x, margin_y);
		if (off_screen != 0u)
		{
			delete_current_object();
			return;
		}
		update_screen_position(&ignored, &margin_x);
		add_to_touchable_list(ignored);
		depth = (uint16_t)(((int32_t)((uint32_t)OBJECT(current_object)->projected_y + OBJECT(current_object)->floor_z) >> 16) - view_y - 0x10u);
		if ((int16_t)depth < -0x10)
			depth = 0xFFF0u;
		if ((int16_t)depth > 0xEE)
			depth = 0xEEu;
		OBJECT(current_object)->icicle_update.draw_depth = depth;
		queue_sprite_at_depth(&ignored, &depth);
		return;
	case 4:
		OBJECT(current_object)->icicle_update.state++;
		object = LINKED(current_object, moving_floor_object);
		landing_offset = OBJECT(object)->speed_x * ((uint16_t)OBJECT(current_object)->icicle_update.timer + 0x19u);
		platform_shift = landing_offset;
		landing_offset = (uint16_t)(((int32_t)landing_offset >> 16) + OBJECT(object)->var_8C);
		swing_limit = 0x87u;
		if ((int16_t)landing_offset > 0x87)
			OBJECT(current_object)->x += platform_shift - ((uint32_t)(uint16_t)((landing_offset - swing_limit) * 2u) << 16);
		else
		{
			swing_limit = (uint16_t)(0u - swing_limit);
			if ((int16_t)landing_offset < (int16_t)swing_limit)
			{
				OBJECT(current_object)->x += platform_shift - ((uint32_t)(uint16_t)((landing_offset - swing_limit) * 2u) << 16);
				project_object(&ignored, &ignored);
				return;
			}
			OBJECT(current_object)->x += platform_shift;
		}
		project_object(&ignored, &ignored);
		return;
	case 5:
		if (OBJECT(current_object)->z_int >= 0x320)
		{
			delete_current_object();
			return;
		}
		timer_2 = OBJECT(current_object)->icicle_update.timer;
		OBJECT(current_object)->icicle_update.timer = timer_2 - 1u;
		if ((int16_t)timer_2 <= 1)
		{
			OBJECT(current_object)->icicle_update.timer = 0;
			OBJECT(current_object)->speed_z += 0x6000u;
			OBJECT(current_object)->z += OBJECT(current_object)->speed_z;
			if ((OBJECT(current_object)->flags & 8u) != 0u)
			{
				object = LINKED(current_object, OBJECT(current_object)->platform);
				if (OBJECT(object)->z_int <= OBJECT(current_object)->z_int)
				{
					OBJECT(current_object)->icicle_update.state++;
					OBJECT(current_object)->speed_z = 0;
					OBJECT(current_object)->sprite_palette |= 0x2000u;
					AS8(OBJECT(object)->spawn_arg_1) = 0xFFu;
				}
			}
		}
		update_screen_position(&ignored, &r1);
		add_to_rider_list();
		ignored = queue_sprite_at_floor();
		return;
	case 6:
		off_screen = is_off_screen();
		if (off_screen != 0u)
		{
			delete_current_object();
			return;
		}
		OBJECT(current_object)->speed_z += 0x6000u;
		OBJECT(current_object)->z += OBJECT(current_object)->speed_z;
		update_screen_position(&ignored, &r1);
		depth = (uint16_t)(OBJECT(LINKED(current_object, OBJECT(current_object)->platform))->var_88 + 1u);
		OBJECT(current_object)->icicle_update.draw_depth = depth;
		queue_sprite_at_depth(&ignored, &depth);
		return;
	case 7:
		if ((uint32_t)OBJECT(current_object)->speed_z != 0u)
		{
			OBJECT(current_object)->z += OBJECT(current_object)->speed_z;
			if ((int16_t)(OBJECT(current_object)->z_int - OBJECT(current_object)->floor_z_int) >= 0x70)
			{
				OBJECT(current_object)->speed_z = 0;
				if (OBJECT(current_object)->icicle_update.counts_goal != 0u)
					goal_unlocked++;
				set_icicle_wall_box();
				direction = OBJECT(current_object)->direction;
				switch (direction >> 8)
				{
				case 0:
				case 4:
					OBJECT(current_object)->y_int = OBJECT(current_object)->icicle_update.line_coordinate;
					OBJECT(current_object)->hitbox_offset_y = 0;
					break;
				case 1:
					OBJECT(current_object)->x_int = OBJECT(current_object)->icicle_update.line_coordinate;
					break;
				case 2:
					OBJECT(current_object)->y_int = OBJECT(current_object)->icicle_update.line_coordinate;
					break;
				case 3:
					OBJECT(current_object)->x_int = OBJECT(current_object)->icicle_update.line_coordinate;
					OBJECT(current_object)->hitbox_offset_x = 0;
					break;
				default:
					unknown_code(0x88BCBu + (direction >> 8) * 2u);
				}
			}
		}
		if (map_screen_active != 0u)
		{
			child = OBJECT(current_object)->child;
			free_object_2(child);
			child = OBJECT(LINKED(current_object, OBJECT(current_object)->child))->child;
			free_object_2(child);
			delete_current_object();
			return;
		}
		margin_x = 0x28u;
		margin_y = 0xA0u;
		r8 = is_off_screen_margin((uint16_t)margin_x, margin_y);
		if (r8 != 0u)
		{
			child = OBJECT(current_object)->child;
			free_object_2(child);
			child = OBJECT(LINKED(current_object, OBJECT(current_object)->child))->child;
			free_object_2(child);
			delete_current_object();
			return;
		}
		update_screen_position(&ignored, &margin_x);
		add_to_touchable_list(ignored);
		OBJECT(current_object)->icicle_update.draw_depth = ((int32_t)((uint32_t)OBJECT(current_object)->projected_y + OBJECT(current_object)->floor_z) >> 16) - view_y;
		depth = OBJECT(current_object)->icicle_update.draw_depth;
		current_object = LINKED(current_object, OBJECT(current_object)->child);
		current_object = LINKED(current_object, OBJECT(current_object)->child);
		queue_sprite_at_depth(&ignored, &depth);
		current_object = object_cursor;
		depth = OBJECT(current_object)->icicle_update.draw_depth;
		queue_sprite_at_depth(&ignored, &depth);
		projected_x = (uint16_t)OBJECT(current_object)->projected_x;
		projected_y = OBJECT(current_object)->projected_y;
		floor_z = OBJECT(current_object)->floor_z;
		point_to_screen(projected_x, floor_z, &projected_y, &r7, &r8);
		current_object = LINKED(current_object, OBJECT(current_object)->child);
		OBJECT(current_object)->sprite_frame = r8;
		depth = OBJECT(object_cursor)->icicle_update.draw_depth;
		queue_sprite_at_depth(&ignored, &depth);
		return;
	default:
		unknown_code(0x88840u + OBJECT(current_object)->icicle_update.state * 3u);
	}
}

void icicle_buried_part_draw(void)
{
	uint16_t child = 0, margin_x = 0, margin_y = 0;
	uint32_t depth = 0, floor_z = 0, ignored = 0, object = 0, off_screen = 0, part_depth = 0, point_x = 0, point_y = 0;

	object = LINKED(current_object, OBJECT(current_object)->parent);
	OBJECT(current_object)->screen_x = OBJECT(object)->screen_x;
	OBJECT(current_object)->screen_y = OBJECT(object)->screen_y;
	if (map_screen_active != 0u)
	{
		child = OBJECT(current_object)->child;
		free_object_2(child);
		child = OBJECT(LINKED(current_object, OBJECT(current_object)->child))->child;
		free_object_2(child);
		delete_current_object();
		return;
	}
	margin_x = 0x28u;
	margin_y = 0xA0u;
	off_screen = is_off_screen_margin(margin_x, margin_y);
	if (off_screen != 0u)
	{
		child = OBJECT(current_object)->child;
		free_object_2(child);
		child = OBJECT(LINKED(current_object, OBJECT(current_object)->child))->child;
		free_object_2(child);
		delete_current_object();
		return;
	}
	part_depth = (uint16_t)(OBJECT(LINKED(current_object, OBJECT(current_object)->parent))->var_88 + 1u);
	buried_part_depth = part_depth;
	current_object = LINKED(current_object, OBJECT(current_object)->child);
	current_object = LINKED(current_object, OBJECT(current_object)->child);
	queue_sprite_at_depth(&ignored, &part_depth);
	current_object = object_cursor;
	part_depth = buried_part_depth;
	queue_sprite_at_depth(&ignored, &part_depth);
	point_x = OBJECT(current_object)->x;
	point_y = OBJECT(current_object)->y;
	floor_z = OBJECT(current_object)->floor_z;
	project_point(floor_z, &ignored, &point_x, &point_y);
	current_object = LINKED(current_object, OBJECT(current_object)->child);
	OBJECT(current_object)->sprite_flags = (OBJECT(current_object)->sprite_flags & ~0xFFFu) | (point_y & 0xFFFu);
	depth = buried_part_depth;
	queue_sprite_at_depth(&ignored, &depth);
}

void icicle_cracks_spread(void)
{
	uint32_t r1 = 0, depth = 0, ignored = 0, object = 0, off_screen = 0;
	uint32_t flags = 0;

	flags = OBJECT(current_object)->flags;
	OBJECT(current_object)->flags = flags | 1u;
	if ((flags & 1u) == 0u)
	{
		OBJECT(current_object)->icicle_cracks.next_depth = 0;
		project_object(&ignored, &ignored);
		OBJECT(current_object)->anchor_x = OBJECT(current_object)->projected_x;
		OBJECT(current_object)->anchor_y = OBJECT(current_object)->projected_y_int;
		return;
	}
	if (map_screen_active != 0u)
	{
		delete_current_object();
		return;
	}
	off_screen = is_off_screen();
	if (off_screen != 0u)
	{
		delete_current_object();
		return;
	}
	object = LINKED(current_object, OBJECT(current_object)->parent);
	if (OBJECT(current_object)->icicle_cracks.next_depth <= (int16_t)(OBJECT(object)->z_int - OBJECT(object)->floor_z_int))
	{
		animate_around_anchor();
		OBJECT(current_object)->icicle_cracks.next_depth += 0x1Cu;
	}
	update_screen_position(&ignored, &r1);
	depth = (uint16_t)(OBJECT(LINKED(current_object, OBJECT(current_object)->parent))->var_88 + 2u);
	queue_sprite_at_depth(&ignored, &depth);
}

void icicle_shadow_follow(void)
{
	uint32_t depth = 0, ignored = 0, object = 0, off_screen = 0;
	uint8_t blink_timer = 0;

	project_object(&ignored, &ignored);
	off_screen = is_off_screen();
	if (off_screen != 0u)
	{
		delete_current_object();
		return;
	}
	object = LINKED(current_object, OBJECT(current_object)->parent);
	if ((AS8(OBJECT(object)->var_82) == 3u || AS8(OBJECT(object)->var_82) == 7u) && (uint32_t)OBJECT(object)->speed_z == 0u)
	{
		delete_current_object();
		return;
	}
	if (AS8(OBJECT(object)->var_96) != 0u)
	{
		OBJECT(current_object)->x += OBJECT(LINKED(current_object, moving_floor_object))->speed_x;
		if (AS8(OBJECT(object)->var_82) == 6u)
		{
			OBJECT(current_object)->speed_z += 0x6000u;
			OBJECT(current_object)->z += OBJECT(current_object)->speed_z;
			OBJECT(current_object)->sprite_palette |= 0x2000u;
		}
		OBJECT(current_object)->flags &= ~0x80000u;
		blink_timer = OBJECT(current_object)->icicle_shadow.blink_timer;
		OBJECT(current_object)->icicle_shadow.blink_timer = blink_timer - 1u;
		if ((int8_t)blink_timer <= 1)
		{
			OBJECT(current_object)->var_86 = 2u;
			OBJECT(current_object)->flags |= 0x80000u;
		}
	}
	if ((int16_t)AS16(OBJECT(object)->var_80) <= 0x32 && (int16_t)OBJECT(current_object)->sprite_scale < 0x21C)
		OBJECT(current_object)->sprite_scale += 9u;
	if ((OBJECT(current_object)->flags & 0x80000u) != 0u)
		return;
	object = LINKED(current_object, OBJECT(current_object)->parent);
	if (AS8(OBJECT(object)->var_82) == 6u)
		depth = OBJECT(object)->var_88;
	else
		depth = (uint16_t)(((int32_t)((uint32_t)OBJECT(current_object)->projected_y + OBJECT(current_object)->floor_z) >> 16) - view_y - 0x20u);
	queue_sprite_at_depth(&ignored, &depth);
}

void set_icicle_wall_box(void)
{
	uint32_t direction = 0;

	OBJECT(current_object)->flags |= 2u;
	direction = OBJECT(current_object)->direction;
	switch (direction >> 8)
	{
	case 0:
		OBJECT(current_object)->hitbox_size_x = 0x70u;
		OBJECT(current_object)->hitbox_size_y = 0x80u;
		OBJECT(current_object)->hitbox_offset_x = -0x20;
		OBJECT(current_object)->hitbox_offset_y = -0x40;
		break;
	case 1:
		OBJECT(current_object)->hitbox_size_x = 0x80u;
		OBJECT(current_object)->hitbox_size_y = 0x70u;
		OBJECT(current_object)->hitbox_offset_x = -0x40;
		OBJECT(current_object)->hitbox_offset_y = -0x40;
		break;
	case 2:
	case 4:
		OBJECT(current_object)->hitbox_size_x = 0x70u;
		OBJECT(current_object)->hitbox_size_y = 0x80u;
		OBJECT(current_object)->hitbox_offset_x = -0x20;
		OBJECT(current_object)->hitbox_offset_y = -0x7F;
		break;
	case 3:
		OBJECT(current_object)->hitbox_size_x = 0x80u;
		OBJECT(current_object)->hitbox_size_y = 0x70u;
		OBJECT(current_object)->hitbox_offset_x = 0;
		OBJECT(current_object)->hitbox_offset_y = -0x20;
		break;
	default:
		unknown_code(0x88EBFu + (direction >> 8) * 2u);
	}
}

void tornado_start(void)
{
	uint16_t angle_offset = 0, radius = 0;
	uint32_t column_slot = 0, columns_left = 0, free_count = 0, ignored = 0, object = 0, object_2 = 0, tornado_record = 0, zone_index = 0;

	if (sand_block_leader_active != 0u && (OBJECT(current_object)->sprite_frame != 1u || scroll_position < 0x9C00u))
		return;
	tornado_record = tornado_records_ADDRESS + tornado_records[OBJECT(current_object)->sprite_frame];
	OBJECT(current_object)->x_int = ROM16(tornado_record);
	OBJECT(current_object)->x_fraction = 0u;
	tornado_record += 2u;
	OBJECT(current_object)->y_int = ROM16(tornado_record);
	OBJECT(current_object)->y_fraction = 0u;
	tornado_record += 2u;
	OBJECT(current_object)->animation = ROM16(tornado_record) + tornado_hazard_lists_ADDRESS;
	tornado_kind = OBJECT(current_object)->sprite_frame;
	tornado_record += 2u;
	object = ROM16(tornado_record) + tornado_whirlwind_lists_ADDRESS;
	OBJECT(current_object)->part_list = object;
	OBJECT(current_object)->tornado.path = tornado_record + 2u;
	OBJECT(current_object)->flags = 2u;
	OBJECT(current_object)->tornado.holding_back = 0u;
	OBJECT(current_object)->kind = 0x1Eu;
	OBJECT(current_object)->tornado.wait_timer = 0;
	OBJECT(current_object)->tornado.rotation_angle = 0u;
	OBJECT(current_object)->hitbox_height = 0x70u;
	OBJECT(current_object)->tornado.off_screen = 0xFFu;
	OBJECT(current_object)->handler = tornado_move_ADDRESS;
	chaser_object = current_object;
	tornado_object = current_object;
	group_leader_0_held = 0xFFu;
	if (tornado_kind == 0u)
		players_frozen = 0xFFu;
	if (tornado_kind == 0u)
	{
		zone_index = 1u;
		object = spawn_chaser_back_zone(zone_index, object);
	}
	else
		object = spawn_tornado_rear_zone(object);
	if (free_object_stack < 5)
		return;
	columns_left = 5u;
	column_slot = current_object + 0x96u;
	angle_offset = 0u;
	radius = 0x4000u;
	do
	{
		allocate_object(&ignored, &object);
		OBJECT(object)->sprite_flags = 0x1004u;
		OBJECT(object)->sprite_palette = 0x1100u;
		OBJECT(object)->handler = tornado_column_circle_ADDRESS;
		OBJECT(object)->flags = 0u;
		OBJECT(object)->sprite_offset_x = 0;
		OBJECT(object)->sprite_offset_y = 0;
		OBJECT(object)->hitbox_size_y = 0x200u;
		OBJECT(object)->speed_z_fraction = radius;
		radius += 0x2000u;
		OBJECT(object)->speed_z_int = angle_offset;
		angle_offset += 0xCCu;
		OBJECT(object)->animation = tornado_column_animation_ADDRESS;
		OBJECT(object)->animation_frame = 0u;
		OBJECT(object)->animation_speed = 1u;
		OBJECT(object)->animation_timer = 0;
		mem_write16(column_slot, object);
		OBJECT(object)->parent = current_object;
		object_2 = object;
		OBJECT(object_2)->child = 0xFFFFu;
		column_slot += 2u;
		if (AS8(tornado_kind) != 0u)
		{
			allocate_object_2(&free_count, &ignored, &object);
			if (free_count != 0u)
			{
				OBJECT(object)->sprite_flags = 0x1004u;
				OBJECT(object)->sprite_palette = 0x1100u;
				OBJECT(object_2)->child = object;
			}
		}
		columns_left--;
	} while (columns_left != 0u);
}

void tornado_move(void)
{
	uint8_t sound_id = 0;
	uint16_t box_across = 0, box_along = 0, hazard_coordinate = 0;
	uint32_t r0 = 0, r1 = 0, r2 = 0, r3 = 0, r11 = 0, columns_left = 0, direction = 0, floor_slope = 0, hazard_record = 0, ignored = 0, move_speed = 0, object = 0, path_cursor = 0, slope_factor = 0, slot_cursor = 0, x = 0, y = 0, z = 0;
	uint8_t hold_timer = 0, sound_timer = 0;
	uint16_t hazard_scroll = 0, record_scroll = 0;
	uint32_t flags = 0;

	if (map_screen_active != 0u)
	{
		object = current_object;
		slot_cursor = current_object + 0x96u;
		columns_left = 5u;
		do
		{
			object = (object & ~0xFFFFu) | mem_read16(slot_cursor);
			OBJECT(object)->flags |= 0x400u;
			columns_left--;
			slot_cursor += 2u;
		} while (columns_left != 0u);
		delete_current_object();
		return;
	}
	flags = OBJECT(current_object)->flags;
	OBJECT(current_object)->flags = flags | 0x80000000u;
	if ((flags & 0x80000000u) != 0u)
	{
		if (OBJECT(current_object)->tornado.holding_back != 0u)
			goto tornado_move_step;
		if (no_player_in_play != 0u || player_mishap != 0u || player_dropping_in != 0u)
		{
			OBJECT(current_object)->tornado.holding_back = 0xFFu;
			OBJECT(current_object)->tornado.saved_base_speed = OBJECT(current_object)->tornado.base_speed;
			OBJECT(current_object)->tornado.base_speed = 0;
			OBJECT(current_object)->tornado.hold_start_position = scroll_position;
			OBJECT(current_object)->tornado.hold_timer = 0xFFu;
			OBJECT(current_object)->tornado.saved_threshold = OBJECT(current_object)->tornado.screen_threshold;
			r1 = 0xFFB0u;
			if (OBJECT(current_object)->direction < 0x100 || OBJECT(current_object)->direction > 0x200)
				r1 = 0x190u;
			OBJECT(current_object)->tornado.screen_threshold = r1;
			goto tornado_move_step;
		}
	}
	if (current_game_mode != 0x11u)
		OBJECT(current_object)->tornado.wait_timer--;
	if (OBJECT(current_object)->tornado.wait_timer > 0)
	{
	tornado_move_step:
		find_floor(&direction, &r1, &r2, &r3, &r11);
		OBJECT(current_object)->z = OBJECT(current_object)->floor_z;
		OBJECT(current_object)->z_int += 8u;
		OBJECT(current_object)->tornado.rotation_angle -= 4u;
		OBJECT(current_object)->tornado.rotation_angle &= 0x3FFu;
		project_object(&r0, &ignored);
		r0 = add_to_zone_list(r0);
		r11 = tornado_throw_whirlwinds((uint8_t)r0, r2, r3, r11);
		hazard_record = OBJECT(current_object)->animation;
		hazard_scroll = mem_read16(hazard_record);
		hazard_record += 2u;
		if (hazard_scroll <= scroll_position)
		{
			hazard_coordinate = mem_read16(hazard_record);
			direction = OBJECT(current_object)->direction;
			switch (direction >> 8)
			{
			case 0:
			case 4:
				if (OBJECT(current_object)->y_int <= (int16_t)hazard_coordinate)
				{
					y = OBJECT(current_object)->y;
				tornado_hazard_x_at_block_edge:
					x = mem_read32(floor_blocks + 2u + OBJECT(current_object)->floor_block) - 0x80000u;
				tornado_place_hazard:
					z = OBJECT(current_object)->z;
					spawn_tornado_hazard(x, y, z, r11);
					OBJECT(current_object)->animation += 4u;
				}
				break;
			case 1:
				if ((int16_t)hazard_coordinate <= OBJECT(current_object)->x_int)
				{
					x = OBJECT(current_object)->x - 0x800000u;
				tornado_hazard_y_at_block_edge:
					y = mem_read32(floor_blocks + 2u + OBJECT(current_object)->floor_block + 8u) - 0x80000u;
					goto tornado_place_hazard;
				}
				break;
			case 2:
				if ((int16_t)hazard_coordinate <= OBJECT(current_object)->y_int)
				{
					y = OBJECT(current_object)->y - 0x800000u;
					goto tornado_hazard_x_at_block_edge;
				}
				break;
			case 3:
				if (OBJECT(current_object)->x_int <= (int16_t)hazard_coordinate)
				{
					x = OBJECT(current_object)->x;
					goto tornado_hazard_y_at_block_edge;
				}
				break;
			default:
				unknown_code(0x89477u + (direction >> 8) * 2u);
			}
		}
		if (OBJECT(current_object)->tornado.holding_back != 0u)
		{
			if ((int16_t)(scroll_position - OBJECT(current_object)->tornado.hold_start_position) >= 0x300)
				goto tornado_stop_holding_back;
			if (player_mishap != 0u || player_dropping_in != 0u || no_player_in_play != 0u)
				OBJECT(current_object)->tornado.hold_timer = 0xFFu;
			hold_timer = OBJECT(current_object)->tornado.hold_timer;
			OBJECT(current_object)->tornado.hold_timer = hold_timer - 1u;
			if (hold_timer == 0u)
				goto tornado_stop_holding_back;
		}
		if (OBJECT(current_object)->screen_y < 0)
		{
		tornado_catch_up:
			if (OBJECT(current_object)->tornado.holding_back != 0u)
			{
			tornado_stop_holding_back:
				OBJECT(current_object)->tornado.holding_back = 0u;
				OBJECT(current_object)->tornado.screen_threshold = OBJECT(current_object)->tornado.saved_threshold;
				OBJECT(current_object)->tornado.base_speed = OBJECT(current_object)->tornado.saved_base_speed;
			}
			slope_factor = 0u;
			if ((OBJECT(current_object)->floor_slope | OBJECT(current_object)->floor_type) != 0u)
			{
				floor_slope = OBJECT(current_object)->floor_slope;
				slope_factor = floor_slope_factors[(uint16_t)(floor_slope * 2u)];
			}
			compute_camera_slope_speed(r3, slope_factor);
			move_speed = (uint16_t)((uint32_t)camera_slope_speed_int * OBJECT(current_object)->tornado.catch_up_factor) >> 8;
			if ((int16_t)move_speed <= OBJECT(current_object)->tornado.base_speed)
			{
			tornado_use_base_speed:
				move_speed = (uint16_t)OBJECT(current_object)->tornado.base_speed;
			}
		}
		else
		{
			direction = OBJECT(current_object)->direction;
			switch (direction >> 8)
			{
			case 0:
			case 3:
				if (OBJECT(current_object)->screen_x <= OBJECT(current_object)->tornado.screen_threshold)
					goto tornado_use_base_speed;
				goto tornado_catch_up;
			case 1:
			case 2:
			case 4:
				if (OBJECT(current_object)->tornado.screen_threshold <= OBJECT(current_object)->screen_x)
					goto tornado_use_base_speed;
				goto tornado_catch_up;
			default:
				unknown_code(0x89545u + (direction >> 8) * 2u);
			}
		}
		if (AS32(players_finished[0]) == 0xFFFFFFu)
			move_speed = (uint16_t)(move_speed << 2);
		chaser_speed = move_speed;
		if (OBJECT(current_object)->tornado.wait_timer <= 0)
		{
			direction = OBJECT(current_object)->direction;
			switch (direction >> 8)
			{
			case 0:
				OBJECT(current_object)->y -= (uint32_t)chaser_speed << 12;
				break;
			case 1:
				OBJECT(current_object)->x += (uint32_t)chaser_speed << 12;
				break;
			case 2:
			case 4:
				OBJECT(current_object)->y += (uint32_t)chaser_speed << 12;
				break;
			case 3:
				OBJECT(current_object)->x -= (uint32_t)chaser_speed << 12;
				break;
			default:
				unknown_code(0x895E6u + (direction >> 8) * 2u);
			}
		}
		direction = OBJECT(current_object)->direction;
		switch (direction >> 8)
		{
		case 0:
		case 3:
			if (OBJECT(current_object)->screen_x > 0x180)
				OBJECT(current_object)->tornado.off_screen = 0xFFu;
			else
			{
				if (OBJECT(current_object)->tornado.off_screen != 0u)
				{
					OBJECT(current_object)->tornado.off_screen = 0u;
					OBJECT(current_object)->tornado.sound_timer = 0u;
				}
				if (current_game_mode != 7u)
				{
					sound_timer = OBJECT(current_object)->tornado.sound_timer;
					OBJECT(current_object)->tornado.sound_timer = sound_timer - 1u;
					if (sound_timer == 0u)
					{
						sound_id = 0x12u;
						play_sound(sound_id, &move_speed, &r2);
						OBJECT(current_object)->tornado.sound_timer = 0x28u;
					}
				}
			}
			break;
		case 1:
		case 2:
		case 4:
			if (OBJECT(current_object)->screen_x < -0x40)
			{
				OBJECT(current_object)->tornado.off_screen = 0xFFu;
				return;
			}
			if (OBJECT(current_object)->tornado.off_screen != 0u)
			{
				OBJECT(current_object)->tornado.off_screen = 0u;
				OBJECT(current_object)->tornado.sound_timer = 0u;
			}
			if (current_game_mode != 7u)
			{
				sound_timer = OBJECT(current_object)->tornado.sound_timer;
				OBJECT(current_object)->tornado.sound_timer = sound_timer - 1u;
				if (sound_timer == 0u)
				{
					sound_id = 0x12u;
					play_sound(sound_id, &move_speed, &r2);
					OBJECT(current_object)->tornado.sound_timer = 0x28u;
				}
			}
			return;
		default:
			unknown_code(0x89634u + (direction >> 8) * 2u);
		}
		return;
	}
	OBJECT(current_object)->tornado.wait_timer = 0;
	path_cursor = OBJECT(current_object)->tornado.path;
	record_scroll = mem_read16(path_cursor);
	path_cursor += 2u;
	if (record_scroll <= scroll_position)
	{
		path_cursor += 2u;
		goto tornado_load_record;
	}
	r1 = mem_read16(path_cursor);
	direction = OBJECT(current_object)->direction;
	path_cursor += 2u;
	switch (direction >> 8)
	{
	case 0:
	case 4:
		if ((int16_t)r1 < OBJECT(current_object)->y_int)
			goto tornado_move_step;
	tornado_load_record:
		if ((int16_t)mem_read16(path_cursor + 2u) >= 0)
		{
			OBJECT(current_object)->tornado.screen_threshold = mem_read16(path_cursor);
			path_cursor += 2u;
			OBJECT(current_object)->tornado.catch_up_factor = mem_read16(path_cursor);
			path_cursor += 2u;
			OBJECT(current_object)->tornado.base_speed = mem_read16(path_cursor);
			path_cursor += 2u;
			OBJECT(current_object)->tornado.wait_timer = mem_read16(path_cursor);
			path_cursor += 2u;
			OBJECT(current_object)->direction = mem_read16(path_cursor);
			OBJECT(current_object)->tornado.path = path_cursor + 2u;
			box_across = 0xF0u;
			box_along = 0x80u;
			direction = OBJECT(current_object)->direction;
			switch (direction >> 8)
			{
			case 0:
			case 4:
				OBJECT(current_object)->hitbox_size_x = box_across;
				OBJECT(current_object)->hitbox_size_y = box_along;
				OBJECT(current_object)->hitbox_offset_x = 0u - (box_across >> 1);
				OBJECT(current_object)->hitbox_offset_y = 0u - (box_along >> 1);
				break;
			case 1:
				OBJECT(current_object)->hitbox_size_x = box_along;
				OBJECT(current_object)->hitbox_size_y = box_across;
				OBJECT(current_object)->hitbox_offset_x = 0u - (box_along >> 1);
				OBJECT(current_object)->hitbox_offset_y = 0u - (box_across >> 1);
				break;
			case 2:
				OBJECT(current_object)->hitbox_size_x = box_across;
				OBJECT(current_object)->hitbox_size_y = box_along;
				OBJECT(current_object)->hitbox_offset_x = 0u - (box_across >> 1);
				OBJECT(current_object)->hitbox_offset_y = 0u - (box_along >> 1);
				break;
			case 3:
				OBJECT(current_object)->hitbox_size_x = box_along;
				OBJECT(current_object)->hitbox_size_y = box_across;
				OBJECT(current_object)->hitbox_offset_x = 0u - (box_along >> 1);
				OBJECT(current_object)->hitbox_offset_y = 0u - (box_across >> 1);
				if ((uint16_t)OBJECT(current_object)->tornado.wait_timer == 0u)
					players_frozen = 0u;
				break;
			default:
				unknown_code(0x8939Du + (direction >> 8) * 2u);
			}
			if (OBJECT(current_object)->tornado.path != tornado_kind_0_end_record_ADDRESS)
				goto tornado_move_step;
			group_leader_0_held = 0u;
			goto tornado_move_step;
		}
		if ((int16_t)mem_read16(path_cursor + 4u) >= 0)
		{
			object = current_object;
			slot_cursor = current_object + 0x96u;
			columns_left = 5u;
			do
			{
				object = (object & ~0xFFFFu) | mem_read16(slot_cursor);
				OBJECT(object)->flags |= 0x400u;
				columns_left--;
				slot_cursor += 2u;
			} while (columns_left != 0u);
			delete_current_object();
			return;
		}
		goal_unlocked = 0xFFu;
		object = current_object;
		slot_cursor = current_object + 0x96u;
		columns_left = 5u;
		do
		{
			object = (object & ~0xFFFFu) | mem_read16(slot_cursor);
			OBJECT(object)->flags |= 0x400u;
			columns_left--;
			slot_cursor += 2u;
		} while (columns_left != 0u);
		delete_current_object();
		return;
	case 1:
		if (OBJECT(current_object)->x_int < (int16_t)r1)
			goto tornado_move_step;
		goto tornado_load_record;
	case 2:
		if (OBJECT(current_object)->y_int < (int16_t)r1)
			goto tornado_move_step;
		goto tornado_load_record;
	case 3:
		if ((int16_t)r1 < OBJECT(current_object)->x_int)
			goto tornado_move_step;
		goto tornado_load_record;
	default:
		unknown_code(0x89306u + (direction >> 8) * 2u);
	}
}

void tornado_column_circle(void)
{
	uint32_t r0 = 0, r1 = 0, r7 = 0, r8 = 0, depth = 0, ignored = 0, object = 0, object_2 = 0, orbit_dx = 0, orbit_dy = 0, radius_limit = 0, radius_rate = 0;

	if ((OBJECT(current_object)->flags & 0x400u) != 0u)
	{
		delete_current_object();
		return;
	}
	OBJECT(current_object)->speed_z_fraction += OBJECT(current_object)->hitbox_size_y;
	if ((int16_t)OBJECT(current_object)->speed_z_fraction < 0x1000)
	{
		next_random(&radius_rate, &ignored, &ignored);
		radius_rate = (radius_rate + 0x100u) & 0x3FFu;
		radius_limit = 0x1000u;
	tornado_column_bounce_radius:
		OBJECT(current_object)->hitbox_size_y = radius_rate;
		OBJECT(current_object)->speed_z_fraction = radius_limit;
	}
	else if ((int16_t)OBJECT(current_object)->speed_z_fraction > 0x6800)
	{
		next_random(&radius_rate, &ignored, &ignored);
		radius_rate = (uint16_t)(0u - ((radius_rate + 0x100u) & 0x3FFu));
		radius_limit = 0x6800u;
		goto tornado_column_bounce_radius;
	}
	object_2 = LINKED(current_object, OBJECT(current_object)->parent);
	OBJECT(current_object)->x = OBJECT(object_2)->x;
	OBJECT(current_object)->y = OBJECT(object_2)->y;
	orbit_dx = (OBJECT(object_2)->spawn_arg_2 + OBJECT(current_object)->speed_z_int) & 0x3FFu;
	orbit_dy = OBJECT(current_object)->speed_z_fraction;
	polar_to_xy(&r1, &orbit_dx, &orbit_dy);
	OBJECT(current_object)->x += orbit_dx;
	OBJECT(current_object)->y += orbit_dy;
	animate(&r1, &r7, &r8);
	find_floor(&r0, &r1, &radius_limit, &ignored, &object_2);
	OBJECT(current_object)->z = OBJECT(current_object)->floor_z;
	project_object(&ignored, &r1);
	queue_sprite(r1);
	if ((int16_t)OBJECT(current_object)->child < 0)
		return;
	object = LINKED(current_object, OBJECT(current_object)->child);
	OBJECT(object)->screen_x = OBJECT(current_object)->screen_x;
	OBJECT(object)->screen_y = OBJECT(current_object)->screen_y;
	OBJECT(object)->screen_y -= 0xE0u;
	OBJECT(object)->sprite_frame = tornado_column_top_frames[OBJECT(current_object)->animation_frame];
	depth = (uint16_t)OBJECT(current_object)->screen_y;
	current_object = object;
	queue_sprite_at_depth(&ignored, &depth);
}

void spawn_tornado_hazard(uint32_t x, uint32_t y, uint32_t z, uint32_t hazard)
{
	uint32_t direction = 0, free_count = 0, ignored = 0;
	uint32_t saved_current_object = 0;

	allocate_object(&free_count, &hazard);
	if (free_count == 0u)
		return;
	OBJECT(hazard)->x = x;
	OBJECT(hazard)->y = y;
	OBJECT(hazard)->z = z;
	OBJECT(hazard)->sprite_flags = 0x1004u;
	OBJECT(hazard)->handler = tornado_hazard_update_ADDRESS;
	OBJECT(hazard)->flags = 2u;
	OBJECT(hazard)->hitbox_offset_x = 0;
	OBJECT(hazard)->hitbox_offset_y = 0;
	OBJECT(hazard)->direction = OBJECT(current_object)->direction;
	OBJECT(hazard)->kind = 0xCu;
	OBJECT(hazard)->hitbox_height = 0x50u;
	direction = OBJECT(hazard)->direction;
	switch (direction >> 8)
	{
	case 0:
	case 2:
	case 4:
		OBJECT(hazard)->hitbox_size_x = 0xF0u;
		OBJECT(hazard)->hitbox_size_y = 0x80u;
		break;
	case 1:
	case 3:
		OBJECT(hazard)->hitbox_size_x = 0x80u;
		OBJECT(hazard)->hitbox_size_y = 0xF0u;
		break;
	default:
		unknown_code(0x897BBu + (direction >> 8) * 2u);
	}
	saved_current_object = current_object;
	current_object = hazard;
	project_object(&ignored, &ignored);
	current_object = saved_current_object;
}

void tornado_hazard_update(void)
{
	uint16_t margin_y = 0;
	uint32_t ignored = 0, margin_x = 0, off_screen = 0;

	margin_x = 0xF0u;
	margin_y = 0xF0u;
	off_screen = is_off_screen_margin((uint16_t)margin_x, margin_y);
	if (off_screen == 0u)
	{
		add_to_touchable_list(0u);
		update_screen_position(&ignored, &margin_x);
		return;
	}
	delete_current_object();
}

uint32_t tornado_throw_whirlwinds(uint8_t spawned, uint32_t r2, uint32_t whirlwind, uint32_t object)
{
	uint32_t saved_current_object = 0;

	if (mem_read16(OBJECT(current_object)->part_list) <= scroll_position)
	{
		saved_current_object = current_object;
		object = spawn_whirlwind(spawned, r2, whirlwind, object);
		current_object = saved_current_object;
		OBJECT(current_object)->part_list += 2u;
	}
	return object;
}

uint32_t spawn_tornado_rear_zone(uint32_t zone)
{
	uint32_t free_count = 0;

	allocate_object(&free_count, &zone);
	if (free_count != 0u)
	{
		OBJECT(zone)->sprite_flags = 0x1004u;
		OBJECT(zone)->kind = 9u;
		OBJECT(zone)->flags = 0x40000002u;
		OBJECT(zone)->hitbox_height = 0x160u;
		OBJECT(zone)->hitbox_offset_x = 0;
		OBJECT(zone)->hitbox_offset_y = 0;
		OBJECT(zone)->handler = tornado_rear_zone_follow_ADDRESS;
	}
	return zone;
}

void tornado_rear_zone_follow(void)
{
	uint32_t direction = 0, object = 0;

	if (scroll_position >= 0xE500u)
	{
		delete_current_object();
		return;
	}
	object = LINKED(current_object, chaser_object);
	OBJECT(current_object)->hitbox_size_x = OBJECT(object)->hitbox_size_x;
	OBJECT(current_object)->hitbox_size_y = OBJECT(object)->hitbox_size_y;
	OBJECT(current_object)->x = OBJECT(object)->x;
	OBJECT(current_object)->y = OBJECT(object)->y;
	OBJECT(current_object)->hitbox_offset_x = OBJECT(object)->hitbox_offset_x;
	OBJECT(current_object)->hitbox_offset_y = OBJECT(object)->hitbox_offset_y;
	OBJECT(current_object)->z = OBJECT(object)->z;
	OBJECT(current_object)->z_int += 0x60u;
	OBJECT(current_object)->direction = OBJECT(object)->direction;
	direction = OBJECT(object)->direction;
	switch (direction >> 8)
	{
	case 0:
	case 4:
		OBJECT(current_object)->hitbox_size_y = 0x200u;
		OBJECT(current_object)->y_int += 0x40u;
		OBJECT(current_object)->hitbox_offset_y = 0;
		break;
	case 1:
		OBJECT(current_object)->hitbox_size_x = 0x200u;
		OBJECT(current_object)->x_int -= 0x240u;
		OBJECT(current_object)->hitbox_offset_x = 0;
		break;
	case 2:
		OBJECT(current_object)->hitbox_size_y = 0x200u;
		OBJECT(current_object)->y_int -= 0x240u;
		OBJECT(current_object)->hitbox_offset_y = 0;
		break;
	case 3:
		OBJECT(current_object)->hitbox_size_x = 0x200u;
		OBJECT(current_object)->x_int += 0x40u;
		OBJECT(current_object)->hitbox_offset_x = 0;
		break;
	default:
		unknown_code(0x898C7u + (direction >> 8) * 2u);
	}
	direction = add_to_zone_list(direction);
}

void quicksand_start(void)
{
	uint32_t r0 = 0, block_offset = 0, ignored = 0, object = 0, object_2 = 0, y = 0;

	OBJECT(current_object)->flags = 0u;
	if (OBJECT(current_object)->sprite_frame != 0u)
		OBJECT(current_object)->flags |= 1u;
	OBJECT(current_object)->sprite_flags = 0x1000u;
	OBJECT(current_object)->sprite_frame = 0x74E0u;
	OBJECT(current_object)->sprite_offset_x = 0;
	OBJECT(current_object)->sprite_offset_y = -0x34;
	OBJECT(current_object)->hitbox_size_x = 0x60u;
	OBJECT(current_object)->hitbox_height = 0x50u;
	OBJECT(current_object)->hitbox_size_y = 0u;
	OBJECT(current_object)->handler = quicksand_swirl_ADDRESS;
	OBJECT(current_object)->kind = 0x1Cu;
	OBJECT(current_object)->quicksand.palette_index = 0u;
	OBJECT(current_object)->quicksand.palette_delay = 4u;
	OBJECT(current_object)->quicksand.palette_timer = 0u;
	OBJECT(current_object)->quicksand.palette_list = quicksand_palettes_ADDRESS;
	OBJECT(current_object)->animation_frame = 0u;
	OBJECT(current_object)->animation_speed = 4u;
	OBJECT(current_object)->animation_timer = 0;
	OBJECT(current_object)->animation = quicksand_animation_ADDRESS;
	find_floor(&r0, &y, &block_offset, &ignored, &object);
	OBJECT(current_object)->z = OBJECT(current_object)->floor_z;
	OBJECT(current_object)->z_int += 0x30u;
	project_object(&ignored, &ignored);
	OBJECT(current_object)->child = 0xFFFFu;
	if (free_object_stack_2[0] >= 2)
	{
		allocate_object_2(&ignored, &ignored, &object);
		OBJECT(object)->sprite_flags = 0xD000u;
		OBJECT(current_object)->child = object;
		object_2 = object;
		allocate_object_2(&ignored, &ignored, &object);
		move_halfwords(object, quicksand_parts_ADDRESS, 0x11u);
		OBJECT(object_2)->part_list = object;
	}
}

void quicksand_swirl(void)
{
	uint16_t object = 0, sprite_frame = 0, step_frame = 0;
	uint32_t depth = 0, ignored = 0, margin = 0, next_step = 0, object_2 = 0, off_screen = 0, part_cursor = 0, parts_left = 0, step_list = 0;
	uint8_t animation_timer = 0;
	uint16_t last_step = 0;

	margin = 0x70u;
	off_screen = check_left_screen_margin((uint16_t)margin);
	if (off_screen == 0u)
	{
		add_to_touchable_list(0u);
		update_screen_position(&ignored, &margin);
		if ((OBJECT(current_object)->flags & 1u) != 0u)
			return;
		animate_sign();
		if ((int16_t)OBJECT(current_object)->child < 0)
		{
			current_object = object_cursor;
			depth = (uint16_t)(OBJECT(current_object)->screen_y - 0x58u);
			queue_sprite_at_depth(&ignored, &depth);
			return;
		}
		animation_timer = OBJECT(current_object)->animation_timer;
		OBJECT(current_object)->animation_timer = animation_timer - 1u;
		if ((int8_t)animation_timer <= 1)
		{
			OBJECT(current_object)->animation_timer = OBJECT(current_object)->animation_speed;
			step_list = OBJECT(current_object)->animation;
			next_step = OBJECT(current_object)->animation_frame;
			next_step = (uint8_t)(next_step + 1u);
			last_step = mem_read16(step_list);
			step_list += 2u;
			if ((int16_t)last_step < (int16_t)next_step)
				next_step = 0u;
			OBJECT(current_object)->animation_frame = next_step;
			step_frame = mem_read16(step_list + next_step * 2u);
			part_cursor = OBJECT(LINKED(current_object, OBJECT(current_object)->child))->part_list;
			parts_left = mem_read16(part_cursor);
			part_cursor += 2u;
			do
			{
				mem_write16(part_cursor, step_frame);
				part_cursor += 8u;
				parts_left--;
			} while (parts_left != 0u);
		}
		object_2 = LINKED(current_object, OBJECT(current_object)->child);
		OBJECT(object_2)->screen_x = OBJECT(current_object)->screen_x;
		OBJECT(object_2)->screen_y = OBJECT(current_object)->screen_y;
		current_object = LINKED(current_object, OBJECT(current_object)->child);
		depth = (uint16_t)(OBJECT(current_object)->screen_y - 0x58u);
		queue_sprite_at_depth(&ignored, &depth);
		current_object = object_cursor;
		depth = (uint16_t)(OBJECT(current_object)->screen_y - 0x58u);
		queue_sprite_at_depth(&ignored, &depth);
		return;
	}
	object_2 = LINKED(current_object, OBJECT(current_object)->child);
	sprite_frame = OBJECT(object_2)->sprite_frame;
	free_object_2(sprite_frame);
	object = (uint16_t)object_2;
	free_object_2(object);
	delete_current_object();
}

void sinkhole_start(void)
{
	uint32_t r0 = 0, block_offset = 0, cursor = 0, growth_record = 0, ignored = 0, y = 0;
	uint32_t sprite_frame = 0;

	OBJECT(current_object)->sprite_flags = 8u;
	sprite_frame = OBJECT(current_object)->sprite_frame;
	growth_record = sprite_frame & ~0x8000u;
	if ((sprite_frame & 0x8000u) != 0u)
		OBJECT(current_object)->sprite_flags |= 0x40u;
	growth_record = sinkhole_growth_records_ADDRESS + growth_record * 4u;
	OBJECT(current_object)->sinkhole.zoom_step = mem_read16(growth_record);
	OBJECT(current_object)->sinkhole.open_position = mem_read16(growth_record + 2u);
	OBJECT(current_object)->sinkhole.open_position += scroll_position;
	OBJECT(current_object)->sprite_palette = 0x1020u;
	OBJECT(current_object)->sprite_scale = 1u;
	OBJECT(current_object)->flags = 0u;
	OBJECT(current_object)->sinkhole.unscaled_offset_x = -8;
	if ((OBJECT(current_object)->sprite_flags & 0x40u) != 0u)
		OBJECT(current_object)->sinkhole.unscaled_offset_x = 0u - OBJECT(current_object)->sinkhole.unscaled_offset_x;
	OBJECT(current_object)->sinkhole.unscaled_offset_y = -0x30;
	OBJECT(current_object)->hitbox_size_x = 1u;
	OBJECT(current_object)->sinkhole.unscaled_hitbox_size_x = 0x4Cu;
	OBJECT(current_object)->hitbox_height = 0x40u;
	OBJECT(current_object)->hitbox_size_y = 0u;
	OBJECT(current_object)->handler = sinkhole_open_ADDRESS;
	OBJECT(current_object)->kind = 0x1Bu;
	OBJECT(current_object)->direction = 0x200u;
	if ((OBJECT(current_object)->sprite_flags & 0x40u) != 0u)
		OBJECT(current_object)->direction = 0x300u;
	OBJECT(current_object)->animation_frame = 0u;
	OBJECT(current_object)->animation_speed = 4u;
	OBJECT(current_object)->animation_timer = 0;
	OBJECT(current_object)->animation = sinkhole_animation_ADDRESS;
	find_floor(&r0, &y, &block_offset, &ignored, &cursor);
	OBJECT(current_object)->z = OBJECT(current_object)->floor_z;
	project_object(&ignored, &ignored);
}

void sinkhole_open(void)
{
	uint32_t r1 = 0, r2 = 0, r7 = 0, ignored = 0, off_screen = 0, scale = 0, sound_id = 0;

	r1 = 0x68u;
	off_screen = check_left_screen_margin((uint16_t)r1);
	if (off_screen != 0u)
	{
		delete_current_object();
		return;
	}
	if (scroll_position < OBJECT(current_object)->sinkhole.open_position)
		return;
	if ((int16_t)OBJECT(current_object)->sprite_scale >= 0x180)
	{
		add_to_touchable_list(sound_id);
		update_screen_position(&ignored, &r1);
		OBJECT(current_object)->sinkhole.draw_depth = OBJECT(current_object)->screen_y;
		r1 = queue_sprite(r1);
		return;
	}
	if (OBJECT(current_object)->sprite_scale == 1u)
	{
		sound_id = 0xFu;
		play_sound((uint8_t)sound_id, &r1, &r2);
	}
	OBJECT(current_object)->sprite_scale += OBJECT(current_object)->sinkhole.zoom_step;
	scale = OBJECT(current_object)->sprite_scale;
	OBJECT(current_object)->sprite_offset_x = (int32_t)(OBJECT(current_object)->sinkhole.unscaled_offset_x * scale) >> 8;
	OBJECT(current_object)->sprite_offset_y = (int32_t)(OBJECT(current_object)->sinkhole.unscaled_offset_y * scale) >> 8;
	animate(&scale, &r7, &off_screen);
	r1 = OBJECT(current_object)->sprite_scale;
	OBJECT(current_object)->hitbox_size_x = (int32_t)(OBJECT(current_object)->sinkhole.unscaled_hitbox_size_x * r1) >> 8;
	add_to_touchable_list(sound_id);
	update_screen_position(&ignored, &r1);
	OBJECT(current_object)->sinkhole.draw_depth = OBJECT(current_object)->screen_y;
	r1 = queue_sprite(r1);
}

void sand_patch_start(void)
{
	uint32_t r0 = 0, block_offset = 0, cursor = 0, ignored = 0, y = 0;

	OBJECT(current_object)->sprite_flags = 0x1000u;
	if ((OBJECT(current_object)->sprite_frame & 0x8000u) != 0u)
		OBJECT(current_object)->sprite_flags |= 0x40u;
	OBJECT(current_object)->sprite_frame = 0x7530u;
	OBJECT(current_object)->sprite_palette = 0x9020u;
	OBJECT(current_object)->handler = sand_patch_draw_ADDRESS;
	OBJECT(current_object)->flags = 0u;
	OBJECT(current_object)->sprite_offset_x = 0;
	OBJECT(current_object)->sprite_offset_y = 0;
	find_floor(&r0, &y, &block_offset, &ignored, &cursor);
	OBJECT(current_object)->z = OBJECT(current_object)->floor_z;
	project_object(&ignored, &ignored);
}

void sand_patch_draw(void)
{
	uint16_t unseen_margin = 0;
	uint32_t depth = 0, ignored = 0, margin = 0, off_screen = 0;

	margin = 0x38u;
	unseen_margin = 0x10u;
	off_screen = check_left_screen_margins((uint16_t)margin, unseen_margin);
	if (off_screen == 0u)
	{
		update_screen_position(&ignored, &margin);
		depth = (uint16_t)(OBJECT(current_object)->screen_y - 0x38u);
		queue_sprite_at_depth(&ignored, &depth);
		return;
	}
	delete_current_object();
}

void rimmed_sinkhole_start(void)
{
	uint32_t block_offset = 0, free_count = 0, growth_record = 0, ignored = 0, object = 0, y = 0;
	uint32_t sprite_frame = 0;

	OBJECT(current_object)->sprite_flags = 0x1000u;
	OBJECT(current_object)->hitbox_size_x = 0x70u;
	OBJECT(current_object)->hitbox_height = 0x40u;
	OBJECT(current_object)->sprite_offset_x = 0;
	OBJECT(current_object)->sprite_offset_y = 0;
	OBJECT(current_object)->hitbox_size_y = 0u;
	AS16(OBJECT(current_object)->flags) = 0u;
	OBJECT(current_object)->kind = 0x1Du;
	allocate_object(&free_count, &object);
	if (free_count == 0u)
		return;
	OBJECT(object_cursor)->handler = rimmed_sinkhole_edge_zone_update_ADDRESS;
	current_object = object;
	OBJECT(object_cursor)->parent = current_object;
	OBJECT(current_object)->var_86 = object_cursor;
	OBJECT(current_object)->x = OBJECT(object_cursor)->x;
	OBJECT(current_object)->y = OBJECT(object_cursor)->y;
	OBJECT(current_object)->sprite_flags = 0u;
	sprite_frame = OBJECT(object_cursor)->sprite_frame;
	growth_record = sprite_frame & ~0x8000u;
	if ((sprite_frame & 0x8000u) != 0u)
		OBJECT(current_object)->sprite_flags |= 0x40u;
	growth_record = sinkhole_growth_records_ADDRESS + growth_record * 4u;
	AS16(OBJECT(current_object)->var_80) = mem_read16(growth_record);
	OBJECT(current_object)->var_84 = mem_read16(growth_record + 2u);
	OBJECT(current_object)->var_84 += scroll_position;
	OBJECT(current_object)->sprite_frame = 0x7518u;
	OBJECT(current_object)->sprite_palette = 0x9020u;
	OBJECT(current_object)->sprite_scale = 1u;
	OBJECT(current_object)->flags = 0u;
	OBJECT(current_object)->sprite_offset_x = 0;
	OBJECT(current_object)->sprite_offset_y = 0;
	OBJECT(current_object)->handler = rimmed_sinkhole_open_ADDRESS;
	OBJECT(current_object)->hitbox_size_x = 1u;
	OBJECT(current_object)->var_8C = 0x4Cu;
	OBJECT(current_object)->hitbox_height = 0x40u;
	OBJECT(current_object)->hitbox_size_y = 0u;
	OBJECT(current_object)->kind = 0x1Bu;
	OBJECT(current_object)->direction = 0u;
	if ((OBJECT(current_object)->sprite_flags & 0x40u) != 0u)
		OBJECT(current_object)->direction = 0x100u;
	OBJECT(current_object)->animation = rimmed_sinkhole_animation_ADDRESS;
	OBJECT(current_object)->animation_frame = 0u;
	OBJECT(current_object)->animation_speed = 4u;
	OBJECT(current_object)->animation_timer = 0;
	find_floor(&free_count, &y, &block_offset, &ignored, &object);
	OBJECT(current_object)->z = OBJECT(current_object)->floor_z;
	OBJECT(object_cursor)->z = OBJECT(current_object)->floor_z;
	project_object(&ignored, &ignored);
	allocate_object_2(&free_count, &ignored, &object);
	if (free_count == 0u)
		return;
	OBJECT(object)->sprite_flags = 0u;
	if ((OBJECT(current_object)->sprite_flags & 0x40u) != 0u)
		OBJECT(object)->sprite_flags |= 0x40u;
	OBJECT(object)->sprite_frame = 0x7538u;
	OBJECT(object)->sprite_palette = 0xF020u;
	OBJECT(object)->sprite_scale = 1u;
	OBJECT(current_object)->child = object;
}

void rimmed_sinkhole_open(void)
{
	uint16_t child = 0;
	uint32_t r7 = 0, depth = 0, ignored = 0, margin = 0, off_screen = 0, sound_id = 0, unseen_margin = 0;

	margin = 0x58u;
	unseen_margin = 0x20u;
	off_screen = check_left_screen_margins((uint16_t)margin, (uint16_t)unseen_margin);
	if (off_screen != 0u)
	{
		child = OBJECT(current_object)->child;
		free_object_2(child);
		current_object = (current_object & ~0xFFFFu) | OBJECT(current_object)->rimmed_sinkhole.edge_zone;
		OBJECT(current_object)->flags |= 0x400u;
		delete_current_object();
		return;
	}
	if (scroll_position < OBJECT(current_object)->rimmed_sinkhole.open_position)
		return;
	if ((int16_t)OBJECT(current_object)->sprite_scale < 0x100)
	{
		if (OBJECT(current_object)->sprite_scale == 1u)
		{
			sound_id = 0xFu;
			play_sound((uint8_t)sound_id, &margin, &unseen_margin);
		}
		OBJECT(current_object)->sprite_scale += OBJECT(current_object)->rimmed_sinkhole.zoom_step;
		OBJECT(LINKED(current_object, OBJECT(current_object)->child))->sprite_scale = OBJECT(current_object)->sprite_scale;
		animate(&margin, &r7, &off_screen);
		OBJECT(current_object)->hitbox_size_x = (int32_t)((uint32_t)OBJECT(current_object)->rimmed_sinkhole.unscaled_hitbox_size_x * OBJECT(current_object)->sprite_scale) >> 8;
	}
	add_to_touchable_list(sound_id);
	update_screen_position(&ignored, &margin);
	depth = (uint16_t)(OBJECT(current_object)->screen_y - 0x30u);
	if ((int16_t)depth <= -0xF)
		depth = 0xFFF1u;
	OBJECT(current_object)->rimmed_sinkhole.draw_depth = depth;
	queue_sprite_at_depth(&ignored, &depth);
	current_object = LINKED(current_object, OBJECT(current_object)->child);
	OBJECT(current_object)->screen_x = OBJECT(object_cursor)->screen_x;
	OBJECT(current_object)->screen_y = OBJECT(object_cursor)->screen_y;
	depth = (uint16_t)(OBJECT(object_cursor)->rimmed_sinkhole.draw_depth - 8u);
	queue_sprite_at_depth(&ignored, &depth);
}

void rimmed_sinkhole_edge_zone_update(void)
{
	uint32_t r0 = 0, object = 0;

	if ((OBJECT(current_object)->flags & 0x400u) == 0u)
	{
		object = LINKED(current_object, OBJECT(current_object)->parent);
		OBJECT(current_object)->sprite_scale = OBJECT(object)->sprite_scale;
		OBJECT(current_object)->rimmed_sinkhole.draw_depth = OBJECT(object)->var_88;
		r0 = add_to_touchable_list(0u);
		return;
	}
	delete_current_object();
}

void spawn_sand_block_leader(uint32_t record, uint32_t *p_spawned, uint32_t *p_leader)
{
	uint32_t leader = *p_leader;
	uint32_t free_count = 0, spawned = 0;

	allocate_object_outside_loop(&free_count, &leader);
	if (free_count == 0u)
	{
		*p_spawned = free_count;
		*p_leader = leader;
		return;
	}
	group_leaders[mem_read8(record) & 3u] = leader;
	record++;
	OBJECT(leader)->x_int = mem_read16(record);
	OBJECT(leader)->x_fraction = 0u;
	record += 2u;
	OBJECT(leader)->y_int = mem_read16(record);
	OBJECT(leader)->y_fraction = 0u;
	record += 2u;
	OBJECT(leader)->z_int = mem_read16(record);
	OBJECT(leader)->z_fraction = 0u;
	OBJECT(leader)->floor_z = OBJECT(leader)->z;
	record += 2u;
	OBJECT(leader)->direction = mem_read16(record);
	OBJECT(leader)->sand_block_leader.path = mem_read16(record + 2u) + group_leader_data_ADDRESS;
	OBJECT(leader)->handler = sand_block_leader_move_ADDRESS;
	OBJECT(leader)->flags = 0u;
	OBJECT(leader)->sprite_offset_x = 0;
	OBJECT(leader)->sprite_offset_y = 0;
	OBJECT(leader)->sand_block_leader.holding_back = 0u;
	OBJECT(leader)->sand_block_leader.wait_timer = 1;
	sand_block_leader_active = 0xFFu;
	if (group_leader_0_held != 0u)
		OBJECT(leader)->handler = sand_block_leader_wait_ADDRESS;
	spawned = 0xFFFFFFFFu;
	*p_spawned = spawned;
	*p_leader = leader;
}

void sand_block_leader_wait(void)
{
	if (scroll_position >= 0x5100u || group_leader_0_held == 0u)
		OBJECT(current_object)->handler = sand_block_leader_move_ADDRESS;
}

void sand_block_leader_move(void)
{
	uint16_t move_speed = 0;
	uint32_t r0 = 0, r3 = 0, block_offset = 0, block_record = 0, hold_threshold = 0, ignored = 0, path_cursor = 0;
	uint8_t hold_timer = 0;
	uint16_t wait_timer = 0, record_scroll = 0;
	uint32_t flags = 0;

	flags = OBJECT(current_object)->flags;
	OBJECT(current_object)->flags = flags | 0x80000000u;
	if ((flags & 0x80000000u) == 0u)
	{
	sand_block_leader_read_script:
		wait_timer = OBJECT(current_object)->sand_block_leader.wait_timer;
		OBJECT(current_object)->sand_block_leader.wait_timer = wait_timer - 1u;
		if ((int16_t)wait_timer <= 1)
		{
			OBJECT(current_object)->sand_block_leader.wait_timer = 0;
			path_cursor = OBJECT(current_object)->sand_block_leader.path;
			record_scroll = mem_read16(path_cursor);
			path_cursor += 2u;
			if (record_scroll <= scroll_position)
			{
				if ((int16_t)mem_read16(path_cursor + 2u) < 0)
				{
					delete_current_object();
					return;
				}
				OBJECT(current_object)->sand_block_leader.screen_threshold = mem_read16(path_cursor);
				path_cursor += 2u;
				OBJECT(current_object)->sand_block_leader.catch_up_factor = mem_read16(path_cursor);
				path_cursor += 2u;
				OBJECT(current_object)->sand_block_leader.base_speed = mem_read16(path_cursor);
				path_cursor += 2u;
				OBJECT(current_object)->sand_block_leader.wait_timer = mem_read16(path_cursor);
				OBJECT(current_object)->sand_block_leader.path = path_cursor + 2u;
			}
		}
	}
	else if (OBJECT(current_object)->sand_block_leader.holding_back == 0u)
	{
		if (no_player_in_play == 0u && player_mishap == 0u && player_dropping_in == 0u)
			goto sand_block_leader_read_script;
		OBJECT(current_object)->sand_block_leader.holding_back = 0xFFu;
		OBJECT(current_object)->sand_block_leader.saved_base_speed = OBJECT(current_object)->sand_block_leader.base_speed;
		OBJECT(current_object)->sand_block_leader.base_speed = 0;
		OBJECT(current_object)->sand_block_leader.hold_start_position = scroll_position;
		OBJECT(current_object)->sand_block_leader.hold_timer = 0xFFu;
		OBJECT(current_object)->sand_block_leader.saved_threshold = OBJECT(current_object)->sand_block_leader.screen_threshold;
		hold_threshold = 0xFFE0u;
		if ((OBJECT(current_object)->direction & 1u) != 0u)
			hold_threshold = 0x160u;
		OBJECT(current_object)->sand_block_leader.screen_threshold = hold_threshold;
	}
	find_floor(&r0, &hold_threshold, &block_offset, &r3, &block_record);
	project_object(&ignored, &ignored);
	add_to_rider_list();
	if (OBJECT(current_object)->sand_block_leader.holding_back != 0u)
	{
		if ((int16_t)(scroll_position - OBJECT(current_object)->sand_block_leader.hold_start_position) >= 0x300)
			goto sand_block_leader_stop_holding_back;
		if (no_player_in_play != 0u)
			OBJECT(current_object)->sand_block_leader.hold_timer = 0xFFu;
		hold_timer = OBJECT(current_object)->sand_block_leader.hold_timer;
		OBJECT(current_object)->sand_block_leader.hold_timer = hold_timer - 1u;
		if (hold_timer == 0u)
			goto sand_block_leader_stop_holding_back;
	}
	switch (OBJECT(current_object)->direction)
	{
	case 0:
	case 2:
	case 4:
		if (OBJECT(current_object)->sand_block_leader.screen_threshold <= OBJECT(current_object)->screen_x)
		{
		sand_block_leader_use_base_speed:
			move_speed = (uint16_t)OBJECT(current_object)->sand_block_leader.base_speed;
		}
		else
		{
		sand_block_leader_catch_up:
			OBJECT(current_object)->sand_block_leader.wait_timer = 0;
			if (OBJECT(current_object)->sand_block_leader.holding_back != 0u)
			{
			sand_block_leader_stop_holding_back:
				OBJECT(current_object)->sand_block_leader.holding_back = 0u;
				OBJECT(current_object)->sand_block_leader.screen_threshold = OBJECT(current_object)->sand_block_leader.saved_threshold;
				OBJECT(current_object)->sand_block_leader.base_speed = OBJECT(current_object)->sand_block_leader.saved_base_speed;
			}
			compute_camera_ground_speed(r3);
			move_speed = (uint16_t)((uint32_t)camera_ground_speed_int * OBJECT(current_object)->sand_block_leader.catch_up_factor) >> 8;
			if ((int16_t)move_speed <= OBJECT(current_object)->sand_block_leader.base_speed)
				goto sand_block_leader_use_base_speed;
		}
		break;
	case 1:
	case 3:
		if (OBJECT(current_object)->sand_block_leader.screen_threshold < OBJECT(current_object)->screen_x || OBJECT(current_object)->screen_y > 0xE0)
			goto sand_block_leader_catch_up;
		goto sand_block_leader_use_base_speed;
	default:
		unknown_code(0x8A046u + OBJECT(current_object)->direction * 2u);
	}
	chaser_speed = move_speed;
	if (OBJECT(current_object)->sand_block_leader.wait_timer <= 0)
	{
		block_record = floor_blocks + 2u + OBJECT(current_object)->floor_block;
		switch (OBJECT(current_object)->direction)
		{
		case 0:
		case 4:
			OBJECT(current_object)->y += (uint32_t)chaser_speed << 12;
			OBJECT(current_object)->x = ((int32_t)(mem_read32(block_record + 4u) - mem_read32(block_record)) >> 1) + mem_read32(block_record);
			break;
		case 1:
			OBJECT(current_object)->y -= (uint32_t)chaser_speed << 12;
			OBJECT(current_object)->x = ((int32_t)(mem_read32(block_record + 4u) - mem_read32(block_record)) >> 1) + mem_read32(block_record);
			break;
		case 2:
			OBJECT(current_object)->x += (uint32_t)chaser_speed << 12;
			OBJECT(current_object)->y = ((int32_t)(mem_read32(block_record + 0xCu) - mem_read32(block_record + 8u)) >> 1) + mem_read32(block_record + 8u);
			break;
		case 3:
			OBJECT(current_object)->x -= (uint32_t)chaser_speed << 12;
			OBJECT(current_object)->y = ((int32_t)(mem_read32(block_record + 0xCu) - mem_read32(block_record + 8u)) >> 1) + mem_read32(block_record + 8u);
			break;
		default:
			unknown_code(0x8A0D4u + OBJECT(current_object)->direction * 2u);
		}
	}
	if (OBJECT(current_object)->y_int <= 0xA2B)
		sand_block_leader_active = 0u;
}
