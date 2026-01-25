/**
 * Io.h
 * Created by Matheus Leme Da Silva
 */
#ifndef IO_H
#define IO_H
#include "Types.h"

static inline void OutU8(U16 p, U8 b) {
  __asm__ volatile("outb %b0, %w1" ::"a"(b), "Nd"(p) : "memory");
}

static inline U8 InU8(unsigned short port) {
  U8 ret;
  __asm__ volatile("inb %1, %0" : "=a"(ret) : "Nd"(port));
  return ret;
}

#endif // IO_H
