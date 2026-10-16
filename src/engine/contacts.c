/**
 *    Name:        engine/contacts.c
 *    Description: contacts between objects and players: the zones, the hitter, touchable, rider, platform and wall lists, and the checks that pair them
 *    Version:     0.1.0
 *    Created:     12-10-2026
 *    Modified:    15-10-2026
 *    Author:      Sebastian Toyos
 *
 *    SegaSonic The Hedgehog (arcade, Sega System 32): the game's V60 program decompiled into C by
 *    tools/decompile.py, with the names of symbols.txt.
 */
#include "game.h"

uint32_t check_zones(uint32_t box_edge)
{
	uint16_t box_start = 0;
	uint32_t hitter = 0, object = 0;

	hitter = unk_20F016_ADDRESS;
	object = unk_20F016_ADDRESS;
	if ((int16_t)zone_list < 0)
	{
		zone_list = 0xFFFFu;
		return box_edge;
	}
	while ((int16_t)OBJECT(hitter)->hit_list_next >= 0)
	{
		hitter = LINKED(hitter, OBJECT(hitter)->hit_list_next);
		object = LINKED(object, zone_list);
		for (;;)
		{
			box_edge = (box_edge & ~0xFFFFu) | (uint16_t)OBJECT(object)->hitbox_offset_x;
			box_start = (uint16_t)(OBJECT(object)->x_int + box_edge);
			if (OBJECT(hitter)->x_int < (int16_t)box_start || (int16_t)(box_start + OBJECT(object)->hitbox_size_x) < OBJECT(hitter)->x_int)
			{
				if ((int16_t)OBJECT(object)->hit_list_next < 0)
					break;
				object = LINKED(object, OBJECT(object)->hit_list_next);
				continue;
			}
			box_edge = (box_edge & ~0xFFFFu) | (uint16_t)OBJECT(object)->hitbox_offset_y;
			box_start = (uint16_t)(OBJECT(object)->y_int + box_edge);
			if (OBJECT(hitter)->y_int < (int16_t)box_start || (int16_t)(box_start + OBJECT(object)->hitbox_size_y) < OBJECT(hitter)->y_int)
			{
				if ((int16_t)OBJECT(object)->hit_list_next < 0)
					break;
				object = LINKED(object, OBJECT(object)->hit_list_next);
				continue;
			}
			box_edge = (box_edge & ~0xFFFFu) | (uint16_t)OBJECT(object)->z_int;
			box_edge = (box_edge & ~0xFFFFu) | (uint16_t)(box_edge - OBJECT(object)->hitbox_height);
			if (OBJECT(hitter)->z_int < (int16_t)box_edge)
			{
				if ((int16_t)OBJECT(object)->hit_list_next < 0)
					break;
				object = LINKED(object, OBJECT(object)->hit_list_next);
				continue;
			}
			box_edge = (box_edge & ~0xFFFFu) | (uint16_t)OBJECT(hitter)->z_int;
			box_edge = (box_edge & ~0xFFFFu) | (uint16_t)(box_edge - OBJECT(hitter)->hitbox_height);
			if (OBJECT(object)->z_int < (int16_t)box_edge)
			{
				if ((int16_t)OBJECT(object)->hit_list_next < 0)
					break;
				object = LINKED(object, OBJECT(object)->hit_list_next);
				continue;
			}
			OBJECT(object)->flags |= 0x8000000u;
			OBJECT(hitter)->flags |= 0x8000000u;
			OBJECT(hitter)->anchor_x = object;
			OBJECT(object)->anchor_x = hitter;
			break;
		}
	}
	zone_list = 0xFFFFu;
	return box_edge;
}

uint32_t add_to_zone_list(uint32_t old_head)
{
	old_head = LINKED(old_head, zone_list);
	zone_list = current_object;
	OBJECT(current_object)->hit_list_next = old_head;
	OBJECT(current_object)->flags &= ~0x8000000u;
	return old_head;
}

