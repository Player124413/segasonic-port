"""
Name:        memory.py
Description: the game's variables, tables and hardware registers by name: work RAM variables as C
             lvalues laid over the original RAM, program ROM tables as arrays, the board's registers as
             named addresses
Version:     0.1.0
Created:     12-10-2026
Modified:    13-10-2026
Author:      Sebastian Toyos

Work RAM (0x200000-0x20FFFF) stays the board's byte array, so the state compares byte for byte with the
reference. A variable is a macro for a typed lvalue at its address (game/support.h: RAM8, RAM16, RAM32):
"frames_to_wait = 1;", "unk_20E5A2 &= 0x7Fu;". One the code reads with an index ("[R0]") is an array:
"unk_20E590[r0]". The program ROM's data is read the same way through rom(): "rom_10B6CC[r1]". The
board's registers keep going through the bus (reading or writing them does things), by name:
"mem_write8(IO_PORT_D, r2)".

Names come from symbols.txt (var, io and table lines, disassemble.load_names). Whatever has none yet
gets one made of its address, with the width the code uses most: unk_ for work RAM, rom_ for tables,
and the board area for registers (VIDEO_, IO_...). An access with another width than the variable's,
or into the middle of it, says so (AS16(name), AT8(name, 1)). An index the analysis cannot show to be
small stays an address ("RAM16(0x20E562u + r3)"), so it wraps the way the V60's did.

Writes that do something besides storing (the protection device at 0x20E5C4, see machine.c) keep going
through the bus.

The program ROM shows up several times in the address space (sonic_rom.PROGRAM_AREAS): code that runs in
a mirror reads its tables through that mirror. A table is named, and read, at its own copy.
"""
from collections import Counter

import sonic_rom
from v60 import ir

WORK_RAM = (0x200000, 0x210000)
TYPES = {8: "uint8_t", 16: "uint16_t", 32: "uint32_t"}
SIGNED = {"u8": (8, False), "s8": (8, True), "u16": (16, False), "s16": (16, True),
          "u32": (32, False), "s32": (32, True)}
BUS_ONLY = [(0x20E5C4, 0x20E5C6)]    # writes the protection device watches
VARIABLES = 0x208000    # where the game's variables start: R25 of the original, which some code adds offsets to
HARDWARE = [(0x300000, 0x320000, "VIDEO"), (0x400000, 0x420000, "SPRITE_RAM"), (0x500000, 0x500010, "SPRITE_CONTROL"),
            (0x600000, 0x610000, "PALETTE"), (0x610000, 0x620000, "MIXER"), (0x700000, 0x702000, "SOUND"),
            (0xC00000, 0xC00080, "IO"), (0xD00000, 0xD00010, "IRQ"), (0xD80000, 0xD80010, "RANDOM")]


def work_ram(address):
    return WORK_RAM[0] <= address < WORK_RAM[1]


def program_rom(address):
    return address < 0x200000 or address >= 0xF00000


def own_copy(address):
    """A ROM address at the copy the program's tables are named by; any other address as it is."""
    if program_rom(address):
        found = sonic_rom.canonical_address(address)
        return found if found is not None else address
    return address


def hardware_area(address):
    for start, end, name in HARDWARE:
        if start <= address < end:
            return name
    return None


def bus_only(address, bits):
    end = address + bits // 8
    return any(address < stop and start < end for start, stop in BUS_ONLY)


def constant_address(expression):
    """The address an expression always gives (24 bits), or None."""
    if isinstance(expression, ir.Const):
        return expression.value & 0xFFFFFF
    return None


def based(expression):
    """(constant base, rest) for "base + rest", or None."""
    if isinstance(expression, ir.Op) and expression.name == "add":
        left, right = expression.args
        if isinstance(left, ir.Const) and not isinstance(right, ir.Const):
            return left.value & 0xFFFFFF, right
        if isinstance(right, ir.Const) and not isinstance(left, ir.Const):
            return right.value & 0xFFFFFF, left
    return None


def paged(expression):
    """(base, rest) for "page | (uint16_t)(low + rest)": a table read with 16-bit arithmetic in the 64 KB page
    it is in (the code adds the index to the low half of a register that holds the page), or None."""
    if not (isinstance(expression, ir.Op) and expression.name == "or"):
        return None
    for page, low in (expression.args, reversed(expression.args)):
        if not isinstance(page, ir.Const) or page.value & 0xFFFF:
            continue
        narrow = False
        while isinstance(low, ir.Op) and low.name in ("zext", "fits", "trunc"):
            narrow = narrow or (low.args[0].bits if low.name == "zext" else low.bits) <= 16
            low = low.args[0]
        if isinstance(low, ir.Op) and low.name == "add" and (narrow or low.bits <= 16):
            parts = based(low)
            if parts is not None:
                return (page.value | parts[0]) & 0xFFFFFF, parts[1]
    return None


