/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   pwd.c                                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kaye <kaye@student.42.fr>                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/05/21 14:21:48 by kaye              #+#    #+#             */
/*   Updated: 2021/05/21 16:09:57 by kaye             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

int    ft_pwd(void)
{
    char *path;

    path = getcwd(NULL, 0);
    if (!path)
        return (ERROR);
    printf("%s\n", path);
    free(path);
    return (SUCCESS);      
}