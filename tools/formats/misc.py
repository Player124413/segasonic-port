"""
Name:        misc.py
Description: the formats of the program data no other family takes: texts, fonts, the attract demos'
             inputs, the sprite frame records, number tables, the test mode's tables, the ranking, the map
             screen, the players' tables and the system's (vector table, EEPROM defaults, video set-up)
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
- texts: print_text records and print_text_here records one after the other, each as above;
  text_list_from_characters: the characters of a print_text_list record whose cell and attribute end the
  piece before (a quoted string alone on its line), then print_text_list records; start_prompt_texts:
  the offsets of the prompts, then their print_text_here records.
- c_string: characters ended by a 0 (queue_string_at's strings, the test mode's cursor characters).
- string_records: queue_string_record's records (screen x, y, palette, scale, then the characters).
- text_cells: text map cells (u16) as row and column; cells_and_padding: cells up to an FFFF, then the FF
  bytes that pad the chip.
- text_pointers: tables of pointers (u32) to texts: the program address and, where it points at a named
  place of symbols.txt, its name (the address is what goes back; a name alone is read too).
The characters are the text layer's (load_text_font): ASCII from 20 to 7E, written as such; any other
byte is \\xNN (and \\" \\\\ for the quote and the backslash). One reader (texts_reader) takes them all.

Number tables (uNxK / sNxK): K values a line, unsigned in hex, signed in decimal; a line starts with the
index of its first value ('NNN:', ignored on the way back); s16x2_head8: only the first 8 bytes are the
table, the rest is kept as hex.

The rest: object_types, the text font and the fraction digits (pixels as characters), a 1-bit picture
(bitmap_32), slope pairs, bounce records, sound commands, test mode cursor lists, C.R.T. test bars, coin
settings, the attract demos' recordings, the sprite frame records, tile set lists and runs of one byte
value; hit boxes and the handlers reset_objects_from_list starts (labelled records); the values init_mixer
and boot copy to the mixer and the interrupt controller ('reg ADDRESS VALUE'); the ranking (its default
records, row numbers in ranking letters, portrait frames, letter sprites), the map screen (wipes, marker
layouts, trails), the players' and the character chooser's tables, the ring tally, and the system's: the
vector table and the ROM checksums in it, the EEPROM defaults and default settings, the video and sprite
register set-up, the attract sound mask, the RAM test patterns. Those are records in columns
(tools/formats/common.py): a keyword, then 'field value' pairs, the index in the comment.

The program ROM is little endian (the V60's). Names of pointers come from symbols.txt: name_of here names
places of the data area at any distance (tile_set_lists needs it); the formats taken from the code chips
use common.name_of.
"""
import re
import struct
from bisect import bisect_right
from pathlib import Path

from formats import animations, common
from formats.common import (END, KEPT, POINT, WORK_RAM, Out, Rec, address_value, byte_table, content, fmt,
                            frame_text, frame_value, hex_lines, indexed, join, labelled_table, phased_formats,
                            phased_text, raw, run_of, single, tokens_of, u16_at)

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


def _number_table(kind, per_line, head=None):
    """A table of 'kind' values, 'per_line' a line: unsigned in hex, signed in decimal. With 'head', only
    the first 'head' bytes are the table: the rest is kept as 'bytes' lines."""
    size, code = NUMBER_KINDS[kind]
    signed = kind.startswith("s")

    def to_text(data):
        rest = b""
        if head is not None:
            data, rest = data[:head], data[head:]
        count = len(data) // size
        values = struct.unpack(f"<{count}{code}", data[:count * size])
        digits = len(f"{max(count - 1, 0):X}")
        lines = [f"# {kind} values, {'decimal' if signed else 'hex'}; the index of each line's first value in hex"]
        for at in range(0, count, per_line):
            chunk = values[at:at + per_line]
            text = " ".join(f"{value:6d}" if signed else f"{value:0{size * 2}X}" for value in chunk)
            lines.append(f"{at:0{digits}X}: {text}")
        lines += _bytes_lines(data[count * size:])
        if rest:
            lines += [f"# {KEPT}"] + _bytes_lines(rest)
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


TEXT_LIST_HEADER = [
    "# print_text_list records: text cell (row 0-31, column 0-63 of the text map at 31F000), attribute",
    "# (the cell's high byte: palette x 2 + the character's bit 8), the characters; 'end' ends a list (0000)"]
TEXT_HERE_HEADER = [
    "# print_text_here / print_text_lines texts: attribute (palette x 2 + the character's bit 8), the",
    "# characters, a 0; a further quoted line of the same text follows an FF (print_text_lines: next row)"]
NOT_TEXT = "# not text records (no reader known):"


def _text_record(data, at):
    """The print_text record at data[at:] as its line ('row R col C attr 0xAA "..."'), and its size; or None
    when no text record is there (a cell is F000 or more, even, and a 0 ends the characters)."""
    if at + 3 > len(data):
        return None
    position = data[at] | data[at + 1] << 8
    stop = data.find(b"\0", at + 3)
    if stop < 0 or position < 0xF000 or position % 2:
        return None
    return f"{_cell_text(position)} attr 0x{data[at + 2]:02X} {_quote(data[at + 3:stop])}", stop + 1 - at


