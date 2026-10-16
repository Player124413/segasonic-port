/**
 *    Name:        scenes/ring_tally.c
 *    Description: the ring tally after a stage (game mode 0x1D): the total and the players' panels
 *    Version:     0.1.0
 *    Created:     12-10-2026
 *    Modified:    16-10-2026
 *    Author:      Sebastian Toyos
 *
 *    SegaSonic The Hedgehog (arcade, Sega System 32): the game's V60 program decompiled into C by
 *    tools/decompile.py, with the names of symbols.txt.
 */
#include "game.h"

void start_ring_tally(uint32_t *p_stage_rings_3, uint32_t *p_player_3_out_flags, uint32_t *p_ring_total, uint32_t *p_p2_object, uint32_t *p_p3_object, uint32_t *p_object)
{
	uint16_t bonus_percent = 0;
	uint32_t ignored = 0, object = 0, object_2 = 0, object_3 = 0, object_4 = 0, percent_fixed = 0, player = 0, player_1 = 0, player_3 = 0, player_3_out_flags = 0, ring_total = 0, stage_index = 0, stage_rings_1 = 0, stage_rings_2 = 0, stage_rings_3 = 0;
	uint32_t out_flags = 0;

	stage_results_done = 0u;
	ring_tally_panels_counting = 0u;
	ring_tally_counted = 0u;
	player_1 = PLAYER_BLOCK(1);
	object = PLAYER_BLOCK(2);
	player_3 = PLAYER_BLOCK(3);
	object_2 = LINKED(WORK_RAM, player_1_object);
	object_3 = LINKED(WORK_RAM, player_2_object);
	object_4 = LINKED(WORK_RAM, player_3_object);
	stage_rings_1 = player_1_stage_rings;
	stage_rings_2 = player_2_stage_rings;
	stage_rings_3 = player_3_stage_rings;
	stage_index = stage;
	bonus_percent = stage_ring_bonus_percents[stage_index];
	ring_total = stage_ring_counts[stage_index];
	percent_fixed = divide_s32((uint32_t)(uint16_t)(stage_rings_1 + stage_rings_2 + stage_rings_3) << 16, ring_total) * 0x64u;
	if ((percent_fixed & 0x800u) != 0u)
		percent_fixed += 0x100u;
	percent_fixed = percent_fixed >> 16;
	ring_tally_percent = percent_fixed;
	stage_rings_1 = stage_rings_1 | 0x100u;
	stage_rings_2 = stage_rings_2 | 0x100u;
	stage_rings_3 = stage_rings_3 | 0x100u;
	player = object;
	pick_ring_bonus_players(&stage_rings_1, &stage_rings_2, &stage_rings_3);
	if ((int16_t)percent_fixed < (int16_t)bonus_percent)
	{
		stage_rings_1 = stage_rings_1 & 0xFFu;
		stage_rings_2 = stage_rings_2 & 0xFFu;
		stage_rings_3 = stage_rings_3 & 0xFFu;
	}
	if ((OBJECT(object_2)->flags & 0x10000400u) == 0u)
	{
		ring_total = (uint16_t)stage_rings_1;
		allocate_object_outside_loop(&ignored, &object);
		OBJECT(object)->handler = ring_tally_player_1_update_ADDRESS;
		OBJECT(object)->ring_tally_player.started = 0u;
		OBJECT(object)->hitbox_height = (uint8_t)PLAYER(player_1)->stage_rings;
		ring_total = ring_total >> 8;
		OBJECT(object)->tally_gets_bonus = ring_total;
		OBJECT(object)->step = 1u;
		OBJECT(object)->ring_tally_player.character = OBJECT(object_2)->kind;
		ring_tally_panels_counting++;
		OBJECT(object)->speed_x = 0x208400;
	}
	if ((OBJECT(object_3)->flags & 0x10000400u) == 0u)
	{
		allocate_object_outside_loop(&ignored, &object);
		OBJECT(object)->handler = ring_tally_player_2_update_ADDRESS;
		OBJECT(object)->ring_tally_player.started = 0u;
		OBJECT(object)->hitbox_height = (uint8_t)PLAYER(player)->stage_rings;
		OBJECT(object)->tally_gets_bonus = (uint16_t)stage_rings_2 >> 8;
		OBJECT(object)->step = 2u;
		OBJECT(object)->ring_tally_player.character = OBJECT(object_3)->kind;
		ring_tally_panels_counting++;
		OBJECT(object)->speed_x = 0x208600;
	}
	out_flags = OBJECT(object_4)->flags & 0x10000400u;
	player_3_out_flags = out_flags;
	if (out_flags == 0u)
	{
		allocate_object_outside_loop(&ignored, &object);
		OBJECT(object)->handler = ring_tally_player_3_update_ADDRESS;
		OBJECT(object)->ring_tally_player.started = 0u;
		OBJECT(object)->hitbox_height = (uint8_t)PLAYER(player_3)->stage_rings;
		stage_rings_3 = (uint16_t)stage_rings_3 >> 8;
		OBJECT(object)->tally_gets_bonus = stage_rings_3;
		OBJECT(object)->step = 3u;
		OBJECT(object)->ring_tally_player.character = OBJECT(object_4)->kind;
		ring_tally_panels_counting++;
		OBJECT(object)->speed_x = 0x208800;
	}
	allocate_object_outside_loop(&ignored, &object);
	OBJECT(object)->handler = ring_tally_percent_update_ADDRESS;
	OBJECT(object)->ring_tally_percent_view.started = 0u;
	OBJECT(object)->hit_list_next = ring_tally_percent;
	allocate_object_outside_loop(&ignored, &object);
	OBJECT(object)->handler = ring_tally_ticker_update_ADDRESS;
	OBJECT(object)->ring_tally_ticker.started = 0u;
	allocate_object_outside_loop(&ignored, &object);
	OBJECT(object)->handler = ring_tally_end_sound_wait_ADDRESS;
	*p_stage_rings_3 = (*p_stage_rings_3 & 0xFFFF0000u) | (stage_rings_3 & 0xFFFFu);
	*p_player_3_out_flags = player_3_out_flags;
	*p_ring_total = ring_total;
	*p_p2_object = object_3;
	*p_p3_object = object_4;
	*p_object = object;
}

