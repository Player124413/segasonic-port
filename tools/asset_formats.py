"""
Name:        asset_formats.py
Description: converters from System 32 data formats to standard files (indexed PNG, WAV), and back
Version:     0.1.0
Created:     11-10-2026
Modified:    14-10-2026
Author:      Sebastian Toyos

Formats follow MAME's segas32 driver (BSD-3-Clause):
  - tiles: 16x16 pixels, 4 bpp, 128 bytes per tile; gfx_layout bgcharlayout in segas32.cpp
  - PCM: RF5C68 sign-magnitude bytes (bit 7 set = positive), 0xFF ends a sample; sound/rf5c68.cpp
  - sprites: rows of 32-bit words (big endian in the ROM image), 8 pens of 4 bits or 4 of 8 bits each, top
    bits first; a frame is a record of sprite_frame_records in the program ROM (symbols.txt): width in
    pixels, height (low byte) and address bits 16-19 (high nibble), bank bits (0x0800, 0x4000), address
PNGs keep the original pen numbers as palette indices, so they can be converted back losslessly.
"""
import struct
import zlib

TILE_BYTES = 128
TILE_SIZE = 16

# A tile row is 8 bytes; its 16 pixels come from the bytes in this order (two pixels per byte,
# high nibble first). It is MAME's x offsets {0,4,16,20,8,12,24,28,32,36,48,52,40,44,56,60}.
TILE_ROW_ORDER = (0, 2, 1, 3, 4, 6, 5, 7)

GRAY16 = bytes(level for index in range(16) for level in (index * 17,) * 3)


def _png_chunk(kind, data):
    chunk = kind + data
    return struct.pack(">I", len(data)) + chunk + struct.pack(">I", zlib.crc32(chunk) & 0xFFFFFFFF)


def write_png_4bpp(path, width, rows, palette=GRAY16):
    """Write an indexed 4-bit PNG. 'rows' holds packed rows: width / 2 bytes, high nibble first."""
    header = struct.pack(">IIBBBBB", width, len(rows), 4, 3, 0, 0, 0)
    raw = b"".join(b"\x00" + row for row in rows)
    with open(path, "wb") as out:
        out.write(b"\x89PNG\r\n\x1a\n")
        out.write(_png_chunk(b"IHDR", header))
        out.write(_png_chunk(b"PLTE", palette))
        out.write(_png_chunk(b"IDAT", zlib.compress(raw, 9)))
        out.write(_png_chunk(b"IEND", b""))


def tile_sheet_rows(data, columns):
    """Lay 16x16x4 tiles out in a grid 'columns' tiles wide. Returns packed 4-bit rows."""
    count = len(data) // TILE_BYTES
    rows = []
    for first in range(0, count, columns):
        tiles = [data[(first + column) * TILE_BYTES:(first + column + 1) * TILE_BYTES]
                 for column in range(min(columns, count - first))]
        for y in range(TILE_SIZE):
            row = bytearray()
            for tile in tiles:
                source = tile[y * 8:y * 8 + 8]
                row += bytes(source[index] for index in TILE_ROW_ORDER)
            row += bytes(8 * (columns - len(tiles)))
            rows.append(bytes(row))
    return rows


def split_rf5c68(data, minimum=16):
    """Split a PCM bank into samples: runs of bytes between 0xFF end markers. Returns [(offset, bytes)]."""
    samples = []
    start = None
    for offset, value in enumerate(data):
        if value != 0xFF and start is None:
            start = offset
        elif value == 0xFF and start is not None:
            if offset - start >= minimum:
                samples.append((start, data[start:offset]))
            start = None
    if start is not None and len(data) - start >= minimum:
        samples.append((start, data[start:]))
    return samples


def rf5c68_to_pcm16(data):
    """Sign-magnitude 8-bit (bit 7 set = positive) to signed 16-bit little-endian samples."""
    values = [((value & 0x7F) << 8) if value & 0x80 else -((value & 0x7F) << 8) for value in data]
    return struct.pack(f"<{len(values)}h", *values)


def write_wav(path, pcm16, rate):
    """Write mono 16-bit PCM."""
    with open(path, "wb") as out:
        out.write(b"RIFF" + struct.pack("<I", 36 + len(pcm16)) + b"WAVE")
        out.write(b"fmt " + struct.pack("<IHHIIHH", 16, 1, 1, rate, rate * 2, 2, 16))
        out.write(b"data" + struct.pack("<I", len(pcm16)) + pcm16)


