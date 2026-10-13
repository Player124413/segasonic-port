/**
 *    Name:        script.c
 *    Description: scripted controls for test runs (format in s32/script.h)
 *    Version:     0.1.0
 *    Created:     11-10-2026
 *    Modified:    11-10-2026
 *    Author:      Sebastian Toyos
 *
 *    license:BSD-3-Clause
 */
#include "s32/script.h"
#include "s32/machine.h"

#include <errno.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define DEFAULT_PRESS_FRAMES 4
#define PLAYERS 3
#define DIP_SWITCHES 4

enum
{
	ACTION_PRESS,
	ACTION_HOLD,
	ACTION_RELEASE,
	ACTION_ROLL,
	ACTION_DIP,
	ACTION_POKE,
	ACTION_END
};

/* Controls on the 315-5296 input ports, active low */
typedef struct control
{
	const char *name;
	int port;              /* 315-5296 port: 0 = A, 1 = B, 4 = E */
	uint8_t bit;
} control;

static const control controls[] =
{
	{ "button1", 0, 0x01 },
	{ "button3", 0, 0x04 },
	{ "button2", 1, 0x01 },
	{ "service", 4, 0x01 },
	{ "test", 4, 0x02 },
	{ "coin1", 4, 0x04 },
	{ "coin2", 4, 0x08 },
	{ "start1", 4, 0x10 },
	{ "start2", 4, 0x20 },
	{ "coin3", 4, 0x40 },
	{ "start3", 4, 0x80 }
};

#define CONTROL_COUNT (sizeof(controls) / sizeof(controls[0]))

typedef struct script_event
{
	uint64_t frame;
	int line;              /* line of the script, also the order of events in the same frame */
	int action;
	int target;            /* control index, player (0-2) or DIP switch (0-3) */
	int32_t dx;            /* roll: counts per frame */
	int32_t dy;
	uint32_t frames;       /* press: frames held */
	uint32_t address;      /* poke */
	uint8_t value;         /* poke: byte; dip: 1 = on */
} script_event;

struct s32_script
{
	script_event *events;
	size_t count;
	size_t next;                          /* first event not applied yet */
	uint64_t frames;                      /* frames to run, from 'end'; 0 = not said */
	uint64_t release_at[CONTROL_COUNT];   /* frame a press ends; 0 = not pressed by 'press' */
	int32_t roll_x[PLAYERS];
	int32_t roll_y[PLAYERS];
};

static uint8_t *control_port(int port)
{
	switch (port)
	{
		case 0: return &s32_board.input.port_a;
		case 1: return &s32_board.input.port_b;
		default: return &s32_board.input.port_e;
	}
}

static void set_control(int index, bool pressed)
{
	uint8_t *port = control_port(controls[index].port);
	*port = pressed ? (uint8_t)(*port & ~controls[index].bit) : (uint8_t)(*port | controls[index].bit);
}

static int find_control(const char *name)
{
	for (size_t index = 0; index < CONTROL_COUNT; index++)
		if (strcmp(controls[index].name, name) == 0)
			return (int)index;
	return -1;
}

static bool parse_unsigned(const char *text, int base, uint64_t *value)
{
	char *end;

	if (!text[0] || text[0] == '-' || text[0] == '+')
		return false;
	errno = 0;
	*value = strtoull(text, &end, base);
	return errno == 0 && *end == '\0';
}

static bool parse_signed(const char *text, int32_t *value)
{
	char *end;
	long number;

	errno = 0;
	number = strtol(text, &end, 10);
	if (!text[0] || errno != 0 || *end != '\0' || number < -2048 || number > 2048)
		return false;
	*value = (int32_t)number;
	return true;
}

