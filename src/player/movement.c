/**
 *    Name:        player/movement.c
 *    Description: the player's movement: kept on screen, the camera speeds, trackball speed and steering, turning and facing, ledges, jumps, walls, blinking
 *    Version:     0.1.0
 *    Created:     12-10-2026
 *    Modified:    15-10-2026
 *    Author:      Sebastian Toyos
 *
 *    SegaSonic The Hedgehog (arcade, Sega System 32): the game's V60 program decompiled into C by
 *    tools/decompile.py, with the names of symbols.txt.
 */
#include "game.h"

uint32_t keep_player_on_screen(uint32_t depth)
{
	uint8_t sound_id = 0;
	uint16_t base_x = 0, base_y = 0, edge = 0, scroll_direction = 0;
	uint32_t direction_row = 0, floor_slope = 0, offset_x = 0, slope_direction = 0, slope_factor = 0, slope_speed = 0, voice_range = 0;
	uint8_t health = 0;

	if (nearby_floor_blocks_locked != 0u || (OBJECT(current_object)->flags & 0x4000000u) != 0u)
		return depth;
	offset_x = (depth & ~0xFFFFu) | (uint16_t)OBJECT(current_object)->sprite_offset_x;
	base_x = (uint16_t)(OBJECT(current_object)->screen_x - offset_x);
	depth = (offset_x & ~0xFFFFu) | (uint16_t)OBJECT(current_object)->sprite_offset_y;
	base_y = (uint16_t)(OBJECT(current_object)->screen_y - depth);
	if ((stage != 8u || scroll_position >= 0x4000u) && ((uint16_t)(base_x + 0x10u) >= 0x160u || (uint16_t)(base_y + 0xE0u) >= 0x1E0u))
	{
		if (stage == 5u)
			OBJECT(current_object)->handler = player_respawn_ADDRESS;
		else
		{
			health = OBJECT(current_object)->health;
			OBJECT(current_object)->health = health - 8u;
			if ((int8_t)health > 8)
			{
				OBJECT(current_object)->handler = player_respawn_ADDRESS;
				return depth;
			}
			OBJECT(current_object)->health = 0;
			PLAYER(current_player)->state = 0x16u;
			PLAYER(current_player)->substate = 0u;
		}
	}
	else
	{
		if ((OBJECT(current_object)->flags & 0x1000u) != 0u || PLAYER(current_player)->ignore_screen_edges != 0u || OBJECT(current_object)->floor_z_int >= 0x7FF)
			return depth;
		if ((int16_t)base_x > 0x10)
		{
			if ((int16_t)base_x < 0x128)
			{
				if (PLAYER(current_player)->state == 4u)
					base_y -= 9u;
				base_y += OBJECT(current_object)->floor_z_int - OBJECT(current_object)->z_int;
				if ((int16_t)base_y > 0x20)
				{
					if ((int16_t)base_y < 0xD0)
						return depth;
					edge = 2u;
				}
				else
					edge = 0u;
			}
			else
				edge = 1u;
		}
		else
			edge = 3u;
		PLAYER(current_player)->screen_edge = edge;
		if ((PLAYER(current_player)->flags & 0x20u) == 0u)
		{
			voice_range = edge_voice_scroll_ranges_ADDRESS + stage * 4u;
			if ((uint16_t)(scroll_position - ROM16(voice_range)) < ROM16(voice_range + 2u) && players_in_play >= 2)
			{
				sound_id = 0x39u;
				if (OBJECT(current_object)->kind != 0u)
				{
					sound_id = 0x43u;
					if (OBJECT(current_object)->kind != 1u)
						sound_id = 0x4Cu;
				}
				play_sound(sound_id, &depth, &slope_speed);
				PLAYER(current_player)->flags |= 0x20u;
			}
		}
		slope_factor = 0u;
		if ((OBJECT(current_object)->floor_slope | OBJECT(current_object)->floor_type) != 0u)
		{
			floor_slope = OBJECT(current_object)->floor_slope;
			slope_factor = floor_slope_factors[(uint16_t)(floor_slope * 2u)];
		}
		compute_camera_slope_speed(0u, slope_factor);
		if (camera_slope_speed == 0u)
			return push_player_from_screen_edge();
		scroll_direction = camera_scroll_direction;
		direction_row = 0u;
		if ((uint8_t)scroll_direction != 0u)
			direction_row = (uint16_t)(direction_row + 4u);
		direction_row = (uint16_t)(direction_row + (scroll_direction >> 8));
		if ((int8_t)ROM8((uint16_t)(direction_row << 2) + screen_edge_scroll_carry_flags_ADDRESS + PLAYER(current_player)->screen_edge) >= 0)
			return push_player_from_screen_edge();
		push_player_from_screen_edge();
		slope_direction = (int16_t)camera_slope_direction;
		slope_speed = camera_slope_speed >> 14;
		OBJECT(current_object)->x += (int32_t)(sine_table[slope_direction] * slope_speed) >> 8;
		PLAYER(current_player)->previous_x = OBJECT(current_object)->x;
		depth = (int32_t)(cosine_table[slope_direction] * slope_speed) >> 8;
		OBJECT(current_object)->y -= depth;
		PLAYER(current_player)->previous_y = OBJECT(current_object)->y;
	}
	return depth;
}

uint32_t push_player_from_screen_edge(void)
{
	uint16_t relative_direction = 0;
	uint32_t r1 = 0, push_record = 0, push_x = 0, push_y = 0;

	push_record = screen_edge_push_directions_ADDRESS + PLAYER(current_player)->screen_edge * 4u;
	relative_direction = (OBJECT(current_object)->direction - ROM16(push_record)) & 0x3FFu;
	push_record += 2u;
	if (relative_direction >= 0x101u)
	{
		OBJECT(current_object)->x = PLAYER(current_player)->previous_x;
		OBJECT(current_object)->y = PLAYER(current_player)->previous_y;
		OBJECT(current_object)->floor_block = OBJECT(current_object)->previous_floor_block;
		OBJECT(current_object)->floor_z = OBJECT(current_object)->previous_floor_z;
	}
	push_x = ROM16(push_record);
	push_y = 0x200u;
	polar_to_xy(&r1, &push_x, &push_y);
	OBJECT(current_object)->x += push_x;
	OBJECT(current_object)->y += push_y;
	PLAYER(current_player)->previous_x = OBJECT(current_object)->x;
	PLAYER(current_player)->previous_y = OBJECT(current_object)->y;
	return r1;
}

