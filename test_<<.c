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
# include <readline/readline.h>
# include "libft.h"

char *_cat[2] = {"cat", NULL};

int main()
{
    char *str;
    char *d_input;
    int fd;


    str = NULL;
    d_input = NULL;
    while (1)
    {
        str = readline("Prompt -> ");
        add_history(str);

        if (!strcmp(str, "cat << lol"))
        {
            fd = open("/tmp/fuck", O_RDWR | O_CREAT | O_TRUNC |O_APPEND, 0666);
            if (fd < 0)
                exit(1);
            while (1)
            {
                d_input = readline("> ");
                if (!strcmp(d_input, "lol"))
                {
                    free(d_input);
                    break ;
                }
                write(fd, d_input, strlen(d_input));
                write(fd, "\n", 1);
            }
            close(fd);
            fd = open("/tmp/fuck", O_RDONLY);
            dup2(fd, STDIN_FILENO);
            execvp(_cat[0], _cat);
        }
        free(str);
    }
}