"""
Name:        check_nsp.py
Description: reads an .nsp the way the console does and checks every hash in it: the NCAs' headers
             deciphered with the console's keys, each section's hash table or IVFC levels, the content meta's
             hashes and sizes; and takes its files out to compare them with the ones they were made from
Version:     0.1.0
Created:     23-10-2026
Modified:    23-10-2026
Author:      Sebastian Toyos

Usage: python tools/check_nsp.py NSP KEYS [--pieces FOLDER] [--assets FILE]
  NSP       the .nsp (SegaSonic NSP's)
  KEYS      the console's keys (prod.keys: header_key, key_area_key_application_00); nothing of them is shown
  --pieces  the game's Switch build the .nsp was made from (main, main.npdm, control.nacp, icon.jpg)
  --assets  an assets pack made from the same ROM set (build/bin/segasonic.assets)

A check of SegaSonic NSP (ports/switch/nsp) by another implementation: AES from the cryptography package,
the formats read here as the console reads them (sectors deciphered one by one, the counters from the FS
headers, the RomFS walked through its tables). Exits 1 at the first thing that does not hold.
"""
import hashlib
import struct
import sys
from pathlib import Path

from cryptography.hazmat.primitives.ciphers import Cipher, algorithms, modes

CONTENT_TYPES = {0: "program", 1: "meta", 2: "control"}


class Failed(Exception):
    pass


def expect(condition, what):
    if not condition:
        raise Failed(what)


def keys_from(path):
    keys = {}
    for line in Path(path).read_text(encoding="ascii", errors="replace").splitlines():
        if "=" in line:
            name, value = (part.strip() for part in line.split("=", 1))
            keys[name] = value
    expect("header_key" in keys and "key_area_key_application_00" in keys, "the keys have no header_key or "
           "key_area_key_application_00")
    return bytes.fromhex(keys["header_key"]), bytes.fromhex(keys["key_area_key_application_00"])


def pfs0(data):
    """{name: bytes} of a PFS0, and its entries in order."""
    expect(data[:4] == b"PFS0", "not a PFS0")
    count, names_size = struct.unpack_from("<II", data, 4)
    names_at = 0x10 + 0x18 * count
    data_at = names_at + names_size
    files = {}
    for index in range(count):
        offset, size, name_offset = struct.unpack_from("<QQI", data, 0x10 + 0x18 * index)
        name = data[names_at + name_offset:data.index(b"\0", names_at + name_offset)].decode("ascii")
        files[name] = data[data_at + offset:data_at + offset + size]
        expect(len(files[name]) == size, f"{name}: past the PFS0's end")
    return files


def romfs(data):
    """{name: bytes} of a RomFS's root, walked through its file table and checked against its hash table."""
    header = struct.unpack_from("<10Q", data, 0)
    expect(header[0] == 0x50, "a RomFS header of another size")
    _, _, _, _, _, file_hash_at, file_hash_size, file_table_at, file_table_size, data_at = header
    buckets = file_hash_size // 4
    files = {}
    at = 0
    while at < file_table_size:
        parent, sibling, offset, size, _, name_size = struct.unpack_from("<IIQQII", data, file_table_at + at)
        name = data[file_table_at + at + 0x20:file_table_at + at + 0x20 + name_size].decode("ascii")
        files[name] = data[data_at + offset:data_at + offset + size]
        # the name's bucket leads to this entry
        hash_value = parent ^ 123456789
        for byte in name.encode("ascii"):
            hash_value = (((hash_value >> 5) | (hash_value << 27)) & 0xFFFFFFFF) ^ byte
        entry = struct.unpack_from("<I", data, file_hash_at + 4 * (hash_value % buckets))[0]
        while entry != at and entry != 0xFFFFFFFF:
            entry = struct.unpack_from("<I", data, file_table_at + entry + 0x18)[0]
        expect(entry == at, f"{name}: not in its hash bucket")
        at += 0x20 + (name_size + 3) // 4 * 4
        if sibling == 0xFFFFFFFF:
            break
        expect(sibling == at, f"{name}: its sibling is not the next entry")
    return files


