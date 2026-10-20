"""
Name:        make_icon.py
Description: the console build's executable icon from the player's ROM set: Sonic's, Mighty's and Ray's
             heads (the join chooser's), as an .ico and the .rc that puts it in SegaSonic.exe
Version:     0.1.0
Created:     20-10-2026
Modified:    20-10-2026
Author:      Sebastian Toyos
Usage: python tools/make_icon.py [ROM folder or .zip] [output folder]
       (default: rom/, and build/cmake/data)
The same picture console_window_icon (console/console.c) makes the window's icon from at run time: three
32 x 32 heads in 64 x 64, two above and one under them, pen 0 see-through. The .ico holds it at 64, 32 and
16 pixels (each half the one before, a pixel of four). It stays in the build folder: no picture of the
game goes into the repository.
"""
import struct
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parent.parent
sys.path.insert(0, str(ROOT / "tools"))

import asset_formats  # noqa: E402
import sonic_rom  # noqa: E402

FRAME_RECORDS = 0x130000     # sprite_frame_records
FRAME_COUNT = 6138
HEAD_SIZE = 32
ICON_SIZE = 64
HEADS = (                    # frame (join_chooser_portraits), its palette in the program ROM, where it goes
    (0x90C8, 0x10738E, 0, 0),                        # Sonic
    (0x90D0, 0x1073AE, HEAD_SIZE, 0),                # Mighty
    (0x90D8, 0x1073CE, HEAD_SIZE // 2, HEAD_SIZE),   # Ray
)


def rom_color(program, address):
    """A palette entry of the program ROM (the palette window's alternate format) as (red, green, blue)."""
    value = int.from_bytes(program[address:address + 2], "little")
    red = ((value >> 12) & 1) | ((value << 1) & 0x1E)
    green = ((value >> 13) & 1) | ((value >> 3) & 0x1E)
    blue = ((value >> 14) & 1) | ((value >> 7) & 0x1E)
    return tuple(channel * 255 // 31 for channel in (red, green, blue))


def heads_picture(program, sprites):
    """The 64 x 64 picture: rows of (red, green, blue, alpha)."""
    frames = {frame["number"]: frame for frame in asset_formats.sprite_frames(program, FRAME_RECORDS, FRAME_COUNT)}
    picture = [[(0, 0, 0, 0)] * ICON_SIZE for _ in range(ICON_SIZE)]
    for number, palette, left, top in HEADS:
        frame = frames[number]
        if frame["width"] != HEAD_SIZE or frame["height"] != HEAD_SIZE:
            raise SystemExit("The heads of the icon are not in the ROM set.")
        for y, row in enumerate(asset_formats.sprite_pens(sprites, frame)):
            for x, pen in enumerate(row):
                if pen:
                    picture[top + y][left + x] = rom_color(program, palette + pen * 2) + (255,)
    return picture


def halved(picture):
    """Half the size: each pixel the average of four (see-through ones count as nothing)."""
    size = len(picture) // 2
    result = []
    for y in range(size):
        row = []
        for x in range(size):
            four = [picture[2 * y + dy][2 * x + dx] for dy in (0, 1) for dx in (0, 1)]
            seen = [pixel for pixel in four if pixel[3]]
            if not seen:
                row.append((0, 0, 0, 0))
                continue
            row.append(tuple(sum(pixel[channel] for pixel in seen) // len(seen) for channel in range(3))
                       + (255 * len(seen) // 4,))
        result.append(row)
    return result


def ico_image(picture):
    """An .ico image: a 32-bit BITMAPINFOHEADER bitmap, bottom row first, then its 1-bit AND mask."""
    size = len(picture)
    header = struct.pack("<IiiHHIIiiII", 40, size, size * 2, 1, 32, 0, 0, 0, 0, 0, 0)
    colors = b"".join(bytes((blue, green, red, alpha)) for row in reversed(picture)
                      for red, green, blue, alpha in row)
    mask_row = ((size + 31) // 32) * 4
    mask = b"".join(bytes(mask_row) for _ in range(size))    # (the alpha says what shows)
    return header + colors + mask


def write_ico(path, pictures):
    images = [ico_image(picture) for picture in pictures]
    offset = 6 + 16 * len(images)
    directory = struct.pack("<HHH", 0, 1, len(images))
    for picture, image in zip(pictures, images):
        size = len(picture)
        directory += struct.pack("<BBBBHHII", size % 256, size % 256, 0, 0, 1, 32, len(image), offset)
        offset += len(image)
    path.write_bytes(directory + b"".join(images))


def main():
    source = Path(sys.argv[1]) if len(sys.argv) > 1 else ROOT / "rom"
    out_dir = Path(sys.argv[2]) if len(sys.argv) > 2 else ROOT / "build" / "cmake" / "data"
    rom = sonic_rom.load_set(source)
    picture = heads_picture(sonic_rom.build_program(rom), sonic_rom.build_sprites(rom))
    pictures = [picture, halved(picture)]
    pictures.append(halved(pictures[-1]))
    out_dir.mkdir(parents=True, exist_ok=True)
    write_ico(out_dir / "segasonic.ico", pictures)
    (out_dir / "segasonic.rc").write_text('1 ICON "%s"\n' % (out_dir / "segasonic.ico").as_posix(), encoding="ascii")
    print("icon: %s" % (out_dir / "segasonic.ico"))


if __name__ == "__main__":
    main()
