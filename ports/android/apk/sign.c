/**
 *    Name:        sign.c
 *    Description: APK Signature Scheme v2 for the APK tool: the APK's chunked SHA-256, the signed data, its
 *                 ECDSA signature by the player's key and the APK Signing Block that holds them
 *    Version:     0.1.0
 *    Created:     25-10-2026
 *    Modified:    25-10-2026
 *    Author:      Sebastian Toyos
 *
 *    license:BSD-3-Clause
 *    The scheme signs the APK's bytes whole: its entries, its central directory and its end record (its
 *    central directory's offset as though the signing block were not there), each cut in chunks of 1 MB;
 *    each chunk's SHA-256 is of 0xA5, its size and its bytes, and the APK's of 0x5A, their count and them.
 *    The block holds one signer: the signed data (that digest, the certificate, no attributes), its
 *    signature (ECDSA with SHA-256, 0x0201) and the public key. Android takes v2 from 7.0 (API 24, the
 *    game's least), so no v1 (JAR) signature is made; one that the game's Android build had is left out
 *    with its files (make_apk.c). Every length in the block is little-endian: 32 bits inside the scheme's
 *    value, 64 bits for the block's and its pairs'.
 */
#include "sign.h"
#include "crypto.h"
#include "ecdsa.h"

#include <stdlib.h>
#include <string.h>

#define CHUNK_SIZE       0x100000u      /* 1 MB */
#define END_SIZE         22             /* the zip's end record (the APK's has no comment) */
#define SCHEME_V2        0x7109871Au    /* the signing block's ID of the scheme's value */
#define ECDSA_SHA256     0x0201u        /* the scheme's ECDSA with SHA-256, its digest the chunked SHA-256 */
#define BLOCK_MAGIC      "APK Sig Block 42"

static size_t put32(uint8_t *at, size_t value)
{
	for (int byte = 0; byte < 4; byte++)
		at[byte] = (uint8_t)(value >> (8 * byte));
	return 4;
}

static size_t put64(uint8_t *at, uint64_t value)
{
	for (int byte = 0; byte < 8; byte++)
		at[byte] = (uint8_t)(value >> (8 * byte));
	return 8;
}

static size_t put(uint8_t *at, const void *bytes, size_t size)
{
	memcpy(at, bytes, size);
	return size;
}

/* The APK's chunked SHA-256: its entries, its central directory, its end record */
static void content_digest(const uint8_t *apk, size_t size, size_t central, uint8_t digest[32])
{
	const size_t starts[3] = { 0, central, size - END_SIZE }, ends[3] = { central, size - END_SIZE, size };
	size_t chunks = 0;
	uint8_t head[5], chunk_digest[32];
	sha256 whole;

	for (int section = 0; section < 3; section++)
		chunks += (ends[section] - starts[section] + CHUNK_SIZE - 1) / CHUNK_SIZE;
	head[0] = 0x5A;
	put32(head + 1, chunks);
	sha256_init(&whole);
	sha256_update(&whole, head, sizeof(head));
	for (int section = 0; section < 3; section++)
		for (size_t at = starts[section]; at < ends[section]; at += CHUNK_SIZE)
		{
			const size_t length = ends[section] - at < CHUNK_SIZE ? ends[section] - at : CHUNK_SIZE;
			sha256 chunk;

			head[0] = 0xA5;
			put32(head + 1, length);
			sha256_init(&chunk);
			sha256_update(&chunk, head, sizeof(head));
			sha256_update(&chunk, apk + at, length);
			sha256_final(&chunk, chunk_digest);
			sha256_update(&whole, chunk_digest, sizeof(chunk_digest));
		}
	sha256_final(&whole, digest);
}

uint8_t *sign_block(const uint8_t *apk, size_t size, size_t central, const apk_key *key, size_t *block_size)
{
	const size_t certificate = key->certificate_size;
	/* the signed data: its digests (one), its certificates (one), its attributes (none) */
	const size_t signed_size = 4 + (4 + 8 + 32) + 4 + (4 + certificate) + 4;
	uint8_t *signed_data, *block, digest[32], hash[32], r[32], s[32], signature[ECDSA_SIGNATURE_SIZE];
	size_t signature_size, signer_size, value_size, at = 0;

	*block_size = 0;
	if (size < END_SIZE || central > size - END_SIZE || !(signed_data = (uint8_t *)malloc(signed_size)))
		return NULL;
	content_digest(apk, size, central, digest);
	at += put32(signed_data + at, 4 + 8 + 32);
	at += put32(signed_data + at, 8 + 32);
	at += put32(signed_data + at, ECDSA_SHA256);
	at += put32(signed_data + at, 32);
	at += put(signed_data + at, digest, 32);
	at += put32(signed_data + at, 4 + certificate);
	at += put32(signed_data + at, certificate);
	at += put(signed_data + at, key->certificate, certificate);
	put32(signed_data + at, 0);

	/* its signature, checked before it goes in */
	sha256_of(signed_data, signed_size, hash);
	ecdsa_sign(key->private_key, hash, r, s);
	if (!ecdsa_verify(key->public_key, hash, r, s))
	{
		free(signed_data);
		return NULL;
	}
	signature_size = ecdsa_der(r, s, signature);

	/* the signer (the signed data, its signatures, the public key), in the sequence of signers, in the
	   block's one pair */
	signer_size = (4 + signed_size) + (4 + 4 + 8 + signature_size) + (4 + KEY_PUBLIC_SIZE);
	value_size = 4 + 4 + signer_size;
	*block_size = 8 + (8 + 4 + value_size) + 8 + 16;
	if (!(block = (uint8_t *)malloc(*block_size)))
	{
		free(signed_data);
		*block_size = 0;
		return NULL;
	}
	at = put64(block, *block_size - 8);
	at += put64(block + at, 4 + value_size);
	at += put32(block + at, SCHEME_V2);
	at += put32(block + at, 4 + signer_size);
	at += put32(block + at, signer_size);
	at += put32(block + at, signed_size);
	at += put(block + at, signed_data, signed_size);
	at += put32(block + at, 4 + 8 + signature_size);
	at += put32(block + at, 8 + signature_size);
	at += put32(block + at, ECDSA_SHA256);
	at += put32(block + at, signature_size);
	at += put(block + at, signature, signature_size);
	at += put32(block + at, KEY_PUBLIC_SIZE);
	at += put(block + at, key->public_info, KEY_PUBLIC_SIZE);
	at += put64(block + at, *block_size - 8);
	put(block + at, BLOCK_MAGIC, 16);
	free(signed_data);
	return block;
}
