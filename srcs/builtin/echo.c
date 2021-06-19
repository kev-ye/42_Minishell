/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   echo.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kaye <kaye@student.42.fr>                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/05/21 15:17:41 by kaye              #+#    #+#             */
/*   Updated: 2021/06/19 18:58:50 by kaye             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

int check_opt_n(char *opt)
{
	int i;

	i = 0;
	if (opt && opt[i] && !ft_strncmp(opt, "-", 1) && opt[i + 1])
	{
		++i;
		while (opt[i])
		{
			if (opt[i] && ft_strncmp(opt + i, "n", 1) != 0)
				return (0);
			++i;
		}
	}
	else
		return (0);
	return (1);
}

int	ft_echo(char **cmds)
{
	int	len_cmd;
	int	i;

	i = 0;
	if (!cmds || !*cmds)
		return (ERROR);
	while (cmds[i])
		++i;
	len_cmd = i;
	if (*cmds && *(cmds + 1) && check_opt_n(cmds[1]))
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
}
