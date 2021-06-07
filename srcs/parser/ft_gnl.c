/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_gnl.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: besellem <besellem@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/06/02 12:55:07 by besellem          #+#    #+#             */
/*   Updated: 2021/06/07 11:20:43 by besellem         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

// READ _TERMCAPS_ARROW_LEN BYTE AT A TIME
#ifndef _TMP_BUF_SZ_
# define _TMP_BUF_SZ_ _TERMCAPS_ARROW_LEN
#endif	/* ifndef _TMP_BUF_SZ_ */

// termcaps lookup table declaration
static struct s_termcaps	g_terms[] = {
	{K_UP, ft_termcap_history},
	{K_DOWN, ft_termcap_history},
	{K_RIGHT, ft_termcap_edition},
	{K_LEFT, ft_termcap_edition},
	{NULL, 0}
};

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
		if ('\n' == str[i])
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
		if (0 == strncmp(s, g_terms[i].termcap, ft_strlen(g_terms[i].termcap)))
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
	else if (ft_strchr(buf, '\t'))
		ft_termcap_search_cmd(ptr);
	else if (ft_strchr(buf, 0x1B))
	{
		if (is_arrow_pressed(ptr, buf) == NOT_FOUND)
			;//ft_termcap_esc(ptr); // other cases starting with `\e' but not
	}
	else
		return (FALSE);
	return (TRUE);
}

void	print_inline(char **ptr, char *buffer)
{
	char		*ret;
	const void	*tmp_ptr = *ptr;
	const char	*go = tgetstr("ch", NULL);

	ft_asprintf(&ret, "%.*s%s%.*s", (int)singleton()->edit.current_index,
		*ptr, buffer,
		(int)singleton()->edit.len - (int)singleton()->edit.current_index,
		*ptr + (int)singleton()->edit.current_index);
	singleton()->edit.len += ft_strlen(buffer);
	singleton()->edit.current_index += ft_strlen(buffer);

	// ft_putstr_fd(tgoto(go, 0, ft_strlen(singleton()->cwd_basename) + \
	// 	PROMPT_CPADDING + singleton()->edit.current_index), STDIN_FILENO);
	
	// ft_putstr_fd(tgetstr("kE", NULL)[0], STDIN_FILENO);
	// ft_putstr_fd("\e[0K\r", STDIN_FILENO);
	
	ft_putstr_fd(CLR_LINE, STDIN_FILENO);
	print_prompt();
	ft_putstr_fd(ret, STDIN_FILENO);
	*ptr = ret;
	ft_putstr_fd(tgoto(go, 0, ft_strlen(singleton()->cwd_basename) + \
		PROMPT_CPADDING + singleton()->edit.current_index), STDIN_FILENO);
	(void)tmp_ptr;
	// ft_memdel((void **)&tmp_ptr);
}

static char	*ft_read_line(int fd, char *str, char **line, int *check)
{
	char	buffer[_TMP_BUF_SZ_ + 1];
	char	*tmp;
	int		r;

	r = 1;
	while (r > 0)
	{
		r = read(fd, buffer, _TMP_BUF_SZ_);
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
			break ;
		}
		else
			print_inline(&str, buffer);
		ft_memdel((void **)&tmp);
	}
	return (ft_realloc_str(str, line, check));
}

int	ft_gnl_stdin(char **line)
{
	static char	*str;
	int			check;

	if (read(STDIN_FILENO, str, 0) || _TMP_BUF_SZ_ <= 0 || !line)
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

#ifdef _TMP_BUF_SZ_
# undef _TMP_BUF_SZ_
#endif	/* ifdef _TMP_BUF_SZ_ */
