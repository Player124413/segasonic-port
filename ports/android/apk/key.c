/**
 *    Name:        key.c
 *    Description: the player's own signing key: its folder, made once with the system's random generator,
 *                 kept as PKCS #8 with a self-signed X.509 certificate of it, read back for every APK
 *    Version:     0.1.0
 *    Created:     25-10-2026
 *    Modified:    25-10-2026
 *    Author:      Sebastian Toyos
 *
 *    license:BSD-3-Clause
 *    The key is ECDSA's on P-256 (ecdsa.c), drawn from the system's random generator (BCryptGenRandom on
 *    Windows, getrandom on Linux, /dev/urandom elsewhere) the first time the tool signs an APK, and written
 *    as apksigner takes one (--key apk_key.pk8 --cert apk_key.x509.der): a PKCS #8 PrivateKeyInfo with its
 *    public key, and an X.509 certificate signed by it, CN=SegaSonic, valid from 25-10-2026 to the end of
 *    9999 (Android does not look at the dates). The certificate depends on the key alone (its serial from
 *    the public key's SHA-256, its signature RFC 6979's), and is kept beside it all the same, so that an
 *    APK made by another version of the tool is still signed by the certificate of the first. The key
 *    file is made only when there is none, and only for the user (0600 elsewhere than on Windows); a key
 *    there that cannot be read stops the tool, and is never replaced: an APK signed by another key would
 *    not install over the last one. Its bytes are wiped after use.
 */
#include "key.h"
#include "ecdsa.h"
#include "crypto.h"

#ifdef _WIN32
#include <windows.h>
#include <bcrypt.h>
#include <direct.h>
#include <fcntl.h>
#include <io.h>
#else
#include <errno.h>
#include <fcntl.h>
#include <unistd.h>
#ifdef __linux__
#include <sys/random.h>
#endif
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>

#define KEY_PATH_SIZE  1200
#define KEY_FILE_SIZE  138

/* PKCS #8's PrivateKeyInfo of a P-256 key: version 0, id-ecPublicKey and prime256v1, then SEC 1's
   ECPrivateKey (version 1, the private key, [1] the public key) */
static const uint8_t key_head[36] = {
	0x30, 0x81, 0x87, 0x02, 0x01, 0x00, 0x30, 0x13, 0x06, 0x07, 0x2A, 0x86, 0x48, 0xCE, 0x3D, 0x02, 0x01, 0x06,
	0x08, 0x2A, 0x86, 0x48, 0xCE, 0x3D, 0x03, 0x01, 0x07, 0x04, 0x6D, 0x30, 0x6B, 0x02, 0x01, 0x01, 0x04, 0x20,
};
static const uint8_t key_middle[6] = { 0xA1, 0x44, 0x03, 0x42, 0x00, 0x04 };

/* X.509's SubjectPublicKeyInfo: id-ecPublicKey and prime256v1, the point uncompressed (04, x, y) */
static const uint8_t public_head[27] = {
	0x30, 0x59, 0x30, 0x13, 0x06, 0x07, 0x2A, 0x86, 0x48, 0xCE, 0x3D, 0x02, 0x01, 0x06, 0x08, 0x2A, 0x86, 0x48,
	0xCE, 0x3D, 0x03, 0x01, 0x07, 0x03, 0x42, 0x00, 0x04,
};

/* The certificate's fixed parts: ecdsa-with-SHA256, the name CN=SegaSonic, the validity */
static const uint8_t signature_algorithm[12] = {
	0x30, 0x0A, 0x06, 0x08, 0x2A, 0x86, 0x48, 0xCE, 0x3D, 0x04, 0x03, 0x02,
};
static const uint8_t certificate_name[22] = {
	0x30, 0x14, 0x31, 0x12, 0x30, 0x10, 0x06, 0x03, 0x55, 0x04, 0x03, 0x0C, 0x09,
	'S', 'e', 'g', 'a', 'S', 'o', 'n', 'i', 'c',
};
static const uint8_t certificate_validity[34] = {
	0x30, 0x20,
	0x17, 0x0D, '2', '6', '1', '0', '2', '5', '0', '0', '0', '0', '0', '0', 'Z',         /* UTCTime */
	0x18, 0x0F, '9', '9', '9', '9', '1', '2', '3', '1', '2', '3', '5', '9', '5', '9', 'Z',    /* GeneralizedTime */
};

