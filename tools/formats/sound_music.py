"""
Name:        sound_music.py
Description: the sound program's music data: the sound command groups, the playlists, the songs table and
             its records as text, and the sequences the players read as Standard MIDI Files
Version:     0.1.0
Created:     14-10-2026
Modified:    14-10-2026
Author:      Sebastian Toyos

How the driver reads them (src/sound/sound_00.c, build/sound/listing.s):

- sound_groups (4A78): poll_sound_command takes sound_command_group & 7F (the V60 writes 80-83) as the index
  of a 16-bit pointer to the group's list. A list is its last number (a command above it plays nothing), then
  by number the address of the sound's header. Every list here has entries past its last number: they are
  never played. Text:
      group 0x80  0x4A80
      list 0x80  last 0x03
        0x00  0x52AB          # name of the place (sound_symbols.txt), not read back
- A sound header: its first byte, the flags, picks what happens (poll_sound_command, 48E0):
      bit 7 set:   an effect: bits 0-3 the priority (a lower one does not take a busy player); bit 6 clear:
                   the player at EB00, bit 6 set: EC00 + (channel of the first event - 0Ah); the sequence
                   follows the flags byte.
      bit 6 set:   the music player (E900), the sequence follows the flags byte.
      bit 5 set:   a song record: bank, then the 16-bit address of the sequence (start_sequence).
      else:        a playlist: the first song's number, then the playlist.
- songs (4C66): start_song: by song number, the address of a song record. Text: "song 0x05  0x4E5C" (a run of
  three or more equal entries: "songs 0x12-0x7F  0x4E94").
- song records: start_sequence copies the bank to the player's field 1B and the address to its position.
  An address in A000-BFFF is in the bank's window (chip offset = bank * 2000h + address - A000h); below A000
  the fixed part of the chip. Text: "record flags 0x20  bank 0x08  address 0xBDFD  # 011DFD name".
- playlists: when a sequence ends (step_player's end_of_sequence) the player reads the next playlist byte:
  00 stops it, 01-EF is the next song (start_song), F0-FF a command of playlist_command_table: F0 (and
  F3-FD) jumps to the 16-bit address after it and plays the song there, F1/F2 play song 90h + (E709/E70A & 7),
  FE stops this player, FF clears every player. The first song is the header's second byte, always a song
  (poll_sound_command starts it without looking at it). Text, one block a playlist:
      playlist flags 0x10  first 0x07
      song 0x08
      jump 0x4D73          # F0; "jump 0x4D73 command 0xF5" for the others
      end                  # 00 (also stop_player FE, stop_all_players FF, chosen_song_1 F1, chosen_song_2 F2)
  "hex .." keeps bytes that are no playlist.

- The sequences (step_player 4729, next_sequence_byte 4A64, chain_next_event 477F): an event stream much like
  the track of a Standard MIDI File, read once every four of YM3438 #1's timer A interrupts (1/822.5 s, a
  tick):
      event, delta time, event, delta time, ..., FF 2F
  The delta time comes after the event, the ticks to the next one: one byte 00-7F, or two bytes 1hhhhhhh
  0lllllll ((h << 7) | l: the variable-length quantity of MIDI files, two bytes at most). An event is a
  status byte (or none: the running status, the last status byte read, as in MIDI) and its data bytes:
      8n kk         note off: one data byte, the driver queues velocity 7F
      9n kk vv, An kk vv, Bn cc vv, En ll mm   two data bytes; Cn pp, Dn vv   one
      FF 2F         the end (the byte after it, 00 in the game's sequences, is not read)
      FF 51 ll vv   four bytes (meta_51_value); FF tt ll ...: ll bytes (00: 256); F0 ... F7; F7 ll ...;
                    F1-F6, F8-FE end the sequence too
  Bit 7 of an event's last data byte (a value under 80h always) chains: the next event follows at once,
  with no delta time byte. The game's sequences chain 26,022 times and write a 00 delta time 3,739 times,
  with no rule that tells which (the same events do both): both have to come back.

The sequences as MIDI files (format 0, one track, 384 ticks a quarter note, one tick of the file one tick
of the driver; the tempo event makes a player play them at the driver's speed, 128.5 quarter notes a
minute: the music's bars do not fall on the file's, its delta times come from real time). The track holds
the driver's events in order, each with the delta time that comes before the next one in the driver:
  - a channel event as itself, with the running status kept exactly where the driver has it (note off as
    8n kk 7F, the velocity the driver queues);
  - the delta time bytes as the driver has them; a chain is the delta time 00 and an explicit 00 after a
    channel event is written 80 00 (the same zero, two bytes); a two-byte 80 00 (the driver then waits
    65536 ticks) is written 84 80 00. A delta time of a sequencer's own (any variable-length quantity) is
    read too: 0 chains where the driver can.
  - what a MIDI file cannot hold as a channel event goes in a sequencer-specific meta event (FF 7F, the
    non-commercial manufacturer id 7D, then a tag), ignored by players:
      7D 43 'C'  a channel event of the driver a MIDI channel event cannot carry (a data byte over 7F, as
                 the driver's controller 80h, or a running status after a meta event): its bytes as the
                 driver has them, the chain bit cleared (the next delta time says it, as above)
      7D 45 'E'  any other event of the driver (meta and system events): its bytes as the driver has them
      7D 54 'T'  the bytes from here to the end of the piece as they are: what does not read as events
                 (after the delta time the meta event carries)
      7D 41 'A'  at the start: the bytes after the driver's FF 2F when they are not one 00
      7D 48 'H'  at the start: the flags byte of the sound header the sequence follows (z80_sound_sequence)
  - the file's own meta events (tempo, names, texts, markers) and system exclusive events are not the
    driver's: they are left out on the way back, their delta times added to the next event's.
  - the first event's delta time is not read back (the driver plays the first event at once).
On the way back the running status is kept where the file keeps it (a status byte is written when the
driver's running status is another), and a note off loses its velocity. A sequencer that saves the file
again writes its own delta times and running status: the bytes change (and their count with them).
"""
import re
import struct
from bisect import bisect_right
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
SYMBOLS = ROOT / "sound_symbols.txt"    # the names of places (load_names may point at another copy)
WINDOW = 0xA000                         # below: the chip as the Z80 sees it; A000-BFFF: a bank of it
BANK_SIZE = 0x2000


