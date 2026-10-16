/**
 *    Name:        objects/eggman.c
 *    Description: Eggman: his booth that slides in during his speeches, the speech scripts (stages 3, 8, A), his scene with the button box in stage B and the sliding door, his teleport (never spawned)
 *    Version:     0.1.0
 *    Created:     12-10-2026
 *    Modified:    16-10-2026
 *    Author:      Sebastian Toyos
 *
 *    SegaSonic The Hedgehog (arcade, Sega System 32): the game's V60 program decompiled into C by
 *    tools/decompile.py, with the names of symbols.txt.
 */
#include "game.h"

void spawn_eggman_booth(uint32_t spawned, uint32_t piece_count, uint32_t booth, uint32_t *p_free_count, uint32_t *p_block)
{
	uint32_t free_count = *p_free_count;
	uint32_t block = *p_block;
	uint32_t ignored = 0, object = 0;

	spawn_count = 2u;
	spawn_object_with_pieces(0x012B2Fu, &spawned, &ignored, &free_count, &booth, &block);
	if ((uint8_t)spawned == 0u)
	{
		*p_free_count = free_count;
		*p_block = block;
		return;
	}
	OBJECT(booth)->sprite_flags |= 0x100u;
	OBJECT(booth)->eggman_booth.step_timer = 0x2F;
	OBJECT(booth)->sprite_palette = 0x8200u;
	OBJECT(booth)->screen_x = 0x9F;
	OBJECT(booth)->handler = eggman_booth_slide_in_ADDRESS;
	object = LINKED(booth, OBJECT(booth)->child);
	OBJECT(object)->sprite_frame = 0x5718u;
	OBJECT(object)->sprite_palette = 0x8210u;
	OBJECT(object)->screen_x = 0x9F;
	OBJECT(object)->sprite_flags |= 0x100u;
	*p_free_count = free_count;
	*p_block = block;
}

void eggman_booth_slide_in(void)
{
	OBJECT(current_object)->eggman_booth.slide_y += 4u;
	if (OBJECT(current_object)->eggman_booth.slide_y < 0x2C)
	{
		draw_eggman_booth();
		return;
	}
	OBJECT(current_object)->eggman_booth.slide_y = 0x2F;
	OBJECT(current_object)->handler = eggman_booth_animate_ADDRESS;
	OBJECT(current_object)->eggman_booth.step = 0u;
	eggman_booth_animate();
}

void eggman_booth_animate(void)
{
	uint32_t ignored = 0;
	uint16_t slide_y = 0;

	if (OBJECT(current_object)->eggman_booth.step != 0x1Du)
	{
		OBJECT(current_object)->eggman_booth.step_timer--;
		if (OBJECT(current_object)->eggman_booth.step_timer >= 0)
		{
			draw_eggman_booth();
			return;
		}
		OBJECT(current_object)->eggman_booth.step++;
		OBJECT(current_object)->eggman_booth.step_timer = eggman_booth_frame_durations[eggman_booth_steps[OBJECT(current_object)->eggman_booth.step]];
		draw_eggman_booth();
		return;
	}
	slide_y = OBJECT(current_object)->eggman_booth.slide_y;
	OBJECT(current_object)->eggman_booth.slide_y = slide_y - 4u;
	if (slide_y >= 4u)
	{
		draw_eggman_booth();
		return;
	}
	delete_object_and_children(&ignored, &ignored, &ignored);
}

void draw_eggman_booth(void)
{
	uint8_t bucket = 0, sound_id = 0;
	uint32_t booth_frame = 0, ignored = 0;

	OBJECT(current_object)->screen_y = OBJECT(object_cursor)->draw_eggman_booth.slide_y;
	booth_frame = eggman_booth_steps[OBJECT(current_object)->draw_eggman_booth.step];
	OBJECT(current_object)->sprite_frame = eggman_booth_frames[booth_frame];
	if (stage != 0u && stage != 5u && booth_frame == 0xBu)
	{
		sound_id = 0x41u;
		play_sound(sound_id, &ignored, &ignored);
	}
	bucket = 0u;
	queue_sprite_in_bucket(bucket);
	current_object = LINKED(current_object, OBJECT(current_object)->child);
	OBJECT(current_object)->screen_y = OBJECT(object_cursor)->draw_eggman_booth.slide_y;
	bucket = 0u;
	queue_sprite_in_bucket(bucket);
}

