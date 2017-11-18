#pragma once

typedef struct command {
    char **argv;
    char *input_file;
    char *output_file;
    struct command *next;
} command;

void free_cmd(command *cmd);
command *get_command();
int execute_command(command *cmd);
char *get_cwd_name();