# ---- names of sound_symbols.txt ----

_NAMES = {}


def load_names(path=None):
    """Read the names again (from 'path' when given): the test points them at its copy of the symbols."""
    global SYMBOLS
    if path is not None:
        SYMBOLS = Path(path)
    _NAMES.clear()


def _names():
    """{'fixed': {address: name}, 'banked': {chip offset: name}} and their sorted places, from the table,
    data and banked lines of sound_symbols.txt."""
    if _NAMES:
        return _NAMES
    fixed, banked = {}, {}
    lines = SYMBOLS.read_text(encoding="utf-8").splitlines() if SYMBOLS.exists() else []
    for line in lines:
        match = re.match(r"([0-9A-F]{4,6})\s+(data|table|banked)\s+(\w+)", line)
        if not match:
            continue
        value, kind, name = int(match.group(1), 16), match.group(2), match.group(3)
        if kind == "banked":
            banked.setdefault(value, name)
        elif value < WINDOW:
            fixed.setdefault(value, name)
    _NAMES.update({"fixed": fixed, "banked": banked, "fixed_places": sorted(fixed), "banked_places": sorted(banked)})
    return _NAMES


def _name_of(value, banked=False):
    """The name of a place (name+0xNN inside a named piece), or None."""
    names = _names()
    table = names["banked" if banked else "fixed"]
    if value in table:
        return table[value]
    places = names["banked_places" if banked else "fixed_places"]
    index = bisect_right(places, value) - 1
    if index >= 0:
        return f"{table[places[index]]}+0x{value - places[index]:X}"
    return None


def _comment(name):
    return f"  # {name}" if name else ""


def _chip_offset(bank, address):
    """Where a bank and a Z80 address are in the chip (the fixed part below A000)."""
    return address if address < WINDOW else bank * BANK_SIZE + address - WINDOW


