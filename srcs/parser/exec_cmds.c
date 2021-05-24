/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   exec_cmds.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: besellem <besellem@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/05/20 22:33:29 by besellem          #+#    #+#             */
/*   Updated: 2021/05/24 15:16:49 by besellem         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

// seems that it must be used on forked commands - not on the main program
void	ft_quit(int code)
{
	ft_dprintf(STDIN_FILENO, B_RED "SIGNAL\n" CLR_COLOR);
	ft_dprintf(2, "Quit: %d\n", code);
	// exit(code);
}

void	ft_interrupt(int code)
{
	ft_dprintf(STDIN_FILENO, B_RED "SIGNAL\n" CLR_COLOR);
	ft_dprintf(STDIN_FILENO, "interrupt: %d\n", code);
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
	return (SUCCESS);
}
//////////////////////////////////////////////////////

int	ft_exec_cmd(char *file, char **cmds)
{
	const pid_t	id = fork();

	if (id == 0)
	{
		// signal(SIGQUIT, ft_quit);
		// signal(SIGINT, ft_interrupt);
		// signal(SIGKILL, ft_interrupt);
		return (execve(file, cmds, NULL));
	}
	else
	{
		wait(NULL);
	}
	return (ERROR);
}

char	**ft_lst_to_strs(t_list *lst)
{
	char	**s;
	t_list	*tmp;
	size_t	i;

	if (!lst)
		return (NULL);
	s = ft_calloc(ft_lstsize(lst) + 1, sizeof(char *));
	if (!s)
		return (NULL);
	tmp = lst;
	i = 0;
	while (tmp)
	{
		s[i] = ft_calloc(ft_strlen(tmp->content) + 1, sizeof(char));
		if (!s[i])
			return (ft_strsfree(i, s));
		ft_memcpy(s[i++], tmp->content, ft_strlen(tmp->content));
		tmp = tmp->next;
	}
	s[i] = NULL;
	return (s);
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
		singleton()->last_return_value = ft_exec_cmd(ex, cmd->args);
		ft_memdel((void **)&ex);
	}
	else
	{
		
	}
	ft_strsfree(ft_strslen(cmd->args) + 1, cmd->args);
}

void	ft_lstiter_replace(t_list *lst, void *(*f)(void *))
{
	t_list	*tmp;

	tmp = lst;
	while (tmp)
	{
		if ((*f))
			tmp->content = (*f)(tmp->content);
		tmp = tmp->next;
	}
}

void	ft_exec_each_cmd(void)
{
	ft_lstiter(singleton()->lst, ft_pre_exec_cmd);
	// ft_lstprint_cmd(singleton()->lst, '\n');
}
