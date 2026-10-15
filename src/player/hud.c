/**
 *    Name:        player/hud.c
 *    Description: the player's HUD (gauge, portrait, ring count, start prompt, messages); before it the unused stage slot mover and the skid dust
 *    Version:     0.1.0
 *    Created:     12-10-2026
 *    Modified:    15-10-2026
 *    Author:      Sebastian Toyos
 *
 *    SegaSonic The Hedgehog (arcade, Sega System 32): the game's V60 program decompiled into C by
 *    tools/decompile.py, with the names of symbols.txt.
 */
#include "game.h"

void move_object_to_next_stage_slot(void)
{
	if (next_stage_slot_request != 0u)
	{
		OBJECT(current_object)->x = (uint32_t)OBJECT(current_object)->x - stage_link_from_x + stage_link_to_x;
		OBJECT(current_object)->y = (uint32_t)OBJECT(current_object)->y - stage_link_from_y + stage_link_to_y;
		OBJECT(current_object)->previous_floor_z_int = OBJECT(current_object)->previous_floor_z_int - stage_link_from_height + stage_link_to_height;
		OBJECT(current_object)->z_int = OBJECT(current_object)->z_int - stage_link_from_height + stage_link_to_height;
		OBJECT(current_object)->previous_floor_block = (uint16_t)(OBJECT(current_object)->previous_floor_block - stage_link_from_block + stage_link_to_block);
	}
}

void spawn_skid_dust(uint32_t dust)
{
	uint16_t priority_bits = 0;
	uint32_t free_count = 0;

	allocate_object(&free_count, &dust);
	if (free_count != 0u)
	{
		OBJECT(dust)->sprite_flags = 0x1000u;
		OBJECT(dust)->sprite_palette = 0x8260u;
		OBJECT(dust)->animation_frame = 0u;
		OBJECT(dust)->animation_speed = 3u;
		OBJECT(dust)->animation_timer = 3;
		OBJECT(dust)->animation = skid_dust_animation_ADDRESS;
		OBJECT(dust)->sprite_frame = mem_read16(OBJECT(dust)->animation + 2u);
		OBJECT(dust)->projected_x = OBJECT(current_object)->projected_x;
		OBJECT(dust)->projected_y = OBJECT(current_object)->projected_y;
		OBJECT(dust)->z = OBJECT(current_object)->z;
		OBJECT(dust)->sprite_offset_x = 0;
		OBJECT(dust)->sprite_offset_y = 0;
		OBJECT(dust)->handler = skid_dust_update_ADDRESS;
		priority_bits = OBJECT(current_object)->sprite_palette & 0x6000u;
		OBJECT(dust)->sprite_palette &= 0x9FFFu;
		OBJECT(dust)->sprite_palette |= priority_bits;
	}
}

void skid_dust_update(void)
{
	uint32_t r1 = 0, r8 = 0, ignored = 0, wrapped = 0;

	animate(&r1, &wrapped, &r8);
	if ((uint8_t)wrapped == 0u)
	{
		update_screen_position(&ignored, &r1);
		r1 = queue_sprite(r1);
		return;
	}
	delete_current_object();
}

