#ifndef ENV_H
#define ENV_H

#define EXIT_VALUE_SUCCESS 0
#define EXIT_VALUE_MEMORY 1
#define EXIT_VALUE_BAD_ENV 2
#define EXIT_VALUE_NO_FILE 3
#define EXIT_VALUE_PERMISSION 4 
#define EXIT_VALUE_INTERRUPT 5
#define EXIT_VALUE_EXEC 6
#define EXIT_VALUE_BAD_ARG 7
#define EXIT_VALUE_OPEN 8

int change_dir(const char* path);
int change_dir_home();
const char* change_dir_old();

void set_exit_value(int val);
int get_exit_value();

#endif
