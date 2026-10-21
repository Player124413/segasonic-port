"""
Name:        bisect_code.py
Description: finds the bytes of the original code a scenario still reads as data: the ones the assets pack
             leaves out (the code) that keep it from matching the reference at a frame
Version:     0.1.0
Created:     20-10-2026
Modified:    20-10-2026
Author:      Sebastian Toyos

Usage: python tools/bisect_code.py SCENARIO FRAME

The game reads its data from the assets pack, which holds no code (tools/make_pack.py); the bytes of the code
the original also reads as data (through a null pointer, past a table's end) go into the assets by
assets.json's read_as_data. When a scenario stops matching the reference at FRAME and the C reads no code
there that a diagnostic sees (an index into a table the C reaches as rom(base) is applied after rom()), this
finds those bytes: s32ref dumps the board's memories at FRAME, then packs with parts of the code added run
the scenario to FRAME (sonic_game_headless --assets), split in halves until only the bytes it needs are left.
The V60's stack (20FE00-20FFFF) is left out of the comparison: the C has none. Writes build/bisect/.
"""
import subprocess
import sys
from pathlib import Path

TOOLS = Path(__file__).resolve().parent
ROOT = TOOLS.parent
sys.path.insert(0, str(TOOLS))
sys.path.insert(0, str(ROOT))
import extract_assets  # noqa: E402
import make_pack  # noqa: E402
import sonic_rom  # noqa: E402

BIN = ROOT / "build" / "bin"
WORK = ROOT / "build" / "bisect"
COMPARED = ("work_ram.bin", "sprite_ram.bin", "video_ram.bin", "palette.bin", "shared_ram.bin")
STACK = (0x20FE00 - 0x200000, 0x210000 - 0x200000)    # in work_ram.bin
MIRRORS = {"program": ((0x020000, 0x080000), (0x0A0000, 0x100000), (0x180000, 0x200000)),
           "sound": ((0x040000, 0x100000),)}           # the loader repeats what the pack holds there


class Bisection:
    def __init__(self, scenario, frame):
        self.scenario, self.frame, self.runs = scenario, frame, 0
        manifest = extract_assets.load_manifest()
        self.assets = make_pack.pack_entries(manifest)
        rom = sonic_rom.load_set(ROOT / "rom")
        self.regions = {name: builder(rom) for name, builder in extract_assets.REGION_BUILDERS.items()}
        self.code = sorted((region, low, high) for region, spans in make_pack.code_ranges(manifest).items()
                           for low, high in spans
                           if not any(start <= low < end for start, end in MIRRORS.get(region, ())))
        self.reference = WORK / "reference"

    def dump(self, command, folder):
        folder.mkdir(parents=True, exist_ok=True)
        for old in folder.iterdir():
            old.unlink()
        subprocess.run(command + ["--script", str(ROOT / "tests" / "controls" / f"{self.scenario}.txt"),
                                  "--frames", str(self.frame + 1), "--dump-frame", str(self.frame),
                                  "--dump-dir", str(folder)],
                       cwd=ROOT, stdout=subprocess.DEVNULL, stderr=subprocess.DEVNULL, check=False)

    def matches(self, folder):
        for name in COMPARED:
            one = bytearray((self.reference / name).read_bytes())
            other = bytearray((folder / name).read_bytes())
            if name == "work_ram.bin":
                one[STACK[0]:STACK[1]] = other[STACK[0]:STACK[1]] = bytes(STACK[1] - STACK[0])
            if one != other:
                return False
        return True

    def test(self, spans):
        """The scenario with the assets and these spans of the code matches the reference at the frame."""
        self.runs += 1
        entries = self.assets + [(make_pack.KIND_BYTES, region, f"code_{low:06X}", low, self.regions[region][low:high])
                                 for region, low, high in spans]
        make_pack.write_pack(WORK / "bisect.assets", entries)
        self.dump([str(BIN / "sonic_game_headless"), "--assets", str(WORK / "bisect.assets")], WORK / "game")
        return self.matches(WORK / "game")

    def needed(self, candidates, base):
        """A small part of 'candidates' that, with 'base', matches (base with all of them does)."""
        if self.test(base):
            return []
        if size(candidates) <= 1:
            return candidates
        first, second = halves(candidates)
        if self.test(base + first):
            return self.needed(first, base)
        if self.test(base + second):
            return self.needed(second, base)
        found = self.needed(first, base + second)
        return found + self.needed(second, base + found)


def size(spans):
    return sum(high - low for _, low, high in spans)


def halves(spans):
    half, taken, first, second = size(spans) // 2, 0, [], []
    for region, low, high in spans:
        if taken >= half:
            second.append((region, low, high))
        elif taken + high - low <= half:
            first.append((region, low, high))
            taken += high - low
        else:
            first.append((region, low, low + half - taken))
            second.append((region, low + half - taken, high))
            taken = half
    return first, second


def main():
    if len(sys.argv) != 3:
        raise SystemExit("Usage: python tools/bisect_code.py SCENARIO FRAME")
    bisection = Bisection(sys.argv[1], int(sys.argv[2]))
    bisection.dump([str(BIN / "s32ref")], bisection.reference)
    if bisection.test([]):
        print("the scenario matches the reference at that frame with the assets alone")
        return 0
    if not bisection.test(bisection.code):
        raise SystemExit("the scenario differs from the reference at that frame with all of the code too")
    found = bisection.needed(bisection.code, [])
    print(f"{bisection.runs} runs; the bytes of the code the scenario reads at frame {bisection.frame}:")
    for region, low, high in found:
        print(f"  {region} {low:06X}-{high - 1:06X}  {bisection.regions[region][low:high].hex(' ')}")
    return 0


if __name__ == "__main__":
    sys.exit(main())