# ---- text helpers ----

def _lines(text):
    """The lines that matter, split in tokens: comments ('#' to the end of a line) and blank lines out."""
    for line in text.split("\n"):
        line = line.split("#", 1)[0].strip()
        if line:
            yield line.split()


def _number(token):
    return int(token, 16) if token.lower().startswith("0x") else int(token)


def _hex_lines(data):
    return [f"hex {' '.join(f'{byte:02X}' for byte in data[at:at + 16])}" for at in range(0, len(data), 16)]


def _hex_value(tokens):
    return bytes(int(token, 16) for token in tokens[1:])


def _u16(data, at):
    return data[at] | data[at + 1] << 8


def _word(value):
    if not 0 <= value <= 0xFFFF:
        raise ValueError(f"0x{value:X} is not a 16-bit value")
    return struct.pack("<H", value)


def _byte(value):
    if not 0 <= value <= 0xFF:
        raise ValueError(f"0x{value:X} is not a byte")
    return bytes([value])


# ---- sound_groups ----

GROUP_COUNT = 4    # the V60 sends groups 80-83

GROUPS_HEADER = """\
# sound_groups (poll_sound_command, 48E0): by sound_command_group & 7F, the address of the group's list.
# A list: its last number (a command above it plays nothing), then by number the address of the sound's
# header. Entries past the last number are never played. A name after '#' is where an address points.
"""


def _group_layout(data):
    """The offsets of the four lists in the piece, when they follow the pointers one after the other."""
    if len(data) < GROUP_COUNT * 2:
        return None
    pointers = [_u16(data, at * 2) for at in range(GROUP_COUNT)]
    base = pointers[0] - GROUP_COUNT * 2
    offsets = [pointer - base for pointer in pointers] + [len(data)]
    for start, end in zip(offsets, offsets[1:]):
        if end <= start or (end - start) % 2 != 1:
            return None
    return offsets


def sound_groups_to_text(data):
    offsets = _group_layout(data)
    if offsets is None:
        return GROUPS_HEADER + "\n".join(_hex_lines(data)) + "\n"
    lines = [GROUPS_HEADER.rstrip("\n")]
    for group in range(GROUP_COUNT):
        lines.append(f"group 0x{0x80 + group:02X}  0x{_u16(data, group * 2):04X}")
    for group in range(GROUP_COUNT):
        start, end = offsets[group], offsets[group + 1]
        last = data[start]
        lines += ["", f"list 0x{0x80 + group:02X}  last 0x{last:02X}"]
        for number, at in enumerate(range(start + 1, end, 2)):
            pointer = _u16(data, at)
            note = _name_of(pointer)
            if number > last:
                note = f"{note}, past the last number" if note else "past the last number"
            lines.append(f"  0x{number:02X}  0x{pointer:04X}{_comment(note)}")
    return "\n".join(lines) + "\n"


def sound_groups_from_text(text):
    out = bytearray()
    for tokens in _lines(text):
        if tokens[0] == "group":
            out += _word(_number(tokens[2]))
        elif tokens[0] == "list":
            if tokens[2] != "last":
                raise ValueError(f"not a list line: {' '.join(tokens)}")
            out += _byte(_number(tokens[3]))
        elif tokens[0] == "hex":
            out += _hex_value(tokens)
        else:
            out += _word(_number(tokens[1]))
    return bytes(out)


# ---- songs ----

SONGS_HEADER = """\
# songs (start_song, 4919): by song number, the address of the song's record {20h, bank, address of the
# sequence}; playlists name songs by these numbers. A name after '#' is where an address points.
"""
SHORTEST_RUN = 3


