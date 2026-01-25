/**
 * Main.c
 * Created by Matheus Leme Da Silva
 */
#include "Gdt.h"
#include "Pic.h"
#include "Vga.h"

// The Main function don't return
void Main(void) {
  GdtInit();
  PicRemap(0x20, 0x28);
  Puts("Hello Kernel!\r\n");

  for (;;)
    ;
}
