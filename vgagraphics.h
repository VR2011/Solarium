#ifndef VGA_GRAPHICS_H
#define VGA_GRAPHICS_H
#include <stdint.h>
#define VGA13_WIDTH 320
#define VGA13_HEIGHT 200

void vga_enter_mode13h(void);
void vga_exit_mode13h(void);
void vga_set_palette(const uint8_t palette[256][3]);
void vga_blit(const uint8_t *pixels);
void vga_save_font(void);
void vga_restore_font(void);
void vga_load_font(const uint8_t *font);

#endif