/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: besellem <besellem@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/05/19 14:06:33 by besellem          #+#    #+#             */
/*   Updated: 2021/05/23 12:07:36 by besellem         ###   ########.fr       */
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

int	builtin_cd(char *dir)
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
		return (0);
	}
	return (1);
}

int	builtin_exit(int code) __attribute__((noreturn));

int	builtin_exit(int code)
{
	exit(code);
}

void	print_prompt(void)
{
	const char	*pwd = getcwd(NULL, 0);

	if (pwd)
	{
		ft_dprintf(STDIN_FILENO, PROMPT, ft_strrchr(pwd, '/') + 1);
		free((char *)pwd);
	}
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
		free(ret);
		if (r <= 0)
			break ;
	}
}

///////////////////////////////////////////////////////////////
t_cmd	*init_cmd(void)   /////////////////////////////////kaye
{
	t_cmd *cmd;

	cmd = calloc(1, sizeof(t_cmd));
	if (!cmd)
		return (NULL);
	cmd->ac = -1;
	cmd->av = NULL;
	return (cmd);
}
//////////////////////////////////////////////////////////////

int	main(__attribute__((unused))int ac,
		__attribute__((unused))const char **av,
		__attribute__((unused))char **env)
{
	// ft_printstrs(STDOUT_FILENO, env);
	singleton()->env = env;

	// char *ex = search_executable("ls");
	// if (ex)
	// {
	// 	ft_printf("`ls' command:\n");
	// 	ft_exec_cmd(ex);
	// 	free(ex);
	// }
	// ft_printf("pwd=[%s]\n", getenv("PWD"));
	prompt();
	return (0);
}
