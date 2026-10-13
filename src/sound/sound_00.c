/**
 *    Name:        sound_00.c
 *    Description: the sound program's functions, part 00 (in the order of their Z80 addresses)
 *    Version:     0.1.0
 *    Created:     13-10-2026
 *    Modified:    13-10-2026
 *    Author:      Sebastian Toyos
 *
 *    SegaSonic The Hedgehog (arcade, Sega System 32): the Z80 sound program decompiled into C by
 *    tools/decompile_sound.py, with the names of sound_symbols.txt.
 */
#include "sound.h"

void sound_reset(void)
{
	uint8_t bc = 0;

	z80_block(14);
	interrupts_off();
	z80_block(201);
	SOUND_RAM8(0xE000u) = 0u;
	move_block(0xE000u, 0xE001u, 0x1EFFu, 1u);
	bc = 0u;
	port_out(0xD0u, 0u);
	port_out(0xD1u, 1u);
	port_out(0xD2u, 2u);
	port_out(0xC0u, 0u);
	port_out(0xC1u, 0u);
	port_out(0xD8u, 0xBu);
	port_out(0xD3u, 0u);
	sound_0174(bc);
	z80_block(4);
	interrupts_on_after_block();
}

void sound_irq(void)
{
	z80_block(121);
	interrupts_off();
	port_out(0xC1u, 0u);
	port_out(0x80u, 0x27u);
	port_out(0x81u, 0x15u);
	sound_4645();
	z80_block(54);
	interrupts_on();
}

void sound_main_loop(void)
{
	uint32_t zero = 0, carry = 0;
	uint16_t hl = 0, iy = 0;
	uint16_t stack_2 = 0;

	do
	{
		z80_block(31);
		interrupts_off();
		zero = SOUND_RAM8(0xFFFFu) == 0u;
	} while (zero == 0u);
	z80_block(29);
	interrupts_on();
	carry = port_in(0x90u) & 1u;
	if (carry)
	{
		z80_block(61);
		port_out(0x90u, 0x27u);
		port_out(0x91u, 0x15u);
		sound_0BF4();
	}
	z80_block(53);
	iy = SOUND_RAM16(0xE032u);
	zero = z80_read8((uint16_t)(iy + 0xAu)) == 0u;
	if (zero == 0u)
	{
		z80_cycles(7);
		sound_61EF(iy);
	}
	z80_block(87);
	stack_2 = iy;
	hl = stack_2 + 0x12u;
	zero = (uint8_t)(0x68u - hl) == 0u;
	SOUND_RAM16(0xE032u) = hl;
	if (zero)
	{
		z80_block(26);
		SOUND_RAM16(0xE032u) = 0xE490u;
	}
	z80_block(17);
	sound_48E0();
	z80_block(17);
	sound_040B();
	z80_block(10);
}

void sound_0174(uint8_t bc)
{
	uint8_t a = 0, bc_3 = 0;
	uint16_t bc_2 = 0;

	z80_block(17);
	sound_02D1(bc);
	z80_block(17);
	bc_2 = sound_01D1();
	z80_block(278);
	SOUND_RAM8(0xE758u) = 0u;
	SOUND_RAM8(0xE002u) = 0xFFu;
	SOUND_RAM8(0xE740u) = 1u;
	SOUND_RAM16(0xE010u) = 0x8Cu;
	SOUND_RAM16(0xE02Fu) = 0xE490u;
	SOUND_RAM8(0xE02Eu) = 0xCu;
	SOUND_RAM8(0xE01Cu) = 0xFFu;
	SOUND_RAM8(0xE008u) = 0x80u;
	SOUND_RAM8(0xE000u) = 2u;
	SOUND_RAM8(0xE029u) = 8u;
	SOUND_RAM16(0xE027u) = 0xE400u;
	SOUND_RAM16(0xE032u) = 0xE490u;
	sound_0373((uint8_t)bc_2);
	z80_block(17);
	sound_039B();
	z80_block(17);
	sound_03B5();
	z80_block(31);
	a = 0x15u;
	bc_3 = 0x27u;
	sound_023D(bc_3, a);
	z80_block(31);
	a = 0x15u;
	bc_3 = 0x27u;
	sound_025B(bc_3, a);
	z80_block(10);
}

uint16_t sound_01D1(void)
{
	uint8_t a = 0, bc_2 = 0;
	uint16_t bc = 0, hl = 0;

	z80_block(17);
	hl = 0x273u;
	bc = 0xB00u;
	for (;;)
	{
		z80_block(43);
		bc |= z80_read8(hl);
		hl++;
		a = z80_read8(hl);
		hl++;
		sound_023D((uint8_t)bc, a);
		z80_block(67);
		sound_025B((uint8_t)bc, a);
		z80_block(8);
		bc = (uint32_t)(uint8_t)((bc >> 8) - 1u) << 8;
		if (bc >> 8 == 0u)
			break;
		z80_cycles(5);
	}
	z80_block(31);
	bc_2 = 0x24u;
	a = 0xFBu;
	sound_023D(bc_2, a);
	z80_block(81);
	bc_2 = 0x25u;
	a = 3u;
	sound_023D(bc_2, a);
	z80_block(17);
	hl = 0x289u;
	bc = 0x2400u;
	for (;;)
	{
		z80_block(43);
		bc = (bc & ~0xFFu) | z80_read8(hl);
		hl++;
		a = z80_read8(hl);
		hl++;
		sound_023D((uint8_t)bc, a);
		z80_block(67);
		sound_025B((uint8_t)bc, a);
		z80_block(67);
		sound_0249((uint8_t)bc, a);
		z80_block(67);
		sound_0267((uint8_t)bc, a);
		z80_block(8);
		bc = (bc & 0xFFu) | ((uint32_t)(uint8_t)((bc >> 8) - 1u) << 8);
		if (bc >> 8 == 0u)
			break;
		z80_cycles(5);
	}
	z80_block(10);
	return bc;
}

uint8_t sound_0221(uint8_t bc, uint8_t a)
{
	uint32_t zero = 0;
	uint8_t a_2 = 0;
	uint16_t bc_2 = 0;

	z80_block(39);
	bc_2 = (bc & 0xFFu) | ((uint32_t)a << 8);
	zero = ((SOUND_RAM8(0xE01Fu) >> 3) & 1u) == 0u;
	a_2 = (uint8_t)(bc_2 >> 8);
	if (zero)
	{
		sound_023D((uint8_t)bc_2, a_2);
		return a_2;
	}
	z80_block(10);
	sound_025B((uint8_t)bc_2, a_2);
	return a_2;
}

void sound_022E(uint8_t bc, uint8_t a)
{
	uint32_t zero = 0;
	uint8_t a_2 = 0;
	uint16_t bc_2 = 0;

	z80_block(35);
	bc_2 = (bc & 0xFFu) | ((uint32_t)a << 8);
	a_2 = SOUND_RAM8(0xE01Fu);
	zero = ((a_2 >> 3) & 1u) == 0u;
	if (zero == 0u)
	{
		z80_block(22);
		zero = ((a_2 >> 2) & 1u) == 0u;
		a_2 = (uint8_t)(bc_2 >> 8);
		if (zero == 0u)
		{
			sound_0267((uint8_t)bc_2, a_2);
			return;
		}
		sound_025B((uint8_t)bc_2, a_2);
		return;
	}
	z80_block(22);
	zero = ((a_2 >> 2) & 1u) == 0u;
	a_2 = (uint8_t)(bc_2 >> 8);
	if (zero == 0u)
	{
		sound_0249((uint8_t)bc_2, a_2);
		return;
	}
	sound_023D((uint8_t)bc_2, a_2);
}

void sound_023D(uint8_t bc, uint8_t a)
{
	uint8_t stack_1 = 0;

	z80_block(73);
	stack_1 = a;
	interrupts_off();
	port_out(0x80u, (uint8_t)bc);
	port_out(0x81u, stack_1);
	interrupts_on();
}

void sound_0249(uint8_t bc, uint8_t a)
{
	uint8_t stack_1 = 0;

	z80_block(73);
	stack_1 = a;
	interrupts_off();
	port_out(0x82u, (uint8_t)bc);
	port_out(0x83u, stack_1);
	interrupts_on();
}

void sound_025B(uint8_t bc, uint8_t a)
{
	uint8_t stack_1 = 0;

	z80_block(73);
	stack_1 = a;
	interrupts_off();
	port_out(0x90u, (uint8_t)bc);
	port_out(0x91u, stack_1);
	interrupts_on();
}

void sound_0267(uint8_t bc, uint8_t a)
{
	uint8_t stack_1 = 0;

	z80_block(73);
	stack_1 = a;
	interrupts_off();
	port_out(0x92u, (uint8_t)bc);
	port_out(0x93u, stack_1);
	interrupts_on();
}

void sound_02D1(uint8_t bc)
{
	uint8_t a = 0, bc_3 = 0;
	uint16_t bc_2 = 0, hl = 0;
	uint16_t stack_2 = 0;

	z80_block(14);
	a = 0u;
	bc_2 = (bc & 0xFFu) | 0x800u;
	for (;;)
	{
		z80_block(35);
		stack_2 = bc_2;
		bc_3 = 7u;
		sound_036D(bc_3, a);
		z80_block(84);
		z80_write8(0xD000u, 0x80u);
		move_block(0xD000u, 0xD001u, 0x1000u, 1u);
		a++;
		bc_3 = 7u;
		sound_036D(bc_3, a);
		z80_block(134);
		z80_write8(0xD000u, 0xFFu);
		move_block(0xD000u, 0xD001u, 0x1000u, 1u);
		z80_write8(0xD2BCu, 0x80u);
		move_block(0xD2BCu, 0xD2BDu, 0x800u, 1u);
		a++;
		bc_2 = (stack_2 & 0xFFu) | ((uint32_t)(uint8_t)((stack_2 >> 8) - 1u) << 8);
		if (bc_2 >> 8 == 0u)
			break;
		z80_cycles(5);
	}
	z80_block(17);
	hl = 0x31Bu;
	bc_2 = 0x2900u;
	for (;;)
	{
		z80_block(43);
		bc_2 |= z80_read8(hl);
		hl++;
		a = z80_read8(hl);
		hl++;
		sound_036D((uint8_t)bc_2, a);
		z80_block(8);
		bc_2 = (uint32_t)(uint8_t)((bc_2 >> 8) - 1u) << 8;
		if (bc_2 >> 8 == 0u)
			break;
		z80_cycles(5);
	}
	z80_block(10);
}

void sound_036D(uint8_t bc, uint8_t a)
{
	z80_block(45);
	z80_write8((bc & 0xFFu) | 0xC000u, a);
}

void sound_0373(uint8_t bc)
{
	uint16_t bc_2 = 0, de = 0, hl = 0;
	uint16_t stack_2 = 0;

	z80_block(27);
	hl = 0xE401u;
	de = 0x387u;
	bc_2 = (bc & 0xFFu) | 0x1400u;
	for (;;)
	{
		z80_block(70);
		stack_2 = bc_2;
		z80_write8(hl, z80_read8(de));
		de++;
		hl += 0x12u;
		bc_2 = (stack_2 & 0xFFu) | ((uint32_t)(uint8_t)((stack_2 >> 8) - 1u) << 8);
		if (bc_2 >> 8 == 0u)
			break;
		z80_cycles(5);
	}
	z80_block(10);
}

void sound_039B(void)
{
	uint16_t bc = 0, hl = 0;

	z80_block(17);
	hl = 0xE409u;
	bc = 0x800u;
	for (;;)
	{
		z80_block(98);
		z80_write8(hl, 0xD0u);
		hl += 7u;
		z80_write8(hl, 0xFFu);
		hl++;
		z80_write8(hl, 7u);
		hl += 0xAu;
		bc = (uint32_t)(uint8_t)((bc >> 8) - 1u) << 8;
		if (bc >> 8 == 0u)
			break;
		z80_cycles(5);
	}
	z80_block(10);
}

void sound_03B5(void)
{
	uint32_t zero = 0;
	uint8_t a = 0, de = 0;
	uint16_t bc = 0, hl = 0, iy = 0;

	z80_block(34);
	a = 1u;
	hl = 0x1594u;
	hl = sound_03FF(hl, a);
	z80_block(70);
	move_block(hl, 0xE300u, 0x48u, 1u);
	a = 0u;
	hl = 0x1594u;
	hl = sound_03FF(hl, a);
	z80_block(57);
	move_block(hl, 0xE348u, 0x2Du, 1u);
	hl += 0x2Du;
	iy = 0xE300u;
	bc = 0xD00u;
	for (;;)
	{
		z80_block(56);
		hl = (hl & 0xFFu) | ((uint32_t)z80_read8((uint16_t)(iy + 2u)) << 8);
		a = 0xFFu;
		if ((z80_read8(iy) & 1u) != 0u)
		{
		L_0003EA:
			z80_block(21);
			de = a;
			hl = sound_0DFE(de, hl);
			z80_block(18);
			a = (uint8_t)(hl >> 8);
			zero = a == 0u;
			a = a;
			if (zero == 0u)
			{
				z80_block(4);
				a++;
			}
			z80_block(52);
			z80_write8((uint16_t)(iy + 8u), a);
			iy += 9u;
			bc = (uint32_t)(uint8_t)((bc >> 8) - 1u) << 8;
			if (bc >> 8 == 0u)
				break;
			z80_cycles(5);
			continue;
		}
		z80_block(13);
		a = SOUND_RAM8(0xE01Cu);
		goto L_0003EA;
	}
	z80_block(10);
}

uint16_t sound_03FF(uint16_t hl, uint8_t a)
{
	uint16_t bc = 0, hl_2 = 0;

	z80_block(88);
	bc = a;
	hl = hl + bc + bc;
	hl_2 = ((uint32_t)z80_read8((uint16_t)(hl + 1u)) << 8) | z80_read8(hl);
	return hl_2;
}

void sound_040B(void)
{
	uint32_t zero = 0;
	uint8_t a = 0;
	uint16_t bc = 0, hl = 0, ignored16 = 0, ix = 0;
	uint16_t stack_2 = 0, stack_4 = 0;

	z80_block(26);
	hl = 0xE759u;
	zero = SOUND_RAM8(0xE759u) == 0u;
	if (zero)
	{
		z80_cycles(6);
		return;
	}
	z80_block(28);
	z80_write8(hl, z80_read8(hl) - 1u);
	sound_0B92(&bc, &ignored16, &a);
	z80_block(18);
	zero = a >> 7 == 0u;
	if (zero)
	{
		z80_block(13);
		a = SOUND_RAM8(0xE008u);
	}
	else
	{
		z80_block(23);
		SOUND_RAM8(0xE008u) = a;
	}
	z80_block(49);
	hl = ((a & 0xFu) << 8) | SOUND_RAM8(0xE002u);
	ix = 0xE300u;
	bc = (bc & 0xFFu) | 0xD00u;
	for (;;)
	{
		z80_block(30);
		if (z80_read8(ix) >> 7 == 0u)
		{
		L_000449:
			z80_block(33);
			ix += 9u;
			bc = (bc & 0xFFu) | ((uint32_t)(uint8_t)((bc >> 8) - 1u) << 8);
			if (bc >> 8 == 0u)
				break;
			z80_cycles(5);
			continue;
		}
		z80_block(33);
		zero = (uint8_t)((hl >> 8) - z80_read8((uint16_t)(ix + 1u))) == 0u;
		if (zero == 0u)
			goto L_000449;
		z80_block(39);
		stack_2 = bc;
		stack_4 = hl;
		sound_046C(bc, ix);
		z80_block(37);
		hl = stack_4;
		bc = stack_2;
		SOUND_RAM8(0xE002u) = (uint8_t)hl;
		goto L_000449;
	}
	sound_0450((uint8_t)bc);
}

void sound_0450(uint8_t bc)
{
	uint32_t zero = 0;
	uint8_t a = 0;
	uint16_t hl = 0;

	z80_block(28);
	hl = 0xE002u;
	zero = (uint8_t)bc >> 7 == 0u;
	if (zero == 0u)
	{
		z80_block(22);
		a = bc & 0xF0u;
		z80_write8(hl, z80_read8(hl) + 1u);
	}
	else
	{
		z80_block(30);
		a = SOUND_RAM8(0xE008u) & ~0xFu;
	}
	z80_block(12);
	zero = (uint8_t)(a + 0x40u) == 0u;
	if (zero)
	{
		z80_cycles(6);
		return;
	}
	z80_block(12);
	zero = (uint8_t)(a + 0x30u) == 0u;
	if (zero)
	{
		z80_cycles(6);
		return;
	}
	z80_block(21);
	z80_write8(hl, z80_read8(hl) + 1u);
}

