/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   exec_cmds.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kaye <kaye@student.42.fr>                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/05/20 22:33:29 by besellem          #+#    #+#             */
/*   Updated: 2021/06/11 18:14:55 by kaye             ###   ########.fr       */
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
		{
			// wait(&status);
			waitpid(pid, &status, 0);
		}
		if (WIFEXITED(status) != 0)
		{
			// printf("normal exit\n");
			singleton()->last_return_value = WEXITSTATUS(status);
		}
		else if (WIFSIGNALED(status) == 1)
		{
			// printf("signal exit\n");
			singleton()->last_return_value = LRV_KILL_SIG + WTERMSIG(status);
		}	
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
		if (((t_cmd *)tmp->content)->args
			&& ((((t_cmd *)tmp->content)->status_flag & FLG_EO_CMD)
			|| (((t_cmd *)tmp->content)->status_flag & FLG_EOL)))
		{
			printf(B_PURPLE"simple cmd"CLR_COLOR"\n");
			built_exec = simple_cmd(tmp->content);
		}
		else
		{
			cmd_line = part_cmd_check(tmp);
			if (cmd_line == ONLY_PIPE)
			{
				// printf(B_PURPLE"pipe cmd"CLR_COLOR"\n");
				cmd_with_pipe(tmp);
			}
			else if (cmd_line == ONLY_REDIR)
			{
				// printf(B_PURPLE"redir cmd"CLR_COLOR"\n");
				cmd_with_redir(tmp->content, tmp);
			}
			else if (cmd_line == MIX)
			{
				// printf(B_PURPLE"mix cmd"CLR_COLOR"\n");	
				cmd_with_mix(tmp);
			}
		}
		while (tmp && !(((t_cmd *)tmp->content)->status_flag & FLG_EO_CMD))
			tmp = tmp->next;
		if (tmp)
			tmp = tmp->next;
	}
	return (built_exec);
}