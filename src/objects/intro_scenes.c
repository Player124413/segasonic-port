/**
 *    Name:        objects/intro_scenes.c
 *    Description: the scenes that start a game: stage 7's how to play (the hands on the button and the trackball, the panel), stage 6's opening (the rushing ground, the giant machine head)
 *    Version:     0.1.0
 *    Created:     12-10-2026
 *    Modified:    15-10-2026
 *    Author:      Sebastian Toyos
 *
 *    SegaSonic The Hedgehog (arcade, Sega System 32): the game's V60 program decompiled into C by
 *    tools/decompile.py, with the names of symbols.txt.
 */
#include "game.h"

void how_to_play_button_hand_start(void)
{
	OBJECT(current_object)->sprite_flags = 0xD000u;
	OBJECT(current_object)->sprite_offset_x = 0;
	OBJECT(current_object)->sprite_offset_y = 0;
	OBJECT(current_object)->flags = 0u;
	OBJECT(current_object)->handler = how_to_play_button_hand_update_ADDRESS;
	OBJECT(current_object)->part_list = how_to_play_button_hand_parts_ADDRESS;
	OBJECT(current_object)->animation = how_to_play_button_hand_animation_ADDRESS;
	OBJECT(current_object)->animation_frame = 0u;
	OBJECT(current_object)->how_to_play_button_hand.next_press = how_to_play_button_presses_ADDRESS;
	OBJECT(current_object)->screen_x = 0x42;
	OBJECT(current_object)->screen_y = 0xB5;
	OBJECT(current_object)->handler = how_to_play_button_hand_update_ADDRESS;
}

void how_to_play_button_hand_update(void)
{
	uint16_t frames = 0, kind = 0, screen_x = 0, screen_y = 0;
	uint32_t depth = 0, ignored = 0;

	if (stage != 7u)
	{
		delete_current_object();
		return;
	}
	if ((uint16_t)game_mode_timer == 0xC8u)
	{
		screen_x = 0xA0u;
		screen_y = 0x7Cu;
		kind = 0xCu;
		frames = 0x12Cu;
		spawn_message_box(screen_x, screen_y, kind, frames, 0u);
	}
	if ((uint16_t)game_mode_timer == mem_read16(OBJECT(current_object)->how_to_play_button_hand.next_press))
	{
		OBJECT(current_object)->animation_frame = (uint8_t)mem_read16(OBJECT(current_object)->how_to_play_button_hand.next_press + 2u);
		OBJECT(current_object)->how_to_play_button_hand.next_press += 4u;
		animate_part_list_step();
	}
	depth = 0xEEu;
	queue_sprite_at_depth(&ignored, &depth);
}

void how_to_play_trackball_hand_start(void)
{
	uint32_t free_count = 0, ignored = 0, object = 0;

	OBJECT(current_object)->sprite_flags = 0x1000u;
	OBJECT(current_object)->sprite_frame = 0x71C8u;
	OBJECT(current_object)->sprite_palette = 0x8530u;
	OBJECT(current_object)->sprite_offset_x = 0;
	OBJECT(current_object)->sprite_offset_y = 0;
	OBJECT(current_object)->flags = 0u;
	OBJECT(current_object)->handler = how_to_play_trackball_hand_update_ADDRESS;
	OBJECT(current_object)->how_to_play_trackball_hand.step = 0u;
	OBJECT(current_object)->screen_x = 0xFB;
	OBJECT(current_object)->screen_y = 0xB3;
	OBJECT(current_object)->projected_x = OBJECT(current_object)->screen_x;
	OBJECT(current_object)->handler = how_to_play_trackball_hand_update_ADDRESS;
	OBJECT(current_object)->animation = how_to_play_trackball_animation_ADDRESS;
	OBJECT(current_object)->animation_frame = 0u;
	OBJECT(current_object)->animation_speed = 4u;
	OBJECT(current_object)->animation_timer = 0;
	OBJECT(current_object)->child = 0u;
	allocate_object_2(&free_count, &ignored, &object);
	if (free_count != 0u)
	{
		OBJECT(object)->sprite_flags = 0x1000u;
		OBJECT(object)->sprite_frame = 0x71C0u;
		OBJECT(object)->sprite_palette = 0x8550u;
		OBJECT(object)->screen_x = OBJECT(current_object)->screen_x;
		OBJECT(object)->screen_x -= 0x38u;
		OBJECT(object)->screen_y = OBJECT(current_object)->screen_y;
		OBJECT(object)->screen_y -= 8u;
		OBJECT(current_object)->child = object;
	}
}

