"""
Name:        disassemble.py
Description: recursive-descent disassembly of the V60 program into a listing and a coverage report
Version:     0.1.0
Created:     11-10-2026
Modified:    11-10-2026
Author:      Sebastian Toyos

Usage: python tools/disassemble.py [rom folder or .zip]   (default: rom/)
Writes build/listing/sonic.s and build/listing/report.txt.

Starts at the entry points in symbols.txt and follows every branch, call and jump whose
target can be known without running the game. Whatever is never reached stays as data.
On top of plain flow it understands two patterns the game uses everywhere:
  - branch tables: "jmp table[PC](R0)" lands on a run of "br" instructions (with "nop" padding)
  - pointer tables: "mov.w table[PC](R0), R0" followed by "jsr [R0]" reads a table of addresses
  - code pointer slots: "movea.b func[PC], 1C[R20]" stores a handler that "jsr [1C[R20]]" calls
A stored or tabled address is accepted as code only if a trial decode of it is clean.
"""
import sys
from collections import deque
from pathlib import Path

TOOLS = Path(__file__).resolve().parent
ROOT = TOOLS.parent
sys.path.insert(0, str(TOOLS))
import sonic_rom  # noqa: E402
from v60 import disassembler as dis  # noqa: E402

SYMBOL_KINDS = ("func", "code", "data")
STACK_POINTER = 31


def load_symbols(path):
    """Read symbols.txt: {canonical address: (kind, name)}."""
    symbols = {}
    for number, line in enumerate(path.read_text(encoding="utf-8").splitlines(), 1):
        line = line.split("#", 1)[0].strip()
        if not line:
            continue
        parts = line.split()
        if len(parts) != 3 or parts[1] not in SYMBOL_KINDS:
            raise SystemExit(f"{path.name}:{number}: expected 'ADDRESS KIND NAME' with KIND in {SYMBOL_KINDS}")
        address = sonic_rom.canonical_address(int(parts[0], 16))
        if address is None:
            raise SystemExit(f"{path.name}:{number}: {parts[0]} is not in the program ROM")
        symbols[address] = (parts[1], parts[2])
    return symbols