/* ---- files ---- */

static int exists(const char *path)
{
	FILE *probe = fopen(path, "rb");

	if (probe)
		fclose(probe);
	return probe != NULL;
}

/* A file of at most 'room' bytes: its size, or 0 when it cannot be read or is longer */
static size_t read_small(const char *path, uint8_t *data, size_t room)
{
	FILE *input = fopen(path, "rb");
	size_t got;

	if (!input)
		return 0;
	got = fread(data, 1, room, input);
	if (got == room && fgetc(input) != EOF)
		got = 0;
	fclose(input);
	return got;
}

/* A file of the user's alone: 'only_new' when it must not be there yet. False when it is not written
   whole (and then it is not there) */
static int write_own(const char *path, const uint8_t *data, size_t size, int only_new)
{
#ifdef _WIN32
	const int file = _open(path, _O_WRONLY | _O_CREAT | _O_BINARY | (only_new ? _O_EXCL : _O_TRUNC),
		_S_IREAD | _S_IWRITE);
	int written;

	if (file < 0)
		return 0;
	written = _write(file, data, (unsigned)size) == (int)size;
	written = _close(file) == 0 && written;
#else
	const int file = open(path, O_WRONLY | O_CREAT | (only_new ? O_EXCL : O_TRUNC), 0600);
	size_t done = 0;
	int written;

	if (file < 0)
		return 0;
	while (done < size)
	{
		const ssize_t wrote = write(file, data + done, size - done);

		if (wrote < 0 && errno == EINTR)
			continue;
		if (wrote <= 0)
			break;
		done += (size_t)wrote;
	}
	written = close(file) == 0 && done == size;
#endif
	if (!written)
		remove(path);
	return written;
}

static int folder_exists(const char *folder)
{
	char path[KEY_PATH_SIZE];
	struct stat status;
	size_t length;

	snprintf(path, sizeof(path), "%s", folder);
	length = strlen(path);
	/* (without its last separator, which Windows' stat does not take) */
	if (length > 1 && (path[length - 1] == '/' || path[length - 1] == '\\'))
		path[length - 1] = '\0';
	return stat(path, &status) == 0 && (status.st_mode & S_IFMT) == S_IFDIR;
}

/* The folder, and each above it that is not there: true when it is there */
static int make_folder(const char *folder)
{
	char path[KEY_PATH_SIZE];

	snprintf(path, sizeof(path), "%s", folder);
	for (char *at = path + 1; *at; at++)
		if (*at == '/' || *at == '\\')
		{
			const char separator = *at;

			*at = '\0';
			/* (one that is there already fails, and that is all) */
#ifdef _WIN32
			_mkdir(path);
#else
			mkdir(path, 0700);
#endif
			*at = separator;
		}
	return folder_exists(folder);
}

int key_folder(char *folder, size_t size)
{
#ifdef _WIN32
	const char *const roaming = getenv("APPDATA");

	if (!roaming || !roaming[0])
		return 0;
	return snprintf(folder, size, "%s\\SegaSonic\\", roaming) < (int)size;
#elif defined(__APPLE__)
	const char *const home = getenv("HOME");

	if (!home || !home[0])
		return 0;
	return snprintf(folder, size, "%s/Library/Application Support/SegaSonic/", home) < (int)size;
#else
	const char *const config = getenv("XDG_CONFIG_HOME"), *const home = getenv("HOME");

	/* (XDG's: a relative one is not taken) */
	if (config && config[0] == '/')
		return snprintf(folder, size, "%s/SegaSonic/", config) < (int)size;
	if (!home || !home[0])
		return 0;
	return snprintf(folder, size, "%s/.config/SegaSonic/", home) < (int)size;
#endif
}

/* ---- the key ---- */

