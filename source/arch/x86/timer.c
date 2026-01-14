/**
 * timer.c
 * Criado por Matheus Leme Da Silva
 */
#include <../../defs.h>
#include <../../stdint.h>
#include <ints/pic.h>
#include <io.h>

#define MAKE_PIT_COMMAND(channel, access, op, bcd)                             \
  (((channel) << 6) | ((access) << 4) | ((op) << 1) | ((bcd) & 1))

#define PIT_BASE_FREQUENCY 1193182

void timer_set_freq(U16 freq) {
  U32 divisor = PIT_BASE_FREQUENCY / freq;
  if (divisor == 0)
    divisor = 1;
  if (divisor > 65535)
    divisor = 65535;
  outb(0x43, MAKE_PIT_COMMAND(0, 3, 2, 0));
  outb(0x40, divisor & 0xff);
  outb(0x40, divisor >> 8);
}

U32 volatile ticks = 0;

// Handlers
extern void terminal_tick();

// IRQ 0
void timer(struct regs *r) {
  (void)r;
  terminal_tick();
  ticks++;
}

// Inicializa TIMER
void timer_init(void) {
  timer_set_freq(KERNEL_PIT_FREQ);
  pic_unmask_irq(0);
  pic_set_irq_handler(0, timer);
}
