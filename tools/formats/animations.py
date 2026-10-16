"""
Name:        animations.py
Description: the animations of the characters and objects as text, and back, byte for byte: animations,
             tables of sprite frames, multi-part sprites' part lists and the steps animations pick
Version:     0.1.0
Created:     14-10-2026
Modified:    15-10-2026
Author:      Sebastian Toyos

An object plays an animation through its fields animation (a ROM pointer), animation_frame (the current
step), animation_timer and animation_speed. animate/animate_step (00DEA2/00DEB3) and their kin read it:
a word whose low byte is the number of the last step, then the steps, one after the other:
  frames    a step is the sprite frame (animate_step): 2 bytes
  flagged   the sprite flags (bits 6/7: x/y flip, ORed into sprite_flags) then the frame
            (animate_step_with_flags): 4 bytes
  moving    the frame, then a signed byte dx and a signed byte dy (animate_and_move adds them to the
            projected position, animate_around_anchor places the object at anchor + dx/dy,
            animate_with_screen_offset adds them to the screen position; the flips negate them): 4 bytes
A frame (FRAME below) reads as its sprite sheet and its index there (sonic/walking[3]: the fourth frame of
assets/sprites/sonic/walking.png), or frame_XXXX (the offset of its record in sprite_frame_records) where
no sheet holds it. The steps hold no time: each one lasts animation_speed frames, which the code that starts
the animation sets (a frame shown twice in a row is shown twice as long).

The text, one item a line, in the order of the bytes ('#' starts a comment):
  animation KIND [header=0xXXXX]   then its steps, one a line: the step number, then
     N FRAME [flip_x] [flip_y] [flags=0xXXXX]        (flagged)
     N FRAME dx=D dy=D                               (moving)
     N FRAME                                         (frames)
    the header word is the number of steps - 1 unless header= gives it
  frame FRAME                      a lone frame word
  step FRAME [flip_x]...           a lone (flags, frame) step
  offset +XXXX                     a word: bytes from the start of the piece to what it points at
  entry +XXXX [flip_x]...          a (flags, offset) pair
  pointer 0xXXXXXX                 a 32-bit program address
  words XXXX ... / bytes XX ...    what no reader explains, kept as it is
anim_frames, anim_moving and anim_flagged are one animation of their kind of steps; anim_frames_list and
anim_moving_list animations one after the other; name_banner a stage's name banner (its position, then
an animation of frames).

Tables of sprite frames (sprite_list and the tables a reader of its own picks from: spiked_roller_frames,
chooser_portrait_frames, text_frames...): one frame a line, 'NN: FRAME' (NN the index in hex; FFFF
reads 'end', where the lists that have one stop), a comment after it where the index means something;
sprite_list_headN: only the first N bytes are frames, the rest is kept as hex.

Multi-part sprites (part_lists, part_list_animation), the propeller machine's steps of two frames, the
last index before a list (last_index) and the tables the animation steps pick from (poses, heights,
starting steps, animation speeds) are records in columns (tools/formats/common.py): a keyword, then
'field value' pairs, the index in the comment.
"""
import re
import struct
from pathlib import Path

from formats.common import (KEPT, Out, Rec, byte_table, content, fmt, frame_text, frame_value, hex_lines, indexed,
                            join, number, raw, single, u16_at)

SUFFIX = ".anim.txt"
STEP_SIZES = {"frames": 2, "flagged": 4, "moving": 4}
FLIP_FLAGS = (("flip_x", 0x40), ("flip_y", 0x80))
LEFTOVER = "no reader of this piece reads these bytes as part of it: kept as they are"
TIMING = "A step lasts animation_speed frames, set by the code that starts the animation (not here)."
TEETERING_TABLE = 0x1128AE      # teetering_animations: its offsets reach the records after it
ROM_DATA = (0x100000, 0x180000)


# ---- names of places, for the comments only ----

_names = None