def _here_record(data, at):
    """The print_text_here record at data[at:] as its lines (the attribute and the first line of the
    text, then each further line), and its size; or None when no 0 ends it."""
    stop = data.find(b"\0", at + 1)
    if stop < 0:
        return None
    parts = data[at + 1:stop].split(b"\xFF")
    return [f"attr 0x{data[at]:02X} {_quote(parts[0])}"] + [f"          {_quote(part)}" for part in parts[1:]], \
        stop + 1 - at


def _text_list_lines(data):
    lines = []
    at = 0
    while at < len(data):
        if data[at:at + 2] == b"\0\0":
            lines.append("end")
            at += 2
            continue
        found = _text_record(data, at)
        if found:
            lines.append(found[0])
            at += found[1]
            continue
        lines.append(NOT_TEXT)
        lines += _bytes_lines(data[at:])
        break
    return lines


def text_list_to_text(data):
    return _finish(TEXT_LIST_HEADER + _text_list_lines(data))


def text_list_from_characters_to_text(data):
    """A piece that starts with the characters of a print_text_list record (its cell and attribute end the
    piece before), then print_text_list records."""
    lines = ["# the characters (ended by a 0) of a print_text_list record whose cell and attribute end the piece",
             "# before, then print_text_list records: text cell (row 0-31, column 0-63 of the text map at 31F000),",
             "# attribute (the cell's high byte: palette x 2 + the character's bit 8), the characters"]
    stop = data.find(b"\0")
    if stop < 0:
        return _finish(lines + _bytes_lines(data))
    return _finish(lines + [_quote(data[:stop])] + _text_list_lines(data[stop + 1:]))


def text_here_to_text(data):
    lines = list(TEXT_HERE_HEADER)
    at = 0
    while at < len(data):
        found = _here_record(data, at)
        if found is None:
            lines += _bytes_lines(data[at:])
            break
        lines += found[0]
        at += found[1]
    return _finish(lines)


def texts_to_text(data):
    """print_text records and print_text_here records, one after the other."""
    lines = ["# texts: 'row R col C attr 0xAA \"...\"' a print_text record (the text map cell, the attribute, the",
             "# characters), 'attr 0xAA \"...\"' a print_text_here record (the attribute and the characters, at the",
             "# cell the code gives; a further quoted line of the same text follows an FF)"]
    at = 0
    while at < len(data):
        found = _text_record(data, at)
        if found:
            lines.append(found[0])
            at += found[1]
            continue
        found = _here_record(data, at)
        if found is None:
            lines.append(NOT_TEXT)
            lines += _bytes_lines(data[at:])
            break
        lines += found[0]
        at += found[1]
    return _finish(lines)


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


def texts_reader(*recs):
    """from_text of the text formats: print_text records ('row R col C attr 0xAA "..."' or 'cell 0xNNNN
    attr ...'), 'end' (a 0 halfword), print_text_here records ('attr 0xAA "..."', then a line '"..."' for
    each further line of the text, after an FF), strings ended by a 0 ('"..."' with no text open), 'bytes'
    lines, and the lines of the records in columns 'recs' (common.Rec)."""
    table = {rec.keyword: rec for rec in recs}

    def from_text(text):
        out = bytearray()
        open_text = False       # a print_text_here record whose 0 is still to come
        for line in _lines(text):
            if line.startswith('"'):
                out += b"\xFF" + _string_of(line) if open_text else _string_of(line) + b"\0"
                continue
            if open_text:
                out += b"\0"
                open_text = False
            keyword = line.split()[0]
            if keyword == "bytes":
                out += _bytes_value(line)
            elif keyword == "end":
                out += b"\0\0"
            elif keyword == "attr":
                out.append(_number(line.split()[1], 16))
                out += _string_of(line)
                open_text = True
            elif keyword in table:
                out += table[keyword].pack(tokens_of(line)[1:], line.strip())
            else:
                position, rest = _cell_value(line)
                match = re.match(r"attr\s+(\S+)\s+", rest)
                if not match:
                    raise ValueError(f"no attribute: {line}")
                out += struct.pack("<HB", position, _number(match.group(1), 16)) + _string_of(rest) + b"\0"
        if open_text:
            out += b"\0"
        return bytes(out)

    return from_text


texts_from_text = texts_reader()


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


# ---- the formats ----

FORMATS = {
    "text_list": (".txt", text_list_to_text, texts_from_text),
    "text_list_from_characters": (".txt", text_list_from_characters_to_text, texts_from_text),
    "text_here": (".txt", text_here_to_text, texts_from_text),
    "texts": (".txt", texts_to_text, texts_from_text),
    "c_string": (".txt", c_string_to_text, texts_from_text),
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
}
for _kind, _per_line in (("u8", 16), ("u8", 2), ("s8", 2), ("s8", 4), ("u16", 1), ("u16", 2), ("u16", 3),
                         ("u16", 4), ("u16", 8), ("s16", 2), ("s16", 3), ("s16", 8), ("u32", 4)):
    FORMATS[f"{_kind}x{_per_line}"] = (".txt",) + _number_table(_kind, _per_line)
