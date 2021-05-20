/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: besellem <besellem@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/05/19 14:06:33 by besellem          #+#    #+#             */
/*   Updated: 2021/05/20 11:55:17 by besellem         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

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

#define CWD_BUFFER 1024
void	ft_cd(char *dir)
{
	char	*pwd;

	if (getenv("PWD"))
	{
		if (chdir(dir))
			perror(dir);
		pwd = getcwd(NULL, 0);
		setenv("PWD", pwd, 1);
		printf("pwd => [%s], [%s]\n", pwd, getenv("PWD"));
		free(pwd);
	}
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
		
		//
		//	PARSE HERE
		//
		char **args_split = ft_split(ret, ' ');

		if (*args_split && ft_strslen(args_split) > 1 && ft_strcmp(*args_split, "cd") == 0)
			ft_cd(args_split[1]);
		else if (*args_split && ft_strcmp(*args_split, "exit") == 0)
			exit(0);

		size_t i = 0;
		while (args_split[i])
		{
			ft_printf("[%s]", args_split[i]);
			if (args_split[i + 1])
				ft_printf(" ");
			++i;
		}
		
		if (ft_strslen(args_split) > 0)
			ft_printf("\n");

		ft_strsfree(ft_strslen(args_split), args_split);
		free(ret);
		if (r <= 0)
			break ;
	}
}

void	ft_printstrs(int fd, char **strs)
{
	size_t	i;

	i = 0;
	while (strs[i])
		ft_dprintf(fd, "%s\n", strs[i++]);
	ft_dprintf(fd, "\n");
}

int	main(int ac, char **av, char **env)
{
	(void)ac;
	(void)av;
	(void)env;
	// ft_printstrs(STDOUT_FILENO, env);
	prompt();
	// singleton();
	// free(singleton());
	return (0);
}
