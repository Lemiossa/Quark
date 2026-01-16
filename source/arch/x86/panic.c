/**
 * panic.c
 * Criado por Matheus Leme Da Silva
 */
#include <asm.h>
#include <panic.h>
#include <printk.h>

// Dá um panic no kernel
void panic(char *str, struct regs *r) {
  printk("Kernel panic!\n");
  printk("%s\n\n", str);

  printk("eip=%08x cs=%08x ds=%08x\n", r->eip, r->cs, r->ds);
  printk("es=%08x fs=%08x gs=%08x\n", r->es, r->fs, r->gs);
  printk("esp=%08x eflags=%08x\n", r->esp, r->eflags);
  printk("eax=%08x ebx=%08x ecx=%08x\n", r->eax, r->ebx, r->ecx);
  printk("edx=%08x esi=%08x edi=%08x\n\n", r->edx, r->esi, r->edi);

  printk("System halted! please, reboot\n");

  for (;;) {
    CLI();
    HLT();
  }
}
