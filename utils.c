#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <unistd.h>
#include <sys/wait.h>
#include <string.h>

#include "inbuilt.h"
#include "utils.h"

char **get_command()
{
    char **strings = NULL;
    int count = 0,
        str_size = 1,
        num_of_args = 0;
    char *str = NULL;
    /* Filling array with words */
    int quote = 0;
    int c;
    while ((c = getchar()) != EOF && (quote || c != '\n')) {
        if (c == '\"') {
            quote = !quote;
        }
        else if (!quote && isspace(c)) {
            if (count == 0) continue;
            str[count] = '\0';
            num_of_args++;
            strings = (char**)realloc(strings, num_of_args * sizeof(char*));
            strings[num_of_args-1] = str;
            str = NULL;
            count = 0;
            str_size = 1;
        }
        else {
            count++;
            if (count == str_size) {
                str_size += 10;
                str = (char*)realloc(str, str_size * sizeof(char));
            }
            str[count-1] = c;
        }
    }
    if (count > 0) {
        str[count] = '\0';
        num_of_args++;
        strings = (char**)realloc(strings, num_of_args * sizeof(char*));
        strings[num_of_args-1] = str;
    }
    if (strings != NULL) {
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

void print_cwd_name()
{
    int size = 256;
    char *cur_dir = (char*)calloc(size, sizeof(char));
    while (getcwd(cur_dir, size) == NULL) {
        size += 100;
        cur_dir = (char*)realloc(cur_dir, size * sizeof(char));
    }
    int shift;
    for (int i = 0; cur_dir[i] != '\0'; i++) {
        if (cur_dir[i] == '/') {
            shift = i;
        }
    }
    if (cur_dir[shift + 1] == '\0') {
        printf("/ $ ");
    }
    else {
        printf("..%s $ ", cur_dir + shift);
    }
    free(cur_dir);
}
