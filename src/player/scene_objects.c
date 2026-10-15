/**
 *    Name:        player/scene_objects.c
 *    Description: around the player: the ride of game mode 0x1C, joining with a character select, the result bonus, the climb zone splash, the intro marker, the hurt lamp, the stall check
 *    Version:     0.1.0
 *    Created:     12-10-2026
 *    Modified:    15-10-2026
 *    Author:      Sebastian Toyos
 *
 *    SegaSonic The Hedgehog (arcade, Sega System 32): the game's V60 program decompiled into C by
 *    tools/decompile.py, with the names of symbols.txt.
 */
#include "game.h"

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
		OBJECT(object)->animation = climb_zone_effect_animation_ADDRESS;
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
		OBJECT(object)->animation = intro_marker_animation_ADDRESS;
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
