#include "ramfs.h"
#include "print.h"
#include "string.h"

#define RAMFS_MAX_FILES 8
#define RAMFS_FILE_SIZE 128

struct RamfsFile {
    char name[32];
    char data[RAMFS_FILE_SIZE];
    size_t size;
};

static struct RamfsFile files[RAMFS_MAX_FILES];
static size_t file_count = 0;

void ramfs_init() {
    strcpy(files[0].name, "hello.txt");
    strcpy(files[0].data, "Hello from RAMFS!\n");
    files[0].size = strlen(files[0].data);
    file_count = 1;
}

void ramfs_ls() {
    for (size_t i = 0; i < file_count; i++) {
        print_str(files[i].name);
        print_str("\n");
    }
}

void ramfs_cat(const char* filename) {
    for (size_t i = 0; i < file_count; i++) {
        if (!strcmp(files[i].name, filename)) {
            print_str(files[i].data);
            return;
        }
    }
    print_str("File not found\n");
}
