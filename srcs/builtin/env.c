/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   env.c                                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kaye <kaye@student.42.fr>                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/05/21 15:17:43 by kaye              #+#    #+#             */
/*   Updated: 2021/05/23 15:45:06 by kaye             ###   ########.fr       */
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
        {
            // maybe need add a check condition if set a var without '='
            ft_dprintf(STDIN_FILENO, "%s\n", singleton()->env[i++]);
        }
    // else if (cmd->ac >= 2)
    // {
    //     // coming soon ...
    //     return ;
    // }
    return (SUCCESS);
}
