/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_termcaps.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: besellem <besellem@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/06/02 13:45:53 by besellem          #+#    #+#             */
/*   Updated: 2021/06/02 18:23:12 by besellem         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

void	ft_termcap_history(char **ptr, char *termcap)
{
	t_list	*tmp;

	if (0 == ft_strcmp(termcap, K_UP) && singleton()->hist.current > 0)
		singleton()->hist.current--;
	else if (0 == ft_strcmp(termcap, K_DOWN))
		singleton()->hist.current++;
	if (singleton()->hist.current > singleton()->hist.size)
		singleton()->hist.current = singleton()->hist.size;
	ft_putstr_fd(CLR_LINE, STDIN_FILENO);
	print_prompt();
	tmp = ft_lstindex(singleton()->hist.history, singleton()->hist.current);
	if (tmp && tmp->content)
	{
		// if (*ptr)
		// 	free(*ptr);
		*ptr = tmp->content;
		ft_dprintf(STDIN_FILENO, "%s", tmp->content);
	}
	else
		*ptr = ft_strdup("");
}

void	ft_termcap_edition(char **ptr, char *termcap)
{
	if (BONUS)
	{
		if (0 == ft_strcmp(termcap, K_RIGHT))
		{
			// ft_printf("RIGHT\n");
		}
		else if (0 == ft_strcmp(termcap, K_LEFT))
		{
			// ft_printf("LEFT\n");
		}
	}
	(void)ptr;
}

void	ft_termcap_delete_char(char **ptr)
{
	ft_putstr_fd(CLR_LINE, STDIN_FILENO);
	print_prompt();
	if (ft_strlen(*ptr) > 0)
	{
		*ptr = ft_substr(*ptr, 0, ft_strlen(*ptr) - 1);
		ft_dprintf(STDIN_FILENO, "%s", *ptr);
	}
}

void	ft_termcap_clear_line(char **ptr)
{
	ft_putstr_fd(CLR_LINE, STDIN_FILENO);
	print_prompt();
	*ptr = ft_strdup("");
}

void	ft_termcap_clear_screen(char **ptr)
{
	ft_putstr_fd(CLR_SCREEN, STDIN_FILENO);
	print_prompt();
	ft_dprintf(STDIN_FILENO, "%s", *ptr);
}

void	ft_termcap_esc(char **ptr)
{
	(void)ptr;
	ft_exit();
}
