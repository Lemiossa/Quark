/**
 * char.h
 * Criado por Matheus Leme Da Silva
 */
#pragma once
#include "../stdint.h"
#include "../kdefs.h"

#define CHAR_WIDTH 8
#if KERNEL_FONT == 0x03
#define CHAR_HEIGHT 8
#elif KERNEL_FONT == 0x06
#define CHAR_HEIGHT 16
#else
#error "Invalid font"
#endif

void draw_char(int x, int y, char c, u8 fg, u8 bg);
