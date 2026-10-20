/**
 *    Name:        console.c
 *    Description: the console build's changes to the game: no licence screen, the attract mode's own order and
 *                 sound, no coins, PRESS START on the title and a menu over it
 *    Version:     0.1.0
 *    Created:     19-10-2026
 *    Modified:    19-10-2026
 *    Author:      Sebastian Toyos
 *
 *    The game's C (src/) stays as the decompilation wrote it. A function listed in console/replaced.txt is
 *    original_NAME in this build, and the function here takes its place (and may call it).
 *
 *    The menu is drawn over the title, which keeps drawing SegaSonic's logo under it. It reads the game's
 *    own controls: player 1's trackball up and down moves the cursor (the arrow keys roll it), the
 *    start button or the button picks.
 */
#include "game.h"
#include "console.h"

#include <string.h>

/* System modes (run_frame) */
#define MODE_OPTIONS   1u    /* never in the arcade's attract sequence: the console's OPTIONS */
#define MODE_RANKING   2u
#define MODE_TITLE     3u
#define MODE_DEMO_1    4u
#define MODE_DEMO_2    5u
#define MODE_GAME      6u

/* inputs_pressed: player 1's start and button */
#define PRESSED_START_1    0x10u
#define PRESSED_BUTTON_1   0x10000u

#define PRESS_START_ROW    25u      /* under the emblem's ribbon */
#define MENU_ROW           24u      /* the menu's first line, under the logo */
#define COPYRIGHT_ROW      27u      /* the title's (C) SEGA 1992 line */
#define MENU_TRAVEL        48       /* the trackball's y travel that moves the cursor one line, */
#define MENU_PAUSE         16       /* and the frames before it moves again */
#define SEGA_LOGO_LAST     0x3C     /* sega_logo_timer at the end of its last step (run_sega_logo, step 6) */
#define BLINK_FRAMES       32u
#define TITLE_HOLD         0x240    /* title_timer the title keeps under the menu */
#define MENU_TEXT          0x00u    /* text attributes (even ones: the odd ones are clear): white, */
#define MENU_CHOSEN        0x0Au    /* and yellow for the line the cursor is on */

/* The attract mode's screens: the title (the SEGA logo, then SegaSonic's), then the ranking, then a demo */
static const uint8_t attract_order[8] = {
	MODE_TITLE, MODE_RANKING, MODE_DEMO_1, MODE_TITLE, MODE_RANKING, MODE_DEMO_2, MODE_TITLE, MODE_RANKING
};

static const char *const menu_items[] = { "START GAME", "OPTIONS", "EXIT" };
#define MENU_ITEMS  3

static uint32_t title_frames;     /* frames the title has shown its logo (PRESS START blinks by them) */
static bool sega_logo_shown;      /* the SEGA logo has been shown (once, at power-on) */
static bool title_music_started;  /* this title's music has been asked for */
static bool menu_open;            /* the menu is over the title */
static bool options_open;         /* OPTIONS is on screen (system mode 1) */
static bool menu_after_title;     /* back from OPTIONS: the title opens its menu */
static bool menu_opening;         /* its first frame */
static int menu_choice;
static int menu_travel;
static int menu_wait;             /* frames before the cursor moves again */
static bool quit_requested;
static bool game_sees_start;      /* the menu started a game: its first frame sees player 1's start */
static uint32_t actions;          /* the port's actions newly pressed this frame, */
static uint32_t actions_held;     /* and held */

/* ---- text in the game's own font (the text layer: a cell is attribute << 8 | character) ---- */

static uint8_t text_attribute(void)
{
	return ROM8(credits_label_text_ADDRESS);    /* the credit line's colours */
}

static void write_text(uint32_t column, uint32_t row, const char *text, uint8_t attribute)
{
	for (; *text; text++, column++)
		mem_write16(TEXT_CELL(column, row), (uint16_t)((attribute << 8) | (uint8_t)*text));
}

static void erase_line(uint32_t row)
{
	uint32_t column;

	for (column = 0; column < 40u; column++)
		mem_write16(TEXT_CELL(column, row), (uint16_t)((text_attribute() << 8) | ' '));
}

