/**
 * Pic.c
 * Created by Matheus Leme Da Silva
 */
#include "Io.h"
#include "Types.h"

#define PIC1_COMMAND 0x20
#define PIC2_COMMAND 0xA0
#define PIC1_DATA 0x21
#define PIC2_DATA 0xA1

#define PIC_EOI 0x20

// Send PIC EOI
void PicSendEoi(U8 irq) {
  if (irq >= 8)
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
