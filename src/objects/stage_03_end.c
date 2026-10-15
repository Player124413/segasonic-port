/**
 *    Name:        objects/stage_03_end.c
 *    Description: stage 3's end pillar; the unused exit gate and the unused target of the spiked roller's ram
 *    Version:     0.1.0
 *    Created:     12-10-2026
 *    Modified:    15-10-2026
 *    Author:      Sebastian Toyos
 *
 *    SegaSonic The Hedgehog (arcade, Sega System 32): the game's V60 program decompiled into C by
 *    tools/decompile.py, with the names of symbols.txt.
 */
#include "game.h"

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
	uint16_t kind = 0, x_2 = 0, y_2 = 0, z = 0;
	uint32_t block_offset = 0, ignored = 0, x = 0, y = 0;

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
	find_floor(&x, &y, &block_offset, &ignored, &sign);
	OBJECT(current_object)->z = OBJECT(current_object)->floor_z;
	OBJECT(current_object)->z_int -= 0xE0u;
	project_object(&ignored, &ignored);
	x_2 = 0x942u;
	y_2 = 0x10E0u;
	kind = 8u;
	z = (uint16_t)(OBJECT(current_object)->floor_z_int - 0x50u);
	spawn_sign_2(x_2, y_2, kind, z, sign);
}

void exit_gate_update(void)
{
	uint8_t sound_id = 0;
	uint32_t r1 = 0, ignored = 0, write_index_address = 0;

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
			play_sound(sound_id, &r1, &write_index_address);
		}
	}
	update_screen_position(&ignored, &r1);
	r1 = queue_sprite(r1);
}

void ram_target_start(void)
{
	uint32_t block_offset = 0, cursor = 0, ignored = 0, x = 0, y = 0;

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
	find_floor(&x, &y, &block_offset, &ignored, &cursor);
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
	uint32_t block_offset = 0, cursor = 0, ignored = 0, part_list = 0, x = 0, y = 0;

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
	find_floor(&x, &y, &block_offset, &ignored, &cursor);
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