void check_hits(uint32_t *p_box_edge, uint32_t *p_distance, uint32_t *p_r3, uint32_t *p_r4, uint32_t *p_radius, uint32_t *p_centre_offset, uint32_t *p_previous)
{
	uint32_t box_edge = *p_box_edge;
	uint32_t distance = *p_distance;
	uint32_t r3 = *p_r3;
	uint32_t r4 = *p_r4;
	uint32_t radius = *p_radius;
	uint32_t centre_offset = *p_centre_offset;
	uint32_t previous = *p_previous;
	uint32_t object = 0, other = 0;

	if ((int16_t)hitter_list < 0)
	{
		hitter_list = 0xFFFFu;
		touchable_list = 0xFFFFu;
		return;
	}
	other = 0x200000u;
	previous = 0x200000u;
	object = LINKED(WORK_RAM, hitter_list);
	do
	{
		if ((int16_t)OBJECT(object)->hit_list_next < 0)
		{
		check_hits_against_touchables:
			other = LINKED(other, touchable_list);
			if ((int16_t)other >= 0)
			{
				previous = input_ports_ADDRESS;
				check_hit(other, object, &box_edge, &distance, &r3, &r4, &radius, &centre_offset, &previous);
			}
		}
		else
		{
			other = LINKED(other, OBJECT(object)->hit_list_next);
			previous = (previous & ~0xFFFFu) | (uint16_t)object;
			check_hit(other, object, &box_edge, &distance, &r3, &r4, &radius, &centre_offset, &previous);
			if ((OBJECT(object)->flags & 4u) == 0u)
				goto check_hits_against_touchables;
		}
		object = LINKED(object, OBJECT(object)->hit_list_next);
	} while ((int16_t)object >= 0);
	hitter_list = 0xFFFFu;
	touchable_list = 0xFFFFu;
	*p_box_edge = box_edge;
	*p_distance = (*p_distance & 0xFFFF0000u) | (distance & 0xFFFFu);
	*p_r3 = (*p_r3 & 0xFFFF0000u) | (r3 & 0xFFFFu);
	*p_r4 = (*p_r4 & 0xFFFF0000u) | (r4 & 0xFFFFu);
	*p_radius = (*p_radius & 0xFFFF0000u) | (radius & 0xFFFFu);
	*p_centre_offset = (*p_centre_offset & 0xFFFF0000u) | (centre_offset & 0xFFFFu);
	*p_previous = previous;
}

