"""
Name:        decompile_sound.py
Description: decompiles the Z80 sound program into readable C, one C function per original function
Version:     0.1.0
Created:     13-10-2026
Modified:    13-10-2026
Author:      Sebastian Toyos

Usage: python tools/decompile_sound.py [--check] [--out DIR] [--timing FILE]
  --check     build every function's IR and report what does not lift and what the analysis finds,
              without writing anything
  --out       where to write the C (src/sound/ by default)
  --timing    where to write the blocks of the timing model for tools/reference/s32ref (--sound-timing;
              build/sound/timing.txt by default)

The sound program (epr-15785, read from the ROM set in rom/ by tools/sonic_rom.py) goes
through the decompiler the V60 program goes through (tools/decompiler/), with the Z80's profile
(decompiler/cpu.py): tools/z80/analysis.py splits it into functions with sound_symbols.txt,
tools/z80/function.py lifts them to IR with the sound board's timing model, and the passes work out
what each function takes and hands back. It writes src/sound/: sound.h (prototypes) and sound_NN.c (the
functions, in the order of their Z80 addresses).
"""
import re
import sys
import time
from collections import Counter
from pathlib import Path

TOOLS = Path(__file__).resolve().parent
ROOT = TOOLS.parent
sys.path.insert(0, str(TOOLS))
from decompile import call_results  # noqa: E402
import sonic_rom  # noqa: E402
from decompiler import cleanup  # noqa: E402
from decompiler import conditions  # noqa: E402
from decompiler import cpu  # noqa: E402
from decompiler import dataflow  # noqa: E402
from decompiler import expressions  # noqa: E402
from decompiler import knownbits  # noqa: E402
from decompiler import preserved  # noqa: E402
from decompiler import signatures  # noqa: E402
from decompiler import stack  # noqa: E402
from decompiler import structure  # noqa: E402
from v60 import ir  # noqa: E402
from z80 import analysis  # noqa: E402
from z80 import emit  # noqa: E402
from z80 import function as functions  # noqa: E402
from z80 import lift  # noqa: E402

SYMBOLS = ROOT / "sound_symbols.txt"
ROM_SET = ROOT / "rom"
INTERRUPT_HANDLER = 0x0038
FUNCTIONS_PER_FILE = 120
CREATED = "13-10-2026"    # when the C was first written; the files say when it was last written too
IDENTIFIER = re.compile(r"[A-Za-z_][A-Za-z0-9_]*\Z")


def load_program():
    if not ROM_SET.exists():
        raise SystemExit(f"{ROM_SET}: no such ROM folder")
    return analysis.load_program(sonic_rom.build_sound(sonic_rom.load_set(ROM_SET)), SYMBOLS)


def configure():
    """The Z80 profile, and the sound program's own: no registers it keeps for the whole program."""
    cpu.use(cpu.Z80)
    signatures.CONTEXT = ()
    signatures.SCRATCH_CONTEXT = {}
    signatures.CONSTANT = {}


def build_functions(program):
    builder = functions.Builder(program)
    built = {}
    failures = []
    for entry in sorted(program.entries):
        try:
            built[entry] = builder.build(entry)
        except lift.Unsupported as problem:
            failures.append((entry, str(problem)))
    return built, failures


def analyse(program, built):
    """Stack slots, preserved registers and the data flow summaries, in place."""
    for function in built.values():
        stack.analyse(function)
    flow = dataflow.Program(built, program.analysis, [INTERRUPT_HANDLER], signatures.CONTEXT)
    flow.solve()
    while sum(len(preserved.analyse(function, flow.call_writes, context=signatures.CONTEXT))
              for function in built.values()):
        flow.solve()
    changed = sum(conditions.recover(function, flow) for function in built.values())
    if changed:
        flow.reset()
        flow.solve()
    flow.conditions_recovered = changed
    return flow


def name(variable):
    if variable[0] == "r":
        return lift.NAMES.get(variable[1], f"r{variable[1]}")
    return variable[1]


