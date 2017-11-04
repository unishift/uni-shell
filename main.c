#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "list.h"
#include "parser.h"

int main()
{
    char *cur_dir,
         *tmp = getenv("HOME");
    cur_dir = (char*)calloc(strlen(tmp) + 1, sizeof(char));
    strcpy(cur_dir, tmp);
    int eof = 0;
    while (!eof) {
        node *cmd_list = get_commands();
        /* printf("%s@%s:%s$ ", getenv("USER"), getenv("HOSTNAME"), cur_dir); */
        for (node *p = cmd_list; p != NULL; p = p->next) {
            if (p->str == NULL) {
                eof = 1;
                continue;
            }
            printf("%s ", p->str);
        }
        putchar('\n');
        delete_list(cmd_list);
    } 
    free(cur_dir);
    return 0;
}
