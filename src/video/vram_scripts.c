/**
 *    Name:        video/vram_scripts.c
 *    Description: run_vram_script and its commands; the unused eraser
 *    Version:     0.1.0
 *    Created:     12-10-2026
 *    Modified:    15-10-2026
 *    Author:      Sebastian Toyos
 *
 *    SegaSonic The Hedgehog (arcade, Sega System 32): the game's V60 program decompiled into C by
 *    tools/decompile.py, with the names of symbols.txt.
 */
#include "game.h"

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
}

void vram_script_end(void)
{
}

void vram_script_set_address(uint32_t script_code, uint32_t *p_script, uint32_t *p_dest)
{
	uint32_t script = *p_script;
	uint32_t dest = 0;

	dest = 0x300000u + ((script_code & ~0xFFFFu) | mem_read16(script)) * 2u;
	script += 2u;
	*p_script = script;
	*p_dest = dest;
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
