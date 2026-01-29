/**
 * Acpi.h
 * Created by Matheus Leme Da Silva
 */
#ifndef ACPI_H
#define ACPI_H
#include "Types.h"

struct ACPIRsdp {
	char Sig[8];
	U8 Checksum;
	char OemID[6];
	U8 Revision;
	U32 RsdtAddr;
} __attribute__((packed));

struct ACPISdt {
	char Sig[4];
	U32 Length;
	U8 Revision;
	U8 Checksum;
	char OemID[6];
	char OemTableID[8];
	U32 OemRevsion;
	U32 CreatorID;
	U32 CreatorRevision;
} __attribute__((packed));

struct ACPIRsdt {
	struct ACPISdt h;
	U32 PtrOSdt[];
} __attribute__((packed));

extern struct ACPIRsdp *Rsdp;
extern struct ACPIRsdt *Rsdt;
extern int ACPIVersion;

void *ACPIFindRsdp(U32 base);
void *ACPIRsdtFind(struct ACPIRsdt *Rsdt, char *sig);
void ACPIInit(void);

#endif // ACPI_H