uint32_t compute_camera_ground_speed(uint32_t octant)
{
	uint16_t dy = 0;
	uint32_t ground_angle = 0, ignored = 0, map_angle = 0, vx = 0, vy = 0;

	if (camera_ground_speed_vx != (uint32_t)camera_velocity_x || camera_ground_speed_vy != (uint32_t)camera_velocity_y)
	{
		vx = (int32_t)camera_velocity_x >> 5;
		vy = (int32_t)camera_velocity_y >> 4;
		vx = vx * vx + vy * vy;
		square_root(&vx, &ignored);
		camera_ground_speed = (vx & 0xFFFFu) << 13;
		ground_angle = (int32_t)camera_velocity_x >> 9;
		dy = (uint16_t)((int32_t)camera_velocity_y >> 8);
		angle_of_vector(dy, &ground_angle, &ignored);
		camera_ground_direction = (ground_angle + 0x80u) & 0x3FFu;
		map_angle = (int32_t)camera_velocity_x >> 8;
		dy = (uint16_t)((int32_t)camera_velocity_y >> 8);
		angle_of_vector(dy, &map_angle, &octant);
		camera_map_direction = map_angle;
		camera_ground_speed_vx = camera_velocity_x;
		camera_ground_speed_vy = camera_velocity_y;
	}
	return octant;
}

uint32_t compute_camera_slope_speed(uint32_t octant, uint32_t slope_factor)
{
	uint16_t dy = 0;
	uint32_t ignored = 0, scroll_angle = 0, slope_angle = 0, vx = 0, vy = 0;

	if (((uint32_t)camera_velocity_x != 0u || (uint32_t)camera_velocity_y != 0u || camera_slope_speed == 0u) && camera_slope_factor == (uint16_t)slope_factor && camera_slope_speed_vx == (uint32_t)camera_velocity_x && camera_slope_speed_vy == (uint32_t)camera_velocity_y)
		return octant;
	camera_slope_factor = slope_factor;
	if (slope_factor == 0u)
	{
		vx = (int32_t)camera_velocity_x >> 5;
		vy = (int32_t)camera_velocity_y >> 4;
		vx = vx * vx + vy * vy;
		square_root(&vx, &ignored);
		camera_slope_speed = (vx & 0xFFFFu) << 13;
		slope_angle = (int32_t)camera_velocity_x >> 9;
		dy = (uint16_t)((int32_t)camera_velocity_y >> 8);
		angle_of_vector(dy, &slope_angle, &ignored);
		camera_slope_direction = (slope_angle + 0x80u) & 0x3FFu;
	}
	else
	{
		if ((int32_t)slope_factor < 0)
			slope_factor = 0u - slope_factor;
		vy = divide_s32((uint32_t)camera_velocity_y << 11, slope_factor + 0x10000u);
		slope_velocity_y = vy;
		vx = (int32_t)camera_velocity_x >> 5;
		vx = vx * vx + vy * vy;
		square_root(&vx, &ignored);
		camera_slope_speed = (vx & 0xFFFFu) << 13;
		slope_angle = (int32_t)camera_velocity_x >> 5;
		dy = (uint16_t)slope_velocity_y;
		angle_of_vector(dy, &slope_angle, &ignored);
		camera_slope_direction = (slope_angle + 0x80u) & 0x3FFu;
	}
	scroll_angle = (int32_t)camera_velocity_x >> 5;
	dy = (uint16_t)((int32_t)camera_velocity_y >> 5);
	angle_of_vector(dy, &scroll_angle, &octant);
	camera_scroll_direction = scroll_angle;
	camera_slope_speed_vx = camera_velocity_x;
	camera_slope_speed_vy = camera_velocity_y;
	return octant;
}

void compute_scroll_drift(uint32_t block, uint8_t drift_byte)
{
	uint16_t dy = 0;
	uint32_t divisor = 0, drift_angle = 0, ignored = 0, scroll_angle = 0, slope = 0, vx = 0, vy = 0;

	if (scroll_drift_block == block && scroll_drift_vx == (uint32_t)camera_velocity_x && scroll_drift_vy == (uint32_t)camera_velocity_y)
		return;
	unused_drift_byte = drift_byte;
	scroll_drift_block = block;
	if (mem_read8(block + 0x13u) == 0u)
	{
		vx = (int32_t)camera_velocity_x >> 5;
		vy = (int32_t)camera_velocity_y >> 4;
		vx = vx * vx + vy * vy;
		square_root(&vx, &ignored);
		scroll_drift_speed = (vx & 0xFFFFu) << 13;
		drift_angle = (int32_t)camera_velocity_x >> 9;
		dy = (uint16_t)((int32_t)camera_velocity_y >> 8);
		angle_of_vector(dy, &drift_angle, &ignored);
		scroll_drift_direction = (drift_angle + 0x80u) & 0x3FFu;
	}
	else
	{
		slope = mem_read8(block + 0x13u);
		divisor = floor_slope_factors[(uint16_t)(slope * 2u)];
		if ((int32_t)divisor < 0)
			divisor = 0u - divisor;
		vy = camera_velocity_y;
		divisor += 0x10000u;
		unused_drift_divisor = divisor;
		vy = divide_s32(vy << 11, divisor);
		vx = (int32_t)camera_velocity_x >> 5;
		vx = vx * vx + vy * vy;
		square_root(&vx, &ignored);
		scroll_drift_speed = (vx & 0xFFFFu) << 13;
		scroll_drift_direction = (uint32_t)ROM8(scroll_drift_block + 0x15u) << 8;
	}
	scroll_angle = (int32_t)camera_velocity_x >> 5;
	dy = (uint16_t)((int32_t)camera_velocity_y >> 5);
	angle_of_vector(dy, &scroll_angle, &ignored);
	camera_scroll_direction = scroll_angle;
	scroll_drift_vx = camera_velocity_x;
	scroll_drift_vy = camera_velocity_y;
}

void slip_effect_update(void)
{
	uint8_t bucket = 0;
	uint32_t r1 = 0, r7 = 0, r8 = 0, ignored = 0;

	animate(&r1, &r7, &r8);
	if (OBJECT(current_object)->animation_frame == 0u)
	{
		delete_current_object();
		return;
	}
	update_screen_position(&ignored, &r1);
	if ((int16_t)OBJECT(current_object)->direction >= 2)
	{
		bucket = 0u;
		queue_sprite_in_bucket(bucket);
		return;
	}
	bucket = 0xFFu;
	queue_sprite_in_bucket(bucket);
}

