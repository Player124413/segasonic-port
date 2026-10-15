/**
 *    Name:        objects/fire_vents.c
 *    Description: stage 3's fire vents
 *    Version:     0.1.0
 *    Created:     12-10-2026
 *    Modified:    15-10-2026
 *    Author:      Sebastian Toyos
 *
 *    SegaSonic The Hedgehog (arcade, Sega System 32): the game's V60 program decompiled into C by
 *    tools/decompile.py, with the names of symbols.txt.
 */
#include "game.h"

void fire_vent_spawner_start(void)
{
	fire_vent_cursor = fire_vent_list_ADDRESS;
	OBJECT(current_object)->handler = fire_vent_spawner_update_ADDRESS;
	fire_vent_spawner_update();
}

void fire_vent_spawner_update(void)
{
	uint32_t free_count = 0, object = 0, vent_record = 0;
	uint16_t spawn_position = 0;

	vent_record = fire_vent_cursor;
	spawn_position = ROM16(vent_record);
	vent_record += 2u;
	if (scroll_position < spawn_position)
		return;
	if ((int16_t)ROM16(vent_record + 6u) < 0)
	{
		delete_current_object();
		return;
	}
	allocate_object(&free_count, &object);
	if (free_count != 0u)
	{
		OBJECT(object)->x_int = ROM16(vent_record);
		OBJECT(object)->x_fraction = 0u;
		vent_record += 2u;
		OBJECT(object)->y_int = ROM16(vent_record);
		OBJECT(object)->y_fraction = 0u;
		vent_record += 2u;
		OBJECT(object)->z_int = ROM16(vent_record);
		OBJECT(object)->z_fraction = 0u;
		OBJECT(object)->fire_vent.blast_delay = ROM16(vent_record + 2u);
		OBJECT(object)->sprite_flags = 0x1006u;
		OBJECT(object)->sprite_frame = 0x6A48u;
		OBJECT(object)->sprite_palette = 0x9110u;
		OBJECT(object)->flags = 2u;
		OBJECT(object)->handler = fire_vent_start_ADDRESS;
		OBJECT(object)->fire_vent.burst_state = 0u;
		OBJECT(object)->fire_vent.palette_list = fire_vent_glow_palettes_ADDRESS;
		OBJECT(object)->fire_vent.palette_index = 0u;
		OBJECT(object)->fire_vent.palette_delay = 8u;
		OBJECT(object)->fire_vent.palette_timer = 0u;
		OBJECT(object)->sprite_offset_x = 0;
		OBJECT(object)->sprite_offset_y = 0x10;
		OBJECT(object)->kind = 0x22u;
		OBJECT(object)->hitbox_size_x = 0x90u;
		OBJECT(object)->hitbox_size_y = 0x48u;
		OBJECT(object)->hitbox_height = 0x28u;
		OBJECT(object)->hitbox_offset_x = -0x48;
		OBJECT(object)->hitbox_offset_y = -8;
		fire_vent_cursor += 0xAu;
	}
}

void fire_vent_start(void)
{
	uint32_t ignored = 0;

	project_object(&ignored, &ignored);
	OBJECT(current_object)->handler = fire_vent_update_ADDRESS;
	fire_vent_update();
}

