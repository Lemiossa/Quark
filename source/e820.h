/**
 * e820.h
 * Criado por Matheus Leme Da Silva
 */
#ifndef E820_H
#define E820_H
#include <stdint.h>

struct e820_entry {
  U64 base;
  U64 length;
  U32 type;
  U32 attributes;
} __attribute__((packed));

extern struct e820_entry e820_table[128];
extern U8 e820_table_count;

void e820_init(void);

#endif // E820_h
