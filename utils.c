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
    CON, /* Conveyor */
    END
} token;


void free_cmd(command *cmd)
{
    if (cmd->next != NULL) free_cmd(cmd->next);
    for (int i = 0; cmd->argv[i] != NULL; i++) {
        free(cmd->argv[i]);
    }
    if (cmd->input_file != -1) close(cmd->input_file);
    if (cmd->output_file != -1) close(cmd->output_file);
    free(cmd->argv);
    free(cmd);
}

token get_word(char **ptr)
{
    *ptr = NULL;
    int str_size = 1;
    int quote = 0;
    int count = 0;
    int c;
    while (isspace(c = getchar()) && c != '\n'); /* Skip spaces */
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
                case '|':
                    if (*ptr != NULL) (*ptr)[count] = '\0';
                    return CON;
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
    /* Init command */
    command *root = (command*)malloc(sizeof(command));
    command *tmp = root;
    tmp->argv = (char**)malloc(sizeof(char*));
    tmp->argv[0] = NULL;
    tmp->input_file = -1;
    tmp->output_file = -1;
    tmp->next = NULL;
    /* */
    int require_command = 0;
    int dont_stop = 0;
    int num_of_args = 1; /* With last NULL */
    char *str = NULL;
    token tk;
    do {
        tk = get_word(&str);
        if (str != NULL) {
            require_command = 0;
            dont_stop = 0;
            num_of_args++;
            tmp->argv = (char**)realloc(tmp->argv, num_of_args * sizeof(char*));
            tmp->argv[num_of_args-2] = str;
            tmp->argv[num_of_args-1] = NULL;
        }
        if (tk != END && tk != WORD && require_command) {
            free_cmd(root);
            fprintf(stderr, "Error: expected command\n");
            /* Skip string */
            char c;
            while ((c = getchar()) != '\n' && c != EOF);
            return NULL;
        }
        int fd[2];
        switch (tk) {
            case IN:
                tk = get_word(&str);
                if (str != NULL) {
                    int fd = open(str, O_RDONLY);
                    free(str);
                    if (fd != -1) {
                        if (tmp->input_file != -1) close(tmp->input_file);
                        tmp->input_file = fd;
                    }
                    else { /* Error handle */
                        perror("Error");
                        free_cmd(root);
                        return NULL;
                    }
                }
                else { /* Syntax error */
                    fprintf(stderr, "Error: unexpected EOF\n");
                    free_cmd(root);
                    return NULL;
                }
                break;
            case OUT:
                tk = get_word(&str);
                if (str != NULL) {
                    int fd = open(str, O_WRONLY | O_TRUNC | O_CREAT, 0666);
                    free(str);
                    if (fd != -1) {
                        if (tmp->output_file != -1) close(tmp->output_file);
                        tmp->output_file = fd;
                    }
                    else { /* Error handle */
                        perror("Error");
                        free_cmd(root);
                        return NULL;
                    }
                }
                else { /* Syntax error */
                    fprintf(stderr, "Error: unexpected EOF\n");
                    free_cmd(root);
                    return NULL;
                }
                break;
            case CON:
                require_command = 1;
                dont_stop = 1;
                pipe(fd);
                tmp->output_file = fd[1];
                /* Init next command */
                tmp = tmp->next = (command*)malloc(sizeof(command));
                tmp->input_file = fd[0];
                tmp->output_file = -1;
                tmp->argv = (char**)malloc(sizeof(char*));
                tmp->argv[0] = NULL;
                tmp->next = NULL;
                num_of_args = 1;
                break;
            default:
                if (dont_stop) putchar('>');
        }
    } while (tk != END || dont_stop);
    /* */
    if (root->argv[0] == NULL) {
        free_cmd(root);
        root = NULL;
    }
    return root;
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
        /* Close files */
        if (cmd->input_file != -1) close(cmd->input_file);
        if (cmd->output_file != -1) close(cmd->output_file);
        /* */
        if (cmd->next == NULL) {
            while (wait(&status) != -1);
            return status;
        }
        else {
            return execute_command(cmd->next);
        }
    }
    else if (child == 0) { /* Child branch */
        if (cmd->input_file != -1) { /* Input redirection */
            dup2(cmd->input_file, 0);
        }
        if (cmd->output_file != -1) { /* Output redirection */
            dup2(cmd->output_file, 1);
        }
        execvp(cmd->argv[0], cmd->argv);
        perror("Error");
        exit(1);
    } 
    else { /* Error branch */
        perror("Error");
        return -1;
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
