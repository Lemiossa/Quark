/**
 * Util.c
 * Created by Matheus Leme Da Silva
 */
#include "Util.h"
#include "MemDefs.h"
#include "Types.h"
#include "Vga.h"

const char *PrintChars = "0123456789ABCDEF";

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

// Prints error message and halt system
void Panic(void) {
	Puts("Panic!\r\n");
	Puts("System halted!\r\n");
	for (;;);
}

// Read COUNT sectors at LBA using BIOS and copy to BUFFER
// Useful for if you want to charge things above 1MiB
void DiskRead(U8 drive, U32 lba, U8 count, void *buffer) {
	void *lbuf = (void *)DISK_BUFFER;
	U32 size = count * SCT_SIZE;
	Memset(lbuf, 0, size);
	Memset(buffer, 0, size);

	U16 driveParams = BiosGetDriveParameters(drive);

	U8 SPT = driveParams & 0xFF;
	U8 HPC = (driveParams >> 8) & 0xFF;

	if (SPT == 0 || HPC == 0) {
		Puts("Failed to read sector: 0x");
		PutHexU32(lba);
		Puts("\r\nInvalid SPT or HPC of drive\r\n");
		Panic();
	}

	U32 tmp = lba / SPT;
	U32 sector = (lba % SPT) + 1;
	U32 head = tmp % HPC;
	U32 cylinder = tmp / HPC;

	U8 ret = 0;
	for (int i = 0; i < 3; i++) {
		ret = BiosDiskRead(drive, cylinder, head, sector, count, lbuf);
		if (ret == 0) {
			Memcpy(buffer, lbuf, size);
			break;
		}
		BiosDiskReset(drive);
	}

	if (ret != 0) {
		Puts("Failed to read sector: 0x");
		PutHexU32(lba);
		Puts("\r\nError code: 0x");
		PutHexU8(ret);
		Puts("\r\n");
		Panic();
	}
}

// Get path part
// Returns non zero if have error
int GetPathPart(const char *path, char *out, U32 part, U32 outMax) {
	if (!path || !out || outMax == 0)
		return 1;

	if (path[0] == '/')
		path++;

	while (*path && part > 0) {
		if (*path == '/')
			part--;
		path++;
	}

	if (*path == 0)
		return 1;

	for (U32 i = 0; i < outMax; i++) {
		char c = path[i];
		if (c == '/' || c == 0)
			break;
		out[i] = c;
	}

	return 0;
}
