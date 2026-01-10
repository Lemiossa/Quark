/**
 * panic.c
 * Criado por Matheus Leme Da Silva
 */
#include "panic.h"
#include "stdint.h"
#include "terminal.h"

const char *hex_chars = "0123456789abcdef";

// Imprime um byte hexadecimal
void print_hex_byte(u8 value) {
  terminal_putchar(hex_chars[(value >> 4) & 0xf]);
  terminal_putchar(hex_chars[value & 0xf]);
}

// Imprime um word hexadecimal
void print_hex_word(u16 value) {
  print_hex_byte((value >> 8) & 0xff);
  print_hex_byte(value & 0xff);
}

// Imprime um dword hexadecimal
void print_hex_dword(u32 value) {
  print_hex_word((value >> 16) & 0xffff);
  print_hex_word(value & 0xffff);
}

// Imprime um qword hexadecimal
void print_hex_qword(u64 value) {
  print_hex_word((value >> 48) & 0xffff);
  print_hex_word((value >> 32) & 0xffff);
  print_hex_word((value >> 16) & 0xffff);
  print_hex_word(value & 0xffff);
}

// Dá um panic no kernel
void panic(char *str, struct regs regs) {
  terminal_putstring("Panic! \n");

  terminal_putstring("Message: ");
  terminal_putstring(str);

  terminal_putstring("\n\n");

  terminal_putstring("eax = ");
  print_hex_dword(regs.eax);
  terminal_putchar(' ');
  terminal_putstring("esi = ");
  print_hex_dword(regs.esi);
  terminal_putchar('\n');

  terminal_putstring("ebx = ");
  print_hex_dword(regs.ebx);
  terminal_putchar(' ');
  terminal_putstring("edi = ");
  print_hex_dword(regs.edi);
  terminal_putchar('\n');

  terminal_putstring("ecx = ");
  print_hex_dword(regs.ecx);
  terminal_putchar(' ');
  terminal_putstring("ebp = ");
  print_hex_dword(regs.ebp);
  terminal_putchar('\n');

  terminal_putstring("edx = ");
  print_hex_dword(regs.edx);
  terminal_putchar(' ');
  terminal_putstring("esp = ");
  print_hex_dword(regs.esp);
  terminal_putchar('\n');

  terminal_putstring("ds  = ");
  print_hex_dword(regs.ds);
  terminal_putchar(' ');
  terminal_putstring("cs  = ");
  print_hex_dword(regs.cs);
  terminal_putchar('\n');

  terminal_putstring("eip = ");
  print_hex_dword(regs.eip);
  terminal_putchar(' ');
  terminal_putstring("eflags = ");
  print_hex_dword(regs.eflags);
  terminal_putchar('\n');

  terminal_putchar('\n');
  terminal_putstring("System halted! please, reboot\n");

  terminal_draw_framebuffer();

  for (;;)
    __asm__ volatile("cli; hlt");
}
