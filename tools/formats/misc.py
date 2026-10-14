"""
Name:        misc.py
Description: the formats of the program data no other family takes: texts, fonts, the attract demos'
             inputs, the sprite frame records, number tables and the test mode's tables
Version:     0.1.0
Created:     14-10-2026
Modified:    14-10-2026
Author:      Sebastian Toyos

Each format is a pair to_text(bytes) -> text and from_text(text) -> bytes with from_text(to_text(data)) ==
data for any bytes: what a format cannot read as its records stays in the text as 'bytes' lines (hex), so
a piece that holds a table and some bytes no one reads comes back whole. Lines starting with '#' are
comments, ignored on the way back.

The texts:
- text_list: print_text_list's records (print_text, 0995F4): the text map cell (a halfword: 31F000 + row x
  0x80 + column x 2, 64 cells a row), the attribute (the cell's high byte: palette x 2, bit 0 the
  character's bit 8), the characters, a 0. A 0 halfword ends a list. One record a line:
      row 1 col 15 attr 0x18 "TEST MODE"
      end
- text_here: print_text_here / print_text_lines records with no cell: the attribute, the characters, a 0;
  print_text_lines starts the next text row at a character with bit 7 set (FF in the game's texts): each
  further quoted line of a record follows an FF.
- c_string: characters ended by a 0 (queue_string_at's strings, the test mode's cursor characters).
- text_cells: text map cells (u16) as row and column.
- text_pointers: tables of pointers (u32) to texts: the program address and, where it points at a named
  place of symbols.txt, its name (the address is what goes back; a name alone is read too).
The characters are the text layer's (load_text_font): ASCII from 20 to 7E, written as such; any other
byte is \\xNN (and \\" \\\\ for the quote and the backslash).

Number tables (uNxK / sNxK): K values a line, unsigned in hex, signed in decimal; a line starts with the
index of its first value ('NNN:', ignored on the way back).

The rest: object_types, the text font and the fraction digits (pixels as characters), slope pairs, bounce
records, sound commands, test mode cursor lists, C.R.T. test bars, coin settings, the attract demos'
recordings, the sprite frame records, tile set lists and runs of one byte value.

The program ROM is little endian (the V60's). Names of pointers come from symbols.txt.
"""
import re
import struct
from bisect import bisect_right
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
DATA_AREA = (0x100000, 0x180000)


# ---- names of symbols.txt ----

_NAMES = {}


def _names():
    """{'address': {address: name}, 'name': {name: address}, 'places': sorted addresses of the data area's
    named places} from symbols.txt's func, code, data and table lines."""
    if _NAMES:
        return _NAMES
    by_address, by_name, places = {}, {}, []
    path = ROOT / "symbols.txt"
    lines = path.read_text(encoding="utf-8").splitlines() if path.exists() else []
    for line in lines:
        match = re.match(r"([0-9A-F]{6})\s+(func|code|data|table)\s+(\w+)", line)
        if not match:
            continue
        address, kind, name = int(match.group(1), 16), match.group(2), match.group(3)
        if address not in by_address:
            by_address[address] = name
            if kind in ("data", "table") and DATA_AREA[0] <= address < DATA_AREA[1]:
                places.append(address)
        by_name.setdefault(name, address)
    _NAMES.update({"address": by_address, "name": by_name, "places": sorted(places)})
    return _NAMES


def name_of(value):
    """The name of program address 'value' (name+0xNN inside a named piece of the data area), or None."""
    names = _names()
    name = names["address"].get(value)
    if name is not None and names["name"].get(name) == value:
        return name
    if DATA_AREA[0] <= value < DATA_AREA[1]:
        index = bisect_right(names["places"], value) - 1
        if index >= 0:
            base = names["places"][index]
            name = names["address"][base]
            if names["name"].get(name) == base:
                return f"{name}+0x{value - base:X}"
    return None


def _pointer_text(value):
    """A pointer as its address and, where it has one, its name."""
    name = name_of(value)
    return f"0x{value:06X}" + (f" {name}" if name else "")


def _pointer_value(token):
    """A pointer from its address (0x...) or its name (name, name+0xNN)."""
    if token.lower().startswith("0x"):
        return int(token, 16)
    name, _, offset = token.partition("+")
    names = _names()
    if name not in names["name"]:
        raise ValueError(f"unknown name '{name}'")
    return names["name"][name] + (_number(offset, 16) if offset else 0)