/* One line into an event; false with 'error' set when the line is wrong */
static bool parse_event(const char *line, uint64_t previous, script_event *event, char *error, size_t error_size)
{
	char words[6][32];
	uint64_t number;
	const int fields = sscanf(line, "%31s %31s %31s %31s %31s %31s",
		words[0], words[1], words[2], words[3], words[4], words[5]);

	if (fields < 2 || fields > 5)
	{
		snprintf(error, error_size, "expected 'FRAME ACTION [ARGUMENTS]'");
		return false;
	}
	if (words[0][0] == '+')
	{
		if (!parse_unsigned(words[0] + 1, 10, &number))
		{
			snprintf(error, error_size, "bad relative frame '%s'", words[0]);
			return false;
		}
		event->frame = previous + number;
	}
	else if (!parse_unsigned(words[0], 10, &event->frame))
	{
		snprintf(error, error_size, "bad frame '%s'", words[0]);
		return false;
	}

	if (strcmp(words[1], "press") == 0 || strcmp(words[1], "hold") == 0 || strcmp(words[1], "release") == 0)
	{
		event->action = words[1][0] == 'p' ? ACTION_PRESS : words[1][0] == 'h' ? ACTION_HOLD : ACTION_RELEASE;
		event->frames = DEFAULT_PRESS_FRAMES;
		if (fields < 3 || (event->action != ACTION_PRESS && fields > 3) || fields > 4)
		{
			snprintf(error, error_size, "expected '%s INPUT%s'", words[1], event->action == ACTION_PRESS ? " [FRAMES]" : "");
			return false;
		}
		event->target = find_control(words[2]);
		if (event->target < 0)
		{
			snprintf(error, error_size, "unknown input '%s'", words[2]);
			return false;
		}
		if (fields == 4)
		{
			if (!parse_unsigned(words[3], 10, &number) || number == 0 || number > 100000)
			{
				snprintf(error, error_size, "bad frame count '%s'", words[3]);
				return false;
			}
			event->frames = (uint32_t)number;
		}
		return true;
	}
	if (strcmp(words[1], "roll") == 0)
	{
		event->action = ACTION_ROLL;
		if (fields != 5 || !parse_unsigned(words[2], 10, &number) || number < 1 || number > PLAYERS
			|| !parse_signed(words[3], &event->dx) || !parse_signed(words[4], &event->dy))
		{
			snprintf(error, error_size, "expected 'roll PLAYER DX DY' with PLAYER 1-3 and DX, DY in -2048..2048");
			return false;
		}
		event->target = (int)number - 1;
		return true;
	}
	if (strcmp(words[1], "dip") == 0)
	{
		event->action = ACTION_DIP;
		if (fields != 4 || !parse_unsigned(words[2], 10, &number) || number < 1 || number > DIP_SWITCHES
			|| (strcmp(words[3], "on") != 0 && strcmp(words[3], "off") != 0))
		{
			snprintf(error, error_size, "expected 'dip SWITCH on|off' with SWITCH 1-4");
			return false;
		}
		event->target = (int)number - 1;
		event->value = strcmp(words[3], "on") == 0;
		return true;
	}
	if (strcmp(words[1], "poke") == 0)
	{
		uint64_t value;

		event->action = ACTION_POKE;
		if (fields != 4 || !parse_unsigned(words[2], 16, &number) || number > 0xffffff
			|| !parse_unsigned(words[3], 16, &value) || value > 0xff)
		{
			snprintf(error, error_size, "expected 'poke ADDRESS VALUE' in hexadecimal, a 24-bit address and a byte");
			return false;
		}
		event->address = (uint32_t)number;
		event->value = (uint8_t)value;
		return true;
	}
	if (strcmp(words[1], "end") == 0)
	{
		event->action = ACTION_END;
		if (fields != 2)
		{
			snprintf(error, error_size, "expected 'end' with no arguments");
			return false;
		}
		return true;
	}
	snprintf(error, error_size, "unknown action '%s'", words[1]);
	return false;
}

static int compare_events(const void *left, const void *right)
{
	const script_event *a = (const script_event *)left;
	const script_event *b = (const script_event *)right;

	if (a->frame != b->frame)
		return a->frame < b->frame ? -1 : 1;
	return a->line - b->line;
}

