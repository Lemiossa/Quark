/**
 * bootsector.S
 * Criado por Matheus Leme Da Silva
 */
.section .text
.code16

#define START_LBA 1

#ifndef SECTORS_TO_LOAD
#define SECTORS_TO_LOAD 62
#endif

#ifndef LOAD_ADDR
#define LOAD_ADDR 0x7e00
#endif

.global _start
_start:
	cli
	xorw %ax, %ax
	movw %ax, %ds
	movw %ax, %es
	movw %ax, %ss
	movw $0x7c00, %sp
	sti

	movb %dl, boot_drive

	movw $message, %si
	call print_string

	movl $LOAD_ADDR, %ebx
	movw $START_LBA, %si

load_loop:
	cmpw $SECTORS_TO_LOAD, %si
	jg jump

	movb boot_drive, %dl
	movw %si, %ax
	call read_sector
	cmpb $0, %ah
	jne error

	addl $512, %ebx
	incw %si

	pushw %ax
	movb $'.', %al
	movb $0x0e, %ah
	int $0x10
	popw %ax

	jmp load_loop

jump:
	ljmp $(LOAD_ADDR >> 4), $(LOAD_ADDR & 0xf)
	jmp hang

error:
	movw $error_message, %si
	call print_string

hang:
	jmp hang

// Lê um setor em um disco
// LBA = %eax
// DEST = %ebx
// DRIVE = %dl
// Retorna: %ah != 0 em caso de erro
read_sector:
	pushw %si
	pushw %cx
	pushl %ebx

	movl %eax, disk_packet_lba

	// segment
	shrl $4, %ebx
	movw %bx, disk_packet_segment

	// offset
	andl $0xf, %ebx
	movw %bx, disk_packet_offset

	movb $0x42, %ah
	movw $disk_packet, %si
	int $0x13

	popl %ebx
	popw %cx
	popw %si
	ret

// Imprime uma string
// PTR = %si
print_string:
	pushw %si
	pushw %ax

print_char:
	lodsb
	cmpb $0, %al
	je print_string_end
	movb $0x0e, %ah
	int $0x10
	jmp print_char

print_string_end:
	popw %ax
	popw %si
	ret

boot_drive:      .byte 0
message:         .ascii "Loading\0"
error_message:   .ascii "Failed to load\n\r\0"

.align 4
disk_packet:
	disk_packet_size:       .byte 0x10
	disk_packet_reserved:   .byte 0x00
	disk_packet_count:      .word 0x0001
	disk_packet_offset:     .word 0x0000
	disk_packet_segment:    .word 0x0000
	disk_packet_lba:        .quad 0

.org 446
/* Só pro compilador dar erro caso o codigo entre na tabela de partições */
/* Vai ser preenchida com sfdisk depois */
.org 510
.word 0xaa55

