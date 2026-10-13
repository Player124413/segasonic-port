/**
 *    Name:        internal.h
 *    Description: functions shared between the files of the System 32 machine model
 *    Version:     0.1.0
 *    Created:     11-10-2026
 *    Modified:    11-10-2026
 *    Author:      Sebastian Toyos
 *
 *    license:BSD-3-Clause
 */
#ifndef S32_INTERNAL_H
#define S32_INTERNAL_H

#include "s32/machine.h"

/* I/O area at 0xC00000 (mirrored every 0x80 bytes): 315-5296 at 0x00-0x1F, trackballs at 0x40-0x57.
   'offset' is the byte address inside the 0x80-byte window; devices sit on the low byte lane. */
uint8_t s32_io_read(uint32_t offset);
void s32_io_write(uint32_t offset, uint8_t data);
void s32_io_reset(void);

void s32_log(const char *format, ...);

#endif
