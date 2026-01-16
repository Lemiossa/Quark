/**
 * pic.c
 * Criado por Matheus Leme Da Silva
 */
#include <ints/pic.h>
#include <io.h>
#include <stddef.h>

// Envia EOI pro PIC
void pic_send_eoi(U8 irq) {
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

// Mascara uma IRQ
void pic_mask_irq(U8 irq) {
  U16 port = PIC1_DATA;

  if (irq >= 8) {
    port = PIC2_DATA;
    irq -= 8;
  }

  U8 data = (inb(port) | (1 << irq));
  outb(port, data);
}

// Desmascara uma IRQ
void pic_unmask_irq(U8 irq) {
  U16 port = PIC1_DATA;

  if (irq >= 8) {
    port = PIC2_DATA;
    irq -= 8;
  }

  U8 data = (inb(port) & ~(1 << irq));
  outb(port, data);
}

irq_handler_t irqs[16] = {NULL};

// Muda o handler de uma IRQ
// NOTE: não aceita mudar o handler da IRQ 2
void pic_set_irq_handler(U8 irq, irq_handler_t handler) {
  if (irq == 2 || !handler)
    return;

  irqs[irq] = handler;
}

// Executa uma IRQ
struct regs *pic_execute_irq(U8 irq, struct regs *r) {
  if (irq >= 16)
    return r;
  return irqs[irq](r);
}
