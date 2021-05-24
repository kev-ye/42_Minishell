/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kaye <kaye@student.42.fr>                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/05/19 14:06:33 by besellem          #+#    #+#             */
/*   Updated: 2021/05/24 15:07:13 by kaye             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

t_minishl	*singleton(void)
{
	static t_minishl	*ptr = NULL;

	if (!ptr)
	{
		ptr = ft_calloc(1, sizeof(t_minishl));
		if (!ptr)
		{
			ptr = NULL;
			return (NULL);
		}
	}
	return (ptr);
}

void	print_prompt(void)
{
	char	*pwd;

	pwd = getcwd(NULL, 0);
	if (pwd)
	{
		/*
		** avoid this problem:
		** mkdir test && cd test && rm -rf ../test
		*/
		ft_memdel((void **)(&singleton()->cwd));
		singleton()->cwd = pwd;
	}
	ft_dprintf(STDIN_FILENO, PROMPT, singleton()->cwd);
}

void	prompt(void)
{
	char	*ret;
	int		r;

	while (1)
	{
		print_prompt();
		r = get_next_line(STDIN_FILENO, &ret);
		ft_parse(ret);
		ft_exec_each_cmd();
		ft_memdel((void **)(&ret));
		if (r <= 0)
			break ;
	}
}

int	main(__attribute__((unused))int ac,
		__attribute__((unused))const char **av,
		__attribute__((unused))char **env)
{
	singleton()->env = env;
	prompt();
	return (0);
}
