/**
 *    Name:        objects/stage_0a.c
 *    Description: stage A's objects, the sand: the cactus, the centipede, the wall pieces, the shaking blocks, the sand rock, the sand arch, the stepped wall, the tornado and the whirlwinds it throws, quicksand, sinkholes, the collapsing sand blocks
 *    Version:     0.1.0
 *    Created:     12-10-2026
 *    Modified:    16-10-2026
 *    Author:      Sebastian Toyos
 *
 *    SegaSonic The Hedgehog (arcade, Sega System 32): the game's V60 program decompiled into C by
 *    tools/decompile.py, with the names of symbols.txt.
 */
#include "game.h"

void cactus_init(void)
{
	OBJECT(current_object)->sprite_frame = 0x77D8u;
	OBJECT(current_object)->sprite_palette = 0x9070u;
	OBJECT(current_object)->handler = cactus_update_ADDRESS;
	if (OBJECT(current_object)->spawn_arg_1 != 0u)
		OBJECT(current_object)->sprite_flags |= 0x40u;
	current_object = LINKED(current_object, OBJECT(current_object)->child);
	OBJECT(current_object)->sprite_frame = 0x5D80u;
	OBJECT(current_object)->sprite_palette = 0x87A0u;
	OBJECT(current_object)->sprite_scale = 0x60u;
}

void cactus_update(void)
{
	uint32_t bucket = 0, deleted = 0, direction = 0, ignored = 0, speed_x = 0, speed_y = 0, spin_speed = 0;

	move_object_projected(&ignored, &ignored, &ignored, &ignored);
	delete_if_off_screen(&deleted, &ignored, &ignored);
	if ((uint8_t)deleted == 1u)
		return;
	if (OBJECT(current_object)->cactus.lifted != 0u)
	{
		OBJECT(current_object)->cactus.angle -= 0x10u;
		OBJECT(current_object)->cactus.angle &= 0x3FFu;
		OBJECT(current_object)->cactus.spiral_speed += 0x18u;
		direction = OBJECT(current_object)->cactus.angle;
		spin_speed = OBJECT(current_object)->cactus.spiral_speed;
		direction_to_speed(spin_speed, &direction, &speed_x, &speed_y);
		OBJECT(current_object)->speed_x = speed_x;
		OBJECT(current_object)->speed_y = speed_y;
		OBJECT(current_object)->speed_z -= 0xC00u;
	}
	else
	{
		r14 = tornado_object;
		distance_to_object();
		if (nearest_player_distance <= 0x60)
		{
			OBJECT(current_object)->cactus.lifted = 1u;
			OBJECT(current_object)->sprite_frame = 0x77E0u;
			OBJECT(current_object)->speed_z = -0xC001;
			OBJECT(current_object)->cactus.angle -= 0x10u;
			OBJECT(current_object)->cactus.angle &= 0x3FFu;
			OBJECT(current_object)->cactus.spiral_speed += 0x18u;
			direction = OBJECT(current_object)->cactus.angle;
			spin_speed = OBJECT(current_object)->cactus.spiral_speed;
			direction_to_speed(spin_speed, &direction, &speed_x, &speed_y);
			OBJECT(current_object)->speed_x = speed_x;
			OBJECT(current_object)->speed_y = speed_y;
			OBJECT(current_object)->speed_z -= 0xC00u;
			queue_sprite_at_floor();
			current_object = LINKED(current_object, OBJECT(current_object)->child);
			bucket = place_sprite_at_cursor_floor();
			queue_sprite_in_bucket(bucket);
			return;
		}
		fall_to_floor(&ignored, &ignored, &ignored);
		if (nearest_player_distance <= 0x60 && (uint16_t)OBJECT(current_object)->floor_z_int == (uint16_t)OBJECT(current_object)->z_int)
			OBJECT(current_object)->z_int -= 3u;
	}
	queue_sprite_at_floor();
	current_object = LINKED(current_object, OBJECT(current_object)->child);
	bucket = place_sprite_at_cursor_floor();
	queue_sprite_in_bucket(bucket);
}

uint32_t spawn_whirlwind(uint32_t spawned, uint32_t free_count, uint32_t whirlwind, uint32_t object)
{
	uint32_t ignored = 0;

	spawn_count = 1u;
	spawn_object_with_pieces(0x01217Cu, &spawned, &ignored, &free_count, &whirlwind, &object);
	if ((uint8_t)spawned != 0u)
	{
		current_object = whirlwind;
		copy_position_to(whirlwind);
		whirlwind_spin_step += 0xBu;
		whirlwind_spin_step &= 0x3Fu;
		OBJECT(current_object)->sprite_palette = 0x90F0u;
		OBJECT(current_object)->whirlwind.spin_step = whirlwind_spin_step;
		OBJECT(current_object)->whirlwind.self_copied_byte = OBJECT(current_object)->whirlwind.self_copied_byte;
		OBJECT(current_object)->hitbox_size_x = 8u;
		OBJECT(current_object)->hitbox_height = 0x28u;
		OBJECT(current_object)->kind = 0x1Fu;
		OBJECT(current_object)->handler = whirlwind_aim_ADDRESS;
	}
	return object;
}

void whirlwind_aim(void)
{
	uint16_t farthest_distance = 0;

	face_nearest_player(0u, 0u);
	distance_to_player_1++;
	distance_to_player_2++;
	distance_to_player_3++;
	farthest_distance = (uint16_t)distance_to_player_1;
	OBJECT(current_object)->direction = direction_to_player_1;
	if ((int16_t)farthest_distance < distance_to_player_2)
	{
		farthest_distance = (uint16_t)(distance_to_player_2 + 1u);
		OBJECT(current_object)->direction = direction_to_player_2;
	}
	if ((int16_t)farthest_distance < distance_to_player_3)
		OBJECT(current_object)->direction = direction_to_player_2;
	OBJECT(current_object)->speed = (uint16_t)nearest_player_distance * 3u;
	OBJECT(current_object)->whirlwind.timer = 0xF0;
	OBJECT(current_object)->handler = whirlwind_move_ADDRESS;
	whirlwind_move();
}

void whirlwind_move(void)
{
	uint32_t aim_speed = 0, deleted = 0, direction = 0, frame_index = 0, ignored = 0, speed = 0, speed_x = 0, speed_y = 0, spin_angle = 0;

	if (OBJECT(current_object)->whirlwind.timer <= 0)
	{
		OBJECT(current_object)->speed_x = 0;
		OBJECT(current_object)->speed_y = 0;
		OBJECT(current_object)->whirlwind.frame_index = 0;
		OBJECT(current_object)->handler = whirlwind_vanish_ADDRESS;
		whirlwind_vanish();
		return;
	}
	OBJECT(current_object)->whirlwind.timer--;
	if ((OBJECT(current_object)->flags & 4u) != 0u)
	{
		OBJECT(current_object)->speed_x = 0;
		OBJECT(current_object)->speed_y = 0;
		OBJECT(current_object)->whirlwind.frame_index = 0;
		OBJECT(current_object)->handler = whirlwind_vanish_ADDRESS;
		whirlwind_vanish();
		return;
	}
	move_object_projected(&ignored, &ignored, &ignored, &ignored);
	delete_if_off_screen(&deleted, &ignored, &ignored);
	if ((uint8_t)deleted != 1u)
	{
		spin_angle = (OBJECT(current_object)->whirlwind.spin_angle + OBJECT(current_object)->whirlwind.spin_step) & 0x3FFu;
		OBJECT(current_object)->whirlwind.spin_angle = spin_angle;
		speed = 0x100u;
		direction_to_speed(speed, &spin_angle, &speed_x, &speed_y);
		OBJECT(current_object)->speed_x = speed_x;
		OBJECT(current_object)->speed_y = speed_y;
		direction = OBJECT(current_object)->direction;
		aim_speed = OBJECT(current_object)->speed;
		direction_to_speed(aim_speed, &direction, &speed_x, &speed_y);
		OBJECT(current_object)->speed_x += speed_x;
		OBJECT(current_object)->speed_y += speed_y;
		frame_index = (uint8_t)OBJECT(current_object)->whirlwind.frame_index;
		frame_index = (uint8_t)(frame_index + 1u);
		frame_index = frame_index & 3u;
		OBJECT(current_object)->whirlwind.frame_index = frame_index;
		OBJECT(current_object)->sprite_frame = whirlwind_sprites[frame_index];
		queue_sprite(aim_speed);
		add_to_touchable_list(frame_index);
	}
}

void whirlwind_vanish(void)
{
	uint32_t ignored = 0;
	uint16_t odd_frame = 0;

	move_object_projected(&ignored, &ignored, &ignored, &ignored);
	OBJECT(current_object)->whirlwind.timer++;
	odd_frame = OBJECT(current_object)->whirlwind.timer & 1u;
	OBJECT(current_object)->whirlwind.timer = odd_frame;
	if (odd_frame != 0u)
	{
		queue_sprite(0u);
		add_to_touchable_list(ignored);
		return;
	}
	OBJECT(current_object)->sprite_frame = whirlwind_vanish_sprites[(uint8_t)OBJECT(current_object)->whirlwind.frame_index];
	OBJECT(current_object)->whirlwind.frame_index++;
	if (OBJECT(current_object)->whirlwind.frame_index <= 7)
	{
		queue_sprite(0u);
		add_to_touchable_list(ignored);
		return;
	}
	delete_object_and_children(&ignored, &ignored, &ignored);
}

void centipede_init(void)
{
	uint16_t follow_delay = 0;
	uint32_t block = 0, free_count = 0, ignored = 0, object = 0, previous_segment = 0;

	OBJECT(current_object)->hitbox_size_x = 0x10u;
	OBJECT(current_object)->hitbox_height = 0x32u;
	OBJECT(current_object)->kind = 0x24u;
	OBJECT(current_object)->sprite_frame = 0x7940u;
	OBJECT(current_object)->sprite_palette = 0x9270u;
	current_object = LINKED(current_object, OBJECT(current_object)->child);
	OBJECT(current_object)->sprite_frame = 0x5D80u;
	OBJECT(current_object)->sprite_palette = 0x87A0u;
	r14 = 0xAu;
	follow_delay = 0u;
	do
	{
		previous_segment = current_object;
		spawn_count = 2u;
		spawn_object_with_pieces(0x012351u, &ignored, &ignored, &free_count, &object, &block);
		copy_position_to(object);
		OBJECT(object)->sprite_frame = 0x7980u;
		OBJECT(object)->sprite_palette = 0x9270u;
		OBJECT(object)->handler = centipede_segment_wait_ADDRESS;
		follow_delay += 5u;
		OBJECT(object)->centipede_segment.path_index = follow_delay;
		current_object = object;
		object = LINKED(object, OBJECT(object)->child);
		OBJECT(object)->sprite_frame = 0x5D80u;
		OBJECT(object)->sprite_palette = 0x87A0u;
		OBJECT(object)->sprite_scale = 0xA0u;
		r14 = r14 - 1u;
	} while (r14 != 0u);
	OBJECT(current_object)->centipede_tail.segment_ahead = previous_segment;
	OBJECT(current_object)->sprite_scale = 0x100u;
	OBJECT(current_object)->sprite_frame = 0x7A08u;
	OBJECT(current_object)->sprite_palette = 0x9270u;
	OBJECT(current_object)->handler = centipede_tail_wait_ADDRESS;
	current_object = object_cursor;
	OBJECT(current_object)->handler = centipede_chase_ADDRESS;
	centipede_command = 0u;
	add_to_rider_list();
}

void centipede_chase(void)
{
	uint32_t r0 = 0, r2 = 0, r3 = 0, r11 = 0, path_cursor = 0;
	uint32_t path_write_index = 0;

	move_object_projected(&r0, &r2, &r3, &r11);
	if (scroll_position <= OBJECT(current_object)->spawn_arg_1)
	{
		if ((int8_t)active_player_count > 0)
		{
			nearest_player_distance = 0;
			centipede_command = 0u;
			if (OBJECT(current_object)->speed_z > 0)
			{
				centipede_move(r11);
				return;
			}
			r0 = add_to_touchable_list(r0);
			face_nearest_player(r0, r3);
			if (nearest_player_floor_z < 0x800)
			{
				centipede_move(r11);
				return;
			}
			OBJECT(current_object)->anchor_y = 0;
			centipede_move(r11);
			return;
		}
		path_write_index = (uint8_t)OBJECT(current_object)->centipede.path_write_index;
		path_cursor = path_write_index - 1u;
		if (path_write_index == 0u)
			path_cursor = 0x32u;
		path_cursor = path_cursor * 4u + centipede_path_ADDRESS;
		OBJECT(current_object)->projected_x = CENTIPEDE_PATH_POINT(path_cursor)->projected_x;
		OBJECT(current_object)->projected_y_int = CENTIPEDE_PATH_POINT(path_cursor)->projected_y;
		path_cursor = (uint8_t)OBJECT(current_object)->centipede.path_write_index * 4u + centipede_path_ADDRESS;
		CENTIPEDE_PATH_POINT(path_cursor)->projected_x = OBJECT(current_object)->projected_x;
		path_cursor += 2u;
		CENTIPEDE_PATH_POINT(path_cursor - 0x2u)->projected_y = OBJECT(current_object)->projected_y_int;
		OBJECT(current_object)->speed_x = 0;
		OBJECT(current_object)->speed_y = 0;
		OBJECT(current_object)->anchor_y = 0x280;
		centipede_command = 1u;
		path_cursor += 2u;
		centipede_hop(&path_cursor, &r2);
		if (OBJECT(current_object)->centipede.hop_timer == 0x5Au)
			spawn_centipede_sand_puff(path_cursor, r2, r3, r11);
		draw_centipede_piece();
		return;
	}
	OBJECT(current_object)->handler = centipede_leave_ADDRESS;
	nearest_player_distance = 0;
	centipede_command = 0u;
	if (OBJECT(current_object)->speed_z > 0)
	{
		centipede_move(r11);
		return;
	}
	r0 = add_to_touchable_list(r0);
	face_nearest_player(r0, r3);
	if (nearest_player_floor_z < 0x800)
	{
		centipede_move(r11);
		return;
	}
	OBJECT(current_object)->anchor_y = 0;
	centipede_move(r11);
}

