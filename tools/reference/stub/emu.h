/**
 *    Name:        emu.h
 *    Description: minimal stand-in for MAME's emu.h so v60d.cpp builds on its own
 *    Version:     0.1.0
 *    Created:     11-10-2026
 *    Modified:    11-10-2026
 *    Author:      Sebastian Toyos
 */
#pragma once

#include <cstdint>
#include <cstdio>
#include <ostream>

using u8 = uint8_t;
using u16 = uint16_t;
using u32 = uint32_t;
using u64 = uint64_t;
using s8 = int8_t;
using s16 = int16_t;
using s32 = int32_t;
using offs_t = uint32_t;

namespace util {

// printf-style formatting into a stream, like MAME's util::stream_format
template <typename... Args>
void stream_format(std::ostream &stream, const char *format, Args... args)
{
	char buffer[256];
	std::snprintf(buffer, sizeof(buffer), format, args...);
	stream << buffer;
}

class disasm_interface
{
public:
	enum : u32
	{
		LENGTHMASK = 0x0000ffff,
		STEP_COND = 0x10000000,
		STEP_OVER = 0x20000000,
		STEP_OUT = 0x40000000,
		SUPPORTED = 0x80000000
	};

	// Little-endian reads from a flat image; addresses wrap with a mask, like a mirrored bus
	class data_buffer
	{
	public:
		data_buffer(const u8 *data, u32 mask) : m_data(data), m_mask(mask) {}
		u8 r8(offs_t address) const { return m_data[address & m_mask]; }
		u16 r16(offs_t address) const { return u16(r8(address) | (r8(address + 1) << 8)); }
		u32 r32(offs_t address) const { return u32(r16(address)) | (u32(r16(address + 2)) << 16); }

	private:
		const u8 *m_data;
		u32 m_mask;
	};

	virtual ~disasm_interface() = default;
	virtual u32 opcode_alignment() const = 0;
	virtual offs_t disassemble(std::ostream &stream, offs_t pc, const data_buffer &opcodes, const data_buffer &params) = 0;
};

} // namespace util
