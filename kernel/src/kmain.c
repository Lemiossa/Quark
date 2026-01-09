/**
 * kmain.c
 * Criado por Matheus Leme Da Silva
 */
#include "stdint.h"
#include "terminal.h"
#include "graphics.h"

const char *hex_chars = "0123456789abcdef";

// Imprime um byte hexadecimal
void print_hex_byte(u8 value) {
	terminal_putchar(hex_chars[(value >> 4) & 0xf]);
	terminal_putchar(hex_chars[value & 0xf]);
}

// Imprime um word hexadecimal
void print_hex_word(u16 value) {
	print_hex_byte((value >> 8) & 0xff);
	print_hex_byte(value & 0xff);
}

// Imprime um dword hexadecimal
void print_hex_dword(u32 value) {
	print_hex_word((value >> 16) & 0xffff);
	print_hex_word(value & 0xffff);
}

// Imprime um qword hexadecimal
void print_hex_qword(u64 value) {
	print_hex_word((value >> 48) & 0xffff);
	print_hex_word((value >> 32) & 0xffff);
	print_hex_word((value >> 16) & 0xffff);
	print_hex_word(value & 0xffff);
}

typedef struct regs {
	u32 ds;
	u32 edi, esi, ebp, esp, ebx, edx, ecx, eax;
	u32 int_no, err_code;
	u32 eip, cs, eflags, usersp, ss;
} regs_t;

// Dá um panic no kernel
void panic(char *str, regs_t regs) {
	terminal_set_color(VGA_WHITE, VGA_RED);
	terminal_clear();
	terminal_putstring("=== KERNEL PANIC! ===\n");

	terminal_putstring("Message: ");
	terminal_putstring(str);

	terminal_putstring("\n\n");

	terminal_putstring("eax = ");
	print_hex_dword(regs.eax);
	terminal_putchar('\n');

	terminal_putstring("ebx = ");
	print_hex_dword(regs.ebx);
	terminal_putchar('\n');

	terminal_putstring("ecx = ");
	print_hex_dword(regs.ecx);
	terminal_putchar('\n');

	terminal_putstring("edx = ");
	print_hex_dword(regs.edx);
	terminal_putchar('\n');

	terminal_putstring("esi = ");
	print_hex_dword(regs.esi);
	terminal_putchar('\n');

	terminal_putstring("edi = ");
	print_hex_dword(regs.edi);
	terminal_putchar('\n');

	terminal_putstring("ebp = ");
	print_hex_dword(regs.ebp);
	terminal_putchar('\n');

	terminal_putstring("esp = ");
	print_hex_dword(regs.esp);
	terminal_putchar('\n');

	terminal_set_color(VGA_YELLOW, VGA_RED);

	terminal_putstring("#####   #####  ###   ## ##   #####\n");
	terminal_putstring("##   # ##   ## ## #  ## ## ##    ##\n");
	terminal_putstring("#####  ####### ##  # ## ## ##\n");
	terminal_putstring("##     ##   ## ##   ### ## ##    ##\n");
	terminal_putstring("##     ##   ## ##    ## ##   #####\n");

	__asm__ volatile("cli");
	for (;;)
		__asm__ volatile("hlt");
}

// Dá um kernel panic
void kernel_panic(char *str) {
	regs_t regs = {0};

	panic(str, regs);
}

// Func principal do kernel
void kmain(void) {
	terminal_init();
	terminal_set_color(VGA_LIGHT_GRAY, VGA_BLACK);
	terminal_clear();

	terminal_putstring("Hello World");

	for (;;)
		__asm__ volatile("hlt");
}
