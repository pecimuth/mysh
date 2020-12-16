#include "../build/parser.h"
#include "../include/ast.h"
#include "../include/env.h"
#include "../include/front.h"
#include <assert.h>
#include <errno.h>
#include <readline/history.h>
#include <readline/readline.h>
#include <signal.h>
#include <stdbool.h>
#include <stdlib.h>
#include <unistd.h>

#define YYWRAP_CONTINUE 0
#define YYWRAP_STOP 1

typedef struct yy_buffer_state* YY_BUFFER_STATE;
extern YY_BUFFER_STATE yy_scan_string(char*);
extern void yy_delete_buffer(YY_BUFFER_STATE);
extern void yyrestart(FILE*);
int yywrap();

static bool interactive = false;

void start(int argc, char** argv) {
    int opt;
    while ((opt = getopt(argc, argv, "c:")) != -1) {
        switch (opt) {
        case 'c': {
            char* buf = make_rl_buf(optarg);
            YY_BUFFER_STATE buf_state = yy_scan_string(buf);
            yyparse();
            yy_delete_buffer(buf_state);
            free(buf);
            return;
        }
        default:
            break;
        }
    }

    if (optind == argc) {
        if (isatty(STDIN_FILENO)) {
            interactive = true;
            yywrap();
        }
        yyparse();
        return;
    }

    parse_file(argv[optind]);
}

void parse_file(char* filename) {
    FILE* input_file = fopen(filename, "r");
    if (input_file == NULL) {
        exit(EXIT_VALUE_OPEN);
    }
    yyrestart(input_file);
    yyparse();
    fclose(input_file);
}

static void signal_handler(int sig) {
    (void)sig;
    set_exit_value(EXIT_VALUE_INTERRUPT);
}

static char* rl_buf = NULL;
static YY_BUFFER_STATE rl_buf_state;

int yywrap() {
    if (!interactive) {
        return YYWRAP_STOP;
    }

    delete_buffers();

    char prompt[256];
    make_prompt(prompt, sizeof(prompt));

    char* line = read_line_or_reset(prompt);
    if (line == NULL) {
        printf("\n");
        return YYWRAP_STOP;
    }
    if (strcmp(line, "") != 0) {
        add_history(line);
    }

    rl_buf = make_rl_buf(line);
    free(line);

    rl_buf_state = yy_scan_string(rl_buf);
    return YYWRAP_CONTINUE;
}

void delete_buffers() {
    if (rl_buf != NULL) {
        free(rl_buf);
        yy_delete_buffer(rl_buf_state);
    }
}

void make_prompt(char* buf, size_t size) {
    static size_t other_chars = strlen("mysh:$ ") + 1;
    size -= other_chars;
    assert(size > 0);

    char* cwd = getcwd(NULL, size);
    if (cwd == NULL) {
        exit(EXIT_VALUE_BAD_ENV);
    }

    sprintf(buf, "mysh:%s$ ", cwd);
    free(cwd);
}

static int reset_prompt() {
    printf("\n");
    rl_on_new_line();
    rl_replace_line("", 0);
    rl_redisplay();
    return EXIT_VALUE_SUCCESS;
}

char* read_line_or_reset(char* prompt) {
    rl_signal_event_hook = reset_prompt;
    struct sigaction sa = { .sa_handler = signal_handler }, old_sa;
    if (sigaction(SIGINT, &sa, &old_sa) == -1) {
        exit(EXIT_VALUE_INTERNAL);
    }
    char* line = readline(prompt);
    if (sigaction(SIGINT, &old_sa, NULL) == -1) {
        exit(EXIT_VALUE_INTERNAL);
    }
    return line;
}

char* make_rl_buf(char* line) {
    size_t buf_len = strlen(line) + 3;
    char* rl_buf = malloc(buf_len);
    if (rl_buf == NULL) {
        exit(EXIT_VALUE_MEMORY);
    }
    strcpy(rl_buf, line);
    rl_buf[buf_len - 1] = 0;
    rl_buf[buf_len - 2] = 0;
    rl_buf[buf_len - 3] = '\n';
    return rl_buf;
}
