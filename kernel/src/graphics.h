/**
 * graphics.h
 * Criado por Matheus Leme Da Silva
 */
#pragma once
#include "stdint.h"

#define VIDEO_MODE_WIDTH 640
#define VIDEO_MODE_HEIGHT 480

extern u8 vbe_supported;

void put_pixel(int x, int y, u8 color);
