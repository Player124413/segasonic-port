/**
 *    Name:        game_11.c
 *    Description: the game's functions, part 11 (in the order of their V60 addresses)
 *    Version:     0.1.0
 *    Created:     12-10-2026
 *    Modified:    13-10-2026
 *    Author:      Sebastian Toyos
 *
 *    SegaSonic The Hedgehog (arcade, Sega System 32): the game's V60 program decompiled into C by
 *    tools/decompile.py, with the names of symbols.txt.
 */
#include "game.h"

void draw_bcd_4(uint32_t value, uint32_t attribute, uint32_t text_position)
{
	uint32_t digit_count = 0;

	value = (uint16_t)value;
	digit_count = 4u;
	draw_bcd_digits(value, digit_count, &attribute, &text_position);
}

void draw_bcd_6(uint32_t value, uint32_t attribute, uint32_t text_position)
{
	uint32_t digit_count = 0;

	value &= 0xFFFFFFu;
	digit_count = 6u;
	draw_bcd_digits(value, digit_count, &attribute, &text_position);
}

void draw_bcd_8(uint32_t value, uint32_t attribute, uint32_t text_position)
{
	uint32_t digit_count = 0;

	digit_count = 8u;
	draw_bcd_digits(value, digit_count, &attribute, &text_position);
}

void draw_bcd_digits(uint32_t value, uint32_t digit_count, uint32_t *p_attribute, uint32_t *p_text_position)
{
	uint16_t attribute = (uint16_t)*p_attribute;
	uint32_t text_position = *p_text_position;
	uint8_t shift = 0;
	uint32_t remaining = 0;

	text_position += digit_count * 2u;
	shift = 0xFCu;
	do
	{
		attribute = (uint16_t)((attribute & ~0xFFu) | (uint8_t)value);
		attribute = (uint16_t)((attribute & ~0xFFu) | (attribute & 0xFu));
		attribute = (uint16_t)((attribute & ~0xFFu) | (uint8_t)(attribute + 1u));
		text_position -= 2u;
		mem_write16(text_position, attribute);
		remaining = shift_by(value, shift, 32);
		value = remaining;
		digit_count--;
	} while ((digit_count != 0u) & (remaining != 0u));
	if (digit_count == 0u)
	{
		*p_attribute = (*p_attribute & 0xFFFFFF00u) | (attribute & 0xFFu);
		*p_text_position = text_position;
		return;
	}
	do
	{
		text_position -= 2u;
		mem_write16(text_position, value);
		digit_count--;
	} while (digit_count != 0u);
	*p_attribute = (*p_attribute & 0xFFFFFF00u) | (attribute & 0xFFu);
	*p_text_position = text_position;
}

void init_sprites(uint32_t *p_end_mark, uint32_t *p_source)
{
	uint16_t end_mark = 0;
	uint32_t control_cursor = 0, control_init = 0, dest_end = 0, ignored = 0, list_start = 0, registers_left = 0;

	vblank_hook = swap_sprite_buffers_ADDRESS;
	frames_to_wait = 0;
	frames_per_wait = 0u;
	control_init = sprite_control_init_ADDRESS;
	mem_write16(sprite_flip, setting_monitor_flip);
	registers_left = 4u;
	control_cursor = SPRITE_CONTROL + 0x6u;
	do
	{
		mem_write16(control_cursor, ROM8(control_init));
		registers_left--;
		control_init++;
		control_cursor += 2u;
	} while (registers_left != 0u);
	move_string(sprite_list_header_ADDRESS, 0x10u, 0x400000u, 0x10u, 2u, 0u, 0u, fill_value, &dest_end, &ignored);
	list_start = dest_end;
	sprite_jump_init = 0xA400u;
	sprite_buffer_flip = 0u;
	sprite_back_buffer = list_start;
	sprite_list_start = list_start;
	end_mark = 0xFFFFu;
	mem_write16(list_start, 0xFFFFu);
	mem_write16(list_start + 0x1FF0u, 0xFFFFu);
	mem_write16(list_start + 0x3FE0u, 0xFFFFu);
	mem_write16(list_start + 0x5FD0u, 0xFFFFu);
	sprite_list_end_command = 0xFFFFu;
	sprite_frames = 0x130000u;
	ignored = clear_sprite_buckets();
	*p_end_mark = (*p_end_mark & 0xFFFF0000u) | end_mark;
	*p_source = control_init;
}

uint32_t clear_sprite_buckets(void)
{
	uint32_t bucket = 0, buckets_left = 0;

	sprite_buckets_end = 0xFFFFu;
	buckets_left = 0x100u;
	bucket = sprite_buckets_ADDRESS;
	do
	{
		mem_write16(bucket, bucket);
		bucket += 2u;
		buckets_left--;
	} while (buckets_left != 0u);
	return bucket;
}

uint32_t queue_sprite(uint32_t depth)
{
	uint32_t ignored = 0;

	depth = (depth & ~0xFFFFu) | (uint16_t)OBJECT(current_object)->screen_y;
	queue_sprite_at_depth(&ignored, &depth);
	return depth;
}

void queue_sprite_at_depth(uint32_t *p_bucket_head, uint32_t *p_depth)
{
	uint32_t depth = *p_depth;
	uint32_t bucket_head = 0;

	depth = (depth & ~0xFFFFu) | (uint16_t)(depth + 0x10u);
	if ((int16_t)depth < 0xFE)
	{
		if ((int16_t)depth >= 0)
		{
			bucket_head = sprite_buckets_ADDRESS + ((uint16_t)(0xFFu - depth) & 0xFFu) * 2u;
			OBJECT(current_object)->draw_next = mem_read16(bucket_head);
			mem_write16(bucket_head, current_object);
			*p_bucket_head = bucket_head;
			*p_depth = depth;
			return;
		}
		depth = 0u;
		bucket_head = sprite_buckets_ADDRESS + ((uint16_t)(0xFFu - depth) & 0xFFu) * 2u;
		OBJECT(current_object)->draw_next = mem_read16(bucket_head);
		mem_write16(bucket_head, current_object);
		*p_bucket_head = bucket_head;
		*p_depth = depth;
		return;
	}
	depth = 0xFEu;
	bucket_head = sprite_buckets_ADDRESS + ((uint16_t)(0xFFu - depth) & 0xFFu) * 2u;
	OBJECT(current_object)->draw_next = mem_read16(bucket_head);
	mem_write16(bucket_head, current_object);
	*p_bucket_head = bucket_head;
	*p_depth = depth;
}

void queue_sprite_in_layer(void)
{
	uint32_t bucket_head = 0;

	bucket_head = sprite_buckets_ADDRESS + (OBJECT(current_object)->sprite_layer & 0xFFu) * 2u;
	OBJECT(current_object)->draw_next = mem_read16(bucket_head);
	mem_write16(bucket_head, current_object);
}

void queue_sprite_on_top(void)
{
	uint8_t bucket = 0;

	bucket = 0u;
	queue_sprite_in_bucket(bucket);
}

void queue_sprite_in_bucket(uint32_t bucket)
{
	bucket = sprite_buckets_ADDRESS + (bucket & 0xFFu) * 2u;
	OBJECT(current_object)->draw_next = mem_read16(bucket);
	mem_write16(bucket, current_object);
}

uint32_t queue_sprite_at_floor(void)
{
	uint32_t bucket_head = 0, floor_y = 0;

	floor_y = (int32_t)((uint32_t)OBJECT(current_object)->projected_y + OBJECT(current_object)->floor_z) >> 16;
	floor_y = (floor_y & ~0xFFFFu) | (uint16_t)(floor_y - view_y);
	floor_y = (floor_y & ~0xFFFFu) | (uint16_t)(floor_y + 0x10u);
	if ((int16_t)floor_y < 0xFE)
	{
		if ((int16_t)floor_y < 0)
			floor_y = 0u;
	}
	else
		floor_y = 0xFEu;
	bucket_head = sprite_buckets_ADDRESS + ((uint16_t)(0xFFu - floor_y) & 0xFFu) * 2u;
	OBJECT(current_object)->draw_next = mem_read16(bucket_head);
	mem_write16(bucket_head, current_object);
	return floor_y;
}

uint32_t queue_sprite_at_previous_floor(void)
{
	uint32_t bucket_head = 0, floor_y = 0;

	floor_y = (int32_t)((uint32_t)OBJECT(current_object)->projected_y + OBJECT(current_object)->previous_floor_z) >> 16;
	floor_y = (floor_y & ~0xFFFFu) | (uint16_t)(floor_y - view_y);
	floor_y = (floor_y & ~0xFFFFu) | (uint16_t)(floor_y + 0x10u);
	if ((int16_t)floor_y < 0xFE)
	{
		if ((int16_t)floor_y < 0)
			floor_y = 0u;
	}
	else
		floor_y = 0xFEu;
	bucket_head = sprite_buckets_ADDRESS + ((uint16_t)(0xFFu - floor_y) & 0xFFu) * 2u;
	OBJECT(current_object)->draw_next = mem_read16(bucket_head);
	mem_write16(bucket_head, current_object);
	return floor_y;
}

uint32_t queue_sprite_at_base(uint32_t depth)
{
	uint32_t bucket_head = 0;

	depth = (depth & ~0xFFFFu) | (uint16_t)OBJECT(current_object)->screen_y;
	depth = (depth & ~0xFFFFu) | (uint16_t)(depth - OBJECT(current_object)->sprite_offset_y);
	depth = (depth & ~0xFFFFu) | (uint16_t)(depth + 0x10u);
	if ((int16_t)depth < 0xFE)
	{
		if ((int16_t)depth < 0)
			depth = 0u;
	}
	else
		depth = 0xFEu;
	bucket_head = sprite_buckets_ADDRESS + ((uint16_t)(0xFFu - depth) & 0xFFu) * 2u;
	OBJECT(current_object)->draw_next = mem_read16(bucket_head);
	mem_write16(bucket_head, current_object);
	return depth;
}

uint32_t depth_to_bucket(uint32_t bucket, uint16_t depth)
{
	depth += 0x10u;
	if ((int16_t)depth < 0xFE)
	{
		if ((int16_t)depth < 0)
			depth = 0u;
	}
	else
		depth = 0xFEu;
	bucket = (bucket & ~0xFFFFu) | 0xFFu;
	bucket = (bucket & ~0xFFFFu) | (uint16_t)(bucket - depth);
	return bucket;
}

void queue_sprite_at_depth_offset(uint16_t depth, uint8_t bucket_offset)
{
	uint32_t bucket_head = 0;

	depth += 0x10u;
	if ((int16_t)depth < 0xFE)
	{
		if ((int16_t)depth < 0)
			depth = 0u;
	}
	else
		depth = 0xFEu;
	bucket_head = sprite_buckets_ADDRESS + ((uint16_t)(0xFFu - depth + bucket_offset) & 0xFFu) * 2u;
	OBJECT(current_object)->draw_next = mem_read16(bucket_head);
	mem_write16(bucket_head, current_object);
}

void queue_sprite_at_previous_floor_2(void)
{
	uint16_t floor_depth = 0;
	uint32_t bucket_head = 0;

	floor_depth = (uint16_t)(((int32_t)((uint32_t)OBJECT(current_object)->projected_y + OBJECT(current_object)->previous_floor_z) >> 16) - view_y + 0x10u);
	if ((int16_t)floor_depth < 0xFE)
	{
		if ((int16_t)floor_depth < 0)
			floor_depth = 0u;
	}
	else
		floor_depth = 0xFEu;
	bucket_head = sprite_buckets_ADDRESS + ((uint16_t)(0xFFu - floor_depth) & 0xFFu) * 2u;
	OBJECT(current_object)->draw_next = mem_read16(bucket_head);
	mem_write16(bucket_head, current_object);
}

void get_previous_floor_bucket(uint32_t top_bucket, uint32_t *p_floor_depth, uint32_t *p_bucket)
{
	uint32_t bucket = 0, floor_depth = 0;

	floor_depth = (int32_t)((uint32_t)OBJECT(current_object)->projected_y + OBJECT(current_object)->previous_floor_z) >> 16;
	floor_depth = (floor_depth & ~0xFFFFu) | (uint16_t)(floor_depth - view_y);
	floor_depth = (floor_depth & ~0xFFFFu) | (uint16_t)(floor_depth + 0x10u);
	if ((int16_t)floor_depth < 0xFE)
	{
		if ((int16_t)floor_depth >= 0)
		{
			top_bucket = (top_bucket & ~0xFFFFu) | 0xFFu;
			bucket = (top_bucket & ~0xFFFFu) | (uint16_t)(top_bucket - floor_depth);
			*p_floor_depth = floor_depth;
			*p_bucket = bucket;
			return;
		}
		floor_depth = 0u;
		top_bucket = (top_bucket & ~0xFFFFu) | 0xFFu;
		bucket = (top_bucket & ~0xFFFFu) | (uint16_t)(top_bucket - floor_depth);
		*p_floor_depth = floor_depth;
		*p_bucket = bucket;
		return;
	}
	floor_depth = 0xFEu;
	top_bucket = (top_bucket & ~0xFFFFu) | 0xFFu;
	bucket = (top_bucket & ~0xFFFFu) | (uint16_t)(top_bucket - floor_depth);
	*p_floor_depth = floor_depth;
	*p_bucket = bucket;
}

void swap_sprite_buffers(void)
{
	uint32_t swap_entry = 0;
	uint8_t status = 0;

	if (sprite_list_ready == 0u)
		return;
	sprite_list_ready = 0u;
	sprite_buffer_flip += 2u;
	sprite_buffer_flip += 2u;
	swap_entry = sprite_buffer_swaps_ADDRESS + (6u & sprite_buffer_flip) * 2u;
	AS16(sprite_back_buffer) = ROM16(swap_entry);
	mem_write16(sprite_list_jump, ROM16(swap_entry + 2u));
	status = mem_read8(sprite_status);
}

void cancel_sprite_buffer_swap(void)
{
	sprite_list_ready = 0u;
}

void load_sized_frame_address_at_row(uint16_t row)
{
	uint16_t row_bytes = 0;
	uint32_t frame_record = 0;

	frame_record = (sprite_frames & ~0xFFFFu) | OBJECT(current_object)->sprite_frame;
	row_bytes = mem_read16(frame_record);
	frame_record += 2u;
	if ((OBJECT(current_object)->sprite_flags & 0x200u) == 0u)
		row_bytes = row_bytes >> 1;
	OBJECT(current_object)->sprite_scale = (uint32_t)(row_bytes >> 2) * row + mem_read16(frame_record + 4u);
}

void load_sized_frame_address(void)
{
	OBJECT(current_object)->sprite_scale = mem_read16(((sprite_frames & ~0xFFFFu) | OBJECT(current_object)->sprite_frame) + 6u);
}

void load_frame_size(void)
{
	uint32_t frame_record = 0;

	frame_record = (sprite_frames & ~0xFFFFu) | OBJECT(current_object)->sprite_frame;
	OBJECT(current_object)->sprite_width = mem_read16(frame_record);
	OBJECT(current_object)->sprite_height = mem_read8(frame_record + 2u);
}

void build_sprite_list(uint32_t *p_height, uint32_t *p_scale_x, uint32_t *p_scale_y, uint32_t *p_sprite_flags, uint32_t *p_sprite_entry)
{
	uint32_t height = *p_height;
	uint32_t scale_x = *p_scale_x;
	uint32_t scale_y = *p_scale_y;
	uint16_t sprite_flags = (uint16_t)*p_sprite_flags;
	uint8_t scale_shift = 0;
	uint16_t dest_height = 0, dest_width = 0, part_dest_width = 0, screen_x = 0, screen_y = 0, sized_width = 0, source_width_2 = 0;
	uint32_t r4 = 0, flags_word = 0, frame_data = 0, ignored = 0, object = 0, part_scale = 0, parts_left = 0, rounding = 0, source_width = 0, sprite_entry = 0, sprite_scale = 0;
	uint16_t part_width = 0, part_height = 0, width_bits = 0;

	sprite_entry = sprite_list_start;
	last_object = 0x20FC02u;
	frame_data = sprite_frames;
	rounding = 0x80u;
	scale_shift = 0xF8u;
	for (;;)
	{
		last_object = last_object - 2u;
		current_object = LINKED(current_object, OBJECT(last_object)->draw_next);
		if ((uint16_t)last_object == (uint16_t)current_object)
			continue;
		if ((uint16_t)last_object < (uint16_t)current_object)
		{
			*p_height = height;
			*p_scale_x = scale_x;
			*p_scale_y = scale_y;
			*p_sprite_flags = (*p_sprite_flags & 0xFFFF0000u) | sprite_flags;
			*p_sprite_entry = sprite_entry;
			return;
		}
		do
		{
			sprite_flags = OBJECT(current_object)->sprite_flags;
			flags_word = sprite_flags;
			flags_word = (uint16_t)((flags_word << 6) | (flags_word >> 10));
			switch (flags_word & 0x34u)
			{
			case 0x0u:
			case 0x37u:
				frame_data = (frame_data & ~0xFFFFu) | OBJECT(current_object)->sprite_frame;
				sprite_scale = OBJECT(current_object)->sprite_scale;
				source_width = mem_read16(frame_data);
				frame_data += 2u;
				height = (height & ~0xFFFFu) | mem_read16(frame_data);
				dest_width = (uint16_t)shift_by(source_width * sprite_scale + rounding, scale_shift, 32);
				dest_height = (uint16_t)shift_by((uint8_t)height * sprite_scale + rounding, scale_shift, 32);
				source_width = ((uint16_t)(source_width << 6) & ~0xFFu) | (uint8_t)height;
				height &= ~0xFFu;
				height = (height & ~0xFFFFu) | (uint16_t)(height | dest_height);
				frame_data += 2u;
			build_sprite_list_write_entry:
				dest_width |= mem_read16(frame_data);
				mem_write16(sprite_entry, sprite_flags);
				sprite_entry += 2u;
				mem_write16(sprite_entry, rotate_by(source_width, scale_shift, 16));
				sprite_entry += 2u;
				mem_write16(sprite_entry, height);
				sprite_entry += 2u;
				mem_write16(sprite_entry, dest_width);
				sprite_entry += 2u;
				mem_write32(sprite_entry, AS32(OBJECT(current_object)->screen_y));
				sprite_entry += 4u;
				frame_data += 2u;
				mem_write16(sprite_entry, mem_read16(frame_data));
				sprite_entry += 2u;
				mem_write16(sprite_entry, OBJECT(current_object)->sprite_palette);
				sprite_entry += 2u;
				break;
			case 0x3u:
			case 0x4u:
				frame_data = (frame_data & ~0xFFFFu) | OBJECT(current_object)->sprite_frame;
				source_width = mem_read16(frame_data);
				dest_width = (uint16_t)source_width;
				frame_data += 2u;
				height = (height & ~0xFFFFu) | mem_read16(frame_data);
				source_width = ((uint16_t)(source_width << 6) & ~0xFFu) | (uint8_t)height;
				sprite_flags &= 0x2FFFu;
				frame_data += 2u;
				goto build_sprite_list_write_entry;
			case 0x7u:
			case 0x8u:
			case 0x9u:
			case 0xAu:
			case 0xBu:
			case 0xCu:
			case 0xDu:
			case 0xEu:
			case 0xFu:
			case 0x10u:
			case 0x13u:
			case 0x14u:
				object = current_object + 2u;
				mem_write32(sprite_entry, AS32(OBJECT(object - 0x2u)->sprite_flags));
				sprite_entry += 4u;
				object += 4u;
				mem_write32(sprite_entry, AS32(OBJECT(object - 0x6u)->sprite_palette));
				sprite_entry += 4u;
				object += 4u;
				mem_write32(sprite_entry, AS32(OBJECT(object - 0xAu)->sprite_layer));
				sprite_entry += 4u;
				mem_write32(sprite_entry, AS32(OBJECT(object - 0xAu)->screen_x));
				sprite_entry += 4u;
				break;
			case 0x17u:
			case 0x18u:
			case 0x19u:
			case 0x1Au:
			case 0x1Bu:
			case 0x1Cu:
			case 0x1Du:
			case 0x1Eu:
			case 0x1Fu:
			case 0x20u:
				mem_write16(sprite_entry, (sprite_entry >> 4) - 0x7FFFu);
				sprite_entry += 2u;
				mem_write32(sprite_entry, AS32(OBJECT(current_object)->screen_y));
				sprite_entry += 0xEu;
				break;
			case 0x23u:
			case 0x24u:
				sprite_flags &= 0x2FFFu;
				frame_data = (frame_data & ~0xFFFFu) | OBJECT(current_object)->sprite_frame;
				source_width = mem_read16(frame_data);
				frame_data += 2u;
				height = (height & ~0xFFFFu) | mem_read16(frame_data);
				source_width = rotate_by(((uint16_t)(source_width << 6) & ~0xFFu) | (uint8_t)height, scale_shift, 16);
				height &= ~0xFFu;
				height = (height & ~0xFFFFu) | (uint16_t)(height | OBJECT(current_object)->sprite_height);
				frame_data += 2u;
				sized_width = (uint16_t)(OBJECT(current_object)->sprite_width | mem_read16(frame_data));
				mem_write16(sprite_entry, sprite_flags);
				sprite_entry += 2u;
				mem_write16(sprite_entry, source_width);
				sprite_entry += 2u;
				mem_write16(sprite_entry, height);
				sprite_entry += 2u;
				mem_write16(sprite_entry, sized_width);
				sprite_entry += 2u;
				mem_write32(sprite_entry, AS32(OBJECT(current_object)->screen_y));
				sprite_entry += 4u;
				mem_write16(sprite_entry, OBJECT(current_object)->sprite_scale);
				sprite_entry += 2u;
				mem_write16(sprite_entry, OBJECT(current_object)->sprite_palette);
				sprite_entry += 2u;
				frame_data += 2u;
				break;
			case 0x27u:
			case 0x28u:
			case 0x29u:
			case 0x2Au:
			case 0x2Bu:
			case 0x2Cu:
			case 0x2Du:
			case 0x2Eu:
			case 0x2Fu:
			case 0x30u:
				part_scale = OBJECT(current_object)->sprite_scale;
			build_sprite_list_write_parts:
				switch ((sprite_flags >> 5) & 6u)
				{
				case 0x0u:
					scale_x = part_scale;
				build_sprite_list_y_not_flipped:
					scale_y = part_scale;
					break;
				case 0x2u:
					scale_x = 0u - part_scale;
					goto build_sprite_list_y_not_flipped;
				case 0x4u:
					scale_x = part_scale;
					scale_y = 0u - part_scale;
					break;
				case 0x6u:
					scale_x = 0u - part_scale;
					scale_y = 0u - part_scale;
					break;
				default:
					unknown_code(0x9A3C4u + ((sprite_flags >> 5) & 6u));
				}
				sprite_flags &= 0xF0u;
				screen_x = (uint16_t)OBJECT(current_object)->screen_x;
				screen_y = (uint16_t)OBJECT(current_object)->screen_y;
				r14 = OBJECT(current_object)->part_list;
				parts_left = mem_read16(r14);
				r14 = r14 + 2u;
				if (parts_left >= 0x11u)
				{
					int_default(&flags_word, &source_width, &ignored, &r4);
					*p_height = r4;
					*p_scale_x = scale_x;
					*p_scale_y = scale_y;
					*p_sprite_flags = (*p_sprite_flags & 0xFFFF0000u) | sprite_flags;
					*p_sprite_entry = sprite_entry;
					return;
				}
				do
				{
					frame_data = (frame_data & ~0xFFFFu) | mem_read16(r14);
					part_width = mem_read16(frame_data);
					frame_data += 2u;
					source_width_2 = part_width;
					part_height = mem_read16(frame_data);
					frame_data += 2u;
					height = (height & ~0xFFFFu) | part_height;
					part_dest_width = (uint16_t)shift_by(source_width_2 * part_scale + rounding, scale_shift, 32);
					dest_height = (uint16_t)shift_by((uint8_t)height * part_scale + rounding, scale_shift, 32);
					source_width_2 = (uint16_t)(((uint16_t)((uint32_t)source_width_2 << 6) & ~0xFFu) | (uint8_t)height);
					height &= ~0xFFu;
					height = (height & ~0xFFFFu) | (uint16_t)(height | dest_height);
					width_bits = mem_read16(frame_data);
					frame_data += 2u;
					r14 = r14 + 2u;
					mem_write16(sprite_entry, mem_read16(r14) ^ sprite_flags);
					sprite_entry += 2u;
					mem_write16(sprite_entry, rotate_by(source_width_2, scale_shift, 16));
					sprite_entry += 2u;
					mem_write16(sprite_entry, height);
					sprite_entry += 2u;
					mem_write16(sprite_entry, part_dest_width | width_bits);
					sprite_entry += 2u;
					r14 = r14 + 2u;
					source_width = (int8_t)mem_read8(r14) * scale_x;
					r14 = r14 + 1u;
					mem_write16(sprite_entry, shift_signed_by((int8_t)mem_read8(r14) * scale_y, scale_shift, 32) + screen_y);
					sprite_entry += 2u;
					mem_write16(sprite_entry, shift_signed_by(source_width, scale_shift, 32) + screen_x);
					sprite_entry += 2u;
					mem_write16(sprite_entry, mem_read16(frame_data));
					sprite_entry += 2u;
					r14 = r14 + 1u;
					mem_write16(sprite_entry, mem_read16(r14));
					sprite_entry += 2u;
					parts_left--;
					r14 = r14 + 2u;
				} while (parts_left != 0u);
				break;
			case 0x33u:
			case 0x34u:
				part_scale = 0x100u;
				goto build_sprite_list_write_parts;
			default:
				unknown_code(0x9A2E0u + (flags_word & 0x34u));
			}
			current_object = LINKED(current_object, OBJECT(current_object)->draw_next);
		} while ((uint16_t)current_object != (uint16_t)last_object);
		OBJECT(last_object)->draw_next = last_object;
	}
	int_default(&flags_word, &source_width, &ignored, &r4);
	*p_height = r4;
	*p_scale_x = scale_x;
	*p_scale_y = scale_y;
	*p_sprite_flags = (*p_sprite_flags & 0xFFFF0000u) | sprite_flags;
	*p_sprite_entry = sprite_entry;
}

uint32_t load_default_palettes(void)
{
	uint32_t r11 = 0, ignored = 0, palette_list = 0;

	vblank_copies_pending = 0u;
	vblank_copy_count = 0u;
	palette_list = default_palette_list_ADDRESS;
	load_palette_list(palette_list, &ignored, &r11);
	return r11;
}

void load_palette_list(uint32_t palette_list, uint32_t *p_source, uint32_t *p_dest)
{
	uint32_t copy_mode = 0, dest = 0, entries_left = 0, size = 0, source = 0;

	entries_left = mem_read16(palette_list);
	dest = 0x600000u;
	palette_list += 2u;
	do
	{
		source = mem_read32(palette_list);
		palette_list += 4u;
		dest = (dest & ~0xFFFFu) | mem_read16(palette_list);
		palette_list += 2u;
		copy_mode = mem_read16(palette_list);
		palette_list += 2u;
		size = mem_read32(palette_list) - source;
		switch (copy_mode)
		{
		case 0x0u:
			copy_palettes_with_pen_0((uint16_t)copy_mode, size, &source, &dest);
			break;
		case 0x2u:
			copy_palette_block(size, &source, &dest);
			break;
		case 0x4u:
			repeat_palette_block(size, &source, &dest, &palette_list);
			break;
		case 0x6u:
			copy_palettes_with_pen_0((uint16_t)copy_mode, size, &source, &dest);
			break;
		default:
			unknown_code(0x9A4DFu + copy_mode);
		}
		entries_left--;
	} while (entries_left != 0u);
	*p_source = source;
	*p_dest = dest;
}

void copy_palettes_with_pen_0(uint16_t pen_0_color, uint32_t size, uint32_t *p_source, uint32_t *p_dest)
{
	uint32_t source = *p_source;
	uint32_t dest = *p_dest;
	uint16_t pen_15 = 0;
	uint32_t pens_1_2 = 0, pens_3_4 = 0, pens_5_6 = 0, pens_7_8 = 0, pens_9_10 = 0, pens_11_12 = 0, pens_13_14 = 0, halfwords_left = 0;

	size = size >> 1;
	do
	{
		mem_write16(dest, pen_0_color);
		pens_1_2 = mem_read32(source);
		source += 4u;
		dest += 2u;
		mem_write32(dest, pens_1_2);
		pens_3_4 = mem_read32(source);
		source += 4u;
		dest += 4u;
		mem_write32(dest, pens_3_4);
		pens_5_6 = mem_read32(source);
		source += 4u;
		dest += 4u;
		mem_write32(dest, pens_5_6);
		pens_7_8 = mem_read32(source);
		source += 4u;
		dest += 4u;
		mem_write32(dest, pens_7_8);
		pens_9_10 = mem_read32(source);
		source += 4u;
		dest += 4u;
		mem_write32(dest, pens_9_10);
		pens_11_12 = mem_read32(source);
		source += 4u;
		dest += 4u;
		mem_write32(dest, pens_11_12);
		pens_13_14 = mem_read32(source);
		source += 4u;
		dest += 4u;
		mem_write32(dest, pens_13_14);
		pen_15 = mem_read16(source);
		source += 2u;
		dest += 4u;
		mem_write16(dest, pen_15);
		halfwords_left = size;
		size = halfwords_left - 0xFu;
		dest += 2u;
	} while ((int32_t)halfwords_left > 0xF);
	*p_source = source;
	*p_dest = dest;
}

