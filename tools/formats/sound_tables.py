"""
Name:        sound_tables.py
Description: the sound driver's data tables as text: the chips' init tables, the voices' channels, the
             FM patch registers, the pitch tables, the MIDI part templates, the instruments, the note codes,
             the velocity curves, the FM patches, the drum kits, the vibrato presets and the sample headers
Version:     0.1.0
Created:     14-10-2026
Modified:    20-10-2026
Author:      Sebastian Toyos

The tables the Z80 sound driver (src/sound/sound_00.c) reads below A000 of the sound program's chip, little
endian. Each format is a pair to_text(bytes) -> text and from_text(text) -> bytes with
from_text(to_text(data)) == data for any bytes: what a format cannot read as its records stays in the text
as 'bytes XX XX ...' lines, and runs of one byte as 'fill XX x COUNT', so a piece goes back whole. A line
starting with '#' is a comment, and so is whatever follows a '#' that has a space or the line's end before
and after it (a sharp, C#, is not one); comments are ignored on the way back.

The formats (names start with z80_: FORMATS is one dictionary for every module):
- z80_fm_register_pairs: register, value pairs for the YM3438s (fm_global_init, fm_operator_init), one a
  line: "22 = 0A", the comment saying what the register is.
- z80_pcm_register_pairs: the same for the RF5C68 (pcm_register_init).
- z80_voice_chip_channels: the chip channel of each of the 20 voices (init_track_channels): "pcm 3" an
  RF5C68 channel, "fm 2 5" YM3438 #2's channel 5.
- z80_pcm_channel_bits: by RF5C68 channel, its bit in pcm_channels_on and register 8.
- z80_fm_register_list: YM3438 registers, one a line (fm_patch_registers, fm_level_registers).
- z80_fm_fnum_tables, z80_pcm_pitch_tables: the 12 semitone pointers, then the runs they point into (16
  steps of 1/16 semitone a run): YM3438 F-numbers with a block offset, RF5C68 FD values.
- z80_pitch_table_pointers: pointers alone (the tables of the octave below that nothing reads).
- z80_part_templates: the MIDI part templates CC 20 picks (9 bytes a part).
- z80_instruments: by program, a pointer and the 5-byte header it points at.
- z80_note_codes: by MIDI note number (+ transpose), the note code as SEMITONE/OCTAVE.
- z80_velocity_levels: curves of 128 levels by note on velocity.
- z80_pitch_bend_steps: by the pitch bend's distance from the centre, the steps the pitch moves.
- z80_fm_patches: by patch number, the 25-byte YM3438 voice as an FM patch editor shows it.
- z80_drum_kits: by program - F0, 128 notes of 4 bytes.
- z80_vibrato_presets: by CC 11's value, the 7 bytes of an FM voice's vibrato.
- z80_pcm_sample_headers: by sample number, a pointer and the 6-byte header (bank and page of the sample;
  the comment names its WAV in assets/sound/samples), then the fill up to the next piece.
- z80_jump_table: a table the driver jumps through (JP (HL), playlist_command_table): by index, the Z80
  address it jumps to and that code's name in sound_symbols.txt (the code is the C's: the address goes back).

Where a table is pointers and records, each record is written under the pointers that point at it (all of
them, when several share it), in the order the records lie in the ROM: the pointer lines give the table's
words back (by their index), the record lines the bytes after it, in the order written. Pointers read as
Z80 addresses (0x1985); where one points at a place sound_symbols.txt names outside its own table, the name
follows it (only the address goes back).
"""
import re
import struct
from pathlib import Path

SYMBOLS = Path(__file__).resolve().parents[2] / "sound_symbols.txt"
WINDOW = 0xA000                     # the Z80 sees the chip's own offsets below here

# Where the pointer tables are (the driver reads them at these addresses: ld hl,$193B...)
FM_FNUM_TABLES = 0x137C
PART_TEMPLATES = 0x1594
INSTRUMENTS = 0x193B
PCM_PITCH_TABLES = 0x1E96
FM_PATCHES = 0x242E
DRUM_KITS = 0x3F2E
VIBRATO_PRESETS = 0x45E2
PCM_SAMPLE_HEADERS = 0x6429

SEMITONES = ("C", "C#", "D", "D#", "E", "F", "F#", "G", "G#", "A", "A#", "B")
SEMITONE_OF = {name: index for index, name in enumerate(SEMITONES)}
RUN_STEPS = 16                      # pitch table steps a semitone (1/16 semitone, 6.25 cents)
RUN_BYTES = RUN_STEPS * 2
SEMITONE_POINTERS = 12              # fm_fnum_tables / pcm_pitch_tables: by note code & 0F, codes have 0-11
FIRST_DRUM_PROGRAM = 0xF0           # play_midi_message: programs from F0 are drum kits (drum_kits[program - F0])
DRUM_NOTES = 128
OPERATOR_OF_SLOT = (1, 3, 2, 4)     # a YM3438 channel's registers +0, +4, +8, +C are operators 1, 3, 2, 4
SLOT_OF_OPERATOR = {operator: slot for slot, operator in enumerate(OPERATOR_OF_SLOT)}
PCM_PAGE = 0x100
PCM_BANK = 0x2000
EMPTY_DRUM_NOTE = bytes((0x00, 0x04, 0x00, 0xFF))   # step 0x0400, pan 0, no sample: the kits' empty notes
FILL_RUN = 16                       # a run of one byte this long or longer is written as a fill
COMMENT = re.compile(r"(^|\s)#(\s.*)?$")


# ---- names of sound_symbols.txt ----

_NAMES = {}


def _names():
    """[(address, name or None)]: the places sound_symbols.txt names below A000, sorted; None for code (a
    function or label), which ends the data piece before it."""
    if "places" in _NAMES:
        return _NAMES["places"]
    places = {}
    lines = SYMBOLS.read_text(encoding="utf-8").splitlines() if SYMBOLS.exists() else []
    for line in lines:
        match = re.match(r"([0-9A-F]{4})\s+(table|data|jumps|func|code|loop)\s+(\w+)", line)
        if match:
            address = int(match.group(1), 16)
            if address < WINDOW:
                places.setdefault(address, match.group(3) if match.group(2) in ("table", "data", "jumps") else None)
    _NAMES["places"] = sorted(places.items())
    return _NAMES["places"]


def _name_of(address):
    """The name of Z80 address 'address' (name+0xNN inside a named data piece), or None."""
    found = None
    for place, name in _names():
        if place > address:
            break
        found = (place, name)
    if found is None or found[1] is None:
        return None
    return found[1] if found[0] == address else f"{found[1]}+0x{address - found[0]:X}"


# ---- helpers ----

def _lines(text):
    """The lines that matter: stripped, without comments or blank lines."""
    for line in text.split("\n"):
        line = line.strip()
        if line.startswith("#"):
            continue
        line = COMMENT.sub("", line).strip()
        if line:
            yield line


def _finish(lines):
    return "\n".join(lines) + "\n"


def _signed(value):
    """A signed number with its sign: +3, -24, 0."""
    return f"{value:+d}" if value else "0"


def _s8(value):
    return value - 0x100 if value & 0x80 else value


def _u8(value, what):
    """A signed or unsigned number back to a byte."""
    if not -0x80 <= value <= 0xFF:
        raise ValueError(f"{what} {value} does not fit a byte")
    return value & 0xFF


def _hex_byte(token, what):
    value = int(token, 16)
    if not 0 <= value <= 0xFF:
        raise ValueError(f"{what} {token} does not fit a byte")
    return value


