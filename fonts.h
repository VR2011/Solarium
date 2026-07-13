#ifndef FONTS_H
#define FONTS_H
#define FONT_COUNT 4
#define DEFAULT_FONT font_regular
#include <stdint.h>
#include "Fonts/font_regular.h"
#include "Fonts/font_bold.h"
#include "Fonts/font_dejavu.h"
#include "Fonts/font_comicsans.h"

typedef struct {
    const char *name;
    const uint8_t *data;
} font_entry_t;

static const font_entry_t fonts[FONT_COUNT] = {
    {"regular", font_regular},
    {"bold", font_bold},
    {"dejavu", font_dejavu},
    {"comicsans", font_comicsans}
};

#endif