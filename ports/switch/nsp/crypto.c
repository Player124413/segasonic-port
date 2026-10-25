/**
 *    Name:        crypto.c
 *    Description: AES-128 encryption (ECB, CTR, XTS) and SHA-256 for the .nsp tool
 *    Version:     0.1.0
 *    Created:     23-10-2026
 *    Modified:    23-10-2026
 *    Author:      Sebastian Toyos
 *
 *    license:BSD-3-Clause
 *    Plain table AES (the tool ciphers a few tens of megabytes once, on the player's computer): the S-box, the
 *    key expansion and the rounds of FIPS-197. SHA-256 as FIPS-180-4 has it. XTS as IEEE P1619, with the
 *    tweak's starting value each sector's number in big-endian, as the Switch's NCA headers take it.
 */
#include "crypto.h"

#include <string.h>

static const uint8_t sbox[256] = {
	0x63, 0x7C, 0x77, 0x7B, 0xF2, 0x6B, 0x6F, 0xC5, 0x30, 0x01, 0x67, 0x2B, 0xFE, 0xD7, 0xAB, 0x76,
	0xCA, 0x82, 0xC9, 0x7D, 0xFA, 0x59, 0x47, 0xF0, 0xAD, 0xD4, 0xA2, 0xAF, 0x9C, 0xA4, 0x72, 0xC0,
	0xB7, 0xFD, 0x93, 0x26, 0x36, 0x3F, 0xF7, 0xCC, 0x34, 0xA5, 0xE5, 0xF1, 0x71, 0xD8, 0x31, 0x15,
	0x04, 0xC7, 0x23, 0xC3, 0x18, 0x96, 0x05, 0x9A, 0x07, 0x12, 0x80, 0xE2, 0xEB, 0x27, 0xB2, 0x75,
	0x09, 0x83, 0x2C, 0x1A, 0x1B, 0x6E, 0x5A, 0xA0, 0x52, 0x3B, 0xD6, 0xB3, 0x29, 0xE3, 0x2F, 0x84,
	0x53, 0xD1, 0x00, 0xED, 0x20, 0xFC, 0xB1, 0x5B, 0x6A, 0xCB, 0xBE, 0x39, 0x4A, 0x4C, 0x58, 0xCF,
	0xD0, 0xEF, 0xAA, 0xFB, 0x43, 0x4D, 0x33, 0x85, 0x45, 0xF9, 0x02, 0x7F, 0x50, 0x3C, 0x9F, 0xA8,
	0x51, 0xA3, 0x40, 0x8F, 0x92, 0x9D, 0x38, 0xF5, 0xBC, 0xB6, 0xDA, 0x21, 0x10, 0xFF, 0xF3, 0xD2,
	0xCD, 0x0C, 0x13, 0xEC, 0x5F, 0x97, 0x44, 0x17, 0xC4, 0xA7, 0x7E, 0x3D, 0x64, 0x5D, 0x19, 0x73,
	0x60, 0x81, 0x4F, 0xDC, 0x22, 0x2A, 0x90, 0x88, 0x46, 0xEE, 0xB8, 0x14, 0xDE, 0x5E, 0x0B, 0xDB,
	0xE0, 0x32, 0x3A, 0x0A, 0x49, 0x06, 0x24, 0x5C, 0xC2, 0xD3, 0xAC, 0x62, 0x91, 0x95, 0xE4, 0x79,
	0xE7, 0xC8, 0x37, 0x6D, 0x8D, 0xD5, 0x4E, 0xA9, 0x6C, 0x56, 0xF4, 0xEA, 0x65, 0x7A, 0xAE, 0x08,
	0xBA, 0x78, 0x25, 0x2E, 0x1C, 0xA6, 0xB4, 0xC6, 0xE8, 0xDD, 0x74, 0x1F, 0x4B, 0xBD, 0x8B, 0x8A,
	0x70, 0x3E, 0xB5, 0x66, 0x48, 0x03, 0xF6, 0x0E, 0x61, 0x35, 0x57, 0xB9, 0x86, 0xC1, 0x1D, 0x9E,
	0xE1, 0xF8, 0x98, 0x11, 0x69, 0xD9, 0x8E, 0x94, 0x9B, 0x1E, 0x87, 0xE9, 0xCE, 0x55, 0x28, 0xDF,
	0x8C, 0xA1, 0x89, 0x0D, 0xBF, 0xE6, 0x42, 0x68, 0x41, 0x99, 0x2D, 0x0F, 0xB0, 0x54, 0xBB, 0x16,
};