def songs_to_text(data):
    lines = [SONGS_HEADER.rstrip("\n")]
    count = len(data) // 2
    entries = [_u16(data, number * 2) for number in range(count)]
    number = 0
    while number < count:
        last = number
        while last + 1 < count and entries[last + 1] == entries[number]:
            last += 1
        pointer = entries[number]
        if last - number + 1 >= SHORTEST_RUN:
            lines.append(f"songs 0x{number:02X}-0x{last:02X}  0x{pointer:04X}{_comment(_name_of(pointer))}")
            number = last + 1
        else:
            lines.append(f"song 0x{number:02X}  0x{pointer:04X}{_comment(_name_of(pointer))}")
            number += 1
    lines += _hex_lines(data[count * 2:])
    return "\n".join(lines) + "\n"


def songs_from_text(text):
    out = bytearray()
    for tokens in _lines(text):
        if tokens[0] == "song":
            out += _word(_number(tokens[2]))
        elif tokens[0] == "songs":
            first, last = (_number(token) for token in tokens[1].split("-"))
            out += _word(_number(tokens[2])) * (last - first + 1)
        elif tokens[0] == "hex":
            out += _hex_value(tokens)
        else:
            raise ValueError(f"not a songs line: {' '.join(tokens)}")
    return bytes(out)


# ---- song records ----

RECORDS_HEADER = """\
# song records (start_sequence, 4937; songs points at them): the flags of a sound header (20h: a song
# record), then the sequence: by name for a song of the banked part ('sequence NAME': the build gives the
# bank and the address of wherever it lays the song out, so a song may change size), else its bank and
# address (A000-BFFF: in the bank's window; below A000: the fixed part, the bank is not used). After
# '#': where the sequence is in the chip, and its name.
"""


def _banked(offset):
    """The bank and the window address a song record gives for a place in the banked part."""
    return offset // BANK_SIZE, WINDOW + offset % BANK_SIZE


def _sequence_offset(name):
    """Where the sequence 'name' is: where the build lays it out (data_formats.PLACES, while it builds),
    else its banked line in sound_symbols.txt."""
    import data_formats
    if name in data_formats.PLACES:
        return data_formats.PLACES[name]
    for offset, known in _names()["banked"].items():
        if known == name:
            return offset
    raise ValueError(f"no sequence named {name}")


def song_records_to_text(data):
    lines = [RECORDS_HEADER.rstrip("\n")]
    count = len(data) // 4
    for at in range(0, count * 4, 4):
        flags, bank, address = data[at], data[at + 1], _u16(data, at + 2)
        offset = _chip_offset(bank, address)
        name = _name_of(offset, banked=address >= WINDOW)
        if address >= WINDOW and name and "+" not in name and _banked(offset) == (bank, address):
            lines.append(f"record flags 0x{flags:02X}  sequence {name}  # bank 0x{bank:02X}, address 0x{address:04X}: {offset:06X}")
            continue
        lines.append(f"record flags 0x{flags:02X}  bank 0x{bank:02X}  address 0x{address:04X}  # {offset:06X}"
                     + (f" {name}" if name else ""))
    lines += _hex_lines(data[count * 4:])
    return "\n".join(lines) + "\n"


def song_records_from_text(text):
    out = bytearray()
    for tokens in _lines(text):
        if tokens[0] == "record":
            fields = dict(zip(tokens[1::2], tokens[2::2]))
            if "sequence" in fields:
                bank, address = _banked(_sequence_offset(fields["sequence"]))
            else:
                bank, address = _number(fields["bank"]), _number(fields["address"])
            out += _byte(_number(fields["flags"])) + _byte(bank) + _word(address)
        elif tokens[0] == "hex":
            out += _hex_value(tokens)
        else:
            raise ValueError(f"not a song record line: {' '.join(tokens)}")
    return bytes(out)


# ---- playlists ----

PLAYLISTS_HEADER = """\
# playlists (poll_sound_command 48E0 starts the first song; step_player's end_of_sequence reads on when a
# song ends): 'playlist flags F first S' is the sound header (flags with bits 7-5 clear) and the first song;
# then song N (start_song), jump ADDRESS (F0: the playlist goes on there, the song there plays next; F3-FD
# do the same: 'command'), chosen_song_1 / chosen_song_2 (F1/F2: song 90h + E709/E70A & 7), stop_player
# (FE), stop_all_players (FF), end (00: the player stops).
"""
PLAYLIST_COMMANDS = {0xF1: "chosen_song_1", 0xF2: "chosen_song_2", 0xFE: "stop_player", 0xFF: "stop_all_players"}
PLAYLIST_CODES = {name: code for code, name in PLAYLIST_COMMANDS.items()}


