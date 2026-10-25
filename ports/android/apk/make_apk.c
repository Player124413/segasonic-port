/**
 *    Name:        make_apk.c
 *    Description: SegaSonic APK's work: the Android game as an APK to install, made from the game's Android
 *                 build and the player's ROM set, signed with the player's own key (for the command line
 *                 and the window alike)
 *    Version:     0.1.0
 *    Created:     25-10-2026
 *    Modified:    25-10-2026
 *    Author:      Sebastian Toyos
 *
 *    license:BSD-3-Clause
 *    As the computer's first run does, it takes the game's assets out of the player's sonic.zip
 *    (runtime/src/extract.c), and puts them in the APK as assets/segasonic.assets, stored: the game reads
 *    them in place through Android's asset manager and makes nothing. The game's Android build,
 *    segasonic-base.apk (ports/android/android.cmake), is the APK without them and without a signature; any
 *    signature it has is left out all the same (a v1 signature's META-INF files, an APK Signing Block), and
 *    an assets pack it has is replaced. An icon of the player's own goes in the build's own icon entries:
 *    the launcher's adaptive icon (res/mipmap-anydpi-v26/ic_launcher.xml) takes the bytes of the one made
 *    for it (ic_launcher_own.xml, compiled binary XML, whose background is ic_launcher_art inset by 18 dp),
 *    and the pictures ic_launcher_art and ic_launcher (the legacy icon) the player's picture's (Android's
 *    decoders tell a PNG from a JPEG by its bytes, whatever the entry's name). The rest of the build's
 *    entries are copied packed as they are, aligned as zipalign -p 4 aligns them (zip.c), and the APK is
 *    signed with APK Signature Scheme v2 (sign.c) by the player's key (key.c), the same for every APK, so
 *    that each installs over the last. Nothing it reads or writes leaves the player's computer. The command
 *    line (main.c) and the window (window.c) make it here alike. What it holds as it goes is freed however
 *    it ends (the window makes one APK after another), and the key's bytes are wiped first.
 */
#include "make_apk.h"
#include "crypto.h"
#include "ecdsa.h"
#include "key.h"
#include "sign.h"
#include "zip.h"

#include "game/extract.h"

#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>

#define ASSETS_ENTRY    "assets/segasonic.assets"
#define ICONS_FOLDER    "res/mipmap-"
#define ICON_SHAPE      "ic_launcher.xml"        /* the adaptive icon */
#define ICON_OWN_SHAPE  "ic_launcher_own.xml"    /* the adaptive icon made for a picture of the player's */
#define ICON_LEGACY     "ic_launcher"            /* the pictures, by their names without their type */
#define ICON_ART        "ic_launcher_art"
#define ENTRY_DATE      0x0221                   /* 1 January 1981 in MS-DOS's date, aapt2's for every entry */

#ifdef _WIN32
#define SEPARATOR "\\"
#else
#define SEPARATOR "/"
#endif

typedef struct
{
	uint8_t *data;
	size_t size;
} bytes;

/* What making the APK holds as it goes: all of it freed at the end */
typedef struct
{
	bytes base, icon, assets;
	zip_archive archive;
	zip_writer apk;
	apk_key key;
	uint8_t *block;
} apk_work;

/* What an entry of the build is to the APK */
enum
{
	ENTRY_KEPT,
	ENTRY_SIGNATURE,    /* a v1 signature's: left out */
	ENTRY_ASSETS,       /* an assets pack: replaced */
	ENTRY_SHAPE,        /* the adaptive icon */
	ENTRY_ART,          /* the adaptive icon's picture */
	ENTRY_LEGACY        /* the legacy icon's picture */
};

static void bytes_free(bytes *data)
{
	free(data->data);
	data->data = NULL;
	data->size = 0;
}

static int read_file(const char *path, bytes *out)
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

void apk_folder_of(const char *path, char *folder, size_t size)
{
	const char *slash = strrchr(path, '/'), *backslash = strrchr(path, '\\');
	const char *end = backslash > slash ? backslash : slash;
	const size_t length = end ? (size_t)(end - path) + 1 : 0;

	snprintf(folder, size, "%.*s", (int)(length < size ? length : size - 1), path);
}

