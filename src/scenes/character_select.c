/**
 *    Name:        scenes/character_select.c
 *    Description: choosing a character: the PLAYER SELECT chooser at a new game and the chooser of a player joining
 *    Version:     0.1.0
 *    Created:     12-10-2026
 *    Modified:    16-10-2026
 *    Author:      Sebastian Toyos
 *
 *    SegaSonic The Hedgehog (arcade, Sega System 32): the game's V60 program decompiled into C by
 *    tools/decompile.py, with the names of symbols.txt.
 */
#include "game.h"

void reset_character_select(void)
{
	characters_taken = 0u;
	players_in_join_chooser = 0u;
	free_character_count = 0u;
	character_select_timer = 0;
	character_select_timer = 0;
}

void start_character_chooser(uint32_t block)
{
	uint16_t chooser_x = 0;
	uint32_t r2 = 0, character = 0, frame_index = 0, ignored = 0, object = 0, player = 0, x_index = 0;

	count_free_characters(&ignored, &r2, &object);
	players_choosing_character++;
	if (character_select_timer <= 0)
	{
		mem_write16(video_layer_disable, 0x2Fu);
		mem_write16(video_backdrop_control, 0u);
		show_nbg0_picture(&r2, &ignored, &ignored, &block);
		spawn_count = 2u;
		spawn_count = 0xAu;
		spawn_object_with_pieces(0x08F9E2u, &ignored, &ignored, &r2, &object, &block);
		OBJECT(object)->screen_x = 0x118;
		OBJECT(object)->screen_y = 0x1F;
		OBJECT(object)->sprite_palette = 0x8AD0u;
		OBJECT(object)->handler = character_select_countdown_update_ADDRESS;
		object = LINKED(object, OBJECT(object)->child);
		OBJECT(object)->screen_x = 0x90;
		OBJECT(object)->screen_y = 0x20;
		OBJECT(object)->sprite_frame = 0xB708u;
		OBJECT(object)->sprite_palette = 0x8D90u;
		OBJECT(object)->piece_value = 1u;
	}
	character_select_timer = 0x258;
	spawn_count = 8u;
	spawn_object_with_pieces(0x08FA36u, &ignored, &ignored, &r2, &object, &block);
	AS32(OBJECT(object)->hitbox_size_x) = current_player;
	player = OBJECT(object_cursor)->player;
	OBJECT(object)->kind = player;
	character = player;
	frame_index = character * 4u;
	while ((characters_taken & shl32(1u, (int)(character_taken_bits[character]))) != 0u)
	{
		character = (uint8_t)(character + 1u);
		frame_index = character * 4u;
		if ((int8_t)character <= 2)
			continue;
		character = 0u;
		frame_index &= ~0xFFu;
	}
	OBJECT(object)->sprite_frame = chooser_portrait_frames[frame_index];
	OBJECT(object)->sprite_palette = chooser_portrait_palettes[character];
	x_index = player;
	if ((cabinet_config & 1u) == 0u)
		x_index += 3u;
	chooser_x = chooser_x_by_player[x_index];
	OBJECT(object)->screen_x = chooser_x;
	OBJECT(object)->screen_y = 0xA0;
	OBJECT(object)->sprite_flags |= 0x100u;
	OBJECT(object)->character_chooser.character = player;
	OBJECT(object)->character_chooser.turn_direction = 1;
	OBJECT(object)->handler = character_chooser_wait_ADDRESS;
	OBJECT(object)->character_chooser.right_count = OBJECT(object)->screen_x;
	OBJECT(object)->character_chooser.left_count = OBJECT(object)->screen_y;
	frame_index = (frame_index & ~0xFFu) | (uint8_t)(frame_index + 4u);
	if ((int8_t)frame_index > 8)
		frame_index &= ~0xFFu;
	character = (uint8_t)(character + 1u);
	character = character % 3u;
	object = LINKED(object, OBJECT(object)->child);
	OBJECT(object)->sprite_flags |= 0x100u;
	OBJECT(object)->sprite_frame = chooser_portrait_frames[frame_index];
	OBJECT(object)->sprite_palette = chooser_portrait_palettes[character];
	OBJECT(object)->screen_x = chooser_x;
	OBJECT(object)->screen_x -= 0x18u;
	OBJECT(object)->screen_y = 0x88;
	OBJECT(object)->piece_value = character;
	OBJECT(object)->sprite_scale = 0xC0u;
	frame_index = (frame_index & ~0xFFu) | (uint8_t)(frame_index + 4u);
	if ((int8_t)frame_index > 8)
		frame_index &= ~0xFFu;
	character = (uint8_t)(character + 1u);
	character = character % 3u;
	object = LINKED(object, OBJECT(object)->child);
	OBJECT(object)->sprite_flags |= 0x100u;
	OBJECT(object)->sprite_frame = chooser_portrait_frames[frame_index];
	OBJECT(object)->sprite_palette = chooser_portrait_palettes[character];
	OBJECT(object)->screen_x = chooser_x;
	OBJECT(object)->screen_x += 0x18u;
	OBJECT(object)->screen_y = 0x88;
	OBJECT(object)->piece_value = character;
	OBJECT(object)->sprite_scale = 0xC0u;
	object = LINKED(object, OBJECT(object)->child);
	OBJECT(object)->sprite_palette = 0x8DB0u;
	OBJECT(object)->sprite_frame = character_name_sprites[player];
	OBJECT(object)->screen_x = chooser_x;
	OBJECT(object)->screen_y = 0xB8;
	object = LINKED(object, OBJECT(object)->child);
	OBJECT(object)->sprite_frame = chooser_player_labels[player];
	OBJECT(object)->sprite_palette = 0x8DA0u;
	OBJECT(object)->screen_x = chooser_x;
	OBJECT(object)->screen_y = 0xD0;
	object = LINKED(object, OBJECT(object)->child);
	OBJECT(object)->sprite_flags = 8u;
	OBJECT(object)->sprite_frame = 0xB740u;
	OBJECT(object)->sprite_palette = 0x8DC0u;
	OBJECT(object)->screen_x = chooser_x;
	OBJECT(object)->screen_y = 0x30;
}

