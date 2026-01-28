/**
 * Pic.h
 * Created by Matheus Leme Da Silva
 */
#ifndef PIC_H
#define PIC_H
#include "Types.h"
#include "Idt.h"

typedef void (*Irq)(struct IntFrame *f);
extern Irq Irqs[16];

void PicExecIrq(U8 IRQ, struct IntFrame *f);
void PicSendEoi(U8 IRQ);
void PicRemap(U32 off1, U32 off2);
void PicUnmaskIrq(U8 IRQ);
void PicMaskIrq(U8 IRQ);

#endif // PIC_H