void centipede_leave(void)
{
	uint32_t r11 = 0, deleted = 0, ignored = 0;

	move_object_projected(&ignored, &ignored, &ignored, &r11);
	delete_if_off_screen(&deleted, &ignored, &ignored);
	if ((uint8_t)deleted == 1u)
	{
		centipede_command = 9u;
		return;
	}
	centipede_command = 0u;
	OBJECT(current_object)->direction = OBJECT(current_object)->spawn_arg_2;
	nearest_player_distance = 0x1F;
	nearest_player_dx = 0;
	centipede_move(r11);
}

void centipede_move(uint32_t block)
{
	uint16_t saved_direction = 0;
	uint32_t angle_or_path_cursor = 0, chase_speed = 0, direction = 0, head_frame = 0, speed = 0, speed_x = 0, speed_y = 0, wobble_speed_x = 0, wobble_speed_y = 0;

	head_frame = OBJECT(current_object)->direction;
	saved_direction = (uint16_t)head_frame;
	head_frame = (uint16_t)((int16_t)head_frame / 0x92);
	head_frame = (uint16_t)(head_frame + 1u);
	OBJECT(current_object)->sprite_frame = centipede_head_frames[head_frame & 7u];
	direction = saved_direction;
	chase_speed = (uint16_t)nearest_player_distance * 2u;
	chase_speed = (chase_speed & ~0xFFFFu) | (chase_speed & 0x3FFu);
	chase_speed = (chase_speed & ~0xFFFFu) | (uint16_t)(chase_speed + 0x100u);
	r14 = chase_speed;
	chase_speed = chase_speed * 2u;
	direction_to_speed(chase_speed, &direction, &speed_x, &speed_y);
	OBJECT(current_object)->speed_x = speed_x;
	OBJECT(current_object)->speed_y = speed_y;
	OBJECT(current_object)->centipede.wobble_angle -= 0x10u;
	OBJECT(current_object)->centipede.wobble_angle &= 0x3FFu;
	angle_or_path_cursor = OBJECT(current_object)->centipede.wobble_angle;
	speed = r14;
	direction_to_speed(speed, &angle_or_path_cursor, &wobble_speed_x, &wobble_speed_y);
	OBJECT(current_object)->speed_x += wobble_speed_x;
	OBJECT(current_object)->speed_y += wobble_speed_y;
	if (centipede_command != 1u)
	{
		angle_or_path_cursor = (uint8_t)OBJECT(current_object)->centipede.path_write_index * 4u + centipede_path_ADDRESS;
		CENTIPEDE_PATH_POINT(angle_or_path_cursor)->projected_x = OBJECT(current_object)->projected_x;
		angle_or_path_cursor += 2u;
		CENTIPEDE_PATH_POINT(angle_or_path_cursor - 0x2u)->projected_y = OBJECT(current_object)->projected_y_int;
		OBJECT(current_object)->centipede.path_write_index++;
		angle_or_path_cursor += 2u;
		if (OBJECT(current_object)->centipede.path_write_index > 0x32)
			OBJECT(current_object)->centipede.path_write_index = 0;
	}
	centipede_hop(&angle_or_path_cursor, &wobble_speed_x);
	if (OBJECT(current_object)->centipede.hop_timer == 0x5Au)
		spawn_centipede_sand_puff(angle_or_path_cursor, wobble_speed_x, wobble_speed_y, block);
	draw_centipede_piece();
}

void centipede_segment_wait(void)
{
	uint32_t r0 = 0, r2 = 0;

	OBJECT(current_object)->centipede_segment.path_index--;
	if (OBJECT(current_object)->centipede_segment.path_index <= 0)
	{
		OBJECT(current_object)->handler = centipede_segment_follow_ADDRESS;
		centipede_segment_follow();
		return;
	}
	centipede_hop(&r0, &r2);
	if (OBJECT(current_object)->centipede_segment.hop_timer == 0x5Au)
		spawn_centipede_sand_puff(r0, r2, 0u, 0u);
	draw_centipede_piece();
}

void centipede_segment_follow(void)
{
	uint32_t r0 = 0, r2 = 0, r3 = 0, r11 = 0, ignored = 0;

	move_object_projected(&r0, &r2, &r3, &r11);
	if (centipede_command == 9u)
	{
		delete_object_and_children(&ignored, &ignored, &ignored);
		return;
	}
	if (centipede_command != 1u)
		r0 = centipede_follow_path();
	centipede_hop(&r0, &r2);
	if (OBJECT(current_object)->centipede_segment.hop_timer == 0x5Au)
		spawn_centipede_sand_puff(r0, r2, r3, r11);
	draw_centipede_piece();
}

void centipede_tail_wait(void)
{
	uint32_t r0 = 0, r2 = 0;

	OBJECT(current_object)->centipede_tail.path_index--;
	if (OBJECT(current_object)->centipede_tail.path_index <= 0)
	{
		OBJECT(current_object)->handler = centipede_tail_follow_ADDRESS;
		centipede_tail_follow();
		return;
	}
	centipede_hop(&r0, &r2);
	if (OBJECT(current_object)->centipede_tail.hop_timer == 0x5Au)
		spawn_centipede_sand_puff(r0, r2, 0u, 0u);
	draw_centipede_piece();
}

void centipede_tail_follow(void)
{
	uint32_t r0 = 0, r2 = 0, r3 = 0, r11 = 0, ignored = 0, tail_frame = 0;

	move_object_projected(&r0, &r2, &r3, &r11);
	if (centipede_command == 9u)
	{
		delete_object_and_children(&ignored, &ignored, &ignored);
		return;
	}
	if (centipede_command != 1u)
	{
		r14 = OBJECT(current_object)->centipede_tail.segment_ahead;
		r3 = direction_to_object();
		tail_frame = OBJECT(current_object)->direction;
		tail_frame = (uint16_t)((int16_t)tail_frame / 0x92);
		tail_frame = (uint16_t)(tail_frame + 1u);
		tail_frame = (uint16_t)(tail_frame + 4u);
		OBJECT(current_object)->sprite_frame = centipede_tail_frames[tail_frame & 7u];
		r0 = centipede_follow_path();
	}
	centipede_hop(&r0, &r2);
	if (OBJECT(current_object)->centipede_tail.hop_timer == 0x5Au)
		spawn_centipede_sand_puff(r0, r2, r3, r11);
	draw_centipede_piece();
}

uint32_t centipede_follow_path(void)
{
	uint32_t path_cursor = 0;

	path_cursor = (uint16_t)OBJECT(current_object)->centipede_follow_path.path_index * 4u + centipede_path_ADDRESS;
	OBJECT(current_object)->projected_x = CENTIPEDE_PATH_POINT(path_cursor)->projected_x;
	path_cursor += 2u;
	OBJECT(current_object)->projected_y_int = CENTIPEDE_PATH_POINT(path_cursor - 0x2u)->projected_y;
	OBJECT(current_object)->centipede_follow_path.path_index++;
	path_cursor += 2u;
	if (OBJECT(current_object)->centipede_follow_path.path_index > 0x32)
		OBJECT(current_object)->centipede_follow_path.path_index = 0;
	return path_cursor;
}

void centipede_hop(uint32_t *p_height, uint32_t *p_z)
{
	uint32_t height = *p_height;
	uint32_t z = *p_z;
	uint32_t fall_height = 0, ignored = 0;

	if (OBJECT(current_object)->centipede.hop_timer == 0u)
	{
		fall_to_floor(&fall_height, &ignored, &z);
		OBJECT(current_object)->speed_z -= 0x4000u;
		if (OBJECT(current_object)->z < 8)
		{
			*p_height = fall_height;
			*p_z = z;
			return;
		}
		OBJECT(current_object)->speed_z = 0;
		OBJECT(current_object)->centipede.hop_timer = 0x5Au;
		*p_height = fall_height;
		*p_z = z;
		return;
	}
	OBJECT(current_object)->centipede.hop_timer--;
	OBJECT(current_object)->speed_z -= 0x600u;
}

void draw_centipede_piece(void)
{
	uint32_t depth = 0, ignored = 0;

	queue_sprite_at_floor();
	add_to_rider_list();
	current_object = LINKED(current_object, OBJECT(current_object)->child);
	place_sprite_at_cursor_floor();
	depth = (uint16_t)(OBJECT(current_object)->screen_y - 0xAu);
	queue_sprite_at_depth(&ignored, &depth);
}

void spawn_centipede_sand_puff(uint32_t spawned, uint32_t free_count, uint32_t puff, uint32_t block)
{
	uint32_t ignored = 0;

	spawn_count = 1u;
	spawn_object_with_pieces(0x012698u, &spawned, &ignored, &free_count, &puff, &block);
	if ((uint8_t)spawned == 0u)
		return;
	copy_position_to(puff);
	OBJECT(puff)->projected_y_int += 6u;
	OBJECT(puff)->sand_puff.lifetime = 0x2Du;
	OBJECT(puff)->sprite_frame = 0x75A8u;
	OBJECT(puff)->sprite_palette = 0x1020u;
	OBJECT(puff)->sprite_scale = 0x180u;
	OBJECT(puff)->handler = sand_puff_start_ADDRESS;
}

void sand_puff_start(void)
{
	uint32_t ignored = 0;

	unproject_object(&ignored, &ignored, &ignored, &ignored);
	add_to_rider_list();
	OBJECT(current_object)->handler = sand_puff_animate_ADDRESS;
}

void sand_puff_animate(void)
{
	uint32_t ignored = 0, next_frame = 0;
	uint16_t lifetime = 0;

	unproject_object(&ignored, &ignored, &ignored, &ignored);
	if (OBJECT(current_object)->floor_z_int >= 0x800)
	{
		delete_object_and_children(&ignored, &ignored, &ignored);
		return;
	}
	lifetime = OBJECT(current_object)->sand_puff.lifetime;
	OBJECT(current_object)->sand_puff.lifetime = lifetime - 1u;
	if (lifetime == 0u)
	{
		delete_object_and_children(&ignored, &ignored, &ignored);
		return;
	}
	next_frame = (uint16_t)OBJECT(current_object)->sand_puff.frame_index;
	next_frame = (uint16_t)(next_frame + 1u);
	if ((int16_t)next_frame > 2)
		next_frame = 0u;
	OBJECT(current_object)->sand_puff.frame_index = next_frame;
	OBJECT(current_object)->sprite_frame = sand_puff_frames[next_frame];
	queue_sprite(0u);
	add_to_rider_list();
}

void wall_piece_init(void)
{
	uint32_t box_record = 0, spawn_arg_1 = 0;

	spawn_arg_1 = OBJECT(current_object)->spawn_arg_1;
	OBJECT(current_object)->sprite_frame = wall_piece_frames[spawn_arg_1];
	OBJECT(current_object)->sprite_palette = 0x9080u;
	OBJECT(current_object)->handler = wall_piece_draw_ADDRESS;
	OBJECT(current_object)->flags |= 2u;
	OBJECT(current_object)->hitbox_height = 0x28u;
	box_record = wall_piece_boxes_ADDRESS + spawn_arg_1 * 6u;
	OBJECT(current_object)->hitbox_size_x = ROM16(box_record);
	box_record += 2u;
	OBJECT(current_object)->hitbox_size_y = ROM16(box_record);
	box_record += 2u;
	OBJECT(current_object)->hitbox_offset_x = ROM8(box_record);
	OBJECT(current_object)->hitbox_offset_y = ROM8(box_record + 1u);
}

void wall_piece_draw(void)
{
	uint32_t deleted = 0, depth = 0, ignored = 0;

	unproject_object(&ignored, &ignored, &ignored, &ignored);
	delete_if_off_screen(&deleted, &ignored, &ignored);
	if ((uint8_t)deleted == 1u)
		return;
	depth = (uint16_t)OBJECT(current_object)->screen_y;
	if (OBJECT(current_object)->screen_y <= 0xE0)
	{
		depth = (uint16_t)(depth + 0x14u);
		if ((int16_t)depth > 0xEE)
			depth = 0xEEu;
	}
	queue_sprite_at_depth(&ignored, &depth);
}

void shaking_block_init(void)
{
	uint32_t box_record = 0, ignored = 0, object = 0, spawned = 0;

	OBJECT(current_object)->sprite_palette = 0x9050u;
	OBJECT(current_object)->flags |= 2u;
	OBJECT(current_object)->hitbox_height = 0x28u;
	box_record = shaking_block_boxes_ADDRESS + OBJECT(current_object)->spawn_arg_1 * 6u;
	OBJECT(current_object)->hitbox_size_x = ROM16(box_record);
	box_record += 2u;
	OBJECT(current_object)->hitbox_size_y = ROM16(box_record);
	box_record += 2u;
	OBJECT(current_object)->hitbox_offset_x = ROM8(box_record);
	OBJECT(current_object)->hitbox_offset_y = ROM8(box_record + 1u);
	spawn_count = 1u;
	add_pieces_to_current_object(&spawned, &ignored, &ignored, &object);
	if ((uint8_t)spawned == 0u)
		return;
	if (OBJECT(current_object)->spawn_arg_1 != 0u)
	{
		OBJECT(object)->sprite_frame = 0x7AA0u;
		OBJECT(object)->sprite_palette = 0x9050u;
	}
	else
	{
		OBJECT(object)->sprite_frame = 0x7570u;
		OBJECT(object)->sprite_palette = 0x1060u;
	}
	OBJECT(current_object)->handler = shaking_block_animate_ADDRESS;
}

