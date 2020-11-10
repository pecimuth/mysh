%{
#include <stdio.h>

int yylex();
int yyerror();
%}

%token EOL
%token CHAR

%%

unit:
    EOL
    | something unit
    ;

something:
    CHAR { printf("CHAR\n"); }
    ;

%%

int yyerror(char* str) {
    fprintf(stderr, "parser error: %s\n", str);
    return 1;
}
