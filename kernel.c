#include "terminal.h"
#include "shell.h"
#include "delay.h"
#include "keyboard.h"
#include "vgagraphics.h"
#include "framebuffer.h"
#include "logo_fb.h"
#include "fonts.h"

#define SPLASH_SECONDS 3

static void draw_boot_logo(void) {
    uint32_t x_offset = (fb_width() > LOGO_FB_WIDTH) ? (fb_width() - LOGO_FB_WIDTH) / 2 : 0;
    uint32_t y_offset = (fb_height() > LOGO_FB_HEIGHT) ? (fb_height() - LOGO_FB_HEIGHT) / 2 : 0;
    for(uint32_t y = 0; y < LOGO_FB_HEIGHT; y++) {
        for(uint32_t x = 0; x < LOGO_FB_WIDTH; x++) {
            const uint8_t *p = logo_fb_pixels[y * LOGO_FB_WIDTH + x];
            fb_put_pixel(x_offset + x, y_offset + y, p[0], p[1], p[2]);
        }
    }
}

void kernel_main(uint32_t multiboot_info_addr) {
    if(!fb_init(multiboot_info_addr)) {
        while(1) {__asm__ volatile("hlt");}
    }
    fb_clear(0, 0, 0);
    draw_boot_logo();
    fb_present();
    delay_seconds(SPLASH_SECONDS);
    vga_load_font(DEFAULT_FONT);
    terminal_initialize();
    terminal_hide_cursor();
    keyboard_flush();
    terminal_set_cursor(0, 0);
    terminal_show_cursor();
    terminal_write("Solarium\n");
    terminal_write("Version 2.0\n");
    terminal_write("Enter 'help' for a list of commands.\n");
    shell_run();
    while(1) {__asm__ volatile("hlt");}
}