%{
#include <stdio.h>
#include "parser.h"
#include "../include/ast.h"
%}

%%
[a-zA-Z0-9_-]+         { alloc_copy(&yylval.word, yytext); return WORD; }
;|\n                   { return SEP; }   
<<EOF>>                { return END; }                
.                      /* ignore */
%%
