/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   builtin_exec.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: besellem <besellem@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/06/08 13:06:04 by kaye              #+#    #+#             */
/*   Updated: 2021/06/14 17:59:13 by besellem         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

static int ft_exec_builtin_cmd(char **cmds)
{
	static t_builtin	builtin[] = {
		{"echo", ft_echo, NULL}, {"cd", ft_cd, NULL}, {"pwd", NULL, ft_pwd},
		{"env", ft_env, NULL}, {"unset", ft_unset, NULL}, 
		{"export", ft_export, NULL}, {"exit", ft_exit, NULL},
		{"clear", NULL, ft_clear}, {NULL, NULL, NULL}
	};
	int					i;

	i = 0;
	while (builtin[i].cmd)
	{
		if (cmds && !ft_strcmp(cmds[0], builtin[i].cmd))
		{
			if (!builtin[i].f1)
				return (builtin[i].f2());
			else
				return (builtin[i].f1(cmds));
		}
		++i;
	}
	return (NOT_FOUND);
}

int builtin_exec(char **cmds)
{
	int ret;

	ret = ft_exec_builtin_cmd(cmds);
	if (ret == NOT_FOUND)
		return (ret);
	singleton()->last_return_value = ret;
	// ft_strsfree(ft_strslen(cmds) + 1, cmds);
	return (SUCCESS);
}