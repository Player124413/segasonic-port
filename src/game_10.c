/**
 *    Name:        game_10.c
 *    Description: the game's functions, part 10 (in the order of their V60 addresses)
 *    Version:     0.1.0
 *    Created:     12-10-2026
 *    Modified:    13-10-2026
 *    Author:      Sebastian Toyos
 *
 *    SegaSonic The Hedgehog (arcade, Sega System 32): the game's V60 program decompiled into C by
 *    tools/decompile.py, with the names of symbols.txt.
 */
#include "game.h"

void ring_tally_ticker_update(void)
{
	uint8_t sound_id = 0;
	uint32_t ignored = 0;

	if (OBJECT(current_object)->ring_tally_ticker.started == 0u)
	{
		OBJECT(current_object)->ring_tally_ticker.started = 1u;
		OBJECT(current_object)->ring_tally_ticker.timer = 0;
	}
	if (ring_tally_counted != 0u)
	{
		delete_current_object();
		return;
	}
	OBJECT(current_object)->ring_tally_ticker.timer++;
	if (OBJECT(current_object)->ring_tally_ticker.timer >= 6)
	{
		OBJECT(current_object)->ring_tally_ticker.timer = 0;
		sound_id = 0x83u;
		queue_sound(sound_id, &ignored, &ignored);
	}
}

void ring_tally_player_1_update(void)
{
	uint32_t character = 0;

	if (OBJECT(current_object)->ring_tally_player.started != 0u)
	{
		update_ring_tally_panel();
		return;
	}
	character = OBJECT(current_object)->ring_tally_player.character;
	build_ring_tally_panel(character);
	OBJECT(current_object)->ring_tally_player.step_delay = 4u;
	OBJECT(object_cursor)->ring_tally_player.palette_delay = 2u;
	OBJECT(current_object)->ring_tally_player.step = 0u;
	OBJECT(current_object)->ring_tally_player.rings_counted = 0;
	OBJECT(current_object)->ring_tally_player.started = 1u;
}

void ring_tally_player_2_update(void)
{
	uint32_t character = 0;

	if (OBJECT(current_object)->ring_tally_player.started != 0u)
	{
		update_ring_tally_panel();
		return;
	}
	if ((cabinet_config & 1u) == 0u)
	{
		ring_tally_player_3_update();
		return;
	}
	character = OBJECT(current_object)->ring_tally_player.character;
	build_ring_tally_panel(character);
	OBJECT(object_cursor)->screen_x = 0xA0;
	mem_write16(OBJECT(current_object)->speed_y + 0xEu, 0xA0u);
	mem_write16(OBJECT(current_object)->z + 0xEu, 0xA0u);
	mem_write16(OBJECT(current_object)->ring_tally_player.part_3 + 0xEu, 0xA0u);
	mem_write16(OBJECT(current_object)->animation + 0xEu, 0xB6u);
	mem_write16(OBJECT(current_object)->x + 0xEu, 0xADu);
	mem_write16(OBJECT(current_object)->y + 0xEu, 0xA4u);
	OBJECT(current_object)->ring_tally_player.step_delay = 4u;
	OBJECT(object_cursor)->ring_tally_player.palette_delay = 2u;
	OBJECT(current_object)->ring_tally_player.step = 0u;
	OBJECT(current_object)->ring_tally_player.rings_counted = 0;
	OBJECT(current_object)->ring_tally_player.started = 1u;
}

void ring_tally_player_3_update(void)
{
	uint32_t character = 0;

	if (OBJECT(current_object)->ring_tally_player.started != 0u)
	{
		update_ring_tally_panel();
		return;
	}
	character = OBJECT(current_object)->ring_tally_player.character;
	build_ring_tally_panel(character);
	OBJECT(object_cursor)->screen_x = 0x100;
	mem_write16(OBJECT(current_object)->speed_y + 0xEu, 0x100u);
	mem_write16(OBJECT(current_object)->z + 0xEu, 0x100u);
	mem_write16(OBJECT(current_object)->ring_tally_player.part_3 + 0xEu, 0x100u);
	mem_write16(OBJECT(current_object)->animation + 0xEu, 0x116u);
	mem_write16(OBJECT(current_object)->x + 0xEu, 0x10Du);
	mem_write16(OBJECT(current_object)->y + 0xEu, 0x104u);
	OBJECT(current_object)->ring_tally_player.step_delay = 4u;
	OBJECT(object_cursor)->ring_tally_player.palette_delay = 2u;
	OBJECT(current_object)->ring_tally_player.step = 0u;
	OBJECT(current_object)->ring_tally_player.rings_counted = 0;
	OBJECT(current_object)->ring_tally_player.started = 1u;
}

void ring_tally_player_3_update_spare_return(void)
{
}

void build_ring_tally_panel(uint32_t character)
{
	uint32_t ignored = 0, object = 0, portrait_record = 0;

	portrait_record = ring_tally_portraits[character];
	OBJECT(current_object)->sprite_palette = mem_read16(portrait_record);
	OBJECT(current_object)->sprite_palette |= 0x8000u;
	portrait_record += 2u;
	OBJECT(current_object)->sprite_frame = mem_read16(portrait_record);
	OBJECT(current_object)->sprite_flags = 0u;
	OBJECT(current_object)->sprite_scale = 0x100u;
	OBJECT(current_object)->screen_y = 0xA0;
	OBJECT(current_object)->screen_x = 0x40;
	OBJECT(current_object)->sprite_layer = 0u;
	OBJECT(current_object)->floor_block = portrait_record + 2u;
	allocate_object_2(&ignored, &ignored, &object);
	OBJECT(current_object)->speed_y = object;
	OBJECT(object)->sprite_frame = 0x9EF0u;
	if (OBJECT(current_object)->build_ring_tally_panel.gets_bonus == 0u)
		OBJECT(object)->sprite_frame = 0x9EF8u;
	OBJECT(object)->sprite_palette = 0xC40u;
	OBJECT(object)->sprite_palette |= 0x8000u;
	OBJECT(object)->screen_x = 0x40;
	OBJECT(object)->screen_y = 0x59;
	OBJECT(object)->sprite_layer = 0u;
	OBJECT(object)->sprite_flags = 0u;
	OBJECT(object)->sprite_scale = 0x100u;
	allocate_object_2(&ignored, &ignored, &object);
	OBJECT(current_object)->z = object;
	OBJECT(object)->screen_x = 0x40;
	OBJECT(object)->screen_y = 0xA0;
	OBJECT(object)->sprite_layer = 1u;
	OBJECT(object)->sprite_frame = 0x9DD8u;
	OBJECT(object)->sprite_palette = 0xC50u;
	OBJECT(object)->sprite_palette |= 0x8000u;
	OBJECT(object)->sprite_flags = 0u;
	OBJECT(object)->sprite_scale = 0x100u;
	allocate_object_2(&ignored, &ignored, &object);
	OBJECT(current_object)->build_ring_tally_panel.part_3 = object;
	OBJECT(object)->screen_x = 0x40;
	OBJECT(object)->screen_y = 0x70;
	OBJECT(object)->sprite_layer = 0u;
	OBJECT(object)->sprite_frame = 0x9E90u;
	OBJECT(object)->sprite_palette = 0xC20u;
	OBJECT(object)->sprite_palette |= 0x8000u;
	OBJECT(object)->sprite_flags = 0u;
	OBJECT(object)->sprite_scale = 0x100u;
	allocate_object_2(&ignored, &ignored, &object);
	OBJECT(current_object)->animation = object;
	OBJECT(object)->screen_x = 0x56;
	OBJECT(object)->screen_y = 0x75;
	OBJECT(object)->sprite_layer = 0u;
	OBJECT(object)->sprite_frame = 0x9E40u;
	OBJECT(object)->sprite_palette = 0xC10u;
	OBJECT(object)->sprite_palette |= 0x8000u;
	OBJECT(object)->sprite_flags = 0u;
	OBJECT(object)->sprite_scale = 0x100u;
	allocate_object_2(&ignored, &ignored, &object);
	OBJECT(current_object)->x = object;
	OBJECT(object)->screen_x = 0x4D;
	OBJECT(object)->screen_y = 0x75;
	OBJECT(object)->sprite_layer = 0u;
	OBJECT(object)->sprite_frame = 0x9E40u;
	OBJECT(object)->sprite_palette = 0xC10u;
	OBJECT(object)->sprite_palette |= 0x8000u;
	OBJECT(object)->sprite_flags = 0u;
	OBJECT(object)->sprite_scale = 0x100u;
	allocate_object_2(&ignored, &ignored, &object);
	OBJECT(current_object)->y = object;
	OBJECT(object)->screen_x = 0x44;
	OBJECT(object)->screen_y = 0x75;
	OBJECT(object)->sprite_layer = 0u;
	OBJECT(object)->sprite_frame = 0x9E40u;
	OBJECT(object)->sprite_palette = 0xC10u;
	OBJECT(object)->sprite_palette |= 0x8000u;
	OBJECT(object)->sprite_flags = 0u;
	OBJECT(object)->sprite_scale = 0x100u;
}

