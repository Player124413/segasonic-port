/**
 *    Name:        system/coins.c
 *    Description: coins and credits: the coin inputs and counters, the credits and start prompts drawn outside a game
 *    Version:     0.1.0
 *    Created:     12-10-2026
 *    Modified:    15-10-2026
 *    Author:      Sebastian Toyos
 *
 *    SegaSonic The Hedgehog (arcade, Sega System 32): the game's V60 program decompiled into C by
 *    tools/decompile.py, with the names of symbols.txt.
 */
#include "game.h"

void apply_settings(void)
{
	uint16_t credit_cap = 0;
	uint32_t block_cursor = 0, credit_blocks_left = 0, credits = 0, saved_cursor = 0, vitality = 0;

	credit_coin_cost = setting_coins_per_credit;
	credits_per_start = setting_credits_to_start;
	health_setting = setting_difficulty;
	vitality = setting_initial_vitality;
	starting_health = starting_health_by_vitality[vitality & 3u];
	AS32(coin_counter_1_on) = 0u;
	chute_1_multiplier = setting_chute_1_multiplier;
	AS32(coin_counter_2_on) = 0u;
	chute_2_multiplier = setting_chute_2_multiplier;
	service_coin_sounds = 0u;
	coin_sounds = 0u;
	AS16(shared_coin_counter) = (uint8_t)((chute_2_multiplier == chute_1_multiplier) * 0xFCu);
	bonus_adder = setting_bonus_adder;
	free_play = 0u;
	if (bonus_adder == 1u)
		free_play = 1u;
	credit_cap = 0x900u;
	saved_cursor = saved_credits;
	block_cursor = p1_coins_ADDRESS;
	credit_blocks_left = 3u;
	do
	{
		credits = mem_read16(saved_cursor);
		credits = credits & ~0xFFu;
		saved_cursor += 2u;
		if ((int16_t)credit_cap < (int16_t)credits)
			credits = credit_cap;
		mem_write32(block_cursor, credits);
		block_cursor += 4u;
		credit_blocks_left--;
	} while (credit_blocks_left != 0u);
}