/* ---- AES-128 ---- */

void aes128_init(aes128 *aes, const uint8_t key[16])
{
	static const uint8_t round_constants[10] = { 0x01, 0x02, 0x04, 0x08, 0x10, 0x20, 0x40, 0x80, 0x1B, 0x36 };
	uint8_t *words = aes->round_keys;

	memcpy(words, key, 16);
	for (int at = 16; at < 176; at += 4)
	{
		uint8_t word[4];

		memcpy(word, words + at - 4, 4);
		if (at % 16 == 0)
		{
			/* RotWord, SubWord, Rcon */
			const uint8_t first = word[0];

			word[0] = (uint8_t)(sbox[word[1]] ^ round_constants[at / 16 - 1]);
			word[1] = sbox[word[2]];
			word[2] = sbox[word[3]];
			word[3] = sbox[first];
		}
		for (int byte = 0; byte < 4; byte++)
			words[at + byte] = (uint8_t)(words[at - 16 + byte] ^ word[byte]);
	}
}

static uint8_t times_2(uint8_t value)
{
	return (uint8_t)((value << 1) ^ ((value & 0x80u) ? 0x1Bu : 0x00u));
}

void aes128_encrypt_block(const aes128 *aes, const uint8_t in[16], uint8_t out[16])
{
	uint8_t state[16];

	for (int byte = 0; byte < 16; byte++)
		state[byte] = (uint8_t)(in[byte] ^ aes->round_keys[byte]);
	for (int round = 1; round <= 10; round++)
	{
		uint8_t shifted[16];

		/* SubBytes and ShiftRows: the state is column by column, row r of column c at 4c + r */
		for (int column = 0; column < 4; column++)
			for (int row = 0; row < 4; row++)
				shifted[column * 4 + row] = sbox[state[((column + row) % 4) * 4 + row]];
		if (round < 10)
		{
			/* MixColumns */
			for (int column = 0; column < 4; column++)
			{
				const uint8_t *a = shifted + column * 4;
				const uint8_t all = (uint8_t)(a[0] ^ a[1] ^ a[2] ^ a[3]);
				uint8_t mixed[4];

				for (int row = 0; row < 4; row++)
					mixed[row] = (uint8_t)(a[row] ^ all ^ times_2((uint8_t)(a[row] ^ a[(row + 1) % 4])));
				memcpy(state + column * 4, mixed, 4);
			}
		}
		else
			memcpy(state, shifted, 16);
		for (int byte = 0; byte < 16; byte++)
			state[byte] ^= aes->round_keys[round * 16 + byte];
	}
	memcpy(out, state, 16);
}

void aes128_ecb_encrypt(const aes128 *aes, uint8_t *data, size_t size)
{
	for (size_t at = 0; at + 16 <= size; at += 16)
		aes128_encrypt_block(aes, data + at, data + at);
}

void aes128_ctr_crypt(const aes128 *aes, const uint8_t upper[8], uint64_t offset, uint8_t *data, size_t size)
{
	uint8_t counter[16], stream[16];
	uint64_t block = offset >> 4;

	memcpy(counter, upper, 8);
	for (size_t at = 0; at < size; at += 16, block++)
	{
		for (int byte = 0; byte < 8; byte++)
			counter[15 - byte] = (uint8_t)(block >> (8 * byte));
		aes128_encrypt_block(aes, counter, stream);
		for (size_t byte = 0; byte < 16 && at + byte < size; byte++)
			data[at + byte] ^= stream[byte];
	}
}

