#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>

int cd(char **argv)
{
    char *path;
    if (argv[0] == NULL) {
        path = getenv("HOME");
    }
    else if (argv[1] != NULL) {
        errno = E2BIG; /* Too many arguments */
        return -1;
    }
    else {
        path = (char*)calloc(strlen(argv[0]) + 1, sizeof(char));
        strcpy(path, argv[0]);
    }
    int status = chdir(path); 
    if (argv[0] != NULL) free(path);
    return status;
}
