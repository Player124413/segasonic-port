/**
 *    Name:        midi_port.c
 *    Description: unused: a MIDI port on an 8251 USART (ports F0 and F1)
 *    Version:     0.1.0
 *    Created:     13-10-2026
 *    Modified:    15-10-2026
 *    Author:      Sebastian Toyos
 *
 *    SegaSonic The Hedgehog (arcade, Sega System 32): the Z80 sound program decompiled into C by
 *    tools/decompile_sound.py, with the names of sound_symbols.txt.
 */
#include "sound.h"

void receive_midi_byte(void)
{
	uint8_t message_length = 0, midi_byte = 0, received = 0, write_index = 0;
	uint16_t bytes_left_address = 0;
	uint8_t old_bytes_left = 0;

	for (;;)
	{
		z80_block(25);
		if ((port_in(MIDI_USART_CONTROL) & 0x38u) == 0u)
			break;
		z80_block(17);
		reset_midi_errors();
		z80_block(10);
	}
	z80_cycles(5);
	z80_block(28);
	received = port_in(MIDI_USART_DATA);
	if (received == 0xF2u)
	{
		z80_block(7);
		received = 0x80u;
	}
	z80_block(12);
	if (received >= 0xF0u)
	{
		z80_cycles(6);
		return;
	}
	z80_block(35);
	midi_byte = received;
	bytes_left_address = midi_in_bytes_left_ADDRESS;
	if (midi_in_bytes_left == 0u)
	{
		z80_block(18);
		if (midi_byte >> 7 != 0u)
		{
			z80_block(28);
			message_length = 2u;
			received = midi_byte;
			if (received < 0xC0u)
			{
				z80_block(4);
				message_length++;
			}
			else
			{
				z80_block(17);
				if (received >= 0xE0u)
				{
					z80_block(4);
					message_length++;
					z80_block(24);
					midi_in_bytes_left = message_length;
					midi_in_message_length = message_length;
					z80_block(72);
					write_index = midi_queue_write;
					z80_write8(0xE100u | write_index, midi_byte);
					midi_queue_write = write_index + 1u;
					old_bytes_left = midi_in_bytes_left;
					midi_in_bytes_left = old_bytes_left - 1u;
					if (old_bytes_left != 1u)
					{
						z80_cycles(6);
						return;
					}
					z80_block(31);
					midi_queue_count++;
					return;
				}
			}
			z80_block(24);
			midi_in_bytes_left = message_length;
			midi_in_message_length = message_length;
		}
		else
		{
			z80_block(34);
			midi_in_bytes_left = midi_in_message_length - 1u;
		}
	}
	z80_block(72);
	write_index = midi_queue_write;
	z80_write8(0xE100u | write_index, midi_byte);
	midi_queue_write = write_index + 1u;
	old_bytes_left = midi_in_bytes_left;
	midi_in_bytes_left = old_bytes_left - 1u;
	if (old_bytes_left != 1u)
	{
		z80_cycles(6);
		return;
	}
	z80_block(31);
	midi_queue_count++;
}

void receive_midi_byte_spare_return_1(void)
{
	z80_block(10);
}

void receive_midi_byte_spare_return_2(void)
{
	z80_block(10);
}

void send_midi_byte(uint8_t midi_byte)
{
	uint32_t carry = 0;
	uint8_t status = 0, status_copy = 0;

	for (;;)
	{
		z80_block(38);
		for (;;)
		{
			z80_block(32);
			status = port_in(MIDI_USART_CONTROL);
			status_copy = status;
			if ((status & 0x38u) != 0u)
				break;
			z80_block(18);
			carry = status_copy & 1u;
			if (carry)
			{
				z80_block(63);
				port_out(MIDI_USART_DATA, (uint8_t)midi_byte);
				return;
			}
		}
		z80_block(17);
		reset_midi_errors();
		z80_block(10);
	}
}

void reset_midi_errors(void)
{
	z80_block(70);
	port_out(MIDI_USART_CONTROL, 0x37u);
}

void init_midi_port(void)
{
	z80_block(329);
	port_out(MIDI_USART_CONTROL, 0u);
	port_out(MIDI_USART_CONTROL, 0u);
	port_out(MIDI_USART_CONTROL, 0u);
	port_out(MIDI_USART_CONTROL, 0x40u);
	port_out(MIDI_USART_CONTROL, 0x4Eu);
	port_out(MIDI_USART_CONTROL, 0x37u);
}
