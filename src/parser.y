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
%token                          SEMIC               ";"
%token                          LF                  "line feed"
%token<word_node>               WORD                "word"

%type<word_node_head>           command

%%

script:
    %empty
    |
    LF
    |
    error LF
    |
    script script_line SEMIC LF
    |
    script script_line LF
    ;

script_line:
    command                     { execute(&$1); destroy_word_nodes(&$1); }
    |
    script_line SEMIC command   { execute(&$3); destroy_word_nodes(&$3); }
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
