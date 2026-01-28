/**
 * Pic.c
 * Created by Matheus Leme Da Silva
 */
#include "Io.h"
#include "Types.h"
#include "Idt.h"
#include "Pic.h"

#define PIC1_COMMAND 0x20
#define PIC2_COMMAND 0xA0
#define PIC1_DATA 0x21
#define PIC2_DATA 0xA1

#define PIC_EOI 0x20

Irq Irqs[16];

// Execute a IRQ
void PicExecIrq(U8 IRQ, struct IntFrame *f) {
	if (Irqs[IRQ])
		Irqs[IRQ](f);
}

// Send PIC EOI
void PicSendEoi(U8 IRQ) {
	if (IRQ >= 8)
		OutU8(PIC2_COMMAND, PIC_EOI);
	OutU8(PIC1_COMMAND, PIC_EOI);
}

// Remap PIC
void PicRemap(U32 off1, U32 off2) {
	// ICW1
	OutU8(PIC1_COMMAND, 0x11);
	IoWait();
	OutU8(PIC2_COMMAND, 0x11);
	IoWait();

	// ICW2
	OutU8(PIC1_DATA, off1);
	IoWait();
	OutU8(PIC2_DATA, off2);
	IoWait();

	// ICW3
	OutU8(PIC1_DATA, 4);
	IoWait();
	OutU8(PIC2_DATA, 2);
	IoWait();

	// ICW4
	// Uses 8086 PIC mode
	OutU8(PIC1_DATA, 1);
	IoWait();
	OutU8(PIC2_DATA, 1);
	IoWait();

	OutU8(PIC1_DATA, 0xFF);
	OutU8(PIC2_DATA, 0xFF);
}

// Unmask a PIC IRQ
void PicUnmaskIrq(U8 IRQ) {
	if (IRQ >= 16) return;
	U16 port = PIC1_DATA;

	if (IRQ >= 8) {
		port = PIC2_DATA;
		IRQ -= 8;
	}

	U8 val = InU8(port) & ~(1 << IRQ);
	OutU8(port, val);
}

// Mask a PIC IRQ
void PicMaskIrq(U8 IRQ) {
	if (IRQ >= 16) return;
	U16 port = PIC1_DATA;

	if (IRQ >= 8) {
		port = PIC2_DATA;
		IRQ -= 8;
	}

	U8 val = InU8(port) | (1 << IRQ);
	OutU8(port, val);
}
