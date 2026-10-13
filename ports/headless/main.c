/**
 *    Name:        main.c
 *    Description: runs the game with no screen or sound, with the controls a script drives, and writes a
 *                 hash of the board state (and of the frame the video draws) after every frame, in the
 *                 format tools/reference/s32ref uses
 *    Version:     0.1.0
 *    Created:     11-10-2026
 *    Modified:    13-10-2026
 *    Author:      Sebastian Toyos
 *
 *    license:BSD-3-Clause
 *    Usage: sonic_headless [--frames N] [--hashes FILE] [--script FILE]
 *                          [--dump-frame N --dump-dir DIR] [--video-hashes FILE] [--video-dump N ... --video-dir DIR]
 *                          [--sound-hashes FILE]
 *    The game's data is the one the executable carries (game/data.h).
 *    --dump-frame saves the board's memories at the end of that frame, in the files tools/reference/s32ref
 *    writes, so the two compare byte by byte. --video-hashes and --video-dump draw every frame with the
 *    board's video (runtime/src/video.c): the hash of each frame, and frames as BMP. --sound-hashes runs the
 *    sound board too (the decompiled sound program and the chips, runtime/src/sound_cpu.c and
 *    sound_chips.c), and writes for each frame the hash of its samples and of the sound board's memories.
 */
#include "s32/machine.h"
#include "s32/program.h"
#include "s32/script.h"
#include "s32/video.h"
#include "sound/chips.h"
#include "game/data.h"

#include <inttypes.h>
#include <setjmp.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define USAGE "Usage: sonic_headless [--frames N] [--hashes FILE] [--script FILE]" \
	" [--dump-frame N --dump-dir DIR] [--video-hashes FILE] [--video-dump N ... --video-dir DIR]" \
	" [--sound-hashes FILE]\n"
#define MAX_VIDEO_DUMPS 64

static jmp_buf finished;
static uint64_t frames_wanted = 600;
static FILE *hashes;
static s32_script *script;
static uint64_t dump_frame = UINT64_MAX;
static const char *dump_dir;
static FILE *video_hashes;
static uint64_t video_dumps[MAX_VIDEO_DUMPS];
static int video_dump_count;
static const char *video_dir;
static FILE *sound_hashes;

static void write_file(const char *name, const void *data, size_t size)
{
	char path[1024];
	FILE *output;

	snprintf(path, sizeof(path), "%s/%s", dump_dir, name);
	output = fopen(path, "wb");
	if (!output)
	{
		fprintf(stderr, "%s: cannot write\n", path);
		return;
	}
	fwrite(data, 1, size, output);
	fclose(output);
}

static void dump_state(void)
{
	write_file("work_ram.bin", s32_board.work_ram, sizeof(s32_board.work_ram));
	write_file("video_ram.bin", s32_board.video_ram, sizeof(s32_board.video_ram));
	write_file("sprite_ram.bin", s32_board.sprite_ram, sizeof(s32_board.sprite_ram));
	write_file("palette.bin", s32_board.palette, sizeof(s32_board.palette));
	write_file("mixer.bin", s32_board.mixer, sizeof(s32_board.mixer));
	write_file("shared_ram.bin", s32_board.shared_ram, sizeof(s32_board.shared_ram));
	write_file("eeprom.bin", s32_board.eeprom.data, sizeof(s32_board.eeprom.data));
}

/* The last frame the video drew, as a 24-bit BMP */
static void write_bmp(const char *path)
{
	int width;
	const uint32_t *pixels = s32_video_frame(&width);
	const uint32_t row = ((uint32_t)width * 3 + 3) & ~3u;
	const uint32_t size = 54 + row * S32_VIDEO_HEIGHT;
	uint8_t *file = (uint8_t *)calloc(size, 1);
	FILE *output;

	if (!file)
		return;
	for (int byte = 0; byte < 4; byte++)
	{
		file[2 + byte] = (uint8_t)(size >> (8 * byte));
		file[18 + byte] = (uint8_t)((uint32_t)width >> (8 * byte));
		file[22 + byte] = (uint8_t)((uint32_t)S32_VIDEO_HEIGHT >> (8 * byte));
	}
	file[0] = 'B';
	file[1] = 'M';
	file[10] = 54;
	file[14] = 40;
	file[26] = 1;
	file[28] = 24;
	for (int y = 0; y < S32_VIDEO_HEIGHT; y++)
		for (int x = 0; x < width; x++)
		{
			const uint32_t pixel = pixels[y * S32_VIDEO_MAX_WIDTH + x];
			uint8_t *out = &file[54 + (uint32_t)(S32_VIDEO_HEIGHT - 1 - y) * row + (uint32_t)x * 3];

			out[0] = (uint8_t)pixel;
			out[1] = (uint8_t)(pixel >> 8);
			out[2] = (uint8_t)(pixel >> 16);
		}
	output = fopen(path, "wb");
	if (output)
	{
		fwrite(file, 1, size, output);
		fclose(output);
	}
	else
		fprintf(stderr, "%s: cannot write\n", path);
	free(file);
}

static void on_irq_changed(bool asserted)
{
	program_irq_line(asserted);
}

