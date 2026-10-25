/*
 * SegaSonic The Hedgehog — Android Port
 * Native JNI Bridge for LauncherActivity (libsegasonic_launcher.so)
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 * Handles legal first-run MAME `sonic.zip` CRC32 validation, `segasonic.assets`
 * ("SSAP" pack) extraction, integrity verification, and Sonic/Mighty/Ray portrait
 * extraction in the main Launcher process without loading SDL2 or game `.bss`.
 */

#include <jni.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include "game/data.h"
#include "game/extract.h"

#define ICON_SIZE            64
#define HEAD_SIZE            32
#define SPRITE_RECORDS_ADDR  0x130000u

static uint32_t read_u16_le(const uint8_t *p, uint32_t addr)
{
	return (uint32_t)p[addr] | ((uint32_t)p[addr + 1u] << 8);
}

static uint32_t decode_rom_color(const uint8_t *program, uint32_t address)
{
	const uint32_t value = read_u16_le(program, address);
	const uint32_t red   = ((value >> 12) & 1u) | ((value << 1) & 0x1eu);
	const uint32_t green = ((value >> 13) & 1u) | ((value >> 3) & 0x1eu);
	const uint32_t blue  = ((value >> 14) & 1u) | ((value >> 7) & 0x1eu);

	return 0xff000000u |
	       ((red   * 255u / 31u) << 16) |
	       ((green * 255u / 31u) << 8)  |
	       (blue   * 255u / 31u);
}

static bool extract_character_icon(uint32_t out_argb[ICON_SIZE * ICON_SIZE],
	const uint8_t *sprites, size_t sprites_size, const uint8_t *program)
{
	static const struct
	{
		uint16_t frame;
		uint32_t palette;
		int x, y;
	} heads[3] = {
		{ 0x90c8u, 0x10738eu, 0,             0         }, /* Sonic */
		{ 0x90d0u, 0x1073aeu, HEAD_SIZE,     0         }, /* Mighty */
		{ 0x90d8u, 0x1073ceu, HEAD_SIZE / 2, HEAD_SIZE }  /* Ray */
	};

	memset(out_argb, 0, ICON_SIZE * ICON_SIZE * sizeof(uint32_t));

	for (size_t i = 0; i < 3; i++)
	{
		const uint32_t rec = SPRITE_RECORDS_ADDR + heads[i].frame;
		const int w = (int)read_u16_le(program, rec);
		const int h = (int)(read_u16_le(program, rec + 2u) & 0xffu);
		const uint32_t bank_bits = read_u16_le(program, rec + 4u);
		const uint32_t word = (((bank_bits & 0x0800u) >> 11) | ((bank_bits & 0x4000u) >> 13)) << 20
			| ((read_u16_le(program, rec + 2u) & 0xf000u) << 4)
			| read_u16_le(program, rec + 6u);
		const int words_per_row = w >> 3;

		if (w != HEAD_SIZE || h != HEAD_SIZE)
			return false;
		if (((size_t)word + (size_t)h * (size_t)words_per_row) * 4u > sprites_size)
			return false;

		for (int y = 0; y < HEAD_SIZE; y++)
		{
			for (int x = 0; x < HEAD_SIZE; x++)
			{
				const size_t byte_idx = ((size_t)word + (size_t)y * (size_t)words_per_row + (size_t)(x >> 3)) * 4u
					+ (size_t)((x & 7) >> 1);
				const uint8_t b = sprites[byte_idx];
				const uint8_t pen = (x & 1) ? (b & 0x0fu) : (b >> 4);

				if (pen != 0u)
				{
					out_argb[(heads[i].y + y) * ICON_SIZE + heads[i].x + x] =
						decode_rom_color(program, heads[i].palette + (uint32_t)pen * 2u);
				}
			}
		}
	}
	return true;
}