uint32_t set_speed_from_trackball(void)
{
	uint16_t trackball_y_2 = 0;
	uint32_t ignored = 0, speed = 0, trackball_angle = 0, trackball_y = 0;

	read_player_trackball();
	speed = PLAYER(current_player)->trackball_x;
	trackball_y = PLAYER(current_player)->trackball_y;
	speed = speed * speed + trackball_y * trackball_y;
	square_root(&speed, &ignored);
	speed = (uint16_t)speed;
	speed = speed * speed;
	if ((int32_t)speed > 0x5000000)
		speed = 0x5000000u;
	OBJECT(current_object)->speed = speed;
	trackball_angle = (uint16_t)PLAYER(current_player)->trackball_x;
	trackball_y_2 = (uint16_t)PLAYER(current_player)->trackball_y;
	angle_of_vector(trackball_y_2, &trackball_angle, &ignored);
	OBJECT(current_object)->direction = (trackball_angle + 0x80u) & 0x3FFu;
	return set_speed_from_direction();
}

uint32_t set_climb_speed_from_trackball(void)
{
	uint16_t trackball_y_2 = 0;
	uint32_t ignored = 0, speed = 0, trackball_angle = 0, trackball_y = 0;

	read_player_trackball();
	speed = PLAYER(current_player)->trackball_x;
	trackball_y = PLAYER(current_player)->trackball_y;
	speed = speed * speed + trackball_y * trackball_y;
	square_root(&speed, &ignored);
	speed = (uint16_t)speed;
	speed = speed * speed;
	if ((int32_t)speed > 0x3000000)
		speed = 0x3000000u;
	OBJECT(current_object)->speed = speed;
	trackball_angle = (uint16_t)PLAYER(current_player)->trackball_x;
	trackball_y_2 = (uint16_t)PLAYER(current_player)->trackball_y;
	angle_of_vector(trackball_y_2, &trackball_angle, &ignored);
	OBJECT(current_object)->direction = trackball_angle & 0x3FFu;
	return set_speed_from_direction();
}

uint32_t update_ledge_pullback(void)
{
	uint32_t r1 = 0;

	r1 = set_speed_from_trackball();
	switch ((uint16_t)PLAYER(current_player)->ledge_side)
	{
	case 0:
	case 5:
		if (OBJECT(current_object)->speed_y_int >= 0)
			PLAYER(current_player)->ledge_pullback += OBJECT(current_object)->speed_y_int;
		break;
	case 1:
		if (OBJECT(current_object)->speed_x_int < 0)
			PLAYER(current_player)->ledge_pullback -= OBJECT(current_object)->speed_x_int;
		break;
	case 2:
		if (OBJECT(current_object)->speed_y_int < 0)
			PLAYER(current_player)->ledge_pullback -= OBJECT(current_object)->speed_y_int;
		break;
	case 3:
		if (OBJECT(current_object)->speed_x_int >= 0)
			PLAYER(current_player)->ledge_pullback += OBJECT(current_object)->speed_x_int;
		break;
	case 4:
		break;
	default:
		unknown_code(0xB36Fu + (uint16_t)PLAYER(current_player)->ledge_side * 2u);
	}
	return r1;
}

void get_ledge_side(uint32_t *p_side, uint32_t *p_block)
{
	uint32_t block = 0, bound_cursor = 0, side = 0;
	uint32_t x_min = 0, x_max = 0, y_min = 0;

	bound_cursor = floor_blocks + OBJECT(current_object)->previous_floor_block + 2u;
	block = bound_cursor;
	side = 3u;
	x_min = mem_read32(bound_cursor);
	bound_cursor += 4u;
	if (OBJECT(current_object)->x < (int32_t)x_min)
	{
		*p_side = side;
		*p_block = block;
		return;
	}
	side = 1u;
	x_max = mem_read32(bound_cursor);
	bound_cursor += 4u;
	if ((int32_t)x_max < OBJECT(current_object)->x)
	{
		*p_side = side;
		*p_block = block;
		return;
	}
	side = 0u;
	y_min = mem_read32(bound_cursor);
	bound_cursor += 4u;
	if (OBJECT(current_object)->y < (int32_t)y_min)
	{
		*p_side = side;
		*p_block = block;
		return;
	}
	side = 2u;
	if ((int32_t)mem_read32(bound_cursor) < OBJECT(current_object)->y)
	{
		*p_side = side;
		*p_block = block;
		return;
	}
	side = 4u;
	*p_side = side;
	*p_block = block;
}

