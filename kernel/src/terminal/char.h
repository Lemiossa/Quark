/**
 * char.h
 * Criado por Matheus Leme Da Silva
 */
#pragma once
#include "../stdint.h"

#define CHAR_WIDTH 8
#define CHAR_HEIGHT 8

void draw_char(int x, int y, char c, u8 fg, u8 bg);
