/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   cd.c                                               :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kaye <kaye@student.42.fr>                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/05/21 14:11:08 by kaye              #+#    #+#             */
/*   Updated: 2021/05/30 19:45:29 by kaye             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

int	ft_cd(char **cmds)
{
	if (!cmds || !*cmds)
		return (ERROR);
		// exit(1);
	if (*cmds && !*(cmds + 1))
	{
		if (chdir(getenv("HOME")) == -1)
		{
			ft_dprintf(STDERR_FILENO, "%s: cd: HOME not set\n", PROG_NAME);
			return (ERROR);
			// exit(1);
		}
	}
	if (*cmds && *(cmds + 1) && chdir(cmds[1]) == -1)
	{
		ft_dprintf(STDERR_FILENO, "%s: cd: %s: %s\n",
			PROG_NAME, cmds[1], strerror(errno));
		return (ERROR);
		// exit(1);
	}
	return (SUCCESS);
	// exit(0);
}
