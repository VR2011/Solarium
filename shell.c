#include "shell.h"
#include "keyboard.h"
#include "terminal.h"
#include "string.h"
#include "vgagraphics.h"
#include "fonts.h"

#define PROMPT "> "
#define HISTORY_SIZE 4096
#define LINE_BUFFER_SIZE 128

static char history[HISTORY_SIZE][LINE_BUFFER_SIZE];
static int history_count = 0;
static int history_position = 0;

void execute_command(const char *cmd) {
    if(cmd[0] == '\0') {return;}
    if(strcmp(cmd, "help") == 0) {
        terminal_write("List of commands:\n");
        terminal_write("help - Get the list of commands\n");
        terminal_write("echo <string> - Repeat the message said.\n");
        terminal_write("whoami - Name of the user.\n");
        terminal_write("about - Info about the OS.\n");        
        terminal_write("clear - Clears the terminal.\n");
        terminal_write("font <name> - Selects a font to be used. The following fonts you can choose are:regular bold dejavu comicsans\n");
    }
    else if(strcmp(cmd, "whoami") == 0) {
        terminal_write("user\n");
    }
    else if(strncmp(cmd, "echo ", 5) == 0) {
        terminal_write(cmd + 5);
        terminal_putchar('\n');
    }
    else if(strcmp(cmd, "about") == 0) {
        terminal_write_flame("Solarium OS\n");
        terminal_write("Version 1.2\n");
    }
    else if(strcmp(cmd, "clear") == 0) {
        terminal_clear();
    }
    else if(strncmp(cmd, "font ", 5) == 0) {
        const char *requested = cmd + 5;
        int found = 0;
        for(int i = 0; i < FONT_COUNT; i++) {
            if(strcmp(requested, fonts[i].name) == 0) {
                vga_load_font(fonts[i].data);
                terminal_write("Font changed to: ");
                terminal_write(fonts[i].name);
                terminal_putchar('\n');
                found = 1;
                break;
            }
        }
        if(!found) {
            terminal_write("Unknown font! Please select a correct font name.");
        }
    }
    else {
        terminal_write("Unknown command!\n");
    }
}

static int history_oldest(void) {return history_count > HISTORY_SIZE ? history_count - HISTORY_SIZE: 0;}

static size_t input_row = 0;

static void redraw_input(const char *buffer, int len, int cursor) {
    (void)len;
    terminal_set_cursor(strlen(PROMPT), input_row);
    terminal_clear_line_from_cursor();
    terminal_write(buffer);
    terminal_set_cursor(strlen(PROMPT) + (size_t)cursor, input_row);
}

static void load_history(char *buffer, int *len, int *cursor) {
    strcpy(buffer, history[history_position % HISTORY_SIZE]);
    *len = strlen(buffer);
    *cursor = *len;
    redraw_input(buffer, *len, *cursor);
}

void shell_run(void) {
    char buffer[LINE_BUFFER_SIZE];
    int len = 0;
    int cursor = 0;
    buffer[0] = '\0';
    terminal_write(PROMPT);
    input_row = terminal_get_row();
    while(1) {
        key_event_t event = keyboard_getevent();
        if(event.type == KEY_ENTER) {
            buffer[len] = '\0';
            terminal_putchar('\n');
            execute_command(buffer);
            if(buffer[0] != '\0') {
                strcpy(history[history_count % HISTORY_SIZE], buffer);
                history_count++;
            }
            history_position = history_count;
            len = 0;
            cursor = 0;
            buffer[0] = '\0';
            terminal_write(PROMPT);
            input_row = terminal_get_row();
        }
        else if(event.type == KEY_BACKSPACE) {
            if(cursor > 0) {
                for(int i = cursor - 1; i < len - 1; i++) {
                    buffer[i] = buffer[i + 1];
                }
                len--;
                cursor--;
                buffer[len] = '\0';
                redraw_input(buffer, len, cursor);
            }
        }
        else if(event.type == KEY_UP) {
            if(history_count == 0 || history_position <= history_oldest()) {continue;}
            history_position--;
            load_history(buffer, &len, &cursor);
        }
        else if(event.type == KEY_DOWN) {
            if(history_position >= history_count) {continue;}
            history_position++;
            if(history_position == history_count) {
                terminal_set_cursor(strlen(PROMPT), input_row);
                terminal_clear_line_from_cursor();
                buffer[0] = '\0';
                len = 0;
                cursor = 0;
            }
            else {load_history(buffer, &len, &cursor);}
        }
        else if(event.type == KEY_LEFT) {
            if(cursor > 0) {
                cursor--;
                terminal_move_left();
            }
        }
        else if(event.type == KEY_RIGHT) {
            if(cursor < len) {
                cursor++;
                terminal_move_right();
            }
        }
        else if(event.type == KEY_CHAR) {
            if(len < LINE_BUFFER_SIZE - 1) {
                if(cursor == len) {
                    buffer[len++] = event.ascii;
                    buffer[len] = '\0';
                    terminal_putchar(event.ascii);
                    cursor = len;
                }
                else {
                    for(int i = len; i > cursor; i--) {
                        buffer[i] = buffer[i - 1];
                    }
                    buffer[cursor] = event.ascii;
                    len++;
                    cursor++;
                    buffer[len] = '\0';
                    redraw_input(buffer, len, cursor);
                }
            }
        }
        
    }
}