void shaking_block_animate(void)
{
	uint8_t bucket = 0;
	uint16_t offset_x = 0, offset_y = 0;
	uint32_t bucket_2 = 0, deleted = 0, ignored = 0;
	uint16_t frame_timer = 0;

	unproject_object(&ignored, &ignored, &ignored, &ignored);
	delete_if_off_screen(&deleted, &ignored, &ignored);
	if ((uint8_t)deleted == 1u)
		return;
	frame_timer = OBJECT(current_object)->shaking_block.frame_timer;
	OBJECT(current_object)->shaking_block.frame_timer = frame_timer - 1u;
	if (frame_timer == 0u)
	{
		OBJECT(current_object)->shaking_block.frame_timer = 2u;
		OBJECT(current_object)->sprite_frame = shaking_block_frames[(uint8_t)OBJECT(current_object)->shaking_block.frame_index + OBJECT(current_object)->spawn_arg_1 * 3u];
		OBJECT(current_object)->shaking_block.frame_index++;
		if (OBJECT(current_object)->shaking_block.frame_index > 2)
			OBJECT(current_object)->shaking_block.frame_index = 0;
	}
	if (OBJECT(current_object)->spawn_arg_1 == 0u)
	{
		bucket = 0xFFu;
		queue_sprite_in_bucket(bucket);
		current_object = LINKED(current_object, OBJECT(current_object)->child);
		bucket_2 = place_sprite_at_cursor_floor();
		OBJECT(current_object)->screen_x -= 0x10u;
		OBJECT(current_object)->screen_y += 7u;
		queue_sprite_in_bucket(bucket_2);
	}
	else
	{
		bucket = 0u;
		queue_sprite_in_bucket(bucket);
		current_object = LINKED(current_object, OBJECT(current_object)->child);
		offset_x = 0xFFD8u;
		offset_y = 0x20u;
		place_sprite_near_cursor(offset_x, offset_y);
		bucket = 0u;
		queue_sprite_in_bucket(bucket);
	}
}

void sand_rock_init(void)
{
	OBJECT(current_object)->sprite_flags = 0u;
	OBJECT(current_object)->sprite_frame = 0x7BA8u;
	OBJECT(current_object)->sprite_palette = 0x9010u;
	OBJECT(current_object)->flags |= 2u;
	OBJECT(current_object)->hitbox_size_x = 0x22u;
	OBJECT(current_object)->hitbox_size_y = 0x21u;
	OBJECT(current_object)->hitbox_offset_x = -0x21;
	OBJECT(current_object)->hitbox_offset_y = 3;
	OBJECT(current_object)->hitbox_height = 0x20u;
	OBJECT(current_object)->kind = 5u;
	OBJECT(current_object)->handler = sand_rock_update_ADDRESS;
	current_object = LINKED(current_object, OBJECT(current_object)->child);
	OBJECT(current_object)->sprite_flags = 0u;
	OBJECT(current_object)->sprite_palette = 0x9010u;
}

void sand_rock_update(void)
{
	uint32_t r3 = 0, r11 = 0, crumble_frame = 0, deleted = 0, depth = 0, ignored = 0, random_value = 0, ring = 0;

	unproject_object(&ignored, &ignored, &r3, &r11);
	fall_to_floor(&ignored, &ignored, &ignored);
	delete_if_off_screen(&deleted, &ignored, &r3);
	if ((uint8_t)deleted == 1u)
		return;
	if (OBJECT(current_object)->sand_rock.crumbling != 0u)
	{
	sand_rock_crumble:
		OBJECT(current_object)->sand_rock.frame_count++;
		if (OBJECT(current_object)->sand_rock.frame_count <= 1)
		{
			depth = (uint16_t)(OBJECT(current_object)->screen_y + 0x14u);
			queue_sprite_at_depth(&ignored, &depth);
			if ((uint8_t)OBJECT(current_object)->sand_rock.crumble_frame == 0u)
				add_to_touchable_list(ignored);
			return;
		}
		if (OBJECT(current_object)->sand_rock.crumble_frame <= 0xD)
		{
			crumble_frame = (uint8_t)OBJECT(current_object)->sand_rock.crumble_frame;
			OBJECT(current_object)->sprite_frame = sand_rock_crumble_frames[crumble_frame];
			OBJECT(current_object)->sand_rock.frame_count = 0;
			OBJECT(current_object)->sand_rock.crumble_frame++;
			crumble_frame++;
			if ((int8_t)crumble_frame > 0xD)
			{
				depth = (uint16_t)(OBJECT(current_object)->screen_y + 0x14u);
				queue_sprite_at_depth(&ignored, &depth);
				if ((uint8_t)OBJECT(current_object)->sand_rock.crumble_frame == 0u)
					add_to_touchable_list(ignored);
				return;
			}
			OBJECT(LINKED(WORK_RAM, OBJECT(current_object)->child))->sprite_frame = sand_rock_crumble_frames[crumble_frame];
			depth = (uint16_t)(OBJECT(current_object)->screen_y + 0x14u);
			queue_sprite_at_depth(&ignored, &depth);
			if ((uint8_t)OBJECT(current_object)->sand_rock.crumble_frame == 0u)
				add_to_touchable_list(ignored);
			return;
		}
		OBJECT(current_object)->sand_rock.frame_count = 0;
		OBJECT(current_object)->sand_rock.crumble_frame = 0;
		OBJECT(current_object)->sand_rock.crumbling = 0u;
		OBJECT(current_object)->sprite_frame = sand_rock_crumble_frames[(uint8_t)OBJECT(current_object)->sand_rock.crumble_frame];
		delete_object_and_children(&ignored, &ignored, &ignored);
		return;
	}
	if ((OBJECT(current_object)->flags & 4u) == 0u)
	{
		depth = (uint16_t)(OBJECT(current_object)->screen_y + 0x14u);
		queue_sprite_at_depth(&ignored, &depth);
		if ((uint8_t)OBJECT(current_object)->sand_rock.crumble_frame == 0u)
			add_to_touchable_list(ignored);
		return;
	}
	OBJECT(current_object)->sand_rock.crumbling = 1u;
	if (OBJECT(current_object)->sand_rock.ring_count == 0u)
		goto sand_rock_crumble;
	released_ring_flip = 0u;
	rings_to_release = OBJECT(current_object)->sand_rock.ring_count;
	released_ring_angle = OBJECT(current_object)->spawn_arg_2;
	drop_rings(r3, &r11, &random_value, &ring);
	depth = (uint16_t)(OBJECT(current_object)->screen_y + 0x14u);
	queue_sprite_at_depth(&ignored, &depth);
	if ((uint8_t)OBJECT(current_object)->sand_rock.crumble_frame == 0u)
		add_to_touchable_list(ignored);
}

void sand_rock_draw_piece(void)
{
	uint32_t depth = 0, ignored = 0;

	current_object = LINKED(current_object, OBJECT(object_cursor)->child);
	OBJECT(current_object)->screen_x = OBJECT(object_cursor)->screen_x;
	OBJECT(current_object)->screen_y = OBJECT(object_cursor)->screen_y;
	depth = (uint16_t)(OBJECT(current_object)->screen_y + 0x1Fu);
	queue_sprite_at_depth(&ignored, &depth);
}

void sand_arch_init(void)
{
	uint16_t ball_phase = 0, phase_spacing = 0;
	uint32_t balls_left = 0, block = 0, free_count = 0, ignored = 0, object = 0, object_2 = 0;

	OBJECT(current_object)->direction = OBJECT(current_object)->sand_arch.row_direction * 0x40u;
	OBJECT(current_object)->sprite_flags = 0u;
	OBJECT(current_object)->sprite_frame = 0xA1C0u;
	OBJECT(current_object)->sprite_palette = 0x9200u;
	OBJECT(current_object)->handler = sand_arch_update_ADDRESS;
	current_object = LINKED(current_object, OBJECT(current_object)->child);
	OBJECT(current_object)->sprite_flags = 0u;
	OBJECT(current_object)->sprite_frame = 0xA1C0u;
	OBJECT(current_object)->sprite_palette = 0x87A0u;
	object = current_object;
	balls_left = (uint8_t)OBJECT(object_cursor)->sand_arch.ball_count;
	current_player = (current_player & ~0xFFFFu) | 6u;
	ball_phase = 0u;
	phase_spacing = divide_s16(OBJECT(object_cursor)->spawn_arg_2, (uint16_t)(balls_left + 1u));
	do
	{
		spawn_count = 2u;
		spawn_object_with_pieces(0x01369Cu, &ignored, &ignored, &free_count, &object_2, &block);
		OBJECT(object)->child = object_2;
		copy_position_to(object_2);
		OBJECT(object_2)->sprite_flags = 0u;
		OBJECT(object_2)->sprite_frame = 0xA1C0u;
		OBJECT(object_2)->sprite_palette = 0x9200u;
		OBJECT(object_2)->direction = OBJECT(object_cursor)->direction;
		OBJECT(object_2)->hitbox_size_x = 0x10u;
		OBJECT(object_2)->hitbox_height = 0x10u;
		OBJECT(object_2)->kind = 0x24u;
		OBJECT(object_2)->handler = sand_arch_ball_init_ADDRESS;
		ball_phase = (ball_phase - phase_spacing) & 0x3FFu;
		OBJECT(object_2)->sand_arch.bob_phase = ball_phase;
		OBJECT(object_2)->sand_arch.row_position = current_player;
		current_player = (current_player & ~0xFFFFu) | (uint16_t)(current_player + 6u);
		object_2 = LINKED(object_2, OBJECT(object_2)->child);
		OBJECT(object_2)->sprite_flags = 0u;
		OBJECT(object_2)->sprite_frame = 0xA1C0u;
		OBJECT(object_2)->sprite_palette = 0x87A0u;
		object = object_2;
		balls_left--;
	} while (balls_left != 0u);
}

void sand_arch_update(void)
{
	uint32_t bucket = 0, ignored = 0, on_screen = 0;

	unproject_object(&ignored, &ignored, &ignored, &ignored);
	on_screen = is_on_screen();
	OBJECT(current_object)->sand_arch.row_position += on_screen;
	OBJECT(current_object)->sand_arch.row_position &= 1u;
	if (OBJECT(current_object)->sand_arch.balls_gone < OBJECT(current_object)->sand_arch.ball_count)
	{
		current_object = LINKED(current_object, OBJECT(current_object)->child);
		bucket = place_sprite_at_cursor_floor();
		queue_sprite_in_bucket(bucket);
		current_object = object_cursor;
		return;
	}
	delete_object_and_children(&ignored, &ignored, &ignored);
}

void sand_arch_ball_init(void)
{
	uint32_t ball_direction = 0, ignored = 0, speed = 0, speed_x = 0, speed_y = 0;

	ball_direction = OBJECT(current_object)->direction;
	speed = 0x200u * OBJECT(current_object)->sand_arch.row_position;
	direction_to_speed(speed, &ball_direction, &speed_x, &speed_y);
	OBJECT(current_object)->speed_x = speed_x;
	OBJECT(current_object)->speed_y = speed_y;
	move_object_projected(&ignored, &ignored, &ignored, &ignored);
	OBJECT(current_object)->speed_x = 0;
	OBJECT(current_object)->speed_y = 0;
	OBJECT(current_object)->handler = sand_arch_ball_delay_ADDRESS;
	sand_arch_ball_delay();
}

void sand_arch_ball_delay(void)
{
	add_to_rider_list();
	OBJECT(current_object)->sand_arch.bob_phase += 2u;
	if (OBJECT(current_object)->sand_arch.bob_phase <= 0x3FF)
	{
		sand_arch_ball_gone();
		return;
	}
	OBJECT(current_object)->sand_arch.bob_phase &= 0x3FFu;
	OBJECT(current_object)->handler = sand_arch_ball_move_ADDRESS;
}

void sand_arch_ball_move(void)
{
	uint16_t below_floor = 0, offset_x = 0, offset_y = 0;
	uint32_t depth = 0, ignored = 0;
	uint16_t direction = 0;

	move_object_projected(&ignored, &ignored, &ignored, &ignored);
	add_to_touchable_list(ignored);
	add_to_rider_list();
	if (OBJECT(current_object)->floor_z_int >= 0x800)
	{
		if (OBJECT(current_object)->z_int >= 0x800)
		{
			current_object = LINKED(current_object, OBJECT(current_object)->parent);
			OBJECT(current_object)->sand_arch.balls_gone++;
			current_object = object_cursor;
			OBJECT(current_object)->handler = sand_arch_ball_gone_ADDRESS;
		}
		fall_to_floor(&ignored, &ignored, &ignored);
		goto sand_arch_ball_draw;
	}
	sand_arch_ball_bob();
	below_floor = (uint16_t)(OBJECT(current_object)->z_int - OBJECT(current_object)->floor_z_int);
	if ((int16_t)below_floor >= 0x10)
	{
		sand_arch_ball_gone();
		return;
	}
	if ((int16_t)below_floor < -8)
	{
	sand_arch_ball_draw:
		ignored = queue_sprite_at_floor();
	}
	if ((int16_t)(OBJECT(current_object)->floor_z_int - OBJECT(current_object)->z_int) <= 0x10)
	{
		current_object = LINKED(current_object, OBJECT(current_object)->child);
		direction = OBJECT(current_object)->direction;
		OBJECT(current_object)->direction = direction - 1u;
		if (direction == 0u)
		{
			OBJECT(current_object)->direction = 2u;
			OBJECT(current_object)->sprite_frame = sand_arch_spray_frames[(uint8_t)OBJECT(current_object)->piece_frame];
			OBJECT(current_object)->sprite_palette = 0x9020u;
			OBJECT(current_object)->piece_frame++;
			OBJECT(current_object)->piece_frame = (uint8_t)OBJECT(current_object)->piece_frame % 4u;
		}
		offset_x = 0u;
		offset_y = 8u;
		place_sprite_near_cursor(offset_x, offset_y);
		OBJECT(current_object)->screen_y -= OBJECT(object_cursor)->z_int - OBJECT(object_cursor)->floor_z_int;
		queue_sprite(ignored);
		sand_arch_ball_gone();
		return;
	}
	current_object = LINKED(current_object, OBJECT(current_object)->child);
	OBJECT(current_object)->sprite_frame = 0xA1C0u;
	OBJECT(current_object)->sprite_palette = 0x87A0u;
	offset_x = 0u;
	offset_y = (uint16_t)(OBJECT(object_cursor)->floor_z_int - OBJECT(object_cursor)->z_int);
	place_sprite_near_cursor(offset_x, offset_y);
	depth = (uint16_t)(OBJECT(current_object)->screen_y - 0x10u);
	queue_sprite_at_depth(&ignored, &depth);
	OBJECT(current_object)->piece_frame = 0;
	sand_arch_ball_gone();
}

