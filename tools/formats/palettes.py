"""
Name:        palettes.py
Description: the game's palettes as text: palette lists, colours, cycles, sprite palette words and the
             per-stage and per-player tables
Version:     0.1.0
Created:     14-10-2026
Modified:    14-10-2026
Author:      Sebastian Toyos

The colours the game keeps in the program ROM are 16-bit palette entries in the palette window's
alternate format (the one palette RAM shows from 608000: the low bit of blue, green and red in bits
14-12, the rest 4:4:4; runtime/src/machine.c to_alternate_format), little endian. A colour reads as
#RRGGBB (5 bits a component, scaled to 8 as in tools/data_formats.py), with '+' after it when bit 15 is
set. Palette RAM: 608000-608FFF the tile layers' 128 palettes, 609000 the text layer's (mixer text 002F),
609200 the bitmap layer's (the same base + 31FF8C = 0010), 609400 backdrop_color, 609800 the sprites'
(mixer sprite groups 003x), 60B800 the stage's sprite palettes (load_stage_palette).

The formats:
- palette_list: load_palette_list's lists (count, then source, palette RAM offset, kind; the size of each
  entry is the next u32 minus its source: the next entry's source, or the end pointer):
      entries 5
      copy_with_pen_0  text_palettes  to 0x609000       kind 0: copy_palettes_with_pen_0
      repeat  tile_bank_0_palette_repeats  tile_bank_0_palettes  to 0x608000
                                                        kind 4: repeat_palette_block (counts, palettes)
      copy  sprite_palettes  to 0x609800                kind 2: copy_palette_block
      end  tile_bank_1_palette_list                     where the last source ends
- palettes: 16-colour palettes, one a line: "05: #000000 #F8F8F8 ..." (the number is only a label).
- palettes_without_pen_0: the 15 colours (pens 1-15) of copy_palettes_with_pen_0's palettes, one a line.
- palette_frames: cycle_palette's frames, one 16-colour palette a line: "frame 03: ...".
- palette_repeat_counts: repeat_palette_block's counts, one a line: "05: 3" (palette 05 fills 3 slots).
- stage_palette_table: stage_palettes, one stage a line: "stage 04: stage_04_sprite_palettes 592" (source,
  colour count).
- player_palette_list_table: player_palette_lists, one stage a line: "stage 00: sonic X ray Y mighty Z".
- sprite_palette_lists: lists of sprite palette words (animate_sign's and the player's 80 palette_list):
  "last 1: 8130 8340 8130" (the last index, then the words).
- sprite_palettes_by_character: sprite palette words, 8 a character: "sonic: 8130 8370 ...".
- sprite_palette_cycles: animate_hud's and animate_sign's cycles, one after the other: "last 3: 84A0 84B0
  84C0 84D0" (the last index, then that many words and one more); a comment marks where each one starts.
- palette_table, fire_vent_blast_palettes: sprite palette words one a line, "palette word 0x9110  # index
  00" (records in columns, tools/formats/common.py).

Pointers read as the name of the place symbols.txt names there, or as 0x10ABCD; both go back. '#'
followed by a space starts a comment (a colour's '#' is followed by its digits). What a format cannot
read as its records stays as "bytes 01 02 ..." lines, so every piece goes back byte for byte.
"""
import re
import struct
from pathlib import Path

from formats.common import Rec, indexed

SYMBOLS = Path(__file__).resolve().parents[2] / "symbols.txt"

DATA_START = 0x100000              # the program ROM's data area, where the pointers point
DATA_END = 0x180000
PALETTE_RAM = 0x600000
CHARACTERS = ("sonic", "ray", "mighty")    # object kind 0, 1, 2 (sonic/ray/mighty_standing_animations)
LIST_KINDS = {0: "copy_with_pen_0", 2: "copy", 4: "repeat"}    # load_palette_list's jump (9A4DF + kind)
KIND_CODES = {name: code for code, name in LIST_KINDS.items()}
COLOR = re.compile(r"#([0-9A-Fa-f]{6})(\+?)$")
COMMENT = re.compile(r"(^|\s)#(\s.*)?$")
HEX_PER_LINE = 16

