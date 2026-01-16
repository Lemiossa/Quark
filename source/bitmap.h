/**
 * bitmap.h
 * Criado por Matheus Leme Da Silva
 */
#ifndef BITMAP_H
#define BITMAP_H
#include <stdint.h>

inline static void U32_bitmap_set(U32 *d, U32 b) {
  d[b >> 5] |= (1u << (b & 31));
}
inline static void U32_bitmap_clear(U32 *d, U32 b) {
  d[b >> 5] &= ~(1u << (b & 31));
}
inline static int U32_bitmap_test(U32 *d, U32 b) {
  return d[b >> 5] & (1u << (b & 31));
}

#endif // BITMAP_H