FORMATS["s16x2_head8"] = (".txt",) + _number_table("s16", 2, head=8)


# ---- a 1-bit picture ----

def bitmap_32_to_text(data):
    lines = ["# 1-bit rows of 32 pixels (4 bytes a row, the high bit of the first byte leftmost): X set, . clear"]
    count = len(data) // 4
    for row in range(count):
        bits = int.from_bytes(data[row * 4:row * 4 + 4], "big")
        lines.append("".join("X" if bits & (1 << (31 - bit)) else "." for bit in range(32)))
    lines += hex_lines(data[count * 4:], KEPT)
    return "".join(line + "\n" for line in lines)


def bitmap_32_from_text(text):
    out = bytearray()
    for line_number, tokens in content(text):
        where = f"line {line_number}"
        found = raw(tokens, where)
        if found is not None:
            out += found
            continue
        row = tokens[0]
        if len(tokens) != 1 or len(row) != 32 or set(row) - set("X."):
            raise ValueError(f"{where}: a row is 32 characters, X or .")
        out += sum(1 << (31 - bit) for bit, char in enumerate(row) if char == "X").to_bytes(4, "big")
    return bytes(out)


FORMATS["bitmap_32"] = (".txt", bitmap_32_to_text, bitmap_32_from_text)


# ---- texts drawn as sprites (queue_string_record) ----

def string_records_to_text(data):
    lines = []
    at = 0
    while at < len(data):
        stop = data.find(b"\0", at + 8)
        if at + 8 > len(data) or stop < 0:
            break
        x, y, palette, scale = struct.unpack_from("<4H", data, at)
        lines.append(f"string  x 0x{x:02X}  y 0x{y:02X}  palette 0x{palette:04X}  scale 0x{scale:04X}  "
                     + _quote(data[at + 8:stop]))
        at = stop + 1
    lines += hex_lines(data[at:], KEPT)
    header = ["queue_string_record's records: screen x and y, the palette word and the scale word, which",
              "queue_string puts in string_queue for draw_queued_strings, then the characters, ended by a 0."]
    return join(header, lines)


def string_records_from_text(text):
    out = bytearray()
    for line_number, line in enumerate(text.split("\n"), 1):
        where = f"line {line_number}"
        stripped = line.strip()
        if not stripped or stripped.startswith("#"):
            continue
        tokens = stripped.split()
        found = raw(tokens, where)
        if found is not None:
            out += found
            continue
        quote = stripped.find('"')
        if tokens[0] != "string" or quote < 0:
            raise ValueError(f"{where}: 'string x X y Y palette P scale S \"TEXT\"' expected")
        fields = stripped[:quote].split()[1:]
        pairs = dict(zip(fields[0::2], fields[1::2]))
        try:
            values = [common.number(pairs[name], where) for name in ("x", "y", "palette", "scale")]
        except KeyError as missing:
            raise ValueError(f"{where}: {missing} is missing") from None
        out += struct.pack("<4H", *(value & 0xFFFF for value in values)) + _string_of(stripped) + b"\0"
    return bytes(out)


FORMATS["string_records"] = (".txt", string_records_to_text, string_records_from_text)


# ---- texts the code finds through tables, and the text map cells at the end of the chip at 080000 ----

_PROMPT_OFFSET = Rec("prompt", 2, [("text", 0, "H", ("off", 0x09FEDE))])
_PROMPT_OFFSET_2 = Rec("prompt", 2, [("text", 0, "H", ("off", 0x09FEE6))])


def _start_prompts_text(data):
    out = Out(data, ["start_prompt_texts (blink_start_prompt, draw_player_start_prompt, draw_attract_credits): by",
                     "prompt the offset of its text from this table (0 'Push start button', 1 'Insert more coin', 2",
                     "'Insert coin', 3 '2 credits start'); the texts are print_text_here records: the attribute,",
                     "then the lines (print_text_lines goes to the next row at each FF between them). The one-line",
                     "texts of common_start_prompt_texts come first."])
    for prompt in range(4):
        out.rec(_PROMPT_OFFSET_2, f"prompt {prompt}")
    while out.at < len(data):
        found = _here_record(data, out.at)
        if found is None:
            break
        out.comment(f"+{out.at:04X}")
        lines, size = found
        for index, line in enumerate(lines):
            out.special(line, size if index == 0 else 0)
    return out.done()


def cells_and_padding_to_text(data):
    """Text map cells up to an FFFF, then the FF bytes that pad the chip."""
    lines = ["# text map cells (halfwords: 31F000 + row x 0x80 + column x 2), one a line; then the FF bytes that",
             "# fill the chip up to the reset code at 09FFF0"]
    at = 0
    while at + 2 <= len(data) and data[at:at + 2] != b"\xFF\xFF":
        lines.append(_cell_text(data[at] | data[at + 1] << 8))
        at += 2
    return _finish(lines + _bytes_lines(data[at:]))


