/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kaye <kaye@student.42.fr>                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/05/19 14:06:33 by besellem          #+#    #+#             */
/*   Updated: 2021/06/11 17:57:15 by kaye             ###   ########.fr       */
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
		singleton()->cwd_basename = ft_strrchr(singleton()->cwd, '/');
		if (*(singleton()->cwd_basename + 1))
			singleton()->cwd_basename++;
		ft_asprintf(&singleton()->prompt, BPROMPT, singleton()->cwd_basename);
	}
	// if (singleton()->isatty_stdin)
	// 	ft_dprintf(STDERR_FILENO, PROMPT, singleton()->cwd_basename);
}


// a revoir : cant add env with export and unset fuck
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
	return (1);
}

// void	prompt(void)
// {
// 	char	*ret;
// 	int		r;

// 	signal(SIGINT, ft_interrupt);
// 	// signal(SIGQUIT, ft_interrupt);
// 	while (TRUE)
// 	{
// 		ft_bzero(&singleton()->edit, sizeof(t_edition));
// 		__reset_termcaps__(SET_TERMCAPS);
// 		print_prompt();
// 		if (singleton()->option.fd == STDIN_FILENO)
// 			r = ft_gnl(singleton()->option.fd, &ret);
// 		else
// 			r = get_next_line(singleton()->option.fd, &ret);
// 		__reset_termcaps__(RESET_TERMCAPS);
// 		if (singleton()->isatty_stdin)
// 			add2history(ft_strdup(ret));
// 		ft_parse(ret);
// 		ft_exec_each_cmd(singleton()->lst);
// 		ft_memdel((void **)(&ret));
// 		if (r <= 0)
// 		{
// 			ft_exit();
// 			break ;
// 		}
// 		// signal(SIGQUIT)
// 	}
// }


void	prompt(void)
{
	char	*ret;
	int		r;

	signal(SIGQUIT, ft_interrupt);
	signal(SIGINT, ft_interrupt);

	r = TRUE;
	while (TRUE)
	{
		ft_bzero(&singleton()->edit, sizeof(t_edition));
		print_prompt();
		if (singleton()->option.fd == STDIN_FILENO)
		{
			ret = readline(singleton()->prompt);
			if (!ret)
				ft_interrupt(3);
		}
		else
			r = get_next_line(singleton()->option.fd, &ret);
		if (!ret)
		{
			ft_exit_for_prompt();
			break ;
		}
		if (singleton()->isatty_stdin)
			add2history(ret);
		// rl_redisplay();
		ft_parse(ret);
		ft_exec_each_cmd(singleton()->lst);
		ft_memdel((void **)(&ret));
		if (r <= 0)
		{
			ft_exit_for_prompt();
			break ;
		}
	}
}

static void	ft_c_option(int ac, const char **av)
{
	if (ac >= 3)
	{
		ft_parse((char *)av[2]);
		ft_exec_each_cmd(singleton()->lst);
	}
}

static void	parse_args(int ac, const char **av)
{
	ft_bzero(&singleton()->option, sizeof(struct s_options));
	singleton()->option.fd = STDIN_FILENO;
	if (ac > 1)
	{
		if (0 == ft_strcmp((char *)av[1], "-c"))
		{
			singleton()->option.opt_c = TRUE;
			if (ac < 3)
			{
				ft_dprintf(2, PROG_NAME ": -c: option requires an argument\n");
				exit(EXIT_FAILURE);
			}
		}
		else
		{
			singleton()->isatty_stdin = 0;
			singleton()->option.fd = open(av[1], O_RDONLY);
			if (singleton()->option.fd == -1)
			{
				ft_dprintf(2, PROG_NAME ": %s: %s\n", av[1], strerror(errno));
				ft_free_exit();
				exit(EXIT_FAILURE);
			}
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
	parse_args(ac, av);
	if (singleton()->option.opt_c)
		ft_c_option(ac, av);
	else
		prompt();
	return (EXIT_SUCCESS);
}
