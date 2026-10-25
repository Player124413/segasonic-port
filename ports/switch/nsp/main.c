/**
 *    Name:        main.c
 *    Description: SegaSonic NSP: the Switch game as an .nsp to install, made on the player's computer from the
 *                 game's Switch build, the player's ROM set and the player's console keys
 *    Version:     0.1.0
 *    Created:     23-10-2026
 *    Modified:    23-10-2026
 *    Author:      Sebastian Toyos
 *
 *    license:BSD-3-Clause
 *    Usage: SegaSonic NSP [--keys FILE] [--rom FILE] [--switch FOLDER] [--out FILE] [--icon FILE]
 *
 *    As the computer's first run does, it takes the game's assets out of the player's sonic.zip
 *    (runtime/src/extract.c), and puts them in the Switch game's RomFS: the Switch reads them there and makes
 *    nothing. The .nsp holds three NCAs: the program (its ExeFS: main, main.npdm; its RomFS: segasonic.assets),
 *    its control data (control.nacp and the icon, a copy for each language) and the content meta that lists
 *    them, ciphered with the keys of the player's console (prod.keys: header_key and
 *    key_area_key_application_00). Nothing it reads or writes leaves the player's computer. The NCAs are
 *    not signed by Nintendo: the console installs them with its signature patches, as any homebrew .nsp.
 *    By default every file is beside the tool (prod.keys also in .switch in the user's folder), the game's
 *    Switch pieces in the folder switch beside it, and the .nsp is written beside it. Files dropped on the
 *    tool are taken by what they are: a .keys (or .txt) the keys, a .jpg the icon, anything else the ROM set
 *    (sonic.zip, or a folder of its files); started so, or with nothing, its window waits for Enter at the end.
 */
#include "crypto.h"
#include "formats.h"

#include "game/extract.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define ASSETS_FILE  "segasonic.assets"
#define NACP_SIZE    0x4000u
#define LANGUAGES    16

static const char *const icon_names[LANGUAGES] = {
	"icon_AmericanEnglish.dat", "icon_BritishEnglish.dat", "icon_Japanese.dat", "icon_French.dat",
	"icon_German.dat", "icon_LatinAmericanSpanish.dat", "icon_Spanish.dat", "icon_Italian.dat", "icon_Dutch.dat",
	"icon_CanadianFrench.dat", "icon_Portuguese.dat", "icon_Russian.dat", "icon_Korean.dat",
	"icon_TraditionalChinese.dat", "icon_SimplifiedChinese.dat", "icon_BrazilianPortuguese.dat",
};

static int waits;    /* started without arguments (a double click): the window waits for Enter at the end */

static int finish(int code)
{
	if (waits)
	{
		printf("\nPress Enter to close.\n");
		(void)getchar();
	}
	return code;
}

static int failed(const char *what, const char *detail)
{
	fprintf(stderr, "%s%s%s\n", what, detail ? ": " : "", detail ? detail : "");
	return finish(1);
}

static int read_file(const char *path, buffer *out)
{
	FILE *input = fopen(path, "rb");
	long length;

	out->data = NULL;
	out->size = 0;
	if (!input)
		return 0;
	fseek(input, 0, SEEK_END);
	length = ftell(input);
	fseek(input, 0, SEEK_SET);
	out->data = length > 0 ? (uint8_t *)malloc((size_t)length) : NULL;
	if (out->data && fread(out->data, 1, (size_t)length, input) == (size_t)length)
		out->size = (size_t)length;
	else
	{
		free(out->data);
		out->data = NULL;
	}
	fclose(input);
	return out->data != NULL;
}

static int exists(const char *path)
{
	FILE *probe = fopen(path, "rb");

	if (probe)
		fclose(probe);
	return probe != NULL;
}

/* The folder of a path, with its separator: "" for a bare name */
static void folder_of(const char *path, char *folder, size_t size)
{
	const char *slash = strrchr(path, '/'), *backslash = strrchr(path, '\\');
	const char *end = backslash > slash ? backslash : slash;
	const size_t length = end ? (size_t)(end - path) + 1 : 0;

	snprintf(folder, size, "%.*s", (int)(length < size ? length : size - 1), path);
}

/* ---- prod.keys: "name = hex" lines ---- */

static int hex_value(char c)
{
	if (c >= '0' && c <= '9')
		return c - '0';
	if ((c | 0x20) >= 'a' && (c | 0x20) <= 'f')
		return (c | 0x20) - 'a' + 10;
	return -1;
}

