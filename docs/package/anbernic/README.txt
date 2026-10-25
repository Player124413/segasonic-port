SegaSonic The Hedgehog - native port, for Anbernic's H700 handhelds
https://github.com/cheviman/segasonic-port

The console version of the game for the stock firmware of Anbernic's H700 handhelds (played
on the RG40XX H; the RG35XX Plus, RG40XX V and RG CubeXX should run it as it does).

INSTALLING
1. Make the game's assets on your computer: play SegaSonic, this project's game for Windows,
   Linux or macOS, once with your sonic.zip (MAME's ROM set, SegaSonic The Hedgehog, Japan,
   rev. C). It writes segasonic.assets: beside SegaSonic.exe on Windows, in
   ~/.local/share/SegaSonic on Linux, in ~/Library/Application Support/SegaSonic on macOS.
2. Put segasonic.assets in PORTS/segasonic/, beside SegaSonic.
3. Copy the folder PORTS into Roms/ on the handheld's card (Roms/PORTS/SegaSonic.sh and
   Roms/PORTS/segasonic/).
SegaSonic is then in the firmware's PORTS. segasonic.assets carries your ROM's assets: it
is yours alone, never share it.

PLAYING
  d-pad or left stick   move (the trackball)
  A                     jump, pick
  B                     back in the menus (nothing in a stage)
  START                 start, pause
  MENU                  leaves the game
  volume keys           the game's volume
OPTIONS > CONTROLS changes the buttons. The game keeps its settings, records and
segasonic.log in its folder, Roms/PORTS/segasonic/.

No game data is included in this package. SegaSonic The Hedgehog is a trademark of SEGA;
this project is not affiliated with or endorsed by SEGA, nor with Anbernic. Licenses:
PORTS/segasonic/LICENSE.txt, PORTS/segasonic/licenses/.
