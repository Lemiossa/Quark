/**
 * Gdt.c
 * Created by Matheus Leme Da Silva
 */
#include "Gdt.h"
#include "Types.h"

struct GdtEnt Gdt[GDT_ENTRIES];
struct Gdtr Gdtr;

// Set a GDT entry
void GdtSetEnt(U16 ent, U32 limit, U32 base, U8 access, U8 flags) {
  if (ent >= GDT_ENTRIES)
    return;

  limit = limit & 0xFFFFF;

  Gdt[ent].LimitLo = limit & 0xFFFF;
  Gdt[ent].BaseLo = base & 0xFFFF;
  Gdt[ent].BaseMi = (base >> 16) & 0xFF;
  Gdt[ent].Access = access;
  Gdt[ent].Flags = ((flags & 0xF) << 4) | (limit >> 16);
  Gdt[ent].BaseHi = (base >> 24) & 0xFF;
}

// Initializes GDT
void GdtInit(void) {
  GdtSetEnt(0, 0, 0, 0, 0);
  GdtSetEnt(1, 0xFFFFF, 0, 0b10011010, 0b1100);
  GdtSetEnt(2, 0xFFFFF, 0, 0b10010010, 0b1100);

  __asm__ volatile("lgdt %0" ::"m"(Gdtr));
}
