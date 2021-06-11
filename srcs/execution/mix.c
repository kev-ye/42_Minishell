/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   mix.c                                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kaye <kaye@student.42.fr>                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/06/10 15:39:17 by kaye              #+#    #+#             */
/*   Updated: 2021/06/11 17:07:50 by kaye             ###   ########.fr       */
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
		break ;
	}
}

////////////////////////////////////////////////////////////////////////////////////

int     check_last_cmd_flag(t_list *lst_cmd)
{
    t_list *tmp;

    tmp = lst_cmd;
	if (tmp && flag_check(tmp) == FLG_PIPE)
	{
    	while (tmp && flag_check(tmp) == FLG_PIPE)
       		tmp = tmp->next;
    	if ((((t_cmd *)tmp->content)->status_flag & FLG_EO_CMD)
        	|| (((t_cmd *)tmp->content)->status_flag & FLG_EOL))
        	return (NO_OPTION);
		else
			return (REDIR_OPEN);
	}
	else if (tmp && is_redir(tmp))
	{
		while (tmp && is_redir(tmp))
			tmp = tmp->next;
		if ((((t_cmd *)tmp->content)->status_flag & FLG_EO_CMD)
			|| (((t_cmd *)tmp->content)->status_flag & FLG_EOL))
			return (NO_OPTION);
		else
			return (PIPE_OPEN);
	}
    return (NO_OPTION);
}

void interm_cmd_with_pipe_mix(void *cmd, int *fd, int fd_index, int next_option, int *redir_fd)
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
		if (next_option)
		{
			dup2(redir_fd[0], STDIN_FILENO);
		}
		else
		{
			dup2(fd[fd_index * 2], STDIN_FILENO);
		}
		if (!next_option)
			dup2(fd[(fd_index + 1) * 2 + 1], STDOUT_FILENO);
		else
		{
			dup2(fd[1], STDOUT_FILENO);
		}
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
		if (!next_option)
			close(fd[(fd_index + 1) * 2 + 1]);
		else
			close(fd[1]);
	}
	if (WIFEXITED(status) != 0)
		singleton()->last_return_value = WEXITSTATUS(status);
	else if (WIFSIGNALED(status) == 1)
		singleton()->last_return_value = LRV_KILL_SIG + WTERMSIG(status);
}