void sand_arch_ball_gone(void)
{
}

void sand_arch_ball_bob(void)
{
	OBJECT(current_object)->sand_arch.bob_phase += 0xCu;
	OBJECT(current_object)->sand_arch.bob_phase &= 0x3FFu;
	OBJECT(current_object)->speed_z = 0u - sine_table[(uint16_t)OBJECT(current_object)->sand_arch.bob_phase] * 0xCu;
}

void stepped_wall_init(void)
{
	OBJECT(current_object)->sprite_flags = 6u;
	OBJECT(current_object)->sprite_frame = 0xB670u;
	OBJECT(current_object)->sprite_palette = 0x8D60u;
	OBJECT(current_object)->sprite_palette |= 0x2000u;
	OBJECT(current_object)->flags |= 2u;
	OBJECT(current_object)->hitbox_size_x = 0x1Du;
	OBJECT(current_object)->hitbox_size_y = 0xDCu;
	OBJECT(current_object)->hitbox_offset_x = 0x73;
	OBJECT(current_object)->hitbox_offset_y = -0x7E;
	OBJECT(current_object)->hitbox_height = 0x80u;
	OBJECT(current_object)->kind = 0xCu;
	OBJECT(current_object)->handler = stepped_wall_draw_ADDRESS;
}

void stepped_wall_draw(void)
{
	uint32_t bucket = 0, deleted = 0, ignored = 0;

	unproject_object(&ignored, &ignored, &ignored, &ignored);
	delete_if_off_screen(&deleted, &ignored, &ignored);
	if ((uint8_t)deleted == 0u)
	{
		bucket = 0xFFu;
		queue_sprite_in_bucket(bucket);
		add_to_touchable_list(bucket);
		return;
	}
}

void tornado_start(void)
{
	uint16_t angle_offset = 0, radius = 0;
	uint32_t column_slot = 0, columns_left = 0, free_count = 0, ignored = 0, object = 0, object_2 = 0, tornado_record = 0, zone_index = 0;

	if (sand_block_leader_active != 0u && (OBJECT(current_object)->sprite_frame != 1u || scroll_position < 0x9C00u))
		return;
	tornado_record = tornado_records_ADDRESS + tornado_records[OBJECT(current_object)->sprite_frame];
	OBJECT(current_object)->x_int = ROM16(tornado_record);
	OBJECT(current_object)->x_fraction = 0u;
	tornado_record += 2u;
	OBJECT(current_object)->y_int = ROM16(tornado_record);
	OBJECT(current_object)->y_fraction = 0u;
	tornado_record += 2u;
	OBJECT(current_object)->animation = ROM16(tornado_record) + tornado_hazard_lists_ADDRESS;
	tornado_kind = OBJECT(current_object)->sprite_frame;
	tornado_record += 2u;
	object = ROM16(tornado_record) + tornado_whirlwind_lists_ADDRESS;
	OBJECT(current_object)->part_list = object;
	OBJECT(current_object)->tornado.path = tornado_record + 2u;
	OBJECT(current_object)->flags = 2u;
	OBJECT(current_object)->tornado.holding_back = 0u;
	OBJECT(current_object)->kind = 0x1Eu;
	OBJECT(current_object)->tornado.wait_timer = 0;
	OBJECT(current_object)->tornado.rotation_angle = 0u;
	OBJECT(current_object)->hitbox_height = 0x70u;
	OBJECT(current_object)->tornado.off_screen = 0xFFu;
	OBJECT(current_object)->handler = tornado_move_ADDRESS;
	chaser_object = current_object;
	tornado_object = current_object;
	group_leader_0_held = 0xFFu;
	if (tornado_kind == 0u)
		players_frozen = 0xFFu;
	if (tornado_kind == 0u)
	{
		zone_index = 1u;
		object = spawn_chaser_back_zone(zone_index, object);
	}
	else
		object = spawn_tornado_rear_zone(object);
	if (free_object_stack < 5)
		return;
	columns_left = 5u;
	column_slot = current_object + 0x96u;
	angle_offset = 0u;
	radius = 0x4000u;
	do
	{
		allocate_object(&ignored, &object);
		OBJECT(object)->sprite_flags = 0x1004u;
		OBJECT(object)->sprite_palette = 0x1100u;
		OBJECT(object)->handler = tornado_column_circle_ADDRESS;
		OBJECT(object)->flags = 0u;
		OBJECT(object)->sprite_offset_x = 0;
		OBJECT(object)->sprite_offset_y = 0;
		OBJECT(object)->hitbox_size_y = 0x200u;
		OBJECT(object)->speed_z_fraction = radius;
		radius += 0x2000u;
		OBJECT(object)->speed_z_int = angle_offset;
		angle_offset += 0xCCu;
		OBJECT(object)->animation = tornado_column_animation_ADDRESS;
		OBJECT(object)->animation_frame = 0u;
		OBJECT(object)->animation_speed = 1u;
		OBJECT(object)->animation_timer = 0;
		mem_write16(column_slot, object);
		OBJECT(object)->parent = current_object;
		object_2 = object;
		OBJECT(object_2)->child = 0xFFFFu;
		column_slot += 2u;
		if ((uint8_t)tornado_kind != 0u)
		{
			allocate_object_2(&free_count, &ignored, &object);
			if (free_count != 0u)
			{
				OBJECT(object)->sprite_flags = 0x1004u;
				OBJECT(object)->sprite_palette = 0x1100u;
				OBJECT(object_2)->child = object;
			}
		}
		columns_left--;
	} while (columns_left != 0u);
}

void tornado_move(void)
{
	uint8_t sound_id = 0;
	uint16_t box_across = 0, box_along = 0, hazard_coordinate = 0, move_speed = 0, threshold = 0;
	uint32_t r0 = 0, r2 = 0, r3 = 0, r11 = 0, columns_left = 0, direction = 0, floor_slope = 0, hazard_record = 0, ignored = 0, object = 0, path_cursor = 0, slope_factor = 0, slot_cursor = 0, x = 0, y = 0, z = 0;
	uint8_t hold_timer = 0, sound_timer = 0;
	uint16_t hazard_scroll = 0, record_scroll = 0;
	uint32_t flags = 0;

	if (map_screen_active != 0u)
	{
		object = current_object;
		slot_cursor = current_object + 0x96u;
		columns_left = 5u;
		do
		{
			object = (object & ~0xFFFFu) | mem_read16(slot_cursor);
			OBJECT(object)->flags |= 0x400u;
			columns_left--;
			slot_cursor += 2u;
		} while (columns_left != 0u);
		delete_current_object();
		return;
	}
	flags = OBJECT(current_object)->flags;
	OBJECT(current_object)->flags = flags | 0x80000000u;
	if ((flags & 0x80000000u) != 0u)
	{
		if (OBJECT(current_object)->tornado.holding_back != 0u)
			goto tornado_move_step;
		if (no_player_in_play != 0u || player_mishap != 0u || player_dropping_in != 0u)
		{
			OBJECT(current_object)->tornado.holding_back = 0xFFu;
			OBJECT(current_object)->tornado.saved_base_speed = OBJECT(current_object)->tornado.base_speed;
			OBJECT(current_object)->tornado.base_speed = 0;
			OBJECT(current_object)->tornado.hold_start_position = scroll_position;
			OBJECT(current_object)->tornado.hold_timer = 0xFFu;
			OBJECT(current_object)->tornado.saved_threshold = OBJECT(current_object)->tornado.screen_threshold;
			threshold = 0xFFB0u;
			if (OBJECT(current_object)->direction < 0x100 || OBJECT(current_object)->direction > 0x200)
				threshold = 0x190u;
			OBJECT(current_object)->tornado.screen_threshold = threshold;
			goto tornado_move_step;
		}
	}
	if (current_game_mode != 0x11u)
		OBJECT(current_object)->tornado.wait_timer--;
	if (OBJECT(current_object)->tornado.wait_timer > 0)
	{
	tornado_move_step:
		find_floor(&ignored, &ignored, &r2, &r3, &r11);
		OBJECT(current_object)->z = OBJECT(current_object)->floor_z;
		OBJECT(current_object)->z_int += 8u;
		OBJECT(current_object)->tornado.rotation_angle -= 4u;
		OBJECT(current_object)->tornado.rotation_angle &= 0x3FFu;
		project_object(&r0, &ignored);
		r0 = add_to_zone_list(r0);
		r11 = tornado_throw_whirlwinds((uint8_t)r0, r2, r3, r11);
		hazard_record = OBJECT(current_object)->animation;
		hazard_scroll = TORNADO_HAZARD(hazard_record)->position;
		hazard_record += 2u;
		if (hazard_scroll <= scroll_position)
		{
			hazard_coordinate = (uint16_t)TORNADO_HAZARD(hazard_record - 0x2u)->coordinate;
			direction = OBJECT(current_object)->direction;
			switch (direction >> 8)
			{
			case 0:
			case 4:
				if (OBJECT(current_object)->y_int <= (int16_t)hazard_coordinate)
				{
					y = OBJECT(current_object)->y;
				tornado_hazard_x_at_block_edge:
					x = mem_read32(floor_blocks + 2u + OBJECT(current_object)->floor_block) - 0x80000u;
				tornado_place_hazard:
					z = OBJECT(current_object)->z;
					spawn_tornado_hazard(x, y, z, r11);
					OBJECT(current_object)->animation += 4u;
				}
				break;
			case 1:
				if ((int16_t)hazard_coordinate <= OBJECT(current_object)->x_int)
				{
					x = OBJECT(current_object)->x - 0x800000u;
				tornado_hazard_y_at_block_edge:
					y = mem_read32(floor_blocks + 2u + OBJECT(current_object)->floor_block + 8u) - 0x80000u;
					goto tornado_place_hazard;
				}
				break;
			case 2:
				if ((int16_t)hazard_coordinate <= OBJECT(current_object)->y_int)
				{
					y = OBJECT(current_object)->y - 0x800000u;
					goto tornado_hazard_x_at_block_edge;
				}
				break;
			case 3:
				if (OBJECT(current_object)->x_int <= (int16_t)hazard_coordinate)
				{
					x = OBJECT(current_object)->x;
					goto tornado_hazard_y_at_block_edge;
				}
				break;
			default:
				invalid_jump(0x89477u + (direction >> 8) * 2u);
			}
		}
		if (OBJECT(current_object)->tornado.holding_back != 0u)
		{
			if ((int16_t)(scroll_position - OBJECT(current_object)->tornado.hold_start_position) >= 0x300)
				goto tornado_stop_holding_back;
			if (player_mishap != 0u || player_dropping_in != 0u || no_player_in_play != 0u)
				OBJECT(current_object)->tornado.hold_timer = 0xFFu;
			hold_timer = OBJECT(current_object)->tornado.hold_timer;
			OBJECT(current_object)->tornado.hold_timer = hold_timer - 1u;
			if (hold_timer == 0u)
				goto tornado_stop_holding_back;
		}
		if (OBJECT(current_object)->screen_y < 0)
		{
		tornado_catch_up:
			if (OBJECT(current_object)->tornado.holding_back != 0u)
			{
			tornado_stop_holding_back:
				OBJECT(current_object)->tornado.holding_back = 0u;
				OBJECT(current_object)->tornado.screen_threshold = OBJECT(current_object)->tornado.saved_threshold;
				OBJECT(current_object)->tornado.base_speed = OBJECT(current_object)->tornado.saved_base_speed;
			}
			slope_factor = 0u;
			if ((OBJECT(current_object)->floor_slope | OBJECT(current_object)->floor_type) != 0u)
			{
				floor_slope = OBJECT(current_object)->floor_slope;
				slope_factor = floor_slope_factors[(uint16_t)(floor_slope * 2u)];
			}
			compute_camera_slope_speed(r3, slope_factor);
			move_speed = (uint16_t)((uint32_t)camera_slope_speed_int * OBJECT(current_object)->tornado.catch_up_factor) >> 8;
			if ((int16_t)move_speed <= OBJECT(current_object)->tornado.base_speed)
			{
			tornado_use_base_speed:
				move_speed = (uint16_t)OBJECT(current_object)->tornado.base_speed;
			}
		}
		else
		{
			direction = OBJECT(current_object)->direction;
			switch (direction >> 8)
			{
			case 0:
			case 3:
				if (OBJECT(current_object)->screen_x <= OBJECT(current_object)->tornado.screen_threshold)
					goto tornado_use_base_speed;
				goto tornado_catch_up;
			case 1:
			case 2:
			case 4:
				if (OBJECT(current_object)->tornado.screen_threshold <= OBJECT(current_object)->screen_x)
					goto tornado_use_base_speed;
				goto tornado_catch_up;
			default:
				invalid_jump(0x89545u + (direction >> 8) * 2u);
			}
		}
		if (AS32(players_finished[0]) == 0xFFFFFFu)
			move_speed = (uint16_t)((uint32_t)move_speed << 2);
		chaser_speed = move_speed;
		if (OBJECT(current_object)->tornado.wait_timer <= 0)
		{
			direction = OBJECT(current_object)->direction;
			switch (direction >> 8)
			{
			case 0:
				OBJECT(current_object)->y -= (uint32_t)chaser_speed << 12;
				break;
			case 1:
				OBJECT(current_object)->x += (uint32_t)chaser_speed << 12;
				break;
			case 2:
			case 4:
				OBJECT(current_object)->y += (uint32_t)chaser_speed << 12;
				break;
			case 3:
				OBJECT(current_object)->x -= (uint32_t)chaser_speed << 12;
				break;
			default:
				invalid_jump(0x895E6u + (direction >> 8) * 2u);
			}
		}
		direction = OBJECT(current_object)->direction;
		switch (direction >> 8)
		{
		case 0:
		case 3:
			if (OBJECT(current_object)->screen_x > 0x180)
				OBJECT(current_object)->tornado.off_screen = 0xFFu;
			else
			{
				if (OBJECT(current_object)->tornado.off_screen != 0u)
				{
					OBJECT(current_object)->tornado.off_screen = 0u;
					OBJECT(current_object)->tornado.sound_timer = 0u;
				}
				if (current_game_mode != 7u)
				{
					sound_timer = OBJECT(current_object)->tornado.sound_timer;
					OBJECT(current_object)->tornado.sound_timer = sound_timer - 1u;
					if (sound_timer == 0u)
					{
						sound_id = 0x12u;
						play_sound(sound_id, &ignored, &ignored);
						OBJECT(current_object)->tornado.sound_timer = 0x28u;
					}
				}
			}
			break;
		case 1:
		case 2:
		case 4:
			if (OBJECT(current_object)->screen_x < -0x40)
			{
				OBJECT(current_object)->tornado.off_screen = 0xFFu;
				return;
			}
			if (OBJECT(current_object)->tornado.off_screen != 0u)
			{
				OBJECT(current_object)->tornado.off_screen = 0u;
				OBJECT(current_object)->tornado.sound_timer = 0u;
			}
			if (current_game_mode != 7u)
			{
				sound_timer = OBJECT(current_object)->tornado.sound_timer;
				OBJECT(current_object)->tornado.sound_timer = sound_timer - 1u;
				if (sound_timer == 0u)
				{
					sound_id = 0x12u;
					play_sound(sound_id, &ignored, &ignored);
					OBJECT(current_object)->tornado.sound_timer = 0x28u;
				}
			}
			return;
		default:
			invalid_jump(0x89634u + (direction >> 8) * 2u);
		}
		return;
	}
	OBJECT(current_object)->tornado.wait_timer = 0;
	path_cursor = OBJECT(current_object)->tornado.path;
	record_scroll = ROM16(path_cursor);
	path_cursor += 2u;
	if (record_scroll <= scroll_position)
	{
		path_cursor += 2u;
		goto tornado_load_record;
	}
	threshold = ROM16(path_cursor);
	direction = OBJECT(current_object)->direction;
	path_cursor += 2u;
	switch (direction >> 8)
	{
	case 0:
	case 4:
		if ((int16_t)threshold < OBJECT(current_object)->y_int)
			goto tornado_move_step;
	tornado_load_record:
		if ((int16_t)ROM16(path_cursor + 2u) >= 0)
		{
			OBJECT(current_object)->tornado.screen_threshold = ROM16(path_cursor);
			path_cursor += 2u;
			OBJECT(current_object)->tornado.catch_up_factor = ROM16(path_cursor);
			path_cursor += 2u;
			OBJECT(current_object)->tornado.base_speed = ROM16(path_cursor);
			path_cursor += 2u;
			OBJECT(current_object)->tornado.wait_timer = ROM16(path_cursor);
			path_cursor += 2u;
			OBJECT(current_object)->direction = ROM16(path_cursor);
			OBJECT(current_object)->tornado.path = path_cursor + 2u;
			box_across = 0xF0u;
			box_along = 0x80u;
			direction = OBJECT(current_object)->direction;
			switch (direction >> 8)
			{
			case 0:
			case 4:
				OBJECT(current_object)->hitbox_size_x = box_across;
				OBJECT(current_object)->hitbox_size_y = box_along;
				OBJECT(current_object)->hitbox_offset_x = 0u - (box_across >> 1);
				OBJECT(current_object)->hitbox_offset_y = 0u - (box_along >> 1);
				break;
			case 1:
				OBJECT(current_object)->hitbox_size_x = box_along;
				OBJECT(current_object)->hitbox_size_y = box_across;
				OBJECT(current_object)->hitbox_offset_x = 0u - (box_along >> 1);
				OBJECT(current_object)->hitbox_offset_y = 0u - (box_across >> 1);
				break;
			case 2:
				OBJECT(current_object)->hitbox_size_x = box_across;
				OBJECT(current_object)->hitbox_size_y = box_along;
				OBJECT(current_object)->hitbox_offset_x = 0u - (box_across >> 1);
				OBJECT(current_object)->hitbox_offset_y = 0u - (box_along >> 1);
				break;
			case 3:
				OBJECT(current_object)->hitbox_size_x = box_along;
				OBJECT(current_object)->hitbox_size_y = box_across;
				OBJECT(current_object)->hitbox_offset_x = 0u - (box_along >> 1);
				OBJECT(current_object)->hitbox_offset_y = 0u - (box_across >> 1);
				if ((uint16_t)OBJECT(current_object)->tornado.wait_timer == 0u)
					players_frozen = 0u;
				break;
			default:
				invalid_jump(0x8939Du + (direction >> 8) * 2u);
			}
			if (OBJECT(current_object)->tornado.path != tornado_kind_0_end_record_ADDRESS)
				goto tornado_move_step;
			group_leader_0_held = 0u;
			goto tornado_move_step;
		}
		if ((int16_t)ROM16(path_cursor + 4u) >= 0)
		{
			object = current_object;
			slot_cursor = current_object + 0x96u;
			columns_left = 5u;
			do
			{
				object = (object & ~0xFFFFu) | mem_read16(slot_cursor);
				OBJECT(object)->flags |= 0x400u;
				columns_left--;
				slot_cursor += 2u;
			} while (columns_left != 0u);
			delete_current_object();
			return;
		}
		goal_unlocked = 0xFFu;
		object = current_object;
		slot_cursor = current_object + 0x96u;
		columns_left = 5u;
		do
		{
			object = (object & ~0xFFFFu) | mem_read16(slot_cursor);
			OBJECT(object)->flags |= 0x400u;
			columns_left--;
			slot_cursor += 2u;
		} while (columns_left != 0u);
		delete_current_object();
		return;
	case 1:
		if (OBJECT(current_object)->x_int < (int16_t)threshold)
			goto tornado_move_step;
		goto tornado_load_record;
	case 2:
		if (OBJECT(current_object)->y_int < (int16_t)threshold)
			goto tornado_move_step;
		goto tornado_load_record;
	case 3:
		if ((int16_t)threshold < OBJECT(current_object)->x_int)
			goto tornado_move_step;
		goto tornado_load_record;
	default:
		invalid_jump(0x89306u + (direction >> 8) * 2u);
	}
}

