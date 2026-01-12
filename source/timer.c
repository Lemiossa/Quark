/**
 * timer.c
 * Criado por Matheus Leme Da Silva
 */
#include "kdefs.h"
#include "pic.h"
#include "ports.h"
#include "stdint.h"

#define PIT_BASE_FREQUENCY 1193182

void timer_set_freq(u16 freq) {
  u32 divisor = PIT_BASE_FREQUENCY / freq;
  if (divisor == 0)
    divisor = 1;
  if (divisor > 65535)
    divisor = 65535;
  outb(0x43, 0b00110100);
  outb(0x40, divisor & 0xff);
  outb(0x40, (divisor >> 8) & 0xff);
}

u32 volatile ticks = 0;

// Handlers
extern void terminal_tick();

// IRQ 0
void timer(struct regs *r) {
  terminal_tick();
  ticks++;
}

// Inicializa TIMER
void timer_init(void) {
  timer_set_freq(PIT_FREQ);
  pic_unmask_irq(0);
  pic_set_irq_handler(0, timer);
}
