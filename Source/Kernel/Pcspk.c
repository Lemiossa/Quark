/**
 * Pcspk.c
 * Created by Matheus Leme Da Silva
 */
#include "Types.h"
#include "Io.h"
#include "Pit.h"

// Enable PCSPK
void PcspkEnable(void) {
	U8 tmp = InU8(0x61);
	OutU8(0x61, tmp | 3);
}

// Sets PCSPK Frequency
void PcspkSetFreq(U16 freq) {
    if (freq == 0) return;
	PITSetFreq(freq, PIT2, PIT_MODE_SQRWAVGEN);
}

// Turn off PCSPK sound
void PcspkDisable(void) {
	U8 tmp = InU8(0x61);
	OutU8(0x61, tmp & 0xFC);
}
