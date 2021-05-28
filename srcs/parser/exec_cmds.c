/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   exec_cmds.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kaye <kaye@student.42.fr>                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/05/20 22:33:29 by besellem          #+#    #+#             */
/*   Updated: 2021/05/28 20:45:35 by kaye             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

// seems that it must be used on forked commands - not on the main program
void	ft_quit(int code)
{
	ft_dprintf(STDERR_FILENO, B_RED "SIGNAL: Quit: %d\n" CLR_COLOR, code);
	// exit(code);
}

void	ft_interrupt(int code)
{
	ft_dprintf(STDERR_FILENO, B_RED "SIGNAL: Interrupt[%d]\n" CLR_COLOR, code);
	// kill(singleton()->thread_pid, code);
	
	// exit(code);
}

//////////////////////////////////////////////////kaye
int ft_exec_builtin_cmd(char **cmds)
{
	const t_builtin builtin[] = {{"echo", ft_echo, NULL}, 
				{"cd", ft_cd, NULL}, {"pwd", NULL, ft_pwd}, 
				{"env", ft_env, NULL}, {"unset", ft_unset, NULL}, 
				{"export", ft_export, NULL}, {"exit", NULL, ft_exit}, {NULL, NULL, NULL}};
	int i;

	i = 0;
	while (builtin[i].cmd)
	{
		if (!ft_strcmp(cmds[0], builtin[i].cmd))
		{
			if (!builtin[i].f1)
				return (!builtin[i].f2());
			else
				return (!builtin[i].f1(cmds));
		}
		++i;
	}
	return (ERROR);
}
//////////////////////////////////////////////////////

// int	ft_exec_cmd(char *file, t_cmd *cmds)
// {
// 	const pid_t	id = fork();

// 	if (id == 0)
// 	{
// 		return (execve(file, cmds->args, ft_lst2strs(&singleton()->env)));
// 	}
// 	else
// 	{
// 		wait(NULL);
// 	}
// 	return (ERROR);
// }

int	ft_exec_cmd(char *file, t_cmd *cmds)
{
	int i = execve(file, cmds->args, NULL);
	if (i == -1)
		return (ERROR);
	else
		return (i);
}

void	ft_pre_exec_cmd(void *ptr)
{
	t_cmd	*cmd;
	char	*ex;
	char	*bl;

	cmd = ptr;
	if (!cmd->args || !*cmd->args)
		return ;
	ex = search_executable(cmd->args[0]);
	bl = search_builtin_executable(cmd->args[0]);
	if (bl)
	{
		ft_printf(B_RED "`%s' builtin command:\n" CLR_COLOR, bl);
		singleton()->last_return_value = ft_exec_builtin_cmd(cmd->args);
	}
	else if (ex)
	{
		ft_printf(B_RED "`%s' command:\n" CLR_COLOR, ex);
		singleton()->last_return_value = ft_exec_cmd(ex, cmd);
		ft_memdel((void **)&ex);
	}
	ft_strsfree(ft_strslen(cmd->args) + 1, cmd->args);
}

void	set_io(int read_fd, int write_fd)
{
	if(read_fd != STDIN_FILENO)
	{
		dup2(read_fd, STDIN_FILENO);
		close(read_fd);
	}
	if(write_fd != STDOUT_FILENO)
	{
		dup2(write_fd, STDOUT_FILENO);
		close(write_fd);
	}
}

// void	simple_cmd_exec(t_list *lst)
// {
// 	const pid_t pid = fork();

// 	if (pid < 0)
// 		exit(ERROR);
// 	else if (pid == 0)
// 	{
// 		ft_pre_exec_cmd(lst->content);
// 		exit(1);
// 	}
// 	else
// 		wait(NULL);
// }

void	multi_cmd_exec(t_list *lst)
{
	pid_t	pid;
	int fd[2];

	if (lst && !lst->next)
	{
		set_io(STDIN_FILENO, STDOUT_FILENO);
		ft_pre_exec_cmd(lst->content);
		exit(1);
	}
	pipe(fd);
	pid = fork();
	if (pid < 0)
		exit(ERROR);
	else if (pid == 0)
	{
		close(fd[0]);
		set_io(STDIN_FILENO, fd[1]);
		ft_pre_exec_cmd(lst->content);
		exit(1);
	}
	else
	{
		wait(NULL);
		lst = lst->next;
		close(fd[1]);
		set_io(fd[0], STDOUT_FILENO);
		multi_cmd_exec(lst);
	}
}

// void	multi_cmd_exec(t_list *lst)
// {
// 	pid_t	pid;
// 	int fd[2];
// 	int read_fd;
// 	int write_fd;

// 	read_fd = fd[0];
// 	write_fd = fd[1];
// 	pipe(fd);
// 	for (; lst != NULL; lst = lst->next)
// 	{
// 		pid = fork();
// 		if (pid < 0)
// 			exit(ERROR);
// 		else if (pid == 0)
// 		{
// 			if (((t_cmd *)lst->content)->status_flag & FLG_PIPE)
// 			{
// 				close(read_fd);
// 				dup2(write_fd, STDOUT_FILENO);
// 			}
// 			ft_pre_exec_cmd(lst->content);
// 			exit(1);
// 		}
// 		else
// 		{
// 			wait(NULL);
// 			close(write_fd);
// 			dup2(read_fd, STDIN_FILENO);
// 		}
// 		// lst = lst->next;
// 	}
// }

void	ft_exec_each_cmd(void)
{
	pid_t	id;
	id = fork();
	if (id < 0)
		exit(ERROR);
	if (id == 0)
	{
		multi_cmd_exec(singleton()->lst);
	}
	else
		wait(NULL);
	// ft_lstiter(singleton()->lst, ft_pre_exec_cmd);
	// ft_lstprint_cmd(singleton()->lst, '\n');
}
