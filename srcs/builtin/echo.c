/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   echo.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kaye <kaye@student.42.fr>                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/05/21 15:17:41 by kaye              #+#    #+#             */
/*   Updated: 2021/05/22 15:48:12 by kaye             ###   ########.fr       */
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
            printf("in loop i : %d len_cmd : %d\n", i, len_cmd);
            printf("0 : %s 1 : %s 2 : %s\n", cmds[0], cmds[1], cmds[2]);
            printf("%s", cmds[i++]);
            if (i < len_cmd)
                printf(" ");
        }
    }
    else
    {
        i = 1;
        while (i < len_cmd && cmds[i])
        {
            printf("%s", cmds[i++]);
            if (i < len_cmd)
                printf(" ");
        }
        printf("\n");
    }
    return (SUCCESS);
}