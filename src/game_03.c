/**
 *    Name:        game_03.c
 *    Description: the game's functions, part 03 (in the order of their V60 addresses)
 *    Version:     0.1.0
 *    Created:     12-10-2026
 *    Modified:    13-10-2026
 *    Author:      Sebastian Toyos
 *
 *    SegaSonic The Hedgehog (arcade, Sega System 32): the game's V60 program decompiled into C by
 *    tools/decompile.py, with the names of symbols.txt.
 */
#include "game.h"

void cactus_update(void)
{
	uint32_t r2 = 0, r3 = 0, r11 = 0, bucket = 0, deleted = 0, direction = 0, ignored = 0, speed_x = 0, speed_y = 0, spin_speed = 0;

	move_object_projected(&ignored, &r2, &r3, &r11);
	delete_if_off_screen(&deleted, &r2, &r3);
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
		fall_to_floor(&ignored, &spin_speed, &r2);
		if (nearest_player_distance <= 0x60 && (uint16_t)OBJECT(current_object)->floor_z_int == (uint16_t)OBJECT(current_object)->z_int)
			OBJECT(current_object)->z_int -= 3u;
	}
	queue_sprite_at_floor();
	current_object = LINKED(current_object, OBJECT(current_object)->child);
	bucket = place_sprite_at_cursor_floor();
	queue_sprite_in_bucket(bucket);
}

