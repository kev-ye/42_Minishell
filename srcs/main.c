/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kaye <kaye@student.42.fr>                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/05/19 14:06:33 by besellem          #+#    #+#             */
/*   Updated: 2021/05/24 16:00:44 by kaye             ###   ########.fr       */
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

t_list	*get_env(char **env)
{
	t_list	*new_env;
	t_list	*tmp;
	void	*ptr;
	int		i;

	new_env = NULL;
	i = 0;
	while (env[i])
	{
		ptr = ft_strdup(env[i++]);
		if (!ptr)
		{
			ft_lstclear(&new_env, free);
			return (ft_memdel((void **)&ptr));
		}
		tmp = ft_lstnew(ptr);
		if (!tmp)
		{
			ft_lstclear(&new_env, free);
			return (ft_memdel((void **)&tmp));
		}
		ft_lstadd_back(&new_env, tmp);
	}
	return (new_env);
}

int	main(__attribute__((unused)) int ac,
		__attribute__((unused)) const char **av,
		__attribute__((unused)) char **env)
{
	singleton()->env = get_env(env);
	prompt();
	// if (ac == 3)
	// {
	// 	__attribute__((unused)) char *new = ft_strdup(av[1]);

	// 	ft_printf("str: [%s], charset: [%s]\n", av[1], av[2]);
	// 	ft_strnclean(new + 10, av[2], 100);
	// 	ft_printf("str: [%s]\n", new);
	// 	// ft_printf("str: [%s]\n", ft_strclean(new, av[2]));
	// 	ft_memdel((void **)&new);
	// }
	return (0);
}
