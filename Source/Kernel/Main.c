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
#include "Io.h"

struct QuarkBootInfo {
	struct E820Entry *E820Table;
	U8 E820EntryCnt;
};

#define PITBASEFREQ 1193182

U64 Ticks = 0;

void PitHandler(struct IntFrame *f) {
	(void)f;
	Ticks++;
}

void PitInit(U16 freq) {
	if (freq == 0) return;
	U16 divisor = PITBASEFREQ/freq;
	OutU8(0x43, 0b00110100);
	OutU8(0x40, divisor & 0xFF); // Low byte
	OutU8(0x40, (divisor >> 8) & 0xFF); // High byte
	PicUnmaskIrq(0);
	Irqs[0] = PitHandler;
}

// The Main function don't return
void Main(struct QuarkBootInfo *bootInfo) {
	SetCurrentAttr(MAKE_VGA_COLOR(WHITE, BLUE));
	Clear();
	E820Print(bootInfo->E820Table, bootInfo->E820EntryCnt);
	PmmInit(bootInfo->E820Table, bootInfo->E820EntryCnt);

	SerialInit(COM1, 3);

	GdtInit();
	IdtInit();
	PicRemap(0x20, 0x28);
	PitInit(100);
	__asm__ volatile("sti");

	Puts("Hello Kernel!\r\n");

	for (;;);
}