/* ---- the icon ---- */

/* A PNG's or a JPEG's size, from its header (PNG's IHDR, JPEG's frame header SOF0-SOF2): false for neither */
static int picture_size(const bytes *picture, unsigned *width, unsigned *height)
{
	static const uint8_t png[16] = { 0x89, 'P', 'N', 'G', 0x0D, 0x0A, 0x1A, 0x0A, 0, 0, 0, 13, 'I', 'H', 'D', 'R' };
	const uint8_t *const data = picture->data;
	size_t at = 2;

	if (picture->size >= 24 && memcmp(data, png, sizeof(png)) == 0)
	{
		*width = (unsigned)data[16] << 24 | (unsigned)data[17] << 16 | (unsigned)data[18] << 8 | data[19];
		*height = (unsigned)data[20] << 24 | (unsigned)data[21] << 16 | (unsigned)data[22] << 8 | data[23];
		return 1;
	}
	if (picture->size < 4 || data[0] != 0xFF || data[1] != 0xD8)
		return 0;
	while (at + 9 < picture->size && data[at] == 0xFF)
	{
		const uint8_t marker = data[at + 1];

		if (marker == 0xFF)
		{
			at++;    /* (a fill byte before a marker) */
			continue;
		}
		if (marker >= 0xC0 && marker <= 0xC2)
		{
			*height = (unsigned)data[at + 5] << 8 | data[at + 6];
			*width = (unsigned)data[at + 7] << 8 | data[at + 8];
			return 1;
		}
		at += 2 + ((size_t)data[at + 2] << 8 | data[at + 3]);
	}
	return 0;
}

static const char *picture_problem(const bytes *picture)
{
	unsigned width = 0, height = 0;

	if (!picture_size(picture, &width, &height) || !width || !height)
		return "not a PNG or JPEG";
	if (width != height)
		return "not square";
	if (width > APK_ICON_MOST)
		return "larger than 2048 x 2048";
	return NULL;
}

/* ---- the build's entries ---- */

/* An entry's name past its last '/' */
static const char *file_of(const zip_entry *entry, size_t *size)
{
	size_t start = entry->name_size;

	while (start > 0 && entry->name[start - 1] != '/')
		start--;
	*size = entry->name_size - start;
	return entry->name + start;
}

static int starts(const zip_entry *entry, const char *head)
{
	const size_t length = strlen(head);

	return entry->name_size >= length && memcmp(entry->name, head, length) == 0;
}

/* The file's name, letters' case aside, is 'name' (or ends with it, 'ending') */
static int named_alike(const char *file, size_t size, const char *name, int ending)
{
	const size_t length = strlen(name);

	if (size < length || (!ending && size != length))
		return 0;
	file += size - length;
	for (size_t at = 0; at < length; at++)
		if ((file[at] | 0x20) != (name[at] | 0x20))
			return 0;
	return 1;
}

/* A picture of the icon's: <stem>.<type>, the type not xml */
static int icon_picture(const char *file, size_t size, const char *stem)
{
	const size_t length = strlen(stem);

	return size > length + 1 && memcmp(file, stem, length) == 0 && file[length] == '.'
		&& memchr(file + length + 1, '.', size - length - 1) == NULL && !named_alike(file, size, ".xml", 1);
}

static int entry_kind(const zip_entry *entry)
{
	size_t size;
	const char *const file = file_of(entry, &size);

	/* (META-INF's own files, as the JAR signature's verifiers take them: in any case) */
	if (starts(entry, "META-INF/") && (size_t)(file - entry->name) == 9
		&& (named_alike(file, size, "MANIFEST.MF", 0) || named_alike(file, size, ".SF", 1)
			|| named_alike(file, size, ".RSA", 1) || named_alike(file, size, ".DSA", 1)
			|| named_alike(file, size, ".EC", 1)))
		return ENTRY_SIGNATURE;
	if (zip_named(entry, ASSETS_ENTRY))
		return ENTRY_ASSETS;
	if (!starts(entry, ICONS_FOLDER))
		return ENTRY_KEPT;
	if (size == strlen(ICON_SHAPE) && memcmp(file, ICON_SHAPE, size) == 0)
		return ENTRY_SHAPE;
	if (icon_picture(file, size, ICON_ART))
		return ENTRY_ART;
	if (icon_picture(file, size, ICON_LEGACY))
		return ENTRY_LEGACY;
	return ENTRY_KEPT;
}

