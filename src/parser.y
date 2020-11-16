%{
#include <stdio.h>

int yylex();
int yyerror();
%}

%union {
    char* word;
}

%token                          END         0       "end of file"
%token                          SEP                 "; or newline"
%token<word>                    WORD                "word"

%type<word>                     command

%%

script:
    %empty
    |
    script SEP
    |
    command                     { printf("command: %s\n", $1); }
    |
    script SEP command          { printf("command: %s\n", $3); }
    ;

command:
    WORD                        { $$ = $1;}
    |
    command WORD                { $$ = $1;}
    ;

%%

int yyerror(char* str) {
    fprintf(stderr, "parser error: %s\n", str);
    return 1;
}
