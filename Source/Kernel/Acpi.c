/**
 * Acpi.h
 * Created by Matheus Leme Da Silva
 */
#include "Types.h"
#include "Acpi.h"
#include "Util.h"
#include "Panic.h"
#include "Vga.h"

struct ACPIRsdp *Rsdp = NULL;
struct ACPIRsdt *Rsdt = NULL;
int ACPIVersion = 0;


// Finds RSDP in address
void *ACPIFindRsdp(U32 base) {
	for (U32 p = base; p < base + 1024; p += 16) {
		if (Memcmp((void *)p, "RSD PTR ", 8) == 0) {
			return (void *)p;
		}
	}
	return NULL;
}

// Finds SDT in RSDT
void *ACPIRsdtFind(struct ACPIRsdt *Rsdt, char *sig) {
	U32 EntCnt = (Rsdt->h.Length - sizeof(Rsdt->h)) / 4;

	for (U32 i = 0; i < EntCnt; i++) {
		struct ACPISdt *h = (struct ACPISdt *)Rsdt->PtrOSdt[i];
		if (Memcmp(h->Sig, sig, 4) == 0)
			return (void *)h;
	}

	return NULL;
}

// Initializes ACPI
void ACPIInit(void) {
	U16 volatile EBDASeg = *(U16 volatile*)0x40E;

	void *addr = (void *)(U32)(EBDASeg << 4);
	Rsdp = ACPIFindRsdp((U32)addr); // Case 1: EBDA
	if (!Rsdp) { // Case 2: 0xE0000 - 0xFFFFF
		addr = (void *)0xE0000;
		while (!Rsdp && addr <= (void *)0xFFFFF) {
			Rsdp = ACPIFindRsdp((U32)addr);
			if (Rsdp) break;
			addr += 1024;
		}
	}

	if (Rsdp) {
		if (Checksum((U8 *)Rsdp, sizeof(struct ACPIRsdp)) != 0)
			Panic("Invalid RSDP checksum");

		Puts("Found ACPI RSDP at 0x");
		PutHexU32((U32)Rsdp);
		Puts("\r\n");

		Puts("Oem id: '");
		for (int i = 0; i < 6; i++ ) {
			Putc(Rsdp->OemID[i]);
		}

		Puts("', Revision: 0x");
		PutHexU8(Rsdp->Revision);
		switch (Rsdp->Revision) {
			case 0: {
				Puts("(ACPI 1.0)");
			} break;
			case 2: {
				Puts("(ACPI 2.0+)\r\n");
				Panic("Only ACPI 1.0 is supported");
			} break;
			default: {
				Puts("(Unknown)\r\n");
				Panic("Invalid ACPI");
			} break;
		}

		Puts("\r\n");
	}

	Puts("RSDT Addr: 0x");
	PutHexU32(Rsdp->RsdtAddr);
	Puts("\r\n");
	Rsdt = (void *)Rsdp->RsdtAddr;
}

