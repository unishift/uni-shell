#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>

#include "utils.h"

char **get_command()
{
    char **strings = NULL;
    int count = 0,
        str_size = 1,
        num_of_args = 0;
    char *str = NULL;
    /* Filling array with words */
    int quote = 0;
    int c;
    while ((c = getchar()) != EOF && (quote || c != '\n')) {
        if (c == '\"') {
            quote = !quote;
        }
        else if (!quote && isspace(c)) {
            if (count == 0) continue;
            str[count] = '\0';
            num_of_args++;
            strings = (char**)realloc(strings, num_of_args * sizeof(char*));
            strings[num_of_args-1] = str;
            str = NULL;
            count = 0;
            str_size = 1;
        }
        else {
            count++;
            if (count == str_size) {
                str_size += 10;
                str = (char*)realloc(str, str_size * sizeof(char));
            }
            str[count-1] = c;
        }
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
