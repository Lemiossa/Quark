/**
 * string.c
 * Criado por Matheus Leme Da Silva
 */
#include <stddef.h>
#include <stdint.h>
#include <string.h>

// Pega o tamanho de uma string até \0
int strlen(char *s) {
  if (!s)
    return 0;
  int len = 0;
  while (*s++)
    len++;
  return len;
}

// Copia de S até \0 para D
void strcpy(char *d, const char *s) {
  if (!d || !s)
    return;

  U8 *dest = (U8 *)d;
  U8 *src = (U8 *)s;
  while (*src) {
    *dest++ = *src++;
  }
}

// Retorna 1 se S1 == S2, compara até \0
int strcmp(const char *s1, const char *s2) {
  if (!s1 || !s2)
    return 0;

  while (*s1 && *s2) {
    if (*s1++ != *s2++)
      return 0;
  }
  return 1;
}

// Copia N bytes de S para D
void *memcpy(void *d, const void *s, U32 n) {
  if (!d || !s)
    return NULL;
  if (n == 0)
    return d;
  U8 *dest = (U8 *)d;
  const U8 *src = (const U8 *)s;

  for (U32 i = 0; i < n; i++) {
    dest[i] = src[i];
  }

  return (void *)dest;
}

// Coloca N bytes com C valor em D
void memset(void *d, U8 c, U32 n) {
  if (!d || n == 0)
    return;
  U8 *dest = (U8 *)d;
  for (U32 i = 0; i < n; i++) {
    dest[i] = c;
  }
}

// Converte N para ASCII em D usando BASE e retorna o numero de caracteres
// escritos
int int_to_ascii(U64 n, int base, int is_neg, int is_upper, int pad,
                 int zero_pad, int neg_pad, char *d) {
  if (!d || base < 2 || base > 36)
    return 0;

  if (n == 0) {
    int di = 0;

    if (is_neg)
      d[di++] = '-';

    if (!neg_pad) {
      for (int i = 0; i < pad - 1; i++) {
        d[di++] = zero_pad ? '0' : ' ';
      }
    }

    d[di++] = '0';

    if (neg_pad) {
      for (int i = 0; i < pad - 1; i++) {
        d[di++] = ' ';
      }
    }

    d[di] = 0;

    return di;
  }

  const char *digits = !is_upper ? "0123456789abcdefghijklmnopqrstuvwxyz"
                                 : "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ";

  char tmp[65];
  int len = 0;
  while (n > 0) {
    tmp[len++] = digits[n % base];
    n /= base;
  }

  tmp[len] = 0;

  int di = 0;

  if (is_neg)
    d[di++] = '-';

  int num_len = len;

  if (!neg_pad) {
    for (int i = num_len + is_neg; i < pad; i++) {
      d[di++] = zero_pad ? '0' : ' ';
    }
  }

  while (len > 0)
    d[di++] = tmp[--len];

  if (neg_pad) {
    for (int i = num_len + is_neg; i < pad; i++) {
      d[di++] = ' ';
    }
  }

  d[di] = 0;

  return di;
}
