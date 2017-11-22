#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <signal.h>

#include "utils.h"

int main(int argc, char **argv)
{
    signal(SIGINT, SIG_IGN);

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
        /* */
        command_list *list = get_command();
        if (list == NULL) continue;
        /* Command execution */
        for (command_list *p = list; p != NULL; p = p->next) {
            int status = execute_command(p->cmd);
            /* Handle logic links */
            if (p->link == OR && status == 0) {
                while (p->link == OR)
                    p = p->next;
            }
            if (p->link == AND && status != 0) {
                while (p->link == AND)
                    p = p->next;
            }
        }
        /* Free memory */
        free_list(list);
    }
    return 0;
}