void sound_046C(uint16_t bc, uint16_t ix)
{
	uint32_t zero = 0, carry = 0;
	uint8_t a = 0, bc_3 = 0, de_2 = 0, hl_2 = 0, ignored8 = 0;
	uint16_t bc_2 = 0, de = 0, hl = 0, ignored16 = 0, iy = 0;
	uint8_t stack_1 = 0;
	uint16_t stack_2 = 0, stack_4 = 0;
	uint8_t t1 = 0, t2 = 0, t3 = 0, t4 = 0, t5 = 0, t6 = 0, t7 = 0, t8 = 0, t9 = 0, t10 = 0, t11 = 0, t12 = 0, t13 = 0;

	z80_block(37);
	a = SOUND_RAM8(0xE008u) & ~0xFu;
	zero = (uint8_t)(a + 0x70u) == 0u;
	if (zero)
	{
		z80_block(18);
		zero = (uint8_t)bc >> 7 == 0u;
		if (zero == 0u)
		{
			z80_block(17);
			sound_0B92(&bc, &ignored16, &ignored8);
		}
		z80_block(96);
		bc = (bc & 0xFFu) | ((uint32_t)(uint8_t)bc << 8);
		SOUND_RAM8(0xE012u) = z80_read8((uint16_t)(0x1A3Eu + (uint8_t)(z80_read8((uint16_t)(ix + 4u)) + bc)));
		sound_0B92(&bc, &ignored16, &a);
		z80_block(14);
		zero = a == 0u;
		a = a;
		if (zero)
		{
		L_000714:
			z80_block(31);
			a = z80_read8((uint16_t)(ix + 3u));
			carry = a < 0xF0u;
			if (carry == 0u)
			{
				z80_cycles(6);
				return;
			}
			z80_block(27);
			hl = 0x193Bu;
			hl = sound_03FF(hl, a);
			z80_block(22);
			if (((z80_read8(hl) >> 1) & 1u) != 0u)
			{
				sound_1192(&ignored16, &hl, &ignored8);
				return;
			}
			z80_block(51);
			iy = 0xE400u;
			de = 0x12u;
			bc_2 = 0x800u;
			a = SOUND_RAM8(0xE008u) & 0xFu;
			for (;;)
			{
				z80_block(29);
				zero = (uint8_t)(a - z80_read8((uint16_t)(iy + 3u))) == 0u;
				if (zero == 0u)
				{
				L_000751:
					z80_block(23);
					iy += de;
					bc_2 = (uint32_t)(uint8_t)((bc_2 >> 8) - 1u) << 8;
					if (bc_2 >> 8 == 0u)
					{
						z80_block(10);
						return;
					}
					z80_cycles(5);
					continue;
				}
				z80_block(46);
				hl_2 = a;
				zero = (uint8_t)(SOUND_RAM8(0xE012u) - z80_read8((uint16_t)(iy + 2u))) == 0u;
				if (zero == 0u)
				{
				L_000750:
					z80_block(4);
					a = (uint8_t)hl_2;
					goto L_000751;
				}
				z80_block(37);
				a = z80_read8(iy);
				zero = ((a >> 1) & 1u) == 0u;
				if (zero == 0u)
					goto L_000750;
				z80_block(17);
				if ((a & 0x7Fu) != 0u)
					break;
				goto L_000750;
			}
			z80_block(65);
			if (z80_read8((uint16_t)(0xE060u + hl_2)) >> 7 == 0u)
			{
				sound_0D06(iy);
				return;
			}
			z80_block(33);
			z80_write8(iy, z80_read8(iy) | 2u);
			return;
		}
		z80_block(88);
		de = a;
		SOUND_RAM8(0xE013u) = z80_read8((uint16_t)(0x1BFEu + de));
		a = z80_read8((uint16_t)(ix + 3u));
		carry = a < 0xF0u;
		if (carry)
		{
			z80_block(27);
			hl = 0x193Bu;
			hl = sound_03FF(hl, a);
			z80_block(22);
			if (((z80_read8(hl) >> 1) & 1u) != 0u)
			{
				z80_block(76);
				SOUND_RAM8(0xE00Du) = z80_read8((uint16_t)(hl + 1u));
				hl = SOUND_RAM16(0xE02Fu);
				de = 0x12u;
				bc_2 = ((uint32_t)SOUND_RAM8(0xE02Eu) << 8) | 0xCu;
				for (;;)
				{
					z80_block(21);
					zero = z80_read8(hl) == 0u;
					if (zero)
						goto L_00105B;
					z80_block(23);
					bc_2 = (bc_2 & ~0xFFu) | (uint8_t)(bc_2 - 1u);
					hl += de;
					bc_2 = (bc_2 & 0xFFu) | ((uint32_t)(uint8_t)((bc_2 >> 8) - 1u) << 8);
					if (bc_2 >> 8 == 0u)
						break;
					z80_cycles(5);
				}
				z80_block(18);
				zero = (uint8_t)bc_2 == 0u;
				if (zero)
				{
					z80_block(14);
					return;
				}
				z80_block(24);
				hl = 0xE490u;
				de = 0x12u;
				bc_2 = (uint32_t)(uint8_t)bc_2 << 8;
				for (;;)
				{
					z80_block(21);
					zero = z80_read8(hl) == 0u;
					if (zero)
						break;
					z80_block(23);
					hl += de;
					bc_2 = (uint32_t)(uint8_t)((bc_2 >> 8) - 1u) << 8;
					if (bc_2 >> 8 == 0u)
					{
						z80_block(14);
						return;
					}
					z80_cycles(5);
				}
			L_00105B:
				z80_block(89);
				SOUND_RAM16(0xE036u) = hl;
				a = z80_read8((uint16_t)(0xE070u + (SOUND_RAM8(0xE008u) & 0xFu)));
				zero = a == 0u;
				a = a;
				if (zero == 0u)
				{
					z80_block(27);
					hl = 0x45E2u;
					hl = sound_03FF(hl, a);
					z80_block(353);
					de = ((uint32_t)(hl >> 8) << 8) | (uint8_t)hl;
					hl = SOUND_RAM16(0xE036u) + 7u;
					z80_write8(hl, z80_read8(de));
					de++;
					hl++;
					z80_write8(hl, z80_read8(de));
					de++;
					hl++;
					z80_write8(hl, z80_read8(de));
					de++;
					hl++;
					z80_write8(hl, z80_read8(de));
					de++;
					hl++;
					z80_write8(hl, z80_read8(de));
					de++;
					hl++;
					z80_write8(hl, z80_read8(de));
					z80_write8((uint16_t)(hl + 1u), z80_read8((uint16_t)(de + 1u)));
					iy = SOUND_RAM16(0xE036u);
					z80_write8((uint16_t)(iy + 0xDu), (z80_read8((uint16_t)(iy + 9u)) & ~0xFu) + z80_read8((uint16_t)(iy + 0xDu)));
					z80_write8((uint16_t)(iy + 0xEu), z80_read8((uint16_t)(iy + 8u)));
				}
				z80_block(31);
				hl = 0xE02Eu;
				t1 = SOUND_RAM8(0xE02Eu);
				SOUND_RAM8(0xE02Eu) = t1 - 1u;
				if (t1 != 1u)
				{
					z80_block(53);
					SOUND_RAM16(0xE02Fu) += 0x12u;
				}
				else
				{
					z80_block(46);
					z80_write8(hl, 0xCu);
					SOUND_RAM16(0xE02Fu) = 0xE490u;
				}
				z80_block(218);
				hl = SOUND_RAM16(0xE036u);
				z80_write8(hl, 0x40u);
				hl++;
				SOUND_RAM8(0xE01Fu) = z80_read8(hl);
				hl++;
				a = SOUND_RAM8(0xE012u);
				z80_write8(hl, a);
				bc_2 = a;
				hl++;
				carry = 0u;
				a = SOUND_RAM8(0xE008u) & 0xFu;
				z80_write8(hl, a);
				bc_2 |= (uint32_t)a << 8;
				hl += 2u;
				t2 = SOUND_RAM8(0xE013u);
				a = (t2 >> 1) | (carry << 7);
				carry = t2 & 1u;
				t3 = a;
				a = (t3 >> 1) | (carry << 7);
				carry = t3 & 1u;
				a = (((a >> 1) | (carry << 7)) & 0x1Fu) - 0x20u;
				z80_write8(hl, a);
				hl = (hl & 0xFFu) | ((uint32_t)a << 8);
				a = z80_read8((uint16_t)(ix + 8u));
				de_2 = a;
				zero = a == 0u;
				a = a;
				if (zero == 0u)
				{
					z80_block(20);
					de_2 += (uint8_t)~a >> 1;
				}
				z80_block(17);
				hl = sound_0DFE(de_2, hl);
				z80_block(425);
				SOUND_RAM8(0xE013u) = 0x7Fu - ((uint16_t)((uint32_t)((hl >> 8) >> 1) << 8) >> 8);
				de = bc_2 >> 8;
				SOUND_RAM8(0xE01Bu) = (z80_read8((uint16_t)(0xE060u + de)) & 7u) | 0xC0u;
				hl = 0xE040u + de + de;
				a = z80_read8((uint16_t)(ix + 5u));
				de_2 = a;
				t4 = a;
				a = ((uint32_t)t4 << 1) | (t4 >> 7);
				carry = t4 >> 7;
				stack_2 = (((uint32_t)z80_read8((uint16_t)(hl + 1u)) << 8) | z80_read8(hl)) + (de_2 | ((uint32_t)(uint8_t)(a - a - carry) << 8));
				hl = 0x137Cu + (uint8_t)((bc_2 & 0xFu) * 2u);
				de = stack_2;
				hl = (((uint32_t)z80_read8((uint16_t)(hl + 1u)) << 8) | z80_read8(hl)) + de;
				carry = (uint16_t)(hl + de) < hl;
				hl += de;
				hl = ((uint32_t)z80_read8((uint16_t)(hl + 1u)) << 8) | z80_read8(hl);
				hl = (hl & 0xFFu) | ((uint32_t)(uint8_t)(((((uint8_t)bc_2 >> 1) | (carry << 7)) & 0x38u) + (hl >> 8)) << 8);
				de = (SOUND_RAM8(0xE01Fu) & 3u) << 8;
				bc_3 = 0xA4u | (de >> 8);
				a = (uint8_t)(hl >> 8);
				sound_022E(bc_3, a);
				z80_block(36);
				bc_3 = 0xA0u | (de >> 8);
				a = (uint8_t)hl;
				sound_022E(bc_3, a);
				z80_block(17);
				sound_1329(&bc_2, &de, &hl);
				z80_block(7);
				bc_2 = (bc_2 & 0xFFu) | 0x400u;
				for (;;)
				{
					z80_block(63);
					stack_2 = bc_2;
					a = z80_read8(de) + bc_2;
					de++;
					bc_2 = a;
					a = z80_read8(hl);
					hl++;
					zero = a >> 7 == 0u;
					if (zero)
					{
						z80_block(21);
						bc_2 |= (uint32_t)a << 8;
						a = SOUND_RAM8(0xE013u) + (bc_2 >> 8);
					}
					z80_block(24);
					a &= 0x7Fu;
					sound_022E((uint8_t)bc_2, a);
					z80_block(18);
					bc_2 = (stack_2 & 0xFFu) | ((uint32_t)(uint8_t)((stack_2 >> 8) - 1u) << 8);
					if (bc_2 >> 8 == 0u)
						break;
					z80_cycles(5);
				}
				z80_block(45);
				bc_2 = (uint8_t)(0xB4u + bc_2);
				a = SOUND_RAM8(0xE01Bu);
				sound_022E((uint8_t)bc_2, a);
				z80_block(44);
				bc_3 = 0x28u;
				a = (SOUND_RAM8(0xE01Fu) & 7u) | 0xF0u;
				a = sound_0221(bc_3, a);
				return;
			}
			z80_block(84);
			hl++;
			SOUND_RAM8(0xE00Du) = z80_read8(hl);
			hl += 2u;
			SOUND_RAM8(0xE017u) = z80_read8(hl);
			SOUND_RAM8(0xE031u) = z80_read8((uint16_t)(hl + 1u));
		L_00052B:
			z80_block(31);
			hl = 0xE400u;
			de = 0x12u;
			bc_2 = (bc & 0xFFu) | 0x800u;
			a = 0u;
			for (;;)
			{
				z80_block(17);
				zero = (uint8_t)(a - z80_read8(hl)) == 0u;
				if (zero)
				{
					z80_block(25);
					stack_2 = hl;
					iy = stack_2;
					goto L_00059C;
				}
				z80_block(19);
				hl += de;
				bc_2 = (bc_2 & 0xFFu) | ((uint32_t)(uint8_t)((bc_2 >> 8) - 1u) << 8);
				if (bc_2 >> 8 == 0u)
					break;
				z80_cycles(5);
			}
			z80_block(21);
			hl = 0xE406u;
			bc_2 = (bc_2 & 0xFFu) | 0x800u;
			a--;
			for (;;)
			{
				z80_block(17);
				carry = a < z80_read8(hl);
				if (carry == 0u)
				{
					z80_block(11);
					a = z80_read8(hl);
					bc_2 = (bc_2 & ~0xFFu) | (uint8_t)hl;
				}
				z80_block(19);
				hl += de;
				bc_2 = (bc_2 & 0xFFu) | ((uint32_t)(uint8_t)((bc_2 >> 8) - 1u) << 8);
				if (bc_2 >> 8 == 0u)
					break;
				z80_cycles(5);
			}
			z80_block(333);
			iy = 0xFFFAu + ((hl & ~0xFFu) | (uint8_t)bc_2);
			SOUND_RAM8(0xE406u)--;
			SOUND_RAM8(0xE418u)--;
			SOUND_RAM8(0xE42Au)--;
			SOUND_RAM8(0xE43Cu)--;
			SOUND_RAM8(0xE44Eu)--;
			SOUND_RAM8(0xE460u)--;
			SOUND_RAM8(0xE472u)--;
			SOUND_RAM8(0xE484u)--;
			SOUND_RAM8(0xE020u) = 7u;
			a = SOUND_RAM8(0xE026u) ^ z80_read8((uint16_t)(0x6F1u + z80_read8((uint16_t)(iy + 1u))));
			SOUND_RAM8(0xE026u) = a;
			a = ~a;
			bc_3 = 8u;
			sound_036D(bc_3, a);
			z80_block(67);
			z80_write8((uint16_t)(iy + 9u), 0xD0u);
			z80_write8((uint16_t)(iy + 0x10u), 0xFFu);
			z80_write8((uint16_t)(iy + 0x11u), 7u);
		L_00059C:
			z80_block(36);
			carry = z80_read8((uint16_t)(ix + 3u)) < 0xF0u;
			if (carry)
			{
				z80_block(63);
				a = z80_read8((uint16_t)(iy + 1u));
				SOUND_RAM8(0xE025u) = a;
				a |= 0xC0u;
				bc_3 = 7u;
				sound_036D(bc_3, a);
				z80_block(35);
				a = ((a & 7u) * 2u) | 0x80u;
				sound_036D(bc_3, a);
				z80_block(47);
				a = ((uint32_t)a << 1) | (a >> 7);
				a = ((uint32_t)a << 1) | (a >> 7);
				a = ((uint32_t)a << 1) | (a >> 7);
				a = (((uint32_t)a << 1) | (a >> 7)) & 0xF0u;
				bc_3 = 5u;
				sound_036D(bc_3, a);
				z80_block(24);
				bc_3 = 6u;
				sound_036D(bc_3, a);
				z80_block(177);
				a = SOUND_RAM8(0xE012u);
				z80_write8((uint16_t)(iy + 2u), a);
				bc_2 = a;
				a = SOUND_RAM8(0xE008u) & 0xFu;
				z80_write8((uint16_t)(iy + 3u), a);
				bc_2 |= (uint32_t)a << 8;
				z80_write8((uint16_t)(iy + 5u), SOUND_RAM8(0xE013u));
				a = SOUND_RAM8(0xE020u);
				z80_write8((uint16_t)(iy + 6u), a);
				SOUND_RAM8(0xE020u) = a + 1u;
				sound_0B9C(iy);
				z80_block(282);
				z80_write8(iy, 0x40u);
				hl = 0xE040u + (uint8_t)((bc_2 >> 8) * 2u);
				a = SOUND_RAM8(0xE031u);
				de_2 = a;
				t5 = a;
				a = ((uint32_t)t5 << 1) | (t5 >> 7);
				carry = t5 >> 7;
				stack_2 = (((uint32_t)z80_read8((uint16_t)(hl + 1u)) << 8) | z80_read8(hl)) + (de_2 | ((uint32_t)(uint8_t)(a - a - carry) << 8));
				hl = 0x1E96u + (uint8_t)((bc_2 & 0xFu) * 2u);
				hl = (((uint32_t)z80_read8((uint16_t)(hl + 1u)) << 8) | z80_read8(hl)) + stack_2 + stack_2;
				hl = ((uint32_t)z80_read8((uint16_t)(hl + 1u)) << 8) | z80_read8(hl);
				t6 = bc_2 & 0xF0u;
				a = t6;
				if (t6 != 0u)
				{
					z80_block(33);
					a = (a >> 1) | ((uint32_t)a << 7);
					a = (a >> 1) | ((uint32_t)a << 7);
					a = (a >> 1) | ((uint32_t)a << 7);
					a = (a >> 1) | ((uint32_t)a << 7);
					carry = a < 8u;
					if (carry)
					{
						z80_block(4);
						bc_2 = (uint32_t)a << 8;
						for (;;)
						{
							z80_block(24);
							carry = (uint8_t)hl >> 7;
							hl = (hl & ~0xFFu) | (uint8_t)((uint32_t)hl << 1);
							hl = (hl & 0xFFu) | ((uint32_t)(uint8_t)(((uint32_t)(hl >> 8) << 1) | carry) << 8);
							bc_2 = (uint32_t)(uint8_t)((bc_2 >> 8) - 1u) << 8;
							if (bc_2 >> 8 == 0u)
								break;
							z80_cycles(5);
						}
					}
					else
					{
						z80_block(19);
						bc_2 = ((~a + 1u) & 0xFu) << 8;
						for (;;)
						{
							z80_block(24);
							carry = (hl >> 8) & 1u;
							hl = (hl & 0xFFu) | ((uint32_t)((hl >> 8) >> 1) << 8);
							hl = (hl & ~0xFFu) | (uint8_t)(((uint8_t)hl >> 1) | (carry << 7));
							bc_2 = (uint32_t)(uint8_t)((bc_2 >> 8) - 1u) << 8;
							if (bc_2 >> 8 == 0u)
								break;
							z80_cycles(5);
						}
						z80_block(10);
					}
				}
			}
			else
			{
				z80_block(63);
				a = z80_read8((uint16_t)(iy + 1u));
				SOUND_RAM8(0xE025u) = a;
				a |= 0xC0u;
				bc_3 = 7u;
				sound_036D(bc_3, a);
				z80_block(35);
				a = ((a & 7u) * 2u) | 0x80u;
				sound_036D(bc_3, a);
				z80_block(47);
				a = ((uint32_t)a << 1) | (a >> 7);
				a = ((uint32_t)a << 1) | (a >> 7);
				a = ((uint32_t)a << 1) | (a >> 7);
				a = (((uint32_t)a << 1) | (a >> 7)) & 0xF0u;
				bc_3 = 5u;
				sound_036D(bc_3, a);
				z80_block(24);
				bc_3 = 6u;
				sound_036D(bc_3, a);
				z80_block(140);
				z80_write8((uint16_t)(iy + 2u), SOUND_RAM8(0xE012u));
				z80_write8((uint16_t)(iy + 3u), SOUND_RAM8(0xE008u) & 0xFu);
				z80_write8((uint16_t)(iy + 5u), SOUND_RAM8(0xE013u));
				zero = (uint8_t)((SOUND_RAM8(0xE008u) & 0xFu) - 0xFu) == 0u;
				if (zero)
				{
					z80_block(49);
					a = SOUND_RAM8(0xE020u);
					z80_write8((uint16_t)(iy + 6u), a);
					SOUND_RAM8(0xE020u) = a + 1u;
				}
				else
				{
					z80_block(73);
					a = SOUND_RAM8(0xE020u) | 0xF0u;
					z80_write8((uint16_t)(iy + 6u), a);
					SOUND_RAM8(0xE020u) = (a & 0xFu) + 1u;
				}
				z80_block(17);
				sound_0B9C(iy);
				z80_block(63);
				z80_write8(iy, 0x50u);
				hl = SOUND_RAM8(0xE01Du) | ((uint32_t)SOUND_RAM8(0xE01Eu) << 8);
			}
			z80_block(28);
			bc_3 = 3u;
			a = (uint8_t)(hl >> 8);
			sound_036D(bc_3, a);
			z80_block(28);
			bc_3 = 2u;
			a = (uint8_t)hl;
			sound_036D(bc_3, a);
			z80_block(91);
			z80_write8((uint16_t)(iy + 0xEu), (uint8_t)hl);
			z80_write8((uint16_t)(iy + 0xFu), hl >> 8);
			hl = (hl & 0xFFu) | ((uint32_t)SOUND_RAM8(0xE013u) << 8);
			de_2 = z80_read8((uint16_t)(ix + 8u));
			hl = sound_0DFE(de_2, hl);
			z80_block(28);
			a = (uint8_t)(hl >> 8);
			bc_3 = 0u;
			sound_036D(bc_3, a);
			z80_block(37);
			a = SOUND_RAM8(0xE017u);
			bc_3 = 1u;
			sound_036D(bc_3, a);
			z80_block(99);
			a = SOUND_RAM8(0xE026u) | z80_read8((uint16_t)(0x6F1u + SOUND_RAM8(0xE025u)));
			SOUND_RAM8(0xE026u) = a;
			a = ~a;
			bc_3 = 8u;
			sound_036D(bc_3, a);
			return;
		}
		z80_block(34);
		a += 0x10u;
		hl = 0x3F2Eu;
		hl = sound_03FF(hl, a);
		z80_block(158);
		de = bc >> 8;
		hl = hl + de + de + de + de;
		SOUND_RAM8(0xE01Du) = z80_read8(hl);
		hl++;
		SOUND_RAM8(0xE01Eu) = z80_read8(hl);
		hl++;
		SOUND_RAM8(0xE017u) = z80_read8(hl);
		a = z80_read8((uint16_t)(hl + 1u));
		SOUND_RAM8(0xE00Du) = a;
		zero = (uint8_t)(a + 1u) == 0u;
		if (zero)
		{
			z80_cycles(6);
			return;
		}
		z80_block(10);
		goto L_00052B;
	}
	z80_block(17);
	zero = (uint8_t)(a + 0x80u) == 0u;
	if (zero)
	{
		z80_block(18);
		zero = (uint8_t)bc >> 7 == 0u;
		if (zero == 0u)
		{
			z80_block(17);
			sound_0B92(&bc, &ignored16, &ignored8);
		}
		z80_block(96);
		SOUND_RAM8(0xE012u) = z80_read8((uint16_t)(0x1A3Eu + (uint8_t)(z80_read8((uint16_t)(ix + 4u)) + bc)));
		SOUND_RAM8(0xE002u)++;
		goto L_000714;
	}
	z80_block(17);
	zero = (uint8_t)(a + 0x20u) == 0u;
	if (zero)
	{
		z80_block(18);
		zero = (uint8_t)bc >> 7 == 0u;
		if (zero == 0u)
		{
			z80_block(17);
			sound_0B92(&bc, &ignored16, &ignored8);
		}
		z80_block(21);
		bc = (uint32_t)(uint8_t)bc << 8;
		sound_0B92(&bc, &ignored16, &a);
		z80_block(26);
		carry = a & 1u;
		bc_2 = (bc & ~0xFFu) | (a >> 1);
		a = (uint8_t)(bc_2 >> 8);
		if (carry)
		{
			z80_block(7);
			a |= 0x80u;
		}
		z80_block(78);
		carry = bc_2 & 1u;
		bc_2 = (uint8_t)bc_2 >> 1;
		a = (a >> 1) | (carry << 7);
		carry = bc_2 & 1u;
		bc_2 = bc_2 >> 1;
		a = (a >> 1) | (carry << 7);
		carry = bc_2 & 1u;
		bc_2 = bc_2 >> 1;
		a = (a >> 1) | (carry << 7);
		carry = bc_2 & 1u;
		bc_2 = bc_2 >> 1;
		a = (a >> 1) | (carry << 7);
		carry = bc_2 & 1u;
		bc_2 = bc_2 >> 1;
		a = (a >> 1) | (carry << 7);
		zero = (bc_2 & 1u) == 0u;
		if (zero)
		{
			z80_block(18);
			t7 = ~a + 1u;
			a = t7;
			if (t7 == 0u)
			{
				z80_block(4);
				a--;
			}
		}
		z80_block(57);
		a = z80_read8((uint16_t)(0x232Eu + a));
		carry = bc_2 & 1u;
		bc_2 = bc_2 >> 1;
		if (carry == 0u)
		{
			z80_block(22);
			bc_2 = (uint8_t)(bc_2 - 1u);
			t8 = ~a + 1u;
			a = t8;
			if (t8 == 0u)
			{
				z80_block(4);
				a--;
			}
		}
		z80_block(145);
		bc_2 = ((uint32_t)bc_2 << 8) | a;
		SOUND_RAM16(0xE014u) = bc_2;
		hl = 0xE040u + (uint8_t)((SOUND_RAM8(0xE008u) & 0xFu) * 2u);
		z80_write8(hl, (uint8_t)bc_2);
		z80_write8((uint16_t)(hl + 1u), bc_2 >> 8);
		iy = 0xE400u;
		bc_2 = (bc_2 & 0xFFu) | 0x1400u;
		a = SOUND_RAM8(0xE008u) & 0xFu;
		for (;;)
		{
			z80_block(29);
			zero = (uint8_t)(a - z80_read8((uint16_t)(iy + 3u))) == 0u;
			if (zero == 0u)
			{
			L_0007DE:
				z80_block(33);
				iy += 0x12u;
				bc_2 = (bc_2 & 0xFFu) | ((uint32_t)(uint8_t)((bc_2 >> 8) - 1u) << 8);
				if (bc_2 >> 8 == 0u)
					break;
				z80_cycles(5);
				continue;
			}
			z80_block(47);
			stack_1 = a;
			if ((z80_read8(iy) & 0x7Fu) != 0u)
			{
				z80_cycles(7);
				bc_2 = sound_07E6(bc_2, ix, iy);
			}
			z80_block(10);
			a = stack_1;
			goto L_0007DE;
		}
		z80_block(10);
		return;
	}
	z80_block(17);
	zero = (uint8_t)(a + 0x50u) == 0u;
	if (zero)
	{
		z80_block(18);
		zero = (uint8_t)bc >> 7 == 0u;
		if (zero == 0u)
		{
			z80_block(17);
			sound_0B92(&bc, &ignored16, &ignored8);
		}
		z80_block(21);
		bc = (uint32_t)(uint8_t)bc << 8;
		sound_0B92(&bc, &hl, &ignored8);
		z80_block(21);
		a = (uint8_t)(bc >> 8);
		carry = a == 0u;
		zero = (uint8_t)(a - 1u) == 0u;
		if (zero)
		{
			z80_block(123);
			t9 = bc;
			a = (t9 >> 1) | (carry << 7);
			carry = t9 & 1u;
			t10 = a;
			a = (t10 >> 1) | (carry << 7);
			carry = t10 & 1u;
			t11 = a;
			a = (t11 >> 1) | (carry << 7);
			carry = t11 & 1u;
			a = ((a >> 1) | (carry << 7)) & 7u;
			bc_2 = a;
			SOUND_RAM8(0xE01Bu) = a;
			hl = 0xE060u + (SOUND_RAM8(0xE008u) & 0xFu);
			a = z80_read8(hl);
			bc_2 |= (uint32_t)a << 8;
			zero = (uint8_t)((a & 0x7Fu) - bc_2) == 0u;
			if (zero)
			{
				z80_cycles(6);
				return;
			}
			z80_block(59);
			z80_write8(hl, ((bc_2 >> 8) & 0x80u) | bc_2);
			hl = 0xE403u;
			bc_2 = 0x1400u;
			a = SOUND_RAM8(0xE008u) & 0xFu;
			for (;;)
			{
				z80_block(17);
				zero = (uint8_t)(a - z80_read8(hl)) == 0u;
				if (zero == 0u)
				{
				L_0008E9:
					z80_block(29);
					hl += 0x12u;
					bc_2 = (uint32_t)(uint8_t)((bc_2 >> 8) - 1u) << 8;
					if (bc_2 >> 8 == 0u)
						break;
					z80_cycles(5);
					continue;
				}
				z80_block(46);
				de = (uint32_t)a << 8;
				hl = hl - 1u - 1u - 1u;
				if ((z80_read8(hl) & 0x7Fu) != 0u)
				{
					z80_cycles(7);
					sound_08F0(&bc_2, &hl);
				}
				z80_block(22);
				hl += 3u;
				a = (uint8_t)(de >> 8);
				goto L_0008E9;
			}
			z80_block(10);
			return;
		}
		z80_block(17);
		zero = (uint8_t)(a - 7u) == 0u;
		if (zero)
		{
			z80_block(18);
			t12 = bc * 2u;
			a = t12;
			if (t12 != 0u)
			{
				z80_block(4);
				a++;
			}
			z80_block(60);
			z80_write8((uint16_t)(ix + 2u), a);
			hl = (hl & 0xFFu) | ((uint32_t)a << 8);
			a = 0xFFu;
			if ((z80_read8(ix) & 1u) == 0u)
			{
				z80_block(13);
				a = SOUND_RAM8(0xE01Cu);
			}
			z80_block(21);
			de_2 = a;
			hl = sound_0DFE(de_2, hl);
			z80_block(18);
			a = (uint8_t)(hl >> 8);
			zero = a == 0u;
			a = a;
			if (zero == 0u)
			{
				z80_block(4);
				a++;
			}
			z80_block(36);
			z80_write8((uint16_t)(ix + 8u), a);
			hl = 0xE400u;
			bc_2 = 0x1400u;
			for (;;)
			{
				z80_block(33);
				stack_2 = hl;
				if (((z80_read8(hl) >> 6) & 1u) == 0u)
				{
				L_000A2F:
					z80_block(39);
					hl = stack_2 + 0x12u;
					bc_2 = (uint32_t)(uint8_t)((bc_2 >> 8) - 1u) << 8;
					if (bc_2 >> 8 == 0u)
						break;
					z80_cycles(5);
					continue;
				}
				z80_block(55);
				hl += 3u;
				zero = (uint8_t)((SOUND_RAM8(0xE008u) & 0xFu) - z80_read8(hl)) == 0u;
				if (zero == 0u)
					goto L_000A2F;
				z80_block(37);
				hl = hl - 1u - 1u;
				bc_2 |= z80_read8(hl);
				zero = (uint8_t)bc_2 >> 7 == 0u;
				if (zero)
				{
					z80_block(35);
					a = bc_2 | 0xC0u;
					bc_2 = (bc_2 & ~0xFFu) | 7u;
					sound_036D((uint8_t)bc_2, a);
					z80_block(64);
					hl = ((hl + 4u) & 0xFFu) | ((uint32_t)z80_read8((uint16_t)(hl + 4u)) << 8);
					de_2 = z80_read8((uint16_t)(ix + 8u));
					hl = sound_0DFE(de_2, hl);
					z80_block(28);
					a = (uint8_t)(hl >> 8);
					bc_2 &= ~0xFFu;
					sound_036D((uint8_t)bc_2, a);
					goto L_000A2F;
				}
				z80_block(17);
				sound_12D1((uint8_t)bc_2, hl, ix);
				z80_block(10);
				goto L_000A2F;
			}
			z80_block(10);
			return;
		}
		z80_block(17);
		zero = (uint8_t)(a - 0x10u) == 0u;
		if (zero)
		{
			z80_block(18);
			t13 = bc * 2u;
			a = t13;
			if (t13 != 0u)
			{
				z80_block(4);
				a++;
			}
			z80_block(34);
			SOUND_RAM8(0xE01Cu) = a;
			iy = 0xE300u;
			bc_2 = (bc & 0xFFu) | 0x800u;
			for (;;)
			{
				z80_block(67);
				stack_2 = bc_2;
				hl = (hl & 0xFFu) | ((uint32_t)z80_read8((uint16_t)(iy + 2u)) << 8);
				a = 0xFFu;
				if ((z80_read8(iy) & 1u) != 0u)
				{
				L_000975:
					z80_block(21);
					de_2 = a;
					hl = sound_0DFE(de_2, hl);
					z80_block(18);
					a = (uint8_t)(hl >> 8);
					zero = a == 0u;
					a = a;
					if (zero == 0u)
					{
						z80_block(4);
						a++;
					}
					z80_block(36);
					z80_write8((uint16_t)(iy + 8u), a);
					hl = 0xE403u;
					bc_2 = 0x1400u;
					for (;;)
					{
						z80_block(51);
						stack_4 = hl;
						bc_2 |= z80_read8(hl);
						zero = (uint8_t)(z80_read8((uint16_t)(iy + 1u)) - bc_2) == 0u;
						if (zero)
						{
							z80_block(40);
							hl = hl - 1u - 1u - 1u;
							if ((z80_read8(hl) >> 6) & 1u)
							{
								z80_block(31);
								hl++;
								bc_2 = (bc_2 & ~0xFFu) | z80_read8(hl);
								zero = (uint8_t)bc_2 >> 7 == 0u;
								if (zero)
								{
									z80_block(35);
									a = bc_2 | 0xC0u;
									bc_2 = (bc_2 & ~0xFFu) | 7u;
									sound_036D((uint8_t)bc_2, a);
									z80_block(64);
									hl = ((hl + 4u) & 0xFFu) | ((uint32_t)z80_read8((uint16_t)(hl + 4u)) << 8);
									de_2 = z80_read8((uint16_t)(iy + 8u));
									hl = sound_0DFE(de_2, hl);
									z80_block(28);
									a = (uint8_t)(hl >> 8);
									bc_2 &= ~0xFFu;
									sound_036D((uint8_t)bc_2, a);
								}
								else
								{
									z80_block(17);
									sound_1279((uint8_t)bc_2, hl, iy);
									z80_block(10);
								}
							}
						}
						z80_block(39);
						hl = stack_4 + 0x12u;
						bc_2 = (uint32_t)(uint8_t)((bc_2 >> 8) - 1u) << 8;
						if (bc_2 >> 8 == 0u)
							break;
						z80_cycles(5);
					}
					z80_block(43);
					iy += 9u;
					bc_2 = (stack_2 & 0xFFu) | ((uint32_t)(uint8_t)((stack_2 >> 8) - 1u) << 8);
					if (bc_2 >> 8 == 0u)
						break;
					z80_cycles(5);
					continue;
				}
				z80_block(13);
				a = SOUND_RAM8(0xE01Cu);
				goto L_000975;
			}
			z80_block(10);
			return;
		}
		z80_block(17);
		zero = (uint8_t)(a - 0x40u) == 0u;
		if (zero)
		{
			z80_block(21);
			carry = (uint8_t)bc < 0x40u;
			if (carry == 0u)
			{
				z80_block(77);
				hl = 0xE060u + (SOUND_RAM8(0xE008u) & 0xFu);
				z80_write8(hl, z80_read8(hl) | 0x80u);
				return;
			}
			z80_block(51);
			iy = 0xE400u;
			de = 0x12u;
			bc_2 = 0x1400u;
			a = SOUND_RAM8(0xE008u) & 0xFu;
			for (;;)
			{
				z80_block(29);
				zero = (uint8_t)(a - z80_read8((uint16_t)(iy + 3u))) == 0u;
				if (zero == 0u)
				{
				L_000943:
					z80_block(23);
					iy += de;
					bc_2 = (uint32_t)(uint8_t)((bc_2 >> 8) - 1u) << 8;
					if (bc_2 >> 8 == 0u)
						break;
					z80_cycles(5);
					continue;
				}
				z80_block(41);
				hl = (hl & ~0xFFu) | a;
				zero = ((z80_read8(iy) >> 1) & 1u) == 0u;
				if (zero == 0u)
				{
					z80_block(37);
					bc_2 |= z80_read8((uint16_t)(iy + 1u));
					zero = (uint8_t)bc_2 >> 7 == 0u;
					if (zero)
					{
						z80_block(39);
						stack_2 = hl;
						stack_4 = bc_2;
						sound_0D06(iy);
						z80_block(30);
						bc_2 = stack_4;
						hl = stack_2;
						de = 0x12u;
					}
					else
					{
						z80_block(17);
						sound_125B(iy);
						z80_block(10);
					}
				}
				z80_block(4);
				a = (uint8_t)hl;
				goto L_000943;
			}
			z80_block(77);
			hl = 0xE060u + (SOUND_RAM8(0xE008u) & 0xFu);
			z80_write8(hl, z80_read8(hl) & 0x7Fu);
			return;
		}
		z80_block(17);
		zero = (uint8_t)(a - 0x7Bu) == 0u;
		if (zero)
		{
			z80_block(55);
			iy = 0xE400u;
			bc = (SOUND_RAM8(0xE008u) & 0xFu) | 0x800u;
			for (;;)
			{
				z80_block(30);
				if ((z80_read8(iy) >> 6) & 1u)
				{
					z80_block(33);
					zero = (uint8_t)(bc - z80_read8((uint16_t)(iy + 3u))) == 0u;
					if (zero)
					{
						z80_block(28);
						stack_2 = bc;
						sound_0D06(iy);
						z80_block(20);
						bc = stack_2;
					}
				}
				z80_block(19);
				bc = (bc & 0xFFu) | ((uint32_t)(uint8_t)((bc >> 8) - 1u) << 8);
				if (bc >> 8 == 0u)
					break;
				z80_cycles(5);
			}
		L_000B4E:
			z80_block(47);
			a = SOUND_RAM8(0xE008u) & 0xFu;
			hl = 0xE490u;
			de = 0x12u;
			bc_2 = (bc & 0xFFu) | 0xC00u;
			for (;;)
			{
				z80_block(22);
				if ((z80_read8(hl) >> 6) & 1u)
				{
					z80_block(35);
					hl += 3u;
					zero = (uint8_t)(a - z80_read8(hl)) == 0u;
					if (zero == 0u)
					{
						z80_block(18);
						hl = hl - 1u - 1u - 1u;
					}
					else
					{
						z80_block(46);
						hl = hl - 1u - 1u - 1u;
						stack_2 = bc_2;
						sound_1192(&de, &hl, &a);
						z80_block(35);
						bc_2 = stack_2;
						z80_write8(hl, z80_read8(hl) & 0xBFu);
					}
				}
				z80_block(19);
				hl += de;
				bc_2 = (bc_2 & 0xFFu) | ((uint32_t)(uint8_t)((bc_2 >> 8) - 1u) << 8);
				if (bc_2 >> 8 == 0u)
					break;
				z80_cycles(5);
			}
			z80_block(118);
			z80_write8((uint16_t)(0xE060u + (uint8_t)bc_2), 0u);
			hl = 0xE040u + (uint8_t)(bc_2 * 2u);
			z80_write8(hl, 0u);
			z80_write8((uint16_t)(hl + 1u), 0u);
			return;
		}
		z80_block(17);
		zero = (uint8_t)(a - 0x50u) == 0u;
		if (zero)
		{
			z80_block(21);
			zero = (uint8_t)(bc - 0x7Eu) == 0u;
			if (zero)
			{
				z80_block(31);
				iy = 0xE400u;
				bc_2 = (bc & 0xFFu) | 0x800u;
				for (;;)
				{
					z80_block(30);
					if (((z80_read8(iy) >> 6) & 1u) == 0u)
					{
					L_000AB1:
						z80_block(19);
						bc_2 = (bc_2 & 0xFFu) | ((uint32_t)(uint8_t)((bc_2 >> 8) - 1u) << 8);
						if (bc_2 >> 8 != 0u)
						{
							z80_cycles(5);
							continue;
						}
						break;
					}
					z80_block(30);
					if (((z80_read8(iy) >> 4) & 1u) == 0u)
					{
					L_000AA9:
						z80_block(28);
						stack_2 = bc_2;
						sound_0D06(iy);
						z80_block(20);
						bc_2 = stack_2;
						goto L_000AB1;
					}
					z80_block(36);
					zero = (uint8_t)(z80_read8((uint16_t)(iy + 3u)) - 0xFu) == 0u;
					if (zero == 0u)
						goto L_000AB1;
					goto L_000AA9;
				}
				z80_block(10);
			L_000A59:
				z80_block(31);
				iy = 0xE490u;
				de = 0x12u;
				bc_2 = (bc_2 & 0xFFu) | 0xC00u;
				for (;;)
				{
					z80_block(30);
					if (((z80_read8(iy) >> 6) & 1u) == 0u)
					{
					L_000A71:
						z80_block(23);
						iy += de;
						bc_2 = (bc_2 & 0xFFu) | ((uint32_t)(uint8_t)((bc_2 >> 8) - 1u) << 8);
						if (bc_2 >> 8 != 0u)
						{
							z80_cycles(5);
							continue;
						}
						break;
					}
					z80_block(28);
					stack_2 = bc_2;
					sound_11D7(iy);
					z80_block(20);
					bc_2 = stack_2;
					de = 0x12u;
					goto L_000A71;
				}
				z80_block(17);
				hl = 0xE060u;
				bc_2 = 0x1000u;
				for (;;)
				{
					z80_block(24);
					z80_write8(hl, 0u);
					hl++;
					bc_2 = (uint32_t)(uint8_t)((bc_2 >> 8) - 1u) << 8;
					if (bc_2 >> 8 == 0u)
						break;
					z80_cycles(5);
				}
				z80_block(17);
				hl = 0xE040u;
				bc_2 = 0x1000u;
				for (;;)
				{
					z80_block(24);
					z80_write8(hl, 0u);
					hl++;
					bc_2 = (uint32_t)(uint8_t)((bc_2 >> 8) - 1u) << 8;
					if (bc_2 >> 8 == 0u)
						break;
					z80_cycles(5);
				}
				z80_block(10);
				return;
			}
			z80_block(31);
			iy = 0xE400u;
			de = 0x12u;
			bc_2 = (bc & 0xFFu) | 0x800u;
			for (;;)
			{
				z80_block(30);
				if (((z80_read8(iy) >> 6) & 1u) == 0u)
				{
				L_000A55:
					z80_block(23);
					iy += de;
					bc_2 = (bc_2 & 0xFFu) | ((uint32_t)(uint8_t)((bc_2 >> 8) - 1u) << 8);
					if (bc_2 >> 8 != 0u)
					{
						z80_cycles(5);
						continue;
					}
					break;
				}
				z80_block(28);
				stack_2 = bc_2;
				sound_0D06(iy);
				z80_block(20);
				bc_2 = stack_2;
				de = 0x12u;
				goto L_000A55;
			}
			goto L_000A59;
		}
		z80_block(17);
		zero = (uint8_t)(a - 0x20u) == 0u;
		if (zero)
		{
			z80_block(31);
			a = (uint8_t)bc;
			hl = 0x1594u;
			hl = sound_03FF(hl, a);
			z80_block(57);
			move_block(hl, 0xE300u, 0x48u, 1u);
			hl += 0x48u;
			iy = 0xE300u;
			bc_2 = 0x800u;
			for (;;)
			{
				z80_block(56);
				hl = (hl & 0xFFu) | ((uint32_t)z80_read8((uint16_t)(iy + 2u)) << 8);
				a = 0xFFu;
				if ((z80_read8(iy) & 1u) != 0u)
				{
				L_000ADB:
					z80_block(21);
					de_2 = a;
					hl = sound_0DFE(de_2, hl);
					z80_block(18);
					a = (uint8_t)(hl >> 8);
					zero = a == 0u;
					a = a;
					if (zero == 0u)
					{
						z80_block(4);
						a++;
					}
					z80_block(52);
					z80_write8((uint16_t)(iy + 8u), a);
					iy += 9u;
					bc_2 = (uint32_t)(uint8_t)((bc_2 >> 8) - 1u) << 8;
					if (bc_2 >> 8 == 0u)
						break;
					z80_cycles(5);
					continue;
				}
				z80_block(13);
				a = SOUND_RAM8(0xE01Cu);
				goto L_000ADB;
			}
			z80_block(10);
			return;
		}
		z80_block(17);
		zero = (uint8_t)(a - 0x51u) == 0u;
		if (zero)
		{
			z80_block(60);
			hl = 0xE400u;
			bc_2 = z80_read8((uint16_t)(0x1A3Eu + (uint8_t)bc)) | 0x800u;
			for (;;)
			{
				z80_block(22);
				if (((z80_read8(hl) >> 6) & 1u) == 0u)
				{
				L_000B1F:
					z80_block(29);
					hl += 0x12u;
					bc_2 = (bc_2 & 0xFFu) | ((uint32_t)(uint8_t)((bc_2 >> 8) - 1u) << 8);
					if (bc_2 >> 8 == 0u)
						break;
					z80_cycles(5);
					continue;
				}
				z80_block(33);
				hl += 2u;
				zero = (uint8_t)(bc_2 - z80_read8(hl)) == 0u;
				if (zero == 0u)
				{
				L_000B1D:
					z80_block(12);
					hl = hl - 1u - 1u;
					goto L_000B1F;
				}
				z80_block(43);
				hl++;
				zero = (uint8_t)((SOUND_RAM8(0xE008u) & 0xFu) - z80_read8(hl)) == 0u;
				if (zero == 0u)
				{
					z80_block(6);
					hl--;
					goto L_000B1D;
				}
				z80_block(43);
				hl = hl - 1u - 1u - 1u;
				z80_write8(hl, z80_read8(hl) | 1u);
				goto L_000B1F;
			}
			z80_block(10);
			return;
		}
		z80_block(17);
		zero = (uint8_t)(a - 0x11u) == 0u;
		if (zero)
		{
			z80_block(18);
			zero = (uint8_t)bc == 0u;
			if (zero)
			{
				z80_block(87);
				a = SOUND_RAM8(0xE008u) & 0xFu;
				z80_write8((uint16_t)(0xE070u + a), 0u);
				bc_2 = 0xC00u;
				iy = 0xE490u;
				for (;;)
				{
					z80_block(29);
					zero = (uint8_t)(a - z80_read8((uint16_t)(iy + 3u))) == 0u;
					if (zero)
						break;
					z80_block(33);
					iy += 0x12u;
					bc_2 = (uint32_t)(uint8_t)((bc_2 >> 8) - 1u) << 8;
					if (bc_2 >> 8 == 0u)
					{
						z80_block(10);
						return;
					}
					z80_cycles(5);
				}
				z80_block(36);
				iy += 4u;
				bc_2 = 0x600u;
				for (;;)
				{
					z80_block(37);
					z80_write8(iy, (uint8_t)bc_2);
					iy++;
					bc_2 = (uint32_t)(uint8_t)((bc_2 >> 8) - 1u) << 8;
					if (bc_2 >> 8 == 0u)
						break;
					z80_cycles(5);
				}
				z80_block(39);
				iy += 8u;
			L_004554:
				z80_block(49);
				zero = (uint8_t)((SOUND_RAM8(0xE008u) & 0xFu) - z80_read8((uint16_t)(iy + 3u))) == 0u;
				if (zero)
				{
					z80_block(31);
					hl = 0x45E2u;
					a = (uint8_t)bc_2;
					hl = sound_03FF(hl, a);
					z80_block(344);
					de = ((uint32_t)(hl >> 8) << 8) | (uint8_t)hl;
					z80_write8((uint16_t)(iy + 7u), z80_read8(de));
					de++;
					z80_write8((uint16_t)(iy + 8u), z80_read8(de));
					de++;
					z80_write8((uint16_t)(iy + 9u), z80_read8(de));
					de++;
					z80_write8((uint16_t)(iy + 0xAu), z80_read8(de));
					de++;
					z80_write8((uint16_t)(iy + 0xBu), z80_read8(de));
					de++;
					z80_write8((uint16_t)(iy + 0xCu), z80_read8(de));
					z80_write8((uint16_t)(iy + 0xDu), z80_read8((uint16_t)(de + 1u)));
					z80_write8((uint16_t)(iy + 0xEu), z80_read8((uint16_t)(iy + 8u)));
					z80_write8((uint16_t)(iy + 0xDu), (z80_read8((uint16_t)(iy + 0xDu)) & 1u) + z80_read8((uint16_t)(iy + 9u)));
				L_00455F:
					z80_block(33);
					iy += 0x12u;
					bc_2 = (bc_2 & 0xFFu) | ((uint32_t)(uint8_t)((bc_2 >> 8) - 1u) << 8);
					if (bc_2 >> 8 == 0u)
					{
						z80_block(10);
						return;
					}
					z80_cycles(5);
				L_00454C:
					z80_block(36);
					if ((z80_read8(iy) & 0x40u) == 0u)
						goto L_00455F;
					goto L_004554;
				}
				goto L_00455F;
			}
			z80_block(80);
			z80_write8((uint16_t)(0xE070u + (SOUND_RAM8(0xE008u) & 0xFu)), (uint8_t)bc);
			bc_2 = (bc & 0xFFu) | 0xC00u;
			iy = 0xE490u;
			goto L_00454C;
		}
		z80_block(17);
		zero = (uint8_t)(a + 0x80u) == 0u;
		if (zero == 0u)
		{
			z80_block(10);
			return;
		}
		goto L_000B4E;
	}
	z80_block(17);
	zero = (uint8_t)(a + 0x40u) == 0u;
	if (zero)
	{
		z80_block(18);
		zero = (uint8_t)bc >> 7 == 0u;
		if (zero == 0u)
		{
			z80_block(17);
			sound_0B92(&bc, &ignored16, &ignored8);
		}
		z80_block(29);
		z80_write8((uint16_t)(ix + 3u), (uint8_t)bc);
		return;
	}
	z80_block(17);
	zero = (uint8_t)(a + 0x30u) == 0u;
	if (zero)
	{
		sound_0450((uint8_t)bc);
		return;
	}
	z80_block(17);
	zero = (uint8_t)(a + 0x60u) == 0u;
	if (zero)
	{
		sound_0450((uint8_t)bc);
		return;
	}
	z80_block(10);
}