def playlists_to_text(data):
    lines = [PLAYLISTS_HEADER.rstrip("\n")]
    at = 0
    while at < len(data):
        if at + 2 > len(data):
            lines += _hex_lines(data[at:])
            break
        lines += ["", f"playlist flags 0x{data[at]:02X}  first 0x{data[at + 1]:02X}"]
        at += 2
        while at < len(data):
            byte = data[at]
            if byte == 0:
                lines.append("end")
                at += 1
                break
            if byte < 0xF0:
                lines.append(f"song 0x{byte:02X}")
                at += 1
            elif byte in PLAYLIST_COMMANDS:
                lines.append(PLAYLIST_COMMANDS[byte])
                at += 1
            elif at + 3 <= len(data):
                target = _u16(data, at + 1)
                lines.append(f"jump 0x{target:04X}" + (f" command 0x{byte:02X}" if byte != 0xF0 else "")
                             + _comment(_name_of(target)))
                at += 3
                break
            else:
                lines += _hex_lines(data[at:])
                at = len(data)
    return "\n".join(lines) + "\n"


def playlists_from_text(text):
    out = bytearray()
    for tokens in _lines(text):
        word = tokens[0]
        if word == "playlist":
            fields = dict(zip(tokens[1::2], tokens[2::2]))
            out += _byte(_number(fields["flags"])) + _byte(_number(fields["first"]))
        elif word == "song":
            out += _byte(_number(tokens[1]))
        elif word == "end":
            out.append(0)
        elif word in PLAYLIST_CODES:
            out.append(PLAYLIST_CODES[word])
        elif word == "jump":
            command = _number(tokens[3]) if len(tokens) > 3 and tokens[2] == "command" else 0xF0
            out += _byte(command) + _word(_number(tokens[1]))
        elif word == "hex":
            out += _hex_value(tokens)
        else:
            raise ValueError(f"not a playlist line: {' '.join(tokens)}")
    return bytes(out)


# ---- the sequences: the driver's reading ----

CHAIN = None    # a delta time that is the chain bit of the event before (no delta time bytes)


class Event:
    """One event of the driver: its bytes as the stream has them (the chain bit of a channel event's last
    data byte cleared), and the delta time after it (bytes, or CHAIN)."""

    def __init__(self, raw, status, explicit, channel):
        self.raw = raw
        self.status = status
        self.explicit = explicit
        self.channel = channel    # a channel event (80-EF): the next delta time may be its chain bit
        self.delta = b""


def _read_event(data, at, running):
    """The event at 'at' as step_player reads it: (Event, end) or ('end', at) for an FF 2F with its status
    byte, None where the stream does not read as an event of its own (another end, or past the data)."""
    if at >= len(data):
        return None
    if data[at] & 0x80:
        status, first, explicit = data[at], at + 1, True
    elif running is not None:
        status, first, explicit = running, at, False
    else:
        return None
    if status == 0xFF:
        if first >= len(data):
            return None
        if data[first] == 0x2F:
            return ("end", at) if explicit else None
        if data[first] == 0x51:
            end = first + 3                                # meta_51: type, length, one value byte
        elif first + 1 < len(data):
            end = first + 2 + (data[first + 1] or 256)     # meta_event: DJNZ on the length byte
        else:
            return None
    elif status == 0xF0:
        end = first
        while end < len(data) and data[end] != 0xF7:
            end += 1
        end += 1
    elif status == 0xF7:
        if first >= len(data):
            return None
        end = first + (data[first] or 256)                 # the length byte counts itself
    elif status > 0xF0:
        return None                                        # end_of_sequence through system_event
    else:
        end = first + (1 if status & 0xF0 in (0x80, 0xC0, 0xD0) else 2)
    if end > len(data):
        return None
    raw = bytearray(data[at:end])
    channel = status < 0xF0
    if channel:
        raw[-1] &= 0x7F
    return Event(bytes(raw), status, explicit, channel), end


