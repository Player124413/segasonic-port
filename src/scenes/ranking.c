/**
 *    Name:        scenes/ranking.c
 *    Description: the ranking: entering initials at game over, loading and resetting the table, the TOP15 display
 *    Version:     0.1.0
 *    Created:     12-10-2026
 *    Modified:    15-10-2026
 *    Author:      Sebastian Toyos
 *
 *    SegaSonic The Hedgehog (arcade, Sega System 32): the game's V60 program decompiled into C by
 *    tools/decompile.py, with the names of symbols.txt.
 */
#include "game.h"

void start_ranking_entry(void)
{
	uint8_t ranking_size = 0;
	uint16_t rings = 0;
	uint32_t block = 0, entry_x_index = 0, next_block = 0, object = 0, piece = 0, piece_copy = 0, pieces_left = 0, player = 0, previous_block = 0, record = 0, x_index = 0;

	PLAYER(current_player)->game_over_done = 0u;
	rings = PLAYER(current_player)->rings;
	r14 = 0xFu;
	ranking_size = 0xFu;
	record = ranking_ADDRESS;
	do
	{
		if ((int16_t)rings < (int16_t)mem_read16(record))
			break;
		r14 = r14 - 1u;
		record += 8u;
	} while (r14 != 0u);
	if ((int8_t)r14 < (int8_t)ranking_size)
	{
		player = OBJECT(object_cursor)->player;
		ranking_entry_players |= shl32(1u, (int)(player));
		block = ranking_entry_blocks[player];
		next_block = (block & ~0xFFFFu) | (uint16_t)(block + 0xA0u);
		mem_write16(block + 0x28u, next_block);
		object = block;
		init_object(object);
		pieces_left = 4u;
		do
		{
			piece = next_block;
			next_block = (next_block & ~0xFFFFu) | (uint16_t)(next_block + 0x30u);
			mem_write16(piece + 0x28u, next_block);
			piece_copy = piece;
			init_object_2(piece_copy);
			pieces_left--;
		} while (pieces_left != 0u);
		mem_write16(piece + 0x28u, 0xFFFFu);
		mem_write32(block + 0x10u, current_player);
		mem_write8(block + 0x47u, player);
		entry_x_index = player;
		if ((cabinet_config & 1u) == 0u)
			entry_x_index += 3u;
		mem_write16(block + 0xEu, ranking_entry_x[entry_x_index]);
		mem_write16(block + 0xCu, 0xD0u);
		mem_write32(block + 0x3Cu, ranking_entry_trackballs[player]);
		mem_write8(block + 0x80u, ranking_entry_buttons[player]);
		mem_write16(block + 0x84u, 0x4B0u);
		mem_write16(block + 0x24u, r14);
		mem_write8(block + 0x36u, OBJECT(current_object)->kind);
		mem_write16(block + 0x34u, PLAYER(current_player)->rings);
		mem_write8(block + 0x37u, PLAYER(current_player)->stages_reached);
		mem_write32(block + 0x1Cu, 0u);
		previous_block = (piece_copy & ~0xFFFFu) | (uint16_t)block;
		block = (block & ~0xFFFFu) | mem_read16(block + 0x28u);
		mem_write16(block + 4u, 0xA238u);
		mem_write16(block + 0xEu, mem_read16(previous_block + 0xEu));
		mem_write16(block + 0xCu, mem_read16(previous_block + 0xCu));
		mem_write16(block + 0xEu, mem_read16(block + 0xEu) + 0xDu);
		mem_write16(block + 6u, 0x8AD0u);
		mem_write8(block + 0x2Au, 0x80u);
		previous_block = (previous_block & ~0xFFFFu) | (uint16_t)block;
		block = (block & ~0xFFFFu) | mem_read16(block + 0x28u);
		mem_write16(block + 4u, 0xA238u);
		mem_write16(block + 0xEu, mem_read16(previous_block + 0xEu));
		mem_write16(block + 0xCu, mem_read16(previous_block + 0xCu));
		mem_write16(block + 0xEu, mem_read16(block + 0xEu) + 0xDu);
		mem_write16(block + 6u, 0x8AD0u);
		mem_write8(block + 0x2Au, 0x80u);
		previous_block = (previous_block & ~0xFFFFu) | (uint16_t)block;
		block = (block & ~0xFFFFu) | mem_read16(block + 0x28u);
		mem_write16(block + 4u, 0xA238u);
		mem_write16(block + 0xEu, mem_read16(previous_block + 0xEu));
		mem_write16(block + 0xCu, mem_read16(previous_block + 0xCu));
		mem_write16(block + 0xEu, mem_read16(block + 0xEu) + 0xDu);
		mem_write16(block + 6u, 0x8AD0u);
		mem_write8(block + 0x2Au, 0x80u);
		previous_block = (previous_block & ~0xFFFFu) | (uint16_t)block;
		block = (block & ~0xFFFFu) | mem_read16(block + 0x28u);
		mem_write16(block + 0xEu, mem_read16(previous_block + 0xEu));
		mem_write16(block + 0xCu, mem_read16(previous_block + 0xCu));
		mem_write16(block + 0xEu, mem_read16(block + 0xEu) + 0xDu);
		mem_write8(block + 0x2Du, 0x22u);
		mem_write16(block + 4u, 0xA230u);
		mem_write16(block + 6u, 0x8AD0u);
		return;
	}
	player = OBJECT(object_cursor)->player;
	ranking_entry_players |= shl32(1u, (int)(player));
	block = ranking_entry_blocks[player];
	x_index = OBJECT(object_cursor)->player;
	if ((cabinet_config & 1u) == 0u)
		x_index += 3u;
	mem_write16(block + 0xEu, ranking_entry_x[x_index]);
	mem_write16(block + 0x82u, 0x78u);
	mem_write32(block + 0x10u, current_player);
	mem_write32(block + 0x1Cu, 1u);
}

