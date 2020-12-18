#ifndef AST_H
#define AST_H
#include <sys/queue.h>

typedef struct word_node {
    char* word;
    SLIST_ENTRY(word_node) nodes;
} word_node_t;

typedef SLIST_HEAD(command, word_node) command_t;

word_node_t* make_word_node(const char* yytext);
void destroy_word_node(word_node_t* node);

command_t* make_command();
void destroy_command(command_t* head);
void prepend_word_node(command_t* head, word_node_t* node);

typedef enum redir_node_kind {
    COMMAND,
    RE_INPUT,
    RE_OUTPUT,
    RE_APPEND
} redir_node_kind_t;

typedef struct redir_command_node {
    redir_node_kind_t kind;
    command_t* command; // set iff kind == COMMAND
    word_node_t* filename; // set iff kind == RE_*
    SLIST_ENTRY(redir_command_node) nodes;
} redir_command_node_t;

typedef SLIST_HEAD(redir_command, redir_command_node) redir_command_t;

redir_command_t* make_redir_command();
void destroy_redir_command(redir_command_t* head);
void prepend_redir_command_node(redir_command_t* head, redir_command_node_t* node);
redir_command_node_t* make_redir_command_node(redir_node_kind_t kind, command_t* command, word_node_t* filename);
void destroy_redir_command_node(redir_command_node_t* node);

typedef struct pipe_command_node {
    redir_command_t* redir_command;
    SLIST_ENTRY(pipe_command_node) nodes;
} pipe_command_node_t;

typedef SLIST_HEAD(pipe_command, pipe_command_node) pipe_command_t;

pipe_command_t* make_pipe_command();
void prepend_redir_command(pipe_command_t* head, redir_command_t* redir_command);
void destroy_pipe_command_node(pipe_command_node_t* node);
void destroy_pipe_command(pipe_command_t* head);

#endif