void how_to_play_trackball_hand_update(void)
{
	uint8_t last_step = 0;
	uint16_t child = 0, child_palette = 0, frames = 0, kind = 0, screen_x = 0, screen_y = 0;
	uint32_t box = 0, depth = 0, ignored = 0, next_step = 0, step_list = 0;
	uint8_t pause_timer = 0, animation_timer = 0;

	if (stage == 7u)
	{
		if (game_mode_timer >= 0x32)
		{
			switch (OBJECT(current_object)->how_to_play_trackball_hand.step)
			{
			case 0:
			case 3:
				OBJECT(current_object)->screen_x -= 0xAu;
				OBJECT(current_object)->screen_y += 0xAu;
				if ((int16_t)(OBJECT(current_object)->screen_x - OBJECT(current_object)->projected_x) <= -0x20)
				{
					OBJECT(current_object)->how_to_play_trackball_hand.step++;
					OBJECT(current_object)->how_to_play_trackball_hand.pause_timer = 8;
				}
				break;
			case 1:
				pause_timer = OBJECT(current_object)->how_to_play_trackball_hand.pause_timer;
				OBJECT(current_object)->how_to_play_trackball_hand.pause_timer = pause_timer - 1u;
				if ((int8_t)pause_timer <= 1)
					OBJECT(current_object)->how_to_play_trackball_hand.step++;
				break;
			case 2:
				OBJECT(current_object)->screen_x += 4u;
				OBJECT(current_object)->screen_y -= 4u;
				if (OBJECT(current_object)->projected_x <= OBJECT(current_object)->screen_x)
					OBJECT(current_object)->how_to_play_trackball_hand.step = 0u;
				break;
			default:
				unknown_code(0x83E6Du + OBJECT(current_object)->how_to_play_trackball_hand.step * 2u);
			}
		}
		if ((uint16_t)game_mode_timer == 0xAu)
			OBJECT(current_object)->sprite_palette = 0x8760u;
		else if ((uint16_t)game_mode_timer == 0xBEu)
			OBJECT(current_object)->sprite_palette = 0x8530u;
		if ((uint16_t)game_mode_timer == 0xAu)
		{
			screen_x = 0x40u;
			screen_y = 0x30u;
			kind = 0xDu;
			frames = 0x1E0u;
			box = spawn_message_box(screen_x, screen_y, kind, frames, 0u);
			screen_x = 0xA0u;
			screen_y = 0x7Cu;
			kind = 0xBu;
			frames = 0xB4u;
			spawn_message_box(screen_x, screen_y, kind, frames, box);
		}
		if (game_mode_timer >= 0x20)
		{
			scroll_mode = 0u;
			AS8(scroll_speed) = 0x10u;
		}
		depth = 0xEEu;
		queue_sprite_at_depth(&ignored, &depth);
		current_object = LINKED(current_object, OBJECT(current_object)->child);
		if ((uint16_t)current_object == 0u)
			return;
		if (game_mode_timer >= 0x32)
		{
			animation_timer = OBJECT(object_cursor)->animation_timer;
			OBJECT(object_cursor)->animation_timer = animation_timer - 1u;
			if ((int8_t)animation_timer <= 1)
			{
				OBJECT(object_cursor)->animation_timer = OBJECT(object_cursor)->animation_speed;
				step_list = OBJECT(object_cursor)->animation;
				last_step = (uint8_t)mem_read16(step_list);
				next_step = OBJECT(object_cursor)->animation_frame;
				next_step = (uint8_t)(next_step + 1u);
				step_list += 2u;
				if ((int8_t)last_step < (int8_t)next_step)
					next_step = 0u;
				OBJECT(object_cursor)->animation_frame = next_step;
				OBJECT(current_object)->sprite_frame = mem_read16(step_list + next_step * 2u);
			}
		}
		child_palette = 0x8750u;
		if (OBJECT(object_cursor)->sprite_palette != 0x8760u)
			child_palette = 0x8550u;
		OBJECT(current_object)->sprite_palette = child_palette;
		depth = 0xEDu;
		queue_sprite_at_depth(&ignored, &depth);
		return;
	}
	if (OBJECT(current_object)->child != 0u)
	{
		child = OBJECT(current_object)->child;
		free_object_2(child);
	}
	delete_current_object();
}

