#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "libft.h"

char * cmds1[3] = {"/bin/ls",".", NULL};
char * cmds2[3] = {"/usr/bin/wc", NULL};

void	exec_cmd_1()
{
	execve(cmds1[0], cmds1, NULL);
}

void	exec_cmd_2()
{
	execve(cmds2[0], cmds2, NULL);
}

int main()
{
    int fd[2];

    pipe(fd);
    int pid = fork();

    if(pid==0)
	{
        close(fd[0]);
        dup2(fd[1], STDOUT_FILENO);
		exec_cmd_1();
        exit(1);

    }
    else if(pid >0)
	{
		close(fd[1]);
        wait(NULL);
    }

    int pid3 = fork();

    if(pid3==0)
	{
        close(fd[1]);
        dup2(fd[0],STDIN_FILENO);
		exec_cmd_2();
        exit(1);

    }
    else if(pid >0){

		close(fd[1]);
        wait(NULL);

    }
    return 0;
}