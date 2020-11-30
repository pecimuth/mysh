#include "../build/parser.h"
#include "../include/ast.h"
#include "../include/env.h"
#include "../include/front.h"

int main(int argc, char** argv) {
    start(argc, argv);
    return get_exit_value();
}
