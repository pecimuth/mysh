#include "../include/env.h"
#include <stdlib.h>
#include <stdio.h>

const char* get_pwd() {
    return getenv("PWD");
}

void set_pwd(const char* new_pwd) {
    const char* pwd = get_pwd();
    setenv("OLDPWD", pwd, 1);
    setenv("PWD", new_pwd, 1);
}

void set_old_pwd() {
    const char* old_pwd = getenv("OLDPWD");
    set_pwd(old_pwd);
}

const char* find_in_path(const char* cmd) {
    (void)cmd;
    return getenv("PATH");
}
