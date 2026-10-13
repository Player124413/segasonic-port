/**
 *    Name:        sound.h
 *    Description: prototypes of the sound program's functions
 *    Version:     0.1.0
 *    Created:     13-10-2026
 *    Modified:    13-10-2026
 *    Author:      Sebastian Toyos
 *
 *    SegaSonic The Hedgehog (arcade, Sega System 32): the Z80 sound program decompiled into C by
 *    tools/decompile_sound.py, with the names of sound_symbols.txt.
 */
#ifndef SONIC_SOUND_H
#define SONIC_SOUND_H

#include "sound/support.h"



void sound_reset(void);
void sound_irq(void);
void sound_main_loop(void);
void sound_0174(uint8_t bc);
uint16_t sound_01D1(void);
uint8_t sound_0221(uint8_t bc, uint8_t a);
void sound_022E(uint8_t bc, uint8_t a);
void sound_023D(uint8_t bc, uint8_t a);
void sound_0249(uint8_t bc, uint8_t a);
void sound_025B(uint8_t bc, uint8_t a);
void sound_0267(uint8_t bc, uint8_t a);
void sound_02D1(uint8_t bc);
void sound_036D(uint8_t bc, uint8_t a);
void sound_0373(uint8_t bc);
void sound_039B(void);
void sound_03B5(void);
uint16_t sound_03FF(uint16_t hl, uint8_t a);
void sound_040B(void);
void sound_0450(uint8_t bc);
void sound_046C(uint16_t bc, uint16_t ix);
uint16_t sound_07E6(uint16_t bc, uint16_t ix, uint16_t iy);
void sound_08F0(uint16_t *p_bc, uint16_t *p_hl);
void sound_0B92(uint16_t *p_bc, uint16_t *p_hl, uint8_t *p_a);
void sound_0B9C(uint16_t iy);
void sound_0BF4(void);
void sound_0D06(uint16_t iy);
uint16_t sound_0DFE(uint16_t de, uint16_t hl);
void sound_0E29(uint16_t *p_de, uint16_t *p_hl);
void sound_1192(uint16_t *p_de, uint16_t *p_hl, uint8_t *p_a);
uint8_t sound_11D7(uint16_t iy);
void sound_125B(uint16_t iy);
void sound_1279(uint8_t bc, uint16_t hl, uint16_t iy);
void sound_12D1(uint8_t bc, uint16_t hl, uint16_t ix);
void sound_1329(uint16_t *p_bc, uint16_t *p_de, uint16_t *p_hl);
void sound_4645(void);
void sound_46AA(uint16_t de, uint16_t ix, uint16_t iy, uint16_t *p_hl, uint8_t *p_a);
void sound_4726(uint16_t hl, uint16_t iy);
void sound_4729(uint16_t hl, uint16_t iy);
uint8_t sound_477F(uint16_t ix, uint8_t a);
void sound_48C0(uint16_t iy);
void sound_48E0(void);
void sound_4919(uint16_t de, uint8_t a);
void sound_4937(uint16_t de, uint16_t hl);
void sound_49A9(uint16_t *p_de, uint16_t *p_hl);
uint16_t sound_49B4(uint16_t hl);
void sound_49C3(uint16_t iy);
void sound_4A1C(uint16_t de, uint16_t hl, uint16_t iy);
uint16_t sound_4A64(uint16_t hl, uint16_t ix);
void sound_61EF(uint16_t iy);

#endif