void character_chooser_wait(void)
{
	uint8_t left_turns = 0, right_turns = 0, turn = 0;
	uint16_t trackball_x = 0;
	uint32_t r1 = 0, idle_frame = 0, kind = 0, piece_value = 0, taken_bit = 0;
	uint8_t right_steps = 0;
	uint16_t step_timer = 0;

	if (free_character_count == 1u || character_select_timer < 0x1E)
	{
		character_chooser_confirm();
		return;
	}
	kind = OBJECT(current_object)->kind;
	if ((inputs_pressed & shl32(1u, (int)(chooser_button_bits[kind]))) != 0u)
	{
		character_chooser_confirm();
		return;
	}
	trackball_x = RAM16(GAME_VARIABLES + chooser_trackball_offsets[kind]);
	if (trackball_x == 0u)
	{
	character_chooser_idle:
		r1 = character_taken_bits[OBJECT(current_object)->character_chooser.character];
		if ((characters_taken & shl32(1u, (int)(r1))) != 0u)
		{
			OBJECT(current_object)->character_chooser.step_timer = 0;
			OBJECT(current_object)->character_chooser.turn_timer = 0x18u;
			OBJECT(current_object)->handler = character_chooser_turn_ADDRESS;
			character_chooser_turn();
			return;
		}
		step_timer = OBJECT(current_object)->character_chooser.step_timer;
		OBJECT(current_object)->character_chooser.step_timer = step_timer - 1u;
		if (step_timer == 0u)
		{
			OBJECT(current_object)->character_chooser.step_timer = 3;
			r1 = OBJECT(current_object)->character_chooser.idle_step;
			idle_frame = chooser_idle_steps[r1];
			OBJECT(current_object)->sprite_frame = chooser_portrait_frames[(uint8_t)(idle_frame + OBJECT(current_object)->character_chooser.character * 4u)];
			OBJECT(current_object)->character_chooser.idle_step++;
			OBJECT(current_object)->character_chooser.idle_step &= 3u;
		}
		queue_sprite(r1);
		r14 = 2u;
		do
		{
			current_object = LINKED(current_object, OBJECT(current_object)->child);
			piece_value = OBJECT(current_object)->piece_value;
			taken_bit = character_taken_bits[piece_value];
			if ((characters_taken & shl32(1u, (int)(taken_bit))) != 0u)
			{
				OBJECT(current_object)->sprite_palette = chooser_portrait_palettes[piece_value];
				OBJECT(current_object)->sprite_palette = 0x87A0u;
			}
			queue_sprite(taken_bit);
			r14 = r14 - 1u;
		} while (r14 != 0u);
		draw_character_chooser_labels();
		return;
	}
	if ((int16_t)trackball_x < 0)
	{
		OBJECT(current_object)->character_chooser.right_count = 0u;
		OBJECT(current_object)->character_chooser.left_count += 8u;
	}
	else
	{
		OBJECT(current_object)->character_chooser.right_count += 8u;
		OBJECT(current_object)->character_chooser.left_count = 0u;
	}
	right_turns = OBJECT(current_object)->character_chooser.right_turns;
	left_turns = OBJECT(current_object)->character_chooser.left_turns;
	OBJECT(current_object)->character_chooser.left_turns = 0u;
	OBJECT(current_object)->character_chooser.right_turns = 0u;
	right_steps = right_turns;
	turn = (uint8_t)(right_steps - left_turns);
	if (right_steps == left_turns)
		goto character_chooser_idle;
	OBJECT(current_object)->character_chooser.turn_direction = turn;
	OBJECT(current_object)->character_chooser.step_timer = 0;
	OBJECT(current_object)->character_chooser.turn_timer = 0x18u;
	OBJECT(current_object)->handler = character_chooser_turn_ADDRESS;
	character_chooser_turn();
}

