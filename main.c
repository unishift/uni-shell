#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>

#include "utils.h"

int main(int argc, char **argv)
{
    if (argc > 1) { /* Replace stdin with file from argv[1] */ 
        int fd = open(argv[1], O_RDONLY);
        if (fd == -1) {
            printf("Couldn't read from file %s\n", argv[1]);
            return 1;
        }
        dup2(fd, 0);
    }
    /* */
    while (1) {
        /* printf("%s %s $ ", getenv("USER"), getenv("PWD")); */
        printf("$ ");
        char **cmd = get_command();
        if (cmd != NULL) {
            /* Command execution */
            execute_command(cmd);
            /* Free memory */
            for (int i = 0; cmd[i] != NULL; i++) {
                free(cmd[i]);
            }
            free(cmd);
        }
        if (feof(stdin)) break;
    } 
    return 0;
}