void tornado_column_circle(void)
{
	uint16_t radius_limit = 0;
	uint32_t depth = 0, ignored = 0, object = 0, object_2 = 0, orbit_dx = 0, orbit_dy = 0, radius_rate = 0;

	if ((OBJECT(current_object)->flags & 0x400u) != 0u)
	{
		delete_current_object();
		return;
	}
	OBJECT(current_object)->speed_z_fraction += OBJECT(current_object)->hitbox_size_y;
	if ((int16_t)OBJECT(current_object)->speed_z_fraction < 0x1000)
	{
		next_random(&radius_rate, &ignored, &ignored);
		radius_rate = (radius_rate + 0x100u) & 0x3FFu;
		radius_limit = 0x1000u;
	tornado_column_bounce_radius:
		OBJECT(current_object)->hitbox_size_y = radius_rate;
		OBJECT(current_object)->speed_z_fraction = radius_limit;
	}
	else if ((int16_t)OBJECT(current_object)->speed_z_fraction > 0x6800)
	{
		next_random(&radius_rate, &ignored, &ignored);
		radius_rate = (uint16_t)(0u - ((radius_rate + 0x100u) & 0x3FFu));
		radius_limit = 0x6800u;
		goto tornado_column_bounce_radius;
	}
	object_2 = LINKED(current_object, OBJECT(current_object)->parent);
	OBJECT(current_object)->x = OBJECT(object_2)->x;
	OBJECT(current_object)->y = OBJECT(object_2)->y;
	orbit_dx = (OBJECT(object_2)->spawn_arg_2 + OBJECT(current_object)->speed_z_int) & 0x3FFu;
	orbit_dy = OBJECT(current_object)->speed_z_fraction;
	polar_to_xy(&ignored, &orbit_dx, &orbit_dy);
	OBJECT(current_object)->x += orbit_dx;
	OBJECT(current_object)->y += orbit_dy;
	animate(&ignored, &ignored, &ignored);
	find_floor(&ignored, &ignored, &ignored, &ignored, &ignored);
	OBJECT(current_object)->z = OBJECT(current_object)->floor_z;
	project_object(&ignored, &ignored);
	queue_sprite(ignored);
	if ((int16_t)OBJECT(current_object)->child < 0)
		return;
	object = LINKED(current_object, OBJECT(current_object)->child);
	OBJECT(object)->screen_x = OBJECT(current_object)->screen_x;
	OBJECT(object)->screen_y = OBJECT(current_object)->screen_y;
	OBJECT(object)->screen_y -= 0xE0u;
	OBJECT(object)->sprite_frame = tornado_column_top_frames[OBJECT(current_object)->animation_frame];
	depth = (uint16_t)OBJECT(current_object)->screen_y;
	current_object = object;
	queue_sprite_at_depth(&ignored, &depth);
}

void spawn_tornado_hazard(uint32_t x, uint32_t y, uint32_t z, uint32_t hazard)
{
	uint32_t direction = 0, free_count = 0, ignored = 0;
	uint32_t saved_current_object = 0;

	allocate_object(&free_count, &hazard);
	if (free_count == 0u)
		return;
	OBJECT(hazard)->x = x;
	OBJECT(hazard)->y = y;
	OBJECT(hazard)->z = z;
	OBJECT(hazard)->sprite_flags = 0x1004u;
	OBJECT(hazard)->handler = tornado_hazard_update_ADDRESS;
	OBJECT(hazard)->flags = 2u;
	OBJECT(hazard)->hitbox_offset_x = 0;
	OBJECT(hazard)->hitbox_offset_y = 0;
	OBJECT(hazard)->direction = OBJECT(current_object)->direction;
	OBJECT(hazard)->kind = 0xCu;
	OBJECT(hazard)->hitbox_height = 0x50u;
	direction = OBJECT(hazard)->direction;
	switch (direction >> 8)
	{
	case 0:
	case 2:
	case 4:
		OBJECT(hazard)->hitbox_size_x = 0xF0u;
		OBJECT(hazard)->hitbox_size_y = 0x80u;
		break;
	case 1:
	case 3:
		OBJECT(hazard)->hitbox_size_x = 0x80u;
		OBJECT(hazard)->hitbox_size_y = 0xF0u;
		break;
	default:
		invalid_jump(0x897BBu + (direction >> 8) * 2u);
	}
	saved_current_object = current_object;
	current_object = hazard;
	project_object(&ignored, &ignored);
	current_object = saved_current_object;
}

void tornado_hazard_update(void)
{
	uint16_t margin_x = 0, margin_y = 0;
	uint32_t ignored = 0, off_screen = 0;

	margin_x = 0xF0u;
	margin_y = 0xF0u;
	off_screen = is_off_screen_margin(margin_x, margin_y);
	if (off_screen == 0u)
	{
		add_to_touchable_list(0u);
		update_screen_position(&ignored, &ignored);
		return;
	}
	delete_current_object();
}

uint32_t tornado_throw_whirlwinds(uint8_t spawned, uint32_t free_count, uint32_t whirlwind, uint32_t object)
{
	uint32_t saved_current_object = 0;

	if (mem_read16(OBJECT(current_object)->part_list) <= scroll_position)
	{
		saved_current_object = current_object;
		object = spawn_whirlwind(spawned, free_count, whirlwind, object);
		current_object = saved_current_object;
		OBJECT(current_object)->part_list += 2u;
	}
	return object;
}

uint32_t spawn_tornado_rear_zone(uint32_t zone)
{
	uint32_t free_count = 0;

	allocate_object(&free_count, &zone);
	if (free_count != 0u)
	{
		OBJECT(zone)->sprite_flags = 0x1004u;
		OBJECT(zone)->kind = 9u;
		OBJECT(zone)->flags = 0x40000002u;
		OBJECT(zone)->hitbox_height = 0x160u;
		OBJECT(zone)->hitbox_offset_x = 0;
		OBJECT(zone)->hitbox_offset_y = 0;
		OBJECT(zone)->handler = tornado_rear_zone_follow_ADDRESS;
	}
	return zone;
}

void tornado_rear_zone_follow(void)
{
	uint32_t direction = 0, object = 0;

	if (scroll_position >= 0xE500u)
	{
		delete_current_object();
		return;
	}
	object = LINKED(current_object, chaser_object);
	OBJECT(current_object)->hitbox_size_x = OBJECT(object)->hitbox_size_x;
	OBJECT(current_object)->hitbox_size_y = OBJECT(object)->hitbox_size_y;
	OBJECT(current_object)->x = OBJECT(object)->x;
	OBJECT(current_object)->y = OBJECT(object)->y;
	OBJECT(current_object)->hitbox_offset_x = OBJECT(object)->hitbox_offset_x;
	OBJECT(current_object)->hitbox_offset_y = OBJECT(object)->hitbox_offset_y;
	OBJECT(current_object)->z = OBJECT(object)->z;
	OBJECT(current_object)->z_int += 0x60u;
	OBJECT(current_object)->direction = OBJECT(object)->direction;
	direction = OBJECT(object)->direction;
	switch (direction >> 8)
	{
	case 0:
	case 4:
		OBJECT(current_object)->hitbox_size_y = 0x200u;
		OBJECT(current_object)->y_int += 0x40u;
		OBJECT(current_object)->hitbox_offset_y = 0;
		break;
	case 1:
		OBJECT(current_object)->hitbox_size_x = 0x200u;
		OBJECT(current_object)->x_int -= 0x240u;
		OBJECT(current_object)->hitbox_offset_x = 0;
		break;
	case 2:
		OBJECT(current_object)->hitbox_size_y = 0x200u;
		OBJECT(current_object)->y_int -= 0x240u;
		OBJECT(current_object)->hitbox_offset_y = 0;
		break;
	case 3:
		OBJECT(current_object)->hitbox_size_x = 0x200u;
		OBJECT(current_object)->x_int += 0x40u;
		OBJECT(current_object)->hitbox_offset_x = 0;
		break;
	default:
		invalid_jump(0x898C7u + (direction >> 8) * 2u);
	}
	direction = add_to_zone_list(direction);
}

