"""
Name:        data_formats.py
Description: the program ROM's data as readable text files, and back, byte for byte
Version:     0.1.0
Created:     14-10-2026
Modified:    14-10-2026
Author:      Sebastian Toyos

The data area of the program ROM (0x100000-0x17FFFF: the tables, palettes, scripts, level data the code
reads) is cut where symbols.txt names a place, and each piece goes to a text file of its own in a format
that says what it is: data_formats.json gives a piece's format by its name; a piece it does not name is
written as 16-bit words in hex (bytes when its size is odd), which goes back just as well. Each format is a
pair: to_text(bytes) -> text, and from_text(text) -> bytes, with from_text(to_text(data)) == data.

The program ROM is the V60's: little endian.
"""
import json
import re
import struct


# ---- the formats ----

def words_to_text(data):
    """16-bit words, eight a line, hex."""
    words = struct.unpack(f"<{len(data) // 2}H", data)
    return "".join(" ".join(f"{word:04X}" for word in words[at:at + 8]) + "\n" for at in range(0, len(words), 8))


def words_from_text(text):
    return b"".join(struct.pack("<H", int(word, 16)) for word in text.split())


def bytes_to_text(data):
    """Bytes, sixteen a line, hex."""
    return "".join(" ".join(f"{byte:02X}" for byte in data[at:at + 16]) + "\n" for at in range(0, len(data), 16))


def bytes_from_text(text):
    return bytes(int(byte, 16) for byte in text.split())


def palette_to_text(data):
    """Palette entries as the game keeps them (the palette window's alternate format: the low bit of blue,
    green and red in bits 14-12, the rest 4:4:4), one a line: #RRGGBB (5 bits a component, scaled to 8),
    and '+' where bit 15 is set; sixteen entries a block, a blank line between blocks."""
    lines = []
    for index, (value,) in enumerate(struct.iter_unpack("<H", data[:len(data) // 2 * 2])):
        red = ((value >> 12) & 1) | ((value << 1) & 0x1E)
        green = ((value >> 13) & 1) | ((value >> 3) & 0x1E)
        blue = ((value >> 14) & 1) | ((value >> 7) & 0x1E)
        if index and index % 16 == 0:
            lines.append("")
        lines.append(f"#{red * 255 // 31:02X}{green * 255 // 31:02X}{blue * 255 // 31:02X}" + ("+" if value & 0x8000 else ""))
    text = "\n".join(lines) + "\n"
    if len(data) % 2:
        text += f"byte {data[-1]:02X}\n"
    return text


def palette_from_text(text):
    out = bytearray()
    for line in text.split("\n"):
        line = line.strip()
        if not line:
            continue
        if line.startswith("byte "):
            out.append(int(line[5:], 16))
            continue
        red, green, blue = (round(int(line[at:at + 2], 16) * 31 / 255) for at in (1, 3, 5))
        value = (0x8000 if line.endswith("+") else 0) | (blue & 1) << 14 | (green & 1) << 13 | (red & 1) << 12 \
            | (blue & 0x1E) << 7 | (green & 0x1E) << 3 | (red & 0x1E) >> 1
        out += struct.pack("<H", value)
    return bytes(out)


FORMATS = {
    "words": (".txt", words_to_text, words_from_text),
    "bytes": (".txt", bytes_to_text, bytes_from_text),
    "palette": (".pal.txt", palette_to_text, palette_from_text),
}


def _gather():
    """The formats of tools/formats/, one module a family of data."""
    import importlib
    import pkgutil
    import formats
    for module in pkgutil.iter_modules(formats.__path__):
        FORMATS.update(importlib.import_module(f"formats.{module.name}").FORMATS)


_gather()


# ---- the pieces ----

def pieces(symbols_path, start, end):
    """[(address, name)] of the places symbols.txt names in start..end, the first one at 'start'."""
    found = {}
    for line in symbols_path.read_text(encoding="utf-8").splitlines():
        match = re.match(r"([0-9A-F]{6})\s+(data|table)\s+(\w+)", line)
        if match:
            address = int(match.group(1), 16)
            if start <= address < end:
                found.setdefault(address, match.group(3))
    if start not in found:
        found[start] = f"data_{start:06X}"
    return sorted(found.items())


def export(region, base, symbols_path, formats_path, folder):
    """Every piece of 'region' (which starts at program address 'base') to a file in 'folder', and
    layout.json, the pieces in order. Returns how many files."""
    chosen = json.loads(formats_path.read_text(encoding="ascii"))["formats"] if formats_path.exists() else {}
    places = pieces(symbols_path, base, base + len(region))
    folder.mkdir(parents=True, exist_ok=True)
    layout = []
    for index, (address, name) in enumerate(places):
        end = places[index + 1][0] if index + 1 < len(places) else base + len(region)
        data = region[address - base:end - base]
        kind = chosen.get(name, "words")
        if kind not in FORMATS or (kind == "words" and len(data) % 2):
            kind = "bytes" if len(data) % 2 or kind not in FORMATS else kind
        suffix, to_text, from_text = FORMATS[kind]
        text = to_text(data)
        if from_text(text) != data:    # a format that does not fit this piece: keep it as bytes
            kind = "bytes"
            suffix, to_text, _ = FORMATS[kind]
            text = to_text(data)
        file_name = f"{address:06X}_{name}{suffix}"
        (folder / file_name).write_text(text, encoding="ascii")
        layout.append({"address": f"{address:06X}", "name": name, "size": len(data), "format": kind, "file": file_name})
    (folder / "layout.json").write_text(json.dumps({"base": f"{base:06X}", "pieces": layout}, indent=1) + "\n",
                                        encoding="ascii")
    return len(layout) + 1


def build(folder, size):
    """The region back from 'folder': every piece's file, in the order layout.json gives."""
    layout = json.loads((folder / "layout.json").read_text(encoding="ascii"))
    out = bytearray()
    for piece in layout["pieces"]:
        _, _, from_text = FORMATS[piece["format"]]
        data = from_text((folder / piece["file"]).read_text(encoding="ascii"))
        if len(data) != piece["size"]:
            # the code reaches every piece at a fixed address: one that grows or shrinks moves the rest
            raise ValueError(f"{piece['file']}: {len(data)} bytes, not {piece['size']}")
        out += data
    if len(out) != size:
        raise ValueError(f"{folder}: the pieces make {len(out)} bytes, not {size}")
    return bytes(out)
