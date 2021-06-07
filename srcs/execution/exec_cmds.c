/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   exec_cmds.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kaye <kaye@student.42.fr>                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/05/20 22:33:29 by besellem          #+#    #+#             */
/*   Updated: 2021/06/07 18:19:27 by kaye             ###   ########.fr       */
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

/*
** Old exec_cmd
*/

// int	ft_exec_cmd(char *file, t_cmd *cmds)
// {
// 	char	**env;
// 	int		ret = 0;

// 	env = ft_lst2strs(&singleton()->env);
// 	singleton()->last_return_value = execve(file, cmds->args, env);
// 	ft_memdel((void **)env);	// ne passe jamais ici
// 	return (ret);
// }

// void	ft_pre_exec_cmd(void *ptr)
// {
// 	t_cmd	*cmd;
// 	char	*ex;

// 	cmd = ptr;
// 	if (!cmd->args || !cmd->args)
// 		return ;
// 	singleton()->last_return_value = ft_exec_builtin_cmd(cmd->args);
// 	if (singleton()->last_return_value == NOT_FOUND)
// 	{
// 		ex = search_executable(cmd->args[0]);
// 		if (ex)
// 		{
// 			singleton()->last_return_value = ft_exec_cmd(ex, cmd);
// 			ft_memdel((void **)&ex);
// 		}
// 		else
// 		{
// 			ft_dprintf(STDERR_FILENO, PROG_NAME ": %s: command not found\n",
// 				cmd->args[0]);
// 		}
// 	}
// 	ft_strsfree(ft_strslen(cmd->args) + 1, cmd->args);
// }

/*
** new exec_cmd
*/

void	ft_exec_cmd(char *file, t_cmd *cmds)
{
	char	**env;
	int		ret = 0;

	env = ft_lst2strs(&singleton()->env);
	ret = execve(file, cmds->args, env);
	printf("msg check\n");
	// prog exit here

	ft_memdel((void **)env);	// ne passe jamais ici              //by kaye : psk quand execve lance , le prog exit
}

void	ft_pre_exec_cmd(void *ptr)
{
	t_cmd	*cmd;
	char	*ex;
	int 	not_found;
	int 	builtin;

	cmd = ptr;
	not_found = 0;
	builtin = NOT_FOUND;
	if (!cmd->args || !cmd->args)
		return ;
	builtin = ft_exec_builtin_cmd(cmd->args);
	if (builtin == NOT_FOUND)
	{
		ex = search_executable(cmd->args[0]);
		if (ex)
		{
			ft_exec_cmd(ex, cmd);
			ft_memdel((void **)&ex);
		}
		else
		{
			ft_dprintf(STDERR_FILENO, PROG_NAME ": %s: command not found\n",
				cmd->args[0]);
			not_found = 1;
		}
	}
	ft_strsfree(ft_strslen(cmd->args) + 1, cmd->args);
	if (builtin != NOT_FOUND)
		exit(SUCCESS);
	if (not_found == 1)
		exit(LRV_CMD_NOT_FOUND);
}

//////////////////////////////////////////////////////////////////////////////////////////////////////// exec
//// kaye ///////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////
//// last time here -> echo > f1 ; ls ; ls | grep incs | wc ; ls -> seg
//// and have file "wc" created
/////////////////////////////////////////////////////////////////////////////////////////////////////////////

void	simple_cmd(void *cmd)
{
	pid_t	pid;
	int status;

	status = 1;
	if (!ft_strcmp(((t_cmd *)cmd)->args[0], "cd") || !ft_strcmp(((t_cmd *)cmd)->args[0], "exit"))
		ft_pre_exec_cmd(cmd);
	else
	{	
		pid = fork();
		if (pid < 0)
				exit(PID_FAILURE);
		else if (pid == 0)
		{
			ft_pre_exec_cmd(cmd);
			exit(EXEC_FAILURE);
		}
		else
			wait(&status);
		if (WIFEXITED(status) != 0)
			singleton()->last_return_value = WEXITSTATUS(status);
		else if (WIFSIGNALED(status) == 1)
			singleton()->last_return_value = LRV_SIGINT;
	}
}

