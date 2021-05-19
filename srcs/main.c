/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: besellem <besellem@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/05/19 14:06:33 by besellem          #+#    #+#             */
/*   Updated: 2021/05/20 00:19:47 by besellem         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

#define CWD_BUFFER 1024

void	*singleton(void)
{
	static void	*ptr;

	if (!ptr)
	{
		ptr = ft_calloc(1, sizeof(void *));
		if (!ptr)
		{
			ptr = NULL;
			return (NULL);
		}
	}
	return (ptr);
}

void	ft_cd(char *dir)
{
	char	buf[CWD_BUFFER];

	getcwd(buf, CWD_BUFFER);
	if (chdir(dir))
	{
		perror(dir);
		// exit(errno);
	}
	getcwd(buf, CWD_BUFFER);
}

void	prompt(void)
{
	char	*ret;
	int		r;

	while (1)
	{
		char	*pwd = getcwd(NULL, 0);
		ft_dprintf(STDIN_FILENO, B_RED "\e[1m" "%s $" CLR_COLOR " ", ft_strrchr(pwd, '/') + 1);
		free(pwd);
		r = get_next_line(STDIN_FILENO, &ret);
		
		if (ft_strncmp(ret, "cd ", 3) == 0)
			ft_cd(ret + 3);
		else if (ft_strncmp(ret, "exit", 4) == 0)
			exit(0);
		//
		//	PARSE HERE
		//
		printf("[%s]\n", ret);
		free(ret);
		if (r <= 0)
			break ;
	}
}

int	main(int ac, char **av, char **env)
{
	(void)ac;
	(void)av;
	(void)env;
	prompt();
	singleton();
	free(singleton());
	return (0);
}