static void write_centered(uint32_t row, const char *text, uint8_t attribute)
{
	uint32_t length = 0;

	while (text[length])
		length++;
	write_text(20u - (length + 1u) / 2u, row, text, attribute);
}

static void run_menu(void);
static void run_options(void);
static void open_menu(void);

/* ---- the SEGA logo's jingle: two chords through the sound driver's own MIDI queue ---- */

/* The sound driver plays its sequences by putting their MIDI messages in a queue in the sound RAM the main
   CPU shares (E100-E1FF there, 0x700100 here), for its MIDI parts to play with the game's instruments: two
   chords put there play like a song of the game's. MIDI channel B plays them: one of the effects' parts,
   at full volume whatever music_volume says (fixed_volume) and with no transpose, with the program the
   port's settings name; its own program comes back after them. */
#define JINGLE_CHANNEL     0x0Bu
#define LAYER_CHANNEL      0x0Cu    /* the layer under it: an octave down, another instrument */
#define MIDI_QUEUE         (SOUND_RAM + 0x100u)
#define MIDI_QUEUE_WRITE   (SOUND_RAM + 0x758u)
#define MIDI_QUEUE_COUNT   (SOUND_RAM + 0x759u)
#define JINGLE_STEP        4u       /* sega_logo_step when the logo has come together */
#define JINGLE_FIRST_FRAMES 36      /* "SE": 0.6 seconds, */
#define JINGLE_END_FRAMES  108      /* "GA": 1.2 more */
#define CHORD_NOTES        3

/* "SE-GA": E flat major, then C major (our own two chords) */
static const uint8_t chord_se[CHORD_NOTES] = { 63u, 67u, 75u };
static const uint8_t chord_ga[CHORD_NOTES] = { 60u, 64u, 72u };

static int jingle_frame = -1;     /* frames since the jingle began, -1: none */
static int jingle_program = 13;   /* the instrument (the port's settings: sega_jingle_program) */
static int jingle_layer = 2;      /* the layer's (sega_jingle_layer; -1: no layer) */

static void midi_message(uint8_t status, uint8_t first, uint8_t second, int bytes)
{
	uint8_t write = mem_read8(MIDI_QUEUE_WRITE);
	const uint8_t message[3] = { status, first, second };
	int i;

	for (i = 0; i < bytes; i++)
	{
		mem_write8(MIDI_QUEUE + write, message[i]);
		write++;
	}
	mem_write8(MIDI_QUEUE_WRITE, write);
	mem_write8(MIDI_QUEUE_COUNT, (uint8_t)(mem_read8(MIDI_QUEUE_COUNT) + 1u));
}

/* One voice of the jingle on 'channel', 'shift' semitones away: at this frame of it, its chords on or off */
static void jingle_voice(uint8_t channel, int program, int shift)
{
	int note;

	if (program < 0)
		return;
	if (jingle_frame == 0)
	{
		midi_message(0xB0u | channel, 7u, 0x7Fu, 3);    /* CC 7, the volume: all of it */
		midi_message(0xC0u | channel, (uint8_t)program, 0u, 2);
		for (note = 0; note < CHORD_NOTES; note++)
			midi_message(0x90u | channel, (uint8_t)(chord_se[note] + shift), 0x7Fu, 3);
	}
	else if (jingle_frame == JINGLE_FIRST_FRAMES)
	{
		for (note = 0; note < CHORD_NOTES; note++)
		{
			midi_message(0x80u | channel, (uint8_t)(chord_se[note] + shift), 0u, 3);
			midi_message(0x90u | channel, (uint8_t)(chord_ga[note] + shift), 0x7Fu, 3);
		}
	}
	else if (jingle_frame == JINGLE_END_FRAMES)
	{
		for (note = 0; note < CHORD_NOTES; note++)
			midi_message(0x80u | channel, (uint8_t)(chord_ga[note] + shift), 0u, 3);
		midi_message(0xC0u | channel, 0u, 0u, 2);    /* the part's own program again */
	}
}

