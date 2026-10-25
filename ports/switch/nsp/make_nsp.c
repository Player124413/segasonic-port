/**
 *    Name:        make_nsp.c
 *    Description: SegaSonic NSP's work: the Switch game as an .nsp to install, made from the game's Switch
 *                 build, the player's ROM set and the player's console keys (for the command line and the
 *                 window alike)
 *    Version:     0.1.0
 *    Created:     25-10-2026
 *    Modified:    25-10-2026
 *    Author:      Sebastian Toyos
 *
 *    license:BSD-3-Clause
 *    As the computer's first run does, it takes the game's assets out of the player's sonic.zip
 *    (runtime/src/extract.c), and puts them in the Switch game's RomFS: the Switch reads them there and makes
 *    nothing. The .nsp holds three NCAs: the program (its ExeFS: main, main.npdm; its RomFS: segasonic.assets),
 *    its control data (control.nacp and the icon, a copy for each language) and the content meta that lists
 *    them, ciphered with the keys of the player's console (prod.keys: header_key and
 *    key_area_key_application_00). Nothing it reads or writes leaves the player's computer. The NCAs are
 *    not signed by Nintendo: the console installs them with its signature patches, as any homebrew .nsp.
 *    The command line (main.c) and the window (window.c) make it here alike, so the same inputs make the
 *    same .nsp from either. What it holds as it goes is freed however it ends (the window makes one .nsp
 *    after another), and the keys' bytes are wiped first.
 */
#include "make_nsp.h"
#include "crypto.h"
#include "formats.h"

#include "game/extract.h"

#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>

#define ASSETS_FILE  "segasonic.assets"
#define NACP_SIZE    0x4000u
#define LANGUAGES    16

static const char *const icon_names[LANGUAGES] = {
	"icon_AmericanEnglish.dat", "icon_BritishEnglish.dat", "icon_Japanese.dat", "icon_French.dat",
	"icon_German.dat", "icon_LatinAmericanSpanish.dat", "icon_Spanish.dat", "icon_Italian.dat", "icon_Dutch.dat",
	"icon_CanadianFrench.dat", "icon_Portuguese.dat", "icon_Russian.dat", "icon_Korean.dat",
	"icon_TraditionalChinese.dat", "icon_SimplifiedChinese.dat", "icon_BrazilianPortuguese.dat",
};

/* What making the .nsp holds as it goes: all of it freed at the end */
typedef struct
{
	buffer keys_file, nso, npdm, nacp, icon, assets, exefs, romfs, control_romfs, cnmt, meta_pfs0, program_nca,
		control_nca, meta_nca, nsp;
	nca_section sections[2];
	nca_keys keys;
} nsp_work;

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

/* A buffer that held the keys: its bytes wiped, then freed */
static void forget(buffer *data)
{
	if (data->data)
		memset(data->data, 0, data->size);
	buffer_free(data);
}

static int exists(const char *path)
{
	FILE *probe = fopen(path, "rb");

	if (probe)
		fclose(probe);
	return probe != NULL;
}

void nsp_folder_of(const char *path, char *folder, size_t size)
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

/* ---- the inputs ---- */

void nsp_take_dropped(nsp_inputs *inputs, const char *path)
{
	const char *dot = strrchr(path, '.');
	char *target;

	if (dot && (strcmp(dot, ".keys") == 0 || strcmp(dot, ".txt") == 0))
		target = inputs->keys;
	else if (dot && (strcmp(dot, ".jpg") == 0 || strcmp(dot, ".jpeg") == 0 || strcmp(dot, ".JPG") == 0))
		target = inputs->icon;
	else
		target = inputs->rom;    /* (sonic.zip, or a folder of its files) */
	snprintf(target, NSP_PATH_SIZE, "%s", path);
}

void nsp_defaults(nsp_inputs *inputs, const char *here)
{
	if (!inputs->keys[0])
	{
		const char *home = getenv("USERPROFILE") ? getenv("USERPROFILE") : getenv("HOME");

		snprintf(inputs->keys, sizeof(inputs->keys), "%sprod.keys", here);
		if (!exists(inputs->keys) && home)
			snprintf(inputs->keys, sizeof(inputs->keys), "%s/.switch/prod.keys", home);
	}
	if (!inputs->rom[0])
		snprintf(inputs->rom, sizeof(inputs->rom), "%ssonic.zip", here);
	if (!inputs->pieces[0])
		snprintf(inputs->pieces, sizeof(inputs->pieces), "%sswitch", here);
	if (!inputs->out[0])
		snprintf(inputs->out, sizeof(inputs->out), "%s" NSP_FILE, here);
	if (!inputs->icon[0])
	{
		snprintf(inputs->icon, sizeof(inputs->icon), "%s" NSP_OWN_ICON, here);
		if (!exists(inputs->icon))
			inputs->icon[0] = '\0';    /* (the build's own) */
	}
}

