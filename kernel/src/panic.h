/**
 * panic.h
 * Criado por Matheus Leme Da Silva
 */
#pragma once
#include "idt.h"
#include "stdint.h"

void panic(char *str, struct regs regs);
