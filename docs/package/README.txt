SegaSonic The Hedgehog - native port
https://github.com/cheviman/segasonic-port

Two games come in this package:

  SegaSonic          The console version: play this one. No coins; a menu (START GAME,
                     TIME ATTACK, OPTIONS, EXIT), up to three players, WIDE 16:9 stages.

  SegaSonic Arcade   The arcade game as the board runs it, coins and all:
                     5 puts a coin in, 1 starts, Escape quits.

THE FIRST RUN
The game needs your own MAME ROM set: sonic.zip (SegaSonic The Hedgehog, Japan, rev. C).
Put it beside the game (beside the .app on macOS) before starting it, or drop it on the
window that asks for it. The game takes out what it uses, once; it does not need the
ROM set again.

PLAYING SEGASONIC (player 1)
  Move                arrows or WASD, or the mouse (after a click in the window)
  Jump, spin          Z or Space, or the left mouse button
  Start, pause        Enter or P
  Back, pause         Escape or Backspace
Players 2 and 3, game controllers and every key: OPTIONS > CONTROLS.
F11 or Alt+Enter: full screen.

THE MOUSE, AND CLOSING THE GAME
In SegaSonic the mouse is a pointer until you click in the window: from then on it is
player 1's trackball and the pointer hides. Escape pauses the game and gives the pointer
back (the window's close button); Escape again goes on, and a click takes the mouse again.
Alt+F4 (Windows) or Cmd+Q (macOS) closes the game at any time. On the title, EXIT.
In SegaSonic Arcade the mouse is always the trackball, and Escape quits.

MACOS
The apps are not signed by a developer. The first time, right click the app > Open (on
macOS 15 and later: try to open it, then System Settings > Privacy & Security > Open
Anyway). If macOS says the app is damaged, run this in Terminal, in this folder:
  xattr -dr com.apple.quarantine *.app

WHERE THE GAME KEEPS ITS FILES
Windows: beside the game. Linux: ~/.local/share/SegaSonic.
macOS: ~/Library/Application Support/SegaSonic.

No game data is included in this package. SegaSonic The Hedgehog is a trademark of SEGA;
this project is not affiliated with or endorsed by SEGA. Licenses: LICENSE.txt, licenses/.
