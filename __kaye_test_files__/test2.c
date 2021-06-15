#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "libft.h"

char *cmds1[3] = {"/bin/ls",".", NULL};
char *cmds2[3] = {"/bin/cat", NULL};
char *cmds3[3] = {"/usr/bin/wc", NULL};
char *cmds4[3] = {"/usr/bin/grep", "1", NULL};
char *cmd[2] = {"grep", "incs"};

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

void	exec4()
{
	// execve(cmds4[0], cmds4, NULL);
	execvp(cmd[0], cmd);
}

// void	command_pre_pipe(int fd_in, int fd_out)
int	*command_pre_pipe(void)
{
	pid_t pid;
	int *fd = malloc(sizeof(int) * 2);
	if (!fd)
		return (NULL);

	pipe(fd);
	pid = fork();
	if (pid < 0)
			exit(1);
	else if (pid == 0)
	{
		close(fd[0]);
		dup2(fd[1], STDOUT_FILENO);
		exec1();
	}
	else
	{
		close(fd[1]);
		wait(NULL);
	}
	return (fd);
}

int	*command_pre_pipe2(int *get_fd)
{
	pid_t pid;
	int *tmp = get_fd;
	int *fd = malloc(sizeof(int) * 2);
	if (!fd)
		return (NULL);

	pipe(fd);
	pid = fork();
	if (pid < 0)
			exit(1);
	else if (pid == 0)
	{
		close(get_fd[1]);
		dup2(get_fd[0], STDIN_FILENO);

		close(fd[0]);
		dup2(fd[1], STDOUT_FILENO);
		exec2();
	}
	else
	{
		close(fd[1]);
		wait(NULL);
	}
	free(tmp);
	return (fd);
}

int	*command_pre_pipe3(int *get_fd)
{
	pid_t pid;
	int *tmp = get_fd;
	int *fd = malloc(sizeof(int) * 2);
	if (!fd)
		return (NULL);

	pipe(fd);
	pid = fork();
	if (pid < 0)
			exit(1);
	else if (pid == 0)
	{
		close(get_fd[1]);
		dup2(get_fd[0], STDIN_FILENO);
	
		close(fd[0]);
		dup2(fd[1], STDOUT_FILENO);
		exec4();
	}
	else
	{
		close(fd[1]);
		wait(NULL);
	}
	free(tmp);
	return (fd);
}

void	command_need_stdin(int *fd)
{
	pid_t pid;

	pid = fork();
	if (pid < 0)
			exit(1);
	else if (pid == 0)
	{
		close(fd[1]);
		dup2(fd[0], STDIN_FILENO);
		exec3();
	}
	else
	{
		close(fd[1]);
		wait(NULL);
	}
}

int main()
{
	// int fd[2];
	int *fd;
	pid_t	pid;

	// pipe(fd);
	int i;
	// for (int i = 0; i < 2; i++)
	// {
		// pid = fork();
		// if (pid < 0)
		// 	exit(1);
		// else if (pid == 0)
		// {
	i = 0;
			if (i == 0)
			{
				fd = command_pre_pipe();
				// exec1();
			}
	i = 1;
			if (i == 1)
			{
				fd = command_pre_pipe2(fd);
				// exec1();
			}
	i = 2;
			if (i == 2)
			{
				fd = command_pre_pipe3(fd);
				// exec3();
			}
	i = 3;
			if (i == 3)
			{
				// command_need_stdin(fd[0], fd[1]);
				command_need_stdin(fd);
				// exec1();
			}
		// }
		// else
		// {
		// 	close(fd[1]);
		// 	wait(NULL);
		// }
	// }
	return (0);
}