/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   create_fd_utils.c                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kaye <kaye@student.42.fr>                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/06/15 18:13:57 by kaye              #+#    #+#             */
/*   Updated: 2021/06/24 15:17:34 by kaye             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

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
	char	*new_name;
	int		fd;

	new_name = NULL;
	fd = -1;
	while (1)
	{
		ft_asprintf(&new_name, "%s%d", TMP_FD, i);
		fd = open(new_name, O_RDWR);
		if (fd != -1)
		{
			close(fd);
			free(new_name);
			++i;
		}
		else
			return (new_name);
	}
	return (NULL);
}
