#pragma once

typedef struct command {
    char **argv;
    int input_file;
    int output_file;
    int backgr;
    struct command *next;
} command;

typedef struct command_list {
    command *cmd;
    struct command_list *next;
} command_list;

void free_cmd(command *cmd);
void free_list(command_list *list);
command_list *get_command();
int execute_command(command *cmd);
char *get_cwd_name();
