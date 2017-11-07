#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include "parser.h"

node *get_commands()
{
    node *root = NULL;
    int count = 0,
        cur_size = 1;
    char *str = NULL;
    int quote = 0;
    int c = getchar();
    while (quote || (c != '\n' && c != EOF)) {
        if (c == '\"') {
            quote = !quote;
            c = getchar();
            continue;
        }
        if (!quote && isspace(c)) {
            if (count > 0) {
                str[count] = '\0';
                insert(&root, str);
            }
            count = 0;
            cur_size = 1;
            str = NULL;
            c = getchar();
            continue;
        }
        count++;
        if (count == cur_size) {
            cur_size += 10;
            str = (char*)realloc(str, cur_size * sizeof(char));
        }
        str[count-1] = c;
        c = getchar();
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
