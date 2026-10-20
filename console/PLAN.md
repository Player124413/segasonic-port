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
  (segasonic.ini), SOUND TEST (the game's ten songs; the one played becomes the title's music,
  title_music in segasonic.ini, stage 3's at first), STAGE SELECT (the stage first, then player select;
  a stage a touch), BACK. Direct3D 11 on Windows.

Next (in this order unless Sebas says otherwise):
1. CONTROLS in OPTIONS: the actions' keys and buttons changed from the menu (ports/sdl2/actions.c's table,
   kept in segasonic.ini), shown with Kenney's Input Prompts (CC0; ask before downloading it).
2. Stage select on the island map (scenes/map.c), with the stages already cleared marked: a progress file
   (which stages were cleared) kept beside the EEPROM; until then STAGE SELECT is the original's debug
   screen, a stage a touch.
3. Credits as continues: a game starts with the 3 lives a coin gives and then 3 continues (credits),
   instead of endless free play.
4. The Japanese texts in English, drawn to fit, in the game's own font (the how to play screen...).
5. Wide (16:9): the HUD where it is, only the view wider at the sides (the board's 416-pixel mode).
6. A CRT filter as a third PIXELS choice.
7. Time Attack: a clock on screen, records per stage.
8. Achievements: our own, defined on the game's memory as RetroAchievements does (the work RAM is the
   original's, byte for byte); RetroAchievements itself only runs in the emulators it approves.
9. The SEGA logo: a recorded voice of our own (sega.wav) in place of the chords, if Sebas records one.
10. One SegaSonic.exe with SDL2 inside; first run asks for the player's ROM set and extracts the assets;
    the port's README; a release for Quiver Launcher.

For the port's README: docs/screenshots/console_menu.png (the menu over the title, 640 x 448 like the others).
