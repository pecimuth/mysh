#include "../include/ast.h"
#include "../build/parser.h"
#include "../include/env.h"
#include "../include/front.h"

int main(int argc, char** argv) {
    set_args(argc, argv);
    return get_exit_value();
}
