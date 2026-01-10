/**
 * kmain.c
 * Criado por Matheus Leme Da Silva
 */
#include "idt.h"
#include "kdefs.h"
#include "pic.h"
#include "stdarg.h"
#include "string.h"
#include "terminal.h"

int vsprintf(char *s, const char *format, va_list args) {}

static const char *hex_chars = "0123456789abcdef";

// Imprime um byte hexadecimal
static void print_hex_byte(u8 value) {
  terminal_putchar(hex_chars[(value >> 4) & 0xf]);
  terminal_putchar(hex_chars[value & 0xf]);
}

// Imprime um word hexadecimal
static void print_hex_word(u16 value) {
  print_hex_byte((value >> 8) & 0xff);
  print_hex_byte(value & 0xff);
}

// Imprime um dword hexadecimal
static void print_hex_dword(u32 value) {
  print_hex_word((value >> 16) & 0xffff);
  print_hex_word(value & 0xffff);
}

// Imprime um qword hexadecimal
static void print_hex_qword(u64 value) {
  print_hex_word((value >> 48) & 0xffff);
  print_hex_word((value >> 32) & 0xffff);
  print_hex_word((value >> 16) & 0xffff);
  print_hex_word(value & 0xffff);
}

struct e820_entry {
  u64 base;
  u64 length;
  u32 type;
  u32 attributes;
} __attribute__((packed));

struct e820_entry e820_table[128];
extern u8 e820_table_count;

// Inicializa o e820 copiando para um buffer em BSS
void e820_init(void) {
  memcpy(e820_table, (void *)E820_LOC,
         e820_table_count * sizeof(struct e820_entry));
}

// Func principal do kernel
void kmain(void) {
  idt_init();
  pic_remap();

  e820_init();

  terminal_init();
  terminal_set_color(VGA_WHITE, VGA_BLACK);
  terminal_clear();

  for (u8 i = 0; i < e820_table_count; i++) {
    terminal_putstring("Entry ");
    print_hex_byte(i);
    terminal_putstring(": \n");

    terminal_putstring("   Base=");
    print_hex_qword(e820_table[i].base);
    terminal_putstring("\n");

    terminal_putstring("   Length=");
    print_hex_qword(e820_table[i].length);
    terminal_putstring("\n");

    terminal_putstring("   Type=");
    print_hex_dword(e820_table[i].type);
    terminal_putstring("\n");

    terminal_putstring("   Attributes=");
    print_hex_dword(e820_table[i].attributes);
    terminal_putstring("\n");
  }

  __asm__ volatile("sti");

  __asm__ volatile("int $3");

  for (;;)
    __asm__ volatile("hlt");
}