def _read_delta(data, event, end):
    """The delta time after the event: (bytes or CHAIN, where the next event starts), or None."""
    if event.channel and data[end - 1] & 0x80:
        return CHAIN, end
    if end >= len(data):
        return None
    if data[end] < 0x80:
        return data[end:end + 1], end + 1
    if end + 1 >= len(data) or data[end + 1] & 0x80:
        return None
    return data[end:end + 2], end + 2


def read_sequence(data):
    """The driver's events of a sequence: (events, end, rest). With end True the stream ends with FF 2F and
    rest is what follows it; else rest is what follows the last event's delta time and does not read."""
    events = []
    at, running = 0, None
    while True:
        found = _read_event(data, at, running)
        if found is None:
            return events, False, data[at:]
        if found[0] == "end":
            return events, True, data[at + 2:]
        event, end = found
        after = _read_delta(data, event, end)
        if after is None:
            return events, False, data[at:]
        event.delta, at = after
        if event.explicit:
            running = event.status
        events.append(event)


# ---- MIDI files ----

PPQN = 384                                       # ticks a quarter note in the files
TICK_SECONDS = 4 * 17 * 144 / 8053975            # four timer A periods: (1024 - 3EFh) x 144 YM3438 clocks
TEMPO = round(PPQN * TICK_SECONDS * 1e6)         # microseconds a quarter note (466866: 128.5 a minute)
PRIVATE = 0x7D                                   # the non-commercial manufacturer id
TAG_CHANNEL, TAG_EVENT, TAG_REST, TAG_AFTER_END, TAG_HEADER = b"C", b"E", b"T", b"A", b"H"


def _vlq(value):
    out = [value & 0x7F]
    value >>= 7
    while value:
        out.append(0x80 | value & 0x7F)
        value >>= 7
    return bytes(reversed(out))


def _meta(kind, payload):
    return bytes([0xFF, kind]) + _vlq(len(payload)) + payload


def _private(tag, payload):
    return _meta(0x7F, bytes([PRIVATE]) + tag + payload)


def _file_delta(delta, channel):
    """The delta time of the file for the driver's delta time after an event."""
    if delta is CHAIN:
        return b"\x00"
    if delta == b"\x00":
        return b"\x80\x00" if channel else b"\x00"
    if delta == b"\x80\x00":
        return b"\x84\x80\x00"                   # a delta time of 0 in two bytes: 65536 ticks
    return delta


def _header_text(flags):
    """What the sound header's flags say (poll_sound_command), for the file's text event."""
    if flags & 0x80:
        player = "the player of the first event's channel (EC00 + channel - 0Ah)" if flags & 0x40 else "the effects player (EB00)"
        return f"sound header 0x{flags:02X}: {player}, priority {flags & 0x0F}"
    if flags & 0x40:
        return f"sound header 0x{flags:02X}: the music player (E900)"
    return f"sound header 0x{flags:02X}"


def sequence_to_midi(data, flags=None):
    """A sequence (with the flags byte of its sound header before it, given apart) as a MIDI file."""
    events, ended, rest = read_sequence(data)
    track = bytearray(b"\x00" + _meta(0x51, TEMPO.to_bytes(3, "big")))
    if flags is not None:
        track += b"\x00" + _private(TAG_HEADER, bytes([flags]))
        track += b"\x00" + _meta(0x01, _header_text(flags).encode("ascii"))
    if ended and rest != b"\x00":
        track += b"\x00" + _private(TAG_AFTER_END, rest)
    running = None                               # the file's running status
    delta = b"\x00"                              # the first event's
    for event in events:
        track += delta
        data_bytes = event.raw[1:] if event.explicit else event.raw
        fits = event.channel and all(byte < 0x80 for byte in data_bytes) and (event.explicit or running == event.status)
        if fits:
            track += event.raw + (b"\x7F" if event.status & 0xF0 == 0x80 else b"")
            running = event.status
        else:
            track += _private(TAG_CHANNEL if event.channel else TAG_EVENT, event.raw)
            running = None
        delta = _file_delta(event.delta, event.channel)
    if ended:
        track += delta + b"\xFF\x2F\x00"
    else:
        track += delta + _private(TAG_REST, rest) + b"\x00\xFF\x2F\x00"
    header = b"MThd" + struct.pack(">IHHH", 6, 0, 1, PPQN)
    return header + b"MTrk" + struct.pack(">I", len(track)) + bytes(track)


