/**
 * Vga.h
 * Created by Matheus Leme Da Silva
 */
#ifndef VGA_H
#define VGA_H
#include "Types.h"

#define MAKE_VGA_COLOR(fg, bg) (U8)((((bg) & 0xF) << 4) | ((fg) & 0xF))
#define GET_FG(attr) (U8)((attr) & 0xF)
#define GET_BG(attr) (U8)(((attr) >> 4) & 0xF)

enum VgaColor {
	BLACK = 0,
	BLUE,
	GREEN,
	CYAN,
	RED,
	PURPLE,
	BROWN,
	GRAY,
	DARK_GRAY,
	LIGHT_BLUE,
	LIGHT_GREEN,
	LIGHT_CYAN,
	LIGHT_RED,
	LIGHT_PURPLE,
	YELLOW,
	WHITE
};

U8 GetCurrentAttr(void);
void SetCurrentAttr(U8 attr);
U16 GetCursorPos(void);
void CursorUpdate(U16 pos);
void Putcat(U16 x, U16 y, char c);
void Putc(char c);
void Puts(const char *s);
void Clear(void);

#endif // VGA_H