/* The adaptive icon made for a picture of the player's, beside the adaptive icon 'shape' */
static const zip_entry *own_shape(const zip_archive *archive, const zip_entry *shape)
{
	char name[512];
	size_t size;
	const char *const file = file_of(shape, &size);
	const int folder = (int)(file - shape->name);

	snprintf(name, sizeof(name), "%.*s" ICON_OWN_SHAPE, folder, shape->name);
	return zip_find(archive, name);
}

/* The build has an adaptive icon (each with its own made for the player's picture), its picture and the
   legacy icon */
static int takes_icon(const zip_archive *archive)
{
	int shapes = 0, arts = 0, legacies = 0;

	for (size_t index = 0; index < archive->count; index++)
	{
		const zip_entry *const entry = &archive->entries[index];
		const int kind = entry_kind(entry);

		if (kind == ENTRY_SHAPE && !own_shape(archive, entry))
			return 0;
		shapes += kind == ENTRY_SHAPE;
		arts += kind == ENTRY_ART;
		legacies += kind == ENTRY_LEGACY;
	}
	return shapes && arts && legacies;
}

/* ---- the inputs ---- */

void apk_take_dropped(apk_inputs *inputs, const char *path)
{
	const char *const dot = strrchr(path, '.');
	const size_t size = dot ? strlen(dot) : 0;
	char *target;

	if (dot && named_alike(dot, size, ".apk", 0))
		target = inputs->base;
	else if (dot && (named_alike(dot, size, ".png", 0) || named_alike(dot, size, ".jpg", 0)
		|| named_alike(dot, size, ".jpeg", 0)))
		target = inputs->icon;
	else
		target = inputs->rom;    /* (sonic.zip, or a folder of its files) */
	snprintf(target, APK_PATH_SIZE, "%s", path);
}

void apk_defaults(apk_inputs *inputs, const char *here)
{
	if (!inputs->rom[0])
		snprintf(inputs->rom, sizeof(inputs->rom), "%ssonic.zip", here);
	if (!inputs->base[0])
		snprintf(inputs->base, sizeof(inputs->base), "%s" APK_BASE_FOLDER SEPARATOR APK_BASE_FILE, here);
	if (!inputs->out[0])
		snprintf(inputs->out, sizeof(inputs->out), "%s" APK_FILE, here);
}

const char *apk_icon_problem(const char *path)
{
	bytes picture;
	const char *problem;

	if (!read_file(path, &picture))
		return exists(path) ? "cannot be read" : "not found";
	problem = picture_problem(&picture);
	bytes_free(&picture);
	return problem;
}

const char *apk_base_problem(const char *path)
{
	bytes base;
	zip_archive archive;
	int apk;

	if (!read_file(path, &base))
		return exists(path) ? "cannot be read" : "not found";
	apk = zip_read(base.data, base.size, &archive) && zip_find(&archive, "AndroidManifest.xml");
	zip_free(&archive);
	bytes_free(&base);
	return apk ? NULL : "not an APK";
}

int apk_base_takes_icon(const char *path)
{
	bytes base;
	zip_archive archive;
	int takes;

	if (!read_file(path, &base))
		return 0;
	takes = zip_read(base.data, base.size, &archive) && takes_icon(&archive);
	zip_free(&archive);
	bytes_free(&base);
	return takes;
}

int apk_file_exists(const char *path)
{
	return exists(path);
}

