/**
 * Pit.c
 * Created by Matheus Leme Da Silva
 */
#include "Idt.h"
#include "Pic.h"
#include "Io.h"
#include "Pit.h"

U64 Ticks = 0;

// PIT handler
void PITHandler(struct IntFrame *f) {
	(void)f;
	Ticks++;
}

// Sets PIT frequency
void PITSetFreq(U16 freq, U8 PIT, U8 mode) {
	if (freq == 0) return;
	if (PIT > 2) return;
	U16 divisor = PITBASEFREQ/freq;
	OutU8(0x43, ((PIT & 3) << 6) | 0b00110000 | ((mode & 7) << 1));
	OutU8(0x40 + PIT, divisor & 0xFF); // Low byte
	OutU8(0x40 + PIT, (divisor >> 8) & 0xFF); // High byte
}

// Initializes PIT with a custom frequency
void PITInit(U16 freq) {
	PITSetFreq(freq, PIT0, PIT_MODE_RATEGEN);
	Irqs[0] = PITHandler;
	PICUnmaskIrq(0);
}

// Returns Timer TICKS
U64 PITGetTicks(void) {
	return Ticks;
}