void check_player_floor_edge(uint32_t *p_depth, uint32_t *p_block)
{
	uint32_t depth = *p_depth;
	uint32_t block = *p_block;
	uint8_t next_state = 0;
	uint32_t r1 = 0, block_2 = 0, draw_depth = 0, edge_speed_x = 0, edge_speed_y = 0, side = 0;

	if (PLAYER(current_player)->state == 0x17u)
	{
		check_fall_into_pit();
		*p_depth = depth;
		*p_block = block;
		return;
	}
	if (PLAYER(current_player)->state == 4u)
	{
		if (OBJECT(current_object)->floor_block >= 0)
		{
			check_fall_into_pit();
			return;
		}
		PLAYER(current_player)->previous_state = PLAYER(current_player)->state;
		PLAYER(current_player)->state = 7u;
		PLAYER(current_player)->substate = 1u;
		get_ledge_side(&side, &block);
		if ((uint16_t)side == 4u)
			side = (mem_read8(floor_blocks + 2u + OBJECT(current_object)->previous_floor_block + 0x15u) + 2u) & 3u;
		PLAYER(current_player)->ledge_side = side;
		draw_depth = (int32_t)((uint32_t)OBJECT(current_object)->projected_y + OBJECT(current_object)->previous_floor_z) >> 16;
		draw_depth = (draw_depth & ~0xFFFFu) | (uint16_t)(draw_depth - view_y);
		PLAYER(current_player)->fall_draw_depth = draw_depth;
		if (OBJECT(current_object)->floor_type == 1u)
		{
			if (PLAYER(current_player)->ledge_side < 2)
			{
				draw_depth = (draw_depth & ~0xFFFFu) | OBJECT(LINKED(current_object, OBJECT(current_object)->platform))->var_88;
				draw_depth = (draw_depth & ~0xFFFFu) | (uint16_t)(draw_depth - 1u);
				PLAYER(current_player)->fall_draw_depth = draw_depth;
			}
			PLAYER(current_player)->fall_draw_depth -= 0x10u;
		}
		else if (PLAYER(current_player)->ledge_side < 2)
		{
			if (stage != 0xAu)
			{
				PLAYER(current_player)->fall_draw_depth -= 0x10u;
				OBJECT(current_object)->flags &= ~0x100u;
				if (mem_read8(floor_blocks + 2u + OBJECT(current_object)->previous_floor_block + 0x12u) == 0x10u || OBJECT(current_object)->floor_type == 1u)
				{
					OBJECT(current_object)->flags |= 0x100u;
					PLAYER(current_player)->edge_object = OBJECT(LINKED(current_object, OBJECT(current_object)->platform))->parent;
				}
				PLAYER(current_player)->heading = OBJECT(current_object)->direction;
				PLAYER(current_player)->ledge_pullback = 0;
				PLAYER(current_player)->slip_distance = 0;
				PLAYER(current_player)->control_lock = 0u;
				OBJECT(current_object)->sprite_palette = mem_read16(OBJECT(current_object)->check_player.palette_list + 2u);
				OBJECT(current_object)->flags &= ~0x40u;
				OBJECT(current_object)->flags &= ~0x20000u;
				PLAYER(current_player)->flags |= 0x80u;
				OBJECT(current_object)->flags &= ~0x2000u;
				*p_depth = draw_depth;
				*p_block = block;
				return;
			}
			PLAYER(current_player)->fall_draw_depth -= 0x20u;
			PLAYER(current_player)->fall_draw_depth -= 0x10u;
			OBJECT(current_object)->flags &= ~0x100u;
			if (mem_read8(floor_blocks + 2u + OBJECT(current_object)->previous_floor_block + 0x12u) == 0x10u || OBJECT(current_object)->floor_type == 1u)
			{
				OBJECT(current_object)->flags |= 0x100u;
				PLAYER(current_player)->edge_object = OBJECT(LINKED(current_object, OBJECT(current_object)->platform))->parent;
			}
			PLAYER(current_player)->heading = OBJECT(current_object)->direction;
			PLAYER(current_player)->ledge_pullback = 0;
			PLAYER(current_player)->slip_distance = 0;
			PLAYER(current_player)->control_lock = 0u;
			OBJECT(current_object)->sprite_palette = mem_read16(OBJECT(current_object)->check_player.palette_list + 2u);
			OBJECT(current_object)->flags &= ~0x40u;
			OBJECT(current_object)->flags &= ~0x20000u;
			PLAYER(current_player)->flags |= 0x80u;
			OBJECT(current_object)->flags &= ~0x2000u;
			*p_depth = draw_depth;
			*p_block = block;
			return;
		}
		OBJECT(current_object)->flags &= ~0x100u;
		if (mem_read8(floor_blocks + 2u + OBJECT(current_object)->previous_floor_block + 0x12u) == 0x10u || OBJECT(current_object)->floor_type == 1u)
		{
			OBJECT(current_object)->flags |= 0x100u;
			PLAYER(current_player)->edge_object = OBJECT(LINKED(current_object, OBJECT(current_object)->platform))->parent;
		}
		PLAYER(current_player)->heading = OBJECT(current_object)->direction;
		PLAYER(current_player)->ledge_pullback = 0;
		PLAYER(current_player)->slip_distance = 0;
		PLAYER(current_player)->control_lock = 0u;
		OBJECT(current_object)->sprite_palette = mem_read16(OBJECT(current_object)->check_player.palette_list + 2u);
		OBJECT(current_object)->flags &= ~0x40u;
		OBJECT(current_object)->flags &= ~0x20000u;
		PLAYER(current_player)->flags |= 0x80u;
		OBJECT(current_object)->flags &= ~0x2000u;
		*p_depth = draw_depth;
		*p_block = block;
		return;
	}
	if (PLAYER(current_player)->state == 2u || OBJECT(current_object)->floor_z_int <= 0x7FF)
		return;
	if ((PLAYER(current_player)->flags & 2u) != 0u)
	{
		PLAYER(current_player)->previous_state = PLAYER(current_player)->state;
		PLAYER(current_player)->state = 7u;
		PLAYER(current_player)->substate = 0u;
		if (OBJECT(current_object)->speed_int >= 0x200)
			return;
		edge_speed_x = OBJECT(current_object)->direction;
		edge_speed_y = 0x100u;
		polar_to_xy(&r1, &edge_speed_x, &edge_speed_y);
		OBJECT(current_object)->speed_x = edge_speed_x;
		OBJECT(current_object)->speed_y = edge_speed_y;
		*p_depth = r1;
		return;
	}
	PLAYER(current_player)->substate = 0u;
	next_state = 5u;
	if (OBJECT(current_object)->floor_type != 1u && (OBJECT(current_object)->flags & 0x10u) == 0u)
	{
		block_2 = floor_blocks + 2u + OBJECT(current_object)->previous_floor_block;
		if (mem_read8(block_2 + 0x12u) != 0x12u && mem_read8(block_2 + 0x12u) != 0x23u && mem_read8(block_2 + 0x12u) != 0x18u && mem_read8(block_2 + 0x12u) != 0x19u && OBJECT(current_object)->speed_int > 0x200)
			next_state = 7u;
	}
	PLAYER(current_player)->previous_state = PLAYER(current_player)->state;
	PLAYER(current_player)->state = next_state;
}

void check_fall_into_pit(void)
{
	if (OBJECT(current_object)->speed_z_int >= 0 && OBJECT(current_object)->floor_z_int >= 0x7FF && OBJECT(current_object)->floor_type != 6u && OBJECT(current_object)->previous_floor_z_int <= OBJECT(current_object)->z_int)
	{
		PLAYER(current_player)->substate = 0u;
		PLAYER(current_player)->state = 9u;
	}
}

void add_player_to_lists(void)
{
	add_to_hitter_list();
	add_to_rider_list();
}

void remember_platform(void)
{
	OBJECT(current_object)->flags &= 0xFFFFFFEFu;
	if ((OBJECT(current_object)->flags & 8u) != 0u)
		OBJECT(current_object)->flags |= 0x10u;
}

void update_player_blink(void)
{
	if (PLAYER(current_player)->invulnerable_timer < 0)
	{
		OBJECT(current_object)->flags &= ~0x80000u;
		OBJECT(current_object)->flags &= 0xDFFFFFFFu;
		return;
	}
	if ((OBJECT(current_object)->flags & 0x20000000u) == 0u)
		return;
	toggle_hidden();
}