void how_to_play_panel_start(void)
{
	OBJECT(current_object)->sprite_flags = 0x100Au;
	OBJECT(current_object)->sprite_frame = 0x71F0u;
	OBJECT(current_object)->sprite_palette = 0x8780u;
	OBJECT(current_object)->screen_x = 0;
	OBJECT(current_object)->screen_y = 0x88;
	OBJECT(current_object)->handler = how_to_play_panel_show_ADDRESS;
	how_to_play_panel_show();
}

void how_to_play_panel_show(void)
{
	uint32_t depth = 0, ignored = 0;

	if (stage == 7u)
	{
		depth = 0xECu;
		queue_sprite_at_depth(&ignored, &depth);
		return;
	}
	delete_current_object();
}

void opening_ground_start(void)
{
	OBJECT(current_object)->sprite_flags = 0x1000u;
	OBJECT(current_object)->sprite_palette = 0x9120u;
	OBJECT(current_object)->flags = 0u;
	OBJECT(current_object)->speed_y = 0;
	OBJECT(current_object)->handler = opening_ground_run_ADDRESS;
	OBJECT(current_object)->x = 0xA00000;
	OBJECT(current_object)->y = 0xB80000;
	OBJECT(current_object)->animation_frame = 0u;
	OBJECT(current_object)->animation = opening_ground_animation_ADDRESS;
	OBJECT(current_object)->animation_timer = 0;
	OBJECT(current_object)->animation_speed = 1u;
}

void opening_ground_run(void)
{
	uint16_t shake_dy = 0;
	uint32_t r7 = 0, r8 = 0, depth = 0, frame = 0, ignored = 0;

	if ((int8_t)scene_step >= 2)
	{
		delete_current_object();
		return;
	}
	OBJECT(current_object)->y += OBJECT(current_object)->speed_y;
	OBJECT(current_object)->screen_x = OBJECT(current_object)->x_int;
	OBJECT(current_object)->screen_y = OBJECT(current_object)->y_int;
	shake_dy = AT8(stage_06_shake, 1);
	if (stage_06_shake_sign < 0)
		shake_dy = (uint16_t)(0u - shake_dy);
	OBJECT(current_object)->screen_y += shake_dy;
	animate(&frame, &r7, &r8);
	depth = 0x20u;
	queue_sprite_at_depth(&ignored, &depth);
}

void opening_machine_start(void)
{
	OBJECT(current_object)->opening_machine.scene_step_seen = 0xFFu;
	OBJECT(current_object)->handler = opening_machine_update_ADDRESS;
	opening_machine_update();
}

