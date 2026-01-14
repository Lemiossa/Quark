/**
 * int.S
 * Criado por Matheus Leme Da Silva
 */
#include "../../../defs.h"
.extern interrupt_handler

.macro ISR_STUB_NO_ERR num
.global isr_\num
isr_\num:
	pushl $0
	pushl $\num
	jmp isr_common
.endm

.macro ISR_STUB_ERR num
.global isr_\num
isr_\num:
	pushl $\num
	jmp isr_common
.endm

isr_common:
	pusha

	xorl %eax, %eax
	movw %ds, %ax
	pushl %eax

	movw $KERNEL_DATA_SELECTOR, %ax
	movw %ax, %ds
	movw %ax, %es
	movw %ax, %fs
	movw %ax, %gs

	pushl %esp
	call interrupt_handler
	addl $4, %esp

	popl %eax
	movw %ax, %ds
	movw %ax, %es
	movw %ax, %fs
	movw %ax, %gs

	popa
	addl $8, %esp
	iret

ISR_STUB_NO_ERR 0
ISR_STUB_NO_ERR 1
ISR_STUB_NO_ERR 2
ISR_STUB_NO_ERR 3
ISR_STUB_NO_ERR 4
ISR_STUB_NO_ERR 5
ISR_STUB_NO_ERR 6
ISR_STUB_NO_ERR 7
ISR_STUB_ERR 8
ISR_STUB_NO_ERR 9
ISR_STUB_ERR 10
ISR_STUB_ERR 11
ISR_STUB_ERR 12
ISR_STUB_ERR 13
ISR_STUB_ERR 14
ISR_STUB_NO_ERR 15
ISR_STUB_NO_ERR 16
ISR_STUB_ERR 17
ISR_STUB_NO_ERR 18
ISR_STUB_NO_ERR 19
ISR_STUB_NO_ERR 20
ISR_STUB_NO_ERR 21
ISR_STUB_NO_ERR 22
ISR_STUB_NO_ERR 23
ISR_STUB_NO_ERR 24
ISR_STUB_NO_ERR 25
ISR_STUB_NO_ERR 26
ISR_STUB_NO_ERR 27
ISR_STUB_NO_ERR 28
ISR_STUB_NO_ERR 29
ISR_STUB_ERR 30
ISR_STUB_NO_ERR 31

ISR_STUB_NO_ERR 32
ISR_STUB_NO_ERR 33
ISR_STUB_NO_ERR 34
ISR_STUB_NO_ERR 35
ISR_STUB_NO_ERR 36
ISR_STUB_NO_ERR 37
ISR_STUB_NO_ERR 38
ISR_STUB_NO_ERR 39
ISR_STUB_NO_ERR 40
ISR_STUB_NO_ERR 41
ISR_STUB_NO_ERR 42
ISR_STUB_NO_ERR 43
ISR_STUB_NO_ERR 44
ISR_STUB_NO_ERR 45
ISR_STUB_NO_ERR 46
ISR_STUB_NO_ERR 47

