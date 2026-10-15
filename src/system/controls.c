/**
 *    Name:        system/controls.c
 *    Description: the frame wait and the controls: read_controls, the inputs the attract demos replay
 *    Version:     0.1.0
 *    Created:     12-10-2026
 *    Modified:    15-10-2026
 *    Author:      Sebastian Toyos
 *
 *    SegaSonic The Hedgehog (arcade, Sega System 32): the game's V60 program decompiled into C by
 *    tools/decompile.py, with the names of symbols.txt.
 */
#include "game.h"

uint32_t wait_for_frames(void)
{
	uint32_t r8 = 0, ignored = 0;

	frames_to_wait = frames_per_wait;
	wait_for_frame(&ignored, &ignored, &ignored, &r8, &ignored, &ignored);
	return r8;
}

void wait_for_frame(uint32_t *p_pressed, uint32_t *p_released, uint32_t *p_input_cursor, uint32_t *p_count, uint32_t *p_trackball_port, uint32_t *p_trackball_cursor)
{
	uint32_t r1 = 0, r2 = 0, r3 = 0, r8 = 0, r10 = 0, r11 = 0, inputs = 0, poll_count = 0;
	uint8_t frames_left = 0;

	poll_count = 0u;
	sprite_list_ready = 0xFFu;
	do
	{
		frame_wait(frames_to_wait_ADDRESS);
		frames_left = frames_to_wait;
		take_interrupts();
		poll_count--;
	} while ((poll_count != 0u) & ((int8_t)frames_left >= 0));
	frame_end();
	inputs = 0u - poll_count;
	frames_to_wait = frames_per_wait;
	sprite_buffer_flip &= 0xFF04u;
	AS16(sprite_list_start) = AS16(sprite_back_buffer);
	attract_timer++;
	read_controls(&inputs, &r1, &r2, &r3, &r8, &r10, &r11);
	*p_pressed = r1;
	*p_released = r2;
	*p_input_cursor = r3;
	*p_count = r8;
	*p_trackball_port = r10;
	*p_trackball_cursor = r11;
}

void read_controls(uint32_t *p_inputs, uint32_t *p_pressed, uint32_t *p_released, uint32_t *p_input_cursor, uint32_t *p_count, uint32_t *p_trackball_port, uint32_t *p_trackball_cursor)
{
	uint32_t inputs = *p_inputs;
	uint32_t axes_left = 0, count = 0, input_cursor = 0, pressed = 0, released = 0, trackball_cursor = 0, trackball_port = 0;

	inputs = (inputs & ~0xFFu) | mem_read8(io_port_b);
	inputs = (((inputs << 8) | (inputs >> 24)) & ~0xFFu) | mem_read8(io_port_a);
	inputs = (((inputs << 8) | (inputs >> 24)) & ~0xFFu) | mem_read8(io_port_f);
	inputs = (((inputs << 8) | (inputs >> 24)) & ~0xFFu) | mem_read8(io_port_e);
	inputs = play_demo_buttons(inputs);
	pressed = input_ports;
	input_ports = inputs;
	input_ports_active = ~inputs;
	released = ~pressed & inputs;
	inputs = ~inputs;
	pressed &= inputs;
	inputs_pressed = pressed;
	input_cursor = inputs_released_ADDRESS;
	inputs_released = released;
	trackball_port = IO_PORTS + 0x40u;
	trackball_cursor = trackball_1_x_ADDRESS;
	axes_left = 6u;
	do
	{
		inputs = (inputs & ~0xFFFFu) | (uint16_t)(int8_t)mem_read8(trackball_port);
		if ((int16_t)inputs <= 0x60 && (int16_t)inputs >= -0x60)
			mem_write16(trackball_cursor, inputs);
		trackball_cursor += 2u;
		axes_left--;
		trackball_port += 4u;
	} while (axes_left != 0u);
	trackball_port = IO_PORTS + 0x40u;
	inputs &= ~0xFFFFu;
	count = 3u;
	do
	{
		mem_write16(trackball_port, inputs);
		count--;
		trackball_port += 8u;
	} while (count != 0u);
	trackball_1_x = 0u - trackball_1_x;
	trackball_2_x = 0u - trackball_2_x;
	trackball_3_x = 0u - trackball_3_x;
	play_demo_trackballs(&count, &trackball_port, &trackball_cursor);
	*p_inputs = inputs;
	*p_pressed = pressed;
	*p_released = released;
	*p_input_cursor = input_cursor;
	*p_count = count;
	*p_trackball_port = trackball_port;
	*p_trackball_cursor = trackball_cursor;
}

uint32_t play_demo_buttons(uint32_t inputs)
{
	uint32_t demo_cursor = 0, ignored = 0;

	if ((int8_t)game_in_progress >= 0 && (int8_t)test_mode >= 0 && (current_screen == 4u || current_screen == 5u) && current_game_mode == 3u)
	{
		demo_patched_inputs = inputs;
		demo_cursor = demo_input;
		move_bits_down(demo_cursor, 0u, temp_0_ADDRESS, 0x19u, 1u, &ignored, &ignored);
		move_bits_down(demo_cursor, 1u, temp_0_ADDRESS, 0x1Au, 1u, &ignored, &ignored);
		move_bits_down(demo_cursor, 2u, temp_0_ADDRESS, 0x1Bu, 1u, &ignored, &ignored);
		inputs = demo_patched_inputs;
		demo_input++;
	}
	return inputs;
}

void play_demo_trackballs(uint32_t *p_count, uint32_t *p_demo_cursor, uint32_t *p_trackball_cursor)
{
	uint32_t count = *p_count;
	uint32_t demo_cursor = *p_demo_cursor;
	uint32_t trackball_cursor = *p_trackball_cursor;

	if ((int8_t)game_in_progress < 0 || (int8_t)test_mode < 0 || (current_screen != 4u && current_screen != 5u))
		return;
	if (current_game_mode != 3u)
		return;
	demo_cursor = demo_input;
	trackball_cursor = trackball_1_x_ADDRESS;
	count = 6u;
	do
	{
		mem_write16(trackball_cursor, (int8_t)mem_read8(demo_cursor));
		trackball_cursor += 2u;
		count--;
		demo_cursor++;
	} while (count != 0u);
	demo_input = demo_cursor;
	*p_count = count;
	*p_demo_cursor = demo_cursor;
	*p_trackball_cursor = trackball_cursor;
}

void pause_on_start_3(void)
{
	uint32_t ignored = 0;

	if ((inputs_pressed & 0x80u) == 0u)
		return;
	debug_pause++;
	do
	{
		attract_timer--;
		wait_for_frame(&ignored, &ignored, &ignored, &ignored, &ignored, &ignored);
	} while ((inputs_pressed & 0x80u) == 0u);
	debug_pause--;
}