void pick_ring_bonus_players(uint32_t *p_player_1_rings, uint32_t *p_player_2_rings, uint32_t *p_player_3_rings)
{
	uint16_t player_1_rings = (uint16_t)*p_player_1_rings;
	uint16_t player_2_rings = (uint16_t)*p_player_2_rings;
	uint16_t player_3_rings = (uint16_t)*p_player_3_rings;

	if ((uint8_t)player_2_rings == (uint8_t)player_1_rings)
	{
		if ((uint8_t)player_3_rings == (uint8_t)player_1_rings)
			return;
		if ((int8_t)player_3_rings < (int8_t)player_1_rings)
		{
			player_3_rings &= 0xFFu;
			*p_player_3_rings = (*p_player_3_rings & 0xFFFF0000u) | player_3_rings;
			return;
		}
		player_1_rings &= 0xFFu;
		player_2_rings &= 0xFFu;
		*p_player_1_rings = (*p_player_1_rings & 0xFFFF0000u) | player_1_rings;
		*p_player_2_rings = (*p_player_2_rings & 0xFFFF0000u) | player_2_rings;
		return;
	}
	if ((uint8_t)player_3_rings == (uint8_t)player_2_rings)
	{
		if ((int8_t)player_2_rings < (int8_t)player_1_rings)
		{
			player_2_rings &= 0xFFu;
			player_3_rings &= 0xFFu;
			*p_player_2_rings = (*p_player_2_rings & 0xFFFF0000u) | player_2_rings;
			*p_player_3_rings = (*p_player_3_rings & 0xFFFF0000u) | player_3_rings;
			return;
		}
		player_1_rings &= 0xFFu;
		*p_player_1_rings = (*p_player_1_rings & 0xFFFF0000u) | player_1_rings;
		return;
	}
	if ((uint8_t)player_3_rings == (uint8_t)player_1_rings)
	{
		if ((int8_t)player_2_rings < (int8_t)player_1_rings)
		{
			player_2_rings &= 0xFFu;
			*p_player_2_rings = (*p_player_2_rings & 0xFFFF0000u) | player_2_rings;
			return;
		}
		player_1_rings &= 0xFFu;
		player_3_rings &= 0xFFu;
		*p_player_1_rings = (*p_player_1_rings & 0xFFFF0000u) | player_1_rings;
		*p_player_3_rings = (*p_player_3_rings & 0xFFFF0000u) | player_3_rings;
		return;
	}
	if ((int8_t)player_2_rings < (int8_t)player_1_rings)
	{
		if ((int8_t)player_3_rings < (int8_t)player_1_rings)
		{
			player_2_rings &= 0xFFu;
			player_3_rings &= 0xFFu;
			*p_player_2_rings = (*p_player_2_rings & 0xFFFF0000u) | player_2_rings;
			*p_player_3_rings = (*p_player_3_rings & 0xFFFF0000u) | player_3_rings;
			return;
		}
		player_1_rings &= 0xFFu;
		player_2_rings &= 0xFFu;
		*p_player_1_rings = (*p_player_1_rings & 0xFFFF0000u) | player_1_rings;
		*p_player_2_rings = (*p_player_2_rings & 0xFFFF0000u) | player_2_rings;
		return;
	}
	if ((int8_t)player_3_rings < (int8_t)player_2_rings)
	{
		player_1_rings &= 0xFFu;
		player_3_rings &= 0xFFu;
		*p_player_1_rings = (*p_player_1_rings & 0xFFFF0000u) | player_1_rings;
		*p_player_3_rings = (*p_player_3_rings & 0xFFFF0000u) | player_3_rings;
		return;
	}
	player_1_rings &= 0xFFu;
	player_2_rings &= 0xFFu;
	*p_player_1_rings = (*p_player_1_rings & 0xFFFF0000u) | player_1_rings;
	*p_player_2_rings = (*p_player_2_rings & 0xFFFF0000u) | player_2_rings;
}

