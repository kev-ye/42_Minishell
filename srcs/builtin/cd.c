/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   cd.c                                               :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kaye <kaye@student.42.fr>                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/05/21 14:11:08 by kaye              #+#    #+#             */
/*   Updated: 2021/05/21 16:52:41 by kaye             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

int    ft_cd(const char *path)
{
    if (chdir(path) == -1)
    {
        printf("minishell: cd: %s: %s\n", path, strerror(errno));
        return (ERROR);
    }
    return (SUCCESS);
}