void character_chooser_confirm(void)
{
	uint8_t sound_id = 0;
	uint32_t character = 0, chosen_character = 0, frame_index = 0, ignored = 0, random_value = 0, x_index = 0;

	while ((characters_taken & shl32(1u, (int)(character_taken_bits[OBJECT(current_object)->character_chooser.character]))) != 0u)
	{
		OBJECT(current_object)->character_chooser.character++;
		OBJECT(current_object)->character_chooser.character = OBJECT(current_object)->character_chooser.character % 3u;
	}
	character = OBJECT(current_object)->character_chooser.character;
	OBJECT(LINKED(current_object, OBJECT(current_object)->parent))->kind = character;
	characters_taken |= shl32(1u, (int)(character_taken_bits[character]));
	free_character_count--;
	players_choosing_character--;
	if ((int8_t)players_choosing_character < 1)
		character_select_timer = 0x3C;
	OBJECT(current_object)->sprite_scale = 0x100u;
	x_index = OBJECT(current_object)->kind;
	if ((cabinet_config & 1u) == 0u)
		x_index += 3u;
	OBJECT(current_object)->screen_x = chooser_x_by_player[x_index];
	OBJECT(current_object)->screen_y = 0xA0;
	frame_index = OBJECT(current_object)->character_chooser.character;
	chosen_character = frame_index;
	frame_index = (uint8_t)(frame_index * 4u);
	OBJECT(current_object)->sprite_frame = chooser_portrait_frames[(uint8_t)(frame_index + 3u)];
	OBJECT(current_object)->sprite_palette = chooser_portrait_palettes[chosen_character];
	OBJECT(current_object)->handler = character_chooser_chosen_ADDRESS;
	next_random(&random_value, &ignored, &ignored);
	sound_id = ROM8(chooser_voice_sounds_ADDRESS + (OBJECT(current_object)->character_chooser.character * 4u + (random_value & 3u)));
	play_sound(sound_id, &ignored, &ignored);
	sound_id = 0x20u;
	play_sound(sound_id, &ignored, &ignored);
	character_chooser_chosen();
}

