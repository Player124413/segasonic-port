"""
Name:        animations.py
Description: the animations of the characters and objects as text, and back, byte for byte
Version:     0.1.0
Created:     14-10-2026
Modified:    14-10-2026
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
A frame is the offset of its record in sprite_frame_records (0x130000): frame_XXXX, the name of its PNG in
assets/sprites. The steps hold no time: each one lasts animation_speed frames, which the code that starts
the animation sets (a frame shown twice in a row is shown twice as long).

The text, one item a line, in the order of the bytes ('#' starts a comment):
  animation KIND [header=0xXXXX]   then its steps, one a line: the step number, then
     N frame_XXXX [flip_x] [flip_y] [flags=0xXXXX]   (flagged)
     N frame_XXXX dx=D dy=D                          (moving)
     N frame_XXXX                                    (frames)
    the header word is the number of steps - 1 unless header= gives it
  frame frame_XXXX                 a lone frame word
  step frame_XXXX [flip_x]...      a lone (flags, frame) step
  offset +XXXX                     a word: bytes from the start of the piece to what it points at
  entry +XXXX [flip_x]...          a (flags, offset) pair
  pointer 0xXXXXXX                 a 32-bit program address
  words XXXX ... / bytes XX ...    what no reader explains, kept as it is
"""
import re
import struct
from pathlib import Path

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


def _frame_token(value):
    return f"frame_{value:04X}" if value % 8 == 0 else f"0x{value:04X}"


def _flags_tokens(flags):
    tokens = [name for name, bit in FLIP_FLAGS if flags & bit]
    rest = flags & ~0xC0
    if rest:
        tokens.append(f"flags=0x{rest:04X}")
    return tokens


def _step_text(kind, data, at):
    if kind == "frames":
        return _frame_token(_u16(data, at))
    if kind == "flagged":
        return " ".join([f"{_frame_token(_u16(data, at + 2)):<10}"] + _flags_tokens(_u16(data, at))).rstrip()
    dx, dy = struct.unpack_from("<bb", data, at + 2)
    return f"{_frame_token(_u16(data, at)):<10} dx={dx:<4} dy={dy}"


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
    return at + 2, ["frame " + _frame_token(_u16(data, at))]


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


def frames_animation_to_text(data):
    piece = _Piece(data, "An animation of frames (animate, animate_step): each step is a sprite frame.\n" + TIMING)
    piece.add(0, lambda data, at: _read_animation(data, at, "frames"))
    return piece.render()


def frames_animation_list_to_text(data):
    piece = _Piece(data, "Animations of frames, one after the other (each one reached by its own pointer).\n"
                         + TIMING)
    at = 0
    while at < len(data):
        end = piece.add(at, lambda data, at: _read_animation(data, at, "frames"))
        if end is None:
            break
        at = end
    return piece.render()


def moving_animation_to_text(data):
    piece = _Piece(data, "An animation of (frame, dx, dy) steps: the edge falls move the projected position\n"
                         "by dx/dy each step (animate_and_move); the climb over the top places the player at\n"
                         "the anchor + dx/dy (animate_around_anchor). The x/y flips negate them.\n" + TIMING)
    piece.add(0, lambda data, at: _read_animation(data, at, "moving"))
    return piece.render()


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
    "anim_frames_list": (SUFFIX, frames_animation_list_to_text, animations_from_text),
    "anim_moving": (SUFFIX, moving_animation_to_text, animations_from_text),
    "anim_edge_hang": (SUFFIX, edge_hang_to_text, animations_from_text),
    "anim_knocked_out": (SUFFIX, knocked_out_to_text, animations_from_text),
    "anim_pointers": (SUFFIX, animation_pointers_to_text, animations_from_text),
}
