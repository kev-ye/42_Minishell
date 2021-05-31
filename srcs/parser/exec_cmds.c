/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   exec_cmds.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kaye <kaye@student.42.fr>                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/05/20 22:33:29 by besellem          #+#    #+#             */
/*   Updated: 2021/05/31 18:08:44 by kaye             ###   ########.fr       */
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
	ft_putstr_fd("\n", STDERR_FILENO);
	print_prompt();
}

int ft_exec_builtin_cmd(char **cmds)
{
	static t_builtin	builtin[] = {
		{"echo", ft_echo, NULL}, {"cd", ft_cd, NULL}, {"pwd", NULL, ft_pwd},
		{"env", ft_env, NULL}, {"unset", ft_unset, NULL}, 
		{"export", ft_export, NULL}, {"exit", NULL, ft_exit},
		{"clear", NULL, ft_clear}, {NULL, NULL, NULL}
	};
	int					i;

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
	return (NOT_FOUND);
}

int	ft_exec_cmd(char *file, t_cmd *cmds)
{
	char	**env;
	int		ret = 0;

	env = ft_lst2strs(&singleton()->env);
	singleton()->last_return_value = execve(file, cmds->args, env);
	ft_memdel((void **)env);	// ne passe jamais ici
	return (ret);
	// if (-1 == ret)
	// 	return (ERROR);
	// else
	// 	return (ret);
}

// void	ft_pre_exec_cmd(void *ptr)
// {
// 	t_cmd	*cmd;
// 	char	*ex;
// 	int bl;

// 	cmd = ptr;
// 	if (!cmd->args || !*cmd->args)
// 		return ;
// 	ex = search_executable(cmd->args[0]);
// 	bl = ft_exec_builtin_cmd(char **cmds)
// 	if (bl)
// 	{
// 		ft_printf(B_RED "`%s' builtin command:\n" CLR_COLOR, bl);
// 		singleton()->last_return_value = ft_exec_builtin_cmd(cmd->args);
// 	}
// 	else if (ex)
// 	{
// 		ft_printf(B_RED "`%s' command:\n" CLR_COLOR, ex);
// 		singleton()->last_return_value = ft_exec_cmd(ex, cmd);
// 		ft_memdel((void **)&ex);
// 	}
// 	ft_strsfree(ft_strslen(cmd->args) + 1, cmd->args);
// }

void	ft_pre_exec_cmd(void *ptr)
{
	t_cmd	*cmd;
	char	*ex;

    cmd = ptr;
    if (!cmd->args || !cmd->args)
        return ;
    singleton()->last_return_value = ft_exec_builtin_cmd(cmd->args);
    if (singleton()->last_return_value == NOT_FOUND)
    {
		// printf("\n\n\n\n\n\nim here\n\n\n\n\n\n");
        ex = search_executable(cmd->args[0]);
        if (ex)
        {
            // ft_printf(B_RED "`%s' command:\n" CLR_COLOR, ex);
            singleton()->last_return_value = ft_exec_cmd(ex, cmd);
            ft_memdel((void **)&ex);
        }
        else
        {
            ft_dprintf(STDERR_FILENO, PROG_NAME ": %s: command not found\n",
                cmd->args[0]);
        }
    }
    ft_strsfree(ft_strslen(cmd->args) + 1, cmd->args);
}

/////////////////////////////////////////////////////////////////////////////////////////////// start pipe

int	*first_cmd_with_pipe(void *cmd)
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
	// char buff[1000];
	// int ret = read(fd[0], buff, 1000);
	// if (ret != -1)
	// 	buff[ret] = '\0';
	// printf("-> buff :\n%s ---- \n->ret : %d\n", buff, ret);
	return (fd);
}

int	*interm_cmd_with_pipe(void *cmd, int *get_fd)
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

void	last_cmd_with_pipe(void *cmd, int *get_fd)
{
	const pid_t	pid = fork();

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
}

void	simple_cmd(void *cmd)
{
	pid_t	pid;

	if (!ft_strcmp(((t_cmd *)cmd)->args[0], "cd"))
		ft_pre_exec_cmd(cmd);
	else
	{	
		pid = fork();
		if (pid < 0)
				exit(1);
		else if (pid == 0)
		{
			ft_pre_exec_cmd(cmd);
			exit(0);
		}
		else
			wait(NULL);
	}
}

/////////////////////////////////////////////////////////////////////////////////////////////// end pipe

/////////////////////////////////////////////////////////////////////////////////////////////// start direction

int	*first_cmd_with_redir(void *cmd)
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
	// char buff[1000];
	// int ret = read(fd[0], buff, 1000);
	// if (ret != -1)
	// 	buff[ret] = '\0';
	// printf("-> buff :\n%s ---- \n->ret : %d\n", buff, ret);
	return (fd);
}

// int	*interm_cmd_with_redir(void *cmd, int *get_fd)
// {
// 	pid_t	pid;
// 	int		*tmp = get_fd;
// 	int		*fd = malloc(sizeof(int) * 2);

