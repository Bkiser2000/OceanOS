#include "terminal.h"
#include "print.h"
#include "keyboard.h"
#include "ramfs.h"
#include <string.h>


static void handle_command(const char* cmd) {
    if (!strcmp(cmd, "help")) {
        print_str("Commands: help, ls, cat <file>, echo <text>\n");
    } else if (!strcmp(cmd, "ls")) {
        ramfs_ls();
    } else if (strncmp(cmd, "cat ", 4) == 0) {
        ramfs_cat(cmd + 4);
    } else if (strncmp(cmd, "echo ", 5) == 0) {
        print_str(cmd + 5);
        print_str("\n");
    } else {
        print_str("Unknown command\n");
    }
}

void terminal_run() {
    char buf[128];
    while (1) {
        print_str("OceanOS> ");
        size_t i = 0;
        while (i < sizeof(buf) - 1) {
            char c = keyboard_get_char();
            if (c == '\n' || c == '\r') break;
            buf[i++] = c;
            print_char(c);
        }
        buf[i] = '\0';
        handle_command(buf);
    }
}