void toggle_hidden(void)
{
	OBJECT(current_object)->flags ^= 0x80000u;
}

void check_player_jump(void)
{
	uint8_t jump_delay = 0;

	if (players_frozen != 0u)
		return;
	if (OBJECT(current_object)->floor_type == 6u)
	{
		if ((uint8_t)PLAYER(current_player)->jump_delay != 2u)
		{
			PLAYER(current_player)->state = 0x17u;
			PLAYER(current_player)->substate = 0u;
			PLAYER(current_player)->ledge_side = -1;
		}
		return;
	}
	jump_delay = PLAYER(current_player)->jump_delay;
	PLAYER(current_player)->jump_delay = jump_delay - 1u;
	if ((int8_t)jump_delay >= 1)
		return;
	PLAYER(current_player)->jump_delay = 0;
	if (PLAYER(current_player)->state == 2u || PLAYER(current_player)->control_lock != 0u)
		return;
	start_jump_if_pressed();
}

void start_jump_if_pressed(void)
{
	uint32_t button_bit = 0;

	if ((uint8_t)PLAYER(current_player)->pushed_out != 0u || PLAYER(current_player)->path != 0u)
		return;
	if (game_in_progress != 0u)
	{
		button_bit = 0x10u;
		if (OBJECT(current_object)->player != 0u)
		{
			button_bit = 0x18u;
			if (OBJECT(current_object)->player != 1u)
				button_bit = 0x12u;
		}
	}
	else
	{
		button_bit = 0x19u;
		if (OBJECT(current_object)->player != 0u)
		{
			button_bit = 0x1Au;
			if (OBJECT(current_object)->player != 1u)
				button_bit = 0x1Bu;
		}
	}
	if ((inputs_pressed & shl32(1u, (int)(button_bit))) != 0u)
	{
		PLAYER(current_player)->state = 4u;
		PLAYER(current_player)->substate = 0u;
		PLAYER(current_player)->ledge_side = -1;
	}
}

void push_player_out_of_wall(uint32_t *p_angle, uint32_t *p_push_direction, uint32_t *p_record_index)
{
	uint32_t angle = *p_angle;
	uint16_t push_direction = (uint16_t)*p_push_direction;
	uint32_t record_index = *p_record_index;
	uint16_t drift_bounce_direction = 0, new_direction = 0;
	uint32_t rebound_direction = 0;
	uint8_t wall_timer = 0;

	if (PLAYER(current_player)->path_request != 0u || PLAYER(current_player)->path != 0u || all_players_path_request != 0u)
		return;
	if ((OBJECT(current_object)->flags & 0x20u) != 0u)
	{
		if (OBJECT(current_object)->wall != PLAYER(current_player)->last_wall)
		{
			PLAYER(current_player)->wall_timer = 0x1E;
			PLAYER(current_player)->last_wall = OBJECT(current_object)->wall;
		push_player_out_of_wall_push_back:
			if ((stage != 8u || scroll_position >= 0x4000u) && PLAYER(current_player)->state != 0x25u && PLAYER(current_player)->state != 0x26u)
				OBJECT(current_object)->flags |= 0x1000u;
			OBJECT(current_object)->x = PLAYER(current_player)->previous_x;
			OBJECT(current_object)->y = PLAYER(current_player)->previous_y;
			OBJECT(current_object)->floor_block = OBJECT(current_object)->previous_floor_block;
			OBJECT(current_object)->floor_z = OBJECT(current_object)->previous_floor_z;
			PLAYER(current_player)->control_lock = 0u;
		}
		else
		{
			wall_timer = PLAYER(current_player)->wall_timer;
			PLAYER(current_player)->wall_timer = wall_timer - 1u;
			if ((int8_t)wall_timer > 1)
				goto push_player_out_of_wall_push_back;
			PLAYER(current_player)->wall_timer = 0;
		}
		if (PLAYER(current_player)->state == 4u || PLAYER(current_player)->state == 0x17u)
		{
			rebound_direction = (angle & ~0xFFFFu) | OBJECT(LINKED(current_object, OBJECT(current_object)->wall))->direction;
			rebound_direction = (rebound_direction & ~0xFFFFu) | (uint16_t)(rebound_direction << 8);
			rebound_direction = (rebound_direction & ~0xFFFFu) | (uint16_t)(rebound_direction + 0x200u);
			rebound_direction = (rebound_direction & ~0xFFFFu) | (rebound_direction & 0x3FFu);
			OBJECT(current_object)->direction = rebound_direction;
			rebound_direction = face_movement_direction(rebound_direction);
			OBJECT(current_object)->speed = 0x4000000;
			OBJECT(current_object)->speed_z = -0x18000;
			PLAYER(current_player)->state_timer = 6;
			OBJECT(current_object)->flags |= 0x2000u;
			PLAYER(current_player)->control_lock = 0xAu;
			*p_angle = rebound_direction;
			return;
		}
		if (current_game_mode == 0xAu || PLAYER(current_player)->state == 0xDu || PLAYER(current_player)->state == 0x20u)
		{
			angle = 0u;
			OBJECT(current_object)->speed = 0;
			OBJECT(current_object)->speed_x = 0;
			OBJECT(current_object)->speed_y = 0;
			PLAYER(current_player)->free_turn_timer = 3u;
			OBJECT(current_object)->flags |= 0x800000u;
			PLAYER(current_player)->previous_state = PLAYER(current_player)->state;
			PLAYER(current_player)->state = 0u;
			*p_angle = angle;
			return;
		}
		if (PLAYER(current_player)->state == 0x25u || PLAYER(current_player)->state == 0x26u)
		{
			angle = (angle & ~0xFFFFu) | scroll_drift_direction;
			angle = (angle & ~0xFFFFu) | (uint16_t)(angle + 0x200u);
			angle = (angle & ~0xFFFFu) | (angle & 0x3FFu);
			drift_bounce_direction = sonic_bounce_records[AS8(OBJECT(LINKED(current_object, OBJECT(current_object)->wall))->direction) * 3u];
			angle = (angle & ~0xFFFFu) | (uint16_t)(angle - drift_bounce_direction);
			angle = (angle & ~0xFFFFu) | (angle & 0x3FFu);
			if ((int16_t)angle < 0x200)
				drift_bounce_direction -= 0x80u;
			else
				drift_bounce_direction += 0x80u;
			scroll_drift_direction = drift_bounce_direction & 0x3FFu;
		push_player_out_of_wall_deflect:
			angle = (angle & ~0xFFFFu) | OBJECT(current_object)->direction;
			angle = (angle & ~0xFFFFu) | (uint16_t)(angle + 0x200u);
			angle = (angle & ~0xFFFFu) | (angle & 0x3FFu);
			record_index = AS8(OBJECT(LINKED(current_object, OBJECT(current_object)->wall))->direction) * 3u;
			push_direction = sonic_bounce_records[record_index];
			new_direction = (uint16_t)push_direction;
			angle = (angle & ~0xFFFFu) | (uint16_t)(angle - push_direction);
			angle = (angle & ~0xFFFFu) | (angle & 0x3FFu);
			if ((int16_t)angle < 0x200)
				new_direction -= 0x80u;
			else
				new_direction += 0x80u;
			OBJECT(current_object)->direction = new_direction & 0x3FFu;
			PLAYER(current_player)->control_lock = 4u;
			if (stage != 8u || scroll_position >= 0x4000u)
			{
				*p_angle = angle;
				*p_push_direction = (*p_push_direction & 0xFFFF0000u) | push_direction;
				*p_record_index = record_index;
				return;
			}
			angle = (angle & ~0xFFFFu) | OBJECT(LINKED(current_object, OBJECT(current_object)->wall))->direction;
			angle = (angle & ~0xFFFFu) | (uint16_t)(angle << 8);
			angle = (angle & ~0xFFFFu) | (uint16_t)(angle + 0x200u);
			angle = (angle & ~0xFFFFu) | (angle & 0x3FFu);
			OBJECT(current_object)->direction = angle;
			*p_angle = angle;
			*p_push_direction = (*p_push_direction & 0xFFFF0000u) | push_direction;
			*p_record_index = record_index;
			return;
		}
		if (OBJECT(current_object)->floor_type == 4u || OBJECT(current_object)->speed_int >= 0x200)
			goto push_player_out_of_wall_deflect;
		angle = 0u;
		OBJECT(current_object)->speed = 0;
		OBJECT(current_object)->speed_x = 0;
		OBJECT(current_object)->speed_y = 0;
		PLAYER(current_player)->free_turn_timer = 3u;
		OBJECT(current_object)->flags |= 0x800000u;
		PLAYER(current_player)->previous_state = PLAYER(current_player)->state;
		PLAYER(current_player)->state = 0u;
		*p_angle = angle;
		return;
	}
	PLAYER(current_player)->last_wall = 0xFFFFu;
}