void quicksand_start(void)
{
	uint32_t ignored = 0, object = 0, object_2 = 0;

	OBJECT(current_object)->flags = 0u;
	if (OBJECT(current_object)->sprite_frame != 0u)
		OBJECT(current_object)->flags |= 1u;
	OBJECT(current_object)->sprite_flags = 0x1000u;
	OBJECT(current_object)->sprite_frame = 0x74E0u;
	OBJECT(current_object)->sprite_offset_x = 0;
	OBJECT(current_object)->sprite_offset_y = -0x34;
	OBJECT(current_object)->hitbox_size_x = 0x60u;
	OBJECT(current_object)->hitbox_height = 0x50u;
	OBJECT(current_object)->hitbox_size_y = 0u;
	OBJECT(current_object)->handler = quicksand_swirl_ADDRESS;
	OBJECT(current_object)->kind = 0x1Cu;
	OBJECT(current_object)->quicksand.palette_index = 0u;
	OBJECT(current_object)->quicksand.palette_delay = 4u;
	OBJECT(current_object)->quicksand.palette_timer = 0u;
	OBJECT(current_object)->quicksand.palette_list = quicksand_palettes_ADDRESS;
	OBJECT(current_object)->animation_frame = 0u;
	OBJECT(current_object)->animation_speed = 4u;
	OBJECT(current_object)->animation_timer = 0;
	OBJECT(current_object)->animation = quicksand_animation_ADDRESS;
	find_floor(&ignored, &ignored, &ignored, &ignored, &object);
	OBJECT(current_object)->z = OBJECT(current_object)->floor_z;
	OBJECT(current_object)->z_int += 0x30u;
	project_object(&ignored, &ignored);
	OBJECT(current_object)->child = 0xFFFFu;
	if (free_object_stack_2[0] >= 2)
	{
		allocate_object_2(&ignored, &ignored, &object);
		OBJECT(object)->sprite_flags = 0xD000u;
		OBJECT(current_object)->child = object;
		object_2 = object;
		allocate_object_2(&ignored, &ignored, &object);
		move_halfwords(object, quicksand_parts_ADDRESS, 0x11u);
		OBJECT(object_2)->part_list = object;
	}
}

void quicksand_swirl(void)
{
	uint16_t margin = 0, object = 0, sprite_frame = 0, step_frame = 0;
	uint32_t depth = 0, ignored = 0, next_step = 0, object_2 = 0, off_screen = 0, part_cursor = 0, parts_left = 0, step_list = 0;
	uint8_t animation_timer = 0;
	uint16_t last_step = 0;

	margin = 0x70u;
	off_screen = check_left_screen_margin(margin);
	if (off_screen == 0u)
	{
		add_to_touchable_list(0u);
		update_screen_position(&ignored, &ignored);
		if ((OBJECT(current_object)->flags & 1u) != 0u)
			return;
		animate_sign();
		if ((int16_t)OBJECT(current_object)->child < 0)
		{
			current_object = object_cursor;
			depth = (uint16_t)(OBJECT(current_object)->screen_y - 0x58u);
			queue_sprite_at_depth(&ignored, &depth);
			return;
		}
		animation_timer = OBJECT(current_object)->animation_timer;
		OBJECT(current_object)->animation_timer = animation_timer - 1u;
		if ((int8_t)animation_timer <= 1)
		{
			OBJECT(current_object)->animation_timer = OBJECT(current_object)->animation_speed;
			step_list = OBJECT(current_object)->animation;
			next_step = OBJECT(current_object)->animation_frame;
			next_step = (uint8_t)(next_step + 1u);
			last_step = ROM16(step_list);
			step_list += 2u;
			if ((int16_t)last_step < (int16_t)next_step)
				next_step = 0u;
			OBJECT(current_object)->animation_frame = next_step;
			step_frame = ROM16(step_list + next_step * 2u);
			part_cursor = OBJECT(LINKED(current_object, OBJECT(current_object)->child))->part_list;
			parts_left = SPRITE_PART_LIST(part_cursor)->count;
			part_cursor += 2u;
			do
			{
				SPRITE_PART(part_cursor)->frame = step_frame;
				part_cursor += 8u;
				parts_left--;
			} while (parts_left != 0u);
		}
		object_2 = LINKED(current_object, OBJECT(current_object)->child);
		OBJECT(object_2)->screen_x = OBJECT(current_object)->screen_x;
		OBJECT(object_2)->screen_y = OBJECT(current_object)->screen_y;
		current_object = LINKED(current_object, OBJECT(current_object)->child);
		depth = (uint16_t)(OBJECT(current_object)->screen_y - 0x58u);
		queue_sprite_at_depth(&ignored, &depth);
		current_object = object_cursor;
		depth = (uint16_t)(OBJECT(current_object)->screen_y - 0x58u);
		queue_sprite_at_depth(&ignored, &depth);
		return;
	}
	object_2 = LINKED(current_object, OBJECT(current_object)->child);
	sprite_frame = OBJECT(object_2)->sprite_frame;
	free_object_2(sprite_frame);
	object = (uint16_t)object_2;
	free_object_2(object);
	delete_current_object();
}

void sinkhole_start(void)
{
	uint32_t growth_record = 0, ignored = 0;
	uint32_t sprite_frame = 0;

	OBJECT(current_object)->sprite_flags = 8u;
	sprite_frame = OBJECT(current_object)->sprite_frame;
	growth_record = sprite_frame & ~0x8000u;
	if ((sprite_frame & 0x8000u) != 0u)
		OBJECT(current_object)->sprite_flags |= 0x40u;
	growth_record = sinkhole_growth_records_ADDRESS + growth_record * 4u;
	OBJECT(current_object)->sinkhole.zoom_step = SINKHOLE_GROWTH(growth_record)->zoom_step;
	OBJECT(current_object)->sinkhole.open_position = SINKHOLE_GROWTH(growth_record)->open_delay;
	OBJECT(current_object)->sinkhole.open_position += scroll_position;
	OBJECT(current_object)->sprite_palette = 0x1020u;
	OBJECT(current_object)->sprite_scale = 1u;
	OBJECT(current_object)->flags = 0u;
	OBJECT(current_object)->sinkhole.unscaled_offset_x = -8;
	if ((OBJECT(current_object)->sprite_flags & 0x40u) != 0u)
		OBJECT(current_object)->sinkhole.unscaled_offset_x = 0u - OBJECT(current_object)->sinkhole.unscaled_offset_x;
	OBJECT(current_object)->sinkhole.unscaled_offset_y = -0x30;
	OBJECT(current_object)->hitbox_size_x = 1u;
	OBJECT(current_object)->sinkhole.unscaled_hitbox_size_x = 0x4Cu;
	OBJECT(current_object)->hitbox_height = 0x40u;
	OBJECT(current_object)->hitbox_size_y = 0u;
	OBJECT(current_object)->handler = sinkhole_open_ADDRESS;
	OBJECT(current_object)->kind = 0x1Bu;
	OBJECT(current_object)->direction = 0x200u;
	if ((OBJECT(current_object)->sprite_flags & 0x40u) != 0u)
		OBJECT(current_object)->direction = 0x300u;
	OBJECT(current_object)->animation_frame = 0u;
	OBJECT(current_object)->animation_speed = 4u;
	OBJECT(current_object)->animation_timer = 0;
	OBJECT(current_object)->animation = sinkhole_animation_ADDRESS;
	find_floor(&ignored, &ignored, &ignored, &ignored, &ignored);
	OBJECT(current_object)->z = OBJECT(current_object)->floor_z;
	project_object(&ignored, &ignored);
}

void sinkhole_open(void)
{
	uint16_t scale = 0;
	uint32_t ignored = 0, margin_or_scale = 0, off_screen = 0, sound_id = 0;

	margin_or_scale = 0x68u;
	off_screen = check_left_screen_margin((uint16_t)margin_or_scale);
	if (off_screen != 0u)
	{
		delete_current_object();
		return;
	}
	if (scroll_position < OBJECT(current_object)->sinkhole.open_position)
		return;
	if ((int16_t)OBJECT(current_object)->sprite_scale >= 0x180)
	{
		add_to_touchable_list(sound_id);
		update_screen_position(&ignored, &margin_or_scale);
		OBJECT(current_object)->draw_depth = OBJECT(current_object)->screen_y;
		margin_or_scale = queue_sprite(margin_or_scale);
		return;
	}
	if (OBJECT(current_object)->sprite_scale == 1u)
	{
		sound_id = 0xFu;
		play_sound((uint8_t)sound_id, &ignored, &ignored);
	}
	OBJECT(current_object)->sprite_scale += OBJECT(current_object)->sinkhole.zoom_step;
	scale = OBJECT(current_object)->sprite_scale;
	OBJECT(current_object)->sprite_offset_x = (int32_t)((uint32_t)OBJECT(current_object)->sinkhole.unscaled_offset_x * scale) >> 8;
	OBJECT(current_object)->sprite_offset_y = (int32_t)((uint32_t)OBJECT(current_object)->sinkhole.unscaled_offset_y * scale) >> 8;
	animate(&ignored, &ignored, &ignored);
	margin_or_scale = OBJECT(current_object)->sprite_scale;
	OBJECT(current_object)->hitbox_size_x = (int32_t)(OBJECT(current_object)->sinkhole.unscaled_hitbox_size_x * margin_or_scale) >> 8;
	add_to_touchable_list(sound_id);
	update_screen_position(&ignored, &margin_or_scale);
	OBJECT(current_object)->draw_depth = OBJECT(current_object)->screen_y;
	margin_or_scale = queue_sprite(margin_or_scale);
}

void sand_patch_start(void)
{
	uint32_t ignored = 0;

	OBJECT(current_object)->sprite_flags = 0x1000u;
	if ((OBJECT(current_object)->sprite_frame & 0x8000u) != 0u)
		OBJECT(current_object)->sprite_flags |= 0x40u;
	OBJECT(current_object)->sprite_frame = 0x7530u;
	OBJECT(current_object)->sprite_palette = 0x9020u;
	OBJECT(current_object)->handler = sand_patch_draw_ADDRESS;
	OBJECT(current_object)->flags = 0u;
	OBJECT(current_object)->sprite_offset_x = 0;
	OBJECT(current_object)->sprite_offset_y = 0;
	find_floor(&ignored, &ignored, &ignored, &ignored, &ignored);
	OBJECT(current_object)->z = OBJECT(current_object)->floor_z;
	project_object(&ignored, &ignored);
}

void sand_patch_draw(void)
{
	uint16_t margin = 0, unseen_margin = 0;
	uint32_t depth = 0, ignored = 0, off_screen = 0;

	margin = 0x38u;
	unseen_margin = 0x10u;
	off_screen = check_left_screen_margins(margin, unseen_margin);
	if (off_screen == 0u)
	{
		update_screen_position(&ignored, &ignored);
		depth = (uint16_t)(OBJECT(current_object)->screen_y - 0x38u);
		queue_sprite_at_depth(&ignored, &depth);
		return;
	}
	delete_current_object();
}

void rimmed_sinkhole_start(void)
{
	uint32_t free_count = 0, growth_record = 0, ignored = 0, object = 0;
	uint32_t sprite_frame = 0;

	OBJECT(current_object)->sprite_flags = 0x1000u;
	OBJECT(current_object)->hitbox_size_x = 0x70u;
	OBJECT(current_object)->hitbox_height = 0x40u;
	OBJECT(current_object)->sprite_offset_x = 0;
	OBJECT(current_object)->sprite_offset_y = 0;
	OBJECT(current_object)->hitbox_size_y = 0u;
	OBJECT(current_object)->flags_low = 0u;
	OBJECT(current_object)->kind = 0x1Du;
	allocate_object(&free_count, &object);
	if (free_count == 0u)
		return;
	OBJECT(object_cursor)->handler = rimmed_sinkhole_edge_zone_update_ADDRESS;
	current_object = object;
	OBJECT(object_cursor)->parent = current_object;
	OBJECT(current_object)->rimmed_sinkhole.edge_zone = object_cursor;
	OBJECT(current_object)->x = OBJECT(object_cursor)->x;
	OBJECT(current_object)->y = OBJECT(object_cursor)->y;
	OBJECT(current_object)->sprite_flags = 0u;
	sprite_frame = OBJECT(object_cursor)->sprite_frame;
	growth_record = sprite_frame & ~0x8000u;
	if ((sprite_frame & 0x8000u) != 0u)
		OBJECT(current_object)->sprite_flags |= 0x40u;
	growth_record = sinkhole_growth_records_ADDRESS + growth_record * 4u;
	OBJECT(current_object)->rimmed_sinkhole.zoom_step = SINKHOLE_GROWTH(growth_record)->zoom_step;
	OBJECT(current_object)->rimmed_sinkhole.open_position = SINKHOLE_GROWTH(growth_record)->open_delay;
	OBJECT(current_object)->rimmed_sinkhole.open_position += scroll_position;
	OBJECT(current_object)->sprite_frame = 0x7518u;
	OBJECT(current_object)->sprite_palette = 0x9020u;
	OBJECT(current_object)->sprite_scale = 1u;
	OBJECT(current_object)->flags = 0u;
	OBJECT(current_object)->sprite_offset_x = 0;
	OBJECT(current_object)->sprite_offset_y = 0;
	OBJECT(current_object)->handler = rimmed_sinkhole_open_ADDRESS;
	OBJECT(current_object)->hitbox_size_x = 1u;
	OBJECT(current_object)->rimmed_sinkhole.unscaled_hitbox_size_x = 0x4Cu;
	OBJECT(current_object)->hitbox_height = 0x40u;
	OBJECT(current_object)->hitbox_size_y = 0u;
	OBJECT(current_object)->kind = 0x1Bu;
	OBJECT(current_object)->direction = 0u;
	if ((OBJECT(current_object)->sprite_flags & 0x40u) != 0u)
		OBJECT(current_object)->direction = 0x100u;
	OBJECT(current_object)->animation = rimmed_sinkhole_animation_ADDRESS;
	OBJECT(current_object)->animation_frame = 0u;
	OBJECT(current_object)->animation_speed = 4u;
	OBJECT(current_object)->animation_timer = 0;
	find_floor(&ignored, &ignored, &ignored, &ignored, &object);
	OBJECT(current_object)->z = OBJECT(current_object)->floor_z;
	OBJECT(object_cursor)->z = OBJECT(current_object)->floor_z;
	project_object(&ignored, &ignored);
	allocate_object_2(&free_count, &ignored, &object);
	if (free_count == 0u)
		return;
	OBJECT(object)->sprite_flags = 0u;
	if ((OBJECT(current_object)->sprite_flags & 0x40u) != 0u)
		OBJECT(object)->sprite_flags |= 0x40u;
	OBJECT(object)->sprite_frame = 0x7538u;
	OBJECT(object)->sprite_palette = 0xF020u;
	OBJECT(object)->sprite_scale = 1u;
	OBJECT(current_object)->child = object;
}

