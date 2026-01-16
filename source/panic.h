/**
 * panic.h
 * Criado por Matheus Leme Da Silva
 */
#ifndef PANIC_H
#define PANIC_H
#include <ints/idt.h>
#include <stdint.h>

void panic(char *str, struct regs *r);

#endif // PANIC_H
