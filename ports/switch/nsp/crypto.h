/**
 *    Name:        crypto.h
 *    Description: what the .nsp tool ciphers and hashes with: AES-128 (its encryption: XTS for an NCA's header,
 *                 CTR for its sections, ECB for its key area) and SHA-256
 *    Version:     0.1.0
 *    Created:     23-10-2026
 *    Modified:    23-10-2026
 *    Author:      Sebastian Toyos
 *
 *    license:BSD-3-Clause
 *    Written from FIPS-197 (AES), FIPS-180-4 (SHA-256) and IEEE P1619 (XTS); crypto_self_test checks them
 *    against those documents' vectors.
 */
#ifndef SEGASONIC_NSP_CRYPTO_H
#define SEGASONIC_NSP_CRYPTO_H

#include <stddef.h>
#include <stdint.h>

typedef struct
{
	uint8_t round_keys[176];
} aes128;

typedef struct
{
	uint32_t state[8];
	uint64_t length;      /* bytes hashed so far */
	uint8_t block[64];
	size_t used;          /* bytes of 'block' filled */
} sha256;

void aes128_init(aes128 *aes, const uint8_t key[16]);
void aes128_encrypt_block(const aes128 *aes, const uint8_t in[16], uint8_t out[16]);

/* ECB: 'size' bytes (a multiple of 16), in place */
void aes128_ecb_encrypt(const aes128 *aes, uint8_t *data, size_t size);

/* CTR over 'size' bytes in place, the counter's upper 8 bytes 'upper' (big-endian) and its lower 8 the
   absolute 'offset' of 'data' / 16 (big-endian), as an NCA's sections count it; 'offset' a multiple of 16 */
void aes128_ctr_crypt(const aes128 *aes, const uint8_t upper[8], uint64_t offset, uint8_t *data, size_t size);

/* XTS over 'size' bytes in place (a multiple of 'sector_size', itself of 16), from sector 'first_sector':
   each sector's tweak its number as a 128-bit big-endian value (Nintendo's), the data key 'data' and the
   tweak key 'tweak' */
void aes128_xts_encrypt(const aes128 *data, const aes128 *tweak, uint64_t first_sector, size_t sector_size,
	uint8_t *buffer, size_t size);

void sha256_init(sha256 *hash);
void sha256_update(sha256 *hash, const void *data, size_t size);
void sha256_final(sha256 *hash, uint8_t digest[32]);
void sha256_of(const void *data, size_t size, uint8_t digest[32]);

/* The standards' vectors: NULL when they all hold, else which failed */
const char *crypto_self_test(void);

#endif