static void play_jingle(void)
{
	if (jingle_frame < 0)
		return;
	jingle_voice(JINGLE_CHANNEL, jingle_program, 0);
	jingle_voice(LAYER_CHANNEL, jingle_layer, -12);
	if (jingle_frame == JINGLE_END_FRAMES)
	{
		jingle_frame = -2;    /* (done) */
		return;
	}
	jingle_frame++;
}

void console_jingle_program(int program, int layer)
{
	jingle_program = program;
	jingle_layer = layer;
}

/* ---- the attract mode ---- */

/* The title shows SegaSonic's emblem: its timer runs (title_emblem_animate), the SEGA logo is over */
static bool title_logo_up(void)
{
	return title_scene_built != 0u && title_timer > 0;
}

/* System mode 0 at power-on (and after the test mode): instead of the licence screen, the title; and no
   coins: the game's own free play (COIN/CREDIT SETTING 26), set over what apply_settings just read */
void attract_warning(uint32_t *p_pixels, uint32_t *p_text)
{
	(void)p_pixels;
	(void)p_text;
	free_play = 1u;
	attract_step = 0u;
	system_mode = attract_order[0];
	system_mode_step = 0u;
}

/* The next attract screen, in the console's order */
void advance_attract_screen(void)
{
	uint32_t next_step = 0;

	if (menu_open)
		return;    /* (the title stays under the menu) */

	next_step = (uint8_t)(attract_step + 1u) & 7u;
	system_mode_step = 0u;
	system_mode = attract_order[next_step];
	attract_step = next_step;
}

/* A sound: in the attract mode too, every one and the music, as at home (the arcade lets some effects
   through there, and only with ADVERTISE SOUND on) */
void play_sound(uint8_t sound_id, uint32_t *p_write_index, uint32_t *p_write_index_address)
{
	uint32_t write_index = *p_write_index;
	uint32_t write_index_address = *p_write_index_address;

	queue_sound(sound_id, &write_index, &write_index_address);
	*p_write_index = write_index;
	*p_write_index_address = write_index_address;
}

/* System mode 3, the title: with music from its first frame, and PRESS START blinking once the logo is up */
void attract_title(uint32_t *p_color, uint32_t *p_row_pairs_left, uint32_t *p_blocks_per_row, uint32_t *p_text)
{
	uint32_t write_index = 0, write_index_address = 0;

	if (system_mode_step == 0u)
	{
		title_frames = 0;
		title_music_started = false;
		title_timer = 0;    /* (left from the last title until this one builds its emblem) */
		jingle_frame = -1;
	}
	if (system_mode == MODE_TITLE && !sega_logo_shown && sega_logo_step >= JINGLE_STEP && jingle_frame == -1)
		jingle_frame = 0;    /* the SEGA logo is up: its jingle */
	play_jingle();
	if (menu_open && title_timer > TITLE_HOLD)
		title_timer = TITLE_HOLD;    /* (at 0x258 the title frees its logo and ends: not under the menu) */
	original_attract_title(p_color, p_row_pairs_left, p_blocks_per_row, p_text);
	if (system_mode == MODE_TITLE && sega_logo_shown && sega_logo_step == 1u)
	{
		/* the SEGA logo only once, at power-on: then on to its last step, which clears what it drew and
		   lets the title build SegaSonic's emblem */
		sega_logo_step = 6u;
		sega_logo_timer = SEGA_LOGO_LAST;
	}
	if (system_mode == MODE_TITLE && !title_logo_up())
		erase_line(COPYRIGHT_ROW);    /* the (C) SEGA 1992 line: on the title and its menu, not the SEGA logo */
	if (system_mode != MODE_TITLE || !title_logo_up())
		return;
	if (!title_music_started)
	{
		title_music_started = true;    /* (once: asked again, the song starts over every frame) */
		/* the emblem: its music (the arcade's title has none; segasonic.ini's title_music, stage 3's at first) */
		queue_sound((uint8_t)console_window_settings.title_music, &write_index, &write_index_address);
	}
	sega_logo_shown = true;
	if (menu_after_title && !menu_open)
	{
		menu_after_title = false;    /* (back from OPTIONS: the menu again, on OPTIONS) */
		open_menu();
		menu_choice = 1;
	}
	if (menu_open)
	{
		run_menu();
		return;
	}
	title_frames++;
	if ((title_frames / BLINK_FRAMES) & 1u)
		erase_line(PRESS_START_ROW);
	else
		write_centered(PRESS_START_ROW, "PRESS START", text_attribute());
}

