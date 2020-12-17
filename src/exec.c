#include "env.h"
#include "exec.h"
#include <assert.h>
#include <errno.h>
#include <fcntl.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

#define OK_OR_RETURN(result, retval) \
    if ((result) == -1) \
    return retval

int execute(pipe_command_t* head) {
    assert(head != NULL);
    
    if (has_lexer_error()) {
        clear_lexer_error();
        return EXIT_VALUE_SUCCESS;
    }

    pipe_command_node_t* elem = SLIST_FIRST(head);
    SLIST_REMOVE_HEAD(head, nodes);

    if (SLIST_EMPTY(head)) {
        execute_redir_command(elem->redir_command);    
        return EXIT_VALUE_SUCCESS;
    }

    int fd[2];
    OK_OR_RETURN(pipe(fd), EXIT_VALUE_INTERNAL);

    int pid = fork();
    switch (pid) {
    case -1:
        return EXIT_VALUE_INTERNAL;
    case 0:
        OK_OR_RETURN(close(fd[0]), EXIT_VALUE_INTERNAL);
        OK_OR_RETURN(dup2(fd[1], 1), EXIT_VALUE_INTERNAL);
        OK_OR_RETURN(close(fd[1]), EXIT_VALUE_INTERNAL);
        exit(execute(head));
        break;
    default: {
        int old_input = dup(0);
        OK_OR_RETURN(old_input, EXIT_VALUE_INTERNAL);
        OK_OR_RETURN(dup2(fd[0], 0), EXIT_VALUE_INTERNAL);
        OK_OR_RETURN(close(fd[0]), EXIT_VALUE_INTERNAL);
        OK_OR_RETURN(close(fd[1]), EXIT_VALUE_INTERNAL);
        execute_redir_command(elem->redir_command);
        OK_OR_RETURN(waitpid(pid, NULL, 0), EXIT_VALUE_INTERNAL);
        OK_OR_RETURN(dup2(old_input, 0), EXIT_VALUE_INTERNAL);
        OK_OR_RETURN(close(old_input), EXIT_VALUE_INTERNAL);
        break;
    }
    }
    return EXIT_VALUE_SUCCESS;
}

void execute_redir_command(redir_command_t* head) {
    assert(head != NULL);

    command_t* cmd = get_command(head);
    assert(cmd != NULL);
    word_node_t* node = SLIST_FIRST(cmd);
    assert(node != NULL);
    assert(node->word != NULL);

    int argc;
    char** argv;

    prepare_args(cmd, &argc, &argv);
    int exit_value = 1;

    if (strcmp(node->word, "exit") == 0) {
        exit(get_exit_value());
    } else if (strcmp(node->word, "pwd") == 0) {
        exit_value = exec_pwd(argc);
    } else if (strcmp(node->word, "cd") == 0) {
        exit_value = exec_cd(argc, argv);
    } else {
        exit_value = exec_subshell(head, node->word, argv);
    }

    free(argv);
    set_exit_value(exit_value);
}

command_t* get_command(redir_command_t* head) {
    assert(head != NULL);
    redir_command_node_t* node;
    SLIST_FOREACH(node, head, nodes) {
        if (node->kind == COMMAND) {
            return node->command;
        }
    }
    assert(false);
    return NULL;
}

void prepare_args(command_t* head, int* argc, char*** argv) {
    *argc = 0;
    word_node_t* node;
    SLIST_FOREACH(node, head, nodes)
    ++*argc;

    *argv = (char**)malloc(sizeof(char*) * (*argc + 1));
    assert(*argv != NULL);

    char** it = *argv;
    SLIST_FOREACH(node, head, nodes) {
        *it = node->word;
        ++it;
    }
    *it = NULL;
}

int exec_pwd(int argc) {
    if (argc != 1) {
        return 1;
    }
    char* pwd = getcwd(NULL, 0);
    if (pwd == NULL) {
        return 1;
    }
    printf("%s\n", pwd);
    free(pwd);
    return EXIT_VALUE_SUCCESS;
}