void player_hud_init(void)
{
	uint32_t gauge_position = 0, object = 0, player = 0, player_2 = 0, portrait_anim = 0, portrait_position = 0, portrait_sprite = 0;

	PLAYER(current_player)->hud_blink_on = 0u;
	PLAYER(current_player)->stage_rings_shown = 0xFFFFu;
	PLAYER(current_player)->hud_blink_timer = 0x1E;
	player = current_player + 0x142u;
	PLAYER(player - 0x142u)->hud_gauge_flags = 0x900Au;
	PLAYER(player - 0x142u)->hud_gauge_tile = 0x89E0u;
	PLAYER(player - 0x142u)->hud_gauge_address = mem_read16(((sprite_frames & ~0xFFFFu) | PLAYER(player - 0x142u)->hud_gauge_tile) + 6u);
	PLAYER(player - 0x142u)->hud_gauge_health = OBJECT(current_object)->health;
	PLAYER(player - 0x142u)->hud_gauge_frame = OBJECT(current_object)->player_view.health_pad;
	player_2 = OBJECT(current_object)->player;
	gauge_position = hud_gauge_positions_3p_ADDRESS + player_2 * 4u;
	if ((cabinet_config & 1u) == 0u)
		gauge_position = hud_gauge_positions_2p_ADDRESS + player_2 * 4u;
	PLAYER(player - 0x142u)->hud_gauge_x = ROM16(gauge_position);
	PLAYER(player - 0x142u)->hud_gauge_y = ROM16(gauge_position + 2u);
	PLAYER(player - 0x142u)->hud_gauge_palette = 0x88E0u;
	PLAYER(player - 0x142u)->hud_gauge_height = 8u;
	PLAYER(current_player)->hud_gauge = player;
	PLAYER(player - 0x142u)->hud_gauge_anim = hud_gauge_hit_palettes_ADDRESS;
	PLAYER(player - 0x142u)->hud_gauge_anim_delay = 3u;
	PLAYER(player - 0x142u)->hud_gauge_frame = 0u;
	PLAYER(player - 0x142u)->hud_gauge_anim_repeats = 0xFFu;
	PLAYER(player - 0x142u)->hud_gauge_reset = 0u;
	player = current_player + 0x172u;
	PLAYER(player - 0x172u)->hud_portrait_flags = 0x100Au;
	player_2 = OBJECT(current_object)->player;
	portrait_position = hud_portrait_positions_3p_ADDRESS + player_2 * 4u;
	if ((cabinet_config & 1u) == 0u)
		portrait_position = hud_portrait_positions_2p_ADDRESS + player_2 * 4u;
	PLAYER(player - 0x172u)->hud_portrait_x = ROM16(portrait_position);
	PLAYER(player - 0x172u)->hud_portrait_y = ROM16(portrait_position + 2u);
	portrait_sprite = hud_portrait_sprites_ADDRESS + OBJECT(current_object)->kind * 4u;
	PLAYER(player - 0x172u)->hud_portrait_tile = ROM16(portrait_sprite);
	PLAYER(player - 0x172u)->hud_portrait_palette = ROM16(portrait_sprite + 2u);
	PLAYER(player - 0x172u)->hud_portrait_height = 0xAu;
	PLAYER(current_player)->hud_portrait = player;
	PLAYER(player - 0x172u)->hud_portrait_anim_delay = 3u;
	PLAYER(player - 0x172u)->hud_portrait_frame = 0u;
	PLAYER(player - 0x172u)->hud_portrait_anim_repeats = 0xFFu;
	portrait_anim = hud_portrait_hurt_palettes_sonic_ADDRESS;
	if (OBJECT(current_object)->kind != 0u)
	{
		portrait_anim = hud_portrait_hurt_palettes_ray_ADDRESS;
		if (OBJECT(current_object)->kind != 1u)
			portrait_anim = hud_portrait_hurt_palettes_mighty_ADDRESS;
	}
	PLAYER(player - 0x172u)->hud_portrait_anim = portrait_anim;
	player = current_player + 0x1A2u;
	PLAYER(player - 0x1A2u)->hud_ring_effect_flags = 0x1000u;
	PLAYER(player - 0x1A2u)->hud_ring_effect_palette = 0x1B0u;
	PLAYER(player - 0x1A2u)->hud_ring_effect_anim = ring_sparkle_animation_ADDRESS;
	PLAYER(player - 0x1A2u)->hud_ring_effect_frame = 0xFFu;
	PLAYER(player - 0x1A2u)->hud_ring_effect_anim_timer = 0u;
	PLAYER(player - 0x1A2u)->hud_ring_effect_anim_delay = 2u;
	object = LINKED(current_object, PLAYER(current_player)->hud_gauge);
	PLAYER(player - 0x1A2u)->hud_ring_effect_x = OBJECT(object)->screen_x;
	PLAYER(player - 0x1A2u)->hud_ring_effect_x += 0x10u;
	PLAYER(player - 0x1A2u)->hud_ring_effect_y = OBJECT(object)->screen_y;
	PLAYER(current_player)->hud_ring_effect = player;
	PLAYER(current_player)->unused_hud_byte = 0u;
	PLAYER(current_player)->unused_hud_word = 0xFFFFu;
}

