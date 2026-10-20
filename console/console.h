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

#include <stdbool.h>

/* True once the menu's EXIT was picked: the port closes the game */
bool console_quit_requested(void);

#endif