void copy_palette_block(uint32_t size, uint32_t *p_source, uint32_t *p_dest)
{
	uint32_t source = *p_source;
	uint32_t dest = *p_dest;
	uint32_t dest_end = 0, source_end = 0;

	size = size >> 1;
	move_string(source, size, dest, size, 2u, 0u, 0u, fill_value, &dest_end, &source_end);
	source = source_end;
	dest = dest_end;
	*p_source = source;
	*p_dest = dest;
}

void repeat_palette_block(uint32_t size, uint32_t *p_source, uint32_t *p_dest, uint32_t *p_palette_list)
{
	uint32_t source = *p_source;
	uint32_t dest = *p_dest;
	uint32_t palette_list = *p_palette_list;
	uint32_t dest_end = 0, repeat_counts = 0, repeats_left = 0, source_end = 0;

	repeat_counts = source;
	source = mem_read32(palette_list);
	size = size >> 1;
	palette_list += 4u;
	do
	{
		repeats_left = mem_read16(repeat_counts);
		repeat_counts += 2u;
		do
		{
			move_string(source, 0x10u, dest, 0x10u, 2u, 0u, 0u, fill_value, &dest_end, &source_end);
			dest = dest_end;
			repeats_left--;
		} while (repeats_left != 0u);
		source = source_end;
		size--;
	} while (size != 0u);
	*p_source = source;
	*p_dest = dest;
	*p_palette_list = palette_list;
}

void queue_vblank_copy(uint32_t source, uint32_t dest, uint16_t halfwords, uint32_t *p_copy_offset, uint32_t *p_copy_entry)
{
	uint32_t copy_entry = 0, copy_offset = 0;

	vblank_copies_pending = 0u;
	copy_offset = vblank_copy_count;
	vblank_copy_count++;
	copy_offset = copy_offset * 5u;
	copy_entry = vblank_copies_ADDRESS + copy_offset * 2u;
	mem_write32(copy_entry, source);
	copy_entry += 4u;
	mem_write32(copy_entry, dest);
	copy_entry += 4u;
	mem_write16(copy_entry, halfwords);
	vblank_copies_pending = 1u;
	copy_entry += 2u;
	*p_copy_offset = copy_offset;
	*p_copy_entry = copy_entry;
}

uint32_t init_sound(void)
{
	uint32_t sound_hold = 0;
	uint8_t cnt_bits = 0, port_d_bits = 0;

	sound_queue_read = 0u;
	sound_queue_write = 1u;
	sound_queue[0] = 0x96u;
	sound_queue[1] = 0u;
	cnt_bits = mem_read8(io_cnt);
	mem_write8(io_cnt, cnt_bits | 4u);
	port_d_bits = mem_read8(io_port_d);
	mem_write8(io_port_d, port_d_bits & ~8u);
	sound_hold = 0u;
	mem_write8(sound_cpu_hold, 0u);
	return sound_hold;
}

void set_port_d_bit_3_if_sound_allowed(void)
{
	uint8_t port_d_bits = 0;

	if (unused_coin_byte != 0u || game_in_progress != 0u || setting_advertise_sound != 0u)
	{
		port_d_bits = mem_read8(io_port_d);
		mem_write8(io_port_d, port_d_bits | 8u);
	}
}

void queue_coin_sound(void)
{
	uint8_t sound_id = 0;
	uint32_t ignored = 0;
	uint8_t port_d_bits = 0;

	port_d_bits = mem_read8(io_port_d);
	mem_write8(io_port_d, port_d_bits | 8u);
	sound_id = 0x7Fu;
	queue_sound(sound_id, &ignored, &ignored);
}

void play_sound(uint8_t sound_id, uint32_t *p_r1, uint32_t *p_r2)
{
	uint32_t r1 = *p_r1;
	uint32_t r2 = *p_r2;
	uint32_t r1_2 = 0, r2_2 = 0, mask_bit = 0, mask_index = 0;

	if (game_in_progress != 0u)
	{
		queue_sound(sound_id, &r1_2, &r2_2);
		*p_r1 = r1_2;
		*p_r2 = r2_2;
		return;
	}
	if (setting_advertise_sound == 0u)
		return;
	mask_index = (uint8_t)sound_id;
	mask_bit = mask_index & 0x1Fu;
	mask_index = mask_index >> 5;
	if ((attract_sound_mask[mask_index] & shl32(1u, (int)(mask_bit))) == 0u)
	{
		*p_r1 = mask_bit;
		*p_r2 = mask_index;
		return;
	}
	queue_sound(sound_id, &r1_2, &r2_2);
	*p_r1 = r1_2;
	*p_r2 = r2_2;
}

void queue_sound(uint8_t sound_id, uint32_t *p_r1, uint32_t *p_r2)
{
	uint32_t write_index = 0, write_index_address = 0;

	write_index = sound_queue_write;
	sound_queue[write_index] = sound_id;
	write_index = (uint8_t)(write_index + 1u);
	write_index = write_index & 0x1Fu;
	write_index_address = sound_queue_write_ADDRESS;
	sound_queue_write = write_index;
	*p_r1 = write_index;
	*p_r2 = write_index_address;
}

void load_stage_palette(uint32_t *p_palette_record, uint32_t *p_source, uint32_t *p_dest)
{
	uint32_t dest = 0, halfword_count = 0, palette_record = 0, source = 0;

	palette_record = stage_palettes_ADDRESS + ((uint32_t)stage << 3);
	source = ROM32(palette_record);
	palette_record += 4u;
	halfword_count = ROM32(palette_record);
	dest = PALETTE_RAM + 0xB800u;
	move_halfwords(PALETTE_RAM + 0xB800u, source, halfword_count);
	*p_palette_record = palette_record;
	*p_source = source;
	*p_dest = dest;
}

void split_frame_count(uint32_t frames, uint32_t frames_high, uint32_t *p_seconds, uint32_t *p_minutes, uint32_t *p_hours, uint32_t *p_days)
{
	uint32_t days = 0, hours = 0, hours_dividend = 0, hours_dividend_high = 0, minutes = 0, minutes_dividend = 0, minutes_dividend_high = 0, seconds = 0, seconds_dividend = 0, seconds_dividend_high = 0;

	divx_pair(0x4E9FA0u, &frames, &frames_high);
	days = frames;
	hours_dividend = frames_high;
	hours_dividend_high = 0u;
	divx_pair(0x346A7u, &hours_dividend, &hours_dividend_high);
	hours = hours_dividend;
	minutes_dividend = hours_dividend_high;
	minutes_dividend_high = 0u;
	divx_pair(0xDFBu, &minutes_dividend, &minutes_dividend_high);
	minutes = minutes_dividend;
	seconds_dividend = minutes_dividend_high;
	seconds_dividend_high = 0u;
	divx_pair(0x3Cu, &seconds_dividend, &seconds_dividend_high);
	seconds = seconds_dividend;
	*p_seconds = seconds;
	*p_minutes = minutes;
	*p_hours = hours;
	*p_days = days;
}

void clear_bitmap_layer(uint8_t is_8bpp, uint32_t *p_top, uint32_t *p_color, uint32_t *p_r5, uint32_t *p_r8, uint32_t *p_blocks_per_row, uint32_t *p_r10)
{
	uint32_t color = *p_color;
	uint32_t r5 = *p_r5;
	uint32_t r8 = *p_r8;
	uint32_t blocks_per_row = *p_blocks_per_row;
	uint32_t r10 = *p_r10;
	uint8_t is_8bpp_2 = 0;
	uint16_t bottom = 0, right = 0, scroll_x = 0, scroll_y = 0, window_top = 0, x = 0, y = 0;
	uint32_t clip_right_bottom = 0, top = 0, video_base = 0;

	is_8bpp_2 = (uint8_t)is_8bpp != 0u;
	scroll_x = 0u;
	scroll_y = 0u;
	set_bitmap_scroll(scroll_x, scroll_y, is_8bpp_2);
	x = 0u;
	y = 0u;
	get_bitmap_address(x, y);
	video_base = video_control;
	mem_write16(video_layer_disable, mem_read16(video_layer_disable) & 0x7BDFu);
	mem_write32(video_bitmap_clip_left, 0u);
	clip_right_bottom = 0xDF013Fu;
	if ((int16_t)mem_read16(video_control) < 0)
		clip_right_bottom = (clip_right_bottom & ~0xFFFFu) | 0x19Fu;
	mem_write32(video_base + 0x84u, clip_right_bottom);
	clip_right_bottom &= ~0xFFFFu;
	window_top = 0u;
	right = 0x1FFu;
	bottom = 0xDFu;
	set_bitmap_window((uint16_t)clip_right_bottom, window_top, right, bottom);
	clip_right_bottom |= (uint16_t)bitmap_window_left;
	top = (uint16_t)bitmap_window_top;
	right = (uint16_t)bitmap_window_right;
	bottom = (uint16_t)bitmap_window_bottom;
	color &= ~0xFFu;
	fill_rect_unclipped(right, bottom, &clip_right_bottom, &top, &color, &r5, &r8, &blocks_per_row, &r10);
	*p_top = top;
	*p_color = color;
	*p_r5 = r5;
	*p_r8 = r8;
	*p_blocks_per_row = blocks_per_row;
	*p_r10 = r10;
}

void get_bitmap_address(uint16_t x, uint16_t y)
{
	uint8_t x_shift = 0;

	x_shift = (uint8_t)(0xFFu + bitmap_8bpp);
	bitmap_address = 0x300000u + (shift_signed_by((uint32_t)(int16_t)x, x_shift, 32) + shift_by((uint32_t)(int16_t)y, (uint8_t)(x_shift + 9u), 32));
}

void set_bitmap_scroll(uint16_t scroll_x, uint16_t scroll_y, uint8_t is_8bpp)
{
	mem_write16(video_bitmap_scroll_x, scroll_x);
	mem_write16(video_bitmap_scroll_y, scroll_y);
	is_8bpp &= 1u;
	bitmap_8bpp = is_8bpp;
	mem_write16(video_control, mem_read16(video_control) & ~0x800u);
	mem_write16(video_control, mem_read16(video_control) | ((uint32_t)is_8bpp << 11));
}

void set_bitmap_window(uint16_t left, uint16_t top, uint16_t right, uint16_t bottom)
{
	uint16_t old_left = 0, old_top = 0;

	if ((int16_t)right < (int16_t)left)
	{
		old_left = left;
		left = (uint16_t)right;
		right = old_left;
	}
	if ((int16_t)bottom < (int16_t)top)
	{
		old_top = top;
		top = (uint16_t)bottom;
		bottom = old_top;
	}
	bitmap_window_left = left;
	bitmap_window_top = top;
	bitmap_window_right = right;
	bitmap_window_bottom = bottom;
}

void set_bitmap_clip(uint16_t left, uint16_t top, uint16_t right, uint16_t bottom)
{
	uint16_t old_left = 0, old_top = 0;

	if ((int16_t)right < (int16_t)left)
	{
		old_left = left;
		left = (uint16_t)right;
		right = old_left;
	}
	if ((int16_t)bottom < (int16_t)top)
	{
		old_top = top;
		top = (uint16_t)bottom;
		bottom = old_top;
	}
	mem_write16(video_layer_disable, mem_read16(video_layer_disable) | 0x8000u);
	mem_write16(video_bitmap_clip_left, left);
	mem_write16(video_bitmap_clip_top, top);
	mem_write16(video_bitmap_clip_right, right);
	mem_write16(video_bitmap_clip_bottom, bottom);
}

void draw_pixel(uint32_t x, uint32_t y, uint8_t color)
{
	uint8_t keep_mask = 0, saved_x = 0;
	uint32_t outcode = 0;

	if (bitmap_8bpp != 0u)
	{
		get_point_outcode(&x, &y, &outcode);
		if ((uint8_t)outcode != 0u)
			return;
		mem_write8((y << 9) + x + bitmap_address, color);
		return;
	}
	get_point_outcode(&x, &y, &outcode);
	if ((uint8_t)outcode != 0u)
		return;
	saved_x = (uint8_t)x;
	y = (y << 8) + ((int32_t)x >> 1) + bitmap_address;
	keep_mask = 0xF0u;
	color &= 0xFu;
	if ((saved_x & 1u) != 0u)
	{
		keep_mask = 0xFu;
		color = (uint8_t)((uint32_t)color << 4);
	}
	mem_write8(y, (keep_mask & mem_read8(y)) | color);
}

uint32_t read_pixel(uint32_t x, uint32_t y)
{
	uint8_t saved_x = 0;
	uint32_t r0 = 0, outcode = 0, pixel = 0;

	if (bitmap_8bpp != 0u)
	{
		get_point_outcode(&x, &y, &outcode);
		if ((uint8_t)outcode != 0u)
		{
			r0 = 0xFFFFFFFFu;
			return r0;
		}
		pixel = mem_read8((y << 9) + x + bitmap_address);
		return pixel;
	}
	get_point_outcode(&x, &y, &outcode);
	if ((uint8_t)outcode != 0u)
	{
		r0 = 0xFFFFFFFFu;
		return r0;
	}
	saved_x = (uint8_t)x;
	pixel = mem_read8((y << 8) + ((int32_t)x >> 1) + bitmap_address);
	if ((saved_x & 1u) != 0u)
		pixel = (uint8_t)((pixel << 4) | (pixel >> 4));
	pixel &= 0xFu;
	return pixel;
}

void draw_line(uint32_t end_x, uint32_t end_y, uint32_t *p_start_x, uint32_t *p_start_y, uint32_t *p_color, uint32_t *p_r8)
{
	uint32_t start_x = *p_start_x;
	uint32_t start_y = *p_start_y;
	uint32_t color = *p_color;
	uint32_t r8 = *p_r8;
	uint32_t zero = 0, negative = 0, overflow = 0, carry = 0;
	uint8_t high_nibble_mask = 0, low_nibble_color = 0;
	uint16_t fraction_4bpp = 0, fraction_8bpp = 0;
	uint32_t direction_index = 0, end_outcode = 0, end_x_copy = 0, end_y_copy = 0, end_y_copy_8bpp = 0, ignored = 0, major_step = 0, minor_step = 0, outcode = 0, row_step_4bpp = 0, slope = 0, span_end_copy = 0, span_length = 0, steep_slope = 0, step_record = 0, steps_left = 0, top_row = 0;
	uint16_t end_y_4bpp = 0, dy_4bpp = 0, end_x_4bpp = 0, dx_4bpp = 0, end_y_8bpp = 0, dy_8bpp = 0, span_end_x = 0, end_x_8bpp = 0, dx_8bpp = 0;
	uint32_t pixels_left = 0, saved_step = 0, saved_dy = 0;

	for (;;)
	{
		get_window_outcode(&start_x, &start_y, &end_x, &end_y, &outcode);
		end_outcode = outcode;
		get_window_outcode(&start_x, &start_y, &end_x, &end_y, &outcode);
		if ((uint16_t)(outcode & end_outcode) != 0u)
		{
			start_x = 0xFFFFFFFFu;
			*p_start_x = start_x;
			*p_start_y = start_y;
			*p_r8 = r8;
			return;
		}
		if ((uint16_t)(outcode | end_outcode) == 0u)
			break;
		clip_line_to_window(outcode, end_outcode, &start_x, &start_y, &end_x, &end_y, &r8);
	}
	if (bitmap_8bpp == 0u)
	{
		row_step_4bpp = 0x100u;
		end_y_copy = (uint16_t)end_y;
		end_y_4bpp = end_y_copy;
		dy_4bpp = end_y_4bpp - start_y;
		overflow = (uint32_t)(int16_t)end_y_4bpp - (int16_t)start_y != (uint32_t)(int16_t)(end_y_4bpp - start_y);
		zero = dy_4bpp == 0u;
		negative = dy_4bpp >> 15;
		slope = dy_4bpp;
		if (end_y_4bpp == (uint16_t)start_y)
		{
			fill_span_4bpp((uint16_t)end_x, &start_x, &start_y, &color, &ignored);
			*p_start_x = start_x;
			*p_start_y = start_y;
			*p_color = color;
			*p_r8 = r8;
			return;
		}
		if ((negative ^ overflow) | zero)
		{
			row_step_4bpp = 0u - row_step_4bpp;
			slope = (uint16_t)(0u - slope);
		}
		end_x_copy = (uint16_t)end_x;
		end_x_4bpp = end_x_copy;
		dx_4bpp = end_x_4bpp - start_x;
		overflow = (uint32_t)(int16_t)end_x_4bpp - (int16_t)start_x != (uint32_t)(int16_t)(end_x_4bpp - start_x);
		zero = dx_4bpp == 0u;
		negative = dx_4bpp >> 15;
		steep_slope = dx_4bpp;
		if (end_x_4bpp == (uint16_t)start_x)
		{
			draw_vertical_span_4bpp((uint16_t)start_x, row_step_4bpp, &start_y, &color, &slope);
			*p_start_x = start_x;
			*p_start_y = start_y;
			*p_color = color;
			*p_r8 = r8;
			return;
		}
		if ((negative ^ overflow) | zero)
		{
			steep_slope = (uint16_t)(0u - steep_slope);
			start_x = (start_x & ~0xFFFFu) | (uint16_t)end_x;
			start_y = (uint16_t)end_y;
			row_step_4bpp = 0u - row_step_4bpp;
		}
		start_y = ((uint32_t)(uint16_t)start_y << 8) + ((uint16_t)start_x >> 1) + bitmap_address;
		color = (color & ~0xFFu) | (color & 0xFu);
		low_nibble_color = (uint8_t)color;
		color = (color & ~0xFFu) | (uint8_t)(color << 4);
		high_nibble_mask = 0xF0u;
		slope = (uint16_t)(slope + 1u);
		steep_slope = (uint16_t)(steep_slope + 1u);
		if (steep_slope < slope)
		{
			steep_slope = divide_u32(steep_slope << 16, slope);
			fraction_4bpp = 0u;
			if ((start_x & 1u) != 0u)
			{
				do
				{
				draw_line_steep_odd_pixel:
					start_x = (start_x & ~0xFFu) | mem_read8(start_y);
					start_x = (start_x & ~0xFFu) | (start_x & 0xFu);
					start_x = (start_x & ~0xFFu) | (uint8_t)(start_x | color);
					mem_write8(start_y, start_x);
					start_y += row_step_4bpp;
					carry = (uint16_t)(fraction_4bpp + steep_slope) < fraction_4bpp;
					fraction_4bpp += steep_slope;
					slope--;
				} while ((slope != 0u) & (carry == 0u));
				start_y++;
				if (slope == 0u)
				{
					*p_start_x = start_x;
					*p_start_y = start_y;
					*p_color = color;
					*p_r8 = r8;
					return;
				}
			}
			do
			{
				start_x = (start_x & ~0xFFu) | mem_read8(start_y);
				start_x = (start_x & ~0xFFu) | (start_x & high_nibble_mask);
				start_x = (start_x & ~0xFFu) | (uint8_t)(start_x | low_nibble_color);
				mem_write8(start_y, start_x);
				start_y += row_step_4bpp;
				carry = (uint16_t)(fraction_4bpp + steep_slope) < fraction_4bpp;
				fraction_4bpp += steep_slope;
				slope--;
			} while ((slope != 0u) & (carry == 0u));
			if (slope == 0u)
			{
				*p_start_x = start_x;
				*p_start_y = start_y;
				*p_color = color;
				*p_r8 = r8;
				return;
			}
			goto draw_line_steep_odd_pixel;
		}
		if (steep_slope == slope)
		{
			if ((start_x & 1u) != 0u)
			{
			draw_line_diagonal_odd_pixel:
				start_x = (start_x & ~0xFFu) | mem_read8(start_y);
				start_x = (start_x & ~0xFFu) | (start_x & 0xFu);
				start_x = (start_x & ~0xFFu) | (uint8_t)(start_x | color);
				mem_write8(start_y, start_x);
				start_y = start_y + 1u + row_step_4bpp;
				steep_slope--;
				if (steep_slope == 0u)
				{
					*p_start_x = start_x;
					*p_start_y = start_y;
					*p_color = color;
					*p_r8 = r8;
					return;
				}
			}
			start_x = (start_x & ~0xFFu) | mem_read8(start_y);
			start_x = (start_x & ~0xFFu) | (start_x & high_nibble_mask);
			start_x = (start_x & ~0xFFu) | (uint8_t)(start_x | low_nibble_color);
			mem_write8(start_y, start_x);
			start_y += row_step_4bpp;
			pixels_left = steep_slope;
			steep_slope = pixels_left - 1u;
			if (pixels_left == 1u)
			{
				*p_start_x = start_x;
				*p_start_y = start_y;
				*p_color = color;
				*p_r8 = r8;
				return;
			}
			goto draw_line_diagonal_odd_pixel;
		}
		slope = divide_u32(slope << 16, steep_slope);
		fraction_4bpp = 0u;
		if ((start_x & 1u) != 0u)
		{
		draw_line_shallow_odd_pixel:
			start_x = (start_x & ~0xFFu) | mem_read8(start_y);
		draw_line_shallow_set_high_nibble:
			start_x = (start_x & ~0xFFu) | (start_x & 0xFu);
			start_x = (start_x & ~0xFFu) | (uint8_t)(start_x | color);
			mem_write8(start_y, start_x);
			carry = (uint16_t)(fraction_4bpp + slope) < fraction_4bpp;
			fraction_4bpp += slope;
			steep_slope--;
			start_y++;
			if ((steep_slope != 0u) & (carry == 0u))
			{
			draw_line_shallow_even_pixel:
				start_x = (start_x & ~0xFFu) | mem_read8(start_y);
				start_x = (start_x & ~0xFFu) | (start_x & high_nibble_mask);
				start_x = (start_x & ~0xFFu) | (uint8_t)(start_x | low_nibble_color);
				carry = (uint16_t)(fraction_4bpp + slope) < fraction_4bpp;
				fraction_4bpp += slope;
				steep_slope--;
				if ((steep_slope != 0u) & (carry == 0u))
					goto draw_line_shallow_set_high_nibble;
				mem_write8(start_y, start_x);
				if (steep_slope == 0u)
				{
					*p_start_x = start_x;
					*p_start_y = start_y;
					*p_color = color;
					*p_r8 = r8;
					return;
				}
				start_y += row_step_4bpp;
				goto draw_line_shallow_odd_pixel;
			}
			start_y += row_step_4bpp;
			if (steep_slope == 0u)
			{
				*p_start_x = start_x;
				*p_start_y = start_y;
				*p_color = color;
				*p_r8 = r8;
				return;
			}
			goto draw_line_shallow_even_pixel;
		}
		goto draw_line_shallow_even_pixel;
	}
	direction_index = 0u;
	end_y_copy_8bpp = end_y;
	end_y_8bpp = end_y_copy_8bpp;
	dy_8bpp = end_y_8bpp - start_y;
	overflow = (uint32_t)(int16_t)end_y_8bpp - (int16_t)start_y != (uint32_t)(int16_t)(end_y_8bpp - start_y);
	zero = dy_8bpp == 0u;
	negative = dy_8bpp >> 15;
	slope = (end_y_copy_8bpp & ~0xFFFFu) | dy_8bpp;
	if (end_y_8bpp == (uint16_t)start_y)
	{
		start_y = ((uint32_t)(uint16_t)start_y << 9) + start_x + bitmap_address;
		span_end_copy = (uint16_t)end_x;
		span_end_x = span_end_copy;
		span_length = (uint16_t)(span_end_x - start_x);
		if ((int16_t)start_x <= (int16_t)span_end_x)
		{
			fill_span_8bpp(span_length, &start_x, &start_y, &color);
			*p_start_x = start_x;
			*p_start_y = start_y;
			*p_color = color;
			*p_r8 = r8;
			return;
		}
		span_length = (uint16_t)(0u - span_length);
		start_y -= span_length;
		fill_span_8bpp(span_length, &start_x, &start_y, &color);
		*p_start_x = start_x;
		*p_start_y = start_y;
		*p_color = color;
		*p_r8 = r8;
		return;
	}
	if ((negative ^ overflow) | zero)
	{
		slope = (slope & ~0xFFFFu) | (uint16_t)(0u - slope);
		direction_index = (uint8_t)(direction_index + 1u);
	}
	steps_left = end_x;
	end_x_8bpp = steps_left;
	dx_8bpp = end_x_8bpp - start_x;
	overflow = (uint32_t)(int16_t)end_x_8bpp - (int16_t)start_x != (uint32_t)(int16_t)(end_x_8bpp - start_x);
	zero = dx_8bpp == 0u;
	negative = dx_8bpp >> 15;
	steps_left = (steps_left & ~0xFFFFu) | dx_8bpp;
	if (end_x_8bpp == (uint16_t)start_x)
	{
		if (direction_index == 0u)
		{
			start_y = (uint32_t)(uint16_t)start_y << 9;
			start_y = ((start_y & ~0xFFFFu) | (uint16_t)(start_y + start_x)) + bitmap_address;
			start_y = draw_vertical_line_8bpp(start_y, (uint8_t)color, slope);
			*p_start_x = start_x;
			*p_start_y = start_y;
			*p_color = color;
			*p_r8 = r8;
			return;
		}
		top_row = end_y;
		top_row = (uint32_t)(uint16_t)top_row << 9;
		top_row = ((top_row & ~0xFFFFu) | (uint16_t)(top_row + start_x)) + bitmap_address;
		top_row = draw_vertical_line_8bpp(top_row, (uint8_t)color, slope);
		*p_start_x = start_x;
		*p_start_y = top_row;
		*p_color = color;
		*p_r8 = r8;
		return;
	}
	if ((negative ^ overflow) | zero)
	{
		steps_left = (steps_left & ~0xFFFFu) | (uint16_t)(0u - steps_left);
		direction_index = (uint8_t)(direction_index + 2u);
	}
	start_y = ((uint32_t)(int16_t)start_y << 9) + start_x + bitmap_address;
	step_record = line_direction_steps_ADDRESS + direction_index * 4u;
	minor_step = (int16_t)ROM16(step_record);
	major_step = (int16_t)ROM16(step_record + 2u);
	if ((uint16_t)steps_left == (uint16_t)slope)
	{
		minor_step += major_step;
		slope = (slope & ~0xFFFFu) | (uint16_t)(slope + 1u);
		carry = slope & 1u;
		slope = slope >> 1;
		if (carry)
			slope = (slope & ~0xFFFFu) | (uint16_t)(slope + 1u);
		do
		{
			mem_write8(start_y, color);
			start_y += minor_step;
			mem_write8(start_y, color);
			start_y += minor_step;
			slope--;
		} while (slope != 0u);
		start_x = (uint16_t)slope;
		*p_start_x = start_x;
		*p_start_y = start_y;
		*p_r8 = r8;
		return;
	}
	if ((uint16_t)steps_left < (uint16_t)slope)
	{
		saved_step = major_step;
		major_step = minor_step;
		minor_step = saved_step;
		saved_dy = slope;
		slope = steps_left;
		steps_left = saved_dy;
	}
	steps_left = (steps_left & ~0xFFFFu) | (uint16_t)(steps_left + 1u);
	slope = divide_s32((uint32_t)(uint16_t)(slope + 1u) << 16, steps_left);
	fraction_8bpp = 0u;
	for (;;)
	{
		mem_write8(start_y, color);
		start_y += major_step;
		carry = (uint16_t)(fraction_8bpp + slope) < fraction_8bpp;
		fraction_8bpp += slope;
		steps_left--;
		if ((steps_left != 0u) & (carry == 0u))
			continue;
		start_y += minor_step;
		if (steps_left != 0u)
			continue;
		break;
	}
	start_x = (uint16_t)steps_left;
	*p_start_x = start_x;
	*p_start_y = start_y;
	*p_r8 = r8;
}

