#include "print.h"
#include "keyboard.h"
#include "x86_64/rtc.h"
#include "ramfs.h"
#include "user.h"
#include "terminal.h"
#include "bool.h"
#include "pci.h"
#include "e1000.h"

#define KEY_CODE_A 0x1E
#define KEY_CODE_B 0x30
#define KEY_CODE_C 0x2E
#define KEY_CODE_D 0x20
#define KEY_CODE_E 0x12
#define KEY_CODE_F 0x21
#define KEY_CODE_G 0x22
#define KEY_CODE_H 0x23
#define KEY_CODE_I 0x17
#define KEY_CODE_J 0x24
#define KEY_CODE_K 0x25
#define KEY_CODE_L 0x26
#define KEY_CODE_M 0x32
#define KEY_CODE_N 0x31
#define KEY_CODE_O 0x18
#define KEY_CODE_P 0x19
#define KEY_CODE_Q 0x10
#define KEY_CODE_R 0x13
#define KEY_CODE_S 0x1F
#define KEY_CODE_T 0x14
#define KEY_CODE_U 0x16
#define KEY_CODE_V 0x2F
#define KEY_CODE_W 0x11
#define KEY_CODE_X 0x2D
#define KEY_CODE_Y 0x15
#define KEY_CODE_Z 0x2C
#define KEY_CODE_SPACE 0x39
#define KEY_CODE_ENTER 0x1C
#define KEY_CODE_BACKSPACE 0x0E
#define KEY_CODE_LEFT_SHIFT 0x2A
#define KEY_CODE_RIGHT_SHIFT 0x36

static bool shift_pressed = false;

 char to_ascii(uint16_t code, bool shift) {
    switch (code) {
        case KEY_CODE_A: return shift ? 'A' : 'a';
        case KEY_CODE_B: return shift ? 'B' : 'b';
        case KEY_CODE_C: return shift ? 'C' : 'c';
        case KEY_CODE_D: return shift ? 'D' : 'd';
        case KEY_CODE_E: return shift ? 'E' : 'e';
        case KEY_CODE_F: return shift ? 'F' : 'f';
        case KEY_CODE_G: return shift ? 'G' : 'g';
        case KEY_CODE_H: return shift ? 'H' : 'h';
        case KEY_CODE_I: return shift ? 'I' : 'i';
        case KEY_CODE_J: return shift ? 'J' : 'j';
        case KEY_CODE_K: return shift ? 'K' : 'k';
        case KEY_CODE_L: return shift ? 'L' : 'l';
        case KEY_CODE_M: return shift ? 'M' : 'm';
        case KEY_CODE_N: return shift ? 'N' : 'n';
        case KEY_CODE_O: return shift ? 'O' : 'o';
        case KEY_CODE_P: return shift ? 'P' : 'p';
        case KEY_CODE_Q: return shift ? 'Q' : 'q';
        case KEY_CODE_R: return shift ? 'R' : 'r';
        case KEY_CODE_S: return shift ? 'S' : 's';
        case KEY_CODE_T: return shift ? 'T' : 't';
        case KEY_CODE_U: return shift ? 'U' : 'u';
        case KEY_CODE_V: return shift ? 'V' : 'v';
        case KEY_CODE_W: return shift ? 'W' : 'w';
        case KEY_CODE_X: return shift ? 'X' : 'x';
        case KEY_CODE_Y: return shift ? 'Y' : 'y';
        case KEY_CODE_Z: return shift ? 'Z' : 'z';

        case 0x02: return shift ? '!' : '1';
        case 0x03: return shift ? '@' : '2';
        case 0x04: return shift ? '#' : '3';
        case 0x05: return shift ? '$' : '4';
        case 0x06: return shift ? '%' : '5';
        case 0x07: return shift ? '^' : '6';
        case 0x08: return shift ? '&' : '7';
        case 0x09: return shift ? '*' : '8';
        case 0x0A: return shift ? '(' : '9';
        case 0x0B: return shift ? ')' : '0';
    
        case 0x0C: return shift ? '_' : '-';
        case 0x0D: return shift ? '+' : '=';
        case 0x1A: return shift ? '{' : '[';
        case 0x1B: return shift ? '}' : ']';
        case 0x27: return shift ? ':' : ';';
        case 0x28: return shift ? '"' : '\'';
        case 0x29: return shift ? '~' : '`';
        case 0x2B: return shift ? '|' : '\\';
        case 0x33: return shift ? '<' : ',';
        case 0x34: return shift ? '>' : '.';
        case 0x35: return shift ? '?' : '/';

        case KEY_CODE_SPACE: return ' ';
        case KEY_CODE_ENTER: return '\n';
        case KEY_CODE_BACKSPACE: return '\b';
    }
    return '?';
}

void handle_input(struct KeyboardEvent event) {
    if(event.code == KEY_CODE_LEFT_SHIFT || event.code == KEY_CODE_RIGHT_SHIFT) {
        if (event.type == KEYBOARD_EVENT_TYPE_MAKE) shift_pressed = true;
        else if (event.type == KEYBOARD_EVENT_TYPE_BREAK) shift_pressed = false;
        return; // Don't push shift key itself
    }
    if (event.type == KEYBOARD_EVENT_TYPE_MAKE) {
        char ascii = to_ascii(event.code, shift_pressed);
        if (ascii != '?') {
            keyboard_buffer_push(ascii);
        }
    }
}

void kernel_main() {
    print_clear();
    print_set_color(PRINT_COLOR_YELLOW, PRINT_COLOR_BLACK);
    print_str("Welcome to OceanOS!");

    keyboard_init();
    keyboard_set_handler(handle_input);
    
    uint8_t prev_seconds = 0;
    
    for (uint8_t i = 0; i < 5;) {
        uint8_t seconds = rtc_seconds();
        
        if (seconds != prev_seconds) {
            i++;
            print_set_color(PRINT_COLOR_GREEN, PRINT_COLOR_BLACK);
            print_str("\nSeconds: ");
            print_uint64_dec(seconds);
        }
        
        prev_seconds = seconds;
    }
    
    print_str(" - Seconds loop disabled.\n");
    
    ramfs_init();
    while(1) {
        if (user_login_prompt()) {
            break;
        }
        print_str("\nTry again.\n");
    }

    terminal_run();

    while (1);
}