def nca(data, header_key, key_area_key):
    """The NCA's content type, program id and its sections' contents ('pfs0' or 'romfs': {name: bytes})."""
    header = b""
    for sector in range(6):
        decryptor = Cipher(algorithms.AES(header_key), modes.XTS(sector.to_bytes(16, "big"))).decryptor()
        header += decryptor.update(data[sector * 0x200:(sector + 1) * 0x200]) + decryptor.finalize()
    expect(header[0x200:0x204] == b"NCA3", "the header does not decipher to NCA3")
    content_type, key_generation_old, key_index = header[0x205], header[0x206], header[0x207]
    size, program_id = struct.unpack_from("<QQ", header, 0x208)
    expect(size == len(data), "the content size is not the NCA's")
    expect(max(key_generation_old, header[0x220]) <= 1 and key_index == 0, "another key than the application's 00")
    expect(struct.unpack_from("<I", header, 0x21C)[0] >= 0x000B0000, "an SDK version the FS refuses")
    ecb = Cipher(algorithms.AES(key_area_key), modes.ECB()).decryptor()
    key_area = ecb.update(header[0x300:0x340]) + ecb.finalize()
    content_key = key_area[0x20:0x30]
    sections = []
    for index in range(4):
        start, end = struct.unpack_from("<II", header, 0x240 + 0x10 * index)
        if start == end == 0:
            continue
        fs_header = header[0x400 + 0x200 * index:0x600 + 0x200 * index]
        expect(hashlib.sha256(fs_header).digest() == header[0x280 + 0x20 * index:0x2A0 + 0x20 * index],
               f"section {index}: its FS header's hash")
        version, fs_type, hash_type, encryption = struct.unpack_from("<HBBB", fs_header, 0)
        expect(version == 2 and encryption == 3, f"section {index}: not a version 2 AES-CTR section")
        upper = fs_header[0x140:0x148][::-1]
        counter = upper + (start * 0x200 >> 4).to_bytes(8, "big")
        decryptor = Cipher(algorithms.AES(content_key), modes.CTR(counter)).decryptor()
        plain = decryptor.update(data[start * 0x200:end * 0x200]) + decryptor.finalize()
        if fs_type == 1:
            expect(hash_type == 2, f"section {index}: a PFS0 without its hash table")
            master = fs_header[0x08:0x28]
            block_size, layers = struct.unpack_from("<II", fs_header, 0x28)
            table_at, table_size, pfs0_at, pfs0_size = struct.unpack_from("<QQQQ", fs_header, 0x30)
            expect(layers == 2 and block_size & (block_size - 1) == 0, f"section {index}: its hash layers")
            table = plain[table_at:table_at + table_size]
            expect(hashlib.sha256(table).digest() == master, f"section {index}: its master hash")
            image = plain[pfs0_at:pfs0_at + pfs0_size]
            for block in range(0, len(image), block_size):
                expect(hashlib.sha256(image[block:block + block_size]).digest() == table[block // block_size * 0x20:
                       block // block_size * 0x20 + 0x20], f"section {index}: block {block // block_size}'s hash")
            sections.append(("pfs0", pfs0(image)))
        else:
            expect(fs_type == 0 and hash_type == 3 and fs_header[0x08:0x0C] == b"IVFC",
                   f"section {index}: a RomFS without IVFC levels")
            layers = struct.unpack_from("<I", fs_header, 0x14)[0]
            levels = [struct.unpack_from("<QQI", fs_header, 0x18 + 0x18 * level) for level in range(layers - 1)]
            master = fs_header[0xC8:0xE8]
            first_offset, first_size, _ = levels[0]
            expect(hashlib.sha256(plain[first_offset:first_offset + first_size]).digest() == master,
                   f"section {index}: its master hash")
            for level in range(len(levels) - 1):
                hashes_at, _, _ = levels[level]
                offset, size, order = levels[level + 1]
                block = 1 << order
                for at in range(0, size, block):
                    piece = plain[offset + at:offset + at + block].ljust(block, b"\0")
                    expect(hashlib.sha256(piece).digest() == plain[hashes_at + at // block * 0x20:
                           hashes_at + at // block * 0x20 + 0x20], f"section {index}: level {level + 1}'s hashes")
            offset, size, _ = levels[-1]
            sections.append(("romfs", romfs(plain[offset:offset + size])))
    return CONTENT_TYPES.get(content_type, str(content_type)), program_id, sections


def main():
    arguments = sys.argv[1:]
    options = {}
    for option in ("--pieces", "--assets"):
        if option in arguments:
            at = arguments.index(option)
            options[option] = Path(arguments[at + 1])
            del arguments[at:at + 2]
    if len(arguments) != 2:
        print(__doc__.split("Usage:")[1].split("\n\n")[0].strip())
        return 2
    nsp_path, keys_path = Path(arguments[0]), arguments[1]
    header_key, key_area_key = keys_from(keys_path)
    contents = pfs0(nsp_path.read_bytes())
    found = {}
    try:
        for name, data in contents.items():
            kind, program_id, sections = nca(data, header_key, key_area_key)
            expect(name.split(".")[0] == hashlib.sha256(data).hexdigest()[:32], f"{name}: not named by its hash")
            found[kind] = (name, data, program_id, sections)
            print(f"{name:45s} {kind:8s} {len(data):10d} bytes  title {program_id:016X}  "
                  f"{len(sections)} section(s), every hash holds")
        expect(set(found) == {"program", "control", "meta"}, "not a program, its control data and their meta")
        title = found["program"][2]
        expect(all(entry[2] == title for entry in found.values()), "NCAs of different titles")

        # the content meta lists the other two by hash and size
        cnmt_files = found["meta"][3][0][1]
        expect(list(cnmt_files) == [f"Application_{title:016x}.cnmt"], "the meta's file is not Application_TITLE.cnmt")
        cnmt = next(iter(cnmt_files.values()))
        meta_id, _, meta_type, extended, count = struct.unpack_from("<QIBxHH", cnmt, 0)
        expect(meta_id == title and meta_type == 0x80 and extended == 0x10, "the meta is not the title's application")
        for record in range(count):
            at = 0x20 + extended + 0x38 * record
            content_hash, content_id = cnmt[at:at + 0x20], cnmt[at + 0x20:at + 0x30]
            size = int.from_bytes(cnmt[at + 0x30:at + 0x36], "little")
            kind = {1: "program", 3: "control"}[cnmt[at + 0x36]]
            name, data = found[kind][0], found[kind][1]
            expect(hashlib.sha256(data).digest() == content_hash and content_id == content_hash[:16]
                   and size == len(data) and name.startswith(content_id.hex()), f"the meta's {kind} record")
        print(f"content meta: Application_{title:016x}.cnmt lists the program and the control data by their "
              f"hashes and sizes")

        # what they hold
        exefs, assets = found["program"][3][0][1], found["program"][3][1][1]
        control = found["control"][3][0][1]
        expect(set(exefs) == {"main", "main.npdm"} and set(assets) == {"segasonic.assets"}, "the program's files")
        expect("control.nacp" in control and "icon_AmericanEnglish.dat" in control, "the control data's files")
        nacp = control["control.nacp"]
        expect(nacp[0x3025] == 0 and nacp[0x30F1] == 0, "control.nacp: a user to pick at start, or its logo")
        if "--pieces" in options:
            pieces = options["--pieces"]
            expect(exefs["main"] == (pieces / "main").read_bytes(), "main is not the Switch build's")
            expect(exefs["main.npdm"] == (pieces / "main.npdm").read_bytes(), "main.npdm is not the Switch build's")
            expect(all(control[name] == (pieces / "icon.jpg").read_bytes() for name in control if name.startswith("icon_")),
                   "an icon is not the Switch build's")
            print("program and control data: the Switch build's files")
        if "--assets" in options:
            expect(assets["segasonic.assets"] == options["--assets"].read_bytes(),
                   f"segasonic.assets differs from {options['--assets']}")
            print(f"segasonic.assets: identical to {options['--assets']}")
    except Failed as failure:
        print(f"FAILED: {failure}")
        return 1
    print("the .nsp holds")
    return 0


if __name__ == "__main__":
    sys.exit(main())
