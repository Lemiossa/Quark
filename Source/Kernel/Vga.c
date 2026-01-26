/**
 * Vga.c
 * Created by Matheus Leme Da Silva
 */
#include "Io.h"
#include "Types.h"

#define VGA_WIDTH 80
#define VGA_HEIGHT 25

U16 *Vga = (U16 *)0xB8000;
U16 CursorX = 0;
U16 CursorY = 0;
U8 CurrentAttributes = 0x07;

// Text-mode-cursor https://wiki.osdev.org/Text_Mode_Cursor
// Updates the cursor position
void CursorUpdate(U16 pos) {
	OutU8(0x3D4, 0xF);
	OutU8(0x3D5, (U8)pos & 0xFF);
	OutU8(0x3D4, 0xE);
	OutU8(0x3D5, (U8)(pos >> 8) & 0xFF);
}

// Get actual cursor position on text mode
U16 GetCursorPos(void) {
	U16 pos = 0;
	OutU8(0x3D4, 0xF);
	pos |= InU8(0x3D5);
	OutU8(0x3D4, 0xE);
	pos |= ((U16)InU8(0x3D5)) << 8;
	return pos;
}

// Set the current attributes
void SetCurrentAttr(U8 attr) {
	CurrentAttributes = attr;
}

// Returns the current attributes
U8 GetCurrentAttr(void) {
	return CurrentAttributes;
}

// Put char C at X and Y position with current_attributes
void Putcat(U16 x, U16 y, char c) {
	if (x >= VGA_WIDTH || y >= VGA_HEIGHT)
		return;

	U8 uc = c;
	Vga[y * VGA_WIDTH + x] = (CurrentAttributes << 8) | uc;
}

// Scroll N lines the screen
void Scroll(U16 n) {
	for (U16 y = n; y < VGA_HEIGHT; y++) {
		for (U16 x = 0; x < VGA_WIDTH; x++) {
			Vga[(y - n) * VGA_WIDTH + x] = Vga[y * VGA_WIDTH + x];
		}
	}

	for (int y = VGA_HEIGHT - n; y < VGA_HEIGHT; y++) {
		for (int x = 0; x < VGA_WIDTH; x++) {
			Vga[y * VGA_WIDTH + x] = 0x0720;
		}
	}
}

// Print char C at current cursor position and move de Cursor, scrolls if needed
void Putc(char c) {
	U16 pos = GetCursorPos();
	CursorY = pos / VGA_WIDTH;
	CursorX = pos % VGA_WIDTH;
	if (c == '\r') {
		CursorX = 0;
	} else if (c == '\n') {
		CursorY++;
	} else if (c == '\t') {
		CursorX += 4;
	} else if (c == '\b') {
		CursorX--;
	} else {
		Putcat(CursorX, CursorY, c);
		CursorX++;
	}

	if (CursorX >= VGA_WIDTH) {
		CursorX = 0;
		CursorY++;
	}

	if (CursorY >= VGA_HEIGHT) {
		Scroll(1);
		CursorY = VGA_HEIGHT - 1;
	}

	pos = CursorY * VGA_WIDTH + CursorX;
	CursorUpdate(pos);
}

// Prints a string using putc
void Puts(const char *s) {
	while (*s)
		Putc(*s++);
}
