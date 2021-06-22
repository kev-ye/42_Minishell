/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   exec_all_in_one.c                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kaye <kaye@student.42.fr>                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/06/16 19:03:54 by kaye              #+#    #+#             */
/*   Updated: 2021/06/22 16:33:03 by kaye             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

static void	cmd_with_multi_flag(t_list *lst_cmd, int *fd)
{
	t_list	*tmp;
	int		fd_index;

	tmp = lst_cmd;
	fd_index = 0;
	while (tmp && (is_redir(tmp) || flag_check(tmp) == FLG_PIPE))
	{
		if (!check_is_inter(tmp))
			break ;
		interm_cmd(tmp->content, fd, fd_index, tmp);
		while (tmp && is_redir(tmp))
			tmp = tmp->next;
		++fd_index;
		tmp = tmp->next;
	}
	last_cmd(tmp->content, fd, fd_index, tmp);
}

static void	exec_multi_flag_cmd(t_list *tmp, int pipe_len, int *fd)
{
	if (pipe_len > 0)
	{
		cmd_with_multi_flag(tmp->next, fd);
		if (fd)
			free(fd);
	}
}

void	all_in_one(t_list *lst_cmd)
{
	t_list	*tmp;
	int		*fd;
	int		pipe_len;

	tmp = lst_cmd;
	fd = NULL;
	if (tmp && (flag_check(tmp) == FLG_EO_CMD || flag_check(tmp) == FLG_EOL))
	{
		simple_cmd(tmp->content);
		return ;
	}
	pipe_len = count_pipe(tmp);
	if (pipe_len > 0)
	{
		fd = malloc(sizeof(int) * (pipe_len * 2));
		if (!fd)
			return ;
		pipe(fd);
	}
	create_fd_output(lst_cmd);
	first_cmd(tmp->content, fd, tmp, pipe_len);
	while (tmp && is_redir(tmp))
		tmp = tmp->next;
	exec_multi_flag_cmd(tmp, pipe_len, fd);
}

void	exec_all_in_one(t_list *lst_cmd)
{
	if (!lst_cmd)
		return ;
	if (!((t_cmd *)lst_cmd->content)->args && check_is_redir_cmd(lst_cmd))
		lst_cmd->content = get_complete_cmd(lst_cmd->content, lst_cmd);
	if (!((t_cmd *)lst_cmd->content)->args)
		return ;
	all_in_one(lst_cmd);
}
