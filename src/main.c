#include "../include/ast.h"
#include "../build/parser.h"
#include "../include/env.h"

int main() {
    yyparse();
    return get_exit_value();
}
