/**
 * entry.S
 * Criado por Matheus Leme Da Silva
 */
.code16
.section .text.entry
.extern kmain

#include "defs.h"

#ifndef KERNEL_STACK_SIZE
#error "Need KERNEL_STACL_SIZE"
#endif

.global _start
_start:
	cli
	xorw %ax, %ax
	movw %ax, %ds
	movw %ax, %es
	movw %ax, %ss
	movw $0xffff, %sp

	// Preparar algumas coisas antes do modo protegido
	movw $0x1112, %ax
	int $0x10

	call font
#ifdef KERNEL_VESA_MODE
	call vbe
#endif
	call e820

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

font:
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
	ret

.global bios_font_segment
.global bios_font_offset
bios_font_segment: .word 0
bios_font_offset: .word 0

#ifdef KERNEL_VESA_MODE
vbe:
	movw $0x4f01, %ax
	movw $KERNEL_VESA_MODE, %cx
	movw $vbe_mode_info, %di

	int $0x10
	cmpw $0x4f, %ax
	je vbe_ok
vbe_unsupported:
	movb $0, vbe_supported
	jmp after_vbe
vbe_ok:
	movw $0x4f02, %ax
	movw $KERNEL_VESA_MODE | 0x4000, %bx
	int $0x10
	movb $1, vbe_supported
after_vbe:
	ret
#endif

.global vbe_mode_info
vbe_mode_info: .space 256
.global vbe_supported
vbe_supported: .byte 0

#ifndef KERNEL_E820_LOC
#error "Need KERNEL_E820_LOC"
#endif

#define E820M 0x534d4150
#define E820_SEG (KERNEL_E820_LOC >> 4)
#define E820_OFF (KERNEL_E820_LOC & 0xf)
e820:
	movw %es, %ax
	pushw %ax
	movw $E820_SEG, %ax
	movw %ax, %es

	movw $E820_OFF, %di
	xorl %ebx, %ebx
0:
	movl $E820M, %edx
	movl $0xe820, %eax
	movl $24, %ecx
	int $0x15
	jc e820_error
	cmpl $E820M, %eax
	jne e820_error
	addw $24, %di
	incb e820_table_count
	cmpl $0, %ebx
	je e820_end
	jmp 0b
e820_error:
	movb $0, e820_table_count
e820_end:
	popw %ax
	movw %ax, %es
	ret

.global e820_table_count
e820_table_count: .byte 0

.extern bss_start
.extern bss_end

.code32
_start32:
	cli
	movw $0x10, %ax
	movw %ax, %ds
	movw %ax, %es
	movw %ax, %fs
	movw %ax, %gs
	movw %ax, %ss
	movl $stack, %esp

  movl $bss_start, %edi
  movl $bss_end, %ecx
  subl %edi, %ecx
  movb $0, %al
  rep stosb

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

	// KERNEL CODE32
	.word 0xffff
	.word 0x0000
	.byte 0x00
	.byte 0b10011010
	.byte 0b11000000 | 0xf
	.byte 0x00

	// KERNEL DATA32
	.word 0xffff
	.word 0x0000
	.byte 0x00
	.byte 0b10010010
	.byte 0b11000000 | 0xf
	.byte 0x00

gdt_end:

gdtr:
	.word gdt_end - gdt - 1
	.long gdt

.section .bss
.align 16
stack_bottom: .skip KERNEL_STACK_SIZE
stack:

