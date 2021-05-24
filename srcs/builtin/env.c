/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   env.c                                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kaye <kaye@student.42.fr>                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/05/21 15:17:43 by kaye              #+#    #+#             */
/*   Updated: 2021/05/24 14:31:25 by kaye             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

static void    show_env(t_list **env)
{
    t_list *tmp;

    tmp = *env;
    while (tmp)
    {
        if (ft_strchr((char *)tmp->content, '='))
            ft_dprintf(STDIN_FILENO, "%s\n", (char *)tmp->content);
        tmp = tmp->next;
    }
}

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
        show_env(&singleton()->env);
    // else if (cmd->ac >= 2)
    // {
    //     // coming soon ...
    //     return ;
    // }
    return (SUCCESS);
}
