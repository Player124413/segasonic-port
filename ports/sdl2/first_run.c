/**
 *    Name:        first_run.c
 *    Description: the first run: the game's assets pack from the player's ROM set, found beside the game or
 *                 dropped on a window that asks for it
 *    Version:     0.1.0
 *    Created:     20-10-2026
 *    Modified:    20-10-2026
 *    Author:      Sebastian Toyos
 *
 *    license:BSD-3-Clause
 *    The window has none of the game's letters (they are in the ROM): it writes with Kenney Mini (Kenney
 *    Fonts, CC0; font.bmp beside the executable, tools/make_font.py), a 16 x 16 cell a character from 20h,
 *    each as wide as its lit columns. The ROM set is read once (runtime/src/extract.c): the pack is the
 *    assets alone, and the game never opens the ROM set again.
 */
#include "first_run.h"

#include "game/extract.h"

#include <SDL.h>

#include <stdio.h>
#include <string.h>

#define ROM_SET_FILE    "sonic.zip"
#define FONT_FILE       "font.bmp"
#define FONT_CELL       16
#define FONT_FIRST      0x20
#define FONT_LAST       0x7E
#define FONT_ROW        16
#define FONT_SPACING    2        /* pixels between two characters, past the lit columns */
#define SPACE_WIDTH     4
#define WINDOW_WIDTH    640
#define WINDOW_HEIGHT   448
#define LINE_GAP        8

typedef struct
{
	SDL_Texture *sheet;
	int widths[FONT_LAST - FONT_FIRST + 1];
} font;

/* The font's sheet and each character's width (its last lit column + 1): 0 when there is none */
static int load_font(SDL_Renderer *renderer, const char *base, font *letters)
{
	char path[1024];
	SDL_Surface *loaded, *sheet;
	int index;

	snprintf(path, sizeof(path), "%s" FONT_FILE, base ? base : "");
	if (!(loaded = SDL_LoadBMP(path)))
		return 0;
	sheet = SDL_ConvertSurfaceFormat(loaded, SDL_PIXELFORMAT_ARGB8888, 0);
	SDL_FreeSurface(loaded);
	if (!sheet)
		return 0;
	for (index = 0; index <= FONT_LAST - FONT_FIRST; index++)
	{
		const int left = (index % FONT_ROW) * FONT_CELL, top = (index / FONT_ROW) * FONT_CELL;
		int x, y;

		letters->widths[index] = 0;
		for (y = top; y < top + FONT_CELL && y < sheet->h; y++)
			for (x = left; x < left + FONT_CELL && x < sheet->w; x++)
				if (((const Uint32 *)((const Uint8 *)sheet->pixels + y * sheet->pitch))[x] & 0x00FFFFFFu)
					if (x - left + 1 > letters->widths[index])
						letters->widths[index] = x - left + 1;
	}
	letters->widths[0] = SPACE_WIDTH;
	SDL_SetColorKey(sheet, SDL_TRUE, SDL_MapRGB(sheet->format, 0, 0, 0));
	letters->sheet = SDL_CreateTextureFromSurface(renderer, sheet);
	SDL_FreeSurface(sheet);
	return letters->sheet != NULL;
}

static int text_width(const font *letters, const char *text, int scale)
{
	int width = 0;

	for (; *text; text++)
		if (*text >= FONT_FIRST && *text <= FONT_LAST)
			width += (letters->widths[*text - FONT_FIRST] + FONT_SPACING) * scale;
	return width > 0 ? width - FONT_SPACING * scale : 0;
}

/* A line of text centred across the window, its top at y, in the colour given */
static void draw_text(SDL_Renderer *renderer, const font *letters, const char *text, int y, int scale, SDL_Color colour)
{
	int x = (WINDOW_WIDTH - text_width(letters, text, scale)) / 2;

	SDL_SetTextureColorMod(letters->sheet, colour.r, colour.g, colour.b);
	for (; *text; text++)
	{
		SDL_Rect from, to;
		int index;

		if (*text < FONT_FIRST || *text > FONT_LAST)
			continue;
		index = *text - FONT_FIRST;
		from.x = (index % FONT_ROW) * FONT_CELL;
		from.y = (index / FONT_ROW) * FONT_CELL;
		from.w = letters->widths[index] > 0 ? letters->widths[index] : 1;
		from.h = FONT_CELL;
		to.x = x;
		to.y = y;
		to.w = from.w * scale;
		to.h = FONT_CELL * scale;
		if (*text != ' ')
			SDL_RenderCopy(renderer, letters->sheet, &from, &to);
		x += (letters->widths[index] + FONT_SPACING) * scale;
	}
}

