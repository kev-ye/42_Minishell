/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   exec_cmds.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kaye <kaye@student.42.fr>                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/05/20 22:33:29 by besellem          #+#    #+#             */
/*   Updated: 2021/06/06 20:02:54 by kaye             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

#define SYNTAXERR "syntax error near unexpected token"

#define SIMPLE 0
#define ONLY_PIPE 1
#define ONLY_REDIR 2
#define MIX 4

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

//////////////////////////////////////////////////////////////////////////////////////////////////////// exec
//// kaye ///////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////
//// last time here -> echo > f1 ; ls ; ls | grep incs | wc ; ls -> seg
//// and have file "wc" created
/////////////////////////////////////////////////////////////////////////////////////////////////////////////

void	simple_cmd(void *cmd)
{
	pid_t	pid;

	if (!ft_strcmp(((t_cmd *)cmd)->args[0], "cd") || !ft_strcmp(((t_cmd *)cmd)->args[0], "exit"))
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

int only_pipe(t_list *lst_cmd)
{
	t_list *tmp;
	int ret;

	tmp = lst_cmd;
	ret = -1;
	while (tmp)
	{
		if (((t_cmd *)tmp->content)->status_flag & FLG_PIPE)
		{
			ret = (int)((t_cmd *)tmp->content)->status_flag;
			tmp = tmp->next;
		}
		else
			break ;
	}
	if (ret != -1
		&& ((((t_cmd *)tmp->content)->status_flag & FLG_EOL)
		|| (((t_cmd *)tmp->content)->status_flag & FLG_EO_CMD)))
		return (ONLY_PIPE);
	return (-1);
}

int only_redir(t_list *lst_cmd)
{
	t_list *tmp;
	int ret;

	tmp = lst_cmd;
	ret = -1;
	while (tmp)
	{
		if ((((t_cmd *)tmp->content)->status_flag & FLG_INPUT)
			|| (((t_cmd *)tmp->content)->status_flag & FLG_OUTPUT)
			|| (((t_cmd *)tmp->content)->status_flag & FLG_APPEND))
		{
			ret = (int)((t_cmd *)tmp->content)->status_flag;
			tmp = tmp->next;
		}
		else
			break ;
	}
	if (ret != -1
		&&((((t_cmd *)tmp->content)->status_flag & FLG_EOL)
		|| (((t_cmd *)tmp->content)->status_flag & FLG_EO_CMD)))
		return (ONLY_REDIR);
	return (-1);
}

int part_cmd_check(t_list *lst_cmd)
{
	t_list *tmp;
	int ret_only_pipe;
	int ret_only_redir;

	tmp = lst_cmd;
	ret_only_pipe = only_pipe(lst_cmd);
	ret_only_redir = only_redir(lst_cmd);
	if (ret_only_pipe == -1 && ret_only_redir == -1
		&& (!(((t_cmd *)tmp->content)->status_flag & FLG_EO_CMD)
		|| !(((t_cmd *)tmp->content)->status_flag & FLG_EOL)))
		return (MIX);
	else if (ret_only_pipe != -1)
		return (ONLY_PIPE);
	else if (ret_only_redir != -1)
		return (ONLY_REDIR);
	return (-1);
}

int 	syntax_error(t_list *lst_cmd)
{
	t_list *tmp;

	tmp = lst_cmd;
	if (tmp && !((t_cmd *)tmp->content)->args
		&& ((((t_cmd *)tmp->content)->status_flag & FLG_PIPE)
		|| (((t_cmd *)tmp->content)->status_flag & FLG_EO_CMD)
		|| (((t_cmd *)tmp->content)->status_flag & FLG_INPUT)
		|| (((t_cmd *)tmp->content)->status_flag & FLG_OUTPUT)
		|| (((t_cmd *)tmp->content)->status_flag & FLG_APPEND)))
	{
		if (((t_cmd *)tmp->content)->status_flag & FLG_PIPE)
			ft_dprintf(STDERR_FILENO, PROG_NAME": "SYNTAXERR" `|'\n");
		else if (((t_cmd *)tmp->content)->status_flag & FLG_EO_CMD)
			ft_dprintf(STDERR_FILENO, PROG_NAME": "SYNTAXERR" `;'\n");
		else
			ft_dprintf(STDERR_FILENO, PROG_NAME": "SYNTAXERR" `newline'\n");
		return (1);
	}
	return (0);
}

int 	syntax_error2(t_list *lst_cmd)
{
	t_list *tmp;

	tmp = lst_cmd;
	if (tmp && tmp->next && ((t_cmd *)tmp->content)->args
		&& !((t_cmd *)tmp->next->content)->args
		&& ((((t_cmd *)tmp->content)->status_flag & FLG_INPUT)
		|| (((t_cmd *)tmp->content)->status_flag & FLG_OUTPUT)
		|| (((t_cmd *)tmp->content)->status_flag & FLG_APPEND)))
	{
		ft_dprintf(STDERR_FILENO, PROG_NAME": "SYNTAXERR" `newline'\n");
		return (1);
	}
	return (0);
}

void	ft_exec_each_cmd(t_list *lst_cmd)
{
	t_list	*tmp;
	int cmd_line;

	tmp = lst_cmd;
	cmd_line = -1;
	while (tmp)
	{
		if (syntax_error(tmp) || syntax_error2(tmp) || !((t_cmd *)tmp->content)->args)
			return ;
		if (!((t_cmd *)tmp->content)->args
			&& ((((t_cmd *)tmp->content)->status_flag & FLG_PIPE)
			|| (((t_cmd *)tmp->content)->status_flag & FLG_EO_CMD)))
		{
			if (((t_cmd *)tmp->content)->status_flag & FLG_PIPE)
				ft_dprintf(STDERR_FILENO, PROG_NAME": "SYNTAXERR" `|'\n");
			else if (((t_cmd *)tmp->content)->status_flag & FLG_EO_CMD)
				ft_dprintf(STDERR_FILENO, PROG_NAME": "SYNTAXERR" `;'\n");
			return ;
		}
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
				cmd_with_pipe(tmp);
			else if (cmd_line == ONLY_REDIR)
				cmd_with_redir(tmp->content, tmp);
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
// 		// else if (!((t_cmd *)tmp->content)->args)                              //////////////////////// some pb here : quit with ctrl + D show message
// 		// {
// 		// 	ft_dprintf(STDERR_FILENO, PROG_NAME": syntax error near unexpected token `|'\n");
// 		// 	return ;
// 		// }
// 		tmp = tmp->next;
// 	}
// }

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