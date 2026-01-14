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

void idt_set_gate(U8 gate, void *isr, U16 segment_selector, U8 attributes) {
  idt[gate].offset_low = (U32)isr & 0xffff;
  idt[gate].offset_high = ((U32)isr >> 16) & 0xffff;
  idt[gate].attributes = attributes;
  idt[gate].segment_selector = segment_selector;
  idt[gate].zero = 0;
}

void idt_init(void) {
  for (U8 i = 0; i < isr_count; i++) {
    idt_set_gate(i, (void *)isr_table[i], KERNEL_CODE_SELECTOR, 0x8e);
  }

  idtr.size = sizeof(struct idt_gate) * IDT_MAX_ENTRIES - 1;
  idtr.offset = (U32)&idt[0];
  __asm__ volatile("lidt %0" ::"m"(idtr));
}

char *exception_messages[32] = {
    "Division by zero",               // 0
    "Debug exception",                // 1
    "Non-maskable interrupt",         // 2
    "Breakpoint",                     // 3
    "Overflow",                       // 4
    "Bound range exceeded",           // 5
    "Invalid opcode",                 // 6
    "Device not available",           // 7
    "Double fault",                   // 8
    "Coprocessor segment overrun",    // 9
    "Invalid TSS",                    // 10
    "Segment not present",            // 11
    "Stack-segment fault",            // 12
    "General protection fault",       // 13
    "Page fault",                     // 14
    "Reserved",                       // 15
    "x87 floating-point exception",   // 16
    "Alignment check",                // 17
    "Machine check",                  // 18
    "SIMD floating-point exception",  // 19
    "Virtualization exception",       // 20
    "Control protection exception",   // 21
    "Reserved",                       // 22
    "Reserved",                       // 23
    "Reserved",                       // 24
    "Reserved",                       // 25
    "Reserved",                       // 26
    "Reserved",                       // 27
    "Hypervisor injection exception", // 28
    "VMM communication exception",    // 29
    "Security exception",             // 30
    "Reserved"                        // 31
};

void interrupt_handler(struct regs *r) {
  if (r->int_no < 32)
    panic(exception_messages[r->int_no], r);

  if (r->int_no >= 32 && r->int_no <= 47) {
    pic_execute_irq(r->int_no - 32, r);
    pic_send_eoi(r->int_no - 32);
    return;
  }
}