void opening_machine_update(void)
{
	uint16_t shake_dy = 0;
	uint32_t depth = 0, free_count = 0, ignored = 0, object = 0, scale = 0;

	switch (scene_step)
	{
	case 0:
	case 1:
	case 4:
		if (OBJECT(current_object)->opening_machine.scene_step_seen != scene_step)
		{
			OBJECT(current_object)->opening_machine.scene_step_seen = scene_step;
			OBJECT(current_object)->sprite_flags = 0xC000u;
			OBJECT(current_object)->opening_machine.zoom = 0xC00000;
			OBJECT(current_object)->sprite_scale = OBJECT(current_object)->opening_machine.zoom_int;
			OBJECT(current_object)->opening_machine.zoom_speed = 0x8000;
			OBJECT(current_object)->flags = 0u;
			OBJECT(current_object)->opening_machine.phase = 0;
			OBJECT(current_object)->speed_y = -0xC0000;
			OBJECT(current_object)->x = 0xA00000;
			OBJECT(current_object)->y = 0xF80000;
			OBJECT(current_object)->animation_frame = 0u;
			OBJECT(current_object)->animation = opening_machine_leap_animation_ADDRESS;
			OBJECT(current_object)->animation_timer = 0;
			OBJECT(current_object)->animation_speed = 2u;
			opening_machine_object = current_object;
		}
		OBJECT(current_object)->opening_machine.draw_depth = OBJECT(current_object)->screen_y;
		switch ((uint8_t)OBJECT(current_object)->opening_machine.phase)
		{
		case 0:
			opening_machine_leap_in();
			break;
		case 1:
			opening_machine_approach();
			break;
		case 2:
			opening_machine_lunge();
			break;
		case 3:
			opening_machine_show_mouth();
			break;
		case 4:
			opening_machine_engulf();
			break;
		case 5:
			opening_machine_hold();
			break;
		case 6:
			opening_machine_leap_in();
			break;
		default:
			unknown_code(0x84157u + (uint8_t)OBJECT(current_object)->opening_machine.phase * 3u);
		}
		OBJECT(current_object)->screen_x = OBJECT(current_object)->x_int;
		OBJECT(current_object)->screen_y = OBJECT(current_object)->y_int;
		shake_dy = AT8(stage_06_shake, 1);
		if (stage_06_shake_sign < 0)
			shake_dy = (uint16_t)(0u - shake_dy);
		OBJECT(current_object)->screen_y += shake_dy;
		if (OBJECT(current_object)->opening_machine.phase < 3)
			animate_part_list();
		depth = OBJECT(current_object)->opening_machine.draw_depth;
		queue_sprite_at_depth(&ignored, &depth);
		return;
	case 2:
	case 3:
		if (OBJECT(current_object)->opening_machine.scene_step_seen != scene_step)
		{
			OBJECT(current_object)->opening_machine.scene_step_seen = scene_step;
			if (OBJECT(current_object)->opening_machine.scene_step_seen == 2u)
			{
				OBJECT(current_object)->sprite_flags = 0xC000u;
				OBJECT(current_object)->opening_machine.zoom = 0;
				OBJECT(current_object)->flags = 0u;
				OBJECT(current_object)->y = -0x600000;
				OBJECT(current_object)->animation_frame = 0u;
				OBJECT(current_object)->animation = opening_machine_fly_away_animation_ADDRESS;
				OBJECT(current_object)->animation_timer = 0;
				OBJECT(current_object)->animation_speed = 2u;
				OBJECT(current_object)->child = 0xFFFFu;
				allocate_object_2(&free_count, &ignored, &object);
				if (free_count != 0u)
				{
					OBJECT(object)->sprite_flags = 0u;
					OBJECT(object)->sprite_frame = 0x7078u;
					OBJECT(object)->sprite_palette = 0x87A0u;
					OBJECT(object)->sprite_width = 0;
					OBJECT(object)->sprite_height = 0x18;
					OBJECT(current_object)->child = object;
				}
			}
		}
		OBJECT(current_object)->opening_machine.zoom += 2u;
		OBJECT(current_object)->sprite_scale = divide_s32(0x4000u, OBJECT(current_object)->opening_machine.zoom + 0x10u);
		OBJECT(current_object)->y += 0x10000u;
		scale = OBJECT(current_object)->sprite_scale;
		OBJECT(current_object)->screen_y = ((int32_t)(OBJECT(current_object)->y_int * scale) >> 10) + 0xB8u;
		animate_part_list();
		queue_sprite(scale);
		draw_opening_machine_shadow();
		return;
	default:
		unknown_code(0x84089u + scene_step * 3u);
	}
}

void opening_machine_leap_in(void)
{
	uint8_t sound_id = 0;
	uint32_t write_index = 0, write_index_address = 0;

	if (OBJECT(current_object)->speed_y_int < 0)
		OBJECT(current_object)->opening_machine.draw_depth = 0x10u;
	if (scene_timer >= 0x3C)
	{
		OBJECT(current_object)->speed_y += 0x6000u;
		OBJECT(current_object)->y += OBJECT(current_object)->speed_y;
		if (OBJECT(current_object)->speed_y_int >= 0 && OBJECT(current_object)->y_int >= 0x63)
		{
			OBJECT(current_object)->y = 0x630000;
			stage_06_shake = 0x800u;
			stage_06_shake_sign = 0;
			OBJECT(current_object)->animation_frame = 0u;
			OBJECT(current_object)->animation = opening_machine_landed_animation_ADDRESS;
			OBJECT(current_object)->animation_timer = 0;
			OBJECT(current_object)->animation_speed = 2u;
			sound_id = 0xAu;
			play_sound(sound_id, &write_index, &write_index_address);
			OBJECT(current_object)->opening_machine.phase++;
		}
	}
}