void check_hit(uint32_t other, uint32_t hitter, uint32_t *p_box_edge, uint32_t *p_distance, uint32_t *p_r3, uint32_t *p_r4, uint32_t *p_radius, uint32_t *p_centre_offset, uint32_t *p_previous)
{
	uint32_t box_edge = *p_box_edge;
	uint16_t distance = (uint16_t)*p_distance;
	uint16_t r3 = (uint16_t)*p_r3;
	uint16_t r4 = (uint16_t)*p_r4;
	uint16_t radius = (uint16_t)*p_radius;
	uint16_t centre_offset = (uint16_t)*p_centre_offset;
	uint16_t previous = (uint16_t)*p_previous;
	uint16_t box_start = 0, diameter = 0;
	uint32_t floor_slope = 0, quadrant = 0, slope_factor = 0, slope_kind = 0, slope_record = 0;

	for (;;)
	{
		if ((int8_t)OBJECT(other)->kind <= 2 && (int8_t)OBJECT(hitter)->kind <= 2)
		{
		check_hit_next_pair:
			if ((int16_t)OBJECT(other)->hit_list_next < 0)
			{
				*p_box_edge = box_edge;
				*p_distance = (*p_distance & 0xFFFF0000u) | distance;
				*p_r3 = (*p_r3 & 0xFFFF0000u) | r3;
				*p_r4 = (*p_r4 & 0xFFFF0000u) | r4;
				*p_radius = (*p_radius & 0xFFFF0000u) | radius;
				*p_centre_offset = (*p_centre_offset & 0xFFFF0000u) | centre_offset;
				*p_previous = (*p_previous & 0xFFFF0000u) | previous;
				return;
			}
			previous = (uint16_t)other;
			other = LINKED(other, OBJECT(other)->hit_list_next);
			continue;
		}
		if ((OBJECT(hitter)->flags & 2u) == 0u)
		{
			if ((OBJECT(other)->flags & 2u) == 0u)
			{
				radius = (uint16_t)(OBJECT(hitter)->hitbox_size_x + OBJECT(other)->hitbox_size_x);
				diameter = (uint16_t)(radius * 2u + 1u);
				centre_offset = (uint16_t)(int8_t)AS8(OBJECT(hitter)->hitbox_size_y);
				r3 = (uint16_t)(OBJECT(hitter)->x_int + centre_offset);
				distance = (uint16_t)(OBJECT(other)->x_int + (int8_t)AS8(OBJECT(other)->hitbox_size_y) - r3);
				if (diameter <= (uint16_t)(distance + radius))
					goto check_hit_next_pair;
				centre_offset = (uint16_t)(int8_t)AT8(OBJECT(hitter)->hitbox_size_y, 1);
				r4 = (uint16_t)(OBJECT(hitter)->y_int + centre_offset);
				r3 = (uint16_t)(OBJECT(other)->y_int + (int8_t)AT8(OBJECT(other)->hitbox_size_y, 1) - r4);
				if (diameter <= (uint16_t)(r3 + radius))
					goto check_hit_next_pair;
				if (OBJECT(hitter)->floor_slope != 0u)
				{
					floor_slope = OBJECT(hitter)->floor_slope;
					slope_record = floor_slopes_ADDRESS + (uint16_t)(floor_slope * 2u) * 4u;
					slope_kind = ROM8(slope_record) * 2u;
					slope_record++;
					switch (slope_kind)
					{
					case 0x0u:
					case 0x2u:
					case 0x8u:
						box_edge = (int16_t)r3;
						break;
					case 0x4u:
					case 0x6u:
						box_edge = (int16_t)distance;
						break;
					default:
						unknown_code(0x1FDEu + slope_kind);
					}
					if ((int16_t)box_edge < 0)
						box_edge = 0u - box_edge;
					slope_factor = ROM32(slope_record + 1u);
					if ((int32_t)slope_factor < 0)
						slope_factor = 0u - slope_factor;
					box_edge = (((box_edge * slope_factor) >> 8) * 0xB5u) >> 16;
					quadrant = 0u;
					if ((int16_t)distance < 0)
						quadrant = (uint16_t)(quadrant + 2u);
					if ((int16_t)r3 < 0)
						quadrant = (uint16_t)(quadrant + 1u);
					switch (quadrant)
					{
					case 0:
					check_hit_add_slope_to_dx:
						distance += box_edge;
						r3 -= box_edge;
						break;
					case 1:
					case 4:
						if ((int16_t)r3 < (int16_t)distance)
							goto check_hit_add_slope_to_dx;
					case 2:
					check_hit_sub_slope_from_dx:
						distance -= box_edge;
						r3 += box_edge;
						break;
					case 3:
						if ((int16_t)r3 <= (int16_t)distance)
							goto check_hit_add_slope_to_dx;
						goto check_hit_sub_slope_from_dx;
					default:
						unknown_code(0x202Eu + quadrant * 2u);
					}
				}
				r3 = (uint16_t)((uint32_t)r3 * r3);
				distance = (uint16_t)((uint32_t)distance * distance + r3);
				radius = (uint16_t)((uint32_t)radius * radius);
				if ((int16_t)radius < (int16_t)distance)
					goto check_hit_next_pair;
			}
			else
			{
				box_edge = (box_edge & ~0xFFFFu) | (uint16_t)OBJECT(other)->hitbox_offset_x;
				box_start = (uint16_t)(OBJECT(other)->x_int + box_edge);
				if (OBJECT(hitter)->x_int < (int16_t)box_start || (int16_t)(box_start + OBJECT(other)->hitbox_size_x) < OBJECT(hitter)->x_int)
					goto check_hit_next_pair;
				box_edge = (box_edge & ~0xFFFFu) | (uint16_t)OBJECT(other)->hitbox_offset_y;
				box_start = (uint16_t)(OBJECT(other)->y_int + box_edge);
				if (OBJECT(hitter)->y_int < (int16_t)box_start || (int16_t)(box_start + OBJECT(other)->hitbox_size_y) < OBJECT(hitter)->y_int)
					goto check_hit_next_pair;
			}
		}
		else if ((OBJECT(other)->flags & 2u) == 0u)
		{
			r3 = (uint16_t)OBJECT(hitter)->hitbox_offset_x;
			distance = (uint16_t)(OBJECT(hitter)->x_int + r3);
			if (OBJECT(other)->x_int < (int16_t)distance)
			{
				box_edge = (box_edge & ~0xFFFFu) | (uint16_t)OBJECT(other)->x_int;
				box_edge = (box_edge & ~0xFFFFu) | (uint16_t)(box_edge + OBJECT(other)->hitbox_size_x);
				if ((int16_t)box_edge < (int16_t)distance)
					goto check_hit_next_pair;
			}
			distance += OBJECT(hitter)->hitbox_size_x;
			if ((int16_t)distance < OBJECT(other)->x_int)
			{
				box_edge = (box_edge & ~0xFFFFu) | (uint16_t)OBJECT(other)->x_int;
				box_edge = (box_edge & ~0xFFFFu) | (uint16_t)(box_edge - OBJECT(other)->hitbox_size_x);
				if ((int16_t)distance < (int16_t)box_edge)
					goto check_hit_next_pair;
			}
			r3 = (uint16_t)OBJECT(hitter)->hitbox_offset_y;
			distance = (uint16_t)(OBJECT(hitter)->y_int + r3);
			if (OBJECT(other)->y_int < (int16_t)distance)
			{
				box_edge = (box_edge & ~0xFFFFu) | (uint16_t)OBJECT(other)->y_int;
				box_edge = (box_edge & ~0xFFFFu) | (uint16_t)(box_edge + OBJECT(other)->hitbox_size_x);
				if ((int16_t)box_edge < (int16_t)distance)
					goto check_hit_next_pair;
			}
			distance += OBJECT(hitter)->hitbox_size_y;
			if ((int16_t)distance < OBJECT(other)->y_int)
			{
				box_edge = (box_edge & ~0xFFFFu) | (uint16_t)OBJECT(other)->y_int;
				box_edge = (box_edge & ~0xFFFFu) | (uint16_t)(box_edge - OBJECT(other)->hitbox_size_x);
				if ((int16_t)distance < (int16_t)box_edge)
					goto check_hit_next_pair;
			}
		}
		else
		{
			r3 = (uint16_t)OBJECT(hitter)->hitbox_offset_x;
			distance = (uint16_t)(OBJECT(hitter)->x_int + r3);
			radius = (uint16_t)OBJECT(other)->hitbox_offset_x;
			r4 = (uint16_t)(OBJECT(other)->x_int + radius);
			if ((int16_t)(distance + OBJECT(hitter)->hitbox_size_x) < (int16_t)r4)
				goto check_hit_next_pair;
			r4 += OBJECT(other)->hitbox_size_x;
			if ((int16_t)r4 < (int16_t)distance)
				goto check_hit_next_pair;
			r3 = (uint16_t)OBJECT(hitter)->hitbox_offset_y;
			distance = (uint16_t)(OBJECT(hitter)->y_int + r3);
			radius = (uint16_t)OBJECT(other)->hitbox_offset_y;
			r4 = (uint16_t)(OBJECT(other)->y_int + radius);
			if ((int16_t)(distance + OBJECT(hitter)->hitbox_size_y) < (int16_t)r4)
				goto check_hit_next_pair;
			r4 += OBJECT(other)->hitbox_size_y;
			if ((int16_t)r4 < (int16_t)distance)
				goto check_hit_next_pair;
		}
		box_edge = (box_edge & ~0xFFFFu) | (uint16_t)OBJECT(other)->z_int;
		box_edge = (box_edge & ~0xFFFFu) | (uint16_t)(box_edge - OBJECT(other)->hitbox_height);
		if (OBJECT(hitter)->z_int < (int16_t)box_edge)
			goto check_hit_next_pair;
		box_edge = (box_edge & ~0xFFFFu) | (uint16_t)OBJECT(hitter)->z_int;
		box_edge = (box_edge & ~0xFFFFu) | (uint16_t)(box_edge - OBJECT(hitter)->hitbox_height);
		if ((int16_t)box_edge <= OBJECT(other)->z_int)
			break;
		goto check_hit_next_pair;
	}
	OBJECT(other)->flags |= 4u;
	OBJECT(hitter)->flags |= 4u;
	OBJECT(hitter)->touch_distance = distance;
	OBJECT(other)->touch_distance = distance;
	OBJECT(hitter)->touched_by = other;
	OBJECT(other)->touched_by = hitter;
	*p_box_edge = box_edge;
	*p_distance = (*p_distance & 0xFFFF0000u) | distance;
	*p_r3 = (*p_r3 & 0xFFFF0000u) | r3;
	*p_r4 = (*p_r4 & 0xFFFF0000u) | r4;
	*p_radius = (*p_radius & 0xFFFF0000u) | radius;
	*p_centre_offset = (*p_centre_offset & 0xFFFF0000u) | centre_offset;
	*p_previous = (*p_previous & 0xFFFF0000u) | previous;
}

