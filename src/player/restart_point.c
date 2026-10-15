/**
 *    Name:        player/restart_point.c
 *    Description: the player placed at the stage's restart point and the unprojection it uses; aiming at the screen centre, the hurt flash; before them the palette steps of signs
 *    Version:     0.1.0
 *    Created:     12-10-2026
 *    Modified:    15-10-2026
 *    Author:      Sebastian Toyos
 *
 *    SegaSonic The Hedgehog (arcade, Sega System 32): the game's V60 program decompiled into C by
 *    tools/decompile.py, with the names of symbols.txt.
 */
#include "game.h"

void animate_sign(void)
{
	uint8_t last_index = 0;
	uint32_t next_index = 0, palettes = 0;
	uint8_t palette_timer = 0;

	palette_timer = OBJECT(current_object)->animate_sign.palette_timer;
	OBJECT(current_object)->animate_sign.palette_timer = palette_timer - 1u;
	if ((int8_t)palette_timer > 1)
		return;
	OBJECT(current_object)->animate_sign.palette_timer = OBJECT(current_object)->animate_sign.palette_delay;
	palettes = OBJECT(current_object)->animate_sign.palette_list;
	last_index = (uint8_t)mem_read16(palettes);
	next_index = OBJECT(current_object)->animate_sign.palette_index;
	next_index = (uint8_t)(next_index + 1u);
	palettes += 2u;
	if ((int8_t)last_index <= (int8_t)next_index && (int8_t)last_index < (int8_t)next_index)
		next_index = 0u;
	OBJECT(current_object)->animate_sign.palette_index = next_index;
	OBJECT(current_object)->sprite_palette = mem_read16(palettes + next_index * 2u);
}

void cycle_part_palettes(void)
{
	uint8_t last_index = 0;
	uint16_t palette = 0;
	uint32_t next_index = 0, palettes = 0, part = 0, parts_left = 0;
	uint8_t palette_timer = 0;

	palette_timer = OBJECT(current_object)->cycle_part_palettes.palette_timer;
	OBJECT(current_object)->cycle_part_palettes.palette_timer = palette_timer - 1u;
	if ((int8_t)palette_timer > 1)
		return;
	OBJECT(current_object)->cycle_part_palettes.palette_timer = OBJECT(current_object)->cycle_part_palettes.palette_delay;
	palettes = OBJECT(current_object)->cycle_part_palettes.palette_list;
	last_index = (uint8_t)mem_read16(palettes);
	next_index = OBJECT(current_object)->cycle_part_palettes.palette_index;
	next_index = (uint8_t)(next_index + 1u);
	palettes += 2u;
	if ((int8_t)last_index <= (int8_t)next_index && (int8_t)last_index < (int8_t)next_index)
		next_index = 0u;
	OBJECT(current_object)->cycle_part_palettes.palette_index = next_index;
	palette = mem_read16(palettes + next_index * 2u);
	part = OBJECT(current_object)->part_list;
	parts_left = mem_read16(part);
	part += 2u;
	do
	{
		mem_write16(part + 6u, palette);
		part += 8u;
		parts_left--;
	} while (parts_left != 0u);
}

