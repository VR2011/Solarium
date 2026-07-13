#include "terminal.h"
#include "framebuffer.h"
#define CHAR_W 8
#define CHAR_H 16
#define MAX_COLS 256
#define MAX_ROWS 128

static uint32_t text_cols = 0;
static uint32_t text_rows = 0;
static size_t terminal_row = 0;
static size_t terminal_column = 0;
static uint8_t terminal_color = 0;
static char cell_char[MAX_ROWS][MAX_COLS];
static uint8_t cell_color_buf[MAX_ROWS][MAX_COLS];
static const uint8_t *current_font = 0;
static int cursor_visible = 0;
static size_t cursor_row = 0;
static size_t cursor_col = 0;
static int blink_phase = 1;
static int batch_depth = 0;

static void maybe_present(void) {
    if(batch_depth == 0) {fb_present();}
}

void terminal_begin_batch(void) {batch_depth++;}

void terminal_end_batch(void) {
    if(batch_depth > 0) {batch_depth--;}
    if(batch_depth == 0) {fb_present();}
}

static const uint8_t vga_palette[16][3] = {{0, 0, 0}, {0, 0, 170}, {0, 170, 0}, {0, 170, 170}, {170, 0, 0}, {170, 0, 170}, {170, 85, 0}, {170, 170, 170}, {85, 85, 85}, {85, 85, 255}, {85, 255, 85}, {85, 255, 255}, {255, 85, 85}, {255, 85, 255}, {255, 255, 85}, {255, 255, 255},};

static inline uint8_t vga_entry_color(enum vga_color fg, enum vga_color bg) {return (uint8_t)(fg | (bg << 4));}

static inline uint8_t blend_channel(uint8_t bg, uint8_t fg, uint8_t coverage) {
    return (uint8_t)(bg + (((int)fg - (int)bg) * (int)coverage) / 255);
}

static void draw_glyph(size_t col, size_t row, char c, uint8_t color, int with_cursor) {
    if(col >= text_cols || row >= text_rows) {return;}
    uint8_t fg = color & 0x0F;
    uint8_t bg = (color >> 4) & 0x0F;
    const uint8_t *fg_rgb = vga_palette[fg];
    const uint8_t *bg_rgb = vga_palette[bg];
    uint32_t px = (uint32_t)col * CHAR_W;
    uint32_t py = (uint32_t)row * CHAR_H;
    const uint8_t *glyph = current_font + (unsigned char)c * (CHAR_W * CHAR_H / 2);
    int show_cursor_bar = with_cursor && blink_phase;

    for(uint32_t y = 0; y < CHAR_H; y++) {
        int cursor_bar_row = show_cursor_bar && (y >= CHAR_H - 2);
        for(uint32_t x = 0; x < CHAR_W; x++) {
            if(cursor_bar_row) {
                fb_put_pixel(px + x, py + y, fg_rgb[0], fg_rgb[1], fg_rgb[2]);
                continue;
            }
            uint32_t pixel_index = y * CHAR_W + x;
            uint8_t packed = glyph[pixel_index / 2];
            uint8_t nibble = (pixel_index % 2 == 0) ? (packed >> 4) : (packed & 0x0F);
            uint8_t coverage = (uint8_t)(nibble * 17);
            uint8_t r = blend_channel(bg_rgb[0], fg_rgb[0], coverage);
            uint8_t g = blend_channel(bg_rgb[1], fg_rgb[1], coverage);
            uint8_t b = blend_channel(bg_rgb[2], fg_rgb[2], coverage);
            fb_put_pixel(px + x, py + y, r, g, b);
        }
    }
}

static void render_cell(size_t col, size_t row) {
    int with_cursor = cursor_visible && row == cursor_row && col == cursor_col;
    draw_glyph(col, row, cell_char[row][col], cell_color_buf[row][col], with_cursor);
}

static void move_cursor_to(size_t col, size_t row) {
    if(cursor_visible) {
        size_t old_row = cursor_row, old_col = cursor_col;
        cursor_row = row;
        cursor_col = col;
        blink_phase = 1;
        render_cell(old_col, old_row);
        render_cell(cursor_col, cursor_row);
    }
    else {
        cursor_row = row;
        cursor_col = col;
    }
}

static void set_cell(size_t col, size_t row, char c) {
    if(col >= text_cols || row >= text_rows) {return;}
    cell_char[row][col] = c;
    cell_color_buf[row][col] = terminal_color;
    render_cell(col, row);
}

static void redraw_all(void) {for(size_t y = 0; y < text_rows; y++) {for(size_t x = 0; x < text_cols; x++) {render_cell(x, y);}}}

static void terminal_scroll(void) {
    for(size_t y = 1; y < text_rows; y++) {
        for(size_t x = 0; x < text_cols; x++) {
            cell_char[y - 1][x] = cell_char[y][x];
            cell_color_buf[y - 1][x] = cell_color_buf[y][x];
        }
    }
    for(size_t x = 0; x < text_cols; x++) {
        cell_char[text_rows - 1][x] = ' ';
        cell_color_buf[text_rows - 1][x] = terminal_color;
    }
    terminal_row = text_rows - 1;
    redraw_all();
}

