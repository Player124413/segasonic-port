/**
 *    Name:        text/text_layer.c
 *    Description: printing on the text layer: print_text, erase_text, lines, fractions; the unused formatted text
 *    Version:     0.1.0
 *    Created:     12-10-2026
 *    Modified:    15-10-2026
 *    Author:      Sebastian Toyos
 *
 *    SegaSonic The Hedgehog (arcade, Sega System 32): the game's V60 program decompiled into C by
 *    tools/decompile.py, with the names of symbols.txt.
 */
#include "game.h"

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
}

void erase_formatted_text_skip_triple(uint32_t cell, uint32_t text, uint32_t text_position)
{
	text_position += (int8_t)mem_read8(text) * 2u;
	text++;
	erase_formatted_text_triple(cell, text, text_position);
}