def _read_vlq(track, at):
    value, start = 0, at
    while True:
        if at >= len(track):
            raise ValueError("a MIDI track ends inside a variable-length number")
        byte = track[at]
        at += 1
        value = value << 7 | byte & 0x7F
        if not byte & 0x80:
            return value, track[start:at], at


def _read_track(track):
    """[(time, delta bytes, kind, status, explicit, payload)] of a MIDI track: kind 'channel' (payload the
    data bytes), 'meta' (status the meta type), 'sysex'."""
    events, at, time, running = [], 0, 0, None
    while at < len(track):
        delta, raw_delta, at = _read_vlq(track, at)
        time += delta
        if at >= len(track):
            raise ValueError("a MIDI track ends after a delta time")
        byte = track[at]
        if byte == 0xFF:
            kind = track[at + 1]
            length, _, at = _read_vlq(track, at + 2)
            events.append((time, raw_delta, "meta", kind, True, track[at:at + length]))
            at += length
            if kind == 0x2F:
                break
            continue
        if byte in (0xF0, 0xF7):
            length, _, at = _read_vlq(track, at + 1)
            events.append((time, raw_delta, "sysex", byte, True, track[at:at + length]))
            at += length
            continue
        if byte & 0x80:
            if byte > 0xEF:
                raise ValueError(f"status byte {byte:02X} in a MIDI track")
            status, explicit = byte, True
            at += 1
        elif running is None:
            raise ValueError("a running status with no status byte before it in a MIDI track")
        else:
            status, explicit = running, False
        running = status
        count = 1 if status & 0xF0 in (0xC0, 0xD0) else 2
        events.append((time, raw_delta, "channel", status, explicit, track[at:at + count]))
        at += count
    return events


DRIVER_TAGS = (TAG_CHANNEL, TAG_EVENT, TAG_REST)


def _tag(event):
    """The tag of one of these files' sequencer-specific meta events (FF 7F 7D tag ...), or None."""
    if event[2] == "meta" and event[3] == 0x7F and len(event[5]) >= 2 and event[5][0] == PRIVATE:
        return event[5][1:2]
    return None


def _is_end(event):
    return event[2] == "meta" and event[3] == 0x2F


def _read_midi(blob):
    """The events of a MIDI file the driver's stream comes from, [(time, delta bytes, kind, status,
    explicit, payload, tag, the driver's)], and the file's private attributes {tag: bytes}. The track with
    the channel events (several: merged in time order, with the delta times a sequencer would write)."""
    if blob[:4] != b"MThd":
        raise ValueError("not a MIDI file")
    at, tracks = 8 + struct.unpack(">I", blob[4:8])[0], []
    while at + 8 <= len(blob):
        kind, length = blob[at:at + 4], struct.unpack(">I", blob[at + 4:at + 8])[0]
        if kind == b"MTrk":
            tracks.append(_read_track(blob[at + 8:at + 8 + length]))
        at += 8 + length
    attributes = {}
    for track in tracks:
        for event in track:
            if _tag(event) in (TAG_HEADER, TAG_AFTER_END):
                attributes.setdefault(_tag(event), event[5][2:])
    playing = [track for track in tracks if any(event[2] == "channel" or _tag(event) in DRIVER_TAGS for event in track)]
    if len(playing) > 1:
        merged = sorted((event for track in playing for event in track if not _is_end(event)), key=lambda event: event[0])
        end = max([event[0] for track in playing for event in track if _is_end(event)] + [merged[-1][0]])
        merged.append((end, b"", "meta", 0x2F, True, b""))
        events, time = [], 0
        for event in merged:
            events.append((event[0], _vlq(event[0] - time)) + tuple(event[2:]))
            time = event[0]
    else:
        events = playing[0] if playing else (tracks[0] if tracks else [])
    return [tuple(event) + (_tag(event), event[2] == "channel" or _tag(event) in DRIVER_TAGS or _is_end(event))
            for event in events], attributes


