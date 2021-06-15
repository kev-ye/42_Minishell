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

char *cmd[2] = {"ls", "."};

int main()
{
    const pid_t	pid = fork();
    char *test = "lol";

	int fd2;

	if (pid < 0)
		exit(1);
	else if (0 == pid)
	{
        fd2 = open("file2" , O_WRONLY | O_TRUNC |O_CREAT, 0666);
		if (fd2 == -1)
		{
			printf("write fd2 down\n");
			exit(0);
		}
		close(fd2);

		fd2 = open("file3" , O_WRONLY | O_TRUNC | O_CREAT, 0666);
		if (fd2 == -1)
		{
			printf("write fd3 down\n");
			exit(0);
		}
		// close(fd2);

		dup2(fd2, STDOUT_FILENO);
		execvp(cmd[0], cmd);
		close(fd2);

		exit(0);
	}
	else
	{
		wait(NULL);
	}
}