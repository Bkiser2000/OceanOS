#pragma once

#define USERNAME_MAX 32
#define PASSWORD_MAX 32

struct User {
    char username[USERNAME_MAX];
    char password[PASSWORD_MAX];
};

int user_login_prompt();