void spawn_eggman_teleport(uint32_t free_count, uint32_t teleport, uint32_t block)
{
	uint32_t ignored = 0;

	spawn_count = 2u;
	spawn_object_with_pieces(0x013434u, &ignored, &ignored, &free_count, &teleport, &block);
	OBJECT(teleport)->handler = eggman_teleport_init_ADDRESS;
}

void eggman_teleport_init(void)
{
	OBJECT(current_object)->sprite_scale = 0x180u;
	OBJECT(current_object)->sprite_flags |= 0x100u;
	OBJECT(current_object)->sprite_palette = 0x8220u;
	OBJECT(current_object)->handler = eggman_teleport_animate_ADDRESS;
	current_object = LINKED(current_object, OBJECT(current_object)->child);
	OBJECT(current_object)->sprite_frame = 0x79A8u;
}

void eggman_teleport_animate(void)
{
	uint32_t depth = 0, frame_index = 0, ignored = 0;
	uint16_t step_timer = 0, odd_frame = 0, flicker_step = 0;

	unproject_object(&ignored, &ignored, &ignored, &ignored);
	step_timer = OBJECT(current_object)->eggman_teleport.step_timer;
	OBJECT(current_object)->eggman_teleport.step_timer = step_timer - 1u;
	if (step_timer == 0u)
	{
		frame_index = eggman_teleport_steps[OBJECT(current_object)->eggman_teleport.step];
		OBJECT(current_object)->eggman_teleport.frame_index = frame_index;
		if (frame_index == 0xFFu)
		{
			delete_object_and_children(&ignored, &ignored, &ignored);
			return;
		}
		OBJECT(current_object)->sprite_frame = eggman_teleport_frames[frame_index];
		OBJECT(current_object)->eggman_teleport.step_timer = eggman_teleport_durations[frame_index];
		OBJECT(current_object)->eggman_teleport.step++;
	}
	OBJECT(current_object)->eggman_teleport.beam_phase++;
	odd_frame = OBJECT(current_object)->eggman_teleport.beam_phase & 1u;
	OBJECT(current_object)->eggman_teleport.beam_phase = odd_frame;
	if (odd_frame != 0u)
	{
		current_object = LINKED(current_object, OBJECT(current_object)->child);
		place_sprite_at_cursor_floor();
		OBJECT(current_object)->sprite_palette = 0u;
		OBJECT(current_object)->screen_x += 8u;
		depth = (uint16_t)(OBJECT(current_object)->screen_y - 2u);
		queue_sprite_at_depth(&ignored, &depth);
		current_object = object_cursor;
	}
	OBJECT(current_object)->eggman_teleport.flicker_phase++;
	flicker_step = OBJECT(current_object)->eggman_teleport.flicker_phase & 3u;
	OBJECT(current_object)->eggman_teleport.flicker_phase = flicker_step;
	if (flicker_step != 0u)
		queue_sprite_at_floor();
}

void eggman_scene_init(void)
{
	uint32_t block = 0, free_count = 0, ignored = 0, object = 0;

	OBJECT(current_object)->sprite_frame = 0x88A8u;
	OBJECT(current_object)->sprite_palette = 0x81F0u;
	OBJECT(current_object)->sprite_flags |= 0x100u;
	OBJECT(current_object)->eggman_scene.step_timer = 0x3Cu;
	OBJECT(current_object)->handler = eggman_scene_wait_ADDRESS;
	current_object = LINKED(current_object, OBJECT(current_object)->child);
	OBJECT(current_object)->sprite_frame = 0xAA50u;
	OBJECT(current_object)->sprite_palette = 0x250u;
	OBJECT(current_object)->sprite_palette = 0x9FFEu;
	spawn_count = 1u;
	spawn_object_with_pieces(0x01409Cu, &ignored, &ignored, &free_count, &object, &block);
	copy_position_to(object);
	OBJECT(object)->projected_x -= 0x1Eu;
	OBJECT(object)->projected_y_int += 0x12u;
	OBJECT(object)->handler = button_box_init_ADDRESS;
}

