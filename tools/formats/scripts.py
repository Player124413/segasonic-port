"""
Name:        scripts.py
Description: the vram scripts run_vram_script runs (tilemaps written into video RAM) as text, one command a line
Version:     0.1.0
Created:     14-10-2026
Modified:    14-10-2026
Author:      Sebastian Toyos

A vram script (run_vram_script, 09986B) is a string of bytes: a byte under 0x80 writes word [byte] of
vram_script_dictionary (20F300) at the write position, and 0x80 + 3n are commands whose operands follow
(the vram_script_* functions read them). Every write moves the position one cell on, except the column
fills and repeat. The scripts write tilemap pages: a page is 32 x 16 cells at 0x300000 + page * 0x400, a
cell is the tile number (bits 0-12), flipped left-right (bit 14) and upside down (bit 15).

The text, one command a line ('#' starts a comment; page and cells in hex, the rest in decimal):

    dict 03 2F 00                       00-7F: write dictionary words 03, 2F, 00
    address page 0A row 3 col 10        80: write position 300000 + word * 2; word = page * 0x200 + row * 32 + col
    skip -2                             83: move the position by a signed byte of cells
    fill 12 1E03                        86: the cell 12 times
    count_up 12 1E03                    89: 1E03, 1E04, ... 12 cells
    count_down 12 1E03                  8C: 1E03, 1E02, ... 12 cells
    bytes 1E: 00 05 07                  8F: cells 1E00, 1E05, 1E07 (one high byte, then the low bytes)
    words 1E03 0000 1E07x               92: these cells
    words_to_dictionary 1E03 1E04       95: these cells, also stored in the dictionary in turn from word 00
    clear_pages page 0A count 1         98: write position at the page; clears count pages (0x200 cells each) to 0
    fill_down 4 1E03                    9B: down a column (+32 cells)
    fill_down_wide 4 1E03               9E: down a column of a 64-cell wide map (+64 cells)
    fill_up 4 1E03                      A1: up a column (-32 cells)
    repeat 4 1E03                       A4: the cell 4 times at the same position
    skip_long -300                      A7: move the position by a signed word of cells
    end                                 AA: back to the caller
    hex 8E 03 FF*120                    bytes the game does not run as above (FF*120: 120 bytes FF)

A cell reads as its 13-bit tile number (and bit 13, never set in the game's scripts) in hex, then x when
flipped left-right and y when flipped upside down: 1E03, 1E03x, 1E03y, 1E03xy.

The first script of a piece is shown as the game runs it from the piece's first byte, up to a byte that
is no command (some code points a few bytes into the tail of another script) or a command whose operands
run past the piece (a script cut by the next piece). The piece can go on with more scripts, one after the
other: each one that comes to its end is shown too, after a blank line; what does not decode to an end is
kept as hex.

tile_set_list_offsets is the table of u16 offsets get_tile_set_list adds to blank_tile_set_lists for
stages 0-2, 11 and 12: one tile set a line, the list's program address (or its name).
"""
import re
import struct
from pathlib import Path

SYMBOLS = Path(__file__).resolve().parents[2] / "symbols.txt"
BLANK_TILE_SET_LISTS = 0x170602    # get_tile_set_list: u16 offset + blank_tile_set_lists_ADDRESS

HEADER = "# vram script (run_vram_script): one command a line; syntax in tools/formats/scripts.py"
OFFSETS_HEADER = ("# u16 offsets get_tile_set_list adds to blank_tile_set_lists (170602) for stages 0-2, 11, 12:"
                  " one tile set a line, its script list")

# command byte: (name, operands)
COMMANDS = {
    0x80: ("address", "address"),
    0x83: ("skip", "s8"),
    0x86: ("fill", "count_cell"),
    0x89: ("count_up", "count_cell"),
    0x8C: ("count_down", "count_cell"),
    0x8F: ("bytes", "bytes"),
    0x92: ("words", "words"),
    0x95: ("words_to_dictionary", "words"),
    0x98: ("clear_pages", "clear"),
    0x9B: ("fill_down", "count_cell"),
    0x9E: ("fill_down_wide", "count_cell"),
    0xA1: ("fill_up", "count_cell"),
    0xA4: ("repeat", "count_cell"),
    0xA7: ("skip_long", "s16"),
    0xAA: ("end", None),
}
CODES = {name: code for code, (name, _) in COMMANDS.items()}
END = 0xAA
DICTIONARY_PER_LINE = 16
HEX_PER_LINE = 16
SHORTEST_RUN = 8    # raw bytes repeated this many times or more read as XX*count


# ---- cells and positions ----

def _cell_text(word):
    return f"{word & 0x3FFF:04X}" + ("x" if word & 0x4000 else "") + ("y" if word & 0x8000 else "")


