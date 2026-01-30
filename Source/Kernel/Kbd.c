/**
 * Kbd.c
 * Created by Matheus Leme Da Silva
 */
#include "Kbd.h"
#include "Types.h"
#include "Io.h"
#include "Pic.h"

struct KbdEvent Events[KBD_BUFFER_SIZE];
U16 Head = 0;
U16 Tail = 0;
U8 CurMod = 0;

// Pushes a new Event in kbd buffer
// Return non zero if error
int KbdPush(struct KbdEvent event) {
	U16 next = (Head + 1) % KBD_BUFFER_SIZE;
	if (next == Tail) return 1;
	Events[Head] = event;
	Head = next;
	return  0;
}

// Pop a Event in kbd buffer
// Return non zero if error
int KbdPop(struct KbdEvent *out) {
	if (Tail == Head) return 1;
	if (out)
		*out = Events[Tail];
	Tail = (Tail + 1) % KBD_BUFFER_SIZE;
	return 0;
}

// PS/2 KBD handler
void KbdHandler(struct IntFrame *f) {
	(void)f;
	U8 sc = InU8(0x60);
	int released = 0;

	if (sc & 0x80) {
		released = 1;
		sc &= ~0x80;
	}

	U32 mod = 0;
	if (sc == 0x38) { // ALT
		mod = KBD_MOD_ALT;
	} else if (sc == 0x1D) {
		mod = KBD_MOD_CTRL;
	} else if (sc == 0x2A) {
		mod = KBD_MOD_SHIFT;
	} else if (sc == 0x5B) {
		mod = KBD_MOD_SUPER;
	}

	if (mod) {
		CurMod = released ? CurMod & ~mod : CurMod | mod;
		return;
	}

	struct KbdEvent ev;
	ev.Modifiers = CurMod;
	ev.Scancode = sc;
	ev.Released = released;

	KbdPush(ev);
}

// Initializes PS/2 Keyboard
void KbdInit(void) {
	Irqs[1] = KbdHandler;
	PicUnmaskIrq(1);
}

