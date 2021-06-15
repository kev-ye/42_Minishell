/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   builtin_exec.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: besellem <besellem@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/06/08 13:06:04 by kaye              #+#    #+#             */
/*   Updated: 2021/06/15 17:14:06 by besellem         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

static t_builtin	g_builtin[] = {
	{"echo", ft_echo, NULL},
	{"cd", ft_cd, NULL},
	{"pwd", NULL, ft_pwd},
	{"env", ft_env, NULL},
	{"unset", ft_unset, NULL},
	{"export", ft_export, NULL},
	{"exit", ft_exit, NULL},
	{"clear", NULL, ft_clear},
	{NULL, NULL, NULL}
};

static int	ft_exec_builtin_cmd(char **cmds)
{
	int	i;

	i = 0;
	while (g_builtin[i].cmd)
	{
		if (cmds && FALSE == ft_strcmp(cmds[0], g_builtin[i].cmd))
		{
			if (!g_builtin[i].f1)
				return (g_builtin[i].f2());
			else
				return (g_builtin[i].f1(cmds));
		}
		++i;
	}
	return (NOT_FOUND);
}

int	builtin_exec(char **cmds)
{
	int	ret;

	ret = ft_exec_builtin_cmd(cmds);
	if (NOT_FOUND == ret)
		return (ret);
	singleton()->last_return_value = ret;
	return (SUCCESS);
}
