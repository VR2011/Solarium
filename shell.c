#include "shell.h"
#include "keyboard.h"
#include "terminal.h"
#include "string.h"

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
        terminal_write("echo - Repeat the message said.\n");
        terminal_write("whoami - Name of the user.\n");
        terminal_write("about - Info about the OS.\n");        
        terminal_write("clear - Clears the terminal.\n");
    }
    else if(strcmp(cmd, "whoami") == 0) {
        terminal_write("user\n");
    }
    else if(strncmp(cmd, "echo ", 5) == 0) {
        terminal_write(cmd + 5);
        terminal_putchar('\n');
    }
    else if(strcmp(cmd, "clear") == 0) {
        terminal_clear();
    }
    else if(strcmp(cmd, "about") == 0) {
        terminal_write_flame("Solarium OS\n");
        terminal_write("Version 1.1\n");
    }
    else {
        terminal_write("Unknown command!\n");
    }
}

static int history_oldest(void) {return history_count > HISTORY_SIZE ? history_count - HISTORY_SIZE: 0;}

static void redraw_input(const char *buffer, int len, int cursor) {
    terminal_clear_current_input();
    terminal_write(buffer);
    for(int i = 0; i < len - cursor; i++) {
        terminal_move_left();
    }
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
        }
        else if(event.type == KEY_BACKSPACE) {
            if(cursor > 0) {
                len--;
                cursor--;
                buffer[len] = '\0';
                terminal_backspace();
            }
            else {
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
                terminal_clear_current_input();
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