def check(program):
    built, failures = build_functions(program)
    for entry, function in built.items():
        try:
            stack.analyse(function)
        except stack.StackError as problem:
            failures.append((entry, str(problem)))
        left = stack.leftovers(function)
        if left:
            failures.append((entry, f"{len(left)} statements still use SP, the first: {left[0]}"))
    ends = Counter(type(block.end).__name__ for function in built.values() for block in function.blocks.values())
    print(f"functions:  {len(built)}")
    print(f"blocks:     {sum(ends.values())}")
    print("block ends: " + ", ".join(f"{kind} {count}" for kind, count in ends.most_common()))
    print(f"problems:   {len(failures)}")
    for entry, problem in failures[:20]:
        print(f"  {entry:04X}  {problem}")
    if failures:
        return 1
    built, _ = build_functions(program)
    flow = analyse(program, built)
    found = signatures.compute(flow)
    kept = Counter(len(getattr(function, "preserved", ())) for function in built.values())
    print("preserved registers per function: " + ", ".join(f"{k}: {v}" for k, v in sorted(kept.items())))
    print(f"conditions recovered: {flow.conditions_recovered}")
    for entry in sorted(built):
        signature = found[entry]
        params = ", ".join(sorted(name(variable) for variable in signature.params))
        results = ", ".join(sorted(name(variable) for variable in signature.results))
        print(f"  {entry:04X} {program.names[entry]:<24} takes ({params})  hands back ({results})")
    handler = flow.summaries[INTERRUPT_HANDLER]
    changed = sorted(name(variable) for variable in handler.writes if variable[0] == "r")
    print("interrupt handler writes: " + (", ".join(changed) or "nothing"))
    return 0


def file_header(name, description):
    """The header every file of the project starts with, for a file this program writes."""
    return ("/**\n"
            f" *    Name:        {name}\n"
            f" *    Description: {description}\n"
            " *    Version:     0.1.0\n"
            f" *    Created:     {CREATED}\n"
            f" *    Modified:    {time.strftime('%d-%m-%Y')}\n"
            " *    Author:      Sebastian Toyos\n"
            " *\n"
            " *    SegaSonic The Hedgehog (arcade, Sega System 32): the Z80 sound program decompiled into C by\n"
            " *    tools/decompile_sound.py, with the names of sound_symbols.txt.\n"
            " */\n")


