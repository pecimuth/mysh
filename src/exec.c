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

#define IO_EXPECT_OK(result) \
    if ((result) == -1) \
    return EXIT_VALUE_IO

void execute(pipe_command_t* head) {
    assert(head != NULL);

    if (has_lexer_error()) {
        clear_lexer_error();
        destroy_pipe_command(head);
        return;
    }

    int result = execute_pipe_command(head);
    if (result != EXIT_VALUE_SUCCESS) {
        exit(result);
    }
}

/**
 * Recursively execute the command with pipes.
 * Free the argument recursively.
 * Non-success exit value implies inconsistent IO state.
 */
int execute_pipe_command(pipe_command_t* head) {
    assert(head != NULL);
    assert(!SLIST_EMPTY(head));

    pipe_command_node_t* elem = SLIST_FIRST(head);
    SLIST_REMOVE_HEAD(head, nodes);

    if (SLIST_EMPTY(head)) {
        execute_redir_command(elem->redir_command);
        destroy_pipe_command_node(elem);
        destroy_pipe_command(head);
        return EXIT_VALUE_SUCCESS;
    }

    int fd[2];
    IO_EXPECT_OK(pipe(fd));

    int pid = fork();
    switch (pid) {
    case -1:
        return EXIT_VALUE_INTERNAL;
    case 0:
        IO_EXPECT_OK(close(fd[0]));
        IO_EXPECT_OK(dup2(fd[1], 1));
        IO_EXPECT_OK(close(fd[1]));
        exit(execute_pipe_command(head));
        break;
    default: {
        // save the current input fd
        int old_input = dup(0);
        IO_EXPECT_OK(old_input);

        // redirect pipe's output
        IO_EXPECT_OK(dup2(fd[0], 0));
        IO_EXPECT_OK(close(fd[0]));
        IO_EXPECT_OK(close(fd[1]));

        execute_redir_command(elem->redir_command);
        IO_EXPECT_OK(waitpid(pid, NULL, 0));

        // restore old input
        IO_EXPECT_OK(dup2(old_input, 0));
        IO_EXPECT_OK(close(old_input));
        break;
    }
    }
    destroy_pipe_command_node(elem);
    destroy_pipe_command(head);
    return EXIT_VALUE_SUCCESS;
}

/**
 * Execute a command with redirections.
 * Either in the foreground or in a subshell.
 * Set the shell exit value.
 * */
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

/**
 * Return the command part from a list of exactly one command
 * and some number of redirections.
 */
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

/**
 * Create a subshell for the given command, which handles the redirections 
 * and calls exec(). Wait for it to finish and collect its return value.
 */
int exec_subshell(redir_command_t* head, char* cmd, char** argv) {
    int pid = fork();
    switch (pid) {
    case -1:
        return EXIT_VALUE_EXEC;
    case 0: {
        int result = apply_redirections(head);
        if (result != EXIT_VALUE_SUCCESS) {
            exit(result);
        }
        execvp(cmd, argv);
        if (errno == ENOENT) {
            fprintf(stderr, "No such file '%s'\n", cmd);
            exit(EXIT_VALUE_NO_FILE);
        } else if (errno == EACCES) {
            fprintf(stderr, "Permission denied: '%s'\n", cmd);
            exit(EXIT_VALUE_PERMISSION);
        }
        exit(EXIT_VALUE_EXEC);
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

/**
 * Opens and redirects files in a command.
 * Non-success exit value implies inconsistent IO state.
 */
int apply_redirections(redir_command_t* head) {
    assert(head != NULL);
    int fd = -1;
    redir_command_node_t* node;
    SLIST_FOREACH(node, head, nodes) {
        switch (node->kind) {
        case RE_INPUT:
            assert(node->filename != NULL && node->filename->word != NULL);
            fd = open(node->filename->word, O_RDONLY);
            IO_EXPECT_OK(fd);
            IO_EXPECT_OK(dup2(fd, 0));
            break;
        case RE_OUTPUT:
            assert(node->filename != NULL && node->filename->word != NULL);
            fd = open(node->filename->word, O_WRONLY | O_CREAT, 0666);
            IO_EXPECT_OK(fd);
            IO_EXPECT_OK(dup2(fd, 1));
            break;
        case RE_APPEND:
            assert(node->filename != NULL && node->filename->word != NULL);
            fd = open(node->filename->word, O_WRONLY | O_CREAT | O_APPEND, 0666);
            IO_EXPECT_OK(fd);
            IO_EXPECT_OK(dup2(fd, 1));
            break;
        default:
            break;
        }
    }
    return EXIT_VALUE_SUCCESS;
}
