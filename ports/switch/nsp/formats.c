/**
 *    Name:        formats.c
 *    Description: PFS0, RomFS, hashed NCA sections, NCAs and the content meta, written into memory
 *    Version:     0.1.0
 *    Created:     23-10-2026
 *    Modified:    23-10-2026
 *    Author:      Sebastian Toyos
 *
 *    license:BSD-3-Clause
 *    Everything is little-endian but the CTR counters and the XTS tweaks. An NCA is its header (0xC00 bytes,
 *    ciphered with AES-XTS by the console's header key, sectors of 0x200 numbered from 0), then its sections,
 *    each ciphered with AES-CTR by the key in slot 2 of its key area (the counter: the FS header's upper
 *    IV, then the byte's offset in the NCA / 16) and padded to 0x200; the key area is ciphered by the
 *    console's key area key. A section's FS header holds the hash of what it holds: a PFS0's master hash of
 *    its table of block hashes, a RomFS's the first of its IVFC levels. hacBrewPack writes NCAs the same
 *    way (its padding of a hash table or level already aligned with a whole block more is not needed and
 *    not done here).
 */
#include "formats.h"

#include "crypto.h"

#include <stdlib.h>
#include <string.h>

#define MEDIA_UNIT       0x200u      /* an NCA's sections start and end at multiples of it */
#define IVFC_BLOCK       0x4000u
#define IVFC_LEVELS      6           /* five levels of hashes and the RomFS */
#define ROMFS_DATA_START 0x200u
#define EMPTY            0xFFFFFFFFu
#define SDK_VERSION      0x000C1100u /* the FS takes 0x000B0000 on */

static size_t align(size_t value, size_t to)
{
	return (value + to - 1) / to * to;
}

static void put16(uint8_t *at, uint16_t value)
{
	at[0] = (uint8_t)value;
	at[1] = (uint8_t)(value >> 8);
}

static void put32(uint8_t *at, uint32_t value)
{
	for (int byte = 0; byte < 4; byte++)
		at[byte] = (uint8_t)(value >> (8 * byte));
}

static void put64(uint8_t *at, uint64_t value)
{
	for (int byte = 0; byte < 8; byte++)
		at[byte] = (uint8_t)(value >> (8 * byte));
}

static int allocate(buffer *out, size_t size)
{
	out->data = (uint8_t *)calloc(size ? size : 1, 1);
	out->size = out->data ? size : 0;
	return out->data != NULL;
}

void buffer_free(buffer *data)
{
	free(data->data);
	data->data = NULL;
	data->size = 0;
}

/* ---- PFS0: "PFS0", the count, the string table's size, the entries (offset, size, name), the names ---- */

int pfs0_build(const named_file *files, int count, buffer *out)
{
	size_t names = 0, header, total, offset = 0, name_at = 0;

	for (int file = 0; file < count; file++)
		names += strlen(files[file].name) + 1;
	names = align(names, 0x20);
	header = 0x10 + 0x18 * (size_t)count + names;
	total = header;
	for (int file = 0; file < count; file++)
		total += files[file].size;
	if (!allocate(out, total))
		return 0;
	memcpy(out->data, "PFS0", 4);
	put32(out->data + 4, (uint32_t)count);
	put32(out->data + 8, (uint32_t)names);
	for (int file = 0; file < count; file++)
	{
		uint8_t *entry = out->data + 0x10 + 0x18 * (size_t)file;
		const size_t length = strlen(files[file].name) + 1;

		put64(entry, offset);
		put64(entry + 8, files[file].size);
		put32(entry + 0x10, (uint32_t)name_at);
		memcpy(out->data + 0x10 + 0x18 * (size_t)count + name_at, files[file].name, length);
		memcpy(out->data + header + offset, files[file].data, files[file].size);
		name_at += length;
		offset += files[file].size;
	}
	return 1;
}

/* ---- RomFS: the files' data from 0x200, then the directories' and files' hash tables and entries ---- */

