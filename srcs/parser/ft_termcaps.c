/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_termcaps.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: besellem <besellem@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/06/02 13:45:53 by besellem          #+#    #+#             */
/*   Updated: 2021/06/03 11:22:27 by besellem         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

void	ft_termcap_history(char **ptr, char *termcap)
{
	const void	*tmp_ptr = *ptr;
	t_list		*tmp;

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
		*ptr = ft_strdup(tmp->content);
		ft_dprintf(STDIN_FILENO, "%s", ft_strdup(tmp->content));
	}
	else
		*ptr = ft_strdup("");
	ft_memdel((void **)&tmp_ptr);
}

void	ft_termcap_edition(char **ptr, char *termcap)
{
	// if (BONUS)
	// {
		singleton()->edit.len = ft_strlen(*ptr);
		if (0 == ft_strcmp(termcap, K_RIGHT))
		{
			ft_putstr_fd("\e[1C", STDIN_FILENO);
		}
		else if (0 == ft_strcmp(termcap, K_LEFT))
		{
			ft_putstr_fd("\e[1D", STDIN_FILENO);
		}
	// }
	(void)ptr;
}

void	ft_termcap_delete_char(char **ptr)
{
	const void	*tmp_ptr = *ptr;

	ft_putstr_fd(CLR_LINE, STDIN_FILENO);
	print_prompt();
	if (ft_strlen(*ptr) > 0)
	{
		*ptr = ft_substr(*ptr, 0, ft_strlen(*ptr) - 1);
		ft_dprintf(STDIN_FILENO, "%s", *ptr);
		ft_memdel((void **)&tmp_ptr);
	}
}

void	ft_termcap_clear_line(char **ptr)
{
	const void	*tmp_ptr = *ptr;

	ft_putstr_fd(CLR_LINE, STDIN_FILENO);
	print_prompt();
	*ptr = ft_strdup("");
	ft_memdel((void **)&tmp_ptr);
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
