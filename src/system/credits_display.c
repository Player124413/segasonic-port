/**
 *    Name:        system/credits_display.c
 *    Description: the credits and start prompts drawn outside a game; an unused older credit pool
 *    Version:     0.1.0
 *    Created:     12-10-2026
 *    Modified:    15-10-2026
 *    Author:      Sebastian Toyos
 *
 *    SegaSonic The Hedgehog (arcade, Sega System 32): the game's V60 program decompiled into C by
 *    tools/decompile.py, with the names of symbols.txt.
 */
#include "game.h"

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
