/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   exec_cmds.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kaye <kaye@student.42.fr>                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/05/20 22:33:29 by besellem          #+#    #+#             */
/*   Updated: 2021/05/22 16:06:39 by kaye             ###   ########.fr       */
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
	const t_builtin builtin[BUILTIN] = {{"echo", ft_echo, NULL}, 
				{"cd", ft_cd, NULL}, {"pwd", NULL, ft_pwd}, 
				{"env", ft_env, NULL}, {"unset", ft_unset, NULL}, 
				{"export", NULL, NULL}, {"exit", NULL, ft_exit}};
	int i;

	i = 0;
	while (i < BUILTIN)
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

void	ft_pre_exec_cmd(void *cmd)
{
	__attribute__((unused)) char	*s;
	__attribute__((unused)) t_list	*cmds;
	__attribute__((unused)) size_t	i;

	// got_quotes(0, 0, 1);
	// s = (char *)cmd;
	// i = 0;
	// while (s[i])
	// {
	// 	got_quotes((s[i] == '\''), (s[i] == '"'), 0);
	// 	if (!got_quotes(0, 0, 0))
	// 	{
	// 		ft_lstadd_back(&cmds, ft_lstnew(ft_substr(s, 0, i)));
	// 		s = s + i;
	// 		i = 0;
	// 	}
	// 	else
	// 		++i;
	// }
	// char **cmd_list = ft_lst_to_strs(cmds);
	char **cmd_list = ft_split(cmd, ' ');
	if (!cmd_list || !*cmd_list)
		return ;
	char *ex = search_executable(cmd_list[0]);
	//////////////////////////////////////////////////////// kaye
	char *bl = search_builtin_executable(cmd_list[0]);
	if (bl)
	{
		ft_printf(B_RED "`%s' builtin command:\n" CLR_COLOR, bl);
		singleton()->last_return_value = ft_exec_builtin_cmd(cmd_list);
		// free(bl);
	}
	////////////////////////////////////////////////////////////
	else if (ex)
	{
		ft_printf(B_RED "`%s' command:\n" CLR_COLOR, ex);
		singleton()->last_return_value = ft_exec_cmd(ex, cmd_list);
		free(ex);
	}
	ft_strsfree(ft_strslen(cmd_list) + 1, cmd_list);
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

void	*clean_data(void *data)
{
	// void	*tmp;

	// tmp = data;
	// data = ft_strtrim(data, " ");
	// free(tmp);
	return (data);
}

void	ft_exec_each_cmd(void)
{
	ft_lstiter_replace(singleton()->lst, clean_data);
	ft_lstiter(singleton()->lst, ft_pre_exec_cmd);

	// ft_lstprint(singleton()->lst, '\n');
}
