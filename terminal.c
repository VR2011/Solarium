#include "terminal.h"
#include "io.h"

#define VGA_WIDTH 80
#define VGA_HEIGHT 25
#define VGA_MEMORY ((volatile uint16_t*)0xB8000)

static size_t terminal_row;
static size_t terminal_column;
static uint8_t terminal_color;
static volatile uint16_t* terminal_buffer;

static inline uint8_t vga_entry_color(enum vga_color fg, enum vga_color bg) {
    return fg | bg << 4;
}

static inline uint16_t vga_entry(unsigned char uc, uint8_t color) {
    return(uint16_t) uc | (uint16_t) color << 8;
}

static void terminal_scroll(void) {
    for(size_t y = 1; y < VGA_HEIGHT; y++) {
        for(size_t x = 0; x < VGA_WIDTH; x++) {
            terminal_buffer[(y - 1) * VGA_WIDTH + x] = terminal_buffer[y * VGA_WIDTH + x];
        }
    }
    for(size_t x = 0; x < VGA_WIDTH; x++) {
        terminal_buffer[(VGA_HEIGHT - 1) * VGA_WIDTH + x] = vga_entry(' ', terminal_color);
    }
    terminal_row = VGA_HEIGHT - 1;
}

static void terminal_update_cursor(void) {
    uint16_t position = terminal_row * VGA_WIDTH + terminal_column;

    outb(0x3D4, 0x0F);
    outb(0x3D5, (uint8_t)(position & 0xFF));
    outb(0x3D4, 0x0E);
    outb(0x3D5, (uint8_t)((position >> 8) & 0xFF));
}

void terminal_initialize(void) {
    terminal_row = 0;
    terminal_column = 0;
    terminal_color = vga_entry_color(COLOR_LIGHT_GREY, COLOR_BLACK);
    terminal_buffer = VGA_MEMORY;

    for(size_t y = 0; y < VGA_HEIGHT; y++) {
        for(size_t x = 0; x < VGA_WIDTH; x++) {
            terminal_buffer[y * VGA_WIDTH + x] = vga_entry(' ', terminal_color);
        }
    }
    terminal_update_cursor();
}

void terminal_setcolor(uint8_t color) {
    terminal_color = color;
}

void terminal_putchar(char c) {
    if(c == '\n') {
        terminal_column = 0;
        terminal_row++;
        if(terminal_row >= VGA_HEIGHT) {
            terminal_scroll();
            terminal_update_cursor();

        }
        return;
    }
    terminal_buffer[terminal_row * VGA_WIDTH + terminal_column] = vga_entry(c, terminal_color);
    terminal_column++;
    if(terminal_column >= VGA_WIDTH) {
        terminal_column = 0;
        terminal_row++;
        if(terminal_row >= VGA_HEIGHT) {
            terminal_scroll();
            terminal_update_cursor();
        }
    }
    terminal_update_cursor();
}

void terminal_write(const char *data) {
    while(*data) {
        terminal_putchar(*data++);
    }
}

void terminal_clear(void) {
    terminal_row = 0;
    terminal_column = 0;

    for(size_t y = 0; y < VGA_HEIGHT; y++) {
        for(size_t x = 0; x < VGA_WIDTH; x++) {
            terminal_buffer[y * VGA_WIDTH + x] = vga_entry(' ', terminal_color);
        }
    }
}

void terminal_backspace(void) {
    if(terminal_column == 0) {
        if(terminal_row == 0) {return;}
        terminal_row--;
        terminal_column = VGA_WIDTH - 1;
    }
    else {terminal_column--;}
    
    terminal_buffer[terminal_row * VGA_WIDTH + terminal_column] = vga_entry(' ', terminal_color);
    terminal_update_cursor();
}

void terminal_move_left(void) {
    if(terminal_column > 0) {
        terminal_column--;
        terminal_update_cursor();
    }
}

void terminal_move_right(void) {
    if(terminal_column < VGA_WIDTH - 1) {
        terminal_column++;
        terminal_update_cursor();
    }
}

void terminal_clear_line_from_cursor(void) {
    int x = terminal_column;
    int y = terminal_row;
    for(int i = x; i < VGA_WIDTH; i++) {
        terminal_buffer[y * VGA_WIDTH + i] = vga_entry(' ', terminal_color);
    }
}

void terminal_clear_current_input(void) {
    while(terminal_column > 2) {terminal_backspace();}
    terminal_update_cursor();
}

void terminal_write_flame(const char *data) {
    static const uint8_t flame_palette[] = {COLOR_RED, COLOR_LIGHT_RED, COLOR_BROWN, COLOR_LIGHT_BROWN, COLOR_LIGHT_RED, COLOR_RED};
    const size_t flame_len = sizeof(flame_palette) / sizeof(flame_palette[0]);
    uint8_t saved_color = terminal_color;
    size_t i = 0;
    while(*data) {
        if(*data == '\n') {
            terminal_putchar(*data++);
            continue;
        }
        terminal_color = vga_entry_color(flame_palette[i % flame_len], COLOR_BLACK);
        terminal_putchar(*data++);
        i++;
    }
    terminal_color = saved_color;
}