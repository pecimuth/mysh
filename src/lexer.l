%{
#include <stdio.h>
#include "../include/ast.h"
#include "../include/env.h"
#include "../include/front.h"
#include "parser.h"
%}

%option nounput
%option noinput

%x comment

%%
<comment>{
    \n                 { BEGIN(INITIAL); return LF; }
    <<EOF>>            { return END; }
    .                  /* ignore */
}
[a-zA-Z0-9_/.+-]+      { yylval.word_node = make_word_node(yytext); return WORD; }
#                      { BEGIN(comment); }
;                      { return SEMIC; }
\n                     { return LF; }
<<EOF>>                { return END; }
[ \t\r\f]+             /* ignore */
.                      { unexpected_character(yytext[0]); }
%%
