/**
 *    Name:        objects/group_leader_hold.c
 *    Description: the hold on a group leader (stage 4 at 8B00-9500, stage 9 at 3300)
 *    Version:     0.1.0
 *    Created:     12-10-2026
 *    Modified:    15-10-2026
 *    Author:      Sebastian Toyos
 *
 *    SegaSonic The Hedgehog (arcade, Sega System 32): the game's V60 program decompiled into C by
 *    tools/decompile.py, with the names of symbols.txt.
 */
#include "game.h"

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
