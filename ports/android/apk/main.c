/**
 *    Name:        main.c
 *    Description: SegaSonic APK: the Android game as an APK to install, made on the player's computer from
 *                 the game's Android build and the player's ROM set, signed with the player's own key
 *    Version:     0.1.0
 *    Created:     25-10-2026
 *    Modified:    25-10-2026
 *    Author:      Sebastian Toyos
 *
 *    license:BSD-3-Clause
 *    Usage: SegaSonic APK [--rom FILE] [--icon FILE] [--base FILE] [--out FILE]
 *
 *    The APK is made in make_apk.c. By default every file is beside the tool, the game's Android build in
 *    the folder android beside it, and the APK is written beside it; the signing key is the player's own
 *    (key.c), made the first time. Files dropped on the tool are taken by what they are: an .apk the
 *    game's Android build, a .png or .jpg the icon, anything else the ROM set (sonic.zip, or a folder of
 *    its files); started so, its console waits for Enter at the end. Started with nothing (a double
 *    click), it opens its window (window.c, when it is built with SDL2 and the computer can show one),
 *    which shows the same inputs, takes files dropped on it and makes the same APK; else it makes the APK
 *    at once with the defaults, as with files dropped on it. With options it is a console program and
 *    nothing else.
 */
#include "make_apk.h"

#ifdef SEGASONIC_APK_WINDOW
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
	apk_inputs inputs;
	apk_outcome outcome;

	memset(&inputs, 0, sizeof(inputs));
	waits = 1;
	apk_folder_of(argv[0], here, sizeof(here));
#ifdef SEGASONIC_APK_WINDOW
	if (argc < 2)
	{
		const int code = apk_window(here);

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
			apk_take_dropped(&inputs, argv[at]);    /* (files dropped on the tool: each by what it is) */
			continue;
		}
		if (strcmp(argv[at], "--rom") == 0)
			target = inputs.rom;
		else if (strcmp(argv[at], "--icon") == 0)
			target = inputs.icon;
		else if (strcmp(argv[at], "--base") == 0)
			target = inputs.base;
		else if (strcmp(argv[at], "--out") == 0)
			target = inputs.out;
		if (!target || at + 1 >= argc)
		{
			fprintf(stderr, "Usage: SegaSonic APK [--rom FILE] [--icon FILE] [--base FILE] [--out FILE]\n"
				"  --rom   the arcade ROM set (sonic.zip, MAME's, Japan, rev. C; by default beside it)\n"
				"  --icon  the launcher's icon, a square PNG or JPEG of yours (by default the game's own)\n"
				"  --base  the game's Android build (by default android/segasonic-base.apk beside it)\n"
				"  --out   the APK to write (by default SegaSonic.apk beside it)\n");
			return 2;
		}
		snprintf(target, APK_PATH_SIZE, "%s", argv[++at]);
	}
	printf("SegaSonic APK: the Android game, with the assets of your ROM set, signed with your own key\n\n");
	apk_defaults(&inputs, here);
	if (!make_apk(&inputs, stdout, NULL, NULL, &outcome))
	{
		fprintf(stderr, "%s%s%s\n", outcome.what, outcome.detail[0] ? ": " : "", outcome.detail);
		return finish(1);
	}
	return finish(0);
}
