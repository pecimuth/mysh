%{
#include <stdio.h>
#include "../include/ast.h"
#include "../include/exec.h"

int yylex();
int yyerror();
%}

%union {
    struct word_node* word_node;
    struct word_node_head word_node_head;
}

%token                          END         0       "end of file"
%token                          SEP                 "; or newline"
%token                          BADSEP
%token<word_node>               WORD                "word"

%type<word_node_head>           command

%%

script:
    %empty
    |
    script SEP
    |
    command                     { print_word_nodes(&$1); execute(&$1); destroy_word_nodes(&$1); }
    |
    script SEP command          { print_word_nodes(&$3); execute(&$3); destroy_word_nodes(&$3); }
    ;

command:
    WORD                        { $$ = make_word_node_head(); prepend_word_node(&$$, $1);}
    |
    WORD command                { $$ = $2; prepend_word_node(&$$, $1); }
    ;

%%

int yyerror(char* str) {
    fprintf(stderr, "parser error: %s\n", str);
    return 1;
}
