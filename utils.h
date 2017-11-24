#pragma once

extern FILE *term;

typedef enum token {
    WORD,
    IN, /* Input stream redirection */
    OUT, /* Output stream redirection */
    AOUT, /* Append */
    CON, /* Pipeline */
    BCKG, /* Background */
    SEP, /* Separator */
    OR,
    AND,
    SUBSH_ST,
    SUBSH_END,
    END
} token;

typedef struct command {
    char **argv;
    struct command *subshell;
    int input_file;
    int output_file;
    int backgr;
    token link;
    struct command *next;
} command;

void free_cmd(command *cmd);
void free_list(command *list);
command *get_command();
int execute_command(command *cmd);
char *get_cwd_name();
