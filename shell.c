#include "shell.h"
#include "terminal.h"
#include "keyboard.h"
#include "string.h"

#define BUFFER_SIZE 128

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
        terminal_write("Solarium OS\n");
        terminal_write("Version 1.0\n");
    }
    else {
        terminal_write("Unknown command!\n");
    }
}

void shell_run(void) {
    char buffer[128];
    int index = 0;
    terminal_write("> ");
    while(1) {
        char c = keyboard_getchar();
        if(c == '\n') {
            buffer[index] = '\0';
            terminal_putchar('\n');
            execute_command(buffer);
            index = 0;
            terminal_write("> ");
        }
        else if(c == '\b') {
            if(index > 0) {
                index--;
                terminal_backspace();
            }
        }
        else {
            if(index < BUFFER_SIZE - 1) {
                buffer[index++] = c;
                terminal_putchar(c);
            }
        }
    }
}