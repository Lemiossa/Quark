/**
 * idt.h
 * Criado por Matheus Leme Da Silva
 */
#ifndef IDT_H
#define IDT_H
#include <stdint.h>

struct regs {
  U32 gs, fs, es, ds;
  U32 edi, esi, ebp, esp, ebx, edx, ecx, eax;
  U32 int_no, err_code;
  U32 eip, cs, eflags, usersp, ss;
} __attribute__((packed));

void idt_init(void);

#endif // IDT_H
