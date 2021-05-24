/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   unset.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kaye <kaye@student.42.fr>                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/05/21 15:17:36 by kaye              #+#    #+#             */
/*   Updated: 2021/05/24 12:23:25 by kaye             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

void    del_env(t_list **lst_env, size_t len, char *cmd)
{
    t_list *prev;
    t_list *next;
    t_list *tmp;

    tmp = *lst_env;
    prev = NULL;
    while (tmp)
        if (!ft_strncmp((char *)tmp->content, cmd, len) 
            && ((char *)(tmp->content))[len] == '=')
        {
            next = tmp->next;
            ft_lstdelone(tmp, free);
            if (prev)
            {
                tmp = prev;
                tmp->next = next;
            }
            else
                tmp = next;
            return ;
        }
        else
        {
            prev = tmp;
            tmp = tmp->next;
        }
}

int ft_unset(char **cmds)
{
    if (!cmds || !*cmds)
        return (ERROR);
    if (!*(cmds + 1))
        return (SUCCESS);
    del_env(&singleton()->env, ft_strlen(cmds[1]), cmds[1]);
    return (SUCCESS);
}