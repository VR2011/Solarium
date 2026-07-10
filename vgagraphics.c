#include "vgagraphics.h"
#include "io.h"
#define VGA13_MEMORY ((volatile uint8_t*)0xA0000)
#define FONT_CHARS 256
#define FONT_HEIGHT 16

static const uint8_t mode_13h_registers[61] = {
    0x63,
    0x03,
    0x01,
    0x0F,
    0x00,
    0x0E,
    0x5F,
    0x4F,
    0x50,
    0x82,
    0x54,
    0x80,
    0xBF,
    0x1F,
    0x00,
    0x41,
    0x00,
    0x00,
    0x00,
    0x00,
    0x00,
    0x00,
    0x9C,
    0x0E,
    0x8F,
    0x28,
    0x40,
    0x96,
    0xB9,
    0xA3,
    0xFF,
    0x00,
    0x00,
    0x00,
    0x00,
    0x00,
    0x40,
    0x05,
    0x0F,
    0xFF,
    0x00,
    0x01,
    0x02,
    0x03,
    0x04,
    0x05,
    0x06,
    0x07,
    0x08,
    0x09,
    0x0A,
    0x0B,
    0x0C,
    0x0D,
    0x0E,
    0x0F,
    0x41,
    0x00,
    0x0F,
    0x00,
    0x00
};

static const uint8_t mode_03h_registers[61] = {
    0x67,
    0x03,
    0x00,
    0x03,
    0x00,
    0x02,
    0x5F,
    0x4F,
    0x50,
    0x82,
    0x55,
    0x81,
    0xBF,
    0x1F,
    0x00,
    0x4F,
    0x0D,
    0x0E,
    0x00,
    0x00,
    0x00,
    0x00,
    0x9C,
    0x8E,
    0x8F,
    0x28,
    0x1F,
    0x96,
    0xB9,
    0xA3,
    0xFF,
    0x00,
    0x00,
    0x00,
    0x00,
    0x00,
    0x10,
    0x0E,
    0x00,
    0xFF,
    0x00,
    0x01,
    0x02,
    0x03,
    0x04,
    0x05,
    0x06,
    0x07,
    0x08,
    0x09,
    0x0A,
    0x0B,
    0x0C,
    0x0D,
    0x0E,
    0x0F,
    0x0C,
    0x00,
    0x0F,
    0x08,
    0x00
};

static void vga_write_registers(const uint8_t *regs_in) {
    uint8_t regs[61];
    for(uint8_t i = 0; i < 61; i++) {regs[i] = regs_in[i];}
    uint8_t *p = regs;
    outb(0x3C2, *p);
    p++;
    for(uint8_t i = 0; i < 5; i++) {
        outb(0x3C4, i);
        outb(0x3C5, *p);
        p++;
    }
    outb(0x3D4, 0x03);
    outb(0x3D5, (uint8_t)(inb(0x3D5) | 0x80));
    outb(0x3D4, 0x11);
    outb(0x3D5, (uint8_t)(inb(0x3D5) & 0x7F));
    p[0x03] |= 0x80;
    p[0x11] &= 0x7F;
    for(uint8_t i = 0; i < 25; i++) {
        outb(0x3D4, i);
        outb(0x3D5, *p);
        p++;
    }
    for(uint8_t i = 0; i < 9; i++) {
        outb(0x3CE, i);
        outb(0x3CF, *p);
        p++;
    }
    for(uint8_t i = 0; i < 21; i++) {
        (void)inb(0x3DA);
        outb(0x3C0, i);
        outb(0x3C0, *p);
        p++;
    }
    (void)inb(0x3DA);
    outb(0x3C0, 0x20);
}
 
void vga_enter_mode13h(void) {vga_write_registers(mode_13h_registers);}
 
static const uint8_t default_text_palette[16][3] = {
    { 0,  0,  0}, { 0,  0, 42}, { 0, 42,  0}, { 0, 42, 42},
    {42,  0,  0}, {42,  0, 42}, {42, 21,  0}, {42, 42, 42},
    {21, 21, 21}, {21, 21, 63}, {21, 63, 21}, {21, 63, 63},
    {63, 21, 21}, {63, 21, 63}, {63, 63, 21}, {63, 63, 63},
};
 
static void vga_restore_default_palette(void) {
    outb(0x3C8, 0);
    for(uint8_t i = 0; i < 16; i++) {
        outb(0x3C9, default_text_palette[i][0]);
        outb(0x3C9, default_text_palette[i][1]);
        outb(0x3C9, default_text_palette[i][2]);
    }
}
 
void vga_exit_mode13h(void) {
    vga_write_registers(mode_03h_registers);
    vga_restore_default_palette();
}
 
void vga_set_palette(const uint8_t palette[256][3]) {
    outb(0x3C8, 0);
    for(uint16_t i = 0; i < 256; i++) {
        outb(0x3C9, palette[i][0]);
        outb(0x3C9, palette[i][1]);
        outb(0x3C9, palette[i][2]);
    }
}
 
void vga_blit(const uint8_t *pixels) {for(uint32_t i = 0; i < (uint32_t)VGA13_WIDTH * VGA13_HEIGHT; i++) {VGA13_MEMORY[i] = pixels[i];}}

static uint8_t saved_font[FONT_CHARS * FONT_HEIGHT];

static void vga_plane2_access_begin(void) {
    outb(0x3C4, 0x00);
    outb(0x3C5, 0x01);
    outb(0x3C4, 0x02);
    outb(0x3C5, 0x04);
    outb(0x3C4, 0x04);
    outb(0x3C5, 0x07);
    outb(0x3CE, 0x04);
    outb(0x3CF, 0x02);
    outb(0x3CE, 0x05);
    outb(0x3CF, 0x00);
    outb(0x3CE, 0x06);
    outb(0x3CF, 0x00);
}
 
static void vga_plane2_access_end(void) {
    outb(0x3C4, 0x00);
    outb(0x3C5, 0x01);
    outb(0x3C4, 0x02);
    outb(0x3C5, 0x03);
    outb(0x3C4, 0x04);
    outb(0x3C5, 0x02);
    outb(0x3C4, 0x00);
    outb(0x3C5, 0x03);
    outb(0x3CE, 0x04);
    outb(0x3CF, 0x00);
    outb(0x3CE, 0x05);
    outb(0x3CF, 0x10);
    outb(0x3CE, 0x06);
    outb(0x3CF, 0x0E);
}

void vga_save_font(void) {
    vga_plane2_access_begin();
    volatile uint8_t *src = VGA13_MEMORY;
    for(uint16_t c = 0; c < FONT_CHARS; c++) {for(uint8_t row = 0; row < FONT_HEIGHT; row++) {saved_font[c * FONT_HEIGHT + row] = src[c * 32 + row];}}
    vga_plane2_access_end();
}

void vga_restore_font(void) {
    vga_plane2_access_begin();
    volatile uint8_t *dst = VGA13_MEMORY;
    for(uint16_t c = 0; c < FONT_CHARS; c++) {for(uint8_t row = 0; row < FONT_HEIGHT; row++) {dst[c * 32 + row] = saved_font[c * FONT_HEIGHT + row];}}
    vga_plane2_access_end();
}

void vga_load_font(const uint8_t *font) {
    vga_plane2_access_begin();
    volatile uint8_t *dst = VGA13_MEMORY;
    for(uint16_t c = 0; c < FONT_CHARS; c++) {for(uint8_t row = 0; row < FONT_HEIGHT; row++) {dst[c * 32 + row] = font[c * FONT_HEIGHT + row];}}
    vga_plane2_access_end();
}