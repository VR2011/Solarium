#include "terminal.h"
#include "shell.h"

void kernel_main(void) {
    terminal_initialize();
    terminal_write("Solarium\n");
    terminal_write("Version 1.1\n");
    terminal_write("Enter 'help' for a list of commands.\n");
    shell_run();
    while(1) {}
}