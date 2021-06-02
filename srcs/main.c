/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: besellem <besellem@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/05/19 14:06:33 by besellem          #+#    #+#             */
/*   Updated: 2021/06/02 18:19:24 by besellem         ###   ########.fr       */
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

// void	prompt(void)
// {
// 	char	*ret;
// 	int		r;

// 	signal(SIGINT, ft_interrupt);
// 	while (1)
// 	{
// 		print_prompt();
// 		r = get_next_line(STDIN_FILENO, &ret);
// 		// r = ft_get_next_line(&ret);
// 		ft_parse(ret);
// 		// ft_exec_each_cmd();
// 		ft_exec_each_cmd(singleton()->lst);
// 		ft_memdel((void **)(&ret));
// 		if (r <= 0)
// 		{
// 			ft_exit();
// 			break ;
// 		}
// 	}
// }

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

#define BUF_SIZE 4096


int	check_arrow(char buf[])
{
	struct s_termcaps	terms[] = {
		{K_UP, ft_termcap_history}, {K_DOWN, ft_termcap_history},
		{K_RIGHT, ft_termcap_edition}, {K_LEFT, ft_termcap_edition}, {NULL, 0}
	};
	int			i;

	i = 0;
	while (terms[i].termcap)
	{
		if (!strncmp(buf, terms[i].termcap, ft_strlen(terms[i].termcap)))
			terms[i].f(terms[i].termcap);
		++i;
	}
	return (1);
}

// special putchar
int	ft_sputchar_fd(int c, int fd)
{
	if (write(fd, &c, sizeof(int)) < 0)
		return (EOF);
	return (c);
}

int	ft_sputchar(int c)
{
	return (ft_sputchar_fd(c, STDIN_FILENO));
}

#define HISTORY_FILENAME ".minishell_history"

void	create_history(void)
{
	char	*path;
	char	*home;

	home = NULL;	// ft_getenv("HOME");
	if (!home || ft_is_openable(home, O_RDONLY))
	{
		if (home)
			free(home);
		home = getcwd(NULL, 0);
	}
	ft_asprintf(&path, "%s/" HISTORY_FILENAME, home);
	if (home)
		ft_memdel((void **)&home);
	if (!path)
		ft_malloc_error(__FILE__, __LINE__);
	singleton()->hist.path = path;
	singleton()->hist.fd = open(singleton()->hist.path,
		O_RDWR | O_CREAT | O_APPEND, 0644);
	if (singleton()->hist.fd == -1)
		ft_malloc_error(__FILE__, __LINE__);
	ft_memdel((void **)&path);
}

void	init_history(void)
{
	char	*ret;
	int		check;
	t_list	*new;

	while (1)
	{
		check = get_next_line(singleton()->hist.fd, &ret);
		if ((0 == ft_strlen(ret) || ft_strisall(ret, ft_isspace)))
		{
			ft_memdel((void **)&ret);
			if (check > 0)
				continue ;
			else
				break ;
		}
		new = ft_lstnew(ret);
		if (check < 0 || !new)
		{
			ft_lstclear(&singleton()->hist.history, free);
			ft_malloc_error(__FILE__, __LINE__);
		}

		/////////////////////////////////////////////
		// HERE LAST TIME - ben
		/////////////////////////////////////////////


		ft_lstadd_back(&singleton()->hist.history, new);
		if (0 == check)
			break ;
	}
	singleton()->hist.size = ft_lstsize(singleton()->hist.history);
	singleton()->hist.current = singleton()->hist.size;
}

void	add2history(char *cmd)
{
	t_list	*new;

	singleton()->hist.current = singleton()->hist.size;
	if (!cmd || ft_strisall(cmd, ft_isspace) || 0 == ft_strlen(cmd))
		return ;
	if (!ft_is_openable(singleton()->hist.path, O_RDONLY))
	{
		singleton()->hist.fd = open(singleton()->hist.path,
			O_RDWR | O_CREAT | O_APPEND, 0644);
		if (singleton()->hist.fd == -1)
			ft_malloc_error(__FILE__, __LINE__);
	}
	ft_putendl_fd(cmd, singleton()->hist.fd);
	new = ft_lstnew(cmd);
	if (!new)
		ft_malloc_error(__FILE__, __LINE__);
	ft_lstadd_back(&singleton()->hist.history, new);
	singleton()->hist.size++;
	singleton()->hist.current = singleton()->hist.size;
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
		add2history(ret);
		// tputs(ret, 1, ft_sputchar);
		// ft_sputchar_fd('\n', STDERR_FILENO);
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
	// prompt();
	
	__attribute__((unused)) int				i;
	__attribute__((unused)) char			buf[BUF_SIZE + 1] = {0};
	// __attribute__((unused)) struct termios	tattr;

	tcgetattr(STDIN_FILENO, &singleton()->tattr);
	singleton()->tattr.c_lflag &= ~(ICANON | ECHO); /* Clear ICANON and ECHO. */
	singleton()->tattr.c_cc[VMIN] = 1;
	singleton()->tattr.c_cc[VTIME] = 0;
	tcsetattr(STDIN_FILENO, TCSAFLUSH, &singleton()->tattr);

	create_history();
	init_history();

	// signal(SIGINT, SIG_IGN);
	// print_prompt();
	// prompt();

	prompt();
	

	// i = 0;
	// while (1)
	// {
	// 	ft_printf("ret: [%s]\n", ret);
	// 	int		ret = read(STDIN_FILENO, buf + i, 1);
		
	// 	(void)ret;
		
	// 	if (0x1b == buf[0])
	// 	{
	// 		if (3 == ++i)
	// 		{
	// 			check_arrow(buf);
	// 			ft_bzero(buf, BUF_SIZE + 1);
	// 			i = 0;
	// 		}
	// 		continue ;
	// 	}
	// 	if ('\n' == buf[0] || '\0' == buf[0])
	// 	{
	// 		signal(SIGINT, ft_interrupt);
	// 		print_prompt();
	// 		r = get_next_line(STDIN_FILENO, &ret);
	// 		// r = ft_get_next_line(&ret);
	// 		ft_printf("ret: [%s]\n", ret);
	// 		ft_parse(ret);
	// 		// ft_exec_each_cmd();
	// 		ft_exec_each_cmd(singleton()->lst);
	// 		ft_memdel((void **)(&ret));
	// 		if (r <= 0)
	// 			ft_exit();
	// 		// prompt();
	// 		continue ;
	// 	}
	// 	else
	// 		tputs(buf, 1, ft_sputchar);
	// 	ft_printf("ret[%d], buf[", ret);
	// 	int _i = 0;
	// 	while (buf[_i])
	// 		ft_dprintf(STDERR_FILENO, "%x", buf[_i++]);
	// 	ft_putendl_fd("]", STDERR_FILENO);

	// 	if (0x4 == buf[0])
	// 	{
	// 		ft_putendl_fd("CTRL-D", STDERR_FILENO);
	// 		break ;
	// 	}
	// }
	close(singleton()->hist.fd);
	ft_lstclear(&singleton()->hist.history, free);
	return (EXIT_SUCCESS);
}
