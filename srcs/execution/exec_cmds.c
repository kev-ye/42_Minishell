/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   exec_cmds.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kaye <kaye@student.42.fr>                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/05/20 22:33:29 by besellem          #+#    #+#             */
/*   Updated: 2021/06/18 18:29:33 by kaye             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

// seems that it must be used on forked commands - not on the main program
void	ft_quit(int code)
{
	ft_dprintf(STDERR_FILENO, B_RED "SIGNAL: Quit: %d\n" CLR_COLOR, code);
	// exit(code);
}

int	simple_cmd(void *cmd)
{
	pid_t	pid;
	int status;
	int built_exec;

	status = 1;
	built_exec = 0;
	if (builtin_exec(((t_cmd *)cmd)->args) == NOT_FOUND)
	{	
		pid = fork();
		if (pid < 0)
				exit(PID_FAILURE);
		else if (pid == 0)
		{
			sys_exec(cmd);
			exit(EXEC_FAILURE);
		}
		else
			waitpid(pid, &status, 0);
		if (WIFEXITED(status) != 0)
			singleton()->last_return_value = WEXITSTATUS(status);
		else if (WIFSIGNALED(status) == 1)
			singleton()->last_return_value = LRV_KILL_SIG + WTERMSIG(status);
	}
	else
		built_exec = 1;
	return (built_exec);
}

int	ft_exec_each_cmd(t_list *lst_cmd)
{
	t_list	*tmp;
	int		cmd_line;
	int		built_exec;

	tmp = lst_cmd;
	cmd_line = -1;
	built_exec = 0;
	if (syntax_parser(tmp))
	{
		singleton()->last_return_value = LRV_SYNTAX_ERROR;
		return (0);
	}
	while (tmp)
	{
		exec_all_in_one(tmp);
		while (tmp && flag_check(tmp) != FLG_EO_CMD)	// to remove -> "ls abcd; echo $?" case
			tmp = tmp->next;												// to remove -> "ls abcd; echo $?" case
		if (tmp)															// to remove -> "ls abcd; echo $?" case
			tmp = tmp->next;
	}
	return (built_exec);
}