def table_index(rest, bits):
    """True if "base + rest" reads an element of a table or array at 'base': the index is small, or scaled by
    the size of the elements."""
    index = element_index(rest, bits)
    if index is not None and bits > 8 and index is not rest:
        return True
    return small(index if index is not None else rest)


def element_index(rest, bits):
    """The element number when 'rest' is a byte offset into an array of 'bits'-wide elements, or None."""
    size = bits // 8
    if size == 1:
        return rest
    if isinstance(rest, ir.Op) and rest.name == "mul":
        for value, other in ((rest.args[1], rest.args[0]), (rest.args[0], rest.args[1])):
            if isinstance(value, ir.Const) and value.value == size:
                return other
    if isinstance(rest, ir.Op) and rest.name == "shl" and isinstance(rest.args[1], ir.Const) \
            and 1 << rest.args[1].value == size:
        return rest.args[0]
    return None


def small(index):
    """True for an index that cannot be negative or huge: a zero-extended byte or halfword."""
    if isinstance(index, ir.Const):
        return index.value < 0x10000
    if isinstance(index, ir.Op):
        if index.name in ("zext", "fits") and index.args[0].bits <= 16:
            return True
        if index.name == "fits" and index.bits <= 16:
            return True
        if index.name == "and" and any(isinstance(arg, ir.Const) and arg.value < 0x10000 for arg in index.args):
            return True
    if isinstance(index, ir.Load) and index.bits <= 16:
        return True
    return False


class Item:
    def __init__(self, address, name, bits, signed=False):
        self.address = address
        self.name = name
        self.bits = bits
        self.signed = signed
        self.indexed = False