void enter_ranking_initials(uint32_t jump_offset)
{
	uint16_t abs_dx = 0, abs_dy = 0, dx = 0, dy = 0, speed_x_fraction = 0, y = 0;
	uint32_t blink_timer = 0, button_bit = 0, copy_dest = 0, copy_source = 0, count = 0, crc = 0, data = 0, dest = 0, ignored = 0, letters_left = 0, links_left = 0, object = 0, palette = 0, player = 0, read_cursor = 0, record_initials = 0, record_rings = 0, records_left = 0, scale = 0, screen_x = 0, slot = 0, speed_y = 0, text = 0, text_x = 0, trackball = 0, x = 0;
	uint16_t out_of_rank_timer = 0, time_left = 0, slot_position = 0, letter_position = 0, direction = 0;

	player = OBJECT(object_cursor)->player;
	if ((ranking_entry_players & shl32(1u, (int)(player))) == 0u)
		return;
	current_object = ranking_entry_blocks[player];
	if (OBJECT(current_object)->handler != 0u)
	{
		out_of_rank_timer = OBJECT(current_object)->var_82;
		OBJECT(current_object)->var_82 = out_of_rank_timer - 1u;
		if (out_of_rank_timer == 0u)
		{
			mem_write8(AS32(OBJECT(current_object)->hitbox_size_x) + 0x135u, 1u);
			ranking_entry_players &= ~shl32(1u, (int)(OBJECT(object_cursor)->player));
			return;
		}
		blink_timer = (jump_offset & ~0xFFFFu) | OBJECT(current_object)->var_82;
		if ((blink_timer & 3u) != 0u)
		{
			x = (blink_timer & ~0xFFFFu) | (uint16_t)OBJECT(current_object)->screen_x;
			y = 0xC8u;
			palette = 0x8F70u;
			scale = 0x100u;
			text = out_of_rank_text_ADDRESS;
			queue_string_at(x, y, &palette, &scale, &text);
		}
		return;
	}
	r14 = current_object;
	button_bit = OBJECT(current_object)->var_80;
	if ((inputs_pressed & shl32(1u, (int)(button_bit))) == 0u)
	{
	enter_ranking_initials_count_down:
		time_left = OBJECT(current_object)->var_84;
		OBJECT(current_object)->var_84 = time_left - 1u;
		if (time_left != 0u)
		{
			screen_x = (uint16_t)OBJECT(current_object)->screen_x;
			y = (uint16_t)(OBJECT(current_object)->screen_y - 0x20u);
			palette = 0x8AD0u;
			scale = 0x100u;
			text = countdown_second_texts_ADDRESS + (int32_t)OBJECT(current_object)->var_84 / 0x3C * 3u;
			queue_string_at(screen_x, y, &palette, &scale, &text);
			text_x = (uint16_t)OBJECT(current_object)->screen_x;
			text_x = (uint16_t)(text_x + 0x20u);
			y = (uint16_t)(OBJECT(current_object)->screen_y - 0x20u);
			palette = 0x8AD0u;
			scale = 0x100u;
			text = ranking_rank_texts_ADDRESS + OBJECT(current_object)->direction * 0xAu;
			queue_string_at(text_x, y, &palette, &scale, &text);
			trackball = OBJECT(current_object)->speed_z;
			abs_dx = mem_read16(trackball);
			dx = (uint16_t)abs_dx;
			trackball += 2u;
			if ((int16_t)abs_dx < 0)
				abs_dx = (uint16_t)(0u - abs_dx);
			abs_dy = mem_read16(trackball);
			dy = (uint16_t)abs_dy;
			if ((int16_t)abs_dy < 0)
				abs_dy = (uint16_t)(0u - abs_dy);
			if ((int16_t)abs_dy < (int16_t)abs_dx)
			{
				slot_position = OBJECT(current_object)->spawn_arg_1 + dx;
				OBJECT(current_object)->spawn_arg_1 = slot_position;
				if ((int16_t)slot_position < 0)
					OBJECT(current_object)->spawn_arg_1 = 0u;
				if (AT8(OBJECT(current_object)->spawn_arg_2, 1) != AT8(OBJECT(current_object)->spawn_arg_1, 1))
				{
					AS8(OBJECT(current_object)->spawn_arg_1) = 0x80u;
					OBJECT(current_object)->spawn_arg_2 = OBJECT(current_object)->spawn_arg_1;
				}
			}
			links_left = AT8(OBJECT(current_object)->spawn_arg_1, 1);
			slot = links_left;
			links_left = (uint8_t)(links_left + 1u);
			if ((int8_t)links_left > 4)
			{
				AT8(OBJECT(current_object)->spawn_arg_1, 1) = 3u;
				links_left = 4u;
				slot = 3u;
			}
			do
			{
				current_object = LINKED(current_object, OBJECT(current_object)->child);
				links_left--;
			} while (links_left != 0u);
			AS8(OBJECT(current_object)->projected_y_int) = 1u;
			if (slot != 3u && dy != 0u)
			{
				letter_position = OBJECT(current_object)->projected_x + dy;
				OBJECT(current_object)->projected_x = letter_position;
				if ((int16_t)letter_position < 0)
					AT16(OBJECT(current_object)->projected_x, 1) = 0x2Bu;
				if ((int8_t)AT8(OBJECT(current_object)->projected_x, 1) > 0x2B)
					AT8(OBJECT(current_object)->projected_x, 1) = 0u;
				if (AT8(OBJECT(current_object)->projected_y, 1) != AT8(OBJECT(current_object)->projected_x, 1))
					AS8(OBJECT(current_object)->projected_x) = 0x80u;
				OBJECT(current_object)->projected_y_fraction = OBJECT(current_object)->projected_x;
				mem_write8(ranking_initial_fields[slot] + r14, AT8(OBJECT(current_object)->projected_x, 1));
			}
			current_object = ranking_entry_blocks[OBJECT(object_cursor)->player];
			letters_left = 3u;
			do
			{
				current_object = LINKED(current_object, OBJECT(current_object)->child);
				if (AS8(OBJECT(current_object)->projected_y_int) == 0u)
				{
				enter_ranking_initials_draw_letter:
					queue_sprite_on_top();
				}
				else
				{
					AS8(OBJECT(current_object)->projected_y_int) = 0u;
					OBJECT(current_object)->sprite_frame = ranking_letter_sprites[AT8(OBJECT(current_object)->projected_x, 1)];
					OBJECT(current_object)->direction++;
					if ((OBJECT(current_object)->direction & 3u) != 0u)
						goto enter_ranking_initials_draw_letter;
				}
				object = current_object;
				letters_left--;
			} while (letters_left != 0u);
			current_object = LINKED(current_object, OBJECT(current_object)->child);
			if (AS8(OBJECT(current_object)->projected_y_int) == 0u)
			{
				OBJECT(current_object)->screen_x = OBJECT(object)->screen_x;
				OBJECT(current_object)->screen_y = OBJECT(object)->screen_y;
				OBJECT(current_object)->screen_x += 0x18u;
				queue_sprite_on_top();
			}
			else
			{
				AS8(OBJECT(current_object)->projected_y_int) = 0u;
				direction = OBJECT(current_object)->direction;
				OBJECT(current_object)->direction = direction - 1u;
				if (direction != 0u)
				{
					OBJECT(current_object)->screen_x = OBJECT(object)->screen_x;
					OBJECT(current_object)->screen_y = OBJECT(object)->screen_y;
					OBJECT(current_object)->screen_x += 0x18u;
					queue_sprite_on_top();
					current_object = object_cursor;
					return;
				}
				OBJECT(current_object)->direction = 3u;
			}
			current_object = object_cursor;
			return;
		}
	enter_ranking_initials_save_record:
		mem_write8(AS32(OBJECT(current_object)->hitbox_size_x) + 0x135u, 1u);
		speed_x_fraction = OBJECT(current_object)->speed_x_fraction;
		read_cursor = ranking_ADDRESS;
		if ((int16_t)speed_x_fraction < (int16_t)AS16(ranking))
		{
			current_object = object_cursor;
			ranking_entry_players &= ~shl32(1u, (int)(OBJECT(object_cursor)->player));
			return;
		}
		dest = read_cursor;
		r14 = 0xEu;
		read_cursor += 8u;
		do
		{
			record_rings = mem_read32(read_cursor);
			read_cursor += 4u;
			if ((int16_t)speed_x_fraction < (int16_t)record_rings)
				break;
			record_initials = mem_read32(read_cursor);
			mem_write32(dest, record_rings);
			dest += 4u;
			mem_write32(dest, record_initials);
			r14 = r14 - 1u;
			read_cursor += 4u;
			dest += 4u;
		} while (r14 != 0u);
		speed_y = OBJECT(current_object)->speed_y;
		if (speed_y == 0x170412u || speed_y == 0x171717u || speed_y == 0x172B17u || speed_y == 0x2B1717u)
			OBJECT(current_object)->speed_y = 0xA0013;
		mem_write16(dest, OBJECT(current_object)->speed_x_fraction);
		dest += 2u;
		mem_write8(dest, AS8(OBJECT(current_object)->speed_x_int));
		dest++;
		mem_write8(dest, AT8(OBJECT(current_object)->speed_x_int, 1));
		dest++;
		mem_write8(dest, AS8(OBJECT(current_object)->speed_y));
		dest++;
		mem_write8(dest, AT8(OBJECT(current_object)->speed_y, 1));
		dest++;
		mem_write8(dest, AS8(OBJECT(current_object)->speed_y_int));
		mem_write8(dest + 1u, AT8(OBJECT(current_object)->speed_y_int, 1));
		copy_source = ranking_ADDRESS;
		copy_dest = saved_ranking;
		records_left = 0xFu;
		do
		{
			mem_write32(copy_dest, mem_read32(copy_source));
			copy_source += 4u;
			copy_dest += 4u;
			mem_write32(copy_dest, mem_read32(copy_source));
			records_left--;
			copy_source += 4u;
			copy_dest += 4u;
		} while (records_left != 0u);
		data = ranking_ADDRESS;
		count = 0x78u;
		compute_crc16(count, &crc, &data, &ignored);
		mem_write16(ranking_checksum, crc);
		current_object = object_cursor;
		ranking_entry_players &= ~shl32(1u, (int)(OBJECT(object_cursor)->player));
		return;
	}
	if (AT8(OBJECT(current_object)->spawn_arg_1, 1) == 3u)
		goto enter_ranking_initials_save_record;
	AT8(OBJECT(current_object)->spawn_arg_1, 1)++;
	if ((int8_t)AT8(OBJECT(current_object)->spawn_arg_1, 1) <= 3)
		goto enter_ranking_initials_count_down;
	AT8(OBJECT(current_object)->spawn_arg_1, 1) = 3u;
	goto enter_ranking_initials_count_down;
}

