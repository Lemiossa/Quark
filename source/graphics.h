/**
 * graphics.h
 * Criado por Matheus Leme Da Silva
 */
#ifndef GRAPHICS_H
#define GRAPHICS_H
#include <defs.h>
#include <stdint.h>

#ifdef KERNEL_VESA_MODE
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
#endif // if KERNEL_VESA_MODE == 0x100
#endif // KERNEL_VESA_MODE

extern U8 vbe_supported;

void put_pixel(int x, int y, U8 color);

#endif // GRAPHICS_H