void player_steer_in_air(void)
{
	uint32_t ignored = 0, input_speed = 0, speed_diff = 0, trackball_y = 0;
	uint32_t speed = 0, target_speed = 0;

	read_player_trackball();
	input_speed = PLAYER(current_player)->trackball_x;
	trackball_y = PLAYER(current_player)->trackball_y;
	input_speed = input_speed * input_speed + trackball_y * trackball_y;
	square_root(&input_speed, &ignored);
	input_speed &= 0xFFFFu;
	input_speed = input_speed * input_speed;
	if ((int32_t)input_speed > 0x5000000)
		input_speed = 0x5000000u;
	speed = OBJECT(current_object)->speed;
	target_speed = input_speed;
	speed_diff = target_speed - speed;
	if ((int32_t)target_speed <= (int32_t)speed)
		OBJECT(current_object)->speed += (int32_t)speed_diff >> 5;
	else
		OBJECT(current_object)->speed += speed_diff;
	if ((PLAYER(current_player)->trackball_x | PLAYER(current_player)->trackball_y) != 0u && (OBJECT(current_object)->flags & 0x2000u) == 0u)
		set_target_heading_from_trackball();
	turn_player();
}

void player_steer(uint32_t wanted_speed)
{
	uint32_t carry = 0;
	uint8_t next_state = 0;
	uint16_t animation_speed_int = 0;
	uint32_t animation_speed_diff = 0, direction = 0, floor_slope = 0, ignored = 0, input_animation_speed = 0, input_y = 0, rounded_heading = 0, rounded_speed = 0, run_threshold = 0, slope_pair = 0, speed_diff = 0, speed_factor = 0, trackball_y = 0;
	uint32_t speed = 0, target_speed = 0, speed_2 = 0, target_speed_2 = 0, animation_speed = 0, target_animation_speed = 0;

	wanted_speed = follow_player_path(wanted_speed);
	if (PLAYER(current_player)->path != 0u)
	{
	player_steer_approach_speed:
		wanted_speed = apply_speed_scale(wanted_speed);
		if (OBJECT(current_object)->floor_type == 7u && PLAYER(current_player)->path == 0u)
		{
			speed = OBJECT(current_object)->speed;
			target_speed = wanted_speed;
			speed_diff = target_speed - speed;
			if ((int32_t)target_speed <= (int32_t)speed)
				OBJECT(current_object)->speed += (int32_t)speed_diff >> 6;
			else
				OBJECT(current_object)->speed += (int32_t)speed_diff >> 5;
		}
		else
		{
			speed_2 = OBJECT(current_object)->speed;
			target_speed_2 = wanted_speed;
			speed_diff = target_speed_2 - speed_2;
			if ((int32_t)target_speed_2 <= (int32_t)speed_2)
				OBJECT(current_object)->speed += (int32_t)speed_diff >> 5;
			else
				OBJECT(current_object)->speed += speed_diff;
		}
		if (OBJECT(current_object)->floor_type == 7u || (PLAYER(current_player)->push_x | PLAYER(current_player)->push_y) != 0u || PLAYER(current_player)->speed_scale != 0u)
		{
			if ((PLAYER(current_player)->trackball_x | PLAYER(current_player)->trackball_y) != 0u || (OBJECT(current_object)->floor_type != 7u && (int16_t)AT16(PLAYER(current_player)->animation_speed, 2) >= 0x20))
			{
				input_animation_speed = PLAYER(current_player)->trackball_x;
				trackball_y = PLAYER(current_player)->trackball_y;
				input_animation_speed = input_animation_speed * input_animation_speed + trackball_y * trackball_y;
				square_root(&input_animation_speed, &ignored);
				input_animation_speed &= 0xFFFFu;
				input_animation_speed = input_animation_speed * input_animation_speed;
				input_animation_speed += input_animation_speed >> 1;
				if ((int32_t)input_animation_speed > 0x5000000)
					input_animation_speed = 0x5000000u;
				animation_speed = PLAYER(current_player)->animation_speed;
				target_animation_speed = input_animation_speed;
				animation_speed_diff = target_animation_speed - animation_speed;
				if ((int32_t)target_animation_speed <= (int32_t)animation_speed)
					PLAYER(current_player)->animation_speed += (int32_t)animation_speed_diff >> 5;
				else
					PLAYER(current_player)->animation_speed += animation_speed_diff;
			}
			else
				PLAYER(current_player)->animation_speed = 0;
		}
		else
			PLAYER(current_player)->animation_speed = OBJECT(current_object)->speed;
		if (PLAYER(current_player)->state != 4u && PLAYER(current_player)->state != 0x17u && PLAYER(current_player)->state != 0x25u && PLAYER(current_player)->state != 0x26u && PLAYER(current_player)->state != 0x2Du)
		{
			next_state = 0u;
			animation_speed_int = AT16(PLAYER(current_player)->animation_speed, 2);
			if (animation_speed_int != 0u)
			{
				next_state = 1u;
				run_threshold = 0x200u;
				if (OBJECT(current_object)->floor_type == 4u)
					run_threshold = (run_threshold * 0x60u) >> 8;
				if ((int16_t)run_threshold < (int16_t)animation_speed_int)
				{
					next_state = 0x13u;
					if (OBJECT(current_object)->floor_type != 4u && (int16_t)animation_speed_int > 0x4F0)
						next_state = 0x14u;
				}
			}
			PLAYER(current_player)->previous_state = PLAYER(current_player)->state;
			PLAYER(current_player)->state = next_state;
		}
	}
	else
	{
		read_player_trackball();
		if (players_frozen != 0u)
		{
			PLAYER(current_player)->trackball_x = 0;
			PLAYER(current_player)->trackball_y = 0;
		}
		if (PLAYER(current_player)->state == 4u || PLAYER(current_player)->state == 0x17u || PLAYER(current_player)->state == 0x25u || PLAYER(current_player)->state == 0x26u || PLAYER(current_player)->state == 0x2Du || (PLAYER(current_player)->trackball_x | PLAYER(current_player)->trackball_y) != 0u || (PLAYER(current_player)->push_x | PLAYER(current_player)->push_y) != 0u)
		{
			if ((OBJECT(current_object)->flags & 0x2000u) == 0u)
				set_target_heading_from_trackball();
		player_steer_speed_from_trackball:
			wanted_speed = (uint32_t)PLAYER(current_player)->trackball_x + PLAYER(current_player)->push_x;
			input_y = (uint32_t)PLAYER(current_player)->trackball_y + PLAYER(current_player)->push_y;
			wanted_speed = wanted_speed * wanted_speed + input_y * input_y;
			square_root(&wanted_speed, &ignored);
			wanted_speed &= 0xFFFFu;
			wanted_speed = wanted_speed * wanted_speed;
			wanted_speed += wanted_speed >> 1;
			if ((int32_t)wanted_speed > 0x5000000)
				wanted_speed = 0x5000000u;
			if ((OBJECT(current_object)->flags & 0x80000000u) != 0u)
			{
				wanted_speed = wanted_speed >> 2;
				if ((PLAYER(current_player)->flags & 4u) != 0u)
				{
					wanted_speed = 0u;
					OBJECT(current_object)->speed = 0;
				}
			}
			if (OBJECT(current_object)->floor_block >= 0 && OBJECT(current_object)->floor_type == 0xAu)
			{
				PLAYER(current_player)->struggle_speed = wanted_speed >> 16;
				wanted_speed = (wanted_speed >> 10) * (uint16_t)(0x100u - ((uint16_t)PLAYER(current_player)->sink_depth >> 8) * 6u);
			}
			if (OBJECT(current_object)->floor_type != 4u)
				goto player_steer_approach_speed;
			wanted_speed = (wanted_speed >> 8) * 0x60u;
			goto player_steer_approach_speed;
		}
		if (OBJECT(current_object)->speed_int >= 0x20)
			goto player_steer_speed_from_trackball;
		OBJECT(current_object)->speed = 0;
		PLAYER(current_player)->previous_state = PLAYER(current_player)->state;
		PLAYER(current_player)->state = 0u;
	}
	turn_player();
	if ((OBJECT(current_object)->flags & 0x80000000u) == 0u)
	{
		rounded_heading = (uint16_t)(PLAYER(current_player)->heading + 0x20u) & 0x3FFu;
		PLAYER(current_player)->ground_speed = ((uint32_t)OBJECT(current_object)->speed >> 8) * heading_speed_factors[rounded_heading >> 6];
	}
	else
		PLAYER(current_player)->ground_speed = OBJECT(current_object)->speed;
	if (OBJECT(current_object)->floor_slope == 0u)
		return;
	floor_slope = OBJECT(current_object)->floor_slope;
	direction = OBJECT(current_object)->direction;
	slope_pair = slope_speed_direction_pairs_ADDRESS + floor_slope_direction_tables[(uint16_t)(floor_slope << 2)] + (direction >> 6) * 4u;
	speed_factor = ROM16(slope_pair);
	rounded_speed = PLAYER(current_player)->ground_speed;
	carry = (rounded_speed >> 11) & 1u;
	rounded_speed = rounded_speed >> 12;
	slope_pair += 2u;
	if (carry)
		rounded_speed++;
	PLAYER(current_player)->ground_speed = rounded_speed * speed_factor;
	OBJECT(current_object)->direction = ROM16(slope_pair);
}