const char *nsp_keys_problem(const char *path)
{
	buffer file;
	uint8_t key[32];
	const char *problem = NULL;

	if (!read_file(path, &file))
		return exists(path) ? "cannot be read" : "not found";
	if (!find_key(&file, "header_key", key, 32))
		problem = "no header_key in them";
	else if (!find_key(&file, "key_area_key_application_00", key, 16))
		problem = "no key_area_key_application_00 in them";
	memset(key, 0, sizeof(key));
	forget(&file);
	return problem;
}

const char *nsp_icon_problem(const char *path)
{
	buffer jpeg;
	int square;

	if (!read_file(path, &jpeg))
		return exists(path) ? "cannot be read" : "not found";
	square = jpeg_256(&jpeg);
	buffer_free(&jpeg);
	return square ? NULL : "not a 256 x 256 JPEG";
}

int nsp_file_exists(const char *path)
{
	return exists(path);
}

int nsp_folder_exists(const char *path)
{
	char folder[NSP_PATH_SIZE];
	struct stat status;
	size_t length;

	snprintf(folder, sizeof(folder), "%s", path[0] ? path : ".");
	length = strlen(folder);
	/* (without its last separator, which Windows' stat does not take but for a drive's root) */
	if (length > 1 && (folder[length - 1] == '/' || folder[length - 1] == '\\') && folder[length - 2] != ':')
		folder[length - 1] = '\0';
	return stat(folder, &status) == 0 && (status.st_mode & S_IFMT) == S_IFDIR;
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

/* What failed, and on what (NULL for nothing): false, for the caller to return */
static int failed(nsp_outcome *outcome, const char *what, const char *detail)
{
	snprintf(outcome->what, sizeof(outcome->what), "%s", what);
	snprintf(outcome->detail, sizeof(outcome->detail), "%s", detail ? detail : "");
	return 0;
}

static void tell(nsp_step step, void *context, const char *name)
{
	if (step)
		step(context, name);
}

/* The steps, each holding what it makes in 'work' (make_nsp frees it all) */
static int build(const nsp_inputs *inputs, FILE *log, nsp_step step, void *context, nsp_work *work,
	nsp_outcome *outcome)
{
	char path[1200], pack_path[1300], program_name[48], control_name[48], meta_name[48], cnmt_name[64];
	named_file exefs_files[2], romfs_files[1], control_files[1 + LANGUAGES], meta_files[1], nsp_files[3];
	uint8_t program_hash[32], control_hash[32], meta_hash[32];
	uint64_t title_id = 0;
	const char *problem;

	if ((problem = crypto_self_test()) != NULL)
		return failed(outcome, "The tool's own check failed", problem);

	/* the keys */
	tell(step, context, "reading the console's keys");
	if (!read_file(inputs->keys, &work->keys_file))
		return failed(outcome, "The console's keys cannot be read", inputs->keys);
	if (!find_key(&work->keys_file, "header_key", work->keys.header_key, 32))
		return failed(outcome, "The console's keys have no header_key", inputs->keys);
	if (!find_key(&work->keys_file, "key_area_key_application_00", work->keys.key_area_key, 16))
		return failed(outcome, "The console's keys have no key_area_key_application_00", inputs->keys);
	forget(&work->keys_file);
	if (log)
		fprintf(log, "keys      %s\n", inputs->keys);

	/* the game's Switch pieces */
	tell(step, context, "reading the game's Switch build");
	snprintf(path, sizeof(path), "%s/main", inputs->pieces);
	if (!read_file(path, &work->nso) || work->nso.size < 4 || memcmp(work->nso.data, "NSO0", 4) != 0)
		return failed(outcome, "The game's Switch program cannot be read", path);
	snprintf(path, sizeof(path), "%s/main.npdm", inputs->pieces);
	if (!read_file(path, &work->npdm) || !npdm_title_id(&work->npdm, &title_id))
		return failed(outcome, "The game's Switch permissions cannot be read", path);
	snprintf(path, sizeof(path), "%s/control.nacp", inputs->pieces);
	if (!read_file(path, &work->nacp) || work->nacp.size != NACP_SIZE)
		return failed(outcome, "The game's Switch control data cannot be read", path);
	if (inputs->icon[0])
		snprintf(path, sizeof(path), "%s", inputs->icon);
	else
		snprintf(path, sizeof(path), "%s/icon.jpg", inputs->pieces);
	if (!read_file(path, &work->icon))
		return failed(outcome, "The icon cannot be read", path);
	if (!jpeg_256(&work->icon))
		return failed(outcome, "The icon is not a 256 x 256 JPEG", path);
	if (log && inputs->icon[0])
		fprintf(log, "icon      %s\n", inputs->icon);
	if (log)
		fprintf(log, "game      %s (title %016llX)\n", inputs->pieces, (unsigned long long)title_id);

	/* the assets, as the computer's first run takes them out */
	tell(step, context, "taking the assets out of the ROM set");
	snprintf(pack_path, sizeof(pack_path), "%s.assets", inputs->out);
	if (log)
		fprintf(log, "ROM set   %s\n", inputs->rom);
	if ((problem = game_data_extract(inputs->rom, pack_path)) != NULL)
		return failed(outcome, "The ROM set cannot be used", problem);
	if (!read_file(pack_path, &work->assets))
		return failed(outcome, "The assets cannot be read back", pack_path);
	remove(pack_path);

	/* the program: ExeFS (main, main.npdm) and RomFS (the assets) */
	tell(step, context, "making the program");
	exefs_files[0] = (named_file){ "main", work->nso.data, work->nso.size };
	exefs_files[1] = (named_file){ "main.npdm", work->npdm.data, work->npdm.size };
	romfs_files[0] = (named_file){ ASSETS_FILE, work->assets.data, work->assets.size };
	if (!pfs0_build(exefs_files, 2, &work->exefs) || !section_from_pfs0(&work->exefs, 0x10000, &work->sections[0])
		|| !romfs_build(romfs_files, 1, &work->romfs) || !section_from_romfs(&work->romfs, &work->sections[1])
		|| !nca_build(&work->keys, NCA_PROGRAM, title_id, work->sections, 2, &work->program_nca, program_hash))
		return failed(outcome, "There is not memory enough for the program", NULL);
	buffer_free(&work->exefs);
	buffer_free(&work->romfs);
	buffer_free(&work->assets);
	buffer_free(&work->nso);
	buffer_free(&work->npdm);

	/* the control data: control.nacp and the icon for each language */
	tell(step, context, "making the control data");
	nacp_for_install(work->nacp.data);
	control_files[0] = (named_file){ "control.nacp", work->nacp.data, work->nacp.size };
	for (int language = 0; language < LANGUAGES; language++)
		control_files[1 + language] = (named_file){ icon_names[language], work->icon.data, work->icon.size };
	if (!romfs_build(control_files, 1 + LANGUAGES, &work->control_romfs)
		|| !section_from_romfs(&work->control_romfs, &work->sections[0])
		|| !nca_build(&work->keys, NCA_CONTROL, title_id, work->sections, 1, &work->control_nca, control_hash))
		return failed(outcome, "There is not memory enough for the control data", NULL);
	buffer_free(&work->control_romfs);
	buffer_free(&work->nacp);
	buffer_free(&work->icon);

	/* the content meta, which lists them */
	tell(step, context, "making the content meta");
	snprintf(cnmt_name, sizeof(cnmt_name), "Application_%016llx.cnmt", (unsigned long long)title_id);
	meta_files[0].name = cnmt_name;
	if (!cnmt_build(title_id, program_hash, work->program_nca.size, control_hash, work->control_nca.size,
		&work->cnmt))
		return failed(outcome, "There is not memory enough for the content meta", NULL);
	meta_files[0].data = work->cnmt.data;
	meta_files[0].size = work->cnmt.size;
	if (!pfs0_build(meta_files, 1, &work->meta_pfs0) || !section_from_pfs0(&work->meta_pfs0, 0x1000, &work->sections[0])
		|| !nca_build(&work->keys, NCA_META, title_id, work->sections, 1, &work->meta_nca, meta_hash))
		return failed(outcome, "There is not memory enough for the content meta", NULL);
	buffer_free(&work->meta_pfs0);
	buffer_free(&work->cnmt);

	/* the .nsp: the three, each named by its hash */
	tell(step, context, "writing the .nsp");
	content_id(program_hash, program_name, ".nca");
	content_id(control_hash, control_name, ".nca");
	content_id(meta_hash, meta_name, ".cnmt.nca");
	nsp_files[0] = (named_file){ program_name, work->program_nca.data, work->program_nca.size };
	nsp_files[1] = (named_file){ control_name, work->control_nca.data, work->control_nca.size };
	nsp_files[2] = (named_file){ meta_name, work->meta_nca.data, work->meta_nca.size };
	if (!pfs0_build(nsp_files, 3, &work->nsp))
		return failed(outcome, "There is not memory enough for the .nsp", NULL);
	if (!write_file(inputs->out, &work->nsp))
		return failed(outcome, "The .nsp cannot be written", inputs->out);
	outcome->size = work->nsp.size;
	if (log)
	{
		fprintf(log, "\nwritten   %s (%.1f MB)\n", inputs->out, (double)work->nsp.size / (1024.0 * 1024.0));
		fprintf(log, "          %s  program\n          %s  control data\n          %s  content meta\n", program_name,
			control_name, meta_name);
	}
	return 1;
}

int make_nsp(const nsp_inputs *inputs, FILE *log, nsp_step step, void *context, nsp_outcome *outcome)
{
	nsp_work work;
	buffer *const held[] = {
		&work.nso, &work.npdm, &work.nacp, &work.icon, &work.assets, &work.exefs, &work.romfs, &work.control_romfs,
		&work.cnmt, &work.meta_pfs0, &work.program_nca, &work.control_nca, &work.meta_nca, &work.nsp,
		&work.sections[0].data, &work.sections[1].data,
	};
	int made;

	memset(&work, 0, sizeof(work));
	memset(outcome, 0, sizeof(*outcome));
	made = build(inputs, log, step, context, &work, outcome);
	forget(&work.keys_file);
	memset(&work.keys, 0, sizeof(work.keys));
	for (size_t at = 0; at < sizeof(held) / sizeof(held[0]); at++)
		buffer_free(held[at]);
	return made;
}
