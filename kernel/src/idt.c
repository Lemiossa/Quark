/**
 * idt.c
 * Criado por Matheus Leme Da Silva
 */
#include "idt.h"
#include "panic.h"
#include "pic.h"
#include "stdint.h"

struct idtr {
  u16 size;
  u32 offset;
} __attribute__((packed));

struct idt_gate {
  u16 offset_low;
  u16 segment_selector;
  u8 zero;
  u8 attributes;
  u16 offset_high;
} __attribute__((packed));

#define IDT_MAX_ENTRIES 256

struct idt_gate idt[IDT_MAX_ENTRIES];
struct idtr idtr;

void idt_set_gate(u8 gate, void *isr, u16 segment_selector, u8 attributes) {
  idt[gate].offset_low = (u32)isr & 0xffff;
  idt[gate].offset_high = ((u32)isr >> 16) & 0xffff;
  idt[gate].attributes = attributes;
  idt[gate].segment_selector = segment_selector;
  idt[gate].zero = 0;
}

#include "isr.h"

void idt_init(void) {
  isr_init();

  idtr.size = sizeof(struct idt_gate) * IDT_MAX_ENTRIES - 1;
  idtr.offset = (u32)&idt[0];
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
    panic(exception_messages[r->int_no], *r);

  if (r->int_no >= 32 && r->int_no <= 48)
    pic_send_eoi(r->int_no - 32);
}
