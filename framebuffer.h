#ifndef FRAMEBUFFER_H
#define FRAMEBUFFER_H
#include <stdint.h>
#include <stddef.h>

int fb_init(uint32_t multiboot_info_addr);
uint32_t fb_width(void);
uint32_t fb_height(void);
void fb_put_pixel(uint32_t x, uint32_t y, uint8_t r, uint8_t g, uint8_t b);
void fb_fill_rect(uint32_t x, uint32_t y, uint32_t w, uint32_t h, uint8_t r, uint8_t g, uint8_t b);
void fb_clear(uint8_t r, uint8_t g, uint8_t b);
void fb_present(void);

#endif