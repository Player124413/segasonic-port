# The console build: what it changes, and what is next

Done:
- No licence screen: power-on goes straight to the title (the SEGA logo, then SegaSonic's).
- The attract mode in the console's order: title, ranking, demo, title, ranking, the other demo.
- No coins: the game's own free play, without its "Free play" line.
- The attract mode with every sound and the music (the arcade's lets some effects through, and only with
  ADVERTISE SOUND on); the title plays the map's music (sound 3; the arcade's title has none).

- The title menu, PRESS START, player select by a touch, pause in grey, actions instead of keys.
- The SEGA logo's jingle: two chords of our own (E flat major, C major) through the sound driver's own MIDI queue on the effects'
  channels B and C (an octave under), instruments from segasonic.ini; it stays (no recorded voice).
- (C) SEGA 1992 on the title and its menu only.

- OPTIONS, a page of its own over the ranking's drifting Sonic faces: DIFFICULTY (VERY EASY, EASY,
  NORMAL, HARD: the test mode's 1-4) and VITALITY (the EEPROM, as the test mode keeps them), CONTINUES,
  VIDEO (a page: WINDOW, FULL SCREEN, PIXELS, sharp or smooth as sharp bilinear), CONTROLS, VOLUME
  (segasonic.ini), SOUND TEST (the game's ten songs; the one played becomes the title's music,
  title_music in segasonic.ini, stage 3's at first), STAGE SELECT (the island map, then player select),
  BACK. Direct3D 11 on Windows.
- CONTINUES (segasonic.ini, 3 at first): a game starts with the credits of its start and that many
  continues, shared (a player who joins takes them too), free play off until it ends; FREE is the game's
  own free play. With too few credits the start prompt shows nothing, not "Insert coin": the credit
  count beside it says 0.
- STAGE SELECT on the island map (game mode 8): the map as the map screen fades it in, every level open
  (the opening, Eggman blinking for it, then the 7 markers, each one's number and name blinking); the
  levels cleared in this session's games in the map's own cleared colours, in memory only (no file: it is
  not a continue). A pick starts the level's first stage as the original's stage select does.
- CONTROLS in OPTIONS, for players 1-3 (PLAYER < 1 >): each action's first two keys and its game
  controller button, drawn with Kenney's Input Prompts Pixel (CC0, ports/sdl2/prompts); a line picked waits
  for the new key or button (Escape leaves it), DEFAULTS puts that player's back; kept in controls.ini
  beside the executable, a section a player. Each player rolls and jumps with their own keys and game
  controller (plugged in in their turn); the menus take any player's.
- A player's start prompt up (to join, or a continue's count): their start joins, it does not pause.
  The join chooser in a stage (a continue) turns at a touch, as player select's does.
- The 3-player cabinet: with the 2-player one the original draws player 3's start prompt in the middle
  of the stage (it reads past start_prompt_positions_2p).

Next (in this order unless Sebas says otherwise):
1. The Japanese texts in English, drawn to fit, in the game's own font (the how to play screen...).
2. Wide (16:9): the HUD where it is, only the view wider at the sides (the board's 416-pixel mode).
3. A CRT filter as a third PIXELS choice (in VIDEO).
4. Time Attack: a clock on screen, records per stage.
5. Achievements: our own, defined on the game's memory as RetroAchievements does (the work RAM is the
   original's, byte for byte); RetroAchievements itself only runs in the emulators it approves.
6. The icon: the window's from the player's own ROM (a sprite of the game's, taken from the extracted
   assets at run time); the .exe's own, ours, with no art of the game in it (the release carries none).
7. One SegaSonic.exe with SDL2 inside; first run asks for the player's ROM set and extracts the assets;
   the port's README (crediting Kenney's Input Prompts); a release for Quiver Launcher.

For the port's README: docs/screenshots/console_menu.png (the menu over the title, 640 x 448 like the others).
