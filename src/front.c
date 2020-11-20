#include "../include/front.h"
#include <stdlib.h>
#include <readline/history.h>
#include <readline/readline.h>

#define YY_NULL 0

size_t get_input(char* buf, size_t size) {
    char* line = readline("mysh$ ");
    if (!line) {
        return YY_NULL;
    }
    if (strlen(line) > size - 2) {
        return YY_NULL;
    }
    sprintf(buf, "%s\n", line);
    add_history(line);
    free(line);
    return strlen(buf);
}
