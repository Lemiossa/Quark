/**
 * kdefs.h
 * Criado por Matheus Leme Da Silva
 */
#pragma once

#define KERNEL_CODE_SELECTOR 0x08
#define KERNEL_DATA_SELECTOR 0x10

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
