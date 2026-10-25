/**
 *    Name:        zip.c
 *    Description: the APK's zip: reading the game's Android build's entries, writing the new APK's aligned,
 *                 its central directory and its end record
 *    Version:     0.1.0
 *    Created:     25-10-2026
 *    Modified:    25-10-2026
 *    Author:      Sebastian Toyos
 *
 *    license:BSD-3-Clause
 *    The entries are read from the central directory (its sizes and CRCs: a local header's may be left
 *    for a data descriptor after the data) and copied packed as they are, nothing unpacked or packed again.
 *    Each is written with a local header of its own, the central directory's fields in it and no data
 *    descriptor; a stored entry's extra field is apksigner's alignment field (0xD935: its alignment, then
 *    zeros) as long as its data needs to start at a multiple of 4, or of 4096 for a library (.so), which
 *    Android maps in place: what zipalign -p 4 asks for. Whatever came between the entries and the central
 *    directory (an APK Signing Block) is left behind.
 */
#include "zip.h"

#include <stdlib.h>
#include <string.h>

#define LOCAL_SIGNATURE    0x04034B50u
#define CENTRAL_SIGNATURE  0x02014B50u
#define END_SIGNATURE      0x06054B50u
#define LOCAL_SIZE         30
#define CENTRAL_SIZE       46
#define END_SIZE           22
#define ALIGNMENT_FIELD    0xD935u      /* apksigner's extra field: the alignment (2 bytes), then zeros */
#define DATA_DESCRIPTOR    0x0008u      /* (a flag: the CRC and sizes after the data) */
#define ZIP32_LIMIT        0xFFFFFFFFu  /* zip64's marker: no offset or size reaches it */

static uint16_t read16(const uint8_t *at)
{
	return (uint16_t)(at[0] | at[1] << 8);
}

static uint32_t read32(const uint8_t *at)
{
	return (uint32_t)at[0] | (uint32_t)at[1] << 8 | (uint32_t)at[2] << 16 | (uint32_t)at[3] << 24;
}

static void put16(uint8_t *at, size_t value)
{
	at[0] = (uint8_t)value;
	at[1] = (uint8_t)(value >> 8);
}

static void put32(uint8_t *at, size_t value)
{
	for (int byte = 0; byte < 4; byte++)
		at[byte] = (uint8_t)(value >> (8 * byte));
}

/* ---- reading ---- */

int zip_read(const uint8_t *data, size_t size, zip_archive *archive)
{
	size_t end = 0, central, central_end, count, at;
	int found = 0;

	memset(archive, 0, sizeof(*archive));
	/* the end record: the last one whose comment ends the zip */
	for (size_t back = 0; back <= 0xFFFF && back + END_SIZE <= size && !found; back++)
	{
		end = size - END_SIZE - back;
		found = read32(data + end) == END_SIGNATURE && read16(data + end + 20) == back;
	}
	if (!found)
		return 0;
	count = read16(data + end + 10);
	central = read32(data + end + 16);
	central_end = central + read32(data + end + 12);
	/* (one disk, and no zip64) */
	if (read16(data + end + 4) != 0 || read16(data + end + 6) != 0 || read16(data + end + 8) != count
		|| count == 0xFFFF || central > end || central_end > end)
		return 0;
	if (!(archive->entries = (zip_entry *)calloc(count ? count : 1, sizeof(zip_entry))))
		return 0;
	at = central;
	for (size_t index = 0; index < count; index++)
	{
		zip_entry *const entry = &archive->entries[index];
		const uint8_t *const record = data + at;
		size_t local, data_at;

		if (at + CENTRAL_SIZE > central_end || read32(record) != CENTRAL_SIGNATURE)
			break;
		entry->made_by = read16(record + 4);
		entry->needed = read16(record + 6);
		entry->flags = read16(record + 8);
		entry->method = read16(record + 10);
		entry->time = read16(record + 12);
		entry->date = read16(record + 14);
		entry->crc = read32(record + 16);
		entry->packed = read32(record + 20);
		entry->unpacked = read32(record + 24);
		entry->name_size = read16(record + 28);
		entry->extra_size = read16(record + 30);
		entry->comment_size = read16(record + 32);
		entry->internal = read16(record + 36);
		entry->external = read32(record + 38);
		local = read32(record + 42);
		entry->name = (const char *)record + CENTRAL_SIZE;
		entry->extra = record + CENTRAL_SIZE + entry->name_size;
		entry->comment = entry->extra + entry->extra_size;
		at += CENTRAL_SIZE + (size_t)entry->name_size + entry->extra_size + entry->comment_size;
		/* its local header, before the central directory, of the same name; its data after it */
		if (at > central_end || read16(record + 34) != 0 || entry->packed == ZIP32_LIMIT
			|| entry->unpacked == ZIP32_LIMIT || local + LOCAL_SIZE + entry->name_size > central
			|| read32(data + local) != LOCAL_SIGNATURE || read16(data + local + 26) != entry->name_size
			|| memcmp(data + local + LOCAL_SIZE, entry->name, entry->name_size) != 0)
			break;
		data_at = local + LOCAL_SIZE + entry->name_size + read16(data + local + 28);
		if (data_at > central || entry->packed > central - data_at)
			break;
		entry->data = data + data_at;
		archive->count++;
	}
	if (archive->count != count)
	{
		zip_free(archive);
		return 0;
	}
	return 1;
}

void zip_free(zip_archive *archive)
{
	free(archive->entries);
	archive->entries = NULL;
	archive->count = 0;
}

int zip_named(const zip_entry *entry, const char *name)
{
	return strlen(name) == entry->name_size && memcmp(entry->name, name, entry->name_size) == 0;
}

