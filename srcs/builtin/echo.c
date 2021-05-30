/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   echo.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kaye <kaye@student.42.fr>                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/05/21 15:17:41 by kaye              #+#    #+#             */
/*   Updated: 2021/05/30 19:14:17 by kaye             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

int	ft_echo(char **cmds)
{
	int	len_cmd;
	int	i;

	i = 0;
	if (!cmds || !*cmds)
		return (ERROR);
		// exit(1);
	while (cmds[i])
		++i;
	len_cmd = i;
	if (*cmds && *(cmds + 1) && !ft_strcmp(cmds[1], "-n"))
	{
		i = 2;
		while (i < len_cmd)
		{
			ft_dprintf(STDOUT_FILENO, "%s", cmds[i++]);
			if (i < len_cmd)
				ft_dprintf(STDOUT_FILENO, " ");
		}
	}
	else
	{
		i = 1;
		while (i < len_cmd && cmds[i])
		{
			ft_dprintf(STDOUT_FILENO, "%s", cmds[i++]);
			if (i < len_cmd)
				ft_dprintf(STDOUT_FILENO, " ");
		}
		ft_dprintf(STDOUT_FILENO, "\n");
	}
	return (SUCCESS);
	// exit(0);
}