void aes128_xts_encrypt(const aes128 *data, const aes128 *tweak, uint64_t first_sector, size_t sector_size,
	uint8_t *buffer, size_t size)
{
	uint64_t sector = first_sector;

	for (size_t start = 0; start + sector_size <= size; start += sector_size, sector++)
	{
		uint8_t value[16] = { 0 };

		for (int byte = 0; byte < 8; byte++)
			value[15 - byte] = (uint8_t)(sector >> (8 * byte));
		aes128_encrypt_block(tweak, value, value);
		for (size_t at = start; at < start + sector_size; at += 16)
		{
			uint8_t carry;

			for (int byte = 0; byte < 16; byte++)
				buffer[at + byte] ^= value[byte];
			aes128_encrypt_block(data, buffer + at, buffer + at);
			for (int byte = 0; byte < 16; byte++)
				buffer[at + byte] ^= value[byte];
			/* the tweak times x in GF(2^128), its bytes least significant first */
			carry = (uint8_t)(value[15] >> 7);
			for (int byte = 15; byte > 0; byte--)
				value[byte] = (uint8_t)((value[byte] << 1) | (value[byte - 1] >> 7));
			value[0] = (uint8_t)((value[0] << 1) ^ (carry ? 0x87u : 0x00u));
		}
	}
}

/* ---- SHA-256 ---- */

static const uint32_t sha256_constants[64] = {
	0x428A2F98, 0x71374491, 0xB5C0FBCF, 0xE9B5DBA5, 0x3956C25B, 0x59F111F1, 0x923F82A4, 0xAB1C5ED5,
	0xD807AA98, 0x12835B01, 0x243185BE, 0x550C7DC3, 0x72BE5D74, 0x80DEB1FE, 0x9BDC06A7, 0xC19BF174,
	0xE49B69C1, 0xEFBE4786, 0x0FC19DC6, 0x240CA1CC, 0x2DE92C6F, 0x4A7484AA, 0x5CB0A9DC, 0x76F988DA,
	0x983E5152, 0xA831C66D, 0xB00327C8, 0xBF597FC7, 0xC6E00BF3, 0xD5A79147, 0x06CA6351, 0x14292967,
	0x27B70A85, 0x2E1B2138, 0x4D2C6DFC, 0x53380D13, 0x650A7354, 0x766A0ABB, 0x81C2C92E, 0x92722C85,
	0xA2BFE8A1, 0xA81A664B, 0xC24B8B70, 0xC76C51A3, 0xD192E819, 0xD6990624, 0xF40E3585, 0x106AA070,
	0x19A4C116, 0x1E376C08, 0x2748774C, 0x34B0BCB5, 0x391C0CB3, 0x4ED8AA4A, 0x5B9CCA4F, 0x682E6FF3,
	0x748F82EE, 0x78A5636F, 0x84C87814, 0x8CC70208, 0x90BEFFFA, 0xA4506CEB, 0xBEF9A3F7, 0xC67178F2,
};

static uint32_t rotate_right(uint32_t value, int bits)
{
	return (value >> bits) | (value << (32 - bits));
}

static void sha256_block(sha256 *hash, const uint8_t block[64])
{
	uint32_t words[64], a, b, c, d, e, f, g, h;

	for (int at = 0; at < 16; at++)
		words[at] = (uint32_t)block[at * 4] << 24 | (uint32_t)block[at * 4 + 1] << 16
			| (uint32_t)block[at * 4 + 2] << 8 | block[at * 4 + 3];
	for (int at = 16; at < 64; at++)
	{
		const uint32_t s0 = rotate_right(words[at - 15], 7) ^ rotate_right(words[at - 15], 18) ^ (words[at - 15] >> 3);
		const uint32_t s1 = rotate_right(words[at - 2], 17) ^ rotate_right(words[at - 2], 19) ^ (words[at - 2] >> 10);

		words[at] = words[at - 16] + s0 + words[at - 7] + s1;
	}
	a = hash->state[0];
	b = hash->state[1];
	c = hash->state[2];
	d = hash->state[3];
	e = hash->state[4];
	f = hash->state[5];
	g = hash->state[6];
	h = hash->state[7];
	for (int at = 0; at < 64; at++)
	{
		const uint32_t t1 = h + (rotate_right(e, 6) ^ rotate_right(e, 11) ^ rotate_right(e, 25)) + ((e & f) ^ (~e & g))
			+ sha256_constants[at] + words[at];
		const uint32_t t2 = (rotate_right(a, 2) ^ rotate_right(a, 13) ^ rotate_right(a, 22)) + ((a & b) ^ (a & c) ^ (b & c));

		h = g;
		g = f;
		f = e;
		e = d + t1;
		d = c;
		c = b;
		b = a;
		a = t1 + t2;
	}
	hash->state[0] += a;
	hash->state[1] += b;
	hash->state[2] += c;
	hash->state[3] += d;
	hash->state[4] += e;
	hash->state[5] += f;
	hash->state[6] += g;
	hash->state[7] += h;
}