def _driver_delta(raw, value, plain, channel):
    """The driver's delta time for the file's (CHAIN or bytes); 'plain': no event of the file's own came
    between the two events (their raw delta time is the driver's)."""
    if plain:
        if raw == b"\x80\x00":
            return b"\x00"
        if raw == b"\x84\x80\x00":
            return b"\x80\x00"
        if len(raw) == 1 and value or len(raw) == 2 and value:
            return raw
    if value == 0:
        return CHAIN if channel else b"\x00"
    if value < 0x80:
        return bytes([value])
    if value < 0x4000:
        return bytes([0x80 | value >> 7, value & 0x7F])
    if value == 0x10000:
        return b"\x80\x00"
    raise ValueError(f"a delta time of {value} ticks: the driver waits at most 16383 between events")


def midi_to_sequence(blob):
    """A MIDI file back to the driver's bytes: (flags of the sound header or None, the sequence)."""
    events, attributes = _read_midi(blob)
    out = bytearray()
    running = None                    # the driver's running status
    last = None                       # where the last event's chain bit goes, None when it cannot chain
    pending, plain, started = 0, True, False
    after = attributes.get(TAG_AFTER_END, b"\x00")
    for time, raw_delta, kind, status, explicit, payload, tag, drives in events:
        value, _, _ = _read_vlq(raw_delta, 0)
        if not drives:
            pending += value
            plain = False
            continue
        if started:
            delta = _driver_delta(raw_delta, pending + value, plain, last is not None)
            if delta is CHAIN:
                out[last] |= 0x80
            else:
                out += delta
        started, pending, plain = True, 0, True
        if kind == "meta" and status == 0x2F:
            out += b"\xFF\x2F" + after
            break
        if tag == TAG_REST:
            out += payload[2:]
            break
        if kind == "channel":
            if explicit or status != running:
                out.append(status)
                running = status
            out += payload[:1] if status & 0xF0 == 0x80 else payload
            last = len(out) - 1
            continue
        raw = payload[2:]
        out += raw
        if raw and raw[0] & 0x80:
            running = raw[0]
        last = len(out) - 1 if tag == TAG_CHANNEL and raw else None
    else:
        # no end event in the file: the sequence ends right after its last event
        if started:
            delta = _driver_delta(b"", pending, False, last is not None)
            if delta is CHAIN:
                out[last] |= 0x80
            else:
                out += delta
        out += b"\xFF\x2F" + after
    flags = attributes.get(TAG_HEADER)
    return (flags[0] if flags else None), bytes(out)


def sequence_to_mid(data):
    return sequence_to_midi(data)


def sequence_from_mid(blob):
    return midi_to_sequence(blob)[1]


def sound_sequence_to_mid(data):
    """A sound header with its sequence after the flags byte (effects and inline music)."""
    if not data:
        return sequence_to_midi(b"")
    return sequence_to_midi(data[1:], flags=data[0])


def sound_sequence_from_mid(blob):
    flags, sequence = midi_to_sequence(blob)
    if flags is None:
        if sequence:
            raise ValueError("the MIDI file has no sound header flags")
        return b""
    return bytes([flags]) + sequence


FORMATS = {
    "z80_sound_groups": (".txt", sound_groups_to_text, sound_groups_from_text),
    "z80_songs": (".txt", songs_to_text, songs_from_text),
    "z80_song_records": (".txt", song_records_to_text, song_records_from_text),
    "z80_playlists": (".txt", playlists_to_text, playlists_from_text),
    "z80_sequence": (".mid", sequence_to_mid, sequence_from_mid, "binary"),
    "z80_sound_sequence": (".mid", sound_sequence_to_mid, sound_sequence_from_mid, "binary"),
}
