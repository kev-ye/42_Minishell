/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   redir_output_utils.c                               :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kaye <kaye@student.42.fr>                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/06/15 18:13:57 by kaye              #+#    #+#             */
/*   Updated: 2021/06/15 18:59:56 by kaye             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

static int trunc_fd = O_WRONLY | O_TRUNC | O_CREAT;
static int append_fd = O_WRONLY | O_APPEND | O_CREAT;

int check_for_next(t_list *lst_cmd)
{
    int next_flag;

    next_flag = 0;
    if (flag_check(lst_cmd) == FLG_INPUT)
        next_flag = F_INPUT;
    else if (flag_check(lst_cmd) == FLG_OUTPUT)
        next_flag = FLG_OUTPUT;
    else if (flag_check(lst_cmd) == FLG_APPEND)
        next_flag = F_APPEND;
    else if (flag_check(lst_cmd) == FLG_DINPUT)
        next_flag = F_DINPUT;
    return (next_flag);
}

void creat_fd_inter(t_list *cmd, int *fd_output, int flag_is)
{
    if (flag_is == F_INPUT)
        *fd_output = open(((t_cmd *)cmd->content)->args[0], trunc_fd, 0666);
    else if (flag_is == F_APPEND)
        *fd_output = open(((t_cmd *)cmd->content)->args[0], append_fd, 0666);
    if (*fd_output == -1)
    {
        ft_dprintf(STDERR_FILENO, "minishell: %s: %s\n", ((t_cmd *)cmd->content)->args[0], strerror(errno));
		exit(LRV_GENERAL_ERROR);
    }
}

void creat_fd_last(t_list *cmd, int *fd_output, int flag_is)
{
    if (flag_is == F_INPUT)
    {
        if (*fd_output != -1 && *fd_output != -2)
            close(*fd_output);
        *fd_output = open(((t_cmd *)cmd->content)->args[0], trunc_fd, 0666);
    }
    else if (flag_is == F_APPEND)
    {
        if (*fd_output != -1 && *fd_output != -2)
            close(*fd_output);
        *fd_output = open(((t_cmd *)cmd->content)->args[0], append_fd, 0666);
    }
    if (*fd_output == -1)
    {
        ft_dprintf(STDERR_FILENO, "minishell: %s: %s\n", ((t_cmd *)cmd->content)->args[0], strerror(errno));
		exit(LRV_GENERAL_ERROR);
    }
}

int create_fd(t_list *cmd)
{
    int first;
    int fd_output;
    int flag_is;

    fd_output = -2;
    flag_is = 0;
    first = 1;
    while (cmd && is_redir(cmd))
    {
        if (first == 1)
            first = 0;
        else if (!first && is_redir(cmd))
            creat_fd_inter(cmd, &fd_output, flag_is);
        else if (!first)
            creat_fd_last(cmd, &fd_output, flag_is);
        flag_is = check_for_next(cmd);
        cmd = cmd->next;
    }
    return (fd_output);
}