/* The credit line: with free play (always, in the console) nothing; the game's otherwise */
void draw_credit_count_now(uint32_t credit_block, uint32_t *p_credits, uint32_t *p_coins_per_credit, uint32_t *p_text,
	uint32_t *p_text_position)
{
	if (free_play != 0u)
	{
		credit_text_position = *p_text_position;
		return;
	}
	original_draw_credit_count_now(credit_block, p_credits, p_coins_per_credit, p_text, p_text_position);
}

/* ---- the menu ---- */

static void draw_menu(void)
{
	int item;

	for (item = 0; item < MENU_ITEMS; item++)
	{
		erase_line(MENU_ROW + (uint32_t)item);
		write_centered(MENU_ROW + (uint32_t)item, menu_items[item], item == menu_choice ? MENU_CHOSEN : MENU_TEXT);
	}
}

static void erase_menu(void)
{
	int item;

	for (item = 0; item < MENU_ITEMS; item++)
		erase_line(MENU_ROW + (uint32_t)item);
}

static void open_menu(void)
{
	erase_line(PRESS_START_ROW);
	menu_open = true;
	menu_opening = true;
	menu_choice = 0;
	menu_travel = 0;
}

/* A start button outside a game: on the title, the menu (the frame goes on with the title and the menu,
   which reads the button itself); on the ranking or a demo, back to the title. The game starts from the
   menu. */
void run_frame_start_game(uint32_t credits, uint32_t blink_bit, uint32_t credits_needed, uint32_t count,
	uint32_t message_text, uint32_t text_position, uint32_t *p_scale, uint32_t *p_cursor, uint32_t *p_height)
{
	uint32_t sprite_entry = 0, string_cursor = 0;

	(void)credits;
	(void)blink_bit;
	(void)credits_needed;
	(void)text_position;
	if (system_mode == MODE_TITLE)
	{
		if (title_logo_up() && !menu_open)
			open_menu();
		attract_title(p_scale, &count, p_height, &message_text);    /* the title, and the menu over it */
	}
	else if (system_mode == MODE_OPTIONS)
		system_mode_idle();    /* (OPTIONS reads the button itself) */
	else
	{
		attract_step = 0u;
		system_mode = MODE_TITLE;
		system_mode_step = 0u;
	}
	/* the rest of the frame, as the game's own does after starting a game */
	run_objects();
	build_sprite_list(p_scale, &count, p_height, &message_text, &sprite_entry);
	draw_queued_strings((uint16_t)count, (uint16_t)message_text, sprite_entry, p_scale, &string_cursor, p_height);
	*p_cursor = string_cursor;
}

/* A move of a cursor: -1, +1 or none, from player 1's trackball along one axis ('trackball', travelled up to
   MENU_TRAVEL, then MENU_PAUSE frames before the next) or the actions 'minus' and 'plus' */
static int cursor_step(int *travel, int *wait, int16_t trackball, console_action minus, console_action plus)
{
	const uint32_t both = ACTION_BIT(minus) | ACTION_BIT(plus);

	if (*wait > 0)
	{
		(*wait)--;
		*travel = 0;
	}
	else if (actions_held & both)
		/* held, it repeats; the trackball does not count then (an arrow key held rolls it too) */
		*travel = (actions_held & ACTION_BIT(plus)) ? MENU_TRAVEL : -MENU_TRAVEL;
	else
		*travel += trackball;
	if (actions & ACTION_BIT(minus))
		*travel = -MENU_TRAVEL;
	if (actions & ACTION_BIT(plus))
		*travel = MENU_TRAVEL;
	if (*travel > -MENU_TRAVEL && *travel < MENU_TRAVEL)
		return 0;
	*wait = MENU_PAUSE;
	if (*travel > 0)
	{
		*travel = 0;
		return 1;
	}
	*travel = 0;
	return -1;
}