class Analysis:
    def __init__(self, image, symbols):
        self.image = image
        self.mem = sonic_rom.ProgramMemory(image)
        self.symbols = symbols
        self.instructions = {}   # start address -> Instruction
        self.owner = {}          # every byte of decoded code -> start of its instruction
        self.labels = {}         # address -> "func" | "loc" | "data"
        self.invalid = []        # (address, text, where the path came from)
        self.unresolved = []     # (address, text): jumps and calls whose target needs run-time data
        self.overlaps = []       # (address, start of the instruction already covering it, origin)
        self.outside = []        # (address, target): targets outside the program ROM
        self.slots = set()       # displacements of memory slots that indirect calls read their target from
        self.pointer_writes = [] # (address of the store, stored value, slot displacement)
        self.rejected = []       # (candidate, address of the store): stored values that do not decode cleanly
        self.data_words = {}     # address of a pointer table entry -> address it points to
        self.pending = deque()

    def add_target(self, address, kind, origin, label=True):
        canonical = sonic_rom.canonical_address(address)
        if canonical is None:
            self.outside.append((origin, address))
            return
        if label and (kind == "func" or canonical not in self.labels):
            self.labels[canonical] = kind
        self.pending.append((canonical, origin))

    def run(self):
        for address, (kind, _) in sorted(self.symbols.items()):
            if kind == "data":
                self.labels[address] = "data"
            else:
                self.add_target(address, "func" if kind == "func" else "loc", address)
        tried = set()
        while True:
            while self.pending:
                address, origin = self.pending.popleft()
                self.trace(address, origin)
            # stored code pointers whose slot is read by an indirect call
            found = False
            for store, value, slot in self.pointer_writes:
                candidate = sonic_rom.canonical_address(value)
                if slot not in self.slots or candidate is None or candidate in self.instructions \
                        or (candidate, store) in tried:
                    continue
                tried.add((candidate, store))
                if self.trial(candidate):
                    self.add_target(candidate, "func", store)
                    found = True
                else:
                    self.rejected.append((candidate, store))
            if not found:
                return

    def trial(self, start, limit=4000):
        """Decode one function's local flow without committing it: clean if no invalid or overlapping code."""
        seen = set()
        stack = [start]
        while stack and len(seen) < limit:
            address = stack.pop()
            while address not in seen and address not in self.instructions:
                if address in self.owner:
                    return False
                instruction = dis.decode(self.mem, address)
                last_byte = address + instruction.length - 1
                if instruction.flow == dis.FLOW_INVALID or sonic_rom.canonical_address(last_byte) != last_byte:
                    return False
                seen.add(address)
                if instruction.target is not None:
                    target = sonic_rom.canonical_address(instruction.target)
                    if target is None:
                        return False
                    if instruction.flow in (dis.FLOW_BRANCH, dis.FLOW_JUMP):
                        stack.append(target)
                if instruction.flow in (dis.FLOW_JUMP, dis.FLOW_RETURN):
                    break
                address += instruction.length
        return True

    def branch_table(self, base, called, origin):
        """A run of 'br' entries (and 'nop' padding) indexed by a register."""
        cursor = sonic_rom.canonical_address(base)
        if cursor is None:
            self.outside.append((origin, base))
            return
        self.labels.setdefault(cursor, "loc")
        for _ in range(1024):
            entry = dis.decode(self.mem, cursor)
            if entry.mnemonic == "br":
                self.add_target(cursor, "loc", origin, label=False)
                if called:
                    target = sonic_rom.canonical_address(entry.target)
                    if target is not None:
                        self.labels[target] = "func"
            elif entry.mnemonic == "nop":
                self.add_target(cursor, "loc", origin, label=False)
            else:
                return
            cursor += entry.length

    def pointer_table(self, base, called, origin):
        """A run of 32-bit code addresses indexed by a register; ends at the first entry that is not code."""
        cursor = sonic_rom.canonical_address(base)
        if cursor is None:
            self.outside.append((origin, base))
            return
        self.labels.setdefault(cursor, "data")
        for _ in range(512):
            target = sonic_rom.canonical_address(self.mem.r32(cursor))
            if target is None or not self.trial(target):
                return
            self.data_words[cursor] = target
            self.add_target(target, "func" if called else "loc", origin)
            cursor += 4

    def trace(self, address, origin):
        previous = None
        while True:
            if address in self.instructions:
                return
            if address in self.owner:
                self.overlaps.append((address, self.owner[address], origin))
                return
            instruction = dis.decode(self.mem, address)
            last_byte = address + instruction.length - 1
            if instruction.flow == dis.FLOW_INVALID or sonic_rom.canonical_address(last_byte) != last_byte:
                self.invalid.append((address, instruction.text, origin))
                return
            taken = [a for a in range(address, last_byte + 1) if a in self.owner]
            if taken:
                self.overlaps.append((address, self.owner[taken[0]], origin))
                return

            self.instructions[address] = instruction
            for byte_address in range(address, last_byte + 1):
                self.owner[byte_address] = address
            self.follow(instruction, previous)
            if instruction.flow in (dis.FLOW_JUMP, dis.FLOW_RETURN):
                return
            previous = instruction
            address += instruction.length

    def follow(self, instruction, previous):
        kind = "func" if instruction.flow == dis.FLOW_CALL else "loc"
        first = instruction.operands[0] if instruction.operands else None
        if instruction.target is not None:
            self.add_target(instruction.target, kind, instruction.address)
        elif instruction.target_pointer is not None:
            pointer = sonic_rom.canonical_address(instruction.target_pointer)
            if pointer is None:
                self.unresolved.append((instruction.address, instruction.text))
            else:
                self.labels.setdefault(pointer, "data")
                self.add_target(self.mem.r32(pointer), kind, instruction.address)
        elif instruction.mnemonic in ("jmp", "jsr") and first.mode in ("pc_disp_indexed", "direct_indexed"):
            self.branch_table(first.value, instruction.mnemonic == "jsr", instruction.address)
        elif instruction.mnemonic in ("jmp", "jsr") and first.mode == "reg_indirect" \
                and self.loads_table_entry(previous, first.reg):
            self.pointer_table(previous.operands[0].value, instruction.mnemonic == "jsr", instruction.address)
        elif instruction.mnemonic in ("jmp", "jsr", "call"):
            self.unresolved.append((instruction.address, instruction.text))
            if first is not None and first.mode == "disp_indirect":
                self.slots.add(first.disp)

        # code pointer stores: "movea.b func[PC], slot" or "mov.w #func, slot"
        if len(instruction.operands) == 2 and instruction.flow == dis.FLOW_NORMAL:
            source, dest = instruction.operands
            if dest.mode == "disp":
                if instruction.mnemonic.startswith("movea") and source.mode == "pc_disp":
                    self.pointer_writes.append((instruction.address, source.value, dest.disp))
                elif instruction.mnemonic == "mov.w" and source.mode == "immediate":
                    self.pointer_writes.append((instruction.address, source.value, dest.disp))

        # "movea target[PC], [-SP]" right before a return: the return lands on target
        if instruction.flow == dis.FLOW_RETURN and previous is not None \
                and previous.mnemonic.startswith("movea") and len(previous.operands) == 2:
            source, dest = previous.operands
            if source.mode == "pc_disp" and dest.mode == "autodec" and dest.reg == STACK_POINTER:
                self.add_target(source.value, "loc", previous.address)

    @staticmethod
    def loads_table_entry(previous, reg):
        """True for "mov.w table[PC](Rx), reg": a 32-bit read from a PC-relative table into reg."""
        if previous is None or previous.mnemonic != "mov.w" or len(previous.operands) != 2:
            return False
        source, dest = previous.operands
        return source.mode == "pc_disp_indexed" and dest.mode == "reg" and dest.reg == reg

    def label_name(self, address):
        if address in self.symbols:
            return self.symbols[address][1]
        kind = self.labels.get(address)
        if kind is None:
            return None
        prefix = {"func": "func", "loc": "loc", "data": "data"}[kind]
        return f"{prefix}_{address:06X}"

    def target_comment(self, instruction):
        target = instruction.target
        if target is None and instruction.target_pointer is not None:
            target = instruction.target_pointer
        if target is None:
            return ""
        canonical = sonic_rom.canonical_address(target)
        name = self.label_name(canonical) if canonical is not None else None
        return f"  ; {name}" if name else ""


