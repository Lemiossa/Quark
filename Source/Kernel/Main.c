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
#include "Pit.h"
#include "Acpi.h"
#include "Kbd.h"

struct MuonBootInfo {
	struct E820Entry *E820Table;
	U8 E820EntryCnt;
};

static const char KbdUS[128] = {
    0,   27, '1', '2', '3', '4', '5', '6',  // 0x00 - 0x07
   '7', '8', '9', '0', '-', '=', '\b', '\t', // 0x08 - 0x0F
   'q', 'w', 'e', 'r', 't', 'y', 'u', 'i',  // 0x10 - 0x17
   'o', 'p', '[', ']', '\r',  0,  'a', 's', // 0x18 - 0x1F
   'd', 'f', 'g', 'h', 'j', 'k', 'l', ';',  // 0x20 - 0x27
  '\'', '`',   0, '\\', 'z', 'x', 'c', 'v', // 0x28 - 0x2F
   'b', 'n', 'm', ',', '.', '/',   0,  '*', // 0x30 - 0x37
     0,  ' ',   0,    0,    0,    0,    0,   0, // 0x38 - 0x3F
};

static const char KbdUSShift[128] = {
    0,   27, '!', '@', '#', '$', '%', '^',  // 0x00 - 0x07
   '&', '*', '(', ')', '_', '+', '\b', '\t', // 0x08 - 0x0F
   'Q', 'W', 'E', 'R', 'T', 'Y', 'U', 'I',  // 0x10 - 0x17
   'O', 'P', '{', '}', '\r',  0,  'A', 'S', // 0x18 - 0x1F
   'D', 'F', 'G', 'H', 'J', 'K', 'L', ':',  // 0x20 - 0x27
   '"', '~',   0,  '|', 'Z', 'X', 'C', 'V', // 0x28 - 0x2F
   'B', 'N', 'M', '<', '>', '?',   0,  '*', // 0x30 - 0x37
     0,  ' ',   0,    0,    0,    0,    0,   0, // 0x38 - 0x3F
};

// It waits for a button to be pressed and returns its ASCII character.
char Getc(void) {
	struct KbdEvent ev;
wait:
	while (KbdPop(&ev) != 0);
	if (ev.Released)
		goto wait;

	if (ev.Modifiers & KBD_MOD_SHIFT)
		return KbdUSShift[ev.Scancode];
	else
		return KbdUS[ev.Scancode];
}

// Gets a string from keyboard
void Gets(char *out, int max) {
	if (!out || max == 0)
		return;
	int pos = 0;

	while (1)  {
		U8 c = Getc();

		if (c == '\r') {
			out[pos] = 0;
			Puts("\r\n");
			return;
		} else if (c == '\b' && pos > 0) {
			out[pos--] = 0;
			Puts("\b \b");
		} else if (c >= 32 && c <= 127 && pos < max) {
			out[pos++] = c;
			Putc(c);
		}
	}
}

// The Main function don't return
void Main(struct MuonBootInfo *bootInfo) {
	GdtInit();
	SetCurrentAttr(MAKE_VGA_COLOR(WHITE, BLUE));
	Clear();
	SerialInit(COM1, 3);

	E820Print(bootInfo->E820Table, bootInfo->E820EntryCnt);
	PmmInit(bootInfo->E820Table, bootInfo->E820EntryCnt);

	ACPIInit();

	IdtInit();
	PicRemap(0x20, 0x28);
	PitInit(1000);

	KbdInit();
	__asm__ volatile("sti");
	Clear();

	Puts("Hello Kernel!\r\n");

	char line[256];
	for (;;) {
		Puts("# ");
		Gets(line, sizeof(line));
	}
}
