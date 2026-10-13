#!/usr/bin/env python3
"""
Name:        extract_assets.py
Description: extracts the game's assets from the user's own ROM, driven by assets.json (as in n64decomp/sm64)
Version:     0.1.0
Created:     11-10-2026
Modified:    11-10-2026
Author:      Sebastian Toyos

Usage:
  python extract_assets.py [rom folder or .zip]   extract what is missing (default ROM location: rom/)
  python extract_assets.py --force [rom]          extract everything again
  python extract_assets.py --clean                delete every extracted asset
"""
import json
import shutil
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parent
sys.path.insert(0, str(ROOT / "tools"))
import asset_formats  # noqa: E402
import sonic_rom  # noqa: E402

MANIFEST = ROOT / "assets.json"
STATE = ROOT / "assets" / ".extracted"

REGION_BUILDERS = {
    "program": sonic_rom.build_program,
    "tiles": sonic_rom.build_tiles,
    "sprites": sonic_rom.build_sprites,
    "sound": sonic_rom.build_sound,
}


def load_manifest():
    manifest = json.loads(MANIFEST.read_text(encoding="utf-8"))
    for asset in manifest["assets"]:
        asset["offset"] = int(asset["offset"], 0)
        asset["size"] = int(asset["size"], 0)
        if asset["region"] not in REGION_BUILDERS:
            raise SystemExit(f"assets.json: unknown region '{asset['region']}' for {asset['path']}")
    return manifest


def extract(asset, data):
    path = ROOT / asset["path"]
    kind = asset["type"]
    if kind == "binary":
        path.parent.mkdir(parents=True, exist_ok=True)
        path.write_bytes(data)
        return 1
    if kind == "tiles_16x16x4":
        path.parent.mkdir(parents=True, exist_ok=True)
        columns = asset["columns"]
        asset_formats.write_png_4bpp(path, columns * asset_formats.TILE_SIZE,
                                     asset_formats.tile_sheet_rows(data, columns))
        return 1
    if kind == "pcm_rf5c68":
        path.mkdir(parents=True, exist_ok=True)
        samples = asset_formats.split_rf5c68(data)
        for offset, sample in samples:
            pcm = asset_formats.rf5c68_to_pcm16(sample)
            asset_formats.write_wav(path / f"sample_{asset['offset'] + offset:06X}.wav", pcm, asset["rate"])
        return len(samples)
    raise SystemExit(f"assets.json: unknown type '{kind}' for {asset['path']}")


def remove(asset):
    path = ROOT / asset["path"]
    if path.is_dir():
        shutil.rmtree(path)
    elif path.exists():
        path.unlink()


def main():
    arguments = sys.argv[1:]
    manifest = load_manifest()
    if "--clean" in arguments:
        for asset in manifest["assets"]:
            remove(asset)
        STATE.unlink(missing_ok=True)
        print("Extracted assets removed")
        return 0

    force = "--force" in arguments
    arguments = [argument for argument in arguments if argument != "--force"]
    source = Path(arguments[0]) if arguments else ROOT / "rom"
    version = str(manifest["manifest_version"])
    up_to_date = STATE.exists() and STATE.read_text(encoding="ascii").strip() == version
    todo = [asset for asset in manifest["assets"]
            if force or not up_to_date or not (ROOT / asset["path"]).exists()]
    if not todo:
        print("Assets are up to date")
        return 0

    rom = sonic_rom.load_set(source)
    regions = {}
    files = 0
    for asset in todo:
        region = asset["region"]
        if region not in regions:
            regions[region] = REGION_BUILDERS[region](rom)
        remove(asset)
        data = regions[region][asset["offset"]:asset["offset"] + asset["size"]]
        files += extract(asset, data)
        print(f"  {asset['path']}")
    STATE.parent.mkdir(parents=True, exist_ok=True)
    STATE.write_text(version + "\n", encoding="ascii")
    print(f"{len(todo)} assets extracted ({files} files)")
    return 0


if __name__ == "__main__":
    sys.exit(main())