void update_ring_tally_panel(void)
{
	uint8_t sound_id = 0;
	uint32_t counted_bcd = 0, ignored = 0, speed_x = 0;

	if (stage_results_done != 0u)
	{
		delete_current_object();
		return;
	}
	switch (OBJECT(current_object)->update_ring_tally_panel.step)
	{
	case 0:
	update_ring_tally_panel_count_up:
		queue_sprite_in_layer();
		current_object = OBJECT(object_cursor)->update_ring_tally_panel.part_3;
		queue_sprite_in_layer();
		current_object = OBJECT(object_cursor)->animation;
		queue_sprite_in_layer();
		current_object = OBJECT(object_cursor)->y;
		if (OBJECT(current_object)->sprite_frame == 0x9E40u)
		{
			current_object = OBJECT(object_cursor)->x;
			if (OBJECT(current_object)->sprite_frame != 0x9E40u)
				queue_sprite_in_layer();
		}
		else
		{
			queue_sprite_in_layer();
			current_object = OBJECT(object_cursor)->x;
			queue_sprite_in_layer();
		}
		OBJECT(object_cursor)->update_ring_tally_panel.step_delay--;
		if (OBJECT(object_cursor)->update_ring_tally_panel.step_delay != 0u)
			return;
		if (OBJECT(object_cursor)->update_ring_tally_panel.ring_total <= OBJECT(object_cursor)->update_ring_tally_panel.rings_counted)
		{
			ring_tally_panels_counting--;
			if (ring_tally_panels_counting == 0u)
				ring_tally_counted = 1u;
			OBJECT(object_cursor)->update_ring_tally_panel.step++;
			if (OBJECT(object_cursor)->update_ring_tally_panel.gets_bonus == 0u)
			{
				OBJECT(object_cursor)->update_ring_tally_panel.step++;
				OBJECT(object_cursor)->update_ring_tally_panel.step++;
			}
		}
		counted_bcd = (uint8_t)OBJECT(object_cursor)->update_ring_tally_panel.rings_counted;
		convert_to_bcd(&counted_bcd, &ignored);
		current_object = OBJECT(object_cursor)->animation;
		OBJECT(current_object)->sprite_frame = ring_tally_digits[counted_bcd & 0xFu];
		current_object = OBJECT(object_cursor)->x;
		OBJECT(current_object)->sprite_frame = ring_tally_digits[(counted_bcd >> 4) & 0xFu];
		current_object = OBJECT(object_cursor)->y;
		OBJECT(current_object)->sprite_frame = ring_tally_digits[(counted_bcd >> 8) & 0xFu];
		OBJECT(object_cursor)->update_ring_tally_panel.step_delay = 2u;
		OBJECT(object_cursor)->update_ring_tally_panel.rings_counted++;
		return;
	case 1:
	case 5:
		speed_x = OBJECT(object_cursor)->speed_x;
		give_result_bonus(speed_x);
		sound_id = ring_tally_character_sounds[OBJECT(object_cursor)->update_ring_tally_panel.character];
		queue_sound(sound_id, &ignored, &ignored);
		sound_id = 0x88u;
		queue_sound(sound_id, &ignored, &ignored);
		OBJECT(object_cursor)->update_ring_tally_panel.step++;
		goto update_ring_tally_panel_count_up;
	case 2:
		OBJECT(object_cursor)->sprite_frame = mem_read16(OBJECT(object_cursor)->floor_block + 2u);
		queue_sprite_in_layer();
		current_object = OBJECT(object_cursor)->speed_y;
		queue_sprite_in_layer();
		cycle_ring_tally_label_palette();
		current_object = OBJECT(object_cursor)->z;
		queue_sprite_in_layer();
		current_object = OBJECT(object_cursor)->update_ring_tally_panel.part_3;
		queue_sprite_in_layer();
		current_object = OBJECT(object_cursor)->animation;
		queue_sprite_in_layer();
		current_object = OBJECT(object_cursor)->y;
		if (OBJECT(current_object)->sprite_frame == 0x9E40u)
		{
			current_object = OBJECT(object_cursor)->x;
			if (OBJECT(current_object)->sprite_frame != 0x9E40u)
				queue_sprite_in_layer();
		}
		else
		{
			queue_sprite_in_layer();
			current_object = OBJECT(object_cursor)->x;
			queue_sprite_in_layer();
		}
		update_ring_tally_panel_tail();
		return;
	case 3:
		sound_id = 0x89u;
		queue_sound(sound_id, &ignored, &ignored);
		OBJECT(object_cursor)->update_ring_tally_panel.step++;
		OBJECT(object_cursor)->sprite_frame = mem_read16(OBJECT(object_cursor)->floor_block);
		queue_sprite_in_layer();
		current_object = OBJECT(object_cursor)->speed_y;
		queue_sprite_in_layer();
		current_object = OBJECT(object_cursor)->update_ring_tally_panel.part_3;
		queue_sprite_in_layer();
		current_object = OBJECT(object_cursor)->animation;
		queue_sprite_in_layer();
		current_object = OBJECT(object_cursor)->y;
		if (OBJECT(current_object)->sprite_frame == 0x9E40u)
		{
			current_object = OBJECT(object_cursor)->x;
			if (OBJECT(current_object)->sprite_frame != 0x9E40u)
				queue_sprite_in_layer();
		}
		else
		{
			queue_sprite_in_layer();
			current_object = OBJECT(object_cursor)->x;
			queue_sprite_in_layer();
		}
		return;
	case 4:
		OBJECT(object_cursor)->sprite_frame = mem_read16(OBJECT(object_cursor)->floor_block);
		queue_sprite_in_layer();
		current_object = OBJECT(object_cursor)->speed_y;
		queue_sprite_in_layer();
		current_object = OBJECT(object_cursor)->update_ring_tally_panel.part_3;
		queue_sprite_in_layer();
		current_object = OBJECT(object_cursor)->animation;
		queue_sprite_in_layer();
		current_object = OBJECT(object_cursor)->y;
		if (OBJECT(current_object)->sprite_frame == 0x9E40u)
		{
			current_object = OBJECT(object_cursor)->x;
			if (OBJECT(current_object)->sprite_frame != 0x9E40u)
				queue_sprite_in_layer();
		}
		else
		{
			queue_sprite_in_layer();
			current_object = OBJECT(object_cursor)->x;
			queue_sprite_in_layer();
		}
		return;
	default:
		unknown_code(0x98C43u + OBJECT(current_object)->update_ring_tally_panel.step * 3u);
	}
}

void update_ring_tally_panel_tail(void)
{
}

void cycle_ring_tally_label_palette(void)
{
	OBJECT(object_cursor)->cycle_ring_tally_label_palette.palette_delay--;
	if (OBJECT(object_cursor)->cycle_ring_tally_label_palette.palette_delay != 0u)
		return;
	OBJECT(object_cursor)->cycle_ring_tally_label_palette.palette_delay = 2u;
	OBJECT(object_cursor)->cycle_ring_tally_label_palette.palette_index++;
	if (OBJECT(object_cursor)->cycle_ring_tally_label_palette.palette_index > 6)
		OBJECT(object_cursor)->cycle_ring_tally_label_palette.palette_index = 0;
	current_object = OBJECT(object_cursor)->z;
	OBJECT(current_object)->sprite_palette = ring_tally_label_palettes[(uint8_t)OBJECT(object_cursor)->cycle_ring_tally_label_palette.palette_index];
	OBJECT(current_object)->sprite_palette |= 0x8000u;
}

uint32_t wait_for_frames(void)
{
	uint32_t r8 = 0, ignored = 0;

	frames_to_wait = frames_per_wait;
	wait_for_frame(&ignored, &ignored, &ignored, &r8, &ignored, &ignored);
	return r8;
}

void wait_for_frame(uint32_t *p_r1, uint32_t *p_released, uint32_t *p_input_cursor, uint32_t *p_count, uint32_t *p_trackball_port, uint32_t *p_trackball_cursor)
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
	*p_r1 = r1;
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

void square_root(uint32_t *p_value, uint32_t *p_r1)
{
	uint32_t value = *p_value;
	uint32_t carry = 0;
	uint32_t radicand = 0, root_remainder = 0, steps_left = 0;

	radicand = value;
	steps_left = 0x10u;
	root_remainder = 0u;
	value = 0u;
	for (;;)
	{
		carry = radicand * 2u < radicand;
		radicand = radicand * 2u;
		root_remainder = root_remainder * 2u + carry;
		carry = radicand * 2u < radicand;
		radicand = radicand * 2u;
		root_remainder = root_remainder * 2u + carry;
		value = value * 2u + 1u;
		if (root_remainder < value)
		{
			value--;
			steps_left--;
			if (steps_left == 0u)
			{
				value = value >> 1;
				*p_value = value;
				*p_r1 = steps_left;
				return;
			}
			continue;
		}
		root_remainder -= value;
		value++;
		steps_left--;
		if (steps_left != 0u)
			continue;
		break;
	}
	value = value >> 1;
	*p_value = value;
	*p_r1 = steps_left;
}

void angle_of_vector(uint32_t dy, uint32_t *p_dx, uint32_t *p_octant)
{
	uint32_t dx = *p_dx;
	uint32_t octant = 0, octant_index = 0;
	uint32_t abs_dx = 0;

	octant_index = 0u;
	dx = (uint16_t)dx;
	if ((int16_t)dx < 0)
	{
		dx = (uint16_t)(0u - dx);
		octant_index = (uint8_t)(octant_index + 1u);
	}
	dy = (uint16_t)dy;
	if ((int16_t)dy < 0)
	{
		dy = (uint16_t)(0u - dy);
		octant_index = (uint8_t)(octant_index + 2u);
	}
	if (dy < dx)
		dy = divide_u32(dy << 8, dx);
	else
	{
		abs_dx = dx;
		dx = dy;
		dy = abs_dx;
		octant_index = (uint8_t)(octant_index + 4u);
		if (dx != 0u)
		{
			dy = divide_u32(dy << 8, dx);
			dx = mem_read8(angle_ratios_ADDRESS + dy);
			octant = angle_octants[octant_index];
			if ((int16_t)octant >= 0)
			{
				dx = (uint16_t)(dx + octant);
				dx = dx & 0x3FFu;
				*p_dx = dx;
				*p_octant = octant;
				return;
			}
			octant = (uint16_t)~octant;
			dx = (uint16_t)(0u - dx);
			dx = (uint16_t)(dx + octant);
			dx = dx & 0x3FFu;
			*p_dx = dx;
			*p_octant = octant;
			return;
		}
	}
	dx = mem_read8(angle_ratios_ADDRESS + dy);
	octant = angle_octants[octant_index];
	if ((int16_t)octant >= 0)
	{
		dx = (uint16_t)(dx + octant);
		dx = dx & 0x3FFu;
		*p_dx = dx;
		*p_octant = octant;
		return;
	}
	octant = (uint16_t)~octant;
	dx = (uint16_t)(0u - dx);
	dx = (uint16_t)(dx + octant);
	dx = dx & 0x3FFu;
	*p_dx = dx;
	*p_octant = octant;
}

uint32_t convert_from_bcd(uint32_t value)
{
	uint32_t digits_left = 0, number = 0;

	digits_left = 8u;
	number = 0u;
	do
	{
		value = (value << 4) | (value >> 28);
		number = number * 0xAu + (0xFu & value);
		digits_left--;
	} while (digits_left != 0u);
	value = number;
	return value;
}

