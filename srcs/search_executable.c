/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   search_executable.c                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: besellem <besellem@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/05/20 17:51:02 by besellem          #+#    #+#             */
/*   Updated: 2021/05/20 17:53:48 by besellem         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

char	*search_executable(char *s)
{
	const char	*path = getenv("PATH");
	char		**exectbl;
	char		*cmd;
	int			fd;
	size_t		i;

	if (!path)
		return (NULL);
	exectbl = ft_split(path, ':');
	if (!exectbl)
		return (NULL);
	i = 0;
	while (exectbl[i])
	{
		ft_asprintf(&cmd, "%s/%s", exectbl[i++], s);
		if (!cmd)
			continue ;
		fd = open(cmd, O_RDONLY);
		if (fd != -1)
		{
			ft_strsfree(ft_strslen(exectbl), exectbl);
			close(fd);
			return (cmd);
		}
		ft_memdel((void **)&cmd);
	}
	ft_strsfree(ft_strslen(exectbl), exectbl);
	return (NULL);
}