uint32_t load_ranking(void)
{
	uint32_t count = 0, count_2 = 0, crc = 0, data = 0, dest = 0, ignored = 0, source = 0;

	data = saved_ranking;
	count_2 = 0x78u;
	compute_crc16(count_2, &crc, &data, &ignored);
	if (mem_read16(ranking_checksum) != (uint16_t)crc)
		return reset_ranking();
	source = saved_ranking;
	dest = ranking_ADDRESS;
	count = 0xFu;
	return copy_ranking_records(source, dest, count);
}

uint32_t reset_ranking(void)
{
	uint32_t count = 0, dest = 0, letter = 0, read_cursor = 0, records_left = 0, source = 0, write_cursor = 0;
	uint8_t second_letter = 0, third_letter = 0, last_byte = 0;

	read_cursor = default_ranking_ADDRESS;
	write_cursor = ranking_ADDRESS;
	records_left = 0xFu;
	do
	{
		mem_write16(write_cursor, ROM16(read_cursor));
		write_cursor += 2u;
		read_cursor += 2u;
		mem_write16(write_cursor, ROM16(read_cursor));
		write_cursor += 2u;
		read_cursor += 2u;
		letter = ROM8(read_cursor);
		read_cursor++;
		letter = ascii_to_ranking_letter(letter);
		mem_write8(write_cursor, letter);
		second_letter = ROM8(read_cursor);
		read_cursor++;
		letter = second_letter;
		write_cursor++;
		letter = ascii_to_ranking_letter(letter);
		mem_write8(write_cursor, letter);
		third_letter = ROM8(read_cursor);
		read_cursor++;
		letter = third_letter;
		write_cursor++;
		letter = ascii_to_ranking_letter(letter);
		mem_write8(write_cursor, letter);
		last_byte = ROM8(read_cursor);
		read_cursor++;
		write_cursor++;
		mem_write8(write_cursor, last_byte);
		records_left--;
		write_cursor++;
	} while (records_left != 0u);
	source = ranking_ADDRESS;
	dest = saved_ranking;
	count = 0xFu;
	return copy_ranking_records(source, dest, count);
}