static int random_bytes(uint8_t *out, size_t size)
{
#ifdef _WIN32
	return BCRYPT_SUCCESS(BCryptGenRandom(NULL, out, (ULONG)size, BCRYPT_USE_SYSTEM_PREFERRED_RNG));
#else
	FILE *source;
	size_t got = 0;

#ifdef __linux__
	while (got < size)
	{
		const ssize_t drawn = getrandom(out + got, size - got, 0);

		if (drawn < 0 && errno == EINTR)
			continue;
		if (drawn <= 0)
			break;
		got += (size_t)drawn;
	}
	if (got == size)
		return 1;
#endif
	if (!(source = fopen("/dev/urandom", "rb")))
		return 0;
	got = fread(out, 1, size, source);
	fclose(source);
	return got == size;
#endif
}

/* The public key, from the private one, as X.509 writes it too */
static void fill_public(apk_key *key)
{
	ecdsa_public_key(key->private_key, key->public_key);
	memcpy(key->public_info, public_head, sizeof(public_head));
	memcpy(key->public_info + sizeof(public_head), key->public_key, 64);
}

/* The key file as key_head says it, holding the public key of its private one */
static int read_key(const char *path, apk_key *key)
{
	uint8_t file[KEY_FILE_SIZE + 1];
	const size_t size = read_small(path, file, sizeof(file));
	int good = size == KEY_FILE_SIZE && memcmp(file, key_head, sizeof(key_head)) == 0
		&& memcmp(file + 36 + 32, key_middle, sizeof(key_middle)) == 0 && ecdsa_key_valid(file + 36);

	if (good)
	{
		memcpy(key->private_key, file + 36, 32);
		fill_public(key);
		good = memcmp(file + 36 + 32 + 6, key->public_key, 64) == 0;
	}
	memset(file, 0, sizeof(file));
	return good;
}

static void key_file(const apk_key *key, uint8_t file[KEY_FILE_SIZE])
{
	memcpy(file, key_head, sizeof(key_head));
	memcpy(file + 36, key->private_key, 32);
	memcpy(file + 36 + 32, key_middle, sizeof(key_middle));
	memcpy(file + 36 + 32 + 6, key->public_key, 64);
}

/* ---- the certificate ---- */

/* An element of DER: its tag, its length, its 'content' (which may be where the element goes): its size */
static size_t der(uint8_t *out, uint8_t tag, const uint8_t *content, size_t size)
{
	size_t header = 2;

	out[0] = tag;
	if (size < 0x80)
		out[1] = (uint8_t)size;
	else if (size < 0x100)
	{
		out[1] = 0x81;
		out[2] = (uint8_t)size;
		header = 3;
	}
	else
	{
		out[1] = 0x82;
		out[2] = (uint8_t)(size >> 8);
		out[3] = (uint8_t)size;
		header = 4;
	}
	memmove(out + header, content, size);
	return header + size;
}

static size_t put(uint8_t *out, const uint8_t *bytes, size_t size)
{
	memcpy(out, bytes, size);
	return size;
}

/* The certificate (X.509 v1, RFC 5280): the TBSCertificate (serial, algorithm, issuer, validity, subject,
   public key), the algorithm, the signature of the first by the key */
static void make_certificate(apk_key *key)
{
	uint8_t body[KEY_CERTIFICATE_ROOM], tbs[KEY_CERTIFICATE_ROOM], hash[32], r[32], s[32];
	uint8_t signature[ECDSA_SIGNATURE_SIZE];
	size_t at = 0, tbs_size, signature_size;

	sha256_of(key->public_info, KEY_PUBLIC_SIZE, hash);
	hash[0] = (uint8_t)((hash[0] & 0x7F) | 0x40);    /* (a positive serial of 16 bytes) */
	at += der(body + at, 0x02, hash, 16);
	at += put(body + at, signature_algorithm, sizeof(signature_algorithm));
	at += put(body + at, certificate_name, sizeof(certificate_name));
	at += put(body + at, certificate_validity, sizeof(certificate_validity));
	at += put(body + at, certificate_name, sizeof(certificate_name));
	at += put(body + at, key->public_info, KEY_PUBLIC_SIZE);
	tbs_size = der(tbs, 0x30, body, at);
	sha256_of(tbs, tbs_size, hash);
	ecdsa_sign(key->private_key, hash, r, s);
	signature_size = ecdsa_der(r, s, signature);
	at = put(body, tbs, tbs_size);
	at += put(body + at, signature_algorithm, sizeof(signature_algorithm));
	/* a BIT STRING: its length (below 128), no unused bits, the signature */
	body[at++] = 0x03;
	body[at++] = (uint8_t)(1 + signature_size);
	body[at++] = 0;
	at += put(body + at, signature, signature_size);
	key->certificate_size = der(key->certificate, 0x30, body, at);
}