void ring_tally_percent_update(void)
{
	uint32_t digit = 0, ignored = 0, object = 0, percent_bcd = 0, percent_digit = 0, shown_percent = 0;

	if (OBJECT(current_object)->ring_tally_percent_view.started != 0u)
	{
		if (stage_results_done != 0u)
		{
			delete_current_object();
			return;
		}
		queue_sprite_in_layer();
		current_object = OBJECT(object_cursor)->speed_y;
		queue_sprite_in_layer();
		if (ring_tally_counted != 0u)
		{
			shown_percent = OBJECT(object_cursor)->hit_list_next;
			update_ring_tally_percent();
			goto ring_tally_percent_set_digits;
		}
		OBJECT(object_cursor)->ring_tally_percent_view.roll_delay--;
		if (OBJECT(object_cursor)->ring_tally_percent_view.roll_delay == 0u)
		{
			OBJECT(object_cursor)->ring_tally_percent_view.roll_delay = 1u;
			next_random(&shown_percent, &ignored, &ignored);
		ring_tally_percent_set_digits:
			percent_bcd = shown_percent & 0xFFu;
			convert_to_bcd(&percent_bcd, &digit);
			current_object = OBJECT(object_cursor)->z;
			percent_bcd &= 0x1FFu;
			percent_digit = (digit & ~0xFFFFu) | percent_bcd;
			percent_digit = (percent_digit & ~0xFFFFu) | (percent_digit & 0xFu);
			OBJECT(current_object)->sprite_frame = percent_digit_sprites[percent_digit];
			current_object = OBJECT(object_cursor)->ring_tally_percent_view.tens_digit;
			percent_digit = (percent_digit & ~0xFFFFu) | percent_bcd;
			percent_digit = (percent_digit & ~0xFFFFu) | (percent_digit & 0xF0u);
			percent_digit = (percent_digit & ~0xFFFFu) | ((uint16_t)percent_digit >> 4);
			OBJECT(current_object)->sprite_frame = percent_digit_sprites[percent_digit];
			current_object = OBJECT(object_cursor)->animation;
			percent_digit = (percent_digit & ~0xFFFFu) | percent_bcd;
			percent_digit = (percent_digit & ~0xFFFFu) | (percent_digit & 0xF00u);
			OBJECT(current_object)->sprite_frame = percent_digit_sprites[(percent_digit & ~0xFFFFu) | ((uint16_t)percent_digit >> 8)];
		}
		current_object = OBJECT(object_cursor)->z;
		queue_sprite_in_layer();
		current_object = OBJECT(object_cursor)->ring_tally_percent_view.tens_digit;
		queue_sprite_in_layer();
		current_object = OBJECT(object_cursor)->animation;
		if (OBJECT(current_object)->sprite_frame == 0x9DF0u)
			queue_sprite_in_layer();
		return;
	}
	OBJECT(current_object)->ring_tally_percent_view.started = 1u;
	OBJECT(current_object)->sprite_frame = 0x9E38u;
	OBJECT(current_object)->sprite_palette = 0xC00u;
	OBJECT(current_object)->sprite_palette |= 0x8000u;
	OBJECT(current_object)->sprite_scale = 0x100u;
	OBJECT(current_object)->screen_y = 0x10;
	OBJECT(current_object)->screen_x = 0xA0;
	OBJECT(current_object)->sprite_layer = 0u;
	OBJECT(current_object)->sprite_flags = 0u;
	allocate_object_2(&ignored, &ignored, &object);
	OBJECT(current_object)->speed_y = object;
	OBJECT(object)->sprite_frame = 0x9DE0u;
	OBJECT(object)->sprite_palette = 0xBA0u;
	OBJECT(object)->sprite_palette |= 0x8000u;
	OBJECT(object)->sprite_scale = 0x100u;
	OBJECT(object)->screen_y = 0x30;
	OBJECT(object)->screen_x = 0xA0;
	OBJECT(object)->sprite_layer = 1u;
	OBJECT(object)->sprite_flags = 0u;
	allocate_object_2(&ignored, &ignored, &object);
	OBJECT(current_object)->z = object;
	OBJECT(object)->sprite_palette = 0xBF0u;
	OBJECT(object)->sprite_palette |= 0x8000u;
	OBJECT(object)->sprite_scale = 0x100u;
	OBJECT(object)->screen_y = 0x31;
	OBJECT(object)->screen_x = 0x9E;
	OBJECT(object)->sprite_layer = 0u;
	OBJECT(object)->sprite_flags = 0u;
	allocate_object_2(&ignored, &ignored, &object);
	OBJECT(current_object)->ring_tally_percent_view.tens_digit = object;
	OBJECT(object)->sprite_palette = 0xBF0u;
	OBJECT(object)->sprite_palette |= 0x8000u;
	OBJECT(object)->sprite_scale = 0x100u;
	OBJECT(object)->screen_y = 0x31;
	OBJECT(object)->screen_x = 0x8E;
	OBJECT(object)->sprite_layer = 0u;
	OBJECT(object)->sprite_flags = 0u;
	allocate_object_2(&ignored, &ignored, &object);
	OBJECT(current_object)->animation = object;
	OBJECT(object)->sprite_palette = 0xBF0u;
	OBJECT(object)->sprite_palette |= 0x8000u;
	OBJECT(object)->sprite_scale = 0x100u;
	OBJECT(object)->screen_y = 0x31;
	OBJECT(object)->screen_x = 0x7E;
	OBJECT(object)->sprite_layer = 0u;
	OBJECT(object)->sprite_flags = 0u;
	OBJECT(current_object)->ring_tally_percent_view.roll_delay = 1u;
	OBJECT(current_object)->ring_tally_percent_view.blink_delay = 2u;
	OBJECT(current_object)->ring_tally_percent_view.blink_index = 0u;
	OBJECT(current_object)->ring_tally_percent_view.results_timer = 0x12Cu;
}

