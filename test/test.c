/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   test.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kaye <kaye@student.42.fr>                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/06/16 16:34:59 by kaye              #+#    #+#             */
/*   Updated: 2021/06/18 18:00:01 by kaye             ###   ########.fr       */
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
// # include "libft.h"
# include "readline/readline.h"

int main()
{
    const char *test = "test";
    int fd1 = open(test, O_RDWR | O_CREAT, 0666);
    if (fd1 == -1)
    {
        printf("first\n");
        exit(-1);
    }
    write(fd1, "test1", 5);
    int i = unlink(test);
    printf("unlink : %d\n", i);

    // close(fd1);
    // int fd2 = open(test, O_RDWR);
    // if (fd2 == -1)
    // {
    //     printf("second\n");
    //     exit(-1);
    // }
    write(fd1, "test2", 5);
    system("ls");
    close(fd1);
}