/**
 * entry.S
 * Criado por Matheus Leme Da Silva
 */
.code16
.section .text.entry
.extern kmain

#include "kdefs.h"

.global _start
_start:
	cli
	xorw %ax, %ax
	movw %ax, %ds
	movw %ax, %es
	movw %ax, %ss
	movw $0xffff, %sp

	// Preparar algumas coisas antes do modo protegido
	// COMEÇO FONTE
	pushw %bp
	movw %es, %ax
	pushw %ax

	movw $0x1130, %ax
	movb $KERNEL_FONT, %bh
	int $0x10

	movw %es, bios_font_segment
	movw %bp, bios_font_offset

	popw %ax
	movw %ax, %es
	popw %bp
	// FIM FONTE

#ifndef KERNEL_VESA_MODE
	jmp vbe_unsupported
#endif

	// COMEÇO VBE MODE
	movw $0x4f01, %ax
	movw $KERNEL_VESA_MODE, %cx
	movw $vbe_mode_info, %di
	int $0x10
	cmpw $0x4f, %ax
	je vbe_ok
vbe_unsupported:
	movb $0, vbe_supported
	movw $0x1112, %ax
	int $0x10
	jmp after_vbe
vbe_ok:
	movw $0x4f02, %ax
	movw $KERNEL_VESA_MODE | 0x4000, %bx
	int $0x10
after_vbe:
	// FIM VBE MODE

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

.global bios_font_segment
.global bios_font_offset
bios_font_segment: .word 0
bios_font_offset: .word 0

.global vbe_mode_info
vbe_mode_info: .space 256
.global vbe_supported
vbe_supported: .byte 1

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