void update_coins(void)
{
	uint8_t changed_flag = 0, max_credits = 0, sound_id = 0;
	uint32_t block_cursor = 0, bookkeeping_chute = 0, chute_offset = 0, coin_credit_block = 0, coin_credits_added = 0, coin_inputs = 0, counter_bit = 0, counter_state = 0, credit_block = 0, credit_blocks_left = 0, ignored = 0, pending_offset = 0, service_credits_added = 0;
	uint32_t coins_released = 0;

	block_cursor = p1_coins_ADDRESS;
	changed_flag = 0u;
	credit_blocks_left = 3u;
	do
	{
		mem_write8(block_cursor + 3u, changed_flag);
		block_cursor += 4u;
		credit_blocks_left--;
	} while (credit_blocks_left != 0u);
	if (free_play != 0u)
		return;
	if (coin_sounds != 0u)
	{
		sound_id = 0x7Fu;
		queue_sound(sound_id, &ignored, &ignored);
		coin_sounds--;
	}
	if (service_coin_sounds != 0u)
	{
		sound_id = 0x7Fu;
		queue_sound(sound_id, &ignored, &ignored);
		service_coin_sounds--;
	}
	max_credits = 9u;
	service_credits_added = 0u;
	if ((inputs_pressed & 0x1001u) != 0u)
	{
		switch (cabinet_config)
		{
		case 0:
		case 1:
			break;
		case 2:
		update_coins_service_p2:
			credit_block = p2_coins_ADDRESS;
			service_credits_added = add_service_credit(service_credits_added, credit_block, max_credits);
			break;
		case 3:
		case 4:
			credit_block = p3_coins_ADDRESS;
			service_credits_added = add_service_credit(service_credits_added, credit_block, max_credits);
			goto update_coins_service_p2;
		default:
			unknown_code(0xFF7CDu + cabinet_config * 2u);
		}
		credit_block = p1_coins_ADDRESS;
		service_credits_added = add_service_credit(service_credits_added, credit_block, max_credits);
		mem_write16(bookkeeping_service_credits, mem_read16(bookkeeping_service_credits) + service_credits_added);
	}
	coin_credits_added = 0u;
	coins_released = inputs_released & 0x4Cu;
	coin_inputs = coins_released;
	if (coins_released != 0u)
	{
		coin_credit_block = p1_coins_ADDRESS;
		pending_offset = 0x7408u;
		chute_offset = 0u;
		switch (cabinet_config)
		{
		case 0:
		case 1:
		case 4:
			if ((coin_inputs & 8u) != 0u)
			{
				bookkeeping_chute = bookkeeping_coin_chute_2;
				if (shared_coin_counter == 0u)
				{
					mem_write8(cabinet_events, 0x11u);
					pending_offset = 0x740Cu;
					chute_offset = 4u;
				}
				else
					mem_write8(cabinet_events, 0x11u);
				coin_credits_added = add_coin(chute_offset, pending_offset, bookkeeping_chute, coin_credits_added, coin_credit_block, max_credits);
				pending_offset = 0x7408u;
				chute_offset = 0u;
			}
			break;
		case 2:
		update_coins_individual_chute_2:
			if ((coin_inputs & 8u) != 0u)
			{
				mem_write8(cabinet_events, 0x11u);
				bookkeeping_chute = bookkeeping_coin_chute_2;
				coin_credits_added = add_coin(chute_offset, pending_offset, bookkeeping_chute, coin_credits_added, coin_credit_block, max_credits);
			}
			coin_credit_block += 4u;
			break;
		case 3:
			if ((coin_inputs & 0x40u) != 0u)
			{
				mem_write8(cabinet_events, 0x11u);
				bookkeeping_chute = bookkeeping_coin_chute_3;
				coin_credits_added = add_coin(chute_offset, pending_offset, bookkeeping_chute, coin_credits_added, coin_credit_block, max_credits);
			}
			coin_credit_block += 4u;
			goto update_coins_individual_chute_2;
		default:
			unknown_code(0xFF821u + cabinet_config * 3u);
		}
		if ((coin_inputs & 4u) != 0u)
		{
			mem_write8(cabinet_events, 0x11u);
			bookkeeping_chute = bookkeeping_coin_chute_1;
			coin_credits_added = add_coin(chute_offset, pending_offset, bookkeeping_chute, coin_credits_added, coin_credit_block, max_credits);
		}
		mem_write16(bookkeeping_coin_credits, mem_read16(bookkeeping_coin_credits) + coin_credits_added);
	}
	if (shared_coin_counter == 0u)
	{
		counter_state = coin_counter_2_on_ADDRESS;
		counter_bit = 1u;
		pulse_coin_counter(counter_bit, counter_state, (uint16_t)service_credits_added, (uint16_t)coin_credits_added);
	}
	counter_state = coin_counter_1_on_ADDRESS;
	counter_bit = 0u;
	pulse_coin_counter(counter_bit, counter_state, (uint16_t)service_credits_added, (uint16_t)coin_credits_added);
}

void pulse_coin_counter(uint32_t counter_bit, uint32_t counter_state, uint16_t service_credits, uint16_t coin_credits)
{
	uint32_t pulse_timers = 0;
	uint8_t on_frames = 0;
	uint32_t port_word_before_on = 0, port_word_before_off = 0;

	pulse_timers = mem_read16(counter_state);
	counter_state += 2u;
	if (pulse_timers == 0u)
	{
		if (mem_read8(counter_state) != 0u)
		{
			mem_write8(counter_state, mem_read8(counter_state) - 1u);
			port_word_before_on = mem_read32(io_port_d);
			mem_write32(io_port_d, port_word_before_on | shl32(1u, (int)(counter_bit)));
			mem_write16(counter_state - 2u, 0x606u);
		}
	}
	else if ((uint8_t)pulse_timers == 0u)
	{
		counter_state--;
		mem_write8(counter_state, mem_read8(counter_state) - 1u);
	}
	else
	{
		on_frames = mem_read8(counter_state - 2u);
		mem_write8(counter_state - 2u, on_frames - 1u);
		if (on_frames == 1u)
		{
			port_word_before_off = mem_read32(io_port_d);
			mem_write32(io_port_d, port_word_before_off & ~shl32(1u, (int)(counter_bit)));
		}
	}
	if ((uint16_t)service_credits != 0u)
	{
		save_credits();
		return;
	}
	if ((uint16_t)coin_credits == 0u)
		return;
	save_credits();
}

uint32_t add_service_credit(uint32_t service_credits, uint32_t credit_block, uint8_t max_credits)
{
	if ((uint8_t)max_credits <= mem_read8(credit_block + 1u))
		return service_credits;
	mem_write8(credit_block + 3u, 1u);
	service_credits = (service_credits & ~0xFFFFu) | (uint16_t)(service_credits + 1u);
	mem_write8(credit_block + 1u, mem_read8(credit_block + 1u) + 1u);
	service_coin_sounds++;
	if ((uint8_t)max_credits <= mem_read8(credit_block + 1u))
	{
		mem_write8(credit_block + 2u, 0u);
		mem_write8(credit_block, 0u);
	}
	return service_credits;
}

uint32_t add_coin(uint32_t chute_offset, uint32_t pending_offset, uint32_t bookkeeping_chute, uint32_t coin_credits, uint32_t credit_block, uint8_t max_credits)
{
	uint8_t chute_multiplier = 0, coin_cost = 0, coin_tally = 0;
	uint32_t coins_per_bonus = 0;
	uint32_t counter_pending = 0;

	mem_write16(bookkeeping_chute, mem_read16(bookkeeping_chute) + 1u);
	counter_pending = GAME_VARIABLES + pending_offset;
	mem_write8(counter_pending, mem_read8(counter_pending) + 1u);
	if ((uint8_t)max_credits <= mem_read8(credit_block + 1u))
		return coin_credits;
	mem_write8(credit_block + 3u, 1u);
	coin_sounds++;
	chute_multiplier = mem_read8(chute_1_multiplier_ADDRESS + chute_offset);
	coins_per_bonus = bonus_adder;
	if (coins_per_bonus != 0u)
	{
		coin_tally = (uint8_t)(mem_read8(credit_block + 2u) + chute_multiplier);
		while (coins_per_bonus <= coin_tally)
		{
			mem_write8(credit_block, mem_read8(credit_block) + 1u);
			coin_tally -= coins_per_bonus;
		}
		mem_write8(credit_block + 2u, coin_tally);
	}
	coin_cost = credit_coin_cost;
	coin_tally = (uint8_t)(mem_read8(credit_block) + chute_multiplier);
	do
	{
		if (coin_tally < coin_cost)
		{
			mem_write8(credit_block, coin_tally);
			return coin_credits;
		}
		coin_tally -= coin_cost;
		coin_credits = (coin_credits & ~0xFFFFu) | (uint16_t)(coin_credits + 1u);
		mem_write8(credit_block + 1u, mem_read8(credit_block + 1u) + 1u);
	} while (mem_read8(credit_block + 1u) < (uint8_t)max_credits);
	mem_write8(credit_block + 1u, max_credits);
	mem_write8(credit_block + 2u, 0u);
	mem_write8(credit_block, 0u);
	return coin_credits;
}

void save_credits(void)
{
	uint32_t credit_block = 0, dest = 0;

	dest = saved_credits;
	credit_block = p1_coins_ADDRESS;
	copy_credit_counts(dest, credit_block);
	update_bookkeeping_checksum();
	dest = saved_credits_backup;
	credit_block = p1_coins_ADDRESS;
	copy_credit_counts(dest, credit_block);
}

void copy_credit_counts(uint32_t dest, uint32_t credit_block)
{
	r14 = 3u;
	do
	{
		mem_write16(dest, mem_read16(credit_block));
		credit_block += 4u;
		r14 = r14 - 1u;
		dest += 2u;
	} while (r14 != 0u);
}

