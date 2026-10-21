/**
 *    Name:        rom_set.c
 *    Description: fills the board's memory (game/data.h) from the player's ROM set, for the reference
 *    Version:     0.1.0
 *    Created:     20-10-2026
 *    Modified:    20-10-2026
 *    Author:      Sebastian Toyos
 *
 *    license:BSD-3-Clause
 *    The reference runs the original code, so it reads the ROM set itself when it starts, as MAME does;
 *    the game never does (it loads its assets pack). The set's files are in a folder, by MAME's names or
 *    the older epr-c-NN ones, each checked by its CRC32; the regions are laid out as tools/sonic_rom.py
 *    lays them, after ROM_START( sonic ) in MAME's src/mame/sega/segas32.cpp (BSD-3-Clause).
 */
#include "rom_set.h"

#include "game/data.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct
{
	const char *name;
	const char *older;    /* the name older dumps give it, or NULL */
	uint32_t crc, size;
} rom_file;

enum
{
	IC17, IC8, IC18, IC9, IC36, IC35, IC34, IC33, IC14, IC5, IC32, IC30, IC28, IC26, IC31, IC29, IC27, IC25, FILES
};

static const rom_file files[FILES] = {
	{ "epr-15787c.ic17", "epr-c-87.ic17", 0x25e3c27eu, 0x020000u },
	{ "epr-15786c.ic8", "epr-c-86.ic8", 0xefe9524cu, 0x020000u },
	{ "epr-15781c.ic18", "epr-c-81.ic18", 0x65b06c25u, 0x040000u },
	{ "epr-15780c.ic9", "epr-c-80.ic9", 0x2db66fd2u, 0x040000u },
	{ "epr-15785.ic36", NULL, 0x0fe7422eu, 0x040000u },
	{ "mpr-15784.ic35", NULL, 0x42f06714u, 0x100000u },
	{ "mpr-15783.ic34", NULL, 0xe4220eeau, 0x100000u },
	{ "mpr-15782.ic33", NULL, 0xcf56b5a0u, 0x100000u },
	{ "mpr-15789.ic14", NULL, 0x4378f12bu, 0x100000u },
	{ "mpr-15788.ic5", NULL, 0xa6ed5d7au, 0x100000u },
	{ "mpr-15790.ic32", NULL, 0xc69d51b1u, 0x200000u },
	{ "mpr-15792.ic30", NULL, 0x1006bb67u, 0x200000u },
	{ "mpr-15794.ic28", NULL, 0x8672b480u, 0x200000u },
	{ "mpr-15796.ic26", NULL, 0x95b8011cu, 0x200000u },
	{ "mpr-15791.ic31", NULL, 0x42217066u, 0x100000u },
	{ "mpr-15793.ic29", NULL, 0x75bafe55u, 0x100000u },
	{ "mpr-15795.ic27", NULL, 0x7f3dad30u, 0x100000u },
	{ "mpr-15797.ic25", NULL, 0x013c6cabu, 0x100000u },
};

static uint32_t crc32_of(const uint8_t *data, uint32_t size)
{
	uint32_t crc = 0xFFFFFFFFu, index;
	int bit;

	for (index = 0; index < size; index++)
	{
		crc ^= data[index];
		for (bit = 0; bit < 8; bit++)
			crc = (crc >> 1) ^ (0xEDB88320u & (0u - (crc & 1u)));
	}
	return ~crc;
}

/* The file of the set at 'folder' by either of its names, whole and with its CRC; NULL otherwise */
static uint8_t *read_file(const char *folder, const rom_file *file)
{
	const char *names[2] = { file->name, file->older };
	char path[1024];
	uint8_t *data = malloc(file->size);
	int name;

	for (name = 0; name < 2 && data && names[name]; name++)
	{
		FILE *input;
		size_t read;

		snprintf(path, sizeof(path), "%s/%s", folder, names[name]);
		if (!(input = fopen(path, "rb")))
			continue;
		read = fread(data, 1, file->size, input);
		fclose(input);
		if (read == file->size && crc32_of(data, file->size) == file->crc)
			return data;
	}
	free(data);
	return NULL;
}

/* ROM_LOAD16_BYTE: even bytes from one chip, odd bytes from the other */
static void interleave16(uint8_t *out, const uint8_t *even, const uint8_t *odd, uint32_t size)
{
	uint32_t index;

	for (index = 0; index < size; index++)
	{
		out[index * 2u] = even[index];
		out[index * 2u + 1u] = odd[index];
	}
}

/* ROM_LOAD64_WORD: four chips' 16-bit words spread over 8-byte groups, from 'base' */
static void sprite_lanes(uint32_t base, uint8_t *const *lanes, uint32_t size)
{
	uint32_t lane, word;

	for (lane = 0; lane < 4u; lane++)
		for (word = 0; word < size / 2u; word++)
		{
			game_sprites[base + word * 8u + lane * 2u] = lanes[lane][word * 2u];
			game_sprites[base + word * 8u + lane * 2u + 1u] = lanes[lane][word * 2u + 1u];
		}
}

const char *reference_load_rom_set(const char *folder)
{
	uint8_t *chips[FILES];
	const char *missing = NULL;
	int file, copy;

	for (file = 0; file < FILES; file++)
		if (!(chips[file] = read_file(folder, &files[file])) && !missing)
			missing = files[file].name;
	if (!missing)
	{
		for (copy = 0; copy < 4; copy++)
		{
			memcpy(game_program + copy * 0x20000u, chips[IC17], 0x20000u);
			memcpy(game_program + 0x080000u + copy * 0x20000u, chips[IC8], 0x20000u);
			memcpy(game_sound + copy * 0x40000u, chips[IC36], 0x40000u);
		}
		interleave16(game_program + 0x100000u, chips[IC18], chips[IC9], 0x40000u);
		memcpy(game_program + 0x180000u, game_program + 0x100000u, 0x80000u);
		interleave16(game_tiles, chips[IC14], chips[IC5], 0x100000u);
		memset(game_sprites, 0xFF, GAME_SPRITES_SIZE);
		sprite_lanes(0x000000u, &chips[IC32], 0x200000u);
		sprite_lanes(0x800000u, &chips[IC31], 0x100000u);
		memcpy(game_sound + 0x100000u, chips[IC35], 0x100000u);
		memcpy(game_sound + 0x200000u, chips[IC34], 0x100000u);
		memcpy(game_sound + 0x300000u, chips[IC33], 0x100000u);
	}
	for (file = 0; file < FILES; file++)
		free(chips[file]);
	return missing;
}
