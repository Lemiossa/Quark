/**
 * Main.c
 * Created by Matheus Leme Da Silva
 */
#include "Vga.h"
#include "Gdt.h"
#include "Idt.h"
#include "Pic.h"

struct E820Entry {
	U64 Base;
	U64 Length;
	U32 Type;
	U32 Attr;
} __attribute__((packed));

struct QuarkBootInfo {
	struct E820Entry *E820Table;
	U8 E820EntryCnt;
};

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

#define E820_USABLE         1
#define E820_RESERVED       2
#define E820_ACPI_RECLAIMABLE 3
#define E820_ACPI_NVS       4
#define E820_BAD_MEMORY     5

const char *E820TypeStrings[] = {
    "Unknown",
    "Usable RAM",
    "Reserved",
    "ACPI Reclaimable",
    "ACPI NVS",
    "Bad Memory"
};

#define E820_TYPE_MAX 5

// Gets E820 type string
static inline const char *E820GetTypeString(U32 type) {
    if (type > E820_TYPE_MAX)
        return "Unknown/Reserved";
    return E820TypeStrings[type];
}

// Prints E820
void E820Print(struct E820Entry *table, U8 entries) {
	for (U8 i = 0; i < entries; i++) {
		Puts("BIOS-E820: \r\n    Base: 0x");
		PutHexU64(table[i].Base);
		Puts("\r\n    Length: 0x");
		PutHexU64(table[i].Length);
		Puts("\r\n    Type: ");
		Puts(E820GetTypeString(table[i].Type));
		Puts("\r\n");
	}
}

// The Main function don't return
void Main(struct QuarkBootInfo *bootInfo) {
	SetCurrentAttr(MAKE_VGA_COLOR(GRAY, BLACK));
	E820Print(bootInfo->E820Table, bootInfo->E820EntryCnt);

	GdtInit();
	IdtInit();
	PicRemap(0x20, 0x28);

	__asm__ volatile("sti");
	Puts("Hello Kernel!\r\n");

	for (;;);
}