void add_to_hitter_list(void)
{
	uint16_t old_head = 0;

	old_head = hitter_list;
	hitter_list = current_object;
	OBJECT(current_object)->hit_list_next = old_head;
	OBJECT(current_object)->flags &= ~4u;
	OBJECT(current_object)->flags &= ~0x8000000u;
}

uint32_t add_to_touchable_list(uint32_t old_head)
{
	old_head = LINKED(old_head, touchable_list);
	touchable_list = current_object;
	OBJECT(current_object)->hit_list_next = old_head;
	OBJECT(current_object)->flags &= ~4u;
	OBJECT(current_object)->flags &= ~0x40u;
	return old_head;
}

void add_to_rider_list(void)
{
	uint16_t old_head = 0;

	old_head = rider_list;
	rider_list = current_object;
	OBJECT(current_object)->platform_list_next = old_head;
	OBJECT(current_object)->flags &= ~8u;
	OBJECT(current_object)->flags &= ~0x20u;
}

void add_to_platform_list(void)
{
	uint16_t old_head = 0;

	old_head = platform_list;
	platform_list = current_object;
	OBJECT(current_object)->platform_list_next = old_head;
	OBJECT(current_object)->flags &= ~8u;
}

void check_platforms(uint32_t *p_middle, uint32_t *p_plank, uint32_t *p_span, uint32_t *p_sag, uint32_t *p_sag_step, uint32_t *p_load_count, uint32_t *p_passes_left, uint32_t *p_platform, uint32_t *p_load_list)
{
	uint32_t middle = *p_middle;
	uint32_t plank = *p_plank;
	uint32_t span = *p_span;
	uint32_t sag = *p_sag;
	uint32_t sag_step = *p_sag_step;
	uint32_t load_count = *p_load_count;
	uint32_t passes_left = *p_passes_left;
	uint32_t load_list = *p_load_list;
	uint16_t plank_count = 0, x_int = 0, y_int = 0;
	uint32_t r8 = 0, r11 = 0, load_list_2 = 0, object = 0, rider = 0;
	uint32_t flags = 0;

	bridge_0_load = 2u;
	bridge_0_load_first = 0u;
	bridge_0_load_last = AS8(bridge_0_plank_count);
	bridge_1_load = 2u;
	bridge_1_load_first = 0u;
	bridge_1_load_last = AS8(bridge_1_plank_count);
	rider = 0x20F01Eu;
	object = 0x20F01Eu;
	if ((int16_t)platform_list < 0)
	{
		rider_list = 0xFFFFu;
		platform_list = 0xFFFFu;
		*p_platform = object;
		return;
	}
	for (;;)
	{
	check_platforms_next_rider:
		if ((int16_t)OBJECT(rider)->platform_list_next < 0)
			break;
		rider = LINKED(rider, OBJECT(rider)->platform_list_next);
		object = LINKED(object, platform_list);
		for (;;)
		{
			x_int = (uint16_t)OBJECT(object)->x_int;
			if (OBJECT(rider)->x_int < (int16_t)x_int || (int16_t)(x_int + OBJECT(object)->hitbox_size_x) < OBJECT(rider)->x_int)
			{
				if ((int16_t)OBJECT(object)->platform_list_next < 0)
					break;
				object = LINKED(object, OBJECT(object)->platform_list_next);
				continue;
			}
			y_int = (uint16_t)OBJECT(object)->y_int;
			if (OBJECT(rider)->y_int < (int16_t)y_int || (int16_t)(y_int + OBJECT(object)->hitbox_size_y) < OBJECT(rider)->y_int)
			{
				if ((int16_t)OBJECT(object)->platform_list_next < 0)
					break;
				object = LINKED(object, OBJECT(object)->platform_list_next);
				continue;
			}
			goto check_platforms_link_rider;
		}
	}
	if (bridge_0_plank_count != 0u)
	{
		load_list_2 = bridge_0_load_ADDRESS;
		current_player = bridge_0_sag_ADDRESS;
		plank_count = bridge_0_plank_count;
		compute_bridge_sag(load_list_2, plank_count, &middle, &plank, &span, &sag, &sag_step, &load_count, &passes_left, &object, &load_list);
	}
	if (bridge_1_plank_count == 0u)
	{
		rider_list = 0xFFFFu;
		platform_list = 0xFFFFu;
		*p_middle = (*p_middle & 0xFFFF0000u) | (middle & 0xFFFFu);
		*p_plank = plank;
		*p_span = span;
		*p_sag = (*p_sag & 0xFFFF0000u) | (sag & 0xFFFFu);
		*p_sag_step = (*p_sag_step & 0xFFFF0000u) | (sag_step & 0xFFFFu);
		*p_load_count = load_count;
		*p_passes_left = passes_left;
		*p_platform = object;
		*p_load_list = load_list;
		return;
	}
	load_list_2 = bridge_1_load_ADDRESS;
	current_player = bridge_1_sag_ADDRESS;
	plank_count = bridge_1_plank_count;
	compute_bridge_sag(load_list_2, plank_count, &middle, &plank, &span, &sag, &sag_step, &r8, &passes_left, &object, &r11);
	rider_list = 0xFFFFu;
	platform_list = 0xFFFFu;
	*p_middle = (*p_middle & 0xFFFF0000u) | (middle & 0xFFFFu);
	*p_plank = plank;
	*p_span = span;
	*p_sag = (*p_sag & 0xFFFF0000u) | (sag & 0xFFFFu);
	*p_sag_step = (*p_sag_step & 0xFFFF0000u) | (sag_step & 0xFFFFu);
	*p_load_count = r8;
	*p_passes_left = passes_left;
	*p_platform = object;
	*p_load_list = r11;
	return;
check_platforms_link_rider:
	OBJECT(rider)->flags |= 8u;
	OBJECT(rider)->platform = object;
	OBJECT(object)->platform = rider;
	if (OBJECT(rider)->z_int < OBJECT(object)->floor_z_int)
		goto check_platforms_next_rider;
	flags = OBJECT(object)->flags;
	OBJECT(object)->flags = flags | 8u;
	if ((flags & 8u) != 0u)
		goto check_platforms_next_rider;
	load_list = bridge_0_load_ADDRESS;
	if (AT8(OBJECT(object)->spawn_arg_2, 1) != 0u)
		load_list = bridge_1_load_ADDRESS;
	mem_write8(load_list, mem_read8(load_list) + 1u);
	load_count = mem_read8(load_list);
	mem_write8(load_list + load_count, AT8(OBJECT(object)->speed, 1));
	goto check_platforms_next_rider;
}