void player_hud_start(void)
{
	uint32_t r1 = 0, init_bit = 0, object = 0, player = 0, sprite_record = 0, text = 0;

	sprite_record = hud_portrait_sprites_ADDRESS + OBJECT(current_object)->kind * 4u;
	object = LINKED(current_object, PLAYER(current_player)->hud_portrait);
	OBJECT(object)->sprite_frame = ROM16(sprite_record);
	OBJECT(object)->sprite_palette = ROM16(sprite_record + 2u);
	player = OBJECT(current_object)->player;
	if ((int8_t)player_status[player] >= 0 || player_status[player] == 0x82u)
	{
		player_hud_update(&r1, &object);
		return;
	}
	text = hud_message_blank_text_ADDRESS;
	player = OBJECT(current_object)->player;
	object = 0x310000u | hud_text_positions_3p[player];
	if ((cabinet_config & 1u) == 0u)
		object = (object & ~0xFFFFu) | hud_text_positions_2p[player];
	erase_text_lines(&text, &object);
	if (game_in_progress != 0u)
	{
		player_hud_update(&r1, &object);
		return;
	}
	init_bit = 0xFu;
	if (OBJECT(current_object)->player != 0u)
	{
		init_bit = 0x12u;
		if (OBJECT(current_object)->player != 1u)
			init_bit = 0x13u;
	}
	stage_init_flags &= ~shl32(1u, (int)(init_bit));
	player_hud_update(&r1, &object);
}

