/**
 *    Name:        objects/eggman_scene.c
 *    Description: object types 31 and 32 (stage B): Eggman's scene with the button box, the sliding door
 *    Version:     0.1.0
 *    Created:     12-10-2026
 *    Modified:    15-10-2026
 *    Author:      Sebastian Toyos
 *
 *    SegaSonic The Hedgehog (arcade, Sega System 32): the game's V60 program decompiled into C by
 *    tools/decompile.py, with the names of symbols.txt.
 */
#include "game.h"

void eggman_scene_init(void)
{
	uint32_t block = 0, free_count = 0, object = 0, piece_count = 0, spawned = 0;

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
	spawn_object_with_pieces(0x01409Cu, &spawned, &piece_count, &free_count, &object, &block);
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
	uint32_t direction = 0, write_index = 0, write_index_address = 0;
	uint16_t step_timer = 0, talk_timer = 0;

	step_timer = OBJECT(current_object)->eggman_scene.step_timer;
	OBJECT(current_object)->eggman_scene.step_timer = step_timer - 1u;
	if (step_timer == 0u)
	{
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
		play_sound(sound_id, &write_index, &write_index_address);
		OBJECT(current_object)->direction++;
	}
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
	uint32_t r1 = 0, r2 = 0, bucket_2 = 0, cursor = 0, deleted = 0, ignored = 0, slope_height = 0;
	uint16_t direction = 0;

	move_object_projected(&ignored, &r2, &slope_height, &cursor);
	fall_to_floor(&ignored, &r1, &r2);
	bucket = 0u;
	queue_sprite_in_bucket(bucket);
	delete_if_off_screen(&deleted, &r2, &slope_height);
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
	if (OBJECT(LINKED(current_object, OBJECT(current_object)->parent))->var_80 != 1u)
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
	if (OBJECT(LINKED(current_object, OBJECT(current_object)->parent))->var_81 != 2u)
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
	uint32_t block_offset = 0, cursor = 0, ignored = 0, slope_height = 0;

	unproject_object(&ignored, &block_offset, &slope_height, &cursor);
	delete_if_off_screen(&ignored, &block_offset, &slope_height);
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
		{
		sliding_door_wait_count_ready:
			ready_players++;
		}
		else if (mem_read8(player_base + 0x140u) != 0u)
		{
			done_players++;
			goto sliding_door_wait_count_ready;
		}
		player_base += 0x200u;
		players_left--;
	} while (players_left != 0u);
	if (ready_players != 3u || done_players == 0u)
	{
		sliding_door_draw();
		return;
	}
	open_delay = OBJECT(current_object)->sliding_door.open_delay;
	OBJECT(current_object)->sliding_door.open_delay = open_delay - 1u;
	if (open_delay != 0u)
	{
		sliding_door_draw();
		return;
	}
	OBJECT(current_object)->handler = sliding_door_open_ADDRESS;
	sliding_door_open();
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
	uint32_t block_offset = 0, cursor = 0, deleted = 0, depth = 0, ignored = 0, slope_height = 0;

	move_object_projected(&ignored, &block_offset, &slope_height, &cursor);
	delete_if_off_screen(&deleted, &block_offset, &slope_height);
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