def _cell_value(token):
    match = re.fullmatch(r"([0-9A-Fa-f]{1,4})(x?)(y?)", token)
    if not match or int(match.group(1), 16) > 0x3FFF:
        raise ValueError(f"not a cell: {token}")
    return int(match.group(1), 16) | (0x4000 if match.group(2) else 0) | (0x8000 if match.group(3) else 0)


def _position_text(word, always):
    """A video RAM word index as page (hex), row and column (decimal); row and column left out at 0 unless
    'always'."""
    text = f"page {word >> 9:02X}"
    if always or word & 0x1FF:
        text += f" row {(word >> 5) & 15} col {word & 31}"
    return text


def _take_position(tokens):
    """page PP [row R] [col C] at the front of 'tokens' -> (word, the tokens after them)."""
    fields = {"page": None, "row": 0, "col": 0}
    while tokens and tokens[0] in fields:
        if len(tokens) < 2:
            raise ValueError(f"{tokens[0]} without a value")
        fields[tokens[0]] = int(tokens[1], 16) if tokens[0] == "page" else int(tokens[1])
        tokens = tokens[2:]
    if fields["page"] is None or not 0 <= fields["page"] < 0x80 or not 0 <= fields["row"] < 16 \
            or not 0 <= fields["col"] < 32:
        raise ValueError("a position is page 00-7F, row 0-15, col 0-31")
    return fields["page"] << 9 | fields["row"] << 5 | fields["col"], tokens


# ---- bytes to text ----

def _command(data, at):
    """The command at data[at] (a byte 80 or more) -> (its line, where the next one starts), or None when
    it is no command or its operands run past the data."""
    code = data[at]
    if code not in COMMANDS:
        return None
    name, operands = COMMANDS[code]
    at += 1
    left = len(data) - at
    if operands is None:
        return name, at
    if operands == "address":
        if left < 2:
            return None
        return f"{name} {_position_text(struct.unpack_from('<H', data, at)[0], True)}", at + 2
    if operands == "s8":
        if left < 1:
            return None
        return f"{name} {struct.unpack_from('<b', data, at)[0]}", at + 1
    if operands == "s16":
        if left < 2:
            return None
        return f"{name} {struct.unpack_from('<h', data, at)[0]}", at + 2
    if operands == "count_cell":
        if left < 3:
            return None
        return f"{name} {data[at]} {_cell_text(struct.unpack_from('<H', data, at + 1)[0])}", at + 3
    if operands == "clear":
        if left < 3:
            return None
        word, pages = struct.unpack_from("<HB", data, at)
        return f"{name} {_position_text(word, False)} count {pages}", at + 3
    count = data[at] if left else 0
    if operands == "bytes":
        # a count of 0 would run on for 2^32 bytes: no such script
        if count == 0 or left < 2 + count:
            return None
        lows = " ".join(f"{byte:02X}" for byte in data[at + 2:at + 2 + count])
        return f"{name} {data[at + 1]:02X}: {lows}", at + 2 + count
    if count == 0 or left < 1 + 2 * count:
        return None
    cells = struct.unpack_from(f"<{count}H", data, at + 1)
    return f"{name} " + " ".join(_cell_text(cell) for cell in cells), at + 1 + 2 * count


def _script(data, at):
    """Decode one script from data[at] -> (lines, where decoding stopped, whether it came to its end)."""
    lines = []
    refs = []

    def flush():
        for start in range(0, len(refs), DICTIONARY_PER_LINE):
            lines.append("dict " + " ".join(refs[start:start + DICTIONARY_PER_LINE]))
        refs.clear()

    while at < len(data):
        if data[at] < 0x80:
            refs.append(f"{data[at]:02X}")
            at += 1
            continue
        decoded = _command(data, at)
        if decoded is None:
            break
        flush()
        line, at = decoded
        lines.append(line)
        if line == "end":
            return lines, at, True
    flush()
    return lines, at, False


def _hex_lines(data):
    tokens = []
    at = 0
    while at < len(data):
        run = 1
        while at + run < len(data) and data[at + run] == data[at]:
            run += 1
        if run >= SHORTEST_RUN:
            tokens.append(f"{data[at]:02X}*{run}")
            at += run
        else:
            tokens.append(f"{data[at]:02X}")
            at += 1
    return ["hex " + " ".join(tokens[start:start + HEX_PER_LINE]) for start in range(0, len(tokens), HEX_PER_LINE)]


def script_to_text(data):
    lines = [HEADER]
    at = 0
    while at < len(data):
        script_lines, stop, complete = _script(data, at)
        if at and not complete:
            break    # a later script that does not come to its end: hex from its first byte
        if at:
            lines += ["", f"# next script: +0x{at:X}"]
        lines += script_lines
        at = stop
        if not complete:
            break    # the first script, up to the byte that is no command
    lines += _hex_lines(data[at:])
    return "\n".join(lines) + "\n"


