#ifndef EXEC_H
#define EXEC_H
#include "ast.h"

void execute_redir_command(redir_command_t* head);
void execute_command(command_t* head);

void prepare_args(command_t* head, int* argc, char*** argv);

int exec_pwd(int argc);
int exec_cd(int argc, char** argv);

int exec_subshell(char* cmd, char** argv);

#endif