void eggman_scene_wait(void)
{
	if ((int16_t)scroll_position <= 0xFC0)
	{
		eggman_scene_move();
		return;
	}
	scroll_mode = 2u;
	stage_trigger_bits = 1u;
	OBJECT(current_object)->handler = eggman_scene_pause_ADDRESS;
	eggman_scene_pause();
}

void eggman_scene_pause(void)
{
	uint16_t step_timer = 0;

	step_timer = OBJECT(current_object)->eggman_scene.step_timer;
	OBJECT(current_object)->eggman_scene.step_timer = step_timer - 1u;
	if (step_timer != 0u)
	{
		eggman_scene_move();
		return;
	}
	OBJECT(current_object)->eggman_scene.step_timer = 0u;
	OBJECT(current_object)->eggman_scene.phase = 1u;
	OBJECT(current_object)->handler = eggman_scene_talk_ADDRESS;
	eggman_scene_talk();
}

void eggman_scene_talk(void)
{
	uint8_t sound_id = 0;
	uint16_t kind = 0, screen_x = 0, screen_y = 0, text_duration = 0;
	uint32_t direction = 0, ignored = 0;
	uint16_t step_timer = 0, talk_timer = 0;

	step_timer = OBJECT(current_object)->eggman_scene.step_timer;
	OBJECT(current_object)->eggman_scene.step_timer = step_timer - 1u;
	if (step_timer != 0u)
	{
		talk_timer = OBJECT(current_object)->eggman_scene.talk_timer;
		OBJECT(current_object)->eggman_scene.talk_timer = talk_timer - 1u;
		if (talk_timer != 0u)
		{
			eggman_scene_move();
			return;
		}
		OBJECT(current_object)->eggman_scene.talk_timer = 6u;
		OBJECT(current_object)->sprite_frame = eggman_talk_frames[eggman_talk_steps[(uint8_t)OBJECT(current_object)->eggman_scene.step]];
		OBJECT(current_object)->eggman_scene.step++;
		OBJECT(current_object)->eggman_scene.step = (uint8_t)OBJECT(current_object)->eggman_scene.step % 0xAu;
		eggman_scene_move();
		return;
	}
	if ((int16_t)OBJECT(current_object)->direction > 2)
	{
		OBJECT(current_object)->eggman_scene.step = 0;
		OBJECT(current_object)->eggman_scene.step_timer = 0u;
		OBJECT(current_object)->handler = eggman_scene_press_button_ADDRESS;
		eggman_scene_press_button();
		return;
	}
	direction = OBJECT(current_object)->direction;
	screen_x = 0xA0u;
	screen_y = 0x80u;
	kind = eggman_scene_texts[direction];
	text_duration = eggman_scene_text_durations[direction];
	OBJECT(current_object)->eggman_scene.step_timer = text_duration;
	spawn_message_box(screen_x, screen_y, kind, text_duration, 0u);
	sound_id = (uint8_t)eggman_scene_sounds[OBJECT(current_object)->direction];
	play_sound(sound_id, &ignored, &ignored);
	OBJECT(current_object)->direction++;
	talk_timer = OBJECT(current_object)->eggman_scene.talk_timer;
	OBJECT(current_object)->eggman_scene.talk_timer = talk_timer - 1u;
	if (talk_timer != 0u)
	{
		eggman_scene_move();
		return;
	}
	OBJECT(current_object)->eggman_scene.talk_timer = 6u;
	OBJECT(current_object)->sprite_frame = eggman_talk_frames[eggman_talk_steps[(uint8_t)OBJECT(current_object)->eggman_scene.step]];
	OBJECT(current_object)->eggman_scene.step++;
	OBJECT(current_object)->eggman_scene.step = (uint8_t)OBJECT(current_object)->eggman_scene.step % 0xAu;
	eggman_scene_move();
}