s32_script *s32_script_load(const char *path, char *error, size_t error_size)
{
	FILE *input = fopen(path, "r");
	s32_script *script;
	char line[256];
	char problem[160];
	size_t capacity = 0;
	uint64_t previous = 0;
	int number = 0;

	if (!input)
	{
		snprintf(error, error_size, "%s: cannot be read", path);
		return NULL;
	}
	script = (s32_script *)calloc(1, sizeof(*script));
	if (!script)
	{
		fclose(input);
		snprintf(error, error_size, "%s: out of memory", path);
		return NULL;
	}
	while (fgets(line, sizeof(line), input))
	{
		char *comment = strchr(line, '#');
		script_event event;

		number++;
		if (comment)
			*comment = '\0';
		if (strspn(line, " \t\r\n") == strlen(line))
			continue;
		memset(&event, 0, sizeof(event));
		event.line = number;
		if (!parse_event(line, previous, &event, problem, sizeof(problem)))
		{
			snprintf(error, error_size, "%s:%d: %s", path, number, problem);
			fclose(input);
			s32_script_free(script);
			return NULL;
		}
		previous = event.frame;
		if (event.action == ACTION_END)
		{
			if (!script->frames || event.frame + 1 < script->frames)
				script->frames = event.frame + 1;
			continue;
		}
		if (script->count == capacity)
		{
			const size_t grown = capacity ? capacity * 2 : 64;
			script_event *events = (script_event *)realloc(script->events, grown * sizeof(*events));
			if (!events)
			{
				snprintf(error, error_size, "%s: out of memory", path);
				fclose(input);
				s32_script_free(script);
				return NULL;
			}
			script->events = events;
			capacity = grown;
		}
		script->events[script->count++] = event;
	}
	fclose(input);
	if (script->count)
		qsort(script->events, script->count, sizeof(*script->events), compare_events);
	return script;
}

uint64_t s32_script_frames(const s32_script *script)
{
	return script ? script->frames : 0;
}

void s32_script_free(s32_script *script)
{
	if (!script)
		return;
	free(script->events);
	free(script);
}

static void run_event(s32_script *script, const script_event *event, uint64_t frame)
{
	switch (event->action)
	{
		case ACTION_PRESS:
			set_control(event->target, true);
			script->release_at[event->target] = frame + event->frames;
			break;
		case ACTION_HOLD:
			set_control(event->target, true);
			script->release_at[event->target] = 0;
			break;
		case ACTION_RELEASE:
			set_control(event->target, false);
			script->release_at[event->target] = 0;
			break;
		case ACTION_ROLL:
			script->roll_x[event->target] = event->dx;
			script->roll_y[event->target] = event->dy;
			break;
		case ACTION_DIP:
			/* a switch that is on pulls its line low */
			if (event->value)
				s32_board.input.port_f &= (uint8_t)~(1u << event->target);
			else
				s32_board.input.port_f |= (uint8_t)(1u << event->target);
			break;
		case ACTION_POKE:
			s32_write8(event->address, event->value);
			break;
	}
}

void s32_script_apply(s32_script *script, uint64_t frame)
{
	if (!script)
		return;
	for (size_t index = 0; index < CONTROL_COUNT; index++)
		if (script->release_at[index] && script->release_at[index] <= frame)
		{
			set_control((int)index, false);
			script->release_at[index] = 0;
		}
	while (script->next < script->count && script->events[script->next].frame <= frame)
		run_event(script, &script->events[script->next++], frame);
	for (int player = 0; player < PLAYERS; player++)
	{
		s32_board.input.trackball_x[player] = (uint16_t)(s32_board.input.trackball_x[player] + script->roll_x[player]);
		s32_board.input.trackball_y[player] = (uint16_t)(s32_board.input.trackball_y[player] + script->roll_y[player]);
	}
}
