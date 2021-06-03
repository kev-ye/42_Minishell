/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_gnl.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: besellem <besellem@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/06/02 12:55:07 by besellem          #+#    #+#             */
/*   Updated: 2021/06/03 18:27:10 by besellem         ###   ########.fr       */
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

static int	ft_istermcap(char **ptr, char *read_buffer, int len)
{
	char	buf[_TERMCAPS_ARROW_LEN + 1];

	ft_bzero(buf, _TERMCAPS_ARROW_LEN + 1);
	ft_memcpy(buf, read_buffer, len);
	if (ft_strchr(buf, K_DELETE))
		ft_termcap_delete_char(ptr);
	else if (ft_strchr(buf, K_CTRL_D))
		ft_exit();
	else if (ft_strchr(buf, K_CTRL_L))
		ft_termcap_clear_screen(ptr);
	else if (ft_strchr(buf, K_CTRL_U))
		ft_termcap_clear_line(ptr);
	else if (ft_strchr(buf, 0x1B))
	{
		if (is_arrow_pressed(ptr, buf) == NOT_FOUND)
			ft_termcap_esc(ptr);
	}
	else
		return (FALSE);
	return (TRUE);
}

void	print_inline(char **ptr, char *buffer)
{
	char		*ret;
	const void	*tmp_ptr = *ptr;
	const char	*head = ft_substr(*ptr, 0, singleton()->edit.current_index);
	const char	*tail = ft_substr(*ptr + singleton()->edit.current_index, 0,
					singleton()->edit.len - singleton()->edit.current_index);
	const char	*go = tgetstr("ch", NULL);

	singleton()->edit.len += ft_strlen(buffer);
	singleton()->edit.current_index += ft_strlen(buffer);
	ft_asprintf(&ret, "%s%s%s", head, buffer, tail);
	ft_putstr_fd(CLR_LINE, STDIN_FILENO);
	print_prompt();
	ft_dprintf(STDIN_FILENO, "%s", ret);
	*ptr = ret;
	tputs(tgoto(go, 0, ft_strlen(singleton()->cwd_basename) + PROMPT_CPADDING +\
		singleton()->edit.current_index), 1, ft_sputchar);
	(void)tmp_ptr;
	// ft_memdel((void **)&tmp_ptr);
	ft_memdel((void **)&head);
	ft_memdel((void **)&tail);
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
		tmp = str;
		if (ft_istermcap(&str, buffer, r))
			continue ;
		if (buffer[0] == '\n')
		{
			ft_putstr_fd(buffer, STDIN_FILENO);
			str = ft_strjoin(str, buffer);

			///////////////////////////////
			// SOME OPTI TO DO HERE
			///////////////////////////////


		}
		else
			print_inline(&str, buffer);
		ft_memdel((void **)&tmp);
		if (ft_strchr(str, '\n'))
			break ;
		
			///////////////////////////////
			// AND HERE
			///////////////////////////////
		
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