# ---- helpers ----

def _number(token, base=10):
    """A number: 0x... is hex, anything else in 'base'; a sign is allowed."""
    token = token.strip()
    sign = -1 if token.startswith("-") else 1
    token = token.lstrip("+-")
    if token.lower().startswith("0x"):
        return sign * int(token[2:], 16)
    return sign * int(token, base)


def _lines(text):
    """The lines that matter: stripped, without comments (a line starting with '#') or blank lines."""
    for line in text.split("\n"):
        line = line.strip()
        if line and not line.startswith("#"):
            yield line


def _bytes_lines(data, keyword="bytes"):
    """Bytes as 'bytes XX XX ...' lines, sixteen a line."""
    return [keyword + " " + " ".join(f"{byte:02X}" for byte in data[at:at + 16]) for at in range(0, len(data), 16)]


def _bytes_value(line):
    """The bytes of a 'bytes XX XX ...' line."""
    return bytes(int(token, 16) for token in line.split()[1:])


def _finish(lines):
    return "\n".join(lines) + "\n"


def _quote(data):
    """Characters as a quoted string: ASCII 20-7E as such, the rest \\xNN."""
    out = []
    for byte in data:
        if byte == 0x22:
            out.append('\\"')
        elif byte == 0x5C:
            out.append("\\\\")
        elif 0x20 <= byte < 0x7F:
            out.append(chr(byte))
        else:
            out.append(f"\\x{byte:02X}")
    return '"' + "".join(out) + '"'


def _unquote(line, at):
    """The bytes of the quoted string that starts at line[at], and where it ends (past the quote)."""
    if line[at] != '"':
        raise ValueError(f"no string at: {line}")
    out = bytearray()
    index = at + 1
    while True:
        if index >= len(line):
            raise ValueError(f"unterminated string: {line}")
        char = line[index]
        if char == '"':
            return bytes(out), index + 1
        if char == "\\":
            follower = line[index + 1]
            if follower == "x":
                out.append(int(line[index + 2:index + 4], 16))
                index += 4
            else:
                out.append(ord(follower))
                index += 2
            continue
        out.append(ord(char))
        index += 1


def _string_of(line):
    """The bytes of the first quoted string of 'line'."""
    return _unquote(line, line.index('"'))[0]


# ---- number tables ----

NUMBER_KINDS = {"u8": (1, "B"), "s8": (1, "b"), "u16": (2, "H"), "s16": (2, "h"), "u32": (4, "I")}


def _number_table(kind, per_line):
    """A table of 'kind' values, 'per_line' a line: unsigned in hex, signed in decimal."""
    size, code = NUMBER_KINDS[kind]
    signed = kind.startswith("s")

    def to_text(data):
        count = len(data) // size
        values = struct.unpack(f"<{count}{code}", data[:count * size])
        digits = len(f"{max(count - 1, 0):X}")
        lines = [f"# {kind} values, {'decimal' if signed else 'hex'}; the index of each line's first value in hex"]
        for at in range(0, count, per_line):
            chunk = values[at:at + per_line]
            text = " ".join(f"{value:6d}" if signed else f"{value:0{size * 2}X}" for value in chunk)
            lines.append(f"{at:0{digits}X}: {text}")
        lines += _bytes_lines(data[count * size:])
        return _finish(lines)

    def from_text(text):
        out = bytearray()
        for line in _lines(text):
            if line.startswith("bytes"):
                out += _bytes_value(line)
                continue
            for token in line.split():
                if not token.endswith(":"):
                    out += struct.pack("<" + code, _number(token, 10 if signed else 16))
        return bytes(out)

    return to_text, from_text


# ---- texts ----

def _cell_text(position):
    """A text map cell (the halfword print_text adds to 310000) as row and column, or as a number."""
    if 0xF000 <= position <= 0xFFFF and position % 2 == 0:
        return f"row {(position - 0xF000) >> 7} col {(position & 0x7F) >> 1}"
    return f"cell 0x{position:04X}"