/* A pick: player 1's start or button, or the actions accept and start */
static bool picked(void)
{
	return (inputs_pressed & (PRESSED_START_1 | PRESSED_BUTTON_1)) != 0u
		|| (actions & (ACTION_BIT(ACTION_ACCEPT) | ACTION_BIT(ACTION_START))) != 0u;
}

static void open_options(void);

/* The menu over the title: the cursor follows player 1's trackball up and down (or the actions up and
   down), the start button, the button or the actions accept and start pick */
static void run_menu(void)
{
	bool opening = menu_opening;
	int step = 0;

	menu_opening = false;
	step = cursor_step(&menu_travel, &menu_wait, (int16_t)trackball_1_y, ACTION_UP, ACTION_DOWN);
	menu_choice = (menu_choice + step + MENU_ITEMS) % MENU_ITEMS;
	draw_menu();
	if (opening || !picked())
		return;    /* (the press that opened the menu picks nothing) */
	switch (menu_choice)
	{
	case 0:    /* START GAME: as the game's own start does */
		menu_open = false;
		erase_menu();
		system_mode = MODE_GAME;
		game_mode = 0u;
		system_mode_step = 0u;
		game_in_progress = 0xFFu;
		game_sees_start = true;
		break;
	case 1:    /* OPTIONS */
		open_options();
		break;
	default:   /* EXIT */
		quit_requested = true;
		break;
	}
}

/* ---- OPTIONS: a page of its own (system mode 1) over the ranking's drifting Sonic faces ---- */

enum
{
	OPTION_DIFFICULTY,
	OPTION_VITALITY,
	OPTION_WINDOW,
	OPTION_FULL_SCREEN,
	OPTION_PIXELS,
	OPTION_VOLUME,
	OPTION_SOUND_TEST,
	OPTION_STAGE_SELECT,
	OPTION_BACK,
	OPTION_COUNT
};

static const char *const option_names[OPTION_COUNT] = {
	"DIFFICULTY", "VITALITY", "WINDOW", "FULL SCREEN", "PIXELS", "VOLUME", "SOUND TEST", "STAGE SELECT", "BACK"
};

#define OPTIONS_TITLE_ROW   4u
#define OPTIONS_FIRST_ROW   8u      /* then a line every two rows */
#define OPTION_NAME_COLUMN  8u
#define OPTION_VALUE_COLUMN 24u
#define OPTIONS_LAST_ROW    (OPTIONS_FIRST_ROW + 2u * OPTION_COUNT - 2u)
#define WINDOW_SCALE_MOST   6
#define VOLUME_MOST         10

console_window console_window_settings = { 3, false, false, 5, 0x01, false };

static int option_choice;
static int option_travel, option_wait, value_travel, value_wait;
static bool game_settings_changed;

/* SOUND TEST: the game's songs, by the sound id that plays each (sound_commands, group 81) */
static const struct
{
	uint8_t sound;
	const char *name;
} songs[] = {
	{ 0x03u, "MAP" }, { 0x01u, "STAGE 3" }, { 0x02u, "STAGES 4 8" }, { 0x04u, "STAGES 5 6" },
	{ 0x81u, "STAGES 9 10" }, { 0x7Du, "STAGE A" }, { 0x00u, "STAGES B-F" }, { 0x7Cu, "ESCAPE" },
	{ 0x7Bu, "STAGE CLEAR" }, { 0x82u, "ENDING" },
};
#define SONGS ((int)(sizeof(songs) / sizeof(songs[0])))

static int song_choice;
static bool stage_select_pending;    /* STAGE SELECT: the stage select (game mode 8), then player select */
static int stage_select_phase;       /* 0 before the stage select, 1 in it */

