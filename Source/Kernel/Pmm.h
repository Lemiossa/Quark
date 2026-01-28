/**
 * Pmm.h
 * Created by Matheus Leme Da Silva
 */
#ifndef PMM_H
#define PMM_H
#include "Types.h"

void PmmInit(struct E820Entry *table, U8 entries);
void *PmmAllocPage(void);
void PmmFreePage(void *p);

#endif // PMM_H