int apk_folder_exists(const char *path)
{
	char folder[APK_PATH_SIZE];
	struct stat status;
	size_t length;

	snprintf(folder, sizeof(folder), "%s", path[0] ? path : ".");
	length = strlen(folder);
	/* (without its last separator, which Windows' stat does not take but for a drive's root) */
	if (length > 1 && (folder[length - 1] == '/' || folder[length - 1] == '\\') && folder[length - 2] != ':')
		folder[length - 1] = '\0';
	return stat(folder, &status) == 0 && (status.st_mode & S_IFMT) == S_IFDIR;
}

/* ---- the APK ---- */

/* What failed, and on what (NULL for nothing): false, for the caller to return */
static int failed(apk_outcome *outcome, const char *what, const char *detail)
{
	snprintf(outcome->what, sizeof(outcome->what), "%s", what);
	snprintf(outcome->detail, sizeof(outcome->detail), "%s", detail ? detail : "");
	return 0;
}

static void tell(apk_step step, void *context, const char *name)
{
	if (step)
		step(context, name);
}

/* The build's entries, but its signature's and an assets pack; with an icon of the player's, its
   entries; then the assets pack, stored */
static void put_entries(apk_work *work, int own_icon)
{
	zip_entry assets;

	for (size_t index = 0; index < work->archive.count; index++)
	{
		zip_entry entry = work->archive.entries[index];
		const int kind = entry_kind(&entry);

		if (kind == ENTRY_SIGNATURE || kind == ENTRY_ASSETS)
			continue;
		if (own_icon && (kind == ENTRY_ART || kind == ENTRY_LEGACY))
		{
			entry.flags &= ~0x0006u;    /* (deflate's own flags) */
			entry.method = ZIP_STORED;
			entry.data = work->icon.data;
			entry.packed = entry.unpacked = (uint32_t)work->icon.size;
			entry.crc = zip_crc32(work->icon.data, work->icon.size);
		}
		else if (own_icon && kind == ENTRY_SHAPE)
		{
			const zip_entry *const own = own_shape(&work->archive, &entry);

			entry.flags = own->flags;
			entry.method = own->method;
			entry.data = own->data;
			entry.packed = own->packed;
			entry.unpacked = own->unpacked;
			entry.crc = own->crc;
		}
		zip_add(&work->apk, &entry);
	}
	memset(&assets, 0, sizeof(assets));
	assets.name = ASSETS_ENTRY;
	assets.name_size = (uint16_t)strlen(ASSETS_ENTRY);
	assets.needed = 10;    /* (1.0: stored) */
	assets.method = ZIP_STORED;
	assets.date = ENTRY_DATE;
	assets.data = work->assets.data;
	assets.packed = assets.unpacked = (uint32_t)work->assets.size;
	assets.crc = zip_crc32(work->assets.data, work->assets.size);
	zip_add(&work->apk, &assets);
}

static int write_apk(const char *path, const zip_writer *apk, size_t central, const uint8_t *block, size_t block_size)
{
	FILE *output = fopen(path, "wb");
	int written;

	if (!output)
		return 0;
	written = fwrite(apk->data, 1, central, output) == central && fwrite(block, 1, block_size, output) == block_size
		&& fwrite(apk->data + central, 1, apk->size - central, output) == apk->size - central;
	return fclose(output) == 0 && written;
}

