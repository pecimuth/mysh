%code requires {
#include "ast.h"
}

%{
#include <stdio.h>
#include "env.h"
#include "exec.h"

int yylex();
void yyerror();
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
    error LF
    |
    SEMIC LF { clear_lexer_error(); }
    |
    script LF { clear_lexer_error(); }
    |
    script script_line SEMIC LF { clear_lexer_error(); }
    |
    script script_line LF { clear_lexer_error(); }
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

void yyerror(char* str) {
    fprintf(stderr, "parser error: %s\n", str);
    set_exit_value(EXIT_VALUE_PARSER);
}