void fill_span_8bpp(uint32_t length, uint32_t *p_blocks, uint32_t *p_pixel_address, uint32_t *p_color)
{
	uint32_t blocks = *p_blocks;
	uint32_t pixel_address = *p_pixel_address;
	uint32_t color = *p_color;
	uint32_t zero = 0;
	uint8_t tail_count = 0;
	uint32_t length_left = 0, block_count = 0, tail_pixels = 0;

	if ((pixel_address & 1u) != 0u)
	{
		mem_write8(pixel_address, color);
		length_left = length - 1u;
		length = length_left;
		pixel_address++;
		if ((int32_t)length_left < 0)
		{
			pixel_address = (pixel_address & ~0xFFFFu) | (uint16_t)(pixel_address - 1u);
			*p_pixel_address = pixel_address;
			return;
		}
	}
	if (length != 0u)
	{
		color = (uint8_t)color * 0x1010101u;
		length++;
		block_count = length >> 4;
		blocks = block_count;
		if (block_count != 0u)
		{
			do
			{
				mem_write32(pixel_address, color);
				pixel_address += 4u;
				mem_write32(pixel_address, color);
				pixel_address += 4u;
				mem_write32(pixel_address, color);
				pixel_address += 4u;
				mem_write32(pixel_address, color);
				blocks--;
				pixel_address += 4u;
			} while (blocks != 0u);
		}
		length &= 0xFu;
		if (length >= 8u)
		{
			mem_write32(pixel_address, color);
			pixel_address += 4u;
			mem_write32(pixel_address, color);
			length -= 8u;
			pixel_address += 4u;
		}
		if (length >= 4u)
		{
			mem_write32(pixel_address, color);
			pixel_address += 4u;
		}
		tail_pixels = length & 3u;
		tail_count = (uint8_t)tail_pixels;
		if (tail_pixels == 0u)
		{
			pixel_address = (pixel_address & ~0xFFFFu) | (uint16_t)(pixel_address - 1u);
			*p_blocks = blocks;
			*p_pixel_address = pixel_address;
			*p_color = color;
			return;
		}
		zero = (uint8_t)(tail_count - 2u) == 0u;
		if (tail_count >= 2u)
		{
			mem_write16(pixel_address, color);
			pixel_address += 2u;
			if (zero)
			{
				pixel_address = (pixel_address & ~0xFFFFu) | (uint16_t)(pixel_address - 1u);
				*p_blocks = blocks;
				*p_pixel_address = pixel_address;
				*p_color = color;
				return;
			}
		}
	}
	mem_write8(pixel_address, color);
	pixel_address++;
	pixel_address = (pixel_address & ~0xFFFFu) | (uint16_t)(pixel_address - 1u);
	*p_blocks = blocks;
	*p_pixel_address = pixel_address;
	*p_color = color;
}

uint32_t draw_vertical_line_8bpp(uint32_t pixel_address, uint8_t color, uint32_t length)
{
	uint32_t row_step = 0;

	row_step = 0x200u;
	draw_vertical_span_8bpp(row_step, color, &pixel_address, &length);
	return pixel_address;
}

void draw_vertical_span_8bpp(uint32_t row_step, uint8_t color, uint32_t *p_pixel_address, uint32_t *p_length)
{
	uint32_t pixel_address = *p_pixel_address;
	uint32_t length = *p_length;
	uint32_t carry = 0;

	length = (length & ~0xFFFFu) | (uint16_t)(length + 1u);
	carry = length & 1u;
	length = (length & ~0xFFFFu) | ((uint16_t)length >> 1);
	if (carry == 0u)
	{
		mem_write8(pixel_address, color);
		pixel_address += row_step;
	}
	else
		length = (length & ~0xFFFFu) | (uint16_t)(length + 1u);
	for (;;)
	{
		mem_write8(pixel_address, color);
		pixel_address += row_step;
		length--;
		if (length == 0u)
			break;
		mem_write8(pixel_address, color);
		pixel_address += row_step;
	}
	pixel_address -= row_step;
	*p_pixel_address = pixel_address;
	*p_length = length;
}

void fill_span_4bpp(uint16_t end_x, uint32_t *p_start_x, uint32_t *p_start_y, uint32_t *p_color, uint32_t *p_length)
{
	uint32_t start_x = *p_start_x;
	uint32_t start_y = *p_start_y;
	uint32_t color = *p_color;
	uint32_t carry = 0;
	uint32_t end_x_copy = 0, keep_mask = 0, length = 0, longs_left = 0;
	uint16_t end_column = 0;
	uint32_t length_left = 0, tail_pixels = 0, pixels_before_byte = 0, pixels_before_second = 0;

	end_x_copy = (uint16_t)end_x;
	end_column = end_x_copy;
	length = (uint16_t)(end_column - start_x);
	if ((int16_t)end_column < (int16_t)start_x)
	{
		start_x = (start_x & ~0xFFFFu) | (uint16_t)end_x;
		length = (uint16_t)(0u - length);
	}
	color = (color & ~0xFFu) | (color & 0xFu);
	start_y = ((uint32_t)(uint16_t)start_y << 8) + bitmap_address;
	carry = start_x & 1u;
	start_x = (start_x & ~0xFFFFu) | ((uint16_t)start_x >> 1);
	if (carry)
	{
		start_y = (start_y & ~0xFFFFu) | (uint16_t)(start_y + start_x);
		start_x = (start_x & ~0xFFu) | mem_read8(start_y);
		start_x = (start_x & ~0xFFu) | (start_x & 0xFu);
		start_x = (start_x & ~0xFFu) | (uint8_t)(start_x | (color << 4));
		mem_write8(start_y, start_x);
		length_left = length - 1u;
		length = length_left;
		start_y++;
		if ((int32_t)length_left < 0)
		{
			*p_start_x = start_x;
			*p_start_y = start_y;
			*p_color = color;
			*p_length = length;
			return;
		}
		start_x = 0u;
	}
	start_y = (start_y & ~0xFFFFu) | (uint16_t)(start_y + start_x);
	length++;
	if ((start_y & 1u) == 0u)
	{
		color = color * 0x11111111u;
		if ((uint16_t)length >= 8u)
		{
			longs_left = length >> 3;
			do
			{
				mem_write32(start_y, color);
				start_y += 4u;
				longs_left--;
			} while (longs_left != 0u);
			tail_pixels = length & 7u;
			length = tail_pixels;
			if (tail_pixels == 0u)
			{
				*p_start_x = start_x;
				*p_start_y = start_y;
				*p_color = color;
				*p_length = length;
				return;
			}
		}
		length = (length & ~0xFFu) | (uint8_t)(length * 2u);
		length = (length & ~0xFFu) | (uint8_t)(length * 2u);
		keep_mask = shift_by(0xFFFFFFFFu, (uint8_t)length, 32);
		start_x = ~keep_mask;
		color = color & start_x & start_x;
		mem_write32(start_y, (keep_mask & mem_read32(start_y)) | color);
		*p_start_x = start_x;
		*p_start_y = start_y;
		*p_color = color;
		*p_length = length;
		return;
	}
	pixels_before_byte = length;
	length = pixels_before_byte - 1u;
	if (pixels_before_byte != 1u)
	{
		start_x = (start_x & ~0xFFu) | (uint8_t)color;
		start_x = (start_x & ~0xFFu) | (uint8_t)(start_x << 4);
		start_x = (start_x & ~0xFFu) | (uint8_t)(start_x | color);
		mem_write8(start_y, start_x);
		pixels_before_second = length;
		length = pixels_before_second - 1u;
		start_y++;
		if (pixels_before_second == 1u)
		{
			*p_start_x = start_x;
			*p_start_y = start_y;
			*p_color = color;
			*p_length = length;
			return;
		}
		color = color * 0x11111111u;
		if ((uint16_t)length >= 8u)
		{
			longs_left = length >> 3;
			do
			{
				mem_write32(start_y, color);
				start_y += 4u;
				longs_left--;
			} while (longs_left != 0u);
			tail_pixels = length & 7u;
			length = tail_pixels;
			if (tail_pixels == 0u)
			{
				*p_start_x = start_x;
				*p_start_y = start_y;
				*p_color = color;
				*p_length = length;
				return;
			}
		}
		length = (length & ~0xFFu) | (uint8_t)(length * 2u);
		length = (length & ~0xFFu) | (uint8_t)(length * 2u);
		keep_mask = shift_by(0xFFFFFFFFu, (uint8_t)length, 32);
		start_x = ~keep_mask;
		color = color & start_x & start_x;
		mem_write32(start_y, (keep_mask & mem_read32(start_y)) | color);
		*p_start_x = start_x;
		*p_start_y = start_y;
		*p_color = color;
		*p_length = length;
		return;
	}
	start_x = (start_x & ~0xFFu) | 0xF0u;
	start_x = (start_x & ~0xFFu) | (start_x & mem_read8(start_y));
	start_x = (start_x & ~0xFFu) | (uint8_t)(start_x | color);
	mem_write8(start_y, start_x);
	*p_start_x = start_x;
	*p_start_y = start_y;
	*p_color = color;
	*p_length = length;
}

void draw_vertical_span_4bpp(uint16_t start_x, uint32_t row_step, uint32_t *p_start_y, uint32_t *p_color, uint32_t *p_length)
{
	uint32_t start_y = *p_start_y;
	uint32_t color = *p_color;
	uint32_t length = *p_length;

	start_y = (uint32_t)(uint16_t)start_y << 8;
	start_y = ((start_y & ~0xFFFFu) | (uint16_t)(start_y + ((uint16_t)start_x >> 1))) + bitmap_address;
	draw_vertical_span_4bpp_here((uint8_t)start_x, row_step, &start_y, &color, &length);
	*p_start_y = start_y;
	*p_color = (*p_color & 0xFFFFFF00u) | (color & 0xFFu);
	*p_length = length;
}

void draw_vertical_span_4bpp_here(uint8_t start_x, uint32_t row_step, uint32_t *p_pixel_address, uint32_t *p_color, uint32_t *p_length)
{
	uint32_t pixel_address = *p_pixel_address;
	uint8_t color = (uint8_t)*p_color;
	uint32_t length = *p_length;
	uint32_t carry = 0;
	uint8_t keep_mask = 0;

	length = (length & ~0xFFFFu) | (uint16_t)(length + 1u);
	keep_mask = 0xF0u;
	color &= 0xFu;
	if ((start_x & 1u) != 0u)
	{
		color = (uint8_t)((uint32_t)color << 4);
		keep_mask = 0xFu;
	}
	carry = length & 1u;
	length = length >> 1;
	if (carry == 0u)
	{
		mem_write8(pixel_address, (mem_read8(pixel_address) & keep_mask) | color);
		pixel_address += row_step;
	}
	else
		length++;
	for (;;)
	{
		mem_write8(pixel_address, (mem_read8(pixel_address) & keep_mask) | color);
		pixel_address += row_step;
		length--;
		if (length == 0u)
			break;
		mem_write8(pixel_address, (mem_read8(pixel_address) & keep_mask) | color);
		pixel_address += row_step;
	}
	*p_pixel_address = pixel_address;
	*p_color = (*p_color & 0xFFFFFF00u) | color;
	*p_length = length;
}

void draw_dotted_line_8bpp(uint32_t start_x, uint32_t start_y, uint32_t end_x, uint32_t end_y, uint8_t color)
{
	uint32_t zero = 0, negative = 0, overflow = 0, carry = 0;
	uint16_t fraction = 0;
	uint32_t direction_index = 0, dot_count = 0, dots_left = 0, dy = 0, end_outcode = 0, end_x_copy = 0, end_y_copy = 0, major_step = 0, minor_step = 0, outcode = 0, row_step = 0, span_end_copy = 0, span_length = 0, step_record = 0, steps_left = 0, unclipped_y = 0;
	uint16_t end_y_16 = 0, dy_16 = 0, span_end_16 = 0, span_dots = 0, end_x_16 = 0, dx_16 = 0, column_dots = 0, diagonal_dots = 0;
	uint32_t saved_step = 0, saved_dy = 0;

	get_window_outcode(&start_x, &start_y, &end_x, &end_y, &outcode);
	end_outcode = outcode;
	get_window_outcode(&start_x, &start_y, &end_x, &end_y, &outcode);
	if ((uint16_t)(outcode & end_outcode) != 0u)
		return;
	if ((uint16_t)(outcode | end_outcode) != 0u)
		clip_line_to_window(outcode, end_outcode, &start_x, &start_y, &end_x, &end_y, &unclipped_y);
	direction_index = 0u;
	end_y_copy = end_y;
	end_y_16 = end_y_copy;
	dy_16 = end_y_16 - start_y;
	overflow = (uint32_t)(int16_t)end_y_16 - (int16_t)start_y != (uint32_t)(int16_t)(end_y_16 - start_y);
	zero = dy_16 == 0u;
	negative = dy_16 >> 15;
	dy = (end_y_copy & ~0xFFFFu) | dy_16;
	if (end_y_16 == (uint16_t)start_y)
	{
		start_y = ((uint32_t)(uint16_t)start_y << 9) + start_x + bitmap_address;
		span_end_copy = (uint16_t)end_x;
		span_end_16 = span_end_copy;
		span_length = (uint16_t)(span_end_16 - start_x);
		if ((int16_t)span_end_16 < (int16_t)start_x)
		{
			span_length = (uint16_t)(0u - span_length);
			start_y -= span_length;
		}
		span_length = (uint16_t)(span_length + 1u);
		span_dots = span_length >> 1;
		dots_left = span_dots;
		if (span_dots == 0u)
		{
			dots_left = (uint16_t)(dots_left + 1u);
		}
		do
		{
			mem_write8(start_y, color);
			start_y = ((start_y + 1u) & ~0xFFFFu) | (uint16_t)(start_y + 1u + 1u);
			dots_left--;
		} while (dots_left != 0u);
		return;
	}
	if ((negative ^ overflow) | zero)
	{
		dy = (dy & ~0xFFFFu) | (uint16_t)(0u - dy);
		direction_index = (uint8_t)(direction_index + 1u);
	}
	end_x_copy = end_x;
	end_x_16 = end_x_copy;
	dx_16 = end_x_16 - start_x;
	overflow = (uint32_t)(int16_t)end_x_16 - (int16_t)start_x != (uint32_t)(int16_t)(end_x_16 - start_x);
	zero = dx_16 == 0u;
	negative = dx_16 >> 15;
	steps_left = (end_x_copy & ~0xFFFFu) | dx_16;
	if (end_x_16 == (uint16_t)start_x)
	{
		if (direction_index != 0u)
			start_y = end_y;
		start_y = (uint32_t)(uint16_t)start_y << 9;
		start_y = ((start_y & ~0xFFFFu) | (uint16_t)(start_y + start_x)) + bitmap_address;
		row_step = 0x400u;
		dy = (dy & ~0xFFFFu) | (uint16_t)(dy + 1u);
		column_dots = (uint16_t)dy >> 1;
		dot_count = (dy & ~0xFFFFu) | column_dots;
		if (column_dots == 0u)
		{
			dot_count = (dot_count & ~0xFFFFu) | (uint16_t)(dot_count + 1u);
		}
		do
		{
			mem_write8(start_y, color);
			start_y += row_step;
			dot_count--;
		} while (dot_count != 0u);
		return;
	}
	if ((negative ^ overflow) | zero)
	{
		steps_left = (steps_left & ~0xFFFFu) | (uint16_t)(0u - steps_left);
		direction_index = (uint8_t)(direction_index + 2u);
	}
	start_y = ((uint32_t)(uint16_t)start_y << 9) + start_x + bitmap_address;
	step_record = line_direction_steps_ADDRESS + direction_index * 4u;
	minor_step = (int16_t)ROM16(step_record);
	major_step = (int16_t)ROM16(step_record + 2u);
	if ((uint16_t)steps_left == (uint16_t)dy)
	{
		minor_step = (minor_step + major_step) * 2u;
		dy = (dy & ~0xFFFFu) | (uint16_t)(dy + 1u);
		diagonal_dots = (uint16_t)dy >> 1;
		dot_count = (dy & ~0xFFFFu) | diagonal_dots;
		if (diagonal_dots == 0u)
		{
			dot_count = (dot_count & ~0xFFFFu) | (uint16_t)(dot_count + 1u);
		}
		do
		{
			mem_write8(start_y, color);
			start_y += minor_step;
			dot_count--;
		} while (dot_count != 0u);
		return;
	}
	if ((uint16_t)steps_left < (uint16_t)dy)
	{
		saved_step = major_step;
		major_step = minor_step;
		minor_step = saved_step;
		saved_dy = dy;
		dy = steps_left;
		steps_left = saved_dy;
	}
	steps_left = (steps_left & ~0xFFFFu) | (uint16_t)(steps_left + 1u);
	dy = divide_s32((uint32_t)(uint16_t)(dy + 1u) << 16, steps_left);
	fraction = 0u;
	for (;;)
	{
		mem_write8(start_y, color);
		start_y += major_step;
		carry = (uint16_t)(fraction + dy) < fraction;
		fraction += dy;
		steps_left--;
		if (((steps_left != 0u) & (carry == 0u)) == 0u)
		{
			start_y += minor_step;
			if (steps_left == 0u)
				break;
		}
		start_y += major_step;
		carry = (uint16_t)(fraction + dy) < fraction;
		fraction += dy;
		steps_left--;
		if ((steps_left != 0u) & (carry == 0u))
			continue;
		start_y += minor_step;
		if ((int32_t)steps_left <= 0)
			break;
	}
}

void draw_rect_outline(uint32_t left, uint32_t top, uint32_t width, uint32_t height, uint32_t color)
{
	uint32_t column_address = 0, column_length = 0, left_byte_offset = 0, length = 0, pixel_address = 0, right_side_length = 0, row_step = 0, saved_top_left = 0, top_left_address = 0, unclipped_edges = 0;

	clip_rect_to_window(&left, &top, &width, &height, &left_byte_offset, &unclipped_edges);
	if (unclipped_edges == 0u)
		return;
	left = (int16_t)left;
	top = (int16_t)top;
	width = (int16_t)width - left;
	height = (int16_t)height - top;
	unclipped_edges = unclipped_edges * 3u;
	if (bitmap_8bpp == 0u)
	{
		color = (color & 0xFu) * 0x11111111u;
		left_byte_offset = left >> 1;
		top = (top << 8) + left_byte_offset + bitmap_address;
		switch (unclipped_edges)
		{
		case 0x0u:
		case 0x9u:
		case 0xCu:
		case 0xFu:
		case 0x12u:
		case 0x15u:
		case 0x18u:
		case 0x1Bu:
		case 0x1Eu:
		case 0x21u:
		case 0x24u:
		case 0x27u:
		case 0x2Au:
		case 0x2Du:
			return;
		case 0x3u:
			break;
		case 0x6u:
		case 0x30u:
			left = (uint16_t)(left + width);
			top = (top & ~0xFFFFu) | (uint16_t)(top + ((left >> 1) - left_byte_offset));
			break;
		default:
			unknown_code(0x9ADA3u + unclipped_edges);
		}
		column_length = (uint16_t)height;
		draw_vertical_span_4bpp_here((uint8_t)left, unclipped_edges, &top, &color, &column_length);
		return;
	}
	color = (uint8_t)color * 0x1010101u;
	top = (top << 9) + left + bitmap_address;
	switch (unclipped_edges)
	{
	case 0x0u:
	case 0x30u:
	case 0x39u:
	case 0x3Cu:
	case 0x3Fu:
	case 0x42u:
	case 0x45u:
	case 0x48u:
	case 0x4Bu:
	case 0x4Eu:
	case 0x51u:
	case 0x54u:
	case 0x57u:
	case 0x5Au:
	case 0x5Du:
		return;
	case 0x3u:
		column_length = height;
		top = draw_vertical_line_8bpp(top, (uint8_t)color, column_length);
		return;
	case 0x6u:
		top += width;
		column_length = height;
		top = draw_vertical_line_8bpp(top, (uint8_t)color, column_length);
		return;
	case 0x9u:
		if (height == 0u)
		{
			column_length = height;
			top = draw_vertical_line_8bpp(top, (uint8_t)color, column_length);
			return;
		}
		top = draw_rect_sides_8bpp(top, width, height, (uint8_t)color);
		return;
	case 0xCu:
		length = width;
		fill_span_8bpp(length, &left, &top, &color);
		return;
	case 0xFu:
		saved_top_left = top;
		column_length = height;
		length = width;
		fill_span_8bpp(length, &left, &top, &color);
		column_address = saved_top_left;
		column_address = draw_vertical_line_8bpp(column_address, (uint8_t)color, column_length);
		return;
	case 0x12u:
		column_length = height;
		length = width;
		fill_span_8bpp(length, &left, &top, &color);
		top = draw_vertical_line_8bpp(top, (uint8_t)color, column_length);
		return;
	case 0x15u:
		if (height != 0u)
		{
			top_left_address = top;
			draw_rect_sides_8bpp(top, width, height, (uint8_t)color);
			pixel_address = top_left_address;
			length = width;
			fill_span_8bpp(length, &top_left_address, &pixel_address, &color);
			return;
		}
		saved_top_left = top;
		column_length = height;
		length = width;
		fill_span_8bpp(length, &left, &top, &color);
		column_address = saved_top_left;
		column_address = draw_vertical_line_8bpp(column_address, (uint8_t)color, column_length);
		return;
	case 0x18u:
		top += height << 9;
		length = width;
		fill_span_8bpp(length, &left, &top, &color);
		return;
	case 0x1Bu:
		column_length = height;
		length = width;
		top = draw_vertical_line_8bpp(top, (uint8_t)color, column_length);
		fill_span_8bpp(length, &left, &top, &color);
		return;
	case 0x1Eu:
		right_side_length = height;
		top += height << 9;
		length = width;
		fill_span_8bpp(length, &left, &top, &color);
		row_step = (int16_t)0xFE00u;
		draw_vertical_span_8bpp(row_step, (uint8_t)color, &top, &right_side_length);
		return;
	case 0x21u:
		if (height == 0u)
		{
			column_length = height;
			length = width;
			top = draw_vertical_line_8bpp(top, (uint8_t)color, column_length);
			fill_span_8bpp(length, &left, &top, &color);
			return;
		}
		top = draw_rect_sides_8bpp(top, width, height, (uint8_t)color);
		length = width;
		fill_span_8bpp(length, &left, &top, &color);
		return;
	case 0x24u:
		top = draw_rect_top_bottom_8bpp(top, width, height, color);
		return;
	case 0x27u:
		if (width != 0u)
		{
			top_left_address = top;
			draw_rect_top_bottom_8bpp(top, width, height, color);
			column_address = top_left_address;
			column_length = height;
			column_address = draw_vertical_line_8bpp(column_address, (uint8_t)color, column_length);
			return;
		}
		saved_top_left = top;
		column_length = height;
		length = width;
		fill_span_8bpp(length, &left, &top, &color);
		column_address = saved_top_left;
		column_address = draw_vertical_line_8bpp(column_address, (uint8_t)color, column_length);
		return;
	case 0x2Au:
		if (width == 0u)
		{
			column_length = height;
			length = width;
			fill_span_8bpp(length, &left, &top, &color);
			top = draw_vertical_line_8bpp(top, (uint8_t)color, column_length);
			return;
		}
		top = draw_rect_top_bottom_8bpp(top, width, height, color);
		column_length = height;
		top = draw_vertical_line_8bpp(top, (uint8_t)color, column_length);
		return;
	case 0x2Du:
		if (width == 0u)
		{
			if (height != 0u)
			{
				top_left_address = top;
				draw_rect_sides_8bpp(top, width, height, (uint8_t)color);
				pixel_address = top_left_address;
				length = width;
				fill_span_8bpp(length, &top_left_address, &pixel_address, &color);
				return;
			}
			saved_top_left = top;
			column_length = height;
			length = width;
			fill_span_8bpp(length, &left, &top, &color);
			column_address = saved_top_left;
			column_address = draw_vertical_line_8bpp(column_address, (uint8_t)color, column_length);
			return;
		}
		top_left_address = top;
		draw_rect_top_bottom_8bpp(top, width, height, color);
		top = top_left_address;
		if (height == 0u)
		{
			column_length = height;
			top = draw_vertical_line_8bpp(top, (uint8_t)color, column_length);
			return;
		}
		top = draw_rect_sides_8bpp(top, width, height, (uint8_t)color);
		return;
	case 0x33u:
		column_length = (uint16_t)height;
		draw_vertical_span_4bpp_here((uint8_t)left, unclipped_edges, &top, &color, &column_length);
		return;
	case 0x36u:
	case 0x60u:
		left = (uint16_t)(left + width);
		top = (top & ~0xFFFFu) | (uint16_t)(top + ((left >> 1) - left_byte_offset));
		column_length = (uint16_t)height;
		draw_vertical_span_4bpp_here((uint8_t)left, unclipped_edges, &top, &color, &column_length);
		return;
	default:
		unknown_code(0x9AD73u + unclipped_edges);
	}
}

uint32_t draw_rect_sides_8bpp(uint32_t pixel_address, uint32_t width, uint32_t height, uint8_t color)
{
	uint32_t right_address = 0, row_step = 0;

	right_address = pixel_address + width;
	row_step = 0x200u;
	height = (height & ~0xFFFFu) | (uint16_t)(height + 1u);
	do
	{
		mem_write8(pixel_address, color);
		mem_write8(right_address, color);
		pixel_address += row_step;
		right_address += row_step;
		height--;
	} while (height != 0u);
	pixel_address -= row_step;
	return pixel_address;
}

uint32_t draw_rect_top_bottom_8bpp(uint32_t pixel_address, uint32_t width, uint32_t height, uint32_t color)
{
	uint8_t byte_count = 0, tail_bytes = 0;
	uint32_t bottom_address = 0, bytes_left = 0, longs_left = 0;
	uint32_t bytes_after_odd = 0, long_count = 0, tail_count = 0;

	bottom_address = (height << 9) + pixel_address;
	bytes_left = width;
	if ((pixel_address & 1u) != 0u)
	{
		mem_write8(pixel_address, color);
		mem_write8(bottom_address, color);
		bottom_address++;
		bytes_after_odd = bytes_left - 1u;
		bytes_left = bytes_after_odd;
		pixel_address++;
		if ((int32_t)bytes_after_odd < 0)
		{
			pixel_address = (pixel_address & ~0xFFFFu) | (uint16_t)(pixel_address - 1u);
			return pixel_address;
		}
	}
	if (bytes_left == 0u)
	{
		mem_write8(pixel_address, color);
		mem_write8(bottom_address, color);
		pixel_address++;
	}
	else
	{
		bytes_left++;
		byte_count = (uint8_t)bytes_left;
		long_count = bytes_left >> 2;
		longs_left = long_count;
		if (long_count != 0u)
		{
			do
			{
				mem_write32(pixel_address, color);
				mem_write32(bottom_address, color);
				bottom_address += 4u;
				longs_left--;
				pixel_address += 4u;
			} while (longs_left != 0u);
		}
		tail_count = byte_count & 3u;
		tail_bytes = (uint8_t)tail_count;
		if (tail_count == 0u)
		{
			pixel_address = (pixel_address & ~0xFFFFu) | (uint16_t)(pixel_address - 1u);
			return pixel_address;
		}
		if (tail_bytes < 2u)
		{
			mem_write8(pixel_address, color);
			mem_write8(bottom_address, color);
			pixel_address++;
			pixel_address = (pixel_address & ~0xFFFFu) | (uint16_t)(pixel_address - 1u);
			return pixel_address;
		}
		mem_write16(pixel_address, color);
		mem_write16(bottom_address, color);
		bottom_address += 2u;
		pixel_address += 2u;
		if (tail_bytes != 2u)
		{
			mem_write8(pixel_address, color);
			mem_write8(bottom_address, color);
			pixel_address++;
			pixel_address = (pixel_address & ~0xFFFFu) | (uint16_t)(pixel_address - 1u);
			return pixel_address;
		}
	}
	pixel_address = (pixel_address & ~0xFFFFu) | (uint16_t)(pixel_address - 1u);
	return pixel_address;
}

void fill_rect(uint32_t top, uint32_t right, uint32_t bottom, uint32_t *p_left, uint32_t *p_color, uint32_t *p_blocks_per_row)
{
	uint32_t left = *p_left;
	uint32_t color = *p_color;
	uint32_t blocks_per_row = *p_blocks_per_row;
	uint32_t r5 = 0, row_pairs_left = 0, unclipped_edges = 0;

	clip_rect_to_window(&left, &top, &right, &bottom, &r5, &unclipped_edges);
	if (unclipped_edges == 0u)
	{
		left = 0xFFFFFFFFu;
		*p_left = left;
		return;
	}
	fill_rect_unclipped(right, bottom, &left, &top, &color, &r5, &row_pairs_left, &blocks_per_row, &unclipped_edges);
	*p_left = left;
	*p_color = color;
	*p_blocks_per_row = blocks_per_row;
}

