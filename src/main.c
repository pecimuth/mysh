#include "parser.h"
#include "ast.h"
#include "env.h"
#include "front.h"

int main(int argc, char** argv) {
    start(argc, argv);
    delete_buffers();
    return get_exit_value();
}
