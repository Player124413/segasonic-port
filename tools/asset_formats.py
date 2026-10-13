"""
Name:        asset_formats.py
Description: converters from System 32 data formats to standard files (indexed PNG, WAV)
Version:     0.1.0
Created:     11-10-2026
Modified:    11-10-2026
Author:      Sebastian Toyos

Formats follow MAME's segas32 driver (BSD-3-Clause):
  - tiles: 16x16 pixels, 4 bpp, 128 bytes per tile; gfx_layout bgcharlayout in segas32.cpp
  - PCM: RF5C68 sign-magnitude bytes (bit 7 set = positive), 0xFF ends a sample; sound/rf5c68.cpp
PNGs keep the original 4-bit pen numbers as palette indices, so they can be converted back losslessly.
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
