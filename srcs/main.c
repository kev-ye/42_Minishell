/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: besellem <besellem@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/05/19 14:06:33 by besellem          #+#    #+#             */
/*   Updated: 2021/05/19 23:56:58 by besellem         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

#define PROMPT F_RED "➜  "

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

void	prompt(void)
{
	char	*ret;
	int		r;

	while (1)
	{
		char	*pwd = getcwd(NULL, 0);
		ft_dprintf(STDIN_FILENO, PROMPT "%s" CLR_COLOR " ", ft_strrchr(pwd, '/') + 1);
		free(pwd);
		r = get_next_line(STDIN_FILENO, &ret);
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
