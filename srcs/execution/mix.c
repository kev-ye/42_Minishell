/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   mix.c                                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kaye <kaye@student.42.fr>                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/06/10 15:39:17 by kaye              #+#    #+#             */
/*   Updated: 2021/06/10 19:27:38 by kaye             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

int     check_last_cmd_flag(t_list *lst_cmd)
{
    t_list *tmp;

    tmp = lst_cmd;
    while (tmp && flag_check(tmp) == FLG_PIPE)
        tmp = tmp->next;
    if (!(((t_cmd *)tmp->content)->status_flag & FLG_EO_CMD)
        || !(((t_cmd *)tmp->content)->status_flag & FLG_EOL))
        return (REDIR_OPEN);
    while (tmp && is_redir(tmp))
        tmp = tmp->next;
    if (!(((t_cmd *)tmp->content)->status_flag & FLG_EO_CMD)
        || !(((t_cmd *)tmp->content)->status_flag & FLG_EOL))
        return (PIPE_OPEN);
    return (NO_OPTION);
}

int cmd_with_multi_pipe_mix(t_list *lst_cmd, int *fd, int option)
{
	t_list 	*tmp;
	int		fd_index;
    int     fd_ret;

	tmp = lst_cmd;
	fd_index = 0;
    fd_ret = -1;
	while (tmp && flag_check(tmp) == FLG_PIPE)
	{
		interm_cmd_with_pipe(tmp->content, fd, fd_index);
		++fd_index;
		tmp = tmp->next;
	}
	if (is_redir(tmp) && option == REDIR_OPEN)
        fd_ret = fd[fd_index * 2];
	else
		last_cmd_with_pipe(tmp->content, fd, fd_index);
    return (fd_ret);
}

int cmd_with_pipe_mix(t_list *lst_cmd, int first, int option, int *fd)
{
    int fd_ret;

    fd_ret = -1;

	// lauch the first cmd with pipe
	if (first == FIRST)
		first_cmd_with_pipe(lst_cmd->content, fd);

	// multi cmd with pipe
	fd_ret = cmd_with_multi_pipe_mix(lst_cmd->next, fd, option);

    return (fd_ret);
}

void	cmd_with_redir_mix(void *cmd, t_list *lst_cmd, int input_fd)
{
	pid_t	pid;
	int		output_fd;
	// int 	input_fd;
	int 	tmp_errno;
	int 	status = 1;
	int 	builtin_status = 1;

	// input_fd = -1;
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
    int 	*fd = NULL;
	int 	i = 0;
	int 	pipe_len = 0;

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
            pipe_len = count_pipe(lst_cmd);
	        fd = malloc(sizeof(int) * (pipe_len * 2));
	        if (!fd)
		        return ;
            while (i < pipe_len)
		        pipe(fd + (i++ * 2));

            input_fd = cmd_with_pipe_mix(tmp, first, option, fd);
            while (flag_check(tmp) == FLG_PIPE)
                tmp = tmp->next;
            first = NO_FIRST;
        }
        else
        {
            cmd_with_redir_mix(tmp->content, tmp, input_fd); // -> next step
            while (is_redir(tmp))
                tmp = tmp->next; 
            first = NO_FIRST;
            i = 0;
	        while (i < pipe_len * 2)
	        	close(fd[i++]);
	        free(fd);
        }
        if ((((t_cmd *)tmp->content)->status_flag & FLG_EO_CMD)
            || (((t_cmd *)tmp->content)->status_flag & FLG_EOL))
            return ;
    }
}