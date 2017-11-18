#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <unistd.h>
#include <sys/wait.h>
#include <string.h>

#include "inbuilt.h"
#include "utils.h"

typedef enum token {
    WORD,
    IN, /* Input stream redirection */
    OUT, /* Output stream redirection */
    END
} token;

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
                /* case '<': */
                /*     if (*ptr != NULL) (*ptr)[count] = '\0'; */
                /*     return IN; */
                /* case '>': */
                /*     if (*ptr != NULL) (*ptr)[count] = '\0'; */
                /*     return OUT; */
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

char **get_command()
{
    char **strings = NULL;
    int num_of_args = 0;
    char *str = NULL;
    token tk;
    do {
        tk = get_word(&str);
        if (str != NULL) {
            num_of_args++;
            strings = (char**)realloc(strings, num_of_args * sizeof(char*));
            strings[num_of_args-1] = str;
        }
    } while (tk != END);
    
    if (num_of_args > 0) {
        num_of_args++;
        strings = (char**)realloc(strings, num_of_args * sizeof(char*));
        strings[num_of_args-1] = NULL;
    }
    return strings;
}

int execute_command(char **cmd)
{
    int status;
    if (strcmp(cmd[0], "cd") == 0) {
         status = cd(cmd + 1);
         if (status == -1){
             perror("Error");
         }
         return status;
    }
    if (strcmp(cmd[0], "exit") == 0) {
        for (int i = 0; cmd[i] != NULL; i++) {
            free(cmd[i]);
        }
        free(cmd);

        exit(0);
    }
    pid_t child;
    if ((child = fork()) > 0) {
        wait(&status);
        return status;
    }
    else if (child == 0) { /* Child branch */
        execvp(cmd[0], cmd);
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
