#include "../include/env.h"
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <dirent.h>

const char* get_pwd() {
    return getenv("PWD");
}

static void set_pwd(const char* new_pwd) {
    const char* pwd = get_pwd();
    setenv("OLDPWD", pwd, 1);
    setenv("PWD", new_pwd, 1);
}

int change_dir(const char* path) {
    int result = chdir(path);
    if (result != 0) {
        return result;
    }

    char buf[256];
    char* cwd_result = getcwd(buf, 256);
    if (cwd_result == NULL) {
        return 1;
    }
    set_pwd(cwd_result);

    return 0;
}

int change_dir_home() {
    const char* home = getenv("HOME");
    return change_dir(home);
}

const char* change_dir_old() {
    const char* old = getenv("OLDPWD");
    int result = change_dir(old);
    if (result != 0) {
        return NULL;
    }
    return get_pwd();
}

static int exit_value = 0;

void set_exit_value(int val) {
    exit_value = val;
}

int get_exit_value() {
    return exit_value;
}
