/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   test.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kaye <kaye@student.42.fr>                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/06/16 16:34:59 by kaye              #+#    #+#             */
/*   Updated: 2021/06/16 16:59:00 by kaye             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

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
# include "libft.h"

char *cmd[2] = {"ls", "."};

void	show_fd(int fd, char *msg)
{
	char *line = NULL;
	int r = 1;

	if (msg)
		printf("%s\n", msg);
	while (r)
	{
		r = get_next_line(fd, &line);
		printf("> %s\n", line);
		free(line);
		// break ;
	}
}


int main()
{
    const pid_t pid = fork();
    char *test = "lol";

        int fd2;

        int fd[2];
        pipe(fd);

        if (pid < 0)
                exit(1);
        else if (0 == pid)
        {
            // fd2 = open("./f1" , O_RDONLY);
            // if (fd2 == -1)
            // {
            //     printf("write fd2 down\n");
            //     exit(0);
            // }
            // dup2(fd2, STDIN_FILENO);
            dup2(fd[1], STDOUT_FILENO);
            execvp(cmd[0], cmd);
            exit(0);
        }
        else
        {
            waitpid(pid, NULL, 0);
            close(fd[1]);
            show_fd(fd[0], "parent");
        }
}