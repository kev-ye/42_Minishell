/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: besellem <besellem@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/05/19 14:06:33 by besellem          #+#    #+#             */
/*   Updated: 2021/05/31 17:51:23 by besellem         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

static char *ft_join(char *s1, char *s2)
{
	size_t len;
	char *s;
	int i;

	if (!s1 && !s2)
		return (NULL);
	len = ((s1)? ft_strlen(s1) : 0) + ((s2)? ft_strlen(s2) : 0);
	s = malloc(sizeof(char) * (len + 1));
	if (!s)
		return (NULL);
	i = 0;
	while (s1 && s1[i])
		*s++ = s1[i++];
	i = 0;
	while (s2 && s2[i])
		*s++ = s2[i++];
	*s = '\0';
	if (s1)
		free(s1);
	return (s - len);
}

static int ft_newline(char *s)
{
	int i = 0;

	if (!s)
		return (0);
	while (s[i])
	{
		if (s[i] == '\n')
			return (1);
		i++;
	}
	return (0);
}

static char *ft_getline(char *s)
{
	char *str;
	int i;

	if (!s)
		return (NULL);
	i = 0;
	while (s[i] && s[i] != '\n')
		i++;
	str = malloc(sizeof(char) * i + 1);
	if (!str)
		return (NULL);
	i = 0;
	while (s[i] && s[i] != '\n')
	{
		str[i] = s[i];
		i++;
	}
	str[i] = '\0';
	return (str);
}

static char *ft_nextline(char *s)
{
	char *str;
	int i = 0;
	int j = 0;

	if (!s)
		return (NULL);
	while (s[i] && s[i] != '\n')
		i++;
	if (!s[i])
	{
		free(s);
		return (NULL);
	}
	str = malloc(sizeof(char) * (ft_strlen(s) - i + 1));
	if (!str)
		return (NULL);
	i++;
	while (s[i])
	{
		str[j] = s[i];
		j++;
		i++;
	}
	str[j] = '\0';
	free(s);
	return (str);
}

int				ft_get_next_line(char **line)
{
	static char *s;
	int ret = 1;
	char buff[1 + 1];

	if (!line)
		return (-1);
	while (!ft_newline(s) && ret != 0)
	{
		ret = read(STDIN_FILENO, buff, 1);
		if (ret < 0)
			return (-1);
		buff[ret] = '\0';
		s = ft_join(s, buff);
	}
	*line = ft_getline(s);
	s = ft_nextline(s);
	return ((ret) ? 1 : 0);
}

///////////////////////////////////////////////////

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

void	prompt(void)
{
	char	*ret;
	int		r;

	signal(SIGINT, ft_interrupt);
	while (1)
	{
		print_prompt();
		r = get_next_line(STDIN_FILENO, &ret);
		// r = ft_get_next_line(&ret);
		ft_parse(ret);
		// ft_exec_each_cmd();
		ft_exec_each_cmd(singleton()->lst);
		ft_memdel((void **)(&ret));
		if (r <= 0)
		{
			ft_exit();
			break ;
		}
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

// Key codes
#define K_UP "\x1b\x5b\x41"
#define K_DOWN "\x1b\x5b\x42"
#define K_RIGHT "\x1b\x5b\x43"
#define K_LEFT "\x1b\x5b\x44"

void	ft_termcap_history(char *termcap)
{
	if (0 == ft_strcmp(termcap, K_UP))
	{
		ft_printf("UP\n");
	}
	else if (0 == ft_strcmp(termcap, K_DOWN))
	{
		ft_printf("DOWN\n");
	}
}

void	ft_termcap_edition(char *termcap)
{
	if (0 == ft_strcmp(termcap, K_RIGHT))
	{
		ft_printf("RIGHT\n");
	}
	else if (0 == ft_strcmp(termcap, K_LEFT))
	{
		ft_printf("LEFT\n");
	}
}

int	check_arrow(char buf[])
{
	struct s_termcaps	termcaps[] = {
		{K_UP, ft_termcap_history}, {K_DOWN, ft_termcap_history},
		{K_RIGHT, ft_termcap_edition}, {K_LEFT, ft_termcap_edition}, {NULL, 0}
	};
	int			i;

	i = 0;
	while (termcaps[i].termcap)
	{
		if (!strncmp(buf, termcaps[i].termcap, ft_strlen(termcaps[i].termcap)))
			termcaps[i].f(termcaps[i].termcap);
		++i;
	}
	return (1);
}

int	main(__attribute__((unused)) int ac,
		__attribute__((unused)) const char **av,
		__attribute__((unused)) char **env)
{
	if (!ft_init_minishell(env))
		return (EXIT_FAILURE);
	// prompt();
	
	char			buf[BUF_SIZE + 1] = {0};
	struct termios	tattr;

	tcgetattr(STDIN_FILENO, &tattr);
	tattr.c_lflag &= ~(ICANON|ECHO); /* Clear ICANON and ECHO. */
	tattr.c_cc[VMIN] = 1;
	tattr.c_cc[VTIME] = 0;
	tcsetattr(STDIN_FILENO, TCSAFLUSH, &tattr);

	signal(SIGINT, SIG_IGN);
	int i = 0;
	while (1)
	{
		int		ret = read(STDIN_FILENO, buf + i, 1);
		
		if (buf[0] == 0x1b)
		{
			if (3 == ++i && check_arrow(buf))
			{
				ft_bzero(buf, BUF_SIZE + 1);
				i = 0;
			}
			continue ;
		}
		
		ft_printf("ret[%d], buf[", ret);
		int _i = 0;
		while (buf[_i])
			ft_dprintf(STDERR_FILENO, "%x", buf[_i++]);
		ft_putendl_fd("]", STDERR_FILENO);

		if (buf[0] == 0x4)
		{
			ft_putendl_fd("CTRL-D", STDERR_FILENO);
			break ;
		}
	}
	return (EXIT_SUCCESS);
}