void convert_to_bcd(uint32_t *p_value, uint32_t *p_digit)
{
	uint32_t value = *p_value;
	uint32_t bcd = 0, digit = 0, digits_left = 0;

	digits_left = 8u;
	bcd = 0u;
	do
	{
		digit = 0u;
		divux_pair(0xAu, &value, &digit);
		bcd += digit;
		bcd = (bcd << 28) | (bcd >> 4);
		digits_left--;
	} while ((digits_left != 0u) & (value != 0u));
	if (value != 0u)
	{
		digit = (digit & ~0xFFu) | (uint8_t)value;
		digit = (digit & ~0xFFu) | ((uint8_t)digit % 0xAu);
		digit = (digit & ~0xFFu) | (uint8_t)(digit + ((uint32_t)((uint8_t)value / 0xAu) << 4));
		value = bcd;
		*p_value = value;
		*p_digit = digit;
		return;
	}
	if (digits_left == 0u)
	{
		value = bcd;
		*p_value = value;
		*p_digit = digit;
		return;
	}
	do
	{
		bcd = (bcd << 28) | (bcd >> 4);
		digits_left--;
	} while (digits_left != 0u);
	value = bcd;
	*p_value = value;
	*p_digit = digit;
}

void convert_64_to_bcd(uint32_t *p_number_low, uint32_t *p_number_high, uint32_t *p_bcd_high)
{
	uint32_t number_low = *p_number_low;
	uint32_t number_high = *p_number_high;
	uint8_t digit_pair = 0;
	uint32_t bcd_high = 0, bcd_low = 0, bcd_middle = 0, cursor = 0, dividend = 0, pairs_left = 0, remainder = 0, value_high = 0, value_low = 0;

	value_low = number_low;
	value_high = number_high;
	cursor = stack_pointer;
	pairs_left = 0xAu;
	do
	{
		remainder = 0u;
		dividend = value_high;
		divux_pair(0xAu, &dividend, &remainder);
		value_high = dividend;
		dividend = value_low;
		divux_pair(0xAu, &dividend, &remainder);
		value_low = dividend;
		digit_pair = (uint8_t)remainder;
		remainder = 0u;
		dividend = value_high;
		divux_pair(0xAu, &dividend, &remainder);
		value_high = dividend;
		dividend = value_low;
		divux_pair(0xAu, &dividend, &remainder);
		value_low = dividend;
		digit_pair = (uint8_t)(((uint32_t)digit_pair << 4) | (digit_pair >> 4) | remainder);
		cursor--;
		mem_write8(cursor, ((uint32_t)digit_pair << 4) | (digit_pair >> 4));
		pairs_left--;
	} while ((pairs_left != 0u) & (value_high != 0u));
	do
	{
		remainder = 0u;
		divux_pair(0xAu, &dividend, &remainder);
		digit_pair = (uint8_t)remainder;
		remainder = 0u;
		divux_pair(0xAu, &dividend, &remainder);
		digit_pair = (uint8_t)(((uint32_t)digit_pair << 4) | (digit_pair >> 4) | remainder);
		cursor--;
		mem_write8(cursor, ((uint32_t)digit_pair << 4) | (digit_pair >> 4));
		pairs_left--;
	} while ((pairs_left != 0u) & (dividend != 0u));
	pairs_left += 2u;
	do
	{
		cursor--;
		mem_write8(cursor, dividend);
		pairs_left--;
	} while (pairs_left != 0u);
	bcd_high = byte_swap32(mem_read32(cursor));
	cursor += 4u;
	bcd_middle = byte_swap32(mem_read32(cursor));
	cursor += 4u;
	bcd_low = byte_swap32(mem_read32(cursor));
	stack_pointer = cursor + 4u;
	*p_number_low = bcd_low;
	*p_number_high = bcd_middle;
	*p_bcd_high = bcd_high;
}

void reset_objects(uint32_t *p_block, uint32_t *p_stack_slot)
{
	uint32_t r0 = 0, r3 = 0;

	reset_object_list();
	reset_object_pool_2(&r0, &r3);
	run_objects();
	*p_block = r0;
	*p_stack_slot = r3;
}

void reset_objects_alt(void)
{
	reset_object_list();
	reset_object_pool_2_alt();
	run_objects();
}

void run_objects(void)
{
	free_objects = free_object_stack_ADDRESS;
	if (object_count == 0u)
		return;
	first_object = first_object_address;
	last_object = last_object_address;
	object_cursor = last_object;
	do
	{
		object_cursor = LINKED(object_cursor, OBJECT(object_cursor)->next);
		current_object = object_cursor;
		call_code(OBJECT(current_object)->handler);
	} while ((uint16_t)object_cursor != (uint16_t)last_object);
	AS16(first_object_address) = first_object;
	AS16(last_object_address) = last_object;
}

void reset_object_list(void)
{
	uint16_t object_offset = 0;
	uint32_t objects_left = 0, stack_slot = 0;

	object_offset = 0u;
	first_object_address = 0x200000u;
	last_object_address = 0x200000u;
	objects_left = 0x80u;
	free_object_stack = 0x80;
	stack_slot = 0x20E902u;
	do
	{
		mem_write16(stack_slot, object_offset);
		objects_left--;
		stack_slot += 2u;
		object_offset += 0xA0u;
	} while (objects_left != 0u);
	object_count = objects_left;
}

void take_free_object(uint32_t *p_free_count, uint32_t *p_object)
{
	uint32_t object = *p_object;
	uint32_t free_count = 0;

	free_count = mem_read16(free_objects);
	if (free_count == 0u)
	{
		*p_free_count = free_count;
		return;
	}
	object = 0x200000u | mem_read16(free_objects + free_count * 2u);
	mem_write16(free_objects, mem_read16(free_objects) - 1u);
	*p_free_count = free_count;
	*p_object = object;
}

uint32_t allocate_object_or_start_list(uint32_t object)
{
	uint32_t ignored = 0, new_object = 0;

	if (object_count != 0u)
	{
		allocate_object(&ignored, &object);
		return object;
	}
	new_object = 0x200000u | mem_read16(free_objects + mem_read16(free_objects) * 2u);
	mem_write16(free_objects, mem_read16(free_objects) - 1u);
	AS16(first_object_address) = new_object;
	first_object = new_object;
	last_object = new_object;
	link_object(new_object);
	return new_object;
}

void allocate_object(uint32_t *p_free_count, uint32_t *p_object)
{
	uint32_t object = *p_object;
	uint32_t free_count = 0;

	free_count = mem_read16(free_objects);
	if (free_count == 0u)
	{
		*p_free_count = free_count;
		return;
	}
	object = 0x200000u | mem_read16(free_objects + free_count * 2u);
	mem_write16(free_objects, mem_read16(free_objects) - 1u);
	link_object(object);
	*p_free_count = free_count;
	*p_object = object;
}

void link_object(uint32_t object)
{
	mem_write16(object + 0x20u, last_object);
	OBJECT(last_object)->next = object;
	OBJECT(first_object)->previous = object;
	mem_write16(object + 0x22u, first_object);
	AS16(last_object_address) = object;
	last_object = object;
	object_count++;
}

void allocate_object_after_current(void)
{
	uint32_t free_count = 0, new_object = 0, object = 0;

	free_count = mem_read16(free_objects);
	if (free_count == 0u)
		return;
	new_object = 0x200000u | mem_read16(free_objects + free_count * 2u);
	mem_write16(free_objects, mem_read16(free_objects) - 1u);
	object = LINKED(current_object, OBJECT(current_object)->next);
	OBJECT(current_object)->next = new_object;
	OBJECT(object)->previous = new_object;
	mem_write16(new_object + 0x20u, current_object);
	mem_write16(new_object + 0x22u, object);
	object_count++;
	if ((uint16_t)last_object == (uint16_t)current_object)
		last_object = (last_object & ~0xFFFFu) | (uint16_t)new_object;
}

void delete_current_object(void)
{
	uint32_t object = 0, object_2 = 0;

	object_count--;
	object = LINKED(object_cursor, OBJECT(object_cursor)->previous);
	object_2 = LINKED(object_cursor, OBJECT(object_cursor)->next);
	OBJECT(object)->next = object_2;
	OBJECT(object_2)->previous = object;
	if ((uint16_t)object_cursor == (uint16_t)first_object)
		first_object = (first_object & ~0xFFFFu) | (uint16_t)object_2;
	if ((uint16_t)object_cursor == (uint16_t)last_object)
		last_object = (last_object & ~0xFFFFu) | (uint16_t)object;
	current_object = object_cursor;
	object_cursor = (object_cursor & ~0xFFFFu) | (uint16_t)object;
	push_free_object();
}

void push_free_object(void)
{
	mem_write16(free_objects, mem_read16(free_objects) + 1u);
	mem_write16(free_objects + mem_read16(free_objects) * 2u, current_object);
}

void delete_next_object(void)
{
	uint32_t ignored = 0, object = 0;

	object = LINKED(object_cursor, OBJECT(current_object)->next);
	delete_object(object, &ignored, &ignored);
}

void delete_object(uint32_t object, uint32_t *p_free_count, uint32_t *p_previous)
{
	uint32_t free_count = 0, object_2 = 0, object_3 = 0;

	object_count--;
	object_2 = LINKED(object, OBJECT(object)->previous);
	object_3 = LINKED(object, OBJECT(object)->next);
	OBJECT(object_3)->previous = object_2;
	OBJECT(object_2)->next = object_3;
	if ((uint16_t)object == (uint16_t)first_object)
		first_object = (first_object & ~0xFFFFu) | (uint16_t)object_3;
	if ((uint16_t)object != (uint16_t)last_object)
	{
		mem_write16(free_objects, mem_read16(free_objects) + 1u);
		free_count = mem_read16(free_objects);
		mem_write16(free_objects + free_count * 2u, object);
		*p_free_count = free_count;
		*p_previous = object_2;
		return;
	}
	last_object = (last_object & ~0xFFFFu) | (uint16_t)object_2;
	mem_write16(free_objects, mem_read16(free_objects) + 1u);
	free_count = mem_read16(free_objects);
	mem_write16(free_objects + free_count * 2u, object);
	*p_free_count = free_count;
	*p_previous = object_2;
}

void reset_object_pool_2(uint32_t *p_block, uint32_t *p_stack_slot)
{
	uint32_t block = 0, pool_blocks_left = 0, stack_slot = 0;

	block = 0x205000u;
	pool_blocks_left = 0xFFu;
	free_object_stack_2[0] = 0xFF;
	stack_slot = 0x20EA12u;
	do
	{
		mem_write16(stack_slot, block);
		pool_blocks_left--;
		stack_slot += 2u;
		block += 0x30u;
	} while (pool_blocks_left != 0u);
	*p_block = block;
	*p_stack_slot = stack_slot;
}

