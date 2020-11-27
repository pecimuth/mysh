#include "../include/ast.h"
#include "../build/parser.h"
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

typedef struct yy_buffer_state* YY_BUFFER_STATE;
extern YY_BUFFER_STATE yy_scan_string(char*);
extern void yy_delete_buffer(YY_BUFFER_STATE);
extern void yyrestart(FILE*);

static bool interactive = false;

void set_args(int argc, char** argv) {
    int opt;
    while ((opt = getopt(argc, argv, "c:")) != -1) {
        switch (opt) {
        case 'c': {
            size_t buf_len = strlen(optarg) + 3;
            char* buf = malloc(buf_len);
            strcpy(buf, optarg);
            buf[buf_len - 1] = 0;
            buf[buf_len - 2] = 0;
            buf[buf_len - 3] = '\n';
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
        interactive = true;
        if (isatty(STDIN_FILENO)) {
            yyrestart(fopen("/dev/null", "r"));
        }
        yyparse();
        return;
    }

    FILE* input_file = fopen(argv[optind], "r");
    yyrestart(input_file);
    yyparse();
    fclose(input_file);
}

static void signal_handler(int sig) {
    (void)sig;
    set_exit_value(1);
}

static int reset_prompt() {
    printf("\n");
    rl_on_new_line();
    rl_replace_line("", 0);
    rl_redisplay();
    return 0;
}

static char* rl_buf = NULL;
static YY_BUFFER_STATE rl_buf_state;

int yywrap() {
    if (!interactive) {
        return 1;
    }

    if (rl_buf != NULL) {
        free(rl_buf);
        yy_delete_buffer(rl_buf_state);
    }

    char prompt[32];
    const char* cwd = getcwd(NULL, 24);
    sprintf(prompt, "mysh:%.24s$ ", cwd);
    if (cwd != NULL) {
        free(cwd);
    }

    rl_signal_event_hook = reset_prompt;
    struct sigaction sa = { .sa_handler = signal_handler }, old_sa;
    sigaction(SIGINT, &sa, &old_sa);
    char* line = readline(prompt);
    sigaction(SIGINT, &old_sa, NULL);

    if (line == NULL) {
        printf("\n");
        return 1;
    }

    if (strcmp(line, "") != 0) {
        add_history(line);
    }

    size_t buf_len = strlen(line) + 3;
    char* rl_buf = malloc(buf_len);
    strcpy(rl_buf, line);
    free(line);
    rl_buf[buf_len - 1] = 0;
    rl_buf[buf_len - 2] = 0;
    rl_buf[buf_len - 3] = '\n';

    rl_buf_state = yy_scan_string(rl_buf);
    return 0;
}
