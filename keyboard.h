#ifndef KEYBOARD_H
#define KEYBOARD_H

#include <stdint.h>

typedef enum {
    KEY_NONE,
    KEY_CHAR,
    KEY_UP,
    KEY_DOWN,
    KEY_LEFT,
    KEY_RIGHT,
    KEY_ENTER,
    KEY_BACKSPACE,
    KEY_TAB,
    KEY_ESCAPE
}
key_type_t;

typedef struct {
    key_type_t type;
    char ascii;
}
key_event_t;

key_event_t keyboard_getevent(void);

#endif