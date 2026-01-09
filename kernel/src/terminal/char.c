/**
 * char.c
 * Criado por Matheus Leme Da Silva
 */
#include "../stdint.h"
#include "../graphics.h"
#include "../terminal.h"
#include "char.h"

// Desenha um caractere no modo grafico
void draw_char(int x, int y, char c, u8 fg, u8 bg) {
	if (!font) return;

	u8 uc = (u8)c;
	u8 *glyph = font + (uc * CHAR_HEIGHT);
	for (int i = 0; i < CHAR_HEIGHT; i++) {
		u8 line = glyph[i];
		for (int j = 0; j < CHAR_WIDTH; j++) {
			if (line & (0x80 >> j)) {
				put_pixel((x * CHAR_WIDTH) + j, (y * CHAR_HEIGHT) + i, fg);
			} else {
				put_pixel((x * CHAR_WIDTH) + j, (y * CHAR_HEIGHT) + i, bg);
			}
		}
	}
}
