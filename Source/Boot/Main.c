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

// The Main function don't return
void Main(void) {
  Clear();
  Puts("Loading...");

  // Read the MBR
  DiskRead(BootDrive, 0, Mbr);
  if (Mbr[510] != 0x55 && Mbr[511] != 0xAA) {
    Puts("Invalid MBR!\r\n");
    Panic();
  }

  for (int i = 0; i < 4; i++) {
    if (PartTable[i].BootSig)
      part = i;
  }

  if (FatInit(BootDrive, PartTable[part].StartLBA) != 0) {
    Puts("Failed to initialize partition\r\n");
    Panic();
  }

  for (;;)
    ;
}
