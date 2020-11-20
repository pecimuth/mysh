#include "../include/exec.h"
#include "../include/env.h"
#include <assert.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

void execute(word_node_head_t* head) {
    assert(head != NULL);
    word_node_t* node = SLIST_FIRST(head);
    assert(node != NULL);
    assert(node->word != NULL);

    int argc;
    char** argv;

    prepare_args(head, &argc, &argv);

    if (strcmp(node->word, "exit") == 0) {
        exec_exit(argc, argv);
    } else if (strcmp(node->word, "pwd") == 0) {
        exec_pwd(argc, argv);
    } else if (strcmp(node->word, "cd") == 0) {
        exec_cd(argc, argv);
    }

    free(argv);
}

void prepare_args(word_node_head_t* head, int* argc, char*** argv) {
    *argc = 0;
    word_node_t* node;
    SLIST_FOREACH(node, head, nodes)
        ++*argc;

    *argv = (char**)malloc(sizeof(char*) * *argc);
    assert(*argv != NULL);

    char** it = *argv;
    SLIST_FOREACH(node, head, nodes) {
        *it = node->word;
        ++it;
    }
}

int exec_exit(int argc, char** argv) {
    (void)argc;
    (void)argv;
    return 1;
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
    return 1;
}
