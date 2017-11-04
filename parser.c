#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include "parser.h"

node *get_commands()
{
    node *root = NULL;
    int c, count = 0,
        cur_size = 1;
    char *str = NULL;
    while ( (c = getchar()) != '\n' && c != EOF) {
        if (isspace(c)) {
            if (count > 0) {
                str[count] = '\0';
                insert(&root, str);
            }
            count = 0;
            cur_size = 1;
            str = NULL;
            continue;
        }
        count++;
        if (count == cur_size) {
            cur_size += 10;
            str = (char*)realloc(str, cur_size * sizeof(char));
        }
        str[count-1] = c;
    }
    if (count > 0) {
        str[count] = '\0';
        insert(&root, str);
    }
    if (c == EOF) {
        insert(&root, NULL); /* Stop program sign */
    }
    return root;
}
