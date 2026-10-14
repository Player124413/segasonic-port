"""
Name:        stages.py
Description: the level data as text: floor blocks and their screen bounds, floor types and slopes, the stage
             spawn lists, restart points, player paths, goals and the small tables that go with them
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

A piece never changes size (the code reaches every piece at a fixed address): a level designer moves a
block, an object or a restart point by editing its numbers, not by adding lines.
"""
import re
import struct
from decimal import ROUND_HALF_EVEN, Decimal
from pathlib import Path

_ROOT = Path(__file__).resolve().parents[2]
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

def _content(text):
    """(line number, tokens) of every line that holds something."""
    for number, line in enumerate(text.split("\n"), 1):
        line = line.split("#", 1)[0].strip()
        if line:
            yield number, line.split()


def _number(token, where):
    """A value as written: 0x.. hex or decimal, either may be negative."""
    try:
        if token.lstrip("-+").lower().startswith("0x"):
            return int(token, 16)
        return int(token, 10)
    except ValueError:
        raise ValueError(f"{where}: '{token}' is not a number") from None


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


def _fixed_text(raw):
    """A 16.16 value as the shortest decimal that comes back to it."""
    exact = Decimal(raw) / 65536
    for places in range(7):
        text = str(exact.quantize(Decimal(1).scaleb(-places), rounding=ROUND_HALF_EVEN))
        if _fixed_value(text) == raw:
            return text
    return str(exact)


def _fixed_value(text):
    return int((Decimal(text) * 65536).to_integral_value(rounding=ROUND_HALF_EVEN))


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
            return _fixed_text(value)
        if isinstance(self.style, tuple):
            return f"0x{self.style[1] + value:06X}"
        return str(value)

    def pack(self, token, where):
        if self.style == "fixed":
            try:
                value = _fixed_value(token)
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
        for number, tokens in _content(text):
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
    Written as the program address they reach."""
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
            lines.append(f"{keyword} {first + index:02X}  0x{target:06X}" + _note(_target_name(target, back), note))
        lines += _bytes_lines(data[count * size:])
        return _join(header, lines)

    def from_text(text):
        out = bytearray()
        for number, tokens in _content(text):
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
                value = _number(tokens[2], where) - (0 if base is None else base)
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
    for number, tokens in _content(text):
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
    for number, tokens in _content(text):
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
    for number, tokens in _content(text):
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
    for number, tokens in _content(text):
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
        for number, tokens in _content(text):
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
        for number, tokens in _content(text):
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
    for number, tokens in _content(text):
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
    for number, tokens in _content(text):
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
    for number, tokens in _content(text):
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
    for number, tokens in _content(text):
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
