/**
 * except.c
 * Criado por Matheus Leme Da Silva
 */
#include <ints/idt.h>
#include <panic.h>
#include <printk.h>
#include <sched.h>

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

// Handler de exceções
struct regs *exception_handler(struct regs *r) {
  if (current_task->pid == 0)
    panic("Exception on kernel!", r);

  printk("%s: Fatal error: %s\n", current_task->name,
         exception_messages[r->int_no]);
  printk("The task cannot continue running\n");

  current_task->status = TASK_ZOMBIE;
  current_task = current_task->next;

  return current_task->r;
}
