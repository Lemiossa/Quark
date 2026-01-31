/**
 * Util.c
 * Created by Matheus Leme Da Silva
 */
#include "Types.h"
#include "Vga.h"
#include "Util.h"
#include "Pit.h"
#include "Pcspk.h"

const char *PrintChars = "0123456789ABCDEF";

// Prints a hex byte
void PutHexU8(U8 b) {
	Putc(PrintChars[b >> 4]);
	Putc(PrintChars[b & 0xF]);
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

// Prints a hex qword
void PutHexU64(U64 qw) {
	PutHexU32(qw >> 32);
	PutHexU32(qw & 0xFFFFFFFF);
}

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

// Create a checksum with SUM
U8 Checksum(U8 *data, U32 len) {
	U8 sum = 0;
	for (U32 i = 0; i < len; i++) {
		sum += data[i];
	}
	return sum;
}

// Sleep N ticks
// Busy Wait
void SleepTicks(U64 N) {
	U64 start = PITGetTicks();
	while ((PITGetTicks() - start) < N);
}

// Plays a beep sound at FREQ for TICKS
void Beep(U16 freq, U64 ticks) {
	if (freq == 0 || ticks == 0)
		return;
	PcspkSetFreq(freq);
	PcspkEnable();
	SleepTicks(ticks);
	PcspkDisable();
}