static void on_log(const char *message)
{
	fprintf(stderr, "[s32] %s\n", message);
}

static void on_frame_done(void)
{
	if (hashes)
		fprintf(hashes, "%" PRIu64 " %016" PRIx64 "\n", program_frames() - 1, s32_state_hash());
	if (dump_dir && program_frames() - 1 == dump_frame)
		dump_state();
	if (video_hashes)
		fprintf(video_hashes, "%" PRIu64 " %016" PRIx64 "\n", program_frames() - 1, s32_video_frame_hash());
	if (sound_hashes)
	{
		s32_board.time_passes(s32_board.cycles);
		sound_chips_end_frame(s32_board.cycles);
		fprintf(sound_hashes, "%" PRIu64 " %016" PRIx64 " %016" PRIx64 "\n", program_frames() - 1,
			sound_chips_frame_hash(), sound_board_state_hash());
	}
	for (int index = 0; index < video_dump_count && video_dir; index++)
		if (video_dumps[index] == program_frames() - 1)
		{
			char path[1024];

			snprintf(path, sizeof(path), "%s/frame_%" PRIu64 ".bmp", video_dir, video_dumps[index]);
			write_bmp(path);
		}
	if (program_frames() >= frames_wanted)
		longjmp(finished, 1);
}

static void on_controls_update(void)
{
	/* VBLANK of the board's frame is starting */
	s32_script_apply(script, s32_board.frame);
}

int main(int argc, char **argv)
{
	const char *hashes_path = NULL;
	const char *script_path = NULL;
	const char *video_hashes_path = NULL;
	const char *sound_hashes_path = NULL;
	bool frames_given = false;

	for (int index = 1; index < argc; index++)
	{
		if (strcmp(argv[index], "--frames") == 0 && index + 1 < argc)
		{
			frames_wanted = strtoull(argv[++index], NULL, 10);
			frames_given = true;
		}
		else if (strcmp(argv[index], "--hashes") == 0 && index + 1 < argc)
			hashes_path = argv[++index];
		else if (strcmp(argv[index], "--script") == 0 && index + 1 < argc)
			script_path = argv[++index];
		else if (strcmp(argv[index], "--dump-frame") == 0 && index + 1 < argc)
			dump_frame = strtoull(argv[++index], NULL, 10);
		else if (strcmp(argv[index], "--dump-dir") == 0 && index + 1 < argc)
			dump_dir = argv[++index];
		else if (strcmp(argv[index], "--video-hashes") == 0 && index + 1 < argc)
			video_hashes_path = argv[++index];
		else if (strcmp(argv[index], "--video-dump") == 0 && index + 1 < argc && video_dump_count < MAX_VIDEO_DUMPS)
			video_dumps[video_dump_count++] = strtoull(argv[++index], NULL, 10);
		else if (strcmp(argv[index], "--video-dir") == 0 && index + 1 < argc)
			video_dir = argv[++index];
		else if (strcmp(argv[index], "--sound-hashes") == 0 && index + 1 < argc)
			sound_hashes_path = argv[++index];
		else
		{
			fprintf(stderr, USAGE);
			return 1;
		}
	}
	if (script_path)
	{
		char error[256];

		script = s32_script_load(script_path, error, sizeof(error));
		if (!script)
		{
			fprintf(stderr, "%s\n", error);
			return 1;
		}
		if (!frames_given && s32_script_frames(script))
			frames_wanted = s32_script_frames(script);
	}
	if (hashes_path)
		hashes = fopen(hashes_path, "w");
	if (video_hashes_path || video_dump_count)
	{
		s32_video_init(game_tile_rom, GAME_TILE_ROM_SIZE, game_sprite_rom, GAME_SPRITE_ROM_SIZE);
		if (video_hashes_path)
			video_hashes = fopen(video_hashes_path, "w");
	}
	if (sound_hashes_path)
	{
		s32_board.sound_rom = game_sound_rom;
		sound_chips_attach();
		sound_cpu_attach();
		sound_hashes = fopen(sound_hashes_path, "w");
	}

	s32_board.irq_changed = on_irq_changed;
	s32_board.controls_update = on_controls_update;
	s32_board.log = on_log;
	s32_board.frame_timing = true;
	s32_init(game_program_rom);
	program_reset();
	program_frame_done = on_frame_done;

	if (!setjmp(finished))
		program_run();

	if (hashes)
		fclose(hashes);
	if (video_hashes)
		fclose(video_hashes);
	if (sound_hashes)
		fclose(sound_hashes);
	s32_script_free(script);
	printf("frames:            %" PRIu64 "\n", program_frames());
	printf("interrupts taken:  %u\n", program_interrupts());
	printf("sprite lists:      %u\n", s32_board.sprite_lists);
	printf("sound requests:    %u\n", s32_board.sound_requests);
	printf("unmapped reads:    %u\n", s32_board.unmapped_reads);
	printf("unmapped writes:   %u\n", s32_board.unmapped_writes);
	printf("eeprom written:    %s\n", s32_board.eeprom.dirty ? "yes" : "no");
	return 0;
}