void terminal_initialize(void) {
    text_cols = fb_width() / CHAR_W;
    text_rows = fb_height() / CHAR_H;
    if(text_cols > MAX_COLS) {text_cols = MAX_COLS;}
    if(text_rows > MAX_ROWS) {text_rows = MAX_ROWS;}
    terminal_row = 0;
    terminal_column = 0;
    terminal_color = vga_entry_color(COLOR_LIGHT_GREY, COLOR_BLACK);
    cursor_visible = 0;
    for(size_t y = 0; y < text_rows; y++) {
        for(size_t x = 0; x < text_cols; x++) {
            cell_char[y][x] = ' ';
            cell_color_buf[y][x] = terminal_color;
        }
    }
    fb_clear(0, 0, 0);
    fb_present();
}

void terminal_setcolor(uint8_t color) {terminal_color = color;}

void terminal_putchar(char c) {
    if(c == '\n') {
        terminal_column = 0;
        terminal_row++;
        if(terminal_row >= text_rows) {terminal_scroll();}
        move_cursor_to(terminal_column, terminal_row);
        maybe_present();
        return;
    }
    set_cell(terminal_column, terminal_row, c);
    terminal_column++;
    if(terminal_column >= text_cols) {
        terminal_column = 0;
        terminal_row++;
        if(terminal_row >= text_rows) {terminal_scroll();}
    }
    move_cursor_to(terminal_column, terminal_row);
    maybe_present();
}

void terminal_write(const char *data) {
    terminal_begin_batch();
    while(*data) {terminal_putchar(*data++);}
    terminal_end_batch();
}

void terminal_clear(void) {
    terminal_row = 0;
    terminal_column = 0;
    for(size_t y = 0; y < text_rows; y++) {
        for(size_t x = 0; x < text_cols; x++) {
            cell_char[y][x] = ' ';
            cell_color_buf[y][x] = terminal_color;
        }
    }
    fb_clear(0, 0, 0);
    move_cursor_to(terminal_column, terminal_row);
    maybe_present();
}

void terminal_backspace(void) {
    if(terminal_column == 0) {
        if(terminal_row == 0) {return;}
        terminal_row--;
        terminal_column = text_cols - 1;
    }
    else {terminal_column--;}
    set_cell(terminal_column, terminal_row, ' ');
    move_cursor_to(terminal_column, terminal_row);
    maybe_present();
}

void terminal_move_left(void) {
    if(terminal_column > 0) {
        terminal_column--;
        move_cursor_to(terminal_column, terminal_row);
        maybe_present();
    }
}

void terminal_move_right(void) {
    if(terminal_column < text_cols - 1) {
        terminal_column++;
        move_cursor_to(terminal_column, terminal_row);
        maybe_present();
    }
}

void terminal_clear_line_from_cursor(void) {
    for(size_t x = terminal_column; x < text_cols; x++) {
        cell_char[terminal_row][x] = ' ';
        cell_color_buf[terminal_row][x] = terminal_color;
        render_cell(x, terminal_row);
    }
    maybe_present();
}

void terminal_clear_current_input(void) {while(terminal_column > 2) {terminal_backspace();}}

void terminal_write_flame(const char *data) {
    static const uint8_t flame_palette[] = {COLOR_RED, COLOR_LIGHT_RED, COLOR_BROWN, COLOR_LIGHT_BROWN, COLOR_LIGHT_RED, COLOR_RED};
    const size_t flame_len = sizeof(flame_palette) / sizeof(flame_palette[0]);
    uint8_t saved_color = terminal_color;
    size_t i = 0;
    terminal_begin_batch();
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
    terminal_end_batch();
}

uint8_t terminal_make_color(enum vga_color fg, enum vga_color bg) {return vga_entry_color(fg, bg);}

void terminal_put_at(size_t x, size_t y, char c, uint8_t color) {
    if(x >= text_cols || y >= text_rows) {return;}
    cell_char[y][x] = c;
    cell_color_buf[y][x] = color;
    render_cell(x, y);
    maybe_present();
}

void terminal_set_cursor(size_t x, size_t y) {
    if(x >= text_cols) {x = text_cols - 1;}
    if(y >= text_rows) {y = text_rows - 1;}
    terminal_column = x;
    terminal_row = y;
    move_cursor_to(terminal_column, terminal_row);
    maybe_present();
}

void terminal_hide_cursor(void) {
    if(cursor_visible) {
        cursor_visible = 0;
        render_cell(cursor_col, cursor_row);
        maybe_present();
    }
}

void terminal_show_cursor(void) {
    cursor_visible = 1;
    blink_phase = 1;
    cursor_row = terminal_row;
    cursor_col = terminal_column;
    render_cell(cursor_col, cursor_row);
    maybe_present();
}

void terminal_blink_cursor(void) {
    if(!cursor_visible) {return;}
    blink_phase = !blink_phase;
    render_cell(cursor_col, cursor_row);
    maybe_present();
}

size_t terminal_get_row(void) {return terminal_row;}

void terminal_set_font(const uint8_t *font) {
    current_font = font;
    redraw_all();
    maybe_present();
}