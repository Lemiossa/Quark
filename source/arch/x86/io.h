/**
 * io.h
 * Criado por Matheus Leme Da Silva
 */
#ifndef PORTS_H
#define PORTS_H
#include <stdint.h>

// Envia val para port
static inline void outb(U16 port, U8 val) {
  __asm__ volatile("outb %b0, %w1;" ::"a"(val), "d"(port) :);
}

// Lê dados de uma port
static inline U8 inb(U16 port) {
  U8 ret;
  __asm__ volatile("inb %w1, %b0;" : "=a"(ret) : "d"(port) :);
  return ret;
}

static inline void io_wait(void) { outb(0x80, 0); }

#endif // PORTS_H
