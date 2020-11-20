#ifndef ENV_H
#define ENV_H

const char* get_pwd();
int change_dir(const char* path);
int change_dir_home();
const char* change_dir_old();

void set_exit_value(int val);
int get_exit_value();

#endif
