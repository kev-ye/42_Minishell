/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   unset.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kaye <kaye@student.42.fr>                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/05/21 15:17:36 by kaye              #+#    #+#             */
/*   Updated: 2021/05/22 15:48:19 by kaye             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

int ft_unset(char **cmds)
{
    size_t len;
    char **env;
    char **new_env;

    if (!cmds || !*cmds)
        return (ERROR);
    if (!*(cmds + 1))
        return (SUCCESS);
    new_env = NULL;
    len = ft_strlen(cmds[1]);
    env = singleton()->env;
    if (env)
    {
        while (*env)
        {
            if (!ft_strncmp(*env, cmds[1], len))
            {
                new_env = env;
                while(*new_env)
                {
                    new_env[0] = new_env[1];
                    ++new_env;
                }
            }
            else
                ++env;
        }
    }
    return (SUCCESS);
}