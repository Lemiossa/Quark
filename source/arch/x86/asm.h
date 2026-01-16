/**
 * asm.h
 * Criado por Matheus Leme Da Silva
 */
#ifndef ASM_H
#define ASM_H

#define HLT() __asm__ volatile("hlt")
#define CLI() __asm__ volatile("cli")
#define STI() __asm__ volatile("sti")

#endif // ASM_H