def _place_name(address):
    """'name' or 'name + 0xNN' for a program address in the data area, from symbols.txt ('' if none)."""
    global _names
    if _names is None:
        _names = []
        symbols = Path(__file__).resolve().parents[2] / "symbols.txt"
        try:
            for line in symbols.read_text(encoding="utf-8").splitlines():
                match = re.match(r"([0-9A-F]{6})\s+(data|table)\s+(\w+)", line)
                if match:
                    _names.append((int(match.group(1), 16), match.group(3)))
        except OSError:
            pass
        _names.sort()
    best = None
    for place, name in _names:
        if place > address:
            break
        best = (place, name)
    if best is None or address - best[0] >= 0x400:
        return ""
    return best[1] if best[0] == address else f"{best[1]} + 0x{address - best[0]:X}"


# ---- words and tokens ----

def _u16(data, at):
    return struct.unpack_from("<H", data, at)[0]


def _flags_tokens(flags):
    tokens = [name for name, bit in FLIP_FLAGS if flags & bit]
    rest = flags & ~0xC0
    if rest:
        tokens.append(f"flags=0x{rest:04X}")
    return tokens


def _step_text(kind, data, at):
    if kind == "frames":
        return frame_text(_u16(data, at))
    if kind == "flagged":
        return " ".join([f"{frame_text(_u16(data, at + 2)):<10}"] + _flags_tokens(_u16(data, at))).rstrip()
    dx, dy = struct.unpack_from("<bb", data, at + 2)
    return f"{frame_text(_u16(data, at)):<10} dx={dx:<4} dy={dy}"


def _hex_lines(data, start, end):
    """What no reader explains: 16-bit words (bytes when the run is odd), eight (sixteen) a line."""
    lines = []
    if (end - start) % 2 == 0:
        words = [f"{_u16(data, at):04X}" for at in range(start, end, 2)]
        lines += ["words " + " ".join(words[at:at + 8]) for at in range(0, len(words), 8)]
    else:
        values = [f"{byte:02X}" for byte in data[start:end]]
        lines += ["bytes " + " ".join(values[at:at + 16]) for at in range(0, len(values), 16)]
    return lines


# ---- reading the pieces: each reader gives (end, lines) or None when the bytes do not fit ----

def _read_animation(data, at, kind):
    if at + 2 > len(data):
        return None
    header = _u16(data, at)
    steps = (header & 0xFF) + 1
    end = at + 2 + steps * STEP_SIZES[kind]
    if end > len(data):
        return None
    lines = [f"animation {kind}" + (f" header=0x{header:04X}" if header >> 8 else "")]
    for index in range(steps):
        lines.append(f"  {index:2d} " + _step_text(kind, data, at + 2 + index * STEP_SIZES[kind]))
    return end, lines


def _read_frame(data, at):
    if at + 2 > len(data):
        return None
    return at + 2, ["frame " + frame_text(_u16(data, at))]


def _read_step(data, at):
    if at + 4 > len(data):
        return None
    return at + 4, ["step " + _step_text("flagged", data, at)]


def _read_teeter(data, at):
    """A first frame (shown at once), then the animation of frames."""
    first = _read_frame(data, at)
    animation = first and _read_animation(data, first[0], "frames")
    return animation and (animation[0], first[1] + animation[1])


def _read_jump(data, at):
    """The sprite flags and frame of the take-off, then the animation of (flags, frame) steps."""
    first = _read_step(data, at)
    animation = first and _read_animation(data, first[0], "flagged")
    return animation and (animation[0], first[1] + animation[1])