/* The key 'name' of 'size' bytes: true when the file has it whole */
static int find_key(const buffer *file, const char *name, uint8_t *key, size_t size)
{
	const size_t length = strlen(name);
	const char *text = (const char *)file->data, *end = text + file->size;

	for (const char *line = text; line < end;)
	{
		const char *next = memchr(line, '\n', (size_t)(end - line));
		const char *stop = next ? next : end, *at = line;

		while (at < stop && (*at == ' ' || *at == '\t'))
			at++;
		if ((size_t)(stop - at) > length && strncmp(at, name, length) == 0
			&& (at[length] == ' ' || at[length] == '\t' || at[length] == '='))
		{
			size_t got = 0;

			at += length;
			while (at < stop && (*at == ' ' || *at == '\t' || *at == '='))
				at++;
			while (got < size && at + 1 < stop && hex_value(at[0]) >= 0 && hex_value(at[1]) >= 0)
			{
				key[got++] = (uint8_t)(hex_value(at[0]) * 16 + hex_value(at[1]));
				at += 2;
			}
			return got == size;
		}
		line = next ? next + 1 : end;
	}
	return 0;
}

/* True for a JPEG of 256 x 256, the HOME menu's icon: its frame header (SOF0-SOF2) gives its size */
static int jpeg_256(const buffer *jpeg)
{
	size_t at = 2;

	if (jpeg->size < 4 || jpeg->data[0] != 0xFF || jpeg->data[1] != 0xD8)
		return 0;
	while (at + 9 < jpeg->size && jpeg->data[at] == 0xFF)
	{
		const uint8_t marker = jpeg->data[at + 1];
		const size_t length = (size_t)jpeg->data[at + 2] << 8 | jpeg->data[at + 3];

		if (marker >= 0xC0 && marker <= 0xC2)
			return ((jpeg->data[at + 5] << 8 | jpeg->data[at + 6]) == 256)
				&& ((jpeg->data[at + 7] << 8 | jpeg->data[at + 8]) == 256);
		at += 2 + length;
	}
	return 0;
}

/* ---- the .nsp ---- */

/* The program's id in main.npdm: its ACI0's */
static int npdm_title_id(const buffer *npdm, uint64_t *title_id)
{
	uint32_t aci0;

	if (npdm->size < 0x80 || memcmp(npdm->data, "META", 4) != 0)
		return 0;
	aci0 = (uint32_t)npdm->data[0x70] | (uint32_t)npdm->data[0x71] << 8 | (uint32_t)npdm->data[0x72] << 16
		| (uint32_t)npdm->data[0x73] << 24;
	if (aci0 + 0x18u > npdm->size || memcmp(npdm->data + aci0, "ACI0", 4) != 0)
		return 0;
	*title_id = 0;
	for (int byte = 7; byte >= 0; byte--)
		*title_id = *title_id << 8 | npdm->data[aci0 + 0x10 + byte];
	return 1;
}

/* control.nacp as an installed game takes it: no user to pick when it starts, no save data of the
   system's (it keeps its files on the SD card), the system's own logo handling (it has no logo section) */
static void nacp_for_install(uint8_t *nacp)
{
	nacp[0x3025] = 0;                /* StartupUserAccount: none */
	memset(nacp + 0x3080, 0, 16);    /* UserAccountSaveDataSize and its journal's */
	nacp[0x30F1] = 0;                /* LogoHandling: auto */
}

static void content_id(const uint8_t hash[32], char *name, const char *suffix)
{
	static const char digits[] = "0123456789abcdef";

	for (int byte = 0; byte < 16; byte++)
	{
		name[byte * 2] = digits[hash[byte] >> 4];
		name[byte * 2 + 1] = digits[hash[byte] & 15];
	}
	strcpy(name + 32, suffix);
}

static int write_file(const char *path, const buffer *data)
{
	FILE *output = fopen(path, "wb");
	int written;

	if (!output)
		return 0;
	written = fwrite(data->data, 1, data->size, output) == data->size;
	return fclose(output) == 0 && written;
}

