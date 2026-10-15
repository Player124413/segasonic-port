/**
 *    Name:        main.c
 *    Description: the reset, the timer interrupt (the players' tick), the main loop and sound_init
 *    Version:     0.1.0
 *    Created:     13-10-2026
 *    Modified:    15-10-2026
 *    Author:      Sebastian Toyos
 *
 *    SegaSonic The Hedgehog (arcade, Sega System 32): the Z80 sound program decompiled into C by
 *    tools/decompile_sound.py, with the names of sound_symbols.txt.
 */
#include "sound.h"

void sound_reset(void)
{
	uint8_t unused_c = 0;

	z80_block(14);
	interrupts_off();
	z80_block(201);
	midi_in_message_length = 0u;
	move_block(midi_in_message_length_ADDRESS, midi_in_bytes_left_ADDRESS, 0x1EFFu, 1u);
	unused_c = 0u;
	port_out(IRQ_VECTOR0_INPUT, 0u);
	port_out(IRQ_VECTOR1_INPUT, 1u);
	port_out(IRQ_VECTOR2_INPUT, 2u);
	port_out(IRQ_CONTROL, 0u);
	port_out(IRQ_ACKNOWLEDGE, 0u);
	port_out(IRQ_CONTROL_D8, 0xBu);
	port_out(IRQ_MASK, 0u);
	sound_init(unused_c);
	z80_block(4);
	interrupts_on_after_block();
}

void sound_irq(void)
{
	z80_block(121);
	interrupts_off();
	port_out(IRQ_ACKNOWLEDGE, 0u);
	port_out(FM1_ADDRESS, 0x27u);
	port_out(FM1_DATA, 0x15u);
	sequence_tick();
	z80_block(54);
	interrupts_on();
}

void sound_main_loop(void)
{
	uint16_t fm_track = 0, next_fm_track = 0;
	uint16_t saved_fm_track = 0;

	do
	{
		z80_block(31);
		interrupts_off();
	} while (sound_cpu_hold != 0u);
	z80_block(29);
	interrupts_on();
	if (port_in(FM2_ADDRESS) & 1u)
	{
		z80_block(61);
		port_out(FM2_ADDRESS, 0x27u);
		port_out(FM2_DATA, 0x15u);
		refill_pcm_voice();
	}
	z80_block(53);
	fm_track = vibrato_voice;
	if (FM_VOICE(fm_track)->vibrato_depth != 0u)
	{
		z80_cycles(7);
		step_fm_vibrato(fm_track);
	}
	z80_block(87);
	saved_fm_track = fm_track;
	next_fm_track = saved_fm_track + 0x12u;
	vibrato_voice = next_fm_track;
	if (0x68u == (uint8_t)next_fm_track)
	{
		z80_block(26);
		vibrato_voice = FM_VOICE_ADDRESS(0);
	}
	z80_block(17);
	poll_sound_command();
	z80_block(17);
	handle_midi_message();
	z80_block(10);
}

void sound_init(uint8_t unused_c)
{
	uint8_t fm_register = 0, leftover_c = 0, timer_control = 0;
	uint16_t leftover_bc = 0;

	z80_block(17);
	init_pcm_chip(unused_c);
	z80_block(17);
	leftover_bc = init_fm_chips();
	leftover_c = (uint8_t)leftover_bc;
	z80_block(278);
	midi_queue_write = 0u;
	midi_queue_read = 0xFFu;
	tick_countdown = 1u;
	unread_e010 = 0x8Cu;
	next_fm_voice = FM_VOICE_ADDRESS(0);
	fm_voices_to_end = 0xCu;
	music_volume = 0xFFu;
	midi_status = 0x80u;
	midi_in_message_length = 2u;
	refill_voices_left = 8u;
	refill_voice = SAMPLE_VOICE_ADDRESS(0);
	vibrato_voice = FM_VOICE_ADDRESS(0);
	unused_c = leftover_c;
	init_track_channels(unused_c);
	z80_block(17);
	init_pcm_tracks();
	z80_block(17);
	init_midi_parts();
	z80_block(31);
	timer_control = 0x15u;
	fm_register = 0x27u;
	write_fm1(fm_register, timer_control);
	z80_block(31);
	timer_control = 0x15u;
	fm_register = 0x27u;
	write_fm2(fm_register, timer_control);
	z80_block(10);
}
