/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   create_fd_utils.c                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kaye <kaye@student.42.fr>                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/06/15 18:13:57 by kaye              #+#    #+#             */
/*   Updated: 2021/06/22 15:55:40 by kaye             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

typedef struct s_tmp_fd
{
	char	*old_name;
	char	*new_name;
	char	*fd_nbr;
	int		fd;
}	t_tmp_fd;

static t_tmp_fd	init_tmp_fd(void)
{
	t_tmp_fd	tmp_fd;

	tmp_fd.old_name = NULL;
	tmp_fd.new_name = NULL;
	tmp_fd.fd_nbr = NULL;
	tmp_fd.fd = -1;
	return (tmp_fd);
}

int	check_for_next(t_list *lst_cmd)
{
	int	next_flag;

	next_flag = 0;
	if (flag_check(lst_cmd) == FLG_INPUT)
		next_flag = F_INPUT;
	else if (flag_check(lst_cmd) == FLG_OUTPUT)
		next_flag = F_TRUNC;
	else if (flag_check(lst_cmd) == FLG_APPEND)
		next_flag = F_APPEND;
	else if (flag_check(lst_cmd) == FLG_DINPUT)
		next_flag = F_DINPUT;
	return (next_flag);
}

char	*new_tmp_fd_name(int i)
{
	t_tmp_fd	tmp_fd;

	tmp_fd = init_tmp_fd();
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
			free(tmp_fd.new_name);
			++i;
		}
		else
			return (tmp_fd.new_name);
	}
	return (NULL);
}
