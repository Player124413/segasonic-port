/**
 *    Name:        bus.h
 *    Description: the game program's view of memory: reads and writes through the System 32 bus
 *    Version:     0.1.0
 *    Created:     11-10-2026
 *    Modified:    11-10-2026
 *    Author:      Sebastian Toyos
 *
 *    license:BSD-3-Clause
 *    The V60 drives 24 address lines, so an address wraps at 16 MB. Values are little endian, as the V60
 *    keeps them.
 */
#ifndef S32_BUS_H
#define S32_BUS_H

#include <stdint.h>

#include "s32/machine.h"

#ifdef __cplusplus
extern "C" {
#endif

static inline uint8_t mem_read8(uint32_t address)
{
	return s32_read8(address & 0xffffff);
}

static inline uint16_t mem_read16(uint32_t address)
{
	return s32_read16(address & 0xffffff);
}

static inline uint32_t mem_read32(uint32_t address)
{
	return s32_read32(address & 0xffffff);
}

static inline void mem_write8(uint32_t address, uint8_t data)
{
	s32_write8(address & 0xffffff, data);
}

static inline void mem_write16(uint32_t address, uint16_t data)
{
	s32_write16(address & 0xffffff, data);
}

static inline void mem_write32(uint32_t address, uint32_t data)
{
	s32_write32(address & 0xffffff, data);
}

static inline uint64_t mem_read64(uint32_t address)
{
	const uint64_t low = mem_read32(address);
	return low | ((uint64_t)mem_read32(address + 4) << 32);
}

static inline void mem_write64(uint32_t address, uint64_t data)
{
	mem_write32(address, (uint32_t)data);
	mem_write32(address + 4, (uint32_t)(data >> 32));
}

#ifdef __cplusplus
}
#endif

#endif
