/**
 * Main.c
 * Created by Matheus Leme Da Silva
 */
#include "Fat.h"
#include "Types.h"
#include "Util.h"
#include "Vga.h"

struct MbrPart {
	U8 BootSig;
	U8 StartHead, StartSct, StartCyl;
	U8 Type;
	U8 EndHead, EndSct, EndCyl;
	U32 StartLBA;
	U32 TotalScts;
} __attribute__((packed));

U8 Mbr[512];
struct MbrPart *PartTable = (struct MbrPart *)&Mbr[446];
int part = 0;

struct MuonBootInfo {
	struct E820Entry *E820Table;
	U8 E820EntryCnt;
};

struct MuonBootInfo BootInfo;
struct E820Entry E820Table[128];
U8 E820EntryCnt = 0;

#define FILEPATH "/boot/kernel.sys"


// The Main function don't return
void Main(void) {
	Puts("Loading '" FILEPATH "'...\r\n");

	// Read the MBR
	DiskRead(BootDrive, 0, 1, Mbr);
	if (Mbr[510] != 0x55 || Mbr[511] != 0xAA) {
		Puts("Invalid MBR!\r\n");
		Panic();
	}

	for (int i = 0; i < 4; i++) {
	if (PartTable[i].BootSig)
		part = i;
	}

	struct FatPart p;
	if (FatInit(BootDrive, PartTable[part].StartLBA, &p) != 0) {
		Puts("Failed to initialize partition\r\n");
		Panic();
	}

	struct FatDirEntry e;
	if (FatFind(p, FILEPATH, &e) != 0) {
		Puts("Failed to find " FILEPATH "\r\n");
		Panic();
	}

	E820EntryCnt = E820GetTable(E820Table, 128);

	BootInfo.E820Table = &E820Table[0];
	BootInfo.E820EntryCnt = E820EntryCnt;

	void *file_content = (void *)0x100000;

	U32 remaining = e.FileSz;
	U32 offset = 0;
	U32 blkSz = 65536;
	while (remaining > 0) {
		U32 toRead = remaining > blkSz ? blkSz : remaining;
		U32 r = FatRead(p, e, offset, toRead, file_content + offset);
		if (r != toRead) {
			Puts("\r\nFailed to read " FILEPATH "\r\n");
			Panic();
		}

		remaining -= toRead;
		offset += toRead;
		Puts("\rLoaded 0x");
		PutHexU32(e.FileSz - remaining);
		Puts(" bytes of 0x");
		PutHexU32(e.FileSz);
	}
	Puts("\r\n");


	__asm__ volatile("jmp *%1" :: "a"(&BootInfo), "m"(file_content));

	for (;;);
}
