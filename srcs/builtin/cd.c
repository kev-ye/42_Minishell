/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   cd.c                                               :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: besellem <besellem@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/05/21 14:11:08 by kaye              #+#    #+#             */
/*   Updated: 2021/05/25 16:21:07 by besellem         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

int	ft_cd(char **cmds)
{
	if (!cmds || !*cmds)
		return (ERROR);
	if (*cmds && !*(cmds + 1))
	{
		if (chdir(getenv("HOME")) == -1)
		{
			ft_dprintf(STDERR_FILENO, "%s: cd: HOME not set\n", PROG_NAME);
			return (ERROR);
		}
	}
	if (*cmds && *(cmds + 1) && chdir(cmds[1]) == -1)
	{
		ft_dprintf(STDERR_FILENO, "%s: cd: %s: %s\n",
			PROG_NAME, cmds[1], strerror(errno));
		return (ERROR);
	}
	return (SUCCESS);
}