void fire_vent_update(void)
{
	uint32_t zero = 0, negative = 0, overflow = 0;
	uint8_t sound_id = 0;
	uint32_t depth = 0, free_count = 0, ignored = 0, margin = 0, object = 0, off_screen = 0, write_index_address = 0;
	uint16_t blast_delay = 0, next_delay = 0;

	margin = 0x40u;
	off_screen = check_left_screen_margin((uint16_t)margin);
	if (off_screen != 0u)
	{
		delete_current_object();
		return;
	}
	if ((int8_t)OBJECT(current_object)->fire_vent.palette_index >= 0)
	{
		if (OBJECT(current_object)->fire_vent.blast_delay <= 0x1E)
			OBJECT(current_object)->fire_vent.palette_delay = 2u;
		animate_sign();
	}
	if ((uint16_t)OBJECT(current_object)->fire_vent.blast_delay == 0u)
	{
	fire_vent_blast:
		OBJECT(current_object)->fire_vent.blast_delay = -1;
		OBJECT(current_object)->fire_vent.burst_state = 1u;
		OBJECT(current_object)->sprite_frame = 0x6A50u;
		OBJECT(current_object)->sprite_palette = 0x9120u;
		OBJECT(current_object)->fire_vent.palette_index = 0xFFu;
		allocate_object(&free_count, &object);
		if (free_count != 0u)
		{
			OBJECT(object)->sprite_flags = 0x1009u;
			OBJECT(object)->sprite_palette = 0x90B0u;
			OBJECT(object)->sprite_offset_x = 0;
			OBJECT(object)->sprite_offset_y = 0;
			OBJECT(object)->flags = 0u;
			OBJECT(object)->handler = fire_vent_burst_animate_ADDRESS;
			OBJECT(object)->parent = current_object;
			OBJECT(object)->animation = fire_vent_blast_animation_ADDRESS;
			OBJECT(object)->animation_frame = 0u;
			OBJECT(object)->animation_speed = 3u;
			OBJECT(object)->animation_timer = 0;
			OBJECT(object)->projected_x = OBJECT(current_object)->projected_x;
			OBJECT(object)->projected_x += 0x3Au;
			OBJECT(object)->anchor_x = OBJECT(object)->projected_x;
			OBJECT(object)->projected_y = OBJECT(current_object)->projected_y;
			OBJECT(object)->projected_y_int -= 0x31u;
			OBJECT(object)->anchor_y = OBJECT(object)->projected_y_int;
			OBJECT(object)->z = OBJECT(current_object)->z;
			sound_id = 0xAu;
			play_sound(sound_id, &margin, &write_index_address);
		}
	}
	else if (OBJECT(current_object)->screen_y >= 0x18)
	{
		blast_delay = OBJECT(current_object)->fire_vent.blast_delay;
		next_delay = blast_delay - 1u;
		overflow = blast_delay == 0x8000u;
		zero = next_delay == 0u;
		negative = next_delay >> 15;
		OBJECT(current_object)->fire_vent.blast_delay = next_delay;
		if (blast_delay == 1u)
			goto fire_vent_blast;
		if ((negative ^ overflow) | zero)
			OBJECT(current_object)->fire_vent.blast_delay = -1;
	}
	update_screen_position(&ignored, &margin);
	if (OBJECT(current_object)->fire_vent.burst_state == 0u)
	{
		depth = (uint16_t)(OBJECT(current_object)->screen_y - 0x48u);
		queue_sprite_at_depth(&ignored, &depth);
		return;
	}
	if ((int8_t)OBJECT(current_object)->fire_vent.burst_state < 0)
	{
		OBJECT(current_object)->fire_vent.burst_state = 1u;
		OBJECT(current_object)->kind = 0xCu;
	}
	add_to_touchable_list(ignored);
	depth = (uint16_t)(OBJECT(current_object)->screen_y - 0x48u);
	queue_sprite_at_depth(&ignored, &depth);
}

void fire_vent_burst_animate(void)
{
	uint32_t r1 = 0, r7 = 0, at_last_step = 0, depth = 0, ignored = 0;

	animate(&r1, &r7, &at_last_step);
	if ((uint8_t)OBJECT(current_object)->animation_timer == OBJECT(current_object)->animation_speed)
		OBJECT(current_object)->sprite_palette = fire_vent_blast_palettes[OBJECT(current_object)->animation_frame];
	if ((uint8_t)at_last_step == 0u)
	{
		update_screen_position(&ignored, &r1);
		depth = (uint16_t)(OBJECT(current_object)->screen_y + 0x68u);
		queue_sprite_at_depth(&ignored, &depth);
		return;
	}
	AS8(OBJECT(LINKED(current_object, OBJECT(current_object)->parent))->var_88) = 0xFFu;
	delete_current_object();
}
