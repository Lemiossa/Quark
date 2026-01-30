/**
 * Kbd.h
 * Created by Matheus Leme Da Silva
 */
#ifndef KBD_H
#define KBD_H
#include "Types.h"

#define KBD_BUFFER_SIZE 256

#define KBD_MOD_ALT 1
#define KBD_MOD_CTRL 2
#define KBD_MOD_SHIFT 4
#define KBD_MOD_SUPER 8

struct KbdEvent {
	U8 Scancode;
	U8 Modifiers;
	U8 Released;
};

int KbdPush(struct KbdEvent event);
int KbdPop(struct KbdEvent *out);
void KbdInit(void);

#endif // KBD_H
