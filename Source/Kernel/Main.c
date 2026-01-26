/**
 * Main.c
 * Created by Matheus Leme Da Silva
 */
#include "Vga.h"
#include "Gdt.h"
#include "Idt.h"
#include "Pic.h"

// The Main function don't return
void Main(void) {
	SetCurrentAttr(MAKE_VGA_COLOR(GRAY, BLACK));

	GdtInit();
	IdtInit();
	PicRemap(0x20, 0x28);

	__asm__ volatile("sti");
	Puts("Hello Kernel!\r\n");


	for (;;);
}
