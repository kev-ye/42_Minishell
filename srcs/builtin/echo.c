/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   echo.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: besellem <besellem@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/05/21 15:17:41 by kaye              #+#    #+#             */
/*   Updated: 2021/05/23 12:13:34 by besellem         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

int    ft_echo(char **cmds)
{
    int i;
    int len_cmd;

    i = 0;
    if (!cmds || !*cmds)
        return (ERROR);
    while (cmds[i])
        ++i;
    len_cmd = i;
    if (*cmds && *(cmds + 1) && !ft_strcmp(cmds[1], "-n"))
    {
        i = 2;
        while (i < len_cmd)
        {
            ft_dprintf(STDIN_FILENO, "%s", cmds[i++]);
            if (i < len_cmd)
                ft_dprintf(STDIN_FILENO, " ");
        }
    }
    else
    {
        i = 1;
        while (i < len_cmd && cmds[i])
        {
            ft_dprintf(STDIN_FILENO, "%s", cmds[i++]);
            if (i < len_cmd)
                ft_dprintf(STDIN_FILENO, " ");
        }
        ft_dprintf(STDIN_FILENO, "\n");
    }
    return (SUCCESS);
}