void opening_machine_approach(void)
{
	uint16_t shake_before = 0;

	if (stage_06_shake != 0u)
	{
		shake_before = stage_06_shake;
		stage_06_shake = shake_before - 0x80u;
		if ((int16_t)shake_before < 0x80)
			stage_06_shake = 0u;
		stage_06_shake_sign = ~stage_06_shake_sign;
	}
	OBJECT(current_object)->opening_machine.zoom += OBJECT(current_object)->opening_machine.zoom_speed;
	OBJECT(current_object)->sprite_scale = OBJECT(current_object)->opening_machine.zoom_int;
	if (scene_timer > 0xC0)
		OBJECT(current_object)->opening_machine.phase++;
}

void opening_machine_lunge(void)
{
	OBJECT(current_object)->opening_machine.zoom_speed += 0x1000u;
	OBJECT(current_object)->opening_machine.zoom += OBJECT(current_object)->opening_machine.zoom_speed;
	OBJECT(current_object)->sprite_scale = OBJECT(current_object)->opening_machine.zoom_int;
	if ((int16_t)OBJECT(current_object)->sprite_scale >= 0x1B3)
		OBJECT(current_object)->y -= 0xE000u;
	if (OBJECT(current_object)->opening_machine.zoom_int >= 0x32B)
		OBJECT(current_object)->opening_machine.phase++;
}

void opening_machine_show_mouth(void)
{
	OBJECT(current_object)->opening_machine.phase++;
	OBJECT(current_object)->sprite_flags = 0u;
	OBJECT(current_object)->sprite_frame = 0x7198u;
	OBJECT(current_object)->sprite_palette = 0x8510u;
	OBJECT(current_object)->y += (OBJECT(current_object)->sprite_scale * 0x1Eu) << 8;
	opening_machine_engulf();
}

void opening_machine_engulf(void)
{
	OBJECT(current_object)->opening_machine.zoom_speed += 0x10000u;
	OBJECT(current_object)->opening_machine.zoom += OBJECT(current_object)->opening_machine.zoom_speed;
	OBJECT(current_object)->sprite_scale = OBJECT(current_object)->opening_machine.zoom_int;
	if (OBJECT(current_object)->opening_machine.zoom_int < 0x5B0)
	{
		opening_machine_hold();
		return;
	}
	OBJECT(current_object)->opening_machine.phase++;
	OBJECT(current_object)->sprite_palette = 0x87A0u;
	opening_machine_hold();
}

void opening_machine_hold(void)
{
}

void draw_opening_machine_shadow(void)
{
	uint16_t scaled_dy = 0;
	uint32_t depth = 0, ignored = 0, sprite_scale = 0;

	current_object = LINKED(current_object, OBJECT(current_object)->child);
	sprite_scale = OBJECT(object_cursor)->sprite_scale;
	OBJECT(current_object)->screen_x = ((int32_t)(OBJECT(current_object)->sprite_width * sprite_scale) >> 8) + OBJECT(object_cursor)->screen_x;
	scaled_dy = (uint16_t)((int32_t)(OBJECT(current_object)->sprite_height * sprite_scale) >> 8);
	OBJECT(current_object)->screen_y = OBJECT(object_cursor)->screen_y;
	OBJECT(current_object)->screen_y += scaled_dy;
	OBJECT(current_object)->sprite_scale = OBJECT(object_cursor)->sprite_scale;
	depth = (uint16_t)(OBJECT(object_cursor)->screen_y - 0x10u);
	queue_sprite_at_depth(&ignored, &depth);
}

void draw_opening_machine_shadow_spare_return(void)
{
}
