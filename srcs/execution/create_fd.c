/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   create_fd.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kaye <kaye@student.42.fr>                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/06/15 18:13:57 by kaye              #+#    #+#             */
/*   Updated: 2021/06/19 17:49:23 by kaye             ###   ########.fr       */
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

char    *new_tmp_fd_name(int i)
{
    char *old_name;
    char *new_name;
    char *fd_nbr;
    int fd;

    fd = -1;
    while (1)
    {
        fd_nbr = ft_itoa(i);
        old_name = malloc(sizeof(char) * ft_strlen(TMP_FD) + 1);
        if (!old_name)
            return (NULL);
        ft_strcpy(old_name, TMP_FD);
        new_name = ft_strjoin(old_name, fd_nbr);
        free(old_name);
        free(fd_nbr);
        fd = open(new_name, O_RDWR);
        if (fd != -1)
        {
            close(fd);
            free(new_name);
            ++i;
        }
        else
            return (new_name);
    }
    return (NULL);
}

t_list    *check_input_fd(t_list *cmd, int fd_input, int flag_is)
{
    char *input_str;
    char *tmp_fd;
    int i;

    i = 1;
    tmp_fd = NULL;
    if (flag_is == F_DINPUT)
    {
        // fd_input = open(TMP_FD, O_RDWR | O_CREAT | O_TRUNC | O_APPEND, 0666);
        tmp_fd = new_tmp_fd_name(i);
        fd_input = open(tmp_fd, O_RDWR | O_CREAT | O_TRUNC | O_APPEND, 0666);
		if (fd_input == -1)
		{
            free(tmp_fd);
			ft_dprintf(STDERR_FILENO, "open for double input crash in check_input_fd\n");
			exit(LRV_GENERAL_ERROR);
		}
        else
        {
            while (1)
            {
                singleton()->rl_lvl = 2;
                input_str = readline("> ");
                if (input_str && !ft_strcmp(input_str, ((t_cmd *)cmd->content)->args[0]))
                {
                    free(input_str);
                    if (((t_cmd *)cmd->content)->status_flag & FLG_DINPUT)
                    {
                        close(fd_input);
                        fd_input = open(tmp_fd, O_RDWR | O_CREAT | O_TRUNC | O_APPEND, 0666);
                        if (fd_input == -1)
                        {
                            ft_dprintf(STDERR_FILENO, "open for double input crash in readline\n");
                            exit(LRV_GENERAL_ERROR);
                        }
                        cmd = cmd->next;
                        continue ;
                    }
                    else
                        break ;
                }
                else if (!input_str)
                    break ;
                if (input_str)
                    ft_putendl_fd(input_str, fd_input);
            }
            if (fd_input != -1)
                close(fd_input);
            free(tmp_fd);
        }
    }
    return (cmd);
}

void create_fd(t_list *cmd)
{
    int first;
    int fd_output;
    int fd_input;
    int flag_is;

    fd_output = -1;
    fd_input = -1;
    flag_is = 0;
    first = 1;
    while (cmd)
    {
        if (first == 1)
            first = 0;
        else if (!first && (flag_is == F_APPEND || flag_is == F_TRUNC))
            creat_output_fd(cmd, fd_output, flag_is);
        if (flag_check(cmd) == FLG_EO_CMD)
            break ;
        flag_is = check_for_next(cmd);
        cmd = cmd->next;
    }
}

void create_fd_input(t_list *cmd)
{
    int first;
    int fd_output;
    int fd_input;
    int flag_is;

    fd_output = -1;
    fd_input = -1;
    flag_is = 0;
    first = 1;
    while (cmd)
    {
        if (first == 1)
            first = 0;
        else if (!first && flag_is == F_DINPUT)
            cmd = check_input_fd(cmd, fd_input, flag_is);
        flag_is = check_for_next(cmd);
        cmd = cmd->next;
    }
}