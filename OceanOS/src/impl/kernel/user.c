#include "user.h"
#include "print.h"
#include "keyboard.h"
#include <string.h>

static struct User default_user = { "ADMIN", "PASSWORD" };

static void get_input(char* buf, size_t max) {
    size_t i = 0;
    while (i < max - 1) {
        char c = keyboard_get_char();
        if (c == '\n' || c == '\r') break;
        if (c == '\b') {
            if (i > 0) {
                i--;
                print_char('\b');
                print_char(' ');
                print_char('\b');
            }
            continue;
        }
        buf[i++] = c;
        print_char(c);
    }
    buf[i] = '\0';
}

int user_login_prompt() {
    char username[USERNAME_MAX];
    char password[PASSWORD_MAX];

    print_str("Username: ");
    get_input(username, USERNAME_MAX);
    print_str("\nPassword: ");
    get_input(password, PASSWORD_MAX);

    if (!strcmp(username, default_user.username) && !strcmp(password, default_user.password)) {
        print_str("\nLogin successful!\n");
        return 1;
    } else {
        print_str("\nLogin failed!\n");
        return 0;
    }
}