void reset_object_pool_2_alt(void)
{
	uint16_t block = 0;
	uint32_t pool_blocks_left = 0, stack_slot = 0;

	block = 0x5000u;
	pool_blocks_left = 0x1D8u;
	free_object_stack_2[0] = 0x1D8;
	stack_slot = 0x20EA12u;
	do
	{
		mem_write16(stack_slot, block);
		pool_blocks_left--;
		stack_slot += 2u;
		block += 0x34u;
	} while (pool_blocks_left != 0u);
}

void allocate_object_2(uint32_t *p_free_count, uint32_t *p_free_stack, uint32_t *p_object)
{
	uint32_t object = *p_object;
	uint32_t free_count = 0, free_stack = 0;

	free_stack = free_object_stack_2_ADDRESS;
	free_count = (uint16_t)free_object_stack_2[0];
	if (free_count == 0u)
	{
		*p_free_count = free_count;
		*p_free_stack = free_stack;
		return;
	}
	object = 0x200000u | (uint16_t)free_object_stack_2[free_count];
	free_object_stack_2[0]--;
	*p_free_count = free_count;
	*p_free_stack = free_stack;
	*p_object = object;
}

uint32_t free_object_2(uint16_t object)
{
	uint32_t free_stack = 0;

	free_stack = free_object_stack_2_ADDRESS;
	free_object_stack_2[0]++;
	free_object_stack_2[(uint16_t)free_object_stack_2[0]] = object;
	return free_stack;
}

void load_text_font(uint32_t *p_pixels, uint32_t *p_color_index)
{
	uint8_t color = 0;
	uint16_t count = 0;
	uint32_t r4 = 0, r5 = 0, dest = 0, dest_end = 0, ignored = 0, source = 0, source_end = 0;

	mem_write16(video_text_control, 0x1F7u);
	move_string(text_font_ADDRESS, 0x9A0u, VIDEO_RAM + 0x1C000u, 0x9A0u, 2u, 0u, 0u, fill_value, &dest_end, &source_end);
	source = source_end;
	dest = dest_end;
	count = 0x5Bu;
	color = 1u;
	expand_font(color, count, source, dest, &r4, &r5);
	clear_text_layer(&ignored, &ignored, &ignored);
	*p_pixels = r4;
	*p_color_index = r5;
}

void clear_text_layer(uint32_t *p_value, uint32_t *p_count, uint32_t *p_dest)
{
	uint32_t r0 = 0, count = 0, dest = 0;

	dest = TEXT_CELL(0, 0);
	count = 0xE0u;
	clear_blocks(&r0, &count, &dest);
	*p_value = r0;
	*p_count = count;
	*p_dest = dest;
}

void expand_font(uint8_t color, uint32_t count, uint32_t source, uint32_t dest, uint32_t *p_pixels, uint32_t *p_color_index)
{
	uint32_t carry = 0;
	uint8_t font_row = 0;
	uint32_t char_rows_left = 0, color_index = 0, pairs_left = 0, pixel_mask = 0, pixels = 0, set_pixels = 0, shadow_pixels = 0;

	color_index = (uint8_t)color & 0xFu;
	count = (uint16_t)count;
	do
	{
		char_rows_left = 8u;
		shadow_pixels = 0u;
		do
		{
			font_row = mem_read8(source);
			pairs_left = 4u;
			pixels = 0u;
			source++;
			do
			{
				carry = (uint8_t)(font_row * 2u) < font_row;
				font_row = (uint8_t)(font_row * 2u);
				pixels = rotate_with_carry_by(pixels, 4u, carry, 32);
				carry = (uint8_t)(font_row * 2u) < font_row;
				font_row = (uint8_t)(font_row * 2u);
				pixels = rotate_with_carry_by(pixels, 4u, carry, 32);
				pairs_left--;
			} while (pairs_left != 0u);
			pixels = (pixels << 29) | (pixels >> 3);
			set_pixels = pixels;
			pixel_mask = pixels * 0xFu;
			pixels = pixels * color_index;
			mem_write32(dest, byte_swap32((shadow_pixels & ~pixel_mask) | pixels));
			shadow_pixels = (pixels + set_pixels) >> 4;
			char_rows_left--;
			dest += 4u;
		} while (char_rows_left != 0u);
		count--;
	} while (count != 0u);
	*p_pixels = pixels;
	*p_color_index = color_index;
}

void expand_font_2bpp(uint8_t color, uint32_t count, uint32_t source, uint32_t dest)
{
	uint32_t carry = 0;
	uint16_t font_row = 0;
	uint32_t color_nibbles = 0, pairs_left = 0, pixels = 0;

	color_nibbles = ((uint8_t)color & 0xFu) * 0x11111111u;
	count = (uint32_t)(uint16_t)count << 3;
	do
	{
		font_row = mem_read16(source);
		pairs_left = 4u;
		pixels = 0u;
		source += 2u;
		do
		{
			carry = (uint16_t)(font_row * 2u) < font_row;
			font_row = (uint16_t)(font_row * 2u);
			pixels = pixels * 2u + carry;
			carry = (uint16_t)(font_row * 2u) < font_row;
			font_row = (uint16_t)(font_row * 2u);
			pixels = rotate_with_carry_by(pixels, 3u, carry, 32);
			carry = (uint16_t)(font_row * 2u) < font_row;
			font_row = (uint16_t)(font_row * 2u);
			pixels = pixels * 2u + carry;
			carry = (uint16_t)(font_row * 2u) < font_row;
			font_row = (uint16_t)(font_row * 2u);
			pixels = rotate_with_carry_by(pixels, 3u, carry, 32);
			pairs_left--;
		} while (pairs_left != 0u);
		mem_write32(dest, byte_swap32(((pixels << 30) | (pixels >> 2)) + color_nibbles));
		count--;
		dest += 4u;
	} while (count != 0u);
}

void expand_font_2bpp_transparent(uint8_t color, uint32_t count, uint32_t pixels, uint32_t opaque_mask, uint32_t source, uint32_t dest)
{
	uint8_t pixel = 0;
	uint16_t font_row = 0;
	uint32_t color_nibbles = 0, pairs_left = 0;

	color_nibbles = ((uint8_t)color & 0xFu) * 0x11111111u;
	count = (uint32_t)(uint16_t)count << 3;
	do
	{
		font_row = mem_read16(source);
		pairs_left = 4u;
		source += 2u;
		do
		{
			font_row = (uint16_t)(((uint32_t)font_row << 2) | (font_row >> 14));
			pixel = font_row & 3u;
			pixels = pixels << 4;
			pixels = (pixels & ~0xFFu) | (uint8_t)(pixels | pixel);
			opaque_mask = opaque_mask << 4;
			opaque_mask = (opaque_mask & ~0xFFu) | (uint8_t)(opaque_mask | (pixel != 0u));
			font_row = (uint16_t)(((uint32_t)font_row << 2) | (font_row >> 14));
			pixel = font_row & 3u;
			pixels = pixels << 4;
			pixels = (pixels & ~0xFFu) | (uint8_t)(pixels | pixel);
			opaque_mask = opaque_mask << 4;
			opaque_mask = (opaque_mask & ~0xFFu) | (uint8_t)(opaque_mask | (pixel != 0u));
			pairs_left--;
		} while (pairs_left != 0u);
		opaque_mask = opaque_mask * 0xFu;
		pixels = (pixels + color_nibbles) & opaque_mask;
		mem_write32(dest, byte_swap32(pixels));
		count--;
		dest += 4u;
	} while (count != 0u);
}

void init_video_registers(void)
{
	uint8_t mode = 0;
	uint16_t value = 0;
	uint32_t count = 0, defaults_cursor = 0, ignored = 0, rects_left = 0, register_cursor = 0, right_bottom = 0;

	vblank_start_hook = apply_video_registers_ADDRESS;
	defaults_cursor = video_register_defaults_ADDRESS;
	register_cursor = video_control;
	count = 1u;
	copy_blocks(count, &defaults_cursor, &register_cursor);
	count = 4u;
	clear_blocks(&ignored, &count, &register_cursor);
	value = 0x200u;
	count = 6u;
	register_cursor = fill_halfwords(value, count, register_cursor);
	mem_write32(register_cursor, mem_read32(defaults_cursor));
	register_cursor += 4u;
	right_bottom = 0xDF013Fu;
	rects_left = 5u;
	defaults_cursor += 4u;
	do
	{
		mem_write32(register_cursor, 0u);
		register_cursor += 4u;
		mem_write32(register_cursor, right_bottom);
		rects_left--;
		register_cursor += 4u;
	} while (rects_left != 0u);
	mem_write32(register_cursor, mem_read32(defaults_cursor));
	mem_write32(register_cursor + 4u, mem_read32(defaults_cursor + 4u));
	mode = 0u;
	set_screen_mode(mode);
	ignored = clear_page_0_and_scroll();
}

uint32_t clear_page_0_and_scroll(void)
{
	uint32_t count = 0, dest = 0, ignored = 0;

	dest = 0x300000u;
	count = 0x40u;
	clear_blocks(&ignored, &count, &dest);
	dest = nbg0_scroll_x_ADDRESS;
	count = 8u;
	clear_blocks(&ignored, &count, &dest);
	return dest;
}

void init_video_registers_layers_off(void)
{
	init_video_registers();
	mem_write16(video_layer_disable, mem_read16(video_layer_disable) | 0xFu);
	vblank_start_hook = empty_vblank_hook_ADDRESS;
	empty_vblank_hook();
}

void empty_vblank_hook(void)
{
}

void apply_video_registers(void)
{
	move_halfwords(VIDEO_RAM + 0x1FF10u, video_registers_latched_ADDRESS, 0x20u);
	mem_write8(video_layer_opaque_flags, video_layer_opaque_latched);
	move_halfwords(video_registers_latched_ADDRESS, nbg0_scroll_x_ADDRESS, 0x21u);
}

