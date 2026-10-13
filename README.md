# SegaSonic The Hedgehog — native port

A reconstruction of Sega's 1993 arcade game *SegaSonic The Hedgehog* (Sega System 32) into
platform-agnostic C — not an emulator — with ports to PC, Nintendo Switch and Linux ARM
handhelds (Anbernic RG40XX) on top of it.

**This repository contains no game code and no game data.** As with
[n64decomp/sm64](https://github.com/n64decomp/sm64), you bring your own ROM: the tools verify it,
extract the assets and generate everything else on your machine, and you build your own executable.

## Status

- [x] ROM set verification (MAME set `sonic`, Japan rev. C), by CRC32 and SHA-1 of every file
- [x] NEC V60 disassembler, output identical to MAME's at all 786,432 byte offsets of the program ROM
- [x] Recursive disassembly that follows branch tables, pointer tables and stored code pointers:
      526 functions, about 24,000 instructions, no invalid or overlapping paths
- [x] Asset extraction: tile sheets (PNG), sprite ROM banks, Z80 sound driver, 175 PCM samples (WAV)
- [ ] Remaining code coverage (object handlers that come from data tables)
- [ ] V60 to C generator
- [ ] System 32 video library (tilemaps, sprites, mixer) with swappable rendering backends
- [ ] Sound: Z80 driver, two YM3438, RF5C68
- [ ] Reimplementation of the FD1149 (317-0213) protection
- [ ] Platform layers: Windows and Linux, Nintendo Switch, Anbernic

## Getting started

Requirements: Python 3.10 or newer. Building the reference disassembler needs Visual Studio Build Tools.

1. Put your MAME `sonic.zip`, or the files inside it, in `rom/`.
2. Check it:

   ```bash
   python tools/verify_rom.py
   ```

3. Extract the assets into `assets/`:

   ```bash
   python extract_assets.py
   ```

4. Disassemble the program into `build/listing/sonic.s`, with a coverage report in `build/listing/report.txt`:

   ```bash
   python tools/disassemble.py
   ```

5. Optional, Windows: build MAME's V60 disassembler with `tools\reference\build_oracle.cmd` and compare it with ours:

   ```bash
   python tools/reference/compare_with_mame.py
   ```

## Layout

```
rom/                  your ROM (ignored by git)
assets.json           asset manifest: positions and formats, no data
extract_assets.py     extracts assets/ from your ROM
symbols.txt           names of known functions and data (addresses only)
tools/sonic_rom.py    ROM set definition and memory region layouts
tools/v60/            NEC V60 instruction decoder
tools/disassemble.py  recursive disassembler, listing and coverage report
tools/reference/      MAME's V60 disassembler, built only to cross-check ours
assets/, build/       generated on your machine (ignored by git)
```

## How the port will work

- **Game code.** The V60 machine code is translated to C at build time, from your ROM, so the result
  runs natively with no CPU emulation. Generated functions are then replaced, one at a time, by
  hand-written C in `src/`, which is the part of the code that lives in this repository.
- **Video.** The behavior of the System 32 video hardware (tilemaps, sprites, priorities, blending)
  becomes a library with a small backend interface, so it can be drawn through Vulkan, Direct3D,
  OpenGL ES or whatever a platform offers.
- **Sound.** The Z80 sound driver goes through the same translation; the YM3438 and RF5C68 chips
  become a sound library.
- **Platforms.** Each port only provides windowing, input, audio output and a video backend.

## Legal

- This project is licensed under the BSD 3-Clause License; see `LICENSE`.
- No ROM contents, generated code or extracted assets are distributed by this project.
- `tools/v60/disassembler.py` is derived from MAME's `v60d.cpp`, and `tools/reference/mame/` holds
  copies of MAME files. Those are BSD-3-Clause; see `tools/reference/mame/LICENSE`.
- SegaSonic The Hedgehog is a trademark of SEGA. This project is not affiliated with or endorsed by SEGA.