/* The value of an option as its line shows it: "< 2 >", "< ON  >"; nothing for the ones that are picked */
static void option_value(int option, char *text)
{
	int value = 0;

	switch (option)
	{
	case OPTION_DIFFICULTY:
		value = setting_difficulty + 1;
		break;
	case OPTION_VITALITY:
		value = setting_initial_vitality + 1;
		break;
	case OPTION_WINDOW:
		value = console_window_settings.window_scale;
		break;
	case OPTION_VOLUME:
		value = console_window_settings.volume;
		break;
	case OPTION_FULL_SCREEN:
		strcpy(text, console_window_settings.full_screen ? "< ON  >" : "< OFF >");
		return;
	case OPTION_PIXELS:
		strcpy(text, console_window_settings.smooth ? "< SMOOTH >" : "< SHARP  >");
		return;
	case OPTION_SOUND_TEST:
		strcpy(text, "< ");
		strcat(text, songs[song_choice].name);
		strcat(text, " >");
		return;
	default:
		text[0] = 0;
		return;
	}
	text[0] = '<';
	text[1] = ' ';
	text[2] = (char)(value >= 10 ? '1' : ' ');
	text[3] = (char)('0' + value % 10);
	text[4] = option == OPTION_WINDOW ? 'X' : ' ';
	text[5] = ' ';
	text[6] = '>';
	text[7] = 0;
}

static void draw_options(void)
{
	char value[24];
	int option;

	write_centered(OPTIONS_TITLE_ROW, "OPTIONS", MENU_CHOSEN);
	for (option = 0; option < OPTION_COUNT; option++)
	{
		const uint32_t row = OPTIONS_FIRST_ROW + 2u * (uint32_t)option;
		const uint8_t attribute = option == option_choice ? MENU_CHOSEN : MENU_TEXT;

		erase_line(row);
		write_text(OPTION_NAME_COLUMN, row, option_names[option], attribute);
		option_value(option, value);
		write_text(OPTION_VALUE_COLUMN, row, value, attribute);
	}
}

static void erase_options(void)
{
	uint32_t row;

	for (row = OPTIONS_TITLE_ROW; row < OPTIONS_FIRST_ROW + 2u * OPTION_COUNT; row++)
		erase_line(row);
}

static int clamp(int value, int least, int most)
{
	return value < least ? least : value > most ? most : value;
}

static void change_option(int option, int step)
{
	switch (option)
	{
	case OPTION_DIFFICULTY:
		setting_difficulty = (uint8_t)clamp(setting_difficulty + step, 0, 3);
		game_settings_changed = true;
		break;
	case OPTION_VITALITY:
		setting_initial_vitality = (uint8_t)clamp(setting_initial_vitality + step, 0, 3);
		game_settings_changed = true;
		break;
	case OPTION_WINDOW:
		console_window_settings.window_scale = clamp(console_window_settings.window_scale + step, 1,
			WINDOW_SCALE_MOST);
		console_window_settings.changed = true;
		break;
	case OPTION_FULL_SCREEN:
		console_window_settings.full_screen = !console_window_settings.full_screen;
		console_window_settings.changed = true;
		break;
	case OPTION_PIXELS:
		console_window_settings.smooth = !console_window_settings.smooth;
		console_window_settings.changed = true;
		break;
	case OPTION_VOLUME:
		console_window_settings.volume = clamp(console_window_settings.volume + step, 0, VOLUME_MOST);
		console_window_settings.changed = true;
		break;
	case OPTION_SOUND_TEST:
		song_choice = (song_choice + step + SONGS) % SONGS;
		break;
	default:
		break;
	}
}

static uint32_t options_backdrop;    /* the object that drifts the faces (ranking_display_hold) */

static void open_options(void)
{
	int song;

	for (song = 0; song < SONGS; song++)    /* (SOUND TEST shows the title's song) */
		if (songs[song].sound == console_window_settings.title_music)
			song_choice = song;
	menu_open = false;
	options_open = true;
	option_choice = 0;
	option_travel = value_travel = 0;
	option_wait = value_wait = MENU_PAUSE;
	game_settings_changed = false;
	system_mode = MODE_OPTIONS;
	system_mode_step = 0u;
}