uint32_t set_screen_mode(uint16_t mode)
{
	uint16_t control_bits = 0;
	uint32_t control_register = 0;

	mode = (uint8_t)(mode ^ (setting_monitor_flip & 2u));
	screen_mode = (uint8_t)mode;
	control_register = video_control;
	control_bits = (uint16_t)(mode & 1u);
	mem_write8(sprite_wide_screen, (uint8_t)control_bits);
	mem_write16(video_control, mem_read16(video_control) & 0x7DFFu);
	mode &= 2u;
	mode = (uint16_t)(((uint32_t)mode << 8) | (mode >> 8));
	control_bits = (uint16_t)(((uint32_t)control_bits << 15) | (control_bits >> 1) | mode);
	mode = (uint16_t)(mode * 3u);
	mem_write8(sprite_flip, ((uint32_t)mode << 7) | (mode >> 9));
	mem_write16(video_control, mem_read16(video_control) | control_bits);
	return control_register;
}

void copy_blocks(uint32_t count, uint32_t *p_source, uint32_t *p_dest)
{
	uint32_t source = *p_source;
	uint32_t dest = *p_dest;

	do
	{
		mem_write32(dest, mem_read32(source));
		dest += 4u;
		source += 4u;
		mem_write32(dest, mem_read32(source));
		dest += 4u;
		source += 4u;
		mem_write32(dest, mem_read32(source));
		dest += 4u;
		source += 4u;
		mem_write32(dest, mem_read32(source));
		dest += 4u;
		count--;
		source += 4u;
	} while (count != 0u);
	*p_source = source;
	*p_dest = dest;
}

void clear_blocks(uint32_t *p_value, uint32_t *p_count, uint32_t *p_dest)
{
	uint32_t count = *p_count;
	uint32_t dest = *p_dest;
	uint32_t value = 0;

	value = 0u;
	do
	{
		mem_write32(dest, value);
		dest += 4u;
		mem_write32(dest, value);
		dest += 4u;
		mem_write32(dest, value);
		dest += 4u;
		mem_write32(dest, value);
		count--;
		dest += 4u;
	} while (count != 0u);
	*p_value = value;
	*p_count = count;
	*p_dest = dest;
}

uint32_t fill_halfwords(uint16_t value, uint32_t count, uint32_t dest)
{
	do
	{
		mem_write16(dest, value);
		count--;
		dest += 2u;
	} while (count != 0u);
	return dest;
}

void draw_fraction(uint16_t attribute, uint32_t *p_numerator, uint32_t *p_denominator, uint32_t *p_text_position)
{
	uint16_t numerator = (uint16_t)*p_numerator;
	uint16_t denominator = (uint16_t)*p_denominator;
	uint32_t text_position = *p_text_position;

	attribute = (uint16_t)((uint32_t)attribute << 8);
	numerator &= 0xFu;
	denominator &= 0xFu;
	numerator = (uint8_t)(numerator - 0x80u) | attribute;
	denominator = (uint8_t)(denominator - 0x70u) | attribute;
	mem_write16(text_position, numerator);
	text_position += 2u;
	mem_write16(text_position, denominator);
	*p_numerator = (*p_numerator & 0xFFFF0000u) | numerator;
	*p_denominator = (*p_denominator & 0xFFFF0000u) | denominator;
	*p_text_position = text_position;
}

void print_text_list(uint32_t *p_text, uint32_t *p_text_position)
{
	uint32_t text = *p_text;
	uint32_t text_position = *p_text_position;

	while (mem_read16(text) != 0u)
	{
		print_text(&text, &text_position);
	}
	*p_text = text;
	*p_text_position = text_position;
}

void print_text(uint32_t *p_text, uint32_t *p_text_position)
{
	uint32_t text = *p_text;
	uint32_t r0 = 0, text_position = 0;

	text_position = 0x310000u | mem_read16(text);
	text += 2u;
	print_text_here(&r0, &text, &text_position);
	*p_text = text;
	*p_text_position = text_position;
}

void print_text_here(uint32_t *p_cell, uint32_t *p_text, uint32_t *p_text_position)
{
	uint32_t text = *p_text;
	uint32_t text_position = *p_text_position;
	uint16_t cell = 0;
	uint8_t next_char = 0;

	cell = (uint16_t)((uint32_t)mem_read8(text) << 8);
	text++;
	cell |= mem_read8(text);
	text++;
	if ((uint8_t)cell == 0u)
	{
		*p_cell = (*p_cell & 0xFFFF0000u) | cell;
		*p_text = text;
		return;
	}
	do
	{
		mem_write16(text_position, cell);
		next_char = mem_read8(text);
		text++;
		cell = (uint16_t)((cell & ~0xFFu) | next_char);
		text_position += 2u;
	} while ((uint8_t)cell != 0u);
	*p_cell = (*p_cell & 0xFFFF0000u) | cell;
	*p_text = text;
	*p_text_position = text_position;
}

void erase_text_list(uint32_t text)
{
	while (mem_read16(text) != 0u)
	{
		text = erase_text(text);
	}
}

uint32_t erase_text(uint32_t text)
{
	uint16_t cell = 0;
	uint32_t text_position = 0;

	cell = 0u;
	text_position = 0x310000u | mem_read16(text);
	text += 2u;
	erase_text_here(cell, &text, &text_position);
	return text;
}

void erase_text_here(uint16_t cell, uint32_t *p_text, uint32_t *p_text_position)
{
	uint32_t text = *p_text;
	uint32_t text_position = *p_text_position;
	uint8_t first_char = 0, next_char = 0;

	text++;
	first_char = mem_read8(text);
	text++;
	if (first_char == 0u)
	{
		*p_text = text;
		return;
	}
	do
	{
		mem_write16(text_position, cell);
		next_char = mem_read8(text);
		text++;
		text_position += 2u;
	} while (next_char != 0u);
	*p_text = text;
	*p_text_position = text_position;
}

void print_text_lines(uint32_t *p_text, uint32_t *p_text_position)
{
	uint32_t text = *p_text;
	uint32_t text_position = *p_text_position;
	uint16_t text_cell = 0;
	uint32_t line_start = 0;

	line_start = text_position;
	text_cell = (uint16_t)((uint32_t)mem_read8(text) << 8);
	text++;
	for (;;)
	{
		text_cell = (uint16_t)((text_cell & ~0xFFu) | mem_read8(text));
		text++;
		if ((uint8_t)text_cell == 0u)
			break;
		if ((int8_t)text_cell >= 0)
		{
			mem_write16(text_position, text_cell);
			text_position += 2u;
			continue;
		}
		line_start += 0x80u;
		text_position = line_start;
	}
	*p_text = text;
	*p_text_position = text_position;
}

void erase_text_lines(uint32_t *p_text, uint32_t *p_text_position)
{
	uint32_t text = *p_text;
	uint32_t text_position = *p_text_position;
	uint32_t negative = 0;
	uint16_t blank_cell = 0;
	uint32_t line_start = 0;
	uint8_t text_char = 0;

	line_start = text_position;
	blank_cell = 0u;
	text++;
	for (;;)
	{
		text_char = mem_read8(text);
		negative = text_char >> 7;
		text++;
		if (text_char == 0u)
			break;
		if (negative == 0u)
		{
			mem_write16(text_position, blank_cell);
			text_position += 2u;
			continue;
		}
		line_start += 0x80u;
		text_position = line_start;
	}
	*p_text = text;
	*p_text_position = text_position;
}

void print_formatted_text(uint32_t text)
{
	uint32_t text_position = 0;

	text_position = TEXT_CELL(0, 0);
	formatted_text_position_single(text, text_position);
}

void formatted_text_position_single(uint32_t text, uint32_t text_position)
{
	text_position = (text_position & ~0xFFFFu) | mem_read16(text);
	text += 2u;
	formatted_text_attribute_single(text, text_position);
}

void formatted_text_attribute_single(uint32_t text, uint32_t text_position)
{
	uint16_t cell = 0;

	cell = (uint16_t)((uint32_t)mem_read8(text) << 8);
	text++;
	formatted_text_single(cell, text, text_position);
}

void formatted_text_single(uint16_t cell, uint32_t text, uint32_t text_position)
{
	uint32_t zero = 0;
	uint32_t code = 0;
	uint8_t next_char = 0;

	for (;;)
	{
		cell = (uint16_t)((cell & ~0xFFu) | mem_read8(text));
		zero = (uint8_t)cell == 0u;
		text++;
		if ((uint8_t)cell != 0u)
		{
			do
			{
				mem_write16(text_position, cell);
				next_char = mem_read8(text);
				text++;
				cell = (uint16_t)((cell & ~0xFFu) | next_char);
				zero = (uint8_t)cell == 0u;
				text_position += 2u;
			} while ((uint8_t)cell != 0u);
		}
		code = mem_read8(text);
		text++;
		switch (code)
		{
		case 0x0u:
			break;
		case 0x3u:
			continue;
		case 0x6u:
			formatted_text_double(cell, text, text_position);
			return;
		case 0x9u:
			formatted_text_triple(cell, text, text_position);
			return;
		case 0xCu:
			formatted_text_attribute_single(text, text_position);
			return;
		case 0xFu:
			formatted_text_position_single(text, text_position);
			return;
		case 0x12u:
			formatted_text_attribute_single(text, text_position);
			return;
		case 0x15u:
			formatted_text_position_double(text, text_position);
			return;
		case 0x18u:
			formatted_text_attribute_double(text, text_position);
			return;
		case 0x1Bu:
			formatted_text_position_triple(text, text_position);
			return;
		case 0x1Eu:
			formatted_text_attribute_triple(text, text_position);
			return;
		case 0x21u:
			formatted_text_skip_single(cell, text, text_position);
			return;
		case 0x24u:
			formatted_text_skip_long_single(cell, text, text_position);
			return;
		default:
			unknown_code(0x996A7u + code);
		}
		if (zero)
			return;
	}
	formatted_text_skip_long_single(cell, text, text_position);
}

void formatted_text_skip_single(uint16_t cell, uint32_t text, uint32_t text_position)
{
	text_position += (int8_t)mem_read8(text) * 2u;
	text++;
	formatted_text_single(cell, text, text_position);
}

void formatted_text_skip_long_single(uint16_t cell, uint32_t text, uint32_t text_position)
{
	text_position += (int16_t)mem_read16(text) * 2u;
	text += 2u;
	formatted_text_single(cell, text, text_position);
}

void formatted_text_position_double(uint32_t text, uint32_t text_position)
{
	text_position = (text_position & ~0xFFFFu) | mem_read16(text);
	text += 2u;
	formatted_text_attribute_double(text, text_position);
}