class Memory:
    def __init__(self, names, accesses, data_names=None):
        """'names': disassemble.Names; 'accesses': (address expression, bits) of every read and write;
        'data_names': {address: name} of the data labels symbols.txt has (tables of code addresses), which
        name the tables that start there."""
        self.variables = {}
        self.tables = {}
        self.hardware = {}
        self.overlays = {}    # (address, bits) -> Item: a narrower name for a variable's first bytes
        self.scoped = {}      # family -> {address: Item}: work RAM a family of functions uses its own way
        for (struct, family), found in names.scoped.items():
            if struct == "global":
                self.scoped[family] = {address: Item(address, name, *SIGNED[kind])
                                       for address, (name, kind) in found.items()}
        for (struct, address, bits), (name, kind) in names.overlays.items():
            if struct == "global":
                self.overlays[(address, bits)] = Item(address, name, *SIGNED[kind])
        for address, (name, kind) in names.variables.items():
            bits, signed = SIGNED[kind]
            self.variables[address] = Item(address, name, bits, signed)
        for address, (name, kind) in names.tables.items():
            bits, signed = SIGNED[kind]
            self.tables[address] = Item(address, name, bits, signed)
        for address, (name, kind) in names.hardware.items():
            self.hardware[address] = Item(address, name, SIGNED[kind][0])

        widths = {"variables": {}, "tables": {}, "hardware": {}}
        indexed = set()
        for address_expression, bits in accesses:
            if bits not in TYPES:
                continue
            value = constant_address(address_expression)
            parts = (based(address_expression) or paged(address_expression)) if value is None else None
            if parts is not None:
                if not table_index(parts[1], bits):
                    continue    # maybe a pointer and a displacement (access() keeps it on the bus)
                if 0x208400 <= parts[0] < 0x208A00:
                    continue    # a field of the player blocks, reached by the player's number (pointers.py)
                value = parts[0]
            if value is None:
                continue
            value = own_copy(value)
            where = "variables" if work_ram(value) else "tables" if program_rom(value) else \
                "hardware" if hardware_area(value) else None
            if where is None or (where == "hardware" and parts is not None):
                continue
            widths[where].setdefault(value, Counter())[bits] += 1
            if parts is not None:
                indexed.add(value)
        self.add_unnamed(self.variables, widths["variables"], lambda address: f"unk_{address:06X}")
        data_names = data_names or {}
        self.add_unnamed(self.tables, widths["tables"],
                         lambda address: data_names.get(address, f"rom_{address:06X}"))
        self.add_unnamed(self.hardware, widths["hardware"],
                         lambda address: f"{hardware_area(address)}_{address:06X}")
        for address in indexed:
            for items in (self.variables, self.tables):
                if address in items:
                    items[address].indexed = True

    def add_unnamed(self, items, widths, name_of):
        for address, counts in sorted(widths.items()):
            if address in items or self.inside(items, address) is not None:
                continue
            bits = max(counts, key=lambda width: (counts[width], -width))
            items[address] = Item(address, name_of(address), bits)

    @staticmethod
    def inside(items, address):
        """An item that 'address' falls inside of (not at its start), or None."""
        for start in range(address - 1, address - 4, -1):
            item = items.get(start)
            if item is not None and not item.indexed and start + item.bits // 8 > address:
                return item
        return None

    # ---- what the printer asks ----

    def scalar(self, items, address, bits):
        item = items.get(address)
        if item is not None:
            name = f"{item.name}[0]" if item.indexed else item.name
            overlay = self.overlays.get((address, bits)) if items is self.variables else None
            if item.bits != bits and overlay is not None:
                return overlay.name
            return name if item.bits == bits else f"AS{bits}({name})"
        outer = self.inside(items, address)
        if outer is not None:
            return f"AT{bits}({outer.name}, {address - outer.address})"
        return None

    def access(self, address, bits, index_text):
        """C for an access of 'bits' at 'address' (an IR expression): (text, signed, is RAM) or None. The
        text is an lvalue for work RAM and an rvalue for the ROM. 'index_text' prints an index."""
        if bits not in TYPES:
            return None
        value = constant_address(address)
        if value is not None:
            value = own_copy(value)
            for items, ram in ((self.variables, True), (self.tables, False)):
                if (work_ram(value) if ram else program_rom(value)):
                    text = self.scalar(items, value, bits)
                    if text is None:
                        return None
                    item = items.get(value)
                    overlay = self.overlays.get((value, bits)) if ram else None
                    if item is not None and item.bits != bits and overlay is not None:
                        return text, overlay.signed, ram
                    return text, item is not None and item.signed and item.bits == bits, ram
            return None
        parts = based(address) or paged(address)
        if parts is None:
            return None
        base, rest = parts
        base = own_copy(base)
        # "constant + register" is a table and an index only when the index is small, or scaled by the size
        # of the table's elements; a register that may hold anything could be a pointer with a
        # displacement, and that read stays on the bus
        index = element_index(rest, bits)
        if not table_index(rest, bits):
            return None
        ram = work_ram(base)
        if not ram and (not program_rom(base) or base + 0xFFFF * (bits // 8) >= 0x200000):
            return None
        items = self.variables if ram else self.tables
        loaded = rest
        while isinstance(loaded, ir.Op) and loaded.name in ("zext", "fits"):
            loaded = loaded.args[0]
        if base == VARIABLES and isinstance(loaded, ir.Load) and base not in items:
            # R25 plus an offset the code reads from a table (direction_to_player_offsets): the variable
            # that many bytes into the variables
            return f"RAM{bits}(GAME_VARIABLES + {index_text(rest)})", False, True
        if base not in items:
            return None
        item = items[base]
        if index is not None and item.bits == bits:
            return f"{item.name}[{index_text(index)}]", item.signed, ram
        macro = ("RAM" if ram else "ROM") + str(bits)
        start = "GAME_VARIABLES" if base == VARIABLES else f"{item.name}_ADDRESS"
        return f"{macro}({start} + {index_text(rest)})", False, ram

    def register(self, value):
        """Name of a hardware register at 'value', or None."""
        item = self.hardware.get(value)
        return item.name if item is not None else None

    # ---- the header ----

    def declarations(self):
        lines = ["/* the game's variables in work RAM (the original code reaches them from R25, which holds",
                 "   GAME_VARIABLES; some code adds offsets it reads from tables) */",
                 f"#define GAME_VARIABLES 0x{VARIABLES:06X}u"]
        for address, item in sorted(self.variables.items()):
            lines += self.declare(item, "RAM", "ram")
            for (start, bits), overlay in sorted(self.overlays.items()):
                if start == address:
                    lines += self.declare(overlay, "RAM", "ram")
        if self.scoped:
            lines += ["", "/* work RAM as one family of functions uses it (their scratch variables) */"]
            for family, items in sorted(self.scoped.items()):
                for address, item in sorted(items.items()):
                    lines += self.declare(item, "RAM", "ram")
        lines += ["", "/* data in the program ROM the code reads */"]
        for address, item in sorted(self.tables.items()):
            lines += self.declare(item, "ROM", "rom")
        lines += ["", "/* the board's registers (read and written through the bus) */"]
        for address, item in sorted(self.hardware.items()):
            lines.append(f"#define {item.name} 0x{address:06X}u")
        return lines

    @staticmethod
    def declare(item, macro, function):
        kind = TYPES[item.bits]
        if item.signed:
            kind = kind[1:]
        constness = "const " if macro == "ROM" else ""
        lines = [f"#define {item.name}_ADDRESS 0x{item.address:06X}u"]
        if item.indexed:
            lines.append(f"#define {item.name} (({constness}{kind} *){function}({item.name}_ADDRESS))")
        else:
            suffix = "S" if item.signed else ""
            lines.append(f"#define {item.name} {macro}{item.bits}{suffix}({item.name}_ADDRESS)")
        return lines


def accesses_of(functions):
    """(address expression, bits) of every memory read and write of the functions."""
    found = []
    for function in functions:
        for block in function.blocks.values():
            for statement in block.statements + [block.end]:
                if isinstance(statement, ir.Store):
                    found.append((statement.address, statement.bits))
                for expression in ir.statement_expressions(statement):
                    for node in ir.walk(expression):
                        if isinstance(node, ir.Load):
                            found.append((node.address, node.bits))
    return found