void draw_attract_credits(uint32_t *p_credits, uint32_t *p_blink_bit, uint32_t *p_message_text, uint32_t *p_text_position)
{
	uint32_t credits = *p_credits;
	uint32_t blink_bit = *p_blink_bit;
	uint32_t r10 = 0, credit_block = 0, message_text = 0, prompt_texts = 0, text = 0, text_position = 0;

	switch (cabinet_config)
	{
	case 0:
	case 1:
	case 4:
		credit_block = p1_coins_ADDRESS;
		text_position = 0x310000u | common_credit_position;
		draw_credit_count(credit_block, &credits, &blink_bit, &r10, &text_position);
		credits = blink_demo_prompt(credits, credit_block);
		if (credits_per_start != 2u || free_play != 0u)
		{
			*p_credits = credits;
			*p_blink_bit = blink_bit;
			*p_message_text = r10;
			*p_text_position = text_position;
			return;
		}
		message_text = two_credits_one_play_text_ADDRESS;
		print_text(&message_text, &text_position);
		*p_credits = credits;
		*p_blink_bit = blink_bit;
		*p_message_text = message_text;
		*p_text_position = text_position;
		return;
	case 2:
		credit_block = p2_coins_ADDRESS;
		text_position = 0x310000u | credit_position_p3;
		draw_credit_count(credit_block, &credits, &blink_bit, &text, &text_position);
		text_position = 0x310000u | start_prompt_position_p3;
		prompt_texts = start_prompt_texts_ADDRESS;
		blink_start_prompt(credit_block, prompt_texts, &blink_bit, &text, &text_position);
		credit_block = p1_coins_ADDRESS;
		text_position = 0x310000u | credit_positions_3p[0];
		draw_credit_count(credit_block, &credits, &blink_bit, &r10, &text_position);
		text_position = 0x310000u | start_prompt_positions_3p[0];
		prompt_texts = start_prompt_texts_ADDRESS;
		blink_start_prompt(credit_block, prompt_texts, &blink_bit, &r10, &text_position);
		*p_credits = credits;
		*p_blink_bit = blink_bit;
		*p_message_text = r10;
		*p_text_position = text_position;
		return;
	case 3:
		credit_block = p3_coins_ADDRESS;
		text_position = 0x310000u | credit_position_p3;
		draw_credit_count(credit_block, &credits, &blink_bit, &text, &text_position);
		text_position = 0x310000u | start_prompt_position_p3;
		prompt_texts = start_prompt_texts_ADDRESS;
		blink_start_prompt(credit_block, prompt_texts, &blink_bit, &text, &text_position);
		credit_block = p2_coins_ADDRESS;
		text_position = 0x310000u | credit_position_p2;
		draw_credit_count(credit_block, &credits, &blink_bit, &text, &text_position);
		text_position = 0x310000u | start_prompt_position_p2;
		prompt_texts = start_prompt_texts_ADDRESS;
		blink_start_prompt(credit_block, prompt_texts, &blink_bit, &text, &text_position);
		credit_block = p1_coins_ADDRESS;
		text_position = 0x310000u | credit_positions_3p[0];
		draw_credit_count(credit_block, &credits, &blink_bit, &r10, &text_position);
		text_position = 0x310000u | start_prompt_positions_3p[0];
		prompt_texts = start_prompt_texts_ADDRESS;
		blink_start_prompt(credit_block, prompt_texts, &blink_bit, &r10, &text_position);
		*p_credits = credits;
		*p_blink_bit = blink_bit;
		*p_message_text = r10;
		*p_text_position = text_position;
		return;
	default:
		unknown_code(0xFFBA6u + cabinet_config * 3u);
	}
}

