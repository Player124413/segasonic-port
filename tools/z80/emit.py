# license:BSD-3-Clause
# copyright-holders:Sebastian Toyos
"""
Name:        emit.py
Description: writes the sound program's functions as C, through the decompiler's writer
Version:     0.1.0
Created:     13-10-2026
Modified:    13-10-2026
Author:      Sebastian Toyos

The decompiler's FunctionWriter does the work (decompiler/emit.py); this gives it the Z80's memory and
effects. Memory at a constant address prints as the place it is: SOUND_RAM8/16(address) for the RAM the
Z80 shares with the V60 (E000-FFFF), SOUND_ROM8/16(address) for the program ROM (0000-9FFF), the names
sound_symbols.txt gives them where it does; a computed address goes over the board's bus,
z80_read8/16() and z80_write8/16() (runtime/include/sound/support.h). The timing model's Effects
(z80_block, z80_cycles), DI and EI, OUT and the block moves print as calls of that header.
"""
from decompiler import emit
from decompiler import memory
from v60 import ir

SHARED_RAM = (0xE000, 0x10000)
PROGRAM_ROM = (0x0000, 0xA000)


class SoundMemory:
    """The names of the sound program's memory, with the interface decompiler/emit.py asks of its
    memory.Memory: access(), register(), scoped, declarations()."""

    def __init__(self, variables=None):
        self.variables = variables or {}    # address -> (name, bits)
        self.scoped = {}
        self.tables = {}
        self.overlays = {}

    @staticmethod
    def place(address, bits):
        """SOUND_RAM8(0xE701) or SOUND_ROM16(0x4706), or None."""
        if SHARED_RAM[0] <= address and address + bits // 8 <= SHARED_RAM[1]:
            return f"SOUND_RAM{bits}(0x{address:04X}u)", True
        if PROGRAM_ROM[0] <= address and address + bits // 8 <= PROGRAM_ROM[1]:
            return f"SOUND_ROM{bits}(0x{address:04X}u)", False
        return None

    def access(self, address, bits, index_text):
        if bits not in (8, 16):
            return None
        value = memory.constant_address(address)
        if value is None:
            return None
        named = self.variables.get(value)
        if named is not None and named[1] == bits:
            return named[0], False, True
        found = self.place(value, bits)
        if found is None:
            return None
        return found[0], False, found[1]

    def register(self, value):
        return None

    def declarations(self):
        return [f"#define {name} SOUND_RAM{bits}(0x{address:04X}u)"
                for address, (name, bits) in sorted(self.variables.items())]


class SoundNames(emit.Names):
    def memory(self, load):
        found = super().memory(load)
        if found is not None:
            return found
        # a computed address: over the board's bus
        return f"z80_read{load.bits}({self.writer.exact(load.address)})", False


class SoundPrinter(emit.Printer):
    def __init__(self, writer):
        super().__init__(writer)
        self.names = SoundNames(writer)


class SoundWriter(emit.FunctionWriter):
    def __init__(self, *args, **kwargs):
        super().__init__(*args, **kwargs)
        self.printer = SoundPrinter(self)

    def statement(self, statement):
        if isinstance(statement, ir.Store):
            target = self.memory(statement.address, statement.bits, id(statement))
            if target is not None and target[2]:
                return [self.assignment(target[0], statement, target[1])]
            if target is not None:
                raise emit.EmitError(f"{self.function.name}: writes the program ROM")
            return [f"z80_write{statement.bits}({self.exact(statement.address)}, {self.stored(statement.value, statement.bits)});"]
        return super().statement(statement)

    def effect(self, statement):
        name = statement.name
        args = statement.args
        if name == "z80_block":
            return [f"z80_block({args[0].value});"]
        if name == "z80_cycles":
            return [f"z80_cycles({args[0].value});"]
        if name == "interrupts_off":
            return ["interrupts_off();"]
        if name == "interrupts_on":
            # EI as a block's last instruction: the interrupt waits for the block after the next start
            return ["interrupts_on_after_block();" if args[0].value else "interrupts_on();"]
        if name == "port_out":
            return [f"port_out({self.exact(args[0])}, {self.exact(args[1])});"]
        if name == "move_block":
            source, destination, count, upward = (self.exact(arg) for arg in args)
            return [f"move_block({source}, {destination}, {count}, {upward});"]
        raise emit.EmitError(f"{self.function.name}: effect {name}")


class SoundEmitter(emit.Emitter):
    """emit.Emitter with nothing of the V60 program's: no context registers, no structures."""

    def __init__(self, functions, signatures, names, memory_, always):
        super().__init__(functions, signatures, names, (), set(), memory_, always, lambda address: 0)
        self.layouts = NoLayouts()
        self.flow = None

    def restores_current(self, entry):
        return None, None


class NoLayouts:
    """No structures (yet): pointers.Layouts' answers when nothing is described."""

    def families(self, struct):
        return []

    def declarations(self):
        return []