void player_hud_update(uint32_t *p_digit_attribute, uint32_t *p_text_position)
{
	uint32_t digit_attribute = *p_digit_attribute;
	uint32_t text_position = *p_text_position;
	uint32_t r11 = 0, digit = 0, message_position = 0, object = 0, player = 0, player_2 = 0, portrait_anim = 0, text = 0, value = 0, wrapped = 0;
	uint32_t flags = 0;

	player = OBJECT(current_object)->player;
	if (player_status[player] == 0x82u)
	{
		text = hud_wait_text_ADDRESS;
		text_position = blink_player_hud_message(text, text_position);
		AT8(OBJECT(LINKED(current_object, PLAYER(current_player)->hud_gauge))->projected_y, 1) = 0xFFu;
		*p_text_position = text_position;
		return;
	}
	if (PLAYER(current_player)->state == 0x23u || PLAYER(current_player)->state == 0x2Eu || PLAYER(current_player)->state == 0x34u || PLAYER(current_player)->state == 0x24u || PLAYER(current_player)->state == 0x37u)
	{
		AT8(OBJECT(LINKED(current_object, PLAYER(current_player)->hud_gauge))->projected_y, 1) = 0xFFu;
		return;
	}
	if (PLAYER(current_player)->state == 0x2Bu)
	{
		if (PLAYER(current_player)->hud_message_shown == 0u)
		{
			AT8(OBJECT(LINKED(current_object, PLAYER(current_player)->hud_gauge))->projected_y, 1) = 0xFFu;
			return;
		}
		PLAYER(current_player)->hud_message_shown = 0u;
		text = hud_message_blank_text_ADDRESS;
		player = OBJECT(current_object)->player;
		text_position = 0x310000u | hud_text_positions_3p[player];
		if ((cabinet_config & 1u) == 0u)
			text_position = (text_position & ~0xFFFFu) | hud_text_positions_2p[player];
		erase_text_lines(&text, &text_position);
		AT8(OBJECT(LINKED(current_object, PLAYER(current_player)->hud_gauge))->projected_y, 1) = 0xFFu;
		*p_text_position = text_position;
		return;
	}
	if (PLAYER(current_player)->state == 0x39u && game_mode_timer >= 0x3C)
	{
		AT8(OBJECT(LINKED(current_object, PLAYER(current_player)->hud_gauge))->projected_y, 1) = 0xFFu;
		return;
	}
	if ((int8_t)player_status[player] < 0 || player_status[player] == 1u)
	{
		object = LINKED(current_object, PLAYER(current_player)->hud_gauge);
		current_object = LINKED(current_object, PLAYER(current_player)->hud_portrait);
		if (AT8(OBJECT(object)->projected_y, 1) != 0u)
		{
			AT8(OBJECT(object)->projected_y, 1) = 0u;
			AS8(OBJECT(object)->direction) = OBJECT(object_cursor)->health;
			AS8(OBJECT(object)->projected_y) = 0u;
			AS8(OBJECT(current_object)->projected_y) = 0u;
			OBJECT(object)->sprite_palette = mem_read16(AS32(OBJECT(object)->child) + 2u);
			portrait_anim = hud_portrait_hurt_palettes_sonic_ADDRESS;
			if (OBJECT(object_cursor)->kind != 0u)
			{
				portrait_anim = hud_portrait_hurt_palettes_ray_ADDRESS;
				if (OBJECT(object_cursor)->kind != 1u)
					portrait_anim = hud_portrait_hurt_palettes_mighty_ADDRESS;
			}
			AS32(OBJECT(current_object)->child) = portrait_anim;
			OBJECT(current_object)->sprite_palette = mem_read16(AS32(OBJECT(current_object)->child) + 2u);
		}
		flags = PLAYER(current_player)->flags;
		PLAYER(current_player)->flags = flags & ~0x40u;
		if ((flags & 0x40u) == 0u)
		{
			if (OBJECT(object_cursor)->health < (int8_t)AS8(OBJECT(object)->direction))
			{
				OBJECT(object)->child = 0xCD4Du;
				OBJECT(object)->projected_x = 0;
				AT8(OBJECT(object)->direction, 1) = 0u;
				AT8(OBJECT(object)->parent, 1) = 0u;
				AS8(OBJECT(object)->projected_y) = 3u;
				portrait_anim = hud_portrait_hurt_palettes_sonic_ADDRESS;
				if (OBJECT(object_cursor)->kind != 0u)
				{
					portrait_anim = hud_portrait_hurt_palettes_ray_ADDRESS;
					if (OBJECT(object_cursor)->kind != 1u)
						portrait_anim = hud_portrait_hurt_palettes_mighty_ADDRESS;
				}
				AS32(OBJECT(current_object)->child) = portrait_anim;
				AT8(OBJECT(current_object)->direction, 1) = 0u;
				AT8(OBJECT(current_object)->parent, 1) = 0u;
				AS8(OBJECT(current_object)->projected_y) = 3u;
			}
		}
		else
		{
			OBJECT(object)->child = 0xCD37u;
			OBJECT(object)->projected_x = 0;
			AT8(OBJECT(object)->direction, 1) = 0u;
			AT8(OBJECT(object)->parent, 1) = 0u;
			AS8(OBJECT(object)->projected_y) = 1u;
			AT8(OBJECT((current_object & ~0xFFFFu) | PLAYER(current_player)->hud_ring_effect)->direction, 1) = 0u;
		}
		AS8(OBJECT(object)->direction) = OBJECT(object_cursor)->health;
		if (OBJECT(object_cursor)->health <= 8 && (int8_t)AS8(OBJECT(object)->projected_y) <= 0)
		{
			OBJECT(object)->child = 0xCD55u;
			OBJECT(object)->projected_x = 0;
			AT8(OBJECT(object)->direction, 1) = 0u;
			AT8(OBJECT(object)->parent, 1) = 0u;
			AS8(OBJECT(object)->projected_y) = 3u;
			portrait_anim = hud_portrait_low_health_palettes_sonic_ADDRESS;
			if (OBJECT(object_cursor)->kind != 0u)
			{
				portrait_anim = hud_portrait_low_health_palettes_ray_ADDRESS;
				if (OBJECT(object_cursor)->kind != 1u)
					portrait_anim = hud_portrait_low_health_palettes_mighty_ADDRESS;
			}
			AS32(OBJECT(current_object)->child) = portrait_anim;
			AT8(OBJECT(current_object)->direction, 1) = 0u;
			AT8(OBJECT(current_object)->parent, 1) = 0u;
			AS8(OBJECT(current_object)->projected_y) = 3u;
		}
		if (game_in_progress != 0u)
		{
			value = PLAYER(current_player)->rings;
			convert_to_bcd(&value, &digit);
			digit_attribute = (digit & ~0xFFFFu) | 0x800u;
			if (OBJECT(object_cursor)->kind != 0u)
			{
				digit_attribute = (digit_attribute & ~0xFFFFu) | 0xA00u;
				if (OBJECT(object_cursor)->kind != 1u)
					digit_attribute = (digit_attribute & ~0xFFFFu) | 0xC00u;
			}
			player_2 = OBJECT(object_cursor)->player;
			text_position = 0x310000u | hud_ring_count_positions_3p[player_2];
			if ((cabinet_config & 1u) == 0u)
				text_position = (text_position & ~0xFFFFu) | hud_ring_count_positions_2p[player_2];
			draw_hex_4(&value, &digit_attribute, &text_position);
		}
		current_object = (current_object & ~0xFFFFu) | PLAYER(current_player)->hud_ring_effect;
		if ((int8_t)AT8(OBJECT(current_object)->direction, 1) >= 0)
		{
			animate_hud_tiles(&digit_attribute, &wrapped);
			if ((int8_t)wrapped < 0)
				AT8(OBJECT(current_object)->direction, 1) = 0xFFu;
			queue_sprite_on_top();
		}
		current_object = LINKED(current_object, PLAYER(current_player)->hud_portrait);
		if ((int8_t)AS8(OBJECT(current_object)->projected_y) > 0)
		{
			animate_hud_palette(&digit_attribute, &wrapped);
			if (wrapped != 0u)
				AS8(OBJECT(current_object)->projected_y)--;
		}
		queue_sprite_on_top();
		current_object = LINKED(current_object, PLAYER(current_player)->hud_gauge);
		if ((int8_t)AS8(OBJECT(current_object)->projected_y) > 0)
		{
			animate_hud_palette(&digit_attribute, &wrapped);
			if (wrapped != 0u)
				AS8(OBJECT(current_object)->projected_y)--;
		}
		OBJECT(current_object)->sprite_width = (uint8_t)OBJECT(object_cursor)->health * 2u;
		if (OBJECT(current_object)->sprite_width <= 0)
		{
			current_object = (current_object & ~0xFFFFu) | (uint16_t)object_cursor;
			*p_digit_attribute = digit_attribute;
			*p_text_position = text_position;
			return;
		}
		OBJECT(current_object)->sprite_width++;
		queue_sprite_on_top();
		current_object = (current_object & ~0xFFFFu) | (uint16_t)object_cursor;
		*p_digit_attribute = digit_attribute;
		*p_text_position = text_position;
		return;
	}
	AT8(OBJECT(LINKED(current_object, PLAYER(current_player)->hud_gauge))->projected_y, 1) = 0xFFu;
	if (current_game_mode == 0x18u)
	{
		if (PLAYER(current_player)->hud_message_shown == 0u)
		{
			AT8(OBJECT(LINKED(current_object, PLAYER(current_player)->hud_gauge))->projected_y, 1) = 0xFFu;
			return;
		}
		PLAYER(current_player)->hud_message_shown = 0u;
		text = hud_message_blank_text_ADDRESS;
		player = OBJECT(current_object)->player;
		text_position = 0x310000u | hud_text_positions_3p[player];
		if ((cabinet_config & 1u) == 0u)
			text_position = (text_position & ~0xFFFFu) | hud_text_positions_2p[player];
		erase_text_lines(&text, &text_position);
		AT8(OBJECT(LINKED(current_object, PLAYER(current_player)->hud_gauge))->projected_y, 1) = 0xFFu;
		*p_text_position = text_position;
		return;
	}
	if (joining_closed == 0u)
	{
		r11 = draw_player_start_prompt();
		*p_digit_attribute = digit_attribute;
		*p_text_position = r11;
		return;
	}
	if (PLAYER(current_player)->hud_message_shown != 0u)
		return;
	PLAYER(current_player)->hud_message_shown = 0xFFu;
	text = hud_message_blank_text_ADDRESS;
	player = OBJECT(current_object)->player;
	message_position = 0x310000u | hud_text_positions_3p[player];
	if ((cabinet_config & 1u) == 0u)
		message_position = (message_position & ~0xFFFFu) | hud_text_positions_2p[player];
	erase_text_lines(&text, &message_position);
	player = OBJECT(current_object)->player;
	text_position = 0x310000u | start_prompt_positions_3p[player];
	if ((cabinet_config & 1u) == 0u)
		text_position = (text_position & ~0xFFFFu) | start_prompt_positions_2p[player];
	text = hud_no_continue_text_ADDRESS;
	print_text_lines(&text, &text_position);
	*p_text_position = text_position;
}