def write_listing(analysis, path):
    image = analysis.image
    with open(path, "w", encoding="ascii", newline="\n") as out:
        out.write("; SegaSonic The Hedgehog (sonic, Japan rev. C) - V60 program\n")
        out.write("; Generated by tools/disassemble.py from the user's own ROM. Do not redistribute.\n")
        for start, end in sonic_rom.PROGRAM_AREAS:
            out.write(f"\n; ======== {start:06X}-{end - 1:06X} ========\n")
            address = start
            while address < end:
                label = analysis.label_name(address)
                if label:
                    out.write(f"\n{label}:\n")
                instruction = analysis.instructions.get(address)
                if instruction:
                    raw = image[address:address + min(instruction.length, 10)].hex(" ")
                    if instruction.length > 10:
                        raw += " .."
                    comment = analysis.target_comment(instruction)
                    out.write(f"    {address:06X}  {raw:<32} {instruction.text}{comment}\n")
                    address += instruction.length
                    continue
                if address in analysis.data_words:
                    value = int.from_bytes(image[address:address + 4], "little")
                    name = analysis.label_name(analysis.data_words[address])
                    out.write(f"    {address:06X}  .long {value:08X}{'':<38} ; {name}\n")
                    address += 4
                    continue
                stop = address + 1
                while stop < end and stop - address < 16 and stop not in analysis.instructions \
                        and stop not in analysis.data_words and analysis.label_name(stop) is None:
                    stop += 1
                chunk = image[address:stop]
                text = "".join(chr(b) if 0x20 <= b < 0x7F else "." for b in chunk)
                values = ",".join(f"{b:02X}" for b in chunk)
                out.write(f"    {address:06X}  .byte {values:<47} ; {text}\n")
                address = stop


def write_report(analysis, path):
    lines = ["V60 program analysis", "====================", ""]
    total_code = 0
    total_size = 0
    for start, end in sonic_rom.PROGRAM_AREAS:
        code = sum(1 for a in range(start, end) if a in analysis.owner)
        total_code += code
        total_size += end - start
        lines.append(f"{start:06X}-{end - 1:06X}: {code:7} of {end - start:7} bytes decoded as code "
                     f"({100 * code / (end - start):5.1f}%)")
    lines.append(f"total:         {total_code:7} of {total_size:7} bytes ({100 * total_code / total_size:5.1f}%)")
    functions = sum(1 for kind in analysis.labels.values() if kind == "func")
    lines += [
        "",
        f"instructions:        {len(analysis.instructions)}",
        f"functions:           {functions}",
        f"code labels:         {sum(1 for kind in analysis.labels.values() if kind == 'loc')}",
        f"unresolved jumps:    {len(analysis.unresolved)}",
        f"invalid paths:       {len(analysis.invalid)}",
        f"overlapping paths:   {len(analysis.overlaps)}",
        f"targets outside ROM: {len(analysis.outside)}",
    ]
    sections = [
        ("Unresolved jumps and calls (target comes from registers or RAM)", analysis.unresolved,
         lambda item: f"{item[0]:06X}  {item[1]}"),
        ("Invalid instructions reached", analysis.invalid,
         lambda item: f"{item[0]:06X}  {item[1]:<30} reached from {item[2]:06X}"),
        ("Overlapping instructions", analysis.overlaps,
         lambda item: f"{item[0]:06X}  inside instruction at {item[1]:06X}, reached from {item[2]:06X}"),
        ("Targets outside the program ROM", analysis.outside,
         lambda item: f"{item[0]:06X}  -> {item[1]:08X}"),
        ("Stored code pointers rejected by the trial decode", analysis.rejected,
         lambda item: f"{item[0]:06X}  stored at {item[1]:06X}"),
    ]
    for title, items, render in sections:
        if items:
            lines += ["", title, "-" * len(title)]
            lines += [render(item) for item in sorted(items)]
    path.write_text("\n".join(lines) + "\n", encoding="ascii")
    return lines


def main():
    source = Path(sys.argv[1]) if len(sys.argv) > 1 else ROOT / "rom"
    image = sonic_rom.build_program(sonic_rom.load_set(source))
    analysis = Analysis(image, load_symbols(ROOT / "symbols.txt"))
    analysis.run()

    out_dir = ROOT / "build" / "listing"
    out_dir.mkdir(parents=True, exist_ok=True)
    write_listing(analysis, out_dir / "sonic.s")
    report = write_report(analysis, out_dir / "report.txt")
    print("\n".join(report[:17]))
    print(f"\nListing: {out_dir / 'sonic.s'}\nReport:  {out_dir / 'report.txt'}")
    return 0


if __name__ == "__main__":
    sys.exit(main())
