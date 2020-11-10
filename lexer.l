%{
#include <stdio.h>
#include "parser.h"
%}

%%
.               { return CHAR; }
<<EOF>>         { return EOL; }
%%
