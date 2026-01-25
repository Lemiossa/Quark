/**
 * Vga.h
 * Created by Matheus Leme Da Silva
 */
#ifndef VGA_H
#define VGA_H

#include "Types.h"

void CursorUpdate(U16 x, U16 y);
void Putcat(U16 x, U16 y, char c);
void Putc(char c);
void Puts(const char *s);

#endif // VGA_H