uint16_t sound_07E6(uint16_t bc, uint16_t ix, uint16_t iy)
{
	uint32_t zero = 0, carry = 0;
	uint8_t a = 0, bc_3 = 0, de_2 = 0;
	uint16_t bc_2 = 0, de = 0, hl = 0;
	uint16_t stack_2 = 0;
	uint8_t t1 = 0, t2 = 0;

	z80_block(37);
	a = z80_read8((uint16_t)(iy + 1u));
	zero = a >> 7 == 0u;
	if (zero == 0u)
	{
		z80_block(313);
		stack_2 = bc;
		SOUND_RAM8(0xE01Fu) = a;
		a = z80_read8((uint16_t)(iy + 2u));
		bc_3 = a;
		hl = 0x137Cu + (uint8_t)((a & 0xFu) * 2u);
		de = SOUND_RAM16(0xE014u);
		hl = (((uint32_t)z80_read8((uint16_t)(hl + 1u)) << 8) | z80_read8(hl)) + de + de;
		a = z80_read8((uint16_t)(ix + 5u));
		de_2 = a;
		t1 = a;
		a = ((uint32_t)t1 << 1) | (t1 >> 7);
		carry = t1 >> 7;
		hl = (((uint32_t)z80_read8((uint16_t)(hl + 1u)) << 8) | z80_read8(hl)) + (de_2 | ((uint32_t)(uint8_t)(a - a - carry) << 8));
		hl = (hl & 0xFFu) | ((uint32_t)(uint8_t)(((bc_3 & ~0xFu) >> 1) + (hl >> 8)) << 8);
		de = (SOUND_RAM8(0xE01Fu) & 3u) << 8;
		bc_3 = 0xA4u | (de >> 8);
		a = (uint8_t)(hl >> 8);
		sound_022E(bc_3, a);
		z80_block(36);
		bc_3 = 0xA0u | (de >> 8);
		a = (uint8_t)hl;
		sound_022E(bc_3, a);
		z80_block(20);
		bc_2 = stack_2;
		return bc_2;
	}
	z80_block(31);
	a |= 0xC0u;
	bc_2 = (bc & ~0xFFu) | 7u;
	sound_036D((uint8_t)bc_2, a);
	z80_block(177);
	a = z80_read8((uint16_t)(iy + 2u));
	bc_2 = (bc_2 & ~0xFFu) | a;
	hl = 0x1E96u + (uint8_t)((a & 0xFu) * 2u);
	de = SOUND_RAM16(0xE014u);
	hl = (((uint32_t)z80_read8((uint16_t)(hl + 1u)) << 8) | z80_read8(hl)) + de + de;
	hl = ((uint32_t)z80_read8((uint16_t)(hl + 1u)) << 8) | z80_read8(hl);
	t2 = bc_2 & 0xF0u;
	a = t2;
	if (t2 != 0u)
	{
		z80_block(33);
		a = (a >> 1) | ((uint32_t)a << 7);
		a = (a >> 1) | ((uint32_t)a << 7);
		a = (a >> 1) | ((uint32_t)a << 7);
		a = (a >> 1) | ((uint32_t)a << 7);
		carry = a < 8u;
		if (carry)
		{
			z80_block(4);
			bc_2 = (uint32_t)a << 8;
			for (;;)
			{
				z80_block(24);
				carry = (uint8_t)hl >> 7;
				hl = (hl & ~0xFFu) | (uint8_t)((uint32_t)hl << 1);
				hl = (hl & 0xFFu) | ((uint32_t)(uint8_t)(((uint32_t)(hl >> 8) << 1) | carry) << 8);
				bc_2 = (uint32_t)(uint8_t)((bc_2 >> 8) - 1u) << 8;
				if (bc_2 >> 8 == 0u)
					break;
				z80_cycles(5);
			}
		}
		else
		{
			z80_block(19);
			bc_2 = ((~a + 1u) & 0xFu) << 8;
			for (;;)
			{
				z80_block(24);
				carry = (hl >> 8) & 1u;
				hl = (hl & 0xFFu) | ((uint32_t)((hl >> 8) >> 1) << 8);
				hl = (hl & ~0xFFu) | (uint8_t)(((uint8_t)hl >> 1) | (carry << 7));
				bc_2 = (uint32_t)(uint8_t)((bc_2 >> 8) - 1u) << 8;
				if (bc_2 >> 8 == 0u)
					break;
				z80_cycles(5);
			}
			z80_block(10);
		}
	}
	z80_block(28);
	bc_2 = (bc_2 & ~0xFFu) | 3u;
	a = (uint8_t)(hl >> 8);
	sound_036D((uint8_t)bc_2, a);
	z80_block(28);
	bc_2 = (bc_2 & ~0xFFu) | 2u;
	a = (uint8_t)hl;
	sound_036D((uint8_t)bc_2, a);
	z80_block(196);
	de = z80_read8((uint16_t)(iy + 0xEu)) | ((uint32_t)z80_read8((uint16_t)(iy + 0xFu)) << 8);
	z80_write8((uint16_t)(iy + 0xEu), (uint8_t)hl);
	z80_write8((uint16_t)(iy + 0xFu), hl >> 8);
	carry = 0u;
	hl = hl - de - carry + (z80_read8((uint16_t)(iy + 0x10u)) | ((uint32_t)z80_read8((uint16_t)(iy + 0x11u)) << 8));
	z80_write8((uint16_t)(iy + 0x10u), (uint8_t)hl);
	z80_write8((uint16_t)(iy + 0x11u), hl >> 8);
	return bc_2;
}

