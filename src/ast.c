#include "../include/ast.h"
#include <stdlib.h>
#include <string.h>
#include <assert.h>

void alloc_copy(char** dest, const char* src) {
    *dest = malloc(strlen(src) + 1);
    assert(*dest != NULL);
    strcpy(*dest, src);
}

word_node_t* make_word_node(const char* yytext) {
    word_node_t* node = malloc(sizeof(word_node_t));
    assert(node != NULL);
    alloc_copy(&node->word, yytext);
    return node;
}
