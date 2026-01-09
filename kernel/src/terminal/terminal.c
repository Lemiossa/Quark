/**
 * terminal.c
 * Criado por Matheus Leme Da Silva
 */
#include "../string.h"
#include "../ports.h"
#include "../stdint.h"
#include "../terminal.h"
#include "../graphics.h"
#include "char.h"

struct vga_entry {
	u8 ch;
	u8 cl;
} __attribute__((packed));

extern u16 bios_font_segment;
extern u16 bios_font_offset;
u8 *font = NULL;

int terminal_width = 80;
int terminal_height = 50;

typedef struct vga_entry vga_entry_t;
volatile vga_entry_t *vga_buffer = (volatile vga_entry_t *)0xb8000;

int cursor_x_position = 0;
int cursor_y_position = 0;
u8 current_color = 0x07;

// Muda a posição do cursor visual do terminal
void terminal_set_cursor_visual_position(int x, int y) {
	if (x > terminal_width || y > terminal_height)
		return;
	int pos = y * terminal_width + x;

	outb(0x3d4, 0xf);
	outb(0x3d5, (u8)(pos & 0xff));
	outb(0x3d4, 0xe);
	outb(0x3d5, (u8)((pos >> 8) & 0xff));
}

// Muda a posição do cursor do terminal
void terminal_set_cursor_position(int x, int y) {
	if (x > terminal_width || y > terminal_height)
		return;
	cursor_x_position = x;
	cursor_y_position = y;
	terminal_set_cursor_visual_position(x, y);
}

// Seta a cor atual do VGA
void terminal_set_color(u8 fg, u8 bg) { current_color = ((bg << 4) | fg); }

// Coloca um caractere com a cor atual em uma posição específica da tela
void terminal_putchar_at(char c, int x, int y) {
	if (x < 0 || y < 0 || x >= terminal_width || y >= terminal_height)
		return;
	if (!vbe_supported) {
		int pos = y * terminal_width + x;
		vga_buffer[pos].ch = c;
		vga_buffer[pos].cl = current_color;
	} else {
		draw_char(x, y, c, current_color & 0xf, (current_color >> 4) & 0xf);
	}
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
	if (cursor_x_position >= terminal_width) {
		cursor_x_position = 0;
		cursor_y_position++;
	}

	// scroll
	if (cursor_y_position >= terminal_height) {
		memcpy((void *)vga_buffer, (void *)(vga_buffer + terminal_width),
			   (terminal_height - 1) * terminal_width * 2);
		cursor_y_position = terminal_height - 1;
		cursor_x_position = 0;

		for (int x = 0; x < terminal_width; x++) {
			terminal_putchar_at(' ', x, terminal_height - 1);
		}
	}

	if (vbe_supported) terminal_set_cursor_visual_position(cursor_x_position, cursor_y_position);
}

// Imprime uma string no terminal
void terminal_putstring(char *s) {
	while (*s) {
		terminal_putchar(*s++);
	}
}

// Limpa o terminal
void terminal_clear(void) {
	for (int y = 0; y < terminal_height; y++)
		for (int x = 0; x < terminal_width; x++)
			terminal_putchar_at(' ', x, y);
	cursor_x_position = 0;
	cursor_y_position = 0;
	terminal_set_cursor_visual_position(0, 0);
}

// Inicia o sistema de terminal
void terminal_init(void) {
	if (vbe_supported) {
		terminal_width = VIDEO_MODE_WIDTH / CHAR_WIDTH;
		terminal_height = VIDEO_MODE_HEIGHT / CHAR_HEIGHT;
		u32 font_address = (bios_font_segment << 4) + bios_font_offset;
		font = (u8 *)font_address;
	}
}
