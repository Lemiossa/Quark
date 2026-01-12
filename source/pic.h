/**
 * pic.h
 * Criado por Matheus Leme Da Silva
 */
#ifndef PIC_H
#define PIC_H
#include "idt.h"
#include "stdint.h"

#define PIC1_COMMAND 0x20
#define PIC1_DATA 0x21

#define PIC2_COMMAND 0xa0
#define PIC2_DATA 0xa1

#define PIC_EOI 0x20

typedef void (*irq_handler_t)(struct regs *r);

void pic_send_eoi(u8 irq);
void pic_remap(void);
void pic_mask_irq(u8 irq);
void pic_unmask_irq(u8 irq);
void pic_set_irq_handler(u8 irq, irq_handler_t handler);
void pic_execute_irq(u8 irq, struct regs *r);

#endif // PIC_H