void compute_bridge_sag(uint32_t load_list, uint16_t plank_count, uint32_t *p_middle, uint32_t *p_plank, uint32_t *p_span, uint32_t *p_sag, uint32_t *p_sag_step, uint32_t *p_count, uint32_t *p_passes_left, uint32_t *p_load_entry, uint32_t *p_sag_curve)
{
	uint16_t middle = (uint16_t)*p_middle;
	uint32_t plank = *p_plank;
	uint32_t span = *p_span;
	uint16_t sag = (uint16_t)*p_sag;
	uint16_t sag_step = (uint16_t)*p_sag_step;
	uint32_t passes_left = *p_passes_left;
	uint32_t load_entry = *p_load_entry;
	uint8_t smallest = 0, swapped = 0;
	uint16_t plank_total = 0;
	uint32_t clear_dest = 0, compares_left = 0, count = 0, curve_index = 0, fill_word = 0, next_curve_index = 0, sag_cursor = 0, sag_curve = 0, scan = 0, slot = 0;

	if (mem_read8(load_list) == 2u)
	{
		count = ((uint16_t)plank_count >> 1) + 1u;
		clear_dest = current_player;
		fill_word = 0u;
		do
		{
			mem_write32(clear_dest, fill_word);
			count--;
			clear_dest += 4u;
		} while (count != 0u);
		*p_count = count;
		*p_sag_curve = clear_dest;
		return;
	}
	slot = load_list;
	passes_left = mem_read8(slot);
	passes_left = (uint8_t)(passes_left - 1u);
	slot++;
	do
	{
		compares_left = passes_left;
		smallest = mem_read8(slot);
		scan = slot + 1u;
		do
		{
			if ((int8_t)mem_read8(scan) < (int8_t)smallest)
			{
				swapped = (uint8_t)smallest;
				smallest = mem_read8(scan);
				mem_write8(scan, swapped);
			}
			scan++;
			compares_left--;
		} while (compares_left != 0u);
		mem_write8(slot, smallest);
		passes_left--;
		slot++;
	} while (passes_left != 0u);
	sag_cursor = current_player;
	load_entry = load_list;
	count = mem_read8(load_entry);
	count = (uint8_t)(count - 1u);
	sag_curve = bridge_sag_curve_ADDRESS;
	load_entry++;
	do
	{
		plank = mem_read8(load_entry);
		curve_index = plank;
		middle = (uint16_t)plank_count;
		plank_total = (uint16_t)middle;
		middle = middle >> 1;
		load_entry++;
		if ((int16_t)middle < (int16_t)curve_index)
		{
			curve_index = (uint16_t)(0u - curve_index);
			curve_index = (uint16_t)(curve_index + plank_total);
		}
		sag = ROM8(sag_curve + curve_index);
		span = mem_read8(load_entry);
		next_curve_index = span;
		if ((int16_t)middle < (int16_t)next_curve_index)
		{
			next_curve_index = (uint16_t)(0u - next_curve_index);
			next_curve_index = (uint16_t)(next_curve_index + plank_total);
		}
		sag = (uint16_t)((uint32_t)sag << 8);
		span = (uint16_t)(span - plank);
		sag_step = divide_s16((uint16_t)(((uint32_t)ROM8(sag_curve + next_curve_index) << 8) - sag), span);
		do
		{
			mem_write16(sag_cursor, sag);
			sag += sag_step;
			span--;
			sag_cursor += 2u;
		} while (span != 0u);
		count--;
	} while (count != 0u);
	*p_middle = (*p_middle & 0xFFFF0000u) | middle;
	*p_plank = plank;
	*p_span = span;
	*p_sag = (*p_sag & 0xFFFF0000u) | sag;
	*p_sag_step = (*p_sag_step & 0xFFFF0000u) | sag_step;
	*p_count = count;
	*p_passes_left = passes_left;
	*p_load_entry = load_entry;
	*p_sag_curve = sag_curve;
}