void formatted_text_attribute_double(uint32_t text, uint32_t text_position)
{
	uint16_t cell = 0;

	cell = (uint16_t)((uint32_t)mem_read8(text) << 8);
	text++;
	formatted_text_double(cell, text, text_position);
}

void formatted_text_double(uint16_t cell, uint32_t text, uint32_t text_position)
{
	uint32_t zero = 0;
	uint32_t code = 0;
	uint8_t next_char = 0;

	for (;;)
	{
		cell = (uint16_t)((cell & ~0xFFu) | mem_read8(text));
		zero = (uint8_t)cell == 0u;
		text++;
		if ((uint8_t)cell != 0u)
		{
			do
			{
				mem_write16(text_position, cell);
				cell++;
				text_position += 2u;
				mem_write16(text_position, cell);
				next_char = mem_read8(text);
				text++;
				cell = (uint16_t)((cell & ~0xFFu) | next_char);
				zero = (uint8_t)cell == 0u;
				text_position += 2u;
			} while ((uint8_t)cell != 0u);
		}
		code = mem_read8(text);
		text++;
		switch (code)
		{
		case 0x0u:
			if (zero)
				return;
			formatted_text_single(cell, text, text_position);
			return;
		case 0x3u:
			formatted_text_single(cell, text, text_position);
			return;
		case 0x6u:
			continue;
		case 0x9u:
			formatted_text_triple(cell, text, text_position);
			return;
		case 0xCu:
			formatted_text_attribute_double(text, text_position);
			return;
		case 0xFu:
			formatted_text_position_single(text, text_position);
			return;
		case 0x12u:
			formatted_text_attribute_single(text, text_position);
			return;
		case 0x15u:
			formatted_text_position_double(text, text_position);
			return;
		case 0x18u:
			formatted_text_attribute_double(text, text_position);
			return;
		case 0x1Bu:
			formatted_text_position_triple(text, text_position);
			return;
		case 0x1Eu:
			formatted_text_attribute_triple(text, text_position);
			return;
		case 0x21u:
			formatted_text_skip_double(cell, text, text_position);
			return;
		case 0x24u:
			formatted_text_skip_long_double(cell, text, text_position);
			return;
		default:
			unknown_code(0x99703u + code);
		}
	}
	if (zero)
		return;
	formatted_text_single(cell, text, text_position);
}

void formatted_text_skip_double(uint16_t cell, uint32_t text, uint32_t text_position)
{
	text_position += (int8_t)mem_read8(text) * 2u;
	text++;
	formatted_text_double(cell, text, text_position);
}

void formatted_text_skip_long_double(uint16_t cell, uint32_t text, uint32_t text_position)
{
	text_position += (int16_t)mem_read16(text) * 2u;
	text += 2u;
	formatted_text_double(cell, text, text_position);
}

void formatted_text_position_triple(uint32_t text, uint32_t text_position)
{
	text_position = (text_position & ~0xFFFFu) | mem_read16(text);
	text += 2u;
	formatted_text_attribute_triple(text, text_position);
}

void formatted_text_attribute_triple(uint32_t text, uint32_t text_position)
{
	uint16_t cell = 0;

	cell = (uint16_t)((uint32_t)mem_read8(text) << 8);
	text++;
	formatted_text_triple(cell, text, text_position);
}

void formatted_text_triple(uint16_t cell, uint32_t text, uint32_t text_position)
{
	uint32_t zero = 0;
	uint32_t code = 0;
	uint8_t next_char = 0;

	for (;;)
	{
		cell = (uint16_t)((cell & ~0xFFu) | mem_read8(text));
		zero = (uint8_t)cell == 0u;
		text++;
		if ((uint8_t)cell != 0u)
		{
			do
			{
				mem_write16(text_position, cell);
				cell++;
				text_position += 2u;
				mem_write16(text_position, cell);
				cell++;
				text_position += 2u;
				mem_write16(text_position, cell);
				next_char = mem_read8(text);
				text++;
				cell = (uint16_t)((cell & ~0xFFu) | next_char);
				zero = (uint8_t)cell == 0u;
				text_position += 2u;
			} while ((uint8_t)cell != 0u);
		}
		code = mem_read8(text);
		text++;
		switch (code)
		{
		case 0x0u:
			if (zero)
				return;
			formatted_text_single(cell, text, text_position);
			return;
		case 0x3u:
			formatted_text_single(cell, text, text_position);
			return;
		case 0x6u:
			formatted_text_double(cell, text, text_position);
			return;
		case 0x9u:
			continue;
		case 0xCu:
			formatted_text_attribute_triple(text, text_position);
			return;
		case 0xFu:
			formatted_text_position_single(text, text_position);
			return;
		case 0x12u:
			formatted_text_attribute_single(text, text_position);
			return;
		case 0x15u:
			formatted_text_position_double(text, text_position);
			return;
		case 0x18u:
			formatted_text_attribute_double(text, text_position);
			return;
		case 0x1Bu:
			formatted_text_position_triple(text, text_position);
			return;
		case 0x1Eu:
			formatted_text_attribute_triple(text, text_position);
			return;
		case 0x21u:
			formatted_text_skip_triple(cell, text, text_position);
			return;
		case 0x24u:
			formatted_text_skip_long_triple(cell, text, text_position);
			return;
		default:
			unknown_code(0x99764u + code);
		}
	}
	if (zero)
		return;
	formatted_text_single(cell, text, text_position);
}

void formatted_text_skip_triple(uint16_t cell, uint32_t text, uint32_t text_position)
{
	text_position += (int8_t)mem_read8(text) * 2u;
	text++;
	formatted_text_triple(cell, text, text_position);
}

void formatted_text_skip_long_triple(uint16_t cell, uint32_t text, uint32_t text_position)
{
	text_position += (int16_t)mem_read16(text) * 2u;
	text += 2u;
	formatted_text_triple(cell, text, text_position);
}

void erase_formatted_text(uint32_t cell, uint32_t text)
{
	uint32_t text_position = 0;

	text_position = TEXT_CELL(0, 0);
	cell &= ~0xFFFFu;
	erase_formatted_text_position_single(cell, text, text_position);
}

void erase_formatted_text_position_single(uint32_t cell, uint32_t text, uint32_t text_position)
{
	text_position = (text_position & ~0xFFFFu) | mem_read16(text);
	text += 2u;
	erase_formatted_text_attribute_single(cell, text, text_position);
}

void erase_formatted_text_attribute_single(uint32_t cell, uint32_t text, uint32_t text_position)
{
	text++;
	erase_formatted_text_single(cell, text, text_position);
}

void erase_formatted_text_single(uint32_t cell, uint32_t text, uint32_t text_position)
{
	uint32_t zero = 0;
	uint32_t code = 0;
	uint8_t first_char = 0, next_char = 0;

	for (;;)
	{
		first_char = mem_read8(text);
		zero = first_char == 0u;
		text++;
		if (first_char != 0u)
		{
			do
			{
				mem_write16(text_position, cell);
				next_char = mem_read8(text);
				text++;
				zero = next_char == 0u;
				text_position += 2u;
			} while (next_char != 0u);
		}
		code = mem_read8(text);
		text++;
		switch (code)
		{
		case 0x0u:
			break;
		case 0x3u:
			continue;
		case 0x6u:
			erase_formatted_text_double(cell, text, text_position);
			return;
		case 0x9u:
			erase_formatted_text_triple(cell, text, text_position);
			return;
		case 0xCu:
			erase_formatted_text_attribute_single(cell, text, text_position);
			return;
		case 0xFu:
			erase_formatted_text_position_single(cell, text, text_position);
			return;
		case 0x12u:
			erase_formatted_text_attribute_single(cell, text, text_position);
			return;
		case 0x15u:
			erase_formatted_text_position_double(cell, text, text_position);
			return;
		case 0x18u:
			erase_formatted_text_attribute_double(cell, text, text_position);
			return;
		case 0x1Bu:
			erase_formatted_text_position_triple(cell, text, text_position);
			return;
		case 0x1Eu:
			erase_formatted_text_attribute_triple(cell, text, text_position);
			return;
		case 0x21u:
			erase_formatted_text_skip_single(cell, text, text_position);
			return;
		default:
			unknown_code(0x997BCu + code);
		}
		if (zero)
			return;
	}
	erase_formatted_text_skip_single(cell, text, text_position);
}

void erase_formatted_text_skip_single(uint32_t cell, uint32_t text, uint32_t text_position)
{
	text_position += (int8_t)mem_read8(text) * 2u;
	text++;
	erase_formatted_text_single(cell, text, text_position);
}

void erase_formatted_text_position_double(uint32_t cell, uint32_t text, uint32_t text_position)
{
	text_position = (text_position & ~0xFFFFu) | mem_read16(text);
	text += 2u;
	erase_formatted_text_attribute_double(cell, text, text_position);
}

void erase_formatted_text_attribute_double(uint32_t cell, uint32_t text, uint32_t text_position)
{
	text++;
	erase_formatted_text_double(cell, text, text_position);
}

void erase_formatted_text_double(uint32_t cell, uint32_t text, uint32_t text_position)
{
	uint32_t zero = 0;
	uint32_t code = 0;
	uint8_t first_char = 0, next_char = 0;

	for (;;)
	{
		first_char = mem_read8(text);
		zero = first_char == 0u;
		text++;
		if (first_char != 0u)
		{
			do
			{
				mem_write32(text_position, cell);
				next_char = mem_read8(text);
				text++;
				zero = next_char == 0u;
				text_position += 4u;
			} while (next_char != 0u);
		}
		code = mem_read8(text);
		text++;
		switch (code)
		{
		case 0x0u:
			if (zero)
				return;
			erase_formatted_text_single(cell, text, text_position);
			return;
		case 0x3u:
			erase_formatted_text_single(cell, text, text_position);
			return;
		case 0x6u:
			continue;
		case 0x9u:
			erase_formatted_text_triple(cell, text, text_position);
			return;
		case 0xCu:
			erase_formatted_text_attribute_double(cell, text, text_position);
			return;
		case 0xFu:
			erase_formatted_text_position_single(cell, text, text_position);
			return;
		case 0x12u:
			erase_formatted_text_attribute_single(cell, text, text_position);
			return;
		case 0x15u:
			erase_formatted_text_position_double(cell, text, text_position);
			return;
		case 0x18u:
			erase_formatted_text_attribute_double(cell, text, text_position);
			return;
		case 0x1Bu:
			erase_formatted_text_position_triple(cell, text, text_position);
			return;
		case 0x1Eu:
			erase_formatted_text_attribute_triple(cell, text, text_position);
			return;
		case 0x21u:
			erase_formatted_text_skip_double(cell, text, text_position);
			return;
		default:
			unknown_code(0x997FDu + code);
		}
	}
	if (zero)
		return;
	erase_formatted_text_single(cell, text, text_position);
}