void eggman_scene_press_button(void)
{
	uint32_t step = 0;
	uint16_t step_timer = 0;

	step_timer = OBJECT(current_object)->eggman_scene.step_timer;
	OBJECT(current_object)->eggman_scene.step_timer = step_timer - 1u;
	if (step_timer != 0u)
	{
		eggman_scene_move();
		return;
	}
	step = (uint8_t)OBJECT(current_object)->eggman_scene.step;
	OBJECT(current_object)->eggman_scene.step_timer = eggman_press_durations[step];
	OBJECT(current_object)->sprite_frame = eggman_press_frames[step];
	OBJECT(current_object)->eggman_scene.step++;
	if ((uint8_t)OBJECT(current_object)->eggman_scene.step == 5u)
	{
		OBJECT(current_object)->eggman_scene.phase = 2u;
		escape_started = 1u;
	}
	if (OBJECT(current_object)->eggman_scene.step <= 6)
	{
		eggman_scene_move();
		return;
	}
	OBJECT(current_object)->eggman_scene.step = 0;
	OBJECT(current_object)->eggman_scene.step_timer = 0x3Cu;
	OBJECT(current_object)->handler = eggman_scene_run_off_ADDRESS;
	eggman_scene_run_off();
}

void eggman_scene_run_off(void)
{
	uint32_t direction = 0, speed = 0, speed_x = 0, speed_y = 0;
	uint16_t step_timer = 0;

	step_timer = OBJECT(current_object)->eggman_scene.step_timer;
	OBJECT(current_object)->eggman_scene.step_timer = step_timer - 1u;
	if (step_timer != 0u)
	{
		eggman_scene_move();
		return;
	}
	OBJECT(current_object)->eggman_scene.step_timer = 2u;
	OBJECT(current_object)->sprite_frame = eggman_run_frames[(uint8_t)OBJECT(current_object)->eggman_scene.step];
	OBJECT(current_object)->eggman_scene.step++;
	OBJECT(current_object)->eggman_scene.step = (uint8_t)OBJECT(current_object)->eggman_scene.step % 8u;
	direction = 0x2A0u;
	speed = 0x20u;
	direction_to_speed(speed, &direction, &speed_x, &speed_y);
	OBJECT(current_object)->speed_x += speed_x;
	OBJECT(current_object)->speed_y += speed_y;
	eggman_scene_move();
}

void eggman_scene_move(void)
{
	uint8_t bucket = 0;
	uint32_t bucket_2 = 0, deleted = 0, ignored = 0;
	uint16_t direction = 0;

	move_object_projected(&ignored, &ignored, &ignored, &ignored);
	fall_to_floor(&ignored, &ignored, &ignored);
	bucket = 0u;
	queue_sprite_in_bucket(bucket);
	delete_if_off_screen(&deleted, &ignored, &ignored);
	if ((uint8_t)deleted != 0u)
	{
		scroll_mode = 7u;
		stage_trigger_bits = 0u;
		return;
	}
	current_object = LINKED(current_object, OBJECT(current_object)->child);
	direction = OBJECT(current_object)->direction;
	OBJECT(current_object)->direction = direction - 1u;
	if (direction == 0u)
	{
		OBJECT(current_object)->direction = 1u;
		bucket_2 = place_sprite_at_cursor_floor();
		OBJECT(current_object)->screen_y += 4u;
		queue_sprite_in_bucket(bucket_2);
	}
}

void button_box_init(void)
{
	OBJECT(current_object)->button_box.step_timer = 0xB4u;
	OBJECT(current_object)->sprite_frame = 0x8928u;
	OBJECT(current_object)->sprite_palette = 0x11D0u;
	OBJECT(current_object)->handler = button_box_wait_ADDRESS;
}

void button_box_wait(void)
{
	if ((uint8_t)OBJECT(LINKED(current_object, OBJECT(current_object)->parent))->eggman_scene.step != 1u)
	{
		draw_button_box();
		return;
	}
	OBJECT(current_object)->handler = button_box_rise_ADDRESS;
	button_box_rise();
}

void button_box_rise(void)
{
	uint32_t box_frame = 0, step = 0;
	uint16_t step_timer = 0;

	if (OBJECT(current_object)->button_box.step != 0x13u)
	{
		step_timer = OBJECT(current_object)->button_box.step_timer;
		OBJECT(current_object)->button_box.step_timer = step_timer - 1u;
		if (step_timer != 0u)
		{
			draw_button_box();
			return;
		}
		step = OBJECT(current_object)->button_box.step;
		box_frame = button_box_steps[step];
		OBJECT(current_object)->button_box.step_timer = button_box_durations[step];
		OBJECT(current_object)->sprite_frame = button_box_frames[box_frame];
		OBJECT(current_object)->button_box.step++;
		draw_button_box();
		return;
	}
	if (OBJECT(LINKED(current_object, OBJECT(current_object)->parent))->eggman_scene.phase != 2u)
	{
		draw_button_box();
		return;
	}
	OBJECT(current_object)->handler = button_box_press_ADDRESS;
	button_box_press();
}

