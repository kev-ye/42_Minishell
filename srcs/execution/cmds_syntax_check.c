/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   cmds_syntax_check.c                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kaye <kaye@student.42.fr>                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/06/07 14:47:59 by kaye              #+#    #+#             */
/*   Updated: 2021/06/09 15:03:16 by kaye             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

static int 	redir_error(t_list *lst_cmd)
{
	t_list *tmp;

	tmp = lst_cmd;
    if (((t_cmd *)tmp->next->content)->status_flag & FLG_EOL)
        ft_dprintf(STDERR_FILENO, PROG_NAME": "SYNTAXERR" `newline'\n");
	else if (((t_cmd *)tmp->next->content)->status_flag & FLG_PIPE)
		ft_dprintf(STDERR_FILENO, PROG_NAME": "SYNTAXERR" `|'\n");
	else if (((t_cmd *)tmp->next->content)->status_flag & FLG_EO_CMD)
		ft_dprintf(STDERR_FILENO, PROG_NAME": "SYNTAXERR" `;'\n");
	else if (((t_cmd *)tmp->next->content)->status_flag & FLG_INPUT)
		ft_dprintf(STDERR_FILENO, PROG_NAME": "SYNTAXERR" `<'\n");
	else if (((t_cmd *)tmp->next->content)->status_flag & FLG_OUTPUT)
		ft_dprintf(STDERR_FILENO, PROG_NAME": "SYNTAXERR" `>'\n");
	else if (((t_cmd *)tmp->next->content)->status_flag & FLG_APPEND)
		ft_dprintf(STDERR_FILENO, PROG_NAME": "SYNTAXERR" `>>'\n");
	else if (((t_cmd *)tmp->next->content)->status_flag & FLG_DINPUT)
		ft_dprintf(STDERR_FILENO, PROG_NAME": "SYNTAXERR" `<<'\n");
	else
		return (0);
	return (1);
}

static int 	multi_error(t_list *lst_cmd)
{
	t_list *tmp;

	tmp = lst_cmd;
	if ((((t_cmd *)tmp->content)->status_flag & FLG_PIPE)
		&& (!((t_cmd *)tmp->next->content)->args))
        ft_dprintf(STDERR_FILENO, PROG_NAME": You should not do this.\n");
    else if (((t_cmd *)tmp->content)->status_flag & FLG_PIPE)
        ft_dprintf(STDERR_FILENO, PROG_NAME": "SYNTAXERR" `|'\n");
    else if (((t_cmd *)tmp->content)->status_flag & FLG_EO_CMD)
        ft_dprintf(STDERR_FILENO, PROG_NAME": "SYNTAXERR" `;'\n");
	else if ((((t_cmd *)tmp->next->content)->status_flag & FLG_INPUT)
		|| (((t_cmd *)tmp->next->content)->status_flag & FLG_OUTPUT)
		|| (((t_cmd *)tmp->next->content)->status_flag & FLG_APPEND)
		|| (((t_cmd *)tmp->next->content)->status_flag & FLG_DINPUT))
		ft_dprintf(STDERR_FILENO, PROG_NAME": "SYNTAXERR" `newline'\n");
    else if (((t_cmd *)tmp->next->content)->status_flag & FLG_PIPE)
        ft_dprintf(STDERR_FILENO, PROG_NAME": "SYNTAXERR" `|'\n");
    else if (((t_cmd *)tmp->next->content)->status_flag & FLG_EO_CMD)
        ft_dprintf(STDERR_FILENO, PROG_NAME": "SYNTAXERR" `;'\n");
    else
        return (0);
	return (1);
}

static int syntax_with_redir(t_list *lst_cmd)
{
    t_list *tmp;

    tmp = lst_cmd;
    if (((t_cmd *)tmp->content)->args
		&& ((((t_cmd *)tmp->content)->status_flag & FLG_INPUT)
		|| (((t_cmd *)tmp->content)->status_flag & FLG_OUTPUT)
		|| (((t_cmd *)tmp->content)->status_flag & FLG_APPEND)
		|| (((t_cmd *)tmp->content)->status_flag & FLG_DINPUT))
		&& tmp->next && (!((t_cmd *)tmp->next->content)->args))
		return (1);
    else if (((t_cmd *)tmp->content)->args
		&& ((((t_cmd *)tmp->content)->status_flag & FLG_INPUT)
		|| (((t_cmd *)tmp->content)->status_flag & FLG_OUTPUT)
		|| (((t_cmd *)tmp->content)->status_flag & FLG_APPEND)
		|| (((t_cmd *)tmp->content)->status_flag & FLG_DINPUT))
		&& tmp->next && (!((t_cmd *)tmp->next->content)->args)
		&& (((t_cmd *)tmp->next->content)->status_flag & FLG_EOL))
		return (1);
	else if (!((t_cmd *)tmp->content)->args
		&& ((((t_cmd *)tmp->content)->status_flag & FLG_INPUT)
		|| (((t_cmd *)tmp->content)->status_flag & FLG_OUTPUT)
		|| (((t_cmd *)tmp->content)->status_flag & FLG_APPEND)
		|| (((t_cmd *)tmp->content)->status_flag & FLG_DINPUT))
		&& tmp->next && (!((t_cmd *)tmp->next->content)->args))
		return (1);
    return (0);
}

static int syntax_with_multi(t_list *lst_cmd)
{
    t_list *tmp;

    tmp = lst_cmd;
    if (((t_cmd *)tmp->content)->args
		&& (((t_cmd *)tmp->content)->status_flag & FLG_EO_CMD)
        && tmp->next && (((t_cmd *)tmp->next->content)->status_flag &FLG_EOL))
        return (0);
	else if (((t_cmd *)tmp->content)->args
		&& (((t_cmd *)tmp->content)->status_flag & FLG_PIPE)
        && tmp->next && (!((t_cmd *)tmp->next->content)->args)
		&& (((t_cmd *)tmp->next->content)->status_flag &FLG_EOL))
        return (2); 
    else if (((t_cmd *)tmp->content)->args
		&& ((((t_cmd *)tmp->content)->status_flag & FLG_PIPE)
		|| (((t_cmd *)tmp->content)->status_flag & FLG_EO_CMD))
		&& tmp->next && (!((t_cmd *)tmp->next->content)->args))
		return (2);
    else if (!((t_cmd *)tmp->content)->args
		    && ((((t_cmd *)tmp->content)->status_flag & FLG_PIPE)
		    || (((t_cmd *)tmp->content)->status_flag & FLG_EO_CMD))
		    && tmp->next)
		return (2);
    return (0);
}

int syntax_parser(t_list *lst_cmd)
{
	t_list *tmp;
	int syntax;
	int ret;

	ret = 0;
	syntax = 0;
	tmp = lst_cmd;
	while (tmp)
	{
        syntax = syntax_with_redir(tmp);
        if (syntax == 1)
            break ;
        syntax = syntax_with_multi(tmp);
        if (syntax == 2)
            break ;
		tmp = tmp->next;
	}
	if (tmp)
	{
		if (syntax == 1)
		{
			printf("in redir error\n");
			ret = redir_error(tmp);
		}
		else if (syntax == 2)
		{
			printf("in multi error\n");
			ret = multi_error(tmp);
		}
	}
	return (ret);
}