/**
 *    Name:        zip.h
 *    Description: the APK as a zip: the entries of the game's Android build read from its central
 *                 directory, and the new APK's written one after the other, aligned as zipalign -p 4 aligns
 *                 them (zip.c)
 *    Version:     0.1.0
 *    Created:     25-10-2026
 *    Modified:    25-10-2026
 *    Author:      Sebastian Toyos
 *
 *    license:BSD-3-Clause
 *    Written from PKWARE's APPNOTE.TXT (the zip format) and from what zipalign and apksigner write and check
 *    (the alignment, its extra field 0xD935). No zip64: an APK of the game is far below 4 GB.
 */
#ifndef SEGASONIC_APK_ZIP_H
#define SEGASONIC_APK_ZIP_H

#include <stddef.h>
#include <stdint.h>

#define ZIP_STORED 0

/* An entry: its central directory record's fields, and its bytes as they are in the zip (packed) */
typedef struct
{
	const char *name;               /* not ended by a 0: name_size bytes */
	const uint8_t *extra, *comment;    /* the central directory's */
	const uint8_t *data;
	uint16_t name_size, extra_size, comment_size;
	uint16_t made_by, needed, flags, method, time, date, internal;
	uint32_t crc, packed, unpacked, external;
} zip_entry;

typedef struct
{
	zip_entry *entries;
	size_t count;
} zip_archive;

/* The zip being written: its entries, then (zip_finish) its central directory and its end record */
typedef struct
{
	uint8_t *data;
	size_t size, room;
	uint8_t *central;
	size_t central_size, central_room;
	size_t count;
	int failed;                     /* out of memory, or past what a zip without zip64 holds */
} zip_writer;

/* The entries of the zip in 'data', in its central directory's order (pointing into 'data'): false when
   it is not a zip the tool reads */
int zip_read(const uint8_t *data, size_t size, zip_archive *archive);
void zip_free(zip_archive *archive);

/* The entry of that name, or NULL */
const zip_entry *zip_find(const zip_archive *archive, const char *name);

/* True when the entry's name is 'name' */
int zip_named(const zip_entry *entry, const char *name);

uint32_t zip_crc32(const uint8_t *data, size_t size);

/* An entry after the last: a stored one with its data at a multiple of 4 (a library, .so, of 4096) */
void zip_add(zip_writer *zip, const zip_entry *entry);

/* The central directory and the end record after the entries: true when the zip is whole in 'data';
   'central' the central directory's offset, where the entries end */
int zip_finish(zip_writer *zip, size_t *central);
void zip_writer_free(zip_writer *zip);

#endif
