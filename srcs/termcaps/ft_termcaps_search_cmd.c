/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_termcaps_search_cmd.c                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: besellem <besellem@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/06/07 17:44:53 by besellem          #+#    #+#             */
/*   Updated: 2021/06/07 18:13:50 by besellem         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

static size_t	get_max_len(void)
{
	DIR				*directory;
	struct dirent	*_dirent;
	size_t			len;

	directory = opendir(singleton()->cwd);
	if (!directory)
		return (0);
	len = 0;
	while (TRUE)
	{
		_dirent = readdir(directory);
		if (!_dirent)
			break ;
		if (len < _dirent->d_namlen)
			len = _dirent->d_namlen;
	}
	closedir(directory);
	return (len);
}

static char	*is_cmd_found(char *search)
{
	DIR				*directory;
	struct dirent	*_dirent;
	size_t			found;
	char			*cmd;

	if (!search)
		return (NULL);
	directory = opendir(singleton()->cwd);
	cmd = NULL;
	found = SUCCESS;
	while (TRUE)
	{
		_dirent = readdir(directory);
		if (!_dirent)
			break ;
		if (0 == ft_strncmp(search, _dirent->d_name, ft_strlen(search)))
		{
			if (ERROR == found)
				return (ft_memdel((void **)&cmd));
			cmd = ft_strdup(_dirent->d_name);
			found = ERROR;
		}
	}
	closedir(directory);
	return (cmd);
}

static int	can_replace_cmd(char **ptr, char *search)
{
	const void	*tmp_ptr = *ptr;
	const char	*cmd_found = is_cmd_found(search);

	if (cmd_found)
	{
		ft_asprintf(ptr, "%.*s%s", (search - *ptr), *ptr, cmd_found);
		singleton()->edit.len = ft_strlen(*ptr);
		singleton()->edit.current_index = singleton()->edit.len;
		ft_memdel((void **)&tmp_ptr);
		return (1);
	}
	else
		return (0);
}

static void	print_closest(char *search, double ratio, int term_cols)
{
	DIR				*directory;
	struct dirent	*d;
	size_t			i;

	directory = opendir(singleton()->cwd);
	if (!directory)
	{
		ft_putstr_fd(tgetstr("bl", NULL), STDIN_FILENO);
		return ;
	}
	ft_putstr_fd("\n", STDIN_FILENO);
	while (TRUE)
	{
		i = 0;
		while (i < (size_t)ratio)
		{
			d = readdir(directory);
			if (!d)
				break ;
			if (ft_strcmp(d->d_name, ".") && ft_strcmp(d->d_name, "..")
				&& !ft_strncmp(search, d->d_name, ft_strlen(search)) && ++i)
				ft_printf("%-*s ", (int)(term_cols / ratio), d->d_name);
		}
		if (!d)
			break ;
		ft_putstr_fd("\n", STDIN_FILENO);
	}
	ft_putstr_fd("\n", STDIN_FILENO);
	closedir(directory);
}

void	ft_termcap_search_cmd(char **ptr)
{
	const int		term_cols = tgetnum("co");
	const size_t	len = get_max_len();
	const char		*go = tgetstr("ch", NULL);
	const double	ratio = ft_fmax(len, term_cols) / ft_fmin(len, term_cols);
	char			*search;

	if (BONUS)
	{
		search = ft_strrchr(*ptr, ' ');
		if (search)
			search++;
		if (!can_replace_cmd(ptr, search))
			print_closest(search, ratio, term_cols);
		ft_putstr_fd(CLR_LINE, STDIN_FILENO);
		print_prompt();
		ft_putstr_fd(*ptr, STDIN_FILENO);
		ft_putstr_fd(tgoto(go, 0, ft_strlen(singleton()->cwd_basename) + \
			PROMPT_CPADDING + singleton()->edit.current_index), STDIN_FILENO);
	}
}
