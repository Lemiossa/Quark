; Disk.asm
; Created by Matheus Leme Da Silva
BITS 32
SECTION .text

%INCLUDE "Source/Boot/CpuModes.asm"

; Resets the disk controller
; void DiskReset(U8 drive);
GLOBAL DiskReset
DiskReset:
	PUSH EBP
	MOV EBP, ESP

	ToRealMode
	BITS 16

	MOV DL, [EBP + 8]
	MOV AH, 0
	INT 0x13

	ToProtectedMode
	BITS 32

	POP EBP
	RET

; Get drive parameters
; Returns SPT | HPC
; U16 GetDriveParameters(U8 drive);
GLOBAL GetDriveParameters
GetDriveParameters:
	PUSH EBP
	MOV EBP, ESP

	PUSHAD
	ToRealMode
	BITS 16

	XOR AX, AX
	MOV ES, AX
	MOV DI, AX

	MOV DL, [EBP + 8]
	MOV AH, 0x08
	INT 0x13

	AND CL, 0x3F

	INC DH
	MOV AH, CL
	MOV AL, DH
	MOV [.RET], AX

	ToProtectedMode
	BITS 32
	POPAD

	XOR EAX, EAX
	MOV AX, [.RET]

	POP EBP
	RET
.RET: DW 0

; Read disk using EDD
; Returns error code(0 == no error)
; U8 ExtendedDiskRead(U8 drive, U32 lba, U16 count, void *buffer);
GLOBAL ExtendedDiskRead
ExtendedDiskRead:
	PUSH EBP
	MOV EBP, ESP

	MOV ECX, [EBP+12]
	MOV [DISK_PACKET.LBA_LOW], ECX
	MOV CX, [EBP+16]
	MOV [DISK_PACKET.SECTORS], CX

	MOV EDI, [EBP+20]

	PUSH EDI
	AND EDI, 0xF
	MOV [DISK_PACKET.OFFSET], DI
	POP EDI

	PUSH EDI
	SHR EDI, 4
	MOV [DISK_PACKET.SEGMENT], DI
	POP EDI


	MOV DL, [EBP + 8]

	PUSHAD
	ToRealMode
	BITS 16

	PUSH DS
	PUSH DI

	MOV AH, 0x42
	MOV SI, DISK_PACKET
	INT 0x13
	MOV [.RET], AH

	POP DI
	POP DS

	ToProtectedMode
	BITS 32
	POPAD

	XOR EAX, EAX
	MOV AL, [.RET]

	POP EBP
	RET
.RET: DB 0

DISK_PACKET:
	.SIZE: DB 0x10
	.ZERO: DB 0
	.SECTORS: DW 1
	.OFFSET: DW 0
	.SEGMENT: DW 0
	.LBA_LOW: DD 0
	.LBA_HIGH: DD 0

