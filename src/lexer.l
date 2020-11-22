%{
#include <stdio.h>
#include "../include/ast.h"
#include "../include/front.h"
#include "parser.h"
%}

%option nounput
%option noinput

%%
[a-zA-Z0-9_/.+-]+      { yylval.word_node = make_word_node(yytext); return WORD; }
;                      { return SEMIC; }
\n                     { return LF; }
<<EOF>>                { return END; }
.                      /* ignore */
%%