void erase_attract_credits(void)
{
	uint32_t prompt_texts = 0, text_position = 0;

	switch (cabinet_config)
	{
	case 0:
	case 1:
	case 4:
		text_position = common_credit_position;
		erase_credit_count(text_position);
		text_position = common_start_prompt_position;
		prompt_texts = common_start_prompt_texts_ADDRESS;
		erase_start_prompt(text_position, prompt_texts);
		return;
	case 2:
		text_position = credit_position_p2;
		erase_credit_count(text_position);
		text_position = start_prompt_position_p2;
		prompt_texts = start_prompt_texts_ADDRESS;
		erase_start_prompt(text_position, prompt_texts);
		text_position = credit_position_p3;
		erase_credit_count(text_position);
		text_position = start_prompt_positions_3p[0];
		prompt_texts = start_prompt_texts_ADDRESS;
		erase_start_prompt(text_position, prompt_texts);
		return;
	case 3:
		text_position = credit_position_p3;
		erase_credit_count(text_position);
		text_position = start_prompt_position_p3;
		prompt_texts = start_prompt_texts_ADDRESS;
		erase_start_prompt(text_position, prompt_texts);
		text_position = credit_position_p2;
		erase_credit_count(text_position);
		text_position = start_prompt_position_p2;
		prompt_texts = start_prompt_texts_ADDRESS;
		erase_start_prompt(text_position, prompt_texts);
		text_position = credit_position_p3;
		erase_credit_count(text_position);
		text_position = start_prompt_positions_3p[0];
		prompt_texts = start_prompt_texts_ADDRESS;
		erase_start_prompt(text_position, prompt_texts);
		return;
	default:
		unknown_code(0x9FC96u + cabinet_config * 2u);
	}
}

void blink_start_prompt(uint32_t credit_block, uint32_t prompt_texts, uint32_t *p_blink_bit, uint32_t *p_text, uint32_t *p_text_position)
{
	uint32_t blink_bit = *p_blink_bit;
	uint32_t text = *p_text;
	uint32_t text_position = *p_text_position;
	uint16_t credits = 0;
	uint32_t frame_bit = 0, prompt_index = 0;
	uint8_t port_d_bits = 0;

	if ((frame_counter & 0xFu) != 0u)
		return;
	prompt_index = 0u;
	frame_bit = (uint8_t)(5u - frames_per_wait);
	if ((AS32(frame_counter) & shl32(1u, (int)(frame_bit))) != 0u)
	{
		port_d_bits = mem_read8(io_port_d);
		mem_write8(io_port_d, port_d_bits & ~0xCu);
		text = prompt_texts + mem_read16(prompt_texts + prompt_index * 2u);
		erase_text_lines(&text, &text_position);
		*p_blink_bit = frame_bit;
		*p_text = text;
		*p_text_position = text_position;
		return;
	}
	if (free_play != 0u)
	{
		if ((cabinet_config & 1u) != 0u || setting_moving_seat != 0u)
		{
			text = prompt_texts + mem_read16(prompt_texts + prompt_index * 2u);
			print_text_lines(&text, &text_position);
			*p_blink_bit = frame_bit;
			*p_text = text;
			*p_text_position = text_position;
			return;
		}
		r14 = p1_coins_ADDRESS;
		text = prompt_texts + mem_read16(prompt_texts + prompt_index * 2u);
		print_text_lines(&text, &text_position);
		*p_blink_bit = frame_bit;
		*p_text = text;
		*p_text_position = text_position;
		return;
	}
	credits = mem_read8(credit_block + 1u);
	if (credits_per_start <= credits)
	{
		if ((cabinet_config & 1u) != 0u || setting_moving_seat != 0u)
		{
			text = prompt_texts + mem_read16(prompt_texts + prompt_index * 2u);
			print_text_lines(&text, &text_position);
			*p_blink_bit = frame_bit;
			*p_text = text;
			*p_text_position = text_position;
			return;
		}
		r14 = p1_coins_ADDRESS;
		text = prompt_texts + mem_read16(prompt_texts + prompt_index * 2u);
		print_text_lines(&text, &text_position);
		*p_blink_bit = frame_bit;
		*p_text = text;
		*p_text_position = text_position;
		return;
	}
	prompt_index = 1u;
	if (credits_per_start == 2u)
	{
		frame_bit++;
		if ((AS32(frame_counter) & shl32(1u, (int)(frame_bit))) != 0u)
			prompt_index = 3u;
	}
	if (mem_read8(credit_block) != 0u || credits != 0u)
	{
		text = prompt_texts + mem_read16(prompt_texts + prompt_index * 2u);
		print_text_lines(&text, &text_position);
		*p_blink_bit = frame_bit;
		*p_text = text;
		*p_text_position = text_position;
		return;
	}
	prompt_index = 2u;
	text = prompt_texts + mem_read16(prompt_texts + prompt_index * 2u);
	print_text_lines(&text, &text_position);
	*p_blink_bit = frame_bit;
	*p_text = text;
	*p_text_position = text_position;
}

