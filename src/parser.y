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
    struct command* command;
    struct redir_command* redir_command;
    struct redir_command_node* redir_command_node;
    struct pipe_command* pipe_command;
}

%token                          END         0       "end of file"
%token                          SEMIC               ";"
%token                          LF                  "line feed"
%token                          PIPE                "|"
%token                          LT                  "<"
%token                          GT                  ">"
%token                          DGT                 ">>"
%token<word_node>               WORD                "word"

%type<command>                  command
%type<redir_command>            redir_command
%type<redir_command>            redir_command_suffix
%type<redir_command_node>       redir_command_node
%type<pipe_command>             pipe_command

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
    pipe_command { execute($1); /*print_redir_command($1); destroy_redir_command($1); */ }
    |
    script_line SEMIC pipe_command { execute($3); /*print_redir_command($3); destroy_redir_command($3); */ }
    ;

pipe_command:
    redir_command { $$ = make_pipe_command(); prepend_redir_command($$, $1); }
    |
    pipe_command PIPE redir_command { $$ = $1; prepend_redir_command($$, $3); }
    ;

redir_command_suffix:
    %empty { $$ = make_redir_command(); }
    |
    redir_command_node redir_command_suffix { $$ = $2; prepend_redir_command_node($$, $1); }
    ;

redir_command:
    command redir_command_suffix {$$ = $2; prepend_redir_command_node($$, make_redir_command_node(COMMAND, $1, NULL)); }
    |
    redir_command_node redir_command { $$ = $2; prepend_redir_command_node($$, $1); }
    ;

redir_command_node:
    LT WORD { $$ = make_redir_command_node(RE_INPUT, NULL, $2); }
    |
    GT WORD { $$ = make_redir_command_node(RE_OUTPUT, NULL,  $2); }
    |
    DGT WORD { $$ = make_redir_command_node(RE_APPEND, NULL, $2); }
    ;

command:
    WORD { $$ = make_command(); prepend_word_node($$, $1);}
    |
    WORD command { $$ = $2; prepend_word_node($$, $1); }
    ;

%%

void yyerror(char* str) {
    fprintf(stderr, "parser error: %s\n", str);
    set_exit_value(EXIT_VALUE_PARSER);
}
