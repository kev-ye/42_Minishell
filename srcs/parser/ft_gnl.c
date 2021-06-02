/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_gnl.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: besellem <besellem@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/06/02 12:55:07 by besellem          #+#    #+#             */
/*   Updated: 2021/06/02 18:20:40 by besellem         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

// termcaps lookup table declaration
static struct s_termcaps	g_terms[] = {
	{K_UP, ft_termcap_history},
	{K_DOWN, ft_termcap_history},
	{K_RIGHT, ft_termcap_edition},
	{K_LEFT, ft_termcap_edition},
	{NULL, 0}
};

// READ 1 BYTE AT A TIME
#ifndef __TMP_BUF_SZ
# define __TMP_BUF_SZ _TERMCAPS_ARROW_LEN
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

static int	is_arrow_pressed(char **ptr, char *s)
{
	int	i;

	i = 0;
	while (g_terms[i].termcap)
	{
		if (!strncmp(s, g_terms[i].termcap, ft_strlen(g_terms[i].termcap)))
		{
			g_terms[i].f(ptr, g_terms[i].termcap);
			return (FOUND);
		}
		++i;
	}
	return (NOT_FOUND);
}

static int	ft_istermcap(char **ptr, char *read_buffer)
{
	char	buf[_TERMCAPS_ARROW_LEN + 1];

	ft_bzero(buf, _TERMCAPS_ARROW_LEN + 1);
	ft_memcpy(buf, read_buffer, ft_strlen(read_buffer));
	if (ft_strchr(buf, 0x7F))			// if (c == 127) (delete)
		ft_termcap_delete_char(ptr);
	else if (ft_strchr(buf, 0x4))		// if (c == 4) (CTRL-D)
		ft_exit();
	else if (ft_strchr(buf, 0xC))		// if (c == 12) (CTRL-L)
		ft_termcap_clear_screen(ptr);
	else if (ft_strchr(buf, 0x15))		// if (c == 21) (CTRL-U)
		ft_termcap_clear_line(ptr);
	else if (ft_strchr(buf, 0x1B))		// may be ESC or one of the arrow keys
	{
		if (is_arrow_pressed(ptr, buf) == NOT_FOUND)
			ft_termcap_esc(ptr);
	}
	else
		return (FALSE);
	return (TRUE);
}

static char	*ft_read_line(int fd, char *str, char **line, int *check)
{
	char	buffer[__TMP_BUF_SZ + 1];
	char	*tmp;
	int		r;
	
	r = 1;
	while (r > 0)
	{
		r = read(fd, buffer, __TMP_BUF_SZ);
		if (r <= 0)
			break ;
		buffer[r] = '\0';
		if (ft_istermcap(&str, buffer))
			continue ;
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
