# The console build: what it changes, and what is next

Done:
- No licence screen: power-on goes straight to the title (the SEGA logo, then SegaSonic's).
- The attract mode in the console's order: title, ranking, demo, title, ranking, the other demo.
- No coins: the game's own free play, without its "Free play" line.
- The attract mode with every sound and the music (the arcade's lets some effects through, and only with
  ADVERTISE SOUND on); the title plays the map's music (sound 3; the arcade's title has none).

- The title menu, PRESS START, player select by a touch, pause in grey, actions instead of keys.
- The SEGA logo's jingle: two chords of our own (E flat major, C major) through the sound driver's own MIDI queue on the effects'
  channels B and C (an octave under), instruments from segasonic.ini; it sounds like a handheld's, so a
  recorded voice of our own (sega.wav, played by the port) may take its place later.
- (C) SEGA 1992 on the title and its menu only.

- OPTIONS, a page of its own over the ranking's drifting Sonic faces: DIFFICULTY, VITALITY (the EEPROM,
  as the test mode keeps them), WINDOW, FULL SCREEN, PIXELS (sharp, or smooth as sharp bilinear), VOLUME
  (segasonic.ini), SOUND TEST (the game's ten songs), STAGE SELECT (the stage first, then player select;
  a stage a touch), BACK. Direct3D 11 on Windows.

Next (in this order unless Sebas says otherwise):
1. The title with a blinking PRESS START (the game's menu font); start opens a small menu over SegaSonic's
   logo with the three characters on a blue layer (Sonic 1's title): START GAME, OPTIONS, EXIT.
2. Pause: start in play freezes the game, everything turns grey, "PAUSE" on top in the game's font.
3. OPTIONS: GAME (difficulty, initial vitality, attract sound), SCREEN (window size or resolution, full
   screen or window, filters: smooth pixels, maybe a CRT one), SOUND (volume), CONTROLS, EXTRAS (the
   original's hidden Stage Select, game mode 8).
4. Stage select on the island map (scenes/map.c), with the stages already cleared marked: a progress file
   (which stages were cleared) kept beside the EEPROM; until then OPTIONS' STAGE SELECT is the original's
   debug screen, a stage a touch.
5. Credits as continues: a game starts with the 3 lives a coin gives and then 3 continues (credits), instead
   of endless free play.
5. The Japanese texts in English, drawn to fit (the screens that have them).
6. Time Attack: a clock on screen, records per stage.
7. Achievements: our own, defined on the game's memory as RetroAchievements does (the work RAM is the
   original's, byte for byte); RetroAchievements itself only runs in the emulators it approves.
8. One SegaSonic.exe with SDL2 inside; first run asks for the player's ROM set and extracts the assets;
   a release for Quiver Launcher.

For the port's README: docs/screenshots/console_menu.png (the menu over the title, 640 x 448 like the others).
