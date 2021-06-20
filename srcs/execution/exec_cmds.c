/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   exec_cmds.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kaye <kaye@student.42.fr>                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/05/20 22:33:29 by besellem          #+#    #+#             */
/*   Updated: 2021/06/20 10:43:03 by kaye             ###   ########.fr       */
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

void 	unlink_all_tmp_fd(int i)
{
	char *old_name;
    char *new_name;
    char *fd_nbr;
	int fd;

	fd = -1;
    fd_nbr = ft_itoa(i);
    old_name = malloc(sizeof(char) * ft_strlen(TMP_FD) + 1);
    if (!old_name)
        return ;
    ft_strcpy(old_name, TMP_FD);
    new_name = ft_strjoin(old_name, fd_nbr);
    free(old_name);
    free(fd_nbr);
    fd = open(new_name, O_RDWR);
    if (fd != -1)
    {
        close(fd);
		unlink(new_name);
        free(new_name);
        unlink_all_tmp_fd(i++);
    }
	else
		return ;
}

int	ft_exec_each_cmd(t_list *lst_cmd)
{
	t_list	*tmp;
	int		cmd_line;
	int		built_exec;
	int i;

	tmp = lst_cmd;
	cmd_line = -1;
	built_exec = 0;
	i = 0;
	if (syntax_parser(tmp))
	{
		singleton()->last_return_value = LRV_SYNTAX_ERROR;
		return (0);
	}
	if (tmp)
		create_fd_input(tmp);
	while (tmp)
	{
		exec_all_in_one(tmp);
		while (tmp && flag_check(tmp) != FLG_EO_CMD)	// to remove -> "ls abcd; echo $?" case
			tmp = tmp->next;												// to remove -> "ls abcd; echo $?" case
		if (tmp)															// to remove -> "ls abcd; echo $?" case
			tmp = tmp->next;
	}
	unlink_all_tmp_fd(i);
	return (built_exec);
}
