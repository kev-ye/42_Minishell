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

int main()
{
    const pid_t	pid = fork();
    char *test = "lol";

	if (pid < 0)
		exit(1);
	else if (0 == pid)
	{
        int fd = open("file" , O_WRONLY | O_CREAT, 0666);
		if (fd == -1)
		{
			printf("write fd down\n");
			exit(0);
		}
		write(fd, test, strlen(test));

        //////////////////////////////////////////////////////////////// last time here fo dup2 -> copy fd to fd2
        int fd2 = open("file2" , O_WRONLY | O_CREAT, 0666);
		if (fd2 == -1)
		{
			printf("write fd down\n");
			exit(0);
		}
		dup2(fd, fd2);

		exit(0);
	}
	else
	{
		wait(NULL);
	}
}