/* Buckets for a hash table of 'entries' entries, as the console's RomFS tables count them */
static uint32_t romfs_buckets(uint32_t entries)
{
	uint32_t count = entries;

	if (entries < 3)
		return 3;
	if (entries < 19)
		return entries | 1;
	while (count % 2 == 0 || count % 3 == 0 || count % 5 == 0 || count % 7 == 0 || count % 11 == 0
		|| count % 13 == 0 || count % 17 == 0)
		count++;
	return count;
}

static uint32_t romfs_hash(uint32_t parent, const char *name)
{
	uint32_t hash = parent ^ 123456789u;

	for (; *name; name++)
		hash = ((hash >> 5) | (hash << 27)) ^ (uint8_t)*name;
	return hash;
}

static int by_name(const void *a, const void *b)
{
	return strcmp(((const named_file *)a)->name, ((const named_file *)b)->name);
}

int romfs_build(named_file *files, int count, buffer *out)
{
	const uint32_t file_buckets = romfs_buckets((uint32_t)count), directory_buckets = romfs_buckets(1);
	size_t data_size = 0, offsets[64], entries_size = 0, tables, directory_hash, directory_table, file_hash,
		file_table, total;
	uint8_t *root;

	if (count > 64)
		return 0;
	qsort(files, (size_t)count, sizeof(files[0]), by_name);
	for (int file = 0; file < count; file++)
	{
		offsets[file] = align(data_size, 0x10);
		data_size = offsets[file] + files[file].size;
		entries_size += 0x20 + align(strlen(files[file].name), 4);
	}
	tables = align(ROMFS_DATA_START + data_size, 4);
	directory_hash = tables;
	directory_table = directory_hash + 4 * directory_buckets;
	file_hash = directory_table + 0x18;    /* (the root, nameless) */
	file_table = file_hash + 4 * file_buckets;
	total = file_table + entries_size;
	if (!allocate(out, total))
		return 0;
	put64(out->data + 0x00, 0x50);
	put64(out->data + 0x08, directory_hash);
	put64(out->data + 0x10, 4 * (uint64_t)directory_buckets);
	put64(out->data + 0x18, directory_table);
	put64(out->data + 0x20, 0x18);
	put64(out->data + 0x28, file_hash);
	put64(out->data + 0x30, 4 * (uint64_t)file_buckets);
	put64(out->data + 0x38, file_table);
	put64(out->data + 0x40, entries_size);
	put64(out->data + 0x48, ROMFS_DATA_START);
	for (int file = 0; file < count; file++)
		memcpy(out->data + ROMFS_DATA_START + offsets[file], files[file].data, files[file].size);

	/* the root: its own parent, no sibling or child, its first file; in its bucket */
	memset(out->data + directory_hash, 0xFF, 4 * directory_buckets);
	memset(out->data + file_hash, 0xFF, 4 * file_buckets);
	root = out->data + directory_table;
	put32(root + 0x00, 0);
	put32(root + 0x04, EMPTY);
	put32(root + 0x08, EMPTY);
	put32(root + 0x0C, count > 0 ? 0 : EMPTY);
	put32(root + 0x10, EMPTY);
	put32(root + 0x14, 0);
	put32(out->data + directory_hash + 4 * (romfs_hash(0, "") % directory_buckets), 0);

	/* the files: each its sibling the next, in its bucket's chain (an entry holds the bucket's head before it) */
	for (int file = 0, entry_at = 0; file < count; file++)
	{
		const size_t length = strlen(files[file].name);
		const size_t size = 0x20 + align(length, 4);
		uint8_t *entry = out->data + file_table + entry_at;
		uint8_t *bucket = out->data + file_hash + 4 * (romfs_hash(0, files[file].name) % file_buckets);

		put32(entry + 0x00, 0);
		put32(entry + 0x04, file + 1 < count ? (uint32_t)(entry_at + size) : EMPTY);
		put64(entry + 0x08, offsets[file]);
		put64(entry + 0x10, files[file].size);
		memcpy(entry + 0x18, bucket, 4);
		put32(entry + 0x1C, (uint32_t)length);
		memcpy(entry + 0x20, files[file].name, length);
		put32(bucket, (uint32_t)entry_at);
		entry_at += (int)size;
	}
	return 1;
}