# ---- sprites ----

SPRITE_RECORD = 8    # bytes of a frame record in the program ROM


def sprite_frames(program, offset, count):
    """The frame records at 'offset' in the program ROM image (V60 order: little endian), in record order:
    [{"number": the frame's sprite_frame (record offset), "width", "height", "word": where its graphics start
    in the sprite ROM (32-bit words, bank << 20 | address), "bits": 4 or 8 a pen, "words": its length}].
    The bits per pen are not in the record (the object drawing it says them): they come from the layout,
    each frame's graphics running up to the next one's."""
    frames = []
    for index in range(count):
        width, height, bank_bits, address = struct.unpack_from("<HHHH", program, offset + index * SPRITE_RECORD)
        bank = (bank_bits & 0x0800) >> 11 | (bank_bits & 0x4000) >> 13
        frames.append({"number": index * SPRITE_RECORD, "width": width, "height": height & 0xFF,
                       "word": bank << 20 | (height & 0xF000) << 4 | address})
    ordered = sorted(frames, key=lambda frame: frame["word"])
    for position, frame in enumerate(ordered):
        following = ordered[position + 1]["word"] if position + 1 < len(ordered) else None
        words4 = (frame["width"] >> 3) * frame["height"]
        words8 = (frame["width"] >> 2) * frame["height"]
        frame["bits"] = 8 if following is not None and following - frame["word"] == words8 else 4
        frame["words"] = words8 if frame["bits"] == 8 else words4
    return frames


def sprite_pens(sprites, frame):
    """The frame's pens: one list a row, (width >> 3) words of 8 pens (4 bits) or (width >> 2) of 4 (8 bits)."""
    bits = frame["bits"]
    per_word = 32 // bits
    words = frame["width"] >> (3 if bits == 4 else 2)
    rows = []
    for row in range(frame["height"]):
        pens = []
        for index in range(words):
            at = (frame["word"] + row * words + index) * 4
            value = struct.unpack_from(">I", sprites, at)[0]
            pens += [(value >> (32 - bits * (pen + 1))) & ((1 << bits) - 1) for pen in range(per_word)]
        rows.append(pens)
    return rows


def pens_to_words(rows, bits):
    """Rows of pens back to the sprite ROM's bytes (big-endian 32-bit words)."""
    per_word = 32 // bits
    out = bytearray()
    for pens in rows:
        for first in range(0, len(pens), per_word):
            value = 0
            for pen in pens[first:first + per_word]:
                value = value << bits | pen
            out += struct.pack(">I", value)
    return bytes(out)


