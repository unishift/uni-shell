#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <unistd.h>
#include <sys/wait.h>
#include <string.h>
#include <fcntl.h>

#include "inbuilt.h"
#include "utils.h"

typedef enum token {
    WORD,
    IN, /* Input stream redirection */
    OUT, /* Output stream redirection */
    /* CON, #<{(| Conveyor |)}># */
    END
} token;


void free_cmd(command *cmd)
{
    if (cmd->next != NULL) free_cmd(cmd->next);
    if (cmd->argv != NULL) {
        for (int i = 0; cmd->argv[i] != NULL; i++) {
            free(cmd->argv[i]);
        }
        free(cmd->argv);
    }
    free(cmd->input_file);
    free(cmd->output_file);
    free(cmd);
}

token get_word(char **ptr)
{
    *ptr = NULL;
    int str_size = 1;
    int quote = 0;
    int count = 0;
    int c;
    while (isspace(c = getchar())); /* Skip spaces */
    do {
        if (!quote) {
            switch (c) {
                case '\"':
                    quote = 1;
                    continue;
                case '<':
                    if (*ptr != NULL) (*ptr)[count] = '\0';
                    return IN;
                case '>':
                    if (*ptr != NULL) (*ptr)[count] = '\0';
                    return OUT;
                /* case '|': */
                /*     if (*ptr != NULL) (*ptr)[count] = '\0'; */
                /*     return CON; */
                case EOF: case '\n':
                    if (*ptr != NULL) (*ptr)[count] = '\0';
                    return END;
            }
            if (isspace(c)) {
                if (*ptr != NULL) (*ptr)[count] = '\0';
                return WORD;
            }
        }
        else if (c == '\"') {
            quote = 0;
            continue;
        }
        count++;
        if (count == str_size) {
            str_size += 10;
            *ptr = (char*)realloc(*ptr, str_size * sizeof(char));
        }
        (*ptr)[count-1] = c;
    } while ((c = getchar()) != EOF);
    /* EOF encountered */
    (*ptr)[count] = '\0';
    return END;
}

command *get_command()
{
    command *tmp = (command*)malloc(sizeof(command));
    tmp->argv = NULL;
    tmp->input_file = NULL;
    tmp->output_file = NULL;
    tmp->next = NULL;
    int num_of_args = 0;
    char *str = NULL;
    token tk;
    do {
        tk = get_word(&str);
        if (str != NULL) {
            num_of_args++;
            tmp->argv = (char**)realloc(tmp->argv, num_of_args * sizeof(char*));
            tmp->argv[num_of_args-1] = str;
        }
        switch (tk) {
            case IN:
                tk = get_word(&str);
                if (str != NULL) {
                    tmp->input_file = str;
                }
                else {
                    /* Syntax error */
                }
                break;
            case OUT:
                tk = get_word(&str);
                if (str != NULL) {
                    tmp->output_file = str;
                }
                else {
                    /* Syntax error */
                }
                break;
            default: ;
        }
    } while (tk != END);
    /* End of array */
    if (num_of_args > 0) {
        num_of_args++;
        tmp->argv = (char**)realloc(tmp->argv, num_of_args * sizeof(char*));
        tmp->argv[num_of_args-1] = NULL;
    }
    /* */
    return tmp;
}

int execute_command(command *cmd)
{
    int status;
    if (strcmp(cmd->argv[0], "cd") == 0) {
         status = cd(cmd->argv + 1);
         if (status == -1){
             perror("Error");
         }
         return status;
    }
    if (strcmp(cmd->argv[0], "exit") == 0) {
        free_cmd(cmd);
        exit(0);
    }
    pid_t child;
    if ((child = fork()) > 0) {
        wait(&status);
        return status;
    }
    else if (child == 0) { /* Child branch */
        if (cmd->input_file != NULL) {
            int fd = open(cmd->input_file, O_RDONLY);
            dup2(fd, 0);
            free(cmd->input_file);
        }
        if (cmd->output_file != NULL) {
            int fd = open(cmd->output_file, O_WRONLY | O_TRUNC | O_CREAT, 0666);
            dup2(fd, 1);
            free(cmd->output_file);
        }
        execvp(cmd->argv[0], cmd->argv);
        perror("Error");
        exit(1);
    } 
    else { /* Error branch */
        perror("Error");
        exit(1);
    }
}

char *get_cwd_name()
{
    int size = 256;
    char *cur_dir = (char*)calloc(size, sizeof(char));
    while (getcwd(cur_dir, size) == NULL) {
        size += 100;
        cur_dir = (char*)realloc(cur_dir, size * sizeof(char));
    }
    return cur_dir;
}
