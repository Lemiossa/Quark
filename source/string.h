/**
 * string.h
 * Criado por Matheus Leme Da Silva
 */
#ifndef STRING_H
#define STRING_H
#include <stdint.h>

int strlen(char *s);
void strcpy(char *d, const char *s);
int strcmp(const char *s1, const char *s2);
void *memcpy(void *d, const void *s, U32 n);
void memset(void *d, U8 c, U32 n);
int int_to_ascii(U64 n, int base, int is_neg, int is_upper, int pad,
                 int zero_pad, int neg_pad, char *d);

#endif // STRING_H
