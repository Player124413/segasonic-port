/**
 *    Name:        text/digits.c
 *    Description: hexadecimal and BCD digits on the text layer
 *    Version:     0.1.0
 *    Created:     12-10-2026
 *    Modified:    15-10-2026
 *    Author:      Sebastian Toyos
 *
 *    SegaSonic The Hedgehog (arcade, Sega System 32): the game's V60 program decompiled into C by
 *    tools/decompile.py, with the names of symbols.txt.
 */
#include "game.h"

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

void draw_bcd_4(uint32_t value, uint32_t attribute, uint32_t text_position)
{
	uint32_t digit_count = 0;

	value = (uint16_t)value;
	digit_count = 4u;
	draw_bcd_digits(value, digit_count, &attribute, &text_position);
}

void draw_bcd_6(uint32_t value, uint32_t attribute, uint32_t text_position)
{
	uint32_t digit_count = 0;

	value &= 0xFFFFFFu;
	digit_count = 6u;
	draw_bcd_digits(value, digit_count, &attribute, &text_position);
}

void draw_bcd_8(uint32_t value, uint32_t attribute, uint32_t text_position)
{
	uint32_t digit_count = 0;

	digit_count = 8u;
	draw_bcd_digits(value, digit_count, &attribute, &text_position);
}

void draw_bcd_digits(uint32_t value, uint32_t digit_count, uint32_t *p_attribute, uint32_t *p_text_position)
{
	uint16_t attribute = (uint16_t)*p_attribute;
	uint32_t text_position = *p_text_position;
	uint8_t shift = 0;
	uint32_t remaining = 0;

	text_position += digit_count * 2u;
	shift = 0xFCu;
	do
	{
		attribute = (uint16_t)((attribute & ~0xFFu) | (uint8_t)value);
		attribute = (uint16_t)((attribute & ~0xFFu) | (attribute & 0xFu));
		attribute = (uint16_t)((attribute & ~0xFFu) | (uint8_t)(attribute + 1u));
		text_position -= 2u;
		mem_write16(text_position, attribute);
		remaining = shift_by(value, shift, 32);
		value = remaining;
		digit_count--;
	} while ((digit_count != 0u) & (remaining != 0u));
	if (digit_count == 0u)
	{
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
	*p_attribute = (*p_attribute & 0xFFFFFF00u) | (attribute & 0xFFu);
	*p_text_position = text_position;
}