int exec_cd(int argc, char** argv) {
    if (argc == 1) {
        return change_dir_home();
    } else if (argc == 2) {
        if (strcmp(argv[1], "-") == 0) {
            const char* old = change_dir_old();
            if (old == NULL) {
                return EXIT_VALUE_BAD_ENV;
            }
            printf("%s\n", old);
            return EXIT_VALUE_SUCCESS;
        }

        return change_dir(argv[1]);
    }
    fprintf(stderr, "Got too many arguments\n");
    return EXIT_VALUE_BAD_ARG;
}

static void handler_do_nothing(int sig) {
    (void)sig;
}

static void disable_sigint(struct sigaction* old_sa) {
    struct sigaction sa = {
        .sa_handler = handler_do_nothing,
        .sa_flags = SA_RESTART
    };
    if (sigaction(SIGINT, &sa, old_sa) == -1) {
        exit(EXIT_VALUE_INTERNAL);
    }
}

static void restore_sigint(struct sigaction* old_sa) {
    if (sigaction(SIGINT, old_sa, NULL) == -1) {
        exit(EXIT_VALUE_INTERNAL);
    }
}

int exec_subshell(redir_command_t* head, char* cmd, char** argv) {
    int pid = fork();
    switch (pid) {
    case -1:
        return EXIT_VALUE_EXEC;
    case 0: {
        int result = apply_redirections(head);
        if (result != EXIT_VALUE_SUCCESS) {
            return result;
        }
        execvp(cmd, argv);
        if (errno == ENOENT) {
            fprintf(stderr, "No such file '%s'\n", cmd);
            exit(EXIT_VALUE_NO_FILE);
        } else if (errno == EACCES) {
            fprintf(stderr, "Permission denied: '%s'\n", cmd);
            exit(EXIT_VALUE_PERMISSION);
        }
        return EXIT_VALUE_EXEC;
    }
    default: {
        int wstatus = 0;
        struct sigaction old_sa;
        disable_sigint(&old_sa);
        if (waitpid(pid, &wstatus, 0) != -1) {
            restore_sigint(&old_sa);
            if (WIFSIGNALED(wstatus)) {
                return EXIT_VALUE_SIG_OFFSET + WTERMSIG(wstatus);
            }
            return WEXITSTATUS(wstatus);
        }
        restore_sigint(&old_sa);
    }
    }
    return EXIT_VALUE_EXEC;
}

int apply_redirections(redir_command_t* head) {
    assert(head != NULL);
    int fd = -1;
    redir_command_node_t* node;
    SLIST_FOREACH(node, head, nodes) {
        switch (node->kind) {
        case RE_INPUT:
            assert(node->filename != NULL && node->filename->word != NULL);
            fd = open(node->filename->word, O_RDONLY);
            OK_OR_RETURN(fd, EXIT_VALUE_OPEN);
            OK_OR_RETURN(dup2(fd, 0), EXIT_VALUE_INTERNAL);
            break;
        case RE_OUTPUT:
            assert(node->filename != NULL && node->filename->word != NULL);
            fd = open(node->filename->word, O_WRONLY | O_CREAT, 0666);
            OK_OR_RETURN(fd, EXIT_VALUE_OPEN);
            OK_OR_RETURN(dup2(fd, 1), EXIT_VALUE_INTERNAL);
            break;
        case RE_APPEND:
            assert(node->filename != NULL && node->filename->word != NULL);
            fd = open(node->filename->word, O_WRONLY | O_CREAT | O_APPEND, 0666);
            OK_OR_RETURN(fd, EXIT_VALUE_OPEN);
            OK_OR_RETURN(dup2(fd, 1), EXIT_VALUE_INTERNAL);
            break;
        default:
            break;
        }
    }
    return EXIT_VALUE_SUCCESS;
}
