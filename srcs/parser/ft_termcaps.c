/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_termcaps.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: besellem <besellem@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/06/02 13:45:53 by besellem          #+#    #+#             */
/*   Updated: 2021/06/02 14:42:27 by besellem         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

void	ft_termcap_history(char *termcap)
{
	t_list	*tmp;

	tmp = NULL;
	if (0 == ft_strcmp(termcap, K_UP))
		singleton()->hist.current--;
	else if (0 == ft_strcmp(termcap, K_DOWN))
		singleton()->hist.current++;
	else
		return ;
	if (singleton()->hist.current > singleton()->hist.size)
		singleton()->hist.current = singleton()->hist.size;
	ft_putstr_fd(CLR_LINE, STDIN_FILENO);
	print_prompt();
	tmp = ft_lstindex(singleton()->hist.history, singleton()->hist.current);
	if (tmp)
		ft_dprintf(STDIN_FILENO, "%s", tmp->content);
}

void	ft_termcap_edition(char *termcap)
{
	if (0 == ft_strcmp(termcap, K_RIGHT))
	{
		ft_printf("RIGHT\n");
	}
	else if (0 == ft_strcmp(termcap, K_LEFT))
	{
		ft_printf("LEFT\n");
	}
}