void sound_08F0(uint16_t *p_bc, uint16_t *p_hl)
{
	uint16_t bc = (uint16_t)*p_bc;
	uint16_t hl = (uint16_t)*p_hl;
	uint32_t zero = 0;
	uint8_t a = 0, bc_3 = 0;
	uint16_t bc_2 = 0;
	uint16_t stack_2 = 0;

	z80_block(26);
	hl++;
	bc_2 = (bc & ~0xFFu) | z80_read8(hl);
	zero = (uint8_t)bc_2 >> 7 == 0u;
	if (zero)
	{
		z80_cycles(6);
		*p_bc = bc_2;
		*p_hl = hl;
		return;
	}
	z80_block(10);
	z80_block(90);
	stack_2 = bc_2;
	a = (uint8_t)bc_2;
	SOUND_RAM8(0xE01Fu) = a;
	bc_3 = (uint8_t)((a & 3u) - 0x4Cu);
	a = (SOUND_RAM8(0xE01Bu) & 7u) | 0xC0u;
	sound_022E(bc_3, a);
	z80_block(26);
	hl--;
	bc_2 = stack_2;
	*p_bc = bc_2;
	*p_hl = hl;
}

void sound_0B92(uint16_t *p_bc, uint16_t *p_hl, uint8_t *p_a)
{
	uint8_t a = 0;
	uint16_t bc = 0, hl = 0;

	z80_block(56);
	SOUND_RAM8(0xE002u)++;
	hl = SOUND_RAM8(0xE002u) | 0xE100u;
	a = z80_read8(hl);
	bc = a;
	*p_bc = (*p_bc & 0xFF00u) | bc;
	*p_hl = hl;
	*p_a = a;
}

void sound_0B9C(uint16_t iy)
{
	uint32_t zero = 0;
	uint8_t a = 0, bc = 0;
	uint16_t de = 0, hl = 0;
	uint16_t stack_4 = 0;

	z80_block(52);
	a = ~SOUND_RAM8(0xE026u);
	bc = 8u;
	sound_036D(bc, a);
	z80_block(40);
	a = SOUND_RAM8(0xE00Du);
	hl = 0x6429u;
	hl = sound_03FF(hl, a);
	z80_block(293);
	a = z80_read8(hl);
	z80_write8((uint16_t)(iy + 7u), a);
	a &= 7u;
	SOUND_RAM8(0xE02Du) = a;
	port_out(0xB0u, a);
	hl++;
	a = z80_read8(hl);
	z80_write8((uint16_t)(iy + 8u), a);
	SOUND_RAM8(0xE02Cu) = a;
	port_out(0xA0u, a);
	de = (uint32_t)z80_read8((uint16_t)(iy + 9u)) << 8;
	hl++;
	stack_4 = (uint32_t)z80_read8(hl) << 8;
	hl++;
	z80_write8((uint16_t)(iy + 0xBu), z80_read8(hl));
	hl++;
	z80_write8((uint16_t)(iy + 0xCu), z80_read8(hl));
	z80_write8((uint16_t)(iy + 0xDu), z80_read8((uint16_t)(hl + 1u)));
	hl = stack_4;
	sound_0E29(&de, &hl);
	z80_block(40);
	z80_write8((uint16_t)(iy + 9u), de >> 8);
	zero = (uint8_t)(0xC0u - (hl >> 8)) == 0u;
	if (zero)
	{
		z80_block(30);
		hl = 0xA000u;
		z80_write8((uint16_t)(iy + 8u), z80_read8((uint16_t)(iy + 8u)) + 1u);
	}
	z80_block(39);
	z80_write8((uint16_t)(iy + 0xAu), hl >> 8);
}

