/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   echo.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kaye <kaye@student.42.fr>                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/05/21 15:17:41 by kaye              #+#    #+#             */
/*   Updated: 2021/05/21 16:52:20 by kaye             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

void    ft_echo(t_cmd *cmd)
{
    int i;

    if (ft_strcmp(cmd->av[1]), "-n")
    {
        i = 2;
        while (i < ac)
        {
            printf("%s", cmd->av[i++]);
            if (i < ac - 1)
                printf(" ");
        }
    }
    else
    {
        i = 1;
        while (i < ac)
        {
            printf("%s", cmd->av[i++]);
            if (i < ac - 1)
                printf(" ");
        }
        printf("\n");
    }
}