/**
 * Util.h
 * Created by Matheus Leme Da Silva
 */
#ifndef UTIL_H
#define UTIL_H
#include "Types.h"

#define SCT_SIZE 512

extern U8 BootDrive;

void DiskReset(U8 drive);
U8 ExtendedDiskRead(U8 drive, U32 lba, U16 count, void *buffer);

void Memcpy(void *d, const void *s, U32 n);
void Memset(void *d, U8 b, U32 n);
int Memcmp(const void *s1, const void *s2, U32 n);
void PutHexU8(U8 b);
void PutHexU16(U16 w);
void PutHexU32(U32 dw);
void Panic(void);
void DiskRead(U8 drive, U32 lba, U16 count, void *buffer);
int GetPathPart(const char *path, char *out, U32 part, U32 outMax);

#endif // UTIL_H
