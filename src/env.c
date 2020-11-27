#include "../include/env.h"
#include <assert.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <dirent.h>

int change_dir(const char* path) {
    char* pwd = getcwd(NULL, 0);
    if (pwd == NULL) {
        return 1;
    }

    if (chdir(path) != 0) {
        return 2;
    }

    char* new_pwd = getcwd(NULL, 0);
    if (new_pwd == NULL) {
        return 3;
    }
    
    setenv("OLDPWD", pwd, 1);
    setenv("PWD", new_pwd, 1);
    
    free(pwd);
    free(new_pwd);

    return 0;
}

int change_dir_home() {
    const char* home = getenv("HOME");
    if (home == NULL) {
        return 5;
    }
    return change_dir(home);
}

const char* change_dir_old() {
    const char* old = getenv("OLDPWD");
    int result = change_dir(old);
    if (result != 0) {
        return NULL;
    }
    return getenv("PWD");
}

static int exit_value = 0;

void set_exit_value(int val) {
    exit_value = val;
}

int get_exit_value() {
    return exit_value;
}