void sound_0BF4(void)
{
	uint32_t zero = 0;
	uint8_t a = 0;
	uint16_t bc = 0, de = 0, hl = 0, iy = 0;
	uint8_t t1 = 0;

	z80_block(50);
	iy = SOUND_RAM16(0xE027u);
	if (((z80_read8(iy) >> 6) & 1u) == 0u)
	{
		z80_block(53);
		iy += 0x12u;
		hl = 0xE029u;
		t1 = SOUND_RAM8(0xE029u);
		SOUND_RAM8(0xE029u) = t1 - 1u;
		if (t1 != 1u)
		{
			z80_block(30);
			SOUND_RAM16(0xE027u) = iy;
			return;
		}
		z80_cycles(5);
		z80_block(46);
		z80_write8(hl, 8u);
		SOUND_RAM16(0xE027u) = 0xE400u;
		return;
	}
	z80_block(105);
	hl = (z80_read8((uint16_t)(iy + 0x10u)) | ((uint32_t)z80_read8((uint16_t)(iy + 0x11u)) << 8)) + (z80_read8((uint16_t)(iy + 0xEu)) | ((uint32_t)z80_read8((uint16_t)(iy + 0xFu)) << 8));
	zero = (((hl >> 8) >> 4) & 1u) == 0u;
	if (zero)
	{
		z80_block(25);
		bc = 0u;
		zero = (((hl >> 8) >> 3) & 1u) == 0u;
		if (zero == 0u)
		{
		L_000C28:
			z80_block(252);
			hl = (hl & 0xFFu) | (((hl >> 8) & ~8u) << 8);
			z80_write8((uint16_t)(iy + 0x10u), (uint8_t)hl);
			z80_write8((uint16_t)(iy + 0x11u), hl >> 8);
			z80_write8(0xC007u, (z80_read8((uint16_t)(iy + 1u)) * 2u) | 0x80u);
			a = z80_read8((uint16_t)(iy + 7u)) & 7u;
			SOUND_RAM8(0xE02Du) = a;
			port_out(0xB0u, a);
			a = z80_read8((uint16_t)(iy + 8u));
			SOUND_RAM8(0xE02Cu) = a;
			port_out(0xA0u, a);
			de = (uint32_t)z80_read8((uint16_t)(iy + 9u)) << 8;
			hl = (uint32_t)z80_read8((uint16_t)(iy + 0xAu)) << 8;
			zero = bc >> 8 == 0u;
			if (zero)
			{
			L_000CA5:
				z80_block(17);
				sound_0E29(&de, &hl);
				z80_block(27);
				hl--;
				zero = (uint8_t)(z80_read8(hl) + 1u) == 0u;
				if (zero)
				{
					z80_cycles(5);
					sound_0D06(iy);
					return;
				}
				z80_block(27);
				hl++;
				zero = (uint8_t)(0xC0u - (hl >> 8)) == 0u;
				if (zero)
				{
					z80_block(73);
					hl = (hl & 0xFFu) | 0xA000u;
					z80_write8((uint16_t)(iy + 8u), z80_read8((uint16_t)(iy + 8u)) + 1u);
					a = z80_read8((uint16_t)(iy + 8u));
					SOUND_RAM8(0xE02Cu) = a;
					port_out(0xA0u, a);
				}
				z80_block(21);
				zero = (uint8_t)(0xE0u - (de >> 8)) == 0u;
				if (zero)
				{
					z80_block(7);
					de = (de & 0xFFu) | 0xD000u;
				}
				z80_block(17);
				sound_0E29(&de, &hl);
				z80_block(27);
				hl--;
				zero = (uint8_t)(z80_read8(hl) + 1u) == 0u;
				if (zero)
				{
					z80_cycles(5);
					sound_0D06(iy);
					return;
				}
				z80_block(27);
				hl++;
				zero = (uint8_t)(0xC0u - (hl >> 8)) == 0u;
				if (zero)
				{
					z80_block(30);
					hl = 0xA000u;
					z80_write8((uint16_t)(iy + 8u), z80_read8((uint16_t)(iy + 8u)) + 1u);
				}
				z80_block(21);
				zero = (uint8_t)(0xE0u - (de >> 8)) == 0u;
				if (zero)
				{
					z80_block(7);
					de = 0xD000u;
				}
				z80_block(38);
				z80_write8((uint16_t)(iy + 9u), de >> 8);
				z80_write8((uint16_t)(iy + 0xAu), hl >> 8);
				z80_block(53);
				iy += 0x12u;
				hl = 0xE029u;
				t1 = SOUND_RAM8(0xE029u);
				SOUND_RAM8(0xE029u) = t1 - 1u;
				if (t1 != 1u)
				{
					z80_block(30);
					SOUND_RAM16(0xE027u) = iy;
					return;
				}
				z80_cycles(5);
				z80_block(46);
				z80_write8(hl, 8u);
				SOUND_RAM16(0xE027u) = 0xE400u;
				return;
			}
			z80_block(17);
			sound_0E29(&de, &hl);
			z80_block(30);
			hl--;
			zero = (uint8_t)(z80_read8(hl) + 1u) == 0u;
			if (zero)
			{
				sound_0D06(iy);
				return;
			}
			z80_block(27);
			hl++;
			zero = (uint8_t)(0xC0u - (hl >> 8)) == 0u;
			if (zero)
			{
				z80_block(73);
				hl = (hl & 0xFFu) | 0xA000u;
				z80_write8((uint16_t)(iy + 8u), z80_read8((uint16_t)(iy + 8u)) + 1u);
				a = z80_read8((uint16_t)(iy + 8u));
				SOUND_RAM8(0xE02Cu) = a;
				port_out(0xA0u, a);
			}
			z80_block(21);
			zero = (uint8_t)(0xE0u - (de >> 8)) == 0u;
			if (zero)
			{
				z80_block(7);
				de = (de & 0xFFu) | 0xD000u;
			}
			z80_block(17);
			sound_0E29(&de, &hl);
			z80_block(27);
			hl--;
			zero = (uint8_t)(z80_read8(hl) + 1u) == 0u;
			if (zero)
			{
				z80_cycles(5);
				sound_0D06(iy);
				return;
			}
			z80_block(27);
			hl++;
			zero = (uint8_t)(0xC0u - (hl >> 8)) == 0u;
			if (zero)
			{
				z80_block(73);
				hl = (hl & 0xFFu) | 0xA000u;
				z80_write8((uint16_t)(iy + 8u), z80_read8((uint16_t)(iy + 8u)) + 1u);
				a = z80_read8((uint16_t)(iy + 8u));
				SOUND_RAM8(0xE02Cu) = a;
				port_out(0xA0u, a);
			}
			z80_block(21);
			zero = (uint8_t)(0xE0u - (de >> 8)) == 0u;
			if (zero == 0u)
				goto L_000CA5;
			z80_block(7);
			de = (de & 0xFFu) | 0xD000u;
			goto L_000CA5;
		}
		z80_block(48);
		z80_write8((uint16_t)(iy + 0x10u), (uint8_t)hl);
		z80_write8((uint16_t)(iy + 0x11u), hl >> 8);
		z80_block(53);
		iy += 0x12u;
		hl = 0xE029u;
		t1 = SOUND_RAM8(0xE029u);
		SOUND_RAM8(0xE029u) = t1 - 1u;
		if (t1 != 1u)
		{
			z80_block(30);
			SOUND_RAM16(0xE027u) = iy;
			return;
		}
		z80_cycles(5);
		z80_block(46);
		z80_write8(hl, 8u);
		SOUND_RAM16(0xE027u) = 0xE400u;
		return;
	}
	z80_block(25);
	hl = (hl & 0xFFu) | (((hl >> 8) & 0xEFu) << 8);
	bc = 0x100u;
	goto L_000C28;
}

void sound_0D06(uint16_t iy)
{
	uint32_t carry = 0;
	uint8_t a = 0, bc = 0;
	uint16_t bc_2 = 0, de = 0, hl = 0;

	z80_block(50);
	a = z80_read8((uint16_t)(iy + 1u)) | 0xC0u;
	bc = 7u;
	sound_036D(bc, a);
	z80_block(72);
	a = ((a & 7u) * 2u) | 0x80u;
	a = ((uint32_t)a << 1) | (a >> 7);
	a = ((uint32_t)a << 1) | (a >> 7);
	a = ((uint32_t)a << 1) | (a >> 7);
	a = ((((uint32_t)a << 1) | (a >> 7)) & 0xF0u) + 0x18u;
	bc = 5u;
	sound_036D(bc, a);
	z80_block(227);
	z80_write8(iy, 0u);
	SOUND_RAM8(0xE026u) ^= z80_read8((uint16_t)(0x6F1u + z80_read8((uint16_t)(iy + 1u))));
	z80_write8((uint16_t)(iy + 3u), 0u);
	z80_write8((uint16_t)(iy + 9u), 0xD0u);
	z80_write8((uint16_t)(iy + 0x10u), 0xFFu);
	z80_write8((uint16_t)(iy + 0x11u), 7u);
	SOUND_RAM8(0xE020u)--;
	carry = z80_read8((uint16_t)(iy + 6u)) < 0x10u;
	if (carry == 0u)
	{
		z80_cycles(6);
		return;
	}
	z80_block(46);
	hl = 0xE406u;
	de = 0x12u;
	a = z80_read8((uint16_t)(iy + 6u));
	bc_2 = 0x800u;
	for (;;)
	{
		z80_block(17);
		carry = a < z80_read8(hl);
		if (carry == 0u)
		{
		L_000D60:
			z80_block(19);
			hl += de;
			bc_2 = (uint32_t)(uint8_t)((bc_2 >> 8) - 1u) << 8;
			if (bc_2 >> 8 == 0u)
				break;
			z80_cycles(5);
			continue;
		}
		z80_block(11);
		z80_write8(hl, z80_read8(hl) - 1u);
		goto L_000D60;
	}
	z80_block(10);
}

uint16_t sound_0DFE(uint16_t de, uint16_t hl)
{
	uint32_t carry = 0;

	z80_block(35);
	de &= 0xFFu;
	hl &= ~0xFFu;
	carry = (int16_t)hl < 0;
	hl = hl * 2u;
	if (carry)
	{
		z80_block(11);
		hl += de;
	}
	z80_block(21);
	carry = (int16_t)hl < 0;
	hl = hl * 2u;
	if (carry)
	{
		z80_block(11);
		hl += de;
	}
	z80_block(21);
	carry = (int16_t)hl < 0;
	hl = hl * 2u;
	if (carry)
	{
		z80_block(11);
		hl += de;
	}
	z80_block(21);
	carry = (int16_t)hl < 0;
	hl = hl * 2u;
	if (carry)
	{
		z80_block(11);
		hl += de;
	}
	z80_block(21);
	carry = (int16_t)hl < 0;
	hl = hl * 2u;
	if (carry)
	{
		z80_block(11);
		hl += de;
	}
	z80_block(21);
	carry = (int16_t)hl < 0;
	hl = hl * 2u;
	if (carry)
	{
		z80_block(11);
		hl += de;
	}
	z80_block(21);
	carry = (int16_t)hl < 0;
	hl = hl * 2u;
	if (carry)
	{
		z80_block(11);
		hl += de;
	}
	z80_block(16);
	carry = (int16_t)hl < 0;
	hl = hl * 2u;
	if (carry == 0u)
	{
		z80_cycles(6);
		return hl;
	}
	z80_block(21);
	hl += de;
	return hl;
}

void sound_0E29(uint16_t *p_de, uint16_t *p_hl)
{
	uint16_t de = (uint16_t)*p_de;
	uint16_t hl = (uint16_t)*p_hl;

	z80_block(4106);
	z80_write8(de, z80_read8(hl));
	hl++;
	de++;
	z80_write8(de, z80_read8(hl));
	hl++;
	de++;
	z80_write8(de, z80_read8(hl));
	hl++;
	de++;
	z80_write8(de, z80_read8(hl));
	hl++;
	de++;
	z80_write8(de, z80_read8(hl));
	hl++;
	de++;
	z80_write8(de, z80_read8(hl));
	hl++;
	de++;
	z80_write8(de, z80_read8(hl));
	hl++;
	de++;
	z80_write8(de, z80_read8(hl));
	hl++;
	de++;
	z80_write8(de, z80_read8(hl));
	hl++;
	de++;
	z80_write8(de, z80_read8(hl));
	hl++;
	de++;
	z80_write8(de, z80_read8(hl));
	hl++;
	de++;
	z80_write8(de, z80_read8(hl));
	hl++;
	de++;
	z80_write8(de, z80_read8(hl));
	hl++;
	de++;
	z80_write8(de, z80_read8(hl));
	hl++;
	de++;
	z80_write8(de, z80_read8(hl));
	hl++;
	de++;
	z80_write8(de, z80_read8(hl));
	hl++;
	de++;
	z80_write8(de, z80_read8(hl));
	hl++;
	de++;
	z80_write8(de, z80_read8(hl));
	hl++;
	de++;
	z80_write8(de, z80_read8(hl));
	hl++;
	de++;
	z80_write8(de, z80_read8(hl));
	hl++;
	de++;
	z80_write8(de, z80_read8(hl));
	hl++;
	de++;
	z80_write8(de, z80_read8(hl));
	hl++;
	de++;
	z80_write8(de, z80_read8(hl));
	hl++;
	de++;
	z80_write8(de, z80_read8(hl));
	hl++;
	de++;
	z80_write8(de, z80_read8(hl));
	hl++;
	de++;
	z80_write8(de, z80_read8(hl));
	hl++;
	de++;
	z80_write8(de, z80_read8(hl));
	hl++;
	de++;
	z80_write8(de, z80_read8(hl));
	hl++;
	de++;
	z80_write8(de, z80_read8(hl));
	hl++;
	de++;
	z80_write8(de, z80_read8(hl));
	hl++;
	de++;
	z80_write8(de, z80_read8(hl));
	hl++;
	de++;
	z80_write8(de, z80_read8(hl));
	hl++;
	de++;
	z80_write8(de, z80_read8(hl));
	hl++;
	de++;
	z80_write8(de, z80_read8(hl));
	hl++;
	de++;
	z80_write8(de, z80_read8(hl));
	hl++;
	de++;
	z80_write8(de, z80_read8(hl));
	hl++;
	de++;
	z80_write8(de, z80_read8(hl));
	hl++;
	de++;
	z80_write8(de, z80_read8(hl));
	hl++;
	de++;
	z80_write8(de, z80_read8(hl));
	hl++;
	de++;
	z80_write8(de, z80_read8(hl));
	hl++;
	de++;
	z80_write8(de, z80_read8(hl));
	hl++;
	de++;
	z80_write8(de, z80_read8(hl));
	hl++;
	de++;
	z80_write8(de, z80_read8(hl));
	hl++;
	de++;
	z80_write8(de, z80_read8(hl));
	hl++;
	de++;
	z80_write8(de, z80_read8(hl));
	hl++;
	de++;
	z80_write8(de, z80_read8(hl));
	hl++;
	de++;
	z80_write8(de, z80_read8(hl));
	hl++;
	de++;
	z80_write8(de, z80_read8(hl));
	hl++;
	de++;
	z80_write8(de, z80_read8(hl));
	hl++;
	de++;
	z80_write8(de, z80_read8(hl));
	hl++;
	de++;
	z80_write8(de, z80_read8(hl));
	hl++;
	de++;
	z80_write8(de, z80_read8(hl));
	hl++;
	de++;
	z80_write8(de, z80_read8(hl));
	hl++;
	de++;
	z80_write8(de, z80_read8(hl));
	hl++;
	de++;
	z80_write8(de, z80_read8(hl));
	hl++;
	de++;
	z80_write8(de, z80_read8(hl));
	hl++;
	de++;
	z80_write8(de, z80_read8(hl));
	hl++;
	de++;
	z80_write8(de, z80_read8(hl));
	hl++;
	de++;
	z80_write8(de, z80_read8(hl));
	hl++;
	de++;
	z80_write8(de, z80_read8(hl));
	hl++;
	de++;
	z80_write8(de, z80_read8(hl));
	hl++;
	de++;
	z80_write8(de, z80_read8(hl));
	hl++;
	de++;
	z80_write8(de, z80_read8(hl));
	hl++;
	de++;
	z80_write8(de, z80_read8(hl));
	hl++;
	de++;
	z80_write8(de, z80_read8(hl));
	hl++;
	de++;
	z80_write8(de, z80_read8(hl));
	hl++;
	de++;
	z80_write8(de, z80_read8(hl));
	hl++;
	de++;
	z80_write8(de, z80_read8(hl));
	hl++;
	de++;
	z80_write8(de, z80_read8(hl));
	hl++;
	de++;
	z80_write8(de, z80_read8(hl));
	hl++;
	de++;
	z80_write8(de, z80_read8(hl));
	hl++;
	de++;
	z80_write8(de, z80_read8(hl));
	hl++;
	de++;
	z80_write8(de, z80_read8(hl));
	hl++;
	de++;
	z80_write8(de, z80_read8(hl));
	hl++;
	de++;
	z80_write8(de, z80_read8(hl));
	hl++;
	de++;
	z80_write8(de, z80_read8(hl));
	hl++;
	de++;
	z80_write8(de, z80_read8(hl));
	hl++;
	de++;
	z80_write8(de, z80_read8(hl));
	hl++;
	de++;
	z80_write8(de, z80_read8(hl));
	hl++;
	de++;
	z80_write8(de, z80_read8(hl));
	hl++;
	de++;
	z80_write8(de, z80_read8(hl));
	hl++;
	de++;
	z80_write8(de, z80_read8(hl));
	hl++;
	de++;
	z80_write8(de, z80_read8(hl));
	hl++;
	de++;
	z80_write8(de, z80_read8(hl));
	hl++;
	de++;
	z80_write8(de, z80_read8(hl));
	hl++;
	de++;
	z80_write8(de, z80_read8(hl));
	hl++;
	de++;
	z80_write8(de, z80_read8(hl));
	hl++;
	de++;
	z80_write8(de, z80_read8(hl));
	hl++;
	de++;
	z80_write8(de, z80_read8(hl));
	hl++;
	de++;
	z80_write8(de, z80_read8(hl));
	hl++;
	de++;
	z80_write8(de, z80_read8(hl));
	hl++;
	de++;
	z80_write8(de, z80_read8(hl));
	hl++;
	de++;
	z80_write8(de, z80_read8(hl));
	hl++;
	de++;
	z80_write8(de, z80_read8(hl));
	hl++;
	de++;
	z80_write8(de, z80_read8(hl));
	hl++;
	de++;
	z80_write8(de, z80_read8(hl));
	hl++;
	de++;
	z80_write8(de, z80_read8(hl));
	hl++;
	de++;
	z80_write8(de, z80_read8(hl));
	hl++;
	de++;
	z80_write8(de, z80_read8(hl));
	hl++;
	de++;
	z80_write8(de, z80_read8(hl));
	hl++;
	de++;
	z80_write8(de, z80_read8(hl));
	hl++;
	de++;
	z80_write8(de, z80_read8(hl));
	hl++;
	de++;
	z80_write8(de, z80_read8(hl));
	hl++;
	de++;
	z80_write8(de, z80_read8(hl));
	hl++;
	de++;
	z80_write8(de, z80_read8(hl));
	hl++;
	de++;
	z80_write8(de, z80_read8(hl));
	hl++;
	de++;
	z80_write8(de, z80_read8(hl));
	hl++;
	de++;
	z80_write8(de, z80_read8(hl));
	hl++;
	de++;
	z80_write8(de, z80_read8(hl));
	hl++;
	de++;
	z80_write8(de, z80_read8(hl));
	hl++;
	de++;
	z80_write8(de, z80_read8(hl));
	hl++;
	de++;
	z80_write8(de, z80_read8(hl));
	hl++;
	de++;
	z80_write8(de, z80_read8(hl));
	hl++;
	de++;
	z80_write8(de, z80_read8(hl));
	hl++;
	de++;
	z80_write8(de, z80_read8(hl));
	hl++;
	de++;
	z80_write8(de, z80_read8(hl));
	hl++;
	de++;
	z80_write8(de, z80_read8(hl));
	hl++;
	de++;
	z80_write8(de, z80_read8(hl));
	hl++;
	de++;
	z80_write8(de, z80_read8(hl));
	hl++;
	de++;
	z80_write8(de, z80_read8(hl));
	hl++;
	de++;
	z80_write8(de, z80_read8(hl));
	hl++;
	de++;
	z80_write8(de, z80_read8(hl));
	hl++;
	de++;
	z80_write8(de, z80_read8(hl));
	hl++;
	de++;
	z80_write8(de, z80_read8(hl));
	hl++;
	de++;
	z80_write8(de, z80_read8(hl));
	hl++;
	de++;
	z80_write8(de, z80_read8(hl));
	hl++;
	de++;
	z80_write8(de, z80_read8(hl));
	hl++;
	de++;
	z80_write8(de, z80_read8(hl));
	hl++;
	de++;
	z80_write8(de, z80_read8(hl));
	hl++;
	de++;
	z80_write8(de, z80_read8(hl));
	hl++;
	de++;
	z80_write8(de, z80_read8(hl));
	hl++;
	de++;
	z80_write8(de, z80_read8(hl));
	hl++;
	de++;
	z80_write8(de, z80_read8(hl));
	hl++;
	de++;
	z80_write8(de, z80_read8(hl));
	hl++;
	de++;
	z80_write8(de, z80_read8(hl));
	hl++;
	de++;
	z80_write8(de, z80_read8(hl));
	hl++;
	de++;
	z80_write8(de, z80_read8(hl));
	hl++;
	de++;
	z80_write8(de, z80_read8(hl));
	hl++;
	de++;
	z80_write8(de, z80_read8(hl));
	hl++;
	de++;
	z80_write8(de, z80_read8(hl));
	hl++;
	de++;
	z80_write8(de, z80_read8(hl));
	hl++;
	de++;
	z80_write8(de, z80_read8(hl));
	hl++;
	de++;
	z80_write8(de, z80_read8(hl));
	hl++;
	de++;
	z80_write8(de, z80_read8(hl));
	hl++;
	de++;
	z80_write8(de, z80_read8(hl));
	hl++;
	de++;
	z80_write8(de, z80_read8(hl));
	hl++;
	de++;
	z80_write8(de, z80_read8(hl));
	hl++;
	de++;
	z80_write8(de, z80_read8(hl));
	hl++;
	de++;
	z80_write8(de, z80_read8(hl));
	hl++;
	de++;
	z80_write8(de, z80_read8(hl));
	hl++;
	de++;
	z80_write8(de, z80_read8(hl));
	hl++;
	de++;
	z80_write8(de, z80_read8(hl));
	hl++;
	de++;
	z80_write8(de, z80_read8(hl));
	hl++;
	de++;
	z80_write8(de, z80_read8(hl));
	hl++;
	de++;
	z80_write8(de, z80_read8(hl));
	hl++;
	de++;
	z80_write8(de, z80_read8(hl));
	hl++;
	de++;
	z80_write8(de, z80_read8(hl));
	hl++;
	de++;
	z80_write8(de, z80_read8(hl));
	hl++;
	de++;
	z80_write8(de, z80_read8(hl));
	hl++;
	de++;
	z80_write8(de, z80_read8(hl));
	hl++;
	de++;
	z80_write8(de, z80_read8(hl));
	hl++;
	de++;
	z80_write8(de, z80_read8(hl));
	hl++;
	de++;
	z80_write8(de, z80_read8(hl));
	hl++;
	de++;
	z80_write8(de, z80_read8(hl));
	hl++;
	de++;
	z80_write8(de, z80_read8(hl));
	hl++;
	de++;
	z80_write8(de, z80_read8(hl));
	hl++;
	de++;
	z80_write8(de, z80_read8(hl));
	hl++;
	de++;
	z80_write8(de, z80_read8(hl));
	hl++;
	de++;
	z80_write8(de, z80_read8(hl));
	hl++;
	de++;
	z80_write8(de, z80_read8(hl));
	hl++;
	de++;
	z80_write8(de, z80_read8(hl));
	hl++;
	de++;
	z80_write8(de, z80_read8(hl));
	hl++;
	de++;
	z80_write8(de, z80_read8(hl));
	hl++;
	de++;
	z80_write8(de, z80_read8(hl));
	hl++;
	de++;
	z80_write8(de, z80_read8(hl));
	hl++;
	de++;
	z80_write8(de, z80_read8(hl));
	hl++;
	de++;
	z80_write8(de, z80_read8(hl));
	hl++;
	de++;
	z80_write8(de, z80_read8(hl));
	hl++;
	de++;
	z80_write8(de, z80_read8(hl));
	hl++;
	de++;
	z80_write8(de, z80_read8(hl));
	hl++;
	de++;
	z80_write8(de, z80_read8(hl));
	hl++;
	de++;
	z80_write8(de, z80_read8(hl));
	hl++;
	de++;
	z80_write8(de, z80_read8(hl));
	hl++;
	de++;
	z80_write8(de, z80_read8(hl));
	hl++;
	de++;
	z80_write8(de, z80_read8(hl));
	hl++;
	de++;
	z80_write8(de, z80_read8(hl));
	hl++;
	de++;
	z80_write8(de, z80_read8(hl));
	hl++;
	de++;
	z80_write8(de, z80_read8(hl));
	hl++;
	de++;
	z80_write8(de, z80_read8(hl));
	hl++;
	de++;
	z80_write8(de, z80_read8(hl));
	hl++;
	de++;
	z80_write8(de, z80_read8(hl));
	hl++;
	de++;
	z80_write8(de, z80_read8(hl));
	hl++;
	de++;
	z80_write8(de, z80_read8(hl));
	hl++;
	de++;
	z80_write8(de, z80_read8(hl));
	hl++;
	de++;
	z80_write8(de, z80_read8(hl));
	hl++;
	de++;
	z80_write8(de, z80_read8(hl));
	hl++;
	de++;
	z80_write8(de, z80_read8(hl));
	hl++;
	de++;
	z80_write8(de, z80_read8(hl));
	hl++;
	de++;
	z80_write8(de, z80_read8(hl));
	hl++;
	de++;
	z80_write8(de, z80_read8(hl));
	hl++;
	de++;
	z80_write8(de, z80_read8(hl));
	hl++;
	de++;
	z80_write8(de, z80_read8(hl));
	hl++;
	de++;
	z80_write8(de, z80_read8(hl));
	hl++;
	de++;
	z80_write8(de, z80_read8(hl));
	hl++;
	de++;
	z80_write8(de, z80_read8(hl));
	hl++;
	de++;
	z80_write8(de, z80_read8(hl));
	hl++;
	de++;
	z80_write8(de, z80_read8(hl));
	hl++;
	de++;
	z80_write8(de, z80_read8(hl));
	hl++;
	de++;
	z80_write8(de, z80_read8(hl));
	hl++;
	de++;
	z80_write8(de, z80_read8(hl));
	hl++;
	de++;
	z80_write8(de, z80_read8(hl));
	hl++;
	de++;
	z80_write8(de, z80_read8(hl));
	hl++;
	de++;
	z80_write8(de, z80_read8(hl));
	hl++;
	de++;
	z80_write8(de, z80_read8(hl));
	hl++;
	de++;
	z80_write8(de, z80_read8(hl));
	hl++;
	de++;
	z80_write8(de, z80_read8(hl));
	hl++;
	de++;
	z80_write8(de, z80_read8(hl));
	hl++;
	de++;
	z80_write8(de, z80_read8(hl));
	hl++;
	de++;
	z80_write8(de, z80_read8(hl));
	hl++;
	de++;
	z80_write8(de, z80_read8(hl));
	hl++;
	de++;
	z80_write8(de, z80_read8(hl));
	hl++;
	de++;
	z80_write8(de, z80_read8(hl));
	hl++;
	de++;
	z80_write8(de, z80_read8(hl));
	hl++;
	de++;
	z80_write8(de, z80_read8(hl));
	hl++;
	de++;
	z80_write8(de, z80_read8(hl));
	hl++;
	de++;
	z80_write8(de, z80_read8(hl));
	hl++;
	de++;
	z80_write8(de, z80_read8(hl));
	hl++;
	de++;
	z80_write8(de, z80_read8(hl));
	hl++;
	de++;
	z80_write8(de, z80_read8(hl));
	hl++;
	de++;
	z80_write8(de, z80_read8(hl));
	hl++;
	de++;
	z80_write8(de, z80_read8(hl));
	hl++;
	de++;
	z80_write8(de, z80_read8(hl));
	hl++;
	de++;
	z80_write8(de, z80_read8(hl));
	hl++;
	de++;
	z80_write8(de, z80_read8(hl));
	hl++;
	de++;
	z80_write8(de, z80_read8(hl));
	hl++;
	de++;
	z80_write8(de, z80_read8(hl));
	hl++;
	de++;
	z80_write8(de, z80_read8(hl));
	hl++;
	de++;
	z80_write8(de, z80_read8(hl));
	hl++;
	de++;
	z80_write8(de, z80_read8(hl));
	hl++;
	de++;
	z80_write8(de, z80_read8(hl));
	hl++;
	de++;
	z80_write8(de, z80_read8(hl));
	hl++;
	de++;
	z80_write8(de, z80_read8(hl));
	hl++;
	de++;
	z80_write8(de, z80_read8(hl));
	hl++;
	de++;
	z80_write8(de, z80_read8(hl));
	hl++;
	de++;
	z80_write8(de, z80_read8(hl));
	hl++;
	de++;
	z80_write8(de, z80_read8(hl));
	hl++;
	de++;
	z80_write8(de, z80_read8(hl));
	hl++;
	de++;
	z80_write8(de, z80_read8(hl));
	hl++;
	de++;
	z80_write8(de, z80_read8(hl));
	hl++;
	de++;
	z80_write8(de, z80_read8(hl));
	hl++;
	de++;
	z80_write8(de, z80_read8(hl));
	hl++;
	de++;
	z80_write8(de, z80_read8(hl));
	hl++;
	de++;
	z80_write8(de, z80_read8(hl));
	hl++;
	de++;
	z80_write8(de, z80_read8(hl));
	hl++;
	de++;
	z80_write8(de, z80_read8(hl));
	hl++;
	de++;
	z80_write8(de, z80_read8(hl));
	hl++;
	de++;
	*p_de = de;
	*p_hl = hl;
}

