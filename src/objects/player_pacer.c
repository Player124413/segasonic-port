/**
 *    Name:        objects/player_pacer.c
 *    Description: the pacer that keeps two players together (stages 3, 4, 8)
 *    Version:     0.1.0
 *    Created:     12-10-2026
 *    Modified:    15-10-2026
 *    Author:      Sebastian Toyos
 *
 *    SegaSonic The Hedgehog (arcade, Sega System 32): the game's V60 program decompiled into C by
 *    tools/decompile.py, with the names of symbols.txt.
 */
#include "game.h"

void player_pacer_start(void)
{
	OBJECT(current_object)->speed_x_fraction = pacer_end_positions[OBJECT(current_object)->sprite_frame];
	OBJECT(current_object)->handler = player_pacer_update_ADDRESS;
	player_pacer_update();
}

void player_pacer_update(void)
{
	uint32_t axis_field = 0, decreasing = 0, leader_object = 0, middle_player = 0, other_object = 0, player = 0, player_1 = 0, player_2 = 0, player_3 = 0, player_4 = 0, players_left = 0, playing_count = 0, status_cursor = 0;
	uint8_t status = 0;
	uint32_t swap = 0, swap_2 = 0, swap_3 = 0, swap_4 = 0;

	if (OBJECT(current_object)->speed_x_fraction <= scroll_position)
	{
		player_1_speed_scale = 0u;
		player_2_speed_scale = 0u;
		player_3_speed_scale = 0u;
		delete_current_object();
		return;
	}
	player_1_speed_scale = 0u;
	player_2_speed_scale = 0u;
	player_3_speed_scale = 0u;
	status_cursor = player_status_ADDRESS;
	players_left = 3u;
	playing_count = 0u;
	do
	{
		status = mem_read8(status_cursor);
		status_cursor++;
		if (status == 0xFFu)
			playing_count = (uint8_t)(playing_count + 1u);
		players_left--;
	} while (players_left != 0u);
	switch (playing_count)
	{
	case 0:
	case 1:
		return;
	case 2:
	case 4:
		player = PLAYER_BLOCK(1);
		player_2 = PLAYER_BLOCK(2);
		player_3 = PLAYER_BLOCK(3);
		if (player_status[0] == 0xFFu)
		{
			if (player_2_status != 0xFFu)
				player_2 = player_3;
		}
		else
		{
			player = player_2;
			player_2 = player_3;
		}
		get_pacing_axis(player, &axis_field, &decreasing);
		if ((int16_t)mem_read16(LINKED(current_object, PLAYER(player)->object) + axis_field * 2u) < (int16_t)mem_read16(LINKED(current_object, PLAYER(player_2)->object) + axis_field * 2u))
		{
			swap = player;
			player = player_2;
			player_2 = swap;
		}
		if ((uint16_t)decreasing != 0u)
		{
			mem_write16(player_2 + 0xC4u, 0xDu);
			return;
		}
		PLAYER(player)->speed_scale = 0xDu;
		return;
	case 3:
		player_1 = PLAYER_BLOCK(1);
		get_pacing_axis(player_1, &axis_field, &decreasing);
		player = PLAYER_BLOCK(1);
		middle_player = PLAYER_BLOCK(2);
		player_4 = PLAYER_BLOCK(3);
		leader_object = LINKED(current_object, player_1_object);
		other_object = LINKED(current_object, player_2_object);
		if ((int16_t)mem_read16(leader_object + axis_field * 2u) < (int16_t)mem_read16(other_object + axis_field * 2u))
		{
			swap_2 = player;
			player = middle_player;
			middle_player = swap_2;
			leader_object = LINKED(leader_object, PLAYER(player)->object);
		}
		other_object = LINKED(other_object, PLAYER(player_4)->object);
		if ((int16_t)mem_read16(leader_object + axis_field * 2u) < (int16_t)mem_read16(other_object + axis_field * 2u))
		{
			swap_3 = player;
			player = player_4;
			player_4 = swap_3;
		}
		if ((int16_t)mem_read16(((leader_object & ~0xFFFFu) | mem_read16(middle_player + 0x14u)) + axis_field * 2u) < (int16_t)mem_read16(((other_object & ~0xFFFFu) | mem_read16(player_4 + 0x14u)) + axis_field * 2u))
		{
			swap_4 = middle_player;
			middle_player = player_4;
			player_4 = swap_4;
		}
		if ((uint16_t)decreasing != 0u)
		{
			mem_write16(player_4 + 0xC4u, 0xDu);
			mem_write16(middle_player + 0xC4u, 0xEu);
			return;
		}
		PLAYER(player)->speed_scale = 0xDu;
		mem_write16(middle_player + 0xC4u, 0xEu);
		return;
	default:
		unknown_code(0x84428u + playing_count * 2u);
	}
}

void get_pacing_axis(uint32_t player, uint32_t *p_axis_field, uint32_t *p_decreasing)
{
	uint16_t decreasing = 0;
	uint32_t axis_field = 0;

	switch (mem_read8(floor_blocks + 2u + OBJECT(LINKED(current_object, PLAYER(player)->object))->floor_block + 0x15u))
	{
	case 0:
	case 4:
		axis_field = 0x29u;
		decreasing = 0xFFFFu;
		*p_axis_field = axis_field;
		*p_decreasing = (*p_decreasing & 0xFFFF0000u) | decreasing;
		return;
	case 1:
		axis_field = 0x27u;
		decreasing = 0u;
		*p_axis_field = axis_field;
		*p_decreasing = (*p_decreasing & 0xFFFF0000u) | decreasing;
		return;
	case 2:
		axis_field = 0x29u;
		decreasing = 0u;
		*p_axis_field = axis_field;
		*p_decreasing = (*p_decreasing & 0xFFFF0000u) | decreasing;
		return;
	case 3:
		axis_field = 0x27u;
		decreasing = 0xFFFFu;
		*p_axis_field = axis_field;
		*p_decreasing = (*p_decreasing & 0xFFFF0000u) | decreasing;
		return;
	default:
		unknown_code(0x8451Du + mem_read8(floor_blocks + 2u + OBJECT(LINKED(current_object, PLAYER(player)->object))->floor_block + 0x15u) * 2u);
	}
}