void button_box_press(void)
{
	uint32_t box_frame = 0, step = 0;
	uint16_t step_timer = 0;

	if (OBJECT(current_object)->button_box.step == 0x23u)
	{
		OBJECT(current_object)->handler = draw_button_box_ADDRESS;
		draw_button_box();
		return;
	}
	step_timer = OBJECT(current_object)->button_box.step_timer;
	OBJECT(current_object)->button_box.step_timer = step_timer - 1u;
	if (step_timer != 0u)
	{
		draw_button_box();
		return;
	}
	step = OBJECT(current_object)->button_box.step;
	box_frame = button_box_steps[step];
	OBJECT(current_object)->button_box.step_timer = button_box_durations[step];
	OBJECT(current_object)->sprite_frame = button_box_frames[box_frame];
	OBJECT(current_object)->button_box.step++;
	draw_button_box();
}

void draw_button_box(void)
{
	uint8_t bucket = 0;
	uint32_t ignored = 0;

	unproject_object(&ignored, &ignored, &ignored, &ignored);
	delete_if_off_screen(&ignored, &ignored, &ignored);
	bucket = 0xFFu;
	queue_sprite_in_bucket(bucket);
}

void sliding_door_init(void)
{
	OBJECT(current_object)->sprite_flags |= 0x4000u;
	OBJECT(current_object)->sprite_flags = 0x5000u;
	OBJECT(current_object)->sprite_frame = 0x7FFu;
	OBJECT(current_object)->sprite_palette = 0u;
	OBJECT(current_object)->sprite_scale = 0x7FFu;
	OBJECT(current_object)->flags |= 2u;
	OBJECT(current_object)->hitbox_size_x = 0xE6u;
	OBJECT(current_object)->hitbox_size_y = 0x21u;
	OBJECT(current_object)->hitbox_offset_x = 9;
	OBJECT(current_object)->hitbox_offset_y = -0xA;
	OBJECT(current_object)->hitbox_height = 0x100u;
	OBJECT(current_object)->kind = 0xCu;
	OBJECT(current_object)->sliding_door.open_delay = 0xAu;
	OBJECT(current_object)->handler = sliding_door_wait_ADDRESS;
	current_object = LINKED(current_object, OBJECT(current_object)->child);
	OBJECT(current_object)->sprite_flags = 6u;
	OBJECT(current_object)->sprite_frame = 0x7830u;
	OBJECT(current_object)->sprite_palette = 0x9070u;
	current_object = LINKED(current_object, OBJECT(current_object)->child);
	OBJECT(current_object)->sprite_flags = 0x5000u;
	OBJECT(current_object)->sprite_frame = 0x7FFu;
}

void sliding_door_wait(void)
{
	uint8_t done_players = 0, ready_players = 0;
	uint32_t player_base = 0, players_left = 0, status_cursor = 0;
	uint8_t status = 0;
	uint16_t open_delay = 0;

	status_cursor = player_status_ADDRESS;
	player_base = PLAYER_BLOCK(1);
	players_left = 3u;
	ready_players = 0u;
	done_players = 0u;
	do
	{
		status = mem_read8(status_cursor);
		status_cursor++;
		if ((int8_t)status >= 0)
			ready_players++;
		else if (PLAYER(player_base)->stage_b_path_done != 0u)
		{
			done_players++;
			ready_players++;
		}
		player_base += 0x200u;
		players_left--;
	} while (players_left != 0u);
	if (ready_players == 3u && done_players != 0u)
	{
		open_delay = OBJECT(current_object)->sliding_door.open_delay;
		OBJECT(current_object)->sliding_door.open_delay = open_delay - 1u;
		if (open_delay == 0u)
		{
			OBJECT(current_object)->handler = sliding_door_open_ADDRESS;
			sliding_door_open();
			return;
		}
	}
	sliding_door_draw();
}