void erase_formatted_text_skip_double(uint32_t cell, uint32_t text, uint32_t text_position)
{
	text_position += (int8_t)mem_read8(text) * 2u;
	text++;
	erase_formatted_text_double(cell, text, text_position);
}

void erase_formatted_text_position_triple(uint32_t cell, uint32_t text, uint32_t text_position)
{
	text_position = (text_position & ~0xFFFFu) | mem_read16(text);
	text += 2u;
	erase_formatted_text_attribute_triple(cell, text, text_position);
}

void erase_formatted_text_attribute_triple(uint32_t cell, uint32_t text, uint32_t text_position)
{
	text++;
	erase_formatted_text_triple(cell, text, text_position);
}

void erase_formatted_text_triple(uint32_t cell, uint32_t text, uint32_t text_position)
{
	uint32_t zero = 0;
	uint32_t code = 0;
	uint8_t first_char = 0, next_char = 0;

	for (;;)
	{
		first_char = mem_read8(text);
		zero = first_char == 0u;
		text++;
		if (first_char != 0u)
		{
			do
			{
				mem_write32(text_position, cell);
				text_position += 4u;
				mem_write16(text_position, cell);
				next_char = mem_read8(text);
				text++;
				zero = next_char == 0u;
				text_position += 2u;
			} while (next_char != 0u);
		}
		code = mem_read8(text);
		text++;
		switch (code)
		{
		case 0x0u:
			if (zero)
				return;
			erase_formatted_text_single(cell, text, text_position);
			return;
		case 0x3u:
			erase_formatted_text_single(cell, text, text_position);
			return;
		case 0x6u:
			erase_formatted_text_double(cell, text, text_position);
			return;
		case 0x9u:
			continue;
		case 0xCu:
			erase_formatted_text_attribute_triple(cell, text, text_position);
			return;
		case 0xFu:
			erase_formatted_text_position_single(cell, text, text_position);
			return;
		case 0x12u:
			erase_formatted_text_attribute_single(cell, text, text_position);
			return;
		case 0x15u:
			erase_formatted_text_position_double(cell, text, text_position);
			return;
		case 0x18u:
			erase_formatted_text_attribute_double(cell, text, text_position);
			return;
		case 0x1Bu:
			erase_formatted_text_position_triple(cell, text, text_position);
			return;
		case 0x1Eu:
			erase_formatted_text_attribute_triple(cell, text, text_position);
			return;
		case 0x21u:
			erase_formatted_text_skip_triple(cell, text, text_position);
			return;
		default:
			unknown_code(0x99841u + code);
		}
	}
	if (zero)
		return;
	erase_formatted_text_single(cell, text, text_position);
}

void erase_formatted_text_skip_triple(uint32_t cell, uint32_t text, uint32_t text_position)
{
	text_position += (int8_t)mem_read8(text) * 2u;
	text++;
	erase_formatted_text_triple(cell, text, text_position);
}

void run_vram_script(uint32_t *p_count, uint32_t *p_script, uint32_t *p_dest)
{
	uint32_t count = *p_count;
	uint32_t script = *p_script;
	uint32_t dest = *p_dest;
	uint32_t dictionary_cursor = 0, script_code = 0;

	dictionary_cursor = vram_script_dictionary_ADDRESS;
	for (;;)
	{
		script_code = mem_read8(script);
		script++;
		if ((int8_t)script_code >= 0)
		{
			mem_write16(dest, vram_script_dictionary[script_code]);
			dest += 2u;
			continue;
		}
		script_code = script_code & 0x7Fu;
		switch (script_code)
		{
		case 0x0u:
			vram_script_set_address(script_code, &script, &dest);
			continue;
		case 0x3u:
			vram_script_skip(&script, &dest);
			continue;
		case 0x6u:
			vram_script_fill(&count, &script, &dest);
			continue;
		case 0x9u:
			vram_script_count_up(&count, &script, &dest);
			continue;
		case 0xCu:
			vram_script_count_down(&count, &script, &dest);
			continue;
		case 0xFu:
			vram_script_bytes(&count, &script, &dest);
			continue;
		case 0x12u:
			vram_script_words(&count, &script, &dest);
			continue;
		case 0x15u:
			vram_script_words_to_dictionary(&count, &script, &dest, &dictionary_cursor);
			continue;
		case 0x18u:
			vram_script_clear_pages(&script, &dest);
			continue;
		case 0x1Bu:
			vram_script_fill_down(&count, &script, &dest);
			continue;
		case 0x1Eu:
			vram_script_fill_down_wide(&count, &script, &dest);
			continue;
		case 0x21u:
			vram_script_fill_up(&count, &script, &dest);
			continue;
		case 0x24u:
			vram_script_repeat(dest, &count, &script);
			continue;
		case 0x27u:
			vram_script_skip_long(&script, &dest);
			continue;
		case 0x2Au:
			*p_count = count;
			*p_script = script;
			*p_dest = dest;
			return;
		default:
			unknown_code(0x9988Eu + script_code);
		}
	}
	*p_count = count;
	*p_script = script;
	*p_dest = dest;
}

void vram_script_end(void)
{
}

void vram_script_set_address(uint32_t r0, uint32_t *p_script, uint32_t *p_r11)
{
	uint32_t script = *p_script;
	uint32_t dest = 0;

	dest = 0x300000u + ((r0 & ~0xFFFFu) | mem_read16(script)) * 2u;
	script += 2u;
	*p_script = script;
	*p_r11 = dest;
}

void vram_script_skip(uint32_t *p_script, uint32_t *p_dest)
{
	uint32_t script = *p_script;
	uint32_t dest = *p_dest;

	dest += (int8_t)mem_read8(script) * 2u;
	script++;
	*p_script = script;
	*p_dest = dest;
}

void vram_script_skip_long(uint32_t *p_script, uint32_t *p_dest)
{
	uint32_t script = *p_script;
	uint32_t dest = *p_dest;

	dest += (int16_t)mem_read16(script) * 2u;
	script += 2u;
	*p_script = script;
	*p_dest = dest;
}

void vram_script_fill(uint32_t *p_count, uint32_t *p_script, uint32_t *p_dest)
{
	uint32_t script = *p_script;
	uint32_t dest = *p_dest;
	uint16_t tile = 0;
	uint32_t count = 0;

	count = mem_read8(script);
	script++;
	tile = mem_read16(script);
	script += 2u;
	do
	{
		mem_write16(dest, tile);
		count--;
		dest += 2u;
	} while (count != 0u);
	*p_count = count;
	*p_script = script;
	*p_dest = dest;
}

void vram_script_count_up(uint32_t *p_count, uint32_t *p_script, uint32_t *p_dest)
{
	uint32_t script = *p_script;
	uint32_t dest = *p_dest;
	uint16_t tile = 0;
	uint32_t count = 0;

	count = mem_read8(script);
	script++;
	tile = mem_read16(script);
	script += 2u;
	do
	{
		mem_write16(dest, tile);
		tile++;
		count--;
		dest += 2u;
	} while (count != 0u);
	*p_count = count;
	*p_script = script;
	*p_dest = dest;
}

void vram_script_count_down(uint32_t *p_count, uint32_t *p_script, uint32_t *p_dest)
{
	uint32_t script = *p_script;
	uint32_t dest = *p_dest;
	uint16_t tile = 0;
	uint32_t count = 0;

	count = mem_read8(script);
	script++;
	tile = mem_read16(script);
	script += 2u;
	do
	{
		mem_write16(dest, tile);
		tile--;
		count--;
		dest += 2u;
	} while (count != 0u);
	*p_count = count;
	*p_script = script;
	*p_dest = dest;
}

void vram_script_bytes(uint32_t *p_count, uint32_t *p_script, uint32_t *p_dest)
{
	uint32_t script = *p_script;
	uint32_t dest = *p_dest;
	uint16_t tile = 0;
	uint32_t count = 0;

	count = mem_read8(script);
	script++;
	tile = (uint16_t)((uint32_t)mem_read8(script) << 8);
	script++;
	do
	{
		tile = (uint16_t)((tile & ~0xFFu) | mem_read8(script));
		mem_write16(dest, tile);
		dest += 2u;
		count--;
		script++;
	} while (count != 0u);
	*p_count = count;
	*p_script = script;
	*p_dest = dest;
}

void vram_script_words(uint32_t *p_count, uint32_t *p_script, uint32_t *p_dest)
{
	uint32_t script = *p_script;
	uint32_t dest = *p_dest;
	uint32_t count = 0;

	count = mem_read8(script);
	script++;
	do
	{
		mem_write16(dest, mem_read16(script));
		dest += 2u;
		count--;
		script += 2u;
	} while (count != 0u);
	*p_count = count;
	*p_script = script;
	*p_dest = dest;
}

void vram_script_words_to_dictionary(uint32_t *p_count, uint32_t *p_script, uint32_t *p_dest, uint32_t *p_dictionary_cursor)
{
	uint32_t script = *p_script;
	uint32_t dest = *p_dest;
	uint32_t dictionary_cursor = *p_dictionary_cursor;
	uint16_t tile = 0;
	uint32_t count = 0;

	count = mem_read8(script);
	script++;
	do
	{
		tile = mem_read16(script);
		mem_write16(dest, tile);
		dest += 2u;
		mem_write16(dictionary_cursor, tile);
		dictionary_cursor = (dictionary_cursor & ~0xFFu) | (uint8_t)(dictionary_cursor + 2u);
		count--;
		script += 2u;
	} while (count != 0u);
	*p_count = count;
	*p_script = script;
	*p_dest = dest;
	*p_dictionary_cursor = (*p_dictionary_cursor & 0xFFFFFF00u) | (dictionary_cursor & 0xFFu);
}

void vram_script_clear_pages(uint32_t *p_script, uint32_t *p_dest)
{
	uint32_t script = *p_script;
	uint32_t dest = 0, halfword_count = 0;

	dest = 0x300000u + mem_read16(script) * 2u;
	script += 2u;
	halfword_count = (uint32_t)mem_read8(script) << 9;
	fill_value = 0u;
	script++;
	set_halfwords(dest, halfword_count, fill_value);
	*p_script = script;
	*p_dest = dest;
}

