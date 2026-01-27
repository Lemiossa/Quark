; Entry.asm
; Created by Matheus Leme Da Silva
BITS 32
SECTION .text.entry

EXTERN Main

GLOBAL _START
_START:
	CLI
	MOV ESP, STACK_TOP

	PUSH EAX ; The bootloader sets EAX with boot info structure pointer
	CALL Main
	ADD ESP, 4

HANG:
	CLI
	HLT

SECTION .bss
STACK_BOTTOM:
	RESB 65536
STACK_TOP:
