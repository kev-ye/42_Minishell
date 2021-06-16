/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   create_fd.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kaye <kaye@student.42.fr>                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/06/15 18:13:57 by kaye              #+#    #+#             */
/*   Updated: 2021/06/16 16:38:17 by kaye             ###   ########.fr       */
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
        next_flag = F_TRUNC;
    else if (flag_check(lst_cmd) == FLG_APPEND)
        next_flag = F_APPEND;
    else if (flag_check(lst_cmd) == FLG_DINPUT)
        next_flag = F_DINPUT;
    return (next_flag);
}

void creat_output_fd(t_list *cmd, int fd_output, int flag_is)
{
    if (flag_is == F_TRUNC)
        fd_output = open(((t_cmd *)cmd->content)->args[0], trunc_fd, 0666);
    else if (flag_is == F_APPEND)
        fd_output = open(((t_cmd *)cmd->content)->args[0], append_fd, 0666);
    if (fd_output == -1)
		exit(LRV_GENERAL_ERROR);
    else
        close(fd_output);
}

t_list *check_input_fd(t_list *cmd, int fd_input, int flag_is)
{
    if (flag_is == F_INPUT)
    {
        fd_input = open(((t_cmd *)cmd->content)->args[0], O_RDWR);
        if (fd_input == -1)
        {
            while (cmd && is_redir(cmd))
                cmd = cmd->next;
            cmd = cmd->next;
        }
        else
            close(fd_input);
    }
    return (cmd);
}

int create_fd(t_list *cmd)
{
    int first;
    int fd_output;
    int fd_input;
    int flag_is;

    fd_output = -2;
    fd_input = -2;
    flag_is = 0;
    first = 1;
    while (cmd)
    {
        if (first == 1)
            first = 0;
        else if (!first && (flag_is == F_APPEND || flag_is == F_TRUNC))
            creat_output_fd(cmd, fd_output, flag_is);
        else
            cmd = check_input_fd(cmd, fd_input, flag_is);
        flag_is = check_for_next(cmd);
        cmd = cmd->next;
    }
    return (fd_output);
}