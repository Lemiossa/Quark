/**
 * kmain.c
 * Criado por Matheus Leme Da Silva
 */
#include <defs.h>
#include <ints/idt.h>
#include <ints/pic.h>
#include <io.h>
#include <printk.h>
#include <string.h>
#include <terminal.h>
#include <timer.h>

#ifndef VERSION
#define VERSION "UNDEFINED"
#endif

struct e820_entry {
  U64 base;
  U64 length;
  U32 type;
  U32 attributes;
} __attribute__((packed));

struct e820_entry e820_table[128];
extern U8 e820_table_count;

U64 total_ram = 0;
U64 total_usable_ram = 0;

U32 page_bitmap[32768] = {0};

void U32_bitmap_set(U32 *d, U32 b) { d[b >> 5] |= (1u << (b & 31)); }
void U32_bitmap_clear(U32 *d, U32 b) { d[b >> 5] &= ~(1u << (b & 31)); }
int U32_bitmap_test(U32 *d, U32 b) { return d[b >> 5] & (1u << (b & 31)); }

extern U8 bss_start;
extern U8 bss_end;

// Inicializa PMM
// NOTE: Assume que todos endereços e tamanho das entradas E820 estão alinhadas
// com 4K, pode dar problemas em alguns PCs filha-da-puta
void pmm_init(void) {
  for (U32 i = 0; i < 256; i++) {
    U32_bitmap_set(page_bitmap, i);
  }

  for (U8 i = 0; i < e820_table_count; i++) {
    if (e820_table[i].base > 0xfffffffful ||
        e820_table[i].length > 0xfffffffful || e820_table[i].base < 0x1000000)
      continue;

    if (e820_table[i].type != 1) {
      U32 start_page = e820_table[i].base / 4096;
      U32 pages_to_set = e820_table[i].length / 4096;
      for (U32 j = 0; j < pages_to_set; j++) {
        U32_bitmap_set(page_bitmap, start_page + j);
      }
    }
  }

  // BSS fica >1MiB
  U32 start_page = (U32)(&bss_start) / 4096;
  U32 pages_to_set = ((U32)&bss_end - (U32)&bss_start) / 4096;
  for (U32 j = 0; j < pages_to_set; j++) {
    U32_bitmap_set(page_bitmap, start_page + j);
  }
}

// Aloca uma página
void *pmm_alloc_page(void) {
  U32 bit = 0;
  for (U32 i = 0; i < 32768; i++) {
    if (page_bitmap[i] == 0xffffffff) {
      bit += 32;
      continue;
    }
    for (U32 j = 0; j < 32; j++) {
      if (!U32_bitmap_test(page_bitmap, bit)) {
        U32_bitmap_set(page_bitmap, bit);
        return (void *)(bit * 4096);
      }
      bit++;
    }
  }
  return NULL;
}

// Libera uma pagina
// NOTE: Não libera paginas abaixo de 1MiB
void pmm_free_page(void *page) {
  if ((U32)page < 0x100000)
    return;
  U32_bitmap_clear(page_bitmap, (U32)page / 4096);
}

char *e820_type_strings[5] = {"usable", "reserved", "ACPI reclaimable",
                              "ACPI NVS", "bad"};

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
  printk("IDT initialized!\n");
#endif
  pic_remap();
#ifdef KERNEL_VERBOSE_MODE
  printk("PIC has been remapped\n");
#endif

  memcpy(e820_table, (void *)KERNEL_E820_LOC,
         e820_table_count * sizeof(struct e820_entry));

  if (e820_table_count > 0) {
#ifdef KERNEL_VERBOSE_MODE
    printk("E820: Found %hhu entries\n", e820_table_count);
#endif
    for (U8 i = 0; i < e820_table_count; i++) {
      char *type = e820_type_strings[e820_table[i].type - 1];
      total_ram += e820_table[i].length;

      if (e820_table[i].type == 1)
        total_usable_ram += e820_table[i].length;
#ifdef KERNEL_VERBOSE_MODE
      printk("0x%016llx-0x%016llx %s\n", e820_table[i].base,
             e820_table[i].length, type);
#endif
    }
    printk("Total RAM: %llu, usable: %llu\n", total_ram, total_usable_ram);
  }

  pmm_init();
#ifdef KERNEL_VERBOSE_MODE
  printk("PMM initialized!\n");
#endif

  timer_init();
#ifdef KERNEL_VERBOSE_MODE
  printk("Timer initialized! Frequency=%dHZ\n", KERNEL_PIT_FREQ);
#endif

  __asm__ volatile("sti");

  printk("Quark v%s kernel\n\n", VERSION);

  for (;;) {
    __asm__ volatile("hlt");
  }
}
