/**
 *    Name:        actions.h
 *    Description: the keys and game controller buttons of each of the console's actions (console/console_actions.h),
 *                 and which are held and pressed this frame
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

/* Once a frame (and while paused): the actions held now, and the ones newly held */
void actions_read(SDL_GameController *const *controllers, int count);
uint32_t actions_held(void);
uint32_t actions_pressed(void);

#endif
