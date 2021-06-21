/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   exec_cmds.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kaye <kaye@student.42.fr>                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/05/20 22:33:29 by besellem          #+#    #+#             */
/*   Updated: 2021/06/21 19:59:18 by kaye             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

int	simple_cmd(void *cmd)
{
	pid_t	pid;
	int		status;
	int		built_exec;
	int		lrv;

	status = 1;
	built_exec = 0;
	if (builtin_exec((t_cmd *)cmd) == NOT_FOUND)
	{
		pid = fork();
		if (pid < 0)
			exit(PID_FAILURE);
		else if (pid == 0)
		{
			lrv = sys_exec(cmd);
			exit(lrv);
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
	char	*old_name;
	char	*new_name;
	char	*fd_nbr;
	int		fd;

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
	{
		ft_memdel((void **)&new_name);
		return ;
	}
}

void	cmd_up(t_list *lst_cmd)
{
	int		i;
	int		j;
	char	*new;
	char	*tmp;

	while (lst_cmd)
	{
		i = 0;
		while ((t_cmd *)lst_cmd->content
			&& ((t_cmd *)lst_cmd->content)->args
			&& ((t_cmd *)lst_cmd->content)->args[i])
		{
			j = 0;
			while (TRUE)
			{
				while (((t_cmd *)lst_cmd->content)->args[i][j]
					&& ((t_cmd *)lst_cmd->content)->args[i][j] != -1)
					++j;
				if (((t_cmd *)lst_cmd->content)->args[i][j] == '\0')
					break ;
				ft_asprintf(&new, "%.*s%d%s", j,
					((t_cmd *)lst_cmd->content)->args[i],
					singleton()->last_return_value,
					((t_cmd *)lst_cmd->content)->args[i] + j + 1);
				tmp = ((t_cmd *)lst_cmd->content)->args[i];
				((t_cmd *)lst_cmd->content)->args[i] = new;
				ft_memdel((void **)&tmp);
				j += ft_nblen(singleton()->last_return_value);
			}
			++i;
		}
		if (flag_check(lst_cmd) == FLG_EO_CMD)
			break ;
		lst_cmd = lst_cmd->next;
	}
}

int	ft_exec_each_cmd(t_list *lst_cmd)
{
	t_list	*tmp;
	int		cmd_line;
	int		built_exec;
	int		i;

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
		cmd_up(tmp);
		exec_all_in_one(tmp);
		while (tmp && flag_check(tmp) != FLG_EO_CMD)
			tmp = tmp->next;
		if (tmp)
			tmp = tmp->next;
	}
	unlink_all_tmp_fd(i);
	return (built_exec);
}
