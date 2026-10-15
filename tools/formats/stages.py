"""
Name:        stages.py
Description: the level data as text: floor blocks and their screen bounds, floor types and slopes, the stage
             spawn lists, restart points, player paths, goals and the small tables that go with them; the
             per-stage tables, block maps, events and object templates of the code chip at 000000, the
             ending's timed lists; the stage objects of the code chip at 080000 (spawn lists and groups,
             chase scripts and paths, walls, pieces that fly off, the readers' own tables)
Version:     0.1.0
Created:     14-10-2026
Modified:    14-10-2026
Author:      Sebastian Toyos

Every format here writes one record a line: a keyword, sometimes a label (the record's index or position,
checked on the way back), then 'field value' pairs. Values read as the game uses them: scroll positions,
indices, flags and pointers in hex (0x..), coordinates and heights in decimal (signed where the game reads
them signed), floor block edges (16.16 fixed point) as decimals with as many places as it takes to come back
exactly. '#' starts a comment. 'bytes' and 'words' lines (hex, words little endian, an optional 0x.. position
label first) keep what no record explains. Pointers into the program are written as program addresses
(0x10ABCD), followed by the name symbols.txt gives that place, as a comment.

The tables of one record a stage: per-stage pointers (stage_pointers, u32) and u16 offsets from the table
(stage_*_offsets: one format a table, its address in it), written as the address they reach; per-stage
values ('stage NN  field value', sound_by_stage among them). The lists the stage readers walk (events, ring
rows, link points, the ending's timed lists) are labelled records of tools/formats/common.py; a table that
symbols.txt cuts where the code reads a field takes one format a field: NAME for the whole records,
NAME_from_FIELD for a piece that starts at FIELD (object_templates, wall_kinds, stage_object_records_2,
rope_offsets, event_1a_scroll_script). The stage objects of the code chip at 080000 are records in columns
(tools/formats/common.py): a keyword, then 'field value' pairs, the names of addresses after them; their
offset tables say '+XXXX', bytes from the start of the piece, and a comment marks where each list starts.
A chase script's or path's entry is read once its keys are reached and sets the screen threshold, the
catch-up factor, the base speed and the wait; 'stop' (a negative factor) ends it.

A piece never changes size (the code reaches every piece at a fixed address): a level designer moves a
block, an object or a restart point by editing its numbers, not by adding lines.
"""
import re
import struct
from pathlib import Path

from formats.common import (END, KEPT, POINT, WORK_RAM, Out, Rec, address_value, content, end_rec, fixed_text,
                            fixed_value, fmt, frames_line, frames_special, hex_lines, indexed, join, labelled_line,
                            labelled_list, labelled_table, offset_lists_text, phased_formats, phased_text, raw,
                            record_tokens, run_of, s16_at, table_text, u16_at)

_ROOT = Path(__file__).resolve().parents[2]
PROGRAM_END = 0x200000      # the program ROM (code chips and data area, with their mirrors)
_names = None


def _name(address):
    """The name symbols.txt gives a data place, a table or a function at 'address' (comments only)."""
    global _names
    if _names is None:
        _names = {}
        try:
            for line in (_ROOT / "symbols.txt").read_text(encoding="utf-8").splitlines():
                match = re.match(r"([0-9A-F]{6})\s+(data|table|func|code)\s+(\w+)", line)
                if match:
                    _names.setdefault(int(match.group(1), 16), match.group(3))
        except OSError:
            pass
    return _names.get(address)


def _target_name(address, back=0):
    """The name of a pointer's target ('name + back' when the name sits 'back' bytes before it), or None."""
    name = _name(address)
    if name is None and back and _name(address - back):
        name = f"{_name(address - back)} + {back}"
    return name


def _note(*parts):
    """A trailing comment of the parts that are there."""
    parts = [part for part in parts if part]
    return "  # " + ", ".join(parts) if parts else ""


# ---- reading the text ----

def _number(token, where):
    """A value as written: 0x.. hex or decimal, either may be negative."""
    try:
        if token.lstrip("-+").lower().startswith("0x"):
            return int(token, 16)
        return int(token, 10)
    except ValueError:
        raise ValueError(f"{where}: '{token}' is not a number") from None


def _address(token, where):
    """A program address as written: a number, or a name of symbols.txt (name, name+0xNN)."""
    return _number(token, where) if token[:1].isdigit() or token[:1] in "+-" else address_value(token, where)


def _label(token, where):
    """A record label: hex, with or without 0x."""
    try:
        return int(token, 16)
    except ValueError:
        raise ValueError(f"{where}: '{token}' is not a label") from None


def _pairs(tokens, where):
    if len(tokens) % 2:
        raise ValueError(f"{where}: a field without a value")
    pairs = {}
    for name, value in zip(tokens[0::2], tokens[1::2]):
        if name in pairs:
            raise ValueError(f"{where}: '{name}' twice")
        pairs[name] = value
    return pairs


def _check(found, expected, where, what="label"):
    if found != expected:
        raise ValueError(f"{where}: {what} {found:X} where {expected:X} was due")


def _raw(keyword, tokens, out, position, where):
    """A 'bytes' or 'words' line: an optional 0x.. label (the position, from 'position'), then hex values."""
    if tokens and tokens[0].lower().startswith("0x"):
        _check(_label(tokens[0], where), position + len(out), where, "position")
        tokens = tokens[1:]
    try:
        if keyword == "bytes":
            out += bytes(int(token, 16) for token in tokens)
        else:
            out += b"".join(struct.pack("<H", int(token, 16)) for token in tokens)
    except (ValueError, struct.error):
        raise ValueError(f"{where}: a {keyword} line holds something that is not hex") from None


# ---- writing ----

def _bytes_lines(data, label_base=None, per_line=16):
    """'bytes' lines for what no record explains; with label_base, each starts with its position."""
    lines = []
    for at in range(0, len(data), per_line):
        label = f"0x{label_base + at:03X}  " if label_base is not None else ""
        lines.append(f"bytes {label}" + " ".join(f"{byte:02X}" for byte in data[at:at + per_line]))
    return lines


def _words_lines(data, label_base=None, per_line=8):
    if len(data) % 2:
        return _bytes_lines(data, label_base)
    lines = []
    for at in range(0, len(data), per_line * 2):
        label = f"0x{label_base + at:03X}  " if label_base is not None else ""
        words = struct.unpack_from(f"<{min(per_line * 2, len(data) - at) // 2}H", data, at)
        lines.append(f"words {label}" + " ".join(f"{word:04X}" for word in words))
    return lines


# ---- records ----

_SIZES = {"B": 1, "b": 1, "H": 2, "h": 2, "I": 4, "i": 4}


class _Field:
    """One field of a record: its name, offset, struct code and how it reads (dec, hex, hex2: at least two
    digits, fixed, or ('address', base): an offset from 'base' written as the program address it reaches)."""

    def __init__(self, name, offset, code, style="dec"):
        self.name, self.offset, self.code, self.style = name, offset, code, style
        self.size = _SIZES[code]

    def text(self, data, at):
        value = struct.unpack_from("<" + self.code, data, at)[0]
        if self.style == "hex":
            return f"0x{value:0{self.size * 2}X}"
        if self.style == "hex2":
            return f"0x{value:02X}"
        if self.style == "fixed":
            return fixed_text(value)
        if isinstance(self.style, tuple):
            return f"0x{self.style[1] + value:06X}"
        return str(value)

    def pack(self, token, where):
        if self.style == "fixed":
            try:
                value = fixed_value(token)
            except ArithmeticError:
                raise ValueError(f"{where}: '{token}' is not a number") from None
        else:
            value = _number(token, where)
            if isinstance(self.style, tuple):
                value -= self.style[1]
        bits = self.size * 8
        if not -(1 << (bits - 1)) <= value < (1 << bits):
            raise ValueError(f"{where}: {self.name} {token} does not fit in {bits} bits")
        return (value & ((1 << bits) - 1)).to_bytes(self.size, "little")


class _Layout:
    """A record: fields that tile its bytes."""

    def __init__(self, size, *fields):
        self.size = size
        self.fields = [_Field(*field) for field in fields]
        self.by_name = {field.name: field for field in self.fields}

    def tiles(self, lo, hi):
        """Whether whole fields cover the record's bytes lo..hi exactly."""
        at = lo
        for field in self.fields:
            if field.offset == at and field.offset + field.size <= hi:
                at += field.size
        return at == hi

    def text(self, data, at, lo=0, hi=None, skip=()):
        """'name value' pairs of the fields of the record at data[at:] within its bytes lo..hi."""
        hi = self.size if hi is None else hi
        return "  ".join(f"{field.name} {field.text(data, at + field.offset)}" for field in self.fields
                         if lo <= field.offset and field.offset + field.size <= hi and field.name not in skip)

    def pack(self, pairs, where, partial=False):
        """The bytes of the fields named in 'pairs': all of them, or with 'partial' (a record a piece cuts)
        a run of fields that follow each other."""
        unknown = [name for name in pairs if name not in self.by_name]
        if unknown:
            raise ValueError(f"{where}: no field '{unknown[0]}' here")
        missing = [field.name for field in self.fields if field.name not in pairs]
        if missing and not partial:
            raise ValueError(f"{where}: '{missing[0]}' is missing")
        fields = sorted((self.by_name[name] for name in pairs), key=lambda field: field.offset)
        out = bytearray()
        for index, field in enumerate(fields):
            if index and field.offset != fields[index - 1].offset + fields[index - 1].size:
                raise ValueError(f"{where}: a field is missing before '{field.name}'")
            out += field.pack(pairs[field.name], where)
        return out


def _record(tokens, layout, out, where, label_check=None, partial=False):
    """A record line's tokens after the keyword: an optional label (when the count is odd), then pairs."""
    if len(tokens) % 2:
        if label_check is not None:
            label_check(_label(tokens[0], where))
        tokens = tokens[1:]
    out += layout.pack(_pairs(tokens, where), where, partial)


def _expect(keyword, allowed, where):
    if keyword not in allowed:
        raise ValueError(f"{where}: '{keyword}' is not a line this file takes")


def _is_value(token):
    return token[:1].isdigit() or token[:1] in "-+"


def _align(lines):
    """Lines with the same keyword and field names get their values in columns (numbers lined up on the
    decimal point) and their comments lined up; 'bytes', 'words' and comment lines stay as they are."""
    parsed = []
    widths = {}
    for line in lines:
        content, _, comment = line.partition("#")
        tokens = content.split()
        if not tokens or tokens[0] in ("bytes", "words"):
            parsed.append((None, line))
            continue
        shape = tuple("*" if _is_value(token) else token for token in tokens)
        column = widths.setdefault(shape, [(0, 0)] * len(tokens))
        for index, token in enumerate(tokens):
            whole, dot, fraction = token.partition(".")
            column[index] = (max(column[index][0], len(whole)), max(column[index][1], len(dot + fraction)))
        parsed.append((shape, (tokens, comment.strip())))
    rebuilt = []
    for shape, item in parsed:
        if shape is None:
            rebuilt.append((None, item))
            continue
        tokens, comment = item
        column = widths[shape]
        text = tokens[0]
        for index in range(1, len(tokens)):
            token = tokens[index]
            if _is_value(token):
                whole, dot, fraction = token.partition(".")
                after_value = index > 1 and _is_value(tokens[index - 1])
                text += ("  " if after_value else " ") + whole.rjust(column[index][0]) \
                    + (dot + fraction).ljust(column[index][1])
            else:
                text += "  " + token
        rebuilt.append((shape, (text.rstrip(), comment)))
    content_widths = {}
    for shape, item in rebuilt:
        if shape is not None and item[1]:
            content_widths[shape] = max(content_widths.get(shape, 0), len(item[0]))
    out = []
    for shape, item in rebuilt:
        if shape is None:
            out.append(item)
        elif item[1]:
            out.append(item[0].ljust(content_widths[shape]) + "  # " + item[1])
        else:
            out.append(item[0])
    return out


def _join(header, lines):
    return "".join(f"# {line}\n" if line else "#\n" for line in header) \
        + "".join(line.rstrip() + "\n" for line in _align(lines))


# ---- tables indexed by stage (or by anything else) ----

def _indexed_format(keyword, layout, header, first=0, digits=2, comment=None):
    """A table of records, one per index (stage, floor type...), then whatever follows as bytes."""

    def to_text(data):
        lines = []
        count = len(data) // layout.size
        for index in range(count):
            note = comment(data, index * layout.size, first + index) if comment else ""
            lines.append(f"{keyword} {first + index:0{digits}X}  " + layout.text(data, index * layout.size) + note)
        lines += _bytes_lines(data[count * layout.size:])
        return _join(header, lines)

    def from_text(text):
        out = bytearray()
        for number, tokens in content(text):
            where = f"line {number}"
            if tokens[0] in ("bytes", "words"):
                _raw(tokens[0], tokens[1:], out, 0, where)
                continue
            _expect(tokens[0], (keyword,), where)
            due = first + len(out) // layout.size
            _record(tokens[1:], layout, out, where, lambda label: _check(label, due, where))
        return bytes(out)

    return to_text, from_text


