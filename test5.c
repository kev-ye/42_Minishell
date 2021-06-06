# include <stdio.h>
# include <stdint.h>
# include <stdlib.h>
# include <unistd.h>
# include <fcntl.h>
# include <signal.h>
# include <dirent.h>
# include <termios.h>
# include <curses.h>
# include <term.h>
# include <sys/wait.h>
# include <sys/stat.h>
# include <sys/errno.h>
# include <sys/ioctl.h>
# include <libc.h>

char *_echo[3] = {"echo", "test", NULL};
char *_cat[2] = {"cat", NULL};
char *_wc[2] = {"wc", NULL};

int	test_func_output(int fd1)
{
		dup2(fd1, STDOUT_FILENO);
		return (fd1);
}

int	test_func_input(int *fd)
{
		close(fd[1]);
		fd[0] = open("f2" , O_RDWR);
		if (fd[1] == -1)
		{
			printf("f2 down\n");
		}
		dup2(fd[0], STDIN_FILENO);
		return (fd[0]);
}

int main()
{
    const pid_t	pid = fork();
	int fd[2];
	
	pipe(fd);		
	if (pid < 0)
		exit(1);
	else if (0 == pid)
	{
		fd[0] = test_func_input(fd);
	
		execvp(_cat[0], _cat);
		close(fd[0]);

		exit(0);
	}
	else
	{
		close(fd[0]);
		wait(NULL);
	}

	const pid_t	pid2 = fork();
	int fd2[2];
	
	pipe(fd2);
	if (pid2 < 0)
		exit(1);
	else if (0 == pid2)
	{
		close(fd[0]);
		dup2(fd[1], STDIN_FILENO);
		execvp(_wc[0], _wc);
	}
	else
	{
		close(fd[1]);
		wait(NULL);	
	}
	
}