void rimmed_sinkhole_open(void)
{
	uint16_t child = 0, margin = 0, unseen_margin = 0;
	uint32_t depth = 0, ignored = 0, off_screen = 0, sound_id = 0;

	margin = 0x58u;
	unseen_margin = 0x20u;
	off_screen = check_left_screen_margins(margin, unseen_margin);
	if (off_screen != 0u)
	{
		child = OBJECT(current_object)->child;
		free_object_2(child);
		current_object = LINKED(current_object, OBJECT(current_object)->rimmed_sinkhole.edge_zone);
		OBJECT(current_object)->flags |= 0x400u;
		delete_current_object();
		return;
	}
	if (scroll_position < OBJECT(current_object)->rimmed_sinkhole.open_position)
		return;
	if ((int16_t)OBJECT(current_object)->sprite_scale < 0x100)
	{
		if (OBJECT(current_object)->sprite_scale == 1u)
		{
			sound_id = 0xFu;
			play_sound((uint8_t)sound_id, &ignored, &ignored);
		}
		OBJECT(current_object)->sprite_scale += OBJECT(current_object)->rimmed_sinkhole.zoom_step;
		OBJECT(LINKED(current_object, OBJECT(current_object)->child))->sprite_scale = OBJECT(current_object)->sprite_scale;
		animate(&ignored, &ignored, &ignored);
		OBJECT(current_object)->hitbox_size_x = (int32_t)((uint32_t)OBJECT(current_object)->rimmed_sinkhole.unscaled_hitbox_size_x * OBJECT(current_object)->sprite_scale) >> 8;
	}
	add_to_touchable_list(sound_id);
	update_screen_position(&ignored, &ignored);
	depth = (uint16_t)(OBJECT(current_object)->screen_y - 0x30u);
	if ((int16_t)depth <= -0xF)
		depth = 0xFFF1u;
	OBJECT(current_object)->draw_depth = depth;
	queue_sprite_at_depth(&ignored, &depth);
	current_object = LINKED(current_object, OBJECT(current_object)->child);
	OBJECT(current_object)->screen_x = OBJECT(object_cursor)->screen_x;
	OBJECT(current_object)->screen_y = OBJECT(object_cursor)->screen_y;
	depth = (uint16_t)(OBJECT(object_cursor)->draw_depth - 8u);
	queue_sprite_at_depth(&ignored, &depth);
}

void rimmed_sinkhole_edge_zone_update(void)
{
	uint32_t ignored = 0, object = 0;

	if ((OBJECT(current_object)->flags & 0x400u) == 0u)
	{
		object = LINKED(current_object, OBJECT(current_object)->parent);
		OBJECT(current_object)->sprite_scale = OBJECT(object)->sprite_scale;
		OBJECT(current_object)->draw_depth = OBJECT(object)->draw_depth;
		ignored = add_to_touchable_list(0u);
		return;
	}
	delete_current_object();
}

void spawn_sand_block_leader(uint32_t record, uint32_t *p_spawned, uint32_t *p_leader)
{
	uint32_t leader = *p_leader;
	uint32_t free_count = 0, spawned = 0;

	allocate_object_outside_loop(&free_count, &leader);
	if (free_count == 0u)
	{
		*p_spawned = free_count;
		*p_leader = leader;
		return;
	}
	group_leaders[LEADER_SPAWN(record)->leader & 3u] = leader;
	record++;
	OBJECT(leader)->x_int = LEADER_SPAWN(record - 0x1u)->x;
	OBJECT(leader)->x_fraction = 0u;
	record += 2u;
	OBJECT(leader)->y_int = LEADER_SPAWN(record - 0x3u)->y;
	OBJECT(leader)->y_fraction = 0u;
	record += 2u;
	OBJECT(leader)->z_int = LEADER_SPAWN(record - 0x5u)->z;
	OBJECT(leader)->z_fraction = 0u;
	OBJECT(leader)->floor_z = OBJECT(leader)->z;
	record += 2u;
	OBJECT(leader)->direction = LEADER_SPAWN(record - 0x7u)->direction;
	OBJECT(leader)->sand_block_leader.path = LEADER_SPAWN(record - 0x7u)->path + group_leader_data_ADDRESS;
	OBJECT(leader)->handler = sand_block_leader_move_ADDRESS;
	OBJECT(leader)->flags = 0u;
	OBJECT(leader)->sprite_offset_x = 0;
	OBJECT(leader)->sprite_offset_y = 0;
	OBJECT(leader)->sand_block_leader.holding_back = 0u;
	OBJECT(leader)->sand_block_leader.wait_timer = 1;
	sand_block_leader_active = 0xFFu;
	if (group_leader_0_held != 0u)
		OBJECT(leader)->handler = sand_block_leader_wait_ADDRESS;
	spawned = 0xFFFFFFFFu;
	*p_spawned = spawned;
	*p_leader = leader;
}

void sand_block_leader_wait(void)
{
	if (scroll_position >= 0x5100u || group_leader_0_held == 0u)
		OBJECT(current_object)->handler = sand_block_leader_move_ADDRESS;
}

void sand_block_leader_move(void)
{
	uint16_t hold_threshold = 0, move_speed = 0;
	uint32_t block_record = 0, ignored = 0, path_cursor = 0;
	uint8_t hold_timer = 0;
	uint16_t wait_timer = 0, record_scroll = 0;
	uint32_t flags = 0;

	flags = OBJECT(current_object)->flags;
	OBJECT(current_object)->flags = flags | 0x80000000u;
	if ((flags & 0x80000000u) == 0u)
	{
	sand_block_leader_read_script:
		wait_timer = OBJECT(current_object)->sand_block_leader.wait_timer;
		OBJECT(current_object)->sand_block_leader.wait_timer = wait_timer - 1u;
		if ((int16_t)wait_timer <= 1)
		{
			OBJECT(current_object)->sand_block_leader.wait_timer = 0;
			path_cursor = OBJECT(current_object)->sand_block_leader.path;
			record_scroll = ROM16(path_cursor);
			path_cursor += 2u;
			if (record_scroll <= scroll_position)
			{
				if ((int16_t)ROM16(path_cursor + 2u) < 0)
				{
					delete_current_object();
					return;
				}
				OBJECT(current_object)->sand_block_leader.screen_threshold = ROM16(path_cursor);
				path_cursor += 2u;
				OBJECT(current_object)->sand_block_leader.catch_up_factor = ROM16(path_cursor);
				path_cursor += 2u;
				OBJECT(current_object)->sand_block_leader.base_speed = ROM16(path_cursor);
				path_cursor += 2u;
				OBJECT(current_object)->sand_block_leader.wait_timer = ROM16(path_cursor);
				OBJECT(current_object)->sand_block_leader.path = path_cursor + 2u;
			}
		}
	}
	else if (OBJECT(current_object)->sand_block_leader.holding_back == 0u)
	{
		if (no_player_in_play == 0u && player_mishap == 0u && player_dropping_in == 0u)
			goto sand_block_leader_read_script;
		OBJECT(current_object)->sand_block_leader.holding_back = 0xFFu;
		OBJECT(current_object)->sand_block_leader.saved_base_speed = OBJECT(current_object)->sand_block_leader.base_speed;
		OBJECT(current_object)->sand_block_leader.base_speed = 0;
		OBJECT(current_object)->sand_block_leader.hold_start_position = scroll_position;
		OBJECT(current_object)->sand_block_leader.hold_timer = 0xFFu;
		OBJECT(current_object)->sand_block_leader.saved_threshold = OBJECT(current_object)->sand_block_leader.screen_threshold;
		hold_threshold = 0xFFE0u;
		if ((OBJECT(current_object)->direction_low & 1u) != 0u)
			hold_threshold = 0x160u;
		OBJECT(current_object)->sand_block_leader.screen_threshold = hold_threshold;
	}
	find_floor(&ignored, &ignored, &ignored, &ignored, &ignored);
	project_object(&ignored, &ignored);
	add_to_rider_list();
	if (OBJECT(current_object)->sand_block_leader.holding_back != 0u)
	{
		if ((int16_t)(scroll_position - OBJECT(current_object)->sand_block_leader.hold_start_position) >= 0x300)
			goto sand_block_leader_stop_holding_back;
		if (no_player_in_play != 0u)
			OBJECT(current_object)->sand_block_leader.hold_timer = 0xFFu;
		hold_timer = OBJECT(current_object)->sand_block_leader.hold_timer;
		OBJECT(current_object)->sand_block_leader.hold_timer = hold_timer - 1u;
		if (hold_timer == 0u)
			goto sand_block_leader_stop_holding_back;
	}
	switch (OBJECT(current_object)->direction)
	{
	case 0:
	case 2:
	case 4:
		if (OBJECT(current_object)->sand_block_leader.screen_threshold <= OBJECT(current_object)->screen_x)
		{
		sand_block_leader_use_base_speed:
			move_speed = (uint16_t)OBJECT(current_object)->sand_block_leader.base_speed;
		}
		else
		{
		sand_block_leader_catch_up:
			OBJECT(current_object)->sand_block_leader.wait_timer = 0;
			if (OBJECT(current_object)->sand_block_leader.holding_back != 0u)
			{
			sand_block_leader_stop_holding_back:
				OBJECT(current_object)->sand_block_leader.holding_back = 0u;
				OBJECT(current_object)->sand_block_leader.screen_threshold = OBJECT(current_object)->sand_block_leader.saved_threshold;
				OBJECT(current_object)->sand_block_leader.base_speed = OBJECT(current_object)->sand_block_leader.saved_base_speed;
			}
			compute_camera_ground_speed(ignored);
			move_speed = (uint16_t)((uint32_t)camera_ground_speed_int * OBJECT(current_object)->sand_block_leader.catch_up_factor) >> 8;
			if ((int16_t)move_speed <= OBJECT(current_object)->sand_block_leader.base_speed)
				goto sand_block_leader_use_base_speed;
		}
		break;
	case 1:
	case 3:
		if (OBJECT(current_object)->sand_block_leader.screen_threshold < OBJECT(current_object)->screen_x || OBJECT(current_object)->screen_y > 0xE0)
			goto sand_block_leader_catch_up;
		goto sand_block_leader_use_base_speed;
	default:
		invalid_jump(0x8A046u + OBJECT(current_object)->direction * 2u);
	}
	chaser_speed = move_speed;
	if (OBJECT(current_object)->sand_block_leader.wait_timer <= 0)
	{
		block_record = floor_blocks + 2u + OBJECT(current_object)->floor_block;
		switch (OBJECT(current_object)->direction)
		{
		case 0:
		case 4:
			OBJECT(current_object)->y += (uint32_t)chaser_speed << 12;
			OBJECT(current_object)->x = (uint32_t)((int32_t)((uint32_t)FLOOR_BLOCK(block_record)->x_max - FLOOR_BLOCK(block_record)->x_min) >> 1) + FLOOR_BLOCK(block_record)->x_min;
			break;
		case 1:
			OBJECT(current_object)->y -= (uint32_t)chaser_speed << 12;
			OBJECT(current_object)->x = (uint32_t)((int32_t)((uint32_t)FLOOR_BLOCK(block_record)->x_max - FLOOR_BLOCK(block_record)->x_min) >> 1) + FLOOR_BLOCK(block_record)->x_min;
			break;
		case 2:
			OBJECT(current_object)->x += (uint32_t)chaser_speed << 12;
			OBJECT(current_object)->y = (uint32_t)((int32_t)((uint32_t)FLOOR_BLOCK(block_record)->y_max - FLOOR_BLOCK(block_record)->y_min) >> 1) + FLOOR_BLOCK(block_record)->y_min;
			break;
		case 3:
			OBJECT(current_object)->x -= (uint32_t)chaser_speed << 12;
			OBJECT(current_object)->y = (uint32_t)((int32_t)((uint32_t)FLOOR_BLOCK(block_record)->y_max - FLOOR_BLOCK(block_record)->y_min) >> 1) + FLOOR_BLOCK(block_record)->y_min;
			break;
		default:
			invalid_jump(0x8A0D4u + OBJECT(current_object)->direction * 2u);
		}
	}
	if (OBJECT(current_object)->y_int <= 0xA2B)
		sand_block_leader_active = 0u;
}