void sound_1192(uint16_t *p_de, uint16_t *p_hl, uint8_t *p_a)
{
	uint16_t hl = (uint16_t)*p_hl;
	uint32_t zero = 0;
	uint8_t a = 0;
	uint16_t bc = 0, de = 0, hl_2 = 0, iy = 0;

	z80_block(51);
	iy = 0xE490u;
	de = 0x12u;
	bc = 0xC00u;
	a = SOUND_RAM8(0xE008u) & 0xFu;
	for (;;)
	{
		z80_block(29);
		zero = (uint8_t)(a - z80_read8((uint16_t)(iy + 3u))) == 0u;
		if (zero == 0u)
		{
		L_0011BE:
			z80_block(23);
			iy += de;
			bc = (uint32_t)(uint8_t)((bc >> 8) - 1u) << 8;
			if (bc >> 8 == 0u)
			{
				z80_block(10);
				*p_de = de;
				*p_hl = hl;
				*p_a = a;
				return;
			}
			z80_cycles(5);
			continue;
		}
		z80_block(46);
		hl = (hl & ~0xFFu) | a;
		zero = (uint8_t)(SOUND_RAM8(0xE012u) - z80_read8((uint16_t)(iy + 2u))) == 0u;
		if (zero == 0u)
		{
		L_0011BD:
			z80_block(4);
			a = (uint8_t)hl;
			goto L_0011BE;
		}
		z80_block(37);
		a = z80_read8(iy);
		zero = ((a >> 1) & 1u) == 0u;
		if (zero == 0u)
			goto L_0011BD;
		z80_block(17);
		if ((a & 0x7Fu) != 0u)
			break;
		goto L_0011BD;
	}
	z80_block(65);
	a = (uint8_t)hl;
	de = a;
	hl_2 = 0xE060u + de;
	if (z80_read8(hl_2) >> 7 == 0u)
	{
		a = sound_11D7(iy);
		*p_de = de;
		*p_hl = hl_2;
		*p_a = a;
		return;
	}
	z80_block(33);
	z80_write8(iy, z80_read8(iy) | 2u);
	*p_de = de;
	*p_hl = hl_2;
	*p_a = a;
}

uint8_t sound_11D7(uint16_t iy)
{
	uint8_t a = 0, bc = 0;

	z80_block(212);
	z80_write8(iy, 0u);
	z80_write8((uint16_t)(iy + 3u), 0u);
	z80_write8((uint16_t)(iy + 9u), 0u);
	z80_write8((uint16_t)(iy + 0xAu), 0u);
	z80_write8((uint16_t)(iy + 0xFu), 0u);
	z80_write8((uint16_t)(iy + 0x10u), 0u);
	z80_write8((uint16_t)(iy + 0x11u), 0u);
	z80_write8((uint16_t)(iy + 0xDu), 0u);
	a = z80_read8((uint16_t)(iy + 1u));
	SOUND_RAM8(0xE01Fu) = a;
	a &= 7u;
	bc = 0x28u;
	return sound_0221(bc, a);
}

void sound_125B(uint16_t iy)
{
	uint32_t zero = 0;
	uint8_t a = 0, bc = 0;

	z80_block(99);
	z80_write8(iy, 0u);
	z80_write8((uint16_t)(iy + 3u), 0u);
	bc = 0x28u;
	a = z80_read8((uint16_t)(iy + 1u));
	SOUND_RAM8(0xE01Fu) = a;
	zero = ((a >> 3) & 1u) == 0u;
	if (zero)
	{
		z80_block(17);
		a &= 7u;
		sound_023D(bc, a);
		return;
	}
	z80_block(17);
	a &= 7u;
	sound_025B(bc, a);
}

void sound_1279(uint8_t bc, uint16_t hl, uint16_t iy)
{
	uint32_t zero = 0;
	uint8_t a = 0, de = 0;
	uint16_t bc_2 = 0, de_2 = 0, hl_2 = 0;
	uint16_t stack_4 = 0;

	z80_block(104);
	a = (uint8_t)bc;
	SOUND_RAM8(0xE01Fu) = a;
	bc_2 = a & 3u;
	hl = ((hl + 4u) & 0xFFu) | ((uint32_t)z80_read8((uint16_t)(hl + 4u)) << 8);
	a = z80_read8((uint16_t)(iy + 8u));
	de = a;
	zero = a == 0u;
	a = a;
	if (zero == 0u)
	{
		z80_block(20);
		de += (uint8_t)~a >> 1;
	}
	z80_block(17);
	hl = sound_0DFE(de, hl);
	z80_block(78);
	SOUND_RAM8(0xE013u) = 0x7Fu - ((uint16_t)((uint32_t)((hl >> 8) >> 1) << 8) >> 8);
	a = z80_read8((uint16_t)(iy + 3u));
	hl_2 = 0x193Bu;
	hl_2 = sound_03FF(hl_2, a);
	z80_block(40);
	a = z80_read8((uint16_t)(hl_2 + 1u));
	hl_2 = 0x242Eu;
	hl_2 = sound_03FF(hl_2, a);
	z80_block(38);
	hl_2 += 0x15u;
	bc_2 |= 0x400u;
	de_2 = 0x1360u;
	for (;;)
	{
		z80_block(63);
		stack_4 = bc_2;
		a = z80_read8(de_2) + bc_2;
		de_2++;
		bc_2 = a;
		a = z80_read8(hl_2);
		hl_2++;
		zero = a >> 7 == 0u;
		if (zero == 0u)
		{
		L_0012CC:
			z80_block(18);
			bc_2 = (stack_4 & 0xFFu) | ((uint32_t)(uint8_t)((stack_4 >> 8) - 1u) << 8);
			if (bc_2 >> 8 == 0u)
				break;
			z80_cycles(5);
			continue;
		}
		z80_block(38);
		bc_2 |= (uint32_t)a << 8;
		a = SOUND_RAM8(0xE013u) + (bc_2 >> 8);
		sound_022E((uint8_t)bc_2, a);
		goto L_0012CC;
	}
	z80_block(20);
}

void sound_12D1(uint8_t bc, uint16_t hl, uint16_t ix)
{
	uint32_t zero = 0;
	uint8_t a = 0, de = 0;
	uint16_t bc_2 = 0, de_2 = 0, hl_2 = 0;
	uint16_t stack_4 = 0;

	z80_block(104);
	a = (uint8_t)bc;
	SOUND_RAM8(0xE01Fu) = a;
	bc_2 = a & 3u;
	hl = ((hl + 4u) & 0xFFu) | ((uint32_t)z80_read8((uint16_t)(hl + 4u)) << 8);
	a = z80_read8((uint16_t)(ix + 8u));
	de = a;
	zero = a == 0u;
	a = a;
	if (zero == 0u)
	{
		z80_block(20);
		de += (uint8_t)~a >> 1;
	}
	z80_block(17);
	hl = sound_0DFE(de, hl);
	z80_block(78);
	SOUND_RAM8(0xE013u) = 0x7Fu - ((uint16_t)((uint32_t)((hl >> 8) >> 1) << 8) >> 8);
	a = z80_read8((uint16_t)(ix + 3u));
	hl_2 = 0x193Bu;
	hl_2 = sound_03FF(hl_2, a);
	z80_block(40);
	a = z80_read8((uint16_t)(hl_2 + 1u));
	hl_2 = 0x242Eu;
	hl_2 = sound_03FF(hl_2, a);
	z80_block(38);
	hl_2 += 0x15u;
	bc_2 |= 0x400u;
	de_2 = 0x1360u;
	for (;;)
	{
		z80_block(63);
		stack_4 = bc_2;
		a = z80_read8(de_2) + bc_2;
		de_2++;
		bc_2 = a;
		a = z80_read8(hl_2);
		hl_2++;
		zero = a >> 7 == 0u;
		if (zero == 0u)
		{
		L_001324:
			z80_block(18);
			bc_2 = (stack_4 & 0xFFu) | ((uint32_t)(uint8_t)((stack_4 >> 8) - 1u) << 8);
			if (bc_2 >> 8 == 0u)
				break;
			z80_cycles(5);
			continue;
		}
		z80_block(38);
		bc_2 |= (uint32_t)a << 8;
		a = SOUND_RAM8(0xE013u) + (bc_2 >> 8);
		sound_022E((uint8_t)bc_2, a);
		goto L_001324;
	}
	z80_block(20);
}

void sound_1329(uint16_t *p_bc, uint16_t *p_de, uint16_t *p_hl)
{
	uint8_t a = 0, bc_2 = 0;
	uint16_t bc = 0, de = 0, hl = 0;
	uint16_t stack_2 = 0;

	z80_block(64);
	bc = SOUND_RAM8(0xE01Fu) & 3u;
	a = SOUND_RAM8(0xE00Du);
	hl = 0x242Eu;
	hl = sound_03FF(hl, a);
	z80_block(17);
	de = 0x134Bu;
	bc |= 0x1500u;
	for (;;)
	{
		z80_block(62);
		stack_2 = bc;
		a = z80_read8(de) + bc;
		de++;
		bc_2 = a;
		a = z80_read8(hl);
		hl++;
		sound_022E(bc_2, a);
		z80_block(18);
		bc = (stack_2 & 0xFFu) | ((uint32_t)(uint8_t)((stack_2 >> 8) - 1u) << 8);
		if (bc >> 8 == 0u)
			break;
		z80_cycles(5);
	}
	z80_block(10);
	*p_bc = bc;
	*p_de = de;
	*p_hl = hl;
}

void sound_4645(void)
{
	uint32_t zero = 0, carry = 0;
	uint16_t de = 0, hl = 0, iy = 0;
	uint8_t t1 = 0;

	z80_block(28);
	hl = 0xE740u;
	t1 = SOUND_RAM8(0xE740u);
	SOUND_RAM8(0xE740u) = t1 - 1u;
	if (t1 == 1u)
	{
		z80_cycles(5);
		z80_block(55);
		z80_write8(hl, 4u);
		iy = 0xE700u;
		carry = SOUND_RAM8(0xE759u) < 0x40u;
		if (carry == 0u)
		{
			z80_cycles(6);
			return;
		}
		z80_block(51);
		de = 0xE100u | SOUND_RAM8(0xE758u);
		hl = 0xEA00u;
		sound_4A1C(de, hl, iy);
		z80_block(24);
		hl = hl;
		sound_4726(hl, iy);
		z80_block(27);
		hl = 0xEB00u;
		sound_4726(hl, iy);
		z80_block(27);
		hl = 0xEC00u;
		sound_4726(hl, iy);
		z80_block(27);
		hl = 0xED00u;
		sound_4726(hl, iy);
		z80_block(27);
		hl = 0xEE00u;
		sound_4726(hl, iy);
		z80_block(27);
		hl = 0xEF00u;
		sound_4726(hl, iy);
		z80_block(20);
		hl = 0xF000u;
		sound_4726(hl, iy);
		return;
	}
	z80_block(19);
	zero = (uint8_t)(2u - z80_read8(hl)) == 0u;
	if (zero == 0u)
	{
		z80_cycles(6);
		return;
	}
	z80_block(45);
	iy = 0xE700u;
	carry = SOUND_RAM8(0xE759u) < 0x40u;
	if (carry == 0u)
	{
		z80_cycles(6);
		return;
	}
	z80_block(27);
	zero = SOUND_RAM8(0xE918u) == 0u;
	if (zero == 0u)
	{
		z80_cycles(7);
		sound_49C3(iy);
	}
	z80_block(32);
	hl = 0xE900u;
	if (SOUND_RAM8(0xE900u) >> 7 != 0u)
	{
		sound_4729(hl, iy);
		return;
	}
	z80_block(10);
}