_names = None


# ---- names ----

def _load_names():
    """({address: name} of the data and table lines, {name: address} of every line), from symbols.txt."""
    global _names
    if _names is None:
        by_address, by_name = {}, {}
        if SYMBOLS.exists():
            for line in SYMBOLS.read_text(encoding="utf-8").splitlines():
                match = re.match(r"([0-9A-F]{6})\s+(\w+)\s+(\w+)", line)
                if not match:
                    continue
                address = int(match.group(1), 16)
                if match.group(2) in ("data", "table"):
                    by_address.setdefault(address, match.group(3))
                by_name.setdefault(match.group(3), address)
        _names = (by_address, by_name)
    return _names


def _pointer_text(address):
    return _load_names()[0].get(address, f"0x{address:06X}")


def _pointer_value(token):
    if token.lower().startswith("0x"):
        return int(token, 16)
    address = _load_names()[1].get(token)
    if address is None:
        raise ValueError(f"unknown name: {token}")
    return address


def _is_data_pointer(address):
    return DATA_START <= address < DATA_END


# ---- shared pieces of text ----

def _color_text(value):
    red = ((value >> 12) & 1) | ((value << 1) & 0x1E)
    green = ((value >> 13) & 1) | ((value >> 3) & 0x1E)
    blue = ((value >> 14) & 1) | ((value >> 7) & 0x1E)
    return f"#{red * 255 // 31:02X}{green * 255 // 31:02X}{blue * 255 // 31:02X}" + ("+" if value & 0x8000 else "")


def _color_value(token):
    match = COLOR.match(token)
    if not match:
        raise ValueError(f"not a colour: {token}")
    digits = match.group(1)
    red, green, blue = (round(int(digits[at:at + 2], 16) * 31 / 255) for at in (0, 2, 4))
    return (0x8000 if match.group(2) else 0) | (blue & 1) << 14 | (green & 1) << 13 | (red & 1) << 12 \
        | (blue & 0x1E) << 7 | (green & 0x1E) << 3 | (red & 0x1E) >> 1


def _bytes_lines(data):
    return [("bytes " + " ".join(f"{byte:02X}" for byte in data[at:at + HEX_PER_LINE]))
            for at in range(0, len(data), HEX_PER_LINE)]


def _lines(text):
    """The text's lines without comments and blank lines."""
    for line in text.split("\n"):
        line = COMMENT.sub("", line).strip()
        if line:
            yield line


def _after_label(line):
    """What follows 'label:' (the whole line when there is no label)."""
    return line.split(":", 1)[1] if ":" in line else line


def _bytes_value(line):
    return bytes(int(byte, 16) for byte in line.split()[1:])