/* The steps, each holding what it makes in 'work' (make_apk frees it all) */
static int build(const apk_inputs *inputs, FILE *log, apk_step step, void *context, apk_work *work,
	apk_outcome *outcome)
{
	char what[80], pack_path[APK_PATH_SIZE + 16], key_path[APK_PATH_SIZE + 64];
	const int own_icon = inputs->icon[0] != '\0';
	const char *problem;
	size_t central, block_size, offset;
	FILE *output;

	if ((problem = crypto_self_test()) != NULL || (problem = ecdsa_self_test()) != NULL)
		return failed(outcome, "The tool's own check failed", problem);

	/* the game's Android build */
	tell(step, context, "reading the game's Android build");
	if (!read_file(inputs->base, &work->base))
		return failed(outcome, "The game's Android build cannot be read", inputs->base);
	if (!zip_read(work->base.data, work->base.size, &work->archive) || !zip_find(&work->archive, "AndroidManifest.xml"))
		return failed(outcome, "The game's Android build is not an APK", inputs->base);
	if (log)
		fprintf(log, "game      %s\n", inputs->base);

	/* the icon */
	if (own_icon)
	{
		tell(step, context, "reading the icon");
		if (!read_file(inputs->icon, &work->icon))
			return failed(outcome, "The icon cannot be read", inputs->icon);
		if ((problem = picture_problem(&work->icon)) != NULL)
		{
			snprintf(what, sizeof(what), "The icon is %s", problem);
			return failed(outcome, what, inputs->icon);
		}
		if (!takes_icon(&work->archive))
			return failed(outcome, "The icon cannot go into this build of the game", inputs->base);
		if (log)
			fprintf(log, "icon      %s\n", inputs->icon);
	}

	/* the assets, as the computer's first run takes them out (beside the APK, for a moment: where it goes is
	   tried first, so that a folder it cannot go in is not told as a ROM set that cannot be used) */
	tell(step, context, "taking the assets out of the ROM set");
	snprintf(pack_path, sizeof(pack_path), "%s.assets", inputs->out);
	if (!(output = fopen(pack_path, "wb")))
		return failed(outcome, APK_FILE " cannot be written", inputs->out);
	fclose(output);
	if (log)
		fprintf(log, "ROM set   %s\n", inputs->rom);
	if ((problem = game_data_extract(inputs->rom, pack_path)) != NULL)
	{
		remove(pack_path);
		return failed(outcome, "The ROM set cannot be used", problem);
	}
	if (!read_file(pack_path, &work->assets))
	{
		remove(pack_path);
		return failed(outcome, "The assets cannot be read back", pack_path);
	}
	remove(pack_path);

	/* the key: the player's, or made now */
	tell(step, context, "reading the signing key");
	if ((problem = key_obtain(&work->key, key_path, sizeof(key_path))) != NULL)
		return failed(outcome, problem, key_path);
	outcome->key_made = work->key.made;
	if (log)
		fprintf(log, "key       %s (%s)\n", key_path, work->key.made ? "made now" : "kept from before");

	/* the APK: the build's entries, the icon, the assets */
	tell(step, context, "putting the APK together");
	put_entries(work, own_icon);
	if (!zip_finish(&work->apk, &central))
		return failed(outcome, "There is not memory enough for the APK", NULL);
	bytes_free(&work->assets);
	bytes_free(&work->icon);

	/* its signature, before its central directory, whose offset moves past it */
	tell(step, context, "signing the APK");
	if (!(work->block = sign_block(work->apk.data, work->apk.size, central, &work->key, &block_size)))
		return failed(outcome, "The APK cannot be signed", NULL);
	offset = central + block_size;
	for (int byte = 0; byte < 4; byte++)
		work->apk.data[work->apk.size - 22 + 16 + byte] = (uint8_t)(offset >> (8 * byte));

	tell(step, context, "writing " APK_FILE);
	if (!write_apk(inputs->out, &work->apk, central, work->block, block_size))
		return failed(outcome, APK_FILE " cannot be written", inputs->out);
	outcome->size = work->apk.size + block_size;
	if (log)
		fprintf(log, "\nwritten   %s (%.1f MB)\n", inputs->out, (double)outcome->size / (1024.0 * 1024.0));
	return 1;
}

int make_apk(const apk_inputs *inputs, FILE *log, apk_step step, void *context, apk_outcome *outcome)
{
	apk_work work;
	int made;

	memset(&work, 0, sizeof(work));
	memset(outcome, 0, sizeof(*outcome));
	made = build(inputs, log, step, context, &work, outcome);
	key_forget(&work.key);
	free(work.block);
	zip_writer_free(&work.apk);
	zip_free(&work.archive);
	bytes_free(&work.base);
	bytes_free(&work.icon);
	bytes_free(&work.assets);
	return made;
}
