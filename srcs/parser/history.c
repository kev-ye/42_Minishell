/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   history.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: besellem <besellem@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/06/02 20:53:49 by besellem          #+#    #+#             */
/*   Updated: 2021/06/02 23:12:45 by besellem         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

// home = ft_getenv("HOME");
static void	create_history(void)
{
	char	*path;
	char	*home;

	home = NULL;
	if (!home || !ft_is_openable(home, O_RDONLY))
	{
		if (home)
			free(home);
		home = getcwd(NULL, 0);
	}
	ft_asprintf(&path, "%s/" HISTORY_FILENAME, home);
	if (home)
		ft_memdel((void **)&home);
	if (!path)
		ft_malloc_error(__FILE__, __LINE__);
	singleton()->hist.path = path;
	singleton()->hist.fd = open(singleton()->hist.path,
		O_RDWR | O_CREAT | O_APPEND, 0644);
	if (singleton()->hist.fd == -1)
		ft_malloc_error(__FILE__, __LINE__);
}

void	init_history(void)
{
	char	*ret;
	int		check;
	t_list	*new;

	create_history();
	while (1)
	{
		check = get_next_line(singleton()->hist.fd, &ret);
		if ((0 == ft_strlen(ret) || ft_strisall(ret, ft_isspace)))
		{
			ft_memdel((void **)&ret);
			if (check > 0)
				continue ;
			else
				break ;
		}
		new = ft_lstnew(ret);
		if (check < 0 || !new)
			ft_malloc_error(__FILE__, __LINE__);
		ft_lstadd_back(&singleton()->hist.history, new);
		if (0 == check)
			break ;
	}
	singleton()->hist.size = ft_lstsize(singleton()->hist.history);
	singleton()->hist.current = singleton()->hist.size;
}

void	add2history(char *cmd)
{
	t_list	*new;

	singleton()->hist.current = singleton()->hist.size;
	if (!cmd || ft_strisall(cmd, ft_isspace) || 0 == ft_strlen(cmd))
		return ;
	if (!ft_is_openable(singleton()->hist.path, O_RDONLY))
	{
		singleton()->hist.fd = open(singleton()->hist.path,
			O_RDWR | O_CREAT | O_APPEND, 0644);
		if (singleton()->hist.fd == -1)
			ft_malloc_error(__FILE__, __LINE__);
	}
	ft_putendl_fd(cmd, singleton()->hist.fd);
	new = ft_lstnew(cmd);
	if (!new)
		ft_malloc_error(__FILE__, __LINE__);
	ft_lstadd_back(&singleton()->hist.history, new);
	singleton()->hist.size++;
	singleton()->hist.current = singleton()->hist.size;
}
