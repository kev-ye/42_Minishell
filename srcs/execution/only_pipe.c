/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   only_pipe.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kaye <kaye@student.42.fr>                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/06/06 19:39:24 by kaye              #+#    #+#             */
/*   Updated: 2021/06/08 13:46:01 by kaye             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

static int	*first_cmd_with_pipe(void *cmd)
{
	pid_t	pid;
	int		*fd = malloc(sizeof(int) * 2);
	int status = 1;

	if (!fd)
		return (NULL);
	pipe(fd);
	pid = fork();
	if (pid < 0)
			exit(PID_FAILURE);
	else if (pid == 0)
	{
		close(fd[0]);
		dup2(fd[1], STDOUT_FILENO);
		if (builtin_exec(((t_cmd *)cmd)->args) == NOT_FOUND)
			sys_exec(cmd);
		// close(fd[1]);
		exit(EXEC_FAILURE);
	}
	else
	{
		close(fd[1]);
		wait(&status);
	}
	if (WIFEXITED(status) != 0)
		singleton()->last_return_value = WEXITSTATUS(status);
	else if (WIFSIGNALED(status) == 1)
		singleton()->last_return_value = LRV_SIGINT;
	return (fd);
}

static int	*interm_cmd_with_pipe(void *cmd, int *get_fd)
{
	pid_t	pid;
	int		*tmp = get_fd;
	int		*fd = malloc(sizeof(int) * 2);
	int status = 1;

	if (!fd)
		return (NULL);
	pipe(fd);
	pid = fork();
	if (pid < 0)
		exit(PID_FAILURE);
	else if (pid == 0)
	{
		close(get_fd[1]);
		dup2(get_fd[1], STDIN_FILENO);
		close(get_fd[0]);
	
		close(fd[0]);
		dup2(fd[1], STDOUT_FILENO);
		if (builtin_exec(((t_cmd *)cmd)->args) == NOT_FOUND)
			sys_exec(cmd);
		close(fd[1]);
		exit(EXEC_FAILURE);
	}
	else
	{
		close(fd[1]);
		wait(&status);
	}
	free(tmp);
	if (WIFEXITED(status) != 0)
		singleton()->last_return_value = WEXITSTATUS(status);
	else if (WIFSIGNALED(status) == 1)
		singleton()->last_return_value = LRV_SIGINT;
	return (fd);
}

static void	last_cmd_with_pipe(void *cmd, int *get_fd)
{
	const pid_t	pid = fork();
	int		*tmp = get_fd;
	int status = 1;

	if (pid < 0)
		exit(PID_FAILURE);
	else if (0 == pid)
	{
		close(get_fd[1]);
		dup2(get_fd[0], STDIN_FILENO);
		if (builtin_exec(((t_cmd *)cmd)->args) == NOT_FOUND)
			sys_exec(cmd);
		close(get_fd[0]);
		exit(EXEC_FAILURE);
	}
	else
	{
		close(get_fd[1]);
		wait(&status);
	}
	free(tmp);
	if (WIFEXITED(status) != 0)
		singleton()->last_return_value = WEXITSTATUS(status);
	else if (WIFSIGNALED(status) == 1)
		singleton()->last_return_value = LRV_SIGINT;
}

void cmd_with_pipe(t_list *lst_cmd)
{
	t_list *tmp;
	int first;
	int *fd;

	first = 1;
	tmp = lst_cmd;
	while (tmp)
	{
		if (first == 1)
		{
			fd = first_cmd_with_pipe((t_cmd *)tmp->content);
			first = 0;
		}
		else if (((t_cmd *)tmp->content)->status_flag & FLG_PIPE)
		{
			fd = interm_cmd_with_pipe((t_cmd *)tmp->content, fd);
		}
		else
		{
			last_cmd_with_pipe((t_cmd *)tmp->content, fd);
		}
		tmp = tmp->next;
	}
}