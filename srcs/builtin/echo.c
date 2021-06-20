/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   echo.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: besellem <besellem@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/05/21 15:17:41 by kaye              #+#    #+#             */
/*   Updated: 2021/06/20 14:26:12 by besellem         ###   ########.fr       */
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

// void	case_last_return_value(char *arg)
// {
// 	ft_
// }


// void	get_var_last_return_case(char *s)
// {
// 	char	*ptr;
// 	char	*found = ft_strstr(s, "$?");

// 	ft_asprintf(&ptr, "%.*s%d%s", i - 1, s, singleton()->last_return_value,
// 		s + i + 1);
// 	*s = ptr;
// 	return (ft_nblen(singleton()->last_return_value) - 1);
// }



int	ft_echo(t_cmd *cmds)
{
	int	len_cmd;
	int	i;

	i = 0;
	if (!cmds || !cmds->args || !*cmds->args)
		return (ERROR);
	while (cmds->args[i])
		++i;
	len_cmd = i;
	if (*cmds->args && *(cmds->args + 1) && check_opt_n(cmds->args[1]))
	{
		i = 2;
		while (i < len_cmd)
		{

			// echo " bonjour          $? "
			// if (cmds->status_flag & FLG_LRV)
			// 	ft_dprintf(STDOUT_FILENO, "%s", cmds->args[i]);
			// else
				ft_dprintf(STDOUT_FILENO, "%s", cmds->args[i]);
			if (i < len_cmd)
				ft_dprintf(STDOUT_FILENO, " ");
			++i;
		}
	}
	else
	{
		i = 1;
		while (i < len_cmd && cmds->args[i])
		{
			ft_dprintf(STDOUT_FILENO, "%s", cmds->args[i++]);
			if (i < len_cmd)
				ft_dprintf(STDOUT_FILENO, " ");
		}
		ft_dprintf(STDOUT_FILENO, "\n");
	}
	return (SUCCESS);
}