def _pointer_format(header, base=None, first=0, keyword="stage", back=0, none_at_zero=False, comment=None):
    """A table of pointers, one per stage: absolute (u32) when 'base' is None, else u16 offsets from 'base'.
    Written as the program address they reach (a name of symbols.txt in its place is read too)."""
    size = 4 if base is None else 2

    def to_text(data):
        lines = []
        count = len(data) // size
        for index in range(count):
            value = struct.unpack_from("<I" if size == 4 else "<H", data, index * size)[0]
            note = comment(first + index) if comment else None
            if none_at_zero and value == 0:
                lines.append(f"{keyword} {first + index:02X}  none" + _note(note))
                continue
            target = value if base is None else base + value
            name = _target_name(target, back) if target < PROGRAM_END else "not a program address"
            lines.append(f"{keyword} {first + index:02X}  0x{target:06X}" + _note(name, note))
        lines += _bytes_lines(data[count * size:])
        return _join(header, lines)

    def from_text(text):
        out = bytearray()
        for number, tokens in content(text):
            where = f"line {number}"
            if tokens[0] in ("bytes", "words"):
                _raw(tokens[0], tokens[1:], out, 0, where)
                continue
            _expect(tokens[0], (keyword,), where)
            if len(tokens) != 3:
                raise ValueError(f"{where}: '{keyword} NN ADDRESS' expected")
            _check(_label(tokens[1], where), first + len(out) // size, where)
            if none_at_zero and tokens[2] == "none":
                value = 0
            else:
                value = _address(tokens[2], where) - (0 if base is None else base)
            if not 0 <= value < (1 << (size * 8)):
                raise ValueError(f"{where}: {tokens[2]} is out of the table's reach")
            out += value.to_bytes(size, "little")
        return bytes(out)

    return to_text, from_text


# ---- stage spawn lists (spawn_objects_by_type) ----

_SPAWN = _Layout(14, ("at", 0, "H", "hex"), ("x", 2, "H"), ("y", 4, "h"), ("z", 6, "h"),
                 ("type", 8, "H", "hex2"), ("arg_1", 10, "H", "hex"), ("arg_2", 12, "H", "hex"))

_SPAWN_HEADER = [
    "Objects spawn_objects_by_type starts in this stage, sorted by scroll position: each record waits until",
    "scroll_position passes 'at', then starts one object of 'type' (an index into object_types: its handler",
    "and how many sprite pieces it takes) at projected (map screen) position x, y and height z; arg_1 and",
    "arg_2 go to the object's spawn_arg_1 / spawn_arg_2 (fields 92, 94). 'end' (FFFF) closes the list.",
]


def type_spawns_to_text(data):
    lines = []
    at = 0
    while at < len(data):
        if at + 2 <= len(data) and struct.unpack_from("<H", data, at)[0] == 0xFFFF:
            lines.append("end")
            lines += _bytes_lines(data[at + 2:])
            break
        if at + _SPAWN.size > len(data):
            lines += _bytes_lines(data[at:])
            break
        lines.append("spawn  " + _SPAWN.text(data, at))
        at += _SPAWN.size
    return _join(_SPAWN_HEADER, lines)


def type_spawns_from_text(text):
    out = bytearray()
    for number, tokens in content(text):
        where = f"line {number}"
        if tokens[0] in ("bytes", "words"):
            _raw(tokens[0], tokens[1:], out, 0, where)
        elif tokens[0] == "end":
            out += b"\xFF\xFF"
        else:
            _expect(tokens[0], ("spawn",), where)
            _record(tokens[1:], _SPAWN, out, where)
    return bytes(out)


# ---- floor blocks ----

_FLOOR_BLOCK = _Layout(22, ("x_min", 0, "i", "fixed"), ("x_max", 4, "i", "fixed"), ("y_min", 8, "i", "fixed"),
                       ("y_max", 12, "i", "fixed"), ("height", 16, "H"), ("type", 18, "B", "hex"),
                       ("slope", 19, "B", "hex"), ("fall_priorities", 20, "B", "hex"),
                       ("ledge_side", 21, "B", "hex"))

_FLOOR_BLOCK_FIELDS = [
    "A block is a box of the map in ground coordinates: x_min..x_max, y_min..y_max (16.16, inclusive), its",
    "floor at 'height' (z: larger is lower; 2048 = no floor, and find_floor does not keep 2047 or more as",
    "the previous floor), 'type' an index into floor_types (the floor's behaviour and attribute), 'slope'",
    "an index into floor_slopes (00 = flat), fall_priorities an index into fall_sprite_priorities (the",
    "sprite priority by side while a player falls off it) and ledge_side a side (0-3) the teetering and",
    "falling code takes when the player is past none of the block's sides.",
]


def floor_block_list_to_text(data):
    """The stage's floor blocks: a count, then 22-byte blocks (and what follows them)."""
    lines = []
    count = struct.unpack_from("<H", data, 0)[0] if len(data) >= 2 else None
    if count is not None:
        lines.append(f"count {count}")
    at = 2
    index = 0
    while at + _FLOOR_BLOCK.size <= len(data):
        if index == count:
            lines.append("# past the count: blocks the game reaches only by their offset (objects, links)")
        lines.append(f"block 0x{at - 2:04X}  " + _FLOOR_BLOCK.text(data, at))
        at += _FLOOR_BLOCK.size
        index += 1
    lines += _bytes_lines(data[at:] if len(data) >= 2 else data)
    header = ["The floor blocks of a stage (floor_blocks): find_floor looks for the one under an object among",
              "those find_nearby_floor_blocks found near the view. Each block's label is its offset (what an",
              "object's floor_block holds). 'count' is how many blocks the stage has outlines for; the game",
              "takes that count from floor_block_bounds and never reads this one."] + [""] + _FLOOR_BLOCK_FIELDS
    return _join(header, lines)


def floor_block_list_from_text(text):
    out = bytearray()
    for number, tokens in content(text):
        where = f"line {number}"
        if tokens[0] in ("bytes", "words"):
            _raw(tokens[0], tokens[1:], out, 0, where)
        elif tokens[0] == "count":
            if out or len(tokens) != 2:
                raise ValueError(f"{where}: 'count N' goes first")
            out += _Field("count", 0, "H").pack(tokens[1], where)
        else:
            _expect(tokens[0], ("block",), where)
            due = len(out) - 2
            _record(tokens[1:], _FLOOR_BLOCK, out, where, lambda label: _check(label, due, where))
    return bytes(out)


def floor_block_to_text(data):
    """A floor block on its own (no_floor_block)."""
    lines = []
    if len(data) >= _FLOOR_BLOCK.size:
        lines.append("block  " + _FLOOR_BLOCK.text(data, 0))
    lines += _bytes_lines(data[_FLOOR_BLOCK.size if len(data) >= _FLOOR_BLOCK.size else 0:])
    header = ["The floor block find_floor (and get_floor_z_at_point, find_floor_block_at_point) gives an",
              "object when none of the stage's blocks holds it: no floor (height 2048)."] + [""] + _FLOOR_BLOCK_FIELDS
    return _join(header, lines)


def floor_block_from_text(text):
    out = bytearray()
    for number, tokens in content(text):
        where = f"line {number}"
        if tokens[0] in ("bytes", "words"):
            _raw(tokens[0], tokens[1:], out, 0, where)
        else:
            _expect(tokens[0], ("block",), where)
            _record(tokens[1:], _FLOOR_BLOCK, out, where)
    return bytes(out)


# ---- floor block bounds ----

_BOUNDS = _Layout(16, ("left_x", 0, "h"), ("left_y", 2, "h"), ("right_x", 4, "h"), ("right_y", 6, "h"),
                  ("top_x", 8, "h"), ("top_y", 10, "h"), ("bottom_x", 12, "h"), ("bottom_y", 14, "h"))


def floor_block_bounds_to_text(data):
    lines = []
    count = struct.unpack_from("<H", data, 0)[0] if len(data) >= 2 else None
    if count is not None:
        lines.append(f"count {count}")
    at = 2
    index = 0
    while at + _BOUNDS.size <= len(data):
        if index == count:
            lines.append("# past the count: not tested")
        lines.append(f"bounds 0x{index * _FLOOR_BLOCK.size:04X}  " + _BOUNDS.text(data, at))
        at += _BOUNDS.size
        index += 1
    lines += _bytes_lines(data[at:] if len(data) >= 2 else data)
    header = ["The outlines of the stage's floor blocks on the map screen (projected coordinates):",
              "find_nearby_floor_blocks tests 'count' of them every frame and lists the blocks whose outline",
              "reaches view_x - 0x80 .. + 0x1C0, view_y - 0x80 .. + 0x160 (left/right x and top/bottom y bound",
              "the box; then any corner within 0x240 x 0x1E0 of the view's corner counts). Record n belongs to",
              "the floor block at offset n x 22, its label."]
    return _join(header, lines)


def floor_block_bounds_from_text(text):
    out = bytearray()
    for number, tokens in content(text):
        where = f"line {number}"
        if tokens[0] in ("bytes", "words"):
            _raw(tokens[0], tokens[1:], out, 0, where)
        elif tokens[0] == "count":
            if out or len(tokens) != 2:
                raise ValueError(f"{where}: 'count N' goes first")
            out += _Field("count", 0, "H").pack(tokens[1], where)
        else:
            _expect(tokens[0], ("bounds",), where)
            due = (len(out) - 2) // _BOUNDS.size * _FLOOR_BLOCK.size
            _record(tokens[1:], _BOUNDS, out, where, lambda label: _check(label, due, where))
    return bytes(out)


# ---- tables cut across their records (the pieces start where the code reads a field) ----

def _phased_lines(data, layout, keyword, phase, digits=2):
    """Records of a table whose piece starts 'phase' bytes into the table: each line holds the fields of
    its record that lie in the piece."""
    lines = []
    position = -(phase % layout.size)
    while position < len(data):
        lo = max(0, -position)
        hi = min(layout.size, len(data) - position)
        index = (position + phase) // layout.size
        if layout.tiles(lo, hi):
            lines.append(f"{keyword} {index:0{digits}X}  " + layout.text(data, position, lo, hi))
        else:
            lines += _bytes_lines(data[max(position, 0):position + hi])
        position += layout.size
    return lines


def _phased_record(tokens, layout, out, phase, where):
    due = (len(out) + phase) // layout.size
    _record(tokens, layout, out, where, lambda label: _check(label, due, where), partial=True)


# floor_slopes: 8 bytes a slope (find_floor, the camera, the players' steering)
_SLOPE = _Layout(8, ("flags", 0, "H", "hex"), ("factor", 2, "i"),
                 ("directions", 6, "H", ("address", 0x10DFCC)))

_SLOPE_HEADER = [
    "floor_slopes: 8 bytes a slope type (a floor block's 'slope'; 00 = flat). flags bit 1: the floor rises",
    "along x (else along y); factor: how much the floor's z changes per unit along that axis (find_floor:",
    "(distance >> 12) x factor >> 4; the camera and compute_scroll_drift scale speeds by it); directions:",
    "the slope's (speed factor, direction) pairs in slope_speed_direction_pairs, by direction >> 6",
    "(player_steer, compute_ground_speed). The table is cut where the code reads its fields",
    "(floor_slopes, floor_slope_factors = +2, floor_slope_direction_tables = +6): each piece holds the",
    "fields from there on.",
]


def _slopes_format(phase):
    def to_text(data):
        return _join(_SLOPE_HEADER, _phased_lines(data, _SLOPE, "slope", phase))

    def from_text(text):
        out = bytearray()
        for number, tokens in content(text):
            where = f"line {number}"
            if tokens[0] in ("bytes", "words"):
                _raw(tokens[0], tokens[1:], out, 0, where)
            else:
                _expect(tokens[0], ("slope",), where)
                _phased_record(tokens[1:], _SLOPE, out, phase, where)
        return bytes(out)

    return to_text, from_text


# floor_types: 40 floor block types of 4 bytes, then the attribute records they point at
_FLOOR_TYPE = _Layout(4, ("floor_type", 0, "B", "hex"), ("unused", 1, "B", "hex"), ("attribute", 2, "H", "hex"))
_FLOOR_TYPE_COUNT = 40
_ATTRIBUTE_RECORDS = {
    "climb_wall": _Layout(16, ("axis", 0, "B", "hex"), ("unused_1", 1, "B", "hex"), ("unused_2", 2, "H", "hex"),
                          ("x_per_rise", 4, "h"), ("height", 6, "H"), ("top_z", 8, "h"), ("along_min", 10, "h"),
                          ("along_length", 12, "H"), ("rise", 14, "H")),
    "moving_floor": _Layout(6, ("push_x", 0, "h"), ("push_y", 2, "h"), ("ride", 4, "B", "hex"),
                            ("unused", 5, "B", "hex")),
    "push": _Layout(4, ("push_x", 0, "h"), ("push_y", 2, "h")),
}
# the attribute record each floor_type reads (the first one listed wins where kinds share a record)
_ATTRIBUTE_KINDS = {0x02: "climb_wall", 0x07: "moving_floor", 0x04: "push", 0x05: "push", 0x0B: "push"}

_FLOOR_TYPE_HEADER = [
    "floor_types: a floor block's 'type' indexes 4-byte records: floor_type (what find_floor gives the",
    "object: 1 run cycle by facing, 2 climbable wall (state 0A), 3 launcher (state 10), 4, 5, 0B and 7",
    "with an attribute push the player, 7 a moving floor, 8 the floor type 8 sequence (state 25)...) and",
    "attribute (floor_attribute). For floor types 2, 4, 5, 7 and 0B the attribute is a byte offset from",
    "floor_types to a record past the 40 types (or 0); for 1 it is the facing base (floor_1_facing_bases).",
    "The attribute records, labelled by that offset:",
    "  climb_wall (floor type 2): axis 0 = the wall runs along y (along_min, along_length bound the",
    "    player's y; pulling up puts it past x_max), else along x;",
    "    x_per_rise (s16, the code reads it as floor_attribute_slopes); height: the climb (start_climb's",
    "    climb_remaining, floor_attribute_wall_heights); top_z: z at the top (pulling up lands there);",
    "    along_min, along_length: the stretch of the wall the player climbs on; rise: z gained per unit",
    "    along the floor when the climb starts partway",
    "  push (4, 5, 0B): push_x, push_y added to the player's push every frame",
    "  moving_floor (7): the same push, and ride 01: the floor carries the player along x at",
    "    moving_floor_object's speed (apply_moving_floor; the stage 9 ice platforms)",
    "The table is cut where the code reads its fields (floor_types; floor_attribute_slopes = +4,",
    "floor_attribute_wall_heights = +6): each piece holds the fields from there on.",
]


def _attribute_lines(data, start, table, end):
    """The attribute records of floor_types from table offset 'start' (data[0] is at it) to 'end'; 'table'
    holds (type, floor_type or None when it lies in another piece, attribute) of the types at hand."""
    references = {}
    for index, floor_type, attribute in table:
        if start <= attribute < end:
            references.setdefault(attribute, []).append((index, floor_type))
    marks = sorted(set(references) | {start, end})
    lines = []
    position = start
    while position < end:
        next_mark = min(mark for mark in marks if mark > position)
        users = references.get(position, [])
        kinds = [_ATTRIBUTE_KINDS.get(floor_type) for _, floor_type in users]
        kind = next((name for name in _ATTRIBUTE_RECORDS if name in kinds), None)
        if kind is None and users and None in [floor_type for _, floor_type in users]:
            # a type whose floor_type lies in another piece: the record kind that fills the space exactly
            kind = next((name for name, layout in _ATTRIBUTE_RECORDS.items()
                         if layout.size == next_mark - position), None)
        layout = _ATTRIBUTE_RECORDS.get(kind)
        note = _note("types " + ", ".join(f"{index:02X}" + (f" (floor type {floor_type:X})" if floor_type is not None
                                                             else "") for index, floor_type in users)
                     if users else None)
        if layout and position + layout.size <= next_mark:
            lines.append(f"{kind} 0x{position:03X}  " + layout.text(data, position - start) + note)
            position += layout.size
            continue
        if note:
            lines.append(note.strip())
        chunk = data[position - start:next_mark - start]
        lines += _words_lines(chunk, position)
        position = next_mark
    return lines


def _floor_types_format(phase):
    def to_text(data):
        types_end = min(len(data), _FLOOR_TYPE_COUNT * 4 - phase)
        lines = _phased_lines(data[:max(types_end, 0)], _FLOOR_TYPE, "type", phase)
        table = []
        for index in range(_FLOOR_TYPE_COUNT):
            at = index * 4 - phase
            if at + 2 >= 0 and at + 4 <= len(data):
                table.append((index, data[at] if at >= 0 else None, struct.unpack_from("<H", data, at + 2)[0]))
        if len(data) > types_end >= 0:
            lines += _attribute_lines(data[types_end:], types_end + phase, table, len(data) + phase)
        return _join(_FLOOR_TYPE_HEADER, lines)

    def from_text(text):
        out = bytearray()
        for number, tokens in content(text):
            where = f"line {number}"
            if tokens[0] in ("bytes", "words"):
                _raw(tokens[0], tokens[1:], out, phase, where)
            elif tokens[0] == "type":
                _phased_record(tokens[1:], _FLOOR_TYPE, out, phase, where)
            else:
                _expect(tokens[0], tuple(_ATTRIBUTE_RECORDS), where)
                position = len(out) + phase
                _record(tokens[1:], _ATTRIBUTE_RECORDS[tokens[0]], out, where,
                        lambda label: _check(label, position, where, "position"))
        return bytes(out)

    return to_text, from_text


# ---- restart points ----

_RESTART_START = _Layout(6, ("spacing_shift", 0, "H"), ("x", 2, "h"), ("y", 4, "h"))
_RESTART_GROUND = _Layout(8, ("until", 0, "H", "hex"), ("kind", 2, "B"), ("pad", 3, "B", "hex"),
                          ("x", 4, "h"), ("y", 6, "h"))
_RESTART_SCREEN = _Layout(8, ("until", 0, "H", "hex"), ("kind", 2, "B"), ("screen_y", 3, "B"),
                          ("screen_x", 4, "h"), ("z", 6, "H"))
_RESTART_OTHER = _Layout(8, ("until", 0, "H", "hex"), ("kind", 2, "B"), ("value_3", 3, "B", "hex"),
                         ("value_4", 4, "H", "hex"), ("value_6", 6, "H", "hex"))


def _restart_layout(kind):
    return _RESTART_GROUND if kind in (0, 2) else _RESTART_SCREEN if kind == 1 else _RESTART_OTHER


def restart_points_to_text(data):
    lines = []
    if len(data) >= _RESTART_START.size:
        lines.append("start  " + _RESTART_START.text(data, 0))
        at = _RESTART_START.size
        while at + 8 <= len(data):
            kind = data[at + 2]
            layout = _restart_layout(kind)
            skip = ("pad",) if layout is _RESTART_GROUND and data[at + 3] == 0 else ()
            lines.append("point  " + layout.text(data, at, skip=skip))
            at += 8
        lines += _bytes_lines(data[at:])
    else:
        lines += _bytes_lines(data)
    header = [
        "A stage's restart points. 'start' is where the players enter the stage (place_player_at_restart_point",
        "at a stage start): x, y; player n at x + (n << spacing_shift). Then the points, sorted by scroll position:",
        "update_restart_point keeps the first one whose 'until' the scroll position has not reached (FFFF: to the",
        "end). kind 0 or 2: ground x, y (player n at x + 2n, player 2 at x - 2); kind 1: a point on the screen",
        "(screen_x, screen_y) at height z, turned into ground coordinates through the view (player n 2 apart).",
        "The stage's pointer (stage_restart_points) reaches the first point, 6 bytes in.",
    ]
    return _join(header, lines)


def restart_points_from_text(text):
    out = bytearray()
    for number, tokens in content(text):
        where = f"line {number}"
        if tokens[0] in ("bytes", "words"):
            _raw(tokens[0], tokens[1:], out, 0, where)
        elif tokens[0] == "start":
            out += _RESTART_START.pack(_pairs(tokens[1:], where), where)
        else:
            _expect(tokens[0], ("point",), where)
            pairs = _pairs(tokens[1:], where)
            kind = _number(pairs.get("kind", "0"), where)
            layout = _restart_layout(kind)
            if layout is _RESTART_GROUND:
                pairs.setdefault("pad", "0")
            out += layout.pack(pairs, where)
    return bytes(out)


# ---- goals ----

_GOAL = _Layout(12, ("at", 0, "H", "hex"), ("direction", 2, "H"), ("line", 4, "h"), ("exit_path", 6, "B", "hex"),
                ("unused", 7, "B", "hex"), ("line_min", 8, "h"), ("line_max", 10, "h"))
_GOAL_RECORDS = 0x1141C0
_GOALS = 0x11419A


def stage_goal_records_to_text(data):
    lines = []
    count = len(data) // _GOAL.size
    for index in range(count):
        offset = _GOAL_RECORDS - _GOALS + index * _GOAL.size
        lines.append(f"goal 0x{offset:02X}  " + _GOAL.text(data, index * _GOAL.size))
    lines += _bytes_lines(data[count * _GOAL.size:])
    header = [
        "The stages' goals (load_stage_goal), labelled by their offset from stage_goals (which holds, per stage,",
        "the offset of its record). Once scroll_position reaches 'at' (FFFF: no goal) a player who crosses the",
        "goal line walks the stage's exit path (state 21): direction 0: y <= line, 1: x >= line, 2: y >= line,",
        "3: x <= line; when line_min is not 0 the other coordinate must lie in line_min..line_max. exit_path is",
        "the path_request of that walk (player_path_offsets); load_stage_goal skips its word, the player reads",
        "it through stage_goal_exit_paths.",
    ]
    return _join(header, lines)


def stage_goal_records_from_text(text):
    out = bytearray()
    for number, tokens in content(text):
        where = f"line {number}"
        if tokens[0] in ("bytes", "words"):
            _raw(tokens[0], tokens[1:], out, 0, where)
        else:
            _expect(tokens[0], ("goal",), where)
            due = _GOAL_RECORDS - _GOALS + len(out)
            _record(tokens[1:], _GOAL, out, where, lambda label: _check(label, due, where))
    return bytes(out)


# ---- player paths ----

_PATH_RECORD = _Layout(8, ("speed", 0, "H", "hex"), ("x", 2, "h"), ("y", 4, "h"), ("flags", 6, "H", "hex"))
_PATH_WAIT = _Layout(8, ("speed", 0, "H", "hex"), ("frames", 2, "H"), ("y", 4, "h"), ("flags", 6, "H", "hex"))
_PLAYER_PATHS = 0x1155D4
_PLAYER_PATH_OFFSETS = 0x115528


def player_paths_to_text(data):
    lines = []
    at = 0
    open_path = False
    while at < len(data):
        first = not open_path
        if first:
            lines.append(f"path 0x{_PLAYER_PATHS + at:06X}")
            open_path = True
        speed = struct.unpack_from("<H", data, at)[0] if at + 2 <= len(data) else None
        if speed is not None and speed & 0x8000:
            lines.append("end" if speed == 0xFFFF else f"end 0x{speed:04X}")
            at += 2
            open_path = False
            continue
        if speed is None or at + 8 > len(data):
            lines += _bytes_lines(data[at:])
            break
        flags, y = struct.unpack_from("<H", data, at + 6)[0], struct.unpack_from("<h", data, at + 4)[0]
        if speed == 0:
            line = "wait  " + _PATH_WAIT.text(data, at, lo=2, hi=4)
            line += f"  y {y}" if y else ""
        else:
            line = "move  " + _PATH_RECORD.text(data, at, hi=6)
        line += f"  flags 0x{flags:04X}" if flags else ""
        lines.append(line + ("" if first else f"  # 0x{_PLAYER_PATHS + at:06X}"))
        at += 8
    header = [
        "The paths players walk on their own (follow_player_path): stage entries, goal exits, scripted walks.",
        "player_path_offsets gives, per path number + player, the address of the path's first record. A record",
        "is 'move' (walk at 'speed' toward x, y; flags bit 0: jump on the way (state 4), bit 1: go on to the",
        "next record without arriving) or 'wait' (stand for 'frames'); 'end' (a word with bit 15 set) stops",
        "the walk. A path without an 'end' runs on into the next one.",
    ]
    return _join(header, lines)


def player_paths_from_text(text):
    out = bytearray()
    for number, tokens in content(text):
        where = f"line {number}"
        keyword = tokens[0]
        if keyword in ("bytes", "words"):
            _raw(keyword, tokens[1:], out, 0, where)
        elif keyword == "path":
            if len(tokens) != 2:
                raise ValueError(f"{where}: 'path ADDRESS' expected")
            _check(_number(tokens[1], where), _PLAYER_PATHS + len(out), where, "address")
        elif keyword == "end":
            value = _number(tokens[1], where) if len(tokens) > 1 else 0xFFFF
            if not 0x8000 <= value <= 0xFFFF:
                raise ValueError(f"{where}: an end word has bit 15 set")
            out += struct.pack("<H", value)
        elif keyword == "move":
            pairs = _pairs(tokens[1:], where)
            pairs.setdefault("flags", "0")
            out += _PATH_RECORD.pack(pairs, where)
        else:
            _expect(keyword, ("wait",), where)
            pairs = _pairs(tokens[1:], where)
            pairs.update(speed="0")
            pairs.setdefault("y", "0")
            pairs.setdefault("flags", "0")
            out += _PATH_WAIT.pack(pairs, where)
    return bytes(out)


def _path_entry(index):
    """Entry n of player_path_offsets is path (2 + 3k) + player."""
    if index < 2:
        return "no path (path 00)"
    return f"path {2 + (index - 2) // 3 * 3:02X}, player {(index - 2) % 3}"


# ---- the ending's flight path ----

_POINT = _Layout(4, ("x", 0, "h"), ("y", 2, "h"))


def flight_path_to_text(data):
    lines = []
    count = len(data) // 4
    index = 0
    while index < count:
        run = 1
        while index + run < count and data[(index + run) * 4:(index + run) * 4 + 4] == data[index * 4:index * 4 + 4]:
            run += 1
        frames = f"frame {index}" if run == 1 else f"frames {index}-{index + run - 1}"
        lines.append("point  " + _POINT.text(data, index * 4) + (f"  repeat {run}" if run > 1 else "")
                     + f"  # {frames}")
        index += run
    lines += _bytes_lines(data[count * 4:])
    header = [
        "Eggman's flight in the ending (ending_eggman_fly_in): his screen position (x, y) frame by frame,",
        "from off the left edge to where he stops; 'repeat n' holds a point n frames. Once x reaches 166 the",
        "handler skips one more point a frame (it only skips equal points); the flight lasts 480 frames, so",
        "the zero points at the end are probably never reached.",
    ]
    return _join(header, lines)


def flight_path_from_text(text):
    out = bytearray()
    for number, tokens in content(text):
        where = f"line {number}"
        if tokens[0] in ("bytes", "words"):
            _raw(tokens[0], tokens[1:], out, 0, where)
            continue
        _expect(tokens[0], ("point",), where)
        pairs = _pairs(tokens[1:], where)
        repeat = _number(pairs.pop("repeat", "1"), where)
        if repeat < 1:
            raise ValueError(f"{where}: repeat below 1")
        out += _POINT.pack(pairs, where) * repeat
    return bytes(out)


# ---- small tables ----

def _tail_words_format(keyword, layout, count, header, first=0, digits=2, tail_note=None):
    """An indexed table of 'count' records, then words no code reads (kept as they are)."""
    to_table, table_from = _indexed_format(keyword, layout, header, first, digits)

    def to_text(data):
        size = min(len(data), count * layout.size)
        text = to_table(data[:size])
        rest = data[size:]
        if rest:
            text += (f"# {tail_note}\n" if tail_note else "") + "".join(line + "\n" for line in _words_lines(rest))
        return text

    return to_text, table_from


_STAGE_ENTRY = _Layout(2, ("path", 0, "H", "hex2"))
_VOICE_RANGE = _Layout(4, ("start", 0, "H", "hex"), ("length", 2, "H", "hex"))
_STATE = _Layout(1, ("state", 0, "B", "hex"))
_CARRY_ROW = _Layout(4, ("top", 0, "B", "hex"), ("right", 1, "B", "hex"), ("bottom", 2, "B", "hex"),
                     ("left", 3, "B", "hex"))
_SIDE = _Layout(1, ("side", 0, "B", "hex"))
_TEXT_POSITION = _Layout(2, ("position", 0, "H", "hex"))
_FACING = _Layout(1, ("facing", 0, "B", "hex"))


def _text_cell(data, at, index):
    value = struct.unpack_from("<H", data, at)[0]
    cell = (value - 0xF000) // 2
    return f"  # row {cell // 64}, column {cell % 64}" if value >= 0xF000 and value % 2 == 0 else ""


FORMATS = {
    "stage_spawn_pointers": (".txt",) + _pointer_format(
        ["stage_spawns_by_type: per stage (hex), the address of the stage's spawn_objects_by_type list."]),
    "stage_type_spawns": (".txt", type_spawns_to_text, type_spawns_from_text),
    "ending_flight_path": (".txt", flight_path_to_text, flight_path_from_text),
    "stage_floor_block_offsets": (".txt",) + _pointer_format(
        ["stage_floor_blocks: per stage, the stage's floor blocks (load_stage_map_pointers sets floor_blocks;",
         "continue_to_next_stage_slot also reads the next stage's). Stored as offsets from stage_floor_blocks."],
        base=0x10B6CC),
    "floor_block_list": (".txt", floor_block_list_to_text, floor_block_list_from_text),
    "floor_block": (".txt", floor_block_to_text, floor_block_from_text),
    "floor_slopes": (".txt",) + _slopes_format(0),
    "floor_slopes_from_factor": (".txt",) + _slopes_format(2),
    "floor_slopes_from_directions": (".txt",) + _slopes_format(6),
    "floor_types": (".txt",) + _floor_types_format(0),
    "floor_types_from_4": (".txt",) + _floor_types_format(4),
    "floor_types_from_6": (".txt",) + _floor_types_format(6),
    "stage_floor_block_bounds_offsets": (".txt",) + _pointer_format(
        ["stage_floor_block_bounds: per stage, the outlines of its floor blocks (floor_block_bounds).",
         "Stored as offsets from stage_floor_block_bounds."], base=0x10E29C),
    "floor_block_bounds": (".txt", floor_block_bounds_to_text, floor_block_bounds_from_text),
    "floor_type_player_states": (".txt",) + _indexed_format("floor_type", _STATE, [
        "floor_type_player_states: by floor_type, the player state the floor forces (player_move; 00 none):",
        "02 climbable wall -> 0A (climbing), 03 -> 10 (launched), 08 -> 25 (the floor type 8 sequence)."]),
    "screen_edge_scroll_carry_flags": (".txt",) + _tail_words_format("row", _CARRY_ROW, 8, [
        "screen_edge_scroll_carry_flags (keep_player_on_screen): by row and screen edge, FF = when that edge",
        "pushes a player back, the scroll also carries the player along. row = camera_scroll_direction >> 8",
        "(the direction in quarters: 0-3), + 4 when the direction lies between two quarters (its low byte",
        "is not 0)."], digits=1, tail_note="after the table: words no code found reads"),
    "floor_block_side_codes": (".txt",) + _tail_words_format("outcode", _SIDE, 11, [
        "floor_block_side_codes: by outcode (where a point lies around a floor block: 1 past x max, 2 before",
        "x min, 4 before y min, 8 past y max), the side: 0 y min side (or inside), 1 x max, 2 y max, 3 x min",
        "(get_floor_block_side), and 4-7 the corners (get_floor_block_side_or_corner, unused)."],
        tail_note="after the table: 16 words (angles 0-3FF by their values) no code found reads"),
    "hud_stage_rings_positions": (".txt",) + _indexed_format("player", _TEXT_POSITION, [
        "hud_stage_rings_positions: by player, the text layer cell where draw_player_stage_rings (unused)",
        "writes stage_rings in hex (0x310000 + position; the layer is 64 cells wide from 0xF000)."],
        digits=1, comment=_text_cell),
    "floor_1_facing_bases": (".txt",) + _indexed_format("attribute", _FACING, [
        "floor_1_facing_bases: on floor type 1, by floor_attribute (1-4), the first of the five facings",
        "(base .. base + 4) that switch player_animate_run_cycle to the alternate run animations; entry 0",
        "is not read."],
        digits=1),
    "stage_player_entries": (".txt",) + _indexed_format("stage", _STAGE_ENTRY, [
        "stage_player_entries: per stage, the path a player walks in on when it joins or respawns",
        "(check_stage_entry_path: state 2A, path_request = this; 0 = none). Paths: player_path_offsets."]),
    "stage_goal_offsets": (".txt",) + _pointer_format(
        ["stage_goals: per stage, its goal record (stage_goal_records; load_stage_goal). Stored as offsets",
         "from stage_goals; the table goes on in stage_goal_exit_paths, which starts at stage 03."],
        base=_GOALS),
    "stage_goal_offsets_from_3": (".txt",) + _pointer_format(
        ["stage_goals, stages 03 on: the code reads this place as stage_goal_exit_paths (the goal record's",
         "byte +6, read through stage_goals[stage]), which is why it is cut here."], base=_GOALS, first=3),
    "stage_goal_records": (".txt", stage_goal_records_to_text, stage_goal_records_from_text),
    "edge_voice_scroll_ranges": (".txt",) + _indexed_format("stage", _VOICE_RANGE, [
        "edge_voice_scroll_ranges: per stage, the scroll positions start .. start + length where",
        "keep_player_on_screen plays the character's voice (39/43/4C) once when a screen edge pushes a",
        "player and two or more play (length 0: never)."]),
    "stage_restart_point_offsets": (".txt",) + _pointer_format(
        ["stage_restart_points: per stage, its restart points (current_restart_point): the first point, 6",
         "bytes into the stage's list (the 6 before are its start). Stored as offsets from",
         "stage_restart_points."], base=0x114B84, back=6),
    "restart_points": (".txt", restart_points_to_text, restart_points_from_text),
    "player_path_offsets": (".txt",) + _pointer_format(
        ["player_path_offsets: by path number + player (follow_player_path), the path's first record",
         "(player_paths). Stored as offsets from player_path_offsets; 'none' = 0 (path 00 is no path)."],
        base=_PLAYER_PATH_OFFSETS, keyword="entry", none_at_zero=True, comment=_path_entry),
    "player_paths": (".txt", player_paths_to_text, player_paths_from_text),
}


# ======== the per-stage tables of the code chip at 000000 ========

# u16 offsets from the table (its own address) to each stage's list: (format name, the table's address,
# what it holds)
_STAGE_OFFSET_TABLES = (
    ("stage_event_offsets", 0x002492, "stage_events: per stage, its list of events (run_stage_events)."),
    ("stage_link_point_offsets", 0x002520,
     "stage_link_points: per stage, its link point (continue_to_next_stage_slot); 0 points at the table itself."),
    ("stage_layer_blending_offsets", 0x002694,
     "stage_layer_blending: per stage, its blending list (apply_stage_layer_blending)."),
    ("stage_name_banner_offsets", 0x0026CF,
     "stage_name_banners: per stage, its name banner (position and animation); the stages with none show the "
     "CLEARED banner."),
    ("stage_block_map_offsets", 0x017C5C,
     "stage_block_maps: per stage, its map of tile sets by block (stage_block_map_widths blocks a row)."),
    ("stage_section_direction_x_offsets", 0x01884A,
     "stage_section_directions_x: per stage, the x of the scroll direction per section (s16 list)."),
    ("stage_section_direction_y_offsets", 0x018870,
     "stage_section_directions_y: per stage, the y of the scroll direction per section (s16 list)."),
    ("stage_section_limit_x_offsets", 0x018DFA,
     "stage_section_limits_x: per stage, the camera x limit per section (list from its second word)."),
    ("stage_section_limit_y_offsets", 0x018E20,
     "stage_section_limits_y: per stage, the camera y limit per section (list from its second word)."),
    ("stage_section_end_offsets", 0x0193E0,
     "stage_section_ends: per stage, the scroll position where each section ends (from its second word; FFFF "
     "ends)."),
    ("stage_initial_tile_set_offsets", 0x019712,
     "stage_initial_tile_sets: per stage, the four tile sets init_stage_view loads (entry 13 is past the "
     "stages)."),
    ("stage_scroll_mode_offsets", 0x0197F2,
     "stage_scroll_modes: per stage, the (scroll mode, duration) pairs, one per section."),
    ("stage_fast_scroll_range_offsets", 0x01A128,
     "stage_fast_scroll_ranges: per stage, the scroll positions of update_background_scroll (FFFF ends; entry 13 "
     "is past the stages)."),
    ("stage_section_speed_offsets", 0x01A162,
     "stage_section_speeds: per stage, scroll_speed then one per section (u8)."),
    ("stage_section_follow_speed_offsets", 0x01A565,
     "stage_section_follow_speeds: per stage, follow_base_speed then one per section (u8)."),
    ("stage_template_spawn_offsets", 0x01F206,
     "stage_template_spawns: per stage, its ring rows (spawn_ring_rows)."),
)
for _table, _base, _header in _STAGE_OFFSET_TABLES:
    FORMATS[_table] = (".txt",) + _pointer_format(
        [_header, f"Stored as u16 offsets from 0x{_base:06X} (the table's address)."], base=_base)
FORMATS["stage_pointers"] = (".txt",) + _pointer_format(
    ["Per stage (hex), a program address (u32), with the name of the place it reaches."])


def _stage_table(header, *fields, count=None):
    """A table of one record a stage, labelled by the stage: the fields (name, struct code[, style])."""
    return (".txt",) + labelled_table(run_of("stage", *fields), header, count=count)


FORMATS.update({
    "stage_cleared_values": _stage_table(
        ["stage_select_cleared_values: per stage, what game mode 8 (stage select) writes to cleared_stages,",
         "so that stage_order gives the stage back."], ("cleared", "H")),
    "sound_by_stage": _stage_table(
        ["A sound id by stage (sound_commands' ids): stage_start_sounds, the one start_stage (and game mode 1)",
         "requests; stage_map_sounds_1-3, the ones map_eggman_update queues on the map screen."], ("sound", "B")),
    "stage_block_map_widths": _stage_table(
        ["stage_block_map_widths: per stage, the blocks a row of its block map (stage_block_maps)."],
        ("width", "B", "d")),
    "stage_scroll_ends": _stage_table(
        ["stage_scroll_ends: per stage, scroll_end."], ("scroll_end", "H")),
    "stage_camera_origins_x": _stage_table(
        ["stage_camera_origins_x: per stage, camera_origin_x (the camera's x at the stage start)."], ("x", "h")),
    "stage_camera_origins_y": _stage_table(
        ["stage_camera_origins_y: per stage, camera_origin_y."], ("y", "h")),
    "stage_view_offsets_x": _stage_table(
        ["stage_view_offsets_x: per stage, view_offset_x."], ("offset", "H")),
    "stage_view_offsets_y": _stage_table(
        ["stage_view_offsets_y: per stage, view_offset_y."], ("offset", "H")),
    "stage_last_points": _stage_table(
        ["scroll_skip_last_points: per stage, the highest POINT (stage_start_section) scroll skip mode lets",
         "you pick."], ("last_point", "H")),
    "stage_tile_bank_bits": _stage_table(
        ["stage_tile_bank_bits: per stage (load_palette_set_and_backdrop): bit 10 of 31FF00 (the tile bank);",
         "entry 13 is past the stages."], ("bank", "B")),
    "stage_layer_priorities": _stage_table(
        ["stage_layer_priorities: per stage, the priority of NBG0-NBG3 (set_stage_layer_mixing: mixer",
         "610022-610028 = this + 0x200)."], ("nbg0", "B"), ("nbg1", "B"), ("nbg2", "B"), ("nbg3", "B")),
    "stage_layer_disables": _stage_table(
        ["stage_layer_disables: per stage, the bits ORed into the layer disable register (31FF02)."],
        ("disable", "B")),
    "stage_nbg3_widths": _stage_table(
        ["stage_nbg3_widths: per stage, the blocks a row of its NBG3 page map (stage_nbg3_block_maps)."],
        ("width", "B", "d")),
    "stage_rowscroll_settings": _stage_table(
        ["stage_rowscroll_settings: per stage, what set_stage_layer_mixing writes to the rowscroll control",
         "(31FF04)."], ("rowscroll", "H"), count=19),
})


# ---- block maps ----

def _block_map(width):
    """Rows of 'width' bytes (hex): a map with one byte a block, read row * width + column."""

    def to_text(data):
        lines = [f"# one byte a block (hex), {width} blocks a row; the label is the row (decimal)"]
        for row, at in enumerate(range(0, len(data), width)):
            lines.append(f"row {row:3d}: " + " ".join(f"{byte:02X}" for byte in data[at:at + width]))
        return "".join(line + "\n" for line in lines)

    def from_text(text):
        out = bytearray()
        for line_number, tokens in content(text):
            where = f"line {line_number}"
            found = raw(tokens, where)
            if found is not None:
                out += found
                continue
            if tokens[0] != "row" or len(tokens) < 3 or not tokens[1].endswith(":"):
                raise ValueError(f"{where}: 'row N: XX XX ...' expected")
            try:
                out += bytes(int(token, 16) for token in tokens[2:])
            except ValueError:
                raise ValueError(f"{where}: a row holds something that is not a hex byte") from None
        return bytes(out)

    return (".txt", to_text, from_text)


for _width in (3, 6, 8, 9, 16):
    FORMATS[f"block_map_{_width}"] = _block_map(_width)


# ---- layer blending lists (apply_stage_layer_blending) ----

_BLEND = run_of("blend", ("mask", "B"), ("bit", "B", "d"), ("shadow", "H", ("off", WORK_RAM)), ("factor", "H"))


def layer_blending_to_text(data):
    lines = []
    if data:
        count = data[0]
        lines.append(f"count {count}")
        at = 1
        while at + _BLEND.size <= len(data):
            lines.append(labelled_line(_BLEND, data, at))
            at += _BLEND.size
        lines += hex_lines(data[at:], KEPT)
    header = ["apply_stage_layer_blending: a count (0: none), then per layer: the sprite blend mask written into",
              "the low 6 bits of the layer's mixer shadow word ('shadow', a work RAM address), the bit set in",
              "it (12: blend with the sprites), and the blend factor ORed into mixer_blend (61004E) with bit 11."]
    return join(header, lines)


def layer_blending_from_text(text):
    out = bytearray()
    for line_number, tokens in content(text):
        where = f"line {line_number}"
        found = raw(tokens, where)
        if found is not None:
            out += found
        elif tokens[0] == "count" and len(tokens) == 2:
            out.append(_number(tokens[1], where) & 0xFF)
        elif tokens[0] == "blend":
            out += _BLEND.pack(tokens[1:], where)
        else:
            raise ValueError(f"{where}: '{tokens[0]}' is not a line this file takes")
    return bytes(out)


FORMATS["layer_blending"] = (".txt", layer_blending_to_text, layer_blending_from_text)


# ---- stage_object_templates (spawn_ring_rows, spawn_stage_template_0) ----

_TEMPLATE = run_of("template", ("sprite_flags", "H"), ("sprite_frame", "H", "frame"), ("sprite_palette", "H"),
                   ("hitbox_size_x", "H", "d"), ("hover_height", "H", "d"), ("animation_speed", "B", "d"),
                   ("animation_timer", "B", "d"), ("animation_frame", "B", "d"), ("kind", "B"),
                   ("animation", "I", "ptr"), ("handler", "I", "ptr"), ("sprite_offset_x", "b"),
                   ("sprite_offset_y", "b"), ("hitbox_height", "H", "d"), phased=True)
_TEMPLATE_HEADER = [
    "stage_object_templates: 26-byte records, the fields an object spawned from them starts with",
    "(spawn_ring_rows: the record a ring row's 'template' names; spawn_stage_template_0 reads record 0",
    "field by field). hover_height: the object floats this high over its floor (field 82). The table is",
    "cut where symbols.txt names record 0's fields: each piece holds the fields from there on.",
]


def _templates(phase):
    """stage_object_templates from 'phase' bytes into its first record: each line holds the fields of its
    record that lie in the piece."""

    def to_text(data):
        lines = []
        position = -phase
        while position < len(data):
            lo = max(0, -position)
            hi = min(_TEMPLATE.size, len(data) - position)
            index = (position + phase) // _TEMPLATE.size
            if _TEMPLATE.tiles(lo, hi):
                lines.append(labelled_line(_TEMPLATE, data, position, f"{index:02X}", lo, hi))
            else:
                lines += hex_lines(data[max(position, 0):position + hi], KEPT)
            position += _TEMPLATE.size
        return join(_TEMPLATE_HEADER, lines)

    def from_text(text):
        out = bytearray()
        for line_number, tokens in content(text):
            where = f"line {line_number}"
            found = raw(tokens, where)
            if found is not None:
                out += found
            elif tokens[0] == "template":
                out += _TEMPLATE.pack(record_tokens(tokens), where)
            else:
                raise ValueError(f"{where}: '{tokens[0]}' is not a line this file takes")
        return bytes(out)

    return (".txt", to_text, from_text)


FORMATS.update(phased_formats("object_templates", _TEMPLATE, _templates))


# ---- the lists the stage readers walk, one record a line ----

_EVENT = run_of("event", ("at", "H"), ("game_mode", "B"), ("unused", "B"))
_LINK = run_of("link", ("from_block", "H"), ("to_block", "H"))
_RING_ROW = run_of("row", ("at", "H"), ("template", "H", ("index", _TEMPLATE.size)), ("count", "H", "d"),
                   ("x", "h"), ("y", "h"), ("dx", "b"), ("dy", "b"))

FORMATS.update({
    "stage_events": (".txt",) + labelled_list(_EVENT, [
        "A stage's events (run_stage_events): once scroll_position reaches 'at', game_mode becomes 'game_mode'",
        "(when the scroll is less than 0x100 past it, or on stage 0x11); 'unused' is not read. 'end' (FFFF)."],
        b"\xFF\xFF"),
    "scroll_modes": (".txt",) + labelled_table(run_of("section", ("mode", "H"), ("duration", "H", "d")), [
        "A stage's scroll modes (init_stage_view's section setup, load_camera_start_section): per section the",
        "scroll_mode and how many frames it lasts (scroll_mode_timer, scroll_mode_duration)."]),
    "link_point": (".txt",) + labelled_table(_LINK, [
        "continue_to_next_stage_slot: the floor block (its offset in this stage's floor_blocks) the players",
        "leave from, and the one of the next stage's floor blocks they arrive on."]),
    "ring_rows": (".txt",) + labelled_list(_RING_ROW, [
        "A stage's ring rows (spawn_ring_rows): once scroll_position reaches 'at', 'count' objects of the",
        "stage_object_templates record 'template' (stored as its byte offset, 26 a record) from x, y in steps of",
        "dx, dy (each with a shadow); the row waits while fewer free objects than 'count' are left. 'end' (FFFF)."],
        b"\xFF\xFF"),
})


# ======== the ending: the timed lists of its objects, next to ending_flight_path ========

def _timed_spawns(base, spawner):
    """The timed spawns 'spawner' reads: the handlers are u16 offsets from 'base'."""
    record = run_of("spawn", ("at", "H", "d"), ("handler", "H", ("off", base)))
    return (".txt",) + labelled_list(record, [
        f"Timed spawns ({spawner}): each record starts an object in",
        "'handler' once the spawner's frame count reaches 'at' (frames); the handler is stored as a",
        f"u16 offset from 0x{base:06X}. 'end' (FFFF FFFF) ends the list."], b"\xFF" * 4)


_UNTIL_FRAME = run_of("show", ("until", "H"), ("sprite", "H", "frame"))
_UNTIL_PALETTE = run_of("show", ("until", "H"), ("palette", "H"))
_UNTIL_HEADER = ["A timed list of the ending: each record holds while the object's timer is below 'until' (then the",
                 "next one); 'end' (FFFF FFFF) removes the object."]

FORMATS.update({
    "timed_spawns_ending_tower": _timed_spawns(0x01BFC4, "run_timed_spawns_2: the ending's tower"),
    "timed_spawns_ending_credits": _timed_spawns(0x01C82F, "run_timed_spawns_3: the credits"),
    "timed_spawns_title": _timed_spawns(0x0973B4, "timed_spawner_run: the title"),
    "ending_until_sprite": (".txt",) + labelled_list(_UNTIL_FRAME, _UNTIL_HEADER, b"\xFF" * 4),
    "ending_until_palette": (".txt",) + labelled_list(_UNTIL_PALETTE, _UNTIL_HEADER, b"\xFF" * 4),
    "ending_tower_frames": (".txt",) + labelled_list(
        run_of("show", ("until", "H"), ("sprite", "H", "frame"), ("palette", "H")), [
            "ending_tower_animate: each record holds while the timer is below 'until': the sprite and its palette;",
            "'end' (FFFF FFFF FFFF) removes the object."], b"\xFF" * 6),
    "ending_y_sprite": (".txt",) + labelled_list(run_of("step", ("screen_y", "H", "d"), ("sprite", "H", "frame")), [
        "ending_dust_cloud_rise: one step a frame: the screen y and the sprite; 'end' (FFFF FFFF)."], b"\xFF" * 4),
    "ending_music_cues": (".txt",) + labelled_list(run_of("cue", ("at", "H"), ("sound", "H")), [
        "ending_music_play: once the timer reaches 'at', queue_sound of 'sound' (sound_commands id);",
        "'end' (FFFF FFFF) removes the object."], b"\xFF" * 4),
    "ending_debris_path": (".txt",) + labelled_list(
        run_of("step", ("until", "H"), ("x", "H", "d"), ("y", "H", "d"), ("sprite", "H", "frame")), [
            "animate_ending_debris: each record holds while the timer is below 'until': screen x, y and sprite;",
            "'end' (FFFF, then the rest of the record) removes the object."], b"\xFF" * 6),
    "ending_sprite_flags": (".txt",) + labelled_list(run_of("step", ("sprite", "H", "frame"), ("flags", "H")), [
        "One step a frame: the sprite and its sprite_flags; 'end' (FFFF) stops the list."], b"\xFF\xFF"),
    "credits_parts": (".txt",) + labelled_list(
        run_of("part", ("sprite", "H", "frame"), ("screen_y", "H", "d"), ("palette", "H"), ("link_field", "H")), [
            "The parts of a credits page (ending_credits_*_scroll): a pool 2 object each, with this sprite, screen",
            "y and palette; 'link_field' is the offset of the field of the page's object that keeps the part.",
            "'end' (FFFF, then the rest of a record) ends the list."], b"\xFF" * 6),
})


# ======== stage objects of the code chip at 080000: the handler spawn lists and the spiked roller ========

ROCK_DEBRIS_ANIMATIONS = 0x09567E
SPIKED_ROLLER_LAYOUTS = 0x08219C

_HANDLER_SPAWN = Rec("spawn", 12, [("at", 0, "H", "x"), ("x", 2, "h"), ("y", 4, "h"), ("arg", 6, "H", "x"),
                                   ("handler", 8, "I", "ptr")])


def _read_handler_spawn(out):
    return out.rec(END) or out.rec(_HANDLER_SPAWN)


_handler_spawns_text, _STAGE_LIST = offset_lists_text(
    ["stage_spawns_by_handler (spawn_objects_by_handler): per stage, the offset of its list from the start of",
     "this table; a list starts an object in 'handler' once scroll_position reaches 'at', at x, y (its field",
     "04, sprite_frame, holds 'arg': a kind or a parameter the handler reads); 'end' (FFFF) closes a list."],
    19, "stage", "stage", _read_handler_spawn)

# spiked_roller_kinds: a kind's record, then its chase script
_ROLLER_KIND = Rec("roller", 7, [("parts", 0, "B"), ("layout", 1, "H", ("off", SPIKED_ROLLER_LAYOUTS)),
                                 ("state", 3, "H", "x"), ("back_zone", 5, "H")])
_CHASE = Rec("chase", 10, [("at", 0, "H", "x"), ("threshold", 2, "h"), ("factor", 4, "h"), ("speed", 6, "h"),
                           ("wait", 8, "H")])
_CHASE_END = Rec("stop", 6, [("at", 0, "H", "x"), ("threshold", 2, "H", "x"), ("factor", 4, "h")])
_CHASE_HEADER = [
    "A chase script (spiked_roller_chase, group_leader_move, sand_block_leader_move, waterfall_move): once",
    "scroll_position reaches 'at' the entry sets the screen threshold, the catch-up factor, the base speed",
    "(1/16 pixel a frame) and 'wait' (frames before the next entry is read); on screen past the threshold the",
    "object moves at the base speed, behind it at max(camera speed x factor >> 8, speed). 'stop' (a negative",
    "factor; its threshold word is not read) ends the script: from 'at' on the object stops (the roller goes",
    "once it is off screen).",
]


def _read_chase(out):
    """A chase entry, or the stop that ends a script (a negative factor at +4)."""
    factor = s16_at(out.data, out.at + 4)
    if factor is not None and factor < 0:
        return out.rec(_CHASE_END)
    return out.rec(_CHASE)


def _read_roller_kind(out):
    if not out.rec(_ROLLER_KIND):
        return False
    while out.at < len(out.data):
        stop = s16_at(out.data, out.at + 4)
        if not _read_chase(out):
            return False
        if stop is not None and stop < 0:
            break
    return True


_roller_kinds_text, _KIND_LIST = offset_lists_text(
    ["spiked_roller_kinds (spiked_roller_start): per kind (field 04) the offset of its record: 'roller' (how",
     "many parts, the layout in spiked_roller_layouts, the leader's first state, the back zone: a record of",
     "object_steps), then the leader's chase script."] + [""] + _CHASE_HEADER,
    6, "kind", "kind", _read_roller_kind)

_ROLLER_LAYOUT = Rec("layout", 16, [("dx", 0, "b"), ("dy", 1, "b"), ("sprite_flags", 2, "H", "x"),
                                    ("box_x", 4, "H"), ("box_y", 6, "H"), ("direction", 8, "H", "x"),
                                    ("flags", 10, "I", "x"), ("box_offset_x", 14, "B", "x"),
                                    ("box_offset_y", 15, "B", "x")])

_ROLLER_EVENT = Rec("event", 8, [("at", 0, "H", "x"), ("test", 2, "H"), ("coordinate", 4, "h"),
                                 ("state", 6, "H", "x")])


def _read_roller_event(out):
    return out.rec(END) or out.rec(_ROLLER_EVENT)


_roller_events_text, _EVENT_LIST = offset_lists_text(
    ["spiked_roller_events (spiked_roller_chase): per kind (field 72 = the offset) the leader's events: once",
     "scroll_position reaches 'at' and the test holds (0 and 4: y <= coordinate, 1: x >= coordinate, 2: y >=",
     "coordinate, 3: x <= coordinate) the leader goes to 'state' (field 80) and the next event; 'end' (FFFF)",
     "is never reached."],
    6, "kind", "kind", _read_roller_event)


def _flying_piece(keyword, look, base):
    """A piece that flies off (rock_debris_fly): sprite flags, dx, dy, speed x and y, and 'look', an offset
    from 'base' (its animation)."""
    return Rec(keyword, 12, [("sprite_flags", 0, "H", "x"), ("dx", 2, "h"), ("dy", 4, "h"), ("speed_x", 6, "h"),
                             ("speed_y", 8, "h"), (look, 10, "H", ("off", base))])


_DEBRIS_HEADER = [
    "Pieces that fly off (rock_debris_fly): sprite flags (1000 drawn as is; 0000 grows toward the viewer),",
    "dx, dy from the screen position of what breaks, speed x and y (12.4 a frame; gravity pulls y down), the",
    "animation (an offset into rock_debris_animations).",
]

_SPEED_REC = Rec("speed", 1, [("frames", 0, "B")])
_DELAY_REC = Rec("delay", 1, [("frames", 0, "B")])


def _speeds_text(data):
    out = Out(data, ["spiked_roller_animation_speeds: by min(chaser_speed >> 4, 5), the leader's animation_speed",
                     "(frames a step: faster turning at higher speed)."])
    for level in range(6):
        out.rec(_SPEED_REC, f"chaser_speed >> 4 = {level}")
    out.comment("no code found reads these (x, y) pairs (an arc, symmetric about x 0); the last one is")
    out.comment("spiked_roller_event_coords")
    while out.rec(POINT):
        pass
    return out.done()


def _event_coords_text(data):
    out = Out(data, ["spiked_roller_event_coords: spiked_roller_events - 4, so that + field 72 reaches the",
                     "coordinate of the event that fired; at offset 0 (never) it would read this pair, the last of",
                     "the arc before it."])
    out.rec(POINT)
    return out.done()


def _rumble_text(data):
    out = Out(data, ["spiked_roller_rumble_delays: by chaser_speed >> 4 (0-4), the frames between rumble sounds (B)."])
    for level in range(5):
        out.rec(_DELAY_REC, f"chaser_speed >> 4 = {level}")
    return out.done()


# ---- catapults (stage 4), the floor blocks of stage 3 ----

_PAIR_TEMPLATE = Rec("template", 26, [
    ("handler", 0, "I", "ptr"), ("animation", 4, "I", "ptr"), ("offset_x", 8, "b"), ("offset_y", 9, "b"),
    ("launch_kind", 10, "B"), ("unused", 11, "B", "x"), ("part_box_x", 12, "H"), ("part_box_y", 14, "H"),
    ("part_box_offset_x", 16, "b"), ("part_box_offset_y", 17, "b"), ("part_handler", 18, "I", "ptr"),
    ("part_animation", 22, "I", "ptr")])
_LAUNCH_DELAY = Rec("kind", 2, [("delay", 0, "H")])

# stage_03_floor_block_kinds: per kind the offset of its record and the fall flag, then the records
_FLOOR_BLOCK_KIND = Rec("kind", 4, [("record", 0, "H", "rel"), ("falls_with", 2, "H")])
_FLOOR_BLOCK_LOOK = Rec("block", 4, [("offset_x", 0, "b"), ("offset_y", 1, "b"), ("depth", 2, "H")])


def _floor_block_kinds_text(data):
    out = Out(data, ["stage_03_floor_block_kinds (stage_03_floor_block_start): per kind (field 04) the offset of",
                     "its record from this table and which stage_03_floor_block_falls byte it waits for; a record",
                     "is the sprite offsets (64/65), the depth (field 38) and the crumble animation (played once",
                     "when the block falls)."])
    while out.at + 4 <= len(data) and out.at < 0x1C:
        out.rec(_FLOOR_BLOCK_KIND, f"kind {out.at // 4}")
    while out.at < len(data):
        out.blank()
        out.comment(f"+{out.at:04X}")
        if not (out.rec(_FLOOR_BLOCK_LOOK) and frames_line(out)):
            break
    return out.done()


FORMATS.update({
    "handler_spawns": fmt(_handler_spawns_text, _STAGE_LIST, _HANDLER_SPAWN, END),
    "spiked_roller_kinds": fmt(_roller_kinds_text, _KIND_LIST, _ROLLER_KIND, _CHASE, _CHASE_END),
    "spiked_roller_layouts": indexed(
        ["spiked_roller_layouts (spiked_roller_start, at the kind's layout offset): dx, dy between one part and",
         "the next (signed bytes, << 16), then the record every part gets: sprite_flags, hitbox sizes, direction",
         "(field 94, << 8 into 24), flags (field 30: 40000002; 80 = the animation runs backwards), hitbox",
         "offsets."], _ROLLER_LAYOUT, lambda index: f"+{index * 16:02X}"),
    "spiked_roller_events": fmt(_roller_events_text, _EVENT_LIST, _ROLLER_EVENT, END),
    "spiked_roller_event_coords": fmt(_event_coords_text, POINT),
    "spiked_roller_animation_speeds": fmt(_speeds_text, _SPEED_REC, POINT),
    "spiked_roller_rumble_delays": fmt(_rumble_text, _DELAY_REC),
    "rock_debris_pieces": indexed(_DEBRIS_HEADER, _flying_piece("piece", "animation", ROCK_DEBRIS_ANIMATIONS),
                                  "piece"),
    "object_pair_templates": indexed(
        ["object_pair_templates (catapult_wait_for_platform_setup): by the catapult's field 34 (0 x, 1A y): its",
         "handler, animation (catapult_x/y_animation), sprite offsets 64/65 and launch kind (field 36); then the",
         "second object it makes, the catapult's arm: hitbox sizes and offsets, handler and animation."],
        _PAIR_TEMPLATE, lambda index: f"+{index * 26:02X}"),
    "catapult_launch_delays": indexed(
        ["catapult_launch_delays: by field 04 (the kind), frames before the catapult launches (0: none)."],
        _LAUNCH_DELAY, "kind"),
    "stage_03_floor_block_kinds": fmt(_floor_block_kinds_text, _FLOOR_BLOCK_KIND, _FLOOR_BLOCK_LOOK,
                                      specials={"frames": frames_special}),
})


# ======== stage objects of stages 3-7 (fireballs, doors, signs, the opening machine, how to play) ========

FIREBALL_TYPES = 0x0832B9
FIREBALL_ANIMATIONS = 0x083329       # the base of the types' animation offsets (a blank animation is there)
SIGN_PALETTE_LISTS = 0x08514D        # the base of the signs' palette list offsets

_EMITTER = Rec("emitter", 6, [("x", 0, "h"), ("y", 2, "h"), ("floor", 4, "h")])
_SHOT = Rec("shot", 12, [("at", 0, "H", "x"), ("x", 2, "h"), ("dy", 4, "h"), ("speed_y", 6, "h"), ("speed_z", 8, "h"),
                         ("type", 10, "H", ("off", FIREBALL_TYPES))])
_SHOT_END = Rec("stop", 12, [("at", 0, "H", "x"), ("x", 2, "h"), ("dy", 4, "h"), ("speed_y", 6, "h"),
                             ("speed_z", 8, "h"), ("type", 10, "h")])


def _read_emitter(out):
    if not out.rec(_EMITTER):
        return False
    while out.at < len(out.data):
        stop = s16_at(out.data, out.at + 10)
        if stop is None:
            return False
        if not out.rec(_SHOT_END if stop < 0 else _SHOT):
            return False
        if stop < 0:
            break
    return True


_emitter_lists_text, _EMITTER_LIST = offset_lists_text(
    ["fireball_emitter_lists (fireball_emitter_start): by field 04 (only 0 is spawned) the offset of the",
     "emitter: its x, y and floor height, then its shots: once scroll_position reaches 'at' a bouncing fireball",
     "starts at x, the emitter's y + dy, with speed_y and speed_z (<< 8) and a type (fireball_types); the",
     "emitter goes on to the next shot, and is removed when the next one's type is negative ('stop': its other",
     "words are not read)."],
    1, "kind", "kind", _read_emitter)

_FIREBALL_TYPE = Rec("type", 16, [
    ("sprite_flags", 0, "H", "x"), ("palette", 2, "H", "x"), ("scale", 4, "H", "x"), ("height", 6, "H"),
    ("box_x", 8, "H"), ("offset_x", 10, "b"), ("offset_y", 11, "b"), ("byte_12", 12, "b"), ("byte_13", 13, "b"),
    ("animation", 14, "H", ("off", FIREBALL_ANIMATIONS))])

_DOOR_FRAGMENT = Rec("piece", 10, [("dx", 0, "h"), ("dy", 2, "h"), ("speed_x", 4, "h"), ("speed_y", 6, "h"),
                                   ("animation", 8, "H", ("off", ROCK_DEBRIS_ANIMATIONS))])
_DOOR_ROLLER = Rec("roller", 6, [("x", 0, "h"), ("y", 2, "h"), ("step", 4, "B"), ("unused", 5, "B", "x")])
_BACK_ZONE = Rec("zone", 6, [("limit", 0, "H", "x"), ("handler", 2, "I", "ptr")])

_SIGN_KIND = Rec("kind", 6, [("frame", 0, "H", "frame"), ("duration", 2, "H"),
                             ("palettes", 4, "H", ("off", SIGN_PALETTE_LISTS))])
_OBJECT_KIND = Rec("kind", 12, [("z", 0, "h"), ("frame", 2, "H", "frame"), ("duration", 4, "H"),
                                ("palettes", 6, "H", ("off", SIGN_PALETTE_LISTS)), ("handler", 8, "I", "ptr")])

_PRESS = Rec("press", 4, [("at", 0, "H"), ("step", 2, "H", "x")])

_HEIGHT_CHASE = Rec("climb", 10, [("height", 0, "h"), ("threshold", 2, "h"), ("factor", 4, "h"), ("speed", 6, "h"),
                                  ("wait", 8, "H")])
_HEIGHT_STOP = Rec("stop", 6, [("height", 0, "h"), ("threshold", 2, "H", "x"), ("factor", 4, "h")])


def _machine_fire_text(data):
    out = Out(data, ["machine_fire_script (machine_fire_update, state 2): the rising fire climbs (z decreases) as",
                     "the chasers move: an entry is read once the fire's top (z - height) is above 'height'; it",
                     "sets the screen-y threshold, the catch-up factor, the base speed and 'wait' (frames). 'stop'",
                     "(a negative factor) ends the climb."])
    while out.at < len(data):
        factor = s16_at(data, out.at + 4)
        if factor is None or not out.rec(_HEIGHT_STOP if factor < 0 else _HEIGHT_CHASE):
            break
    return out.done()


_FIREBALL_SPAWN = Rec("fireball", 14, [("x", 0, "h"), ("y", 2, "h"), ("target_x", 4, "h"), ("target_y", 6, "h"),
                                       ("z", 8, "h"), ("next_handler", 10, "I", "ptr")])
_SPAWN_COUNT = Rec("count", 2, [("value", 0, "H")])


def _next_handler_spawns_text(data):
    out = Out(data, ["spawns_with_next_handler (machine_fire_update, when the machine is wrecked): the count, then",
                     "per fireball (fireball_start) x, y, where it flies to (fields 96 and 9A: init_fireball_flight),",
                     "z, and the handler it goes on to once there (field 9C)."])
    count = u16_at(data, 0) or 0
    out.rec(_SPAWN_COUNT)
    for _ in range(count):
        if not out.rec(_FIREBALL_SPAWN):
            break
    return out.done(leftover="no code found reads these; they look like a chaser path table of one kind (offset 2: "
                             "x, y, floor, then entries and an end of FFFF words)")


FORMATS.update({
    "fireball_emitter_lists": fmt(_emitter_lists_text, _EMITTER_LIST, _EMITTER, _SHOT, _SHOT_END),
    "fireball_types": indexed(
        ["fireball_types (fireball_emitter_update): 16-byte types, reached by the shots' offsets: sprite flags,",
         "palette, scale (sprite_scale), height (hitbox_height), hitbox size x, sprite offsets 64/65, the bytes",
         "the code writes into the fireball's 12 and 13 (the two halves of hitbox_size_y), and the animation (an",
         "offset from blank_fireball_animation, played at speed 4)."],
        _FIREBALL_TYPE, lambda index: f"+{index * 16:02X}"),
    "door_fragments": indexed(
        ["door_fragments (door_wall_wait_to_burst): 10-byte pieces of the door wall; the code only reads the",
         "animation (an offset into rock_debris_animations): position and speeds come from next_random, so dx,",
         "dy and the speeds here are not read."], _DOOR_FRAGMENT, "piece"),
    "door_roller_layout": indexed(
        ["door_roller_layout (spawn_door_rollers): read at + 6 n for n = 4 down to 1 (record 0 is not read): x, y",
         "and the starting step of the roller's animation (spiked_roller_animation)."], _DOOR_ROLLER, "roller"),
    "back_zones": indexed(
        ["Back zones (object_steps: spawn_chaser_back_zone by record, spawn_roller_back_zone by the roller kind's",
         "back_zone): 'limit' goes to field 80 (the zone is removed once scroll_position reaches it), 'handler'",
         "the zone's handler (records 3-7, the roller's, have none: 0)."], _BACK_ZONE, "record"),
    "roller_back_zone_limits": fmt(table_text(
        ["roller_back_zone_limits: records 3-7 of object_steps (spawn_roller_back_zone, by the roller kind's",
         "back_zone): 'limit' goes to the zone's field 80; the handler word is not read (0)."], _BACK_ZONE, None,
        lambda index: f"record {index + 3}"), _BACK_ZONE),
    "screen_sign_kinds": indexed(
        ["screen_sign_kinds (place_screen_sign): by kind, the sign's sprite frame, how long it shows (field 34,",
         "frames) and its palette cycle (an offset from sign_palette_lists)."], _SIGN_KIND, "kind"),
    "object_kinds": indexed(
        ["object_kinds (sign_start, by field 04): z, then init_sign_from_kind's record: sprite frame, how long it",
         "shows (field 34, frames), its palette cycle (an offset from sign_palette_lists, field 80) and the",
         "handler."], _OBJECT_KIND, "kind"),
    "how_to_play_button_presses": indexed(
        ["how_to_play_button_presses (how_to_play_button_hand_update): when game_mode_timer equals 'at' (frames)",
         "the hand's animation goes to 'step' (FF: none); the last word (7FFF) is a time that never comes."],
        _PRESS, "press"),
    "machine_fire_script": fmt(_machine_fire_text, _HEIGHT_CHASE, _HEIGHT_STOP),
    "spawns_with_next_handler": fmt(_next_handler_spawns_text, _SPAWN_COUNT, _FIREBALL_SPAWN),
    "bubble_heights": indexed(["bubble_heights (bubble_start): by kind (field 04), the bubble's z."],
                              Rec("kind", 2, [("z", 0, "h")]), "kind"),
    "pacer_end_positions": indexed(
        ["pacer_end_positions (player_pacer_start): by kind (field 04), the scroll position where the pacer ends",
         "(field 34)."], Rec("kind", 2, [("end", 0, "H", "x")]), "kind"),
})


# ======== fire vents, scenery sprites, Eggman's speech, the ram target, group leader holds ========

RAM_TARGET_DEBRIS_ANIMATIONS = 0x085F05

_VENT = Rec("vent", 10, [("at", 0, "H", "x"), ("x", 2, "h"), ("y", 4, "h"), ("z", 6, "h"), ("delay", 8, "h")])

_SCENERY_KIND = Rec("sprite", 12, [("sprite_flags", 0, "H", "x"), ("frame", 2, "H", "frame"), ("palette", 4, "H", "x"),
                                   ("depth_offset", 6, "h"), ("handler", 8, "I", "ptr")])
_scenery_kinds_text, _SCENERY_LIST = offset_lists_text(
    ["scenery_sprite_kinds (scenery_sprite_start): by field 04 the offset of the kind's record: sprite flags,",
     "frame, palette, a depth offset (field 8A) and the handler."],
    14, "kind", "kind", lambda out: out.rec(_SCENERY_KIND), offset_field="record")

# eggman_speech_run: (delay, command, arguments) records
_SPEECH = {
    0: Rec("end", 4, [("delay", 0, "H")], fixed={2: b"\x00\x00"}),
    1: Rec("booth", 4, [("delay", 0, "H")], fixed={2: b"\x01\x00"}),
    2: Rec("message", 12, [("delay", 0, "H"), ("x", 4, "H", "x"), ("y", 6, "H", "x"), ("kind", 8, "H", "x"),
                           ("frames", 10, "H")], fixed={2: b"\x02\x00"}),
    3: Rec("sound", 6, [("delay", 0, "H"), ("id", 4, "H", "x")], fixed={2: b"\x03\x00"}),
    4: Rec("end_4", 4, [("delay", 0, "H")], fixed={2: b"\x04\x00"}),
    7: Rec("hold", 4, [("delay", 0, "H")], fixed={2: b"\x07\x00"}),
}


def _read_speech(out):
    command = u16_at(out.data, out.at + 2)
    rec = _SPEECH.get(command)
    return rec is not None and out.rec(rec)


_speech_text, _SPEECH_LIST = offset_lists_text(
    ["eggman_speech_scripts (eggman_speech_run): by field 04 the offset of the script; each command waits",
     "'delay' frames, then: booth (spawn_eggman_booth), message (spawn_message_box at screen x, y: the kind of",
     "message_box_kinds, shown 'frames'), sound (play_sound id), hold (stays there, nothing more), end and",
     "end_4 (the object is removed)."],
    4, "kind", "kind", _read_speech)

_DAMAGE_STEP = Rec("hit", 4, [("count", 0, "H"), ("animation", 2, "H", "rel")])
_DAMAGE_END = Rec("never", 2, [("count", 0, "H")])


def _damage_script_text(data):
    out = Out(data, ["ram_target_damage_script (ram_target_check_hits, unused code): once the target has taken",
                     "'count' hits (the low byte) it plays the animation at the offset from this script (the",
                     "animations in ram_target_animation); at 4 hits it bursts. 'never' is a count no target",
                     "reaches."])
    while out.at + 4 <= len(data):
        out.rec(_DAMAGE_STEP)
    out.rec(_DAMAGE_END)
    return out.done()


_HOLD = Rec("hold", 8, [("leader", 0, "B"), ("direction", 1, "B"), ("coordinate", 2, "h"), ("end", 4, "H", "x"),
                        ("flag", 6, "H", "x")])
_hold_records_text, _HOLD_LIST = offset_lists_text(
    ["group_leader_hold_records (group_leader_hold_start): by field 04 the offset of the record: which group",
     "leader (group_leaders[n]), the direction (0/4 y, 1 x, ...: the test against 'coordinate'), the scroll",
     "position where the hold ends, and which flag it sets (0 group_leader_0_held, else",
     "group_leaders_1_3_held)."],
    5, "kind", "kind", lambda out: out.rec(_HOLD), offset_field="record")


FORMATS.update({
    "fire_vent_list": indexed(
        ["fire_vent_list (fire_vent_spawner_update): once scroll_position reaches 'at' a fire vent starts at x, y,",
         "z and blasts 'delay' frames after it is on screen; a negative delay removes the spawner (the last",
         "record, at FFFF, is never reached)."], _VENT, "vent"),
    "scenery_sprite_kinds": fmt(_scenery_kinds_text, _SCENERY_LIST, _SCENERY_KIND),
    "eggman_speech_scripts": fmt(_speech_text, _SPEECH_LIST, *_SPEECH.values()),
    "ram_target_damage_script": fmt(_damage_script_text, _DAMAGE_STEP, _DAMAGE_END),
    "ram_target_debris": indexed(
        ["Pieces that fly off the ram target (ram_target_burst, rock_debris_fly): sprite flags, dx, dy from its",
         "screen position, speed x and y (12.4 a frame), the animation (an offset into",
         "ram_target_debris_animations)."],
        _flying_piece("piece", "animation", RAM_TARGET_DEBRIS_ANIMATIONS), "piece"),
    "group_leader_hold_records": fmt(_hold_records_text, _HOLD_LIST, _HOLD),
})


# ======== chaser paths: entries of two keys, then the threshold, factor, speed and wait ========

def _chase_path_recs(first, second, first_code="H", first_style="x", second_code="h", second_style="d"):
    """The entry of a path whose records start with two keys (a scroll position, a coordinate or a height:
    'first', 'second'), then the screen threshold, the catch-up factor, the speed and the wait; and the
    8-byte 'stop' that ends it (a negative factor; speed and wait are not there)."""
    keys = [(first, 0, first_code, first_style), (second, 2, second_code, second_style)]
    entry = Rec("entry", 12, keys + [("threshold", 4, "h"), ("factor", 6, "h"), ("speed", 8, "h"), ("wait", 10, "H")])
    stop = Rec("stop", 8, keys + [("threshold", 4, "h"), ("factor", 6, "h")])
    return entry, stop


def _read_chase_path_entry(out, entry, stop):
    """An entry, or the stop (a negative factor at +6, or no room for a whole entry): returns "entry",
    "stop", or None when nothing fits."""
    factor = s16_at(out.data, out.at + 6)
    if factor is not None and (factor < 0 or out.at + 12 > len(out.data)):
        return "stop" if out.rec(stop) else None
    return "entry" if out.rec(entry) else None


def _read_chase_path(out, entry, stop):
    """Entries up to the stop: returns whether the stop came."""
    while out.at < len(out.data):
        result = _read_chase_path_entry(out, entry, stop)
        if result != "entry":
            return result == "stop"
    return False


def _chase_path_format(header, entry, stop):
    def to_text(data):
        out = Out(data, header)
        while out.at < len(data) and _read_chase_path_entry(out, entry, stop):
            pass
        return out.done()
    return fmt(to_text, entry, stop)


_CHASE_PATH_HEADER = [
    "An entry is read once its keys are reached; it sets the screen threshold, the catch-up factor, the base",
    "speed (1/16 pixel a frame) and 'wait' (frames before the next entry is read): past the threshold on",
    "screen the object moves at the base speed, behind it at max(camera speed x factor >> 8, speed). A",
    "negative factor is the end ('stop').",
]


# ---- stage 8: burning boulders, embers, the scene carriers, lava ----

BURNING_BOULDER_FLAME_ANIMATIONS = 0x087ABB

_FLAME_LAYOUT = Rec("flame", 6, [("dx", 0, "h"), ("dy", 2, "h"),
                                 ("animation", 4, "H", ("off", BURNING_BOULDER_FLAME_ANIMATIONS))])
_EMBER_SPEEDS = Rec("speeds", 4, [("z", 0, "h"), ("y", 2, "h")])
_BURNING_BOULDER = Rec("boulder", 8, [("x", 0, "h"), ("delay", 2, "H"), ("script", 4, "I", "ptr")])
_BOULDER_ENTRY, _BOULDER_STOP = _chase_path_recs("at", "y")
_BRIDGE_FIRE_ENTRY, _BRIDGE_FIRE_STOP = _chase_path_recs("height", "at", "h", "d", "H", "x")
_LAVA_FLOW_ENTRY, _LAVA_FLOW_STOP = _chase_path_recs("at", "x")

_CARRIER = Rec("carrier", 20, [("x", 0, "h"), ("y", 2, "h"), ("z", 4, "h"), ("speed_x", 6, "i", "fixed"),
                               ("speed_y", 10, "i", "fixed"), ("speed_z", 14, "i", "fixed"), ("end", 18, "H", "x")])
_carriers_text, _CARRIER_LIST = offset_lists_text(
    ["scene_carrier_records (scene_carrier_start): by field 04 the offset of the record: the carrier's x, y, z,",
     "its speeds (16.16 a frame) and the scroll position where it ends."],
    4, "kind", "kind", lambda out: out.rec(_CARRIER), offset_field="record")


FORMATS.update({
    "burning_boulder_flame_layout": indexed(
        ["burning_boulder_flame_layout (burning_boulder_start): the four flames on a burning boulder: screen dx, dy",
         "from the boulder and the animation (an offset from burning_boulder_flame_animations)."],
        _FLAME_LAYOUT, "flame"),
    "falling_ember_speeds": indexed(
        ["falling_ember_speeds (ember_emitter_move): picked at random: speed z and speed y (<< 8)."], _EMBER_SPEEDS, "pick"),
    "burning_boulder_records": indexed(
        ["burning_boulder_records (burning_boulder_start): by field 04: x (y is 3E0 - kind x 60), the frames before",
         "it falls (field 98) and its speed script (field 80; FFFFFFFF: a follower, it copies the leader's",
         "speed)."], _BURNING_BOULDER, "kind"),
    "burning_boulder_speed_script": _chase_path_format(
        ["burning_boulder_speed_script (burning_boulder_update, the leading boulder): an entry is read once",
         "scroll_position reaches 'at' or the boulder's y reaches 'y'."] + _CHASE_PATH_HEADER,
        _BOULDER_ENTRY, _BOULDER_STOP),
    "scene_carrier_records": fmt(_carriers_text, _CARRIER_LIST, _CARRIER),
    "fire_trench_flame_heights": indexed(
        ["fire_trench_flame_heights (fire_trench_flame_burn): the flame's collision height by its animation step."],
        Rec("step", 2, [("height", 0, "H")]), "step"),
    "ember_emitter_end_positions": indexed(
        ["ember_emitter_end_positions (ember_emitter_start): by field 04, the scroll position where it stops."],
        Rec("kind", 2, [("end", 0, "H", "x")]), "kind"),
    "bridge_fire_script": _chase_path_format(
        ["bridge_fire_script (bridge_fire_rise): the fire climbs the hanging log bridge; an entry is read once",
         "the fire's top (z - height) reaches 'height' or, when 'at' is not 0, scroll_position reaches at -",
         "height. A negative threshold stops the fire (state 7); a negative factor ('stop') removes it."]
        + _CHASE_PATH_HEADER[:3], _BRIDGE_FIRE_ENTRY, _BRIDGE_FIRE_STOP),
    "lava_flow_script": _chase_path_format(
        ["lava_flow_script (lava_flow_advance): the lava flows along x; an entry is read once scroll_position",
         "reaches 'at' or, when 'x' is not 0, the flow's x reaches it. 'stop' (a negative factor) removes it."]
        + _CHASE_PATH_HEADER[:3], _LAVA_FLOW_ENTRY, _LAVA_FLOW_STOP),
    "points": indexed(["(x, y) offsets, one pair a line."], POINT, "point"),
})


# ---- stage 9: the icicle droppers ----

ICICLE_DROPPER_SCRIPTS = 0x088FC1

_ICICLE_DROPPER = Rec("dropper", 12, [("x", 0, "h"), ("y", 2, "h"), ("z", 4, "h"), ("direction", 6, "H", "x"),
                                      ("flags", 8, "H", "x"), ("script", 10, "H", ("off", ICICLE_DROPPER_SCRIPTS))])
_icicle_droppers_text, _DROPPER_LIST = offset_lists_text(
    ["icicle_dropper_records (icicle_dropper_start): by field 04 the offset of the dropper: x, y, z, its",
     "direction (24: the high byte, 0 -y, 1 +x, 2/4 +y, 3 -x), its fields 96/97 (riding the ice platform,",
     "the last dropper) and its script (an offset into icicle_dropper_scripts: the scroll position word, 2",
     "bytes into an entry)."],
    10, "kind", "kind", lambda out: out.rec(_ICICLE_DROPPER), offset_field="record")
_ICICLE_ENTRY, _ICICLE_STOP = _chase_path_recs("coordinate", "at", "h", "d", "H", "x")


# ---- stage A: sand, sinkholes, the tornado ----

TORNADO_HAZARD_LISTS = 0x08A79E
TORNADO_WHIRLWIND_LISTS = 0x08A7E4

_SINKHOLE_GROWTH = Rec("kind", 4, [("zoom_step", 0, "H"), ("delay", 2, "H", "x")])
_TORNADO = Rec("tornado", 8, [("x", 0, "h"), ("y", 2, "h"), ("hazards", 4, "H", ("off", TORNADO_HAZARD_LISTS)),
                              ("whirlwinds", 6, "H", ("off", TORNADO_WHIRLWIND_LISTS))])
_TORNADO_ENTRY = Rec("entry", 14, [("at", 0, "H", "x"), ("coordinate", 2, "h"), ("threshold", 4, "h"),
                                   ("factor", 6, "h"), ("speed", 8, "h"), ("wait", 10, "H"),
                                   ("direction", 12, "H", "x")])
_TORNADO_STOP = Rec("stop", 10, [("at", 0, "H", "x"), ("coordinate", 2, "h"), ("threshold", 4, "h"),
                                 ("factor", 6, "h"), ("goal", 8, "h")])
_TORNADO_HEADER = [
    "tornado_records (tornado_start, tornado_move): by field 04 the offset of the tornado: x, y, its hazard",
    "list (tornado_hazard_lists) and whirlwind list (tornado_whirlwind_lists), then its path: an entry is read",
    "once scroll_position reaches 'at' or the tornado passes 'coordinate' along its direction; it sets the",
    "screen threshold, catch-up factor, base speed, wait (frames) and the new direction (the high byte: 0 -y,",
    "1 +x, 2/4 +y, 3 -x; the F0 x 80 box turns with it). 'stop' (a negative factor) ends it: the tornado and its",
    "columns go, and a negative 'goal' sets goal_unlocked first. tornado_move clears group_leader_0_held when",
    "it reads the entry before tornado_kind_0_end_record (its path then points there).",
]
_TORNADO_KIND = Rec("kind", 2, [("record", 0, "H", "rel")])


def _read_tornado_path(out):
    """Entries up to the stop; returns whether the stop came."""
    while out.at < len(out.data):
        factor = s16_at(out.data, out.at + 6)
        if factor is not None and factor < 0:
            return out.rec(_TORNADO_STOP)
        if not out.rec(_TORNADO_ENTRY):
            return False
    return False


def _tornado_text(offsets):
    """The tornado records: with 'offsets', the piece starts with the offsets table; else in the middle of a
    path (tornado_kind_0_end_record: kind 0's stop, then kind 1)."""
    def to_text(data):
        out = Out(data, _TORNADO_HEADER)
        targets = {}
        if offsets:
            for index in range(2):
                targets[u16_at(data, out.at)] = index
                out.rec(_TORNADO_KIND, f"kind {index:02X}")
        elif not _read_tornado_path(out):
            return out.done()
        while out.at < len(data):
            out.blank()
            if out.at in targets:
                out.comment(f"+{out.at:04X}: kind {targets[out.at]:02X}")
            if not out.rec(_TORNADO):
                break
            if not _read_tornado_path(out):
                break
        return out.done()
    return to_text


_HAZARD = Rec("hazard", 4, [("at", 0, "H", "x"), ("coordinate", 2, "h")])
_WHIRLWIND = Rec("whirlwind", 2, [("at", 0, "H", "x")])
_SWIRL = Rec("swirl", 10, [("depth_offset", 0, "h"), ("streak", 2, "I", "ptr"), ("puff", 6, "I", "ptr")])


def _swirl_text(data):
    out = Out(data, ["sink_swirl_effects (spawn_sink_swirl): two objects around a player sinking in quicksand: a",
                     "depth offset, the streak's animation and its sand puff's (played once, same screen position);",
                     "then those animations (last step, frames)."])
    out.rec(_SWIRL)
    out.rec(_SWIRL)
    while out.at < len(data):
        out.comment(f"+{out.at:04X}")
        if not frames_line(out):
            break
    return out.done()


FORMATS.update({
    "icicle_dropper_records": fmt(_icicle_droppers_text, _DROPPER_LIST, _ICICLE_DROPPER),
    "icicle_dropper_scripts": _chase_path_format(
        ["icicle_dropper_scripts (icicle_dropper_move): the dropper's path; an entry is read once the dropper",
         "passes 'coordinate' along its direction or scroll_position reaches 'at' (riding the platform: once",
         "its travel on the platform reaches 'coordinate'); the droppers point 2 bytes into their first entry.",
         "'stop' (a negative factor) ends the drops: icicle_dropper_ready, and the last dropper finishes."]
        + _CHASE_PATH_HEADER, _ICICLE_ENTRY, _ICICLE_STOP),
    "ice_floor_floors": indexed(
        ["ice_floor_floors (ice_floor_start): by field 04, the offset of its floor block in floor_blocks (u32)."],
        Rec("kind", 4, [("floor_block", 0, "I", "x")]), "kind"),
    "sinkhole_growth_records": indexed(
        ["sinkhole_growth_records (sinkhole_start, rimmed_sinkhole_start): by field 04: the zoom step and the",
         "delay in scroll units before it grows."], _SINKHOLE_GROWTH, "kind"),
    "tornado_records": fmt(_tornado_text(True), _TORNADO_KIND, _TORNADO, _TORNADO_ENTRY, _TORNADO_STOP),
    "tornado_records_from_kind_0_end": fmt(_tornado_text(False), _TORNADO_KIND, _TORNADO, _TORNADO_ENTRY,
                                           _TORNADO_STOP),
    "tornado_hazard_lists": indexed(
        ["tornado_hazard_lists (tornado_move): from the tornado's offset, (scroll position, coordinate) pairs:",
         "once scroll_position reaches 'at' and the tornado passes 'coordinate' a hazard starts at its block's",
         "edge (spawn_tornado_hazard); a list ends with 'at' FFFF (never reached)."], _HAZARD,
        lambda index: f"+{index * 4:02X}"),
    "tornado_whirlwind_lists": indexed(
        ["tornado_whirlwind_lists (tornado_throw_whirlwinds): from the tornado's offset, the scroll positions",
         "where it throws a whirlwind; FFFF ends (kind 1's offset points at it: no whirlwinds)."], _WHIRLWIND,
        lambda index: f"+{index * 2:02X}"),
    "sink_swirl_effects": fmt(_swirl_text, _SWIRL, specials={"frames": frames_special}),
})


# ---- stages B-E: the fire front, the biting head, the brick wall, the rising saws, the grate steps ----

BRICK_ANIMATIONS = 0x08BED5
GRATE_STEP_DEBRIS_ANIMATIONS = 0x08C11D

_FIRE_FRONT = Rec("front", 8, [("x", 0, "h"), ("y", 2, "h"), ("floor", 4, "h"), ("direction", 6, "H", "x")])
_FRONT_ENTRY, _FRONT_STOP = _chase_path_recs("at", "limit")


def _read_front(out):
    return out.rec(_FIRE_FRONT) and _read_chase_path(out, _FRONT_ENTRY, _FRONT_STOP)


_fire_front_text, _FRONT_LIST = offset_lists_text(
    ["fire_front_paths (fire_front_init, fire_front_chase): by field 04 the offset of the path: x, y, floor",
     "height, the direction (24: the high byte, 0 -y, 1 +x, 2/4 +y, 3 -x), then the entries: one is read once",
     "scroll_position reaches 'at' (FFFF: never) or the front passes 'limit'. At the entry whose factor is 0",
     "the front stops (box width F2, chaser_stopped); 'stop' (a negative factor) clears chaser_object and",
     "removes it."] + _CHASE_PATH_HEADER, 2, "kind", "kind", _read_front)

_BITING_HEAD = Rec("head", 4, [("x", 0, "h"), ("y", 2, "h")])
_HEAD_ENTRY = Rec("entry", 14, [("at", 0, "H", "x"), ("limit", 2, "h"), ("threshold", 4, "h"), ("factor", 6, "h"),
                                ("speed", 8, "h"), ("wait", 10, "H"), ("direction", 12, "H", "x")])
_HEAD_STOP = Rec("stop", 8, [("at", 0, "H", "x"), ("limit", 2, "h"), ("threshold", 4, "h"), ("factor", 6, "h")])


def _read_head(out):
    if not out.rec(_BITING_HEAD):
        return False
    while out.at < len(out.data):
        factor = s16_at(out.data, out.at + 6)
        if factor is not None and factor < 0:
            return out.rec(_HEAD_STOP)
        if not out.rec(_HEAD_ENTRY):
            return False
    return True


_biting_head_text, _HEAD_LIST = offset_lists_text(
    ["biting_head_paths (biting_head_init, biting_head_chase): by field 04 the offset of the path: x, y, then",
     "the entries (read once scroll_position reaches 'at' or the head passes 'limit'), each with the new",
     "direction (the high byte: 3 = -x)."] + _CHASE_PATH_HEADER, 2, "kind", "kind", _read_head)

_BRICK_LOOK = Rec("palette", 2, [("word", 0, "H", "x")])


def _brick_animations_text(data):
    out = Out(data, ["brick_animations (rock_debris_fly for the bricks of brick_wall_wait): each brick's look is a",
                     "palette word, then the animation (last step, frames); the bricks' offsets point at the",
                     "palette."])
    while out.at < len(data):
        out.comment(f"+{out.at:04X}")
        if not (out.rec(_BRICK_LOOK) and frames_line(out)):
            break
    return out.done()


_DUST = Rec("puff", 8, [("sprite_flags", 0, "H", "x"), ("dx", 2, "h"), ("dy", 4, "h"), ("delay", 6, "H")])

_SAW = Rec("saw", 6, [("x", 0, "h"), ("y", 2, "h"), ("z", 4, "h")])
_SAW_ENTRY, _SAW_STOP = _chase_path_recs("at", "z")


def _read_saw(out):
    return out.rec(_SAW) and _read_chase_path(out, _SAW_ENTRY, _SAW_STOP)


_rising_saw_text, _SAW_LIST = offset_lists_text(
    ["rising_saw_paths (rising_saw_init, rising_saw_rise): by field 04 the offset of the saw: x, y, z, then the",
     "entries (read once scroll_position reaches 'at' or the saw rises to 'z'; the threshold is a screen y)."]
    + _CHASE_PATH_HEADER, 3, "kind", "kind", _read_saw)

_GRATE_PIECE = Rec("piece", 10, [("dx", 0, "h"), ("dy", 2, "h"), ("speed_x", 4, "h"), ("speed_z", 6, "h"),
                                 ("animation", 8, "H", ("off", GRATE_STEP_DEBRIS_ANIMATIONS))])


def _grate_debris_text(data):
    out = Out(data, ["grate_step_debris (grate_step_update): the count, then per piece the map dx, dy, speed x and",
                     "speed z, and its animation (an offset into grate_step_debris_animations)."])
    count = u16_at(data, 0) or 0
    out.rec(_SPAWN_COUNT)
    for _ in range(count):
        if not out.rec(_GRATE_PIECE):
            break
    return out.done()


FORMATS.update({
    "fire_front_paths": fmt(_fire_front_text, _FRONT_LIST, _FIRE_FRONT, _FRONT_ENTRY, _FRONT_STOP),
    "biting_head_paths": fmt(_biting_head_text, _HEAD_LIST, _BITING_HEAD, _HEAD_ENTRY, _HEAD_STOP),
    "biting_head_bounce": indexed(
        ["biting_head_bounce (biting_head_chase): added to the head's screen y frame by frame after a bite;",
         "7F (127) ends."], Rec("bounce", 1, [("dy", 0, "b")]), "frame"),
    "brick_animations": fmt(_brick_animations_text, _BRICK_LOOK, specials={"frames": frames_special}),
    "brick_wall_brick_records": indexed(
        ["brick_wall_brick_records (brick_wall_wait, after the count brick_wall_bricks): per brick the sprite",
         "flags, screen dx, dy, speed x and y (12.4), and its look (an offset into brick_animations: a palette",
         "and an animation); rock_debris_fly moves them."], _flying_piece("brick", "look", BRICK_ANIMATIONS), "brick"),
    "brick_wall_dust_records": indexed(
        ["brick_wall_dust_records (brick_wall_wait, after the count brick_wall_dust): per dust puff the sprite",
         "flags, screen dx, dy and the frames it waits (field 80)."], _DUST, "puff"),
    "rising_saw_paths": fmt(_rising_saw_text, _SAW_LIST, _SAW, _SAW_ENTRY, _SAW_STOP),
    "grate_step_debris": fmt(_grate_debris_text, _SPAWN_COUNT, _GRATE_PIECE),
})


# ---- stage 10: geysers, the waterfall, rafts, the propeller machine ----

GEYSER_LISTS = 0x08D902
GEYSER_LINE_SCRIPTS = 0x08DB20

_ROW_PATTERN = Rec("row", 1, [("pattern", 0, "B")])
_row_patterns_text, _ROW_FRONT = offset_lists_text(
    ["geyser_row_patterns (geyser_front_rise): by front the offset of its rows: one byte a row, which",
     "geyser_row_delays pattern its five geysers take (both fronts share the list)."],
    2, "front", "front", lambda out: out.rec(_ROW_PATTERN), offset_field="rows")

_ROW_DELAYS = Rec("pattern", 10, [("geyser_0", 0, "H"), ("geyser_1", 2, "H"), ("geyser_2", 4, "H"),
                                  ("geyser_3", 6, "H"), ("geyser_4", 8, "H")])

_GEYSER_FRONT = Rec("rise", 6, [("start", 0, "h"), ("top", 2, "h"), ("geysers", 4, "H", ("off", GEYSER_LISTS))])
_FRONT6_ENTRY, _FRONT6_STOP = _chase_path_recs("at", "z")
_geyser_fronts_text, _GEYSER_FRONT_LIST = offset_lists_text(
    ["geyser_fronts (geyser_front_init, geyser_front_rise): by front the offset of its record: the start and",
     "top heights, its geyser list (geyser_lists), then the entries (read once scroll_position reaches 'at'",
     "or the front rises to 'z'; the threshold is a screen y)."] + _CHASE_PATH_HEADER,
    2, "front", "front", lambda out: out.rec(_GEYSER_FRONT) and _read_chase_path(out, _FRONT6_ENTRY, _FRONT6_STOP),
    offset_field="record")

_GEYSER = Rec("geyser", 10, [("at", 0, "H", "x"), ("x", 2, "h"), ("y", 4, "h"), ("z", 6, "h"), ("start", 8, "H", "x")])

_RAFT_SPAWNER = Rec("spawner", 8, [("x", 0, "h"), ("y", 2, "h"), ("floor", 4, "h"), ("end", 6, "H", "x")])
_RAFT = Rec("raft", 8, [("distance", 0, "H"), ("speed", 2, "H", "x"), ("floor_block", 4, "I", "x")])
_RAFT_LOOP = end_rec("loop", b"\x00\x80")


def _read_raft_spawner(out):
    if not out.rec(_RAFT_SPAWNER):
        return False
    while out.at < len(out.data):
        word = u16_at(out.data, out.at)
        if word is None:
            return False
        if word == 0x8000:
            out.rec(_RAFT_LOOP)
            return out.rec(END) or True
        if word & 0x8000:
            return out.rec(END)
        if not out.rec(_RAFT):
            return False
    return True


_raft_spawners_text, _RAFT_LIST = offset_lists_text(
    ["river_raft_spawners (river_raft_spawner_init, river_raft_spawner_update): by field 04 the offset of the",
     "spawner: x, y, its floor height and the scroll position where it ends; then the rafts: each starts",
     "'distance' behind the spawner's y once the last one has moved on, with its speed and floor block (the",
     "offset of a floor_blocks block: x and z come from it). 'loop' (8000) starts the list again; the 'end'",
     "after it (FFFF) is never read."],
    2, "kind", "kind", _read_raft_spawner, offset_field="record")

_STONE_BLOCK = Rec("block", 8, [("x", 0, "h"), ("y", 2, "h"), ("z", 4, "h"), ("depth_offset", 6, "h")])
_LINE_START = Rec("front", 8, [("x", 0, "h"), ("y", 2, "h"), ("z", 4, "h"),
                               ("script", 6, "H", ("off", GEYSER_LINE_SCRIPTS))])
_LINE_ENTRY = Rec("entry", 8, [("at", 0, "H", "x"), ("limit", 2, "h"), ("speed", 4, "h"), ("wait", 6, "h")])

_PROPELLER_ENTRY, _PROPELLER_STOP = _chase_path_recs("at", "y")
_ANCHOR = Rec("anchor", 8, [("x", 0, "h"), ("y", 2, "h"), ("z", 4, "h"), ("end", 6, "H", "x")])


def _ended_lists_text(header, rec, end=END, end_word=0xFFFF):
    """to_text of lists of 'rec' records one after the other, each closed by 'end' (a record starting with
    end_word); a comment marks where each list starts."""
    def to_text(data):
        out = Out(data, header)
        fresh = True
        while out.at < len(data):
            if fresh:
                out.comment(f"+{out.at:04X}")
                fresh = False
            if u16_at(data, out.at) == end_word:
                if not out.rec(end):
                    break
                fresh = True
            elif not out.rec(rec):
                break
        return out.done()
    return to_text


FORMATS.update({
    "geyser_row_patterns": fmt(_row_patterns_text, _ROW_FRONT, _ROW_PATTERN),
    "geyser_row_delays": indexed(
        ["geyser_row_delays (geyser_front_rise): per pattern, the frames each of a row's five geysers waits",
         "(field 80)."], _ROW_DELAYS, "pattern"),
    "geyser_fronts": fmt(_geyser_fronts_text, _GEYSER_FRONT_LIST, _GEYSER_FRONT, _FRONT6_ENTRY, _FRONT6_STOP),
    "geyser_lists": fmt(_ended_lists_text(
        ["geyser_lists (geyser_front_rise): from the front's offset (front 0 +00, front 1 +34), the geysers: once",
         "scroll_position reaches 'at' a geyser starts at x, y, z, erupting from scroll position 'start'; 'end'",
         "(FFFF) closes a list."], _GEYSER), _GEYSER, END),
    "river_raft_spawners": fmt(_raft_spawners_text, _RAFT_LIST, _RAFT_SPAWNER, _RAFT, _RAFT_LOOP, END),
    "stone_block_positions": indexed(
        ["stone_block_positions (stone_block_init): by field 04: x, y, z and a depth offset."], _STONE_BLOCK,
        "kind"),
    "geyser_line_starts": indexed(
        ["geyser_line_starts (spawn_geyser_line): by front: x, y, z and the script (geyser_line_scripts)."],
        _LINE_START, "front"),
    "geyser_line_scripts": indexed(
        ["geyser_line_scripts (geyser_line_move): entries read once scroll_position reaches 'at' or the line",
         "passes 'limit' (y): the speed and the frames to wait; a negative wait ends the script."], _LINE_ENTRY,
        lambda index: f"+{index * 8:02X}"),
    "propeller_machine_path": _chase_path_format(
        ["propeller_machine_path (propeller_machine_chase): an entry is read once scroll_position reaches 'at'",
         "or the machine's y reaches 'y' (it moves +y)."] + _CHASE_PATH_HEADER, _PROPELLER_ENTRY, _PROPELLER_STOP),
    "scene_anchor_positions": indexed(
        ["scene_anchor_positions: x, y, z of the anchor, and the scroll position where it ends."], _ANCHOR, "anchor"),
})


# ======== the walls ========

_WALL = Rec("wall", 14, [("at", 0, "H", "x"), ("end", 2, "H", "x"), ("x", 4, "h"), ("y", 6, "h"), ("z", 8, "h"),
                         ("kind", 10, "H", "x"), ("parts", 12, "B"), ("depth", 13, "B")])


def _read_wall(out):
    return out.rec(END) or out.rec(_WALL)


_wall_spawns_text, _WALL_STAGE = offset_lists_text(
    ["wall_spawns (spawn_walls): per stage the offset of its list: once scroll_position reaches 'at' a wall",
     "(wall_update) starts at x, y, z: removed once scroll_position reaches 'end' (0: once it has left the",
     "screen), its look and box from the wall_kinds record at offset 'kind', 'parts' sprite parts (the box size per",
     "part when the kind's is negative), 'depth' (field 37: depth_from_next). 'end' (FFFF) closes a list."],
    19, "stage", "stage", _read_wall)

# wall_kinds: 20-byte records read at the kind's offset; the table is cut where the code reads a field
_WALL_KIND = Rec("kind", 20, [
    ("box_offset_x", 0, "b"), ("box_offset_y", 1, "b"), ("sprite_offset_x", 2, "b"), ("sprite_offset_y", 3, "b"),
    ("direction", 4, "B"), ("unused", 5, "B", "x"), ("box_x", 6, "H", "x"), ("box_y", 8, "H", "x"),
    ("palette", 10, "H", "x"), ("step_x", 12, "b"), ("step_y", 13, "b"), ("sprite", 14, "H", "frame"),
    ("second_step_x", 16, "b"), ("second_step_y", 17, "b"), ("second_sprite", 18, "H", "frame")], phased=True)
_WALL_KINDS_HEADER = [
    "wall_kinds (spawn_walls, build_wall_parts): 20-byte records, labelled by their offset (a wall's 'kind'):",
    "hitbox and sprite offsets (signed bytes), the direction (field 24's high byte), the box size x and y (bit",
    "15 set: the size of one part, times the wall's part count), the palette, then the parts: each part steps",
    "by step_x, step_y from the last and shows 'sprite' (FFFF: no parts, the wall is only a box), and when",
    "second_sprite is not FFFF a second part (at second_step_x/y, palette 9FFE) goes with each. The table is",
    "cut where the code reads its fields (wall_kinds, wall_kind_second_sprites = +12): each piece holds the",
    "fields from there on.",
]

FORMATS["wall_spawns"] = fmt(_wall_spawns_text, _WALL_STAGE, _WALL, END)
FORMATS.update(phased_formats("wall_kinds", _WALL_KIND, lambda phase: fmt(
    phased_text(_WALL_KINDS_HEADER, _WALL_KIND, phase, lambda index: f"+{index * 20:03X}"), _WALL_KIND)))


# ======== the object groups (spawn_object_groups, spawn_object_groups_2) ========

GROUP_LEADER_DATA = 0x09357D
GROUP_SLOT_TRIGGERS = 0x093539


def _group_rec(keyword, kind):
    """A 24-byte group record (type 'kind'): its members are 'count' objects from the record at offset
    'record' of the records table."""
    return Rec(keyword, 24, [
        ("at", 0, "H", "x"), ("leader", 3, "B"), ("slot", 4, "B"), ("unused", 5, "B", "x"),
        ("floor_block", 6, "I", "x"), ("count", 10, "H"), ("step_x", 12, "h"), ("step_y", 14, "h"),
        ("direction", 16, "H", "x"), ("x", 18, "h"), ("y", 20, "h"), ("record", 22, "H", "x")],
        fixed={2: bytes([kind])})


def _leader_rec(keyword, kind, path_field="path", path_base=GROUP_LEADER_DATA):
    """A 14-byte leader record (type 'kind'): which group_leaders slot, x, y, z, direction, its path."""
    return Rec(keyword, 14, [("at", 0, "H", "x"), ("leader", 3, "B"), ("x", 4, "h"), ("y", 6, "h"), ("z", 8, "h"),
                             ("direction", 10, "H", "x"), (path_field, 12, "H", ("off", path_base))],
               fixed={2: bytes([kind])})


_GROUP_TYPES = {
    0: _group_rec("group", 0),
    1: _leader_rec("leader", 1),
    2: _group_rec("group_stepped", 2),
    3: _leader_rec("slot_trigger", 3, "triggers", GROUP_SLOT_TRIGGERS),
    4: Rec("ice_leader", 14, [("at", 0, "H", "x"), ("leader", 3, "B"), ("end", 4, "H", "x"), ("reference_x", 6, "h"),
                              ("x", 8, "h"), ("y", 10, "h"), ("z", 12, "h")], fixed={2: b"\x04"}),
    5: _leader_rec("sand_leader", 5),
    6: _group_rec("group_silent", 6),
    7: _leader_rec("waterfall", 7),
    8: _leader_rec("leader_8", 8),
}

_RECORD_TYPES_2 = {
    0: _group_rec("bridge", 0),
    1: _leader_rec("leader", 1),
    2: _group_rec("bridge_0", 2),
    3: _group_rec("bridge_1", 3),
    4: _leader_rec("leader_4", 4),
}


def _typed_reader(types):
    def read(out):
        if u16_at(out.data, out.at) == 0xFFFF:
            return out.rec(END)
        kind = out.data[out.at + 2] if out.at + 2 < len(out.data) else None
        return kind in types and out.rec(types[kind])
    return read


_object_groups_text, _GROUP_STAGE = offset_lists_text(
    ["object_group_spawns (spawn_object_groups): per stage the offset of its list, records sorted by 'at' (the",
     "scroll position that starts them), by type:",
     "  group, group_stepped (each member 8 nearer the viewer: field 96 - 8 x count), group_silent (the first",
     "  member marked): 'count' objects from the stage_object_records record at offset 'record', the first at",
     "  x, y, each next one step_x, step_y on, on floor block 'floor_block', with 'direction'; their parent is",
     "  group_leaders[leader]; 'slot' set: they take a free group_slots slot",
     "  leader, leader_8, sand_leader, waterfall: a group leader (spawn_group_leader, spawn_sand_block_leader,",
     "  spawn_waterfall) in group_leaders[leader] at x, y, z moving in 'direction' along its path",
     "  (group_leader_data)",
     "  ice_leader: the stage 9 ice platform leader (spawn_ice_platform_leader): 'end' scroll position, its",
     "  travel (x - reference_x), x, y, z",
     "  slot_trigger (spawn_group_slot_trigger): clears group_slots and runs the triggers at 'triggers'",
     "  (group_slot_triggers); its other fields are not read",
     "'end' (FFFF) closes a list."],
    19, "stage", "stage", _typed_reader(_GROUP_TYPES))

_record_spawns_text, _RECORD_STAGE = offset_lists_text(
    ["stage_record_spawns (spawn_object_groups_2): per stage the offset of its list, by type: bridge (the first",
     "free of the two rope bridges), bridge_0, bridge_1: 'count' planks from the stage_object_records_2 record",
     "at offset 'record' (the fields as object_group_spawns' groups); leader, leader_4: a group leader",
     "(spawn_group_leader). 'end' (FFFF) closes a list."],
    19, "stage", "stage", _typed_reader(_RECORD_TYPES_2))


# stage_object_records: the members' records (22 bytes); multi-part sprites keep a part list pointer in 2-5
def _object_record(keyword, size, extra):
    plain = Rec(keyword, size, [("sprite_flags", 0, "H", "x"), ("frame", 2, "H", "frame"), ("palette", 4, "H", "x"),
                                ("handler", 6, "I", "ptr")] + extra, phased=True)
    parts = Rec(keyword + "_parts", size, [("sprite_flags", 0, "H", "x"), ("parts", 2, "I", "ptr"),
                                           ("handler", 6, "I", "ptr")] + extra, phased=True)
    return plain, parts


_OBJECT_RECORD, _OBJECT_RECORD_PARTS = _object_record("record", 22, [
    ("animation", 10, "I", "ptr"), ("box_x", 14, "H"), ("box_y", 16, "H"), ("offset_x", 18, "b"),
    ("offset_y", 19, "b"), ("field_96", 20, "H", "x")])
_OBJECT_RECORD_2, _OBJECT_RECORD_2_PARTS = _object_record("record", 24, [
    ("animation", 10, "I", "ptr"), ("animation_2", 14, "I", "ptr"), ("box_x", 18, "H"), ("box_y", 20, "H"),
    ("field_96", 22, "H", "x")])


def _object_records_format(header, plain, parts, phase):
    """The records, cut by their pieces like phased_text; a record whose sprite_flags (when in the piece)
    say multi-part (C000/D00x) shows its part list pointer instead of frame and palette."""
    def to_text(data):
        out = Out(data, header)
        position = -(phase % plain.size)
        while position < len(data):
            lo, hi = max(0, -position), min(plain.size, len(data) - position)
            rec = plain
            if lo == 0 and hi >= 2 and struct.unpack_from("<H", data, position)[0] & 0xC000 == 0xC000:
                rec = parts
            if not rec.tiles(lo, hi):
                rec = plain
            if rec.tiles(lo, hi):
                out.part(rec, lo, hi, f"+{(position + phase):03X}")
            else:
                out.raw(position + hi)
            position += plain.size
        return out.done()
    return fmt(to_text, plain, parts)


_OBJECT_RECORDS_HEADER = [
    "stage_object_records (spawn_object_groups): the group members' records, labelled by their offset (a",
    "group's 'record'): sprite flags, frame and palette (or, for a multi-part sprite, flags C000/D00x, the",
    "address of its part list), handler, field 48 (the animation, or data the handler reads), hitbox sizes,",
    "sprite offsets 64/65, field 96.",
]
_OBJECT_RECORDS_2_HEADER = [
    "stage_object_records_2 (spawn_object_groups_2): the rope bridge planks' records, labelled by their offset",
    "(a group's 'record'): sprite flags, frame and palette (or the part list's address for a multi-part sprite),",
    "handler, field 48 (the animation; odd members get animation_2), hitbox sizes, field 96 (sprite offsets 64/65",
    "are 2E/11 for all). The table is cut where rope_bridge_plank_update reads record 0's fields: each piece",
    "holds the fields from there on.",
]

_SLOT_TRIGGER = Rec("trigger", 6, [("at", 0, "H", "x"), ("slot", 2, "H"), ("delay", 4, "H")])
_SLOT_GOAL = Rec("goal", 4, [("at", 0, "H", "x")], fixed={2: b"\x00\x80"})
_SLOT_END = Rec("end", 4, [("at", 0, "H", "x"), ("word", 2, "h")])


def _slot_triggers_text(data):
    out = Out(data, ["group_slot_triggers (group_slot_trigger_update): from the trigger's offset: once",
                     "scroll_position reaches 'at' group slot 'slot' (if taken) is set to 8000 | delay; 'goal'",
                     "(slot word 8000) sets goal_unlocked and ends, 'end' (another negative word) ends."])
    out.comment("+0000")
    while out.at < len(data):
        word = s16_at(data, out.at + 2)
        if word is None:
            break
        if word >= 0:
            if not out.rec(_SLOT_TRIGGER):
                break
            continue
        if not out.rec(_SLOT_GOAL if word == -0x8000 else _SLOT_END):
            break
        if out.at < len(data):
            out.comment(f"+{out.at:04X}")
    return out.done()


def _leader_data_text(data):
    out = Out(data, ["group_leader_data: the paths of the group leaders (group_leader_move), the sand block leader",
                     "and the waterfall, reached by their records' 'path' offsets (the comments give each path's)."]
              + [""] + _CHASE_HEADER)
    out.comment("+0000")
    while out.at < len(data):
        factor = s16_at(data, out.at + 4)
        if factor is None or not _read_chase(out):
            break
        if factor < 0 and out.at < len(data):
            out.blank()
            out.comment(f"+{out.at:04X}")
    return out.done()


_ROPES = Rec("ropes", 8, [("rope_1_dx", 0, "h"), ("rope_1_dy", 2, "h"), ("rope_2_dx", 4, "h"), ("rope_2_dy", 6, "h")],
             phased=True)
_ROPES_HEADER = ["rope_bridge_rope_offsets (rope_bridge_plank_update): the screen offsets (dx, dy) of a plank's two",
                 "ropes: record 0 for a plank, record 1 (wide_rope_bridge_rope_offsets) for the wide plank (kind 30).",
                 "The table is cut where the code reads its words: each piece holds the words from there on."]


FORMATS.update({
    "object_group_spawns": fmt(_object_groups_text, _GROUP_STAGE, END, *_GROUP_TYPES.values()),
    "stage_record_spawns": fmt(_record_spawns_text, _RECORD_STAGE, END, *_RECORD_TYPES_2.values()),
    "stage_object_records": _object_records_format(_OBJECT_RECORDS_HEADER, _OBJECT_RECORD, _OBJECT_RECORD_PARTS, 0),
    "group_slot_triggers": fmt(_slot_triggers_text, _SLOT_TRIGGER, _SLOT_GOAL, _SLOT_END),
    "group_leader_data": fmt(_leader_data_text, _CHASE, _CHASE_END),
})
FORMATS.update(phased_formats("stage_object_records_2", _OBJECT_RECORD_2, lambda phase: _object_records_format(
    _OBJECT_RECORDS_2_HEADER, _OBJECT_RECORD_2, _OBJECT_RECORD_2_PARTS, phase)))
FORMATS.update(phased_formats("rope_offsets", _ROPES, lambda phase: fmt(
    phased_text(_ROPES_HEADER, _ROPES, phase), _ROPES)))


# ======== stage events (stages 0, 5, 6, F, 1A), message boxes ========

_GEYSER_PHASE = Rec("phase", 4, [("frames", 0, "H"), ("speed", 2, "H", "x")])
_FALLING_ROCK = Rec("rock", 14, [("x", 0, "h"), ("y", 2, "h"), ("z", 4, "h"), ("speed_x", 6, "h"), ("speed_y", 8, "h"),
                                 ("animation", 10, "I", "ptr")])
_MESSAGE_BOX = Rec("kind", 8, [("frame", 0, "H", "frame"), ("palette", 2, "H", "x"), ("width", 4, "H"),
                               ("height", 6, "H")])
_TIMED_MESSAGE = Rec("message", 10, [("time", 0, "H"), ("x", 2, "H", "x"), ("y", 4, "H", "x"), ("kind", 6, "H", "x"),
                                     ("frames", 8, "H")])
_TIME_END = end_rec("end", b"\xff\x7f")


def _timed_messages_text(data):
    out = Out(data, ["event_0f_messages (event_0f_show_messages): once scene_timer reaches 'time' a message box",
                     "(spawn_message_box at screen x, y: the kind of message_box_kinds) shows for 'frames'; 'end'",
                     "(7FFF) is a time that never comes."])
    while out.at < len(data):
        if not (out.rec(_TIME_END) if u16_at(data, out.at) == 0x7FFF else out.rec(_TIMED_MESSAGE)):
            break
    return out.done()


_STRIKE = Rec("strike", 10, [("time", 0, "H"), ("x", 2, "H", "x"), ("y", 4, "H", "x"), ("animation", 6, "I", "ptr")])


def _lightning_text(data):
    out = Out(data, ["stage_06_lightning_strikes (spawn_stage_06_lightning): once scene_timer reaches 'time' a",
                     "bolt shows at screen x, y playing 'animation'; 'end' (7FFF) is a time that never comes; then",
                     "the bolts' animations (last step, frames)."])
    while out.at < len(data):
        if u16_at(data, out.at) == 0x7FFF:
            out.rec(_TIME_END)
            break
        if not out.rec(_STRIKE):
            break
    while out.at < len(data):
        out.comment(f"+{out.at:04X}")
        if not frames_line(out):
            break
    return out.done()


_SCROLL_STEP = Rec("step", 6, [("at", 0, "H", "x"), ("mode", 2, "H"), ("speed", 4, "H")], phased=True)
_SCROLL_HEADER = [
    "event_1a_scroll_script (run_game_mode, game mode 1A): by scene_step, once scroll_position reaches 'at'",
    "(FFFF: once 20E63A is set and scene_timer reaches 1E) the scroll mode and speed change; 7FFF is a",
    "position that never comes. The code reads step 0's mode and speed by their own names: the table is cut",
    "there, each piece holding the fields from there on.",
]


FORMATS.update({
    "stage_00_geyser_speeds": indexed(
        ["stage_00_geyser_speeds (stage_00_geyser_rise): the geyser rises at 'speed' (>> 12) for 'frames', then",
         "the next pair; 7FFF frames is the last."], _GEYSER_PHASE, "phase"),
    "falling_rocks": indexed(
        ["falling_rocks (spawn_falling_rocks, falling_rock_count of them): x, y, z, speed x and y (<< C) and the",
         "animation (in rock_debris_animations); falling_rock_bounce moves them."], _FALLING_ROCK, "rock"),
    "message_box_kinds": indexed(
        ["message_box_kinds (spawn_message_box): by kind, the box's sprite (a sized frame), palette, width and",
         "height (sprite 0000: no picture)."], _MESSAGE_BOX, "kind"),
    "event_0f_messages": fmt(_timed_messages_text, _TIMED_MESSAGE, _TIME_END),
    "stage_06_lightning_strikes": fmt(_lightning_text, _STRIKE, _TIME_END, specials={"frames": frames_special}),
})
FORMATS.update(phased_formats("event_1a_scroll_script", _SCROLL_STEP, lambda phase: fmt(
    phased_text(_SCROLL_HEADER, _SCROLL_STEP, phase, lambda index: f"step {index}"), _SCROLL_STEP)))