void set_target_heading_from_trackball(void)
{
	uint16_t dy = 0;
	uint32_t ignored = 0, push_angle = 0, trackball_angle = 0, trackball_y = 0;

	if ((PLAYER(current_player)->trackball_x | PLAYER(current_player)->trackball_y) != 0u)
	{
		trackball_angle = (uint16_t)PLAYER(current_player)->trackball_x;
		dy = (uint16_t)((int32_t)(PLAYER(current_player)->trackball_y * 0x180u) >> 8);
		angle_of_vector(dy, &trackball_angle, &ignored);
		PLAYER(current_player)->facing_angle = trackball_angle;
	}
	if ((PLAYER(current_player)->push_x | PLAYER(current_player)->push_y) == 0u)
	{
		PLAYER(current_player)->target_heading = PLAYER(current_player)->facing_angle;
		return;
	}
	push_angle = (uint16_t)(PLAYER(current_player)->trackball_x + PLAYER(current_player)->push_x);
	trackball_y = PLAYER(current_player)->trackball_y;
	if ((OBJECT(current_object)->flags & 0x80000000u) == 0u)
		trackball_y = (int32_t)(trackball_y * 0x180u) >> 8;
	trackball_y = (uint16_t)(trackball_y + PLAYER(current_player)->push_y);
	angle_of_vector(trackball_y, &push_angle, &ignored);
	PLAYER(current_player)->target_heading = push_angle;
}

