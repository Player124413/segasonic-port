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

/* System modes (run_frame) */
#define MODE_RANKING   2u
#define MODE_TITLE     3u
#define MODE_DEMO_1    4u
#define MODE_DEMO_2    5u
#define MODE_GAME      6u

/* inputs_pressed: player 1's start and button */
#define PRESSED_START_1    0x10u
#define PRESSED_BUTTON_1   0x10000u

#define TITLE_MUSIC        0x00u    /* the sound the title starts (the arcade's title has none): stage 00's music */
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
static bool menu_open;            /* the menu is over the title */
static bool menu_opening;         /* its first frame */
static int menu_choice;
static int menu_travel;
static int menu_wait;             /* frames before the cursor moves again */
static bool quit_requested;
static bool game_sees_start;      /* the menu started a game: its first frame sees player 1's start */
static uint32_t actions;          /* the port's actions newly pressed this frame */

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

/* ---- the SEGA logo's jingle: two notes through the sound driver's own MIDI queue ---- */

/* The sound driver plays its sequences by putting their MIDI messages in a queue in the sound RAM the main
   CPU shares (E100-E1FF there, 0x700100 here), for its MIDI parts to play with the game's instruments: two
   notes put there play like a song of the game's. MIDI channel B plays them: one of the effects' parts,
   at full volume whatever music_volume says (fixed_volume) and with no transpose, with the program the
   port's settings name; its own program comes back after them. */
#define JINGLE_CHANNEL     0x0Bu
#define LAYER_CHANNEL      0x0Cu    /* the layer under it: an octave down, another instrument */
#define MIDI_QUEUE         (SOUND_RAM + 0x100u)
#define MIDI_QUEUE_WRITE   (SOUND_RAM + 0x758u)
#define MIDI_QUEUE_COUNT   (SOUND_RAM + 0x759u)
#define JINGLE_STEP        4u       /* sega_logo_step when the logo has come together */
#define JINGLE_NOTE_FRAMES 37       /* each note: 0.6 seconds */
#define JINGLE_FIRST       79u      /* G, then */
#define JINGLE_SECOND      76u      /* E: "SE-GA" */

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

/* One voice of the jingle on 'channel': at 'frame' of it, its note on or off ('shift' semitones away) */
static void jingle_voice(uint8_t channel, int program, int shift)
{
	const uint8_t first = (uint8_t)(JINGLE_FIRST + shift), second = (uint8_t)(JINGLE_SECOND + shift);

	if (program < 0)
		return;
	if (jingle_frame == 0)
	{
		midi_message(0xB0u | channel, 7u, 0x7Fu, 3);    /* CC 7, the volume: all of it */
		midi_message(0xC0u | channel, (uint8_t)program, 0u, 2);
		midi_message(0x90u | channel, first, 0x7Fu, 3);
	}
	else if (jingle_frame == JINGLE_NOTE_FRAMES)
	{
		midi_message(0x80u | channel, first, 0u, 3);
		midi_message(0x90u | channel, second, 0x7Fu, 3);
	}
	else if (jingle_frame == 2 * JINGLE_NOTE_FRAMES)
	{
		midi_message(0x80u | channel, second, 0u, 3);
		midi_message(0xC0u | channel, 0u, 0u, 2);    /* the part's own program again */
	}
}

static void play_jingle(void)
{
	if (jingle_frame < 0)
		return;
	jingle_voice(JINGLE_CHANNEL, jingle_program, 0);
	jingle_voice(LAYER_CHANNEL, jingle_layer, -12);
	if (jingle_frame == 2 * JINGLE_NOTE_FRAMES)
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
	if (title_frames == 0u)
		queue_sound(TITLE_MUSIC, &write_index, &write_index_address);    /* the emblem: its music */
	sega_logo_shown = true;
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

/* The menu over the title: the cursor follows player 1's trackball up and down (or the actions up and
   down), the start button, the button or the actions accept and start pick */
static void run_menu(void)
{
	bool opening = menu_opening;

	menu_opening = false;
	if (menu_wait > 0)
	{
		menu_wait--;
		menu_travel = 0;
	}
	else
		menu_travel += (int16_t)trackball_1_y;
	if (actions & ACTION_BIT(ACTION_UP))
		menu_travel = -MENU_TRAVEL;
	if (actions & ACTION_BIT(ACTION_DOWN))
		menu_travel = MENU_TRAVEL;
	if (menu_travel <= -MENU_TRAVEL || menu_travel >= MENU_TRAVEL)
	{
		menu_choice = (menu_choice + (menu_travel > 0 ? 1 : MENU_ITEMS - 1)) % MENU_ITEMS;
		menu_travel = 0;
		menu_wait = MENU_PAUSE;
	}
	draw_menu();
	if (opening || ((inputs_pressed & (PRESSED_START_1 | PRESSED_BUTTON_1)) == 0u
		&& (actions & (ACTION_BIT(ACTION_ACCEPT) | ACTION_BIT(ACTION_START))) == 0u))
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
	case 1:    /* OPTIONS: not yet */
		break;
	default:   /* EXIT */
		quit_requested = true;
		break;
	}
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

void console_actions(uint32_t pressed)
{
	actions = pressed;
}

void console_back(void)
{
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