def _cell_value(line):
    """The cell of a 'row R col C' or 'cell 0xNNNN' at the start of 'line', and the rest of the line."""
    match = re.match(r"row\s+(\d+)\s+col\s+(\d+)\s*(.*)$", line)
    if match:
        row, column = int(match.group(1)), int(match.group(2))
        if row > 31 or column > 63:
            raise ValueError(f"no such text cell: {line}")
        return 0xF000 + row * 0x80 + column * 2, match.group(3)
    match = re.match(r"cell\s+(\S+)\s*(.*)$", line)
    if match:
        return _number(match.group(1), 16), match.group(2)
    raise ValueError(f"no text cell: {line}")


def text_list_to_text(data):
    lines = ["# print_text_list records: text cell (row 0-31, column 0-63 of the text map at 31F000), attribute",
             "# (the cell's high byte: palette x 2 + the character's bit 8), the characters; 'end' ends a list (0000)"]
    at = 0
    while at < len(data):
        if data[at:at + 2] == b"\0\0":
            lines.append("end")
            at += 2
            continue
        if at + 3 <= len(data):
            position = data[at] | data[at + 1] << 8
            stop = data.find(b"\0", at + 3)
            if stop >= 0 and 0xF000 <= position and position % 2 == 0:
                lines.append(f"{_cell_text(position)} attr 0x{data[at + 2]:02X} {_quote(data[at + 3:stop])}")
                at = stop + 1
                continue
        lines.append("# not text records (no reader known):")
        lines += _bytes_lines(data[at:])
        break
    return _finish(lines)


def text_list_from_text(text):
    out = bytearray()
    for line in _lines(text):
        if line == "end":
            out += b"\0\0"
        elif line.startswith("bytes"):
            out += _bytes_value(line)
        else:
            position, rest = _cell_value(line)
            match = re.match(r"attr\s+(\S+)\s+", rest)
            if not match:
                raise ValueError(f"no attribute: {line}")
            out += struct.pack("<HB", position, _number(match.group(1), 16)) + _string_of(rest) + b"\0"
    return bytes(out)


def text_here_to_text(data):
    lines = ["# print_text_here / print_text_lines texts: attribute (palette x 2 + the character's bit 8), the",
             "# characters, a 0; a further quoted line of the same text follows an FF (print_text_lines: next row)"]
    at = 0
    while at < len(data):
        stop = data.find(b"\0", at + 1)
        if stop < 0:
            lines += _bytes_lines(data[at:])
            break
        parts = data[at + 1:stop].split(b"\xFF")
        lines.append(f"attr 0x{data[at]:02X} {_quote(parts[0])}")
        lines += [f"          {_quote(part)}" for part in parts[1:]]
        at = stop + 1
    return _finish(lines)


def text_here_from_text(text):
    out = bytearray()
    open_text = False
    for line in _lines(text):
        if line.startswith('"'):
            if not open_text:
                raise ValueError(f"a text line with no attr before it: {line}")
            out += b"\xFF" + _string_of(line)
            continue
        if open_text:
            out += b"\0"
            open_text = False
        if line.startswith("bytes"):
            out += _bytes_value(line)
        elif line.startswith("attr"):
            out.append(_number(line.split()[1], 16))
            out += _string_of(line)
            open_text = True
        else:
            raise ValueError(f"not a text line: {line}")
    if open_text:
        out += b"\0"
    return bytes(out)


def c_string_to_text(data):
    lines = ["# strings ended by a 0, one a line"]
    at = 0
    while at < len(data):
        stop = data.find(b"\0", at)
        if stop < 0:
            lines += _bytes_lines(data[at:])
            break
        lines.append(_quote(data[at:stop]))
        at = stop + 1
    return _finish(lines)


def c_string_from_text(text):
    out = bytearray()
    for line in _lines(text):
        if line.startswith("bytes"):
            out += _bytes_value(line)
        else:
            out += _string_of(line) + b"\0"
    return bytes(out)


def text_cells_to_text(data):
    lines = ["# text map cells (halfwords: 31F000 + row x 0x80 + column x 2), one a line"]
    count = len(data) // 2
    for index in range(count):
        lines.append(_cell_text(data[index * 2] | data[index * 2 + 1] << 8))
    lines += _bytes_lines(data[count * 2:])
    return _finish(lines)