uint32_t draw_player_start_prompt(void)
{
	uint32_t blink_bit = 0, credit_block = 0, player = 0, prompt_position = 0, prompt_texts = 0, text = 0;

	switch (cabinet_config)
	{
	case 0:
	case 1:
	case 4:
		credit_block = p1_coins_ADDRESS;
		player = OBJECT(current_object)->player;
		prompt_position = 0x310000u | start_prompt_positions_3p[player];
		if ((cabinet_config & 1u) == 0u)
			prompt_position = (prompt_position & ~0xFFFFu) | start_prompt_positions_2p[player];
		prompt_texts = start_prompt_texts_ADDRESS;
		blink_start_prompt(credit_block, prompt_texts, &blink_bit, &text, &prompt_position);
		return prompt_position;
	case 2:
	case 3:
		player = OBJECT(current_object)->player;
		credit_block = p1_coins_ADDRESS + player * 4u;
		prompt_position = 0x310000u | start_prompt_positions_3p[player];
		if ((cabinet_config & 1u) == 0u)
			prompt_position = (prompt_position & ~0xFFFFu) | start_prompt_positions_2p[player];
		prompt_texts = start_prompt_texts_ADDRESS;
		blink_start_prompt(credit_block, prompt_texts, &blink_bit, &text, &prompt_position);
		return prompt_position;
	default:
		unknown_code(0xA9E6u + cabinet_config * 2u);
	}
}

