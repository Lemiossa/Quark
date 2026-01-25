/**
 * Vga.c
 * Created by Matheus Leme Da Silva
 */
#include "Io.h"
#include "Types.h"
#include "Util.h"

#define VGA_WIDTH 80
#define VGA_HEIGHT 25

U16 *Vga = (U16 *)0xB8000;
U16 Cursor_x = 0;
U16 Cursor_y = 0;
U8 Current_attributes = 0x07;

// Updates the cursor position
void CursorUpdate(U16 x, U16 y) {
  U16 pos = y * VGA_WIDTH + x;

  OutU8(0x3D4, 0xF);
  OutU8(0x3D5, (U8)pos & 0xFF);
  OutU8(0x3D4, 0xE);
  OutU8(0x3D5, (U8)(pos >> 8) & 0xFF);
}

// Put char C at X and Y position with current_attributes
void Putcat(U16 x, U16 y, char c) {
  if (x >= VGA_WIDTH || y >= VGA_HEIGHT)
    return;

  U8 uc = c;
  Vga[y * VGA_WIDTH + x] = (Current_attributes << 8) | uc;
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
  if (c == '\r') {
    Cursor_x = 0;
  } else if (c == '\n') {
    Cursor_y++;
  } else if (c == '\t') {
    Cursor_x += 4;
  } else {
    Putcat(Cursor_x, Cursor_y, c);
    Cursor_x++;
  }

  if (Cursor_x >= VGA_WIDTH) {
    Cursor_x = 0;
    Cursor_y++;
  }

  if (Cursor_y >= VGA_HEIGHT) {
    Scroll(1);
    Cursor_y = VGA_HEIGHT - 1;
  }

  CursorUpdate(Cursor_x, Cursor_y);
}

// Prints a string using putc
void Puts(const char *s) {
  while (*s) {
    Putc(*s++);
  }
}

// Clear the screen
void Clear(void) {
  Cursor_x = 0;
  Cursor_y = 0;
  Current_attributes = 0x07;
  for (U16 y = 0; y < VGA_HEIGHT; y++) {
    for (U16 x = 0; x < VGA_WIDTH; x++) {
      Putcat(x, y, ' ');
    }
  }
}
