/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   flag_check.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kaye <kaye@student.42.fr>                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/06/10 17:37:33 by kaye              #+#    #+#             */
/*   Updated: 2021/06/10 18:01:11 by kaye             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

int flag_check(t_list *lst_cmd)
{
    if (lst_cmd && (((t_cmd *)lst_cmd->content)->status_flag & FLG_EOL))
        return (FLG_EOL);
    if (lst_cmd && (((t_cmd *)lst_cmd->content)->status_flag & FLG_EO_CMD))
        return (FLG_EO_CMD);
    if (lst_cmd && (((t_cmd *)lst_cmd->content)->status_flag & FLG_PIPE))
        return (FLG_PIPE);
    if (lst_cmd && (((t_cmd *)lst_cmd->content)->status_flag & FLG_APPEND))
        return (FLG_APPEND);
    if (lst_cmd && (((t_cmd *)lst_cmd->content)->status_flag & FLG_INPUT))
        return (FLG_INPUT);
    if (lst_cmd && (((t_cmd *)lst_cmd->content)->status_flag & FLG_OUTPUT))
        return (FLG_OUTPUT);
    if (lst_cmd && (((t_cmd *)lst_cmd->content)->status_flag & FLG_DINPUT))
        return (FLG_DINPUT);
    return (NO_ONE);
}

int is_redir(t_list *lst_cmd)
{
    if (lst_cmd && ((((t_cmd *)lst_cmd->content)->status_flag & FLG_OUTPUT)
        || (((t_cmd *)lst_cmd->content)->status_flag & FLG_APPEND)
        || (((t_cmd *)lst_cmd->content)->status_flag & FLG_INPUT)
        || (((t_cmd *)lst_cmd->content)->status_flag & FLG_DINPUT)))
        return (1);
    return (0);
}

int is_sep_or_end(t_list *lst_cmd)
{
    if (lst_cmd && ((((t_cmd *)lst_cmd->content)->status_flag & FLG_EO_CMD)
        || (((t_cmd *)lst_cmd->content)->status_flag & FLG_EOL)))
        return (1);
    return (0);
}