void fill_rect_unclipped(uint32_t right, uint32_t bottom, uint32_t *p_left, uint32_t *p_top, uint32_t *p_color, uint32_t *p_inner_count, uint32_t *p_row_pairs_left, uint32_t *p_blocks_per_row, uint32_t *p_r10)
{
	uint32_t left = *p_left;
	uint32_t top = *p_top;
	uint32_t color = *p_color;
	uint32_t inner_count = *p_inner_count;
	uint32_t row_pairs_left = *p_row_pairs_left;
	uint32_t blocks_per_row = *p_blocks_per_row;
	uint32_t r10 = *p_r10;
	uint32_t carry = 0;
	uint8_t byte_color = 0, color_low = 0, high_nibble_color = 0, tail_width = 0;
	uint16_t straddle_keep_mask = 0, tail_keep_mask = 0;
	uint32_t r5 = 0, block_cursor = 0, bottom_copy = 0, column_cursor = 0, first_row = 0, last_column = 0, length = 0, lower_cursor = 0, pair_skip = 0, pixel_columns = 0, right_copy = 0, row_bytes = 0, row_offset_halfwords = 0, row_offset_longs = 0, row_pitch_4bpp = 0, row_pitch_8bpp = 0, row_step = 0, row_step_2 = 0, rows_left = 0, rows_to_fill = 0, upper_cursor = 0;
	uint16_t block_count = 0, bottom_row = 0, right_column = 0, rect_dx = 0;
	uint32_t columns_left = 0, tail_columns = 0, tail_pixels = 0;

	if (bitmap_8bpp != 0u)
	{
		bottom = (uint16_t)bottom;
		top = (uint16_t)top;
		bottom = (uint16_t)(bottom - top);
		top = top << 9;
		top = ((top & ~0xFFFFu) | (uint16_t)(top + left)) + bitmap_address;
		color = (uint8_t)color * 0x1010101u;
		right = (uint16_t)right;
		right = (uint16_t)(right - left);
		bottom = (uint16_t)(bottom + 1u);
		row_pitch_8bpp = 0x200u;
		carry = bottom & 1u;
		bottom = bottom >> 1;
		if (carry)
		{
			first_row = top;
			row_pairs_left = bottom;
			length = right;
			fill_span_8bpp(length, &left, &top, &color);
			if (row_pairs_left == 0u)
			{
				*p_left = left;
				*p_top = top;
				*p_color = color;
				*p_row_pairs_left = row_pairs_left;
				return;
			}
			top = first_row + row_pitch_8bpp;
			bottom = row_pairs_left;
		}
		if ((top & 1u) != 0u)
		{
			left = top;
			inner_count = bottom;
			top++;
			do
			{
				mem_write8(left, color);
				mem_write8(left + row_pitch_8bpp, color);
				left += row_pitch_8bpp * 2u;
				inner_count--;
			} while (inner_count != 0u);
			columns_left = right - 1u;
			right = columns_left;
			if ((int32_t)columns_left < 0)
			{
				*p_left = left;
				*p_top = top;
				*p_color = color;
				*p_inner_count = inner_count;
				*p_row_pairs_left = row_pairs_left;
				return;
			}
		}
		if (right != 0u)
		{
			right = (right & ~0xFFFFu) | (uint16_t)(right + 1u);
			row_bytes = right;
			block_count = (uint16_t)row_bytes >> 4;
			blocks_per_row = (row_bytes & ~0xFFFFu) | block_count;
			if (block_count != 0u)
			{
				left = top;
				row_pairs_left = bottom;
				top += 0xFFFFFFF0u & right;
				do
				{
					inner_count = blocks_per_row;
					upper_cursor = left;
					r10 = left + row_pitch_8bpp;
					do
					{
						mem_write32(upper_cursor, color);
						upper_cursor += 4u;
						mem_write32(upper_cursor, color);
						upper_cursor += 4u;
						mem_write32(upper_cursor, color);
						upper_cursor += 4u;
						mem_write32(upper_cursor, color);
						mem_write32(r10, color);
						r10 += 4u;
						mem_write32(r10, color);
						r10 += 4u;
						mem_write32(r10, color);
						r10 += 4u;
						mem_write32(r10, color);
						inner_count--;
						upper_cursor += 4u;
						r10 += 4u;
					} while (inner_count != 0u);
					left += row_pitch_8bpp * 2u;
					row_pairs_left--;
				} while (row_pairs_left != 0u);
			}
			right &= 0xFu;
			if (right >= 8u)
			{
				left = top;
				inner_count = bottom;
				lower_cursor = left + row_pitch_8bpp;
				pair_skip = 0x3F8u;
				top += 8u;
				do
				{
					mem_write32(left, color);
					left += 4u;
					mem_write32(left, color);
					mem_write32(lower_cursor, color);
					lower_cursor += 4u;
					mem_write32(lower_cursor, color);
					left = left + 4u + pair_skip;
					lower_cursor = lower_cursor + 4u + pair_skip;
					inner_count--;
				} while (inner_count != 0u);
				row_pitch_8bpp = 0x200u;
				right -= 8u;
			}
			if (right >= 4u)
			{
				left = top;
				inner_count = bottom;
				row_offset_longs = 0x80u;
				top += 4u;
				do
				{
					mem_write32(left, color);
					mem_write32(left + row_offset_longs * 4u, color);
					left += row_pitch_8bpp * 2u;
					inner_count--;
				} while (inner_count != 0u);
			}
			tail_columns = right & 3u;
			tail_width = (uint8_t)tail_columns;
			if (tail_columns == 0u)
			{
				*p_left = left;
				*p_top = top;
				*p_color = color;
				*p_inner_count = inner_count;
				*p_row_pairs_left = row_pairs_left;
				*p_blocks_per_row = blocks_per_row;
				*p_r10 = r10;
				return;
			}
			if (tail_width >= 2u)
			{
				inner_count = bottom;
				left = top;
				row_offset_halfwords = 0x100u;
				top += 2u;
				do
				{
					mem_write16(left, color);
					mem_write16(left + row_offset_halfwords * 2u, color);
					left += row_pitch_8bpp * 2u;
					inner_count--;
				} while (inner_count != 0u);
				if ((tail_width & 1u) == 0u)
				{
					*p_left = left;
					*p_top = top;
					*p_color = color;
					*p_inner_count = inner_count;
					*p_row_pairs_left = row_pairs_left;
					*p_blocks_per_row = blocks_per_row;
					*p_r10 = r10;
					return;
				}
			}
		}
		do
		{
			mem_write8(top, color);
			mem_write8(top + row_pitch_8bpp, color);
			top += row_pitch_8bpp * 2u;
			bottom--;
		} while (bottom != 0u);
		*p_left = left;
		*p_top = top;
		*p_color = color;
		*p_inner_count = inner_count;
		*p_row_pairs_left = row_pairs_left;
		*p_blocks_per_row = blocks_per_row;
		*p_r10 = r10;
		return;
	}
	color = (color & ~0xFFu) | (color & 0xFu);
	bottom_copy = (uint16_t)bottom;
	top = (uint16_t)top;
	bottom_row = bottom_copy;
	rows_left = (uint16_t)(bottom_row - top);
	if (bottom_row == top)
	{
		fill_span_4bpp((uint16_t)right, &left, &top, &color, &r5);
		*p_left = left;
		*p_top = top;
		*p_color = color;
		*p_inner_count = r5;
		*p_row_pairs_left = row_pairs_left;
		*p_blocks_per_row = blocks_per_row;
		*p_r10 = r10;
		return;
	}
	right_copy = (uint16_t)right;
	right_column = right_copy;
	last_column = (uint16_t)(right_column - left);
	if (right_column == (uint16_t)left)
	{
		row_step_2 = 0x100u;
		draw_vertical_span_4bpp((uint16_t)left, row_step_2, &top, &color, &rows_left);
		*p_left = left;
		*p_top = top;
		*p_color = color;
		*p_inner_count = rows_left;
		*p_row_pairs_left = row_pairs_left;
		*p_blocks_per_row = blocks_per_row;
		*p_r10 = row_step_2;
		return;
	}
	top = top << 8;
	top = ((top & ~0xFFFFu) | (uint16_t)(top + ((uint16_t)left >> 1))) + bitmap_address;
	row_pitch_4bpp = 0x100u;
	rows_left = (uint16_t)(rows_left + 1u);
	rect_dx = last_column;
	pixel_columns = (uint16_t)(rect_dx - 1u);
	if (rect_dx == 1u)
	{
		if ((left & 1u) == 0u)
		{
			color_low = (uint8_t)color;
			color = (color & ~0xFFu) | (uint8_t)(color << 4);
			color = (color & ~0xFFu) | (uint8_t)(color | color_low);
			row_step = row_pitch_4bpp;
			draw_vertical_span_8bpp(row_step, (uint8_t)color, &top, &rows_left);
			*p_left = left;
			*p_top = top;
			*p_color = color;
			*p_inner_count = rows_left;
			*p_row_pairs_left = row_pitch_4bpp;
			*p_blocks_per_row = blocks_per_row;
			*p_r10 = r10;
			return;
		}
		color = (color & ~0xFFFFu) | (uint16_t)(color * 0x110u);
		straddle_keep_mask = 0xF00Fu;
		do
		{
			left = (left & ~0xFFFFu) | mem_read16(top);
			left = (left & ~0xFFFFu) | (left & straddle_keep_mask);
			left = (left & ~0xFFFFu) | (uint16_t)(left | color);
			mem_write16(top, left);
			top += row_pitch_4bpp;
			rows_left--;
		} while (rows_left != 0u);
		*p_left = left;
		*p_top = top;
		*p_color = color;
		*p_inner_count = rows_left;
		*p_row_pairs_left = row_pitch_4bpp;
		return;
	}
	pixel_columns = (uint16_t)(pixel_columns + 2u);
	if ((left & 1u) != 0u)
	{
		high_nibble_color = (uint8_t)(color << 4);
		rows_to_fill = rows_left;
		column_cursor = top;
		top++;
		do
		{
			left = (left & ~0xFFu) | mem_read8(column_cursor);
			left = (left & ~0xFFu) | (left & 0xFu);
			left = (left & ~0xFFu) | (uint8_t)(left | high_nibble_color);
			mem_write8(column_cursor, left);
			column_cursor += row_pitch_4bpp;
			rows_to_fill--;
		} while (rows_to_fill != 0u);
		pixel_columns--;
	}
	if ((top & 1u) != 0u)
	{
		byte_color = (uint8_t)color;
		color = (color & ~0xFFu) | (uint8_t)(color << 4);
		byte_color |= color;
		column_cursor = top;
		rows_to_fill = rows_left;
		top++;
		do
		{
			mem_write8(column_cursor, byte_color);
			column_cursor += row_pitch_4bpp;
			rows_to_fill--;
		} while (rows_to_fill != 0u);
		pixel_columns -= 2u;
	}
	if (pixel_columns >= 4u)
	{
		color = (color & ~0xFFFFu) | (uint16_t)(color * 0x1111u);
		rows_to_fill = rows_left;
		block_cursor = (uint16_t)(rows_left << 8) + top;
		blocks_per_row = pixel_columns >> 2;
		do
		{
			r10 = blocks_per_row;
			block_cursor -= row_pitch_4bpp;
			do
			{
				mem_write16(block_cursor, color);
				block_cursor += 2u;
				r10--;
			} while (r10 != 0u);
			rows_to_fill--;
		} while (rows_to_fill != 0u);
		top = block_cursor;
		tail_pixels = pixel_columns & 3u;
		pixel_columns = tail_pixels;
		if (tail_pixels == 0u)
		{
			*p_left = left;
			*p_top = top;
			*p_color = color;
			*p_inner_count = rows_left;
			*p_row_pairs_left = row_pitch_4bpp;
			*p_blocks_per_row = blocks_per_row;
			*p_r10 = r10;
			return;
		}
	}
	color = (color & ~0xFFFFu) | (color & 0xFu);
	color = (color & ~0xFFFFu) | (uint16_t)(color * 0x1111u);
	tail_keep_mask = shift_by(0xFFFFu, (uint8_t)(pixel_columns * 2u * 2u), 16);
	left = (left & ~0xFFFFu) | (uint16_t)~tail_keep_mask;
	color = (color & ~0xFFFFu) | (uint16_t)(color & left);
	do
	{
		left = (left & ~0xFFFFu) | mem_read16(top);
		left = (left & ~0xFFFFu) | (left & tail_keep_mask);
		left = (left & ~0xFFFFu) | (uint16_t)(left | color);
		mem_write16(top, left);
		top += row_pitch_4bpp;
		rows_left--;
	} while (rows_left != 0u);
	*p_left = left;
	*p_top = top;
	*p_color = color;
	*p_inner_count = rows_left;
	*p_row_pairs_left = row_pitch_4bpp;
	*p_blocks_per_row = blocks_per_row;
	*p_r10 = r10;
}

void fill_triangle(uint32_t top_x, uint32_t top_y, uint32_t middle_x, uint32_t middle_y, uint32_t bottom_x, uint32_t bottom_y, uint8_t color)
{
	uint32_t span_cursor = 0;
	uint16_t swap_x_12 = 0, swap_y_12 = 0, swap_x_13 = 0, swap_y_13 = 0, swap_x_23 = 0, swap_y_23 = 0;

	if ((int16_t)middle_y < (int16_t)top_y)
	{
		swap_x_12 = top_x;
		top_x = (uint16_t)middle_x;
		middle_x = swap_x_12;
		swap_y_12 = top_y;
		top_y = (uint16_t)middle_y;
		middle_y = swap_y_12;
	}
	if ((int16_t)bottom_y < (int16_t)top_y)
	{
		swap_x_13 = top_x;
		top_x = (uint16_t)bottom_x;
		bottom_x = swap_x_13;
		swap_y_13 = top_y;
		top_y = (uint16_t)bottom_y;
		bottom_y = swap_y_13;
	}
	if ((int16_t)bottom_y < (int16_t)middle_y)
	{
		swap_x_23 = middle_x;
		middle_x = (uint16_t)bottom_x;
		bottom_x = swap_x_23;
		swap_y_23 = middle_y;
		middle_y = (uint16_t)bottom_y;
		bottom_y = swap_y_23;
	}
	span_cursor = bitmap_draw_buffer_ADDRESS;
	current_player = bitmap_address;
	top_x = (int16_t)top_x;
	top_y = (int16_t)top_y;
	middle_x = (int16_t)middle_x;
	middle_y = (int16_t)middle_y;
	bottom_x = (int16_t)bottom_x;
	bottom_y = (int16_t)bottom_y;
	fill_triangle_sorted(top_x, top_y, middle_x, middle_y, bottom_x, bottom_y, color, span_cursor);
}

void fill_triangle_sorted(uint32_t top_x, uint32_t top_y, uint32_t middle_x, uint32_t middle_y, uint32_t bottom_x, uint32_t bottom_y, uint8_t color, uint32_t span_cursor)
{
	uint8_t integer_shift = 0;
	uint32_t bottom_x_fixed = 0, height = 0, left_x = 0, length = 0, middle_on_right = 0, previous_left_x = 0, right_edge_x = 0, right_x = 0, row_left_x = 0, rows_left = 0, span_address = 0, span_color = 0, span_count = 0;
	uint32_t saved_middle_y = 0, saved_bottom_y = 0, swap_x_12 = 0, swap_x_13 = 0, swap_top_x = 0, swap_slope = 0;

	saved_middle_y = middle_y;
	rows_left = saved_middle_y - top_y;
	if (saved_middle_y == top_y)
	{
		saved_bottom_y = bottom_y;
		height = saved_bottom_y - top_y;
		if (saved_bottom_y == top_y)
		{
			if ((int32_t)middle_x < (int32_t)top_x)
			{
				swap_x_12 = top_x;
				top_x = middle_x;
				middle_x = swap_x_12;
			}
			if ((int32_t)bottom_x < (int32_t)top_x)
			{
				swap_x_13 = top_x;
				top_x = bottom_x;
				bottom_x = swap_x_13;
			}
			if ((int32_t)bottom_x < (int32_t)middle_x)
				bottom_x = middle_x;
			top_y = (top_y << 9) + top_x + current_player;
			length = bottom_x - top_x;
			span_color = (uint8_t)color;
			fill_span_8bpp(length, &top_x, &top_y, &span_color);
			return;
		}
		if ((int32_t)top_x <= (int32_t)middle_x)
		{
			fill_flat_top_triangle(top_x, top_y, middle_x, bottom_x, height, color, span_cursor);
			return;
		}
		swap_top_x = top_x;
		left_x = middle_x;
		right_x = swap_top_x;
		fill_flat_top_triangle(left_x, top_y, right_x, bottom_x, height, color, span_cursor);
		return;
	}
	bottom_y = bottom_y - top_y + 1u;
	span_address = (top_y << 9) + top_x + current_player;
	previous_left_x = top_x;
	top_x = top_x << 16;
	bottom_x = bottom_x << 16;
	bottom_x_fixed = bottom_x;
	integer_shift = 0xF0u;
	rows_left++;
	middle_x = divide_s32((middle_x << 16) - top_x, rows_left);
	bottom_x = divide_s32(bottom_x - top_x, bottom_y);
	middle_on_right = 0u;
	if ((int32_t)bottom_x < (int32_t)middle_x)
	{
		swap_slope = middle_x;
		middle_x = bottom_x;
		bottom_x = swap_slope;
		middle_on_right = 1u;
	}
	right_edge_x = top_x;
	r14 = 0x200u;
	span_count = bottom_y;
	bottom_y -= rows_left;
	for (;;)
	{
		top_x += middle_x;
		right_edge_x += bottom_x;
		row_left_x = shift_signed_by(top_x, integer_shift, 32);
		span_address -= previous_left_x - row_left_x;
		mem_write32(span_cursor, span_address);
		span_cursor += 4u;
		mem_write16(span_cursor, shift_signed_by(right_edge_x, integer_shift, 32) - row_left_x);
		previous_left_x = row_left_x;
		span_address += r14;
		span_cursor += 2u;
		rows_left--;
		if (rows_left != 0u)
			continue;
		if (bottom_y == 0u)
			break;
		if (middle_on_right == 0u)
		{
			middle_x = divide_s32(bottom_x_fixed - top_x, bottom_y);
			rows_left = bottom_y;
			bottom_y = 0u;
			continue;
		}
		bottom_x = divide_s32(bottom_x_fixed - right_edge_x, bottom_y);
		rows_left = bottom_y;
		bottom_y = 0u;
	}
	fill_span_list(span_count, color);
}

void fill_flat_top_triangle(uint32_t left_x, uint32_t top_y, uint32_t right_x, uint32_t bottom_x, uint32_t height, uint8_t color, uint32_t span_cursor)
{
	uint8_t integer_shift = 0;
	uint32_t left_slope = 0, previous_left_x = 0, row_left_x = 0, span_address = 0, span_count = 0;

	span_address = current_player + ((top_y << 9) + left_x);
	previous_left_x = left_x;
	bottom_x = bottom_x << 16;
	right_x = right_x << 16;
	left_x = left_x << 16;
	height++;
	left_slope = divide_s32(bottom_x - left_x, height);
	bottom_x = divide_s32(bottom_x - right_x, height);
	integer_shift = 0xF0u;
	span_count = height;
	r14 = 0x200u;
	do
	{
		row_left_x = shift_signed_by(left_x, integer_shift, 32);
		mem_write32(span_cursor, span_address);
		span_cursor += 4u;
		mem_write16(span_cursor, shift_signed_by(right_x, integer_shift, 32) - row_left_x);
		span_address = span_address - (previous_left_x - row_left_x) + r14;
		left_x += left_slope;
		right_x += bottom_x;
		previous_left_x = row_left_x;
		span_cursor += 2u;
		height--;
	} while (height != 0u);
	fill_span_list(span_count, color);
}

void fill_span_list_from_r8(uint32_t span_count, uint8_t polygon_color)
{
	uint8_t color = 0;

	color = (uint8_t)polygon_color;
	fill_span_list(span_count, color);
}

void fill_span_list(uint32_t span_count, uint32_t color)
{
	uint32_t zero = 0;
	uint8_t last_bytes = 0, tail_bytes = 0;
	uint32_t blocks_left = 0, length = 0, pixel_address = 0, span_record = 0;
	uint16_t block_count = 0;
	uint32_t odd_bytes = 0, length_after_odd = 0;

	span_record = bitmap_draw_buffer_ADDRESS;
	color = (uint8_t)color * 0x1010101u;
	do
	{
		pixel_address = mem_read32(span_record);
		span_record += 4u;
		length = mem_read16(span_record);
		span_record += 2u;
		if ((int16_t)length >= 0)
		{
			if ((pixel_address & 1u) == 0u)
			{
			fill_span_list_aligned:
				length++;
				tail_bytes = (uint8_t)length;
				block_count = (uint16_t)length >> 4;
				blocks_left = (length & ~0xFFFFu) | block_count;
				if (block_count != 0u)
				{
					do
					{
						mem_write32(pixel_address, color);
						pixel_address += 4u;
						mem_write32(pixel_address, color);
						pixel_address += 4u;
						mem_write32(pixel_address, color);
						pixel_address += 4u;
						mem_write32(pixel_address, color);
						blocks_left--;
						pixel_address += 4u;
					} while (blocks_left != 0u);
				}
				tail_bytes &= 0xFu;
				if (tail_bytes >= 8u)
				{
					mem_write32(pixel_address, color);
					pixel_address += 4u;
					mem_write32(pixel_address, color);
					tail_bytes -= 8u;
					pixel_address += 4u;
				}
				if ((uint8_t)tail_bytes >= 4u)
				{
					mem_write32(pixel_address, color);
					pixel_address += 4u;
				}
				odd_bytes = tail_bytes & 3u;
				last_bytes = (uint8_t)odd_bytes;
				if (odd_bytes != 0u)
				{
					zero = (uint8_t)(last_bytes - 2u) == 0u;
					if (last_bytes < 2u)
					{
					fill_span_list_last_byte:
						mem_write8(pixel_address, color);
					}
					else
					{
						mem_write16(pixel_address, color);
						pixel_address += 2u;
						if (zero == 0u)
							goto fill_span_list_last_byte;
					}
				}
			}
			else
			{
				mem_write8(pixel_address, color);
				length_after_odd = length - 1u;
				length = length_after_odd;
				pixel_address++;
				if ((int32_t)length_after_odd >= 0)
					goto fill_span_list_aligned;
			}
		}
		span_count--;
	} while (span_count != 0u);
}

