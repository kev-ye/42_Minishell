/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   pwd.c                                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kaye <kaye@student.42.fr>                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/05/21 14:21:48 by kaye              #+#    #+#             */
/*   Updated: 2021/05/30 17:52:34 by kaye             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

int	ft_pwd(void)
{
	char *path;

	path = getcwd(NULL, 0);
	if (!path)
		return (ERROR);
	ft_dprintf(STDOUT_FILENO, "%s\n", path);
	free(path);
	// return (SUCCESS);
	exit(0);
}