/* ---- hashed sections ---- */

int section_from_pfs0(const buffer *pfs0, uint32_t block_size, nca_section *section)
{
	const size_t blocks = (pfs0->size + block_size - 1) / block_size;
	const size_t table = 0x20 * blocks, pfs0_at = align(table, MEDIA_UNIT);
	uint8_t *header = section->fs_header;

	if (!allocate(&section->data, align(pfs0_at + pfs0->size, MEDIA_UNIT)))
		return 0;
	for (size_t block = 0; block < blocks; block++)
	{
		const size_t start = block * block_size;
		const size_t size = pfs0->size - start < block_size ? pfs0->size - start : block_size;    /* (the last as it is) */

		sha256_of(pfs0->data + start, size, section->data.data + 0x20 * block);
	}
	memcpy(section->data.data + pfs0_at, pfs0->data, pfs0->size);
	memset(header, 0, 0x200);
	put16(header + 0x00, 2);         /* version */
	header[0x02] = 1;                /* PartitionFS */
	header[0x03] = 2;                /* HierarchicalSha256 */
	header[0x04] = 3;                /* AES-CTR */
	sha256_of(section->data.data, table, header + 0x08);    /* the master hash: of the table */
	put32(header + 0x28, block_size);
	put32(header + 0x2C, 2);         /* layers: the table, the PFS0 */
	put64(header + 0x30, 0);
	put64(header + 0x38, table);
	put64(header + 0x40, pfs0_at);
	put64(header + 0x48, pfs0->size);
	return 1;
}

int section_from_romfs(const buffer *romfs, nca_section *section)
{
	size_t sizes[IVFC_LEVELS], offsets[IVFC_LEVELS], at = 0;
	uint8_t *header = section->fs_header;

	/* the levels' sizes from the RomFS up: each a SHA-256 for each block of the one after it, padded to
	   whole blocks (the RomFS keeps its own size) */
	sizes[IVFC_LEVELS - 1] = romfs->size;
	for (int level = IVFC_LEVELS - 2; level >= 0; level--)
		sizes[level] = align(0x20 * (align(sizes[level + 1], IVFC_BLOCK) / IVFC_BLOCK), IVFC_BLOCK);
	for (int level = 0; level < IVFC_LEVELS; level++)
	{
		offsets[level] = at;
		at += sizes[level];
	}
	if (!allocate(&section->data, align(at, IVFC_BLOCK)))
		return 0;
	memcpy(section->data.data + offsets[IVFC_LEVELS - 1], romfs->data, romfs->size);
	for (int level = IVFC_LEVELS - 2; level >= 0; level--)
	{
		const size_t blocks = align(sizes[level + 1], IVFC_BLOCK) / IVFC_BLOCK;

		/* (whole blocks: the RomFS's last one with the zeros after it) */
		for (size_t block = 0; block < blocks; block++)
			sha256_of(section->data.data + offsets[level + 1] + block * IVFC_BLOCK, IVFC_BLOCK,
				section->data.data + offsets[level] + 0x20 * block);
	}
	memset(header, 0, 0x200);
	put16(header + 0x00, 2);
	header[0x02] = 0;                /* RomFS */
	header[0x03] = 3;                /* HierarchicalIntegrity */
	header[0x04] = 3;                /* AES-CTR */
	memcpy(header + 0x08, "IVFC", 4);
	put32(header + 0x0C, 0x00020000u);
	put32(header + 0x10, 0x20);      /* the master hash's size */
	put32(header + 0x14, IVFC_LEVELS + 1);
	for (int level = 0; level < IVFC_LEVELS; level++)
	{
		uint8_t *entry = header + 0x18 + 0x18 * level;

		put64(entry, offsets[level]);
		put64(entry + 8, sizes[level]);
		put32(entry + 0x10, 14);     /* blocks of 1 << 14 */
	}
	sha256_of(section->data.data, sizes[0], header + 0xC8);    /* the master hash: of the first level */
	return 1;
}

/* ---- the NCA ---- */

