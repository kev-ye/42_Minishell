/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   env.c                                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: besellem <besellem@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/05/21 15:17:43 by kaye              #+#    #+#             */
/*   Updated: 2021/05/23 12:17:30 by besellem         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

int    ft_env(char **cmds)
{
    int i;
    int len_cmd;

    i = 0;
    if (!cmds || !*cmds)
        return (ERROR);
    while (cmds[i])
        ++i;
    len_cmd = i;
    i = 0;
    if (len_cmd == 1)
        while (singleton()->env[i])
            printf("%s\n", singleton()->env[i++]);
    // else if (cmd->ac >= 2)
    // {
    //     // coming soon ...
    //     return ;
    // }
    return (SUCCESS);
}
