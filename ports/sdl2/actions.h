/**
 *    Name:        actions.h
 *    Description: the keys and game controller buttons of each of the console's actions (console/console_actions.h),
 *                 which are held and pressed this frame, and controls.ini
 *    Version:     0.1.0
 *    Created:     19-10-2026
 *    Modified:    19-10-2026
 *    Author:      Sebastian Toyos
 */
#ifndef SEGASONIC_PORT_ACTIONS_H
#define SEGASONIC_PORT_ACTIONS_H

#include "console_actions.h"

#include <SDL.h>
#include <stdbool.h>
#include <stdint.h>

#define ACTION_KEYS     4
#define ACTION_BUTTONS  2

typedef struct
{
	const char *name;                                  /* as the settings file says it */
	SDL_Scancode keys[ACTION_KEYS];                    /* SDL_SCANCODE_UNKNOWN: none */
	SDL_GameControllerButton buttons[ACTION_BUTTONS];  /* SDL_CONTROLLER_BUTTON_INVALID: none */
} action_binding;

extern action_binding action_bindings[ACTIONS];

/* controls.ini at 'path': read (the defaults where it says nothing), or written when there is none */
void actions_open(const char *path);
void actions_save(void);
void actions_defaults(void);    /* (and saved) */

/* The key or the game controller button the player pressed for 'action' on the CONTROLS screen: its first
   one now (and saved) */
void actions_bind_key(int action, SDL_Scancode key);
void actions_bind_button(int action, SDL_GameControllerButton button);

/* Once a frame (and while paused): the actions held now, and the ones newly held */
void actions_read(SDL_GameController *const *controllers, int count);
uint32_t actions_held(void);

/* The action's keys alone (one held), or one game controller's buttons alone: for the game's own
   controls, which take the sticks apart */
bool actions_keys_hold(int action);
bool actions_pad_holds(SDL_GameController *pad, int action);

/* What was pressed this frame counts no more (a key the CONTROLS screen just took for an action) */
void actions_forget_pressed(void);
uint32_t actions_pressed(void);

#endif