void animate_hud_palette(uint32_t *p_list_index, uint32_t *p_wrapped)
{
	uint32_t list_index = *p_list_index;
	uint8_t last_frame = 0;
	uint32_t palettes = 0, wrapped = 0;
	uint8_t anim_timer = 0;

	wrapped = 0u;
	anim_timer = OBJECT(current_object)->animate_hud.anim_timer;
	OBJECT(current_object)->animate_hud.anim_timer = anim_timer - 1u;
	if ((int8_t)anim_timer > 1)
	{
		*p_wrapped = wrapped;
		return;
	}
	OBJECT(current_object)->animate_hud.anim_timer = OBJECT(current_object)->animate_hud.anim_delay;
	palettes = OBJECT(current_object)->animate_hud.anim;
	last_frame = (uint8_t)mem_read16(palettes);
	list_index = OBJECT(current_object)->animate_hud.frame;
	list_index = (uint8_t)(list_index + 1u);
	palettes += 2u;
	if ((int8_t)last_frame < (int8_t)list_index)
	{
		wrapped = 0xFFFFFFFFu;
		list_index = 0u;
	}
	OBJECT(current_object)->animate_hud.frame = list_index;
	OBJECT(current_object)->sprite_palette = mem_read16(palettes + list_index * 2u);
	*p_list_index = list_index;
	*p_wrapped = wrapped;
}