uint32_t copy_ranking_records(uint32_t source, uint32_t dest, uint32_t count)
{
	uint32_t count_2 = 0, crc = 0, data = 0, polynomial = 0;

	do
	{
		mem_write32(dest, mem_read32(source));
		source += 4u;
		dest += 4u;
		mem_write32(dest, mem_read32(source));
		count--;
		source += 4u;
		dest += 4u;
	} while (count != 0u);
	data = ranking_ADDRESS;
	count_2 = 0x78u;
	compute_crc16(count_2, &crc, &data, &polynomial);
	mem_write16(ranking_checksum, crc);
	return polynomial;
}

uint32_t ascii_to_ranking_letter(uint32_t letter)
{
	if ((int8_t)letter >= 0x61)
	{
		letter = (letter & ~0xFFu) | (uint8_t)(letter - 0x61u);
		return letter;
	}
	if ((uint8_t)letter == 0x20u)
	{
		letter = (letter & ~0xFFu) | 0x2Bu;
		return letter;
	}
	do
	{
	} while ((uint8_t)letter != 0x33u);
	letter = (letter & ~0xFFu) | 0x23u;
	return letter;
}

void start_ranking_display(uint32_t display, uint32_t *p_rows, uint32_t *p_source)
{
	uint32_t ignored = 0, object = 0, rows = 0, source = 0;

	display = allocate_object_or_start_list(display);
	OBJECT(display)->parent = object_cursor;
	OBJECT(display)->child = 0xFFFFu;
	init_object(display);
	object = display;
	show_nbg0_picture(&ignored, &rows, &source, &ignored);
	OBJECT(object)->screen_x = 0xA0;
	OBJECT(object)->screen_y = -0x1A4;
	OBJECT(object)->handler = build_ranking_display_ADDRESS;
	*p_rows = rows;
	*p_source = source;
}

