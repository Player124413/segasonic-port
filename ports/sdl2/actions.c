/**
 *    Name:        actions.c
 *    Description: the keys and game controller buttons of each of the console's actions, which are held and
 *                 pressed this frame, and the file that keeps them (controls.ini)
 *    Version:     0.1.0
 *    Created:     19-10-2026
 *    Modified:    20-10-2026
 *    Author:      Sebastian Toyos
 *
 *    The defaults below; controls.ini, beside the executable, keeps what the console's CONTROLS screen
 *    changes, a line an action: "jump = Z, Space | a" (SDL's names of the keys, then of the buttons). A game
 *    controller's left stick counts as its directional pad.
 */
#include "actions.h"

#include <stdio.h>
#include <string.h>

#define NO_KEY     SDL_SCANCODE_UNKNOWN
#define NO_BUTTON  SDL_CONTROLLER_BUTTON_INVALID
#define STICK_DEAD 16000    /* how far the left stick goes before it counts as a direction */
#define LINE_SIZE  256

static const action_binding defaults[ACTIONS] = {
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
	[ACTION_JUMP] = { "jump", { SDL_SCANCODE_Z, SDL_SCANCODE_SPACE, NO_KEY, NO_KEY },
		{ SDL_CONTROLLER_BUTTON_A, NO_BUTTON } },
};

action_binding action_bindings[ACTIONS];

static uint32_t held, pressed;
static char file_path[1024];

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

bool actions_keys_hold(int action)
{
	const Uint8 *keys = SDL_GetKeyboardState(NULL);

	for (int key = 0; key < ACTION_KEYS; key++)
		if (action_bindings[action].keys[key] != NO_KEY && keys[action_bindings[action].keys[key]])
			return true;
	return false;
}

bool actions_pad_holds(SDL_GameController *pad, int action)
{
	for (int button = 0; button < ACTION_BUTTONS; button++)
		if (action_bindings[action].buttons[button] != NO_BUTTON
			&& SDL_GameControllerGetButton(pad, action_bindings[action].buttons[button]))
			return true;
	return false;
}

void actions_forget_pressed(void)
{
	pressed = 0;
}

uint32_t actions_held(void)
{
	return held;
}

uint32_t actions_pressed(void)
{
	return pressed;
}

/* ---- controls.ini ---- */

static char *trimmed(char *text)
{
	char *end;

	while (*text == ' ' || *text == '\t')
		text++;
	end = text + strlen(text);
	while (end > text && (end[-1] == ' ' || end[-1] == '\t' || end[-1] == '\r' || end[-1] == '\n'))
		*--end = 0;
	return text;
}

/* One line "name = Key, Key | button, button": the action it names takes those, in place of its own */
static void read_line(char *line)
{
	char *equals = strchr(line, '='), *bar, *part, *next;
	const char *name;
	int action, slot;

	if (line[0] == ';' || line[0] == '#' || !equals)
		return;
	*equals = 0;
	name = trimmed(line);
	for (action = 0; action < ACTIONS && strcmp(action_bindings[action].name, name) != 0; action++)
		;
	if (action == ACTIONS)
		return;
	bar = strchr(equals + 1, '|');
	if (bar)
		*bar = 0;
	for (slot = 0; slot < ACTION_KEYS; slot++)
		action_bindings[action].keys[slot] = NO_KEY;
	for (slot = 0; slot < ACTION_BUTTONS; slot++)
		action_bindings[action].buttons[slot] = NO_BUTTON;
	for (part = equals + 1, slot = 0; part && slot < ACTION_KEYS; part = next)
	{
		next = strchr(part, ',');
		if (next)
			*next++ = 0;
		if (*trimmed(part))
			action_bindings[action].keys[slot++] = SDL_GetScancodeFromName(trimmed(part));
	}
	for (part = bar ? bar + 1 : NULL, slot = 0; part && slot < ACTION_BUTTONS; part = next)
	{
		next = strchr(part, ',');
		if (next)
			*next++ = 0;
		if (*trimmed(part))
			action_bindings[action].buttons[slot++] = SDL_GameControllerGetButtonFromString(trimmed(part));
	}
}

void actions_open(const char *path)
{
	FILE *file;
	char line[LINE_SIZE];

	memcpy(action_bindings, defaults, sizeof(action_bindings));
	SDL_strlcpy(file_path, path, sizeof(file_path));
	file = fopen(file_path, "r");
	if (!file)
	{
		actions_save();
		return;
	}
	while (fgets(line, sizeof(line), file))
		read_line(line);
	fclose(file);
}

void actions_save(void)
{
	FILE *file = fopen(file_path, "w");

	if (!file)
		return;
	fputs("; The console's controls: an action a line, its keys (SDL's names), then after | its game\n"
		"; controller buttons. OPTIONS > CONTROLS writes this file; it can be changed by hand too.\n", file);
	for (int action = 0; action < ACTIONS; action++)
	{
		const action_binding *binding = &action_bindings[action];
		bool first = true;

		fprintf(file, "%s =", binding->name);
		for (int key = 0; key < ACTION_KEYS; key++)
			if (binding->keys[key] != NO_KEY)
			{
				fprintf(file, "%s %s", first ? "" : ",", SDL_GetScancodeName(binding->keys[key]));
				first = false;
			}
		fputs(" |", file);
		first = true;
		for (int button = 0; button < ACTION_BUTTONS; button++)
			if (binding->buttons[button] != NO_BUTTON)
			{
				fprintf(file, "%s %s", first ? "" : ",",
					SDL_GameControllerGetStringForButton(binding->buttons[button]));
				first = false;
			}
		fputs("\n", file);
	}
	fclose(file);
}

void actions_defaults(void)
{
	memcpy(action_bindings, defaults, sizeof(action_bindings));
	actions_save();
}

/* The key or button the player pressed for 'action': its first one, the one it had before moving on to the
   second (a key that was already the action's moves to the front) */
void actions_bind_key(int action, SDL_Scancode key)
{
	action_binding *binding = &action_bindings[action];
	int slot;

	for (slot = 0; slot < ACTION_KEYS - 1 && binding->keys[slot] != key; slot++)
		;
	memmove(&binding->keys[1], &binding->keys[0], (size_t)slot * sizeof(binding->keys[0]));
	binding->keys[0] = key;
	actions_save();
}

void actions_bind_button(int action, SDL_GameControllerButton button)
{
	action_binding *binding = &action_bindings[action];

	if (binding->buttons[0] != button)
		binding->buttons[1] = binding->buttons[0];
	binding->buttons[0] = button;
	actions_save();
}
