/**
 *    Name:        sign.h
 *    Description: the APK's signature: APK Signature Scheme v2's signing block, by the player's own key
 *                 (sign.c)
 *    Version:     0.1.0
 *    Created:     25-10-2026
 *    Modified:    25-10-2026
 *    Author:      Sebastian Toyos
 *
 *    license:BSD-3-Clause
 *    Written from Android's documentation of the scheme (source.android.com, APK Signature Scheme v2) and
 *    from what apksigner writes and checks (none of its code).
 */
#ifndef SEGASONIC_APK_SIGN_H
#define SEGASONIC_APK_SIGN_H

#include "key.h"

#include <stddef.h>
#include <stdint.h>

/* The signing block of the APK in 'apk' as it is without one: its entries up to 'central', its central
   directory, its end record (its last 22 bytes). It goes before the central directory, whose offset in the
   end record then grows by its size. Freed by the caller (free); NULL when there is no memory, or when the
   signature fails its own check */
uint8_t *sign_block(const uint8_t *apk, size_t size, size_t central, const apk_key *key, size_t *block_size);

#endif
