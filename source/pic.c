/**
 * pic.c
 * Criado por Matheus Leme Da Silva
 */
#include "pic.h"
#include "ports.h"

// Envia EOI pro PIC
void pic_send_eoi(u8 irq) {
  if (irq >= 8)
    outb(PIC2_COMMAND, PIC_EOI);
  outb(PIC1_COMMAND, PIC_EOI);
}

// Remapeia PIC para 0x20 e 0x28
void pic_remap(void) {
  // ICW1
  outb(PIC1_COMMAND, 0x11);
  io_wait();
  outb(PIC2_COMMAND, 0x11);
  io_wait();

  // ICW2
  outb(PIC1_DATA, 0x20);
  io_wait();
  outb(PIC2_DATA, 0x28);
  io_wait();

  // ICW3
  outb(PIC1_DATA, 0x04);
  io_wait();
  outb(PIC2_DATA, 0x02);
  io_wait();

  // ICW4
  outb(PIC1_DATA, 0x01);
  io_wait();
  outb(PIC2_DATA, 0x01);
  io_wait();

  outb(PIC1_DATA, 0b11111111);
  outb(PIC2_DATA, 0b11111111);
}
