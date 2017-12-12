#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <signal.h>

#include "utils.h"

int main(int argc, char **argv)
{
    signal(SIGINT, SIG_IGN);

    if (argc > 1) { /* Replace stdin with file from argv[1] */ 
        int fd = open(argv[1], O_RDONLY);
        if (fd == -1) {
            perror("Error");
            return 1;
        }
        dup2(fd, 0);
    }
    else {
        term = fopen("/dev/tty", "w");
    }

    while (!feof(stdin)) {
        if (term != NULL) {
            char *cur_dir = get_cwd_name();
            fprintf(term, "[ %s ]$ ", cur_dir);
            fflush(term);
            free(cur_dir);
        }

        track_zombies();

        command *cmd = get_command();
        if (cmd == NULL) continue;
        /* Command execution */
        execute_command(cmd);
        /* Free memory */
        free_cmd(cmd);
    }
    /* Terminate remaining processes */
    kill(-getpid(), SIGHUP);

    return 0;
}
