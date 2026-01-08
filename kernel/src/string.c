/**
 * string.c
 * Criado por Matheus Leme Da Silva
 */
#include "string.h"
#include "stddef.h"
#include "stdint.h"

// Copia n bytes de s para d
void *memcpy(void *d, const void *s, u32 n) {
	if (!d || !s)
		return NULL;
	if (n == 0)
		return d;
	u8 *dest = (u8 *)d;
	const u8 *src = (const u8 *)s;

	for (u32 i = 0; i < n; i++) {
		dest[i] = src[i];
	}

	return (void *)dest;
}
