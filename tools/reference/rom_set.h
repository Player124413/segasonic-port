/**
 *    Name:        rom_set.h
 *    Description: fills the board's memory from the player's ROM set, for the reference (rom_set.c)
 *    Version:     0.1.0
 *    Created:     20-10-2026
 *    Modified:    20-10-2026
 *    Author:      Sebastian Toyos
 *
 *    license:BSD-3-Clause
 */
#ifndef REFERENCE_ROM_SET_H
#define REFERENCE_ROM_SET_H

#ifdef __cplusplus
extern "C" {
#endif

/* Fills game/data.h's regions from the set's files in 'folder': NULL, or the name of the first file the
   folder does not have (by either of its names, whole, with its CRC) */
const char *reference_load_rom_set(const char *folder);

#ifdef __cplusplus
}
#endif

#endif
