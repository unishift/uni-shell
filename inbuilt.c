#include <unistd.h>

int cd(char **argv)
{
    return chdir(argv[0]);
}