void build_ranking_display(void)
{
	uint32_t block = 0, character = 0, first_initial = 0, free_count = 0, hundreds_digit = 0, label_second_char = 0, object = 0, rank_label = 0, record = 0, remainder = 0, rows_left = 0, second_initial = 0, stages = 0, tens_digit = 0, third_initial = 0, thousands_digit = 0;

	record = ranking_ADDRESS;
	current_player = 0u;
	rows_left = 0xFu;
	OBJECT(current_object)->speed_z = 0x180;
	OBJECT(current_object)->handler = ranking_display_scroll_in_ADDRESS;
	do
	{
		spawn_count = 0xCu;
		spawn_object_with_pieces(0x08F28Bu, &third_initial, &tens_digit, &free_count, &object, &block);
		OBJECT(object)->screen_x = 0xC;
		rank_label = ranking_row_numbers_ADDRESS + ((current_player & ~0xFFu) | (uint8_t)(current_player * 2u));
		OBJECT(object)->sprite_frame = ranking_letter_sprites[mem_read8(rank_label)];
		OBJECT(object)->sprite_palette = 0x8AD0u;
		OBJECT(object)->spawn_arg_1 = rows_left;
		OBJECT(object)->handler = ranking_row_update_ADDRESS;
		object = LINKED(object, OBJECT(object)->child);
		OBJECT(object)->screen_x = 0x18;
		label_second_char = mem_read8(rank_label + 1u);
		OBJECT(object)->sprite_frame = ranking_letter_sprites[label_second_char];
		OBJECT(object)->sprite_palette = 0x8AD0u;
		remainder = mem_read16(record);
		remainder = remainder & 0x7FFFu;
		object = LINKED(object, OBJECT(object)->child);
		OBJECT(object)->screen_x = 0xC0;
		thousands_digit = (int32_t)remainder / 0x3E8;
		thousands_digit = (thousands_digit & ~0xFFu) | (uint8_t)(thousands_digit + 0x20u);
		OBJECT(object)->sprite_frame = ranking_letter_sprites[thousands_digit];
		OBJECT(object)->sprite_palette = 0x8AC0u;
		record += 2u;
		if ((uint8_t)thousands_digit != 0x20u)
			remainder -= ((thousands_digit & ~0xFFu) | (uint8_t)(thousands_digit - 0x20u)) * 0x3E8u;
		object = LINKED(object, OBJECT(object)->child);
		OBJECT(object)->screen_x = 0xCC;
		hundreds_digit = (int32_t)(uint16_t)remainder / 0x64;
		hundreds_digit = (hundreds_digit & ~0xFFu) | (uint8_t)(hundreds_digit + 0x20u);
		OBJECT(object)->sprite_frame = ranking_letter_sprites[hundreds_digit];
		OBJECT(object)->sprite_palette = 0x8AC0u;
		if ((uint8_t)hundreds_digit != 0x20u)
			remainder -= ((hundreds_digit & ~0xFFu) | (uint8_t)(hundreds_digit - 0x20u)) * 0x64u;
		object = LINKED(object, OBJECT(object)->child);
		OBJECT(object)->screen_x = 0xD8;
		tens_digit = (int32_t)(uint16_t)remainder / 0xA;
		tens_digit = (tens_digit & ~0xFFu) | (uint8_t)(tens_digit + 0x20u);
		OBJECT(object)->sprite_frame = ranking_letter_sprites[tens_digit];
		OBJECT(object)->sprite_palette = 0x8AC0u;
		if ((uint8_t)tens_digit != 0x20u)
			remainder -= ((tens_digit & ~0xFFu) | (uint8_t)(tens_digit - 0x20u)) * 0xAu;
		object = LINKED(object, OBJECT(object)->child);
		OBJECT(object)->screen_x = 0xE4;
		OBJECT(object)->sprite_frame = ranking_letter_sprites[(remainder & ~0xFFu) | (uint8_t)(remainder + 0x20u)];
		OBJECT(object)->sprite_palette = 0x8AC0u;
		character = mem_read8(record);
		character = character % 3u;
		object = LINKED(object, OBJECT(object)->child);
		OBJECT(object)->sprite_flags |= 0x100u;
		OBJECT(object)->screen_x = 0x68;
		OBJECT(object)->sprite_palette = ranking_portrait_palettes[character];
		AS8(OBJECT(object)->projected_y_int) = character;
		object = LINKED(object, OBJECT(object)->child);
		OBJECT(object)->screen_x = 0x96;
		OBJECT(object)->sprite_frame = ranking_name_sprites[character];
		OBJECT(object)->sprite_palette = 0x8B10u;
		record++;
		stages = mem_read8(record);
		stages = stages % 9u;
		object = LINKED(object, OBJECT(object)->child);
		OBJECT(object)->screen_x = 0x114;
		OBJECT(object)->sprite_frame = ranking_stage_sprites[(uint8_t)(stages - 1u)];
		OBJECT(object)->sprite_palette = 0x8AD0u;
		object = LINKED(object, OBJECT(object)->child);
		OBJECT(object)->screen_x = 0x30;
		record++;
		first_initial = mem_read8(record);
		OBJECT(object)->sprite_frame = ranking_letter_sprites[first_initial % 0x2Cu];
		OBJECT(object)->sprite_palette = 0x8AD0u;
		object = LINKED(object, OBJECT(object)->child);
		OBJECT(object)->screen_x = 0x3C;
		record++;
		second_initial = mem_read8(record);
		OBJECT(object)->sprite_frame = ranking_letter_sprites[second_initial % 0x2Cu];
		OBJECT(object)->sprite_palette = 0x8AD0u;
		object = LINKED(object, OBJECT(object)->child);
		OBJECT(object)->screen_x = 0x48;
		record++;
		third_initial = mem_read8(record);
		OBJECT(object)->sprite_frame = ranking_letter_sprites[third_initial % 0x2Cu];
		OBJECT(object)->sprite_palette = 0x8AD0u;
		current_player = (current_player & ~0xFFu) | (uint8_t)(current_player + 1u);
		rows_left--;
		record += 2u;
	} while (rows_left != 0u);
	spawn_count = 5u;
	spawn_object_with_pieces(0x08F482u, &third_initial, &tens_digit, &free_count, &object, &block);
	ranking_title_layout++;
	ranking_title_layout &= 1u;
	OBJECT(object)->screen_x = 0xA0;
	OBJECT(object)->screen_y = 0x70;
	OBJECT(object)->sprite_scale = 0x10u;
	OBJECT(object)->sprite_frame = 0xA2B0u;
	OBJECT(object)->sprite_palette = 0x8B20u;
	OBJECT(object)->handler = ranking_title_spin_in_ADDRESS;
	object = LINKED(object, OBJECT(object)->child);
	OBJECT(object)->sprite_frame = 0xA2A8u;
	OBJECT(object)->sprite_palette = 0x8B20u;
	object = LINKED(object, OBJECT(object)->child);
	OBJECT(object)->sprite_frame = 0xA340u;
	OBJECT(object)->sprite_palette = 0x8B20u;
	object = LINKED(object, OBJECT(object)->child);
	OBJECT(object)->sprite_frame = 0xA2D0u;
	OBJECT(object)->sprite_palette = 0x8B20u;
	object = LINKED(object, OBJECT(object)->child);
	OBJECT(object)->sprite_frame = 0xA360u;
	OBJECT(object)->sprite_palette = 0x8B20u;
}