void sliding_door_open(void)
{
	uint32_t direction = 0, speed = 0, speed_x = 0, speed_y = 0;

	direction = 0xB4u;
	speed = 0x100u;
	direction_to_speed(speed, &direction, &speed_x, &speed_y);
	OBJECT(current_object)->speed_x = speed_x;
	OBJECT(current_object)->speed_y = speed_y;
	if (OBJECT(current_object)->projected_x < 0x3EF)
	{
		sliding_door_draw();
		return;
	}
	OBJECT(current_object)->speed_x = 0;
	OBJECT(current_object)->speed_y = 0;
	sliding_door_opened = 0xFFu;
	OBJECT(current_object)->handler = sliding_door_draw_ADDRESS;
	sliding_door_draw();
}

void sliding_door_draw(void)
{
	uint16_t offset_x = 0, offset_y = 0;
	uint32_t deleted = 0, depth = 0, ignored = 0;

	move_object_projected(&ignored, &ignored, &ignored, &ignored);
	delete_if_off_screen(&deleted, &ignored, &ignored);
	if ((uint8_t)deleted != 0u)
		return;
	depth = (uint16_t)(OBJECT(current_object)->screen_y - 0x50u);
	queue_sprite_at_depth(&ignored, &depth);
	add_to_touchable_list(ignored);
	current_object = LINKED(current_object, OBJECT(current_object)->child);
	offset_x = 0u;
	offset_y = 0u;
	place_sprite_near_cursor(offset_x, offset_y);
	depth = (uint16_t)(OBJECT(object_cursor)->screen_y - 0x50u);
	queue_sprite_at_depth(&ignored, &depth);
	current_object = LINKED(current_object, OBJECT(current_object)->child);
	OBJECT(current_object)->sprite_palette = OBJECT(object_cursor)->screen_x;
	OBJECT(current_object)->sprite_scale = 0x3FFu - OBJECT(object_cursor)->projected_x + OBJECT(object_cursor)->screen_x;
	depth = (uint16_t)(OBJECT(object_cursor)->screen_y - 0x50u);
	queue_sprite_at_depth(&ignored, &depth);
}

void eggman_speech_start(void)
{
	OBJECT(current_object)->eggman_speech.frame_count = 0;
	OBJECT(current_object)->eggman_speech.script = eggman_speech_scripts[OBJECT(current_object)->sprite_frame] + eggman_speech_scripts_ADDRESS;
	OBJECT(current_object)->handler = eggman_speech_run_ADDRESS;
	eggman_speech_run();
}

void eggman_speech_run(void)
{
	uint8_t sound_id = 0;
	uint16_t frames = 0, kind = 0, screen_x = 0, screen_y = 0;
	uint32_t r11 = 0, command = 0, free_count = 0, ignored = 0, script_cursor = 0;
	uint16_t delay = 0;

	if (current_game_mode == 0x11u)
		return;
	OBJECT(current_object)->eggman_speech.frame_count++;
	script_cursor = OBJECT(current_object)->eggman_speech.script;
	delay = ROM16(script_cursor);
	script_cursor += 2u;
	if (OBJECT(current_object)->eggman_speech.frame_count < (int16_t)delay)
		return;
	command = ROM16(script_cursor) * 2u;
	script_cursor += 2u;
	switch (command)
	{
	case 0x0u:
	case 0x8u:
		delete_current_object();
		return;
	case 0x2u:
		OBJECT(current_object)->eggman_speech.script = script_cursor;
		OBJECT(current_object)->eggman_speech.frame_count = 0;
		spawn_eggman_booth(command, 0u, 0u, &free_count, &r11);
		return;
	case 0x4u:
		break;
	case 0x6u:
		sound_id = (uint8_t)ROM16(script_cursor);
		OBJECT(current_object)->eggman_speech.script = script_cursor + 2u;
		OBJECT(current_object)->eggman_speech.frame_count = 0;
		play_sound(sound_id, &ignored, &ignored);
		return;
	case 0xEu:
		return;
	default:
		invalid_jump(0x85794u + command);
	}
	screen_x = ROM16(script_cursor);
	script_cursor += 2u;
	screen_y = ROM16(script_cursor);
	script_cursor += 2u;
	kind = ROM16(script_cursor);
	script_cursor += 2u;
	frames = ROM16(script_cursor);
	OBJECT(current_object)->eggman_speech.script = script_cursor + 2u;
	OBJECT(current_object)->eggman_speech.frame_count = 0;
	r11 = spawn_message_box(screen_x, screen_y, kind, frames, 0u);
}
