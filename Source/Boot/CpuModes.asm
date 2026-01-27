; CpuModes.asm
; Created by Matheus Leme Da Silva

%macro ToRealMode 0
	JMP WORD 0x18:%%ProtectedMode16
%%ProtectedMode16:
	BITS 16

	MOV EAX, CR0
	AND EAX, 0xFFFFFFFE
	MOV CR0, EAX

	JMP WORD 0x00:%%RealMode
%%RealMode:
	XOR AX, AX
	MOV DS, AX
	MOV ES, AX
	MOV FS, AX
	MOV GS, AX
	MOV SS, AX
	STI
%endmacro

%macro ToProtectedMode 0
	CLI
	MOV EAX, CR0
	OR EAX, 1
	MOV CR0, EAX

	JMP DWORD 0x08:%%ProtectedMode
%%ProtectedMode:
	BITS 32
	MOV AX, 0x10
	MOV DS, AX
	MOV ES, AX
	MOV FS, AX
	MOV GS, AX
	MOV SS, AX
%endmacro
