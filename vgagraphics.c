#include "vgagraphics.h"
#include "terminal.h"

void vga_load_font(const uint8_t *font) {terminal_set_font(font);}