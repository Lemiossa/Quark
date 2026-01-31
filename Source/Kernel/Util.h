/**
 * Util.h
 * Created by Matheus Leme Da Silva
 */
#ifndef UTIL_H
#define UTIL_H
#include "Types.h"

void PutHexU8(U8 b);
void PutHexU16(U16 w);
void PutHexU32(U32 dw);
void PutHexU64(U64 qw);

void Memcpy(void *d, const void *s, U32 n);
void Memset(void *d, U8 b, U32 n);
int Memcmp(const void *s1, const void *s2, U32 n);

U8 Checksum(U8 *data, U32 len);

void SleepTicks(U64 N);
void Beep(U16 freq, U64 ticks);

#endif // UTIL_H
