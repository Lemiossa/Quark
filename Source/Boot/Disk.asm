; Disk.asm
; Created by Matheus Leme Da Silva
BITS 32
SECTION .text

%INCLUDE "Source/Boot/CpuModes.asm"

; Resets the disk controller
; void BiosDiskReset(U8 drive);
GLOBAL BiosDiskReset
BiosDiskReset:
	PUSH EBP
	MOV EBP, ESP

	ToRealMode
	BITS 16

	MOV DL, [EBP + 8]
	MOV AH, 0
	INT 0x13

	ToProtectedMode
	BITS 32

	LEAVE
	RET

; Get drive parameters
; Returns SPT | HPC
; U16 BiosGetDriveParameters(U8 drive);
GLOBAL BiosGetDriveParameters
BiosGetDriveParameters:
	PUSH EBP
	MOV EBP, ESP

	MOV DL, [EBP + 8]
	PUSHAD
	ToRealMode
	BITS 16

	PUSH ES
	PUSH DI

	XOR AX, AX
	MOV ES, AX
	MOV DI, AX

	MOV AH, 0x08
	INT 0x13

	AND CL, 0x3F
	INC DH

	MOV AL, CL
	MOV AH, DH
	MOV [.RET], AX

	POP DI
	POP ES

	ToProtectedMode
	BITS 32
	POPAD

	XOR EAX, EAX
	MOV AX, [.RET]

	LEAVE
	RET
.RET: DW 0

; Read disk using BIOS
; Returns error code(0 == no error)
; U8 BiosDiskRead(U8 drive, U16 cylinder, U8 head, U8 sector, U8 count, void *buffer);
GLOBAL BiosDiskRead
BiosDiskRead:
	PUSH EBP
	MOV EBP, ESP

	PUSHAD
	ToRealMode
	BITS 16

	PUSH BX
	PUSH ES

	; Linear addr to SEG:OFF
	MOV EDI, [EBP+28]
	PUSH EDI
	SHR EDI, 4
	MOV ES, DI
	POP EDI

	AND EDI, 0xF
	MOV BX, DI

	; Read
	MOV AX, [EBP+12]
	MOV CH, AL
	MOV CL, AH
	SHL CL, 6
	AND CL, 0xC0
	OR CL, [EBP+20]
	MOV DH, [EBP+16]

	MOV DL, [EBP+8]
	MOV AH, 0x02
	MOV AL, [EBP+24]
	INT 0x13

	POP ES
	POP BX

	MOV [.RET], AH

	ToProtectedMode
	BITS 32
	POPAD

	XOR EAX, EAX
	MOV AL, [.RET]

	LEAVE
	RET
.RET: DB 0
