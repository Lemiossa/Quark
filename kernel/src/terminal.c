/**
 * terminal.c
 * Criado por Matheus Leme Da Silva
 */
#include "string.h"

#include "ports.h"
#include "stdint.h"
#include "terminal.h"

struct vga_entry {
	u8 ch;
	u8 cl;
} __attribute__((packed));

typedef struct vga_entry vga_entry_t;
volatile vga_entry_t *vga_buffer = (volatile vga_entry_t *)0xb8000;

int cursor_x_position = 0;
int cursor_y_position = 0;
u8 current_color = 0x07;

// Muda a posição do cursor visual do terminal
void terminal_set_cursor_visual_position(int x, int y) {
	if (x > TERMINAL_WIDTH || y > TERMINAL_HEIGTH)
		return;
	int pos = y * TERMINAL_WIDTH + x;

	outb(0x3d4, 0xf);
	outb(0x3d5, (u8)(pos & 0xff));
	outb(0x3d4, 0xe);
	outb(0x3d5, (u8)((pos >> 8) & 0xff));
}

// Muda a posição do cursor do terminal
void terminal_set_cursor_position(int x, int y) {
	if (x > TERMINAL_WIDTH || y > TERMINAL_HEIGTH)
		return;
	cursor_x_position = x;
	cursor_y_position = y;
	terminal_set_cursor_visual_position(x, y);
}

// Seta a cor atual do VGA
void terminal_set_color(u8 fg, u8 bg) { current_color = ((bg << 4) | fg); }

// Coloca um caractere com a cor atual em uma posição específica da tela
void terminal_putchar_at(char c, int x, int y) {
	if (x > TERMINAL_WIDTH || y > TERMINAL_HEIGTH)
		return;
	int pos = y * TERMINAL_WIDTH + x;
	vga_buffer[pos].ch = c;
	vga_buffer[pos].cl = current_color;
}

// Imprime um caractere na pos atual
void terminal_putchar(char c) {
	if (c == '\r') {
		cursor_x_position = 0;
		terminal_set_cursor_visual_position(cursor_x_position,
											cursor_y_position);
		return;
	} else if (c == '\n') {
		cursor_y_position++;
		cursor_x_position = 0;
		terminal_set_cursor_visual_position(cursor_x_position,
											cursor_y_position);
		return;
	}

	terminal_putchar_at(c, cursor_x_position, cursor_y_position);
	cursor_x_position++;
	if (cursor_x_position >= TERMINAL_WIDTH) {
		cursor_x_position = 0;
		cursor_y_position++;
	}

	// scroll
	if (cursor_y_position >= TERMINAL_HEIGTH) {
		memcpy((void *)vga_buffer, (void *)(vga_buffer + TERMINAL_WIDTH),
			   (TERMINAL_HEIGTH - 1) * TERMINAL_WIDTH * 2);
		cursor_y_position = TERMINAL_HEIGTH - 1;
		cursor_x_position = 0;

		for (int x = 0; x < TERMINAL_WIDTH; x++) {
			terminal_putchar_at(' ', x, TERMINAL_HEIGTH - 1);
		}
	}

	terminal_set_cursor_visual_position(cursor_x_position, cursor_y_position);
}

// Imprime uma string no terminal
void terminal_putstring(char *s) {
	while (*s) {
		terminal_putchar(*s++);
	}
}

// Limpa o terminal
void terminal_clear(void) {
	for (int y = 0; y < TERMINAL_HEIGTH; y++) {
		for (int x = 0; x < TERMINAL_WIDTH; x++) {
			terminal_putchar_at(' ', x, y);
		}
	}
	cursor_x_position = 0;
	cursor_y_position = 0;
	terminal_set_cursor_visual_position(0, 0);
}
