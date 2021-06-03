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

char *cmd1[3] = {"echo", "test", NULL};
char *cmd2[2] = {"cat", NULL};

int	test_func(void)
{
		int fd1;

		fd1 = -1;
		fd1 = open("f1", O_WRONLY | O_TRUNC | O_CREAT, 0666);
		if (fd1 == -1)
		{
			printf("f1 down\n");
			// exit(0);
		}
		return (fd1);
}

int	test_func2(void)
{
		int fd2;

		fd2 = open("f2" , O_RDWR);
		if (fd2 == -1)
		{
			printf("f2 down\n");
			// exit(0);
		}
		return (fd2);
}

int main()
{
    const pid_t	pid = fork();
    char *test = "lol";

	int fd1;
    int fd2;

	fd1 = -1;
	fd2 = -2;
	if (pid < 0)
		exit(1);
	else if (0 == pid)
	{
		// fd1 = open("f1", O_WRONLY | O_TRUNC | O_CREAT, 0666);
		// if (fd1 == -1)
		// {
		// 	printf("f1 down\n");
		// 	// exit(0);
		// }
		// dup2(fd1, STDOUT_FILENO);

		fd2 = test_func2();
		// fd2 = -1;
		dup2(fd2, STDIN_FILENO);

		fd1 = -1;
		dup2(fd1, STDOUT_FILENO);

		execvp(cmd2[0], cmd2);

		close(fd2);

        // fd1 = open("f1" , O_WRONLY | O_TRUNC |O_CREAT, 0666);
		// if (fd1 == -1)
		// {
		// 	printf("f1 down\n");
		// 	exit(0);
		// }
        // dup2(fd1, STDOUT_FILENO);
        // execvp(cmd2[0], cmd2);
		// close(fd);
        
		// fd = open("f2" , O_RDWR);
		// if (fd == -1)
		// {
		// 	printf("f2 down\n");
		// 	exit(0);
		// }
		// dup2(fd, STDIN_FILENO);
		// // execvp(cmd2[0], cmd2);
		// close(fd);

		exit(0);
	}
	else
	{
		wait(NULL);
	}
}