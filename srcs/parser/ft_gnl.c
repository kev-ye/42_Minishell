/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_gnl.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: besellem <besellem@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/06/02 12:55:07 by besellem          #+#    #+#             */
/*   Updated: 2021/06/02 14:46:33 by besellem         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

// termcaps lookup table declaration
const struct s_termcaps	g_terms[] = {
	{K_UP, ft_termcap_history},
	{K_DOWN, ft_termcap_history},
	{K_RIGHT, ft_termcap_edition},
	{K_LEFT, ft_termcap_edition},
	{NULL, 0}
};

#ifndef __TMP_BUF_SZ
# define __TMP_BUF_SZ 1
#endif	/* ifndef __TMP_BUF_SZ */

static char	*ft_mcat(char *s1, char *s2)
{
	char	*new;
	int		i;
	int		j;

	new = (char *)malloc(ft_strlen(s1) + ft_strlen(s2) + 1);
	if (!new)
		return (NULL);
	i = -1;
	while (s1[++i])
		new[i] = s1[i];
	j = -1;
	while (s2[++j])
		new[i + j] = s2[j];
	new[i + j] = '\0';
	return (new);
}

static char	*ft_realloc_str(char *str, char **line, int *check)
{
	char			*new;
	unsigned int	i;

	new = NULL;
	if (str)
	{
		i = 0;
		while (str[i] && str[i] != '\n')
			++i;
		if (str[i] == '\n')
		{
			*check = 1;
			new = ft_strdup(str + i + 1);
		}
		else if (str[i])
			new = ft_strdup(str + i);
		*line = ft_substr(str, 0, i);
		free(str);
	}
	return (new);
}
//static 
int	is_arrow_pressed(char *s)
{
	int	i;

	i = 0;
	while (g_terms[i].termcap)
	{
		// if (ft_strstr(s, g_terms[i].termcap))
		if (!strncmp(s, g_terms[i].termcap, ft_strlen(g_terms[i].termcap)))
			g_terms[i].f(g_terms[i].termcap);
		++i;
	}
	return (1);
}

static char	*ft_read_line(int fd, char *str, char **line, int *check)
{
	char	buffer[__TMP_BUF_SZ + 1];
	char	term_buf[_TERMCAPS_ARROW_LEN + 1] = {0};
	char	*tmp;
	int		r;

	int		i = 0;
	
	r = 1;
	while (r > 0)
	{
		r = read(fd, buffer, __TMP_BUF_SZ);
		if (r <= 0)
			break ;
		buffer[r] = '\0';
		term_buf[i] = buffer[0];
		if (0x1b == term_buf[0])
		{
			if (3 == ++i)
			{
				is_arrow_pressed(term_buf);
				ft_bzero(term_buf, __TMP_BUF_SZ + 1);
				i = 0;
			}
			continue ;
		}
		ft_putchar_fd(buffer[0], STDIN_FILENO);

		tmp = str;
		str = ft_mcat(str, buffer);
		ft_memdel((void **)&tmp);
		if (ft_strchr(str, '\n'))
			break ;
	}
	return (ft_realloc_str(str, line, check));
}

int	ft_gnl_stdin(char **line)
{
	static char	*str;
	int			check;

	if (read(STDIN_FILENO, str, 0) || __TMP_BUF_SZ <= 0 || !line)
		return (-1);
	check = 0;
	if (str && ft_strchr(str, '\n'))
	{
		str = ft_realloc_str(str, line, &check);
		return (check);
	}
	if (!str)
	{
		str = ft_strdup("");
		if (!str)
			return (-1);
	}
	str = ft_read_line(STDIN_FILENO, str, line, &check);
	return (check);
}

#ifdef __TMP_BUF_SZ
# undef __TMP_BUF_SZ
#endif	/* ifdef __TMP_BUF_SZ */