JNIEXPORT jboolean JNICALL
Java_io_github_segasonic_port_LauncherNative_nativeHasValidAssets(
	JNIEnv *env, jclass clazz, jstring j_assets_path)
{
	const char *path;
	FILE *fp;
	uint8_t magic[4] = { 0, 0, 0, 0 };
	long sz = 0;
	bool valid = false;

	(void)clazz;
	if (!j_assets_path)
		return JNI_FALSE;

	path = (*env)->GetStringUTFChars(env, j_assets_path, NULL);
	if (!path)
		return JNI_FALSE;

	fp = fopen(path, "rb");
	if (fp)
	{
		if (fread(magic, 1, 4, fp) == 4 && memcmp(magic, "SSAP", 4) == 0)
		{
			if (fseek(fp, 0, SEEK_END) == 0)
			{
				sz = ftell(fp);
				valid = (sz > 1024 * 1024);
			}
		}
		fclose(fp);
	}

	(*env)->ReleaseStringUTFChars(env, j_assets_path, path);
	return valid ? JNI_TRUE : JNI_FALSE;
}

JNIEXPORT jstring JNICALL
Java_io_github_segasonic_port_LauncherNative_nativeVerifyRomSource(
	JNIEnv *env, jclass clazz, jstring j_source_path)
{
	const char *source;
	const char *err;

	(void)clazz;
	if (!j_source_path)
		return (*env)->NewStringUTF(env, "No ROM file path provided.");

	source = (*env)->GetStringUTFChars(env, j_source_path, NULL);
	if (!source)
		return (*env)->NewStringUTF(env, "Out of memory reading path.");

	err = game_rom_set_load(source);
	(*env)->ReleaseStringUTFChars(env, j_source_path, source);

	return err ? (*env)->NewStringUTF(env, err) : NULL;
}

JNIEXPORT jstring JNICALL
Java_io_github_segasonic_port_LauncherNative_nativeExtractAssets(
	JNIEnv *env, jclass clazz, jstring j_source_path, jstring j_dest_pack_path)
{
	const char *source;
	const char *dest;
	const char *err;

	(void)clazz;
	if (!j_source_path || !j_dest_pack_path)
		return (*env)->NewStringUTF(env, "Invalid source or destination path.");

	source = (*env)->GetStringUTFChars(env, j_source_path, NULL);
	dest = (*env)->GetStringUTFChars(env, j_dest_pack_path, NULL);
	if (!source || !dest)
	{
		if (source) (*env)->ReleaseStringUTFChars(env, j_source_path, source);
		if (dest)   (*env)->ReleaseStringUTFChars(env, j_dest_pack_path, dest);
		return (*env)->NewStringUTF(env, "Out of memory.");
	}

	err = game_data_extract(source, dest);

	(*env)->ReleaseStringUTFChars(env, j_source_path, source);
	(*env)->ReleaseStringUTFChars(env, j_dest_pack_path, dest);

	return err ? (*env)->NewStringUTF(env, err) : NULL;
}

JNIEXPORT jintArray JNICALL
Java_io_github_segasonic_port_LauncherNative_nativeExtractIconPixels(
	JNIEnv *env, jclass clazz, jstring j_assets_path)
{
	const char *path;
	game_data_result res;
	uint32_t pixels[ICON_SIZE * ICON_SIZE];
	jintArray arr = NULL;

	(void)clazz;
	if (!j_assets_path)
		return NULL;

	path = (*env)->GetStringUTFChars(env, j_assets_path, NULL);
	if (!path)
		return NULL;

	res = game_data_load(path);
	(*env)->ReleaseStringUTFChars(env, j_assets_path, path);

	if (res != GAME_DATA_LOADED)
		return NULL;

	if (!extract_character_icon(pixels, game_sprites, GAME_SPRITES_SIZE, game_program))
		return NULL;

	arr = (*env)->NewIntArray(env, ICON_SIZE * ICON_SIZE);
	if (arr)
		(*env)->SetIntArrayRegion(env, arr, 0, ICON_SIZE * ICON_SIZE, (const jint *)pixels);
	return arr;
}
