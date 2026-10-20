/**
 *    Name:        console_actions.h
 *    Description: what the player asks of the console (move, accept, back, start), whatever key or button
 *                 asks it: each port maps its keys and buttons to these
 *    Version:     0.1.0
 *    Created:     19-10-2026
 *    Modified:    19-10-2026
 *    Author:      Sebastian Toyos
 */
#ifndef SEGASONIC_ACTIONS_H
#define SEGASONIC_ACTIONS_H

typedef enum
{
	ACTION_UP,
	ACTION_DOWN,
	ACTION_LEFT,
	ACTION_RIGHT,
	ACTION_ACCEPT,     /* picks in a menu */
	ACTION_BACK,       /* leaves a menu; pauses a stage */
	ACTION_START,      /* player 1's start; pauses a stage */
	ACTION_JUMP,       /* player 1's button: the jump, the spin */
	ACTIONS
} console_action;

#define ACTION_BIT(action) (1u << (action))

#endif