void fill_quad(uint32_t left_x, uint32_t top_y, uint32_t second_x, uint32_t second_y, uint32_t third_x, uint32_t third_y, uint32_t bottom_x, uint32_t bottom_y, uint8_t fill_color)
{
	uint8_t bottom_index = 0, pixel_shift = 0, second_index = 0, third_index = 0, top_index = 0, triangle_color = 0;
	uint16_t right_column = 0;
	uint32_t apex_x = 0, bend_on_right = 0, bottom_right_x = 0, bottom_vertex = 0, color = 0, edge_parts_left = 0, left_column = 0, left_end = 0, left_pixel = 0, left_slope = 0, length = 0, next_part_rows = 0, previous_left = 0, prior_left = 0, right_side_slope = 0, right_slope = 0, right_x = 0, row_count = 0, rows_after_second = 0, segments_left = 0, single_edge_slope = 0, span_address = 0, span_cursor = 0, triangle_height = 0, triangle_left_x = 0, triangle_right_x = 0, triangle_top_y = 0, vertex_index = 0;
	uint16_t swap_bottom_x = 0;
	uint32_t swap_x_01 = 0, swap_key_01 = 0, swap_x_02 = 0, swap_key_02 = 0, swap_x_03 = 0, swap_key_03 = 0, swap_x_12 = 0, swap_key_12 = 0, swap_x_13 = 0, swap_key_13 = 0, swap_x_23 = 0, swap_key_23 = 0, swap_edge_slope = 0, third_rows = 0, rows_after_left_bend = 0, swap_slope = 0, rows_after_right_bend = 0, swap_top_x = 0, swap_upper_x = 0, swap_lower_x = 0, swap_triangle_x = 0;

	stack_pointer = stack_pointer - 2u;
	mem_write16(stack_pointer, bottom_y);
	stack_pointer = stack_pointer - 2u;
	mem_write16(stack_pointer, bottom_x);
	stack_pointer = stack_pointer - 2u;
	mem_write16(stack_pointer, third_y);
	stack_pointer = stack_pointer - 2u;
	mem_write16(stack_pointer, third_x);
	stack_pointer = stack_pointer - 2u;
	mem_write16(stack_pointer, second_y);
	stack_pointer = stack_pointer - 2u;
	mem_write16(stack_pointer, second_x);
	stack_pointer = stack_pointer - 2u;
	mem_write16(stack_pointer, top_y);
	stack_pointer = stack_pointer - 2u;
	mem_write16(stack_pointer, left_x);
	bottom_x = (int16_t)bottom_x;
	third_x = (int16_t)third_x;
	second_x = (int16_t)second_x;
	left_x = (int16_t)left_x;
	second_y = (uint32_t)(int16_t)second_y << 8;
	top_y = (uint32_t)(int16_t)top_y << 8;
	bottom_y = ((uint32_t)(int16_t)bottom_y << 8) | 3u;
	third_y = ((uint32_t)(int16_t)third_y << 8) | 2u;
	second_y |= (uint8_t)(second_y + 1u);
	if ((int32_t)second_y < (int32_t)top_y)
	{
		swap_x_01 = left_x;
		left_x = second_x;
		second_x = swap_x_01;
		swap_key_01 = top_y;
		top_y = second_y;
		second_y = swap_key_01;
	}
	if ((int32_t)third_y < (int32_t)top_y)
	{
		swap_x_02 = left_x;
		left_x = third_x;
		third_x = swap_x_02;
		swap_key_02 = top_y;
		top_y = third_y;
		third_y = swap_key_02;
	}
	if ((int32_t)bottom_y < (int32_t)top_y)
	{
		swap_x_03 = left_x;
		left_x = bottom_x;
		bottom_x = swap_x_03;
		swap_key_03 = top_y;
		top_y = bottom_y;
		bottom_y = swap_key_03;
	}
	if ((int32_t)third_y < (int32_t)second_y)
	{
		swap_x_12 = second_x;
		second_x = third_x;
		third_x = swap_x_12;
		swap_key_12 = second_y;
		second_y = third_y;
		third_y = swap_key_12;
	}
	if ((int32_t)bottom_y < (int32_t)second_y)
	{
		swap_x_13 = second_x;
		second_x = bottom_x;
		bottom_x = swap_x_13;
		swap_key_13 = second_y;
		second_y = bottom_y;
		bottom_y = swap_key_13;
	}
	if ((int32_t)bottom_y < (int32_t)third_y)
	{
		swap_x_23 = third_x;
		third_x = bottom_x;
		bottom_x = swap_x_23;
		swap_key_23 = third_y;
		third_y = bottom_y;
		bottom_y = swap_key_23;
	}
	top_index = (uint8_t)top_y;
	second_index = (uint8_t)second_y;
	third_index = (uint8_t)third_y;
	bottom_index = (uint8_t)bottom_y;
	top_y = (int32_t)top_y >> 8;
	second_y = (int32_t)second_y >> 8;
	third_y = (int32_t)third_y >> 8;
	bottom_y = (int32_t)bottom_y >> 8;
	span_cursor = bitmap_draw_buffer_ADDRESS;
	current_player = bitmap_address;
	r14 = 0x200u;
	if (second_y != top_y)
	{
		if (bottom_y == third_y)
		{
			if ((uint16_t)third_y == (uint16_t)second_y)
			{
				if ((int32_t)third_x < (int32_t)second_x)
					second_x = third_x;
				if ((int32_t)bottom_x < (int32_t)second_x)
					second_x = bottom_x;
				if ((int32_t)bottom_x < (int32_t)third_x)
					bottom_x = third_x;
				bottom_right_x = bottom_x;
				triangle_color = (uint8_t)fill_color;
				stack_pointer = stack_pointer + 0x10u;
				fill_triangle_sorted(left_x, top_y, second_x, second_y, bottom_right_x, third_y, triangle_color, span_cursor);
				return;
			}
			stack_pointer = stack_pointer + 0x10u;
			if ((int16_t)bottom_x < (int16_t)third_x)
			{
				swap_bottom_x = third_x;
				third_x = (uint16_t)bottom_x;
				bottom_x = swap_bottom_x;
				bottom_index = (uint8_t)third_index;
			}
			second_y -= top_y;
			third_y = third_y - top_y + 1u;
			prior_left = left_x;
			top_y = (top_y << 9) + left_x + current_player;
			left_x = left_x << 16;
			right_x = left_x;
			second_x = second_x << 16;
			third_x = third_x << 16;
			bottom_x = bottom_x << 16;
			pixel_shift = 0xF0u;
			segments_left = 2u;
			second_y++;
			if (((uint8_t)(bottom_index - top_index) & 3u) != 2u)
			{
				span_address = top_y;
				row_count = third_y;
				bottom_x = divide_s32(bottom_x - left_x, third_y);
				third_y -= second_y;
				second_x -= left_x;
				do
				{
					second_x = divide_s32(second_x, second_y);
					do
					{
						left_x += second_x;
						right_x += bottom_x;
						left_end = shift_by(left_x, pixel_shift, 32);
						span_address -= prior_left - left_end;
						mem_write32(span_cursor, span_address);
						span_cursor += 4u;
						mem_write16(span_cursor, shift_by(right_x, pixel_shift, 32) - left_end);
						prior_left = left_end;
						span_address += r14;
						span_cursor += 2u;
						second_y--;
					} while (second_y != 0u);
					second_x = third_x - left_x;
					second_y = third_y;
					segments_left--;
				} while (segments_left != 0u);
				fill_span_list_from_r8(row_count, fill_color);
				return;
			}
			span_address = top_y;
			third_x = divide_s32(third_x - left_x, third_y);
			row_count = third_y;
			third_y -= second_y;
			second_x -= left_x;
			do
			{
				second_x = divide_s32(second_x, second_y);
				do
				{
					left_x += third_x;
					right_x += second_x;
					left_end = shift_by(left_x, pixel_shift, 32);
					span_address -= prior_left - left_end;
					mem_write32(span_cursor, span_address);
					span_cursor += 4u;
					mem_write16(span_cursor, shift_by(right_x, pixel_shift, 32) - left_end);
					prior_left = left_end;
					span_address += r14;
					span_cursor += 2u;
					second_y--;
				} while (second_y != 0u);
				second_x = bottom_x - right_x;
				second_y = third_y;
				segments_left--;
			} while (segments_left != 0u);
			fill_span_list_from_r8(row_count, fill_color);
			return;
		}
		stack_pointer = stack_pointer + 0x10u;
		if (((uint8_t)(bottom_index - top_index) & 3u) != 2u)
		{
			second_y -= top_y;
			third_y = third_y - top_y + 1u;
			bottom_y = bottom_y - top_y + 1u;
			span_address = (uint32_t)(uint16_t)top_y << 9;
			span_address = ((span_address & ~0xFFFFu) | (uint16_t)(span_address + left_x)) + current_player;
			prior_left = left_x;
			left_x = left_x << 16;
			right_x = left_x;
			third_x = third_x << 16;
			bottom_x = bottom_x << 16;
			pixel_shift = 0xF0u;
			row_count = bottom_y;
			second_y++;
			second_x = divide_s32((second_x << 16) - left_x, second_y);
			right_side_slope = divide_s32(bottom_x - left_x, bottom_y);
			bend_on_right = 0u;
			if ((int32_t)right_side_slope < (int32_t)second_x)
			{
				swap_edge_slope = second_x;
				second_x = right_side_slope;
				right_side_slope = swap_edge_slope;
				bend_on_right++;
			}
			edge_parts_left = 3u;
			bottom_y -= third_y;
			third_rows = third_y;
			next_part_rows = third_rows - second_y;
			if (third_rows == second_y)
			{
				edge_parts_left = 2u;
				next_part_rows = bottom_y;
				third_x = bottom_x;
			}
			for (;;)
			{
				left_x += second_x;
				right_x += right_side_slope;
				current_player = left_x;
				current_player = shift_by(current_player, pixel_shift, 32);
				span_address -= prior_left - current_player;
				mem_write32(span_cursor, span_address);
				span_cursor += 4u;
				mem_write16(span_cursor, shift_by(right_x, pixel_shift, 32) - current_player);
				prior_left = current_player;
				span_address += r14;
				span_cursor += 2u;
				second_y--;
				if (second_y != 0u)
					continue;
				if (bend_on_right == 0u)
				{
					second_x = divide_s32(third_x - left_x, next_part_rows);
					second_y = next_part_rows;
					third_x = bottom_x;
					next_part_rows = bottom_y;
					edge_parts_left--;
					if (edge_parts_left == 0u)
					{
						fill_span_list_from_r8(row_count, fill_color);
						return;
					}
					continue;
				}
				right_side_slope = divide_s32(third_x - right_x, next_part_rows);
				second_y = next_part_rows;
				third_x = bottom_x;
				next_part_rows = bottom_y;
				edge_parts_left--;
				if (edge_parts_left != 0u)
					continue;
				break;
			}
			fill_span_list_from_r8(row_count, fill_color);
			return;
		}
		second_y -= top_y;
		third_y = third_y - top_y + 1u;
		bottom_y = bottom_y - top_y + 1u;
		span_address = top_y << 9;
		span_address = ((span_address & ~0xFFFFu) | (uint16_t)(span_address + left_x)) + current_player;
		row_count = bottom_y;
		prior_left = left_x;
		left_x = left_x << 16;
		bottom_x = bottom_x << 16;
		right_x = left_x;
		pixel_shift = 0xF0u;
		second_y++;
		second_x = divide_s32((second_x << 16) - left_x, second_y);
		third_x = divide_s32((third_x << 16) - left_x, third_y);
		bottom_y -= third_y;
		rows_after_second = row_count - second_y;
		segments_left = 2u;
		if ((int32_t)second_x <= (int32_t)third_x)
		{
			for (;;)
			{
				left_x += second_x;
				right_x += third_x;
				left_pixel = shift_by(left_x, pixel_shift, 32);
				span_address -= prior_left - left_pixel;
				mem_write32(span_cursor, span_address);
				span_cursor += 4u;
				mem_write16(span_cursor, shift_by(right_x, pixel_shift, 32) - left_pixel);
				prior_left = left_pixel;
				span_address += r14;
				span_cursor += 2u;
				second_y--;
				if (second_y != 0u)
					continue;
				segments_left--;
				if (segments_left == 0u)
					break;
				second_x = divide_s32(bottom_x - left_x, rows_after_second);
				rows_after_left_bend = rows_after_second;
				second_y = rows_after_left_bend - bottom_y;
				if (rows_after_left_bend == bottom_y)
					break;
			}
			right_slope = divide_s32(bottom_x - right_x, bottom_y);
		}
		else
		{
			swap_slope = second_x;
			left_slope = third_x;
			right_slope = swap_slope;
			for (;;)
			{
				left_x += left_slope;
				right_x += right_slope;
				left_pixel = shift_by(left_x, pixel_shift, 32);
				span_address -= prior_left - left_pixel;
				mem_write32(span_cursor, span_address);
				span_cursor += 4u;
				mem_write16(span_cursor, shift_by(right_x, pixel_shift, 32) - left_pixel);
				prior_left = left_pixel;
				span_address += r14;
				span_cursor += 2u;
				second_y--;
				if (second_y != 0u)
					continue;
				segments_left--;
				if (segments_left == 0u)
					break;
				right_slope = divide_s32(bottom_x - right_x, rows_after_second);
				rows_after_right_bend = rows_after_second;
				second_y = rows_after_right_bend - bottom_y;
				if (rows_after_right_bend == bottom_y)
					break;
			}
			second_x = divide_s32(bottom_x - left_x, bottom_y);
		}
		do
		{
			left_x += second_x;
			right_x += right_slope;
			left_pixel = shift_by(left_x, pixel_shift, 32);
			span_address -= prior_left - left_pixel;
			mem_write32(span_cursor, span_address);
			span_cursor += 4u;
			mem_write16(span_cursor, shift_by(right_x, pixel_shift, 32) - left_pixel);
			prior_left = left_pixel;
			span_address += r14;
			span_cursor += 2u;
			bottom_y--;
		} while (bottom_y != 0u);
		fill_span_list_from_r8(row_count, fill_color);
		return;
	}
	if (third_y != second_y)
	{
		if (bottom_y != third_y)
		{
			if ((int32_t)second_x < (int32_t)left_x)
			{
				swap_top_x = left_x;
				left_x = second_x;
				second_x = swap_top_x;
				top_index = (uint8_t)second_index;
			}
			bottom_y -= second_y;
			third_y -= second_y;
			top_y = (top_y << 9) + left_x + current_player;
			previous_left = left_x;
			right_column = (uint16_t)second_x;
			left_x = left_x << 16;
			second_x = second_x << 16;
			third_x = third_x << 16;
			bottom_x = bottom_x << 16;
			pixel_shift = 0xF0u;
			if (((uint8_t)(bottom_index - top_index) & 3u) != 2u)
			{
				span_address = top_y;
				third_x = divide_s32(third_x - second_x, third_y);
				single_edge_slope = divide_s32(bottom_x - left_x, bottom_y);
				row_count = bottom_y;
				segments_left = 2u;
				bottom_y -= third_y;
				for (;;)
				{
					mem_write32(span_cursor, span_address);
					span_cursor += 4u;
					mem_write16(span_cursor, right_column - previous_left);
					left_x += single_edge_slope;
					second_x += third_x;
					left_column = shift_by(left_x, pixel_shift, 32);
					right_column = (uint16_t)shift_by(second_x, pixel_shift, 32);
					span_address = span_address - (previous_left - left_column) + r14;
					previous_left = left_column;
					span_cursor += 2u;
					third_y--;
					if (third_y != 0u)
						continue;
					third_x = divide_s32(bottom_x - second_x, bottom_y);
					third_y = bottom_y;
					segments_left--;
					if (segments_left != 0u)
						continue;
					break;
				}
				stack_pointer = stack_pointer + 0x10u;
				fill_span_list_from_r8(row_count, fill_color);
				return;
			}
			span_address = top_y;
			third_x = divide_s32(third_x - left_x, third_y);
			single_edge_slope = divide_s32(bottom_x - second_x, bottom_y);
			third_y++;
			bottom_y++;
			row_count = bottom_y;
			segments_left = 2u;
			bottom_y -= third_y;
			for (;;)
			{
				mem_write32(span_cursor, span_address);
				span_cursor += 4u;
				mem_write16(span_cursor, right_column - previous_left);
				left_x += third_x;
				second_x += single_edge_slope;
				left_column = shift_by(left_x, pixel_shift, 32);
				right_column = (uint16_t)shift_by(second_x, pixel_shift, 32);
				span_address = span_address - (previous_left - left_column) + r14;
				previous_left = left_column;
				span_cursor += 2u;
				third_y--;
				if (third_y != 0u)
					continue;
				third_x = divide_s32(bottom_x - left_x, bottom_y);
				third_y = bottom_y;
				segments_left--;
				if (segments_left != 0u)
					continue;
				break;
			}
			stack_pointer = stack_pointer + 0x10u;
			fill_span_list_from_r8(row_count, fill_color);
			return;
		}
		if ((int32_t)second_x < (int32_t)left_x)
		{
			swap_upper_x = left_x;
			left_x = second_x;
			second_x = swap_upper_x;
		}
		if ((int32_t)bottom_x < (int32_t)third_x)
		{
			swap_lower_x = third_x;
			third_x = bottom_x;
			bottom_x = swap_lower_x;
		}
		span_address = (top_y << 9) + left_x + current_player;
		previous_left = left_x;
		right_column = (uint16_t)second_x;
		left_x = left_x << 16;
		second_x = second_x << 16;
		third_y -= second_y;
		third_x = divide_s32((third_x << 16) - left_x, third_y);
		bottom_x = divide_s32((bottom_x << 16) - second_x, third_y);
		pixel_shift = 0xF0u;
		third_y++;
		row_count = third_y;
		stack_pointer = stack_pointer + 0x10u;
		do
		{
			mem_write32(span_cursor, span_address);
			span_cursor += 4u;
			mem_write16(span_cursor, right_column - previous_left);
			left_x += third_x;
			second_x += bottom_x;
			left_column = shift_by(left_x, pixel_shift, 32);
			right_column = (uint16_t)shift_by(second_x, pixel_shift, 32);
			span_address = span_address - (previous_left - left_column) + r14;
			previous_left = left_column;
			span_cursor += 2u;
			third_y--;
		} while (third_y != 0u);
		fill_span_list_from_r8(row_count, fill_color);
		return;
	}
	if (bottom_y != third_y)
	{
		vertex_index = bottom_index & 3u;
		bottom_vertex = stack_pointer + vertex_index * 4u;
		apex_x = (int16_t)mem_read16(bottom_vertex);
		triangle_height = (int16_t)mem_read16(bottom_vertex + 2u);
		vertex_index = (uint8_t)(vertex_index + 1u);
		vertex_index = vertex_index & 3u;
		triangle_right_x = (int16_t)mem_read16(stack_pointer + vertex_index * 4u);
		vertex_index = (uint8_t)(vertex_index + 2u);
		vertex_index = stack_pointer + (vertex_index & 3u) * 4u;
		triangle_left_x = (int16_t)mem_read16(vertex_index);
		triangle_top_y = (int16_t)mem_read16(vertex_index + 2u);
		triangle_height -= triangle_top_y;
		triangle_color = (uint8_t)fill_color;
		stack_pointer = stack_pointer + 0x10u;
		if ((int32_t)triangle_left_x <= (int32_t)triangle_right_x)
		{
			fill_flat_top_triangle(triangle_left_x, triangle_top_y, triangle_right_x, apex_x, triangle_height, triangle_color, span_cursor);
			return;
		}
		swap_triangle_x = triangle_left_x;
		triangle_left_x = triangle_right_x;
		triangle_right_x = swap_triangle_x;
		fill_flat_top_triangle(triangle_left_x, triangle_top_y, triangle_right_x, apex_x, triangle_height, triangle_color, span_cursor);
		return;
	}
	if ((int32_t)second_x < (int32_t)left_x)
		left_x = second_x;
	if ((int32_t)third_x < (int32_t)left_x)
		left_x = third_x;
	if ((int32_t)bottom_x < (int32_t)left_x)
		left_x = bottom_x;
	if ((int32_t)bottom_x < (int32_t)second_x)
		bottom_x = second_x;
	if ((int32_t)bottom_x < (int32_t)third_x)
		bottom_x = third_x;
	top_y = top_y << 9;
	top_y = ((top_y & ~0xFFFFu) | (uint16_t)(top_y + left_x)) + current_player;
	length = bottom_x - left_x;
	color = (uint8_t)fill_color;
	stack_pointer = stack_pointer + 0x10u;
	fill_span_8bpp(length, &left_x, &top_y, &color);
}

void draw_circle_polygon(uint32_t center_x, uint32_t center_y, uint32_t radius, uint8_t circle_color, uint32_t color)
{
	uint8_t sine_shift = 0;
	uint16_t coordinate = 0, cos_offset = 0, end_x = 0, end_y = 0, sin_offset = 0;
	uint32_t angle = 0, angle_step = 0, lines_left = 0, mirror_cursor = 0, point_cursor = 0, points = 0, sine_entry = 0, sines = 0, start_x = 0, start_y = 0, steps_left = 0, x_cursor = 0, y_cursor = 0;

	if ((int16_t)radius < 0)
		radius = (uint16_t)(0u - radius);
	radius = (uint16_t)radius;
	color = (color & ~0xFFu) | (uint8_t)circle_color;
	angle = 0x100000u;
	angle_step = 0x200000u;
	sines = sine_table_ADDRESS;
	points = bitmap_draw_buffer_ADDRESS;
	point_cursor = bitmap_draw_buffer_ADDRESS;
	mirror_cursor = 0x20B600u;
	steps_left = 8u;
	sine_shift = 0xF2u;
	do
	{
		sine_entry = angle;
		angle += angle_step;
		sine_entry = sines + (uint8_t)((sine_entry << 15) | (sine_entry >> 17)) * 2u;
		cos_offset = (uint16_t)shift_by(ROM16(sine_entry + 0x200u) * radius, sine_shift, 32);
		coordinate = (uint16_t)(center_x + cos_offset);
		mem_write16(point_cursor, coordinate);
		mem_write16(mirror_cursor - 4u, coordinate);
		coordinate = (uint16_t)(center_y + cos_offset);
		point_cursor += 2u;
		mem_write16(point_cursor + 0x40u, coordinate);
		mem_write16(mirror_cursor - 0xC2u, coordinate);
		cos_offset = (uint16_t)(0u - cos_offset);
		coordinate = (uint16_t)(center_x + cos_offset);
		mem_write16(point_cursor + 0x7Eu, coordinate);
		mem_write16(mirror_cursor - 0x84u, coordinate);
		coordinate = (uint16_t)(center_y + cos_offset);
		mem_write16(point_cursor + 0xC0u, coordinate);
		mem_write16(mirror_cursor - 0x42u, coordinate);
		sin_offset = (uint16_t)shift_by(ROM16(sine_entry) * radius, sine_shift, 32);
		coordinate = (uint16_t)(center_y + sin_offset);
		mem_write16(point_cursor, coordinate);
		mem_write16(mirror_cursor + 0x7Eu, coordinate);
		coordinate = (uint16_t)(center_x + sin_offset);
		point_cursor += 2u;
		mem_write16(point_cursor + 0xBCu, coordinate);
		mem_write16(mirror_cursor - 0xC4u, coordinate);
		sin_offset = (uint16_t)(0u - sin_offset);
		coordinate = (uint16_t)(center_x + sin_offset);
		mem_write16(point_cursor + 0x3Cu, coordinate);
		mem_write16(mirror_cursor - 0x44u, coordinate);
		coordinate = (uint16_t)(center_y + sin_offset);
		mem_write16(point_cursor + 0x7Fu, coordinate);
		mirror_cursor -= 2u;
		mem_write16(mirror_cursor, coordinate);
		steps_left--;
		mirror_cursor -= 2u;
	} while (steps_left != 0u);
	y_cursor = points;
	x_cursor = y_cursor;
	lines_left = 0x3Fu;
	y_cursor += 2u;
	do
	{
		center_x = (center_x & ~0xFFFFu) | mem_read16(x_cursor);
		center_y = (center_y & ~0xFFFFu) | mem_read16(y_cursor);
		x_cursor += 2u;
		end_x = mem_read16(x_cursor);
		y_cursor += 2u;
		end_y = mem_read16(y_cursor);
		draw_line(end_x, end_y, &center_x, &center_y, &color, &angle);
		lines_left--;
	} while (lines_left != 0u);
	start_x = (center_x & ~0xFFFFu) | AS16(bitmap_draw_buffer);
	start_y = (center_y & ~0xFFFFu) | AT16(bitmap_draw_buffer, 2);
	end_x = mem_read16(x_cursor);
	end_y = mem_read16(y_cursor);
	draw_line(end_x, end_y, &start_x, &start_y, &color, &angle);
}

void draw_circle(uint16_t x, uint16_t y, uint16_t radius, uint8_t color)
{
	uint8_t high_color = 0;
	uint16_t circle_bottom = 0, circle_left = 0, circle_right = 0, circle_top = 0, minus_radius = 0, octant_mask = 0, window_bottom = 0, window_right = 0, window_top = 0;
	uint32_t bitmap_base = 0, center_x = 0, center_y = 0, error = 0, error_step = 0, minus_x_offset = 0, minus_y_offset = 0, point_x = 0, point_y = 0, x_offset = 0, y_offset = 0;
	uint32_t error_before_8bpp = 0, error_before_4bpp = 0;

	if ((int16_t)radius < 0)
		radius = (uint16_t)(0u - radius);
	error = (uint16_t)radius;
	x_offset = error;
	y_offset = 0u;
	center_x = (int16_t)x;
	center_y = (int16_t)y;
	bitmap_base = bitmap_address;
	r14 = (uint16_t)bitmap_window_left;
	window_top = (uint16_t)bitmap_window_top;
	window_right = (uint16_t)bitmap_window_right;
	window_bottom = (uint16_t)bitmap_window_bottom;
	octant_mask = 0xFFFFu;
	circle_right = (uint16_t)(center_x + x_offset);
	if ((int16_t)circle_right < (int16_t)r14)
		return;
	minus_radius = (uint16_t)(0u - x_offset);
	circle_left = (uint16_t)(center_x + minus_radius);
	if ((int16_t)window_right < (int16_t)circle_left)
		return;
	if ((int16_t)window_right < (int16_t)circle_right)
		octant_mask &= 0x5FF5u;
	if ((int16_t)circle_left < (int16_t)r14)
		octant_mask &= ~0xAA0u;
	circle_bottom = (uint16_t)(center_y + x_offset);
	if ((int16_t)circle_bottom < (int16_t)window_top)
		return;
	circle_top = (uint16_t)(center_y + minus_radius);
	if ((int16_t)window_bottom < (int16_t)circle_top)
		return;
	if ((int16_t)window_bottom < (int16_t)circle_bottom)
		octant_mask &= 0x55FFu;
	if ((int16_t)circle_top < (int16_t)window_top)
		octant_mask = (uint16_t)((octant_mask & ~0xFFu) | (octant_mask & 0x55u));
	if ((int16_t)center_x < (int16_t)r14)
		octant_mask &= ~0x550u;
	if ((int16_t)window_right < (int16_t)center_x)
		octant_mask &= 0xAFFAu;
	if ((int16_t)center_y < (int16_t)window_top)
		octant_mask = (uint16_t)((octant_mask & ~0xFFu) | (octant_mask & 0xAAu));
	if ((int16_t)window_bottom < (int16_t)center_y)
		octant_mask &= 0xAAFFu;
	if (bitmap_8bpp != 0u)
	{
		while ((int32_t)y_offset <= (int32_t)x_offset)
		{
			current_player = (current_player & ~0xFFFFu) | octant_mask;
			minus_x_offset = 0u - x_offset;
			minus_y_offset = 0u - y_offset;
			point_x = center_x + x_offset;
			point_y = center_y + y_offset;
			plot_circle_point_8bpp(point_x, point_y, color, bitmap_base, window_top, window_right, window_bottom);
			point_x = center_x + y_offset;
			point_y = center_y + x_offset;
			plot_circle_point_8bpp(point_x, point_y, color, bitmap_base, window_top, window_right, window_bottom);
			point_x = center_x + minus_y_offset;
			point_y = center_y + x_offset;
			plot_circle_point_8bpp(point_x, point_y, color, bitmap_base, window_top, window_right, window_bottom);
			point_x = center_x + minus_x_offset;
			point_y = center_y + y_offset;
			plot_circle_point_8bpp(point_x, point_y, color, bitmap_base, window_top, window_right, window_bottom);
			point_x = center_x + x_offset;
			point_y = center_y + minus_y_offset;
			plot_circle_point_8bpp(point_x, point_y, color, bitmap_base, window_top, window_right, window_bottom);
			point_x = center_x + minus_y_offset;
			point_y = center_y + minus_x_offset;
			plot_circle_point_8bpp(point_x, point_y, color, bitmap_base, window_top, window_right, window_bottom);
			point_x = center_x + minus_x_offset;
			point_y = center_y + minus_y_offset;
			plot_circle_point_8bpp(point_x, point_y, color, bitmap_base, window_top, window_right, window_bottom);
			point_x = center_x + y_offset;
			point_y = center_y + minus_x_offset;
			plot_circle_point_8bpp(point_x, point_y, color, bitmap_base, window_top, window_right, window_bottom);
			error_step = y_offset;
			y_offset++;
			error_step += y_offset;
			error_before_8bpp = error;
			error = error_before_8bpp - error_step;
			if ((int32_t)error_step <= (int32_t)error_before_8bpp)
				continue;
			x_offset--;
			error += x_offset * 2u;
		}
		return;
	}
	high_color = (uint8_t)color;
	color &= 0xFu;
	high_color = (uint8_t)((uint32_t)high_color << 3);
	while ((int32_t)y_offset <= (int32_t)x_offset)
	{
		current_player = (current_player & ~0xFFFFu) | octant_mask;
		minus_x_offset = 0u - x_offset;
		minus_y_offset = 0u - y_offset;
		point_x = center_x + x_offset;
		point_y = center_y + y_offset;
		plot_circle_point_4bpp(point_x, point_y, high_color, color, bitmap_base, window_top, window_right, window_bottom);
		point_x = center_x + y_offset;
		point_y = center_y + x_offset;
		plot_circle_point_4bpp(point_x, point_y, high_color, color, bitmap_base, window_top, window_right, window_bottom);
		point_x = center_x + minus_y_offset;
		point_y = center_y + x_offset;
		plot_circle_point_4bpp(point_x, point_y, high_color, color, bitmap_base, window_top, window_right, window_bottom);
		point_x = center_x + minus_x_offset;
		point_y = center_y + y_offset;
		plot_circle_point_4bpp(point_x, point_y, high_color, color, bitmap_base, window_top, window_right, window_bottom);
		point_x = center_x + x_offset;
		point_y = center_y + minus_y_offset;
		plot_circle_point_4bpp(point_x, point_y, high_color, color, bitmap_base, window_top, window_right, window_bottom);
		point_x = center_x + minus_y_offset;
		point_y = center_y + minus_x_offset;
		plot_circle_point_4bpp(point_x, point_y, high_color, color, bitmap_base, window_top, window_right, window_bottom);
		point_x = center_x + minus_x_offset;
		point_y = center_y + minus_y_offset;
		plot_circle_point_4bpp(point_x, point_y, high_color, color, bitmap_base, window_top, window_right, window_bottom);
		point_x = center_x + y_offset;
		point_y = center_y + minus_x_offset;
		plot_circle_point_4bpp(point_x, point_y, high_color, color, bitmap_base, window_top, window_right, window_bottom);
		error_step = y_offset;
		y_offset++;
		error_step += y_offset;
		error_before_4bpp = error;
		error = error_before_4bpp - error_step;
		if ((int32_t)error_step <= (int32_t)error_before_4bpp)
			continue;
		x_offset--;
		error += x_offset * 2u;
	}
}

void plot_circle_point_4bpp(uint32_t point_x, uint32_t point_y, uint8_t high_color, uint8_t low_color, uint32_t bitmap_base, uint16_t window_top, uint16_t window_right, uint16_t window_bottom)
{
	uint32_t negative = 0, carry = 0;
	uint16_t shifted_mask = 0;

	shifted_mask = current_player * 2u;
	carry = (uint16_t)(current_player * 2u) < (uint16_t)current_player;
	negative = shifted_mask >> 15;
	current_player = (current_player & ~0xFFFFu) | shifted_mask;
	if (carry == 0u && (negative == 0u || (int16_t)point_x < (int16_t)r14 || (int16_t)point_y < (int16_t)window_top || (int16_t)window_right < (int16_t)point_x || (int16_t)window_bottom < (int16_t)point_y))
	{
		current_player = (current_player & ~0xFFFFu) | (uint16_t)(current_player * 2u);
		return;
	}
	point_y = (point_y << 8) + ((int32_t)point_x >> 1) + bitmap_base;
	if ((point_x & 1u) != 0u)
	{
		mem_write8(point_y, (mem_read8(point_y) & 0xFu) | high_color);
		current_player = (current_player & ~0xFFFFu) | (uint16_t)(current_player * 2u);
		return;
	}
	mem_write8(point_y, (0xF0u & mem_read8(point_y)) | low_color);
	current_player = (current_player & ~0xFFFFu) | (uint16_t)(current_player * 2u);
}

void plot_circle_point_8bpp(uint32_t point_x, uint32_t point_y, uint8_t color, uint32_t bitmap_base, uint16_t window_top, uint16_t window_right, uint16_t window_bottom)
{
	uint32_t negative = 0, carry = 0;
	uint16_t shifted_mask = 0;

	shifted_mask = current_player * 2u;
	carry = (uint16_t)(current_player * 2u) < (uint16_t)current_player;
	negative = shifted_mask >> 15;
	current_player = (current_player & ~0xFFFFu) | shifted_mask;
	if (carry || (negative && (int16_t)r14 <= (int16_t)point_x && (int16_t)window_top <= (int16_t)point_y && (int16_t)point_x <= (int16_t)window_right && (int16_t)point_y <= (int16_t)window_bottom))
		mem_write8((point_y << 9) + point_x + bitmap_base, color);
	current_player = (current_player & ~0xFFFFu) | (uint16_t)(current_player * 2u);
}

