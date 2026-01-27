; E820.asm
; Created by Matheus Leme Da Silva
BITS 32
SECTION .text

%include "Source/Boot/CpuModes.asm"

; Gets a E820 Table
; Returns total entries
; U8 E820GetTable(struct E820Entry *out, U8 max);
GLOBAL E820GetTable
E820GetTable:
	PUSH EBP
	MOV EBP, ESP
	MOV EDI, [EBP+8]

	MOV ECX, [EBP+12]

	ToRealMode
	BITS 16

	XOR ESI, ESI
	XOR EBX, EBX

.Loop:
	PUSH ECX
	CMP ESI, ECX
	JAE Error

	PUSH EDI
	; Linear to seg:off
	MOV EAX, EDI
	PUSH EAX
	SHR EAX, 4
	MOV ES, AX
	POP EAX

	AND EAX, 0xF
	MOV DI, AX

	XOR EAX, EAX
	MOV AX, 0xE820
	MOV ECX, 24
	MOV EDX, 'PAMS'

	INT 0x15
	JC Error
	CMP EAX, 'PAMS'
	JNE Error

	INC ESI
	CMP EBX, 0
	JE Success

	POP EDI
	ADD EDI, 24
	POP ECX
	JMP .Loop
Success:
	ToProtectedMode
	BITS 32
	MOV EAX, ESI

	LEAVE
	RET
Error:
	ToProtectedMode
	BITS 32
	MOV EAX, 0

	LEAVE
	RET
