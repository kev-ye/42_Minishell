/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   unset.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kaye <kaye@student.42.fr>                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/05/21 15:17:36 by kaye              #+#    #+#             */
/*   Updated: 2021/06/23 17:58:54 by kaye             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

void	del_and_swap(t_list **head, t_list **env, t_list **prev)
{
	t_list	*current;

	current = *env;
	if (*prev)
	{
		*env = *prev;
		(*env)->next = current->next;
	}
	else
		*head = current->next;
	ft_memdel((void **)&current->content);
	ft_lstdelone(current, NULL);
}

void	del_env(t_list *lst_env, size_t len, char *cmd)
{
	t_list	*prev;

	prev = NULL;
	while (lst_env)
	{
		if (!ft_strncmp(lst_env->content, cmd, len)
			&& '=' == ((char *)(lst_env->content))[len])
		{
			del_and_swap(&singleton()->env, &lst_env, &prev);
			prev = lst_env;
			lst_env = lst_env->next;
		}
		else
		{
			prev = lst_env;
			lst_env = lst_env->next;
		}
	}
}

int	ft_unset(t_cmd *cmds)
{
	if (!cmds || !cmds->args || !*cmds->args)
		return (ERROR);
	if (!*(cmds->args + 1))
		return (SUCCESS);
	del_env(singleton()->env, ft_strlen(cmds->args[1]), cmds->args[1]);
	return (SUCCESS);
}
