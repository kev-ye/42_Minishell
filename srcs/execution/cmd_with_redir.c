/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   cmd_with_redir.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kaye <kaye@student.42.fr>                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/06/06 19:55:19 by kaye              #+#    #+#             */
/*   Updated: 2021/06/10 18:58:28 by kaye             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

void *get_complete_cmd(void *cmd, t_list *lst_cmd)
{
	t_list	*lst_tmp;
	size_t len;
	char **new_cmd;
	int i;
	int j;

	lst_tmp = lst_cmd;
	while (lst_tmp && (((t_cmd *)lst_tmp->content)->status_flag & FLG_OUTPUT
		|| ((t_cmd *)lst_tmp->content)->status_flag & FLG_APPEND
		|| ((t_cmd *)lst_tmp->content)->status_flag & FLG_INPUT))
		lst_tmp = lst_tmp->next;
	len = ft_strslen(((t_cmd *)lst_tmp->content)->args);
	if (lst_tmp && len > 1)
	{
		len += ft_strslen(((t_cmd *)cmd)->args);
		new_cmd = malloc(sizeof(char *) * (len + 1));
		if (!new_cmd)
			return (NULL);
		i = 0;
		while (((t_cmd *)cmd)->args[i])
		{
			new_cmd[i] = ft_strdup(((t_cmd *)cmd)->args[i]);
			++i;
			--len;
		}
		j = 1;
		while (((t_cmd *)lst_tmp->content)->args[j] && len)
		{
			new_cmd[i] = ft_strdup(((t_cmd *)lst_tmp->content)->args[j]);
			++i;
			++j;
			--len;
		}
		new_cmd[i] = NULL;
		ft_strsfree(ft_strslen(((t_cmd *)cmd)->args), ((t_cmd *)cmd)->args);
		((t_cmd *)cmd)->args = new_cmd;
	}
	return (cmd);
}

void	redir_parser(int fd_input, int fd_output, t_list *lst_cmd)
{
	// first cmd
	int first;
	
	int tmp_fd_output;
	int tmp_fd_input;

	int flag_append;
	int flag_trunc;
	int flag_redir_input;
	int flag_d_input;

	// double input
	char *input_str;

	// init
	first = 1;

	tmp_fd_output = -2;
	tmp_fd_input = -2;

	flag_append = 0;
	flag_trunc = 0;
	flag_redir_input = 0;
	flag_d_input = 0;

	input_str = NULL;

	// start
	while(lst_cmd)
	{
		if (first == 1)
			first = 0;
		else if (!first && is_redir(lst_cmd))
		{
			// try file
			if (flag_trunc == 1)
			{
				tmp_fd_output = open(((t_cmd *)lst_cmd->content)->args[0], O_WRONLY | O_TRUNC | O_CREAT, 0666);
			}
			else if (flag_append == 1)
			{
				tmp_fd_output = open(((t_cmd *)lst_cmd->content)->args[0], O_WRONLY | O_APPEND | O_CREAT, 0666);
			}
			else if (flag_redir_input == 1)
			{
				tmp_fd_input = open(((t_cmd *)lst_cmd->content)->args[0], O_RDWR);
				if (tmp_fd_input == -1)
				{
					ft_dprintf(STDERR_FILENO, "minishell: %s: %s\n", ((t_cmd *)lst_cmd->content)->args[0], strerror(errno));
					if (tmp_fd_output != -1)
						close(tmp_fd_output);
					exit(LRV_GENERAL_ERROR);
				}
				else
				{
					fd_input = tmp_fd_input;
					dup2(fd_input, STDIN_FILENO);
				}
			}
			else if (flag_d_input == 1)
			{
				tmp_fd_input = open(TMP_FD, O_RDWR | O_CREAT | O_TRUNC | O_APPEND, 0666);
				if (tmp_fd_input == -1)
				{
					ft_dprintf(STDERR_FILENO, "open for double input crash\n");
					if (tmp_fd_output != -1)
						close(tmp_fd_output);
					exit(LRV_GENERAL_ERROR);
				}
				else
				{
					while (1)
					{
						input_str = readline("> ");
						if (!ft_strcmp(input_str, ((t_cmd *)lst_cmd->content)->args[0]))
						{
							free(input_str);
							break ;
						}
						ft_putendl_fd(input_str, tmp_fd_input);
					}
					close(tmp_fd_input);
					tmp_fd_input = open(TMP_FD, O_RDONLY);
					if (tmp_fd_input == -1)
					{
						ft_dprintf(STDERR_FILENO, "open for double input crash\n");
						if (tmp_fd_output != -1)
							close(tmp_fd_output);
						exit(LRV_GENERAL_ERROR);
					}
					fd_input = tmp_fd_input;
					dup2(fd_input, STDIN_FILENO);
				}
			}
			
			// check output file
			if (tmp_fd_output == -1)
			{
				ft_dprintf(STDERR_FILENO, "minishell: %s: %s\n", ((t_cmd *)lst_cmd->content)->args[0], strerror(errno));
				exit(LRV_GENERAL_ERROR);
			}
		}
		else if (!first)
		{
			// '>' -> create trunc file
			if (flag_trunc == 1)
			{
				// printf("[%s] is trunc\n", ((t_cmd *)lst_cmd->content)->args[0]);
				if (tmp_fd_output != -1)
					close(tmp_fd_output);
				tmp_fd_output = open(((t_cmd *)lst_cmd->content)->args[0], O_WRONLY | O_TRUNC | O_CREAT, 0666);
			}
			// '>>' -> create append file
			else if (flag_append == 1)
			{
				// printf("[%s] is append\n", ((t_cmd *)lst_cmd->content)->args[0]);
				if (tmp_fd_output != -1)
					close(tmp_fd_output);
				tmp_fd_output = open(((t_cmd *)lst_cmd->content)->args[0], O_WRONLY | O_APPEND | O_CREAT, 0666);
			}
			// '<' -> check if file exit
			else if (flag_redir_input == 1)
			{
				tmp_fd_input = open(((t_cmd *)lst_cmd->content)->args[0], O_RDWR);
				if (tmp_fd_input == -1)
				{
					ft_dprintf(STDERR_FILENO, "minishell: %s: %s\n", ((t_cmd *)lst_cmd->content)->args[0], strerror(errno));
					if (tmp_fd_output != -1)
						close(tmp_fd_output);
					exit(LRV_GENERAL_ERROR);
				}
				else
				{
					fd_input = tmp_fd_input;
					dup2(fd_input, STDIN_FILENO);
				}
			}
			// '<<' -> double input
			else if (flag_d_input == 1)
			{
				tmp_fd_input = open(TMP_FD, O_RDWR | O_CREAT | O_TRUNC | O_APPEND, 0666);
				if (tmp_fd_input == -1)
				{
					ft_dprintf(STDERR_FILENO, "open for double input crash\n");
					if (tmp_fd_output != -1)
						close(tmp_fd_output);
					exit(LRV_GENERAL_ERROR);
				}
				else
				{
					while (1)
					{
						input_str = readline("> ");
						if (!ft_strcmp(input_str, ((t_cmd *)lst_cmd->content)->args[0]))
						{
							free(input_str);
							break ;
						}
						ft_putendl_fd(input_str, tmp_fd_input);
					}
					close(tmp_fd_input);
					tmp_fd_input = open(TMP_FD, O_RDONLY);
					if (tmp_fd_input == -1)
					{
						ft_dprintf(STDERR_FILENO, "open for double input crash\n");
						if (tmp_fd_output != -1)
							close(tmp_fd_output);
						exit(LRV_GENERAL_ERROR);
					}
					fd_input = tmp_fd_input;
					dup2(fd_input, STDIN_FILENO);
				}
			}

			if (tmp_fd_output == -1)
			{
				ft_dprintf(STDERR_FILENO, "minishell: %s: %s\n", ((t_cmd *)lst_cmd->content)->args[0], strerror(errno));
				exit(LRV_GENERAL_ERROR);
			}
			else
			{
				fd_output = tmp_fd_output;
				dup2(fd_output, STDOUT_FILENO);
			}
		}

		// active flag
		if (flag_check(lst_cmd) == FLG_INPUT)
		{
			flag_redir_input = 1;
			flag_append = 0;
			flag_trunc = 0;
			flag_d_input = 0;
		}
		else if (flag_check(lst_cmd) == FLG_OUTPUT)
		{
			flag_trunc = 1;
			flag_append = 0;
			flag_redir_input = 0;
			flag_d_input = 0;
		}
		else if (flag_check(lst_cmd) == FLG_APPEND)
		{
			flag_append = 1;
			flag_trunc = 0;
			flag_redir_input = 0;
			flag_d_input = 0;
		}
		else if ((flag_check(lst_cmd) == FLG_DINPUT))
		{
			flag_d_input = 1;
			flag_append = 0;
			flag_trunc = 0;
			flag_redir_input = 0;
		}
		else
			return ;
		lst_cmd = lst_cmd->next;
	}
}

void	cmd_with_redir(void *cmd, t_list *lst_cmd)
{
	pid_t	pid;
	int		output_fd;
	int 	input_fd;
	int 	tmp_errno;
	int 	status = 1;
	int 	builtin_status = 1;

	input_fd = -1;
	output_fd = -1;
	tmp_errno = 0;
	pid = fork();
	if (pid < 0)
			exit(1);
	else if (pid == 0)
	{
		cmd = get_complete_cmd(cmd, lst_cmd);
		redir_parser(input_fd, output_fd, lst_cmd);

		builtin_status = builtin_exec(((t_cmd *)cmd)->args);
		if (builtin_status == NOT_FOUND)
			sys_exec(cmd);
		if (builtin_status != NOT_FOUND)
			exit(SUCCESS);
		exit(EXEC_FAILURE);
	}
	else
	{
		// wait(&status);
		waitpid(pid, &status, 0);
		close(input_fd);
		close(output_fd);
	}
	if (WIFEXITED(status) != 0)
		singleton()->last_return_value = WEXITSTATUS(status);
	else if (WIFSIGNALED(status) == 1)
		singleton()->last_return_value = LRV_KILL_SIG + WTERMSIG(status);
}