void update_ring_tally_percent(void)
{
	uint16_t results_timer = 0;

	results_timer = OBJECT(object_cursor)->update_ring_tally_percent.results_timer;
	OBJECT(object_cursor)->update_ring_tally_percent.results_timer = results_timer - 1u;
	if (results_timer == 1u)
		stage_results_done = 1u;
	if ((int16_t)stage_ring_bonus_percents[stage] <= (int16_t)OBJECT(object_cursor)->hit_list_next)
	{
		OBJECT(object_cursor)->update_ring_tally_percent.blink_delay--;
		if (OBJECT(object_cursor)->update_ring_tally_percent.blink_delay == 0u)
		{
			OBJECT(object_cursor)->update_ring_tally_percent.blink_delay = 2u;
			OBJECT(object_cursor)->update_ring_tally_percent.blink_index++;
			OBJECT(object_cursor)->update_ring_tally_percent.blink_index &= 3u;
			current_object = OBJECT(object_cursor)->speed_y;
			OBJECT(current_object)->sprite_palette = ring_tally_blink_palettes[OBJECT(object_cursor)->update_ring_tally_percent.blink_index];
			OBJECT(current_object)->sprite_palette |= 0x8000u;
		}
	}
}

void ring_tally_end_sound_wait(void)
{
	uint8_t sound_id = 0;
	uint32_t ignored = 0;

	if (ring_tally_counted == 0u)
		return;
	if ((int16_t)ring_tally_percent < (int16_t)stage_ring_bonus_percents[stage])
	{
		sound_id = 0x73u;
		queue_sound(sound_id, &ignored, &ignored);
	}
	delete_current_object();
}