void vram_script_fill_down(uint32_t *p_count, uint32_t *p_script, uint32_t *p_dest)
{
	uint32_t script = *p_script;
	uint32_t dest = *p_dest;
	uint16_t tile = 0;
	uint32_t count = 0;

	count = mem_read8(script);
	script++;
	tile = mem_read16(script);
	script += 2u;
	do
	{
		mem_write16(dest, tile);
		dest += 0x40u;
		count--;
	} while (count != 0u);
	*p_count = count;
	*p_script = script;
	*p_dest = dest;
}

void vram_script_fill_up(uint32_t *p_count, uint32_t *p_script, uint32_t *p_dest)
{
	uint32_t script = *p_script;
	uint32_t dest = *p_dest;
	uint16_t tile = 0;
	uint32_t count = 0;

	count = mem_read8(script);
	script++;
	tile = mem_read16(script);
	script += 2u;
	do
	{
		mem_write16(dest, tile);
		dest -= 0x40u;
		count--;
	} while (count != 0u);
	*p_count = count;
	*p_script = script;
	*p_dest = dest;
}

void vram_script_fill_down_wide(uint32_t *p_count, uint32_t *p_script, uint32_t *p_dest)
{
	uint32_t script = *p_script;
	uint32_t dest = *p_dest;
	uint16_t tile = 0;
	uint32_t count = 0;

	count = mem_read8(script);
	script++;
	tile = mem_read16(script);
	script += 2u;
	do
	{
		mem_write16(dest, tile);
		dest += 0x80u;
		count--;
	} while (count != 0u);
	*p_count = count;
	*p_script = script;
	*p_dest = dest;
}

void vram_script_repeat(uint32_t dest, uint32_t *p_count, uint32_t *p_script)
{
	uint32_t script = *p_script;
	uint16_t tile = 0;
	uint32_t count = 0;

	count = mem_read8(script);
	script++;
	tile = mem_read16(script);
	script += 2u;
	do
	{
		mem_write16(dest, tile);
		count--;
	} while (count != 0u);
	*p_count = count;
	*p_script = script;
}

void erase_vram_script(uint32_t script, uint32_t dest)
{
	uint16_t cell = 0;
	uint32_t script_code = 0;

	cell = 0u;
	for (;;)
	{
		script_code = mem_read8(script);
		script++;
		if ((int8_t)script_code >= 0)
		{
			mem_write16(dest, cell);
			dest += 2u;
			continue;
		}
		script_code = script_code & 0x7Fu;
		switch (script_code)
		{
		case 0x0u:
			vram_script_set_address(script_code, &script, &dest);
			continue;
		case 0x3u:
			vram_script_skip(&script, &dest);
			continue;
		case 0x6u:
			erase_vram_script_fill(cell, &script, &dest);
			continue;
		case 0x9u:
			erase_vram_script_fill(cell, &script, &dest);
			continue;
		case 0xCu:
			erase_vram_script_fill(cell, &script, &dest);
			continue;
		case 0xFu:
			erase_vram_script_bytes(cell, &script, &dest);
			continue;
		case 0x12u:
			erase_vram_script_words(cell, &script, &dest);
			continue;
		case 0x15u:
			erase_vram_script_words(cell, &script, &dest);
			continue;
		case 0x18u:
			vram_script_clear_pages(&script, &dest);
			continue;
		case 0x1Bu:
			erase_vram_script_fill_down(cell, &script, &dest);
			continue;
		case 0x1Eu:
			erase_vram_script_fill_down_wide(cell, &script, &dest);
			continue;
		case 0x21u:
			erase_vram_script_fill_up(cell, &script, &dest);
			continue;
		case 0x24u:
			script = erase_vram_script_repeat(cell, script, dest);
			continue;
		case 0x27u:
			vram_script_skip_long(&script, &dest);
			continue;
		case 0x2Au:
			return;
		default:
			unknown_code(0x999B7u + script_code);
		}
	}
}

void erase_vram_script_end(void)
{
}

void erase_vram_script_fill(uint16_t cell, uint32_t *p_script, uint32_t *p_dest)
{
	uint32_t script = *p_script;
	uint32_t dest = *p_dest;
	uint32_t count = 0;

	count = mem_read8(script);
	script += 3u;
	do
	{
		mem_write16(dest, cell);
		count--;
		dest += 2u;
	} while (count != 0u);
	*p_script = script;
	*p_dest = dest;
}

void erase_vram_script_bytes(uint16_t cell, uint32_t *p_script, uint32_t *p_dest)
{
	uint32_t script = *p_script;
	uint32_t dest = *p_dest;
	uint32_t count = 0;

	count = mem_read8(script);
	script = script + 2u + count;
	do
	{
		mem_write16(dest, cell);
		count--;
		dest += 2u;
	} while (count != 0u);
	*p_script = script;
	*p_dest = dest;
}

void erase_vram_script_words(uint16_t cell, uint32_t *p_script, uint32_t *p_dest)
{
	uint32_t script = *p_script;
	uint32_t dest = *p_dest;
	uint32_t count = 0;

	count = mem_read8(script);
	script = script + 1u + count * 2u;
	do
	{
		mem_write16(dest, cell);
		count--;
		dest += 2u;
	} while (count != 0u);
	*p_script = script;
	*p_dest = dest;
}

void erase_vram_script_fill_down(uint16_t cell, uint32_t *p_script, uint32_t *p_dest)
{
	uint32_t script = *p_script;
	uint32_t dest = *p_dest;
	uint32_t count = 0;

	count = mem_read8(script);
	script += 3u;
	do
	{
		mem_write16(dest, cell);
		dest += 0x40u;
		count--;
	} while (count != 0u);
	*p_script = script;
	*p_dest = dest;
}

void erase_vram_script_fill_up(uint16_t cell, uint32_t *p_script, uint32_t *p_dest)
{
	uint32_t script = *p_script;
	uint32_t dest = *p_dest;
	uint32_t count = 0;

	count = mem_read8(script);
	script += 3u;
	do
	{
		mem_write16(dest, cell);
		dest -= 0x40u;
		count--;
	} while (count != 0u);
	*p_script = script;
	*p_dest = dest;
}

void erase_vram_script_fill_down_wide(uint16_t cell, uint32_t *p_script, uint32_t *p_dest)
{
	uint32_t script = *p_script;
	uint32_t dest = *p_dest;
	uint32_t count = 0;

	count = mem_read8(script);
	script += 3u;
	do
	{
		mem_write16(dest, cell);
		dest += 0x80u;
		count--;
	} while (count != 0u);
	*p_script = script;
	*p_dest = dest;
}

uint32_t erase_vram_script_repeat(uint16_t cell, uint32_t script, uint32_t dest)
{
	uint32_t count = 0;

	count = mem_read8(script);
	script += 3u;
	do
	{
		mem_write16(dest, cell);
		count--;
	} while (count != 0u);
	return script;
}

uint32_t draw_hex_2(uint32_t value, uint32_t attribute, uint32_t text_position)
{
	uint32_t digit_count = 0;

	value = (uint8_t)value;
	digit_count = 2u;
	draw_hex_digits(digit_count, &value, &attribute, &text_position);
	return value;
}

void draw_hex_4(uint32_t *p_value, uint32_t *p_attribute, uint32_t *p_text_position)
{
	uint32_t value = *p_value;
	uint32_t attribute = *p_attribute;
	uint32_t text_position = *p_text_position;
	uint32_t digit_count = 0;

	value = (uint16_t)value;
	digit_count = 4u;
	draw_hex_digits(digit_count, &value, &attribute, &text_position);
	*p_value = value;
	*p_attribute = (*p_attribute & 0xFFFFFF00u) | (attribute & 0xFFu);
	*p_text_position = text_position;
}

void draw_hex_6(uint32_t value, uint32_t attribute, uint32_t text_position)
{
	uint32_t digit_count = 0;

	value &= 0xFFFFFFu;
	digit_count = 6u;
	draw_hex_digits(digit_count, &value, &attribute, &text_position);
}

void draw_hex_8(uint32_t value, uint32_t attribute, uint32_t text_position)
{
	uint32_t digit_count = 0;

	digit_count = 8u;
	draw_hex_digits(digit_count, &value, &attribute, &text_position);
}

void draw_hex_digits(uint32_t digit_count, uint32_t *p_value, uint32_t *p_attribute, uint32_t *p_text_position)
{
	uint32_t value = *p_value;
	uint16_t attribute = (uint16_t)*p_attribute;
	uint32_t text_position = *p_text_position;
	uint32_t remaining = 0;

	text_position += digit_count * 2u;
	do
	{
		attribute = (uint16_t)((attribute & ~0xFFu) | (uint8_t)value);
		attribute = (uint16_t)((attribute & ~0xFFu) | (attribute & 0xFu));
		if ((uint8_t)attribute >= 0xAu)
			attribute = (uint16_t)((attribute & ~0xFFu) | (uint8_t)(attribute + 7u));
		attribute = (uint16_t)((attribute & ~0xFFu) | (uint8_t)(attribute + 0x30u));
		text_position -= 2u;
		mem_write16(text_position, attribute);
		remaining = value >> 4;
		value = remaining;
		digit_count--;
	} while ((digit_count != 0u) & (remaining != 0u));
	if (digit_count == 0u)
	{
		*p_value = value;
		*p_attribute = (*p_attribute & 0xFFFFFF00u) | (attribute & 0xFFu);
		*p_text_position = text_position;
		return;
	}
	do
	{
		text_position -= 2u;
		mem_write16(text_position, value);
		digit_count--;
	} while (digit_count != 0u);
	*p_value = value;
	*p_attribute = (*p_attribute & 0xFFFFFF00u) | (attribute & 0xFFu);
	*p_text_position = text_position;
}

void draw_credit_digits(uint32_t value, uint32_t *p_attribute, uint32_t *p_text_position)
{
	uint32_t attribute = *p_attribute;
	uint32_t text_position = *p_text_position;
	uint32_t digits_left = 0;

	value = (uint8_t)value;
	digits_left = 2u;
	draw_bcd_digits(value, digits_left, &attribute, &text_position);
	*p_attribute = (*p_attribute & 0xFFFFFF00u) | (attribute & 0xFFu);
	*p_text_position = text_position;
}
