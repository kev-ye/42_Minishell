/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   env.c                                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kaye <kaye@student.42.fr>                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/05/21 15:17:43 by kaye              #+#    #+#             */
/*   Updated: 2021/06/20 19:44:51 by kaye             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

static void	show_env(t_list **env)
{
	t_list	*tmp;

	tmp = *env;
	while (tmp)
	{
		if (ft_strchr((char *)tmp->content, '='))
			ft_dprintf(STDOUT_FILENO, "%s\n", (char *)tmp->content);
		tmp = tmp->next;
	}
}

int	check_if_path_exist(t_list *env)
{
	t_list *tmp;

	tmp = env;
	while (tmp)
	{
		if ((char *)tmp->content && !ft_strncmp((char *)tmp->content, "PATH=", 5))
			return (1);
		tmp = tmp->next;
	}
	return (0);

}

int	ft_env(t_cmd *cmds)
{
	int	len_cmd;
	int	i;

	i = 0;
	if (!cmds || !cmds->args || !*cmds->args)
		return (ERROR);
	while (cmds->args[i])
		++i;
	len_cmd = i;
	i = 0;
	if (len_cmd == 1)
		show_env(&singleton()->env);
	// else if (cmd->ac >= 2)
	// {
	// 	// coming soon ...
		// return ;
	// }
	return (SUCCESS);
	// exit(0);
}