static void draw_window(SDL_Renderer *renderer, const font *letters, const char *problem, int reading)
{
	const SDL_Color white = { 255, 255, 255, 255 }, blue = { 120, 170, 255, 255 }, red = { 255, 110, 90, 255 };
	int y = 120;

	SDL_SetRenderDrawColor(renderer, 16, 24, 64, 255);
	SDL_RenderClear(renderer);
	draw_text(renderer, letters, "SegaSonic The Hedgehog", y, 3, blue);
	y += FONT_CELL * 3 + LINE_GAP * 4;
	if (reading)
		draw_text(renderer, letters, "Reading the ROM set...", y, 2, white);
	else
	{
		draw_text(renderer, letters, "Drop the arcade ROM set here", y, 2, white);
		y += FONT_CELL * 2 + LINE_GAP;
		draw_text(renderer, letters, "sonic.zip: MAME's set, Japan, rev. C", y, 1, white);
		y += FONT_CELL + LINE_GAP;
		draw_text(renderer, letters, "The game takes its graphics and sounds from it, once.", y, 1, white);
	}
	if (problem && !reading)
		draw_text(renderer, letters, problem, y + FONT_CELL + LINE_GAP * 3, 1, red);
	SDL_RenderPresent(renderer);
}

/* sonic.zip in 'folder', when it is there and is the set */
static int found_beside(const char *folder, const char *pack)
{
	char path[1024];
	SDL_RWops *probe;

	if (!folder)
		return 0;
	snprintf(path, sizeof(path), "%s" ROM_SET_FILE, folder);
	if (!(probe = SDL_RWFromFile(path, "rb")))
		return 0;
	SDL_RWclose(probe);
	return game_data_extract(path, pack) == NULL;
}

int first_run(const char *base, const char *saves, const char *pack)
{
	SDL_Window *window;
	SDL_Renderer *renderer;
	font letters;
	char problem[192] = "";
	int done = 0, running = 1;

	if (found_beside(base, pack) || found_beside(saves, pack))
		return 1;
	window = SDL_CreateWindow("SegaSonic The Hedgehog", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
		WINDOW_WIDTH, WINDOW_HEIGHT, 0);
	renderer = window ? SDL_CreateRenderer(window, -1, 0) : NULL;
	if (!renderer || !load_font(renderer, base, &letters))
	{
		/* (no window or no letters: the message alone) */
		if (renderer)
			SDL_DestroyRenderer(renderer);
		if (window)
			SDL_DestroyWindow(window);
		SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_INFORMATION, "SegaSonic The Hedgehog",
			"The game takes its graphics and sounds from the arcade ROM set (sonic.zip: MAME's set, Japan, "
			"rev. C). Put sonic.zip beside the game and start it again.", NULL);
		return 0;
	}
	SDL_EventState(SDL_DROPFILE, SDL_ENABLE);
	draw_window(renderer, &letters, NULL, 0);
	while (running && !done)
	{
		SDL_Event event;

		if (!SDL_WaitEvent(&event))
			break;
		if (event.type == SDL_QUIT)
			running = 0;
		else if (event.type == SDL_DROPFILE)
		{
			const char *failed;

			draw_window(renderer, &letters, NULL, 1);
			failed = game_data_extract(event.drop.file, pack);
			SDL_free(event.drop.file);
			if (!failed)
				done = 1;
			else
			{
				/* (a file of the set missing: what the set is not; else what failed) */
				snprintf(problem, sizeof(problem), "%s%s", strncmp(failed, "no ", 3) == 0 ? "Not the set: " : "", failed);
				if (problem[0] >= 'a' && problem[0] <= 'z')
					problem[0] = (char)(problem[0] - 'a' + 'A');
				draw_window(renderer, &letters, problem, 0);
			}
		}
		else if (event.type == SDL_WINDOWEVENT && event.window.event == SDL_WINDOWEVENT_EXPOSED)
			draw_window(renderer, &letters, problem[0] ? problem : NULL, 0);
	}
	SDL_DestroyTexture(letters.sheet);
	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);
	return done;
}
