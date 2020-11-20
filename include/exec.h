#ifndef EXEC_H
#define EXEC_H
#include "ast.h"

void execute(word_node_head_t* head);

void prepare_args(word_node_head_t* head, int* argc, char*** argv);

int exec_exit(int argc, char** argv);
int exec_pwd(int argc, char** argv);
int exec_cd(int argc, char** argv);

#endif
