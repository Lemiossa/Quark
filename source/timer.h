/**
 * timer.c
 * Criado por Matheus Leme Da Silva
 */
#ifndef TIMER_H
#define TIMER_H

#include <defs.h>
#include <stdint.h>

#define MS_TO_TICK(ms) (((ms) * KERNEL_PIT_FREQ) / 1000)
#define TICK_TO_MS(tick) (((tick) * 1000) / KERNEL_PIT_FREQ)

extern U32 volatile ticks;

void timer_init(void);

#endif // TIMER_H
