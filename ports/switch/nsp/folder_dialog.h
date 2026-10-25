/**
 *    Name:        folder_dialog.h
 *    Description: the system's dialog to choose a folder (SegaSonic NSP's output folder): Windows' own, on
 *                 macOS osascript's, on Linux zenity's or kdialog's (folder_dialog.c)
 *    Version:     0.1.0
 *    Created:     25-10-2026
 *    Modified:    25-10-2026
 *    Author:      Sebastian Toyos
 *
 *    license:BSD-3-Clause
 */
#ifndef SEGASONIC_NSP_FOLDER_DIALOG_H
#define SEGASONIC_NSP_FOLDER_DIALOG_H

#include <stddef.h>

enum
{
	FOLDER_CHOSEN = 1,
	FOLDER_CANCELLED = 0,
	FOLDER_NO_DIALOG = -1,    /* the system has none of the dialogs it knows */
	FOLDER_UNUSABLE = -2      /* a folder whose name the C library cannot open (on Windows, out of its code page) */
};

/* The dialog, over the window 'owner' (on Windows its HWND; NULL for none) and starting in 'start' (on
   Windows; NULL for the system's choice): the folder chosen, as fopen takes it, in 'folder'. It returns when
   the dialog is closed */
int folder_dialog(void *owner, const char *start, char *folder, size_t size);

#endif