int nca_build(const nca_keys *keys, uint8_t content_type, uint64_t title_id, nca_section *sections, int count,
	buffer *out, uint8_t hash[32])
{
	uint8_t content_key[16], seed[9], *header;
	size_t total = 0xC00, at = 0xC00;
	aes128 aes, tweak;

	for (int section = 0; section < count; section++)
		total += sections[section].data.size;
	if (!allocate(out, total))
	{
		for (int section = 0; section < count; section++)
			buffer_free(&sections[section].data);
		return 0;
	}
	/* the sections' key: any; this one from the title and the content's kind, so the same inputs make the
	   same NCA */
	put64(seed, title_id);
	seed[8] = content_type;
	sha256_of(seed, sizeof(seed), hash);
	memcpy(content_key, hash, 16);
	aes128_init(&aes, content_key);

	header = out->data;
	for (int section = 0; section < count; section++)
	{
		const uint8_t upper[8] = { 0 };    /* (the FS header's generation and secure value: 0) */
		uint8_t *entry = header + 0x240 + 0x10 * section;

		memcpy(out->data + at, sections[section].data.data, sections[section].data.size);
		put32(entry, (uint32_t)(at / MEDIA_UNIT));
		put32(entry + 4, (uint32_t)((at + sections[section].data.size) / MEDIA_UNIT));
		put32(entry + 8, 1);
		memcpy(header + 0x400 + 0x200 * section, sections[section].fs_header, 0x200);
		sha256_of(sections[section].fs_header, 0x200, header + 0x280 + 0x20 * section);
		aes128_ctr_crypt(&aes, upper, at, out->data + at, sections[section].data.size);
		at += sections[section].data.size;
		buffer_free(&sections[section].data);
	}
	memcpy(header + 0x200, "NCA3", 4);
	header[0x204] = 0;               /* download */
	header[0x205] = content_type;
	header[0x206] = 0;               /* key generation 0 and 1: master key 0, every firmware's */
	header[0x207] = 0;               /* the application's key area key */
	put64(header + 0x208, total);
	put64(header + 0x210, title_id);
	put32(header + 0x218, 0);
	put32(header + 0x21C, SDK_VERSION);
	header[0x220] = 0;
	memcpy(header + 0x300 + 0x20, content_key, 16);    /* slot 2: AES-CTR */
	aes128_init(&aes, keys->key_area_key);
	aes128_ecb_encrypt(&aes, header + 0x300, 0x40);
	aes128_init(&aes, keys->header_key);
	aes128_init(&tweak, keys->header_key + 16);
	aes128_xts_encrypt(&aes, &tweak, 0, 0x200, header, 0xC00);
	sha256_of(out->data, out->size, hash);
	return 1;
}

/* ---- the content meta of an application ---- */

static void content_record(uint8_t *record, const uint8_t hash[32], uint64_t size, uint8_t type)
{
	memcpy(record, hash, 0x20);
	memcpy(record + 0x20, hash, 0x10);    /* the content's id: its hash's first half */
	for (int byte = 0; byte < 6; byte++)
		record[0x30 + byte] = (uint8_t)(size >> (8 * byte));
	record[0x36] = type;
	record[0x37] = 0;
}

int cnmt_build(uint64_t title_id, const uint8_t program_hash[32], uint64_t program_size,
	const uint8_t control_hash[32], uint64_t control_size, buffer *out)
{
	if (!allocate(out, 0xC0))
		return 0;
	put64(out->data + 0x00, title_id);
	put32(out->data + 0x08, 0);           /* version */
	out->data[0x0C] = 0x80;               /* an application */
	put16(out->data + 0x0E, 0x10);        /* its extended header's size */
	put16(out->data + 0x10, 2);           /* contents: the program and its control data */
	put64(out->data + 0x20, title_id + 0x800);    /* its patches' id */
	put32(out->data + 0x28, 0);           /* the system version it needs */
	content_record(out->data + 0x30, program_hash, program_size, 1);
	content_record(out->data + 0x68, control_hash, control_size, 3);
	return 1;
}
