/**
 * graphics.h
 * Criado por Matheus Leme Da Silva
 */
#pragma once
#include "stdint.h"
#include "kdefs.h"

#if KERNEL_VESA_MODE == 0x100
#define VIDEO_MODE_WIDTH 640
#define VIDEO_MODE_HEIGHT 400
#elif KERNEL_VESA_MODE == 0x101
#define VIDEO_MODE_WIDTH 640
#define VIDEO_MODE_HEIGHT 480
#elif KERNEL_VESA_MODE == 0x103
#define VIDEO_MODE_WIDTH 800
#define VIDEO_MODE_HEIGHT 600
#else
#error "Unsuported VESA mode"
#endif

extern u8 vbe_supported;

void put_pixel(int x, int y, u8 color);
