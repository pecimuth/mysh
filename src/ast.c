#include "../include/ast.h"
#include "../include/env.h"
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

word_node_head_t make_word_node_head() {
    word_node_head_t head = SLIST_HEAD_INITIALIZER(head);
    SLIST_INIT(&head);
    return head;
}

void destroy_word_nodes(word_node_head_t* head) {
    assert(head != NULL);
    while (!SLIST_EMPTY(head)) {
        word_node_t* elem = SLIST_FIRST(head);
        SLIST_REMOVE_HEAD(head, nodes);
        destroy_word_node(elem);
    }
}

void prepend_word_node(word_node_head_t* head, word_node_t* node) {
    assert(head != NULL);
    assert(node != NULL);
    SLIST_INSERT_HEAD(head, node, nodes);
}

void print_word_nodes(word_node_head_t* head) {
    assert(head != NULL);
    printf("[");
    word_node_t* node;
    SLIST_FOREACH(node, head, nodes)
    printf(" %s", node->word);
    printf(" ]\n");
}
