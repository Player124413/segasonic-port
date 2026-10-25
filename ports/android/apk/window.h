/**
 *    Name:        window.h
 *    Description: SegaSonic APK's window, for a player without a command line (window.c)
 *    Version:     0.1.0
 *    Created:     25-10-2026
 *    Modified:    25-10-2026
 *    Author:      Sebastian Toyos
 *
 *    license:BSD-3-Clause
 */
#ifndef SEGASONIC_APK_WINDOW_H
#define SEGASONIC_APK_WINDOW_H

/* The window, until it is closed: its exit code then, or -1 when the computer has no window to show (no
   display, no font.bmp) and the caller goes on in its console. 'here' is the tool's folder as its command
   line gives it (with its separator), for when SDL cannot tell where the tool is */
int apk_window(const char *here);

#endif
