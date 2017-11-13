#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include "parser.h"

char **get_command()
{
    char **strings = NULL;
    int count = 0,
        str_size = 1,
        num_of_args = 0;
    char *str = NULL;
    /* Pushing words into list */
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
                num_of_args++;
                strings = (char**)realloc(strings, num_of_args * sizeof(char*));
                strings[num_of_args-1] = str;
                str = NULL;
                count = 0;
                str_size = 1;
            }
            c = getchar();
            continue;
        }
        count++;
        if (count == str_size) {
            str_size += 10;
            str = (char*)realloc(str, str_size * sizeof(char));
        }
        str[count-1] = c;
        c = getchar();
    }
    if (count > 0) {
        str[count] = '\0';
        num_of_args++;
        strings = (char**)realloc(strings, num_of_args * sizeof(char*));
        strings[num_of_args-1] = str;
    }
    if (strings != NULL) {
        num_of_args++;
        strings = (char**)realloc(strings, num_of_args * sizeof(char*));
        strings[num_of_args-1] = NULL;
    }
    return strings;
}
