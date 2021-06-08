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

char *_wc[2] = {"cxzcxz", NULL};

int main()
{
    int status = 0;
    int exec_ret = 0;

    
    pid_t pid = fork();
    if (pid < 0)
        exit(1);
    else if (pid == 0)
    {
        exec_ret = execvp(_wc[0], _wc);
        if (exec_ret == -1)
        {
            printf("exec fail -> errno : %s\n", strerror(errno));
            exit(1);
        }
    }
    else
    {
        wait(&status);
        printf("status : %d\n", status);
    }
}