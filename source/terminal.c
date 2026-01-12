/**
 * terminal.c
 * Criado por Matheus Leme Da Silva
 */
#include "terminal.h"
#include "graphics.h"
#include "kdefs.h"
#include "ports.h"
#include "stdint.h"
#include "string.h"
#include "timer.h"

struct char_entry {
  u8 ch;
  u8 cl;
} __attribute__((packed));

extern u16 bios_font_segment;
extern u16 bios_font_offset;
u8 *font = NULL;

int terminal_width = 80;
int terminal_height = 50;

#define VGA_MEM 0xb8000

#define TERMINAL_MAX_WIDTH 160
#define TERMINAL_MAX_HEIGHT 80

volatile struct char_entry buffer[TERMINAL_MAX_HEIGHT * TERMINAL_MAX_WIDTH] = {
    0};

int cursor_x = 0;
int cursor_y = 0;
int cursor = 1;
u8 current_color = 0x07;

// Desenha um caractere no modo grafico
void terminal_draw_char(int x, int y, char c, u8 fg, u8 bg) {
  if (!font)
    return;
  //
  u8 uc = (u8)c;
  u8 *glyph = font + (uc * CHAR_HEIGHT);
  for (int i = 0; i < CHAR_HEIGHT; i++) {
    u8 line = glyph[i];
    for (int j = 0; j < CHAR_WIDTH; j++) {
      if (line & (0x80 >> j)) {
        put_pixel((x * CHAR_WIDTH) + j, (y * CHAR_HEIGHT) + i, fg);
      } else {
        put_pixel((x * CHAR_WIDTH) + j, (y * CHAR_HEIGHT) + i, bg);
      }
    }
  }
}

// desenha o cursor
void terminal_draw_cursor(void) {
  if (cursor_x > terminal_width || cursor_y > terminal_height || !vbe_supported)
    return;

  for (int i = 0; i < CHAR_HEIGHT; i++) {
    u8 line = i >= CHAR_HEIGHT - 2 ? 0xff : 0x00;
    for (int j = 0; j < CHAR_WIDTH; j++) {
      if (line & (0x80 >> j)) {
        put_pixel((cursor_x * CHAR_WIDTH) + j, (cursor_y * CHAR_HEIGHT) + i,
                  cursor ? current_color & 0xf : (current_color >> 4) & 0xf);
      }
    }
  }
}

// Muda a posição do cursor do terminal
void terminal_set_cursor_position(int x, int y) {
  if (x > terminal_width || y > terminal_height)
    return;
  cursor_x = x;
  cursor_y = y;

  int pos = cursor_y * terminal_width + cursor_x;

  outb(0x3d4, 0xf);
  outb(0x3d5, (u8)(pos & 0xff));
  outb(0x3d4, 0xe);
  outb(0x3d5, (u8)((pos >> 8) & 0xff));
}

// Desenha todo o terminal caso seja grafico
void terminal_draw_framebuffer(void) {
  if (vbe_supported) {
    for (int y = 0; y < terminal_height; y++) {
      for (int x = 0; x < terminal_width; x++) {
        int pos = y * terminal_width + x;
        terminal_draw_char(x, y, buffer[pos].ch, buffer[pos].cl & 0xf,
                           (buffer[pos].cl >> 4) & 0xf);
      }
    }
  } else {
    memcpy((void *)VGA_MEM, (void *)buffer,
           terminal_height * terminal_width * 2);
  }
}

// Seta a cor atual do VGA
void terminal_set_color(u8 fg, u8 bg) { current_color = ((bg << 4) | fg); }

// Coloca um caractere com a cor atual em uma posição específica da tela
void terminal_putchar_at(char c, int x, int y) {
  if (x < 0 || y < 0 || x >= terminal_width || y >= terminal_height)
    return;
  int pos = y * terminal_width + x;
  buffer[pos].ch = c;
  buffer[pos].cl = current_color;
  terminal_draw_char(x, y, buffer[pos].ch, buffer[pos].cl & 0xf,
                     (buffer[pos].cl >> 4) & 0xf);
}

// Imprime um caractere na pos atual
void terminal_putchar(char c) {
  if (c == '\r') {
    terminal_putchar_at(' ', cursor_x, cursor_y);
    cursor_x = 0;
    terminal_draw_cursor();
    return;
  } else if (c == '\n') {
    terminal_putchar_at(' ', cursor_x, cursor_y);
    cursor_y++;
    cursor_x = 0;
    terminal_draw_cursor();
    return;
  }

  terminal_putchar_at(c, cursor_x, cursor_y);
  cursor_x++;
  if (cursor_x >= terminal_width) {
    cursor_x = 0;
    cursor_y++;
  }

  // scroll
  if (cursor_y >= terminal_height) {
    memcpy((void *)buffer, (void *)(buffer + terminal_width),
           (terminal_height - 1) * terminal_width * 2);
    cursor_y = terminal_height - 1;
    cursor_x = 0;
    terminal_draw_framebuffer();

    for (int x = 0; x < terminal_width; x++) {
      terminal_putchar_at(' ', x, terminal_height - 1);
    }
  }
}

// Imprime uma string no terminal
void terminal_putstring(char *s) {
  while (*s) {
    terminal_putchar(*s++);
  }
}

// Limpa o terminal
void terminal_clear(void) {
  for (int y = 0; y < terminal_height; y++) {
    for (int x = 0; x < terminal_width; x++) {
      terminal_putchar_at(' ', x, y);
    }
  }

  cursor_x = 0;
  cursor_y = 0;
}

// Inicia o sistema de terminal
void terminal_init(void) {
  if (vbe_supported) {
    terminal_width = VIDEO_MODE_WIDTH / CHAR_WIDTH;
    terminal_height = VIDEO_MODE_HEIGHT / CHAR_HEIGHT;
    u32 font_address = (bios_font_segment << 4) + bios_font_offset;
    font = (u8 *)font_address;
  }

  cursor_x = 0;
  cursor_y = 0;
}

#ifndef CURSOR_BLINK_MS
#error "Need CURSOR_BLINK_MS"
#endif

u16 volatile terminal_ticks = 0;

// Tick do terminal
void terminal_tick(void) {
  terminal_ticks++;
  if (terminal_ticks >= MS_TO_TICK(CURSOR_BLINK_MS)) {
    terminal_ticks = 0;
    terminal_draw_cursor();
    cursor = !cursor;
  }
}
