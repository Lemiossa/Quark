/**
 * string.c
 * Criado por Matheus Leme Da Silva
 */
#include <string.h>
#include <stddef.h>
#include <stdint.h>

// Pega o tamanho de uma string até \0
int strlen(char *s) {
  if (!s)
    return 0;
  int len = 0;
  while (*s++)
    len++;
  return len;
}

// Copia n bytes de s para d
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

// Converte N para ASCII em D usando BASE e retorna o numero de caracteres
// // escritos
int int_to_ascii(U64 n, int base, int is_neg, int is_upper, int pad,
                 int zero_pad, char *d) {
  if (!d || base < 2 || base > 36)
    return 0;

  if (n == 0) {
    int di = 0;

    if (is_neg)
      d[di++] = '-';

    for (int i = 0; i < pad - 1; i++) {
      d[di++] = zero_pad ? '0' : ' ';
    }

    d[di++] = '0';

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

  for (int i = len; i < pad; i++) {
    d[di++] = zero_pad ? '0' : ' ';
  }

  while (len > 0)
    d[di++] = tmp[--len];
  d[di] = 0;

  return di;
}
