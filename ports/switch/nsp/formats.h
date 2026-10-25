/**
 *    Name:        formats.h
 *    Description: the Switch's formats the .nsp tool writes: PFS0, RomFS, an NCA's sections (hashed: a
 *                 hash table for a PFS0, IVFC levels for a RomFS) and the NCA itself, the content meta
 *    Version:     0.1.0
 *    Created:     23-10-2026
 *    Modified:    23-10-2026
 *    Author:      Sebastian Toyos
 *
 *    license:BSD-3-Clause
 *    Written from the formats as switchbrew documents them and hacBrewPack, hactool and Atmosphère's file
 *    system read and write them (no code of theirs): see ports/switch/README.md.
 */
#ifndef SEGASONIC_NSP_FORMATS_H
#define SEGASONIC_NSP_FORMATS_H

#include <stddef.h>
#include <stdint.h>

typedef struct
{
	uint8_t *data;
	size_t size;
} buffer;

typedef struct
{
	const char *name;
	const uint8_t *data;
	size_t size;
} named_file;

/* An NCA's section before it is ciphered: its bytes (a multiple of 0x200) and its FS header */
typedef struct
{
	buffer data;
	uint8_t fs_header[0x200];
} nca_section;

/* The keys of the player's console the NCAs are ciphered with (prod.keys) */
typedef struct
{
	uint8_t header_key[32];      /* header_key: the data key, then the tweak key */
	uint8_t key_area_key[16];    /* key_area_key_application_00 */
} nca_keys;

enum
{
	NCA_PROGRAM = 0,
	NCA_META = 1,
	NCA_CONTROL = 2
};

void buffer_free(buffer *data);

/* The files in a PFS0, in their order: false when there is no memory for it */
int pfs0_build(const named_file *files, int count, buffer *out);

/* The files in a RomFS's root, sorted by name (the array is sorted in place) */
int romfs_build(named_file *files, int count, buffer *out);

/* A PFS0 as an NCA section: a SHA-256 for each block of 'block_size' before it */
int section_from_pfs0(const buffer *pfs0, uint32_t block_size, nca_section *section);

/* A RomFS as an NCA section: under the IVFC levels of SHA-256s, blocks of 0x4000 */
int section_from_romfs(const buffer *romfs, nca_section *section);

/* The NCA of the sections (freed), ciphered with the keys; 'hash' its SHA-256, whose first 16 bytes name it */
int nca_build(const nca_keys *keys, uint8_t content_type, uint64_t title_id, nca_section *sections, int count,
	buffer *out, uint8_t hash[32]);

/* An application's content meta: its program and control NCAs, by their hashes and sizes */
int cnmt_build(uint64_t title_id, const uint8_t program_hash[32], uint64_t program_size,
	const uint8_t control_hash[32], uint64_t control_size, buffer *out);

#endif
