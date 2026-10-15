/**
 *    Name:        system/interrupts.c
 *    Description: the interrupt handlers: VBLANK start and end, sound, timers; the VBLANK copies, the sound mailbox, the frame and test switch counts
 *    Version:     0.1.0
 *    Created:     12-10-2026
 *    Modified:    15-10-2026
 *    Author:      Sebastian Toyos
 *
 *    SegaSonic The Hedgehog (arcade, Sega System 32): the game's V60 program decompiled into C by
 *    tools/decompile.py, with the names of symbols.txt.
 */
#include "game.h"

void irq_vblank_start(void)
{
	uint32_t zero = 0;
	uint32_t dest = 0, ignored = 0, source = 0;
	uint32_t saved_r14 = 0;
	uint8_t frames_left = 0, cnt_bits = 0;

	saved_r14 = r14;
	move_halfwords(MIXER_REGISTERS + 0x30u, mixer_text_blend_ADDRESS, 0x10u);
	if (vblank_copies_pending != 0u)
		run_vblank_copies(&source, &dest, &ignored, &ignored);
	call_code(vblank_start_hook);
	send_sound_command();
	if (test_mode == 0u)
	{
		count_frames_and_check_test_switch();
		if (free_play == 0u)
			update_coins();
	}
	frames_left = display_off_frames - 1u;
	zero = frames_left == 0u;
	display_off_frames = frames_left;
	if ((int8_t)frames_left >= 0)
	{
		if (zero == 0u)
		{
			mem_write8(irq_acknowledge, 0xFEu);
			vblank_counter++;
			r14 = saved_r14;
			return;
		}
		cnt_bits = mem_read8(io_cnt);
		mem_write8(io_cnt, cnt_bits | 2u);
	}
	display_off_frames = 0xFFu;
	mem_write8(irq_acknowledge, 0xFEu);
	vblank_counter++;
	r14 = saved_r14;
}

void irq_vblank_end(void)
{
	if (debug_pause == 0u)
		call_code(vblank_hook);
	frames_to_wait--;
	mem_write8(irq_acknowledge, 0xFDu);
}

void irq_sound(void)
{
	mem_write8(irq_acknowledge, 0xFBu);
}

void irq_timer0(void)
{
	timer_0_ticks++;
	mem_write16(timer_0_reload, 0x32u);
	mem_write8(irq_acknowledge, 0xF7u);
}

void irq_timer1(void)
{
	timer_1_ticks++;
	mem_write16(timer_1_reload, 0x23u);
	mem_write8(irq_acknowledge, 0xEFu);
}

void int_default(uint32_t *p_source, uint32_t *p_dest, uint32_t *p_copies_left, uint32_t *p_copy_entry)
{
	uint32_t source = *p_source;
	uint32_t dest = *p_dest;
	uint32_t r3 = 0, r4 = 0;

	halt();
	halt();
	halt();
	halt();
	int_vector_02(&source, &dest, &r3, &r4);
	*p_source = source;
	*p_dest = dest;
	*p_copies_left = r3;
	*p_copy_entry = r4;
}

void int_vector_02(uint32_t *p_source, uint32_t *p_dest, uint32_t *p_copies_left, uint32_t *p_copy_entry)
{
	uint32_t source = *p_source;
	uint32_t dest = *p_dest;
	uint32_t r3 = 0, r4 = 0;

	halt();
	halt();
	halt();
	halt();
	run_vblank_copies(&source, &dest, &r3, &r4);
	*p_source = source;
	*p_dest = dest;
	*p_copies_left = r3;
	*p_copy_entry = r4;
}

void run_vblank_copies(uint32_t *p_source, uint32_t *p_dest, uint32_t *p_copies_left, uint32_t *p_copy_entry)
{
	uint32_t source = *p_source;
	uint32_t dest = *p_dest;
	uint8_t repeats_left = 0;
	uint32_t copies_left = 0, copy_entry = 0, copy_halfwords = 0, dest_end = 0, ignored = 0;
	uint8_t repeats_before = 0;

	vblank_copies_pending = 0u;
	copies_left = vblank_copy_count;
	vblank_copy_count = 0u;
	copy_entry = vblank_copies_ADDRESS;
	if ((int16_t)copies_left <= 0)
	{
		*p_copies_left = copies_left;
		*p_copy_entry = copy_entry;
		return;
	}
	do
	{
		source = mem_read32(copy_entry);
		copy_entry += 4u;
		dest = mem_read32(copy_entry);
		copy_entry += 4u;
		copy_halfwords = mem_read16(copy_entry);
		repeats_left = (uint8_t)((dest << 8) | (dest >> 24));
		copy_entry += 2u;
		for (;;)
		{
			move_string(source, copy_halfwords, dest, copy_halfwords, 2u, 0u, 0u, fill_value, &dest_end, &ignored);
			repeats_before = repeats_left;
			repeats_left = (uint8_t)(repeats_before - 1u);
			if ((int8_t)repeats_before <= 1)
				break;
			dest = dest_end;
		}
		copies_left--;
	} while (copies_left != 0u);
	*p_source = source;
	*p_dest = dest;
	*p_copies_left = copies_left;
	*p_copy_entry = copy_entry;
}

void send_sound_command(void)
{
	uint32_t command_entry = 0, queue_base = 0, read_index = 0;

	read_index = sound_queue_read;
	queue_base = sound_queue_ADDRESS;
	if (read_index != sound_queue_write)
	{
		command_entry = sound_commands_ADDRESS + mem_read8(queue_base + read_index) * 2u;
		mem_write8(sound_command_group, ROM8(command_entry));
		mem_write8(sound_command, ROM8(command_entry + 1u));
		sound_queue_read = (read_index + 1u) & 0x1Fu;
	}
}

void read_service_inputs(void)
{
	uint16_t inputs = 0, previous = 0;
	uint8_t port_f_bits = 0, port_e_bits = 0;

	port_f_bits = mem_read8(io_port_f);
	port_e_bits = mem_read8(io_port_e);
	inputs = (uint16_t)((uint16_t)((uint32_t)port_f_bits << 8) | port_e_bits);
	previous = service_inputs;
	service_inputs = inputs;
	service_inputs_previous = previous;
	service_inputs_pressed = previous & ~inputs;
	unk_20F016 = ~previous & inputs;
}

void count_frames_and_check_test_switch(void)
{
	uint32_t carry = 0;
	uint8_t old_divider = 0;
	uint16_t old_frames = 0;

	old_frames = power_on_frames;
	carry = (uint16_t)(old_frames + 1u) < old_frames;
	power_on_frames = old_frames + 1u;
	if (carry)
		power_on_frames_high++;
	old_divider = power_time_save_divider;
	power_time_save_divider = old_divider - 4u;
	if (old_divider < 4u)
	{
		mem_write32(bookkeeping_power_on_time, AS32(power_on_frames));
		mem_write16(bookkeeping_power_on_time_high, AT16(power_on_frames_high, 2));
		update_bookkeeping_checksum();
		mem_write32(bookkeeping_backup_power_on_time, AS32(power_on_frames));
		mem_write16(bookkeeping_backup_power_on_time_high, AT16(power_on_frames_high, 2));
	}
	game_frames++;
	if ((inputs_pressed & 0x2002u) != 0u && (inputs_pressed & 1u) == 0u)
		test_mode = 0xFFu;
}
