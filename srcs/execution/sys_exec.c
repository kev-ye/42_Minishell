/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   sys_exec.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: besellem <besellem@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/06/08 13:07:20 by kaye              #+#    #+#             */
/*   Updated: 2021/06/20 15:49:26 by besellem         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

static void	ft_exec_cmd(char *file, t_cmd *cmds)
{
	char	**env;
	int		ret = 0;

	env = ft_lst2strs(&singleton()->env);            //// maybe get leak here --- need to check
	ret = execve(file, cmds->args, env);
	if (ret == -1)
		ft_memdel((void **)env);
}

void	sys_exec(void *ptr)
{
	t_cmd	*cmd;
	char	*ex;
	int 	not_found;

	cmd = ptr;
	not_found = 0;
	if (!cmd->args || !cmd->args)
		return ;
	ex = search_executable(cmd->args[0]);
	if (ex)
	{
		ft_exec_cmd(ex, cmd);
		ft_memdel((void **)&ex);
	}
	else
	{
		ft_dprintf(STDERR_FILENO, PROG_NAME ": %s: command not found\n",
			cmd->args[0]);
		not_found = 1;
	}
	if (not_found == 1)
		exit(LRV_CMD_NOT_FOUND);
}