int main(int argc, char **argv)
{
	char here[1024], keys_path[1100] = "", rom_path[1100] = "", pieces[1100] = "", out_path[1100] = "",
		icon_path[1100] = "", path[1200], pack_path[1300], program_name[48], control_name[48], meta_name[48], cnmt_name[64];
	buffer keys_file, nso, npdm, nacp, icon, assets, exefs, romfs, control_romfs, cnmt, meta_pfs0,
		program_nca, control_nca, meta_nca, nsp;
	named_file exefs_files[2], romfs_files[1], control_files[1 + LANGUAGES], meta_files[1], nsp_files[3];
	nca_section sections[2];
	uint8_t program_hash[32], control_hash[32], meta_hash[32];
	nca_keys keys;
	uint64_t title_id = 0;
	const char *problem;

	waits = 1;
	folder_of(argv[0], here, sizeof(here));
	for (int at = 1; at < argc; at++)
	{
		char *target = NULL;

		if (strncmp(argv[at], "--", 2) == 0)
			waits = 0;    /* (a command line: the window is someone's console) */
		else
		{
			/* files dropped on the tool: each by what it is */
			const char *dot = strrchr(argv[at], '.');

			if (dot && (strcmp(dot, ".keys") == 0 || strcmp(dot, ".txt") == 0))
				target = keys_path;
			else if (dot && (strcmp(dot, ".jpg") == 0 || strcmp(dot, ".jpeg") == 0 || strcmp(dot, ".JPG") == 0))
				target = icon_path;
			else
				target = rom_path;    /* (sonic.zip, or a folder of its files) */
			snprintf(target, 1100, "%s", argv[at]);
			continue;
		}
		if (strcmp(argv[at], "--keys") == 0)
			target = keys_path;
		else if (strcmp(argv[at], "--rom") == 0)
			target = rom_path;
		else if (strcmp(argv[at], "--switch") == 0)
			target = pieces;
		else if (strcmp(argv[at], "--out") == 0)
			target = out_path;
		else if (strcmp(argv[at], "--icon") == 0)
			target = icon_path;
		if (!target || at + 1 >= argc)
		{
			fprintf(stderr, "Usage: SegaSonic NSP [--keys FILE] [--rom FILE] [--switch FOLDER] [--out FILE] [--icon FILE]\n"
				"  --keys    the console's keys (prod.keys; by default beside it, or in .switch in the user's folder)\n"
				"  --rom     the arcade ROM set (sonic.zip, MAME's, Japan, rev. C; by default beside it)\n"
				"  --switch  the game's Switch build: main, main.npdm, control.nacp, icon.jpg (by default switch beside it)\n"
				"  --out     the .nsp to write (by default SegaSonic.nsp beside it)\n"
				"  --icon    the HOME menu's icon, a 256 x 256 JPEG of yours (by default the build's icon.jpg)\n");
			return 2;
		}
		snprintf(target, 1100, "%s", argv[++at]);
	}
	printf("SegaSonic NSP: the Switch game, with the assets of your ROM set, ciphered with your console's keys\n\n");
	if ((problem = crypto_self_test()) != NULL)
		return failed("The tool's own check failed", problem);
	if (!keys_path[0])
	{
		const char *home = getenv("USERPROFILE") ? getenv("USERPROFILE") : getenv("HOME");

		snprintf(keys_path, sizeof(keys_path), "%sprod.keys", here);
		if (!exists(keys_path) && home)
			snprintf(keys_path, sizeof(keys_path), "%s/.switch/prod.keys", home);
	}
	if (!rom_path[0])
		snprintf(rom_path, sizeof(rom_path), "%ssonic.zip", here);
	if (!pieces[0])
		snprintf(pieces, sizeof(pieces), "%sswitch", here);
	if (!out_path[0])
		snprintf(out_path, sizeof(out_path), "%sSegaSonic.nsp", here);

	/* the keys */
	if (!read_file(keys_path, &keys_file))
		return failed("The console's keys cannot be read", keys_path);
	if (!find_key(&keys_file, "header_key", keys.header_key, 32))
		return failed("The console's keys have no header_key", keys_path);
	if (!find_key(&keys_file, "key_area_key_application_00", keys.key_area_key, 16))
		return failed("The console's keys have no key_area_key_application_00", keys_path);
	buffer_free(&keys_file);
	printf("keys      %s\n", keys_path);

	/* the game's Switch pieces */
	snprintf(path, sizeof(path), "%s/main", pieces);
	if (!read_file(path, &nso) || nso.size < 4 || memcmp(nso.data, "NSO0", 4) != 0)
		return failed("The game's Switch program cannot be read", path);
	snprintf(path, sizeof(path), "%s/main.npdm", pieces);
	if (!read_file(path, &npdm) || !npdm_title_id(&npdm, &title_id))
		return failed("The game's Switch permissions cannot be read", path);
	snprintf(path, sizeof(path), "%s/control.nacp", pieces);
	if (!read_file(path, &nacp) || nacp.size != NACP_SIZE)
		return failed("The game's Switch control data cannot be read", path);
	if (icon_path[0])
		snprintf(path, sizeof(path), "%s", icon_path);
	else
		snprintf(path, sizeof(path), "%s/icon.jpg", pieces);
	if (!read_file(path, &icon))
		return failed("The icon cannot be read", path);
	if (!jpeg_256(&icon))
		return failed("The icon is not a 256 x 256 JPEG", path);
	if (icon_path[0])
		printf("icon      %s\n", icon_path);
	printf("game      %s (title %016llX)\n", pieces, (unsigned long long)title_id);

	/* the assets, as the computer's first run takes them out */
	snprintf(pack_path, sizeof(pack_path), "%s.assets", out_path);
	printf("ROM set   %s\n", rom_path);
	if ((problem = game_data_extract(rom_path, pack_path)) != NULL)
		return failed("The ROM set cannot be used", problem);
	if (!read_file(pack_path, &assets))
		return failed("The assets cannot be read back", pack_path);
	remove(pack_path);

	/* the program: ExeFS (main, main.npdm) and RomFS (the assets) */
	exefs_files[0] = (named_file){ "main", nso.data, nso.size };
	exefs_files[1] = (named_file){ "main.npdm", npdm.data, npdm.size };
	romfs_files[0] = (named_file){ ASSETS_FILE, assets.data, assets.size };
	if (!pfs0_build(exefs_files, 2, &exefs) || !section_from_pfs0(&exefs, 0x10000, &sections[0])
		|| !romfs_build(romfs_files, 1, &romfs) || !section_from_romfs(&romfs, &sections[1])
		|| !nca_build(&keys, NCA_PROGRAM, title_id, sections, 2, &program_nca, program_hash))
		return failed("There is not memory enough for the program", NULL);
	buffer_free(&exefs);
	buffer_free(&romfs);
	buffer_free(&assets);
	buffer_free(&nso);
	buffer_free(&npdm);

	/* the control data: control.nacp and the icon for each language */
	nacp_for_install(nacp.data);
	control_files[0] = (named_file){ "control.nacp", nacp.data, nacp.size };
	for (int language = 0; language < LANGUAGES; language++)
		control_files[1 + language] = (named_file){ icon_names[language], icon.data, icon.size };
	if (!romfs_build(control_files, 1 + LANGUAGES, &control_romfs) || !section_from_romfs(&control_romfs, &sections[0])
		|| !nca_build(&keys, NCA_CONTROL, title_id, sections, 1, &control_nca, control_hash))
		return failed("There is not memory enough for the control data", NULL);
	buffer_free(&control_romfs);
	buffer_free(&nacp);
	buffer_free(&icon);

	/* the content meta, which lists them */
	snprintf(cnmt_name, sizeof(cnmt_name), "Application_%016llx.cnmt", (unsigned long long)title_id);
	meta_files[0].name = cnmt_name;
	if (!cnmt_build(title_id, program_hash, program_nca.size, control_hash, control_nca.size, &cnmt))
		return failed("There is not memory enough for the content meta", NULL);
	meta_files[0].data = cnmt.data;
	meta_files[0].size = cnmt.size;
	if (!pfs0_build(meta_files, 1, &meta_pfs0) || !section_from_pfs0(&meta_pfs0, 0x1000, &sections[0])
		|| !nca_build(&keys, NCA_META, title_id, sections, 1, &meta_nca, meta_hash))
		return failed("There is not memory enough for the content meta", NULL);
	buffer_free(&meta_pfs0);
	buffer_free(&cnmt);

	/* the .nsp: the three, each named by its hash */
	content_id(program_hash, program_name, ".nca");
	content_id(control_hash, control_name, ".nca");
	content_id(meta_hash, meta_name, ".cnmt.nca");
	nsp_files[0] = (named_file){ program_name, program_nca.data, program_nca.size };
	nsp_files[1] = (named_file){ control_name, control_nca.data, control_nca.size };
	nsp_files[2] = (named_file){ meta_name, meta_nca.data, meta_nca.size };
	if (!pfs0_build(nsp_files, 3, &nsp))
		return failed("There is not memory enough for the .nsp", NULL);
	if (!write_file(out_path, &nsp))
		return failed("The .nsp cannot be written", out_path);
	printf("\nwritten   %s (%.1f MB)\n", out_path, (double)nsp.size / (1024.0 * 1024.0));
	printf("          %s  program\n          %s  control data\n          %s  content meta\n", program_name,
		control_name, meta_name);
	buffer_free(&nsp);
	buffer_free(&program_nca);
	buffer_free(&control_nca);
	buffer_free(&meta_nca);
	return finish(0);
}