void flood_fill(uint16_t seed_x, uint16_t seed_y, uint8_t fill_color, uint8_t border_color)
{
	uint8_t seed_pixel = 0;
	uint16_t span_y = 0, window_bottom = 0, window_top = 0;
	uint32_t fill_pattern = 0, span_left_x = 0, span_right_address = 0, span_right_x = 0, window_left = 0, window_right = 0;
	uint32_t saved_object_cursor = 0, saved_last_object = 0;

	window_left = (uint16_t)bitmap_window_left;
	if ((int16_t)seed_x < (int16_t)window_left)
		return;
	window_top = (uint16_t)bitmap_window_top;
	if ((int16_t)seed_y < (int16_t)window_top)
		return;
	window_right = (uint16_t)bitmap_window_right;
	if ((int16_t)window_right < (int16_t)seed_x)
		return;
	window_bottom = (uint16_t)bitmap_window_bottom;
	if ((int16_t)window_bottom < (int16_t)seed_y)
		return;
	saved_object_cursor = object_cursor;
	saved_last_object = last_object;
	r14 = (uint32_t)(uint16_t)seed_y << 9;
	r14 = ((r14 & ~0xFFFFu) | (uint16_t)(r14 + seed_x)) + bitmap_address;
	seed_pixel = mem_read8(r14);
	if (seed_pixel == (uint8_t)fill_color || seed_pixel == (uint8_t)border_color)
	{
		last_object = saved_last_object;
		object_cursor = saved_object_cursor;
		return;
	}
	object_cursor = bitmap_draw_buffer_ADDRESS;
	last_object = bitmap_draw_buffer_ADDRESS;
	span_left_x = (uint16_t)seed_x;
	span_y = (uint8_t)seed_y;
	fill_pattern = (uint8_t)fill_color * 0x1010101u;
	flood_fill_span(border_color, window_left, window_right, fill_pattern, &span_left_x, &span_right_address, &span_right_x);
	flood_fill_y = (uint8_t)span_y;
	flood_fill_left_x = span_left_x;
	flood_fill_left_address = r14;
	flood_fill_right_x = span_right_x;
	flood_fill_right_address = span_right_address;
	span_left_x = flood_fill_scan_seed_rows(border_color, window_left, window_top, window_right, window_bottom, span_left_x, span_y, fill_pattern);
	while (last_object != object_cursor)
	{
		AS16(flood_fill_direction) = mem_read16(last_object);
		last_object = last_object + 2u;
		flood_fill_right_x = OBJECT(last_object)->draw_next;
		last_object = last_object + 2u;
		flood_fill_right_address = AS32(OBJECT(last_object - 0x2u)->sprite_flags);
		last_object = last_object + 4u;
		flood_fill_left_x = OBJECT(last_object - 0x6u)->sprite_palette;
		last_object = last_object + 2u;
		flood_fill_left_address = AS32(OBJECT(last_object - 0x8u)->sprite_scale);
		last_object = last_object + 4u;
		flood_fill_parent_right_x = OBJECT(last_object - 0xCu)->screen_y;
		last_object = last_object + 2u;
		flood_fill_parent_right_address = AS32(OBJECT(last_object - 0xEu)->screen_x);
		last_object = last_object + 4u;
		flood_fill_parent_left_x = OBJECT(last_object - 0x12u)->hitbox_size_y;
		last_object = last_object + 2u;
		flood_fill_parent_left_address = AS32(OBJECT(last_object - 0x14u)->anchor_x);
		last_object = last_object + 4u;
		if ((uint16_t)last_object >= 0xCCC4u)
			last_object = bitmap_draw_buffer_ADDRESS;
		span_y = flood_fill_y;
		span_left_x = flood_fill_scan_span_rows(border_color, window_left, window_top, window_right, window_bottom, span_left_x, span_y, fill_pattern);
	}
	last_object = saved_last_object;
	object_cursor = saved_object_cursor;
}

void flood_fill_push_span(uint8_t border_color, uint32_t window_left, uint32_t window_right, uint8_t direction, uint32_t left_x, uint8_t y, uint32_t fill_pattern, uint32_t *p_right_address, uint32_t *p_right_x)
{
	uint32_t right_address = 0, right_x = 0;

	flood_fill_span(border_color, window_left, window_right, fill_pattern, &left_x, &right_address, &right_x);
	OBJECT(object_cursor)->flood_fill.direction_and_y = (uint16_t)(((uint32_t)y + direction) << 8) | (uint8_t)direction;
	object_cursor = object_cursor + 2u;
	OBJECT(object_cursor - 0x2u)->sprite_flags = right_x;
	object_cursor = object_cursor + 2u;
	OBJECT(object_cursor - 0x4u)->part_list = right_address;
	object_cursor = object_cursor + 4u;
	OBJECT(object_cursor - 0x8u)->sprite_scale = left_x;
	object_cursor = object_cursor + 2u;
	AS32(OBJECT(object_cursor - 0xAu)->sprite_layer) = r14;
	object_cursor = object_cursor + 4u;
	OBJECT(object_cursor - 0xEu)->screen_x = flood_fill_push_parent_right_x;
	object_cursor = object_cursor + 2u;
	AS32(OBJECT(object_cursor - 0x10u)->hitbox_size_x) = flood_fill_push_parent_right_address;
	object_cursor = object_cursor + 4u;
	OBJECT(object_cursor - 0x14u)->anchor_x = flood_fill_push_parent_left_x;
	object_cursor = object_cursor + 2u;
	AS32(OBJECT(object_cursor - 0x16u)->anchor_y) = flood_fill_push_parent_left_address;
	object_cursor = object_cursor + 4u;
	if ((uint16_t)object_cursor < 0xCCC4u)
	{
		*p_right_address = right_address;
		*p_right_x = right_x;
		return;
	}
	object_cursor = bitmap_draw_buffer_ADDRESS;
	*p_right_address = right_address;
	*p_right_x = right_x;
}

void flood_fill_span(uint8_t border_color, uint32_t window_left, uint32_t window_right, uint32_t fill_pattern, uint32_t *p_left_x, uint32_t *p_right_address, uint32_t *p_right_x)
{
	uint32_t left_x = *p_left_x;
	uint32_t zero = 0;
	uint8_t last_pixel = 0, pixel_count = 0, tail_bytes = 0;
	uint16_t pixels = 0;
	uint32_t blocks_left = 0, bytes_left = 0, fill_count = 0, right_address = 0, right_cursor = 0, right_x = 0, scan_count = 0, seed_address = 0;
	uint16_t right_limit = 0;
	uint32_t start_x = 0, count_after_first = 0, block_count = 0, tail_count = 0;

	seed_address = r14;
	right_x = left_x;
	start_x = left_x;
	scan_count = start_x - window_left;
	if ((int32_t)window_left < (int32_t)start_x)
	{
		pixel_count = (uint8_t)(uint16_t)scan_count;
		scan_count = (scan_count & ~0xFFFFu) | ((uint16_t)scan_count >> 1);
		if ((r14 & 1u) != 0u)
		{
			pixel_count--;
			scan_count = (scan_count & ~0xFFFFu) | (uint16_t)(scan_count + 1u);
			r14 = r14 - 1u;
			pixels = mem_read8(r14);
			goto flood_fill_span_left_pixel;
		}
	flood_fill_span_left_pair:
		r14 = r14 - 2u;
		pixels = mem_read16(r14);
		pixels = (uint16_t)(((uint32_t)pixels << 8) | (pixels >> 8));
		if ((uint8_t)pixels == (uint8_t)fill_pattern || (uint8_t)pixels == (uint8_t)border_color)
		{
			r14 = r14 + 1u;
		flood_fill_span_left_edge:
			r14 = r14 + 1u;
		}
		else
		{
			pixels = (uint16_t)(((uint32_t)pixels << 8) | (pixels >> 8));
		flood_fill_span_left_pixel:
			if ((uint8_t)pixels == (uint8_t)fill_pattern)
				goto flood_fill_span_left_edge;
			scan_count--;
			if ((scan_count != 0u) & ((uint8_t)pixels != (uint8_t)border_color))
				goto flood_fill_span_left_pair;
			if ((uint8_t)pixels == (uint8_t)border_color)
				goto flood_fill_span_left_edge;
			if ((pixel_count & 1u) != 0u)
			{
				r14 = r14 - 1u;
				last_pixel = mem_read8(r14);
				if (last_pixel == (uint8_t)fill_pattern || last_pixel == (uint8_t)border_color)
					goto flood_fill_span_left_edge;
			}
		}
		left_x = (left_x & ~0xFFFFu) | (uint16_t)(left_x - (seed_address - r14));
	}
	scan_count = window_right;
	right_limit = scan_count;
	scan_count = (scan_count & ~0xFFFFu) | (uint16_t)(right_limit - right_x);
	if ((int16_t)right_x < (int16_t)right_limit)
	{
		right_cursor = seed_address + 1u;
		pixel_count = (uint8_t)scan_count;
		scan_count = (scan_count & ~0xFFFFu) | ((uint16_t)scan_count >> 1);
		if ((right_cursor & 1u) != 0u)
		{
			pixel_count--;
			scan_count = (scan_count & ~0xFFFFu) | (uint16_t)(scan_count + 1u);
			pixels = mem_read8(right_cursor);
			right_cursor++;
			goto flood_fill_span_right_pixel;
		}
	flood_fill_span_right_pair:
		pixels = mem_read16(right_cursor);
		right_cursor += 2u;
		if ((uint8_t)pixels == (uint8_t)fill_pattern || (uint8_t)pixels == (uint8_t)border_color)
		{
			right_cursor--;
		flood_fill_span_right_edge:
			right_cursor--;
		flood_fill_span_right_end:
			right_cursor--;
		}
		else
		{
			pixels = (uint16_t)(((uint32_t)pixels << 8) | (pixels >> 8));
		flood_fill_span_right_pixel:
			if ((uint8_t)pixels == (uint8_t)fill_pattern)
				goto flood_fill_span_right_edge;
			scan_count--;
			if ((scan_count != 0u) & ((uint8_t)pixels != (uint8_t)border_color))
				goto flood_fill_span_right_pair;
			if ((uint8_t)pixels == (uint8_t)border_color)
				goto flood_fill_span_right_edge;
			if ((pixel_count & 1u) == 0u)
				goto flood_fill_span_right_end;
			last_pixel = mem_read8(right_cursor);
			if (last_pixel == (uint8_t)fill_pattern || last_pixel == (uint8_t)border_color)
				goto flood_fill_span_right_end;
		}
		right_x = (right_x & ~0xFFFFu) | (uint16_t)(right_x + (right_cursor - seed_address));
	}
	right_address = r14;
	fill_count = right_x - left_x;
	if ((right_address & 1u) != 0u)
	{
		mem_write8(right_address, fill_pattern);
		count_after_first = fill_count - 1u;
		fill_count = count_after_first;
		right_address++;
		if ((int32_t)count_after_first < 0)
		{
			right_address--;
			*p_left_x = (*p_left_x & 0xFFFF0000u) | (left_x & 0xFFFFu);
			*p_right_address = right_address;
			*p_right_x = right_x;
			return;
		}
	}
	fill_count++;
	bytes_left = fill_count;
	block_count = fill_count >> 4;
	blocks_left = block_count;
	if (block_count != 0u)
	{
		do
		{
			mem_write32(right_address, fill_pattern);
			right_address += 4u;
			mem_write32(right_address, fill_pattern);
			right_address += 4u;
			mem_write32(right_address, fill_pattern);
			right_address += 4u;
			mem_write32(right_address, fill_pattern);
			blocks_left--;
			right_address += 4u;
		} while (blocks_left != 0u);
	}
	bytes_left &= 0xFu;
	if (bytes_left >= 8u)
	{
		mem_write32(right_address, fill_pattern);
		right_address += 4u;
		mem_write32(right_address, fill_pattern);
		bytes_left -= 8u;
		right_address += 4u;
	}
	if (bytes_left >= 4u)
	{
		mem_write32(right_address, fill_pattern);
		right_address += 4u;
	}
	tail_count = bytes_left & 3u;
	tail_bytes = (uint8_t)tail_count;
	if (tail_count == 0u)
	{
		right_address--;
		*p_left_x = (*p_left_x & 0xFFFF0000u) | (left_x & 0xFFFFu);
		*p_right_address = right_address;
		*p_right_x = right_x;
		return;
	}
	zero = (uint8_t)(tail_bytes - 2u) == 0u;
	if (tail_bytes >= 2u)
	{
		mem_write16(right_address, fill_pattern);
		right_address += 2u;
		if (zero)
		{
			right_address--;
			*p_left_x = (*p_left_x & 0xFFFF0000u) | (left_x & 0xFFFFu);
			*p_right_address = right_address;
			*p_right_x = right_x;
			return;
		}
	}
	mem_write8(right_address, fill_pattern);
	right_address++;
	right_address--;
	*p_left_x = (*p_left_x & 0xFFFF0000u) | (left_x & 0xFFFFu);
	*p_right_address = right_address;
	*p_right_x = right_x;
}

uint32_t flood_fill_scan_seed_rows(uint8_t border_color, uint32_t window_left, uint16_t window_top, uint32_t window_right, uint16_t window_bottom, uint32_t scan_x, uint16_t y, uint32_t fill_pattern)
{
	uint32_t direction = 0;

	if ((int16_t)window_top < (int16_t)y)
	{
		direction = 0xFFFFFFFFu;
		scan_x = flood_fill_scan_row(border_color, window_left, window_right, direction, (uint8_t)y, fill_pattern);
	}
	if ((int16_t)window_bottom <= (int16_t)y)
		return scan_x;
	direction = 1u;
	return flood_fill_scan_row(border_color, window_left, window_right, direction, (uint8_t)y, fill_pattern);
}

uint32_t flood_fill_scan_row(uint8_t border_color, uint32_t window_left, uint32_t window_right, uint32_t direction, uint8_t y, uint32_t fill_pattern)
{
	uint8_t pixel = 0, pixel_count = 0;
	uint16_t pixels = 0;
	uint32_t right_address = 0, right_x = 0, scan_count = 0, scan_start = 0, scan_x = 0;
	uint16_t count_after_first = 0;

	scan_x = flood_fill_left_x;
	r14 = flood_fill_left_address + (direction << 9);
	flood_fill_push_parent_right_x = flood_fill_right_x;
	scan_count = flood_fill_push_parent_right_x;
	flood_fill_push_parent_right_address = flood_fill_right_address;
	flood_fill_push_parent_left_x = flood_fill_left_x;
	flood_fill_push_parent_left_address = flood_fill_left_address;
	for (;;)
	{
		scan_start = r14;
		scan_count = (uint16_t)(scan_count - scan_x);
		if ((r14 & 1u) != 0u)
		{
			pixel = mem_read8(r14);
			if (pixel != (uint8_t)fill_pattern && pixel != (uint8_t)border_color)
			{
			flood_fill_scan_row_push:
				flood_fill_push_span(border_color, window_left, window_right, (uint8_t)direction, scan_x, y, fill_pattern, &right_address, &right_x);
				scan_count = flood_fill_push_parent_right_x;
				scan_count = (uint16_t)(scan_count - 1u);
				if ((int16_t)scan_count <= (int16_t)right_x)
					break;
				scan_x = right_x + 2u;
				r14 = right_address + 2u;
				flood_fill_push_parent_left_x = scan_x;
				flood_fill_push_parent_left_address = r14;
				scan_count = flood_fill_push_parent_right_x;
				continue;
			}
			count_after_first = scan_count - 1u;
			scan_count = count_after_first;
			if ((int16_t)count_after_first < 0)
				break;
			r14 = (r14 & ~0xFFFFu) | (uint16_t)(r14 + 1u);
		}
		if (scan_count == 0u)
		{
		flood_fill_scan_row_odd_pixel:
			pixel = mem_read8(r14);
			if (pixel == (uint8_t)fill_pattern || pixel == (uint8_t)border_color)
				break;
		flood_fill_scan_row_found:
			scan_x = scan_x + r14 - scan_start;
			goto flood_fill_scan_row_push;
		}
		scan_count = (uint16_t)(scan_count + 1u);
		pixel_count = (uint8_t)scan_count;
		scan_count = scan_count >> 1;
		for (;;)
		{
			pixels = mem_read16(r14);
			r14 = r14 + 2u;
			if ((uint8_t)pixels != (uint8_t)fill_pattern && (uint8_t)pixels != (uint8_t)border_color)
				break;
			pixels = (uint16_t)(((uint32_t)pixels << 8) | (pixels >> 8));
			if ((uint8_t)pixels != (uint8_t)fill_pattern && (uint8_t)pixels != (uint8_t)border_color)
				goto flood_fill_scan_row_back_to_pixel;
			scan_count--;
			if (scan_count != 0u)
				continue;
			goto flood_fill_scan_row_pairs_done;
		}
		r14 = (r14 & ~0xFFFFu) | (uint16_t)(r14 - 1u);
	flood_fill_scan_row_back_to_pixel:
		r14 = (r14 & ~0xFFFFu) | (uint16_t)(r14 - 1u);
		goto flood_fill_scan_row_found;
	}
	return scan_x;
flood_fill_scan_row_pairs_done:
	if ((pixel_count & 1u) == 0u)
		return scan_x;
	goto flood_fill_scan_row_odd_pixel;
}

uint32_t flood_fill_scan_span_rows(uint8_t border_color, uint32_t window_left, uint16_t window_top, uint32_t window_right, uint16_t window_bottom, uint32_t scan_x, uint16_t y, uint32_t fill_pattern)
{
	uint32_t direction = 0;

	if ((int16_t)window_top < (int16_t)y)
	{
		direction = 0xFFFFFFFFu;
		scan_x = flood_fill_scan_row_skip_parent(border_color, window_left, window_right, direction, scan_x, (uint8_t)y, fill_pattern);
	}
	if ((int16_t)window_bottom <= (int16_t)y)
		return scan_x;
	direction = 1u;
	return flood_fill_scan_row_skip_parent(border_color, window_left, window_right, direction, scan_x, (uint8_t)y, fill_pattern);
}

uint32_t flood_fill_scan_row_skip_parent(uint8_t border_color, uint32_t window_left, uint32_t window_right, uint32_t direction, uint32_t scan_x, uint8_t y, uint32_t fill_pattern)
{
	uint8_t pixel = 0, pixel_count = 0;
	uint16_t pixels = 0;
	uint32_t right_address = 0, right_x = 0, scan_count = 0, scan_start = 0;
	uint16_t left_pairs = 0, left_count_before = 0, span_right_x = 0, right_limit = 0, right_count_before = 0, right_pairs = 0;

	if ((uint8_t)direction == flood_fill_direction)
		return flood_fill_scan_row(border_color, window_left, window_right, direction, y, fill_pattern);
	flood_fill_push_parent_right_x = flood_fill_right_x;
	flood_fill_push_parent_right_address = flood_fill_right_address;
	flood_fill_push_parent_left_x = flood_fill_left_x;
	flood_fill_push_parent_left_address = flood_fill_left_address;
	scan_x = (scan_x & ~0xFFFFu) | flood_fill_left_x;
	if ((uint16_t)scan_x < flood_fill_parent_left_x)
	{
		r14 = flood_fill_left_address + (direction << 9);
		for (;;)
		{
			scan_count = flood_fill_parent_left_x;
			scan_start = r14;
			scan_count = (uint16_t)(scan_count - scan_x);
			if ((r14 & 1u) == 0u)
			{
			flood_fill_skip_parent_left_pairs:
				pixel_count = (uint8_t)scan_count;
				left_pairs = scan_count >> 1;
				scan_count = left_pairs;
				if (left_pairs == 0u)
				{
				flood_fill_skip_parent_left_odd_pixel:
					if ((pixel_count & 1u) == 0u)
						break;
					pixel = mem_read8(r14);
					if (pixel == (uint8_t)fill_pattern || pixel == (uint8_t)border_color)
						break;
				flood_fill_skip_parent_left_found:
					scan_x = scan_x + r14 - scan_start;
				flood_fill_skip_parent_left_push:
					flood_fill_push_span(border_color, window_left, window_right, (uint8_t)direction, scan_x, y, fill_pattern, &right_address, &right_x);
					if ((int16_t)(flood_fill_parent_left_x - 2u) <= (int16_t)right_x)
						break;
					scan_x = right_x + 2u;
					r14 = right_address + 2u;
					continue;
				}
				for (;;)
				{
					pixels = mem_read16(r14);
					r14 = r14 + 2u;
					if ((uint8_t)pixels == (uint8_t)fill_pattern || (uint8_t)pixels == (uint8_t)border_color)
					{
						pixels = (uint16_t)(((uint32_t)pixels << 8) | (pixels >> 8));
						if ((uint8_t)pixels == (uint8_t)fill_pattern || (uint8_t)pixels == (uint8_t)border_color)
						{
							scan_count--;
							if (scan_count != 0u)
								continue;
							goto flood_fill_skip_parent_left_odd_pixel;
						}
						goto flood_fill_skip_parent_left_back_to_pixel;
					}
					break;
				}
				r14 = (r14 & ~0xFFFFu) | (uint16_t)(r14 - 1u);
			flood_fill_skip_parent_left_back_to_pixel:
				r14 = (r14 & ~0xFFFFu) | (uint16_t)(r14 - 1u);
				goto flood_fill_skip_parent_left_found;
			}
			pixel = mem_read8(r14);
			if (pixel == (uint8_t)fill_pattern || pixel == (uint8_t)border_color)
			{
				left_count_before = scan_count;
				scan_count = (uint16_t)(left_count_before - 1u);
				if ((int16_t)left_count_before <= 1)
					break;
				r14 = (r14 & ~0xFFFFu) | (uint16_t)(r14 + 1u);
				goto flood_fill_skip_parent_left_pairs;
			}
			goto flood_fill_skip_parent_left_push;
		}
	}
	scan_x = (scan_x & ~0xFFFFu) | flood_fill_parent_right_x;
	scan_count = flood_fill_right_x;
	scan_x = (scan_x & ~0xFFFFu) | (uint16_t)(scan_x + 1u);
	span_right_x = scan_count;
	scan_count = (uint16_t)(span_right_x - scan_x);
	if ((int16_t)span_right_x <= (int16_t)scan_x)
		return scan_x;
	r14 = flood_fill_parent_right_address;
	scan_x = (scan_x & ~0xFFFFu) | (uint16_t)(scan_x + 1u);
	r14 = (r14 & ~0xFFFFu) | (uint16_t)(r14 + 2u);
	for (;;)
	{
		scan_start = r14;
		if ((r14 & 1u) != 0u)
		{
			pixel = mem_read8(r14);
			if (pixel != (uint8_t)fill_pattern && pixel != (uint8_t)border_color)
			{
			flood_fill_skip_parent_right_push:
				flood_fill_push_span(border_color, window_left, window_right, (uint8_t)direction, scan_x, y, fill_pattern, &right_address, &right_x);
				scan_count = flood_fill_right_x;
				scan_count = (uint16_t)(scan_count - 1u);
				right_limit = scan_count;
				scan_count = (uint16_t)(right_limit - right_x);
				if ((int16_t)right_limit <= (int16_t)right_x)
					break;
				scan_x = right_x + 2u;
				r14 = right_address + 2u;
				continue;
			}
			right_count_before = scan_count;
			scan_count = (uint16_t)(right_count_before - 1u);
			if ((int16_t)right_count_before <= 1)
				break;
			r14 = (r14 & ~0xFFFFu) | (uint16_t)(r14 + 1u);
		}
		pixel_count = (uint8_t)scan_count;
		right_pairs = scan_count >> 1;
		scan_count = right_pairs;
		if (right_pairs != 0u)
		{
			for (;;)
			{
				pixels = mem_read16(r14);
				r14 = r14 + 2u;
				if ((uint8_t)pixels == (uint8_t)fill_pattern || (uint8_t)pixels == (uint8_t)border_color)
				{
					pixels = (uint16_t)(((uint32_t)pixels << 8) | (pixels >> 8));
					if ((uint8_t)pixels == (uint8_t)fill_pattern || (uint8_t)pixels == (uint8_t)border_color)
					{
						scan_count--;
						if (scan_count != 0u)
							continue;
						goto flood_fill_skip_parent_right_odd_pixel;
					}
					goto flood_fill_skip_parent_right_back_to_pixel;
				}
				break;
			}
			r14 = (r14 & ~0xFFFFu) | (uint16_t)(r14 - 1u);
		flood_fill_skip_parent_right_back_to_pixel:
			r14 = (r14 & ~0xFFFFu) | (uint16_t)(r14 - 1u);
		flood_fill_skip_parent_right_found:
			scan_x = scan_x + r14 - scan_start;
			goto flood_fill_skip_parent_right_push;
		}
	flood_fill_skip_parent_right_odd_pixel:
		if ((pixel_count & 1u) == 0u)
			break;
		pixel = mem_read8(r14);
		if (pixel == (uint8_t)fill_pattern || pixel == (uint8_t)border_color)
			break;
		goto flood_fill_skip_parent_right_found;
	}
	return scan_x;
}

void fill_span_at_point(uint32_t point_x, uint32_t point_y, uint32_t fill_color, uint8_t border_color, uint32_t right_cursor)
{
	uint8_t nibble = 0, pixel_byte = 0, point_pixel = 0;
	uint32_t fill_count = 0, fill_cursor = 0, fill_pattern = 0, ignored = 0, left_x = 0, outcode = 0, right_x = 0, scan_count = 0, window_left = 0, window_right = 0, words_left = 0;
	uint32_t start_x = 0, left_count_before = 0, right_limit = 0, right_count_before = 0, count_after_first = 0, bytes_after_words = 0, bytes_after_half = 0;

	get_point_outcode(&point_x, &point_y, &outcode);
	if ((uint8_t)outcode != 0u)
		return;
	if (bitmap_8bpp == 0u)
	{
		r14 = (point_y << 8) + (point_x >> 1) + bitmap_address;
		point_pixel = mem_read8(r14);
		if ((point_x & 1u) != 0u)
			point_pixel = (uint8_t)(((uint32_t)point_pixel << 4) | (point_pixel >> 4));
		point_pixel &= 0xFu;
		if (point_pixel == (uint8_t)fill_color || point_pixel == (uint8_t)border_color)
			return;
		window_left = bitmap_window_left;
		right_x = outcode;
		start_x = outcode;
		scan_count = start_x - window_left;
		if ((int32_t)window_left < (int32_t)start_x)
		{
			if ((outcode & 1u) != 0u)
			{
				pixel_byte = mem_read8(r14);
				outcode--;
				goto fill_span_at_point_left_even_pixel;
			}
		fill_span_at_point_left_byte:
			r14 = r14 - 1u;
			pixel_byte = mem_read8(r14);
			nibble = pixel_byte >> 4;
			outcode -= 2u;
			if (nibble == (uint8_t)fill_color || nibble == (uint8_t)border_color)
			{
				outcode++;
				r14 = r14 + 1u;
			fill_span_at_point_left_edge:
				outcode++;
			}
			else
			{
				left_count_before = scan_count;
				scan_count = left_count_before - 1u;
				if (left_count_before == 1u)
					goto fill_span_at_point_left_edge;
			fill_span_at_point_left_even_pixel:
				pixel_byte &= 0xFu;
				if (pixel_byte == (uint8_t)fill_color)
					goto fill_span_at_point_left_edge;
				scan_count--;
				if ((scan_count != 0u) & (pixel_byte != (uint8_t)border_color))
					goto fill_span_at_point_left_byte;
				if (pixel_byte == (uint8_t)border_color)
					goto fill_span_at_point_left_edge;
			}
		}
		right_limit = bitmap_window_right;
		scan_count = right_limit - right_x;
		if ((int32_t)right_x < (int32_t)right_limit)
		{
			right_x++;
			if ((right_x & 1u) != 0u)
			{
				pixel_byte = mem_read8(right_cursor);
				right_cursor++;
				right_x++;
				goto fill_span_at_point_right_odd_pixel;
			}
			right_cursor++;
		fill_span_at_point_right_byte:
			pixel_byte = mem_read8(right_cursor);
			nibble = pixel_byte & 0xFu;
			right_cursor++;
			right_x += 2u;
			if (nibble == (uint8_t)fill_color || nibble == (uint8_t)border_color)
			{
				right_x--;
			fill_span_at_point_right_edge:
				right_x--;
			}
			else
			{
				right_count_before = scan_count;
				scan_count = right_count_before - 1u;
				if (right_count_before == 1u)
					goto fill_span_at_point_right_edge;
			fill_span_at_point_right_odd_pixel:
				pixel_byte = pixel_byte >> 4;
				if (pixel_byte == (uint8_t)fill_color)
					goto fill_span_at_point_right_edge;
				scan_count--;
				if ((scan_count != 0u) & (pixel_byte != (uint8_t)border_color))
					goto fill_span_at_point_right_byte;
				if (pixel_byte == (uint8_t)border_color)
					goto fill_span_at_point_right_edge;
			}
		}
		fill_count = right_x - outcode;
		fill_cursor = r14;
		if ((outcode & 1u) != 0u)
		{
			pixel_byte = mem_read8(fill_cursor) & 0xFu;
			pixel_byte = (uint8_t)(((uint32_t)pixel_byte << 4) | (pixel_byte >> 4) | fill_color);
			mem_write8(fill_cursor, ((uint32_t)pixel_byte << 4) | (pixel_byte >> 4));
			count_after_first = fill_count - 1u;
			fill_count = count_after_first;
			fill_cursor++;
			if ((int32_t)count_after_first < 0)
				return;
		}
		if ((fill_cursor & 1u) != 0u)
		{
			if (fill_count == 0u)
			{
				mem_write8(fill_cursor, (0xF0u & mem_read8(fill_cursor)) | (fill_color & 0xFu));
				return;
			}
			fill_color = (uint8_t)(fill_color * 0x11u);
			mem_write8(fill_cursor, fill_color);
			fill_cursor++;
			fill_count -= 2u;
		}
		fill_color = (fill_color & 0xFu) * 0x11111111u;
		fill_count++;
		if (fill_count >= 8u)
		{
			words_left = fill_count >> 3;
			do
			{
				mem_write32(fill_cursor, fill_color);
				fill_cursor += 4u;
				words_left--;
			} while (words_left != 0u);
			bytes_after_words = fill_count & 7u;
			fill_count = bytes_after_words;
			if (bytes_after_words == 0u)
				return;
		}
		if (fill_count < 4u)
		{
			mem_write8(fill_cursor, fill_color);
			fill_cursor++;
			if ((fill_count & 1u) != 0u)
				mem_write8(fill_cursor, (0xF0u & mem_read8(fill_cursor)) | (fill_color & 0xFu));
		}
		else
		{
			mem_write16(fill_cursor, fill_color);
			bytes_after_half = fill_count & 3u;
			fill_count = bytes_after_half;
			fill_cursor += 2u;
			if (bytes_after_half != 0u)
			{
				mem_write8(fill_cursor, fill_color);
				fill_cursor++;
				if ((fill_count & 1u) != 0u)
					mem_write8(fill_cursor, (0xF0u & mem_read8(fill_cursor)) | (fill_color & 0xFu));
				return;
			}
		}
		return;
	}
	r14 = (point_y << 9) + point_x + bitmap_address;
	point_pixel = mem_read8(r14);
	if (point_pixel == (uint8_t)fill_color || point_pixel == (uint8_t)border_color)
		return;
	left_x = point_x;
	fill_pattern = (uint8_t)fill_color * 0x1010101u;
	window_left = (uint16_t)bitmap_window_left;
	window_right = (uint16_t)bitmap_window_right;
	flood_fill_span(border_color, window_left, window_right, fill_pattern, &left_x, &ignored, &ignored);
}

