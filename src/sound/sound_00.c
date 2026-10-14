/**
 *    Name:        sound_00.c
 *    Description: the sound program's functions, part 00 (in the order of their Z80 addresses)
 *    Version:     0.1.0
 *    Created:     13-10-2026
 *    Modified:    13-10-2026
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
	unread_e000 = 0u;
	move_block(unread_e000_ADDRESS, 0xE001u, 0x1EFFu, 1u);
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
	unread_e000 = 2u;
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

uint16_t init_fm_chips(void)
{
	uint8_t fm_register = 0, init_register = 0, pairs_left = 0, value = 0;
	uint16_t init_entry = 0, leftover_bc = 0;

	z80_block(17);
	init_entry = fm_global_init_ADDRESS;
	pairs_left = 0xBu;
	for (;;)
	{
		z80_block(43);
		init_register = SOUND_ROM8(init_entry);
		init_entry++;
		value = SOUND_ROM8(init_entry);
		init_entry++;
		fm_register = init_register;
		write_fm1(fm_register, value);
		z80_block(67);
		fm_register = init_register;
		write_fm2(fm_register, value);
		z80_block(8);
		pairs_left--;
		if (pairs_left == 0u)
			break;
		z80_cycles(5);
	}
	z80_block(31);
	value = 0xFBu;
	fm_register = 0x24u;
	write_fm1(fm_register, value);
	z80_block(81);
	value = 3u;
	fm_register = 0x25u;
	write_fm1(fm_register, value);
	z80_block(17);
	init_entry = fm_operator_init_ADDRESS;
	pairs_left = 0x24u;
	for (;;)
	{
		z80_block(43);
		init_register = SOUND_ROM8(init_entry);
		init_entry++;
		value = SOUND_ROM8(init_entry);
		init_entry++;
		fm_register = init_register;
		write_fm1(fm_register, value);
		z80_block(67);
		fm_register = init_register;
		write_fm2(fm_register, value);
		z80_block(67);
		fm_register = init_register;
		write_fm1_part2(fm_register, value);
		z80_block(67);
		fm_register = init_register;
		write_fm2_part2(fm_register, value);
		z80_block(8);
		pairs_left--;
		if (pairs_left == 0u)
			break;
		z80_cycles(5);
	}
	z80_block(10);
	leftover_bc = ((uint32_t)pairs_left << 8) | init_register;
	return leftover_bc;
}

uint8_t write_fm_global(uint8_t fm_register, uint8_t value)
{
	uint8_t chip_channel = 0;

	z80_block(39);
	chip_channel = fm_channel;
	if (((chip_channel >> 3) & 1u) == 0u)
	{
		write_fm1(fm_register, value);
		return value;
	}
	z80_block(10);
	write_fm2(fm_register, value);
	return value;
}

void write_fm_channel(uint8_t fm_register, uint8_t value)
{
	uint8_t channel_then_value = 0, saved_value = 0;
	uint8_t fm2_chip_channel = 0, fm1_chip_channel = 0;

	z80_block(35);
	saved_value = value;
	channel_then_value = fm_channel;
	if (((channel_then_value >> 3) & 1u) != 0u)
	{
		z80_block(22);
		fm2_chip_channel = channel_then_value;
		channel_then_value = saved_value;
		if (((fm2_chip_channel >> 2) & 1u) != 0u)
		{
			write_fm2_part2(fm_register, channel_then_value);
			return;
		}
		write_fm2(fm_register, channel_then_value);
		return;
	}
	z80_block(22);
	fm1_chip_channel = channel_then_value;
	channel_then_value = saved_value;
	if (((fm1_chip_channel >> 2) & 1u) != 0u)
	{
		write_fm1_part2(fm_register, channel_then_value);
		return;
	}
	write_fm1(fm_register, channel_then_value);
}

void write_fm1(uint8_t fm_register, uint8_t value)
{
	uint8_t saved_value = 0;

	z80_block(73);
	saved_value = value;
	interrupts_off();
	port_out(FM1_ADDRESS, (uint8_t)fm_register);
	port_out(FM1_DATA, saved_value);
	interrupts_on();
}

void write_fm1_part2(uint8_t fm_register, uint8_t value)
{
	uint8_t saved_value = 0;

	z80_block(73);
	saved_value = value;
	interrupts_off();
	port_out(FM1_PART2_ADDRESS, (uint8_t)fm_register);
	port_out(FM1_PART2_DATA, saved_value);
	interrupts_on();
}

void write_fm2(uint8_t fm_register, uint8_t value)
{
	uint8_t saved_value = 0;

	z80_block(73);
	saved_value = value;
	interrupts_off();
	port_out(FM2_ADDRESS, (uint8_t)fm_register);
	port_out(FM2_DATA, saved_value);
	interrupts_on();
}

void write_fm2_part2(uint8_t fm_register, uint8_t value)
{
	uint8_t saved_value = 0;

	z80_block(73);
	saved_value = value;
	interrupts_off();
	port_out(FM2_PART2_ADDRESS, (uint8_t)fm_register);
	port_out(FM2_PART2_DATA, saved_value);
	interrupts_on();
}

void init_pcm_chip(uint32_t unused_c)
{
	uint8_t bank_pairs_left = 0, init_register = 0, pairs_left = 0, pcm_register = 0, value = 0;
	uint16_t init_entry = 0;
	uint8_t saved_bank_pairs_left = 0;

	z80_block(14);
	value = 0u;
	bank_pairs_left = 8u;
	for (;;)
	{
		z80_block(35);
		saved_bank_pairs_left = bank_pairs_left;
		pcm_register = 7u;
		write_pcm_register(pcm_register, value);
		z80_block(84);
		z80_write8(PCM_WAVE_WINDOW, 0x80u);
		move_block(PCM_WAVE_WINDOW, 0xD001u, 0x1000u, 1u);
		value++;
		pcm_register = 7u;
		write_pcm_register(pcm_register, value);
		z80_block(134);
		z80_write8(PCM_WAVE_WINDOW, 0xFFu);
		move_block(PCM_WAVE_WINDOW, 0xD001u, 0x1000u, 1u);
		z80_write8(PCM_SILENT_LOOP, 0x80u);
		move_block(PCM_SILENT_LOOP, 0xD2BDu, 0x800u, 1u);
		value++;
		bank_pairs_left = saved_bank_pairs_left - 1u;
		if (bank_pairs_left == 0u)
			break;
		z80_cycles(5);
	}
	z80_block(17);
	init_entry = pcm_register_init_ADDRESS;
	pairs_left = 0x29u;
	for (;;)
	{
		z80_block(43);
		init_register = SOUND_ROM8(init_entry);
		init_entry++;
		value = SOUND_ROM8(init_entry);
		init_entry++;
		pcm_register = init_register;
		write_pcm_register(pcm_register, value);
		z80_block(8);
		pairs_left--;
		if (pairs_left == 0u)
			break;
		z80_cycles(5);
	}
	z80_block(10);
}

void write_pcm_register(uint8_t pcm_register, uint8_t register_value)
{
	z80_block(45);
	z80_write8((pcm_register & 0xFFu) | 0xC000u, register_value);
}

void init_track_channels(uint32_t unused_c)
{
	uint8_t voices_left = 0;
	uint16_t code_entry = 0, sample_voice = 0;
	uint8_t saved_voices_left = 0;

	z80_block(27);
	sample_voice = 0xE401u;
	code_entry = voice_chip_channels_ADDRESS;
	voices_left = 0x14u;
	for (;;)
	{
		z80_block(70);
		saved_voices_left = voices_left;
		SAMPLE_VOICE(sample_voice - 0x1u)->chip_channel = SOUND_ROM8(code_entry);
		code_entry++;
		sample_voice += 0x12u;
		voices_left = saved_voices_left - 1u;
		if (voices_left == 0u)
			break;
		z80_cycles(5);
	}
	z80_block(10);
}

void init_pcm_tracks(void)
{
	uint8_t voices_left = 0;
	uint16_t sample_voice = 0;

	z80_block(17);
	sample_voice = 0xE409u;
	voices_left = 8u;
	for (;;)
	{
		z80_block(98);
		SAMPLE_VOICE(sample_voice - 0x9u)->wave_page = 0xD0u;
		sample_voice += 7u;
		AS8(SAMPLE_VOICE(sample_voice - 0x10u)->stream_position) = 0xFFu;
		sample_voice++;
		AT8(SAMPLE_VOICE(sample_voice - 0x11u)->stream_position, 1) = 7u;
		sample_voice += 0xAu;
		voices_left--;
		if (voices_left == 0u)
			break;
		z80_cycles(5);
	}
	z80_block(10);
}

void init_midi_parts(void)
{
	uint8_t factor = 0, value = 0;
	uint16_t count = 0, part = 0, template_then_product = 0;

	z80_block(34);
	value = 1u;
	template_then_product = part_templates_ADDRESS;
	template_then_product = read_table_word(template_then_product, value);
	z80_block(70);
	move_block(template_then_product, MIDI_PART_ADDRESS(0), 0x48u, 1u);
	value = 0u;
	template_then_product = part_templates_ADDRESS;
	template_then_product = read_table_word(template_then_product, value);
	z80_block(57);
	move_block(template_then_product, MIDI_PART_ADDRESS(8), 0x2Du, 1u);
	template_then_product += 0x2Du;
	part = MIDI_PART_ADDRESS(0);
	count = 0xD00u;
	for (;;)
	{
		z80_block(56);
		template_then_product = (template_then_product & 0xFFu) | ((uint32_t)MIDI_PART(part)->volume << 8);
		value = 0xFFu;
		if ((MIDI_PART(part)->flags & 1u) != 0u)
		{
		scale_part_volume:
			z80_block(21);
			factor = value;
			template_then_product = multiply_bytes(factor, template_then_product);
			z80_block(18);
			value = (uint8_t)(template_then_product >> 8);
			if (value != 0u)
			{
				z80_block(4);
				value++;
			}
			z80_block(52);
			MIDI_PART(part)->effective_volume = value;
			part += 9u;
			count = (uint32_t)(uint8_t)((count >> 8) - 1u) << 8;
			if (count >> 8 == 0u)
				break;
			z80_cycles(5);
			continue;
		}
		z80_block(13);
		value = music_volume;
		goto scale_part_volume;
	}
	z80_block(10);
}

uint16_t read_table_word(uint16_t table, uint8_t index)
{
	uint16_t offset = 0, word = 0;

	z80_block(88);
	offset = index;
	table = table + offset + offset;
	word = z80_read16(table);
	return word;
}

void handle_midi_message(void)
{
	uint8_t parts_left = 0, status = 0;
	uint16_t channel_and_index = 0, ignored16 = 0, part = 0, parts_left_and_byte = 0;
	uint8_t saved_parts_left = 0, saved_parts_left_and_byte = 0;
	uint16_t saved_channel_and_index = 0;

	z80_block(26);
	channel_and_index = midi_queue_count_ADDRESS;
	if (midi_queue_count == 0u)
	{
		z80_cycles(6);
		return;
	}
	z80_block(28);
	midi_queue_count--;
	read_midi_byte(&parts_left_and_byte, &ignored16, &status);
	z80_block(18);
	if (status >> 7 == 0u)
	{
		z80_block(13);
		status = midi_status;
	}
	else
	{
		z80_block(23);
		midi_status = status;
	}
	z80_block(49);
	channel_and_index = ((status & 0xFu) << 8) | midi_queue_read;
	part = MIDI_PART_ADDRESS(0);
	parts_left = 0xDu;
	for (;;)
	{
		z80_block(30);
		if (MIDI_PART(part)->flags >> 7 == 0u)
		{
			z80_block(33);
			part += 9u;
			parts_left--;
			if (parts_left == 0u)
				break;
			z80_cycles(5);
			continue;
		}
		z80_block(33);
		if (channel_and_index >> 8 == MIDI_PART(part)->midi_channel)
		{
			z80_block(39);
			saved_parts_left = parts_left;
			saved_parts_left_and_byte = (uint8_t)parts_left_and_byte;
			saved_channel_and_index = channel_and_index;
			play_midi_message(parts_left_and_byte, part);
			z80_block(37);
			channel_and_index = saved_channel_and_index;
			parts_left_and_byte = saved_parts_left_and_byte;
			parts_left = saved_parts_left;
			midi_queue_read = (uint8_t)channel_and_index;
		}
		z80_block(33);
		part += 9u;
		parts_left--;
		if (parts_left == 0u)
			break;
		z80_cycles(5);
	}
	skip_midi_data((uint8_t)parts_left_and_byte);
}

void skip_midi_data(uint8_t first_byte)
{
	uint8_t command = 0;
	uint16_t read_index_address = 0;

	z80_block(28);
	read_index_address = midi_queue_read_ADDRESS;
	if ((uint8_t)first_byte >> 7 != 0u)
	{
		z80_block(22);
		command = first_byte & 0xF0u;
		midi_queue_read++;
	}
	else
	{
		z80_block(30);
		command = midi_status & ~0xFu;
	}
	z80_block(12);
	if (command == 0xC0u)
	{
		z80_cycles(6);
		return;
	}
	z80_block(12);
	if (command == 0xD0u)
	{
		z80_cycles(6);
		return;
	}
	z80_block(21);
	midi_queue_read++;
}

void play_midi_message(uint16_t message_byte, uint16_t part)
{
	uint32_t carry = 0;
	uint8_t bend = 0, bend_high = 0, bend_low = 0, bend_sign = 0, bend_steps = 0, channel = 0, channel_index = 0, channels_left = 0, chip_channel = 0, control_register = 0, control_value = 0, controller = 0, controller_number = 0, controls = 0, count_or_channel = 0, depth = 0, detune = 0, detune_low = 0, detune_sign = 0, effective_volume = 0, event = 0, first_byte = 0, first_data_byte = 0, flags = 0, fm_detune = 0, fm_register = 0, ignored8 = 0, keys_on = 0, level = 0, level_register = 0, lowest_order = 0, modulation_depth = 0, note = 0, note_number = 0, note_to_mark = 0, octave = 0, old_controls = 0, operators_left = 0, order = 0, part_channel = 0, parts_left = 0, pcm_register = 0, pitch_bend_high = 0, pitch_bend_low = 0, preset = 0, program = 0, register_number = 0, register_value = 0, rounded_volume = 0, sample = 0, scale = 0, setup = 0, total_level = 0, velocity = 0, velocity_then_steal_low = 0, vibrato_preset = 0, voice_channel = 0, voice_chip_channel = 0, voice_note = 0, voices_left = 0, voices_or_bytes_left = 0, voices_unchecked = 0, volume = 0, volume_curve = 0, volume_scale = 0;
	uint16_t bend_entry = 0, bend_index = 0, channel_in_part = 0, channel_offset = 0, controls_entry = 0, each_part = 0, fnum = 0, fnum_entry = 0, fnum_table_entry = 0, free_fm_voice = 0, free_voice = 0, ignored16 = 0, instrument = 0, kept_channel = 0, kit_note = 0, level_product = 0, new_voice = 0, new_voice_field = 0, note_index = 0, order_entry = 0, patch_levels = 0, pitch = 0, pitch_entry = 0, pitch_table_entry = 0, preset_address = 0, preset_data = 0, preset_target = 0, register_channel = 0, register_list = 0, scaled_part_volume = 0, setup_data = 0, velocity_index = 0, voice = 0, voice_field = 0, voice_size = 0, voices_left_pair = 0, voices_to_end_at = 0;
	uint8_t saved_channel = 0, saved_count = 0, saved_value = 0, saved_voices_left = 0;
	uint16_t saved_outer = 0, saved_inner = 0;
	uint8_t old_voices_to_end = 0, note_level_byte = 0, level_rotated = 0, fm_detune_byte = 0, sample_detune_byte = 0, octave_bits = 0, negated_bend = 0, negated_steps = 0, modulation_value = 0, modulation_rotated_1 = 0, modulation_rotated_2 = 0, doubled_volume = 0, doubled_music_volume = 0;

	first_data_byte = (uint8_t)message_byte;
	z80_block(37);
	event = midi_status & ~0xFu;
	if (event == 0x90u)
	{
		z80_block(18);
		if (first_data_byte >> 7)
		{
			z80_block(17);
			read_midi_byte(&message_byte, &ignored16, &ignored8);
			first_data_byte = (uint8_t)message_byte;
		}
		z80_block(96);
		note_number = first_data_byte;
		note_code = note_codes[(uint8_t)(MIDI_PART(part)->transpose + first_data_byte)];
		read_midi_byte(&message_byte, &ignored16, &velocity);
		velocity_then_steal_low = (uint8_t)message_byte;
		z80_block(14);
		if (velocity == 0u)
		{
		note_off:
			z80_block(31);
			program = MIDI_PART(part)->program;
			if (program >= 0xF0u)
			{
				z80_cycles(6);
				return;
			}
			z80_block(27);
			instrument = instruments_ADDRESS;
			instrument = read_table_word(instrument, program);
			z80_block(22);
			if (((z80_read8(instrument) >> 1) & 1u) != 0u)
			{
				fm_note_off(&ignored16, &instrument, &ignored8);
				return;
			}
			z80_block(51);
			voice = SAMPLE_VOICE_ADDRESS(0);
			voice_size = 0x12u;
			voices_left = 8u;
			channel = midi_status & 0xFu;
			for (;;)
			{
				z80_block(29);
				if (channel != SAMPLE_VOICE(voice)->midi_channel)
				{
				note_off_next_voice:
					z80_block(23);
					voice += voice_size;
					voices_left--;
					if (voices_left == 0u)
					{
						z80_block(10);
						return;
					}
					z80_cycles(5);
					continue;
				}
				z80_block(46);
				channel_index = channel;
				if (note_code != SAMPLE_VOICE(voice)->note)
				{
				note_off_not_this:
					z80_block(4);
					channel = (uint8_t)channel_index;
					goto note_off_next_voice;
				}
				z80_block(37);
				flags = SAMPLE_VOICE(voice)->flags;
				if (((flags >> 1) & 1u) != 0u)
					goto note_off_not_this;
				z80_block(17);
				if ((flags & 0x7Fu) != 0u)
					break;
				goto note_off_not_this;
			}
			z80_block(65);
			if (channel_controls[channel_index] >> 7 == 0u)
			{
				stop_pcm_voice(voice);
				return;
			}
			z80_block(33);
			SAMPLE_VOICE(voice)->flags |= 2u;
			return;
		}
		z80_block(88);
		velocity_index = velocity;
		note_level = SOUND_ROM8(velocity_levels_ADDRESS + velocity_index);
		program = MIDI_PART(part)->program;
		if (program < 0xF0u)
		{
			z80_block(27);
			instrument = instruments_ADDRESS;
			instrument = read_table_word(instrument, program);
			z80_block(22);
			if (((z80_read8(instrument) >> 1) & 1u) != 0u)
			{
				z80_block(76);
				sample_or_patch = z80_read8((uint16_t)(instrument + 1u));
				free_fm_voice = next_fm_voice;
				voice_size = 0x12u;
				voices_left = fm_voices_to_end;
				voices_unchecked = 0xCu;
				for (;;)
				{
					z80_block(21);
					if (FM_VOICE(free_fm_voice)->flags == 0u)
						goto fm_voice_found;
					z80_block(23);
					voices_unchecked--;
					free_fm_voice += voice_size;
					voices_left--;
					if (voices_left == 0u)
						break;
					z80_cycles(5);
				}
				z80_block(18);
				if (voices_unchecked == 0u)
				{
					z80_block(14);
					return;
				}
				z80_block(24);
				free_fm_voice = FM_VOICE_ADDRESS(0);
				voice_size = 0x12u;
				voices_left = voices_unchecked;
				for (;;)
				{
					z80_block(21);
					if (FM_VOICE(free_fm_voice)->flags == 0u)
						break;
					z80_block(23);
					free_fm_voice += voice_size;
					voices_left--;
					if (voices_left == 0u)
					{
						z80_block(14);
						return;
					}
					z80_cycles(5);
				}
			fm_voice_found:
				z80_block(89);
				new_fm_voice = free_fm_voice;
				vibrato_preset = channel_vibrato_presets[midi_status & 0xFu];
				if (vibrato_preset != 0u)
				{
					z80_block(27);
					preset_address = vibrato_presets_ADDRESS;
					preset_address = read_table_word(preset_address, vibrato_preset);
					z80_block(353);
					preset_data = preset_address;
					preset_target = new_fm_voice + 7u;
					FM_VOICE(preset_target - 0x7u)->vibrato_delay = z80_read8(preset_data);
					preset_data++;
					preset_target++;
					FM_VOICE(preset_target - 0x8u)->vibrato_wait = z80_read8(preset_data);
					preset_data++;
					preset_target++;
					FM_VOICE(preset_target - 0x9u)->vibrato_range = z80_read8(preset_data);
					preset_data++;
					preset_target++;
					FM_VOICE(preset_target - 0xAu)->vibrato_depth = z80_read8(preset_data);
					preset_data++;
					preset_target++;
					FM_VOICE(preset_target - 0xBu)->vibrato_rise = z80_read8(preset_data);
					preset_data++;
					preset_target++;
					FM_VOICE(preset_target - 0xCu)->vibrato_fall = z80_read8(preset_data);
					FM_VOICE(preset_target - 0xCu)->vibrato_phase = z80_read8((uint16_t)(preset_data + 1u));
					new_voice = new_fm_voice;
					FM_VOICE(new_voice)->vibrato_phase += FM_VOICE(new_voice)->vibrato_range & ~0xFu;
					FM_VOICE(new_voice)->vibrato_rate = FM_VOICE(new_voice)->vibrato_wait;
				}
				z80_block(31);
				voices_to_end_at = fm_voices_to_end_ADDRESS;
				old_voices_to_end = fm_voices_to_end;
				fm_voices_to_end = old_voices_to_end - 1u;
				if (old_voices_to_end != 1u)
				{
					z80_block(53);
					next_fm_voice += 0x12u;
				}
				else
				{
					z80_block(46);
					fm_voices_to_end = 0xCu;
					next_fm_voice = FM_VOICE_ADDRESS(0);
				}
				z80_block(218);
				new_voice_field = new_fm_voice;
				FM_VOICE(new_voice_field)->flags = 0x40u;
				new_voice_field++;
				fm_channel = FM_VOICE(new_voice_field - 0x1u)->chip_channel;
				new_voice_field++;
				note = note_code;
				FM_VOICE(new_voice_field - 0x2u)->note = note;
				voice_note = note;
				new_voice_field++;
				channel = midi_status & 0xFu;
				FM_VOICE(new_voice_field - 0x3u)->midi_channel = channel;
				voice_channel = channel;
				new_voice_field += 2u;
				note_level_byte = note_level;
				carry = note_level_byte & 1u;
				level_rotated = note_level_byte >> 1;
				level = (level_rotated >> 1) | (carry << 7);
				carry = level_rotated & 1u;
				level = (((level >> 1) | (carry << 7)) & 0x1Fu) - 0x20u;
				FM_VOICE(new_voice_field - 0x5u)->level = level;
				level_product = (new_voice_field & 0xFFu) | ((uint32_t)level << 8);
				effective_volume = MIDI_PART(part)->effective_volume;
				volume_curve = effective_volume;
				if (effective_volume != 0u)
				{
					z80_block(20);
					volume_curve += (uint8_t)~effective_volume >> 1;
				}
				z80_block(17);
				level_product = multiply_bytes(volume_curve, level_product);
				z80_block(425);
				note_level = 0x7Fu - ((uint16_t)((uint32_t)((level_product >> 8) >> 1) << 8) >> 8);
				channel_offset = voice_channel;
				fm_pan_modulation = (z80_read8((uint16_t)(channel_controls_ADDRESS + channel_offset)) & 7u) | 0xC0u;
				bend_entry = channel_pitch_bends_ADDRESS + channel_offset + channel_offset;
				fm_detune = MIDI_PART(part)->fm_detune;
				detune_low = fm_detune;
				fm_detune_byte = fm_detune;
				detune_sign = ((uint32_t)fm_detune_byte << 1) | (fm_detune_byte >> 7);
				carry = fm_detune_byte >> 7;
				saved_outer = z80_read16(bend_entry) + (detune_low | ((uint32_t)(uint8_t)(0u - carry) << 8));
				fnum_table_entry = fm_fnum_tables_ADDRESS + (uint8_t)((voice_note & 0xFu) * 2u);
				bend_index = saved_outer;
				fnum_entry = SOUND_ROM16(fnum_table_entry) + bend_index;
				carry = (uint16_t)(fnum_entry + bend_index) < fnum_entry;
				fnum_entry += bend_index;
				fnum = z80_read16(fnum_entry);
				fnum = (fnum & 0xFFu) | ((uint32_t)(uint8_t)((((voice_note >> 1) | (carry << 7)) & 0x38u) + (fnum >> 8)) << 8);
				register_channel = (fm_channel & 3u) << 8;
				register_value = (uint8_t)(fnum >> 8);
				fm_register = 0xA4u | (register_channel >> 8);
				write_fm_channel(fm_register, register_value);
				z80_block(36);
				register_value = (uint8_t)fnum;
				fm_register = 0xA0u | (register_channel >> 8);
				write_fm_channel(fm_register, register_value);
				z80_block(17);
				load_fm_patch(&channel_in_part, &register_list, &patch_levels);
				part_channel = (uint8_t)channel_in_part;
				z80_block(7);
				operators_left = 4u;
				for (;;)
				{
					z80_block(63);
					saved_count = operators_left;
					saved_value = part_channel;
					register_number = z80_read8(register_list) + part_channel;
					register_list++;
					level_register = register_number;
					total_level = z80_read8(patch_levels);
					patch_levels++;
					if (total_level >> 7 == 0u)
					{
						z80_block(21);
						total_level += note_level;
					}
					z80_block(24);
					total_level &= 0x7Fu;
					fm_register = level_register;
					write_fm_channel(fm_register, total_level);
					z80_block(18);
					part_channel = saved_value;
					operators_left = saved_count - 1u;
					if (operators_left == 0u)
						break;
					z80_cycles(5);
				}
				z80_block(45);
				register_value = fm_pan_modulation;
				fm_register = (uint8_t)(0xB4u + part_channel);
				write_fm_channel(fm_register, register_value);
				z80_block(44);
				register_value = (fm_channel & 7u) | 0xF0u;
				fm_register = 0x28u;
				register_value = write_fm_global(fm_register, register_value);
				return;
			}
			z80_block(84);
			instrument++;
			sample_or_patch = z80_read8(instrument);
			instrument += 2u;
			note_pan = z80_read8(instrument);
			sample_detune = z80_read8((uint16_t)(instrument + 1u));
		allocate_sample_voice:
			z80_block(31);
			free_voice = SAMPLE_VOICE_ADDRESS(0);
			voice_size = 0x12u;
			count_or_channel = 8u;
			lowest_order = 0u;
			for (;;)
			{
				z80_block(17);
				if (lowest_order == SAMPLE_VOICE(free_voice)->flags)
				{
					z80_block(25);
					saved_outer = free_voice;
					new_voice = saved_outer;
					goto start_sample_voice;
				}
				z80_block(19);
				free_voice += voice_size;
				count_or_channel--;
				if (count_or_channel == 0u)
					break;
				z80_cycles(5);
			}
			z80_block(21);
			order_entry = voice_0_allocation_order_ADDRESS;
			count_or_channel = 8u;
			lowest_order--;
			for (;;)
			{
				z80_block(17);
				if (SAMPLE_VOICE(order_entry - 0x6u)->allocation_order <= lowest_order)
				{
					z80_block(11);
					lowest_order = SAMPLE_VOICE(order_entry - 0x6u)->allocation_order;
					velocity_then_steal_low = (uint8_t)order_entry;
				}
				z80_block(19);
				order_entry += voice_size;
				count_or_channel--;
				if (count_or_channel == 0u)
					break;
				z80_cycles(5);
			}
			z80_block(333);
			new_voice = 0xFFFAu + ((order_entry & ~0xFFu) | velocity_then_steal_low);
			voice_0_allocation_order--;
			voice_1_allocation_order--;
			voice_2_allocation_order--;
			voice_3_allocation_order--;
			voice_4_allocation_order--;
			voice_5_allocation_order--;
			voice_6_allocation_order--;
			voice_7_allocation_order--;
			pcm_voices_used = 7u;
			keys_on = pcm_channels_on ^ pcm_channel_bits[SAMPLE_VOICE(new_voice)->chip_channel];
			pcm_channels_on = keys_on;
			keys_on = ~keys_on;
			pcm_register = 8u;
			write_pcm_register(pcm_register, keys_on);
			z80_block(67);
			SAMPLE_VOICE(new_voice)->wave_page = 0xD0u;
			AS8(SAMPLE_VOICE(new_voice)->stream_position) = 0xFFu;
			AT8(SAMPLE_VOICE(new_voice)->stream_position, 1) = 7u;
		start_sample_voice:
			z80_block(36);
			if (MIDI_PART(part)->program < 0xF0u)
			{
				z80_block(63);
				register_value = SAMPLE_VOICE(new_voice)->chip_channel;
				pcm_note_channel = register_value;
				register_value |= 0xC0u;
				control_register = 7u;
				pcm_register = 7u;
				write_pcm_register(pcm_register, register_value);
				z80_block(35);
				register_value = ((register_value & 7u) * 2u) | 0x80u;
				pcm_register = control_register;
				write_pcm_register(pcm_register, register_value);
				z80_block(47);
				register_value = ((uint32_t)register_value << 1) | (register_value >> 7);
				register_value = ((uint32_t)register_value << 1) | (register_value >> 7);
				register_value = ((uint32_t)register_value << 1) | (register_value >> 7);
				register_value = (((uint32_t)register_value << 1) | (register_value >> 7)) & 0xF0u;
				pcm_register = 5u;
				write_pcm_register(pcm_register, register_value);
				z80_block(24);
				pcm_register = 6u;
				write_pcm_register(pcm_register, register_value);
				z80_block(177);
				note = note_code;
				SAMPLE_VOICE(new_voice)->note = note;
				voice_note = note;
				channel = midi_status & 0xFu;
				SAMPLE_VOICE(new_voice)->midi_channel = channel;
				count_or_channel = channel;
				SAMPLE_VOICE(new_voice)->level = note_level;
				order = pcm_voices_used;
				SAMPLE_VOICE(new_voice)->allocation_order = order;
				pcm_voices_used = order + 1u;
				start_pcm_sample(new_voice);
				z80_block(282);
				SAMPLE_VOICE(new_voice)->flags = 0x40u;
				bend_entry = channel_pitch_bends_ADDRESS + (uint8_t)(count_or_channel * 2u);
				detune = sample_detune;
				detune_low = detune;
				sample_detune_byte = detune;
				detune_sign = ((uint32_t)sample_detune_byte << 1) | (sample_detune_byte >> 7);
				carry = sample_detune_byte >> 7;
				saved_outer = z80_read16(bend_entry) + (detune_low | ((uint32_t)(uint8_t)(0u - carry) << 8));
				pitch_table_entry = pcm_pitch_tables_ADDRESS + (uint8_t)((voice_note & 0xFu) * 2u);
				pitch_entry = SOUND_ROM16(pitch_table_entry) + saved_outer + saved_outer;
				pitch = z80_read16(pitch_entry);
				octave_bits = voice_note & ~0xFu;
				octave = octave_bits;
				if (octave_bits != 0u)
				{
					z80_block(33);
					octave = (octave >> 1) | ((uint32_t)octave << 7);
					octave = (octave >> 1) | ((uint32_t)octave << 7);
					octave = (octave >> 1) | ((uint32_t)octave << 7);
					octave = (octave >> 1) | ((uint32_t)octave << 7);
					if (octave < 8u)
					{
						z80_block(4);
						count_or_channel = octave;
						for (;;)
						{
							z80_block(24);
							carry = (uint8_t)pitch >> 7;
							pitch = (pitch & ~0xFFu) | (uint8_t)((uint32_t)pitch << 1);
							pitch = (pitch & 0xFFu) | ((uint32_t)(uint8_t)(((uint32_t)(pitch >> 8) << 1) | carry) << 8);
							count_or_channel--;
							if (count_or_channel == 0u)
								break;
							z80_cycles(5);
						}
					}
					else
					{
						z80_block(19);
						count_or_channel = (~octave + 1u) & 0xFu;
						for (;;)
						{
							z80_block(24);
							carry = (pitch >> 8) & 1u;
							pitch = (pitch & 0xFFu) | ((uint32_t)((pitch >> 8) >> 1) << 8);
							pitch = (pitch & ~0xFFu) | (uint8_t)(((uint8_t)pitch >> 1) | (carry << 7));
							count_or_channel--;
							if (count_or_channel == 0u)
								break;
							z80_cycles(5);
						}
						z80_block(10);
					}
				}
			}
			else
			{
				z80_block(63);
				register_value = SAMPLE_VOICE(new_voice)->chip_channel;
				pcm_note_channel = register_value;
				register_value |= 0xC0u;
				control_register = 7u;
				pcm_register = 7u;
				write_pcm_register(pcm_register, register_value);
				z80_block(35);
				register_value = ((register_value & 7u) * 2u) | 0x80u;
				pcm_register = control_register;
				write_pcm_register(pcm_register, register_value);
				z80_block(47);
				register_value = ((uint32_t)register_value << 1) | (register_value >> 7);
				register_value = ((uint32_t)register_value << 1) | (register_value >> 7);
				register_value = ((uint32_t)register_value << 1) | (register_value >> 7);
				register_value = (((uint32_t)register_value << 1) | (register_value >> 7)) & 0xF0u;
				pcm_register = 5u;
				write_pcm_register(pcm_register, register_value);
				z80_block(24);
				pcm_register = 6u;
				write_pcm_register(pcm_register, register_value);
				z80_block(140);
				SAMPLE_VOICE(new_voice)->note = note_code;
				SAMPLE_VOICE(new_voice)->midi_channel = midi_status & 0xFu;
				SAMPLE_VOICE(new_voice)->level = note_level;
				if ((midi_status & 0xFu) == 0xFu)
				{
					z80_block(49);
					order = pcm_voices_used;
					SAMPLE_VOICE(new_voice)->allocation_order = order;
					pcm_voices_used = order + 1u;
				}
				else
				{
					z80_block(73);
					order = pcm_voices_used | 0xF0u;
					SAMPLE_VOICE(new_voice)->allocation_order = order;
					pcm_voices_used = (order & 0xFu) + 1u;
				}
				z80_block(17);
				start_pcm_sample(new_voice);
				z80_block(63);
				SAMPLE_VOICE(new_voice)->flags = 0x50u;
				pitch = AS16(kit_pitch_low);
			}
			z80_block(28);
			register_value = (uint8_t)(pitch >> 8);
			pcm_register = 3u;
			write_pcm_register(pcm_register, register_value);
			z80_block(28);
			register_value = (uint8_t)pitch;
			pcm_register = 2u;
			write_pcm_register(pcm_register, register_value);
			z80_block(91);
			SAMPLE_VOICE(new_voice)->pitch_step = pitch;
			level_product = (pitch & 0xFFu) | ((uint32_t)note_level << 8);
			scale = MIDI_PART(part)->effective_volume;
			level_product = multiply_bytes(scale, level_product);
			z80_block(28);
			register_value = (uint8_t)(level_product >> 8);
			pcm_register = 0u;
			write_pcm_register(pcm_register, register_value);
			z80_block(37);
			register_value = note_pan;
			pcm_register = 1u;
			write_pcm_register(pcm_register, register_value);
			z80_block(99);
			keys_on = pcm_channels_on | pcm_channel_bits[pcm_note_channel];
			pcm_channels_on = keys_on;
			keys_on = ~keys_on;
			pcm_register = 8u;
			write_pcm_register(pcm_register, keys_on);
			return;
		}
		z80_block(34);
		program += 0x10u;
		kit_note = drum_kits_ADDRESS;
		kit_note = read_table_word(kit_note, program);
		z80_block(158);
		note_index = note_number;
		kit_note = kit_note + note_index + note_index + note_index + note_index;
		kit_pitch_low = z80_read8(kit_note);
		kit_note++;
		kit_pitch_high = z80_read8(kit_note);
		kit_note++;
		note_pan = z80_read8(kit_note);
		sample = z80_read8((uint16_t)(kit_note + 1u));
		sample_or_patch = sample;
		if (sample == 0xFFu)
		{
			z80_cycles(6);
			return;
		}
		z80_block(10);
		goto allocate_sample_voice;
	}
	z80_block(17);
	if (event == 0x80u)
	{
		z80_block(18);
		if (first_data_byte >> 7)
		{
			z80_block(17);
			read_midi_byte(&message_byte, &ignored16, &ignored8);
			first_data_byte = (uint8_t)message_byte;
		}
		z80_block(96);
		note_code = note_codes[(uint8_t)(MIDI_PART(part)->transpose + first_data_byte)];
		midi_queue_read++;
		goto note_off;
	}
	z80_block(17);
	if (event == 0xE0u)
	{
		z80_block(18);
		if (first_data_byte >> 7)
		{
			z80_block(17);
			read_midi_byte(&message_byte, &ignored16, &ignored8);
			first_data_byte = (uint8_t)message_byte;
		}
		z80_block(21);
		bend_low = first_data_byte;
		read_midi_byte(&message_byte, &ignored16, &bend_high);
		z80_block(26);
		carry = bend_high & 1u;
		bend_sign = bend_high >> 1;
		bend = bend_low;
		if (carry)
		{
			z80_block(7);
			bend |= 0x80u;
		}
		z80_block(78);
		carry = bend_sign & 1u;
		bend_sign = bend_sign >> 1;
		bend = (bend >> 1) | (carry << 7);
		carry = bend_sign & 1u;
		bend_sign = bend_sign >> 1;
		bend = (bend >> 1) | (carry << 7);
		carry = bend_sign & 1u;
		bend_sign = bend_sign >> 1;
		bend = (bend >> 1) | (carry << 7);
		carry = bend_sign & 1u;
		bend_sign = bend_sign >> 1;
		bend = (bend >> 1) | (carry << 7);
		carry = bend_sign & 1u;
		bend_sign = bend_sign >> 1;
		bend = (bend >> 1) | (carry << 7);
		if ((bend_sign & 1u) == 0u)
		{
			z80_block(18);
			negated_bend = ~bend + 1u;
			bend = negated_bend;
			if (negated_bend == 0u)
			{
				z80_block(4);
				bend--;
			}
		}
		z80_block(57);
		bend_steps = pitch_bend_steps[bend];
		carry = bend_sign & 1u;
		bend_sign = bend_sign >> 1;
		if (carry == 0u)
		{
			z80_block(22);
			bend_sign--;
			negated_steps = ~bend_steps + 1u;
			bend_steps = negated_steps;
			if (negated_steps == 0u)
			{
				z80_block(4);
				bend_steps--;
			}
		}
		z80_block(145);
		pitch_bend_high = bend_sign;
		pitch_bend_low = bend_steps;
		pitch_bend = ((uint32_t)pitch_bend_high << 8) | pitch_bend_low;
		bend_entry = channel_pitch_bends_ADDRESS + (uint8_t)((midi_status & 0xFu) * 2u);
		z80_write8(bend_entry, pitch_bend_low);
		z80_write8((uint16_t)(bend_entry + 1u), pitch_bend_high);
		voice = SAMPLE_VOICE_ADDRESS(0);
		voices_left = 0x14u;
		channel = midi_status & 0xFu;
		for (;;)
		{
			z80_block(29);
			if (channel != SAMPLE_VOICE(voice)->midi_channel)
			{
				z80_block(33);
				voice += 0x12u;
				voices_left--;
				if (voices_left == 0u)
					break;
				z80_cycles(5);
				continue;
			}
			z80_block(47);
			saved_channel = channel;
			if ((SAMPLE_VOICE(voice)->flags & 0x7Fu) != 0u)
			{
				z80_cycles(7);
				voices_left_pair = ((uint32_t)voices_left << 8) | pitch_bend_low;
				voices_left_pair = retune_voice(voices_left_pair, part, voice);
				voices_left = (uint8_t)(voices_left_pair >> 8);
				pitch_bend_low = (uint8_t)voices_left_pair;
			}
			z80_block(10);
			channel = saved_channel;
			z80_block(33);
			voice += 0x12u;
			voices_left--;
			if (voices_left == 0u)
				break;
			z80_cycles(5);
		}
		z80_block(10);
		return;
	}
	z80_block(17);
	if (event == 0xB0u)
	{
		z80_block(18);
		if (first_data_byte >> 7)
		{
			z80_block(17);
			read_midi_byte(&message_byte, &ignored16, &ignored8);
			first_data_byte = (uint8_t)message_byte;
		}
		z80_block(21);
		controller_number = first_data_byte;
		read_midi_byte(&message_byte, &voice_field, &ignored8);
		control_value = (uint8_t)message_byte;
		z80_block(21);
		controller = controller_number;
		carry = controller == 0u;
		if (controller == 1u)
		{
			z80_block(123);
			modulation_value = control_value;
			depth = (modulation_value >> 1) | (carry << 7);
			carry = modulation_value & 1u;
			modulation_rotated_1 = depth;
			depth = (modulation_rotated_1 >> 1) | (carry << 7);
			carry = modulation_rotated_1 & 1u;
			modulation_rotated_2 = depth;
			depth = (modulation_rotated_2 >> 1) | (carry << 7);
			carry = modulation_rotated_2 & 1u;
			depth = ((depth >> 1) | (carry << 7)) & 7u;
			modulation_depth = depth;
			fm_pan_modulation = depth;
			controls_entry = channel_controls_ADDRESS + (midi_status & 0xFu);
			controls = z80_read8(controls_entry);
			old_controls = controls;
			if ((controls & 0x7Fu) == modulation_depth)
			{
				z80_cycles(6);
				return;
			}
			z80_block(59);
			z80_write8(controls_entry, (old_controls & 0x80u) | modulation_depth);
			voice_field = 0xE403u;
			voices_left = 0x14u;
			channel = midi_status & 0xFu;
			for (;;)
			{
				z80_block(17);
				if (channel != z80_read8(voice_field))
				{
					z80_block(29);
					voice_field += 0x12u;
					voices_left--;
					if (voices_left == 0u)
						break;
					z80_cycles(5);
					continue;
				}
				z80_block(46);
				kept_channel = (uint32_t)channel << 8;
				voice_field = voice_field - 1u - 1u - 1u;
				if ((z80_read8(voice_field) & 0x7Fu) != 0u)
				{
					z80_cycles(7);
					voices_left_pair = (uint32_t)voices_left << 8;
					update_fm_modulation(&voices_left_pair, &voice_field);
					voices_left = (uint8_t)(voices_left_pair >> 8);
				}
				z80_block(22);
				voice_field += 3u;
				channel = (uint8_t)(kept_channel >> 8);
				z80_block(29);
				voice_field += 0x12u;
				voices_left--;
				if (voices_left == 0u)
					break;
				z80_cycles(5);
			}
			z80_block(10);
			return;
		}
		z80_block(17);
		if (controller == 7u)
		{
			z80_block(18);
			doubled_volume = control_value * 2u;
			volume = doubled_volume;
			if (doubled_volume != 0u)
			{
				z80_block(4);
				volume++;
			}
			z80_block(60);
			MIDI_PART(part)->volume = volume;
			scaled_part_volume = (voice_field & 0xFFu) | ((uint32_t)volume << 8);
			volume_scale = 0xFFu;
			if ((MIDI_PART(part)->flags & 1u) == 0u)
			{
				z80_block(13);
				volume_scale = music_volume;
			}
			z80_block(21);
			scale = volume_scale;
			scaled_part_volume = multiply_bytes(scale, scaled_part_volume);
			z80_block(18);
			rounded_volume = (uint8_t)(scaled_part_volume >> 8);
			if (rounded_volume != 0u)
			{
				z80_block(4);
				rounded_volume++;
			}
			z80_block(36);
			MIDI_PART(part)->effective_volume = rounded_volume;
			voice_field = SAMPLE_VOICE_ADDRESS(0);
			voices_left = 0x14u;
			for (;;)
			{
				z80_block(33);
				saved_outer = voice_field;
				if (((z80_read8(voice_field) >> 6) & 1u) == 0u)
				{
					z80_block(39);
					voice_field = saved_outer + 0x12u;
					voices_left--;
					if (voices_left == 0u)
						break;
					z80_cycles(5);
					continue;
				}
				z80_block(55);
				voice_field += 3u;
				if ((midi_status & 0xFu) == z80_read8(voice_field))
				{
					z80_block(37);
					voice_field = voice_field - 1u - 1u;
					voice_chip_channel = z80_read8(voice_field);
					if (voice_chip_channel >> 7 == 0u)
					{
						z80_block(35);
						register_value = voice_chip_channel | 0xC0u;
						pcm_register = 7u;
						write_pcm_register(pcm_register, register_value);
						z80_block(64);
						voice_field = ((voice_field + 4u) & 0xFFu) | ((uint32_t)z80_read8((uint16_t)(voice_field + 4u)) << 8);
						scale = MIDI_PART(part)->effective_volume;
						voice_field = multiply_bytes(scale, voice_field);
						z80_block(28);
						register_value = (uint8_t)(voice_field >> 8);
						pcm_register = 0u;
						write_pcm_register(pcm_register, register_value);
					}
					else
					{
						z80_block(17);
						chip_channel = voice_chip_channel;
						apply_fm_channel_volume(chip_channel, voice_field, part);
						z80_block(10);
					}
				}
				z80_block(39);
				voice_field = saved_outer + 0x12u;
				voices_left--;
				if (voices_left == 0u)
					break;
				z80_cycles(5);
			}
			z80_block(10);
			return;
		}
		z80_block(17);
		if (controller == 0x10u)
		{
			z80_block(18);
			doubled_music_volume = control_value * 2u;
			volume = doubled_music_volume;
			if (doubled_music_volume != 0u)
			{
				z80_block(4);
				volume++;
			}
			z80_block(34);
			music_volume = volume;
			each_part = MIDI_PART_ADDRESS(0);
			parts_left = 8u;
			for (;;)
			{
				z80_block(67);
				saved_count = parts_left;
				scaled_part_volume = (voice_field & 0xFFu) | ((uint32_t)MIDI_PART(each_part)->volume << 8);
				volume_scale = 0xFFu;
				if ((MIDI_PART(each_part)->flags & 1u) != 0u)
				{
				music_volume_scale_part:
					z80_block(21);
					scale = volume_scale;
					scaled_part_volume = multiply_bytes(scale, scaled_part_volume);
					z80_block(18);
					rounded_volume = (uint8_t)(scaled_part_volume >> 8);
					if (rounded_volume != 0u)
					{
						z80_block(4);
						rounded_volume++;
					}
					z80_block(36);
					MIDI_PART(each_part)->effective_volume = rounded_volume;
					voice_field = 0xE403u;
					voices_left = 0x14u;
					for (;;)
					{
						z80_block(51);
						saved_inner = voice_field;
						if (MIDI_PART(each_part)->midi_channel == z80_read8(voice_field))
						{
							z80_block(40);
							voice_field = voice_field - 1u - 1u - 1u;
							if ((z80_read8(voice_field) >> 6) & 1u)
							{
								z80_block(31);
								voice_field++;
								voice_chip_channel = z80_read8(voice_field);
								if (voice_chip_channel >> 7 == 0u)
								{
									z80_block(35);
									register_value = voice_chip_channel | 0xC0u;
									pcm_register = 7u;
									write_pcm_register(pcm_register, register_value);
									z80_block(64);
									voice_field = ((voice_field + 4u) & 0xFFu) | ((uint32_t)z80_read8((uint16_t)(voice_field + 4u)) << 8);
									scale = MIDI_PART(each_part)->effective_volume;
									voice_field = multiply_bytes(scale, voice_field);
									z80_block(28);
									register_value = (uint8_t)(voice_field >> 8);
									pcm_register = 0u;
									write_pcm_register(pcm_register, register_value);
								}
								else
								{
									z80_block(17);
									chip_channel = voice_chip_channel;
									apply_fm_music_volume(chip_channel, voice_field, each_part);
									z80_block(10);
								}
							}
						}
						z80_block(39);
						voice_field = saved_inner + 0x12u;
						voices_left--;
						if (voices_left == 0u)
							break;
						z80_cycles(5);
					}
					z80_block(43);
					each_part += 9u;
					parts_left = saved_count - 1u;
					if (parts_left == 0u)
						break;
					z80_cycles(5);
					continue;
				}
				z80_block(13);
				volume_scale = music_volume;
				goto music_volume_scale_part;
			}
			z80_block(10);
			return;
		}
		z80_block(17);
		if (controller == 0x40u)
		{
			z80_block(21);
			if (control_value >= 0x40u)
			{
				z80_block(77);
				controls_entry = channel_controls_ADDRESS + (midi_status & 0xFu);
				z80_write8(controls_entry, z80_read8(controls_entry) | 0x80u);
				return;
			}
			z80_block(51);
			voice = SAMPLE_VOICE_ADDRESS(0);
			voice_size = 0x12u;
			voices_left = 0x14u;
			channel = midi_status & 0xFu;
			for (;;)
			{
				z80_block(29);
				if (channel != SAMPLE_VOICE(voice)->midi_channel)
				{
					z80_block(23);
					voice += voice_size;
					voices_left--;
					if (voices_left == 0u)
						break;
					z80_cycles(5);
					continue;
				}
				z80_block(41);
				voice_field = (voice_field & ~0xFFu) | channel;
				if ((SAMPLE_VOICE(voice)->flags >> 1) & 1u)
				{
					z80_block(37);
					if (SAMPLE_VOICE(voice)->chip_channel >> 7 == 0u)
					{
						z80_block(39);
						saved_outer = voice_field;
						saved_voices_left = voices_left;
						stop_pcm_voice(voice);
						z80_block(30);
						voices_left = saved_voices_left;
						voice_field = saved_outer;
						voice_size = 0x12u;
					}
					else
					{
						z80_block(17);
						release_held_fm_voice(voice);
						z80_block(10);
					}
				}
				z80_block(4);
				channel = (uint8_t)voice_field;
				z80_block(23);
				voice += voice_size;
				voices_left--;
				if (voices_left == 0u)
					break;
				z80_cycles(5);
			}
			z80_block(77);
			controls_entry = channel_controls_ADDRESS + (midi_status & 0xFu);
			z80_write8(controls_entry, z80_read8(controls_entry) & 0x7Fu);
			return;
		}
		z80_block(17);
		if (controller == 0x7Bu)
		{
			z80_block(55);
			control_value = midi_status & 0xFu;
			voice = SAMPLE_VOICE_ADDRESS(0);
			voices_left = 8u;
			for (;;)
			{
				z80_block(30);
				if ((SAMPLE_VOICE(voice)->flags >> 6) & 1u)
				{
					z80_block(33);
					if (control_value == SAMPLE_VOICE(voice)->midi_channel)
					{
						z80_block(28);
						saved_count = voices_left;
						saved_value = control_value;
						stop_pcm_voice(voice);
						z80_block(20);
						voices_left = saved_count;
						control_value = saved_value;
					}
				}
				z80_block(19);
				voices_left--;
				if (voices_left == 0u)
					break;
				z80_cycles(5);
			}
		fm_notes_off:
			z80_block(47);
			channel = midi_status & 0xFu;
			voice_field = FM_VOICE_ADDRESS(0);
			voice_size = 0x12u;
			voices_left = 0xCu;
			for (;;)
			{
				z80_block(22);
				if ((z80_read8(voice_field) >> 6) & 1u)
				{
					z80_block(35);
					voice_field += 3u;
					if (channel != z80_read8(voice_field))
					{
						z80_block(18);
						voice_field = voice_field - 1u - 1u - 1u;
					}
					else
					{
						z80_block(46);
						voice_field = voice_field - 1u - 1u - 1u;
						saved_count = voices_left;
						saved_value = control_value;
						fm_note_off(&voice_size, &voice_field, &channel);
						z80_block(35);
						voices_left = saved_count;
						control_value = saved_value;
						z80_write8(voice_field, z80_read8(voice_field) & 0xBFu);
					}
				}
				z80_block(19);
				voice_field += voice_size;
				voices_left--;
				if (voices_left == 0u)
					break;
				z80_cycles(5);
			}
			z80_block(118);
			channel_controls[control_value] = 0u;
			bend_entry = channel_pitch_bends_ADDRESS + (uint8_t)(control_value * 2u);
			z80_write8(bend_entry, 0u);
			z80_write8((uint16_t)(bend_entry + 1u), 0u);
			return;
		}
		z80_block(17);
		if (controller == 0x50u)
		{
			z80_block(21);
			if (control_value == 0x7Eu)
			{
				z80_block(31);
				voice = SAMPLE_VOICE_ADDRESS(0);
				voices_left = 8u;
				for (;;)
				{
					z80_block(30);
					if (((SAMPLE_VOICE(voice)->flags >> 6) & 1u) == 0u)
					{
						z80_block(19);
						voices_left--;
						if (voices_left == 0u)
							break;
						z80_cycles(5);
						continue;
					}
					z80_block(30);
					if (((SAMPLE_VOICE(voice)->flags >> 4) & 1u) == 0u)
					{
					stop_music_sample_voice:
						z80_block(28);
						saved_count = voices_left;
						stop_pcm_voice(voice);
						z80_block(20);
						voices_left = saved_count;
					}
					else
					{
						z80_block(36);
						if (SAMPLE_VOICE(voice)->midi_channel == 0xFu)
							goto stop_music_sample_voice;
					}
					z80_block(19);
					voices_left--;
					if (voices_left == 0u)
						break;
					z80_cycles(5);
				}
				z80_block(10);
			stop_all_fm_voices:
				z80_block(31);
				voice = FM_VOICE_ADDRESS(0);
				voice_size = 0x12u;
				voices_left = 0xCu;
				for (;;)
				{
					z80_block(30);
					if (((FM_VOICE(voice)->flags >> 6) & 1u) == 0u)
					{
						z80_block(23);
						voice += voice_size;
						voices_left--;
						if (voices_left == 0u)
							break;
						z80_cycles(5);
						continue;
					}
					z80_block(28);
					saved_count = voices_left;
					stop_fm_voice(voice);
					z80_block(20);
					voices_left = saved_count;
					voice_size = 0x12u;
					z80_block(23);
					voice += voice_size;
					voices_left--;
					if (voices_left == 0u)
						break;
					z80_cycles(5);
				}
				z80_block(17);
				controls_entry = channel_controls_ADDRESS;
				channels_left = 0x10u;
				for (;;)
				{
					z80_block(24);
					z80_write8(controls_entry, 0u);
					controls_entry++;
					channels_left--;
					if (channels_left == 0u)
						break;
					z80_cycles(5);
				}
				z80_block(17);
				bend_entry = channel_pitch_bends_ADDRESS;
				channels_left = 0x10u;
				for (;;)
				{
					z80_block(24);
					z80_write8(bend_entry, 0u);
					bend_entry++;
					channels_left--;
					if (channels_left == 0u)
						break;
					z80_cycles(5);
				}
				z80_block(10);
				return;
			}
			z80_block(31);
			voice = SAMPLE_VOICE_ADDRESS(0);
			voice_size = 0x12u;
			voices_left = 8u;
			for (;;)
			{
				z80_block(30);
				if (((SAMPLE_VOICE(voice)->flags >> 6) & 1u) == 0u)
				{
					z80_block(23);
					voice += voice_size;
					voices_left--;
					if (voices_left == 0u)
						break;
					z80_cycles(5);
					continue;
				}
				z80_block(28);
				saved_count = voices_left;
				stop_pcm_voice(voice);
				z80_block(20);
				voices_left = saved_count;
				voice_size = 0x12u;
				z80_block(23);
				voice += voice_size;
				voices_left--;
				if (voices_left == 0u)
					break;
				z80_cycles(5);
			}
			goto stop_all_fm_voices;
		}
		z80_block(17);
		if (controller == 0x20u)
		{
			z80_block(31);
			setup = control_value;
			setup_data = part_templates_ADDRESS;
			setup_data = read_table_word(setup_data, setup);
			z80_block(57);
			move_block(setup_data, MIDI_PART_ADDRESS(0), 0x48u, 1u);
			setup_data += 0x48u;
			each_part = MIDI_PART_ADDRESS(0);
			parts_left = 8u;
			for (;;)
			{
				z80_block(56);
				setup_data = (setup_data & 0xFFu) | ((uint32_t)MIDI_PART(each_part)->volume << 8);
				volume_scale = 0xFFu;
				if ((MIDI_PART(each_part)->flags & 1u) != 0u)
				{
				part_template_scale_volume:
					z80_block(21);
					scale = volume_scale;
					setup_data = multiply_bytes(scale, setup_data);
					z80_block(18);
					rounded_volume = (uint8_t)(setup_data >> 8);
					if (rounded_volume != 0u)
					{
						z80_block(4);
						rounded_volume++;
					}
					z80_block(52);
					MIDI_PART(each_part)->effective_volume = rounded_volume;
					each_part += 9u;
					parts_left--;
					if (parts_left == 0u)
						break;
					z80_cycles(5);
					continue;
				}
				z80_block(13);
				volume_scale = music_volume;
				goto part_template_scale_volume;
			}
			z80_block(10);
			return;
		}
		z80_block(17);
		if (controller == 0x51u)
		{
			z80_block(60);
			note_to_mark = note_codes[control_value];
			voice_field = SAMPLE_VOICE_ADDRESS(0);
			voices_left = 8u;
			for (;;)
			{
				z80_block(22);
				if (((z80_read8(voice_field) >> 6) & 1u) == 0u)
				{
					z80_block(29);
					voice_field += 0x12u;
					voices_left--;
					if (voices_left == 0u)
						break;
					z80_cycles(5);
					continue;
				}
				z80_block(33);
				voice_field += 2u;
				if (note_to_mark != z80_read8(voice_field))
				{
				mark_note_not_this:
					z80_block(12);
					voice_field = voice_field - 1u - 1u;
				}
				else
				{
					z80_block(43);
					voice_field++;
					if ((midi_status & 0xFu) != z80_read8(voice_field))
					{
						z80_block(6);
						voice_field--;
						goto mark_note_not_this;
					}
					z80_block(43);
					voice_field = voice_field - 1u - 1u - 1u;
					z80_write8(voice_field, z80_read8(voice_field) | 1u);
				}
				z80_block(29);
				voice_field += 0x12u;
				voices_left--;
				if (voices_left == 0u)
					break;
				z80_cycles(5);
			}
			z80_block(10);
			return;
		}
		z80_block(17);
		if (controller == 0x11u)
		{
			z80_block(18);
			if (control_value == 0u)
			{
				z80_block(87);
				control_value = 0u;
				channel = midi_status & 0xFu;
				channel_vibrato_presets[channel] = 0u;
				voices_left = 0xCu;
				voice = FM_VOICE_ADDRESS(0);
				for (;;)
				{
					z80_block(29);
					if (channel == FM_VOICE(voice)->midi_channel)
						break;
					z80_block(33);
					voice += 0x12u;
					voices_left--;
					if (voices_left == 0u)
					{
						z80_block(10);
						return;
					}
					z80_cycles(5);
				}
				z80_block(36);
				voice += 4u;
				voices_or_bytes_left = 6u;
				for (;;)
				{
					z80_block(37);
					z80_write8(voice, control_value);
					voice++;
					voices_or_bytes_left--;
					if (voices_or_bytes_left == 0u)
						break;
					z80_cycles(5);
				}
				z80_block(39);
				voice += 8u;
			vibrato_preset_check_channel:
				z80_block(49);
				if ((midi_status & 0xFu) == z80_read8((uint16_t)(voice + 3u)))
				{
					z80_block(31);
					preset_address = vibrato_presets_ADDRESS;
					preset = control_value;
					preset_address = read_table_word(preset_address, preset);
					z80_block(344);
					preset_data = preset_address;
					z80_write8((uint16_t)(voice + 7u), z80_read8(preset_data));
					preset_data++;
					z80_write8((uint16_t)(voice + 8u), z80_read8(preset_data));
					preset_data++;
					z80_write8((uint16_t)(voice + 9u), z80_read8(preset_data));
					preset_data++;
					z80_write8((uint16_t)(voice + 0xAu), z80_read8(preset_data));
					preset_data++;
					z80_write8((uint16_t)(voice + 0xBu), z80_read8(preset_data));
					preset_data++;
					z80_write8((uint16_t)(voice + 0xCu), z80_read8(preset_data));
					z80_write8((uint16_t)(voice + 0xDu), z80_read8((uint16_t)(preset_data + 1u)));
					z80_write8((uint16_t)(voice + 0xEu), z80_read8((uint16_t)(voice + 8u)));
					z80_write8((uint16_t)(voice + 0xDu), (z80_read8((uint16_t)(voice + 0xDu)) & 1u) + z80_read8((uint16_t)(voice + 9u)));
				vibrato_preset_next_voice:
					z80_block(33);
					voice += 0x12u;
					voices_or_bytes_left--;
					if (voices_or_bytes_left == 0u)
					{
						z80_block(10);
						return;
					}
					z80_cycles(5);
				vibrato_preset_check_voice:
					z80_block(36);
					if ((z80_read8(voice) & 0x40u) == 0u)
						goto vibrato_preset_next_voice;
					goto vibrato_preset_check_channel;
				}
				goto vibrato_preset_next_voice;
			}
			z80_block(80);
			channel_vibrato_presets[midi_status & 0xFu] = control_value;
			voices_or_bytes_left = 0xCu;
			voice = FM_VOICE_ADDRESS(0);
			goto vibrato_preset_check_voice;
		}
		z80_block(17);
		if (controller != 0x80u)
		{
			z80_block(10);
			return;
		}
		goto fm_notes_off;
	}
	z80_block(17);
	if (event == 0xC0u)
	{
		z80_block(18);
		if (first_data_byte >> 7)
		{
			z80_block(17);
			read_midi_byte(&message_byte, &ignored16, &ignored8);
			first_data_byte = (uint8_t)message_byte;
		}
		z80_block(29);
		MIDI_PART(part)->program = first_data_byte;
		return;
	}
	z80_block(17);
	if (event == 0xD0u)
	{
		first_byte = first_data_byte;
		skip_midi_data(first_byte);
		return;
	}
	z80_block(17);
	if (event == 0xA0u)
	{
		first_byte = first_data_byte;
		skip_midi_data(first_byte);
		return;
	}
	z80_block(10);
}

uint16_t retune_voice(uint16_t loop_count, uint16_t part, uint16_t voice)
{
	uint32_t carry = 0;
	uint8_t caller_c = 0, count_or_shifts = 0, detune_low = 0, detune_sign = 0, fm_detune = 0, fm_register = 0, fnum_low_register = 0, note = 0, octave = 0, pcm_register = 0, pitch_low_register = 0, register_value = 0, voice_note = 0;
	uint16_t bend = 0, fnum = 0, fnum_entry = 0, fnum_table_entry = 0, old_pitch = 0, pitch = 0, pitch_entry = 0, pitch_table_entry = 0, register_channel = 0;
	uint8_t saved_count_or_shifts = 0, saved_caller_c = 0;
	uint8_t detune_byte = 0, octave_bits = 0;

	count_or_shifts = (uint8_t)(loop_count >> 8);
	caller_c = (uint8_t)loop_count;
	z80_block(37);
	register_value = SAMPLE_VOICE(voice)->chip_channel;
	if (register_value >> 7 != 0u)
	{
		z80_block(313);
		saved_count_or_shifts = count_or_shifts;
		saved_caller_c = caller_c;
		fm_channel = register_value;
		note = SAMPLE_VOICE(voice)->note;
		voice_note = note;
		fnum_table_entry = fm_fnum_tables_ADDRESS + (uint8_t)((note & 0xFu) * 2u);
		bend = pitch_bend;
		fnum_entry = SOUND_ROM16(fnum_table_entry) + bend + bend;
		fm_detune = MIDI_PART(part)->fm_detune;
		detune_low = fm_detune;
		detune_byte = fm_detune;
		detune_sign = ((uint32_t)detune_byte << 1) | (detune_byte >> 7);
		carry = detune_byte >> 7;
		fnum = z80_read16(fnum_entry) + (detune_low | ((uint32_t)(uint8_t)(0u - carry) << 8));
		fnum = (fnum & 0xFFu) | ((uint32_t)(uint8_t)(((voice_note & ~0xFu) >> 1) + (fnum >> 8)) << 8);
		register_channel = (fm_channel & 3u) << 8;
		register_value = (uint8_t)(fnum >> 8);
		fm_register = 0xA4u | (register_channel >> 8);
		write_fm_channel(fm_register, register_value);
		z80_block(36);
		register_value = (uint8_t)fnum;
		fnum_low_register = 0xA0u | (register_channel >> 8);
		write_fm_channel(fnum_low_register, register_value);
		z80_block(20);
		loop_count = ((uint32_t)saved_count_or_shifts << 8) | saved_caller_c;
		return loop_count;
	}
	z80_block(31);
	register_value |= 0xC0u;
	pcm_register = 7u;
	write_pcm_register(pcm_register, register_value);
	z80_block(177);
	note = SAMPLE_VOICE(voice)->note;
	pitch_table_entry = pcm_pitch_tables_ADDRESS + (uint8_t)((note & 0xFu) * 2u);
	bend = pitch_bend;
	pitch_entry = SOUND_ROM16(pitch_table_entry) + bend + bend;
	pitch = z80_read16(pitch_entry);
	octave_bits = note & ~0xFu;
	octave = octave_bits;
	if (octave_bits != 0u)
	{
		z80_block(33);
		octave = (octave >> 1) | ((uint32_t)octave << 7);
		octave = (octave >> 1) | ((uint32_t)octave << 7);
		octave = (octave >> 1) | ((uint32_t)octave << 7);
		octave = (octave >> 1) | ((uint32_t)octave << 7);
		if (octave < 8u)
		{
			z80_block(4);
			count_or_shifts = octave;
			for (;;)
			{
				z80_block(24);
				carry = (uint8_t)pitch >> 7;
				pitch = (pitch & ~0xFFu) | (uint8_t)((uint32_t)pitch << 1);
				pitch = (pitch & 0xFFu) | ((uint32_t)(uint8_t)(((uint32_t)(pitch >> 8) << 1) | carry) << 8);
				count_or_shifts--;
				if (count_or_shifts == 0u)
					break;
				z80_cycles(5);
			}
		}
		else
		{
			z80_block(19);
			count_or_shifts = (~octave + 1u) & 0xFu;
			for (;;)
			{
				z80_block(24);
				carry = (pitch >> 8) & 1u;
				pitch = (pitch & 0xFFu) | ((uint32_t)((pitch >> 8) >> 1) << 8);
				pitch = (pitch & ~0xFFu) | (uint8_t)(((uint8_t)pitch >> 1) | (carry << 7));
				count_or_shifts--;
				if (count_or_shifts == 0u)
					break;
				z80_cycles(5);
			}
			z80_block(10);
		}
	}
	z80_block(28);
	register_value = (uint8_t)(pitch >> 8);
	pcm_register = 3u;
	write_pcm_register(pcm_register, register_value);
	z80_block(28);
	pitch_low_register = 2u;
	register_value = (uint8_t)pitch;
	pcm_register = 2u;
	write_pcm_register(pcm_register, register_value);
	z80_block(196);
	old_pitch = SAMPLE_VOICE(voice)->pitch_step;
	SAMPLE_VOICE(voice)->pitch_step = pitch;
	pitch = pitch - old_pitch + SAMPLE_VOICE(voice)->stream_position;
	SAMPLE_VOICE(voice)->stream_position = pitch;
	loop_count = ((uint32_t)count_or_shifts << 8) | pitch_low_register;
	return loop_count;
}

void update_fm_modulation(uint16_t *p_loop_count, uint16_t *p_voice)
{
	uint16_t loop_count = (uint16_t)*p_loop_count;
	uint16_t voice = (uint16_t)*p_voice;
	uint8_t chip_channel = 0, fm_register = 0, pan_register = 0, register_value = 0, voice_chip_channel = 0;
	uint8_t saved_count = 0, saved_voice_chip_channel = 0;

	z80_block(26);
	voice++;
	voice_chip_channel = z80_read8(voice);
	if (voice_chip_channel >> 7 == 0u)
	{
		z80_cycles(6);
		loop_count = (loop_count & ~0xFFu) | voice_chip_channel;
		*p_loop_count = loop_count;
		*p_voice = voice;
		return;
	}
	z80_block(10);
	z80_block(90);
	saved_count = loop_count >> 8;
	saved_voice_chip_channel = voice_chip_channel;
	chip_channel = voice_chip_channel;
	fm_channel = chip_channel;
	pan_register = (chip_channel & 3u) - 0x4Cu;
	register_value = (fm_pan_modulation & 7u) | 0xC0u;
	fm_register = pan_register;
	write_fm_channel(fm_register, register_value);
	z80_block(26);
	voice--;
	loop_count = ((uint32_t)saved_count << 8) | saved_voice_chip_channel;
	*p_loop_count = loop_count;
	*p_voice = voice;
}

void read_midi_byte(uint16_t *p_byte_copy, uint16_t *p_queue_slot, uint8_t *p_byte)
{
	uint8_t byte = 0;
	uint16_t byte_copy = 0, queue_slot = 0;

	z80_block(56);
	midi_queue_read++;
	queue_slot = midi_queue_read | 0xE100u;
	byte = z80_read8(queue_slot);
	byte_copy = byte;
	*p_byte_copy = (*p_byte_copy & 0xFF00u) | byte_copy;
	*p_queue_slot = queue_slot;
	*p_byte = byte;
}

void start_pcm_sample(uint16_t voice)
{
	uint8_t bank_high = 0, bank_low = 0, channels_off = 0, next_wave_page = 0, pcm_register = 0, sample = 0, wave_page = 0;
	uint16_t header = 0, sample_address = 0, wave_address = 0;
	uint8_t saved_start_page = 0, saved_start_offset = 0;

	z80_block(52);
	channels_off = ~pcm_channels_on;
	pcm_register = 8u;
	write_pcm_register(pcm_register, channels_off);
	z80_block(40);
	sample = sample_or_patch;
	header = pcm_sample_headers_ADDRESS;
	header = read_table_word(header, sample);
	z80_block(293);
	bank_high = z80_read8(header);
	SAMPLE_VOICE(voice)->sample_bank_high = bank_high;
	bank_high &= 7u;
	rom_bank_high = bank_high;
	port_out(ROM_BANK_HIGH, bank_high);
	header++;
	bank_low = z80_read8(header);
	SAMPLE_VOICE(voice)->sample_bank_low = bank_low;
	rom_bank_low = bank_low;
	port_out(ROM_BANK_LOW, bank_low);
	wave_page = SAMPLE_VOICE(voice)->wave_page;
	header++;
	saved_start_page = z80_read8(header);
	saved_start_offset = 0u;
	header++;
	SAMPLE_VOICE(voice)->header_bank_step = z80_read8(header);
	header++;
	SAMPLE_VOICE(voice)->header_address_low = z80_read8(header);
	SAMPLE_VOICE(voice)->header_address_high = z80_read8((uint16_t)(header + 1u));
	sample_address = ((uint32_t)saved_start_page << 8) | saved_start_offset;
	wave_address = (uint32_t)wave_page << 8;
	copy_sample_page(&wave_address, &sample_address);
	next_wave_page = (uint8_t)(wave_address >> 8);
	z80_block(40);
	SAMPLE_VOICE(voice)->wave_page = next_wave_page;
	if (0xC0u == sample_address >> 8)
	{
		z80_block(30);
		sample_address = ROM_BANK_WINDOW;
		SAMPLE_VOICE(voice)->sample_bank_low++;
	}
	z80_block(39);
	SAMPLE_VOICE(voice)->source_page = sample_address >> 8;
}

void refill_pcm_voice(void)
{
	uint8_t bank_high = 0, bank_low = 0, four_pages = 0, wave_offset = 0, wave_page = 0;
	uint16_t progress = 0, sample_address = 0, voice = 0, voices_left_at = 0, wave_address = 0;
	uint8_t old_voices_left = 0;

	z80_block(50);
	voice = refill_voice;
	if (((SAMPLE_VOICE(voice)->flags >> 6) & 1u) == 0u)
	{
		z80_block(53);
		voice += 0x12u;
		voices_left_at = refill_voices_left_ADDRESS;
		old_voices_left = refill_voices_left;
		refill_voices_left = old_voices_left - 1u;
		if (old_voices_left != 1u)
		{
			z80_block(30);
			refill_voice = voice;
			return;
		}
		z80_cycles(5);
		z80_block(46);
		refill_voices_left = 8u;
		refill_voice = SAMPLE_VOICE_ADDRESS(0);
		return;
	}
	z80_block(105);
	progress = SAMPLE_VOICE(voice)->stream_position + SAMPLE_VOICE(voice)->pitch_step;
	if ((((progress >> 8) >> 4) & 1u) == 0u)
	{
		z80_block(25);
		four_pages = 0u;
		if ((((progress >> 8) >> 3) & 1u) != 0u)
		{
		refill_copy_pages:
			z80_block(252);
			progress = (progress & 0xFFu) | (((progress >> 8) & ~8u) << 8);
			SAMPLE_VOICE(voice)->stream_position = progress;
			z80_write8(PCM_CONTROL, (SAMPLE_VOICE(voice)->chip_channel * 2u) | 0x80u);
			bank_high = SAMPLE_VOICE(voice)->sample_bank_high & 7u;
			rom_bank_high = bank_high;
			port_out(ROM_BANK_HIGH, bank_high);
			bank_low = SAMPLE_VOICE(voice)->sample_bank_low;
			rom_bank_low = bank_low;
			port_out(ROM_BANK_LOW, bank_low);
			wave_page = SAMPLE_VOICE(voice)->wave_page;
			wave_offset = 0u;
			sample_address = (uint32_t)SAMPLE_VOICE(voice)->source_page << 8;
			if (four_pages == 0u)
			{
			refill_copy_two_pages:
				z80_block(17);
				wave_address = ((uint32_t)wave_page << 8) | wave_offset;
				copy_sample_page(&wave_address, &sample_address);
				wave_page = (uint8_t)(wave_address >> 8);
				wave_offset = (uint8_t)wave_address;
				z80_block(27);
				sample_address--;
				if (z80_read8(sample_address) == 0xFFu)
				{
					z80_cycles(5);
					stop_pcm_voice(voice);
					return;
				}
				z80_block(27);
				sample_address++;
				if (0xC0u == sample_address >> 8)
				{
					z80_block(73);
					sample_address = (sample_address & 0xFFu) | 0xA000u;
					SAMPLE_VOICE(voice)->sample_bank_low++;
					bank_low = SAMPLE_VOICE(voice)->sample_bank_low;
					rom_bank_low = bank_low;
					port_out(ROM_BANK_LOW, bank_low);
				}
				z80_block(21);
				if (0xE0u == wave_page)
				{
					z80_block(7);
					wave_page = 0xD0u;
				}
				z80_block(17);
				wave_address = ((uint32_t)wave_page << 8) | wave_offset;
				copy_sample_page(&wave_address, &sample_address);
				wave_page = (uint8_t)(wave_address >> 8);
				z80_block(27);
				sample_address--;
				if (z80_read8(sample_address) == 0xFFu)
				{
					z80_cycles(5);
					stop_pcm_voice(voice);
					return;
				}
				z80_block(27);
				sample_address++;
				if (0xC0u == sample_address >> 8)
				{
					z80_block(30);
					sample_address = ROM_BANK_WINDOW;
					SAMPLE_VOICE(voice)->sample_bank_low++;
				}
				z80_block(21);
				if (0xE0u == wave_page)
				{
					z80_block(7);
					wave_page = 0xD0u;
				}
				z80_block(38);
				SAMPLE_VOICE(voice)->wave_page = wave_page;
				SAMPLE_VOICE(voice)->source_page = sample_address >> 8;
				z80_block(53);
				voice += 0x12u;
				voices_left_at = refill_voices_left_ADDRESS;
				old_voices_left = refill_voices_left;
				refill_voices_left = old_voices_left - 1u;
				if (old_voices_left != 1u)
				{
					z80_block(30);
					refill_voice = voice;
					return;
				}
				z80_cycles(5);
				z80_block(46);
				refill_voices_left = 8u;
				refill_voice = SAMPLE_VOICE_ADDRESS(0);
				return;
			}
			z80_block(17);
			wave_address = ((uint32_t)wave_page << 8) | wave_offset;
			copy_sample_page(&wave_address, &sample_address);
			wave_page = (uint8_t)(wave_address >> 8);
			wave_offset = (uint8_t)wave_address;
			z80_block(30);
			sample_address--;
			if (z80_read8(sample_address) == 0xFFu)
			{
				stop_pcm_voice(voice);
				return;
			}
			z80_block(27);
			sample_address++;
			if (0xC0u == sample_address >> 8)
			{
				z80_block(73);
				sample_address = (sample_address & 0xFFu) | 0xA000u;
				SAMPLE_VOICE(voice)->sample_bank_low++;
				bank_low = SAMPLE_VOICE(voice)->sample_bank_low;
				rom_bank_low = bank_low;
				port_out(ROM_BANK_LOW, bank_low);
			}
			z80_block(21);
			if (0xE0u == wave_page)
			{
				z80_block(7);
				wave_page = 0xD0u;
			}
			z80_block(17);
			wave_address = ((uint32_t)wave_page << 8) | wave_offset;
			copy_sample_page(&wave_address, &sample_address);
			wave_page = (uint8_t)(wave_address >> 8);
			wave_offset = (uint8_t)wave_address;
			z80_block(27);
			sample_address--;
			if (z80_read8(sample_address) == 0xFFu)
			{
				z80_cycles(5);
				stop_pcm_voice(voice);
				return;
			}
			z80_block(27);
			sample_address++;
			if (0xC0u == sample_address >> 8)
			{
				z80_block(73);
				sample_address = (sample_address & 0xFFu) | 0xA000u;
				SAMPLE_VOICE(voice)->sample_bank_low++;
				bank_low = SAMPLE_VOICE(voice)->sample_bank_low;
				rom_bank_low = bank_low;
				port_out(ROM_BANK_LOW, bank_low);
			}
			z80_block(21);
			if (0xE0u != wave_page)
				goto refill_copy_two_pages;
			z80_block(7);
			wave_page = 0xD0u;
			goto refill_copy_two_pages;
		}
		z80_block(48);
		SAMPLE_VOICE(voice)->stream_position = progress;
		z80_block(53);
		voice += 0x12u;
		voices_left_at = refill_voices_left_ADDRESS;
		old_voices_left = refill_voices_left;
		refill_voices_left = old_voices_left - 1u;
		if (old_voices_left != 1u)
		{
			z80_block(30);
			refill_voice = voice;
			return;
		}
		z80_cycles(5);
		z80_block(46);
		refill_voices_left = 8u;
		refill_voice = SAMPLE_VOICE_ADDRESS(0);
		return;
	}
	z80_block(25);
	progress = (progress & 0xFFu) | (((progress >> 8) & 0xEFu) << 8);
	four_pages = 1u;
	goto refill_copy_pages;
}

void stop_pcm_voice(uint16_t voice)
{
	uint8_t own_order = 0, pcm_register = 0, register_value = 0, voices_left = 0;
	uint16_t other_order = 0, voice_size = 0;

	z80_block(50);
	register_value = SAMPLE_VOICE(voice)->chip_channel | 0xC0u;
	pcm_register = 7u;
	write_pcm_register(pcm_register, register_value);
	z80_block(72);
	register_value = ((register_value & 7u) * 2u) | 0x80u;
	register_value = ((uint32_t)register_value << 1) | (register_value >> 7);
	register_value = ((uint32_t)register_value << 1) | (register_value >> 7);
	register_value = ((uint32_t)register_value << 1) | (register_value >> 7);
	register_value = ((((uint32_t)register_value << 1) | (register_value >> 7)) & 0xF0u) + 0x18u;
	pcm_register = 5u;
	write_pcm_register(pcm_register, register_value);
	z80_block(227);
	SAMPLE_VOICE(voice)->flags = 0u;
	pcm_channels_on ^= pcm_channel_bits[SAMPLE_VOICE(voice)->chip_channel];
	SAMPLE_VOICE(voice)->midi_channel = 0u;
	SAMPLE_VOICE(voice)->wave_page = 0xD0u;
	AS8(SAMPLE_VOICE(voice)->stream_position) = 0xFFu;
	AT8(SAMPLE_VOICE(voice)->stream_position, 1) = 7u;
	pcm_voices_used--;
	if (SAMPLE_VOICE(voice)->allocation_order >= 0x10u)
	{
		z80_cycles(6);
		return;
	}
	z80_block(46);
	other_order = voice_0_allocation_order_ADDRESS;
	voice_size = 0x12u;
	own_order = SAMPLE_VOICE(voice)->allocation_order;
	voices_left = 8u;
	for (;;)
	{
		z80_block(17);
		if (SAMPLE_VOICE(other_order - 0x6u)->allocation_order <= own_order)
		{
			z80_block(19);
			other_order += voice_size;
			voices_left--;
			if (voices_left == 0u)
				break;
			z80_cycles(5);
			continue;
		}
		z80_block(11);
		SAMPLE_VOICE(other_order - 0x6u)->allocation_order--;
		z80_block(19);
		other_order += voice_size;
		voices_left--;
		if (voices_left == 0u)
			break;
		z80_cycles(5);
	}
	z80_block(10);
}

uint16_t multiply_bytes(uint16_t factor, uint16_t value)
{
	uint32_t carry = 0;

	z80_block(35);
	factor &= 0xFFu;
	value &= ~0xFFu;
	carry = (int16_t)value < 0;
	value = value * 2u;
	if (carry)
	{
		z80_block(11);
		value += factor;
	}
	z80_block(21);
	carry = (int16_t)value < 0;
	value = value * 2u;
	if (carry)
	{
		z80_block(11);
		value += factor;
	}
	z80_block(21);
	carry = (int16_t)value < 0;
	value = value * 2u;
	if (carry)
	{
		z80_block(11);
		value += factor;
	}
	z80_block(21);
	carry = (int16_t)value < 0;
	value = value * 2u;
	if (carry)
	{
		z80_block(11);
		value += factor;
	}
	z80_block(21);
	carry = (int16_t)value < 0;
	value = value * 2u;
	if (carry)
	{
		z80_block(11);
		value += factor;
	}
	z80_block(21);
	carry = (int16_t)value < 0;
	value = value * 2u;
	if (carry)
	{
		z80_block(11);
		value += factor;
	}
	z80_block(21);
	carry = (int16_t)value < 0;
	value = value * 2u;
	if (carry)
	{
		z80_block(11);
		value += factor;
	}
	z80_block(16);
	carry = (int16_t)value < 0;
	value = value * 2u;
	if (carry == 0u)
	{
		z80_cycles(6);
		return value;
	}
	z80_block(21);
	value += factor;
	return value;
}

void copy_sample_page(uint16_t *p_wave_address, uint16_t *p_sample_address)
{
	uint16_t wave_address = (uint16_t)*p_wave_address;
	uint16_t sample_address = (uint16_t)*p_sample_address;

	z80_block(4106);
	for (uint32_t repeat = 0; repeat < 256u; repeat++)
	{
		z80_write8(wave_address, z80_read8(sample_address));
		sample_address++;
		wave_address++;
	}
	*p_wave_address = wave_address;
	*p_sample_address = sample_address;
}

void fm_note_off(uint16_t *p_de, uint16_t *p_saved_channel, uint8_t *p_a)
{
	uint16_t saved_channel = (uint16_t)*p_saved_channel;
	uint8_t channel = 0, flags = 0, key_off_value = 0, voices_left = 0;
	uint16_t channel_offset = 0, controls_entry = 0, voice = 0, voice_size = 0;

	z80_block(51);
	voice = FM_VOICE_ADDRESS(0);
	voice_size = 0x12u;
	voices_left = 0xCu;
	channel = midi_status & 0xFu;
	for (;;)
	{
		z80_block(29);
		if (channel != FM_VOICE(voice)->midi_channel)
		{
		fm_note_off_next_voice:
			z80_block(23);
			voice += voice_size;
			voices_left--;
			if (voices_left == 0u)
			{
				z80_block(10);
				*p_de = voice_size;
				*p_saved_channel = saved_channel;
				*p_a = channel;
				return;
			}
			z80_cycles(5);
			continue;
		}
		z80_block(46);
		saved_channel = (saved_channel & ~0xFFu) | channel;
		if (note_code != FM_VOICE(voice)->note)
		{
		fm_note_off_not_this:
			z80_block(4);
			channel = (uint8_t)saved_channel;
			goto fm_note_off_next_voice;
		}
		z80_block(37);
		flags = FM_VOICE(voice)->flags;
		if (((flags >> 1) & 1u) != 0u)
			goto fm_note_off_not_this;
		z80_block(17);
		if ((flags & 0x7Fu) != 0u)
			break;
		goto fm_note_off_not_this;
	}
	z80_block(65);
	channel = (uint8_t)saved_channel;
	channel_offset = channel;
	controls_entry = channel_controls_ADDRESS + channel_offset;
	if (z80_read8(controls_entry) >> 7 == 0u)
	{
		key_off_value = stop_fm_voice(voice);
		*p_de = channel_offset;
		*p_saved_channel = controls_entry;
		*p_a = key_off_value;
		return;
	}
	z80_block(33);
	FM_VOICE(voice)->flags |= 2u;
	*p_de = channel_offset;
	*p_saved_channel = controls_entry;
	*p_a = channel;
}

uint8_t stop_fm_voice(uint16_t voice)
{
	uint8_t fm_register = 0, register_value = 0;

	z80_block(212);
	FM_VOICE(voice)->flags = 0u;
	FM_VOICE(voice)->midi_channel = 0u;
	FM_VOICE(voice)->vibrato_range = 0u;
	FM_VOICE(voice)->vibrato_depth = 0u;
	AS8(FM_VOICE(voice)->vibrato_offset) = 0u;
	AT8(FM_VOICE(voice)->vibrato_offset, 1) = 0u;
	FM_VOICE(voice)->vibrato_block = 0u;
	FM_VOICE(voice)->vibrato_phase = 0u;
	register_value = FM_VOICE(voice)->chip_channel;
	fm_channel = register_value;
	register_value &= 7u;
	fm_register = 0x28u;
	return write_fm_global(fm_register, register_value);
}

void release_held_fm_voice(uint16_t voice)
{
	uint8_t fm_register = 0, key_on_register = 0, register_value = 0;

	z80_block(99);
	FM_VOICE(voice)->flags = 0u;
	FM_VOICE(voice)->midi_channel = 0u;
	key_on_register = 0x28u;
	register_value = FM_VOICE(voice)->chip_channel;
	fm_channel = register_value;
	if (((register_value >> 3) & 1u) == 0u)
	{
		z80_block(17);
		register_value &= 7u;
		fm_register = key_on_register;
		write_fm1(fm_register, register_value);
		return;
	}
	z80_block(17);
	register_value &= 7u;
	fm_register = key_on_register;
	write_fm2(fm_register, register_value);
}

void apply_fm_music_volume(uint8_t chip_channel, uint16_t level, uint16_t part)
{
	uint8_t channel_code = 0, effective_volume = 0, fm_register = 0, level_register = 0, operator_level = 0, operators_left = 0, part_channel = 0, patch = 0, program = 0, register_number = 0, volume_curve = 0;
	uint16_t instrument = 0, patch_levels = 0, register_list = 0;
	uint8_t saved_operators_left = 0, saved_part_channel = 0;

	z80_block(104);
	channel_code = (uint8_t)chip_channel;
	fm_channel = channel_code;
	part_channel = channel_code & 3u;
	level = ((level + 4u) & 0xFFu) | ((uint32_t)z80_read8((uint16_t)(level + 4u)) << 8);
	effective_volume = MIDI_PART(part)->effective_volume;
	volume_curve = effective_volume;
	if (effective_volume != 0u)
	{
		z80_block(20);
		volume_curve += (uint8_t)~effective_volume >> 1;
	}
	z80_block(17);
	level = multiply_bytes(volume_curve, level);
	z80_block(78);
	note_level = 0x7Fu - ((uint16_t)((uint32_t)((level >> 8) >> 1) << 8) >> 8);
	program = MIDI_PART(part)->program;
	instrument = instruments_ADDRESS;
	instrument = read_table_word(instrument, program);
	z80_block(40);
	patch = z80_read8((uint16_t)(instrument + 1u));
	patch_levels = fm_patches_ADDRESS;
	patch_levels = read_table_word(patch_levels, patch);
	z80_block(38);
	patch_levels += 0x15u;
	operators_left = 4u;
	register_list = fm_level_registers_ADDRESS;
	for (;;)
	{
		z80_block(63);
		saved_operators_left = operators_left;
		saved_part_channel = part_channel;
		register_number = SOUND_ROM8(register_list) + part_channel;
		register_list++;
		level_register = register_number;
		operator_level = z80_read8(patch_levels);
		patch_levels++;
		if (operator_level >> 7 != 0u)
		{
			z80_block(18);
			part_channel = saved_part_channel;
			operators_left = saved_operators_left - 1u;
			if (operators_left == 0u)
				break;
			z80_cycles(5);
			continue;
		}
		z80_block(38);
		operator_level += note_level;
		fm_register = level_register;
		write_fm_channel(fm_register, operator_level);
		z80_block(18);
		part_channel = saved_part_channel;
		operators_left = saved_operators_left - 1u;
		if (operators_left == 0u)
			break;
		z80_cycles(5);
	}
	z80_block(20);
}

void apply_fm_channel_volume(uint8_t chip_channel, uint16_t level, uint16_t part)
{
	uint8_t channel_code = 0, effective_volume = 0, fm_register = 0, level_register = 0, operator_level = 0, operators_left = 0, part_channel = 0, patch = 0, program = 0, register_number = 0, volume_curve = 0;
	uint16_t instrument = 0, patch_levels = 0, register_list = 0;
	uint8_t saved_operators_left = 0, saved_part_channel = 0;

	z80_block(104);
	channel_code = (uint8_t)chip_channel;
	fm_channel = channel_code;
	part_channel = channel_code & 3u;
	level = ((level + 4u) & 0xFFu) | ((uint32_t)z80_read8((uint16_t)(level + 4u)) << 8);
	effective_volume = MIDI_PART(part)->effective_volume;
	volume_curve = effective_volume;
	if (effective_volume != 0u)
	{
		z80_block(20);
		volume_curve += (uint8_t)~effective_volume >> 1;
	}
	z80_block(17);
	level = multiply_bytes(volume_curve, level);
	z80_block(78);
	note_level = 0x7Fu - ((uint16_t)((uint32_t)((level >> 8) >> 1) << 8) >> 8);
	program = MIDI_PART(part)->program;
	instrument = instruments_ADDRESS;
	instrument = read_table_word(instrument, program);
	z80_block(40);
	patch = z80_read8((uint16_t)(instrument + 1u));
	patch_levels = fm_patches_ADDRESS;
	patch_levels = read_table_word(patch_levels, patch);
	z80_block(38);
	patch_levels += 0x15u;
	operators_left = 4u;
	register_list = fm_level_registers_ADDRESS;
	for (;;)
	{
		z80_block(63);
		saved_operators_left = operators_left;
		saved_part_channel = part_channel;
		register_number = SOUND_ROM8(register_list) + part_channel;
		register_list++;
		level_register = register_number;
		operator_level = z80_read8(patch_levels);
		patch_levels++;
		if (operator_level >> 7 != 0u)
		{
			z80_block(18);
			part_channel = saved_part_channel;
			operators_left = saved_operators_left - 1u;
			if (operators_left == 0u)
				break;
			z80_cycles(5);
			continue;
		}
		z80_block(38);
		operator_level += note_level;
		fm_register = level_register;
		write_fm_channel(fm_register, operator_level);
		z80_block(18);
		part_channel = saved_part_channel;
		operators_left = saved_operators_left - 1u;
		if (operators_left == 0u)
			break;
		z80_cycles(5);
	}
	z80_block(20);
}

void load_fm_patch(uint16_t *p_channel_in_part, uint16_t *p_register_list, uint16_t *p_patch_data)
{
	uint8_t fm_register = 0, part_channel = 0, patch = 0, patch_byte = 0, patch_register = 0, register_number = 0, registers_left = 0;
	uint16_t channel_in_part = 0, patch_data = 0, register_list = 0;
	uint8_t saved_registers_left = 0, saved_part_channel = 0;

	z80_block(64);
	part_channel = fm_channel & 3u;
	patch = sample_or_patch;
	patch_data = fm_patches_ADDRESS;
	patch_data = read_table_word(patch_data, patch);
	z80_block(17);
	register_list = fm_patch_registers_ADDRESS;
	registers_left = 0x15u;
	for (;;)
	{
		z80_block(62);
		saved_registers_left = registers_left;
		saved_part_channel = part_channel;
		register_number = SOUND_ROM8(register_list) + part_channel;
		register_list++;
		patch_register = register_number;
		patch_byte = z80_read8(patch_data);
		patch_data++;
		fm_register = patch_register;
		write_fm_channel(fm_register, patch_byte);
		z80_block(18);
		part_channel = saved_part_channel;
		registers_left = saved_registers_left - 1u;
		if (registers_left == 0u)
			break;
		z80_cycles(5);
	}
	z80_block(10);
	channel_in_part = ((uint32_t)registers_left << 8) | part_channel;
	*p_channel_in_part = channel_in_part;
	*p_register_list = register_list;
	*p_patch_data = patch_data;
}

void sequence_tick(void)
{
	uint16_t countdown_at = 0, mailbox_page = 0, player = 0, queue_pointer = 0;
	uint8_t old_countdown = 0;

	z80_block(28);
	countdown_at = tick_countdown_ADDRESS;
	old_countdown = tick_countdown;
	tick_countdown = old_countdown - 1u;
	if (old_countdown == 1u)
	{
		z80_cycles(5);
		z80_block(55);
		tick_countdown = 4u;
		mailbox_page = sound_command_ADDRESS;
		if (midi_queue_count >= 0x40u)
		{
			z80_cycles(6);
			return;
		}
		z80_block(51);
		player = SEQUENCE_PLAYER_ADDRESS(1);
		queue_pointer = 0xE100u | midi_queue_write;
		fade_player_volume(queue_pointer, player, mailbox_page);
		z80_block(24);
		step_player_if_playing(player, mailbox_page);
		z80_block(27);
		player = SEQUENCE_PLAYER_ADDRESS(2);
		step_player_if_playing(player, mailbox_page);
		z80_block(27);
		player = SEQUENCE_PLAYER_ADDRESS(3);
		step_player_if_playing(player, mailbox_page);
		z80_block(27);
		player = SEQUENCE_PLAYER_ADDRESS(4);
		step_player_if_playing(player, mailbox_page);
		z80_block(27);
		player = SEQUENCE_PLAYER_ADDRESS(5);
		step_player_if_playing(player, mailbox_page);
		z80_block(27);
		player = SEQUENCE_PLAYER_ADDRESS(6);
		step_player_if_playing(player, mailbox_page);
		z80_block(20);
		player = SEQUENCE_PLAYER_ADDRESS(7);
		step_player_if_playing(player, mailbox_page);
		return;
	}
	z80_block(19);
	if (2u != tick_countdown)
	{
		z80_cycles(6);
		return;
	}
	z80_block(45);
	mailbox_page = sound_command_ADDRESS;
	if (midi_queue_count >= 0x40u)
	{
		z80_cycles(6);
		return;
	}
	z80_block(27);
	if (music_fade_speed != 0u)
	{
		z80_cycles(7);
		fade_music_volume(mailbox_page);
	}
	z80_block(32);
	player = SEQUENCE_PLAYER_ADDRESS(0);
	if (music_player_flags >> 7 != 0u)
	{
		step_player(player, mailbox_page);
		return;
	}
	z80_block(10);
}

void playlist_command(uint16_t queue_pointer, uint16_t player, uint16_t mailbox_page, uint16_t *p_playlist, uint8_t *p_playlist_byte)
{
	uint16_t playlist = (uint16_t)*p_playlist;
	uint8_t playlist_byte = (uint8_t)*p_playlist_byte;
	uint8_t next_song = 0;
	uint16_t cleared_player = 0, handler = 0;
	uint16_t saved_playlist = 0, saved_player = 0;
	uint32_t target1 = 0;

	z80_block(45);
	saved_playlist = playlist;
	playlist_byte &= 0xFu;
	handler = playlist_command_table_ADDRESS;
	handler = read_table_word(handler, playlist_byte);
	z80_block(4);
	target1 = handler;
	switch (target1)
	{
	case 0x0046B4u:
		z80_block(57);
		playlist = z80_read16(saved_playlist);
		next_song = z80_read8(playlist);
		playlist++;
		*p_playlist = playlist;
		*p_playlist_byte = next_song;
		return;
	case 0x0046BCu:
		z80_block(53);
		playlist = saved_playlist;
		next_song = (playlist_song_choice_1 & 7u) | 0x90u;
		*p_playlist = playlist;
		*p_playlist_byte = next_song;
		return;
	case 0x0046C5u:
		z80_block(53);
		playlist = saved_playlist;
		next_song = (playlist_song_choice_2 & 7u) | 0x90u;
		*p_playlist = playlist;
		*p_playlist_byte = next_song;
		return;
	case 0x0046CEu:
		z80_block(117);
		saved_player = player;
		cleared_player = saved_player;
		next_song = 0u;
		z80_write8(cleared_player, 0u);
		move_block(cleared_player, (uint16_t)(cleared_player + 1u), 0x30u, 1u);
		playlist = saved_playlist;
		*p_playlist = playlist;
		*p_playlist_byte = next_song;
		return;
	case 0x0046DFu:
		z80_block(32);
		if (queue_pointer >> 8 == 0xE2u)
		{
			z80_block(57);
			next_song = 0u;
			spare_players = 0u;
			move_block(spare_players_ADDRESS, 0xF101u, 0x4FFu, 1u);
		}
		else
		{
			z80_block(67);
			next_song = 0u;
			music_player_flags = 0u;
			move_block(SEQUENCE_PLAYER_ADDRESS(0), 0xE901u, 0x7FFu, 1u);
		}
		z80_block(30);
		playlist = saved_playlist;
		*p_playlist = playlist;
		*p_playlist_byte = next_song;
		return;
	default:
		sound_unknown_code(target1);
	}
}

void step_player_if_playing(uint16_t player_address, uint16_t mailbox_page)
{
	z80_block(17);
	if (SEQUENCE_PLAYER(player_address)->flags >> 7 == 0u)
	{
		z80_cycles(6);
		return;
	}
	step_player(player_address, mailbox_page);
}

void step_player(uint16_t player_address, uint16_t mailbox_page)
{
	uint32_t carry = 0;
	uint8_t delta_time_high = 0, delta_time_low = 0, event_byte = 0, meta_bytes_left = 0, new_delta_high = 0, queue_page = 0, queued_byte = 0, sysex_bytes_left = 0, volume_value = 0, write_index = 0;
	uint16_t cursor = 0, player = 0, player_2 = 0, queue_pointer = 0;
	uint8_t saved_queue_page = 0, saved_write_index = 0;
	uint16_t player_then_queue_pointer = 0, saved_player = 0;
	uint8_t controller_number = 0;
	uint16_t t1 = 0;

	z80_block(64);
	cursor = (player_address & ~0xFFu) | 6u;
	delta_time_low = AS8(SEQUENCE_PLAYER(cursor - 0x6u)->delta_time);
	cursor = (cursor & ~0xFFu) | (uint8_t)(cursor + 1u);
	delta_time_high = AT8(SEQUENCE_PLAYER(cursor - 0x7u)->delta_time, 1);
	t1 = (((uint32_t)delta_time_high << 8) | delta_time_low) - 1u;
	delta_time_high = (uint8_t)((uint16_t)((((uint32_t)delta_time_high << 8) | delta_time_low) - 1u) >> 8);
	delta_time_low = (uint8_t)t1;
	AT8(SEQUENCE_PLAYER(cursor - 0x7u)->delta_time, 1) = delta_time_high;
	cursor--;
	z80_write8(cursor, delta_time_low);
	if ((delta_time_high | delta_time_low) != 0u)
	{
		z80_cycles(6);
		return;
	}
	z80_block(56);
	queue_page = 0xE1u;
	write_index = midi_queue_write;
	player_then_queue_pointer = cursor & ~0xFFu;
	player = player_then_queue_pointer;
	for (;;)
	{
		z80_block(83);
		port_out(ROM_BANK_HIGH, 0u);
		port_out(ROM_BANK_LOW, SEQUENCE_PLAYER(player)->bank);
		cursor = SEQUENCE_PLAYER(player)->position;
		for (;;)
		{
			z80_block(48);
			SEQUENCE_PLAYER(player)->flags &= 0xBFu;
			event_byte = z80_read8(cursor);
			if (event_byte >> 7 == 0u)
			{
				z80_block(35);
				event_byte = SEQUENCE_PLAYER(player)->running_status;
				cursor--;
			}
			else
			{
				z80_block(19);
				SEQUENCE_PLAYER(player)->running_status = event_byte;
			}
			z80_block(17);
			if (event_byte == 0xFFu)
			{
				z80_block(17);
				cursor = next_sequence_byte(cursor, player);
				z80_block(24);
				event_byte = z80_read8(cursor);
				if (event_byte == 0x2Fu)
					goto end_of_sequence;
				z80_block(17);
				if (event_byte == 0x51u)
				{
					z80_block(17);
					cursor = next_sequence_byte(cursor, player);
					z80_block(17);
					cursor = next_sequence_byte(cursor, player);
					z80_block(25);
					event_byte = z80_read8(cursor);
					if (event_byte >> 7)
					{
						z80_block(23);
						event_byte = (0u - event_byte) | 0x80u;
					}
					z80_block(36);
					SEQUENCE_PLAYER(player)->meta_51_value = event_byte;
					cursor = next_sequence_byte(cursor, player);
					z80_block(10);
				}
				else
				{
					z80_block(17);
					cursor = next_sequence_byte(cursor, player);
					z80_block(7);
					meta_bytes_left = z80_read8(cursor);
					for (;;)
					{
						z80_block(17);
						cursor = next_sequence_byte(cursor, player);
						z80_block(8);
						meta_bytes_left--;
						if (meta_bytes_left == 0u)
							break;
						z80_cycles(5);
					}
					z80_block(17);
					cursor = next_sequence_byte(cursor, player);
					z80_block(10);
				}
			read_delta_time:
				z80_block(30);
				if (((SEQUENCE_PLAYER(player)->flags >> 6) & 1u) != 0u)
					continue;
				z80_block(24);
				event_byte = z80_read8(cursor);
				cursor = next_sequence_byte(cursor, player);
				z80_block(14);
				if (event_byte == 0u)
					continue;
				goto set_delta_time;
			}
			z80_block(28);
			queued_byte = event_byte;
			event_byte &= ~0xFu;
			if (event_byte == 0xF0u)
			{
				z80_block(21);
				event_byte = queued_byte;
				cursor = next_sequence_byte(cursor, player);
				z80_block(17);
				if (event_byte == 0xF7u)
				{
					z80_block(7);
					sysex_bytes_left = z80_read8(cursor);
					for (;;)
					{
						z80_block(24);
						cursor = next_sequence_byte(cursor, player);
						z80_block(8);
						sysex_bytes_left--;
						if (sysex_bytes_left == 0u)
							break;
						z80_cycles(5);
					}
					z80_block(10);
				count_midi_message:
					z80_block(33);
					midi_queue_count++;
					goto read_delta_time;
				}
				z80_block(17);
				if (event_byte == 0xF0u)
				{
					for (;;)
					{
						z80_block(24);
						event_byte = z80_read8(cursor);
						cursor = next_sequence_byte(cursor, player);
						z80_block(17);
						if (event_byte == 0xF7u)
							break;
						z80_block(10);
					}
					goto count_midi_message;
				}
				break;
			}
			z80_block(17);
			if (event_byte == 0x80u)
			{
				z80_block(32);
				z80_write8((uint16_t)(((uint32_t)queue_page << 8) | write_index), queued_byte);
				write_index++;
				cursor = next_sequence_byte(cursor, player);
				z80_block(25);
				event_byte = z80_read8(cursor);
				if (event_byte >> 7 != 0u)
				{
					z80_cycles(7);
					event_byte = chain_next_event(player, event_byte);
				}
				z80_block(28);
				z80_write8((uint16_t)(((uint32_t)queue_page << 8) | write_index), event_byte);
				write_index++;
				cursor = next_sequence_byte(cursor, player);
				z80_block(51);
				z80_write8((uint16_t)(((uint32_t)queue_page << 8) | write_index), 0x7Fu);
				write_index++;
				midi_queue_count++;
				goto read_delta_time;
			}
			z80_block(17);
			if (event_byte == 0xB0u)
			{
				z80_block(32);
				z80_write8((uint16_t)(((uint32_t)queue_page << 8) | write_index), queued_byte);
				write_index++;
				cursor = next_sequence_byte(cursor, player);
				z80_block(35);
				event_byte = z80_read8(cursor);
				z80_write8((uint16_t)(((uint32_t)queue_page << 8) | write_index), event_byte);
				write_index++;
				cursor = next_sequence_byte(cursor, player);
				z80_block(24);
				controller_number = event_byte;
				event_byte = z80_read8(cursor);
				if (controller_number == 0x10u)
				{
					z80_block(37);
					volume_value = event_byte;
					if (SEQUENCE_PLAYER(player)->fade_speed == 0u)
					{
						z80_block(31);
						event_byte = volume_value;
						SEQUENCE_PLAYER(player)->fade_volume = volume_value & 0x7Fu;
					}
					else
					{
						z80_block(29);
						event_byte = SEQUENCE_PLAYER(player)->fade_volume;
					}
				}
				z80_block(18);
				if (event_byte >> 7 != 0u)
				{
					z80_cycles(7);
					event_byte = chain_next_event(player, event_byte);
				}
				z80_block(28);
				z80_write8((uint16_t)(((uint32_t)queue_page << 8) | write_index), event_byte);
				write_index++;
				cursor = next_sequence_byte(cursor, player);
				z80_block(33);
				midi_queue_count++;
				goto read_delta_time;
			}
			z80_block(17);
			if (event_byte != 0xC0u)
			{
				z80_block(17);
				if (event_byte != 0xD0u)
				{
					z80_block(10);
					z80_block(32);
					z80_write8((uint16_t)(((uint32_t)queue_page << 8) | write_index), queued_byte);
					write_index++;
					cursor = next_sequence_byte(cursor, player);
					z80_block(7);
					queued_byte = z80_read8(cursor);
				}
			}
			z80_block(32);
			z80_write8((uint16_t)(((uint32_t)queue_page << 8) | write_index), queued_byte);
			write_index++;
			cursor = next_sequence_byte(cursor, player);
			z80_block(25);
			event_byte = z80_read8(cursor);
			if (event_byte >> 7 != 0u)
			{
				z80_cycles(7);
				event_byte = chain_next_event(player, event_byte);
			}
			z80_block(28);
			z80_write8((uint16_t)(((uint32_t)queue_page << 8) | write_index), event_byte);
			write_index++;
			cursor = next_sequence_byte(cursor, player);
			goto count_midi_message;
		}
		z80_block(10);
	end_of_sequence:
		z80_block(30);
		if (((SEQUENCE_PLAYER(player)->flags >> 5) & 1u) == 0u)
		{
			z80_block(30);
			if (((SEQUENCE_PLAYER(player)->flags >> 4) & 1u) != 0u)
			{
				z80_cycles(7);
				clear_player_end_flag(mailbox_page);
			}
			z80_block(100);
			SEQUENCE_PLAYER(player)->flags = 0u;
			midi_queue_write = write_index;
			port_out(ROM_BANK_HIGH, rom_bank_high);
			port_out(ROM_BANK_LOW, rom_bank_low);
			return;
		}
		z80_block(62);
		cursor = SEQUENCE_PLAYER(player)->playlist;
		event_byte = z80_read8(cursor);
		cursor = next_sequence_byte(cursor, player);
		z80_block(17);
		if (event_byte >= 0xF0u)
		{
			z80_cycles(7);
			queue_pointer = (uint32_t)queue_page << 8;
			playlist_command(queue_pointer, player, mailbox_page, &cursor, &event_byte);
		}
		z80_block(14);
		if (event_byte == 0u)
		{
			z80_block(30);
			if (((SEQUENCE_PLAYER(player)->flags >> 4) & 1u) != 0u)
			{
				z80_cycles(7);
				clear_player_end_flag(mailbox_page);
			}
			z80_block(100);
			SEQUENCE_PLAYER(player)->flags = 0u;
			midi_queue_write = write_index;
			port_out(ROM_BANK_HIGH, rom_bank_high);
			port_out(ROM_BANK_LOW, rom_bank_low);
			return;
		}
		z80_block(91);
		SEQUENCE_PLAYER(player)->playlist = cursor;
		saved_queue_page = queue_page;
		saved_write_index = write_index;
		saved_player = player;
		player_2 = (uint32_t)(saved_player >> 8) << 8;
		start_song(player_2, event_byte);
		z80_block(20);
		queue_page = saved_queue_page;
		write_index = saved_write_index;
	}
set_delta_time:
	z80_block(18);
	if (event_byte >> 7)
	{
		z80_block(37);
		event_byte &= 0x7Fu;
		carry = event_byte & 1u;
		new_delta_high = event_byte >> 1;
		event_byte = z80_read8(cursor);
		if (carry)
		{
			z80_block(8);
			event_byte |= 0x80u;
		}
		z80_block(17);
		cursor = next_sequence_byte(cursor, player);
		z80_block(19);
		AT8(SEQUENCE_PLAYER(player)->delta_time, 1) = new_delta_high;
	}
	z80_block(134);
	AS8(SEQUENCE_PLAYER(player)->delta_time) = event_byte;
	SEQUENCE_PLAYER(player)->position = cursor;
	midi_queue_write = write_index;
	port_out(ROM_BANK_HIGH, rom_bank_high);
	port_out(ROM_BANK_LOW, rom_bank_low);
}

uint8_t chain_next_event(uint16_t player, uint8_t data_byte)
{
	z80_block(41);
	data_byte &= 0x7Fu;
	SEQUENCE_PLAYER(player)->flags |= 0x40u;
	return data_byte;
}

void clear_player_end_flag(uint16_t mailbox_page)
{
	z80_block(33);
	player_end_flag = 0u;
}

void poll_sound_command(void)
{
	uint8_t byte = 0, command_number = 0;
	uint16_t address = 0, header = 0;
	uint16_t exchanged_player = 0;

	z80_block(26);
	address = sound_command_group_ADDRESS;
	byte = sound_command_group;
	if (byte == 0u)
	{
		z80_cycles(6);
		return;
	}
	z80_block(55);
	sound_command_group = 0u;
	command_number = z80_read8((address & ~0xFFu) | (uint8_t)(address - 1u));
	byte &= 0x7Fu;
	address = sound_groups_ADDRESS;
	address = read_table_word(address, byte);
	z80_block(16);
	if (z80_read8(address) < command_number)
	{
		z80_cycles(6);
		return;
	}
	z80_block(37);
	byte = command_number;
	address++;
	header = SEQUENCE_PLAYER_ADDRESS(0);
	address = read_table_word(address, byte);
	z80_block(29);
	exchanged_player = header;
	header = address;
	address = exchanged_player;
	byte = z80_read8(header);
	if (byte >> 7 != 0u)
	{
		z80_block(18);
		if (((byte >> 6) & 1u) != 0u)
		{
			z80_block(29);
			address = (uint32_t)(uint8_t)((address >> 8) + 1u) << 8;
			address = (uint32_t)(uint8_t)((address >> 8) + 1u) << 8;
			address = (uint32_t)(uint8_t)((address >> 8) + 1u) << 8;
			select_channel_player(&header, &address);
			z80_block(46);
			byte = z80_read8(header) & 0xFu;
			if (byte < (z80_read8(address) & 0xFu))
			{
				z80_block(10);
				return;
			}
			z80_block(17);
			address = clear_player(address);
			z80_block(87);
			header++;
			SEQUENCE_PLAYER(address)->flags = byte;
			address = (address & ~0xFFu) | 2u;
			AS8(SEQUENCE_PLAYER(address - 0x2u)->position) = (uint8_t)header;
			address = (address & ~0xFFu) | (uint8_t)(address + 1u);
			AT8(SEQUENCE_PLAYER(address - 0x3u)->position, 1) = header >> 8;
			address = (address & ~0xFFu) | 6u;
			AS8(SEQUENCE_PLAYER(address - 0x6u)->delta_time) = 1u;
			address &= ~0xFFu;
			SEQUENCE_PLAYER(address)->flags |= 0x80u;
			return;
		}
		z80_block(10);
		z80_block(54);
		address = (uint32_t)(uint8_t)((address >> 8) + 1u) << 8;
		address = (uint32_t)(uint8_t)((address >> 8) + 1u) << 8;
		byte = z80_read8(header) & 0xFu;
		if (byte < (SEQUENCE_PLAYER(address)->flags & 0xFu))
		{
			z80_block(10);
			return;
		}
		z80_block(17);
		address = clear_player(address);
		z80_block(7);
		SEQUENCE_PLAYER(address)->flags = byte;
		z80_block(80);
		header++;
		address = (address & ~0xFFu) | 2u;
		AS8(SEQUENCE_PLAYER(address - 0x2u)->position) = (uint8_t)header;
		address = (address & ~0xFFu) | (uint8_t)(address + 1u);
		AT8(SEQUENCE_PLAYER(address - 0x3u)->position, 1) = header >> 8;
		address = (address & ~0xFFu) | 6u;
		AS8(SEQUENCE_PLAYER(address - 0x6u)->delta_time) = 1u;
		address &= ~0xFFu;
		SEQUENCE_PLAYER(address)->flags |= 0x80u;
		return;
	}
	z80_block(18);
	if (((byte >> 6) & 1u) != 0u)
	{
		z80_block(80);
		header++;
		address = (address & ~0xFFu) | 2u;
		AS8(SEQUENCE_PLAYER(address - 0x2u)->position) = (uint8_t)header;
		address = (address & ~0xFFu) | (uint8_t)(address + 1u);
		AT8(SEQUENCE_PLAYER(address - 0x3u)->position, 1) = header >> 8;
		address = (address & ~0xFFu) | 6u;
		AS8(SEQUENCE_PLAYER(address - 0x6u)->delta_time) = 1u;
		address &= ~0xFFu;
		SEQUENCE_PLAYER(address)->flags |= 0x80u;
		return;
	}
	z80_block(18);
	if (((byte >> 5) & 1u) != 0u)
	{
		start_sequence(header, address);
		return;
	}
	z80_block(10);
	z80_block(17);
	address = clear_player(address);
	z80_block(82);
	header++;
	byte = z80_read8(header);
	SEQUENCE_PLAYER(address)->flags |= 0x20u;
	header++;
	address = (address & ~0xFFu) | 0x20u;
	AS8(SEQUENCE_PLAYER(address - 0x20u)->playlist) = (uint8_t)header;
	address++;
	AT8(SEQUENCE_PLAYER(address - 0x21u)->playlist, 1) = header >> 8;
	header = address & ~0xFFu;
	start_song(header, byte);
}

void start_song(uint16_t player, uint8_t song_number)
{
	uint16_t address = 0, song_header = 0;
	uint16_t exchanged_player = 0;

	z80_block(27);
	address = songs_ADDRESS;
	address = read_table_word(address, song_number);
	z80_block(21);
	exchanged_player = player;
	song_header = address;
	address = exchanged_player;
	start_sequence(song_header, address);
}

void start_sequence(uint16_t header, uint16_t player)
{
	uint8_t bank = 0;
	uint16_t player_field = 0;

	z80_block(127);
	header++;
	player_field = (player & ~0xFFu) | 0x1Bu;
	bank = z80_read8(header);
	header++;
	SEQUENCE_PLAYER(player_field - 0x1Bu)->bank = bank;
	player_field = (player_field & ~0xFFu) | 2u;
	AS8(SEQUENCE_PLAYER(player_field - 0x2u)->position) = z80_read8(header);
	player_field = (player_field & ~0xFFu) | (uint8_t)(player_field + 1u);
	AT8(SEQUENCE_PLAYER(player_field - 0x3u)->position, 1) = z80_read8((uint16_t)(header + 1u));
	player_field = (player_field & ~0xFFu) | 6u;
	AS8(SEQUENCE_PLAYER(player_field - 0x6u)->delta_time) = 1u;
	player_field &= ~0xFFu;
	SEQUENCE_PLAYER(player_field)->flags |= 0x80u;
}

void select_channel_player(uint16_t *p_header, uint16_t *p_player)
{
	uint16_t header = (uint16_t)*p_header;
	uint16_t player = (uint16_t)*p_player;
	uint8_t player_page = 0;

	z80_block(55);
	header++;
	player_page = (z80_read8(header) & 0xFu) - 0xAu + (player >> 8);
	header--;
	player = (uint32_t)player_page << 8;
	*p_header = header;
	*p_player = (*p_player & 0xFFu) | player;
}

uint16_t clear_player(uint16_t player)
{
	uint16_t saved_player = 0;

	z80_block(101);
	saved_player = player;
	SEQUENCE_PLAYER(player)->flags = 0u;
	move_block(player, (uint16_t)(saved_player + 1u), 0x30u, 1u);
	player = (player + 0x30u) & 0xFF00u;
	return player;
}

void fade_music_volume(uint16_t mailbox_page)
{
	uint8_t queue_page = 0, write_index = 0;
	uint16_t fade_field = 0;

	z80_block(28);
	fade_field = music_fade_wait_ADDRESS;
	if (music_fade_wait != 0u)
	{
		z80_block(21);
		SEQUENCE_PLAYER(fade_field - 0x19u)->fade_wait--;
		return;
	}
	z80_cycles(5);
	z80_block(63);
	queue_page = 0xE1u;
	write_index = midi_queue_write;
	fade_field = (fade_field & ~0xFFu) | 0x15u;
	SEQUENCE_PLAYER(fade_field - 0x15u)->fade_volume--;
	if (SEQUENCE_PLAYER(fade_field - 0x15u)->fade_volume == 0u)
	{
		z80_block(223);
		fade_field &= ~0xFFu;
		SEQUENCE_PLAYER(fade_field)->flags &= 0x7Fu;
		SEQUENCE_PLAYER(fade_field)->fade_speed = 0u;
		z80_write8((uint16_t)(((uint32_t)queue_page << 8) | write_index), 0xBEu);
		write_index++;
		z80_write8((uint16_t)(((uint32_t)queue_page << 8) | write_index), 0x50u);
		write_index++;
		z80_write8((uint16_t)(((uint32_t)queue_page << 8) | write_index), 0x7Eu);
		write_index++;
		midi_queue_count++;
		z80_write8((uint16_t)(((uint32_t)queue_page << 8) | write_index), 0xBEu);
		write_index++;
		z80_write8((uint16_t)(((uint32_t)queue_page << 8) | write_index), 0x79u);
		write_index++;
		z80_write8((uint16_t)(((uint32_t)queue_page << 8) | write_index), 0u);
		midi_queue_count++;
		midi_queue_write = write_index + 1u;
		return;
	}
	z80_block(134);
	z80_write8((uint16_t)(((uint32_t)queue_page << 8) | write_index), 0xBEu);
	write_index++;
	z80_write8((uint16_t)(((uint32_t)queue_page << 8) | write_index), 0x10u);
	write_index++;
	z80_write8((uint16_t)(((uint32_t)queue_page << 8) | write_index), SEQUENCE_PLAYER(fade_field - 0x15u)->fade_volume);
	midi_queue_write = write_index + 1u;
	midi_queue_count++;
	fade_field = (fade_field & ~0xFFu) | 0x18u;
	SEQUENCE_PLAYER(fade_field & ~0xFFu)->fade_wait = SEQUENCE_PLAYER(fade_field - 0x18u)->fade_speed;
}

void fade_player_volume(uint16_t queue_pointer, uint16_t player, uint16_t mailbox_page)
{
	uint8_t write_index = 0;
	uint16_t fade_field = 0;

	write_index = (uint8_t)queue_pointer;
	z80_block(23);
	fade_field = (player & ~0xFFu) | 0x18u;
	if (SEQUENCE_PLAYER(fade_field - 0x18u)->fade_speed == 0u)
	{
		z80_cycles(6);
		return;
	}
	z80_block(28);
	fade_field = (fade_field & ~0xFFu) | 0x19u;
	if (SEQUENCE_PLAYER(fade_field - 0x19u)->fade_wait != 0u)
	{
		z80_block(21);
		SEQUENCE_PLAYER(fade_field - 0x19u)->fade_wait--;
		return;
	}
	z80_block(39);
	fade_field = (fade_field & ~0xFFu) | 0x1Au;
	SEQUENCE_PLAYER(fade_field - 0x1Au)->fade_channel_volume--;
	if (SEQUENCE_PLAYER(fade_field - 0x1Au)->fade_channel_volume == 0u)
	{
		z80_block(138);
		fade_field &= ~0xFFu;
		SEQUENCE_PLAYER(fade_field)->flags = 0u;
		SEQUENCE_PLAYER(fade_field)->fade_speed = 0u;
		z80_write8((queue_pointer & ~0xFFu) | write_index, 0xBAu);
		write_index++;
		z80_write8((queue_pointer & ~0xFFu) | write_index, 0x7Bu);
		write_index++;
		z80_write8((queue_pointer & ~0xFFu) | write_index, 0u);
		midi_queue_count++;
		midi_queue_write = write_index + 1u;
		return;
	}
	z80_block(134);
	z80_write8((queue_pointer & ~0xFFu) | write_index, 0xBAu);
	write_index++;
	z80_write8((queue_pointer & ~0xFFu) | write_index, 7u);
	write_index++;
	z80_write8((queue_pointer & ~0xFFu) | write_index, SEQUENCE_PLAYER(fade_field - 0x1Au)->fade_channel_volume);
	midi_queue_write = write_index + 1u;
	midi_queue_count++;
	fade_field = (fade_field & ~0xFFu) | 0x18u;
	SEQUENCE_PLAYER(fade_field & ~0xFFu)->fade_wait = SEQUENCE_PLAYER(fade_field - 0x18u)->fade_speed;
}

uint16_t next_sequence_byte(uint16_t cursor, uint16_t player)
{
	z80_block(38);
	cursor++;
	if (cursor >> 8 == 0xC0u)
	{
		z80_block(60);
		SEQUENCE_PLAYER(player)->bank++;
		port_out(ROM_BANK_LOW, SEQUENCE_PLAYER(player)->bank);
		cursor = (cursor & 0xFFu) | 0xA000u;
	}
	z80_block(20);
	return cursor;
}

void step_fm_vibrato(uint16_t voice)
{
	uint8_t center_high = 0, center_low = 0, fall_block = 0, fall_limit = 0, fm_register = 0, rise_block = 0, rise_limit = 0, value = 0;
	uint16_t fnumber = 0, frequency = 0;
	uint8_t saved_center_high = 0, saved_center_low = 0;
	uint16_t word_copy = 0;
	uint8_t fall_limit_bits = 0;
	uint16_t fall_floor = 0, fall_fnumber = 0, center_for_fall = 0, rise_ceiling = 0, rise_fnumber = 0, center_for_rise = 0;

	z80_block(33);
	if (FM_VOICE(voice)->vibrato_delay != 0u)
	{
		z80_block(33);
		FM_VOICE(voice)->vibrato_delay--;
		return;
	}
	z80_block(33);
	if (FM_VOICE(voice)->vibrato_wait != 0u)
	{
		z80_block(33);
		FM_VOICE(voice)->vibrato_wait--;
		return;
	}
	z80_block(305);
	FM_VOICE(voice)->vibrato_wait = FM_VOICE(voice)->vibrato_rate;
	fm_channel = FM_VOICE(voice)->chip_channel;
	value = FM_VOICE(voice)->note;
	fnumber = fm_fnum_tables_ADDRESS + (uint8_t)((value & 0xFu) * 2u);
	frequency = pitch_bend;
	fnumber = SOUND_ROM16(fnumber) + frequency + frequency;
	fnumber = z80_read16(fnumber);
	vibrato_center = fnumber;
	fnumber = (fnumber & 0xFFu) | ((uint32_t)(uint8_t)(((value & 0x70u) >> 1) + (fnumber >> 8)) << 8);
	if ((FM_VOICE(voice)->vibrato_phase & 1u) != 0u)
	{
		z80_block(214);
		frequency = fnumber;
		fnumber = FM_VOICE(voice)->vibrato_offset - FM_VOICE(voice)->vibrato_fall;
		FM_VOICE(voice)->vibrato_offset = fnumber;
		fnumber += (frequency & 0xFFu) | (((frequency >> 8) & 7u) << 8);
		fnumber = (fnumber & 0xFFu) | ((uint32_t)(uint8_t)(((frequency >> 8) & ~7u) + (fnumber >> 8)) << 8);
		if ((FM_VOICE(voice)->vibrato_range & 0x70u) == 0u)
		{
		vibrato_fall_last_octave:
			z80_block(167);
			FM_VOICE(voice)->vibrato_range &= 0xFu;
			word_copy = fnumber;
			frequency = word_copy;
			fnumber = vibrato_center - (((FM_VOICE(voice)->vibrato_range & 7u) << 8) | FM_VOICE(voice)->vibrato_depth);
			if (((fnumber >> 8) & 0xC0u) != 0u)
			{
				z80_block(83);
				word_copy = frequency;
				fnumber = frequency;
				if (word_copy > 0u)
				{
					z80_block(97);
					FM_VOICE(voice)->vibrato_range += FM_VOICE(voice)->vibrato_phase & 0x70u;
					FM_VOICE(voice)->vibrato_phase--;
				}
			}
			else
			{
				z80_block(69);
				word_copy = frequency;
				fall_floor = fnumber;
				fnumber = frequency;
				if ((uint16_t)((((word_copy >> 8) & 7u) << 8) | (uint8_t)word_copy) <= fall_floor)
				{
					z80_block(10);
					z80_block(97);
					FM_VOICE(voice)->vibrato_range += FM_VOICE(voice)->vibrato_phase & 0x70u;
					FM_VOICE(voice)->vibrato_phase--;
					z80_block(94);
					fnumber = (fnumber & 0xFFu) | ((uint32_t)(uint8_t)((FM_VOICE(voice)->vibrato_block & ~7u) + (fnumber >> 8)) << 8);
					frequency = (fm_channel & 3u) << 8;
					value = (uint8_t)(fnumber >> 8);
					fm_register = 0xA4u | (frequency >> 8);
					write_fm_channel(fm_register, value);
					z80_block(36);
					value = (uint8_t)fnumber;
					fm_register = 0xA0u | (frequency >> 8);
					write_fm_channel(fm_register, value);
					z80_block(10);
					return;
				}
			}
		}
		else
		{
			z80_block(83);
			word_copy = fnumber;
			fall_fnumber = (fnumber & 0xFFu) | (((fnumber >> 8) & 7u) << 8);
			fnumber = word_copy;
			if (fall_fnumber < 0x269u)
			{
				z80_block(304);
				frequency = fnumber;
				center_for_fall = vibrato_center;
				center_high = (uint8_t)(center_for_fall >> 8);
				center_low = (uint8_t)center_for_fall;
				fnumber = 0x4CFu - (((uint32_t)center_high << 8) | center_low);
				FM_VOICE(voice)->vibrato_offset = fnumber;
				saved_center_high = center_high;
				saved_center_low = center_low;
				fnumber = (((uint32_t)saved_center_high << 8) | saved_center_low) + FM_VOICE(voice)->vibrato_offset;
				fnumber = (fnumber & 0xFFu) | ((uint32_t)(uint8_t)(((frequency >> 8) & 0x38u) + (fnumber >> 8)) << 8);
				frequency = ((fnumber >> 8) & 0x38u) << 8;
				fall_block = (FM_VOICE(voice)->vibrato_block & ~7u) + (frequency >> 8);
				value = 0u - ((FM_VOICE(voice)->vibrato_range & 0x70u) >> 1) + (frequency >> 8);
				fall_limit = value;
				fall_limit_bits = value & ~7u;
				value = fall_limit_bits;
				if (fall_limit_bits != 0u)
				{
					z80_block(17);
					if ((value & 0xC0u) != 0u)
						goto vibrato_fall_block_check;
					z80_block(18);
					if (fall_block <= fall_limit)
						goto vibrato_fall_into_last_octave;
					z80_block(10);
					z80_block(59);
					FM_VOICE(voice)->vibrato_block -= 8u;
					z80_block(94);
					fnumber = (fnumber & 0xFFu) | ((uint32_t)(uint8_t)((FM_VOICE(voice)->vibrato_block & ~7u) + (fnumber >> 8)) << 8);
					frequency = (fm_channel & 3u) << 8;
					value = (uint8_t)(fnumber >> 8);
					fm_register = 0xA4u | (frequency >> 8);
					write_fm_channel(fm_register, value);
					z80_block(36);
					value = (uint8_t)fnumber;
					fm_register = 0xA0u | (frequency >> 8);
					write_fm_channel(fm_register, value);
					z80_block(10);
					return;
				}
			vibrato_fall_block_check:
				z80_block(21);
				if (fall_block <= 8u)
				{
				vibrato_fall_into_last_octave:
					z80_block(49);
					FM_VOICE(voice)->vibrato_block -= 8u;
					goto vibrato_fall_last_octave;
				}
				z80_block(59);
				FM_VOICE(voice)->vibrato_block -= 8u;
			}
		}
	}
	else
	{
		z80_block(164);
		frequency = fnumber;
		fnumber = FM_VOICE(voice)->vibrato_offset + FM_VOICE(voice)->vibrato_rise;
		FM_VOICE(voice)->vibrato_offset = fnumber;
		fnumber += frequency;
		if ((FM_VOICE(voice)->vibrato_range & 0x70u) == 0u)
		{
		vibrato_rise_last_octave:
			z80_block(170);
			FM_VOICE(voice)->vibrato_range &= 0xFu;
			word_copy = fnumber;
			frequency = word_copy;
			fnumber = vibrato_center + (((FM_VOICE(voice)->vibrato_range & 7u) << 8) | FM_VOICE(voice)->vibrato_depth);
			if (((fnumber >> 8) & 0xFu) >= 4u)
			{
				z80_block(7);
				fnumber = (fnumber & 0xFFu) | 0x500u;
			}
			else
				z80_block(10);
			z80_block(84);
			word_copy = frequency;
			rise_ceiling = (fnumber & 0xFFu) | (((fnumber >> 8) & 7u) << 8);
			fnumber = frequency;
			if (rise_ceiling < (uint16_t)((((word_copy >> 8) & 7u) << 8) | (uint8_t)word_copy))
			{
				z80_block(97);
				FM_VOICE(voice)->vibrato_range += FM_VOICE(voice)->vibrato_phase & 0x70u;
				FM_VOICE(voice)->vibrato_phase++;
			}
		}
		else
		{
			z80_block(83);
			word_copy = fnumber;
			rise_fnumber = (fnumber & 0xFFu) | (((fnumber >> 8) & 7u) << 8);
			fnumber = word_copy;
			if (rise_fnumber >= 0x4CEu)
			{
				z80_block(314);
				frequency = fnumber;
				center_for_rise = vibrato_center;
				center_high = (uint8_t)(center_for_rise >> 8);
				center_low = (uint8_t)center_for_rise;
				fnumber = 0x269u - (((uint32_t)center_high << 8) | center_low);
				FM_VOICE(voice)->vibrato_offset = fnumber;
				saved_center_high = center_high;
				saved_center_low = center_low;
				fnumber = (((uint32_t)saved_center_high << 8) | saved_center_low) + FM_VOICE(voice)->vibrato_offset;
				fnumber = (fnumber & 0xFFu) | ((uint32_t)(uint8_t)(((frequency >> 8) & 0x38u) + (fnumber >> 8)) << 8);
				frequency = ((fnumber >> 8) & 0x38u) << 8;
				rise_block = ((FM_VOICE(voice)->vibrato_block & ~7u) + (frequency >> 8)) & 0xF8u;
				value = ((FM_VOICE(voice)->vibrato_range & 0x70u) >> 1) + (frequency >> 8) - 8u;
				rise_limit = value;
				if (value >= 0x30u)
				{
					z80_block(7);
					rise_limit = 0x28u;
				}
				z80_block(21);
				if ((rise_limit & 0xC0u) == 0u)
				{
					z80_block(18);
					if (rise_limit < rise_block)
						goto vibrato_rise_into_last_octave;
					z80_block(10);
					z80_block(45);
					FM_VOICE(voice)->vibrato_block += 8u;
					z80_block(94);
					fnumber = (fnumber & 0xFFu) | ((uint32_t)(uint8_t)((FM_VOICE(voice)->vibrato_block & ~7u) + (fnumber >> 8)) << 8);
					frequency = (fm_channel & 3u) << 8;
					value = (uint8_t)(fnumber >> 8);
					fm_register = 0xA4u | (frequency >> 8);
					write_fm_channel(fm_register, value);
					z80_block(36);
					value = (uint8_t)fnumber;
					fm_register = 0xA0u | (frequency >> 8);
					write_fm_channel(fm_register, value);
					z80_block(10);
					return;
				}
				z80_block(25);
				if ((uint8_t)(rise_limit + 8u) < rise_block)
				{
				vibrato_rise_into_last_octave:
					z80_block(45);
					FM_VOICE(voice)->vibrato_block += 8u;
					goto vibrato_rise_last_octave;
				}
				z80_block(45);
				FM_VOICE(voice)->vibrato_block += 8u;
			}
		}
	}
	z80_block(94);
	fnumber = (fnumber & 0xFFu) | ((uint32_t)(uint8_t)((FM_VOICE(voice)->vibrato_block & ~7u) + (fnumber >> 8)) << 8);
	frequency = (fm_channel & 3u) << 8;
	value = (uint8_t)(fnumber >> 8);
	fm_register = 0xA4u | (frequency >> 8);
	write_fm_channel(fm_register, value);
	z80_block(36);
	value = (uint8_t)fnumber;
	fm_register = 0xA0u | (frequency >> 8);
	write_fm_channel(fm_register, value);
	z80_block(10);
}
