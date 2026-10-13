/**
 *    Name:        settings.h
 *    Description: the window port's settings, kept in sonic.ini beside the executable
 *    Version:     0.1.0
 *    Created:     13-10-2026
 *    Modified:    13-10-2026
 *    Author:      Sebastian Toyos
 *
 *    license:BSD-3-Clause
 */
#ifndef SONIC_SDL2_SETTINGS_H
#define SONIC_SDL2_SETTINGS_H

#include <stdbool.h>

typedef struct port_settings
{
	double mouse_speed;       /* trackball counts a pixel of mouse movement */
	bool ball_rolls_on;       /* the mouse turns a ball that keeps rolling when the mouse stops */
	double ball_drag;         /* a ball rolling on its own keeps this part of its speed a frame ... */
	double ball_friction;     /* ... and loses this many counts a frame more */
	double key_roll;          /* trackball counts a frame for an arrow key held */
	double stick_roll;        /* ... for a controller's stick pushed all the way */
	double window_scale;      /* the window, in times the game's picture */
	bool full_screen;
	bool smooth;              /* smooth pixels, or sharp ones */
	double volume;            /* of the game's sound */
} port_settings;

extern port_settings settings;

/* Reads the file at 'path', or writes it with the settings as they are when there is none */
void settings_open(const char *path);

/* Reads the file again when it was saved since it was last read or written; true when it was */
bool settings_reread(void);

/* Writes the settings into the file, keeping its comments and the order of its lines */
void settings_save(void);

#endif
