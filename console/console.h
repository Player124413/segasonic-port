/**
 *    Name:        console.h
 *    Description: what the console build tells the port
 *    Version:     0.1.0
 *    Created:     19-10-2026
 *    Modified:    19-10-2026
 *    Author:      Sebastian Toyos
 */
#ifndef SEGASONIC_CONSOLE_H
#define SEGASONIC_CONSOLE_H

#include "console_actions.h"

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

/* The window's settings the console's OPTIONS show and change: the port fills them in (and again when its
   settings change), and applies and saves them when OPTIONS sets 'changed' */
typedef struct
{
	int window_scale;     /* 1-6: the window, in times the game's picture */
	bool full_screen;
	bool smooth;          /* smooth pixels (sharp bilinear), or sharp ones */
	bool bilinear;        /* plain bilinear pixels, softer than smooth ones (over smooth) */
	int volume;           /* 0-10: 5 is the game's own */
	int title_music;      /* the sound id of the song the title plays (SOUND TEST picks it) */
	int continues;        /* a game's continues, 0-9, or -1: free play (CONTINUES) */
	bool changed;
} console_window;

extern console_window console_window_settings;

/* The actions newly pressed this frame and the ones held (ACTION_BIT), before the game's frame: the
   menus read them */
void console_actions(uint32_t pressed, uint32_t held);

/* The instruments (the sound driver's programs, 0-36) of the SEGA logo's jingle: its voice, and the layer an
   octave under it (-1: none) */
void console_jingle_program(int program, int layer);

/* True once the menu's EXIT was picked: the port closes the game */
bool console_quit_requested(void);

/* Escape outside a game: the title's menu closes (back to PRESS START) */
void console_back(void);

/* OPTIONS > CONTROLS on screen: the text row of 'action''s line (the port draws its keys and buttons
   there), or -1 */
int console_controls_row(int action);

/* The player whose controls CONTROLS shows (0-2): the port draws theirs, and binds and puts back theirs */
int console_controls_player(void);

/* The action CONTROLS waits for a new key or button for, or -1: the port takes the next one pressed, binds it
   (Escape: leaves it) and says so */
int console_controls_waiting(void);
void console_controls_bound(void);

/* DEFAULTS was picked on CONTROLS (true once): the port puts the shown player's keys and buttons back */
bool console_controls_defaults(void);

#define CONSOLE_CONTROLS_KEYS_X     120    /* where the port draws an action's keys, in pixels, */
#define CONSOLE_CONTROLS_SECOND_X   160    /* its second key, */
#define CONSOLE_CONTROLS_BUTTON_X   224    /* and its game controller button */

/* A player's start prompt is up (to join, or a continue's count): their start is the board's start then,
   even in a stage, where it is otherwise the pause */
bool console_start_prompted(int player);

/* The console's changes to the sprite graphics, on the port's copy before the board draws any: Eggman's
   English lines without their red box, white letters with a dark outline ('program': the program ROM, for
   the frames' records) */
void console_prepare_sprites(uint8_t *sprites, size_t size, const uint8_t *program);

/* True while a stage is played in a game (not the intro, player select, the map, the scenes, nor the
   attract mode's demos): start pauses it */
bool console_in_game(void);

/* "PAUSE" on the text layer's top (shown), or the cells it covered back as they were */
void console_pause_text(bool shown);

#endif