def rgb555(color):
    """A System 32 palette entry (red in bits 0-4, green 5-9, blue 10-14) as 8-bit red, green, blue."""
    return tuple(((color >> shift) & 0x1F) * 255 // 31 for shift in (0, 5, 10))


def sprite_drawn(rows, bits):
    """Which pens of a frame the board draws (sprite list, palette not indirect, not opaque): never pen 0;
    in each word of a row, not the first or last pen when it is all ones (the end code); and a word that
    ends with the end code ends the row (runtime/src/video.c, draw_sprite_row and put_sprite_pixel). Rows
    of booleans."""
    per_word = 32 // bits
    end = (1 << bits) - 1
    drawn = []
    for row in rows:
        flags = []
        ended = False
        for first in range(0, len(row), per_word):
            for index, pen in enumerate(row[first:first + per_word]):
                edge = index == 0 or index == per_word - 1
                flags.append(not ended and pen != 0 and not (edge and pen == end))
            if row[first + per_word - 1] == end:
                ended = True
        drawn.append(flags)
    return drawn


def write_png_indexed(path, rows, palette, bits, transparent=None, alphas=None):
    """An indexed PNG of 'bits' (4 or 8) a pixel: 'rows' of palette indices, 'palette' of (r, g, b) entries;
    'transparent' is an index shown as see-through, or 'alphas' gives each entry's opacity."""
    width = len(rows[0]) if rows else 0
    header = struct.pack(">IIBBBBB", width, len(rows), bits, 3, 0, 0, 0)
    raw = bytearray()
    for row in rows:
        raw.append(0)
        if bits == 8:
            raw += bytes(row)
        else:
            raw += bytes(row[index] << 4 | (row[index + 1] if index + 1 < len(row) else 0)
                         for index in range(0, len(row), 2))
    with open(path, "wb") as out:
        out.write(b"\x89PNG\r\n\x1a\n")
        out.write(_png_chunk(b"IHDR", header))
        out.write(_png_chunk(b"PLTE", b"".join(bytes(entry) for entry in palette)))
        if alphas is not None:
            out.write(_png_chunk(b"tRNS", bytes(alphas)))
        elif transparent is not None:
            out.write(_png_chunk(b"tRNS", bytes(255 if index != transparent else 0 for index in range(len(palette)))))
        out.write(_png_chunk(b"IDAT", zlib.compress(bytes(raw), 9)))
        out.write(_png_chunk(b"IEND", b""))


def _paeth(left, up, up_left):
    estimate = left + up - up_left
    near_left, near_up, near_up_left = abs(estimate - left), abs(estimate - up), abs(estimate - up_left)
    if near_left <= near_up and near_left <= near_up_left:
        return left
    return up if near_up <= near_up_left else up_left


def read_png_indexed(path):
    """The palette indices of an indexed PNG (1, 2, 4 or 8 bits a pixel, any filter, not interlaced), one
    list a row, and its palette [(r, g, b)]. An image edited elsewhere comes back as indices, as long as it
    stays indexed."""
    data = open(path, "rb").read()
    if data[:8] != b"\x89PNG\r\n\x1a\n":
        raise ValueError(f"{path}: not a PNG")
    at = 8
    idat = bytearray()
    palette = []
    header = None
    while at < len(data):
        length, kind = struct.unpack_from(">I4s", data, at)
        body = data[at + 8:at + 8 + length]
        if kind == b"IHDR":
            header = struct.unpack(">IIBBBBB", body)
        elif kind == b"PLTE":
            palette = [tuple(body[index:index + 3]) for index in range(0, len(body), 3)]
        elif kind == b"IDAT":
            idat += body
        at += 12 + length
    width, height, bits, color_type, _, _, interlace = header
    if color_type != 3 or interlace:
        raise ValueError(f"{path}: not an indexed, non-interlaced PNG")
    raw = zlib.decompress(bytes(idat))
    stride = (width * bits + 7) // 8
    rows = []
    previous = bytearray(stride)
    at = 0
    for _ in range(height):
        kind = raw[at]
        line = bytearray(raw[at + 1:at + 1 + stride])
        at += 1 + stride
        for index in range(stride):
            left = line[index - 1] if index else 0
            up = previous[index]
            up_left = previous[index - 1] if index else 0
            if kind == 1:
                line[index] = (line[index] + left) & 0xFF
            elif kind == 2:
                line[index] = (line[index] + up) & 0xFF
            elif kind == 3:
                line[index] = (line[index] + (left + up) // 2) & 0xFF
            elif kind == 4:
                line[index] = (line[index] + _paeth(left, up, up_left)) & 0xFF
        previous = line
        per_byte = 8 // bits
        indices = []
        for byte in line:
            for pixel in range(per_byte):
                indices.append(byte >> (8 - bits * (pixel + 1)) & ((1 << bits) - 1))
        rows.append(indices[:width])
    return rows, palette


def build_sprite_rom(folder, size):
    """The sprite ROM image (32-bit big-endian words, 'size' bytes) from the frames extract_assets.py wrote
    to 'folder' (frames.json and its PNGs): each frame's pens where its graphics go, what no frame covers
    from its strips, 0xFF everywhere else, as the chips leave it."""
    import json
    index = json.loads((folder / "frames.json").read_text(encoding="ascii"))
    image = bytearray(b"\xff" * size)
    for frame in index["frames"]:
        rows, _ = read_png_indexed(folder / frame["file"])
        # (4-bit frames: indices 16-31 are pens 0-15 where the board does not draw them)
        mask = (1 << frame["bits"]) - 1
        data = pens_to_words([[pen & mask for pen in row] for row in rows], frame["bits"])
        at = int(frame["word"], 16) * 4
        image[at:at + len(data)] = data
    for strip in index["leftovers"]:
        rows, _ = read_png_indexed(folder / strip["file"])
        data = pens_to_words(rows, 4)
        at = int(strip["word"], 16) * 4
        image[at:at + len(data)] = data
    return bytes(image)
