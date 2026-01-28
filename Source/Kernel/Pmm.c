/**
 * Pmm.c
 * Created by Matheus Leme Da Silva
 */
#include "Types.h"
#include "E820.h"
#include "Util.h"
#include "Vga.h"

#define PAGE_SIZE (4 * 1024)

U32 *PmmBitmap = NULL;
U32 PmmTotalPages = 0;
U32 PmmBitmapSize = 0;
U32 PmmLastFreePage = 0;

// Mark a page in bitmap
void PmmMarkPage(U32 page) {
	if (!PmmBitmap) return;
	U32 *ele = &PmmBitmap[page / 32];
	*ele |= (1 << (page % 32));
}

// Unmark a page in bitmap
void PmmUnmarkPage(U32 page) {
	if (!PmmBitmap) return;
	U32 *ele = &PmmBitmap[page / 32];
	*ele &= ~(1 << (page % 32));
}

// Return 1 if page is used
// -1 if have error
int PmmCheckPage(U32 page) {
	if (!PmmBitmap) return -1;
	U32 *ele = &PmmBitmap[page / 32];
	return *ele & (1 << (page % 32));
}

// Initializes Physical Memory Manager
void PmmInit(struct E820Entry *table, U8 entries) {
	for (U8 i = 0; i < entries;i++) {
		U32 start = (U32)table[i].Base;
		U32 end = start + (U32)table[i].Length;
		PmmTotalPages += (U32)(end - start) / PAGE_SIZE;
	}
	PmmBitmapSize = PmmTotalPages / 32;
	PmmBitmap = (U32 *)__BSS_END_ADDR;
	Memset(PmmBitmap, 0, PmmBitmapSize);

	// Set reserved areas
	for (U32 page = 0; page < 256; page++){
		PmmMarkPage(page);
	}

	// Kernel
	U32 kernelStartPage = ALIGN_DOWN((U32)__START_ADDR, PAGE_SIZE) / PAGE_SIZE;
	U32 kernelPages = ALIGN_UP((U32)__END_ADDR - (U32)__START_ADDR, PAGE_SIZE) / PAGE_SIZE;
	for (U32 page = kernelStartPage; page < kernelStartPage + kernelPages; page++){
		PmmMarkPage(page);
	}

	for (U8 i = 0; i < entries;i++) {
		if (table[i].Type != E820_USABLE) {
			U32 start = ALIGN_DOWN((U32)table[i].Base, PAGE_SIZE);
			U32 pages = ALIGN_UP(((U32)table[i].Base + (U32)table[i].Length) - (U32)table[i].Base, PAGE_SIZE);
			for (U32 page = start; page < start + pages; page++){
				PmmMarkPage(page);
			}
		}
	}
}

// Alloc a page
void *PmmAllocPage(void) {
	U32 page = PmmLastFreePage;

	if (page != 0) {
		PmmLastFreePage = 0;
		return (void *)(page * PAGE_SIZE);
	}

	page = 0;
	while (page < PmmTotalPages) {
		if (PmmCheckPage(page) == 0) {
			PmmMarkPage(page);
			return (void *)(page * PAGE_SIZE);
		}

		page++;

	}

	return NULL;
}

// Free a page
void PmmFreePage(void *p) {
	if (!p) return;
	U32 page = (U32)p / PAGE_SIZE;
	PmmLastFreePage = page;
	PmmUnmarkPage(page);
}

