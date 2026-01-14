/**
 * graphics.c
 * Criado por Matheus Leme Da Silva
 */
#include <stdint.h>

#include <graphics.h>

// https://wiki.osdev.org/VESA_Video_Modes
struct vbe_mode_info_structure {
  U16 attributes; // deprecated, only bit 7 should be of interest to you, and it
                  // indicates the mode supports a linear frame buffer.
  U8 window_a;    // deprecated
  U8 window_b;    // deprecated
  U16 granularity; // deprecated; used while calculating bank numbers
  U16 window_size;
  U16 segment_a;
  U16 segment_b;
  U32 win_func_ptr; // deprecated; used to switch banks from protected mode
                    // without returning to real mode
  U16 pitch;        // number of bytes per horizontal line
  U16 width;        // width in pixels
  U16 height;       // height in pixels
  U8 w_char;        // unused...
  U8 y_char;        // ...
  U8 planes;
  U8 bpp;   // bits per pixel in this mode
  U8 banks; // deprecated; total number of banks in this mode
  U8 memory_model;
  U8 bank_size; // deprecated; size of a bank, almost always 64 KB but may be 16
                // KB...
  U8 image_pages;
  U8 reserved0;

  U8 red_mask;
  U8 red_position;
  U8 green_mask;
  U8 green_position;
  U8 blue_mask;
  U8 blue_position;
  U8 reserved_mask;
  U8 reserved_position;
  U8 direct_color_attributes;

  U32 framebuffer; // physical address of the linear frame buffer; write here to
                   // draw to the screen
  U32 off_screen_mem_off;
  U16 off_screen_mem_size; // size of memory in the framebuffer but not being
                           // displayed on the screen
  U8 reserved1[206];
} __attribute__((packed));

extern struct vbe_mode_info_structure vbe_mode_info;

// Coloca um pixel no modo gráfico
void put_pixel(int x, int y, U8 color) {
  if (!vbe_supported)
    return;

  U8 *vram = (U8 *)vbe_mode_info.framebuffer;
#ifndef KERNEL_VESA_MODE
  (void)x;
  (void)y;
  (void)color;
  (void)vram;
#endif
#ifdef KERNEL_VESA_MODE
  vram[y * VIDEO_MODE_WIDTH + x] = color;
#endif
}
