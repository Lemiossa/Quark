/**
 * Util.c
 * Created by Matheus Leme Da Silva
 */
#include "Util.h"
#include "Types.h"
#include "Vga.h"

const char *HexChars = "0123456789ABCDEF";

// Copy N bytes from S to D
void Memcpy(void *d, const void *s, U32 n) {
  if (n == 0 || !d || !s)
    return;

  U8 *dest = (U8 *)d;
  U8 *src = (U8 *)s;
  for (U32 i = 0; i < n; i++) {
    dest[i] = src[i];
  }
}

// Set N bytes of D with B
void Memset(void *d, U8 b, U32 n) {
  if (n == 0 || !d)
    return;

  U8 *dest = (U8 *)d;
  for (U32 i = 0; i < n; i++) {
    dest[i] = b;
  }
}

// Compares N bytes of S1 and S2
int Memcmp(const void *s1, const void *s2, U32 n) {
  U8 *src1 = (U8 *)s1;
  U8 *src2 = (U8 *)s2;

  for (U32 i = 0; i < n; i++) {
    if (src1[i] != src2[i])
      return src1[i] < src2[i] ? -1 : 1;
  }

  return 0;
}

// Prints a hex byte
void PutHexU8(U8 b) {
  Putc(HexChars[b >> 4]);
  Putc(HexChars[b & 0xF]);
}

// Prints a hex word
void PutHexU16(U16 w) {
  PutHexU8(w >> 8);
  PutHexU8(w & 0xFF);
}

// Prints a hex dword
void PutHexU32(U32 dw) {
  PutHexU16(dw >> 16);
  PutHexU16(dw & 0xFFFF);
}

// Prints error message and halt system
void Panic(void) {
  Puts("Bootloader panic!\r\n");
  Puts("System halted!\r\n");
  for (;;)
    ;
}

// Read LBA sector using BIOS and copy to BUFFER
// Useful for if you want to charge things above 1MiB
void DiskRead(U8 drive, U32 lba, void *buffer) {
  void *lbuf = (void *)0x5000;
  Memset(lbuf, 0, 512);
  Memset(buffer, 0, 512);

  U8 ret;
  for (int i = 0; i < 3; i++) {
    ret = ExtendedDiskRead(drive, lba, lbuf);
    if (ret == 0) {
      Memcpy(buffer, lbuf, 512);
      break;
    } else if (i <= 2) {
      DiskReset(drive);
    }
  }

  if (ret != 0) {
    Puts("Failed tu read sector: 0x");
    PutHexU32(lba);
    Puts("\r\nError code: 0x");
    PutHexU8(ret);
    Puts("\r\n");
    Panic();
  }
}
