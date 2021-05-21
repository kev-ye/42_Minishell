/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   search_executable.c                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: besellem <besellem@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/05/20 17:51:02 by besellem          #+#    #+#             */
/*   Updated: 2021/05/20 18:16:43 by besellem         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

static char	*find_exec(char **exectbl, char *command)
{
	char	*cmd;
	int		fd;
	size_t	i;

	i = 0;
	while (exectbl[i])
	{
		ft_asprintf(&cmd, "%s/%s", exectbl[i++], command);
		if (!cmd)	// may want to exit() minishell when this happens
			continue ;
		fd = open(cmd, O_RDONLY);
		if (fd != -1)
		{
			close(fd);
			return (cmd);
		}
		ft_memdel((void **)&cmd);
	}
	return (NULL);
}

char	*search_executable(char *command)
{
	const char	*path = getenv("PATH");
	char		**exectbl;
	char		*cmd;

	if (!path)
		return (NULL);
	exectbl = ft_split(path, ':');
	if (!exectbl)
		return (NULL);
	cmd = find_exec(exectbl, command);
	ft_strsfree(ft_strslen(exectbl), exectbl);
	return (cmd);
}
