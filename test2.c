#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

void	exec1()
{

	char *argv1[] = {".", NULL};
	execve("/bin/ls", argv1, NULL);
}

void	exec2()
{
	execve("/usr/bin/wc", NULL, NULL);
}

int main()
{
	int fd[2];

	char *argv1[] = {".", NULL};

	pipe(fd);
	for (int i = 0; i < 2; i++)
	{
		if (fork() == 0)
		{
			// printf("child\n");
			close(1);
			dup2(fd[1], 1);
			close(fd[0]);
			if (i == 0)
				exec1();
			else
				exec2();
			// execve("/bin/ls", argv1, NULL);
			// execlp( "ls", "ls", "-1", NULL );
		}
		else
		{
			// printf("parent\n");
			close(0);
			dup2(fd[0], 0);
			close(fd[1]);
			// exec2();
			// execve("/usr/bin/wc", NULL, NULL);
			// execlp( "wc", "wc", "-l", NULL );
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