#include "../include/env.h"
#include "../include/front.h"
#include <readline/history.h>
#include <readline/readline.h>
#include <stdlib.h>
#include <errno.h>
#include <stdbool.h>
#include <unistd.h>

static size_t get_input_from_yyin(char* buf, size_t max_size, FILE* yyin) {
    size_t result = 0;
    errno = 0;
    while ((result = fread(buf, 1, max_size, yyin)) == 0 && ferror(yyin)) {
        if(errno != EINTR) {
            //YY_FATAL_ERROR( "input in flex scanner failed" );
            return 0;
        }
        errno = 0;
        clearerr(yyin);
    }
    return result;
}

static size_t get_input_from_readline(char* buf, size_t max_size) {
    char prompt[32];
    sprintf(prompt, "mysh:%.24s$ ", get_pwd());
    char* line = readline(prompt);
    if (line == NULL) {
        printf("\n");
        return 0;
    }
    if (strlen(line) + 2 > max_size) {
        free(line);
        return 0;
    }
    int result = sprintf(buf, "%s\n", line);
    if (strcmp(line, "") != 0) {
        add_history(line);
    }
    free(line);
    return result;
}

size_t get_input(char* buf, size_t max_size, FILE* yyin) {
    if (isatty(STDIN_FILENO)) {
        return get_input_from_readline(buf, max_size);
    }
    return get_input_from_yyin(buf, max_size, yyin);
}