void ring_tally_ticker_update(void)
{
	uint8_t sound_id = 0;
	uint32_t ignored = 0;

	if (OBJECT(current_object)->ring_tally_ticker.started == 0u)
	{
		OBJECT(current_object)->ring_tally_ticker.started = 1u;
		OBJECT(current_object)->timer = 0;
	}
	if (ring_tally_counted != 0u)
	{
		delete_current_object();
		return;
	}
	OBJECT(current_object)->timer++;
	if (OBJECT(current_object)->timer >= 6)
	{
		OBJECT(current_object)->timer = 0;
		sound_id = 0x83u;
		queue_sound(sound_id, &ignored, &ignored);
	}
}

void ring_tally_player_1_update(void)
{
	uint32_t character = 0;

	if (OBJECT(current_object)->ring_tally_player.started != 0u)
	{
		update_ring_tally_panel();
		return;
	}
	character = OBJECT(current_object)->ring_tally_player.character;
	build_ring_tally_panel(character);
	OBJECT(current_object)->ring_tally_player.step_delay = 4u;
	OBJECT(object_cursor)->ring_tally_player.palette_delay = 2u;
	OBJECT(current_object)->ring_tally_player.step = 0u;
	OBJECT(current_object)->ring_tally_player.rings_counted = 0;
	OBJECT(current_object)->ring_tally_player.started = 1u;
}

void ring_tally_player_2_update(void)
{
	uint32_t character = 0;

	if (OBJECT(current_object)->ring_tally_player.started != 0u)
	{
		update_ring_tally_panel();
		return;
	}
	if ((cabinet_config & 1u) == 0u)
	{
		ring_tally_player_3_update();
		return;
	}
	character = OBJECT(current_object)->ring_tally_player.character;
	build_ring_tally_panel(character);
	OBJECT(object_cursor)->screen_x = 0xA0;
	OBJECT(OBJECT(current_object)->ring_tally_player.bonus_label)->screen_x = 0xA0;
	OBJECT(OBJECT(current_object)->ring_tally_player.backdrop)->screen_x = 0xA0;
	OBJECT(OBJECT(current_object)->ring_tally_player.part_3)->screen_x = 0xA0;
	OBJECT(OBJECT(current_object)->ring_tally_player.units_digit)->screen_x = 0xB6;
	OBJECT(OBJECT(current_object)->ring_tally_player.tens_digit)->screen_x = 0xAD;
	OBJECT(OBJECT(current_object)->ring_tally_player.hundreds_digit)->screen_x = 0xA4;
	OBJECT(current_object)->ring_tally_player.step_delay = 4u;
	OBJECT(object_cursor)->ring_tally_player.palette_delay = 2u;
	OBJECT(current_object)->ring_tally_player.step = 0u;
	OBJECT(current_object)->ring_tally_player.rings_counted = 0;
	OBJECT(current_object)->ring_tally_player.started = 1u;
}

void ring_tally_player_3_update(void)
{
	uint32_t character = 0;

	if (OBJECT(current_object)->ring_tally_player.started != 0u)
	{
		update_ring_tally_panel();
		return;
	}
	character = OBJECT(current_object)->ring_tally_player.character;
	build_ring_tally_panel(character);
	OBJECT(object_cursor)->screen_x = 0x100;
	OBJECT(OBJECT(current_object)->ring_tally_player.bonus_label)->screen_x = 0x100;
	OBJECT(OBJECT(current_object)->ring_tally_player.backdrop)->screen_x = 0x100;
	OBJECT(OBJECT(current_object)->ring_tally_player.part_3)->screen_x = 0x100;
	OBJECT(OBJECT(current_object)->ring_tally_player.units_digit)->screen_x = 0x116;
	OBJECT(OBJECT(current_object)->ring_tally_player.tens_digit)->screen_x = 0x10D;
	OBJECT(OBJECT(current_object)->ring_tally_player.hundreds_digit)->screen_x = 0x104;
	OBJECT(current_object)->ring_tally_player.step_delay = 4u;
	OBJECT(object_cursor)->ring_tally_player.palette_delay = 2u;
	OBJECT(current_object)->ring_tally_player.step = 0u;
	OBJECT(current_object)->ring_tally_player.rings_counted = 0;
	OBJECT(current_object)->ring_tally_player.started = 1u;
}

void ring_tally_player_3_update_spare_return(void)
{
}