def _number(token, what, low, high):
    """A decimal number (0x... hex) in low..high."""
    value = int(token, 16) if token.lower().startswith("0x") else int(token)
    if not low <= value <= high:
        raise ValueError(f"{what} {token} is not in {low}..{high}")
    return value


def _tail_lines(data):
    """Bytes no record takes: runs of one byte (FILL_RUN or more) as 'fill XX x COUNT', the rest as
    'bytes XX XX ...' lines, sixteen a line."""
    lines = []
    loose = bytearray()

    def flush():
        lines.extend("bytes " + " ".join(f"{byte:02X}" for byte in loose[at:at + 16]) for at in range(0, len(loose), 16))
        loose.clear()

    at = 0
    while at < len(data):
        end = at
        while end < len(data) and data[end] == data[at]:
            end += 1
        if end - at >= FILL_RUN:
            flush()
            lines.append(f"fill {data[at]:02X} x {end - at}")
        else:
            loose.extend(data[at:end])
        at = end
    flush()
    return lines


def _tail_value(line):
    """The bytes of a 'bytes ...' or 'fill XX x COUNT' line, or None for another line."""
    tokens = line.split()
    if tokens[0] == "bytes":
        return bytes(_hex_byte(token, "byte") for token in tokens[1:])
    if tokens[0] == "fill":
        if len(tokens) != 4 or tokens[2] != "x":
            raise ValueError(f"not a fill: {line}")
        return bytes([_hex_byte(tokens[1], "fill byte")]) * _number(tokens[3], "fill count", 0, 0x40000)
    return None


def _note_name(number):
    """A MIDI note number's name, C4 = 60 (note 0 is C-1)."""
    return f"{SEMITONES[number % 12]}{number // 12 - 1}"


def _pan_text(value):
    """An RF5C68 pan byte: the low nibble the left level, the high one the right (0-15)."""
    return f"L{value & 0x0F} R{value >> 4}"


def _pan_value(left, right):
    if not (left[:1] == "L" and right[:1] == "R"):
        raise ValueError(f"not a pan: {left} {right}")
    return _number(left[1:], "left level", 0, 15) | _number(right[1:], "right level", 0, 15) << 4


def _keyed(tokens, keys, what):
    """{key: [values]} from 'KEY v... KEY v...' tokens; 'keys' gives how many values each key takes."""
    found = {}
    at = 0
    while at < len(tokens):
        key = tokens[at]
        if key not in keys:
            raise ValueError(f"{what}: unknown field '{key}'")
        if key in found:
            raise ValueError(f"{what}: '{key}' twice")
        count = keys[key]
        if at + 1 + count > len(tokens):
            raise ValueError(f"{what}: '{key}' without its value")
        found[key] = tokens[at + 1:at + 1 + count]
        at += 1 + count
    return found


# ---- pointer tables and their records ----

def _pointer_count(data, base):
    """How many 16-bit pointers start the piece at 'base': up to the lowest one that points past them,
    into the piece."""
    lowest = base + len(data)
    count = 0
    while base + 2 * count < lowest and 2 * count + 2 <= len(data):
        word = struct.unpack_from("<H", data, 2 * count)[0]
        count += 1
        if base + 2 * count <= word < lowest:
            lowest = word
    if base + 2 * count > lowest:
        count -= 1
    return count


def _pointed_text(data, base, count, pointer_line, records, head_records=None):
    """The lines of a table of 'count' pointers at 'base' and what follows it: the bytes after the table
    are cut where pointers point, and each part goes under the pointer lines of the indices that point
    at its start (pointer_line(index, value)); records(part, address) gives a part's lines. What comes
    before the first pointed place goes through head_records (or as bytes). Pointers that point
    elsewhere come first."""
    words = struct.unpack_from(f"<{count}H", data, 0) if count else ()
    body = base + 2 * count
    end = base + len(data)
    starts = sorted({word for word in words if body <= word < end})
    by_start = {}
    for index, word in enumerate(words):
        by_start.setdefault(word, []).append(index)
    lines = [pointer_line(index, word) for index, word in enumerate(words) if word not in starts]
    if lines:
        lines.append("")
    cuts = [body] + starts + [end]
    for at, finish in zip(cuts, cuts[1:]):
        if at == finish:
            continue
        part = data[at - base:finish - base]
        if at in starts:
            lines.extend(pointer_line(index, at) for index in by_start[at])
            lines.extend(records(part, at))
        else:
            lines.extend(head_records(part, at) if head_records else _tail_lines(part))
        lines.append("")
    return lines[:-1] if lines and lines[-1] == "" else lines


def _pointed_bytes(text, keyword, index_of, record_bytes):
    """A table back from _pointed_text's lines: 'keyword INDEX 0xADDRESS' lines give the pointers, the
    rest the bytes after the table in the order written (record_bytes(line, state) gives a record's
    bytes, b'' while a record spans more lines; None: not a record)."""
    pointers = {}
    body = bytearray()
    state = {}
    for line in _lines(text):
        tokens = line.split()
        if tokens[0] == keyword:
            if len(tokens) < 3 or not tokens[2].lower().startswith("0x"):
                raise ValueError(f"not a pointer: {line}")
            index = index_of(tokens[1])
            if index in pointers:
                raise ValueError(f"{keyword} {tokens[1]} given twice")
            pointers[index] = int(tokens[2], 16)
            continue
        tail = _tail_value(line)
        if tail is not None:
            body += tail
            continue
        record = record_bytes(line, state)
        if record is None:
            raise ValueError(f"not a line of this table: {line}")
        body += record
    if state.get("open"):
        raise ValueError(f"an unfinished record at the end: {state['open']}")
    missing = [index for index in range(len(pointers)) if index not in pointers]
    if missing:
        raise ValueError(f"{keyword} {missing[0]} missing")
    return b"".join(struct.pack("<H", pointers[index]) for index in range(len(pointers))) + bytes(body)


def _fixed_records(size, to_line, limit=None):
    """records() for _pointed_text: records of 'size' bytes, at most 'limit' of them, to_line(record,
    number) each; what is left as bytes."""
    def records(part, address):
        count = len(part) // size if limit is None else min(len(part) // size, limit)
        lines = [to_line(part[at * size:at * size + size], at) for at in range(count)]
        return lines + _tail_lines(part[count * size:])
    return records


# ---- register, value pairs ----

FM_GLOBAL_REGISTERS = {0x21: "test", 0x22: "LFO", 0x24: "timer A, high 8 bits", 0x25: "timer A, low 2 bits",
                       0x26: "timer B", 0x27: "timer control, channel 3 mode", 0x28: "key on/off",
                       0x2A: "DAC data", 0x2B: "DAC enable", 0x2C: "test 2"}
FM_OPERATOR_REGISTERS = {0x30: "DT/MUL", 0x40: "TL", 0x50: "KS/AR", 0x60: "AM/D1R", 0x70: "D2R",
                         0x80: "SL/RR", 0x90: "SSG-EG"}
FM_CHANNEL_REGISTERS = {0xA0: "F-number low", 0xA4: "block, F-number high", 0xA8: "channel 3 operator F-number low",
                        0xAC: "channel 3 operator block, F-number high", 0xB0: "FB/ALG", 0xB4: "L/R/AMS/PMS"}


def _fm_register_name(register):
    """What YM3438 register 'register' is (part I's numbering: channels 1-3; part II's are 4-6)."""
    if register in FM_GLOBAL_REGISTERS:
        return FM_GLOBAL_REGISTERS[register]
    channel = register & 3
    if 0x30 <= register <= 0x9F:
        name = FM_OPERATOR_REGISTERS[register & 0xF0]
        if channel == 3:
            return f"{name}, no channel"
        return f"{name}, channel {channel + 1} operator {OPERATOR_OF_SLOT[(register >> 2) & 3]}"
    if 0xA0 <= register <= 0xB6:
        name = FM_CHANNEL_REGISTERS[register & 0xFC]
        return f"{name}, no channel" if channel == 3 else f"{name}, channel {channel + 1}"
    return "not a register"


