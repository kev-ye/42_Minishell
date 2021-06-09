/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   only_pipe.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kaye <kaye@student.42.fr>                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/06/06 19:39:24 by kaye              #+#    #+#             */
/*   Updated: 2021/06/09 15:04:23 by kaye             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

int	count_pipe(t_list *lst_cmd)
{
	t_list *tmp;
	int count;

	tmp = lst_cmd;
	count = 0;
	while (tmp && (((t_cmd *)tmp->content)->status_flag & FLG_PIPE))
	{
		++count;
		tmp = tmp->next;
	}
	return (count);
}

static void	*first_cmd_with_pipe(void *cmd, int *fd)
{
	pid_t	pid;
	int 	status = 1;
	int 	builtin_status = 1;

	pid = fork();
	if (pid < 0)
			exit(PID_FAILURE);
	else if (pid == 0)
	{
		// printf("\n---> first cmd go in\n");
		dup2(fd[1], STDOUT_FILENO);
		builtin_status = builtin_exec(((t_cmd *)cmd)->args);
		if (builtin_status == NOT_FOUND)
			sys_exec(cmd);
		if (builtin_status != NOT_FOUND)
			exit(SUCCESS);
		exit(EXEC_FAILURE);
	}
	else
	{
		wait(&status);
		close(fd[1]);           // get stdout, need to close, because if not, stdout is always open, so the fd for stdin never have EOF
	}
	if (WIFEXITED(status) != 0)
		singleton()->last_return_value = WEXITSTATUS(status);
	else if (WIFSIGNALED(status) == 1)
		singleton()->last_return_value = LRV_KILL_SIG + WTERMSIG(status);
	return (fd);
}

static void interm_cmd_with_pipe(void *cmd, int *fd, int fd_index)
{
	pid_t	pid;
	int 	status = 1;
	int 	builtin_status = 1;

	pid = fork();
	if (pid < 0)
		exit(PID_FAILURE);
	else if (pid == 0)
	{
		// printf("\n---> interm cmd go in\n");
		dup2(fd[fd_index * 2], STDIN_FILENO);
	
		dup2(fd[(fd_index + 1) * 2 + 1], STDOUT_FILENO);
		builtin_status = builtin_exec(((t_cmd *)cmd)->args);
		if (builtin_status == NOT_FOUND)
			sys_exec(cmd);
		// need add free here because sys_exec can fail
		close(fd[1]);
		if (builtin_status != NOT_FOUND)
			exit(SUCCESS);
		exit(EXEC_FAILURE);
	}
	else
	{
		wait(&status);
		close(fd[(fd_index + 1) * 2 + 1]);
	}
	if (WIFEXITED(status) != 0)
		singleton()->last_return_value = WEXITSTATUS(status);
	else if (WIFSIGNALED(status) == 1)
		singleton()->last_return_value = LRV_KILL_SIG + WTERMSIG(status);
}

static void	last_cmd_with_pipe(void *cmd, int *fd, int fd_index)
{
	pid_t	pid;
	int 	status = 1;
	int 	builtin_status = 1;

	pid = fork();
	if (pid < 0)
			exit(PID_FAILURE);
	else if (pid == 0)
	{
		// printf("\n---> last cmd go in\n\n");
		dup2(fd[fd_index * 2], STDIN_FILENO);
		builtin_status = builtin_exec(((t_cmd *)cmd)->args);
		if (builtin_status == NOT_FOUND)
			sys_exec(cmd);
		if (builtin_status != NOT_FOUND)
			exit(SUCCESS);
		exit(EXEC_FAILURE);
	}
	else
	{
		wait(&status);
	}
	if (WIFEXITED(status) != 0)
		singleton()->last_return_value = WEXITSTATUS(status);
	else if (WIFSIGNALED(status) == 1)
		singleton()->last_return_value = LRV_KILL_SIG + WTERMSIG(status);
}

static void cmd_with_multi_pipe(t_list *lst_cmd, int *fd)
{
	t_list 	*tmp;
	int		fd_index;

	tmp = lst_cmd;
	fd_index = 0;
	while (tmp && (((t_cmd *)tmp->content)->status_flag & FLG_PIPE))
	{
		interm_cmd_with_pipe(tmp->content, fd, fd_index);
		++fd_index;
		tmp = tmp->next;
	}
	last_cmd_with_pipe(tmp->content, fd, fd_index);
}

void cmd_with_pipe(t_list *lst_cmd)
{
	t_list 	*tmp;
	int 	*fd;
	int 	i;
	int 	pipe_len;

	tmp = lst_cmd;
	pipe_len = count_pipe(tmp);
	fd = malloc(sizeof(int) * (pipe_len * 2));
	if (!fd)
		return ;
	
	// create pipe for each cmd
	i = 0;
	while (i < pipe_len)
		pipe(fd + (i++ * 2));

	// lauch the first cmd with pipe
	first_cmd_with_pipe(tmp->content, fd);

	// multi cmd with pipe
	cmd_with_multi_pipe(tmp->next, fd);

	// close all pipe fd
	i = 0;
	while (i < pipe_len * 2)
		close(fd[i++]);
}