void build_ring_tally_panel(uint32_t character)
{
	uint32_t ignored = 0, object = 0, portrait_record = 0;

	portrait_record = ring_tally_portraits[character];
	OBJECT(current_object)->sprite_palette = RING_TALLY_PORTRAIT(portrait_record)->sprite_palette;
	OBJECT(current_object)->sprite_palette |= 0x8000u;
	portrait_record += 2u;
	OBJECT(current_object)->sprite_frame = RING_TALLY_PORTRAIT(portrait_record - 0x2u)->sprite_frame;
	OBJECT(current_object)->sprite_flags = 0u;
	OBJECT(current_object)->sprite_scale = 0x100u;
	OBJECT(current_object)->screen_y = 0xA0;
	OBJECT(current_object)->screen_x = 0x40;
	OBJECT(current_object)->sprite_layer = 0u;
	OBJECT(current_object)->build_ring_tally_panel.result_frames = portrait_record + 2u;
	allocate_object_2(&ignored, &ignored, &object);
	OBJECT(current_object)->build_ring_tally_panel.bonus_label = object;
	OBJECT(object)->sprite_frame = 0x9EF0u;
	if (OBJECT(current_object)->build_ring_tally_panel.gets_bonus == 0u)
		OBJECT(object)->sprite_frame = 0x9EF8u;
	OBJECT(object)->sprite_palette = 0xC40u;
	OBJECT(object)->sprite_palette |= 0x8000u;
	OBJECT(object)->screen_x = 0x40;
	OBJECT(object)->screen_y = 0x59;
	OBJECT(object)->sprite_layer = 0u;
	OBJECT(object)->sprite_flags = 0u;
	OBJECT(object)->sprite_scale = 0x100u;
	allocate_object_2(&ignored, &ignored, &object);
	OBJECT(current_object)->build_ring_tally_panel.backdrop = object;
	OBJECT(object)->screen_x = 0x40;
	OBJECT(object)->screen_y = 0xA0;
	OBJECT(object)->sprite_layer = 1u;
	OBJECT(object)->sprite_frame = 0x9DD8u;
	OBJECT(object)->sprite_palette = 0xC50u;
	OBJECT(object)->sprite_palette |= 0x8000u;
	OBJECT(object)->sprite_flags = 0u;
	OBJECT(object)->sprite_scale = 0x100u;
	allocate_object_2(&ignored, &ignored, &object);
	OBJECT(current_object)->build_ring_tally_panel.part_3 = object;
	OBJECT(object)->screen_x = 0x40;
	OBJECT(object)->screen_y = 0x70;
	OBJECT(object)->sprite_layer = 0u;
	OBJECT(object)->sprite_frame = 0x9E90u;
	OBJECT(object)->sprite_palette = 0xC20u;
	OBJECT(object)->sprite_palette |= 0x8000u;
	OBJECT(object)->sprite_flags = 0u;
	OBJECT(object)->sprite_scale = 0x100u;
	allocate_object_2(&ignored, &ignored, &object);
	OBJECT(current_object)->build_ring_tally_panel.units_digit = object;
	OBJECT(object)->screen_x = 0x56;
	OBJECT(object)->screen_y = 0x75;
	OBJECT(object)->sprite_layer = 0u;
	OBJECT(object)->sprite_frame = 0x9E40u;
	OBJECT(object)->sprite_palette = 0xC10u;
	OBJECT(object)->sprite_palette |= 0x8000u;
	OBJECT(object)->sprite_flags = 0u;
	OBJECT(object)->sprite_scale = 0x100u;
	allocate_object_2(&ignored, &ignored, &object);
	OBJECT(current_object)->build_ring_tally_panel.tens_digit = object;
	OBJECT(object)->screen_x = 0x4D;
	OBJECT(object)->screen_y = 0x75;
	OBJECT(object)->sprite_layer = 0u;
	OBJECT(object)->sprite_frame = 0x9E40u;
	OBJECT(object)->sprite_palette = 0xC10u;
	OBJECT(object)->sprite_palette |= 0x8000u;
	OBJECT(object)->sprite_flags = 0u;
	OBJECT(object)->sprite_scale = 0x100u;
	allocate_object_2(&ignored, &ignored, &object);
	OBJECT(current_object)->build_ring_tally_panel.hundreds_digit = object;
	OBJECT(object)->screen_x = 0x44;
	OBJECT(object)->screen_y = 0x75;
	OBJECT(object)->sprite_layer = 0u;
	OBJECT(object)->sprite_frame = 0x9E40u;
	OBJECT(object)->sprite_palette = 0xC10u;
	OBJECT(object)->sprite_palette |= 0x8000u;
	OBJECT(object)->sprite_flags = 0u;
	OBJECT(object)->sprite_scale = 0x100u;
}

