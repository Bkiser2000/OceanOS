#include "terminal.h"
#include "print.h"
#include "keyboard.h"
#include "ramfs.h"
#include <string.h>

struct RamfsFile* current_dir = NULL;

static void handle_command(const char* cmd) {
    print_char('\n');
    if (!strcmp(cmd, "help")) {
        print_str("Commands: help, ls <dir>, cat <file>, echo <text>, mkdir <dir>, touch <file>, write <file> <data>, rm <file>, cp <src> <dest>, mv <src> <dest>, cd <dir>, home\n");
    } else if (strncmp(cmd, "mkdir ", 6) == 0) {
        ramfs_create_dir(cmd + 6, current_dir);
    } else if (!strcmp(cmd, "ls")) {
        ramfs_ls(current_dir);
    } else if (strncmp(cmd, "ls ", 3) == 0) {
        struct RamfsFile* dir = ramfs_find(cmd + 3, current_dir);
        if (dir && dir->type == RAMFS_TYPE_DIR) {
            ramfs_ls(dir);
        } else {
            print_str("Directory not found\n");
        }
    } else if (strncmp(cmd, "cat ", 4) == 0) {
        struct RamfsFile* file = ramfs_find(cmd + 4, current_dir);
        if (file && file->type == RAMFS_TYPE_FILE) {
            print_str(file->data);
        } else {
            print_str("File not found\n");
        }
    } else if (strncmp(cmd, "touch ", 6) == 0) {
        ramfs_create_file(cmd + 6, current_dir);
    } else if (strncmp(cmd, "write ", 6) == 0) {
        char* space = strchr(cmd + 6, ' ');
        if (space) {
            *space = '\0';
            struct RamfsFile* file = ramfs_find(cmd + 6, current_dir);
            if (file && file->type == RAMFS_TYPE_FILE) {
                ramfs_write_file(cmd + 6, space + 1, current_dir);
            } else {
                print_str("File not found\n");
            }
        } else {
            print_str("Usage: write <file> <data>\n");
        }
    } else if (strncmp(cmd, "rm ", 3) == 0) {
        struct RamfsFile* file = ramfs_find(cmd + 3, current_dir);
        if (file && file->type == RAMFS_TYPE_FILE) {
            ramfs_delete(file->name, file->parent);
        } else {
            print_str("File not found\n");
        }
    } else if (strncmp(cmd, "cp ", 3) == 0) {
        char* space = strchr(cmd + 3, ' ');
        if (space) {
            *space = '\0';
            struct RamfsFile* src = ramfs_find(cmd + 3, current_dir);
            if (src && src->type == RAMFS_TYPE_FILE) {
                ramfs_copy_file(src->name, space + 1, src->parent);
            } else {
                print_str("Source file not found\n");
            }
        } else {
            print_str("Usage: cp <src> <dest>\n");
        }
    } else if (strncmp(cmd, "mv ", 3) == 0) {
        char* space = strchr(cmd + 3, ' ');
        if (space) {
            *space = '\0';
            struct RamfsFile* src = ramfs_find(cmd + 3, current_dir);
            if (src && src->type == RAMFS_TYPE_FILE) {
                ramfs_move_file(src->name, space + 1, src->parent);
            } else {
                print_str("Source file not found\n");
            }
        } else {
            print_str("Usage: mv <src> <dest>\n");
        }
    } else if (strncmp(cmd, "cd ", 3) == 0) {
        struct RamfsFile* dir = ramfs_find(cmd + 3, current_dir);
        if (dir && dir->type == RAMFS_TYPE_DIR) {
            current_dir = dir;
            print_str("Changed directory\n");
        } else {
            print_str("Directory not found\n");
        }
    } else if (!strcmp(cmd, "home")) {
        current_dir = root_dir;
        print_str("Returned to home directory\n");
    } else if (strncmp(cmd, "echo ", 5) == 0) {
        print_str(cmd + 5);
        print_char('\n');
    } else {
        print_str("Unrecognized command\n");
    }
}


void terminal_run() {
    current_dir = root_dir;
    char buf[128];
    while (1) {
        print_str("OceanOS> ");
        size_t i = 0;
        while (i < sizeof(buf) - 1) {
            char c = keyboard_get_char();
            if (c == '\n' || c == '\r') break;
            if (c == '\b') {
                if (i > 0) {
                    i--;
                    // Move cursor back, overwrite with space, move back again
                    print_char('\b');
                    print_char(' ');
                    print_char('\b');
                }
                continue;
            }
            // Only print printable ASCII characters
            if (c >= 32 && c <= 126) {
                buf[i++] = c;
                print_char(c);
            }
        }
        buf[i] = '\0';
        handle_command(buf);
    }
}
