%{
#include <stdio.h>
#include "../include/ast.h"
#include "parser.h"

%}

%%
[a-zA-Z0-9_-]+         { yylval.word_node = make_word_node(yytext); return WORD; }
;|\n                   { return SEP; }   
<<EOF>>                { return END; }                
.                      /* ignore */
%%
