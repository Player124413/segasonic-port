/**
 *    Name:        key.h
 *    Description: the player's own key, which signs every APK the tool makes: made once with the system's
 *                 random generator and kept in the player's folder, with its certificate (key.c)
 *    Version:     0.1.0
 *    Created:     25-10-2026
 *    Modified:    25-10-2026
 *    Author:      Sebastian Toyos
 *
 *    license:BSD-3-Clause
 *    Android installs an APK over the one installed only when both are signed with the same certificate, so
 *    the key and its certificate are made once and kept: on Windows in %APPDATA%\SegaSonic, on macOS in
 *    ~/Library/Application Support/SegaSonic, elsewhere in $XDG_CONFIG_HOME/SegaSonic (~/.config/SegaSonic).
 */
#ifndef SEGASONIC_APK_KEY_H
#define SEGASONIC_APK_KEY_H

#include <stddef.h>
#include <stdint.h>

#define KEY_FILE              "apk_key.pk8"         /* the private key, PKCS #8 (DER) */
#define KEY_CERTIFICATE_FILE  "apk_key.x509.der"    /* its certificate, X.509 (DER) */
#define KEY_PUBLIC_SIZE       91                    /* an X.509 SubjectPublicKeyInfo of P-256 */
#define KEY_CERTIFICATE_ROOM  512

typedef struct
{
	uint8_t private_key[32];
	uint8_t public_key[64];                       /* the point's x, then its y */
	uint8_t public_info[KEY_PUBLIC_SIZE];         /* the public key as X.509 and APK signatures write it */
	uint8_t certificate[KEY_CERTIFICATE_ROOM];    /* self-signed */
	size_t certificate_size;
	int made;                                     /* made now, not kept from before */
} apk_key;

enum
{
	KEY_NONE = 0,         /* none kept yet */
	KEY_KEPT = 1,         /* one kept, which the tool reads */
	KEY_UNREADABLE = -1   /* one there that the tool cannot read */
};

/* The folder the key is kept in, with its separator: false when the system tells no folder of the user's */
int key_folder(char *folder, size_t size);

/* The key kept, or one made and kept now when there is none: NULL, or what failed, with the file or the
   folder it failed on in 'path'. A key there that cannot be read is never replaced */
const char *key_obtain(apk_key *key, char *path, size_t size);

/* What the folder holds (KEY_NONE, KEY_KEPT, KEY_UNREADABLE) */
int key_state(void);

/* The key's bytes wiped */
void key_forget(apk_key *key);

#endif
