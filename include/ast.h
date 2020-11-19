#ifndef AST_H
#define AST_H
#include <sys/queue.h>

typedef struct word_node {
    char* word;
    SLIST_ENTRY(word_node) nodes;
} word_node_t;

typedef SLIST_HEAD(word_node_head, word_node) word_node_head_t;

char* alloc_copy(const char* src);
word_node_t* make_word_node(const char* yytext);
void destroy_word_node(word_node_t* node);

word_node_head_t make_word_node_head();
void destroy_word_nodes(word_node_head_t* head);
void prepend_word_node(word_node_head_t* head, word_node_t* node);

void print_word_nodes(word_node_head_t* head);

#endif
