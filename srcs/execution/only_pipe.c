/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   only_pipe.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kaye <kaye@student.42.fr>                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/06/06 19:39:24 by kaye              #+#    #+#             */
/*   Updated: 2021/06/06 19:50:14 by kaye             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

static int	*first_cmd_with_pipe(void *cmd)
{
	pid_t	pid;
	int		*fd = malloc(sizeof(int) * 2);

	if (!fd)
		return (NULL);
	pipe(fd);
	pid = fork();
	if (pid < 0)
			exit(1);
	else if (pid == 0)
	{
		close(fd[0]);
		dup2(fd[1], STDOUT_FILENO);
		ft_pre_exec_cmd(cmd);
		close(fd[1]);
		exit(0);
	}
	else
	{
		close(fd[1]);
		wait(NULL);
	}
	return (fd);
}

static int	*interm_cmd_with_pipe(void *cmd, int *get_fd)
{
	pid_t	pid;
	int		*tmp = get_fd;
	int		*fd = malloc(sizeof(int) * 2);

	if (!fd)
		return (NULL);
	pipe(fd);
	pid = fork();
	if (pid < 0)
		exit(ERROR);
	else if (pid == 0)
	{
		close(get_fd[1]);
		dup2(get_fd[0], STDIN_FILENO);
		close(get_fd[0]);
	
		close(fd[0]);
		dup2(fd[1], STDOUT_FILENO);
		ft_pre_exec_cmd(cmd);
		close(fd[1]);
		exit(0);
	}
	else
	{
		close(fd[1]);
		wait(NULL);
	}
	free(tmp);
	return (fd);
}

static void	last_cmd_with_pipe(void *cmd, int *get_fd)
{
	const pid_t	pid = fork();
	int		*tmp = get_fd;

	if (pid < 0)
		exit(ERROR);
	else if (0 == pid)
	{
		close(get_fd[1]);
		dup2(get_fd[0], STDIN_FILENO);
		ft_pre_exec_cmd(cmd);
		close(get_fd[0]);
		exit(0);
	}
	else
	{
		close(get_fd[1]);
		wait(NULL);
	}
	free(tmp);
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