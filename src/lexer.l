%{
#include <stdio.h>
#include "../include/ast.h"
#include "../include/front.h"
#include "parser.h"

#define YY_INPUT(buf,result,max_size) result = get_input(buf, max_size, yyin);

%}

%%
[a-zA-Z0-9_/.+-]+      { yylval.word_node = make_word_node(yytext); return WORD; }
;                      { return SEMIC; }
\n                     { return LF; }
<<EOF>>                { return END; }
.                      /* ignore */
%%