def write_c(program, out_dir):
    built, failures = build_functions(program)
    if failures:
        raise SystemExit(f"{len(failures)} functions do not lift, the first: {failures[0][0]:04X} {failures[0][1]}")
    flow = analyse(program, built)
    found = signatures.compute(flow)
    names = dict(program.names)
    for entry, function_name in names.items():
        if not IDENTIFIER.match(function_name):
            raise SystemExit(f"{entry:04X}: '{function_name}' cannot be a C function name")

    before = sum(len(block.statements) for function in built.values() for block in function.blocks.values())
    arguments = {}
    for entry, signature in found.items():
        registers = {value[1] for value in signature.params - signature.results if value[0] == "r"}
        returned = next(iter(signature.results)) if len(signature.results) == 1 else None
        if returned is not None and returned[0] == "r" and returned in signature.params:
            registers.add(returned[1])
        arguments[entry] = registers
    whole = set()
    live_after = {}
    live_at_start = {}
    for entry, function in built.items():
        after, start = cleanup.eliminate(flow, function, found[entry].results, signatures.CONTEXT, arguments)
        expressions.tidy(function, flow, after, whole)
        results_of_calls = call_results(function, found, flow)
        if expressions.propagate_constants(function, results_of_calls, signatures.CONTEXT):
            after, start = cleanup.eliminate(flow, function, found[entry].results, signatures.CONTEXT, arguments)
            expressions.tidy(function, flow, after, whole)
        if expressions.inline_registers(function, flow, after, whole):
            expressions.simplify_function(function)
        if expressions.switch_values(function, flow):
            after, start = cleanup.eliminate(flow, function, found[entry].results, signatures.CONTEXT, arguments)
            expressions.tidy(function, flow, after, whole)
        knownbits.simplify(function, found[entry], signatures.CONTEXT, results_of_calls)
        expressions.simplify_function(function)
        live_after[entry] = after
        live_at_start[entry] = start

    always = {entry: set(summary.always) for entry, summary in flow.summaries.items()}
    emitter = emit.SoundEmitter(built, found, names, emit.SoundMemory(), always)
    emitter.writes = {entry: set(summary.writes) for entry, summary in flow.summaries.items()}
    emitter.flow = flow
    emitter.code_labels = {address: symbol.name for address, symbol in program.symbols.items() if symbol.kind == "code"}
    for entry, function in built.items():
        structure.split_loop_entries(function, (live_after[entry], {}))
    writers = {entry: emit.SoundWriter(emitter, function, live_after[entry], {}, live_at_start[entry])
               for entry, function in built.items()}
    texts = {}
    problems = []
    for entry, writer in writers.items():
        try:
            texts[entry] = writer.write()
        except emit.emit.EmitError as problem:
            problems.append((entry, str(problem)))
    if problems:
        for entry, problem in problems[:20]:
            print(f"  {entry:04X}  {problem}")
        raise SystemExit(f"{len(problems)} functions could not be written")

    out_dir.mkdir(parents=True, exist_ok=True)
    for stale in out_dir.glob("*.c"):
        stale.unlink()
    entries = sorted(built)
    prototypes = [emitter.prototype(entry) + ";" for entry in entries]
    (out_dir / "sound.h").write_text(
        file_header("sound.h", "prototypes of the sound program's functions")
        + "#ifndef SONIC_SOUND_H\n#define SONIC_SOUND_H\n\n#include \"sound/support.h\"\n\n"
        + "\n".join(emitter.memory.declarations()) + "\n\n"
        + "\n".join(prototypes) + "\n\n#endif\n", encoding="ascii")
    for number, first in enumerate(range(0, len(entries), FUNCTIONS_PER_FILE)):
        chunk = [texts[entry] for entry in entries[first:first + FUNCTIONS_PER_FILE]]
        (out_dir / f"sound_{number:02}.c").write_text(
            file_header(f"sound_{number:02}.c", f"the sound program's functions, part {number:02} (in the order "
                        "of their Z80 addresses)")
            + '#include "sound.h"\n\n' + "\n".join(chunk), encoding="ascii")
    left = sum(len(block.statements) for function in built.values() for block in function.blocks.values())
    gotos = sum(text.count("goto ") for text in texts.values())
    print(f"functions:  {len(entries)} in {out_dir}")
    print(f"statements: {left} ({before - left} dead ones removed)")
    print(f"branches:   {flow.conditions_recovered} now compare values instead of flags")
    print(f"gotos:      {gotos}")
    return 0


def write_timing(program, path):
    """The blocks of the timing model as the C has them (runtime/src/sound_cpu.c), for the reference to
    keep the same time: every block's first instruction, its cycles, its last instruction and that one's
    length, and the extra cycles when its condition takes the longer way; the block moves; the loop's top
    and the interrupt handler."""
    builder = functions.Builder(program)
    lines = ["# the sound program's timing model (tools/decompile_sound.py): see runtime/src/sound_cpu.c",
             f"loop {builder.loop:04X}", f"irq {INTERRUPT_HANDLER:04X}"]
    for entry in sorted(program.entries):
        function = builder.build(entry)
        body = program.bodies[entry]
        for label, block in function.blocks.items():
            if not isinstance(label, int):
                continue
            last = body[block.addresses[-1]]
            taken, short = functions.timing.cycles(last)
            lines.append(f"block {label:04X} {builder.block_cycles(body, block)} {last.address:04X} {last.length} "
                         f"{taken - short}")
            for address in block.addresses:
                if body[address].mnemonic in ("ldir", "lddr"):
                    lines.append(f"repeat {address:04X}")
    Path(path).write_text("\n".join(lines) + "\n", encoding="ascii")


def main():
    arguments = sys.argv[1:]
    out_dir = ROOT / "src" / "sound"
    if "--out" in arguments:
        position = arguments.index("--out")
        out_dir = Path(arguments[position + 1])
        del arguments[position:position + 2]
    timing_path = ROOT / "build" / "sound" / "timing.txt"
    if "--timing" in arguments:
        position = arguments.index("--timing")
        timing_path = Path(arguments[position + 1])
        del arguments[position:position + 2]
    configure()
    program = load_program()
    if "--check" in arguments:
        return check(program)
    timing_path.parent.mkdir(parents=True, exist_ok=True)
    write_timing(program, timing_path)
    return write_c(program, out_dir)


if __name__ == "__main__":
    sys.exit(main())
