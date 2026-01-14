/**
 * terminal.h
 * Criado por Matheus Leme Da Silva
 */
#ifndef TERMINAL_H
#define TERMINAL_H
#include <defs.h>
#include <stddef.h>
#include <stdint.h>

#define VGA_BLACK 0
#define VGA_BLUE 1
#define VGA_GREEN 2
#define VGA_CYAN 3
#define VGA_RED 4
#define VGA_MAGENTA 5
#define VGA_BROWN 6
#define VGA_LIGHT_GRAY 7

#define VGA_DARK_GRAY 8
#define VGA_LIGHT_BLUE 9
#define VGA_LIGHT_GREEN 10
#define VGA_LIGHT_CYAN 11
#define VGA_LIGHT_RED 12
#define VGA_LIGHT_MAGENTA 13
#define VGA_YELLOW 14
#define VGA_WHITE 15

#define CHAR_WIDTH 8
#if KERNEL_FONT == 0x03
#define CHAR_HEIGHT 8
#elif KERNEL_FONT == 0x06
#define CHAR_HEIGHT 16
#else
#error "Invalid font"
#endif

extern U16 bios_font_segment;
extern U16 bios_font_offset;
extern U8 *font;
extern int terminal_width;
extern int terminal_height;

void terminal_draw_framebuffer(void);
void terminal_init(void);
void terminal_set_color(U8 fg, U8 bg);
void terminal_putchar_at(char c, int x, int y);
void terminal_putchar(char c);
void terminal_putstring(char *s);
void terminal_clear(void);
void terminal_set_cursor_position(int x, int y);

#endif // TERMINAL_H
