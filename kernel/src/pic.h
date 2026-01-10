/**
 * pic.h
 * Criado por Matheus Leme Da Silva
 */
#pragma once
#include "stdint.h"

#define PIC1_COMMAND 0x20
#define PIC1_DATA 0x21

#define PIC2_COMMAND 0xa0
#define PIC2_DATA 0xa1

#define PIC_EOI 0x20

void pic_send_eoi(u8 irq);
void pic_remap(void);