void draw_character_chooser_labels(void)
{
	uint32_t ignored = 0;

	current_object = LINKED(current_object, OBJECT(current_object)->child);
	if (OBJECT(object_cursor)->draw_character_chooser_labels.name_changed != 0u)
		OBJECT(current_object)->sprite_frame = character_name_sprites[OBJECT(object_cursor)->draw_character_chooser_labels.character];
	ignored = queue_sprite(0u);
	current_object = LINKED(current_object, OBJECT(current_object)->child);
	ignored = queue_sprite(ignored);
	current_object = LINKED(current_object, OBJECT(current_object)->child);
	queue_sprite(ignored);
	OBJECT(object_cursor)->draw_character_chooser_labels.name_changed = 0u;
}

void character_chooser_turn(void)
{
	uint16_t turn_step = 0;
	uint32_t r1 = 0, character = 0, idle_frame = 0, piece_value = 0, taken_bit = 0, x_index = 0;
	uint8_t next_character = 0;
	uint16_t turn_timer = 0, step_timer = 0, scale_step = 0;

	for (;;)
	{
		if (free_character_count == 1u)
		{
			character_chooser_confirm();
			return;
		}
		if (character_select_timer < 0x1E)
			character_select_timer = 0x1D;
		turn_timer = OBJECT(current_object)->character_chooser.turn_timer;
		OBJECT(current_object)->character_chooser.turn_timer = turn_timer - 1u;
		if (turn_timer != 0u)
			break;
		x_index = OBJECT(current_object)->kind;
		if ((cabinet_config & 1u) == 0u)
			x_index += 3u;
		OBJECT(current_object)->screen_x = chooser_x_by_player[x_index];
		OBJECT(current_object)->screen_y = 0xA0;
		OBJECT(current_object)->sprite_scale = 0x100u;
		next_character = OBJECT(current_object)->character_chooser.character + OBJECT(current_object)->character_chooser.turn_direction;
		OBJECT(current_object)->character_chooser.character = next_character;
		if ((int8_t)next_character < 0)
			OBJECT(current_object)->character_chooser.character = 2u;
		OBJECT(current_object)->character_chooser.character = OBJECT(current_object)->character_chooser.character % 3u;
		current_object = LINKED(current_object, OBJECT(current_object)->child);
		OBJECT(current_object)->screen_x = OBJECT(object_cursor)->screen_x;
		OBJECT(current_object)->screen_x -= 0x18u;
		OBJECT(current_object)->screen_y = 0x88;
		OBJECT(current_object)->sprite_scale = 0xC0u;
		OBJECT(current_object)->piece_value = OBJECT(object_cursor)->character_chooser.character;
		OBJECT(current_object)->piece_value++;
		OBJECT(current_object)->piece_value = OBJECT(current_object)->piece_value % 3u;
		piece_value = OBJECT(current_object)->piece_value;
		OBJECT(current_object)->sprite_frame = chooser_portrait_frames[piece_value * 4u];
		OBJECT(current_object)->sprite_palette = chooser_portrait_palettes[piece_value];
		if ((characters_taken & shl32(1u, (int)(character_taken_bits[piece_value]))) != 0u)
		{
			OBJECT(current_object)->sprite_palette = chooser_portrait_palettes[piece_value];
			OBJECT(current_object)->sprite_palette = 0x87A0u;
		}
		current_object = LINKED(current_object, OBJECT(current_object)->child);
		OBJECT(current_object)->screen_x = OBJECT(object_cursor)->screen_x;
		OBJECT(current_object)->screen_x += 0x18u;
		OBJECT(current_object)->screen_y = 0x88;
		OBJECT(current_object)->sprite_scale = 0xC0u;
		OBJECT(current_object)->piece_value = OBJECT(object_cursor)->character_chooser.character;
		OBJECT(current_object)->piece_value += 2u;
		OBJECT(current_object)->piece_value = OBJECT(current_object)->piece_value % 3u;
		piece_value = OBJECT(current_object)->piece_value;
		OBJECT(current_object)->sprite_frame = chooser_portrait_frames[piece_value * 4u];
		OBJECT(current_object)->sprite_palette = chooser_portrait_palettes[piece_value];
		if ((characters_taken & shl32(1u, (int)(character_taken_bits[piece_value]))) != 0u)
		{
			OBJECT(current_object)->sprite_palette = chooser_portrait_palettes[piece_value];
			OBJECT(current_object)->sprite_palette = 0x87A0u;
		}
		current_object = object_cursor;
		character = OBJECT(current_object)->character_chooser.character;
		OBJECT(current_object)->sprite_frame = chooser_portrait_frames[character * 4u];
		OBJECT(current_object)->sprite_palette = chooser_portrait_palettes[character];
		r1 = character_taken_bits[character];
		if ((characters_taken & shl32(1u, (int)(r1))) == 0u)
		{
			OBJECT(current_object)->character_chooser.name_changed = 1u;
			OBJECT(current_object)->handler = character_chooser_wait_ADDRESS;
			r1 = character_taken_bits[OBJECT(current_object)->character_chooser.character];
			if ((characters_taken & shl32(1u, (int)(r1))) != 0u)
			{
				OBJECT(current_object)->character_chooser.step_timer = 0;
				OBJECT(current_object)->character_chooser.turn_timer = 0x18u;
				OBJECT(current_object)->handler = character_chooser_turn_ADDRESS;
				continue;
			}
			step_timer = OBJECT(current_object)->character_chooser.step_timer;
			OBJECT(current_object)->character_chooser.step_timer = step_timer - 1u;
			if (step_timer == 0u)
			{
				OBJECT(current_object)->character_chooser.step_timer = 3;
				r1 = OBJECT(current_object)->character_chooser.idle_step;
				idle_frame = chooser_idle_steps[r1];
				OBJECT(current_object)->sprite_frame = chooser_portrait_frames[(uint8_t)(idle_frame + OBJECT(current_object)->character_chooser.character * 4u)];
				OBJECT(current_object)->character_chooser.idle_step++;
				OBJECT(current_object)->character_chooser.idle_step &= 3u;
			}
			queue_sprite(r1);
			r14 = 2u;
			do
			{
				current_object = LINKED(current_object, OBJECT(current_object)->child);
				piece_value = OBJECT(current_object)->piece_value;
				taken_bit = character_taken_bits[piece_value];
				if ((characters_taken & shl32(1u, (int)(taken_bit))) != 0u)
				{
					OBJECT(current_object)->sprite_palette = chooser_portrait_palettes[piece_value];
					OBJECT(current_object)->sprite_palette = 0x87A0u;
				}
				queue_sprite(taken_bit);
				r14 = r14 - 1u;
			} while (r14 != 0u);
			draw_character_chooser_labels();
			return;
		}
		OBJECT(current_object)->sprite_palette = chooser_portrait_palettes[character];
		OBJECT(current_object)->sprite_palette = 0x87A0u;
		OBJECT(current_object)->character_chooser.step_timer = 0;
		OBJECT(current_object)->character_chooser.turn_timer = 0x18u;
		OBJECT(current_object)->handler = character_chooser_turn_ADDRESS;
	}
	r14 = OBJECT(current_object)->character_chooser.turn_direction;
	turn_step = (uint16_t)r14;
	OBJECT(current_object)->screen_x += turn_step;
	OBJECT(current_object)->screen_y--;
	scale_step = r14 * 2u;
	r14 = scale_step;
	if ((int16_t)scale_step >= 0)
		OBJECT(current_object)->sprite_scale -= r14;
	else
		OBJECT(current_object)->sprite_scale += r14;
	r1 = queue_sprite(r1);
	current_object = LINKED(current_object, OBJECT(current_object)->child);
	if ((uint8_t)turn_step == 1u)
	{
		OBJECT(current_object)->screen_x += turn_step;
		OBJECT(current_object)->screen_y += turn_step;
		OBJECT(current_object)->sprite_scale += r14;
	}
	else
		OBJECT(current_object)->screen_x -= r14;
	r1 = queue_sprite(r1);
	current_object = LINKED(current_object, OBJECT(current_object)->child);
	if ((uint8_t)turn_step == 0xFFu)
	{
		OBJECT(current_object)->screen_x += turn_step;
		OBJECT(current_object)->screen_y -= turn_step;
		OBJECT(current_object)->sprite_scale -= r14;
	}
	else
		OBJECT(current_object)->screen_x -= r14;
	queue_sprite(r1);
	draw_character_chooser_labels();
}