void add_to_wall_list(void)
{
	uint16_t old_head = 0;

	old_head = wall_list;
	wall_list = current_object;
	OBJECT(current_object)->platform_list_next = old_head;
	OBJECT(current_object)->flags &= ~0x20u;
}

uint32_t check_walls(uint32_t hitbox_offset)
{
	uint16_t box_start = 0;
	uint32_t object = 0, rider = 0;

	rider = 0x20F01Eu;
	object = 0x20F01Eu;
	if ((int16_t)wall_list < 0)
	{
		wall_list = 0xFFFFu;
		return hitbox_offset;
	}
	while ((int16_t)OBJECT(rider)->platform_list_next >= 0)
	{
		rider = LINKED(rider, OBJECT(rider)->platform_list_next);
		object = LINKED(object, wall_list);
		for (;;)
		{
			hitbox_offset = (hitbox_offset & ~0xFFFFu) | (uint16_t)OBJECT(object)->hitbox_offset_x;
			box_start = (uint16_t)(OBJECT(object)->x_int + hitbox_offset);
			if (OBJECT(rider)->x_int < (int16_t)box_start || (int16_t)(box_start + OBJECT(object)->hitbox_size_x) < OBJECT(rider)->x_int)
			{
				if ((int16_t)OBJECT(object)->platform_list_next < 0)
					break;
				object = LINKED(object, OBJECT(object)->platform_list_next);
				continue;
			}
			hitbox_offset = (hitbox_offset & ~0xFFFFu) | (uint16_t)OBJECT(object)->hitbox_offset_y;
			box_start = (uint16_t)(OBJECT(object)->y_int + hitbox_offset);
			if (OBJECT(rider)->y_int < (int16_t)box_start || (int16_t)(box_start + OBJECT(object)->hitbox_size_y) < OBJECT(rider)->y_int || OBJECT(object)->hitbox_height <= (uint16_t)(OBJECT(object)->z_int - OBJECT(rider)->z_int))
			{
				if ((int16_t)OBJECT(object)->platform_list_next < 0)
					break;
				object = LINKED(object, OBJECT(object)->platform_list_next);
				continue;
			}
			OBJECT(rider)->flags |= 0x20u;
			OBJECT(object)->flags |= 0x20u;
			OBJECT(rider)->wall = object;
			OBJECT(object)->wall = rider;
			break;
		}
	}
	wall_list = 0xFFFFu;
	return hitbox_offset;
}
