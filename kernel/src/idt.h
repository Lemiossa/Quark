/**
 * idt.h
 * Criado por Matheus Leme Da Silva
 */
#pragma once
#include "stdint.h"

struct regs {
  u32 ds;
  u32 edi, esi, ebp, esp, ebx, edx, ecx, eax;
  u32 int_no, err_code;
  u32 eip, cs, eflags, usersp, ss;
} __attribute__((packed));

void idt_init(void);