void character_chooser_chosen(void)
{
	uint32_t ignored = 0;

	if (character_select_timer <= 0)
	{
		delete_object_and_children(&ignored, &ignored, &ignored);
		return;
	}
	ignored = queue_sprite(0u);
	current_object = LINKED(current_object, OBJECT(current_object)->child);
	current_object = LINKED(current_object, OBJECT(current_object)->child);
	current_object = LINKED(current_object, OBJECT(current_object)->child);
	OBJECT(current_object)->sprite_frame = character_name_sprites[OBJECT(object_cursor)->character_chooser.character];
	ignored = queue_sprite(ignored);
	current_object = LINKED(current_object, OBJECT(current_object)->child);
	ignored = queue_sprite(ignored);
	current_object = LINKED(current_object, OBJECT(current_object)->child);
	queue_sprite(ignored);
}

void character_select_countdown_update(void)
{
	uint32_t ignored = 0, seconds = 0;
	uint16_t time_left = 0, direction = 0;

	if (players_choosing_character == 0u && character_select_timer <= 0)
	{
		delete_object_and_children(&ignored, &ignored, &ignored);
		return;
	}
	time_left = character_select_timer;
	character_select_timer = time_left - 1u;
	if (time_left == 0u)
		character_select_timer = 1;
	seconds = (uint16_t)character_select_timer;
	seconds = (uint16_t)((int16_t)seconds / 0x3C);
	if (OBJECT(current_object)->character_select_countdown.shown_seconds != (uint8_t)seconds)
	{
		OBJECT(current_object)->character_select_countdown.shown_seconds = seconds;
		OBJECT(current_object)->sprite_frame = ranking_letter_sprites[(uint16_t)(seconds + 0x20u)];
		OBJECT(current_object)->sprite_scale = 0x10u;
	}
	if ((int16_t)OBJECT(current_object)->sprite_scale < 0x140)
		OBJECT(current_object)->sprite_scale += 8u;
	ignored = queue_sprite(0u);
	current_object = LINKED(current_object, OBJECT(current_object)->child);
	direction = OBJECT(current_object)->direction;
	OBJECT(current_object)->direction = direction - 1u;
	if (direction != 0u)
	{
		queue_sprite(ignored);
		return;
	}
	OBJECT(current_object)->direction = 2u;
	OBJECT(current_object)->sprite_palette = player_select_title_palettes[(uint8_t)OBJECT(current_object)->piece_frame];
	OBJECT(current_object)->piece_frame += OBJECT(current_object)->piece_value;
	if (OBJECT(current_object)->piece_frame > 0xC)
	{
		OBJECT(current_object)->direction_low = 8u;
		OBJECT(current_object)->piece_frame = 0xC;
		OBJECT(current_object)->piece_value = 0xFFu;
	}
	if (OBJECT(current_object)->piece_frame < 0)
	{
		OBJECT(current_object)->direction_low = 8u;
		OBJECT(current_object)->piece_frame = 0;
		OBJECT(current_object)->piece_value = 1u;
	}
	queue_sprite(ignored);
}

