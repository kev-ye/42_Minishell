/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   cmds_line_check.c                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kaye <kaye@student.42.fr>                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/06/07 12:45:35 by kaye              #+#    #+#             */
/*   Updated: 2021/06/07 12:48:15 by kaye             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

static int only_pipe(t_list *lst_cmd)
{
	t_list *tmp;
	int ret;

	tmp = lst_cmd;
	ret = -1;
	while (tmp)
	{
		if (((t_cmd *)tmp->content)->status_flag & FLG_PIPE)
		{
			ret = (int)((t_cmd *)tmp->content)->status_flag;
			tmp = tmp->next;
		}
		else
			break ;
	}
	if (ret != -1
		&& ((((t_cmd *)tmp->content)->status_flag & FLG_EOL)
		|| (((t_cmd *)tmp->content)->status_flag & FLG_EO_CMD)))
		return (ONLY_PIPE);
	return (-1);
}

static int only_redir(t_list *lst_cmd)
{
	t_list *tmp;
	int ret;

	tmp = lst_cmd;
	ret = -1;
	while (tmp)
	{
		if ((((t_cmd *)tmp->content)->status_flag & FLG_INPUT)
			|| (((t_cmd *)tmp->content)->status_flag & FLG_OUTPUT)
			|| (((t_cmd *)tmp->content)->status_flag & FLG_APPEND))
		{
			ret = (int)((t_cmd *)tmp->content)->status_flag;
			tmp = tmp->next;
		}
		else
			break ;
	}
	if (ret != -1
		&&((((t_cmd *)tmp->content)->status_flag & FLG_EOL)
		|| (((t_cmd *)tmp->content)->status_flag & FLG_EO_CMD)))
		return (ONLY_REDIR);
	return (-1);
}

int part_cmd_check(t_list *lst_cmd)
{
	t_list *tmp;
	int ret_only_pipe;
	int ret_only_redir;

	tmp = lst_cmd;
	ret_only_pipe = only_pipe(lst_cmd);
	ret_only_redir = only_redir(lst_cmd);
	if (ret_only_pipe == -1 && ret_only_redir == -1
		&& (!(((t_cmd *)tmp->content)->status_flag & FLG_EO_CMD)
		|| !(((t_cmd *)tmp->content)->status_flag & FLG_EOL)))
		return (MIX);
	else if (ret_only_pipe != -1)
		return (ONLY_PIPE);
	else if (ret_only_redir != -1)
		return (ONLY_REDIR);
	return (-1);
}

int 	syntax_error(t_list *lst_cmd)
{
	t_list *tmp;

	tmp = lst_cmd;
	if (tmp && !((t_cmd *)tmp->content)->args
		&& ((((t_cmd *)tmp->content)->status_flag & FLG_PIPE)
		|| (((t_cmd *)tmp->content)->status_flag & FLG_EO_CMD)
		|| (((t_cmd *)tmp->content)->status_flag & FLG_INPUT)
		|| (((t_cmd *)tmp->content)->status_flag & FLG_OUTPUT)
		|| (((t_cmd *)tmp->content)->status_flag & FLG_APPEND)))
	{
		if (((t_cmd *)tmp->content)->status_flag & FLG_PIPE)
			ft_dprintf(STDERR_FILENO, PROG_NAME": "SYNTAXERR" `|'\n");
		else if (((t_cmd *)tmp->content)->status_flag & FLG_EO_CMD)
			ft_dprintf(STDERR_FILENO, PROG_NAME": "SYNTAXERR" `;'\n");
		else
			ft_dprintf(STDERR_FILENO, PROG_NAME": "SYNTAXERR" `newline'\n");
		return (1);
	}
	return (0);
}

int 	syntax_error2(t_list *lst_cmd)
{
	t_list *tmp;

	tmp = lst_cmd;
	if (tmp && tmp->next && ((t_cmd *)tmp->content)->args
		&& !((t_cmd *)tmp->next->content)->args
		&& ((((t_cmd *)tmp->content)->status_flag & FLG_INPUT)
		|| (((t_cmd *)tmp->content)->status_flag & FLG_OUTPUT)
		|| (((t_cmd *)tmp->content)->status_flag & FLG_APPEND)))
	{
		ft_dprintf(STDERR_FILENO, PROG_NAME": "SYNTAXERR" `newline'\n");
		return (1);
	}
	return (0);
}