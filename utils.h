#pragma once

typedef struct {
    char **argv;
    char *input_file;
    char *output_file;
} command;

command get_command();
int execute_command(command cmd);
char *get_cwd_name();