void turn_player(void)
{
	uint16_t max_turn = 0, turn = 0;
	uint32_t speed_int = 0;
	uint16_t heading_delta = 0;

	if (PLAYER(current_player)->free_turn_timer != 0u || PLAYER(current_player)->state == 2u || PLAYER(current_player)->state == 4u || PLAYER(current_player)->state == 0x17u || PLAYER(current_player)->state == 0x25u || PLAYER(current_player)->state == 0x26u || (OBJECT(current_object)->flags & 0x80000000u) != 0u)
		PLAYER(current_player)->heading = PLAYER(current_player)->target_heading;
	else
	{
		speed_int = (uint16_t)OBJECT(current_object)->speed_int;
		if ((int16_t)speed_int <= 0x200)
		{
			PLAYER(current_player)->heading = PLAYER(current_player)->target_heading;
			PLAYER(current_player)->heading &= 0x3FFu;
			set_heading_direction();
			return;
		}
		max_turn = (uint16_t)divide_s32(0x180000u, speed_int);
		heading_delta = ((uint32_t)PLAYER(current_player)->target_heading << 6) - ((uint32_t)PLAYER(current_player)->heading << 6);
		turn = heading_delta;
		if ((int16_t)heading_delta < 0)
		{
			turn = (uint16_t)(0u - turn);
			if (turn >= 0x4000u)
			{
				PLAYER(current_player)->substate = 0u;
				PLAYER(current_player)->previous_state = PLAYER(current_player)->state;
				PLAYER(current_player)->state = 0xCu;
				set_heading_direction();
				return;
			}
			if ((int16_t)max_turn < (int16_t)turn)
				turn = (uint16_t)max_turn;
			PLAYER(current_player)->heading -= turn >> 6;
		}
		else
		{
			if (turn >= 0x4000u)
			{
				PLAYER(current_player)->substate = 0u;
				PLAYER(current_player)->previous_state = PLAYER(current_player)->state;
				PLAYER(current_player)->state = 0xCu;
				set_heading_direction();
				return;
			}
			if ((int16_t)max_turn < (int16_t)turn)
				turn = (uint16_t)max_turn;
			PLAYER(current_player)->heading += turn >> 6;
		}
	}
	PLAYER(current_player)->heading &= 0x3FFu;
	set_heading_direction();
}

void set_heading_direction(void)
{
	uint32_t snapped_heading = 0;

	snapped_heading = (PLAYER(current_player)->heading + 0x20u) & 0x3C0u;
	PLAYER(current_player)->heading_snapped = snapped_heading;
	OBJECT(current_object)->direction = (snapped_heading + 0x80u) & 0x3C0u;
	snapped_heading = set_facing(snapped_heading);
}

uint32_t set_facing(uint32_t facing)
{
	uint32_t new_facing = 0;

	new_facing = (facing & ~0xFFFFu) | PLAYER(current_player)->facing_angle;
	new_facing = (new_facing & ~0xFFFFu) | (uint16_t)(new_facing + 0x20u);
	new_facing = (new_facing & ~0xFFFFu) | ((uint16_t)new_facing >> 6);
	new_facing = (new_facing & ~0xFFFFu) | (new_facing & 0xFu);
	PLAYER(current_player)->facing = new_facing;
	return new_facing;
}

uint32_t face_movement_direction(uint32_t facing)
{
	uint32_t new_heading = 0;

	new_heading = (facing & ~0xFFFFu) | OBJECT(current_object)->direction;
	new_heading = (new_heading & ~0xFFFFu) | (uint16_t)(new_heading - 0xA0u);
	new_heading = (new_heading & ~0xFFFFu) | (new_heading & 0x3FFu);
	PLAYER(current_player)->target_heading = new_heading;
	PLAYER(current_player)->heading = new_heading;
	PLAYER(current_player)->facing_angle = new_heading;
	return set_facing(new_heading);
}

uint32_t compute_ground_speed(void)
{
	uint32_t carry = 0;
	uint32_t r1 = 0, direction = 0, floor_slope = 0, rounded_heading = 0, rounded_speed = 0, slope_pair = 0, speed_factor = 0;

	rounded_heading = (uint16_t)(PLAYER(current_player)->heading + 0x20u) & 0x3FFu;
	r1 = heading_speed_factors[rounded_heading >> 6];
	PLAYER(current_player)->ground_speed = ((uint32_t)OBJECT(current_object)->speed >> 8) * r1;
	if (OBJECT(current_object)->floor_slope == 0u)
		return r1;
	floor_slope = OBJECT(current_object)->floor_slope;
	r1 = floor_slope_direction_tables[(uint16_t)(floor_slope << 2)];
	direction = OBJECT(current_object)->direction;
	slope_pair = slope_speed_direction_pairs_ADDRESS + r1 + (direction >> 6) * 4u;
	speed_factor = ROM16(slope_pair);
	rounded_speed = PLAYER(current_player)->ground_speed;
	carry = (rounded_speed >> 11) & 1u;
	rounded_speed = rounded_speed >> 12;
	slope_pair += 2u;
	if (carry)
		rounded_speed++;
	PLAYER(current_player)->ground_speed = rounded_speed * speed_factor;
	OBJECT(current_object)->direction = ROM16(slope_pair);
	return r1;
}

void read_player_trackball(void)
{
	uint16_t axis_value = 0;
	uint32_t trackball = 0;

	trackball = trackball_1_x_ADDRESS + OBJECT(current_object)->player * 4u;
	axis_value = mem_read16(trackball);
	trackball += 2u;
	if ((int16_t)axis_value >= 0)
		axis_value = (uint16_t)((axis_value & ~1u) << 8);
	else
		axis_value = (uint16_t)(0u - (((0u - axis_value) & 0xFFFEu) << 8));
	PLAYER(current_player)->trackball_x = axis_value;
	axis_value = mem_read16(trackball);
	if ((int16_t)axis_value >= 0)
		axis_value = (uint16_t)((axis_value & ~1u) << 8);
	else
		axis_value = (uint16_t)(0u - (((0u - axis_value) & 0xFFFEu) << 8));
	PLAYER(current_player)->trackball_y = axis_value;
}
