/**
 *    Name:        ecdsa.h
 *    Description: what the APK tool signs with: ECDSA on the curve P-256 with SHA-256, its nonce RFC 6979's
 *                 (deterministic, from HMAC-SHA256)
 *    Version:     0.1.0
 *    Created:     25-10-2026
 *    Modified:    25-10-2026
 *    Author:      Sebastian Toyos
 *
 *    license:BSD-3-Clause
 *    Written from FIPS-186-4 (ECDSA, P-256), SEC 1 (its points), RFC 6979 (the nonce) and RFC 2104 (HMAC);
 *    SHA-256 is the .nsp tool's (ports/switch/nsp/crypto.c). ecdsa_self_test checks them against RFC 4231's
 *    and RFC 6979's vectors. Numbers are 32 bytes, big-endian, as the standards write them.
 */
#ifndef SEGASONIC_APK_ECDSA_H
#define SEGASONIC_APK_ECDSA_H

#include <stddef.h>
#include <stdint.h>

#define ECDSA_SIGNATURE_SIZE 72    /* the most a DER signature takes: SEQUENCE of two INTEGERs of 33 bytes */

void hmac_sha256(const uint8_t *key, size_t key_size, const void *data, size_t size, uint8_t out[32]);

/* True for a private key the curve takes: from 1 to its order less 1 */
int ecdsa_key_valid(const uint8_t key[32]);

/* The public key of a private one: the point's x, then its y */
void ecdsa_public_key(const uint8_t key[32], uint8_t public_key[64]);

/* The signature of a SHA-256 'hash' by 'key': r and s */
void ecdsa_sign(const uint8_t key[32], const uint8_t hash[32], uint8_t r[32], uint8_t s[32]);

/* True when r and s are the signature of 'hash' by the public key's owner */
int ecdsa_verify(const uint8_t public_key[64], const uint8_t hash[32], const uint8_t r[32], const uint8_t s[32]);

/* r and s as X.509 and Java write them, a DER SEQUENCE of two INTEGERs: its size */
size_t ecdsa_der(const uint8_t r[32], const uint8_t s[32], uint8_t der[ECDSA_SIGNATURE_SIZE]);

/* The standards' vectors: NULL when they all hold, else which failed */
const char *ecdsa_self_test(void);

#endif
