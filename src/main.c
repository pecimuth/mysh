#include "../build/parser.h"
#include "../include/ast.h"
#include "../include/env.h"
#include "../include/front.h"

int main(int argc, char** argv) {
    start(argc, argv);
    delete_buffers();
    return get_exit_value();
}