/* The page's first frame: as the ranking sets itself up (attract_ranking, start_ranking_display), but with
   no table: the NBG0 picture of Sonic faces, and the object that drifts it */
static void build_options_page(void)
{
	uint32_t ignored = 0, display = 0, rows = 0, source = 0;
	uint8_t mode = 0u;

	set_screen_mode(mode);
	clear_text_layer(&ignored, &ignored, &ignored);
	display = clear_page_0_and_scroll();
	reset_objects(&ignored, &ignored);
	current_screen = 2u;
	display = allocate_object_or_start_list(display);
	OBJECT(display)->parent = object_cursor;
	OBJECT(display)->child = 0xFFFFu;
	init_object(display);
	show_nbg0_picture(&ignored, &rows, &source, &ignored);
	OBJECT(display)->handler = ranking_display_hold_ADDRESS;
	options_backdrop = display;
}

/* Back to the title and its menu: the game's own settings (difficulty, vitality) worked out again and kept
   in the EEPROM as the test mode keeps them */
static void close_options(void)
{
	uint32_t command = 0, words_left = 0;

	erase_options();
	options_open = false;
	menu_after_title = true;
	attract_step = 0u;
	system_mode = MODE_TITLE;
	system_mode_step = 0u;
	if (!game_settings_changed)
		return;
	apply_settings();
	free_play = 1u;
	eeprom_save_and_lock(&command, &words_left);
}

/* System mode 1, OPTIONS: its page on the first frame, then its lines; the faces never stop drifting (the
   backdrop's hold timer stays at 0) */
void system_mode_idle(void)
{
	if (system_mode_step == 0u)
	{
		system_mode_step = 1u;
		build_options_page();
	}
	OBJECT(options_backdrop)->ranking_display.hold_timer = 0;
	run_options();
}

/* The options: up and down choose a line, left and right change its value; SOUND TEST (it plays the song,
   which the title plays from then on),
   STAGE SELECT and BACK are picked; the action back is BACK */
static void run_options(void)
{
	int step = 0;

	step = cursor_step(&option_travel, &option_wait, (int16_t)trackball_1_y, ACTION_UP, ACTION_DOWN);
	option_choice = (option_choice + step + OPTION_COUNT) % OPTION_COUNT;
	step = cursor_step(&value_travel, &value_wait, (int16_t)trackball_1_x, ACTION_LEFT, ACTION_RIGHT);
	if (step != 0)
		change_option(option_choice, step);
	draw_options();
	if (!picked())
		return;
	if (option_choice == OPTION_SOUND_TEST)
	{
		uint32_t write_index = 0, write_index_address = 0;

		queue_sound(songs[song_choice].sound, &write_index, &write_index_address);    /* (it plays the song, */
		console_window_settings.title_music = songs[song_choice].sound;    /* the title's from now on) */
		console_window_settings.changed = true;
	}
	else if (option_choice == OPTION_BACK)
		close_options();
	else if (option_choice == OPTION_STAGE_SELECT)
	{
		close_options();
		menu_after_title = false;
		stage_select_pending = true;
		stage_select_phase = 0;
		system_mode = MODE_GAME;
		game_mode = 0u;
		system_mode_step = 0u;
		game_in_progress = 0xFFu;
		game_sees_start = true;
	}
}

/* ---- the stage select (game mode 8) ---- */

static int stage_travel, stage_wait;

/* A step of the trackball (+1, -1 or 0): in the stage select, as the menus move (a stage a touch of left or
   right, held it repeats slowly); the game's own everywhere else */
void read_trackball_step(uint32_t *p_player_number, uint32_t *p_trackball)
{
	int step = 0;

	if (game_mode != 8u)
	{
		original_read_trackball_step(p_player_number, p_trackball);
		return;
	}
	*p_trackball = trackball_1_x_ADDRESS + *p_player_number * 4u;
	step = cursor_step(&stage_travel, &stage_wait, (int16_t)trackball_1_x, ACTION_LEFT, ACTION_RIGHT);
	*p_player_number = (*p_player_number & 0xFFFF0000u) | (step < 0 ? 0xFFFFu : (uint32_t)step);
}

