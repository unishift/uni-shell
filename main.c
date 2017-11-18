#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>

#include "utils.h"

int main(int argc, char **argv)
{
    int fr_file = 0;
    if (argc > 1) { /* Replace stdin with file from argv[1] */ 
        fr_file = 1;
        int fd = open(argv[1], O_RDONLY);
        if (fd == -1) {
            perror("Error");
            return 1;
        }
        dup2(fd, 0);
    }
    /* */
    while (!feof(stdin)) {
        if (!fr_file) {
            char *cur_dir = get_cwd_name();
            int shift;
            for (int i = 0; cur_dir[i] != '\0'; i++) {
                if (cur_dir[i] == '/') {
                    shift = i;
                }
            }
            if (cur_dir[1] == '\0') { /* Root directory */
                printf("[ / ]$ ");
            }
            else {
                printf("[ %s ]$ ", cur_dir + shift + 1);
            }
            free(cur_dir);
        }
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
    }
    return 0;
}
