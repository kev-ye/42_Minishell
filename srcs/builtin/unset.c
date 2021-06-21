/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   unset.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kaye <kaye@student.42.fr>                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/05/21 15:17:36 by kaye              #+#    #+#             */
/*   Updated: 2021/06/21 19:10:54 by kaye             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

void	del_and_swap(t_list **tmp, t_list **next, t_list **prev)
{
	*next = (*tmp)->next;
	ft_lstdelone(*tmp, free);
	if (*prev)
	{
		*tmp = *prev;
		(*tmp)->next = *next;
	}
	else
		*tmp = *next;
}

void	del_env(t_list **lst_env, size_t len, char *cmd)
{
	t_list	*prev;
	t_list	*next;
	t_list	*tmp;

	tmp = *lst_env;
	prev = NULL;
	while (tmp)
	{
		if (!ft_strncmp(tmp->content, cmd, len)
			&& '=' == ((char *)(tmp->content))[len])
		{
			del_and_swap(&tmp, &next, &prev);
			return ;
		}
		else
		{
			prev = tmp;
			tmp = tmp->next;
		}
	}
}

int	ft_unset(t_cmd *cmds)
{
	if (!cmds || !cmds->args || !*cmds->args)
		return (ERROR);
	if (!*(cmds->args + 1))
		return (SUCCESS);
	del_env(&singleton()->env, ft_strlen(cmds->args[1]), cmds->args[1]);
	return (SUCCESS);
}