uint32_t start_join_character_chooser(void)
{
	uint16_t chooser_x = 0;
	uint32_t block = 0, character = 0, ignored = 0, object = 0, player = 0, x_index = 0;

	count_free_characters(&x_index, &ignored, &ignored);
	if (free_character_count == 0u)
		return x_index;
	character = OBJECT(object_cursor)->kind;
	player = OBJECT(object_cursor)->player;
	block = join_chooser_blocks[player];
	object = block;
	init_object(object);
	while ((characters_taken & shl32(1u, (int)(mem_read8(character_taken_bits_ADDRESS + character)))) != 0u)
	{
		character = ((character & ~0xFFu) | (uint8_t)(character + 1u)) % 3u;
	}
	OBJECT(block)->kind = player;
	AS32(players_in_join_chooser) |= shl32(1u, (int)(player));
	AS32(OBJECT(block)->hitbox_size_x) = current_player;
	x_index = player;
	if ((cabinet_config & 1u) == 0u)
		x_index += 3u;
	chooser_x = chooser_x_by_player[x_index];
	OBJECT(block)->screen_x = chooser_x;
	OBJECT(block)->screen_y = 0xCC;
	OBJECT(block)->join_character_chooser.countdown = 0x21Cu;
	OBJECT(block)->sprite_flags |= 0x100u;
	OBJECT(block)->sprite_frame = join_chooser_portraits[character];
	OBJECT(block)->sprite_palette = join_chooser_palettes[character];
	OBJECT(block)->join_character_chooser.character = character;
	OBJECT(block)->handler = join_character_chooser_update_ADDRESS;
	OBJECT(block)->hitbox_centre_y = 1;
	OBJECT(block)->child = block;
	OBJECT(block)->child += 0xA0u;
	block = LINKED(block, OBJECT(block)->child);
	object = block;
	init_object_2(object);
	OBJECT(block)->screen_x = chooser_x;
	OBJECT(block)->screen_y = 0xDD;
	OBJECT(block)->sprite_frame = character_name_sprites[character];
	OBJECT(block)->sprite_palette = 0x8DB0u;
	OBJECT(block)->piece_value = character;
	return x_index;
}

