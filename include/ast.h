#ifndef AST_H
#define AST_H
#include <sys/queue.h>

typedef struct word_node {
    char* word;
    SLIST_ENTRY(word_node) nodes;
} word_node_t;

void alloc_copy(char** dest, const char* src);
word_node_t* make_word_node(const char* yytext);

#endif