void save_bitmap_rect(uint16_t left, uint16_t top, uint32_t right, uint32_t bottom, uint32_t buffer)
{
	uint16_t swap_x = 0, swap_y = 0;

	if ((int16_t)right < (int16_t)left)
	{
		swap_x = left;
		left = (uint16_t)right;
		right = swap_x;
	}
	if ((int16_t)bottom < (int16_t)top)
	{
		swap_y = top;
		top = (uint16_t)bottom;
		bottom = swap_y;
	}
	right = (uint32_t)(int16_t)right - (int16_t)left + 1u;
	bottom = (uint32_t)(int16_t)bottom - (int16_t)top + 1u;
	if (bitmap_8bpp != 0u)
		right = right << 3;
	else
	{
		right = (right & ~0xFFu) | (uint8_t)(right * 2u);
		right = (right & ~0xFFu) | (uint8_t)(right * 2u);
	}
	mem_write32(buffer, right);
	mem_write32(buffer + 4u, bottom);
	do
	{
		invalid_operand(0x09C312u);
		bottom--;
	} while (bottom != 0u);
}

void draw_saved_bitmap_rect(uint32_t buffer)
{
	uint32_t rows_left = 0;

	rows_left = mem_read32(buffer + 4u);
	do
	{
		invalid_operand(0x09C372u);
		rows_left--;
	} while (rows_left != 0u);
}

void get_window_outcode(uint32_t *p_start_x, uint32_t *p_start_y, uint32_t *p_end_x, uint32_t *p_end_y, uint32_t *p_outcode)
{
	uint32_t start_x = *p_start_x;
	uint32_t start_y = *p_start_y;
	uint16_t end_x = (uint16_t)*p_end_x;
	uint16_t end_y = (uint16_t)*p_end_y;
	uint32_t r10 = 0, old_start_x = 0, old_start_y = 0, point_x = 0, point_y = 0;
	uint32_t old_end_x = 0, old_end_y = 0;

	old_end_x = end_x;
	old_start_x = start_x;
	point_x = old_end_x;
	old_end_y = end_y;
	old_start_y = start_y;
	point_y = old_end_y;
	get_point_outcode(&point_x, &point_y, &r10);
	*p_start_x = point_x;
	*p_start_y = point_y;
	*p_end_x = old_start_x;
	*p_end_y = old_start_y;
	*p_outcode = r10;
}

void get_point_outcode(uint32_t *p_point_x, uint32_t *p_point_y, uint32_t *p_outcode)
{
	uint32_t point_x = *p_point_x;
	uint32_t point_y = *p_point_y;
	uint32_t outcode = 0;

	outcode = 0u;
	if ((int16_t)point_x < bitmap_window_left)
		outcode = (uint8_t)(outcode + 1u);
	if (bitmap_window_right < (int16_t)point_x)
		outcode = (uint8_t)(outcode + 2u);
	if ((int16_t)point_y < bitmap_window_top)
		outcode = (uint8_t)(outcode + 4u);
	if ((int16_t)point_y <= bitmap_window_bottom)
	{
		point_x = (int16_t)point_x;
		point_y = (int16_t)point_y;
		*p_point_x = point_x;
		*p_point_y = point_y;
		*p_outcode = outcode;
		return;
	}
	outcode = (uint8_t)(outcode + 8u);
	point_x = (int16_t)point_x;
	point_y = (int16_t)point_y;
	*p_point_x = point_x;
	*p_point_y = point_y;
	*p_outcode = outcode;
}

void clip_line_to_window(uint32_t outcode, uint32_t end_outcode, uint32_t *p_start_x, uint32_t *p_start_y, uint32_t *p_end_x, uint32_t *p_end_y, uint32_t *p_unclipped_y)
{
	uint32_t start_x = *p_start_x;
	uint32_t start_y = *p_start_y;
	uint32_t end_x = *p_end_x;
	uint32_t end_y = *p_end_y;
	uint32_t unclipped_y = *p_unclipped_y;
	uint32_t r8 = 0, swapped_end_x = 0, swapped_end_y = 0, swapped_start_x = 0, swapped_start_y = 0;
	uint32_t old_start_x = 0, old_start_y = 0;

	switch (outcode)
	{
	case 0:
		clip_line_end_inside();
		break;
	case 1:
		clip_line_to_left_edge(end_x, end_y, &start_x, &start_y, &unclipped_y);
		break;
	case 2:
		clip_line_to_right_edge(end_x, end_y, &start_x, &start_y, &unclipped_y);
		break;
	case 3:
		clip_line_end_inside();
		break;
	case 4:
		clip_line_to_top_edge(end_x, end_y, &start_x, &start_y, &unclipped_y);
		break;
	case 5:
		clip_line_to_top_edge(end_x, end_y, &start_x, &start_y, &unclipped_y);
		break;
	case 6:
		clip_line_to_top_edge(end_x, end_y, &start_x, &start_y, &unclipped_y);
		break;
	case 7:
		clip_line_end_inside();
		break;
	case 8:
		clip_line_to_bottom_edge(end_x, end_y, &start_x, &start_y, &unclipped_y);
		break;
	case 9:
		clip_line_to_bottom_edge(end_x, end_y, &start_x, &start_y, &unclipped_y);
		break;
	case 10:
		clip_line_to_bottom_edge(end_x, end_y, &start_x, &start_y, &unclipped_y);
		break;
	case 11:
		clip_line_end_inside();
		break;
	default:
		unknown_code(0x9C3D2u + outcode * 3u);
	}
	old_start_x = start_x;
	swapped_start_x = end_x;
	swapped_end_x = old_start_x;
	old_start_y = start_y;
	swapped_start_y = end_y;
	swapped_end_y = old_start_y;
	switch (end_outcode)
	{
	case 0:
		clip_line_end_inside();
		*p_start_x = swapped_start_x;
		*p_start_y = swapped_start_y;
		*p_end_x = swapped_end_x;
		*p_end_y = swapped_end_y;
		*p_unclipped_y = unclipped_y;
		return;
	case 1:
		clip_line_to_left_edge(swapped_end_x, swapped_end_y, &swapped_start_x, &swapped_start_y, &r8);
		*p_start_x = swapped_start_x;
		*p_start_y = swapped_start_y;
		*p_end_x = swapped_end_x;
		*p_end_y = swapped_end_y;
		*p_unclipped_y = r8;
		return;
	case 2:
		clip_line_to_right_edge(swapped_end_x, swapped_end_y, &swapped_start_x, &swapped_start_y, &r8);
		*p_start_x = swapped_start_x;
		*p_start_y = swapped_start_y;
		*p_end_x = swapped_end_x;
		*p_end_y = swapped_end_y;
		*p_unclipped_y = r8;
		return;
	case 3:
		clip_line_end_inside();
		*p_start_x = swapped_start_x;
		*p_start_y = swapped_start_y;
		*p_end_x = swapped_end_x;
		*p_end_y = swapped_end_y;
		*p_unclipped_y = unclipped_y;
		return;
	case 4:
		clip_line_to_top_edge(swapped_end_x, swapped_end_y, &swapped_start_x, &swapped_start_y, &r8);
		*p_start_x = swapped_start_x;
		*p_start_y = swapped_start_y;
		*p_end_x = swapped_end_x;
		*p_end_y = swapped_end_y;
		*p_unclipped_y = r8;
		return;
	case 5:
		clip_line_to_top_edge(swapped_end_x, swapped_end_y, &swapped_start_x, &swapped_start_y, &r8);
		*p_start_x = swapped_start_x;
		*p_start_y = swapped_start_y;
		*p_end_x = swapped_end_x;
		*p_end_y = swapped_end_y;
		*p_unclipped_y = r8;
		return;
	case 6:
		clip_line_to_top_edge(swapped_end_x, swapped_end_y, &swapped_start_x, &swapped_start_y, &r8);
		*p_start_x = swapped_start_x;
		*p_start_y = swapped_start_y;
		*p_end_x = swapped_end_x;
		*p_end_y = swapped_end_y;
		*p_unclipped_y = r8;
		return;
	case 7:
		clip_line_end_inside();
		*p_start_x = swapped_start_x;
		*p_start_y = swapped_start_y;
		*p_end_x = swapped_end_x;
		*p_end_y = swapped_end_y;
		*p_unclipped_y = unclipped_y;
		return;
	case 8:
		clip_line_to_bottom_edge(swapped_end_x, swapped_end_y, &swapped_start_x, &swapped_start_y, &r8);
		*p_start_x = swapped_start_x;
		*p_start_y = swapped_start_y;
		*p_end_x = swapped_end_x;
		*p_end_y = swapped_end_y;
		*p_unclipped_y = r8;
		return;
	case 9:
		clip_line_to_bottom_edge(swapped_end_x, swapped_end_y, &swapped_start_x, &swapped_start_y, &r8);
		*p_start_x = swapped_start_x;
		*p_start_y = swapped_start_y;
		*p_end_x = swapped_end_x;
		*p_end_y = swapped_end_y;
		*p_unclipped_y = r8;
		return;
	case 10:
		clip_line_to_bottom_edge(swapped_end_x, swapped_end_y, &swapped_start_x, &swapped_start_y, &r8);
		*p_start_x = swapped_start_x;
		*p_start_y = swapped_start_y;
		*p_end_x = swapped_end_x;
		*p_end_y = swapped_end_y;
		*p_unclipped_y = r8;
		return;
	case 11:
		clip_line_end_inside();
		*p_start_x = swapped_start_x;
		*p_start_y = swapped_start_y;
		*p_end_x = swapped_end_x;
		*p_end_y = swapped_end_y;
		*p_unclipped_y = unclipped_y;
		return;
	default:
		unknown_code(0x9C3D2u + end_outcode * 3u);
	}
}

void clip_line_end_inside(void)
{
}

void clip_line_to_left_edge(uint32_t end_x, uint32_t end_y, uint32_t *p_start_x, uint32_t *p_start_y, uint32_t *p_unclipped_y)
{
	uint32_t start_x = *p_start_x;
	uint32_t start_y = *p_start_y;
	uint32_t r8 = 0, dx = 0;

	dx = bitmap_window_left - start_x;
	clip_line_at_x(end_x, end_y, dx, &start_x, &start_y, &r8);
	*p_start_x = start_x;
	*p_start_y = start_y;
	*p_unclipped_y = r8;
}

void clip_line_to_right_edge(uint32_t end_x, uint32_t end_y, uint32_t *p_start_x, uint32_t *p_start_y, uint32_t *p_unclipped_y)
{
	uint32_t start_x = *p_start_x;
	uint32_t start_y = *p_start_y;
	uint32_t r8 = 0, dx = 0;

	dx = bitmap_window_right - start_x;
	clip_line_at_x(end_x, end_y, dx, &start_x, &start_y, &r8);
	*p_start_x = start_x;
	*p_start_y = start_y;
	*p_unclipped_y = r8;
}

void clip_line_at_x(uint32_t end_x, uint32_t end_y, uint32_t dx, uint32_t *p_start_x, uint32_t *p_start_y, uint32_t *p_unclipped_y)
{
	uint32_t start_x = *p_start_x;
	uint32_t start_y = *p_start_y;
	uint32_t dy_scaled = 0, line_dx = 0, unclipped_y = 0;

	line_dx = end_x - start_x;
	unclipped_y = start_y;
	dy_scaled = (end_y - unclipped_y) * dx;
	start_x += dx;
	if (line_dx != 0u)
	{
		start_y += divide_s32(dy_scaled, line_dx);
		*p_start_x = start_x;
		*p_start_y = start_y;
		*p_unclipped_y = unclipped_y;
		return;
	}
	*p_start_x = start_x;
	*p_unclipped_y = unclipped_y;
}

void clip_line_to_top_edge(uint32_t end_x, uint32_t end_y, uint32_t *p_start_x, uint32_t *p_start_y, uint32_t *p_unclipped_y)
{
	uint32_t start_x = *p_start_x;
	uint32_t start_y = *p_start_y;
	uint32_t r8 = 0, dy = 0;

	dy = bitmap_window_top - start_y;
	clip_line_at_y(end_x, end_y, dy, &start_x, &start_y, &r8);
	*p_start_x = start_x;
	*p_start_y = start_y;
	*p_unclipped_y = r8;
}

void clip_line_to_bottom_edge(uint32_t end_x, uint32_t end_y, uint32_t *p_start_x, uint32_t *p_start_y, uint32_t *p_unclipped_y)
{
	uint32_t start_x = *p_start_x;
	uint32_t start_y = *p_start_y;
	uint32_t r8 = 0, dy = 0;

	dy = bitmap_window_bottom - start_y;
	clip_line_at_y(end_x, end_y, dy, &start_x, &start_y, &r8);
	*p_start_x = start_x;
	*p_start_y = start_y;
	*p_unclipped_y = r8;
}

void clip_line_at_y(uint32_t end_x, uint32_t end_y, uint32_t dy, uint32_t *p_start_x, uint32_t *p_start_y, uint32_t *p_unclipped_y)
{
	uint32_t start_x = *p_start_x;
	uint32_t start_y = *p_start_y;
	uint32_t dx_scaled = 0, line_dy = 0, unclipped_y = 0;

	unclipped_y = start_y;
	line_dy = end_y - unclipped_y;
	dx_scaled = (end_x - start_x) * dy;
	start_y += dy;
	if (line_dy != 0u)
	{
		start_x += divide_s32(dx_scaled, line_dy);
		*p_start_x = start_x;
		*p_start_y = start_y;
		*p_unclipped_y = unclipped_y;
		return;
	}
	*p_start_y = start_y;
	*p_unclipped_y = unclipped_y;
}

void clip_rect_to_window(uint32_t *p_left, uint32_t *p_top, uint32_t *p_right, uint32_t *p_bottom, uint32_t *p_r5, uint32_t *p_unclipped_edges)
{
	uint16_t left = (uint16_t)*p_left;
	uint16_t top = (uint16_t)*p_top;
	uint16_t right = (uint16_t)*p_right;
	uint16_t bottom = (uint16_t)*p_bottom;
	uint16_t window_max = 0, window_min = 0;
	uint32_t unclipped_edges = 0;
	uint16_t old_left = 0, old_top = 0;

	if ((int16_t)right < (int16_t)left)
	{
		old_left = left;
		left = (uint16_t)right;
		right = old_left;
	}
	if ((int16_t)bottom < (int16_t)top)
	{
		old_top = top;
		top = (uint16_t)bottom;
		bottom = old_top;
	}
	unclipped_edges = 0xFu;
	window_min = (uint16_t)bitmap_window_left;
	window_max = (uint16_t)bitmap_window_right;
	if (window_max < (uint16_t)left || (int16_t)right < (int16_t)window_min)
	{
		unclipped_edges = 0u;
		*p_left = (*p_left & 0xFFFF0000u) | left;
		*p_top = (*p_top & 0xFFFF0000u) | top;
		*p_right = (*p_right & 0xFFFF0000u) | right;
		*p_bottom = (*p_bottom & 0xFFFF0000u) | bottom;
		*p_r5 = (*p_r5 & 0xFFFF0000u) | window_min;
		*p_unclipped_edges = unclipped_edges;
		return;
	}
	if ((int16_t)left < (int16_t)window_min)
	{
		left = (uint16_t)window_min;
		unclipped_edges = 0xEu;
	}
	if ((int16_t)window_max < (int16_t)right)
	{
		right = (uint16_t)window_max;
		unclipped_edges = unclipped_edges & 0xDu;
	}
	window_min = (uint16_t)bitmap_window_top;
	window_max = (uint16_t)bitmap_window_bottom;
	if (window_max < (uint16_t)top || (int16_t)bottom < (int16_t)window_min)
	{
		unclipped_edges = 0u;
		*p_left = (*p_left & 0xFFFF0000u) | left;
		*p_top = (*p_top & 0xFFFF0000u) | top;
		*p_right = (*p_right & 0xFFFF0000u) | right;
		*p_bottom = (*p_bottom & 0xFFFF0000u) | bottom;
		*p_r5 = (*p_r5 & 0xFFFF0000u) | window_min;
		*p_unclipped_edges = unclipped_edges;
		return;
	}
	if ((int16_t)top < (int16_t)window_min)
	{
		top = (uint16_t)window_min;
		unclipped_edges = unclipped_edges & 0xBu;
	}
	if ((int16_t)bottom <= (int16_t)window_max)
	{
		*p_left = (*p_left & 0xFFFF0000u) | left;
		*p_top = (*p_top & 0xFFFF0000u) | top;
		*p_right = (*p_right & 0xFFFF0000u) | right;
		*p_bottom = (*p_bottom & 0xFFFF0000u) | bottom;
		*p_r5 = (*p_r5 & 0xFFFF0000u) | window_min;
		*p_unclipped_edges = unclipped_edges;
		return;
	}
	bottom = (uint16_t)window_max;
	unclipped_edges = unclipped_edges & 7u;
	*p_left = (*p_left & 0xFFFF0000u) | left;
	*p_top = (*p_top & 0xFFFF0000u) | top;
	*p_right = (*p_right & 0xFFFF0000u) | right;
	*p_bottom = (*p_bottom & 0xFFFF0000u) | bottom;
	*p_r5 = (*p_r5 & 0xFFFF0000u) | window_min;
	*p_unclipped_edges = unclipped_edges;
}

void attract_warning(uint32_t *p_pixels, uint32_t *p_text)
{
	uint32_t pixels = *p_pixels;
	uint32_t text = *p_text;
	uint8_t new_mode = 0;
	uint32_t r0 = 0, r11 = 0, ignored = 0;
	uint8_t cnt_bits = 0;

	switch (system_mode_step)
	{
	case 0:
	case 4:
		break;
	case 1:
		wait_attract_timer();
		*p_pixels = pixels;
		*p_text = text;
		return;
	case 2:
		advance_attract_screen();
		*p_pixels = pixels;
		*p_text = text;
		return;
	default:
		unknown_code(0x9C4EFu + system_mode_step * 3u);
	}
	attract_timer = -0x258;
	system_mode_step++;
	display_off_frames = 3u;
	cnt_bits = mem_read8(io_cnt);
	mem_write8(io_cnt, cnt_bits & ~2u);
	load_text_font(&pixels, &ignored);
	init_video_registers();
	init_sprites(&r0, &ignored);
	new_mode = 1u;
	set_screen_mode(new_mode);
	init_sound();
	text = warning_screen_text_ADDRESS;
	print_text_list(&text, &r11);
	unused_coin_byte = 0u;
	current_screen = 0u;
	*p_pixels = pixels;
	*p_text = text;
}

void system_mode_idle(void)
{
	system_mode_step = 0u;
}

void wait_attract_timer(void)
{
	if ((uint16_t)attract_timer == 0u)
	{
		system_mode_step++;
		frame_counter = 0u;
	}
}

void eeprom_lock(void)
{
	uint8_t command = 0;
	uint32_t ignored = 0, lines = 0, port_d = 0;

	command = 0u;
	eeprom_select(&lines, &ignored, &port_d);
	lines = eeprom_send_command(lines, command, port_d);
	eeprom_deselect(lines, port_d);
	interrupts_on();
}

void eeprom_unlock(void)
{
	uint8_t command = 0;
	uint32_t ignored = 0, lines = 0, port_d = 0;

	command = 0x30u;
	eeprom_select(&lines, &ignored, &port_d);
	lines = eeprom_send_command(lines, command, port_d);
	eeprom_deselect(lines, port_d);
	interrupts_on();
}

uint32_t eeprom_read_word_at(uint32_t data, uint8_t command)
{
	uint32_t lines = 0, port_d = 0, port_f = 0;

	command |= 0x80u;
	eeprom_select(&lines, &port_f, &port_d);
	eeprom_read_word(command, port_f, port_d, &data, &lines);
	interrupts_on();
	return data;
}

void load_settings(uint32_t crc)
{
	uint32_t ignored = 0;
	uint8_t port_f_bits = 0;

	eeprom_read_all(crc);
	if (eeprom_magic == 0x53333241u)
	{
		if (eeprom_rom_stamp != (uint16_t)(rom_checksums + rom_checksum_ic8))
		{
			move_halfwords(setting_advertise_sound_ADDRESS, default_settings_ADDRESS, 0x19u);
			port_f_bits = mem_read8(io_port_f);
			mem_write8(moving_seat_dip, port_f_bits & 1u);
			mem_write16(bookkeeping_rom_stamp, 0u);
			eeprom_rom_stamp = rom_checksums + rom_checksum_ic8;
			compute_eeprom_checksum(&crc, &ignored);
			eeprom_checksum = crc;
			eeprom_save(&ignored, &ignored);
			return;
		}
		compute_eeprom_checksum(&crc, &ignored);
		if ((uint16_t)crc == eeprom_checksum)
		{
			crc = compute_eeprom_backup_checksum();
			if ((uint16_t)crc != eeprom_backup_checksum || (uint16_t)crc != eeprom_checksum)
			{
				eeprom_save(&ignored, &ignored);
				return;
			}
			return;
		}
		crc = compute_eeprom_backup_checksum();
		if ((uint16_t)crc == eeprom_backup_checksum)
		{
			move_halfwords(eeprom_magic_ADDRESS, eeprom_backup_ADDRESS, 0x20u);
			eeprom_save(&ignored, &ignored);
			return;
		}
		move_halfwords(setting_advertise_sound_ADDRESS, default_settings_ADDRESS, 0x19u);
		port_f_bits = mem_read8(io_port_f);
		mem_write8(moving_seat_dip, port_f_bits & 1u);
		mem_write16(bookkeeping_rom_stamp, 0u);
		eeprom_rom_stamp = rom_checksums + rom_checksum_ic8;
		compute_eeprom_checksum(&crc, &ignored);
		eeprom_checksum = crc;
		eeprom_save(&ignored, &ignored);
		return;
	}
	if (eeprom_magic != 0x41474553u)
	{
		move_halfwords(eeprom_magic_ADDRESS, eeprom_defaults_ADDRESS, 0x20u);
		port_f_bits = mem_read8(io_port_f);
		mem_write8(moving_seat_dip, port_f_bits & 1u);
		mem_write16(bookkeeping_rom_stamp, 0u);
		eeprom_rom_stamp = rom_checksums + rom_checksum_ic8;
		compute_eeprom_checksum(&crc, &ignored);
		eeprom_checksum = crc;
		eeprom_save(&ignored, &ignored);
		return;
	}
	move_halfwords(eeprom_magic_ADDRESS, eeprom_defaults_ADDRESS, 0x20u);
	eeprom_write_count = 0x64u;
	port_f_bits = mem_read8(io_port_f);
	mem_write8(moving_seat_dip, port_f_bits & 1u);
	mem_write16(bookkeeping_rom_stamp, 0u);
	eeprom_rom_stamp = rom_checksums + rom_checksum_ic8;
	compute_eeprom_checksum(&crc, &ignored);
	eeprom_checksum = crc;
	eeprom_save(&ignored, &ignored);
}

void eeprom_read_all_and_enable_interrupts(uint32_t data)
{
	eeprom_read_all(data);
	interrupts_on();
}

void eeprom_read_all(uint32_t data)
{
	uint8_t command = 0;
	uint32_t eeprom_words_left = 0, lines = 0, port_d = 0, port_f = 0, word_dest = 0;

	word_dest = eeprom_magic_ADDRESS;
	command = 0x80u;
	eeprom_words_left = 0x40u;
	eeprom_select(&lines, &port_f, &port_d);
	do
	{
		eeprom_read_word(command, port_f, port_d, &data, &lines);
		command++;
		mem_write16(word_dest, data);
		eeprom_words_left--;
		word_dest += 2u;
	} while (eeprom_words_left != 0u);
}

void eeprom_write_word_at(uint16_t data, uint8_t command)
{
	uint32_t lines = 0, port_d = 0, port_f = 0;

	command |= 0x40u;
	eeprom_select(&lines, &port_f, &port_d);
	eeprom_write_word(data, lines, command, port_f, port_d);
	interrupts_on();
}

void eeprom_save_and_lock(uint32_t *p_command, uint32_t *p_words_left)
{
	uint32_t command = 0, words_left = 0;

	eeprom_save(&command, &words_left);
	eeprom_lock();
	*p_command = command;
	*p_words_left = words_left;
}

void eeprom_save(uint32_t *p_command, uint32_t *p_words_left)
{
	uint16_t data = 0;
	uint32_t command = 0, crc = 0, ignored = 0, lines = 0, polynomial = 0, port_d = 0, port_f = 0, word_source = 0, words_left = 0;

	eeprom_write_count++;
	compute_eeprom_checksum(&crc, &polynomial);
	eeprom_checksum = crc;
	move_halfwords(eeprom_backup_ADDRESS, eeprom_magic_ADDRESS, 0x20u);
	command = (polynomial & ~0xFFu) | 0x30u;
	eeprom_select(&lines, &ignored, &port_d);
	eeprom_send_command(lines, (uint8_t)command, port_d);
	word_source = eeprom_magic_ADDRESS;
	command = (command & ~0xFFu) | 0x40u;
	words_left = 0x40u;
	eeprom_select(&lines, &port_f, &port_d);
	do
	{
		data = mem_read16(word_source);
		word_source += 2u;
		lines = eeprom_write_word(data, lines, (uint8_t)command, port_f, port_d);
		command = (command & ~0xFFu) | (uint8_t)(command + 1u);
		words_left--;
	} while (words_left != 0u);
	*p_command = command;
	*p_words_left = words_left;
}

void eeprom_read_word(uint8_t command, uint32_t port_f, uint32_t port_d, uint32_t *p_data, uint32_t *p_lines)
{
	uint32_t data = *p_data;
	uint32_t lines = *p_lines;
	uint32_t bits_left = 0;

	lines = eeprom_send_command(lines, command, port_d);
	lines = (lines & 0x3Fu) | 0x20u;
	bits_left = 0x12u;
	do
	{
		data = eeprom_set_lines(data, (uint8_t)lines, port_d);
		data = ((data & ~0xFFu) | mem_read8(port_f)) * 2u;
		lines = lines ^ 0x40u;
		data = eeprom_set_lines(data, (uint8_t)lines, port_d);
		lines = lines ^ 0x40u;
		bits_left--;
	} while (bits_left != 0u);
	lines = eeprom_deselect(lines, port_d);
	data = data >> 9;
	*p_data = data;
	*p_lines = (*p_lines & 0xFFFFFF00u) | (lines & 0xFFu);
}

