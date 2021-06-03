/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: besellem <besellem@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/05/19 14:06:33 by besellem          #+#    #+#             */
/*   Updated: 2021/06/03 11:17:31 by besellem         ###   ########.fr       */
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
	char	*basename;

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
	if (singleton()->isatty_stdin)
	{
		basename = ft_strrchr(singleton()->cwd, '/');
		if (!*(basename + 1))
			ft_dprintf(STDERR_FILENO, PROMPT, "/");
		else
			ft_dprintf(STDERR_FILENO, PROMPT, basename + 1);
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

static void	init_termcaps(struct termios *tattr)
{
	// const char	*term_name = ft_getenv("TERM");
	// int			ret;

	// if (!term_name)
	// {
	// 	ft_dprintf(STDERR_FILENO,
	// 		B_RED "$TERM not set. Termcaps won't work.\n" CLR_COLOR);
	// 	ft_free_exit();
	// }
	// ret = tgetent(NULL, term_name);
	// ft_memdel((void **)&term_name);
	// if (ret <= 0)
	// {
	// 	ft_dprintf(STDERR_FILENO,
	// 		B_RED "Could not access the termcap data base.\n" CLR_COLOR);
	// 	ft_free_exit();
	// }
	tcgetattr(STDIN_FILENO, tattr);
	tattr->c_lflag &= ~(ICANON | ECHO);
	tattr->c_cc[VMIN] = 1;
	tattr->c_cc[VTIME] = 0;
	tcsetattr(STDIN_FILENO, TCSAFLUSH, tattr);
	// tcsetattr(STDIN_FILENO, TCSANOW, tattr);
}

static int	ft_init_minishell(char **env)
{
	const char	*args[] = {"export", NULL, NULL};
	char		*shlvl;
	char		*ret;

	if (singleton() == NULL)
		return ((int)ft_malloc_error(__FILE__, __LINE__));
	singleton()->env = get_env(env);
	shlvl = ft_getenv("SHLVL");
	if (shlvl)
		ft_asprintf(&ret, "SHLVL=%d", ft_atoi(shlvl) + 1);
	else
		ret = ft_strdup("SHLVL=1");
	if (!ret && ft_memdel((void **)&shlvl) == NULL)
		return ((int)ft_malloc_error(__FILE__, __LINE__));
	args[1] = ret;
	ft_export((char **)args);
	ft_memdel((void **)&ret);
	ft_memdel((void **)&shlvl);
	singleton()->isatty_stdin = isatty(STDIN_FILENO);
	init_termcaps(&singleton()->tattr);
	return (1);
}

void	prompt(void)
{
	char	*ret;
	int		r;

	signal(SIGINT, ft_interrupt);
	while (1)
	{
		// char *s = tgetstr("al", NULL);
		// tputs(s, 1, ft_sputchar);
		print_prompt();
		r = ft_gnl_stdin(&ret);
		if (singleton()->isatty_stdin)
			add2history(ret);
		// tputs(ret, 1, ft_sputchar);
		ft_parse(ret);
		ft_exec_each_cmd(singleton()->lst);
		// ft_memdel((void **)(&ret));
		if (r <= 0)
		{
			ft_exit();
			break ;
		}
	}
}

int	main(__attribute__((unused)) int ac,
		__attribute__((unused)) const char **av,
		__attribute__((unused)) char **env)
{
	if (!ft_init_minishell(env))
		return (EXIT_FAILURE);
	if (singleton()->isatty_stdin)
		init_history();
	prompt();
	return (EXIT_SUCCESS);
}
