/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   exec_cmds.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: besellem <besellem@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/05/20 22:33:29 by besellem          #+#    #+#             */
/*   Updated: 2021/05/27 13:32:25 by besellem         ###   ########.fr       */
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
	// kill(singleton()->thread_pid, code);	
	// exit(code);
	(void)code;
	// print_prompt();
	ft_putstr_fd("\n", STDERR_FILENO);
	print_prompt();
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

int	ft_exec_cmd(char *file, t_cmd *cmds)
{
	const pid_t	id = fork();

	if (id == 0)
	{
		// (cmds->status_flag & FLG_PIPE)
		// signal(SIGQUIT, ft_quit);
		// signal(SIGINT, ft_interrupt);
		// signal(SIGKILL, ft_interrupt);
		return (execve(file, cmds->args, ft_lst2strs(&singleton()->env)));
	}
	else
	{
		wait(NULL);
	}
	return (ERROR);
}

// int	ft_exec_cmd(char *file, t_cmd *cmds)
// {
// 	int i = execve(file, cmds->args, NULL);
// 	if (i == -1)
// 		return (ERROR);
// 	else
// 		return (i);
// }


// char	**ft_lst_to_strs(t_list *lst)
// {
// 	char	**s;
// 	t_list	*tmp;
// 	size_t	i;

// 	if (!lst)
// 		return (NULL);
// 	s = ft_calloc(ft_lstsize(lst) + 1, sizeof(char *));
// 	if (!s)
// 		return (NULL);
// 	tmp = lst;
// 	i = 0;
// 	while (tmp)
// 	{
// 		s[i] = ft_calloc(ft_strlen(tmp->content) + 1, sizeof(char));
// 		if (!s[i])
// 			return (ft_strsfree(i, s));
// 		ft_memcpy(s[i++], tmp->content, ft_strlen(tmp->content));
// 		tmp = tmp->next;
// 	}
// 	s[i] = NULL;
// 	return (s);
// }

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
		// if (singleton()->last_return_value == ERROR)
		// 	exit(1);
		// if (singleton()->last_return_value == ERROR)
		// 	ft_dprintf(STDERR_FILENO, "%s: %s: %s\n", PROG_NAME, cmd->args[0], strerror(errno));
	}
	else if (ex)
	{
		ft_printf(B_RED "`%s' command:\n" CLR_COLOR, ex);
		singleton()->last_return_value = ft_exec_cmd(ex, cmd);
		// if (singleton()->last_return_value == ERROR)
		// 	exit(1);
	}
	else
	{
		ft_dprintf(STDERR_FILENO, PROG_NAME ": %s: command not found\n", cmd->args[0]);
	}
	if (ex)
		ft_memdel((void **)&ex);
	ft_strsfree(ft_strslen(cmd->args) + 1, cmd->args);
}

// void	ft_lstiter_replace(t_list *lst, void *(*f)(void *))
// {
// 	t_list	*tmp;

// 	tmp = lst;
// 	while (tmp)
// 	{
// 		if ((*f))
// 			tmp->content = (*f)(tmp->content);
// 		tmp = tmp->next;
// 	}
// }

void set_io(int read_fd, int write_fd)
{
	if (read_fd != STDIN_FILENO)
	{
        dup2(read_fd, STDIN_FILENO);
        close(read_fd);
    }
    if (write_fd != STDOUT_FILENO)
	{
        dup2(write_fd, STDOUT_FILENO);
        close(write_fd);
	}
}

// void	multi_cmd_exec(t_list *lst)
// {
// 	pid_t	id;
// 	int fd[2];
// 	int read_fd;
// 	int write_fd;

// 	pid_t id2 = fork();
// 	if (id2 < 0)
// 		exit(ERROR);
// 	else if (id2 == 0)
// 	{
// 		if (lst && !lst->next)
// 		{
// 			set_io(STDIN_FILENO, STDOUT_FILENO);
// 			ft_pre_exec_cmd(lst->content);
// 			return ;
// 		}
// 		id = fork();
// 		pipe(fd);
// 		read_fd = fd[0];
// 		write_fd = fd[1];
// 		if (id < 0)
// 			exit(ERROR);
// 		if (id == 0)
// 		{
// 			close(read_fd);
// 			set_io(STDIN_FILENO, write_fd);
// 			ft_pre_exec_cmd(lst->content);
// 			exit(1);
// 		}
// 		else
// 		{
// 			wait(NULL);
// 			lst = lst->next;
// 			close(write_fd);
// 			set_io(read_fd, STDOUT_FILENO);  
// 			multi_cmd_exec(lst);
// 		}
// 	}
// 	else
// 	{
// 		wait(NULL);
// 	}
// }

void	ft_exec_each_cmd(void)
{
	// pid_t	id;
	// id = fork();
	// if (id < 0)
	// 	exit(ERROR);
	// if (id == 0)
	// {
		// multi_cmd_exec(singleton()->lst);
	// }
	// else
		// wait(NULL);
	ft_lstiter(singleton()->lst, ft_pre_exec_cmd);
	// ft_lstprint_cmd(singleton()->lst, '\n');
}