def text_cells_from_text(text):
    out = bytearray()
    for line in _lines(text):
        if line.startswith("bytes"):
            out += _bytes_value(line)
        else:
            out += struct.pack("<H", _cell_value(line)[0])
    return bytes(out)


def text_pointers_to_text(data):
    lines = ["# pointers (u32), one a line: the program address, and its name in symbols.txt where it has one"]
    count = len(data) // 4
    for index in range(count):
        lines.append(_pointer_text(struct.unpack_from("<I", data, index * 4)[0]))
    lines += _bytes_lines(data[count * 4:])
    return _finish(lines)


def text_pointers_from_text(text):
    out = bytearray()
    for line in _lines(text):
        if line.startswith("bytes"):
            out += _bytes_value(line)
        else:
            out += struct.pack("<I", _pointer_value(line.split()[0]))
    return bytes(out)


# ---- fonts ----

TEXT_FONT_CHARACTERS = 0x9A     # load_text_font copies 0x9A0 halfwords: 0x9A characters of 32 bytes
EXPANDED_CHARACTERS = 0x5B      # expand_font turns the first 0x5B 1-bit characters into characters 9A-F4
PENS = ".123456789ABCDEF"


def _pen_rows(data):
    """32 bytes of a 4-bit character as 8 rows of 8 pens (the high nibble first)."""
    return ["".join(PENS[byte >> 4] + PENS[byte & 15] for byte in data[row * 4:row * 4 + 4]) for row in range(8)]


def _bit_row(byte):
    """A 1-bit row, the high bit leftmost: X set, . clear."""
    return "".join("X" if byte & (0x80 >> bit) else "." for bit in range(8))


def _is_row(line):
    """A row of 8 pixels: pens (. and hex digits) or bits (X and .)."""
    return len(line) == 8 and all(char in ".X0123456789ABCDEFabcdef" for char in line)


def _rows_from_text(text, four_bit_header=None):
    """Rows of pixels back to bytes: the rows under a 'four_bit_header' line are 8 pens (4 bytes, the high
    nibble first; '.' or 0 for pen 0), the rows under any other header 8 bits (1 byte: X set, . clear, the
    high bit leftmost); 'bytes' lines as hex."""
    out = bytearray()
    four_bit = False
    for line in _lines(text):
        if line.startswith("bytes"):
            out += _bytes_value(line)
        elif not _is_row(line):
            four_bit = line.split()[0] == four_bit_header
        elif four_bit:
            pens = [0 if char in ".0" else int(char, 16) for char in line]
            out += bytes(pens[at] << 4 | pens[at + 1] for at in range(0, 8, 2))
        elif set(line) <= set(".X"):
            out.append(sum(0x80 >> bit for bit, char in enumerate(line) if char == "X"))
        else:
            raise ValueError(f"a 1-bit row is X and .: {line}")
    return bytes(out)


