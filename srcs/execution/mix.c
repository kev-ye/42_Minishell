/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   mix.c                                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kaye <kaye@student.42.fr>                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/06/10 15:39:17 by kaye              #+#    #+#             */
/*   Updated: 2021/06/16 18:17:06 by kaye             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

void show_content(t_list *lst_cmd, char *msg)
{
	if (msg)
		printf("%s\n", msg);
	printf("arg[0] : -> %s\n", ((t_cmd *)lst_cmd->content)->args[0]);
}

void	show_fd(int fd, char *msg)
{
	char *line = NULL;
	int r = 1;

	if (msg)
		printf("%s\n", msg);
	while (r)
	{
		r = get_next_line(fd, &line);
		printf("> %s\n", line);
		free(line);
		// break ;
	}
}

////////////////////////////////////////////////////////////////////////////////////

void	*first_cmd_with_pipe_mix(void *cmd, int *fd, t_list *lst_cmd)
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
        cmd = get_complete_cmd(cmd, lst_cmd);
		redir_parser2(lst_cmd, &input_fd, &output_fd);
	
		if (output_fd == -1)
			dup2(fd[1], STDOUT_FILENO);
		else
			dup2(fd[1], output_fd);

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
		close(input_fd);
		close(output_fd);
		close(fd[1]);           // get stdout, need to close, because if not, stdout is always open, so the fd for stdin never have EOF
	}
	if (WIFEXITED(status) != 0)
		singleton()->last_return_value = WEXITSTATUS(status);
	else if (WIFSIGNALED(status) == 1)
		singleton()->last_return_value = LRV_KILL_SIG + WTERMSIG(status);
	return (fd);
}

void interm_cmd_with_pipe_mix(void *cmd, int *fd, int fd_index, t_list *lst_cmd)
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
        cmd = get_complete_cmd(cmd, lst_cmd);
		redir_parser2(lst_cmd, &input_fd, &output_fd);

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

void	last_cmd_with_pipe_mix(void *cmd, int *fd, int fd_index, t_list *lst_cmd)
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
        cmd = get_complete_cmd(cmd, lst_cmd);
		redir_parser2(lst_cmd, &input_fd, &output_fd);
    
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

int check_is_inter(t_list *lst_cmd)
{
	while (lst_cmd && is_redir(lst_cmd))
		lst_cmd = lst_cmd->next;
	if (flag_check(lst_cmd) == FLG_EO_CMD || flag_check(lst_cmd) == FLG_EOL)
		return (0);
	return (1);
}

void cmd_with_multi_pipe_mix(t_list *lst_cmd, int *fd)
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
		interm_cmd_with_pipe_mix(tmp->content, fd, fd_index, tmp);
        while (tmp && is_redir(tmp))
            tmp = tmp->next;
		++fd_index;
		tmp = tmp->next;
	}
	// show_content(tmp, "last");
	last_cmd_with_pipe_mix(tmp->content, fd, fd_index, tmp);
}

int	count_pipe_mix(t_list *lst_cmd)
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

void cmd_with_pipe_mix(t_list *lst_cmd)
{
	t_list 	*tmp;
	int 	*fd;
	int 	i;
	int 	pipe_len;

	tmp = lst_cmd;
	pipe_len = count_pipe_mix(tmp);
	fd = malloc(sizeof(int) * (pipe_len * 2));
	if (!fd)
		return ;
	
	// create fd
	create_fd(lst_cmd);

	// create pipe for each cmd
	i = 0;
	while (i < pipe_len)
		pipe(fd + (i++ * 2));

	// lauch the first cmd with pipe
	// show_content(tmp, "first");
	first_cmd_with_pipe_mix(tmp->content, fd, tmp);
    while (tmp && is_redir(tmp))
        tmp = tmp->next;

	// multi cmd with pipe
	cmd_with_multi_pipe_mix(tmp->next, fd);

	// close all pipe fd
	i = 0;
	while (i < pipe_len * 2)
		close(fd[i++]);
	free(fd);
}

void	cmd_with_mix(t_list *lst_cmd)
{
    if (!lst_cmd || !((t_cmd *)lst_cmd->content)->args)
        return ;
    cmd_with_pipe_mix(lst_cmd);
}