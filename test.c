#include <sys/stat.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <sys/errno.h>
#include <stdlib.h>

extern char**environ;

int main ()
{
    char **var;

    for (var =environ;*var !=NULL;++var)
        printf ("%s\n",*var);
    return 0;
}