FORMATS.update({
    "common_start_prompt_texts": indexed(
        ["common_start_prompt_texts (erase_attract_credits, common chutes): by prompt the offset of its one-line",
         "text from this table (the texts are in start_prompt_texts)."], _PROMPT_OFFSET, "prompt"),
    "start_prompt_texts": (".txt", _start_prompts_text, texts_reader(_PROMPT_OFFSET_2)),
    "cells_and_padding": (".txt", cells_and_padding_to_text, text_cells_from_text),
})


# ---- the ranking ----

RANKING_ALPHABET = {**{code: chr(ord("a") + code) for code in range(26)},
                    **{0x20 + digit: str(digit) for digit in range(10)}, 0x2B: " "}
RANKING_CODES = {char: code for code, char in RANKING_ALPHABET.items()}


def _ranking_text(data):
    """Letter codes of the ranking (ranking_letter_sprites) as a quoted string: a-z, 0-9 and the blank as
    such, any other code \\xNN."""
    return '"' + "".join(RANKING_ALPHABET.get(code, f"\\x{code:02X}") for code in data) + '"'


def _ranking_value(token, where):
    """The letter codes of a quoted ranking string."""
    if len(token) < 2 or token[0] != '"' or token[-1] != '"':
        raise ValueError(f"{where}: '{token}' is not a quoted string")
    text = token[1:-1]
    out = bytearray()
    at = 0
    while at < len(text):
        if text[at] == "\\" and text[at + 1:at + 2] == "x":
            out.append(int(text[at + 2:at + 4], 16))
            at += 4
            continue
        if text[at] not in RANKING_CODES:
            raise ValueError(f"{where}: '{text[at]}' is no ranking letter")
        out.append(RANKING_CODES[text[at]])
        at += 1
    return bytes(out)


def _row_numbers_text(data):
    out = Out(data, ["ranking_row_numbers (build_ranking_display): per row, from the lowest (15) to the top, the",
                     "two ranking letter codes of its number (ranking_letter_sprites: a-z, 0-9, ' ' the blank)."])
    while out.at + 2 <= len(data):
        out.special(f"number {_ranking_text(data[out.at:out.at + 2])}", 2)
    return out.done()


def _default_ranking_text(data):
    out = Out(data, ["default_ranking (reset_ranking): the 15 records the ranking starts with, the lowest first:",
                     "rings, character (0 Sonic, 1 Ray, 2 Mighty), stages reached, the three letters of the name",
                     "(ASCII: ascii_to_ranking_letter turns a-z, ' ' and '3' into letter codes; any other",
                     "character hangs it) and a last byte copied as is."])
    while out.at + 8 <= len(data):
        record = data[out.at:out.at + 8]
        rings, character, stages = struct.unpack_from("<HBB", record)
        out.special(f"entry  rings {rings:4d}  character {character}  stages {stages}  name "
                    f"{_quote(record[4:7])}  last 0x{record[7]:02X}", 8)
    return out.done()


def _default_rank_special(tokens, where):
    fields = {}
    index = 0
    while index + 1 < len(tokens):
        fields[tokens[index]] = tokens[index + 1]
        index += 2
    try:
        name = _quoted(fields["name"], where)
        if len(name) != 3:
            raise ValueError(f"{where}: a name is three letters")
        return struct.pack("<HBB", common.number(fields["rings"], where), common.number(fields["character"], where),
                           common.number(fields["stages"], where)) + name \
            + bytes([common.number(fields["last"], where)])
    except KeyError as missing:
        raise ValueError(f"{where}: {missing.args[0]} is missing") from None


def _quoted(token, where):
    """The bytes of a quoted string token."""
    if not token.startswith('"') or not token.endswith('"') or len(token) < 2:
        raise ValueError(f"{where}: '{token}' is not a quoted string")
    return _unquote(token, 0)[0]


_PORTRAIT_LIST = Rec("character", 4, [("frames", 0, "I", "ptr")])


