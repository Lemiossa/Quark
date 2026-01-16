/**
 * pmm.c
 * Criado por Matheus Leme Da Silva
 */
#include <bitmap.h>
#include <e820.h>
#include <pmm.h>
#include <stddef.h>
#include <stdint.h>

U32 page_bitmap[32768] = {0};

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
      U32 start_page = e820_table[i].base / PAGE_SIZE;
      U32 pages_to_set = e820_table[i].length / PAGE_SIZE;
      for (U32 j = 0; j < pages_to_set; j++) {
        U32_bitmap_set(page_bitmap, start_page + j);
      }
    }
  }

  // BSS fica >1MiB
  U32 start_page = (U32)(&bss_start) / PAGE_SIZE;
  U32 pages_to_set = ((U32)&bss_end - (U32)&bss_start) / PAGE_SIZE;
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
        return (void *)(UPTR)(bit * PAGE_SIZE);
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
  U32_bitmap_clear(page_bitmap, (U32)page / PAGE_SIZE);
}
