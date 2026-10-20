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
#include <stdint.h>

/* The actions newly pressed this frame (ACTION_BIT), before the game's frame: the menus read them */
void console_actions(uint32_t pressed);

/* True once the menu's EXIT was picked: the port closes the game */
bool console_quit_requested(void);

/* Escape outside a game: the title's menu closes (back to PRESS START) */
void console_back(void);

/* True while a stage is played in a game (not the intro, player select, the map, the scenes, nor the
   attract mode's demos): start pauses it */
bool console_in_game(void);

/* "PAUSE" on the text layer's top (shown), or the cells it covered back as they were */
void console_pause_text(bool shown);

#endif