void sound_46AA(uint16_t de, uint16_t ix, uint16_t iy, uint16_t *p_hl, uint8_t *p_a)
{
	uint16_t hl = (uint16_t)*p_hl;
	uint8_t a = (uint8_t)*p_a;
	uint32_t zero = 0;
	uint8_t a_2 = 0;
	uint16_t hl_2 = 0;
	uint16_t stack_2 = 0, stack_6 = 0;
	uint32_t target1 = 0;

	z80_block(45);
	stack_2 = hl;
	a &= 0xFu;
	hl_2 = 0x4706u;
	hl_2 = sound_03FF(hl_2, a);
	z80_block(4);
	target1 = hl_2;
	switch (target1)
	{
	case 0x0046B4u:
		z80_block(57);
		hl_2 = ((uint32_t)z80_read8((uint16_t)(stack_2 + 1u)) << 8) | z80_read8(stack_2);
		a_2 = z80_read8(hl_2);
		hl_2++;
		*p_hl = hl_2;
		*p_a = a_2;
		return;
	case 0x0046BCu:
		z80_block(53);
		hl_2 = stack_2;
		a_2 = (z80_read8((uint16_t)(iy + 9u)) & 7u) | 0x90u;
		*p_hl = hl_2;
		*p_a = a_2;
		return;
	case 0x0046C5u:
		z80_block(53);
		hl_2 = stack_2;
		a_2 = (z80_read8((uint16_t)(iy + 0xAu)) & 7u) | 0x90u;
		*p_hl = hl_2;
		*p_a = a_2;
		return;
	case 0x0046CEu:
		z80_block(117);
		stack_6 = ix;
		hl_2 = stack_6;
		a_2 = 0u;
		z80_write8(hl_2, 0u);
		move_block(hl_2, (uint16_t)(((uint8_t)hl_2 | ((uint32_t)(hl_2 >> 8) << 8)) + 1u), 0x30u, 1u);
		hl_2 = stack_2;
		*p_hl = hl_2;
		*p_a = a_2;
		return;
	case 0x0046DFu:
		z80_block(32);
		zero = (uint8_t)((de >> 8) + 0x1Eu) == 0u;
		if (zero)
		{
			z80_block(57);
			a_2 = 0u;
			SOUND_RAM8(0xF100u) = 0u;
			move_block(0xF100u, 0xF101u, 0x4FFu, 1u);
		}
		else
		{
			z80_block(67);
			a_2 = 0u;
			SOUND_RAM8(0xE900u) = 0u;
			move_block(0xE900u, 0xE901u, 0x7FFu, 1u);
		}
		z80_block(30);
		hl_2 = stack_2;
		*p_hl = hl_2;
		*p_a = a_2;
		return;
	default:
		sound_unknown_code(target1);
	}
}

void sound_4726(uint16_t hl, uint16_t iy)
{
	z80_block(17);
	if (z80_read8(hl) >> 7 == 0u)
	{
		z80_cycles(6);
		return;
	}
	sound_4729(hl, iy);
}

void sound_4729(uint16_t hl, uint16_t iy)
{
	uint32_t zero = 0, carry = 0;
	uint8_t a = 0, bc_2 = 0;
	uint16_t bc = 0, de = 0, hl_2 = 0, ix = 0;
	uint16_t stack_2 = 0, stack_4 = 0;

	z80_block(64);
	hl_2 = (hl & ~0xFFu) | 6u;
	bc = z80_read8(hl_2);
	hl_2 = (hl_2 & ~0xFFu) | (uint8_t)(hl_2 + 1u);
	bc = (bc | ((uint32_t)z80_read8(hl_2) << 8)) - 1u;
	z80_write8(hl_2, bc >> 8);
	hl_2--;
	z80_write8(hl_2, (uint8_t)bc);
	if ((uint8_t)((bc >> 8) | bc) != 0u)
	{
		z80_cycles(6);
		return;
	}
	z80_block(56);
	de = 0xE100u | SOUND_RAM8(0xE758u);
	stack_2 = hl_2 & ~0xFFu;
	ix = stack_2;
	for (;;)
	{
		z80_block(83);
		port_out(0xB0u, 0u);
		port_out(0xA0u, z80_read8((uint16_t)(ix + 0x1Bu)));
		hl_2 = z80_read8((uint16_t)(ix + 2u)) | ((uint32_t)z80_read8((uint16_t)(ix + 3u)) << 8);
		for (;;)
		{
			z80_block(48);
			z80_write8(ix, z80_read8(ix) & 0xBFu);
			a = z80_read8(hl_2);
			zero = a >> 7 == 0u;
			if (zero)
			{
				z80_block(35);
				a = z80_read8((uint16_t)(ix + 0x14u));
				hl_2--;
			}
			else
			{
				z80_block(19);
				z80_write8((uint16_t)(ix + 0x14u), a);
			}
			z80_block(17);
			zero = (uint8_t)(a + 1u) == 0u;
			if (zero)
			{
				z80_block(17);
				hl_2 = sound_4A64(hl_2, ix);
				z80_block(24);
				a = z80_read8(hl_2);
				zero = (uint8_t)(a - 0x2Fu) == 0u;
				if (zero)
					goto L_00487C;
				z80_block(17);
				zero = (uint8_t)(a - 0x51u) == 0u;
				if (zero)
				{
					z80_block(17);
					hl_2 = sound_4A64(hl_2, ix);
					z80_block(17);
					hl_2 = sound_4A64(hl_2, ix);
					z80_block(25);
					a = z80_read8(hl_2);
					zero = a >> 7 == 0u;
					if (zero == 0u)
					{
						z80_block(23);
						a = (0u - ((uint16_t)((uint32_t)a << 8) >> 8)) | 0x80u;
					}
					z80_block(36);
					z80_write8((uint16_t)(ix + 0xAu), a);
					hl_2 = sound_4A64(hl_2, ix);
					z80_block(10);
				}
				else
				{
					z80_block(17);
					hl_2 = sound_4A64(hl_2, ix);
					z80_block(7);
					bc = (uint32_t)z80_read8(hl_2) << 8;
					for (;;)
					{
						z80_block(17);
						hl_2 = sound_4A64(hl_2, ix);
						z80_block(8);
						bc = (uint32_t)(uint8_t)((bc >> 8) - 1u) << 8;
						if (bc >> 8 == 0u)
							break;
						z80_cycles(5);
					}
					z80_block(17);
					hl_2 = sound_4A64(hl_2, ix);
					z80_block(10);
				}
			L_004840:
				z80_block(30);
				if (((z80_read8(ix) >> 6) & 1u) != 0u)
					continue;
				z80_block(24);
				a = z80_read8(hl_2);
				hl_2 = sound_4A64(hl_2, ix);
				z80_block(14);
				zero = a == 0u;
				a = a;
				if (zero)
					continue;
				goto L_00484F;
			}
			z80_block(28);
			bc_2 = a;
			a &= ~0xFu;
			zero = (uint8_t)(a + 0x10u) == 0u;
			if (zero)
			{
				z80_block(21);
				a = (uint8_t)bc_2;
				hl_2 = sound_4A64(hl_2, ix);
				z80_block(17);
				zero = (uint8_t)(a + 9u) == 0u;
				if (zero)
				{
					z80_block(7);
					bc = (uint32_t)z80_read8(hl_2) << 8;
					for (;;)
					{
						z80_block(24);
						hl_2 = sound_4A64(hl_2, ix);
						z80_block(8);
						bc = (uint32_t)(uint8_t)((bc >> 8) - 1u) << 8;
						if (bc >> 8 == 0u)
							break;
						z80_cycles(5);
					}
					z80_block(10);
				L_0047EF:
					z80_block(33);
					z80_write8((uint16_t)(iy + 0x59u), z80_read8((uint16_t)(iy + 0x59u)) + 1u);
					goto L_004840;
				}
				z80_block(17);
				zero = (uint8_t)(a + 0x10u) == 0u;
				if (zero)
				{
					for (;;)
					{
						z80_block(24);
						a = z80_read8(hl_2);
						hl_2 = sound_4A64(hl_2, ix);
						z80_block(17);
						zero = (uint8_t)(a + 9u) == 0u;
						if (zero)
							break;
						z80_block(10);
					}
					goto L_0047EF;
				}
				break;
			}
			z80_block(17);
			zero = (uint8_t)(a + 0x80u) == 0u;
			if (zero)
			{
				z80_block(32);
				z80_write8(de, bc_2);
				de = (de & ~0xFFu) | (uint8_t)(de + 1u);
				hl_2 = sound_4A64(hl_2, ix);
				z80_block(25);
				a = z80_read8(hl_2);
				zero = a >> 7 == 0u;
				if (zero == 0u)
				{
					z80_cycles(7);
					a = sound_477F(ix, a);
				}
				z80_block(28);
				z80_write8(de, a);
				de = (de & ~0xFFu) | (uint8_t)(de + 1u);
				hl_2 = sound_4A64(hl_2, ix);
				z80_block(51);
				z80_write8(de, 0x7Fu);
				de = (de & ~0xFFu) | (uint8_t)(de + 1u);
				z80_write8((uint16_t)(iy + 0x59u), z80_read8((uint16_t)(iy + 0x59u)) + 1u);
				goto L_004840;
			}
			z80_block(17);
			zero = (uint8_t)(a + 0x50u) == 0u;
			if (zero)
			{
				z80_block(32);
				z80_write8(de, bc_2);
				de = (de & ~0xFFu) | (uint8_t)(de + 1u);
				hl_2 = sound_4A64(hl_2, ix);
				z80_block(35);
				a = z80_read8(hl_2);
				z80_write8(de, a);
				de = (de & ~0xFFu) | (uint8_t)(de + 1u);
				hl_2 = sound_4A64(hl_2, ix);
				z80_block(24);
				zero = (uint8_t)(a - 0x10u) == 0u;
				a = z80_read8(hl_2);
				if (zero)
				{
					z80_block(37);
					bc_2 = a;
					zero = z80_read8((uint16_t)(ix + 0x18u)) == 0u;
					if (zero)
					{
						z80_block(31);
						a = (uint8_t)bc_2;
						z80_write8((uint16_t)(ix + 0x15u), bc_2 & 0x7Fu);
					}
					else
					{
						z80_block(29);
						a = z80_read8((uint16_t)(ix + 0x15u));
					}
				}
				z80_block(18);
				zero = a >> 7 == 0u;
				if (zero == 0u)
				{
					z80_cycles(7);
					a = sound_477F(ix, a);
				}
				z80_block(28);
				z80_write8(de, a);
				de = (de & ~0xFFu) | (uint8_t)(de + 1u);
				hl_2 = sound_4A64(hl_2, ix);
				z80_block(33);
				z80_write8((uint16_t)(iy + 0x59u), z80_read8((uint16_t)(iy + 0x59u)) + 1u);
				goto L_004840;
			}
			z80_block(17);
			zero = (uint8_t)(a + 0x40u) == 0u;
			if (zero == 0u)
			{
				z80_block(17);
				zero = (uint8_t)(a + 0x30u) == 0u;
				if (zero == 0u)
				{
					z80_block(10);
					z80_block(32);
					z80_write8(de, bc_2);
					de = (de & ~0xFFu) | (uint8_t)(de + 1u);
					hl_2 = sound_4A64(hl_2, ix);
					z80_block(7);
					bc_2 = z80_read8(hl_2);
				}
			}
			z80_block(32);
			z80_write8(de, bc_2);
			de = (de & ~0xFFu) | (uint8_t)(de + 1u);
			hl_2 = sound_4A64(hl_2, ix);
			z80_block(25);
			a = z80_read8(hl_2);
			zero = a >> 7 == 0u;
			if (zero == 0u)
			{
				z80_cycles(7);
				a = sound_477F(ix, a);
			}
			z80_block(28);
			z80_write8(de, a);
			de = (de & ~0xFFu) | (uint8_t)(de + 1u);
			hl_2 = sound_4A64(hl_2, ix);
			goto L_0047EF;
		}
		z80_block(10);
	L_00487C:
		z80_block(30);
		if (((z80_read8(ix) >> 5) & 1u) == 0u)
		{
			z80_block(30);
			if (((z80_read8(ix) >> 4) & 1u) != 0u)
			{
				z80_cycles(7);
				sound_48C0(iy);
			}
			z80_block(100);
			z80_write8(ix, 0u);
			z80_write8((uint16_t)(iy + 0x58u), (uint8_t)de);
			port_out(0xB0u, SOUND_RAM8(0xE02Du));
			port_out(0xA0u, SOUND_RAM8(0xE02Cu));
			return;
		}
		z80_block(62);
		hl_2 = z80_read8((uint16_t)(ix + 0x20u)) | ((uint32_t)z80_read8((uint16_t)(ix + 0x21u)) << 8);
		a = z80_read8(hl_2);
		hl_2 = sound_4A64(hl_2, ix);
		z80_block(17);
		carry = a < 0xF0u;
		if (carry == 0u)
		{
			z80_cycles(7);
			sound_46AA(de, ix, iy, &hl_2, &a);
		}
		z80_block(14);
		zero = a == 0u;
		a = a;
		if (zero)
		{
			z80_block(30);
			if (((z80_read8(ix) >> 4) & 1u) != 0u)
			{
				z80_cycles(7);
				sound_48C0(iy);
			}
			z80_block(100);
			z80_write8(ix, 0u);
			z80_write8((uint16_t)(iy + 0x58u), (uint8_t)de);
			port_out(0xB0u, SOUND_RAM8(0xE02Du));
			port_out(0xA0u, SOUND_RAM8(0xE02Cu));
			return;
		}
		z80_block(91);
		z80_write8((uint16_t)(ix + 0x20u), (uint8_t)hl_2);
		z80_write8((uint16_t)(ix + 0x21u), hl_2 >> 8);
		stack_2 = de;
		stack_4 = ix;
		de = stack_4;
		sound_4919(de, a);
		z80_block(20);
		de = stack_2;
	}
L_00484F:
	z80_block(18);
	zero = a >> 7 == 0u;
	if (zero == 0u)
	{
		z80_block(37);
		a &= 0x7Fu;
		carry = a & 1u;
		bc_2 = a >> 1;
		a = z80_read8(hl_2);
		if (carry)
		{
			z80_block(8);
			a |= 0x80u;
		}
		z80_block(17);
		hl_2 = sound_4A64(hl_2, ix);
		z80_block(19);
		z80_write8((uint16_t)(ix + 7u), bc_2);
	}
	z80_block(134);
	z80_write8((uint16_t)(ix + 6u), a);
	z80_write8((uint16_t)(ix + 2u), (uint8_t)hl_2);
	z80_write8((uint16_t)(ix + 3u), hl_2 >> 8);
	z80_write8((uint16_t)(iy + 0x58u), (uint8_t)de);
	port_out(0xB0u, SOUND_RAM8(0xE02Du));
	port_out(0xA0u, SOUND_RAM8(0xE02Cu));
}

uint8_t sound_477F(uint16_t ix, uint8_t a)
{
	z80_block(41);
	a &= 0x7Fu;
	z80_write8(ix, z80_read8(ix) | 0x40u);
	return a;
}

void sound_48C0(uint16_t iy)
{
	z80_block(33);
	z80_write8((uint16_t)(iy + 0xBu), 0u);
}

void sound_48E0(void)
{
	uint32_t zero = 0, carry = 0;
	uint8_t a = 0;
	uint16_t bc = 0, de = 0, hl = 0;
	uint16_t t1 = 0;

	z80_block(26);
	hl = 0xE701u;
	a = SOUND_RAM8(0xE701u);
	zero = a == 0u;
	a = a;
	if (zero)
	{
		z80_cycles(6);
		return;
	}
	z80_block(55);
	z80_write8(hl, 0u);
	bc = (uint32_t)z80_read8((hl & ~0xFFu) | (uint8_t)(hl - 1u)) << 8;
	a &= 0x7Fu;
	hl = 0x4A78u;
	hl = sound_03FF(hl, a);
	z80_block(16);
	carry = z80_read8(hl) < bc >> 8;
	if (carry)
	{
		z80_cycles(6);
		return;
	}
	z80_block(37);
	a = (uint8_t)(bc >> 8);
	hl++;
	de = 0xE900u;
	hl = sound_03FF(hl, a);
	z80_block(29);
	t1 = de;
	de = hl;
	hl = t1;
	a = z80_read8(de);
	zero = a >> 7 == 0u;
	if (zero == 0u)
	{
		z80_block(18);
		zero = ((a >> 6) & 1u) == 0u;
		if (zero == 0u)
		{
			z80_block(29);
			hl = (uint32_t)(uint8_t)((hl >> 8) + 1u) << 8;
			hl = (uint32_t)(uint8_t)((hl >> 8) + 1u) << 8;
			hl = (uint32_t)(uint8_t)((hl >> 8) + 1u) << 8;
			sound_49A9(&de, &hl);
			z80_block(46);
			a = z80_read8(de) & 0xFu;
			carry = a < (uint16_t)((z80_read8(hl) & 0xFu) << 8) >> 8;
			if (carry)
			{
				z80_block(10);
				return;
			}
			z80_block(17);
			hl = sound_49B4(hl);
			z80_block(87);
			de++;
			z80_write8(hl, a);
			hl = (hl & ~0xFFu) | 2u;
			z80_write8(hl, (uint8_t)de);
			hl = (hl & ~0xFFu) | (uint8_t)(hl + 1u);
			z80_write8(hl, de >> 8);
			hl = (hl & ~0xFFu) | 6u;
			z80_write8(hl, 1u);
			hl &= ~0xFFu;
			z80_write8(hl, z80_read8(hl) | 0x80u);
			return;
		}
		z80_block(10);
		z80_block(54);
		hl = (uint32_t)(uint8_t)((hl >> 8) + 1u) << 8;
		hl = (uint32_t)(uint8_t)((hl >> 8) + 1u) << 8;
		a = z80_read8(de) & 0xFu;
		carry = a < (uint16_t)((z80_read8(hl) & 0xFu) << 8) >> 8;
		if (carry)
		{
			z80_block(10);
			return;
		}
		z80_block(17);
		hl = sound_49B4(hl);
		z80_block(7);
		z80_write8(hl, a);
		z80_block(80);
		de++;
		hl = (hl & ~0xFFu) | 2u;
		z80_write8(hl, (uint8_t)de);
		hl = (hl & ~0xFFu) | (uint8_t)(hl + 1u);
		z80_write8(hl, de >> 8);
		hl = (hl & ~0xFFu) | 6u;
		z80_write8(hl, 1u);
		hl &= ~0xFFu;
		z80_write8(hl, z80_read8(hl) | 0x80u);
		return;
	}
	z80_block(18);
	zero = ((a >> 6) & 1u) == 0u;
	if (zero == 0u)
	{
		z80_block(80);
		de++;
		hl = (hl & ~0xFFu) | 2u;
		z80_write8(hl, (uint8_t)de);
		hl = (hl & ~0xFFu) | (uint8_t)(hl + 1u);
		z80_write8(hl, de >> 8);
		hl = (hl & ~0xFFu) | 6u;
		z80_write8(hl, 1u);
		hl &= ~0xFFu;
		z80_write8(hl, z80_read8(hl) | 0x80u);
		return;
	}
	z80_block(18);
	zero = ((a >> 5) & 1u) == 0u;
	if (zero == 0u)
	{
		sound_4937(de, hl);
		return;
	}
	z80_block(10);
	z80_block(17);
	hl = sound_49B4(hl);
	z80_block(82);
	de++;
	a = z80_read8(de);
	z80_write8(hl, z80_read8(hl) | 0x20u);
	de++;
	hl = (hl & ~0xFFu) | 0x20u;
	z80_write8(hl, (uint8_t)de);
	hl++;
	z80_write8(hl, de >> 8);
	de = hl & ~0xFFu;
	sound_4919(de, a);
}

void sound_4919(uint16_t de, uint8_t a)
{
	uint16_t de_2 = 0, hl = 0;
	uint16_t t1 = 0;

	z80_block(27);
	hl = 0x4C66u;
	hl = sound_03FF(hl, a);
	z80_block(21);
	t1 = de;
	de_2 = hl;
	hl = t1;
	sound_4937(de_2, hl);
}

