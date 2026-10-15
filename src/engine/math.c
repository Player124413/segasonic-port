/**
 *    Name:        engine/math.c
 *    Description: square_root, angle_of_vector, BCD conversions
 *    Version:     0.1.0
 *    Created:     12-10-2026
 *    Modified:    15-10-2026
 *    Author:      Sebastian Toyos
 *
 *    SegaSonic The Hedgehog (arcade, Sega System 32): the game's V60 program decompiled into C by
 *    tools/decompile.py, with the names of symbols.txt.
 */
#include "game.h"

void square_root(uint32_t *p_value, uint32_t *p_steps_left)
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
				*p_steps_left = steps_left;
				return;
			}
			continue;
		}
		root_remainder -= value;
		value++;
		steps_left--;
		if (steps_left == 0u)
			break;
	}
	value = value >> 1;
	*p_value = value;
	*p_steps_left = steps_left;
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
