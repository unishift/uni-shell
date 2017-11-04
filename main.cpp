#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char *get_string()
{
    int c, count = 0;
    char *str = (char*)calloc(1, sizeof(char));
    int cur_size = 1;
    while ( (c = getchar()) != '\n' && c != EOF) {
        count++;
        if (count == cur_size - 1) {
            cur_size += 10;
            str = (char*)realloc(str, cur_size);
        }
        str[count-1] = c;
    }
    if (c == EOF && cur_size == 1) {
        free(str);
        return NULL;
    }
    str[count] = '\0';
    return str;
}

int main()
{
    char *cur_dir,
         *tmp = getenv("HOME");
    cur_dir = (char*)calloc(strlen(tmp), sizeof(char));
    strcpy(cur_dir, tmp);
//
    for (;;) {
        printf("%s@%s:%s$ ", getenv("USER"),getenv("HOSTNAME"), cur_dir);
        tmp = get_string();
        if (tmp == NULL) break;
    } 
    free(cur_dir);
    free(tmp);
    return 0;
}
