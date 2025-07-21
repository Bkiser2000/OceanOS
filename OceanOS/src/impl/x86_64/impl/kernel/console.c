#include <stddef.h>
#include <stdint.h>

#define VGA_WIDTH 80
#define VGA_HEIGHT 25
#define VGA_MEMORY ((volatile uint16_t*)0xB8000)
#define VGA_COLOR 0x07 // Light grey on black

static size_t cursor_row = 0;
static size_t cursor_col = 0;

void console_putchar(char c) {
    if (c == '\n') {
        cursor_col = 0;
        if (++cursor_row == VGA_HEIGHT)
            cursor_row = 0;
        return;
    }
    VGA_MEMORY[cursor_row * VGA_WIDTH + cursor_col] = ((uint16_t)VGA_COLOR << 8) | c;
    if (++cursor_col == VGA_WIDTH) {
        cursor_col = 0;
        if (++cursor_row == VGA_HEIGHT)
            cursor_row = 0;
    }
}

