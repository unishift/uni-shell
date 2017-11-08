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
<<<<<<< HEAD
    /* Pushing words into list */
    while ( (c = getchar()) != '\n' && c != EOF) {
        if (isspace(c)) {
=======
    int quote = 0;
    int c = getchar();
    while (quote || (c != '\n' && c != EOF)) {
        if (c == '\"') {
            quote = !quote;
            c = getchar();
            continue;
        }
        if (!quote && isspace(c)) {
>>>>>>> 329c86ff9cabea784dbc28f4e0348a7ea59c9bad
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
        insert(&root, NULL); /* End program sign */
    }
    return root;
}