# ---- text to bytes ----

def _hex_bytes(tokens):
    out = bytearray()
    for token in tokens:
        value, _, count = token.partition("*")
        out += bytes([int(value, 16)]) * (int(count) if count else 1)
    return out


def script_from_text(text):
    out = bytearray()
    for number, line in enumerate(text.split("\n"), 1):
        tokens = line.split("#", 1)[0].split()
        if not tokens:
            continue
        name, tokens = tokens[0], tokens[1:]
        try:
            if name == "dict":
                out += bytes(_dictionary_byte(token) for token in tokens)
                continue
            if name == "hex":
                out += _hex_bytes(tokens)
                continue
            if name not in CODES:
                raise ValueError(f"no command {name}")
            code = CODES[name]
            operands = COMMANDS[code][1]
            out.append(code)
            if operands is None:
                rest = tokens
            elif operands == "address":
                word, rest = _take_position(tokens)
                out += struct.pack("<H", word)
            elif operands == "s8":
                out += struct.pack("<b", int(tokens[0]))
                rest = tokens[1:]
            elif operands == "s16":
                out += struct.pack("<h", int(tokens[0]))
                rest = tokens[1:]
            elif operands == "count_cell":
                out += struct.pack("<BH", int(tokens[0]), _cell_value(tokens[1]))
                rest = tokens[2:]
            elif operands == "clear":
                word, rest = _take_position(tokens)
                if len(rest) < 2 or rest[0] != "count":
                    raise ValueError("clear_pages wants count N")
                out += struct.pack("<HB", word, int(rest[1]))
                rest = rest[2:]
            elif operands == "bytes":
                if not tokens or not tokens[0].endswith(":"):
                    raise ValueError("bytes wants HH: LL LL ...")
                lows = [int(token, 16) for token in tokens[1:]]
                out += bytes([len(lows), int(tokens[0][:-1], 16)] + lows)
                rest = []
            else:    # words
                cells = [_cell_value(token) for token in tokens]
                out += struct.pack(f"<B{len(cells)}H", len(cells), *cells)
                rest = []
            if rest:
                raise ValueError(f"left over: {' '.join(rest)}")
        except (ValueError, IndexError, struct.error) as error:
            raise ValueError(f"line {number}: {line.strip()}: {error}") from None
    return bytes(out)


def _dictionary_byte(token):
    value = int(token, 16)
    if not 0 <= value < 0x80:
        raise ValueError(f"a dictionary word is 00-7F, not {token}")
    return value


# ---- the offsets of the blank tile set lists ----

_names = None


def _symbol_names():
    """{address: name} and {name: address} of symbols.txt's data and table lines."""
    global _names
    if _names is None:
        by_address, by_name = {}, {}
        if SYMBOLS.exists():
            for line in SYMBOLS.read_text(encoding="utf-8").splitlines():
                match = re.match(r"([0-9A-F]{6})\s+(data|table)\s+(\w+)", line)
                if match:
                    by_address.setdefault(int(match.group(1), 16), match.group(3))
                    by_name[match.group(3)] = int(match.group(1), 16)
        _names = by_address, by_name
    return _names


def offsets_to_text(data):
    by_address, _ = _symbol_names()
    lines = [OFFSETS_HEADER]
    for index, (offset,) in enumerate(struct.iter_unpack("<H", data[:len(data) // 2 * 2])):
        address = BLANK_TILE_SET_LISTS + offset
        lines.append(f"{by_address.get(address, f'0x{address:06X}'):24s}  # tile set {index:02X}")
    if len(data) % 2:
        lines.append(f"hex {data[-1]:02X}")
    return "\n".join(lines) + "\n"


def offsets_from_text(text):
    _, by_name = _symbol_names()
    out = bytearray()
    for number, line in enumerate(text.split("\n"), 1):
        tokens = line.split("#", 1)[0].split()
        if not tokens:
            continue
        if tokens[0] == "hex":
            out += _hex_bytes(tokens[1:])
            continue
        target = tokens[0]
        address = int(target, 16) if target.lower().startswith("0x") else by_name.get(target)
        if address is None or not 0 <= address - BLANK_TILE_SET_LISTS <= 0xFFFF:
            raise ValueError(f"line {number}: {line.strip()}: not a list within reach of blank_tile_set_lists")
        out += struct.pack("<H", address - BLANK_TILE_SET_LISTS)
    return bytes(out)


FORMATS = {
    "vram_script": (".vram.txt", script_to_text, script_from_text),
    "tile_set_list_offsets": (".txt", offsets_to_text, offsets_from_text),
}
