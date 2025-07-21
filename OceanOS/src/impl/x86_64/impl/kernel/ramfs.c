#include "ramfs.h"
#include "print.h"
#include "string.h"

#define RAMFS_MAX_FILES 8
#define RAMFS_FILE_SIZE 128
#define RAMFS_TYPE_FILE 0
#define RAMFS_TYPE_DIR 1

static struct RamfsFile files[RAMFS_MAX_FILES];
static size_t file_count = 0;

struct RamfsFile* root_dir = NULL;
void ramfs_init() {
    root_dir = &files[0];
    strcpy(files[0].name, "/");
    files[0].type = RAMFS_TYPE_DIR;
    files[0].size = 0;
    files[0].parent = NULL;
    file_count = 1;
}

void ramfs_ls(struct RamfsFile* dir) {
    for (size_t i = 0; i < file_count; i++) {
        if (files[i].parent == dir) {
            print_str(files[i].name);
            if (files[i].type == RAMFS_TYPE_DIR) print_str("/");
            print_str("\n");
        }
    }
}

void ramfs_cat(const char* filename, struct RamfsFile* parent) {
    for (size_t i = 0; i < file_count; i++) {
        if (!strcmp(files[i].name, filename) && files[i].parent == parent) {
            print_str(files[i].data);
            return;
        }
    }
    print_str("File not found\n");
}

void ramfs_create_dir(const char* name, struct RamfsFile* parent) {
    if (file_count >= RAMFS_MAX_FILES) {
        print_str("RAMFS full\n");
        return;
    }
    strcpy(files[file_count].name, name);
    files[file_count].type = RAMFS_TYPE_DIR;
    files[file_count].size = 0;
    files[file_count].parent = parent;
    files[file_count].data[0] = '\0';
    file_count++;
    print_str("Directory created\n");
}

void ramfs_create_file(const char* name, struct RamfsFile* parent) {
    if (file_count >= RAMFS_MAX_FILES) {
        print_str("RAMFS full\n");
        return;
    }
    // Check if file exists
    for (size_t i = 0; i < file_count; i++) {
        if (!strcmp(files[i].name, name)) {
            print_str("File already exists\n");
            return;
        }
    }
    strcpy(files[file_count].name, name);
    files[file_count].parent = parent;
    files[file_count].type = RAMFS_TYPE_FILE;
    files[file_count].data[0] = '\0';
    files[file_count].size = 0;
    file_count++;
    print_str("File created\n");
}

void ramfs_write_file(const char* name, const char* data, struct RamfsFile* parent) {
    for (size_t i = 0; i < file_count; i++) {
        if (!strcmp(files[i].name, name) && files[i].parent == parent) {
            strncpy(files[i].data, data, RAMFS_FILE_SIZE - 1);
            files[i].data[RAMFS_FILE_SIZE - 1] = '\0';
            files[i].size = strlen(files[i].data);
            print_str("File written\n");
            return;
        }
    }
    print_str("File not found\n");
}

void ramfs_delete(const char* name, struct RamfsFile* parent) {
    for (size_t i = 0; i < file_count; i++) {
        if (!strcmp(files[i].name, name) && files[i].parent == parent) {
            for (size_t j = i; j < file_count - 1; j++) {
                files[j] = files[j + 1];
            }
            file_count--;
            print_str("File deleted\n");
            return;
        }
    }
    print_str("File not found\n");
}

void ramfs_copy_file(const char* src, const char* dest, struct RamfsFile* parent) {
    if (file_count >= RAMFS_MAX_FILES) {
        print_str("RAMFS full\n");
        return;
    }
    for (size_t i = 0; i < file_count; i++) {
        if (!strcmp(files[i].name, src) && files[i].parent == parent) {
            ramfs_create_file(dest, parent);
            ramfs_write_file(dest, files[i].data, parent);
            print_str("File copied\n");
            return;
        }
    }
    print_str("Source file not found\n");
}

void ramfs_move_file(const char* src, const char* dest, struct RamfsFile* parent) {
    ramfs_copy_file(src, dest, parent);
    ramfs_delete(src, parent);
}

struct RamfsFile* ramfs_find(const char* path, struct RamfsFile* parent) {
    // Simple path parser: supports "dir/file" and single names
    char temp[32];
    const char* p = path;
    struct RamfsFile* current = parent;
    while (*p) {
        size_t len = 0;
        while (p[len] && p[len] != '/') len++;
        strncpy(temp, p, len);
        temp[len] = '\0';
        p += len;
        if (*p == '/') p++;
        struct RamfsFile* found = NULL;
        for (size_t i = 0; i < file_count; i++) {
            if (!strcmp(files[i].name, temp) && files[i].parent == current) {
                found = &files[i];
                break;
            }
        }
        if (!found) return NULL;
        current = found;
    }
    return current;
}