// 	if (!fd)
// 		return (NULL);
// 	pipe(fd);
// 	pid = fork();
// 	if (pid < 0)
// 		exit(ERROR);
// 	else if (pid == 0)
// 	{
// 		close(get_fd[1]);
// 		dup2(get_fd[0], STDIN_FILENO);
// 		close(get_fd[0]);
	
// 		close(fd[0]);
// 		dup2(fd[1], STDOUT_FILENO);
// 		close(fd[1]);
// 		exit(0);
// 	}
// 	else
// 	{
// 		close(fd[1]);
// 		wait(NULL);
// 	}
// 	free(tmp);
// 	return (fd);
// }

void	last_cmd_with_redir(void *cmd, int *get_fd)
{
	const pid_t	pid = fork();
	int write_fd;
	char *test = "lol";

	//
	(void)cmd;
	(void)get_fd;
	//

	write_fd = -1;
	if (pid < 0)
		exit(ERROR);
	else if (0 == pid)
	{
		write_fd = open(((t_cmd *)cmd)->args[0] , O_WRONLY | O_CREAT, 0666);
		if (write_fd == -1)
		{
			printf("write fd down\n");
			exit(0);
		}
		// close(get_fd[1]);
		// dup2(write_fd, get_fd[0]);
		write(write_fd, test, strlen(test));
		// close(get_fd[0]);
		exit(0);


		// int fd = open("file" , O_WRONLY | O_TRUNC |O_CREAT, 0666);
		// if (fd == -1)
		// {
		// 	printf("write fd down\n");
		// 	exit(0);
		// }
		// write(fd, test, strlen(test));
		// exit(0);
	}
	else
	{
		// close(get_fd[1]);
		wait(NULL);
	}
}

/////////////////////////////////////////////////////////////////////////////////////////////// end direction

void	ft_exec_each_cmd(t_list *lst)
{
	t_list	*tmp;
	int		first;
	int		redir_flag;
	int		*fd;
	// int fd;
	
	if (!lst || !((t_cmd *)lst->content)->args)
		return ;
	tmp = lst;
	first = 1;
	redir_flag = 0;
	fd = NULL;
	while (tmp)
	{
		if ((((t_cmd *)tmp->content)->status_flag & FLG_OUTPUT) && first == 1)
		{
			printf("fist with redir\n");
			fd = first_cmd_with_redir(tmp->content);
			first = 0;
			redir_flag = 1;
		}
		else if ((((t_cmd *)tmp->content)->status_flag & FLG_EOL) && redir_flag == 1)
		{
			printf("last with redir\n");
			last_cmd_with_redir(tmp->content, fd);
		}
		else if (((t_cmd *)tmp->content)->args)
		{
			printf("simple\n");
			simple_cmd(tmp->content);
		}
		tmp = tmp->next;
	}
}

// void	ft_exec_each_cmd(t_list *lst)
// {
// 	t_list	*tmp;
// 	int		first;
// 	int		pipe_flag;
// 	int		*fd;
	
// 	if (!lst || !((t_cmd *)lst->content)->args)
// 		return ;
// 	tmp = lst;
// 	first = 1;
// 	pipe_flag = 0;
// 	fd = NULL;
// 	while (tmp)
// 	{
// 		if ((((t_cmd *)tmp->content)->status_flag & FLG_PIPE) && first == 1)
// 		{
// 			printf("fist with pipe\n");
// 			fd = first_cmd_with_pipe(tmp->content);
// 			first = 0;
// 			pipe_flag = 1;
// 		}
// 		else if ((((t_cmd *)tmp->content)->status_flag & FLG_PIPE) && first == 0)
// 		{
// 			printf("interm with pipe\n");
// 			fd = interm_cmd_with_pipe(tmp->content, fd);
// 			pipe_flag = 1;
// 		}
// 		else if ((((t_cmd *)tmp->content)->status_flag & FLG_EO_CMD) && first == 0 && pipe_flag == 1)
// 		{
// 			printf("with ;\n");
// 			last_cmd_with_pipe(tmp->content, fd);
// 			pipe_flag = 0;
// 		}
// 		else if ((((t_cmd *)tmp->content)->status_flag & FLG_EOL) && pipe_flag == 1)
// 		{
// 			printf("last with pipe\n");
// 			last_cmd_with_pipe(tmp->content, fd);
// 		}
// 		else if (((t_cmd *)tmp->content)->args)
// 		{
// 			printf("simple\n");
// 			simple_cmd(tmp->content);
// 		}
// 		tmp = tmp->next;
// 	}
// }

// void	ft_exec_each_cmd(void)
// {
// 	pid_t	id;
// 	id = fork();
// 	if (id < 0)
// 		exit(ERROR);
// 	if (id == 0)
// 	{
// 		multi_cmd_exec(singleton()->lst);
// 	}
// 	else
// 		wait(NULL);
// }