void ranking_title_spin_in(void)
{
	uint32_t direction = 0, speed = 0, speed_x = 0, speed_y = 0;

	if ((int16_t)OBJECT(current_object)->sprite_scale < 0x200)
		OBJECT(current_object)->sprite_scale += 2u;
	OBJECT(current_object)->direction += 8u;
	OBJECT(current_object)->direction &= 0x3FFu;
	direction = OBJECT(current_object)->direction;
	speed = 0x2000u;
	direction_to_speed(speed, &direction, &speed_x, &speed_y);
	OBJECT(current_object)->speed_x = speed_x;
	OBJECT(current_object)->speed_y = speed_y;
	if ((int16_t)OBJECT(current_object)->sprite_scale < 0x200 || OBJECT(current_object)->direction != 0x300u)
	{
		draw_ranking_title();
		return;
	}
	OBJECT(current_object)->ranking_title.pause_timer = 0x1C;
	OBJECT(current_object)->handler = ranking_title_pause_ADDRESS;
	ranking_title_pause();
}

void ranking_title_pause(void)
{
	uint16_t pause_timer = 0;

	pause_timer = OBJECT(current_object)->ranking_title.pause_timer;
	OBJECT(current_object)->ranking_title.pause_timer = pause_timer - 1u;
	if (pause_timer != 0u)
	{
		draw_ranking_title();
		return;
	}
	OBJECT(current_object)->handler = ranking_title_rise_ADDRESS;
	ranking_title_rise();
}

void ranking_title_rise(void)
{
	OBJECT(current_object)->screen_y--;
	if (OBJECT(current_object)->screen_y > 0x23)
	{
		draw_ranking_title();
		return;
	}
	OBJECT(current_object)->screen_y = 0x23;
	OBJECT(current_object)->handler = draw_ranking_title_ADDRESS;
	draw_ranking_title();
}

