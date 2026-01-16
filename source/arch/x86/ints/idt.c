/**
 * idt.c
 * Criado por Matheus Leme Da Silva
 */
#include <defs.h>
#include <ints/idt.h>
#include <ints/pic.h>
#include <panic.h>
#include <stdint.h>

struct idtr {
  U16 size;
  U32 offset;
} __attribute__((packed));

struct idt_gate {
  U16 offset_low;
  U16 segment_selector;
  U8 zero;
  U8 attributes;
  U16 offset_high;
} __attribute__((packed));

#define IDT_MAX_ENTRIES 256

struct idt_gate idt[IDT_MAX_ENTRIES];
struct idtr idtr;

extern U32 isr_table[];
extern U8 isr_count;

// Configura uma entrada na IDT
void idt_set_gate(U8 gate, void *isr, U16 segment_selector, U8 attributes) {
  idt[gate].offset_low = (U32)isr & 0xffff;
  idt[gate].offset_high = ((U32)isr >> 16) & 0xffff;
  idt[gate].attributes = attributes;
  idt[gate].segment_selector = segment_selector;
  idt[gate].zero = 0;
}

// Inicializa a IDT
void idt_init(void) {
  for (U8 i = 0; i < isr_count; i++) {
    idt_set_gate(i, (void *)isr_table[i], KERNEL_CODE_SELECTOR, 0x8e);
  }

  idtr.size = sizeof(struct idt_gate) * IDT_MAX_ENTRIES - 1;
  idtr.offset = (U32)&idt[0];
  __asm__ volatile("lidt %0" ::"m"(idtr));
}

extern struct regs *exception_handler(struct regs *r);

// Handler de interrupt
struct regs *interrupt_handler(struct regs *r) {
  if (r->int_no < 32)
    return exception_handler(r);

  if (r->int_no >= 32 && r->int_no <= 47) {
    struct regs *ret = pic_execute_irq(r->int_no - 32, r);
    pic_send_eoi(r->int_no - 32);
    return ret;
  }

  return r;
}
