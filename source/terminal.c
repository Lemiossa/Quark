/**
 * terminal.c
 * Criado por Matheus Leme Da Silva
 */
#include <defs.h>
#include <graphics.h>
#include <io.h>
#include <stdint.h>
#include <string.h>
#include <terminal.h>
#include <timer.h>

struct char_entry {
  U8 ch;
  U8 cl;
} __attribute__((packed));

extern U16 bios_font_segment;
extern U16 bios_font_offset;
U8 *font = NULL;

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
U8 current_color = 0x07;

enum {
  ESCAPE,
  CSI,
  NORMAL,
} terminal_mode;

// Desenha um caractere no modo grafico
void terminal_draw_char(int x, int y, char c, U8 fg, U8 bg) {
#ifdef KERNEL_VESA_MODE
  if (!font)
    return;
  if (vbe_supported) {
    U8 uc = (U8)c;
    U8 *glyph = font + (uc * CHAR_HEIGHT);
    for (int i = 0; i < CHAR_HEIGHT; i++) {
      U8 line = glyph[i];
      for (int j = 0; j < CHAR_WIDTH; j++) {
        if (line & (0x80 >> j)) {
          put_pixel((x * CHAR_WIDTH) + j, (y * CHAR_HEIGHT) + i, fg);
        } else {
          put_pixel((x * CHAR_WIDTH) + j, (y * CHAR_HEIGHT) + i, bg);
        }
      }
    }
    return;
  }
#endif
  struct char_entry *vga = (struct char_entry *)VGA_MEM;
  vga[y * terminal_width + x].ch = c;
  vga[y * terminal_width + x].cl = ((bg & 0xf) << 4) | (fg & 0xf);
}

// desenha o cursor
void terminal_draw_cursor(void) {
  if (cursor_x > terminal_width || cursor_y > terminal_height)
    return;

  if (!vbe_supported) {
    int pos = cursor_y * terminal_width + cursor_x;

    outb(0x3d4, 0xf);
    outb(0x3d5, (U8)(pos & 0xff));
    outb(0x3d4, 0xe);
    outb(0x3d5, (U8)(pos >> 8));
    return;
  }

  struct char_entry c = buffer[cursor_y * terminal_width + cursor_x];
  if (cursor)
    terminal_draw_char(cursor_x, cursor_y, c.ch, current_color >> 4,
                       current_color & 0xf);
  else
    terminal_draw_char(cursor_x, cursor_y, c.ch, current_color & 0xf,
                       current_color >> 4);
}

// Desenha todo o terminal
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

// Muda a posição do cursor do terminal
void terminal_set_cursor_position(int x, int y) {
  if (x > terminal_width || y > terminal_height)
    return;
  cursor_x = x;
  cursor_y = y;
}

// Seta a cor atual do VGA
void terminal_set_color(U8 fg, U8 bg) { current_color = ((bg << 4) | fg); }

// Coloca um caractere com a cor atual em uma posição específica da tela
void terminal_putchar_at(char c, int x, int y) {
  if (x < 0 || y < 0 || x >= terminal_width || y >= terminal_height)
    return;
  int pos = y * terminal_width + x;
  buffer[pos].ch = c;
  buffer[pos].cl = current_color;
  terminal_draw_char(x, y, buffer[pos].ch, buffer[pos].cl & 0xf,
                     buffer[pos].cl >> 4);
}

// Imprime um caractere na pos atual
void terminal_putchar(char c) {
  switch (c) {
  case '\r': {
    terminal_putchar_at(buffer[cursor_y * terminal_width + cursor_x].ch,
                        cursor_x, cursor_y);
    cursor_x = 0;
    cursor = 1;
    terminal_draw_cursor();
  } break;
  case '\n': {
    terminal_putchar_at(buffer[cursor_y * terminal_width + cursor_x].ch,
                        cursor_x, cursor_y);

    cursor_y++;
    cursor_x = 0;
    cursor = 1;
    terminal_draw_cursor();
  } break;
  case '\b': {
    terminal_putchar_at(buffer[cursor_y * terminal_width + cursor_x].ch,
                        cursor_x, cursor_y);

    if (cursor_x == 0)
      return;

    cursor_x--;

    terminal_putchar_at(' ', cursor_x, cursor_y);
    cursor = 1;
    terminal_draw_cursor();

  } break;
  case '\t': {
    for (int i = 0; i < 4; i++) {
      terminal_putchar(' ');
    }
  } break;
  case '\x1b': {
    terminal_mode = ESCAPE;
    return;
  } break;
  default: {
    if (terminal_mode == ESCAPE) {
      if (c == '[') {
        terminal_mode = CSI;
        return;
      }
    } else if (terminal_mode == CSI) {
      if (c == 'D') {
        if (cursor_x > 0)
          cursor_x--;
      } else if (c == 'C') {
        if (cursor_x >= terminal_width) {
          cursor_x = 0;
          cursor_y++;
        } else
          cursor_x++;
      } else if (c == 'A') {
        if (cursor_y > 0)
          cursor_y--;
      } else if (c == 'B') {
        cursor_y++;
      }
      goto after_print;
    }
    terminal_mode = NORMAL;
    terminal_putchar_at(c, cursor_x, cursor_y);
    cursor_x++;
    if (cursor_x >= terminal_width) {
      cursor_x = 0;
      cursor_y++;
    }
    cursor = 1;
    terminal_draw_cursor();

  } break;
  }
after_print:

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
#ifdef KERNEL_VESA_MODE
  if (vbe_supported) {
    terminal_width = VIDEO_MODE_WIDTH / CHAR_WIDTH;
    terminal_height = VIDEO_MODE_HEIGHT / CHAR_HEIGHT;
    U32 font_address = (bios_font_segment << 4) + bios_font_offset;
    font = (U8 *)font_address;
  }
#endif

  cursor_x = 0;
  cursor_y = 0;
  cursor = 1;
  terminal_draw_cursor();
}

#ifndef KERNEL_CURSOR_BLINK_MS
#error "Need KERNEL_CURSOR_BLINK_MS"
#endif

U16 volatile terminal_ticks = 0;

// Tick do terminal
void terminal_tick(void) {
  terminal_ticks++;
  if (terminal_ticks >= MS_TO_TICK(KERNEL_CURSOR_BLINK_MS) && vbe_supported) {
    terminal_ticks = 0;
    terminal_draw_cursor();
    cursor = !cursor;
  }
}