class _Piece:
    """The items of a piece by position; render() puts them in order and keeps the rest as hex."""

    def __init__(self, data, title):
        self.data = data
        self.title = title
        self.items = {}

    def add(self, at, read, notes=()):
        """Reads an item at 'at' (once); returns its end, or None when it does not fit."""
        if at in self.items:
            return self.items[at][0]
        found = read(self.data, at) if 0 <= at < len(self.data) else None
        if found:
            self.items[at] = (found[0], list(notes), found[1])
            return found[0]
        return None

    def render(self):
        blocks = []         # (start, end, notes, lines); lines None: bytes kept as hex
        at = 0
        for start in sorted(self.items):
            end, notes, item_lines = self.items[start]
            if start < at:          # inside an item already written: its bytes are there
                continue
            if start > at:
                blocks.append((at, start, [LEFTOVER], None))
            blocks.append((start, end, notes, item_lines))
            at = end
        if at < len(self.data):
            blocks.append((at, len(self.data), [LEFTOVER], None))
        lines = [f"# {line}" if line else "#" for line in self.title.split("\n")]
        for start, end, notes, item_lines in blocks:
            lines.append("")
            if len(blocks) > 1 or notes:
                lines.append(f"# +{start:04X}" + (": " + "; ".join(notes) if notes else ""))
            lines += item_lines if item_lines is not None else _hex_lines(self.data, start, end)
        return "\n".join(lines) + "\n"


# ---- the formats ----

def _facing_offsets(piece, read_target, what):
    """16 words, one a facing: offset from the start of the table to the facing's record."""
    table = piece.data[:32]
    if len(table) < 32:
        return
    lines = [f"offset +{_u16(table, facing * 2):04X}   # facing {facing}" for facing in range(16)]
    piece.items[0] = (32, [f"per facing 0-15, where its {what} starts (bytes from the start of the table)"], lines)
    users = {}
    for facing in range(16):
        users.setdefault(_u16(table, facing * 2), []).append(str(facing))
    for target, facings in sorted(users.items()):
        piece.add(target, read_target, ["facing" + ("s " if len(facings) > 1 else " ") + ", ".join(facings)])


def facing_animations_to_text(data):
    piece = _Piece(data, "Per facing (the player's facing, 0-15) an animation of (sprite flags, frame) steps.\n"
                         "The player handlers point animation at table + offset[facing] (skidding: facing + 8)\n"
                         "and step it with animate_step_with_flags. Standing, skidding (speed 3), shocked\n"
                         "(speed 2) and tumbling (speed 4) step on the timer, animation_speed frames a step;\n"
                         "walking and running step when walk_cycle wraps (it grows with the speed); hurt and\n"
                         "the attack effect (the player's animation_frame) pick the step themselves.")
    _facing_offsets(piece, lambda data, at: _read_animation(data, at, "flagged"), "animation")
    return piece.render()


def facing_jumps_to_text(data):
    piece = _Piece(data, "The jump, per facing (0-15): the take-off step (sprite flags and frame, set at once),\n"
                         "then the animation of (sprite flags, frame) steps (player_animate_jumping,\n"
                         "player_launch: animation = record + 4; animation_speed 1 at attack power 4, else 2,\n"
                         "3 when launched).")
    _facing_offsets(piece, _read_jump, "take-off step")
    return piece.render()