void erase_start_prompt(uint32_t text_position, uint32_t prompt_texts)
{
	uint32_t text = 0;
	uint8_t port_d_bits = 0;

	port_d_bits = mem_read8(io_port_d);
	mem_write8(io_port_d, port_d_bits & ~0xCu);
	text = prompt_texts + mem_read16(prompt_texts + 2u);
	erase_text_lines(&text, &text_position);
}

void draw_credit_count(uint32_t credit_block, uint32_t *p_credits, uint32_t *p_coins_per_credit, uint32_t *p_text, uint32_t *p_text_position)
{
	uint32_t credits = *p_credits;
	uint32_t coins_per_credit = *p_coins_per_credit;
	uint32_t text_position = *p_text_position;
	uint32_t r10 = 0, compared_block = 0, drawn_bit = 0;
	uint32_t old_flags = 0;

	drawn_bit = 0xFu;
	compared_block = p1_coins_ADDRESS;
	if (credit_block != p1_coins_ADDRESS)
	{
		drawn_bit = 0x12u;
		compared_block = p2_coins_ADDRESS;
		if (credit_block != p2_coins_ADDRESS)
			drawn_bit = 0x13u;
	}
	old_flags = stage_init_flags;
	stage_init_flags = old_flags | shl32(1u, (int)(drawn_bit));
	if ((old_flags & shl32(1u, (int)(drawn_bit))) == 0u || mem_read8(credit_block + 3u) != 0u)
	{
		draw_credit_count_now(credit_block, &credits, &coins_per_credit, &r10, &text_position);
		*p_credits = credits;
		*p_coins_per_credit = coins_per_credit;
		*p_text = r10;
		*p_text_position = text_position;
		return;
	}
	*p_text = compared_block;
}

void draw_credit_count_now(uint32_t credit_block, uint32_t *p_credits, uint32_t *p_coins_per_credit, uint32_t *p_text, uint32_t *p_text_position)
{
	uint32_t credits = *p_credits;
	uint32_t coins_per_credit = *p_coins_per_credit;
	uint32_t text_position = *p_text_position;
	uint8_t attribute = 0;
	uint16_t tens = 0;
	uint32_t r1 = 0, coin_cost = 0, coins = 0, plural_cell = 0, text = 0;
	uint8_t tens_nibble = 0;

	credit_text_position = text_position;
	if (free_play != 0u)
	{
		text = free_play_text_ADDRESS;
		print_text_here(&plural_cell, &text, &text_position);
		*p_credits = credits;
		*p_coins_per_credit = coins_per_credit;
		*p_text = text;
		*p_text_position = text_position;
		return;
	}
	text = credits_label_text_ADDRESS;
	print_text_here(&plural_cell, &text, &text_position);
	r1 = (credits & ~0xFFu) | mem_read8(credit_block + 1u);
	if ((uint8_t)r1 >= 2u)
	{
		plural_cell = (plural_cell & ~0xFFu) | 0x73u;
		mem_write16(text_position - 4u, plural_cell);
	}
	tens = (uint16_t)((plural_cell & ~0xFFu) | (uint8_t)r1);
	tens = (uint16_t)((tens & ~0xFFu) | ((uint8_t)tens / 0xAu));
	r1 = (r1 & ~0xFFu) | ((uint8_t)r1 % 0xAu);
	tens_nibble = (uint32_t)tens << 4;
	tens = (uint16_t)((tens & ~0xFFu) | tens_nibble);
	if (tens_nibble == 0u)
	{
		mem_write16(text_position + 2u, 0u);
		text_position = (text_position & ~0xFFFFu) | (uint16_t)(text_position - 2u);
	}
	tens = (uint16_t)((tens & ~0xFFu) | (uint8_t)(tens | r1));
	r1 = (r1 & ~0xFFFFu) | (uint16_t)tens;
	draw_credit_digits(tens, &r1, &text_position);
	text_position = (text_position & ~0xFFFFu) | (uint16_t)(text_position + 4u);
	coin_cost = credit_coin_cost;
	if (coin_cost == 1u)
	{
		mem_write32(text_position, 0u);
		*p_credits = r1;
		*p_coins_per_credit = coin_cost;
		*p_text = text;
		*p_text_position = text_position;
		return;
	}
	coins = mem_read8(credit_block);
	if (coins == 0u)
	{
		mem_write32(text_position, 0u);
		*p_credits = coins;
		*p_coins_per_credit = coin_cost;
		*p_text = text;
		*p_text_position = text_position;
		return;
	}
	attribute = 0u;
	draw_fraction(attribute, &coins, &coin_cost, &text_position);
	*p_credits = coins;
	*p_coins_per_credit = coin_cost;
	*p_text = text;
	*p_text_position = text_position;
}