uint32_t place_player_at_restart_point(void)
{
	uint8_t offset_shift = 0;
	uint32_t kind_offset = 0, player_offset = 0, record = 0, restart_x = 0, restart_y = 0, spacing_step = 0, stage_index = 0, z = 0;

	if ((OBJECT(current_object)->flags & 0x2000000u) != 0u && game_in_progress != 0u && current_game_mode != 1u)
	{
		stage_index = stage;
		record = stage_restart_points_ADDRESS + stage_restart_points[stage_index] - 6u;
		offset_shift = (uint8_t)ROM16(record);
		record += 2u;
		OBJECT(current_object)->x_int = ROM16(record);
		player_offset = OBJECT(current_object)->player;
		player_offset = shift_signed_by(player_offset, (uint8_t)offset_shift, 16);
		OBJECT(current_object)->x_int += player_offset;
		OBJECT(current_object)->x_fraction = 0u;
		OBJECT(current_object)->y_int = ROM16(record + 2u);
		OBJECT(current_object)->y_fraction = 0u;
		return player_offset;
	}
	record = current_restart_point + 2u;
	kind_offset = ROM8(record) * 3u;
	record++;
	switch (kind_offset)
	{
	case 0x0u:
	case 0x6u:
		record++;
		OBJECT(current_object)->x_int = ROM16(record);
		player_offset = OBJECT(current_object)->player;
		record += 2u;
		if (player_offset == 2u)
			player_offset = 0xFFFFu;
		player_offset = (uint16_t)(player_offset << 1);
		OBJECT(current_object)->x_int += player_offset;
		OBJECT(current_object)->x_fraction = 0u;
		OBJECT(current_object)->y_int = ROM16(record);
		OBJECT(current_object)->y_fraction = 0u;
		return player_offset;
	case 0x3u:
		restart_y = ROM8(record);
		record++;
		restart_x = ROM16(record);
		z = (uint32_t)ROM16(record + 2u) << 16;
		screen_point_to_ground(z, &restart_x, &restart_y);
		spacing_step = OBJECT(current_object)->player;
		if (spacing_step == 2u)
			spacing_step = 0xFFFFu;
		spacing_step = spacing_step << 17;
		OBJECT(current_object)->x = restart_x + spacing_step;
		OBJECT(current_object)->y = restart_y;
		return spacing_step;
	default:
		unknown_code(0xCC13u + kind_offset);
	}
}

void screen_point_to_ground(uint32_t z, uint32_t *p_point_x, uint32_t *p_point_y)
{
	uint32_t point_x = *p_point_x;
	uint32_t point_y = *p_point_y;

	point_x = (uint32_t)(uint16_t)(point_x + view_x) << 16;
	point_y = (((uint32_t)(uint16_t)(point_y + view_y) << 16) - z) * 2u;
	unproject_point(&point_x, &point_y);
	*p_point_x = point_x;
	*p_point_y = point_y;
}

void unproject_point(uint32_t *p_point_x, uint32_t *p_point_y)
{
	uint32_t point_x = *p_point_x;
	uint32_t point_y = *p_point_y;
	uint32_t new_x = 0;

	new_x = ((int32_t)(point_x - point_y) >> 8) * 0xB5u;
	point_y = ((int32_t)(point_y + point_x) >> 8) * 0xB5u;
	point_x = new_x;
	*p_point_x = point_x;
	*p_point_y = point_y;
}

void aim_at_screen_centre(uint32_t *p_cos_part, uint32_t *p_dx, uint32_t *p_dy)
{
	uint16_t centre_dy = 0;
	uint32_t centre_angle = 0, centre_x = 0, centre_y = 0, cos_part = 0, dx = 0, dy = 0, ignored = 0, z = 0;

	centre_x = 0xA0u;
	centre_y = 0x70u;
	z = OBJECT(current_object)->z;
	screen_point_to_ground(z, &centre_x, &centre_y);
	centre_angle = (int32_t)(centre_x - OBJECT(current_object)->x) >> 12;
	centre_dy = (uint16_t)((int32_t)(centre_y - OBJECT(current_object)->y) >> 12);
	angle_of_vector(centre_dy, &centre_angle, &ignored);
	dx = (uint16_t)centre_angle;
	dy = 0x200u;
	polar_to_xy(&cos_part, &dx, &dy);
	*p_cos_part = cos_part;
	*p_dx = dx;
	*p_dy = dy;
}

void flash_hurt_palette(void)
{
	uint16_t palette = 0;
	uint8_t flash_timer = 0, flashing = 0;

	flash_timer = OBJECT(current_object)->flash_hurt_palette.flash_timer;
	OBJECT(current_object)->flash_hurt_palette.flash_timer = flash_timer - 1u;
	if ((int8_t)flash_timer > 1)
		return;
	OBJECT(current_object)->flash_hurt_palette.flash_timer = OBJECT(current_object)->flash_hurt_palette.flash_period;
	palette = mem_read16(OBJECT(current_object)->flash_hurt_palette.palette_list + 2u);
	flashing = ~OBJECT(current_object)->flash_hurt_palette.flash_on;
	OBJECT(current_object)->flash_hurt_palette.flash_on = flashing;
	if (flashing != 0u)
		palette = 0x83B0u;
	OBJECT(current_object)->sprite_palette = palette;
}
