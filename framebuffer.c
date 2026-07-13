#include "framebuffer.h"
#define MULTIBOOT_FLAG_FRAMEBUFFER 0x1000
#define FRAMEBUFFER_TYPE_RGB 1
#define BACKBUFFER_MAX_BYTES (1920u * 1080u * 4u)

typedef struct {
    uint32_t flags;
    uint32_t mem_lower;
    uint32_t mem_upper;
    uint32_t boot_device;
    uint32_t cmdline;
    uint32_t mods_count;
    uint32_t mods_addr;
    uint32_t syms[4];
    uint32_t mmap_length;
    uint32_t mmap_addr;
    uint32_t drives_length;
    uint32_t drives_addr;
    uint32_t config_table;
    uint32_t boot_loader_name;
    uint32_t apm_table;
    uint32_t vbe_control_info;
    uint32_t vbe_mode_info;
    uint16_t vbe_mode;
    uint16_t vbe_interface_seg;
    uint16_t vbe_interface_off;
    uint16_t vbe_control_len;
    uint64_t framebuffer_addr;
    uint32_t framebuffer_pitch;
    uint32_t framebuffer_width;
    uint32_t framebuffer_height;
    uint8_t framebuffer_bpp;
    uint8_t framebuffer_type;
    uint8_t framebuffer_red_field_position;
    uint8_t framebuffer_red_mask_size;
    uint8_t framebuffer_green_field_position;
    uint8_t framebuffer_green_mask_size;
    uint8_t framebuffer_blue_field_position;
    uint8_t framebuffer_blue_mask_size;
} __attribute__((packed)) multiboot_info_t;

static volatile uint8_t *fb_ptr = 0;
static uint32_t fb_w = 0;
static uint32_t fb_h = 0;
static uint32_t fb_pitch = 0;
static uint8_t fb_bpp = 0;
static uint8_t red_pos = 0;
static uint8_t red_size = 0;
static uint8_t green_pos = 0;
static uint8_t green_size = 0;
static uint8_t blue_pos = 0;
static uint8_t blue_size = 0;
static uint8_t backbuffer[BACKBUFFER_MAX_BYTES];

int fb_init(uint32_t multiboot_info_addr) {const multiboot_info_t *info = (const multiboot_info_t *)(uintptr_t)multiboot_info_addr;
    if(!(info->flags & MULTIBOOT_FLAG_FRAMEBUFFER)) {return 0;}
    if(info->framebuffer_type != FRAMEBUFFER_TYPE_RGB) {return 0;}
    if(info->framebuffer_bpp != 15 && info->framebuffer_bpp != 16 && info->framebuffer_bpp != 24 && info->framebuffer_bpp != 32) {return 0;}
    if((info->framebuffer_addr >> 32) != 0) {return 0;}
    if((uint64_t)info->framebuffer_pitch * info->framebuffer_height > BACKBUFFER_MAX_BYTES) {return 0;}
    fb_ptr = (volatile uint8_t *)(uintptr_t)info->framebuffer_addr;
    fb_w = info->framebuffer_width;
    fb_h = info->framebuffer_height;
    fb_pitch = info->framebuffer_pitch;
    fb_bpp = info->framebuffer_bpp;
    if(fb_bpp == 32) {
        red_pos = 16;
        red_size = 8;
        green_pos = 8;
        green_size = 8;
        blue_pos = 0;
        blue_size = 8;
    }
    else {
        red_pos = info->framebuffer_red_field_position;
        red_size = info->framebuffer_red_mask_size;
        green_pos = info->framebuffer_green_field_position;
        green_size = info->framebuffer_green_mask_size;
        blue_pos = info->framebuffer_blue_field_position;
        blue_size = info->framebuffer_blue_mask_size;
    }
    return 1;
}

uint32_t fb_width(void) {return fb_w;}

uint32_t fb_height(void) {return fb_h;}

static inline uint32_t scale_channel(uint8_t value, uint8_t field_size) {
    if(field_size >= 8) {return value;}
    return value >> (8 - field_size);
}

void fb_put_pixel(uint32_t x, uint32_t y, uint8_t r, uint8_t g, uint8_t b) {
    if(x >= fb_w || y >= fb_h || !fb_ptr) {return;}
    uint32_t color = (scale_channel(r, red_size) << red_pos) | (scale_channel(g, green_size) << green_pos) | (scale_channel(b, blue_size) << blue_pos);
    uint32_t bytes_per_pixel = (fb_bpp == 15) ? 2 : (fb_bpp / 8);
    uint8_t *pixel = backbuffer + y * fb_pitch + x * bytes_per_pixel;
    pixel[0] = (uint8_t)(color & 0xFF);
    if(bytes_per_pixel >= 2) {pixel[1] = (uint8_t)((color >> 8) & 0xFF);}
    if(bytes_per_pixel >= 3) {pixel[2] = (uint8_t)((color >> 16) & 0xFF);}
    if(bytes_per_pixel >= 4) {pixel[3] = (uint8_t)((color >> 24) & 0xFF);}
}

void fb_fill_rect(uint32_t x, uint32_t y, uint32_t w, uint32_t h, uint8_t r, uint8_t g, uint8_t b) {for(uint32_t j = 0; j < h; j++) {for(uint32_t i = 0; i < w; i++) {fb_put_pixel(x + i, y + j, r, g, b);}}}

void fb_clear(uint8_t r, uint8_t g, uint8_t b) {fb_fill_rect(0, 0, fb_w, fb_h, r, g, b);}

void fb_present(void) {
    if(!fb_ptr) {return;}
    uint32_t total = fb_pitch * fb_h;
    uint32_t words = total / 4;
    volatile uint32_t *dst32 = (volatile uint32_t *)fb_ptr;
    const uint32_t *src32 = (const uint32_t *)backbuffer;
    for(uint32_t i = 0; i < words; i++) {dst32[i] = src32[i];}
    for(uint32_t i = words * 4; i < total; i++) {fb_ptr[i] = backbuffer[i];}
}