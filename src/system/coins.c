/**
 *    Name:        system/coins.c
 *    Description: coins and credits: apply_settings, update_coins, the coin counters, saving the credits
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
