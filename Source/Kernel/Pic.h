/**
 * Pic.h
 * Created by Matheus Leme Da Silva
 */
#ifndef PIC_H
#define PIC_H
#include "Types.h"

void PicSendEoi(U8 irq);
void PicRemap(U32 off1, U32 off2);

#endif // PIC_H
