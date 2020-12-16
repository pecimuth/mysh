#include "env.h"
#include <assert.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <stdbool.h>

int change_dir(const char* path) {
    char* pwd = getcwd(NULL, 0);
    if (pwd == NULL) {
        return EXIT_VALUE_BAD_ENV;
    }

    if (chdir(path) != 0) {
        return EXIT_VALUE_BAD_ENV;
    }

    char* new_pwd = getcwd(NULL, 0);
    if (new_pwd == NULL) {
        return EXIT_VALUE_BAD_ENV;
    }
    
    setenv("OLDPWD", pwd, 1);
    setenv("PWD", new_pwd, 1);
    
    free(pwd);
    free(new_pwd);

    return EXIT_VALUE_SUCCESS;
}

int change_dir_home() {
    const char* home = getenv("HOME");
    if (home == NULL) {
        return EXIT_VALUE_BAD_ENV;
    }
    return change_dir(home);
}

const char* change_dir_old() {
    const char* old = getenv("OLDPWD");
    if (old == NULL) {
        return NULL;
    }
    if (change_dir(old) != EXIT_VALUE_SUCCESS) {
        return NULL;
    }
    return getenv("PWD");
}

static int exit_value = EXIT_VALUE_SUCCESS;

void set_exit_value(int val) {
    exit_value = val;
}

int get_exit_value() {
    return exit_value;
}

static bool lexer_error = false;

void unexpected_character(char c) {
    fprintf(stderr, "unexpected character '%c'\n", c);
    lexer_error = true;
    set_exit_value(EXIT_VALUE_LEXER);
}

bool has_lexer_error() {
    return lexer_error;
}

void clear_lexer_error() {
    lexer_error = false;
}
