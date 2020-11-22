#ifndef FRONT_H
#define FRONT_H
#include <stddef.h>
#include <stdio.h>

void set_args(int argc, char** argv);
size_t get_input(char* buf, size_t size, FILE* yyin);

#endif
