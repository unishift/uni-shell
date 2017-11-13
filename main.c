#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/wait.h>

#include "parser.h"

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
    int eof = 0;
    while (!eof) {
        char **cmd = get_command();
        if (cmd != NULL) {
            /* Command execution */
            if (fork() > 0) {
                int status;
                wait(&status);
                /* printf("[ Process exited with code %d ]\n", status); */
            }
            else {
                execvp(cmd[0], cmd);
                /* printf("Process didn't start due to some error\n"); */
                /* Free memory */
                for (int i = 0; cmd[i] != NULL; i++) {
                    free(cmd[i]);
                }
                free(cmd);
                return 1;
            }
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