void draw_ranking_title(void)
{
	uint32_t r1 = 0, layout_offset = 0;

	r1 = queue_sprite(0u);
	current_object = LINKED(current_object, OBJECT(current_object)->child);
	OBJECT(current_object)->sprite_scale = OBJECT(object_cursor)->sprite_scale;
	OBJECT(current_object)->screen_x = OBJECT(object_cursor)->screen_x;
	OBJECT(current_object)->screen_y = OBJECT(object_cursor)->screen_y;
	OBJECT(current_object)->screen_x += OBJECT(object_cursor)->speed_x_int;
	OBJECT(current_object)->screen_y += OBJECT(object_cursor)->speed_y_int;
	r1 = queue_sprite(r1);
	current_object = LINKED(current_object, OBJECT(current_object)->child);
	OBJECT(current_object)->sprite_scale = OBJECT(object_cursor)->sprite_scale;
	OBJECT(current_object)->screen_x = OBJECT(object_cursor)->screen_x;
	OBJECT(current_object)->screen_y = OBJECT(object_cursor)->screen_y;
	OBJECT(current_object)->screen_x -= OBJECT(object_cursor)->speed_x_int;
	OBJECT(current_object)->screen_y -= OBJECT(object_cursor)->speed_y_int;
	r1 = queue_sprite(r1);
	current_object = LINKED(current_object, OBJECT(current_object)->child);
	OBJECT(current_object)->sprite_scale = OBJECT(object_cursor)->sprite_scale;
	OBJECT(current_object)->screen_x = OBJECT(object_cursor)->screen_x;
	OBJECT(current_object)->screen_y = OBJECT(object_cursor)->screen_y;
	OBJECT(current_object)->screen_x += OBJECT(object_cursor)->speed_x_int * 2u;
	layout_offset = (r1 & ~0xFFFFu) | (uint16_t)OBJECT(object_cursor)->speed_y_int;
	layout_offset = (layout_offset & ~0xFFFFu) | (uint16_t)(layout_offset * 2u);
	if (ranking_title_layout == 0u)
		OBJECT(current_object)->screen_x += layout_offset;
	else
		OBJECT(current_object)->screen_y += layout_offset;
	layout_offset = queue_sprite(layout_offset);
	current_object = LINKED(current_object, OBJECT(current_object)->child);
	OBJECT(current_object)->sprite_scale = OBJECT(object_cursor)->sprite_scale;
	OBJECT(current_object)->screen_x = OBJECT(object_cursor)->screen_x;
	OBJECT(current_object)->screen_y = OBJECT(object_cursor)->screen_y;
	OBJECT(current_object)->screen_x -= OBJECT(object_cursor)->speed_x_int * 2u;
	layout_offset = (layout_offset & ~0xFFFFu) | (uint16_t)OBJECT(object_cursor)->speed_y_int;
	layout_offset = (layout_offset & ~0xFFFFu) | (uint16_t)(layout_offset * 2u);
	if (ranking_title_layout == 0u)
		OBJECT(current_object)->screen_x -= layout_offset;
	else
		OBJECT(current_object)->screen_y -= layout_offset;
	queue_sprite(layout_offset);
}

void ranking_display_scroll_in(void)
{
	OBJECT(current_object)->screen_y++;
	if (OBJECT(current_object)->screen_y < 0x24)
	{
		drift_ranking_background();
		return;
	}
	OBJECT(current_object)->ranking_display.rows_animate = 1u;
	OBJECT(current_object)->handler = ranking_display_hold_ADDRESS;
	ranking_display_hold();
}

void ranking_display_hold(void)
{
	OBJECT(current_object)->ranking_display.hold_timer++;
	if (OBJECT(current_object)->ranking_display.hold_timer <= 0x12C)
	{
		drift_ranking_background();
		return;
	}
	AS16(OBJECT(current_object)->ranking_display.rows_remove) = 1u;
	OBJECT(current_object)->handler = ranking_display_end_ADDRESS;
	drift_ranking_background();
}

void drift_ranking_background(void)
{
	uint32_t direction = 0, speed = 0, speed_x = 0, speed_y = 0;
	uint16_t speed_fraction = 0;

	if ((uint16_t)OBJECT(current_object)->speed_z_int != 0u)
	{
	drift_ranking_background_swing_down:
		OBJECT(current_object)->speed_z_fraction -= 2u;
		if ((int16_t)OBJECT(current_object)->speed_z_fraction < 0x180)
			OBJECT(current_object)->speed_z_int = 0;
	}
	else
	{
		OBJECT(current_object)->speed_z_fraction += 2u;
		if ((int16_t)OBJECT(current_object)->speed_z_fraction > 0x280)
		{
			OBJECT(current_object)->speed_z_int = 1;
			goto drift_ranking_background_swing_down;
		}
	}
	if ((uint16_t)OBJECT(current_object)->speed_int != 0u)
	{
		speed_fraction = OBJECT(current_object)->speed_fraction;
		OBJECT(current_object)->speed_fraction = speed_fraction - 8u;
		if (speed_fraction < 8u)
			OBJECT(current_object)->speed = 0;
	}
	else
	{
		OBJECT(current_object)->speed_fraction += 8u;
		if ((int16_t)OBJECT(current_object)->speed_fraction > 0x300)
		{
			OBJECT(current_object)->speed_int = 1;
			speed_fraction = OBJECT(current_object)->speed_fraction;
			OBJECT(current_object)->speed_fraction = speed_fraction - 8u;
			if (speed_fraction < 8u)
				OBJECT(current_object)->speed = 0;
			direction = OBJECT(current_object)->speed_z_fraction;
			speed = 0x100u;
			direction_to_speed(speed, &direction, &speed_x, &speed_y);
			OBJECT(current_object)->speed_x += speed_x;
			OBJECT(current_object)->speed_y += speed_y;
			nbg0_scroll_x_int = OBJECT(current_object)->speed_x_int;
			nbg0_scroll_y_int = OBJECT(current_object)->speed_y_int;
			return;
		}
	}
	direction = OBJECT(current_object)->speed_z_fraction;
	speed = 0x100u;
	direction_to_speed(speed, &direction, &speed_x, &speed_y);
	OBJECT(current_object)->speed_x += speed_x;
	OBJECT(current_object)->speed_y += speed_y;
	nbg0_scroll_x_int = OBJECT(current_object)->speed_x_int;
	nbg0_scroll_y_int = OBJECT(current_object)->speed_y_int;
}

