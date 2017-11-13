#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
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
            for (int i = 0; cmd[i] != NULL; i++) {
                printf("%s ", cmd[i]);
            }
            for (int i = 0; cmd[i] != NULL; i++) {
                free(cmd[i]);
            }
            free(cmd);
        }
        if (feof(stdin)) break;
    } 
    return 0;
}