uint32_t spawn_whirlwind(uint32_t spawned, uint32_t r2, uint32_t whirlwind, uint32_t object)
{
	uint32_t piece_count = 0;

	spawn_count = 1u;
	spawn_object_with_pieces(0x01217Cu, &spawned, &piece_count, &r2, &whirlwind, &object);
	if ((uint8_t)spawned != 0u)
	{
		current_object = whirlwind;
		copy_position_to(whirlwind);
		whirlwind_spin_step += 0xBu;
		whirlwind_spin_step &= 0x3Fu;
		OBJECT(current_object)->sprite_palette = 0x90F0u;
		OBJECT(current_object)->var_86 = whirlwind_spin_step;
		OBJECT(current_object)->var_81 = OBJECT(current_object)->var_81;
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
	uint32_t r2 = 0, r3 = 0, r11 = 0, aim_speed = 0, deleted = 0, direction = 0, frame_index = 0, ignored = 0, speed = 0, speed_x = 0, speed_y = 0, spin_angle = 0;

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
	move_object_projected(&ignored, &r2, &r3, &r11);
	delete_if_off_screen(&deleted, &r2, &r3);
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
	uint32_t r2 = 0, r3 = 0, r11 = 0, ignored = 0;
	uint16_t odd_frame = 0;

	move_object_projected(&ignored, &r2, &r3, &r11);
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
	delete_object_and_children(&ignored, &r2, &r3);
}

void centipede_init(void)
{
	uint16_t follow_delay = 0;
	uint32_t block = 0, free_count = 0, object = 0, piece_count = 0, previous_segment = 0, spawned = 0;

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
		spawn_object_with_pieces(0x012351u, &spawned, &piece_count, &free_count, &object, &block);
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
	AS32(OBJECT(current_object)->spawn_arg_1) = previous_segment;
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
		OBJECT(current_object)->projected_x = mem_read16(path_cursor);
		OBJECT(current_object)->projected_y_int = mem_read16(path_cursor + 2u);
		path_cursor = (uint8_t)OBJECT(current_object)->centipede.path_write_index * 4u + centipede_path_ADDRESS;
		mem_write16(path_cursor, OBJECT(current_object)->projected_x);
		path_cursor += 2u;
		mem_write16(path_cursor, OBJECT(current_object)->projected_y_int);
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
	uint32_t r2 = 0, r3 = 0, r11 = 0, deleted = 0, ignored = 0;

	move_object_projected(&ignored, &r2, &r3, &r11);
	delete_if_off_screen(&deleted, &r2, &r3);
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

void centipede_move(uint32_t r11)
{
	uint16_t saved_direction = 0;
	uint32_t r0 = 0, chase_speed = 0, direction = 0, head_frame = 0, speed = 0, speed_x = 0, speed_y = 0, wobble_speed_x = 0, wobble_speed_y = 0;

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
	r0 = OBJECT(current_object)->centipede.wobble_angle;
	speed = r14;
	direction_to_speed(speed, &r0, &wobble_speed_x, &wobble_speed_y);
	OBJECT(current_object)->speed_x += wobble_speed_x;
	OBJECT(current_object)->speed_y += wobble_speed_y;
	if (centipede_command != 1u)
	{
		r0 = (uint8_t)OBJECT(current_object)->centipede.path_write_index * 4u + centipede_path_ADDRESS;
		mem_write16(r0, OBJECT(current_object)->projected_x);
		r0 += 2u;
		mem_write16(r0, OBJECT(current_object)->projected_y_int);
		OBJECT(current_object)->centipede.path_write_index++;
		r0 += 2u;
		if (OBJECT(current_object)->centipede.path_write_index > 0x32)
			OBJECT(current_object)->centipede.path_write_index = 0;
	}
	centipede_hop(&r0, &wobble_speed_x);
	if (OBJECT(current_object)->centipede.hop_timer == 0x5Au)
		spawn_centipede_sand_puff(r0, wobble_speed_x, wobble_speed_y, r11);
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
		delete_object_and_children(&ignored, &r2, &r3);
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
		delete_object_and_children(&ignored, &r2, &r3);
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
	OBJECT(current_object)->projected_x = mem_read16(path_cursor);
	path_cursor += 2u;
	OBJECT(current_object)->projected_y_int = mem_read16(path_cursor);
	OBJECT(current_object)->centipede_follow_path.path_index++;
	path_cursor += 2u;
	if (OBJECT(current_object)->centipede_follow_path.path_index > 0x32)
		OBJECT(current_object)->centipede_follow_path.path_index = 0;
	return path_cursor;
}

void centipede_hop(uint32_t *p_r0, uint32_t *p_r2)
{
	uint32_t r0 = *p_r0;
	uint32_t r2 = *p_r2;
	uint32_t r1 = 0, fall_height = 0;

	if (OBJECT(current_object)->centipede.hop_timer == 0u)
	{
		fall_to_floor(&fall_height, &r1, &r2);
		OBJECT(current_object)->speed_z -= 0x4000u;
		if (OBJECT(current_object)->z < 8)
		{
			*p_r0 = fall_height;
			*p_r2 = r2;
			return;
		}
		OBJECT(current_object)->speed_z = 0;
		OBJECT(current_object)->centipede.hop_timer = 0x5Au;
		*p_r0 = fall_height;
		*p_r2 = r2;
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

void spawn_centipede_sand_puff(uint32_t spawned, uint32_t r2, uint32_t puff, uint32_t r11)
{
	uint32_t piece_count = 0;

	spawn_count = 1u;
	spawn_object_with_pieces(0x012698u, &spawned, &piece_count, &r2, &puff, &r11);
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
	uint32_t r11 = 0, ignored = 0;

	unproject_object(&ignored, &ignored, &ignored, &r11);
	add_to_rider_list();
	OBJECT(current_object)->handler = sand_puff_animate_ADDRESS;
}

void sand_puff_animate(void)
{
	uint32_t r2 = 0, r3 = 0, r11 = 0, ignored = 0, next_frame = 0;
	uint16_t lifetime = 0;

	unproject_object(&ignored, &r2, &r3, &r11);
	if (OBJECT(current_object)->floor_z_int >= 0x800)
	{
		delete_object_and_children(&ignored, &r2, &r3);
		return;
	}
	lifetime = OBJECT(current_object)->sand_puff.lifetime;
	OBJECT(current_object)->sand_puff.lifetime = lifetime - 1u;
	if (lifetime == 0u)
	{
		delete_object_and_children(&ignored, &r2, &r3);
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

void crawler_init(void)
{
	uint8_t segment_walk_step = 0;
	uint16_t segment_x = 0, step_stagger = 0;
	uint32_t block = 0, free_count = 0, object = 0, offset_record = 0, piece_count = 0, rom_record = 0, variant = 0;

	variant = OBJECT(current_object)->crawler.variant;
	r14 = 8u;
	step_stagger = 0u;
	segment_walk_step = 0u;
	segment_x = 0u;
	current_player = 0u;
	do
	{
		spawn_count = 2u;
		spawn_object_with_pieces(0x01276Bu, &offset_record, &piece_count, &free_count, &object, &block);
		copy_position_to(object);
		offset_record = crawler_segment_offsets_ADDRESS + variant * 4u;
		segment_x += ROM16(offset_record);
		current_player = (current_player & ~0xFFFFu) | (uint16_t)(current_player + ROM16(offset_record + 2u));
		OBJECT(object)->projected_x += segment_x;
		OBJECT(object)->projected_y_int += current_player;
		segment_walk_step++;
		if ((int8_t)segment_walk_step > 5)
			segment_walk_step = 0u;
		OBJECT(object)->direction = 0x280u;
		step_stagger += 4u;
		OBJECT(object)->crawler.step_timer = step_stagger;
		OBJECT(object)->crawler.walk_step = segment_walk_step;
		OBJECT(object)->sprite_palette = 0x90A0u;
		OBJECT(object)->flags |= 2u;
		OBJECT(object)->hitbox_size_x = 0x29u;
		OBJECT(object)->hitbox_size_y = 0x2Fu;
		OBJECT(object)->hitbox_offset_x = 0xA;
		OBJECT(object)->hitbox_offset_y = -0x3D;
		OBJECT(object)->hitbox_height = 0x20u;
		OBJECT(object)->kind = 4u;
		OBJECT(object)->handler = crawler_segment_animate_ADDRESS;
		OBJECT(object)->spawn_arg_1 = OBJECT(object_cursor)->spawn_arg_1;
		OBJECT(object)->spawn_arg_2 = OBJECT(object_cursor)->spawn_arg_2;
		current_object = object;
		object = LINKED(object, OBJECT(object)->child);
		OBJECT(object)->sprite_frame = 0x5D80u;
		OBJECT(object)->sprite_palette = 0x9FFEu;
		OBJECT(object)->sprite_scale = 0x100u;
		r14 = r14 - 1u;
	} while (r14 != 0u);
	OBJECT(current_object)->direction = 0x280u;
	OBJECT(current_object)->var_80 = 0xFFu;
	rom_record = crawler_tail_offsets_ADDRESS + variant * 4u;
	OBJECT(current_object)->projected_x += ROM16(rom_record);
	OBJECT(current_object)->projected_y_int += ROM16(rom_record + 2u);
	OBJECT(current_object)->sprite_frame = crawler_tail_frames[variant];
	OBJECT(current_object)->sprite_palette = 0x90C0u;
	rom_record = crawler_collision_boxes_ADDRESS + variant * 6u;
	OBJECT(current_object)->hitbox_size_x = ROM16(rom_record);
	rom_record += 2u;
	OBJECT(current_object)->hitbox_size_y = ROM16(rom_record);
	rom_record += 2u;
	OBJECT(current_object)->hitbox_offset_x = ROM8(rom_record);
	OBJECT(current_object)->hitbox_offset_y = ROM8(rom_record + 1u);
	OBJECT(current_object)->flags |= 2u;
	OBJECT(current_object)->hitbox_height = 0x20u;
	OBJECT(current_object)->kind = 0xDu;
	OBJECT(current_object)->handler = crawler_segment_update_ADDRESS;
	OBJECT(object_cursor)->parent = object_cursor;
	rom_record = crawler_head_offsets_ADDRESS + variant * 4u;
	OBJECT(object_cursor)->projected_x += ROM16(rom_record);
	OBJECT(object_cursor)->projected_y_int += ROM16(rom_record + 2u);
	OBJECT(object_cursor)->sprite_palette = 0x90B0u;
	variant = OBJECT(object_cursor)->crawler.variant;
	OBJECT(object_cursor)->sprite_frame = crawler_head_frames[variant];
	rom_record = crawler_collision_boxes_ADDRESS + variant * 6u;
	OBJECT(object_cursor)->hitbox_size_x = ROM16(rom_record);
	rom_record += 2u;
	OBJECT(object_cursor)->hitbox_size_y = ROM16(rom_record);
	rom_record += 2u;
	OBJECT(object_cursor)->hitbox_offset_x = ROM8(rom_record);
	OBJECT(object_cursor)->hitbox_offset_y = ROM8(rom_record + 1u);
	OBJECT(object_cursor)->hitbox_height = 0x20u;
	OBJECT(object_cursor)->kind = 0xDu;
	OBJECT(object_cursor)->flags |= 2u;
	OBJECT(object_cursor)->handler = crawler_head_update_ADDRESS;
	current_object = LINKED(current_object, OBJECT(object_cursor)->child);
	OBJECT(current_object)->sprite_frame = 0x5D80u;
	OBJECT(current_object)->sprite_palette = 0x9FFEu;
	OBJECT(current_object)->sprite_scale = 0x100u;
}

void crawler_head_update(void)
{
	uint32_t r2 = 0, r3 = 0, r11 = 0, deleted = 0, depth = 0, ignored = 0, is_over_pit = 0;

	move_object_projected(&ignored, &r2, &r3, &r11);
	fall_to_floor(&ignored, &is_over_pit, &r2);
	OBJECT(current_object)->crawler_head.over_pit = is_over_pit;
	delete_if_off_screen(&deleted, &r2, &r3);
	if ((uint8_t)deleted == 1u)
		return;
	find_nearest_player_x();
	if (OBJECT(current_object)->crawler_head.variant != 0u)
	{
		if (nearest_player_distance <= 0x35)
			OBJECT(current_object)->crawler_head.triggered = 1u;
	}
	else if (nearest_player_distance <= 0x28)
	{
		OBJECT(current_object)->crawler_head.triggered = 1u;
		if (nearest_player_distance <= 0x35)
			OBJECT(current_object)->crawler_head.triggered = 1u;
		OBJECT(current_object)->direction = 0x280u;
		add_to_touchable_list(deleted);
		add_to_rider_list();
		if (OBJECT(current_object)->crawler_head.over_pit != 0u && OBJECT(current_object)->crawler_head.on_platform != 0u)
		{
			depth = (uint16_t)(OBJECT(LINKED(WORK_RAM, OBJECT(current_object)->platform))->var_88 - 0xFu);
			queue_sprite_at_depth(&ignored, &depth);
			crawler_step();
			return;
		}
		queue_sprite(is_over_pit);
		current_object = LINKED(current_object, OBJECT(current_object)->child);
		place_sprite_at_cursor_floor();
		depth = (uint16_t)(OBJECT(current_object)->screen_y - 0xAu);
		queue_sprite_at_depth(&ignored, &depth);
		current_object = object_cursor;
		crawler_step();
		return;
	}
	OBJECT(current_object)->direction = 0x280u;
	add_to_touchable_list(deleted);
	add_to_rider_list();
	if (OBJECT(current_object)->crawler_head.over_pit != 0u && OBJECT(current_object)->crawler_head.on_platform != 0u)
	{
		depth = (uint16_t)(OBJECT(LINKED(WORK_RAM, OBJECT(current_object)->platform))->var_88 - 0xFu);
		queue_sprite_at_depth(&ignored, &depth);
		crawler_step();
		return;
	}
	queue_sprite(is_over_pit);
	current_object = LINKED(current_object, OBJECT(current_object)->child);
	place_sprite_at_cursor_floor();
	depth = (uint16_t)(OBJECT(current_object)->screen_y - 0xAu);
	queue_sprite_at_depth(&ignored, &depth);
	current_object = object_cursor;
	crawler_step();
}

void crawler_segment_animate(void)
{
	OBJECT(current_object)->crawler.frame_timer++;
	if (OBJECT(current_object)->crawler.frame_timer <= 3)
	{
		crawler_segment_update();
		return;
	}
	OBJECT(current_object)->crawler.frame_timer = 0;
	OBJECT(current_object)->sprite_frame = crawler_segment_frames[mem_read8(crawler_walk_steps_ADDRESS + (OBJECT(current_object)->crawler.variant * 6u + (uint8_t)OBJECT(current_object)->crawler.walk_step))];
	OBJECT(current_object)->crawler.walk_step++;
	if (OBJECT(current_object)->crawler.walk_step <= 5)
	{
		crawler_segment_update();
		return;
	}
	OBJECT(current_object)->crawler.walk_step = 0;
	crawler_segment_update();
}

void crawler_segment_update(void)
{
	uint32_t r2 = 0, r3 = 0, r11 = 0, deleted = 0, depth = 0, ignored = 0, is_over_pit = 0;

	move_object_projected(&deleted, &r2, &r3, &r11);
	if (OBJECT(LINKED(current_object, OBJECT(current_object)->parent))->var_80 != 0u)
	{
		delete_if_off_screen(&deleted, &r2, &r3);
		if ((uint8_t)deleted == 1u)
			return;
	}
	add_to_touchable_list(deleted);
	add_to_rider_list();
	fall_to_floor(&ignored, &is_over_pit, &r2);
	if ((uint8_t)is_over_pit != 0u)
	{
		if (OBJECT(LINKED(WORK_RAM, OBJECT(current_object)->parent))->var_80 == 0u)
		{
			OBJECT(current_object)->speed_z = 0;
			crawler_step();
			return;
		}
		if (OBJECT(current_object)->crawler.on_platform != 0u)
		{
			depth = (uint16_t)(OBJECT(LINKED(WORK_RAM, OBJECT(current_object)->platform))->var_88 - 0xFu);
			queue_sprite_at_depth(&ignored, &depth);
			crawler_step();
			return;
		}
	}
	if (OBJECT(current_object)->crawler.variant == 0u || (uint8_t)OBJECT(current_object)->crawler.walk_step != 0xFFu)
		queue_sprite(is_over_pit);
	else
	{
		depth = (uint16_t)(OBJECT(current_object)->screen_y + 8u);
		queue_sprite_at_depth(&ignored, &depth);
	}
	current_object = LINKED(current_object, OBJECT(current_object)->child);
	place_sprite_at_cursor_floor();
	depth = (uint16_t)(OBJECT(current_object)->screen_y - 0xAu);
	queue_sprite_at_depth(&ignored, &depth);
	current_object = object_cursor;
	crawler_step();
}

void crawler_step(void)
{
	uint32_t direction = 0, speed = 0, speed_x = 0, speed_y = 0;
	uint16_t step_timer = 0;

	if (OBJECT(LINKED(WORK_RAM, OBJECT(current_object)->parent))->var_8C == 0u)
		return;
	step_timer = OBJECT(current_object)->crawler.step_timer;
	OBJECT(current_object)->crawler.step_timer = step_timer - 1u;
	if (step_timer == 0u)
	{
		OBJECT(current_object)->speed_x = 0;
		OBJECT(current_object)->speed_y = 0;
		if (OBJECT(current_object)->crawler.moving == 0u)
		{
			OBJECT(current_object)->crawler.moving = 1u;
			OBJECT(current_object)->crawler.step_timer = 4u;
		}
		else
		{
			OBJECT(current_object)->crawler.moving = 0u;
			OBJECT(current_object)->crawler.step_timer = 4u;
		}
	}
	if (OBJECT(current_object)->crawler.moving != 0u)
	{
		direction = OBJECT(current_object)->spawn_arg_2;
		speed = 0x200u;
		direction_to_speed(speed, &direction, &speed_x, &speed_y);
		OBJECT(current_object)->speed_x = speed_x;
		OBJECT(current_object)->speed_y = speed_y;
	}
}

void spawn_eggman_booth(uint32_t spawned, uint32_t r1, uint32_t booth, uint32_t *p_r2, uint32_t *p_r11)
{
	uint32_t r2 = *p_r2;
	uint32_t r11 = *p_r11;
	uint32_t object = 0;

	spawn_count = 2u;
	spawn_object_with_pieces(0x012B2Fu, &spawned, &r1, &r2, &booth, &r11);
	if ((uint8_t)spawned == 0u)
	{
		*p_r2 = r2;
		*p_r11 = r11;
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
	*p_r2 = r2;
	*p_r11 = r11;
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
	uint32_t r2 = 0, r3 = 0, ignored = 0;
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
	delete_object_and_children(&ignored, &r2, &r3);
}

void draw_eggman_booth(void)
{
	uint8_t bucket = 0, sound_id = 0;
	uint32_t r2 = 0, booth_frame = 0;

	OBJECT(current_object)->screen_y = OBJECT(object_cursor)->draw_eggman_booth.slide_y;
	booth_frame = eggman_booth_steps[OBJECT(current_object)->draw_eggman_booth.step];
	OBJECT(current_object)->sprite_frame = eggman_booth_frames[booth_frame];
	if (stage != 0u && stage != 5u && booth_frame == 0xBu)
	{
		sound_id = 0x41u;
		play_sound(sound_id, &booth_frame, &r2);
	}
	bucket = 0u;
	queue_sprite_in_bucket(bucket);
	current_object = LINKED(current_object, OBJECT(current_object)->child);
	OBJECT(current_object)->screen_y = OBJECT(object_cursor)->draw_eggman_booth.slide_y;
	bucket = 0u;
	queue_sprite_in_bucket(bucket);
}

void object_type_4_idle(void)
{
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
	uint32_t r2 = 0, r3 = 0, r11 = 0, deleted = 0, depth = 0, ignored = 0;

	unproject_object(&ignored, &r2, &r3, &r11);
	delete_if_off_screen(&deleted, &r2, &r3);
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
	uint32_t box_record = 0, first = 0, ignored = 0, object = 0, spawned = 0;

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
	add_pieces_to_current_object(&spawned, &ignored, &first, &object);
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
	uint32_t r2 = 0, r3 = 0, r11 = 0, bucket_2 = 0, deleted = 0, ignored = 0;
	uint16_t frame_timer = 0;

	unproject_object(&ignored, &r2, &r3, &r11);
	delete_if_off_screen(&deleted, &r2, &r3);
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
	uint32_t r1 = 0, r2 = 0, r3 = 0, r11 = 0, crumble_frame = 0, deleted = 0, depth = 0, ignored = 0, random_value = 0, ring = 0;

	unproject_object(&ignored, &r2, &r3, &r11);
	fall_to_floor(&ignored, &r1, &r2);
	delete_if_off_screen(&deleted, &r2, &r3);
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
		delete_object_and_children(&ignored, &r2, &r3);
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

void snow_mound_init(void)
{
	OBJECT(current_object)->projected_y_int += 0x20u;
	add_to_rider_list();
	OBJECT(current_object)->sprite_flags = 0u;
	OBJECT(current_object)->sprite_frame = 0x7B30u;
	OBJECT(current_object)->sprite_palette = 0x91B0u;
	OBJECT(current_object)->flags |= 2u;
	OBJECT(current_object)->hitbox_size_x = 0x22u;
	OBJECT(current_object)->hitbox_size_y = 0x21u;
	OBJECT(current_object)->hitbox_offset_x = -0x21;
	OBJECT(current_object)->hitbox_offset_y = 3;
	OBJECT(current_object)->hitbox_height = 0x20u;
	OBJECT(current_object)->kind = 5u;
	OBJECT(current_object)->snow_mound.part_3_frame = 4u;
	OBJECT(current_object)->handler = snow_mound_update_ADDRESS;
	current_object = LINKED(current_object, OBJECT(current_object)->child);
	OBJECT(current_object)->sprite_flags = 0u;
	OBJECT(current_object)->sprite_palette = 0x91B0u;
	current_object = LINKED(current_object, OBJECT(current_object)->child);
	OBJECT(current_object)->sprite_flags = 0u;
	OBJECT(current_object)->sprite_flags |= 0x40u;
	OBJECT(current_object)->sprite_palette = 0x91B0u;
	current_object = LINKED(current_object, OBJECT(current_object)->child);
	OBJECT(current_object)->sprite_flags = 0u;
	OBJECT(current_object)->sprite_flags |= 0x80u;
	OBJECT(current_object)->sprite_palette = 0x11C0u;
	OBJECT(current_object)->sprite_palette |= 0x2000u;
}

void snow_mound_update(void)
{
	uint8_t bucket = 0;
	uint16_t offset_x = 0, offset_y = 0;
	uint32_t r2 = 0, r3 = 0, r11 = 0, burst_frame = 0, deleted = 0, depth = 0, ignored = 0, object = 0, object_2 = 0, random_value = 0, ring = 0;

	move_object_projected(&ignored, &r2, &r3, &r11);
	fall_to_floor(&ignored, &depth, &r2);
	delete_if_off_screen(&deleted, &r2, &r3);
	if ((uint8_t)deleted == 1u)
		return;
	if (OBJECT(current_object)->snow_mound.bursting != 0u)
	{
	snow_mound_burst:
		OBJECT(current_object)->snow_mound.frame_count++;
		OBJECT(current_object)->snow_mound.part_3_count++;
		if (OBJECT(current_object)->snow_mound.frame_count <= 1)
		{
		snow_mound_draw:
			depth = (depth & ~0xFFFFu) | (uint16_t)OBJECT(current_object)->screen_y;
			depth = (depth & ~0xFFFFu) | (uint16_t)(depth + 0x12u);
			queue_sprite_at_depth(&ignored, &depth);
			if ((uint8_t)OBJECT(current_object)->snow_mound.burst_frame != 0u)
			{
				current_object = LINKED(current_object, OBJECT(object_cursor)->child);
				OBJECT(current_object)->screen_x = OBJECT(object_cursor)->screen_x;
				OBJECT(current_object)->screen_y = OBJECT(object_cursor)->screen_y;
				depth = (depth & ~0xFFFFu) | (uint16_t)OBJECT(current_object)->screen_y;
				depth = (depth & ~0xFFFFu) | (uint16_t)(depth + 0x11u);
				queue_sprite_at_depth(&ignored, &depth);
				current_object = LINKED(current_object, OBJECT(object_cursor)->child);
				current_object = LINKED(current_object, OBJECT(current_object)->child);
				OBJECT(current_object)->screen_x = OBJECT(object_cursor)->screen_x;
				OBJECT(current_object)->screen_y = OBJECT(object_cursor)->screen_y;
				OBJECT(current_object)->screen_y -= 0x16u;
				queue_sprite(depth);
			}
			else
			{
				add_to_touchable_list(ignored);
				add_to_rider_list();
			}
			current_object = LINKED(current_object, OBJECT(object_cursor)->child);
			object = current_object;
			current_object = LINKED(current_object, OBJECT(current_object)->child);
			current_object = LINKED(current_object, OBJECT(current_object)->child);
			OBJECT(current_object)->sprite_frame = OBJECT(object_cursor)->sprite_frame;
			if ((uint8_t)OBJECT(object_cursor)->snow_mound.burst_frame != 0u)
				OBJECT(current_object)->sprite_frame = OBJECT(object)->sprite_frame;
			offset_x = 0u;
			offset_y = 0x15u;
			place_sprite_near_cursor(offset_x, offset_y);
			bucket = 0xFFu;
			queue_sprite_in_bucket(bucket);
			return;
		}
		if (OBJECT(current_object)->snow_mound.burst_frame <= 0xE)
		{
			burst_frame = (uint8_t)OBJECT(current_object)->snow_mound.burst_frame;
			OBJECT(current_object)->sprite_frame = snow_mound_burst_frames[burst_frame];
			OBJECT(current_object)->snow_mound.frame_count = 0;
			OBJECT(current_object)->snow_mound.burst_frame++;
			burst_frame += 2u;
			if ((int8_t)burst_frame > 0xE)
				goto snow_mound_draw;
			object_2 = LINKED(WORK_RAM, OBJECT(current_object)->child);
			OBJECT(object_2)->sprite_frame = snow_mound_burst_frames[burst_frame];
			if (OBJECT(current_object)->snow_mound.part_3_count <= 2)
				goto snow_mound_draw;
			object_2 = LINKED(object_2, OBJECT(current_object)->child);
			object_2 = LINKED(object_2, OBJECT(object_2)->child);
			OBJECT(current_object)->snow_mound.part_3_count = 0;
			OBJECT(current_object)->snow_mound.part_3_frame++;
			OBJECT(object_2)->sprite_frame = snow_mound_burst_frames[OBJECT(current_object)->snow_mound.part_3_frame];
			goto snow_mound_draw;
		}
		OBJECT(current_object)->snow_mound.frame_count = 0;
		OBJECT(current_object)->snow_mound.part_3_count = 0;
		OBJECT(current_object)->snow_mound.burst_frame = 0;
		OBJECT(current_object)->snow_mound.bursting = 0u;
		OBJECT(current_object)->snow_mound.part_3_frame = 4u;
		OBJECT(current_object)->sprite_frame = snow_mound_burst_frames[(uint8_t)OBJECT(current_object)->snow_mound.burst_frame];
		delete_object_and_children(&ignored, &r2, &r3);
		return;
	}
	if ((OBJECT(current_object)->flags & 4u) == 0u)
		goto snow_mound_draw;
	OBJECT(current_object)->snow_mound.bursting = 1u;
	if (OBJECT(current_object)->snow_mound.ring_count == 0u)
		goto snow_mound_burst;
	released_ring_flip = 0u;
	rings_to_release = OBJECT(current_object)->snow_mound.ring_count;
	released_ring_angle = OBJECT(current_object)->spawn_arg_2;
	released_rings_random = OBJECT(current_object)->snow_mound.random_scatter;
	drop_rings(r3, &r11, &random_value, &ring);
	goto snow_mound_burst;
}

void rock_wall_init(void)
{
	OBJECT(current_object)->sprite_frame = 0x7C18u;
	OBJECT(current_object)->sprite_palette = 0x87C0u;
	OBJECT(current_object)->flags |= 2u;
	OBJECT(current_object)->hitbox_size_x = 0x18u;
	OBJECT(current_object)->hitbox_size_y = 0x4Du;
	OBJECT(current_object)->hitbox_offset_x = 9;
	OBJECT(current_object)->hitbox_offset_y = -0x39;
	OBJECT(current_object)->hitbox_height = 0xB4u;
	OBJECT(current_object)->kind = 0xCu;
	OBJECT(current_object)->rock_wall.frame_count = 2;
	OBJECT(current_object)->handler = rock_wall_update_ADDRESS;
	current_object = LINKED(current_object, OBJECT(current_object)->child);
	OBJECT(current_object)->sprite_palette = 0x8A10u;
	current_object = LINKED(current_object, OBJECT(current_object)->child);
	OBJECT(current_object)->sprite_palette = 0x8A10u;
}

void rock_wall_update(void)
{
	uint8_t bucket = 0;
	uint16_t offset_x = 0, offset_y = 0;
	uint32_t r2 = 0, r3 = 0, r11 = 0, deleted = 0, depth = 0, ignored = 0, random_value = 0, ring = 0;
	uint16_t twinkle_delay = 0, direction = 0, direction_2 = 0;

	unproject_object(&ignored, &r2, &r3, &r11);
	delete_if_off_screen(&deleted, &r2, &r3);
	if ((uint8_t)deleted == 1u)
		return;
	if (OBJECT(current_object)->rock_wall.breaking == 0u)
	{
		if ((OBJECT(current_object)->flags & 4u) == 0u || (OBJECT(LINKED(WORK_RAM, OBJECT(current_object)->touched_by))->flags & 0x40u) == 0u)
			goto rock_wall_keep_touchable;
		OBJECT(current_object)->rock_wall.breaking = 1u;
	}
	if (OBJECT(current_object)->rock_wall.break_frame < 0xD)
	{
		OBJECT(current_object)->rock_wall.frame_count++;
		if (OBJECT(current_object)->rock_wall.frame_count > 2)
		{
			OBJECT(current_object)->rock_wall.break_frame++;
			OBJECT(current_object)->sprite_frame = rock_wall_break_frames[(uint8_t)OBJECT(current_object)->rock_wall.break_frame];
			OBJECT(current_object)->rock_wall.frame_count = 0;
			if ((uint8_t)OBJECT(current_object)->rock_wall.break_frame == 5u && OBJECT(current_object)->spawn_arg_1 != 0u)
			{
				rings_to_release = OBJECT(current_object)->spawn_arg_1;
				released_ring_angle = OBJECT(current_object)->spawn_arg_2;
				released_ring_flip = 0u;
				released_rings_random = 0u;
				drop_rings(r3, &r11, &random_value, &ring);
			}
		}
	rock_wall_keep_touchable:
		add_to_touchable_list(deleted);
	}
	bucket = 0xFFu;
	queue_sprite_in_bucket(bucket);
	if (OBJECT(current_object)->rock_wall.break_frame < 1)
	{
		twinkle_delay = OBJECT(current_object)->rock_wall.twinkle_delay;
		OBJECT(current_object)->rock_wall.twinkle_delay = twinkle_delay - 1u;
		if (twinkle_delay != 0u)
			return;
		OBJECT(current_object)->rock_wall.twinkle_delay = 0u;
		current_object = LINKED(current_object, OBJECT(current_object)->child);
		direction = OBJECT(current_object)->direction;
		OBJECT(current_object)->direction = direction - 1u;
		if (direction == 0u)
		{
			OBJECT(current_object)->direction = 2u;
			OBJECT(current_object)->sprite_frame = rock_wall_sparkle_frames[AT8(OBJECT(current_object)->projected_y, 1)];
			AT8(OBJECT(current_object)->projected_y, 1)++;
			if ((int8_t)AT8(OBJECT(current_object)->projected_y, 1) > 0x19)
				AT8(OBJECT(current_object)->projected_y, 1) = 0u;
		}
		offset_x = 8u;
		offset_y = 0xFFEBu;
		place_sprite_near_cursor(offset_x, offset_y);
		depth = (uint16_t)(OBJECT(object_cursor)->screen_y - 1u);
		queue_sprite_at_depth(&ignored, &depth);
		current_object = LINKED(current_object, OBJECT(current_object)->child);
		direction_2 = OBJECT(current_object)->direction;
		OBJECT(current_object)->direction = direction_2 - 1u;
		if (direction_2 == 0u)
		{
			OBJECT(current_object)->direction = 4u;
			OBJECT(current_object)->sprite_frame = rock_wall_glint_frames[rock_wall_glint_steps[AT8(OBJECT(current_object)->projected_y, 1)]];
			AT8(OBJECT(current_object)->projected_y, 1)++;
			if ((int8_t)AT8(OBJECT(current_object)->projected_y, 1) > 0x13)
			{
				AT8(OBJECT(current_object)->projected_y, 1) = 0u;
				OBJECT(object_cursor)->rock_wall.twinkle_delay = 0x1Eu;
			}
		}
		offset_x = 5u;
		offset_y = 0xFFD1u;
		place_sprite_near_cursor(offset_x, offset_y);
		depth = (uint16_t)(OBJECT(object_cursor)->screen_y - 1u);
		queue_sprite_at_depth(&ignored, &depth);
	}
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
	uint32_t r2 = 0, r3 = 0, r11 = 0, depth = 0, frame_index = 0, ignored = 0;
	uint16_t step_timer = 0, odd_frame = 0, flicker_step = 0;

	unproject_object(&ignored, &r2, &r3, &r11);
	step_timer = OBJECT(current_object)->eggman_teleport.step_timer;
	OBJECT(current_object)->eggman_teleport.step_timer = step_timer - 1u;
	if (step_timer == 0u)
	{
		frame_index = eggman_teleport_steps[OBJECT(current_object)->eggman_teleport.step];
		OBJECT(current_object)->eggman_teleport.frame_index = frame_index;
		if (frame_index == 0xFFu)
		{
			delete_object_and_children(&ignored, &r2, &r3);
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

void sand_arch_init(void)
{
	uint16_t ball_phase = 0, phase_spacing = 0;
	uint32_t balls_left = 0, block = 0, free_count = 0, object = 0, object_2 = 0, piece_count = 0, spawned = 0;

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
		spawn_object_with_pieces(0x01369Cu, &spawned, &piece_count, &free_count, &object_2, &block);
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
	uint32_t r2 = 0, r3 = 0, r11 = 0, bucket = 0, ignored = 0, on_screen = 0;

	unproject_object(&ignored, &r2, &r3, &r11);
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
	delete_object_and_children(&ignored, &r2, &r3);
}

void sand_arch_ball_init(void)
{
	uint32_t r11 = 0, ball_direction = 0, ignored = 0, speed = 0, speed_x = 0, speed_y = 0;

	ball_direction = OBJECT(current_object)->direction;
	speed = 0x200u * OBJECT(current_object)->sand_arch.row_position;
	direction_to_speed(speed, &ball_direction, &speed_x, &speed_y);
	OBJECT(current_object)->speed_x = speed_x;
	OBJECT(current_object)->speed_y = speed_y;
	move_object_projected(&ignored, &ignored, &ignored, &r11);
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
	uint32_t r1 = 0, r2 = 0, r11 = 0, depth = 0, ignored = 0;
	uint16_t direction = 0;

	move_object_projected(&ignored, &r2, &ignored, &r11);
	add_to_touchable_list(ignored);
	add_to_rider_list();
	if (OBJECT(current_object)->floor_z_int >= 0x800)
	{
		if (OBJECT(current_object)->z_int >= 0x800)
		{
			current_object = LINKED(current_object, OBJECT(current_object)->parent);
			OBJECT(current_object)->var_80++;
			current_object = object_cursor;
			OBJECT(current_object)->handler = sand_arch_ball_gone_ADDRESS;
		}
		fall_to_floor(&ignored, &r1, &r2);
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
		r1 = queue_sprite_at_floor();
	}
	if ((int16_t)(OBJECT(current_object)->floor_z_int - OBJECT(current_object)->z_int) <= 0x10)
	{
		current_object = LINKED(current_object, OBJECT(current_object)->child);
		direction = OBJECT(current_object)->direction;
		OBJECT(current_object)->direction = direction - 1u;
		if (direction == 0u)
		{
			OBJECT(current_object)->direction = 2u;
			OBJECT(current_object)->sprite_frame = sand_arch_spray_frames[AT8(OBJECT(current_object)->projected_y, 1)];
			OBJECT(current_object)->sprite_palette = 0x9020u;
			AT8(OBJECT(current_object)->projected_y, 1)++;
			AT8(OBJECT(current_object)->projected_y, 1) = AT8(OBJECT(current_object)->projected_y, 1) % 4u;
		}
		offset_x = 0u;
		offset_y = 8u;
		place_sprite_near_cursor(offset_x, offset_y);
		OBJECT(current_object)->screen_y -= OBJECT(object_cursor)->z_int - OBJECT(object_cursor)->floor_z_int;
		queue_sprite(r1);
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
	AT8(OBJECT(current_object)->projected_y, 1) = 0u;
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

void fence_post_init(void)
{
	uint16_t arc_phase = 0;
	uint32_t block = 0, free_count = 0, object = 0, object_2 = 0, piece_count = 0, rom_record = 0, row_direction = 0;
	uint32_t posts_left = 0;

	electric_fence_state = 1u;
	r14 = OBJECT(current_object)->fence_post.extra_posts;
	row_direction = OBJECT(current_object)->fence_post.row_direction;
	arc_phase = OBJECT(current_object)->fence_post.arc_phase;
	for (;;)
	{
		OBJECT(current_object)->sprite_frame = 0x6CA8u;
		OBJECT(current_object)->sprite_palette = 0x9060u;
		OBJECT(current_object)->flags |= 2u;
		OBJECT(current_object)->hitbox_height = 0x64u;
		OBJECT(current_object)->kind = 0xEu;
		rom_record = fence_post_boxes_ADDRESS + row_direction * 4u;
		OBJECT(current_object)->hitbox_size_x = ROM16(rom_record);
		OBJECT(current_object)->hitbox_size_y = ROM16(rom_record + 2u);
		rom_record = fence_post_box_offsets_ADDRESS + row_direction * 2u;
		OBJECT(current_object)->hitbox_offset_x = ROM8(rom_record);
		OBJECT(current_object)->hitbox_offset_y = ROM8(rom_record + 1u);
		OBJECT(current_object)->handler = fence_post_start_ADDRESS;
		object = LINKED(WORK_RAM, OBJECT(current_object)->child);
		OBJECT(object)->direction = arc_phase;
		OBJECT(object)->sprite_frame = 0x6C88u;
		OBJECT(object)->sprite_palette = 0x9030u;
		OBJECT(object)->sprite_flags = fence_arc_flags[row_direction];
		posts_left = r14;
		r14 = posts_left - 1u;
		if (posts_left == 0u)
			break;
		current_player = current_object;
		spawn_count = 2u;
		spawn_object_with_pieces(0x013956u, &object, &piece_count, &free_count, &object_2, &block);
		if ((uint8_t)object == 0u)
			break;
		OBJECT(object_2)->screen_x = OBJECT(current_player)->screen_x;
		OBJECT(object_2)->screen_y = OBJECT(current_player)->screen_y;
		OBJECT(object_2)->projected_x = OBJECT(current_player)->projected_x;
		OBJECT(object_2)->projected_y = OBJECT(current_player)->projected_y;
		OBJECT(object_2)->z = OBJECT(current_player)->z;
		OBJECT(object_2)->floor_z = OBJECT(current_player)->floor_z;
		OBJECT(object_2)->spawn_arg_2 = OBJECT(current_player)->spawn_arg_2;
		rom_record = fence_post_offsets_ADDRESS + row_direction * 4u;
		OBJECT(object_2)->projected_x += ROM16(rom_record);
		OBJECT(object_2)->projected_y_int += ROM16(rom_record + 2u);
		current_object = object_2;
	}
}

void fence_post_start(void)
{
	uint32_t r11 = 0, ignored = 0;

	add_to_rider_list();
	unproject_object(&ignored, &ignored, &ignored, &r11);
	OBJECT(current_object)->handler = fence_post_update_ADDRESS;
}

void fence_post_update(void)
{
	uint16_t offset_x = 0, offset_y = 0;
	uint32_t r2 = 0, r3 = 0, arc_offset_record = 0, depth = 0, ignored = 0;
	uint16_t odd_frame = 0;

	if (electric_fence_state != 1u)
	{
		if (electric_fence_state == 0xFFu)
		{
			delete_object_and_children(&ignored, &r2, &r3);
			return;
		}
		OBJECT(current_object)->kind = 5u;
		OBJECT(current_object)->hitbox_size_x = 0x1Bu;
		OBJECT(current_object)->hitbox_size_y = 0x19u;
		OBJECT(current_object)->hitbox_offset_x = -9;
		OBJECT(current_object)->hitbox_offset_y = -0x13;
		OBJECT(current_object)->handler = fence_post_draw_ADDRESS;
	}
	current_object = LINKED(current_object, OBJECT(current_object)->child);
	arc_offset_record = fence_arc_offsets_ADDRESS + OBJECT(object_cursor)->fence_post.row_direction * 4u;
	offset_x = ROM16(arc_offset_record);
	offset_y = ROM16(arc_offset_record + 2u);
	place_sprite_near_cursor(offset_x, offset_y);
	AS16(OBJECT(current_object)->handler)++;
	odd_frame = AS16(OBJECT(current_object)->handler) & 1u;
	AS16(OBJECT(current_object)->handler) = odd_frame;
	if (odd_frame == 0u)
	{
		OBJECT(current_object)->direction++;
		OBJECT(current_object)->direction &= 3u;
		OBJECT(current_object)->sprite_frame = fence_arc_frames[OBJECT(current_object)->direction];
	}
	depth = (uint16_t)(OBJECT(current_object)->screen_y + 8u);
	queue_sprite_at_depth(&ignored, &depth);
	current_object = object_cursor;
	fence_post_draw();
}

void fence_post_draw(void)
{
	uint32_t r1 = 0, r2 = 0, r3 = 0, r11 = 0, deleted = 0, ignored = 0;

	unproject_object(&ignored, &r2, &r3, &r11);
	fall_to_floor(&ignored, &r1, &r2);
	delete_if_off_screen(&deleted, &r2, &r3);
	if ((uint8_t)deleted != 1u)
	{
		queue_sprite(r1);
		OBJECT(current_object)->health = 0x7F;
		add_to_touchable_list(deleted);
		add_to_rider_list();
	}
}

void bubble_column_init(void)
{
	uint8_t sound_id = 0;
	uint32_t r1 = 0, r2 = 0;

	OBJECT(current_object)->sprite_palette = 0x1060u;
	OBJECT(current_object)->handler = bubble_column_animate_ADDRESS;
	sound_id = 0x24u;
	play_sound(sound_id, &r1, &r2);
}

void bubble_column_animate(void)
{
	uint32_t r11 = 0, frame_base = 0, frame_index = 0, ignored = 0;
	uint16_t frame_timer = 0;

	unproject_object(&ignored, &ignored, &ignored, &r11);
	frame_timer = OBJECT(current_object)->bubble_column.frame_timer;
	OBJECT(current_object)->bubble_column.frame_timer = frame_timer - 1u;
	if (frame_timer == 0u)
	{
		OBJECT(current_object)->bubble_column.frame_timer = 2u;
		frame_index = (uint8_t)OBJECT(current_object)->bubble_column.frame_index;
		frame_base = OBJECT(current_object)->spawn_arg_1;
		frame_base = (uint16_t)(frame_base * 0x13u);
		OBJECT(current_object)->sprite_frame = mine_bubble_frames[(uint16_t)(frame_index + frame_base)];
		OBJECT(current_object)->bubble_column.frame_index++;
		if (OBJECT(current_object)->bubble_column.frame_index > 0x13)
			OBJECT(current_object)->bubble_column.frame_index = 0;
	}
	queue_sprite(frame_base);
}

void mine_init(void)
{
	if (OBJECT(current_object)->spawn_arg_2 != 0u)
		OBJECT(current_object)->sprite_scale = 0x80u;
	OBJECT(current_object)->sprite_frame = 0x8610u;
	OBJECT(current_object)->hitbox_size_x = 8u;
	OBJECT(current_object)->hitbox_height = 0x1Eu;
	OBJECT(current_object)->kind = 0x24u;
	OBJECT(current_object)->handler = mine_float_ADDRESS;
	current_object = LINKED(current_object, OBJECT(current_object)->child);
	OBJECT(current_object)->sprite_scale += 0x24u;
	OBJECT(current_object)->sprite_scale = OBJECT(object_cursor)->sprite_scale;
	OBJECT(current_object)->sprite_frame = 0x5D90u;
	OBJECT(current_object)->sprite_palette = 0x7A0u;
	OBJECT(current_object)->sprite_palette |= 0x4000u;
	current_object = LINKED(current_object, OBJECT(current_object)->child);
	OBJECT(current_object)->sprite_palette = 0x1060u;
	OBJECT(current_object)->sprite_palette |= 0x4000u;
	current_object = LINKED(current_object, OBJECT(current_object)->child);
	AT8(OBJECT(current_object)->projected_y, 1) = 1u;
	OBJECT(current_object)->sprite_palette = 0x1060u;
	OBJECT(current_object)->sprite_palette |= 0x4000u;
	current_object = LINKED(current_object, OBJECT(current_object)->child);
	AT8(OBJECT(current_object)->projected_y, 1) = 2u;
	OBJECT(current_object)->sprite_palette = 0x1060u;
	OBJECT(current_object)->sprite_palette |= 0x4000u;
	OBJECT(current_object)->sprite_flags |= 0x40u;
}

void mine_float(void)
{
	face_nearest_player(0u, 0u);
	if (nearest_player_distance < 0x80)
		OBJECT(current_object)->handler = mine_fall_ADDRESS;
	OBJECT(current_object)->speed_z = 0;
	OBJECT(current_object)->mine.timer += 0x10u;
	OBJECT(current_object)->mine.timer &= 0x3FFu;
	OBJECT(current_object)->speed_z += sine_table[OBJECT(current_object)->mine.timer];
	mine_fall();
}

void mine_fall(void)
{
	uint8_t sinking = 0, sound_id = 0;
	uint32_t r0 = 0, r1 = 0, r2 = 0, r3 = 0, r11 = 0, bucket = 0, deleted = 0, ignored = 0;
	uint16_t odd_frame = 0;

	move_object_projected(&ignored, &r2, &r3, &r11);
	delete_if_off_screen(&deleted, &r2, &r3);
	if ((uint8_t)deleted == 1u)
		return;
	fall_to_floor(&r0, &r1, &r2);
	if (OBJECT(current_object)->z_int < (int16_t)OBJECT(current_object)->spawn_arg_1)
	{
		sinking = 0u;
		flash_mine_palette(sinking);
		queue_sprite_at_floor();
		add_to_touchable_list(r0);
		current_object = LINKED(current_object, OBJECT(current_object)->child);
		OBJECT(current_object)->direction++;
		odd_frame = OBJECT(current_object)->direction & 1u;
		OBJECT(current_object)->direction = odd_frame;
		if (odd_frame == 0u)
		{
			bucket = place_sprite_at_cursor_floor();
			queue_sprite_in_bucket(bucket);
		}
		return;
	}
	spawn_mine_splash(r0, r2, r3, r11);
	OBJECT(current_object)->mine.timer = 0u;
	OBJECT(current_object)->speed_z = 0;
	OBJECT(current_object)->z_int += 4u;
	OBJECT(current_object)->handler = mine_sink_ADDRESS;
	sound_id = 0x16u;
	play_sound(sound_id, &r1, &r2);
	animate_sinking_mine();
}

void mine_sink(void)
{
	uint8_t sound_id = 0;
	uint32_t r1 = 0, r2 = 0, r11 = 0, height = 0, ignored = 0;

	move_object_projected(&ignored, &r2, &ignored, &r11);
	OBJECT(current_object)->speed_z -= 0x5800u;
	fall_to_floor(&height, &r1, &r2);
	if (height != 0u)
	{
		animate_sinking_mine();
		return;
	}
	sound_id = 0x20u;
	play_sound(sound_id, &r1, &r2);
	OBJECT(current_object)->mine.timer = 0u;
	OBJECT(current_object)->mine.frame_index = 0;
	OBJECT(current_object)->sprite_palette = 0x1070u;
	OBJECT(current_object)->handler = mine_explode_ADDRESS;
	current_object = LINKED(current_object, OBJECT(current_object)->child);
	AS8(OBJECT(current_object)->projected_y_int) = 1u;
	current_object = object_cursor;
	mine_explode();
}

void mine_explode(void)
{
	uint8_t sound = 0;
	uint32_t r1 = 0, r2 = 0, r3 = 0, r11 = 0, frame_index = 0, ignored = 0;
	uint8_t next_frame = 0;
	uint16_t timer = 0;

	unproject_object(&ignored, &r2, &r3, &r11);
	timer = OBJECT(current_object)->mine.timer;
	OBJECT(current_object)->mine.timer = timer - 1u;
	if (timer != 0u)
	{
		draw_mine_with_bubbles();
		return;
	}
	frame_index = (uint8_t)OBJECT(current_object)->mine.frame_index;
	OBJECT(current_object)->sprite_frame = underwater_explosion_frames[frame_index];
	OBJECT(current_object)->mine.timer = underwater_explosion_durations[frame_index];
	OBJECT(current_object)->mine.frame_index++;
	next_frame = (uint8_t)OBJECT(current_object)->mine.frame_index % 0x17u;
	OBJECT(current_object)->mine.frame_index = next_frame;
	if (next_frame == 0u)
	{
		sound = 0x24u;
		play_sound(sound, &r1, &r2);
		delete_object_and_children(&ignored, &r2, &r3);
		return;
	}
	if (OBJECT(current_object)->mine.frame_index <= 0xF)
	{
		draw_mine_with_bubbles();
		return;
	}
	draw_mine_with_bubbles();
}

void animate_sinking_mine(void)
{
	uint8_t sinking = 0;

	sinking = 1u;
	flash_mine_palette(sinking);
	OBJECT(current_object)->sprite_frame = mine_frames[mine_sink_steps[OBJECT(current_object)->animate_sinking_mine.sink_step]];
	OBJECT(current_object)->animate_sinking_mine.sink_step++;
	OBJECT(current_object)->animate_sinking_mine.sink_step = OBJECT(current_object)->animate_sinking_mine.sink_step % 8u;
	draw_mine_with_bubbles();
}

void draw_mine_with_bubbles(void)
{
	uint16_t offset_x = 0, offset_y = 0;
	uint32_t bucket = 0, depth = 0, ignored = 0;
	uint16_t shadow_odd_frame = 0, bubble_1_odd_frame = 0, bubble_2_odd_frame = 0, bubble_3_odd_frame = 0;

	add_to_touchable_list(0u);
	queue_sprite_at_floor();
	current_object = LINKED(current_object, OBJECT(current_object)->child);
	if (AS8(OBJECT(current_object)->projected_y_int) == 0u)
	{
		OBJECT(current_object)->direction++;
		shadow_odd_frame = OBJECT(current_object)->direction & 1u;
		OBJECT(current_object)->direction = shadow_odd_frame;
		if (shadow_odd_frame == 0u)
		{
			bucket = place_sprite_at_cursor_floor();
			r14 = (uint16_t)OBJECT(current_object)->screen_y;
			queue_sprite_in_bucket(bucket);
		}
	}
	current_object = LINKED(current_object, OBJECT(current_object)->child);
	offset_x = 0u;
	offset_y = 0xFFF6u;
	place_sprite_near_cursor(offset_x, offset_y);
	OBJECT(current_object)->direction++;
	bubble_1_odd_frame = OBJECT(current_object)->direction & 1u;
	OBJECT(current_object)->direction = bubble_1_odd_frame;
	if (bubble_1_odd_frame != 0u)
	{
		OBJECT(current_object)->sprite_frame = mine_bubble_frames[AT8(OBJECT(current_object)->projected_y, 1)];
		AT8(OBJECT(current_object)->projected_y, 1)++;
		if ((int8_t)AT8(OBJECT(current_object)->projected_y, 1) > 0x13)
			AT8(OBJECT(current_object)->projected_y, 1) = 0u;
	}
	r14 = (uint16_t)(r14 + 1u);
	depth = r14;
	queue_sprite_at_depth(&ignored, &depth);
	current_object = LINKED(current_object, OBJECT(current_object)->child);
	offset_x = 5u;
	offset_y = 0xFFECu;
	place_sprite_near_cursor(offset_x, offset_y);
	OBJECT(current_object)->direction++;
	bubble_2_odd_frame = OBJECT(current_object)->direction & 1u;
	OBJECT(current_object)->direction = bubble_2_odd_frame;
	if (bubble_2_odd_frame != 0u)
	{
		OBJECT(current_object)->sprite_frame = mine_bubble_frames_2[AT8(OBJECT(current_object)->projected_y, 1)];
		AT8(OBJECT(current_object)->projected_y, 1)++;
		if ((int8_t)AT8(OBJECT(current_object)->projected_y, 1) > 0x13)
			AT8(OBJECT(current_object)->projected_y, 1) = 0u;
	}
	r14 = (uint16_t)(r14 - 2u);
	depth = r14;
	queue_sprite_at_depth(&ignored, &depth);
	current_object = LINKED(current_object, OBJECT(current_object)->child);
	offset_x = 0xAu;
	offset_y = 0xFFECu;
	place_sprite_near_cursor(offset_x, offset_y);
	OBJECT(current_object)->direction++;
	bubble_3_odd_frame = OBJECT(current_object)->direction & 1u;
	OBJECT(current_object)->direction = bubble_3_odd_frame;
	if (bubble_3_odd_frame != 0u)
	{
		OBJECT(current_object)->sprite_frame = mine_bubble_frames[AT8(OBJECT(current_object)->projected_y, 1)];
		AT8(OBJECT(current_object)->projected_y, 1)++;
		if ((int8_t)AT8(OBJECT(current_object)->projected_y, 1) > 0x13)
			AT8(OBJECT(current_object)->projected_y, 1) = 0u;
	}
	depth = (uint16_t)r14;
	queue_sprite_at_depth(&ignored, &depth);
}

void flash_mine_palette(uint8_t sinking)
{
	uint16_t flash_timer = 0;

	flash_timer = OBJECT(current_object)->flash_mine_palette.flash_timer;
	OBJECT(current_object)->flash_mine_palette.flash_timer = flash_timer - 1u;
	if (flash_timer != 0u)
		return;
	OBJECT(current_object)->flash_mine_palette.flash_timer = 4u;
	OBJECT(current_object)->sprite_palette = mine_flash_palettes[mine_flash_steps[OBJECT(current_object)->flash_mine_palette.flash_step]];
	if ((uint8_t)sinking != 0u)
	{
		OBJECT(current_object)->sprite_palette ^= 0x8000u;
		OBJECT(current_object)->sprite_palette |= 0x4000u;
	}
	OBJECT(current_object)->flash_mine_palette.flash_step++;
	OBJECT(current_object)->flash_mine_palette.flash_step = OBJECT(current_object)->flash_mine_palette.flash_step % 0xCu;
}

void spawn_mine_splash(uint32_t spawned, uint32_t r2, uint32_t splash, uint32_t r11)
{
	uint32_t object = 0, piece_count = 0;

	OBJECT(current_object)->sprite_palette = 0x1070u;
	spawn_count = 2u;
	spawn_object_with_pieces(0x013F45u, &spawned, &piece_count, &r2, &splash, &r11);
	if ((uint8_t)spawned != 0u)
	{
		copy_position_to(splash);
		OBJECT(splash)->projected_y_int++;
		OBJECT(splash)->sprite_palette = 0x90A0u;
		OBJECT(splash)->handler = mine_splash_animate_ADDRESS;
		object = LINKED(splash, OBJECT(splash)->child);
		OBJECT(object)->sprite_frame = 0x8570u;
		OBJECT(object)->sprite_palette = 0x1080u;
		OBJECT(object)->sprite_palette |= 0x4000u;
	}
}

void mine_splash_animate(void)
{
	uint32_t r2 = 0, r3 = 0, r11 = 0, depth = 0, ignored = 0;
	uint16_t frame_timer = 0, direction = 0;

	unproject_object(&ignored, &r2, &r3, &r11);
	if (OBJECT(current_object)->mine_splash.frame_index > 0x19)
	{
		delete_object_and_children(&ignored, &r2, &r3);
		return;
	}
	frame_timer = OBJECT(current_object)->mine_splash.frame_timer;
	OBJECT(current_object)->mine_splash.frame_timer = frame_timer - 1u;
	if (frame_timer == 0u)
	{
		OBJECT(current_object)->mine_splash.frame_timer = 2u;
		OBJECT(current_object)->sprite_frame = mine_splash_frames[(uint8_t)OBJECT(current_object)->mine_splash.frame_index];
		OBJECT(current_object)->mine_splash.frame_index++;
	}
	queue_sprite_at_floor();
	current_object = LINKED(current_object, OBJECT(object_cursor)->child);
	if ((int8_t)AT8(OBJECT(current_object)->projected_y, 1) >= 0x14)
		return;
	direction = OBJECT(current_object)->direction;
	OBJECT(current_object)->direction = direction - 1u;
	if (direction == 0u)
	{
		OBJECT(current_object)->direction = 2u;
		OBJECT(current_object)->sprite_frame = mine_splash_spray_frames[AT8(OBJECT(current_object)->projected_y, 1)];
		AT8(OBJECT(current_object)->projected_y, 1)++;
	}
	OBJECT(current_object)->screen_x = OBJECT(object_cursor)->screen_x;
	OBJECT(current_object)->screen_y = OBJECT(object_cursor)->screen_y;
	OBJECT(current_object)->screen_y += 7u;
	depth = (uint16_t)(r14 + 2u);
	queue_sprite_at_depth(&ignored, &depth);
}

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
	uint32_t r1 = 0, r2 = 0, direction = 0;
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
		sound_id = (uint8_t)ROM16(eggman_scene_sounds_ADDRESS + OBJECT(current_object)->direction * 2u);
		play_sound(sound_id, &r1, &r2);
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
	uint32_t r1 = 0, r2 = 0, r3 = 0, r11 = 0, bucket_2 = 0, deleted = 0, ignored = 0;
	uint16_t direction = 0;

	move_object_projected(&ignored, &r2, &r3, &r11);
	fall_to_floor(&ignored, &r1, &r2);
	bucket = 0u;
	queue_sprite_in_bucket(bucket);
	delete_if_off_screen(&deleted, &r2, &r3);
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
	uint32_t r2 = 0, r3 = 0, r11 = 0, ignored = 0;

	unproject_object(&ignored, &r2, &r3, &r11);
	delete_if_off_screen(&ignored, &r2, &r3);
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
	uint32_t r2 = 0, r3 = 0, r11 = 0, deleted = 0, depth = 0, ignored = 0;

	move_object_projected(&ignored, &r2, &r3, &r11);
	delete_if_off_screen(&deleted, &r2, &r3);
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

void lid_trap_init(void)
{
	OBJECT(current_object)->sprite_frame = 0x8840u;
	OBJECT(current_object)->sprite_palette = 0x10F0u;
	OBJECT(current_object)->sprite_palette |= 0x4000u;
	OBJECT(current_object)->flags |= 2u;
	OBJECT(current_object)->hitbox_size_x = 0x26u;
	OBJECT(current_object)->hitbox_size_y = 0x26u;
	OBJECT(current_object)->hitbox_offset_x = 0x1A;
	OBJECT(current_object)->hitbox_offset_y = -0x44;
	OBJECT(current_object)->hitbox_height = 0u;
	OBJECT(current_object)->kind = 0x23u;
	OBJECT(current_object)->handler = lid_trap_update_ADDRESS;
	current_object = LINKED(current_object, OBJECT(current_object)->child);
	OBJECT(current_object)->sprite_frame = 0x8850u;
	OBJECT(current_object)->sprite_palette = 0x1100u;
	OBJECT(current_object)->sprite_palette |= 0x4000u;
}

void lid_trap_update(void)
{
	uint8_t sound_id = 0;
	uint16_t offset_x = 0, offset_y = 0;
	uint32_t r1 = 0, r2 = 0, r3 = 0, r11 = 0, deleted = 0, depth = 0, ignored = 0;
	uint16_t direction = 0;

	unproject_object(&ignored, &r2, &r3, &r11);
	delete_if_off_screen(&deleted, &r2, &r3);
	if ((uint8_t)deleted != 0u)
		return;
	if ((OBJECT(current_object)->flags & 4u) != 0u)
	{
		OBJECT(current_object)->kind = 5u;
		OBJECT(current_object)->sprite_frame = 0x8848u;
		OBJECT(current_object)->lid_trap.sprung = 1u;
		current_object = LINKED(current_object, OBJECT(current_object)->child);
		OBJECT(current_object)->sprite_palette = 0x1110u;
		sound_id = 0x11u;
		play_sound(sound_id, &r1, &r2);
		current_object = object_cursor;
	}
	depth = (uint16_t)(OBJECT(current_object)->screen_y - 0x58u);
	queue_sprite_at_depth(&ignored, &depth);
	add_to_touchable_list(ignored);
	current_object = LINKED(current_object, OBJECT(current_object)->child);
	if (OBJECT(object_cursor)->lid_trap.sprung != 0u && (int16_t)AT16(OBJECT(current_object)->projected_y, 1) < 4)
	{
		direction = OBJECT(current_object)->direction;
		OBJECT(current_object)->direction = direction - 1u;
		if (direction == 0u)
		{
			OBJECT(current_object)->direction = 3u;
			OBJECT(current_object)->sprite_frame = lid_trap_lid_frames[lid_trap_close_steps[AT8(OBJECT(current_object)->projected_y, 1)]];
			AT8(OBJECT(current_object)->projected_y, 1)++;
		}
	}
	offset_x = 0u;
	offset_y = 0xFFF0u;
	place_sprite_near_cursor(offset_x, offset_y);
	depth = (uint16_t)(OBJECT(object_cursor)->screen_y - 0x20u);
	queue_sprite_at_depth(&ignored, &depth);
}

void turtle_init(void)
{
	OBJECT(current_object)->sprite_frame = 0x7F28u;
	OBJECT(current_object)->sprite_palette = 0x1040u;
	OBJECT(current_object)->sprite_palette |= 0x4000u;
	OBJECT(current_object)->flags |= 2u;
	OBJECT(current_object)->hitbox_size_x = 0x26u;
	OBJECT(current_object)->hitbox_size_y = 0x26u;
	OBJECT(current_object)->hitbox_offset_x = 0x1A;
	OBJECT(current_object)->hitbox_offset_y = -0x44;
	OBJECT(current_object)->hitbox_height = 0u;
	OBJECT(current_object)->kind = 0xCu;
	OBJECT(current_object)->handler = turtle_wait_ADDRESS;
	current_object = LINKED(current_object, OBJECT(current_object)->child);
	OBJECT(current_object)->sprite_frame = 0x7F70u;
	OBJECT(current_object)->sprite_palette = 0x1050u;
	OBJECT(current_object)->sprite_palette |= 0x4000u;
	current_object = LINKED(current_object, OBJECT(current_object)->child);
	OBJECT(current_object)->sprite_frame = 0x5D88u;
	OBJECT(current_object)->sprite_palette = 0x7A0u;
	OBJECT(current_object)->sprite_palette |= 0x4000u;
}

void turtle_wait(void)
{
	uint8_t bucket = 0;
	uint16_t offset_x = 0, offset_y = 0;
	uint32_t r11 = 0, depth = 0, ignored = 0;
	uint8_t head_step = 0;
	uint16_t direction = 0;

	if ((OBJECT(current_object)->flags & 4u) != 0u)
	{
		OBJECT(current_object)->turtle.stroke_timer = 0u;
		OBJECT(current_object)->turtle.stroke_step = 1u;
		OBJECT(current_object)->handler = turtle_surface_ADDRESS;
		turtle_surface();
		return;
	}
	unproject_object(&ignored, &ignored, &ignored, &r11);
	depth = (uint16_t)(OBJECT(current_object)->screen_y - 0x20u);
	queue_sprite_at_depth(&ignored, &depth);
	add_to_touchable_list(ignored);
	current_object = LINKED(current_object, OBJECT(current_object)->child);
	if (OBJECT(object_cursor)->turtle.stroke_step != 4u)
		return;
	direction = OBJECT(current_object)->direction;
	OBJECT(current_object)->direction = direction - 1u;
	if (direction != 0u)
	{
		offset_x = 0xBu;
		offset_y = 0xFFEDu;
		place_sprite_near_cursor(offset_x, offset_y);
		depth = (uint16_t)(OBJECT(object_cursor)->screen_y - 0x1Fu);
		queue_sprite_at_depth(&ignored, &depth);
	}
	else
	{
		OBJECT(current_object)->direction = 4u;
		OBJECT(current_object)->sprite_frame = turtle_head_frames[turtle_head_steps[AT16(OBJECT(current_object)->projected_y, 1)]];
		AT8(OBJECT(current_object)->projected_y, 1)++;
		head_step = AT8(OBJECT(current_object)->projected_y, 1) & 0xFu;
		AT8(OBJECT(current_object)->projected_y, 1) = head_step;
		if (head_step != 0u)
		{
			offset_x = 0xBu;
			offset_y = 0xFFEDu;
			place_sprite_near_cursor(offset_x, offset_y);
			depth = (uint16_t)(OBJECT(object_cursor)->screen_y - 0x1Fu);
			queue_sprite_at_depth(&ignored, &depth);
			current_object = LINKED(current_object, OBJECT(current_object)->child);
			offset_x = 0u;
			offset_y = (uint16_t)(OBJECT(object_cursor)->floor_z_int - OBJECT(object_cursor)->z_int);
			place_sprite_near_cursor(offset_x, offset_y);
			bucket = 0xFFu;
			queue_sprite_in_bucket(bucket);
			return;
		}
		OBJECT(current_object)->direction = 0x1E0u;
	}
	current_object = LINKED(current_object, OBJECT(current_object)->child);
	offset_x = 0u;
	offset_y = (uint16_t)(OBJECT(object_cursor)->floor_z_int - OBJECT(object_cursor)->z_int);
	place_sprite_near_cursor(offset_x, offset_y);
	bucket = 0xFFu;
	queue_sprite_in_bucket(bucket);
}

void turtle_surface(void)
{
	OBJECT(current_object)->speed_z = 0u - ((OBJECT(current_object)->speed_z + 0x8000u) & 0x3FFFFu);
	if ((int16_t)(OBJECT(current_object)->floor_z_int - 0x20u) < OBJECT(current_object)->z_int)
	{
		turtle_swim();
		return;
	}
	OBJECT(current_object)->speed_z = 0;
	OBJECT(current_object)->handler = turtle_drop_rings_ADDRESS;
	turtle_drop_rings();
}

void turtle_drop_rings(void)
{
	uint32_t r11 = 0, random_value = 0, ring = 0;
	uint16_t spawn_arg_1 = 0;

	spawn_arg_1 = OBJECT(current_object)->spawn_arg_1;
	OBJECT(current_object)->spawn_arg_1 = spawn_arg_1 - 1u;
	if (spawn_arg_1 != 0u)
	{
		turtle_swim();
		return;
	}
	OBJECT(current_object)->spawn_arg_1 = 0x30u;
	OBJECT(current_object)->spawn_arg_2++;
	if ((int16_t)OBJECT(current_object)->spawn_arg_2 > 4)
		OBJECT(current_object)->handler = turtle_swim_ADDRESS;
	released_ring_flip = 2u;
	released_rings_random = 0u;
	released_ring_angle = 0x154u;
	rings_to_release = 1u;
	drop_rings(0u, &r11, &random_value, &ring);
	turtle_swim();
}

void turtle_swim(void)
{
	uint16_t offset_x = 0, offset_y = 0, shadow_offset_y = 0;
	uint32_t r2 = 0, r3 = 0, r11 = 0, bucket = 0, deleted = 0, direction = 0, height_above_floor = 0, ignored = 0, speed = 0, speed_x = 0, speed_y = 0, stroke_step = 0;
	uint8_t wrapped_stroke_step = 0, wrapped_head_step = 0;
	uint16_t stroke_timer = 0, direction_2 = 0;

	stroke_timer = OBJECT(current_object)->turtle.stroke_timer;
	OBJECT(current_object)->turtle.stroke_timer = stroke_timer - 1u;
	if (stroke_timer == 0u)
	{
		OBJECT(current_object)->turtle.stroke_timer = 2u;
		stroke_step = OBJECT(current_object)->turtle.stroke_step;
		OBJECT(current_object)->sprite_frame = turtle_swim_frames[stroke_step];
		OBJECT(current_object)->speed = turtle_swim_speeds[stroke_step];
		OBJECT(current_object)->turtle.stroke_step++;
		wrapped_stroke_step = OBJECT(current_object)->turtle.stroke_step & 0xFu;
		OBJECT(current_object)->turtle.stroke_step = wrapped_stroke_step;
		if (wrapped_stroke_step == 0u)
			OBJECT(current_object)->turtle.stroke_timer = 0x3Cu;
	}
	OBJECT(current_object)->speed -= 3u;
	direction = 0x14Cu;
	speed = OBJECT(current_object)->speed + 0xE0u;
	direction_to_speed(speed, &direction, &speed_x, &speed_y);
	OBJECT(current_object)->speed_x = speed_x;
	OBJECT(current_object)->speed_y = speed_y;
	move_object_projected(&ignored, &r2, &r3, &r11);
	delete_if_off_screen(&deleted, &r2, &r3);
	if ((uint8_t)deleted != 0u)
		return;
	current_object = LINKED(current_object, OBJECT(current_object)->child);
	direction_2 = OBJECT(current_object)->direction;
	OBJECT(current_object)->direction = direction_2 - 1u;
	if (direction_2 == 0u)
	{
		OBJECT(current_object)->direction = 1u;
		OBJECT(current_object)->sprite_frame = turtle_head_frames[turtle_head_steps[AT16(OBJECT(current_object)->projected_y, 1)]];
		AT8(OBJECT(current_object)->projected_y, 1)++;
		wrapped_head_step = AT8(OBJECT(current_object)->projected_y, 1) & 0xFu;
		AT8(OBJECT(current_object)->projected_y, 1) = wrapped_head_step;
		if (wrapped_head_step == 0u)
			OBJECT(current_object)->direction = 0x78u;
	}
	offset_x = 0xBu;
	offset_y = 0xFFEDu;
	place_sprite_near_cursor(offset_x, offset_y);
	height_above_floor = (uint16_t)(OBJECT(object_cursor)->floor_z_int - OBJECT(object_cursor)->z_int);
	r14 = height_above_floor;
	height_above_floor = (uint16_t)(height_above_floor + OBJECT(current_object)->screen_y + 0x13u + 1u);
	queue_sprite_at_depth(&bucket, &height_above_floor);
	if (OBJECT(object_cursor)->floor_z_int < 0x800)
	{
		current_object = LINKED(current_object, OBJECT(current_object)->child);
		offset_x = 0u;
		shadow_offset_y = (uint16_t)r14;
		place_sprite_near_cursor(offset_x, shadow_offset_y);
		OBJECT(current_object)->sprite_frame = OBJECT(object_cursor)->sprite_frame;
		OBJECT(current_object)->sprite_scale = 0x100u;
		OBJECT(current_object)->sprite_scale -= shadow_offset_y;
		bucket = 0xFFu;
		queue_sprite_in_bucket(bucket);
	}
	current_object = object_cursor;
	queue_sprite_at_floor();
	add_to_touchable_list(bucket);
}

void eel_init(void)
{
	uint16_t explode_delay_step = 0;
	uint32_t block = 0, free_count = 0, object = 0, object_2 = 0, piece_count = 0, segment_amplitude = 0, segments_left = 0, spawned = 0;

	OBJECT(current_object)->handler = eel_anchor_update_ADDRESS;
	OBJECT(current_object)->sprite_flags = 0u;
	OBJECT(current_object)->sprite_frame = 0xA898u;
	OBJECT(current_object)->sprite_palette = 0x1210u;
	OBJECT(current_object)->sprite_palette |= 0x4000u;
	OBJECT(current_object)->hitbox_size_x = 8u;
	OBJECT(current_object)->hitbox_height = 0xAu;
	OBJECT(current_object)->kind = 0x24u;
	OBJECT(current_object)->eel_init.stretch_step = 0x10;
	OBJECT(current_object)->speed = 0x480;
	OBJECT(current_object)->direction = 0x34Au;
	if (OBJECT(current_object)->eel_init.facing != 0u)
		OBJECT(current_object)->direction = 0x14Au;
	current_object = LINKED(current_object, OBJECT(current_object)->child);
	OBJECT(current_object)->sprite_flags = 0u;
	OBJECT(current_object)->sprite_palette = 0x1200u;
	OBJECT(current_object)->sprite_palette |= 0x4000u;
	current_object = LINKED(current_object, OBJECT(current_object)->child);
	OBJECT(current_object)->sprite_flags = 0u;
	OBJECT(current_object)->sprite_palette = 0x12E0u;
	OBJECT(current_object)->sprite_palette |= 0x4000u;
	current_object = LINKED(current_object, OBJECT(current_object)->child);
	OBJECT(current_object)->sprite_flags = 0u;
	OBJECT(current_object)->sprite_frame = 0xA898u;
	OBJECT(current_object)->sprite_palette = 0x7A0u;
	OBJECT(current_object)->sprite_palette |= 0x4000u;
	r14 = 0x30u;
	explode_delay_step = 0x30u;
	segments_left = OBJECT(object_cursor)->eel_init.piece_count;
	segment_amplitude = OBJECT(object_cursor)->eel_init.wave_step;
	current_player = current_object;
	spawn_count = 4u;
	spawn_object_with_pieces(0x014A32u, &spawned, &piece_count, &free_count, &object, &block);
	for (;;)
	{
		OBJECT(current_player)->child = object;
		copy_position_to(object);
		OBJECT(object)->sprite_flags = 0u;
		OBJECT(object)->sprite_frame = 0xA898u;
		OBJECT(object)->sprite_palette = 0x1210u;
		OBJECT(object)->sprite_palette |= 0x4000u;
		OBJECT(object)->eel_segment.explode_delay = r14;
		OBJECT(object)->hitbox_size_x = 0x10u;
		OBJECT(object)->hitbox_height = 0x10u;
		OBJECT(object)->kind = 0x24u;
		OBJECT(object)->spawn_arg_1 = segment_amplitude;
		OBJECT(object)->spawn_arg_2 = OBJECT(object_cursor)->spawn_arg_1;
		OBJECT(object)->eel_segment.height_offset = 0x4000u * segment_amplitude;
		OBJECT(object)->handler = eel_segment_move_ADDRESS;
		object_2 = object;
		object = LINKED(object, OBJECT(object)->child);
		OBJECT(object)->sprite_flags = 0u;
		OBJECT(object)->sprite_palette = 0x1200u;
		OBJECT(object)->sprite_palette |= 0x4000u;
		object = LINKED(object, OBJECT(object)->child);
		OBJECT(object)->sprite_flags = 0u;
		OBJECT(object)->sprite_palette = 0x12E0u;
		OBJECT(object)->sprite_palette |= 0x4000u;
		AT8(OBJECT(object)->projected_y, 1) = (uint8_t)segment_amplitude % 0x1Fu;
		object = LINKED(object, OBJECT(object)->child);
		OBJECT(object)->sprite_flags = 0u;
		OBJECT(object)->sprite_frame = 0xA898u;
		OBJECT(object)->sprite_palette = 0x7A0u;
		OBJECT(object)->sprite_palette |= 0x4000u;
		current_player = object;
		r14 = (uint16_t)(r14 + explode_delay_step);
		segment_amplitude = (uint16_t)(segment_amplitude + OBJECT(current_object)->eel_init.wave_step);
		segments_left--;
		if (segments_left == 0u)
			break;
		spawn_count = 4u;
		spawn_object_with_pieces(0x014A3Fu, &spawned, &piece_count, &free_count, &object, &block);
	}
	OBJECT(object_2)->sprite_frame = 0xAA38u;
	OBJECT(object_2)->sprite_palette = 0x1210u;
	OBJECT(object_2)->sprite_palette |= 0x4000u;
	OBJECT(object_2)->handler = eel_head_move_ADDRESS;
	object_2 = LINKED(object_2, OBJECT(object_2)->child);
	OBJECT(object_2)->sprite_flags = 0u;
	OBJECT(object_2)->sprite_frame = 0xAA38u;
	OBJECT(object_2)->sprite_palette = 0x7A0u;
	OBJECT(object_2)->sprite_palette |= 0x4000u;
}

void eel_anchor_update(void)
{
	uint32_t r2 = 0, r3 = 0, r11 = 0, bucket = 0, deleted = 0, ignored = 0, segment_speed = 0, speed_step = 0, stretch_step = 0;
	uint16_t pause_timer = 0;

	if ((stage_trigger_bits & shl32(1u, (int)(OBJECT(current_object)->eel_anchor.den_number))) != 0u)
	{
		OBJECT(current_object)->handler = eel_anchor_wait_explode_ADDRESS;
		eel_anchor_wait_explode();
		return;
	}
	unproject_object(&ignored, &r2, &r3, &r11);
	delete_if_off_screen(&deleted, &r2, &r3);
	if ((uint8_t)deleted != 0u)
		return;
	queue_sprite_at_floor();
	add_to_touchable_list(deleted);
	if (OBJECT(current_object)->eel_anchor.pausing == 0u)
	{
	eel_anchor_stretch:
		stretch_step = OBJECT(current_object)->eel_anchor.stretch_step;
		OBJECT(current_object)->speed += stretch_step;
		if ((int32_t)stretch_step >= 0)
		{
			if (OBJECT(current_object)->speed > 0x1111)
			{
				OBJECT(current_object)->eel_anchor.pause_timer = 0x3Cu;
				OBJECT(current_object)->eel_anchor.pausing = 1u;
				OBJECT(current_object)->eel_anchor.stretch_step = -0x18;
			}
		}
		else if (OBJECT(current_object)->speed < 0x480)
		{
			OBJECT(current_object)->eel_anchor.pause_timer = 0x3Cu;
			OBJECT(current_object)->eel_anchor.pausing = 1u;
			OBJECT(current_object)->eel_anchor.stretch_step = 0x28;
		}
		segment_speed = OBJECT(current_object)->speed;
		speed_step = segment_speed;
		current_object = LINKED(current_object, OBJECT(object_cursor)->child);
		current_object = LINKED(current_object, OBJECT(current_object)->child);
		current_object = LINKED(current_object, OBJECT(current_object)->child);
		for (;;)
		{
			current_object = LINKED(current_object, OBJECT(current_object)->child);
			OBJECT(current_object)->direction = OBJECT(object_cursor)->direction;
			OBJECT(current_object)->speed = segment_speed;
			OBJECT(current_object)->var_81 = OBJECT(object_cursor)->eel_anchor.stretch_step;
			current_object = LINKED(current_object, OBJECT(current_object)->child);
			current_object = LINKED(current_object, OBJECT(current_object)->child);
			current_object = LINKED(current_object, OBJECT(current_object)->child);
			if (OBJECT(current_object)->child == 0xFFFFu)
				break;
			segment_speed += speed_step;
		}
	}
	else
	{
		pause_timer = OBJECT(current_object)->eel_anchor.pause_timer;
		OBJECT(current_object)->eel_anchor.pause_timer = pause_timer - 1u;
		if (pause_timer == 0u)
		{
			OBJECT(current_object)->eel_anchor.pausing = 0u;
			goto eel_anchor_stretch;
		}
	}
	current_object = object_cursor;
	current_object = LINKED(current_object, OBJECT(current_object)->child);
	current_object = LINKED(current_object, OBJECT(current_object)->child);
	current_object = LINKED(current_object, OBJECT(current_object)->child);
	bucket = place_sprite_at_cursor_floor();
	queue_sprite_in_bucket(bucket);
}

void eel_anchor_wait_explode(void)
{
	uint32_t r2 = 0, r3 = 0, distance_y = 0, ignored = 0;

	if (OBJECT(current_object)->eel_anchor.piece_count <= 0)
		delete_object_and_children(&ignored, &r2, &r3);
	if (OBJECT(current_object)->eel_anchor.facing == 0u)
	{
		r14 = LINKED(WORK_RAM, chaser_object);
		distance_xy_to_object(&ignored, &distance_y);
		if ((int32_t)distance_y <= 0x10)
			stage_trigger_bits |= shl32(1u, (int)(OBJECT(current_object)->eel_anchor.den_number));
	}
}

void eel_head_move(void)
{
	uint16_t explosion_delay = 0, head_offset_x = 0, head_offset_y = 0;
	uint32_t r11 = 0, bucket = 0, ignored = 0;
	uint16_t frame_timer = 0, frame_timer_2 = 0;

	if ((stage_trigger_bits & shl32(1u, (int)(OBJECT(current_object)->eel_head.den_number))) == 0u)
	{
		place_eel_piece();
		if (OBJECT(current_object)->eel_head.facing != 0u)
		{
			head_offset_x = 0xBu;
			head_offset_y = 0u;
			frame_timer = OBJECT(current_object)->eel_head.frame_timer;
			OBJECT(current_object)->eel_head.frame_timer = frame_timer - 1u;
			if (frame_timer == 0u)
			{
				OBJECT(current_object)->eel_head.frame_timer = 3u;
				if (OBJECT(current_object)->eel_head.stretch_step < 0)
				{
					OBJECT(current_object)->sprite_frame = eel_head_retract_frames[(uint8_t)OBJECT(current_object)->eel_head.frame_index];
					OBJECT(current_object)->eel_head.frame_index++;
					if (OBJECT(current_object)->eel_head.frame_index > 3)
					{
						OBJECT(current_object)->eel_head.frame_index = 0;
						OBJECT(current_object)->eel_head.frame_timer = 0x3Cu;
					}
				}
				else
				{
					OBJECT(current_object)->eel_head.frame_index = (uint8_t)OBJECT(current_object)->eel_head.frame_index % 3u;
					OBJECT(current_object)->sprite_frame = eel_head_extend_frames[(uint8_t)OBJECT(current_object)->eel_head.frame_index];
					OBJECT(current_object)->eel_head.frame_index++;
					if (OBJECT(current_object)->eel_head.frame_index > 2)
					{
						OBJECT(current_object)->eel_head.frame_index = 0;
						OBJECT(current_object)->eel_head.frame_timer = 0x3Cu;
					}
				}
			}
		}
		else
		{
			head_offset_x = 0xFFF4u;
			head_offset_y = 3u;
			frame_timer_2 = OBJECT(current_object)->eel_head.frame_timer;
			OBJECT(current_object)->eel_head.frame_timer = frame_timer_2 - 1u;
			if (frame_timer_2 == 0u)
			{
				OBJECT(current_object)->eel_head.frame_timer = 3u;
				OBJECT(current_object)->sprite_frame = eel_head_frames[(uint8_t)OBJECT(current_object)->eel_head.frame_index];
				OBJECT(current_object)->eel_head.frame_index++;
				if (OBJECT(current_object)->eel_head.frame_index > 2)
				{
					OBJECT(current_object)->eel_head.frame_index = 0;
					OBJECT(current_object)->eel_head.frame_timer = 0x3Cu;
				}
			}
		}
		OBJECT(current_object)->projected_x += head_offset_x;
		OBJECT(current_object)->projected_y_int += head_offset_y;
		move_object_projected(&ignored, &ignored, &ignored, &r11);
		queue_sprite_at_floor();
		current_object = LINKED(current_object, OBJECT(current_object)->child);
		bucket = place_sprite_at_cursor_floor();
		queue_sprite_in_bucket(bucket);
		return;
	}
	current_object = LINKED(current_object, OBJECT(current_object)->parent);
	explosion_delay = (uint16_t)(divide_s32((uint32_t)OBJECT(object_cursor)->speed, (uint32_t)OBJECT(current_object)->speed) * 0xBu);
	current_object = object_cursor;
	OBJECT(current_object)->eel_head.explode_delay = explosion_delay;
	OBJECT(current_object)->handler = eel_head_explode_delay_ADDRESS;
	eel_head_explode_delay();
}

void eel_segment_move(void)
{
	uint16_t explosion_delay = 0;
	uint32_t r11 = 0, bucket = 0, depth = 0, ignored = 0;

	if ((stage_trigger_bits & shl32(1u, (int)(OBJECT(current_object)->eel_segment.den_number))) == 0u)
	{
		place_eel_piece();
		move_object_projected(&ignored, &ignored, &ignored, &r11);
		depth = queue_sprite_at_floor();
		r14 = depth;
		add_to_touchable_list(ignored);
		current_object = LINKED(current_object, OBJECT(current_object)->child);
		current_object = LINKED(current_object, OBJECT(current_object)->child);
		current_object = LINKED(current_object, OBJECT(current_object)->child);
		bucket = place_sprite_at_cursor_floor();
		queue_sprite_in_bucket(bucket);
		return;
	}
	current_object = LINKED(current_object, OBJECT(current_object)->parent);
	explosion_delay = (uint16_t)(divide_s32((uint32_t)OBJECT(object_cursor)->speed, (uint32_t)OBJECT(current_object)->speed) * 0xBu);
	current_object = object_cursor;
	OBJECT(current_object)->eel_segment.explode_delay = explosion_delay;
	OBJECT(current_object)->handler = eel_segment_explode_delay_ADDRESS;
	eel_segment_explode_delay();
}

void place_eel_piece(void)
{
	uint32_t direction = 0, speed = 0, wave_offset = 0;

	current_object = LINKED(current_object, OBJECT(object_cursor)->parent);
	OBJECT(object_cursor)->projected_x = OBJECT(current_object)->projected_x;
	OBJECT(object_cursor)->projected_y = OBJECT(current_object)->projected_y;
	OBJECT(object_cursor)->z = OBJECT(current_object)->z;
	OBJECT(object_cursor)->floor_z = OBJECT(current_object)->floor_z;
	OBJECT(object_cursor)->z -= OBJECT(object_cursor)->place_eel_piece.height_offset;
	current_object = object_cursor;
	direction = OBJECT(current_object)->direction;
	speed = OBJECT(current_object)->speed;
	OBJECT(current_object)->speed_x = (int32_t)(sine_table[direction] * speed) >> 6;
	OBJECT(current_object)->speed_y = 0u - ((int32_t)(cosine_table[direction] * speed) >> 6);
	OBJECT(current_object)->place_eel_piece.wave_angle += 8u;
	OBJECT(current_object)->place_eel_piece.wave_angle &= 0x3FFu;
	wave_offset = (uint32_t)sine_table[OBJECT(current_object)->place_eel_piece.wave_angle] * OBJECT(current_object)->place_eel_piece.wave_amplitude;
	OBJECT(current_object)->z += wave_offset;
	OBJECT(current_object)->speed_y += wave_offset * 2u;
}

void eel_head_explode_delay(void)
{
	uint32_t r11 = 0, bucket = 0, ignored = 0;
	uint16_t explode_delay = 0;

	explode_delay = OBJECT(current_object)->eel_head.explode_delay;
	OBJECT(current_object)->eel_head.explode_delay = explode_delay - 1u;
	if (explode_delay == 0u)
	{
		eel_piece_start_explosion();
		return;
	}
	unproject_object(&ignored, &ignored, &ignored, &r11);
	queue_sprite_at_floor();
	current_object = LINKED(current_object, OBJECT(current_object)->child);
	bucket = place_sprite_at_cursor_floor();
	queue_sprite_in_bucket(bucket);
}

void eel_segment_explode_delay(void)
{
	uint32_t r11 = 0, bucket = 0, ignored = 0;
	uint16_t explode_delay = 0;

	explode_delay = OBJECT(current_object)->eel_segment.explode_delay;
	OBJECT(current_object)->eel_segment.explode_delay = explode_delay - 1u;
	if (explode_delay == 0u)
	{
		eel_piece_start_explosion();
		return;
	}
	unproject_object(&ignored, &ignored, &ignored, &r11);
	queue_sprite_at_floor();
	current_object = LINKED(current_object, OBJECT(current_object)->child);
	current_object = LINKED(current_object, OBJECT(current_object)->child);
	current_object = LINKED(current_object, OBJECT(current_object)->child);
	bucket = place_sprite_at_cursor_floor();
	queue_sprite_in_bucket(bucket);
}

void eel_piece_start_explosion(void)
{
	uint8_t sound_id = 0;
	uint32_t r1 = 0, r2 = 0;

	OBJECT(current_object)->projected_y_int += 0x18u;
	OBJECT(current_object)->eel_piece.frame_timer = 0u;
	OBJECT(current_object)->eel_piece.frame_index = 2;
	OBJECT(current_object)->sprite_palette = 0x1070u;
	OBJECT(current_object)->sprite_palette |= 0x4000u;
	OBJECT(current_object)->sprite_flags = 4u;
	OBJECT(current_object)->handler = eel_piece_explode_ADDRESS;
	sound_id = 0x24u;
	play_sound(sound_id, &r1, &r2);
	eel_piece_explode();
}

void eel_piece_explode(void)
{
	uint8_t sound_id = 0;
	uint32_t r1 = 0, r2 = 0, r11 = 0, depth = 0, ignored = 0;
	uint16_t frame_timer = 0;

	frame_timer = OBJECT(current_object)->eel_piece.frame_timer;
	OBJECT(current_object)->eel_piece.frame_timer = frame_timer - 1u;
	if (frame_timer == 0u)
	{
		OBJECT(current_object)->eel_piece.frame_timer = 2u;
		OBJECT(current_object)->sprite_frame = underwater_explosion_frames[(uint8_t)OBJECT(current_object)->eel_piece.frame_index];
		OBJECT(current_object)->eel_piece.frame_index++;
		if (OBJECT(current_object)->eel_piece.frame_index > 0x15)
		{
			current_object = LINKED(current_object, OBJECT(current_object)->parent);
			AS8(OBJECT(current_object)->spawn_arg_2)--;
			current_object = object_cursor;
			OBJECT(current_object)->handler = eel_piece_gone_ADDRESS;
			sound_id = 0x24u;
			play_sound(sound_id, &r1, &r2);
		}
	}
	unproject_object(&ignored, &ignored, &ignored, &r11);
	depth = (uint16_t)(OBJECT(current_object)->screen_y - 0x10u);
	queue_sprite_at_depth(&ignored, &depth);
	eel_piece_gone();
}

void eel_piece_gone(void)
{
}

void breakable_panel_init(void)
{
	OBJECT(current_object)->sprite_frame = 0x9278u;
	OBJECT(current_object)->sprite_palette = 0x91F0u;
	OBJECT(current_object)->flags |= 2u;
	OBJECT(current_object)->hitbox_size_x = 7u;
	OBJECT(current_object)->hitbox_size_y = 0x40u;
	OBJECT(current_object)->hitbox_offset_x = -4;
	OBJECT(current_object)->hitbox_offset_y = -0x28;
	OBJECT(current_object)->hitbox_height = 0x64u;
	OBJECT(current_object)->kind = 5u;
	OBJECT(current_object)->handler = breakable_panel_wait_ADDRESS;
	current_object = LINKED(current_object, OBJECT(current_object)->child);
	OBJECT(current_object)->sprite_frame = 0x9308u;
	OBJECT(current_object)->sprite_palette = 0x9200u;
}

void breakable_panel_wait(void)
{
	uint32_t r11 = 0, random_value = 0, ring = 0;

	if ((OBJECT(current_object)->flags & 4u) == 0u || (OBJECT(LINKED(WORK_RAM, OBJECT(current_object)->touched_by))->flags & 0x40u) == 0u)
	{
		draw_breakable_panel();
		return;
	}
	released_ring_flip = 0u;
	released_rings_random = 1u;
	released_ring_angle = OBJECT(current_object)->spawn_arg_1;
	rings_to_release = OBJECT(current_object)->spawn_arg_2;
	drop_rings(0u, &r11, &random_value, &ring);
	OBJECT(current_object)->handler = breakable_panel_break_ADDRESS;
	breakable_panel_break();
}

void breakable_panel_break(void)
{
	uint16_t frame_timer = 0;

	frame_timer = OBJECT(current_object)->breakable_panel.frame_timer;
	OBJECT(current_object)->breakable_panel.frame_timer = frame_timer - 1u;
	if (frame_timer != 0u)
	{
		draw_breakable_panel();
		return;
	}
	OBJECT(current_object)->sprite_frame = breakable_panel_break_frames[(uint8_t)OBJECT(current_object)->breakable_panel.frame_index];
	OBJECT(current_object)->breakable_panel.frame_timer = 2u;
	OBJECT(current_object)->breakable_panel.frame_index++;
	if (OBJECT(current_object)->breakable_panel.frame_index <= 0x10)
	{
		draw_breakable_panel();
		return;
	}
	OBJECT(current_object)->handler = draw_breakable_panel_ADDRESS;
	draw_breakable_panel();
}

void draw_breakable_panel(void)
{
	uint16_t offset_x = 0, offset_y = 0;
	uint32_t r2 = 0, r3 = 0, r11 = 0, deleted = 0, depth = 0, ignored = 0;
	uint16_t direction = 0;

	unproject_object(&ignored, &r2, &r3, &r11);
	delete_if_off_screen(&deleted, &r2, &r3);
	if ((uint8_t)deleted != 0u)
		return;
	depth = (uint16_t)(OBJECT(current_object)->screen_y - 0x20u);
	queue_sprite_at_depth(&ignored, &depth);
	add_to_touchable_list(ignored);
	if ((uint8_t)OBJECT(current_object)->draw_breakable_panel.frame_index != 0u)
		return;
	current_object = LINKED(current_object, OBJECT(current_object)->child);
	direction = OBJECT(current_object)->direction;
	OBJECT(current_object)->direction = direction - 1u;
	if (direction == 0u)
	{
		OBJECT(current_object)->sprite_palette = breakable_panel_glow_palettes[breakable_panel_glow_steps[AT8(OBJECT(current_object)->projected_y, 1)]];
		OBJECT(current_object)->direction = 2u;
		AT8(OBJECT(current_object)->projected_y, 1)++;
		if (AT8(OBJECT(current_object)->projected_y, 1) == 0xFu)
			OBJECT(current_object)->direction = 0xFu;
		if ((int8_t)AT8(OBJECT(current_object)->projected_y, 1) > 0x1C)
		{
			OBJECT(current_object)->direction = 0x1Eu;
			AT8(OBJECT(current_object)->projected_y, 1) = 0u;
		}
	}
	offset_x = 0u;
	offset_y = 0u;
	place_sprite_near_cursor(offset_x, offset_y);
	depth = (uint16_t)(OBJECT(object_cursor)->screen_y - 0x19u);
	queue_sprite_at_depth(&ignored, &depth);
}

void white_puff_init(void)
{
	OBJECT(current_object)->sprite_palette = 0x90A0u;
	OBJECT(current_object)->sprite_palette = 0x2000u;
	OBJECT(current_object)->handler = white_puff_animate_ADDRESS;
}

void white_puff_animate(void)
{
	uint32_t r2 = 0, r3 = 0, r11 = 0, deleted = 0, ignored = 0;
	uint8_t wrapped_frame_index = 0;
	uint16_t frame_timer = 0;

	unproject_object(&ignored, &r2, &r3, &r11);
	delete_if_off_screen(&deleted, &r2, &r3);
	if ((uint16_t)deleted != 0u)
		return;
	frame_timer = OBJECT(current_object)->white_puff.frame_timer;
	OBJECT(current_object)->white_puff.frame_timer = frame_timer - 1u;
	if (frame_timer == 0u)
	{
		OBJECT(current_object)->sprite_frame = white_puff_frames[OBJECT(current_object)->white_puff.frame_index];
		OBJECT(current_object)->white_puff.frame_timer = 3u;
		OBJECT(current_object)->white_puff.frame_index++;
		wrapped_frame_index = OBJECT(current_object)->white_puff.frame_index % 0xAu;
		OBJECT(current_object)->white_puff.frame_index = wrapped_frame_index;
		if (wrapped_frame_index == 0u)
			OBJECT(current_object)->white_puff.frame_timer = 0x5Au;
	}
	queue_sprite(0u);
}

void checker_pad_init(void)
{
	OBJECT(current_object)->sprite_palette = 0x9040u;
	OBJECT(current_object)->handler = checker_pad_animate_ADDRESS;
}

void checker_pad_animate(void)
{
	uint32_t r2 = 0, r3 = 0, deleted = 0;
	uint16_t frame_timer = 0;

	update_screen_position_no_offset();
	delete_if_off_screen(&deleted, &r2, &r3);
	if ((uint8_t)deleted != 0u)
		return;
	frame_timer = OBJECT(current_object)->checker_pad.frame_timer;
	OBJECT(current_object)->checker_pad.frame_timer = frame_timer - 1u;
	if (frame_timer == 0u)
	{
		OBJECT(current_object)->sprite_frame = checker_pad_frames[AS16(OBJECT(current_object)->checker_pad.frame_index)];
		OBJECT(current_object)->checker_pad.frame_timer = 8u;
		OBJECT(current_object)->checker_pad.frame_index++;
		if (OBJECT(current_object)->checker_pad.frame_index > 4)
			OBJECT(current_object)->checker_pad.frame_index = 0;
	}
	queue_sprite(0u);
}

void static_triangle_init(void)
{
	OBJECT(current_object)->sprite_palette = 0x9050u;
	OBJECT(current_object)->handler = static_triangle_animate_ADDRESS;
	current_object = LINKED(current_object, OBJECT(current_object)->child);
	OBJECT(current_object)->sprite_frame = 0x7828u;
	OBJECT(current_object)->sprite_palette = 0x9060u;
}

void static_triangle_animate(void)
{
	uint16_t offset_x = 0, offset_y = 0;
	uint32_t r2 = 0, r3 = 0, deleted = 0, depth = 0;
	uint16_t frame_timer = 0, direction = 0;

	update_screen_position_no_offset();
	delete_if_off_screen(&deleted, &r2, &r3);
	if ((uint8_t)deleted != 0u)
		return;
	frame_timer = OBJECT(current_object)->static_triangle.frame_timer;
	OBJECT(current_object)->static_triangle.frame_timer = frame_timer - 1u;
	if (frame_timer == 0u)
	{
		OBJECT(current_object)->sprite_frame = static_triangle_frames[(uint8_t)OBJECT(current_object)->static_triangle.frame_index];
		OBJECT(current_object)->static_triangle.frame_timer = 2u;
		OBJECT(current_object)->static_triangle.frame_index++;
		if (OBJECT(current_object)->static_triangle.frame_index > 2)
		{
			OBJECT(current_object)->static_triangle.frame_index = 0;
			OBJECT(current_object)->var_81 = 0u;
		}
	}
	depth = queue_sprite(0u);
	current_object = LINKED(current_object, OBJECT(current_object)->child);
	direction = OBJECT(current_object)->direction;
	OBJECT(current_object)->direction = direction - 1u;
	if (direction == 0u)
	{
		OBJECT(current_object)->direction = 1u;
		offset_x = 0x68u;
		offset_y = 0u;
		place_sprite_near_cursor(offset_x, offset_y);
		queue_sprite(depth);
	}
}

void railing_init(void)
{
	uint32_t block = 0, object = 0, spawned = 0;

	OBJECT(current_object)->sprite_frame = 0x7838u;
	OBJECT(current_object)->sprite_palette = 0x9080u;
	OBJECT(current_object)->flags |= 2u;
	OBJECT(current_object)->hitbox_size_x = 0xFu;
	OBJECT(current_object)->hitbox_size_y = 0x57u;
	OBJECT(current_object)->hitbox_offset_x = 3;
	OBJECT(current_object)->hitbox_offset_y = -0x67;
	OBJECT(current_object)->hitbox_height = 0x100u;
	OBJECT(current_object)->kind = 0xCu;
	OBJECT(current_object)->handler = railing_draw_ADDRESS;
	spawn_count = 1u;
	spawn_objects(0x015389u, &spawned, &object, &block);
	OBJECT(object_cursor)->child = object;
	copy_position_to(object);
	OBJECT(object)->flags |= 2u;
	OBJECT(object)->hitbox_size_x = 0x56u;
	OBJECT(object)->hitbox_size_y = 0x11u;
	OBJECT(object)->hitbox_offset_x = 0x12;
	OBJECT(object)->hitbox_offset_y = -0x11;
	OBJECT(object)->hitbox_height = 0x100u;
	OBJECT(object)->kind = 0xCu;
	OBJECT(object)->handler = railing_side_update_ADDRESS;
}

void railing_draw(void)
{
	uint32_t r2 = 0, r3 = 0, r11 = 0, deleted = 0, ignored = 0;

	unproject_object(&ignored, &r2, &r3, &r11);
	delete_if_off_screen(&deleted, &r2, &r3);
	if ((uint8_t)deleted == 0u)
	{
		queue_sprite(0u);
		add_to_touchable_list(deleted);
		return;
	}
}

void railing_side_update(void)
{
	uint32_t r11 = 0, ignored = 0;

	unproject_object(&ignored, &ignored, &ignored, &r11);
	add_to_touchable_list(ignored);
}

void spike_gate_init(void)
{
	uint32_t block = 0, free_count = 0, object = 0, piece_count = 0, spawned = 0;

	OBJECT(current_object)->sprite_frame = 0xA200u;
	OBJECT(current_object)->sprite_palette = 0x91F0u;
	OBJECT(current_object)->flags |= 2u;
	OBJECT(current_object)->hitbox_size_x = 0x2Du;
	OBJECT(current_object)->hitbox_size_y = 0x21u;
	OBJECT(current_object)->hitbox_offset_x = -0xE;
	OBJECT(current_object)->hitbox_offset_y = -0x1A;
	OBJECT(current_object)->hitbox_height = 0x100u;
	OBJECT(current_object)->kind = 0xCu;
	OBJECT(current_object)->handler = spike_gate_pillar_update_ADDRESS;
	spawn_count = 1u;
	spawn_object_with_pieces(0x01543Du, &spawned, &piece_count, &free_count, &object, &block);
	copy_position_to(object);
	OBJECT(object)->projected_x += 0x94u;
	OBJECT(object)->projected_y_int += 0x4Au;
	OBJECT(object)->sprite_frame = 0xA208u;
	OBJECT(object)->sprite_palette = 0x91F0u;
	OBJECT(object)->flags |= 2u;
	OBJECT(object)->hitbox_size_x = 0x2Cu;
	OBJECT(object)->hitbox_size_y = 0x29u;
	OBJECT(object)->hitbox_offset_x = -9;
	OBJECT(object)->hitbox_offset_y = -0x20;
	OBJECT(object)->hitbox_height = 0x100u;
	OBJECT(object)->kind = 0xCu;
	OBJECT(object)->spawn_arg_2 = OBJECT(object_cursor)->spawn_arg_2;
	OBJECT(object)->handler = spike_gate_pillar_update_ADDRESS;
	spawn_count = 4u;
	spawn_object_with_pieces(0x0154A4u, &spawned, &piece_count, &free_count, &object, &block);
	copy_position_to(object);
	OBJECT(object)->direction = 0x300u;
	OBJECT(object)->projected_x += 0x1Au;
	OBJECT(object)->projected_y_int += 0x14u;
	OBJECT(object)->sprite_frame = 0xA1E8u;
	OBJECT(object)->sprite_palette = 0x91E0u;
	OBJECT(object)->flags |= 2u;
	OBJECT(object)->hitbox_size_x = 0x2Du;
	OBJECT(object)->hitbox_size_y = 0xDAu;
	OBJECT(object)->hitbox_offset_x = 3;
	OBJECT(object)->hitbox_offset_y = -0x38;
	OBJECT(object)->hitbox_height = 0x28u;
	OBJECT(object)->kind = 0xCu;
	OBJECT(object)->spawn_arg_2 = OBJECT(object_cursor)->spawn_arg_2;
	OBJECT(object)->handler = spike_gate_beam_wait_ADDRESS;
	object = LINKED(object, OBJECT(object)->child);
	OBJECT(object)->sprite_frame = 0xA1F0u;
	OBJECT(object)->sprite_palette = 0x91E0u;
	object = LINKED(object, OBJECT(object)->child);
	OBJECT(object)->sprite_frame = 0xA1F8u;
	OBJECT(object)->sprite_palette = 0x91E0u;
	object = LINKED(object, OBJECT(object)->child);
	OBJECT(object)->sprite_scale = 0x180u;
	OBJECT(object)->sprite_frame = 0xA788u;
	OBJECT(object)->sprite_palette = 0x87A0u;
}

void spike_gate_pillar_update(void)
{
	uint16_t count = 0;
	uint32_t r2 = 0, r3 = 0, r11 = 0, depth = 0, head_distance_x = 0, ignored = 0;

	unproject_object(&ignored, &r2, &r3, &r11);
	r14 = LINKED(WORK_RAM, biting_head);
	distance_xy_to_object(&head_distance_x, &ignored);
	if ((int32_t)head_distance_x <= 0x20 || (int16_t)OBJECT(current_object)->spawn_arg_2 < (int16_t)scroll_position)
	{
		count = 5u;
		OBJECT(current_object)->projected_x -= 0x90u;
		OBJECT(current_object)->projected_y_int -= 0x40u;
		spawn_debris(head_distance_x, count, r11);
		delete_object_and_children(&ignored, &r2, &r3);
		return;
	}
	depth = (uint16_t)(OBJECT(current_object)->screen_y - 8u);
	queue_sprite_at_depth(&ignored, &depth);
	add_to_touchable_list(ignored);
}

void spike_gate_beam_rise(void)
{
	uint16_t height_above_floor = 0;

	OBJECT(current_object)->z_int -= 3u;
	height_above_floor = (uint16_t)(OBJECT(current_object)->floor_z_int - OBJECT(current_object)->z_int);
	if ((int16_t)height_above_floor > 0x20)
		OBJECT(current_object)->kind = 0x24u;
	if ((int16_t)height_above_floor <= 0x50)
	{
		spike_gate_beam_update();
		return;
	}
	OBJECT(current_object)->spike_gate_beam.pause_timer = 8u;
	OBJECT(current_object)->handler = spike_gate_beam_hold_ADDRESS;
	spike_gate_beam_hold();
}

void spike_gate_beam_hold(void)
{
	uint16_t pause_timer = 0;

	pause_timer = OBJECT(current_object)->spike_gate_beam.pause_timer;
	OBJECT(current_object)->spike_gate_beam.pause_timer = pause_timer - 1u;
	if (pause_timer != 0u)
	{
		spike_gate_beam_update();
		return;
	}
	OBJECT(current_object)->handler = spike_gate_beam_drop_ADDRESS;
	spike_gate_beam_drop();
}

void spike_gate_beam_drop(void)
{
	uint8_t sound_id = 0;
	uint32_t r1 = 0, r2 = 0, height = 0;

	OBJECT(current_object)->speed_z += 0x2000u;
	fall_to_floor(&height, &r1, &r2);
	if ((int32_t)height >= 1)
	{
		spike_gate_beam_update();
		return;
	}
	OBJECT(current_object)->kind = 0xCu;
	OBJECT(current_object)->z_int -= 6u;
	OBJECT(current_object)->spike_gate_beam.bounce_count++;
	if (OBJECT(current_object)->spike_gate_beam.bounce_count <= 2)
	{
		spike_gate_beam_update();
		return;
	}
	OBJECT(current_object)->spike_gate_beam.bounce_count = 0;
	OBJECT(current_object)->z_int += 3u;
	OBJECT(current_object)->spike_gate_beam.pause_timer = 0xAu;
	sound_id = 7u;
	play_sound(sound_id, &r1, &r2);
	OBJECT(current_object)->handler = spike_gate_beam_wait_ADDRESS;
	spike_gate_beam_wait();
}

void spike_gate_beam_wait(void)
{
	uint16_t pause_timer = 0;

	pause_timer = OBJECT(current_object)->spike_gate_beam.pause_timer;
	OBJECT(current_object)->spike_gate_beam.pause_timer = pause_timer - 1u;
	if (pause_timer != 0u)
	{
		spike_gate_beam_update();
		return;
	}
	OBJECT(current_object)->handler = spike_gate_beam_rise_ADDRESS;
	spike_gate_beam_update();
}

void spike_gate_beam_update(void)
{
	uint16_t count = 0, offset_x = 0, offset_y = 0;
	uint32_t r2 = 0, r3 = 0, r11 = 0, bucket = 0, depth = 0, head_distance_x = 0, ignored = 0;

	move_object_projected(&ignored, &r2, &r3, &r11);
	r14 = LINKED(WORK_RAM, biting_head);
	distance_xy_to_object(&head_distance_x, &depth);
	if ((int32_t)head_distance_x <= 0x20 || (int16_t)OBJECT(current_object)->spawn_arg_2 < (int16_t)scroll_position)
	{
		count = 5u;
		OBJECT(current_object)->projected_x -= 0x90u;
		OBJECT(current_object)->projected_y_int -= 0x40u;
		spawn_debris(head_distance_x, count, r11);
		delete_object_and_children(&ignored, &r2, &r3);
		return;
	}
	depth = cursor_floor_screen_y(depth);
	depth = (uint16_t)(depth - 0x10u);
	queue_sprite_at_depth(&ignored, &depth);
	add_to_touchable_list(ignored);
	current_object = LINKED(current_object, OBJECT(current_object)->child);
	offset_x = 0x30u;
	offset_y = 0x19u;
	place_sprite_near_cursor(offset_x, offset_y);
	depth = cursor_floor_screen_y(depth);
	depth = (uint16_t)(depth - 0x14u);
	queue_sprite_at_depth(&ignored, &depth);
	current_object = LINKED(current_object, OBJECT(current_object)->child);
	offset_x = 0x60u;
	offset_y = 0x2Du;
	place_sprite_near_cursor(offset_x, offset_y);
	depth = cursor_floor_screen_y(depth);
	depth = (uint16_t)(depth - 0x10u);
	queue_sprite_at_depth(&ignored, &depth);
	current_object = LINKED(current_object, OBJECT(current_object)->child);
	bucket = place_sprite_at_cursor_floor();
	OBJECT(current_object)->screen_x += 0x30u;
	OBJECT(current_object)->screen_y += 0x48u;
	queue_sprite_in_bucket(bucket);
}

void eel_den_init(void)
{
	OBJECT(current_object)->flags |= 2u;
	OBJECT(current_object)->hitbox_size_x = 0x47u;
	OBJECT(current_object)->hitbox_size_y = 0x42u;
	OBJECT(current_object)->hitbox_offset_x = -3;
	OBJECT(current_object)->hitbox_offset_y = -0x43;
	OBJECT(current_object)->hitbox_height = 0x28u;
	OBJECT(current_object)->kind = 0xCu;
	OBJECT(current_object)->sprite_frame = 0xAC70u;
	OBJECT(current_object)->sprite_palette = 0x1300u;
	OBJECT(current_object)->sprite_palette |= 0x4000u;
	OBJECT(current_object)->health = 1;
	OBJECT(current_object)->handler = eel_den_wait_ADDRESS;
	current_object = object_cursor;
}

void eel_den_wait(void)
{
	uint32_t r11 = 0, distance_y = 0, ignored = 0, random_value = 0, ring = 0;

	if ((stage_trigger_bits & shl32(1u, (int)(OBJECT(current_object)->spawn_arg_1))) == 0u)
	{
		r14 = LINKED(WORK_RAM, chaser_object);
		distance_xy_to_object(&ignored, &distance_y);
		if ((int16_t)distance_y > 0x35 && ((OBJECT(current_object)->flags & 4u) == 0u || OBJECT(current_object)->health > 0))
		{
			eel_den_draw();
			return;
		}
	}
	stage_trigger_bits |= shl32(1u, (int)(OBJECT(current_object)->spawn_arg_1));
	OBJECT(current_object)->handler = eel_den_collapse_ADDRESS;
	OBJECT(current_object)->projected_y_int -= 0x18u;
	released_ring_angle = 0x144u;
	rings_to_release = 3u;
	released_ring_flip = 2u;
	released_rings_random = 1u;
	drop_rings(0u, &r11, &random_value, &ring);
	OBJECT(current_object)->projected_y_int += 0x18u;
	eel_den_collapse();
}
