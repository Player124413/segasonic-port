/**
 *    Name:        engine/object_list.c
 *    Description: the object list and its two pools: reset, allocate, link, delete, run_objects
 *    Version:     0.1.0
 *    Created:     12-10-2026
 *    Modified:    15-10-2026
 *    Author:      Sebastian Toyos
 *
 *    SegaSonic The Hedgehog (arcade, Sega System 32): the game's V60 program decompiled into C by
 *    tools/decompile.py, with the names of symbols.txt.
 */
#include "game.h"

void reset_objects(uint32_t *p_block, uint32_t *p_stack_slot)
{
	uint32_t r0 = 0, r3 = 0;

	reset_object_list();
	reset_object_pool_2(&r0, &r3);
	run_objects();
	*p_block = r0;
	*p_stack_slot = r3;
}

void reset_objects_alt(void)
{
	reset_object_list();
	reset_object_pool_2_alt();
	run_objects();
}

void run_objects(void)
{
	free_objects = free_object_stack_ADDRESS;
	if (object_count == 0u)
		return;
	first_object = first_object_address;
	last_object = last_object_address;
	object_cursor = last_object;
	do
	{
		object_cursor = LINKED(object_cursor, OBJECT(object_cursor)->next);
		current_object = object_cursor;
		call_code(OBJECT(current_object)->handler);
	} while ((uint16_t)object_cursor != (uint16_t)last_object);
	AS16(first_object_address) = first_object;
	AS16(last_object_address) = last_object;
}

void reset_object_list(void)
{
	uint16_t object_offset = 0;
	uint32_t objects_left = 0, stack_slot = 0;

	object_offset = 0u;
	first_object_address = 0x200000u;
	last_object_address = 0x200000u;
	objects_left = 0x80u;
	free_object_stack = 0x80;
	stack_slot = 0x20E902u;
	do
	{
		mem_write16(stack_slot, object_offset);
		objects_left--;
		stack_slot += 2u;
		object_offset += 0xA0u;
	} while (objects_left != 0u);
	object_count = objects_left;
}

void take_free_object(uint32_t *p_free_count, uint32_t *p_object)
{
	uint32_t object = *p_object;
	uint32_t free_count = 0;

	free_count = mem_read16(free_objects);
	if (free_count == 0u)
	{
		*p_free_count = free_count;
		return;
	}
	object = 0x200000u | mem_read16(free_objects + free_count * 2u);
	mem_write16(free_objects, mem_read16(free_objects) - 1u);
	*p_free_count = free_count;
	*p_object = object;
}

uint32_t allocate_object_or_start_list(uint32_t object)
{
	uint32_t ignored = 0, new_object = 0;

	if (object_count != 0u)
	{
		allocate_object(&ignored, &object);
		return object;
	}
	new_object = 0x200000u | mem_read16(free_objects + mem_read16(free_objects) * 2u);
	mem_write16(free_objects, mem_read16(free_objects) - 1u);
	AS16(first_object_address) = new_object;
	first_object = new_object;
	last_object = new_object;
	link_object(new_object);
	return new_object;
}

void allocate_object(uint32_t *p_free_count, uint32_t *p_object)
{
	uint32_t object = *p_object;
	uint32_t free_count = 0;

	free_count = mem_read16(free_objects);
	if (free_count == 0u)
	{
		*p_free_count = free_count;
		return;
	}
	object = 0x200000u | mem_read16(free_objects + free_count * 2u);
	mem_write16(free_objects, mem_read16(free_objects) - 1u);
	link_object(object);
	*p_free_count = free_count;
	*p_object = object;
}

void link_object(uint32_t object)
{
	mem_write16(object + 0x20u, last_object);
	OBJECT(last_object)->next = object;
	OBJECT(first_object)->previous = object;
	mem_write16(object + 0x22u, first_object);
	AS16(last_object_address) = object;
	last_object = object;
	object_count++;
}

void allocate_object_after_current(void)
{
	uint32_t free_count = 0, new_object = 0, object = 0;

	free_count = mem_read16(free_objects);
	if (free_count == 0u)
		return;
	new_object = 0x200000u | mem_read16(free_objects + free_count * 2u);
	mem_write16(free_objects, mem_read16(free_objects) - 1u);
	object = LINKED(current_object, OBJECT(current_object)->next);
	OBJECT(current_object)->next = new_object;
	OBJECT(object)->previous = new_object;
	mem_write16(new_object + 0x20u, current_object);
	mem_write16(new_object + 0x22u, object);
	object_count++;
	if ((uint16_t)last_object == (uint16_t)current_object)
		last_object = (last_object & ~0xFFFFu) | (uint16_t)new_object;
}

