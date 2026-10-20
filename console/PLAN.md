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
  VIDEO (a page: WINDOW, FULL SCREEN, PIXELS: sharp, smooth as sharp bilinear, or plain bilinear), CONTROLS, VOLUME
  (segasonic.ini), SOUND TEST (the game's ten songs; the one played becomes the title's music,
  title_music in segasonic.ini, stage 3's at first), STAGE SELECT (the island map, then player select),
  BACK. Direct3D 11 on Windows.
- CONTINUES (segasonic.ini, 3 at first): a game starts with the credits of its start and that many
  continues, shared (a player who joins takes them too), free play off until it ends; FREE is the game's
  own free play. With too few credits the start prompt shows nothing, not "Insert coin": the credit
  count beside it says 0.
- STAGE SELECT's level starts after player select, in the how to play's place (player select runs with
  stage 7, as in a game, so the chosen stage's objects do not run behind it).
- The Japanese captions in English: the game's own English boxes, which its program never shows
  (place_message_box). The how to play: CONTROLS in its box and the line in yellow, their outlines dark
  as the Japanese ones' (a pen of their palettes). Eggman's lines without their red box: white letters
  with a dark outline, made at start on the port's copy of the sprite graphics (console_prepare_sprites).
  "haaa!!" (kind 0x12), which has no English box, is "Huh!" in its frame's place: h, u and ! cut from the
  English lines, an H of their strokes.
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
- WIDE (VIDEO, segasonic.ini, off at first): a 16:9 window (the video's 416 pixels less 9 at each side).
  The stages and the how to play drawn wider (s32_video_set_wide: 48 more pixels of the tilemaps, the
  bitmap and the sprites at each side, the text layer and so the HUD where they were); their tile sets
  from the blocks of the wide left edge on (set_nbg0_scroll_from_camera); the how to play's panel strip
  copied at the sides; the SEGA logo, the title, the ranking and the attract demos too. The other screens
  with 320-pixel pictures (player select, the scenes) between bars. The window takes the largest whole
  scale under WINDOW at which it fits the screen, so a 16:9 one stays 16:9.
- The icon: Sonic's, Mighty's and Ray's heads (the join chooser's) from the player's ROM: the window's at
  run time (console_window_icon), the .exe's at build time (tools/make_icon.py, in the build folder).

Next (in this order unless Sebas says otherwise):
1. A CRT filter as a third PIXELS choice (in VIDEO).
2. Time Attack: a clock on screen, records per stage.
3. Achievements: our own, defined on the game's memory as RetroAchievements does (the work RAM is the
   original's, byte for byte); RetroAchievements itself only runs in the emulators it approves.
4. One SegaSonic.exe with SDL2 inside; first run asks for the player's ROM set and extracts the assets;
   the port's README (crediting Kenney's Input Prompts); a release for Quiver Launcher. The released
   .exe carries no ROM, so no icon from it: a neutral one of ours (the window's still comes from the ROM).

For the port's README: docs/screenshots/console_menu.png (the menu over the title, 640 x 448 like the others).
