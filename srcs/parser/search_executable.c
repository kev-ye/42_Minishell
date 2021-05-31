/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   search_executable.c                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: besellem <besellem@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/05/20 17:51:02 by besellem          #+#    #+#             */
/*   Updated: 2021/05/31 18:11:16 by besellem         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

static char	*find_exec(char **exectbl, char *command)
{
	char	*cmd;
	size_t	i;

	if (ft_strchr(command, '/'))
	{
		ft_asprintf(&cmd, "%s", command);
		if (ft_is_openable(cmd, O_RDONLY))
			return (cmd);
	}
	else
	{
		i = 0;
		while (exectbl[i])
		{
			ft_asprintf(&cmd, "%s/%s", exectbl[i++], command);
			if (!cmd)
				return (NULL);
			// struct stat	s;
			// stat()
			if (ft_is_openable(cmd, O_RDONLY))
				return (cmd);
			ft_memdel((void **)&cmd);
		}
	}
	return (NULL);
}

char	*search_executable(char *command)
{
	const char	*path = ft_getenv("PATH");
	char		**exectbl;
	char		*cmd;

	if (!path)
	{
		/*
		** if $PATH is not set, bash does return a "No such file or directory"
		** message
		*/
		// printf("%s: %s: %s\n", PROG_NAME, command, strerror(errno));
		return (NULL);
	}
	exectbl = ft_split(path, ':');
	ft_memdel((void **)&path);
	if (!exectbl)
		return (NULL);
	cmd = find_exec(exectbl, command);
	ft_strsfree(ft_strslen(exectbl), exectbl);
	return (cmd);
}