def _portrait_frames_text(data):
    out = Out(data, ["ranking_portrait_frames (ranking_row_update): per character (0 Sonic, 1 Ray, 2 Mighty) the",
                     "address of its 28 (hex) portrait frames: 14 for the rows, 14 more for the top row; then the",
                     "lists."])
    for character in range(3):
        out.rec(_PORTRAIT_LIST, ("Sonic", "Ray", "Mighty")[character])
    while out.at + 2 <= len(data):
        if (out.at - 12) % 80 == 0:
            out.comment(f"+{out.at:04X}")
        count = min(10, (len(data) - out.at) // 2)
        words = struct.unpack_from(f"<{count}H", data, out.at)
        out.special("frames " + " ".join(frame_text(word) for word in words), count * 2)
    return out.done()


def _frames_words_special(tokens, where):
    return b"".join(struct.pack("<H", frame_value(token, where) & 0xFFFF) for token in tokens)


FORMATS.update({
    "default_ranking": fmt(_default_ranking_text, specials={"entry": _default_rank_special}),
    "ranking_row_numbers": fmt(_row_numbers_text, specials={
        "number": lambda tokens, where: _ranking_value(tokens[0], where)}),
    "ranking_portrait_frames": fmt(_portrait_frames_text, _PORTRAIT_LIST, specials={"frames": _frames_words_special}),
    "ranking_initial_fields": indexed(
        ["ranking_initial_fields (enter_ranking_initials): by letter index, the entry block field the chosen",
         "letter goes into."], Rec("letter", 2, [("field", 0, "H", "x")]), "letter"),
    "ranking_letter_sprites": animations.sprite_list_format(
        ["ranking_letter_sprites: the sprite frame of each ranking letter code (the index): the ranking rows, the",
         "entry's letters, the character select countdown digits: a-z, 0-9 from 20, 2B the blank."],
        lambda code: f"'{RANKING_ALPHABET[code]}'" if code in RANKING_ALPHABET else None),
})


# ---- the map screen ----

_WIPE = Rec("stage", 6, [("left", 0, "h"), ("top", 2, "h"), ("frames", 4, "H")])
_MAP_POSITION = Rec("marker", 4, [("x", 0, "H"), ("y", 2, "H")])
_MARKER = Rec("marker", 12, [("priority", 0, "H"), ("x", 2, "H"), ("y", 4, "H"), ("sprite", 6, "H", "frame"),
                             ("palette", 8, "H", "x"), ("cleared_palette", 10, "H", "x")], phased=True)
_MARKERS_HEADER = [
    "map_marker_layouts (draw_map_stage_markers): 12 bytes a marker part: its priority (sprite layer), screen",
    "x, y, sprite, palette, and the palette once the stage is cleared. Records 7 on are the numbered badges",
    "(map_number_badge_sprites = + 6 in record 7: blink_current_map_marker reads the current stage's at",
    "+ 12 x stage_map_markers[stage]); the table is cut there, each piece holding the fields from there on.",
]
_BLINK_SPRITE = Rec("marker", 10, [("layer", 0, "H"), ("x", 2, "H"), ("y", 4, "H"), ("sprite", 6, "H", "frame"),
                                   ("palette", 8, "H", "x")])
_TRAIL_LIST = Rec("stage", 4, [("trail", 0, "I", "ptr")])
_TRAIL_FRAMES = Rec("trail", 2, [("frames", 0, "H")])


def _map_trails_text(data):
    out = Out(data, ["stage_map_trails (start_map_trail): per stage the address of its trail on the map",
                     "(FFFFFFFF: none, not above 0): the frames a segment takes, then the points (x, y) the dots go",
                     "through; 'end' (FFFF) closes a trail."])
    for stage in range(19):
        if not out.rec(_TRAIL_LIST, f"stage {stage:02X}"):
            break
    fresh = True
    while out.at < len(data):
        if fresh:
            out.blank()
            out.comment(f"+{out.at:04X}")
            if not out.rec(_TRAIL_FRAMES):
                break
            fresh = False
        elif u16_at(data, out.at) == 0xFFFF:
            out.rec(END)
            fresh = True
        elif not out.rec(POINT):
            break
    return out.done()


FORMATS.update({
    "map_wipe_windows": indexed(
        ["Map screen wipes (start_window_wipe): by stage the window's left x, top y (it is 100 wide and 80 high,",
         "clipped to the screen) and how many frames the wipe takes."], _WIPE, "stage"),
    "map_stage_positions": indexed(
        ["map_stage_positions (map_screen_update): by marker (stage_map_markers) where the stage's icon goes on",
         "the map: x, y."], _MAP_POSITION, "marker"),
    "stage_map_markers": byte_table(
        ["stage_map_markers: by stage, its marker on the map (negative: none)."], "stage", "marker", "stage",
        code="b"),
    "stage_map_trails": fmt(_map_trails_text, _TRAIL_LIST, _TRAIL_FRAMES, POINT, END),
    "map_eggman_gesture_frames": byte_table(
        ["map_eggman_gesture_frames (map_eggman_update, step 2): one byte each 4 frames: the pose n (sprite 5760",
         "+ (n - 1) x 8; sound 21 on 7); -1 (FF) ends, the last pose stays."], "gesture", "pose", "step", code="b"),
    "map_marker_blink_sprites": indexed(
        ["map_marker_blink_sprites (blink_current_map_marker): by marker (stage_map_markers[stage]) the sprite",
         "that blinks over it: layer, screen x, y, sprite, palette."], _BLINK_SPRITE, "marker"),
})
FORMATS.update(phased_formats("map_marker_layouts", _MARKER, lambda phase: fmt(
    phased_text(_MARKERS_HEADER, _MARKER, phase, lambda index: f"marker {index:02X}"), _MARKER)))


# ---- the players, the character chooser, the ring tally ----

_R25_OFFSET = Rec("player", 2, [("variable", 0, "H", ("off", WORK_RAM))])
_PORTRAIT_POINTER = Rec("character", 4, [("portrait", 0, "I", "ptr")])
_PORTRAIT = Rec("portrait", 8, [("palette", 0, "H", "x"), ("frame", 2, "H", "frame"), ("frame_2", 4, "H", "frame"),
                                ("frame_3", 6, "H", "frame")])


def _ring_portraits_text(data):
    out = Out(data, ["ring_tally_portraits (build_ring_tally_panel): per character (0 Sonic, 1 Ray, 2 Mighty) the",
                     "address of its record: the palette (| 8000), the portrait's sprite, then two more sprite",
                     "words the panel keeps a pointer to (its field 0E)."])
    for character in range(3):
        out.rec(_PORTRAIT_POINTER, ("Sonic", "Ray", "Mighty")[character])
    while out.rec(_PORTRAIT):
        pass
    return out.done()


FORMATS.update({
    "count": single("count", "H", header=["How many records the table after this word holds (the code reads it",
                                          "as a count)."]),
    "player_block_offsets": indexed(
        ["player_block_offsets: per player the offset of its block from 208000 (R25): 400, 600, 800."],
        Rec("player", 4, [("offset", 0, "I", "x")]), "player"),
    "player_variable_offsets": indexed(
        ["Per player, the offset from 208000 (R25) of a variable (the name symbols.txt gives it)."],
        _R25_OFFSET, "player"),
    "screen_x_by_player": indexed(
        ["Screen x by player: [player] for 3 players, then [player + 3] for 2-player cabinets."],
        Rec("player", 2, [("x", 0, "H")]), lambda index: f"{index % 3 + 1} of {3 if index < 3 else 2}"),
    "ram_addresses": indexed(["Work RAM addresses, one a line by player (the name symbols.txt gives each)."],
                             Rec("player", 4, [("address", 0, "I", "ptr")]), "player"),
    "input_bits": indexed(
        ["By player, the bit of the trackball button in the inputs (inputs_pressed)."],
        Rec("player", 1, [("bit", 0, "B", "x")]), "player"),
    "character_taken_bits": indexed(
        ["character_taken_bits: by character (0 Sonic, 1 Ray, 2 Mighty) its bit in characters_taken."],
        Rec("character", 1, [("bit", 0, "B")]), "character"),
    "chooser_voice_sounds": indexed(
        ["chooser_voice_sounds: [character x 4 + random 0-3], the voice the character chooser plays for the one",
         "chosen (a sound id)."], Rec("voice", 1, [("sound", 0, "B", "x")]),
        lambda index: f"{('Sonic', 'Ray', 'Mighty')[index // 4] if index < 12 else index // 4} {index % 4}"),
    "decimal_by_stage": byte_table(["A number by stage (decimal): stage_ring_bonus_percents, the percentage the",
                                    "players must reach for the bonus; stage_ring_counts, the rings in the stage."],
                                   "stage", "value", "stage"),
    "ring_tally_character_sounds": byte_table(
        ["ring_tally_character_sounds (update_ring_tally_panel): by character, the voice when it gets the bonus."],
        "character", "sound", "character", "x"),
    "ring_tally_portraits": fmt(_ring_portraits_text, _PORTRAIT_POINTER, _PORTRAIT),
})


# ---- objects started from a list (reset_objects_from_list) ----

def object_handler_list_to_text(data):
    lines = []
    if len(data) >= 2:
        count = struct.unpack_from("<H", data, 0)[0]
        lines.append(f"count {count}")
        at = 2
        while at + 4 <= len(data):
            value = struct.unpack_from("<I", data, at)[0]
            name = common.name_of(value)
            lines.append(f"handler 0x{value:06X}" + (f"  # {name}" if name else ""))
            at += 4
        lines += hex_lines(data[at:], KEPT)
    else:
        lines += hex_lines(data)
    header = ["reset_objects_from_list (start_player_objects, start_player_objects_alt): the count of objects,",
              "then the handler each one starts in, one a line (the address, its name in the comment)."]
    return join(header, lines)


def object_handler_list_from_text(text):
    out = bytearray()
    for line_number, tokens in content(text):
        where = f"line {line_number}"
        found = raw(tokens, where)
        if found is not None:
            out += found
        elif tokens[0] == "count" and len(tokens) == 2:
            out += struct.pack("<H", common.number(tokens[1], where))
        elif tokens[0] == "handler" and len(tokens) == 2:
            out += struct.pack("<I", address_value(tokens[1], where))
        else:
            raise ValueError(f"{where}: '{tokens[0]}' is not a line this file takes")
    return bytes(out)


FORMATS.update({
    "object_handler_list": (".txt", object_handler_list_to_text, object_handler_list_from_text),
    "hit_boxes": (".txt",) + labelled_table(
        run_of("box", ("size_x", "H", "d"), ("size_y", "H", "d"), ("offset_x", "b"), ("offset_y", "b")), [
            "Hit boxes (6-byte records): hitbox_size_x, hitbox_size_y and the bytes hitbox_offset_x,",
            "hitbox_offset_y (fields 10, 12, 7C, 7D: the box spans x + offset_x .. + size_x, the same in y)."]),
})


# ---- registers set from a table: the mixer, boot ----

def _register_values(base, size, names, header):
    """Values the code copies to consecutive registers from 'base' ('size' bytes each), one a line:
    'reg ADDRESS VALUE', the register's name in a comment."""

    def to_text(data):
        lines = []
        count = len(data) // size
        for index in range(count):
            value = int.from_bytes(data[index * size:index * size + size], "little")
            address = base + index * size
            name = names.get(address, "")
            lines.append(f"reg 0x{address:06X}  0x{value:0{size * 2}X}" + (f"  # {name}" if name else ""))
        lines += hex_lines(data[count * size:], KEPT)
        return join(header, lines)

    def from_text(text):
        out = bytearray()
        for line_number, tokens in content(text):
            where = f"line {line_number}"
            found = raw(tokens, where)
            if found is not None:
                out += found
                continue
            if tokens[0] != "reg" or len(tokens) != 3:
                raise ValueError(f"{where}: 'reg ADDRESS VALUE' expected")
            due = base + len(out)
            if common.number(tokens[1], where) != due:
                raise ValueError(f"{where}: register {tokens[1]} where 0x{due:06X} was due")
            value = common.number(tokens[2], where)
            if not 0 <= value < 1 << (size * 8):
                raise ValueError(f"{where}: {tokens[2]} does not fit in {size * 8} bits")
            out += value.to_bytes(size, "little")
        return bytes(out)

    return to_text, from_text


MIXER = 0x610000
MIXER_NAMES = {MIXER + group * 2: f"sprite group {group:X}" for group in range(16)}
MIXER_NAMES.update({
    0x610020: "text layer", 0x610022: "mixer_nbg0", 0x610024: "NBG1", 0x610026: "NBG2", 0x610028: "mixer_nbg3",
    0x61002A: "mixer_bitmap", 0x610030: "mixer_text_blend (the shadow 20F1C0 on)", 0x610032: "mixer_nbg0_blend",
    0x610034: "mixer_nbg1_blend", 0x610036: "mixer_nbg2_blend", 0x610038: "mixer_nbg3_blend",
    0x61003A: "mixer_bitmap_blend", 0x61003E: "mixer_offset_select", 0x610040: "color_offset_red",
    0x610042: "color_offset_green", 0x610044: "color_offset_blue", 0x610046: "color_offset_2_red",
    0x610048: "color_offset_2_green", 0x61004A: "color_offset_2_blue", 0x61004C: "mixer_sprite_control",
    0x61004E: "mixer_blend",
})
MIXER_HEADER = ["init_mixer: 0x28 words to the mixer at 610000-61004F (priority and palette of the sprite groups",
                "and the layers, then the blend controls and colour offsets); the last 0x10 also go to the mixer",
                "shadow at 20F1C0, which every VBLANK copies to 610030. The table is cut where the code reads",
                "single entries (mixer_default_nbg0, mixer_default_nbg3): each piece holds the entries from there."]

BOOT_NAMES = {0xD00000 + vector: f"irq_vectors (vector {vector})" for vector in range(5)}
BOOT_NAMES.update({0xD00005: "irq_control_5", 0xD00006: "irq_mask", 0xD00007: "irq_acknowledge",
                   0xD00008: "timer_0_reload (low)", 0xD00009: "timer_0_reload (high)",
                   0xD0000A: "timer_1_reload (low)", 0xD0000B: "timer_1_reload (high)"})

for _first in (0x00, 0x11, 0x14):
    FORMATS[f"mixer_values_{_first:02X}"] = (".txt",) + _register_values(MIXER + _first * 2, 2, MIXER_NAMES,
                                                                          MIXER_HEADER)
FORMATS["boot_e00000"] = (".txt",) + _register_values(0xE00000, 1, {}, [
    "boot: the 16 bytes it copies to E00000-E0000F (nothing is mapped there on this board: MAME and",
    "s32ref ignore them), then (boot_d00000) the 12 for the interrupt controller."])
FORMATS["boot_d00000"] = (".txt",) + _register_values(0xD00000, 1, BOOT_NAMES, [
    "boot: the bytes it copies to the interrupt controller at D00000-D0000B: the five vector numbers,",
    "D00005, the mask, the acknowledge register and the two timer reloads."])


# ---- the system: video and sprite set-up, sounds in attract, settings, ROM checks, the vector table ----

def _video_register(index):
    """init_video_registers writes 31FF00-0F, 31FF5C-5F and 31FF88-8F from the table, in that order."""
    if index < 8:
        return f"0x{0x31FF00 + index * 2:06X}"
    if index < 10:
        return f"0x{0x31FF5C + (index - 8) * 2:06X}"
    return f"0x{0x31FF88 + (index - 10) * 2:06X}"


_EEPROM_HEADER = Rec("eeprom", 14, [("magic", 0, "I", "x"), ("rom_stamp", 4, "H", "x"), ("word_06", 6, "H", "x"),
                                    ("checksum", 8, "H", "x"), ("write_count", 10, "I")])


def _eeprom_defaults_text(data):
    out = Out(data, ["eeprom_defaults (load_settings): the start of the EEPROM's 0x40 bytes as the defaults write",
                     "them (20F600 on): the magic (41 32 33 53, 'A23S' in memory order), the ROM stamp, a word, the",
                     "checksum and the write count; the settings follow (default_settings)."])
    out.rec(_EEPROM_HEADER)
    return out.done()


_SETTING = Rec("setting", 1, [("value", 0, "B", "x")])
SETTINGS = 0x20F60E


def _default_settings_text(data):
    out = Out(data, ["default_settings (load_settings): the settings bytes 20F60E-20F63F the defaults set, one a",
                     "line (the comment names the variable when symbols.txt has one)."])
    for index in range(min(0x32, len(data))):
        name = common.name_of(SETTINGS + index)
        out.rec(_SETTING, f"{SETTINGS + index:06X}" + (f" {name}" if name else ""))
    return out.done()


_VECTOR = Rec("vector", 4, [("handler", 0, "I", "ptr")], phased=True)
_CHECKSUM = Rec("checksum", 2, [("crc", 0, "H", "x")])


def _vector_text(start):
    """The vector table from byte 'start' of it: 32-bit handlers by vector number; entries 23 and 24 hold
    the four ROM checksums, 25 two words no code reads."""
    def to_text(data):
        out = Out(data, ["The vector table (SBR = 0FF000: the CPU takes vector n at 0FF000 + 4n; the copy at 09F000):",
                         "the handler of each vector. Entries 23-24 hold the ROM checksums (rom_checksums:",
                         "check_rom_checksums compares them with compute_crc16 of IC17 and IC8 and",
                         "compute_crc16_alternate_bytes of IC18 and IC9); entry 25 holds two words no code reads.",
                         "Vectors 100-13F lie past the 256 the CPU uses."])
        position = start
        names = {0x8C: "IC17 (000000-01FFFF)", 0x8E: "IC8 (080000-09EFFF)", 0x90: "IC18", 0x92: "IC9"}
        while out.at < len(data):
            if position in names:
                out.rec(_CHECKSUM, names[position])
                position += 2
                continue
            if 0x94 <= position < 0x98:
                out.raw(min(len(data), out.at + 0x98 - position), "vector 25: no code reads these words")
                position = 0x98
                continue
            if position % 4 or out.at + 4 > len(data):
                break
            out.rec(_VECTOR, f"vector {position // 4:02X}")
            position += 4
        return out.done()
    return fmt(to_text, _VECTOR, _CHECKSUM)


FORMATS.update({
    "angle_octants": indexed(
        ["angle_octants (099082): by octant the base angle (complemented: mirrored)."],
        Rec("octant", 2, [("angle", 0, "H", "x")]), "octant"),
    "video_register_defaults": indexed(
        ["video_register_defaults (init_video_registers): the values written to 31FF00-0F, 31FF5C-5F and",
         "31FF88-8F, in that order (the comment names the register)."], Rec("register", 2, [("value", 0, "H", "x")]),
        _video_register),
    "attract_sound_mask": indexed(
        ["attract_sound_mask (play_sound in attract mode): bit (id & 1F) of word (id >> 5) set: sound id may play."],
        Rec("ids", 4, [("mask", 0, "I", "x")]), lambda index: f"ids {index * 32:02X}-{index * 32 + 31:02X}"),
    "sprite_control_init": indexed(
        ["sprite_control_init (init_sprites): the values written (as halfwords) to the sprite control registers",
         "500006, 500008, 50000A, 50000C."], Rec("register", 1, [("value", 0, "B", "x")]),
        lambda index: f"0x{0x500006 + index * 2:06X}"),
    "sprite_list_header": indexed(
        ["sprite_list_header (init_sprites copies 10 halfwords from here to sprite RAM 400000: the first two",
         "16-byte entries, so the copy runs on into sprite_buffer_swaps): entry 0 (7000: clip to 0-13F x 0-DF,",
         "no exclusion) and the start of entry 1 (offset 70/A0)."], Rec("word", 2, [("value", 0, "H", "x")]),
        lambda index: f"entry {index // 8} word {index % 8}"),
    "sprite_buffer_swaps": indexed(
        ["sprite_buffer_swaps (swap_sprite_buffers): per buffer (+0, +8) the back buffer's low word and the jump",
         "of entry 1; words 2-3 are not read (init_sprites' copy of sprite_list_header ends in the first 10",
         "bytes)."], Rec("buffer", 8, [("back_buffer", 0, "H", "x"), ("jump", 2, "H", "x"), ("word_2", 4, "H", "x"),
                                       ("word_3", 6, "H", "x")]), "buffer"),
    "line_direction_steps": indexed(
        ["line_direction_steps (draw_line, draw_dotted_line_8bpp): by direction_index (dy < 0: +1, dx < 0: +2) the",
         "row step and the column step."], Rec("direction", 4, [("row_step", 0, "h"), ("column_step", 2, "h")]),
        "direction"),
    "eeprom_defaults": fmt(_eeprom_defaults_text, _EEPROM_HEADER),
    "default_settings": fmt(_default_settings_text, _SETTING),
    "ram_test_patterns": indexed(
        ["ram_test_patterns (test_work_ram): the ROM data each pass copies over the work RAM (entry 0: none)."],
        Rec("pattern", 4, [("data", 0, "I", "ptr")]), "pattern"),
    "vector_table": _vector_text(0),
    "rom_checksums": _vector_text(0x8C),
    "rom_checksum_ic8": _vector_text(0x8E),
})