uint32_t eeprom_write_word(uint32_t data, uint32_t lines, uint8_t command, uint32_t port_f, uint32_t port_d)
{
	uint16_t data_bits = 0;
	uint32_t clocks_left = 0;

	data_bits = (uint16_t)data;
	lines = eeprom_send_command(lines, command, port_d);
	data_bits = (uint16_t)(((uint32_t)data_bits << 8) | (data_bits >> 8));
	clocks_left = 0x10u;
	do
	{
		data = (data & ~0xFFu) | (uint8_t)data_bits;
		data = (data & ~0xFFu) | (data & 0x80u);
		data_bits = (uint16_t)(((uint32_t)data_bits << 1) | (data_bits >> 15));
		lines = ((lines & ~0xFFu) | (lines & 0x1Fu)) + 0x20u + data;
		data = eeprom_set_lines(data, (uint8_t)lines, port_d);
		lines = (lines & ~0xFFu) | (uint8_t)(lines ^ 0x40u);
		data = eeprom_set_lines(data, (uint8_t)lines, port_d);
		clocks_left--;
	} while (clocks_left != 0u);
	clocks_left = 2u;
	do
	{
		lines = (lines & ~0xFFu) | (lines & 0x1Fu);
		data = eeprom_set_lines(data, (uint8_t)lines, port_d);
		lines = (lines & ~0xFFu) | (uint8_t)(lines ^ 0x40u);
		data = eeprom_set_lines(data, (uint8_t)lines, port_d);
		clocks_left--;
	} while (clocks_left != 0u);
	lines = (lines & ~0xFFu) | (lines & 0x1Fu);
	do
	{
		lines = (lines & ~0xFFu) | (uint8_t)(lines | 0x20u);
		eeprom_set_lines(data, (uint8_t)lines, port_d);
		data = mem_read8(port_f);
		lines = (lines & ~0xFFu) | (uint8_t)(lines ^ 0x40u);
		data = eeprom_set_lines(data, (uint8_t)lines, port_d);
	} while ((int8_t)data >= 0);
	lines = eeprom_deselect(lines, port_d);
	return lines;
}

uint32_t eeprom_send_command(uint32_t lines, uint8_t command, uint32_t port_d)
{
	uint8_t command_bits = 0;
	uint32_t bits_left = 0, data = 0, data_bit = 0;

	command_bits = (uint8_t)command;
	lines = (lines & ~0xFFu) | (lines & 0x1Fu);
	data = eeprom_set_lines(0u, (uint8_t)lines, port_d);
	lines = (lines & ~0xFFu) | (uint8_t)(lines ^ 0x40u);
	data = eeprom_set_lines(data, (uint8_t)lines, port_d);
	lines = (lines & ~0xFFu) | (uint8_t)(lines ^ 0x60u);
	data = eeprom_set_lines(data, (uint8_t)lines, port_d);
	lines = (lines & ~0xFFu) | (uint8_t)(lines ^ 0x40u);
	data = eeprom_set_lines(data, (uint8_t)lines, port_d);
	lines = (lines & ~0xFFu) | (uint8_t)(lines ^ 0xC0u);
	data = eeprom_set_lines(data, (uint8_t)lines, port_d);
	lines = (lines & ~0xFFu) | (uint8_t)(lines ^ 0x40u);
	eeprom_set_lines(data, (uint8_t)lines, port_d);
	bits_left = 8u;
	do
	{
		data_bit = command_bits;
		command_bits = (uint8_t)(command_bits * 2u);
		data_bit = data_bit & 0x80u;
		lines = (lines & ~0xFFu) | (lines & 0x3Fu);
		lines = (lines & ~0xFFu) | (uint8_t)(lines | data_bit);
		data_bit = eeprom_set_lines(data_bit, (uint8_t)lines, port_d);
		lines = (lines & ~0xFFu) | (uint8_t)(lines ^ 0x40u);
		eeprom_set_lines(data_bit, (uint8_t)lines, port_d);
		bits_left--;
	} while (bits_left != 0u);
	return lines;
}

void eeprom_select(uint32_t *p_lines, uint32_t *p_port_f, uint32_t *p_port_d)
{
	uint8_t lines = 0;
	uint32_t port_d = 0, port_f = 0;

	port_d = io_port_d;
	port_f = io_port_f;
	lines = mem_read8(io_port_d);
	interrupts_off();
	*p_lines = (*p_lines & 0xFFFFFF00u) | lines;
	*p_port_f = port_f;
	*p_port_d = port_d;
}

uint32_t compute_eeprom_backup_checksum(void)
{
	uint32_t crc = 0, ignored = 0;

	current_object = 0x20F64Au;
	compute_eeprom_crc(&crc, &ignored);
	return crc;
}

void compute_eeprom_checksum(uint32_t *p_crc, uint32_t *p_polynomial)
{
	uint32_t r4 = 0, crc = 0;

	current_object = eeprom_write_count_ADDRESS;
	compute_eeprom_crc(&crc, &r4);
	*p_crc = crc;
	*p_polynomial = r4;
}

void compute_eeprom_crc(uint32_t *p_crc, uint32_t *p_polynomial)
{
	uint32_t carry = 0;
	uint32_t bits_left = 0, crc = 0, polynomial = 0;

	free_objects = 0x36u;
	polynomial = 0x10210000u;
	crc = 0xDEBDEB00u;
	do
	{
		crc = (crc & ~0xFFu) | mem_read8(current_object);
		bits_left = 8u;
		current_object = current_object + 1u;
		do
		{
			carry = crc * 2u < crc;
			crc = crc * 2u;
			if (carry)
				crc ^= polynomial;
			bits_left--;
		} while (bits_left != 0u);
		free_objects = free_objects - 1u;
	} while (free_objects != 0u);
	bits_left = 8u;
	do
	{
		carry = crc * 2u < crc;
		crc = crc * 2u;
		if (carry)
			crc ^= polynomial;
		bits_left--;
	} while (bits_left != 0u);
	crc = crc >> 16;
	*p_crc = crc;
	*p_polynomial = polynomial;
}

uint32_t eeprom_deselect(uint32_t lines, uint32_t port_d)
{
	uint32_t data = 0;

	lines = (lines & ~0xFFu) | (lines & 0x1Fu);
	data = eeprom_set_lines(0u, (uint8_t)lines, port_d);
	return lines;
}

uint32_t eeprom_set_lines(uint32_t data, uint8_t lines, uint32_t port_d)
{
	mem_write8(port_d, lines);
	return data;
}

void start_test_mode(uint32_t *p_color, uint32_t *p_blocks_per_row)
{
	uint32_t color = *p_color;
	uint32_t blocks_per_row = *p_blocks_per_row;
	uint8_t is_8bpp = 0, mode = 0, sound_id = 0;
	uint16_t scroll_x = 0;
	uint32_t r0 = 0, r5 = 0, r8 = 0, r10 = 0, ignored = 0, scroll_y = 0;
	uint8_t cnt_bits = 0, port_d_bits = 0, seat_port_bits = 0;

	cnt_bits = mem_read8(io_cnt);
	mem_write8(io_cnt, cnt_bits & ~2u);
	show_eeprom_write_count = 0u;
	if ((0x1001u & input_ports_active) != 0u)
		show_eeprom_write_count = 1u;
	display_off_frames = 0x14u;
	load_default_palettes();
	init_video_registers();
	init_sprites(&r0, &r10);
	init_sound();
	port_d_bits = mem_read8(io_port_d);
	mem_write8(io_port_d, port_d_bits & ~8u);
	sound_id = 0x94u;
	queue_sound(sound_id, &ignored, &ignored);
	r8 = wait_for_frames();
	reset_objects(&ignored, &ignored);
	clear_sprite_buckets();
	mode = 0u;
	set_screen_mode(mode);
	clear_text_layer(&r0, &ignored, &ignored);
	clear_bitmap_layer((uint8_t)r0, &scroll_y, &color, &r5, &r8, &blocks_per_row, &r10);
	mem_write16(video_layer_disable, 0xFu);
	scroll_x = 0u;
	scroll_y &= ~0xFFFFu;
	is_8bpp = 1u;
	set_bitmap_scroll(scroll_x, (uint16_t)scroll_y, is_8bpp);
	test_mode = 0xFFu;
	test_screen = 0u;
	test_cursor = 0u;
	test_continue_mode = 0u;
	test_screen_ready = 0u;
	seat_port_bits = mem_read8(io_port_d);
	mem_write8(io_port_d, seat_port_bits & ~0xCu);
	color_offset_red = 0u;
	color_offset_blue = 0u;
	color_offset_green = 0u;
	color_offset_2_red = 0u;
	color_offset_2_blue = 0u;
	color_offset_2_green = 0u;
	backup_clear_timer = 0u;
	mem_write16(mixer_bitmap, 0x21u);
	mem_write32(video_control, mem_read32(video_control) | 0x800u);
	fill_value = (fill_value & ~0xFFFFu) | 0x909u;
	set_halfwords(VIDEO_RAM, 0xE000u, fill_value);
	run_test_mode(scroll_y, (uint16_t)r8, &color, &blocks_per_row);
	*p_color = color;
	*p_blocks_per_row = blocks_per_row;
}

void run_test_mode(uint32_t r1, uint16_t r8, uint32_t *p_r4, uint32_t *p_r9)
{
	uint32_t r4 = *p_r4;
	uint32_t r9 = *p_r9;

	switch (test_screen)
	{
	case 0:
		test_menu();
		break;
	case 1:
		toggle_test_continue_mode();
		break;
	case 2:
		test_memory(&r4, &r9);
		break;
	case 3:
		test_inputs(r8, &r4, &r9);
		break;
	case 4:
		test_sound();
		break;
	case 5:
		test_crt(r1, &r4, &r9);
		break;
	case 6:
		test_game_assignments(&r4, &r9);
		break;
	case 7:
		r4 = test_coin_assignments(r4);
		break;
	case 8:
		test_bookkeeping();
		break;
	case 9:
		r4 = test_backup_clear(r4);
		break;
	case 10:
		test_outputs();
		break;
	case 11:
		exit_test_mode();
		break;
	case 12:
		test_menu();
		break;
	default:
		unknown_code(0x9CA0Eu + test_screen * 3u);
	}
	*p_r4 = r4;
	*p_r9 = r9;
}

void test_menu(void)
{
	uint32_t cursor_entry = 0, cursor_positions = 0, ignored = 0;

	cursor_positions = test_menu_cursor_positions_ADDRESS;
	erase_test_cursor(cursor_positions);
	if (test_screen_ready == 0u)
	{
		test_screen_ready++;
		clear_text_layer(&ignored, &ignored, &ignored);
		test_cursor = 0xAu;
		draw_test_menu();
		return;
	}
	if ((inputs_pressed & 1u) != 0u || (inputs_pressed & 0x1000u) != 0u)
	{
		test_cursor++;
		if (mem_read8(moving_seat_dip) == 0u && test_cursor == 9u)
			test_cursor++;
	}
	for (;;)
	{
		cursor_entry = test_menu_cursor_positions_ADDRESS + ((uint32_t)test_cursor << 1);
		if ((int8_t)ROM8(cursor_entry) >= 0)
			break;
		test_cursor = 0u;
	}
	test_cursor_column = ROM8(cursor_entry);
	test_cursor_row = ROM8(cursor_entry + 1u);
	blink_test_cursor();
	if ((inputs_pressed & 2u) != 0u || (inputs_pressed & 0x2000u) != 0u)
	{
		test_screen = test_cursor;
		test_screen++;
		test_menu_item = test_cursor;
		test_screen_ready = 0u;
	}
}

void draw_test_menu(void)
{
	uint16_t attribute = 0;
	uint32_t ignored = 0, print_position = 0, text = 0, write_count_bcd = 0;

	test_cursor_positions = test_menu_cursor_positions_ADDRESS;
	clear_text_layer(&ignored, &ignored, &print_position);
	if (show_eeprom_write_count != 0u)
	{
		write_count_bcd = AS16(eeprom_write_count);
		convert_to_bcd(&write_count_bcd, &ignored);
		write_count_bcd &= 0xFFFFFu;
		print_position = TEXT_CELL(31, 26);
		attribute = 0x1C00u;
		draw_hex_8(write_count_bcd, attribute, print_position);
	}
	text = test_menu_text_ADDRESS;
	print_text_list(&text, &print_position);
	if (mem_read8(moving_seat_dip) != 0u)
	{
		text = test_menu_output_test_text_ADDRESS;
		print_text_list(&text, &print_position);
	}
}

void toggle_test_continue_mode(void)
{
	uint32_t text = 0, text_position = 0;

	text = individual_mode_text_ADDRESS;
	test_continue_mode = ~test_continue_mode;
	if (test_continue_mode != 0u)
		text = continue_mode_text_ADDRESS;
	print_text_list(&text, &text_position);
	test_screen = 0u;
	test_screen_ready = 1u;
}

void test_memory(uint32_t *p_results_left, uint32_t *p_r9)
{
	uint32_t results_left = *p_results_left;
	uint32_t r9 = *p_r9;
	uint8_t is_8bpp = 0, mode = 0;
	uint16_t scroll_x = 0, scroll_y = 0;
	uint32_t r0 = 0, r4 = 0, r5 = 0, r8 = 0, r10 = 0, r11 = 0, delay_count = 0, ignored = 0, offset_entry = 0, result_index = 0, text = 0;
	uint8_t cnt_bits = 0;

	if (test_screen_ready == 0u)
	{
		interrupts_off();
		test_screen_ready++;
		clear_text_layer(&ignored, &ignored, &r11);
		text = memory_test_text_ADDRESS;
		print_text_list(&text, &r11);
		text = memory_test_dashes_text_ADDRESS;
		print_text_list(&text, &r11);
		memory_test_errors = 0u;
		mem_write8(sound_cpu_hold, 1u);
		delay_count = 0x1000u;
		do
		{
			delay_count--;
		} while (delay_count != 0u);
		check_rom_checksums();
		test_work_ram();
		test_video_ram();
		test_sprite_ram();
		test_palette_ram();
		test_sound_ram();
		cnt_bits = mem_read8(io_cnt);
		mem_write8(io_cnt, cnt_bits & ~2u);
		interrupts_on();
		display_off_frames = 0x78u;
		delay_count = 0x3Cu;
		do
		{
			delay_count--;
		} while (delay_count != 0u);
		load_default_palettes();
		load_text_font(&r4, &r5);
		init_video_registers();
		reset_objects(&ignored, &ignored);
		init_sound();
		init_sprites(&ignored, &r10);
		clear_sprite_buckets();
		mode = 0u;
		set_screen_mode(mode);
		clear_text_layer(&r0, &ignored, &r11);
		clear_bitmap_layer((uint8_t)r0, &ignored, &r4, &r5, &r8, &r9, &r10);
		mem_write16(video_layer_disable, 0xFu);
		scroll_x = 0u;
		scroll_y = 0u;
		is_8bpp = 1u;
		set_bitmap_scroll(scroll_x, scroll_y, is_8bpp);
		mem_write32(video_control, mem_read32(video_control) | 0x800u);
		fill_value = (fill_value & ~0xFFFFu) | 0x909u;
		set_halfwords(VIDEO_RAM, 0xE000u, fill_value);
		color_offset_red = 0u;
		color_offset_blue = 0u;
		color_offset_green = 0u;
		color_offset_2_red = 0u;
		color_offset_2_blue = 0u;
		color_offset_2_green = 0u;
		text = memory_test_text_ADDRESS;
		print_text_list(&text, &r11);
		text = memory_test_done_text_ADDRESS;
		print_text_list(&text, &r11);
		text = memory_test_dashes_text_ADDRESS;
		print_text_list(&text, &r11);
		if (memory_test_errors != 0u)
		{
			text = memory_test_error_text_ADDRESS;
			print_text_list(&text, &r11);
		}
		results_left = 0x13u;
		result_index = 0u;
		do
		{
			offset_entry = memory_test_good_offsets_ADDRESS + result_index * 2u;
			if ((memory_test_errors & shl32(1u, (int)(result_index))) != 0u)
				offset_entry = memory_test_bad_offsets_ADDRESS + result_index * 2u;
			r9 = memory_test_result_texts_ADDRESS;
			text = ROM16(offset_entry) + memory_test_result_texts_ADDRESS;
			print_text_list(&text, &r11);
			result_index = (uint8_t)(result_index + 1u);
			results_left--;
		} while (results_left != 0u);
	}
	if ((0x2002u & inputs_pressed) == 0u)
	{
		*p_results_left = results_left;
		*p_r9 = r9;
		return;
	}
	if (test_continue_mode != 0u)
	{
		test_screen++;
		test_screen_ready = 0u;
		*p_results_left = results_left;
		*p_r9 = r9;
		return;
	}
	test_cursor = test_menu_item;
	test_screen = 0u;
	test_screen_ready = 1u;
	draw_test_menu();
	*p_results_left = results_left;
	*p_r9 = r9;
}

void check_rom_checksums(void)
{
	uint32_t count = 0, crc = 0, data = 0, ignored = 0, stored_crc = 0;
	uint16_t ic8_crc = 0, ic18_crc = 0;

	data = 0u;
	count = 0x20000u;
	compute_crc16(count, &crc, &data, &ignored);
	stored_crc = rom_checksum_ic8_ADDRESS;
	if ((uint16_t)crc != rom_checksums)
		memory_test_errors |= 1u;
	data = 0x80000u;
	count = 0x1F000u;
	compute_crc16(count, &crc, &data, &ignored);
	ic8_crc = ROM16(stored_crc);
	stored_crc += 2u;
	if ((uint16_t)crc != ic8_crc)
		memory_test_errors |= 4u;
	data = 0x100000u;
	count = 0x40000u;
	crc = compute_crc16_alternate_bytes(data, count);
	ic18_crc = ROM16(stored_crc);
	stored_crc += 2u;
	if ((uint16_t)crc != ic18_crc)
		memory_test_errors |= 2u;
	data = 0x100001u;
	count = 0x3FFFFu;
	crc = compute_crc16_alternate_bytes(data, count);
	if ((uint16_t)crc != ROM16(stored_crc))
		memory_test_errors |= 8u;
}

void test_work_ram(void)
{
	uint32_t bytes_left = 0, passes_left = 0, pattern = 0, pattern_index = 0, ram_cursor = 0;
	uint8_t expected = 0, actual = 0;

	move_bytes(VIDEO_RAM, WORK_RAM, 0x10000u);
	ram_cursor = 0x200000u;
	passes_left = 0x42u;
	do
	{
		pattern_index = 4u;
		do
		{
			pattern = ram_test_patterns[pattern_index];
			bytes_left = 0xF9u;
			do
			{
				mem_write8(ram_cursor, ROM8(pattern));
				pattern++;
				ram_cursor++;
				if ((int32_t)ram_cursor >= 0x210000)
					goto test_work_ram_verify;
				bytes_left--;
			} while (bytes_left != 0u);
			pattern_index--;
		} while (pattern_index != 0u);
		passes_left--;
	} while (passes_left != 0u);
test_work_ram_verify:
	ram_cursor = 0x200000u;
	passes_left = 0x42u;
	do
	{
		pattern_index = 4u;
		do
		{
			pattern = ram_test_patterns[pattern_index];
			bytes_left = 0xF9u;
			do
			{
				expected = ROM8(pattern);
				actual = mem_read8(ram_cursor);
				pattern++;
				ram_cursor++;
				if (actual != expected)
				{
					if ((ram_cursor & 1u) != 0u)
						mem_write32(memory_test_video_word, mem_read32(memory_test_video_word) | 0x20u);
					else
						mem_write32(memory_test_video_word, mem_read32(memory_test_video_word) | 0x40u);
				}
				if ((int32_t)ram_cursor >= 0x210000)
				{
					move_bytes(WORK_RAM, VIDEO_RAM, 0x10000u);
					return;
				}
				bytes_left--;
			} while (bytes_left != 0u);
			pattern_index--;
		} while (pattern_index != 0u);
		passes_left--;
	} while (passes_left != 0u);
	move_bytes(WORK_RAM, VIDEO_RAM, 0x10000u);
}

void test_video_ram(void)
{
	uint32_t bytes_left = 0, passes_left = 0, pattern = 0, pattern_index = 0, ram_cursor = 0;
	uint8_t expected = 0, actual = 0;

	ram_cursor = 0x300000u;
	passes_left = 0x84u;
	do
	{
		pattern_index = 4u;
		do
		{
			pattern = ram_test_patterns[pattern_index];
			bytes_left = 0xF9u;
			do
			{
				mem_write8(ram_cursor, ROM8(pattern));
				ram_cursor++;
				pattern++;
				if ((int32_t)ram_cursor >= 0x320000)
					goto test_video_ram_verify;
				bytes_left--;
			} while (bytes_left != 0u);
			pattern_index--;
		} while (pattern_index != 0u);
		passes_left--;
	} while (passes_left != 0u);
test_video_ram_verify:
	ram_cursor = 0x300000u;
	passes_left = 0x84u;
	do
	{
		pattern_index = 4u;
		do
		{
			pattern = ram_test_patterns[pattern_index];
			bytes_left = 0xF9u;
			do
			{
				expected = ROM8(pattern);
				actual = mem_read8(ram_cursor);
				ram_cursor++;
				pattern++;
				if (actual != expected)
				{
					if ((ram_cursor & 1u) != 0u)
					{
						if ((mem_read8(ram_cursor - 1u) & 0xFu) != (mem_read8(pattern - 1u) & 0xFu))
							memory_test_errors |= 0x10000u;
						if ((mem_read8(ram_cursor - 1u) & ~0xFu) != (mem_read8(pattern - 1u) & ~0xFu))
							memory_test_errors |= 0x8000u;
					}
					if ((mem_read8(ram_cursor - 1u) & 0xFu) != (mem_read8(pattern - 1u) & 0xFu))
						memory_test_errors |= 0x4000u;
					if ((mem_read8(ram_cursor - 1u) & ~0xFu) != (mem_read8(pattern - 1u) & ~0xFu))
						memory_test_errors |= 0x2000u;
				}
				if ((int32_t)ram_cursor >= 0x320000)
					return;
				bytes_left--;
			} while (bytes_left != 0u);
			pattern_index--;
		} while (pattern_index != 0u);
		passes_left--;
	} while (passes_left != 0u);
}

void test_sprite_ram(void)
{
	uint32_t bytes_left = 0, passes_left = 0, pattern = 0, pattern_index = 0, ram_cursor = 0;
	uint8_t expected = 0, actual = 0;

	ram_cursor = 0x400000u;
	passes_left = 0x84u;
	do
	{
		pattern_index = 4u;
		do
		{
			pattern = ram_test_patterns[pattern_index];
			bytes_left = 0xF9u;
			do
			{
				mem_write8(ram_cursor, ROM8(pattern));
				ram_cursor++;
				pattern++;
				if ((int32_t)ram_cursor >= 0x420000)
					goto test_sprite_ram_verify;
				bytes_left--;
			} while (bytes_left != 0u);
			pattern_index--;
		} while (pattern_index != 0u);
		passes_left--;
	} while (passes_left != 0u);
test_sprite_ram_verify:
	ram_cursor = 0x400000u;
	passes_left = 0x84u;
	do
	{
		pattern_index = 4u;
		do
		{
			pattern = ram_test_patterns[pattern_index];
			bytes_left = 0xF9u;
			do
			{
				expected = ROM8(pattern);
				actual = mem_read8(ram_cursor);
				ram_cursor++;
				pattern++;
				if (actual != expected)
				{
					if ((ram_cursor & 1u) != 0u)
					{
						if ((mem_read8(ram_cursor - 1u) & 0xFu) != (mem_read8(pattern - 1u) & 0xFu))
							memory_test_errors |= 0x1000u;
						if ((mem_read8(ram_cursor - 1u) & ~0xFu) != (mem_read8(pattern - 1u) & ~0xFu))
							memory_test_errors |= 0x800u;
					}
					if ((mem_read8(ram_cursor - 1u) & 0xFu) != (mem_read8(pattern - 1u) & 0xFu))
						memory_test_errors |= 0x400u;
					if ((mem_read8(ram_cursor - 1u) & ~0xFu) != (mem_read8(pattern - 1u) & ~0xFu))
						memory_test_errors |= 0x200u;
				}
				if ((int32_t)ram_cursor >= 0x420000)
					return;
				bytes_left--;
			} while (bytes_left != 0u);
			pattern_index--;
		} while (pattern_index != 0u);
		passes_left--;
	} while (passes_left != 0u);
}

void test_palette_ram(void)
{
	uint32_t halfwords_left = 0, passes_left = 0, pattern = 0, pattern_index = 0, ram_cursor = 0;
	uint16_t expected_first = 0, actual_first = 0, expected_second = 0, actual_second = 0;

	ram_cursor = 0x600000u;
	passes_left = 8u;
	do
	{
		pattern_index = 4u;
		do
		{
			pattern = ram_test_patterns[pattern_index];
			halfwords_left = 0xF9u;
			do
			{
				mem_write16(ram_cursor, ROM16(pattern));
				ram_cursor += 2u;
				pattern += 2u;
				if ((int32_t)ram_cursor >= 0x604000)
					goto test_palette_ram_verify_first_half;
				halfwords_left--;
			} while (halfwords_left != 0u);
			pattern_index--;
		} while (pattern_index != 0u);
		passes_left--;
	} while (passes_left != 0u);
test_palette_ram_verify_first_half:
	ram_cursor = 0x600000u;
	passes_left = 8u;
	do
	{
		pattern_index = 4u;
		do
		{
			pattern = ram_test_patterns[pattern_index];
			halfwords_left = 0xF9u;
			do
			{
				expected_first = ROM16(pattern);
				actual_first = mem_read16(ram_cursor);
				ram_cursor += 2u;
				pattern += 2u;
				if (actual_first != expected_first)
				{
					if ((mem_read16(ram_cursor - 2u) & 0xFFu) != (mem_read16(pattern - 2u) & 0xFFu))
						memory_test_errors |= 0x100u;
					if ((mem_read16(ram_cursor - 2u) & ~0xFFu) != (mem_read16(pattern - 2u) & ~0xFFu))
						memory_test_errors |= 0x80u;
				}
				if ((int32_t)ram_cursor >= 0x604000)
					goto test_palette_ram_fill_second_half;
				halfwords_left--;
			} while (halfwords_left != 0u);
			pattern_index--;
		} while (pattern_index != 0u);
		passes_left--;
	} while (passes_left != 0u);
test_palette_ram_fill_second_half:
	ram_cursor = PALETTE_RAM + 0x4000u;
	passes_left = 8u;
	do
	{
		pattern_index = 4u;
		do
		{
			pattern = ram_test_patterns[pattern_index];
			halfwords_left = 0xF9u;
			do
			{
				mem_write16(ram_cursor, ROM16(pattern));
				ram_cursor += 2u;
				pattern += 2u;
				if ((int32_t)ram_cursor >= 0x608000)
					goto test_palette_ram_verify_second_half;
				halfwords_left--;
			} while (halfwords_left != 0u);
			pattern_index--;
		} while (pattern_index != 0u);
		passes_left--;
	} while (passes_left != 0u);
test_palette_ram_verify_second_half:
	ram_cursor = PALETTE_RAM + 0x4000u;
	passes_left = 8u;
	do
	{
		pattern_index = 4u;
		do
		{
			pattern = ram_test_patterns[pattern_index];
			halfwords_left = 0xF9u;
			do
			{
				expected_second = ROM16(pattern);
				actual_second = mem_read16(ram_cursor);
				ram_cursor += 2u;
				pattern += 2u;
				if (actual_second != expected_second)
				{
					if ((mem_read16(ram_cursor - 2u) & 0xFFu) != (mem_read16(pattern - 2u) & 0xFFu))
						memory_test_errors |= 0x40000u;
					if ((mem_read16(ram_cursor - 2u) & ~0xFFu) != (mem_read16(pattern - 2u) & ~0xFFu))
						memory_test_errors |= 0x20000u;
				}
				if ((int32_t)ram_cursor >= 0x608000)
					return;
				halfwords_left--;
			} while (halfwords_left != 0u);
			pattern_index--;
		} while (pattern_index != 0u);
		passes_left--;
	} while (passes_left != 0u);
}