void ranking_display_end(void)
{
	uint32_t block_offset = 0, ignored = 0, slope_height = 0;

	ranking_done = 1u;
	delete_object_and_children(&ignored, &block_offset, &slope_height);
}

void ranking_row_update(void)
{
	uint8_t bucket = 0;
	uint32_t frame_index = 0, ignored = 0, portrait_frames = 0, slope_height = 0;
	uint16_t direction = 0;

	current_object = LINKED(current_object, OBJECT(current_object)->parent);
	OBJECT(object_cursor)->ranking_row.animate = OBJECT(current_object)->var_80;
	if (OBJECT(current_object)->var_81 != 0u)
	{
		delete_object_and_children(&ignored, &frame_index, &slope_height);
		return;
	}
	OBJECT(object_cursor)->screen_y = OBJECT(current_object)->screen_y;
	current_object = object_cursor;
	OBJECT(current_object)->screen_y += OBJECT(current_object)->ranking_row.row * 0x17u;
	bucket = 0u;
	queue_sprite_in_bucket(bucket);
	current_object = LINKED(current_object, OBJECT(current_object)->child);
	OBJECT(current_object)->screen_y = OBJECT(object_cursor)->screen_y;
	bucket = 0u;
	queue_sprite_in_bucket(bucket);
	current_object = LINKED(current_object, OBJECT(current_object)->child);
	OBJECT(current_object)->screen_y = OBJECT(object_cursor)->screen_y;
	bucket = 0u;
	queue_sprite_in_bucket(bucket);
	current_object = LINKED(current_object, OBJECT(current_object)->child);
	OBJECT(current_object)->screen_y = OBJECT(object_cursor)->screen_y;
	bucket = 0u;
	queue_sprite_in_bucket(bucket);
	current_object = LINKED(current_object, OBJECT(current_object)->child);
	OBJECT(current_object)->screen_y = OBJECT(object_cursor)->screen_y;
	bucket = 0u;
	queue_sprite_in_bucket(bucket);
	current_object = LINKED(current_object, OBJECT(current_object)->child);
	OBJECT(current_object)->screen_y = OBJECT(object_cursor)->screen_y;
	bucket = 0u;
	queue_sprite_in_bucket(bucket);
	current_object = LINKED(current_object, OBJECT(current_object)->child);
	OBJECT(current_object)->screen_y = OBJECT(object_cursor)->screen_y;
	if (OBJECT(object_cursor)->ranking_row.animate == 0u)
	{
	ranking_row_set_portrait_frame:
		portrait_frames = ranking_portrait_frames[AS8(OBJECT(current_object)->projected_y_int)];
		frame_index = AT8(OBJECT(current_object)->projected_y, 1);
		if (OBJECT(object_cursor)->ranking_row.row == 1u)
			frame_index += 0x14u;
		OBJECT(current_object)->sprite_frame = mem_read16(portrait_frames + frame_index * 2u);
		if (OBJECT(object_cursor)->ranking_row.animate != 0u && (int8_t)AT8(OBJECT(current_object)->projected_y, 1) < 0x13)
			AT8(OBJECT(current_object)->projected_y, 1)++;
	}
	else
	{
		direction = OBJECT(current_object)->direction;
		OBJECT(current_object)->direction = direction - 1u;
		if (direction == 0u)
		{
			OBJECT(current_object)->direction = 1u;
			goto ranking_row_set_portrait_frame;
		}
	}
	bucket = 0u;
	queue_sprite_in_bucket(bucket);
	current_object = LINKED(current_object, OBJECT(current_object)->child);
	OBJECT(current_object)->screen_y = OBJECT(object_cursor)->screen_y;
	bucket = 0u;
	queue_sprite_in_bucket(bucket);
	current_object = LINKED(current_object, OBJECT(current_object)->child);
	OBJECT(current_object)->screen_y = OBJECT(object_cursor)->screen_y;
	bucket = 0u;
	queue_sprite_in_bucket(bucket);
	current_object = LINKED(current_object, OBJECT(current_object)->child);
	OBJECT(current_object)->screen_y = OBJECT(object_cursor)->screen_y;
	bucket = 0u;
	queue_sprite_in_bucket(bucket);
	current_object = LINKED(current_object, OBJECT(current_object)->child);
	OBJECT(current_object)->screen_y = OBJECT(object_cursor)->screen_y;
	bucket = 0u;
	queue_sprite_in_bucket(bucket);
	current_object = LINKED(current_object, OBJECT(current_object)->child);
	OBJECT(current_object)->screen_y = OBJECT(object_cursor)->screen_y;
	bucket = 0u;
	queue_sprite_in_bucket(bucket);
}
