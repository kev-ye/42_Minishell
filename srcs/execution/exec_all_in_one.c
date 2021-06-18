/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   exec_all_in_one.c                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kaye <kaye@student.42.fr>                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/06/16 19:03:54 by kaye              #+#    #+#             */
/*   Updated: 2021/06/18 18:29:32 by kaye             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

static int check_is_inter(t_list *lst_cmd)
{
	while (lst_cmd && is_redir(lst_cmd))
		lst_cmd = lst_cmd->next;
	if (flag_check(lst_cmd) == FLG_EO_CMD || flag_check(lst_cmd) == FLG_EOL)
		return (0);
	return (1);
}

static int check_is_redir_cmd(t_list *lst_cmd)
{
	if (lst_cmd && is_redir(lst_cmd))
		return (1);
	return (0);
}

void	*first_cmd(void *cmd, int *fd, t_list *lst_cmd, int pipe_len)
{
	pid_t	pid;
	int 	status = 1;
	int 	builtin_status = 1;
    int 	input_fd;
	int		output_fd;

    input_fd = -1;
	output_fd = -1;
	pid = fork();
	if (pid < 0)
			exit(PID_FAILURE);
	else if (pid == 0)
	{
		if (check_is_redir_cmd(lst_cmd))
		{
			cmd = get_complete_cmd(cmd, lst_cmd);
			redir_parser2(lst_cmd, &input_fd, &output_fd);
		}
        if (pipe_len > 0)
        {
            if (output_fd == -1)
                dup2(fd[1], STDOUT_FILENO);
            else
                dup2(fd[1], output_fd);
        }
		builtin_status = builtin_exec(((t_cmd *)cmd)->args);
		if (builtin_status == NOT_FOUND)
			sys_exec(cmd);
		if (builtin_status != NOT_FOUND)
			exit(SUCCESS);
		exit(EXEC_FAILURE);
	}
	else
	{
		waitpid(pid, &status, 0);
		// close(input_fd);
		// close(output_fd);
		close(fd[1]);
	}
	if (WIFEXITED(status) != 0)
		singleton()->last_return_value = WEXITSTATUS(status);
	else if (WIFSIGNALED(status) == 1)
		singleton()->last_return_value = LRV_KILL_SIG + WTERMSIG(status);
	return (fd);
}

void interm_cmd(void *cmd, int *fd, int fd_index, t_list *lst_cmd)
{
	pid_t	pid;
	int 	status = 1;
	int 	builtin_status = 1;
	int 	output_fd;
    int 	input_fd;

    input_fd = -1;
	output_fd = -1;
	pid = fork();
	if (pid < 0)
		exit(PID_FAILURE);
	else if (pid == 0)
	{
		if (check_is_redir_cmd(lst_cmd))
		{
			cmd = get_complete_cmd(cmd, lst_cmd);
			redir_parser2(lst_cmd, &input_fd, &output_fd);
		}
		if (input_fd == -1)
			dup2(fd[fd_index * 2], STDIN_FILENO);
		else
			dup2(fd[fd_index * 2], input_fd);
		
		if (output_fd == -1)
			dup2(fd[(fd_index + 1) * 2 + 1], STDOUT_FILENO);
		else
			dup2(fd[(fd_index + 1) * 2 + 1], output_fd);

		builtin_status = builtin_exec(((t_cmd *)cmd)->args);
		if (builtin_status == NOT_FOUND)
			sys_exec(cmd);
		// need add free here because sys_exec can fail
		if (builtin_status != NOT_FOUND)
			exit(SUCCESS);
		exit(EXEC_FAILURE);
	}
	else
	{
		// wait(&status);
		waitpid(pid, &status, 0);
		close(fd[(fd_index + 1) * 2 + 1]);
	}
	if (WIFEXITED(status) != 0)
		singleton()->last_return_value = WEXITSTATUS(status);
	else if (WIFSIGNALED(status) == 1)
		singleton()->last_return_value = LRV_KILL_SIG + WTERMSIG(status);
}

void	last_cmd(void *cmd, int *fd, int fd_index, t_list *lst_cmd)
{
	pid_t	pid;
	int 	status = 1;
	int 	builtin_status = 1;
	int 	input_fd;
    int		output_fd;

    output_fd = -1;
	input_fd = -1;
	pid = fork();
	if (pid < 0)
			exit(PID_FAILURE);
	else if (pid == 0)
	{
		if (check_is_redir_cmd(lst_cmd))
		{
			cmd = get_complete_cmd(cmd, lst_cmd);
			redir_parser2(lst_cmd, &input_fd, &output_fd);
		}
		if (input_fd == -1)
			dup2(fd[fd_index * 2], STDIN_FILENO);
		else
			dup2(fd[fd_index * 2], input_fd);
			
		builtin_status = builtin_exec(((t_cmd *)cmd)->args);
		if (builtin_status == NOT_FOUND)
			sys_exec(cmd);
		if (builtin_status != NOT_FOUND)
			exit(SUCCESS);
		exit(EXEC_FAILURE);
	}
	else
	{
		// wait(&status);
		waitpid(pid, &status, 0);
	}
	if (WIFEXITED(status) != 0)
		singleton()->last_return_value = WEXITSTATUS(status);
	else if (WIFSIGNALED(status) == 1)
		singleton()->last_return_value = LRV_KILL_SIG + WTERMSIG(status);
}

void cmd_with_multi_flag(t_list *lst_cmd, int *fd)
{
	t_list 	*tmp;
	int		fd_index;

	tmp = lst_cmd;
	fd_index = 0;
	while (tmp && (is_redir(tmp) || flag_check(tmp) == FLG_PIPE))
	{
		if (!check_is_inter(tmp))
			break ;
		// show_content(tmp, "inter");
		interm_cmd(tmp->content, fd, fd_index, tmp);
        while (tmp && is_redir(tmp))
            tmp = tmp->next;
		++fd_index;
		tmp = tmp->next;
	}
	// show_content(tmp, "last");
	last_cmd(tmp->content, fd, fd_index, tmp);
}

static int	count_pipe_mix(t_list *lst_cmd)
{
	t_list *tmp;
	int count;

	tmp = lst_cmd;
	count = 0;
	while (tmp && (flag_check(tmp) != FLG_EO_CMD || flag_check(tmp) != FLG_EOL))
	{
		if (flag_check(tmp) == FLG_PIPE)
			++count;
		tmp = tmp->next;
	}
	return (count);
}

void all_in_one(t_list *lst_cmd)
{
	t_list 	*tmp;
	int 	*fd;
	int 	i;
	int 	pipe_len;

	tmp = lst_cmd;
    if (tmp && (flag_check(tmp) == FLG_EO_CMD || flag_check(tmp) == FLG_EOL))
    {
        simple_cmd(tmp->content);
        return ;
    }
    pipe_len = count_pipe_mix(tmp);
	fd = malloc(sizeof(int) * (pipe_len * 2));
	if (!fd)
		return ;
	create_fd(lst_cmd);
	i = 0;
	while (i < pipe_len)
		pipe(fd + (i++ * 2));
	first_cmd(tmp->content, fd, tmp, pipe_len);
    while (tmp && is_redir(tmp))
        tmp = tmp->next;
    if (pipe_len > 0)
	    cmd_with_multi_flag(tmp->next, fd);
	i = 0;
	while (i < pipe_len * 2)
		close(fd[i++]);
    if (fd)
	    free(fd);
}

void	exec_all_in_one(t_list *lst_cmd)
{
    if (!lst_cmd || !((t_cmd *)lst_cmd->content)->args)
        return ;
    all_in_one(lst_cmd);
}