def text_font_to_text(data):
    lines = ["# text_font (load_text_font): 9A characters of 8 x 8 pixels, 4 bits a pixel (pens: . for 0, then",
             "# 1-9 A-F), copied to characters 00-99 of the text layer; then 1-bit characters by ASCII code (X set,",
             "# . clear; the high bit leftmost), of which expand_font turns the first 5B (00-5A) into characters",
             "# 9A-F4 in pen 1"]
    count = min(TEXT_FONT_CHARACTERS, len(data) // 32)
    for number in range(count):
        lines.append(f"char 0x{number:02X}")
        lines += _pen_rows(data[number * 32:number * 32 + 32])
    at = count * 32
    number = 0
    while at + 8 <= len(data):
        shown = f" '{chr(number)}'" if 0x21 <= number < 0x7F else ""
        expanded = f" -> character 0x{TEXT_FONT_CHARACTERS + number:02X}" if number < EXPANDED_CHARACTERS else ""
        lines.append(f"bitmap 0x{number:02X}{shown}{expanded}")
        lines += [_bit_row(byte) for byte in data[at:at + 8]]
        at += 8
        number += 1
    lines += _bytes_lines(data[at:])
    return _finish(lines)


def text_font_from_text(text):
    return _rows_from_text(text, "char")


def fraction_font_to_text(data):
    lines = ["# fraction_digit_font (build_fraction_glyph): 1-bit rows (X set, . clear; the digits in the low",
             "# four pixels); the digits 0-9 of 8 rows, 4 rows no one reads, then the slash ORed over the denominator"]
    blocks = [(index * 8, f"digit {index}") for index in range(10)] + [(80, "gap"), (84, "slash")]
    if len(data) != 92:
        blocks = [(at, f"glyph {at // 8}") for at in range(0, len(data), 8)]
    for index, (at, title) in enumerate(blocks):
        end = blocks[index + 1][0] if index + 1 < len(blocks) else len(data)
        lines.append(title)
        lines += [_bit_row(byte) for byte in data[at:end]]
    return _finish(lines)


def fraction_font_from_text(text):
    return _rows_from_text(text)


# ---- records ----

def object_types_to_text(data):
    lines = ["# object_types (spawn_objects_by_type): [type] spawn_count (how many object blocks it takes, a word)",
             "# and the handler the objects start in (its address, and its name)"]
    count = len(data) // 8
    for index in range(count):
        spawn_count, handler = struct.unpack_from("<II", data, index * 8)
        lines.append(f"type {index:2d}  count {spawn_count}  handler {_pointer_text(handler)}")
    lines += _bytes_lines(data[count * 8:])
    return _finish(lines)


def object_types_from_text(text):
    out = bytearray()
    for line in _lines(text):
        if line.startswith("bytes"):
            out += _bytes_value(line)
            continue
        match = re.match(r"type\s+\S+\s+count\s+(\S+)\s+handler\s+(\S+)", line)
        if not match:
            raise ValueError(f"not an object type: {line}")
        out += struct.pack("<II", _number(match.group(1)), _pointer_value(match.group(2)))
    return bytes(out)


def slope_pairs_to_text(data):
    lines = ["# slope_speed_direction_pairs: per slope (floor_slope_direction_tables offsets: +0x00, +0x40 ...) 16",
             "# pairs by direction >> 6: the speed factor (0x1000 = 1.0) and the direction"]
    count = len(data) // 4
    for index in range(count):
        if index % 16 == 0:
            lines.append(f"# +0x{index * 4:02X}")
        factor, direction = struct.unpack_from("<HH", data, index * 4)
        lines.append(f"{index % 16:X}: factor 0x{factor:04X}  direction 0x{direction:04X}")
    lines += _bytes_lines(data[count * 4:])
    return _finish(lines)


def slope_pairs_from_text(text):
    out = bytearray()
    for line in _lines(text):
        if line.startswith("bytes"):
            out += _bytes_value(line)
            continue
        match = re.match(r"\S+:\s+factor\s+(\S+)\s+direction\s+(\S+)", line)
        if not match:
            raise ValueError(f"not a pair: {line}")
        out += struct.pack("<HH", _number(match.group(1), 16), _number(match.group(2), 16))
    return bytes(out)


def bounce_records_to_text(data):
    lines = ["# [bounce_direction] records: the direction, sprite_flags (40 = x flip) and the animation's offset",
             "# from the table's start"]
    count = len(data) // 6
    for index in range(count):
        direction, flags, animation = struct.unpack_from("<HHH", data, index * 6)
        lines.append(f"{index}: direction 0x{direction:04X}  sprite_flags 0x{flags:02X}  animation +0x{animation:02X}")
    lines += _bytes_lines(data[count * 6:])
    return _finish(lines)


def bounce_records_from_text(text):
    out = bytearray()
    for line in _lines(text):
        if line.startswith("bytes"):
            out += _bytes_value(line)
            continue
        match = re.match(r"\S+:\s+direction\s+(\S+)\s+sprite_flags\s+(\S+)\s+animation\s+\+?(\S+)", line)
        if not match:
            raise ValueError(f"not a bounce record: {line}")
        out += struct.pack("<HHH", *(_number(match.group(group), 16) for group in (1, 2, 3)))
    return bytes(out)


SOUND_GROUPS = {0x80: "control", 0x81: "music", 0x82: "effects", 0x83: "a third set"}


def sound_commands_to_text(data):
    lines = ["# sound_commands: by sound id, the command for the sound board: the group (the low byte, to 700701:",
             "# 80 control, 81 music, 82 effects, 83 a third set) and the number in the group (the high byte, to 700700)"]
    count = len(data) // 2
    for index in range(count):
        group, number = data[index * 2], data[index * 2 + 1]
        lines.append(f"0x{index:02X}  group 0x{group:02X}  number 0x{number:02X}")
    lines += _bytes_lines(data[count * 2:])
    return _finish(lines)


def sound_commands_from_text(text):
    out = bytearray()
    for line in _lines(text):
        if line.startswith("bytes"):
            out += _bytes_value(line)
            continue
        match = re.match(r"\S+\s+group\s+(\S+)\s+number\s+(\S+)", line)
        if not match:
            raise ValueError(f"not a sound command: {line}")
        out += bytes([_number(match.group(1), 16), _number(match.group(2), 16)])
    return bytes(out)


def cursor_positions_to_text(data):
    lines = ["# (column, row) text cells, one place a line; 'end' is the FF that ends a list"]
    at = 0
    while at < len(data):
        if data[at] == 0xFF:
            lines.append("end")
            at += 1
        elif at + 2 <= len(data):
            lines.append(f"col {data[at]} row {data[at + 1]}")
            at += 2
        else:
            lines += _bytes_lines(data[at:])
            break
    return _finish(lines)


def cursor_positions_from_text(text):
    out = bytearray()
    for line in _lines(text):
        if line == "end":
            out.append(0xFF)
        elif line.startswith("bytes"):
            out += _bytes_value(line)
        else:
            match = re.match(r"col\s+(\S+)\s+row\s+(\S+)", line)
            if not match:
                raise ValueError(f"not a place: {line}")
            out += bytes([_number(match.group(1)), _number(match.group(2))])
    return bytes(out)


def crt_bars_to_text(data):
    lines = ["# crt_test_color_bars: a row of 32 bars a line (test_crt): x, y of the first bar, its colour (+1 a bar)"]
    count = len(data) // 5
    for index in range(count):
        x, y, color = struct.unpack_from("<HHB", data, index * 5)
        lines.append(f"x 0x{x:02X}  y 0x{y:02X}  color 0x{color:02X}")
    lines += _bytes_lines(data[count * 5:])
    return _finish(lines)


def crt_bars_from_text(text):
    out = bytearray()
    for line in _lines(text):
        if line.startswith("bytes"):
            out += _bytes_value(line)
            continue
        match = re.match(r"x\s+(\S+)\s+y\s+(\S+)\s+color\s+(\S+)", line)
        if not match:
            raise ValueError(f"not a bar row: {line}")
        out += struct.pack("<HHB", *(_number(match.group(group), 16) for group in (1, 2, 3)))
    return bytes(out)


def coin_settings_to_text(data):
    lines = ["# coin_setting_values: one COIN/CREDIT SETTING a line (apply_coin_setting): coins per credit, bonus",
             "# adder, chute 1 multiplier, chute 2 multiplier (setting_coins_per_credit, setting_bonus_adder ...)"]
    count = len(data) // 4
    for index in range(count):
        values = data[index * 4:index * 4 + 4]
        lines.append(f"setting {index + 1:2d}  coins {values[0]}  bonus {values[1]}  chute_1 {values[2]}  chute_2 {values[3]}")
    lines += _bytes_lines(data[count * 4:])
    return _finish(lines)


def coin_settings_from_text(text):
    out = bytearray()
    for line in _lines(text):
        if line.startswith("bytes"):
            out += _bytes_value(line)
            continue
        match = re.match(r"setting\s+\S+\s+coins\s+(\S+)\s+bonus\s+(\S+)\s+chute_1\s+(\S+)\s+chute_2\s+(\S+)", line)
        if not match:
            raise ValueError(f"not a coin setting: {line}")
        out += bytes(_number(match.group(group)) for group in (1, 2, 3, 4))
    return bytes(out)


DEMO_HEADER = 16
DEMO_FRAME = 7
DEMO_FRAMES = 0x23F     # attract_demo_1/2 last 0x240 frames (attract_timer); the game reads 0x23F frames of
                        # the recording (the reference emulator, tests/controls/attract.txt), the rest is not
                        # a recording (left-over bytes in demo 1, zeros in demo 2)


def demo_inputs_to_text(data):
    lines = ["# attract demo recording: a 16-byte header (start_stage reads the stage at +0, stage_start_section",
             "# at +2), then a frame a line: the button byte (play_demo_buttons: bits 0-2, players 1-3) and the",
             "# three trackballs' x and y (play_demo_trackballs: signed counts), in decimal; the first number is the",
             "# frame (hex, ignored on the way back)"]
    if len(data) < DEMO_HEADER:
        return _finish(lines + _bytes_lines(data))
    stage, section = struct.unpack_from("<HH", data)
    lines.append(f"stage 0x{stage:02X}")
    lines.append(f"start_section 0x{section:02X}")
    lines += _bytes_lines(data[4:DEMO_HEADER], "header")
    frames = min(DEMO_FRAMES, (len(data) - DEMO_HEADER) // DEMO_FRAME)
    lines.append("# frame  buttons   p1 x  p1 y   p2 x  p2 y   p3 x  p3 y")
    for frame in range(frames):
        record = data[DEMO_HEADER + frame * DEMO_FRAME:DEMO_HEADER + frame * DEMO_FRAME + DEMO_FRAME]
        moves = struct.unpack_from("<6b", record, 1)
        lines.append(f"{frame:04X}     {record[0]:02X}     " + "  ".join(f"{move:4d} {moves[index + 1]:4d}"
                                                                     for index, move in enumerate(moves) if index % 2 == 0))
    rest = data[DEMO_HEADER + frames * DEMO_FRAME:]
    if rest:
        lines.append("# past the 0x23F frames the demo reads: no one reads these")
        lines += _bytes_lines(rest)
    return _finish(lines)


def demo_inputs_from_text(text):
    out = bytearray()
    for line in _lines(text):
        words = line.split()
        if words[0] == "stage":
            out += struct.pack("<H", _number(words[1], 16))
        elif words[0] == "start_section":
            out += struct.pack("<H", _number(words[1], 16))
        elif words[0] in ("header", "bytes"):
            out += _bytes_value(line)
        else:
            if len(words) != 8:
                raise ValueError(f"a frame is its number, the buttons and six moves: {line}")
            out.append(int(words[1], 16))
            out += struct.pack("<6b", *(_number(word) for word in words[2:]))
    return bytes(out)


SPRITE_RECORD = 8


def sprite_frames_to_text(data):
    lines = ["# sprite_frame_records: a frame a line, named as its PNG in assets/sprites (frame_XXXX: the record's",
             "# offset, an object's sprite_frame): width and height in pixels, and where its graphics start in the",
             "# sprite ROM: the bank (0-3: record word 2, bits 11 and 14) and the address in 32-bit words (20 bits:",
             "# record word 1's top nibble, then word 3); frames.json's word is bank << 20 | address"]
    count = len(data) // SPRITE_RECORD
    for index in range(count):
        width, height_word, bank_word, address_low = struct.unpack_from("<HHHH", data, index * SPRITE_RECORD)
        bank = (bank_word >> 11) & 1 | (bank_word >> 13) & 2
        address = (height_word & 0xF000) << 4 | address_low
        line = (f"frame_{index * SPRITE_RECORD:04X}  width {width:3d}  height {height_word & 0xFF:3d}  "
                f"bank {bank}  address 0x{address:05X}")
        if height_word & 0x0F00:
            line += f"  height_bits 0x{height_word & 0x0F00:04X}"
        if bank_word & ~0x4800:
            line += f"  bank_bits 0x{bank_word & ~0x4800:04X}"
        lines.append(line)
    lines += _bytes_lines(data[count * SPRITE_RECORD:])
    return _finish(lines)


def sprite_frames_from_text(text):
    out = bytearray()
    for line in _lines(text):
        if line.startswith("bytes"):
            out += _bytes_value(line)
            continue
        words = line.split()
        fields = {words[at]: _number(words[at + 1]) for at in range(1, len(words) - 1, 2)}
        bank, address = fields["bank"], fields["address"]
        height_word = fields["height"] | fields.get("height_bits", 0) | (address >> 4) & 0xF000
        bank_word = (bank & 1) << 11 | (bank & 2) << 13 | fields.get("bank_bits", 0)
        out += struct.pack("<HHHH", fields["width"], height_word, bank_word, address & 0xFFFF)
    return bytes(out)


TILE_SET_BANK = 0x100000    # get_tile_set_list_bank / get_tile_set_script_bank add it for the stages that bank


def tile_set_lists_to_text(data):
    lines = ["# tile set lists (load_tile_set): pointers (u32) as stored, without the +100000 the game adds",
             "# (get_tile_set_list_bank, get_tile_set_script_bank); the comment names the place with it; 'end' is",
             "# FFFFFFFF, the end of a list"]
    count = len(data) // 4
    for index in range(count):
        value = struct.unpack_from("<I", data, index * 4)[0]
        if value == 0xFFFFFFFF:
            lines.append("end")
            continue
        name = name_of(value + TILE_SET_BANK) if value < TILE_SET_BANK else None
        lines.append(f"0x{value:06X}" + (f"  # {name}" if name else ""))
    lines += _bytes_lines(data[count * 4:])
    return _finish(lines)


def tile_set_lists_from_text(text):
    out = bytearray()
    for line in _lines(text):
        if line == "end":
            out += b"\xFF\xFF\xFF\xFF"
        elif line.startswith("bytes"):
            out += _bytes_value(line)
        else:
            out += struct.pack("<I", _number(line.split()[0], 16))
    return bytes(out)


FILL_RUN = 16   # runs of one value this long or longer are written as 'fill'


def fill_to_text(data):
    lines = ["# 'fill XX COUNT': COUNT bytes XX (count in decimal); other bytes as 'bytes' lines"]
    at = 0
    loose = bytearray()
    while at < len(data):
        end = at
        while end < len(data) and data[end] == data[at]:
            end += 1
        if end - at >= FILL_RUN:
            lines += _bytes_lines(loose)
            loose = bytearray()
            lines.append(f"fill {data[at]:02X} {end - at}")
        else:
            loose += data[at:end]
        at = end
    lines += _bytes_lines(loose)
    return _finish(lines)


def fill_from_text(text):
    out = bytearray()
    for line in _lines(text):
        if line.startswith("bytes"):
            out += _bytes_value(line)
        elif line.startswith("fill"):
            words = line.split()
            out += bytes([int(words[1], 16)]) * _number(words[2])
        else:
            raise ValueError(f"not a fill line: {line}")
    return bytes(out)


# ---- the formats ----

FORMATS = {
    "text_list": (".txt", text_list_to_text, text_list_from_text),
    "text_here": (".txt", text_here_to_text, text_here_from_text),
    "c_string": (".txt", c_string_to_text, c_string_from_text),
    "text_cells": (".txt", text_cells_to_text, text_cells_from_text),
    "text_pointers": (".txt", text_pointers_to_text, text_pointers_from_text),
    "text_font": (".txt", text_font_to_text, text_font_from_text),
    "fraction_font": (".txt", fraction_font_to_text, fraction_font_from_text),
    "object_types": (".txt", object_types_to_text, object_types_from_text),
    "slope_pairs": (".txt", slope_pairs_to_text, slope_pairs_from_text),
    "bounce_records": (".txt", bounce_records_to_text, bounce_records_from_text),
    "sound_commands": (".txt", sound_commands_to_text, sound_commands_from_text),
    "cursor_positions": (".txt", cursor_positions_to_text, cursor_positions_from_text),
    "crt_bars": (".txt", crt_bars_to_text, crt_bars_from_text),
    "coin_settings": (".txt", coin_settings_to_text, coin_settings_from_text),
    "demo_inputs": (".txt", demo_inputs_to_text, demo_inputs_from_text),
    "sprite_frames": (".txt", sprite_frames_to_text, sprite_frames_from_text),
    "tile_set_lists": (".txt", tile_set_lists_to_text, tile_set_lists_from_text),
    "fill": (".txt", fill_to_text, fill_from_text),
}
for _kind, _per_line in (("u8", 16), ("u8", 2), ("s8", 4), ("u16", 1), ("u16", 2), ("u16", 3), ("u16", 4),
                         ("u16", 8), ("s16", 2), ("s16", 8)):
    FORMATS[f"{_kind}x{_per_line}"] = (".txt",) + _number_table(_kind, _per_line)
