#ifndef EXEC_H
#define EXEC_H
#include "ast.h"

void execute(redir_command_t* head);

command_t* get_command(redir_command_t* head);
void prepare_args(command_t* head, int* argc, char*** argv);

int exec_pwd(int argc);
int exec_cd(int argc, char** argv);

int exec_subshell(redir_command_t* head, char* cmd, char** argv);
int apply_redirections(redir_command_t* head);

#endif
