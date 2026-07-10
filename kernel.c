#include "terminal.h"
#include "shell.h"
#include "delay.h"
#include "keyboard.h"
#include "vgagraphics.h"
#include "logo_mode13.h"
#include "fonts.h"

#define SPLASH_SECONDS 3

void kernel_main(void) {
    vga_enter_mode13h();
    vga_set_palette(logo13_palette);
    vga_blit(logo13_pixels);
    delay_seconds(SPLASH_SECONDS);
    vga_exit_mode13h();
    vga_load_font(DEFAULT_FONT);
    terminal_initialize();
    terminal_hide_cursor();
    keyboard_flush();
    terminal_set_cursor(0, 0);
    terminal_show_cursor();
    terminal_write("Solarium\n");
    terminal_write("Version 1.2\n");
    terminal_write("Enter 'help' for a list of commands.\n");
    shell_run();
    while(1) {}
}