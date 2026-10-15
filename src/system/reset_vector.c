/**
 *    Name:        system/reset_vector.c
 *    Description: the reset vector at the end of the chip (FFFFFFF0): jmp 0
 *    Version:     0.1.0
 *    Created:     12-10-2026
 *    Modified:    15-10-2026
 *    Author:      Sebastian Toyos
 *
 *    SegaSonic The Hedgehog (arcade, Sega System 32): the game's V60 program decompiled into C by
 *    tools/decompile.py, with the names of symbols.txt.
 */
#include "game.h"

void reset(void)
{
	boot();
}