def _u16s(data):
    return [value for (value,) in struct.iter_unpack("<H", data[:len(data) // 2 * 2])]


def _text(header, lines):
    return "\n".join(header + lines) + "\n"


# ---- colours ----

def _color_rows(data, per_row, label, header):
    values = _u16s(data)
    lines = list(header)
    for row, at in enumerate(range(0, len(values), per_row)):
        lines.append(f"{label}{row:02X}: " + " ".join(f"{_color_text(value):<8}" for value in values[at:at + per_row]).rstrip())
    if len(data) % 2:
        lines += _bytes_lines(data[-1:])
    return "\n".join(lines) + "\n"


def _colors_from_text(text):
    out = bytearray()
    for line in _lines(text):
        if line.startswith("bytes"):
            out += _bytes_value(line)
            continue
        for token in _after_label(line).split():
            out += struct.pack("<H", _color_value(token))
    return bytes(out)


def palettes_to_text(data):
    return _color_rows(data, 16, "", [
        "# 16-colour palettes in the alternate format, one a line, pens 0-15: #RRGGBB (5 bits a component),",
        "# '+' where bit 15 is set; the number before ':' is the palette's place in this piece (a label)"])


def palettes_without_pen_0_to_text(data):
    return _color_rows(data, 15, "", [
        "# copy_palettes_with_pen_0's palettes: pens 1-15 of each, one a line (pen 0 is not kept here:",
        "# load_palette_list writes 0000 in front of each); #RRGGBB, '+' where bit 15 is set"])


def palette_frames_to_text(data):
    return _color_rows(data, 16, "frame ", [
        "# cycle_palette's frames: one 16-colour palette a line, in the order the cycle shows them (a frame",
        "# goes to every row the caller names); #RRGGBB, '+' where bit 15 is set"])


# ---- load_palette_list's lists ----

def palette_list_to_text(data):
    lines = [
        "# load_palette_list (09A4BC): the entries, each copied to palette RAM at its 'to' (60xxxx):",
        "#   copy_with_pen_0 SOURCE         palettes of 15 colours, each written after a pen 0 of 0000",
        "#   copy SOURCE                    the colours as they are",
        "#   repeat COUNTS PALETTES         each 16-colour palette written as many times as its count says",
        "# an entry's size is the next pointer minus its source (the next entry's, or 'end')",
    ]
    if len(data) < 2:
        return _text(lines, _bytes_lines(data))
    count = struct.unpack_from("<H", data, 0)[0]
    lines.append(f"entries {count}")
    at = 2
    entries = []
    for _ in range(count):
        if at + 8 > len(data):
            break
        source, dest, kind = struct.unpack_from("<IHH", data, at)
        palettes = None
        if kind == 4:
            if at + 12 > len(data):
                break
            palettes = struct.unpack_from("<I", data, at + 8)[0]
        entries.append((source, dest, kind, palettes))
        at += 8 if palettes is None else 12
    end = None
    if entries and entries[-1][2] != 4 and at + 4 <= len(data):
        end = struct.unpack_from("<I", data, at)[0]
    for index, (source, dest, kind, palettes) in enumerate(entries):
        name = LIST_KINDS.get(kind, f"kind_{kind:04X}")
        pointers = _pointer_text(source) + ("" if palettes is None else "  " + _pointer_text(palettes))
        following = palettes if palettes is not None else (
            entries[index + 1][0] if index + 1 < len(entries) else end)
        lines.append(f"{name}  {pointers}  to 0x{PALETTE_RAM | dest:06X}" + _entry_note(source, dest, kind, following))
    if end is not None:
        lines.append(f"end  {_pointer_text(end)}")
        at += 4
    return _text(lines, _bytes_lines(data[at:]))


def _entry_note(source, dest, kind, following):
    """A comment with what the entry writes, when its size makes sense."""
    if following is None or not source < following <= source + 0x10000:
        return ""
    halfwords = (following - source) >> 1
    first = (dest >> 1) & 0x3FFF
    if kind == 0:
        palettes = max(1, -(-halfwords // 15))
        return f"  # {palettes} palettes, colours {first:03X}-{first + palettes * 16 - 1:03X}"
    if kind == 2:
        return f"  # {halfwords} colours, {first:03X}-{first + halfwords - 1:03X}"
    if kind == 4:
        return f"  # {halfwords} palettes from colour {first:03X}"
    return ""


def palette_list_from_text(text):
    out = bytearray()
    for line in _lines(text):
        tokens = line.split()
        word = tokens[0]
        if word == "bytes":
            out += _bytes_value(line)
        elif word == "entries":
            out += struct.pack("<H", int(tokens[1], 0))
        elif word == "end":
            out += struct.pack("<I", _pointer_value(tokens[1]))
        else:
            if word in KIND_CODES:
                kind = KIND_CODES[word]
            elif word.startswith("kind_"):
                kind = int(word[5:], 16)
            else:
                raise ValueError(f"not a palette list line: {line}")
            if "to" not in tokens:
                raise ValueError(f"no 'to' in: {line}")
            split = tokens.index("to")
            pointers = [_pointer_value(token) for token in tokens[1:split]]
            dest = int(tokens[split + 1], 16) & 0xFFFF
            if len(pointers) != (2 if kind == 4 else 1):
                raise ValueError(f"wrong number of pointers in: {line}")
            out += struct.pack("<IHH", pointers[0], dest, kind)
            if kind == 4:
                out += struct.pack("<I", pointers[1])
    return bytes(out)


# ---- repeat_palette_block's counts ----

def palette_repeat_counts_to_text(data):
    lines = [
        "# repeat_palette_block's counts: palette NN of the palettes the list names next is written COUNT",
        "# times in a row (slots: the 16-colour palettes it fills from the list's 'to'); counts in decimal",
    ]
    slot = 0
    for index, count in enumerate(_u16s(data)):
        last = slot + max(count, 1) - 1
        lines.append(f"{index:02X}: {count}" + (f"  # slot {slot:02X}" if last == slot else f"  # slots {slot:02X}-{last:02X}"))
        slot = last + 1
    if len(data) % 2:
        lines += _bytes_lines(data[-1:])
    return "\n".join(lines) + "\n"


def palette_repeat_counts_from_text(text):
    out = bytearray()
    for line in _lines(text):
        if line.startswith("bytes"):
            out += _bytes_value(line)
            continue
        for token in _after_label(line).split():
            out += struct.pack("<H", int(token, 0) if token.lower().startswith("0x") else int(token, 10))
    return bytes(out)


# ---- stage_palettes ----

def stage_palette_table_to_text(data):
    lines = [
        "# stage_palettes (load_stage_palette): per stage the source and how many colours go to palette",
        "# RAM 60B800 (the sprite palettes from colour 1C00); counts in decimal",
    ]
    at = 0
    stage = 0
    while at + 8 <= len(data):
        source, count = struct.unpack_from("<II", data, at)
        if not (_is_data_pointer(source) and count <= 0x4000 and source + 2 * count <= DATA_END):
            break
        lines.append(f"stage {stage:02X}: {_pointer_text(source)}  {count}" + (f"  # {count // 16} palettes" if count % 16 == 0 else ""))
        at += 8
        stage += 1
    if at + 4 <= len(data) and _is_data_pointer(struct.unpack_from("<I", data, at)[0]):
        source = struct.unpack_from("<I", data, at)[0]
        lines.append(f"stage {stage:02X}: {_pointer_text(source)}  # a source with no count of its own: the 4 bytes after it would be read as one")
        at += 4
    return _text(lines, _bytes_lines(data[at:]))


def stage_palette_table_from_text(text):
    out = bytearray()
    for line in _lines(text):
        if line.startswith("bytes"):
            out += _bytes_value(line)
            continue
        tokens = _after_label(line).split()
        out += struct.pack("<I", _pointer_value(tokens[0]))
        if len(tokens) > 1:
            out += struct.pack("<I", int(tokens[1], 0) if tokens[1].lower().startswith("0x") else int(tokens[1], 10))
    return bytes(out)


# ---- player_palette_lists ----

def player_palette_list_table_to_text(data):
    lines = [
        "# player_palette_lists (player_reset): per stage, each character's palette list (player 80",
        "# palette_list; the player's palette is the list's word +2)",
    ]
    at = 0
    stage = 0
    while at + 12 <= len(data):
        pointers = struct.unpack_from("<3I", data, at)
        if not all(_is_data_pointer(pointer) for pointer in pointers):
            break
        lines.append(f"stage {stage:02X}: " + "  ".join(f"{character} {_pointer_text(pointer)}"
                                                       for character, pointer in zip(CHARACTERS, pointers)))
        at += 12
        stage += 1
    return _text(lines, _bytes_lines(data[at:]))


def player_palette_list_table_from_text(text):
    out = bytearray()
    for line in _lines(text):
        if line.startswith("bytes"):
            out += _bytes_value(line)
            continue
        for token in _after_label(line).split():
            if token not in CHARACTERS:
                out += struct.pack("<I", _pointer_value(token))
    return bytes(out)


# ---- sprite palette words ----

def sprite_palette_lists_to_text(data):
    """Lists of four words: the last index, then three sprite palette words (the player's lists are all
    like that); a record that does not look like one (an index above 15 or a palette word 0) ends them."""
    lines = [
        "# sprite palette lists: the last index (animate_sign wraps after it), then the sprite palette words",
        "# (sprite_palette: the palette, the bits above it the mixer's priority group); the first one, at",
        "# +2, is what player_reset and the flashes put back as the player's palette",
    ]
    at = 0
    while at + 8 <= len(data):
        last, *palettes = struct.unpack_from("<4H", data, at)
        if last > 15 or 0 in palettes:
            break
        lines.append(f"last {last}: " + " ".join(f"{palette:04X}" for palette in palettes))
        at += 8
    return _text(lines, _bytes_lines(data[at:]))


def sprite_palette_lists_from_text(text):
    out = bytearray()
    for line in _lines(text):
        if line.startswith("bytes"):
            out += _bytes_value(line)
            continue
        label, _, words = line.partition(":")
        out += struct.pack("<H", int(label.split()[1], 10))
        for token in words.split():
            out += struct.pack("<H", int(token, 16))
    return bytes(out)


def sprite_palette_cycles_to_text(data):
    """Lists of a last index N, then N + 1 sprite palette words, one after the other; a word that is no
    last index (above 0x40) or a list that runs past the piece ends them."""
    lines = [
        "# sprite palette cycles (animate_hud, animate_sign, cycle_part_palettes): the last index (the cycle goes",
        "# back to the first word after it), then the sprite palette words, one a step (sprite_palette: the",
        "# palette, the bits above it the mixer's priority group); the comments mark where each list starts",
    ]
    values = _u16s(data)
    at = 0
    while at < len(values):
        last = values[at]
        if last > 0x40 or at + 2 + last > len(values):
            break
        if at:
            lines.append(f"# +{at * 2:04X}")
        lines.append(f"last {last}: " + " ".join(f"{palette:04X}" for palette in values[at + 1:at + 2 + last]))
        at += 2 + last
    return _text(lines, _bytes_lines(data[at * 2:]))


def sprite_palettes_by_character_to_text(data):
    lines = [
        "# sprite palette words (sprite_palette), 8 a character: [character x 8 + animation_frame]",
    ]
    values = _u16s(data)
    rows = len(values) // 8
    for row in range(rows):
        label = CHARACTERS[row] if row < len(CHARACTERS) else f"character_{row}"
        lines.append(f"{label}: " + " ".join(f"{value:04X}" for value in values[row * 8:row * 8 + 8]))
    return _text(lines, _bytes_lines(data[rows * 16:]))


def sprite_palettes_by_character_from_text(text):
    out = bytearray()
    for line in _lines(text):
        if line.startswith("bytes"):
            out += _bytes_value(line)
            continue
        for token in _after_label(line).split():
            out += struct.pack("<H", int(token, 16))
    return bytes(out)


FORMATS = {
    "palette_list": (".txt", palette_list_to_text, palette_list_from_text),
    "palettes": (".pal.txt", palettes_to_text, _colors_from_text),
    "palettes_without_pen_0": (".pal.txt", palettes_without_pen_0_to_text, _colors_from_text),
    "palette_frames": (".pal.txt", palette_frames_to_text, _colors_from_text),
    "palette_repeat_counts": (".txt", palette_repeat_counts_to_text, palette_repeat_counts_from_text),
    "stage_palette_table": (".txt", stage_palette_table_to_text, stage_palette_table_from_text),
    "player_palette_list_table": (".txt", player_palette_list_table_to_text, player_palette_list_table_from_text),
    "sprite_palette_lists": (".txt", sprite_palette_lists_to_text, sprite_palette_lists_from_text),
    "sprite_palettes_by_character": (".txt", sprite_palettes_by_character_to_text, sprite_palettes_by_character_from_text),
    "sprite_palette_cycles": (".txt", sprite_palette_cycles_to_text, sprite_palette_lists_from_text),
}


# ---- tables of sprite palette words, one a line ----

def _palette_table(header, label=None):
    """A table of sprite palette words (sprite_palette: the palette, the mixer group above it), one a line."""
    return indexed(header, Rec("palette", 2, [("word", 0, "H", "x")]), label)


FORMATS.update({
    "palette_table": _palette_table(["Sprite palette words (sprite_palette: the palette, the mixer group above",
                                     "it), one a line, by index."], "index"),
    "fire_vent_blast_palettes": _palette_table(
        ["fire_vent_blast_palettes (fire_vent_blast): the blast's sprite palette by its animation step."], "step"),
})