void build_fraction_glyph(uint32_t numerator, uint32_t denominator, uint32_t *p_numerator_top_rows, uint32_t *p_numerator_bottom_rows, uint32_t *p_denominator_top_rows, uint32_t *p_denominator_bottom_rows)
{
	uint32_t denominator_bottom_rows = 0, denominator_rows = 0, denominator_top_rows = 0, numerator_bottom_rows = 0, numerator_top_rows = 0;

	numerator = 0x104B79u + numerator * 2u * 4u;
	numerator_top_rows = mem_read32(numerator);
	numerator_bottom_rows = mem_read32(numerator + 4u);
	denominator_rows = fraction_digit_font_ADDRESS + denominator * 2u * 4u;
	denominator_top_rows = mem_read32(denominator_rows) | rom_104BCA;
	denominator_bottom_rows = mem_read32(denominator_rows + 4u) | rom_104BCE;
	*p_numerator_top_rows = numerator_top_rows;
	*p_numerator_bottom_rows = numerator_bottom_rows;
	*p_denominator_top_rows = denominator_top_rows;
	*p_denominator_bottom_rows = denominator_bottom_rows;
}

void erase_credit_count(uint32_t text_position)
{
	uint16_t cell = 0;
	uint32_t text = 0;

	text = credits_label_text_ADDRESS;
	cell = 0u;
	erase_text_here(cell, &text, &text_position);
	erase_text_here(cell, &text, &text_position);
}

uint32_t add_to_credit_pool(uint32_t credits_to_add)
{
	uint8_t credits_added = 0, excess = 0;
	uint32_t crc = 0, result = 0;
	uint8_t new_total = 0;

	if (free_play != 0u)
	{
		credits_to_add &= ~0xFFu;
		return credits_to_add;
	}
	credits_added = (uint8_t)credits_to_add;
	new_total = credits_to_add + credit_pool;
	excess = (uint8_t)(new_total - 9u);
	if ((int8_t)new_total >= 9)
	{
		credit_pool_coins = 0u;
		credits_added -= excess;
	}
	credit_pool += credits_added;
	mem_write8(bookkeeping_coin_credits, credit_pool);
	crc = update_bookkeeping_checksum();
	mem_write8(bookkeeping_backup, credit_pool);
	result = (crc & ~0xFFFFu) | credits_added;
	return result;
}

void take_from_credit_pool(uint8_t credits_to_take)
{
	if (free_play == 0u)
	{
		credit_pool -= credits_to_take;
		mem_write8(bookkeeping_coin_credits, credit_pool);
		update_bookkeeping_checksum();
		mem_write8(bookkeeping_backup, credit_pool);
	}
}