void sand_block_update(void)
{
	uint8_t sound_id = 0;
	uint16_t child = 0, part_scale = 0, span_y = 0;
	uint32_t blocks_left = 0, depth = 0, floor_z = 0, ignored = 0, object = 0, object_2 = 0, object_3 = 0, part_cursor = 0, part_list = 0, point_x = 0, point_y = 0, stream_parts_left = 0;

	switch (OBJECT(current_object)->sand_block.state)
	{
	case 0:
	case 4:
		project_object(&ignored, &ignored);
		OBJECT(current_object)->sand_block.state++;
		current_player = current_object;
		current_player = LINKED(current_player, OBJECT(current_object)->parent);
		switch (OBJECT(current_object)->direction)
		{
		case 0:
		case 1:
		case 2:
		case 3:
		case 4:
			break;
		default:
			invalid_jump(0x8A1A5u + OBJECT(current_object)->direction * 2u);
		}
		if (OBJECT(current_player)->y_int <= (int16_t)(OBJECT(current_object)->y_int + OBJECT(current_object)->hitbox_size_y - 0x10u))
			OBJECT(current_object)->sand_block.state++;
		add_to_platform_list();
		update_screen_position(&ignored, &ignored);
		if ((uint16_t)(OBJECT(current_object)->screen_x + 0x140u) >= 0x281u)
			return;
		depth = (uint16_t)(OBJECT(current_object)->screen_y + OBJECT(current_object)->sand_block.depth_offset - OBJECT(current_object)->sprite_offset_y);
		OBJECT(current_object)->draw_depth = depth;
		queue_sprite_at_depth(&ignored, &depth);
		return;
	case 1:
		current_player = current_object;
		current_player = LINKED(current_player, OBJECT(current_object)->parent);
		switch (OBJECT(current_object)->direction)
		{
		case 0:
		case 1:
		case 2:
		case 3:
		case 4:
			break;
		default:
			invalid_jump(0x8A1A5u + OBJECT(current_object)->direction * 2u);
		}
		if (OBJECT(current_player)->y_int <= (int16_t)(OBJECT(current_object)->y_int + OBJECT(current_object)->hitbox_size_y - 0x10u))
			OBJECT(current_object)->sand_block.state++;
		add_to_platform_list();
		update_screen_position(&ignored, &ignored);
		if ((uint16_t)(OBJECT(current_object)->screen_x + 0x140u) >= 0x281u)
			return;
		depth = (uint16_t)(OBJECT(current_object)->screen_y + OBJECT(current_object)->sand_block.depth_offset - OBJECT(current_object)->sprite_offset_y);
		OBJECT(current_object)->draw_depth = depth;
		queue_sprite_at_depth(&ignored, &depth);
		return;
	case 2:
		if (free_object_stack < 2 || free_object_stack_2[0] < 3)
		{
			add_to_platform_list();
			update_screen_position(&ignored, &ignored);
			if ((uint16_t)(OBJECT(current_object)->screen_x + 0x140u) >= 0x281u)
				return;
			depth = (uint16_t)(OBJECT(current_object)->screen_y + OBJECT(current_object)->sand_block.depth_offset - OBJECT(current_object)->sprite_offset_y);
			OBJECT(current_object)->draw_depth = depth;
			queue_sprite_at_depth(&ignored, &depth);
			return;
		}
		OBJECT(current_object)->sand_block.state++;
		allocate_object(&ignored, &object_2);
		OBJECT(object_2)->sprite_flags = 0xD002u;
		OBJECT(object_2)->handler = sand_face_update_ADDRESS;
		OBJECT(object_2)->flags = 0u;
		OBJECT(object_2)->speed_z = 0;
		OBJECT(object_2)->sand_face.depth_offset = 0x70;
		OBJECT(object_2)->animation_frame = 0u;
		OBJECT(object_2)->animation_speed = 2u;
		OBJECT(object_2)->animation_timer = 0;
		OBJECT(object_2)->animation = sand_face_animation_ADDRESS;
		OBJECT(object_2)->x = OBJECT(current_object)->x;
		OBJECT(object_2)->y = OBJECT(current_object)->y;
		OBJECT(object_2)->sprite_offset_x = -6;
		OBJECT(object_2)->sprite_offset_y = 0x10;
		OBJECT(object_2)->z = OBJECT(current_object)->z;
		OBJECT(object_2)->parent = current_object;
		OBJECT(current_object)->sand_block.face_object = object_2;
		object_3 = object_2;
		allocate_object_2(&ignored, &ignored, &object_2);
		move_halfwords_backward(object_2, sand_face_parts_ADDRESS, 9u);
		part_list = OBJECT(current_object)->part_list;
		OBJECT(object_2)->sprite_scale = ROM16(part_list + 8u);
		OBJECT(object_2)->hitbox_size_x = ROM16(part_list + 8u);
		OBJECT(object_3)->part_list = object_2;
		allocate_object(&ignored, &object_2);
		OBJECT(object_2)->sprite_flags = 0xD002u;
		OBJECT(object_2)->handler = sand_streams_update_ADDRESS;
		OBJECT(object_2)->flags = 0u;
		move_halfwords_backward(object_2 + 0x40u, sand_stream_parts_ADDRESS, 0x19u);
		OBJECT(object_2)->part_list = object_2 + 0x40u;
		part_scale = (uint16_t)mem_read32(OBJECT(object_3)->part_list + 8u);
		part_cursor = OBJECT(object_2)->part_list;
		stream_parts_left = SPRITE_PART_LIST(part_cursor)->count;
		part_cursor += 2u;
		do
		{
			SPRITE_PART(part_cursor)->palette = part_scale;
			part_cursor += 8u;
			stream_parts_left--;
		} while (stream_parts_left != 0u);
		OBJECT(object_2)->parent = object_3;
		OBJECT(object_3)->child = object_2;
		blocks_left = 2u;
		object_2 = current_object;
		do
		{
			object_3 = object_2;
			allocate_object_2(&ignored, &ignored, &object_2);
			object = object_2 + 2u;
			OBJECT(object - 0x2u)->sprite_flags = 0x5000u;
			object += 2u;
			OBJECT(object - 0x4u)->sprite_frame = 0xE0u;
			object += 2u;
			OBJECT(object - 0x6u)->sprite_palette = 0u;
			object += 2u;
			OBJECT(object - 0x8u)->sprite_scale = 0x140u;
			object += 2u;
			OBJECT(object - 0xAu)->sprite_layer = 0u;
			object += 2u;
			OBJECT(object - 0xCu)->screen_y = 0;
			object += 2u;
			OBJECT(object - 0xEu)->screen_x = 0;
			OBJECT(object - 0xEu)->hitbox_size_x = 0u;
			OBJECT(object_2)->parent = object_3;
			OBJECT(object_3)->child = object_2;
			blocks_left--;
		} while (blocks_left != 0u);
		if (OBJECT(current_object)->screen_x <= 0x148 && OBJECT(current_object)->screen_y <= 0x130)
		{
			sound_id = 0x11u;
			play_sound(sound_id, &ignored, &ignored);
		}
	case 3:
		update_screen_position(&ignored, &ignored);
		object = LINKED(current_object, OBJECT(current_object)->parent);
		span_y = (uint16_t)(OBJECT(object)->y_int - OBJECT(current_object)->y_int);
		OBJECT(current_object)->hitbox_size_y = span_y;
		if ((int16_t)span_y > 0)
		{
			point_x = OBJECT(current_object)->x + ((uint32_t)OBJECT(current_object)->hitbox_size_x << 16);
			point_y = OBJECT(object)->y;
			floor_z = OBJECT(current_object)->floor_z;
			project_point(floor_z, &ignored, &point_x, &point_y);
			OBJECT(LINKED(current_object, OBJECT(current_object)->child))->sprite_scale = point_x;
			add_to_platform_list();
			if ((uint16_t)(OBJECT(current_object)->screen_x + 0x140u) < 0x281u)
			{
				OBJECT(current_object)->draw_depth = OBJECT(current_object)->screen_y + OBJECT(current_object)->sand_block.depth_offset - OBJECT(current_object)->sprite_offset_y;
				current_object = LINKED(current_object, OBJECT(current_object)->child);
				current_object = LINKED(current_object, OBJECT(current_object)->child);
				depth = OBJECT(object_cursor)->draw_depth;
				queue_sprite_at_depth(&ignored, &depth);
				current_object = object_cursor;
				depth = OBJECT(object_cursor)->draw_depth;
				queue_sprite_at_depth(&ignored, &depth);
				current_object = LINKED(current_object, OBJECT(current_object)->child);
				depth = OBJECT(object_cursor)->draw_depth;
				queue_sprite_at_depth(&ignored, &depth);
			}
			return;
		}
		object = LINKED(current_object, OBJECT(current_object)->sand_block.face_object);
		OBJECT(object)->flags |= 0x400u;
		child = OBJECT(current_object)->child;
		free_object_2(child);
		current_object = LINKED(current_object, OBJECT(current_object)->child);
		child = OBJECT(current_object)->child;
		free_object_2(child);
		delete_current_object();
		return;
	default:
		invalid_jump(0x8A17Cu + OBJECT(current_object)->sand_block.state * 3u);
	}
}

void sand_face_update(void)
{
	uint16_t part_list = 0;
	uint32_t depth = 0, ignored = 0, object = 0;

	if ((OBJECT(current_object)->flags & 0x400u) == 0u)
	{
		object = LINKED(current_object, OBJECT(current_object)->parent);
		object = LINKED(object, OBJECT(object)->parent);
		OBJECT(current_object)->y = OBJECT(object)->y;
		OBJECT(current_object)->z = OBJECT(object)->z;
	}
	else
	{
		if (OBJECT(current_object)->screen_y >= 0x150)
		{
			part_list = (uint16_t)OBJECT(current_object)->part_list;
			free_object_2(part_list);
			object = LINKED(current_object, OBJECT(current_object)->child);
			OBJECT(object)->flags |= 0x400u;
			delete_current_object();
			return;
		}
		OBJECT(current_object)->sand_face.depth_offset = 0x38;
		OBJECT(current_object)->animation = sand_face_fall_animation_ADDRESS;
		OBJECT(current_object)->speed_z += 0x6000u;
		OBJECT(current_object)->z += OBJECT(current_object)->speed_z;
	}
	animate_all_parts(&ignored, &ignored);
	project_object(&ignored, &ignored);
	depth = (uint16_t)(OBJECT(current_object)->screen_y - OBJECT(current_object)->sand_face.depth_offset);
	queue_sprite_at_depth(&ignored, &depth);
}

void sand_streams_update(void)
{
	uint16_t stream_frame = 0;
	uint32_t depth = 0, ignored = 0, object = 0, part_cursor = 0, parts_left = 0;

	if ((OBJECT(current_object)->flags & 0x400u) != 0u)
	{
		delete_current_object();
		return;
	}
	object = LINKED(current_object, OBJECT(current_object)->parent);
	OBJECT(current_object)->screen_x = OBJECT(object)->screen_x;
	OBJECT(current_object)->screen_x -= OBJECT(object)->sprite_offset_x;
	OBJECT(current_object)->screen_y = OBJECT(object)->screen_y;
	OBJECT(current_object)->screen_y -= OBJECT(object)->sprite_offset_y;
	stream_frame = sand_stream_frames[OBJECT(object)->animation_frame];
	part_cursor = OBJECT(current_object)->part_list;
	parts_left = SPRITE_PART_LIST(part_cursor)->count;
	part_cursor += 2u;
	do
	{
		SPRITE_PART(part_cursor)->frame = stream_frame;
		part_cursor += 8u;
		parts_left--;
	} while (parts_left != 0u);
	depth = (uint16_t)(OBJECT(object)->screen_y - OBJECT(object)->sand_face.depth_offset);
	queue_sprite_at_depth(&ignored, &depth);
}

uint32_t spawn_sink_swirl(uint32_t allocated)
{
	uint32_t effect_record = 0, ignored = 0, object = 0, swirls_left = 0;

	if (free_object_stack < 4)
		return allocated;
	effect_record = sink_swirl_effects_ADDRESS;
	swirls_left = 2u;
	do
	{
		allocate_object(&ignored, &allocated);
		OBJECT(allocated)->sprite_flags = 0x1004u;
		OBJECT(allocated)->sprite_palette = 0x1210u;
		OBJECT(allocated)->flags_low = 0u;
		OBJECT(allocated)->sprite_offset_x = 0;
		OBJECT(allocated)->sprite_offset_y = 0;
		OBJECT(allocated)->sink_swirl.depth_offset = ROM16(effect_record);
		OBJECT(allocated)->handler = sink_swirl_start_ADDRESS;
		effect_record += 2u;
		OBJECT(allocated)->animation = ROM32(effect_record);
		OBJECT(allocated)->animation_frame = 0xFFu;
		OBJECT(allocated)->animation_speed = 0xAu;
		OBJECT(allocated)->animation_timer = 0;
		OBJECT(allocated)->x = OBJECT(current_object)->x;
		OBJECT(allocated)->y = OBJECT(current_object)->y;
		OBJECT(allocated)->z = OBJECT(current_object)->z;
		OBJECT(allocated)->z_int += 8u;
		object = allocated;
		effect_record += 4u;
		take_free_object(&ignored, &allocated);
		OBJECT(allocated)->sprite_flags = 0x1004u;
		OBJECT(allocated)->sprite_palette = 0x1230u;
		OBJECT(allocated)->flags_low = 0u;
		OBJECT(allocated)->animation = ROM32(effect_record);
		OBJECT(allocated)->animation_frame = 0xFFu;
		OBJECT(allocated)->animation_speed = 4u;
		OBJECT(allocated)->animation_timer = 0;
		OBJECT(object)->child = allocated;
		swirls_left--;
		effect_record += 4u;
	} while (swirls_left != 0u);
	return allocated;
}

void sink_swirl_start(void)
{
	uint32_t ignored = 0;

	project_object(&ignored, &ignored);
	OBJECT(current_object)->handler = sink_swirl_animate_ADDRESS;
	sink_swirl_animate();
}

void sink_swirl_animate(void)
{
	uint32_t at_last_step = 0, depth = 0, ignored = 0;

	animate(&ignored, &ignored, &at_last_step);
	if ((uint8_t)at_last_step != 0u)
	{
		current_object = LINKED(current_object, OBJECT(current_object)->child);
		push_free_object();
		delete_current_object();
		return;
	}
	update_screen_position(&ignored, &ignored);
	current_object = LINKED(current_object, OBJECT(current_object)->child);
	animate(&ignored, &ignored, &ignored);
	OBJECT(current_object)->screen_x = OBJECT(object_cursor)->screen_x;
	OBJECT(current_object)->screen_y = OBJECT(object_cursor)->screen_y;
	depth = (uint16_t)(OBJECT(current_object)->screen_y + OBJECT(object_cursor)->sink_swirl.depth_offset);
	queue_sprite_at_depth(&ignored, &depth);
	current_object = object_cursor;
	depth = (uint16_t)(OBJECT(current_object)->screen_y + OBJECT(current_object)->sink_swirl.depth_offset);
	queue_sprite_at_depth(&ignored, &depth);
}
