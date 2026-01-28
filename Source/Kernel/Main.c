/**
 * Main.c
 * Created by Matheus Leme Da Silva
 */
#include "Types.h"
#include "Vga.h"
#include "Gdt.h"
#include "Idt.h"
#include "Pic.h"
#include "E820.h"
#include "Pmm.h"
#include "Serial.h"
#include "Util.h"

struct QuarkBootInfo {
	struct E820Entry *E820Table;
	U8 E820EntryCnt;
};

// The Main function don't return
void Main(struct QuarkBootInfo *bootInfo) {
	SetCurrentAttr(MAKE_VGA_COLOR(WHITE, BLUE));
	Clear();
	E820Print(bootInfo->E820Table, bootInfo->E820EntryCnt);
	PmmInit(bootInfo->E820Table, bootInfo->E820EntryCnt);

	GdtInit();
	IdtInit();
	if (SerialInit(COM1, 3) == 0) {
		Puts("Initialized Serial port 0x");
		PutHexU16(COM1);
		Puts("\r\n");
	}
	PicRemap(0x20, 0x28);


	Puts("Hello Kernel!\r\n");

	Puts("Kernel size: 0x");
	PutHexU32((U32)__END_ADDR - (U32)__START_ADDR);
	Puts("\r\n");

	void *ptr = (void *)PmmAllocPage();
	Puts ("Allocated page: 0x");
	PutHexU32((U32)ptr);
	Puts("\r\n");

	for (;;);
}
