/**
 * e820.c
 * Criado por Matheus Leme Da Silva
 */
#include <defs.h>
#include <e820.h>
#include <printk.h>
#include <string.h>

struct e820_entry e820_table[128];

extern U64 total_ram;
extern U64 total_usable_ram;

char *e820_type_strings[5] = {"usable", "reserved", "ACPI reclaimable",
                              "ACPI NVS", "bad"};

// Inicializa E820, no caso só copia pra um buffer local
void e820_init(void) {
  memcpy(e820_table, (void *)KERNEL_E820_LOC,
         e820_table_count * sizeof(struct e820_entry));

  if (e820_table_count > 0) {
#ifdef KERNEL_VERBOSE_MODE
    printk("E820: Found %hhu entries\n", e820_table_count);
#endif
    for (U8 i = 0; i < e820_table_count; i++) {
      total_ram += e820_table[i].length;

      if (e820_table[i].type == 1)
        total_usable_ram += e820_table[i].length;
#ifdef KERNEL_VERBOSE_MODE
      char *type = e820_type_strings[e820_table[i].type - 1];
      printk("0x%016llx-0x%016llx %s\n", e820_table[i].base,
             e820_table[i].length, type);
#endif
    }
  }
}
