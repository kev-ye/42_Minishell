#include <sys/stat.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <sys/errno.h>
#include <stdlib.h>

int main(int ac, char **av, char **env)
{
    printf("%s\n", getenv("lol"));
    return (0);
}