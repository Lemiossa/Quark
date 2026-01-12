/**
 * kdefs.h
 * Criado por Matheus Leme Da Silva
 */
#ifndef KDEFS_H
#define KDEFS_H

#define KERNEL_STACK_SIZE 16384

#define KERNEL_CODE_SELECTOR 0x08
#define KERNEL_DATA_SELECTOR 0x10

// https://wiki.osdev.org/VESA_Video_Modes
// Modo VESA que o kernel vai colocar
// MODE    RESOLUTION  BITS PER PIXEL  MAXIMUM COLORS
// 0x0100  640x400     8               256
// 0x0101  640x480     8               256
// 0x0103  800x600     8               256
// Só modos de 256 cores suportados
#define KERNEL_VESA_MODE 0x101

// Fonte que vai pegar da BIOS
// 0x03 = 8x8, 0x06 = 8x16
#define KERNEL_FONT 0x03

// Aonde a tabela de memoria do E820 vai ficar temporariamente
#define E820_LOC 0x80000

// Frequencia do PIT do kernel
#define PIT_FREQ 100

// Intervalo de blink do cursor
#define CURSOR_BLINK_MS 500

#endif // KDEFS_H