void animate_hud_tiles(uint32_t *p_list_index, uint32_t *p_wrapped)
{
	uint32_t list_index = *p_list_index;
	uint8_t last_frame = 0;
	uint32_t frame_list = 0, wrapped = 0;
	uint8_t anim_timer = 0;

	wrapped = 0u;
	anim_timer = OBJECT(current_object)->animate_hud.anim_timer;
	OBJECT(current_object)->animate_hud.anim_timer = anim_timer - 1u;
	if ((int8_t)anim_timer > 1)
	{
		*p_wrapped = wrapped;
		return;
	}
	OBJECT(current_object)->animate_hud.anim_timer = OBJECT(current_object)->animate_hud.anim_delay;
	frame_list = OBJECT(current_object)->animate_hud.anim;
	last_frame = (uint8_t)mem_read16(frame_list);
	list_index = OBJECT(current_object)->animate_hud.frame;
	list_index = (uint8_t)(list_index + 1u);
	frame_list += 2u;
	if ((int8_t)list_index <= (int8_t)last_frame)
		OBJECT(current_object)->sprite_frame = mem_read16(frame_list + list_index * 2u);
	else
	{
		wrapped = 0xFFFFFFFFu;
		list_index = 0u;
	}
	OBJECT(current_object)->animate_hud.frame = list_index;
	*p_list_index = list_index;
	*p_wrapped = wrapped;
}

uint32_t blink_player_hud_message(uint32_t text, uint32_t text_position)
{
	uint32_t player = 0, prompt_position = 0;
	uint8_t hud_blink_timer = 0;

	hud_blink_timer = PLAYER(current_player)->hud_blink_timer;
	PLAYER(current_player)->hud_blink_timer = hud_blink_timer - 1u;
	if ((int8_t)hud_blink_timer > 1)
		return text_position;
	PLAYER(current_player)->hud_blink_timer = 0x1E;
	PLAYER(current_player)->hud_blink_on = ~PLAYER(current_player)->hud_blink_on;
	player = OBJECT(current_object)->player;
	prompt_position = 0x310000u | hud_prompt_positions_3p[player];
	if ((cabinet_config & 1u) == 0u)
		prompt_position = (prompt_position & ~0xFFFFu) | hud_prompt_positions_2p[player];
	if (PLAYER(current_player)->hud_blink_on == 0u)
	{
		erase_text_lines(&text, &prompt_position);
		return prompt_position;
	}
	print_text_lines(&text, &prompt_position);
	return prompt_position;
}

void draw_player_stage_rings(void)
{
	uint16_t attribute = 0;
	uint32_t text_position = 0, value = 0;

	if (PLAYER(current_player)->stage_rings_shown != PLAYER(current_player)->stage_rings)
	{
		PLAYER(current_player)->stage_rings_shown = PLAYER(current_player)->stage_rings;
		text_position = 0x310000u | hud_stage_rings_positions[OBJECT(current_object)->player];
		attribute = 0u;
		value = PLAYER(current_player)->stage_rings_shown;
		draw_hex_2(value, attribute, text_position);
	}
}
