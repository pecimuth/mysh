#ifndef FRONT_H
#define FRONT_H
#include <stddef.h>
#include <stdio.h>

void start(int argc, char** argv);
void parse_file(char* filename);
size_t get_input(char* buf, size_t size, FILE* yyin);
void delete_buffers();

void make_prompt(char* buf, size_t size);
char* read_line_or_reset(char* prompt);
char* make_rl_buf(char* line);

#endif
