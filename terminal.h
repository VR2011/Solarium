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
void terminal_move_left(void);
void terminal_move_right(void);
void terminal_clear_line_from_cursor(void);
void terminal_clear_current_input(void);
void terminal_write_flame(const char* data);
uint8_t terminal_make_color(enum vga_color fg, enum vga_color bg);
void terminal_put_at(size_t x, size_t y, char c, uint8_t color);
void terminal_set_cursor(size_t x, size_t y);
void terminal_hide_cursor(void);
void terminal_show_cursor(void);
size_t terminal_get_row(void);
void terminal_blink_cursor(void);
void terminal_set_font(const uint8_t *font);
void terminal_begin_batch(void);
void terminal_end_batch(void);

#endif