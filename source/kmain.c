/**
 * kmain.c
 * Criado por Matheus Leme Da Silva
 */
#include "idt.h"
#include "kdefs.h"
#include "pic.h"
#include "printk.h"
#include "string.h"
#include "terminal.h"
#include "timer.h"

struct e820_entry {
  u64 base;
  u64 length;
  u32 type;
  u32 attributes;
} __attribute__((packed));

struct e820_entry e820_table[128];
extern u8 e820_table_count;

// Func principal do kernel
void kmain(void) {
  idt_init();
  pic_remap();

  memcpy(e820_table, (void *)E820_LOC,
         e820_table_count * sizeof(struct e820_entry));

  terminal_init();
  terminal_set_color(VGA_WHITE, VGA_BLACK);
  terminal_clear();

  timer_init();

  __asm__ volatile("sti");

  for (;;) {
    __asm__ volatile("hlt");
  }
}