void sha256_init(sha256 *hash)
{
	static const uint32_t start[8] = {
		0x6A09E667, 0xBB67AE85, 0x3C6EF372, 0xA54FF53A, 0x510E527F, 0x9B05688C, 0x1F83D9AB, 0x5BE0CD19
	};

	memcpy(hash->state, start, sizeof(start));
	hash->length = 0;
	hash->used = 0;
}

void sha256_update(sha256 *hash, const void *data, size_t size)
{
	const uint8_t *bytes = (const uint8_t *)data;

	hash->length += size;
	while (size > 0)
	{
		size_t take = 64 - hash->used;

		if (take > size)
			take = size;
		memcpy(hash->block + hash->used, bytes, take);
		hash->used += take;
		bytes += take;
		size -= take;
		if (hash->used == 64)
		{
			sha256_block(hash, hash->block);
			hash->used = 0;
		}
	}
}

void sha256_final(sha256 *hash, uint8_t digest[32])
{
	const uint64_t bits = hash->length * 8;
	static const uint8_t pad[64] = { 0x80 };
	uint8_t length[8];

	sha256_update(hash, pad, hash->used < 56 ? 56 - hash->used : 120 - hash->used);
	for (int byte = 0; byte < 8; byte++)
		length[byte] = (uint8_t)(bits >> (56 - 8 * byte));
	sha256_update(hash, length, 8);
	for (int word = 0; word < 8; word++)
		for (int byte = 0; byte < 4; byte++)
			digest[word * 4 + byte] = (uint8_t)(hash->state[word] >> (24 - 8 * byte));
}

void sha256_of(const void *data, size_t size, uint8_t digest[32])
{
	sha256 hash;

	sha256_init(&hash);
	sha256_update(&hash, data, size);
	sha256_final(&hash, digest);
}

/* ---- the standards' vectors ---- */

static int hex_is(const uint8_t *bytes, const char *hex)
{
	for (size_t at = 0; hex[at * 2]; at++)
	{
		unsigned value = 0;

		for (int digit = 0; digit < 2; digit++)
		{
			const char c = hex[at * 2 + digit];

			value = value * 16 + (unsigned)(c <= '9' ? c - '0' : (c | 0x20) - 'a' + 10);
		}
		if (bytes[at] != value)
			return 0;
	}
	return 1;
}

const char *crypto_self_test(void)
{
	uint8_t key[16], block[32], digest[32];
	aes128 aes, other;

	/* FIPS-197, appendix C.1 */
	for (int byte = 0; byte < 16; byte++)
	{
		key[byte] = (uint8_t)byte;
		block[byte] = (uint8_t)(byte * 0x11);
	}
	aes128_init(&aes, key);
	aes128_encrypt_block(&aes, block, block);
	if (!hex_is(block, "69c4e0d86a7b0430d8cdb78070b4c55a"))
		return "AES-128 (FIPS-197 C.1)";
	/* FIPS-180-4: "abc" and the 448-bit message */
	sha256_of("abc", 3, digest);
	if (!hex_is(digest, "ba7816bf8f01cfea414140de5dae2223b00361a396177a9cb410ff61f20015ad"))
		return "SHA-256 (abc)";
	sha256_of("abcdbcdecdefdefgefghfghighijhijkijkljklmklmnlmnomnopnopq", 56, digest);
	if (!hex_is(digest, "248d6a61d20638b8e5c026930c3e6039a33ce45964ff2167f6ecedd419db06c1"))
		return "SHA-256 (448 bits)";
	/* IEEE P1619, XTS-AES-128 vector 1: both keys 0, sector 0, 32 bytes of 0 */
	memset(key, 0, sizeof(key));
	memset(block, 0, sizeof(block));
	aes128_init(&aes, key);
	aes128_init(&other, key);
	aes128_xts_encrypt(&aes, &other, 0, 32, block, 32);
	if (!hex_is(block, "917cf69ebd68b2ec9b9fe9a3eadda692cd43d2f59598ed858c02c2652fbf922e"))
		return "AES-128-XTS (IEEE P1619 vector 1)";
	return NULL;
}
