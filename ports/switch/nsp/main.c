/**
 *    Name:        main.c
 *    Description: SegaSonic NSP: the Switch game as an .nsp to install, made on the player's computer from the
 *                 game's Switch build, the player's ROM set and the player's console keys
 *    Version:     0.2.0
 *    Created:     23-10-2026
 *    Modified:    25-10-2026
 *    Author:      Sebastian Toyos
 *
 *    license:BSD-3-Clause
 *    Usage: SegaSonic NSP [--keys FILE] [--rom FILE] [--switch FOLDER] [--out FILE] [--icon FILE]
 *
 *    The .nsp is made in make_nsp.c. By default every file is beside the tool (prod.keys also in .switch in
 *    the user's folder), the game's Switch pieces in the folder switch beside it, and the .nsp is written
 *    beside it. Files dropped on the tool are taken by what they are: a .keys (or .txt) the keys, a .jpg the
 *    icon, anything else the ROM set (sonic.zip, or a folder of its files); started so, its console waits for
 *    Enter at the end. Started with nothing (a double click), it opens its window (window.c, when it is
 *    built with SDL2 and the computer can show one), which shows the same inputs, takes files dropped on it
 *    and makes the same .nsp; else it makes the .nsp at once with the defaults, as with files dropped on it.
 *    With options it is a console program and nothing else.
 */
#include "make_nsp.h"

#ifdef SEGASONIC_NSP_WINDOW
#include "window.h"
#endif

#include <stdio.h>
#include <string.h>

static int waits;    /* started without options (a double click, files dropped on it): it waits for Enter */

static int finish(int code)
{
	if (waits)
	{
		printf("\nPress Enter to close.\n");
		(void)getchar();
	}
	return code;
}

int main(int argc, char **argv)
{
	char here[1024];
	nsp_inputs inputs;
	nsp_outcome outcome;

	memset(&inputs, 0, sizeof(inputs));
	waits = 1;
	nsp_folder_of(argv[0], here, sizeof(here));
#ifdef SEGASONIC_NSP_WINDOW
	if (argc < 2)
	{
		const int code = nsp_window(here);

		if (code >= 0)
			return code;
	}
#endif
	for (int at = 1; at < argc; at++)
	{
		char *target = NULL;

		if (strncmp(argv[at], "--", 2) == 0)
			waits = 0;    /* (a command line: the window is someone's console) */
		else
		{
			nsp_take_dropped(&inputs, argv[at]);    /* (files dropped on the tool: each by what it is) */
			continue;
		}
		if (strcmp(argv[at], "--keys") == 0)
			target = inputs.keys;
		else if (strcmp(argv[at], "--rom") == 0)
			target = inputs.rom;
		else if (strcmp(argv[at], "--switch") == 0)
			target = inputs.pieces;
		else if (strcmp(argv[at], "--out") == 0)
			target = inputs.out;
		else if (strcmp(argv[at], "--icon") == 0)
			target = inputs.icon;
		if (!target || at + 1 >= argc)
		{
			fprintf(stderr, "Usage: SegaSonic NSP [--keys FILE] [--rom FILE] [--switch FOLDER] [--out FILE] [--icon FILE]\n"
				"  --keys    the console's keys (prod.keys; by default beside it, or in .switch in the user's folder)\n"
				"  --rom     the arcade ROM set (sonic.zip, MAME's, Japan, rev. C; by default beside it)\n"
				"  --switch  the game's Switch build: main, main.npdm, control.nacp, icon.jpg (by default switch beside it)\n"
				"  --out     the .nsp to write (by default SegaSonic.nsp beside it)\n"
				"  --icon    the HOME menu's icon, a 256 x 256 JPEG of yours (by default the build's icon.jpg)\n");
			return 2;
		}
		snprintf(target, NSP_PATH_SIZE, "%s", argv[++at]);
	}
	printf("SegaSonic NSP: the Switch game, with the assets of your ROM set, ciphered with your console's keys\n\n");
	nsp_defaults(&inputs, here);
	if (!make_nsp(&inputs, stdout, NULL, NULL, &outcome))
	{
		fprintf(stderr, "%s%s%s\n", outcome.what, outcome.detail[0] ? ": " : "", outcome.detail);
		return finish(1);
	}
	return finish(0);
}