void delete_current_object(void)
{
	uint32_t object = 0, object_2 = 0;

	object_count--;
	object = LINKED(object_cursor, OBJECT(object_cursor)->previous);
	object_2 = LINKED(object_cursor, OBJECT(object_cursor)->next);
	OBJECT(object)->next = object_2;
	OBJECT(object_2)->previous = object;
	if ((uint16_t)object_cursor == (uint16_t)first_object)
		first_object = (first_object & ~0xFFFFu) | (uint16_t)object_2;
	if ((uint16_t)object_cursor == (uint16_t)last_object)
		last_object = (last_object & ~0xFFFFu) | (uint16_t)object;
	current_object = object_cursor;
	object_cursor = (object_cursor & ~0xFFFFu) | (uint16_t)object;
	push_free_object();
}

void push_free_object(void)
{
	mem_write16(free_objects, mem_read16(free_objects) + 1u);
	mem_write16(free_objects + mem_read16(free_objects) * 2u, current_object);
}

void delete_next_object(void)
{
	uint32_t ignored = 0, object = 0;

	object = LINKED(object_cursor, OBJECT(current_object)->next);
	delete_object(object, &ignored, &ignored);
}

void delete_object(uint32_t object, uint32_t *p_free_count, uint32_t *p_previous)
{
	uint32_t free_count = 0, object_2 = 0, object_3 = 0;

	object_count--;
	object_2 = LINKED(object, OBJECT(object)->previous);
	object_3 = LINKED(object, OBJECT(object)->next);
	OBJECT(object_3)->previous = object_2;
	OBJECT(object_2)->next = object_3;
	if ((uint16_t)object == (uint16_t)first_object)
		first_object = (first_object & ~0xFFFFu) | (uint16_t)object_3;
	if ((uint16_t)object != (uint16_t)last_object)
	{
		mem_write16(free_objects, mem_read16(free_objects) + 1u);
		free_count = mem_read16(free_objects);
		mem_write16(free_objects + free_count * 2u, object);
		*p_free_count = free_count;
		*p_previous = object_2;
		return;
	}
	last_object = (last_object & ~0xFFFFu) | (uint16_t)object_2;
	mem_write16(free_objects, mem_read16(free_objects) + 1u);
	free_count = mem_read16(free_objects);
	mem_write16(free_objects + free_count * 2u, object);
	*p_free_count = free_count;
	*p_previous = object_2;
}

void reset_object_pool_2(uint32_t *p_block, uint32_t *p_stack_slot)
{
	uint32_t block = 0, pool_blocks_left = 0, stack_slot = 0;

	block = 0x205000u;
	pool_blocks_left = 0xFFu;
	free_object_stack_2[0] = 0xFF;
	stack_slot = 0x20EA12u;
	do
	{
		mem_write16(stack_slot, block);
		pool_blocks_left--;
		stack_slot += 2u;
		block += 0x30u;
	} while (pool_blocks_left != 0u);
	*p_block = block;
	*p_stack_slot = stack_slot;
}

void reset_object_pool_2_alt(void)
{
	uint16_t block = 0;
	uint32_t pool_blocks_left = 0, stack_slot = 0;

	block = 0x5000u;
	pool_blocks_left = 0x1D8u;
	free_object_stack_2[0] = 0x1D8;
	stack_slot = 0x20EA12u;
	do
	{
		mem_write16(stack_slot, block);
		pool_blocks_left--;
		stack_slot += 2u;
		block += 0x34u;
	} while (pool_blocks_left != 0u);
}

void allocate_object_2(uint32_t *p_free_count, uint32_t *p_free_stack, uint32_t *p_object)
{
	uint32_t object = *p_object;
	uint32_t free_count = 0, free_stack = 0;

	free_stack = free_object_stack_2_ADDRESS;
	free_count = (uint16_t)free_object_stack_2[0];
	if (free_count == 0u)
	{
		*p_free_count = free_count;
		*p_free_stack = free_stack;
		return;
	}
	object = 0x200000u | (uint16_t)free_object_stack_2[free_count];
	free_object_stack_2[0]--;
	*p_free_count = free_count;
	*p_free_stack = free_stack;
	*p_object = object;
}

uint32_t free_object_2(uint16_t object)
{
	uint32_t free_stack = 0;

	free_stack = free_object_stack_2_ADDRESS;
	free_object_stack_2[0]++;
	free_object_stack_2[(uint16_t)free_object_stack_2[0]] = object;
	return free_stack;
}
