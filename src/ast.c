#include "ast.h"
#include "env.h"
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

word_node_t* make_word_node(const char* yytext) {
    word_node_t* node = malloc(sizeof(word_node_t));
    if (node == NULL) {
        exit(EXIT_VALUE_MEMORY);
    }
    node->word = strdup(yytext);
    if (node->word == NULL) {
        exit(EXIT_VALUE_MEMORY);
    }
    return node;
}

void destroy_word_node(word_node_t* node) {
    assert(node != NULL);
    if (node->word != NULL) {
        free(node->word);
    }
    free(node);
}

command_t* make_command() {
    command_t* head = malloc(sizeof(command_t));
    if (head == NULL) {
        exit(EXIT_VALUE_MEMORY);
    }
    SLIST_INIT(head);
    return head;
}

void destroy_command(command_t* head) {
    assert(head != NULL);
    while (!SLIST_EMPTY(head)) {
        word_node_t* elem = SLIST_FIRST(head);
        SLIST_REMOVE_HEAD(head, nodes);
        destroy_word_node(elem);
    }
    free(head);
}

void prepend_word_node(command_t* head, word_node_t* node) {
    assert(head != NULL);
    assert(node != NULL);
    SLIST_INSERT_HEAD(head, node, nodes);
}

void print_command(command_t* head) {
    assert(head != NULL);
    printf("[");
    word_node_t* node;
    SLIST_FOREACH(node, head, nodes)
    printf(" %s", node->word);
    printf(" ]\n");
}

redir_command_t* make_redir_command() {
    redir_command_t* head = malloc(sizeof(command_t));
    if (head == NULL) {
        exit(EXIT_VALUE_MEMORY);
    }
    SLIST_INIT(head);
    return head;
}

void destroy_redir_command(redir_command_t* head) {
    assert(head != NULL);
    while (!SLIST_EMPTY(head)) {
        redir_command_node_t* elem = SLIST_FIRST(head);
        SLIST_REMOVE_HEAD(head, nodes);
        destroy_redir_command_node(elem);
    }
}

void prepend_redir_command_node(redir_command_t* head, redir_command_node_t* node) {
    assert(head != NULL);
    assert(node != NULL);
    SLIST_INSERT_HEAD(head, node, nodes);
}

redir_command_node_t* make_redir_command_node(redir_node_kind_t kind, command_t* command, word_node_t* filename) {
    assert(kind == COMMAND || (command == NULL && filename != NULL));
    assert(kind != COMMAND || (command != NULL && filename == NULL));

    redir_command_node_t* node = malloc(sizeof(redir_command_node_t));
    if (node == NULL) {
        exit(EXIT_VALUE_MEMORY);
    }
    node->command = command;
    node->filename = filename;
    return node;
}

void destroy_redir_command_node(redir_command_node_t* node) {
    assert(node != NULL);
    if (node->kind == COMMAND) {
        assert(node->command != NULL);
        assert(node->filename == NULL);
        destroy_command(node->command);
    } else {
        assert(node->command == NULL);
        assert(node->filename != NULL);
        destroy_word_node(node->filename);
    }
    free(node);
}
