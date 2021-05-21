/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   sig.c                                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: besellem <besellem@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/02/24 00:14:07 by besellem          #+#    #+#             */
/*   Updated: 2021/05/20 18:00:42 by besellem         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <libc.h>
#include <errno.h>

// #define SIG_INTERRUPT 130

// void	ft_kill(int sig)
// {
// 	printf("KILL [%1$d] [%1$#x]\n", sig);
// 	exit(0);
// }

// void	ft_interrupt(int sig)
// {
// 	printf("INTERRUPT [%1$d] [%1$#x]\n", sig);
// 	exit(SIG_INTERRUPT);
// }

// int		main(int ac, char **av)
// {
// 	signal(SIGINT, ft_interrupt);
// 	signal(SIGKILL, ft_kill);
// 	while (1)
// 		;
// 	return (0);
// }

// 	CD
// #define CWD_BUFFER 1024

// int	main(int ac, char **av)
// {
// 	char buf[CWD_BUFFER];

// 	if (ac == 2)
// 	{
// 		getcwd(buf, CWD_BUFFER);
// 		printf("00 Current Directory => [%s]\n", buf);
// 		if (chdir(av[1]))
// 		{
// 			perror(av[1]);
// 			exit(errno);
// 		}
// 		else
// 		{
// 			printf("Directory changed to '%s'\n", av[1]);
// 		}
// 		getcwd(buf, CWD_BUFFER);
// 		printf("01 Current Directory => [%s]\n", buf);
// 	}
// 	else if (ac > 2)
// 		printf("%s: Arguments\n", av[0]);
// 	else
// 		printf("Usage: %s path\n", av[0]);
// 	return (0);
// }
