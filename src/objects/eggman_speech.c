/**
 *    Name:        objects/eggman_speech.c
 *    Description: Eggman's speech scripts (stages 3, 8, A)
 *    Version:     0.1.0
 *    Created:     12-10-2026
 *    Modified:    15-10-2026
 *    Author:      Sebastian Toyos
 *
 *    SegaSonic The Hedgehog (arcade, Sega System 32): the game's V60 program decompiled into C by
 *    tools/decompile.py, with the names of symbols.txt.
 */
#include "game.h"

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