/* The game's modes: the first frame of a game the menu started sees player 1's start pressed, as the
   arcade's start frame does (run_frame_start_game runs the game in the frame of the press): that is how the
   game takes player 1 in and lets him choose a character */
void run_game_mode(uint32_t pressed, uint32_t position, uint32_t speed, uint16_t next_scroll_position, uint32_t object,
	uint32_t *p_speed_value, uint32_t *p_count, uint32_t *p_cycle, uint32_t *p_list_cursor)
{
	if (game_sees_start)
	{
		game_sees_start = false;
		inputs_pressed |= PRESSED_START_1;
	}
	/* STAGE SELECT: the stage first (the original's stage select, game mode 8, where player select would
	   begin), then player select (where the stage select would start the stage), then the stage */
	if (stage_select_pending && stage_select_phase == 0 && game_mode == 0x1Bu)
	{
		game_mode = 8u;
		stage_select_phase = 1;
	}
	else if (stage_select_pending && stage_select_phase == 1 && game_mode == 2u)
	{
		game_mode = 0x1Bu;
		stage_select_pending = false;
	}
	original_run_game_mode(pressed, position, speed, next_scroll_position, object, p_speed_value, p_count, p_cycle,
		p_list_cursor);
}

/* ---- player select ---- */

#define CHOOSERS           3
#define TURN_NOW           0xF8u    /* a count's low byte one frame (+ 8) short of a turn */

static bool chooser_rolling[CHOOSERS];

/* A player's character chooser: the game turns once every 32 frames the trackball keeps moving (+ 8 a
   frame into the count's low byte, a turn at each carry); the first frame of a move turns at once, so a
   touch of left or right turns one character, and holding it goes on as the game does */
void character_chooser_wait(void)
{
	uint32_t kind = OBJECT(current_object)->kind;
	int16_t trackball_x = 0;

	if (kind < CHOOSERS)
	{
		trackball_x = (int16_t)RAM16(GAME_VARIABLES + chooser_trackball_offsets[kind]);
		if (trackball_x != 0 && !chooser_rolling[kind])
		{
			if (trackball_x < 0)
				OBJECT(current_object)->character_chooser.left_count =
					(uint16_t)((OBJECT(current_object)->character_chooser.left_count & 0xFF00u) | TURN_NOW);
			else
				OBJECT(current_object)->character_chooser.right_count =
					(uint16_t)((OBJECT(current_object)->character_chooser.right_count & 0xFF00u) | TURN_NOW);
		}
		chooser_rolling[kind] = trackball_x != 0;
	}
	original_character_chooser_wait();
}

/* ---- for the port ---- */

#define PAUSE_ROW          2u

static uint16_t pause_saved[40];

void console_actions(uint32_t pressed, uint32_t held)
{
	actions = pressed;
	actions_held = held;
}

void console_back(void)
{
	if (options_open)
	{
		close_options();
		return;
	}
	if (!menu_open)
		return;
	menu_open = false;
	erase_menu();
	title_frames = 0;    /* (PRESS START again, from its shown half) */
}

bool console_in_game(void)
{
	if (system_mode != MODE_GAME || (int8_t)game_in_progress >= 0)
		return false;
	/* a stage being played (game_mode_play_stage: modes 3, 5 and 6; the screen wipe that opens one, 0x11);
	   not the intro, player select, the how to play, the map or the scenes, where start is start */
	return game_mode == 3u || game_mode == 5u || game_mode == 6u || game_mode == 0x11u;
}

void console_pause_text(bool shown)
{
	uint32_t column;

	if (shown)
	{
		for (column = 0; column < 40u; column++)
			pause_saved[column] = mem_read16(TEXT_CELL(column, PAUSE_ROW));
		write_centered(PAUSE_ROW, "PAUSE", MENU_CHOSEN);
		return;
	}
	for (column = 0; column < 40u; column++)
		mem_write16(TEXT_CELL(column, PAUSE_ROW), pause_saved[column]);
}

bool console_quit_requested(void)
{
	return quit_requested;
}

