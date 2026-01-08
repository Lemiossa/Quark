/**
 * entry.S
 * Criado por Matheus Leme Da Silva
 */
.code16
.section .text.entry
.extern kmain

.global _start
_start:
	cli
	xorw %ax, %ax
	movw %ax, %ds
	movw %ax, %es
	movw %ax, %ss
	movw $0xffff, %sp

	inb $0x92, %al
	orb $2, %al
	outb %al, $0x92

	lgdt gdtr

	movl %cr0, %eax
	orb $1, %al
	movl %eax, %cr0

	ljmp $0x08, $_start32

hang:
	jmp hang

.code32
_start32:
	cli
	movw $0x10, %ax
	movw %ax, %ds
	movw %ax, %es
	movw %ax, %fs
	movw %ax, %gs
	movw %ax, %ss
	movl $stack, %esp // Por enquanto a pilha vai crescer abaixo do kerneil

	call kmain

hang32:
	jmp hang32

.align 4
gdt:
	// NULL
	.word 0x0000
	.word 0x0000
	.byte 0x00
	.byte 0x00
	.byte 0x00
	.byte 0x00

	// CODE32
	.word 0xffff
	.word 0x0000
	.byte 0x00
	.byte 0b10011010
	.byte 0b11001111
	.byte 0x00

	// DATA32
	.word 0xffff
	.word 0x0000
	.byte 0x00
	.byte 0b10010010
	.byte 0b11001111
	.byte 0x00

gdt_end:

gdtr:
	.word gdt_end - gdt - 1
	.long gdt

.section .bss
.align 16
stack_bottom: .skip 16384
stack:

