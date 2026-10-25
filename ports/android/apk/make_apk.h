/**
 *    Name:        make_apk.h
 *    Description: SegaSonic APK's work, for its command line and its window alike: the APK from its inputs,
 *                 their defaults, and what the window shows of each
 *    Version:     0.1.0
 *    Created:     25-10-2026
 *    Modified:    25-10-2026
 *    Author:      Sebastian Toyos
 *
 *    license:BSD-3-Clause
 *    Paths are as the C library's fopen takes them (on Windows, in the system's code page).
 */
#ifndef SEGASONIC_APK_MAKE_APK_H
#define SEGASONIC_APK_MAKE_APK_H

#include <stddef.h>
#include <stdio.h>

#define APK_PATH_SIZE    1100
#define APK_FILE         "SegaSonic.apk"
#define APK_BASE_FOLDER  "android"
#define APK_BASE_FILE    "segasonic-base.apk"
#define APK_ICON_MOST    2048    /* the icon's most pixels a side: the launcher draws it from memory */

/* What the APK is made of, and where it goes */
typedef struct
{
	char rom[APK_PATH_SIZE];     /* the arcade ROM set: sonic.zip, or a folder of its files */
	char icon[APK_PATH_SIZE];    /* the launcher's icon, a square PNG or JPEG of the player's; "" the game's own */
	char base[APK_PATH_SIZE];    /* the game's Android build: segasonic-base.apk, without the assets or a signature */
	char out[APK_PATH_SIZE];     /* the APK to write */
} apk_inputs;

/* How making it went */
typedef struct
{
	char what[80];                       /* what failed */
	char detail[APK_PATH_SIZE + 128];    /* what it failed on (a file, or what the part that failed tells), or "" */
	size_t size;                         /* the APK's bytes, when it is written */
	int key_made;                        /* the signing key was made for it (else it was kept from before) */
} apk_outcome;

/* Told the step making the APK is at, a name of its own ("taking the assets out of the ROM set") */
typedef void (*apk_step)(void *context, const char *step);

/* A file dropped on the tool, taken by what it is: an .apk the game's Android build, a .png or .jpg the
   icon, anything else the ROM set (sonic.zip, or a folder of its files) */
void apk_take_dropped(apk_inputs *inputs, const char *path);

/* The inputs not given, beside the tool (in the folder 'here', with its separator): sonic.zip,
   android/segasonic-base.apk, SegaSonic.apk */
void apk_defaults(apk_inputs *inputs, const char *here);

/* The APK: true when it is written. Each step's lines go to 'log' (the command line's) unless it is NULL;
   'step' (or NULL) is told each step as it starts. What failed is in 'outcome' */
int make_apk(const apk_inputs *inputs, FILE *log, apk_step step, void *context, apk_outcome *outcome);

/* The folder of a path, with its separator: "" for a bare name */
void apk_folder_of(const char *path, char *folder, size_t size);

/* What the window shows of each input: NULL when it is good, else what is wrong with it ("not found") */
const char *apk_icon_problem(const char *path);
const char *apk_base_problem(const char *path);
int apk_base_takes_icon(const char *path);    /* the build has the entries an icon of the player's goes in */
int apk_file_exists(const char *path);
int apk_folder_exists(const char *path);

#endif