def teeter_offsets_to_text(data):
    """teetering_animations: [character x 2 + variant] offsets from the table to a teeter record."""
    piece = _Piece(data, "Teetering (player state 5): [character x 2 + variant] the offset from this table to a\n"
                         "teeter record (first frame, then the animation). Characters 0 Sonic, 1 Ray, 2 Mighty;\n"
                         "variant 0 the far edges (sides 0/1), 1 the near edges (sides 2/3).")
    count = min(6, len(data) // 2)
    lines = []
    for index in range(count):
        offset = _u16(data, index * 2)
        target = TEETERING_TABLE + offset
        name = _place_name(target)
        character = ("Sonic", "Ray", "Mighty")[index // 2]
        lines.append(f"offset +{offset:04X}   # {character}, {('far', 'near')[index % 2]} edges: "
                     f"0x{target:06X}" + (f" {name}" if name else ""))
    if lines:
        piece.items[0] = (count * 2, [], lines)
    for index in range(count):
        offset = _u16(data, index * 2)
        if count * 2 <= offset < len(data):
            piece.add(offset, _read_teeter)
    return piece.render()


def teeter_to_text(data):
    piece = _Piece(data, "A teeter record (player state 5): the frame shown at once, then the animation of\n"
                         "frames (animation = record + 2). Falling off (state 6) plays the moving animation\n"
                         "at animation + 0x1C, the one right after this record.\n" + TIMING)
    piece.add(0, _read_teeter)
    return piece.render()


def _animation_text(kind, title):
    """to_text of a piece that is one animation of 'kind' steps."""
    def to_text(data):
        piece = _Piece(data, title)
        piece.add(0, lambda data, at: _read_animation(data, at, kind))
        return piece.render()
    return to_text


def _animation_list_text(kind, title):
    """to_text of a piece of animations of 'kind' steps one after the other."""
    def to_text(data):
        piece = _Piece(data, title)
        at = 0
        while at < len(data):
            end = piece.add(at, lambda data, at: _read_animation(data, at, kind))
            if end is None:
                break
            at = end
        return piece.render()
    return to_text


frames_animation_to_text = _animation_text(
    "frames", "An animation of frames (animate, animate_step): each step is a sprite frame.\n" + TIMING)


def edge_hang_to_text(data):
    piece = _Piece(data, "Hanging from an edge (player states 7 and 8): the code reaches each part at a fixed\n"
                         "offset from the start of the set (player_animate_falling and player_state_over_edge\n"
                         "pick the far or the near set by the ledge side).")
    frames = lambda data, at: _read_animation(data, at, "frames")
    parts = ((frames, "hanging on (state 7, animation_speed 1)"),
             (frames, "climbing back (state 7: animation += 0x16, animation_speed 2)"),
             (frames, "the slip effect, an object of its own (animation + 0x22, animation_speed 3)"),
             (_read_frame, "state 8 at its start (animation + 0x36)"),
             (_read_frame, "state 8 when the slip effect appears (animation + 0x38)"))
    at = 0
    for read, note in parts:
        end = piece.add(at, read, [note])
        if end is None:
            break
        at = end
    if at == 0x3A and len(data) - at == 2:
        piece.add(at, _read_frame, ["Mighty: the slip piece's frame (spawn_slip_piece, animation + 0x3A)"])
    elif at == 0x3A:
        piece.add(at, frames, ["Ray: the slip pieces' animation (spawn_slip_pieces, animation + 0x3A, "
                               "animation_speed 4)"])
    return piece.render()


def knocked_out_to_text(data):
    piece = _Piece(data, "Knocked out (player_state_tumbling with no health left): [facing >> 2] the sprite flags\n"
                         "and the offset from this table to the animation of frames (animation_speed 3: a step\n"
                         "lasts 3 frames).")
    count = 4 if len(data) >= 16 else 0
    lines = []
    for index in range(count):
        flags, offset = struct.unpack_from("<HH", data, index * 4)
        lines.append(" ".join([f"entry +{offset:04X}"] + _flags_tokens(flags))
                     + f"   # facings {index * 4}-{index * 4 + 3}")
    if lines:
        piece.items[0] = (count * 4, [], lines)
    for index in range(count):
        offset = _u16(data, index * 4 + 2)
        piece.add(offset, lambda data, at: _read_animation(data, at, "frames"))
    return piece.render()


def animation_pointers_to_text(data):
    piece = _Piece(data, "Pointers to animations of frames, chosen by how far the player got in escaping\n"
                         "(stuck: escape_progress / 0x5555; trapped: escape_progress >> 16).\n" + TIMING)
    at = 0
    lines = []
    while at + 4 <= len(data):
        pointer = struct.unpack_from("<I", data, at)[0]
        if not ROM_DATA[0] <= pointer < ROM_DATA[1]:
            break
        name = _place_name(pointer)
        lines.append(f"pointer 0x{pointer:06X}" + (f"   # {name}" if name else ""))
        at += 4
    if lines:
        piece.items[0] = (at, [], lines)
    while at < len(data):
        end = piece.add(at, lambda data, at: _read_animation(data, at, "frames"))
        if end is None:
            break
        at = end
    return piece.render()


# ---- back to bytes: one parser for every format ----

def _parse_frame(token):
    if token.endswith("]"):
        return frame_value(token, "a frame")
    if token.startswith("frame_"):
        value = int(token[6:], 16)
    elif token.lower().startswith("0x"):
        value = int(token, 16)
    else:
        raise ValueError(f"not a frame: {token}")
    if not 0 <= value <= 0xFFFF:
        raise ValueError(f"frame out of range: {token}")
    return value


def _parse_flags(tokens):
    flags = 0
    for token in tokens:
        for name, bit in FLIP_FLAGS:
            if token == name:
                flags |= bit
                break
        else:
            if token.startswith("flags="):
                flags |= int(token[6:], 16)
            else:
                raise ValueError(f"not a flag: {token}")
    return flags


def _parse_signed_byte(token, name):
    if not token.startswith(name + "="):
        raise ValueError(f"expected {name}=: {token}")
    value = int(token[len(name) + 1:])
    if not -128 <= value <= 127:
        raise ValueError(f"{name} out of range: {token}")
    return value


def _step_bytes(kind, tokens):
    if kind == "frames":
        if len(tokens) != 1:
            raise ValueError(f"a step of frames is one frame: {' '.join(tokens)}")
        return struct.pack("<H", _parse_frame(tokens[0]))
    if kind == "flagged":
        return struct.pack("<HH", _parse_flags(tokens[1:]), _parse_frame(tokens[0]))
    if len(tokens) != 3:
        raise ValueError(f"a moving step is frame dx= dy=: {' '.join(tokens)}")
    return struct.pack("<Hbb", _parse_frame(tokens[0]), _parse_signed_byte(tokens[1], "dx"),
                       _parse_signed_byte(tokens[2], "dy"))


def animations_from_text(text):
    out = bytearray()
    animation = None        # [kind, header or None, step bytes]

    def close():
        if animation is None:
            return
        kind, header, steps = animation
        if not steps:
            raise ValueError("an animation without steps")
        if header is None:
            if len(steps) > 0x100:
                raise ValueError("more than 256 steps")
            header = len(steps) - 1
        out.extend(struct.pack("<H", header))
        for step in steps:
            out.extend(step)

    for raw in text.split("\n"):
        tokens = raw.split("#", 1)[0].split()
        if not tokens:
            continue
        keyword = tokens[0]
        if keyword.isdigit():
            if animation is None:
                raise ValueError(f"a step outside an animation: {raw.strip()}")
            if int(keyword) != len(animation[2]):
                raise ValueError(f"step {keyword} where step {len(animation[2])} was due")
            animation[2].append(_step_bytes(animation[0], tokens[1:]))
            continue
        close()
        animation = None
        if keyword == "animation":
            if len(tokens) < 2 or tokens[1] not in STEP_SIZES:
                raise ValueError(f"animation of what: {raw.strip()}")
            header = None
            for token in tokens[2:]:
                if not token.startswith("header="):
                    raise ValueError(f"not an animation option: {token}")
                header = int(token[7:], 16)
            animation = [tokens[1], header, []]
        elif keyword == "frame":
            out += struct.pack("<H", _parse_frame(tokens[1]))
        elif keyword == "step":
            out += struct.pack("<HH", _parse_flags(tokens[2:]), _parse_frame(tokens[1]))
        elif keyword == "offset":
            out += struct.pack("<H", int(tokens[1].lstrip("+"), 16))
        elif keyword == "entry":
            out += struct.pack("<HH", _parse_flags(tokens[2:]), int(tokens[1].lstrip("+"), 16))
        elif keyword == "pointer":
            out += struct.pack("<I", int(tokens[1], 16))
        elif keyword == "words":
            out += b"".join(struct.pack("<H", int(word, 16)) for word in tokens[1:])
        elif keyword == "bytes":
            out += bytes(int(byte, 16) for byte in tokens[1:])
        else:
            raise ValueError(f"unknown item: {raw.strip()}")
    close()
    return bytes(out)


FORMATS = {
    "anim_facing": (SUFFIX, facing_animations_to_text, animations_from_text),
    "anim_facing_jumps": (SUFFIX, facing_jumps_to_text, animations_from_text),
    "anim_teeter_offsets": (SUFFIX, teeter_offsets_to_text, animations_from_text),
    "anim_teeter": (SUFFIX, teeter_to_text, animations_from_text),
    "anim_frames": (SUFFIX, frames_animation_to_text, animations_from_text),
    "anim_frames_list": (SUFFIX, _animation_list_text(
        "frames", "Animations of frames, one after the other (each one reached by its own pointer).\n" + TIMING),
        animations_from_text),
    "anim_moving": (SUFFIX, _animation_text(
        "moving", "An animation of (frame, dx, dy) steps: the edge falls move the projected position\n"
                  "by dx/dy each step (animate_and_move); the climb over the top places the player at\n"
                  "the anchor + dx/dy (animate_around_anchor). The x/y flips negate them.\n" + TIMING),
        animations_from_text),
    "anim_moving_list": (SUFFIX, _animation_list_text(
        "moving", "Animations of (frame, dx, dy) steps, one after the other (each reached by its\n"
                  "own pointer).\n" + TIMING), animations_from_text),
    "anim_flagged": (SUFFIX, _animation_text(
        "flagged", "An animation of (sprite flags, frame) steps (animate_step_with_flags): the\n"
                   "flags (flip_x/flip_y) are ORed into sprite_flags.\n" + TIMING), animations_from_text),
    "anim_edge_hang": (SUFFIX, edge_hang_to_text, animations_from_text),
    "anim_knocked_out": (SUFFIX, knocked_out_to_text, animations_from_text),
    "anim_pointers": (SUFFIX, animation_pointers_to_text, animations_from_text),
}


# ---- the stage name banners (game mode 0x17) ----

def name_banner_to_text(data):
    header = ["A stage's name banner (game_mode_stage_clear: stage_name_banners[stage]): its screen x and y,",
              "then the animation of frames the banner plays (animation = record + 4, animation_speed 4)."]
    if len(data) < 4:
        return join(header, hex_lines(data))
    x, y = struct.unpack_from("<HH", data, 0)
    return join(header, [f"position  x {x}  y {y}"]) + frames_animation_to_text(data[4:])


def name_banner_from_text(text):
    out = bytearray()
    rest = []
    for line in text.split("\n"):
        tokens = line.split("#", 1)[0].split()
        if tokens and tokens[0] == "position":
            if out or len(tokens) != 5 or tokens[1] != "x" or tokens[3] != "y":
                raise ValueError(f"'position x N y N' goes first: {line.strip()}")
            out += struct.pack("<HH", number(tokens[2], "position") & 0xFFFF, number(tokens[4], "position") & 0xFFFF)
        else:
            rest.append(line)
    return bytes(out) + animations_from_text("\n".join(rest))


FORMATS["name_banner"] = (SUFFIX, name_banner_to_text, name_banner_from_text)


# ---- tables of sprite frames ----

SPRITE_LIST_HEADER = ["sprite frames, one a line: the index (hex), then the frame (its sheet in assets/sprites and its "
                      "index there)"]


def sprite_list_format(header=SPRITE_LIST_HEADER, comment=None, head=None, leftover=None):
    """Sprite frame words, one a line: 'NN: FRAME' (the index in hex; FFFF reads 'end', where the
    lists that have one stop), then comment(index) when it gives one. With 'head', only the first 'head'
    bytes are sprites: the rest is kept as hex, with the note 'leftover'."""

    def to_text(data):
        size = len(data) if head is None else min(head, len(data))
        count = size // 2
        lines = []
        for index in range(count):
            value = struct.unpack_from("<H", data, index * 2)[0]
            shown = "end" if value == 0xFFFF else frame_text(value)
            note = comment(index) if comment else None
            lines.append(f"{index:02X}: " + (f"{shown:<10}  # {note}" if note else shown))
        rest = data[count * 2:]
        if rest:
            note = leftover or (KEPT if head is not None else "a byte past the last word (padding)")
            lines += hex_lines(rest, note)
        return join(header, lines)

    def from_text(text):
        out = bytearray()
        for line_number, tokens in content(text):
            where = f"line {line_number}"
            found = raw(tokens, where)
            if found is not None:
                out += found
                continue
            for token in tokens:
                if token.endswith(":"):
                    continue
                value = 0xFFFF if token == "end" else frame_value(token, where)
                if not 0 <= value <= 0xFFFF:
                    raise ValueError(f"{where}: {token} does not fit in a word")
                out += struct.pack("<H", value)
        return bytes(out)

    return (".txt", to_text, from_text)


def _character_comment(code):
    """The character a text code stands for, quoted (ASCII 20-7E), or None."""
    return f"'{chr(code)}'" if 0x20 <= code < 0x7F else None


def _by_character_step(index):
    """[character x 4 + step]: the character and the step."""
    return f"{('Sonic', 'Ray', 'Mighty')[index // 4] if index < 12 else index // 4} step {index % 4}"


FORMATS.update({
    "sprite_list": sprite_list_format(),
    "spiked_roller_frames": sprite_list_format(
        ["spiked_roller_frames: the roller's animation (spiked_roller_animation, last step 8: these 9 frames);",
         "spiked_roller_start and spawn_door_rollers set sprite_frame from it by the starting step."]),
    "floor_sign_sprites": sprite_list_format(
        ["floor_sign_sprites (floor_sign_start): by field 04, the sign's sprite frame; entry 1 is the first word",
         "of screen_sign_kinds (6378)."]),
    "chooser_portrait_frames": sprite_list_format(
        ["chooser_portrait_frames: [character x 4 + step] the portrait's sprite frame; step 3 is the chosen pose."],
        _by_character_step),
    "ranking_stage_sprites": sprite_list_format(
        ["ranking_stage_sprites (build_ranking_display): by stages_reached % 9 - 1, the sprite of the row's stage",
         "label: STAGE 1 ... STAGE 7, then ALL CLEAR (index 7); 8 and 9 are not reached."], head=20),
    "title_smoke_puff_frames": sprite_list_format(
        ["title_smoke_puff_frames (title_smoke_puff_animate): the puff's sprite every 3 frames; FFFF ends (the",
         "puff is removed)."], head=20),
    "text_frames": sprite_list_format(
        ["text_frames (draw_queued_strings): the sprite frame of each character code of a queued string (the",
         "index); 0 is a blank (nothing drawn; the next character moves on by its width)."], _character_comment),
})
for _bytes in (4, 8, 14, 22):
    FORMATS[f"sprite_list_head{_bytes}"] = sprite_list_format(head=_bytes)


# ---- multi-part sprites: part lists and the animations that step through them ----

_PART_COUNT = Rec("list", 2, [("parts", 0, "H")])
_PART = Rec("part", 8, [("frame", 0, "H", "frame"), ("flags", 2, "H", "x"), ("dx", 4, "b"), ("dy", 5, "b"),
                        ("palette", 6, "H", "x")])
_PART_HEADER = [
    "Part lists of multi-part sprites (build_sprite_list, sprite_flags C000/D00x; the object's 04-07 point at",
    "one): the number of parts, then each part's sprite frame, its flags (XORed with the object's), dx, dy",
    "from the object's screen position (signed bytes, scaled with the object) and its palette word.",
]


def _read_part_list(out):
    count = u16_at(out.data, out.at)
    if count is None or count > 16 or out.at + 2 + count * 8 > len(out.data):
        return False
    out.rec(_PART_COUNT)
    for _ in range(count):
        out.rec(_PART)
    return True


def _part_lists_text(data):
    out = Out(data, _PART_HEADER)
    while out.at < len(data):
        if out.at:
            out.blank()
            out.comment(f"+{out.at:04X}")
        if not _read_part_list(out):
            break
    return out.done()


_PART_ANIMATION = Rec("animation", 2, [("last", 0, "H")])
_PART_STEP = Rec("step", 2, [("list", 0, "H", "rel")])


def _part_animation_text(data):
    """A part-list animation (animate_part_list_step): the last step, then per step the offset from the
    animation to its part list (sprite_frame, 32-bit, points there); then the part lists."""
    out = Out(data, ["A part-list animation (animate_part_list_step): the last step, then per step the offset",
                     "from the start of the animation to the part list shown (the object's 04-07 point there).",
                     ""] + _PART_HEADER)
    last = u16_at(data, 0)
    if last is None or last > 0x40 or 2 + (last + 1) * 2 > len(data):
        return out.done()
    out.rec(_PART_ANIMATION)
    for step in range(last + 1):
        out.rec(_PART_STEP, f"step {step}")
    while out.at < len(data):
        out.blank()
        out.comment(f"+{out.at:04X}")
        if not _read_part_list(out):
            break
    return out.done()


# propeller_machine_animation: steps of two frames
_PAIR_ANIMATION = Rec("animation", 2, [("last", 0, "H")])
_PAIR_STEP = Rec("step", 4, [("frame_a", 0, "H", "frame"), ("frame_b", 2, "H", "frame")])


def _propeller_animation_text(data):
    out = Out(data, ["propeller_machine_animation (propeller_machine_chase): the last step, then per step two",
                     "frames, which the machine writes into its propeller parts."])
    last = u16_at(data, 0)
    if last is not None and last < 0x40:
        out.rec(_PAIR_ANIMATION)
        for step in range(last + 1):
            if not out.rec(_PAIR_STEP, f"step {step}"):
                break
    return out.done()


FORMATS.update({
    "part_lists": fmt(_part_lists_text, _PART_COUNT, _PART),
    "part_list_animation": fmt(_part_animation_text, _PART_ANIMATION, _PART_STEP, _PART_COUNT, _PART),
    "propeller_machine_animation": fmt(_propeller_animation_text, _PAIR_ANIMATION, _PAIR_STEP),
    "last_index": single("last", "H", header=["The last index of the list that follows (the code wraps or stops",
                                              "after it)."]),
})


# ---- steps and poses: what an animation step picks ----

_BITE_STEP = Rec("step", 4, [("frame", 0, "H", "frame"), ("hold", 2, "B"), ("unused", 3, "B", "x")])

FORMATS.update({
    "spiked_roller_part_frames": indexed(
        ["spiked_roller_part_frames: the starting animation step of each part (they do not turn in step)."],
        Rec("part", 1, [("step", 0, "B")]), "part"),
    "burning_boulder_roll_rates": indexed(
        ["burning_boulder_roll_rates: the rolling animation's speed (frames a step) by the speed >> 4."],
        Rec("rate", 1, [("frames", 0, "B")]), "speed >> 4 ="),
    "biting_head_bite_step_records": indexed(
        ["biting_head_bite_step_records (biting_head_chase, part 3: the jaw): by step (biting_head_bite_steps' last",
         "index is 5) the jaw's sprite frame and the frames it holds; step 5 starts biting_head_bounce."],
        _BITE_STEP, "step"),
    "chooser_idle_steps": indexed(
        ["chooser_idle_steps (character_chooser_wait): the idle portrait's step by field 80 (0-3)."],
        Rec("step", 1, [("pose", 0, "B")]), lambda index: f"field 80 = {index}"),
    "title_sonic_rise_heights": byte_table(
        ["title_sonic_rise_heights (the title): by title_sonic_step (0-13 hex), Sonic's height above y 70 as he",
         "rises out of the emblem; the last byte (FF) is not read."], "step", "height", "step"),
    "title_sonic_loop_frames": byte_table(
        ["title_sonic_loop_frames (the title, from title_sonic_step 14 on): one byte a step, the frame n (sprite",
         "5300 + (n - 1) x 8); FF loops back to the first."], "step", "frame", "step"),
})