def _fm_value_meaning(register, value):
    """What 'value' sets in YM3438 register 'register', for a comment."""
    if register == 0x22:
        return f"on, rate {value & 7}" if value & 8 else "off"
    if register == 0x28:
        channel = value & 7
        if channel & 3 == 3:
            return "no channel"
        operators = [str(operator) for operator in (1, 2, 3, 4) if value >> (3 + operator) & 1]
        return f"channel {(channel & 3) + 1 + (3 if channel & 4 else 0)}, " + \
            ("operators " + " ".join(operators) + " on" if operators else "all operators off")
    if register == 0x2B:
        return "on" if value & 0x80 else "off"
    if 0x40 <= register <= 0x4F:
        return f"TL {value & 0x7F}" + (" (silent)" if value & 0x7F == 0x7F else "")
    if 0x80 <= register <= 0x8F:
        return f"SL {value >> 4}, RR {value & 15}"
    if 0x90 <= register <= 0x9F:
        return "off" if value == 0 else f"mode {value & 7}" + ("" if value & 8 else ", disabled")
    return ""


PCM_REGISTERS = {0: "ENV (volume)", 1: "PAN", 2: "FDL (pitch step, low)", 3: "FDH (pitch step, high)",
                 4: "LSL (loop address, low)", 5: "LSH (loop address, high)", 6: "ST (start address, high)",
                 7: "control", 8: "channels on/off"}


def _pcm_value_meaning(register, value):
    """What 'value' sets in RF5C68 register 'register', for a comment."""
    register &= 0x0F
    if register == 7:
        target = f"registers 0-6 address channel {value & 7}" if value & 0x40 else f"wave bank {value & 15}"
        return ("sound on, " if value & 0x80 else "sound off, ") + target
    if register == 8:
        on = [str(channel) for channel in range(8) if not value >> channel & 1]
        return ("channels " + " ".join(on) + " on" if on else "all off") + " (a set bit is off)"
    if register == 6:
        return f"start at {value << 8:04X}"
    if register == 5:
        return f"loop at {value:02X}xx"
    if register == 4:
        return f"loop at xx{value:02X}"
    if register == 1:
        return _pan_text(value)
    if register == 0:
        return f"level {value}"
    return ""


