#include "../include/env.h"
#include "../include/front.h"
#include <readline/history.h>
#include <readline/readline.h>
#include <stdlib.h>

#define YY_NULL 0

size_t get_input(char* buf, size_t size) {
    char prompt[256];
    sprintf(prompt, "mysh:%s$ ", get_pwd());
    char* line = readline(prompt);
    if (line == NULL) {
        return YY_NULL;
    }
    if (strlen(line) + 2 > size) {
        free(line);
        return YY_NULL;
    }
    sprintf(buf, "%s\n", line);
    add_history(line);
    free(line);
    return strlen(buf);
}
