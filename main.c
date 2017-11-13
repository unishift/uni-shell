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
            pid_t child;
            if ( (child = fork()) > 0) { /* Parent branch */
                int status;
                wait(&status);
                /* printf("[ Process exited with code %d ]\n", status); */
            }
            else if (child == 0) { /* Child branch */
                execvp(cmd[0], cmd);
                printf("Error: Process didn't start\n");
                /* Free memory */
                for (int i = 0; cmd[i] != NULL; i++) {
                    free(cmd[i]);
                }
                free(cmd);
                return 1;
            }
            else { /* Error branch */
                printf("Error: Couldn't create new process\n");
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
