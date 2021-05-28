#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "libft.h"

char *cmds1[3] = {"/bin/ls",".", NULL};
char *cmds2[3] = {"/bin/cat", NULL};
char *cmds3[3] = {"/usr/bin/wc", NULL};

void	exec1()
{
	execve(cmds1[0], cmds1, NULL);
}

void	exec2()
{
	execve(cmds2[0], cmds2, NULL);
}

void	exec3()
{
	execve(cmds3[0], cmds3, NULL);
}

int main()
{
	int fd[2];
	int fd2[2];
	pid_t	pid;

	pipe(fd);
	pipe(fd2);
	for (int i = 0; i < 3; i++)
	{
		pid = fork();
		if (pid < 0)
			exit(1);
		else if (pid == 0)
		{
			if (i == 0)
			{
				close(fd[0]);
				dup2(fd[1], STDOUT_FILENO);
				exec1();
				exit(1);
			}
			else if (i == 1)
			{
				close(fd[1]);
				dup2(fd[0], STDIN_FILENO);

				close(fd2[0]);
				dup2(fd2[1], STDOUT_FILENO);
				exec2();
				exit(1);
			}
			else if (i == 2)
			{
				close(fd2[1]);
				dup2(fd2[0], STDIN_FILENO);
				exec3();
				exit(1);
			}
		}
		else
		{
			close(fd[1]);
			close(fd2[1]);
			wait(NULL);
			close(fd2[1]);
			dup2(fd2[0], STDIN_FILENO);
		}
	}
	return (0);
}

// #include <ntsid.h>
// #include <unistd.h>
// #include <printf.h>
// #include <stdlib.h>

// int main(int argc, char *argv[]) {
//     pid_t pid;
//     int count = 0;
//     //获得当前进程ID
//     printf("Current Process Id = %d \n", getpid());

//     if ((pid = fork()) < 0) {
//         printf("异常退出");
//         exit(1);
//     } else if (pid == 0) {
//         count++;
//         printf("进入子进程, 当前进程 currentPid = %d, 父进程 parentPid = %d \n", getpid(),getppid());
//     } else {
//         count++;
//         printf("当前进程   当前进程 currentPid = %d, 子进程 childPid = %d \n", getpid(), pid);
//     }
//     printf("当前进程 currentPid = %d, Count = %d \n", getpid(), count);
//     return 0;
// }
 
// int main()
// {
//     pid_t pid;
//     char buffer[32];
// 	int pipe_default[2];
 
//     memset(buffer, 0, 32);
//     if(pipe(pipe_default) < 0)
//     {
//         printf("Failed to create pipe!\n");
//         return 0;
//     }
 
//     if(0 == (pid = fork()))
//     {
// 		printf("Child :\n");
// 		close(1);
// 		if(-1 != write(0, buffer, sizeof(buffer)))
//         {
//             printf("Send data to parent: hello!\n");
//         }
//         close(0);
//     }
//     else
//     {
// 		wait(NULL);
// 		printf("Parent :\n");
//         close(0);
//         if(read(1, buffer, 32) > 0)
//         {
//             printf("Receive data from child: %s!\n", buffer);
//         }
//         close(1);
//     }
 
//     return 1;
// }