def _register_pairs_to_text(data, title, name_of, meaning_of):
    lines = title + ["# REGISTER = VALUE, hex; the comment says what the register is and what the value sets"]
    for at in range(0, len(data) - 1, 2):
        register, value = data[at], data[at + 1]
        what = name_of(register)
        meaning = meaning_of(register, value)
        lines.append(f"{register:02X} = {value:02X}    # {what}" + (f": {meaning}" if meaning else ""))
    lines += _tail_lines(data[len(data) // 2 * 2:])
    return _finish(lines)


def _register_pairs_from_text(text):
    out = bytearray()
    for line in _lines(text):
        tail = _tail_value(line)
        if tail is not None:
            out += tail
            continue
        tokens = line.split()
        if len(tokens) != 3 or tokens[1] != "=":
            raise ValueError(f"not a register pair: {line}")
        out += bytes((_hex_byte(tokens[0], "register"), _hex_byte(tokens[2], "value")))
    return bytes(out)


def fm_register_pairs_to_text(data):
    """fm_global_init, fm_operator_init (init_fm_chips): pairs of register, value."""
    return _register_pairs_to_text(
        data, ["# YM3438 register, value pairs init_fm_chips writes in order: fm_global_init to part I of both chips,",
               "# fm_operator_init to all four parts (channels 1-3 are a part's own: part II's are channels 4-6)"],
        _fm_register_name, _fm_value_meaning)


def pcm_register_pairs_to_text(data):
    """pcm_register_init (init_pcm_chip): pairs of RF5C68 register, value."""
    return _register_pairs_to_text(
        data, ["# RF5C68 register, value pairs init_pcm_chip writes in order (registers 0-6 are those of the channel",
               "# register 7 selects; channels 0-7, 8 KB of wave memory each)"],
        lambda register: PCM_REGISTERS.get(register & 0x0F, "not a register"), _pcm_value_meaning)


# ---- channels and register lists ----

def _chip_channel_text(code):
    """A voice's chip_channel: 'pcm N' (RF5C68 channel 0-7), 'fm CHIP CHANNEL' (YM3438 #1-2, channel 1-6:
    80 + 8 for the second chip + 4 for part II + the channel in the part), or 'code 0xNN'."""
    if code < 8:
        return f"pcm {code}"
    if code & 0xF0 == 0x80 and code & 3 != 3:
        return f"fm {(code >> 3 & 1) + 1} {(code & 3) + 1 + (3 if code & 4 else 0)}"
    return f"code 0x{code:02X}"


def _chip_channel_value(tokens, line):
    if tokens[0] == "pcm" and len(tokens) == 2:
        return _number(tokens[1], "RF5C68 channel", 0, 7)
    if tokens[0] == "fm" and len(tokens) == 3:
        chip = _number(tokens[1], "YM3438", 1, 2)
        channel = _number(tokens[2], "YM3438 channel", 1, 6) - 1
        return 0x80 | (chip - 1) << 3 | (4 if channel >= 3 else 0) | channel % 3
    if tokens[0] == "code" and len(tokens) == 2:
        return _hex_byte(tokens[1][2:] if tokens[1].lower().startswith("0x") else tokens[1], "code")
    raise ValueError(f"not a chip channel: {line}")


def voice_chip_channels_to_text(data):
    """voice_chip_channels: init_track_channels copies them in order into the 20 voices' chip_channel."""
    lines = ["# The chip channel of each voice: init_track_channels copies these, in order, into the chip_channel",
             "# (+01) of the 20 voices, 12h bytes apart: E400-E48F the 8 sample voices, E490-E567 the 12 FM voices.",
             "# pcm N: RF5C68 channel N (0-7); fm CHIP CHANNEL: YM3438 #CHIP (1-2), its channel 1-6 (4-6 on part II)"]
    for index, code in enumerate(data):
        lines.append(f"voice {0xE400 + index * 0x12:04X}: {_chip_channel_text(code)}")
    return _finish(lines)


def voice_chip_channels_from_text(text):
    out = bytearray()
    for line in _lines(text):
        head, colon, rest = line.partition(":")
        if not colon or not head.startswith("voice"):
            raise ValueError(f"not a voice line: {line}")
        out.append(_chip_channel_value(rest.split(), line))
    return bytes(out)


def pcm_channel_bits_to_text(data):
    """pcm_channel_bits: by RF5C68 channel, the bit that is its in pcm_channels_on."""
    lines = ["# By RF5C68 channel (0-7), its bit in pcm_channels_on and in register 8 (where a set bit is off):",
             "# 'bit N', or 'mask 0xNN' for a value that is not one bit"]
    for channel, value in enumerate(data):
        single = value and not value & (value - 1)
        lines.append(f"channel {channel}: " + (f"bit {value.bit_length() - 1}" if single else f"mask 0x{value:02X}"))
    return _finish(lines)


def pcm_channel_bits_from_text(text):
    out = bytearray()
    for line in _lines(text):
        _, colon, rest = line.partition(":")
        tokens = rest.split()
        if not colon or len(tokens) != 2:
            raise ValueError(f"not a channel line: {line}")
        if tokens[0] == "bit":
            out.append(1 << _number(tokens[1], "bit", 0, 7))
        elif tokens[0] == "mask":
            out.append(_number(tokens[1], "mask", 0, 0xFF))
        else:
            raise ValueError(f"not a channel line: {line}")
    return bytes(out)


def fm_register_list_to_text(data):
    """fm_patch_registers, fm_level_registers: YM3438 registers (of channel 1: the driver adds the channel
    within the part)."""
    lines = ["# YM3438 registers, one a line, in the order the driver writes a patch's bytes to them (it adds the",
             "# channel within the part: these are channel 1's)"]
    lines += [f"{register:02X}    # {_fm_register_name(register)}" for register in data]
    return _finish(lines)


def fm_register_list_from_text(text):
    out = bytearray()
    for line in _lines(text):
        tokens = line.split()
        if len(tokens) != 1:
            raise ValueError(f"not a register: {line}")
        out.append(_hex_byte(tokens[0], "register"))
    return bytes(out)


# ---- the pitch tables ----

def _run_label(address, c_run):
    """SEMITONE/OCTAVE of the run at 'address', from where semitone C's run starts (None: unknown)."""
    if c_run is None or (address - c_run) % RUN_BYTES:
        return "run"
    steps = (address - c_run) // RUN_BYTES
    return f"{SEMITONES[steps % 12]}/{steps // 12:+d}"


def _semitone_index(token):
    if token in SEMITONE_OF:
        return SEMITONE_OF[token]
    return _number(token, "semitone", 0, 15)


def _semitone_pointer_line():
    def line(index, value):
        name = SEMITONES[index] if index < 12 else str(index)
        return f"semitone {name:<2}  0x{value:04X}"
    return line


def _c_run(data, base):
    """Where semitone C's run starts: the table's first pointer, if it points into the table."""
    if len(data) < 2:
        return None
    first = struct.unpack_from("<H", data, 0)[0]
    return first if base + 2 * SEMITONE_POINTERS <= first < base + len(data) else None


def _fnum_run_line(run, address, c_run):
    """A run of YM3438 F-numbers: the block offset (bits 11-15 of a value, signed, added to the note's
    block) most of the run has, then the F-numbers (11 bits) in hex; a value with another offset is
    written whole, 0xNNNN."""
    words = struct.unpack(f"<{len(run) // 2}H", run)
    offsets = [((word >> 11) ^ 0x10) - 0x10 for word in words]
    block = max(set(offsets), key=offsets.count)
    values = [f"{word & 0x7FF:03X}" if offset == block else f"0x{word:04X}" for word, offset in zip(words, offsets)]
    return f"run {_run_label(address, c_run):<5} block {_signed(block):>2}: " + " ".join(values)


def _fnum_run_bytes(line):
    head, colon, rest = line.partition(":")
    tokens = head.split()
    if not colon or tokens[:1] != ["run"]:
        return None
    if "block" not in tokens[:-1]:
        raise ValueError(f"{line}: no block offset")
    block = _number(tokens[tokens.index("block") + 1], "block offset", -16, 15)
    out = bytearray()
    for token in rest.split():
        if token.lower().startswith("0x"):
            word = _number(token, "value", 0, 0xFFFF)
        else:
            word = (block & 0x1F) << 11 | _number("0x" + token, "F-number", 0, 0x7FF)
        out += struct.pack("<H", word)
    return bytes(out)


def _fd_run_line(run, address, c_run):
    """A run of RF5C68 FD values (hex: 0x0800 plays a sample at its own rate)."""
    words = struct.unpack(f"<{len(run) // 2}H", run)
    return f"run {_run_label(address, c_run):<5}: " + " ".join(f"{word:04X}" for word in words)


def _fd_run_bytes(line):
    head, colon, rest = line.partition(":")
    if not colon or head.split()[:1] != ["run"]:
        return None
    return b"".join(struct.pack("<H", _number("0x" + token, "FD", 0, 0xFFFF)) for token in rest.split())


def _runs(line_of, c_run_of):
    def records(part, address):
        c_run = c_run_of()
        count = len(part) // RUN_BYTES
        lines = [line_of(part[at * RUN_BYTES:(at + 1) * RUN_BYTES], address + at * RUN_BYTES, c_run) for at in range(count)]
        return lines + _tail_lines(part[count * RUN_BYTES:])
    return records


def _pitch_table_to_text(data, base, header, line_of):
    if len(data) < 2 * SEMITONE_POINTERS:
        return _finish(header + _tail_lines(data))
    c_run = _c_run(data, base)
    records = _runs(line_of, lambda: c_run)
    return _finish(header + _pointed_text(data, base, SEMITONE_POINTERS, _semitone_pointer_line(), records, records))


def fm_fnum_tables_to_text(data):
    """fm_fnum_tables: by semitone, where its run of 16 YM3438 F-numbers starts, and the runs."""
    header = ["# fm_fnum_tables: by semitone (the note code's low nibble), where its run of YM3438 F-numbers starts;",
              "# then the runs, each 16 steps of 1/16 semitone (6.25 cents) up from its semitone. The note on reads",
              "# the run of its semitone at the pitch bend (+-31 steps) plus the part's fm_detune (steps), so it",
              "# reaches into the runs before and after (labelled SEMITONE/OCTAVE from the C run). A value is the",
              "# F-number (11 bits, hex) and, in bits 11-15, a block offset (signed) added to the note's block:",
              "# 'block -1' runs are a block below the note's. A value with another offset is written whole (0xNNNN)."]
    return _pitch_table_to_text(data, FM_FNUM_TABLES, header, _fnum_run_line)


def fm_fnum_tables_from_text(text):
    return _pointed_bytes(text, "semitone", _semitone_index, lambda line, state: _fnum_run_bytes(line))


def pcm_pitch_tables_to_text(data):
    """pcm_pitch_tables: by semitone, where its run of 16 RF5C68 FD values starts, and the runs."""
    header = ["# pcm_pitch_tables: by semitone (the note code's low nibble), where its run of RF5C68 FD values",
              "# starts; then the runs, each 16 steps of 1/16 semitone (6.25 cents) up from its semitone. The FD is",
              "# how far a sample moves per output sample, in 1/2048ths (0800: its own rate). The note on reads the",
              "# run of its semitone at the pitch bend (+-31 steps) plus the instrument's detune (steps), so it",
              "# reaches into the runs of the octaves before and after (labelled SEMITONE/OCTAVE from the C run),",
              "# then shifts the value by the note code's octave."]
    return _pitch_table_to_text(data, PCM_PITCH_TABLES, header, _fd_run_line)


def pcm_pitch_tables_from_text(text):
    return _pointed_bytes(text, "semitone", _semitone_index, lambda line, state: _fd_run_bytes(line))


def pitch_table_pointers_to_text(data):
    """Pointers alone, one a line, labelled by semitone of the octave below (index 0 = C/-1)."""
    lines = ["# 12 pointers by semitone of the octave below, in the shape of fm_fnum_tables' and pcm_pitch_tables'",
             "# pointers (which follow them): where a semitone's run starts. Nothing reads them."]
    for index in range(len(data) // 2):
        word = struct.unpack_from("<H", data, 2 * index)[0]
        name = _name_of(word)
        label = f"{SEMITONES[index % 12]}/{index // 12 - 1:+d}"
        lines.append(f"{label:<5}  0x{word:04X}" + (f"  {name}" if name else ""))
    lines += _tail_lines(data[len(data) // 2 * 2:])
    return _finish(lines)


def _code_names():
    """{address: name} of the code sound_symbols.txt names below A000 (its functions and labels)."""
    if "code" not in _NAMES:
        found = {}
        lines = SYMBOLS.read_text(encoding="utf-8").splitlines() if SYMBOLS.exists() else []
        for line in lines:
            match = re.match(r"([0-9A-F]{4})\s+(func|code|loop)\s+(\w+)", line)
            if match and int(match.group(1), 16) < WINDOW:
                found.setdefault(int(match.group(1), 16), match.group(3))
        _NAMES["code"] = found
    return _NAMES["code"]


def jump_table_to_text(data):
    """A jump table, one entry a line: its index, the address the driver jumps to, the code's name there."""
    lines = ["# by index, where the driver jumps (JP (HL)): the code there is the C's, only the address goes back"]
    for index in range(len(data) // 2):
        word = struct.unpack_from("<H", data, 2 * index)[0]
        name = _code_names().get(word)
        lines.append(f"{index:02X}  0x{word:04X}" + (f"  {name}" if name else ""))
    lines += _tail_lines(data[len(data) // 2 * 2:])
    return _finish(lines)


def jump_table_from_text(text):
    return pitch_table_pointers_from_text(text)


def pitch_table_pointers_from_text(text):
    out = bytearray()
    for line in _lines(text):
        tail = _tail_value(line)
        if tail is not None:
            out += tail
            continue
        tokens = line.split()
        if len(tokens) < 2 or not tokens[1].lower().startswith("0x"):
            raise ValueError(f"not a pointer: {line}")
        out += struct.pack("<H", _number(tokens[1], "pointer", 0, 0xFFFF))
    return bytes(out)


# ---- note codes, velocity, pitch bend ----

def _note_code_text(code):
    """SEMITONE/OCTAVE: the low nibble's semitone, the high nibble as a signed octave; 0xNN for a low
    nibble past B."""
    if code & 15 >= 12:
        return f"0x{code:02X}"
    octave = ((code >> 4) ^ 8) - 8
    return f"{SEMITONES[code & 15]}/{octave:+d}"


def _note_code_value(token):
    if token.lower().startswith("0x"):
        return _number(token, "note code", 0, 0xFF)
    semitone, slash, octave = token.partition("/")
    if not slash or semitone not in SEMITONE_OF:
        raise ValueError(f"not a note code: {token}")
    return (_number(octave, "octave", -8, 7) & 15) << 4 | SEMITONE_OF[semitone]


def note_codes_to_text(data):
    """note_codes: by MIDI note number + transpose, the note code; 12 a line."""
    lines = ["# note_codes: by MIDI note number + the part's transpose (one byte: it wraps), the note code the",
             "# driver plays, SEMITONE/OCTAVE: the semitone (low nibble) and the octave (high nibble, -8..+7).",
             "# RF5C68 notes: the semitone's FD from pcm_pitch_tables, shifted up by the octave (down when it is",
             "# negative: /+0 is the table's rate, /+1 an octave up). YM3438 notes: the semitone's F-number from",
             "# fm_fnum_tables, the octave's low 3 bits the block (/-8 block 0 ... /-1 block 7, /+0 block 0 again).",
             "# A line: the first index (and its MIDI note, C4 = 60), then 12 codes (the label is not read back).",
             "# An index past the codes reads the bytes that follow them."]
    for at in range(0, len(data), 12):
        label = f"{at:3d} {_note_name(at) if at < 128 else '':>4}"
        lines.append(f"{label}: " + " ".join(f"{_note_code_text(code):>6}" for code in data[at:at + 12]))
    return _finish(lines)


def note_codes_from_text(text):
    out = bytearray()
    for line in _lines(text):
        _, colon, rest = line.partition(":")
        if not colon:
            raise ValueError(f"not a line of note codes: {line}")
        out += bytes(_note_code_value(token) for token in rest.split())
    return bytes(out)


def _numbers_lines(data, label, per_line=16):
    return [f"{label(at)}: " + " ".join(f"{value:3d}" for value in data[at:at + per_line])
            for at in range(0, len(data), per_line)]


def _numbers_from_text(text, what, high=0xFF):
    out = bytearray()
    for line in _lines(text):
        _, colon, rest = line.partition(":")
        if not colon:
            raise ValueError(f"not a line of {what}: {line}")
        out += bytes(_number(token, what, 0, high) for token in rest.split())
    return bytes(out)


def velocity_levels_to_text(data):
    """A curve of 128 levels (0-255) by note on velocity, 16 a line."""
    lines = ["# Levels by note on velocity (0-127), 0-255, 16 a line (the velocity before the colon is not read",
             "# back). velocity_levels is the curve the driver reads (play_midi_message, 04B4): a sample note's",
             "# RF5C68 envelope (times the part's effective volume); an FM note's carrier attenuation takes its",
             "# top 5 bits. The unused curves next to it have its shape."]
    lines += _numbers_lines(data, lambda at: f"velocity {at:3d}")
    return _finish(lines)


def velocity_levels_from_text(text):
    return _numbers_from_text(text, "level")


def pitch_bend_steps_to_text(data):
    """pitch_bend_steps: by the bend's distance from the centre, the steps of the pitch tables."""
    lines = ["# pitch_bend_steps: by the pitch bend's distance from the centre (the 14-bit value >> 5, 0-255; below",
             "# the centre the distance is negated), how many steps of the pitch tables (1/16 semitone) the note",
             "# moves, up or down with the bend. 16 a line, the first distance before the colon (not read back)."]
    lines += _numbers_lines(data, lambda at: f"0x{at:02X}")
    return _finish(lines)


def pitch_bend_steps_from_text(text):
    return _numbers_from_text(text, "steps")


# ---- the MIDI part templates ----

PART_FIELDS = {"midi_channel": 1, "volume": 1, "program": 1, "drums": 1, "transpose": 1, "fm_detune": 1,
               "spare": 2, "effective_volume": 1, "on": 0, "off": 0, "fixed_volume": 0, "flag_bits": 1}


def _program_text(program):
    """A program: 'program N' (an instrument, decimal) or 'drums N' (drum kit N: program F0 + N)."""
    return f"drums {program - FIRST_DRUM_PROGRAM}" if program >= FIRST_DRUM_PROGRAM else f"program {program}"


def _program_value(fields, what):
    if "drums" in fields:
        return FIRST_DRUM_PROGRAM + _number(fields["drums"][0], "drum kit", 0, 0xFF - FIRST_DRUM_PROGRAM)
    if "program" in fields:
        return _number(fields["program"][0], "program", 0, 0xFF)
    raise ValueError(f"{what}: no program")


def _part_line(record, number):
    flags, channel, volume, program, transpose, detune, spare_6, spare_7, effective = record
    words = (["on"] if flags & 0x80 else []) + (["fixed_volume"] if flags & 1 else [])
    if flags & 0x7E:
        words.append(f"flag_bits 0x{flags & 0x7E:02X}")
    return (f"part {' '.join(words) or 'off':<15}  midi_channel {channel:X}  volume {volume:3d}  {_program_text(program):<11}  "
            f"transpose {_signed(_s8(transpose)):>3}  fm_detune {_signed(_s8(detune)):>3}  spare {spare_6:02X} {spare_7:02X}"
            f"  effective_volume {effective}")


def _part_bytes(line, state):
    tokens = line.split()
    if tokens[0] != "part":
        return None
    fields = _keyed(tokens[1:], PART_FIELDS, line)
    flags = (0x80 if "on" in fields else 0) | (1 if "fixed_volume" in fields else 0)
    if "flag_bits" in fields:
        flags |= _number(fields["flag_bits"][0], "flag bits", 0, 0x7E) & 0x7E
    for key in ("midi_channel", "volume", "transpose", "fm_detune", "spare", "effective_volume"):
        if key not in fields:
            raise ValueError(f"{line}: no {key}")
    return bytes((flags, _number("0x" + fields["midi_channel"][0], "midi_channel", 0, 0xFF),
                  _number(fields["volume"][0], "volume", 0, 255), _program_value(fields, line),
                  _u8(_number(fields["transpose"][0], "transpose", -128, 127), "transpose"),
                  _u8(_number(fields["fm_detune"][0], "fm_detune", -128, 127), "fm_detune"),
                  _hex_byte(fields["spare"][0], "spare"), _hex_byte(fields["spare"][1], "spare"),
                  _number(fields["effective_volume"][0], "effective_volume", 0, 255)))


def part_templates_to_text(data):
    """part_templates: by CC 20's value, the parts copied to E300."""
    header = ["# part_templates: by CC 20's value, where the MIDI parts it copies to E300 start (8 parts, 48h bytes,",
              "# whatever follows: template 0 holds 5 parts, then template 1 begins); init_midi_parts copies",
              "# template 1 to E300 and template 0's 5 parts (2Dh bytes) to E348, the parts of channels A-E.",
              "# A part (9 bytes, field names of struct midi_part): on (bit 7: it plays), fixed_volume (bit 0: its",
              "# volume ignores music_volume); midi_channel, the channel it listens on (hex); volume 0-255 (CC 7's",
              "# value x 2 + 1); program N (instruments[N]) or drums N (drum kit N, program F0 + N); transpose",
              "# (semitones added to the note number); fm_detune (1/16 semitone steps, FM notes only); spare, bytes",
              "# 06-07 (nothing reads them: pad_06); effective_volume (the driver sets it from volume and",
              "# music_volume)."]
    count = _pointer_count(data, PART_TEMPLATES)
    return _finish(header + _pointed_text(data, PART_TEMPLATES, count,
                                          lambda index, value: f"template {index}  0x{value:04X}",
                                          _fixed_records(9, _part_line, 8)))


def part_templates_from_text(text):
    return _pointed_bytes(text, "template", lambda token: _number(token, "template", 0, 0xFFFF), _part_bytes)


# ---- instruments ----

INSTRUMENT_FIELDS = {"fm": 0, "sample": 1, "patch": 1, "flags": 1, "spare": 1, "pan": 2, "detune": 1}


def _instrument_line(record, number):
    """'fm patch NN' (first byte 02), 'sample NN' (01), or 'flags 0xNN' and the one its bit 1 picks."""
    flags, sound, spare, pan, detune = record
    kind = {1: "", 2: "fm "}.get(flags, f"flags 0x{flags:02X} ")
    what = kind + (f"patch {sound:02X}" if flags & 2 else f"sample {sound:02X}")
    return f"{what:<12}  spare {spare:02X}  pan {_pan_text(pan):<7}  detune {_signed(_s8(detune)):>3}"


def _instrument_bytes(line, state):
    tokens = line.split()
    if tokens[0] not in ("fm", "sample", "flags"):
        return None
    fields = _keyed(tokens, INSTRUMENT_FIELDS, line)
    if "flags" in fields:
        flags = _number(fields["flags"][0], "flags", 0, 0xFF)
    else:
        flags = 2 if "fm" in fields else 1
    for key in ("patch" if flags & 2 else "sample", "spare", "pan", "detune"):
        if key not in fields:
            raise ValueError(f"{line}: no {key}")
    sound = fields["patch" if flags & 2 else "sample"][0]
    return bytes((flags, _hex_byte(sound, "patch or sample"), _hex_byte(fields["spare"][0], "spare"),
                  _pan_value(*fields["pan"]), _u8(_number(fields["detune"][0], "detune", -128, 127), "detune")))


def instruments_to_text(data):
    """instruments: by program (below F0), a pointer to a 5-byte header."""
    count = _pointer_count(data, INSTRUMENTS)
    header = ["# instruments: by MIDI program (below F0; from F0 they are drum kits), a pointer to the instrument's",
              f"# 5 bytes ({count} programs here). fm: patch NN, the fm_patches number (hex); sample NN:",
              "# the pcm_sample_headers number (hex; the WAV assets/sound/samples/sample_NN.wav); flags 0xNN for",
              "# another first byte (its bit 1: FM). spare: byte 2, nothing reads it. pan (RF5C68: left and right",
              "# levels 0-15) and detune (1/16 semitone steps added to the pitch bend) are read for sample",
              "# instruments only."]
    return _finish(header + _pointed_text(data, INSTRUMENTS, count,
                                          lambda index, value: f"program {index}  0x{value:04X}",
                                          _fixed_records(5, _instrument_line, 1)))


def instruments_from_text(text):
    return _pointed_bytes(text, "program", lambda token: _number(token, "program", 0, 0xFFFF), _instrument_bytes)


# ---- FM patches ----

# The 25 bytes of a patch: the registers of fm_patch_registers (DT/MUL, KS/AR, AM/D1R, D2R, SL/RR, each for
# the slots +0 +4 +8 +C: operators 1 3 2 4; then FB/ALG), then the total levels of fm_level_registers.
PATCH_GROUPS = (0x30, 0x50, 0x60, 0x70, 0x80)
PATCH_SIZE = 25
OPERATOR_FIELDS = (    # field, register group, shift, bits
    ("DT", 0x30, 4, 3), ("MUL", 0x30, 0, 4), ("KS", 0x50, 6, 2), ("AR", 0x50, 0, 5), ("AM", 0x60, 7, 1),
    ("D1R", 0x60, 0, 5), ("D2R", 0x70, 0, 5), ("SL", 0x80, 4, 4), ("RR", 0x80, 0, 4))
GROUP_MASKS = {0x30: 0x7F, 0x50: 0xDF, 0x60: 0x9F, 0x70: 0x1F, 0x80: 0xFF}


def _operator_line(patch, operator):
    slot = SLOT_OF_OPERATOR[operator]
    registers = {group: patch[index * 4 + slot] for index, group in enumerate(PATCH_GROUPS)}
    level = patch[21 + slot]
    field = {name: registers[group] >> shift & ((1 << bits) - 1) for name, group, shift, bits in OPERATOR_FIELDS}
    text = (f"  op{operator}  DT {field['DT']}  MUL {field['MUL']:2d}  TL {level & 0x7F:3d} "
            f"{'fixed ' if level & 0x80 else 'scaled'}  KS {field['KS']}  AR {field['AR']:2d}  AM {field['AM']}  "
            f"D1R {field['D1R']:2d}  D2R {field['D2R']:2d}  SL {field['SL']:2d}  RR {field['RR']:2d}")
    for group, value in registers.items():
        if value & ~GROUP_MASKS[group]:
            text += f"  spare_{group:02X} 0x{value & ~GROUP_MASKS[group] & 0xFF:02X}"
    return text


def _patch_lines(patch, number):
    lines = [f"  algorithm {patch[20] & 7}  feedback {patch[20] >> 3 & 7}"
             + (f"  spare_B0 0x{patch[20] & 0xC0:02X}" if patch[20] & 0xC0 else "")]
    return lines + [_operator_line(patch, operator) for operator in (1, 2, 3, 4)]


def _patch_records(part, address):
    """A pointed patch's lines (one patch: what follows it, up to the next pointed place, as bytes)."""
    if len(part) < PATCH_SIZE:
        return _tail_lines(part)
    return _patch_lines(part[:PATCH_SIZE], 0) + _tail_lines(part[PATCH_SIZE:])


OPERATOR_KEYS = {"DT": 1, "MUL": 1, "TL": 2, "KS": 1, "AR": 1, "AM": 1, "D1R": 1, "D2R": 1, "SL": 1, "RR": 1,
                 "spare_30": 1, "spare_50": 1, "spare_60": 1, "spare_70": 1}


def _patch_bytes(line, state):
    """A patch's lines: 'algorithm' opens it, its four operator lines close it (b'' until then)."""
    tokens = line.split()
    if tokens[0] == "algorithm":
        if state.get("open"):
            raise ValueError(f"a patch without its four operators before {line}")
        fields = _keyed(tokens, {"algorithm": 1, "feedback": 1, "spare_B0": 1}, line)
        if "feedback" not in fields:
            raise ValueError(f"{line}: no feedback")
        spare = _number(fields["spare_B0"][0], "spare_B0", 0, 0xFF) & 0xC0 if "spare_B0" in fields else 0
        state["open"] = line
        state["patch"] = bytearray(PATCH_SIZE)
        state["patch"][20] = spare | _number(fields["feedback"][0], "feedback", 0, 7) << 3 | \
            _number(fields["algorithm"][0], "algorithm", 0, 7)
        state["operators"] = set()
        return b""
    if re.fullmatch(r"op[1-4]", tokens[0]):
        if not state.get("open"):
            raise ValueError(f"an operator outside a patch: {line}")
        operator = int(tokens[0][2])
        if operator in state["operators"]:
            raise ValueError(f"operator {operator} twice: {line}")
        fields = _keyed(tokens[1:], OPERATOR_KEYS, line)
        slot = SLOT_OF_OPERATOR[operator]
        patch = state["patch"]
        for name, group, shift, bits in OPERATOR_FIELDS:
            if name not in fields:
                raise ValueError(f"{line}: no {name}")
            patch[PATCH_GROUPS.index(group) * 4 + slot] |= _number(fields[name][0], name, 0, (1 << bits) - 1) << shift
        for group in PATCH_GROUPS:
            key = f"spare_{group:02X}"
            if key in fields:
                patch[PATCH_GROUPS.index(group) * 4 + slot] |= _number(fields[key][0], key, 0, 0xFF) & ~GROUP_MASKS[group] & 0xFF
        if "TL" not in fields or fields["TL"][1] not in ("fixed", "scaled"):
            raise ValueError(f"{line}: TL without a level and 'fixed' or 'scaled'")
        patch[21 + slot] = _number(fields["TL"][0], "TL", 0, 127) | (0x80 if fields["TL"][1] == "fixed" else 0)
        state["operators"].add(operator)
        if len(state["operators"]) < 4:
            return b""
        state["open"] = None
        return bytes(patch)
    return None


def fm_patches_to_text(data):
    """fm_patches: by patch number, a pointer to a 25-byte YM3438 voice."""
    header = ["# fm_patches: by patch number (hex; instruments give it), a pointer to the 25 bytes of a YM3438 voice,",
              "# written as an FM patch editor shows it. load_fm_patch writes the first 21 to fm_patch_registers",
              "# (30-3C DT/MUL, 50-5C KS/AR, 60-6C AM/D1R, 70-7C D2R, 80-8C SL/RR, then B0 FB/ALG; the four",
              "# registers of a kind are operators 1, 3, 2, 4); the last 4 are the operators' TL (fm_level_registers):",
              "# 'fixed' (bit 7) is written as it is and kept when the volume changes; 'scaled' gets the note's",
              "# attenuation (velocity, part and music volume) added at note on and at every volume change.",
              "# DT 0-7 (0-3 up, 4-7 down), MUL 0-15, TL 0-127, KS 0-3, AR 0-31, AM 0-1, D1R 0-31, D2R 0-31, SL 0-15,",
              "# RR 0-15; algorithm 0-7, feedback 0-7. A patch holds no SSG-EG (fm_operator_init sets 90-9E to 0 and",
              "# nothing writes them again) and no L/R/AMS/PMS (B4 is C0 | the channel's modulation at note on).",
              "# spare_NN: bits of register NN outside its fields (written only when set)."]
    count = _pointer_count(data, FM_PATCHES)
    return _finish(header + _pointed_text(data, FM_PATCHES, count,
                                          lambda index, value: f"patch {index:02X}  0x{value:04X}", _patch_records))


def fm_patches_from_text(text):
    return _pointed_bytes(text, "patch", lambda token: _number("0x" + token, "patch", 0, 0xFFFF), _patch_bytes)


# ---- drum kits ----

DRUM_FIELDS = {"step": 1, "pan": 2, "sample": 1}


def _drum_line(record, number):
    label = f"note {number:3d} {_note_name(number):<4}:"
    if record == EMPTY_DRUM_NOTE:
        return f"{label} none"
    step = record[0] | record[1] << 8
    sample = "none" if record[3] == 0xFF else f"{record[3]:02X}"
    return f"{label} step 0x{step:04X}  pan {_pan_text(record[2]):<7}  sample {sample}"


def _drum_bytes(line, state):
    head, colon, rest = line.partition(":")
    if not colon or head.split()[:1] != ["note"]:
        return None
    tokens = rest.split()
    if tokens == ["none"]:
        return EMPTY_DRUM_NOTE
    fields = _keyed(tokens, DRUM_FIELDS, line)
    for key in DRUM_FIELDS:
        if key not in fields:
            raise ValueError(f"{line}: no {key}")
    step = _number(fields["step"][0], "step", 0, 0xFFFF)
    sample = 0xFF if fields["sample"][0] == "none" else _hex_byte(fields["sample"][0], "sample")
    return bytes((step & 0xFF, step >> 8, _pan_value(*fields["pan"]), sample))


def drum_kits_to_text(data):
    """drum_kits: by program - F0, a pointer to 128 notes of 4 bytes."""
    header = ["# drum_kits: by program - F0 (drums N in the part templates), a pointer to the kit: 128 notes by MIDI",
              "# note number (not transposed; C4 = 60), 4 bytes each: step (the RF5C68 FD the sample plays at, in",
              "# 1/2048ths: 0x0800 its own rate, 0x0400 an octave down), pan (left and right levels 0-15), sample",
              "# (the pcm_sample_headers number, hex: the WAV assets/sound/samples/sample_NN.wav; none: FF, the",
              "# note is silent). 'none' alone is step 0x0400, pan L0 R0, no sample. The note before the colon is",
              "# a label: the notes go back in the order written."]
    count = _pointer_count(data, DRUM_KITS)
    return _finish(header + _pointed_text(data, DRUM_KITS, count,
                                          lambda index, value: f"drums {index}  0x{value:04X}",
                                          _fixed_records(4, _drum_line, DRUM_NOTES)))


def drum_kits_from_text(text):
    return _pointed_bytes(text, "drums", lambda token: _number(token, "drum kit", 0, 0xFFFF), _drum_bytes)


# ---- vibrato presets ----

VIBRATO_FIELDS = {"delay": 1, "wait": 1, "depth": 1, "range": 1, "rise": 1, "fall": 1, "phase": 1}


def _vibrato_line(record, number):
    delay, wait, vibrato_range, depth, rise, fall, phase = record
    return (f"delay {delay:3d}  wait {wait:3d}  depth {(vibrato_range & 7) << 8 | depth:4d}  range 0x{vibrato_range & 0xF8:02X}"
            f"  rise {rise:3d}  fall {fall:3d}  phase 0x{phase:02X}")


def _vibrato_bytes(line, state):
    tokens = line.split()
    if tokens[0] != "delay":
        return None
    fields = _keyed(tokens, VIBRATO_FIELDS, line)
    for key in VIBRATO_FIELDS:
        if key not in fields:
            raise ValueError(f"{line}: no {key}")
    depth = _number(fields["depth"][0], "depth", 0, 0x7FF)
    vibrato_range = _number(fields["range"][0], "range", 0, 0xFF)
    if vibrato_range & 7:
        raise ValueError(f"{line}: range has bits 0-2 set (they are the depth's)")
    return bytes((_number(fields["delay"][0], "delay", 0, 255), _number(fields["wait"][0], "wait", 0, 255),
                  vibrato_range | depth >> 8, depth & 0xFF, _number(fields["rise"][0], "rise", 0, 255),
                  _number(fields["fall"][0], "fall", 0, 255), _number(fields["phase"][0], "phase", 0, 0xFF)))


def vibrato_presets_to_text(data):
    """vibrato_presets: by CC 11's value, 7 bytes copied to an FM voice's vibrato fields."""
    header = ["# vibrato_presets: by CC 11's value (a channel's preset; at note on 0 means no vibrato), a pointer to",
              "# the 7 bytes copied to an FM voice's vibrato (step_fm_vibrato runs it, one voice a main loop pass):",
              "# delay (the voice's turns before it starts), wait (turns between steps), depth (how far the",
              "# F-number swings from the note's, 0-2047: byte 3, and bits 0-2 of byte 2), range (the rest of byte",
              "# 2: bits 4-6, uncertain, how many blocks the swing may cross; its high nibble is also added to",
              "# phase at note on), rise and fall (F-number units a step going up and down), phase (bit 0: going",
              "# down; bits 4-6 what range gets back at each turn)."]
    count = _pointer_count(data, VIBRATO_PRESETS)
    return _finish(header + _pointed_text(data, VIBRATO_PRESETS, count,
                                          lambda index, value: f"preset {index}  0x{value:04X}",
                                          _fixed_records(7, _vibrato_line, 1)))


def vibrato_presets_from_text(text):
    return _pointed_bytes(text, "preset", lambda token: _number(token, "preset", 0, 0xFFFF), _vibrato_bytes)


# ---- sample headers ----

def _sample_start(header):
    """Where a header's sample starts in the sound region (as tools/asset_formats.py pcm_sample_starts: the
    board's bank is A0 | (B0 bit 2) << 6 | (B0 bits 0-1) << 7, 2000h bytes each; the page is in the window)."""
    high, low, page = header[:3]
    bank = (low & 0x3F) | (high & 0x04) << 4 | (high & 0x03) << 7
    return bank * PCM_BANK + (page * PCM_PAGE & (PCM_BANK - 1))


def _sample_wavs(data, count):
    """{header address: WAV name} for the headers the 'count' pointers reach inside the piece: the WAV of a
    sample is named by the lowest number whose header starts where it does (extract_assets.py)."""
    first_number = {}
    starts = {}
    for number in range(count):
        word = struct.unpack_from("<H", data, 2 * number)[0]
        at = word - PCM_SAMPLE_HEADERS
        if 2 * count <= at <= len(data) - 3:
            start = _sample_start(data[at:at + 3])
            first_number.setdefault(start, number)
            starts[word] = start
    return {word: f"sample_{first_number[start]:02X}.wav" for word, start in starts.items()}


def pcm_sample_headers_to_text(data):
    """pcm_sample_headers: by sample number, a pointer to a 6-byte header; then the fill."""
    header = ["# pcm_sample_headers: by sample number (hex: the number instruments and drum kits give), a pointer to",
              "# the sample's 6-byte header; the headers follow in the order they lie in the ROM, each under the",
              "# numbers that point at it. bank: byte 0 (to port B0, the bank's high bits) and byte 1 (to port A0,",
              "# its low 6 bits); page: byte 2, where the sample starts in the A000-BFFF window (A0-BF); unread:",
              "# bytes 3-5 (a byte and an address in the window), copied to the voice and never read. The comment",
              "# gives the sample's WAV (assets/sound/samples) and where it starts in the sound region (the",
              "# sample plays up to an FFh byte)."]
    count = _pointer_count(data, PCM_SAMPLE_HEADERS)
    wavs = _sample_wavs(data, count)

    def header_line(record, number):
        return f"header  bank {record[0]:02X} {record[1]:02X}  page {record[2]:02X}  unread {record[3]:02X} {record[5]:02X}{record[4]:02X}"

    def records(part, address):
        lines = _fixed_records(6, header_line, 1)(part, address)
        if len(part) >= 6:
            lines[0] += f"    # {wavs.get(address, 'no WAV')}, sound region {_sample_start(part):06X}"
        return lines

    return _finish(header + _pointed_text(data, PCM_SAMPLE_HEADERS, count,
                                          lambda index, value: f"sample {index:02X}  0x{value:04X}", records))


def _sample_header_bytes(line, state):
    tokens = line.split()
    if tokens[0] != "header":
        return None
    fields = _keyed(tokens[1:], {"bank": 2, "page": 1, "unread": 2}, line)
    for key in ("bank", "page", "unread"):
        if key not in fields:
            raise ValueError(f"{line}: no {key}")
    address = _number("0x" + fields["unread"][1], "unread address", 0, 0xFFFF)
    return bytes((_hex_byte(fields["bank"][0], "bank"), _hex_byte(fields["bank"][1], "bank"),
                  _hex_byte(fields["page"][0], "page"), _hex_byte(fields["unread"][0], "unread"),
                  address & 0xFF, address >> 8))


def pcm_sample_headers_from_text(text):
    return _pointed_bytes(text, "sample", lambda token: _number("0x" + token, "sample", 0, 0xFFFF), _sample_header_bytes)


FORMATS = {
    "z80_fm_register_pairs": (".txt", fm_register_pairs_to_text, _register_pairs_from_text),
    "z80_pcm_register_pairs": (".txt", pcm_register_pairs_to_text, _register_pairs_from_text),
    "z80_voice_chip_channels": (".txt", voice_chip_channels_to_text, voice_chip_channels_from_text),
    "z80_pcm_channel_bits": (".txt", pcm_channel_bits_to_text, pcm_channel_bits_from_text),
    "z80_fm_register_list": (".txt", fm_register_list_to_text, fm_register_list_from_text),
    "z80_fm_fnum_tables": (".txt", fm_fnum_tables_to_text, fm_fnum_tables_from_text),
    "z80_pcm_pitch_tables": (".txt", pcm_pitch_tables_to_text, pcm_pitch_tables_from_text),
    "z80_pitch_table_pointers": (".txt", pitch_table_pointers_to_text, pitch_table_pointers_from_text),
    "z80_part_templates": (".txt", part_templates_to_text, part_templates_from_text),
    "z80_instruments": (".txt", instruments_to_text, instruments_from_text),
    "z80_note_codes": (".txt", note_codes_to_text, note_codes_from_text),
    "z80_velocity_levels": (".txt", velocity_levels_to_text, velocity_levels_from_text),
    "z80_pitch_bend_steps": (".txt", pitch_bend_steps_to_text, pitch_bend_steps_from_text),
    "z80_fm_patches": (".txt", fm_patches_to_text, fm_patches_from_text),
    "z80_drum_kits": (".txt", drum_kits_to_text, drum_kits_from_text),
    "z80_vibrato_presets": (".txt", vibrato_presets_to_text, vibrato_presets_from_text),
    "z80_pcm_sample_headers": (".txt", pcm_sample_headers_to_text, pcm_sample_headers_from_text),
    "z80_jump_table": (".txt", jump_table_to_text, jump_table_from_text),
}