void update_ring_tally_panel(void)
{
	uint8_t sound_id = 0;
	uint32_t counted_bcd = 0, ignored = 0, speed_x = 0;

	if (stage_results_done != 0u)
	{
		delete_current_object();
		return;
	}
	switch (OBJECT(current_object)->update_ring_tally_panel.step)
	{
	case 0:
	update_ring_tally_panel_count_up:
		queue_sprite_in_layer();
		current_object = OBJECT(object_cursor)->update_ring_tally_panel.part_3;
		queue_sprite_in_layer();
		current_object = OBJECT(object_cursor)->update_ring_tally_panel.units_digit;
		queue_sprite_in_layer();
		current_object = OBJECT(object_cursor)->update_ring_tally_panel.hundreds_digit;
		if (OBJECT(current_object)->sprite_frame == 0x9E40u)
		{
			current_object = OBJECT(object_cursor)->update_ring_tally_panel.tens_digit;
			if (OBJECT(current_object)->sprite_frame != 0x9E40u)
				queue_sprite_in_layer();
		}
		else
		{
			queue_sprite_in_layer();
			current_object = OBJECT(object_cursor)->update_ring_tally_panel.tens_digit;
			queue_sprite_in_layer();
		}
		OBJECT(object_cursor)->update_ring_tally_panel.step_delay--;
		if (OBJECT(object_cursor)->update_ring_tally_panel.step_delay != 0u)
			return;
		if (OBJECT(object_cursor)->update_ring_tally_panel.ring_total <= OBJECT(object_cursor)->update_ring_tally_panel.rings_counted)
		{
			ring_tally_panels_counting--;
			if (ring_tally_panels_counting == 0u)
				ring_tally_counted = 1u;
			OBJECT(object_cursor)->update_ring_tally_panel.step++;
			if (OBJECT(object_cursor)->update_ring_tally_panel.gets_bonus == 0u)
			{
				OBJECT(object_cursor)->update_ring_tally_panel.step++;
				OBJECT(object_cursor)->update_ring_tally_panel.step++;
			}
		}
		counted_bcd = (uint8_t)OBJECT(object_cursor)->update_ring_tally_panel.rings_counted;
		convert_to_bcd(&counted_bcd, &ignored);
		current_object = OBJECT(object_cursor)->update_ring_tally_panel.units_digit;
		OBJECT(current_object)->sprite_frame = ring_tally_digits[counted_bcd & 0xFu];
		current_object = OBJECT(object_cursor)->update_ring_tally_panel.tens_digit;
		OBJECT(current_object)->sprite_frame = ring_tally_digits[(counted_bcd >> 4) & 0xFu];
		current_object = OBJECT(object_cursor)->update_ring_tally_panel.hundreds_digit;
		OBJECT(current_object)->sprite_frame = ring_tally_digits[(counted_bcd >> 8) & 0xFu];
		OBJECT(object_cursor)->update_ring_tally_panel.step_delay = 2u;
		OBJECT(object_cursor)->update_ring_tally_panel.rings_counted++;
		return;
	case 1:
	case 5:
		speed_x = OBJECT(object_cursor)->speed_x;
		give_result_bonus(speed_x);
		sound_id = ring_tally_character_sounds[OBJECT(object_cursor)->update_ring_tally_panel.character];
		queue_sound(sound_id, &ignored, &ignored);
		sound_id = 0x88u;
		queue_sound(sound_id, &ignored, &ignored);
		OBJECT(object_cursor)->update_ring_tally_panel.step++;
		goto update_ring_tally_panel_count_up;
	case 2:
		OBJECT(object_cursor)->sprite_frame = ROM16(OBJECT(object_cursor)->update_ring_tally_panel.result_frames + 2u);
		queue_sprite_in_layer();
		current_object = OBJECT(object_cursor)->update_ring_tally_panel.bonus_label;
		queue_sprite_in_layer();
		cycle_ring_tally_backdrop_palette();
		current_object = OBJECT(object_cursor)->update_ring_tally_panel.backdrop;
		queue_sprite_in_layer();
		current_object = OBJECT(object_cursor)->update_ring_tally_panel.part_3;
		queue_sprite_in_layer();
		current_object = OBJECT(object_cursor)->update_ring_tally_panel.units_digit;
		queue_sprite_in_layer();
		current_object = OBJECT(object_cursor)->update_ring_tally_panel.hundreds_digit;
		if (OBJECT(current_object)->sprite_frame == 0x9E40u)
		{
			current_object = OBJECT(object_cursor)->update_ring_tally_panel.tens_digit;
			if (OBJECT(current_object)->sprite_frame != 0x9E40u)
				queue_sprite_in_layer();
		}
		else
		{
			queue_sprite_in_layer();
			current_object = OBJECT(object_cursor)->update_ring_tally_panel.tens_digit;
			queue_sprite_in_layer();
		}
		update_ring_tally_panel_tail();
		return;
	case 3:
		sound_id = 0x89u;
		queue_sound(sound_id, &ignored, &ignored);
		OBJECT(object_cursor)->update_ring_tally_panel.step++;
		OBJECT(object_cursor)->sprite_frame = ROM16(OBJECT(object_cursor)->update_ring_tally_panel.result_frames);
		queue_sprite_in_layer();
		current_object = OBJECT(object_cursor)->update_ring_tally_panel.bonus_label;
		queue_sprite_in_layer();
		current_object = OBJECT(object_cursor)->update_ring_tally_panel.part_3;
		queue_sprite_in_layer();
		current_object = OBJECT(object_cursor)->update_ring_tally_panel.units_digit;
		queue_sprite_in_layer();
		current_object = OBJECT(object_cursor)->update_ring_tally_panel.hundreds_digit;
		if (OBJECT(current_object)->sprite_frame == 0x9E40u)
		{
			current_object = OBJECT(object_cursor)->update_ring_tally_panel.tens_digit;
			if (OBJECT(current_object)->sprite_frame != 0x9E40u)
				queue_sprite_in_layer();
		}
		else
		{
			queue_sprite_in_layer();
			current_object = OBJECT(object_cursor)->update_ring_tally_panel.tens_digit;
			queue_sprite_in_layer();
		}
		return;
	case 4:
		OBJECT(object_cursor)->sprite_frame = ROM16(OBJECT(object_cursor)->update_ring_tally_panel.result_frames);
		queue_sprite_in_layer();
		current_object = OBJECT(object_cursor)->update_ring_tally_panel.bonus_label;
		queue_sprite_in_layer();
		current_object = OBJECT(object_cursor)->update_ring_tally_panel.part_3;
		queue_sprite_in_layer();
		current_object = OBJECT(object_cursor)->update_ring_tally_panel.units_digit;
		queue_sprite_in_layer();
		current_object = OBJECT(object_cursor)->update_ring_tally_panel.hundreds_digit;
		if (OBJECT(current_object)->sprite_frame == 0x9E40u)
		{
			current_object = OBJECT(object_cursor)->update_ring_tally_panel.tens_digit;
			if (OBJECT(current_object)->sprite_frame != 0x9E40u)
				queue_sprite_in_layer();
		}
		else
		{
			queue_sprite_in_layer();
			current_object = OBJECT(object_cursor)->update_ring_tally_panel.tens_digit;
			queue_sprite_in_layer();
		}
		return;
	default:
		invalid_jump(0x98C43u + OBJECT(current_object)->update_ring_tally_panel.step * 3u);
	}
}

void update_ring_tally_panel_tail(void)
{
}

void cycle_ring_tally_backdrop_palette(void)
{
	OBJECT(object_cursor)->cycle_ring_tally_backdrop_palette.palette_delay--;
	if (OBJECT(object_cursor)->cycle_ring_tally_backdrop_palette.palette_delay != 0u)
		return;
	OBJECT(object_cursor)->cycle_ring_tally_backdrop_palette.palette_delay = 2u;
	OBJECT(object_cursor)->cycle_ring_tally_backdrop_palette.palette_index++;
	if (OBJECT(object_cursor)->cycle_ring_tally_backdrop_palette.palette_index > 6)
		OBJECT(object_cursor)->cycle_ring_tally_backdrop_palette.palette_index = 0;
	current_object = OBJECT(object_cursor)->z;
	OBJECT(current_object)->sprite_palette = ring_tally_backdrop_palettes[(uint8_t)OBJECT(object_cursor)->cycle_ring_tally_backdrop_palette.palette_index];
	OBJECT(current_object)->sprite_palette |= 0x8000u;
}