void join_character_chooser_update(uint32_t text_x)
{
	uint8_t left_steps = 0, right_turns = 0, turn = 0;
	uint16_t trackball_x = 0, y = 0;
	uint32_t character = 0, kind = 0, palette = 0, player = 0, scale = 0, taken_bit = 0, text = 0;
	uint8_t right_steps = 0, next_character = 0;
	uint16_t time_left = 0;

	player = OBJECT(object_cursor)->player;
	if ((AS32(players_in_join_chooser) & shl32(1u, (int)(player))) == 0u)
		return;
	current_object = join_chooser_blocks[player];
	time_left = OBJECT(current_object)->join_character_chooser.countdown;
	OBJECT(current_object)->join_character_chooser.countdown = time_left - 1u;
	if (time_left == 0u)
	{
		for (;;)
		{
		join_character_chooser_take_character:
			character = OBJECT(current_object)->join_character_chooser.character;
			OBJECT(object_cursor)->kind = character;
			taken_bit = character_taken_bits[character];
			if ((characters_taken & shl32(1u, (int)(taken_bit))) == 0u)
				break;
			OBJECT(current_object)->join_character_chooser.character++;
			OBJECT(current_object)->join_character_chooser.character = OBJECT(current_object)->join_character_chooser.character % 3u;
		}
		characters_taken |= shl32(1u, (int)(taken_bit));
		AS32(players_in_join_chooser) &= ~shl32(1u, (int)(OBJECT(current_object)->kind));
	}
	else
	{
		text_x = (text_x & ~0xFFFFu) | (uint16_t)OBJECT(current_object)->screen_x;
		text_x = (text_x & ~0xFFFFu) | (uint16_t)(text_x - 0x25u);
		y = (uint16_t)(OBJECT(current_object)->screen_y - 0x13u);
		palette = 0x8AD0u;
		scale = 0x140u;
		text = countdown_second_texts_ADDRESS + (int32_t)OBJECT(current_object)->join_character_chooser.countdown / 0x3C * 3u;
		queue_string_at(text_x, y, &palette, &scale, &text);
		text_x = (text_x & ~0xFFFFu) | (uint16_t)OBJECT(current_object)->screen_x;
		text_x = (text_x & ~0xFFFFu) | (uint16_t)(text_x + 9u);
		y = (uint16_t)(OBJECT(current_object)->screen_y - 0x13u);
		queue_string_at(text_x, y, &palette, &scale, &text);
		if (joining_closed != 0u)
			goto join_character_chooser_take_character;
		kind = OBJECT(current_object)->kind;
		if ((inputs_pressed & shl32(1u, (int)(chooser_button_bits[kind]))) != 0u)
			goto join_character_chooser_take_character;
		trackball_x = RAM16(GAME_VARIABLES + chooser_trackball_offsets[kind]);
		if (trackball_x != 0u)
		{
			if ((int16_t)trackball_x < 0)
			{
				OBJECT(current_object)->spawn_arg_1 = 0u;
				OBJECT(current_object)->spawn_arg_2 += 8u;
			}
			else
			{
				OBJECT(current_object)->spawn_arg_1 += 8u;
				OBJECT(current_object)->spawn_arg_2 = 0u;
			}
			right_turns = OBJECT(current_object)->spawn_arg_1_high;
			left_steps = OBJECT(current_object)->spawn_arg_2_high;
			OBJECT(current_object)->spawn_arg_2_high = 0u;
			OBJECT(current_object)->spawn_arg_1_high = 0u;
			right_steps = right_turns;
			turn = (uint8_t)(right_steps - left_steps);
			if (right_steps != left_steps)
			{
				OBJECT(current_object)->hitbox_centre_y = turn;
				OBJECT(current_object)->join_character_chooser.portrait_changed = 1u;
				next_character = OBJECT(current_object)->join_character_chooser.character + turn;
				OBJECT(current_object)->join_character_chooser.character = next_character;
				if ((int8_t)next_character >= 0)
					OBJECT(current_object)->join_character_chooser.character = OBJECT(current_object)->join_character_chooser.character % 3u;
				else
					OBJECT(current_object)->join_character_chooser.character = 2u;
			}
		}
		for (;;)
		{
			character = OBJECT(current_object)->join_character_chooser.character;
			if ((characters_taken & shl32(1u, (int)(character_taken_bits[character]))) == 0u)
				break;
			OBJECT(current_object)->join_character_chooser.portrait_changed = 1u;
			OBJECT(current_object)->join_character_chooser.character += OBJECT(current_object)->hitbox_centre_y;
			if ((int8_t)OBJECT(current_object)->join_character_chooser.character < 0)
				OBJECT(current_object)->join_character_chooser.character = 2u;
			OBJECT(current_object)->join_character_chooser.character = OBJECT(current_object)->join_character_chooser.character % 3u;
		}
		if (OBJECT(current_object)->join_character_chooser.portrait_changed != 0u)
		{
			OBJECT(current_object)->sprite_frame = join_chooser_portraits[character];
			OBJECT(current_object)->sprite_palette = join_chooser_palettes[character];
		}
		queue_sprite_on_top();
		OBJECT(current_object)->join_character_chooser.portrait_changed = 0u;
		r14 = current_object;
		current_object = LINKED(current_object, OBJECT(current_object)->child);
		if (OBJECT(current_object)->piece_value != OBJECT(r14)->join_character_chooser.character)
		{
			OBJECT(current_object)->piece_value = OBJECT(r14)->join_character_chooser.character;
			OBJECT(current_object)->sprite_frame = character_name_sprites[OBJECT(current_object)->piece_value];
		}
		queue_sprite_on_top();
	}
	current_object = object_cursor;
}

void count_free_characters(uint32_t *p_characters_left, uint32_t *p_character, uint32_t *p_taken_bit)
{
	uint8_t free_characters = 0;
	uint32_t character = 0, characters_left = 0, taken_bit = 0;

	characters_left = 3u;
	free_characters = 0u;
	do
	{
		character = characters_left - 1u;
		taken_bit = ROM8(character_taken_bits_ADDRESS + character);
		if ((characters_taken & shl32(1u, (int)(taken_bit))) == 0u)
			free_characters++;
		characters_left--;
	} while (characters_left != 0u);
	free_character_count = free_characters;
	*p_characters_left = characters_left;
	*p_character = character;
	*p_taken_bit = taken_bit;
}
