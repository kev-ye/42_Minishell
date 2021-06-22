/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   redir_parser_utils.c                               :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kaye <kaye@student.42.fr>                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/06/16 13:53:35 by kaye              #+#    #+#             */
/*   Updated: 2021/06/22 14:38:59 by kaye             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

static int	g_trunc_fd = O_WRONLY | O_TRUNC | O_CREAT;
static int	g_append_fd = O_WRONLY | O_APPEND | O_CREAT;

typedef struct s_tmp_fd
{
	char	*old_name;
	char	*new_name;
	char	*fd_nbr;
	int		fd;
}	t_tmp_fd;

static void	parser_output_fd(t_list *cmd, int *fd_output, int flag_is)
{
	if (flag_is == F_TRUNC)
	{
		if (*fd_output != -1)
			close(*fd_output);
		*fd_output = open(((t_cmd *)cmd->content)->args[0], g_trunc_fd, 0666);
	}
	else if (flag_is == F_APPEND)
	{
		if (*fd_output != -1)
			close(*fd_output);
		*fd_output = open(((t_cmd *)cmd->content)->args[0], g_append_fd, 0666);
	}
	if (*fd_output != -1)
		dup2(*fd_output, STDOUT_FILENO);
}

static void	msg_error_with_exit(int opt, t_list *cmd)
{
	if (opt == 1)
	{
		ft_dprintf(STDERR_FILENO, "minishell: %s: %s\n",
			((t_cmd *)cmd->content)->args[0], strerror(errno));
		exit(LRV_GENERAL_ERROR);
	}
	else if (opt == 2)
	{
		ft_dprintf(STDERR_FILENO, "open for double input crash\n");
		exit(LRV_GENERAL_ERROR);
	}
}

static t_list	*parser_input_fd(t_list *cmd, int *fd_input, int flag_is)
{
	char	*tmp_fd_name;
	int		i;

	tmp_fd_name = NULL;
	i = 1;
	if (flag_is == F_INPUT)
	{
		*fd_input = open(((t_cmd *)cmd->content)->args[0], O_RDWR);
		if (*fd_input == -1)
			msg_error_with_exit(1, cmd);
		else
			dup2(*fd_input, STDIN_FILENO);
	}
	else if (flag_is == F_DINPUT)
	{
		tmp_fd_name = get_tmp_fd(i);
		*fd_input = open(tmp_fd_name, O_RDWR);
		free(tmp_fd_name);
		if (*fd_input == -1)
			msg_error_with_exit(2, cmd);
		else
			dup2(*fd_input, STDIN_FILENO);
	}
	return (cmd);
}

char	*get_tmp_fd(int i)
{
	t_tmp_fd	tmp_fd;

	tmp_fd.fd = -1;
	while (1)
	{
		tmp_fd.fd_nbr = ft_itoa(i);
		tmp_fd.old_name = malloc(sizeof(char) * ft_strlen(TMP_FD) + 1);
		if (!tmp_fd.old_name)
			return (NULL);
		ft_strcpy(tmp_fd.old_name, TMP_FD);
		tmp_fd.new_name = ft_strjoin(tmp_fd.old_name, tmp_fd.fd_nbr);
		free(tmp_fd.old_name);
		free(tmp_fd.fd_nbr);
		tmp_fd.fd = open(tmp_fd.new_name, O_RDWR);
		if (tmp_fd.fd != -1)
		{
			close(tmp_fd.fd);
			return (tmp_fd.new_name);
		}
		else
		{
			free(tmp_fd.new_name);
			++i;
		}
	}
	return (NULL);
}

void	redir_parser(t_list *cmd, int *fd_input, int *fd_output)
{
	int	first;
	int	flag_is;

	flag_is = 0;
	first = 1;
	while (cmd)
	{
		if (first == 1)
			first = 0;
		else if (!first && (flag_is == F_APPEND || flag_is == F_TRUNC))
			parser_output_fd(cmd, fd_output, flag_is);
		else if (!first && (flag_is == F_INPUT || flag_is == F_DINPUT))
			cmd = parser_input_fd(cmd, fd_input, flag_is);
		flag_is = check_for_next(cmd);
		if (!flag_is)
			return ;
		cmd = cmd->next;
	}
}