void	last_cmd_with_pipe_mix(void *cmd, int *fd, int fd_index, int next_option, int *redir_fd)
{
	(void)fd_index;
	pid_t	pid;
	int 	status = 1;
	int 	builtin_status = 1;

	pid = fork();
	if (pid < 0)
			exit(PID_FAILURE);
	else if (pid == 0)
	{
		// printf("\n---> last cmd go in\n\n");
		if (next_option)
		{
			dup2(redir_fd[0], STDIN_FILENO);
		}
		else
		{
			dup2(fd[0], STDIN_FILENO);
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
		// wait(&status);
		waitpid(pid, &status, 0);
	}
	if (WIFEXITED(status) != 0)
		singleton()->last_return_value = WEXITSTATUS(status);
	else if (WIFSIGNALED(status) == 1)
		singleton()->last_return_value = LRV_KILL_SIG + WTERMSIG(status);
}

int cmd_with_multi_pipe_mix(t_list *lst_cmd, int *fd, int option, int next_option, int *redir_fd)
{
	t_list 	*tmp;
	int		fd_index;
    int     fd_ret;

	tmp = lst_cmd;
	fd_index = 0;
    fd_ret = -1;
	while (tmp && flag_check(tmp) == FLG_PIPE)
	{
		interm_cmd_with_pipe_mix(tmp->content, fd, fd_index, next_option, redir_fd);
		if (!next_option)
			++fd_index;
		tmp = tmp->next;
		if (next_option)
			next_option = 0;
	}
	if (is_redir(tmp) && option == REDIR_OPEN)
        fd_ret = fd[fd_index * 2];
	else
	{
		last_cmd_with_pipe_mix(tmp->content, fd, fd_index, next_option, redir_fd);
	}
	return (fd_ret);
}

int cmd_with_pipe_mix(t_list *lst_cmd, int first, int option, int *pipe_fd, int next_option, int *redir_fd)
{
    int fd_ret;

    fd_ret = -1;

	// lauch the first cmd with pipe
	if (first == FIRST)
		first_cmd_with_pipe(lst_cmd->content, pipe_fd);

	// multi cmd with pipe
	fd_ret = cmd_with_multi_pipe_mix(lst_cmd->next, pipe_fd, option, next_option, redir_fd);

    return (fd_ret);
}

void cmd_with_redir_mix(void *cmd, t_list *lst_cmd, int input_fd, int **redir_fd, int option)
{
	pid_t	pid;
	int		output_fd;
	int 	tmp_errno;
	int 	status = 1;
	int 	builtin_status = 1;

	output_fd = -1;
	tmp_errno = 0;
	pid = fork();
	if (pid < 0)
			exit(1);
	else if (pid == 0)
	{
		dup2(input_fd, STDIN_FILENO);
		
		cmd = get_complete_cmd(cmd, lst_cmd);
		redir_parser(input_fd, output_fd, lst_cmd);

		// printf("option [%d] == PIPE_OPEN [%d]\n", option, PIPE_OPEN);
		if (option == PIPE_OPEN)
			dup2((*redir_fd)[1], STDOUT_FILENO);
		close((*redir_fd)[1]);

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
		close((*redir_fd)[1]);

		// show_fd((*redir_fd)[0], "in redir mix");
	}
	if (WIFEXITED(status) != 0)
		singleton()->last_return_value = WEXITSTATUS(status);
	else if (WIFSIGNALED(status) == 1)
		singleton()->last_return_value = LRV_KILL_SIG + WTERMSIG(status);
}

void	cmd_with_mix(t_list *lst_cmd)
{
    // general
    t_list *tmp;
    int option;
    int first;
    int input_fd;

    // pipe
    int 	*pipe_fd = NULL;
	int 	i = 0;
	int 	pipe_len = 0;

	// preparation
	int next_is_pipe = 0;
	int next_is_redir = 0;

	// redir
	int *redir_fd = NULL;

    tmp = lst_cmd;
    option = NO_OPTION;
    first = FIRST;
    input_fd = -1;
    while (tmp)
    {
        // check option
        option = check_last_cmd_flag(tmp);
        if (flag_check(tmp) == FLG_PIPE)
        {
			if (next_is_pipe)
			{
            	pipe_len = count_pipe(tmp->next);
			}
			else
				pipe_len = count_pipe(tmp);
			if (pipe_len > 0)
			{
	        	pipe_fd = malloc(sizeof(int) * (pipe_len * 2));
				if (!pipe_fd)
					return ;
			}
			i = 0;
            while (i < pipe_len)
			{
		        pipe(pipe_fd + (i++ * 2));
			}

            input_fd = cmd_with_pipe_mix(tmp, first, option, pipe_fd, next_is_pipe, redir_fd);
            while (flag_check(tmp) == FLG_PIPE)
                tmp = tmp->next;
            first = NO_FIRST;

			if (option == REDIR_OPEN)
			{
				next_is_redir = 1;
				next_is_pipe = 0;
			}
			else
			{
				next_is_pipe = 0;
				next_is_redir = 0;
			}
        }
        else if (is_redir(tmp))
        {
			redir_fd = malloc(sizeof(int) * 2);
	        if (!redir_fd)
		        return ; 	
			pipe(redir_fd);		

            cmd_with_redir_mix(tmp->content, tmp, input_fd, &redir_fd, option);
            while (is_redir(tmp))
                tmp = tmp->next; 
            first = NO_FIRST;
            i = 0;
	        while (i < pipe_len * 2)
	        	close(pipe_fd[i++]);
			if (pipe_fd)
	        	free(pipe_fd);

			if (option == PIPE_OPEN)
			{
				next_is_pipe = 1;
				next_is_redir = 0;
			}
			else
			{
				next_is_pipe = 0;
				next_is_redir = 0;
			}
			
        }
        if ((((t_cmd *)tmp->content)->status_flag & FLG_EO_CMD)
            || (((t_cmd *)tmp->content)->status_flag & FLG_EOL))
            return ;
    }
}