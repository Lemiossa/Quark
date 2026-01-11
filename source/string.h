/**
 * string.h
 * Criado por Matheus Leme Da Silva
 */
#ifndef STRING_H
#define STRING_H
#include "stdint.h"

void *memcpy(void *d, const void *s, u32 n);
int int_to_ascii(u64 n, int base, int is_neg, int is_upper, int pad,
                 int zero_pad, char *d);

#endif // STRING_H
