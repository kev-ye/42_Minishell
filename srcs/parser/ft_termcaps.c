/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_termcaps.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kaye <kaye@student.42.fr>                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/06/02 13:45:53 by besellem          #+#    #+#             */
/*   Updated: 2021/06/07 18:50:42 by kaye             ###   ########.fr       */
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
	singleton()->edit.len = ft_strlen(*ptr);
	singleton()->edit.current_index = singleton()->edit.len;
}

void	ft_termcap_edition(__attribute__((unused)) char **ptr, char *termcap)
{
	t_edition	*edit;

	// if (BONUS)
	// {
		edit = &singleton()->edit;
		if (0 == ft_strcmp(termcap, K_RIGHT) && edit->current_index < ft_strlen(*ptr))
		{
			edit->current_index++;
			ft_dprintf(STDIN_FILENO, "%s", tgetstr("nd", NULL));
		}
		else if (0 == ft_strcmp(termcap, K_LEFT) && edit->current_index > 0)
		{
			edit->current_index--;
			ft_dprintf(STDIN_FILENO, "%s", tgetstr("le", NULL));
		}
	// }
}

void	ft_termcap_delete_char(char **ptr)
{
	const void	*tmp_ptr = *ptr;
	const char	*go = tgetstr("ch", NULL);
	char		*ret;

	if (singleton()->edit.len > 0 && singleton()->edit.current_index > 0)
		singleton()->edit.len--;
	ft_putstr_fd(CLR_LINE, STDIN_FILENO);
	print_prompt();
	if (singleton()->edit.current_index > 0)
	{
		ft_asprintf(&ret, "%.*s%.*s",
			(int)singleton()->edit.current_index - 1,
			*ptr,
			(int)singleton()->edit.len - (int)(singleton()->edit.current_index - 1),
			*ptr + singleton()->edit.current_index);
		*ptr = ret;
		singleton()->edit.current_index--;
	}
	else
		*ptr = ft_strdup(*ptr);
	ft_dprintf(STDIN_FILENO, "%s", *ptr);
	ft_dprintf(STDIN_FILENO, "%s", tgoto(go, 0,
		ft_strlen(singleton()->cwd_basename) + PROMPT_CPADDING + \
		singleton()->edit.current_index));
	ft_memdel((void **)&tmp_ptr);
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
	ft_memdel((void **)ptr);
	ft_exit_for_prompt();
}
