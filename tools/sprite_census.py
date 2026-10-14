"""
Name:        sprite_census.py
Description: finds where in the program ROM the palette of each sprite frame is, for extract_assets.py
Version:     0.1.0
Created:     14-10-2026
Modified:    14-10-2026
Author:      Sebastian Toyos

The frame records say nothing of colors: the object that draws a frame picks its palette. So the game is
run through the scenarios of tests/controls with the sprite census on (sonic_game_headless
--sprite-census: every sprite the list draws, with the color each pen shows), and each set of colors a
frame was drawn with is looked for in the program ROM, where the game keeps its palettes (16-bit RGB555
entries, the V60's byte order). What it writes, sprite_palettes.json, holds positions only: for each frame,
where its palette starts in the program ROM; extract_assets.py reads the colors from the user's ROM.
A frame no scenario draws takes the palette of the nearest frame before it that has one (the frames of a
character or an object lie together).

Usage:
  py tools/sprite_census.py run DIR       the census of every scenario into DIR (one file each)
  py tools/sprite_census.py resolve DIR   sprite_palettes.json from the census in DIR
"""
import json
import struct
import subprocess
import sys
from collections import Counter
from concurrent.futures import ThreadPoolExecutor
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]
sys.path.insert(0, str(ROOT / "tools"))
import asset_formats  # noqa: E402
import sonic_rom  # noqa: E402

OUTPUT = ROOT / "sprite_palettes.json"
FRAME_RECORDS = 0x130000          # sprite_frame_records (symbols.txt)
FRAME_COUNT = 0xBFD0 // asset_formats.SPRITE_RECORD
JOBS = 8


def run(out_dir):
    out_dir.mkdir(parents=True, exist_ok=True)
    runner = ROOT / "build" / "bin" / ("sonic_game_headless.exe" if sys.platform == "win32" else "sonic_game_headless")
    scripts = sorted((ROOT / "tests" / "controls").glob("*.txt"))

    def one(script):
        result = subprocess.run([str(runner), "--script", str(script), "--sprite-census",
                                 str(out_dir / (script.stem + ".txt"))], capture_output=True, text=True)
        return script.stem, result.returncode
    with ThreadPoolExecutor(JOBS) as pool:
        for name, code in pool.map(one, scripts):
            print(f"{name:<40} {'done' if code == 0 else 'failed'}", flush=True)


def read_census(census_dir):
    """{graphics word: Counter of color tuples}"""
    seen = {}
    for path in sorted(census_dir.glob("*.txt")):
        for line in path.read_text(encoding="ascii").splitlines():
            parts = line.split()
            word = int(parts[0], 16)
            colors = tuple(int(part, 16) for part in parts[4:])
            seen.setdefault(word, Counter())[colors] += 1
    return seen


def alternate_format(color):
    """A palette entry as the game writes it through the upper half of the palette window, and keeps it in
    the ROM (runtime/src/machine.c): the low bit of blue, green and red in bits 14-12, the rest 4:4:4."""
    red, green, blue = color & 0x1F, (color >> 5) & 0x1F, (color >> 10) & 0x1F
    return (color & 0x8000) | (blue & 1) << 14 | (green & 1) << 13 | (red & 1) << 12 \
        | (blue & 0x1E) << 7 | (green & 0x1E) << 3 | (red & 0x1E) >> 1


def find_palette(program, colors, index):
    """Where 'colors' (RGB555, as palette RAM holds them) start in the program ROM, or None: the whole set,
    else all but the last pen (the transparent one, whatever color it shows); in the format the game keeps
    them in, else as they are. 'index' maps a 4-byte prefix to its offsets."""
    for convert in (alternate_format, lambda color: color):
        for count in (len(colors), len(colors) - 1):
            wanted = struct.pack(f"<{count}H", *(convert(color) for color in colors[:count]))
            for offset in index.get(wanted[:4], ()):
                if program[offset:offset + len(wanted)] == wanted:
                    return offset
    return None


def resolve(census_dir):
    rom = sonic_rom.load_set(ROOT / "rom")
    program = sonic_rom.build_program(rom)
    frames = asset_formats.sprite_frames(program, FRAME_RECORDS, FRAME_COUNT)
    seen = read_census(census_dir)
    # every even offset of the program ROM by its first two colors
    index = {}
    for offset in range(0, len(program) - 4, 2):
        index.setdefault(program[offset:offset + 4], []).append(offset)
    found = {}
    missing = Counter()
    for frame in frames:
        sets = seen.get(frame["word"])
        if not sets:
            continue
        for colors, _ in sets.most_common():
            offset = find_palette(program, colors, index)
            if offset is not None:
                found[frame["number"]] = offset
                break
        else:
            missing["drawn, palette not in the ROM"] += 1
    # the frames no scenario draws: the palette of the nearest frame before them in the sprite ROM
    ordered = sorted(frames, key=lambda frame: frame["word"])
    guessed = {}
    last = None
    for frame in ordered:
        if frame["number"] in found:
            last = found[frame["number"]]
        elif last is not None:
            guessed[frame["number"]] = last
    palettes = {f"{number:04X}": f"0x{offset:06X}" for number, offset in sorted(found.items())}
    nearby = {f"{number:04X}": f"0x{offset:06X}" for number, offset in sorted(guessed.items())}
    OUTPUT.write_text(json.dumps({
        "notes": ["Positions only: where in the program ROM each sprite frame's palette starts (frame = its "
                  "sprite_frame, the record's offset in sprite_frame_records); tools/sprite_census.py writes it.",
                  "'drawn': the palette the frame was drawn with in the scenarios; 'nearby': a frame no scenario "
                  "draws, with the palette of the frame before it in the sprite ROM."],
        "drawn": palettes, "nearby": nearby}, indent=1) + "\n", encoding="ascii")
    print(f"{len(frames)} frames: {len(found)} drawn with a palette found in the ROM, {len(guessed)} take a nearby "
          f"frame's, {len(frames) - len(found) - len(guessed)} none; {dict(missing)}")
    print(f"written: {OUTPUT}")


def main():
    if len(sys.argv) != 3 or sys.argv[1] not in ("run", "resolve"):
        raise SystemExit(__doc__.split("Usage:")[1])
    if sys.argv[1] == "run":
        run(Path(sys.argv[2]))
    else:
        resolve(Path(sys.argv[2]))


if __name__ == "__main__":
    main()