/* A certificate kept: a DER SEQUENCE holding the key's public key */
static int certificate_holds(const uint8_t *certificate, size_t size, const apk_key *key)
{
	if (size < KEY_PUBLIC_SIZE || certificate[0] != 0x30)
		return 0;
	for (size_t at = 0; at + KEY_PUBLIC_SIZE <= size; at++)
		if (memcmp(certificate + at, key->public_info, KEY_PUBLIC_SIZE) == 0)
			return 1;
	return 0;
}

/* ---- the key, kept ---- */

const char *key_obtain(apk_key *key, char *path, size_t size)
{
	char folder[KEY_PATH_SIZE];
	uint8_t file[KEY_FILE_SIZE];

	memset(key, 0, sizeof(*key));
	path[0] = '\0';
	if (!key_folder(folder, sizeof(folder)))
		return "There is no folder of yours for the signing key";
	snprintf(path, size, "%s" KEY_FILE, folder);
	if (exists(path))
	{
		if (!read_key(path, key))
			return "The signing key cannot be read";
	}
	else
	{
		/* made once: a number from 1 to the curve's order less 1, drawn again in the rare case it is not */
		if (!make_folder(folder))
		{
			snprintf(path, size, "%s", folder);
			return "The signing key's folder cannot be made";
		}
		do
		{
			if (!random_bytes(key->private_key, sizeof(key->private_key)))
				return "The system's random generator failed";
		} while (!ecdsa_key_valid(key->private_key));
		fill_public(key);
		key_file(key, file);
		key->made = write_own(path, file, sizeof(file), 1);
		memset(file, 0, sizeof(file));
		if (!key->made)
			return "The signing key cannot be kept";
	}

	/* its certificate: the one kept, else made now (for a key made now, always) */
	snprintf(path, size, "%s" KEY_CERTIFICATE_FILE, folder);
	if (!key->made && exists(path))
	{
		key->certificate_size = read_small(path, key->certificate, sizeof(key->certificate));
		if (!key->certificate_size)
			return "The signing key's certificate cannot be read";
		if (!certificate_holds(key->certificate, key->certificate_size, key))
			return "The signing key's certificate is not the key's";
	}
	else
	{
		make_certificate(key);
		if (!write_own(path, key->certificate, key->certificate_size, 0))
			return "The signing key's certificate cannot be kept";
	}
	snprintf(path, size, "%s" KEY_FILE, folder);
	return NULL;
}

int key_state(void)
{
	char folder[KEY_PATH_SIZE], path[KEY_PATH_SIZE + 32];
	apk_key key;
	int state = KEY_UNREADABLE;

	if (!key_folder(folder, sizeof(folder)))
		return KEY_UNREADABLE;
	snprintf(path, sizeof(path), "%s" KEY_FILE, folder);
	if (!exists(path))
		return KEY_NONE;
	memset(&key, 0, sizeof(key));
	if (read_key(path, &key))
	{
		snprintf(path, sizeof(path), "%s" KEY_CERTIFICATE_FILE, folder);
		key.certificate_size = exists(path) ? read_small(path, key.certificate, sizeof(key.certificate)) : 0;
		if (!exists(path) || certificate_holds(key.certificate, key.certificate_size, &key))
			state = KEY_KEPT;
	}
	key_forget(&key);
	return state;
}

void key_forget(apk_key *key)
{
	memset(key, 0, sizeof(*key));
}
