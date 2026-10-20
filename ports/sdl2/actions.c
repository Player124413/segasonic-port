/**
 *    Name:        actions.c
 *    Description: the keys and game controller buttons of each of the console's actions, and which are held
 *                 and pressed this frame
 *    Version:     0.1.0
 *    Created:     19-10-2026
 *    Modified:    19-10-2026
 *    Author:      Sebastian Toyos
 *
 *    The defaults below; the settings file (and the console's CONTROLS screen) can change them. A game
 *    controller's left stick counts as its directional pad.
 */
#include "actions.h"

#define NO_KEY     SDL_SCANCODE_UNKNOWN
#define NO_BUTTON  SDL_CONTROLLER_BUTTON_INVALID
#define STICK_DEAD 16000    /* how far the left stick goes before it counts as a direction */

action_binding action_bindings[ACTIONS] = {
	[ACTION_UP] = { "up", { SDL_SCANCODE_UP, SDL_SCANCODE_W, NO_KEY, NO_KEY },
		{ SDL_CONTROLLER_BUTTON_DPAD_UP, NO_BUTTON } },
	[ACTION_DOWN] = { "down", { SDL_SCANCODE_DOWN, SDL_SCANCODE_S, NO_KEY, NO_KEY },
		{ SDL_CONTROLLER_BUTTON_DPAD_DOWN, NO_BUTTON } },
	[ACTION_LEFT] = { "left", { SDL_SCANCODE_LEFT, SDL_SCANCODE_A, NO_KEY, NO_KEY },
		{ SDL_CONTROLLER_BUTTON_DPAD_LEFT, NO_BUTTON } },
	[ACTION_RIGHT] = { "right", { SDL_SCANCODE_RIGHT, SDL_SCANCODE_D, NO_KEY, NO_KEY },
		{ SDL_CONTROLLER_BUTTON_DPAD_RIGHT, NO_BUTTON } },
	[ACTION_ACCEPT] = { "accept", { SDL_SCANCODE_RETURN, SDL_SCANCODE_KP_ENTER, SDL_SCANCODE_Z, SDL_SCANCODE_SPACE },
		{ SDL_CONTROLLER_BUTTON_A, NO_BUTTON } },
	[ACTION_BACK] = { "back", { SDL_SCANCODE_ESCAPE, SDL_SCANCODE_BACKSPACE, NO_KEY, NO_KEY },
		{ SDL_CONTROLLER_BUTTON_B, NO_BUTTON } },
	[ACTION_START] = { "start", { SDL_SCANCODE_RETURN, SDL_SCANCODE_KP_ENTER, SDL_SCANCODE_P, NO_KEY },
		{ SDL_CONTROLLER_BUTTON_START, NO_BUTTON } },
};

static uint32_t held, pressed;

static bool stick_says(SDL_GameController *pad, int action)
{
	const int x = SDL_GameControllerGetAxis(pad, SDL_CONTROLLER_AXIS_LEFTX);
	const int y = SDL_GameControllerGetAxis(pad, SDL_CONTROLLER_AXIS_LEFTY);

	switch (action)
	{
		case ACTION_UP: return y < -STICK_DEAD;
		case ACTION_DOWN: return y > STICK_DEAD;
		case ACTION_LEFT: return x < -STICK_DEAD;
		case ACTION_RIGHT: return x > STICK_DEAD;
		default: return false;
	}
}

void actions_read(SDL_GameController *const *controllers, int count)
{
	const Uint8 *keys = SDL_GetKeyboardState(NULL);
	const bool alt = (SDL_GetModState() & KMOD_ALT) != 0;    /* (Alt+Enter is full screen, not an action) */
	uint32_t now = 0;

	for (int action = 0; action < ACTIONS; action++)
	{
		const action_binding *binding = &action_bindings[action];
		bool on = false;

		for (int key = 0; key < ACTION_KEYS && !on; key++)
			on = binding->keys[key] != NO_KEY && keys[binding->keys[key]] && !alt;
		for (int pad = 0; pad < count && !on; pad++)
		{
			if (!controllers[pad])
				continue;
			for (int button = 0; button < ACTION_BUTTONS && !on; button++)
				on = binding->buttons[button] != NO_BUTTON
					&& SDL_GameControllerGetButton(controllers[pad], binding->buttons[button]);
			on = on || stick_says(controllers[pad], action);
		}
		if (on)
			now |= ACTION_BIT(action);
	}
	pressed = now & ~held;
	held = now;
}

uint32_t actions_held(void)
{
	return held;
}

uint32_t actions_pressed(void)
{
	return pressed;
}
