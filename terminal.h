#ifndef TERMINAL_H
#define TERMINAL_H

#include <stddef.h>
#include <stdint.h>

enum vga_color {
    COLOR_BLACK = 0,
    COLOR_BLUE,
    COLOR_GREEN,
    COLOR_CYAN,
    COLOR_RED,
    COLOR_MAGENTA,
    COLOR_BROWN,
    COLOR_LIGHT_GREY,
    COLOR_DARK_GREY,
    COLOR_LIGHT_BLUE,
    COLOR_LIGHT_GREEN,
    COLOR_LIGHT_CYAN,
    COLOR_LIGHT_RED,
    COLOR_LIGHT_MAGENTA,
    COLOR_LIGHT_BROWN,
    COLOR_WHITE,
};

void terminal_initialize(void);
void terminal_setcolor(uint8_t color);
void terminal_putchar(char c);
void terminal_write(const char* data);
void terminal_clear(void);
void terminal_backspace(void);

#endif