#include "../include/exec.h"
#include "../include/env.h"
#include <assert.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <errno.h>

void execute(word_node_head_t* head) {
    assert(head != NULL);
    word_node_t* node = SLIST_FIRST(head);
    assert(node != NULL);
    assert(node->word != NULL);

    int argc;
    char** argv;

    prepare_args(head, &argc, &argv);
    int exit_value = 1;

    if (strcmp(node->word, "exit") == 0) {
        exit_value = exec_exit(argc, argv);
    } else if (strcmp(node->word, "pwd") == 0) {
        exit_value = exec_pwd(argc, argv);
    } else if (strcmp(node->word, "cd") == 0) {
        exit_value = exec_cd(argc, argv);
    } else {
        int pid = fork();
        if (pid == -1) {
            exit_value = 1;
        } else if (pid == 0) {
            execvp(node->word, argv);
            if (errno == EACCES) {
                printf("%s not found\n", node->word);
            }
            exit(1);
        } else {
            int wstatus;
            exit_value = waitpid(pid, &wstatus, 0);
            if (exit_value == 0) {
                exit_value = WEXITSTATUS(wstatus);
            }
        }
    }

    free(argv);
    set_exit_value(exit_value);
}

void prepare_args(word_node_head_t* head, int* argc, char*** argv) {
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

int exec_exit(int argc, char** argv) {
    (void)argc;
    (void)argv;
    exit(get_exit_value());
    return 0;
}

int exec_pwd(int argc, char** argv) {
    (void)argv;
    if (argc != 1) {
        return 1;
    }
    printf("%s\n", get_pwd());
    return 0;
}

int exec_cd(int argc, char** argv) {
    (void)argc;
    (void)argv;

    if (argc == 1) {
        return change_dir_home();
    } else if (argc == 2) {
        if (strcmp(argv[1], "-") == 0) {
            const char* old = change_dir_old();
            if (old == NULL) {
                return 1;
            }
            printf("%s\n", old);
            return 0;
        }

        return change_dir(argv[1]);
    }
    return 1;
}