void	ft_exec_each_cmd(t_list *lst_cmd)
{
	t_list	*tmp;
	int		cmd_line;

	tmp = lst_cmd;
	cmd_line = -1;
	if (syntax_parser(tmp))
	{
		singleton()->last_return_value = LRV_SYNTAX_ERROR;
		return ;
	}
	while (tmp)
	{
		if (((t_cmd *)tmp->content)->args
			&& ((((t_cmd *)tmp->content)->status_flag & FLG_EO_CMD)
			|| (((t_cmd *)tmp->content)->status_flag & FLG_EOL)))
		{
			printf(B_PURPLE"simple cmd"CLR_COLOR"\n");
			simple_cmd(tmp->content);
		}
		else
		{
			cmd_line = part_cmd_check(tmp);
			if (cmd_line == ONLY_PIPE)
			{
				printf(B_PURPLE"pipe cmd"CLR_COLOR"\n");
				cmd_with_pipe(tmp);
			}
			else if (cmd_line == ONLY_REDIR)
			{
				printf(B_PURPLE"redir cmd"CLR_COLOR"\n");
				cmd_with_redir(tmp->content, tmp);
			}
			else if (cmd_line == MIX)
				printf("mix\n");
		}
		while (tmp && !(((t_cmd *)tmp->content)->status_flag & FLG_EO_CMD))
			tmp = tmp->next;
		if (tmp)
			tmp = tmp->next;
	}
}

/*
** old exec
*/

// void	ft_exec_each_cmd(t_list *lst)
// {
// 	t_list	*tmp;
// 	int		first;
// 	int		redir_flag;
// 	int 	pipe_flag;
// 	int		*fd;
	
// 	// if (!lst || !((t_cmd *)lst->content)->args)
// 	// 	return ;
// 	if (!lst)
// 		return ;
// 	tmp = lst;
// 	first = 1;
// 	redir_flag = 0;
// 	pipe_flag = 0;
// 	fd = NULL;
// 	while (tmp)
// 	{
// 		if (((t_cmd *)tmp->content)->args && (((t_cmd *)tmp->content)->status_flag & FLG_PIPE) && first == 1)
// 		{
// 			printf("fist with pipe\n");
// 			fd = first_cmd_with_pipe(tmp->content);
// 			first = 0;
// 			pipe_flag = 1;
// 		}
// 		else if (((t_cmd *)tmp->content)->args && (((t_cmd *)tmp->content)->status_flag & FLG_PIPE) && first == 0)
// 		{
// 			printf("interm with pipe\n");
// 			fd = interm_cmd_with_pipe(tmp->content, fd);
// 			pipe_flag = 1;
// 		}
// 		else if (((t_cmd *)tmp->content)->args && (((t_cmd *)tmp->content)->status_flag & FLG_EO_CMD) && first == 0 && pipe_flag == 1)
// 		{
// 			printf("with ;\n");
// 			last_cmd_with_pipe(tmp->content, fd);
// 			pipe_flag = 0;
// 		}
// 		else if (((t_cmd *)tmp->content)->args && (((t_cmd *)tmp->content)->status_flag & FLG_EOL) && pipe_flag == 1)
// 		{
// 			printf("last with pipe\n");
// 			last_cmd_with_pipe(tmp->content, fd);
// 		}
// 		else if (((((t_cmd *)tmp->content)->status_flag & FLG_OUTPUT) || (((t_cmd *)tmp->content)->status_flag & FLG_APPEND || (((t_cmd *)tmp->content)->status_flag & FLG_INPUT))))
// 		{
// 			printf("cmd with redir\n");
// 			tmp->content = get_complete_cmd(tmp->content, lst);
// 			fd = cmd_with_redir(tmp->content, tmp);
// 			redir_flag = 1;
// 			while (tmp && (((t_cmd *)tmp->content)->status_flag & FLG_OUTPUT || ((t_cmd *)tmp->content)->status_flag & FLG_APPEND || (((t_cmd *)tmp->content)->status_flag & FLG_INPUT)))
// 				tmp = tmp->next;
// 		}
// 		else if (((t_cmd *)tmp->content)->args && ((t_cmd *)tmp->content)->args && redir_flag == 1)
// 		{
// 			printf("last with pipe before get redir\n");
// 			cmd_before_get_redir(tmp->content, fd);
// 		}
// 		else if (((t_cmd *)tmp->content)->args)
// 		{
// 			printf("simple\n");
// 			simple_cmd(tmp->content);
// 		}
// 		// else if (!((t_cmd *)tmp->content)->args)
// 		// {
// 		// 	ft_dprintf(STDERR_FILENO, PROG_NAME": syntax error near unexpected token `|'\n");
// 		// 	return ;
// 		// }
// 		tmp = tmp->next;
// 	}
// }