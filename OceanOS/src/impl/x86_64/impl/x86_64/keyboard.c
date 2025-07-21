#include <stddef.h>
#include "bool.h"
#include "keyboard.h"
#include "x86_64/idt.h"
#include "x86_64/ps2.h"

#define KEYBOARD_EXTENDED_SCAN_CODE 0xE0

static char key_buffer[128];
static size_t key_buf_len = 0;

// This function should be called from your keyboard interrupt handler
void keyboard_buffer_push(char c) {
    if (key_buf_len < sizeof(key_buffer) - 1) {
        key_buffer[key_buf_len++] = c;
    }
}

// Returns the next character from the buffer, blocking until one is available
char keyboard_get_char() {
    while (key_buf_len == 0) {
        // Add a CPU pause or yield here
    }
    char c = key_buffer[0];
    for (size_t i = 1; i < key_buf_len; i++) {
        key_buffer[i - 1] = key_buffer[i];
    }
    key_buf_len--;
    return c;
}

void (*keyboard_handler_user)(struct KeyboardEvent event);

void keyboard_handler() {
	static bool is_extended = 0;
	
	uint8_t scan_code = ps2_read_scan_code();
	
	if (scan_code == KEYBOARD_EXTENDED_SCAN_CODE) {
		is_extended = true;
		return;
	}
	
	if (keyboard_handler_user == NULL) {
		return;
	}
	
	uint16_t fat_code = scan_code & 0x7F;

	if (is_extended) {
		is_extended = false;
		fat_code |= KEYBOARD_EXTENDED_SCAN_CODE << 8;
	}
	
	struct KeyboardEvent event;
	
	if ((scan_code & 0x80) == 0) {
		event.type = KEYBOARD_EVENT_TYPE_MAKE;
	} else {
		event.type = KEYBOARD_EVENT_TYPE_BREAK;
	}
	
	event.code = fat_code;
	
	keyboard_handler_user(event);
}

void keyboard_init() {
	idt_init();
	idt_set_handler_keyboard(keyboard_handler);
}

void keyboard_set_handler(void (*handler)(struct KeyboardEvent event)) {
	keyboard_handler_user = handler;	
}
