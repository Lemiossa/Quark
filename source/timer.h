/**
 * timer.c
 * Criado por Matheus Leme Da Silva
 */
#ifndef TIMER_H
#define TIMER_H

#include "kdefs.h"
#include "stdint.h"

#define MS_TO_TICK(ms) ((ms * KERNEL_PIT_FREQ) / 1000)

extern u32 volatile ticks;

void timer_init(void);

#endif // TIMER_H
