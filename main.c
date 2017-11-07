#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include "list.h"
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
        node *cmd_list = get_commands();
        for (node *p = cmd_list; p != NULL; p = p->next) {
            if (p->str == NULL) {
                eof = 1;
                continue;
            }
            printf("%s ", p->str);
        }
        if (!eof) putchar('\n');
        delete_list(cmd_list);
    } 
    return 0;
}