void sound_4937(uint16_t de, uint16_t hl)
{
	uint8_t a = 0;
	uint16_t hl_2 = 0;

	z80_block(127);
	de++;
	hl_2 = (hl & ~0xFFu) | 0x1Bu;
	a = z80_read8(de);
	de++;
	z80_write8(hl_2, a);
	hl_2 = (hl_2 & ~0xFFu) | 2u;
	z80_write8(hl_2, z80_read8(de));
	hl_2 = (hl_2 & ~0xFFu) | (uint8_t)(hl_2 + 1u);
	z80_write8(hl_2, z80_read8((uint16_t)(de + 1u)));
	hl_2 = (hl_2 & ~0xFFu) | 6u;
	z80_write8(hl_2, 1u);
	hl_2 &= ~0xFFu;
	z80_write8(hl_2, z80_read8(hl_2) | 0x80u);
}

void sound_49A9(uint16_t *p_de, uint16_t *p_hl)
{
	uint16_t de = (uint16_t)*p_de;
	uint16_t hl = (uint16_t)*p_hl;

	z80_block(55);
	de++;
	hl = (uint32_t)(uint8_t)((z80_read8(de) & 0xFu) - 0xAu + (hl >> 8)) << 8;
	de--;
	*p_de = de;
	*p_hl = (*p_hl & 0xFFu) | hl;
}

uint16_t sound_49B4(uint16_t hl)
{
	uint16_t stack_4 = 0;

	z80_block(101);
	stack_4 = hl;
	z80_write8(hl, 0u);
	move_block(hl, (uint16_t)(stack_4 + 1u), 0x30u, 1u);
	hl = (hl + 0x30u) & 0xFF00u;
	return hl;
}

void sound_49C3(uint16_t iy)
{
	uint32_t zero = 0;
	uint16_t de = 0, hl = 0;

	z80_block(28);
	hl = 0xE919u;
	if (SOUND_RAM8(0xE919u) != 0u)
	{
		z80_block(21);
		z80_write8(hl, z80_read8(hl) - 1u);
		return;
	}
	z80_cycles(5);
	z80_block(63);
	de = 0xE100u | SOUND_RAM8(0xE758u);
	hl = (hl & ~0xFFu) | 0x15u;
	z80_write8(hl, z80_read8(hl) - 1u);
	zero = z80_read8(hl) == 0u;
	if (zero)
	{
		z80_block(223);
		hl &= ~0xFFu;
		z80_write8(hl, z80_read8(hl) & 0x7Fu);
		z80_write8(hl | 0x18u, 0u);
		z80_write8(de, 0xBEu);
		de = (de & ~0xFFu) | (uint8_t)(de + 1u);
		z80_write8(de, 0x50u);
		de = (de & ~0xFFu) | (uint8_t)(de + 1u);
		z80_write8(de, 0x7Eu);
		de = (de & ~0xFFu) | (uint8_t)(de + 1u);
		z80_write8((uint16_t)(iy + 0x59u), z80_read8((uint16_t)(iy + 0x59u)) + 1u);
		z80_write8(de, 0xBEu);
		de = (de & ~0xFFu) | (uint8_t)(de + 1u);
		z80_write8(de, 0x79u);
		de = (de & ~0xFFu) | (uint8_t)(de + 1u);
		z80_write8(de, 0u);
		z80_write8((uint16_t)(iy + 0x59u), z80_read8((uint16_t)(iy + 0x59u)) + 1u);
		z80_write8((uint16_t)(iy + 0x58u), de + 1u);
		return;
	}
	z80_block(134);
	z80_write8(de, 0xBEu);
	de = (de & ~0xFFu) | (uint8_t)(de + 1u);
	z80_write8(de, 0x10u);
	de = (de & ~0xFFu) | (uint8_t)(de + 1u);
	z80_write8(de, z80_read8(hl));
	z80_write8((uint16_t)(iy + 0x58u), de + 1u);
	z80_write8((uint16_t)(iy + 0x59u), z80_read8((uint16_t)(iy + 0x59u)) + 1u);
	hl = (hl & ~0xFFu) | 0x18u;
	z80_write8((hl & ~0xFFu) | 0x19u, z80_read8(hl));
}

void sound_4A1C(uint16_t de, uint16_t hl, uint16_t iy)
{
	uint32_t zero = 0;
	uint16_t hl_2 = 0;

	z80_block(23);
	hl_2 = (hl & ~0xFFu) | 0x18u;
	zero = z80_read8(hl_2) == 0u;
	if (zero)
	{
		z80_cycles(6);
		return;
	}
	z80_block(28);
	hl_2 = (hl_2 & ~0xFFu) | 0x19u;
	zero = z80_read8(hl_2) == 0u;
	if (zero == 0u)
	{
		z80_block(21);
		z80_write8(hl_2, z80_read8(hl_2) - 1u);
		return;
	}
	z80_block(39);
	hl_2 = (hl_2 & ~0xFFu) | 0x1Au;
	z80_write8(hl_2, z80_read8(hl_2) - 1u);
	zero = z80_read8(hl_2) == 0u;
	if (zero)
	{
		z80_block(138);
		hl_2 &= ~0xFFu;
		z80_write8(hl_2, 0u);
		z80_write8(hl_2 | 0x18u, 0u);
		z80_write8(de, 0xBAu);
		de = (de & ~0xFFu) | (uint8_t)(de + 1u);
		z80_write8(de, 0x7Bu);
		de = (de & ~0xFFu) | (uint8_t)(de + 1u);
		z80_write8(de, 0u);
		z80_write8((uint16_t)(iy + 0x59u), z80_read8((uint16_t)(iy + 0x59u)) + 1u);
		z80_write8((uint16_t)(iy + 0x58u), de + 1u);
		return;
	}
	z80_block(134);
	z80_write8(de, 0xBAu);
	de = (de & ~0xFFu) | (uint8_t)(de + 1u);
	z80_write8(de, 7u);
	de = (de & ~0xFFu) | (uint8_t)(de + 1u);
	z80_write8(de, z80_read8(hl_2));
	z80_write8((uint16_t)(iy + 0x58u), de + 1u);
	z80_write8((uint16_t)(iy + 0x59u), z80_read8((uint16_t)(iy + 0x59u)) + 1u);
	hl_2 = (hl_2 & ~0xFFu) | 0x18u;
	z80_write8((hl_2 & ~0xFFu) | 0x19u, z80_read8(hl_2));
}

uint16_t sound_4A64(uint16_t hl, uint16_t ix)
{
	uint32_t zero = 0;

	z80_block(38);
	hl++;
	zero = (uint8_t)((hl >> 8) + 0x40u) == 0u;
	if (zero)
	{
		z80_block(60);
		z80_write8((uint16_t)(ix + 0x1Bu), z80_read8((uint16_t)(ix + 0x1Bu)) + 1u);
		port_out(0xA0u, z80_read8((uint16_t)(ix + 0x1Bu)));
		hl = (hl & 0xFFu) | 0xA000u;
	}
	z80_block(20);
	return hl;
}

void sound_61EF(uint16_t iy)
{
	uint32_t zero = 0, carry = 0;
	uint8_t a = 0, bc_2 = 0;
	uint16_t bc = 0, de = 0, hl = 0;
	uint16_t stack_2 = 0;
	uint8_t t1 = 0;

	z80_block(33);
	zero = z80_read8((uint16_t)(iy + 7u)) == 0u;
	if (zero == 0u)
	{
		z80_block(33);
		z80_write8((uint16_t)(iy + 7u), z80_read8((uint16_t)(iy + 7u)) - 1u);
		return;
	}
	z80_block(33);
	zero = z80_read8((uint16_t)(iy + 8u)) == 0u;
	if (zero == 0u)
	{
		z80_block(33);
		z80_write8((uint16_t)(iy + 8u), z80_read8((uint16_t)(iy + 8u)) - 1u);
		return;
	}
	z80_block(305);
	z80_write8((uint16_t)(iy + 8u), z80_read8((uint16_t)(iy + 0xEu)));
	SOUND_RAM8(0xE01Fu) = z80_read8((uint16_t)(iy + 1u));
	a = z80_read8((uint16_t)(iy + 2u));
	hl = 0x137Cu + (uint8_t)((a & 0xFu) * 2u);
	de = SOUND_RAM16(0xE014u);
	hl = (((uint32_t)z80_read8((uint16_t)(hl + 1u)) << 8) | z80_read8(hl)) + de + de;
	hl = ((uint32_t)z80_read8((uint16_t)(hl + 1u)) << 8) | z80_read8(hl);
	SOUND_RAM16(0xE034u) = hl;
	hl = (hl & 0xFFu) | ((uint32_t)(uint8_t)(((a & 0x70u) >> 1) + (hl >> 8)) << 8);
	if ((z80_read8((uint16_t)(iy + 0xDu)) & 1u) != 0u)
	{
		z80_block(214);
		de = hl;
		carry = 0u;
		hl = (z80_read8((uint16_t)(iy + 0xFu)) | ((uint32_t)z80_read8((uint16_t)(iy + 0x10u)) << 8)) - z80_read8((uint16_t)(iy + 0xCu)) - carry;
		z80_write8((uint16_t)(iy + 0xFu), (uint8_t)hl);
		z80_write8((uint16_t)(iy + 0x10u), hl >> 8);
		hl += (de & 0xFFu) | (((de >> 8) & 7u) << 8);
		hl = (hl & 0xFFu) | ((uint32_t)(uint8_t)(((uint16_t)(((de >> 8) & ~7u) << 8) >> 8) + (hl >> 8)) << 8);
		if ((z80_read8((uint16_t)(iy + 9u)) & 0x70u) == 0u)
		{
		L_0063D9:
			z80_block(167);
			z80_write8((uint16_t)(iy + 9u), z80_read8((uint16_t)(iy + 9u)) & 0xFu);
			stack_2 = hl;
			de = stack_2;
			carry = 0u;
			hl = SOUND_RAM16(0xE034u) - (((z80_read8((uint16_t)(iy + 9u)) & 7u) << 8) | z80_read8((uint16_t)(iy + 0xAu))) - carry;
			if (((hl >> 8) & 0xC0u) != 0u)
			{
				z80_block(83);
				stack_2 = de;
				carry = 0u;
				carry = z80_carry_sub16(0u, (uint16_t)((stack_2 & 0xFFu) | ((uint32_t)(stack_2 >> 8) << 8)), carry);
				hl = de;
				if (carry)
				{
					z80_block(97);
					z80_write8((uint16_t)(iy + 9u), (z80_read8((uint16_t)(iy + 0xDu)) & 0x70u) + z80_read8((uint16_t)(iy + 9u)));
					z80_write8((uint16_t)(iy + 0xDu), z80_read8((uint16_t)(iy + 0xDu)) - 1u);
				}
			}
			else
			{
				z80_block(69);
				stack_2 = de;
				carry = 0u;
				carry = z80_carry_sub16(hl, (uint16_t)((stack_2 & 0xFFu) | (((stack_2 >> 8) & 7u) << 8)), carry);
				hl = de;
				if (carry == 0u)
				{
					z80_block(10);
					z80_block(97);
					z80_write8((uint16_t)(iy + 9u), (z80_read8((uint16_t)(iy + 0xDu)) & 0x70u) + z80_read8((uint16_t)(iy + 9u)));
					z80_write8((uint16_t)(iy + 0xDu), z80_read8((uint16_t)(iy + 0xDu)) - 1u);
					z80_block(94);
					hl = (hl & 0xFFu) | ((uint32_t)(uint8_t)((z80_read8((uint16_t)(iy + 0x11u)) & ~7u) + (hl >> 8)) << 8);
					de = (SOUND_RAM8(0xE01Fu) & 3u) << 8;
					bc_2 = 0xA4u | (de >> 8);
					a = (uint8_t)(hl >> 8);
					sound_022E(bc_2, a);
					z80_block(36);
					bc_2 = 0xA0u | (de >> 8);
					a = (uint8_t)hl;
					sound_022E(bc_2, a);
					z80_block(10);
					return;
				}
			}
		}
		else
		{
			z80_block(83);
			stack_2 = hl;
			carry = 0u;
			carry = z80_carry_sub16((uint16_t)((hl & 0xFFu) | (((hl >> 8) & 7u) << 8)), 0x269u, carry);
			hl = stack_2;
			if (carry)
			{
				z80_block(304);
				de = hl;
				bc = SOUND_RAM16(0xE034u);
				carry = 0u;
				hl = 0x4CFu - bc - carry;
				z80_write8((uint16_t)(iy + 0xFu), (uint8_t)hl);
				z80_write8((uint16_t)(iy + 0x10u), hl >> 8);
				stack_2 = bc;
				hl = stack_2 + (z80_read8((uint16_t)(iy + 0xFu)) | ((uint32_t)z80_read8((uint16_t)(iy + 0x10u)) << 8));
				hl = (hl & 0xFFu) | ((uint32_t)(uint8_t)(((de >> 8) & 0x38u) + (hl >> 8)) << 8);
				de = ((hl >> 8) & 0x38u) << 8;
				a = 0u - ((z80_read8((uint16_t)(iy + 9u)) & 0x70u) >> 1) + (de >> 8);
				bc = (uint8_t)((z80_read8((uint16_t)(iy + 0x11u)) & ~7u) + (de >> 8)) | ((uint32_t)a << 8);
				t1 = a & ~7u;
				a = t1;
				if (t1 != 0u)
				{
					z80_block(17);
					if ((a & 0xC0u) != 0u)
						goto L_0063BE;
					z80_block(18);
					carry = bc >> 8 < (uint8_t)bc;
					if (carry == 0u)
						goto L_0063D0;
					z80_block(10);
					z80_block(59);
					z80_write8((uint16_t)(iy + 0x11u), z80_read8((uint16_t)(iy + 0x11u)) - 8u);
					z80_block(94);
					hl = (hl & 0xFFu) | ((uint32_t)(uint8_t)((z80_read8((uint16_t)(iy + 0x11u)) & ~7u) + (hl >> 8)) << 8);
					de = (SOUND_RAM8(0xE01Fu) & 3u) << 8;
					bc_2 = 0xA4u | (de >> 8);
					a = (uint8_t)(hl >> 8);
					sound_022E(bc_2, a);
					z80_block(36);
					bc_2 = 0xA0u | (de >> 8);
					a = (uint8_t)hl;
					sound_022E(bc_2, a);
					z80_block(10);
					return;
				}
			L_0063BE:
				z80_block(21);
				carry = (uint8_t)bc > 8u;
				if (carry == 0u)
				{
				L_0063D0:
					z80_block(49);
					z80_write8((uint16_t)(iy + 0x11u), z80_read8((uint16_t)(iy + 0x11u)) - 8u);
					goto L_0063D9;
				}
				z80_block(59);
				z80_write8((uint16_t)(iy + 0x11u), z80_read8((uint16_t)(iy + 0x11u)) - 8u);
			}
		}
	}
	else
	{
		z80_block(164);
		de = hl;
		hl = (z80_read8((uint16_t)(iy + 0xFu)) | ((uint32_t)z80_read8((uint16_t)(iy + 0x10u)) << 8)) + z80_read8((uint16_t)(iy + 0xBu));
		z80_write8((uint16_t)(iy + 0xFu), (uint8_t)hl);
		z80_write8((uint16_t)(iy + 0x10u), hl >> 8);
		hl += de;
		if ((z80_read8((uint16_t)(iy + 9u)) & 0x70u) == 0u)
		{
		L_0062F5:
			z80_block(170);
			z80_write8((uint16_t)(iy + 9u), z80_read8((uint16_t)(iy + 9u)) & 0xFu);
			stack_2 = hl;
			de = stack_2;
			hl = SOUND_RAM16(0xE034u) + (((z80_read8((uint16_t)(iy + 9u)) & 7u) << 8) | z80_read8((uint16_t)(iy + 0xAu)));
			carry = ((hl >> 8) & 0xFu) < 4u;
			if (carry == 0u)
			{
				z80_block(7);
				hl = (hl & 0xFFu) | 0x500u;
			}
			else
				z80_block(10);
			z80_block(84);
			stack_2 = de;
			carry = 0u;
			carry = z80_carry_sub16((uint16_t)((hl & 0xFFu) | (((hl >> 8) & 7u) << 8)), (uint16_t)((stack_2 & 0xFFu) | (((stack_2 >> 8) & 7u) << 8)), carry);
			hl = de;
			if (carry)
			{
				z80_block(97);
				z80_write8((uint16_t)(iy + 9u), (z80_read8((uint16_t)(iy + 0xDu)) & 0x70u) + z80_read8((uint16_t)(iy + 9u)));
				z80_write8((uint16_t)(iy + 0xDu), z80_read8((uint16_t)(iy + 0xDu)) + 1u);
			}
		}
		else
		{
			z80_block(83);
			stack_2 = hl;
			carry = 0u;
			carry = z80_carry_sub16((uint16_t)((hl & 0xFFu) | (((hl >> 8) & 7u) << 8)), 0x4CEu, carry);
			hl = stack_2;
			if (carry == 0u)
			{
				z80_block(314);
				de = hl;
				bc = SOUND_RAM16(0xE034u);
				carry = 0u;
				hl = 0x269u - bc - carry;
				z80_write8((uint16_t)(iy + 0xFu), (uint8_t)hl);
				z80_write8((uint16_t)(iy + 0x10u), hl >> 8);
				stack_2 = bc;
				hl = stack_2 + (z80_read8((uint16_t)(iy + 0xFu)) | ((uint32_t)z80_read8((uint16_t)(iy + 0x10u)) << 8));
				hl = (hl & 0xFFu) | ((uint32_t)(uint8_t)(((de >> 8) & 0x38u) + (hl >> 8)) << 8);
				de = ((hl >> 8) & 0x38u) << 8;
				a = ((z80_read8((uint16_t)(iy + 9u)) & 0x70u) >> 1) + (de >> 8) - 8u;
				bc = ((((z80_read8((uint16_t)(iy + 0x11u)) & ~7u) + (de >> 8)) & 0xF8u) << 8) | a;
				carry = a < 0x30u;
				if (carry == 0u)
				{
					z80_block(7);
					bc = (bc & ~0xFFu) | 0x28u;
				}
				z80_block(21);
				if ((bc & 0xC0u) == 0u)
				{
					z80_block(18);
					carry = (uint8_t)bc < bc >> 8;
					if (carry)
						goto L_0062ED;
					z80_block(10);
					z80_block(45);
					z80_write8((uint16_t)(iy + 0x11u), 8u + z80_read8((uint16_t)(iy + 0x11u)));
					z80_block(94);
					hl = (hl & 0xFFu) | ((uint32_t)(uint8_t)((z80_read8((uint16_t)(iy + 0x11u)) & ~7u) + (hl >> 8)) << 8);
					de = (SOUND_RAM8(0xE01Fu) & 3u) << 8;
					bc_2 = 0xA4u | (de >> 8);
					a = (uint8_t)(hl >> 8);
					sound_022E(bc_2, a);
					z80_block(36);
					bc_2 = 0xA0u | (de >> 8);
					a = (uint8_t)hl;
					sound_022E(bc_2, a);
					z80_block(10);
					return;
				}
				z80_block(25);
				carry = (uint8_t)(bc + 8u) < bc >> 8;
				if (carry)
				{
				L_0062ED:
					z80_block(45);
					z80_write8((uint16_t)(iy + 0x11u), 8u + z80_read8((uint16_t)(iy + 0x11u)));
					goto L_0062F5;
				}
				z80_block(45);
				z80_write8((uint16_t)(iy + 0x11u), 8u + z80_read8((uint16_t)(iy + 0x11u)));
			}
		}
	}
	z80_block(94);
	hl = (hl & 0xFFu) | ((uint32_t)(uint8_t)((z80_read8((uint16_t)(iy + 0x11u)) & ~7u) + (hl >> 8)) << 8);
	de = (SOUND_RAM8(0xE01Fu) & 3u) << 8;
	bc_2 = 0xA4u | (de >> 8);
	a = (uint8_t)(hl >> 8);
	sound_022E(bc_2, a);
	z80_block(36);
	bc_2 = 0xA0u | (de >> 8);
	a = (uint8_t)hl;
	sound_022E(bc_2, a);
	z80_block(10);
}
