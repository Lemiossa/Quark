/**
 * Main.c
 * Created by Matheus Leme Da Silva
 */
#include "Gdt.h"
#include "Vga.h"

// The Main function don't return
void Main(void) {
  GdtInit();
  Puts("Hello Kernel!\r\n");

  for (;;)
    ;
}
