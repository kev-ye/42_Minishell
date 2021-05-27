/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kaye <kaye@student.42.fr>                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/05/19 14:06:33 by besellem          #+#    #+#             */
/*   Updated: 2021/05/26 22:30:12 by kaye             ###   ########.fr       */
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
	basename = ft_strrchr(singleton()->cwd, '/');
	if (!*(basename + 1))
		ft_dprintf(STDERR_FILENO, PROMPT, "/");
	else
		ft_dprintf(STDERR_FILENO, PROMPT, basename + 1);
}

void	prompt(void)
{
	char	*ret;
	int		r;

	while (1)
	{
		print_prompt();
		// r = get_next_line(STDIN_FILENO, &ret);
		r = ft_get_next_line(&ret);
		ft_parse(ret);
		ft_exec_each_cmd();
		ft_memdel((void **)(&ret));
		if (r <= 0)
		{
			ft_free_exit();
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

int	main(__attribute__((unused)) int ac,
		__attribute__((unused)) const char **av,
		__attribute__((unused)) char **env)
{
	singleton()->env = get_env(env);
	prompt();
	return (0);
}