const zip_entry *zip_find(const zip_archive *archive, const char *name)
{
	for (size_t index = 0; index < archive->count; index++)
		if (zip_named(&archive->entries[index], name))
			return &archive->entries[index];
	return NULL;
}

uint32_t zip_crc32(const uint8_t *data, size_t size)
{
	uint32_t table[256], crc = 0xFFFFFFFFu;

	for (uint32_t index = 0; index < 256; index++)
	{
		uint32_t value = index;

		for (int bit = 0; bit < 8; bit++)
			value = value & 1u ? 0xEDB88320u ^ (value >> 1) : value >> 1;
		table[index] = value;
	}
	for (size_t at = 0; at < size; at++)
		crc = table[(crc ^ data[at]) & 0xFFu] ^ (crc >> 8);
	return ~crc;
}

/* ---- writing ---- */

/* 'more' bytes after the 'size' of 'data', which grows when it has no room for them: where they go, or
   NULL (and 'failed') when there is no memory */
static uint8_t *grow(uint8_t **data, size_t *size, size_t *room, size_t more, int *failed)
{
	uint8_t *at;

	if (*failed)
		return NULL;
	if (*size + more > *room)
	{
		size_t next = *room ? *room : 0x10000;
		uint8_t *bigger;

		while (next < *size + more)
			next *= 2;
		if (!(bigger = (uint8_t *)realloc(*data, next)))
		{
			*failed = 1;
			return NULL;
		}
		*data = bigger;
		*room = next;
	}
	at = *data + *size;
	*size += more;
	return at;
}

static void copy(uint8_t *at, const void *bytes, size_t size)
{
	if (size)
		memcpy(at, bytes, size);
}

void zip_add(zip_writer *zip, const zip_entry *entry)
{
	const size_t offset = zip->size;
	const int library = entry->name_size > 3 && memcmp(entry->name + entry->name_size - 3, ".so", 3) == 0;
	const size_t alignment = library ? 4096 : 4;
	size_t extra = 0;
	uint8_t *at;

	if (entry->method == ZIP_STORED)
	{
		/* (its field's header and alignment, then zeros as far as an aligned start) */
		const size_t data_at = offset + LOCAL_SIZE + entry->name_size + 6;

		extra = 6 + (alignment - data_at % alignment) % alignment;
	}
	if (offset + LOCAL_SIZE + entry->name_size + extra + entry->packed >= ZIP32_LIMIT)
		zip->failed = 1;
	if (!(at = grow(&zip->data, &zip->size, &zip->room, LOCAL_SIZE + entry->name_size + extra + entry->packed,
		&zip->failed)))
		return;
	put32(at, LOCAL_SIGNATURE);
	put16(at + 4, entry->needed);
	put16(at + 6, entry->flags & ~DATA_DESCRIPTOR);
	put16(at + 8, entry->method);
	put16(at + 10, entry->time);
	put16(at + 12, entry->date);
	put32(at + 14, entry->crc);
	put32(at + 18, entry->packed);
	put32(at + 22, entry->unpacked);
	put16(at + 26, entry->name_size);
	put16(at + 28, extra);
	copy(at + LOCAL_SIZE, entry->name, entry->name_size);
	at += LOCAL_SIZE + entry->name_size;
	if (extra)
	{
		memset(at, 0, extra);
		put16(at, ALIGNMENT_FIELD);
		put16(at + 2, extra - 4);
		put16(at + 4, alignment);
		at += extra;
	}
	copy(at, entry->data, entry->packed);

	/* its central directory record, kept until the entries end */
	if (!(at = grow(&zip->central, &zip->central_size, &zip->central_room,
		CENTRAL_SIZE + (size_t)entry->name_size + entry->extra_size + entry->comment_size, &zip->failed)))
		return;
	put32(at, CENTRAL_SIGNATURE);
	put16(at + 4, entry->made_by);
	put16(at + 6, entry->needed);
	put16(at + 8, entry->flags & ~DATA_DESCRIPTOR);
	put16(at + 10, entry->method);
	put16(at + 12, entry->time);
	put16(at + 14, entry->date);
	put32(at + 16, entry->crc);
	put32(at + 20, entry->packed);
	put32(at + 24, entry->unpacked);
	put16(at + 28, entry->name_size);
	put16(at + 30, entry->extra_size);
	put16(at + 32, entry->comment_size);
	put16(at + 34, 0);
	put16(at + 36, entry->internal);
	put32(at + 38, entry->external);
	put32(at + 42, offset);
	copy(at + CENTRAL_SIZE, entry->name, entry->name_size);
	copy(at + CENTRAL_SIZE + entry->name_size, entry->extra, entry->extra_size);
	copy(at + CENTRAL_SIZE + entry->name_size + entry->extra_size, entry->comment, entry->comment_size);
	zip->count++;
}

int zip_finish(zip_writer *zip, size_t *central)
{
	uint8_t *at;

	*central = zip->size;
	if (zip->count >= 0xFFFF || zip->size + zip->central_size + END_SIZE >= ZIP32_LIMIT)
		zip->failed = 1;
	if (!(at = grow(&zip->data, &zip->size, &zip->room, zip->central_size + END_SIZE, &zip->failed)))
		return 0;
	copy(at, zip->central, zip->central_size);
	at += zip->central_size;
	put32(at, END_SIGNATURE);
	put16(at + 4, 0);
	put16(at + 6, 0);
	put16(at + 8, zip->count);
	put16(at + 10, zip->count);
	put32(at + 12, zip->central_size);
	put32(at + 16, *central);
	put16(at + 20, 0);
	return 1;
}

void zip_writer_free(zip_writer *zip)
{
	free(zip->data);
	free(zip->central);
	memset(zip, 0, sizeof(*zip));
}
