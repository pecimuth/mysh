#ifndef ENV_H
#define ENV_H

const char* get_pwd();
void set_pwd(const char* new_pwd);
void set_old_pwd();

const char* find_in_path(const char* cmd);

#endif
