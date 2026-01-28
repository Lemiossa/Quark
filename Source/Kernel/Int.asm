; Int.asm
; Created by Matheus Leme Da Silva
BITS 32
SECTION .text
EXTERN InterruptHandler

%macro IsrErr 1
GLOBAL Isr%1
Isr%1:
	PUSH DWORD %1
	JMP IsrCommon
%endmacro

%macro IsrNoErr 1
GLOBAL Isr%1
Isr%1:
	PUSH DWORD 0
	PUSH DWORD %1
	JMP IsrCommon
%endmacro

IsrCommon:
	PUSHAD

	PUSH GS
	PUSH FS
	PUSH ES
	PUSH DS

	MOV EAX, CR3
	PUSH EAX

	MOV EAX, CR2
	PUSH EAX

	MOV AX, 0x10
	MOV DS, AX
	MOV ES, AX
	MOV FS, AX
	MOV GS, AX

	PUSH ESP
	CALL InterruptHandler
	ADD ESP, 4 ; Esp

	POP EAX
	MOV CR2, EAX

	POP EAX
	MOV CR3, EAX

	POP DS
	POP ES
	POP FS
	POP GS

	POPAD
	ADD ESP, 8 ; Error code & Int num
	IRET

IsrNoErr 0
IsrNoErr 1
IsrNoErr 2
IsrNoErr 3
IsrNoErr 4
IsrNoErr 5
IsrNoErr 6
IsrNoErr 7
IsrErr 8
IsrNoErr 9
IsrErr 10
IsrErr 11
IsrErr 12
IsrErr 13
IsrErr 14
IsrNoErr 15
IsrNoErr 16
IsrErr 17
IsrNoErr 18
IsrNoErr 19
IsrNoErr 20
IsrErr 21
IsrNoErr 22
IsrNoErr 23
IsrNoErr 24
IsrNoErr 25
IsrNoErr 26
IsrNoErr 27
IsrNoErr 28
IsrNoErr 29
IsrNoErr 30
IsrNoErr 31

IsrNoErr 32
IsrNoErr 33
IsrNoErr 34
IsrNoErr 35
IsrNoErr 36
IsrNoErr 37
IsrNoErr 38
IsrNoErr 39
IsrNoErr 40
IsrNoErr 41
IsrNoErr 42
IsrNoErr 43
IsrNoErr 44
IsrNoErr 45
IsrNoErr 46
IsrNoErr 47
