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

extern char bss_end;

// Func principal do kernel
void kmain(void) {
  terminal_init();
  terminal_set_color(VGA_LIGHT_GRAY, VGA_BLACK);
  terminal_clear();
#ifdef KERNEL_VERBOSE_MODE
  printk("Terminal initialized!\n");
#endif

  idt_init();
#ifdef KERNEL_VERBOSE_MODE
  printk("IDT initialized\n");
#endif
  pic_remap();
#ifdef KERNEL_VERBOSE_MODE
  printk("PIC has been remapped\n");
#endif

  memcpy(e820_table, (void *)KERNEL_E820_LOC,
         e820_table_count * sizeof(struct e820_entry));

  if (e820_table_count > 0) {
    printk("E820: Found %hhu entries\n", e820_table_count);
#ifdef KERNEL_VERBOSE_MODE
    printk("            BASE |           LENGTH | TYPE\n");
    for (u8 i = 0; i < e820_table_count; i++) {
      char *type;
      switch (e820_table[i].type) {
      case 1:
        type = "Usable";
        break;
      case 2:
        type = "Reserved";
        break;
      case 3:
        type = "ACPI reclaimable";
        break;
      case 4:
        type = "ACPI NVS";
        break;
      case 5:
        type = "Bad";
        break;
      default:
        type = "Unknown";
        break;
      }
      printk("%016llx | %016llx | %s\n", e820_table[i].base,
             e820_table[i].length, type);
    }
#endif
  }

  timer_init();
#ifdef KERNEL_VERBOSE_MODE
  printk("Timer initialized! Frequency=%dHZ\n", KERNEL_PIT_FREQ);
#endif

  __asm__ volatile("sti");

  printk("\n");

  printk("Hello World\n");

  for (;;) {
    __asm__ volatile("hlt");
  }
}
