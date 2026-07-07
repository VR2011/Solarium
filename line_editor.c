#include "line_editor.h"
#include "terminal.h"
#include "keyboard.h"

void readline(char *buffer) {
    int index = 0;
    while(1) {
        key_event_t event = keyboard_getevent();
        if(event.type == KEY_ENTER) {
            buffer[index] = '\0';
            terminal_putchar('\n');
            return;
        }
        if(event.type == KEY_BACKSPACE) {
            if(index > 0) {
                index--;
                buffer[index] = '\0';
                terminal_backspace();
            }
            continue;
        }
        if(event.type == KEY_CHAR) {
            if(index < LINE_BUFFER_SIZE - 1) {
                buffer[index++] = event.ascii;
                buffer[index] = '\0';
                terminal_putchar(event.ascii);
            }
        }
    }
}